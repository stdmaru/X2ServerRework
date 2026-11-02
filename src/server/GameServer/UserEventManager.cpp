#include "GSUser.h"
#include "./UserEventManager.h"
//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
//}}



KUserEventManager::KUserEventManager(void)
{
	Clear();
}

KUserEventManager::~KUserEventManager(void)
{
}

void KUserEventManager::Clear()
{
	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

	m_mapConnectTimeEvent.clear();
	//{{ 2009. 12. 7  최육사	누적시간이벤트
	m_mapCumulativeTimeEvent.clear();
	m_setCompletedEvent.clear();
	m_setDirectRewardEvent.clear();
	//}}
}

//{{ 2012. 10. 13	박세훈	접속 이벤트 인자 정리
void KUserEventManager::InitTimeEvent( IN KGameEventManager::ConnectEventFactorSet& kFactor )
{
	if( ( kFactor.m_pVecConnectTimeEvent == NULL ) ||
		( kFactor.m_pVecCumulativeTimeEvent == NULL ) )
	{
		START_LOG( cerr, L"Invalid connection event argument; this should never happen. Check immediately." )
			<< BUILD_LOG( kFactor.m_pVecConnectTimeEvent )
			<< BUILD_LOG( kFactor.m_pVecCumulativeTimeEvent )
			<< END_LOG;
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 1. 이벤트 정보 초기화
	Clear();

	//////////////////////////////////////////////////////////////////////////
	// 2. DB로부터 받아온 유닛별 접속 시간 이벤트 정보 저장

	CTime tEventTime;
	CTime tCurTime = CTime::GetCurrentTime();

	// 누적 시간 이벤트 정보
	for( u_int ui = 0; ui < kFactor.m_pVecCumulativeTimeEvent->size(); ++ui )
	{
		std::pair< KUserCumulativeTimeEventInfo, KTimerManager > kUserCumulativeEvent;
		kUserCumulativeEvent.first.m_iEventUID = ( *kFactor.m_pVecCumulativeTimeEvent )[ui].m_iEventUID;
		kUserCumulativeEvent.second.restart();
		kUserCumulativeEvent.second.SetAddTime( static_cast<double>( ( *kFactor.m_pVecCumulativeTimeEvent )[ui].m_iCumulativeTime * 60 ) );
		m_mapCumulativeTimeEvent.insert( std::make_pair( ( *kFactor.m_pVecCumulativeTimeEvent )[ui].m_iEventUID, kUserCumulativeEvent ) );
	}

	// 접속 시간 이벤트 정보
	for( u_int ui = 0; ui < kFactor.m_pVecConnectTimeEvent->size(); ++ui )
	{
		if( !KncUtil::ConvertStringToCTime( ( *kFactor.m_pVecConnectTimeEvent )[ui].m_wstrEventTime, tEventTime ) )
		{
			START_LOG( cerr, L"Failed to convert the time string." )
				<< BUILD_LOG( ( *kFactor.m_pVecConnectTimeEvent )[ui].m_wstrEventTime )
				<< END_LOG;

			// 문자열 변환 실패시 보상 주지 않는것으로 처리
			tEventTime = CTime( 2050, tCurTime.GetMonth(), tCurTime.GetDay(), 23, 59, 59 );
		}

		// 이미 이벤트 보상을 받았는가?
		if( tCurTime <= tEventTime )
		{
			// 완료 리스트에 넣는다.
			m_setCompletedEvent.insert( ( *kFactor.m_pVecConnectTimeEvent )[ui].m_iEventUID );
			continue;
		}

		// 시간 이벤트 리스트에서 누적 시간 이벤트가 아닌것만 찾아서 저장한다.
		if( m_mapCumulativeTimeEvent.find( ( *kFactor.m_pVecConnectTimeEvent )[ui].m_iEventUID ) == m_mapCumulativeTimeEvent.end() )
		{
			std::pair< KUserConnectTimeEventInfo, KTimerManager > kUserConnectEvent;
			kUserConnectEvent.first.m_fEventTime = END_EVENT; // 기본값으로 종료된 이벤트값을 넣는다
			kUserConnectEvent.first.m_kCTime		= tEventTime.GetTime();
			kUserConnectEvent.first.m_bGiveOnce	= true;
			kUserConnectEvent.second.restart(); // 타이머 초기화
			m_mapConnectTimeEvent.insert( std::make_pair( ( *kFactor.m_pVecConnectTimeEvent )[ui].m_iEventUID, kUserConnectEvent ) );
		}
	}

	kFactor.m_pMapEventTime			= &m_mapConnectTimeEvent;
	kFactor.m_pSetCompletedEvent	= &m_setCompletedEvent;

	// 3. 현재 진행중인 이벤트를 체크하여 EventTime을 얻는다.
	SiKGameEventManager()->CheckEnableConnectTimeEventForCharInit( kFactor );

	// 4. 현재 진행중인 이벤트를 체크하여 EventTime을 얻는다.
	SiKGameEventManager()->CheckEnableCumulativeTimeEventForCharInit(
		kFactor.m_ucLevel,
		kFactor.m_bIsPcBang

		//{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
		, kFactor.m_iChannelCode
		//}}
		//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
		//}}

		, m_mapCumulativeTimeEvent
		, m_setCompletedEvent
		);

	for( u_int ui = 0; ui < kFactor.m_pVecCompletedAdvertisementEvent->size(); ++ui )
	{
		if( m_setCompletedEvent.find( ( *kFactor.m_pVecCompletedAdvertisementEvent )[ui] ) == m_setCompletedEvent.end() )
		{
			m_setCompletedEvent.insert( ( *kFactor.m_pVecCompletedAdvertisementEvent )[ui] );
		}
	}
}

void KUserEventManager::CheckConnectTimeEvent( IN KGSUserPtr spUser, IN KGameEventManager::ConnectEventFactorSet& kFactor )
{
	if( spUser == NULL )
		return;

	KDBE_UPDATE_EVENT_TIME_REQ kPacketReq;
	KEGS_TIME_EVENT_INFO_NOT kPacketNotBegin;
	KEGS_TIME_EVENT_INFO_NOT kPacketNotEnd;

	kFactor.m_pMapEventTime			= &m_mapConnectTimeEvent;
	kFactor.m_pSetCompletedEvent	= &m_setCompletedEvent;
	kFactor.m_pVecUpdatedTime		= &kPacketReq.m_vecConnectTimeEvent;
	kFactor.m_pVecBeginEvent		= &kPacketNotBegin.m_vecTimeEvent;
	kFactor.m_pVecEndEvent			= &kPacketNotEnd.m_vecTimeEvent;

	// 이벤트 보상 받을 시간이 다 되었는지 체크, 종료되는 이벤트 체크, 새로 시작하는 이벤트 체크
	SiKGameEventManager()->CheckConnectTimeEvent( kFactor );

	// 접속시간을 충족시켜서 보상받을 이벤트가 있는지?
	if( !kPacketReq.m_vecConnectTimeEvent.empty() )
	{
		// 접속 시간 이벤트 보상 받으러 가자!
		kPacketReq.m_iUnitUID = spUser->GetCharUID();
		spUser->SendToGameDB( DBE_UPDATE_EVENT_TIME_REQ, kPacketReq );
	}

	// 종료된 이벤트가 있는지?
	if( !kPacketNotEnd.m_vecTimeEvent.empty() )
	{
		kPacketNotEnd.m_cNotifyType = KEGS_TIME_EVENT_INFO_NOT::TENT_END;
		spUser->SendPacket( EGS_TIME_EVENT_INFO_NOT, kPacketNotEnd );
	}

	// 새로 시작되는 이벤트가 있는지?
	if( !kPacketNotBegin.m_vecTimeEvent.empty() )
	{
		kPacketNotBegin.m_cNotifyType = KEGS_TIME_EVENT_INFO_NOT::TENT_BEGIN;
		spUser->SendPacket( EGS_TIME_EVENT_INFO_NOT, kPacketNotBegin );
	}
}
//}}

//{{ 2010. 7. 27  최육사	접속 시간 이벤트 버그 수정

void KUserEventManager::GetEventTimerInfo( OUT std::map< int, KUserConnectTimeEventInfo >& mapEventTimer )
{
	CTime tCurrentTime = CTime::GetCurrentTime();

	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = m_mapConnectTimeEvent.begin(); mit != m_mapConnectTimeEvent.end(); ++mit )
	{
		const KUserConnectTimeEventInfo& kConnectEventInfo = mit->second.first;
		const KTimerManager& kEventTimer = mit->second.second;

		// 1. 이미 보상을 받았는지 검사
		if( tCurrentTime.GetTime() <= kConnectEventInfo.m_kCTime )
			continue;

		// 2. END_EVENT값은 이미 '끝나버린 이벤트'임.
		if( kConnectEventInfo.m_fEventTime <= END_EVENT )
			continue;

		// 3. 현재 까지 진행한 이벤트 타이머 얻기
		KUserConnectTimeEventInfo kInfo;
		kInfo = kConnectEventInfo;
		kInfo.m_fBackUpTime = kEventTimer.elapsed();
		mapEventTimer.insert( std::make_pair( mit->first, kInfo ) );
	}
}

void KUserEventManager::SetEventTimerInfo( IN const std::map< int, KUserConnectTimeEventInfo >& mapEventTimer )
{
	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::iterator mitFind;
	std::map< int, KUserConnectTimeEventInfo >::const_iterator mit;
	for( mit = mapEventTimer.begin(); mit != mapEventTimer.end(); ++mit )
	{
		mitFind = m_mapConnectTimeEvent.find( mit->first );
		if( mitFind == m_mapConnectTimeEvent.end() )
		{
			// 이벤트가 진행중이 아니면 옛날 채널에서 직접 받아온걸 넣자!
			std::pair< KUserConnectTimeEventInfo, KTimerManager > pairTimeEvent;
			pairTimeEvent.first = mit->second;
			pairTimeEvent.second.SetAddTime( mit->second.m_fBackUpTime );

			m_mapConnectTimeEvent.insert( std::make_pair( mit->first, pairTimeEvent ) );
		}
		else
		{
			// 진행중인 이벤트 정보가 있다면 시간만 연장 하자!
			KTimerManager& kEventTimer = mitFind->second.second;

			kEventTimer.SetAddTime( mit->second.m_fBackUpTime );
		}
	}
}

//}}

//{{ 2009. 12. 7  최육사	누적시간이벤트
//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
void KUserEventManager::CheckCumulativeTimeEvent( IN KGSUserPtr spUser )
//}}
{
	if( spUser == NULL )
		return;

	KDBE_UPDATE_EVENT_TIME_REQ kPacketReq;
	KEGS_TIME_EVENT_INFO_NOT kPacketNotBegin;
	KEGS_TIME_EVENT_INFO_NOT kPacketNotEnd;

	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	//}}

	// 이벤트 보상 받을 시간이 다 되었는지 체크, 종료되는 이벤트 체크, 새로 시작하는 이벤트 체크
	SiKGameEventManager()->CheckCumulativeTimeEvent( spUser->GetLevel(),
													 spUser->IsPcBang(),
													 //{{ 2011. 08. 09	최육사	투니 랜드 채널링 이벤트
													 spUser->GetChannelCode(),
													 //}}
													 //{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
													 //}}
													 m_mapCumulativeTimeEvent,
													 m_setCompletedEvent,
													 m_setDirectRewardEvent,
													 kPacketReq.m_vecCumulativeTimeEvent,
													 kPacketNotBegin.m_vecTimeEvent,
													 kPacketNotEnd.m_vecTimeEvent );

	// 접속시간을 충족시켜서 보상받을 이벤트가 있는지?
	if( !kPacketReq.m_vecCumulativeTimeEvent.empty() )
	{
		// 접속 시간 이벤트 보상 받으러 가자!
		kPacketReq.m_iUnitUID = spUser->GetCharUID();
		spUser->SendToGameDB( DBE_UPDATE_EVENT_TIME_REQ, kPacketReq );
	}

	// 새로 시작되는 이벤트가 있는지?
	if( !kPacketNotBegin.m_vecTimeEvent.empty() )
	{
		kPacketNotBegin.m_cNotifyType = KEGS_TIME_EVENT_INFO_NOT::TENT_BEGIN;
		spUser->SendPacket( EGS_TIME_EVENT_INFO_NOT, kPacketNotBegin );
	}

	// 종료된 이벤트가 있는지?
	if( !kPacketNotEnd.m_vecTimeEvent.empty() )
	{
		kPacketNotEnd.m_cNotifyType = KEGS_TIME_EVENT_INFO_NOT::TENT_END;
		spUser->SendPacket( EGS_TIME_EVENT_INFO_NOT, kPacketNotEnd );
	}
}

void KUserEventManager::GetDBUpdateInfo( OUT std::vector< KCumulativeTimeEventInfo >& vecEventTime )
{
	vecEventTime.clear();

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = m_mapCumulativeTimeEvent.begin(); mit != m_mapCumulativeTimeEvent.end(); ++mit )
	{
		// 초기화가 되지 않은 이벤트는 이미 종료된 이벤트이므로 DB에 업데이트 할 필요가 없다.
		if( mit->second.first.m_bInit == false )
			continue;

		// 완료한 이벤트는 DB에 업데이트할 필요가 없다.
		if( m_setCompletedEvent.find( mit->first ) != m_setCompletedEvent.end() )
			continue;

		const KTimerManager& kEventTimer = mit->second.second;

		KCumulativeTimeEventInfo kInfo;
		kInfo.m_iEventUID = mit->first;
		kInfo.m_iCumulativeTime = static_cast<int>( kEventTimer.elapsed() / 60.0 );
		//{{ 2010. 06. 15  최육사	계정단위 접속시간 이벤트
		kInfo.m_bAccountEvent = mit->second.first.m_bAccountEvent;
		//}}
		vecEventTime.push_back( kInfo );
	}
}

void KUserEventManager::DeleteDirectRewardAndGetDBUpdateInfo( OUT std::vector< KCumulativeTimeEventInfo >& vecCompleteEvent )
{
	vecCompleteEvent.clear();

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::iterator mit;
	std::set< int >::const_iterator sit;
	for( sit = m_setDirectRewardEvent.begin(); sit != m_setDirectRewardEvent.end(); ++sit )
	{
		mit = m_mapCumulativeTimeEvent.find( *sit );
		if( mit == m_mapCumulativeTimeEvent.end() )
			continue;

		KUserCumulativeTimeEventInfo& kCumulativeEventInfo = mit->second.first;

		// DB에 업데이트할 정보 저장
		KCumulativeTimeEventInfo kInfo;
		kInfo.m_iEventUID = kCumulativeEventInfo.m_iEventUID;
		kInfo.m_iCumulativeTime = static_cast<int>(kCumulativeEventInfo.m_fEventTime);
		vecCompleteEvent.push_back( kInfo );

		// 진행중인 이벤트에서 삭제
		m_mapCumulativeTimeEvent.erase( mit );

		// 완료 이벤트에 추가
		m_setCompletedEvent.insert( *sit );
	}

	// DirectReward 초기화
	m_setDirectRewardEvent.clear();
}

void KUserEventManager::RestartDirectRewardEvent( IN KGSUserPtr spUser, IN const std::vector< int >& vecRestartEvent )
{
	if( spUser == NULL )
		return;

	std::vector< int >::const_iterator vit;
	for( vit = vecRestartEvent.begin(); vit != vecRestartEvent.end(); ++vit )
	{
		// 완료 이벤트에서 삭제 한다.
		m_setCompletedEvent.erase( *vit );
	}

	// 이벤트를 재시작하기위해 이벤트 체크 함수 호출
	//{{ 2012. 12. 12	박세훈	겨울 방학 전야 이벤트( 임시, 하드 코딩 )
	CheckCumulativeTimeEvent( spUser );
	//}}
}

//}}

//{{ 2009. 12. 4  최육사	접속시간이벤트시스템개편
void KUserEventManager::GetNowProcessingTimeEvent( OUT std::vector< KTimeEventSimpleInfo >& vecTimeEvent )
{
	vecTimeEvent.clear();

	std::map< int, std::pair< KUserConnectTimeEventInfo, KTimerManager > >::const_iterator mit;
	for( mit = m_mapConnectTimeEvent.begin(); mit != m_mapConnectTimeEvent.end(); ++mit )
	{
		const KUserConnectTimeEventInfo& kConnectEventInfo = mit->second.first;

		KTimeEventSimpleInfo kInfo;
		kInfo.m_iEventUID	  = mit->first;
		kInfo.m_iEventID	  = kConnectEventInfo.m_iEventID;
		kInfo.m_wstrEventName = kConnectEventInfo.m_wstrEventName;
		kInfo.m_fEventTime	  = kConnectEventInfo.m_fEventTime;
		vecTimeEvent.push_back( kInfo );
	}

	std::map< int, std::pair< KUserCumulativeTimeEventInfo, KTimerManager > >::const_iterator mitCT;
	for( mitCT = m_mapCumulativeTimeEvent.begin(); mitCT != m_mapCumulativeTimeEvent.end(); ++mitCT )
	{
		// 완료한 이벤트는 클라이언트에 줄필요 없다.
		if( m_setCompletedEvent.find( mitCT->first ) != m_setCompletedEvent.end() )
			continue;

		const KUserCumulativeTimeEventInfo& kCumulativeEventInfo = mitCT->second.first;
		const KTimerManager& kEventTimer = mitCT->second.second;

		// 초기화 되지 않은 이벤트는 클라이언트에 줄필요 없다.
		if( !kCumulativeEventInfo.m_bInit )
			continue;

		KTimeEventSimpleInfo kInfo;
		kInfo.m_iEventUID	  = mitCT->first;
		kInfo.m_iEventID	  = kCumulativeEventInfo.m_iEventID;
		kInfo.m_wstrEventName = kCumulativeEventInfo.m_wstrEventName;
		kInfo.m_fEventTime	  = kCumulativeEventInfo.m_fEventTime - static_cast<float>( kEventTimer.elapsed() / 60.0 );
		if( kInfo.m_fEventTime < 0.0f )
			kInfo.m_fEventTime = 0.0f;
		vecTimeEvent.push_back( kInfo );
	}
}
//}}

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

void KUserEventManager::CheckAdvertisementEvent( IN KGSUserPtr spUser )
{
	if( spUser == NULL )
		return;

	KEGS_ADVERTISEMENT_EVENT_INFO_NOT kPacketNotBegin;
	KEGS_ADVERTISEMENT_EVENT_INFO_NOT kPacketNotEnd;
	KDBE_INSERT_ADVERTISEMENT_EVENT_INFO_NOT kPacketDB;

	SiKGameEventManager()->CheckEnableAdvertisementEvent( m_setCompletedEvent, kPacketNotBegin.m_vecTimeEvent, kPacketNotEnd.m_vecTimeEvent );

	// 새로 시작되는 이벤트가 있다면 클라이언트에 알리고
	if( !kPacketNotBegin.m_vecTimeEvent.empty() )
	{
		kPacketNotBegin.m_cNotifyType = KEGS_TIME_EVENT_INFO_NOT::TENT_BEGIN;
		spUser->SendPacket( EGS_ADVERTISEMENT_EVENT_INFO_NOT, kPacketNotBegin );

		for( u_int ui = 0; ui < kPacketNotBegin.m_vecTimeEvent.size(); ++ui )
		{
			m_setCompletedEvent.insert( kPacketNotBegin.m_vecTimeEvent[ui].m_iEventUID );
			kPacketDB.m_vecCompletedEvent.push_back( kPacketNotBegin.m_vecTimeEvent[ui].m_iEventUID );
		}

		kPacketDB.m_iUserUID = spUser->GetUID();
				spUser->SendToGameDB( DBE_INSERT_ADVERTISEMENT_EVENT_INFO_NOT, kPacketDB );
	}
}

void KUserEventManager::GetAdvertisementURL( OUT std::vector< std::wstring >& vecAdvertisementURL )
{
	SiKGameEventManager()->GetAdvertisementURL( vecAdvertisementURL );
}
