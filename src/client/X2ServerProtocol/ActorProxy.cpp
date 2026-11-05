#include "StdAfx.h"

#include "ActorProxy.h"

NiImplementRTTI( KActorProxy, KSession );

KActorProxy::KActorProxy()
:
    KSession( true, true ),     // a proxy, on the client side (decision T34)
    m_hWnd( NULL ),
    m_pSendGameMessage( NULL ),
    m_bThrowWinMsg( false ),
    m_ServerMsgID( 0 ),
    m_nAckOK( -1 ),
    m_iClassID( 0 ),
    m_iLoad( 0 ),
    m_iServerUID( 0 )
{
	m_spEventAck.reset( CreateEvent( NULL, false, false, NULL ), CloseHandle );
}

ImplToStringW( KActorProxy )
{
	return START_TOSTRING_PARENTW( KSession );
}

void KActorProxy::PostEvent( const KEvent* pkEvent_ )
{
	// The game gets: message = m_ServerMsgID, wParam = the event ID, lParam = a copy of the data (the receiver
	// deletes it). The original once posted it to the window (PostMessage); it calls the game's callback instead.

	if( m_bThrowWinMsg )
	{
		if( g_bIsSERVICE == false )
		{
			dbg::clog << L"post message : " << pkEvent_->GetIDStr() << dbg::endl;
		}

		KSerBuffer* pBuff = new KSerBuffer( pkEvent_->m_kbuff );

		if( m_pSendGameMessage != NULL )
			m_pSendGameMessage(
			m_ServerMsgID,
			static_cast<WPARAM>(pkEvent_->m_usEventID),
			reinterpret_cast<LPARAM>(pBuff),
			false
			);
		else
			delete pBuff;   // Port: the original leaked it
	}
	else
	{
        KEventPtr spEvent( new KEvent( *pkEvent_ ) );

		KLocker lock( m_csRecvMsg );
		m_queRecvMsg.push( spEvent );
	}
}

void KActorProxy::SetHwnd( HWND hWnd_ )
{
	m_hWnd = hWnd_;

	// Port: the original asked ::IsWindow(); the handle is only a marker here (the SDL window outside Windows)
	if( m_hWnd != NULL )
		m_bThrowWinMsg = true;
}

void KActorProxy::SetThrowWinMsg( bool bThrowWinMsg_ )
{
	m_bThrowWinMsg = bThrowWinMsg_;

	if( m_bThrowWinMsg && m_hWnd == NULL )
	{
		START_LOG( cerr, L"Invalid Window Handle" ) << END_LOG;
		assert( !"set a valid HWND on KUserProxy" );
	}
}

int KActorProxy::ConnectNAuth( IN const WCHAR* pIP,
							  IN const int port,
							  IN const KEvent& kEvent_,
							  IN DWORD dwTimeOut_ )
{
	std::wstring wIP = pIP;
	std::string IP = KncUtil::toNarrowString( wIP );

	if( !KSession::Connect( IP.c_str(), static_cast<u_short>(port) ) )
	{
		START_LOG( cerr, L"game server connect failed. IP : "
			<< pIP
			<< ":" << port );
		return -1;
	}

	if( ! KSession::SendPacket( kEvent_ ) )
	{
		START_LOG( cerr, L"sending verify account req packet failed." );
		return -2;
	}

	m_nAckOK = -99; // unused-error code.

	if( m_bUseIocp )    // with the IOCP the session has no thread of its own
	{
		DWORD dwTick = ::GetTickCount();
		while( GetTickCount() - dwTick < dwTimeOut_ && m_nAckOK == -99 )
		{
			Sleep( 30 );
			Tick();
		}

		if( m_nAckOK != -99 )   // the ack arrived
			return m_nAckOK;
	}
	else
	{
		switch( ::WaitForSingleObject( m_spEventAck.get(), dwTimeOut_ ) ) {
		case WAIT_OBJECT_0: // the ack arrived
			return m_nAckOK;
		case WAIT_TIMEOUT:
			START_LOG( cerr, L"verify account timeout." );
			break;
		default:
			START_LOG( cerr, L"invalide event object." );
		}
	}

	return -3;
}
