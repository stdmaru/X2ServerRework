#pragma once
#include "Event.h"
#include "CommonPacket.h"
//#include <boost/timer.hpp>
#include "TimerManager.h"
//#include "GameEvent\GameEventConnectTime.h"
//{{ 2009. 12. 7  최육사	누적시간이벤트
//#ifdef CUMULATIVE_TIME_EVENT
//	#include "GameEvent\GameEventCumulativeTime.h"
//#endif CUMULATIVE_TIME_EVENT
//}}

//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
	#include "GameEvent/GameEventManager.h"
//}}


SmartPointer(KGSUser);

class KUserEventManager
{
public:

	enum USER_EVENT_ENUM
	{
		EVENT_TIME_CONVERT = 60,
	};

	KUserEventManager(void);
	~KUserEventManager(void);

	void		Clear();

	//////////////////////////////////////////////////////////////////////////
	// 접속 시간 이벤트

	//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
	void		InitTimeEvent( IN KGameEventManager::ConnectEventFactorSet& kFactor );
	void		CheckConnectTimeEvent( IN KGSUserPtr spUser, IN KGameEventManager::ConnectEventFactorSet& kFactor );
	//}}

	//{{ 2010. 7. 27  최육사	접속 시간 이벤트 버그 수정
	void		GetEventTimerInfo( OUT std::map< int, KUserConnectTimeEventInfo >& mapEventTimer );			// 이벤트 타이머 정보 얻기
	void		SetEventTimerInfo( IN const std::map< int, KUserConnectTimeEventInfo >& mapEventTimer );	// 이벤트 타이머 정보 저장
	//}}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2009. 12. 7  최육사	누적시간이벤트
	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	void		CheckCumulativeTimeEvent( IN KGSUserPtr spUser );
		//}}

	void		GetDBUpdateInfo( OUT std::vector< KCumulativeTimeEventInfo >& vecEventTime );

	//{{ 2009. 12. 8  최육사	크리스마스이벤트
	bool		IsCompleteDirectRewardEvent() { return !m_setDirectRewardEvent.empty(); }
	void		DeleteDirectRewardAndGetDBUpdateInfo( OUT std::vector< KCumulativeTimeEventInfo >& vecCompleteEvent );
	void		RestartDirectRewardEvent( IN KGSUserPtr spUser, IN const std::vector< int >& vecRestartEvent );
	//}}

	//}}

	//////////////////////////////////////////////////////////////////////////
	// 공용 함수
	//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
	void		GetNowProcessingTimeEvent( OUT std::vector< KTimeEventSimpleInfo >& vecTimeEvent );
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

	void		CheckAdvertisementEvent( IN KGSUserPtr spUser );
	void		GetAdvertisementURL( OUT std::vector< std::wstring >& vecAdvertisementURL );
private:
	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >		m_mapConnectTimeEvent;		// 현재 진행중인 접속시간 이벤트
	//{{ 2009. 12. 7  최육사	누적시간이벤트
	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >	m_mapCumulativeTimeEvent;	// 현재 진행중인 누적시간 이벤트
	std::set< int >									m_setCompletedEvent;		// 완료한 이벤트
	std::set< int >									m_setDirectRewardEvent;		// 직접 보상 받아야하는 이벤트
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}
};
