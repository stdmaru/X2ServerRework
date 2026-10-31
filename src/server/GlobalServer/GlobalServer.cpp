#include <WinSock2.h>
#include <sstream>
#include <iomanip>
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;
#include "GlobalServer.h"
#include "ActorManager.h"
#include "GlobalSimLayer.h"
#include "GlobalNetLayer.h"
#include "Socket/NetCommon.h"
#include "DBLayer.h"

#include "GlobalLogDBThread.h"
//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
//{{ 2012. 10. 31	최육사	배틀필드 시스템
	#include "GlobalGameDBThread.h"
//}}
#include "GlobalAccountDBThread.h"
#include "DefaultFSM.h"
#include "GlobalUser.h"

//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	#include "GlobalScriptRefreshManager.h"
//}}

//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

#include "NetError.h"

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
#include "WorldMissionManager.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가

// 동접 관리
#include "GlobalServerCCUManager.h"

//{{ 2011. 07. 04	최육사	대전 개편
	#include "MatchMakingManager.h"
//}}
//{{ 2012. 02. 09	최육사	배틀필드 시스템
	#include "AutoPartyMakingManager.h"
//}}

//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	#include "X2Data/XSLQuestManager.h"
//}}

//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
//}}

//{{ 2012. 12. 17	박세훈	잭팟 이벤트
//}}

#include "GameEvent/GameEventManager.h"

#include "../Common/OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h"
#include "GameSysVal/GameSysVal.h"

//{{ 2013. 05. 14	최육사	제재 리스트 통합
	#include "BlockListManager.h"
//}}


//{{ 2012. 09. 06	임홍락	글로벌 미션 매니저
#include "GlobalMissionManager.h"
//}} 2012. 09. 06	임홍락	글로벌 미션 매니저

//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

	#include "X2Data/XSLBattleFieldManager.h"
	#include "X2Data/XSLFieldBossData.h"
	#include "FieldBossManager.h"

//#include "vld.h"

NiImplementRTTI( KGlobalServer, KBaseServer );
ImplPfID( KGlobalServer, PI_GLOBAL_SERVER );

#define CLASS_TYPE KGlobalServer

ImplToStringW( KGlobalServer )
{
	return START_TOSTRING_PARENTW( KBaseServer );
}

KGlobalServer::KGlobalServer(void)
: m_iTimeEnchantEventUID( 0 )

, m_iExchangeLimitUID( 0 )
{
	m_kTimer.restart();
	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	m_kTimerRandomQuest.restart();
	//}}
	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}
}

KGlobalServer::~KGlobalServer(void)
{
}

#include <lua_tinker.h>

void KGlobalServer::RegToLua()
{
	KBaseServer::RegToLua();

	lua_tinker::class_add<KGlobalServer>( g_pLua, "KGlobalServer" );
	lua_tinker::class_inh<KGlobalServer, KBaseServer>( g_pLua );

	lua_tinker::class_def<KGlobalServer>( g_pLua, "dump",				&KGlobalServer::Dump );
	//{{ 2010. 10. 19	최육사	SMS테스트
	//}}

	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}

	lua_tinker::decl( g_pLua, "Server", this );
}

const wchar_t*  g_szConfigFile = L"config_gb";
KBaseServer*    CreateKNC()                             { return KBaseServer::KeepObject<KGlobalServer>(); }
KThread*        CreateDBThread( int iDBConnectionInfo, const wchar_t* szDSN, bool bDBConnStr )
{
	switch( iDBConnectionInfo )
	{
	case KDBLayer::DC_ACCOUNT:
		return new KGlobalAccountDBThread( szDSN, bDBConnStr );
		//{{ 2012. 10. 31	최육사	배틀필드 시스템
	case KDBLayer::DC_GAME:
		return new KGlobalGameDBThread( szDSN, bDBConnStr );
		//}}
	case KDBLayer::DC_LOG:
		return new KGlobalLogDBThread( szDSN, bDBConnStr );
		//{{ 2010. 10. 12	최육사	서버 모니터링
		//}}
	default:
		START_LOG( cerr, L"접속하려는 DB 종류가 이상함." )
			<< BUILD_LOG( iDBConnectionInfo )
			<< END_LOG;
		return new KGlobalLogDBThread( szDSN, bDBConnStr );
	}
}

bool KGlobalServer::Init()
{
	KSimLayer::KeepObject<KGlobalSimLayer>();
	KNetLayer::KeepObject<KGlobalNetLayer>();
	KActorManager::KeepObject<KActorManager>();

	KNetLayer::GetKObj()->SetActorFactory( IActorFactoryPtr( new KActorFactory<KGlobalUser, KDefaultFSM, 512*1024> ) );
	//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
	KNetLayer::GetKObj()->SetSessionType( KSession::SST_SERVER );
	//}}

	_JIF( KBaseServer::Init(), return false );

	//WriteServerInfoToDB();

	//{{ 2011. 07. 04	최육사	대전 개편
	SiKMatchMakingManager()->Init();
	//}}
	//{{ 2012. 02. 09	최육사	배틀필드 시스템
	SiKAutoPartyMakingManager()->Init();
	//}}

	SetServerReady( true );

	//{{ 2009. 8. 27  최육사	server ready
	OnServerReadyComplete();
	//}}

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	SiCXSLQuestManager()->SelectTodayRandomQuest( true );
	//}}
	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	m_bServerRunningProcessCommunicationOnOff = false;

	return true;
}

void KGlobalServer::OnServerReadyComplete()
{
	SendToLogDB( DBE_TIME_ENCHANT_EVENT_INFO_REQ );

	SendToLogDB( DBE_EXCHANGE_LIMIT_INFO_REQ );

	KBaseServer::OnServerReadyComplete();

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKGlobalScriptRefreshManager()->Init();
	//}}

	// 이벤트 정보 얻기
	SiKGameEventManager()->Init();

	if (SiKGameSysVal()->GetProcessCommunication() == true)
	{
		SiKProcessCommunicationManager()->InitRead(TEXT("\\\\.\\pipe\\GlobalServerRead"));
		SiKProcessCommunicationManager()->InitWrite(TEXT("\\\\.\\pipe\\GlobalServerWrite"));
		SiKProcessCommunicationManager()->Begin();
	}
	//{{ 2013. 05. 14	최육사	제재 리스트 통합
	SiKBlockListManager()->Init();
	//}}

	// 서버군 이름 표시
	START_LOG( cout2, SEnum::GetServerGroupName( GetServerGroupID() ) )
		<< BUILD_LOG( GetServerGroupID() )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetName() );
}

void KGlobalServer::ShutDown()
{
	//{{ 2011. 07. 04	최육사	대전 개편
	SiKMatchMakingManager()->ShutDown();
	//}}
	//{{ 2012. 02. 09	최육사	배틀필드 시스템
	SiKAutoPartyMakingManager()->ShutDown();
	//}}

	//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
	SiKGlobalScriptRefreshManager()->EndThread();
	KGlobalScriptRefreshManager::ReleaseInstance();
	//}}

	if (SiKGameSysVal()->GetProcessCommunication() == true)
	{
		SiKProcessCommunicationManager()->ShutDown();
	}

	// 아직 아무 종료처리도 진행되기 전.
	KBaseServer::ShutDown();        // 이곳에서 모든 종료가 끝난다. 각각의 Layer들도 모두 소멸됨.
}

bool KGlobalServer::DestroyAllSession()
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

void KGlobalServer::Tick()
{
	// 현재 시간 기록
	const CTime tCurrentTime = CTime::GetCurrentTime();

	KBaseServer::Tick();

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	// 월드 미션이 진행중이면
	// 1분 마다 수정의 수를 DB에 기록한다
	if( m_kTimer.elapsed() >= 60.0 )
	{
		SiKWorldMissionManager()->WriteWorldMissionCrystalCount();
		m_kTimer.restart();
	}
	//}} 2011. 04. 13  김민성  글로벌 서버 추가

	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	if( m_kTimerRandomQuest.elapsed() >= 30.0 )
	{
		if( SiCXSLQuestManager()->SelectTodayRandomQuest( false ) == true )
		{
			KEGB_TODAY_RANDOM_QUEST_ACK kAck;

			SiCXSLQuestManager()->GetRandomQuestGroupList( kAck.m_mapTodayRandomQuest );

			BroadCastAllGS( EGB_TODAY_RANDOM_QUEST_ACK, kAck );
		}
		m_kTimerRandomQuest.restart();
	}
	//}}

	// 게임 이벤트
	SiKGameEventManager()->Tick();

	//{{ 2011. 09. 29  김민성	셧다운제 도입
	//}}

	//{{ 공존의 축제 - 지진 효과 - 김민성
	//}}

	//{{ 2012. 12. 17	박세훈	잭팟 이벤트
	//}}

	CheckHeroPvpUserList();

	if ( m_tTimeProcessCommunicationONOFF.elapsed() > 60.0f )
	{
		if (SiKGameSysVal()->GetProcessCommunication() == true && m_bServerRunningProcessCommunicationOnOff == true)
		{
			SiKProcessCommunicationManager()->InitRead(TEXT("\\\\.\\pipe\\GlobalServerRead"));
			SiKProcessCommunicationManager()->InitWrite(TEXT("\\\\.\\pipe\\GlobalServerWrite"));
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

	CheckTimeEnchantEvent();

	CheckExchangeLimit();

	// 글로벌 미션 정보들을 1분 마다 DB에 기록
	if( m_kTimerGlobalMission.elapsed() >= 60.0 )
	{
		SiKGlobalMissionManager()->WriteGlobalMissionInfo();
		m_kTimerGlobalMission.restart();
	}

	CheckFieldBossSystem( tCurrentTime );
}

//{{ 2010. 10. 19	최육사	SMS테스트
//}}

void KGlobalServer::ProcessEvent( const KEventPtr& spEvent_ )
{
	KSerializer ks;
	ks.BeginReading( &spEvent_->m_kbuff );

	DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

	switch( spEvent_->m_usEventID )
	{
	   _CASE( DBE_SERVER_ON_ACK, KDBE_LOGIN_SERVER_ON_ACK );
		CASE_NOPARAM( DBE_UPDATE_SERVER_INFO_ACK );

		//{{ 2010. 07. 19  최육사	실시간 스크립트 갱신
	   _CASE( ESR_ORDER_TO_GLOBAL_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT );
	   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
		//}}

		//{{ 2010. 10. 12	최육사	서버 모니터링
		//}}

		//{{ 2011. 04. 13  김민성  글로벌 서버 추가
		CASE( DBE_GET_DEFENCE_CRYSTAL_ACK );
		//}} 2011. 04. 13  김민성  글로벌 서버 추가

		//{{ 2012. 04. 25	박세훈	어둠의 문 개편
		_CASE( EGB_INCREASE_THE_PROTECTED_CRYSTAL_COUNT_NOT, int );
		CASE_NOPARAM( EGB_RESET_INCREASE_THE_PROTECTED_CRYSTAL_COUNT_NOT );
		//}}
		//{{ 2012. 12. 17	박세훈	잭팟 이벤트
		//}}


		CASE( DBE_CHECK_EVENT_UPDATE_ACK );

		CASE( DBE_EVENT_UPDATE_ACK );

		//{{ 2013. 05. 14	최육사	제재 리스트 통합
		CASE( DBE_CHECK_BLOCK_LIST_ACK );
		//}}

		CASE( DBE_TIME_ENCHANT_EVENT_INFO_ACK );
		CASE( DBE_TIME_ENCHANT_EVENT_INFO_NOT );

		CASE( DBE_EXCHANGE_LIMIT_INFO_ACK );
		CASE( DBE_EXCHANGE_LIMIT_INFO_NOT );

		CASE( DBE_GET_GLOBAL_MISSION_INFO_ACK );

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

//{{ 2010. 07. 19  최육사	실시간 스크립트 갱신

_IMPL_ON_FUNC( ESR_ORDER_TO_GLOBAL_SERVER_NOT, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	// 글로벌서버로 가는 명령인지 확인!
	if( KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GS_SENTINEL < kPacket_.m_iOrderType  &&
		KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GB_SENTINEL > kPacket_.m_iOrderType )
	{
		KEventPtr spEvent( new KEvent );
		spEvent->SetData( PI_GLOBAL_SERVER, NULL, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
		SiKGlobalScriptRefreshManager()->QueueingEvent( spEvent );
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
	// 별도의 Thread에서 Refresh를 해야만 하는것은 여기서 처리하자!
	switch( kPacket_.m_iOrderType )
	{
		//{{ 2011. 07. 23	최육사	대전 개편
	case KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GB_PVP_MATCH_MANAGER:
		{
			KEventPtr spEvent( new KEvent );
			spEvent->SetData( PI_GLOBAL_SERVER, NULL, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
			SiKMatchMakingManager()->QueueingEventToMatch( spEvent );
		}
		return;
		//}}
		//{{ 2012. 11. 9	최육사	배틀필드 시스템
	case KESR_SCRIPT_REFRESH_ORDER_NOT::OT_GB_AUTO_PARTY_SCRIPT_MANAGER:
		{
			KEventPtr spEvent( new KEvent );
			spEvent->SetData( PI_GLOBAL_SERVER, NULL, ESR_ORDER_TO_REFRESH_MANAGER_REQ, kPacket_ );
			SiKAutoPartyMakingManager()->QueueingEvent( spEvent );
		}
		return;
		//}}
	}

	switch( kPacket_.m_iOrderType )
	{
		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GB_WORLD_MISSION_MANAGER, KWorldMissionManager );
		//{{ 2011. 11. 04	김민성 일일 랜덤 퀘스트 실시간 패치
		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GB_QUEST_MANAGER, CXSLQuestManager );
		//}}
		//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
		//}}
		//{{ 2013. 02. 19	최육사	로그 시스템 개선
		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GB_LOG_MANAGER, KLogManager );
		//}}

		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GB_BATTLE_FIELD_MANAGER, CXSLBattleFieldManager );
		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_GB_FIELD_BOSS_DATA, CXSLFieldBossData );

	default:
		{
			START_LOG( cerr, L"이쪽으로 오면 안되는 타입인데?" )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		break;
	}

	SiKProcessCommunicationManager()->QueueingProcessWrite(boost::str(boost::wformat(L"%1%_%2%") % 0 %L"GlobalServer 스크립트 실시간 패치 완료"));
}

//}}

//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
IMPL_ON_FUNC( DBE_GET_DEFENCE_CRYSTAL_ACK )
{
	SiKWorldMissionManager()->SetInitMissionInfo( kPacket_ );
}
//}} 2011. 04. 13  김민성  글로벌 서버 추가

//{{ 2011. 09. 29  김민성	셧다운제 도입
//}}

//{{ 2012. 04. 25	박세훈	어둠의 문 개편
_IMPL_ON_FUNC( EGB_INCREASE_THE_PROTECTED_CRYSTAL_COUNT_NOT, int )
{
	START_LOG( cout, L"WorldMissionManager 보호된 수정의 개수 증가 처리 시작.");
	SiKWorldMissionManager()->IncreaseProtectedCrystalCount( kPacket_ );
	START_LOG( cout, L"WorldMissionManager 보호된 수정의 개수 증가 처리 완료.");
}

IMPL_ON_FUNC_NOPARAM( EGB_RESET_INCREASE_THE_PROTECTED_CRYSTAL_COUNT_NOT )
{
	START_LOG( cout, L"WorldMissionManager 보호된 수정의 개수 초기화 처리 시작.");
	SiKWorldMissionManager()->ResetProtectedCrystalCount();
	START_LOG( cout, L"WorldMissionManager 보호된 수정의 개수 초기화 처리 완료.");
}
//}}

//{{ 공존의 축제 - 지진 효과 - 김민성
//}}

//{{ 2012. 12. 17	박세훈	잭팟 이벤트
//}}

void KGlobalServer::CheckHeroPvpUserList( void )
{
	KEGS_ADMIN_HERO_PVP_USER_CONNECTION_NOT kPacket;

	// GetHeroPvpUserChangeInfo 는 데이터를 꺼낸 뒤에 비우는 함수다.
	GetKGlobalSimLayer()->GetHeroPvpUserChangeInfo( kPacket.m_vecUserList );
	if( kPacket.m_vecUserList.empty() == true )
	{
		return;
	}

	std::vector< std::pair< UidType, UidType > > vecHeroPvpObserverList;
	GetKGlobalSimLayer()->GetHeroPvpObserverList( vecHeroPvpObserverList );
	if( vecHeroPvpObserverList.empty() == true )
	{
		return;
	}

	std::vector< std::pair< UidType, UidType > >::iterator it;
	for( it = vecHeroPvpObserverList.begin(); it != vecHeroPvpObserverList.end(); ++it )
	{
		SendToGSCharacter( it->first, it->second, EGB_ADMIN_HERO_PVP_USER_CONNECTION_NOT, kPacket );
	}
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

IMPL_ON_FUNC( DBE_EVENT_UPDATE_ACK )
{
	if( kPacket_.m_vecEventList.size() > 0 )
	{
		SiKGameEventManager()->SetEvent( kPacket_.m_vecEventList );
	}
}

//{{ 2013. 05. 14	최육사	제재 리스트 통합
IMPL_ON_FUNC( DBE_CHECK_BLOCK_LIST_ACK )
{
	std::vector< KBlockInfo > vecNewBlock;
	std::vector< KBlockInfo > vecDelBlock;

	// DB에서 가져온 블럭 리스트를 업데이트하자.
	SiKBlockListManager()->UpdateblockList( kPacket_.m_iReleaseTick, kPacket_.m_mapBlockInfo, vecNewBlock, vecDelBlock );

	// 새로 실행되는 블럭은 해당 유저를 찾아서 블럭시키자!
	BOOST_TEST_FOREACH( const KBlockInfo&, kInfo, vecNewBlock )
	{
		KGlobalUserList::KGSUserInfo kGSUserInfo;
		if( GetKGlobalSimLayer()->GetGSUserInfoByUserUID( kInfo.m_iBlockTarget, kGSUserInfo ) == true )
		{
			KEGB_UPDATE_BLOCK_RESULT_NOT kPacketNot;
			kPacketNot.m_mapBlockResult.insert( std::make_pair( kInfo.m_cBlockType, true ) );
            SendToGSUser( kGSUserInfo.m_nGSUID, kGSUserInfo.m_nUserUID, EGB_UPDATE_BLOCK_RESULT_NOT, kPacketNot );
		}
	}

	// 삭제되는 블럭은 해당 유저를 찾아서 해제 시키자!
	BOOST_TEST_FOREACH( const KBlockInfo&, kInfo, vecDelBlock )
	{
		if( kInfo.m_cBlockType != KBlockInfo::BT_TRADE_BLOCK )
			continue;

		KGlobalUserList::KGSUserInfo kGSUserInfo;
		if( GetKGlobalSimLayer()->GetGSUserInfoByUserUID( kInfo.m_iBlockTarget, kGSUserInfo ) == true )
		{
			KEGB_UPDATE_BLOCK_RESULT_NOT kPacketNot;
			kPacketNot.m_mapBlockResult.insert( std::make_pair( KBlockInfo::BT_TRADE_BLOCK, false ) );
			SendToGSUser( kGSUserInfo.m_nGSUID, kGSUserInfo.m_nUserUID, EGB_UPDATE_BLOCK_RESULT_NOT, kPacketNot );
		}
	}
}
//}}

bool KGlobalServer::RetrieveTimeEnchantEventData( OUT KTimeEnchantEventInfo& kInfo, OUT int& iTimeEnchantEventUID )
{
	if( m_mapBeginTimeEnchantEventInfo.empty() == true )
	{
		return false;
	}

	std::map<int, KTimeEnchantEventInfo>::iterator it = m_mapBeginTimeEnchantEventInfo.begin();

	kInfo					= it->second;
	iTimeEnchantEventUID	= m_iTimeEnchantEventUID;

	// 바로 제거한다.
	m_mapBeginTimeEnchantEventInfo.erase( it );
	m_setEndTimeEnchantEventInfo.insert( it->first );
	return true;
}

void KGlobalServer::CheckTimeEnchantEvent( void )
{
	if( m_kTimeEnchantEventTimer.elapsed() < 10.0f )
	{
		return;
	}
	m_kTimeEnchantEventTimer.restart();

	__time64_t tCurrentTime = CTime::GetCurrentTime().GetTime();

	bool bChanged = ( m_setEndTimeEnchantEventInfo.empty() == false );
	std::map<int, KTimeEnchantEventInfo>::iterator it;

	// 종료된 이벤트 처리
	BOOST_TEST_FOREACH( const int, iEventUID, m_setEndTimeEnchantEventInfo )
	{
		m_mapBeginTimeEnchantEventInfo.erase( iEventUID );
	}
	m_setEndTimeEnchantEventInfo.clear();

	for( it = m_mapBeginTimeEnchantEventInfo.begin(); it != m_mapBeginTimeEnchantEventInfo.end(); )
	{
		if( it->second.m_tEndDate <= tCurrentTime )
		{
			it = m_mapBeginTimeEnchantEventInfo.erase( it );
			bChanged = true;
			continue;
		}

		++it;
	}

	// 시작/종료된 이벤트 처리
	for( it = m_mapTimeEnchantEventInfo.begin(); it != m_mapTimeEnchantEventInfo.end(); )
	{
		if( it->second.m_tEndDate <= tCurrentTime )
		{
			it = m_mapTimeEnchantEventInfo.erase( it );
			continue;
		}
		else if( it->second.m_tBeginDate <= tCurrentTime )
		{
			m_mapBeginTimeEnchantEventInfo.insert( std::map<int, KTimeEnchantEventInfo>::value_type( it->first, it->second ) );
			it = m_mapTimeEnchantEventInfo.erase( it );
			bChanged = true;
			continue;
		}

		++it;
	}

	if( bChanged == true )
	{
		KEGB_TIME_ENCHANT_EVENT_INFO_NOT kPacket;
		GetTimeEnchantEventUID( kPacket.m_iTimeEnchantEventUID );
		GetNumOfBeginTimeEnchantEventInfo( kPacket.m_iNumOfTimeEnchantEvent );
		BroadCastAllGS( EGB_TIME_ENCHANT_EVENT_INFO_NOT, kPacket );
	}
}

IMPL_ON_FUNC( DBE_TIME_ENCHANT_EVENT_INFO_ACK )
{
	if( ++m_iTimeEnchantEventUID == 0 )
	{
		++m_iTimeEnchantEventUID;
	}

	m_mapTimeEnchantEventInfo.clear();
	m_mapBeginTimeEnchantEventInfo.clear();
	m_setEndTimeEnchantEventInfo.clear();

	BOOST_TEST_FOREACH( const KTimeEnchantEventInfo&, kInfo, kPacket_.m_vecTimeEnchantEventInfo )
	{
		m_mapTimeEnchantEventInfo.insert( std::map<int, KTimeEnchantEventInfo>::value_type( kInfo.m_iEventUID, kInfo ) );
	}

	START_LOG( cout, L"특정 시각 강화 부여 이벤트 설정" )
		<< BUILD_LOG( m_iTimeEnchantEventUID )
		<< BUILD_LOG( m_mapTimeEnchantEventInfo.size() )
		<< END_LOG;
}

IMPL_ON_FUNC( DBE_TIME_ENCHANT_EVENT_INFO_NOT )
{
	if( CheckTimeEnchantEventUID( kPacket_.m_iTimeEnchantEventUID ) == false )
	{
		return;
	}

	m_mapTimeEnchantEventInfo.insert( std::map<int, KTimeEnchantEventInfo>::value_type( kPacket_.m_kTimeEnchantEventInfo.m_iEventUID, kPacket_.m_kTimeEnchantEventInfo ) );
}

void KGlobalServer::GetBeginExchangeLimitInfo( OUT std::map< int, int >& mapBeginExchangeLimitInfo ) const
{
	for( EXCHANGE_LIMIT_CONTAINER::const_iterator it = m_listBeginExchangeLimitInfo.begin(); it != m_listBeginExchangeLimitInfo.end(); ++it )
	{
		BOOST_TEST_FOREACH( const int, iDestItemID, it->m_setDestItemID )
		{
			std::pair< std::map< int, int >::iterator, bool > pairResult = mapBeginExchangeLimitInfo.insert( std::map< int, int >::value_type( iDestItemID, it->m_iLimitQuantity ) );
			if( pairResult.second == false )
			{
				pairResult.first->second += it->m_iLimitQuantity;
			}
		}
	}
}

int KGlobalServer::RetrieveExchangeLimitData( IN const int iDestItemID, OUT int& iLimitUID )
{
	iLimitUID = 0;

	if( m_iExchangeLimitUID == 0 )
	{
		return KGlobalServer::ELR_NOT_INITED;
	}

	// 여기서는 진행 중인 제한 중에서 종료되지 않은 것을 정확하게 찾을 필요가 있다.
	__time64_t tCurrentTime = CTime::GetCurrentTime().GetTime();
	int iResult = KGlobalServer::ELR_PASS;

	for( EXCHANGE_LIMIT_CONTAINER::iterator it = m_listBeginExchangeLimitInfo.begin(); it != m_listBeginExchangeLimitInfo.end(); ++it )
	{
		KExchangeLimitInfo& kInfo = *it;

		if( kInfo.m_setDestItemID.find( iDestItemID ) == kInfo.m_setDestItemID.end() )
		{
			// 찾고 있는 교환 대상이 아니라면 넘어가자
			continue;
		}

		if( kInfo.m_iLimitQuantity <= 0 )
		{
			// 이미 허용 수량이 소진된 제한이라면 넘어가자
			iResult = KGlobalServer::ELR_EXHAUSTED;
			continue;
		}

		--kInfo.m_iLimitQuantity;
		m_bExchangeLimitInfoChanged	= true;

		iLimitUID	= kInfo.m_iLimitUID;
		iResult		= KGlobalServer::ELR_SUCCEED;
		break;
	}

	return iResult;
}

int KGlobalServer::ReInsertExchangeLimitData( IN const int iLimitUID )
{
	if( m_iExchangeLimitUID == 0 )
	{
		return KGlobalServer::ELR_NOT_INITED;
	}

	for( EXCHANGE_LIMIT_CONTAINER::iterator it = m_listBeginExchangeLimitInfo.begin(); it != m_listBeginExchangeLimitInfo.end(); ++it )
	{
		KExchangeLimitInfo& kInfo = *it;

		if( kInfo.m_iLimitUID != iLimitUID )
		{
			// 찾고 있는 대상이 아니라면 넘어가자
			continue;
		}

		++kInfo.m_iLimitQuantity;
		m_bExchangeLimitInfoChanged = true;
		break;
	}

	return KGlobalServer::ELR_SUCCEED;
}

void KGlobalServer::CheckExchangeLimit( IN const bool bForceNotify /*= false */)
{
	bool bExchangeLimitChanged = false;
	if( KGlobalServer::ELI_CHECK_TERM <= m_kExchangeLimitCheckTimer.elapsed() )
	{
		m_kExchangeLimitCheckTimer.restart();

		__time64_t tCurrentTime = CTime::GetCurrentTime().GetTime();

		// 종료된 이벤트 처리
		for( EXCHANGE_LIMIT_CONTAINER::iterator it = m_listBeginExchangeLimitInfo.begin(); it != m_listBeginExchangeLimitInfo.end(); )
		{
			if( it->m_tEndDate <= tCurrentTime )
			{
				it = m_listBeginExchangeLimitInfo.erase( it );
				bExchangeLimitChanged = true;
			}
			else
			{
				++it;
			}
		}

		// 시작된 이벤트 처리
		for( EXCHANGE_LIMIT_CONTAINER::iterator it = m_listExchangeLimitInfo.begin(); it != m_listExchangeLimitInfo.end(); )
		{
			// 이미 종료된 이벤트는 제거한다.
			if( it->m_tEndDate <= tCurrentTime )
			{
				it = m_listExchangeLimitInfo.erase( it );
			}
			else if( it->m_tBeginDate <= tCurrentTime )
			{
				// 시작된 이벤트 처리
				m_listBeginExchangeLimitInfo.push_back( *it );
				it = m_listExchangeLimitInfo.erase( it );
				bExchangeLimitChanged = true;
			}
			else
			{
				++it;
			}
		}
	}

	if( ( bExchangeLimitChanged == true ) ||	// 새로 시작된 교환 제한 목록이 있거나,
		( ( m_bExchangeLimitInfoChanged == true ) &&	// 수량에 변화가 있을 것이라 예상되는 상태에서
		  ( KGlobalServer::ELI_QUANTITY_TERM <= m_kExchangeLimitQuantityTimer.elapsed() ) )	||// 동기화 제한 시간을 충족하거나
		  ( bForceNotify == true )	// 동기화 패킷을 날릴 것을 강제할 때
		  )
	{
		m_bExchangeLimitInfoChanged = false;
		m_kExchangeLimitQuantityTimer.restart();

		KEGB_EXCHANGE_LIMIT_INFO_NOT kPacket;
		GetExchangeLimitUID( kPacket.m_iExchangeLimitUID );
		GetBeginExchangeLimitInfo( kPacket.m_mapExchangeLimitInfo );
		BroadCastAllGS( EGB_EXCHANGE_LIMIT_INFO_NOT, kPacket );
	}
}

IMPL_ON_FUNC( DBE_EXCHANGE_LIMIT_INFO_ACK )
{
	if( ++m_iExchangeLimitUID == 0 )
	{
		++m_iExchangeLimitUID;
	}

	m_listExchangeLimitInfo	= kPacket_.m_listExchangeLimitInfo;
	m_listBeginExchangeLimitInfo.clear();
	m_bExchangeLimitInfoChanged = false;

	CheckExchangeLimit( true );

	START_LOG( cout, L"교환 제한 정보 설정" )
		<< BUILD_LOG( m_iExchangeLimitUID )
		<< BUILD_LOG( m_listExchangeLimitInfo.size() )
		<< END_LOG;
}

IMPL_ON_FUNC( DBE_EXCHANGE_LIMIT_INFO_NOT )
{
	if( CheckExchangeLimitUID( kPacket_.m_iExchangeLimitUID ) == false )
	{
		return;
	}

	ReInsertExchangeLimitData( kPacket_.m_iLimitUID );
}

//{{ 2012. 09. 03	임홍락	글로벌 미션 매니저
IMPL_ON_FUNC( DBE_GET_GLOBAL_MISSION_INFO_ACK )
{
	SiKGlobalMissionManager()->SetInitMissionInfo( kPacket_ );
}

void KGlobalServer::CheckFieldBossSystem( IN const CTime tCurrentTime )
{
	if( SiKFieldBossManager()->CheckTerm( tCurrentTime.GetTime() ) == false )
		return;

	const bool bPortalOpen	= SiKFieldBossManager()->IsPortalOpen();
	const bool bCoolTime	= SiKFieldBossManager()->IsCoolTime();

	const CTime tPortalAppearanceTime( SiKFieldBossManager()->GetPortalAppearanceTime() );

	// 포탈이 열려 있는지 확인
	if( bPortalOpen == true )
	{
		const CTime tPortalHoldingTime = tPortalAppearanceTime + CTimeSpan( 0, 0, SiCXSLFieldBossData()->GetFieldBossPortalHoldingTime_M(), 0 );
		if( tPortalHoldingTime <= tCurrentTime )
		{
			// 포탈 닫힘 처리
			SiKFieldBossManager()->ClosePortal();
		}
	}

	// 쿨 타임 체크
	if( bCoolTime == true )
	{
		const CTime tFieldBossCoolTime = tPortalAppearanceTime + CTimeSpan( 0, 0, SiCXSLFieldBossData()->GetFieldBossCoolTime_M(), 0 );
		if( tFieldBossCoolTime <= tCurrentTime )
		{
			// 쿨 타임 종료 처리
			SiKFieldBossManager()->SetCoolTimeState( false );
			SiKFieldBossManager()->ClearTotalDangerousValue();	// TotalDangerousValue를 처음부터 계산한다.
		}
	}

	// 포탈이 닫히거나 쿨 타임이 종료되었다면 알림 패킷을 보내자
	if( ( bPortalOpen != SiKFieldBossManager()->IsPortalOpen() ) || ( bCoolTime != SiKFieldBossManager()->IsCoolTime() ) )
	{
		KEGB_BATTLE_FIELD_BOSS_INFO_NOT kPacket;
		kPacket.m_iPortalAppearanceMap	= SiKFieldBossManager()->GetPortalAppearanceMap();
		kPacket.m_iPortalDestination	= SiKFieldBossManager()->GetPortalDestination();
		kPacket.m_bPortalOpen			= SiKFieldBossManager()->IsPortalOpen();
		kPacket.m_bCoolTime				= SiKFieldBossManager()->IsCoolTime();
		BroadCastAllGS( EGB_BATTLE_FIELD_BOSS_INFO_NOT, kPacket );
	}
}
