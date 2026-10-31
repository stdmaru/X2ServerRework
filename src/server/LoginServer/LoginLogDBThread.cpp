#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "LoginLogDBThread.h"
#include "LoginServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "Enum/Enum.h"

#define CLASS_TYPE KLoginLogDBThread
ImplPfID( KLoginLogDBThread, PI_LOGIN_LOG_DB );

IMPL_PROFILER_DUMP( KLoginLogDBThread )
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

KLoginLogDBThread::~KLoginLogDBThread(void)
{
}

void KLoginLogDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_REQ, KNetAddress );
	   //{{ 2011. 07. 19	최육사	서버간 접속 코드 리팩토링
	   //#ifdef SERV_SERVER_CONNECT_CODE_REFACTORING
	   //{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
	   _CASE( DBE_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ );
	   //}}
	   //#endif SERV_SERVER_CONNECT_CODE_REFACTORING
	   //}}
	    //{{ 2008. 5. 27  최육사  체험ID 동접
	    CASE( DBE_UPDATE_SERVER_INFO_REQ );
	    //}}
		//{{ 2009. 7. 6  최육사		랭킹 개편
		CASE_NOPARAM( DBE_GET_HENIR_RANKING_INFO_REQ );
		CASE( DBE_DELETE_RANKING_INFO_NOT );
		CASE( DBE_BACKUP_RANKING_INFO_NOT );
		//}}
		//{{ 2010. 06. 01  최육사	동접 처리 개편
		CASE( DBE_UPDATE_CCU_INFO_NOT );
		//}}
		//{{ 2010. 10. 13	최육사	DLL Manager
		CASE_NOPARAM( DBE_GET_MODULE_INFO_LIST_REQ );
	   _CASE( DBE_UPDATE_MODULE_INFO_LIST_REQ, KDBE_GET_MODULE_INFO_LIST_ACK );
		//}}
		//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
		CASE_NOPARAM( DBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_REQ );
		CASE( DBE_UPDATE_TIME_RANDOM_ITEM_EVENT_RESULT_NOT );
		//}}
		//{{ 2011. 05. 11	최육사	대리상인
		CASE( ELOG_INSERT_PERSONAL_SHOP_LOG_NOT );
		//}}
		//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
		CASE( DBE_TICK_PERFORMANCE_LOG_NOT );
		//}}
		//{{ 2013. 02. 21	최육사	로그 시스템 개선
		CASE( DBE_LOG_STATISTICS_INFO_NOT );
		//}}

        CASE_NOPARAM( DBE_SERVER_OFF_NOT );

    default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KLoginLogDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KLoginLogDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

void KLoginLogDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID )
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
		% KBaseServer::ESC_LOGIN
		% kPacket_.m_wstrIP
		);
	//}}

    if( !m_kODBC.BeginFetch() )
    {
        START_LOG( cerr, L"로긴 서버 인증 실패." )
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

	//{{ 2011. 07. 22	최육사	모든 쿼리 SP사용
	DO_QUERY( L"exec dbo.P_LLoginProxy_By_ServerUID_SEL", L"%d, %d", % KBaseServer::GetKObj()->GetServerGroupID()
																	 % kPacket.m_kServerInfo.m_iUID );
	//}}
	if( !m_kODBC.BeginFetch() )
	{
		START_LOG( cerr, L"난 누군가의 로긴 서버다." )
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
            START_LOG( cerr, L"서버 켜짐 표시 실패." )
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
//{{ 2010. 05. 13  최육사	대전 던전 서버군 통합

_IMPL_ON_FUNC( DBE_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ )
{
	KECN_VERIFY_SERVER_CONNECT_ACK kPacket;
	kPacket.m_nOK = NetError::ERR_ODBC_01;
	kPacket.m_iServerUID = KBaseServer::GetKObj()->GetUID();

//////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////

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

//}}
//#endif SERV_SERVER_CONNECT_CODE_REFACTORING
//}}

//{{ 2008. 5. 27  최육사  체험ID 동접
IMPL_ON_FUNC( DBE_UPDATE_SERVER_INFO_REQ )
{
	// 동접을 DB에 적는다.
	DO_QUERY( L"exec dbo.P_StatCCUMinuteGuestUser_INS", L"%d, N\'%s\', %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_wstrRegDate.c_str()
		% kPacket_.m_iServerUID
		% kPacket_.m_iConcurrentUser
		);

	// 반환값이 없는 stored procedure임

end_proc:
	SendToServer( DBE_UPDATE_SERVER_INFO_ACK );
}

//void KLoginLogDBThread::WriteServerInfoToDB( IN KServerInfo kPacket_, OUT KServerList& kPacket )
//{
//    // 동접을 DB에 적는다.
//    int iOK;
//	DO_QUERY( ( L"exec dbo.lup_update_concurrent_user %d, %d" ) % kPacket_.m_iUID % kPacket_.m_iConcurrentUser ); // 주석 해제시 P_LServerList_CCU_UPD 사용해야함
//    if( m_kODBC.BeginFetch() )
//    {
//        FETCH_DATA( iOK );
//        m_kODBC.EndFetch();
//    }
//
//    // DB로부터 서버 리스트 데이터를 받아온다
//    DO_QUERY( ( L"exec dbo.lup_get_server_list" ) );  // 주석해제시 P_LServerList_SEL 이거 사용해야함
//
//    while( m_kODBC.Fetch() )
//    {
//        KServerInfo kServerInfo;
//
//        FETCH_DATA( kServerInfo.m_iUID
//            >> kServerInfo.m_wstrName
//            >> kServerInfo.m_iServerGroupID
//            >> kServerInfo.m_iServerClass
//            >> kServerInfo.m_wstrIP
//            >> kServerInfo.m_usMasterPort
//            >> kServerInfo.m_usNCUDPPort
//            >> kServerInfo.m_nMaxUser
//            >> kServerInfo.m_nCurrentUser
//            >> kServerInfo.m_bOn );
//
//
//        std::pair< KServerList::iterator, bool > pairIB;
//        std::vector< KServerInfo > vecServerList;
//        pairIB = kPacket.insert( std::make_pair( kServerInfo.m_iServerGroupID, vecServerList ) );
//        pairIB.first->second.push_back( kServerInfo );
//    }
//
//end_proc:
//    return;
//}

//{{ 2009. 7. 6  최육사		랭킹 개편
IMPL_ON_FUNC_NOPARAM( DBE_GET_HENIR_RANKING_INFO_REQ )
{
	KDBE_GET_HENIR_RANKING_INFO_ACK kPacket;

	//////////////////////////////////////////////////////////////////////////
	// 일간 헤니르 시공 랭킹

	DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Day_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		CTime tRegDate;
		std::wstring wstrRegDate;
		KHenirRankingInfo kInfo;

		FETCH_DATA( kInfo.m_iRank
			>> kInfo.m_iUnitUID
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> kInfo.m_iStageCount
			>> kInfo.m_ulPlayTime
			>> wstrRegDate
			);

		// RegDate 변환
		KncUtil::ConvertStringToCTime( wstrRegDate, tRegDate );
		kInfo.m_tRegDate = tRegDate.GetTime();

		// RankingInfo 저장
		std::map< int , std::vector< KHenirRankingInfo > >::iterator mit;
		mit = kPacket.m_mapHenirRanking.find( SEnum::RT_DAY_RANKING );
		if( mit == kPacket.m_mapHenirRanking.end() )
		{
			std::vector< KHenirRankingInfo > vecRanking;
			vecRanking.push_back( kInfo );
			kPacket.m_mapHenirRanking.insert( std::make_pair( SEnum::RT_DAY_RANKING, vecRanking ) );
		}
		else
		{
			mit->second.push_back( kInfo );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 주간 헤니르 시공 랭킹

	DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Week_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		CTime tRegDate;
		std::wstring wstrRegDate;
		KHenirRankingInfo kInfo;

		FETCH_DATA( kInfo.m_iRank
			>> kInfo.m_iUnitUID
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> kInfo.m_iStageCount
			>> kInfo.m_ulPlayTime
			>> wstrRegDate
			);

		// RegDate 변환
		KncUtil::ConvertStringToCTime( wstrRegDate, tRegDate );
		kInfo.m_tRegDate = tRegDate.GetTime();

		// RankingInfo 저장
		std::map< int , std::vector< KHenirRankingInfo > >::iterator mit;
		mit = kPacket.m_mapHenirRanking.find( SEnum::RT_WEEK_RANKING );
		if( mit == kPacket.m_mapHenirRanking.end() )
		{
			std::vector< KHenirRankingInfo > vecRanking;
			vecRanking.push_back( kInfo );
			kPacket.m_mapHenirRanking.insert( std::make_pair( SEnum::RT_WEEK_RANKING, vecRanking ) );
		}
		else
		{
			mit->second.push_back( kInfo );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 월간 헤니르 시공 랭킹

	DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Month_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		CTime tRegDate;
		std::wstring wstrRegDate;
		KHenirRankingInfo kInfo;

		FETCH_DATA( kInfo.m_iRank
			>> kInfo.m_iUnitUID
			>> kInfo.m_wstrNickName
			>> kInfo.m_cUnitClass
			>> kInfo.m_ucLevel
			>> kInfo.m_iStageCount
			>> kInfo.m_ulPlayTime
			>> wstrRegDate
			);

		// RegDate 변환
		KncUtil::ConvertStringToCTime( wstrRegDate, tRegDate );
		kInfo.m_tRegDate = tRegDate.GetTime();

		// RankingInfo 저장
		std::map< int , std::vector< KHenirRankingInfo > >::iterator mit;
		mit = kPacket.m_mapHenirRanking.find( SEnum::RT_MONTH_RANKING );
		if( mit == kPacket.m_mapHenirRanking.end() )
		{
			std::vector< KHenirRankingInfo > vecRanking;
			vecRanking.push_back( kInfo );
			kPacket.m_mapHenirRanking.insert( std::make_pair( SEnum::RT_MONTH_RANKING, vecRanking ) );
		}
		else
		{
			mit->second.push_back( kInfo );
		}
	}


end_proc:
	SendToServer( DBE_GET_HENIR_RANKING_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_DELETE_RANKING_INFO_NOT )
{
	// 해당 타입의 랭킹을 초기화
	switch( kPacket_.m_iRankingType )
	{
	case SEnum::RT_DAY_RANKING:
		{
			// 일간 비우기
			//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
			DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Day_DEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
			//}}

			START_LOG( cout, L"헤니르 시공 일일 랭킹 DB에서 삭제 완료!" );
		}
		break;

	case SEnum::RT_WEEK_RANKING:
		{
			// 주간 비우기
			//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
			DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Week_DEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
			//}}

            START_LOG( cout, L"헤니르 시공 주간 랭킹 DB에서 삭제 완료!" );
		}
		break;

	case SEnum::RT_MONTH_RANKING:
		{
			// 월간 비우기
			//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
			DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Month_DEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
			//}}

			START_LOG( cout, L"헤니르 시공 월간 랭킹 DB에서 삭제 완료!" );
		}
		break;


	default:
		{
			START_LOG( cerr, L"DB테이블이 없는 랭킹타입을 초기화하려 하였습니다" )
				<< BUILD_LOG( kPacket_.m_iRankingType )
				<< END_LOG;
		}
	}

	//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
end_proc:
	return;
	//}}
}

IMPL_ON_FUNC( DBE_BACKUP_RANKING_INFO_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
	const CTime tCurTime = CTime::GetCurrentTime();
	const CStringW strCurTime = tCurTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	const std::wstring wstrCurDate = strCurTime.GetString();
	//}}

	std::map< int, std::vector< KHenirRankingInfo > >::const_iterator mit;
	for( mit = kPacket_.m_mapHenirRanking.begin(); mit != kPacket_.m_mapHenirRanking.end(); ++mit )
	{
		const int iRankingType = mit->first;
		const std::vector< KHenirRankingInfo >& vecRanking = mit->second;

		switch( iRankingType )
		{
		case SEnum::RT_DAY_RANKING:
			{
				// 일간 비우기
				//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
				DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Day_DEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
				//}}
			}
			break;

		case SEnum::RT_WEEK_RANKING:
			{
				// 주간 비우기
				//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
				DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Week_DEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
				//}}
			}
			break;

		case SEnum::RT_MONTH_RANKING:
			{
				// 월간 비우기
				//{{ 2011. 09. 07	최육사	헤니르 시공 랭킹 백업 업데이트
				DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Month_DEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
				//}}
			}
			break;


		default:
			{
				START_LOG( cerr, L"DB테이블이 없는 랭킹타입을 초기화하려 하였습니다" )
					<< BUILD_LOG( iRankingType )
					<< END_LOG;

				continue;
			}
		}

		std::vector< KHenirRankingInfo >::const_iterator vit;
		for( vit = vecRanking.begin(); vit != vecRanking.end(); ++vit )
		{
			const KHenirRankingInfo& kRankingInfo = *vit;

			// RegDate 변환
			CTime tRegDate = CTime( kRankingInfo.m_tRegDate );
			CStringW strRegDate = tRegDate.Format(_T("%Y-%m-%d %H:%M:%S"));
			const std::wstring wstrRegDate = strRegDate.GetString();

			// DB Update
			switch( iRankingType )
			{
			case SEnum::RT_DAY_RANKING:
				{
					DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Day_INS", L"%d, %d, %d, N\'%s\', %d, %d, %d, %d, N\'%s\', N\'%s\'",
						% KBaseServer::GetKObj()->GetServerGroupID()
						% kRankingInfo.m_iRank
						% kRankingInfo.m_iUnitUID
						% kRankingInfo.m_wstrNickName
						% (int)kRankingInfo.m_cUnitClass
						% (int)kRankingInfo.m_ucLevel
						% kRankingInfo.m_iStageCount
						% kRankingInfo.m_ulPlayTime
						% wstrRegDate
						% wstrCurDate
						);
				}
				break;

			case SEnum::RT_WEEK_RANKING:
				{
					DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Week_INS", L"%d, %d, %d, N\'%s\', %d, %d, %d, %d, N\'%s\', N\'%s\'",
						% KBaseServer::GetKObj()->GetServerGroupID()
						% kRankingInfo.m_iRank
						% kRankingInfo.m_iUnitUID
						% kRankingInfo.m_wstrNickName
						% (int)kRankingInfo.m_cUnitClass
						% (int)kRankingInfo.m_ucLevel
						% kRankingInfo.m_iStageCount
						% kRankingInfo.m_ulPlayTime
						% wstrRegDate
						% wstrCurDate
						);
				}
				break;

			case SEnum::RT_MONTH_RANKING:
				{
					DO_QUERY( L"exec dbo.P_Rank_SpaceTime_Month_INS", L"%d, %d, %d, N\'%s\', %d, %d, %d, %d, N\'%s\', N\'%s\'",
						% KBaseServer::GetKObj()->GetServerGroupID()
						% kRankingInfo.m_iRank
						% kRankingInfo.m_iUnitUID
						% kRankingInfo.m_wstrNickName
						% (int)kRankingInfo.m_cUnitClass
						% (int)kRankingInfo.m_ucLevel
						% kRankingInfo.m_iStageCount
						% kRankingInfo.m_ulPlayTime
						% wstrRegDate
						% wstrCurDate
						);
				}
				break;


			default:
				{
					START_LOG( cerr, L"이상한 랭킹타입 입니다" )
						<< BUILD_LOG( iRankingType )
						<< END_LOG;
					continue;
				}
			}

			//if( m_kODBC.BeginFetch() )
			//{
			//	FETCH_DATA( iOK );
			//	m_kODBC.EndFetch();

			//	if( iOK != NetError::NET_OK )
			//	{
			//		START_LOG( cerr, L"헤니르의 시공 랭킹 정보 업데이트 실패" )
			//			<< BUILD_LOG( mit->first )
			//			<< BUILD_LOG( kRankingInfo.m_iRank )
			//			<< BUILD_LOG( kRankingInfo.m_iUnitUID )
			//			<< BUILD_LOG( kRankingInfo.m_wstrNickName )
			//			<< BUILD_LOG( (int)kRankingInfo.m_cUnitClass )
			//			<< BUILD_LOG( (int)kRankingInfo.m_ucLevel )
			//			<< BUILD_LOG( kRankingInfo.m_iStageCount )
			//			<< BUILD_LOG( kRankingInfo.m_ulRemainPlayTime )
			//			<< BUILD_LOG( wstrRegDate )
			//			<< END_LOG;
			//	}
			//}
		}

		START_LOG( cout, L"헤니르의 시공 랭킹 정보 DB백업 완료!" )
			<< BUILD_LOG( iRankingType )
			<< BUILD_LOG( vecRanking.size() );
	}

end_proc:
	return;
}
//}}

//{{ 2010. 06. 01  최육사	동접 처리 개편

IMPL_ON_FUNC( DBE_UPDATE_CCU_INFO_NOT )
{
	for( u_int ui = 0; ui < kPacket_.m_vecCCUInfo.size(); ++ui )
	{
		const KUpdateCCUInfo& kInfo = kPacket_.m_vecCCUInfo[ui];

		// 동접을 DB에 적는다.
		int iOK = NetError::ERR_ODBC_01;

		// 서버UID가 0이면 전체동접
		if( kInfo.m_iServerUID == 0 )
		{
			DO_QUERY( L"exec dbo.P_StatCCUMinute_INS", L"%d, N\'%s\', %d, %d",
				% KBaseServer::GetKObj()->GetServerGroupID()
				% kPacket_.m_wstrRegDate.c_str()
				% kInfo.m_iServerUID
				% kInfo.m_iConcurrentUser
				);
		}
		else if( kInfo.m_iServerUID == KBaseServer::GetKObj()->GetUID() )
		{
			DO_QUERY( L"exec dbo.P_StatCCUMinuteGuestUser_INS", L"%d, N\'%s\', %d, %d",
				% KBaseServer::GetKObj()->GetServerGroupID()
				% kPacket_.m_wstrRegDate.c_str()
				% kInfo.m_iServerUID
				% kInfo.m_iConcurrentUser
				);
		}
		else
		{
			DO_QUERY( L"exec dbo.P_LServerList_CCU_UPD", L"%d, N\'%s\', %d, %d, %d, %d",
				% KBaseServer::GetKObj()->GetServerGroupID()
				% kPacket_.m_wstrRegDate.c_str()
				% kInfo.m_iServerUID
				% kInfo.m_iConcurrentUser
				% kInfo.m_iCurPartyCount
				% kInfo.m_iPlayGamePartyCount
				);
		}

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}


		//if( iOK != NetError::NET_OK )
		//{
		//	START_LOG( cerr, L"동접 정보 기록에 실패하였습니다!" )
		//		<< BUILD_LOG( kPacket_.m_wstrRegDate )
		//		<< BUILD_LOG( kInfo.m_iServerUID )
		//		<< BUILD_LOG( kInfo.m_iConcurrentUser )
		//		<< BUILD_LOG( kInfo.m_iCurPartyCount )
		//		<< BUILD_LOG( kInfo.m_iPlayGamePartyCount )
		//		<< END_LOG;
		//}

end_proc:
		continue;
	}
}

//}}

//{{ 2010. 10. 13	최육사	DLL Manager

IMPL_ON_FUNC_NOPARAM( DBE_GET_MODULE_INFO_LIST_REQ )
{
	KDBE_GET_MODULE_INFO_LIST_ACK kPacket;

	DO_QUERY( L"exec dbo.P_LModuleTable_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KModuleInfo kInfo;

		//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
		FETCH_DATA( kInfo.m_iModuleUID
			>> kInfo.m_wstrModuleName
			>> kInfo.m_iCount
			>> kInfo.m_bAllow
			>> kInfo.m_iFlag
			);
		//}}

		kPacket.m_vecModuleList.push_back( kInfo );
	}


	//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	//}}

end_proc:
	SendToServer( DBE_GET_MODULE_INFO_LIST_ACK, kPacket );
}

_IMPL_ON_FUNC( DBE_UPDATE_MODULE_INFO_LIST_REQ, KDBE_GET_MODULE_INFO_LIST_ACK )
{
	KDBE_GET_MODULE_INFO_LIST_ACK kPacket;

	for( const KModuleInfo& kInfo : kPacket_.m_vecModuleList )
	{
		int iOK = NetError::ERR_ODBC_01;

		// 모듈UID가 0이면 새로 insert되는 것임.
		if( kInfo.m_iModuleUID == 0 )
		{
			KModuleInfo kNewModuleInfo = kInfo;

			//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
			DO_QUERY( L"exec dbo.P_LModuleTable_INS", L"%d, N\'%s\', %d, %d, %d",
				% KBaseServer::GetKObj()->GetServerGroupID()
				% kNewModuleInfo.m_wstrModuleName
				% kNewModuleInfo.m_iCount
				% kNewModuleInfo.m_bAllow
				% kNewModuleInfo.m_iFlag
				);
			//}}


			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK
					>> kNewModuleInfo.m_iModuleUID );

				m_kODBC.EndFetch();
			}

			if( iOK != NetError::NET_OK )
			{
				//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
				START_LOG( cerr, L"모듈 정보 insert실패!" )
					<< BUILD_LOG( kNewModuleInfo.m_wstrModuleName )
					<< BUILD_LOG( kNewModuleInfo.m_iCount )
					<< BUILD_LOG( kNewModuleInfo.m_bAllow )
					<< BUILD_LOG( kNewModuleInfo.m_iFlag )
					<< END_LOG;
				//}}
			}
			else
			{
				kPacket.m_vecModuleList.push_back( kNewModuleInfo );
			}
		}
		// 모듈UID가 0이 아니면 update되는 것임.
		else
		{
			DO_QUERY( L"exec dbo.P_LModuleTable_UPD", L"%d, %d, %d",
				% KBaseServer::GetKObj()->GetServerGroupID()
				% kInfo.m_iModuleUID
				% kInfo.m_iCount
				);

			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}

			if( iOK != NetError::NET_OK )
			{
				START_LOG( cerr, L"모듈 정보 update실패!" )
					<< BUILD_LOG( kInfo.m_iModuleUID )
					<< BUILD_LOG( kInfo.m_iCount )
					<< END_LOG;
			}
		}
	}

end_proc:
	SendToServer( DBE_UPDATE_MODULE_INFO_LIST_ACK, kPacket );
}

//}}

//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상

IMPL_ON_FUNC_NOPARAM( DBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_REQ )
{
	KDBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_ACK kPacket;

	DO_QUERY( L"exec dbo.P_LEvent_Christmas_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KTimeOpenRandomItemEventInfo kInfo;
		std::wstring wstrUpdateTime;
		UidType iUserUID = 0;
		UidType iUnitUID = 0;

		//{{ 2011. 12.13    김민성	접속 이벤트 기능 추가 (반복 지급)
		FETCH_DATA( kInfo.m_iID
			>> kInfo.m_iItemID
			>> kInfo.m_wstrDropTime
			>> wstrUpdateTime
			>> iUserUID
			>> iUnitUID
			>> kInfo.m_wstrDropEndTime
			>> kInfo.m_iOpenItemID );
		//}}

		if( iUserUID != 0 )
		{
			// 이미 보상 받은 유저라면 따로 저장하자!
			kPacket.m_setGetRewardUserUIDList.insert( iUserUID );
			continue;
		}

		kPacket.m_vecTimeOpenRandomItemInfo.push_back( kInfo );
	}

end_proc:
	SendToServer( DBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( DBE_UPDATE_TIME_RANDOM_ITEM_EVENT_RESULT_NOT )
{
	int iOK = NetError::NET_OK;

	CTime tUpdateTime = CTime( kPacket_.m_kDBUpdateInfo.m_tUpdateTime );
	CStringW strUpdateTime = tUpdateTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	const std::wstring wstrUpdateTime = strUpdateTime.GetString();

	DO_QUERY( L"exec dbo.P_LEvent_Christmas_UPD", L"%d, %d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_kDBUpdateInfo.m_iID
		% kPacket_.m_kDBUpdateInfo.m_iGetUserUID
		% kPacket_.m_kDBUpdateInfo.m_iGetUnitUID
		% wstrUpdateTime
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_kDBUpdateInfo.m_iID )
		<< BUILD_LOG( kPacket_.m_kDBUpdateInfo.m_iGetUserUID )
		<< BUILD_LOG( kPacket_.m_kDBUpdateInfo.m_iGetUnitUID )
		<< BUILD_LOG( wstrUpdateTime )
		<< END_LOG;
}

//}}

//{{ 2011. 05. 11	최육사	대리상인
IMPL_ON_FUNC( ELOG_INSERT_PERSONAL_SHOP_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.lup_insert_store_log", L"%d, %d, %d, %d, %d, %d, %d, %d",
		% kPacket_.m_iFromUnitUID
		% kPacket_.m_iToUnitUID
		% kPacket_.m_iItemID
		% kPacket_.m_iItemUID
		% kPacket_.m_iQuantity
		% kPacket_.m_iED
		% kPacket_.m_bIsSquare
		% (int)kPacket_.m_cPShopType );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iFromUnitUID )
		<< BUILD_LOG( kPacket_.m_iToUnitUID )
		<< BUILD_LOG( kPacket_.m_iItemID )
		<< BUILD_LOG( kPacket_.m_iItemUID )
		<< BUILD_LOG( kPacket_.m_iQuantity )
		<< BUILD_LOG( kPacket_.m_iED )
		<< BUILD_LOG( kPacket_.m_bIsSquare )
		<< BUILD_LOGc( kPacket_.m_cPShopType )
		<< END_LOG;
}
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
            START_LOG( cerr, L"서버 종료 표시 실패." )
                << BUILD_LOG( GetKLoginServer()->GetUID() )
                << END_LOG;

            iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    return;
}

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
		START_LOG( cerr, L"SimLayer Tick 퍼포먼스 로그 기록 실패!" )
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
	CStringW strCurTime = tCurTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	const std::wstring wstrCurTime = strCurTime.GetString();
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
			START_LOG( cerr, L"Log Statistics 정보 기록 실패!" )
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
