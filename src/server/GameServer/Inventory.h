#pragma once

#include <map>
#include <vector>
#include "KncUidType.h"
#include "InventoryItem.h"
#include "X2Data/XSLInventory.h"
#include "X2Data/XSLRoom.h"
#include "TimerManager.h"
//{{ 2012. 03. 29	최육사	Inventory Lock 기능
	#include "InventoryLocker.h"
//}}
//{{ 2012. 10. 04	최육사		ED&아이템 모니터링 시스템
//}}

//{{ 2012. 10. 09	김민성	배틀필드 아이템 레벨 시스템
#include "X2Data/XSLItem.h"
//}}

SmartPointer(KGSUser);

class KGSUser;
class KInventory
{
public:
    KInventory();
    virtual ~KInventory();

    void Clear();
    void SetOwner( IN KGSUser* pUser );
	//{{ 2010. 8. 3	최육사	펫 시스템
//#ifdef SERV_PET_SYSTEM
	bool Init( IN const std::map< int, int >& mapSlotSize,
			   IN const std::map< UidType, KInventoryItemInfo >& mapInventoryItem,
			   OUT std::map< UidType, KItemPosition >& mapWrongPosItem,
			   IN const bool bIsChannelChange );
//#endif SERV_PET_SYSTEM
	//}}
	//{{ 2010. 01. 04  최육사	인벤토리 기능개선
	void GetSelectUnitInventoryInfo( OUT std::vector< KEGS_SELECT_UNIT_INVENTORY_INFO_NOT >& vecInventoryInfo );
	void GetInventorySlotSize( OUT std::map< int, int >& mapInventorySlotSize );
	//}}
	//{{ 2009. 8. 7  최육사		은행 정보 얻기
	void GetInventoryInfo( IN const int iCategory, OUT std::map< UidType, KInventoryItemInfo >& mapInventoryItem, IN const bool bForce = false );
	void GetInventoryInfo( IN const int iCategory, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo, IN const bool bForce = false );
	//}}

    int  GetFirstCategory() const			{ return CXSLInventory::ST_EQUIP; }
    int  GetSentinelCategory() const		{ return CXSLInventory::ST_END; }

	void GetItemIDSet( OUT std::set< int >& setItemID, bool bOnlyInventory = false );

    int  GetBaseSlotSize( IN const int iCategory ) const;
    int  GetMaxSlotSize( IN const int iCategory ) const;
    int  GetSlotSize( IN const int iCategory ) const;

	char GetItemState( IN const UidType iItemUID ) const;

	//////////////////////////////////////////////////////////////////////////
	// 아이템 검증
    bool IsExist( IN const int iItemID, IN  const bool bOnlyInventory ) const;
	bool IsExistOnlyInventory( IN  const int iItemID ) const;
	bool IsExistWithBank( IN const int iItemID ) const;
    bool IsExist( IN const UidType iItemUID ) const;
	bool IsExistInBank( IN const UidType iItemUID ) const;
	bool IsEquippedItem( IN const int iItemID ) const;

    bool IsEmptySlot( IN const int iCategory, IN const int iSlotID ) const;
    bool IsEnoughItemExist( IN const std::map< int, int >& mapRequiredItem, IN const bool bOnlyInventory ) const;
	bool IsEnoughItemExist( IN const std::map< int, int >& mapRequiredItem
		, IN const bool bOnlyInventory
		, IN const bool bOnlyInfinite	// 기한 제한이 없는 것
		) const;
	bool IsEnoughItemExistOnlyInventory( IN const std::map< int, int >& mapRequiredItem );
    bool IsEnoughItemExist( IN const std::map< UidType, int >& mapRequiredItem );
    bool IsSuitableSlot( IN int iItemID, IN int iCategory, IN int iSlotID );
    bool IsAbleToEquip( IN int iItemID );
    bool IsEnoughSpaceExist( IN const std::map< int, int >& mapItem ); // 빈공간 검사는 은행, 장착슬롯, 퀵슬롯 제외임
	//{{ 2010. 05. 03  최육사	소켓 개편
	bool IsEnoughEmptySlot( IN int iCategory, IN int iEmptySlotNum );
	//}}
	bool IsPeriodExpired( IN UidType iItemUID );
	bool IsPeriodExpired( IN int iCategory, IN int iSlotID );
	//{{ 2008. 12. 21  최육사	깨진 아이템
	bool IsBrokenItem( IN UidType iItemUID );
	//}}
	//{{ 2009. 8. 7  최육사		은행
	CXSLInventory::MEMBERSHIP_PRIVATE_BANK GetBankMemberShip();
	int  GetMyNextUpgradeBankItemID();
	//}}
	//{{ 2009. 8. 27  최육사	봉인
	bool IsSealedItem( IN UidType iItemUID );
	//}}
	//{{ 2010. 01. 04  최육사	PC방 프리미엄
	bool IsDBUpdateItem( IN UidType iItemUID ) const;
	//}}

	//////////////////////////////////////////////////////////////////////////
	// 아이템 정보
    bool GetCategorySlot( IN const UidType iItemUID, OUT int& iCategory, OUT int& iSlotID ) const;
    UidType GetItemUID( IN const int iCategory, IN const int iSlotID ) const;

    int  GetItemID( IN const UidType iItemUID ) const;
    int  GetItemID( IN const int iCategory, IN const int iSlotID ) const;

    bool GetUsageType( IN const UidType iItemUID, OUT int& iUsageType ) const;
    bool GetUsageType( IN const int iCategory, IN const int iSlotID, OUT int& iUsageType ) const;

	bool GetQuantity( IN const int iItemID, OUT int& iQuantity ) const;
    bool GetQuantity( IN const UidType iItemUID, OUT int& iQuantity ) const;
    bool GetQuantity( IN const int iCategory, IN const int iSlotID, OUT int& iQuantity ) const;

	bool GetEndurance( IN const UidType iItemUID, OUT int& iEndurance ) const;
	bool GetEndurance( IN const int iCategory, IN const int iSlotID, OUT int& iEndurance )  const;

	bool GetEnchantLevel( IN UidType iItemUID, OUT int& iEnchantLevel );
	bool GetEnchantLevel( IN int iCategory, IN int iSlotID, OUT int& iEnchantLevel );

	//{{ 2009. 11. 9  최육사	트리플속성
	bool GetAttribEnchant( IN UidType iItemUID, OUT KItemAttributeEnchantInfo& kInfo );
	bool GetAttribEnchant( IN int iCategory, IN int iSlotID, OUT KItemAttributeEnchantInfo& kInfo );
	//}}

	//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
//#ifdef SERV_ITEM_OPTION_DATA_SIZE
	bool GetItemSocketList( IN UidType iItemUID, OUT std::vector< int >& vecItemSocket );
	bool GetItemSocketList( IN int iCategory, IN int iSlotID, OUT std::vector< int >& vecItemSocket );
//#else
//	bool GetItemSocketList( IN UidType iItemUID, OUT std::vector< short >& vecItemSocket );
//	bool GetItemSocketList( IN int iCategory, IN int iSlotID, OUT std::vector< short >& vecItemSocket );
//#endif SERV_ITEM_OPTION_DATA_SIZE
	//}}

	//{{ 2010. 04. 15  최육사	소켓 개편
//#ifdef SERV_SOCKET_NEW
	//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
//#ifdef SERV_ITEM_OPTION_DATA_SIZE
	bool GetItemSocketList( IN UidType iItemUID, OUT std::map< int, int >& mapItemSocket );
//#else
//	bool GetItemSocketList( IN UidType iItemUID, OUT std::map< int, short >& mapItemSocket );
//#endif SERV_ITEM_OPTION_DATA_SIZE
	//}}
//#endif SERV_SOCKET_NEW
	//}}
	//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
//#ifdef SERV_ITEM_OPTION_DATA_SIZE
	bool GetItemSocketOption( IN UidType iItemUID, IN const int iSocketIndex, OUT int& iSocketOption );
//#else
//	bool GetItemSocketOption( IN UidType iItemUID, IN const int iSocketIndex, OUT short& sSocketOption );
//#endif SERV_ITEM_OPTION_DATA_SIZE
	//}}
	bool GetCountAssignedItemSocket( IN UidType iItemUID, OUT int& iAssignedSocketCount );

	//{{ 2013. 05. 15	최육사	아이템 개편
	bool GetRandomSocketList( IN const UidType iItemUID, OUT std::vector< int >& vecRandomSocket );
	bool GetRandomSocketList( IN const int iCategory, IN const int iSlotID, OUT std::vector< int >& vecRandomSocket );
	bool GetRandomSocketList( IN const UidType iItemUID, OUT std::map< int, int >& mapRandomSocket );
	bool GetRandomSocketOption( IN const UidType iItemUID, IN const int iSocketIndex, OUT int& iSocketOptionID );
	bool GetCountAssignedRandomSocket( IN const UidType iItemUID, OUT int& iAssignedSocketCount );

	bool SetRandomSocketOption( IN const UidType iItemUID, IN const std::vector< int >& vecRandomSocket );
	//}}

    bool GetItemInfo( IN UidType iItemUID, OUT KItemInfo& kInfo ) const;
    bool GetItemInfo( IN int iCategory, IN int iSlotID, OUT KItemInfo& kInfo ) const;

    bool GetInventoryItemInfo( IN const UidType iItemUID, OUT KInventoryItemInfo& kInventoryItemInfo ) const;
    bool GetInventoryItemInfo( IN const int iCategory, IN const int iSlotID, OUT KInventoryItemInfo& kInventoryItemInfo ) const;
    bool GetInventorySlotInfo( IN const int iCategory, IN const int iSlotID, OUT KInventoryItemInfo& kInventoryItemInfo ) const;
	bool GetInventoryItemSimpleInfo( IN const UidType iItemUID, OUT KInventoryItemSimpleInfo& kInventoryItemInfo ) const;
	bool GetInventoryItemSimpleInfo( IN const int iCategory, IN const int iSlotID, OUT KInventoryItemSimpleInfo& kInventoryItemInfo ) const;

	void GetSpecialItem( OUT std::map< int, KInventoryItemInfo >& mapSpecialItem ) const;

    void GetEquippedItem( OUT std::map< int, KInventoryItemInfo >& mapEquippedItem ) const;
	void GetEquippedItem( OUT std::vector< KInventoryItemSimpleInfo >& vecEquippedItem ) const;

	//{{ 2012. 10. 09	김민성	배틀필드 아이템 레벨 시스템
	void GetEquippedItem( IN std::set< CXSLItem::ITEM_TYPE >& setItemType, IN bool bFashion, OUT std::map< int, KInventoryItemInfo >& mapEquippedItem ) const;
	//}}

	//{{ 2010. 10. 18	최육사	펫 오라 스킬
//#ifdef SERV_PET_AURA_SKILL
	//{{ 2011. 07. 08    김민성    옵션 수치화
//#ifdef SERV_USE_PERCENT_IN_OPTION_DATA
	void GetEquippedStat( IN const bool bIsDungeon, IN const KStat& kBaseStat, IN OUT float& fTotalIncHPRate, OUT KStat& kAddStat, IN unsigned int& uiHP_OnePoint, IN unsigned int& uiUnitLevel );
//#endif SERV_USE_PERCENT_IN_OPTION_DATA
	//}}
//#endif SERV_PET_AURA_SKILL
	//}}
	//{{ 2009. 7. 28  최육사	GM무기만
	void GetEquippedStatOnlyGMWeapon( OUT KStat& kStat );
	//}}
	//{{ 2009. 2. 2  최육사		소켓옵션
	void GetEquippedItemBonusRate( IN const bool bIsDungeonRoom, OUT float& fAddTitleExpRate, OUT float& fAddTitleEDRate );
	//}}
	//{{ 2011. 10. 18	최육사	배틀필드 아이템 레벨 시스템
	bool GetItemLevel( IN const UidType iItemUID, OUT int& iItemLevel );
	//}}

	//////////////////////////////////////////////////////////////////////////
	// 아이템 수량 및 상태 변화
    void IncreaseQuantity( IN UidType iItemUID, IN int iQuantity, OUT int& iIncreased );
    void IncreaseQuantity( IN int iCategory, IN int iSlotID, IN int iQuantity, OUT int& iIncreased );
	//{{ 2009. 12. 15  최육사	아이템 삭제사유
    void DecreaseQuantity( IN UidType iItemUID, IN int iQuantity, OUT int& iDecreased, IN KDeletedItemInfo::DELETE_REASON eReason );
    void DecreaseQuantity( IN int iCategory, IN int iSlotID, IN int iQuantity, OUT int& iDecreased, IN KDeletedItemInfo::DELETE_REASON eReason );
	//}}

	//{{ 2012. 05. 08	김민성       개인 거래 로직 변경
	// DecreaseQuantity() 와는 기능은 동일하지만 리턴 파라미터에 삭제되는 아이템정보를 넘겨 준다.
	void DecreaseQuantityOrDelete( IN UidType iItemUID, IN int iQuantity, OUT int& iDecreased, IN KDeletedItemInfo::DELETE_REASON eReason, OUT KInventoryItemInfo& kDeleteItemInfo );
	// 추가된 아이템 중 수량성 아이템 및 슬롯 정보 정돈한다.
	void FixUpInventoryItem( IN OUT std::vector< KInventoryItemInfo >& vecInsertedItem, OUT KItemQuantityUpdate& kDBUpdate, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	//}}

    void IncreaseEndurance( IN UidType iItemUID, IN int iEndurance, OUT int& iIncreased );
    void IncreaseEndurance( IN int iCategory, IN int iSlotID, IN int iEndurance, OUT int& iIncreased );
    void DecreaseEndurance( IN UidType iItemUID, IN int iEndurance, OUT int &iDecreased );
    void DecreaseEndurance( IN int iCategory, IN int iSlotID, IN int iEndurance, OUT int& iDecreased );
	void IncreaseEnchantLevel( IN UidType iItemUID, IN int iEnchantLevel, OUT int& iIncreased );
	void IncreaseEnchantLevel( IN int iCategory, IN int iSlotID, IN int iEnchantLevel, OUT int& iIncreased );
	void DecreaseEnchantLevel( IN UidType iItemUID, IN int iEnchantLevel, OUT int &iDecreased );
	void DecreaseEnchantLevel( IN int iCategory, IN int iSlotID, IN int iEnchantLevel, OUT int& iDecreased );

	//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
//#ifdef SERV_ITEM_OPTION_DATA_SIZE
	bool SetItemSocketOption( IN UidType iItemUID, IN const int iSocketIndex, IN const int iSocketOption );
	bool SetItemSocketOption( IN int iCategory, IN int iSlotID, IN const int iSocketIndex, IN const int iSocketOption );
//#else
//	bool SetItemSocketOption( IN UidType iItemUID, IN const int iSocketIndex, IN const short sSocketOption );
//	bool SetItemSocketOption( IN int iCategory, IN int iSlotID, IN const int iSocketIndex, IN const short sSocketOption );
//#endif SERV_ITEM_OPTION_DATA_SIZE
	//}}

	bool SetAttribEnchant( IN UidType iItemUID, IN char cAttribEnchantSlotID, IN char cAttribEnchantType );
	bool SetAttribEnchant( IN int iCategory, IN int iSlotID, IN char cAttribEnchantSlotID, IN char cAttribEnchantType );

	//{{ 2009. 8. 27  최육사	봉인
	bool Seal( IN UidType iItemUID );
	bool Unseal( IN UidType iItemUID );
	//}}

    bool InsertItem( IN UidType iItemUID, IN const KItemInfo& kItemInfo, OUT KInventoryItemInfo& kInventorySlotInfo );
    bool InsertItem( IN const std::map< UidType, KItemInfo >& mapItem, OUT std::vector< KInventoryItemInfo >& vecInserted );

	bool			IsEmptyShareBankSlot();
	bool			IsEmptyShareItem();

	void			SetShareBankSize( int iBankSize )				{ m_iShareBankSize = iBankSize; }
	void			SetShareUnitUID( UidType iShareUnitUID )		{ m_iShareUnitUID = iShareUnitUID; }

	void			SetShareNickname( std::wstring wstrNickname )	{ m_wstrShareNickname = wstrNickname; }
	std::wstring	GetShareNickname()								{ return m_wstrShareNickname; }

	int				GetShareBankSize()								{ return m_iShareBankSize; }
	UidType			GetShareUnitUID()								{ return m_iShareUnitUID; }
	bool			GetIsShareBank()								{ return (m_iShareUnitUID > 0); }
	void			GetShareItemMap( OUT std::map< UidType, KInventoryItemInfo > &mapItem );

	bool			InsertShareItem( IN const UidType iItemUID, IN const KItemInfo& kItemInfo, IN const char cSlotCategory, IN const short sSlotID );
	bool			UpdateShareItemPosition( IN const KTradeShareItemResult& kShareResult );

	//bool			GetShareItemNeedDBUpdate( OUT KDBE_UPDATE_SHARE_ITEM_REQ &kReq );
	bool			GetShareItemNeedDBUpdate( IN const std::set< UidType >& setMovedItemUID, OUT KDBE_UPDATE_SHARE_ITEM_REQ &kReq );

	void			ClearShareBank();

	bool			IsShareItem( UidType iItemUID );

	//{{ 2009. 12. 15  최육사	아이템 삭제사유
	bool DeleteItem( IN UidType iItemUID, OUT KInventoryItemInfo& kInventorySlotInfo, IN KDeletedItemInfo::DELETE_REASON eReason );
	bool DeleteItem( IN int iCategory, IN int iSlotID, OUT KInventoryItemInfo& kInventorySlotInfo, IN KDeletedItemInfo::DELETE_REASON eReason );
	bool DeleteItemOnlyInventory( IN const std::map< int, int >& mapToDelete, OUT std::vector< KInventoryItemInfo >& vecUpdatedItem, IN KDeletedItemInfo::DELETE_REASON eReason );
	//}}
	//{{ 2010. 10. 26	최육사	퀘스트 조건 추가
	void DeleteItemAll( IN int iItemID, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	//}}

	//{{ 2009. 5. 13  최육사	아이템버리기
	bool DiscardItem( IN UidType iItemUID, IN int iQuantity, OUT KInventoryItemInfo& kInventorySlotInfo, IN bool bDebug = false );
	//}}

	bool UseQuickSlotItem( IN int iSlotID, OUT int& iItemID, OUT KInventoryItemInfo& kInventorySlotInfo );
	//{{ 2013. 04. 10	최육사	어둠의 문 개편
	bool UseDefenceDungeonQuickSlotItem( IN const int iSlotID, OUT int& iItemID, OUT KInventoryItemInfo& kInventorySlotInfo );
	//}}

	void ResetQuickSlotCoolTime();

	bool MoveItem( IN UidType iItemUID, IN int iDestCategory, IN int iDestSlotID, IN bool bCoolTimeCheck, OUT std::vector< KInventoryItemInfo >& vecChanged, OUT KDBE_SEAL_ITEM_REQ &kSealReq, IN std::vector< KSellPersonalShopItemInfo > vecPersonalShopItemInfo );
	bool MoveItem( IN int iSrcCategory, IN int iSrcSlotID, IN int iDestCategory, IN int iDestSlotID, IN bool bCoolTimeCheck, OUT std::vector< KInventoryItemInfo >& vecChanged, OUT KDBE_SEAL_ITEM_REQ &kSealReq, IN std::vector< KSellPersonalShopItemInfo > vecPersonalShopItemInfo );

	//{{ 2009. 6. 1  최육사		인벤토리 정렬
	bool SortInventory( IN int iCategory, OUT std::vector< KInventoryItemInfo >& vecChanged );
	//}}

	//{{ 2011. 06. 18	최육사	ED아이템 구매 체크
//#ifdef SERV_CHECK_BUY_ED_ITEM
	bool PrepareBuy( IN const std::map< int, int >& mapItem, IN const bool bDebug, IN const short sPeriod, OUT int& iTotalED, OUT int& iTotalPvPPoint, OUT std::vector< KInventoryItemInfo >& vecUpdatedItem, OUT std::vector< KBuyGPItemInfo >& vecNewItem );
//#endif SERV_CHECK_BUY_ED_ITEM
	//}}
    bool ProcessSell( IN const UidType iItemUID, IN const int iQuantity, OUT int& iItemID, OUT int& iED, OUT KInventoryItemInfo& kInventoryItemInfo );
    bool PrepareInsert( IN std::map< int, KItemInfo >& mapReward, OUT std::map< int, int >& mapReceived, OUT std::vector< KInventoryItemInfo >& vecUpdatedItem, OUT std::vector< KItemInfo >& vecNewItem, IN bool bTransaction = false );
	//{{ 2009. 3. 16  최육사	수량성이 아닌 아이템에 대한 prepareinsert
	bool PrepareInsertForTrade( IN const std::vector< KInventoryItemInfo >& vecReward, OUT std::map< int, int >& mapReceived, OUT std::vector< KInventoryItemInfo >& vecUpdatedItem, OUT std::vector< KItemInfo >& vecNewItem );
	//}}

	//{{ 2009. 12. 15  최육사	아이템 삭제사유
    bool DeleteAndInsert( IN const std::map< int, int >& mapToDelete,
						  IN const std::map< int, KItemInfo >& mapToInsert,
						  OUT std::vector< KInventoryItemInfo >& vecUpdatedItem,
						  OUT std::vector< KItemInfo >& vecNewItem,
						  IN bool bDeleteOnlyInventory,
						  IN KDeletedItemInfo::DELETE_REASON eReason
						  , IN const bool bOnlyInfinite = false	// 기한 제한이 없는 것
						  );

    bool DeleteAndInsert( IN const std::map< UidType, int >& mapToDelete,
						  IN const std::map< int, KItemInfo >& mapToInsert,
						  OUT std::vector< KInventoryItemInfo >& vecUpdatedItem,
						  OUT std::vector< KItemInfo >& vecNewItem,
						  IN KDeletedItemInfo::DELETE_REASON eReason
						  );

    bool DeleteAndInsert( IN const std::map< int, int >& mapToDeleteByItemID,
						  IN OUT std::map< UidType, int >& mapToDeleteByItemUID,
						  IN const std::map< int, KItemInfo >& mapToInsert,
						  OUT std::vector< KInventoryItemInfo >& vecUpdatedItem,
						  OUT std::vector< KItemInfo >& vecNewItem,
						  IN bool bDeleteOnlyInventory,
						  IN KDeletedItemInfo::DELETE_REASON eReason
						  , IN const bool bOnlyInfinite = false	// 기한 제한이 없는 것
						  );
	//}}

    void FlushQuantityChange( OUT std::map< UidType, int >& mapChanged );
    void RollBackInitQuantity( IN const std::map< UidType, int >& mapChanged );
	void FlushEnduranceChange( OUT std::map< UidType, int >& mapChanged );
	void RollBackInitEndurance( IN const std::map< UidType, int >& mapChanged );
	//{{ 2010. 8. 3	최육사	펫 시스템
//#ifdef SERV_PET_SYSTEM
	void FlushPositionChange( OUT std::map< UidType, KItemPosition >& mapChanged );
	void PositionUpdateComplete( IN const std::map< UidType, KItemPosition >& mapChanged );
//#else
//	void FlushPositionChange( OUT std::map< UidType, std::pair< int, int > >& mapChanged );
//	void PositionUpdateComplete( IN const std::map< UidType, std::pair< int, int > >& mapChanged );
//#endif SERV_PET_SYSTEM
	//}}
	//{{ 2009. 12. 15  최육사	아이템 삭제사유
	void FlushDeletedItem( OUT std::vector< KDeletedItemInfo >& vecDeleted );
	void RollBackDeletedItem( IN const std::vector< KDeletedItemInfo >& vecDeleted );
	//}}

	//장착된 아이템의 내구도를 떨어트린다.
	void DecreaseEquippedItemEnduranceInPvp();
	void DecreaseEquippedItemEnduranceInDungeon( IN const int iExpDropNpcNum,
												 IN const int iPartyNumMember,
												 OUT std::vector< KDecreaseEnduranceInfo >& vecSlotInfo,
												 OUT std::vector< KInventoryItemInfo >& vecZeroEndurance );

	void DecreaseEquippedItemEndurance( IN const int iEnduranceDamage,
										IN const int iExpDropNpcNum,
										IN const int iPartyNumMember,
										OUT std::vector< KDecreaseEnduranceInfo >& vecSlotInfo,
										OUT std::vector< KInventoryItemInfo >& vecZeroEndurance );

	//{{ 2012. 03. 06	최육사	배틀필드 시스템
	void DecreaseEquippedItemEnduranceInBattleField( IN const int iExpDropNpcNum,
													 IN const int iPartyNumMember,
													 OUT std::vector< KDecreaseEnduranceInfo >& vecSlotInfo,
													 OUT std::vector< KInventoryItemInfo >& vecZeroEndurance );
	//}}

	//{{ 버프 관리 시스템 - 김민성
	bool RepairItem( IN const std::vector< UidType > vecItemUID, OUT int& iED, OUT int &iVP, OUT std::vector< KInventoryItemInfo >& vecUpdated, IN KGSUserPtr pUser );
		//}

	bool GetZeroOfEndurancebyEquippedItem( IN std::vector<UidType>& vecItemUID, OUT std::vector<UidType>& vecZeroItemUID );	//수리될 아이템중 장착중이고 내구도가 0인 아이템을 찾는다.

	//////////////////////////////////////////////////////////////////////////
	// 아이템 가공
	//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
	//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
	bool OpenRandomItem( IN char cUnitClass, IN UidType iItemUID, IN const int iEventItemID, OUT std::map< int, int >& mapInsertedItem, OUT std::vector< KInventoryItemInfo >& vecUpdated, OUT std::vector< KItemInfo >& vecNewItem, OUT int& iRessurectionCount, OUT int& iRestoreSpirit, OUT int& iED, OUT bool& bCharmItem );
	//}
	//}}


	bool ItemManufacture( IN int iManufactureID, IN int iQuantity, OUT int& iED, OUT std::map< int, int >& mapInsertedItem, OUT std::vector< KInventoryItemInfo >& vecUpdated, OUT std::vector< KItemInfo >& vecNewItem );

	bool ResolveItem( IN UidType iItemUID, IN OUT bool& bJackpot, OUT bool& bFashion, OUT int& iResolveQuantity, OUT std::map< int, int >& mapInsertedItem, OUT std::vector< KInventoryItemInfo >& vecUpdated, OUT std::vector< KItemInfo >& vecNewItem, IN bool bForceInsertItem = false );
	//{{ 2011.03.16   임규수 아바타 분해 시스템
	//}}
	//{{ 2011.5.23 지헌 : 강화시 파괴 방지 아이템 추가
	bool EnchantItem( IN UidType iItemUID, IN bool bIsRareEnchantStone, IN bool bIsNewEnchant, IN bool bIsSupportMaterial, IN bool bDebug, OUT int& iEnchantResult, OUT int& iLevelAfterEnchant, OUT int& iED, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	//}}
	//{{ 2008. 12. 21  최육사	강화 복구
	bool RestoreItem( IN UidType iItemUID, OUT int& iLevelAfterEnchant, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	//}}
	//{{ 2010. 04. 06  최육사	마법석 개편
//#ifdef SERV_SOCKET_NEW
	//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
//#ifdef SERV_ITEM_OPTION_DATA_SIZE
	bool SocketItem( IN UidType iItemUID
				   , IN const std::map< int, UidType >& mapSocketInfo
				   , IN bool bCheat
				   , OUT int& iTargetItemID
				   , OUT int& iED
				   , OUT std::vector< int >& vecSocketResult
				   , OUT std::vector< KInventoryItemInfo >& vecUpdated
				   , OUT byte& byteExpandedSocketNum
				   );
//#endif SERV_ITEM_OPTION_DATA_SIZE
//	//}}
//#endif SERV_SOCKET_NEW
	//}}
	bool AttribEnchantItem( IN UidType iItemUID, IN char cAttribEnchantSlotNo, IN char& cAttribEnchantType, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	bool IdentifyItem( IN UidType iItemUID, IN int iQuantity, OUT std::map< int, int >& mapInsertedItem, OUT std::vector< KInventoryItemInfo >& vecUpdated, OUT std::vector< KItemInfo >& vecNewItem );
	bool UseItemInInventory( IN UidType iItemUID, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo, IN bool bNotDeleteItem = false );

	//{{ 2013. 02. 19   교환 로그 추가 - 김민성
	bool ItemExchangeCheck( IN const int iHouseID
		, IN const UidType iSourceItemUID
		, IN const int iSourceItemID
		, IN const int iSourceQuantity
		, IN const int iDestItemID
		);

	bool ItemExchange( IN const int iHouseID,
		IN const UidType iSourceItemUID,
		IN const int iSourceItemID,
		IN const int iSourceQuantity,
		IN const int iDestItemID,
		OUT std::map< int, int >& mapInsertedItem,
		OUT std::vector< KInventoryItemInfo >& vecUpdated,
		OUT std::vector< KItemInfo >& vecNewItem,
		OUT char& cExchangeType	);
	//}

	//{{ 2008. 12. 25  최육사	부여
	bool EnchantAttachItem( IN UidType iDestItemUID, IN UidType iAttachItemUID, OUT int& iLevelAfterEnchant, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	//}}
	//{{ 2009. 8. 27  최육사	봉인
	bool SealItem( IN UidType iDestItemUID, IN UidType iSealItemUID, OUT u_char& ucBeforeSealCount, OUT u_char& ucSealResult, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	bool UnsealItem( IN UidType iDestItemUID, OUT u_char& ucSealResult, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	//}}

	//{{ 2009. 9. 22  최육사	길드
	bool UseGuildFoundationItemCheckOnly( IN UidType iItemUID );
	bool UseGuildFoundationItem( IN UidType iItemUID, OUT std::vector< KInventoryItemInfo >& vecUpdated, IN bool bCheckOnly );
	//}}

	//{{ 2009. 11. 24  최육사	길드스킬
	bool ResetGuildSkillItemCheckOnly();
	bool ResetGuildSkillItem( OUT std::vector< KInventoryItemInfo >& vecInventorySlotnfo );
	bool InitGuildSkillItemCheckOnly( IN UidType iItemUID );
	bool InitGuildSkillItem( IN UidType iItemUID, OUT std::vector< KInventoryItemInfo >& vecInventorySlotnfo );
	//}}

	int  CheckItemForTrade( IN UidType iItemUID, IN int iQuantity, OUT KInventoryItemInfo& kInventoryItemInfo );
    void ExpireItem( OUT std::vector< UidType >& vecItemUID, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );

	bool ResetSkillItem( OUT KInventoryItemInfo& kInventoryItemInfo );

    bool CompareUnitClass( IN int iItemID );
    bool IsAbleToExpandSlot( IN int nSlot );

	//{{ 2011. 12. 13  김민성	퀵 슬롯 확장 아이템
	bool IsAbleToExpandSlot( IN int iCategory, IN int nSlot );			// 확장권 구입이 가능한지 확인용( nSlot 만큼 여유분이 있는가)
	//}}

	//{{ 2009. 8. 7  최육사		은행
	bool IsAbleToExpandBank( IN int nSlot );
	//}}
    void ExpandSlot( IN int iCategory, IN int iSlot, OUT int& iExpanded );

	//{{ 2009. 2. 10  최육사	개인상점 버그예방
	bool CheckTradeItemOut( IN const std::vector< KInventoryItemInfo >& vecItemOut, OUT KItemQuantityUpdate& kUpdateTemp, IN CXSLRoom::ROOM_TYPE eType );
	bool CheckTradeItemIn( IN const std::vector< KInventoryItemInfo >& vecItemIn, OUT KItemQuantityUpdate& kUpdateTemp, OUT std::vector< KItemInfo >& vecUpdateTemp, IN CXSLRoom::ROOM_TYPE eType );

	bool TrialPrepareInsert( IN const std::vector< KInventoryItemInfo >& vecReward, OUT KItemQuantityUpdate& kUpdateTemp, OUT std::vector< KItemInfo >& vecUpdateTemp );
	bool TrialPrepareInsert( IN const std::map< int, KItemInfo >& mapReward, OUT KItemQuantityUpdate& kUpdateTemp, OUT std::vector< KItemInfo >& vecUpdateTemp );
	//}}

	//{{ 2010. 02. 08  최육사	PC방 채널 이동 오류 수정
	void GetChannelChangeInventoryInfo( OUT std::vector< KInventoryItemInfo >& vecPcBangItemInfo );
	void SetChannelChangeInventoryInfo( IN const std::vector< KInventoryItemInfo >& vecPcBangItemInfo, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	//}}

	//{{ 2010. 8. 3	최육사	펫 시스템
	void InitPetInventory( IN UidType iSummonedPetUID, IN int iPetCategorySlotSize, IN const std::map< UidType, KInventoryItemInfo >& mapPetItem, OUT std::vector< KInventoryItemInfo >& vecPetInventorySlotInfo );
	void ResetPetInventorySize( IN int iPetCategorySlotSize );
	void RemovePetInventory( OUT std::map< UidType, KInventoryItemInfo >& mapRemovedPetItem );
	bool FeedPetInInventory( IN UidType iItemUID, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	bool AutoFeedInPetInventory( IN u_char ucLevel, IN bool bOnlyEatSpecial, OUT int& iFeedItemID, OUT std::vector< KInventoryItemInfo >& vecPetInventorySlotInfo );
	//}}

	//{{ 2010. 8. 16	최육사	기간 리셋 아이템 이벤트
	void GetResetPeriodItemUIDList( OUT std::map< UidType, short >& mapExpandPeriodItemList ) const;
	bool UpdatePeriod( IN UidType iItemUID, IN short sPeriod, IN const std::wstring& wstrExpirationDate, OUT KInventoryItemInfo& kInventorySlotInfo );
	//}}

	//{{ 2011. 01. 04  김민성	속성 부적
	bool AttribAttachItem( IN UidType iDestItemUID, IN UidType iAttachItemUID, OUT KItemAttributeEnchantInfo& kAttribAttachResult, OUT std::vector< KInventoryItemInfo >& vecUpdated );
	//}}

	//{{ 2011. 04. 18	최육사	대리 상인
	bool DeleteItemForPersonalShopAgency( IN const std::vector< KSellPersonalShopItemInfo >& vecSellItemInfo, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	//}}

	//{{ 2011. 01. 06  김민성  스킬슬롯체인지 체크(인벤토리-기간제) 기능 구현
	bool GetSlotChangeBEndDate( IN const int iItemID, IN OUT std::wstring& wstrExpirationDate );

	//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
	//}}

	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	void SetClassChangeItem(  IN std::map< UidType, int >& mapItem, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo  );
	//}}

//	//{{ 2012. 02. 21	김민성	인벤토리 정렬
//#ifdef SERV_SORT_CATEGORY_ITEM
//	//{{ 2012. 02. 27	김민성	인벤토리 정렬	- 무거운 기능을 가볍게 변경
//#ifdef SERV_SORT_CATEGORY_ITEM_FIX
	typedef std::pair< int, UidType >	KInventoryItemSortInfo;		// < itemid, itemuid >

	bool SortCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot, OUT int& iNetError, IN const std::vector< KSellPersonalShopItemInfo > vecPersonalShopSell_ItemInfo );
	void SortEquipCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot );
	void SortConsumptionCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot );
	void SortNormalCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot );
//#else
//	bool SortCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot, OUT int& iNetError );
//	void SortEquipCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot );
//	void SortConsumptionCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot );
//	void SortNormalCategory( IN const int iCategory, OUT std::vector< UidType >& vecUpdatedInventorySlot );
//#endif SERV_SORT_CATEGORY_ITEM_FIX
//	//}}
//#endif SERV_SORT_CATEGORY_ITEM
	//}}

	void	SetUseItemNeedPayment(bool bVal ) {	m_bUseItemNeedPayment = bVal; }
	bool	GetUseItemNeedPayment() const { return m_bUseItemNeedPayment; }

	//{{ 2012. 03. 29	최육사	Inventory Lock 기능
	bool IsLocked()																						{ return m_kInventoryLocker.IsLocked(); }
	bool Lock( IN const KInventoryLocker::LOCK_TYPE eLockType, IN const double fLockTime = 99999.0 )	{ return m_kInventoryLocker.Lock( eLockType, fLockTime ); }
	void Unlock()																						{ m_kInventoryLocker.Unlock(); }
	//}}

	//{{ 2012. 04. 26	박세훈	펫 장난감
	bool CheckPetToyItem( void );
	//}}
	bool CheckKeepItem( const int nProductID );
	//{{ 2012. 08. 23	박세훈	우편 로직 변경
	//}}

	//{{ 2012. 10. 04	최육사		ED&아이템 모니터링 시스템
	//}}

	//{{ 2012. 12. 26	박세훈	인벤토리 개편 테스트	- 허상형 ( Merged by 박세훈 )
	int		GetExpandedSlotSize( IN const int iCategory ) const	{ return GetSlotSize( iCategory ) - GetBaseSlotSize( iCategory ); }
	//}}
	//{{ 2011.05.04   임규수 아바타 합성 시스템
	//}}
	bool CheckKeepItem( void );
	bool CheckKeepItem2( void );
	bool CheckKeepItem3( void );
	bool CheckKeepItem4( void );
	bool CheckKeepItem5( void );
	bool CheckKeepItem6( void );

	bool ExchangeToEvent( IN int iDeleteItemID, IN int iDeleteItemCount, IN int iInsertItemID, IN int iInsertItemCount, OUT std::vector< KInventoryItemInfo >& vecUpdatedInventorySlot, OUT std::vector< KItemInfo >& vecNewItemInfo, IN int iFirstUnitClass  );

//{{ 2010. 10. 27	최육사	퀘스트 조건 추가
	void GetItemUIDListThisItem( IN int iItemID, OUT std::vector< UidType >& vecItemUIDList, IN bool bOnlyInventory );
	//}}

	//{{ 2013. 04. 10	최육사	어둠의 문 개편
	void	SetupDefenceDungeonQuickSlotItems( IN const int iDefenceDungeonPhase, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	//}}

	//{{ 2013. 05. 15	최육사	아이템 개편
	bool	ItemEvaluateCheck( IN const UidType iItemUID, OUT std::vector< int >& vecRandomSocket, OUT int& iCost );
	bool	ItemEvaluateResult( IN const UidType iItemUID, IN const std::vector< int >& vecRandomSocket, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );

	bool	RestoreItemEvaluateCheck( IN const UidType iSupportItemUID, IN const UidType iTargetItemUID, OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );
	bool	RestoreItemEvaluateResult( IN const UidType iTargetItemUID, IN OUT std::vector< KInventoryItemInfo >& vecInventorySlotInfo );

	bool	ItemConvert( IN const UidType& iItemUID, IN const int& iItemQuantity_, OUT std::map< int, int >& mapInsertedItem, OUT std::vector< KItemInfo >& vecNewItem, OUT std::vector< KInventoryItemInfo >& vecUpdated, OUT int& iCommissionED );
	//}}


	bool IsPossibleExtractItem( const int iItemID );

	bool ItemExtract( IN const UidType iSourceItemUID,
		IN const int iSourceQuantity,
		OUT std::map< int, int >& mapInsertedItem,
		OUT std::vector< KInventoryItemInfo >& vecUpdated,
		OUT std::vector< KItemInfo >& vecNewItem	);

	bool UseFinalitySkill( IN const UidType iSourceItemUID,
		IN const int iSourceQuantity,
		OUT std::map< int, int >& mapInsertedItem,
		OUT std::vector< KInventoryItemInfo >& vecUpdated,
		OUT std::vector< KItemInfo >& vecNewItem	);

    int GetNextUpgradeBankED( IN const int& iNextGrade_ );
    int GetNextUpgradeInventoryED( IN const int& iNextGrade_ );


	bool	UpdateExpandedSocketNum( IN UidType iItemUID, IN const byte byteExpandedSocketNum );


protected:
    bool VerifyCategorySlotID( IN const int iCategory, IN const int iSlotID ) const;
	bool VerifyInventoryCategory( IN int iItemID, IN int iCategory );
    int  GetRemainedQuantityCapacity( IN UidType iItemUID );
    int  GetRemainedQuantityCapacity( IN int iCategory, IN int iSlotID );
    int  GetEnduranceToRestore( IN UidType iItemUID );
    int  GetEnduranceToRestore( IN int iCategory, IN int iSlotID );
    bool CompareLevel( IN int iItemID );
	//{{ 2012. 07. 2	박세훈	소비성 아이템 획득 시 퀵슬롯 자동 등록
    int  GetSuitableCategory( IN int iItemID );
	//}}

    bool GetEmptyCategorySlotForItem( IN int iItemID, OUT int& iCategory, OUT int& iSlotID );
	void GetNumFreeSlot( OUT std::map< int, int >& mapNumFreeSlot );
    void GetInventoryItemInfoContainingThisItem( IN const int iItemID, OUT std::vector< KInventoryItemInfo >& vecInventoryItemInfo, const IN bool bOnlyInventory );
	void GetInventoryItemInfoContainingThisItem( IN const int iItemID
		, OUT std::vector< KInventoryItemInfo >& vecInventoryItemInfo
		, IN const bool bOnlyInventory
		, IN const bool bOnlyInfinite	// 기한 제한이 없는 것
		);
	float GetRepairItemDiscount();
	int	 GetEquippedSetItemCount( IN int iSetID );

	//{{ 2011. 01. 20	최육사	아이템 분해 개편
	bool GetSellPrice( IN const UidType iItemUID, IN const int iQuantity, OUT int& iSellPrice );
	//}}

	bool CheckCanMoveShareItem( IN UidType iItemUID, OUT KDBE_SEAL_ITEM_REQ &kSealReq );		//	은행공유 아이템 이동가능 체크

	//{{ 2011. 09. 26  임규수 칭호 수리비 감소 속성 적용
	float GetRepairTitleDiscount( IN const int iTitleID );

	//}}

protected:
    std::map< UidType, KInventoryItem >			m_mapItem;									// 인벤토리 아이템 정보
	std::vector< KDeletedItemInfo >				m_vecDeletedItem;							// 삭제 예정인 아이템 정보(삭제 사유 포함)
    std::vector< UidType >						m_vecInventorySlot[CXSLInventory::ST_END];	// 인벤토리 슬롯 정보
    std::map< int, KTimerManager >				m_mapItemCoolTime;							// 아이템 쿨타임
    KGSUser*						            m_pUser;
	std::set< int >								m_setGetMyInventory;						// 클라이언트가 인벤토리 정보 가져갔는지 확인하기 위한 정보

	int											m_iShareBankSize;
	UidType										m_iShareUnitUID;
	std::wstring								m_wstrShareNickname;						//	은행공유 하고있는 닉네임

	std::set< UidType >							m_setShareItem;								//	은행공유 용도로 가져온 아이템
	std::vector< UidType >						m_vecDeletedShareItem;						//	은행에서 빠져나가서 삭제되어야 할 아이템 추가

	//{{ 2012. 02. 22	박세훈	길드 이름 변경권
	std::map< UidType, KInventoryItem >			m_mapDeletedItem;
	//}}
	//{{ 2012. 03. 29	최육사	Inventory Lock 기능
	KInventoryLocker							m_kInventoryLocker;							// 인벤토리 잠금 장치
	//}}
	//{{ 2012. 10. 04	최육사		ED&아이템 모니터링 시스템
	//}}

	bool										m_bUseItemNeedPayment;
};
