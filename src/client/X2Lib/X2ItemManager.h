#pragma once
#include <set>
#include "X2Eqip.h"





class CX2ItemManager
{
	public:
		enum PREVIEW_PET_TYPE
		{
			PPT_PET,		// 일반 펫
			PPT_RIDING_PET, // 탈 것
		};

		struct PREVIEW_PET_DATA
		{
			int iCashShopItemID;
			int iPreviewUnitID;
			PREVIEW_PET_TYPE ePreviewPetType;

			PREVIEW_PET_DATA( int iCashShopItemID_, int iUnitID_, PREVIEW_PET_TYPE ePreviewPetType_ ):
			iCashShopItemID( iCashShopItemID_ ),
			iPreviewUnitID( iUnitID_ ),
			ePreviewPetType( ePreviewPetType_ )
			{}

			PREVIEW_PET_DATA()
			{}
		};

#include    "X2ItemManager_Preprocessing.inl"


		struct ItemIDList
		{
			vector<int>		m_ItemIDList;
		};

		struct NpcItemIDList
		{
			int				m_NpcId;
			vector<int>		m_ItemIDList;
		};

		struct RandomItemData
		{
			int				m_ItemID;
			int				m_KeyItemID;
			int				m_RequiredKeyCount;
			int				m_iRequiredED;
		};

		struct MaterialData
		{
			int			m_MaterialItemID;
			int			m_MaterialCount;
		};

		struct ManufactureData
		{
			int			m_iServerGroupID;
			int			m_ManufactureType;	// CX2ManufactureUI::MANUFACTURE_TYPE
			int			m_ResultGroupID;
			int			m_Cost;

			std::vector< MaterialData >	m_vecMaterials;
		};

		struct ManufactureResultGroupData
		{
			int					m_ResultGroupID;
			std::vector< int >	m_vecResultGroupItemID;
		};

		struct CashItem
		{
			int					m_ItemID;

			bool				m_bShow;

			bool				m_bIsNew;
			bool				m_bIsHot;
			bool				m_bIsRecommend;
			bool				m_bIsLimited;

			vector< KBillProductInfo > m_vecKBillProductInfo;


			CashItem()
			{
				m_ItemID = -1;

				m_bShow = true;

				m_bIsNew = false;
				m_bIsHot = false;
				m_bIsRecommend = false;
				m_bIsLimited = false;
			}

			//대표 상품 캐시 수치
			int GetCash();
			const KBillProductInfo* GetGateProduct();
			wstring GetPeriod( KBillProductInfo& kBillProductInfo );

			bool GetIsNew() { return m_bIsNew; }
			bool GetIsHot() { return m_bIsHot; }
			bool GetIsRecommend();
		};

		class CCashProductSort
		{
		public:
		public:
			bool operator()( const KBillProductInfo& p, const KBillProductInfo& q ) const
			{
				if( p.m_iSalePrice < q.m_iSalePrice )
					return true;
				else
					return false;
			}
		};

		//{{ 2008. 4. 24  최육사  패키지 아이템
		struct PackageItemData
		{
			int				m_iItemID;
			unsigned short  m_usProductPieces;
			bool			m_bShowItem;
		};
		//}}
		//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
		struct CubePackageData
		{
			int				m_iPackageID1;
			int				m_iPackageID2;
		};
		//}}


		struct IceHeaterItemData
		{
			int					m_iItemID;
			CX2Unit::UNIT_TYPE	m_eUnitType;
		};


		enum ITEM_EXCHANGE_TYPE
		{
			IET_INVALID = 0,
			IET_ALL,
			IET_RANDOM,
			IET_SELECT,
		};

		//
		struct ItemExchangeData
		{
			ITEM_EXCHANGE_TYPE						m_eExchangeType;
			int										m_iSrcItemID;
			int										m_iDestItemID;
			int										m_iSrcQuantity;
			int										m_iQuantity;
			short									m_sPeriod;
		};


		struct tgaDisCountItem
		{
			int					m_DisCountProNo;
			int					m_BasicProNo;
			std::vector<int>	m_DisCountKeepItem;
		};


	public:
		CX2ItemManager(void);
		~CX2ItemManager(void);

		bool	OpenScriptFile( const WCHAR* pFileName );
		bool	OpenItemScriptFile( const WCHAR* pKimFile, unsigned uNumScriptFiles, const WCHAR* apScriptFileName[] );
        bool    DoMemoryNotEncrypt( const char* pBuffer, long nSize );


		bool	AddShopItemList_LUA();
		bool	AddShopItemGroup_LUA();

		bool	AddRandomItemTemplet_LUA( int iItemID_, int iKeyItemID_, int iRequiredKeyCount_, int iRequiredED_ );

		bool	AddManufactureTemplet_LUA();

		//{{ kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록
			bool	AddManufactureVillageData_LUA( int iManufactureID, int iHouseID );
		//}} kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록

		bool	AddManufactureResultGroup( int iGroupID, int iItemID );

		//{{ 2008. 11. 18  최육사
		void	AddItemExchangeGroup_LUA( int iSourceItemID, int iResultItemID, float fRate );
		bool	IsCanItemExchange( int iItemID );
		//}}


		bool 	AddItemExchangeData( int iHOUSEID, ITEM_EXCHANGE_TYPE eExchangeType, int iSrcItemID, int iDestItemID, int iQuantity, float fDestItemRate = 0.f );
		bool	GetItemExchangeData( int iHouseID, std::vector< ItemExchangeData >& vecItemExchangeData );	// kimhc // 2009-07-27 // 헤니르 시공 교환 UI



		//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
		bool	AddCube_PackageData_LUA( int iCubePackageItemID, int iPackageItemID1, int iPackageItemID2, int iUnitType);
		bool	GetPackageCubeItemData( int iPackageItemID, std::vector< PackageItemData >& vecPackageData);
		//}}

		//{{ 2008. 4. 24  최육사  패키지 아이템
		bool	AddPackageItemData_LUA( int iPackageItemID, int iItemID, unsigned short usProductPieces, bool bShowItem );
		bool	GetPackageData( int iPackageItemID, std::vector< PackageItemData >& vecPackageData );
		bool	IsShowPackageItem( int iItemID );
		//}}
		bool	IsShowItemTooniland( int iItemId );

		bool	AddCashItem( vector< KBillProductInfo >& vecKBillProductInfo );
		bool	AddCashItemEx();
		bool	GetIsUpdateCashItem();
		void	ClearCashItemList();

		void	GetShopItemList( int mapID, vector<const CX2Item::ItemTemplet*>& itemTempletList );
		bool    GetNpcShopItemList( int mapID, int npcID, vector<const CX2Item::ItemTemplet*>& itemTempletList );

		//{{ kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가
		typedef	std::multimap<int,RandomItemData*>::iterator RandomItemItr;

		std::pair<RandomItemItr,RandomItemItr> GetRandomItemData( int itemID );
		bool IsRandomItem( std::pair<RandomItemItr,RandomItemItr>& pairValue );
		//}} kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가

		ManufactureData* GetManufactureData( int ManufactureID );

		//{{ kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록
		bool	GetManufactureVillageData( int iHouseID, std::vector< int >& vecManufactureVillage );
		//}} kimhc // 2009-11-23 // 제조 아이템 NPC 별로 가능하도록

		const ManufactureResultGroupData* GetManufactureResultGroupData( int iResultGroupID );

		CashItem*	GetCashItem( int itemID );
		wstring		GetCashItemPeriod( int iProductNo );
		KBillProductInfo* GetCashItemProduct( int iProductNo );
		vector<int> GetNewCashItemList();
		vector<int> GetHotCashItemList();
		vector<int> GetRecommendCashItemList();
		vector<int> GetLimitedCashItemList();
		bool		CheckLimitedCashItem( int itemID );
		bool		CheckEventCashItem( int itemID );
		bool		CheckHotIconCashItem( int itemID );
		vector<int> GetAllCashItemList();





//{{AFX
		void	UpdateEqip( CX2Unit* pUnit, CKTDGXSkinAnimPtr pAnim, CKTDXDeviceXSkinMesh* pMotion, CKTDXDeviceXET* pAnimXET, IN OUT vector<CX2EqipPtr>& viewEqipList, bool bLoad = true );
		void	ReplaceEmptyToBasic( CX2Unit* pUnit, CKTDGXSkinAnimPtr pAnim, CKTDXDeviceXSkinMesh* pMotion, CKTDXDeviceXET* pAnimXET, IN OUT vector<CX2EqipPtr>& viewEqipList, bool bLoad = true );
		void	ReplaceEmptyToBasic( CX2Unit* pUnit, CKTDGXSkinAnimPtr pAnim, CKTDXDeviceXSkinMesh* pMotion, CKTDXDeviceXET* pAnimXET, IN OUT vector<CX2EqipPtr>& viewEqipList, bool* pViewEqipPosition, CX2Unit::EQIP_POSITION eqipPos, int itemID, bool bLoad = true );
//}}AFX

		void	CheckEmptyEqipPosition( bool* pViewEqipPosition, IN OUT vector<CX2EqipPtr>& viewEqipList );



		void	GetItemList( vector<int>& vecItemID, int page, int quantityPerPage );


		static bool IsEDItem( int itemID )
		{
			switch( itemID )
			{
			case BRONZE_ED_ITEM_ID:
			case SILVER_ED_ITEM_ID:
			case GOLD_ED_ITEM_ID:
			case POSTBOX_ED_ITEM_ID:
			case GOLD_BAR_ED_ITEM_ID:
				{
					return true;
				}
			}

			return false;
		}

		void ClearItemExchangeData();
		void ClearManufactureData();
		void ClearManufactureVillageData();
		void ClearManufactureResultGroupData();

//{{ kimhc // 2010.12.9 // 2010-12-23 New Character CHUNG
		static CX2EqipPtr CreateDummyEquip( int iItemID_, CX2Unit* pUnit_, CKTDGXSkinAnimPtr pXSkinAnim_ );
//}} kimhc // 2010.12.9 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업


		void UpdateMaxLevelToSetItemData();
		//}} kimhc // 2011-07-05 // 옵션데이타 수치화 작업

		void	AddCoolTimeGroupItem_LUA( int iCoolTimeGroupID, int iSourceItemID );
		int		GetCoolTimeGroupID( int iGroupID ) const;


		//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
		bool SetItemLevelEnchantData_LUA( int iAtkPhysic, int iAtkMagic, int iDefPhysic, int iDefMagic, int iBaseHP );
		bool GetItemLevel( IN const KItemInfo& kItemInfo, OUT int& iItemLevel );
		void GetCaculatedItemLevelByEnchantStat( IN const KStat& kEnchantItemStat, OUT int& iItemLevel );
		//}}

		bool	AddIceHeaterItemData_LUA( int iIceHeaterItemID_, int iItemID_, int eUnitType_ );

		bool	GetIceHeaterItemData( int iIceHeaterItemID_, CX2Unit::UNIT_TYPE eUnitType_, OUT std::vector< int >& vecIceHeaterItemData_ );

		bool	AddPetPreViewData_LUA( int iCashShopItemID_, int iPetUnitID_, PREVIEW_PET_TYPE ePetType_ = PPT_PET );
		int		GetPreviewPetIdByItemID( int iItemID_ );
		int		GetPreviewRidingPetIdByItemID( int iItemID_ );
		int		GetPreviewIDByItemID( int iItemID_, PREVIEW_PET_TYPE ePetType_);

		void	AddItemConvertInfo_LUA();
		void	AddItemConvertGroup_LUA(){}
		int		GetConvertResultItemID( int iTargetItemID_ );


		inline void	 SetKeepShowItemList( std::map<int,std::vector<int>> TempKeepShowList ){ m_mapKeepShowItemList = TempKeepShowList;}
		bool	CheckKeepShowItem( int TempItemID , std::vector<int>& GetItemID );
		inline void	 SetDisCountList( std::map< int,kDisCountItemInfo > TempDisCountList ){ m_DisCountInfoMap = TempDisCountList;}
		bool	CheckDisCountItemList( int TempProDuct,bool& bIdexReset );
		bool    DisCountItemID_Find(int TempID, std::vector<int>& nTempList,int& nTempSale);
		inline std::map< int , kDisCountItemInfo > GetDisCountListProDcut(void){ return m_DisCountInfoMap; }
		int		GetChangeCashPoint(int TempItemID);

	protected:
		//{{ 080721.hoons.
		//}}

    private:


        std::vector<BYTE>                       m_vecItemTempletInfo;



		map<int, ItemIDList*>					m_ShopItemList;
		map<int, ItemIDList*>					m_ShopItemGroup;
		map< std::pair<int,int>, NpcItemIDList*>	m_ShopNpcItemList;	// Key :: arg 1 : village id, arg 2 : npc id


		map<int, ManufactureData*>				m_mapManufacture;
		map< int, std::vector<int> >			m_mapManufactureVillage;
		map<int, ManufactureResultGroupData>	m_mapManufactureResultGroupData;
		//{{ kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가
		std::multimap<int, RandomItemData*>		m_multimapRandomItem;
		//}} kimhc // 2009-12-22 // 랜덤 아이템에 같은 키값 쓸수있도록 추가


		map<int, CashItem* >					m_mapCashItem;
		vector< int >							m_vecCashItem;
		vector< int >							m_vecNewCashItem;
		vector< int >							m_vecHotCashItem;
		vector< int >							m_vecRecommendCashItem;
		vector< int >							m_vecLimitedCashItem;
		vector< int >							m_vecEventCashItem;
		vector< int >							m_vecHotIconCashItem;

//{{ 2013.01.26 최민철 큐브패키지아이템 미리보기기능
		std::map< int, std::map< int, CubePackageData > > m_mapCube_PackageData;
//}}
		//{{ 2008. 4. 24  최육사  패키지 아이템
		std::map< int, std::vector< PackageItemData > > m_mapPackageData;
		std::set< int >							m_setShowPackageItem;
		//}}


		//{{ 2008. 11. 18  최육사	아이템교환
		map< int, int >							m_mapItemExchange;
		//}}


		std::map< int, std::vector< ItemExchangeData > > m_mapItemExchangeData;


		//int										m_nString_Index;


		//{{ kimhc // 2011-07-05 // 옵션데이타 수치화 작업


		map < int, int >						m_mapCoolTimeGroupID;
		map < int, vector<int> >				m_mapCoolTimeGroupItemIDList;

		//{{ 2011. 10. 17	최육사	배틀필드 아이템 레벨 시스템
		KStat									m_kItemLevelEnchantData;
		//}}

		std::map< int, std::vector< IceHeaterItemData > > m_mapIceHeaterItemData;


		std::vector< PREVIEW_PET_DATA > m_vecPetPreViewData; // 아이템ID, (펫ID, 펫타입)

		int 									m_iLastPackacgeID;
		//}} 2013.01.26 최민철 큐브패키지아이템 미리보기기능


		typedef std::map<int, int> ItemConvertList; // <변경 재료 아이템ID, 변경 결과 아이템 ID>

		// NPC ID, <변경 재료 아이템ID, 변경 결과 아이템 ID>
		std::map< int, ItemConvertList> m_mapNpcItemConvertList;

		std::map<int,std::vector<int>>						m_mapKeepShowItemList;
		std::map< int, kDisCountItemInfo >					m_DisCountInfoMap;
};
