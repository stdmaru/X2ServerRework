#pragma once

#include "RefreshSingleton.h"
#include <vector>
#include "CommonPacket.h"

//{{ 2012. 11. 29	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	#include <map>
//}}


#define SAME_UNIT_TYPE_WAIT_TIME		30.0
#define DIRECT_GAME_START_WAIT_TIME		300.0
//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
#define SAME_DIFFICULTY_LEVEL_WAIT_TIME	120.0
//}}
#define AUTO_PARTY_BONUS_TIME_DEFALUT	120


class KAutoPartyScriptManager
{
	DeclareRefreshSingleton( KAutoPartyScriptManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	enum EVENT_RESULT
	{
		ER_NONE = 0,
		ER_ACTIVATED,
		ER_DEACTIVATED,
	};

public:
	KAutoPartyScriptManager(void);
	virtual ~KAutoPartyScriptManager(void);

	// for lua
	bool	AddAutoPartyDungeonInfo_LUA( int iDungeonMode, int iDungeonIDWithDif, int iBeginLevel, int iEndLevel );
	void	SetSameUnitTypeWaitTime_LUA( double fWaitTime );
	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	void	SetSameDifficultyLevelWaitTime_LUA( double fWaitTime );
	//}}
	void	SetDirectGameStartWaitTime_LUA( double fWaitTime );
	void	SetAutoPartyBonusTime_LUA( double fBonusTime );
	void	SetAutoPartyMakingBeginCount_LUA( int iAutoPartyMakingBeginCount );
	void	SetCheckWaitUserCount_LUA( int iCheckWaitUserCount );
	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	void	SetIsAllMemberCheckSameUnitTypeTime_LUA( bool bVal );
	void	SetIsAllMemberCheckSameDifficultyLevelTime_LUA( bool bVal );
	void	SetIsAllMemberCheckDirectGameStartTime_LUA( bool bVal );
	//}}

	//{{ 2012. 11. 29	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	void	SetAutoPartyMatchingSuccessNum_LUA( IN int iDungeon, IN int iNum );
	//}}

	// function
	const std::vector< KAutoPartyMakingProcessCondition >& GetAutoPartyDungeonInfo() const	{ return m_vecAutoPartyDungeonInfo; }
	double	GetSameUnitTypeWaitTime() const													{ return m_fSameUnitTypeWaitTime; }
	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	double	GetSameDifficultyLevelWaitTime() const											{ return m_fSameDifficultyLevelWaitTime; }
	//}}
	double	GetDirectGameStartWaitTime() const												{ return m_fDirectGameStartWaitTime; }
	double	GetAutoPartyBonusTime() const													{ return m_fAutoPartyBonusTime; }
	int		GetAutoPartyMakingBeginCount() const											{ return m_iAutoPartyMakingBeginCount; }
	int		GetCheckWaitUserCount() const													{ return m_iCheckWaitUserCount; }

	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	bool	IsCheckAllMemberSameUnitTypeTime() const										{ return m_bIsAllMemberCheckSameUnitTypeTime; }
	bool	IsCheckAllMemberSameDifficultyLevelTime() const									{ return m_bIsAllMemberCheckSameDifficultyLevelTime; }
	bool	IsCheckAllMemberDirectGameStartTime() const										{ return m_bIsAllMemberCheckDirectGameStartTime; }
	//}}

	//{{ 2012. 11. 29	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	int		GetAutoPartyMatchingSuccessNum( IN const int iDungeon ) const;
	//}}

private:
	std::vector< KAutoPartyMakingProcessCondition >		m_vecAutoPartyDungeonInfo;
	double												m_fSameUnitTypeWaitTime;
	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	double												m_fSameDifficultyLevelWaitTime;
	//}}
	double												m_fDirectGameStartWaitTime;
	int													m_iAutoPartyMakingBeginCount;
	int													m_iCheckWaitUserCount;

	// 자동 파티 보너스
	double												m_fAutoPartyBonusTime;

	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	// 자동 파티 조건 체크 on/off
	bool												m_bIsAllMemberCheckSameUnitTypeTime;
	bool												m_bIsAllMemberCheckSameDifficultyLevelTime;
	bool												m_bIsAllMemberCheckDirectGameStartTime;
	//}}

	//{{ 2012. 11. 29	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	std::map<int, int>									m_mapAutoPartyMatchingSuccessNum;
	//}}
};

DefRefreshSingletonInline( KAutoPartyScriptManager );
