#pragma once

#include "RefreshSingleton.h"
#include "Lottery.h"
#include "CommonPacket.h"
#include "X2Data/XSLDungeon.h"



class KAttribNpcTable
{
	DeclareRefreshSingleton( KAttribNpcTable );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:

	enum ATTRIB_NPC_DROP
	{
        AND_NORMAL = 0,
		AND_SECRET,

		AND_MAX,
	};

	enum ATTRIB_TYPE
	{
		AT_NONE = 0,

		AT_BLAZE,
		AT_FROZEN,
		AT_POISON,
		AT_PIERCING,
		AT_SHOCK,
		AT_SNATCH,

		AT_BLAZE_MASTER,
		AT_FROZEN_MASTER,
		AT_POISON_MASTER,
		AT_MASTER_PIERCING,
		AT_MASTER_SHOCK,
		AT_MASTER_SNATCH,

		AT_BLAZE_PIERCING,
		AT_BLAZE_SHOCK,
		AT_BLAZE_SNATCH,
		AT_FROZEN_PIERCING,
		AT_FROZEN_SHOCK,
		AT_FROZEN_SNATCH,
		AT_POISON_PIERCING,
		AT_POISON_SHOCK,
		AT_POISON_SNATCH,
	};

	struct KDropNpcCount
	{
		int				m_iSubStageCount;
		int				m_iDropNpcCount;
	};

	typedef std::pair< int, int >	KAttribNpcKey; // 1. NpcLv     2. Difficulty

	struct KAttribNpcInfo
	{
		bool			m_bTotalRandom;

		// 특수능력 랜덤
		KLottery		m_kAtkNpcLot;
		KLottery		m_kDefNpcLot;
		KLottery		m_kTotalLot;

		// 속성 랜덤
		KLottery		m_kAttribLot;
	};

	typedef std::pair< char, char >		KAttribNpcDropKey; // key1[DUNGEON_TYPE], key2[DUNGEON_MODE or DIFFICULTY]

	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	typedef std::pair< char, int >		KAttribNpcCountKey; // key1[DUNGEON_TYPE], key2[Player Count]
	//}}

	struct KAttribNpcRandom
	{
		KAttribNpcInfo	m_kBrave;
		KAttribNpcInfo	m_kClever;
	};

	typedef std::map< int, int >	KAttribDropTable;

public:
	KAttribNpcTable(void);
	~KAttribNpcTable(void);

	//////////////////////////////////////////////////////////////////////////
	// 스크립트 파싱
	//{{ 2010. 04. 08  최육사	비밀던전 헬모드
	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	bool SetAttribSubStageCount_LUA( char cDungeonType, char cMode, int iSubStageCount );
	bool SetNpcDropCount_LUA( char cDungeonType, int iPlayerCount, int iNpcDropCount );
	//}}
	//}}
	bool AddAttribNpcInfo_LUA();
	bool AddAttribDropInfo_LUA( int iNpcLv, int iNormalDropGroupID, int iHardDropGroupID, int iExpertDropGroupID );
	bool AddAttribDropGroupInfo_LUA( int iDropGroupID, int iDropTableGroupID, float fRate );
	bool AddNoDropNpc_LUA( int iNpcID );

	//////////////////////////////////////////////////////////////////////////
	// 속성몬스터 드롭
	//{{ 2010. 04. 08  최육사	비밀던전 헬모드
	bool GetAttribNpcDropCount( IN const char cDungeonType, IN const char cMode, OUT int& iSubStageCount, OUT int& iDropNpcCount );
	bool GetAttribNpcInfo( IN const CXSLDungeon::DUNGEON_TYPE eDungeonType, IN const char cMode, IN const char cNpcLv, IN const int iNpcID, OUT KAttribEnchantNpcInfo& kAttribNpcInfo );
	bool GetAttribNpcInfoExtraStage( IN const int iNpcID, OUT KAttribEnchantNpcInfo& kAttribNpcInfo );
	//}}
	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	bool GetAttribNpcDropCount( IN char cDungeonType, IN char cMode,IN int iPalyerCount, OUT int& iSubStageCount, OUT int& iDropNpcCount );
	//}}

	void GetAttribEnchantValue( IN int iAttribType, OUT char& cAttribEnchant1, OUT char& cAttribEnchant2 );

	//{{ 필드 드롭 개편 - 김민성
	//{{ 2012. 12. 16  드롭 이벤트 - 김민성

	bool AttribNpcDropItem( IN int iDifficulty, IN int iNpcLv, IN const float fUserContribution, IN float fDropRate, OUT std::vector< int >& vecDropItem );

	//}}
// #else
// 	bool AttribNpcDropItem( IN int iDifficulty, IN int iNpcLv, IN int iDropItemNum, OUT std::vector< int >& vecDropItem );
	//}}
	bool IsNoDropNpc( IN int iNpcID );

private:
	//{{ 2010. 04. 08  최육사	비밀던전 헬모드
	std::map< KAttribNpcDropKey, KDropNpcCount >	m_mapDropNpcCount;
	std::map< KAttribNpcKey, KAttribNpcRandom >		m_mapAttribNpc[AND_MAX];
	//}}
	std::map< int, KAttribDropTable >				m_mapAttribDropTable;
	std::map< int, KLottery >						m_mapAttribDropGroup;
	std::set< int >									m_setNoDropNpc;

	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	std::map< KAttribNpcCountKey, int >				m_mapAttribNpcCount;
	//}}
};

DefRefreshSingletonInline( KAttribNpcTable );
