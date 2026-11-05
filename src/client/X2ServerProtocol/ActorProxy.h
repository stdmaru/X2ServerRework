#pragma once

#include "Socket/Session.h"
#include <memory>
#include <queue>
#include <type_traits>

SmartPointer( KActorProxy );

// The client's connection to a server: a client-side proxy KSession (decision T34) that hands what it receives to
// the game through SEND_MSG_FUNC or keeps it in m_queRecvMsg. Not the servers' KActorProxy (src/server/Common).
class KActorProxy : public KSession
{
	NiDeclareRTTI;
	DeclToStringW;

protected:
	KActorProxy();

public:
	virtual ~KActorProxy(void) {}

	void	SetHwnd( HWND hWnd );
	void	SetSendMsgCallBack( SEND_MSG_FUNC pSendGameMessage ){ m_pSendGameMessage = pSendGameMessage; }
	void	SetThrowWinMsg( bool bThrowWinMsg );
	bool	GetThrowWinMsg() { return m_bThrowWinMsg; }

	// 2006-07-20: waits until the authentication is handled; KUserProxy and KGSProxy use it.
	int		ConnectNAuth( IN const WCHAR* pIP, IN const int port, IN const KEvent& kEvent, IN DWORD dwTimeOut );

	void	SetClassID( int iClassID )	{ m_iClassID = iClassID; }  // the ID used to build the window message
	int		GetClassID()				{ return m_iClassID; }
	void	SetServerMsgID( UINT serverMsgID ){ m_ServerMsgID = serverMsgID; }
    void SetLoad( int iLoad )               { m_iLoad = iLoad; }
    int GetLoad()                           { return m_iLoad; }

    void SetServerUID( UidType iServerUID ) { m_iServerUID = iServerUID; }
    UidType GetServerUID()                  { return m_iServerUID; }

protected:
	void PostEvent( const KEvent* pkEvent );    // hands the event to the game, or queues it

	HWND                        m_hWnd;
	SEND_MSG_FUNC				m_pSendGameMessage;
	bool                        m_bThrowWinMsg;     ///< false: received messages are only queued
	UINT						m_ServerMsgID;

	std::shared_ptr< std::remove_pointer_t< HANDLE > > m_spEventAck; // HANDLE is not void* outside Windows
	int                         m_nAckOK;
	int                         m_iClassID;         // tells the proxies apart when events go to the game

    int                         m_iLoad;
    UidType                     m_iServerUID;
public:

	KncCriticalSection          m_csRecvMsg;
	std::queue<KEventPtr>       m_queRecvMsg;       ///< used when m_bThrowWinMsg is false

};
