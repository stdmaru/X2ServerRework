#pragma once

class CX2SlotItem : public CX2SlotManager::CX2Slot
{
	public:
		CX2SlotItem(void);
		virtual ~CX2SlotItem(void);

		virtual void ResetPos();

		CX2Inventory::SORT_TYPE GetSortType() { return m_SortType; }
		void SetSortType( CX2Inventory::SORT_TYPE sortType ) { m_SortType = sortType; }

		int GetSlotID() { return m_SlotID; }
		void SetSlotID( int slotID ) { m_SlotID = slotID; }

		bool GetFashion() { return m_bFashion; }
		CX2Unit::EQIP_POSITION GetEquipPos() { return m_EquipPos; }

		wstring GetSlotDesc() { return m_SlotDesc; }

		void SetItemUID( UidType ItemUID ) { m_ItemUID = ItemUID; }
		UidType GetItemUID() { return m_ItemUID; }

		int GetItemTID() { return m_ItemTID; }
		void SetItemTID( int val ) { m_ItemTID = val; }

		bool CreateItemUI( CX2Item* pItem, int itemQuantity = -1 );
		bool CreateItemUI( int itemID, int itemQuantity = -1, bool bUseImpossibleImg = false, bool bQuantityStr = false );
		bool CreateItemUI(
            const CX2Item::ItemTemplet* pItemTemplet,
            int itemQuantity = -1, bool bUseImpossibleImg = false, bool bQuantityStr = false  );

		bool CreateMaterialItemUI(
            const CX2Item::ItemTemplet* pItemTemplet,
            int itemQuantity, int needItemQuantity );

		bool IsCreatedItemUI() { return m_pDLGSlot != NULL; }
		bool IsResetItemUI();

		bool ResetItemUI( CX2Item* pItem );
		bool ResetItemUI( int itemID, int itemQuantity = -1, bool bUseImpossibleImg = false, bool bQuantityStr = false  );

		bool LostItemUI();

		void DestroyItemUI();

		void SetKBillOrderInfo( KBillOrderInfo& kKBillOrderInfo );
		KBillOrderInfo& GetKBillOrderInfo() { return m_KBillOrderInfo; }


		void CreateRibbonPicture(
            const CX2Item::ItemTemplet* pItemTemplet,
            CKTDGUIStatic* pStatic );
		void SetShowImpossiblePicture(bool bShow);

		//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘
		void CreateSealedStatic();				// 봉인아이콘Picture를 가진 static 생성
		void SetShowSealedImage( bool bShow );	// 봉인아이콘을 보일것인가 말것인가?
		//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘

		//{{ oasis907 : 김상윤 [2010.5.11] // 소켓 강화 사용중인 아이템 표시
		void CreateSocketUseStatic();
		void SetShowSocketUseImage( UidType Uid );
		//}}
		void SetShowExchangeSelectImage( bool bShow );
		void SetShowExchangeUseImage( int itemID );
		void ExchangeReadyEffectStart();
		void ExchangeReadyEffectEnd();

		int GetPeriod() { return m_Period; }
		void SetPeriod( IN int iPeriod ) { m_Period = iPeriod; }

		void CreateSellWaitingStatic();						/// 개인 상점에 등록중인 아이템 표시 아이콘 생성
		void SetShowWaitingSellImage( IN bool bShow_ );		/// 개인 상점에 등록중인 아이템 표시 아이콘 활성 여부


	protected:

		void LuaGetValue( KLuaManager& luaManager );

		CKTDGUIDialogType CreateItemUIDialog();

		CX2Inventory::SORT_TYPE m_SortType;

		bool					m_bFashion;
		CX2Unit::EQIP_POSITION	m_EquipPos;

		wstring					m_SlotDesc;

		UidType					m_ItemUID;
		int						m_ItemTID;

		bool					m_bReset;

		int						m_Period;	//몇일 짜리 기간제 아이템인가?

		KBillOrderInfo			m_KBillOrderInfo;

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hEffectExchangeReady;

};
