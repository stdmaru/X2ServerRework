#pragma once

class CX2NPCUnitViewerUI;



class CX2CashShop : public CX2ItemSlotManager
{
	public:

		enum CASH_SHOP_CUSTOM_UI_MSG
		{
			CSCUM_CASH_CHARGE = 1800,
			CSCUM_BUY,
			CSCUM_PRESENT,
			CSCUM_STORE,	//JJIM

			CSCUM_MAIN_NEW_PREV_PAGE,
			CSCUM_MAIN_NEW_NEXT_PAGE,
			CSCUM_MAIN_HOT_PREV_PAGE,
			CSCUM_MAIN_HOT_NEXT_PAGE,
			CSCUM_MAIN_RECOMMEND_PREV_PAGE,
			CSCUM_MAIN_RECOMMEND_NEXT_PAGE,

			CSCUM_BUY_POP_UP_PREV_PAGE,			// 구매하기 UI, 상품 페이지 1 변경
			CSCUM_BUY_POP_UP_NEXT_PAGE,			// 구매하기 UI, 상품 페이지 1 변경
			CSCUM_BUY_POP_UP_ALL_CHECK_CHANGED,
			CSCUM_BUY_POP_UP_EACH_CHECK_CHANGED,
			CSCUM_BUY_POP_UP_PRICE_CHANGED,		//리스트 박스에서 가격 변경
			CSCUM_BUY_POP_UP_OK,
			CSCUM_BUY_POP_UP_CANCEL,

			CSCUM_CASH_ITEM_DEPOSIT_PREV_PAGE,
			CSCUM_CASH_ITEM_DEPOSIT_NEXT_PAGE,

			CSCUM_CATEGORY_CHANGE,
			CSCUM_SUB_CATEGORY_CHANGE,

			CSCUM_SUB_PREV_PAGE,
			CSCUM_SUB_NEXT_PAGE,
			CSCUM_SUB_SEE_ALL_ITEM_OPTION_CHANGE,

			CSCUM_TAKE_OFF_ALL_ITEM,

			CSCUM_ITEM_TO_INVEN_POP_UP_PREV_PAGE,
			CSCUM_ITEM_TO_INVEN_POP_UP_NEXT_PAGE,
			CSCUM_ITEM_TO_INVEN_POP_UP_OPTION_CHANGED,
			CSCUM_ITEM_TO_INVEN_POP_UP_OK,
			CSCUM_ITEM_TO_INVEN_POP_UP_CANCEL,

			CSCUM_ITEM_TO_INVEN_CHECK_OK,
			CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK,

			CSCUM_BUY_ALL_EQUIPPING_ITEMS,

			CSCUM_CHOICED_BUY,
			CSCUM_CHOICED_PRESENT,
			CSCUM_CHOICED_REMOVE_IN_LIST,
			CSCUM_CHOICED_PRE_EQUIPPING,
			CSCUM_CHOICED_CLOSE,
			CSCUM_CHOICED_BUY_ALL,

			CSCUM_PRESENT_POP_UP_CHANGED_CASH_PRODUCT,
			CSCUM_PRESENT_POP_UP_CAHNGED_FRIEND,
			CSCUM_PRESENT_OK,
			CSCUM_PRESENT_CANCEL,

			CSCUM_BUY_POP_UP_OVERLAP_OK,
			CSCUM_BUY_POP_UP_OVERLAP_CANCEL,

			CSCUM_ITEM_TO_INVEN_POP_UP_ALL_CHECK_CHANGED,
			CSCUM_ITEM_TO_INVEN_POP_UP_EACH_CHECK_CHANGED,
			CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK_PACKAGE,

			CSCUM_BUY_PLATINUM_CARD_OK,
			CSCUM_BUY_PLATINUM_CARD_CANCEL,
//#ifdef CASH_ITEM_REFUND
			CSCUM_CASH_ITEM_REFUND,
			CSCUM_CASH_ITEM_REFUND_OK,
//#endif CASH_ITEM_REFUND

#ifdef CASH_INVEN_PICKUP_ALL
			CSCUM_PICKUP_ALL							= 1856,
#endif //CASH_INVEN_PICKUP_ALL


			CSCUM_USE_BONUS_SELECT						= 1891,
			CSCUM_USE_GAMEPOINT_SELECT					= 1892,
			CSCUM_PRESENT_USE_BONUS_SELECT				= 1893,
			CSCUM_PRESENT_USE_GAMEPOINT_SELECT			= 1894,
			CSCUM_USE_GASH_SELECT						= 1895,
			CSCUM_PRESENT_USE_GASH_SELECT				= 1896,


//#ifdef SERV_NEXON_AUTH_SOAP
			CSUCM_CHECK_AUTHORITY_OK,
//#endif //SERV_NEXON_AUTH_SOAP
//#ifdef SERV_EVENT_DAILY_GIFT_BOX
			CSUCM_CLOSE_DAILY_GIFT_BOX,
//#endif SERV_EVENT_DAILY_GIFT_BOX
			//}}
			CSCUM_TAKE_OFF_ALL_ITEM_AND_INVEN_EQUIP_ITEM,
//#ifdef SAVE_CASH_SHOP_PRE_EQUIP_ITEM_LIST
			CSCUM_RADIO_BUTTON_PRE_EQUIP_SET,
//#endif //SAVE_CASH_SHOP_PRE_EQUIP_ITEM_LIST
			CSCUM_BUY_COUPON_COMBOBOX_CHANGED, // 쿠폰 콤보박스 선택

//#ifdef COUPON_SYSTEM
			CSCUM_BUY_POP_UP_TO_FIRST_PAGE,			// 구매하기 UI, 상품 페이지 첫 페이지로 변경
			CSCUM_BUY_POP_UP_TO_LAST_PAGE,			// 구매하기 UI, 상품 페이지 첫 페이지로 변경
//#endif // COUPON_SYSTEM

		};

		enum CASH_SHOP_STATE
		{
			CSS_MAIN = 0,
			CSS_SUB,
		};



		enum CASH_SHOP_CATEGORY
		{
			CSC_NONE	= 0,
			CSC_FASHION,
			CSC_ACCESSORY,
			CSC_CONSUME,
			CSC_INSTALL,
			CSC_PET,
			CSC_EVENT,
			//{{ kimhc // 2009-09-29 // 자동결제 카테고리 추가
			CSC_AUTO_PAYMENT,
			//}} kimhc // 2009-09-29 // 자동결제 카테고리 추가
			CSC_END,
		};

		enum CASH_SHOP_SUB_CATEGORY
		{
			// CSC_FASHION
			CSSC_ALL			= 0,

			CSSC_FASHION_WEAPON = 1,
			CSSC_FASHION_BODY,
			CSSC_FASHION_LEG,
			CSSC_FASHION_HAIR,
			CSSC_FASHION_HAND,
			CSSC_FASHION_FOOT,
//#ifdef SERV_NEW_ONE_PIECE_AVATAR_SLOT
			CSSC_FASHION_ONE_PIECE,
//#endif SERV_NEW_ONE_PIECE_AVATAR_SLOT

			// CSC_ACCESSORY
			CSSC_ACCESSORY_FACE = 50,
			CSSC_ACCESSORY_BODY,
			CSSC_ACCESSORY_LEG,
			CSSC_ACCESSORY_ARM,
			CSSC_ACCESSORY_RING,
			CSSC_ACCESSORY_NECKLACE,
			CSSC_ACCESSORY_WEAPON,

			CSSC_CONSUME_RESTORE = 100,
			CSSC_CONSUME_PREMIUM,
			CSSC_CONSUME_INIT,
			CSSC_CONSUME_ETC,

			CSSC_INSTALL_ADD_SLOT = 150,
			CSSC_INSTALL_PERSONAL_SHOP,
			CSSC_INSTALL_ETC,

			CSSC_PET_PET = 200,
			CSSC_PET_CONSUME,
//#ifdef RIDING_SYSTEM
			CSSC_PET_RIDING,
//#endif //RIDING_SYSTEM

			CSSC_EVENT_PACKAGE = 250,
			CSSC_EVENT_FASHION,
			CSSC_EVENT_ACCESSORY,
			CSSC_EVENT_CONSUME,
			CSSC_EVENT_INSTALL,
			CSSC_EVENT_PET,

			CSSC_AUTO_PAYMENT_BASIC = 300,

			CSSC_EVENT_2_EVENT		= 400,//ADD_CASH_SHOP_CATEGORY_EVENT_2
		};

		//{{ kimhc // 2009-10-14 // 웹페이지 종류
		//}} kimhc // 2009-10-14 // 웹페이지 종류

		struct CashShopCategory
		{
			D3DXVECTOR2				m_CategoryID;		//x가 고정, y가 실제
			vector< D3DXVECTOR2 >	m_vecSubCategoryID;
		};

		struct CashShopSlot
		{
			int				m_ItemID;
			CKTDGUIDialogType	m_pDLGCashShopSlot;

			CX2Slot::SLOT_TYPE m_SlotType;
			int m_SlotIndex;

			CashShopSlot( CX2Slot::SLOT_TYPE cashShopSlotType, int slotIndex );
			~CashShopSlot();

			void SetItemID( int itemID ) { m_ItemID = itemID; }
			int	GetItemID() { return m_ItemID; }

			void SetShow( bool bShow );
			//{{ kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가
//#ifdef	AUTO_PAYMENT
			void SetInfo( bool bAutoPayment = false );
//#else	AUTO_PAYMENT
//			void SetInfo();
//#endif	AUTO_PAYMENT
			//}} kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가
			wstring GetCutString( const WCHAR* wszItemName, int maxItemNameLen );

			//{{ kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가
			//}} kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가

			void SetShowPresentButton( bool bShow );

		};


		class CashShopSlotManager
		{
			public:

				CashShopSlotManager( CX2Slot::SLOT_TYPE cashShopSlotType );
				~CashShopSlotManager();

				//{{ kimhc // 2009-10-12 // 자동결제 시 캐시 - 원 으로 표시
				void Reset( const vector<int>& vecItemID, bool bAutoPayment = false, bool bPossibleItemOnly = false );
				//}} kimhc // 2009-10-12 // 자동결제 시 캐시 - 원 으로 표시

				//{{ kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가
//#ifdef	AUTO_PAYMENT
				void SetOpen( bool bOpen, bool bAutoPayment = false );
//#else	AUTO_PAYMENT
//				void SetOpen( bool bOpen );
//#endif	AUTO_PAYMENT
				//}} kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가

				int	GetNowPage() { return m_NowPage; }
				int GetMaxPage() { return m_MaxPage; }

				//{{ kimhc // 2009-10-12 // 자동결제 시 캐시 - 원 으로 표시
				bool NextPage( bool bAutoPayment = false );
				bool PrevPage( bool bAutoPayment = false );
				//}} kimhc // 2009-10-12 // 자동결제 시 캐시 - 원 으로 표시

				vector<int> GetNowShowItemIDList();//현재 보여지는 아이템 아이디 리스트
				int	 GetItemID( CKTDGUIDialogType pDialog );

				//{{ kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가
				//}} kimhc // 2009-10-12 // 자동결제 시 선물, 찜 버튼 안보이게 하기 위해 추가


				void SetShowPresentButton();
				bool IsPossiblePresent( int itemID );

			protected:

				//{{ kimhc // 2009-10-12 // 자동결제 시 캐시 - 원 으로 표시
//#ifdef	AUTO_PAYMENT
				void ResetNowPage( bool bAutoPayment = false );
//#else	AUTO_PAYMENT
//				void ResetNowPage();
//#endif	AUTO_PAYMENT
				//}} kimhc // 2009-10-12 // 자동결제 시 캐시 - 원 으로 표시

				bool IsPossibleUsedByMyBankMembership( int itemID ) const;

				bool m_bOpen;

				int m_NowPage;
				int m_MaxPage;

				int	m_MaxSlotNumPerPage;

				vector< int	>			m_vecItemID;
				vector< CashShopSlot* > m_vecCashShopSlot;
		};

		class BuyItemPopupSlot : public CX2PageMgrItem::CX2PageSlotItem
		{
			public:

				BuyItemPopupSlot();
				BuyItemPopupSlot( int index, const WCHAR* pFileName );
				~BuyItemPopupSlot();

				void UpdateUI();
				void SetCheck( bool bCheck );
				void SetCash( int comboBoxIndex );

				CX2PageSlot* Clone( int index, const WCHAR* pFileName ) { return new BuyItemPopupSlot( index, pFileName ); }


		};

		class CashItemToInvenPopupSlot : public CX2PageMgrItem::CX2PageSlotItem
		{
			public:
				CashItemToInvenPopupSlot();
				CashItemToInvenPopupSlot( int index, const WCHAR* pFileName );
				~CashItemToInvenPopupSlot();


				void UpdateUI();

				void SetKBillOrderInfo( KBillOrderInfo& kKBillOrderInfo ) { m_KBillOrderInfo = kKBillOrderInfo; }
				void SetKBillPackageInfo( KBillPackageInfo& kKBillPackageInfo ) { m_KBillPackageInfo = kKBillPackageInfo; }
				void AddItemToComboBox_KOG( CKTDGUIComboBox* pComboBox_, const CX2Item::ItemTemplet* pItemTemplet_ );

				void AddSocketIdListToComboBox( CKTDGUIComboBox* pComboBox_, const CX2Item::ItemTemplet* pItemTemplet_, const int iSocketItemGroupID_ );


				void SetOption( int comboBoxIndex );

				void SetCheck( bool bCheck );

				CX2PageSlot* Clone( int index, const WCHAR* pFileName ) { return new CashItemToInvenPopupSlot( index, pFileName ); }

			protected:

				KBillOrderInfo m_KBillOrderInfo;
				KBillPackageInfo m_KBillPackageInfo;
		};

		class BuyItemPopup : public CX2PageMgrItem
		{
			public:

				BuyItemPopup( const WCHAR* pFileName) :
				CX2PageMgrItem( pFileName )
				, m_bChoicedItemBuy( false )
				{
					m_iUseCashType = KGlobalCashInfo::GCT_PUBLISHER_CASH;// 미국, 일본 포함
				}

				~BuyItemPopup() {}


				void SetCheckAllItem( bool bCheck );
				void ChangeCheckItem( CKTDGUIDialogType pDialog, bool bCheck );


				void ChangeItemPeriod( CKTDGUIDialogType pDialog, int comboBoxIndex ); // 콤보박스에서 살 캐시 변경
				bool GetCheckedProductNoList(OUT vector<unsigned long>& vecProductNoList );
				void UpdateCashUI();

				void SetChoicedItemBuy( bool bCheck ) { m_bChoicedItemBuy = bCheck; }
				bool GetChoicedItemBuy() { return m_bChoicedItemBuy; }

//{{ 김상훈 2010.11.1
				bool RestCashCheck();
//}} 김상훈 2010.11.1

				void SetUseCashType(int iCashType)
				{
					m_iUseCashType = iCashType;
				}

				int GetUseCashType(){ return m_iUseCashType; }




			protected:

				void InitbyOpen();

				void ResetNowPage();

				vector<int>		m_vecCheckedItem;
				map< int, int > m_mapSelectedProductNo;		//itemID, CashData에 KNXBTProductInfo 벡터 인덱스
				bool			m_bChoicedItemBuy;
				int				m_iUseCashType;

		};

		class CashItemToInvenPopup : public CX2PageMgrItem
		{
		public:

			CashItemToInvenPopup( const WCHAR* pFileName) : CX2PageMgrItem( pFileName ) {}
			~CashItemToInvenPopup() {}

			void Reset( vector< int >& vecItemID, vector< KBillOrderInfo >& vecKBillOrderInfo,
						vector< KBillPackageInfo >& vecKBillPackageInfo );
			void ChangeItemOption( CKTDGUIDialogType pDialog, int comboBoxIndex ); // 콤보박스에서 옵션 정하자

			bool GetSelectedOptionListAndGroupID( OUT map<int, int>& mapSelectedOptionList_, OUT map<int, int>& mapOptionGroupIdList_ );

			std::pair<int, int> GetSelectedOptionList( const int iSocketItemGroupID_, const int iSelectedIndex_, const int iItemID_ );

			vector< KBillOrderInfo >&	GetOrderInfoList() { return m_vecKBillOrderInfo; }
			void GetCheckedPackageInfo( vector< KBillPackageInfo >& vecPackageInfo );

			void SetCheckAllItem( bool bCheck );
			void ChangeCheckItem( CKTDGUIDialogType pDialog, bool bCheck );

			bool IsEmptyCheckedItemList() { return m_vecCheckedItem.empty(); }

		protected:

			void InitbyOpen();
			void ResetNowPage();

			void GetPackageInfo( unsigned long ulTranNo, vector< KBillPackageInfo >& vecKBillPackageInfo );
			vector< KBillOrderInfo > m_vecKBillOrderInfo;
			vector< KBillPackageInfo > m_vecKBillPackageInfo;
			map< int, int > m_mapSelectedOption;		//itemID 와 옵션 번호.

			vector<int>		m_vecCheckedItem;
		};

		class PresentItemPopup
		{
			public:

				PresentItemPopup();
				~PresentItemPopup();

				void SetOpen( bool bOpen, int itemID );
				bool GetOpen();

				void UpdateCash();
				unsigned long GetSelectedProductNo();

				void SetSelectFriend( int index );
				wstring GetNickNameToPresent();
				wstring	GetPresentMsg();
				wstring GetPassword();

				void SetChoicedItem( bool bChoicedItem ) { m_bChoicedItem = bChoicedItem; }
				bool GetChoicedItem() { return m_bChoicedItem; }


				void SetUseCashType(int iCashType)
				{
					m_iUseCashType = iCashType;
				}

				int GetUseCashType(){ return m_iUseCashType; }
			protected:

				void InitUI();


				bool			m_bOpen;
				int				m_ItemID;
				unsigned long	m_SelectedProductNum;
				CKTDGUIDialogType	m_pDLGPresent;

				bool			m_bChoicedItem;
				int				m_iUseCashType;
		};

	public:

		CX2CashShop( CKTDXStage* pStage, const WCHAR* wszFileName );
		virtual ~CX2CashShop(void);

		bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		HRESULT OnFrameMove( double fTime, float fElapsedTime );

		bool UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
		bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		wstring GetSlotCashItemPeriodDesc( int itemTID );
		wstring GetSlotCashItemDesc( int itemTID );

		virtual wstring GetSlotItemDescExtra_RBtn( int itemTID, CX2Item* pItem );

		void MouseMove( D3DXVECTOR2 mousePos );

		void SetOpen( bool bOpen );
		bool GetOpen() { return m_bOpen; }

		void ResetUnitViewer( CX2Unit* pUnit );
		void ResetUnitViewerEquip();

		void UpdateCashUI();

		bool GetAutoOpen() { return m_bAutoOpen; }
		void SetAutoOpen( bool bCheck ) { m_bAutoOpen = bCheck; }

		CX2UnitViewerUI* GetUnitViewerUI() { return m_pUnitViewerUI; }


		bool Handler_EGS_GET_WISH_LIST_REQ();
		bool Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( int iPageIndex, int nItemPerPage );

		//{{ kimhc // 2009-08-10 // UI에서 캐시샵의 특정탭으로 바로 가기 위해 추가
		void	SetMenuTypeCallingCashShop( int	iMenuType ) { m_iMenuTypeCallingCashShop = iMenuType; } // 캐쉬샵을 부른 UI 메뉴의 타입
		//}} kimhc // 2009-08-10 // UI에서 캐시샵의 특정탭으로 바로 가기 위해 추가

		void AddDelayedMail( KEGS_RECEIVE_LETTER_NOT& kEvent );
		void PushDelayedMails();

		//{{ 오현빈 // 2012-03-14 // 길드매니져에서 사용 성공한다면, 아이템 삭제에 필요한 정보를 보내주기 위해 추가
		KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ* GetPurchasedCashItemInfo() { return &m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ; }
		//}} 오현빈 // 2012-03-14 // 길드매니져에서 사용 성공한다면, 아이템 삭제에 필요한 정보를 보내주기 위해 추가

		//필드 입장시, 해당 필드 내에 캐쉬샵 입장중인 유저에 대한 처리
		//입장 시점에 X2Game이 생성되어 있지 않기 때문에 임시 저장
		void SetEtnerCashShopUserList( std::vector< UidType >& EnterCashShopUserList_ ) { m_vecEnterCashShopUser.clear(); m_vecEnterCashShopUser = EnterCashShopUserList_; }
		void GetEnterCashShopUserList( OUT std::vector< UidType >& EnterCashShopUserList_ );
		void ClearEnterCashShopUserList(){m_vecEnterCashShopUser.clear();}


		// 해당 유닛의 전직 아이템을 찜하기 목록에서 삭제함.
		void RemoveUnitClassChangeItemInChoice( const CX2Unit::UNIT_CLASS eUnitClass_ );
		// 해당 유닛에 해당하는 전직 아이템 ID를 받아옴.
		int GetUnitClassChangeItemID( const CX2Unit::UNIT_CLASS eUnitClass_ );


	protected:

		void InitUI( CKTDXStage* pStage );

		void CategoryDataParsing( const WCHAR* pFileName );

		bool Handler_EGS_BUY_CASH_ITEM_REQ( vector< unsigned long >& vecProductNoList, bool bChoicedItemBuy = false );
		bool Handler_EGS_BUY_CASH_ITEM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		//////////////////////////////////////////////////////////////////////////
		// 		bool Handler_EBILL_BUY_PRODUCT_REQ( vector< int >& vecProductNoList, bool bChoicedItemBuy = false );
		// 		bool Handler_EBILL_BUY_PRODUCT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_BILL_INVENTORY_INQUIRY_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( KBillOrderInfo& kKBillOrderInfo, map< int, int >& mapOption, const map<int, int>& mapGroupdIdList_ );

		bool Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REQ();
		bool Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REP( KBillOrderInfo& kKBillOrderInfo, map< int, int >& mapOption, const map<int, int>& mapGroupdIdList_, vector< KBillPackageInfo >& vecPackageInfo );


		//////////////////////////////////////////////////////////////////////////

		bool Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ();
		bool Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		bool Handler_EGS_GET_WISH_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_MODIFY_WISH_LIST_REQ( bool bAdd, vector< int >& vecItemID );
		bool Handler_EGS_MODIFY_WISH_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_PRESENT_CASH_ITEM_REQ( const WCHAR* wszReceiverNickName, const WCHAR* wszMessage,
												const WCHAR* wszPassword, unsigned long ulProductNo, bool bChoicedItem = false );
		bool Handler_EGS_PRESENT_CASH_ITEM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		bool Handler_EGS_CHECK_PRESENT_CASH_INVENTORY_NOT( unsigned long ulOrderNo, unsigned long ulProductNo );

		bool Handler_EGS_EXPAND_CHAR_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

		void Send_EGS_VISIT_CASH_SHOP_NOT( const bool bOpen_ ) const;



		bool MouseUp(  D3DXVECTOR2 mousePos );
		bool MouseRButtonUp( D3DXVECTOR2 mousePos );
		bool MouseLRButtonUpEquip( CX2SlotItem* pItemSlot );

		void SetCashShopState( CASH_SHOP_STATE cashShopState, CASH_SHOP_CATEGORY cashShopCate = CSC_NONE );

		void ResetItemSlot( CX2Slot::SLOT_TYPE slotType, const vector<int>& vecItemID );
		void ResetDepositItemSlot( CX2Slot::SLOT_TYPE slotType, vector< KBillOrderInfo >& vecKBillOrderInfo );
		void DestroyItemSlotByType( const CX2Slot::SLOT_TYPE eSlotType_ );
		void ResetMainPageNumUI();
		void ResetSubPageNumUI();

		int GetItemID( CKTDGUIDialogType pDialog );

		void UnitHandling( double fTime, float fElapsedTime );
		void MyUniyHandling( double fTime, float fElapsedTime );
		void PetHandling( double fTime, float fElapsedTime );

		void UpdateDLGPetViewerUI();
		void ResetPetViewer(int iiPetID_ = -1, int iItemID = - 1, CX2ItemManager::PREVIEW_PET_TYPE ePreviewPetType_ = CX2ItemManager::PPT_PET);
		void PetPreview( int iItemID_ )	;
		bool IsInUnitClickReact( D3DXVECTOR2 mousePos );

		vector<int> GetItemByCategory( CASH_SHOP_CATEGORY categoryID );
		vector<int> GetItemByCategory( CASH_SHOP_CATEGORY categoryID, CASH_SHOP_SUB_CATEGORY subCategoryID );

		void ChangeCategory( CKTDGUIRadioButton* pRadioButton );
		void ChangeSubCategory( CKTDGUIRadioButton* pRadioButton );
		void ChangeSubCategory( CX2CashShop::CASH_SHOP_SUB_CATEGORY eSubCategory );

		static bool IsPossibleUsedByMyCharacter( int itemID );

		vector<int> GetHotItemList( vector<int>& vecItemID, int maxItemNum );

		bool CheckIsPossibleEqiupSlot( CX2Slot::SLOT_TYPE slotType );

		bool CheckPreEqipItem( int itemTID );
		void RemovePreEquipItemByEquipPos( int itemTID );
		bool InsertPreEquipItem( int itemTID, bool bForce = false );
 		void PreEquipping();

		void OnRevertItem( int iItemTID );
		void ResetNowEquipUI();
		void ClearNowEquipUI();
		void ResetNowEquipUIByInven( const CX2Unit::UnitData& kMyUnitData, const CX2Inventory& kInventory);
		void ResetNowEquipUIByShopEquip();
		void ResetSellCashItemByPreEquip();
		void UpdateShopEquipedPicture();
		void AttachShopEquipedPicture( CX2SlotItem* pItemSlot );
		void SetShowShopEquipedPicture( CX2SlotItem* pItemSlot, bool bShow );

		void AddChoicedItem( int itemID );
		void RemoveChoicedItem( int itemID );

		void SetEnableBuyEquippingItem( bool bEnable );
		bool IsPossiblePresent( int itemID );

		void ProcessBuyPopUpOk();

		//{{ kimhc // 2009-08-10 // 자신의 은행 회원 등급에 맞지 않으면 보이지 않도록 함
		bool IsPossibleUsedByMyBankMembership( int itemID ) const;
		//}} kimhc // 2009-08-10 // 자신의 은행 회원 등급에 맞지 않으면 보이지 않도록 함

		static int GetClassChangeItemLevel( int itemID );
		const wstring GetClassNameByClassChangeItem( int itemID );

		//{{ kimhc // 2009-10-06 // 자동결제 처리
		//}} kimhc // 2009-10-06 // 자동결제 처리

		static bool GetCanByeSkillNoteItem( int itemID );




		void SetOpenNexonCashChargePage( OUT string& strURL_, OUT RECT& rcWebPage_ );	// 넥슨 캐시 충전 웹페이지



		//{{ kimhc // 2011-08-10 // 채널링
		void SetOpenTooniCashChargePage( OUT string& strURL_, OUT RECT& rcWebPage_ );	// 투니 캐시 충전 웹페이지
		//}} kimhc // 2011-08-10 // 채널링

		//{{ 오현빈 // 2012-04-09 // 실명인증
		//}} 오현빈 // 2012-04-09 // 실명인증

		static int GetUnitClassChangeItemLevel( int itemID );
		const wstring GetClassNameByUnitClassChangeItem( int itemID );

		static bool GetCahBuyExpandQuickSlotItem( int itemID );


		void SetShowOtherUI( bool bShowCashop_ );

		void JobChangeDirectLink( int iEventID );

		static bool GetCahBuySharingBankItem( int itemID );


		void ShowInvenEquipUI( bool bVal_ );



		bool CheckBuyPossibleRelationItem( int iItemID );
		bool CheckBuyPossibleRelationItem( const vector<int>& vecItemIDList );

		bool ShowPopUpGuideByItemID( const int iItemID_ );


		bool					m_bOpen;
		CASH_SHOP_STATE			m_CashShopState;
		CASH_SHOP_CATEGORY		m_CashShopCategory;
		CASH_SHOP_SUB_CATEGORY	m_CashShopSubCategory;

		CX2UnitViewerUI*		m_pUnitViewerUI;
		CX2NPCUnitViewerUI*		m_pNPCViewerUI;
		int						m_iPrePetItemID;
		float				m_fRemainRotY;
		float				m_fMouseSensitivity;
		bool				m_bRotateReturn;

		D3DXVECTOR2			m_UnitClickPos;
		D3DXVECTOR2			m_UnitClickSize;

		CKTDGUIDialogType		m_pDLGBack;
		CKTDGUIDialogType		m_pDLGFront;

		CKTDGUIDialogType		m_pDLGMainPage;

		CKTDGUIDialogType		m_pDLGSubPage;
		CKTDGUIDialogType		m_pDLGSubPageMenu[CSC_END];		//서브 페이지에서 거시기에 맞게끔 메뉴 표시..
		CKTDGUIDialogType		m_pDLGPetPreview;
		bool				m_bAllCharItem;

		CashShopSlotManager* m_pMainNewSM;
		CashShopSlotManager* m_pMainHotSM;
		CashShopSlotManager* m_pMainRecommendSM;

		CashShopSlotManager* m_pSubSM;						//일반 페이지
		CashShopSlotManager* m_pSubSpeicialSM;

		BuyItemPopup*			m_pBuyItemPopUp;
		CashItemToInvenPopup*	m_pCashItemToInvenPopup;
		PresentItemPopup*		m_pPresentItemPopup;

		vector< KBillOrderInfo >	m_vecKBillOrderInfo;
		vector< KBillPackageInfo >	m_vecKBillPackageInfo;
		KBillOrderInfo				m_TempCashItemToInven;
		int m_iNowDepositPageIndex;
		int m_iReqDepositPageIndex;

		vector< CashShopCategory* > m_vecCashShopCateList;

		bool						m_bAutoOpen;


		CKTDGUIDialogType			m_pDLGCheckCashItemToInven;

		KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ			m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ;

		KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ;
		CKTDGUIDialogType			m_pDLGOverlapCheckCashItemToInven;

		vector<	int >			m_vecPreEquipItemID;


		vector< int >			m_vecChoicedItemID;
		CKTDGUIDialogType			m_pDLGChoicedItemMenu;
		int						m_ChoicedItemMenuItemID;

		bool					m_bSend_EGS_BUY_CASH_ITEM_REQ;

		KEGS_BUY_CASH_ITEM_REQ  m_ReservedKEGS_BUY_CASH_ITEM_REQ;


		//{{ kimhc // 2009-08-10 // 은행에서 캐시샵의 설치탭으로 바로 가기 위해 추가
		int						m_iMenuTypeCallingCashShop;		// 캐쉬샵을 부른 UI 메뉴의 타입
		bool					m_bCheckDialogWarningPlatinumMembership;	// 플래티넘 구매시 다이얼로그로 다시 한번 확인 해야하는가?
		//KNXBTProductInfo		m_KNXBTProductInfoForAutoPayment;			// 구입하기로 선택한 자동결제 상품
		//}} kimhc // 2009-08-10 // 은행에서 캐시샵의 설치탭으로 바로 가기 위해 추가



		bool						m_bUsePShopAgencyItem;

		std::vector< KEGS_RECEIVE_LETTER_NOT >	m_vecDelayedMail;

#ifdef UIRRL_CASH_SHOP
		bool						m_bCreateDialog;
#endif //UIRRL_CASH_SHOP

		//{{ mauntain : 김태환 [2012.06.12] 매일매일 선물 상자 정보 Dialog
		//}}


		//필드 입장시, 해당 필드 내에 캐쉬샵 입장중인 유저에 대한 처리
		//입장 시점에 X2Game이 생성되어 있지 않기 때문에 임시 저장
		std::vector< UidType >			m_vecEnterCashShopUser;

//{{ 2013.03.13 조효진	캐시충전 버튼을 누르면 웹페이지를 띄워줌
	std::wstring GetWstrConnectChargePage();
//}}

		bool						m_bShowInvenEquip;

		bool						m_bShowPartyUIOld;	// 캐쉬샵 열기전에 파티UI를 보여주고 있었나




};
