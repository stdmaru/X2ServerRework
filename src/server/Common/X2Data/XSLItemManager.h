#pragma once

#include "RefreshSingleton.h"
#include "XSLItem.h"
#include "Lottery.h"
#include "XSLUnitManager.h"



class CXSLItemManager
{
	DeclareRefreshSingleton( CXSLItemManager );
	DeclareLuaScriptParser;
	DeclDumpToLogFile;
	DeclDump;
	DeclToStringW;

public:
	enum ITEM_EXCHANGE_TYPE
	{
		IET_INVALID = 0,
		IET_ALL,
		IET_RANDOM,
		IET_SELECT,
	};

	//{{ 2012. 10. 09	김민성	배틀필드 아이템 레벨 시스템
	enum ITEM_ENCHANT_STANDARD_VALUE
	{
		IESV_PHYSIC_ATTACK	= 120,		// 아이템 1레벨 물공 증가치 (이만큼이 증가해야 1레벨이 오름)
		IESV_MAGIC_ATTACK	= 120,		// 아이템 1레벨 마공 증가치	(이만큼이 증가해야 1레벨이 오름)
		IESV_PHYSIC_DEFENCE	= 30,		// 아이템 1레벨 물방 증가치	(이만큼이 증가해야 1레벨이 오름)
		IESV_MAGIC_DEFENCE	= 30,		// 아이템 1레벨 마방 증가치	(이만큼이 증가해야 1레벨이 오름)
		IESV_BASE_HP		= 3000,		// 아이템 1레벨 HP 증가치	(이만큼이 증가해야 1레벨이 오름)
	};
	//}}

	typedef std::pair< int, int > ItemExchangeKey; // key[ HOUSE_ID, SrcItemID ]

	struct ItemExchangeData
	{
		ItemExchangeData()
		{
			m_eExchangeType = IET_INVALID;
		}

		bool AddExchangeData( IN const ITEM_EXCHANGE_TYPE eExchangeType,
			IN const int iDestItemID,
			IN const float fDestItemRate,
			IN const int iDestQuantity
			, IN const short sPeriod
			);

		ITEM_EXCHANGE_TYPE					m_eExchangeType;
		std::map< int, int >				m_mapDestItemID;
		KLottery							m_kLottery;
		std::map< int, short >				m_mapDestItemPeriod;
	};

	//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
	struct ItemIDList
	{
		std::vector< int >	m_ItemIDList;
	};

	struct NpcItemIDList
	{
		int					m_NpcId;
		std::vector< int >	m_ItemIDList;
	};
	//}}

	//{{ 2013. 05. 21	최육사	아이템 개편
	struct SItemConvertInfo
	{
		int						m_iConverType;
		KLotterySameProb< int >	m_kLotResultItemID;
		int						m_iCommission;

		SItemConvertInfo()
		{
			m_iConverType = 0;
			m_iCommission = 0;
		}
	};
	//}}
	struct tgaDisCountItem
	{
		int					m_DisCountProNo;
		int					m_BasicProNo;
		std::vector<int>	m_DisCountKeepItem;
	};

public:
	CXSLItemManager(void);
	~CXSLItemManager(void);

	// for lua
	bool AddItemTemplet_LUA();
	bool AddItemExchangeData_LUA( int iHouseID, ITEM_EXCHANGE_TYPE eExchangeType, int iSrcItemID, int iDestItemID, int iQuantity, float fDestItemRate );
	//{{ 2010. 9. 1	최육사	기간 초기화 아이템
	bool AddItemExpandPeriodData_LUA( int iItemID, short sPeriod );
	//}}
	//{{ 2010. 11. 29	최육사	기간제템 삭제
	//}}
	//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
	bool AddShopItemList_LUA();
	bool AddShopItemGroup_LUA();
	//}}
	//{{ 2012. 04. 04	최육사	배틀필드 시스템
	bool AddCoolTimeGroupItem_LUA( int iCoolTimeGroupEnum, int iItemID );
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	void AddWeddingHallItem_LUA( int iWeddinHallType, int iItemID );
	int GetWeddingHallItem( const char cWeddingHall );

	void AddWeddingCompleteRewardItem_LUA( IN int iItemID, IN int iCount );
	void GetWeddingCompleteRewardItem( OUT std::map<int, int>& mapItemList );
	void AddCoupleAnniversaryDayRewardItem_LUA( int iItemID );
	void GetCoupleAnniversaryDayRewardItem( OUT std::vector<int>& vecItemList );
	void AddWeddingAnniversaryDayRewardItem_LUA( int iItemID );
	void GetWeddingAnniversaryDayRewardItem( OUT std::vector<int>& vecItemList );
	void AddWeddingAnniversaryDayRewardTitle_LUA( int iDays, int iItemID );
	int GetWeddingAnniversaryDayRewardTitle( const int iDays );
	//}
	bool AddKeepShowItem_LUA(void);
	bool IsKeepItemShowItem( const int iItemID );


	// 아이템 정보
	const CXSLItem::ItemTemplet* GetItemTemplet( const int itemID ) const;
	const std::map< int, CXSLItem::ItemTemplet >& GetItemTempletContainer() const { return m_ItemTempletIDMap; }

	const std::map< int, KItemName >& GetItemNameContainer() const { return m_ItemTempletNameMap; }

	// 아이템 교환
	//{{ 2013. 02. 19   교환 로그 추가 - 김민성
	bool GetItemExchangeResult( IN const int iHouseID,
		IN const int iSrcItemID,
		IN const int iSrcQuantity,
		IN const KItemInfo& kSrcItemInfo,
		IN const int iDestItemID,
		OUT std::map< int ,KItemInfo >& mapResultItem,
		OUT bool& bRandomExchange,
		OUT char& cExchangeType);
	//}

	// 봉인 가능 아이템
	bool IsCanSealForCashItem( int iItemID );
	bool IsCanSealForRandomItem( int iItemID );

	//{{ 2010. 03. 22  최육사	기술의 노트
	bool IsSkillNoteMemoItem( int iItemID );
	//}}

	//{{ 2010. 9. 1	최육사	기간 초기화 아이템
	bool IsEmptyExpandPeriodItemList() const			{ return m_mapExpandPeriodItem.empty(); }
	bool GetExpandPeriodByItemID( IN int iItemID, OUT short& sExpandPeriod ) const;
	//}}

	//{{ 2010. 11. 29	최육사	기간제템 삭제
	//}}

	//{{ 2011. 05. 23  김민성	큐피엘 젤리 친밀도 증감
	bool IsFantasticKewpielJelly( const int iItemID );
	bool IsOddKewpielJelly( const int iItemID );
	//}}

	//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
	bool IsNpcSellEDItem( IN const int iItemID ) const { return ( m_setNpcSellEDItemList.find( iItemID ) != m_setNpcSellEDItemList.end() ); }
	//}}

	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}

	//{{ 101020 김정협 트레져박스여부 확인
	//}} 101020 김정협 트레져박스여부 확인

	//{{ 2011. 07. 08    김민성    옵션 수치화
	void UpdateSetIDAndMaxLevelMap( const int iSetID, const int iLevel );
	int  GetSetItemMaxLevel( const int iSetID_ );
	//}}

	//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
	//{{ 2013. 05. 15	최육사	아이템 개편
	bool GetItemLevel( IN const char cUnitType, IN const char cUnitClass, IN const KItemInfo& kItemInfo, OUT int& iItemLevel );
	//}}
	//}}

	//{{ 2012. 04. 04	최육사	배틀필드 시스템
	bool IsCanUseQuickSlotItemInVillage( IN const int iItemID ) const;
	//}}

	//{{ 버프 관리 시스템 - 김민성
	bool IsBuffItem( IN int iItemID );
	//}}
	//{{ 2013. 05. 20	최육사	아이템 개편
	void ItemSealProcess( IN const int iItemID
		, IN const short sPeriod
		, OUT unsigned char& ucSealData
		) const;

	// 아이템 교환
	bool AddItemConvertGroup_LUA( int iGroupID, int iItemID );
	bool AddItemConvertInfo_LUA();
	bool GetResultItemConvert( IN const int iTargetItemID, OUT int& iResultItemID, OUT int& iCommissionED ) const;
	//}}

	int	GetItemCSPoint( IN const int iItemID );
	int	GetItemCSPointPeriod( IN const int iItemID );

	bool IsInventoryOnly( IN const int iItemID );

protected:
	//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
	void GetCaculatedItemLevelByEnchantStat( IN const KStat& kEnchantItemStat, OUT int& iItemLevel );
	//}}

private:
	std::map< int, CXSLItem::ItemTemplet >			m_ItemTempletIDMap;		// 아이템 템플릿

	std::map< int, KItemName >						m_ItemTempletNameMap;

	//{{ 2011. 08. 08	최육사	아이템 교환 시스템 개편
	typedef std::map< ItemExchangeKey, ItemExchangeData >	ItemExchangeTable;
	std::map< int, ItemExchangeTable >				m_mapItemExchangeTable;	// 교환 아이템
	//}}
	//{{ 2010. 9. 1	최육사	기간 초기화 아이템
	std::map< int, short >							m_mapExpandPeriodItem;	// 기간 초기화 아이템
	//}}
	//{{ 2010. 11. 29	최육사	기간제템 삭제
	//}}
	//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
	std::set< int >									m_setNpcSellEDItemList; // NPC가 판매하는 ED아이템 리스트
	//}}
	//{{ 2011. 07. 08    김민성    옵션 수치화
	std::map<int, int>								m_mapSetIDAndMaxLevel;	// 셋트의 최고 레벨
	//}}
	//{{ 2012. 04. 04	최육사	배틀필드 시스템
	std::map< int, CXSLItem::COOLTIME_ITEM_GROUP >	m_mapCoolTimeItemGroup;
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	std::map<char, int>								m_mapWeddingHallTypeItem;	// [타입,itemid]
	std::map< int, int >							m_mapWeddingCompleteRewardItem;
	std::vector< int >								m_vecCoupleAnniversaryDayRewardItem;
	std::vector< int >								m_vecWeddingAnniversaryDayRewardItem;
	std::map< int, int >							m_mapWeddingAnniversaryDayRewardTitle;	// [days, itemid]
	//}
	//{{ 2013. 05. 21	최육사	아이템 개편
	std::map< int, std::vector< int > >				m_mapItemConvertGroup;
	std::map< int, SItemConvertInfo >				m_mapItemConvertTable;
	//}}
	public:
		int							m_InfoType;
		int							m_ShowItem;
		std::vector< int >			m_vecKeepItem_Key;
		std::map< int,std::vector< int > >	m_mapKeepShowItem;
		std::map< int, kDisCountItemInfo > m_DisCountInfoMap;

};

DefRefreshSingletonInline( CXSLItemManager );
