#include "x2/platform/net.h"
#include <functional>
using namespace std::placeholders;
#include <sstream>
#include <iomanip>
#include "ActorManager.h"
#include "ProxyManager.h"
#include "GameServer.h"
#include "GSSimLayer.h"
#include "GSNetLayer.h"
#include "GSAccountDBThread.h"
#include "GSGameDBThread.h"
#include "GSGameDBThread2nd.h"
#include "GSLogDBThread.h"
#include "GSLogDBThread2nd.h"
#include "GSSMSDBThread.h"




#include "GSBillingDBThread.h"

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}
#include "DBLayer.h"
#include "Socket/NetCommon.h"
#include "KncSend.h"
#include "UserManager.h"
#include "GSUser.h"
#include "GSFSM.h"

#include "NetError.h"
#include "RoomListManager.h"
//{{ 2009. 7. 16  최육사	채널매니저
#include "ChannelManager.h"
//}}

#include "X2Data/XSLRandomItemManager.h"
#include "X2Data/XSLManufactureItemManager.h"
#include "X2Data/XSLCashItemManager.h"
#include "X2Data/XSLTitleManager.h"
#include "X2Data/XSLQuestManager.h"
#include "AbuserLog/KAbuserLogManager.h"
#include "RewardTable.h"
#include "X2Data/XSLGuildManager.h"
#include "GameSysVal/GameSysVal.h"
#include "X2Data/XSLResolveItemManager.h"
#include "X2Data/XSLEnchantItemManager.h"
#include "X2Data/XSLSocketItem.h"
#include "X2Data/XSLAttribEnchantItem.h"
#include "RecommendUserTable.h"
#include "ExpTable.h"
//{{ 2010. 9. 11	최육사	펫 시스템
	#include "X2Data/XSLPetManager.h"
//}}
//{{ 2011. 11. 2	최육사	배틀필드 시스템
	#include "BattleFieldListManager.h"
	#include "GSAutoPartyManager.h"
//}}
//{{ 2012. 10. 19	최육사	배틀필드 시스템
	#include "X2Data/XSLBattleFieldManager.h"
	#include "X2Data/XSLPartyManager.h"
//}}

#include "GameEvent/GameEventManager.h"

//081021.hoons.
#include "FieldManager.h"
//081106.hoons.
#include "TRServer.h"

//{{ 2008. 11. 24  최육사	랭킹 매니저
#include "GSRankingManager.h"
//}}

//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewPartyListManager.h"
//}}

//{{ 2009. 4. 22  최육사	대전유저리스트
#include "PVPLobbyManager.h"
//}}

//{{ 2009. 11. 21  최육사	서버간접속구조개선
#include "NexonBillingTCPManager.h"
//}}

//{{ 2010. 02. 16  최육사	해킹툴 리스트
	#include "HackingProtectionManager.h"
//}}
//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	#include "GSScriptRefreshManager.h"
//}}

	#include "PersonalShopItemList.h"


//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	#include "GameEvent/GameEventScriptManager.h"
//}}


// Port: Intel TBB's malloc proxy (tbbmalloc_proxy.h) removed: the platform allocator is used

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	#include "GSWorldMissionManager.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 07. 23	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
//}}
//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	#include "GSContentManager.h"
//}}

//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	#include "CompanyIPTable.h"
//}}
//{{ 2011. 11. 21  김민성	전직 변경 아이템
	#include "ClassChangeTable.h"
//}}

//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	#include "X2Data/XSLStringFilter.h"
//}}

//{{ 2012. 05. 09	최육사	UDP릴레이 퍼포먼스 체크
//}}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}

//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
//}}

//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
//}}
//{{ 2013. 1. 28	박세훈	빙고 이벤트
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}


//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
//}}

// Port: KSMS (ProcessCommunicationManager, Windows named pipes to an external tool) removed, as in the ChannelServer

//{{ 2013. 3. 11	박세훈	 로컬 랭킹 시스템
//}}
//{{ 2013. 04. 15	최육사	어둠의 문 개편
	#include "X2Data/XSLDefenceDungeonManager.h"
	#include "X2Data/XSLBuffManager.h"
//}}

	#include "X2Data/XSLRidingPetManager.h"

//{{ 2013. 04. 01	 인연 시스템 - 김민성
	#include "WeddingHallListManager.h"
	#include "GSWeddingHall.h"
//}

//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
#include "GSGlobalMissionManager.h"
//}} 2012. 09. 13	임홍락	글로벌 미션 매니저

	#include "X2Data/XSLFieldBossData.h"
	#include "GSFieldBossManager.h"

    #include "X2Data/XSLEDInventoryExpand.h"


//#include "vld.h"


NiImplementRTTI( KGameServer, KBaseServer );
ImplPfID( KGameServer, PI_GS_SERVER );

ImplToStringW( KGameServer )
{
    return START_TOSTRING_PARENTW( KBaseServer );
}

#define CLASS_TYPE      KGameServer

KGameServer::KGameServer(void)
: KBaseServer()

	, m_iTimeEnchantEventUID( 0 )
	, m_iNumOfTimeEnchantEvent( 0 )

	, m_iExchangeLimitUID( 0 )

{
}

KGameServer::~KGameServer(void)
{
}

#include <lua_tinker.h>
//using namespace lua_tinker;

void KGameServer::RegToLua()
{
    KBaseServer::RegToLua();

    //class_<KGameServer>( "KGameServer" )
    //    .inh<KBaseServer>()
    //    .def( "dump", KGameServer::Dump );

    //decl( "Server", this );
	lua_tinker::class_add<KGameServer>( g_pLua, "KGameServer" );
	lua_tinker::class_inh<KGameServer, KBaseServer>( g_pLua );
	lua_tinker::class_def<KGameServer>( g_pLua, "dump",						&KGameServer::Dump );
	lua_tinker::class_def<KGameServer>( g_pLua, "ver",						&KGameServer::ServerVersion );
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}

	lua_tinker::decl( g_pLua, "Server", this );

	// TRServer reg lua
	SiKTRServer()->RegToLua();
}

const wchar_t*  g_szConfigFile = L"config_gs";
KBaseServer*    CreateKNC()                             { return KBaseServer::KeepObject<KGameServer>(); }
KThread*        CreateDBThread( int iDBConnectionInfo, const wchar_t* szDSN, bool bDBConnStr )
{
    switch( iDBConnectionInfo )
    {
    case KDBLayer::DC_ACCOUNT:
        return new KGSAccountDBThread( szDSN, bDBConnStr );
    case KDBLayer::DC_GAME:
        return new KGSGameDBThread( szDSN, bDBConnStr );
    case KDBLayer::DC_LOG:
        return new KGSLogDBThread( szDSN, bDBConnStr );
	case KDBLayer::DC_SMS:
		return new KGSSMSDBThread( szDSN, bDBConnStr );
		//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
		//}}
    case KDBLayer::DC_GAME_2ND:
        return new KGSGameDBThread2nd( szDSN, bDBConnStr );
    case KDBLayer::DC_LOG_2ND:
        return new KGSLogDBThread2nd( szDSN, bDBConnStr );
		//{{ 2010. 11. 12  조효진	GameForge 채팅 로그 관련 처리
		//}}
	case KDBLayer::DC_KOG_BILLING:
		return new KGSBillingDBThread( szDSN, bDBConnStr );




		//{{ 2013. 09. 23	최육사	일본 이벤트 중계DB작업
		//}}


    default:
        START_LOG( cerr, L"Invalid database type for connection." )
            << BUILD_LOG( iDBConnectionInfo )
            << END_LOG;
        return new KGSGameDBThread( szDSN, bDBConnStr );
    }
}

bool KGameServer::Init()
{
    KSimLayer::KeepObject<KGSSimLayer>();
    KNetLayer::KeepObject<KGSNetLayer>();
    KActorManager::KeepObject<KUserManager>();
	//081021.hoons.
	KRoomManager::KeepObject<KFieldManager>();
	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}

    KNetLayer::GetKObj()->SetActorFactory( IActorFactoryPtr( new KActorFactory<KGSUser, KGSFSM, 32*1024> ) );
	//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
	KNetLayer::GetKObj()->SetSessionType( KSession::SST_CLIENT );
	//}}

    _JIF( KBaseServer::Init(), return false );

	// 현재 버전
	START_LOG( cout, GetMainVersion() );

	//081021.hoons. BaseServer::Init() 에 모든 루아파싱루트가 들어있기때문에 파싱후 init을 해야함.
	_JIF( GetKFieldManager()->Init(), return false );

	//////////////////////////////////////////////////////////////////////////
	// TRServer 시작하기 전에 Profiler 싱글톤 객체 생성하기
    //SiKProfiler()->GetInstance();
	//////////////////////////////////////////////////////////////////////////

	//081106.hoons. TCP RELAY START.
	_JIF( SiKTRServer()->Init(), return false );

	dbg::cout << dbg::lbreak;

	//{{ 2009. 11. 21  최육사	서버간접속구조개선
    SiKProxyManager()->ConnectAll();
	//}}
    WriteServerInfoToDB();



	// nProtect 한국인증 모듈 사용설정
	//if( KSimLayer::GetKObj()->CheckCommonFlag( KSimLayer::CommonFlag::CF_NPGG ) == true )
	//{
	//	KNP_Enable( true );

	//	if( KNP_Init() == false )
	//	{
	//		START_LOG( cerr, L"nProtect 초기화 실패.!" )
	//			<< END_LOG;

	//		return false;
	//	}
	//}
	// Port: HackShield and XTrap start-up removed with the anti-cheat (T10)

	SetServerReady( true );

	//{{ 2009. 8. 27  최육사	서버 초기화 완료
	OnServerReadyComplete();
	//}}

	SiKPersonalShopItemList();	// 초기생성을 위해.

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	RequestRandomQuestInfo();
	//}}

	if( GetServerRollType() == KServerInfo::SRT_UPDATE_CASH_PRODUCT_LIST )
	{
		KDBE_USE_GM_TOOL_INSERT_ITEM_INFO_NOT kPacketNOT;
		kPacketNOT.m_CurrentItemTempletNameMap = SiCXSLItemManager()->GetItemNameContainer();
		SendToGameDB( DBE_USE_GM_TOOL_INSERT_ITEM_INFO_NOT, kPacketNOT );
	}

    return true;
}

void KGameServer::OnServerReadyComplete()
{
	KBaseServer::OnServerReadyComplete();

	//{{ 2009. 8. 27  최육사	어뷰저 리스트
	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	SiKAbuserLogManager()->SendAbuserListReq();
	//}}
	//}}

	//{{ 2009. 7. 6  최육사		랭킹 개편
	SiKGSRankingManager()->Init(); // 랭킹 정보 초기화
	//}}

	//{{ 2009. 11. 21  최육사	서버간접속구조개선
	switch( KSimLayer::GetKObj()->GetBillingFlag() )
	{
	case KSimLayer::BF_NEXON_KOREA_TEST:
	case KSimLayer::BF_NEXON_KOREA:
		{
			if( SiKNexonBillingTCPManager()->IsFirstConnectSucc() )
			{
				START_LOG( cout, L"Connected to the Nexon billing authentication server for the first time!" );
			}
			else
			{
				START_LOG( cerr, L"Failed to connect to the Nexon billing authentication server." )
					<< END_LOG;

				// First Connect Success체크 해야만 접속끊겼을때 에러로그 출력함.
				SiKNexonBillingTCPManager()->FirstConnectSucc();
			}
		}
		break;
	}
	//}}

	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}

	// 2013.07.18 lygan_조성욱 // SiKGameEventManager 보다 먼저 초기화 되야함

	// 이벤트 정보 얻기
	SiKGameEventManager()->Init();


	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	SiKGameEventScriptManager()->Init();
	//}}
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	SiKHackingProtectionManager()->Init();
	//}}

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKGSScriptRefreshManager()->Init();
	SiKGSScriptRefreshManager()->RegToLua( g_pLua );
	//}}

	//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
	if( SEnum::CheckFlag( GetServerRollType(), KServerInfo::SRT_UPDATE_CASH_PRODUCT_LIST ) == true )
	{
		KELOG_CASH_PRODUCT_LIST_UPDATE_NOT kPacketToLogDB;
		SiKNexonBillingTCPManager()->GetCashProductList( kPacketToLogDB.m_vecCashProductList );
		SendToLogDB( ELOG_CASH_PRODUCT_LIST_UPDATE_NOT, kPacketToLogDB );

		START_LOG( cout, L"--- Updating the cash item list for statistics ---" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacketToLogDB.m_vecCashProductList.size() );
	}
	//}}

	//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
	//}}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	SiKGSWorldMissionManager()->Init();
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2013. 3. 17	박세훈	 로컬 랭킹 시스템
	//}}


	// 서버군 이름 표시
	START_LOG( cout2, SEnum::GetServerGroupName( GetServerGroupID() ) )
		<< BUILD_LOG( GetServerGroupID() )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetName() );


	//{{  2011.11.24     김민성    캐릭터 접속 로그
	//}}
    // 서버군이 2개 국가에서만 동작
    if ( GetKGSSimLayer()->GetServerGroupNum() == KGSSimLayer::DOUBLE_SERVER_GROUP )
    {
        KDBE_CHANNEL_LIST_REQ kPacketToDB2nd;
        kPacketToDB2nd.m_iServerGroupID = ( GetServerGroupID() == SEnum::SGI_SOLES ? SEnum::SGI_GAIA : SEnum::SGI_SOLES );
        SendToLogDB2nd( DBE_CHANNEL_LIST_REQ, kPacketToDB2nd );
    }



	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}
	SiKGSGlobalMissionManager()->Init();
}

void KGameServer::ShutDown()
{
	//{{ 2013. 3. 17	박세훈	 로컬 랭킹 시스템
	//}}

	//{{ 2009. 9. 2  최육사		비정상종료통계
	GetKGSSimLayer()->SendToDBAbnormalReason( true );
	//}}

    KBaseServer::ShutDown();      // 이곳에서 모든 종료가 끝난다. 각각의 Layer들도 모두 소멸됨.

	//korea nProtect 모듈 정상종료
	//KNP_CleanupGameguardAuth();

	GetKFieldManager()->ShutDown();

	//081106.hoons. tcp relay shutdown
	SiKTRServer()->ShutDown();
	KTRServer::ReleaseInstance();

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKGSScriptRefreshManager()->EndThread();
	KGSScriptRefreshManager::ReleaseInstance();
	//}}

	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}
}

bool KGameServer::DestroyAllSession()
{
    // NetLayer의 Accepter만 닫힌 시점. 아직 모든 Layer들이 살아있다.

    KActorManager* pkActorManager = KActorManager::GetKObj();

    START_LOG( cout, L"Current users: " << pkActorManager->GetCount() << L" users" );

	int iIndex = KStatistics::eSIColDR_Server_Shutdown;

	pkActorManager->ForEach( std::bind( &KActor::SetDisconnectReason, _1, iIndex ) );

    pkActorManager->ForEach( std::bind( &KActor::ReserveDestroy, _1 ) );

    DWORD dwBeginTick = ::GetTickCount();


    // 안정적인 종료 조건 :
    // 1. 모든 유저 객체가 소멸되어야 하고
    // 2. 모든 db event가 처리 fetch 되어야 한다. (처리중일수 있음)
    bool bSuccess = pkActorManager->GetCount() == 0 &&
        SiKDBLayer()->GetTotalEventNum() == 0;

    while( !bSuccess && ::GetTickCount() - dwBeginTick < m_dwShutDownWait )   // 하지만 일정 시간이 지나면 그냥 통과.
    {
        ::Sleep( 500 );

        bSuccess = pkActorManager->GetCount() == 0 &&
            SiKDBLayer()->GetTotalEventNum() == 0;
    }

    // proxy manager의 종료 처리는 일단 유저 객체의 소멸처리를 끝낸 다음에 별도 진행한다.
    bool bProxyShutdown = SiKProxyManager()->Shutdown( 60.0f );

    LOG_CONDITION( bSuccess && bProxyShutdown, cout, cerr )
        << BUILD_LOG( m_dwShutDownWait )
        << BUILD_LOG( bSuccess )
        << BUILD_LOG( bProxyShutdown )
        << BUILD_LOG( pkActorManager->GetCount() )
        << BUILD_LOG( SiKDBLayer()->GetTotalEventNum() )
        << BUILD_LOG( ::GetTickCount() - dwBeginTick );

    return bSuccess && bProxyShutdown;
}

void KGameServer::Tick()
{
	CTime tCurrentTime = CTime::GetCurrentTime();

	KBaseServer::Tick();

	//{{ 2010. 05. 31  최육사	동접 정보 개편
	if( ::GetTickCount() - m_dwServerInfoUpdateTick > 20 * 1000 )
	{
		if( GetServerReady() )
		{
			WriteServerInfoToDB();
		}
	}
	//}}

	//nProtect 암호화 모듈 업데이트
	//KNP_Tick();

	// TRServer tick
	SiKTRServer()->Tick();

//#ifdef SERV_KOG_STATISTICS
//	SiKOGAgent()->ConcurrentUser(GetServerGroupID(), SiKChannelManager()->GetChannelID(), KActorManager::GetKObj()->GetCount());
//
//	if( true == SiKOGAgent()->CheckDailyTime() &&	// 보낼시간이고
//		false == SiKOGAgent()->CheckDaily(CTime::GetCurrentTime()) && // 아직 안보낸 날이며
//		SiKChannelManager()->GetChannelID() == 1 // 1번 채널일 경우
//		)
//	{
//		// 보낸날짜 저장
//		SiKOGAgent()->SetDailyTime(CTime::GetCurrentTime());
//
//		//하루통계를 DB에 요청하자
//		SendToLogDB(DBE_CCU_AGENT_REQ, char());
//	}
//#endif SERV_KOG_STATISTICS

	CheckGSFieldBossSystemRequest( tCurrentTime );
	CheckGSFieldBossSystem( tCurrentTime );



}



void KGameServer::ServerVersion()
{
	KWriteStdout( L"Server Ver : " + GetMainVersion() + L"\n" );
}

//{{ 2010. 10. 19	최육사	SMS테스트
//}}

//{{ 2010. 05. 31  최육사	동접 정보 개편
//////////////////////////////////////////////////////////////////////////

void KGameServer::WriteServerInfoToDB()
{
	[[maybe_unused]] CTime tCurrentTime = CTime::GetCurrentTime();

	// 로그인서버에서 동접 정보 집계하도록 보낸다.
	KELG_UPDATE_CCU_INFO_NOT kNot;
	kNot.m_kCCUInfo.m_iServerUID		  = GetUID();
	kNot.m_kCCUInfo.m_iConcurrentUser	  = KActorManager::GetKObj()->GetCount();
	kNot.m_kCCUInfo.m_iCurPartyCount	  = SiKPartyListManager()->GetCurrentPartyCount();
	kNot.m_kCCUInfo.m_iPlayGamePartyCount = SiKPartyListManager()->GetPlayGamePartyCount();


	SendToLoginServer( ELG_UPDATE_CCU_INFO_NOT, kNot );

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	SendToGlobalServer( EGB_UPDATE_CCU_INFO_NOT, kNot );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	m_dwServerInfoUpdateTick = ::GetTickCount();
}

//////////////////////////////////////////////////////////////////////////
//}}



void KGameServer::ProcessEvent( const KEventPtr& spEvent_ )
{
    KSerializer ks;
    ks.BeginReading( &spEvent_->m_kbuff );

    DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

    switch( spEvent_->m_usEventID )
    {
   _CASE( DBE_SERVER_ON_ACK, KDBE_GAME_SERVER_ON_ACK );
   _CASE( DBE_UPDATE_SERVER_INFO_ACK, KServerList );
	//{{ 2009. 2. 13  최육사	채널리스트
	CASE( DBE_CHANNEL_LIST_ACK );
	//}}
    CASE( ERM_ROOM_LIST_INFO_NOT );
	CASE( ERM_SQUARE_LIST_INFO_NOT );

   _CASE( ECN_NOTIFY_MSG_NOT, KEGS_NOTIFY_MSG_NOT );
	CASE( ENX_UPDATE_PRODUCT_LIST_NOT );
	CASE( DBE_EVENT_UPDATE_ACK );

	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}

	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	//}}
	CASE( DBE_INSERT_TITLE_ACK );
	CASE( DBE_INSERT_REWARD_TO_POST_ACK );
	//{{ 2008. 10. 31  최육사	어뷰저 리스트
	CASE( DBE_ABUSER_LIST_ACK );
	//}}
	//{{ 2008. 11. 5  최육사	광장 개편
	CASE( ERM_UPDATE_FIELD_INFO_NOT );
	//{{ 2009. 4. 22  최육사	대전유저리스트
	CASE( ECN_NEW_PVP_USER_INFO_NOT );
	CASE( ECN_UPDATE_PVP_USER_INFO_NOT );
   _CASE( ECN_CHECK_INVALID_PVP_USER_NOT, UidType );
	//}}
	//{{ 2009. 6. 1  최육사		채널이동
	CASE( ECN_CHECK_CHANNEL_CHANGE_REQ );
	//}}

	//{{ 2009. 6. 3  최육사		확성기
   _CASE( ECN_MEGAPHONE_MESSAGE_NOT, KEGS_MEGAPHONE_MESSAGE_NOT );
	//}}

	//{{ 2009. 7. 6  최육사		랭킹 개편
	CASE( ELG_GET_RANKING_FOR_GS_RANKING_MANAGER_ACK );
	CASE( ELG_HENIR_RANKING_REFRESH_NOT );
	CASE( ELG_WEB_RANKING_REFRESH_NOT );
	//}}

	//{{ 2009. 7. 10  최육사	사기 시스템
	CASE( ERM_INCREASE_PARTY_FEVER_NOT );
	//}}
	//{{ 2012. 05. 10	최육사	배틀필드 시스템
	CASE( ERM_MAX_PARTY_FEVER_NOT );
	//}}

	//{{ 2009. 10. 30  최육사	길드레벨
	CASE( DBE_JOIN_GUILD_ACK );

	CASE( ELG_KICK_RESULT_LETTER_NOT );

	CASE( DBE_EXPAND_GUILD_MAX_MEMBER_ACK );

	CASE( DBE_UPDATE_GUILD_EXP_ACK );

   _CASE( ELG_GUILD_LEVEL_UP_NOTIFY_NOT, KEGS_NOTIFY_MSG_NOT );
	//}}

   //{{ 2010. 02. 01  최육사	길드 게시판
   _CASE( ELG_SEND_LETTER_JOIN_GUILD_NOT, KDBE_ACCEPT_JOIN_GUILD_ACK );
   //}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	CASE( DBE_CHECK_HACKING_TOOL_LIST_ACK );
	//}}

	//{{ 2010. 02. 17  최육사	채널파티통합
	CASE( EPM_OPEN_PARTY_LIST_NOT );
	CASE( EPM_CLOSE_PARTY_LIST_NOT );
	CASE( EPM_ADD_PARTY_LIST_NOT );
	CASE( EPM_DEL_PARTY_LIST_NOT );
	CASE( EPM_UPDATE_PARTY_IN_PARTY_LIST_NOT );
	CASE( EPM_ADD_PARTY_MEMBER_IN_PARTY_LIST_NOT );
	CASE( EPM_DEL_PARTY_MEMBER_IN_PARTY_LIST_NOT );
	CASE( EPM_UPDATE_PARTY_MEMBER_IN_PARTY_LIST_NOT );
	//}}
	//{{ 2012. 02. 28	최육사	배틀필드 시스템
	CASE( EPM_UPDATE_PARTY_FEVER_IN_PARTY_LIST_NOT );
	//}}

	//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
	//}}

    CASE( ERM_PERSONAL_SHOP_INFO_NOT );

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
   _CASE( ESR_ORDER_TO_GAME_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT );
   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
	//}}


	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}

   //{{ 2011. 04. 13  김민성  글로벌 서버 추가
   _CASE( EGB_UPDATE_MISSION_INFO_NOT, KEGB_GET_MISSION_INFO_ACK );
	CASE( EGB_GET_MISSION_INFO_ACK );
	CASE( EGB_UPDATE_WORLD_BUFF_NOT );
	CASE( EGB_UPDATE_MISSION_START_TIME_NOT );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	CASE( EGB_TODAY_RANDOM_QUEST_ACK );
	//}}
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}


	//{{ 2012. 03. 23	박세훈	어둠의 문 개편
	CASE( EGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT );
	//}}
	//{{ 2011. 11. 2	최육사	배틀필드 시스템
	CASE( ERM_BATTLE_FIELD_ROOM_INFO_NOT );
	CASE( EGB_GET_AUTO_PARTY_BONUS_INFO_ACK );
	CASE( EGB_UPDATE_AUTO_PARTY_BONUS_INFO_NOT );
	CASE( ERM_END_GAME_REGROUP_PARTY_NOT );
	CASE( ERM_COMPLETE_RETURN_TO_FIELD_NOT );
	//}}
	//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	//}}
	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}
	//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
	//}}
	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}
	//{{ 2013. 3. 11	박세훈	 로컬 랭킹 시스템
	//}}
	CASE( DBE_CHECK_EVENT_UPDATE_ACK );


	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	_CASE( ELG_WEDDING_HALL_INFO_UPDATE_NOT, KDBE_LOAD_WEDDING_HALL_INFO_ACK );
	CASE( ERM_WEDDING_HALL_INFO_NOT );
	//}

	CASE( EGB_TIME_ENCHANT_EVENT_INFO_NOT );
	CASE( EGB_TIME_ENCHANT_EVENT_INFO_ACK );

	CASE( EGB_EXCHANGE_LIMIT_INFO_NOT );
	CASE( EGB_EXCHANGE_LIMIT_INFO_ACK );



	//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
	_CASE( EGB_UPDATE_GLOBAL_MISSION_INFO_NOT, KEGB_GET_GLOBAL_MISSION_INFO_ACK );
	CASE( EGB_GET_GLOBAL_MISSION_INFO_ACK );
	CASE( EGB_UPDATE_GLOBAL_MISSION_START_TIME_NOT );
	//}} 2012. 09. 03	임홍락	글로벌 미션 매니저

	CASE( ERM_UPDATE_TOTAL_DANGEROUS_VALUE_NOT );
	CASE_NOPARAM( EGB_VERIFY_SERVER_CONNECT_NOT );
	CASE_NOPARAM( EGB_VERIFY_SERVER_DISCONNECT_NOT );
	CASE( EGB_BATTLE_FIELD_BOSS_INFO_NOT );

    _CASE( DBE_CHANNEL_LIST_2ND_ACK, KDBE_CHANNEL_LIST_ACK );

	_CASE( EGS_ADD_COMMON_FLAG_NOT, KEGS_ADD_COMMON_FLAG_NOT );
	_CASE( EGS_DEL_COMMON_FLAG_NOT, KEGS_DEL_COMMON_FLAG_NOT );


    default:
        START_LOG( cerr, L"No event handler is defined - " << spEvent_->GetIDStr() )
            << BUILD_LOG( spEvent_->m_usEventID );
    }

    dwElapTime = ::GetTickCount() - dwElapTime;
    if( dwElapTime > 3000 )
    {
        START_LOG_WITH_NAME( cwarn )
            << BUILD_LOG( spEvent_->GetIDStr() )
            << BUILD_LOG( spEvent_->m_usEventID )
            << BUILD_LOG( dwElapTime );
    }
}

void KGameServer::SendToCnServer( unsigned short usEventID )
{
    SendToCnServer( usEventID, char() );
}

void KGameServer::SendToLoginServer( unsigned short usEventID )
{
	SendToLoginServer( usEventID, char() );
}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
void KGameServer::SendToGlobalServer( unsigned short usEventID )
{
	SendToGlobalServer( usEventID, char() );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

_IMPL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_GAME_SERVER_ON_ACK )
{
	// If the start failed, the server shuts down: Authenticate() returns false and Init() fails.
	// The original called ::exit( 0 ) here, from this thread, while the other threads were still running.
	if( kPacket_.m_iOK != NetError::NET_OK)
	{
		SetAuthenticateResult( false );
		SetEvent( m_spEventAck.get() );
		return;
	}

    SetServerInfo( kPacket_.m_kServerInfo );


	std::map< int, KNetAddress >::iterator mit;
	for( mit = kPacket_.m_mapCenterList.begin(); mit != kPacket_.m_mapCenterList.end(); mit++ )
	{
		SiKProxyManager()->AddRemoteAddress( KProxyManager::PT_CENTER, mit->first, mit->second.m_wstrIP.c_str(), mit->second.m_usPort );
	}

	for( mit = kPacket_.m_mapLoginList.begin(); mit != kPacket_.m_mapLoginList.end(); mit++ )
	{
		SiKProxyManager()->AddRemoteAddress( KProxyManager::PT_LOGIN, mit->first, mit->second.m_wstrIP.c_str(), mit->second.m_usPort );
	}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	for( mit = kPacket_.m_mapGlobalList.begin(); mit != kPacket_.m_mapGlobalList.end(); mit++ )
	{
		SiKProxyManager()->AddRemoteAddress( KProxyManager::PT_GLOBAL, mit->first, mit->second.m_wstrIP.c_str(), mit->second.m_usPort );
	}
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2009. 5. 5  최육사		채널 정보
	SiKChannelManager()->Init( GetServerGroupID(), kPacket_.m_iChannelID, kPacket_.m_kBonusInfo );
	//}}

	SetAuthenticateResult( true );
    SetEvent( m_spEventAck.get() );
}

_IMPL_ON_FUNC( DBE_UPDATE_SERVER_INFO_ACK, KServerList )
{
    SetServerList( kPacket_ );
}

//{{ 2009. 2. 13  최육사	채널리스트
IMPL_ON_FUNC( DBE_CHANNEL_LIST_ACK )
{
	SiKChannelManager()->UpdateChannelList( kPacket_.m_mapChannelList );

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	SiKChannelManager()->UpdateChannelBonusList(kPacket_.m_mapChannelBonusList);
	//}}
}

_IMPL_ON_FUNC( DBE_CHANNEL_LIST_2ND_ACK, KDBE_CHANNEL_LIST_ACK )
{
    SiKChannelManager()->UpdateChannelList2nd( kPacket_.m_mapChannelList );

    SiKChannelManager()->UpdateChannelBonusList2nd( kPacket_.m_mapChannelBonusList );
}

IMPL_ON_FUNC( ERM_ROOM_LIST_INFO_NOT )
{
    switch( kPacket_.m_iCode )
    {
    case NetError::ERR_ROOM_14:
        // 방 정보 추가.
		if( SiKRoomListManager()->AddRoomInfo( kPacket_.m_kRoomInfo ) )
        {
			int iProxyID = ( int )KncUid::ExtractServerID( kPacket_.m_kRoomInfo.m_RoomUID );
            SiKProxyManager()->SetRoomServerLoad( iProxyID, true );
        }
        break;
    case NetError::ERR_ROOM_15:
        // 방 정보 수정.
		SiKRoomListManager()->ChangeRoomInfo( kPacket_.m_kRoomInfo );
        break;
    case NetError::ERR_ROOM_16:
        // 방 정보 삭제.
		if( SiKRoomListManager()->DelRoomInfo( kPacket_.m_kRoomInfo ) )
        {
			int iProxyID = ( int )KncUid::ExtractServerID( kPacket_.m_kRoomInfo.m_RoomUID );
            SiKProxyManager()->SetRoomServerLoad( iProxyID, false );
        }
        break;
    default:
        START_LOG( cerr, L"Invalid room information change code." )
            << BUILD_LOG( kPacket_.m_iCode )
            << END_LOG;

        break;
    }
}

IMPL_ON_FUNC( ERM_SQUARE_LIST_INFO_NOT )
{
	//{{ 2010. 05. 22  최육사	대전 던전 서버군 통합
	if( KncUid::ExtractServerGroupID( kPacket_.m_kSquareInfo.m_iSquareUID ) != GetServerGroupID() )
	{
		START_LOG( cerr, L"Plaza information belongs to another server group; it should not have arrived here." )
			<< BUILD_LOG( kPacket_.m_kSquareInfo.m_iSquareUID )
			<< BUILD_LOG( KncUid::ExtractServerGroupID( kPacket_.m_kSquareInfo.m_iSquareUID ) )
			<< BUILD_LOG( GetServerGroupID() )
			<< END_LOG;
	}
	//}}

	switch( kPacket_.m_iCode )
	{
	case NetError::ERR_ROOM_14:
		// 광장 정보 추가.
		if( SiKRoomListManager()->AddSquareInfo( kPacket_.m_kSquareInfo ) )
		{
			//SiKProxyManager()->SetLoad( kPacket_.m_kRoomInfo.m_RoomUID, true );
		}
		break;
	case NetError::ERR_ROOM_15:
		// 광장 정보 수정.
		SiKRoomListManager()->ChangeSquareInfo( kPacket_.m_kSquareInfo );
		break;
	case NetError::ERR_ROOM_16:
		// 광장 정보 삭제.
		if( SiKRoomListManager()->DelSquareInfo( kPacket_.m_kSquareInfo ) )
		{
			//SiKProxyManager()->SetLoad( kPacket_.m_kRoomInfo.m_RoomUID, false );
		}
		break;
	default:
		START_LOG( cerr, L"Invalid plaza information change code." )
			<< BUILD_LOG( kPacket_.m_iCode )
			<< END_LOG;

		break;
	}
}

_IMPL_ON_FUNC( ECN_NOTIFY_MSG_NOT, KEGS_NOTIFY_MSG_NOT )
{
    UidType anTrace[2] = { 0, -1 };
	KncSend( PI_CN_SERVER, 0, PI_GS_USER, 0 , anTrace, ECN_NOTIFY_MSG_NOT, kPacket_ );

	// GM공지일 경우에만 cout으로 남긴다
	if( kPacket_.m_cNotifyType == KEGS_NOTIFY_MSG_NOT::NT_GM_NOTIFY_ALL_SERVERS )
	{
		START_LOG( cout, L"[ALL SERVERS NOTIFY MESSAGE]" )
			<< L" ----------------------------- "		<< dbg::endl
			<< L"MSG : " << kPacket_.m_wstrMSG.c_str()	<< dbg::endl
			<< L" ----------------------------- "		<< dbg::endl;
	}
	else if( kPacket_.m_cNotifyType == KEGS_NOTIFY_MSG_NOT::NT_GM_NOTIFY_SERVER_GROUP )
	{
		START_LOG( cout, L"[SERVER GROUP NOTIFY MESSAGE]" )
			<< L" ----------------------------- "		<< dbg::endl
			<< L"MSG : " << kPacket_.m_wstrMSG.c_str()	<< dbg::endl
			<< L" ----------------------------- "		<< dbg::endl;
	}
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}
	else
	{
		START_LOG( clog, L"[NOTIFY MESSAGE]" )
			<< L" ----------------------------- "		<< dbg::endl
			<< L"MSG : " << kPacket_.m_wstrMSG.c_str()	<< dbg::endl
			<< L" ----------------------------- "		<< dbg::endl;
	}
}

IMPL_ON_FUNC( ENX_UPDATE_PRODUCT_LIST_NOT )
{
	SiCXSLCashItemManager()->UpdateProductList( kPacket_.m_vecProductItemID, kPacket_.m_vecCashItemRessolveInfo, kPacket_.m_ulProductNoResStone );

	//{{ 2011. 02. 23	최육사	캐쉬 상품 리스트
	if( SEnum::CheckFlag( GetServerRollType(), KServerInfo::SRT_UPDATE_CASH_PRODUCT_LIST ) == true )
	{
		KELOG_CASH_PRODUCT_LIST_UPDATE_NOT kPacketToLogDB;
		kPacketToLogDB.m_vecCashProductList = kPacket_.m_vecCashProductList;
		SendToLogDB( ELOG_CASH_PRODUCT_LIST_UPDATE_NOT, kPacketToLogDB );

		START_LOG( cout, L"--- Updating the cash item list for statistics ---" )
			<< BUILD_LOG( GetUID() )
			<< BUILD_LOG( kPacketToLogDB.m_vecCashProductList.size() );
	}
	//}}
}

IMPL_ON_FUNC( DBE_EVENT_UPDATE_ACK )
{
	if( kPacket_.m_vecEventList.size() > 0 )
	{

		SiKGameEventManager()->SetEvent( kPacket_.m_vecEventList );

	}
}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}
//{{ 2008. 10. 9  최육사	타이틀
//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
//}}

IMPL_ON_FUNC( DBE_INSERT_TITLE_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		SendToLoginServer( ELG_INSERT_TITLE_NOT, kPacket_ );
	}
}

IMPL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_ACK )
{
	// 보상 성공시
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		KELG_FIND_USER_SEND_LETTER_NOT kPacketNot;
		kPacketNot.m_kLetter = kPacket_.m_iRewardLetter;
		SendToLoginServer( ELG_FIND_USER_SEND_LETTER_NOT, kPacketNot );
	}
}
//}}

//{{ 2008. 10. 31  최육사	어뷰저 리스트
//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
IMPL_ON_FUNC( DBE_ABUSER_LIST_ACK )
{
	if( kPacket_.m_mapAbuserList.empty() )
		return;

	START_LOG( cout, L"Updated the abuser list!" );

	// 어뷰저 리스트 업데이트
	SiKAbuserLogManager()->UpdateAbuserList( kPacket_.m_mapAbuserList );
}
//}}
//}}

//{{ 2008. 11. 5  최육사	광장 개편
IMPL_ON_FUNC( ERM_UPDATE_FIELD_INFO_NOT )
{
	// 필드 정보 업데이트
	SiCXSLMapData()->UpdateFieldInfo( kPacket_.m_iMapID, kPacket_.m_iFieldUID );

	START_LOG( cout, L"Field information updated!" )
		<< BUILD_LOG( kPacket_.m_iMapID )
		<< BUILD_LOG( kPacket_.m_iFieldUID );
}
//}}

//{{ 2009. 4. 22  최육사	대전유저리스트
IMPL_ON_FUNC( ECN_NEW_PVP_USER_INFO_NOT )
{
	if( !SiKPVPLobbyManager()->AddPVPUser( kPacket_.m_kUserInfo, kPacket_.m_nGSUID ) )
	{
        START_LOG( cerr, L"Failed to add the PvP user!" )
			<< BUILD_LOG( kPacket_.m_nGSUID )
			<< BUILD_LOG( kPacket_.m_kUserInfo.m_iUnitUID )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( ECN_UPDATE_PVP_USER_INFO_NOT )
{
	std::vector< KPVPUserSimpleInfo >::const_iterator vit;
	for( vit = kPacket_.m_vecUserInfo.begin(); vit != kPacket_.m_vecUserInfo.end(); ++vit )
	{
		if( vit->m_cState == KCommunityUserInfo::US_LEAVE_LOBBY )
		{
			if( !SiKPVPLobbyManager()->RemovePVPUser( vit->m_iUnitUID ) )
			{
				START_LOG( cerr, L"Failed to remove PvP user!" )
					<< BUILD_LOG( vit->m_iUnitUID )
					<< END_LOG;
			}
		}
		else
		{
			if( !SiKPVPLobbyManager()->UpdatePVPUserInfo( vit->m_iUnitUID, vit->m_cState, vit->m_iStateCode, vit->m_iRoomUID ) )
			{
				START_LOG( cwarn, L"Failed to update PvP user!" )
					<< BUILD_LOG( vit->m_iUnitUID )
					<< BUILD_LOGc( vit->m_cState )
					<< BUILD_LOG( vit->m_iStateCode )
					<< BUILD_LOG( vit->m_iRoomUID )
					<< END_LOG;
			}
		}
	}
}

_IMPL_ON_FUNC( ECN_CHECK_INVALID_PVP_USER_NOT, UidType )
{
	// 접속이 끊어진 게임서버의 유저들을 리스트에서 제거하기
	SiKPVPLobbyManager()->RemovePVPUserDisconnectGS( kPacket_ );
}
//}}

//{{ 2009. 6. 1  최육사		채널이동
IMPL_ON_FUNC( ECN_CHECK_CHANNEL_CHANGE_REQ )
{
	KECN_CHECK_CHANNEL_CHANGE_ACK kPacket;
	kPacket.m_iSourGSUID = kPacket_.m_iSourGSUID;
	kPacket.m_iCharUID = kPacket_.m_iCharUID;

	// 현재 동접( 채널이동 예약포함 )
	int iMaxUser = GetMaxUser();
	if( iMaxUser <= 0 )
	{
		// 0나누기 방지
		iMaxUser = 1;
	}

	int iNowUser = KActorManager::GetKObj()->GetCountNoLock() + SiKChannelManager()->GetChannelChangeWaitingUserCount();

	// 들어갈수 있는 공간이 5%가 되어야 채널이동 가능
	if( ( static_cast<float>( iMaxUser - iNowUser ) / static_cast<float>( iMaxUser ) ) < 0.05f )
	{
		START_LOG( clog, L"Cannot transfer channels because there is not enough concurrent-user capacity." )
			<< BUILD_LOG( SiKChannelManager()->GetChannelID() )
			<< BUILD_LOG( iMaxUser )
			<< BUILD_LOG( iNowUser )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_CHANNEL_CHANGE_08;
		goto end_proc;
	}

	// 채널 입장 예약
	if( SiKChannelManager()->NewChannelChangeUser( kPacket_.m_iCharUID, kPacket_.m_kChangeUserInfo ) == false )
	{
		START_LOG( cerr, L"Failed to reserve channel entry" )
			<< BUILD_LOG( kPacket_.m_iCharUID )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_CHANNEL_CHANGE_05;
		goto end_proc;
	}

	// 등록성공!
	kPacket.m_iOK = NetError::NET_OK;

end_proc:
	SendToCnServer( ECN_CHECK_CHANNEL_CHANGE_ACK, kPacket );
}
//}}

//{{ 2009. 6. 3  최육사		확성기
_IMPL_ON_FUNC( ECN_MEGAPHONE_MESSAGE_NOT, KEGS_MEGAPHONE_MESSAGE_NOT )
{
	// 모든 유저들에게 브로드캐스팅
	UidType anTrace[2] = { 0, -1 };
	KncSend( PI_CN_SERVER, 0, PI_GS_USER, 0 , anTrace, ECN_MEGAPHONE_MESSAGE_NOT, kPacket_ );
}
//}}

//{{ 2009. 7. 6  최육사		랭킹 개편
IMPL_ON_FUNC( ELG_GET_RANKING_FOR_GS_RANKING_MANAGER_ACK )
{
	// 헤니르의 시공 랭킹 정보 로그인 서버로부터 받아온것 업데이트
	SiKGSRankingManager()->UpdateHenirRanking( kPacket_.m_mapHenirRanking, kPacket_.m_mapLastRank );
}

IMPL_ON_FUNC( ELG_HENIR_RANKING_REFRESH_NOT )
{
	START_LOG( clog2, L"Received Henir ranking information from the login server!" )
		<< BUILD_LOG( kPacket_.m_mapHenirRanking.size() );

	// 갱신된 헤니르의 시공 랭킹 정보 업데이트
	SiKGSRankingManager()->UpdateHenirRanking( kPacket_.m_mapHenirRanking );
}

IMPL_ON_FUNC( ELG_WEB_RANKING_REFRESH_NOT )
{
	// 갱신된 대전&던전 랭킹 정보 업데이트
	SiKGSRankingManager()->UpdateDungeonAndPvpRanking( kPacket_ );
}
//}}

//{{ 2009. 7. 10  최육사	사기 시스템
IMPL_ON_FUNC( ERM_INCREASE_PARTY_FEVER_NOT )
{
	// 파티 사기수치 증가!
	//{{ 2010. 01. 28  최육사	통합파티
	if( SiKPartyListManager()->E_IncreasePartyFever( kPacket_ ) == false )
	//}}
	{
        START_LOG( cerr, L"Failed to increase the party fraud value!" )
			<< BUILD_LOG( kPacket_.m_iPartyUID )
			<< END_LOG;
	}
}
//}}

//{{ 2012. 05. 10	최육사	배틀필드 시스템
IMPL_ON_FUNC( ERM_MAX_PARTY_FEVER_NOT )
{
	// 파티 사기수치 최대 증가
	if( SiKPartyListManager()->E_MaxPartyFever( kPacket_ ) == false )
	{
		START_LOG( cerr, L"Failed to increase the party fraud value!" )
			<< BUILD_LOG( kPacket_.m_iPartyUID )
			<< END_LOG;
	}
}
//}}

//{{ 2009. 10. 30  최육사	길드레벨

IMPL_ON_FUNC( DBE_JOIN_GUILD_ACK )
{
	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"Guild membership failed; this should never happen!" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_kJoinGuildMember.m_iUnitUID )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< END_LOG;

		// 실패하더라도 로그인 서버 한번 갔다오자.
	}

	// 길드원 정보 업데이트하러 가자!
	KELG_JOIN_GUILD_REQ kPacketToLg;
	kPacketToLg.m_iOK = kPacket_.m_iOK;
	kPacketToLg.m_iUnitUID = kPacket_.m_kJoinGuildMember.m_iUnitUID;
	kPacketToLg.m_iGuildUID = kPacket_.m_iGuildUID;
	kPacketToLg.m_kLoginGuildMember = kPacket_.m_kJoinGuildMember;
	kPacketToLg.m_kLoginGuildMember.m_ucMemberShipGrade = SEnum::GUG_NEW_USER; // 길드 처음 가입했으니 길드 신입 회원으로 등록함
	SendToLoginServer( ELG_JOIN_GUILD_REQ, kPacketToLg );
}

IMPL_ON_FUNC( ELG_KICK_RESULT_LETTER_NOT )
{
	// 길드 우편으로 강퇴 사실을 알린다.
	KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
	kPacketToDB.m_iFromUnitUID = kPacket_.m_iUnitUID;
	kPacketToDB.m_iToUnitUID   = kPacket_.m_iUnitUID;
	kPacketToDB.m_iRewardType  = KPostItemInfo::LT_GUILD_KICK; // 길드 강퇴 우편
	kPacketToDB.m_bGameServerEvent = true;
	kPacketToDB.m_wstrMessage = kPacket_.m_wstrGuildName;
	SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_EXPAND_GUILD_MAX_MEMBER_ACK )
{
	// 길드해산이 성공하면 길드매니저에 업데이트 하러 가자!
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		SendToLoginServer( DLG_UPDATE_EXPAND_GUILD_MAX_MEMBER_NOT, kPacket_ );
	}
}

IMPL_ON_FUNC( DBE_UPDATE_GUILD_EXP_ACK )
{
	// 길드 EXP DB업데이트 성공하면 길드매니저에 업데이트 하러 가자!
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		SendToLoginServer( ELG_UPDATE_GUILD_EXP_NOT, kPacket_ );
	}
}

_IMPL_ON_FUNC( ELG_GUILD_LEVEL_UP_NOTIFY_NOT, KEGS_NOTIFY_MSG_NOT )
{
	// 길드 레벨업 전체 공지를 센터서버로 보내자!
	SendToCnServer( ECN_NOTIFY_MSG_NOT, kPacket_ );
}

//}}

//{{ 2010. 02. 01  최육사	길드 게시판

_IMPL_ON_FUNC( ELG_SEND_LETTER_JOIN_GUILD_NOT, KDBE_ACCEPT_JOIN_GUILD_ACK )
{
	// 길드 우편으로 가입 사실을 알린다.
	KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
	kPacketToDB.m_iFromUnitUID = kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID;
	kPacketToDB.m_iToUnitUID   = kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID;
	kPacketToDB.m_iRewardType  = KPostItemInfo::LT_GUILD_JOIN; // 길드 가입 우편
	kPacketToDB.m_wstrMessage  = kPacket_.m_wstrGuildName;
	kPacketToDB.m_bGameServerEvent = true;
	SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
}

//}}

//{{ 2010. 02. 16  최육사	해킹툴 리스트

IMPL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_ACK )
{
	SiKHackingProtectionManager()->UpdateHackingToolList( kPacket_ );
}

//}}

//{{ 2010. 02. 17  최육사	채널파티통합

IMPL_ON_FUNC( EPM_OPEN_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_OpenPartyList( kPacket_.m_kPartyInfo, kPacket_.m_vecUserInfo );
}

IMPL_ON_FUNC( EPM_CLOSE_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_ClosePartyList( kPacket_.m_iPartyUID );
}

IMPL_ON_FUNC( EPM_ADD_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_AddPartyList( kPacket_.m_iPartyUID );
}

IMPL_ON_FUNC( EPM_DEL_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_DelPartyList( kPacket_.m_iPartyUID );
}

IMPL_ON_FUNC( EPM_UPDATE_PARTY_IN_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_UpdatePartyInfo( kPacket_.m_kPartyInfo );
}

//{{ 2012. 02. 28	최육사	배틀필드 시스템
IMPL_ON_FUNC( EPM_UPDATE_PARTY_FEVER_IN_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_UpdatePartyFever( kPacket_.m_iPartyUID, kPacket_.m_sPartyFever );
}
//}}

IMPL_ON_FUNC( EPM_ADD_PARTY_MEMBER_IN_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_AddPartyMemberInPartyList( kPacket_.m_iPartyUID, kPacket_.m_kPartyUserInfo );
}

IMPL_ON_FUNC( EPM_DEL_PARTY_MEMBER_IN_PARTY_LIST_NOT )
{
	SiKPartyListManager()->E_DelPartyMemberInPartyList( kPacket_.m_iPartyUID, kPacket_.m_iUnitUID );
}

IMPL_ON_FUNC( EPM_UPDATE_PARTY_MEMBER_IN_PARTY_LIST_NOT )
{
	// 먼저 이탈 처리
	std::vector< UidType >::const_iterator vit;
	for( vit = kPacket_.m_vecDeletePartyUser.begin(); vit != kPacket_.m_vecDeletePartyUser.end(); ++vit )
	{
		// 삭제 처리
		SiKPartyListManager()->E_DelPartyMemberInPartyList( kPacket_.m_iPartyUID, *vit );
	}

	// 업데이트
    SiKPartyListManager()->E_UpdatePartyMemberInfo( kPacket_.m_iPartyUID, kPacket_.m_vecUpdatePartyUser );
}

//}}

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

IMPL_ON_FUNC( ERM_PERSONAL_SHOP_INFO_NOT )
{
	switch( kPacket_.m_iCode )
	{
		// 아이템 추가
	case NetError::ERR_PERSONAL_SHOP_26:
		{
			SiKPersonalShopItemList()->AddItemInfo(kPacket_);
		}
		break;
		// 아이템 수정
	case NetError::ERR_PERSONAL_SHOP_27:
		{
			SiKPersonalShopItemList()->ChangeItemInfo(kPacket_);
		}
		break;
		// 아이템 삭제
	case NetError::ERR_PERSONAL_SHOP_28:
		{
			SiKPersonalShopItemList()->DelItemInfo(kPacket_);
		}
		break;

	default:
		START_LOG( cerr, L"Personal shop item list change code error." )
			<< BUILD_LOG( kPacket_.m_iCode )
			<< END_LOG;
		break;
	}
}

//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신

_IMPL_ON_FUNC( ESR_ORDER_TO_GAME_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	// 로그인서버로 가는 명령인지 확인!
	if( KESR_SCRIPT_REFRESH_ORDER_NOT::OT_CN_SENTINEL < kPacket_.m_iOrderType
		&& KESR_SCRIPT_REFRESH_ORDER_NOT::OT_LG_SENTINEL > kPacket_.m_iOrderType
		)
	{
		if( SEnum::CheckFlag( GetServerRollType(), KServerInfo::SRT_LOGIN_SERVER_SCRIPT_REFRESH ) == true )
		{
			UidType anTrace[2] = { GetUID(), -1 };
			KncSend( PI_GS_SERVER, GetUID(), PI_LOGIN_SERVER, 0, anTrace, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
		}
	}
	// 게임서버로 가는 명령인지 확인!
	else if( KESR_SCRIPT_REFRESH_ORDER_NOT::OT_LG_SENTINEL < kPacket_.m_iOrderType  &&
			 KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_SENTINEL > kPacket_.m_iOrderType )
	{
		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_GS_SERVER, NULL, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
		SiKGSScriptRefreshManager()->QueueingEvent( spEvent );
	}
	else
	{
		START_LOG( cerr, L"Invalid update request type for this handler!" )
			<< BUILD_LOG( kPacket_.m_iOrderType )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	std::map< int, UidType > mapFieldInfo;

	// MapData가 갱신되는 경우드롭 테이블이 갱신 되는 경우
	if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_MAP_DATA )
	{
        mapFieldInfo = SiCXSLMapData()->GetFieldInfo();
	}


	switch( kPacket_.m_iOrderType )
	{
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_ITEM_MANAGER, CXSLItemManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_RANDOM_ITEM_MANAGER, CXSLRandomItemManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_MANUFACTURE_ITEM_MANAGER, CXSLManufactureItemManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_REWARD_TABLE, KRewardTable );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_QUEST_MANAGER, CXSLQuestManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_TITLE_MANAGER, CXSLTitleManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_DUNGEON_MANAGER, CXSLDungeonManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_MAP_DATA, CXSLMapData );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_GUILD_MANAGER, CXSLGuildManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_GAME_SYS_VAL, KGameSysVal );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_RESOLVE_ITEM_MANAGER, CXSLResolveItemManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_ENCHANT_ITEM_MANAGER, CXSLEnchantItemManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_SOCKET_ITEM, CXSLSocketItem );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_ATTRIB_ENCHANT_ITEM, CXSLAttribEnchantItem );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_RECOMMEND_USER_TABLE, KRecommendUserTable );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_EXP_TABLE, KExpTable );
	//{{ 2010. 9. 11	최육사	펫 시스템
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_PET_MANAGER, CXSLPetManager );
	//}}
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_ABUSER_LOG_MANAGER, KAbuserLogManager );
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_GAME_EVENT_SCRIPT_MANAGER, KGameEventScriptManager );
	//}}
	//{{ 2011. 07. 23	최육사	대전 개편
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_PVP_MATCH_MANAGER, KPvpMatchManager );
	//}}
	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_CONTENT_MANAGER, KGSContentManager );
	//}}
	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_COMPANY_IP_TABLE, KCompanyIPTable );
	//}}
	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_CLASS_CHANGE_TABLE, KClassChangeTable );
	//}}

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_STRING_FILTER, CXSLStringFilter );
	//}}

	//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
	//}}

	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2012. 10. 19	최육사	배틀필드 시스템
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_BATTLE_FIELD_MANAGER, CXSLBattleFieldManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_PARTY_MANAGER, CXSLPartyManager );
	//}}
	//{{ 2013. 1. 28	박세훈	빙고 이벤트
	//}}


	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_LOG_MANAGER, KLogManager );
	//}}
	//{{ 2013. 04. 15	최육사	어둠의 문 개편
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_DEFENCE_DUNGEON_MANAGER, CXSLDefenceDungeonManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_BUFF_MANAGER, CXSLBuffManager );
	//}}

	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_RIDING_PET_MANAGER, CXSLRidingPetManager );

	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_SKILL_TREE, CXSLSkillTree );

	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_FIELD_BOSS_DATA, CXSLFieldBossData );

    CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GS_ED_INVENTORY_EXPAND, CXSLEDInventoryExpand );
	default:
		{
			START_LOG( cerr, L"This type should not reach this point." )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		break;
	}

	// MapData가 갱신되는 경우드롭 테이블이 갱신 되는 경우
	if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_MAP_DATA )
	{
		for( const auto& kFieldInfo : mapFieldInfo )
		{
			SiCXSLMapData()->UpdateFieldInfo( kFieldInfo.first, kFieldInfo.second );
		}

		SiCXSLMapData()->DumpToLogFile();
	}
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	else if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_GAME_EVENT_SCRIPT_MANAGER )
	{
		SiKGameEventManager()->RefreshEventScript();
	}
	//}}
	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	else if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_CONTENT_MANAGER )
	{
		// 캐쉬샵 상점을 열지 말지를 클라이언트에 알린다.
		KEGS_CASH_SHOP_OPEN_NOT kNot;
		kNot.m_bCashShopOpen = SiKGSContentManager()->IsEnableCashShop();

		{
			KEvent kEvent;
			kEvent.SetData( PI_GS_PROXY, NULL, EGS_CASH_SHOP_OPEN_NOT, kNot );
			KActorManager::GetKObj()->SendToAll( kEvent );
		}

		{
			// 클라이언트에 상품 정보를 다시 다 받으라고 알린다.
			KEvent kEvent;
			kEvent.SetData( PI_GS_PROXY, NULL, EGS_CASH_PRODUCT_INFO_CHANGED_NOT );
			KActorManager::GetKObj()->SendToAll( kEvent );
		}

		START_LOG( cout, L"[Content manager] Broadcast cash shop on/off status to clients!" )
			<< BUILD_LOG( kNot.m_bCashShopOpen );
	}
	//}}
	//{{ 2011. 11. 04	김민성 일일 랜덤 퀘스트 실시간 패치
	else if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_QUEST_MANAGER )
	{
		// Random Quest 를 받아 오자
		SendToGlobalServer( EGB_TODAY_RANDOM_QUEST_REQ, KBaseServer::GetKObj()->GetUID() );
	}
		//}}

}

//}}


//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
_IMPL_ON_FUNC( EGB_UPDATE_MISSION_INFO_NOT, KEGB_GET_MISSION_INFO_ACK )
{
	SiKGSWorldMissionManager()->UpdateMissionInfo( kPacket_ );
}
IMPL_ON_FUNC( EGB_GET_MISSION_INFO_ACK )
{
	SiKGSWorldMissionManager()->UpdateMissionInfo( kPacket_ );
}

IMPL_ON_FUNC( EGB_UPDATE_WORLD_BUFF_NOT )
{
	if( SiKGSWorldMissionManager()->UpdateWorldBuffInfo( kPacket_ ) == true )
	{
		KEGS_UPDATE_WORLD_BUFF_NOT kNot;

		SiKGSWorldMissionManager()->GetWorldBuff( kNot );

		UidType anTrace[2] = { 0, -1 };
		KncSend( PI_CN_SERVER, 0, PI_GS_USER, 0 , anTrace, EGB_UPDATE_WORLD_BUFF_NOT, kNot );
	}
}

IMPL_ON_FUNC( EGB_UPDATE_MISSION_START_TIME_NOT )
{
	SiKGSWorldMissionManager()->UpdateMissionStartTime( kPacket_.m_wsrtStartTime );

	if( kPacket_.m_wsrtStartTime.empty() == true)
	{
		START_LOG( cerr, L"The world buff duration is empty." )
			<< BUILD_LOG( kPacket_.m_wsrtStartTime )
			<< END_LOG;
	}
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
void KGameServer::RequestRandomQuestInfo()
{
	SendToGlobalServer( EGB_TODAY_RANDOM_QUEST_REQ, KBaseServer::GetKObj()->GetUID() );
}

IMPL_ON_FUNC( EGB_TODAY_RANDOM_QUEST_ACK )
{
	SiCXSLQuestManager()->SetRandomQuestGroupList( kPacket_.m_mapTodayRandomQuest );

	// 접속해 있는 모든 유저에게 알려준다
	KEGS_CHANGE_RANDOM_QUEST_NOT kNot;
	kNot.m_mapRandomQuestList = kPacket_.m_mapTodayRandomQuest;

	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_GS_SERVER, NULL, EGS_CHANGE_RANDOM_QUEST_NOT, kNot );
	KActorManager::GetKObj()->SendToAll( *spEvent );
}

//}}


//{{ 2011. 12. 16	최육사	동접 최대 인원 실시간 적용
int KGameServer::GetMaxUser() const
{
	// 채널매니저에 저장된 최대인원을 먼저 찾는다.
	int iChannelMaxUser = SiKChannelManager()->GetMaxUser();
	if( iChannelMaxUser == 0 )
	{
        iChannelMaxUser = GetDefaultMaxUser();
	}

	return iChannelMaxUser;
}
//}}

//{{ 2011. 09. 29  김민성	셧다운제 도입
//}}

//{{ 2012. 03. 23	박세훈	어둠의 문 개편
IMPL_ON_FUNC( EGB_THE_GATE_OF_DARKNESS_INDIVIDUAL_BUFF_NOT )
{
	//{{ 2013. 04. 15	박세훈	어둠의 문 개편
	SiKGSWorldMissionManager()->UpdateIndividualBuffInfo( kPacket_ );
	//}}
}
//}}

//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
//}}

//{{ 공존의 축제 - 지진 효과 - 김민성
//}}

//{{ 2011. 11. 2	최육사	배틀필드 시스템
IMPL_ON_FUNC( ERM_BATTLE_FIELD_ROOM_INFO_NOT )
{
	switch( kPacket_.m_cType )
	{
	case KERM_BATTLE_FIELD_ROOM_INFO_NOT::IT_UPDATE:
		{
			// 변경된 배틀필드 정보를 넣는다!
			SiKBattleFieldListManager()->UpdateBattleFieldRoomInfo( kPacket_.m_kBFRoomInfo );
		}
		break;

	case KERM_BATTLE_FIELD_ROOM_INFO_NOT::IT_DELETE:
		{
			// 변경된 배틀필드 정보를 넣는다!
			SiKBattleFieldListManager()->DeleteBattleFieldRoomInfo( kPacket_.m_kBFRoomInfo );
		}
		break;

	default:
		{
			START_LOG( cerr, L"Updating battlefield room information" )
				<< BUILD_LOGc( kPacket_.m_cType )
				<< END_LOG;
		}
		break;
	}
}

IMPL_ON_FUNC( EGB_GET_AUTO_PARTY_BONUS_INFO_ACK )
{
	for( const KAutoPartyBonusInfo& kInfo : kPacket_.m_vecAutoPartyBonusInfo )
	{
        SiKGSAutoPartyManager()->UpdateAutoPartyBonusInfo( kInfo.m_cDungeonMode, kInfo.m_iDungeonIDWithDif, kInfo.m_bActivatedBonus );
	}
}

IMPL_ON_FUNC( EGB_UPDATE_AUTO_PARTY_BONUS_INFO_NOT )
{
	SiKGSAutoPartyManager()->UpdateAutoPartyBonusInfo( kPacket_.m_cDungeonMode, kPacket_.m_iDungeonIDWithDif, kPacket_.m_bActivatedBonus );
}

IMPL_ON_FUNC( ERM_END_GAME_REGROUP_PARTY_NOT )
{
	// 파티 재결합 요청
	SiKPartyListManager()->E_EndGamePartyRegroup( kPacket_.m_iPartyUID );
}

IMPL_ON_FUNC( ERM_COMPLETE_RETURN_TO_FIELD_NOT )
{
    KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_CN_ROOM, kPacket_.m_kReturnToFieldInfo.m_iRoomUID, NULL, ERM_NEXT_RETURN_TO_FIELD_NOT, kPacket_ );
}
//}}

//{{ 2012. 10. 16	최육사		ED&아이템 모니터링 시스템
//}}

//{{ 2012. 12. 17	박세훈	잭팟 이벤트
//}}



IMPL_ON_FUNC( DBE_CHECK_EVENT_UPDATE_ACK )
{
	std::map<int , int>::const_iterator cmit = kPacket_.m_mapReleaseTick.find( KGameEventManager::ERTT_EVENT_CHECK );

	if( cmit !=  kPacket_.m_mapReleaseTick.end() )
	{
		int iOldReleaseTick = SiKGameEventManager()->GetEventReleaseTick();
		int iNewReleaseTick = cmit->second;
		if( iOldReleaseTick != iNewReleaseTick )
		{
			START_LOG( cout2, L"Event changed; retrieving the latest information." )
				<< BUILD_LOG( iOldReleaseTick )
				<< BUILD_LOG( iNewReleaseTick )
				<< END_LOG;

			SendToLogDB( DBE_EVENT_UPDATE_REQ );

			SiKGameEventManager()->SetEventReleaseTick( iNewReleaseTick );
		}
	}
}


//{{ 2013. 04. 01	 인연 시스템 - 김민성
_IMPL_ON_FUNC( ELG_WEDDING_HALL_INFO_UPDATE_NOT, KDBE_LOAD_WEDDING_HALL_INFO_ACK )
{
	std::map<int, KWeddingHallInfo>::iterator mit = kPacket_.m_mapWeddingInfo.begin();
	for(  ; mit != kPacket_.m_mapWeddingInfo.end() ; ++mit )
	{
		SiKWeddingHallListManager()->AddWeddingHallInfo( mit->second );
	}
}

IMPL_ON_FUNC( ERM_WEDDING_HALL_INFO_NOT )
{
	switch( kPacket_.m_cType )
	{
	case KERM_WEDDING_HALL_INFO_NOT::IT_INSERT:
		{
			// 새로 생성된 결혼식장 정보
			SiKWeddingHallListManager()->AddWeddingHallInfo( kPacket_.m_kHallInfo );
		}
		break;
	case KERM_WEDDING_HALL_INFO_NOT::IT_UPDATE:
		{
			// 새로 생성된 결혼식장 정보
			SiKWeddingHallListManager()->UpdateWeddingHallInfo( kPacket_.m_kHallInfo, kPacket_.m_vecJoinUnitList );
		}
		break;
	case KERM_WEDDING_HALL_INFO_NOT::IT_DELETE:
		{
			// 소멸된 결혼식장 정보
			SiKWeddingHallListManager()->DeleteWeddingHallInfo( kPacket_.m_kHallInfo.m_iWeddingUID );
		}
		break;

	default:
		{
			START_LOG( cerr, L"Updating wedding hall information" )
				<< BUILD_LOGc( kPacket_.m_cType )
				<< END_LOG;
		}
		break;
	}
}
//}

bool KGameServer::PopFirstTimeEnchantEvent( void )
{
	if( m_iNumOfTimeEnchantEvent <= 0 )
	{
		return false;
	}

	--m_iNumOfTimeEnchantEvent;
	return true;
}

IMPL_ON_FUNC( EGB_TIME_ENCHANT_EVENT_INFO_NOT )
{
	m_iTimeEnchantEventUID		= kPacket_.m_iTimeEnchantEventUID;
	m_iNumOfTimeEnchantEvent	= kPacket_.m_iNumOfTimeEnchantEvent;
}

IMPL_ON_FUNC( EGB_TIME_ENCHANT_EVENT_INFO_ACK )
{
	m_iTimeEnchantEventUID		= kPacket_.m_iTimeEnchantEventUID;
	m_iNumOfTimeEnchantEvent	= kPacket_.m_iNumOfTimeEnchantEvent;
}

int	KGameServer::DecreaseExchangeLimit( IN const int iDestItemID )
{
	if( m_iExchangeLimitUID == 0 )
	{
		return ELR_NOT_INITED;
	}

	std::map< int, int >::iterator it = m_mapExchangeLimitInfo.find( iDestItemID );
	if( it == m_mapExchangeLimitInfo.end() )
	{
		// 목록 자체에 없다면, 제한 대상이 아니다.
		return ELR_PASS;
	}

	if( it->second <= 0 )
	{
		// 목록엔 있지만, 수량이 소진되었다.
		return ELR_EXHAUSTED;
	}

	// 수량을 감소 시키고, 체크하러 가자.
	--it->second;

	return ELR_CHECK;
}

IMPL_ON_FUNC( EGB_EXCHANGE_LIMIT_INFO_NOT )
{
	m_iExchangeLimitUID		= kPacket_.m_iExchangeLimitUID;
	m_mapExchangeLimitInfo	= kPacket_.m_mapExchangeLimitInfo;
}

IMPL_ON_FUNC( EGB_EXCHANGE_LIMIT_INFO_ACK )
{
	m_iExchangeLimitUID		= kPacket_.m_iExchangeLimitUID;
	m_mapExchangeLimitInfo	= kPacket_.m_mapExchangeLimitInfo;
}


//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
_IMPL_ON_FUNC( EGB_UPDATE_GLOBAL_MISSION_INFO_NOT, KEGB_GET_GLOBAL_MISSION_INFO_ACK )	// 게임 서버에 GlobalMissionInfo 데이터 브로드 캐스팅
{
	SiKGSGlobalMissionManager()->UpdateGlobalMissionInfo( kPacket_ );
}

IMPL_ON_FUNC( EGB_GET_GLOBAL_MISSION_INFO_ACK )
{
	SiKGSGlobalMissionManager()->UpdateGlobalMissionInfo( kPacket_ );
}

IMPL_ON_FUNC( EGB_UPDATE_GLOBAL_MISSION_START_TIME_NOT )
{
	//SiKGSGlobalMissionManager()->UpdateGlobalMissionInfo( kPacket_.m_wstrStartTime );
}
//}} 2012. 09. 03	임홍락	글로벌 미션 매니저

void KGameServer::CheckGSFieldBossSystemRequest( IN const CTime tCurrentTime )
{
	const __time64_t tReservedTimeForInitRequest = SiKGSFieldBossManager()->GetReservedTimeForInitRequest();

	if( tReservedTimeForInitRequest == 0 )
		return;

	if( tCurrentTime.GetTime() < tReservedTimeForInitRequest )
		return;

	// 실패를 대비하여 1분 뒤에 또 시도하도록 예약하자
	SiKGSFieldBossManager()->SetReservedTimeForInitRequest( tCurrentTime.GetTime() + 60 );
	KncSend( PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GLOBAL_USER, 0, NULL, EGB_BATTLE_FIELD_BOSS_INFO_NOT, char() );
}

void KGameServer::CheckGSFieldBossSystem( IN const CTime tCurrentTime )
{
	if( SiKGSFieldBossManager()->CheckTerm( tCurrentTime.GetTime() ) == false )
		return;

	// 글로벌 서버와 연결되어 있다면, 자체적으로 처리할 필요 없다.
	if( SiKGSFieldBossManager()->IsConnected() == true )
		return;

	const bool bPortalOpen	= SiKGSFieldBossManager()->IsPortalOpen();
	const bool bCoolTime	= SiKGSFieldBossManager()->IsCoolTime();

	const __time64_t tPortalAppearanceTime = SiKGSFieldBossManager()->GetPortalAppearanceTime();

	// 포탈이 열려 있는지 확인
	if( bPortalOpen == true )
	{
		const __time64_t tPortalHoldingTime = tPortalAppearanceTime + SiKGSFieldBossManager()->GetRemainPortalTime();
		if( tPortalHoldingTime <= tCurrentTime.GetTime() )
		{
			// 포탈 닫힘 처리
			SiKGSFieldBossManager()->ClosePortal();

			// 모든 유저들에게 브로드캐스팅
			KEGS_BATTLE_FIELD_BOSS_GATE_CLOSE_NOT kPacket;
			kPacket.m_iBattleFieldID	= SiKGSFieldBossManager()->GetPortalAppearanceMap();

			UidType anTrace[2] = { 0, -1 };
			KncSend( PI_CN_SERVER, 0, PI_GS_USER, 0 , anTrace, EGS_BATTLE_FIELD_BOSS_GATE_CLOSE_NOT, kPacket );
		}
	}

	// 쿨 타임 체크
	if( bCoolTime == true )
	{
		const __time64_t tFieldBossCoolTime = tPortalAppearanceTime + SiKGSFieldBossManager()->GetRemainCoolTime();
		if( tFieldBossCoolTime <= tCurrentTime.GetTime() )
		{
			SiKGSFieldBossManager()->SetCoolTimeState( false );
		}
	}
}

IMPL_ON_FUNC( ERM_UPDATE_TOTAL_DANGEROUS_VALUE_NOT )
{
	SendToGlobalServer( EGB_UPDATE_TOTAL_DANGEROUS_VALUE_NOT, kPacket_ );
}

IMPL_ON_FUNC_NOPARAM( EGB_VERIFY_SERVER_CONNECT_NOT )
{
	// 바로 요청을 보내도록 하자
	SiKGSFieldBossManager()->SetReservedTimeForInitRequest( 1 );
	SiKGSFieldBossManager()->SetConnected( true );
}

IMPL_ON_FUNC_NOPARAM( EGB_VERIFY_SERVER_DISCONNECT_NOT )
{
	// 연결이 끊어져있으면 정보 요청 패킷을 보내지 말자
	SiKGSFieldBossManager()->SetReservedTimeForInitRequest( 0 );
	SiKGSFieldBossManager()->SetConnected( false );
}

IMPL_ON_FUNC( EGB_BATTLE_FIELD_BOSS_INFO_NOT )
{
	// 초기화 정보를 얻었으니 계속해서 정보 요청 패킷을 보낼 필요가 없다.
	SiKGSFieldBossManager()->SetReservedTimeForInitRequest( 0 );

	const bool bPortalOpen	= SiKGSFieldBossManager()->IsPortalOpen();

	SiKGSFieldBossManager()->SetInfo( CTime::GetCurrentTime().GetTime()
									, kPacket_.m_tRemainPortalTime
									, kPacket_.m_tRemainCoolTime
									, kPacket_.m_iPortalAppearanceMap
									, kPacket_.m_iPortalDestination
									, kPacket_.m_bPortalOpen
									, kPacket_.m_bCoolTime
									);

	// 포탈 Open/Close에 변화가 없다면 루틴 종료
	if( bPortalOpen == kPacket_.m_bPortalOpen )
		return;

	// BroadCasting
	KEvent kEvent;

	if( kPacket_.m_bPortalOpen == true )
	{
		KEGS_BATTLE_FIELD_BOSS_GATE_OPEN_NOT kPacket;
		kPacket.m_iBattleFieldID	= kPacket_.m_iPortalAppearanceMap;
		kPacket.m_iBossFieldID		= kPacket_.m_iPortalDestination;

		// 모든 유저들에게 브로드캐스팅
		kEvent.SetData( PI_GS_PROXY, NULL, EGS_BATTLE_FIELD_BOSS_GATE_OPEN_NOT, kPacket );
	}
	else
	{
		KEGS_BATTLE_FIELD_BOSS_GATE_CLOSE_NOT kPacket;
		kPacket.m_iBattleFieldID	= kPacket_.m_iPortalAppearanceMap;

		// 모든 유저들에게 브로드캐스팅
		kEvent.SetData( PI_GS_PROXY, NULL, EGS_BATTLE_FIELD_BOSS_GATE_CLOSE_NOT, kPacket );
	}

	KActorManager::GetKObj()->SendToAll( kEvent );
}

_IMPL_ON_FUNC( EGS_ADD_COMMON_FLAG_NOT, KEGS_ADD_COMMON_FLAG_NOT )
{
	START_LOG( cout, L"EGS_ADD_COMMON_FLAG_NOT Recieve UID: " << GetUID() );
	KSimLayer::GetKObj()->AddCommonFlag( kPacket_.dwFlag );
}
_IMPL_ON_FUNC( EGS_DEL_COMMON_FLAG_NOT, KEGS_DEL_COMMON_FLAG_NOT )
{
	START_LOG( cout, L"EGS_DEL_COMMON_FLAG_NOT Recieve UID: " << GetUID() );
	KSimLayer::GetKObj()->DeleteCommonFlag( kPacket_.dwFlag );
}
