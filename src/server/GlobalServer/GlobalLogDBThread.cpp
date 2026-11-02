#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "GlobalLogDBThread.h"
#include "GlobalServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "Enum/Enum.h"

#define CLASS_TYPE KGlobalLogDBThread
ImplPfID( KGlobalLogDBThread, PI_GLOBAL_LOG_DB );

IMPL_PROFILER_DUMP( KGlobalLogDBThread )
{
	for( unsigned int ui = 0; ui < vecDump.size(); ++ui )
	{
		unsigned int iAvg = 0;
		if( vecDump[ui].m_iQueryCount > 0 )	iAvg = vecDump[ui].m_iTotalTime / vecDump[ui].m_iQueryCount;

		DO_QUERY_NO_PROFILE( L"exec dbo.P_QueryStats_INS", L"%d, N\'%s\', %d, %d, %d, %d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% vecDump[ui].m_wstrQuery
			% vecDump[ui].m_iMinTime
			% iAvg
			% vecDump[ui].m_iMaxTime
			% vecDump[ui].m_iOver1Sec
			% vecDump[ui].m_iQueryCount
			% vecDump[ui].m_iQueryFail
			);

		continue;

end_proc:
		START_LOG( cerr, vecDump[ui].m_wstrQuery )
			<< BUILD_LOG( vecDump[ui].m_iMinTime )
			<< BUILD_LOG( vecDump[ui].m_iMaxTime )
			<< BUILD_LOG( vecDump[ui].m_iTotalTime )
			<< BUILD_LOG( vecDump[ui].m_iQueryCount )
			<< BUILD_LOG( iAvg )
			<< BUILD_LOG( vecDump[ui].m_iOver1Sec )
			<< BUILD_LOG( vecDump[ui].m_iQueryFail )
			<< END_LOG;
	}
}

KGlobalLogDBThread::~KGlobalLogDBThread(void)
{
}

void KGlobalLogDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_REQ, KNetAddress );
		//{{ 2011. 07. 19	최육사	서버간 접속 코드 리팩토링
		//#ifdef SERV_SERVER_CONNECT_CODE_REFACTORING
	   _CASE( DBE_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ );
		//#endif SERV_SERVER_CONNECT_CODE_REFACTORING
		//}}
        CASE_NOPARAM( DBE_SERVER_OFF_NOT );

		//{{ 2011. 04. 13  김민성  글로벌 서버 추가
		CASE( DBE_INCREASE_DEFENCE_CRYSTAL_REQ );
		CASE( DBE_GET_DEFENCE_CRYSTAL_REQ );
		//}} 2011. 04. 13  김민성  글로벌 서버 추가

		//{{ 2012. 12. 17	박세훈	잭팟 이벤트
		//}}
		//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
		CASE( DBE_TICK_PERFORMANCE_LOG_NOT );
		//}}

		CASE_NOPARAM( DBE_EVENT_UPDATE_REQ );

		CASE_NOPARAM( DBE_CHECK_EVENT_UPDATE_REQ );

		//{{ 2013. 02. 21	최육사	로그 시스템 개선
		CASE( DBE_LOG_STATISTICS_INFO_NOT );
		//}}

		CASE_NOPARAM( DBE_TIME_ENCHANT_EVENT_INFO_REQ );
		CASE( DBE_TIME_ENCHANT_EVENT_INFO_UPDATE_REQ );

		CASE_NOPARAM( DBE_EXCHANGE_LIMIT_INFO_REQ );
		CASE( DBE_EXCHANGE_LIMIT_INFO_UPDATE_REQ );
		CASE( DBE_EXCHANGE_LIMIT_INFO_ROLLBACK_NOT );

		//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
		CASE( DBE_UPDATE_GLOBAL_MISSION_INFO_REQ );
		CASE( DBE_GET_GLOBAL_MISSION_INFO_REQ );
		CASE( DBE_INSERT_CONNECT_EVENT_REQ );
		//}} 2012. 09. 06	임홍락	글로벌 미션 매니저

    default:
        START_LOG( cerr, L"Event handler is not defined: " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KGlobalLogDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KGlobalLogDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

void KGlobalLogDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID )
{
    SendToGSUser( nGS, nTo, usEventID, char() );
}

_IMPL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress )
{
    KDBE_LOGIN_SERVER_ON_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LServerList_By_PublicIP_SEL", L"%d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% KBaseServer::ESC_GLOBAL
		% kPacket_.m_wstrIP
		);
	//}}

    if( !m_kODBC.BeginFetch() )
    {
        START_LOG( cerr, L"Global server authentication failed." )
            << BUILD_LOG( kPacket_.m_wstrIP )
			<< END_LOG;

        kPacket.m_iOK = NetError::ERR_VERIFY_02;
        goto end_proc;
    }

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	FETCH_DATA( kPacket.m_kServerInfo.m_iUID
		>> kPacket.m_kServerInfo.m_wstrName
		>> kPacket.m_kServerInfo.m_iServerGroupID
		>> kPacket.m_kServerInfo.m_iServerClass
		>> kPacket.m_kServerInfo.m_wstrPublicIP
		>> kPacket.m_kServerInfo.m_wstrPrivateIP
		>> kPacket.m_kServerInfo.m_usMasterPort
		>> kPacket.m_kServerInfo.m_usNCUDPPort
		>> kPacket.m_kServerInfo.m_nMaxUser
		>> kPacket.m_kServerInfo.m_nCurrentUser
		>> kPacket.m_kServerInfo.m_bOn );
	//}}

    m_kODBC.EndFetch();

	// ProxyID를 받아온다.
	DO_QUERY( L"exec dbo.lup_get_LGlobalProxy", L"%d", % kPacket.m_kServerInfo.m_iServerGroupID );
	if( !m_kODBC.BeginFetch() )
	{
		START_LOG( cerr, L"Failed to retrieve the global proxy ID." )
			<< BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_VERIFY_13;
		goto end_proc;
	}

	FETCH_DATA( kPacket.m_iProxyID );
	m_kODBC.EndFetch();

    //서버가 켜졌다고 표시한다.
	DO_QUERY( L"exec dbo.P_LServerList_UPD", L"%d, %d, 1", % KBaseServer::GetKObj()->GetServerGroupID() % kPacket.m_kServerInfo.m_iUID );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( kPacket.m_iOK );
        m_kODBC.EndFetch();

        if( kPacket.m_iOK != NetError::NET_OK )
        {
            START_LOG( cerr, L"Failed to mark the server as online." )
                << BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
                << END_LOG;

            kPacket.m_iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    //결과 보내기
    SendToServer( DBE_SERVER_ON_ACK, kPacket );
}

//{{ 2011. 07. 19	최육사	서버간 접속 코드 리팩토링
//#ifdef SERV_SERVER_CONNECT_CODE_REFACTORING
_IMPL_ON_FUNC( DBE_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ )
{
	KECN_VERIFY_SERVER_CONNECT_ACK kPacket;
	kPacket.m_nOK = NetError::ERR_ODBC_01;
	kPacket.m_iServerUID = KBaseServer::GetKObj()->GetUID();
	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	int iServerSetID = KBaseServer::GetKObj()->GetServerGroupID();
	bool bUsePrivateIP = false;
	int iServerType = (int)KBaseServer::ESC_GAME;	// 국내기준 Login Server에는 게임 서버만 붙는다

	bUsePrivateIP = true;


	DO_QUERY_NO_PROFILE( L"exec dbo.P_LServerList_Verify_Connect_SEL", L"%d, %d, N\'%s\', %d, %d",
		% iServerSetID
		% iServerType
		% kPacket_.m_kNetAddress.m_wstrIP
		% kPacket_.m_kNetAddress.m_usPort
		% (int)bUsePrivateIP );

	if( m_kODBC.BeginFetch() )
	{
		//{{ 2010. 04. 22  최육사	서버군 전체 통합
		FETCH_DATA( kPacket.m_nUID
			>> kPacket.m_iDBRegServerGroupID
			>> kPacket.m_strName
			>> kPacket.m_sMaxNum );
		//}}

		m_kODBC.EndFetch();

		kPacket.m_nOK = NetError::NET_OK;
	}
	else
	{
		START_LOG( cerr, LAST_SENDER_UID )
			<< BUILD_LOG( kPacket_.m_kNetAddress.m_wstrIP )
			<< BUILD_LOG( kPacket_.m_kNetAddress.m_usPort );
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_VERIFY_SERVER_CONNECT_ACK, kPacket );
}
//#endif SERV_SERVER_CONNECT_CODE_REFACTORING
//}}

IMPL_ON_FUNC_NOPARAM( DBE_SERVER_OFF_NOT )
{
    //서버 Enable을 끈다
	DO_QUERY( L"exec dbo.P_LServerList_UPD", L"%d, %d, 0", % KBaseServer::GetKObj()->GetServerGroupID() % KBaseServer::GetKObj()->GetUID() );

    int iOK;
    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();

        if( iOK != NetError::NET_OK )
        {
            START_LOG( cerr, L"Failed to mark the server as offline." )
                << BUILD_LOG( GetKGlobalServer()->GetUID() )
                << END_LOG;

            iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    return;
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
IMPL_ON_FUNC( DBE_INCREASE_DEFENCE_CRYSTAL_REQ )
{
	int iOK = NetError::ERR_ODBC_01;

	// 보호한 수정을 기록 한다.
	DO_QUERY( L"exec dbo.P_LDefense_Crystal_SET", L"%d, %d, N\'%s\', %d",
														% KBaseServer::GetKObj()->GetServerGroupID()
														% kPacket_.m_iTargetCrystal
														% kPacket_.m_wsrtStartTime
														% kPacket_.m_iProtectedCrystalCount );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to insert world mission data." )
			<< BUILD_LOG( kPacket_.m_wsrtStartTime )
			<< BUILD_LOG( kPacket_.m_iProtectedCrystalCount )
			<< BUILD_LOG( kPacket_.m_iTargetCrystal )
			<< END_LOG;
	}

end_proc:
	return;
}

IMPL_ON_FUNC( DBE_GET_DEFENCE_CRYSTAL_REQ )
{
	// 미션 시작 시간을 넣어 쌓여 있던 크리스탈 수를 가져 온다
	DO_QUERY( L"exec dbo.P_LDefense_Crystal_SEL", L"%d, N\'%s\'", % KBaseServer::GetKObj()->GetServerGroupID()
																  % kPacket_.m_wsrtStartTime );

	// 보호된 크리스탈 수 초기화
	kPacket_.m_iProtectedCrystalCount = 0;

	while( m_kODBC.Fetch() )
	{
		FETCH_DATA( kPacket_.m_iTargetCrystal
				 >> kPacket_.m_iProtectedCrystalCount );

		START_LOG( cout, L"Successfully retrieved world mission data." )
			<< BUILD_LOG( kPacket_.m_wsrtStartTime )
			<< BUILD_LOG( kPacket_.m_iProtectedCrystalCount )
			<< BUILD_LOG( kPacket_.m_iTargetCrystal )
			<< END_LOG;
	}

end_proc:
	SendToServer( DBE_GET_DEFENCE_CRYSTAL_ACK, kPacket_ );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2012. 12. 17	박세훈	잭팟 이벤트
//}}

//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
IMPL_ON_FUNC( DBE_TICK_PERFORMANCE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	// SimLayer Tick 퍼포먼스 기록
	DO_QUERY( L"exec dbo.P_LServerSimLayerTickLog_INS", L"%d, %d, %d, %d, N\'%s\', N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iServerUID
		% kPacket_.m_iServerType
		% kPacket_.m_dwMaxTickCount
		% kPacket_.m_wstrMaxTickRegDate
		% kPacket_.m_wstrRegDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record SimLayer tick performance data." )
			<< BUILD_LOG( kPacket_.m_iServerUID )
			<< BUILD_LOG( kPacket_.m_iServerType )
			<< BUILD_LOG( kPacket_.m_dwMaxTickCount )
			<< BUILD_LOG( kPacket_.m_wstrMaxTickRegDate )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< END_LOG;
	}
}
//}}

IMPL_ON_FUNC_NOPARAM( DBE_EVENT_UPDATE_REQ )
{
	KDBE_EVENT_UPDATE_ACK kPacket;

	// DB로부터 이벤트 리스트 데이터를 받아온다
	DO_QUERY( L"exec dbo.P_EventInfo_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KEventInfo kEventInfo;

		FETCH_DATA( kEventInfo.m_iEventUID
			>> kEventInfo.m_wstrEventName
			>> kEventInfo.m_iEventType
			>> kEventInfo.m_wstrBeginDate
			>> kEventInfo.m_wstrEndDate
			>> kEventInfo.m_iScriptID
			//#ifdef SERV_ALLOW_EVENT_ERASE
			>> kEventInfo.m_bOn
			//#else SERV_ALLOW_EVENT_ERASE
			//			>> kEventInfo.m_bEnable
			//#endif SERV_ALLOW_EVENT_ERASE
			);


		kEventInfo.m_bEnable = true;

		kPacket.m_vecEventList.push_back( kEventInfo );
	}

end_proc:
	SendToServer( DBE_EVENT_UPDATE_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( DBE_CHECK_EVENT_UPDATE_REQ )
{
	KDBE_CHECK_EVENT_UPDATE_ACK		AckPacket;
	int iReleaseTickType = 0;
	int iReleaseTick = 0;

	// ReleaseTick 얻기
	DO_QUERY( L"exec dbo.P_ReleaseTick_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		FETCH_DATA( iReleaseTickType
			>> iReleaseTick );
		AckPacket.m_mapReleaseTick.insert( std::make_pair( iReleaseTickType, iReleaseTick ) );
	}

end_proc:
	SendToServer( DBE_CHECK_EVENT_UPDATE_ACK, AckPacket );
}

//{{ 2013. 02. 21	최육사	로그 시스템 개선
IMPL_ON_FUNC( DBE_LOG_STATISTICS_INFO_NOT )
{
	CTime tCurTime = CTime::GetCurrentTime();
	const std::wstring wstrCurTime = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	const UidType iServerUID = KBaseServer::GetKObj()->GetUID();

	for( const KLogStatInfo& kInfo : kPacket_.m_vecLogStat )
	{
		int iOK = NetError::ERR_ODBC_01;

		// 서버 에러 로그 통계
		DO_QUERY( L"exec dbo.P_LServer_ErrorLog_INS", L"%d, N\'%s\', N\'%s\', N\'%s\', %d, N\'%s\'",
			% iServerUID
			% kInfo.m_wstrFunctionName
			% kInfo.m_wstrFileName
			% kInfo.m_wstrLineNum
			% kInfo.m_iCount
			% wstrCurTime
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}
end_proc:
		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to record log statistics." )
				<< BUILD_LOG( iServerUID )
				<< BUILD_LOG( kInfo.m_wstrFunctionName )
				<< BUILD_LOG( kInfo.m_wstrFileName )
				<< BUILD_LOG( kInfo.m_wstrLineNum )
				<< BUILD_LOG( kInfo.m_iCount )
				<< BUILD_LOG( wstrCurTime )
				<< END_LOG;
		}
	}
}
//}}

IMPL_ON_FUNC_NOPARAM( DBE_TIME_ENCHANT_EVENT_INFO_REQ )
{
	KDBE_TIME_ENCHANT_EVENT_INFO_ACK kPacket;

	DO_QUERY_NO_ARG( L"exec dbo.P_LEvent_RandomEnchant_SEL" );

	while( m_kODBC.Fetch() )
	{
		KTimeEnchantEventInfo	kInfo;
		std::wstring			wstrBeginDate;
		std::wstring			wstrEndDate;

		FETCH_DATA( kInfo.m_iEventUID
			>> wstrBeginDate
			>> wstrEndDate
			>> kInfo.m_cEnchantLevel
			);

		CTime tBeginDate;
		CTime tEndDate;

		if( KncUtil::ConvertStringToCTime( wstrBeginDate, tBeginDate ) == false )
		{
			START_LOG( cerr, L"Failed to convert the enhancement event start time." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( wstrBeginDate )
				<< BUILD_LOG( wstrEndDate )
				<< BUILD_LOGc( kInfo.m_cEnchantLevel )
				<< END_LOG;
			continue;
		}

		if( KncUtil::ConvertStringToCTime( wstrEndDate, tEndDate ) == false )
		{
			START_LOG( cerr, L"Failed to convert the enhancement event end time." )
				<< BUILD_LOG( kInfo.m_iEventUID )
				<< BUILD_LOG( wstrBeginDate )
				<< BUILD_LOG( wstrEndDate )
				<< BUILD_LOGc( kInfo.m_cEnchantLevel )
				<< END_LOG;
			continue;
		}

        kInfo.m_tBeginDate	= tBeginDate.GetTime();
		kInfo.m_tEndDate	= tEndDate.GetTime();
		kPacket.m_vecTimeEnchantEventInfo.push_back( kInfo );
	}

end_proc:
	SendToServer( DBE_TIME_ENCHANT_EVENT_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_TIME_ENCHANT_EVENT_INFO_UPDATE_REQ )
{
	KDBE_TIME_ENCHANT_EVENT_INFO_UPDATE_ACK kPacket;
	kPacket.m_iGSUID				= kPacket_.m_iGSUID;
	kPacket.m_iUnitUID				= kPacket_.m_iUnitUID;
	kPacket.m_iAfterWorkStorageKey	= kPacket_.m_iAfterWorkStorageKey;
	kPacket.m_usEventID				= kPacket_.m_usEventID;

	int iOK = NetError::ERR_ODBC_01;

	CStringW strRegDate = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S"));
	std::wstring wstrRegDate = strRegDate.GetString();

	DO_QUERY( L"exec dbo.P_LEvent_RandomEnchant_UPD", L"%d, %d, N\'%s\'",
		% kPacket_.m_kTimeEnchantEventInfo.m_iEventUID
		% kPacket_.m_iUnitUID
		% wstrRegDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK == NetError::NET_OK )
	{
		kPacket.m_cEnchantLevel = kPacket_.m_kTimeEnchantEventInfo.m_cEnchantLevel;
	}
	else
	{
		kPacket.m_cEnchantLevel = 0;
		START_LOG( cerr, L"Failed to process the timed enhancement event." )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_kTimeEnchantEventInfo.m_iEventUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;

		KDBE_TIME_ENCHANT_EVENT_INFO_NOT kPacketToServer;
		kPacketToServer.m_iTimeEnchantEventUID	= kPacket_.m_iTimeEnchantEventUID;
		kPacketToServer.m_kTimeEnchantEventInfo	= kPacket_.m_kTimeEnchantEventInfo;
		SendToServer( DBE_TIME_ENCHANT_EVENT_INFO_NOT, kPacketToServer );
	}

	SendToUser( LAST_SENDER_UID, DBE_TIME_ENCHANT_EVENT_INFO_UPDATE_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( DBE_EXCHANGE_LIMIT_INFO_REQ )
{
	KDBE_EXCHANGE_LIMIT_INFO_ACK kPacket;

	std::map< int, int >				mapTargetID;		// < iLimitUID, iTargetID >
	std::map< int, std::set< int > >	mapTargetItemInfo;	// < iTargetID, iTargetItemID >

	// 제한 정보 가져오기
	DO_QUERY_NO_ARG( L"exec dbo.P_LExchangeLimitInfo_GET" );
	while( m_kODBC.Fetch() )
	{
		KExchangeLimitInfo	kInfo;
		std::wstring		wstrBeginDate;
		std::wstring		wstrEndDate;
		int					iTargetID = 0;

		FETCH_DATA( kInfo.m_iLimitUID
			>> iTargetID
			>> kInfo.m_iLimitQuantity
			>> wstrBeginDate
			>> wstrEndDate
			);

		CTime tBeginDate;
		CTime tEndDate;

		if( KncUtil::ConvertStringToCTime( wstrBeginDate, tBeginDate ) == false )
		{
			START_LOG( cerr, L"Failed to convert the exchange-restriction start time." )
				<< BUILD_LOG( kInfo.m_iLimitUID )
				<< END_LOG;
			continue;
		}

		if( KncUtil::ConvertStringToCTime( wstrEndDate, tEndDate ) == false )
		{
			START_LOG( cerr, L"Failed to convert the exchange-restriction end time." )
				<< BUILD_LOG( kInfo.m_iLimitUID )
				<< END_LOG;
			continue;
		}

		kInfo.m_tBeginDate	= tBeginDate.GetTime();
		kInfo.m_tEndDate	= tEndDate.GetTime();
		kPacket.m_listExchangeLimitInfo.push_back( kInfo );

		mapTargetID.insert( std::map< int, int >::value_type( kInfo.m_iLimitUID, iTargetID ) );
		mapTargetItemInfo.insert( std::map< int, std::set< int > >::value_type( iTargetID, std::set< int >() ) );
	}

	// 제한 아이템 정보 가져오기
	for( std::map< int, std::set< int > >::iterator it = mapTargetItemInfo.begin(); it != mapTargetItemInfo.end(); ++it )
	{
		DO_QUERY( L"exec dbo.P_LExchangeLimitItem_GET", L"%d", %it->first );
		while( m_kODBC.Fetch() )
		{
			int iDestItemID = 0;

			FETCH_DATA( iDestItemID );

			it->second.insert( iDestItemID );
		}
	}

	for( std::list< KExchangeLimitInfo >::iterator it = kPacket.m_listExchangeLimitInfo.begin(); it != kPacket.m_listExchangeLimitInfo.end(); ++it )
	{
		KExchangeLimitInfo& kInfo	= *it;
		int iTargetID				= 0;

		{
			std::map< int, int >::const_iterator itFind = mapTargetID.find( kInfo.m_iLimitUID );
			if( itFind == mapTargetID.end() )
			{
				START_LOG( cerr, L"Could not retrieve restricted-list target data." )
					<< BUILD_LOG( kInfo.m_iLimitUID )
					<< END_LOG;

				continue;
			}

			iTargetID = itFind->second;
		}

		{
			std::map< int, std::set< int > >::const_iterator itFind = mapTargetItemInfo.find( iTargetID );
			if( itFind == mapTargetItemInfo.end() )
			{
				START_LOG( cerr, L"Could not retrieve restricted-list target data." )
					<< BUILD_LOG( kInfo.m_iLimitUID )
					<< BUILD_LOG( iTargetID )
					<< END_LOG;

				continue;
			}

			kInfo.m_setDestItemID = itFind->second;
		}
	}

end_proc:
	SendToServer( DBE_EXCHANGE_LIMIT_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_EXCHANGE_LIMIT_INFO_UPDATE_REQ )
{
	KDBE_EXCHANGE_LIMIT_INFO_UPDATE_ACK kPacket;
	kPacket.m_iOK					= NetError::ERR_ODBC_01;
	kPacket.m_iExchangeLimitUID		= kPacket_.m_iExchangeLimitUID;

	kPacket.m_iGSUID				= kPacket_.m_iGSUID;
	kPacket.m_iUnitUID				= kPacket_.m_iUnitUID;
	kPacket.m_iAfterWorkStorageKey	= kPacket_.m_iAfterWorkStorageKey;

	DO_QUERY( L"exec dbo.P_LExchangeLimitLog_INT", L"%d, %d, %d, N\'%s\'",
		% kPacket_.m_iLimitUID
		% kPacket_.m_iUnitUID
		% kPacket_.m_iDestItemID
		% kPacket_.m_wstrRegDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK
			>> kPacket.m_iRollBackUID
			);
		m_kODBC.EndFetch();
	}

end_proc:
	if( kPacket.m_iOK != NetError::NET_OK )
	{
		kPacket.m_iOK = NetError::ERR_EXCHANGE_LIMIT_01;

		START_LOG( cerr, L"Failed to update exchange-restriction user data." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_iGSUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iLimitUID )
			<< BUILD_LOG( kPacket_.m_iDestItemID )
			<< END_LOG;

		KDBE_EXCHANGE_LIMIT_INFO_NOT kPacketToServer;
		kPacketToServer.m_iExchangeLimitUID	= kPacket_.m_iExchangeLimitUID;
		kPacketToServer.m_iLimitUID			= kPacket_.m_iLimitUID;
		SendToServer( DBE_EXCHANGE_LIMIT_INFO_NOT, kPacketToServer );
	}

	SendToUser( LAST_SENDER_UID, DBE_EXCHANGE_LIMIT_INFO_UPDATE_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_EXCHANGE_LIMIT_INFO_ROLLBACK_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LExchangeLimitLog_DEL", L"%d", % kPacket_.m_iRollBackUID );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to roll back exchange-restriction data." )
			<< BUILD_LOG( kPacket_.m_iRollBackUID )
			<< END_LOG;
	}
}

//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
IMPL_ON_FUNC( DBE_UPDATE_GLOBAL_MISSION_INFO_REQ )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LEvent_GlobalMission_UPD", L"N\'%s\', %d", % kPacket_.m_wstrGlobalMissionDay
		% kPacket_.m_iNowCount );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to insert global mission data." )
			<< BUILD_LOG( kPacket_.m_wstrGlobalMissionDay )
			<< BUILD_LOG( kPacket_.m_iNowCount )
			<< END_LOG;
	}

end_proc:
	return;
}

IMPL_ON_FUNC( DBE_GET_GLOBAL_MISSION_INFO_REQ )
{
	DO_QUERY_NO_ARG( L"exec dbo.P_LEvent_GlobalMission_SEL" );

	while( m_kODBC.Fetch() )
	{
		std::wstring wstrGlobalMissionBeginDate = L"";
		std::wstring wstrGlobalMissionEndDate = L"";
		CTime tGlobalMissionBeginDate;
		CTime tGlobalMissionEndDate;
		KGlobalMissionInfo kGlobalMissionInfo;

		FETCH_DATA( wstrGlobalMissionBeginDate
			>> wstrGlobalMissionEndDate
			>> kGlobalMissionInfo.m_iMissionType
			>> kGlobalMissionInfo.m_iClearCondition
			>> kGlobalMissionInfo.m_iClearCount
			>> kGlobalMissionInfo.m_iNowCount );

		if( false == KncUtil::ConvertStringToCTime( wstrGlobalMissionBeginDate, tGlobalMissionBeginDate ) ||
			false == KncUtil::ConvertStringToCTime( wstrGlobalMissionEndDate, tGlobalMissionEndDate ) )
		{
			START_LOG( cout, L"Failed to convert the time while retrieving global mission data." )
				<< BUILD_LOG( wstrGlobalMissionBeginDate )
				<< BUILD_LOG( wstrGlobalMissionEndDate )
				<< BUILD_LOG( kGlobalMissionInfo.m_iMissionType )
				<< BUILD_LOG( kGlobalMissionInfo.m_iClearCondition )
				<< BUILD_LOG( kGlobalMissionInfo.m_iClearCount )
				<< BUILD_LOG( kGlobalMissionInfo.m_iNowCount )
				<< END_LOG;
		}
		else
		{
			CTimeSpan tTimeGap = tGlobalMissionEndDate - tGlobalMissionBeginDate;
            kGlobalMissionInfo.m_iDurationMinute = static_cast<int>(tTimeGap.GetTotalMinutes());

			kPacket_.m_mapGlobalMissionInfo.insert( std::make_pair( wstrGlobalMissionBeginDate, kGlobalMissionInfo ) );

			START_LOG( cout, L"Successfully retrieved global mission data." )
				<< BUILD_LOG( wstrGlobalMissionBeginDate )
				<< BUILD_LOG( kGlobalMissionInfo.m_iMissionType )
				<< BUILD_LOG( kGlobalMissionInfo.m_iClearCondition )
				<< BUILD_LOG( kGlobalMissionInfo.m_iClearCount )
				<< BUILD_LOG( kGlobalMissionInfo.m_iNowCount )
				<< END_LOG;
		}
	}

end_proc:
	SendToServer( DBE_GET_GLOBAL_MISSION_INFO_ACK, kPacket_ );
}

IMPL_ON_FUNC( DBE_INSERT_CONNECT_EVENT_REQ )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.lup_insert_pirate_EventInfo", L"N\'%s\', %d, N\'%s\', N\'%s\', %d",
		% kPacket_.m_wstrEventName
		% kPacket_.m_iEventType
		% kPacket_.m_wstrNextDayStartTime
		% kPacket_.m_wstrNextDayEndTime
		% kPacket_.m_iScriptID
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to configure the global mission reward event." )
			<< BUILD_LOG( kPacket_.m_wstrEventName )
			<< BUILD_LOG( kPacket_.m_iEventType )
			<< BUILD_LOG( kPacket_.m_wstrNextDayStartTime )
			<< BUILD_LOG( kPacket_.m_wstrNextDayEndTime )
			<< BUILD_LOG( kPacket_.m_iScriptID )
			<< BUILD_LOG( iOK )
			<< END_LOG;
	}

	START_LOG( cout, L"Starting the global mission reward event." )
		<< BUILD_LOG( kPacket_.m_wstrEventName )
		<< BUILD_LOG( kPacket_.m_iEventType )
		<< BUILD_LOG( kPacket_.m_wstrNextDayStartTime )
		<< BUILD_LOG( kPacket_.m_wstrNextDayEndTime )
		<< BUILD_LOG( kPacket_.m_iScriptID )
		<< END_LOG;

end_proc:
	return ;
}
//}}{ 2012. 09. 03	임홍락	글로벌 미션 매니저
