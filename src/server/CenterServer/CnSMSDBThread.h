#pragma once
#include "BaseServer.h" // KBaseServer::GetKObj() in the templates below
#include "SubThread.h"
#include "ODBC/Odbc.h"
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ClientPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

class KCnSMSDBThread : public KSubThread
{
	DeclareDBThread( KCnSMSDBThread );
	DeclPfID;
public:

	KCnSMSDBThread();  // 구현부는 없음. 사용하지 않는다. (사용시 linking error 유발)
	virtual ~KCnSMSDBThread(void);

protected:
	virtual void ProcessEvent( const KEventPtr& spEvent );

protected:
	// packet send function
	template < class T > void SendToServer( unsigned short usEventID, const T& data );
	template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data );
	void SendToServer( unsigned short usEventID );
	void SendToUser( UidType nTo, unsigned short usEventID );
	void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID );

	// event handler
	DECL_ON_FUNC( DBE_SEND_PHONE_MSG_NOT );
	//{{ 2010. 11. 2	최육사	서버간 접속 모니터링
	//}}
};

template < class T >
void KCnSMSDBThread::SendToServer( unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), 0, PI_CN_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KCnSMSDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), 0, PI_CN_USER, nTo, NULL, usEventID, data );
}

template < class T >
void KCnSMSDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { nGS, -1 };
	KncSend( GetPfID(), 0, PI_GS_USER, nTo, anTrace, usEventID, data );
}
