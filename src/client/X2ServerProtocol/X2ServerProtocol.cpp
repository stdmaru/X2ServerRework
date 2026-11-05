#include "StdAfx.h"

bool	g_bIsSERVICE = false;

CX2ServerProtocol::CX2ServerProtocol( SEND_MSG_FUNC pSendGameMessage, HWND hWnd, UINT serverMsgID, const WCHAR** /*ppEventIDStrList*/, int /*eventIDEnd*/, bool bIsSERVICE )
{
	g_bIsSERVICE = bIsSERVICE;

	m_pSendGameMessage		= pSendGameMessage;
	m_hWnd					= hWnd;
	m_ServerMsgID			= serverMsgID;

	NetCommon::InitWinSock();
	KncSecurity::InitSecurity();

	// Port (decision T20): the proxies wait on their own events (Init( false )), as in the original, but the
	// asynchronous I/O behind them now completes on an I/O thread (the original needed none: Windows signaled the
	// OVERLAPPED events). One thread is enough for a client.
	SiKIocp()->GetIoService().Restart();
	SiKIocp()->Init( 1 );
	SiKIocp()->BeginThread();
}

CX2ServerProtocol::~CX2ServerProtocol(void)
{
	// the proxies first (their threads and sockets), then the I/O thread they use
	DisconnectFromGameServer();
	DisconnectFromTRServer();
	DisconnectFromChannelServer();
	DisconnectFromCollectServer();

	SiKIocp()->EndThread();
	KIocp::ReleaseInstance();

	KncSecurity::ReleaseSecurity();
}

void CX2ServerProtocol::AlreadyConnected( const wchar_t* szWhat_, const KUserProxyPtr& spProxy_, const WCHAR* pIP_, int port_ )
{
	if( !spProxy_ )
		return;

	START_LOG( cerr, L"called again while already connected to the " << szWhat_ )
		<< BUILD_LOG( spProxy_->GetIPStr() )
		<< BUILD_LOG( spProxy_->GetPort() )
		<< BUILD_LOG( pIP_ )
		<< BUILD_LOG( port_ );
}

KUserProxyPtr CX2ServerProtocol::Connect( KUserProxy::USER_PROXY_TYPE eType_, int iClassID_, const WCHAR* pIP_, int port_ )
{
	KUserProxyPtr spProxy( new KUserProxy( eType_ ) );
	spProxy->SetServerMsgID( m_ServerMsgID );
	spProxy->SetClassID( iClassID_ );
	spProxy->Init( false );
	spProxy->SetHwnd( m_hWnd );
	spProxy->SetSendMsgCallBack( m_pSendGameMessage );

	std::wstring wIP = pIP_;
	std::string IP = KncUtil::toNarrowString( wIP );

	if( !spProxy->Connect( IP.c_str(), static_cast<u_short>(port_) ) )
		return KUserProxyPtr();

	return spProxy;
}

bool CX2ServerProtocol::ConnectedToGameServer( const WCHAR* pIP, const int port )
{
	if( IsConnected() )
	{
		AlreadyConnected( L"game server", m_pkUserProxy.load(), pIP, port );
		return false;
	}

	KUserProxyPtr spProxy = Connect( KUserProxy::UPT_GAME_SERVER, GSPROXY_ID, pIP, port );
	m_pkUserProxy.store( spProxy );    // null when the connection failed
	return spProxy != nullptr;
}

bool CX2ServerProtocol::ConnectedToTCPRelay( const WCHAR* pIP, const UidType& iUnitUID, const int port /* = 9301  */)
{
	if( IsTRConnected() )
	{
		if( KTRUserPtr spTRUserProxy = m_pkTRUserProxy.load() )
		{
			START_LOG( cerr, L"called again while already connected to the TCP relay" )
				<< BUILD_LOG( spTRUserProxy->GetIPStr() )
				<< BUILD_LOG( spTRUserProxy->GetPort() )
				<< BUILD_LOG( pIP )
				<< BUILD_LOG( port );
		}
		return false;
	}

	KTRUserPtr spTRUser( new KTRUser );
	spTRUser->SetServerMsgID( m_ServerMsgID );
	spTRUser->SetClassID( RTPROXY_ID );
	spTRUser->Init();
	spTRUser->SetSendMsgCallBack(m_pSendGameMessage);

	std::wstring wIP = pIP;
	std::string IP = KncUtil::toNarrowString( wIP );

	if( !spTRUser->Connect( IP.c_str(), static_cast<u_short>(port), iUnitUID ) )
        return false;

	m_pkTRUserProxy.store( spTRUser );
	return true;
}

bool CX2ServerProtocol::ConnectedToChannelServer( const WCHAR* pIP, const int port )
{
	if( IsChConnected() )
	{
		AlreadyConnected( L"channel server", m_pkChUserProxy.load(), pIP, port );
		return false;
	}

	KUserProxyPtr spProxy = Connect( KUserProxy::UPT_CHANNEL_SERVER, CHPROXY_ID, pIP, port );
	m_pkChUserProxy.store( spProxy );
	return spProxy != nullptr;
}

void CX2ServerProtocol::ConnectedToGameServerForWorldTool()
{
	// a proxy that never connects, for the world tool (no message callback, as in the original)
	KUserProxyPtr spProxy( new KUserProxy( KUserProxy::UPT_GAME_SERVER ) );
	spProxy->SetServerMsgID( m_ServerMsgID );

	spProxy->SetClassID( GSPROXY_ID );
	spProxy->Init( false );
	spProxy->SetHwnd( m_hWnd );

	m_pkUserProxy.store( spProxy );
}

bool CX2ServerProtocol::ConnectedToCollectServer( const WCHAR* pIP, const int port)
{
	if( IsCSConnected() )
	{
#if X2_DEV_TOOLS
		AlreadyConnected( L"collect server", m_pkCSUserProxy.load(), pIP, port );
#endif
		return false;
	}

	KUserProxyPtr spProxy = Connect( KUserProxy::UPT_COLLECT_SERVER, CSPROXY_ID, pIP, port );
	m_pkCSUserProxy.store( spProxy );
	return spProxy != nullptr;
}
