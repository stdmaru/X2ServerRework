//{{2011. 04. 13  김민성  글로벌 서버 추가

#pragma once

#include "RefreshSingleton.h"
#include "DBLayer.h"
#include <KncSend.h>
#include <atltime.h>
#include <boost/timer.hpp>
#include "CommonPacket.h"
#include "ServerPacket.h"

//{{ 공존의 축제 - 지진 효과 - 김민성
//}}

//{{ 2013. 04. 13	박세훈	어둠의 문 개편
class KWorldMissionManager
{
	DeclareRefreshSingleton( KWorldMissionManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	enum DEFENSE_REWARD_BUFF_LEVEL_NEEDS
	{
		DEFENSE_REWARD_BUFF_LEVEL1_NEEDS = 0,
		DEFENSE_REWARD_BUFF_LEVEL2_NEEDS = 3,
		DEFENSE_REWARD_BUFF_LEVEL3_NEEDS = 9,
		DEFENSE_REWARD_BUFF_LEVEL4_NEEDS = 15,
		DEFENSE_REWARD_BUFF_MAX_NEEDS = 16,
	};

public:
	KWorldMissionManager(void);
	~KWorldMissionManager(void);

	template < class T > void SendToLogDB( unsigned short usEventID, const T& data );

	void	Tick( void );
	void	Init( void );

	void	SetMissionSuccess_LUA(bool bSuccess);	//	치트용
	void	SetDefenseOpenTime_LUA( int iDayOfWeek, int iStartHour, int iStartMin, int iDurationTime );
	void	SetWorldBuffDurationTime_LUA( int iBuffDurationTime )		{ m_usWorldBuffDurationTime = static_cast<u_short>(iBuffDurationTime); }

	void	WorldMissionDump_LUA();

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

	void	SetReduceTerm_LUA( float fReduceTerm ){	m_fReduceTerm = fReduceTerm;	}
	float	GetReduceTerm( void ) const{	return m_fReduceTerm;	}

	int		GetDayOfTheWeek( void ) const{	return m_tToDayMissionTime.GetDayOfWeek();	}
	bool	SetTodayMissionTime( void );
	void	CheckWorldMissionProcess( void );

	void	SetActive( IN const bool bActive ){ m_bActive = bActive; }
	bool	IsActive( void ) const{ return m_bActive; }

	void	InitMissionData( void );
	void	SetMissionData( void );
	void	StartMission( IN const bool IsReStart = false );
	void	EndMission( void );

	void	BroadCastMission( IN const KEGB_UPDATE_MISSION_INFO_NOT::FLAG_TYPE eType );
	void	BroadCastWorldBuff( IN const int _skill );
	void	BroadCastMissionStartTime( void );

	void	IncreaseProtectedCrystalCount( IN const int iProtectedCrystalCount );
	int		GetProtectedCrystalCount( void ) const{	return m_iProtectedCrystalCount;	}
	void	ResetProtectedCrystalCount( void );

	void	GetWorldMissionInfo( IN const int& iType, OUT KEGB_GET_MISSION_INFO_ACK &kPacketAck ) const;

	void	SetIsFirstTick( IN const bool IsFirst ){	m_bIsFirstTick = IsFirst;	}
	bool	GetIsFirstTick( void ) const{	return m_bIsFirstTick;	}
	int		GetCorrectMissionTime( void ) const;
	CTime	GetToDayMissionTime( void ) const{	return m_tToDayMissionTime;	}

	void	WriteWorldMissionCrystalCount( IN const bool bforce = false );
	void	SetInitMissionInfo( IN const KDBE_GET_DEFENCE_CRYSTAL_ACK& KPacket );

	void	SetMissionTime( IN const int iTime ){	m_iMissionTime = iTime;	}
	bool	GetWorldMissionState( void ) const{	return m_bWorldMissionOn;	}

	void	BroadCastIndividualBuffLevel( IN const int& iIndividualBuffLevel ) const;

	int		CheckWorldBuff( void ) const;
	int		CheckIndividualBuff( void ) const;

	void	BuffProcess( void );

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

private:
	bool			m_bIsFirstTick;

	boost::timer	m_kTimerDOT;			//	던전오픈 시간 체크를 위한 타이머
	boost::timer	m_kTimerEnableCheck;	//	일정 주기 체크를 위한 타이머
	boost::timer	m_kTimerUpdate;			//	1분에 한번씩 게임서버로 현재상태 전달
	boost::timer	m_kTimerWorldBuff;		//	월드 버프 종료 시간 용
	boost::timer	m_kTimerForReduceTheProtectedCrystalCount;		//	분당 소모되는 카운트를 위한 타이머

	bool			m_bActive;				//	현재 던전 개방되어 있지 여부
	bool			m_bWorldBuffTime;		//  월드 버프 시간 용
	int				m_iMissionTime;		//	월드미션 진행시간(단위 : 초)
	int				m_iNextDOT;			//	다음에 개방될 던전 대기시간(단위 : 초)
	int				m_iTotalCCU;

	//	현재 진행 미션
	int				m_iStartedCrystalCount;
	int				m_iProtectedCrystalCount;	//  보호된 전체 수정 수
	int				m_iReduceCount;				//	분당 소모되는 카운트	: m_iTotalCCU / 60

	CTime													m_tToDayMissionTime;	// 스크립드상의 던전 오픈 시간
	CTime													m_tToDayStartTime;		// 실제 던전 오픈된 시간
	std::map< int, std::vector< KDefenseOpenTime > >		m_mapDefenseOpenTime;	// 요일별 오픈 시간
	bool													m_bWorldMissionOn;		// 월드 미션을 하는가?

	u_short			m_usWorldBuffDurationTime;	//  월드 버프 지속 시간

	int				m_iWorldBuffID;
	int				m_iIndividualBuffID;

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

	float			m_fReduceTerm;
};
//}}

DefRefreshSingletonInline( KWorldMissionManager );

template < class T >
void KWorldMissionManager::SendToLogDB( unsigned short usEventID, const T& data )
{
	KncSend( PI_GLOBAL_SERVER, KBaseServer::GetKObj()->GetUID(), PI_LOG_DB, 0, NULL, usEventID, data );
}

//}} 2011. 04. 13  김민성  글로벌 서버 추가
