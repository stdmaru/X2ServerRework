#pragma once

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

#include "SubThread.h"


//{{ 2010. 06. 04  최육사	넥슨PC방 인증 서버 개편


class KNexonAuth : public KSubThread
{
public:
	KNexonAuth( void );
	virtual ~KNexonAuth( void );

protected:

	// derived from KSubThread
	virtual void ProcessEvent( const KEventPtr& spEvent );

    DECL_ON_FUNC( ENX_AUTH_LOGIN_REQ );
    DECL_ON_FUNC( ENX_AUTH_LOGOUT_NOT );
	DECL_ON_FUNC( ENX_AUTH_SYNCHRONIZE_ACK );
};


//}}
