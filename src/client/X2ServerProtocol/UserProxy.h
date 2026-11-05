#pragma once

#include "ActorProxy.h"

SmartPointer( KUserProxy );

// The client's connection to the game, channel or collect server.
class KUserProxy : public KActorProxy
{
    NiDeclareRTTI;
    DeclToStringW;
    DeclPfID;

public:
	enum USER_PROXY_TYPE
	{
		UPT_INVALID,
		UPT_GAME_SERVER,
		UPT_CHANNEL_SERVER,
		UPT_COLLECT_SERVER,
	};

public:
    KUserProxy( USER_PROXY_TYPE eType );
    virtual ~KUserProxy(void);

    //////////////////////////////////////////////////////////////////////////
    // Interfaces for Client Programmer.
	bool GetIntendedDisconnect() const { return m_bIntendedDisconnect; }
	void SetIntendedDisconnect(bool val) { m_bIntendedDisconnect = val; }

protected:
    // derived from KPerformer
    virtual void ProcessEvent( const KEventPtr& spEvent );

    virtual void OnSocketError();
	virtual void OnAuthenticFailed();   // derived from KSession: packets keep failing authentication

private:
	USER_PROXY_TYPE m_eProxyType;
	bool m_bIntendedDisconnect;		// dmlee, 2009-03-11: tells a wanted disconnection from one caused by an error
};
