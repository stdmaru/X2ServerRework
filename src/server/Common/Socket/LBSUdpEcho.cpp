#include <Socket/KTDNUDP.h>
#include "LBSUdpEcho.h"
#include "x2/platform/net_io.h"
#include <dbg/dbg.hpp>
#include "BaseServer.h"
#include "ActorManager.h"

#include <Serializer/SerBuffer.h>
#include <Serializer/SerPair.h>
#include <Serializer/SerList.h>
#include <Serializer/SerVector.h>
#include <Serializer/SerDeque.h>
#include <Serializer/SerMap.h>
#include <Serializer/SerSet.h>
#include <Serializer/SerializeMacro.h>

template < typename T >
inline void Serialize( KSerBuffer* pBuff, T* pData )
{
	KSerializer ks;
	ks.BeginWriting( pBuff );
	ks.Put( *pData );
	ks.EndWriting();
}

template < typename T >
inline void DeSerialize( KSerBuffer* pBuff, T* pData )
{
	KSerializer ks;
	ks.BeginReading( pBuff );
	ks.Get( *pData );
	ks.EndReading();
}

#include "X2Packet.h"

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
// 시스템 내에 KLBSUDPEcho 개체가 1개 밖에 없다고 가정
// 서로 다른 쓰레드에서 여러 개체가 생성될 때는 안전하지 않다.
static std::list<CKTDNUDP::OverlappedDataInOp> s_listOverlappedData1;
static std::list<CKTDNUDP::OverlappedDataInOp> s_listOverlappedData2;
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


KLBSUdpEcho::KLBSUdpEcho(void)
{
    m_usPort = 0;
}

KLBSUdpEcho::~KLBSUdpEcho(void)
{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    End();
    s_listOverlappedData1.clear();
    s_listOverlappedData2.clear();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}

bool KLBSUdpEcho::Init( USHORT usPort_ )
{
    m_usPort = usPort_;

    return true;
}

void KLBSUdpEcho::Run()
{
    START_LOG( clog, L"Enter Run()" );

    DWORD ret;
    m_spUDP = CKTDNUDPPtr( new CKTDNUDP( m_usPort ) );

    while( true )
    {
//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		m_spUDP->OnFrameMove();     // Port: reads and handles the datagrams already queued

		// Port: the original waited here alertably (WaitForSingleObjectEx) until a completion routine ran;
		// now it waits until a datagram arrives, at most 10 ms, and checks the kill event
		x2::platform::net::WaitReadable( m_spUDP->GetSocket(), 10 );
		ret = ::WaitForSingleObject( m_hKillEvent, 0 );

		if( ret == WAIT_OBJECT_0 )
			break;

		m_spUDP->ProcessRecvData();

		_ProcessRecvData();
//#else//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//        ret = ::WaitForSingleObject( m_hKillEvent, 1 );     // sleep 타임을 주면 안된다.
//
//        if( ret == WAIT_OBJECT_0 ) break;
//
//        if( ret == WAIT_TIMEOUT ) Loop();
//
//        else std::cout << "*** WaitForSingleObject() - return : " << ret << std::endl;
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    }// while

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    // Overlapped I/O is destroyed together with the thread that issued it. The call below kept the overlapped
    // data used by the UDP manager alive until the thread is gone. (Port: the I/O is synchronous now; kept as is.)
    m_spUDP->ThreadUnsafe_ObtainOverlappedData( s_listOverlappedData1, s_listOverlappedData2 );
    m_spUDP.reset();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}

u_short KLBSUdpEcho::GetPort()
{
    if( m_spUDP )
    {
        return static_cast<u_short>(m_spUDP->GetMyPort());
    }
    else
    {
        return  m_usPort;
    }
}

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//void KLBSUdpEcho::Loop()
//{
//	//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
//#ifdef SERV_FIX_SYNC_PACKET_USING_RELAY
//	m_spUDP->OnFrameMove();
//#else
//    m_spUDP->OnFrameMove( 0.0f, 0.001f );
//#endif SERV_FIX_SYNC_PACKET_USING_RELAY
//	//}}
//
//    CKTDNUDP::RecvData* pRecvData = m_spUDP->PopRecvData();
//
//    while( pRecvData != NULL )
//    {
//        switch( pRecvData->m_ID )
//        {
//     //       case XPT_CHECK_SERVER_ALIVE_REQ:
//     //           {
//					//KXPT_CHECK_SERVER_ALIVE_ACK kXPT_CHECK_SERVER_ALIVE_ACK;
//					//kXPT_CHECK_SERVER_ALIVE_ACK.UID = ( int )KBaseServer::GetKObj()->GetUID();
//
//					//KSerBuffer buff;
//					//Serialize( &buff, &kXPT_CHECK_SERVER_ALIVE_ACK );
//
//     //               m_spUDP->Send( pRecvData->m_SenderIP.c_str(), pRecvData->m_SenderPort, XPT_CHECK_SERVER_ALIVE_ACK, (char*)buff.GetData(), buff.GetLength() );
//     //           }
//     //           break;
//
//			case XPT_PORT_CHECK_REQ:
//				{
//					KSerBuffer ksBuff;
//					ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//					KXPT_PORT_CHECK_REQ kXPT_PORT_CHECK_REQ;
//					DeSerialize( &ksBuff, &kXPT_PORT_CHECK_REQ );
//
//					KActorPtr pkActor = KActorManager::GetKObj()->Get( kXPT_PORT_CHECK_REQ.m_UserUID );
//
//					if( pkActor != NULL )
//					{
//						pkActor->SetP2PIP( pRecvData->m_SenderIP );
//						pkActor->SetP2PPort( pRecvData->m_SenderPort );
//						//{{ 2013. 1. 8	박세훈	Merge 공인IP 연결 실패시 내부IP로 시도( 박진웅 )
////#ifdef SERV_KTDX_RETRY_USING_INTERNAL_IP
//						pkActor->SetP2PInternalIP( kXPT_PORT_CHECK_REQ.m_wstrInternalIP );
//						pkActor->SetP2PInternalPort( kXPT_PORT_CHECK_REQ.m_usInternalPort );
//
//						START_LOG( clog, L"IP, 포트 확인" )
//							<< BUILD_LOG( kXPT_PORT_CHECK_REQ.m_wstrInternalIP )
//							<< BUILD_LOG( kXPT_PORT_CHECK_REQ.m_usInternalPort )
//							<< END_LOG;
////#endif SERV_KTDX_RETRY_USING_INTERNAL_IP
//						//}}
//					}
//					else
//					{
//						START_LOG( cwarn, L"pkActor 찾기 실패.!" )
//							<< BUILD_LOG( kXPT_PORT_CHECK_REQ.m_UserUID )
//							<< END_LOG;
//					}
//
//					if( m_spUDP != NULL )
//					{
//						KXPT_PORT_CHECK_ACK kXPT_PORT_CHECK_ACK;
//						kXPT_PORT_CHECK_ACK.m_IP	= pRecvData->m_SenderIP;
//						kXPT_PORT_CHECK_ACK.m_Port	= pRecvData->m_SenderPort;
//
//						KSerBuffer buff;
//						Serialize( &buff, &kXPT_PORT_CHECK_ACK );
//
//						m_spUDP->Send( pRecvData->m_SenderIP.c_str(), pRecvData->m_SenderPort, XPT_PORT_CHECK_ACK, (char*)buff.GetData(), buff.GetLength() );
//					}
//				}
//				break;
//        }
//
//        if( pRecvData )
//        {
//            delete pRecvData;
//            pRecvData = NULL;
//        }
//
//        pRecvData = m_spUDP->PopRecvData();
//    }
//}
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
void KLBSUdpEcho::_ProcessRecvData()
{
	std::list< CKTDNUDP::RecvData >::iterator itRecvData;
	while( m_spUDP->GetFrontRecvData( itRecvData ) )
	{
		switch( itRecvData->GetID() )
		{
			//       case XPT_CHECK_SERVER_ALIVE_REQ:
			//           {
			//KXPT_CHECK_SERVER_ALIVE_ACK kXPT_CHECK_SERVER_ALIVE_ACK;
			//kXPT_CHECK_SERVER_ALIVE_ACK.UID = ( int )KBaseServer::GetKObj()->GetUID();

			//KSerBuffer buff;
			//Serialize( &buff, &kXPT_CHECK_SERVER_ALIVE_ACK );

			//               m_spUDP->Send( itRecvData->m_SenderIP.c_str(), itRecvData->m_SenderPort, XPT_CHECK_SERVER_ALIVE_ACK, (char*)buff.GetData(), buff.GetLength() );
			//           }
			//           break;

		case XPT_PORT_CHECK_REQ:
			{

                if ( itRecvData->GetSize() != sizeof(KXPT_PORT_CHECK_REQ) )
                    break;
                const KXPT_PORT_CHECK_REQ* pkReq = (const KXPT_PORT_CHECK_REQ*) itRecvData->GetData();
                const KXPT_PORT_CHECK_REQ& kXPT_PORT_CHECK_REQ = *pkReq;

				KActorPtr pkActor = KActorManager::GetKObj()->Get( kXPT_PORT_CHECK_REQ.m_UserUID );

				if( pkActor != NULL )
				{
                    std::wstring wstrAddr = CKTDNUDP::ConvertAddressToIP( itRecvData->m_SenderIPAddress );


                    pkActor->SetP2PIP( wstrAddr );

					pkActor->SetP2PPort( itRecvData->m_SenderPort );
					//{{ 2013. 1. 8	박세훈	Merge 공인IP 연결 실패시 내부IP로 시도( 박진웅 )
//#ifdef SERV_KTDX_RETRY_USING_INTERNAL_IP

                    pkActor->SetP2PInternalIP( CKTDNUDP::ConvertAddressToIP( kXPT_PORT_CHECK_REQ.m_InternalIPAddress ) );

					pkActor->SetP2PInternalPort( kXPT_PORT_CHECK_REQ.m_usInternalPort );

					START_LOG( clog, L"IP, 포트 확인" )
						<< BUILD_LOG( pkActor->GetP2PInternalIP() )
						<< BUILD_LOG( kXPT_PORT_CHECK_REQ.m_usInternalPort )
						<< END_LOG;
//#endif SERV_KTDX_RETRY_USING_INTERNAL_IP
					//}}
				}
				else
				{
					START_LOG( cwarn, L"pkActor 찾기 실패.!" )
						<< BUILD_LOG( kXPT_PORT_CHECK_REQ.m_UserUID )
						<< END_LOG;
				}

				if( m_spUDP != NULL )
				{
					KXPT_PORT_CHECK_ACK kXPT_PORT_CHECK_ACK;

                    kXPT_PORT_CHECK_ACK.m_IPAddress	= itRecvData->m_SenderIPAddress;

					kXPT_PORT_CHECK_ACK.m_Port	= itRecvData->m_SenderPort;


					m_spUDP->SendToIP(
                        itRecvData->m_SenderIPAddress,
                        itRecvData->m_SenderPort, XPT_PORT_CHECK_ACK, &kXPT_PORT_CHECK_ACK, sizeof(kXPT_PORT_CHECK_ACK) );

				}
			}
			break;
		}

		//
		m_spUDP->ReturnRecvData( itRecvData );
	}
}
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
