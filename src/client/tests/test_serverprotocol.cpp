// X2ServerProtocol over loopback (docs/porting/client-serverprotocol.md): the CX2ServerProtocol facade against a
// server-side KSession of the shared network core (decision T34), as a ChannelServer or GameServer accepts it, and
// against a raw TCP relay. Checks the handshake (E_ACCEPT_CONNECTION_NOT), the client's encryption (its own SA
// database, *_Local) accepted by the server's (with the sequence number check on), a packet larger than the
// original servers' 32 KB buffer, the lost-connection event and the relay's plain framing.
#include "X2ServerProtocolLib.h"

#include <doctest/doctest.h>

#include <chrono>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace {

constexpr UINT kServerMsgID = 1234;

// what the game's SEND_MSG_FUNC receives
struct KGameMessages
{
    std::mutex mutex;
    std::vector< std::pair< WPARAM, std::unique_ptr< KSerBuffer > > > messages;

    bool Has( unsigned short usEventID )
    {
        std::lock_guard<std::mutex> lock( mutex );
        for( const auto& kMsg : messages )
            if( kMsg.first == usEventID )
                return true;
        return false;
    }
    template < typename T >
    bool Get( unsigned short usEventID, T& data )
    {
        std::lock_guard<std::mutex> lock( mutex );
        for( auto& kMsg : messages )
        {
            if( kMsg.first != usEventID )
                continue;
            KSerializer ks;
            ks.BeginReading( kMsg.second.get() );
            return ks.Get( data );
        }
        return false;
    }
    void Clear()
    {
        std::lock_guard<std::mutex> lock( mutex );
        messages.clear();
    }
} g_kGame;

bool SendGameMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, bool /*bDirectProcess*/ )
{
    std::unique_ptr< KSerBuffer > spBuff( reinterpret_cast< KSerBuffer* >( lParam ) );
    if( uMsg != kServerMsgID )
        return false;
    std::lock_guard<std::mutex> lock( g_kGame.mutex );
    g_kGame.messages.emplace_back( wParam, std::move( spBuff ) );
    return true;
}

// a server's end of a client connection (KUser in the servers)
class KTestServerSession : public KSession
{
public:
    KTestServerSession() : KSession( false ) {}

    int GetPfID() override { return 0; }

    void ProcessEvent( const KEventPtr& spEvent ) override
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        m_vecEvents.push_back( spEvent );
    }

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

    // Runs on another thread: no doctest assertions here.
    SOCKET Accept( SOCKADDR_IN& remote )
    {
        remote = SOCKADDR_IN();
        x2_socklen_t len = sizeof( remote );
        return ::accept( sock, reinterpret_cast<SOCKADDR*>( &remote ), &len );
    }
};

// what NetLayer does with an accepted socket, with the sequence number check the servers can turn on
std::shared_ptr<KTestServerSession> AcceptSession( Listener& listener )
{
    SOCKADDR_IN remote;
    SOCKET sock = listener.Accept( remote );
    if( sock == INVALID_SOCKET )
        return nullptr;

    auto spServer = std::make_shared<KTestServerSession>();
    spServer->Init( true );
    spServer->InitCheckSequenceNumInfo( true, 100 );
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

bool RecvAll( SOCKET sock, char* pBuffer, int iSize )
{
    while( iSize > 0 )
    {
        const int iRead = static_cast<int>( ::recv( sock, pBuffer, iSize, 0 ) );
        if( iRead <= 0 )
            return false;
        pBuffer += iRead;
        iSize -= iRead;
    }
    return true;
}

HWND GameWindow() { return reinterpret_cast<HWND>( static_cast<std::uintptr_t>( 1 ) ); }    // any non-null handle

} // namespace

TEST_CASE( "CX2ServerProtocol: channel server connection over loopback" )
{
    g_kGame.Clear();
    CX2ServerProtocol kProtocol( SendGameMessage, GameWindow(), kServerMsgID, nullptr, 0 );
    {
        Listener listener;
        std::shared_ptr<KTestServerSession> spServer;
        std::thread accepter( [&] { spServer = AcceptSession( listener ); } );

        const bool bConnected = kProtocol.ConnectedToChannelServer( L"127.0.0.1", listener.usPort );  // waits for the key
        accepter.join();
        REQUIRE( bConnected );
        REQUIRE( spServer );
        CHECK( kProtocol.IsChConnected() );

        // client -> server: the client's encryption, accepted by the server with the sequence check on
        const std::wstring wstrSent = L"가나다 elsword";
        for( int i = 0; i < 3; ++i )
        {
            std::wstring wstr = wstrSent;
            REQUIRE( kProtocol.SendChPacket( ECH_VERIFY_ACCOUNT_REQ, wstr ) );
        }
        REQUIRE( WaitFor( [&] { return spServer->Collect().size() >= 3; } ) );
        for( const KEventPtr& sp : spServer->Collect() )
        {
            CHECK( sp->m_usEventID == ECH_VERIFY_ACCOUNT_REQ );
            CHECK( sp->m_kDestPerformer.m_dwPerformerID == PI_CHANNEL_USER );
            std::wstring wstrGot;
            KSerializer ks;
            ks.BeginReading( &sp->m_kbuff );
            REQUIRE( ks.Get( wstrGot ) );
            CHECK( wstrGot == wstrSent );
        }
        CHECK( spServer->GetPacketAuthFailCnt() == 0 );

        // server -> client: 40 KB, more than the original servers' 32 KB buffer (decision T34), not compressed
        std::wstring wstrBig( 20000, L'x' );
        REQUIRE( spServer->SendPacket( PI_NULL, 0, NULL, ECH_VERIFY_ACCOUNT_ACK, wstrBig, true, false ) );
        REQUIRE( WaitFor( [&] { return g_kGame.Has( ECH_VERIFY_ACCOUNT_ACK ); } ) );
        std::wstring wstrGot;
        REQUIRE( g_kGame.Get( ECH_VERIFY_ACCOUNT_ACK, wstrGot ) );
        CHECK( wstrGot == wstrBig );

        // the server closes: the game gets E_CONNECTION_LOST_NOT (the disconnection was not intended) and then
        // disconnects the proxy itself (X2State); as in the original, the proxy's socket stays open until then
        spServer->GetSockObj()->CloseSocket();
        REQUIRE( WaitFor( [&] { return g_kGame.Has( E_CONNECTION_LOST_NOT ); } ) );
    }
    kProtocol.DisconnectFromChannelServer();
    CHECK( !kProtocol.IsChUserProxyValid() );
}

TEST_CASE( "KEvent names on the client side: system and client events only" )
{
    CHECK( std::wstring( KEvent::GetIDStr( E_CONNECTION_LOST_NOT ) ) == L"E_CONNECTION_LOST_NOT" );
    CHECK( std::wstring( KEvent::GetIDStr( ECH_VERIFY_ACCOUNT_REQ ) ) == L"ECH_VERIFY_ACCOUNT_REQ" );
    // a server-to-server event is not named (src/client/X2ServerProtocol/EventNames.cpp)
    CHECK( std::wstring( KEvent::GetIDStr( E_SERVER_EVENT_ID_BEGIN + 1 ) ) == L"EGS_CLIENT_EVENT_ID_END" );
    CHECK( std::wstring( KEvent::GetIDStr( 0xFFFF ) ) == L"EGS_CLIENT_EVENT_ID_END" );
}

TEST_CASE( "CX2ServerProtocol: game server connection refused" )
{
    CX2ServerProtocol kProtocol( SendGameMessage, GameWindow(), kServerMsgID, nullptr, 0 );
    unsigned short usPort = 0;
    {
        Listener listener;  // a port nothing listens on once it closes
        usPort = listener.usPort;
    }
    CHECK( !kProtocol.ConnectedToGameServer( L"127.0.0.1", usPort ) );
    CHECK( !kProtocol.IsUserProxyValid() );
    CHECK( !kProtocol.SendID( E_HEART_BEAT ) );
}

TEST_CASE( "CX2ServerProtocol: TCP relay framing" )
{
    g_kGame.Clear();
    CX2ServerProtocol kProtocol( SendGameMessage, GameWindow(), kServerMsgID, nullptr, 0 );

    Listener listener;
    SOCKET sock = INVALID_SOCKET;
    std::thread accepter( [&] { SOCKADDR_IN remote; sock = listener.Accept( remote ); } );
    const bool bConnected = kProtocol.ConnectedToTCPRelay( L"127.0.0.1", 42, listener.usPort );
    accepter.join();
    REQUIRE( bConnected );
    REQUIRE( sock != INVALID_SOCKET );

    // client -> relay: USHORT total length, then the serialized KEvent, no encryption
    std::wstring wstrSent = L"relay";
    REQUIRE( kProtocol.SendTRPacket( ECH_VERIFY_ACCOUNT_REQ, wstrSent ) );
    USHORT usLen = 0;
    REQUIRE( RecvAll( sock, reinterpret_cast<char*>( &usLen ), sizeof( usLen ) ) );
    REQUIRE( usLen > sizeof( usLen ) );
    std::vector<char> vecData( usLen - sizeof( usLen ) );
    REQUIRE( RecvAll( sock, vecData.data(), static_cast<int>( vecData.size() ) ) );
    {
        KSerBuffer kBuff;
        kBuff.Write( vecData.data(), vecData.size() );
        KEvent kEvent;
        KSerializer ks;
        ks.BeginReading( &kBuff );
        REQUIRE( ks.Get( kEvent ) );
        CHECK( kEvent.m_usEventID == ECH_VERIFY_ACCOUNT_REQ );
        kEvent.m_kbuff.UnCompress();
        std::wstring wstrGot;
        KSerializer ksData;
        ksData.BeginReading( &kEvent.m_kbuff );
        REQUIRE( ksData.Get( wstrGot ) );
        CHECK( wstrGot == wstrSent );
    }

    // relay -> client: the same framing, delivered to the game
    {
        std::wstring wstrRelay = L"from the relay";
        KEvent kEvent;
        kEvent.SetData( 0, NULL, ECH_VERIFY_ACCOUNT_ACK, wstrRelay );
        KSerBuffer kBuff;
        KSerializer ks;
        ks.BeginWriting( &kBuff );
        REQUIRE( ks.Put( kEvent ) );
        ks.EndWriting();
        std::vector<char> vecFrame( sizeof( USHORT ) + kBuff.GetLength() );
        const USHORT usFrame = static_cast<USHORT>( vecFrame.size() );
        std::memcpy( vecFrame.data(), &usFrame, sizeof( usFrame ) );
        std::memcpy( vecFrame.data() + sizeof( usFrame ), kBuff.GetData(), kBuff.GetLength() );
        REQUIRE( ::send( sock, vecFrame.data(), static_cast<int>( vecFrame.size() ), 0 ) == static_cast<int>( vecFrame.size() ) );

        REQUIRE( WaitFor( [&] { return g_kGame.Has( ECH_VERIFY_ACCOUNT_ACK ); } ) );
        std::wstring wstrGot;
        REQUIRE( g_kGame.Get( ECH_VERIFY_ACCOUNT_ACK, wstrGot ) );
        CHECK( wstrGot == wstrRelay );
    }

    kProtocol.DisconnectFromTRServer();
    CLOSE_SOCKET( sock );
}
