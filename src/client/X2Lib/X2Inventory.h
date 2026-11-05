#pragma once

#define MAX_INVENTORY_SIZE 24
const static int s_iMaxInventorySize = 64;

class CX2Unit;
class CX2Inventory
{
	public:
		enum SORT_TYPE
		{
			ST_NONE = 0,
			ST_EQUIP,		// [1]  장비
			ST_ACCESSORY,	// [2]  악세사리
			ST_MATERIAL,	// [3]  재료
			ST_SPECIAL,		// [4]  특수
			ST_QUEST,		// [5]  퀘스트
			ST_QUICK_SLOT,	// [6]  소비
			ST_AVARTA,		// [7]  아바타
			//{{ 2009. 8. 3  최육사		은행카테고리
			ST_BANK,		// [8]  은행
			//}}
			ST_E_EQUIP,		// [9]  장착중인 장비
			ST_E_SKILL,		// [10] 장착중인 스킬(안씀)
			ST_E_QUICK_SLOT,// [11] 장착중인 소비템
			ST_PC_BANG,		// [12] PC방 전용 인벤
			//{{ 2010. 8. 2	최육사	펫 시스템
			ST_PET,			// [13] 펫 인벤토리
			//}}
			//#ifdef	SERV_SHARING_BANK_TEST
			ST_SHARE_BANK,	// [14] 공유 은행
			//#endif	SERV_SHARING_BANK_TEST
			//{{ 2013. 04. 10	최육사	어둠의 문 개편
			//#ifdef SERV_NEW_DEFENCE_DUNGEON
			ST_E_DEFENCE_QUICK_SLOT, // [14] 어둠의문 전용 퀵슬롯
			//#endif SERV_NEW_DEFENCE_DUNGEON
			//}}

			ST_END,
		};

		enum	MEMBERSHIP_PRIVATE_BANK
		{
			MPB_NORMAL		=	8,
			MPB_SILVER		=	16,
			MPB_GOLD		=	24,
			MPB_EMERALD		=	32,
			MPB_DIAMOND		=	40,
			MPB_PLATINUM	=	48,
		};

		enum SEAL_COUNT
		{
			SEAL_COUNT_MAX = 255,
		};

private:
		struct InvenSlotData
		{
			CX2Inventory::SORT_TYPE	m_SortType;
			int						m_SlotID;

			CX2Item*	m_pItem;

			InvenSlotData()
			{
				m_SortType	= CX2Inventory::ST_NONE;
				m_SlotID	= -1;
				m_pItem		= NULL;
			}
			~InvenSlotData();
		};

	public:
//{{ robobeg : 2013-11-01
		//CX2Inventory( CX2Unit* pOwnerUnit );
        CX2Inventory();
        void        SetOwnerUnit( CX2Unit* pOwnerUnit ) { m_pOwnerUnit = pOwnerUnit; }
//}} robobeg : 2013-11-01
		~CX2Inventory(void);

		void		Clear();

		void		Reset( std::map< int, int >& mapInventorySlotSize, std::map< UidType, KInventoryItemInfo >& mapItem );

		//{{ kimhc // 2010-01-04 // 기존의 EGS_SELECT_UNIT_ACK에 인벤정보가 포함되어 오던 것 분할
		void		ResetSize( const std::map<int, int>& mapInventorySize );
		void		ResetItems( const std::map< UidType, KInventoryItemInfo >& mapItem );
		void		RemovePCBangEquips();		// 현재 장착중인 아이템들 중에 PC방 템 삭제!!
		//}} kimhc // 2010-01-04 // 기존의 EGS_SELECT_UNIT_ACK에 인벤정보가 포함되어 오던 것 분할

		//{{ kimhc // 2009-08-07 // 캐릭터별 은행
		void		SetBank( std::map< UidType, KInventoryItemInfo >& mapItem );
		//}} kimhc // 2009-08-07 // 캐릭터별 은행

		void		RemovePetInventory();
		void		SetPetInventory( const std::vector<KInventoryItemInfo>& vecInventorySlotInfo, int iItemMaxNum );

		void		UpdateInventorySlotList( const std::vector<KInventoryItemInfo>& vecInventorySlotInfo );

		//{{ 2012. 02. 21	김민성	인벤토리 정렬
		bool		UpdateCategorySlotList( SORT_TYPE sortType, std::vector<UidType>& vecUpdatedInventorySlot );
		//}}

		bool		AddItem( SORT_TYPE sortType, int slotID, CX2Item* pItem );
		bool		AddItem( SORT_TYPE sortType, int slotID, CX2ItemData_Base& kItemData );

		int			GetNumItemByTID( int itemTID, bool bExcludeEquipped = false, bool bIncludeBankType = false ) const;				// TID에 해당하는 아이템의 개수를 return

		//{{ mauntain : 김태환 [2012.07.16] 인벤토리 및 슬롯을 고려한 아이템 구입 갯수 연산
		//}}

		CX2Item*	GetItemByTID( int itemTID, bool bIncludeBankType = false, bool bIncludeEEquipType =  true ) const;
		CX2Item*	GetItem( UidType itemUID, bool bIncludeBankType	= false ) const;
		CX2Item*	GetItem( SORT_TYPE sortType, int slotID ) const;

		SORT_TYPE	GetItemSortType( UidType itemUID ) const;

		void		SetItemMaxNum( SORT_TYPE sortType, int maxNum );
		// GetItemMaxNum에 인자로 ST_BANK를 전달해주면 슬롯의 사이즈를 알수 있고, 그 값으로 은행 회원 등급을 구분!!
		int			GetItemMaxNum( SORT_TYPE sortType ) const { return ( sortType >= SORT_TYPE(0) && sortType < ST_END ) ? m_MaxSize[sortType] : 0; }
		int			GetUsedSlotNum( SORT_TYPE sortType ) const;

		bool		RemoveItem( UidType itemUID );	//	은행은 사용할 수 없음
		bool		RemoveItem( SORT_TYPE sortType, int slotID );

		CX2Unit*	GetOwnerUnit() const { return m_pOwnerUnit; }
        SORT_TYPE	GetSortTypeByItemTemplet( const CX2ItemTemplet_Base* pItemTemplet ) const ;
		SORT_TYPE	GetSortTypeByID( int itemID ) const;

		bool		IsPossibleAddItem( SORT_TYPE sortType ) const;

		wstring		GetInvenSortTypeName( SORT_TYPE sortType ) const;
		CX2Item*	GetEquippingItemByEquipPos( CX2Unit_PreHeader::EQIP_POSITION equipPos, bool bFashion ) const;

		void		GetItemIDAndNum( OUT set<int>& setItemPackage ) const;	//가지고 있는 itemID, 수량

		int			GetEqiuppingSetItemNum( int setID ) const;
		bool		CheckEquippingItem( int itemID, UidType itemUID = -1 ) const;

		CX2Stat::Stat GetSetItemOptionStat() const;

		//{{ kimhc // 2009-10-19 // 최대 MP 증가 값 얻어오기
		float		GetAddMaxMPValue() const;
		//}} kimhc // 2009-10-19 // 최대 MP 증가 값 얻어오기

		//{{ kimhc // 2009-08-07 // 캐릭터별 은행
		int			GetCoutForGetMyBankInfoReq() const { return m_iCoutForGetMyBankInfoReq; }
		void		SetCountForGetMyBankInfoReq( int value ) { m_iCoutForGetMyBankInfoReq = value; }

		CX2Inventory::MEMBERSHIP_PRIVATE_BANK GetBankMembershipGrade() const { return m_eBankMembershipGrade; }

		//}} kimhc // 2009-08-07 // 캐릭터별 은행

		void SetShareBank( std::map< UidType, KInventoryItemInfo >& mapItem );
		int	GetUsedShareBankNum() const { return m_ItemMap[ST_SHARE_BANK].size(); }

		void ClearShareBank();
		bool UpdateItemUID( UidType iBeforeUID, UidType iNewUID );

		bool		IsNewItem( SORT_TYPE sortType, UidType iItemUID) const;
		void		ResetNewItem();
		bool		EraseNewItem( UidType iItemUID );

		UidType GetItemUIDBySortTypeAndItemID( SORT_TYPE sortType_, int ItemID_ ) const;

		bool		GetRelationItemInfoFromServer () const;
		bool	IsAbleToExpandSlot() const;

	private:
		const InvenSlotData* GetInvenSlot( SORT_TYPE sortType, int slotID ) const;

private:
		CX2Unit*						m_pOwnerUnit;
		int								m_MaxSize[ST_END];
		map<UidType,InvenSlotData*>		m_ItemMapTotal;
		map<int, InvenSlotData*>		m_ItemMap[ST_END]; //슬롯 아이디, 아이템

		//{{ kimhc // 2009-08-07 // 캐릭터별 은행
		int								m_iCoutForGetMyBankInfoReq;		// 은행정보를 받아온 횟수
		CX2Inventory::MEMBERSHIP_PRIVATE_BANK	m_eBankMembershipGrade;
		//}} kimhc // 2009-08-07 // 캐릭터별 은행

		//인벤토리의 아이템은 수량만 변경할 때도, 삭제 후 추가하기 때문에, 추가 만으로는 '새롭게 얻은 아이템'에 대한 판별이 안됨.
		//추가한 아이템 중, 삭제한 아이템 목록에 없는 아이템이라면 '새롭게 얻은 아이템'이라고 판단.
		set<UidType>					m_SetRemoveItem;
		set<UidType>					m_SetNewItem;
};
