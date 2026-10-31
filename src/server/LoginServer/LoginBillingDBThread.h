#pragma once
#include "BaseServer.h" // KBaseServer::GetKObj() in the templates below

#include "SubThread.h"
#include "ODBC/Odbc.h"
#include "ODBC/DBUtil.h"
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

//#include "BillManager.h"

class KLoginBillingDBThread : public KSubThread
{
	DeclareDBThread( KLoginBillingDBThread );
	DeclareSPProfiler;
	DeclPfID;

public:
	KLoginBillingDBThread();
	virtual ~KLoginBillingDBThread();

protected:
	// derived from KSubThread
	virtual void ProcessEvent( const KEventPtr& spEvent ); // Port: `inline` dropped, it is defined in the .cpp

	// packet send function
	template < class T > void SendToServer( unsigned short usEventID, const T& data );
	template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
	template < class T > void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data );

	//void SendToBillManager( unsigned short usEventID );
	void SendToServer( unsigned short usEventID );
	void SendToUser( UidType nTo, unsigned short usEventID );
	void SendToClient( UidType nTo, unsigned short usEventID );

	// 트랜잭션 시리얼넘버 생성
	int DoQuery_TransactionNumberServerGet( OUT __int64& iTranNo, OUT time_t& iTime );
	void MakeTID( __int64 iTransactionNo, time_t iTime, std::wstring& wstrTransaction
		);

	// 캐시 전환 주문 등록
	int DoQuery_BillOrderGameServer( IN const std::wstring& wstrOrderID,
										IN const UidType iUserUID,
										IN const std::wstring& wstrUserID,
										IN const int iServerGroupID,
										IN const UidType iUnitUID,
										IN const char cLevel,
										IN const int iChargeCash,
										IN const std::wstring& wstrUserIP,
										IN const char cOrderType
										);

	// 캐시 전환 주문 완료
	int DoQuery_BillOrderResult( IN const std::wstring& wstrOrderID,
									IN const int iOrderResult,
									OUT int& iTotalCash,
									OUT int& iChargeCash,
									OUT int& iChargeBonus,
									OUT int& iBonusItem,
									OUT UidType& iUnitUID
									, OUT KGlobalCashInfo& kRemainCashInfo
									);

	// 쿠폰 주문 등록
	int DoQuery_CouponOrder( IN const std::wstring& wstrCouponID,
		IN const std::wstring& wstrOrderID,
		IN const UidType iUserUID,
		IN const std::wstring& wstrUserID,
		IN const int iServerGroupID,
		IN const UidType iUnitUID,
		IN const char cLevel,
		IN const std::wstring& wstrUserIP,
		IN const char cOrderType );

	// 쿠폰 주문 완료
	int DoQuery_CouponOrderResult( IN const std::wstring& wstrOrderID,
		IN const int iOrderResult,
		IN int& iChargeItem,
		IN int& iChargeCash,
		IN int& iChargeBonus,
		OUT int& iTotalCash );

	int GetReleaseTick( OUT int& iReleaseTick );
	int GetProductInfo( OUT std::map< int, KBillProductInfo >& mapBillProductInfo );
	int GetProductAttribute( OUT std::map< int, KBillProductAttribute >& mapBillProductAttribute );
	int GetPackageInfo( OUT std::map< int, KBillPackageInfo >& mapBillPackageInfo );
	int GetOrderInfo( IN UidType iUserUID, OUT std::vector< KBillOrderInfo >& vecOrderInfo );
	int GetOrderInfoByOrderID( IN UidType iUserUID, IN const std::wstring& wstrOrderID, OUT std::vector< KBillOrderInfo >& vecOrderInfo );
	int InsertCashInventory( IN const std::wstring& wstrOrderNo, IN int iProductNo, IN UidType iUserUID, IN int iPrice, IN KBillOrderInfo::BUY_CASH_ITEM_TYPE eType, IN UidType iReceiverUserUID = 0, IN UidType iSenderUnitUID = 0, IN std::wstring wstrPresentMessage = L"" );
	int InsertCashLog( IN int iOrderIndex, IN UidType iUserUID, IN UidType iUnitUID, IN int iProductNoIn, OUT int& iProductNoOut );


	// event handler
	DECL_ON_FUNC( EBILL_EXCHANGE_CASH_RESERVE_REQ );
	DECL_ON_FUNC( EBILL_EXCHANGE_CASH_REQ );

	_DECL_ON_FUNC( EBILL_USE_COUPON_RESERVE_REQ, KEBILL_USE_COUPON_REQ );
	DECL_ON_FUNC( EBILL_USE_COUPON_RESULT_REQ );



};

template < class T >
void KLoginBillingDBThread::SendToServer( unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), 0, PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KLoginBillingDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), 0, PI_LOGIN_USER, nTo, NULL, usEventID, data );
}

template < class T >
void KLoginBillingDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data )
{
	UidType anTrace[2] = { nGS, -1 };
	KncSend( GetPfID(), 0, PI_GS_USER, nTo, anTrace, usEventID, data );
}
