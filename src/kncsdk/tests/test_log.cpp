#include "dbg/dbg.hpp"

#include <doctest/doctest.h>

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

std::string ReadLog()
{
    std::ifstream in( dbg::logfile::GetLogFileName(), std::ios::binary );
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool LogStatFor( const wchar_t* func, int& count )
{
    std::map< unsigned int, dbg::logfile::KLogStat > stats;
    dbg::cerr.GetLogStat( stats );
    for( const auto& [key, stat] : stats )
        if( stat.m_wstrFunctionName.find( func ) != std::wstring::npos )
        {
            count = stat.m_iCount;
            return true;
        }
    return false;
}

void LogSomething( int value )
{
    START_LOG( cerr, L"value " << value << L" 한글" )
        << BUILD_LOG( value )
        << END_LOG;
}

} // namespace

TEST_CASE("START_LOG writes UTF-8 lines with function, level and data")
{
    dbg::logfile::SetLogLevel( 2 );
    LogSomething( 42 );
    const std::string log = ReadLog();
    CHECK(log.find( "cerr : " ) != std::string::npos);
    CHECK(log.find( "LogSomething(), " ) != std::string::npos);
    CHECK(log.find( "value 42 \xED\x95\x9C\xEA\xB8\x80" ) != std::string::npos);
    CHECK(log.find( "    value : 42" ) != std::string::npos);

    int count = 0;
    REQUIRE(LogStatFor( L"LogSomething", count ));
    CHECK(count >= 1);
}

TEST_CASE("log level filters clog")
{
    dbg::logfile::SetLogLevel( 1 );
    START_LOG( clog, L"hidden-marker" );
    dbg::logfile::SetLogLevel( 2 );
    START_LOG( clog, L"visible-marker" );
    const std::string log = ReadLog();
    CHECK(log.find( "hidden-marker" ) == std::string::npos);
    CHECK(log.find( "visible-marker" ) != std::string::npos);
}

TEST_CASE("lines from several threads are not interleaved")
{
    std::vector<std::thread> threads;
    for( int t = 0; t < 4; ++t )
        threads.emplace_back( [t] {
            for( int i = 0; i < 50; ++i )
                dbg::cout << L"thread-" << t << L"-line-" << i << L"-end" << dbg::endl;
        } );
    for( auto& th : threads )
        th.join();

    std::istringstream lines( ReadLog() );
    std::string line;
    int seen = 0;
    while( std::getline( lines, line ) )
    {
        if( line.rfind( "thread-", 0 ) != 0 )
            continue;
        ++seen;
        CHECK(line.size() >= 4);
        CHECK(line.substr( line.size() - 4 ) == "-end");
        CHECK(line.find( "thread-", 1 ) == std::string::npos);
    }
    CHECK(seen == 200);
}

TEST_CASE("JIF / LIF / cassert")
{
    bool reached = false;
    auto f = [&]( bool ok ) -> bool {
        _JIF( ok, return false );
        reached = true;
        return true;
    };
    CHECK_FALSE(f( false ));
    CHECK_FALSE(reached);
    CHECK(f( true ));
    LIF( 1 == 2 );
    cassert( 1 + 1 == 3 )( 1 + 1 ).msg( L"assert message" );
    CHECK(ReadLog().find( "Assert failure" ) != std::string::npos);
}

TEST_CASE("the file keeps the original text of a record")
{
    dbg::logfile::SetLogLevel( 2 );
    const int iUnitUID = 7;
    START_LOG( cwarn, L"file-format-marker" )
        << BUILD_LOG( iUnitUID )
        << END_LOG;
    const std::string log = ReadLog();
    const size_t pos = log.find( "cwarn : " );
    REQUIRE(pos != std::string::npos);
    const size_t marker = log.find( "file-format-marker\n    iUnitUID : 7\n    (", pos );
    CHECK(marker != std::string::npos);
    CHECK(log.find( "test_log.cpp, ", marker ) != std::string::npos);

    // a statement without endl still ends its line
    dbg::cout << L"no-endl-marker";
    dbg::cout << L"next-marker" << dbg::endl;
    CHECK(ReadLog().find( "no-endl-marker\nnext-marker\n" ) != std::string::npos);

    // _JIF keeps "func(), in expr    (file, line)"
    LIF( iUnitUID == 8 );
    CHECK(ReadLog().find( "(), in iUnitUID == 8    (" ) != std::string::npos);
}

TEST_CASE("console rendering is compact: time, level, function, message, aligned fields")
{
    dbg::entry e;
    e.mode = dbg::lm_error;
    e.time = CTime( 2026, 10, 31, 12, 34, 56 );
    e.Header( { L"cerr", L"OnLogin", true } );
    e.Append( L"user not found" );
    e.EndLine();
    e.Field( L"iUnitUID" );
    e.Append( L"123" );
    e.EndLine();
    e.Field( L"wstrNickName" );
    e.Append( L"abc" );
    e.EndLine();
    e.Append( L"    reason : timeout\n" );
    e.Location( L"/src/User.cpp", 10 );
    e.Break();

    CHECK(dbg::logfile::RenderConsole( e, false ) ==
          "12:34:56 ERROR OnLogin: user not found\n"
          "               iUnitUID     = 123\n"
          "               wstrNickName = abc\n"
          "               reason : timeout\n");

    const std::string colored = dbg::logfile::RenderConsole( e, true );
    CHECK(colored.find( "\x1b[1;31mERROR" ) != std::string::npos);

    dbg::entry blank;
    blank.time = e.time;
    blank.EndLine();
    blank.Break();
    CHECK(dbg::logfile::RenderConsole( blank, false ).empty());

    dbg::entry raw;
    raw.time = e.time;
    raw.Append( L"  dump line 1\n\tdump line 2\n" );
    CHECK(dbg::logfile::RenderConsole( raw, false ) ==
          "12:34:56 NOTE    dump line 1\n"
          "               dump line 2\n");
}

TEST_CASE("console level is independent of the file level")
{
    std::string console;
    dbg::logfile::SetConsoleWriter( [&console]( std::string_view text ) { console += text; }, false );
    dbg::logfile::SetLogLevel( 2 );
    dbg::logfile::SetConsoleLogLevel( 0 );
    START_LOG( clog, L"file-only-marker" );
    START_LOG( cerr, L"both-marker" );
    CHECK(console.find( "file-only-marker" ) == std::string::npos);
    CHECK(console.find( "both-marker" ) != std::string::npos);
    CHECK(ReadLog().find( "file-only-marker" ) != std::string::npos);

    dbg::logfile::SetLogLevel( 0 );
    dbg::logfile::SetConsoleLogLevel( 2 );
    START_LOG( clog, L"console-only-marker" );
    CHECK(console.find( "console-only-marker" ) != std::string::npos);
    CHECK(ReadLog().find( "console-only-marker" ) == std::string::npos);

    dbg::logfile::SetConsoleLogLevel( -1 );
    CHECK(dbg::logfile::GetConsoleLogLevel() == 0);
    dbg::logfile::SetLogLevel( 2 );
    dbg::logfile::SetConsoleWriter( {}, false );
}
