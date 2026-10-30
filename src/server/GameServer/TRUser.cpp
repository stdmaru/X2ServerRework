#include "TRUser.h"
//#include "TRIOCP.h"
#include "Socket/IOCP.h"
#include <cassert>
#include <KncSecurity/KncSecurity.h>
//#include "RelayData.h"

#include "TRServer.h"
#include "TRUserFSM.h"

#include "Socket/NetCommon.h"
#include "Statistics.h"

//////////////////////////////////////////////////////////////////////////
//des : KSkTRUser

void KSkTRUser::OnRecvCompleted( DWORD dwTransfered_ )
{
    if( dwTransfered_ == 0 )
    {
        START_LOG( clog, L"closed by remote machine" );
        OnSocketError();
        return;
    }

    USHORT usPacketLen = 0;
    m_ovlRecv.m_dwLeft += dwTransfered_;

    // 패킷 길이 진행량보다 크다면 계속 새 패킷 가능성을 체크한다.
    while( m_ovlRecv.m_dwLeft >= sizeof( usPacketLen ) )
    {
        // 패킷 길이를 알아낸다.
        memcpy( &usPacketLen, m_ovlRecv.m_pBuffer, sizeof( usPacketLen ) );

        //패킷은 최소 3byte 이상의 길이를 가져야 한다.
        if( usPacketLen < MINIMUM_PACKET_SIZE )
        {
            START_LOG( cerr, L"invalid packet size - " << usPacketLen );
            OnSocketError();
            return;
        }

        // 버퍼에 받아둔 데이터로 완전한 packet이 가능한 경우 다음으로 진행한다.
        if( m_ovlRecv.m_dwLeft >= usPacketLen )
        {
            //{{AFX
            // get recvCnt
            //char cRecvCnt = -99; // 사용하지 않는 값으로 초기화.
            //memcpy( &cRecvCnt, &m_ovlRecv.m_pBuffer[ sizeof(usPacketLen) ], sizeof(cRecvCnt) );

            // get (recvUIDn, data)
            //KSerBuffer buffer;
            //buffer.Write( &m_ovlRecv.m_pBuffer[MINIMUM_PACKET_SIZE], usPacketLen - MINIMUM_PACKET_SIZE );
            //}}AFX

            KTRUserPtr spTRUser = GetTRUserPtr();
            if( spTRUser )
            {
                KSerBuffer buffer;
                buffer.Write( &m_ovlRecv.m_pBuffer[sizeof(usPacketLen)], usPacketLen - sizeof(usPacketLen) );

                spTRUser->OnRecvCompleted( buffer );
            }
            else
            {
                START_LOG( clog, L"TRUser 객체가 이미 지워짐." )
                    << END_LOG;
            }

            // buffer management
            m_ovlRecv.m_dwLeft -= usPacketLen;
            memmove(&m_ovlRecv.m_pBuffer[0], &m_ovlRecv.m_pBuffer[usPacketLen], m_ovlRecv.m_dwLeft );
        }
        else
        {
            break;     // 온전한 패킷을 구성하기에 충분한 데이터가 없음. loop를 빠져나간다.
        }
    } // while( m_ovlRecv.m_dwLeft >= sizeof(DWORD) )

    // recv function call
    LIF( InitRecv() );
}

void KSkTRUser::OnSocketError()
{
    KTRUserPtr spTRUser = GetTRUserPtr();
    if( spTRUser )
    {
        spTRUser->OnSocketError();
    }
    else
    {
        START_LOG( clog, L"TRUser 객체가 이미 지워짐." )
            << END_LOG;
    }
}

void KSkTRUser::OnAcceptConnection()
{
    KTRUserPtr spTRUser = GetTRUserPtr();
    if( spTRUser )
    {
        spTRUser->OnAcceptConnection();
    }
    else
    {
        START_LOG( clog, L"TRUser 객체가 이미 지워짐." )
            << END_LOG;
    }
}

KTRUserPtr KSkTRUser::GetTRUserPtr()
{
    KTRUserPtr spTRUser = m_pkTRUser.lock();

    if( spTRUser )
    {
        return spTRUser;
    }
    else
    {
        return KTRUserPtr();
    }
}

// Port: SendDataWithProfile removed: a copy of KSocketObject::SendData with commented-out profiler hooks, never called
//}}

NiImplementRootRTTI(KTRUser);

KTRUser::KTRUser(void)
:
m_spSockObj(new KSkTRUser),
m_iUnitUID(-1),
m_bDestroyReserved( false )
{
	for( int i = 0; i < TICKS_NUM; i++ )
	{
		m_adwTickCount[i] = ::GetTickCount();
	}
}

KTRUser::~KTRUser(void)
{
}

bool KTRUser::Init()
{
    _JIF( m_spSockObj, return false );
    m_spSockObj->m_pkTRUser = shared_from_this();
	return true;
}

void KTRUser::Tick()
{
	if( GetStateID() == KTRUserFSM::STATE_EXIT )
	{
		START_LOG( cout, "종료 상태에서 다시 Tick 으로 들어 왔다" );
		return ;
	}

	if( m_bDestroyReserved == true )    // 종료가 예약된 상태라면 다시한번 종료처리를 시도한다.
	{
		OnDestroy();            // derived class call
		return;
	}

	switch( GetStateID() )
	{
	case KTRUserFSM::STATE_CONNECTED:

		if( ::GetTickCount() - GetTick( CONNECT_TICK ) > 15000) // Connect 이후 15초간 UID등록이 없다.
		{
			START_LOG( cwarn, L"Reguard " << m_iUnitUID
				<< L" as a Zombie.(Reason : Not Regist UID) (tick: "
				<< ::GetTickCount() - GetTick( CONNECT_TICK )
				<< L" )" );

			ReserveDestroy();
		}
		break;

	case KTRUserFSM::STATE_REGISTERING:
        {
            KTRUserPtr spTRUser = shared_from_this();
		    if( !SiKTRServer()->GetTRUserManager().Insert( spTRUser ) )     // 등록한다.
		    {
			    START_LOG( cwarn, L"Unit UID 등록실패.!" )
				    << BUILD_LOG( m_iUnitUID )
				    << END_LOG;

			    ReserveDestroy();
		    }
		    else
			    StateTransition( KTRUserFSM::INPUT_END_REG_UID );
        }
		break;

	case KTRUserFSM::STATE_REGISTERED:
		//로봇 테스트에서는 뺀다.
		if( ::GetTickCount() - GetTick( HB_TICK ) > 600000 ) // 60s 단위로 heart-bit check
		{
			START_LOG( cwarn, L"Reguard " << m_iUnitUID
				<< L" as a Zombie.(TRUser) (tick: "
				<< ::GetTickCount() - GetTick( HB_TICK )
				<< L")" );

			ReserveDestroy();
		}

		break;

	case KTRUserFSM::STATE_EXIT:
		break;
	} // end switch
}
//{{AFX
//bool KTRUser::Send( IN const std::vector<unsigned char>& buff_ )
//{
//	KncSecurity::KByteStream buff;
//	// 패킷 총 사이즈 + 릴레이 패킷을 의미하는 recvCnt + 전송할 데이터
//	buff.Append( sizeof(USHORT) + sizeof(char) + buff_.size(), sizeof(USHORT) );
//	buff.Append( EVENTID_RELAYED, sizeof(char) );
//	buff.append( &buff_[0] , buff_.size() );
//
//	return m_spSockObj->SendData( (const char*)buff.data(), buff.length() );
//}
//}}AFX

void KTRUser::OnSocketError()
{
	m_bDestroyReserved = true;
}

void KTRUser::OnAcceptConnection()
{
	SetTick( CONNECT_TICK );
}

void KTRUser::OnRecvCompleted( IN KSerBuffer& buff_ )
{
	KSerializer ks;
	KEventPtr spEvent( new KEvent );
	ks.BeginReading( &buff_ );
	ks.Get( *spEvent );
	ks.EndReading();

	spEvent->m_kbuff.UnCompress();

	dbg::clog << L"ⓡⓡ" << spEvent->GetIDStr() << L"TRU Recv" << END_LOG;

	switch( spEvent->m_usEventID )
	{
	case E_HEART_BEAT: //heart bit.
		break;

	case ETR_REG_UID:	// Regist UID. TRUserManager에 등록도 해야 함.
		{
			//일단 체크사항이므로 씨리얼 라이즈해서 UID 를 확인한다.
			KSerializer ks;
			UidType iUID;

			ks.BeginReading( &spEvent->m_kbuff );
			if( !ks.Get(iUID) )
			{
				START_LOG( cerr, L"deserialze failed.	ETR_REG_UID - UidType" );
			}
            ks.EndReading();

			if( m_iUnitUID != -1 )
			{
				START_LOG( cwarn, L"Duplicate UID regist. Current UID : " << m_iUnitUID
					<< L"Recv UID : " << iUID << dbg::endl );
				break;
			}

			if( iUID <= -1 )
			{
				START_LOG( cerr, L"Recv Invalid UID: " << iUID );
				break;
			}

			m_iUnitUID = iUID;
			START_LOG( clog, L"Regist UID : " << m_iUnitUID );

			StateTransition( KTRUserFSM::INPUT_START_REG_UID );
		}
		break;

	case ETR_UNIT_DISCONNECT:
		{
			START_LOG( clog, L"closed by unit disconnect.!" )
				<< BUILD_LOG( m_iUnitUID )
				<< END_LOG;

			ReserveDestroy();
			return;
		}
		break;

		//{{ 2012. 07. 18	최육사		재접속 로직
		//}}

	default:
		if( spEvent->m_usEventID < E_HEART_BEAT )
		{
			START_LOG( cerr, L"Invalide cRecvCnt field value. data size : " << spEvent->m_kbuff.GetLength() )
				<< BUILD_LOGc( spEvent->m_usEventID )
				<< END_LOG;
			return;
		}

		SiKTRServer()->QueueingRelayData( spEvent );
	}


	//{{AFX
	//switch( cRecvCnt_ )
	//{
	//case EVENTID_HEART_BIT: // heart bit.
	//	break;

	//case EVENTID_REG_UID:   // Regist UID. TRUserManager에 등록도 해야 한다.
	//	{
	//		UidType iUID;
	//		buff_.Read( &iUID, sizeof(UidType) );

	//		if( m_iUnitUID != 0 )
	//		{
	//			START_LOG( cerr, L"Duplicate UID regist. Current UID : " << m_iUnitUID
	//				<< L"Recv UID : " << iUID << dbg::endl );
	//			break;
	//		}

	//		if( iUID <= 0 )
	//		{
	//			START_LOG( cerr, L"Recv Invalid UID: " << iUID );
	//			break;
	//		}

	//		m_iUnitUID = iUID;
	//		START_LOG( clog, L"Regist UID : " << m_iUnitUID );

	//		StateTransition( KTRUserFSM::INPUT_START_REG_UID );
	//	}
	//	break;

	//default:
	//	if( cRecvCnt_ < 0 ) // 위에서 필터링 되지 않은 음수 영역. 문제가 있다.
	//	{
	//		START_LOG( cerr, L"Invalide cRecvCnt field value. data size : " << buff_.GetLength() )
	//			<< BUILD_LOGc( cRecvCnt_ )
	//			<< END_LOG;
	//		return;
	//	}

	//	if( buff_.GetLength() < cRecvCnt_ * sizeof(UidType) )
	//	{
	//		START_LOG( cerr, L"Invalide packet received. data size : " << buff_.GetLength() )
	//			<< BUILD_LOGc( cRecvCnt_ )
	//			<< END_LOG;
	//		OnSocketError();
	//	}

	//	KRelayDataPtr spData( new KRelayData );
	//	spData->m_cRecvCnt = cRecvCnt_;

	//	// cRecvCnt_에 담긴 수만큼 DWORD를 읽어낸다.
	//	spData->m_vecUID.resize( cRecvCnt_ );
	//	for( int i = 0 ; i < cRecvCnt_ ; ++i )
	//	{
	//		buff_.Read( &spData->m_vecUID[i], sizeof(UidType) );
	//	}

	//	spData->m_buff.resize( buff_.GetReadLength() );
	//	buff_.Read( &spData->m_buff[0], buff_.GetReadLength() );

	//	SiKTRServer()->QueueingRelayData( spData );
	//}
	//}}AFX

	// renew m_dwHBRecvTick
	SetTick( HB_TICK );
}

void KTRUser::OnDestroy()
{
	m_bDestroyReserved = false;     // 이전에 종료가 예약 되었든 아니든, 한번 Destory 호출이 성공하면 없어진다.

	switch( GetStateID() )
	{
	case KTRUserFSM::STATE_INIT:
		StateTransition( KTRUserFSM::INPUT_CONNECT );
		StateTransition( KTRUserFSM::INPUT_EXIT );
		break;
	case KTRUserFSM::STATE_CONNECTED:
		StateTransition( KTRUserFSM::INPUT_EXIT );
		break;
	case KTRUserFSM::STATE_REGISTERING:
		StateTransition( KTRUserFSM::INPUT_REG_UID_FAILED );
		StateTransition( KTRUserFSM::INPUT_EXIT );
		break;
	case KTRUserFSM::STATE_REGISTERED:
		StateTransition( KTRUserFSM::INPUT_EXIT );
		break;
	case KTRUserFSM::STATE_EXIT:
		break;
	}

    JIF( m_spSockObj );
    m_spSockObj->CloseSocket();
	//{{ 2009. 3. 13  최육사	그냥iocp쓴다
    //SiKTRIocp()->DeleteCompKey( m_spSockObj->GetKey() );
	SiKIocp()->DeleteCompKey( m_spSockObj->GetKey() );
	//}}
}

void KTRUser::SetSendQueueSize( int iQueueSize )
{
    JIF( m_spSockObj );
    m_spSockObj->SetSendQueueSize( iQueueSize );
}

bool KTRUser::SendPacket( const KncSecurity::KByteStream& kByteStream )
{
    _JIF( m_spSockObj, return false );
	//if( m_bDestroyReserved ) return true;///< 종료가 예약되었을때 send 시도. 이런 경우가 다분하므로, true 인정.
	_JIF( m_spSockObj->IsConnected(), return false );    ///< 소켓이 유효하지 않음.

	//////////////////////////////////////////////////////////////////////////
	//BEGIN_PROFILE( L"send size 통계" );
	//////////////////////////////////////////////////////////////////////////

	// send size
	//SiKTRServer()->UpdateSendPacketInfo( kByteStream.length() );

	//////////////////////////////////////////////////////////////////////////
	//END_PROFILE( L"send size 통계" );
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//BEGIN_PROFILE( L"SocketObject에서 SendData하기" );
	//////////////////////////////////////////////////////////////////////////

	//bool bRet = m_spSockObj->SendDataWithProfile( (const char*)kByteStream.data(), kByteStream.length() );
	bool bRet = m_spSockObj->SendData( (const char*)kByteStream.data(), kByteStream.length() );

	//////////////////////////////////////////////////////////////////////////
	//END_PROFILE( L"SocketObject에서 SendData하기" );
	//////////////////////////////////////////////////////////////////////////

	return bRet;
}
