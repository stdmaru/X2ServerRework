#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "GSLogDBThread.h"
#include "GameServer.h"
#include "NetError.h"
#include "UserStatistics.h"
#include "X2Data/XSLRoom.h"
#include "X2Data/XSLDungeon.h"

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}

//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
//}}

// [참고!] DO_QUERY() 에서 전달인자수 정확하게 넣지 않으면 서버 크래쉬 납니다!

//ImplementDBThread( KGSLogDBThread );
ImplPfID( KGSLogDBThread, PI_GS_LOG_DB );

#define CLASS_TYPE KGSLogDBThread

IMPL_PROFILER_DUMP( KGSLogDBThread )
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

KGSLogDBThread::~KGSLogDBThread(void)
{
}

void KGSLogDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
	   _CASE( DBE_SERVER_ON_REQ, KNetAddress );
        CASE( DBE_UPDATE_SERVER_INFO_REQ );
		CASE_NOPARAM( DBE_ADMIN_SERVER_STATUS_REQ );
		CASE_NOPARAM( DBE_SERVER_OFF_NOT );
		//{{ 2009. 2. 13  최육사	채널리스트
		CASE( DBE_CHANNEL_LIST_REQ );
		//}}
		CASE( DBE_CHARACTER_LEVELUP_NOT );
		CASE( ELOG_STAT_PLAY_TIME );
	   _CASE( ELOG_STAT_PVP_MAP, KStatisticsData );
	   _CASE( ELOG_STAT_FPS, KStatisticsData );
	   _CASE( ELOG_STAT_ED, KStatisticsData );
	   _CASE( ELOG_STAT_QUEST, KStatisticsData );
	   //{{ 2011. 02. 28	최육사	사내 통합 통계
	   //}}
		CASE( ELOG_USER_STATISTICS );
		CASE( ELOG_CHEAT_LOG_NOT );
	   _CASE( ELOG_STAT_DISCONNECT_USERFSM, KStatisticsData );
	   _CASE( ELOG_STAT_DISCONNECT_REASON, KStatisticsData );
	   _CASE( DBE_DELETE_UNIT_STAT_NOT, KEGS_DELETE_UNIT_REQ );
        CASE( DBE_UPDATE_UNIT_CLASS_NOT );
		CASE( ELOG_RESURRECTION_STONE_LOG_NOT );
		CASE( ELOG_RESOLVE_JACKPOT_LOG_NOT );
        CASE( ELOG_GET_PURCHASED_CASH_ITEM_NOT );
		CASE_NOPARAM( DBE_EVENT_UPDATE_REQ );

		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		//}}
		CASE( ELOG_ITEM_ABUSER_LOG_NOT );
		CASE( ELOG_ENCHANT_ABUSER_LOG_NOT );
		CASE( ELOG_ED_ABUSER_LOG_NOT );
		//{{ 2008. 10. 22  최육사	캐쉬 분해 로그
		CASE( ELOG_CASH_ITEM_RESOLVE_LOG_NOT );
		//}}
		//{{ 2008. 11. 10  최육사
		CASE( ELOG_ENCHANT_ITEM_BREAK_LOG_NOT );
		//}}
		//{{ 2008. 10. 31  최육사	어뷰저 리스트
		CASE( DBE_ABUSER_LIST_REQ );
		//}}
		CASE( DBE_REG_ABUSER_AUTOMATIC_NOT );
		//{{ 2008. 12. 18  최육사	보조제 로그
		CASE( ELOG_ENCHANT_SUPPORT_MATERIAL_LOG_NOT );
		//}}
		//{{ 2008. 12. 22  최육사	깨짐 복구 로그
		CASE( ELOG_RESTORE_ITEM_LOG_NOT );
		//}}
		//{{ 2008. 12. 22  최육사	부여 로그
		CASE( ELOG_ATTACH_ITEM_ENCHANT_LOG_NOT );
		//}}
		//{{ 2008. 12. 30  최육사	매력아이템 로그
		CASE( ELOG_CHARM_ITEM_LOG_NOT );
		//}}
		//{{ 2009. 4. 14  최육사	닉네임 변경
		CASE( ELOG_DELETE_NICK_NAME_NOT );
		CASE( ELOG_CHANGE_NICK_NAME_NOT );
		//}}

		CASE( ELOG_INIT_SKILL_TREE_NOT );
		CASE( ELOG_INSERT_CASH_SKILL_POINT_NOT );

		//{{ 2009. 7. 22  최육사	메가폰로그
		CASE( ELOG_MEGAPHONE_LOG_NOT );
		//}}

		//{{ 2009. 9. 1  최육사		비정상종료사유
		CASE( ELOG_ABNORMAL_DISCONNECT );
		//}}
		//{{ 2009. 9. 9  최육사		밀봉
		CASE( ELOG_USE_SEAL_ITEM_NOT );
		//}}
		//{{ 2009. 10. 22  최육사	트래픽 유발 유저 해킹유저로 자동등록
		CASE( ELOG_TRAFFIC_ATTACKER_LOG_NOT );
		//}}
		//{{ 2010. 02. 16  최육사	해킹툴 리스트
		CASE( DBE_CHECK_HACKING_TOOL_LIST_REQ );
		//}}
		//{{ 2010. 06. 30  최육사	근성도 통계
		CASE( DBE_SPIRIT_STATISTICS_NOT );
		//}}


		//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
		//}}
		//{{ 2011. 01. 05	최육사	속성 부적
		CASE( ELOG_ATTRIB_ATTACH_ITEM_LOG_NOT );
		//}}
		//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
		CASE( ELOG_USER_CHARACTER_LOG_NOT );
		//}}
		//{{ 2011. 02. 09	최육사	캐쉬템 구매 로그
		CASE( ELOG_BUY_CASH_ITEM_LOG_NOT );
		//}}
		//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
		CASE( ELOG_CASH_PRODUCT_LIST_UPDATE_NOT );
		//}}
		//{{ 2011. 04. 05  김민성	우편 및 거래 감시
		CASE( DBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ );
		//}}
		//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
	   _CASE( ELOG_STAT_ITEM_DB, KStatisticsData );
		//}}


	   //{{ 2011. 05. 02  김민성	2차 보안 시스템
	   CASE( DBE_FIRST_WRITE_SECURITY_TYPE_NOT );
	   //}}

	   _CASE( ELOG_UPDATE_SHARE_ITEM_NOT, std::vector< KTradeShareItemInfo > );
	    CASE( ELOG_ITEM_TRADE_ERROR_NOT );

	   //{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
		 CASE( DBE_FIRST_WRITE_COME_BACK_INFO_NOT );
	   //}}
		 //{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
		 //}
		//{{  2011.11.08     김민성    강화 로그
		 CASE( ELOG_HIGH_ENCHANT_ITEM_LOG_NOT );
		//}}
		 //{{  2011.11.24     김민성    캐릭터 접속 로그
		 //}}
		 //{{ 2011. 12. 02  김민성	AP 구매 로그 수치
		 CASE( DBE_BUY_AP_ITEM_LOG_NOT );
		 //}}
		 //{{ 2011. 12. 14	최육사	패킷 처리 지연 시간 체크
		 CASE( DBE_UPDATE_PAKCET_MORNITORING_LOG_NOT );
		 //}}
		 //{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
		 //}}
		 //{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
		 //}}
		 //{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
		 //}}
		 //{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
		 //}}
		 //{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
		 _CASE( DBE_BAD_ATTITUDE_USER_LOG_NOT, KERM_BAD_ATTITUDE_USER_LOG_NOT );
		 //}
		 //{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
		 CASE( DBE_TICK_PERFORMANCE_LOG_NOT );
		 //}}
		 //{{ 2013. 02. 19   교환 로그 추가 - 김민성
		 CASE( DBE_ITEM_EXCHANGE_LOG_NOT );
		 //}

		 //{{ 허상형 : [2010/8/31/] //	부활석 통계 분리 작업
		 //}} 허상형 : [2010/8/31/] //	부활석 통계 분리 작업

		 //{{ 2011.05.04   임규수 아바타 합성 시스템
		 //}}

		 CASE_NOPARAM( DBE_CHECK_EVENT_UPDATE_REQ );

		 _CASE( DBE_CATCH_HACKUSER_INTO_NOT, KEGS_CATCH_HACKUSER_INFO_NOT );

		 //{{ 2013. 02. 21	최육사	로그 시스템 개선
		 CASE( DBE_LOG_STATISTICS_INFO_NOT );
		 //}}



		 //{{ 2013. 04. 01	 인연 시스템 - 김민성
		 CASE( DBE_WRITE_RELATIONSHIP_INFO_LOG_REQ );
		 //}



		 CASE( ELOG_KOM_FILE_CHECK_LOG_NOT );

         CASE( ELOG_EXPAND_BANK_INVENTORY_NOT );

		 CASE( ELOG_EXPAND_SKILL_PAGE_NOT );

		 CASE( DBE_CHANGE_PET_ID_LOG_NOT );


		CASE( DBE_UNLIMITED_SECOND_CHANGE_JOB_NOT );

    default:
        START_LOG( cerr, L"No event handler is defined. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KGSLogDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KGSLogDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

void KGSLogDBThread::UserStatisticsTableParsing( std::map< std::pair< int, int >, std::vector< int > >& mapStatistics,
								int iTableID, int iSubTableID, int iMaxColumn, int* arrayData )
{
	// 반드시 초기화!
	ZeroMemory( arrayData, iMaxColumn );

	std::map< std::pair< int, int >, std::vector< int > >::iterator mit;
	mit = mapStatistics.find( std::make_pair( iTableID, iSubTableID ) );
	if( mit == mapStatistics.end() )
		return;

	std::vector< int >::iterator vit = mit->second.begin();
	for( int iIndex = 0; iIndex < iMaxColumn; iIndex++ )
	{
		if( vit == mit->second.end() )
		{
			arrayData[iIndex] = 0;
		}
		else
		{
			arrayData[iIndex] = *vit;
			vit++;
		}
	}
}

void KGSLogDBThread::WriteServerInfoToDB( IN KDBE_UPDATE_SERVER_INFO_REQ& kPacket_, OUT KServerList& kPacket )
{
    // 동접을 DB에 적는다.
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_LServerList_CCU_UPD", L"%d, N\'%s\', %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_wstrRegDate.c_str()
		% kPacket_.m_iServerUID
		% kPacket_.m_iConcurrentUser
		% kPacket_.m_iCurPartyCount
		% kPacket_.m_iPlayGamePartyCount
		);

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();
    }

    // DB로부터 서버 리스트 데이터를 받아온다
	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LServerList_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );
	//}}

    while( m_kODBC.Fetch() )
    {
        KServerInfo kServerInfo;

		//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
		FETCH_DATA( kServerInfo.m_iUID
			>> kServerInfo.m_wstrName
			>> kServerInfo.m_iServerGroupID
			>> kServerInfo.m_iServerClass
			>> kServerInfo.m_wstrPublicIP
			>> kServerInfo.m_wstrPrivateIP
			>> kServerInfo.m_usMasterPort
			>> kServerInfo.m_usNCUDPPort
			>> kServerInfo.m_nMaxUser
			>> kServerInfo.m_nCurrentUser
			>> kServerInfo.m_bOn );
		//}}

        std::pair< KServerList::iterator, bool > pairIB;
        std::vector< KServerInfo > vecServerList;
        pairIB = kPacket.insert( std::make_pair( kServerInfo.m_iServerGroupID, vecServerList ) );
        pairIB.first->second.push_back( kServerInfo );
    }

end_proc:
    return;
}

IMPL_ON_FUNC( DBE_UPDATE_SERVER_INFO_REQ )
{
    KServerList kPacket;
    WriteServerInfoToDB( kPacket_, kPacket );

    SendToServer( DBE_UPDATE_SERVER_INFO_ACK, kPacket );
}

IMPL_ON_FUNC( ELOG_STAT_PLAY_TIME )
{
    int iOK = NetError::ERR_ODBC_01;

	int iChannelCode = 0;
	bool bPCBangFlag = kPacket_.m_bIsPcbang;	//m_bIsPcbang 해외도 필요해서 오과금 define 밖으로 뺌

	iChannelCode = static_cast< int >(kPacket_.m_ucChannelCode);
	// SERV_FIELD_PLAY_TIME_LOG define 내부 포함임

	DO_QUERY( L"exec dbo.P_LConnectLog_INS", L"%d, %d, N\'%s\', N\'%s\', N\'%s\', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID
		% kPacket_.m_wstrIP
		% kPacket_.m_wstrLoginTime
		% kPacket_.m_wstrLogoutTime
		% kPacket_.m_bLoginFail
		% kPacket_.m_bLogoutFail
		% kPacket_.m_iTotalPlayTime
		% kPacket_.m_iPvpPlayTime
		% kPacket_.m_iDungeonPlayTime
		% kPacket_.m_iFieldPlayTime
		% kPacket_.m_iDisconnectReason
		% kPacket_.m_iDisconnectUserFSM
		% 0 //kPacket_.m_iArcadePlayTime
		% iChannelCode
		% kPacket_.m_bSteamClient
		% bPCBangFlag
		);

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();
    }

end_proc:
    LOG_SUCCESS( iOK == NetError::NET_OK )
        << BUILD_LOG( iOK )
        << END_LOG;
}

//{{ 2007. 10. 25  최육사  대전 맵 통계
_IMPL_ON_FUNC( ELOG_STAT_PVP_MAP, KStatisticsData )
{
	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); mit++ )
	{
		if( mit->first.m_vecIntKey.empty() )
		{
			START_LOG( cerr, L"Unit ID is empty." )
				<< END_LOG;

			continue;
		}
		int iWorldID = mit->first.m_vecIntKey[0];
		__int64 aiCount[KStatistics::eSIColPvpMap_Max];
		std::vector< __int64 >::iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::eSIColPvpMap_Max; iIndex++ )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iWorldID )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		DO_QUERY( L"exec dbo.P_StatsVsMap_SET", L"%d, %d, %d",
					% KBaseServer::GetKObj()->GetServerGroupID()
					% iWorldID
					% aiCount[KStatistics::eSIColPvpMap_PlayCount]
					);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		START_LOG( clog2, L"PvP map statistics [check]" )
			<< BUILD_LOG( iWorldID )
			<< BUILD_LOG( aiCount[KStatistics::eSIColPvpMap_PlayCount] )
			<< BUILD_LOG( iOK );
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}
//}}

//{{ 2007. 11. 3  최육사  FPS 통계
_IMPL_ON_FUNC( ELOG_STAT_FPS, KStatisticsData )
{
	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); mit++ )
	{
		if( mit->first.m_vecIntKey.empty() )
		{
			START_LOG( cerr, L"The statistics key is empty." )
				<< END_LOG;

			continue;
		}
		int iGameMode = mit->first.m_vecIntKey[0];
		__int64 aiCount[KStatistics::eSIColFps_Max];
		std::vector< __int64 >::iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::eSIColFps_Max; iIndex++ )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iGameMode )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		DO_QUERY( L"exec dbo.P_FrameRate_SET", L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iGameMode										//  @iPlayType  [tinyint] - 던전(훈련소 포함), 대전, 광장 등
			% aiCount[KStatistics::eSIColFps_0_20]			//	@iSUnder20  [int]     - 20 이하 카운트
			% aiCount[KStatistics::eSIColFps_21_30]			//	@iS21_30    [int]     - 21~30 카운트
			% aiCount[KStatistics::eSIColFps_31_40]			//	@iS31_40    [int]
			% aiCount[KStatistics::eSIColFps_41_50]			//	@iS41_50    [int]
			% aiCount[KStatistics::eSIColFps_51_60]			//	@iS51_60    [int]
			% aiCount[KStatistics::eSIColFps_61_70]			//	@iS61_70    [int]
			% aiCount[KStatistics::eSIColFps_71_80]			//	@iS71_80    [int]
			% aiCount[KStatistics::eSIColFps_81_90]			//	@iS81_90    [int]
			% aiCount[KStatistics::eSIColFps_91_unlimited]	//	@iSOver90   [int]     - 90 이상 카운트
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		START_LOG( clog2, L"FPS statistics [check]" )
			<< BUILD_LOG( iGameMode )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_0_20] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_21_30] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_31_40] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_41_50] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_51_60] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_61_70] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_71_80] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_81_90] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColFps_91_unlimited] )
			<< BUILD_LOG( iOK );
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}
//}}

_IMPL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress )
{
    KDBE_GAME_SERVER_ON_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LServerList_By_PublicIP_SEL", L"%d, %d, N\'%s\', %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% KBaseServer::ESC_GAME
		% kPacket_.m_wstrIP
		% kPacket_.m_usPort
		);
	//}}

    if( m_kODBC.BeginFetch() )
    {
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
			>> kPacket.m_kServerInfo.m_bOn
			>> kPacket.m_kServerInfo.m_cServerRollType );
		//}}

        m_kODBC.EndFetch();
    }
    else
    {
        START_LOG( cerr, L"Game server authentication failed." )
            << BUILD_LOG( kPacket_.m_wstrIP )
			<< END_LOG;

        kPacket.m_iOK = NetError::ERR_VERIFY_02;
        goto end_proc;
    }

	//////////////////////////////////////////////////////////////////////////
	// 센터들의 주소를 알아온다.
	kPacket.m_mapCenterList.clear();

	//{{ 2013. 01. 22	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LCenterProxy_By_ServerSetID_SEL", L"%d", % kPacket.m_kServerInfo.m_iServerGroupID );
	//}}

	while( m_kODBC.Fetch() )
	{
		int iProxyID;
		KNetAddress kAddres;
		FETCH_DATA( iProxyID
			>> kAddres.m_wstrIP
			>> kAddres.m_usPort );

		kPacket.m_mapCenterList.insert( std::make_pair( iProxyID, kAddres ) );
	}

	//////////////////////////////////////////////////////////////////////////
	// 로긴서버들의 주소를 알아온다.
	kPacket.m_mapLoginList.clear();


	//{{ 2013. 01. 22	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LLoginProxy_By_ServerSetID_SEL", L"%d", % kPacket.m_kServerInfo.m_iServerGroupID );
	//}}

	while( m_kODBC.Fetch() )
	{
		int iProxyID;
		KNetAddress kAddres;
		FETCH_DATA( iProxyID
			>> kAddres.m_wstrIP
			>> kAddres.m_usPort );

		kPacket.m_mapLoginList.insert( std::make_pair( iProxyID, kAddres ) );
	}

    //서버가 켜졌다고 표시한다.
	DO_QUERY( L"exec dbo.P_LServerList_UPD", L"%d, %d, 1", % KBaseServer::GetKObj()->GetServerGroupID() % kPacket.m_kServerInfo.m_iUID );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( kPacket.m_iOK );
        m_kODBC.EndFetch();

        if( kPacket.m_iOK != NetError::NET_OK )
        {
            kPacket.m_iOK = NetError::ERR_VERIFY_08;
			goto end_proc;
        }
    }

	//{{ 2011. 07. 22	최육사	모든 쿼리 SP사용
	DO_QUERY( L"exec dbo.P_LChannelList_By_ServerUID_SEL", L"%d, %d", % KBaseServer::GetKObj()->GetServerGroupID()
																	 % kPacket.m_kServerInfo.m_iUID 	);

	//}}

	if( m_kODBC.BeginFetch() )
	{
		kPacket.m_iChannelID = 0;

		FETCH_DATA( kPacket.m_iChannelID );
		m_kODBC.EndFetch();

		if( kPacket.m_iChannelID <= 0 )
		{
			START_LOG( cerr, L"Invalid channel ID for this game server." )
				<< BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
				<< BUILD_LOG( kPacket.m_iChannelID )
				<< END_LOG;

			kPacket.m_iOK = NetError::ERR_VERIFY_15;
			goto end_proc;
		}
	}
	else
	{
		START_LOG( cerr, L"No channel information for the game server; this should never happen!" )
			<< BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_VERIFY_15;
		goto end_proc;
	}

	DO_QUERY( L"exec dbo.P_LChannelReward_SEL", L"%d, %d",	% KBaseServer::GetKObj()->GetServerGroupID()
															% kPacket.m_iChannelID );
	if( m_kODBC.BeginFetch() )
	{
		int iChannelID = 0;
		FETCH_DATA( iChannelID
			>> kPacket.m_kBonusInfo.m_iBeginLv
			>> kPacket.m_kBonusInfo.m_iEndLv
			>> kPacket.m_kBonusInfo.m_iPerExp
			>> kPacket.m_kBonusInfo.m_iPerED );
		m_kODBC.EndFetch();

		if( iChannelID != kPacket.m_iChannelID )
		{
			START_LOG( cerr, L"Invalid channel bonus information." )
				<< BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
				<< BUILD_LOG( iChannelID )
				<< BUILD_LOG( kPacket.m_iChannelID )
				<< END_LOG;

			kPacket.m_iOK = NetError::ERR_VERIFY_15;
			goto end_proc;
		}

		if( kPacket.m_kBonusInfo.m_iPerExp > 0  ||  kPacket.m_kBonusInfo.m_iPerED > 0 )
		{
			// 보너스값이 있으면 enable
			kPacket.m_kBonusInfo.m_bEnable = true;
		}
	}
	//}}

	//////////////////////////////////////////////////////////////////////////
	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	kPacket.m_mapGlobalList.clear();

	//{{ 2013. 01. 22	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LGlobalProxy_By_ServerSetID_SEL", L"%d", % kPacket.m_kServerInfo.m_iServerGroupID );
	//}}

	while( m_kODBC.Fetch() )
	{
		int iProxyID;
		KNetAddress kAddres;
		FETCH_DATA( iProxyID
			>> kAddres.m_wstrIP
			>> kAddres.m_usPort );

		kPacket.m_mapGlobalList.insert( std::make_pair( iProxyID, kAddres ) );
	}

	//서버가 켜졌다고 표시한다.
	DO_QUERY( L"exec dbo.P_LServerList_UPD", L"%d, %d, 1", % KBaseServer::GetKObj()->GetServerGroupID() % kPacket.m_kServerInfo.m_iUID );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();

		if( kPacket.m_iOK != NetError::NET_OK )
		{
			kPacket.m_iOK = NetError::ERR_VERIFY_08;
			goto end_proc;
		}
	}
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

end_proc:
    //결과 보내기
    SendToServer( DBE_SERVER_ON_ACK, kPacket );
}

IMPL_ON_FUNC_NOPARAM( DBE_ADMIN_SERVER_STATUS_REQ )
{
    KEGS_ADMIN_SERVER_STATUS_ACK kPacket;
    kPacket.m_iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LServerList_CCU_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

    if( m_kODBC.BeginFetch() )
    {
        kPacket.m_iOK = NetError::NET_OK;
        FETCH_DATA( kPacket.m_iCCU );
        m_kODBC.EndFetch();
    }


end_proc:
    SendToUser( LAST_SENDER_UID, DBE_ADMIN_SERVER_STATUS_ACK, kPacket );
}

//{{ 2009. 2. 13  최육사	채널리스트
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
					START_LOG( cerr, L"Invalid channel bonus information." )
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
            START_LOG( cerr, L"Failed to set server status to online." )
                << BUILD_LOG( GetKGameServer()->GetUID() )
                << END_LOG;

            iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    return;
}

IMPL_ON_FUNC( DBE_CHARACTER_LEVELUP_NOT )
{
    //서버 Enable을 끈다
	DO_QUERY( L"exec dbo.P_LCharLvUp_SET", L"%d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_nCharUID
		% (int)kPacket_.m_ucLevelBefore
		% (int)kPacket_.m_ucLevelAfter );

end_proc:
    return;
}

// boost::format의 문자열 길이 제한에 따른 클래스 재정의
typedef KUserStatistics US;

IMPL_ON_FUNC( ELOG_USER_STATISTICS )
{
	int iOK = NetError::ERR_ODBC_01;
	[[maybe_unused]] int iOK_2 = NetError::ERR_ODBC_01;

	if( kPacket_.m_iUnitUID <= 0 )
	{
		START_LOG( cerr, L"Invalid UnitUID." )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< END_LOG;

		return;
	}

	int etcData[KUserStatistics::US_Etc_Max] = {0};
	int edData[KUserStatistics::US_ED_Max] = {0};

	int pvpMcTeam[KUserStatistics::US_Pvp_MC_Max] = {0};
	int pvpMcTeamDeath[KUserStatistics::US_Pvp_MC_Max] = {0};
	int pvpMcDeath[KUserStatistics::US_Pvp_MC_Max] = {0};

	int pvpPtTeam[KUserStatistics::US_Pvp_PT_Max] = {0};
	int pvpPtTeamDeath[KUserStatistics::US_Pvp_PT_Max] = {0};
	int pvpPtDeath[KUserStatistics::US_Pvp_PT_Max] = {0};

	//std::map< int, std::vector< int > > map_dungeon; // 던전 통계는 이 변수로 값을 추출하자 //2011.10.18 lygan_조성욱 // 중국 전용 유저 통계값 추출
	int dungeon[KUserStatistics::US_D_Max] = {0};
	//map_dungeon.clear();

	int TrainingCenter[KUserStatistics::US_TC_Max] = {0};

	int iIsPcBang = 0;

	if( kPacket_.m_mapEtcStatistics.empty() == false )
	{
		UserStatisticsTableParsing( kPacket_.m_mapEtcStatistics, KUserStatistics::USTable_EtcData,
			0, KUserStatistics::US_Etc_Max, etcData );

		iIsPcBang = ( etcData[KUserStatistics::US_Etc_IsPcBang] == 0 ? 0 : 1 );

	}

	if( kPacket_.m_mapEDStatistics.empty() == false )
	{

		UserStatisticsTableParsing( kPacket_.m_mapEDStatistics, KUserStatistics::USTable_EDData,
			0, KUserStatistics::US_ED_Max, edData );


	}

	if( kPacket_.m_mapPvpStatistics.empty() == false )
	{
		UserStatisticsTableParsing( kPacket_.m_mapPvpStatistics, KUserStatistics::USTable_PvpModeCount,
			CXSLRoom::PGT_TEAM, KUserStatistics::US_Pvp_MC_Max, pvpMcTeam );

		UserStatisticsTableParsing( kPacket_.m_mapPvpStatistics, KUserStatistics::USTable_PvpModeCount,
			CXSLRoom::PGT_TEAM_DEATH, KUserStatistics::US_Pvp_MC_Max, pvpMcTeamDeath );

		UserStatisticsTableParsing( kPacket_.m_mapPvpStatistics, KUserStatistics::USTable_PvpModeCount,
			CXSLRoom::PGT_SURVIVAL, KUserStatistics::US_Pvp_MC_Max, pvpMcDeath );

		UserStatisticsTableParsing( kPacket_.m_mapPvpStatistics, KUserStatistics::USTable_PvpPlayTime,
			CXSLRoom::PGT_TEAM, KUserStatistics::US_Pvp_PT_Max, pvpPtTeam );

		UserStatisticsTableParsing( kPacket_.m_mapPvpStatistics, KUserStatistics::USTable_PvpPlayTime,
			CXSLRoom::PGT_TEAM_DEATH, KUserStatistics::US_Pvp_PT_Max, pvpPtTeamDeath );

		UserStatisticsTableParsing( kPacket_.m_mapPvpStatistics, KUserStatistics::USTable_PvpPlayTime,
			CXSLRoom::PGT_SURVIVAL, KUserStatistics::US_Pvp_PT_Max, pvpPtDeath );

	}


	if( kPacket_.m_mapDungeonStatistics.empty() == false )
	{
		int dungeonTemp[KUserStatistics::US_D_Max] = {0};

		UserStatisticsIterator mit = kPacket_.m_mapDungeonStatistics.begin();
		for( ; mit != kPacket_.m_mapDungeonStatistics.end(); mit++ )
		{

			if( mit->first.first != KUserStatistics::USTable_Dungeon )
			{
				START_LOG( cerr, L"A non-dungeon statistics table entry was added to dungeon statistics?" )
					<< BUILD_LOG( mit->first.first )
					<< END_LOG;

				continue;
			}

			UserStatisticsTableParsing( kPacket_.m_mapDungeonStatistics, KUserStatistics::USTable_Dungeon,
				mit->first.second, KUserStatistics::US_D_Max, dungeonTemp );

			for(int i =0; i< KUserStatistics::US_D_Max; i++)
			{
				dungeon[i] += dungeonTemp[i];
			}

		}
	}

	// 훈련소 통계
	if( kPacket_.m_mapTCenterStatistics.empty() == false )
	{
		int TrainingCenterTemp[KUserStatistics::US_TC_Max] = {0};

		UserStatisticsIterator mit = kPacket_.m_mapTCenterStatistics.begin();
		for( ; mit != kPacket_.m_mapTCenterStatistics.end(); mit++ )
		{
			if( mit->first.first != KUserStatistics::USTable_TrainingCenter )
			{
				START_LOG( cerr, L"A non-training statistics table entry was added to training statistics?" )
					<< BUILD_LOG( mit->first.first )
					<< END_LOG;

				continue;
			}

			UserStatisticsTableParsing( kPacket_.m_mapTCenterStatistics, KUserStatistics::USTable_TrainingCenter,
				mit->first.second, KUserStatistics::US_TC_Max, TrainingCenterTemp );

			for(int i =0; i< KUserStatistics::US_TC_Max; i++)
			{
				TrainingCenter[i] += TrainingCenterTemp[i];
			}

		}
	}

	int dungeonSiglePlayCount	= 0;
	int dungeonParyPlayCount	= 0;

	dungeonSiglePlayCount = dungeon[US::US_D_MC_Win] + dungeon[US::US_D_MC_Lose] + dungeon[US::US_D_MC_Drop];
	dungeonParyPlayCount = dungeon[US::US_D_MC_Party_Win] + dungeon[US::US_D_MC_Party_Lose] + dungeon[US::US_D_MC_Party_Drop];

	int dungeonSiglePlayPT = 0;
	int dungeonParyPlayPT = 0;

	dungeonSiglePlayPT = dungeon[US::US_D_PT_Win] + dungeon[US::US_D_PT_Lose] + dungeon[US::US_D_PT_Drop];
	dungeonParyPlayPT = dungeon[US::US_D_PT_Party_Win] + dungeon[US::US_D_PT_Party_Lose] + dungeon[US::US_D_PT_Party_Drop];

	int pvpWinCount = 0;
	int pvpLoseCount = 0;

	pvpWinCount = pvpMcTeam[US::US_Pvp_MC_Win] + pvpMcTeamDeath[US::US_Pvp_MC_Win] + pvpMcDeath[US::US_Pvp_MC_Win];
	pvpLoseCount = pvpMcTeam[US::US_Pvp_MC_Lose] + pvpMcTeamDeath[US::US_Pvp_MC_Lose] + pvpMcDeath[US::US_Pvp_MC_Lose];

	int trainingcenterPT = 0;
	int trainingcenterCount = 0;

	trainingcenterPT = TrainingCenter[US::US_TC_PT_Success] + TrainingCenter[US::US_TC_PT_Fail] + TrainingCenter[US::US_TC_PT_Drop];
	trainingcenterCount = TrainingCenter[US::US_TC_MC_Success] + TrainingCenter[US::US_TC_MC_Fail] + TrainingCenter[US::US_TC_MC_Drop];

	int reStoneCount = 0;

	reStoneCount = dungeon[US::US_D_MC_ReStone] + dungeon[US::US_D_MC_Party_ReStone];

	// 기존 SP명 : P_LUnitStatisticsTotal
	DO_QUERY( L"exec dbo.P_LUnitStatistics_SET",
		L"%d, "
		//Connect 5개
		L"%d, N\'%s\', N\'%s\', %d, %d,"											 //서버번호, 로그인일시, 로그아웃 일시, PC방 여부, 유저 권한레벨
		//User 3개
		L"%d, %d, N\'%s\', "													//계정고유번호, 퍼블리셔SN, 계정생성일시
		//Unit 9개
		L"%d, N\'%s\', %d, %d, %d, %d, N\'%s\', %d, %d, "					//고유번호, 닉네임, 클래스, 경험치, 레벨, ED, 생성일시, 총 완료 퀘스트 수, 총 중복 완료 퀘스트수
		//ED 26개
		L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d,"								//던전 획득, 트레이닝 획득, 퀘스트 획득, 아이템 판매 획득, 아이템 거래 획득, 아이템 개인거래 획득, 우편 수령 획득, 아이템 판매, 거래지불, 아이템 수리비,
		L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d,"								//아이템 강화 수수료, 아이템 소켓 수수료, 아이템 속성 수수료, 아이템 제조 수수료, 던전 이탈 패널티, 개인거래지불, 개인상점 개설 수수료, 개인상점 구매 수수료, 개인상점 판매 수수료, 우편첨부,
		L"%d, %d, %d, %d, %d, %d,"												// 우편발송 수수료, 우편 아이템 첨부 수수료, 큐브 오픈 수수료, 필드 획득, 코보 익스프레스 수수료, 분해 수수료
		//Play 23개
		L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d,"								//사용 근성도, 근송도 모두 소모여부, 총플래이 시간, 총대기 시간, 총대전 시간, 던전 싱글 플래이 횟수, 던전 파티 플래이 횟수, 던전 싱글 클리어 횟수, 던전 파티 클리어 횟수, 던전 싱글 플래이 시간,
		L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d,"								//던전 파티 플래이 시간, 대전  Kill수 , 대전 승수, 대전 패수, 트레이닝 플래이 시간, 트레이닝 플래이 횟수, 개인 거래 횟수, 광장 출입 횟수, 광장 머문시간, 구매한 아이템수,
		L"%d, %d, %d" ,															//판매한 아이템 수, 획득한 아이템 수, 총 사용한 부활석
		//Connect 5개
		% KBaseServer::GetKObj()->GetServerGroupID()							//@iServerSetID TINYINT
		% KBaseServer::GetKObj()->GetUID()										//@ServerUID					int					-- 서버 번호
		% kPacket_.m_wstrLoginTime												//@logintime					datetime			-- 로그인 일시
		% kPacket_.m_wstrLogoutTime												//@logouttime					datetime			-- 로그아웃 일시
		% iIsPcBang																//@PCBangFlag					bit					-- PC방 여부
		% kPacket_.m_iAuthLevel													//@AuthLevel					tinyint				-- 계정 권한
		//User 3개
		% kPacket_.m_iUserUID													//@UserUID						bigint				-- 계정 고유번호
		% kPacket_.m_uiGiantUID													//@PublisherSN					int					-- 퍼블리셔 SN
		% kPacket_.m_wstrAccountCreateDate										//@UserRegDate					smalldatetime		-- 계정 생성일시
		//Unit 9개
		% kPacket_.m_iUnitUID													//@UnitUID						bigint				-- 고유번호
		% kPacket_.m_wstrNickName												//@NickName						nvarchar(20)		-- 닉네임
		% ( int )kPacket_.m_cUnitClass											//@UnitClass					tinyint				-- 클래스
		% kPacket_.m_iEXP														//@Exp							int					-- 경험치
		% (int)kPacket_.m_ucLevel												//@Level						tinyint				-- 레벨
		% kPacket_.m_iED														//@GamePoint					int					-- ED
		% kPacket_.m_wstrUnitCreateDate											//@UnitRegDate					smalldatetime		-- 생성일시
		% etcData[US::US_Etc_NQuestComplete]									//@QuestCnt						int					-- 총 완료 퀘스트 수
		% etcData[US::US_Etc_RQuestComplete]									//@ReQuestCnt					int					-- 총 중복 완료 퀘스트 수
		//ED 23개
		% edData[US::US_ED_PDungeon]											//@PDungeon						int					-- 던전 획득
		% edData[US::US_ED_PTraining]											//PTraining						int					-- 트레이닝 획득
		% edData[US::US_ED_PQuest]												//@PQuest						int					-- 퀘스트 획득
		% edData[US::US_ED_PItemSell]											//@PItemSell					int					-- 아이템 판매 획득
		% edData[US::US_ED_PItemTrade]											//@PItemTrade					bigint				-- 아이템 거래 획득
		% edData[US::US_ED_PPersonalShop]										//@PPersonalShop				bigint				-- 아이템 개인 거래 획득
		% edData[US::US_ED_PPostLetter]											//@PPostLetter					bigint				-- 우편 수령 획득
		% edData[US::US_ED_MItemBuy]											//@MItemBuy						int					-- 아이템 판매
		% edData[US::US_ED_MItemTrade]											//@MItemTrade					bigint				-- 거래 지불
		% edData[US::US_ED_MItemRepair]											//@MItemRepair					int					-- 아이템 수리비
		% edData[US::US_ED_MEnchant]											//@MItemEnchant					bigint				-- 아이템 강화 수수료
		% edData[US::US_ED_MSocket]												//@MItemSocket					int					-- 아이템 소켓 수수료
		% edData[US::US_ED_MAttribEnchant]										//@MItemAttribute				int					-- 아이템 속성 수수료
		% edData[US::US_ED_MManufacture]										//@MManufacture					int					-- 아이템 제조 수수료
		% edData[US::US_ED_MDunPenalty]											//@MDunPenalty					int					-- 던전 이탈 패널티
		% edData[US::US_ED_MPersonalShop]										//@MPersonalShop				bigint				-- 개인 거래 지불
		% edData[US::US_ED_MOpenPShop]											//@MOpenPShop					bigint				-- 개인 상점 개설 수수료
		% edData[US::US_ED_MSellTaxPShop]										//@MSellTaxPShop				int					-- 개인 상점 구매 수수료
		% edData[US::US_ED_MBuyTaxPShop]										//@MBuyTaxPShop					int					-- 개인 상점 판매 수수료
		% edData[US::US_ED_MPostLetter]											//@MPostLetter					bigint				-- 우편 첨부
		% edData[US::US_ED_MSendLetterBasic]									//@MSendLetterBasic				int					-- 우편 발송 수수료
		% edData[US::US_ED_MSendLetterItem]										//@MSendLetterItem				int					-- 우편 아이템 첨부 수수료
		% edData[US::US_ED_OpenCubeSpendED]									//@iOpenCube					int					-- 큐브 오픈 수수료 // SERV_CUBE_OPEN_ED_CONDITION
		% edData[US::US_ED_PBattleFieldED]									//@iPBattleFieldED				int					-- 필드 획득		// SERV_SEPARATE_DUNGEON_FIELD_ED_LOG
		% edData[US::US_ED_MCoboExpressTicketED]								//@iMCoboExpressTicketED		int					-- 코보 익스프레스 수수료 // SERV_SEPARATE_DUNGEON_FIELD_ED_LOG
		% edData[US::US_ED_MItemResolve]										//@iMItemDecom					bigint				-- 분해 수수료		// SERV_MULTI_RESOLVE
		//Play 23개
		% etcData[US::US_Etc_Used_Spirit]										//@UseSpirit					int					-- 사용 근성도
		% etcData[US::US_Etc_AllUsed_Spirit_Count]								//@AllSpirit					bit					-- 근성도 모두 소모 여부
		% etcData[US::US_Etc_Dungeon_Play_Time]									//@TotalPlayTime				int					-- 총 플래이 시간(초)
		% kPacket_.m_iiDurationTime												//@DurationTime					int					-- 총 대기 시간(초)
		% etcData[US::US_Etc_PVP_Play_Time]										//@VsPlayTime					int					-- 총 대전 시간(초)
		% dungeonSiglePlayCount													//@DSoloCnt						int					-- 던전 싱글 플래이 횟수
		% dungeonParyPlayCount													//@DPartyCnt					int					-- 던전 파티 플래이 횟수
		% dungeon[US::US_D_MC_Win]												//@DSWCnt						int					-- 던전 싱글 클리이 횟수
		% dungeon[US::US_D_MC_Party_Win]										//@DPWCnt						int					-- 던전 파티 클리이 횟수
		% dungeonSiglePlayPT													//@DSPlayTime					int					-- 던전 싱글 플래이 시간(초)
		% dungeonParyPlayPT														//@DPPlayTime					int					-- 던전 파티 플래이 시간(초)
		% etcData[US::US_Etc_Pvp_Kill_Count]									//@VsKillCnt					int					-- 대전 Kill 수
		% pvpWinCount															//@VsWinCnt						smallint			-- 대전 승 수
		% pvpLoseCount															//@VsLoseCnt					smallint			-- 대전 패 수
		% trainingcenterPT														//@TrainingPT					int					-- 트레이닝 플래이 시간(초)
		% trainingcenterCount													//@TrainingCnt					int					-- 트레이닝 플래이 횟수
		% etcData[US::US_Etc_TradeCount]										//@P2PCnt						smallint			-- 개인 거래 횟 수
		% etcData[US::US_Etc_Square_Count]										//@SquareCnt					int					-- 광장 출입 횟 수
		% etcData[US::US_Etc_Square_PlayTime]									//@SquarePlayTime				int					-- 광장 머문 시간
		% etcData[US::US_Etc_Trade_Buy_Item]									//@BuyItemCnt					int					-- 구매한 아이템 수
		% etcData[US::US_Etc_Trade_Sell_Item]									//@SellItemCnt					int					-- 판매한 아이템 수
		% etcData[US::US_Etc_Obtained_Item]										//@GetItemCnt					int					-- 획득한 아이템 수
		% reStoneCount															//@ReStoneCnt					int					-- 총 사용한 부활석 수

		)

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}


end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_mapEtcStatistics.size() )
		<< BUILD_LOG( kPacket_.m_mapPvpStatistics.size() )
		<< BUILD_LOG( kPacket_.m_mapDungeonStatistics.size() )
		<< BUILD_LOG( kPacket_.m_mapTCenterStatistics.size() )
		<< BUILD_LOG( kPacket_.m_mapEDStatistics.size() )
		<< BUILD_LOG( GetKGameServer()->GetServerGroupID() )
		<< BUILD_LOG( kPacket_.m_wstrLoginTime	 )
		<< BUILD_LOG( kPacket_.m_wstrLogoutTime	 )
		<< BUILD_LOG( iIsPcBang	 )
		<< BUILD_LOG( kPacket_.m_iAuthLevel	 )
		<< BUILD_LOG( kPacket_.m_iUserUID )
		<< BUILD_LOG( kPacket_.m_uiGiantUID )
		<< BUILD_LOG( kPacket_.m_wstrAccountCreateDate )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< BUILD_LOG( kPacket_.m_wstrNickName )
		<< BUILD_LOG( ( int )kPacket_.m_cUnitClass )
		<< BUILD_LOG( kPacket_.m_iEXP )
		<< BUILD_LOG( (int)kPacket_.m_ucLevel )
		<< BUILD_LOG( kPacket_.m_iED )
		<< BUILD_LOG( kPacket_.m_wstrUnitCreateDate )
		<< BUILD_LOG( etcData[US::US_Etc_NQuestComplete] )
		<< BUILD_LOG( etcData[US::US_Etc_RQuestComplete] )
		<< BUILD_LOG( edData[US::US_ED_PDungeon] )
		<< BUILD_LOG( edData[US::US_ED_PTraining] )
		<< BUILD_LOG( edData[US::US_ED_PQuest] )
		<< BUILD_LOG( edData[US::US_ED_PItemSell] )
		<< BUILD_LOG( edData[US::US_ED_PItemTrade] )
		<< BUILD_LOG( edData[US::US_ED_PPersonalShop] )
		<< BUILD_LOG( edData[US::US_ED_PPostLetter]	 )
		<< BUILD_LOG( edData[US::US_ED_MItemBuy] )
		<< BUILD_LOG( edData[US::US_ED_MItemTrade] )
		<< BUILD_LOG( edData[US::US_ED_MItemRepair]	 )
		<< BUILD_LOG( edData[US::US_ED_MEnchant] )
		<< BUILD_LOG( edData[US::US_ED_MSocket] )
		<< BUILD_LOG( edData[US::US_ED_MAttribEnchant] )
		<< BUILD_LOG( edData[US::US_ED_MManufacture] )
		<< BUILD_LOG( edData[US::US_ED_MDunPenalty]	 )
		<< BUILD_LOG( edData[US::US_ED_MPersonalShop] )
		<< BUILD_LOG( edData[US::US_ED_MOpenPShop] )
		<< BUILD_LOG( edData[US::US_ED_MSellTaxPShop] )
		<< BUILD_LOG( edData[US::US_ED_MBuyTaxPShop] )
		<< BUILD_LOG( edData[US::US_ED_MPostLetter]	 )
		<< BUILD_LOG( edData[US::US_ED_MSendLetterBasic] )
		<< BUILD_LOG( edData[US::US_ED_MSendLetterItem]	)

		<< BUILD_LOG( etcData[US::US_Etc_Used_Spirit] )
		<< BUILD_LOG( etcData[US::US_Etc_AllUsed_Spirit_Count] )
		<< BUILD_LOG( etcData[US::US_Etc_Dungeon_Play_Time]	 )
		<< BUILD_LOG( dungeon[US::US_Etc_Duration_Time] )
		<< BUILD_LOG( etcData[US::US_Etc_PVP_Play_Time] )
		<< BUILD_LOG( dungeonSiglePlayCount )
		<< BUILD_LOG( dungeonParyPlayCount )
		<< BUILD_LOG( dungeon[US::US_D_MC_Win] )
		<< BUILD_LOG( dungeon[US::US_D_MC_Party_Win] )
		<< BUILD_LOG( dungeonSiglePlayPT )
		<< BUILD_LOG( dungeonParyPlayPT	 )
		<< BUILD_LOG( etcData[US::US_Etc_Pvp_Kill_Count] )
		<< BUILD_LOG( pvpWinCount )
		<< BUILD_LOG( pvpLoseCount )
		<< BUILD_LOG( trainingcenterPT )
		<< BUILD_LOG( trainingcenterCount )
		<< BUILD_LOG( etcData[US::US_Etc_TradeCount]	 )
		<< BUILD_LOG( etcData[US::US_Etc_Square_Count] )
		<< BUILD_LOG( etcData[US::US_Etc_Square_PlayTime]	)
		<< BUILD_LOG( etcData[US::US_Etc_Trade_Buy_Item]	 )
		<< BUILD_LOG( etcData[US::US_Etc_Trade_Sell_Item] )
		<< BUILD_LOG( etcData[US::US_Etc_Obtained_Item]	)
		<< BUILD_LOG( reStoneCount	)

		<< END_LOG;
}

IMPL_ON_FUNC( ELOG_CHEAT_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_CheatLog_INS", L"%d, %d, %d, %d, N\'%s\', N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID
		% kPacket_.m_iUnitUID
		% (int)kPacket_.m_cAuthLevel
		% kPacket_.m_wsCheatCode
		% kPacket_.m_wsCheatContents );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< END_LOG;
}

typedef KStatistics KS;

_IMPL_ON_FUNC( ELOG_STAT_ED, KStatisticsData )
{
	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); mit++ )
	{
		if( mit->first.m_vecIntKey.empty() )
		{
			START_LOG( cerr, L"The statistics key is empty." )
				<< END_LOG;

			continue;
		}
		__int64 aiCount[KStatistics::eSIColDB_ED_Max];
		std::vector< __int64 >::iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::eSIColDB_ED_Max; iIndex++ )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		int ieSIColDB_ED_MItemResolve = 0;


		DO_QUERY( L"exec dbo.P_StatsMoneySupply_INS",
			L"%d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% aiCount[KS::eSIColDB_ED_PDungeon]			// @iPDun			bigint
			% aiCount[KS::eSIColDB_ED_PTraining]		// @iPTraining		bigint
			% aiCount[KS::eSIColDB_ED_PQuest]			// @iPQuest			bigint
			% aiCount[KS::eSIColDB_ED_PItemSell]		// @iPItemSell		bigint
			% aiCount[KS::eSIColDB_ED_PItemTrade]		// @iPItemP2P		bigint
			% aiCount[KS::eSIColDB_ED_PPersonalShop]	// @iPStoreP2P		bigint
			% aiCount[KS::eSIColDB_ED_PPostLetter]		// @iPPostP2P		bigint
			% aiCount[KS::eSIColDB_ED_MItemBuy]			// @iMItemBuy		bigint
			% aiCount[KS::eSIColDB_ED_MItemTrade]		// @iMItemP2P		bigint
			% aiCount[KS::eSIColDB_ED_MItemRepair]		// @iMItemRepair	bigint
			% aiCount[KS::eSIColDB_ED_MManufacture]		// @iMItemMake		bigint
			% aiCount[KS::eSIColDB_ED_MDunPenalty]		// @iMDunPenalty	bigint
			% aiCount[KS::eSIColDB_ED_MPersonalShop]	// @iMStoreP2P		bigint
			% aiCount[KS::eSIColDB_ED_MOpenPShop]		// @iMStoreOpen		bigint
			% aiCount[KS::eSIColDB_ED_MEnchant]			// @iMEnchant		bigint
			% aiCount[KS::eSIColDB_ED_MSocket]			// @iMSocket		bigint
			% aiCount[KS::eSIColDB_ED_MPostLetter]		// @iMPostP2P		bigint
			% aiCount[KS::eSIColDB_ED_MSendLetterBasic]	// @iMPostBasic		bigint
			% aiCount[KS::eSIColDB_ED_MSendLetterItem]	// @iMPostItem		bigint
			% aiCount[KS::eSIColDB_ED_MAttribEnchant]	// @iMAttribute		bigint
			% aiCount[KS::eSIColDB_ED_MSellTaxPShop]	// @iMStoreP2PTax	bigint
			//{{ 2010. 03. 31  최육사	거래게시판
			% aiCount[KS::eSIColDB_ED_MBuyTaxPShop]		// @iMStoreP2PTax	bigint
			//}}
			//{{ 2011. 05. 11	최육사	대리상인
//#ifdef SERV_PSHOP_AGENCY
			% aiCount[KS::eSIColDB_ED_MPShopAgency]
			% aiCount[KS::eSIColDB_ED_MBuyTaxPShopAgency]
			% aiCount[KS::eSIColDB_ED_PPShopPickUp]
//#endif SERV_PSHOP_AGENCY
			//}}
			//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
//#ifdef SERV_CUBE_OPEN_ED_CONDITION
			% aiCount[KS::eSIColDB_ED_POpenCube]
//#endif SERV_CUBE_OPEN_ED_CONDITION
			//}
//#ifdef SERV_SEPARATE_DUNGEON_FIELD_ED_LOG// 작업날짜: 2013-05-08	// 박세훈
			% aiCount[KS::eSIColDB_ED_PBattleField]			// @iPBattleFieldED			bigint
			% aiCount[KS::eSIColDB_ED_MCoboExpressTicketED]	// @iMCoboExpressTicketED	bigint
//#endif // SERV_SEPARATE_DUNGEON_FIELD_ED_LOG
//#ifdef SERV_MULTI_RESOLVE
			% ieSIColDB_ED_MItemResolve
//#endif SERV_MULTI_RESOLVE
//#ifdef SERV_NEW_ITEM_SYSTEM_2013_05// 작업날짜: 2013-06-17	// 박세훈
			% aiCount[KS::eSIColDB_ED_MItemEvaluate]		// @iMItemEvaluate	bigint
//#endif // SERV_NEW_ITEM_SYSTEM_2013_05
//#ifdef SERV_GOOD_ELSWORD // 작업날짜 : 2013-11-08 // 우상혁
            % aiCount[KS::eSIColDB_ED_ExpandInventory]
            % aiCount[KS::eSIColDB_ED_ExpandBank]
//#endif //SERV_GOOD_ELSWORD
//#ifdef SERV_SKILL_PAGE_SYSTEM
			% aiCount[KS::eSIColDB_ED_ExpandSkillPage]
//#endif // SERV_SKILL_PAGE_SYSTEM
			);

			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );

				m_kODBC.EndFetch();
			}

		START_LOG( clog2, L"ED statistics [check]" )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PDungeon] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PTraining] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PQuest] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PItemSell] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PItemTrade] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PPersonalShop] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_PPostLetter] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MItemBuy] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MItemTrade] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MItemRepair] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MManufacture] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MDunPenalty] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MPersonalShop] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MOpenPShop] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MEnchant] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MSocket] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MPostLetter] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MSendLetterBasic] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MSendLetterItem] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MAttribEnchant] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MSellTaxPShop] )
			//{{ 2010. 03. 31  최육사	거래게시판
			<< BUILD_LOG( aiCount[KStatistics::eSIColDB_ED_MBuyTaxPShop] )
			//}}
			//{{ 2011. 05. 11	최육사	대리상인
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_MPShopAgency] )
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_MBuyTaxPShopAgency] )
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_PPShopPickUp] )
			//}}
			//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_POpenCube] )
			//}
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_PBattleField] )
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_MCoboExpressTicketED] )

			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_MItemEvaluate] )
            << BUILD_LOG( aiCount[KS::eSIColDB_ED_ExpandInventory] )
            << BUILD_LOG( aiCount[KS::eSIColDB_ED_ExpandBank] )
//#ifdef SERV_SKILL_PAGE_SYSTEM
			<< BUILD_LOG( aiCount[KS::eSIColDB_ED_ExpandSkillPage] )
//#endif // SERV_SKILL_PAGE_SYSTEM

			<< BUILD_LOG( iOK );
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}

_IMPL_ON_FUNC( ELOG_STAT_QUEST, KStatisticsData )
{
	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); mit++ )
	{
		if( mit->first.m_vecIntKey.empty() )
		{
			START_LOG( cerr, L"The statistics key is empty." )
				<< END_LOG;

			continue;
		}

		int iQuestID = mit->first.m_vecIntKey[0];
		__int64 aiCount[KStatistics::eSIColQuest_Max];
		std::vector< __int64 >::iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::eSIColQuest_Max; iIndex++ )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iQuestID )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		DO_QUERY( L"exec dbo.P_StatsQuestInfo_SET", L"%d, %d, %d, %d, %d, %d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iQuestID											// @iQuestID		[bigint]
			% aiCount[KStatistics::eSIColQuest_Accept]			// @iAccept			[smallint]
			% aiCount[KStatistics::eSIColQuest_GiveUp]			// @iGiveUp			[smallint]
			% aiCount[KStatistics::eSIColQuest_Complete]		// @iComplete		[smallint]
			% aiCount[KStatistics::eSIColQuest_AcceptLevel]		// @iAcceptLevel	[smallint]
			% aiCount[KStatistics::eSIColQuest_GiveUpLevel]		// @iGiveUpLevel	[smallint]
			% aiCount[KStatistics::eSIColQuest_CompleteLevel]	// @iCompleteLevel  [smallint]
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		START_LOG( clog2, L"FPS statistics [check]" )
			<< BUILD_LOG( iQuestID )
			<< BUILD_LOG( aiCount[KStatistics::eSIColQuest_Accept] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColQuest_GiveUp] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColQuest_Complete] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColQuest_AcceptLevel] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColQuest_GiveUpLevel]	)
			<< BUILD_LOG( aiCount[KStatistics::eSIColQuest_CompleteLevel] )
			<< BUILD_LOG( iOK );
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}

//{{ 2011. 02. 28	최육사	사내 통합 통계
//}}

_IMPL_ON_FUNC( ELOG_STAT_DISCONNECT_USERFSM, KStatisticsData )
{
	//////////////////////////////////////////////////////////////////////////
	// 이거 계속 남겨야할 필요가 있을까? 볼 일이 없는데.. 삭제할지 말지 결정하자..
	//////////////////////////////////////////////////////////////////////////

	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); mit++ )
	{
		if( mit->first.m_vecIntKey.empty() )
		{
			START_LOG( cerr, L"The statistics key is empty." )
				<< END_LOG;

			continue;
		}

		int iServerUID = mit->first.m_vecIntKey[0];
		__int64 aiCount[KStatistics::eSIColDU_Num];
		std::vector< __int64 >::iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::eSIColDU_Num; iIndex++ )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		DO_QUERY( L"exec dbo.P_LDisconnectUserFSM_INS",
			L"%d,"
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iServerUID
			% aiCount[KStatistics::eSIColDU_S_INIT]
			% aiCount[KStatistics::eSIColDU_S_UID_UPDATED]
			% aiCount[KStatistics::eSIColDU_S_LOGINED]
			% aiCount[KStatistics::eSIColDU_S_SERVER_SELECT]
			% aiCount[KStatistics::eSIColDU_S_UNIT_SELECT]
			% aiCount[KStatistics::eSIColDU_S_VILLAGE_MAP]
			% aiCount[KStatistics::eSIColDU_S_LOCAL_MAP]
			% aiCount[KStatistics::eSIColDU_S_WORLD_MAP]
			% aiCount[KStatistics::eSIColDU_S_CHANNEL]
			% aiCount[KStatistics::eSIColDU_S_ROOM]
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		START_LOG( clog2, L"Disconnect user FSM statistics." )
			<< BUILD_LOG( iServerUID )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_INIT] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_UID_UPDATED] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_LOGINED] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_SERVER_SELECT] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_UNIT_SELECT] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_VILLAGE_MAP] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_LOCAL_MAP] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_WORLD_MAP] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_CHANNEL] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDU_S_ROOM] )
			<< BUILD_LOG( iOK );
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}

_IMPL_ON_FUNC( ELOG_STAT_DISCONNECT_REASON, KStatisticsData )
{
	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); ++mit )
	{
		if( mit->first.m_vecIntKey.empty() )
		{
			START_LOG( cerr, L"The statistics key is empty." )
				<< END_LOG;

			continue;
		}

		int iServerUID = mit->first.m_vecIntKey[0];
		__int64 aiCount[KStatistics::eSIColDR_Num];
		std::vector< __int64 >::iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::eSIColDR_Num; ++iIndex )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		DO_QUERY( L"exec dbo.P_LDisconnectReason_INS",
			L"%d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, "
			//{{ 2011. 09. 29  김민성	셧다운제 도입
			//#ifdef SERV_SHUTDOWN_SYSTEM
			L"%d",
			//#endif SERV_SHUTDOWN_SYSTEM
			//}}
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iServerUID
			% aiCount[0]	// [0]  이유 없음
			% aiCount[1]	// [1]  Unknown
			% aiCount[2]	// [2]  HackShield 인증 실패 (클라이언트 응답 없음)
			% aiCount[3]	// [3]  PC방 인증 지연
			% aiCount[4]	// [4]  메인버전 다름
			% aiCount[5]	// [5]  수용 인원 초과
			% aiCount[6]	// [6]  넥슨 인증 실패
			% aiCount[7]	// [7]  동일 서버 이중 접속
			% aiCount[8]	// [8]  계정 블록
			% aiCount[9]	// [9]  IP 제한
			% aiCount[10]	// [10] NProtect 처음 인증
			% aiCount[11]	// [11] PC방 인증 실패
			% aiCount[12]	// [12] PC방 인증 결과 이상
			% aiCount[13]	// [13] 사내 계정 생성
			% aiCount[14]	// [14] 정상 종료
			% aiCount[15]	// [15] 클라이언트 크래쉬
			% aiCount[16]	// [16] 클라이언트 해킹
			% aiCount[17]	// [17] HackShield 인증 실패 (응답 틀림)
			% aiCount[18]	// [18] PC방 인증 서버의 종료 요청
			% aiCount[19]	// [19] 서버 Shutdown
			% aiCount[20]	// [20] 접속 종료 감지 (Session)
			% aiCount[21]	// [21] WSAECONNRESET (Socket Object)
			% aiCount[22]	// [22] WSARecvFailed (Socket Object)
			% aiCount[23]	// [23] WSASendFailed (Socket Object) - OnSendCompleted()
			% aiCount[24]	// [24] SendBufferFull (Socket Object)
			% aiCount[25]	// [25] WSASendFailed (Socket Object) - SendData()
			% aiCount[26]	// [26] 운영자 강제 접속 종료
			% aiCount[27]	// [27] 타 서버간 이중접속
			% aiCount[28]	// [28] 유저 등록 중 에러
			% aiCount[29]	// [29] 하트 비트
			% aiCount[30]	// [30] IOCP_GetStatus Closed by local machine
			% aiCount[31]	// [31] nProtect 해킹 감지
			% aiCount[32]	// [32] 채널이동으로 인한 접속종료
			% aiCount[33]	// [33] 채널이동 실패로 인한 접속종료
			% aiCount[34]	// [34] 트래픽 유발로 인한 접속종료
			% aiCount[35]	// [35] 게임서버 계정 블록
			//{{ 2011. 08. 09	최육사	머신ID 블럭기능
			//#ifdef SERV_MACHINE_ID_BLOCK
			% aiCount[36]	// [36] 머신ID 블럭
			//#endif SERV_MACHINE_ID_BLOCK
			//}}
			//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
			//#ifdef SERV_MACHINE_ID_DUPLICATE_CHECK
			% aiCount[37]	// [37] 머신ID 중복 접속
			//#endif SERV_MACHINE_ID_DUPLICATE_CHECK
			//}}
			//{{ 2011. 09. 29  김민성	셧다운제 도입
			//#ifdef SERV_SHUTDOWN_SYSTEM
			% aiCount[38]	// [38] 셧다운제로 인한 접속 불가
			//#endif SERV_SHUTDOWN_SYSTEM
			//}}
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		START_LOG( clog2, L"Disconnect reason statistics." )
			<< BUILD_LOG( iServerUID )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_No_Reason] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Unknown] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_HackShield_Auth_Fail] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_PCBang_Auth_Delay] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Diff_Main_Ver] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Limit_User_Num] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Nexon_Auth_Fail] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Same_Server_Connect] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Account_Block] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_IP_Block] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_NProtect_Auth_First] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_PCBang_Auth_Fail] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_PCBang_Auth_Wrong] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Internal_Account] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Normal_Disconnect] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Client_Crash] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Client_Hacking] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_HackShield_Wrong_Ack] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_PCBang_Auth_Server] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Server_Shutdown] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Session_Close] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_WSAECONNRESET] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_WSARecvFailed] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_WSASendFailed_OnSendComp] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_SendBufferFull] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_WSASendFailed] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_GM_Kick_User] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Diff_Server_Connect] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_User_Reg_Error] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Heart_Beat] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_IOCP_GetStatus] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_nProtect_Hacking] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Channel_Change] )
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Channel_Change_Fail] )
			//{{ 2009. 10. 22  최육사	트래픽 공격 방지
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Traffic_Attack] )
			//}}
			//{{ 2010. 06. 22  최육사	계정블럭기능
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_GameServer_Account_Block] )
			//}}
			//{{ 2011. 08. 09	최육사	머신ID 블럭기능
//#ifdef SERV_MACHINE_ID_BLOCK
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_MachineID_Block] )
//#endif SERV_MACHINE_ID_BLOCK
			//}}
			//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
//#ifdef SERV_MACHINE_ID_DUPLICATE_CHECK
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Same_MachineID_Connect] )
//#endif SERV_MACHINE_ID_DUPLICATE_CHECK
			//}}
			//{{ 2011. 09. 29  김민성	셧다운제 도입
//#ifdef SERV_SHUTDOWN_SYSTEM
			<< BUILD_LOG( aiCount[KStatistics::eSIColDR_Policy_Shutdown_Disconnect] )
//#endif SERV_SHUTDOWN_SYSTEM
			//}}
			<< BUILD_LOG( iOK );

		//{{ 2012. 01. 25  김민성	종료 통계 개편
		CTime tCurr = CTime::GetCurrentTime();
		std::wstring wstrCurTime = (const wchar_t*)tCurr.Format( _T("%Y-%m-%d %H:%M:%S") );
		for( int i = 0 ; i < KStatistics::eSIColDR_Num ; ++i )
		{
			if( aiCount[i] > 0 )
			{
				DO_QUERY( L"exec dbo.P_LDisconnectReason_New_INS", L"%d, %d, N\'%s\', %d, %d",
					% KBaseServer::GetKObj()->GetServerGroupID()
					% iServerUID
					% wstrCurTime
					% i
					% aiCount[i] );

				if( m_kODBC.BeginFetch() )
				{
					FETCH_DATA( iOK );
					m_kODBC.EndFetch();
				}

				START_LOG( clog2, L"Disconnect reason statistics update." )
					<< BUILD_LOG( iServerUID )
					<< BUILD_LOG( wstrCurTime )
					<< BUILD_LOG( i )
					<< BUILD_LOG( aiCount[i] )
					<< BUILD_LOG( iOK );
			}
		}
		//}}

	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}

_IMPL_ON_FUNC( DBE_DELETE_UNIT_STAT_NOT, KEGS_DELETE_UNIT_REQ )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_UnitStatistics_Delete_SET", L"%d, %d", % KBaseServer::GetKObj()->GetServerGroupID() % kPacket_.m_iUnitUID );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< END_LOG;
}

IMPL_ON_FUNC( DBE_UPDATE_UNIT_CLASS_NOT )
{
    int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_UnitStatistics_UnitClass_UPD", L"%d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% ( int )kPacket_.m_cUnitClass
		);

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );

        m_kODBC.EndFetch();
    }

end_proc:
    LOG_SUCCESS( iOK == NetError::NET_OK )
        << BUILD_LOG( iOK )
        << END_LOG;
}

IMPL_ON_FUNC( ELOG_RESURRECTION_STONE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_ResurrectionStoneLog_INS", L"%d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_UnitUID
		% kPacket_.m_iDungeonID
		% kPacket_.m_wstrRegDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_UnitUID )
		<< BUILD_LOG( kPacket_.m_iDungeonID )
		<< BUILD_LOG( kPacket_.m_wstrRegDate )
		<< END_LOG;
}

IMPL_ON_FUNC( ELOG_RESOLVE_JACKPOT_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_StatsJackPot_INS", L"%d, %d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_UnitUID
		% kPacket_.m_iResolveItemID
		% kPacket_.m_iElapsedTime
		% kPacket_.m_wstrRegDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );

		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_UnitUID )
		<< BUILD_LOG( kPacket_.m_iResolveItemID )
		<< BUILD_LOG( kPacket_.m_iElapsedTime )
		<< BUILD_LOG( kPacket_.m_wstrRegDate )
		<< END_LOG;
}

IMPL_ON_FUNC( ELOG_GET_PURCHASED_CASH_ITEM_NOT )
{
    int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_StatsCashLog_INS", L"%d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemID
		% kPacket_.m_sPeriod
		% kPacket_.m_iQuantity
		% (int)kPacket_.m_ucLevel
		% (int)kPacket_.m_cUnitClass
		);

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );

        m_kODBC.EndFetch();
    }

end_proc:
    LOG_SUCCESS( iOK == NetError::NET_OK )
        << BUILD_LOG( iOK )
        << BUILD_LOG( kPacket_.m_iUnitUID )
        << BUILD_LOG( kPacket_.m_iItemID )
        << BUILD_LOG( kPacket_.m_sPeriod )
        << BUILD_LOG( kPacket_.m_iQuantity )
		<< BUILD_LOGc( kPacket_.m_ucLevel )
		<< BUILD_LOGc( kPacket_.m_cUnitClass )
        << END_LOG;
}

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

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}

IMPL_ON_FUNC( ELOG_ITEM_ABUSER_LOG_NOT )
{
	int iOK = NetError::NET_OK;

	std::vector< KItemAbuserLogInfo >::const_iterator vit;
	for( vit = kPacket_.m_vecItemAbuserLog.begin(); vit != kPacket_.m_vecItemAbuserLog.end(); ++vit )
	{
		const KItemAbuserLogInfo& kLogInfo = *vit;

		DO_QUERY( L"exec dbo.P_StatsGetItemLog_INS", L"%d, %d, %d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% kPacket_.m_iUnitUID
			% kLogInfo.m_iRewardState
			% kLogInfo.m_iItemID
			% kLogInfo.m_iQuantity );

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );

			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to update the item-abuser log in the database!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( kLogInfo.m_iRewardState )
				<< BUILD_LOG( kLogInfo.m_iItemID )
				<< BUILD_LOG( kLogInfo.m_iQuantity )
				<< END_LOG;
		}
	}
end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< END_LOG;
}

IMPL_ON_FUNC( ELOG_ENCHANT_ABUSER_LOG_NOT )
{
	int iOK = NetError::NET_OK;

	DO_QUERY( L"exec dbo.P_StatsEnchantLog_INS", L"%d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemID
		% kPacket_.m_iItemUID
		% (int)kPacket_.m_cEnchantLevel
		% kPacket_.m_iEnchantResult
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to update the item-abuser log in the database!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iItemID )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< BUILD_LOG( kPacket_.m_cEnchantLevel )
			<< BUILD_LOG( kPacket_.m_iEnchantResult )
			<< END_LOG;
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< END_LOG;
}

IMPL_ON_FUNC( ELOG_ED_ABUSER_LOG_NOT )
{
	DO_QUERY( L"exec dbo.P_StatsMoneyLog_INS", L"%d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iState
		% kPacket_.m_iBeforeED
		% kPacket_.m_iAfterED
		% kPacket_.m_iED
		);

end_proc:
	return;
}

//{{ 2008. 10. 22  최육사	캐쉬 분해 로그
IMPL_ON_FUNC( ELOG_CASH_ITEM_RESOLVE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsDecomposition_INS", L"%d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemUID
		% kPacket_.m_iResultQuantity
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to update cash item dismantling log in the database!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< BUILD_LOG( kPacket_.m_iResultQuantity )
			<< END_LOG;
	}

	return;
}
//}}

//{{ 2008. 11. 10  최육사
IMPL_ON_FUNC( ELOG_ENCHANT_ITEM_BREAK_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsItemBreak_INS", L"%d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemUID
		% kPacket_.m_iItemID
		% static_cast<int>(kPacket_.m_cEnchantLevel)
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to update the enhancement destruction log in the database!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< BUILD_LOG( kPacket_.m_iItemID )
			<< BUILD_LOGc( kPacket_.m_cEnchantLevel )
			<< END_LOG;
	}

	return;
}
//}}

//{{ 2008. 10. 31  최육사	어뷰저 리스트
//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
IMPL_ON_FUNC( DBE_ABUSER_LIST_REQ )
{
	KDBE_ABUSER_LIST_ACK kPacket;
	std::map< int, int >::const_iterator mit;
	std::map< int, KAbuserList >::iterator mitAL;

	// Release Tick 얻기
	DO_QUERY( L"exec dbo.P_ReleaseTick_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		int iAbuserType = 0;
		int iReleaseTick = 0;

		FETCH_DATA( iAbuserType
			>> iReleaseTick );

		mit = kPacket_.m_mapReleaseTick.find( iAbuserType );
		if( mit != kPacket_.m_mapReleaseTick.end() )
		{
			// DB에 저장된 ReleaseTick이 더 크다면..
			if( iReleaseTick > mit->second )
			{
				KAbuserList kAbuserInfo;
				kAbuserInfo.m_iReleaseTick = iReleaseTick;
				kPacket.m_mapAbuserList.insert( std::make_pair( iAbuserType, kAbuserInfo ) );
			}
		}
		else
		{
			START_LOG( clog, L"This ReleaseTick is unrelated to the abuser list." )
				<< BUILD_LOG( iAbuserType );
		}
	}

	// Abuser List 얻기
	for( mitAL = kPacket.m_mapAbuserList.begin(); mitAL != kPacket.m_mapAbuserList.end(); ++mitAL )
	{
		DO_QUERY( L"exec dbo.P_AbuserList_SEL", L"%d, %d",	% KBaseServer::GetKObj()->GetServerGroupID()
															% mitAL->first );

		while( m_kODBC.Fetch() )
		{
			UidType iUnitUID = 0;

			FETCH_DATA( iUnitUID );

			if( iUnitUID <= 0 )
				continue;

			mitAL->second.m_vecAbuserList.push_back( iUnitUID );
		}
	}

end_proc:
	SendToServer( DBE_ABUSER_LIST_ACK, kPacket );
}
//}}
//}}

IMPL_ON_FUNC( DBE_REG_ABUSER_AUTOMATIC_NOT )
{
	std::map< int, UidType >::const_iterator mit;
	for( mit = kPacket_.m_mapRegAbuserInfo.begin(); mit != kPacket_.m_mapRegAbuserInfo.end(); ++mit )
	{
		const int iType = mit->first;
		const UidType iUID = mit->second;

		int iOK = NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_AbuserList_INS", L"%d, %d, %d",  % KBaseServer::GetKObj()->GetServerGroupID()
																% iType
																% iUID );

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

end_proc:
		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to automatically register the abuser!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( iType )
				<< BUILD_LOG( iUID )
				<< END_LOG;
		}
	}
}

//{{ 2008. 12. 18  최육사	보조제 로그
IMPL_ON_FUNC( ELOG_ENCHANT_SUPPORT_MATERIAL_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가 - 로그 남기기
	// 파괴 방지까지 합쳐서 한번에 적용.
	DO_QUERY( L"exec dbo.P_statsEnchantAid_INS", L"%d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iItemUID
		% static_cast<int>(kPacket_.m_cBeforeLevel)
		% static_cast<int>(kPacket_.m_cAfterLevel)
		);
	//}}

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		//{{ 2011.5.23 지헌 : 강화시 성공 확률 증가 아이템 추가 - 로그 남기기
		// 파괴 방지까지 합쳐서 한번에 적용.
		START_LOG( cerr, L"Failed to record the enhancement aid log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< BUILD_LOGc( kPacket_.m_cBeforeLevel )
			<< BUILD_LOGc( kPacket_.m_cAfterLevel )
			<< END_LOG;
		//}}
	}
	return;
}
//}}

//{{ 2008. 12. 22  최육사	깨짐 복구 로그
IMPL_ON_FUNC( ELOG_RESTORE_ITEM_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsItemResurrection_INS", L"%d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemUID
		% kPacket_.m_iBeforeEnchantLv
		% kPacket_.m_iAfterEnchantLv
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record the enhancement-break recovery log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< BUILD_LOG( kPacket_.m_iBeforeEnchantLv )
			<< BUILD_LOG( kPacket_.m_iAfterEnchantLv )
			<< END_LOG;
	}
	return;
}
//}}

//{{ 2008. 12. 22  최육사	부여 로그
IMPL_ON_FUNC( ELOG_ATTACH_ITEM_ENCHANT_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsCharmItemLog_Direct_INS", L"%d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iAttachItemID
		% kPacket_.m_iDestItemUID
		% kPacket_.m_iBeforeEnchantLv
		% kPacket_.m_iAfterEnchantLv
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record the enhancement assignment log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iAttachItemID )
			<< BUILD_LOG( kPacket_.m_iDestItemUID )
			<< BUILD_LOG( kPacket_.m_iBeforeEnchantLv )
			<< BUILD_LOG( kPacket_.m_iAfterEnchantLv )
			<< END_LOG;
	}
	return;
}
//}}

//{{ 2008. 12. 30  최육사	매력아이템 로그
IMPL_ON_FUNC( ELOG_CHARM_ITEM_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsCharmItemLog_INS", L"%d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iCharmItemID
		% kPacket_.m_iUnitUID
		% kPacket_.m_iResultItemID
		% kPacket_.m_iQuantity
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record charm item usage log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iCharmItemID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iResultItemID )
			<< BUILD_LOG( kPacket_.m_iQuantity )
			<< END_LOG;
	}
	return;
}
//}}

//{{ 2009. 4. 14  최육사	닉네임 변경
IMPL_ON_FUNC( ELOG_DELETE_NICK_NAME_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsNickNameChange_INS", L"%d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrNickName );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to write nickname reset log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< END_LOG;
	}
	return;
}

IMPL_ON_FUNC( ELOG_CHANGE_NICK_NAME_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsNickNameRestore_INS", L"%d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrNickName );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to write nickname change log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< END_LOG;
	}
	return;
}
//}}

IMPL_ON_FUNC( ELOG_INIT_SKILL_TREE_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsAllSkillDelete_INS", L"%d, %d, %d, %d, %d",
														% KBaseServer::GetKObj()->GetServerGroupID()
														% kPacket_.m_iUnitUID
														% kPacket_.m_iItemUID
														% kPacket_.m_iSPoint
														% kPacket_.m_iCSPoint );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record skill tree reset log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iItemUID )
			<< BUILD_LOG( kPacket_.m_iSPoint )
			<< BUILD_LOG( kPacket_.m_iCSPoint )
			<< END_LOG;
	}
	return;
}



IMPL_ON_FUNC( ELOG_INSERT_CASH_SKILL_POINT_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsCSPLog_INS", L"%d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iUnitLevel
		% kPacket_.m_iMaxCSPoint
		% kPacket_.m_iPeriod
		% kPacket_.m_bUpdateEndDateOnly );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record cash skill point usage start!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iUnitLevel )
			<< BUILD_LOG( kPacket_.m_iMaxCSPoint )
			<< BUILD_LOG( kPacket_.m_iPeriod )
			<< BUILD_LOG( kPacket_.m_bUpdateEndDateOnly )
			<< END_LOG;
	}
	return;
}

//{{ 2009. 7. 22  최육사	메가폰로그
IMPL_ON_FUNC( ELOG_MEGAPHONE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsMegaphone_INS", L"%d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemID );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record megaphone usage log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iItemID )
			<< END_LOG;
	}
	return;
}
//}}

//{{ 2009. 9. 1  최육사		비정상종료사유
IMPL_ON_FUNC( ELOG_ABNORMAL_DISCONNECT )
{
	std::map< DWORD, int >::const_iterator mit;
	for( mit = kPacket_.m_mapAbnormalDisconnect.begin(); mit != kPacket_.m_mapAbnormalDisconnect.end(); ++mit )
	{
		DO_QUERY( L"exec dbo.P_StatsAbnormalReason_SEL", L"%d, %d, %d, %d", % KBaseServer::GetKObj()->GetServerGroupID()
																			% kPacket_.m_iServerUID
																			% mit->first
																			% mit->second );
		continue;

end_proc:
		START_LOG( cerr, L"Failed to update abnormal shutdown statistics in the database!" )
			<< BUILD_LOG( kPacket_.m_iServerUID )
			<< BUILD_LOG( mit->first )
			<< BUILD_LOG( mit->second )
			<< END_LOG;
	}
}
//}}

//{{ 2009. 9. 9  최육사		밀봉
IMPL_ON_FUNC( ELOG_USE_SEAL_ITEM_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsSealLog_INS", L"%d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iSealItemUID
		% kPacket_.m_iDestItemUID
		% static_cast<int>(kPacket_.m_ucSealCount)
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record sealing item usage log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iDestItemUID )
			<< BUILD_LOG( kPacket_.m_iSealItemUID )
			<< BUILD_LOGc( kPacket_.m_ucSealCount )
			<< END_LOG;
	}
	return;
}
//}}

//{{ 2009. 10. 22  최육사	트래픽 유발 유저 해킹유저로 자동등록
IMPL_ON_FUNC( ELOG_TRAFFIC_ATTACKER_LOG_NOT )
{
	// 로그 기록
	DO_QUERY( L"exec dbo.P_statsHackUser_INS", L"%d, N\'%s\', %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_wstrUserID
		% kPacket_.m_iChannelID
		);

	return;

end_proc:
	START_LOG( cerr, L"Failed to record traffic attacker log!" )
		<< BUILD_LOG( kPacket_.m_wstrUserID )
		<< BUILD_LOG( kPacket_.m_iChannelID )
		<< END_LOG;
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

//{{ 2010. 06. 30  최육사	근성도 통계

//{{ 2012. 11. 14	박세훈	Field PT 로그 추가
IMPL_ON_FUNC( DBE_SPIRIT_STATISTICS_NOT )
{
	int iOK = NetError::NET_OK;

	DO_QUERY( L"exec dbo.P_StatsSpiritLog_INS", L"%d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iSpiritPlayTime
		% kPacket_.m_iDurationTime
		% kPacket_.m_iUsedSpirit
		);

	return;

end_proc:
	START_LOG( cerr, L"Failed to update stamina statistics in the database!" )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< BUILD_LOG( kPacket_.m_iSpiritPlayTime )
		<< BUILD_LOG( kPacket_.m_iDurationTime )
		<< BUILD_LOG( kPacket_.m_iUsedSpirit )
		<< END_LOG;
}
//}}

//}}



//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
//}}

//{{ 2011. 01. 05	최육사	속성 부적

IMPL_ON_FUNC( ELOG_ATTRIB_ATTACH_ITEM_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsAttrib_AttachLog_INS", L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iAttachItemID
		% kPacket_.m_iDestItemUID
		% static_cast<int>(kPacket_.m_kBeforeAttribEnchantInfo.m_cAttribEnchant0)
		% static_cast<int>(kPacket_.m_kBeforeAttribEnchantInfo.m_cAttribEnchant1)
		% static_cast<int>(kPacket_.m_kBeforeAttribEnchantInfo.m_cAttribEnchant2)
		% static_cast<int>(kPacket_.m_kAfterAttribEnchantInfo.m_cAttribEnchant0)
		% static_cast<int>(kPacket_.m_kAfterAttribEnchantInfo.m_cAttribEnchant1)
		% static_cast<int>(kPacket_.m_kAfterAttribEnchantInfo.m_cAttribEnchant2)
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record attribute assignment log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iAttachItemID )
			<< BUILD_LOG( kPacket_.m_iDestItemUID )
			<< BUILD_LOGc( kPacket_.m_kBeforeAttribEnchantInfo.m_cAttribEnchant0 )
			<< BUILD_LOGc( kPacket_.m_kBeforeAttribEnchantInfo.m_cAttribEnchant1 )
			<< BUILD_LOGc( kPacket_.m_kBeforeAttribEnchantInfo.m_cAttribEnchant2 )
			<< BUILD_LOGc( kPacket_.m_kAfterAttribEnchantInfo.m_cAttribEnchant0 )
			<< BUILD_LOGc( kPacket_.m_kAfterAttribEnchantInfo.m_cAttribEnchant1 )
			<< BUILD_LOGc( kPacket_.m_kAfterAttribEnchantInfo.m_cAttribEnchant2 )
			<< END_LOG;
	}
}

//}}

//{{ 2011. 01. 17	최육사	캐릭터 카운트 정보
IMPL_ON_FUNC( ELOG_USER_CHARACTER_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LCharacterLog_INS", L"%d, N\'%s\', %d, N\'%s\', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, N\'%s\', N\'%s\', %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_wstrRegDate
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrNickName
		% kPacket_.m_bOnlyOneUnit
		% kPacket_.m_iServerUID
		% kPacket_.m_sLogType
		% static_cast<int>(kPacket_.m_ucLevel)
		% static_cast<int>(kPacket_.m_cUnitClass)
		% kPacket_.m_iDungeonID
		% kPacket_.m_iTotalDungeonPlayTime
		% kPacket_.m_iTotalPvpPlayTime
		% kPacket_.m_iDurationTime
		% kPacket_.m_iUserUID
		% kPacket_.m_wstrUnitCreateDate
		% kPacket_.m_wstrAccountCreateDate
		% kPacket_.m_iProductNo
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record character log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< BUILD_LOG( kPacket_.m_bOnlyOneUnit )
			<< BUILD_LOG( kPacket_.m_iServerUID )
			<< BUILD_LOG( kPacket_.m_sLogType )
			<< BUILD_LOGc( kPacket_.m_ucLevel )
			<< BUILD_LOGc( kPacket_.m_cUnitClass )
			<< BUILD_LOG( kPacket_.m_iDungeonID )
			<< BUILD_LOG( kPacket_.m_iTotalDungeonPlayTime )
			<< BUILD_LOG( kPacket_.m_iTotalPvpPlayTime )
			<< BUILD_LOG( kPacket_.m_iDurationTime )
			<< BUILD_LOG( kPacket_.m_wstrUnitCreateDate )
			<< BUILD_LOG( kPacket_.m_wstrAccountCreateDate )
			<< BUILD_LOG( kPacket_.m_iProductNo )
			<< END_LOG;
	}
}
//}}

//{{ 2011. 02. 09	최육사	캐쉬템 구매 로그
IMPL_ON_FUNC( ELOG_BUY_CASH_ITEM_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LCashSalesLog_INS", L"%d, %d, %d, %d, %d, %d, %d, %d, N\'%s\', %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID
		% kPacket_.m_iUnitUID
		% static_cast<int>(kPacket_.m_cUnitClass)
		% static_cast<int>(kPacket_.m_ucLevel)
		% kPacket_.m_iProductNo
		% kPacket_.m_iBuyQuantity
		% kPacket_.m_iTotalSalePrice
		% kPacket_.m_wstrRegDate
		% kPacket_.m_bIsFirstBuy
		% static_cast<int>(kPacket_.m_cBuyType)
		% kPacket_.m_wstrReceiverUserID
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record cash item log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
            << BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOGc( kPacket_.m_cUnitClass )
			<< BUILD_LOGc( kPacket_.m_ucLevel )
			<< BUILD_LOG( kPacket_.m_iProductNo )
			<< BUILD_LOG( kPacket_.m_iBuyQuantity )
			<< BUILD_LOG( kPacket_.m_iTotalSalePrice )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< BUILD_LOG( kPacket_.m_bIsFirstBuy )
			<< BUILD_LOGc( kPacket_.m_cBuyType )
			//{{ 2011. 10. 11    김민성    캐쉬 아이템 선물 할 때 받는 유저의 정보 DB에 기록하기
			<< BUILD_LOG( kPacket_.m_wstrReceiverUserID )
			//}}
			<< END_LOG;
	}
}
//}}

//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
IMPL_ON_FUNC( ELOG_CASH_PRODUCT_LIST_UPDATE_NOT )
{
	// 캐쉬 상품 리스트를 먼저 비운다.
//
//	DO_QUERY_NO_ARG( L"exec dbo.lup_delete_Product_list" ); // 주석 풀면 P_LProductList_DEL 이거 써야함

	// 캐쉬 상품 리스트를 다시 채워 넣는다.
	for( const KCashPruductInfo& kCashProductInfo : kPacket_.m_vecCashProductList )
	{
		int iOK = NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_LProductList_INS", L"%d, %d, %d, %d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% kCashProductInfo.m_ulProductNo
			% kCashProductInfo.m_sProductExpire
			% kCashProductInfo.m_usProductPieces
			% kCashProductInfo.m_iItemID
			% kCashProductInfo.m_ulSalePrice
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			//START_LOG( cerr, L"캐쉬템 로그 등록 실패!" )
			//	<< BUILD_LOG( iOK )
			//	<< BUILD_LOG( kPacket_.m_iUserUID )
			//	<< BUILD_LOG( kPacket_.m_iUnitUID )
			//	<< BUILD_LOGc( kPacket_.m_cUnitClass )
			//	<< BUILD_LOGc( kPacket_.m_ucLevel )
			//	<< BUILD_LOG( kPacket_.m_iProductNo )
			//	<< BUILD_LOG( kPacket_.m_iBuyQuantity )
			//	<< BUILD_LOG( kPacket_.m_iTotalSalePrice )
			//	<< BUILD_LOG( kPacket_.m_wstrRegDate )
			//	<< BUILD_LOG( kPacket_.m_iServerGroupID )
			//	<< BUILD_LOG( kPacket_.m_bIsFirstBuy )
			//	<< BUILD_LOGc( kPacket_.m_cBuyType )
			//	<< END_LOG;
		}
	}

end_proc:
	return;
}
//}}

//{{ 2011. 04. 05  김민성	우편 및 거래 감시
IMPL_ON_FUNC( DBE_WRITE_LETTER_AND_TRADE_MOINTORING_REQ )
{
	KDBE_WRITE_LETTER_AND_TRADE_MOINTORING_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LPost_TradeMonitor_INS", L"%d, %d, N\'%s\', %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iRecvUnitUID
		% kPacket_.m_wstrRegDate
		% static_cast<int>(kPacket_.m_cType)
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		switch( kPacket.m_iOK) // SP반환값 -10 : 중복 데이터가 있다
		{
		case -10:
			{
				kPacket.m_iOK = (kPacket_.m_cType == 0) ? NetError::ERR_EXIST_LETTER_MONITORING : NetError::ERR_EXIST_TRADE_MONITORING;
			}break;
		case -11:
			{
				kPacket.m_iOK = (kPacket_.m_cType == 0) ? NetError::ERR_INSERT_FAILD_LETTER_MONITORING : NetError::ERR_INSERT_FAILD_TRADE_MONITORING;
			}break;
		default:
			{
				START_LOG( cout, L"Failed to record mail and trade monitoring log: ")
					<< BUILD_LOG( kPacket_.m_iRecvUnitUID )
					<< BUILD_LOGc( kPacket_.m_cType )
					<< END_LOG;
			}break;
		}
	}
	else
	{
		kPacket.m_iOK = static_cast<int>(kPacket_.m_cType);

		START_LOG( cout, L"Mail and trade monitoring log recorded successfully: ")
			<< BUILD_LOG( kPacket_.m_iRecvUnitUID )
			<< BUILD_LOGc( kPacket_.m_cType )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< END_LOG;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, DBE_WRITE_LETTER_AND_TRADE_MOINTORING_ACK, kPacket );
}
//}}

//{{ 2011. 04. 13	최육사	아이템 통계 DB기록

_IMPL_ON_FUNC( ELOG_STAT_ITEM_DB, KStatisticsData )
{
	CTime tCurTime = CTime::GetCurrentTime();
	const std::wstring wstrCurTime = (const wchar_t*)tCurTime.Format(_T("%Y-%m-%d %H:00:00"));
	const UidType iServerUID = KBaseServer::GetKObj()->GetUID();

	int iOK = NetError::ERR_ODBC_01;
	KStatisticsData::const_iterator mit;
	for( mit = kPacket_.begin(); mit != kPacket_.end(); ++mit )
	{
		const std::vector< int >& vecIntKey = mit->first.m_vecIntKey;
		if( vecIntKey.empty() )
		{
			START_LOG( cerr, L"The statistics key is empty." )
				<< END_LOG;
			continue;
		}

		const int iItemID = mit->first.m_vecIntKey[0];
		__int64 aiCount[KStatistics::SI_ITEM_MAX];
		std::vector< __int64 >::const_iterator vit = mit->second.begin();
		for( int iIndex = 0; iIndex < KStatistics::SI_ITEM_MAX; ++iIndex )
		{
			if( vit == mit->second.end() )
			{
				START_LOG( clog2, L"The count data is empty." )
					<< BUILD_LOG( iIndex )
					<< END_LOG;

				aiCount[iIndex] = 0;
			}
			else
			{
				aiCount[iIndex] = *vit;
				vit++;
			}
		}

		DO_QUERY( L"exec dbo.P_LItemLog_INS",
			L"%d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
			L"%d, %d, N\'%s\', %d, %d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iItemID										// @iItemID  INT
			% aiCount[KStatistics::SI_ITEM_TRADE]			// @iTrade   INT
			% aiCount[KStatistics::SI_ITEM_PERSONAL_SHOP]	// @iPShop   INT
			% aiCount[KStatistics::SI_ITEM_TRADE_BOARD]		// @iTBoard  INT
			% aiCount[KStatistics::SI_ITEM_BUY]				// @iBuy   INT
			% aiCount[KStatistics::SI_ITEM_SELL]			// @iSell   INT
			% aiCount[KStatistics::SI_ITEM_USE]				// @iUse   INT
			% aiCount[KStatistics::SI_ITEM_MAKE]			// @iMake   INT
			% aiCount[KStatistics::SI_ITEM_FAILED]			// @iFailed  INT
			% aiCount[KStatistics::SI_ITEM_DROP]			// @iDrop   INT
			% aiCount[KStatistics::SI_ITEM_QUEST]			// @iQuest   INT
			% aiCount[KStatistics::SI_ITEM_EQUIP]			// @iEquip   INT
			% aiCount[KStatistics::SI_ITEM_RANDOM]			// @iRandom  INT
			% aiCount[KStatistics::SI_ITEM_RESOLVE]			// @iResolve  INT
			% aiCount[KStatistics::SI_ITEM_R_RESULT]		// @iR_Result  INT
			% aiCount[KStatistics::SI_ITEM_MALL]			// @iMall   INT
			% aiCount[KStatistics::SI_ITEM_POST]			// @iPost   INT
			% aiCount[KStatistics::SI_ITEM_IDENTIFY]		// @iAttrib  INT
			% aiCount[KStatistics::SI_ITEM_EXCHANGE_S]		// @iExchange_S INT
			% aiCount[KStatistics::SI_ITEM_EXCHANGE_R]		// @iExchange_R INT
			% aiCount[KStatistics::SI_ITEM_DISCARD]			// @iDiscard  INT
			//{{ 2010. 9. 14	최육사	펫 시스템
			//#ifdef SERV_PET_SYSTEM
			% aiCount[KStatistics::SI_ITEM_PET_FEED]		// @iPetFeed  INT
			//#endif SERV_PET_SYSTEM
			//}}
			% wstrCurTime									// @sdtRegDate  SMALLDATETIME
				% iServerUID									// @iServerUID  INT
				//{{ 2011. 05. 02	최육사	대리상인
				//#ifdef SERV_PSHOP_AGENCY
				% aiCount[KStatistics::SI_ITEM_REG_PSHOP]
				% aiCount[KStatistics::SI_ITEM_BUY_PSHOP_AGENCY]
				% aiCount[KStatistics::SI_ITEM_PICK_UP_PSHOP]
				//#endif SERV_PSHOP_AGENCY
				//}}
				);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		START_LOG( clog2, L"Item statistics [check]" )
			<< BUILD_LOG( iItemID )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_TRADE] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_PERSONAL_SHOP] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_TRADE_BOARD] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_BUY] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_SELL] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_USE] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_MAKE] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_FAILED] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_DROP] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_QUEST] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_EQUIP] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_RANDOM] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_RESOLVE] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_R_RESULT] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_MALL] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_POST] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_IDENTIFY] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_EXCHANGE_S] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_EXCHANGE_R] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_DISCARD] )
			//{{ 2010. 9. 14	최육사	펫 시스템
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_PET_FEED] )
			//}}
			//{{ 2011. 05. 02	최육사	대리상인
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_REG_PSHOP] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_BUY_PSHOP_AGENCY] )
			<< BUILD_LOG( aiCount[KStatistics::SI_ITEM_PICK_UP_PSHOP] )
			//}}
			<< BUILD_LOG( wstrCurTime )
			<< BUILD_LOG( iServerUID )
			<< BUILD_LOG( iOK );
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.size() )
		<< END_LOG;
}

//}}


//{{ 2011. 05. 02  김민성	2차 보안 시스템
IMPL_ON_FUNC( DBE_FIRST_WRITE_SECURITY_TYPE_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LSecurityTypeLog_INS", L"%d, %d, %d, %d, N\'%s\'",
				% KBaseServer::GetKObj()->GetServerGroupID()
				% kPacket_.m_iUserUID
				% kPacket_.m_iSecurityType
				% kPacket_.m_iAge
				% kPacket_.m_wstrCurrTime);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to call the stored procedure to log the secondary security type!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iSecurityType )
			<< BUILD_LOG( kPacket_.m_iAge )
			<< BUILD_LOG( kPacket_.m_wstrCurrTime )
			<< END_LOG;
	}
}
//}}

_IMPL_ON_FUNC( ELOG_UPDATE_SHARE_ITEM_NOT, std::vector< KTradeShareItemInfo > )
{
	for( const KTradeShareItemInfo& kTradeShareItemInfo : kPacket_ )
	{
		int iOK = NetError::ERR_ODBC_01;
		DO_QUERY( L"exec dbo.P_LTradeBank_INT", L"%d, %d, %d, %d, %d, %d, %d",
			% GetKGameServer()->GetServerGroupID()
			% kTradeShareItemInfo.m_iFromUnitUID
			% kTradeShareItemInfo.m_iToUnitUID
			% kTradeShareItemInfo.m_iItemUID
			% kTradeShareItemInfo.m_iNewItemUID
			% kTradeShareItemInfo.m_iItemID
			% static_cast<int>(kTradeShareItemInfo.m_ucSealCnt)
			);
		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			/*
			START_LOG( cerr, L"은행 공유 : 로그 입력 실패!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( GetKGameServer()->GetServerGroupID() )
				<< BUILD_LOG( kTradeShareItemInfo.m_iFromUnitUID )
				<< BUILD_LOG( kTradeShareItemInfo.m_iToUnitUID )
				<< BUILD_LOG( kTradeShareItemInfo.m_iItemUID )
				<< BUILD_LOG( kTradeShareItemInfo.m_iNewItemUID )
				<< BUILD_LOG( kTradeShareItemInfo.m_iItemID )
				<< BUILD_LOG( kTradeShareItemInfo.m_ucSealCnt )
				<< END_LOG;
				*/
		}
	}

	goto end_proc;

end_proc:
	return;
}

IMPL_ON_FUNC( ELOG_ITEM_TRADE_ERROR_NOT )
{
	int iOK = NetError::NET_OK;

	DO_QUERY( L"exec dbo.P_LItemTradeError_INT", L"%d, %d, %s",
		% kPacket_.m_iUnitUID
		% kPacket_.m_iType
		% kPacket_.m_wsrtRegDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Shared bank: failed to write error log!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iType )
			<< END_LOG;
	}

	goto end_proc;

end_proc:
	return;
}


//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
IMPL_ON_FUNC( DBE_FIRST_WRITE_COME_BACK_INFO_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LComebackUserLog_INS", L"%d, %d, %d, %d, %d, %d, %d, %d, N\'%s\'",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% kPacket_.m_iUserUID
			% kPacket_.m_iAge
			% kPacket_.m_iUnitUID
			% kPacket_.m_iUnitClass
			% kPacket_.m_iUnitLevel
			% kPacket_.m_bTodayFirst
			% kPacket_.m_iRewardLevel
			% kPacket_.m_wsrtRegDate  );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to insert returning-user information into the database!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iAge )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iUnitClass )
			<< BUILD_LOG( kPacket_.m_iUnitLevel )
			<< BUILD_LOG( kPacket_.m_bTodayFirst )
			<< BUILD_LOG( kPacket_.m_iRewardLevel )
			<< BUILD_LOG( kPacket_.m_wsrtRegDate )
			<< END_LOG;
	}
}
//}}

//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
//}

//{{  2011.11.08     김민성    강화 로그
IMPL_ON_FUNC( ELOG_HIGH_ENCHANT_ITEM_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LEnchantLog_INS", L"%d, %d, %d, %d, %d, %d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemUID
		% kPacket_.m_iItemID
		% kPacket_.m_bSupportMaterial
		% kPacket_.m_iBeforeLevel
		% kPacket_.m_iAfterLevel
		% kPacket_.m_iEnchantResult
		% kPacket_.m_wstrTime
		)

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

end_proc:
		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to record high-level item enhancement log." )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( kPacket_.m_iItemUID )
				<< BUILD_LOG( kPacket_.m_iItemID )
				<< BUILD_LOGc( kPacket_.m_iBeforeLevel )
				<< BUILD_LOGc( kPacket_.m_iAfterLevel )
				<< BUILD_LOG( kPacket_.m_bSupportMaterial )
				<< BUILD_LOGc( kPacket_.m_iEnchantResult )
				<< END_LOG;
		}
}
//}}

//{{  2011.11.24     김민성    캐릭터 접속 로그
//}}

//{{ 2011. 12. 02  김민성	AP 구매 로그 수치
IMPL_ON_FUNC( DBE_BUY_AP_ITEM_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LApointLog_INS", L"%d, %d, %d, %d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iItemID
		% kPacket_.m_iQuantity
		% kPacket_.m_iBeforAP
		% kPacket_.m_iAfterAP
		% kPacket_.m_wstrBuyDate
		)

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

end_proc:
		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to log the AP item purchase!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( kPacket_.m_iItemID )
				<< BUILD_LOG( kPacket_.m_iQuantity )
				<< BUILD_LOG( kPacket_.m_iBeforAP )
				<< BUILD_LOG( kPacket_.m_iAfterAP )
				<< BUILD_LOG( kPacket_.m_wstrBuyDate )
				<< END_LOG;
		}
}
//}}

//{{ 2011. 12. 14	최육사	패킷 처리 지연 시간 체크
IMPL_ON_FUNC( DBE_UPDATE_PAKCET_MORNITORING_LOG_NOT )
{
	CTime tCurTime = CTime::GetCurrentTime();
	const std::wstring wstrCurTime = (std::wstring)tCurTime.Format( _T("%Y-%m-%d %H:%M:%S") );
	const UidType iServerUID = KBaseServer::GetKObj()->GetUID();

	for( const KPacketProcessingLatencyInfo& kInfo : kPacket_.m_vecGSUserPacketLatencyInfo )
	{
		int iOK = NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_LServer_GSUser_Packet_Latency_INS", L"%d, %d, N\'%s\', %d, N\'%s\', N\'%s\', %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iServerUID
			% KEvent::GetIDStr( kInfo.m_usEventID )
			% kInfo.m_dwMaxLatency
			% kInfo.m_wstrMaxLatencyTime
			% wstrCurTime
			% kInfo.m_iUnitUID
			)

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to log the AP item purchase!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( iServerUID )
				<< BUILD_LOG( kInfo.m_usEventID )
				<< BUILD_LOG( kInfo.m_dwMaxLatency )
				<< BUILD_LOG( kInfo.m_wstrMaxLatencyTime )
				<< BUILD_LOG( wstrCurTime )
				<< BUILD_LOG( kInfo.m_iUnitUID )
				<< END_LOG;
		}
	}

	START_LOG( cout, L"--------------------- Login Proxy Packet count ---------------------" );

	for( const KPacketProcessingCountInfo& kInfo : kPacket_.m_vecLoginProxyPacketCountInfo )
	{
		START_LOG( cout, KEvent::GetIDStr( kInfo.m_usEventID ) << L" : " << kInfo.m_dwCount );

		//int iOK = NetError::ERR_ODBC_01;

		// 주석 해제 하면 P_LServer_CnProxy_Packet_Count_INS SP 사용해야함
		//DO_QUERY( L"exec dbo.P_LServer_CnProxy_Packet_Count_INT", L"%d, N\'%s\', %d, N\'%s\'",
		//	% iServerUID
		//	% KEvent::GetIDStr( kInfo.m_usEventID )
		//	% kInfo.m_dwCount
		//	% wstrCurTime
		//	)

		//if( m_kODBC.BeginFetch() )
		//{
		//	FETCH_DATA( iOK );
		//	m_kODBC.EndFetch();
		//}

		//if( iOK != NetError::NET_OK )
		//{
		//	START_LOG( cerr, L"AP 아이템 구매 로그 기록 실패!" )
		//		<< BUILD_LOG( iOK )
		//		<< BUILD_LOG( iServerUID )
		//		<< BUILD_LOG( kInfo.m_usEventID )
		//		<< BUILD_LOG( kInfo.m_dwCount )
		//		<< BUILD_LOG( wstrCurTime )
		//		<< END_LOG;
		//}
	}

	for( const KPacketProcessingCountInfo& kInfo : kPacket_.m_vecCnProxyPacketCountInfo )
	{
		int iOK = NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_LServer_CnProxy_Packet_Count_INS", L"%d, %d, N\'%s\', %d, N\'%s\'",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iServerUID
			% KEvent::GetIDStr( kInfo.m_usEventID )
			% kInfo.m_dwCount
			% wstrCurTime
			)

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to log the AP item purchase!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( iServerUID )
				<< BUILD_LOG( kInfo.m_usEventID )
				<< BUILD_LOG( kInfo.m_dwCount )
				<< BUILD_LOG( wstrCurTime )
				<< END_LOG;
		}
	}

	START_LOG( cout, L"--------------------- Global Proxy Packet count ---------------------" );

	for( const KPacketProcessingCountInfo& kInfo : kPacket_.m_vecGlobalProxyPacketCountInfo )
	{
		START_LOG( cout, KEvent::GetIDStr( kInfo.m_usEventID ) << L" : " << kInfo.m_dwCount );

		//int iOK = NetError::ERR_ODBC_01;

		// 주석 해제 하면 P_LServer_CnProxy_Packet_Count_INS SP 사용해야함
		//DO_QUERY( L"exec dbo.P_LServer_CnProxy_Packet_Count_INT", L"%d, N\'%s\', %d, N\'%s\'",
		//	% iServerUID
		//	% KEvent::GetIDStr( kInfo.m_usEventID )
		//	% kInfo.m_dwCount
		//	% wstrCurTime
		//	)

		//if( m_kODBC.BeginFetch() )
		//{
		//	FETCH_DATA( iOK );
		//	m_kODBC.EndFetch();
		//}

		//if( iOK != NetError::NET_OK )
		//{
		//	START_LOG( cerr, L"AP 아이템 구매 로그 기록 실패!" )
		//		<< BUILD_LOG( iOK )
		//		<< BUILD_LOG( iServerUID )
		//		<< BUILD_LOG( kInfo.m_usEventID )
		//		<< BUILD_LOG( kInfo.m_dwCount )
		//		<< BUILD_LOG( wstrCurTime )
		//		<< END_LOG;
		//}
	}

	return;

end_proc:
	START_LOG( cerr, L"Failed to write packet processing information to the log database!" )
		<< BUILD_LOG( kPacket_.m_vecCnProxyPacketCountInfo.size() )
		<< BUILD_LOG( kPacket_.m_vecGSUserPacketLatencyInfo.size() )
		<< END_LOG;
}
//}}

//{{ 2012. 01. 27  김민성	해킹 프로세스 검사 기능
//}}

//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
//}}

//{{ 2012. 03. 29	김민성		x2.exe 모듈 유무 검사
//}}

//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
//}}

//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
_IMPL_ON_FUNC( DBE_BAD_ATTITUDE_USER_LOG_NOT, KERM_BAD_ATTITUDE_USER_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_00;

	DO_QUERY( L"exec dbo.P_LExitLog_INS", L"%d, %d, %d, %d, %d, %d, %d, %d, %d, N\'%s\', %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_iDungeonIDAndDif
		% kPacket_.m_sStartMember
		% kPacket_.m_sNowMember
		% kPacket_.m_sVoteCount
		% kPacket_.m_sExitStage
		% kPacket_.m_sExitSubStage
		% kPacket_.m_sBadAttitudePoint
		% kPacket_.m_wstrRegDate.c_str()
		% kPacket_.m_bExitResult )

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to write the dungeon kick log." )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( kPacket_.m_iDungeonIDAndDif )
				<< BUILD_LOG( kPacket_.m_sStartMember )
				<< BUILD_LOG( kPacket_.m_sNowMember )
				<< BUILD_LOG( kPacket_.m_sVoteCount )
				<< BUILD_LOG( kPacket_.m_sExitStage )
				<< BUILD_LOG( kPacket_.m_sExitSubStage )
				<< BUILD_LOG( kPacket_.m_sBadAttitudePoint )
				<< BUILD_LOG( kPacket_.m_wstrRegDate )
				<< BUILD_LOG( kPacket_.m_bExitResult )
				<< END_LOG;
		}
	return;

end_proc:
	START_LOG( cerr, L"Failed to write the dungeon kick log." )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< BUILD_LOG( kPacket_.m_iDungeonIDAndDif )
		<< BUILD_LOG( kPacket_.m_sStartMember )
		<< BUILD_LOG( kPacket_.m_sNowMember )
		<< BUILD_LOG( kPacket_.m_sVoteCount )
		<< BUILD_LOG( kPacket_.m_sExitStage )
		<< BUILD_LOG( kPacket_.m_sExitSubStage )
		<< BUILD_LOG( kPacket_.m_sBadAttitudePoint )
		<< BUILD_LOG( kPacket_.m_wstrRegDate )
		<< BUILD_LOG( kPacket_.m_bExitResult )
		<< END_LOG;
};
//}

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
		START_LOG( cerr, L"Failed to write SimLayer tick performance data to the log!" )
			<< BUILD_LOG( kPacket_.m_iServerUID )
			<< BUILD_LOG( kPacket_.m_iServerType )
			<< BUILD_LOG( kPacket_.m_dwMaxTickCount )
			<< BUILD_LOG( kPacket_.m_wstrMaxTickRegDate )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< END_LOG;
	}
}
//}}

//{{ 허상형 : [2010/8/31/] //	부활석 통계 분리 작업
//}} 허상형 : [2010/8/31/] //	부활석 통계 분리 작업

//{{ 2011.05.04   임규수 아바타 합성 시스템
//}}

_IMPL_ON_FUNC( DBE_CATCH_HACKUSER_INTO_NOT, KEGS_CATCH_HACKUSER_INFO_NOT )
{
	int iOK = NetError::NET_OK;

	DO_QUERY( L"exec dbo.lup_insert_catch_hackuser_info", L"%d, %d, %d",
		% kPacket_.m_iUserUID
		% kPacket_.m_iUnitUID
		% kPacket_.m_iCrashType
		);

	if( m_kODBC.BeginFetch() )
	{
		m_kODBC.EndFetch();
	}
end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< END_LOG;
}

//{{ 2013. 02. 19   교환 로그 추가 - 김민성
IMPL_ON_FUNC( DBE_ITEM_EXCHANGE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	std::map<int, int>::iterator mit = kPacket_.m_mapResultItem.begin();
	for( ; mit != kPacket_.m_mapResultItem.end() ; ++mit )
	{
		DO_QUERY( L"exec dbo.P_LExchangeLog_INS", L"%d, %d, %d, %d, %d, %d, %d, N\'%s\'",
			% kPacket_.m_iUnitUID
			% kPacket_.m_iUserUID
			% kPacket_.m_iSourceItemID
			% static_cast<short>(kPacket_.m_iSourceItemQuantity)
			% mit->first
			% static_cast<short>(mit->second)
			% (int)kPacket_.m_cExchangeType
			% kPacket_.m_wstrRegDate
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"Failed to record exchange result log." )
				<< BUILD_LOG( kPacket_.m_iUnitUID )
				<< BUILD_LOG( kPacket_.m_iUserUID )
				<< BUILD_LOG( kPacket_.m_iSourceItemID )
				<< BUILD_LOG( kPacket_.m_iSourceItemQuantity )
				<< BUILD_LOG( mit->first )
				<< BUILD_LOG( mit->second )
				<< BUILD_LOGc( kPacket_.m_cExchangeType )
				<< BUILD_LOG( kPacket_.m_wstrRegDate )
				<< END_LOG;
		}
end_proc:;
	}
}
//}

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
			START_LOG( cerr, L"Failed to write log statistics!" )
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




//{{ 2013. 04. 01	 인연 시스템 - 김민성
IMPL_ON_FUNC( DBE_WRITE_RELATIONSHIP_INFO_LOG_REQ )
{
	KDBE_WRITE_RELATIONSHIP_INFO_LOG_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_LCoupleSystemLog_INT", L"%d, %d, %d, %d, %d, N\'%s\'",
		% kPacket_.m_iUserUID
		% kPacket_.m_iUnitUID
		% static_cast<int>(kPacket_.m_ucLevel)
		% static_cast<int>(kPacket_.m_cUnitClass)
		% static_cast<int>(kPacket_.m_cCoupleType)
		% kPacket_.m_wstrRegDate	);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}
end_proc:
	SendToUser( FIRST_SENDER_UID, DBE_WRITE_RELATIONSHIP_INFO_LOG_ACK, kPacket );
}
//}}



IMPL_ON_FUNC( ELOG_KOM_FILE_CHECK_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY( L"exec dbo.P_LKomFileLog_INT", L"%d, \'%s\'",
		% kPacket_.m_iUserUID
		% kPacket_.m_wstrInvaildKomName
		);
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< END_LOG;
}

IMPL_ON_FUNC( ELOG_EXPAND_BANK_INVENTORY_NOT )
{
    /*
    DB
    [Statistics] , [Statistics02]
    SP 이름
    [dbo].[lup_insert_inventory_ED_log]
    전달 인자
    @iUnitUID         BIGINT
    @iUserUID        INT
    @iBeforeED       INT
    @iUseED          INT
    @iInventory       TINYINT
    @iNumSlot        TINYINT
    @iLevel            TINYINT
    */
    int iOK = NetError::ERR_ODBC_01;

    DO_QUERY( L"exec dbo.lup_insert_inventory_ED_log", L"%d, %d, %d, %d, %d, %d, %d",
        % kPacket_.m_iUnitUID
        % kPacket_.m_iUserUID
        % kPacket_.m_iBeforeED
        % kPacket_.m_iUseED
        % kPacket_.m_iCategory
        % kPacket_.m_iNumSlot
        % kPacket_.m_iUnitLevel
        );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();
    }

end_proc:

    LOG_SUCCESS( iOK == NetError::NET_OK )
        << BUILD_LOG( iOK )
        << END_LOG;
}

IMPL_ON_FUNC( ELOG_EXPAND_SKILL_PAGE_NOT )
{
    /*
	[dbo].[P_LSkillPageExpandLog_INT]전달 인자

	전달 인자
	@iUnitUID         BIGINT
	@strNickName   NVARCHAR(50)
	@iLevel            TINYINT
	@iUnitClass       TINYINT
	@iBeforePage    TINYINT           -- 확장하기 이전 보유한 스킬 페이지 수
	@ibCash           TINYINT           -- 0:ED, 1:Cash
    */
    int iOK = NetError::ERR_ODBC_01;

    DO_QUERY( L"exec dbo.P_LSkillPageExpandLog_INT", L"%d, N\'%s\', %d, %d, %d, %d",
        % kPacket_.m_iUnitUID
        % kPacket_.m_wstrSenderNickName
        % kPacket_.m_ucLevel
        % kPacket_.m_ucUnitClass
        % kPacket_.m_ucBeforeTheNumberOfSkillPagesAvailable
        % kPacket_.m_ucPaymentType
        );

    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();
    }

end_proc:

    LOG_SUCCESS( iOK == NetError::NET_OK )
        << BUILD_LOG( iOK )
        << END_LOG;
}


IMPL_ON_FUNC( DBE_CHANGE_PET_ID_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LChangePetLog_INS", L"%d, %d, %d, %d",
		% kPacket_.m_iUnitUID
		% kPacket_.m_iPetUID
		% kPacket_.m_iBeforePetID
		% kPacket_.m_iAfterPetID
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record Halloween fairy pet log." )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iPetUID )
			<< BUILD_LOG( kPacket_.m_iBeforePetID )
			<< BUILD_LOG( kPacket_.m_iAfterPetID )
			<< END_LOG;
	}
}


IMPL_ON_FUNC( DBE_UNLIMITED_SECOND_CHANGE_JOB_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LUnlimitUnitClassChangeLog_INS", L"%d, %d, %d",
		% kPacket_.m_iUnitUID
		% kPacket_.m_iOldUnitClass
		% kPacket_.m_iNewUnitClass
		); // 단순 UnitClass 만 변경

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Failed to record use of the unlimited class-change ticket!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_iOldUnitClass )
			<< BUILD_LOG( kPacket_.m_iNewUnitClass )
			<< END_LOG;
	}
}
