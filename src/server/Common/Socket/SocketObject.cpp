#include "SocketObject.h"
#include "Socket/NetCommon.h"
#include "IOCP.h"

#include <algorithm>

#include <dbg/dbg.hpp>
#include <Thread/Locker.h>

#include "DisconnectReason.h"

KSocketObject::KMaxSizeInfo KSocketObject::ms_kMaxSizeInfo = {
    KncCriticalSection(), 0, 0, CTime::GetCurrentTime()
};

void KSocketObject::KMaxSizeInfo::Check( int iDataSize, int iQueueSize )
{
    KLocker lock( m_cs );

    if( iDataSize > m_iMaxDataSize )
    {
        m_iMaxDataSize  = iDataSize;
        m_iQueueSize    = iQueueSize;
        m_cTime         = CTime::GetCurrentTime();

        START_LOG( cout, L"Send-Queue log update." )
            << BUILD_LOG( m_iMaxDataSize )
            << BUILD_LOG( m_iQueueSize )
            << BUILD_LOG( (LPCTSTR)m_cTime.Format( KNC_TIME_FORMAT ) );
    }

}

void KSocketObject::KMaxSizeInfo::Dump( std::wostream& stm )
{
    KLocker lock( m_cs );

    stm << L" -> Send Q. log " << std::endl
        << TOSTRINGW( m_iMaxDataSize )
        << TOSTRINGW( m_iQueueSize )
        << TOSTRINGW( (LPCTSTR)m_cTime.Format( KNC_TIME_FORMAT ) );
}

KSocketObject::KSocketObject() : m_dwKey( 0 )
{
    m_sockaddr = SOCKADDR_IN();
    m_sockaddr.sin_family   = AF_INET;

    m_ovlRecv.m_eIOMode     = KOVERLAPPED::IO_RECV;
    m_ovlSend.m_eIOMode     = KOVERLAPPED::IO_SEND;

	m_iDisconnectReason		= KDisconnectReason::eSIColDR_No_Reason;
	//{{ 2009-08-31 IOCP disconnection reason
	m_dwLastIocpFailedError = 0;
	//}}

    ::InitializeCriticalSection( &m_csSock );
    ::InitializeCriticalSection( &m_csSendQueue );
}

void KSocketObject::SetIOEvent( KOVERLAPPED::ENUM_IO_MODE eMode_, HANDLE hEvent_ )
{
    // 2006-02-27 florist: there used to be a GetIOEvent that created the event internally. But the
    // WaitFor... happens outside the socket object, so deleting the event at shutdown while the thread still
    // runs caused trouble. The thread's code now creates and destroys the event; this only receives it.

    switch( eMode_ )
    {
    case KOVERLAPPED::IO_SEND: m_ovlSend.hEvent = hEvent_;  break;
    case KOVERLAPPED::IO_RECV: m_ovlRecv.hEvent = hEvent_;  break;
    default:
        return;
    }
}


KSocketObject::~KSocketObject(void)
{
    { // locking scope
        KLocker lock( m_csSock );

        CloseStream();
    } // locking scope

    DeleteCriticalSection( &m_csSock );
    DeleteCriticalSection( &m_csSendQueue );
}

void KSocketObject::CloseStream()
{
    if( m_spStream )
    {
        m_spStream->Close();    // pending operations complete as aborted
        m_spStream.reset();
    }
}

void KSocketObject::CloseSocket()
{
    KLocker lock( m_csSock );

    CloseStream();
}

void KSocketObject::SetSocketInfo( SOCKET socket_, const SOCKADDR_IN& sockAddr_ )
{
    KLocker lock( m_csSock );

    CloseStream();
    m_sockaddr = sockAddr_;

    auto spStream = std::make_shared< x2::platform::net::TcpStream >( SiKIocp()->GetIoService() );
    int iError = 0;
    if( !spStream->Assign( socket_, &iError ) )
    {
        START_LOG( cerr, L"failed to take the accepted socket" )
            << BUILD_LOG( x2::platform::net::SocketErrorMessage( iError ) )
            << END_LOG;
        CLOSE_SOCKET( socket_ );
        return;
    }
    m_spStream = spStream;
}

bool KSocketObject::Connect( const char* szIP_,
                             unsigned short usPort_,
                             bool bUseIocp_,
                             std::vector< std::pair<int,int> >* vecOpt_ )
{
    KLocker lock( m_csSock );

    if( IsConnected() )
    {
        START_LOG( cwarn, L"Already Connected. Key : " << m_dwKey );
        return true;
    }

    auto spStream = std::make_shared< x2::platform::net::TcpStream >( SiKIocp()->GetIoService() );
    int iError = 0;
    if( !spStream->Open( &iError ) )
    {
        START_LOG( cerr, x2::platform::net::SocketErrorMessage( iError ) )
            << BUILD_LOG( iError )
            << END_LOG;
        return false;
    }

    if( vecOpt_ != NULL )
    {
        std::vector< std::pair<int,int> >::iterator vit;
        for( vit = vecOpt_->begin(); vit != vecOpt_->end(); vit++ )
        {
            if( !spStream->SetOption( vit->first, vit->second, 1 ) )
            {
                START_LOG( cerr, GET_WSA_MSG )
                    << BUILD_LOG( WSAGetLastError() )
                    << END_LOG;
                return false;
            }
        }
    }

    if( szIP_ )         SetRemoteIP( szIP_ );
    if( usPort_ != 0 )  SetRemotePort( usPort_ );

    if( !spStream->Connect( m_sockaddr, &iError ) )
    {
        // MUST: sometimes another machine (meeting room) gets WSAENOBUFS, apparently when TCP runs out of
        // buffer space: with 100 clients, the last 5 or so fail. To be fixed some day.
        START_LOG( cerr, x2::platform::net::SocketErrorMessage( iError ) )
            << BUILD_LOG( iError )
            << BUILD_LOG( m_dwKey )
            << BUILD_LOG( szIP_ )
            << BUILD_LOG( usPort_ )
            << END_LOG;
        return false;
    }
    m_spStream = spStream;

    KSocketObjectPtr spSockObj = GetThisPtr< KSocketObject >();
    if( bUseIocp_ )
    {
        _JIF( SiKIocp()->AssociateSocket( spSockObj ), return false );
    }

    _JIF( spSockObj->InitRecv(), return false );

    return  true;
}

void KSocketObject::OnSocketError()
{
    START_LOG( cout, L"pure-virtual function called." );
}

// Port: what the IOCP delivered. With an event set (no IOCP), the result is stored and the event signaled;
// the waiting thread then calls OnIOCompleted(). Otherwise it goes to KIocp like a dequeued completion packet:
// data or a peer close (0 bytes) to OnIOCompleted(), a failure (including the local close) to OnClosedByLocal().
void KSocketObject::OnAsyncCompleted( KOVERLAPPED& kOvl_, const x2::platform::net::IoResult& kResult_ )
{
    using x2::platform::net::IoStatus;

    if( kOvl_.hEvent != NULL )
    {
        kOvl_.m_kResult = kResult_;
        ::SetEvent( kOvl_.hEvent );
        return;
    }

    if( kResult_.status == IoStatus::Ok || kResult_.status == IoStatus::Eof )
    {
        SiKIocp()->OnIOCompleted( m_dwKey, static_cast<DWORD>( kResult_.bytes ), &kOvl_ );
    }
    else
    {
        SiKIocp()->OnClosedByLocal( m_dwKey, kResult_.error );
    }
}

bool KSocketObject::InitRecv()
{
    KLocker lock( m_csSock );

    if( !m_spStream ) return false;

    m_ovlRecv.Clear();

    // the handler keeps the object (and its buffer) and the stream alive until the operation completes
    KSocketObjectPtr spSelf = GetThisPtr< KSocketObject >();
    std::shared_ptr< x2::platform::net::TcpStream > spStream = m_spStream;
    m_spStream->AsyncReadSome( &m_ovlRecv.m_pBuffer[m_ovlRecv.m_dwLeft],
        MAX_PACKET_SIZE - m_ovlRecv.m_dwLeft,
        [spSelf, spStream]( const x2::platform::net::IoResult& kResult ) {
            spSelf->OnAsyncCompleted( spSelf->m_ovlRecv, kResult );
        } );
    return true;
}

void KSocketObject::OnIOCompleted( KOVERLAPPED::ENUM_IO_MODE eMode_ )
{
    using x2::platform::net::IoStatus;

    KOVERLAPPED* pkOvl = NULL;
    switch( eMode_ )
    {
    case KOVERLAPPED::IO_SEND: pkOvl = &m_ovlSend;   break;
    case KOVERLAPPED::IO_RECV: pkOvl = &m_ovlRecv;   break;
    default:
        return;
    }

    // a failed operation reports 0 bytes, as WSAGetOverlappedResult() did
    DWORD dwTransfered = 0;
    { // locking scope
        KLocker lock( m_csSock );
        if( pkOvl->m_kResult.status == IoStatus::Ok )
            dwTransfered = static_cast<DWORD>( pkOvl->m_kResult.bytes );
    } // locking scope

    if( eMode_ == KOVERLAPPED::IO_SEND )
        OnSendCompleted( dwTransfered );
    else
        OnRecvCompleted( dwTransfered );
}

void KSocketObject::MovDataQue2Buf()
{
    KLocker lock( m_csSock );
    KLocker lock2( m_csSendQueue );

    int nSize = MAX_PACKET_SIZE - m_ovlSend.m_dwLeft;
    nSize = m_kSendQueue.Pop( nSize, &m_ovlSend.m_pBuffer[m_ovlSend.m_dwLeft] );
    m_ovlSend.m_dwLeft += nSize;
}

void KSocketObject::StartSend()
{
    m_ovlSend.m_bSending = true;

    KSocketObjectPtr spSelf = GetThisPtr< KSocketObject >();
    std::shared_ptr< x2::platform::net::TcpStream > spStream = m_spStream;
    m_spStream->AsyncWriteSome( m_ovlSend.m_pBuffer, m_ovlSend.m_dwLeft,
        [spSelf, spStream]( const x2::platform::net::IoResult& kResult ) {
            spSelf->OnAsyncCompleted( spSelf->m_ovlSend, kResult );
        } );
}

void KSocketObject::OnSendCompleted( DWORD dwTransfered_ )
{
    KLocker lock( m_csSock );

    if( !m_spStream ) return;

    m_ovlSend.m_bSending    = false;    // 2005-01-27: always false, sent all or not; set again below when resending

    if( dwTransfered_ >= m_ovlSend.m_dwLeft )   // everything requested was sent
    {
        m_ovlSend.m_dwLeft      = 0;
    }
    else    // partly sent: send the rest
    {
        // Port: the original subtracted OVERLAPPED::InternalHigh, which holds the same byte count
        m_ovlSend.m_dwLeft -= dwTransfered_;
        memmove( &m_ovlSend.m_pBuffer[0], &m_ovlSend.m_pBuffer[dwTransfered_], m_ovlSend.m_dwLeft );
    }

    MovDataQue2Buf();   // move data waiting in the send queue into the buffer

    if( m_ovlSend.m_dwLeft == 0 )   // nothing left to send
        return;

    StartSend();
}

bool KSocketObject::SendData( const char* szData_, int iSize_ )
{
    KLocker lock( m_csSock );

    _JIF( m_spStream, return false );                  // invalid socket
    _JIF( iSize_ > 0, return false );                  // sending 0 bytes is an error too
    _JIF( szData_ != NULL, return false );

    MovDataQue2Buf(); // move data waiting in the send queue into the buffer

    // input data, 1: copy into the buffer while it has room
    int iFreeSpace = MAX_PACKET_SIZE - m_ovlSend.m_dwLeft;
    if( iFreeSpace > 0 )
    {
        int iCopySize = std::min( iFreeSpace, iSize_ );
        ::memcpy( &m_ovlSend.m_pBuffer[m_ovlSend.m_dwLeft], szData_, iCopySize );
        m_ovlSend.m_dwLeft += iCopySize;
        szData_ += iCopySize;
        iSize_ -= iCopySize;
    }

    // input data, 2: the rest goes to the send queue
    if( iSize_ > 0 )
    {
        KLocker lock2( m_csSendQueue );

        iSize_ -= m_kSendQueue.Push( szData_, iSize_ );

        ms_kMaxSizeInfo.Check( m_kSendQueue.GetDataSize(), m_kSendQueue.size() );
    }

    // input data, 3: whatever still did not fit means the buffer is full
    if( iSize_ > 0 )
    {
        START_LOG( cerr, L"Send Buffer Full. data size : " << iSize_
            << L", left : " << m_ovlSend.m_dwLeft
            << L", bSending : " << m_ovlSend.m_bSending );

        // nothing was sent: SendPacket has to be called again
        m_ovlSend.m_dwLeft = 0;
        m_ovlSend.m_bSending = false;

		SetDisconnectReason( KDisconnectReason::eSIColDR_SendBufferFull );
        OnSocketError();

        return false;
    }

    // not sending right now: start it
    if( m_ovlSend.m_bSending == false )
        StartSend();

    return true;
}

void KSocketObject::SetDisconnectReason( int iDisconnectReason )
{
	if( m_iDisconnectReason == KDisconnectReason::eSIColDR_No_Reason )
	{
		m_iDisconnectReason = iDisconnectReason;
	}
	else
	{
		START_LOG( cwarn, L"a new DisconnectReason arrived" )
			<< BUILD_LOG( m_iDisconnectReason )
			<< BUILD_LOG( iDisconnectReason )
			<< END_LOG;
	}
}
