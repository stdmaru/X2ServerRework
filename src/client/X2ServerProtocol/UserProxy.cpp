#include "StdAfx.h"

#include "UserProxy.h"

NiImplementRTTI( KUserProxy, KActorProxy );
// Port: the shared KPerformer needs a performer ID (the client copy had none); the client's proxy is PI_GS_PROXY
ImplPfID( KUserProxy, PI_GS_PROXY );

KUserProxy::KUserProxy( USER_PROXY_TYPE eType ) :
m_eProxyType( eType ),
m_bIntendedDisconnect( false )
{
}

KUserProxy::~KUserProxy(void)
{
	End( INFINITE );
}

ImplToStringW( KUserProxy )
{
    return START_TOSTRING_PARENTW( KActorProxy );
}

void KUserProxy::OnSocketError()
{
	KSession::OnSocketError();

	// dmlee 2009-03-11: unless the disconnection was intended, raise an error event
	switch( m_eProxyType )
	{
	default:
	case UPT_INVALID:
		{
			assert( !"invalid proxy type" );
		} break;

	case UPT_GAME_SERVER:
	case UPT_CHANNEL_SERVER:
	case UPT_COLLECT_SERVER:
		{
			if( false == m_bIntendedDisconnect )
			{
				// tell the game the connection was lost
				KEvent kEvent;
				kEvent.m_usEventID = E_CONNECTION_LOST_NOT;

				PostEvent( &kEvent );
			}
		} break;

	}
}

void KUserProxy::OnAuthenticFailed()
{
	KSession::OnAuthenticFailed(); // does nothing, called anyway

	// only toward the game server
	if( m_eProxyType == UPT_GAME_SERVER )
	{
		char cDummy = 0;
		SendPacket( PI_GS_USER, 0, NULL, E_CHECK_SEQUENCE_COUNT_NOT, cDummy, false, false );
	}
}

void KUserProxy::ProcessEvent( const KEventPtr& spEvent_ )
{
	// Port: the original also opened an unused KSerializer reading the event's buffer
	PostEvent( spEvent_.get() );
}
