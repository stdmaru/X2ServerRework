// UDP over loopback: CKTDNUDP datagrams (1-byte ID + raw struct), read by OnFrameMove() without blocking,
// and the port check served by KLBSUdpEcho's thread, which the original woke with completion routines and now
// waits for readable data (see KTDNUDP.cpp).
#include "ActorManager.h"
#include "Socket/KTDNUDP.h"
#include "Socket/LBSUdpEcho.h"
#include "Socket/NetCommon.h"
#include "x2/platform/net_io.h"
#include "../X2Lib/X2UDPPacket.h"

#include <doctest/doctest.h>

#include <chrono>
#include <cstring>
#include <thread>

namespace {

const u_long kLoopback = htonl( INADDR_LOOPBACK );

// receives on the CKTDNUDP and returns the first datagram with the given ID, if any comes within ~1 s
bool ReceiveFrom( CKTDNUDP& kUdp, char cID, std::vector<char>& vecData, u_long& ulSender, int& iSenderPort )
{
    for( int i = 0; i < 100; ++i )
    {
        kUdp.OnFrameMove();     // reads what is queued
        std::list<CKTDNUDP::RecvData>::iterator it;
        while( kUdp.GetFrontRecvData( it ) )
        {
            const bool bMatch = it->GetID() == cID;
            if( bMatch )
            {
                vecData.assign( it->GetData(), it->GetData() + it->GetSize() );
                ulSender = it->m_SenderIPAddress;
                iSenderPort = it->m_SenderPort;
            }
            kUdp.ReturnRecvData( it );
            if( bMatch )
                return true;
        }
        x2::platform::net::WaitReadable( kUdp.GetSocket(), 10 );
    }
    return false;
}

} // namespace

TEST_CASE( "CKTDNUDP: datagrams between two sockets" )
{
    NetCommon::InitWinSock();
    CKTDNUDP kA( 48100 );   // binds the next free port from there
    CKTDNUDP kB( 48100 );
    REQUIRE( kA.GetMyPort() != kB.GetMyPort() );

    KXPT_PORT_CHECK_REQ kReq;
    kReq.m_UserUID = 0x0102030405060708LL;
    kReq.m_InternalIPAddress = 0x11223344;
    kReq.m_usInternalPort = 0x5566;
    REQUIRE( kA.SendToIP( kLoopback, kB.GetMyPort(), XPT_PORT_CHECK_REQ, &kReq, sizeof( kReq ) ) );

    std::vector<char> vecData;
    u_long ulSender = 0;
    int iSenderPort = 0;
    REQUIRE( ReceiveFrom( kB, XPT_PORT_CHECK_REQ, vecData, ulSender, iSenderPort ) );
    CHECK( ulSender == kLoopback );
    CHECK( iSenderPort == kA.GetMyPort() );
    REQUIRE( vecData.size() == sizeof( KXPT_PORT_CHECK_REQ ) );
    KXPT_PORT_CHECK_REQ kGot;
    std::memcpy( static_cast<void*>( &kGot ), vecData.data(), sizeof( kGot ) );
    CHECK( kGot.m_UserUID == kReq.m_UserUID );
    CHECK( kGot.m_InternalIPAddress == kReq.m_InternalIPAddress );
    CHECK( kGot.m_usInternalPort == kReq.m_usInternalPort );

    // nothing queued: OnFrameMove returns without blocking
    const auto tStart = std::chrono::steady_clock::now();
    kB.OnFrameMove();
    CHECK( std::chrono::steady_clock::now() - tStart < std::chrono::milliseconds( 50 ) );
}

TEST_CASE( "KLBSUdpEcho answers XPT_PORT_CHECK_REQ with the address it saw" )
{
    NetCommon::InitWinSock();
    if( !KActorManager::GetKObj() )
        KActorManager::KeepObject<KActorManager>();   // the echo looks the user up (not found here)

    KLBSUdpEcho kEcho;
    REQUIRE( kEcho.Init( 48200 ) );
    REQUIRE( kEcho.Begin() );
    for( int i = 0; i < 100 && !kEcho.GetKTUDP(); ++i )
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    REQUIRE( kEcho.GetKTUDP() );

    CKTDNUDP kClient( 48300 );
    KXPT_PORT_CHECK_REQ kReq;
    kReq.m_UserUID = 42;
    kReq.m_InternalIPAddress = 0;
    kReq.m_usInternalPort = 0;
    REQUIRE( kClient.SendToIP( kLoopback, kEcho.GetPort(), XPT_PORT_CHECK_REQ, &kReq, sizeof( kReq ) ) );

    std::vector<char> vecData;
    u_long ulSender = 0;
    int iSenderPort = 0;
    REQUIRE( ReceiveFrom( kClient, XPT_PORT_CHECK_ACK, vecData, ulSender, iSenderPort ) );
    REQUIRE( vecData.size() == sizeof( KXPT_PORT_CHECK_ACK ) );
    KXPT_PORT_CHECK_ACK kAck;
    std::memcpy( static_cast<void*>( &kAck ), vecData.data(), sizeof( kAck ) );
    CHECK( kAck.m_IPAddress == kLoopback );
    CHECK( kAck.m_Port == kClient.GetMyPort() );

    // the thread notices the kill event without traffic
    const auto tStart = std::chrono::steady_clock::now();
    kEcho.End( 3000 );
    CHECK( std::chrono::steady_clock::now() - tStart < std::chrono::seconds( 1 ) );
    KActorManager::ReleaseKObj();
}
