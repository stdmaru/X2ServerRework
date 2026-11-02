#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "ChannelLogDBThread.h"
#include "ChannelServer.h"
#include "ActorManager.h"
#include "NetError.h"

//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
//}}

//{{ 2012. 07. 04	최육사		대기열 시스템
	#include "Enum/Enum.h"
//}}

#define CLASS_TYPE KChannelLogDBThread
ImplPfID( KChannelLogDBThread, PI_CHANNEL_LOG_DB );

IMPL_PROFILER_DUMP( KChannelLogDBThread )
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

KChannelLogDBThread::~KChannelLogDBThread(void)
{
}

void KChannelLogDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_REQ, KNetAddress );
        CASE_NOPARAM( DBE_SERVER_OFF_NOT );
		CASE( DBE_CHANNEL_LIST_REQ );

		CASE_NOPARAM( DBE_SERVERGROUP_LIST_REQ );

		//{{ 2009. 12. 16  최육사	동접툴
		CASE( DBE_GET_CONCURRENT_USER_INFO_REQ );
		//}}
		//{{ 2010. 02. 16  최육사	해킹툴 리스트
		CASE( DBE_CHECK_HACKING_TOOL_LIST_REQ );
		//}}

		//{{ 2011. 01. 20 김민성	동접 모니터링 툴
		CASE_NOPARAM( DBE_GET_GAME_SERVER_LIST_REQ );
		//}}
		//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
		//}}

		CASE( DBE_INSERT_LOGIN_RESULT_INFO_REQ );

		//{{ 2012. 07. 04	최육사		대기열 시스템
		CASE_NOPARAM( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_REQ );
		//}}
		//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
		CASE( DBE_TICK_PERFORMANCE_LOG_NOT );
		//}}
		//{{ 2013. 02. 21	최육사	로그 시스템 개선
		CASE( DBE_LOG_STATISTICS_INFO_NOT );
		//}}

    default:
        START_LOG( cerr, L"Event handler is not defined. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KChannelLogDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KChannelLogDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

_IMPL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress )
{
    KDBE_CHANNEL_SERVER_ON_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LServerList_By_PublicIP_SEL", L"%d, %d, N\'%s\'", % KBaseServer::GetKObj()->GetServerGroupID() % KBaseServer::ESC_CHANNEL % kPacket_.m_wstrIP );
	//}}

    if( !m_kODBC.BeginFetch() )
    {
        START_LOG( cerr, L"Channel server authentication failed." )
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


    //서버가 켜졌다고 표시한다.
	DO_QUERY( L"exec dbo.P_LServerList_UPD", L"%d, %d, 1", % KBaseServer::GetKObj()->GetServerGroupID() % kPacket.m_kServerInfo.m_iUID );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( kPacket.m_iOK );
        m_kODBC.EndFetch();

        if( kPacket.m_iOK != NetError::NET_OK )
        {
            START_LOG( cerr, L"Failed to mark server as started." )
                << BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
                << END_LOG;

            kPacket.m_iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    //결과 보내기
    SendToServer( DBE_SERVER_ON_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( DBE_SERVER_OFF_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	// Disable the server
	DO_QUERY( L"exec dbo.P_LServerList_UPD", L"%d, %d, 0", % KBaseServer::GetKObj()->GetServerGroupID() % KBaseServer::GetKObj()->GetUID() );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();

        if( iOK != NetError::NET_OK )
        {
            START_LOG( cerr, L"Failed to mark server as stopped." )
                << BUILD_LOG( GetKChannelServer()->GetUID() )
                << END_LOG;

            iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    return;
}

IMPL_ON_FUNC( DBE_CHANNEL_LIST_REQ )
{
	KDBE_CHANNEL_LIST_ACK kPacket;

	// DB로부터 서버 리스트 데이터를 받아온다
	DO_QUERY( L"exec dbo.P_LChannelList_SEL", L"%d", % kPacket_.m_iServerGroupID );

	while( m_kODBC.Fetch() )
	{
		KChannelInfo kInfo;

		FETCH_DATA( kInfo.m_iServerUID
			>> kInfo.m_iChannelID
			>> kInfo.m_wstrChannelName
			>> kInfo.m_iServerGroupID
			>> kInfo.m_wstrIP
			>> kInfo.m_usMasterPort
			>> kInfo.m_usNCUDPPort
			>> kInfo.m_iMaxUser
			>> kInfo.m_iCurrentUser
			>> kInfo.m_iCurPartyCount
			>> kInfo.m_iPlayGamePartyCount
			);

		kPacket.m_mapChannelList.insert( std::make_pair( kInfo.m_iChannelID, kInfo ) );
	}

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	{
		std::map<int, KChannelInfo>::const_iterator mit;
		for( mit = kPacket.m_mapChannelList.begin(); mit != kPacket.m_mapChannelList.end(); ++mit )
		{
			KChannelBonusInfo tempChannelBonusInfo;

			DO_QUERY( L"exec dbo.P_LChannelReward_SEL", L"%d, %d",	% KBaseServer::GetKObj()->GetServerGroupID()
																	% mit->first );

			if( m_kODBC.BeginFetch() )
			{
				int iChannelID = 0;
				FETCH_DATA( iChannelID
					>> tempChannelBonusInfo.m_iBeginLv
					>> tempChannelBonusInfo.m_iEndLv
					>> tempChannelBonusInfo.m_iPerExp
					>> tempChannelBonusInfo.m_iPerED
					);
				m_kODBC.EndFetch();

				if( iChannelID != mit->first )
				{
					START_LOG( cerr, L"Invalid channel bonus data." )
						<< BUILD_LOG( iChannelID )
						<< BUILD_LOG( mit->first )
						<< END_LOG;
				}

				//if( tempChannelBonusInfo.m_iPerExp > 0  ||  tempChannelBonusInfo.m_iPerED > 0 )
				{
					// 보너스값이 있으면 enable
					tempChannelBonusInfo.m_bEnable = true;
					kPacket.m_mapChannelBonusList.insert(std::make_pair(mit->first, tempChannelBonusInfo));
				}
			}
		}
	}
	//}}

end_proc:
	SendToServer( DBE_CHANNEL_LIST_ACK, kPacket );
}

//{{ 2009. 12. 16  최육사	동접툴
IMPL_ON_FUNC( DBE_GET_CONCURRENT_USER_INFO_REQ )
{
	KDBE_GET_CONCURRENT_USER_INFO_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_cCCUType = kPacket_.m_cCCUType;
	kPacket.m_iServerUID = kPacket_.m_iServerUID;
	kPacket.m_wstrDate = kPacket_.m_wstrDate;

	switch( kPacket_.m_cCCUType )
	{
	case KE_TOOL_GET_CCU_INFO_REQ::CT_TODAY:
		{
			DO_QUERY( L"exec dbo.P_StatCCUMinute_Today_SEL", L"%d, %d", % KBaseServer::GetKObj()->GetServerGroupID()
																		% kPacket_.m_iServerUID );
		}
		break;

	case KE_TOOL_GET_CCU_INFO_REQ::CT_YESTERDAY:
		{
			DO_QUERY( L"exec dbo.P_StatCCUMinute_Yesterday_SEL", L"%d, %d", % KBaseServer::GetKObj()->GetServerGroupID()
																			% kPacket_.m_iServerUID );
		}
		break;

	case KE_TOOL_GET_CCU_INFO_REQ::CT_DATE:
		{
			DO_QUERY( L"exec dbo.P_StatCCUMinute_SEL", L"%d, %d, N\'%s\'",  % KBaseServer::GetKObj()->GetServerGroupID()
																			% kPacket_.m_iServerUID
																			% kPacket_.m_wstrDate );
		}
		break;

	case KE_TOOL_GET_CCU_INFO_REQ::CT_NOWTIME:
		{
			DO_QUERY( L"exec dbo.P_StatCCUMinute_Nowtime_SEL", L"%d, %d, N\'%s\'",  % KBaseServer::GetKObj()->GetServerGroupID()
																					% kPacket_.m_iServerUID
																					% kPacket_.m_wstrDate );
		}
		break;

	default:
		{
            START_LOG( cerr, L"Invalid CCU type." )
				<< BUILD_LOGc( kPacket_.m_cCCUType )
				<< END_LOG;

			goto end_proc;
		}
		break;
	}

	while( m_kODBC.Fetch() )
	{
		KCCUInfo kInfo;
		int iServerUID = 0;
		std::wstring wstrDate;

		FETCH_DATA( wstrDate
			>> iServerUID
			>> kInfo.m_iCCUCount
			);

		//{{ 2012. 10. 22	박세훈	동접 정보 DB 실패 처리 추가
		if( wstrDate.empty() == true )
		{
			kPacket.m_vecCCUInfo.clear();
			goto end_proc;
		}
		//}}

		CTime tDate;
		KncUtil::ConvertStringToCTime( wstrDate, tDate );
		kInfo.m_iCCUTime = tDate.GetTime();

		kPacket.m_vecCCUInfo.push_back( kInfo );
	}

	//{{ 2012. 10. 22	박세훈	동접 정보 DB 실패 처리 추가
	if( kPacket.m_vecCCUInfo.empty() == true )
	{
		goto end_proc;
	}
	//}}

	kPacket.m_iOK = NetError::NET_OK;

end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_GET_CONCURRENT_USER_INFO_ACK, kPacket );
}
//}}

//{{ 2010. 02. 16  최육사	해킹툴 리스트

IMPL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_REQ )
{
	KDBE_CHECK_HACKING_TOOL_LIST_ACK kPacket;
	kPacket.m_iReleaseTick = kPacket_.m_iReleaseTick;

	// Release Tick 얻기
	DO_QUERY( L"exec dbo.P_ReleaseTick_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		int iAbuserType = 0;
		int iReleaseTick = 0;

		FETCH_DATA( iAbuserType
			>> iReleaseTick );

		if( iAbuserType != 5 )
			continue;

		if( iReleaseTick > kPacket_.m_iReleaseTick )
		{
			// 새로운 ReleaseTick값을 얻는다.
			kPacket.m_iReleaseTick = iReleaseTick;
		}
	}

	// Release Tick값이 올라갔다면 리스트 새로 받는다.
	if( kPacket.m_iReleaseTick > kPacket_.m_iReleaseTick )
	{
		DO_QUERY( L"exec dbo.P_LHackProcessList_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

		while( m_kODBC.Fetch() )
		{
			KHackingToolInfo kInfo;

			FETCH_DATA( kInfo.m_wstrProcessName
				>> kInfo.m_wstrWindowClassName
				>> kInfo.m_cFlag );

			kPacket.m_vecHackingToolList.push_back( kInfo );
		}
	}

end_proc:
	SendToServer( DBE_CHECK_HACKING_TOOL_LIST_ACK, kPacket );
}

//}}

//{{ 2011. 01. 13 김민성	동접 모니터링 툴
IMPL_ON_FUNC_NOPARAM( DBE_GET_GAME_SERVER_LIST_REQ )
{
	KDBE_GET_GAME_SERVER_LIST_ACK kPacket;
	kPacket.m_iServerCount = 0;

	//서버 목록 얻기
	DO_QUERY( L"exec dbo.P_LToolServerList_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KToolServerList _Server;

		FETCH_DATA( _Server.m_iServerUID
			>> _Server.m_iServerSetID
			>> _Server.m_iServerType
			>> _Server.m_iChannelID
			>> _Server.m_wstrChannelName
			>> _Server.m_wstrIP
			>> _Server.m_iPort	);

		kPacket.m_vecServerList.push_back( _Server );

		kPacket.m_iServerCount++;
	}

end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_GET_GAME_SERVER_LIST_ACK, kPacket );
}
//}}

//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
//}}

//{{ 2012. 07. 04	최육사		대기열 시스템
IMPL_ON_FUNC_NOPARAM( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_REQ )
{
	KDBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_ACK kPacket;

	// DB로부터 서버 리스트 데이터를 받아온다
	DO_QUERY( L"exec dbo.P_LChannelList_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KChannelInfo kInfo;

		FETCH_DATA( kInfo.m_iServerUID
			>> kInfo.m_iChannelID
			>> kInfo.m_wstrChannelName
			>> kInfo.m_iServerGroupID
			>> kInfo.m_wstrIP
			>> kInfo.m_usMasterPort
			>> kInfo.m_usNCUDPPort
			>> kInfo.m_iMaxUser
			>> kInfo.m_iCurrentUser
			>> kInfo.m_iCurPartyCount
			>> kInfo.m_iPlayGamePartyCount
			);

		kPacket.m_iTotalCCU += kInfo.m_iCurrentUser;
	}

end_proc:
	SendToServer( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_ACK, kPacket );
}
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
		START_LOG( cerr, L"Failed to record SimLayer tick performance log!" )
			<< BUILD_LOG( kPacket_.m_iServerUID )
			<< BUILD_LOG( kPacket_.m_iServerType )
			<< BUILD_LOG( kPacket_.m_dwMaxTickCount )
			<< BUILD_LOG( kPacket_.m_wstrMaxTickRegDate )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< END_LOG;
	}
}
//}}

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
			START_LOG( cerr, L"Failed to record log statistics!" )
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

IMPL_ON_FUNC_NOPARAM( DBE_SERVERGROUP_LIST_REQ )
{
	KDBE_SERVERGROUP_LIST_ACK kPacket;

		DO_QUERY( L"exec dbo.P_LServerList_By_ServerSetID_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KServerGroupInfo kInfo;

		FETCH_DATA( kInfo.m_iOrder
			>> kInfo.m_iServerGroupUID
			>> kInfo.m_wstrServerGroupName
			>> kInfo.m_kServerIP
			>> kInfo.m_usMasterPort
			>> kInfo.m_usNCUDPPort
			>> kInfo.m_bEnable
			);

		kPacket.m_mapServerGroupList.insert( std::make_pair( kInfo.m_iServerGroupUID, kInfo ) );
	}

end_proc:
	SendToServer( DBE_SERVERGROUP_LIST_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_INSERT_LOGIN_RESULT_INFO_REQ )
{
	int iOK = NetError::ERR_ODBC_01;
	int index = -1;

	for(int i = 0; i < KLoginResultInfo::LRTE_SENTINEL; i++)
	{
		index = i;
		DO_QUERY( L"exec dbo.P_StatsLoginLOG_INS", L"%d, %d, %d, %d",
			% kPacket_.m_iServerGroupID
			% kPacket_.m_uiServerUID
			% i
			% kPacket_.m_vecResultCount[i]
			);

			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to register login result information." )
			<< BUILD_LOG( kPacket_.m_iServerGroupID )
			<< BUILD_LOG( kPacket_.m_uiServerUID )
			<< BUILD_LOG( index )
			<< BUILD_LOG( kPacket_.m_vecResultCount[index] )
			<< END_LOG;
	}
}
