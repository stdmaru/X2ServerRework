#pragma once
#include "CommonPacket.h"


SmartPointer(KGSUser);


//{{ 2010. 07. 05  최육사	유저 어뷰저 매니저


class KUserAbuserManager
{
public:
	enum ABUSER_ENUM
	{
		AE_ED_ABUSER_LIMIT = 1000000000, // 10억 이상 ED보유시 ED어뷰저로 판단!
		//{{ 2010. 11. 17	최육사	오토핵 검증 기능
		AE_AUTO_HACK_CHECK_LEVEL = 7,
		//}}
	};


protected:
	enum ACCOUNT_ABUSER_TYPE
	{
		AT_MORNITORING_ABUSER = 0,

		AT_MAX,
	};

	enum CHAR_ABUSER_TYPE
	{
		CAT_ITEM_ABUSER = 0,
		CAT_ENCHANT_ABUSER,
		CAT_ED_ABUSER,
		//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
		//}}

		CAT_MAX,
	};

public:
	KUserAbuserManager(void);
	~KUserAbuserManager(void);

	void Clear( bool bOnlyCharAbuser = false );
	void InitAccountAbuser( IN KGSUserPtr spUser );
	void InitCharAbuser( IN KGSUserPtr spUser );

	bool IsEnchantAbuser()			{ return m_bIsCharAbuser[CAT_ENCHANT_ABUSER]; }
	//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
	//}}

	// 어뷰저 등록 함수
	void RegEDAbuser( IN KGSUserPtr spUser );
	void RegItemAbuser( IN KGSUserPtr spUser );
	//{{ 2011. 09. 08	최육사	오토핵 패킷 모니터링
	void RegPacketMornitoring( IN KGSUserPtr spUser );
	//}}

	// 어뷰저 체크 함수
	//{{ 2013. 05. 29	최육사	패킷 모니터링 근성도 항목 추가
	void CheckMornitoringAbuser( IN const UidType iUserUID, IN const UidType iUnitUID, IN const int iED, IN const int iSpirit, IN const unsigned short usEventID );
	//}}
	void CheckItemAbuser( IN KGSUserPtr spUser, IN const int iGetItemState, IN const int iItemID, IN const int iQuantity );
	void CheckItemAbuser( IN KGSUserPtr spUser, IN const int iGetItemState, IN const std::map< int, int >& mapGetItemInfo );
	void CheckEDAbuser( IN KGSUserPtr spUser, IN const int iGetEDState, IN const int iBeforeED );
	//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	void IncreaseGetItemCount( IN KGSUserPtr spUser, IN int iCount );
	int GetItemCount() const		{ return m_iGetItemCount; }
	int GetItemCountMin() const		{ return m_iGetItemCountMin; }
	int GetItemCountMax() const		{ return m_iGetItemCountMax; }
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	bool IsAutoHackLevelUpUser( IN u_char ucCurrentCharLevel
								, UidType iCurrentCharUID
		);
	//}}
	//{{ 2011. 03. 02	최육사	개인 거래 차단
	void SetTradeBlockUser( IN const bool bTradeBlock ) { m_bIsTradeBlock = bTradeBlock; }
	bool IsTradeBlockUser()			{ return m_bIsTradeBlock; }
	//}}
	void SetTradeBlockUnit( IN const bool bIsTradeBlockUnit ) { m_bIsTradeBlockUnit = bIsTradeBlockUnit; }
	bool IsTradeBlockUnit()			{ return m_bIsTradeBlockUnit; }



protected:
	bool CheckAbuserEvent( unsigned short usEventID );

protected:
	bool							m_bIsAccountAbuser[AT_MAX];
	bool							m_bIsCharAbuser[CAT_MAX];

	//{{ 2010. 9. 8	최육사	네트워크 트래픽 유발 유저 모니터링
	//}}

	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	int								m_iGetItemCount;
	int								m_iGetItemCountMin;
	int								m_iGetItemCountMax;
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	CTime							m_tLastLevelUpTime;
	UidType							m_iLastLevelUpUID;
	//}}
	//{{ 2011. 03. 02	최육사	개인 거래 차단
	bool							m_bIsTradeBlock;
	//}}

	bool							m_bIsTradeBlockUnit;

};


//}}
