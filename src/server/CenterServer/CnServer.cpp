#include <WinSock2.h>
#include <sstream>
#include <iomanip>
#include <boost/bind/bind.hpp>
#include "CnServer.h"
#include "ActorManager.h"
#include "CnSimLayer.h"
#include "CnNetLayer.h"
#include "Socket/NetCommon.h"
#include "CnRoomManager.h"
//{{ 2011. 12. 05	최육사	배틀필드
	#include "NewCnPartyManager.h"
//}}
#include "DBLayer.h"
#include "CnAccountDBThread.h"
//{{ 2012. 03. 28	최육사	개인거래 DDOS 방어
	#include "CnGameDBThread.h"
//}}
#include "CnLogDBThread.h"
#include "CnSMSDBThread.h"
#include "DefaultFSM.h"
#include "CnUser.h"

#include "NetError.h"

#include "GameEvent/GameEventManager.h"
#include "X2Data/XSLItemManager.h"
#include "AbuserLog/KAbuserLogManager.h"
//{{ 2009. 5. 23  최육사	시간드롭
#include "KDropTable.h"
//}}
//{{ 2009. 12. 21  최육사	서버군 확장
#include "Enum/Enum.h"
//}}
//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	#include "CnScriptRefreshManager.h"
//}}
//{{ 2010. 11. 15	최육사	던전 결과 처리 개편
	#include "GameEvent/GameEventScriptManager.h"
//}}
//{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현
	#include "DungeonEventManager.h"
//}}


//{{ 2011. 03. 29	최육사	TBB 메모리 관리자
//#ifdef SERV_TBB_MALLOC_PROXY_TEST
//	#include <tbb/tbbmalloc_proxy.h>
//#endif SERV_TBB_MALLOC_PROXY_TEST
//}}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}

//{{ 2013. 2. 5	박세훈	랜선렉 방지 코드2
	#include "InteriorUdpSession.h"
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

#include "../Common/OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h"
#include "GameSysVal/GameSysVal.h"

using namespace boost::placeholders;

#define CASE_SCRIPT_REFRESH_SWAP_INSTANCE( flag, className ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	className::SwapInstance( g_pLua ); \
	START_LOG( cout, L#className L" 스크립트 갱신을 완료하였습니다!" ); \
	Si##className()->DumpToLogFile(); \
	break; \

//#include "vld.h"

NiImplementRTTI( KCnServer, KBaseServer );
ImplPfID( KCnServer, PI_CN_SERVER );

#define CLASS_TYPE KCnServer

ImplToStringW( KCnServer )
{
    return START_TOSTRING_PARENTW( KBaseServer );
}

KCnServer::KCnServer(void)
{
}

KCnServer::~KCnServer(void)
{
	//{{ 2013. 2. 5	박세훈	랜선렉 방지 코드2
	KInteriorUdpSession::GetKObj()->ReleaseKObj();
	//}}
}

#include <lua_tinker.h>

void KCnServer::RegToLua()
{
    KBaseServer::RegToLua();

	lua_tinker::class_add<KCnServer>( g_pLua, "KCnServer" );
	lua_tinker::class_inh<KCnServer, KBaseServer>( g_pLua );
	lua_tinker::class_def<KCnServer>( g_pLua, "dump", &KCnServer::Dump );


	lua_tinker::decl( g_pLua, "Server", this );
}

const wchar_t*  g_szConfigFile = L"config_cn";
KBaseServer*    CreateKNC()                             { return KBaseServer::KeepObject<KCnServer>(); }
KThread*        CreateDBThread( int iDBConnectionInfo, const wchar_t* szDSN, bool bDBConnStr )
{
    switch( iDBConnectionInfo )
    {
    case KDBLayer::DC_ACCOUNT:
        return new KCnAccountDBThread( szDSN, bDBConnStr );
		//{{ 2012. 03. 28	최육사	개인거래 DDOS 방어
	case KDBLayer::DC_GAME:
		return new KCnGameDBThread( szDSN, bDBConnStr );
		//}}
    case KDBLayer::DC_LOG:
        return new KCnLogDBThread( szDSN, bDBConnStr );
	case KDBLayer::DC_SMS:
		return new KCnSMSDBThread( szDSN, bDBConnStr );

    default:
        START_LOG( cerr, L"접속하려는 DB 종류가 이상함." )
            << BUILD_LOG( iDBConnectionInfo )
            << END_LOG;
        return new KCnAccountDBThread( szDSN, bDBConnStr );
    }
}

bool KCnServer::Init()
{
    KSimLayer::KeepObject<KCnSimLayer>();
    KNetLayer::KeepObject<KCnNetLayer>();
    KActorManager::KeepObject<KActorManager>();
	//081020.hoons.
	KRoomManager::KeepObject<KCnRoomManager>();

    KNetLayer::GetKObj()->SetActorFactory( IActorFactoryPtr( new KActorFactory<KCnUser, KDefaultFSM, 512*1024> ) );
	//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
	KNetLayer::GetKObj()->SetSessionType( KSession::SST_SERVER );
	//}}

    _JIF( KBaseServer::Init(), return false );

	//{{ 2010. 02. 20  최육사	채널 파티 통합 - RoomManager Thread를 시작하기 전에 초기화 시켜야함.
	SiKCnPartyManager()->Init();
	//}}

	//081020.hoons. BaseServer::Init() 에 모든 루아파싱루트가 들어있기때문에 파싱후 init을 해야함.
	_JIF( GetKCnRoomManager()->Init(), return false );

	SetServerReady( true );

	//{{ 2009. 8. 27  최육사	server ready
	OnServerReadyComplete();
	//}}

	m_bServerRunningProcessCommunicationOnOff = false;

    return true;
}

void KCnServer::OnServerReadyComplete()
{
	KBaseServer::OnServerReadyComplete();

	//{{ 2009. 8. 27  최육사	어뷰저 로그
	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	SiKAbuserLogManager()->SendAbuserListReq();
	//}}
	//}}

	//{{ 2009. 5. 23  최육사	시간 드롭
	SendToLogDB( DBE_GET_TIME_DROP_INFO_REQ );
	//}}

	// 이벤트 정보 얻기
	SiKGameEventManager()->Init();

	//{{ 2010. 11. 15	최육사	던전 결과 처리 개편
		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		SiKGameEventScriptManager()->Init();
		//}}
	//}}

	//{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현
	SiKDungeonEventManager()->Init();
	//}}

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKCnScriptRefreshManager()->Init();
	//}}

	if (SiKGameSysVal()->GetProcessCommunication() == true)
	{
		SiKProcessCommunicationManager()->InitRead(TEXT("\\\\.\\pipe\\CenterServerRead"));
		SiKProcessCommunicationManager()->InitWrite(TEXT("\\\\.\\pipe\\CenterServerWrite"));
		SiKProcessCommunicationManager()->Begin();
	}

	// 서버군 이름 표시
	START_LOG( cout2, SEnum::GetServerGroupName( GetServerGroupID() ) )
		<< BUILD_LOG( GetServerGroupID() )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetName() );
}



void KCnServer::ShutDown()
{
	if (SiKGameSysVal()->GetProcessCommunication() == true)
	{
		SiKProcessCommunicationManager()->ShutDown();
	}

    // 아직 아무 종료처리도 진행되기 전.
    KBaseServer::ShutDown();        // 이곳에서 모든 종료가 끝난다. 각각의 Layer들도 모두 소멸됨.

	//{{ 2013. 2. 5	박세훈	랜선렉 방지 코드2
//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	KInteriorUdpSession::GetKObj()->ShutDown();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	//}}

    GetKCnRoomManager()->ShutDown();

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKCnScriptRefreshManager()->EndThread();
	KCnScriptRefreshManager::ReleaseInstance();
	//}}

	//{{ 2010. 03. 10  최육사	채널 통합 파티
	KCnPartyManager::ReleaseInstance();
	//}}
}

bool KCnServer::DestroyAllSession()
{
    // NetLayer의 Accepter만 닫힌 시점. 아직 모든 Layer들이 살아있다.

    KActorManager* pkActorManager = KActorManager::GetKObj();

    START_LOG( cout, L"현재 유저 : " << pkActorManager->GetCount() << L" 명" );

	//{{ 2009. 1. 30  최육사	서버 셧다운 종료 사유 설정
	int iIndex = KStatistics::eSIColDR_Server_Shutdown;

	pkActorManager->ForEach( boost::bind( &KActor::SetDisconnectReason, _1, iIndex ) );
	//}}

    pkActorManager->ForEach( boost::bind( &KActor::ReserveDestroy, _1 ) );

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

void KCnServer::Tick()
{
    KBaseServer::Tick();

    //if( ::GetTickCount() - m_dwServerInfoUpdateTick > ms_dwServerInfoUpdateGap )
    //{
    //    if( GetServerReady() )
    //    {
    //        WriteServerInfoToDB();
    //    }
    //}

	if ( m_tTimeProcessCommunicationONOFF.elapsed() > 60.0f )
	{
		if (SiKGameSysVal()->GetProcessCommunication() == true && m_bServerRunningProcessCommunicationOnOff == true)
		{
			SiKProcessCommunicationManager()->InitRead(TEXT("\\\\.\\pipe\\CenterServerRead"));
			SiKProcessCommunicationManager()->InitWrite(TEXT("\\\\.\\pipe\\CenterServerWrite"));
			SiKProcessCommunicationManager()->Begin();
			m_bServerRunningProcessCommunicationOnOff = false;
		}
		else if (SiKGameSysVal()->GetProcessCommunication() == false && m_bServerRunningProcessCommunicationOnOff == false)
		{
			SiKProcessCommunicationManager()->ShutDown();

			m_bServerRunningProcessCommunicationOnOff = true;
		}
		m_tTimeProcessCommunicationONOFF.restart();
	}
}

void KCnServer::ProcessEvent( const KEventPtr& spEvent_ )
{
    KSerializer ks;
    ks.BeginReading( &spEvent_->m_kbuff );

    DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_ACK, KDBE_CENTER_SERVER_ON_ACK );
       _CASE( DBE_UPDATE_SERVER_INFO_ACK, KServerList );
	   _CASE( ECN_NOTIFY_MSG_NOT, KEGS_NOTIFY_MSG_NOT );
		//{{ 2009. 4. 22  최육사	대전유저리스트
		CASE( ECN_NEW_PVP_USER_INFO_NOT );
		CASE( ECN_UPDATE_PVP_USER_INFO_NOT );
	   _CASE( ECN_CHECK_INVALID_PVP_USER_NOT, UidType );
		//}}
		//{{ 2009. 5. 23  최육사	시간드롭
		CASE( DBE_CHECK_TIME_DROP_RESULT_ACK );
		//}}
		//{{ 2009. 6. 1  최육사		채널이동
		CASE( ECN_CHECK_CHANNEL_CHANGE_REQ );
		CASE( ECN_CHECK_CHANNEL_CHANGE_ACK );
		//}}
		//{{ 2009. 6. 3  최육사		확성기
	   _CASE( ECN_MEGAPHONE_MESSAGE_NOT, KEGS_MEGAPHONE_MESSAGE_NOT );
		//}}
		//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	   _CASE( ESR_ORDER_TO_CENTER_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT );
		//}}
	   //{{ 2013. 02. 19	최육사	로그 시스템 개선
	   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
	   //}}

		CASE( DBE_CHECK_EVENT_UPDATE_ACK );

	   //{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
	   //}}
	   _CASE( ECN_ADD_COMMON_FLAG_NOT, KECN_ADD_COMMON_FLAG_NOT );
	   _CASE( ECN_DEL_COMMON_FLAG_NOT, KECN_DEL_COMMON_FLAG_NOT );

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

_IMPL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_CENTER_SERVER_ON_ACK )
{
	//스타트에 실패했으면 프로그램을 끈다
	if( kPacket_.m_iOK != NetError::NET_OK)
	{
		::exit( 0 );
		return;
	}

	SetServerInfo( kPacket_.m_kServerInfo );
	//{{ 2013. 2. 5	박세훈	랜선렉 방지 코드2
	KInteriorUdpSession::KeepObject<KInteriorUdpSession>();
	KInteriorUdpSession::GetKObj()->Init( KNetLayer::GetKObj()->GetNCUDPPort() );
	//}}
	SetProxyID( kPacket_.m_iProxyID );
	GetKCnRoomManager()->PrepareSquare( kPacket_.m_vecSquareList );

    SetEvent( m_spEventAck.get() );
}

_IMPL_ON_FUNC( DBE_UPDATE_SERVER_INFO_ACK, KServerList )
{
    SetServerList( kPacket_ );
}

_IMPL_ON_FUNC( ECN_NOTIFY_MSG_NOT, KEGS_NOTIFY_MSG_NOT )
{
	//{{ 2010. 05. 10  최육사	대전던전 서버군 통합
	if( kPacket_.m_cNotifyType == KEGS_NOTIFY_MSG_NOT::NT_GM_NOTIFY_ALL_SERVERS )
	{
		// 모든 게임서버로 브로드캐스팅
		BroadCastAllGS( ECN_NOTIFY_MSG_NOT, kPacket_ );
	}
	//{{ 2011. 07. 04    김민성    네이버 프로모션
	else if( kPacket_.m_cNotifyType == KEGS_NOTIFY_MSG_NOT::NT_RANDOM_GET_ITEM_ALL_NOTIFY )
	{
		// 모든 게임서버로 브로드캐스팅
		kPacket_.m_cNotifyType = KEGS_NOTIFY_MSG_NOT::NT_RANDOM_GET_ITEM;
		BroadCastAllGS( ECN_NOTIFY_MSG_NOT, kPacket_ );
	}
	//}}
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}
	else
	//}}
	{
		// 같은 서버군의 게임서버로 브로드캐스팅
		BroadCastSameGroupGS( ECN_NOTIFY_MSG_NOT, kPacket_ );
	}
}

//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신

_IMPL_ON_FUNC( ESR_ORDER_TO_CENTER_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	// 센터서버로 가는 명령인지 확인!
	if( KESR_SCRIPT_REFRESH_ORDER_NOT::OT_NONE < kPacket_.m_iOrderType  &&
		KESR_SCRIPT_REFRESH_ORDER_NOT::OT_CN_SENTINEL > kPacket_.m_iOrderType )
	{
		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_CN_SERVER, NULL, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
		SiKCnScriptRefreshManager()->QueueingEvent( spEvent );
	}
	// 로그인서버 또는 게임서버로 가는 명령인지 확인!
	else if( KESR_SCRIPT_REFRESH_ORDER_NOT::OT_CN_SENTINEL < kPacket_.m_iOrderType  &&
			 KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_SENTINEL > kPacket_.m_iOrderType )
	{
		BroadCastAllGS( ESR_ORDER_TO_GAME_SERVER_NOT, kPacket_ );
	}
	else
	{
		START_LOG( cerr, L"정의되지 않은 스크립트 갱신 요청 타입입니다!" )
			<< BUILD_LOG( kPacket_.m_iOrderType )
			<< END_LOG;
	}
}

//}}

//{{ 2013. 02. 19	최육사	로그 시스템 개선

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_LOG_MANAGER, KLogManager );

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

//{{ 2009. 4. 22  최육사	대전유저리스트
IMPL_ON_FUNC( ECN_NEW_PVP_USER_INFO_NOT )
{
	// 모든 게임서버로 브로드캐스팅
	BroadCastAllGS( ECN_NEW_PVP_USER_INFO_NOT, kPacket_ );
}

IMPL_ON_FUNC( ECN_UPDATE_PVP_USER_INFO_NOT )
{
	// 모든 게임서버로 브로드캐스팅
	BroadCastAllGS( ECN_UPDATE_PVP_USER_INFO_NOT, kPacket_ );
}

_IMPL_ON_FUNC( ECN_CHECK_INVALID_PVP_USER_NOT, UidType )
{
	// 모든 게임서버로 브로드캐스팅
	BroadCastAllGS( ECN_CHECK_INVALID_PVP_USER_NOT, kPacket_ );
}
//}}

//{{ 2009. 5. 23  최육사	시간드롭
IMPL_ON_FUNC( DBE_CHECK_TIME_DROP_RESULT_ACK )
{
	// 시간 드롭 아이템을 체크 실패했다면?
	if( kPacket_.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"특정 시각 드롭 이벤트 실패 했는데 여기로 왔네? 일어나서는 안되는 에러!" )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< BUILD_LOG( kPacket_.m_iGSUID )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< BUILD_LOG( kPacket_.m_iTimeDropID )
			<< BUILD_LOG( kPacket_.m_iRewardItemID )
			<< BUILD_LOG( kPacket_.m_wstrRewardItemName )
			<< BUILD_LOG( kPacket_.m_iDropItemUID )
			<< END_LOG;
		return;
	}

	// 유저에게 보상처리한다!
	KEGS_GET_ITEM_NOT kDropNot;
	kDropNot.m_cGetItemType = KEGS_GET_ITEM_NOT::GIT_DROP_EVENT_ITEM;
	kDropNot.m_GetUnitUID	= kPacket_.m_iUnitUID;
	kDropNot.m_iItemID		= kPacket_.m_iRewardItemID;
	kDropNot.m_iDropItemUID = kPacket_.m_iDropItemUID;
	SendToGSCharacter( kPacket_.m_iGSUID, kPacket_.m_iUnitUID, ERM_GET_ITEM_INSERT_TO_INVENTORY_NOT, kDropNot );

	// 전체 공지 날린다!
	KEGS_NOTIFY_MSG_NOT kNotMsg;
	kNotMsg.m_cNotifyType = KEGS_NOTIFY_MSG_NOT::NT_DROP_GET_ITEM;
	kNotMsg.m_Count = 1;
	kNotMsg.m_wstrMSG = kPacket_.m_wstrNickName;
	kNotMsg.m_wstrMSG += L";";
	kNotMsg.m_wstrMSG += kPacket_.m_wstrRewardItemName;
	//{{ 2010. 05. 10  최육사	대전던전 서버군 통합
	BroadCastSameGroupGS( ECN_NOTIFY_MSG_NOT, kNotMsg );
	//}}
}
//}}

//{{ 2009. 6. 1  최육사		채널이동
IMPL_ON_FUNC( ECN_CHECK_CHANNEL_CHANGE_REQ )
{
	// 해당 게임서버로 전송
	SendToGSServer( kPacket_.m_iDestGSUID, ECN_CHECK_CHANNEL_CHANGE_REQ, kPacket_ );
}

IMPL_ON_FUNC( ECN_CHECK_CHANNEL_CHANGE_ACK )
{
	// 응답을 접속중인 게임서버로 전송
	SendToGSCharacter( kPacket_.m_iSourGSUID, kPacket_.m_iCharUID, ECN_CHECK_CHANNEL_CHANGE_ACK, kPacket_ );
}
//}}

//{{ 2009. 6. 3  최육사		확성기
_IMPL_ON_FUNC( ECN_MEGAPHONE_MESSAGE_NOT, KEGS_MEGAPHONE_MESSAGE_NOT )
{
	// 전체 게임서버로 브로드캐스팅
	//{{ 2010. 05. 10  최육사	대전던전 서버군 통합
	// 같은 서버군에게만 전송!
	BroadCastSameGroupGS( ECN_MEGAPHONE_MESSAGE_NOT, kPacket_ );
	//}}
}
//}}

//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
//}}

//{{ 2012. 09. 02	박세훈	Merge ( 대전에서 클라이언트 조작등으로 UDP 패킷이 날아오지 않는 유저가 발견되면 서버에서 킥한다. // 2012.06.11 lygan_조성욱 )
//}}

_IMPL_ON_FUNC( ECN_ADD_COMMON_FLAG_NOT, KECN_ADD_COMMON_FLAG_NOT )
{
	KEGS_ADD_COMMON_FLAG_NOT kPacket;
	kPacket.dwFlag = kPacket_.dwFlag;
	BroadCastAllGS( EGS_ADD_COMMON_FLAG_NOT, kPacket );
}
_IMPL_ON_FUNC( ECN_DEL_COMMON_FLAG_NOT, KECN_DEL_COMMON_FLAG_NOT )
{
	KEGS_DEL_COMMON_FLAG_NOT kPacket;
	kPacket.dwFlag = kPacket_.dwFlag;
	BroadCastAllGS( EGS_DEL_COMMON_FLAG_NOT, kPacket );
}

IMPL_ON_FUNC( DBE_CHECK_EVENT_UPDATE_ACK )
{
	std::map<int , int>::const_iterator cmit = kPacket_.m_mapReleaseTick.find( KGameEventManager::ERTT_EVENT_CHECK );

	if( cmit !=  kPacket_.m_mapReleaseTick.end() )
	{
		int iOldReleaseTick = SiKGameEventManager()->GetEventReleaseTick();
		int iNewReleaseTick = cmit->second;
		if( iOldReleaseTick != iNewReleaseTick )
		{
			START_LOG( cout2, L"이벤트 바뀐 것을 확인하였으므로 새로 받아옵니다." )
				<< BUILD_LOG( iOldReleaseTick )
				<< BUILD_LOG( iNewReleaseTick )
				<< END_LOG;

			SendToLogDB( DBE_EVENT_UPDATE_REQ );


			SiKGameEventManager()->SetEventReleaseTick( iNewReleaseTick );
		}
	}
}
