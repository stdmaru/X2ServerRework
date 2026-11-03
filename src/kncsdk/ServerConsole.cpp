#include "ServerConsole.h"

#include "dbg/dbg.hpp"
#include "x2/platform/control.h"
#include "x2/platform/json.h"
#include "x2/platform/sync.h"
#include "x2/platform/terminal.h"

#include <atomic>
#include <cstdio>
#include <deque>
#include <mutex>
#include <optional>
#include <string>

using x2::platform::JsonString;

namespace {

// Like lua.c: an expression line prints its values ("1+1" -> 2), anything else runs as a statement.
int Load( lua_State* L, const std::string& line )
{
    const std::string expr = "return " + line;
    if( luaL_loadbuffer( L, expr.data(), expr.size(), "=stdin" ) == 0 )
        return 0;
    lua_pop( L, 1 );
    return luaL_loadbuffer( L, line.data(), line.size(), "=stdin" );
}

// print() of a protocol command (or the terminal prompt): like Lua's, into output.
int CapturePrint( lua_State* L )
{
    std::string& out = *static_cast<std::string*>( lua_touserdata( L, lua_upvalueindex( 1 ) ) );
    const int n = lua_gettop( L );
    lua_getglobal( L, "tostring" );
    for( int i = 1; i <= n; ++i )
    {
        lua_pushvalue( L, -1 );
        lua_pushvalue( L, i );
        lua_call( L, 1, 1 );
        size_t len = 0;
        const char* s = lua_tolstring( L, -1, &len );
        if( !s )
            return luaL_error( L, "'tostring' must return a string to 'print'" );
        if( i > 1 )
            out += '\t';
        out.append( s, len );
        lua_pop( L, 1 );
    }
    out += '\n';
    return 0;
}

// Runs one command; returns the error message, empty on success. Results go through Lua's print.
std::string Execute( lua_State* L, const std::string& line )
{
    std::string error;
    const int base = lua_gettop( L );
    int status = Load( L, line );
    if( status == 0 )
        status = lua_pcall( L, 0, LUA_MULTRET, 0 );
    if( status != 0 )
    {
        const char* msg = lua_tostring( L, -1 );
        error = msg ? msg : "(error object is not a string)";
    }
    else if( lua_gettop( L ) > base )
    {
        lua_getglobal( L, "print" );
        lua_insert( L, base + 1 );
        if( lua_pcall( L, lua_gettop( L ) - base - 1, 0, 0 ) != 0 )
        {
            const char* msg = lua_tostring( L, -1 );
            error = msg ? msg : "(error object is not a string)";
        }
    }
    lua_settop( L, base );
    std::fflush( stdout );
    return error;
}

// Captures print() instead of relying on the process-wide stdout redirection.
std::string ExecuteCapturingPrint( lua_State* L, const std::string& line, std::string& output )
{
    lua_getglobal( L, "print" );
    const int savedPrint = luaL_ref( L, LUA_REGISTRYINDEX );
    lua_pushlightuserdata( L, &output );
    lua_pushcclosure( L, CapturePrint, 1 );
    lua_setglobal( L, "print" );
    const std::string error = Execute( L, line );
    lua_rawgeti( L, LUA_REGISTRYINDEX, savedPrint );
    lua_setglobal( L, "print" );
    luaL_unref( L, LUA_REGISTRYINDEX, savedPrint );
    return error;
}

struct Command
{
    int         client = -1;        // -1: stdin (machine mode)
    std::string id = "null";        // JSON value, echoed in the result
    std::string lua;
    bool        stop = false;
};

// {"id":1,"lua":"..."}, {"id":1,"stop":true}, or a bare line of Lua
std::optional<Command> ParseCommand( int client, const std::string& line )
{
    Command command;
    command.client = client;
    const auto object = x2::platform::ParseJsonObject( line );
    if( !object )
    {
        if( line.empty() || line[0] == '{' )
            return std::nullopt;    // not a JSON object: an error reply would have no id either
        command.lua = line;
        return command;
    }
    const auto id = object->find( "id" );
    if( id != object->end() )
        command.id = id->second.kind == x2::platform::JsonValue::String ? JsonString( id->second.text ) : id->second.text;
    const auto stop = object->find( "stop" );
    command.stop = stop != object->end() && stop->second.text == "true";
    const auto lua = object->find( "lua" );
    if( lua != object->end() )
        command.lua = lua->second.text;
    return command;
}

} // namespace

struct KServerConsole::Impl
{
    Options                                 options;
    std::optional<x2::platform::Terminal>   terminal;
    x2::platform::ControlServer             control;
    bool                                    interactive = false;
    bool                                    sinkSet = false;

    std::mutex                              mutex;
    std::deque<Command>                     commands;   // from the control clients
    std::atomic<bool>                       ready{ false };

    const char* State() const { return ready ? "ready" : "loading"; }

    std::string Hello() const
    {
        return std::string( "{\"type\":\"hello\",\"protocol\":1,\"server\":" ) + JsonString( options.name )
            + ",\"pid\":" + std::to_string( ::GetCurrentProcessId() ) + ",\"state\":\"" + State() + "\"}";
    }

    // To the machine stdout and every control client
    void Emit( const std::string& json )
    {
        if( options.machine && terminal )
            terminal->Write( json + "\n" );
        control.Broadcast( json );
    }

    void Reply( int client, const std::string& json )
    {
        if( client < 0 )
        {
            if( terminal )
                terminal->Write( json + "\n" );
        }
        else
        {
            control.Send( client, json );
        }
    }

    // stdout/stderr written outside the log (std::cout, printf), by lines
    void OnOutput( std::string_view text )
    {
        if( interactive )
            terminal->Write( text );
        while( !text.empty() )
        {
            const size_t end = text.find( '\n' );
            std::string_view line = text.substr( 0, end );
            if( !line.empty() && line.back() == '\r' )
                line.remove_suffix( 1 );
            if( !line.empty() )
                Emit( "{\"type\":\"output\",\"text\":" + JsonString( line ) + "}" );
            if( end == std::string_view::npos )
                break;
            text.remove_prefix( end + 1 );
        }
    }

    void RunCommand( lua_State* L, const Command& command )
    {
        if( command.stop )
        {
            Reply( command.client, "{\"type\":\"result\",\"id\":" + command.id + ",\"ok\":true,\"output\":\"\",\"error\":\"\"}" );
            x2::platform::Terminal::RequestStop();
            return;
        }
        // print goes to the reply instead of stdout while the command runs
        std::string output;
        const std::string error = ExecuteCapturingPrint( L, command.lua, output );

        Reply( command.client, "{\"type\":\"result\",\"id\":" + command.id + ",\"ok\":" + ( error.empty() ? "true" : "false" )
                               + ",\"output\":" + JsonString( output ) + ",\"error\":" + JsonString( error ) + "}" );
    }

    // Every ~100 ms on the thread that owns the Lua state
    void Poll( lua_State* L, const std::function<bool()>& isReady )
    {
        if( !ready && isReady && isReady() )
        {
            ready = true;
            if( interactive && !options.name.empty() )
                terminal->SetTitle( options.name + "_Ready" );
            Emit( "{\"type\":\"status\",\"state\":\"ready\"}" );
        }
        for( ;; )
        {
            Command command;
            {
                std::lock_guard lock( mutex );
                if( commands.empty() )
                    break;
                command = std::move( commands.front() );
                commands.pop_front();
            }
            RunCommand( L, command );
        }
    }
};

KServerConsole::KServerConsole( Options options )
    : m_impl( std::make_unique<Impl>() )
{
    Impl& d = *m_impl;
    d.options = std::move( options );

    if( !d.options.noStdin )
    {
        x2::platform::Terminal::Options terminalOptions;
        terminalOptions.machine = d.options.machine;
        d.terminal.emplace( terminalOptions );
        d.interactive = d.terminal->Interactive();
    }
    if( d.interactive )
    {
        // The log goes above the prompt instead of straight to stdout
        dbg::logfile::SetConsoleWriter( [&d]( std::string_view text ) { d.terminal->Write( text ); }, true );
        if( !d.options.name.empty() )
            d.terminal->SetTitle( d.options.name + "_Loading" );
    }
    if( d.options.machine )
    {
        dbg::logfile::SetConsoleWriter( []( std::string_view ) {}, false );   // stdout carries the protocol only
        d.terminal->Write( d.Hello() + "\n" );
    }

    if( !d.options.control.empty() )
    {
        std::string error;
        const bool started = d.control.Start(
            d.options.control, [&d]() { return d.Hello(); },
            [&d]( int client, std::string line ) {
                std::optional<Command> command = ParseCommand( client, line );
                if( !command )
                {
                    d.control.Send( client, "{\"type\":\"result\",\"id\":null,\"ok\":false,\"output\":\"\",\"error\":\"not a command\"}" );
                    return;
                }
                std::lock_guard lock( d.mutex );
                d.commands.push_back( std::move( *command ) );
            },
            error );
        if( started )
        {
            const std::wstring wstrSocket = KncUtil::toWideString( d.control.Path() );
            START_LOG( cout, L"Control socket (x2ctl attach " << KncUtil::toWideString( d.options.control ) << L")" )
                << BUILD_LOG( wstrSocket );
        }
        else
        {
            const std::wstring wstrError = KncUtil::toWideString( error );
            START_LOG( cwarn, L"No control socket." )
                << BUILD_LOG( wstrError );
        }
    }

    if( d.options.machine || d.control.Running() )
    {
        dbg::logfile::SetRecordSink( [&d]( const dbg::entry& e ) { d.Emit( dbg::logfile::RenderJson( e ) ); } );
        d.sinkSet = true;
        if( d.terminal )
            d.terminal->SetOutputHandler( [&d]( std::string_view text ) { d.OnOutput( text ); } );
    }
}

KServerConsole::~KServerConsole()
{
    Impl& d = *m_impl;
    d.Emit( "{\"type\":\"status\",\"state\":\"stopped\"}" );
    if( d.sinkSet )
        dbg::logfile::SetRecordSink( {} );
    if( d.terminal )
        d.terminal->SetOutputHandler( {} );
    d.control.Stop();
    if( d.interactive || d.options.machine )
        dbg::logfile::SetConsoleWriter( {}, false );
    d.terminal.reset();
}

void KServerConsole::Run( lua_State* L, std::function<bool()> ready )
{
    Impl& d = *m_impl;
    auto idle = [&d, L, &ready]() { d.Poll( L, ready ); };

    if( d.options.noStdin )
    {
        START_LOG( cout, L"No stdin (service mode): SIGTERM, SIGINT or SIGHUP shuts the server down." );
        x2::platform::WaitForStop( idle );
    }
    else
    {
        if( d.interactive )
            d.terminal->Write( "Lua prompt: Up/Down for the history, Ctrl+C or Ctrl+D (empty line) to exit.\n" );
        while( const std::optional<std::string> line = d.terminal->ReadLine( idle ) )
        {
            if( line->empty() )
                continue;
            if( d.options.machine )
            {
                if( std::optional<Command> command = ParseCommand( -1, *line ) )
                    d.RunCommand( L, *command );
                else
                    d.Reply( -1, "{\"type\":\"result\",\"id\":null,\"ok\":false,\"output\":\"\",\"error\":\"not a command\"}" );
                continue;
            }
            std::string output;
            const std::string error = d.interactive
                ? ExecuteCapturingPrint( L, *line, output )
                : Execute( L, *line );
            if( !output.empty() && d.interactive )
                d.terminal->Write( output );
            if( error.empty() )
                continue;
            if( d.interactive )
            {
                d.terminal->Write( "\x1b[31m" + error + "\x1b[0m\n" );
            }
            else
            {
                std::fprintf( stderr, "%s\n", error.c_str() );
                std::fflush( stderr );
            }
        }
    }
    d.Emit( "{\"type\":\"status\",\"state\":\"stopping\"}" );
}

int lua_interpreter( lua_State* L )
{
    KServerConsole console( KServerConsole::Options{} );
    console.Run( L, nullptr );
    return 0;
}
