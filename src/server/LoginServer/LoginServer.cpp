#include "x2/platform/net.h"
#include <sstream>
#include <iomanip>
#include <functional>
using namespace std::placeholders;
#include "LoginServer.h"
#include "ActorManager.h"
#include "LoginSimLayer.h"
#include "LoginNetLayer.h"
#include "Socket/NetCommon.h"
//{{ 2011. 04. 29	최육사	대리상인
	#include "LoginRoomManager.h"
//}}
#include "DBLayer.h"
#include "LoginAccountDBThread.h"
//{{ 2009. 11. 9  최육사	로그인서버GameDB
#include "LoginGameDBThread.h"
//}}
#include "LoginLogDBThread.h"
//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}
#include "LoginBillingDBThread.h"


#include "DefaultFSM.h"
#include "LoginUser.h"
//{{ 2009. 7. 6  최육사		랭킹개편
#include "RankingManager.h"
//}}
//{{ 2009. 11. 26  최육사	길드스킬
	#include "GuildManager.h"
//}}
//{{ 2010. 01. 15  최육사	길드게시판
	#include "GuildBoardManager.h"
//}}
//{{ 2010. 06. 01  최육사	동접 처리 개편
	#include "ServerCCUManager.h"
//}}

//{{ 2010. 06. 22  최육사	계정블럭기능
	#include "AccountBlockManager.h"
//}}
//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	#include "LoginScriptRefreshManager.h"
//}}

//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

//{{ 2010. 10. 13	최육사	DLL Manager
	#include "DLLManager.h"
//}}
//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
	#include "RandomItemEventManager.h"
//}}
//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	#include "GameSysVal/GameSysVal.h"
//}}

#include "X2Data/XSLGuildManager.h"
//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	#include "X2Data/XSLTitleManager.h"
//}}

//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}
//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
//}}

//{{ 2013. 04. 01	 인연 시스템 - 김민성
	#include "WeddingHallManager.h"
//}

#include "X2Data/XSLSkillTree.h"

#include "NetError.h"

//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
//}}

// Port: KSMS (ProcessCommunicationManager, Windows named pipes to an external tool) removed, as in the ChannelServer

//{{ 2011. 03. 29	최육사	TBB 메모리 관리자
//#ifdef SERV_TBB_MALLOC_PROXY_TEST
//	#include <tbb/tbbmalloc_proxy.h>
//#endif SERV_TBB_MALLOC_PROXY_TEST
//}}

//#include "vld.h"


NiImplementRTTI( KLoginServer, KBaseServer );
ImplPfID( KLoginServer, PI_LOGIN_SERVER );

#define CLASS_TYPE KLoginServer

ImplToStringW( KLoginServer )
{
    return START_TOSTRING_PARENTW( KBaseServer );
}

KLoginServer::KLoginServer(void)
{
}

KLoginServer::~KLoginServer(void)
{
}

#include <lua_tinker.h>

void KLoginServer::RegToLua()
{
    KBaseServer::RegToLua();

    //lua_tinker::class_<KLoginServer>( "KLoginServer" )
    //    .inh<KBaseServer>()
    //    .def( "dump", KLoginServer::Dump );

    //lua_tinker::decl( "Server", this );
	lua_tinker::class_add<KLoginServer>( g_pLua, "KLoginServer" );
	lua_tinker::class_inh<KLoginServer, KBaseServer>( g_pLua );
	//{{ 2010. 06. 22  최육사	OTP적용
	lua_tinker::class_def<KLoginServer>( g_pLua, "SetPassportLevel",	&KLoginServer::SetPassportLevel_LUA );
	//}}
	lua_tinker::class_def<KLoginServer>( g_pLua, "dump",				&KLoginServer::Dump );
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}


	lua_tinker::decl( g_pLua, "Server", this );
}

const wchar_t*  g_szConfigFile = L"config_lg";
KBaseServer*    CreateKNC()                             { return KBaseServer::KeepObject<KLoginServer>(); }
KThread*        CreateDBThread( int iDBConnectionInfo, const wchar_t* szDSN, bool bDBConnStr )
{
    switch( iDBConnectionInfo )
    {
    case KDBLayer::DC_ACCOUNT:
        return new KLoginAccountDBThread( szDSN, bDBConnStr );
		//{{ 2009. 11. 9  최육사	로그인서버GameDB
	case KDBLayer::DC_GAME:
		return new KLoginGameDBThread( szDSN, bDBConnStr );
		//}}
    case KDBLayer::DC_LOG:
        return new KLoginLogDBThread( szDSN, bDBConnStr );
		//{{ 2010. 10. 12	최육사	서버 모니터링
		//}}
		//{{ 2010. 02. 25  최육사	웹 포인트 이벤트
		//}}
	case KDBLayer::DC_KOG_BILLING:
		return new KLoginBillingDBThread( szDSN, bDBConnStr );
    default:
        START_LOG( cerr, L"접속하려는 DB 종류가 이상함." )
            << BUILD_LOG( iDBConnectionInfo )
            << END_LOG;
        return new KLoginAccountDBThread( szDSN, bDBConnStr );
    }
}

//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
//}}

bool KLoginServer::Init()
{
    KSimLayer::KeepObject<KLoginSimLayer>();
    KNetLayer::KeepObject<KLoginNetLayer>();
    KActorManager::KeepObject<KActorManager>();
	//{{ 2011. 04. 29	최육사	대리상인
	KRoomManager::KeepObject<KLoginRoomManager>();
	//}}

    KNetLayer::GetKObj()->SetActorFactory( IActorFactoryPtr( new KActorFactory<KLoginUser, KDefaultFSM, 512*1024> ) );
	//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
	KNetLayer::GetKObj()->SetSessionType( KSession::SST_SERVER );
	//}}

    _JIF( KBaseServer::Init(), return false );

    //WriteServerInfoToDB();


	//{{ 2011. 04. 29	최육사	대리상인
	_JIF( GetKLoginRoomManager()->Init(), return false );
	//}}
	//{{ 최육사 : [2012/10/9] //	태국 OTP 인증 통신 모듈
	//}}


	SetServerReady( true );

	//{{ 2009. 8. 27  최육사	server ready
	OnServerReadyComplete();
	//}}

    return true;
}

void KLoginServer::OnServerReadyComplete()
{
	KBaseServer::OnServerReadyComplete();

	// Port: Nexon AuthCheck (proprietary AuthCheck.lib: passport level and locale) removed

	//{{ 2009. 7. 6  최육사		랭킹 개편
	SiKRankingManager()->Init();
	//}}

	//{{ 2010. 01. 15  최육사	길드게시판
	SiKGuildBoardManager()->Init();
	//}}

	//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
	SiKRandomItemEventManager()->Init();
	//}}

	//{{ 2010. 06. 01  최육사	동접 처리 개편
	// 서버 켜자마자 현시간부 동접 정보 업데이트
	SiKServerCCUManager()->DBUpdateCCUInfo();
	//}}

	//{{ 2010. 06. 22  최육사	계정블럭기능
	SiKAccountBlockManager()->Init();
	//}}

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKLoginScriptRefreshManager()->Init();
	//}}

	//{{ 2010. 10. 13	최육사	DLL Manager
	SiKDLLManager()->OnServerReadyComplete();
	//}}

	//{{ 2011. 04. 18	최육사	대리 상인
	SendToGameDB( DBE_LOAD_PSHOP_AGENCY_REQ, char() );
	//}}

	//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
	//}}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	SiKWeddingHallManager()->Init();
	//}
	// 서버군 이름 표시
	START_LOG( cout2, SEnum::GetServerGroupName( GetServerGroupID() ) )
		<< BUILD_LOG( GetServerGroupID() )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetName() );
}

void KLoginServer::ShutDown()
{

	//{{ 2013. 3. 15	박세훈	 로컬 랭킹 시스템
	//}}

	//{{ 2009. 7. 6  최육사		랭킹 개편 -	DBLayer가 죽기전에 랭킹정보를 업데이트 해야한다.
    SiKRankingManager()->ShutDown();
	//}}

	//{{ 2010. 10. 13	최육사	DLL Manager
	SiKDLLManager()->Shutdown();
	//}}

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKLoginScriptRefreshManager()->EndThread();
	KLoginScriptRefreshManager::ReleaseInstance();
	//}}


    // 아직 아무 종료처리도 진행되기 전.
    KBaseServer::ShutDown();        // 이곳에서 모든 종료가 끝난다. 각각의 Layer들도 모두 소멸됨.

	//{{ 2011. 04. 29	최육사	대리상인
	GetKLoginRoomManager()->ShutDown();
	//}}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	SiKWeddingHallManager()->ShutDown();
	//}
}

bool KLoginServer::DestroyAllSession()
{
    // NetLayer의 Accepter만 닫힌 시점. 아직 모든 Layer들이 살아있다.

    KActorManager* pkActorManager = KActorManager::GetKObj();

    START_LOG( cout, L"현재 유저 : " << pkActorManager->GetCount() << L" 명" );

	//{{ 2009. 1. 30  최육사	서버 셧다운 종료 사유 설정
	int iIndex = KStatistics::eSIColDR_Server_Shutdown;

	pkActorManager->ForEach( std::bind( &KActor::SetDisconnectReason, _1, iIndex ) );
	//}}

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

    LOG_CONDITION( bSuccess, cout, cerr )
        << BUILD_LOG( m_dwShutDownWait )
        << BUILD_LOG( pkActorManager->GetCount() )
        << BUILD_LOG( SiKDBLayer()->GetTotalEventNum() )
        << BUILD_LOG( ::GetTickCount() - dwBeginTick );

    return bSuccess;
}

void KLoginServer::Tick()
{
    KBaseServer::Tick();

	//{{ 2008. 5. 27  최육사  체험ID 동접
	//{{ 2010. 06. 01  최육사	동접 처리 개편
//#ifdef SERV_CCU_NEW
//	if( ::GetTickCount() - m_dwServerInfoUpdateTick > 20 * 1000 )
//#else
//	if( ::GetTickCount() - m_dwServerInfoUpdateTick > ms_dwServerInfoUpdateGap )
//#endif SERV_CCU_NEW
//	//}}
//    {
//        if( GetServerReady() )
//        {
//            WriteServerInfoToDB();
//        }
//    }
	//}}

	//{{ 2010. 06. 01  최육사	동접 처리 개편
	if( GetServerReady() )
	{
		SiKServerCCUManager()->Tick();
	}
	//}}

	//{{ 2010. 06. 22  최육사	계정블럭기능
	SiKAccountBlockManager()->Tick();
	//}}

}

//{{ 2008. 5. 27  최육사  체험ID 동접
void KLoginServer::WriteServerInfoToDB()
{
	if( KSimLayer::GetKObj()->GetAuthFlag() == KSimLayer::AF_NEXON_KOREA )
	{
		//{{ 2010. 06. 01  최육사	동접 처리 개편
		KUpdateCCUInfo kInfo;
		kInfo.m_iServerUID			= GetUID();
		kInfo.m_iConcurrentUser		= GetKLoginSimLayer()->GetGuestConcurrentUserCount(); // 체험ID 동접
		kInfo.m_iCurPartyCount		= 0;
		kInfo.m_iPlayGamePartyCount = 0;
		SiKServerCCUManager()->UpdateCCUInfo( kInfo );
		//}}
	}

	m_dwServerInfoUpdateTick = ::GetTickCount();
}
//}}

//{{ 2010. 10. 19	최육사	SMS테스트
//}}

void KLoginServer::ProcessEvent( const KEventPtr& spEvent_ )
{
    KSerializer ks;
    ks.BeginReading( &spEvent_->m_kbuff );

    DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_ACK, KDBE_LOGIN_SERVER_ON_ACK );
		CASE_NOPARAM( DBE_UPDATE_SERVER_INFO_ACK );
		CASE( EPUBLISHER_REG_USER_ACK );
		CASE( EPUBLISHER_UNREG_USER_ACK );

// 현재 사용하지 않는 코드로 보임
// #ifdef SERV_GUARANTEE_UNIQUENESS_OF_NAME_CN
// 		CASE( DBE_GET_GIANT_DELETED_UNIT_LIST_ACK );
// #endif //SERV_GUARANTEE_UNIQUENESS_OF_NAME_CN

		//{{ 2009. 7. 6  최육사		랭킹 개편
		CASE( DBE_GET_HENIR_RANKING_INFO_ACK );
	   _CASE( ELG_CLEAR_RANKING_NOT, bool );
		CASE_NOPARAM( DMP_DUMP_RANKING_MANAGER_NOT );
		CASE( ELG_ADMIN_NEW_RECORD_TEST_NOT );
		CASE( ELG_ADMIN_NEW_RECORD_NOT );
		CASE( ELG_ADMIN_DELETE_RECORD_NOT );
		//}}
		//{{ 2009. 11. 9  최육사	로그인서버GameDB
		CASE( DBE_GET_WEB_RANKING_INFO_ACK );
		//}}

		//{{ 2009. 11. 26  최육사	길드스킬
		CASE( DBE_GUILD_LEVEL_UP_ACK );
		//}}

		//{{ 2009. 11. 26  최육사	길드스킬
		CASE( DBE_INSERT_GUILD_CASH_SKILL_POINT_ACK );
		CASE( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_ACK );

	   _CASE( DBE_ADMIN_GET_GUILD_SKILL_POINT_ACK, KELG_ADMIN_GET_GUILD_SKILL_POINT_ACK );
		//}}

		//{{ 2010. 01. 15  최육사	길드게시판
		CASE( DBE_GET_GUILD_BOARD_INFO_ACK );
		CASE( DBE_ACCEPT_JOIN_GUILD_ACK );
		//}}

		//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
		//}}

		//{{ 2010. 06. 22  최육사	계정블럭기능
		CASE( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK );
		//}}

		//{{ 2010. 07. 19  최육사	실시간 스크립트 갱신
	   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT );
	   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
		//}}
		//{{ 2010. 10. 13	최육사	DLL Manager
		CASE( DBE_GET_MODULE_INFO_LIST_ACK );
	   _CASE( DBE_UPDATE_MODULE_INFO_LIST_ACK, KDBE_GET_MODULE_INFO_LIST_ACK );
		//}}
	   //{{ 2010. 10. 12	최육사	서버 모니터링
	   //}}
	   //{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
	   CASE( DBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_ACK );
	   //}}
	   //{{ 2011. 04. 18	최육사	대리 상인
	   CASE( DBE_INSERT_REWARD_TO_POST_ACK );
	   CASE( DBE_LOAD_PSHOP_AGENCY_ACK );
	  _CASE( DBE_BREAK_PSHOP_AGENCY_ACK, KERM_BREAK_PSHOP_AGENCY_NOT );
	  _CASE( ERM_PSHOP_AGENCY_MESSAGE_NOT, KEGS_PSHOP_AGENCY_MESSAGE_NOT );
	   //}}
		//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
		CASE( ELG_RANKING_TITLE_REWARD_NOT );
		CASE( DBE_INSERT_TITLE_ACK );
		//}}
		//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
		//}}
		//{{ 2013. 3. 4	박세훈	 로컬 랭킹 시스템
		//}}

		//{{ 2013. 04. 01	 인연 시스템 - 김민성
		CASE( DBE_LOAD_WEDDING_HALL_INFO_ACK );
		_CASE( ELG_ADD_WEDDING_HALL_INFO_NOT, KDBE_LOAD_WEDDING_HALL_INFO_ACK );
		//}


    default:
        START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
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

_IMPL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_LOGIN_SERVER_ON_ACK )
{
	//스타트에 실패했으면 프로그램을 끈다
	if( kPacket_.m_iOK != NetError::NET_OK)
	{
		::exit( 0 );
		return;
	}

	SetServerInfo( kPacket_.m_kServerInfo );

    SetEvent( m_spEventAck.get() );
}

IMPL_ON_FUNC_NOPARAM( DBE_UPDATE_SERVER_INFO_ACK )
{
	// 기능 없음.
}


//{{ 2009. 7. 6  최육사		랭킹 개편
IMPL_ON_FUNC( DBE_GET_HENIR_RANKING_INFO_ACK )
{
	// 헤니르의 시공 랭킹 업데이트
	SiKRankingManager()->InitHenirRanking( kPacket_.m_mapHenirRanking );
}

_IMPL_ON_FUNC( ELG_CLEAR_RANKING_NOT, bool )
{
	// 랭킹 초기화
	SiKRankingManager()->CheckRankingClearAndReward( true );

	START_LOG( cout, L"랭킹 수동 초기화 및 보상처리 완료!" );
}

IMPL_ON_FUNC_NOPARAM( DMP_DUMP_RANKING_MANAGER_NOT )
{
    SiKRankingManager()->DumpRankingInfo();
}

IMPL_ON_FUNC( ELG_ADMIN_NEW_RECORD_TEST_NOT )
{
	SiKRankingManager()->NewRecordForTest( kPacket_.m_vecNewRecordTest );
}

IMPL_ON_FUNC( ELG_ADMIN_NEW_RECORD_NOT )
{
	if( SiKRankingManager()->NewRecordHenirRanking( kPacket_.m_kNewRecordRanking, kPacket_.m_iRankingType ) )
	{
		START_LOG( cout, L"랭킹 기록 추가 성공!" );
	}
	else
	{
		START_LOG( cout, L"랭킹 기록 추가 실패!" );
	}
};

IMPL_ON_FUNC( ELG_ADMIN_DELETE_RECORD_NOT )
{
	if( SiKRankingManager()->DeleteRecord( kPacket_.m_iRankingType, kPacket_.m_wstrNickName ) )
	{
        START_LOG( cout, L"랭킹 기록 삭제 성공!" )
			<< BUILD_LOG( kPacket_.m_iRankingType )
			<< BUILD_LOG( kPacket_.m_wstrNickName );
	}
	else
	{
		START_LOG( cout, L"랭킹 기록 삭제 실패!" )
			<< BUILD_LOG( kPacket_.m_iRankingType )
			<< BUILD_LOG( kPacket_.m_wstrNickName );
	}
}
//}}

IMPL_ON_FUNC( DBE_GET_WEB_RANKING_INFO_ACK )
{
	// 던전, 대전 랭킹 업데이트
	SiKRankingManager()->UpdateDungeonRanking( kPacket_.m_vecDungeonRanking );
	SiKRankingManager()->UpdatePvpRanking( kPacket_.m_vecPvpRanking );

	// 게임서버로 브로드 캐스팅
	{
		KELG_WEB_RANKING_REFRESH_NOT kPacket;
		kPacket.m_bInit = true;
		kPacket.m_cRankingRefreshType = KELG_WEB_RANKING_REFRESH_NOT::DUNGEON_VECTOR;
		SiKRankingManager()->GetDungeonRankingVector( 0, kPacket );
		BroadCastAllGS( ELG_WEB_RANKING_REFRESH_NOT, kPacket );
	}

	{
		KELG_WEB_RANKING_REFRESH_NOT kPacket;
		kPacket.m_bInit = true;
		kPacket.m_cRankingRefreshType = KELG_WEB_RANKING_REFRESH_NOT::DUNGEON_MAP;
		SiKRankingManager()->GetDungeonRankingMap( 0, kPacket );
		BroadCastAllGS( ELG_WEB_RANKING_REFRESH_NOT, kPacket );
	}

	{
		KELG_WEB_RANKING_REFRESH_NOT kPacket;
		kPacket.m_bInit = true;
		kPacket.m_cRankingRefreshType = KELG_WEB_RANKING_REFRESH_NOT::PVP_VECTOR;
		SiKRankingManager()->GetPvpRankingVector( 0, kPacket );
		BroadCastAllGS( ELG_WEB_RANKING_REFRESH_NOT, kPacket );
	}

	{
		KELG_WEB_RANKING_REFRESH_NOT kPacket;
		kPacket.m_bInit = true;
		kPacket.m_cRankingRefreshType = KELG_WEB_RANKING_REFRESH_NOT::PVP_MAP;
		SiKRankingManager()->GetPvpRankingMap( 0, kPacket );
		BroadCastAllGS( ELG_WEB_RANKING_REFRESH_NOT, kPacket );
	}
}

//{{ 2009. 11. 26  최육사	길드스킬

IMPL_ON_FUNC( DBE_GUILD_LEVEL_UP_ACK )
{
	// 길드 레벨업 업데이트 결과 처리
	if( SiKGuildManager()->UpdateGuildLevelResult( kPacket_.m_iGuildUID, kPacket_ ) == false )
	{
		START_LOG( cerr, L"길드 레벨업 업데이트 결과 처리 실패" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOGc( kPacket_.m_ucGuildLevel )
			<< END_LOG;

		return;
	}

	//{{ 2010. 01. 25  최육사	길드 게시판

	// 길드 광고가 있는지 찾아보고 있다면 길드정보를 갱신하자!
	SiKGuildBoardManager()->UpdateGuildLevelResult( kPacket_.m_iGuildUID, kPacket_.m_ucGuildLevel );

	//}}

	// 길드 레벨 5일때, 10일때, 15일때 전체 공지 보내기
	switch( kPacket_.m_ucGuildLevel )
	{
	case 5:
	case 10:
	case 15:
		{
			KEGS_NOTIFY_MSG_NOT	kPacketNot;
			kPacketNot.m_cNotifyType = KEGS_NOTIFY_MSG_NOT::NT_GUILD_LEVEL_UP;
			kPacketNot.m_Count = 1;

			CStringW cwstrMessage;
			cwstrMessage.Format( L"%s;%d", kPacket_.m_wstrGuildName.c_str(), static_cast<int>(kPacket_.m_ucGuildLevel) );
			kPacketNot.m_wstrMSG = static_cast<LPCTSTR>(cwstrMessage);

			SendToGameServer( ELG_GUILD_LEVEL_UP_NOTIFY_NOT, kPacketNot );
		}
		break;
	}
}

//}}

//{{ 2009. 11. 26  최육사	길드스킬

IMPL_ON_FUNC( DBE_INSERT_GUILD_CASH_SKILL_POINT_ACK )
{
	// 길드 매니저에 결과 업데이트
	if( SiKGuildManager()->InsertGuildCashSkillPointResult( kPacket_ ) == false )
	{
		START_LOG( cerr, L"길드 캐쉬 스킬 포인트 업데이트 결과 처리 실패" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( DBE_EXPIRE_GUILD_CASH_SKILL_POINT_ACK )
{
	// 길드 매니저에 결과 업데이트
	if( SiKGuildManager()->ExpireGuildCashSkillPointResult( kPacket_ ) == false )
	{
		START_LOG( cerr, L"길드 캐쉬 스킬 포인트 기간종료 결과 처리 실패" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( DBE_ADMIN_GET_GUILD_SKILL_POINT_ACK, KELG_ADMIN_GET_GUILD_SKILL_POINT_ACK )
{
	// 치트 결과 업데이트
    if( SiKGuildManager()->AdminInsertGuildSkillPointResult( kPacket_ ) == false )
	{
		START_LOG( cerr, L"길드 스킬 포인트 치트 결과 업데이트 실패!" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< END_LOG;
	}
}

//}}

//{{ 2010. 01. 15  최육사	길드게시판

IMPL_ON_FUNC( DBE_GET_GUILD_BOARD_INFO_ACK )
{
	// DB로부터 받은 길드 게시판 정보 업데이트
	SiKGuildBoardManager()->UpdateGuildBoardInfo( kPacket_ );
}

IMPL_ON_FUNC( DBE_ACCEPT_JOIN_GUILD_ACK )
{
	// 예약된 정보를 지운다
	if( SiKGuildManager()->RemoveReserve( kPacket_.m_iGuildUID, kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID ) == false )
	{
		START_LOG( cerr, L"길드 가입 예약 정보 삭제 실패!" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID )
			<< END_LOG;
	}

	// 길드 가입 성공이 아니면 종료처리
	if( kPacket_.m_iOK != NetError::NET_OK )
		return;

	// 길드 가입 처리!
	if( SiKGuildManager()->JoinGuildMember( kPacket_.m_iGuildUID, kPacket_.m_kJoinGuildMemberInfo, false ) == false )
	{
		START_LOG( cerr, L"길드 멤버 로그인 정보 업데이트 실패!" )
			<< BUILD_LOG( kPacket_.m_iGuildUID )
			<< BUILD_LOG( kPacket_.m_kJoinGuildMemberInfo.m_iUnitUID )
			<< END_LOG;
	}

	// 길드 정보 얻기
	KEGS_GUILD_INFO_NOT kNot;
	SiKGuildManager()->GetGuildInfo( kPacket_.m_iGuildUID, kNot );

	// 현재 등록한 길드 광고가 있는지 확인
	if( SiKGuildBoardManager()->IsExistGuildAd( kPacket_.m_iGuildUID ) == true )
	{
		// 현재 인원 정보 업데이트
		SiKGuildBoardManager()->UpdateGuildCurMemberCount( kPacket_.m_iGuildUID, static_cast<u_short>(kNot.m_vecMemberUnitUID.size()) );
	}

	// 로그오프한 유저를 길드 가입하는것이므로 편지로 가입 사실을 알려주자!
	kPacket_.m_wstrGuildName = kNot.m_kGuildInfo.m_wstrGuildName;
	SendToGameServer( ELG_SEND_LETTER_JOIN_GUILD_NOT, kPacket_ );
}

//}}

//{{ 2010. 02. 23  최육사	웹 포인트 이벤트
//}}

//{{ 2010. 06. 22  최육사	계정블럭기능

IMPL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK )
{
	std::map< int, std::vector< UidType > > mapBlockUserList;
	std::map< int, std::vector< UidType > > mapUnblockUserList;
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	std::vector< std::string > vecBlockMachineIDList;
	SiKAccountBlockManager()->UpdateAccountBlockList( kPacket_, mapBlockUserList, mapUnblockUserList, vecBlockMachineIDList );
	//}}

	// 새로 추가된 차단해제 유저는 접속 여부를 확인하여 접속종료시킨다!
	{
		std::map< int, std::vector< UidType > >::const_iterator mit;
		for( mit = mapBlockUserList.begin(); mit != mapBlockUserList.end(); ++mit )
		{
			const int iBlockType = mit->first;
			const std::vector< UidType >& vecBlockList = mit->second;

			if( iBlockType < 0  ||  iBlockType >= KAccountBlockManager::BT_MAX )
				continue;

			switch( iBlockType )
			{
			case KAccountBlockManager::BT_CONNECT_BLOCK:
				{
					for( const UidType iUserUID : vecBlockList )
					{
						KUserList::KGSUserInfo kInfo;
						if( GetKLoginSimLayer()->GetGSUserInfoByUserUID( iUserUID, kInfo ) == false )
							continue;

						int iOK = NetError::ERR_ADMIN_COMMAND_07;
						SendToGSUser( kInfo.m_nGSUID, kInfo.m_nUserUID, E_KICK_USER_NOT, iOK );

						START_LOG( cout, L"접속 차단 대상자을 실시간으로 KICK합니다!" )
							<< BUILD_LOG( kInfo.m_nGSUID )
							<< BUILD_LOG( kInfo.m_nUserUID )
							<< BUILD_LOG( iUserUID );
					}
				}
				break;

			case KAccountBlockManager::BT_TRADE_BLOCK:
				{
					for( const UidType iUserUID : vecBlockList )
					{
						KUserList::KGSUserInfo kInfo;
						if( GetKLoginSimLayer()->GetGSUserInfoByUserUID( iUserUID, kInfo ) == false )
							continue;

						KELG_TRADE_BLOCK_NOT kNot;
						kNot.m_bIsTradeBlock = true;
						SendToGSUser( kInfo.m_nGSUID, kInfo.m_nUserUID, ELG_TRADE_BLOCK_NOT, kNot );

						START_LOG( cout, L"거래 차단 대상자을 실시간으로 적용합니다!" )
							<< BUILD_LOG( kInfo.m_nGSUID )
							<< BUILD_LOG( kInfo.m_nUserUID )
							<< BUILD_LOG( iUserUID );
					}
				}
				break;

			default:
				break;
			}
		}
	}

	// 삭제된 차단해제 유저는 접속 여부를 확인하여 실시간 해제 시킨다!
	{
		std::map< int, std::vector< UidType > >::const_iterator mit;
		for( mit = mapUnblockUserList.begin(); mit != mapUnblockUserList.end(); ++mit )
		{
			const int iBlockType = mit->first;
			const std::vector< UidType >& vecBlockList = mit->second;

			if( iBlockType < 0  ||  iBlockType >= KAccountBlockManager::BT_MAX )
				continue;

			switch( iBlockType )
			{
			case KAccountBlockManager::BT_CONNECT_BLOCK:
				break;

			case KAccountBlockManager::BT_TRADE_BLOCK:
				{
					for( const UidType iUserUID : vecBlockList )
					{
						KUserList::KGSUserInfo kInfo;
						if( GetKLoginSimLayer()->GetGSUserInfoByUserUID( iUserUID, kInfo ) == false )
							continue;

						KELG_TRADE_BLOCK_NOT kNot;
						kNot.m_bIsTradeBlock = false;
						SendToGSUser( kInfo.m_nGSUID, kInfo.m_nUserUID, ELG_TRADE_BLOCK_NOT, kNot );

						START_LOG( cout, L"거래 차단 대상자을 실시간으로 해제합니다!" )
							<< BUILD_LOG( kInfo.m_nGSUID )
							<< BUILD_LOG( kInfo.m_nUserUID )
							<< BUILD_LOG( iUserUID );
					}
				}
				break;

			default:
				break;
			}
		}
	}

	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	{
		// 접속중인 유저 접속 해제 시키기!
		for( const std::string& strMachineID : vecBlockMachineIDList )
		{
			KUserList::KGSUserInfo kInfo;
			if( GetKLoginSimLayer()->GetGSUserInfoByMachineID( strMachineID, kInfo ) == false )
				continue;

			int iOK = NetError::ERR_ADMIN_COMMAND_07;
			SendToGSUser( kInfo.m_nGSUID, kInfo.m_nUserUID, E_KICK_USER_NOT, iOK );

			START_LOG( cout, L"머신ID 차단 대상자을 실시간으로 차단합니다!" )
				<< BUILD_LOG( kInfo.m_nGSUID )
				<< BUILD_LOG( kInfo.m_nUserUID )
				<< BUILD_LOG( strMachineID );
		}
	}
	//}}
}

//}}

//{{ 2010. 07. 19  최육사	실시간 스크립트 갱신

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	// 로그인서버로 가는 명령인지 확인!
	if( KESR_SCRIPT_REFRESH_ORDER_NOT::OT_CN_SENTINEL < kPacket_.m_iOrderType  &&
		KESR_SCRIPT_REFRESH_ORDER_NOT::OT_LG_SENTINEL > kPacket_.m_iOrderType )
	{
		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_LOGIN_SERVER, NULL, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
		SiKLoginScriptRefreshManager()->QueueingEvent( spEvent );
	}
	else
	{
		START_LOG( cerr, L"이쪽으로 오면 안되는 갱신 요청 타입입니다!" )
			<< BUILD_LOG( kPacket_.m_iOrderType )
			<< END_LOG;
	}
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_LG_GUILD_MANAGER, CXSLGuildManager );
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_LG_GAME_SYS_VAL, KGameSysVal );
	//}}
	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_LG_TITLE_MANAGER, CXSLTitleManager );
	//}}

	//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_LG_LOG_MANAGER, KLogManager );
	//}}

	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_LG_SKILL_TREE, CXSLSkillTree );

	default:
		{
			START_LOG( cerr, L"이쪽으로 오면 안되는 타입인데?" )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		break;
	}
}

//}}

//{{ 2010. 10. 13	최육사	DLL Manager
IMPL_ON_FUNC( DBE_GET_MODULE_INFO_LIST_ACK )
{
	//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
	SiKDLLManager()->Init( kPacket_.m_vecModuleList );
	//}}

	START_LOG( cout, L"DLL 모듈 리스트 업데이트 완료!" )
		<< BUILD_LOG( kPacket_.m_vecModuleList.size() );
}

_IMPL_ON_FUNC( DBE_UPDATE_MODULE_INFO_LIST_ACK, KDBE_GET_MODULE_INFO_LIST_ACK )
{
	SiKDLLManager()->UpdateModuleList( kPacket_.m_vecModuleList );
}
//}}

//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
IMPL_ON_FUNC( DBE_GET_TIME_RANDOM_ITEM_EVENT_INFO_ACK )
{
    SiKRandomItemEventManager()->UpdateEventInfo( kPacket_.m_vecTimeOpenRandomItemInfo, kPacket_.m_setGetRewardUserUIDList );
}
//}}

//{{ 2011. 04. 18	최육사	대리 상인
IMPL_ON_FUNC( DBE_INSERT_REWARD_TO_POST_ACK )
{
	KUserList::KGSUserInfo kUserInfo;
	if( GetKLoginSimLayer()->GetGSUserInfoByUnitUID( kPacket_.m_iRewardLetter.m_iToUnitUID, kUserInfo ) == false )
		return;

	KELG_FIND_USER_SEND_LETTER_NOT kPacket;
	kPacket.m_kLetter = kPacket_.m_iRewardLetter;
	SendToGSUser( kUserInfo.m_nGSUID, kUserInfo.m_nUserUID, ELG_FIND_USER_SEND_LETTER_NOT, kPacket );
}

IMPL_ON_FUNC( DBE_LOAD_PSHOP_AGENCY_ACK )
{
	if( kPacket_.m_vecOpenPShopAgency.empty() )
	{
		START_LOG( cout, L"개설된 대리 상인이 없습니다!" )
			<< BUILD_LOG( kPacket_.m_vecOpenPShopAgency.size() );
		return;
	}

	int iOpenPShopAgencyCount = 0;

	for( const KERM_OPEN_PSHOP_AGENCY_BY_SERVER_NOT& kPacket : kPacket_.m_vecOpenPShopAgency )
	{
		if( kPacket.m_vecSellItemInfo.empty() )
		{
			START_LOG( clog, L"판매 아이템이 비어있습니다! 자동으로 상점개설을 하지 않습니다!" )
				<< BUILD_LOG( kPacket.m_iUnitUID );
			continue;
		}

		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_LOGIN_SERVER, NULL, ERM_OPEN_PSHOP_AGENCY_BY_SERVER_NOT, kPacket );
		//LIF( spEvent->m_kDestPerformer.AddUID( mit->first ) ); -- RoomUID는 이제 필요 없다!
		GetKLoginRoomManager()->QueueingEventToRoom( spEvent );

		++iOpenPShopAgencyCount;
	}

	START_LOG( cout, L"DB에 저장된 대리상인 자동 개설 완료!" )
		<< BUILD_LOG( iOpenPShopAgencyCount );
}

_IMPL_ON_FUNC( DBE_BREAK_PSHOP_AGENCY_ACK, KERM_BREAK_PSHOP_AGENCY_NOT )
{
	KUserList::KGSUserInfo kInfo;
	if( GetKLoginSimLayer()->GetGSUserInfoByUnitUID( kPacket_.m_iHostUID, kInfo ) == false )
		return;

	SendToGSUser( kInfo.m_nGSUID, kInfo.m_nUserUID, ERM_BREAK_PSHOP_AGENCY_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ERM_PSHOP_AGENCY_MESSAGE_NOT, KEGS_PSHOP_AGENCY_MESSAGE_NOT )
{
	KUserList::KGSUserInfo kInfo;
	if( GetKLoginSimLayer()->GetGSUserInfoByUnitUID( kPacket_.m_iHostUnitUID, kInfo ) == false )
		return;

	SendToGSUser( kInfo.m_nGSUID, kInfo.m_nUserUID, ELG_PSHOP_AGENCY_MESSAGE_NOT, kPacket_ );
}
//}}

//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
IMPL_ON_FUNC( ELG_RANKING_TITLE_REWARD_NOT )
{
	// 특정 서버만 업데이트 하도록 할것!
	START_LOG( cout, L"랭킹 매니저의 요청으로 랭킹 칭호 보상 지급을 시작합니다!" )
		<< BUILD_LOG( kPacket_.m_mapTitleReward.size() );

	// GameDB에 타이틀을 insert해준다.
	std::map< UidType, KRankingTitleReward >::const_iterator mit;
	for( mit = kPacket_.m_mapTitleReward.begin(); mit != kPacket_.m_mapTitleReward.end(); ++mit )
	{
		const KRankingTitleReward& kTitleReward = mit->second;
		std::wstring wstrMessage;

		for( const KTitleReward& kRewardInfo : kTitleReward.m_vecReward )
		{
			// 타이틀 보상 주기
			KDBE_INSERT_TITLE_REQ kPacket;
			kPacket.m_iUnitUID		   = kTitleReward.m_iUnitUID;
			kPacket.m_iTitleID		   = kRewardInfo.m_iTitleID;
			kPacket.m_sPeriod		   = kRewardInfo.m_sPeriod;
			SendToGameDB( DBE_INSERT_TITLE_REQ, kPacket );

			const CXSLTitleManager::TitleTemplet* pTitleInfo = SiCXSLTitleManager()->GetTitleTemplet( kRewardInfo.m_iTitleID );
			if( pTitleInfo == NULL )
			{
				START_LOG( cerr, L"존재하지 않는 칭호입니다!" )
					<< BUILD_LOG( kRewardInfo.m_iTitleID )
					<< END_LOG;

				continue;
			}
			//{{ 2011.11.20 임홍락	서버 단일화 // SERV_USE_NATION_FLAG 필수 포함임
			wstrMessage += pTitleInfo->m_wstrTitleName;
			wstrMessage += L",\n\n";
			//}}
		}

		// 타이틀 보상에 대해 편지로 알리기
		KDBE_INSERT_REWARD_TO_POST_REQ kPacketToDB;
		kPacketToDB.m_iFromUnitUID = kTitleReward.m_iUnitUID;
		kPacketToDB.m_iToUnitUID   = kTitleReward.m_iUnitUID;
		kPacketToDB.m_iRewardType  = KPostItemInfo::LT_RANKING_TITLE;
		kPacketToDB.m_iRewardID	   = 0; // 보상없음
		kPacketToDB.m_wstrMessage  = wstrMessage;
		SendToGameDB( DBE_INSERT_REWARD_TO_POST_REQ, kPacketToDB );
	}
}

IMPL_ON_FUNC( DBE_INSERT_TITLE_ACK )
{
	KUserList::KGSUserInfo kUserInfo;
	if( !GetKLoginSimLayer()->GetGSUserInfoByUnitUID( kPacket_.m_iUnitUID, kUserInfo ) )
		return;

	SendToGSUser( kUserInfo.m_nGSUID, kUserInfo.m_nUserUID, ELG_INSERT_TITLE_NOT, kPacket_ );
}
//}}

//{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
//}}


//{{ 2013. 04. 01	 인연 시스템 - 김민성
IMPL_ON_FUNC( DBE_LOAD_WEDDING_HALL_INFO_ACK )
{
	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"DB에 있는 결혼식 정보를 받아오기 실패!" )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< END_LOG;
		return;
	}

	SiKWeddingHallManager()->SetWeddingInfoDB( kPacket_.m_mapWeddingInfo );

	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		BroadCastAllGS( ELG_WEDDING_HALL_INFO_UPDATE_NOT, kPacket_ );
	}
}

_IMPL_ON_FUNC( ELG_ADD_WEDDING_HALL_INFO_NOT, KDBE_LOAD_WEDDING_HALL_INFO_ACK )
{
	SiKWeddingHallManager()->SetWeddingInfoDB( kPacket_.m_mapWeddingInfo );
	BroadCastAllGS( ELG_WEDDING_HALL_INFO_UPDATE_NOT, kPacket_ );
}
//}


IMPL_ON_FUNC( EPUBLISHER_REG_USER_ACK )
{
	// 국가별로 따로 RegUserAck 처리 하는게 없어서 함수로 따로 만들지는 않음 //
	KUserList::KGSUserInfo kInfo;

	if( GetKLoginSimLayer()->GetGSUserInfoByUserID( kPacket_.m_wstrServiceAccountID, kInfo ) )
	{
		//{{ 2012. 09. 03	최육사		중복 접속 버그 수정
		GetKLoginSimLayer()->UnRegAuthWaitUser( kInfo.m_nUserUID );
		//}}
		KELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK kPacketAck;
		kPacketAck.m_iOK = kPacket_.m_iOK;
		kPacketAck.m_wstrServiceAccountID = kPacket_.m_wstrServiceAccountID;
		START_LOG( clog, L"PC방 유저 체크." )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( kPacket_.m_wstrServiceAccountID )
			<< END_LOG;


		UidType anTrace[2] = { kInfo.m_nGSUID, -1 };
		KncSend( PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), PI_GS_USER, kInfo.m_nUserUID, anTrace, ELG_REGISTER_USER_GLOBAL_PUBLISHER_ACK, kPacketAck );
	}
	else
	{
		START_LOG( cerr, L"유저 게임 서버 검색 실패." )
			<< BUILD_LOG( kPacket_.m_iOK )
			<< BUILD_LOG( kPacket_.m_wstrServiceAccountID )
			<< END_LOG;
	}
}

IMPL_ON_FUNC( EPUBLISHER_UNREG_USER_ACK )
{
	// 국가별로 따로 UnRegUserAck 처리 하는게 없어서 함수로 따로 만들지는 않음 //
}
