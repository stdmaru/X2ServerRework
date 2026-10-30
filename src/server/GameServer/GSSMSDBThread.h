#pragma once
#include "BaseServer.h" // KBaseServer::GetKObj() in the templates below

#include "SubThread.h"
#include "ODBC/Odbc.h"
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

class KGSSMSDBThread : public KSubThread
{
	DeclareDBThread( KGSSMSDBThread );

public:
	KGSSMSDBThread();
	virtual ~KGSSMSDBThread(void);

protected:
	// derived from KSubThread
	virtual void ProcessEvent( const KEventPtr& spEvent ); // Port: `inline` dropped, it is defined in the .cpp

protected:
	// packet send function
	template < class T > void SendToServer( unsigned short usEventID, const T& data );
	template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
	void SendToServer( unsigned short usEventID );
	void SendToUser( UidType nTo, unsigned short usEventID );

	// event handler
	DECL_ON_FUNC( DBE_SEND_PHONE_MSG_NOT );
	//{{ 2010. 11. 2	최육사	서버간 접속 모니터링
   //}}
};

template < class T >
void KGSSMSDBThread::SendToServer( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_GAME_DB, 0, PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KGSSMSDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_GAME_DB, 0, PI_GS_USER, nTo, NULL, usEventID, data );
}
