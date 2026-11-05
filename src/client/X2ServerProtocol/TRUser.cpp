#include "StdAfx.h"

#include "TRUser.h"

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

    // while the buffer holds at least a length, look for another packet
    while( m_ovlRecv.m_dwLeft >= sizeof( usPacketLen ) )
    {
        memcpy( &usPacketLen, m_ovlRecv.m_pBuffer, sizeof(usPacketLen) );

        // a packet is at least 3 bytes long
        if( usPacketLen < MINIMUM_PACKET_SIZE )
        {
            START_LOG( cerr, L"invalid packet size - " << usPacketLen );
            OnSocketError();
            return;
        }

        // a whole packet has arrived
        if( m_ovlRecv.m_dwLeft >= usPacketLen )
        {
            KTRUserPtr spTRUser = GetTRUserPtr();
            if( spTRUser )
            {
                // the data without the length
                KSerBuffer buffer;
                buffer.Write( &m_ovlRecv.m_pBuffer[sizeof(usPacketLen)], usPacketLen - sizeof(usPacketLen) );

                spTRUser->OnRecvCompleted( buffer );
            }
            else
            {
                START_LOG( clog, L"the TRUser object is already gone." )
                    << END_LOG;
            }

            // buffer management
            m_ovlRecv.m_dwLeft -= usPacketLen;
            memmove(&m_ovlRecv.m_pBuffer[0], &m_ovlRecv.m_pBuffer[usPacketLen], m_ovlRecv.m_dwLeft );
        }
        else
        {
            break;     // not a whole packet yet
        }
    }

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
        START_LOG( clog, L"the TRUser object is already gone." )
            << END_LOG;
    }
}

KTRUserPtr KSkTRUser::GetTRUserPtr()
{
    return m_pkTRUser.lock();
}

NiImplementRootRTTI( KTRUser );

KTRUser::KTRUser(void)
:
m_spSockObj( new KSkTRUser ),
m_bDestroyReserved( false ),
m_iUnitUID( 0L ),
m_pSendGameMessage( NULL ),
m_ServerMsgID( 0 ),
m_iClassID( 0 )
{
	for( int i = 0; i < KTRUser::TICKS_NUM; i++ )
	{
		m_adwTickCount[i] = ::GetTickCount();
	}

	m_hEvents[EVENT_DISCONNECTED] = NULL;
	m_hEvents[EVENT_RECV_COMPLETED] = NULL;
	m_hEvents[EVENT_SEND_COMPLETED] = NULL;
}

KTRUser::~KTRUser(void)
{
	End( INFINITE );
	OnDestroy();

	if( m_hEvents[EVENT_RECV_COMPLETED] != NULL )
		::CloseHandle( m_hEvents[EVENT_RECV_COMPLETED] );
	if( m_hEvents[EVENT_SEND_COMPLETED] != NULL )
		::CloseHandle( m_hEvents[EVENT_SEND_COMPLETED] );
}

bool KTRUser::Begin()
{
    if( KThread::Begin() )
    {
        m_hEvents[EVENT_DISCONNECTED] = m_hKillEvent;
        return true;
    }

    return false;
}

void KTRUser::Tick()
{
    if( m_bDestroyReserved )    // destruction is reserved: try it again
    {
        OnDestroy();
        return;
    }

    //////////////////////////////////////////////////////////////////////////
    // heartbeat
    if( !IsConnected() )
        return;

    if( ::GetTickCount() - GetTick( KTRUser::HB_TICK ) <= 30000 ) // every 30 s
        return;

    JIF( m_spSockObj );
#if X2_DEV_TOOLS
    START_LOG( clog, L" Heart-bit " << m_iUnitUID << L"  :" << ::GetTickCount() - GetTick( KTRUser::HB_TICK ) );
#endif
    SetTick( KTRUser::HB_TICK );

    KncSecurity::KByteStream buff;
    buff.Append( sizeof(USHORT) + sizeof(char), sizeof(USHORT) );
    buff.Append( E_HEART_BEAT, sizeof(char) );

#if X2_DEV_TOOLS
	LIF( m_spSockObj->SendData( ( const char* )buff.data(), buff.length() ) );
#else
	m_spSockObj->SendData( ( const char* )buff.data(), buff.length() );
#endif
}

bool KTRUser::Init()
{
    _JIF( m_spSockObj, return false );
    m_spSockObj->m_pkTRUser = shared_from_this();

	m_hEvents[EVENT_RECV_COMPLETED] = CreateEvent( NULL, false, false, NULL );
	m_spSockObj->SetIOEvent( KOVERLAPPED::IO_RECV, m_hEvents[EVENT_RECV_COMPLETED] );
	m_hEvents[EVENT_SEND_COMPLETED] = CreateEvent( NULL, false, false, NULL );
	m_spSockObj->SetIOEvent( KOVERLAPPED::IO_SEND, m_hEvents[EVENT_SEND_COMPLETED] );

	return true;
}

bool KTRUser::Connect( const char* szIP_, unsigned short usPort_, const UidType& iUnitUID_ )
{
	std::vector< std::pair<int,int> > vecOpt;
	vecOpt.push_back( std::make_pair( SOL_SOCKET, SO_REUSEADDR ) ); // allow reusing an address in use
	vecOpt.push_back( std::make_pair( IPPROTO_TCP, TCP_NODELAY ) ); // no Nagle: send at once instead of gathering data

    _JIF( m_spSockObj, return false );
	// Port: the original wrote _JIF( ..., false ), which went on after a failed connection
	_JIF( m_spSockObj->Connect( szIP_, usPort_, false, &vecOpt ), return false );

	Begin();    // thread run

	// the UID is registered right after connecting
	m_iUnitUID = iUnitUID_;

	START_LOG( clog, L"Send UID : " << iUnitUID_ );

	return true;
}

bool KTRUser::SendID(unsigned short usEventID, bool bCompress_ /* = true  */)
{
    _JIF( m_spSockObj, return false );
	_JIF( m_spSockObj->IsConnected(), return false );    ///< invalid socket

	KEvent kEvent;
	kEvent.SetData( 0, NULL, usEventID );

	if( kEvent.m_kbuff.GetLength() > KSkTRUser::MAXIMUM_PACKET_SIZE ) return false;

	if( bCompress_ ) kEvent.m_kbuff.Compress();

	// serialize - full event object
	KSerializer     ks;
	KSerBuffer      kbuff;
	ks.BeginWriting( &kbuff );
	ks.Put( kEvent );
	ks.EndWriting();

	KncSecurity::KByteStream    bsbuff;
	bsbuff.reserve( kbuff.GetLength() + sizeof(USHORT) );
	bsbuff.Assign( kbuff.GetLength() + sizeof(USHORT), sizeof(USHORT) );
	bsbuff.append( (byte*)kbuff.GetData(), kbuff.GetLength() );   // the data to send

	if( kEvent.m_usEventID != E_HEART_BEAT )
		dbg::clog << L"ⓢⓢ " << kEvent.GetIDStr() << L" - TR SEND" << dbg::endl;

	return m_spSockObj->SendData( (const char*)bsbuff.data(), bsbuff.length() );
}

void KTRUser::OnSocketError()
{
    m_bDestroyReserved = true;
}

void KTRUser::Run()
{
	DWORD ret       = 0;
	bool bLoop      = true;

	while( bLoop )
	{
		ret = ::WaitForMultipleObjects( EVENT_MAX_VALUE, m_hEvents, false, 100 );     // every 0.1 s

		switch( ret )
		{
		case EVENT_DISCONNECTED + WAIT_OBJECT_0:
			bLoop = false;
			break;
		case EVENT_RECV_COMPLETED + WAIT_OBJECT_0:
			{
                _JIF( m_spSockObj, break );
				m_spSockObj->OnIOCompleted( KOVERLAPPED::IO_RECV );
			}
			break;
		case EVENT_SEND_COMPLETED + WAIT_OBJECT_0:
			{
                _JIF( m_spSockObj, break );
				m_spSockObj->OnIOCompleted( KOVERLAPPED::IO_SEND );
			}
			break;
		case WAIT_TIMEOUT:
			break;
		}

		Tick();
	}
}

void KTRUser::OnRecvCompleted( IN KSerBuffer& buffer_ )
{
    // the client needs no system events from the relay: everything goes to the game
    KSerializer ks;
    KEventPtr spEvent( new KEvent );
    ks.BeginReading( &buffer_ );
    ks.Get( *spEvent );
    ks.EndReading();

    spEvent->m_kbuff.UnCompress();

    dbg::clog << L"ⓡⓡ " << spEvent->GetIDStr() << L" - TRU Recv" << dbg::endl;

    if( m_pSendGameMessage != NULL )
    {
        KSerBuffer* pBuff = new KSerBuffer( spEvent->m_kbuff );   // the receiver deletes it
        m_pSendGameMessage(
        m_ServerMsgID,
        static_cast<WPARAM>(spEvent->m_usEventID),
        reinterpret_cast<LPARAM>(pBuff),
        false
        );
    }
}

void KTRUser::OnDestroy()
{
	m_bDestroyReserved = false;     // whether or not it was reserved, one successful OnDestroy is enough

	SetEvent( m_hKillEvent );
    if( m_spSockObj )
    {
        m_spSockObj->CloseSocket();
    }
    else
    {
        START_LOG( cerr, L"bad socket object pointer." )
            << END_LOG;
    }
}
