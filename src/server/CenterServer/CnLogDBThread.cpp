#include <winsock2.h>
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "CnLogDBThread.h"
#include "CnServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "PvPRoom.h"
#include "DungeonRoom.h"
#include "CnRoomManager.h"

#define CLASS_TYPE KCnLogDBThread
ImplPfID( KCnLogDBThread, PI_CN_LOG_DB );

IMPL_PROFILER_DUMP( KCnLogDBThread )
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

KCnLogDBThread::~KCnLogDBThread(void)
{
}

void KCnLogDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_REQ, KNetAddress );
	   //{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
	   _CASE( DBE_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ );
	   //}}
       //_CASE( DBE_UPDATE_SERVER_INFO_REQ, KServerInfo );
        CASE_NOPARAM( DBE_SERVER_OFF_NOT );
        CASE( ELOG_CHAT_LOG_NOT );
		CASE( ELOG_INSERT_TRADE_LOG_NOT );
		CASE( ELOG_INSERT_PERSONAL_SHOP_LOG_NOT );
	   _CASE( ELOG_STAT_RES_STONE, KStatisticsData );
	   CASE_NOPARAM( DBE_EVENT_UPDATE_REQ );

		//{{ 2008. 3. 4  최육사  어뷰저 로그
		CASE( ELOG_INSERT_ABUSER_LOG_NOT );
		//}}
		//{{ 2008. 10. 31  최육사	어뷰저 리스트
		CASE( DBE_ABUSER_LIST_REQ );
		//}}
		//{{ 2009. 5. 23  최육사	시간드롭
		CASE_NOPARAM( DBE_GET_TIME_DROP_INFO_REQ );
		CASE( DBE_CHECK_TIME_DROP_RESULT_REQ );
		//}}
		//{{ 2009. 7. 27  최육사	헤니르 클리어 로그
		CASE( ELOG_HENIR_DUNGEON_CLEAR_NOT );
		//}}
		//{{ 2010. 11. 8	최육사	던전 로그 DB
	   _CASE( DBE_DB_LOG_DUNGEON_NOT, KE_LOCAL_LOG_DUNGEON_NOT );
		//}}
	   //{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현
	   CASE_NOPARAM( DBE_GET_DUNGEON_EVENT_INFO_REQ );
	   CASE( DBE_UPDATE_EVENT_MONSTER_KILL_INFO_NOT );
	   //}}
	   //{{ 2010. 12. 8	최육사	적정 레벨 장비 통계
	   //}}
	   //{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	   CASE( DBE_REG_DUNGEON_CLEAR_TIME_HACK_USER_NOT );
	   //}}
	   //{{ 2011. 03. 17	김민성	던전 플레이 로그 요청사항
	   //}}
	   //{{ 2011. 06. 13	최육사	스테이지 로그
	   //}}
	   //{{ 2011. 07. 27	최육사	대전 개편
	   CASE( DBE_PVP_MATCH_RESULT_LOG_NOT );
	   //}}
	   //{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
	   CASE( ELOG_COLLECTION_OF_RELAY_AND_P2P_INFO_NOT );
	   //}}
	   //{{ 2012. 09. 05	박세훈	랜선렉 로그 추가
	   //}}
	   //{{ 2012. 10. 18	최육사	배틀필드 시스템
	   CASE( ELOG_BATTLE_FIELD_LEAVE_LOG_NOT );
	   //}}
	   //{{ 2012. 12. 12	최육사	결과창 이탈 유저 로그
	   CASE( DBE_DUNGEON_RESULT_LEAVE_LOG_NOT );
	   //}}
	   //{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
	   CASE( DBE_TICK_PERFORMANCE_LOG_NOT );
	   //}}
	   //{{ 2013. 02. 21	최육사	로그 시스템 개선
	   CASE( DBE_LOG_STATISTICS_INFO_NOT );
	   //}}

	   CASE_NOPARAM( DBE_CHECK_EVENT_UPDATE_REQ );

	   //{{ 2013. 2. 26	박세훈	랜선렉 방지 코드2
	   CASE( DBE_UDP_CHECK_KICK_USER_NOT );
	   //}}
	   //{{ 2013. 04. 01	 인연 시스템 - 김민성
	   CASE( DBE_WRITE_RELATIONSHIP_INFO_LOG_REQ );
	   //}


       CASE( ELOG_HENIR_HACKING_LOG_NOT );

       _CASE( ELOG_GEN_FIELD_MIDDLE_BOSS_STAT_NOT, KFieldBossGenKillStat );
       _CASE( ELOG_KILL_FIELD_MIDDLE_BOSS_STAT_NOT, KFieldBossGenKillStat );

	   CASE( DBE_BOSS_FIELD_LOG_NOT );

    default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
    }

	//////////////////////////////////////////////////////////////////////////
	CALL_PROFILER_DUMP( SP_PROFILER_DUMP_TIME );
	//////////////////////////////////////////////////////////////////////////
}

void KCnLogDBThread::SendToServer( unsigned short usEventID )
{
    SendToServer( usEventID, char() );
}

void KCnLogDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
    SendToUser( nTo, usEventID, char() );
}

void KCnLogDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID )
{
    SendToGSUser( nGS, nTo, usEventID, char() );
}

IMPL_ON_FUNC( ELOG_CHAT_LOG_NOT )
{
    // injection 공격 가능성 제거.
    KODBC::RemoveInvalidMark( kPacket_.m_strMsg );

	DO_QUERY( L"exec dbo.P_ChatLog_INS", L"%d, %d, %d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_nRoomUID
		% kPacket_.m_nCharUID
		% kPacket_.m_nUserUID
		% (int)kPacket_.m_cChatType
		% kPacket_.m_strMsg );

end_proc:
    __noop; // doing nothing.
}

_IMPL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress )
{
    KDBE_CENTER_SERVER_ON_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;

	//{{ 2013. 01. 17	최육사	서버간 통신 사설IP 적용
	DO_QUERY( L"exec dbo.P_LServerList_By_PublicIP_SEL", L"%d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% KBaseServer::ESC_CENTER
		% kPacket_.m_wstrIP
		);
	//}}

    if( !m_kODBC.BeginFetch() )
    {
        START_LOG( cerr, L"센터 서버 인증 실패." )
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
		>> kPacket.m_kServerInfo.m_bOn
		>> kPacket.m_kServerInfo.m_cServerRollType );
	//}}

    m_kODBC.EndFetch();

	//{{ 2011. 07. 22	최육사	모든 쿼리 SP사용
	DO_QUERY( L"exec dbo.P_LCenterProxy_By_ServerUID_SEL", L"%d, %d",	% KBaseServer::GetKObj()->GetServerGroupID()
																		% kPacket.m_kServerInfo.m_iUID	);
	//}}
	if( !m_kODBC.BeginFetch() )
	{
		START_LOG( cerr, L"난 누군가의 센터다." )
			<< BUILD_LOG( kPacket.m_kServerInfo.m_iUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_VERIFY_13;
		goto end_proc;
	}

	FETCH_DATA( kPacket.m_iProxyID );
	m_kODBC.EndFetch();

	//{{ 2010. 11. 1	최육사	광장 리스트 개선
	//{{ 2011. 07. 22	최육사	모든 쿼리 SP사용
	DO_QUERY( L"exec dbo.P_LSquareTable_SEL", L"%d, N\'%s\'", % KBaseServer::GetKObj()->GetServerGroupID()
															  % kPacket_.m_wstrIP );
	//}}
	while( m_kODBC.Fetch() )
	{
		KSquareCreateInfo kSquareCreateInfo;

		FETCH_DATA( kSquareCreateInfo.m_iSquareType
			>> kSquareCreateInfo.m_wstrSquareName );

		kPacket.m_vecSquareList.push_back( kSquareCreateInfo );
	}
	//}}

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
	kPacket.m_iLocalServerGroupID = kPacket_.m_iServerGroupID;

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

//_IMPL_ON_FUNC( DBE_UPDATE_SERVER_INFO_REQ, KServerInfo )
//{
//    KServerList kPacket;
//    WriteServerInfoToDB( kPacket_, kPacket );
//
//    SendToServer( DBE_UPDATE_SERVER_INFO_ACK, kPacket );
//}

//void KCnLogDBThread::WriteServerInfoToDB( IN KServerInfo kPacket_, OUT KServerList& kPacket )
//{
//    // 동접을 DB에 적는다.
//    int iOK;
//    DO_QUERY( ( L"exec dbo.lup_update_concurrent_user %d, %d" ) % kPacket_.m_iUID % kPacket_.m_nCurrentUser );	// 주석 해제 시 P_LServerList_CCU_UPD 사용해야함
//    if( m_kODBC.BeginFetch() )
//    {
//        FETCH_DATA( iOK );
//        m_kODBC.EndFetch();
//    }
//
//    // DB로부터 서버 리스트 데이터를 받아온다
//    DO_QUERY( ( L"exec dbo.lup_get_server_list" ) ); // 주석해제시 P_LServerList_SEL 이거 사용해야함
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
                << BUILD_LOG( GetKCnServer()->GetUID() )
                << END_LOG;

            iOK = NetError::ERR_VERIFY_08;
        }
    }

end_proc:
    return;
}

IMPL_ON_FUNC( ELOG_INSERT_TRADE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LTrade_INS", L"%d, %d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iFromUserUID
		% kPacket_.m_iFromUnitUID
		% kPacket_.m_iToUserUID
		% kPacket_.m_iToUnitUID
		% kPacket_.m_iItemUID
		% kPacket_.m_iItemID
		% kPacket_.m_iQuantity );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iFromUserUID )
		<< BUILD_LOG( kPacket_.m_iFromUnitUID )
		<< BUILD_LOG( kPacket_.m_iToUserUID )
		<< BUILD_LOG( kPacket_.m_iToUnitUID )
		<< BUILD_LOG( kPacket_.m_iItemUID )
		<< BUILD_LOG( kPacket_.m_iItemID )
		<< BUILD_LOG( kPacket_.m_iQuantity )
		<< END_LOG;
	return;
}

//{{ 2008. 2. 26  최육사  개인상점 로그
IMPL_ON_FUNC( ELOG_INSERT_PERSONAL_SHOP_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_STrade_INS", L"%d, %d, %d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
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
		//{{ 2011. 05. 11	최육사	대리상인
		<< BUILD_LOGc( kPacket_.m_cPShopType )
		//}}
		<< END_LOG;
	return;
	//}}
}
//}}

//{{ 2007. 12. 25  최육사  부활석 통계
_IMPL_ON_FUNC( ELOG_STAT_RES_STONE, KStatisticsData )
{
    int iOK = NetError::ERR_ODBC_01;

    KStatisticsData::iterator mit;
    for( mit = kPacket_.begin(); mit != kPacket_.end(); ++mit )
    {
        if( mit->first.m_vecIntKey.empty() )
        {
            START_LOG( cerr, L"통계키값이 비었음." )
                << END_LOG;

            continue;
        }
		__int64 aiCount[KStatistics::eSIColResStone_Max];
        std::vector< __int64 >::iterator vit = mit->second.begin();
        for( int iIndex = 0; iIndex < KStatistics::eSIColResStone_Max; iIndex++ )
        {
            if( vit == mit->second.end() )
            {
                START_LOG( cwarn, L"카운트 데이터가 비었음." )
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

		DO_QUERY( L"exec dbo.P_StatsStoneCnt_SET", L"%d, %d, %d",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% aiCount[KStatistics::eSIColResStone_PlayerCount]
			% aiCount[KStatistics::eSIColResStone_UseCount]
			);

        if( m_kODBC.BeginFetch() )
        {
            FETCH_DATA( iOK );

            m_kODBC.EndFetch();
        }
    }

end_proc:
    LOG_SUCCESS( iOK == NetError::NET_OK )
        << BUILD_LOG( iOK )
        << BUILD_LOG( kPacket_.size() )
        << END_LOG;
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
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_CN_SERVER, NULL, DBE_EVENT_UPDATE_ACK, kPacket );
	GetKCnRoomManager()->QueueingEventToRoomManager( spEvent );
	//}}
}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}

//{{ 2008. 3. 4  최육사  어뷰저 로그
IMPL_ON_FUNC( ELOG_INSERT_ABUSER_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsAbusingLog_INS", L"%d, %d, %d, N\'%s\', %d, %d, %d, %d, N\'%s\', %d, %d, "
		L"%d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID				// @iUserUID		bigint
		% kPacket_.m_iUnitUID				// @iUnitUID		bigint
		% kPacket_.m_wstrIP					// @strIP			nvarchar(15)
		% kPacket_.m_iGameType				// @iVsType			tinyint
		% (int)kPacket_.m_fPlayTimeLimit	// @iVsSetTime		smallint
		% kPacket_.m_bPublic				// @bIsSecret		bit
		% kPacket_.m_iPlayWithMember		// @iPlayerCnt		tinyint
		% kPacket_.m_wstrEndGameTime		// @sdtVsEndTime	smalldatetime
		% kPacket_.m_iPlayTime				// @iPlayTime		int
		% kPacket_.m_iRewardVP				// @iGetVP			int
		% kPacket_.m_iRewardEXP				// @iGetExp			int
		% kPacket_.m_iKillCount				// @iKillCnt		tinyint
		% kPacket_.m_iDeathCount			// @iDeathCnt		tinyint
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUserUID )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< BUILD_LOG( kPacket_.m_wstrIP )
		<< BUILD_LOG( kPacket_.m_iGameType )
		<< BUILD_LOG( kPacket_.m_fPlayTimeLimit )
		<< BUILD_LOG( kPacket_.m_bPublic )
		<< BUILD_LOG( kPacket_.m_iPlayWithMember )
		<< BUILD_LOG( kPacket_.m_wstrEndGameTime )
		<< BUILD_LOG( kPacket_.m_iPlayTime )
		<< BUILD_LOG( kPacket_.m_iRewardVP )
		<< BUILD_LOG( kPacket_.m_iRewardEXP )
		<< BUILD_LOG( kPacket_.m_iKillCount )
		<< BUILD_LOG( kPacket_.m_iDeathCount )
		<< END_LOG;
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
			START_LOG( clog, L"Abuser List와는 관계없는 ReleaseTick입니다." )
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
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_CN_SERVER, NULL, DBE_ABUSER_LIST_ACK, kPacket );
	GetKCnRoomManager()->QueueingEventToRoomManager( spEvent );
	//}}
}
//}}
//}}

//{{ 2009. 5. 23  최육사	시간드롭
IMPL_ON_FUNC_NOPARAM( DBE_GET_TIME_DROP_INFO_REQ )
{
	KDBE_GET_TIME_DROP_INFO_ACK kPacket;

	// Release Tick 얻기
	DO_QUERY( L"exec dbo.P_DropEvent_SEL", L"%d", % KBaseServer::GetKObj()->GetServerGroupID() );

	while( m_kODBC.Fetch() )
	{
		KTimeDropInfo kInfo;
		bool bIsDrop = false;

		FETCH_DATA( kInfo.m_iTimeDropID
			>> kInfo.m_iDropItemID
			>> kInfo.m_wstrDropTime
			>> kInfo.m_wstrServerIP
			>> bIsDrop
			>> kInfo.m_bNoticeMsg );

		// 이미 드롭된 아이템은 주지 않는다
		if( bIsDrop == true )
			continue;

		kPacket.m_vecTimeDropInfo.push_back( kInfo );
	}

end_proc:
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_CN_SERVER, NULL, DBE_GET_TIME_DROP_INFO_ACK, kPacket );
	GetKCnRoomManager()->QueueingEventToRoomManager( spEvent );
}

IMPL_ON_FUNC( DBE_CHECK_TIME_DROP_RESULT_REQ )
{
	KDBE_CHECK_TIME_DROP_RESULT_ACK kPacket;
	kPacket.m_iOK			= NetError::ERR_ODBC_01;
	kPacket.m_iGSUID		= kPacket_.m_iGSUID;
	kPacket.m_iUserUID		= kPacket_.m_iUserUID;
	kPacket.m_iUnitUID		= kPacket_.m_iUnitUID;
	kPacket.m_wstrNickName	= kPacket_.m_wstrNickName;
	kPacket.m_iTimeDropID	= kPacket_.m_iTimeDropID;
	kPacket.m_iRewardItemID = kPacket_.m_iRewardItemID;
	kPacket.m_wstrRewardItemName = kPacket_.m_wstrRewardItemName;
	kPacket.m_iDropItemUID	= kPacket_.m_iDropItemUID;

	// 보상 받아도 되는 유저인지 검사
	DO_QUERY( L"exec dbo.P_DropEvent_UPD", L"%d, %d, %d", % KBaseServer::GetKObj()->GetServerGroupID() % kPacket_.m_iTimeDropID % kPacket_.m_iUserUID
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( kPacket.m_iOK );
		m_kODBC.EndFetch();
	}
	else
	{
		kPacket.m_iOK = NetError::ERR_TIME_DROP_EVENT_00;

		START_LOG( cerr, L"시간 드롭 보상 체크 쿼리 실패!" )
			<< BUILD_LOG( kPacket_.m_iTimeDropID )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< END_LOG;

		goto end_proc;
	}

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		// 이 유저는 보상을 받을수 있는 유저가 아니다!
		kPacket.m_iOK = NetError::ERR_TIME_DROP_EVENT_01;
	}

end_proc:
	if( kPacket.m_iOK == NetError::NET_OK )
	{
		// 성공했다면 보상 처리하러 가자!
		SendToServer( DBE_CHECK_TIME_DROP_RESULT_ACK, kPacket );
	}
	else
	{
		// 실패 했다면 롤백하러 가자!
		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_CN_SERVER, NULL, DBE_CHECK_TIME_DROP_RESULT_ACK, kPacket );
		GetKCnRoomManager()->QueueingEventToRoomManager( spEvent );
	}
}
//}}

//{{ 2009. 7. 27  최육사	헤니르 클리어 로그
IMPL_ON_FUNC( ELOG_HENIR_DUNGEON_CLEAR_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_StatsTimeSpaceTop100_INS", L"%d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID
		% kPacket_.m_iUnitUID
		% kPacket_.m_iPlayTime );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iUserUID )
		<< BUILD_LOG( kPacket_.m_iUnitUID )
		<< BUILD_LOG( kPacket_.m_iPlayTime )
		<< END_LOG;
}
//}}

//{{ 2010. 11. 8	최육사	던전 로그 DB
_IMPL_ON_FUNC( DBE_DB_LOG_DUNGEON_NOT, KE_LOCAL_LOG_DUNGEON_NOT )
{
	const std::wstring wstrIP = NetCommon::GetLocalIPW();

	int iOK = NetError::ERR_ODBC_01;

	int iAPoint = 0; // PVE에서 추가되지만 SP를 변경하지 않기 위해 무조건 남기도록 함.

	DO_QUERY( L"exec dbo.P_LDungeonLog_INS",
		L"%d, "
		L"N\'%s\', %d, %d, N\'%s\', %d, %d, %d, %d, %d, %d, "
		L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
		L"%d, %d, %d, %d, %d, %d, N\'%s\', %d, %d, %d, "
		L"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, "
		L"%d, %d, N\'%s\',%d, %d, %d, %d, %d, %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% wstrIP
		% kPacket_.m_iDungeonID
		% kPacket_.m_nUnitUID
		% kPacket_.m_wstrNickName
		% kPacket_.m_iDifficultyLevel
		% kPacket_.m_iIsChallenge
		% kPacket_.m_iUnitClass
		% kPacket_.m_iClear
		% kPacket_.m_iStartNumMember
		% kPacket_.m_iEndNumMember							// 10번
		% static_cast<int>(kPacket_.m_ucLevel)
		% kPacket_.m_iPlayTime
		% kPacket_.m_iEXP
		% kPacket_.m_iED
		% static_cast<int>(kPacket_.m_cComboRank)
		% kPacket_.m_iComboScore
		% static_cast<int>(kPacket_.m_cTechnicalRank)
		% kPacket_.m_iTechnicalScore
		% static_cast<int>(kPacket_.m_cTimeRank)
		% static_cast<int>(kPacket_.m_cDamagedRank)			// 20번
		% kPacket_.m_iDamageNum
		% static_cast<int>(kPacket_.m_cTotalRank)
		% kPacket_.m_iRessurectionStoneCount
		% kPacket_.m_iPassedStageCount
		% kPacket_.m_iPassedSubStageCount
		% kPacket_.m_bIsWithPet
		% kPacket_.m_wstrRegDate
		% kPacket_.m_sNpcDieCount
		% kPacket_.m_iGivenDamage
		% kPacket_.m_iAttackDamage							// 30번
		% kPacket_.m_sUsingSkillKind
		% kPacket_.m_iTotalEXP
		% kPacket_.m_iPetID
		% static_cast<int>(kPacket_.m_cPetEvoStep)
		% kPacket_.m_iDashCount
		% kPacket_.m_iEndHP
		% kPacket_.m_iStartContinue
		% kPacket_.m_iContinueUseStage
		% kPacket_.m_iContinueUseSub
		% kPacket_.m_iFrame									// 40번
		% kPacket_.m_iBaseHP
		% kPacket_.m_iRecovHP
		% kPacket_.m_wstrChannelIP
		% kPacket_.m_iUserUID
		% static_cast<int>(kPacket_.m_cAutoPartyPlay)
		% kPacket_.m_iAutoPartyWaitTime
		% kPacket_.m_iLeaveReason
		% static_cast<int>(kPacket_.m_cRoomState)
		% kPacket_.m_bSkillSlotType
		% iAPoint
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( wstrIP )
		<< BUILD_LOG( kPacket_.m_iDungeonID )
		<< BUILD_LOG( kPacket_.m_nUnitUID )
		<< BUILD_LOG( kPacket_.m_wstrNickName )
		<< BUILD_LOG( kPacket_.m_iDifficultyLevel )
		<< BUILD_LOG( kPacket_.m_iIsChallenge )
		<< BUILD_LOG( kPacket_.m_iUnitClass )
		<< BUILD_LOG( kPacket_.m_iClear )
		<< BUILD_LOG( kPacket_.m_iStartNumMember )
		<< BUILD_LOG( kPacket_.m_iEndNumMember )
		<< BUILD_LOGc( kPacket_.m_ucLevel )
		<< BUILD_LOG( kPacket_.m_iPlayTime )
		<< BUILD_LOG( kPacket_.m_iEXP )
		<< BUILD_LOG( kPacket_.m_iED )
		<< BUILD_LOGc( kPacket_.m_cComboRank )
		<< BUILD_LOG( kPacket_.m_iComboScore )
		<< BUILD_LOGc( kPacket_.m_cTechnicalRank )
		<< BUILD_LOG( kPacket_.m_iTechnicalScore )
		<< BUILD_LOGc( kPacket_.m_cTimeRank )
		<< BUILD_LOGc( kPacket_.m_cDamagedRank )
		<< BUILD_LOG( kPacket_.m_iDamageNum )
		<< BUILD_LOGc( kPacket_.m_cTotalRank )
		<< BUILD_LOG( kPacket_.m_iRessurectionStoneCount )
		<< BUILD_LOG( kPacket_.m_iPassedStageCount )
		<< BUILD_LOG( kPacket_.m_iPassedSubStageCount )
		<< BUILD_LOG( kPacket_.m_bIsWithPet )
		<< BUILD_LOG( kPacket_.m_wstrRegDate )
		<< BUILD_LOG( kPacket_.m_sNpcDieCount )
		<< BUILD_LOG( kPacket_.m_iGivenDamage )
		<< BUILD_LOG( kPacket_.m_iAttackDamage )
		<< BUILD_LOG( kPacket_.m_sUsingSkillKind )
		<< BUILD_LOG( kPacket_.m_iTotalEXP )
		<< BUILD_LOGc( kPacket_.m_iPetID )
		<< BUILD_LOGc( kPacket_.m_cPetEvoStep )
		<< BUILD_LOG( kPacket_.m_iDashCount )
		<< BUILD_LOG( kPacket_.m_iEndHP )
		<< BUILD_LOG( kPacket_.m_iStartContinue )
		<< BUILD_LOG( kPacket_.m_iContinueUseStage )
		<< BUILD_LOG( kPacket_.m_iContinueUseSub )
		<< BUILD_LOG( kPacket_.m_iFrame )
		<< BUILD_LOG( kPacket_.m_iBaseHP )
		<< BUILD_LOG( kPacket_.m_iRecovHP )
		<< BUILD_LOG( kPacket_.m_wstrChannelIP )
		//{{ 2012. 11. 6	최육사	배틀필드 시스템
		<< BUILD_LOGc( kPacket_.m_cAutoPartyPlay )
		//}}
		//{{ 2012. 12. 12	최육사	배틀필드 시스템
		<< BUILD_LOG( kPacket_.m_iLeaveReason )
		<< BUILD_LOGc( kPacket_.m_cRoomState )
		//}}
		//{{ 2012. 12. 12  스킬 슬롯 사용 타입 로그 - 김민성
		<< BUILD_LOG( kPacket_.m_bSkillSlotType )
		<< BUILD_LOG( iAPoint )
		//}}
		<< END_LOG;
}
//}}

//{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현

IMPL_ON_FUNC_NOPARAM( DBE_GET_DUNGEON_EVENT_INFO_REQ )
{
	KDBE_GET_DUNGEON_EVENT_INFO_ACK kPacket;

	// 특정 시각 몬스터 출현 이벤트 정보 얻기
	DO_QUERY( L"exec dbo.P_LExpo_Event_SEL", L"%d, %d", % KBaseServer::GetKObj()->GetServerGroupID()
														% KBaseServer::GetKObj()->GetUID() );

	while( m_kODBC.Fetch() )
	{
		KDungeonDropEventNpcInfo kInfo;
		UidType iUnitUID = 0;
		int iServerUID = 0;
		std::wstring wstrDropDate;
		std::wstring wstrDieDate;

		FETCH_DATA( kInfo.m_iEventUID
			>> kInfo.m_iNpcID
			>> wstrDropDate
			);

		CTime tDropDate;
		LIF( KncUtil::ConvertStringToCTime( wstrDropDate, tDropDate ) );
		kInfo.m_tDropDate = tDropDate.GetTime();

		kPacket.m_vecEventInfo.push_back( kInfo );
	}

end_proc:
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_CN_SERVER, NULL, DBE_GET_DUNGEON_EVENT_INFO_ACK, kPacket );
	GetKCnRoomManager()->QueueingEventToRoomManager( spEvent );
}

IMPL_ON_FUNC( DBE_UPDATE_EVENT_MONSTER_KILL_INFO_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	CTime tDieDate = CTime( kPacket_.m_tNpcDieDate );
	const std::wstring wstrDieDate = (const wchar_t*)tDieDate.Format( _T( "%Y-%m-%d %H:%M:%S" ) );

	DO_QUERY( L"exec dbo.P_LExpo_Event_UPD", L"%d, %d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iEventUID
		% kPacket_.m_iKillUnitUID
		% wstrDieDate
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( kPacket_.m_iEventUID )
		<< BUILD_LOG( kPacket_.m_iKillUnitUID )
		<< BUILD_LOG( wstrDieDate )
		<< END_LOG;
}

//}}

//{{ 2010. 12. 8	최육사	적정 레벨 장비 통계
//}}

//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
IMPL_ON_FUNC( DBE_REG_DUNGEON_CLEAR_TIME_HACK_USER_NOT )
{
	DO_QUERY( L"exec dbo.P_LDungeonTimeHackUser_INS", L"%d, %d, N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUserUID
		% kPacket_.m_wstrRegDate
		);

end_proc:
	return;
}
//}}

//{{ 2011. 03. 17	김민성	던전 플레이 로그 요청사항
//}}

//{{ 2011. 06. 13	최육사	스테이지 로그
//}}

//{{ 2011. 07. 27	최육사	대전 개편
IMPL_ON_FUNC( DBE_PVP_MATCH_RESULT_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;
	UidType iArenaLogUID = 0;

	//////////////////////////////////////////////////////////////////////////
	// 로그 기록을 위한 준비 작업

	DO_QUERY( L"exec dbo.P_LArenaUIDCreate_SEL", L"%d",	% KBaseServer::GetKObj()->GetServerGroupID() );

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK
			>> iArenaLogUID );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"로그 기록을 위한 ArenaUID값 얻기 실패!" )
			<< BUILD_LOG( iOK )
			<< END_LOG;
		goto end_proc;
	}


	//////////////////////////////////////////////////////////////////////////
	// 로그 기록!

	BOOST_TEST_FOREACH( const KPvpMatchUserResultLog&, kUserLog, kPacket_.m_vecPvpMatchUserLog )
	{
		iOK = NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_LUnitPVPLog_INS",
			L"%d, "
			L"%d, %d, %d, %d, N\'%s\', %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, N\'%s\', %d, %d, %d, %d, %d, "
			L"%d, %d, %d, %d, %d, N\'%s\'",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% iArenaLogUID
			% (int)kPacket_.m_cMatchType
			% (int)kPacket_.m_cNpcPvpType
			% kUserLog.m_iUnitUID
			% kUserLog.m_wstrNickName
			% (int)kUserLog.m_ucLevel
			% (int)kUserLog.m_cUnitClass
			% kUserLog.m_iRating
			% (int)kUserLog.m_cPvpRank
			% kUserLog.m_iRPoint
			% kUserLog.m_iAPoint
			% kUserLog.m_iOfficialMatchCnt
			% (int)kUserLog.m_cTeam
			% (int)kUserLog.m_bPartyApp
			% kUserLog.m_iWaitTime
			% kUserLog.m_iWin
			% (int)kUserLog.m_cEndType
			% kPacket_.m_iPlayTime
			% kUserLog.m_iKill
			% kUserLog.m_iAssist
			% kUserLog.m_iDeath
			% kUserLog.m_iNetRatingInc
			% kUserLog.m_iNetRPInc
			% kUserLog.m_iNetAPInc
			% kUserLog.m_iEXPearned
			% kPacket_.m_wstrRegDate
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"대전 매칭 유저 로그 기록 실패!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( iArenaLogUID )
				<< BUILD_LOGc( kPacket_.m_cMatchType )
				<< BUILD_LOGc( kPacket_.m_cNpcPvpType )
				<< BUILD_LOG( kUserLog.m_iUnitUID )
				<< BUILD_LOG( kUserLog.m_wstrNickName )
				<< BUILD_LOGc( kUserLog.m_ucLevel )
				<< BUILD_LOGc( kUserLog.m_cUnitClass )
				<< BUILD_LOG( kUserLog.m_iRating )
				<< BUILD_LOGc( kUserLog.m_cPvpRank )
				<< BUILD_LOG( kUserLog.m_iRPoint )
				<< BUILD_LOG( kUserLog.m_iAPoint )
				<< BUILD_LOG( kUserLog.m_iOfficialMatchCnt )
				<< BUILD_LOGc( kUserLog.m_cTeam )
				<< BUILD_LOGc( kUserLog.m_bPartyApp )
				<< BUILD_LOG( kUserLog.m_iWaitTime )
				<< BUILD_LOG( kUserLog.m_iWin )
				<< BUILD_LOGc( kUserLog.m_cEndType )
				<< BUILD_LOG( kPacket_.m_iPlayTime )
				<< BUILD_LOG( kUserLog.m_iKill )
				<< BUILD_LOG( kUserLog.m_iAssist )
				<< BUILD_LOG( kUserLog.m_iDeath )
				<< BUILD_LOG( kUserLog.m_iNetRatingInc )
				<< BUILD_LOG( kUserLog.m_iNetRPInc )
				<< BUILD_LOG( kUserLog.m_iNetAPInc )
				<< BUILD_LOG( kUserLog.m_iEXPearned )
				<< BUILD_LOG( kPacket_.m_wstrRegDate )
				<< END_LOG;
		}
	}

	return;

end_proc:
	BOOST_TEST_FOREACH( const KPvpMatchUserResultLog&, kUserLog, kPacket_.m_vecPvpMatchUserLog )
	{
		START_LOG( cerr, L"대전 매칭 유저 로그 기록 실패!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( iArenaLogUID )
			<< BUILD_LOGc( kPacket_.m_cMatchType )
			<< BUILD_LOGc( kPacket_.m_cNpcPvpType )
			<< BUILD_LOG( kUserLog.m_iUnitUID )
			<< BUILD_LOG( kUserLog.m_wstrNickName )
			<< BUILD_LOGc( kUserLog.m_ucLevel )
			<< BUILD_LOGc( kUserLog.m_cUnitClass )
			<< BUILD_LOG( kUserLog.m_iRating )
			<< BUILD_LOGc( kUserLog.m_cPvpRank )
			<< BUILD_LOG( kUserLog.m_iRPoint )
			<< BUILD_LOG( kUserLog.m_iAPoint )
			<< BUILD_LOG( kUserLog.m_iOfficialMatchCnt )
			<< BUILD_LOGc( kUserLog.m_cTeam )
			<< BUILD_LOGc( kUserLog.m_bPartyApp )
			<< BUILD_LOG( kUserLog.m_iWaitTime )
			<< BUILD_LOG( kUserLog.m_iWin )
			<< BUILD_LOGc( kUserLog.m_cEndType )
			<< BUILD_LOG( kPacket_.m_iPlayTime )
			<< BUILD_LOG( kUserLog.m_iKill )
			<< BUILD_LOG( kUserLog.m_iAssist )
			<< BUILD_LOG( kUserLog.m_iDeath )
			<< BUILD_LOG( kUserLog.m_iNetRatingInc )
			<< BUILD_LOG( kUserLog.m_iNetRPInc )
			<< BUILD_LOG( kUserLog.m_iNetAPInc )
			<< BUILD_LOG( kUserLog.m_iEXPearned )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< END_LOG;
	}
}
//}}

//{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
IMPL_ON_FUNC( ELOG_COLLECTION_OF_RELAY_AND_P2P_INFO_NOT )
{
	BOOST_TEST_FOREACH( const KDBUpdateGamePlayNetworkInfo&, kInfo, kPacket_.m_vecDBUpdateInfo )
	{
		int iOK = NetError::ERR_ODBC_01;

		DO_QUERY( L"exec dbo.P_LP2P_RelayLog_INS ",
			L"%d, %d, %d, %d, %d, N\'%s\', N\'%s\'",
			% KBaseServer::GetKObj()->GetServerGroupID()
			% kInfo.m_iGameType
			% kInfo.m_iHolePunchingTryCount
			% kInfo.m_iP2PPlayTime
			% kInfo.m_iRelayPlayTime
			% kInfo.m_wstrRelayServerIP
			% kInfo.m_wstrRegDate
			);

		if( m_kODBC.BeginFetch() )
		{
			FETCH_DATA( iOK );
			m_kODBC.EndFetch();
		}

end_proc:
		if( iOK != NetError::NET_OK )
		{
			START_LOG( cerr, L"P2P 네트워크 플레이 정보 로그 기록 실패!" )
				<< BUILD_LOG( iOK )
				<< BUILD_LOG( kInfo.m_iGameType )
				<< BUILD_LOG( kInfo.m_iHolePunchingTryCount )
				<< BUILD_LOG( kInfo.m_iP2PPlayTime )
				<< BUILD_LOG( kInfo.m_iRelayPlayTime )
				<< BUILD_LOG( kInfo.m_wstrRelayServerIP )
				<< BUILD_LOG( kInfo.m_wstrRegDate )
				<< END_LOG;
		}
	}
}
//}}

//{{ 2012. 09. 05	박세훈	랜선렉 로그 추가
//}}

//{{ 2012. 10. 18	최육사	배틀필드 시스템
//{{ 2012. 11. 20	박세훈	Field Frame 로그 추가
IMPL_ON_FUNC( ELOG_BATTLE_FIELD_LEAVE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LFieldLog_INS",
		L"%d, "
		L"N\'%s\', %d, %d, %d, %d, N\'%s\', %d, %d, %d, %d, "
		L"%d, N\'%s\', %d, %d",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_wstrServerIP
		% kPacket_.m_iBattleFieldID
		% kPacket_.m_iRoomUID
		% static_cast<int>(kPacket_.m_cLeaveType)
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrNickName
		% static_cast<int>(kPacket_.m_cUnitClass)
		% static_cast<int>(kPacket_.m_ucLevel)
		% kPacket_.m_iPlayTime
		% kPacket_.m_iGetEXP
		% kPacket_.m_iGetED
		% kPacket_.m_wstrRegDate
		% kPacket_.m_iFieldFrame
//#ifdef SERV_ADD_FIELD_LOG_COLUMN
		% kPacket_.m_sNpcDieCount
//#endif SERV_ADD_FIELD_LOG_COLUMN
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

end_proc:
	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"배틀필드 이탈 로그 기록 실패!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_wstrServerIP )
			<< BUILD_LOG( kPacket_.m_iBattleFieldID )
			<< BUILD_LOG( kPacket_.m_iRoomUID )
			<< BUILD_LOGc( kPacket_.m_cLeaveType )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< BUILD_LOGc( kPacket_.m_cUnitClass )
			<< BUILD_LOGc( kPacket_.m_ucLevel )
			<< BUILD_LOG( kPacket_.m_iPlayTime )
			<< BUILD_LOG( kPacket_.m_iGetEXP )
			<< BUILD_LOG( kPacket_.m_iGetED )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< BUILD_LOG( kPacket_.m_iFieldFrame )
			<< BUILD_LOG( kPacket_.m_sNpcDieCount )
			<< END_LOG;
	}
}
//}}
//}}

//{{ 2012. 12. 12	최육사	결과창 이탈 유저 로그
IMPL_ON_FUNC( DBE_DUNGEON_RESULT_LEAVE_LOG_NOT )
{
	int iOK = NetError::ERR_ODBC_01;

	DO_QUERY( L"exec dbo.P_LDungeonLeaveLog_INS", L"%d, %d, N\'%s\', N\'%s\', N\'%s\'",
		% KBaseServer::GetKObj()->GetServerGroupID()
		% kPacket_.m_iUnitUID
		% kPacket_.m_wstrLeaveReason
		% kPacket_.m_wstrRoomState
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
		START_LOG( cerr, L"던전 결과창 이탈 로그 기록 실패!" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrLeaveReason )
			<< BUILD_LOG( kPacket_.m_wstrRoomState )
			<< BUILD_LOG( kPacket_.m_wstrRegDate )
			<< END_LOG;
	}
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
	const std::wstring wstrCurTime = ( std::wstring )( tCurTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) );
	const UidType iServerUID = KBaseServer::GetKObj()->GetUID();

	BOOST_TEST_FOREACH( const KLogStatInfo&, kInfo, kPacket_.m_vecLogStat )
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

//{{ 2013. 2. 26	박세훈	랜선렉 방지 코드2
IMPL_ON_FUNC( DBE_UDP_CHECK_KICK_USER_NOT )
{
	CTime tCurrentTime = CTime::GetCurrentTime();
	CStringW str = tCurrentTime.Format(_T("%Y-%m-%d %H:%M:%S"));
	const std::wstring wstrRegTime(str.GetString());

	DO_QUERY( L"exec dbo.P_LAccusationLog_INT",
		L"%d, %d, %d, %d, N\'%s\'",
		% kPacket_.m_iRoomUID
		% kPacket_.m_iPVPChannelClass
		% kPacket_.m_iMatchType
		% kPacket_.m_iUnitUID
		% wstrRegTime
		);

	int iOK = NetError::ERR_ODBC_01;
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"랜선 렉 로그 등록 실패" )
			<< BUILD_LOG( iOK )
			<< BUILD_LOG( kPacket_.m_iRoomUID )
			<< BUILD_LOG( kPacket_.m_iPVPChannelClass )
			<< BUILD_LOG( kPacket_.m_iMatchType )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( wstrRegTime )
			<< END_LOG;
	}

end_proc:
	return;
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
end_proc:;
	// 보낼 필요가 음다
	//SendToServer( DBE_WRITE_RELATIONSHIP_INFO_LOG_ACK, kPacket );
}
//}



IMPL_ON_FUNC( ELOG_HENIR_HACKING_LOG_NOT )
{
    /*프로시저 명: [dbo].[P_LHenirHackUserLog_INS]

    전달 인자:
               @UserUID     bigint
        ,      @UnitUID     bigint
        ,      @StartMember tinyint
        ,      @PlayTime    int
        ,      @RegDate     smalldatetime

        반환 값: Result (인설트 실패 시 -1 반환)*/
    std::wstring wstrTime = (LPCTSTR)CTime::GetCurrentTime().Format( _T("%y%m%d %H:%M:%S") );
    BOOST_TEST_FOREACH( const KHenirHackUserData&, sData, kPacket_.m_vecHackUser )
    {
        DO_QUERY( L"exec dbo.P_LHenirHackUserLog_INS",
            L"%d, %d, %d, %d, N\'%s\' ",
            % sData.m_iUserUID
            % sData.m_iUnitUID
            % sData.m_iMemberNum
            % sData.m_iPlayTime
            % wstrTime );

        int iOK = NetError::ERR_ODBC_01;
        if( m_kODBC.BeginFetch() )
        {
            FETCH_DATA( iOK );
            m_kODBC.EndFetch();
        }
    }

end_proc:
    return;
}

_IMPL_ON_FUNC( ELOG_GEN_FIELD_MIDDLE_BOSS_STAT_NOT, KFieldBossGenKillStat )
{
    DO_QUERY( L"exec dbo.P_StatsFieldBossSpawn_INS",
        L"%d, %d",
        % kPacket_.m_nFieldID
        % kPacket_.m_nSpawnID
        );

    int iOK = NetError::ERR_ODBC_01;
    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();

    }
end_proc:
    LOG_SUCCESS( iOK == 0 )
        << BUILD_LOG( kPacket_.m_nFieldID )
        << BUILD_LOG( kPacket_.m_nSpawnID )
        << END_LOG;

    return;
}

_IMPL_ON_FUNC( ELOG_KILL_FIELD_MIDDLE_BOSS_STAT_NOT, KFieldBossGenKillStat )
{
    DO_QUERY( L"exec dbo.P_StatsFieldBossKill_INS",
        L"%d, %d, %d",
        % kPacket_.m_nFieldID
        % kPacket_.m_nSpawnID
        % kPacket_.m_nNumUser
        );

    int iOK = NetError::ERR_ODBC_01;
    if( m_kODBC.BeginFetch() )
    {
        FETCH_DATA( iOK );
        m_kODBC.EndFetch();

    }
end_proc:
    LOG_SUCCESS( iOK == 0 )
        << BUILD_LOG( kPacket_.m_nFieldID )
        << BUILD_LOG( kPacket_.m_nSpawnID )
        << END_LOG;

    return;
}

IMPL_ON_FUNC( DBE_BOSS_FIELD_LOG_NOT )
{
	// 방에 대한 로그 쓰기
	DO_QUERY( L"exec dbo.P_LBossFieldLog_INS",
		L"%d, %d, N\'%s\', %d, %d, %d",
		% kPacket_.m_kFieldLog.m_iBossFieldID
		% kPacket_.m_kFieldLog.m_iPortalMapID
		% kPacket_.m_kFieldLog.m_wstrPortalOpenTime
		% kPacket_.m_kFieldLog.m_bSucceed
		% kPacket_.m_kFieldLog.m_iPlayTime
		% kPacket_.m_kFieldLog.m_byteRemainUserCount
		);

	int iOK = NetError::ERR_ODBC_01;
	int iLogUID;

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK
			>> iLogUID
			);
		m_kODBC.EndFetch();
	}

	if( iOK != NetError::NET_OK )
	{
		LOG_SUCCESS( iOK == NetError::NET_OK )
			<< END_LOG;

		goto end_proc;
	}

	// 방에 있었던 유저들의 로그 쓰기
	for( KDBE_BOSS_FIELD_LOG_NOT::TYPE_BOSS_FIELD_USER_LOG::const_iterator it = kPacket_.m_mapUserLog.begin(); it != kPacket_.m_mapUserLog.end(); ++it )
	{
		const std::vector<KBossFieldUserLog>& vecUserLog = it->second;

		BOOST_TEST_FOREACH( const KBossFieldUserLog&, kData, vecUserLog )
		{
			DO_QUERY( L"exec dbo.P_LBossFieldLog_Unit_INS",
				L"%d, %d, N\'%s\', %d, %d, %d, %d, %d, %d, %d, "
				L"%d, %d",
				% iLogUID
				% kData.m_iUnitUID
				% kData.m_wstrNickName
				% kData.m_byteLevel
				% kData.m_byteClass
				% kData.m_byteCompletionType
				% kData.m_byteContributionRank
				% kData.m_iEXP
				% kData.m_iED
				% kData.m_iGivenDamage
				% kData.m_iAttackDamage
				% kData.m_sNumResurrectionStone
				);

			if( m_kODBC.BeginFetch() )
			{
				FETCH_DATA( iOK );
				m_kODBC.EndFetch();
			}
		}
	}

end_proc:
	return;
}
