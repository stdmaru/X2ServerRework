// KWorldLineMaps follows the client's chain of scripts to each world's line map (decision T53): village -> world ->
// world script -> line map, sharing a line map that several worlds name, and taking squares by world name.
#include "LineMap/WorldLineMaps.h"
#include "LineMap/XSLLineMap.h"

#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace
{

void WriteFile( const fs::path& path, const char* szText )
{
	std::ofstream( path ) << szText;
}

}	// namespace

TEST_CASE( "KWorldLineMaps loads the line maps of villages and squares" )
{
	const fs::path root = fs::temp_directory_path() / "x2_world_line_maps_test";
	fs::remove_all( root );
	fs::create_directories( root );

	WriteFile( root / "Enum.lua", R"(
VILLAGE_MAP_ID = { VMI_RUBEN = 20000, VMI_LOUNGE = 30000, VMI_GATE = 10000 }
WORLD_ID = { WI_VILLAGE_RUBEN = 258, WI_LOUNGE = 280, WI_GATE = 259, WI_SQUARE = 1, WI_DUNGEON = 900 }
LINE_TYPE = { LT_NORMAL = 0, LT_WALL = 1 }
)" );
	WriteFile( root / "DungeonEnum.lua", "DUNGEON_ID = { DI_TEST = 1 }\n" );
	// STR_ID_1 is not defined: the village list reads strings the server does not have
	WriteFile( root / "DLG_Map_Village.lua", R"(
Village[VILLAGE_MAP_ID.VMI_RUBEN] = { NAME = STR_ID_1, WORLD_ID = WORLD_ID.WI_VILLAGE_RUBEN, DUNGEON = { DUNGEON_ID.DI_TEST } }
Village[VILLAGE_MAP_ID.VMI_LOUNGE] = { WORLD_ID = WORLD_ID.WI_LOUNGE }
Village[VILLAGE_MAP_ID.VMI_GATE] = { WORLD_ID = WORLD_ID.WI_GATE }
)" );
	WriteFile( root / "WorldData.lua", R"(
g_pWorldManager:AddWorldData({ worldName = "WI_VILLAGE_RUBEN", dataFileName = "ruben_village.lua", worldID = WORLD_ID.WI_VILLAGE_RUBEN })
g_pWorldManager:AddWorldData({ worldName = "WI_LOUNGE", dataFileName = "Ruben_Village.lua", worldID = WORLD_ID.WI_LOUNGE })
g_pWorldManager:AddWorldData({ worldName = "WI_GATE", dataFileName = "Missing_Gate.lua", worldID = WORLD_ID.WI_GATE })
g_pWorldManager:AddWorldData({ worldName = "WI_SQUARE", dataFileName = "square.lua", worldID = WORLD_ID.WI_SQUARE })
g_pWorldManager:AddWorldData({ worldName = "WI_DUNGEON", dataFileName = "dungeon.lua", worldID = WORLD_ID.WI_DUNGEON })
)" );
	// a world script builds meshes and lights the server does not have
	WriteFile( root / "Ruben_Village.lua", R"(
g_pWorld:SetBGM( "ruben.ogg" )
g_pWorld:SetLineMap( "ruben_village_LineMap.lua" )
local mesh = g_pWorld:CreateObjectMesh()
mesh:SetPos( D3DXVECTOR3( 0, 0, 0 ) )
)" );
	WriteFile( root / "ruben_village_LineMap.lua", R"(
g_pLineMap:SetLandHeight( 0 )
g_pLineMap:SetTeamStartPos( true, D3DXVECTOR3( 0, 100, 0 ), false, 0 )
g_pLineMap:AddStartPos( 1, D3DXVECTOR3( 50, 100, 0 ), true, 0 )
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 0, 100, 0 ), END_POS = D3DXVECTOR3( 500, 100, 0 ) })
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 500, 100, 0 ), END_POS = D3DXVECTOR3( 900, 300, 0 ) })
)" );
	WriteFile( root / "square.lua", "g_pWorld:SetLineMap( \"Square_LineMap.lua\" )\n" );
	WriteFile( root / "Square_LineMap.lua", R"(
g_pLineMap:AddLine_LUA({ LINE_TYPE = LINE_TYPE.LT_NORMAL, START_POS = D3DXVECTOR3( 0, 0, 0 ), END_POS = D3DXVECTOR3( 100, 0, 0 ) })
)" );
	WriteFile( root / "dungeon.lua", "g_pWorld:SetLineMap( \"Square_LineMap.lua\" )\n" );

	// the working folder and its subfolders are searched first: the fixture's, so that no other script is found
	const fs::path oldCurrent = fs::current_path();
	fs::current_path( root );

	KWorldLineMaps kLineMaps;
	const bool bLoaded = kLineMaps.Load();
	fs::current_path( oldCurrent );
	REQUIRE( bLoaded );

	CHECK( kLineMaps.GetWorldIDOfVillage( 20000 ) == 258 );
	CHECK( kLineMaps.GetWorldIDOfVillage( 12345 ) == 0 );

	const CXSLLineMap* pRuben = kLineMaps.GetByVillage( 20000 );
	REQUIRE( pRuben != nullptr );
	CHECK( pRuben->GetNumLineData() == 2 );
	CHECK( pRuben->GetLineData( 1 )->endPos.y == doctest::Approx( 300.f ) );

	// the lounge's world names the same world script (another letter case): one line map for both
	CHECK( kLineMaps.GetByVillage( 30000 ) == pRuben );

	// a world with no script has no line map; a world outside villages and rooms is not loaded
	CHECK( kLineMaps.GetByVillage( 10000 ) == nullptr );
	CHECK( kLineMaps.GetByWorld( 900 ) == nullptr );

	const CXSLLineMap* pSquare = kLineMaps.GetByWorld( 1 );
	REQUIRE( pSquare != nullptr );
	CHECK( pSquare->GetNumLineData() == 1 );
	CHECK( kLineMaps.GetByWorldName( "WI_SQUARE" ) == pSquare );
	CHECK( kLineMaps.GetByWorldName( "WI_SQUARE_MARKET" ) == nullptr );	// not in this WorldData
	CHECK( kLineMaps.GetByWorldName( "WI_VILLAGE_RUBEN" ) == nullptr );	// villages go by map ID

	CHECK( kLineMaps.GetNumWorlds() == 3 );
	CHECK( kLineMaps.GetNumLineMaps() == 2 );

	fs::remove_all( root );
}
