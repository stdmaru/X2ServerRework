// lua_tinker on LuaJIT 2.1: the binding patterns used by the server scripts (class_def ~800 uses).
#include "KncLua.h"
#include "lua_tinker.h"

#include <doctest/doctest.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {

int Add( int a, int b ) { return a + b; }

int g_iVoidCalls = 0;
void Touch() { ++g_iVoidCalls; }

class KItemTable
{
public:
    KItemTable() = default;

    void AddItem( int iItemID, float fRate ) { m_iLastID = iItemID; m_fLastRate = fRate; ++m_iCount; }
    int  GetCount() const { return m_iCount; }
    const char* GetName() { return "item-table"; }

    int     m_iLastID = 0;
    float   m_fLastRate = 0.f;
    int     m_iCount = 0;
};

struct LuaFixture
{
    lua_State* L;
    LuaFixture() : L( luaL_newstate() )
    {
        luaL_openlibs( L );
        lua_tinker::init( L );
    }
    ~LuaFixture() { lua_close( L ); }
};

} // namespace

TEST_CASE_FIXTURE( LuaFixture, "free functions, globals and calls" )
{
    lua_tinker::def( L, "Add", Add );
    lua_tinker::def( L, "Touch", Touch );
    lua_tinker::dostring( L, "result = Add( 2, 40 ) Touch() Touch()" );
    CHECK( lua_tinker::get<int>( L, "result" ) == 42 );
    CHECK( g_iVoidCalls == 2 );

    lua_tinker::dostring( L, "function Twice( x ) return x * 2 end" );
    CHECK( lua_tinker::call<int>( L, "Twice", 21 ) == 42 );

    lua_tinker::set( L, "fromCpp", 7 );
    lua_tinker::dostring( L, "fromLua = fromCpp + 1" );
    CHECK( lua_tinker::get<int>( L, "fromLua" ) == 8 );
}

TEST_CASE_FIXTURE( LuaFixture, "class registration like the server tables" )
{
    KItemTable table;
    lua_tinker::class_add<KItemTable>( L, "KItemTable" );
    lua_tinker::class_def<KItemTable>( L, "AddItem", &KItemTable::AddItem );
    lua_tinker::class_def<KItemTable>( L, "GetCount", &KItemTable::GetCount );
    lua_tinker::class_def<KItemTable>( L, "GetName", &KItemTable::GetName );
    lua_tinker::class_mem<KItemTable>( L, "m_iLastID", &KItemTable::m_iLastID );
    lua_tinker::decl( L, "g_pItemTable", &table );

    lua_tinker::dostring( L,
        "g_pItemTable:AddItem( 100, 0.5 )\n"
        "g_pItemTable:AddItem( 200, 1.5 )\n"
        "count = g_pItemTable:GetCount()\n"
        "name = g_pItemTable:GetName()\n"
        "last = g_pItemTable.m_iLastID\n" );

    CHECK( table.m_iCount == 2 );
    CHECK( table.m_iLastID == 200 );
    CHECK( table.m_fLastRate == doctest::Approx( 1.5f ) );
    CHECK( lua_tinker::get<int>( L, "count" ) == 2 );
    CHECK( std::string( lua_tinker::get<const char*>( L, "name" ) ) == "item-table" );
    CHECK( lua_tinker::get<int>( L, "last" ) == 200 );
}

TEST_CASE_FIXTURE( LuaFixture, "64-bit integers keep their value" )
{
    lua_tinker::set( L, "big", static_cast<long long>( 9000000000LL ) );
    CHECK( lua_tinker::get<long long>( L, "big" ) == 9000000000LL );
}

TEST_CASE( "lua_interpreter runs statements read from stdin" )
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );

    std::istringstream input( "x = 41 + 1\nthis is not lua\ny = x * 2\n" );
    std::streambuf* old = std::cin.rdbuf( input.rdbuf() );
    lua_interpreter( L );
    std::cin.rdbuf( old );

    CHECK( lua_tinker::get<int>( L, "x" ) == 42 );
    CHECK( lua_tinker::get<int>( L, "y" ) == 84 ); // a bad line does not stop the prompt
    lua_close( L );
}

namespace {

// A data script like Item.lua: one table call per entry (one constant each), past LuaJIT's 65536 constants.
std::string WriteBigScript( const char* path, int entries, const char* tail )
{
    std::ofstream out( path, std::ios::binary );
    out << "\xEF\xBB\xBF-- lua header\n\n";
    for( int i = 0; i < entries; ++i )
        out << "Add\n{\n\tm_ID = " << i << ",\n\tm_Name = \"name " << i << "\",\n\tm_Desc = \"desc " << i << "\",\n}\n";
    out << tail;
    return path;
}

int g_iEntries = 0;
int g_iLastID = -1;
int AddEntry( lua_State* L )
{
    lua_getfield( L, 1, "m_ID" );
    const int id = static_cast<int>( lua_tointeger( L, -1 ) );
    if( id == g_iLastID + 1 )
        ++g_iEntries;
    g_iLastID = id;
    return 0;
}

} // namespace

TEST_CASE( "lua_dofile runs scripts past the constant limit in order" )
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    lua_register( L, "Add", AddEntry );

    const std::string path = WriteBigScript( "big_script.lua", 70000, "done = true\n" );
    REQUIRE( luaL_loadfile( L, path.c_str() ) != 0 ); // the limit is real
    lua_pop( L, 1 );

    g_iEntries = 0;
    g_iLastID = -1;
    const int top = lua_gettop( L );
    CHECK( LUA_DOFILE( L, path.c_str() ) == 0 );
    CHECK( lua_gettop( L ) == top );
    CHECK( g_iEntries == 70000 );
    CHECK( lua_tinker::get<bool>( L, "done" ) );
    lua_close( L );
}

TEST_CASE( "lua_dofile reports errors with the line of the file" )
{
    lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    lua_register( L, "Add", AddEntry );

    // 70000 entries of 6 lines after 2 header lines: the error is on line 420003
    const std::string path = WriteBigScript( "big_script_error.lua", 70000, "error( \"stop\" )\n" );
    REQUIRE( LUA_DOFILE( L, path.c_str() ) != 0 );
    const std::string msg = lua_tostring( L, -1 );
    CHECK( msg.find( "big_script_error.lua:420003: stop" ) != std::string::npos );
    lua_close( L );

    // a syntax error anywhere: nothing runs
    L = luaL_newstate();
    luaL_openlibs( L );
    lua_register( L, "Add", AddEntry );
    g_iEntries = 0;
    g_iLastID = -1;
    WriteBigScript( "big_script_syntax.lua", 70000, "this is not lua\n" );
    CHECK( LUA_DOFILE( L, "big_script_syntax.lua" ) != 0 );
    CHECK( g_iEntries == 0 );
    lua_close( L );
}
