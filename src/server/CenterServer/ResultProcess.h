#pragma once

#include "x2/platform/win32.h"
#include <RTTI.h>
//#include <ToString.h>
#include "RefreshSingleton.h"
//#include <KncLua.h>
//#include <lua_tinker.h>
//#include "Lua/KLuaManager.h"
#include "RoomUserManager.h"
//{{ 2011. 01. 20	최육사	던전 랭크 개편
	#include "RoomMonsterManager.h"
//}}
#include "DungeonMonsterManager.h"

#include "X2Data/XSLDungeon.h"
#include "X2Data/XSLUnit.h"
#include "X2Data/XSLRoom.h"


class KResultProcess
{
	NiDeclareRootRTTI( KResultProcess );
	DeclareRefreshSingleton( KResultProcess );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	enum DUNGEON_PARTY_NUM
	{
		NUM_02	= 2,
		NUM_03	= 3,
		NUM_04	= 4,
		NUM_TOTAL,
	};

	enum GAME_EVENT_VAR
	{
		GEV_EXP = 0,
		GEV_VP,
		GEV_ED,

		GEV_PARTY_EXP,
		GEV_PARTY_VP,
		GEV_PARTY_ED,

		GEV_END,
	};

	struct KTeamData
	{
		int		m_nMaxAlive;
		float	m_fMaxHP;
		int		m_nAverageLevel;
		int		m_nUnitNum;

		void Init()
		{
			m_nMaxAlive		= 0;
			m_fMaxHP		= 0;
			m_nAverageLevel	= 0;
			m_nUnitNum		= 0;
		}
	};

public:
	KResultProcess(void);
	~KResultProcess(void);

	//{{ 2011. 07. 19	최육사	대전 개편
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	bool		Result_TEAM( IN const float fTotalPlayTime,
							 IN const float fRemainingPlayTime,
							 IN const int iPVPChannelClass,
							 IN const bool bOfficialPvpCancel,
							 IN const SEnum::MATCH_TYPE eMatchType,
							 IN const SEnum::PVP_NPC_TYPE ePvpNpcType,
							 IN OUT KRoomUserManagerPtr spRoomUserManager,
							 OUT KEGS_END_GAME_PVP_RESULT_DATA_NOT& kPacketNot,
							 OUT KPvpMatchUserResultLog::END_TYPE& eEndType );

	bool		Result_SURVIVAL( IN const float fTotalPlayTime,
								 IN const float fRemainingPlayTime,
								 IN const int iPVPChannelClass,
								 IN const bool bOfficialPvpCancel,
								 IN const SEnum::MATCH_TYPE eMatchType,
								 IN const SEnum::PVP_NPC_TYPE ePvpNpcType,
								 IN OUT KRoomUserManagerPtr spRoomUserManager,
								 OUT KEGS_END_GAME_PVP_RESULT_DATA_NOT& kPacketNot,
								 OUT KPvpMatchUserResultLog::END_TYPE& eEndType );

	bool		Result_TEAM_DEATH( IN const float fTotalPlayTime,
								   IN const float fRemainingPlayTime,
								   IN const int iPVPChannelClass,
								   IN const bool bOfficialPvpCancel,
								   IN const SEnum::MATCH_TYPE eMatchType,
								   IN const SEnum::PVP_NPC_TYPE ePvpNpcType,
								   IN OUT KRoomUserManagerPtr spRoomUserManager,
								   OUT KEGS_END_GAME_PVP_RESULT_DATA_NOT& kPacketNot,
								   OUT KPvpMatchUserResultLog::END_TYPE& eEndType );
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	//}}

	bool		Result_DUNGEON( IN OUT KRoomUserManagerPtr spRoomUserManager,
								IN OUT KDungeonMonsterManager& kDungeonMonsterManager,
								IN const bool bIsWin,
								IN const CXSLDungeon::DUNGEON_TYPE eDungeonType,
								IN const char cDungeonMode,
								IN const char cDifficulty,
								IN const int iDungeonID,
								//{{ 2011. 08. 22	최육사	헤니르 시공 개편
								IN const int iStageID,
								//}}
								IN const int nTotalPlayTime,
								IN const int nRemainingTime,
								IN const int nDropItemUID,
								IN const int iStartedNumMember,
								//{{ 2012. 03. 15	최육사	배틀필드 시스템
								IN const bool bStartedByAutoParty,
								//}}
								OUT int& iClearStageCount,
								OUT KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT& kUnitInfoNot,
								OUT std::vector< KRoomUserPtr >& vecKRoomUserPtr,
								OUT std::vector< KERM_UPDATE_DUNGEON_UNIT_INFO_NOT >& vecUpdateUnitInfo
								);

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	bool		Result_DefenceDUNGEON( IN OUT KRoomUserManagerPtr spRoomUserManager,
									   IN OUT KDungeonMonsterManager& kDungeonMonsterManager,
									   IN const bool bIsWin,
									   IN const CXSLDungeon::DUNGEON_TYPE eDungeonType,
									   IN const char cDungeonMode,
									   IN const char cDifficulty,
									   IN const int iDungeonID,
									   IN const int nTotalPlayTime,
									   IN const int nRemainingTime,
									   IN const int nDropItemUID,
									   IN const int iStartedNumMember,
									   //{{ 2012. 03. 15	최육사	배틀필드 시스템
									   IN const bool bStartedByAutoParty,
									   //}}
									   OUT int& iClearStageCount,
									   OUT KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT& kUnitInfoNot,
									   OUT std::vector< KRoomUserPtr >& vecKRoomUserPtr,
									   OUT std::vector< KERM_UPDATE_DUNGEON_UNIT_INFO_NOT >& vecUpdateUnitInfo,
									   IN int iProtectedCrystalCount );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가
/*																// 헷갈려서 주석 처리

*/

	//{{ 2011. 07. 25	최육사	대전 개편
	bool		Result_TEAM_BREAK(			IN KRoomUserManagerPtr spRoomUserManager, IN KRoomUserPtr spRoomUser, IN const int iReason, IN const SEnum::MATCH_TYPE eMatchType, IN const SEnum::PVP_NPC_TYPE ePvpNpcType, OUT KERM_UPDATE_PVP_UNIT_INFO_NOT& kInfo );
	bool		Result_TEAM_DEATH_BREAK(	IN KRoomUserManagerPtr spRoomUserManager, IN KRoomUserPtr spRoomUser, IN const int iReason, IN const SEnum::MATCH_TYPE eMatchType, IN const SEnum::PVP_NPC_TYPE ePvpNpcType, OUT KERM_UPDATE_PVP_UNIT_INFO_NOT& kInfo );
	bool		Result_SURVIVAL_BREAK(		IN KRoomUserManagerPtr spRoomUserManager, IN KRoomUserPtr spRoomUser, IN const int iReason, IN const SEnum::MATCH_TYPE eMatchType, IN const SEnum::PVP_NPC_TYPE ePvpNpcType, OUT KERM_UPDATE_PVP_UNIT_INFO_NOT& kInfo );
	//}}
	bool		Result_DUNGEON_BREAK(		IN KRoomUserPtr spRoomUser, OUT KERM_UPDATE_DUNGEON_UNIT_INFO_NOT& kInfo );

	//{{ 2011. 08. 03	최육사	대전 개편
	int			DecidePVPWinTeam( IN const float fTotalPlayTime, IN const float fRemainingPlayTime, IN OUT KRoomUserManagerPtr spRoomUserManager, IN const CXSLRoom::PVP_GAME_TYPE eGameType, OUT KPvpMatchUserResultLog::END_TYPE& eEndType );
	//}}

	void		AddDungeonPartyBonus_LUA( float fTwo, float fThree, float fFour );	//던전 파티보너스 확률.!

	float		GetDungeonPartyBonusRate( int iNumber );	//던전 인원별 확률 제공.

	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	float		GetDungeonPartyDifficultyBonusRate( char cDifficulty );	//던전 난이도별 확률 제공.
	void		AddDungeonPartyDifficultyBonus_LUA( float fNormal, float fHard, float fExpert );
	float		GetDungeonModePartyBonusRate( char cDungeonMode );
	void		AddDungeonModePartyBonus_LUA( float fNormal, float fHenir, float fSecret );
	//}}

	//{{ 2013. 02. 25   던전 이탈 시 랭크 기록 - 김민성
	void		LeaveRoomResultRank( IN int							iDungeonID,
									 IN char						cDifficultyLevel,
									 KRoomUserPtr					spRoomUser,
									 IN OUT KDungeonMonsterManager& kDungeonMonsterManager,
									 IN int							iEndNumMember,
									 IN int							iPartyTotalGivenDamage,
									 IN int							iTotalPlayTime,
									 KERM_UPDATE_DUNGEON_UNIT_INFO_NOT& kUpdateUnitInfoDummy,
									 KDungeonUnitResultInfo&		kDungeonUnitInfoDummy
									);
	//}

	//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	char		GetSubStageComboResult( IN int iDungeonIDDiff, IN int iEndNumMember, IN int iCombo, IN int iMonsterPoint );
	char		GetSubStageTechResult( IN int iDungeonIDDiff, IN int iEndNumMember, IN int iTech, IN int iMonsterPoint );
	//}

protected:
	//아래의 정렬 함수는 대전일경우 순서대로 사용해야 함.
	void		SortbyPVPMD( std::vector<KPVPUnitResultInfo>& vecData );
	void		SortbyPVPKill( std::vector<KPVPUnitResultInfo>& vecData );
	void		SortbyPVPHP( std::vector<KPVPUnitResultInfo>& vecData, KRoomUserManagerPtr spRoomUserManager, CXSLRoom::PVP_GAME_TYPE eGameType );
	void		SortbyPVPLevel( std::vector<KPVPUnitResultInfo>& vecData, KRoomUserManagerPtr spRoomUserManager, CXSLRoom::PVP_GAME_TYPE eGameType );
	void		SortbyPVPSlot( std::vector<KPVPUnitResultInfo>& vecData, KRoomUserManagerPtr spRoomUserManager, CXSLRoom::PVP_GAME_TYPE eGameType );

	//{{ 2012. 12. 17	최육사	이벤트 보너스 코드 리팩토링
	//void		SetGameEvent( int iDungeonID, int iUnitLevel, char cUnitClass, int iPartyNum = 0 ); -- 안쓰는 코드
	//}}

protected:
	float		m_fPartyBonus[NUM_TOTAL];
	//{{ 2012. 12. 17	최육사	이벤트 보너스 코드 리팩토링
	//float		m_fGameEvent[GEV_END]; -- 안쓰는 코드
	//}}
	//{{ 2011. 03. 21  김민성	던전 난이도 보정
	float		m_fDungeonModePartyBonus[CXSLDungeon::DT_NUM];
	float		m_fPartyDifficultyBonus[CXSLDungeon::DL_NUM];
	//}}
};

DefRefreshSingletonInline( KResultProcess );
