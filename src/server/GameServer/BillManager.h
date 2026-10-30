#pragma once

#include <KNCSingleton.h>
#include <Thread/KCSLockType.h>
#include "Performer.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ClientPacket.h"
#include "SystemPacket.h"
#include "ServerPacket.h"

#define NUM_PRODUCT_PER_PAGE 800

class KBillManager : public KPerformer
{
	DeclPfID;
	DeclareSingleton( KBillManager );

public:
	KBillManager();
	virtual ~KBillManager();

	enum CASH_SHOP_RELEASE_TICK_TYPE
	{
		CSRTT_NORMAL			= 0,	// 캐시샵 목록 변경 tick
		CSRTT_BONUS_EVENT		= 1,	// 캐시샵 보너스 캐쉬 이벤트 tick
	};


	bool CheckBonusPeriodTime(std::vector<std::wstring> &_vecStartTime, std::vector<std::wstring> &_vecEndTime );
	int GetReleaseTick(CASH_SHOP_RELEASE_TICK_TYPE _enum);
	void SetReleaseTick(CASH_SHOP_RELEASE_TICK_TYPE _enum ,std::map<int, int> &mapReleaseTick );
	bool GetBonusCashEvent()							{ return m_bCheckBonusTime; }
	void SetBonusCashEvent(bool bCheckBonusTime)		{ m_bCheckBonusTime = bCheckBonusTime; }

	void GetBillProductCategory( std::vector< KBillProductCategory >& vecBillProductCategory );
	void SetBillProductCategory( const std::vector< KBillProductCategory >& vecBillProductCategory );

	void GetBillProductInfo( std::map< int, KBillProductInfo >& mapBillProductInfo );
	void SetBillProductInfo( const std::map< int, KBillProductInfo >& mapBillProductInfo );

	void GetBillProductAttribute( std::map< int, KBillProductAttribute >& mapBillProductAttribute );
	void SetBillProductAttribute( const std::map< int, KBillProductAttribute >& mapBillProductAttribute );

	void GetBillPackageInfo( std::map< int, KBillPackageInfo >& mapBillPackageInfo );
	void SetBillPackageInfo( const std::map< int, KBillPackageInfo>& mapBillPackageInfo );

	bool GetBillProductPage( int iPage, std::vector< KBillProductInfo >& vecBillProductInfo, bool& bIsFinal );
	void SetBillProductPage();

	int GetItemID( int iProductNo );
	bool GetProductInfo( int iProductNo, KBillProductInfo& kInfo );
	bool GetPackageInfo( int iProductNo, KBillPackageInfo& kInfo );
	bool GetProductAttribute( int iProductNo, KBillProductAttribute& kInfo );

	bool IsPackage( int iProductNo );

	bool GetPrice( int iProductNo, int& iPrice );
	bool GetTotalPrice( const std::vector< KNXBTPurchaseReqInfo >& vecPurchaseReqInfo, int& iPrice );

	unsigned long GetNextPurchaseNo();

	// derived from KPerformer
	virtual void Tick();

	std::set<int> GetSellCashItemList();

protected:
	// derived from KPerformer
	virtual void ProcessEvent( const KEventPtr& spEvent );

	void CheckProductInfo();

	// event handler
	DECL_ON_FUNC( EBILL_CHECK_PRODUCT_INFO_ACK );

public:

protected:
	std::map<int, int>								m_mapReleaseTick;

	std::vector< KBillProductCategory >             m_vecBillProductCategory;
	std::map< int, KBillProductInfo >               m_mapBillProductInfo;
	std::map< int, KBillProductAttribute >          m_mapBillProductAttribute;
	std::map< int, KBillPackageInfo >               m_mapBillPackageInfo;
	std::vector< std::vector< KBillProductInfo > >  m_vecBillProductPage;
	DWORD                                           m_dwLastProductCheckTick;
	static DWORD                                    ms_dwProductCheckGap;
	static DWORD                                    ms_dwCashEventCheckGap;
	DWORD                                           m_dwLasCashEvnetCheckTick;

	KCSLOCK_DECLARE( unsigned long,					m_ulCurrentPurchaseNo );// SimLayerThread
	std::vector<std::wstring>						m_vecStartBonusTime;
	std::vector<std::wstring>						m_vecEndBonusTime;
	bool											m_bCheckBonusTime;
};

DefSingletonInline( KBillManager );
