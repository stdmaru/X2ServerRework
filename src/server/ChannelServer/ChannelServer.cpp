#include <WinSock2.h>
#include <sstream>
#include <iomanip>
#include <boost/bind/bind.hpp>
using namespace boost::placeholders;
#include "ChannelServer.h"
#include "ActorManager.h"
#include "ChannelSimLayer.h"
#include "ChannelNetLayer.h"
#include "Socket/NetCommon.h"
#include "DBLayer.h"
#include "ChannelAccountDBThread.h"
#include "ChannelLogDBThread.h"
//{{ 2012. 07. 02	최육사		대기열 시스템
	#include "CHFSM.h"
//}}
#include "ChannelUser.h"

#include "NetError.h"
//{{ 2009. 12. 21  최육사	서버군 확장
#include "Enum/Enum.h"
//}}

//{{ 2010. 02. 16  최육사	해킹툴 리스트
	#include "HackingProtectionManager.h"
//}}




#include "../Common/OnlyGlobal/ProcessCommuniationModule/ProcessCommunicationManager.h"

//{{ 2011. 03. 29	최육사	TBB 메모리 관리자
//#ifdef SERV_TBB_MALLOC_PROXY_TEST
//	#include <tbb/tbbmalloc_proxy.h>
//#endif SERV_TBB_MALLOC_PROXY_TEST
//}}

//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	#include "MachineBlockManager.h"
	#include "GameSysVal/GameSysVal.h"
//}}

//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
//}}
//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
//}}
//{{ 2012. 07. 04	최육사		대기열 시스템
	#include "WaitQueueManager.h"
//}}

//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
//}}

//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능
	#include "ChannelScriptRefreshManager.h"
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

//#include "vld.h"

NiImplementRTTI( KChannelServer, KBaseServer );
ImplPfID( KChannelServer, PI_CHANNEL_SERVER );

#define CLASS_TYPE KChannelServer

ImplToStringW( KChannelServer )
{
	START_TOSTRING_PARENTW( KBaseServer );

	stm_ << L"Channel List Size : " << m_mapChannelList.size() << std::endl;

    return stm_;
}

KChannelServer::KChannelServer(void) :
m_bIsFirstUpdate( false )
,m_bIsFirstUpdateServerGroup( false )
{
}

KChannelServer::~KChannelServer(void)
{
}

#include <lua_tinker.h>

void KChannelServer::RegToLua()
{
    KBaseServer::RegToLua();

	lua_tinker::class_add<KChannelServer>( g_pLua, "KChannelServer" );
	lua_tinker::class_inh<KChannelServer, KBaseServer>( g_pLua );
	lua_tinker::class_def<KChannelServer>( g_pLua, "dump", &KChannelServer::Dump );

	lua_tinker::decl( g_pLua, "Server", this );
}

const wchar_t*  g_szConfigFile = L"config_ch";
KBaseServer*    CreateKNC()                             { return KBaseServer::KeepObject<KChannelServer>(); }
KThread*        CreateDBThread( int iDBConnectionInfo, const wchar_t* szDSN, bool bDBConnStr )
{
    switch( iDBConnectionInfo )
    {
    case KDBLayer::DC_ACCOUNT:
        return new KChannelAccountDBThread( szDSN, bDBConnStr );
    case KDBLayer::DC_LOG:
        return new KChannelLogDBThread( szDSN, bDBConnStr );
		//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
		//}}
    default:
        START_LOG( cerr, L"접속하려는 DB 종류가 이상함." )
            << BUILD_LOG( iDBConnectionInfo )
            << END_LOG;
        return new KChannelLogDBThread( szDSN, bDBConnStr );
    }
}

bool KChannelServer::Init()
{
    KSimLayer::KeepObject<KChannelSimLayer>();
    KNetLayer::KeepObject<KChannelNetLayer>();
    KActorManager::KeepObject<KActorManager>();
	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}

	//{{ 2012. 07. 02	최육사		대기열 시스템
	KNetLayer::GetKObj()->SetActorFactory( IActorFactoryPtr( new KActorFactory<KChannelUser, KCHFSM, 32*1024> ) );
	//}}
	//{{ 2011. 06. 08	최육사	KEvent 출처 등록 기능
	KNetLayer::GetKObj()->SetSessionType( KSession::SST_CLIENT );
	//}}

	InitLoginResultInfo();

    _JIF( KBaseServer::Init(), return false );

    //WriteServerInfoToDB();



	SetServerReady( true );

	//{{ 2009. 8. 27  최육사	server ready
	OnServerReadyComplete();
	//}}

	m_bServerRunningProcessCommunicationOnOff = false;

    return true;
}

void KChannelServer::OnServerReadyComplete()
{
	KBaseServer::OnServerReadyComplete();

	RefreshServerGroupList();

	//{{ 2009. 8. 13  최육사	채널리스트
	RefreshChannelList();
	//}}

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	SiKHackingProtectionManager()->Init();
	//}}

	//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능
	SiKChannelScriptRefreshManager()->Init();
	SiKChannelScriptRefreshManager()->RegToLua( g_pLua );
	//}}

	//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	SiKMachineBlockManager()->Init();
	//}}

	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}

	if (SiKGameSysVal()->GetProcessCommunication() == true)
	{
		SiKProcessCommunicationManager()->InitRead(TEXT("\\\\.\\pipe\\ChannelServerRead"));
		SiKProcessCommunicationManager()->InitWrite(TEXT("\\\\.\\pipe\\ChannelServerWrite"));
		SiKProcessCommunicationManager()->Begin();
	}

	// 서버군 이름 표시
	START_LOG( cout2, SEnum::GetServerGroupName( GetServerGroupID() ) )
		<< BUILD_LOG( GetServerGroupID() )
		<< BUILD_LOG( GetUID() )
		<< BUILD_LOG( GetName() );
}

void KChannelServer::ShutDown()
{
	if (SiKGameSysVal()->GetProcessCommunication() == true)
	{
		SiKProcessCommunicationManager()->ShutDown();
	}

    // 아직 아무 종료처리도 진행되기 전.
    KBaseServer::ShutDown();        // 이곳에서 모든 종료가 끝난다. 각각의 Layer들도 모두 소멸됨.

	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}
}

bool KChannelServer::DestroyAllSession()
{
    // NetLayer의 Accepter만 닫힌 시점. 아직 모든 Layer들이 살아있다.

    KActorManager* pkActorManager = KActorManager::GetKObj();

    START_LOG( cout, L"현재 유저 : " << pkActorManager->GetCount() << L" 명" );

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

void KChannelServer::Tick()
{
    KBaseServer::Tick();

	if( m_kServerGroupListRefreshTimer.elapsed() > 60.f * 60 )
	{
		RefreshServerGroupList();

		m_kServerGroupListRefreshTimer.restart();
	}

	//{{ 2009. 1. 16  최육사	채널 리스트 업데이트
	if( m_kChannelListRefreshTimer.elapsed() > 60.f )
	{
		RefreshChannelList();

		m_kChannelListRefreshTimer.restart();
	}
	//}}

	//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	SiKMachineBlockManager()->Tick();
	//}}

	if( m_kLoginResultInfoTimer.elapsed() > 60.f )
	{
		WriteLoginResultInfoToDB();
		m_kLoginResultInfoTimer.restart();

		InitLoginResultInfo();
	}

	if ( m_tTimeProcessCommunicationONOFF.elapsed() > 60.0f )
	{
		if (SiKGameSysVal()->GetProcessCommunication() == true && m_bServerRunningProcessCommunicationOnOff == true)
		{
			SiKProcessCommunicationManager()->InitRead(TEXT("\\\\.\\pipe\\ChannelServerRead"));
			SiKProcessCommunicationManager()->InitWrite(TEXT("\\\\.\\pipe\\ChannelServerWrite"));
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

void KChannelServer::RefreshServerGroupList()
{
	SendToLogDB( DBE_SERVERGROUP_LIST_REQ );
}

void KChannelServer::UpdateServerGroupList( const std::map< int, KServerGroupInfo >& mapServerGroupList )
{
	m_mapServerGroupList.clear();
	m_mapServerGroupList = mapServerGroupList;

	// 처음 업데이트 한거라면 리스트를 로그로 출력한다!
	if( !m_bIsFirstUpdateServerGroup )
	{
		std::map< int, KServerGroupInfo >::iterator mit;
		for( mit = m_mapServerGroupList.begin(); mit != m_mapServerGroupList.end(); ++mit )
		{
			START_LOG( cout, L"서버군 :" )
				<< BUILD_LOG( mit->second.m_iOrder )
				<< BUILD_LOG( mit->second.m_iServerGroupUID )
				<< BUILD_LOG( mit->second.m_wstrServerGroupName )
				<< BUILD_LOG( mit->second.m_kServerIP )
				<< BUILD_LOG( mit->second.m_usMasterPort )
				<< BUILD_LOG( mit->second.m_usNCUDPPort )
				<< BUILD_LOG( mit->second.m_bEnable );
		}

		m_bIsFirstUpdateServerGroup = true;
	}
}

void KChannelServer::RefreshChannelList()
{
	KDBE_CHANNEL_LIST_REQ kPacketToDB;
	kPacketToDB.m_iServerGroupID = GetServerGroupID();
	SendToLogDB( DBE_CHANNEL_LIST_REQ, kPacketToDB );
}

void KChannelServer::UpdateChannelList( const std::map< int, KChannelInfo >& mapChannelList )
{
	//{{ 2012. 03. 13	김민성		실시간 동접 하락 체크 sms
	//}}

	m_mapChannelList.clear();
	m_mapChannelList = mapChannelList;

	std::map< int, KChannelInfo >::iterator mit;
	for( mit = m_mapChannelList.begin(); mit != m_mapChannelList.end(); ++mit )
	{
		mit->second.m_iCurPartyCount = 0;
		mit->second.m_iPlayGamePartyCount = 0;
	}

	// 처음 업데이트 한거라면 리스트를 로그로 출력한다!
	if( !m_bIsFirstUpdate )
	{
		std::map< int, KChannelInfo >::iterator mit;
		for( mit = m_mapChannelList.begin(); mit != m_mapChannelList.end(); ++mit )
		{
			START_LOG( cout, L"채널 이름 : " << mit->second.m_wstrChannelName << L",  IP : " << mit->second.m_wstrIP << L",  Port : " << mit->second.m_usMasterPort );
		}

		m_bIsFirstUpdate = true;
	}
}

//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
void KChannelServer::UpdateChannelBonusList( const std::map< int, KChannelBonusInfo >& mapChannelBonusList )
{
	m_mapChannelBonusList.clear();

	m_mapChannelBonusList = mapChannelBonusList;

}
//}}

void KChannelServer::ProcessEvent( const KEventPtr& spEvent_ )
{
    KSerializer ks;
    ks.BeginReading( &spEvent_->m_kbuff );

    DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

    switch( spEvent_->m_usEventID )
    {
       _CASE( DBE_SERVER_ON_ACK, KDBE_CHANNEL_SERVER_ON_ACK );
       _CASE( DBE_UPDATE_SERVER_INFO_ACK, KServerList );
		CASE( DBE_CHANNEL_LIST_ACK );

		//{{ 2010. 02. 16  최육사	해킹툴 리스트
		CASE( DBE_CHECK_HACKING_TOOL_LIST_ACK );
		//}}
		//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
		CASE( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK );
		//}}
		//{{ 2012. 07. 04	최육사		대기열 시스템
		CASE( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_ACK );
		//}}

		//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능
		_CASE( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );
		//}}
		CASE( DBE_SERVERGROUP_LIST_ACK );
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

_IMPL_ON_FUNC( DBE_SERVER_ON_ACK, KDBE_CHANNEL_SERVER_ON_ACK )
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

_IMPL_ON_FUNC( DBE_UPDATE_SERVER_INFO_ACK, KServerList )
{
    SetServerList( kPacket_ );
}

IMPL_ON_FUNC( DBE_CHANNEL_LIST_ACK )
{
    UpdateChannelList( kPacket_.m_mapChannelList );

	//{{ 2012. 06. 29	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	UpdateChannelBonusList(kPacket_.m_mapChannelBonusList);
	//}}
}

//{{ 2010. 02. 16  최육사	해킹툴 리스트

IMPL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_ACK )
{
	SiKHackingProtectionManager()->UpdateHackingToolList( kPacket_ );
}

//}}

//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
IMPL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_ACK )
{
	std::vector< std::string > vecBlockMachineIDList;
	SiKMachineBlockManager()->UpdateMachineBlockList( kPacket_, vecBlockMachineIDList );

	START_LOG( cout2, L"MachinBlock ID 목록 갱싱 성공!" );
}
//}}

//{{ 2012. 07. 04	최육사		대기열 시스템
IMPL_ON_FUNC( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_ACK )
{
	SiKWaitQueueManager()->SetTotalCCU( kPacket_.m_iTotalCCU );
}
//}}

//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능
_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
		//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
		//}}
		//{{ 2013. 02. 19	최육사	로그 시스템 개선
		CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CH_LOG_MANAGER, KLogManager );
		//}}

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

IMPL_ON_FUNC( DBE_SERVERGROUP_LIST_ACK )
{
	UpdateServerGroupList( kPacket_.m_mapServerGroupList );
}

void KChannelServer::InitLoginResultInfo()
{
	m_kLoginResultInfo.m_vecResultCount.clear();

	for(int i = 0; i < KLoginResultInfo::LRTE_SENTINEL; i++)
	{
		m_kLoginResultInfo.m_vecResultCount.push_back(0);
	}
}

void KChannelServer::WriteLoginResultInfoToDB()
{
	// InitLoginResultInfo()이 호출될 때는 ServerUID 값을 가져오기 전이라 여기서 처리함.
	m_kLoginResultInfo.m_iServerGroupID = GetServerGroupID();
	m_kLoginResultInfo.m_uiServerUID = GetUID();

	KDBE_INSERT_LOGIN_RESULT_INFO_REQ kPacket;
	kPacket.m_iServerGroupID = m_kLoginResultInfo.m_iServerGroupID;
	kPacket.m_uiServerUID = m_kLoginResultInfo.m_uiServerUID;
	kPacket.m_vecResultCount = m_kLoginResultInfo.m_vecResultCount;

	SendToLogDB( DBE_INSERT_LOGIN_RESULT_INFO_REQ, kPacket );
}

void KChannelServer::PlusLoginResultCount(KLoginResultInfo::LOGIN_RESULT_TYPE_ENUM eResultType)
{
	if( eResultType < KLoginResultInfo::LRTE_SUCCESS || KLoginResultInfo::LRTE_SENTINEL < eResultType )
	{
		START_LOG( cerr, L"유효하지않은 로그인 결과 사유입니다!" )
			<< BUILD_LOG( eResultType )
			<< END_LOG;
		return ;
	}

	m_kLoginResultInfo.m_vecResultCount[eResultType]++;

	START_LOG( clog, L"로그인 결과 사유 카운트 증가!!" )
		<< BUILD_LOG( eResultType )
		<< BUILD_LOG( m_kLoginResultInfo.m_vecResultCount[eResultType] )
		<< END_LOG;
}
