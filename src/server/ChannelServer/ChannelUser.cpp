#include "ChannelUser.h"
#include <dbg/dbg.hpp>
//{{ 2012. 07. 02	최육사		대기열 시스템
	#include "CHFSM.h"
//}}

#include "DBLayer.h"
#include "NetError.h"
#include "ChannelSimLayer.h"
#include "ChannelServer.h"
//#include "GashAuthManager.h"
//{{ 2009. 12. 16  최육사	동접툴
#include "CCUToolManager.h"
//}}

//{{ 2011. 01. 20 김민성	동접 모니터링 툴               디비커리인젝션
#include "ODBC/Odbc.h"
//}}


#include "x2/platform/format.h"

//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
//}}

//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	#include "GameSysVal/GameSysVal.h"
//}}

//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
//}}
//{{ 2012. 07. 02	최육사		대기열 시스템
	#include "WaitQueueManager.h"
//}}
//{{ 2012. 07. 18	최육사		재접속 로직
//}}

NiImplementRTTI( KChannelUser, KActor );
ImplPfID( KChannelUser, PI_CHANNEL_USER );

#define CLASS_TYPE  KChannelUser

KChannelUser::KChannelUser(void)
{
	//{{ 2009. 10. 14  최육사	필터
	InitRepeatEventFilter();
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}
	//{{ 2012. 07. 04	최육사		대기열 시스템
	m_bPassedEnterChannel = false;
	//}}
	//{{ 2012. 07. 18	최육사		재접속 로직
	//}}
	m_wstrID = L"";
}

KChannelUser::~KChannelUser(void)
{
}

ImplToStringW( KChannelUser )
{
    return START_TOSTRING_PARENTW( KActor );
}

void KChannelUser::ProcessEvent( const KEventPtr& spEvent_ )
{
	//{{ 2012. 07. 18	최육사		재접속 로직
	//}}

	if( RoutePacket( spEvent_.get() ) )
		return;

	//{{ 2008. 8. 28  최육사	이벤트 필터
	if( !CheckReqEvent( spEvent_->m_usEventID ) )
		return;

    DWORD dwElapTime = ::GetTickCount();  // 040428. 패킷 처리 소요시간을 구한다.

    switch( spEvent_->m_usEventID )
    {
    CASE( ECH_VERIFY_ACCOUNT_REQ );
    //CASE( DBE_GASH_USER_LOGIN_ACK );// 해외에서 이제 사용안함
	CASE_NOPARAM( ECH_GET_CHANNEL_LIST_REQ );
	CASE_NOPARAM( ECH_DISCONNECT_REQ );

	//{{ 2009. 12. 16  최육사	동접툴
	CASE( E_TOOL_GET_CCU_INFO_REQ );
	CASE( DBE_GET_CONCURRENT_USER_INFO_ACK );
	//}}

	CASE_NOPARAM( E_RESERVE_DESTROY );

	//{{ 2011. 01. 20 김민성	동접 모니터링 툴
	CASE( E_TOOL_CHECK_LOGIN_REQ );
	CASE_NOPARAM( E_TOOL_SERVER_LIST_REQ );
	CASE( DBE_GET_CONCURRENT_USER_LOGIN_ACK );
	CASE( DBE_GET_GAME_SERVER_LIST_ACK );
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

	//{{  2011.11.08     김민성    서버에서 SN 발급 및 유효성 검사
	//}}
	CASE_NOPARAM( ECH_GET_SERVERGROUP_LIST_REQ );

	CASE( EPUBLISHER_AUTHENTICATION_ACK );
	CASE( DBE_CH_USER_GENERAL_LOGIN_ACK );
	CASE( DBE_CH_USER_KOGOTP_LOGIN_ACK );

	CASE( ECH_SECURITY_AUTH_REQ );
	CASE( EPUBLISHER_SECURITY_AUTH_ACK );

	CASE( EPUBLISHER_SECURITY_INFO_NOT );


    default:
        dbg::cerr << L"Event handler is not defined. " << spEvent_->GetIDStr();
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

void KChannelUser::Tick()
{
    KActor::Tick();

	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	CheckHackingToolList();
	//}}

	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}

    switch( GetStateID() )
	{
	case KCHFSM::S_LOGINED:
		{
			// 접속가능 IP 인증을 통과해야만 채널리스트를 주도록함.
			if( m_kTimer[TM_CHANNEL_LIST].elapsed() < 60.f )
				break;

			m_kTimer[TM_CHANNEL_LIST].restart();

			KECH_GET_SERVERGROUP_LIST_NOT kPacketNot;
			kPacketNot.m_mapServerGroupList = GetKChannelServer()->GetServerGroupList();
			SendPacket( ECH_GET_SERVERGROUP_LIST_NOT, kPacketNot );

			//{{ 2012. 07. 02	최육사		대기열 시스템
			if( m_bPassedEnterChannel == true )
			{
				SendChannelList();
			}
			//}}
		}
		break;

		//{{ 2012. 07. 02	최육사		대기열 시스템
	case KCHFSM::S_WAIT_QUEUE:
		{
			// 만약 인원제한모드가 아니면 체크조차 하지 않는다.
			switch( SiKWaitQueueManager()->GetMode() )
			{
			case KWaitQueueManager::WQ_NONE:
				{
					// 인증 패스!
					m_bPassedEnterChannel = true;
					SiKWaitQueueManager()->RegPassedUser( GetUID() );

					// 상태 이동!
					StateTransition( KCHFSM::I_TO_AUTHENTICATED );

					// 해당 유저에게 팝업을 띄우기 위해 패킷 전송
					KECH_WAIT_QUEUE_MESSAGE_NOT kPacketNot;
					kPacketNot.m_bPassed = true;
					SendPacket( ECH_WAIT_QUEUE_MESSAGE_NOT, kPacketNot );

					// 채널 리스트 전송!
					SendChannelList();
				}
				break;

			case KWaitQueueManager::WQ_CCU_LIMIT:
				{
					// 대기큐를 체크하자! 자기 차례가 오면 채널리스트를 클라이언트한테 보낸다!
					if( SiKWaitQueueManager()->IsExist( GetUID() ) == true )
					{
						if( SiKWaitQueueManager()->IsNextEnterUser( GetUID() ) == true )
						{
							LIF( SiKWaitQueueManager()->UnRegWaitUser( GetUID() ) );

							// 인증 패스!
							m_bPassedEnterChannel = true;
                            SiKWaitQueueManager()->RegPassedUser( GetUID() );

							// 상태 이동!
							StateTransition( KCHFSM::I_TO_AUTHENTICATED );

							// 해당 유저에게 팝업을 띄우기 위해 패킷 전송
							KECH_WAIT_QUEUE_MESSAGE_NOT kPacketNot;
							kPacketNot.m_bPassed = true;
							SendPacket( ECH_WAIT_QUEUE_MESSAGE_NOT, kPacketNot );

							// 채널 리스트 전송!
							SendChannelList();
						}
						else
						{
							if( m_kTimer[TM_WAIT_QUEUE].elapsed() > 5.0 )
							{
								m_kTimer[TM_WAIT_QUEUE].restart();

								// 해당 유저에게 팝업을 띄우기 위해 패킷 전송
								KECH_WAIT_QUEUE_MESSAGE_NOT kPacketNot;
								LIF( SiKWaitQueueManager()->GetWaitUserCount( GetUID(), kPacketNot.m_iWaitUserCount ) );
								SendPacket( ECH_WAIT_QUEUE_MESSAGE_NOT, kPacketNot );
							}
						}
					}
				}
				break;

			case KWaitQueueManager::WQ_ENTRY_BAN:
				{
					// 무조건 메시지만 보냅니다.
					if( m_kTimer[TM_WAIT_QUEUE].elapsed() > 5.0 )
					{
						m_kTimer[TM_WAIT_QUEUE].restart();

						// 해당 유저에게 팝업을 띄우기 위해 패킷 전송
						KECH_WAIT_QUEUE_MESSAGE_NOT kPacketNot;
						LIF( SiKWaitQueueManager()->GetWaitUserCount( GetUID(), kPacketNot.m_iWaitUserCount ) );
						SendPacket( ECH_WAIT_QUEUE_MESSAGE_NOT, kPacketNot );
					}
				}
				break;

			default:
				// 그외에 것들은 아무런 동작도 하지 않습니다.
				break;
			}
		}
		break;
		//}}

    case KCHFSM::S_EXIT:
        KActorManager::GetKObj()->ReserveDelete( m_nUID );    // 자기 자신을 스스로 삭제할 수 없음.
        break;
    }
}

void KChannelUser::OnDestroy()
{
    KSession::OnDestroy();

    switch( GetStateID() )
	{
    case KCHFSM::S_INIT:
    case KCHFSM::S_LOGINED:
		//{{ 2012. 07. 04	최육사		대기열 시스템
	case KCHFSM::S_WAIT_QUEUE:
		//}}
		{
			//{{ 2012. 07. 10	최육사		대기열 시스템
			SiKWaitQueueManager()->UnRegWaitUser( GetUID() );
			SiKWaitQueueManager()->UnRegPassedUser( GetUID() );
			//}}

			StateTransition( KCHFSM::I_TO_EXIT );    // OnDestroy()가 불린 현재 KSession::Tick 처리 중이며,
			// KLoginUser::Tick 처리 단계에서 종료 대기명단에 등록된다.
		}
        break;
    }
}

bool KChannelUser::RoutePacket( const KEvent* pkEvent_ )
{
    if( pkEvent_->m_kDestPerformer.m_dwPerformerID != PI_CHANNEL_USER )
    {
        int iEval = CompareServerLevel( pkEvent_->m_kDestPerformer.m_dwPerformerID, PI_CHANNEL_USER );
        if( iEval < 0 )
        {
            _JIF( !pkEvent_->IsEmptyTrace(), return true );
            KEvent kEvent( *pkEvent_ );
            kEvent.PopTrace();
            LIF( KSession::SendPacket( kEvent ) );
            return true;
        }
        else if( iEval > 0 )
        {
            START_LOG( cerr, L"The channel server is currently the top-level server." )
                << BUILD_LOG( pkEvent_->m_kDestPerformer.m_dwPerformerID )
                << BUILD_LOG( pkEvent_->m_usEventID )
                << BUILD_LOG( pkEvent_->GetIDStr() )
                << END_LOG;

            return true;
        }
        else
        {
            KEventPtr spEvent( pkEvent_->Clone() );
            spEvent->PushTrace( GetUID() );

            switch( GetPerformerClass( pkEvent_->m_kDestPerformer.m_dwPerformerID ) )
            {
            case PC_SERVER:
                KBaseServer::GetKObj()->QueueingEvent( spEvent );
                return true;
            case PC_LOG_DB:
                SiKDBLayer()->QueueingEvent( spEvent );
                return true;
            default:
                START_LOG( cerr, L"Invalid packet destination." )
                    << BUILD_LOG( pkEvent_->m_kDestPerformer.m_dwPerformerID )
                    << END_LOG;
                return true;
            }
        }
    }

    return false;
}

bool KChannelUser::SendID( unsigned short usEventID )
{
	return KSession::SendID( PI_GS_PROXY, GetUID(), NULL, usEventID );
}

void KChannelUser::SendToLogDB( unsigned short usEventID )
{
	SendToLogDB( usEventID, char() );
}

//{{ 2008. 8. 28  최육사	이벤트 필터
void KChannelUser::InitRepeatEventFilter()
{
	m_kRepeatEventFilter.RegRepeatEvent( ECH_VERIFY_ACCOUNT_REQ, ECH_VERIFY_ACCOUNT_ACK );
	m_kRepeatEventFilter.RegRepeatEvent( ECH_GET_CHANNEL_LIST_REQ, ECH_GET_CHANNEL_LIST_ACK );

	// 등록 카운트 검사
	if( m_kRepeatEventFilter.GetRegCount() != m_kRepeatEventFilter.GetReqEventCount()  ||
		m_kRepeatEventFilter.GetRegCount() != m_kRepeatEventFilter.GetAckEventCount() )
	{
		START_LOG( cerr, L"Registered event filter counts do not match; some events failed to register!" )
			<< BUILD_LOG( m_kRepeatEventFilter.GetRegCount() )
			<< BUILD_LOG( m_kRepeatEventFilter.GetReqEventCount() )
			<< BUILD_LOG( m_kRepeatEventFilter.GetAckEventCount() )
			<< END_LOG;
	}
}

bool KChannelUser::CheckReqEvent( unsigned short usEventID )
{
	switch( usEventID )
	{
	case ECH_VERIFY_ACCOUNT_REQ:
	case ECH_GET_CHANNEL_LIST_REQ:
		{
			return m_kRepeatEventFilter.CheckReqEvent( usEventID );
		}
		break;

	default:
		break;
	}

	return true;
}

void KChannelUser::CheckAckEvent( unsigned short usEventID )
{
	switch( usEventID )
	{
	case ECH_VERIFY_ACCOUNT_ACK:
	case ECH_GET_CHANNEL_LIST_ACK:
		{
			m_kRepeatEventFilter.CheckAckEvent( usEventID );
		}
		break;

	default:
		break;
	}
}

//{{ 2010. 02. 16  최육사	해킹툴 리스트

void KChannelUser::CheckHackingToolList()
{
	if( m_kTimer[TM_HACKING_TOOL_LIST].elapsed() < m_kUserHackingManager.GetHackingToolCheckTime() )
		return;

	m_kTimer[TM_HACKING_TOOL_LIST].restart();

	KEGS_HACKING_TOOL_LIST_NOT kNot;
	m_kUserHackingManager.GetHackingToolList( kNot.m_iRandomKey, kNot.m_vecHackingToolList );
	SendPacket( EGS_HACKING_TOOL_LIST_NOT, kNot );
}

//}}

//{{ 2013.03.19 Hyojin Cho Configuring an overseas public authentication system (authentication packets are processed separately for overseas)
	IMPL_ON_FUNC( ECH_VERIFY_ACCOUNT_REQ )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

		START_LOG( clog, L"Authentication Request." );

		KECH_VERIFY_ACCOUNT_ACK kPacket;

		//{{ 2009. 6. 10 Choi Yuksa access IP restriction
		if( GetKChannelSimLayer()->CheckIP( GetIPStr() ) == false )
		{
			if( GetKChannelSimLayer()->GetCheckIPMode() == KChannelSimLayer::CIM_SERVICE_MODE )
			{
				START_LOG( cout, L"[Service Mode] A user whose IP is blocked attempted to connect : " << GetIPStr() << L" ,  UserID : " << kPacket_.m_wstrID );
			}
			else
			{
				START_LOG( cout, L"[Inspection Mode] General user access during maintenance : " << GetIPStr() << L" ,  UserID : " << kPacket_.m_wstrID );
			}

			kPacket.m_iOK = NetError::ERR_CONNECT_05;
			SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacket );

			// Schedule termination
			ReserveDestroy();
			return;
		}

		//{{ 2011. 1. 24 Hyojin Cho *Important* Since this is a string that will be entered directly into the query, check for injection.
		if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrID ) )
		{
			kPacket.m_iOK = NetError::ERR_ODBC_00;
			SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacket );
			return;
		}


				CTime tCurrent = CTime::GetCurrentTime();
				kPacket.m_wstrCurrentTime = ( CStringW )tCurrent.Format( _T( "%Y-%m-%d %H:%M:%S" ) );

		DWORD dwAuthFlag = KSimLayer::GetKObj()->GetAuthFlag();
		if(KSimLayer::AF_INTERNAL == dwAuthFlag)
		{
			if( KSimLayer::GetKObj()->GetUseKogOTP() )
			{
				m_wstrID = kPacket_.m_wstrID;

				if (!kPacket_.m_bIDVerified) // Check whether the publisher has been authenticated
				{
					// Originally, you had to request public authentication, but this was bypassed for in-house testing.
					// Nevertheless, in order to update the OTP number when using KOG OTP
					// DBE_CH_USER_GENERAL_LOGIN_REQ must be requested when Publisher authentication is properly received.

					KDBE_CH_USER_GENERAL_LOGIN_REQ kPacketReq;
					kPacketReq.m_wstrServiceAccountID = kPacket_.m_wstrID;
					kPacketReq.m_wstrIP = KncUtil::toWideString(KSession::GetIPStr());
					kPacketReq.m_iChannelingCode = kPacket_.m_iChannelingCode;
						kPacketReq.m_wstrPurchaseTok = L"";
					kPacketReq.m_bServerUseKogOTP = KSimLayer::GetKObj()->GetUseKogOTP();


					SendToAccountDB( DBE_CH_USER_GENERAL_LOGIN_REQ, kPacketReq );
				}
				else
				{
					// If you have passed public authentication, request KOG OTP
					RequestKOGOTPLogin(kPacket_);
				}
			}
			else
			{
				StateTransition( KCHFSM::I_TO_AUTHENTICATED );
				kPacket.m_iOK = NetError::NET_OK;
				SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacket );
			}

			return;
		}

		if( KSimLayer::GetKObj()->GetUseKogOTP() )
		{
			// - For countries using KOG OTP
			// In countries where KOG OTP is used, it is divided into two staff: Publisher Authentication -> KOG OTP Authentication.

			if (!kPacket_.m_bIDVerified)
			{
				m_wstrID = kPacket_.m_wstrID;	// When used with other authentication processes, it may be necessary to temporarily store ID.

				// Request public authentication
				RequestPublisherLogin(kPacket_);
			}
			else
			{
				// If public authentication has passed, request KOG OTP
				RequestKOGOTPLogin(kPacket_);
			}
		}
		else
		{
			m_wstrID = kPacket_.m_wstrID;	// When used with other authentication processes, it may be necessary to temporarily store ID.

			// - For countries that do not use KOG OTP
			// All you need is publisher certification.
			RequestPublisherLogin(kPacket_);
		}
	}


/*
IMPL_ON_FUNC( DBE_GASH_USER_LOGIN_ACK )
{
    VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

    KECH_VERIFY_ACCOUNT_ACK kPacket;
    kPacket.m_iOK = kPacket_.m_iOK;
    kPacket.m_iUserUID = kPacket_.m_iUserUID;
    kPacket.m_wstrPassport = kPacket_.m_wstrOTP;

    SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacket );

    if( kPacket_.m_iOK == NetError::NET_OK )
    {
        StateTransition( KCHFSM::I_TO_AUTHENTICATED );
    }
    else
    {
        ReserveDestroy();
    }

}
//*/

//{{ 2012. 07. 02	최육사		대기열 시스템
//////////////////////////////////////////////////////////////////////////

IMPL_ON_FUNC_NOPARAM( ECH_GET_CHANNEL_LIST_REQ )
{
	switch( GetStateID() )
	{
	case KCHFSM::S_LOGINED:
		break;

	case KCHFSM::S_WAIT_QUEUE:
		{
			KECH_GET_CHANNEL_LIST_ACK kPacketAck;
			kPacketAck.m_iOK = NetError::NET_OK;
			SendPacket( ECH_GET_CHANNEL_LIST_ACK, kPacketAck );
		}
		break;

	default:
		{
			START_LOG( cerr, L"State error. Name: " << m_strName )
				<< L"    Current state: " << GetStateIDString() << dbg::endl
				<< L"    Allowed state: KCHFSM::S_LOGINED" << END_LOG;
			return;
		}
		break;
	}

	KECH_GET_CHANNEL_LIST_ACK kPacketAck;
	kPacketAck.m_iOK = NetError::NET_OK;
	SendPacket( ECH_GET_CHANNEL_LIST_ACK, kPacketAck );

	// 현재 모드에 따른 동작
	switch( SiKWaitQueueManager()->GetMode() )
	{
	case KWaitQueueManager::WQ_NONE:
		{
			if( m_bPassedEnterChannel == false )
			{
				// 그냥 바로 등록!
				m_bPassedEnterChannel = true;
				SiKWaitQueueManager()->RegPassedUser( GetUID() );
			}
		}
		break;

	case KWaitQueueManager::WQ_CCU_LIMIT:
		{
			// 채널 리스트를 받을수 있도록 인증을 받았는가?
			if( m_bPassedEnterChannel == false )
			{
				// 일단 현재 동접이 얼만큼인지 채널이 입장할만큼 인원이 차있는지 본다!
				m_bPassedEnterChannel = SiKWaitQueueManager()->IsCanEnterChannel();
				if( m_bPassedEnterChannel == false )
				{
					// 해당 유저를 대기열에 등록한다!
					LIF( SiKWaitQueueManager()->RegWaitUser( GetUID() ) );

					// 해당 유저에게 팝업을 띄우기 위해 패킷 전송
					KECH_WAIT_QUEUE_MESSAGE_NOT kPacketNot;
					LIF( SiKWaitQueueManager()->GetWaitUserCount( GetUID(), kPacketNot.m_iWaitUserCount ) );
					SendPacket( ECH_WAIT_QUEUE_MESSAGE_NOT, kPacketNot );

					// 상태 이동
					StateTransition( KCHFSM::I_TO_WAIT_QUEUE );
					return;
				}
				else
				{
					// 통과한 유저 등록!
					SiKWaitQueueManager()->RegPassedUser( GetUID() );
				}
			}
		}
		break;

	case KWaitQueueManager::WQ_ENTRY_BAN:
		{
			// 해당 유저를 대기열에 등록한다!
			LIF( SiKWaitQueueManager()->RegWaitUser( GetUID() ) );

			// 해당 유저에게 팝업을 띄우기 위해 패킷 전송
			KECH_WAIT_QUEUE_MESSAGE_NOT kPacketNot;
			LIF( SiKWaitQueueManager()->GetWaitUserCount( GetUID(), kPacketNot.m_iWaitUserCount ) );
			SendPacket( ECH_WAIT_QUEUE_MESSAGE_NOT, kPacketNot );

			// 상태 이동
			StateTransition( KCHFSM::I_TO_WAIT_QUEUE );
			return;
		}
		break;

	default:
		{
			START_LOG( cerr, L"Unsupported mode!" )
				<< BUILD_LOG( SiKWaitQueueManager()->GetMode() )
				<< END_LOG;
		}
		return;
	}

	// 채널 리스트 전송
	SendChannelList();
}

//////////////////////////////////////////////////////////////////////////
//}}

IMPL_ON_FUNC_NOPARAM( ECH_DISCONNECT_REQ )
{
	//{{ 2012. 07. 02	최육사		대기열 시스템
	VERIFY_STATE( ( 3, KCHFSM::S_INIT, KCHFSM::S_LOGINED, KCHFSM::S_WAIT_QUEUE ) );
	//}}

	//{{ 2012. 07. 02	최육사		대기열 시스템
	if( GetStateID() == KCHFSM::S_WAIT_QUEUE )
	{
		if( SiKWaitQueueManager()->IsExist( GetUID() ) == true )
		{
			LIF( SiKWaitQueueManager()->UnRegWaitUser( GetUID() ) );
		}
	}
	//}}

	SendID( ECH_DISCONNECT_ACK );

	ReserveDestroy();
}

//{{ 2009. 12. 16  최육사	동접툴
IMPL_ON_FUNC( E_TOOL_GET_CCU_INFO_REQ )
{
	VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

	// 캐싱데이터 확인
	KE_TOOL_GET_CCU_INFO_ACK kPacketAck;
	if( SiKCCUToolManager()->GetExistCCU( kPacket_, kPacketAck ) == true )	// 요청한 날짜의 CCU 데이타가 있다
	{
		kPacketAck.m_iOK = NetError::NET_OK;
		SendPacket( E_TOOL_GET_CCU_INFO_ACK, kPacketAck );
		return;
	}

	KDBE_GET_CONCURRENT_USER_INFO_REQ kPacketToDB;
	kPacketToDB.m_cCCUType = kPacket_.m_cCCUType;
	kPacketToDB.m_iServerUID = kPacket_.m_iServerUID;
	kPacketToDB.m_wstrDate = kPacket_.m_wstrDate;
	SendToLogDB( DBE_GET_CONCURRENT_USER_INFO_REQ, kPacketToDB );
}

IMPL_ON_FUNC( DBE_GET_CONCURRENT_USER_INFO_ACK )
{
	VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

	KE_TOOL_GET_CCU_INFO_ACK kAck;
	kAck.m_iOK = kPacket_.m_iOK;
	kAck.m_cCCUType = kPacket_.m_cCCUType;
	kAck.m_iServerUID = kPacket_.m_iServerUID;
	kAck.m_wstrDate = kPacket_.m_wstrDate;
	kAck.m_vecCCUInfo = kPacket_.m_vecCCUInfo;

	CTime tCCUDate;

	if( KncUtil::ConvertStringToCTime( kPacket_.m_wstrDate, tCCUDate ) == false )
	{
		START_LOG( cerr, L"Failed to convert date!" )
			<< BUILD_LOG( kPacket_.m_iServerUID )
			<< BUILD_LOG( kPacket_.m_cCCUType )
			<< BUILD_LOG( kPacket_.m_wstrDate )
			<< END_LOG;
		return;
	}

	const KCCUToolManager::CCUDateData* mapCCUData = SiKCCUToolManager()->GetCCUData( tCCUDate );
	if( mapCCUData == NULL )
	{
		if( SiKCCUToolManager()->NewCCUData( kPacket_.m_iServerUID, tCCUDate.GetTime(), kPacket_.m_vecCCUInfo ) == false )
		{
			//[동접툴] 로그 삭제
			START_LOG( cerr, L"Failed to add concurrent user data; it may already exist!" )
				<< BUILD_LOG( kPacket_.m_iServerUID )
				<< BUILD_LOG( kPacket_.m_cCCUType )
				<< BUILD_LOG( kPacket_.m_wstrDate )
				<< END_LOG;

			return;
		}
	}
	else
	{
		if( SiKCCUToolManager()->SetCCUData( tCCUDate.GetTime(), kPacket_.m_iServerUID, kPacket_.m_vecCCUInfo, kPacket_.m_cCCUType ) == false )
		{
			return;
		}
	}

    SendPacket( E_TOOL_GET_CCU_INFO_ACK, kAck );
}
//}}

IMPL_ON_FUNC_NOPARAM( E_RESERVE_DESTROY )
{
	m_bDestroyReserved = true;
}



//{{ 2011. 01. 20 김민성	동접 모니터링 툴
IMPL_ON_FUNC( E_TOOL_CHECK_LOGIN_REQ )
{
	// ID, PW 둘 중 하나라도 특수 문자가 포함 되어 있다면
	if( KODBC::IsInvalidMarkIn( kPacket_.m_wstrID ) || KODBC::IsInvalidMarkIn( kPacket_.m_wstrPW ) )
	{
		START_LOG( cout, L"Disconnecting because the ID or password contains special characters!")
			<< BUILD_LOG( kPacket_.m_wstrID )
			<< BUILD_LOG( kPacket_.m_wstrPW )
			<< END_LOG;

		ReserveDestroy();
		return;
	}

	KDBE_GET_CONCURRENT_USER_LOGIN_REQ kPacketReq;

	kPacketReq.m_wstrID = kPacket_.m_wstrID;
	kPacketReq.m_wstrPW	= kPacket_.m_wstrPW;

 	SendToAccountDB(DBE_GET_CONCURRENT_USER_LOGIN_REQ, kPacketReq );
}

IMPL_ON_FUNC( DBE_GET_CONCURRENT_USER_LOGIN_ACK )
{
	KE_TOOL_CHECK_LOGIN_ACK Login_ACK;

	if( kPacket_.m_iAuthLevel < 0 )
	{
		START_LOG( cout, L"Disconnecting because login failed!")
		<< BUILD_LOG( kPacket_.m_iAuthLevel )
			<< END_LOG;
		ReserveDestroy();
		return;
	}

	Login_ACK.m_iAuthLevel = kPacket_.m_iAuthLevel;

	SendPacket( E_TOOL_CHECK_LOGIN_ACK, Login_ACK );
}

IMPL_ON_FUNC_NOPARAM( E_TOOL_SERVER_LIST_REQ )
{
	SendToLogDB( DBE_GET_GAME_SERVER_LIST_REQ );
}

IMPL_ON_FUNC( DBE_GET_GAME_SERVER_LIST_ACK )
{
	SendPacket( E_TOOL_SERVER_LIST_ACK, kPacket_ );
}

//}}

//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
//}}

//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
bool KChannelUser::SetMachineID( IN const std::string& strMachineID )
{
	if( m_kUserSecurityManager.CheckValidMachineID( strMachineID ) == true )
	{
		m_kUserSecurityManager.SetMachineID( strMachineID );
		return true;
	}

	return false;
}

std::string KChannelUser::GetMachineID()
{
	return m_kUserSecurityManager.GetStringMachineID();
}
//}}

//{{ 2012. 07. 02	최육사		대기열 시스템
void KChannelUser::SendChannelList()
{
	KECH_GET_CHANNEL_LIST_NOT kPacketNot;
	kPacketNot.m_iServerGroupID = KBaseServer::GetKObj()->GetServerGroupID();
	kPacketNot.m_mapChannelList = GetKChannelServer()->GetChannelList();
	//{{ 2012. 07. 2	박세훈	채널 UI 유동적으로 변경 가능한 시스템 ( Merge )
	kPacketNot.m_mapChannelBonusList = GetKChannelServer()->GetChannelBonusList();
	//}}
	SendPacket( ECH_GET_CHANNEL_LIST_NOT, kPacketNot );
}
//}}

//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
//}}

	IMPL_ON_FUNC( EPUBLISHER_AUTHENTICATION_ACK )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

		// Add special processing for each country (process net error values ??for each country) //
		bool bDoNotDisconnect = false;

			ProcessLoginResultCount( kPacket_.m_iOK );	// Process login authentication results (Countries that undergo publisher authentication are processed here)

		HandlePublisherLoginAck(kPacket_, bDoNotDisconnect);

		if (kPacket_.m_iOK == NetError::NET_OK) // Publisher authentication successful
		{
			KDBE_CH_USER_GENERAL_LOGIN_REQ kPacketReq;
			kPacketReq.m_wstrServiceAccountID = kPacket_.m_wstrID;
			kPacketReq.m_uiPublisherUID = kPacket_.m_uiPublisherUID;
			kPacketReq.m_wstrIP = KncUtil::toWideString(KSession::GetIPStr());
			kPacketReq.m_iChannelingCode = kPacket_.m_iChannelingCode;


				kPacketReq.m_wstrPurchaseTok = kPacket_.wstrPurchTok;

			kPacketReq.m_bServerUseKogOTP = KSimLayer::GetKObj()->GetUseKogOTP();


			SendToAccountDB( DBE_CH_USER_GENERAL_LOGIN_REQ, kPacketReq );
		}
		else	// Publisher authentication failed
		{
			KECH_VERIFY_ACCOUNT_ACK kPacketAck;
			kPacketAck.m_iOK = kPacket_.m_iOK;
			kPacketAck.m_wstrUserID = L"";
			//	kPacketAck.m_iUserUID = kPacket_.m_iUserUID;
			kPacketAck.m_wstrPassport = L"";

				kPacketAck.m_wstrPurchaseTok = L"";

				kPacketAck.m_strAgreementURL = kPacket_.m_strAgreementURL;

			// Client ACK Send //
			SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacketAck );

			if (bDoNotDisconnect)
			{
				// Does not hang up if the password is entered incorrectly.
				START_LOG( clog2, L"The ID or password is different, or the GF account has not been verified. (Do not disconnect)" );
			}
			else
			{
				ReserveDestroy();
			}
		}
	}

	IMPL_ON_FUNC( DBE_CH_USER_GENERAL_LOGIN_ACK )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

		KECH_VERIFY_ACCOUNT_ACK kPacketAck;

		kPacketAck.m_iOK = kPacket_.m_iOK;
		kPacketAck.m_wstrUserID = kPacket_.m_wstrUserID;
		kPacketAck.m_iUserUID = kPacket_.m_iUserUID;
		kPacketAck.m_wstrPassport = kPacket_.m_wstrOTP;
		kPacketAck.m_iChannelingCode = kPacket_.m_iChannelingCode;
		kPacketAck.m_wstrPurchaseTok = kPacket_.m_wstrPurchaseTok;
		CTime tCurrent = CTime::GetCurrentTime();
		kPacketAck.m_wstrCurrentTime = ( CStringW )tCurrent.Format( _T( "%Y-%m-%d %H:%M:%S" ) );


		START_LOG( clog, L"[Test log] Send packet token verified successfully" )
			<< BUILD_LOG( kPacketAck.m_iOK )
			<< BUILD_LOG( kPacketAck.m_wstrUserID )
			<< BUILD_LOG( kPacketAck.m_iUserUID )
			<< BUILD_LOG( kPacketAck.m_wstrPassport )
			<< BUILD_LOG( kPacketAck.m_wstrPurchaseTok )
			<< END_LOG;

		SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacketAck );

		// 사내 인증 또는 퍼블리셔 인증 서버로부터 인증받지 않고 자체 인증하는 경우(인도네시아, 필리핀)는 여기서 로그인 카운트 올림
		// 인도네시아, 필리핀은 EPUBLISHER_AUTHENTICATION_ACK 이벤트를 거치지 않기에 여기서 로그인 카운트 올림
		DWORD dwAuthFlag = KSimLayer::GetKObj()->GetAuthFlag();

		if( KSimLayer::AF_INTERNAL == dwAuthFlag )							// 사내 로그인 인증 결과 처리
		{
			ProcessLoginResultCount( kPacket_.m_iOK );
		}

		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			StateTransition( KCHFSM::I_TO_AUTHENTICATED );

			//{{ 2011.03.16  임규수 IP 블록 거래 차단
			//}}
		}
		else
		{
			ReserveDestroy();
		}
	}

	IMPL_ON_FUNC( DBE_CH_USER_KOGOTP_LOGIN_ACK )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

		// 국가별 특이 처리 있는 경우 사용
		HandleKOGOTPLoginAck();


		KECH_VERIFY_ACCOUNT_ACK kPacketAck;

		kPacketAck.m_iOK = kPacket_.m_iOK;
		kPacketAck.m_wstrUserID = kPacket_.m_wstrUserID;
		kPacketAck.m_iUserUID = kPacket_.m_iUserUID;
		kPacketAck.m_wstrPassport = kPacket_.m_wstrOTP;
		kPacketAck.m_wstrPurchaseTok = L"";	// 토큰 값은 GENERAL_LOGIN에서 받음. 여기선 NULL 세팅

		START_LOG( clog, L"[Test log] Send packet token verified successfully" )
			<< BUILD_LOG( kPacketAck.m_iOK )
			<< BUILD_LOG( kPacketAck.m_wstrUserID )
			<< BUILD_LOG( kPacketAck.m_iUserUID )
			<< BUILD_LOG( kPacketAck.m_wstrPassport )
			<< BUILD_LOG( kPacketAck.m_wstrPurchaseTok )
			<< END_LOG;

		SendPacket( ECH_VERIFY_ACCOUNT_ACK, kPacketAck );

		if( kPacket_.m_iOK == NetError::NET_OK )
		{
			StateTransition( KCHFSM::I_TO_AUTHENTICATED );
		}
		else
		{
			ReserveDestroy();
		}
	}

	IMPL_ON_FUNC( ECH_SECURITY_AUTH_REQ )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );

		KECH_SECURITY_AUTH_ACK kPacket;

		DWORD dwAuthFlag = KSimLayer::GetKObj()->GetAuthFlag();
		if (KSimLayer::AF_INTERNAL == dwAuthFlag)
		{
			StateTransition( KCHFSM::I_TO_AUTHENTICATED );
			kPacket.m_iOK = NetError::NET_OK;
			SendPacket( ECH_SECURITY_AUTH_ACK, kPacket );
		}

		RequestPublisherSecurityAuth(kPacket_);
	}

	IMPL_ON_FUNC( EPUBLISHER_SECURITY_AUTH_ACK )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );	// 인증전에 요청되기에 S_INIT임

		HandlePublisherSecurityAuthAck(kPacket_);
	}

	IMPL_ON_FUNC( EPUBLISHER_SECURITY_INFO_NOT )
	{
		VERIFY_STATE( ( 1, KCHFSM::S_INIT ) );	// 인증전에 요청되기에 S_INIT임

		HandlePublisherSecurityInfoNot(kPacket_);
	}

IMPL_ON_FUNC_NOPARAM( ECH_GET_SERVERGROUP_LIST_REQ )
{
	VERIFY_STATE( ( 1, KCHFSM::S_LOGINED ) );

	KECH_GET_SERVERGROUP_LIST_ACK kPacketAck;

	kPacketAck.m_iOK = NetError::NET_OK;
	SendPacket( ECH_GET_SERVERGROUP_LIST_ACK, kPacketAck );

	KECH_GET_SERVERGROUP_LIST_NOT kPacketNot;
	kPacketNot.m_mapServerGroupList = GetKChannelServer()->GetServerGroupList();
	SendPacket( ECH_GET_SERVERGROUP_LIST_NOT, kPacketNot );
}
//{{김준환 서버시간 받아오기


	void KChannelUser::ProcessLoginResultCount( IN const int iResult )
	{
		switch( iResult )
		{
		case NetError::NET_OK:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_SUCCESS);
			break;
		case NetError::ERR_VERIFY_21:	// 인증 서버 문제
		case NetError::ERR_GIANT_VERIFY_07:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_SYSTEM_ERROR);
			break;
		case NetError::ERR_GAMEFORGE_01:
		case NetError::ERR_GIANT_VERIFY_06:
		case NetError::ERR_BUY_CASH_ITEM_25:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_ACCOUNT_NOT_EXIST);
			break;
		case NetError::ERR_GAMEFORGE_00:
		case NetError::ERR_GIANT_VERIFY_05:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_PASSWORD_ERROR);
			break;
		case NetError::ERR_GIANT_VERIFY_02:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_OTP_PASSWORD_ERROR);
			break;
		case NetError::ERR_K3RCOMBO_AUTH_01:
		case NetError::ERR_GIANT_VERIFY_04:
		case NetError::ERR_GASH_17:
		case NetError::ERR_VERIFY_11:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_ACCOUNT_BLOCK);
			break;
		case NetError::ERR_GIANT_VERIFY_03:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_MATRIX_NUMBER_ERROR);
			break;
		case NetError::ERR_GASH_12:
		case NetError::ERR_GASH_13:
		case NetError::ERR_GASH_16:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_REJECT);
			break;
		case NetError::ERR_GAMEFORGE_02:
		case NetError::ERR_K3RCOMBO_AUTH_02:
		case NetError::ERR_UNKNOWN:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_ACCOUNT_NOT_ACTIVATE);
			break;
		default:
			GetKChannelServer()->PlusLoginResultCount(KLoginResultInfo::LRTE_FAIL_UNKNOWN);
			break;
		}
	}
