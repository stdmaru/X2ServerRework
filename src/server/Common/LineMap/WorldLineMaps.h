#pragma once

// Line maps of the worlds where players move without P2P (villages, dungeon gates and lounges, battle field rest
// areas, squares, wedding halls), for the servers to validate movement (decision T53). They come from the client's own
// scripts in ClientScript, found by name through KAutoPath, as the client finds them:
//   VILLAGE_MAP_ID -> DLG_Map_Village.lua (WORLD_ID) -> WorldData.lua (dataFileName) -> the world's script
//   (g_pWorld:SetLineMap("...")) -> the line map script (g_pLineMap:AddLine_LUA{...}).
// Squares and wedding halls are not villages: their worlds are taken by name (WI_SQUARE*, WI_*_WEDDING).

#include <KNCSingleton.h>

#include <map>
#include <memory>
#include <string>

class CXSLLineMap;

class KWorldLineMaps
{
	DeclareSingleton( KWorldLineMaps );

public:
	KWorldLineMaps();
	~KWorldLineMaps();

	// Loads every line map; replaces what was loaded. False when the scripts that map villages to worlds are missing;
	// a world whose script or line map is missing is only logged (the original ships a few).
	bool Load();

	const CXSLLineMap* GetByVillage( int iVillageMapID ) const;
	const CXSLLineMap* GetByWorld( int iWorldID ) const;
	int GetWorldIDOfVillage( int iVillageMapID ) const;	// 0 when unknown

	size_t GetNumWorlds() const		{ return m_mapWorldLineMap.size(); }
	size_t GetNumLineMaps() const;	// distinct line maps (several worlds share one)

private:
	std::map< int, int >								m_mapVillageWorld;	// VILLAGE_MAP_ID -> WORLD_ID
	std::map< int, std::shared_ptr< CXSLLineMap > >		m_mapWorldLineMap;	// WORLD_ID -> line map
};

DefSingletonInline( KWorldLineMaps );
