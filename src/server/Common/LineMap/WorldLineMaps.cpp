#include "BaseServer.h"
#include "LineMap/WorldLineMaps.h"
#include "LineMap/XSLLineMap.h"

#include "KAutoPath.h"

#include <KncLua.h>
#include <lua_tinker.h>

#include <algorithm>
#include <cctype>
#include <vector>

ImplementSingleton( KWorldLineMaps );

namespace
{

// Owns a Lua state with the standard libraries.
class LuaState
{
public:
	LuaState() : m_pState( luaL_newstate() ) { luaL_openlibs( m_pState ); }
	~LuaState() { lua_close( m_pState ); }
	LuaState( const LuaState& ) = delete;
	LuaState& operator=( const LuaState& ) = delete;

	lua_State* Get() const { return m_pState; }

private:
	lua_State* m_pState;
};

std::string FindScript( const std::string& strName )
{
	std::string strPath = strName;
	KAutoPath kAutoPath;
	return kAutoPath.GetPullPath( strPath ) ? strPath : std::string();
}

std::string ToLower( std::string str )
{
	std::transform( str.begin(), str.end(), str.begin(), []( unsigned char c ) { return static_cast<char>( std::tolower( c ) ); } );
	return str;
}

// Runs a script found by name. bRequired: an error is logged as one, otherwise as a warning.
bool RunScript( lua_State* L, const std::string& strName, bool bRequired )
{
	const std::string strPath = FindScript( strName );
	if( strPath.empty() )
	{
		if( bRequired )
		{
			START_LOG( cerr, L"Script not found." )
				<< BUILD_LOG( strName )
				<< END_LOG;
		}
		else
		{
			START_LOG( cwarn, L"Script not found." )
				<< BUILD_LOG( strName )
				<< END_LOG;
		}
		return false;
	}

	if( luaL_dofile( L, strPath.c_str() ) != 0 )
	{
		START_LOG( cerr, L"Failed to run a script." )
			<< BUILD_LOG( strName )
			<< BUILD_LOG( lua_tostring( L, -1 ) )
			<< END_LOG;
		lua_pop( L, 1 );
		return false;
	}
	return true;
}

// What the client's map scripts need besides Enum.lua: DLG_Map_Village.lua fills Village[], WorldData.lua calls
// g_pWorldManager:AddWorldData.
const char* const kMapScriptsPrelude = R"(
Village = {}
x2_worlds = {}
g_pWorldManager = {
	AddWorldData = function( self, t )
		x2_worlds[ #x2_worlds + 1 ] = { id = t.worldID, name = t.worldName, file = t.dataFileName }
	end,
}
)";

// A world script builds the world's meshes, sounds, lights...: everything but SetLineMap does nothing, and any
// global it reads that the server does not have (strings, other enums) is a value that does nothing either.
const char* const kWorldScriptsPrelude = R"(
local any
any = setmetatable( {}, { __index = function() return any end, __call = function() return any end } )
g_pWorld = setmetatable( {
	SetLineMap = function( self, name ) x2_lineMapFile = name end,
}, { __index = function() return function() return any end end } )
setmetatable( _G, { __index = function() return any end } )
)";

bool RunChunk( lua_State* L, const char* szChunk )
{
	if( luaL_dostring( L, szChunk ) != 0 )
	{
		START_LOG( cerr, L"Failed to run a Lua chunk." )
			<< BUILD_LOG( lua_tostring( L, -1 ) )
			<< END_LOG;
		lua_pop( L, 1 );
		return false;
	}
	return true;
}

std::string GetGlobalString( lua_State* L, const char* szName )
{
	lua_getglobal( L, szName );
	std::string str = lua_type( L, -1 ) == LUA_TSTRING ? lua_tostring( L, -1 ) : "";
	lua_pop( L, 1 );
	return str;
}

struct WorldScript
{
	int			iWorldID;
	std::string	strName;
	std::string	strFile;
};

// A world whose line map the servers need outside the village list.
bool IsRoomWorld( const std::string& strWorldName )
{
	const std::string kWedding = "_WEDDING";
	return strWorldName == "WI_SQUARE" || strWorldName == "WI_SQUARE_MARKET"
		|| ( strWorldName.size() > kWedding.size()
			 && strWorldName.compare( strWorldName.size() - kWedding.size(), kWedding.size(), kWedding ) == 0 );
}

std::shared_ptr< CXSLLineMap > LoadLineMapScript( const std::string& strName )
{
	const std::string strPath = FindScript( strName );
	if( strPath.empty() )
	{
		START_LOG( cwarn, L"Line map script not found." )
			<< BUILD_LOG( strName )
			<< END_LOG;
		return nullptr;
	}

	LuaState kLua;
	lua_State* L = kLua.Get();
	if( !RunScript( L, "Enum.lua", true ) )
		return nullptr;

	lua_tinker::class_add<D3DXVECTOR3>( L, "D3DXVECTOR3" );
	lua_tinker::class_con<D3DXVECTOR3>( L, lua_tinker::constructor<float, float, float>() );
	lua_tinker::class_mem<D3DXVECTOR3>( L, "x", &D3DXVECTOR3::x );
	lua_tinker::class_mem<D3DXVECTOR3>( L, "y", &D3DXVECTOR3::y );
	lua_tinker::class_mem<D3DXVECTOR3>( L, "z", &D3DXVECTOR3::z );

	std::shared_ptr< CXSLLineMap > spLineMap( CXSLLineMap::CreateLineMap() );
	if( !spLineMap->LoadLineMap( L, strPath.c_str() ) )
		return nullptr;
	return spLineMap;
}

}	// namespace

KWorldLineMaps::KWorldLineMaps() = default;
KWorldLineMaps::~KWorldLineMaps() = default;

bool KWorldLineMaps::Load()
{
	m_mapVillageWorld.clear();
	m_mapWorldLineMap.clear();

	LuaState kLua;
	lua_State* L = kLua.Get();
	if( !RunScript( L, "Enum.lua", true ) || !RunScript( L, "DungeonEnum.lua", true )
		|| !RunChunk( L, kMapScriptsPrelude )
		|| !RunScript( L, "DLG_Map_Village.lua", true ) || !RunScript( L, "WorldData.lua", true ) )
	{
		return false;
	}

	// Village[VILLAGE_MAP_ID] = { WORLD_ID = ..., ... }
	lua_getglobal( L, "Village" );
	lua_pushnil( L );
	while( lua_next( L, -2 ) != 0 )
	{
		if( lua_type( L, -2 ) == LUA_TNUMBER && lua_istable( L, -1 ) )
		{
			lua_getfield( L, -1, "WORLD_ID" );	// Village, key, value, WORLD_ID
			if( lua_type( L, -1 ) == LUA_TNUMBER )
				m_mapVillageWorld[ static_cast<int>( lua_tointeger( L, -3 ) ) ] = static_cast<int>( lua_tointeger( L, -1 ) );
			lua_pop( L, 1 );
		}
		lua_pop( L, 1 );
	}
	lua_pop( L, 1 );

	std::vector< WorldScript > vecWorld;
	lua_getglobal( L, "x2_worlds" );
	const int iNumWorlds = static_cast<int>( lua_objlen( L, -1 ) );
	for( int i = 1; i <= iNumWorlds; ++i )
	{
		lua_rawgeti( L, -1, i );
		lua_getfield( L, -1, "id" );
		lua_getfield( L, -2, "name" );
		lua_getfield( L, -3, "file" );
		if( lua_type( L, -3 ) == LUA_TNUMBER && lua_type( L, -1 ) == LUA_TSTRING )
		{
			vecWorld.push_back( { static_cast<int>( lua_tointeger( L, -3 ) ),
								  lua_type( L, -2 ) == LUA_TSTRING ? lua_tostring( L, -2 ) : "",
								  lua_tostring( L, -1 ) } );
		}
		lua_pop( L, 4 );
	}
	lua_pop( L, 1 );

	if( !RunChunk( L, kWorldScriptsPrelude ) )
		return false;

	std::map< int, bool > mapVillageWorlds;
	for( const auto& kVillage : m_mapVillageWorld )
		mapVillageWorlds[ kVillage.second ] = true;

	std::map< std::string, std::shared_ptr< CXSLLineMap > > mapByFile;	// lower-case file name -> line map
	for( const WorldScript& kWorld : vecWorld )
	{
		if( mapVillageWorlds.count( kWorld.iWorldID ) == 0 && !IsRoomWorld( kWorld.strName ) )
			continue;

		lua_pushnil( L );
		lua_setglobal( L, "x2_lineMapFile" );
		if( !RunScript( L, kWorld.strFile, false ) )
			continue;

		const std::string strLineMap = GetGlobalString( L, "x2_lineMapFile" );
		if( strLineMap.empty() )
		{
			START_LOG( cwarn, L"World script has no line map." )
				<< BUILD_LOG( kWorld.strFile )
				<< END_LOG;
			continue;
		}

		std::shared_ptr< CXSLLineMap >& spLineMap = mapByFile[ ToLower( strLineMap ) ];
		if( !spLineMap )
			spLineMap = LoadLineMapScript( strLineMap );
		if( spLineMap )
			m_mapWorldLineMap[ kWorld.iWorldID ] = spLineMap;
	}

	START_LOG( cout, L"World line maps loaded." )
		<< BUILD_LOG( m_mapVillageWorld.size() )
		<< BUILD_LOG( m_mapWorldLineMap.size() )
		<< BUILD_LOG( GetNumLineMaps() )
		<< END_LOG;
	return true;
}

const CXSLLineMap* KWorldLineMaps::GetByVillage( int iVillageMapID ) const
{
	return GetByWorld( GetWorldIDOfVillage( iVillageMapID ) );
}

const CXSLLineMap* KWorldLineMaps::GetByWorld( int iWorldID ) const
{
	const auto mit = m_mapWorldLineMap.find( iWorldID );
	return mit != m_mapWorldLineMap.end() ? mit->second.get() : nullptr;
}

int KWorldLineMaps::GetWorldIDOfVillage( int iVillageMapID ) const
{
	const auto mit = m_mapVillageWorld.find( iVillageMapID );
	return mit != m_mapVillageWorld.end() ? mit->second : 0;
}

size_t KWorldLineMaps::GetNumLineMaps() const
{
	std::vector< const CXSLLineMap* > vecLineMap;
	for( const auto& kWorld : m_mapWorldLineMap )
		vecLineMap.push_back( kWorld.second.get() );
	std::sort( vecLineMap.begin(), vecLineMap.end() );
	return static_cast<size_t>( std::unique( vecLineMap.begin(), vecLineMap.end() ) - vecLineMap.begin() );
}
