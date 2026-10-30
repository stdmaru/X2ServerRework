#include <string>
#include "./DEBBilling.h"


#include "NetError.h"

#include "DEB/soapH.h" // or whatever it is called, you must already have it
#include "DEB/AuthService.nsmap" // this is what you have to add to fix the problem


#define CLASS_TYPE  KDEBBilling

ImplPfID( KDEBBilling, PI_GS_PUBLISHER_BILLING );

_ImplementException( KDEBBilling );

KDEBBilling::KDEBBilling(std::string WsdlFile)
{
	m_strWsdlPath = WsdlFile;
	p_gf_soap = NULL;
}

KDEBBilling::~KDEBBilling(void)
{
	End( 1500 );
}

void KDEBBilling::Run()
{
	std::cout << "KSubThread::Run(), thread start." << std::endl;
	START_LOG( cout2, L"[Test Log] DEB Billing Start" ) << END_LOG;

	SoapInitialize();

	START_LOG( cout2, L"[Test Log] DEB Billing Soap Initialized" ) << END_LOG;

	while( true )
	{
		//{{ 2010/11/11 Jo Hyo-jin If SOAP module creation fails, re-create it //
		if( p_gf_soap == NULL )
        {
			SoapInitialize();
		}

		DWORD ret = ::WaitForSingleObject( m_hKillEvent, 1 );

		if( ret == WAIT_OBJECT_0 ) break;
		if( ret == WAIT_TIMEOUT ) Loop();

		else std::cout << "*** WaitForSingleObject() - return :" << ret << std::endl;
	}

	SoapUninitialize();

	START_LOG( cout2, L"[Test Log] DEB Billing Soap End" ) << END_LOG;
}

void KDEBBilling::SoapInitialize()
{
	p_gf_soap = soap_new();
	if( !p_gf_soap )	//// couldn't allocate: stop
	{
		p_gf_soap = NULL;

		START_LOG( cerr, L"Soap_new Error" ) << END_LOG;

		return;
	}
}

void KDEBBilling::SoapUninitialize()
{
	if( p_gf_soap != NULL )
	{
		soap_destroy(p_gf_soap);
		soap_end(p_gf_soap);
		p_gf_soap = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// packet send function
void KDEBBilling::SendToServer( unsigned short usEventID )
{
	SendToServer( usEventID, char() );
}

void KDEBBilling::SendToUser( UidType nTo, unsigned short usEventID )
{
	SendToUser( nTo, usEventID, char() );
}


void KDEBBilling::SendToKOGBillingDB( UidType nUserUID, unsigned short usEventID )
{
	SendToKOGBillingDB( nUserUID, usEventID, char() );
}

//----------------------//
// Process Event handle //
//----------------------//
void KDEBBilling::ProcessEvent( const KEventPtr& spEvent_ )
{
	KSerializer ks;
	ks.BeginReading( &spEvent_->m_kbuff );

	switch( spEvent_->m_usEventID )
	{
		CASE( EPUBLISHER_BILLING_BALANCE_REQ );
		CASE( EBILL_BUY_PRODUCT_REQ );
		CASE( EBILL_GIFT_ITEM_REQ );
		CASE( EBILL_USE_COUPON_REQ );

	    default:
		    START_LOG( cerr, L"Event handler is not defined. " << spEvent_->GetIDStr() );
	}
}


IMPL_ON_FUNC( EPUBLISHER_BILLING_BALANCE_REQ )
{
    // -- EPUBLISHER_BILLING_BALANCE_REQ --
	//unsigned int						m_uiPublisherUID;
	//UidType							m_uiActorUID;
	//std::wstring						m_wstrAccount;					// [65]
	//std::wstring						m_wstrIP;
    // -- EPUBLISHER_BILLING_BALANCE_ACK --
	//int								m_iOK;
	//unsigned long						m_ulBalance;

    KEPUBLISHER_BILLING_BALANCE_ACK kPacketAck;
    kPacketAck.m_iOK = NetError::ERR_UNKNOWN;
    kPacketAck.m_ulBalance = 0;

        // When we look at the cache type as one, we generally see it as public cache.
        kPacketAck.m_iCashType = KGlobalCashInfo::GCT_PUBLISHER_CASH;

	_ns2__GetUserCashRequest request;
	request.uiPublisherUID = kPacket_.m_uiPublisherUID;

	_ns2__GetUserCashResponse response;
	int soap_result = soap_call___ns1__GetUserCash(p_gf_soap, m_strWsdlPath.c_str(), NULL, &request, &response);
	if(soap_result == SOAP_OK) {
		kPacketAck.m_iOK = response.iOK;
		kPacketAck.m_ulBalance = static_cast<ULONG>(response.ulBalance);
	}

    SendToUser( FIRST_SENDER_UID, EPUBLISHER_BILLING_BALANCE_ACK, kPacketAck );
}

IMPL_ON_FUNC( EBILL_BUY_PRODUCT_REQ )
{
    //  -- EBILL_BUY_PRODUCT_REQ (kPacket_) --
	//int                           m_iServerGroupID;
	//int                           m_iChannelingCode;
	//std::wstring                  m_wstrUserID;
	//unsigned int                  m_uiPublisherUID;
	//UidType                       m_iUserUID;
	//UidType                       m_iUnitUID;
	//std::wstring                  m_wstrNickName;
	//std::string                   m_strUserIP;
	//std::vector<KBillBuyInfo>     m_vecBillBuyInfo;
	//KBuyCharInfo				    m_BuyCharInfo;
	//std::wstring                  m_wstrTotalOrderID;
    //  -- KEBILL_BUY_PRODUCT_ACK (kPacketAck) --
    //int                           m_iOK;
	//vector<???>                   m_vecBillOrderInfo;
	//vector<???>                   m_vecPackageID;
    //int                           m_iTotalCash;
    KEBILL_BUY_PRODUCT_REQ kPacketReq = kPacket_;

    KEBILL_BUY_PRODUCT_ACK kPacketAck;
    kPacketAck.m_iOK = NetError::ERR_UNKNOWN;

    // Configure the purchase amount and item list.
    [[maybe_unused]] int iTotalPrice = 0;
	std::string strTotalList = ""; // Format: ProductNo_ProductID_count_price|ProductNo_ProductID_TotalCount_TotalPrice.

	[[maybe_unused]] int count = 0;
    std::vector< KBillBuyInfo >::iterator vit;
    for( vit = kPacket_.m_vecBillBuyInfo.begin(); vit != kPacket_.m_vecBillBuyInfo.end(); vit++ )
    {
		if( vit != kPacket_.m_vecBillBuyInfo.begin() ) {
			strTotalList += "|";
        }

		strTotalList += std::to_string( vit->m_iProductNo ); // Port: was _itoa( ..., 10 ) into tmp
		strTotalList += "_";

		strTotalList += std::to_string( vit->m_iProductID ); // Port: was _itoa( ..., 10 ) into tmp
		strTotalList += "_";

		strTotalList += std::to_string( vit->m_iPoint ); // Port: was _itoa( ..., 10 ) into tmp

		iTotalPrice += vit->m_iPoint;

		count++;
    }

	// Build request
    _ns2__PurchaseProductRequest request;
    request.uiPublisherUID = kPacket_.m_uiPublisherUID;
	request.iChannelingCode = kPacket_.m_iChannelingCode;
	request.iUnitUID = static_cast<int>(kPacket_.m_iUnitUID);
	request.strUserIP = kPacket_.m_strUserIP;
	request.strNickName = KncUtil::toNarrowString(kPacket_.m_wstrNickName);
	request.strTotalOrderID = KncUtil::toNarrowString(kPacket_.m_wstrTotalOrderID);
	request.strTotalList = strTotalList;

    _ns2__PurchaseProductResponse response;
	int soap_result = soap_call___ns1__PurchaseProduct(p_gf_soap, m_strWsdlPath.c_str(), NULL, &request, &response);

	if( soap_result == SOAP_OK )
	{
        // Purchase fail processing
        if( response.iOK != NetError::NET_OK ) {
            kPacketAck.m_iOK = response.iOK;

            START_LOG( clog, L"[DEB] Purchase request has been rejected." )
                << BUILD_LOG( response.iOK )
                << BUILD_LOG( NetError::GetErrStr(response.iOK) )
                << BUILD_LOG( kPacket_.m_wstrTotalOrderID )
                << BUILD_LOG( kPacket_.m_wstrUserID )
                << BUILD_LOG( kPacket_.m_wstrNickName )
                << BUILD_LOG( strTotalList )
                << END_LOG;

            SendToUser( FIRST_SENDER_UID, EBILL_BUY_PRODUCT_ACK, kPacketAck );
            return;
        }

        // if successful just do nothing, do finance logic in publisher side
        // and forward the untouched packet back to KOG Billing Processing down below.
	}
    else
    {
		START_LOG( cerr, L"[DEB] AuthService has encountered a fatal error upon processing purchase request." )
			<< BUILD_LOG( soap_result )
			<< BUILD_LOG( kPacket_.m_wstrTotalOrderID )
			<< BUILD_LOG( kPacket_.m_wstrUserID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< BUILD_LOG( strTotalList )
			<< END_LOG;

		SendToUser( FIRST_SENDER_UID, EBILL_BUY_PRODUCT_ACK, kPacketAck );
		return;
    }

    SendToKOGBillingDB( FIRST_SENDER_UID, EBILL_BUY_PRODUCT_REQ, kPacketReq );
}

IMPL_ON_FUNC( EBILL_GIFT_ITEM_REQ )
{
    //IMPLEMENTATION
    return;
}


IMPL_ON_FUNC( EBILL_USE_COUPON_REQ )
{
    //IMPLEMENTATION
    return;
}
