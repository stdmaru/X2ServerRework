#pragma once

#include "ServerDefine.h"
#include "GameEvent.h"
//#include "ServerDefine.h"
//#include "CommonPacket.h"
//#include "TimerManager.h"

//{{ 2009. 10. 26  최육사	PC방이벤트
//struct SUserConnectTimeEventInfo
//{
//	KUserConnectTimeEventInfo	m_kInfo;
//	KTimerManager				m_kEventTimer;
//};
//}}

class KGameEventConnectTime : public KGameEvent
{
public:
	KGameEventConnectTime(void);
	virtual ~KGameEventConnectTime(void);

	void	SetEventTime( float fEventTime )		{ m_fEventTime = fEventTime; } // 이벤트 타임 단위는 분단위입니다.
	float	GetEventTime()							{ return m_fEventTime; }

	void	SetEventID( int iEventID )				{ m_iEventID = iEventID; }
	int		GetEventID()							{ return m_iEventID; }

	void	SetRewardID( int iRewardID )			{ m_iRewardID = iRewardID; }
	int		GetRewardID()							{ return m_iRewardID; }

	void	SetGiveOnce( bool bGiveOnce )			{ m_bGiveOnce = bGiveOnce; }
	bool	IsGiveOnce()							{ return m_bGiveOnce; }

	void	SetDelEndEvent( bool bDelEndEvent ) 	{ m_bDelEndEvent = bDelEndEvent; }
	bool	IsDelEndEvent()							{ return m_bDelEndEvent; }

	//{{ 2009. 10. 26  최육사	PC방이벤트
	void	SetPcBangEvent( bool bPcBangEvent ) 	{ m_bPcBangEvent = bPcBangEvent; }
	bool	IsPcBangEvent()							{ return m_bPcBangEvent; }
	//}}

	//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
	void	SetAccountEvent( bool bAccountEvent )	{ m_bAccountEvent = bAccountEvent; }
	bool	IsAccountEvent()						{ return m_bAccountEvent; }
	//}}

	//{{ 2009. 12. 16  최육사	이벤트개편
	void	SetUnitLevel( int iUnitLevel )			{ m_iUnitLevel = iUnitLevel; }
	int		GetUnitLevel()							{ return m_iUnitLevel; }
	//}}
	//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
	bool	IsMinMaxReward() const					{ return ( m_mapMinMaxReward.empty() == false ); }
	int		GetMinMaxRewardSize() const				{ return static_cast<int>(m_mapMinMaxReward.size()); }
	void	SetMinMaxReward( const std::map< std::pair< int, int >, int >& mapMinMaxReward ) { m_mapMinMaxReward = mapMinMaxReward; }
	int		GetMinMaxRewardID( const int iLevel ) const;
	//}}
	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
	void	SetChannelCode( int iChannelCode )		{ m_iChannelCode = iChannelCode; }
	int		GetChannelCode()						{ return m_iChannelCode; }
	//}}
	//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
	//}}
	//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
	void	SetUnitType( char cUnitType )				{ m_cUnitType = cUnitType; }
	char	GetUnitType()								{ return m_cUnitType; }
	void	SetUnitClass( char cUnitClass )				{ m_cUnitClass = cUnitClass; }
	char	GetUnitClass()								{ return m_cUnitClass; }
	//}}
	//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
	void	SetRepeatEvent( bool bRepeat )				{ m_bRepeatEvent = bRepeat; }
	bool	GetRepeatEvent()							{ return m_bRepeatEvent; }
	//}}

	void	SetRepeatRewardEvent( bool bRepeatRewardEvent )	{ m_bRepeatRewardEvent = bRepeatRewardEvent; }
	bool	IsRepeatRewardEvent()							{ return m_bRepeatRewardEvent; }

	void	SetBeforeEventID( int iBeforeEventID ) { m_iBeforeEventID = iBeforeEventID; }
	int		GetBeforeEventID()								{ return m_iBeforeEventID; }

	void	SetEventItemID( int iEventItemID )				{ m_iEventItemID = iEventItemID; }
	int		GetEventItemID()							{ return m_iEventItemID; }


	//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
	void	SetEventGroupID( IN const int iEventGroupID ){	m_iEventGroupID = iEventGroupID;	}
	int		GetEventGroupID( void ) const	{ return m_iEventGroupID;	}
	//}}


	void	SetOnlySteamUserEvent( bool bVal_ ) { m_bOnlySteamUserEvent = bVal_; }
	void	SetOnlyNotSteamUserEvent( bool bVal_ ) { m_bOnlyNotSteamUserEvent = bVal_; }
	bool	IsOnlySteamUserEvent() const { return m_bOnlySteamUserEvent; }
	bool	IsOnlyNotSteamUserEvent() const { return m_bOnlyNotSteamUserEvent; }


	void	SetUnitClassLevel( int iUnitClassLevel )	{ m_iUnitClassLevel = iUnitClassLevel; }
	int		GetUnitClassLevel()							{ return m_iUnitClassLevel; }

private:
	float	m_fEventTime;
	int		m_iEventID;
	int		m_iRewardID;
	bool	m_bGiveOnce;
	bool	m_bDelEndEvent;
	//{{ 2009. 10. 26  최육사	PC방이벤트
	bool	m_bPcBangEvent;
	//}}
	//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
	bool	m_bAccountEvent;
	//}}
	//{{ 2009. 12. 16  최육사	이벤트개편
	int		m_iUnitLevel;
	//}}
	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
	int		m_iChannelCode;
	//}}
	//{{  2011.11.08     김민성    버블파이터 공동 프로모션 이벤트
	//}}
	//{{ 2011. 05. 31	최육사	접속 이벤트 레벨 체크
	std::map< std::pair< int, int >, int > m_mapMinMaxReward;
	//}}
	//{{  2011.11.30     김민성    캐릭터별 접속 이벤트
	char		m_cUnitType;
	char		m_cUnitClass;
	//}}
	//{{ 2012. 06. 29	김민성       접속 시간에 따라 아이템 반복 지급
	bool	m_bRepeatEvent;
	//}}
	//{{ 2012. 12. 25	박세훈	특정 유저 전용 접속 이벤트
	int				m_iEventGroupID;
	//}}

	bool	m_bRepeatRewardEvent;
	int		m_iBeforeEventID;

	int			m_iEventItemID;



	bool	m_bOnlySteamUserEvent;
	bool	m_bOnlyNotSteamUserEvent;


	int		m_iUnitClassLevel;
};
