#pragma once

#include <KNCSingleton.h>
#include <ToString.h>
#include <list>
#include "Enum/Enum.h"
#include "AutoParty.h"
#include "AutoPartyUserWaitList.h"
#include "CommonPacket.h"
#include "CompareLimitList.h"

//{{ 2012. 02. 07	최육사	배틀필드 시스템


class KAutoPartyCompleteTimeManager
{
public:
	enum COMPLETE_TIME_ENUM
	{
		CTE_MAX_COUNT = 12,
	};

	//{{ 던전 자동 매칭 예상 시간 수정 - 김민성
	struct KAutoPartyTime
	{
		CTime		m_tAutoPartyTime;
		int			m_iWatiTime;		// 매칭이 성공할때까지 대기한 시간(초단위)

		KAutoPartyTime()
		{
			m_iWatiTime = 0;
		}
	};
	//}

	KAutoPartyCompleteTimeManager();
	~KAutoPartyCompleteTimeManager();

	unsigned int GetListSize() const { return m_listCompleteTime.size(); }
	void AddCompleteTime( IN const int iCompleteTime );
	int	GetAutoPartyMakingEstimatedTime() const;

private:
	//{{ 던전 자동 매칭 예상 시간 수정 - 김민성
	std::list< KAutoPartyTime >		m_listCompleteTime;
	//}
};


class KAutoPartyMakingProcess
{
	enum WAIT_TIME_OUT
	{
		WTO_DIFFERENT_UNIT_TYPE = 0,
		WTO_SAME_UNIT_TYPE,
		WTO_DIRECT_GAME_START,
	};

public:
	struct SLevelCondition
	{
		int		m_iListType;		// 해당 레벨간격마다 ListType이 매겨집니다. 값은 0부터 시작합니다.
		int		m_iBeginLevel;
		int		m_iEndLevel;

		SLevelCondition()
		{
			m_iListType = 0;
			m_iBeginLevel = 0;
			m_iEndLevel = 0;
		}
	};

public:
    KAutoPartyMakingProcess( IN const KAutoPartyMakingProcessCondition& kInfo );
    ~KAutoPartyMakingProcess();

	// Tick
	void	Tick();

	// Init
	void	ClearAllAutoPartyInfo();
	void	AddLevelCondition( IN const int iBeginLevel, IN const int iEndLevel );

	// get info
	int		GetDungeonID() const										{ return m_iDungeonID; }
	int		GetDifficultyLevel() const									{ return m_iDifficultyLevel; }
	int		GetDungeonIDWithDif() const									{ return m_iDungeonID + m_iDifficultyLevel; }
	char	GetDungeonMode() const										{ return static_cast<char>(m_iDungeonMode); }
	//{{ 2012. 11. 21	최육사		대기인원 표시 레벨 구간으로 수정
	int		GetWaitUserCountByListType( IN const int iListType ) const;
	int		GetWaitUserCountByWaitNumber( IN const UidType iWaitNumber );
	//}}

	// WaitNumber
	UidType	NewWaitNumber();
	static UidType ExtractDungeonIDAtWaitNumber( IN const UidType iUID_ );
	static UidType ExtractDungeonModeAtWaitNumber( IN const UidType iUID_ );
	static void SetDungeonIDAtWaitNumber( IN OUT UidType& iDestUID_, IN const UidType iSrcUID_ );
	static void SetDungeonModeAtWaitNumber( IN OUT UidType& iDestUID_, IN const UidType iSrcUID_ );

	// function
	bool	IsExistInWaitListByUnitUID( IN const UidType iUnitUID )		{ return ( m_mapAutoPartyWaitUser.find( iUnitUID ) != m_mapAutoPartyWaitUser.end() ); }

	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	bool	RegAutoPartyWaitList( IN const std::vector< KAutoPartyUserInfo >& vecAutoPartyUserList,
								  IN const KAutoPartyConditionInfo& kAutoPartyCondition,
								  OUT UidType& iWaitNumber,
								  IN const bool bRegFrontOfList = false );
	//}}
	bool	UnRegAutoPartyWaitListByUnitUID( IN const UidType iUnitUID, IN const int iUnRegReason, IN const std::wstring& wstrCancelUnitNickName = L"" );
	bool	UnRegAutoPartyWaitListByWaitNumber( IN const UidType iWaitNumber, IN const int iUnRegReason, IN const std::wstring& wstrCancelUnitNickName = L"" );
	void	MakingAutoPartyProcessing( OUT std::vector< KAutoPartyPtr >& vecMadeAutoParty );
	int		GetAutoPartyMakingEstimatedTime() const						{ return m_kCompleteTimeManager.GetAutoPartyMakingEstimatedTime(); }

	bool	GetListTypeByLevelCondition( IN const u_char ucLevel, OUT int& iListType ) const;

	// 자동 파티 보너스
	bool	IsActivatedAutoPartyBonus() const							{ return m_bActivatedAutoPartyBonus; }
	bool	IsActivatedDelayTimeBonus();
	void	SetActivatedAutoPartyBonus( IN const bool bActivate );
	bool	IsAutoPartyBonusTime() const;
	void	RestartGameStartLastTime()									{ m_tGameStartLastTime.restart(); }

protected:
	void	AutoPartyProcess_GetWaitUserListByOptimalConditionSort( IN const KAutoPartyUserWaitListPtr spFrontAutoPartyWaitUser,
																	IN const double fElapsedWaitTime,
																    IN const std::list< KAutoPartyUserWaitListPtr >& listAutoPartyMakingWaitList,
																	OUT KCompareLimitList< KAutoPartyUserWaitListPtr >& listOptimalAutoPartyWaitList );

	void	AutoPartyProcess_GetWaitNumberListForCompleteMakeAutoParty( IN const KAutoPartyUserWaitListPtr spFrontAutoPartyWaitUser,
																		IN const std::list< KAutoPartyUserWaitListPtr >& listSortedWaitUserList,
																		OUT std::vector< UidType >& vecSelectedWaitUserList,
																		OUT int& iSumAutoPartyPlayerCount
																		//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
																		, OUT char& iDifficulty
																		//}}
																		);

protected:
	// WaitUserList
	KAutoPartyUserPtr			GetAutoPartyWaitUserByUnitUID( IN const UidType iUnitUID );
	KAutoPartyUserWaitListPtr	GetAutoPartyWaitListByWaitNumber( IN const UidType iWaitNumber );

	// AutoParty
	KAutoPartyPtr	GetAutoParty( IN const UidType iAutoPartyUID );
	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	KAutoPartyPtr	CreateAutoParty( IN const std::vector< KAutoPartyUserInfo >& vecAutoPartyUserInfo, IN const KAutoPartyConditionInfo& kInfo );
	//}}
	void			DestroyAutoParty( IN const UidType iAutoPartyUID );

	// make object
	KAutoPartyUserPtr	MakeUser( IN const KAutoPartyUserInfo& kInfo );
	//{{ 2012. 11. 14	최육사		자동파티 대기열 난이도 통합
	KAutoPartyPtr		MakeAutoParty( IN const KAutoPartyConditionInfo& kInfo );
	//}}

	//{{ 2012. 11. 21	최육사		대기인원 표시 레벨 구간으로 수정
	void			IncreaseAutoPartyWaitUserCount( IN const int iListType, IN const int iCount );
	void			DecreaseAutoPartyWaitUserCount( IN const int iListType, IN const int iCount );
	//}}

protected:
	template < class T > void SendToGSCharacter( IN UidType iGSUID, IN UidType iUnitUID, IN unsigned short usEventID, IN const T& data );
	template < class T > void BroadCastGameServer( IN unsigned short usEventID, IN const T& data );

protected:
	// 자동 파티 대기자
	UidType											m_iLastWaitNumber;
	std::map< UidType, KAutoPartyUserPtr >			m_mapAutoPartyWaitUser;			// key : UnitUID
	std::map< UidType, KAutoPartyUserWaitListPtr >	m_mapAutoPartyWaitList;			// key : WaitNumber
	std::list< KAutoPartyUserWaitListPtr >			m_listAutoPartyWaitList;
	static FSMclassPtr								ms_spUserFSM;

	// 자동 파티 조건 정보
	int												m_iDungeonMode;
	int												m_iDungeonID;
	int												m_iDifficultyLevel;
	std::vector< SLevelCondition >					m_vecLevelCondition;			// 레벨대 별로 구분하기 위한 정보입니다.

	// 자동 파티 통계 정보
	KAutoPartyCompleteTimeManager					m_kCompleteTimeManager;

	// 자동 파티 보너스 여부
	bool											m_bActivatedAutoPartyBonus;		// 자동 파티 보너스 여부
	boost::timer									m_tGameStartLastTime;			// 자동 파티 보너스 타이머 [타이머 시간이 2분을 넘으면 보너스적용!]
	std::vector< boost::timer >						m_vecDelayBonusTimer;			// UI갱신과 실제 보너스 적용시간 사이의 간격을 메꾸기 위한 타이머

	//{{ 2012. 11. 21	최육사		대기인원 표시 레벨 구간으로 수정
	std::map< int, int >							m_mapAutoPartyWaitUserCount;	// 자동 파티 대기인원 수 ( 레벨구간별 )
	//}}
};


template < class T >
void KAutoPartyMakingProcess::SendToGSCharacter( IN UidType iGSUID, IN UidType iUnitUID, IN unsigned short usEventID, IN const T& data )
{
	UidType anTrace[2] = { iGSUID, -1 };
	KncSend( PI_GLOBAL_AUTO_PARTY, KBaseServer::GetKObj()->GetUID(), PI_GS_CHARACTER, iUnitUID, anTrace, usEventID, data );
}

template < class T >
void KAutoPartyMakingProcess::BroadCastGameServer( IN unsigned short usEventID, IN const T& data )
{
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_GLOBAL_AUTO_PARTY, KBaseServer::GetKObj()->GetUID(), PI_GS_SERVER, 0, anTrace, usEventID, data );
}


//}}
