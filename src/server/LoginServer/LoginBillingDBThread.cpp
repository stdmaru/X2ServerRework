#include <winsock2.h>
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "LoginBillingDBThread.h"
#include "LoginServer.h"
#include "NetError.h"



//ImplementDBThread( KLoginBillingDBThread );
ImplPfID( KLoginBillingDBThread, PI_LOGIN_KOG_BILLING_DB );

#define CLASS_TYPE KLoginBillingDBThread

KLoginBillingDBThread::~KLoginBillingDBThread()
{
}

void KLoginBillingDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
	KSerializer ks;
	ks.BeginReading( &spEvent_->m_kbuff );

	switch( spEvent_->m_usEventID )
	{

		CASE( EBILL_EXCHANGE_CASH_RESERVE_REQ );
		CASE( EBILL_EXCHANGE_CASH_REQ );

		_CASE( EBILL_USE_COUPON_RESERVE_REQ, KEBILL_USE_COUPON_REQ );
		CASE( EBILL_USE_COUPON_RESULT_REQ );


	default:
		START_LOG( cerr, L"이벤트 핸들러가 정의되지 않았음. " << spEvent_->GetIDStr() );
	}
}

//void KLoginBillingDBThread::SendToBillManager( unsigned short usEventID )
//{
//	SendToBillManager( usEventID, char() );
//}

void KLoginBillingDBThread::SendToServer( unsigned short usEventID )
{
	SendToServer( usEventID, char() );
}

void KLoginBillingDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
	SendToUser( nTo, usEventID, char() );
}


IMPL_ON_FUNC( EBILL_EXCHANGE_CASH_RESERVE_REQ )
{
	KEBILL_EXCHANGE_CASH_RESERVE_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_kPacketReq = kPacket_;

	// 트랜잭션 생성
	__int64 iTransactionNo = 0;
	time_t iTime = 0;
	kPacket.m_iOK = DoQuery_TransactionNumberServerGet( iTransactionNo, iTime );

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		switch( kPacket.m_iOK )
		{
		case -1:
			kPacket.m_iOK = NetError::ERR_GIANT_BILLING_08;
			break;
		}

		START_LOG( cerr, L"빌링 트랜잭션 번호 생성 실패" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< END_LOG;

		goto end_proc;
	}

	MakeTID( iTransactionNo, iTime, kPacket.m_wstrTransaction );


	// 주문 등록
	kPacket.m_iOK = DoQuery_BillOrderGameServer( kPacket.m_wstrTransaction,
		kPacket_.m_PurchaserInfo.m_iUserUID,
		kPacket_.m_PurchaserInfo.m_wstrUserName,
		kPacket_.m_PurchaserInfo.m_iServerGroupID,
		kPacket_.m_PurchaserInfo.m_iUnitUID,
		kPacket_.m_PurchaserInfo.m_ucLevel,
		kPacket_.m_iPoint,
		kPacket_.m_PurchaserInfo.m_wstrIP,
		1
		);

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"자이언트 캐시 전환 주문 등록 실패." )
			<< BUILD_LOG( kPacket.m_wstrTransaction )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_wstrUserName )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_iServerGroupID )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_iUnitUID )
			<< BUILD_LOGc( kPacket_.m_PurchaserInfo.m_ucLevel )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_wstrIP )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_uiPublisherUID )
			<< BUILD_LOG( kPacket_.m_iPoint )
			<< END_LOG;

		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, EBILL_EXCHANGE_CASH_RESERVE_ACK, kPacket );
}

IMPL_ON_FUNC( EBILL_EXCHANGE_CASH_REQ )
{
	KEBILL_EXCHANGE_CASH_ACK kPacket;

	kPacket.m_iOK = DoQuery_BillOrderResult( kPacket_.m_wstrTransaction,
											 kPacket_.m_iRet,
											 kPacket.m_iTotalCash,
											 kPacket.m_iChargeCash,
											 kPacket.m_iChargeBonus,
											 kPacket.m_iBonusItem,
											 kPacket.m_iUnitUID
											 , kPacket.m_RemainCashInfo
											 );

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"자이언트 캐시 전환 DB 기록 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
			<< END_LOG;

		goto end_proc;
	}
	//2011.07.26 lygan_조성욱 // 잔액부족 에러 메시지 출력
	if(kPacket_.m_iRet == -3)
	{
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_02;
		goto end_proc;
	}


	/*
	if( kPacket_.m_iPoint != kPacket.m_iChargeCash )
	{
		START_LOG( cerr, L"충전 예정 금액과 실제 충전 금액이 다름." )
			<< BUILD_LOG( kPacket_.m_iPoint )
			<< BUILD_LOG( kPacket.m_iChargeCash )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_08;
	}
	*/

	if( kPacket_.m_iPoint < 0 )
	{
		START_LOG( cerr, L"충전 금액이 마이너스." )
			<< BUILD_LOG( kPacket_.m_iPoint )
			<< BUILD_LOG( kPacket.m_iChargeCash )
			<< END_LOG;

		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_08;
	}

end_proc:
	SendToGSUser( kPacket_.m_iGSUID, kPacket_.m_iUserUID, EBILL_EXCHANGE_CASH_ACK, kPacket );
}

_IMPL_ON_FUNC( EBILL_USE_COUPON_RESERVE_REQ, KEBILL_USE_COUPON_REQ )
{
	KEBILL_USE_COUPON_RESERVE_ACK kPacket;
	kPacket.m_iOK = NetError::ERR_ODBC_01;
	kPacket.m_kPacketReq = kPacket_;

	// 트랜잭션 생성
	__int64 iTransactionNo = 0;
	time_t iTime = 0;
	kPacket.m_iOK = DoQuery_TransactionNumberServerGet( iTransactionNo, iTime );

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		switch( kPacket.m_iOK )
		{
		case -1:
			kPacket.m_iOK = NetError::ERR_GIANT_BILLING_05;
			break;
		}

		START_LOG( cerr, L"빌링 트랜잭션 번호 생성 실패" )
			<< BUILD_LOG( kPacket.m_iOK )
			<< END_LOG;

		goto end_proc;
	}

	MakeTID( iTransactionNo, iTime, kPacket.m_kPacketReq.m_wstrTransaction );

	// 주문 등록
	kPacket.m_iOK = DoQuery_CouponOrder( kPacket_.m_wstrSerialCode,
		kPacket.m_kPacketReq.m_wstrTransaction,
		kPacket_.m_PurchaserInfo.m_iUserUID,
		kPacket_.m_PurchaserInfo.m_wstrUserName,
		kPacket_.m_PurchaserInfo.m_iServerGroupID,
		kPacket_.m_PurchaserInfo.m_iUnitUID,
		kPacket_.m_PurchaserInfo.m_ucLevel,
		kPacket_.m_PurchaserInfo.m_wstrIP,
		1	// 중국 제외하고는 그냥 1로 타입 고정함
		);

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"자이언트 쿠폰 주문 등록 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( kPacket_.m_wstrSerialCode )
			<< BUILD_LOG( kPacket.m_kPacketReq.m_wstrTransaction )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_wstrUserName )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_iServerGroupID )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_iUnitUID )
			<< BUILD_LOGc( kPacket_.m_PurchaserInfo.m_ucLevel )
			<< BUILD_LOG( kPacket_.m_PurchaserInfo.m_wstrIP )
			<< END_LOG;

		goto end_proc;
	}

end_proc:
	SendToUser( LAST_SENDER_UID, EBILL_USE_COUPON_RESERVE_ACK, kPacket );
}

IMPL_ON_FUNC( EBILL_USE_COUPON_RESULT_REQ )
{
	KEBILL_USE_COUPON_RESULT_ACK kPacket;

	kPacket.m_iOK = DoQuery_CouponOrderResult( kPacket_.m_wstrTransaction,
		kPacket_.m_iRet,
		kPacket_.m_iChargeItem,
		kPacket_.m_iChargeCash,
		kPacket_.m_iChargeBonus,
		kPacket.m_iTotalCash );

	if( kPacket.m_iOK != NetError::NET_OK )
	{
		START_LOG( cerr, L"자이언트 쿠폰 사용 DB 기록 실패." )
			<< BUILD_LOG( kPacket.m_iOK )
			<< BUILD_LOG( NetError::GetErrStr( kPacket.m_iOK ) )
			<< END_LOG;
	}

	switch( kPacket_.m_iRet )
	{
	case KEBILL_USE_COUPON_REQ::GB_RCT_FAIL:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_09;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_ACCOUNT_NOT_EXIST:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_01;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_CASH_NOT_EXIST:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_02;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_ITEM_CARD_NOT_EXIST:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_03;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_ITEM_CARD_NOT_AREA:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_04;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_ITEM_CARD_DUPLICATE:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_05;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_ZONE_CARD_ID_ERROR:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_06;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_ZONE_CARD_PASSWORD_ERROR:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_07;
		break;
	case KEBILL_USE_COUPON_REQ::GB_RCT_DB_ERROR:
		kPacket.m_iOK = NetError::ERR_GIANT_BILLING_08;
		break;
	}

	START_LOG( clog, L"쿠폰 사용 결과" )
		<< BUILD_LOG( kPacket_.m_wstrTransaction )
		<< BUILD_LOG( kPacket_.m_iRet )
		<< BUILD_LOG( kPacket_.m_iChargeItem )
		<< BUILD_LOG( kPacket_.m_iChargeCash )
		<< BUILD_LOG( kPacket_.m_iChargeBonus )
		<< BUILD_LOG( kPacket.m_iOK )
		<< BUILD_LOG( NetError::GetErrStr(kPacket.m_iOK) )
		<< BUILD_LOG( kPacket.m_iTotalCash )
		<< END_LOG;

	SendToGSUser( kPacket_.m_iGSUID, kPacket_.m_iUserUID, EBILL_USE_COUPON_RESULT_ACK, kPacket );
}



int KLoginBillingDBThread::DoQuery_BillOrderGameServer( IN const std::wstring& wstrOrderID,
														IN const UidType iUserUID,
														IN const std::wstring& wstrUserID,
														IN const int iServerGroupID,
														IN const UidType iUnitUID,
														IN const char cLevel,
														IN const int iChargeCash,
														IN const std::wstring& wstrUserIP,
														IN const char cOrderType
														)
{
	int iRet = NetError::ERR_ODBC_01;

	DO_QUERY_NO_PROFILE( L"exec dbo.EBP_BillOrder_GameServer_SET", L"N\'%s\', %d, N\'%s\', %d, %d, %d, %d, N\'%s\', %d",
		% wstrOrderID
		% iUserUID
		% wstrUserID
		% iServerGroupID
		% iUnitUID
		% (int)cLevel
		% iChargeCash
		% wstrUserIP
		% (int)cOrderType
		);

	int iOK;

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}
	else
	{
		goto end_proc;
	}



	if( iOK != 0 )
	{
		iRet = NetError::ERR_EXCHANGE_CASH;
		goto end_proc;
	}

	iRet = NetError::NET_OK;

end_proc:
	LOG_SUCCESS( iRet == NetError::NET_OK )
		<< BUILD_LOG( iRet )
		<< BUILD_LOG( NetError::GetErrStr( iRet ) )
		<< BUILD_LOG( wstrOrderID )
		<< BUILD_LOG( iUserUID )
		<< BUILD_LOG( wstrUserID )
		<< BUILD_LOG( iServerGroupID )
		<< BUILD_LOG( iUnitUID )
		<< BUILD_LOG( cLevel )
		<< BUILD_LOG( iChargeCash )
		<< BUILD_LOG( wstrUserIP )
		<< BUILD_LOG( (int)cOrderType )
		<< BUILD_LOG( iOK )
		<< END_LOG;

	return iRet;
}

int KLoginBillingDBThread::DoQuery_BillOrderResult( IN const std::wstring& wstrOrderID,
												IN const int iOrderResult,
												OUT int& iTotalCash,
												OUT int& iChargeCash,
												OUT int& iChargeBonus,
												OUT int& iBonusItem,
												OUT UidType& iUnitUID
												, OUT KGlobalCashInfo& kRemainCashInfo
												)
{
	int iRet = NetError::ERR_ODBC_01;
	DO_QUERY_NO_PROFILE( L"exec dbo.EBP_BillOrder_Result", L"N\'%s\', %d",
		% wstrOrderID
		% iOrderResult
		);

	int iOK;
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK
			>> iTotalCash
			>> iChargeCash
			>> iChargeBonus
			>> iBonusItem
			>> iUnitUID

			>> kRemainCashInfo.m_ulCash[KGlobalCashInfo::GCT_KOG_ELSWORD_CASH]
			>> kRemainCashInfo.m_ulCash[KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT]

			);
		m_kODBC.EndFetch();
	}
	else
	{
		goto end_proc;
	}

	if( iOK != 0 )
	{
		iRet = NetError::ERR_NX_SHOP_03;
		goto end_proc;
	}

	iRet = NetError::NET_OK;

end_proc:
	LOG_SUCCESS( iRet == NetError::NET_OK )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( wstrOrderID )
		<< BUILD_LOG( iOrderResult )
		<< BUILD_LOG( iRet )
		<< BUILD_LOG( iTotalCash )
		<< BUILD_LOG( iChargeCash )
		<< BUILD_LOG( iChargeBonus )
		<< BUILD_LOG( iBonusItem )
		<< BUILD_LOG( iUnitUID )
		<< BUILD_LOG( kRemainCashInfo.m_ulCash[KGlobalCashInfo::GCT_KOG_ELSWORD_CASH] )
		<< BUILD_LOG( kRemainCashInfo.m_ulCash[KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT] )
		<< END_LOG;

	return iRet;
}

int KLoginBillingDBThread::DoQuery_CouponOrder( IN const std::wstring& wstrCouponID,
												IN const std::wstring& wstrOrderID,
												IN const UidType iUserUID,
												IN const std::wstring& wstrUserID,
												IN const int iServerGroupID,
												IN const UidType iUnitUID,
												IN const char cLevel,
												IN const std::wstring& wstrUserIP,
												IN const char cOrderType )
{
	int iOK = NetError::ERR_ODBC_01;
	DO_QUERY_NO_PROFILE( L"exec dbo.EBP_CouponOrder_SET", L"N\'%s\', N\'%s\', %d, N\'%s\', %d, %d, %d, N\'%s\', %d",
		% wstrCouponID
		% wstrOrderID
		% iUserUID
		% wstrUserID
		% iServerGroupID
		% iUnitUID
		% (int)cLevel
		% wstrUserIP
		% (int)cOrderType
		);

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK );
		m_kODBC.EndFetch();
	}

	switch( iOK )
	{
	case NetError::NET_OK:	// 성공
		break;
	case -11:	// 이미 사용한 쿠폰입니다.
		iOK = NetError::ERR_GIANT_BILLING_05;
		break;
	default:
		iOK = NetError::ERR_GIANT_BILLING_08;
		break;
	}

end_proc:
	LOG_SUCCESS( iOK == NetError::NET_OK )
		<< BUILD_LOG( wstrCouponID )
		<< BUILD_LOG( wstrOrderID )
		<< BUILD_LOG( iUserUID )
		<< BUILD_LOG( wstrUserID )
		<< BUILD_LOG( iServerGroupID )
		<< BUILD_LOG( iUnitUID )
		<< BUILD_LOGc( cLevel )
		<< BUILD_LOG( wstrUserIP )
		<< BUILD_LOGc( cOrderType )
		<< BUILD_LOG( iOK )
		<< END_LOG;

	return iOK;
}

int KLoginBillingDBThread::DoQuery_CouponOrderResult( IN const std::wstring& wstrOrderID,
														IN const int iOrderResult,
														IN int& iChargeItem,
														IN int& iChargeCash,
														IN int& iChargeBonus,
														OUT int& iTotalCash )

{
	int iRet = NetError::ERR_ODBC_01;
	DO_QUERY_NO_PROFILE( L"exec dbo.EBP_CouponOrder_Result", L"N\'%s\', %d, %d, %d, %d",
		% wstrOrderID
		% iOrderResult
		% iChargeItem
		% iChargeCash
		% iChargeBonus
		);

	int iOK;
	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK
			>> iTotalCash );
		m_kODBC.EndFetch();
	}
	else
	{
		goto end_proc;
	}

	if( iOK != 0 )
	{
		iRet = NetError::ERR_GIANT_BILLING_05;
		goto end_proc;
	}

	iRet = NetError::NET_OK;

end_proc:
	LOG_SUCCESS( iRet == NetError::NET_OK )
		<< BUILD_LOG( iRet )
		<< BUILD_LOG( NetError::GetErrStr( iRet ) )
		<< BUILD_LOG( iOK )
		<< BUILD_LOG( wstrOrderID )
		<< BUILD_LOG( iOrderResult )
		<< BUILD_LOG( iTotalCash )
		<< BUILD_LOG( iChargeItem )
		<< BUILD_LOG( iChargeCash )
		<< BUILD_LOG( iChargeBonus )
		<< END_LOG;

	return iRet;
}

int KLoginBillingDBThread::DoQuery_TransactionNumberServerGet( OUT __int64& iTranNo, OUT time_t& iTime )
{
	int iRet = NetError::ERR_ODBC_01;
	DO_QUERY_NO_ARG( L"exec dbo.EBP_TranNumberServer_INT" );

	int iOK;
	__int64 _time;

	if( m_kODBC.BeginFetch() )
	{
		FETCH_DATA( iOK
			>> iTranNo
			>> _time );
		m_kODBC.EndFetch();
	}
	else
	{
		goto end_proc;
	}

	if( iOK != 0 )
	{
		iRet = NetError::ERR_GIANT_BILLING_08;
		goto end_proc;
	}

	iRet = NetError::NET_OK;
	iTime = static_cast<time_t>(_time);

end_proc:
	LOG_SUCCESS( iRet == NetError::NET_OK )
		<< BUILD_LOG( iRet )
		<< BUILD_LOG( NetError::GetErrStr( iRet ) )
		<< BUILD_LOG( iTranNo )
		<< BUILD_LOG( iTime )
		<< END_LOG;

	return iRet;
}

void KLoginBillingDBThread::MakeTID(__int64 iTransactionNo, time_t iTime, std::wstring& wstrTransaction
	)
{
	boost::wformat wfmt(L"%02x%03x%08x%08x");

	wstrTransaction = boost::str(wfmt
		% 1
		% 1
		% iTime
		% iTransactionNo );

}
