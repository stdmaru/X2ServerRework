//{{ kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업

#pragma once

namespace		_CONST_UI_ITEM_EXCHANGE_SHOP_
{
	const int	g_iNumOfSlotSet				=	5;		// 한페이지 당 슬롯셋트의 갯수

	const int	g_iNumOfSlotInSlotSet = 8; // 엘리시스

}
class CX2UIItemExchangeShop : public CX2ItemSlotManager
{
public:

	enum	ITEM_EXCHANGE_SHOP_CUSTOM_MSG
	{
		IESCM_EXIT					= 33000,
		IESCM_PUSH_PREV_PAGE,
		IESCM_PUSH_NEXT_PAGE,
		IESCM_FIRST_EXCHANGE_BUTTON,
		IESCM_SECOND_EXCHANGE_BUTTON,
		IESCM_THIRD_EXCHANGE_BUTTON,
		IESCM_FOURTH_EXCHANGE_BUTTON,
		IESCM_FIFTH_EXCHANGE_BUTTON,
		IESCM_OK,
		IESCM_CANCEL,
	};

	struct	SlotSet
	{
		CKTDGUIStatic*			m_pStaticSlotSet;
		CKTDGUIButton*			m_pButtonForExchange;
		bool					m_bUserHave;			// 교환 대상 아이템을 유저가 가지고 있는가?
		int						m_eType;				// invalid, all, random, select 종료
		int						m_iSrcItemID;			// 교환 대상 itemID

		//int						m_iStartIndexForExchangeData;				// 현재 슬롯셋트에 속하는 element의 m_vecItemExchangeData, CX2SlotItem 벡터 내에서의 시작위치
		//int						m_iEndIndexForExchangeData;					// 현재 슬롯셋트에 속하는 element의 m_vecItemExchangeData, CX2SlotItem 벡터 내에서의 마지막위치
		//int						m_iStartIndexForSlotList;					// 현재 슬롯셋트에 속하는 element의 m_vecItemExchangeData, CX2SlotItem 벡터 내에서의 시작위치
		//int						m_iEndIndexForSlotList;						// 현재 슬롯셋트에 속하는 element의 m_vecItemExchangeData, CX2SlotItem 벡터 내에서의 마지막위치

		SlotSet()
		{
			m_pStaticSlotSet						= NULL;
			m_pButtonForExchange					= NULL;
			m_bUserHave								= false;
			m_eType									= CX2ItemManager::IET_INVALID;
			m_iSrcItemID							= -1;
			/*m_iStartIndexForExchangeData			= 0;
			m_iEndIndexForExchangeData				= 0;
			m_iStartIndexForSlotList				= 0;
			m_iEndIndexForSlotList					= 0;*/
		}
	};

public:
	CX2UIItemExchangeShop( const CKTDXStage* const pNowStage, const WCHAR* const pFileName );
	virtual ~CX2UIItemExchangeShop();

	HRESULT				OnFrameMove( double fTime, float fElapsedTime );
	bool				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool				UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool				UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool				GetShow() const;
	void				SetShow( bool bShow );
	D3DXVECTOR2			GetDLGSize() const;
	void				SetPosition( D3DXVECTOR2 vec );
	void				SetLayer( X2_DIALOG_LAYER layer );
	void				SetHouseID( int iHouseID );
	virtual		bool	MouseDown( D3DXVECTOR2 mousePos );
	void				InvalidLastSrcItem();
	void				InvalidLastSelectExchange();
	void				PlayWarningSelectExchange();
	void				StopWarningSelectExchange();
	bool				SelectExchangeSrcItem( UidType iSrcItemUID );
	bool				CheckExchangeUseItem( int itemID );
	bool				Handler_EGS_ITEM_EXCHANGE_REQ( UidType iSrcItemUID, int iSrcItemID, int iSrcQuantity, int iSelectedItemID = 0);
	bool				Handler_EGS_ITEM_EXCHANGE_REQ() { return Handler_EGS_ITEM_EXCHANGE_REQ( m_iSrcItemUID, m_iSrcItemID, m_iSrcQuantity, m_iDestItemID ); }

private:
	// 핸들러
	bool				Handler_EGS_ITEM_EXCHANGE_REQ( int iSrcItemID, int iSelectedItemID = 0);
	bool				Handler_EGS_ITEM_EXCHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


private:
	// 멤버함수
	bool				InitItemExchangeData();
	bool				InitNowPageAndMaxPage();
	bool				UpdatePageString();
	bool				UpdatePageUI();
	bool				UpdateSlotSetUI( CKTDGUIStatic* pStaticSlotSet, CX2ItemManager::ITEM_EXCHANGE_TYPE eType );
	bool				UpdateSlotList();
	void				LostItemSlotList();
	//void				DestoryUIItemExchangeShop();
	int					SearchItemInUserInventory( int iItemID );	// 인벤토리에 해당 아이템이 없는 경우 -1 리턴
	bool				PushedExchangeButton( int iCustomMsg, bool bPopupCenter = false );

    const wchar_t*      GetItemFullName_( int iItemID );
	//	bool				CompareVector( std::vector< CX2ItemManager::ItemExchangeData >& vFirst, std::vector< CX2ItemManager::ItemExchangeData >& vSecond );

private:
	// 멤버변수
	bool								m_bShow;
	int									m_iHouseID;				// X2LocationManager::HOUSE_ID
	CKTDGUIDialogType						m_pDlgBackground;		// 아이템 교환 다이얼로그
	//CKTDGUIDialogType						m_pDlgSelectedSlot;		// m_pDlgSelectedItem은 mousemove에 사용되고, 이것은 mousedown에 사용됨
	D3DXVECTOR2							m_vSizeDlg;
	D3DXVECTOR2							m_vPosDlg;
	UINT								m_uiNowPage;			// 현재 보여지는 페이지 수
	UINT								m_uiMaxPage;			// 현재 보여줄 수 있는 최대 페이지 수
	UINT								m_uiSelectedSlotID;		// 선택된 슬롯의 ID
	int									m_iLastCustomMsg;
	int									m_iLastSrcItemID;
	UidType								m_iLastSelectSrcItemUID;
	CKTDGUIDialogType					m_pDLGWarningSelectExchange;
	CKTDGUIDialogType						m_pDlgExchangeConfirm;	// 교환 확인 다이얼로그

	SlotSet								m_SlotSet[_CONST_UI_ITEM_EXCHANGE_SHOP_::g_iNumOfSlotSet];

	//std::vector< CX2ItemManager::ItemExchangeData > m_vecTempItemExchangeData;
	std::vector< std::vector< CX2ItemManager::ItemExchangeData > > m_vecItemExchangeData;


	UidType		m_iSrcItemUID;
	int			m_iSrcItemID;
	int			m_iSrcQuantity;
	int			m_iDestItemID;
};

//}} kimhc // 2009-07-25 // 헤니르의 시공에 추가되는 ITEM_EXCHANGE_SHOP 작업
