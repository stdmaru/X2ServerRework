#pragma once

#include "GameEvent.h"
//#include "TimerManager.h"

//{{ 2009. 10. 26  최육사	PC방이벤트
//struct KUserCumulativeTimeEventInfo
//{
//	int				m_iEventUID;
//	int				m_iEventID;
//	bool			m_bInit;
//	std::wstring	m_wstrEventName;
//	float			m_fEventTime;
//	KTimerManager	m_kEventTimer;
//	bool			m_bDirectReward;
//
//	KUserCumulativeTimeEventInfo()
//	{
//		m_iEventUID		= 0;
//		m_iEventID		= 0;
//		m_bInit			= false;
//		m_fEventTime	= 0.0f;
//		m_bDirectReward = false;
//	}
//};
//}}

class KGameEventCumulativeTime : public KGameEvent
{
public:
	KGameEventCumulativeTime(void);
	virtual ~KGameEventCumulativeTime(void);

	void	SetEventTime( float fEventTime )	{ m_fEventTime = fEventTime; } // 이벤트 타임 단위는 분단위입니다.
	float	GetEventTime()						{ return m_fEventTime; }

	void	SetEventID( int iEventID )			{ m_iEventID = iEventID; }
	int		GetEventID()						{ return m_iEventID; }

	void	SetRewardID( int iRewardID )		{ m_iRewardID = iRewardID; }
	int		GetRewardID()						{ return m_iRewardID; }

	//{{ 2009. 10. 26  최육사	PC방이벤트
	void	SetPcBangEvent( bool bPcBangEvent )	{ m_bPcBangEvent = bPcBangEvent; }
	bool	IsPcBangEvent()						{ return m_bPcBangEvent; }
	//}}

	//{{ 2009. 12. 8  최육사	접속시간이벤트
	void	SetDirectReward( bool bVal )		{ m_bDirectReward = bVal; }
	bool	IsDirectReward()					{ return m_bDirectReward; }
	//}}

	//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
	void	SetAccountEvent( bool bAccountEvent )	{ m_bAccountEvent = bAccountEvent; }
	bool	IsAccountEvent()						{ return m_bAccountEvent; }
	//}}
	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
	void	SetChannelCode( int iChannelCode )		{ m_iChannelCode = iChannelCode; }
	int		GetChannelCode()						{ return m_iChannelCode; }
	//}}



private:
	float	m_fEventTime;
	int		m_iEventID;
	int		m_iRewardID;
	//{{ 2009. 10. 26  최육사	PC방이벤트
	bool	m_bPcBangEvent;
	//}}
	//{{ 2009. 12. 8  최육사	접속시간이벤트
	bool	m_bDirectReward;
	//}}
	//{{ 2010. 06. 11  최육사	계정단위 접속시간 이벤트
	bool	m_bAccountEvent;
	//}}
	//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
	int		m_iChannelCode;
	//}}

};
