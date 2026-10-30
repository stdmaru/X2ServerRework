// KSession over loopback with the real network layer (KIocp + KIOThread on x2::platform::net, decision T20):
// accept, security key handshake (E_ACCEPT_CONNECTION_NOT), encrypted packets both ways, a packet larger than
// one socket read, the proxy mode without IOCP (event-driven session thread) and the disconnect.
#include "Event.h"
#include "Socket/IOCP.h"
#include "Socket/NetCommon.h"
#include "Socket/Session.h"
#include "CommonPacket.h"

#include <doctest/doctest.h>

#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace {

class KTestSession : public KSession
{
public:
    explicit KTestSession( bool bIsProxy ) : KSession( bIsProxy ) {}

    int GetPfID() override { return 0; }

    void ProcessEvent( const KEventPtr& spEvent ) override
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        m_vecEvents.push_back( spEvent );
    }

    // runs the queued events (the server does it from its own threads)
    std::vector<KEventPtr> Collect()
    {
        Tick();
        std::lock_guard<std::mutex> lock( m_mutex );
        return m_vecEvents;
    }

private:
    std::mutex m_mutex;
    std::vector<KEventPtr> m_vecEvents;
};

struct Listener
{
    SOCKET sock = INVALID_SOCKET;
    unsigned short usPort = 0;

    Listener()
    {
        sock = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
        SOCKADDR_IN addr = SOCKADDR_IN();
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl( INADDR_LOOPBACK );
        REQUIRE( ::bind( sock, reinterpret_cast<SOCKADDR*>( &addr ), sizeof( addr ) ) == 0 );
        REQUIRE( ::listen( sock, 4 ) == 0 );
        x2_socklen_t len = sizeof( addr );
        REQUIRE( ::getsockname( sock, reinterpret_cast<SOCKADDR*>( &addr ), &len ) == 0 );
        usPort = ntohs( addr.sin_port );
    }
    ~Listener() { CLOSE_SOCKET( sock ); }
};

// what NetLayer does with an accepted socket. Runs on another thread: no doctest assertions here.
std::shared_ptr<KTestSession> AcceptSession( Listener& listener )
{
    SOCKADDR_IN remote = SOCKADDR_IN();
    x2_socklen_t len = sizeof( remote );
    SOCKET sock = ::accept( listener.sock, reinterpret_cast<SOCKADDR*>( &remote ), &len );
    if( sock == INVALID_SOCKET )
        return nullptr;

    auto spServer = std::make_shared<KTestSession>( false );
    spServer->Init( true );
    KSocketObjectPtr spSockObj = spServer->GetSockObj();
    spSockObj->SetSocketInfo( sock, remote );
    if( !SiKIocp()->AssociateSocket( spSockObj ) )
        return nullptr;
    spSockObj->InitRecv();
    spSockObj->OnAcceptConnection();    // sends the security key
    return spServer;
}

template <typename F>
bool WaitFor( F&& done )
{
    for( int i = 0; i < 300 && !done(); ++i )
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    return done();
}

bool HasEvent( const std::vector<KEventPtr>& events, unsigned short usEventID )
{
    for( const KEventPtr& sp : events )
        if( sp->m_usEventID == usEventID )
            return true;
    return false;
}

struct IoThreads
{
    IoThreads()
    {
        NetCommon::InitWinSock();
        SiKIocp()->GetIoService().Restart();
        SiKIocp()->Init( 2 );
        SiKIocp()->BeginThread();
    }
    ~IoThreads()
    {
        SiKIocp()->EndThread();
        KIocp::ReleaseInstance();
    }
};

void Exchange( bool bClientUsesIocp )
{
    Listener listener;
    std::shared_ptr<KTestSession> spServer;
    std::thread accepter( [&] { spServer = AcceptSession( listener ); } );

    auto spClient = std::make_shared<KTestSession>( true );
    spClient->Init( bClientUsesIocp );
    const bool bConnected = spClient->Connect( "127.0.0.1", listener.usPort ); // waits for the security key
    accepter.join();
    REQUIRE( bConnected );
    REQUIRE( spServer );

    // client -> server, encrypted with the negotiated key
    KEGS_CONNECT_REQ kReq;
    kReq.m_wstrVersion = L"가나다";
    REQUIRE( spClient->SendPacket( PI_NULL, 0, NULL, EGS_CONNECT_REQ, kReq ) );
    REQUIRE( WaitFor( [&] { return HasEvent( spServer->Collect(), EGS_CONNECT_REQ ); } ) );
    for( const KEventPtr& sp : spServer->Collect() )
    {
        if( sp->m_usEventID != EGS_CONNECT_REQ )
            continue;
        KEGS_CONNECT_REQ kGot;
        KSerializer ks;
        ks.BeginReading( &sp->m_kbuff );
        REQUIRE( ks.Get( kGot ) );
        CHECK( kGot.m_wstrVersion == kReq.m_wstrVersion );
    }

    // server -> client, larger than one read (MAX_PACKET_SIZE is 32 KB) and not compressed
    KEGS_CONNECT_REQ kBig;
    kBig.m_wstrVersion.assign( 12000, L'x' );   // 24 KB of UTF-16
    REQUIRE( spServer->SendPacket( PI_NULL, 0, NULL, EGS_CONNECT_ACK, kBig, true, false ) );
    REQUIRE( WaitFor( [&] { return HasEvent( spClient->Collect(), EGS_CONNECT_ACK ); } ) );

    // the client closes: the server sees the socket error and reserves its destruction
    spClient->GetSockObj()->CloseSocket();
    REQUIRE( WaitFor( [&] { return !spServer->IsConnected() || spServer->GetDisconnectReason() != 0; } ) );
}

} // namespace

TEST_CASE( "KSession over loopback, proxy on the I/O threads (IOCP mode)" )
{
    IoThreads threads;
    Exchange( true );
}

TEST_CASE( "KSession over loopback, proxy with its own thread (no IOCP)" )
{
    IoThreads threads;
    Exchange( false );
}
