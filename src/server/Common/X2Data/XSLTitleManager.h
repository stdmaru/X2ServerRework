#pragma once

#include "RefreshSingleton.h"
#include "X2Data/XSLMain.h"
#include "X2Data/XSLDungeon.h"
#include <map>
#include <vector>
#include "X2Data/XSLRoom.h"

class CXSLTitleManager
{
	DeclareRefreshSingleton( CXSLTitleManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:

	enum TITLE_TYPE
	{
		TT_NONE = 0,
		TT_RUBEN,
		TT_ELDER,
		TT_BESMA,
		TT_ALTERA,
		TT_ARCADE,
		TT_PVP,
		TT_EVENT,
		TT_VELDER,
		//{{ 2011. 01. 25	최육사	하멜 추가
		TT_HAMEL,
		//}}
		TT_GROW_UP, // 해외. 이름 수정(2013.09.03 김창한)
//#ifdef SERV_ADD_TITLE_CONDITION_2013_08		// 적용날짜: 2013-08-13
		TT_SANDER,
//#endif // SERV_ADD_TITLE_CONDITION_2013_08

		TT_END,
	};

	enum TITLE_MISSION_CLEAR_TYPE
	{
		TMCT_NONE = 0,
		TMCT_NPC_TALK,
		TMCT_NPC_HUNT,
		TMCT_ITEM_COLLECTION,
		TMCT_QUEST_ITEM_COLLECTION,
		TMCT_DUNGEON_TIME,
		TMCT_DUNGEON_RANK,
		TMCT_DUNGEON_DAMAGE,
		TMCT_DUNGEON_CLEAR_COUNT,
		TMCT_PVP_PLAY,
		TMCT_PVP_WIN,
		TMCT_PVP_KILL,
		TMCT_QUEST,
		TMCT_COLLECT_TITLE,
		TMCT_USE_ITEM,
		//{{ 2010. 05. 17  최육사	대전 던전 서버군 통합
		TMCT_WITH_DIF_SERV_USER,
		//}}
		//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
		TMCT_GIVE_PET_FEED,						// 펫 먹이 주기
		TMCT_PLAYER_WITH_DUNGEON_CLEAR,			// 특정 인원↓, 던전클리어
		TMCT_RESURRECTION_STONE,				// 부활석 사용
		TMCT_TOGETHER_MISSION_CLEAR,			// 미션 동시 만족
		TMCT_USER_UNIT_DIE,						// 유저 사망
		//}}
		TMCT_ITEM_SOCKET,
		TMCT_ITEM_ENCHANT_LEVEL,
		TMCT_ITEM_ENCHANT_COUNT,
		TMCT_ITEM_ATTRIB,
		TMCT_ITEM_RESOLVE,
		TMCT_MAX,
	};

	struct TitleTemplet
	{
		int						m_iTitleID;
		int						m_iSortNum;

		TITLE_TYPE				m_eTitleType;
		std::wstring			m_wstrTitleName;
		//std::wstring			m_wstrDescription;

		//std::wstring			m_wstrParticleName;

		CXSLUnit::UNIT_TYPE		m_eUnitType;
		CXSLUnit::UNIT_CLASS	m_eUnitClass;
		int						m_iOpenLevel;
		bool					m_bIsSecretTitle;

		int						m_iBaseHP;
		int						m_iAtkPhysic;
		int						m_iAtkMagic;
		int						m_iDefPhysic;
		int						m_iDefMagic;

		//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
		std::vector< int >		m_vecSocketOption;
		//}}
	};

	//////////////////////////////////////////////////////////////////////////
	// 미션 정보

	// Mission을 받기위한 제약사항
	struct MissionCondition
	{
		int						m_iLevel;
		CXSLUnit::UNIT_TYPE		m_eUnitType;
		CXSLUnit::UNIT_CLASS	m_eUnitClass;
		int						m_iClearMissionID;

		MissionCondition()
		{
			m_iLevel				= -1;
			m_eUnitType				= CXSLUnit::UT_NONE;
			m_eUnitClass			= CXSLUnit::UC_NONE;
			m_iClearMissionID		= 0;
		}
	};

	struct MissionTemplet
	{
		int						 m_iMissionID;
		TITLE_MISSION_CLEAR_TYPE m_eClearType;
		std::wstring			 m_wstrMissionName;

		MissionCondition		 m_kCondition;
		std::vector< int >		 m_vecSubMission;

		short					 m_sPeriod;
		int						 m_iTitleID;

		bool					 m_bIsSecretTitle;

		MissionTemplet()
		{
			m_iMissionID	 = 0;
			m_eClearType	 = TMCT_NONE;
			m_sPeriod		 = 0;
			m_iTitleID		 = 0;
			m_bIsSecretTitle = false;
		}
	};

	//////////////////////////////////////////////////////////////////////////

	//Sub quest type 조건별 확인할 데이터
	struct ClearCondition
	{
		//::공용 데이터
		//::1. Dungeon ID
		SEnum::DUNGEON_ID			m_eDungeonID;
		//::2. Dungeon Difficulty
		char							m_cDifficulty;
		//{{ 2010. 08. 23  최육사	비밀던전 헬모드
		CXSLDungeon::DUNGEON_MODE		m_eDungeonMode;
		//:: Upper Difficulty
		bool							m_bUpperDifficulty;
		//}}

		//::3. NPC ID & Monster ID 2가지 기능.

		//{{ 2012. 1. 11	Merge 박세훈	2012.12.26 임규수 타이틀 미션 클리어 NPC 복수 적용
		std::set< CXSLUnitManager::NPC_UNIT_ID > m_setKillNPCID;
		//}}

		//::4. PVP TYPE
		CXSLRoom::PVP_GAME_TYPE			m_ePVPType;
		//::5. Item ID
		int								m_iCollectionItemID;

		//::조건별 데이터
		//::1. Monster kill count..
		int								m_iKillNum;
		//::2. Item collection count
		int								m_iCollectionItemNum;
		//::3. clear dungeon rank
		CXSLDungeon::RANK_TYPE			m_eDungeonRank;
		//::4. clear dungeon time
		int								m_iDungeonClearTime;
		//::5. talk npc
		CXSLUnitManager::NPC_UNIT_ID	m_eTalkNPCID;
		bool							m_bTalkNPC;
		//::6. pvp play count
		int								m_iPVPPlay;
		//::7. pvp win count
		int								m_iPVPWin;
		//::8. pvp kill count
		int								m_iPVPKill;
		//::9. quest item drop rate
		float							m_fQuestItemDropRate;
		//::10. damage
		int								m_iDungeonDamage;
		//::11. Dungeon clear count
		int								m_iDungeonClearCount;
		//::12. Quest Clear
		int								m_iQuestID;
		//::13. Title Collect
		int								m_iTitleID;
		//::14. Use Item
		int								m_iItemID;
		//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
		//::15. Pet Feed
		CXSLItem::ITEM_GRADE			m_eItemGrade;
		int								m_iPetFeedCount;
		//::16. Player With Dungeon Clear
		int								m_iPlayerCount;
		//::17. Use Resurrection Stone
		bool							m_bCheckResurrectionStone;			// false - 부활하지 않음 , true - 몇번 부활함
//{{ 2011. 09. 19  김민성	칭호 획득 조건 추가
		//::18. User Unit Die Reason
		KEGS_USER_UNIT_DIE_REQ::USER_UNIT_DIE_REASON	m_eDieReason;
//}}
		//}}
		//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
		int								m_iUseResurrectionStoneCount;
		//}

		int								m_iEnchantLevel;
		int								m_iItemLevel;

		ClearCondition()
		{
			//{{ 2012. 1. 11	Merge 박세훈	2012.12.26 임규수 타이틀 미션 클리어 NPC 복수 적용
			//}}
			m_iKillNum				= 0;
			m_iCollectionItemID		= -1;
			m_iCollectionItemNum	= 0;
			m_eDungeonID			= SEnum::DI_NONE;
			m_cDifficulty			= -1;
			//{{ 2010. 08. 23  최육사	비밀던전 헬모드
			m_eDungeonMode			= CXSLDungeon::DM_INVALID;
			m_bUpperDifficulty		= false;
			//}}

			m_eDungeonRank			= CXSLDungeon::RT_NONE;
			m_iDungeonClearTime		= 0;
			m_eTalkNPCID			= CXSLUnitManager::NUI_NONE;
			m_bTalkNPC				= false;
			m_ePVPType				= CXSLRoom::PGT_TEAM;
			m_iPVPPlay				= 0;
			m_iPVPWin				= 0;
			m_iPVPKill				= 0;
			m_fQuestItemDropRate	= 0.0f;
			m_iDungeonDamage		= 0;
			m_iDungeonClearCount	= 0;
			m_iQuestID				= 0;
			m_iTitleID				= 0;
			m_iItemID				= 0;
			//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
			m_eItemGrade			= CXSLItem::IG_NONE;
			m_iPetFeedCount			= 0;
			m_iPlayerCount			= 0;
			m_bCheckResurrectionStone = false;
			m_eDieReason			= KEGS_USER_UNIT_DIE_REQ::UUDR_UNKNOWN;
			//}}
			//{{ 2013. 02. 05  칭호 획득 조건 추가(부활석 사용 횟수, 샌더 마을) - 김민성
			m_iUseResurrectionStoneCount = 0;
			//}
			m_iEnchantLevel			= 0;
			m_iItemLevel			= 0;
		}
	};

	struct SubMissionTemplet
	{
		int					 	 m_iSubMissionID;

		std::wstring			 m_wstrDescription;
		TITLE_MISSION_CLEAR_TYPE m_eClearType;
		ClearCondition			 m_ClearCondition;
		bool					 m_bAutomaticDescription;

		SubMissionTemplet()
		{
			m_iSubMissionID = -1;
			m_eClearType	= TMCT_NONE;
			m_bAutomaticDescription = true;
		}
	};

	//{{ 2011. 04. 27	최육사	칭호 획득 아이템 개편
	struct TitleSimpleInfo
	{
		int						m_iTitleID;
		short					m_sPeriod;

		TitleSimpleInfo()
		{
			m_iTitleID = 0;
			m_sPeriod = 0;
		}
	};
	//}}

public:
	CXSLTitleManager(void);
	virtual ~CXSLTitleManager(void);

	// for lua
	bool AddTitleInfo_LUA();
	bool AddTitleMissionInfo_LUA();
	bool AddSubTitleMissionInfo_LUA();

	const TitleTemplet* GetTitleTemplet( int iTitleID );
	bool IsExistTitleID( int iTitleID );

	const MissionTemplet* GetMissionInfo( int iMissionID );				// Title Mission
	const SubMissionTemplet* GetSubMissionInfo( int iSubMissionID );	// Sub Title Mission

	// Auto Reg Mission
	const std::vector< MissionTemplet >& GetMissionList() { return m_vecAutoRegMission; }

	//{{ 2011. 04. 27	최육사	칭호 획득 아이템 개편
	bool GetUseItemTitleInfoBySubMissionID( IN const int iSubMissionID, OUT TitleSimpleInfo& kInfo );
	bool GetUseItemTitleInfoByItemID( IN const int iItemID, OUT TitleSimpleInfo& kInfo );
	//}}

private:
	bool	LoadClearCondition( KLuaManager& luaManager, SubMissionTemplet& kSubMissionTemplet );

private:
	std::map< int, TitleTemplet >		m_mapTitleInfo;

	std::map< int, MissionTemplet >		m_mapMission;
	std::map< int, SubMissionTemplet >	m_mapSubMission;

	// 자동미션등록을 위한..
	std::vector< MissionTemplet >		m_vecAutoRegMission;

	//{{ 2011. 04. 27	최육사	칭호 획득 아이템 개편
	std::vector< int >					m_vecMissionIDByClearType[TMCT_MAX];
	std::map< int, TitleSimpleInfo >	m_mapTitleInfoByUseItemID;
	//}}
};

DefRefreshSingletonInline( CXSLTitleManager );
