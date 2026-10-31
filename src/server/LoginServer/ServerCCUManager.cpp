#include "LoginSimLayer.h"
#include "BaseServer.h"

#include "./ServerCCUManager.h"





ImplementSingleton( KServerCCUManager );

KServerCCUManager::KServerCCUManager(void)
{
	//m_tNextUpdateTime = CTime::GetCurrentTime();

	//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정
	m_tLastUpdateTime = CTime::GetCurrentTime();
	//}}

}

KServerCCUManager::~KServerCCUManager(void)
{
}

ImplToStringW( KServerCCUManager )
{
	return stm_;
}

void KServerCCUManager::Tick()
{
	//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정 ( 임규수 일본 변경 )
	if( m_kTimer[TM_CCU_UPDATE].elapsed() <= 40.0 )	// 시간 간격 줄이는게 중요
		return;
	//}}
	// DB에 동접 정보 업데이트
	DBUpdateCCUInfo();

	//{{ 2012. 07. 10	최육사		대기열 시스템
	if( m_kTimer[TM_FOR_WAIT_QUEUE].elapsed() > 1.0 )
	{
		CTime tCurrentTime = CTime::GetCurrentTime();
		KDBE_UPDATE_CCU_INFO_NOT kPacket;
		std::map< UidType, KUpdateCCUInfo >::const_iterator mit;
		for( mit = m_mapCCUInfo.begin(); mit != m_mapCCUInfo.end(); ++mit )
		{
			kPacket.m_vecCCUInfo.push_back( mit->second );
		}
		kPacket.m_wstrRegDate = ( CStringW )( tCurrentTime.Format( _T( "%Y-%m-%d %H:%M" ) ) );
		SendToLogDB( DBE_UPDATE_CCU_INFO_NOT, kPacket );

        m_kTimer[TM_FOR_WAIT_QUEUE].restart();
	}
	//}}
}

void KServerCCUManager::DBUpdateCCUInfo()
{
	// 현재 시각 -> DB의 현재 시각 정보를 넣자!
	// 현재 추측되는 상황으로는 CTime에 버그가 있어서 제대로 시각 정보를 못넣어주고 있다는점이 가장 유력한듯..
	m_kTimer[TM_CCU_UPDATE].restart(); // -- 타이머에 버그가 있다고 생각하였으나 타이머는 정상인것 같다.

	CTime tCurrentTime = CTime::GetCurrentTime();

	//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정
	// 같은 시각에 중복으로 입력하는 것을 막기 위해서
	if( tCurrentTime.GetMinute() == m_tLastUpdateTime.GetMinute() ){
		return;
	}

	m_tLastUpdateTime = tCurrentTime;
	//}}


	// 업데이트 패킷 만들기
	KDBE_UPDATE_CCU_INFO_NOT kPacket;

	//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정
	KUpdateCCUInfo kTotalCCUInfo;
	kTotalCCUInfo.m_iServerUID = 0;
	//}}

	std::map< UidType, KUpdateCCUInfo >::const_iterator mit;
	for( mit = m_mapCCUInfo.begin(); mit != m_mapCCUInfo.end(); ++mit )
	{
		kPacket.m_vecCCUInfo.push_back( mit->second );

		//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정
		kTotalCCUInfo.m_iConcurrentUser		+= mit->second.m_iConcurrentUser;
		kTotalCCUInfo.m_iCurPartyCount		+= mit->second.m_iCurPartyCount;
		kTotalCCUInfo.m_iPlayGamePartyCount += mit->second.m_iPlayGamePartyCount;
		//}}

	}

	//{{ 2011.10.12 조효진 동접 정보 버그 수정 및 전체 동접 서버에서 남기도록 수정
	kPacket.m_vecCCUInfo.push_back( kTotalCCUInfo );
	//}}

	// 체험ID 동접
	if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_NEXON_KOREA )
	{
		KUpdateCCUInfo kGuestCCUInfo;
		kGuestCCUInfo.m_iServerUID			= KBaseServer::GetKObj()->GetUID();
		kGuestCCUInfo.m_iConcurrentUser		= GetKLoginSimLayer()->GetGuestConcurrentUserCount(); // 체험ID 동접
		kGuestCCUInfo.m_iCurPartyCount		= 0;
		kGuestCCUInfo.m_iPlayGamePartyCount = 0;
		kPacket.m_vecCCUInfo.push_back( kGuestCCUInfo );
	}

	// 다음 업데이트 시각과 현재 시각이 차이가 난다면 보정값을 넣자!
	//if( tCurrentTime.GetMinute() != m_tNextUpdateTime.GetMinute() )
	//{
	//	kPacket.m_wstrRegDate = ( CStringW )( m_tNextUpdateTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	//	SendToLogDB( DBE_UPDATE_CCU_INFO_NOT, kPacket );

	//	START_LOG( cout, L"동접 정보 업데이트 타임 1분 누락 발생! 보정값을 업데이트 합니다!" )
	//		<< BUILD_LOG( tCurrentTime.GetMinute() )
	//		<< BUILD_LOG( m_tNextUpdateTime.GetMinute() )
	//		<< BUILD_LOG( kPacket.m_wstrRegDate );
	//}

	// 업데이트 패킷
	kPacket.m_wstrRegDate = ( CStringW )( tCurrentTime.Format( _T( "%Y-%m-%d %H:%M" ) ) );
	SendToLogDB( DBE_UPDATE_CCU_INFO_NOT, kPacket );

	// 동접 정보를 비운다.
	m_mapCCUInfo.clear();

	// 다음 업데이트 시각 저장
	//m_tNextUpdateTime = tCurrentTime += CTimeSpan( 0, 0, 1, 0 );

}

void KServerCCUManager::UpdateCCUInfo( IN const KUpdateCCUInfo& kInfo )
{
	std::map< UidType, KUpdateCCUInfo >::iterator mit;
	mit = m_mapCCUInfo.find( kInfo.m_iServerUID );
	if( mit == m_mapCCUInfo.end() )
	{
		m_mapCCUInfo.insert( std::make_pair( kInfo.m_iServerUID, kInfo ) );
	}
	else
	{
        mit->second = kInfo;
	}
}


void KServerCCUManager::SendToLogDB( unsigned short usEventID )
{
	SendToLogDB( usEventID, char() );
}
