#pragma once

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"
#include "SubThread.h"
#include "KncException.h"
#include "BillManager.h"
#include "BaseServer.h"
#include "DEB/SoapClient.h"

class KDEBBilling : public KSubThread
{
	DeclPfID;
	_DeclareException;

public:
	KDEBBilling(std::string WsdlFile);
	~KDEBBilling(void);
	virtual void Run();

	// packet send function
	template < class T > void SendToServer( unsigned short usEventID, const T& data );
	template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToKOGBillingDB( UidType nUserUID, unsigned short usEventID, const T& data );
	void SendToServer( unsigned short usEventID );
	void SendToUser( UidType nTo, unsigned short usEventID );
	void SendToKOGBillingDB( UidType nUserUID, unsigned short usEventID );

	//----------------------//
	// Process Event handle //
	//----------------------//
	virtual void ProcessEvent( const KEventPtr& spEvent );

	// event handler
	DECL_ON_FUNC( EPUBLISHER_BILLING_BALANCE_REQ );
	DECL_ON_FUNC( EBILL_BUY_PRODUCT_REQ );
	DECL_ON_FUNC( EBILL_GIFT_ITEM_REQ );
	DECL_ON_FUNC( EBILL_USE_COUPON_REQ );

protected:
	x2::deb::SoapClient m_kSoapClient;	// Port: own SOAP/HTTP client in place of gSOAP (decision T31)
};


template < class T >
void KDEBBilling::SendToServer( unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_PUBLISHER_BILLING, 0, PI_GS_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KDEBBilling::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
	KncSend( PI_GS_PUBLISHER_BILLING, 0, PI_GS_USER, nTo, NULL, usEventID, data );
}


template < class T >
void KDEBBilling::SendToKOGBillingDB( UidType nUserUID, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { nUserUID, -1 };
	KncSend( PI_GS_PUBLISHER_BILLING, 0, PI_GS_KOG_BILLING_DB, 0, anTrace, usEventID, data );
}
