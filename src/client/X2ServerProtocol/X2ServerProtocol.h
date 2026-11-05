#pragma once

#include "UserProxy.h"
#include "TRUser.h"
#include <atomic>
#include <memory>

// What X2Lib calls to reach the servers: one proxy per server (game, TCP relay, channel, collect), each on its own
// thread. The proxies are swapped atomically because the game and the proxies' threads use them at the same time
// (X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX, on in the original; boost::atomic_load/store became
// std::atomic<std::shared_ptr>).
class CX2ServerProtocol
{
public:
	enum
	{
		GSPROXY_ID = 1,
		RTPROXY_ID,
		CHPROXY_ID,
		CSPROXY_ID,
	};

public:
	// Port: ppEventIDStrList/eventIDEnd were the game's event names for KEvent::GetIDStr; the shared KEvent has
	// them all (Event.h), so they are not used.
	CX2ServerProtocol( SEND_MSG_FUNC pSendGameMessage, HWND hWnd, UINT serverMsgID, const WCHAR** ppEventIDStrList, int eventIDEnd, bool bIsSERVICE = false );
	~CX2ServerProtocol(void);

	bool IsConnected()          { return IsConnected( m_pkUserProxy.load() ); }
	bool IsTRConnected()        { return IsConnected( m_pkTRUserProxy.load() ); }
	bool IsChConnected()        { return IsConnected( m_pkChUserProxy.load() ); }
	bool IsCSConnected()        { return IsConnected( m_pkCSUserProxy.load() ); }

	bool ConnectedToGameServer( const WCHAR* pIP, const int port );
	bool ConnectedToTCPRelay( const WCHAR* pIP, const UidType& iUnitUID, const int port = 9301 );
	bool ConnectedToChannelServer( const WCHAR* pIP, const int port );
	void ConnectedToGameServerForWorldTool();
	bool ConnectedToCollectServer( const WCHAR* pIP, const int port);

	void DisconnectFromGameServer()     { m_pkUserProxy.store( KUserProxyPtr() ); }
	void DisconnectFromTRServer()       { m_pkTRUserProxy.store( KTRUserPtr() ); }
	void DisconnectFromChannelServer()  { m_pkChUserProxy.store( KUserProxyPtr() ); }
	void DisconnectFromCollectServer()  { m_pkCSUserProxy.store( KUserProxyPtr() ); }

	bool            IsUserProxyValid()   { return m_pkUserProxy.load() != nullptr; }
	bool            IsTRUserProxyValid() { return m_pkTRUserProxy.load() != nullptr; }
	bool            IsChUserProxyValid() { return m_pkChUserProxy.load() != nullptr; }
	bool            IsCSUserProxyValid() { return m_pkCSUserProxy.load() != nullptr; }

	bool            UserProxy_SetIntendedDisconnect( bool val )   { return SetIntendedDisconnect( m_pkUserProxy.load(), val ); }
	bool            ChUserProxy_SetIntendedDisconnect( bool val ) { return SetIntendedDisconnect( m_pkChUserProxy.load(), val ); }

	const char*		GetMyIP()
	{
        if ( KUserProxyPtr spUserProxy = m_pkUserProxy.load() )
            return spUserProxy->GetIPStr();
        return NULL;
	}
	unsigned short	GetPort()
	{
        if ( KUserProxyPtr spUserProxy = m_pkUserProxy.load() )
            return spUserProxy->GetPort();
        return 0;
	}

	template < typename T >
	bool SendPacket( unsigned short usEventID, T& data, bool bLogging = true, bool bCompress = false )
	{
        if ( KUserProxyPtr spUserProxy = m_pkUserProxy.load() )
            return spUserProxy->SendPacket( PI_GS_USER, 0, NULL, usEventID, data, bLogging, bCompress );
        return false;
	}

	bool SendID( IN const unsigned short usEventID )
	{
        if ( KUserProxyPtr spUserProxy = m_pkUserProxy.load() )
            return spUserProxy->SendID( PI_GS_USER, 0, NULL, usEventID );
        return false;
	}

	template < typename T >
	bool SendTRPacket( unsigned short usEventID, T& data, bool bCompress = false )
	{
        if ( KTRUserPtr spTRUserProxy = m_pkTRUserProxy.load() )
            return spTRUserProxy->SendPacket( usEventID, data, bCompress );
        return false;
	}
	bool SendTRID( IN const unsigned short usEventID )
	{
        if ( KTRUserPtr spTRUserProxy = m_pkTRUserProxy.load() )
            return spTRUserProxy->SendID( usEventID );
        return false;
	}

	template < typename T >
	bool SendChPacket( unsigned short usEventID, T& data, bool bLogging = true, bool bCompress = false )
	{
        if ( KUserProxyPtr spChUserProxy = m_pkChUserProxy.load() )
            return spChUserProxy->SendPacket( PI_CHANNEL_USER, 0, NULL, usEventID, data, bLogging, bCompress );
        return false;
	}
	bool SendChID( IN const unsigned short usEventID )
	{
        if ( KUserProxyPtr spChUserProxy = m_pkChUserProxy.load() )
            return spChUserProxy->SendID( PI_CHANNEL_USER, 0, NULL, usEventID );
        return false;
	}

	template < typename T >
	bool SendCSPacket( unsigned short usEventID, T& data, bool bCompress = false )
	{
        if ( KUserProxyPtr spCSUserProxy = m_pkCSUserProxy.load() )
            return spCSUserProxy->SendPacket( 0x00000A00 | PC_USER , 0, NULL, usEventID, data, true, bCompress );
        return false;
	}

protected:
	template < typename TPtr >
	static bool IsConnected( const TPtr& spProxy ) { return spProxy && spProxy->IsConnected(); }
	static bool SetIntendedDisconnect( const KUserProxyPtr& spProxy, bool val )
	{
		if( !spProxy )
			return false;
		spProxy->SetIntendedDisconnect( val );
		return true;
	}
	// a new proxy of the game, channel or collect server, connected to pIP:port; null if that failed
	KUserProxyPtr Connect( KUserProxy::USER_PROXY_TYPE eType, int iClassID, const WCHAR* pIP, int port );
	void AlreadyConnected( const wchar_t* szWhat, const KUserProxyPtr& spProxy, const WCHAR* pIP, int port );

	std::atomic< KUserProxyPtr >	m_pkUserProxy;
    std::atomic< KTRUserPtr >       m_pkTRUserProxy;
	std::atomic< KUserProxyPtr >	m_pkChUserProxy;
	std::atomic< KUserProxyPtr >	m_pkCSUserProxy;

	HWND			m_hWnd;
	UINT			m_ServerMsgID;
	SEND_MSG_FUNC	m_pSendGameMessage;
};
