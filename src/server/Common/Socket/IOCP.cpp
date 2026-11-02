#include "SocketObject.h"
#include "IOCP.h"
#include "dbg/dbg.hpp"
#include "IOThread.h"
#include "KncSend.h"    // PI_NULL
#include "Statistics.h"

ImplementSingleton( KIocp );
NiImplementRTTI( KIocp, KThreadManager );
ImplPfID( KIocp, PI_NULL );   // the IOCP does not use a PfID

KIocp::KIocp()
:
m_dwTerm(0L),
m_dwSockObjNum(0L)
{
    m_cTime = CTime::GetCurrentTime();
}

KIocp::~KIocp()
{
    m_kIoService.Stop();
    KThreadManager::EndThread( 3000 );  // the I/O threads use m_kIoService: join them before it goes away

    // close what is still registered, then run the resulting completions before the members go away
    std::map< DWORD, KSocketObjectPtr > mapSockObj;
    {
        KLocker lock( m_csSockObj );
        mapSockObj.swap( m_mapSockObj );
    }
    for( auto& kPair : mapSockObj )
    {
        if( kPair.second )
            kPair.second->CloseSocket();
    }
    mapSockObj.clear();
    m_kIoService.Drain();
}

void KIocp::EndThread()
{
    // the original posted one SIGNAL_SHUTDOWN_SERVER packet per I/O thread
    PostStatus( SIGNAL_SHUTDOWN_SERVER );
}

bool KIocp::AssociateSocket( KSocketObjectPtr spSockObj )
{
    _JIF( spSockObj, return false );

    DWORD dwCompKey = GenNewCompKey();

    if( !spSockObj->IsConnected() )
        return false;

    KLocker lock( m_csSockObj );

    m_mapSockObj.insert( std::make_pair( dwCompKey, spSockObj ) );
    spSockObj->SetKey( dwCompKey );
    return true;
}

bool KIocp::PostStatus( ULONG_PTR CompKey )
{
    _JIF( CompKey == SIGNAL_SHUTDOWN_SERVER, return false );
    m_kIoService.Stop();    // every KIOThread leaves RunFor() and ends
    return true;
}

bool KIocp::DeleteCompKey( IN DWORD dwKey_ )
{
    KLocker lock( m_csSockObj );
    std::map< DWORD, KSocketObjectPtr >::iterator mit = m_mapSockObj.find( dwKey_ );
    if( mit == m_mapSockObj.end() )
    {
        START_LOG( cerr, L"No session exists for the completion key being removed." )
            << BUILD_LOG( dwKey_ )
            << END_LOG;

        return false;
    }

    if( !mit->second )
    {
        START_LOG( cerr, L"Invalid socket object pointer registered with IOCP." )
            << BUILD_LOG( mit->first )
            << END_LOG;
    }
    else
    {
        mit->second->SetKey( 0 );
    }

    m_mapSockObj.erase( mit );
    return true;
}

void KIocp::OnIOCompleted( IN DWORD dwKey_, IN DWORD dwBytesTransfer_, IN KOVERLAPPED* povl_ )
{
    if( povl_ == NULL )
    {
        START_LOG( cerr, L"IOCP Error" );
        return;
    }

    KOVERLAPPED* pkOvl = povl_;

    KSocketObjectPtr spSockObj = GetSockObj( dwKey_ );

    if( !spSockObj )
    {
        START_LOG( cwarn, L"Could not find the session for a socket whose I/O has ended. Key: " << dwKey_ )
            << BUILD_LOG( dwBytesTransfer_ )
            << END_LOG;
        return;
    }

    switch( pkOvl->m_eIOMode ) {
        case KOVERLAPPED::IO_RECV:
            if( &spSockObj->m_ovlRecv != pkOvl )
            {
                START_LOG( cerr, L"diffrent CompKey. (recv) key : " << dwKey_ );
            }
            spSockObj->OnRecvCompleted( dwBytesTransfer_ );
            break;
        case KOVERLAPPED::IO_SEND:
            if( &spSockObj->m_ovlSend != pkOvl )
            {
                START_LOG( cerr, L"diffrent CompKey. (send) key : " << dwKey_ );
            }
            spSockObj->OnSendCompleted( dwBytesTransfer_ );
            break;
    }
}

void KIocp::OnClosedByLocal( IN DWORD dwKey_, IN int iError_ )
{
    KSocketObjectPtr spSockObj = GetSockObj( dwKey_ );

    if( spSockObj )
    {
		//////////////////////////////////////////////////////////////////////////
		//{{ 2009-08-31 find the cause of the failure
		spSockObj->SetLastIocpFailedError( static_cast<DWORD>( iError_ ) );
		//}}
		//////////////////////////////////////////////////////////////////////////

        spSockObj->SetDisconnectReason( KStatistics::eSIColDR_IOCP_GetStatus );
        spSockObj->OnSocketError();
    }
    else
    {
        START_LOG( clog, L"Socket object not found; session object has been destroyed." )
            << BUILD_LOG( dwKey_ )
            << END_LOG;
    }
}

size_t KIocp::GetSockObjNum() const
{
    KLocker lock( m_csSockObj );
    return m_mapSockObj.size();
}

KSocketObjectPtr KIocp::GetSockObj( DWORD dwKey_ )
{
    KLocker lock( m_csSockObj );

    std::map< DWORD, KSocketObjectPtr >::iterator mit;

    mit = m_mapSockObj.find( dwKey_ );

    if( mit == m_mapSockObj.end() )
    {
        START_LOG( clog2, L"Could not find the requested socket pointer. Key: " << dwKey_ )
            << BUILD_LOG( m_mapSockObj.size() )
            << END_LOG;
        return KSocketObjectPtr();
    }

    return mit->second;
}

KThread* KIocp::CreateThread()
{
    return new KIOThread;
}

#include <random>

DWORD KIocp::GenNewCompKey() const
{
    // Port: the original used an unseeded generator through boost::variate_generator; keys only need to be
    // unique among the registered sockets, so a seeded std::mt19937 is used.
    static std::mt19937 rng( std::random_device{}() );
    static std::uniform_int_distribution<DWORD> uint32( 1, UINT_MAX );

    KLocker lock( m_csSockObj );

    for(;;)
    {
        const DWORD dwCompKey = uint32( rng );

        if( m_mapSockObj.find( dwCompKey ) == m_mapSockObj.end() )
            return dwCompKey;
    }
}

ImplToStringW( KIocp )
{
    START_TOSTRING_PARENTW( KThreadManager );

    size_t sockobjNum = GetSockObjNum();

    KLocker lock( m_cs );
    return stm_ << L" -- Max IOCP INFO --" << std::endl
        << L"  Current SockObj NUM : " << sockobjNum << std::endl
        << L"  Key Gen Term        : " << KIocp::m_dwTerm << std::endl
        << L"  Occured Time        : " << ( const wchar_t* )KIocp::m_cTime.Format( _T( "%Y-%m-%d %H:%M:%S" ) ) << std::endl
        << L"  socket object NUM   : " << KIocp::m_dwSockObjNum << std::endl;
}
