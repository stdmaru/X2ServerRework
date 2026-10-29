// lua_tinker on LuaJIT 2.1: the binding patterns used by the server scripts (class_def ~800 uses).
#include "KncLua.h"
#include "lua_tinker.h"

#include <doctest/doctest.h>

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
