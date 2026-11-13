#include "stdafx.h"
#include "./X2CashShop.h"


struct CashItemSorter
{
	bool operator()(const int& lhs, const int& rhs) const
	{
		int leftIndex = 0;
		int rightIndex = 0;
		for ( int i = 0; i < (int)g_pData->GetItemManager()->GetHotCashItemList().size(); i++ )
		{
			int itemID = g_pData->GetItemManager()->GetHotCashItemList()[i];
			if ( itemID == lhs )
			{
				leftIndex = i;
			}
			else if ( itemID == rhs )
			{
				rightIndex = i;
			}
			else if ( leftIndex != 0 && rightIndex != 0 )
			{
				break;
			}
		}

		if ( leftIndex < rightIndex )
			return true;
		else
			return false;
	}
};


CX2CashShop::CX2CashShop( CKTDXStage* pStage, const WCHAR* wszFileName )
: CX2ItemSlotManager( pStage, wszFileName )
//{{ kimhc // ���࿡�� ĳ�ü��� ��ġ������ �ٷ� ���� ���� �߰�
, m_pNPCViewerUI ( NULL )
, m_iPrePetItemID ( -1 )
//}} kimhc // ���࿡�� ĳ�ü��� ��ġ������ �ٷ� ���� ���� �߰�
, m_pDLGPetPreview ( NULL )
,m_iMenuTypeCallingCashShop( 0 )
,m_bCheckDialogWarningPlatinumMembership( false )
,m_bShowInvenEquip ( true )
{
	m_pUnitViewerUI		= NULL;
	CX2UnitViewerUI::SetUnitClicked( false );
	m_fMouseSensitivity = 0.01f;
	m_fRemainRotY		= 0.0f;
	m_bRotateReturn		= true;

	m_UnitClickPos		= D3DXVECTOR2(33,75);
	m_UnitClickSize		= D3DXVECTOR2(287,352);

#if !X2_DEV_TOOLS
	CategoryDataParsing( L"CashShopCategory.lua" );
#else
	CategoryDataParsing( L"CashShopCategory_Test.lua" );
#endif

	m_bOpen				= false;
	m_CashShopState		= CX2CashShop::CSS_MAIN;
	m_CashShopCategory	= CX2CashShop::CSC_NONE;
	m_CashShopSubCategory = CX2CashShop::CSSC_ALL;


	m_pDLGBack			= NULL;
	m_pDLGFront			= NULL;

	m_pDLGMainPage		= NULL;
	m_pDLGSubPage		= NULL;
	for ( CKTDGUIDialogType& pDLGSubPageMenu : m_pDLGSubPageMenu )
		pDLGSubPageMenu = NULL;

	m_bAllCharItem		= false;

	m_pMainNewSM		= NULL;
	m_pMainHotSM		= NULL;
	m_pMainRecommendSM	= NULL;

	m_pSubSM			= NULL;
	m_pSubSpeicialSM	= NULL;

	m_pBuyItemPopUp			= NULL;
	m_pCashItemToInvenPopup = NULL;
	m_pPresentItemPopup		= NULL;


	m_iNowDepositPageIndex = 1;
	m_iReqDepositPageIndex = 1;

	m_bAutoOpen = false;

	m_pDLGCheckCashItemToInven			= NULL;
	m_pDLGOverlapCheckCashItemToInven	= NULL;

	m_ChoicedItemMenuItemID = 0;
	m_pDLGChoicedItemMenu = NULL;

	m_bSend_EGS_BUY_CASH_ITEM_REQ = false;




	m_bUsePShopAgencyItem		= false;


#ifdef CASH_INVEN_PICKUP_ALL
	m_bPickUpAll = false;
#endif // CASH_INVEN_PICKUP_ALL

	InitUI( pStage );
	SetOpen( false );
}//CX2CashShop::CX2CashShop()

CX2CashShop::~CX2CashShop(void)
{
	SAFE_DELETE_KTDGOBJECT( m_pUnitViewerUI );
	SAFE_DELETE_DIALOG( m_pDLGBack );
	SAFE_DELETE_DIALOG( m_pDLGFront );
	SAFE_DELETE_DIALOG( m_pDLGMainPage );
	SAFE_DELETE_DIALOG( m_pDLGSubPage );

	SAFE_DELETE( m_pMainNewSM );
	SAFE_DELETE( m_pMainHotSM );
	SAFE_DELETE( m_pMainRecommendSM );

	SAFE_DELETE( m_pSubSM );
	SAFE_DELETE( m_pSubSpeicialSM );

	SAFE_DELETE( m_pBuyItemPopUp );
	SAFE_DELETE( m_pCashItemToInvenPopup );
	SAFE_DELETE( m_pPresentItemPopup );


	SAFE_DELETE_DIALOG( m_pDLGChoicedItemMenu);

	for ( int i = 0; i < (int)m_vecCashShopCateList.size(); i++ )
	{
		CX2CashShop::CashShopCategory* pCashShopCate = m_vecCashShopCateList[i];
		SAFE_DELETE( pCashShopCate );
	}
	m_vecCashShopCateList.clear();

	for ( int i = 0; i < CX2CashShop::CSC_END; i++)
	{
		CKTDGUIDialogType pDialog = m_pDLGSubPageMenu[i];
		SAFE_DELETE_DIALOG( pDialog );
		m_pDLGSubPageMenu[i] = NULL;
	}

//{{ mauntain : ����ȯ [2012.06.12] ���ϸ��� ���� ���� �ȳ� Dialog
	//}}


	PushDelayedMails();

	SAFE_DELETE_KTDGOBJECT( m_pNPCViewerUI );
	SAFE_DELETE_DIALOG( m_pDLGPetPreview );
}

bool CX2CashShop::MsgProc( HWND /*hWnd*/, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam )
{

	if ( GetCheckOperationCondition() == false )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
			if ( m_pUnitViewerUI != NULL )
			{
				D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();
				int degree = (int)D3DXToDegree( rot.y );
				m_fRemainRotY = D3DXToRadian( degree % 360 );
			}

			CX2UnitViewerUI::SetUnitClicked( false );

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );

			MouseUp( mousePos );


		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );


			if ( MouseDown( mousePos) == true )
				return true;

			if ( m_bOpen == true && IsInUnitClickReact( mousePos ) == true )
			{
				CX2UnitViewerUI::SetUnitClicked( true );
				return true;
			}
		}
		break;

	case WM_MOUSEMOVE:
		{

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			MouseMove( mousePos );
		}
		break;

	case WM_RBUTTONUP:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );

			SetNowDragItemReturn();

			return MouseRButtonUp( mousePos );
		}
		break;
	}

	return false;
}

HRESULT CX2CashShop::OnFrameMove( double fTime, float fElapsedTime )
{
	if ( m_bOpen == false )
		return S_OK;




	CX2ItemSlotManager::OnFrameMove( fTime, fElapsedTime );

	UnitHandling( fTime, fElapsedTime );

	return S_OK;
}

bool CX2CashShop::UICustomEventProc( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM wParam, LPARAM lParam )
{



	switch( wParam )
	{


	case CSCUM_CASH_CHARGE:
		{
			//{{ 2013.03.13 ��ȿ��	ĳ������ ��ư�� ������ ���������� �����
			if( NULL != g_pData && NULL != g_pData->GetMyUser() &&
				true == g_pMain->GetBrowserWrapper()->IsClosed() )
			{
				g_pKTDXApp->GetDevice()->SetDialogBoxMode( TRUE );

				//userID�� PurchaseToken ���� ������ ���⼭ ����
				std::wstring wstrURL = GetWstrConnectChargePage();

				{
					if( false == g_pMain->GetBrowserWrapper()->IsClosed() )
					{
						g_pMain->GetBrowserWrapper()->CloseBrowser();
					}

					RECT rcWebPage;
					GetWindowRect( g_pKTDXApp->GetHWND(), &rcWebPage );

					POINT ptCenter;
					ptCenter.x = rcWebPage.left + ( rcWebPage.right - rcWebPage.left ) / 2;
					ptCenter.y = rcWebPage.top + ( rcWebPage.bottom - rcWebPage.top ) / 2;

					// ���� ������ ũ�� �������� �װɷ� �ٽ� ����
					rcWebPage.left = ptCenter.x - 500;
					rcWebPage.right = ptCenter.x + 500;
					rcWebPage.top = ptCenter.y - 338;
					rcWebPage.bottom = ptCenter.y + 338;

					std::string strURL;
					ConvertWCHARToChar( strURL, wstrURL );
					g_pMain->GetBrowserWrapper()->Create( rcWebPage, g_pKTDXApp->GetHWND() );
					g_pMain->GetBrowserWrapper()->Navigate( strURL.c_str(), NULL );
				}
				//{{ 2010.12.03 ��ȿ��	���� ������ �ɽü����� �ñ�� �ϴ� �ڵ�.
				CX2StateMenu* pStateMenu = (CX2StateMenu*) g_pMain->GetNowState();
				if( NULL != pStateMenu )
				{
					pStateMenu->ToggleCashShop();
				}
				//}}
			}
			//}}

			return true;
		}
		break;
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	case CSCUM_BUY:
		{
			// Buy ��ư ������ ĳ�� üũ �ѹ� �� �Ѵ�.
			CX2State* pNowState = (CX2State*)g_pMain->GetNowState();
			if ( NULL != pNowState )
				pNowState->Handler_EGS_CHECK_BALANCE_REQ();


			m_pBuyItemPopUp->SetUseCashType((int)KGlobalCashInfo::GCT_PUBLISHER_CASH);


			//{{ kimhc // 2009-10-06 // �ڵ����� �ǿ��� ���Ź�ư Ŭ���� �ٸ� ó��
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				int itemID = GetItemID( pControl->GetDialog() );

				// �ش� Ŀ��/��ȥ �������� ���� �� �� ���� ��Ȳ�̶��
				if( true == CheckBuyPossibleRelationItem( itemID ) )
				{
					vector< int > vecItemID;
					if ( itemID != 0 )
						vecItemID.push_back( itemID );
					m_pBuyItemPopUp->Reset( vecItemID );
					m_pBuyItemPopUp->SetOpen( true );
					m_pBuyItemPopUp->SetChoicedItemBuy( false );


					if( GetClassChangeItemLevel( itemID ) > 0 ) // ���� ĳ�� �������̸�
					{
						std::wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_4518, "L", GetClassNameByClassChangeItem( itemID ) ) );
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), g_pMain->GetNowState() );
					}
					else if( true == g_pData->GetSkillTree()->IsUnsealSkillItemID( itemID ) )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4519 ), g_pMain->GetNowState() );
					}

				}

			}

			//{{ kimhc // 2009-10-06 // �ڵ����� ���� ��쿡 �ٸ� ó��

			return true;
		}
		break;

	case CSCUM_PRESENT:
		{

			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				int itemID = GetItemID( pControl->GetDialog() );


				if ( IsPossiblePresent( itemID ) == false )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_6 ), g_pMain->GetNowState() );
					return true;
				}

				if ( m_pPresentItemPopup != NULL )
				{
					m_pPresentItemPopup->SetOpen( true, itemID );
					m_pPresentItemPopup->SetChoicedItem( false );
				}
			}
		}
		break;

	case CSCUM_STORE:
		{



			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				int itemID = GetItemID( pControl->GetDialog() );
				if ( itemID != 0 )
				{
					AddChoicedItem( itemID );
				}
			}
			return true;

		}
		break;

	case CSCUM_MAIN_NEW_PREV_PAGE:
		{
			if ( m_pMainNewSM->PrevPage() == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_MAIN_NEW, m_pMainNewSM->GetNowShowItemIDList() );
				ResetMainPageNumUI();
			}
		}
		break;

	case CSCUM_MAIN_NEW_NEXT_PAGE:
		{
			if ( m_pMainNewSM->NextPage() == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_MAIN_NEW, m_pMainNewSM->GetNowShowItemIDList() );
				ResetMainPageNumUI();
			}
		}
		break;

	case CSCUM_MAIN_HOT_PREV_PAGE:
		{
			if ( m_pMainHotSM->PrevPage() == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_MAIN_HOT, m_pMainHotSM->GetNowShowItemIDList() );
				ResetMainPageNumUI();
			}
		}
		break;

	case CSCUM_MAIN_HOT_NEXT_PAGE:
		{
			if ( m_pMainHotSM->NextPage() == true )
			{

				ResetItemSlot( CX2Slot::ST_CASH_SHOP_MAIN_HOT, m_pMainHotSM->GetNowShowItemIDList() );
				ResetMainPageNumUI();
			}
		}
		break;

	case CSCUM_MAIN_RECOMMEND_PREV_PAGE:
		{
			if ( m_pMainRecommendSM->PrevPage() == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND, m_pMainRecommendSM->GetNowShowItemIDList() );
				ResetMainPageNumUI();
			}
		}
		break;

	case CSCUM_MAIN_RECOMMEND_NEXT_PAGE:
		{
			if ( m_pMainRecommendSM->NextPage() == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND, m_pMainRecommendSM->GetNowShowItemIDList() );
				ResetMainPageNumUI();
			}
		}
		break;

	case CSCUM_BUY_POP_UP_PREV_PAGE:
		{
			m_pBuyItemPopUp->PrevPage();
		}
		break;

	case CSCUM_BUY_POP_UP_NEXT_PAGE:
		{
			m_pBuyItemPopUp->NextPage();
		}
		break;

	case CSCUM_BUY_POP_UP_ALL_CHECK_CHANGED:
		{
			//m_pBuyItemPopUp->
			CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
			if ( pCheckBox != NULL )
				m_pBuyItemPopUp->SetCheckAllItem( pCheckBox->GetChecked() );

			return true;
		}
		break;

	case CSCUM_BUY_POP_UP_EACH_CHECK_CHANGED:
		{
			CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
			if ( pCheckBox != NULL )
				m_pBuyItemPopUp->ChangeCheckItem( pCheckBox->GetDialog(), pCheckBox->GetChecked() );

			return true;
		}
		break;

	case CSCUM_BUY_POP_UP_PRICE_CHANGED:
		{
			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)lParam;
			if ( pComboBox != NULL )
			{
				m_pBuyItemPopUp->ChangeItemPeriod( pComboBox->GetDialog(), pComboBox->GetSelectedItemIndex() );
			}

			return true;
		}
		break;

	case CSCUM_BUY_POP_UP_OK:
		{
			ProcessBuyPopUpOk();
			return true;
		}
		break;

	case CSCUM_BUY_POP_UP_CANCEL:
		{
			if( NULL != m_pBuyItemPopUp )
			{
				m_pBuyItemPopUp->SetOpen( false );
			}

			return true;
		}
		break;

	case CSCUM_CASH_ITEM_DEPOSIT_PREV_PAGE:
		{
			if ( m_iNowDepositPageIndex > 1 )
			{
				return Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( m_iNowDepositPageIndex - 1, CASH_SHOP_DEPOSIT_SLOT_NUM );
			}
		}
		break;

	case CSCUM_CASH_ITEM_DEPOSIT_NEXT_PAGE:
		{
			return Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( m_iNowDepositPageIndex + 1, CASH_SHOP_DEPOSIT_SLOT_NUM );
		}
		break;

	case CSCUM_CATEGORY_CHANGE:
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)lParam;
			ChangeCategory( pRadioButton );
			return true;
		}
		break;

	case CSCUM_SUB_CATEGORY_CHANGE:
		{
			CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)lParam;
			ChangeSubCategory( pRadioButton );
			return true;
		}
		break;

	case CSCUM_SUB_PREV_PAGE:
		{
			//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
			if ( m_pSubSM->PrevPage() == true )
			//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_SUB, m_pSubSM->GetNowShowItemIDList() );
				ResetSubPageNumUI();
			}
		}
		break;

	case CSCUM_SUB_NEXT_PAGE:
		{
			//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
			if ( m_pSubSM->NextPage() == true )
			//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_SUB, m_pSubSM->GetNowShowItemIDList() );
				ResetSubPageNumUI();
			}
		}
		break;

	case CSCUM_SUB_SEE_ALL_ITEM_OPTION_CHANGE:
		{
			CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
			if ( pCheckBox != NULL )
			{
				m_bAllCharItem = pCheckBox->GetChecked();
				vector< int > vecItemID;

				if ( m_CashShopSubCategory == CX2CashShop::CSSC_ALL )
				{
					vecItemID = GetItemByCategory( m_CashShopCategory );
				}
				else
				{
					vecItemID = GetItemByCategory( m_CashShopCategory, m_CashShopSubCategory );
				}

				//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
				m_pSubSM->Reset( vecItemID );
				m_pSubSM->SetShowPresentButton();
				//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
				ResetItemSlot(  CX2Slot::ST_CASH_SHOP_SUB,			m_pSubSM->GetNowShowItemIDList() );

				m_pSubSpeicialSM->Reset( GetHotItemList( vecItemID, CASH_SHOP_SUB_SPECIAL_SLOT_NUM ) );
				ResetItemSlot(  CX2Slot::ST_CASH_SHOP_SUB_SPECIAL,			m_pSubSpeicialSM->GetNowShowItemIDList() );

				ResetSubPageNumUI();
			}
		}
		break;

	case CSCUM_TAKE_OFF_ALL_ITEM:
		{
			if ( m_pUnitViewerUI !=	NULL )
				m_pUnitViewerUI->UpdateEqip( true );

			m_vecPreEquipItemID.resize(0);

			ResetNowEquipUI();
			SetEnableBuyEquippingItem( true );
			ResetPetViewer();
		}
		break;

	case CSCUM_ITEM_TO_INVEN_POP_UP_PREV_PAGE:
		{
			m_pCashItemToInvenPopup->PrevPage();
		}
		break;

	case CSCUM_ITEM_TO_INVEN_POP_UP_NEXT_PAGE:
		{
			m_pCashItemToInvenPopup->NextPage();
		}
		break;

	case CSCUM_ITEM_TO_INVEN_POP_UP_OPTION_CHANGED:
		{
			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)lParam;
			m_pCashItemToInvenPopup->ChangeItemOption( pComboBox->GetDialog(), pComboBox->GetSelectedItemIndex() );
		}
		break;

	case CSCUM_ITEM_TO_INVEN_POP_UP_OK:
		{
//////////////////////////////////////////////////////////////////////////
			vector< KBillOrderInfo > vecKBillOrderInfo;
			vecKBillOrderInfo = m_pCashItemToInvenPopup->GetOrderInfoList();
			if ( vecKBillOrderInfo.empty() == false )
			{
				KBillOrderInfo& kKBillOrderInfo = vecKBillOrderInfo[0];
				map< int, int > mapSelectedOptionList;

				map<int, int> mapOptionGroupIdList;		/// <SocketId, GroupdID>
				if ( m_pCashItemToInvenPopup->GetSelectedOptionListAndGroupID( mapSelectedOptionList, mapOptionGroupIdList ) == true )
				{
					m_pCashItemToInvenPopup->SetOpen( false );
					// ��Ű���� ���صǹǷ� �� ��쿡 ���� ó���� ���� �ʴ´�
					if ( kKBillOrderInfo.m_byteProductKind == 1 )
					{
						vector< KBillPackageInfo > vecKBillPackageInfo;
						m_pCashItemToInvenPopup->GetCheckedPackageInfo( vecKBillPackageInfo );
						if ( vecKBillPackageInfo.empty() == false )
						{
							Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REP( kKBillOrderInfo, mapSelectedOptionList, mapOptionGroupIdList, vecKBillPackageInfo );
						}
					}
					else

					{
						if ( m_pCashItemToInvenPopup->IsEmptyCheckedItemList() == false )
						{
							Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( kKBillOrderInfo, mapSelectedOptionList, mapOptionGroupIdList );
						}
					}
				}
				else
				{
					g_pMain->KTDGUIOKMsgBox(D3DXVECTOR2(250,300), GET_STRING( STR_ID_8 ), g_pMain->GetNowState() );
				}
			}
//////////////////////////////////////////////////////////////////////////
		}
		break;

	case CSCUM_ITEM_TO_INVEN_POP_UP_CANCEL:
		{
			m_pCashItemToInvenPopup->SetOpen( false );
		}
		break;

	case CSCUM_ITEM_TO_INVEN_CHECK_OK:
		{
			if ( m_pDLGCheckCashItemToInven != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCheckCashItemToInven, 0, false );

			m_pDLGCheckCashItemToInven = NULL;

			map< int, int > tempMap;
			//////////////////////////////////////////////////////////////////////////

			Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( m_TempCashItemToInven, tempMap, tempMap );

			//////////////////////////////////////////////////////////////////////////
		}
		break;

	case CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK:
		{
			if ( m_pDLGOverlapCheckCashItemToInven != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOverlapCheckCashItemToInven, 0, false );

			m_pDLGOverlapCheckCashItemToInven = NULL;

			Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REQ();
		}
		break;

	case CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK_PACKAGE:
		{
			if ( m_pDLGOverlapCheckCashItemToInven != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOverlapCheckCashItemToInven, 0, false );

			m_pDLGOverlapCheckCashItemToInven = NULL;

			Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ();
		}
		break;

	case CSCUM_BUY_ALL_EQUIPPING_ITEMS:
		{
			if ( m_vecPreEquipItemID.empty() == false )
			{
				if( false == CheckBuyPossibleRelationItem( m_vecPreEquipItemID ) )
					return false;
				m_pBuyItemPopUp->Reset( m_vecPreEquipItemID );
				m_pBuyItemPopUp->SetOpen( true );
				m_pBuyItemPopUp->SetChoicedItemBuy( false );
			}
			else
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_9 ), g_pMain->GetNowState() );
			}
		}
		break;

	case CSCUM_CHOICED_BUY:
		{

			vector< int > vecItemID;
			// ����� �ϴ� ĳ���������� ĳ�������� ����Ʈ�� ���ٸ�...
			CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( m_ChoicedItemMenuItemID );

			if(pCashItem == NULL)
			{
				return false;
			}
			if ( m_ChoicedItemMenuItemID != 0 )
				vecItemID.push_back( m_ChoicedItemMenuItemID );
			if( false == CheckBuyPossibleRelationItem( vecItemID ) )
				return false;
			m_pBuyItemPopUp->Reset( vecItemID );
			m_pBuyItemPopUp->SetOpen( true );
			m_pBuyItemPopUp->SetChoicedItemBuy( true );

			for( const int& iItemID : vecItemID )
			{
				if ( ShowPopUpGuideByItemID( iItemID ) )
					break;
			}

			if ( m_pDLGChoicedItemMenu != NULL )
				m_pDLGChoicedItemMenu->SetShowEnable( false, false );
		}
		break;

	case CSCUM_CHOICED_PRESENT:
		{
			if ( IsPossiblePresent( m_ChoicedItemMenuItemID ) == false )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_6 ), g_pMain->GetNowState() );
				return true;
			}

			if ( m_pPresentItemPopup != NULL )
			{
				m_pPresentItemPopup->SetOpen( true, m_ChoicedItemMenuItemID );
				m_pPresentItemPopup->SetChoicedItem( true );
			}

			if ( m_pDLGChoicedItemMenu != NULL )
				m_pDLGChoicedItemMenu->SetShowEnable( false, false );

			return true;
		}
		break;

	case CSCUM_CHOICED_REMOVE_IN_LIST:
		{
			RemoveChoicedItem( m_ChoicedItemMenuItemID );
			if ( m_pDLGChoicedItemMenu != NULL )
				m_pDLGChoicedItemMenu->SetShowEnable( false, false );
			return true;
		}
		break;

	case CSCUM_CHOICED_PRE_EQUIPPING:
		{
			InsertPreEquipItem( m_ChoicedItemMenuItemID );
			if ( m_pDLGChoicedItemMenu != NULL )
				m_pDLGChoicedItemMenu->SetShowEnable( false, false );

			ResetNowEquipUI();
			return true;
		}
		break;

	case CSCUM_CHOICED_CLOSE:
		{
			if ( m_pDLGChoicedItemMenu != NULL )
				m_pDLGChoicedItemMenu->SetShowEnable( false, false );

		}
		break;

	case CSCUM_CHOICED_BUY_ALL:
		{
			if ( m_vecChoicedItemID.empty() == false )
			{
				if( false == CheckBuyPossibleRelationItem( m_vecChoicedItemID ) )
					return false;
				m_pBuyItemPopUp->Reset( m_vecChoicedItemID );
				m_pBuyItemPopUp->SetOpen( true );
				m_pBuyItemPopUp->SetChoicedItemBuy( true );

				for( const int& iItemID : m_vecChoicedItemID )
				{
					if ( ShowPopUpGuideByItemID( iItemID ) )
						break;
				}

			}
			else
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_10 ), g_pMain->GetNowState() );
			}
		}
		break;

	case CSCUM_PRESENT_POP_UP_CHANGED_CASH_PRODUCT:
		{
			if ( m_pPresentItemPopup != NULL )
			{
				m_pPresentItemPopup->UpdateCash();
			}
		}
		break;

	case CSCUM_PRESENT_POP_UP_CAHNGED_FRIEND:
		{
			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)lParam;
			if ( m_pPresentItemPopup != NULL )
			{
				m_pPresentItemPopup->SetSelectFriend( pComboBox->GetSelectedItemIndex() );
			}
		}
		break;

	case CSCUM_PRESENT_OK:
		{
			if ( m_pPresentItemPopup != NULL )
			{
				unsigned long productNo = m_pPresentItemPopup->GetSelectedProductNo();
				if ( productNo <= 0 )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_11 ), g_pMain->GetNowState() );
					return true;
				}

				wstring tempNickName = m_pPresentItemPopup->GetNickNameToPresent();
				if ( tempNickName.empty() == true )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_12 ), g_pMain->GetNowState() );
					return true;
				}

				Handler_EGS_PRESENT_CASH_ITEM_REQ( m_pPresentItemPopup->GetNickNameToPresent().c_str(), m_pPresentItemPopup->GetPresentMsg().c_str(),
					m_pPresentItemPopup->GetPassword().c_str(), m_pPresentItemPopup->GetSelectedProductNo(), m_pPresentItemPopup->GetChoicedItem() );

				m_pPresentItemPopup->SetOpen( false, 0 );
			}
		}
		break;

	case CSCUM_PRESENT_CANCEL:
		{
			if ( m_pPresentItemPopup != NULL )
				m_pPresentItemPopup->SetOpen( false, 0 );
		}
		break;

	case CSCUM_BUY_POP_UP_OVERLAP_OK:
		{
			if( true == g_pMain->IsWaitingServerPacket(EGS_BUY_CASH_ITEM_ACK))
				break;

			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), 0, false );

			m_pBuyItemPopUp->SetOpen( false );

			g_pData->GetServerProtocol()->SendPacket( EGS_BUY_CASH_ITEM_REQ, m_ReservedKEGS_BUY_CASH_ITEM_REQ );
			g_pMain->AddServerPacket( EGS_BUY_CASH_ITEM_ACK );

			m_bSend_EGS_BUY_CASH_ITEM_REQ = true;

		}
		break;

	case CSCUM_BUY_POP_UP_OVERLAP_CANCEL:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), 0, false );

			m_pBuyItemPopUp->SetOpen( false );
		}
		break;

	case CSCUM_BUY_PLATINUM_CARD_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), 0, false );
			m_bCheckDialogWarningPlatinumMembership	= true;

			ProcessBuyPopUpOk();
		}
		break;

	case CSCUM_BUY_PLATINUM_CARD_CANCEL:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), 0, false );
			m_bCheckDialogWarningPlatinumMembership	= false;

			m_pBuyItemPopUp->SetOpen( false );
		}
		break;
		/*
		case CSCUM_BUY_POP_UP_ALL_CHECK_CHANGED:
		{
		//m_pBuyItemPopUp->
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
		if ( pCheckBox != NULL )
		m_pBuyItemPopUp->SetCheckAllItem( pCheckBox->GetChecked() );

		return true;
		}
		break;

		case CSCUM_BUY_POP_UP_EACH_CHECK_CHANGED:
		{
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
		if ( pCheckBox != NULL )
		m_pBuyItemPopUp->ChangeCheckItem( pCheckBox->GetDialog(), pCheckBox->GetChecked() );

		return true;
		}
		break;
		*/
	case CSCUM_ITEM_TO_INVEN_POP_UP_ALL_CHECK_CHANGED:
		{
			//m_pBuyItemPopUp->
			CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
			if ( pCheckBox != NULL )
				m_pCashItemToInvenPopup->SetCheckAllItem( pCheckBox->GetChecked() );

			return true;
		}
		break;

	case CSCUM_ITEM_TO_INVEN_POP_UP_EACH_CHECK_CHANGED:
		{
			CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)lParam;
			if ( pCheckBox != NULL )
				m_pCashItemToInvenPopup->ChangeCheckItem( pCheckBox->GetDialog(), pCheckBox->GetChecked() );

			return true;
		}
		break;
 	case CSCUM_USE_BONUS_SELECT:
 		{
 			m_pBuyItemPopUp->SetUseCashType( (int)KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT );
 			m_pBuyItemPopUp->UpdateCashUI();
 		}
 		break;
 	case CSCUM_PRESENT_USE_BONUS_SELECT:
 		{
			m_pPresentItemPopup->SetUseCashType( (int)KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT );
 			m_pPresentItemPopup->UpdateCash();
 		}
 		break;
 	case CSCUM_USE_GAMEPOINT_SELECT:
 		{
			m_pBuyItemPopUp->SetUseCashType( (int)KGlobalCashInfo::GCT_KOG_ELSWORD_CASH );
 			m_pBuyItemPopUp->UpdateCashUI();
 		}
 		break;
 	case CSCUM_PRESENT_USE_GAMEPOINT_SELECT:
 		{
			m_pPresentItemPopup->SetUseCashType( (int)KGlobalCashInfo::GCT_KOG_ELSWORD_CASH );
 			m_pPresentItemPopup->UpdateCash();
 		}
 		break;
//{{ mauntain : ����ȯ [2012.06.12] ���ϸ��� ���� ���� ���� Dialog �ݱ�
//}}

	case CSCUM_TAKE_OFF_ALL_ITEM_AND_INVEN_EQUIP_ITEM:
		{
			m_bShowInvenEquip = !m_bShowInvenEquip;
			m_pUnitViewerUI->SetShowInvenEquip( m_bShowInvenEquip );
			ShowInvenEquipUI( m_bShowInvenEquip );

			if ( m_pUnitViewerUI !=	NULL )
				m_pUnitViewerUI->UpdateEqip( true );

			ResetNowEquipUI();
			PreEquipping();
		} break;

#ifdef CASH_INVEN_PICKUP_ALL
	case CSCUM_PICKUP_ALL:
		{
			dbg::cout << L"����α�-�׽�Ʈ : ĳ�� �κ� ���� �������� ����" << dbg::endl;
			PickUpAll();
		}
		break;
#endif // CASH_INVEN_PICKUP_ALL

	}



	return false;
}

bool CX2CashShop::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	switch( wParam )
	{
	case EGS_BUY_CASH_ITEM_ACK:
		{
			Handler_EGS_BUY_CASH_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
// 	case EBILL_BUY_PRODUCT_ACK:
// 		{
// 			Handler_EBILL_BUY_PRODUCT_ACK( hWnd, uMsg, wParam, lParam );
// 		}
	case EGS_BILL_INVENTORY_INQUIRY_ACK:
		{
			Handler_EGS_BILL_INVENTORY_INQUIRY_ACK( hWnd, uMsg, wParam, lParam );
			return true;
		}
		break;

	case EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK:
		{
			Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK( hWnd, uMsg, wParam, lParam );
			return true;
		}
		break;
	case EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK:
		{
			return Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_GET_WISH_LIST_ACK:
		{
			return Handler_EGS_GET_WISH_LIST_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_MODIFY_WISH_LIST_ACK:
		{
			return Handler_EGS_MODIFY_WISH_LIST_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_PRESENT_CASH_ITEM_ACK:
		{
			return Handler_EGS_PRESENT_CASH_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_EXPAND_CHAR_SLOT_NOT:
		{
			return Handler_EGS_EXPAND_CHAR_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		} break;

		//{{ kimhc // 2009-10-06 // �ڵ����� ó��
	}

	return false;
}



wstring CX2CashShop::GetSlotCashItemPeriodDesc( int itemTID )
{
	wstring slotDesc = L"";

	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->GetItemUID() > 0 )
			return slotDesc;

		if ( m_pNowOverItemSlot->GetSlotType() != CX2Slot::ST_CASH_SHOP_DEPOSIT )
		{
			CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemTID );
			if ( pCashItem != NULL )
			{
				const KBillProductInfo* pKBillProductInfo = pCashItem->GetGateProduct();
				if ( pKBillProductInfo != NULL )
				{
					slotDesc = L"\n";
					for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
					{
						KBillProductInfo& kKBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
						bool TempBool = false;
						if( g_pData->GetItemManager()->CheckDisCountItemList( kKBillProductInfo.m_iProductNo, TempBool ) == false )
						{
							continue;
						}


						slotDesc += pCashItem->GetPeriod( kKBillProductInfo );
						slotDesc += L"\n";
					}
				}
			}
		}
		else if ( m_pNowOverItemSlot->GetSlotType() == CX2Slot::ST_CASH_SHOP_DEPOSIT )
		{
			//�Ⱓ ���ִ� �κ�...
			KBillOrderInfo& kKBillOrderInfo = m_pNowOverItemSlot->GetKBillOrderInfo();
			if ( (int)kKBillOrderInfo.m_kBillProductInfo.m_cPeriod != 0 )
			{
				slotDesc = L"\n";
				wstringstream wstrstm;
				wstrstm << GET_STRING( STR_ID_13 ) << L" : "<< (int)kKBillOrderInfo.m_kBillProductInfo.m_cPeriod << GET_STRING( STR_ID_14 );
				slotDesc += wstrstm.str().c_str();
				slotDesc += L"\n";
			}
		}
	}

	return slotDesc;
}

wstring CX2CashShop::GetSlotCashItemDesc( int itemTID )
{
	std::wstring slotItemDesc = L"";

	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->GetItemUID() > 0 )
			return slotItemDesc;
	}

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemTID );
	if ( pItemTemplet != NULL )
	{
		slotItemDesc += L"\n";
		slotItemDesc += pItemTemplet->GetDescriptionInShop();
	}

	//** ���� ����
	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->GetKBillOrderInfo().m_byteType == KBillOrderInfo::BCIT_GIFT )
		{
			wstring tempName = m_pNowOverItemSlot->GetKBillOrderInfo().m_wstrSenderNickName;

			if( tempName.empty() == false )
			{
				//tempName = tempName.substr( 1, tempName.find( L"]") -1 );
				slotItemDesc += L"\n";
				slotItemDesc += GET_REPLACED_STRING( ( STR_ID_15, "L", tempName ) );
				slotItemDesc += L"\n";
			}

			wstring tempMsg = m_pNowOverItemSlot->GetKBillOrderInfo().m_wstrSenderPresentMessage;

			if( tempMsg.empty() == false )
			{
				//tempMsg = tempMsg.substr( tempMsg.find( L"]") + 2, (int)tempMsg.size() );

				slotItemDesc += GET_REPLACED_STRING( ( STR_ID_16, "L", tempMsg ) );
			}
		}
	}

	return slotItemDesc;
}

/*virtual*/ wstring CX2CashShop::GetSlotItemDescExtra_RBtn( int itemTID, CX2Item* pItem )
{
	std::wstring slotItemDesc = L"";

	if ( m_bShowRBDownMessage == true )
	{
		if ( m_pNowOverItemSlot != NULL )
		{


			if ( m_pNowOverItemSlot->GetSlotType() == CX2Slot::ST_CASH_SHOP_DEPOSIT )
			{
				slotItemDesc += L"\n";
				slotItemDesc += GET_STRING( STR_ID_17 );
			}
			else if ( m_pNowOverItemSlot->GetSlotType() == CX2Slot::ST_CASH_SHOP_CHOICED )
			{
				slotItemDesc += L"\n";
				slotItemDesc += GET_STRING( STR_ID_18 );
			}
			else
			{

				if( NULL != pItem
					&& true == pItem->IsDisabled()
					&& false == pItem->GetIsEvaluation()
					)
				{
					slotItemDesc = L"";
				}
				else
				{
					const CX2Item::ItemTemplet* pkItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemTID );
					if( pkItemTemplet != NULL )
					{
                        switch( pkItemTemplet->GetItemType() )
                        {
                        case CX2Item::IT_WEAPON:
                        case CX2Item::IT_DEFENCE:
                        case CX2Item::IT_ACCESSORY:
                        case CX2Item::IT_QICK_SLOT:
						    {
							    slotItemDesc += L"\n";
							    slotItemDesc += GET_STRING( STR_ID_19 );
						    }
                        default:
                        	break;
                        }//switch
					}
				}
			}
		}
	}

	return slotItemDesc;
}

void CX2CashShop::MouseMove( D3DXVECTOR2 mousePos )
{
	m_NowMousePos = mousePos;

	bool bCheck = false;
	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(i);
		if ( pItemSlot == NULL || pItemSlot->GetEnable() == false )
			continue;

		if ( pItemSlot->IsInMousePoint( mousePos ) == true )
		{
			SetNowOverItemSlotAndDesc( pItemSlot );
			bCheck = true;

			if ( m_pNowOverItemSlot != NULL &&
				m_pNowOverItemSlot->GetSlotType() == CX2Slot::ST_CASH_SHOP_DEPOSIT )
			{
			}

			break;
		}
	}

	if ( bCheck == false )
	{
		SetNowOverItemSlotAndDesc( NULL );
	}

	CX2SlotManager::MouseMove( mousePos );
}

void CX2CashShop::SetOpen( bool bOpen )
{
	if( bOpen == false )
	{
		PushDelayedMails();
	}

	g_pMain->GetInformerManager()->InvalidInfo();

	m_bOpen = bOpen;

	if( NULL != g_pX2Game && g_pX2Game->GetMyUnit() )
		g_pX2Game->GetMyUnit()->SetEnterCashShop( bOpen );

	if ( m_pDLGBack != NULL )
		m_pDLGBack->SetShowEnable( bOpen, bOpen );

	if ( m_pDLGFront != NULL )
		m_pDLGFront->SetShowEnable( bOpen, bOpen );

	if ( m_pUnitViewerUI != NULL )
		m_pUnitViewerUI->SetShowObject( bOpen );


	Send_EGS_VISIT_CASH_SHOP_NOT( bOpen );

	if ( bOpen == true )
	{

		m_bShowInvenEquip = true;
		if ( m_pUnitViewerUI != NULL )
			m_pUnitViewerUI->SetShowInvenEquip ( m_bShowInvenEquip );

		ShowInvenEquipUI ( m_bShowInvenEquip );
		ResetNowEquipUI();

		// ������ // 2013-04-11
		// �Ʒ� �������� m_pUnitViewerUI->UpdateEqip( true ); ���� �ʱ�ȭ �ϰ� ������ �ѹ� �� ���ִ� ����
		// ����� UID�� �ƹ�Ÿ���� �޹�ȣ �� ���, ������ ���� ��� ������.
		// �ƹ�Ÿ ������ ������ �� �� �� �����ϸ�, ��� ���� �ٽ� �ƹ�Ÿ�� ������ ����.
		//
		// ���� ������ ����� CX2UnitViewerUI::AddEqip()���� ó���Ǵµ� �ش� �Լ���
		// UpdateEquip()�̶�� ���������� ���̴� �Լ����� ó���ǰ� �����Ƿ�, ���� ����ó�� ������ �ʰ�
		// �� ������ ������ �ι� �ϵ��� ó����.
		PreEquipping();

		m_bSend_EGS_BUY_CASH_ITEM_REQ = false;

		if ( m_pUnitViewerUI != NULL )
		{
			m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVVMT_DOWN_LANDING );
			m_pUnitViewerUI->UpdateEqip( true );
			m_vecPreEquipItemID.clear();
		}

		//{{ kimhc // 2009-08-10 // ����
		// �÷�Ƽ�� ȸ���� ���Ž� ���Ǵ� bool �� �ʱ�ȭ
		m_bCheckDialogWarningPlatinumMembership	= false;
		// ���࿡�� ĳ�ü��� ��ġ������ �ٷ� ���� ���� �߰�
		switch ( m_iMenuTypeCallingCashShop )
		{
		case CX2UIManager::UI_MENU_PRIVATE_BANK:
		case CX2UIManager::UI_SKILL_SLOT:
		case CX2UIManager::UI_MENU_QUICK_SLOT:
			{
				SetCashShopState( CX2CashShop::CSS_SUB, CX2CashShop::CSC_INSTALL );
				CKTDGUIRadioButton* pRadioButton = NULL;
				pRadioButton = static_cast< CKTDGUIRadioButton* >( m_pDLGFront->GetControl( L"tab_Setup" ) );

				if ( pRadioButton != NULL )
				{
					pRadioButton->SetChecked( true );
				}

			}
			break;
		case CX2UIManager::UI_SKILL_NOTE:
			{
				// # �޸� ���Ÿ� ���� ī�װ����� �̵�
				SetCashShopState( CX2CashShop::CSS_SUB, CX2CashShop::CSC_CONSUME );
				CKTDGUIRadioButton* pRadioButton = static_cast< CKTDGUIRadioButton* >( m_pDLGFront->GetControl( L"tab_Use" ) );
				if ( NULL != pRadioButton )
					pRadioButton->SetChecked( true );

				if( NULL != m_pDLGSubPageMenu[CSC_CONSUME] )
				{
					ChangeSubCategory( CSSC_CONSUME_ETC );
					pRadioButton = static_cast< CKTDGUIRadioButton* >(m_pDLGSubPageMenu[CSC_CONSUME]->GetControl(L"Tab_Other"));
					if( NULL != pRadioButton )
						pRadioButton->SetChecked(true);
				}
			} break;

		case CX2LVUpEventMgr::LUEMCUM_ELSWORD_SWORD_KNIGHT:
		case CX2LVUpEventMgr::LUEMCUM_ELSWORD_MAGIC_KNIGHT:
		case CX2LVUpEventMgr::LUEMCUM_AISHA_HIGH_MAGICIAN:
		case CX2LVUpEventMgr::LUEMCUM_AISHA_DARK_MAGICIAN:
		case CX2LVUpEventMgr::LUEMCUM_RENA_SNIPING_RANGER:
		case CX2LVUpEventMgr::LUEMCUM_RENA_COMBAT_RANGER:
		case CX2LVUpEventMgr::LUEMCUM_RAVEN_SWORD_TAKER:
		case CX2LVUpEventMgr::LUEMCUM_RAVEN_OVER_TAKER:
		case CX2LVUpEventMgr::LUEMCUM_EVE_CODE_EXOTIC:
		case CX2LVUpEventMgr::LUEMCUM_EVE_CODE_ARCHITECTURE:
		case CX2LVUpEventMgr::LUEMCUM_CHUNG_FURY_GUARDIAN:
		case CX2LVUpEventMgr::LUEMCUM_CHUNG_SHOOTING_GUARDIAN:
			//	1�� �߰� ����
		case CX2LVUpEventMgr::LUEMCUM_ELSWORD_SHEATH_KNIGHT:
		case CX2LVUpEventMgr::LUEMCUM_ARME_BATTLE_MAGICIAN:
		case CX2LVUpEventMgr::LUEMCUM_LIRE_TRAPPING_RANGER:
		case CX2LVUpEventMgr::LUEMCUM_RAVEN_WEAPON_TAKER:
		case CX2LVUpEventMgr::LUEMCUM_EVE_ELECTRA:
		case CX2LVUpEventMgr::LUEMCUM_CHUNG_SHELLING_GUARDIAN:
		case CX2LVUpEventMgr::LUEMCUM_ARA_LITTLE_HSIEN:
		case CX2LVUpEventMgr::LUEMCUM_ARA_LITTLE_DEVIL:
		case CX2LVUpEventMgr::LUEMCUM_ELESIS_SABER_KNIGHT:
		case CX2LVUpEventMgr::LUEMCUM_ELESIS_PYRO_KNIGHT:
			{
				JobChangeDirectLink( m_iMenuTypeCallingCashShop );
			} break;

		default:
			{
				SetCashShopState( CX2CashShop::CSS_MAIN );
			}
			break;
		}

		m_iMenuTypeCallingCashShop	= 0;	// �ٽ� �ʱ�ȭ
		//}} kimhc // 2009-08-10 // ����


		UpdateCashUI();
		ResetNowEquipUI();

		SetEnableBuyEquippingItem( true );

		DestroyItemSlotByType( CX2Slot::ST_CASH_SHOP_DEPOSIT );
		Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( 1, CASH_SHOP_DEPOSIT_SLOT_NUM );

		//{{ 09.03.10 ���¿�
		// ��Ƽâ�� ����â�� ������ ���ش�
		if( g_pMain->GetPartyUI() != NULL )
		{
			g_pMain->GetPartyUI()->OpenPartyMenu( false );
			g_pMain->GetPartyUI()->OpenPartyDLG( false );
//			g_pMain->GetPartyUI()->OpenPartyMemberDLG( false );

			//{{ ����� : [2009/10/21] //	�ǹ�â �߰�
			g_pMain->GetPartyUI()->OpenPartyFeverDLG( false );

			//{{ ����� : [2011/3/24/] //	���� �̼�
			//	����̼� ���� â ����
			//g_pData->GetWorldMissionManager()->GetUIWorldMission()->SetShowEXPBonusDlg( false );
			g_pData->GetWorldMissionManager()->GetUIWorldMission()->SetShowTimeDlg( false );
			//}} ����� : [2011/3/24/] //	���� �̼�
			g_pData->GetGlobalMissionManager()->GetUIGlobalMission()->SetShowGlobalMissionDlg( false );
		}

		if(g_pData->GetUIManager()->GetUIQuestNew() != NULL)
		{
			g_pData->GetUIManager()->GetUIQuestNew()->SetShowQuickQuestDLG(false);
		}
		//}}

		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_VILLAGE_MAP:
			{
				if( g_pChatBox->GetOpenChatBoxButton() == false )
				{
					g_pChatBox->SetChatBoxLayerUp(true);
					g_pChatBox->HideChatWindowFrame();
					g_pChatBox->HideChatWindow();
					g_pChatBox->HideChatEditBox();
					g_pChatBox->CloseChatWindow();
					g_pChatBox->OpenChatWindowButton();
				}

				if( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetNpcIndicator() )
				{
					g_pTFieldGame->GetNpcIndicator()->SetShow(false);
				}
			} break;
		case CX2Main::XS_BATTLE_FIELD:
			{
				if( NULL != g_pX2Game )
				{

					CX2BattleFieldGame* pBattleFieldGame = static_cast<CX2BattleFieldGame*>(g_pX2Game);
					if( NULL != pBattleFieldGame ->GetNpcIndicator() )
						pBattleFieldGame->GetNpcIndicator()->SetShow(false);
					if( NULL != pBattleFieldGame->GetMajorParticle() )
						pBattleFieldGame->GetMajorParticle()->SetRenderEffectMadeByGameUnit( false );

					if( NULL != pBattleFieldGame->GetMinorParticle() )
						pBattleFieldGame->GetMinorParticle()->SetRenderEffectMadeByGameUnit( false );

					if( NULL != pBattleFieldGame->GetMajorXMeshPlayer() )
						pBattleFieldGame->GetMajorXMeshPlayer()->SetRenderEffectMadeByGameUnit( false );

					if( NULL != pBattleFieldGame->GetMinorXMeshPlayer() )
						pBattleFieldGame->GetMinorXMeshPlayer()->SetRenderEffectMadeByGameUnit( false );
				}
			} break;
		default:
			break;
		}

		g_pKTDXApp->SkipFrame();
		UpdateDLGPetViewerUI();



	}
	else
	{
		SetEnable( false );
		SetShow( false );

		m_pDLGSelectedItem->SetShow( false );
		SetNowOverItemSlotAndDesc( NULL );
		InvalidSlotDesc();

		if ( m_pDLGMainPage != NULL )
			m_pDLGMainPage->SetShowEnable( false, false );

		if ( m_pDLGSubPage != NULL )
			m_pDLGSubPage->SetShowEnable( false, false );

		for ( int i = 0; i < CX2CashShop::CSC_END; i++ )
		{
			CKTDGUIDialogType pDialog = m_pDLGSubPageMenu[i];
			if ( pDialog != NULL )
				pDialog->SetShowEnable( false, false );
		}

		m_pCashItemToInvenPopup->SetOpen( false );
		m_pBuyItemPopUp->SetOpen( false );
		m_pPresentItemPopup->SetOpen( false, 0 );
		m_pMainNewSM->SetOpen( false );
		m_pMainHotSM->SetOpen( false );
		m_pMainRecommendSM->SetOpen( false );
		m_pSubSM->SetOpen( false );
		m_pSubSpeicialSM->SetOpen( false );

		if ( m_pDLGChoicedItemMenu != NULL )
			m_pDLGChoicedItemMenu->SetShowEnable( false, false );

		g_pMain->GetPartyUI()->OpenPartyMenu( true );

		//{{ 09.03.10 ���¿�
		// ��Ƽâ�� ����â�� ���ش�
		if( true == g_pData->GetPartyManager()->DoIHaveParty() ||
			true == g_pData->GetLocationManager()->IsDungeonGate( g_pData->GetLocationManager()->GetCurrentVillageID() )
			|| g_pData->GetPartyManager()->GetProcessPvpMatch() == true
			)
		{
			//{{ ����� : [2009/10/21] //	Feverâ �߰�
			g_pMain->GetPartyUI()->OpenPartyFeverDLG( true );
			g_pMain->GetPartyUI()->UpdatePartyFeverDLG();
		}

		//NPC�� �����߿� ĳ�Ø� ���� ������ ������Ʈ ��� �����°� ����
			//{{ ����� : [2011/3/24/] //	���� �̼�
			if( g_pData->GetWorldMissionManager()->IsActiveDefenceDungeon() == true )
			{
				g_pData->GetWorldMissionManager()->GetUIWorldMission()->SetShowTimeDlg( true );
			}
			//}} ����� : [2011/3/24/] //	���� �̼�
		if( g_pData->GetGlobalMissionManager()->IsActive() == true )
		{
			if( g_pData->GetGlobalMissionManager()->CheckShowGlobalMissionUI() == true )
			{
				g_pData->GetGlobalMissionManager()->GetUIGlobalMission()->SetShowGlobalMissionDlg( true );
			}
		}

		if(g_pData->GetUIManager()->GetUIQuestNew() != NULL)
		{
			g_pData->GetUIManager()->GetUIQuestNew()->SetShowQuickQuestDLG(true);
		}
			//NPC�� �����߿� ĳ�Ø� ���� ������ ������Ʈ ��� �����°� ����
		if( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
		{
			if( (g_pSquareGame != NULL) ||	// �ŷ������̰ų�
				(g_pTFieldGame != NULL && g_pTFieldGame->GetJoinNpc() == false )	// npc�� ������°� �ƴϰ� ��ųƮ���� �������� ������

				 )
			{
				// ä��UI ������ �°� �ʱ�ȭ �Ѵ�.
				g_pChatBox->SetChatBoxLayerUp(false);
				g_pChatBox->OpenChatWindow();
				g_pChatBox->CloseChatWindowButton();
				g_pChatBox->HideChatEditBox();
			}
		}

		//}}

		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_VILLAGE_MAP:
			{
				if( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetNpcIndicator() )
				{
					g_pTFieldGame->GetNpcIndicator()->SetShow(true);
				}
			} break;
		case CX2Main::XS_BATTLE_FIELD:
			{
				if( NULL != g_pX2Game )
				{

					CX2BattleFieldGame* pBattleFieldGame = static_cast<CX2BattleFieldGame*>(g_pX2Game);
					if( NULL != pBattleFieldGame ->GetNpcIndicator() )
						pBattleFieldGame->GetNpcIndicator()->SetShow(true);

					if( NULL != pBattleFieldGame->GetMajorParticle() )
						pBattleFieldGame->GetMajorParticle()->SetRenderEffectMadeByGameUnit( true );

					if( NULL != pBattleFieldGame->GetMinorParticle() )
						pBattleFieldGame->GetMinorParticle()->SetRenderEffectMadeByGameUnit( true );

					if( NULL != pBattleFieldGame->GetMajorXMeshPlayer() )
						pBattleFieldGame->GetMajorXMeshPlayer()->SetRenderEffectMadeByGameUnit( true );

					if( NULL != pBattleFieldGame->GetMinorXMeshPlayer() )
						pBattleFieldGame->GetMinorXMeshPlayer()->SetRenderEffectMadeByGameUnit( true );
				}
			} break;
		default:
			break;
		}

		ResetPetViewer();


	}




	// note!! CX2CashShop�� cx2statemenu�� ��ӹ��� Ŭ���������� ���ȴٰ� ����

	if( g_pMain->IsInheritStateMenu() )
	{
		CX2StateMenu* pStateMenu = (CX2StateMenu*) g_pMain->GetNowState();
		if( NULL != pStateMenu )
		{
			if( true == m_bOpen )
			{
				pStateMenu->SetShowMouseClickUI( false, CX2State::MouseClickUI::HR_CASH_SHOP );
			}
			else
			{
				pStateMenu->SetShowMouseClickUI( true, CX2State::MouseClickUI::HR_CASH_SHOP );
			}
		}
	}
	SetShowOtherUI( bOpen );


}

void CX2CashShop::ResetUnitViewer( CX2Unit* pUnit )
{
	m_vecPreEquipItemID.resize(0);

	if ( m_pUnitViewerUI == NULL )
	{
		m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
		//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
		//m_pUnitViewerUI->SetLightPos( 300, 300, -500 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
		//m_pUnitViewerUI->SetLightPos( -250000, 50000, -500000 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
		m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����

		g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

		m_pUnitViewerUI->SetUnit( pUnit, CX2UnitViewerUI::UVS_CASHSHOP );
		m_pUnitViewerUI->GetMatrix().Move(  D3DXVECTOR3( -250 -105, -60 + 39 + 20 -5000, 200 ) );
		m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.5f, 1.5f, 1.5f ) );
		m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
		m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );

		m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
	}
	else
	{
		m_pUnitViewerUI->SetUnit( pUnit, CX2UnitViewerUI::UVS_CASHSHOP );
	}



	if ( m_pUnitViewerUI != NULL )
	{
		m_pUnitViewerUI->SetShowObject( m_bOpen, true );
	}
}


void CX2CashShop::ResetUnitViewerEquip()
{
	m_vecPreEquipItemID.resize(0);

	if ( m_pUnitViewerUI != NULL )
	{
		m_pUnitViewerUI->UpdateEqip( true );
		m_pUnitViewerUI->SetShowObject( m_pUnitViewerUI->GetShowObject(), true );
	}
}


void CX2CashShop::UpdateCashUI()
{
	if ( m_pDLGFront != NULL )
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDLGFront->GetControl( L"g_pStaticcash_num" );
		if ( pStatic != NULL )
		{
			pStatic->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalMainCash() ).c_str() );
		}
	}

	if ( m_pBuyItemPopUp != NULL )
		m_pBuyItemPopUp->UpdateCashUI();

	if ( m_pPresentItemPopup != NULL )
		m_pPresentItemPopup->UpdateCash();
}

bool CX2CashShop::Handler_EGS_GET_WISH_LIST_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_GET_WISH_LIST_REQ );
	g_pMain->AddServerPacket( EGS_GET_WISH_LIST_ACK );

	return true;
}

bool CX2CashShop::Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( int iPageIndex, int nItemPerPage )
{
	m_iReqDepositPageIndex = iPageIndex;

	KEGS_BILL_INVENTORY_INQUIRY_REQ kPacket;
	kPacket.m_iCurrentPage = iPageIndex;
	kPacket.m_nItemPerPage = nItemPerPage;

	g_pData->GetServerProtocol()->SendPacket( EGS_BILL_INVENTORY_INQUIRY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_BILL_INVENTORY_INQUIRY_ACK );

	return true;
}


void CX2CashShop::InitUI( CKTDXStage* pStage )
{
	m_pDLGFront			= new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Right_Front.lua" );

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGFront );

	//{{ kimhc // 2009-10-08 // �ڵ������� define������ ������ �ڵ����� ���� ������ �ʵ���!!
	if ( m_pDLGFront->GetRadioButton_LUA( "tab_Auto_Account" ) != NULL )
		m_pDLGFront->GetRadioButton_LUA( "tab_Auto_Account" )->SetShowEnable( false, false );
	//}} kimhc // 2009-10-08 // �ڵ�����

	if ( m_pDLGFront->GetRadioButton_LUA( "tab_Event_2" ) != NULL )
		m_pDLGFront->GetRadioButton_LUA( "tab_Event_2" )->SetShowEnable( false, false );

	CKTDGUIControl* pRefund = m_pDLGFront->GetControl( L"refund" );

	if( pRefund != NULL )
	{
		pRefund->SetShowEnable( false , false );
	}

	m_pDLGMainPage		= new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Mainpage.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMainPage );

#ifdef CASH_INVEN_PICKUP_ALL
	// ���ڸ� ��ư ���̰�
	CKTDGUIButton* pButtonPickUpAll = (CKTDGUIButton*)m_pDLGMainPage->GetControl( L"PickUpAll" );
	if( pButtonPickUpAll != NULL )
		pButtonPickUpAll->SetShowEnable( true, true );
#endif // CASH_INVEN_PICKUP_ALL

	m_pDLGSubPage		= new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGSubPage );

	for ( int i = 0; i < CX2CashShop::CSC_END; i++ )
	{
		CKTDGUIDialogType pDialog = NULL;
		CX2CashShop::CASH_SHOP_CATEGORY cashShopCate = (CX2CashShop::CASH_SHOP_CATEGORY)i;

		switch( cashShopCate )
		{
		case CX2CashShop::CSC_FASHION:
			{
				pDialog = new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage_Fashion.lua" );
			}
			break;

		case CX2CashShop::CSC_ACCESSORY:
			{
				pDialog = new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage_Accessory.lua" );
			}
			break;

		case CX2CashShop::CSC_CONSUME:
			{
				pDialog = new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage_Consume.lua" );
			}
			break;

		case CX2CashShop::CSC_INSTALL:
			{
				pDialog = new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage_Install.lua" );
			}
			break;

		case CX2CashShop::CSC_PET:
			{
				pDialog = new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage_Pet.lua" );


			}
			break;

		case CX2CashShop::CSC_EVENT:
			{
				pDialog = new CKTDGUIDialog( pStage, L"DLG_Cash_Shop_Subpage_Event.lua" );
			}
			break;

			//{{ kimhc // 2009-09-29 // �ڵ����� ī�װ��� �߰�
				//}} kimhc // 2009-09-29 // �ڵ����� ī�װ��� �߰�

		default:
			break;
		}

		if ( pDialog != NULL )
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );

		m_pDLGSubPageMenu[i] = pDialog;
	}

	/*
	vector< ShopSlot* > m_vecMainNewShopSlot;
	vector< ShopSlot* > m_vecMainHotShopSlot;
	vector< ShopSlot* > m_vecMainRecommendShopSlot;
	*/
	m_pMainNewSM		= new CashShopSlotManager( CX2Slot::ST_CASH_SHOP_MAIN_NEW );
	m_pMainHotSM		= new CashShopSlotManager( CX2Slot::ST_CASH_SHOP_MAIN_HOT );
	m_pMainRecommendSM	= new CashShopSlotManager( CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND );

	m_pSubSM			= new CashShopSlotManager( CX2Slot::ST_CASH_SHOP_SUB );
	m_pSubSpeicialSM	= new CashShopSlotManager( CX2Slot::ST_CASH_SHOP_SUB_SPECIAL );

	m_pBuyItemPopUp		= new BuyItemPopup( L"DLG_Cash_Shop_Popup_Buy.lua" );


//	m_pBuyItemPopUp		= new BuyItemPopup( L"DLG_Cash_Shop_Popup_Buy.lua" ); // �ؿ��� ����
	m_pBuyItemPopUp->Init( new BuyItemPopupSlot(), L"DLG_Cash_Shop_Popup_Buy_Slot.lua", 4 );
	m_pBuyItemPopUp->SetReverseDLGLayer( true );

	m_pCashItemToInvenPopup = new CashItemToInvenPopup( L"DLG_Cash_Shop_Popup_ItemToInven.lua" );
	m_pCashItemToInvenPopup->Init( new CashItemToInvenPopupSlot(), L"DLG_Cash_Shop_Popup_ItemToInven_Slot.lua", 3 );
	m_pCashItemToInvenPopup->SetReverseDLGLayer( true );

	m_pPresentItemPopup = new PresentItemPopup();

	ResetUnitViewer( g_pData->GetMyUser()->GetSelectUnit() );

}

void CX2CashShop::CategoryDataParsing( const WCHAR* pFileName )
{
//{{ robobeg : 2008-10-28
	//KLuaManager kLuamanager;
    KLuaManager kLuamanager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//}} robobeg : 2008-10-28

//{{ robobeg : 2008-10-28
	//g_pKTDXApp->LoadAndDoMemory( &kLuamanager, L"Enum.lua" );
//}} robobeg : 2008-10-28
	g_pKTDXApp->LoadAndDoMemory( &kLuamanager, pFileName );


	if( kLuamanager.BeginTable( "CASH_SHOP_REAL_CATEGORY_ID" ) == true )
	{
		for ( int i = 1; i < CSC_END; i++ )
		{
			if( kLuamanager.BeginTable( i ) == true )
			{
				int realID = 0;
				vector< D3DXVECTOR2 > vecSubCateID;

				LUA_GET_VALUE( kLuamanager, "REAL_ID", realID, 0 );

				if( kLuamanager.BeginTable( "CASH_SHOP_REAL_SUB_CATEGORY_ID" ) == true )
				{
					int index = 1;
					while( kLuamanager.BeginTable( index ) == true )
					{
						index++;
						D3DXVECTOR2 cateData;
						LUA_GET_VALUE( kLuamanager, 1, cateData.x, 0 );
						LUA_GET_VALUE( kLuamanager, 2, cateData.y, 0 );

						vecSubCateID.push_back( cateData );

						kLuamanager.EndTable();
					}

					kLuamanager.EndTable();
				}

				kLuamanager.EndTable();

				CX2CashShop::CashShopCategory* pCashShopCategory = new CashShopCategory();
				pCashShopCategory->m_CategoryID = D3DXVECTOR2( (float)i, (float)realID );
				pCashShopCategory->m_vecSubCategoryID = vecSubCateID;
				m_vecCashShopCateList.push_back( pCashShopCategory );
			}
			else
			{
				ErrorLog( XEM_ERROR129 )
#if X2_DEV_TOOLS
					MessageBox( g_pKTDXApp->GetHWND(), L"CX2LocationManager CASH_SHOP_REAL_CATEGORY_ID ���̵� ������ �ֳ�", L"Error", MB_OK );
#endif
			}

		}

		kLuamanager.EndTable();
	}
	else
	{
		ErrorLog( XEM_ERROR128 );
#if X2_DEV_TOOLS
		MessageBox( g_pKTDXApp->GetHWND(), L"CX2CashShop CASH_SHOP_REAL_CATEGORY_ID BeginTable", L"Error", MB_OK );

#endif
		return;
	}
}
bool CX2CashShop::Handler_EGS_BUY_CASH_ITEM_REQ( vector< unsigned long >& vecProductNoList, bool bChoicedItemBuy /* = false */ )
{
	if( true == g_pMain->IsWaitingServerPacket(EGS_BUY_CASH_ITEM_ACK))
		return false;

	KEGS_BUY_CASH_ITEM_REQ kPacket;
	kPacket.m_bIsWishList = bChoicedItemBuy;
	std::vector< KNXBTPurchaseReqInfo > vecKNXBTPurchaseReqInfo;

	for ( int i = 0; i < (int)vecProductNoList.size(); i++ )
	{
		unsigned long productNo = vecProductNoList[i];
		KNXBTPurchaseReqInfo kReqInfo;
		kReqInfo.m_ulProductNo = productNo;
		kReqInfo.m_usOrderQuantity = 1;
		vecKNXBTPurchaseReqInfo.push_back( kReqInfo );
	}

	if ( vecKNXBTPurchaseReqInfo.empty() == true )
		return true;

	kPacket.m_vecPurchaseReqInfo = vecKNXBTPurchaseReqInfo;

	// UI ���� ���� ĳ���� �����Ұ��� �����Ѱ� �����Ѵ�
	// ���� UI ������ �߰��� ���߿�
	kPacket.m_iUseCashType = m_pBuyItemPopUp->GetUseCashType();

	//�˻��غ��� �Žñ� �ϸ� //
	//if (  )
	vector<int> vecOverlapItemID;
	for ( int i = 0; i < (int)kPacket.m_vecPurchaseReqInfo.size(); i++ )
	{
		KNXBTPurchaseReqInfo kReqInfo = kPacket.m_vecPurchaseReqInfo[i];
		KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( (int)kReqInfo.m_ulProductNo );
		if ( pKBillProductInfo != NULL )
		{
			int itemID = pKBillProductInfo->m_iProductID;

			//{{ ����� : [2010/7/9/] //	�κ��丮 Ȯ�� ĳ�� ���� �ڵ�(�ӽ�)
			int iCategory = CX2Inventory::ST_NONE;

			switch( itemID )
			{
			case INVENTORY_SLOT_ADD_ITEM_EQUIP:
				{
					iCategory = CX2Inventory::ST_EQUIP;
					break;
				}
			case INVENTORY_SLOT_ADD_ITEM_ACCESSORY:
				{
					iCategory = CX2Inventory::ST_ACCESSORY;
					break;
				}
			case INVENTORY_SLOT_ADD_ITEM_QUICK_SLOT:
				{
					iCategory = CX2Inventory::ST_QUICK_SLOT;
					break;
				}
			case INVENTORY_SLOT_ADD_ITEM_MATERIAL:
				{
					iCategory = CX2Inventory::ST_MATERIAL;
					break;
				}
			case INVENTORY_SLOT_ADD_ITEM_QUEST:
				{
					iCategory = CX2Inventory::ST_QUEST;
					break;
				}
			case INVENTORY_SLOT_ADD_ITEM_SPECIAL:
				{
					iCategory = CX2Inventory::ST_SPECIAL;
					break;
				}
			default:
				iCategory = CX2Inventory::ST_NONE;
			}

			if( iCategory != CX2Inventory::ST_NONE &&
				//g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetSelectUnit() != NULL
				//g_pData->GetMyUser()->GetSelectUnit()->GetInventory() != NULL
				)
			{
				//  �κ� �ִ� ���� üũ
				if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum( static_cast<CX2Inventory::SORT_TYPE>(iCategory) ) >= INVENTORY_SLOT_MAX_NUM )
				{
					// �ִ� Ȯ�� ������ �����̻��̸� ����ó��
					m_pBuyItemPopUp->SetOpen( false );
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_5197 ), g_pMain->GetNowState() );
					return false;
				}
			}
			//}} ����� : [2010/7/9/] //	�κ��丮 Ȯ�� ĳ�� ���� �ڵ�(�ӽ�)

			//{{ kimhc // 2009-08-11 // PLATINUM ���� ȸ���� ���Ž� ����ó��
			if ( ( itemID	== PLATINUM_MEMBERSHIP_CARD || itemID == COBO_CREDIT_CARD )
				&& m_bCheckDialogWarningPlatinumMembership == false )
			{
				if ( g_pData->GetQuestManager() != NULL )
				{
					// ���� ����Ʈ�� Ŭ���� �ߴ°�?
					//if ( g_pData->GetQuestManager()->GetMapCompleteQuest().find( _CONST_UI_PRIVATE_BANK_::g_iBankQuestID )
					//	== g_pData->GetQuestManager()->GetMapCompleteQuest().end() )	// ����Ʈ�� Ŭ���� ���� �ʾ�����
					if ( g_pData->GetQuestManager()->GetUnitCompleteQuest( _CONST_UI_PRIVATE_BANK_::g_iBankQuestID ) == false )
					{
						if ( itemID	== PLATINUM_MEMBERSHIP_CARD )
						{
							g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_3869 ), CSCUM_BUY_PLATINUM_CARD_OK,
								g_pMain->GetNowState(), CSCUM_BUY_PLATINUM_CARD_CANCEL );
						}
						else if ( itemID == COBO_CREDIT_CARD )
						{
							g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_4353 ), CSCUM_BUY_PLATINUM_CARD_OK,
								g_pMain->GetNowState(), CSCUM_BUY_PLATINUM_CARD_CANCEL );
						}



						return false;
					}
				}
			}
			//}} kimhc // 2009-08-11 // PLATINUM ���� ȸ���� ���Ž� ����ó��

			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
			if ( pItemTemplet != NULL )
			{
				if ( pItemTemplet->GetItemType() == CX2Item::IT_WEAPON ||
					pItemTemplet->GetItemType() == CX2Item::IT_DEFENCE ||
					pItemTemplet->GetItemType() == CX2Item::IT_ACCESSORY )
				{
					if ( g_pData->GetMyUser()->GetSelectUnit() != NULL
						&& g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( itemID, true ) != NULL )
					{
						vecOverlapItemID.push_back( itemID );
					}
				}
			}
			//������ 2�� ���� ������ ���� �ϰ� �ִ� ��� �������� ���ϵ��� ����ó��
			if( itemID == UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID )
			{
				if( g_pData->GetMyUser()->GetSelectUnit() != NULL
					&& ( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( itemID ) != NULL
					|| 2 != g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )
					)
				{
					//���� �������� ���� �� �� �����ϴ�. ��� �޽����� �ٿ���
					m_pBuyItemPopUp->SetOpen( false );
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_29780 ), g_pMain->GetNowState() );
					return false;
				}
				else
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_29805 ), g_pMain->GetNowState() );
				}
			}

		}
	}

	kPacket.m_iUseCashType = m_pBuyItemPopUp->GetUseCashType();

	if ( vecOverlapItemID.empty() == false )
	{
		wstring wstrItemNames;
		for ( int i = 0; i < (int)vecOverlapItemID.size(); i++ )
		{
			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( vecOverlapItemID[i] );
			if ( pItemTemplet != NULL )
			{
				wstrItemNames += pItemTemplet->GetName();
				if ( i != (int)vecOverlapItemID.size() - 1 )
				{
					wstrItemNames += L", ";
				}
			}

		}
		m_ReservedKEGS_BUY_CASH_ITEM_REQ = kPacket;
		g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_20, "L", wstrItemNames ) ), CSCUM_BUY_POP_UP_OVERLAP_OK,
			g_pMain->GetNowState(), CSCUM_BUY_POP_UP_OVERLAP_CANCEL );
		return false;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_BUY_CASH_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_BUY_CASH_ITEM_ACK );

	m_bSend_EGS_BUY_CASH_ITEM_REQ = true;

	return true;
}

bool CX2CashShop::Handler_EGS_BUY_CASH_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_BUY_CASH_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	m_bSend_EGS_BUY_CASH_ITEM_REQ = false;

	if ( g_pMain->DeleteServerPacket( EGS_BUY_CASH_ITEM_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( kEvent.m_bIsWishList == true )
			{
				vector< int > vecItemID;
				for ( int i = 0; i < (int)kEvent.m_vecPurchaseReqInfo.size(); i++ )
				{
					KNXBTPurchaseReqInfo& kKNXBTPurchaseReqInfo = kEvent.m_vecPurchaseReqInfo[i];
					KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( (int)kKNXBTPurchaseReqInfo.m_ulProductNo );
					if ( pKBillProductInfo != NULL )
					{
						vecItemID.push_back( pKBillProductInfo->m_iProductID );
					}
				}
				Handler_EGS_MODIFY_WISH_LIST_REQ( false, vecItemID );
			}

			bool bCheckBuyResurrectionStone		= false;
			bool bCheckBuyInvenSlotAdd			= false;
			//{{ kimhc // 2009-08-11 // ���� ȸ���� ������ ���̾�α� ó�� �ٸ��� �ϱ�����
			bool bCheckBuyBankMembershipItem	= false;
			//{{ kimhc // 2009-08-11 // ���� ȸ���� ������ ���̾�α� ó�� �ٸ��� �ϱ�����

			int rebirthStoneNum = 0;

			for ( int i = 0; i < (int)kEvent.m_vecPurchaseReqInfo.size(); i++ )
			{
				KNXBTPurchaseReqInfo kReqInfo = kEvent.m_vecPurchaseReqInfo[i];
				KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( (int)kReqInfo.m_ulProductNo );
				if ( pKBillProductInfo != NULL )
				{
					int itemID = pKBillProductInfo->m_iProductID;
					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
					if ( pItemTemplet != NULL )
					{
						if ( pItemTemplet->GetItemType() == CX2Item::IT_WEAPON ||
							pItemTemplet->GetItemType() == CX2Item::IT_DEFENCE ||
							pItemTemplet->GetItemType() == CX2Item::IT_ACCESSORY )
						{
							if ( g_pData->GetMyUser()->GetSelectUnit() != NULL
								&& g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( itemID, true ) != NULL )
							{

								g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_21 ), g_pMain->GetNowState() );
							}
						}
					}
				}
			}

			wstringstream wstrstm;
			for ( int i = 0;  i < (int)kEvent.m_vecPurchaseReqInfo.size(); i++ )
			{
				KNXBTPurchaseReqInfo& kKNXBTPurchaseReqInfo = kEvent.m_vecPurchaseReqInfo[i];
				KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( (int)kKNXBTPurchaseReqInfo.m_ulProductNo );

				if ( pKBillProductInfo != NULL )
				{

					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( pKBillProductInfo->m_iProductID );
					CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( pKBillProductInfo->m_iProductID );
					if ( pItemTemplet != NULL && pCashItem != NULL )
					{
						//127030��Ȱ��
						if ( 127030 == pItemTemplet->GetItemID() )
						{
							bCheckBuyResurrectionStone = true;
							rebirthStoneNum = (int)pKBillProductInfo->m_cQuantity;
						}

						//200750 ����ǰ 8ĭ Ȯ��
						if ( INVENTORY_SLOT_ADD_ITEM == pItemTemplet->GetItemID() )
						{
							bCheckBuyInvenSlotAdd = true;
						}

						//{{ kimhc // 2009-08-11 // ���� ȸ���� ���Ž� ���̾�α� ó�� �ٸ��� �ϱ�����
						switch ( pItemTemplet->GetItemID() )
						{
						case SILVER_MEMBERSHIP_CARD:
						case GOLD_MEMBERSHIP_CARD:
						case EMERALD_MEMBERSHIP_CARD:
						case DIAMOND_MEMBERSHIP_CARD:
						case PLATINUM_MEMBERSHIP_CARD:
						case COBO_CREDIT_CARD:
							bCheckBuyBankMembershipItem		= true;
							break;

						default:
							break;
						}
						//{{ kimhc // 2009-08-11 // ���� ȸ���� ������ ���̾�α� ó�� �ٸ��� �ϱ�����
						wstrstm << pItemTemplet->GetName() << L"(" << pCashItem->GetPeriod( *pKBillProductInfo ) << L")";
						if ( i != (int)kEvent.m_vecPurchaseReqInfo.size() - 1 )
						{
							wstrstm << L", ";
						}
					}

				}
				if( i > 2 )
				{
					wstrstm << GET_REPLACED_STRING( (STR_ID_4608, "i", kEvent.m_vecPurchaseReqInfo.size()) ) << L"\n";
					break;
				}

				if ( i == (int)kEvent.m_vecPurchaseReqInfo.size() - 1 )
				{
					wstrstm << GET_STRING( STR_ID_22 ) << L"\n";
					if ( bCheckBuyResurrectionStone == true && g_pData->GetMyUser()->GetSelectUnit() != NULL )
						wstrstm << GET_REPLACED_STRING( ( STR_ID_23, "i", g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum() + rebirthStoneNum ) ) << L"\n";
					if ( bCheckBuyInvenSlotAdd == true && g_pData->GetMyUser()->GetSelectUnit() != NULL )
						wstrstm << GET_REPLACED_STRING( ( STR_ID_25, "i", g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_EQUIP) + 8 ) );
					//{{ kimhc // 2009-08-11 // ���� ȸ���� ������ ���̾�α� ó�� �ٸ��� �ϱ�����
					if ( bCheckBuyBankMembershipItem	== true )
						wstrstm	<< GET_STRING( STR_ID_3868 );
					//{{ kimhc // 2009-08-11 // ���� ȸ���� ������ ���̾�α� ó�� �ٸ��� �ϱ�����
				}

			}


			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), wstrstm.str().c_str(), g_pMain->GetNowState() );
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"������ ���� ����!", g_pMain->GetNowState() );

			CX2State* pNowState = (CX2State*)g_pMain->GetNowState();
			pNowState->Handler_EGS_CHECK_BALANCE_REQ();

			return Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( 1, CASH_SHOP_DEPOSIT_SLOT_NUM );

		}
	}
	return false;
}

bool CX2CashShop::Handler_EGS_BILL_INVENTORY_INQUIRY_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_BILL_INVENTORY_INQUIRY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_BILL_INVENTORY_INQUIRY_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( GetOpen() == true )
			{
				if ( m_iReqDepositPageIndex == 1 || kEvent.m_vecOrderInfo.empty() == false )
				{
					vector< KBillOrderInfo > vecKBillOrderInfo;
					m_vecKBillOrderInfo = kEvent.m_vecOrderInfo;
					m_vecKBillPackageInfo = kEvent.m_vecPackageInfo;			// ��Ű���� ���صǹǷ� ����
					m_iNowDepositPageIndex = m_iReqDepositPageIndex;

					for ( int i = 0; i < (int)m_vecKBillOrderInfo.size(); i++ )
					{
						KBillOrderInfo& kKBillOrderInfo = m_vecKBillOrderInfo[i];
						vecKBillOrderInfo.push_back( kKBillOrderInfo );
					}
					ResetDepositItemSlot( CX2Slot::ST_CASH_SHOP_DEPOSIT, vecKBillOrderInfo );
				}
			}

#ifdef CASH_INVEN_PICKUP_ALL
			if( true == m_bPickUpAll )
			{
				PickUpAll();
			}
#endif //CASH_INVEN_PICKUP_ALL
		}
#ifdef CASH_INVEN_PICKUP_ALL
		else if( true == m_bPickUpAll )
		{
			m_bPickUpAll = false;
		}
#endif //CASH_INVEN_PICKUP_ALL
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CX2CashShop::Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( KBillOrderInfo& kKBillOrderInfo, map< int, int >& mapOption, const map<int, int>& mapGroupdIdList_ )
{
	m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ.m_iTransNo = kKBillOrderInfo.m_iTransNo;
	m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ.m_iProductNo = kKBillOrderInfo.m_kBillProductInfo.m_iProductNo;
	m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ.m_mapSocketForCash = mapOption;
	m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ.m_byteProductKind = kKBillOrderInfo.m_byteProductKind;

	m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ.m_mapSocketGroupIDForCash = mapGroupdIdList_;

	//�Ⱓ�� �������̶�� ... �ߺ��Ǵ��� üũ�� �� �����ð����� ��� �޽��� �ɼǵ� ������� �ϳ� �̷� ����

	bool bCheck = false;

	//KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( kKBillOrderInfo.m_iProductNo );
	//if ( pKBillProductInfo != NULL && (int)pKBillProductInfo->m_cPeriod > 0 )
	if ( (int)kKBillOrderInfo.m_kBillProductInfo.m_cPeriod > 0 )
	{
		//int itemID = pKBillProductInfo->m_iProductID;
		//if ( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( itemID ) > 0 )
		if ( g_pData->GetMyUser()->GetSelectUnit() != NULL
			&& g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( kKBillOrderInfo.m_kBillProductInfo.m_iProductID ) > 0 )
		{
			//����� �������.
			m_pDLGOverlapCheckCashItemToInven = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_26 ), CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK, g_pMain->GetNowState() );
			bCheck = true;
		}
	}

	if ( bCheck == false )
	{
		Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REQ();
	}
	return true;
}
bool CX2CashShop::Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REQ()
{
	KEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ kPacket = m_TempKEGS_BILL_GET_PURCHASED_CASH_ITEM_REQ;

	g_pData->GetServerProtocol()->SendPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK );

	return true;
}
bool CX2CashShop::Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_BILL_GET_PURCHASED_CASH_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
	bool bExpandQuickSlot = false;

	g_pMain->DeleteServerPacket( EGS_BILL_GET_PURCHASED_CASH_ITEM_ACK );

	if ( g_pData == NULL || g_pData->GetMyUser()->GetSelectUnit() == NULL )
		return false;

	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			//need a process
			Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( 1, CASH_SHOP_DEPOSIT_SLOT_NUM );
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );

			bool		bUpdateCashShopState	= false;
			CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();
			map< int, int >::iterator mit;
			for ( mit = kEvent.m_mapExpandedCategorySlot.begin(); mit != kEvent.m_mapExpandedCategorySlot.end(); mit++ )
			{
				//if ( pInventory != NULL )
				{
					kInventory.SetItemMaxNum( (CX2Inventory::SORT_TYPE)mit->first,
						kInventory.GetItemMaxNum((CX2Inventory::SORT_TYPE)mit->first) + mit->second );

					//{{ kimhc // 2009-08-17 // ���� Ȯ����� �������� ���� ������Ʈ
					if ( static_cast< CX2Inventory::SORT_TYPE >( mit->first ) == CX2Inventory::ST_BANK )
						bUpdateCashShopState = true;
					//{{ kimhc // 2009-08-17 // ���� Ȯ����� �������� ���� ������Ʈ
					if (	static_cast< CX2Inventory::SORT_TYPE >( mit->first ) == CX2Inventory::ST_E_QUICK_SLOT )
					{
						if( NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
						{
							g_pData->GetUIManager()->GetUIQuickSlot()->SetExpandQuickSlot( kInventory.GetItemMaxNum( CX2Inventory::ST_E_QUICK_SLOT ) );	// �ƽ� ���� �ø���

							bExpandQuickSlot = true;

							g_pKTDXApp->GetDIManager()->Getkeyboard()->SetAction( GAMEACTION_QUICKSLOT4, DIK_4 );
							g_pKTDXApp->GetDIManager()->Getkeyboard()->SetAction( GAMEACTION_QUICKSLOT5, DIK_5 );
							g_pKTDXApp->GetDIManager()->Getkeyboard()->SetAction( GAMEACTION_QUICKSLOT6, DIK_6 );

							if( g_pKTDXApp->GetDIManager()->GetJoystic() != NULL )
							{
								g_pKTDXApp->GetDIManager()->GetJoystic()->SetAction( GAMEACTION_QUICKSLOT4, JB_BUTTON14 );
								g_pKTDXApp->GetDIManager()->GetJoystic()->SetAction( GAMEACTION_QUICKSLOT5,	JB_BUTTON15 );
								g_pKTDXApp->GetDIManager()->GetJoystic()->SetAction( GAMEACTION_QUICKSLOT6,	JB_BUTTON16 );
							}
						}
					}
				}
			}

			if ( kEvent.m_iExceptionProcessItemID == CHANGE_GUILD_NAME_ITEM_ID )
			{
				g_pData->GetGuildManager()->GetUIGuild()->SetShowChangeGuildNameDlg(true);
				//������ ���� �˾� ���� ���� ����
				return true;
			}


			if ( g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum() == kEvent.m_iNumResurrectionStone &&
				kEvent.m_mapExpandedCategorySlot.empty() == true &&
				kEvent.m_vecInventorySlotInfo.empty() == false )
			{
#ifdef CASH_INVEN_PICKUP_ALL
				if( false == m_bPickUpAll )
#endif
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_109 ), (CKTDXStage*) m_pNowState );
			}
			else
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2684 ), (CKTDXStage*) m_pNowState );
			}

			g_pData->GetMyUser()->GetSelectUnit()->SetResurrectionStoneNum( kEvent.m_iNumResurrectionStone );

			//{{ kimhc // 2009-08-17 // ���� Ȯ����� �������� ���� ������Ʈ
			if ( GetOpen() == true && bUpdateCashShopState == true )
			{
				SetCashShopState( m_CashShopState, m_CashShopCategory );

				//{{  [1/6/2010 sulzi]	����Ʈ ����
				if( NULL != g_pData->GetQuestManager() )
				{
					g_pData->GetQuestManager()->GiveUpForbiddenQuest();
				}
				//}}  [1/6/2010 sulzi]
			}
			//{{ kimhc // 2009-08-17 // ���� Ȯ����� �������� ���� ������Ʈ
			else if ( GetOpen() == true && bExpandQuickSlot == true )
			{
				SetCashShopState( m_CashShopState, m_CashShopCategory );
			}

			return true;
		}
#ifdef CASH_INVEN_PICKUP_ALL
		else
		{
			// ���� ��Ŷ�� ���� PickUpAll �����.
			m_bPickUpAll = false;
		}
#endif // CASH_INVEN_PICKUP_ALL
	}


	return false;
}


bool CX2CashShop::Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REP( KBillOrderInfo& kKBillOrderInfo, map< int, int >& mapOption, const map<int, int>& mapGroupdIdList_, vector< KBillPackageInfo >& vecPackageInfo_ )
{
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_ulOrderNo = kKBillOrderInfo.m_iTransNo;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_ulPackageProductNo = kKBillOrderInfo.m_kBillProductInfo.m_iProductNo;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_byteProductKind = kKBillOrderInfo.m_byteProductKind;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_wstrPackageProductID = kKBillOrderInfo.m_kBillProductInfo.m_wstrProductName;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_mapSocketForCash = mapOption;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_iProductID = kKBillOrderInfo.m_kBillProductInfo.m_iProductID;

	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_mapSocketGroupIDForCash = mapGroupdIdList_;

	//�� ���� ��ǰ ���ͷ� �Žñ� �غ���.
	/*
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_ulProductNo = kKNXBTOrderInfo.m_ulProductNo;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_wstrProductID = kKNXBTOrderInfo.m_wstrProductID;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_usProductExpire = kKNXBTOrderInfo.m_usProductExpire;
	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_usProductPieces = kKNXBTOrderInfo.m_usProductPieces;
	*/

	m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_vecPickUpPackageInfo.clear();
	for ( int i = 0; i < (int)vecPackageInfo_.size(); i++ )
	{
		KBillPackageInfo& kKNXBTPackageInfo = vecPackageInfo_[i];
		KNXBTPickUpPackageInfo kKNXBTPickUpPackageInfo;


		kKNXBTPickUpPackageInfo.m_ulProductNo = kKNXBTPackageInfo.m_ulProductNo;
		kKNXBTPickUpPackageInfo.m_wstrProductID = kKNXBTPackageInfo.m_wstrProductID;
		kKNXBTPickUpPackageInfo.m_sProductExpire = kKNXBTPackageInfo.GetProductExpire();
		kKNXBTPickUpPackageInfo.m_usProductPieces = kKNXBTPackageInfo.m_usProductPieces;
		kKNXBTPickUpPackageInfo.m_iSubProductNo = kKNXBTPackageInfo.m_iProductID;


		m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ.m_vecPickUpPackageInfo.push_back( kKNXBTPickUpPackageInfo );
	}


	//�Ⱓ�� �������̶�� ... �ߺ��Ǵ��� üũ�� �� �����ð����� ��� �޽��� �ɼǵ� ������� �ϳ� �̷� ����

	bool bCheck = false;

	for ( int i = 0; i < (int)vecPackageInfo_.size(); i++ )
	{
		const KBillPackageInfo& kKNXBTPackageInfo = vecPackageInfo_[i];

		if ( kKNXBTPackageInfo.GetProductExpire() > 0 )
		{
			int itemID = _wtoi( kKNXBTPackageInfo.m_wstrProductID.c_str() );
			if ( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( itemID ) > 0 )
			{
				//����� �������.
				m_pDLGOverlapCheckCashItemToInven = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_26 ), CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK_PACKAGE, g_pMain->GetNowState() );
				bCheck = true;
				break;
			}
		}
	}

	if ( bCheck == false )
	{
		Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ();
	}

	return true;
}

bool CX2CashShop::Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ()
{
	KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ kPacket = m_TempKEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK );

	return true;
}

bool CX2CashShop::Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			//need a process
			Handler_EGS_BILL_INVENTORY_INQUIRY_REQ( 1, CASH_SHOP_DEPOSIT_SLOT_NUM );
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );

			CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();
			kInventory.UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );

			map< int, int >::iterator mit;
			for ( mit = kEvent.m_mapExpandedCategorySlot.begin(); mit != kEvent.m_mapExpandedCategorySlot.end(); mit++ )
			{
				{
					kInventory.SetItemMaxNum( (CX2Inventory::SORT_TYPE)mit->first,
						kInventory.GetItemMaxNum((CX2Inventory::SORT_TYPE)mit->first) + mit->second );
				}
			}


			if ( g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum() == kEvent.m_iNumResurrectionStone &&
				kEvent.m_mapExpandedCategorySlot.empty() == true
				&& kEvent.m_vecInventorySlotInfo.empty() == false
				)
			{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_109 ), (CKTDXStage*) m_pNowState );
			}
			else
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_2684 ), (CKTDXStage*) m_pNowState );
			}
			g_pData->GetMyUser()->GetSelectUnit()->SetResurrectionStoneNum( kEvent.m_iNumResurrectionStone );

			UpdateCashUI();
			return true;
		}
	}

	return false;
}


bool CX2CashShop::Handler_EGS_GET_WISH_LIST_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_WISH_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_GET_WISH_LIST_ACK ) == true )
	{
		//if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			m_vecChoicedItemID.resize(0);

			//vector< int > vecRemoveItemID;

			map<int,int>::iterator mit;
			for ( mit = kEvent.m_mapWishList.begin(); mit != kEvent.m_mapWishList.end(); mit++ )
			{
				if ( mit->second <= 0 )			// ������ ID�� 0�̰ų� �����̸� ����
				{
					continue;
				}
				if ( std::find( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), mit->second ) != m_vecChoicedItemID.end() )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_32 ), g_pMain->GetNowState() );
					continue;
				}

				if ( (int)m_vecChoicedItemID.size() >= CASH_SHOP_CHOICED_SLOT_NUM )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_33 ), g_pMain->GetNowState() );
					continue;
				}

				/*
				if ( g_pData->GetItemManager()->GetCashItem( mit->second ) == NULL )
				{
					vecRemoveItemID.push_back( mit->second );
				}
				*/


					m_vecChoicedItemID.push_back( mit->second );
			}

			if ( m_bOpen == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_CHOICED, m_vecChoicedItemID );
			}

			//if ( vecRemoveItemID.empty() == false )
			//	Handler_EGS_MODIFY_WISH_LIST_REQ( false, vecRemoveItemID );


			return true;
		}
	}

	return false;
}

bool CX2CashShop::Handler_EGS_MODIFY_WISH_LIST_REQ( bool bAdd, vector< int >& vecItemID )
{
	KEGS_MODIFY_WISH_LIST_REQ kPacket;
	kPacket.m_bIsDelete = !bAdd;
	kPacket.m_vecWishListItemID = vecItemID;

	g_pData->GetServerProtocol()->SendPacket( EGS_MODIFY_WISH_LIST_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_MODIFY_WISH_LIST_ACK );

	return true;

}

bool CX2CashShop::Handler_EGS_MODIFY_WISH_LIST_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_MODIFY_WISH_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_MODIFY_WISH_LIST_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( kEvent.m_bIsDelete == true )
			{
				for ( int i = 0;  i < (int)kEvent.m_vecWishListItemID.size(); i++ )
				{

					if ( std::find( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), kEvent.m_vecWishListItemID[i] ) != m_vecChoicedItemID.end() )
					{
						//�ϴ� ã�Ұ�..
						m_vecChoicedItemID.erase( std::remove( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), kEvent.m_vecWishListItemID[i] ), m_vecChoicedItemID.end() );

					}
				}


			}
			else
			{
				for ( int i = 0;  i < (int)kEvent.m_vecWishListItemID.size(); i++ )
				{
					if ( std::find( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), kEvent.m_vecWishListItemID[i] ) != m_vecChoicedItemID.end() )
					{
						//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), L"�̹� ���� ��ǰ�Դϴ�!", g_pMain->GetNowState() );
						StateLog( L"�̹� ���� ��ǰ�Դϴ�!" );
						continue;
					}

					if ( (int)m_vecChoicedItemID.size() >= CASH_SHOP_CHOICED_SLOT_NUM )
					{
						//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), L"���� ������ ����� ��á���ϴ�.", g_pMain->GetNowState() );
						StateLog( L"���� ������ ����� ��á���ϴ�." );
						continue;
					}

					m_vecChoicedItemID.push_back( kEvent.m_vecWishListItemID[i] );
				}
			}

			if ( m_bOpen == true )
			{
				ResetItemSlot( CX2Slot::ST_CASH_SHOP_CHOICED, m_vecChoicedItemID );
			}
			return true;
		}
	}

	return false;
}

bool CX2CashShop::Handler_EGS_PRESENT_CASH_ITEM_REQ( const WCHAR* wszReceiverNickName, const WCHAR* wszMessage,
													const WCHAR* wszPassword, unsigned long ulProductNo, bool bChoicedItem )
{
	KEGS_PRESENT_CASH_ITEM_REQ kPacket;
	kPacket.m_wstrReceiverNickName = wszReceiverNickName;
	kPacket.m_wstrMessage = wszMessage;
	kPacket.m_bIsWishList = bChoicedItem;
	kPacket.m_wstrPassword = wszPassword;

	kPacket.m_iUseCashType = m_pPresentItemPopup->GetUseCashType();


	KNXBTPurchaseReqInfo kReqInfo;
	kReqInfo.m_ulProductNo = ulProductNo;
	kReqInfo.m_usOrderQuantity = 1;
	kPacket.m_vecPurchaseReqInfo.push_back( kReqInfo );

	g_pData->GetServerProtocol()->SendPacket( EGS_PRESENT_CASH_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_PRESENT_CASH_ITEM_ACK );

	return true;
}

bool CX2CashShop::Handler_EGS_PRESENT_CASH_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_PRESENT_CASH_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_PRESENT_CASH_ITEM_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if ( kEvent.m_bIsWishList == true && kEvent.m_vecPurchaseReqInfo.empty() == false )
			{
				vector< int > vecItemID;
				for ( int i = 0; i < (int)kEvent.m_vecPurchaseReqInfo.size(); i++ )
				{
					KNXBTPurchaseReqInfo& kKNXBTPurchaseReqInfo = kEvent.m_vecPurchaseReqInfo[i];
					KBillProductInfo* pKNXBTProductInfo = g_pData->GetItemManager()->GetCashItemProduct( kKNXBTPurchaseReqInfo.m_ulProductNo );
					if ( pKNXBTProductInfo != NULL )
					{
						vecItemID.push_back( pKNXBTProductInfo->m_iProductID );
					}
				}
				Handler_EGS_MODIFY_WISH_LIST_REQ( false, vecItemID );
			}

			CX2State* pNowState = (CX2State*)g_pMain->GetNowState();
			pNowState->Handler_EGS_CHECK_BALANCE_REQ();

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_28 ), g_pMain->GetNowState() );
			return true;
		}
	}

	return false;
}

bool CX2CashShop::Handler_EGS_CHECK_PRESENT_CASH_INVENTORY_NOT( unsigned long ulOrderNo, unsigned long ulProductNo )
{
	KEGS_CHECK_PRESENT_CASH_INVENTORY_NOT kPacket;
	kPacket.m_ulOrderNo = ulOrderNo;
	kPacket.m_ulProductNo = ulProductNo;

	g_pData->GetServerProtocol()->SendPacket( EGS_CHECK_PRESENT_CASH_INVENTORY_NOT, kPacket );
	return true;
}



bool CX2CashShop::Handler_EGS_EXPAND_CHAR_SLOT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPAND_CHAR_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pData->GetMyUser()->AccessUserData().maxUnitCount = kEvent.m_iCharSlotSize;

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_REPLACED_STRING( ( STR_ID_29, "i", kEvent.m_iCharSlotSize ) ), g_pMain->GetNowState() );

	return true;
}


bool CX2CashShop::MouseUp( D3DXVECTOR2 mousePos )
{

	// ��� �������� ��� ó�� : ���� ����
	CX2SlotItem* pItemSlot = NULL;
	pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );
	if ( pItemSlot == NULL )
		return false;

	if ( MouseLRButtonUpEquip( pItemSlot ) == true )
		return true;

	return false;
}

bool CX2CashShop::MouseRButtonUp( D3DXVECTOR2 mousePos )
{
	// ��� �������� ��� ó�� : ���� ����
	CX2SlotItem* pItemSlot = NULL;
	pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );
	if ( pItemSlot == NULL )
		return false;

	if ( MouseLRButtonUpEquip( pItemSlot ) == true )
	{
		return true;
	}
	else if ( pItemSlot->GetSlotType() == CX2SlotManager::CX2Slot::ST_CASH_SHOP_CHOICED )
	{
		//RemoveChoicedItem( pItemSlot->GetItemTID() );
		//���⿡�� ���� �޴� ��ư �����η�~
		m_ChoicedItemMenuItemID = pItemSlot->GetItemTID();
		if ( m_pDLGChoicedItemMenu == NULL )
		{
			m_pDLGChoicedItemMenu = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Choiced_Menu_Window.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGChoicedItemMenu );
		}

		m_pDLGChoicedItemMenu->SetShow( true );
		m_pDLGChoicedItemMenu->SetEnable( true );

		//���ø���Ʈ���� ĳ���������� ������ Ŭ���Ͽ��� �� ���� �Ұ����� �������� ���� ��ư�� ��Ȱ��ȭ ��.
		CKTDGUIControl* pGift = m_pDLGChoicedItemMenu->GetControl( L"Gift" );

		if( pGift != NULL
			&& IsPossiblePresent( pItemSlot->GetItemTID() ) == false
			)
		{
			pGift->SetEnable( false );
		}


		D3DXVECTOR2 vMousePos;
		vMousePos.x = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos();
		vMousePos.y = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos();
		vMousePos = g_pKTDXApp->MouseConvertByResolution( vMousePos );

		m_pDLGChoicedItemMenu->SetPos( D3DXVECTOR2( (float)vMousePos.x-30.f, (float)vMousePos.y - 40.f ) );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGChoicedItemMenu, true );
	}
	else if ( pItemSlot->GetSlotType() == CX2SlotManager::CX2Slot::ST_CASH_SHOP_DEPOSIT )
	{

		if ( pItemSlot->GetKBillOrderInfo().m_byteProductKind == 1 )
		{
			vector< int > vecItemID;
			vector< KBillOrderInfo > vecKBillTOrderInfo;

			for ( int i = 0; i < (int)m_vecKBillPackageInfo.size(); i++ )
			{
				KBillPackageInfo& kKBillPackageInfo = m_vecKBillPackageInfo[i];
				if ( /*kKBillPackageInfo.m_ulProductNo == pItemSlot->GetKBillOrderInfo().m_kBillProductInfo.m_iProductNo &&*/
					kKBillPackageInfo.m_iPackageNo == pItemSlot->GetKBillOrderInfo().m_iTransNo )
				{
					//int tempItemID = _wtoi( kKBillPackageInfo.m_wstrProductID.c_str() );
					//int tempItemID = kKBillPackageInfo.m_ulProductNo;
					int tempItemID = kKBillPackageInfo.m_iProductID;

					//if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( tempItemID ) == true )
					{
						if ( std::find( vecItemID.begin(), vecItemID.end(), tempItemID ) == vecItemID.end() )
							vecItemID.push_back( tempItemID );
					}
				}
			}
			vecKBillTOrderInfo.push_back( pItemSlot->GetKBillOrderInfo() );
			m_pCashItemToInvenPopup->Reset( vecItemID, vecKBillTOrderInfo, m_vecKBillPackageInfo );
			m_pCashItemToInvenPopup->SetOpen( true );
		}
		else
		{
			const CX2Item::ItemTemplet* pItemTemplate = g_pData->GetItemManager()->GetItemTemplet( pItemSlot->GetItemTID() );
			if ( pItemTemplate != NULL )
			{
				if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( pItemTemplate->GetItemID() ) == true )
				{
					vector< int > vecItemID;
					vector< KBillOrderInfo > vecKBillOrderInfo;
					vecItemID.push_back( pItemTemplate->GetItemID() );
					vecKBillOrderInfo.push_back( pItemSlot->GetKBillOrderInfo() );
					m_pCashItemToInvenPopup->Reset( vecItemID, vecKBillOrderInfo, m_vecKBillPackageInfo );
					m_pCashItemToInvenPopup->SetOpen( true );
				}
				else
				{
					m_TempCashItemToInven = pItemSlot->GetKBillOrderInfo();
					//KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( m_TempCashItemToInven.m_iProductNo );
					//if( NULL != pKBillProductInfo )
					{
						const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( m_TempCashItemToInven.m_kBillProductInfo.m_iProductID );
						if ( pItemTemplet != NULL )
						{
							if(m_TempCashItemToInven.m_byteProductKind != 1)
							{
								m_pDLGCheckCashItemToInven = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_REPLACED_STRING( ( STR_ID_30, "S", pItemTemplet->GetName() ) ), CSCUM_ITEM_TO_INVEN_CHECK_OK, g_pMain->GetNowState() );
								//�˾� �������.
							}
							else
							{
								m_pDLGCheckCashItemToInven = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_REPLACED_STRING( ( STR_ID_30, "S", pItemTemplet->GetName() ) ), CSCUM_ITEM_TO_INVEN_OVERLAP_CHECK_OK_PACKAGE, g_pMain->GetNowState() );
								//�˾� �������.
							}


						}
					}
				}

			}
		}


	}

	return false;
}

bool CX2CashShop::MouseLRButtonUpEquip( CX2SlotItem* pItemSlot )
{
	if ( pItemSlot == NULL )
		return false;

	if ( CheckIsPossibleEqiupSlot( pItemSlot->GetSlotType() ) == true )
	{
		bool bCheckPackageItem = false;
		int itemID = pItemSlot->GetItemTID();
		CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
		if ( pCashItem != NULL )
		{
			if ( pCashItem->m_vecKBillProductInfo.empty() == false )
			{
				KBillProductInfo& kKBillProductInfo = pCashItem->m_vecKBillProductInfo[0];
				vector< CX2ItemManager::PackageItemData > vecPackageItemData;

				//{{ 2013.01.26 �ֹ�ö ť����Ű�������� �̸�������
				g_pData->GetItemManager()->GetPackageCubeItemData( kKBillProductInfo.m_iProductID, vecPackageItemData );
				if ( vecPackageItemData.empty() == true )
					g_pData->GetItemManager()->GetPackageData( kKBillProductInfo.m_iProductID, vecPackageItemData );
				//}} 2013.01.26 �ֹ�ö ť����Ű�������� �̸�������

				if ( vecPackageItemData.empty() == false )
				{
					bCheckPackageItem = true;
					for ( int i = 0; i < (int)vecPackageItemData.size(); i++ )
					{
						InsertPreEquipItem(vecPackageItemData[i].m_iItemID, true );
						SetEnableBuyEquippingItem( false );
					}
				}
				vector< int > vecIceHeaterItemData;
				CX2Unit::UNIT_TYPE eUnitType = CX2Unit::UT_NONE;
				if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetUnitTemplet()->m_UnitType;
				}
				g_pData->GetItemManager()->GetIceHeaterItemData( _wtoi( kKBillProductInfo.m_wstrProductName.c_str()), eUnitType, vecIceHeaterItemData );

				for( int iItemID : vecIceHeaterItemData )
				{
					InsertPreEquipItem( iItemID, true );
					SetEnableBuyEquippingItem( false );
				}
			}
		}

		if ( bCheckPackageItem == false )
			InsertPreEquipItem( pItemSlot->GetItemTID() );

		ResetNowEquipUI();
		PetPreview( itemID );
		return true;
	}
	else if ( pItemSlot->GetSlotType() == CX2SlotManager::CX2Slot::ST_EQUIPPED )
	{
		RemovePreEquipItemByEquipPos( pItemSlot->GetItemTID() );
		ResetNowEquipUI();
		// Unit Viewer
		PreEquipping();
		return true;
	}

	return false;
}

void CX2CashShop::SetCashShopState( CASH_SHOP_STATE cashShopState, CASH_SHOP_CATEGORY cashShopCate )
{
	m_CashShopState = cashShopState;
	m_CashShopCategory = cashShopCate;

	if ( m_pDLGMainPage != NULL )
		m_pDLGMainPage->SetShowEnable( false, false );

	if ( m_pDLGSubPage != NULL )
		m_pDLGSubPage->SetShowEnable( false, false );

	for ( int i = 0; i < CX2CashShop::CSC_END; i++ )
	{
		CKTDGUIDialogType pDialog = m_pDLGSubPageMenu[i];
		if ( pDialog != NULL )
			pDialog->SetShowEnable( false, false );
	}

	m_pMainNewSM->SetOpen( false );
	m_pMainHotSM->SetOpen( false );
	m_pMainRecommendSM->SetOpen( false );
	m_pSubSM->SetOpen( false );
	m_pSubSpeicialSM->SetOpen( false );

	SetEnable( false );
	SetShow( false );

	SetEnable( CX2Slot::ST_CASH_SHOP_DEPOSIT, true );
	SetShow( CX2Slot::ST_CASH_SHOP_DEPOSIT, true );

	SetEnable( CX2Slot::ST_EQUIPPED, true );
	SetShow( CX2Slot::ST_EQUIPPED, true );
	ResetNowEquipUI();

	SetEnable( CX2Slot::ST_CASH_SHOP_CHOICED, true );
	SetShow( CX2Slot::ST_CASH_SHOP_CHOICED, true );
	ResetItemSlot( CX2Slot::ST_CASH_SHOP_CHOICED, m_vecChoicedItemID );

	ResetSellCashItemByPreEquip();


	if ( m_CashShopState == CX2CashShop::CSS_MAIN )
	{
		CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)m_pDLGFront->GetControl( L"tab_main" );
		if ( pRadioButton != NULL )
		{
			if ( pRadioButton->GetCheckedControl() != NULL )
				pRadioButton->GetCheckedControl()->SetChecked( false );

			pRadioButton->SetChecked( true );
		}

		if ( m_pDLGMainPage != NULL )
			m_pDLGMainPage->SetShowEnable( true, true );

		//������ UI �������ְ�..
		//�� ���뿡 �°Բ� ���� ����Ʈ ��������߰���?

		m_pMainNewSM->Reset( g_pData->GetItemManager()->GetNewCashItemList(), false, true );
		m_pMainHotSM->Reset( g_pData->GetItemManager()->GetHotCashItemList(), false, true );
		m_pMainRecommendSM->Reset( g_pData->GetItemManager()->GetRecommendCashItemList(), false, true );

		m_pMainNewSM->SetOpen( true );
		m_pMainHotSM->SetOpen( true );
		m_pMainRecommendSM->SetOpen( true );

		SetEnable( CX2Slot::ST_CASH_SHOP_MAIN_NEW, true );
		SetEnable( CX2Slot::ST_CASH_SHOP_MAIN_HOT, true );
		SetEnable( CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND, true );
		SetShow( CX2Slot::ST_CASH_SHOP_MAIN_NEW, true );
		SetShow( CX2Slot::ST_CASH_SHOP_MAIN_HOT, true );
		SetShow( CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND, true );

		ResetItemSlot(  CX2Slot::ST_CASH_SHOP_MAIN_NEW,			m_pMainNewSM->GetNowShowItemIDList() );
		ResetItemSlot(  CX2Slot::ST_CASH_SHOP_MAIN_HOT,			m_pMainHotSM->GetNowShowItemIDList() );
		ResetItemSlot(  CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND,	m_pMainRecommendSM->GetNowShowItemIDList() );

		m_pMainHotSM->SetShowPresentButton();
		m_pMainNewSM->SetShowPresentButton();
		m_pMainRecommendSM->SetShowPresentButton();
		ResetMainPageNumUI();

	}
	else if ( m_CashShopState == CX2CashShop::CSS_SUB )
	{
		m_CashShopSubCategory = CX2CashShop::CSSC_ALL;
		switch( cashShopCate )
		{
		case CX2CashShop::CSC_FASHION:
		case CX2CashShop::CSC_ACCESSORY:
		case CX2CashShop::CSC_CONSUME:
		case CX2CashShop::CSC_INSTALL:
		case CX2CashShop::CSC_PET:
		case CX2CashShop::CSC_EVENT:
			//{{ kimhc // 2009-09-29 // �ڵ�����
			//}} kimhc // 2009-09-29 // �ڵ�����
			{
				if ( m_pDLGSubPage != NULL )
					m_pDLGSubPage->SetShowEnable( true, true );

				CKTDGUIDialogType pDialog = m_pDLGSubPageMenu[(int)cashShopCate];
				if ( pDialog != NULL )
				{
					pDialog->SetShowEnable( true, true );
					CKTDGUIRadioButton* pRadioButton = (CKTDGUIRadioButton*)pDialog->GetControl(L"Totalitem");
					if ( pRadioButton != NULL )
					{
						if ( pRadioButton->GetCheckedControl() != NULL )
						{
							pRadioButton->GetCheckedControl()->SetChecked( false );
						}

						pRadioButton->SetChecked( true );
					}
				}

				vector < int > vecItemID = GetItemByCategory( cashShopCate );

				//{{ kimhc // 2009-10-12 // �ڵ����� �� 'ĳ��' ��Ʈ���� '��' ���� ��ħ
				m_pSubSM->Reset( vecItemID );
				m_pSubSM->SetOpen( true );
				//}} kimhc // 2009-10-12 // �ڵ����� �� 'ĳ��' ��Ʈ���� '��' ���� ��ħ


				SetEnable( CX2Slot::ST_CASH_SHOP_SUB, true );
				SetShow( CX2Slot::ST_CASH_SHOP_SUB, true );
				ResetItemSlot(  CX2Slot::ST_CASH_SHOP_SUB,			m_pSubSM->GetNowShowItemIDList() );

				m_pSubSpeicialSM->Reset( GetHotItemList(vecItemID, CASH_SHOP_SUB_SPECIAL_SLOT_NUM ) );
				m_pSubSpeicialSM->SetOpen( true );
				SetEnable( CX2Slot::ST_CASH_SHOP_SUB_SPECIAL, true );
				SetShow( CX2Slot::ST_CASH_SHOP_SUB_SPECIAL, true );
				ResetItemSlot(  CX2Slot::ST_CASH_SHOP_SUB_SPECIAL,			m_pSubSpeicialSM->GetNowShowItemIDList() );

				//ī�װ����� ������ �����ؼ� �������� ����~
			}
			break;
		default:
			break;
		}

		//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
		//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�

		m_pSubSM->SetShowPresentButton();


		ResetSubPageNumUI();
	}
}

void CX2CashShop::ResetItemSlot( CX2Slot::SLOT_TYPE slotType, const vector<int>& vecItemID )
{
	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pSlotItem = (CX2SlotItem*)m_SlotList[i];
		if ( pSlotItem == NULL )
			continue;

		if ( pSlotItem->GetSlotType() == slotType )
		{
			pSlotItem->SetEnable( false );
			pSlotItem->SetShow( false );
			//pSlotItem->DestroyItemUI();
			pSlotItem->LostItemUI();
		}
	}

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pSlotItem = (CX2SlotItem*)m_SlotList[i];
		if ( pSlotItem == NULL )
			continue;

		if ( pSlotItem->GetSlotType() == slotType )
		{
			for ( int j = 0; j < (int)vecItemID.size(); j++ )
			{
				int itemID = vecItemID[j];

				if ( pSlotItem->GetSlotID() == j )
				{
					pSlotItem->SetEnable( true );
					pSlotItem->SetShow( true );
					//pSlotItem->CreateItemUI( itemID );
					if ( pSlotItem->IsCreatedItemUI() == true )
						pSlotItem->ResetItemUI( itemID );
					else
						pSlotItem->CreateItemUI( itemID );

					break;
				}
			}
		}
	}

	ResetSellCashItemByPreEquip();
}

void CX2CashShop::ResetDepositItemSlot( CX2Slot::SLOT_TYPE slotType, vector< KBillOrderInfo >& vecKBillOrderInfo )
{
	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pSlotItem = (CX2SlotItem*)m_SlotList[i];
		if ( pSlotItem == NULL )
			continue;

		if ( pSlotItem->GetSlotType() == slotType )
		{
			pSlotItem->SetEnable( false );
			pSlotItem->SetShow( false );
			pSlotItem->DestroyItemUI();
		}
	}

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pSlotItem = (CX2SlotItem*)m_SlotList[i];
		if ( pSlotItem == NULL )
			continue;

		if ( pSlotItem->GetSlotType() == slotType )
		{
			for ( int j = 0; j < (int)vecKBillOrderInfo.size(); j++ )
			{
				KBillOrderInfo& kKBillOrderInfo = vecKBillOrderInfo[j];

				if ( pSlotItem->GetSlotID() == j )
				{
					pSlotItem->SetEnable( true );
					pSlotItem->SetShow( true );
					pSlotItem->SetKBillOrderInfo( kKBillOrderInfo );
					pSlotItem->CreateItemUI( kKBillOrderInfo.m_kBillProductInfo.m_iProductID, (int)kKBillOrderInfo.m_kBillProductInfo.m_cQuantity );
					break;
				}
			}
		}
	}
}

/** @function : DestroyItemSlotByType
	@brief : ���ڷ� ���޵� ������ Ÿ�԰� ���� ���Ե��� ������� �ʵ��� ��
	@param : ����Ÿ��(eSlotType_)
*/
void CX2CashShop::DestroyItemSlotByType( const CX2Slot::SLOT_TYPE eSlotType_ )
{
	for ( UINT i = 0; i < m_SlotList.size(); i++ )
	{
		CX2SlotItem* pSlotItem = static_cast<CX2SlotItem*>( m_SlotList[i] );
		if ( pSlotItem != NULL && pSlotItem->GetSlotType() == eSlotType_ )
		{
			pSlotItem->SetEnable( false );
			pSlotItem->SetShow( false );
			pSlotItem->DestroyItemUI();
		}
	}
}

void CX2CashShop::ResetMainPageNumUI()
{
	if ( m_pDLGMainPage != NULL )
	{
		CKTDGUIStatic* pStaticNewItemPage = (CKTDGUIStatic*)m_pDLGMainPage->GetControl( L"g_pStaticNewitem_Font" );
		if ( pStaticNewItemPage != NULL )
		{
			pStaticNewItemPage->SetString( 0, L"" );
			wstringstream wstrstm;
			wstrstm << m_pMainNewSM->GetNowPage() << L"/" << m_pMainNewSM->GetMaxPage();
			pStaticNewItemPage->SetString( 0, wstrstm.str().c_str() );
		}
		CKTDGUIStatic* pStaticHotItemPage = (CKTDGUIStatic*)m_pDLGMainPage->GetControl( L"g_pStaticHotitem_Font" );
		if ( pStaticHotItemPage != NULL )
		{
			pStaticHotItemPage->SetString( 0, L"" );
			wstringstream wstrstm;
			wstrstm << m_pMainHotSM->GetNowPage() << L"/" << m_pMainHotSM->GetMaxPage();
			pStaticHotItemPage->SetString( 0, wstrstm.str().c_str() );
		}
		CKTDGUIStatic* pStaticBestItemPage = (CKTDGUIStatic*)m_pDLGMainPage->GetControl( L"g_pStaticBestitem_Font" );
		if ( pStaticBestItemPage != NULL )
		{
			pStaticBestItemPage->SetString( 0, L"" );
			wstringstream wstrstm;
			wstrstm << m_pMainRecommendSM->GetNowPage() << L"/" << m_pMainRecommendSM->GetMaxPage();
			pStaticBestItemPage->SetString( 0, wstrstm.str().c_str() );
		}
	}
}

void CX2CashShop::ResetSubPageNumUI()
{
	if ( m_pDLGSubPage != NULL )
	{
		CKTDGUIStatic* pStaticPage = (CKTDGUIStatic*)m_pDLGSubPage->GetControl( L"g_pStaticCashshop_Sub_Window_TriangleFont" );
		if ( pStaticPage != NULL )
		{
			pStaticPage->SetString( 0, L"" );
			wstringstream wstrstm;
			wstrstm << m_pSubSM->GetNowPage() << L"/" << m_pSubSM->GetMaxPage();
			pStaticPage->SetString( 0, wstrstm.str().c_str() );
		}
	}
}

int CX2CashShop::GetItemID( CKTDGUIDialogType pDialog )
{
	int itemID = 0;

	if ( pDialog == NULL )
		return itemID;


	itemID = m_pMainNewSM->GetItemID( pDialog );
	if ( itemID != 0 )
		return itemID;

	itemID = m_pMainHotSM->GetItemID( pDialog );
	if ( itemID != 0 )
		return itemID;

	itemID = m_pMainRecommendSM->GetItemID( pDialog );
	if ( itemID != 0 )
		return itemID;

	itemID = m_pSubSM->GetItemID( pDialog );
	if ( itemID != 0 )
		return itemID;

	itemID = m_pSubSpeicialSM->GetItemID( pDialog );
	if ( itemID != 0 )
		return itemID;

	return itemID;
}

void CX2CashShop::UnitHandling( double fTime, float fElapsedTime )
{
	MyUniyHandling(fTime, fElapsedTime); //���� ĳ���� ���
	PetHandling(fTime, fElapsedTime);	 //�� ���
}

bool CX2CashShop::IsInUnitClickReact( D3DXVECTOR2 mousePos )
{
	D3DXVECTOR2 m_ConvertPos		= g_pKTDXApp->ConvertByResolution( m_UnitClickPos );
	D3DXVECTOR2 m_ConvertSize	= g_pKTDXApp->ConvertSizeByResolution( m_UnitClickSize );

	if ( mousePos.x >= m_ConvertPos.x   && mousePos.x <= m_ConvertPos.x + m_ConvertSize.x &&
		mousePos.y >= m_ConvertPos.y && mousePos.y <= m_ConvertPos.y + m_ConvertSize.y )
	{
		return true;
	}

	return false;
}

vector<int> CX2CashShop::GetItemByCategory( CASH_SHOP_CATEGORY categoryID )
{
	vector< int > vecAllItem = g_pData->GetItemManager()->GetAllCashItemList();
	vector< int > vecItem;

	for ( int cateIndex = 0; cateIndex < (int)m_vecCashShopCateList.size(); cateIndex++ )
	{
		CX2CashShop::CashShopCategory* pCashShopCategory = m_vecCashShopCateList[cateIndex];
		if ( pCashShopCategory != NULL )
		{
			if ( (int)pCashShopCategory->m_CategoryID.x == (int)categoryID )
			{
				for ( int i = 0; i < (int)vecAllItem.size(); i++ )
				{
					int itemID = vecAllItem[i];

					if ( m_bAllCharItem == false && IsPossibleUsedByMyCharacter(itemID) == false )
						continue;

					//{{ kimhc // 2009-08-10 // �ڽ��� ���� ȸ�� ��޿� ���� ������ ������ �ʵ��� ��
					if ( IsPossibleUsedByMyBankMembership( itemID ) == false )
						continue;
					//}} kimhc // 2009-08-10 // �ڽ��� ���� ȸ�� ��޿� ���� ������ ������ �ʵ��� ��


					int iClassChangeItemLevel = GetClassChangeItemLevel( itemID );
					if( iClassChangeItemLevel > 0 &&															// ���� ĳ�� �������ε�
						1 != iClassChangeItemLevel - g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )	// ���� ĳ���� Ŭ������ ����� �� ���� �������̸�
						continue;

					if( itemID == UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID
						&& 2 != g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )						  //2�� ���� �ƴϸ� ������ �ʵ��� ��
						continue;


					if( itemID == CASH_SKILL_NOTE_ITEM_ID && GetCanByeSkillNoteItem( itemID ) == false )
					{
						continue;
					}

					if( itemID == SHARING_BANK_OPEN_CASH_ITEM_ID && GetCahBuySharingBankItem( itemID ) == false )
					{
						continue;
					}

					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
					if( pItemTemplet == NULL )
						continue;

					int iUnitClassChangeItemLevel = GetUnitClassChangeItemLevel( itemID );
					int iMyClassLevel = g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel();
					int iItemUnitType = pItemTemplet->GetUnitType();
					int iUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
					int iItemUnitClass = pItemTemplet->GetUnitClass();
					int iUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();

					if ( (iUnitClassChangeItemLevel > 0 && iItemUnitType != iUnitType) || iUnitClassChangeItemLevel > iMyClassLevel )
						continue;

					if ( iUnitClassChangeItemLevel > 0 && iUnitClassChangeItemLevel < iMyClassLevel )
						continue;

					if ( iUnitClassChangeItemLevel > 0 && iItemUnitClass == iUnitClass )
						continue;

					if( true== IsCanByeOnlyAraItem(itemID) && //�ƶ� �� �� �ִ� �������ε�
						CX2Unit::UT_ARA != iUnitType ) //���� �ƶ� �ƴ϶��
					{
						continue;
					}

					if( itemID == EXPAND_QUICK_SLOT_ITEM_ID && GetCahBuyExpandQuickSlotItem( itemID ) == false )
					{
						continue;
					}

					//{{ ����� : [2012/12/5] //		Ư�� ������ ������ �־�� ĳ�ü����� ���� ����
					if( g_pData != NULL )
					{
						if( g_pData->GetItemManager() != NULL )
						{
							bool bContinue = false;
							std::vector<int> TempItemID;
							TempItemID.clear();
							if( g_pData->GetItemManager()->CheckKeepShowItem( itemID , TempItemID ) == true )
							{
								for( int i = 0; i < static_cast<int>( TempItemID.size() ); ++i )
								{
									if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( TempItemID[i] ) == NULL )
									{
										bContinue = true;
										break;
									}
								}
								if( bContinue == true)
								{
									continue;
								}
							}
						}
					}
						//}} ����� : [2012/12/5] //		Ư�� ������ ������ �־�� ĳ�ü����� ���� ����



					/// ���� �������ε�, �䱸 Ŭ���� ������ ���� ���� Ŭ���� �������� ���ٸ�
					if( NULL != g_pData &&
						NULL != g_pData->GetSkillTree() &&
						true == g_pData->GetSkillTree()->IsUnsealSkillItemID( itemID ) &&
						iMyClassLevel < g_pData->GetSkillTree()->GetUnsealSkillItemRequireClass( itemID ) )
					{
						continue;
					}

					CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
					if ( pCashItem != NULL )
					{
						// ù��° ������ ī�װ����� �������� ����Ѵ�.
						// �̰� ��� ��ǰ�� ī�װ����� �� ������ ����
						const KBillProductInfo* pkKBillProductInfo = pCashItem->GetGateProduct();
						if ( pkKBillProductInfo != NULL )
						{
							for ( int j = 0; j < (int)pCashShopCategory->m_vecSubCategoryID.size(); j++ )
							{
								D3DXVECTOR2 subCateID = pCashShopCategory->m_vecSubCategoryID[j];

								if ( (int)pkKBillProductInfo->m_cCategoryNo == (int)subCateID.y )
								{
									vecItem.push_back( pCashItem->m_ItemID );
								}
							}
						}
					}
				}

				break;
			}
		}
	}


	return vecItem;
}




vector<int> CX2CashShop::GetItemByCategory( CASH_SHOP_CATEGORY categoryID, CASH_SHOP_SUB_CATEGORY subCategoryID )
{
	vector< int > vecAllItem = g_pData->GetItemManager()->GetAllCashItemList();
	vector< int > vecItem;

	for ( int cateIndex = 0; cateIndex < (int)m_vecCashShopCateList.size(); cateIndex++ )
	{
		CX2CashShop::CashShopCategory* pCashShopCategory = m_vecCashShopCateList[cateIndex];
		if ( pCashShopCategory != NULL )
		{
			if ( (int)pCashShopCategory->m_CategoryID.x == (int)categoryID )
			{
				for ( int i = 0; i < (int)vecAllItem.size(); i++ )
				{
					int itemID = vecAllItem[i];

					if ( m_bAllCharItem == false && IsPossibleUsedByMyCharacter(itemID) == false )
						continue;

					//{{ kimhc // 2009-08-10 // �ڽ��� ���� ȸ�� ��޿� ���� ������ ������ �ʵ��� ��
					if ( IsPossibleUsedByMyBankMembership( itemID ) == false )
						continue;
					//}} kimhc // 2009-08-10 // �ڽ��� ���� ȸ�� ��޿� ���� ������ ������ �ʵ��� ��


					int iClassChangeItemLevel = GetClassChangeItemLevel( itemID );
					if( iClassChangeItemLevel > 0 &&															// ���� ĳ�� �������ε�
						1 != iClassChangeItemLevel - g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )	// ���� ĳ���� Ŭ������ ����� �� ���� �������̸�
						continue;

					if( itemID == UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID
						&& 2 != g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )						  //2�� ���� �ƴϸ� ������ �ʵ��� ��
						continue;


					if( itemID == CASH_SKILL_NOTE_ITEM_ID && GetCanByeSkillNoteItem( itemID ) == false )
					{
						continue;
					}

					if( itemID == SHARING_BANK_OPEN_CASH_ITEM_ID && GetCahBuySharingBankItem( itemID ) == false )
					{
						continue;
					}

					int iUnitClassChangeItemLevel = GetUnitClassChangeItemLevel( itemID );
					int iMyClassLevel = g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel();
					int iItemUnitType = g_pData->GetItemManager()->GetItemTemplet( itemID )->GetUnitType();
					int iUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
					int iItemUnitClass = g_pData->GetItemManager()->GetItemTemplet( itemID )->GetUnitClass();
					int iUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();

					if ( (iUnitClassChangeItemLevel > 0 && iItemUnitType != iUnitType) || iUnitClassChangeItemLevel > iMyClassLevel )
						continue;

					if ( iUnitClassChangeItemLevel > 0 && iUnitClassChangeItemLevel < iMyClassLevel )
						continue;

					if ( iUnitClassChangeItemLevel > 0 && iItemUnitClass == iUnitClass )
						continue;
					if( itemID == EXPAND_QUICK_SLOT_ITEM_ID && GetCahBuyExpandQuickSlotItem( itemID ) == false )
					{
						continue;
					}

					if( g_pData != NULL )
					{
						if( g_pData->GetItemManager() != NULL )
						{
							bool bContinue = false;
							std::vector<int> TempItemID;
							TempItemID.clear();
							if( g_pData->GetItemManager()->CheckKeepShowItem( itemID , TempItemID ) == true )
							{
								for( int i = 0; i < static_cast<int>( TempItemID.size() ); ++i )
								{
									if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( TempItemID[i] ) == NULL )
									{
										bContinue = true;
										break;
									}
								}
								if( bContinue == true)
								{
									continue;
								}
							}
						}
					}



					CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
					if ( pCashItem != NULL )
					{
						const KBillProductInfo* pkKBillProductInfo = pCashItem->GetGateProduct();
						if ( pkKBillProductInfo != NULL )
						{
							for ( int j = 0; j < (int)pCashShopCategory->m_vecSubCategoryID.size(); j++ )
							{
								D3DXVECTOR2 subCateID = pCashShopCategory->m_vecSubCategoryID[j];

								if ( (int)subCateID.x == (int)subCategoryID )
								{
									if ( (int)pkKBillProductInfo->m_cCategoryNo == (int)subCateID.y )
									{
										vecItem.push_back( pCashItem->m_ItemID );
									}
								}
							}
						}
					}
				}

				break;
			}
		}
	}

	return vecItem;
}

void CX2CashShop::ChangeCategory( CKTDGUIRadioButton* pRadioButton )
{
	if ( pRadioButton == NULL )
		return;

	if ( wcscmp( pRadioButton->GetName(), L"tab_main" ) == 0 )
	{
		SetCashShopState( CX2CashShop::CSS_MAIN );
	}
	else
	{
		SetCashShopState( CX2CashShop::CSS_SUB, (CX2CashShop::CASH_SHOP_CATEGORY)pRadioButton->GetDummyInt(0) );
	}
}
void CX2CashShop::ChangeSubCategory( CX2CashShop::CASH_SHOP_SUB_CATEGORY eSubCategory )
{
	m_CashShopSubCategory = eSubCategory;

	vector< int > vecItemID;
	if ( m_CashShopSubCategory == CX2CashShop::CSSC_ALL )
	{
		//���� ���õ� ī�װ����� ��� ��ǰ�� ��������
		vecItemID = GetItemByCategory(m_CashShopCategory);
	}
	else
	{
		vecItemID = GetItemByCategory(m_CashShopCategory, m_CashShopSubCategory );
	}

	//{{ kimhc // 2009-10-12 // �ڵ����� �� 'ĳ��' ��Ʈ���� '��' ���� ��ħ
	m_pSubSM->Reset( vecItemID );
	//}} kimhc // 2009-10-12 // �ڵ����� �� 'ĳ��' ��Ʈ���� '��' ���� ��ħ

	m_pSubSM->SetShowPresentButton();


	ResetItemSlot(  CX2Slot::ST_CASH_SHOP_SUB,			m_pSubSM->GetNowShowItemIDList() );

	m_pSubSpeicialSM->Reset( GetHotItemList( vecItemID, CASH_SHOP_SUB_SPECIAL_SLOT_NUM ) );
	ResetItemSlot(  CX2Slot::ST_CASH_SHOP_SUB_SPECIAL,			m_pSubSpeicialSM->GetNowShowItemIDList() );

	ResetSubPageNumUI();
}
void CX2CashShop::ChangeSubCategory( CKTDGUIRadioButton* pRadioButton )
{
	if ( pRadioButton == NULL )
		return;

	ChangeSubCategory ( static_cast<CX2CashShop::CASH_SHOP_SUB_CATEGORY>(pRadioButton->GetDummyInt(0)) );
}

/*static*/ bool CX2CashShop::IsPossibleUsedByMyCharacter( int itemID )
{
	return CX2Unit::CanUse( itemID, g_pData->GetMyUser()->GetSelectUnit() );
}

vector<int> CX2CashShop::GetHotItemList( vector<int>& vecItemID, int maxItemNum )
{
	vector< int > vecHotItemID;
	vector< int > vecTempHotCashItem =  g_pData->GetItemManager()->GetHotCashItemList();

	for ( int i = 0; i < (int)vecTempHotCashItem.size(); i++ )
	{
		int itemID = vecTempHotCashItem[i];

		CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
		if ( pCashItem == NULL )
			continue;

		if ( pCashItem->GetIsHot() == true )
		{
			if ( std::find(vecItemID.begin(), vecItemID.end(), itemID) != vecItemID.end() )
			{
				vecHotItemID.push_back(itemID);
				if ( (int)vecHotItemID.size() >= maxItemNum )
				{
					break;
				}
			}
		}
	}

	std::sort( vecHotItemID.begin(), vecHotItemID.end(), CashItemSorter() );

	return vecHotItemID;
}

bool CX2CashShop::CheckIsPossibleEqiupSlot( CX2Slot::SLOT_TYPE slotType )
{
	if ( slotType == CX2SlotManager::CX2Slot::ST_CASH_SHOP_MAIN_HOT ||
		slotType == CX2SlotManager::CX2Slot::ST_CASH_SHOP_MAIN_NEW ||
		slotType == CX2SlotManager::CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND ||
		slotType == CX2SlotManager::CX2Slot::ST_CASH_SHOP_SUB ||
		slotType == CX2SlotManager::CX2Slot::ST_CASH_SHOP_SUB_SPECIAL )
	{
		return true;
	}

	return false;
}

bool CX2CashShop::CheckPreEqipItem( int itemTID )
{
	return std::find( m_vecPreEquipItemID.begin(), m_vecPreEquipItemID.end(), itemTID ) != m_vecPreEquipItemID.end();
}

void CX2CashShop::RemovePreEquipItemByEquipPos( int itemTID )
{
	const CX2Item::ItemTemplet* pItemTempletSrc =
		g_pData->GetItemManager()->GetItemTemplet( itemTID );

	if ( pItemTempletSrc == NULL )
		return;


	std::vector< int >::iterator iS;
	for ( iS = m_vecPreEquipItemID.begin(); iS != m_vecPreEquipItemID.end(); iS++ )
	{
		int iCuritemTID = (*iS);
        const CX2Item::ItemTemplet* pItemTemplate =
			g_pData->GetItemManager()->GetItemTemplet( iCuritemTID );

		if ( pItemTemplate->GetEqipPosition() == pItemTempletSrc->GetEqipPosition() &&
			( pItemTemplate->GetItemType() == CX2Item::IT_ACCESSORY || pItemTemplate->GetFashion() == pItemTempletSrc->GetFashion() ))
		{
			m_vecPreEquipItemID.erase( iS );
			return;
		}
	}
}

bool CX2CashShop::InsertPreEquipItem( int itemTID, bool bForce )
{
	// �ɸ��� ������ �´��� üũ
    const CX2Item::ItemTemplet* pItemTemplate = g_pData->GetItemManager()->GetItemTemplet( (int) itemTID );
	if ( pItemTemplate == NULL )
		return false;

	if (g_pData->GetMyUser()->GetSelectUnit()->GetType() != pItemTemplate->GetUnitType()
		&& pItemTemplate->GetUseCondition() != CX2Item::UC_ANYONE )
		return false;



	if ( bForce == true )
	{
		RemovePreEquipItemByEquipPos( itemTID );

		m_vecPreEquipItemID.push_back( itemTID );
	}
	else
	{
		// �ٸ� �������� ��� �߰�
		if ( CheckPreEqipItem( itemTID ) == false )
		{
			RemovePreEquipItemByEquipPos( itemTID );

			m_vecPreEquipItemID.push_back( itemTID );
		}
		// ���� �������� ��� ����
		else
		{
			RemovePreEquipItemByEquipPos( itemTID );
		}
	}


	// Unit Viewer
	PreEquipping();

	return true;
}

void CX2CashShop::PreEquipping()
{
	m_pUnitViewerUI->UpdateEqip( true );

	for ( unsigned int iT = 0; iT < m_vecPreEquipItemID.size(); ++iT )
	{
		int iItemTID = m_vecPreEquipItemID[iT];
		m_pUnitViewerUI->AddEquipByTID( iItemTID );
	}
}

void CX2CashShop::OnRevertItem( int iItemTID )
{
	RemovePreEquipItemByEquipPos( iItemTID );

	ResetNowEquipUI();

	PreEquipping();
}

void CX2CashShop::ResetNowEquipUI()
{
	const CX2Unit::UnitData& kMyUnitData = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

	// ���� �������� ������ ���� �ʱ�ȭ
	ClearNowEquipUI();

	// �� �κ��丮�� �°� ������Ʈ
	ResetNowEquipUIByInven(kMyUnitData, kInventory);

	// �� �̸����� ������ �°� ������Ʈ
	ResetNowEquipUIByShopEquip();

	// ���� ���� ǥ�� ������Ʈ
	UpdateShopEquipedPicture();

	ResetSellCashItemByPreEquip();
}

void CX2CashShop::ClearNowEquipUI()
{
	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlot( i );
		if ( pItemSlot != NULL && pItemSlot->GetSlotType() == CX2ItemSlotManager::CX2Slot::ST_EQUIPPED )
		{
			/*
			CKTDGUIDialogType pDialog = pItemSlot->GetDialog();
			SAFE_DELETE_DIALOG( pDialog );
			pItemSlot->SetDialog( NULL );
			*/

				pItemSlot->LostItemUI();
		}
	}
}

void CX2CashShop::ResetNowEquipUIByInven( const CX2Unit::UnitData& kMyUnitData, const CX2Inventory& kInventory)
{
	if( false == m_bShowInvenEquip )
		return;

	{
		for ( int i = 0; i < (int)kMyUnitData.m_NowEqipItemUIDList.size(); i++ )
		{
			UidType uidType = kMyUnitData.m_NowEqipItemUIDList[i];
			CX2Item* pItem = kInventory.GetItem( uidType );
			if ( pItem == NULL )
				continue;

			for ( int j = 0; j < (int)m_SlotList.size(); j++ )
			{
				CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlot( j );
				if ( pItemSlot->GetSlotType() == CX2ItemSlotManager::CX2Slot::ST_EQUIPPED &&
					pItemSlot->GetEquipPos() == pItem->GetItemTemplet()->GetEqipPosition() &&
					( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItem->GetItemTemplet()->GetFashion() ) )	// 09.06.02 �¿� : �м� �Ǽ��縮 ���� ����ó��
				{
					//pItemSlot->CreateItemUI( pItem );
					if ( pItemSlot->IsCreatedItemUI() == true )
						pItemSlot->ResetItemUI( pItem );
					else
						pItemSlot->CreateItemUI( pItem );

					AttachShopEquipedPicture( pItemSlot );
					break;
				}
			}
		}
	}
}

void CX2CashShop::ResetNowEquipUIByShopEquip()
{

	for ( int iS = 0; iS < (int)m_vecPreEquipItemID.size(); ++iS )
	{
		int itemTID = m_vecPreEquipItemID[iS];
        const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemTID );

		for ( int j = 0; j < (int)m_SlotList.size(); j++ )
		{
			CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlot( j );
			if ( pItemSlot->GetSlotType() == CX2ItemSlotManager::CX2Slot::ST_EQUIPPED &&
				pItemSlot->GetEquipPos() == pItemTemplet->GetEqipPosition() &&
				( pItemTemplet->GetItemType() == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == pItemTemplet->GetFashion() ) )
			{
				if ( pItemSlot->IsCreatedItemUI() == true )
					pItemSlot->ResetItemUI( itemTID, -1 );
				else
					pItemSlot->CreateItemUI( itemTID, -1 );

				AttachShopEquipedPicture( pItemSlot );
				break;
			}
		}
	}
}

void CX2CashShop::ResetSellCashItemByPreEquip()
{
	for ( int iS = 0; iS < (int)m_vecPreEquipItemID.size(); ++iS )
	{
		//CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemTID );

		for ( int j = 0; j < (int)m_SlotList.size(); j++ )
		{
			CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlot( j );
			if ( CheckIsPossibleEqiupSlot( pItemSlot->GetSlotType() ) == true ||
				pItemSlot->GetSlotType() == CX2Slot::ST_CASH_SHOP_CHOICED )
			{
				if ( pItemSlot->IsResetItemUI() == true )
					AttachShopEquipedPicture( pItemSlot );
			}
		}
	}

	for ( int j = 0; j < (int)m_SlotList.size(); j++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlot( j );
		if ( CheckIsPossibleEqiupSlot( pItemSlot->GetSlotType() ) == true ||
			pItemSlot->GetSlotType() == CX2Slot::ST_CASH_SHOP_CHOICED )
		{
			SetShowShopEquipedPicture( pItemSlot, false );
			if ( CheckPreEqipItem( pItemSlot->GetItemTID() ) == true )
			{
				SetShowShopEquipedPicture( pItemSlot, true );
			}
		}
	}
}

void CX2CashShop::UpdateShopEquipedPicture()
{
	for ( int j = 0; j < (int)m_SlotList.size(); j++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlot( j );
		if ( pItemSlot->GetSlotType() == CX2SlotManager::CX2Slot::ST_EQUIPPED)
		{
			SetShowShopEquipedPicture( pItemSlot, false );
			if ( CheckPreEqipItem( pItemSlot->GetItemTID() ) == true )
			{
				SetShowShopEquipedPicture( pItemSlot, true );
			}
		}
	}
}

void CX2CashShop::AttachShopEquipedPicture( CX2SlotItem* pItemSlot )
{
	if( NULL == pItemSlot )
		return;



	CKTDGUIDialogType pkDialog = pItemSlot->GetDialog();
	if ( pkDialog == NULL )
		return;

	CKTDGUIStatic* pStaticEquipedBorder = NULL;
	if ( pkDialog->CheckControl( L"StaticEquipedBorder" ) == true )
	{
		pStaticEquipedBorder = (CKTDGUIStatic*)pkDialog->GetControl( L"StaticEquipedBorder" );
	}

	CKTDGUIStatic* pStaticEquipedBorder2 = NULL;
	if ( pkDialog->CheckControl( L"StaticEquipedBorder2" ) == true )
	{
		pStaticEquipedBorder2 = (CKTDGUIStatic*)pkDialog->GetControl( L"StaticEquipedBorder2" );
	}

	if ( pStaticEquipedBorder == NULL )
	{
		pStaticEquipedBorder = new CKTDGUIStatic();
		pStaticEquipedBorder->SetShow( false );
		pStaticEquipedBorder->SetName( L"StaticEquipedBorder" );
		pkDialog->AddControl( pStaticEquipedBorder );
		pkDialog->ChangeSequence( pStaticEquipedBorder, false );

		CKTDGUIControl::CPictureData* pPictureEquiped = new CKTDGUIControl::CPictureData();
		pPictureEquiped->SetTex( L"HQ_ShopEqipedBorder2.tga" );
		pPictureEquiped->SetPoint( new CKTDGUIControl::UIPointData );

		//{{ 2009.01.19 ���¿� : �ڵ� ����
		switch(pItemSlot->GetSlotType())
		{
		case CX2Slot::ST_EQUIPPED:
		case CX2Slot::ST_CASH_SHOP_SUB_SPECIAL:
		case CX2Slot::ST_CASH_SHOP_CHOICED:
			{
				pPictureEquiped->SetPos( D3DXVECTOR2( -1, -1 ) );

				// ���� �Ǽ��縮 ������ ũ�Ⱑ �ٸ� ���Ժ��� �۾Ƽ� ���� ó��
				pPictureEquiped->SetSize( pItemSlot->GetSize() + D3DXVECTOR2( 2, 2 ) );
				//if( CX2Unit::EP_AC_WEAPON == pItemSlot->m_EquipPos )
				//{
				//	pPictureEquiped->SetSize( D3DXVECTOR2( 43, 43 ) );
				//}
				//else
				//{
				//	pPictureEquiped->SetSize( D3DXVECTOR2( 34, 34 ) );
				//}
			} break;
		case CX2Slot::ST_CASH_SHOP_MAIN_NEW:
		case CX2Slot::ST_CASH_SHOP_MAIN_HOT:
		case CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND:
		case CX2Slot::ST_CASH_SHOP_SUB:
			{
				pPictureEquiped->SetPos( D3DXVECTOR2( -3, -3 ) );
				pPictureEquiped->SetSize( D3DXVECTOR2( 71 + 6, 71 + 6 ) );

				if ( pStaticEquipedBorder2 == NULL )
				{
					pStaticEquipedBorder2 = new CKTDGUIStatic();
					pStaticEquipedBorder2->SetShow( false );
					pStaticEquipedBorder2->SetName( L"StaticEquipedBorder2" );
					pkDialog->AddControl( pStaticEquipedBorder2 );
					pkDialog->ChangeSequence( pStaticEquipedBorder2, true );

					CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
					pStaticEquipedBorder2->AddString( pString );
					pString->fontStyle = CKTDGFontManager::FS_SHELL;
					pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
					pString->color = D3DXCOLOR( 1,1,0,1 );
					pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
					pString->msg = GET_STRING( STR_ID_31 );
					pString->sortFlag = DT_CENTER;
					pString->pos = D3DXVECTOR2( pItemSlot->GetSize().x / 2.0f + 2, pItemSlot->GetSize().y - 20 );
				}
			}break;
		default:
			break;
		}
		//}}
		pStaticEquipedBorder->AddPictureAt( 0, pPictureEquiped );
	}

}

void CX2CashShop::SetShowShopEquipedPicture( CX2SlotItem* pItemSlot, bool bShow )
{
	CKTDGUIDialogType pkDialog = pItemSlot->GetDialog();
	if ( pkDialog == NULL )
		return;

	CKTDGUIStatic* pStaticEquipedBorder = NULL;
	if ( pkDialog->CheckControl( L"StaticEquipedBorder" ) == true )
	{
		pStaticEquipedBorder = (CKTDGUIStatic*)pkDialog->GetControl( L"StaticEquipedBorder" );
	}

	if ( pStaticEquipedBorder != NULL )
		pStaticEquipedBorder->SetShow( bShow );

	CKTDGUIStatic* pStaticEquipedBorder2 = NULL;
	if ( pkDialog->CheckControl( L"StaticEquipedBorder2" ) == true )
	{
		pStaticEquipedBorder2 = (CKTDGUIStatic*)pkDialog->GetControl( L"StaticEquipedBorder2" );
	}

	if ( pStaticEquipedBorder2 != NULL )
		pStaticEquipedBorder2->SetShow( bShow );
}

void CX2CashShop::AddChoicedItem( int itemID )
{

	if ( std::find( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), itemID ) != m_vecChoicedItemID.end() )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_32 ), g_pMain->GetNowState() );
		return;
	}

	if ( (int)m_vecChoicedItemID.size() >= CASH_SHOP_CHOICED_SLOT_NUM )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_33 ), g_pMain->GetNowState() );
		return;
	}

	vector< int > vecItemID;
	vecItemID.push_back( itemID );
	Handler_EGS_MODIFY_WISH_LIST_REQ( true, vecItemID );
}

void CX2CashShop::RemoveChoicedItem( int itemID )
{
	/*
	if ( std::find( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), itemID ) != m_vecChoicedItemID.end() )
	{
		//�ϴ� ã�Ұ�..
		m_vecChoicedItemID.erase( std::remove( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), itemID ), m_vecChoicedItemID.end() );
		ResetItemSlot( CX2Slot::ST_CASH_SHOP_CHOICED, m_vecChoicedItemID );
	}
	*/

		vector< int > vecItemID;
	vecItemID.push_back( itemID );
	Handler_EGS_MODIFY_WISH_LIST_REQ( false, vecItemID );
}

void CX2CashShop::SetEnableBuyEquippingItem( bool bEnable )
{
	if ( m_pDLGFront != NULL )
	{
		CKTDGUIControl* pControl = m_pDLGFront->GetControl(L"EquippingAllBuy");
		if ( pControl != NULL )
			pControl->SetEnable( bEnable );
	}
}
bool CX2CashShop::IsPossiblePresent( int itemID )
{
	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_Level <= 20)
	{
		return false;
	}

	CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );

	if(pCashItem == NULL || pCashItem->m_vecKBillProductInfo.size() < 1)
	{
		return false;
	}

	bool bEnableGift = false;

	for(std::vector<KBillProductInfo>::iterator vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); ++vit)
	{
		bEnableGift = (bEnableGift || vit->m_bEnableGift);
	}

	return bEnableGift;
}

void CX2CashShop::ProcessBuyPopUpOk()
{
	vector<unsigned long> vecProductNoList;
	if ( m_pBuyItemPopUp->GetCheckedProductNoList(vecProductNoList) == true )
	{

	//{{ ����� 2010.11.1
			if ( m_pBuyItemPopUp->RestCashCheck() == true )
			{
				if ( Handler_EGS_BUY_CASH_ITEM_REQ( vecProductNoList, m_pBuyItemPopUp->GetChoicedItemBuy() ) == true )
					m_pBuyItemPopUp->SetOpen( false );
			}
			else
			{
				m_pBuyItemPopUp->SetOpen( false );
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), NetError::GetErrStrF(NetError::ERR_BUY_CASH_ITEM_16) , g_pMain->GetNowState() );
			}
//}} ����� 2010.11.1
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_7 ), g_pMain->GetNowState() );
	}
}

//{{ kimhc // 2009-10-06 // �ڵ����� ó��
//}} kimhc // 2009-10-06 // �ڵ����� ó��

//{{ kimhc // 2009-08-10 // �ڽ��� ���� ȸ�� ��޿� ���� ������ ������ �ʵ��� ��
bool CX2CashShop::IsPossibleUsedByMyBankMembership( int itemID ) const
{
	// ���� �������� �ƴϸ� true�� ����
	switch ( itemID )
	{
	case SILVER_MEMBERSHIP_CARD:
	case GOLD_MEMBERSHIP_CARD:
	case EMERALD_MEMBERSHIP_CARD:
	case DIAMOND_MEMBERSHIP_CARD:
	case PLATINUM_MEMBERSHIP_CARD:
	case COBO_CREDIT_CARD:
		break;

	default:
		return true;
	}

	if ( g_pData->GetMyUser()	!= NULL &&	g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Inventory&	kInventory	= g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		CX2Inventory::MEMBERSHIP_PRIVATE_BANK	eMembershipGrade	= CX2Inventory::MPB_NORMAL;
		eMembershipGrade	= static_cast< CX2Inventory::MEMBERSHIP_PRIVATE_BANK >( kInventory.GetItemMaxNum( CX2Inventory::ST_BANK ) );

		switch ( eMembershipGrade )
		{
		case CX2Inventory::MPB_NORMAL:
			{
				if ( itemID	== SILVER_MEMBERSHIP_CARD )
					return true;
				else if ( itemID == COBO_CREDIT_CARD
					&&  kInventory.GetItemByTID( _CONST_UI_PRIVATE_BANK_::g_iBankQuestItemID, true ) == NULL )
					return true;
			}
			break;

		case CX2Inventory::MPB_SILVER:
			{
				if ( itemID	== GOLD_MEMBERSHIP_CARD )
					return true;
			}
			break;

		case CX2Inventory::MPB_GOLD:
			{
				if ( itemID	== EMERALD_MEMBERSHIP_CARD )
					return true;
			}
			break;

		case CX2Inventory::MPB_EMERALD:
			{
				if ( itemID	== DIAMOND_MEMBERSHIP_CARD )
					return true;
			}
			break;

		case CX2Inventory::MPB_DIAMOND:
			{
				if ( itemID	== PLATINUM_MEMBERSHIP_CARD
					&& kInventory.GetItemByTID( _CONST_UI_PRIVATE_BANK_::g_iBankQuestItemID, true ) == NULL )
					return true;
			}
			break;

		case CX2Inventory::MPB_PLATINUM:
			{
				return false;
			}
			break;
		}
	}

	return false;
}
//}} kimhc // 2009-08-10 // �ڽ��� ���� ȸ�� ��޿� ���� ������ ������ �ʵ��� ��


/*static*/ int CX2CashShop::GetClassChangeItemLevel( int itemID )
{
	switch( itemID )
	{
	case CLASS_CHANGE_ELSWORD_KNIGHT_ITEM_ID:
	case CLASS_CHANGE_MAGIC_KNIGHT_ITEM_ID:
	case CLASS_CHANGE_SHEATH_KNIGHT_ITEM_ID:
	case CLASS_CHANGE_COMBAT_RANGER_ITEM_ID:
	case CLASS_CHANGE_SNIPING_RANGER_ITEM_ID:
	case CLASS_CHANGE_TRAPPING_RANGER_ITEM_ID:
	case CLASS_CHANGE_HIGH_MAGICIAN_ITEM_ID:
	case CLASS_CHANGE_DARK_MAGICIAN_ITEM_ID:
	case CLASS_CHANGE_BATTLE_MAGICIAN_ITEM_ID:
	case CLASS_CHANGE_SOUL_TAKER_ITEM_ID:
	case CLASS_CHANGE_OVER_TAKER_ITEM_ID:
	case CLASS_CHANGE_WEAPON_TAKER_ITEM_ID:
	case CLASS_CHANGE_EXOTIC_GEAR_ITEM_ID:
	case CLASS_CHANGE_ARCHITECTURE_ITEM_ID:
	case CLASS_CHANGE_ELECTRA_ITEM_ID:
		//{{ kimhc // 2011.1.26 // û 1�� ����
	case CLASS_CHANGE_FURY_GUARDIAN_ID:
	case CLASS_CHANGE_SHOOTER_GUARDIAN_ID:
	case CLASS_CHANGE_SHELLING_GUARDIAN_ID:
		//}} kimhc // 2011.1.26 // û 1�� ����
	case CLASS_CHANGE_LITTLE_HSIEN_ID:
	case CLASS_CHANGE_SABER_KNIGHT_ID:
	case CLASS_CHANGE_PYRO_KNIGHT_ID:
	case CLASS_CHANGE_LITTLE_DEVIL_ID:
	case CLASS_CHANGE_ADD_PSYCHIC_TRACER_ID:
		{
			return 1;
		} break;

	case CLASS_CHANGE_LORD_NIGHT_ITEM_ID:	// �ε� ����Ʈ �Ӹ���
	case CLASS_CHANGE_RUNE_SLAYER_ITEM_ID:	// �� �����̾� ¡ǥ
	case CLASS_CHANGE_WIND_SNEAKER_ITEM_ID:
	case CLASS_CHANGE_GRAND_ARCHER_ITEM_ID:
	case CLASS_CHANGE_ELEMENTAL_MASTER_ITEM_ID:
	case CLASS_CHANGE_VOID_PRINCESS_ITEM_ID:
	case CLASS_CHANGE_BLADE_MASTER_ITEM_ID:
	case CLASS_CHANGE_RECKLESS_FIST_ITEM_ID:
		//{{ kimhc // 2010.2.17 //	�̺� 2�� ���� ������
	case CLASS_CHANGE_NEMESIS_ITEM_ID:
	case CLASS_CHANGE_EMPRESS_ITEM_ID:
		//}} kimhc // 2010.2.17 //	�̺� 2�� ���� ������

		//{{ JHKang / ������ / 2011.6.13 / û 2�� ����
	case CLASS_CHANGE_IRON_PALADIN_ID:
	case CLASS_CHANGE_DEADLY_CHASER_ID:
		//}}
	case CLASS_CHANGE_INFINITY_SWORD_ID:
	case CLASS_CHANGE_DIMENSION_WITCH_ID:
	case CLASS_CHANGE_NIGHT_WATCHER_ID:
	case CLASS_CHANGE_VETERAN_COMMANDER_ID:
	case CLASS_CHANGE_BATTLE_SERAPH_ID:
	case CLASS_CHANGE_TACTICAL_TROOPER_ID:
	case CLASS_CHANGE_SAKRA_DEVANAM_ID:
	case CLASS_CHANGE_YAMA_RAJA_ID:
	case CLASS_CHANGE_GRAND_MASTER_ID:
	case CLASS_CHANGE_BLAZING_HEART_ID:
	case CLASS_CHANGE_ADD_LUNATIC_PSYKER_ID:
		{
			return 2;
		} break;
	}

	return -1;
}



const wstring CX2CashShop::GetClassNameByClassChangeItem( int itemID )
{
	switch( itemID )
	{
	case CLASS_CHANGE_ELSWORD_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_KNIGHT )->m_Description;
		} break;
	case CLASS_CHANGE_MAGIC_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_MAGIC_KNIGHT )->m_Description;
		} break;
	case CLASS_CHANGE_SHEATH_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_SHEATH_KNIGHT )->m_Description;
		} break;
	case CLASS_CHANGE_COMBAT_RANGER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_COMBAT_RANGER )->m_Description;
		} break;
	case CLASS_CHANGE_SNIPING_RANGER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_SNIPING_RANGER )->m_Description;
		} break;
	case CLASS_CHANGE_TRAPPING_RANGER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_TRAPPING_RANGER )->m_Description;
		} break;
	case CLASS_CHANGE_HIGH_MAGICIAN_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_HIGH_MAGICIAN )->m_Description;
		} break;
	case CLASS_CHANGE_DARK_MAGICIAN_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_DARK_MAGICIAN )->m_Description;
		} break;
	case CLASS_CHANGE_BATTLE_MAGICIAN_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_BATTLE_MAGICIAN )->m_Description;
		} break;
	case CLASS_CHANGE_SOUL_TAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_SOUL_TAKER )->m_Description;
		} break;
	case CLASS_CHANGE_OVER_TAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_OVER_TAKER )->m_Description;
		} break;
	case CLASS_CHANGE_WEAPON_TAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_WEAPON_TAKER )->m_Description;
		} break;
	case CLASS_CHANGE_EXOTIC_GEAR_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_EXOTIC_GEAR )->m_Description;
		} break;
	case CLASS_CHANGE_ARCHITECTURE_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_ARCHITECTURE )->m_Description;
		} break;
	case CLASS_CHANGE_ELECTRA_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_ELECTRA )->m_Description;
		} break;
	//{{ kimhc // 2011.1.3 // û 1�� ����
	case CLASS_CHANGE_FURY_GUARDIAN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_FURY_GUARDIAN )->m_Description;
		} break;

	case CLASS_CHANGE_SHOOTER_GUARDIAN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN )->m_Description;
		} break;
//}} kimhc // 2011.1.3 // û 1�� ����

	case CLASS_CHANGE_LORD_NIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_LORD_KNIGHT )->m_Description;
		} break;

	case CLASS_CHANGE_RUNE_SLAYER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_RUNE_SLAYER )->m_Description;
		} break;


	case CLASS_CHANGE_WIND_SNEAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_WIND_SNEAKER )->m_Description;
		} break;

	case CLASS_CHANGE_GRAND_ARCHER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_GRAND_ARCHER )->m_Description;
		} break;

	case CLASS_CHANGE_ELEMENTAL_MASTER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_ELEMENTAL_MASTER )->m_Description;
		} break;

	case CLASS_CHANGE_VOID_PRINCESS_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_VOID_PRINCESS )->m_Description;
		} break;

	case CLASS_CHANGE_BLADE_MASTER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_BLADE_MASTER )->m_Description;
		} break;
	case CLASS_CHANGE_RECKLESS_FIST_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_RECKLESS_FIST )->m_Description;
		} break;

	//{{ kimhc // 2010.1.22 // �̺� 2�� ����
	case CLASS_CHANGE_NEMESIS_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_CODE_NEMESIS )->m_Description;
		} break;

	case CLASS_CHANGE_EMPRESS_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_CODE_EMPRESS )->m_Description;
		} break;
		//}} kimhc // 2010.1.22 // �̺� 2�� ����

	//{{ JHKang / ������ / 2011.6.13 / û 2�� ����
	case CLASS_CHANGE_IRON_PALADIN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_IRON_PALADIN )->m_Description;
		} break;

	case CLASS_CHANGE_DEADLY_CHASER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_DEADLY_CHASER )->m_Description;
		} break;
		//}}
	case CLASS_CHANGE_SHELLING_GUARDIAN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_SHELLING_GUARDIAN )->m_Description;
		} break;
	case CLASS_CHANGE_INFINITY_SWORD_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_INFINITY_SWORD )->m_Description;
		} break;
	case CLASS_CHANGE_DIMENSION_WITCH_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_DIMENSION_WITCH )->m_Description;
		} break;
	case CLASS_CHANGE_NIGHT_WATCHER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_NIGHT_WATCHER)->m_Description;
		} break;
	case CLASS_CHANGE_VETERAN_COMMANDER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_VETERAN_COMMANDER)->m_Description;
		} break;
	case CLASS_CHANGE_BATTLE_SERAPH_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_BATTLE_SERAPH )->m_Description;
		} break;
	case CLASS_CHANGE_TACTICAL_TROOPER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_TACTICAL_TROOPER)->m_Description;
		} break;
		//{{ 2013. 1. 22	�ڼ���	�Ҽ�, ��õ ���� �����
	case CLASS_CHANGE_LITTLE_HSIEN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_LITTLE_HSIEN)->m_Description;
		} break;

	case CLASS_CHANGE_SAKRA_DEVANAM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_SAKRA_DEVANAM)->m_Description;
		} break;
		//}}
	case CLASS_CHANGE_SABER_KNIGHT_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_SABER_KNIGHT)->m_Description;
		} break;

	case CLASS_CHANGE_PYRO_KNIGHT_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_PYRO_KNIGHT)->m_Description;
		} break;

	case CLASS_CHANGE_LITTLE_DEVIL_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_LITTLE_DEVIL)->m_Description;
		} break;

	case CLASS_CHANGE_YAMA_RAJA_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_YAMA_RAJA)->m_Description;
		} break;

	case CLASS_CHANGE_GRAND_MASTER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_GRAND_MASTER )->m_Description;
		} break;

	case CLASS_CHANGE_BLAZING_HEART_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_BLAZING_HEART )->m_Description;
		} break;

	case CLASS_CHANGE_ADD_PSYCHIC_TRACER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ADD_PSYCHIC_TRACER )->m_Description;
		} break;

	case CLASS_CHANGE_ADD_LUNATIC_PSYKER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ADD_LUNATIC_PSYKER )->m_Description;
		} break;
	}

	return L"";
}

bool CX2CashShop::GetCanByeSkillNoteItem( int /*itemID*/ )
{
	if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( SKILL_NOTE_ITEM_ID, true ) != NULL )
	{
		return false;
	}

	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		g_pData->GetMyUser()->GetSelectUnit()->GetMaxSkillNoteSlot() > 0 )
	{
		return false;
	}

	return true;;
}

/** @function 	: GetUnitClassChangeItemLevel
	@brief 		: ���� ���� ���������� Ȯ��, 1/2�� Ȯ��
	@param		: Item Identify
	@return		: 1�� ���� : 1, 2�� ���� : 2
*/
int CX2CashShop::GetUnitClassChangeItemLevel( int itemID )
{
	switch( itemID )
	{
	case UNIT_CLASS_CHANGE_SWORD_KNIGHT_ITEM_ID:
	case UNIT_CLASS_CHANGE_MAGIC_KNIGHT_ITEM_ID:
	case UNIT_CLASS_CHANGE_SHEATH_KNIGHT_ITEM_ID:
	case UNIT_CLASS_CHANGE_HIGH_MAGICIAN_ITEM_ID:
	case UNIT_CLASS_CHANGE_DARK_MAGICIAN_ITEM_ID:
	case UNIT_CLASS_CHANGE_BATTLE_MAGICIAN_ITEM_ID:
	case UNIT_CLASS_CHANGE_COMBAT_RANGER_ITEM_ID:
	case UNIT_CLASS_CHANGE_SNIPING_RANGER_ITEM_ID:
	case UNIT_CLASS_CHANGE_TRAPPING_RANGER_ITEM_ID:
	case UNIT_CLASS_CHANGE_SOUL_TAKER_ITEM_ID:
	case UNIT_CLASS_CHANGE_OVER_TAKER_ITEM_ID:
	case UNIT_CLASS_CHANGE_WEAPON_TAKER_ITEM_ID:
	case UNIT_CLASS_CHANGE_EXOTIC_GEAR_ITEM_ID:
	case UNIT_CLASS_CHANGE_ARCHITECTURE_ITEM_ID:
	case UNIT_CLASS_CHANGE_ELECTRA_ITEM_ID:
	case UNIT_CLASS_CHANGE_FURY_GUARDIAN_ID:
	case UNIT_CLASS_CHANGE_SHOOTING_GUARDIAN_ID:
	case UNIT_CLASS_CHANGE_SHELLING_GUARDIAN_ID:
	case UNIT_CLASS_CHANGE_SABER_KNIGHT_ID:/// �����ý� ���̹� ����Ʈ
	case UNIT_CLASS_CHANGE_PYRO_KNIGHT_ID: /// �����ý� ���̷� ����Ʈ
	case UNIT_CLASS_CHANGE_LITTLE_HSIEN_ID:
	case UNIT_CLASS_CHANGE_LITTLE_DEVIL_ID:
		{
			return 1;
		} break;

	case UNIT_CLASS_CHANGE_LORD_KNIGHT_ITEM_ID:
	case UNIT_CLASS_CHANGE_RUNE_SLAYER_ITEM_ID:
	case UNIT_CLASS_CHANGE_ELEMENTAL_MASTER_ITEM_ID:
	case UNIT_CLASS_CHANGE_VOID_PRINCESS_ITEM_ID:
	case UNIT_CLASS_CHANGE_WIND_SNEAKER_ITEM_ID:
	case UNIT_CLASS_CHANGE_GRAND_ARCHER_ITEM_ID:
	case UNIT_CLASS_CHANGE_BLADE_MASTER_ITEM_ID:
	case UNIT_CLASS_CHANGE_RECKLESS_FIST_ITEM_ID:
	case UNIT_CLASS_CHANGE_NEMESIS_ITEM_ID:
	case UNIT_CLASS_CHANGE_EMPRESS_ITEM_ID:
	case UNIT_CLASS_CHANGE_IRON_PALADIN_ID:
	case UNIT_CLASS_CHANGE_DEADLY_CHASER_ID:
	case UNIT_CLASS_CHANGE_INFINITY_SWORD_ITEM_ID:
	case UNIT_CLASS_CHANGE_DIMENSION_WITCH_ITEM_ID:
	case UNIT_CLASS_CHANGE_NIGHT_WATCHER_ITEM_ID:
	case UNIT_CLASS_CHANGE_VETERAN_COMMANDER_ITEM_ID:
	case UNIT_CLASS_CHANGE_BATTLE_SERAPH_ITEM_ID:
	case UNIT_CLASS_CHANGE_TACTICAL_TROOPER_ID:
	case UNIT_CLASS_CHANGE_SAKRA_DEVANAM_ID:
	case UNIT_CLASS_CHANGE_YAMA_RAJA_ID:
	case UNIT_CLASS_CHANGE_GRAND_MASTER_ID:
	case UNIT_CLASS_CHANGE_BLAZING_HEART_ID:
		{
			return 2;
		} break;
	}

	return -1;
}

/** @function 	: GetClassNameByUnitClassChangeItem
	@brief 		:
	@param		:
	@return		:
*/
const wstring CX2CashShop::GetClassNameByUnitClassChangeItem( int itemID )
{
	switch( itemID )
	{
//#pragma region 1�� ����
	case UNIT_CLASS_CHANGE_SWORD_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_KNIGHT )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_MAGIC_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_MAGIC_KNIGHT )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_SHEATH_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_SHEATH_KNIGHT )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_HIGH_MAGICIAN_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_HIGH_MAGICIAN )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_DARK_MAGICIAN_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_DARK_MAGICIAN )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_BATTLE_MAGICIAN_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_BATTLE_MAGICIAN )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_COMBAT_RANGER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_COMBAT_RANGER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_SNIPING_RANGER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_SNIPING_RANGER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_TRAPPING_RANGER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_TRAPPING_RANGER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_SOUL_TAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_SOUL_TAKER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_OVER_TAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_OVER_TAKER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_WEAPON_TAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_WEAPON_TAKER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_EXOTIC_GEAR_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_EXOTIC_GEAR )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_ARCHITECTURE_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_ARCHITECTURE )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_ELECTRA_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_ELECTRA )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_FURY_GUARDIAN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_FURY_GUARDIAN )->m_Description;
		} break;

	case UNIT_CLASS_CHANGE_SHOOTING_GUARDIAN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_SHELLING_GUARDIAN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_SHELLING_GUARDIAN )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_SABER_KNIGHT_ID:/// �����ý� ���̹� ����Ʈ
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_SABER_KNIGHT )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_PYRO_KNIGHT_ID: /// �����ý� ���̷� ����Ʈ
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_PYRO_KNIGHT)->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_LITTLE_HSIEN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_LITTLE_HSIEN )->m_Description;
		} break;

	case UNIT_CLASS_CHANGE_LITTLE_DEVIL_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_LITTLE_DEVIL )->m_Description;
		} break;
//#pragma endregion

//#pragma region 2�� ����
	case UNIT_CLASS_CHANGE_LORD_KNIGHT_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_LORD_KNIGHT )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_RUNE_SLAYER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_RUNE_SLAYER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_ELEMENTAL_MASTER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_ELEMENTAL_MASTER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_VOID_PRINCESS_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_VOID_PRINCESS )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_WIND_SNEAKER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_WIND_SNEAKER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_GRAND_ARCHER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_GRAND_ARCHER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_BLADE_MASTER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_BLADE_MASTER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_RECKLESS_FIST_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_RECKLESS_FIST )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_NEMESIS_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_CODE_NEMESIS )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_EMPRESS_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_CODE_EMPRESS )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_IRON_PALADIN_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_IRON_PALADIN )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_DEADLY_CHASER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_DEADLY_CHASER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_INFINITY_SWORD_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELSWORD_INFINITY_SWORD )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_DIMENSION_WITCH_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARME_DIMENSION_WITCH )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_NIGHT_WATCHER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_LIRE_NIGHT_WATCHER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_VETERAN_COMMANDER_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_RAVEN_VETERAN_COMMANDER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_BATTLE_SERAPH_ITEM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_EVE_BATTLE_SERAPH )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_TACTICAL_TROOPER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_CHUNG_TACTICAL_TROOPER )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_SAKRA_DEVANAM_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_SAKRA_DEVANAM )->m_Description;
		} break;

	case UNIT_CLASS_CHANGE_YAMA_RAJA_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ARA_YAMA_RAJA )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_GRAND_MASTER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_GRAND_MASTER )->m_Description;
		} break;

	case UNIT_CLASS_CHANGE_BLAZING_HEART_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ELESIS_BLAZING_HEART )->m_Description;
		} break;
	case UNIT_CLASS_CHANGE_LUNATIC_PSYKER_ID:
		{
			return g_pData->GetUnitManager()->GetUnitTemplet( CX2Unit::UC_ADD_LUNATIC_PSYKER )->m_Description;
		} break;
//#pragma endregion
	}

	return L"";
}

/** @function : SetOpenNexonCashChargePage
	@brief : �ؽ� ĳ�� ���� ��������
	@param : strURL_( �������� �ּ� ), rcWebPage_( �������� ������ )
*/
void CX2CashShop::SetOpenNexonCashChargePage( OUT string& strURL_, OUT RECT& rcWebPage_ )
{
	strURL_ = "https://nxpay.nexon.com/cash/main.aspx?CHID=ELSW&NXID=";

	string strUserID;

	ConvertWCHARToChar( strUserID, g_pData->GetMyUser()->GetUserData().userID.c_str() );

	strURL_ += strUserID;

	string strCtype = "&CTYPE=1&HKEY=";

	strURL_ += strCtype;

	// �ؽ�Ű ///////////////////
	std::string strSource;
	std::string strTemp;
	strSource = "ELSW";

	strSource += strUserID;

	// oasis907 : ����� [2011.1.10] // ���� �ð� ������ ���� ����
	CTime ctCurrentTime( g_pData->GetServerCurrentTime() );

	ConvertWCHARToChar( strTemp, static_cast< const wchar_t * >( ctCurrentTime.Format( L"%Y%m%d%I" ) ) );
	strSource += strTemp;

	string strMD5 = KncUtil::GetMD5( strSource );


	strURL_ += strMD5;
	////////////////////////////////

	rcWebPage_.left = 250;
	rcWebPage_.top	= 250;
	rcWebPage_.right	= 538;
	rcWebPage_.bottom	= 530;
}

//{{ kimhc // 2011-08-10 // ä�θ� SOAP

/** @function : SetOpenTooniCashCargePage
	@brief : ���� ĳ�� ���� ��������
	@param : strURL_( �������� �ּ� ), rcWebPage_( �������� ������ )
*/
void CX2CashShop::SetOpenTooniCashChargePage( OUT string& strURL_, OUT RECT& rcWebPage_ )
{
	// ���� ���Ϸ��� ���̵� ������ ���� ���� ���ߴٸ�...
	if ( false == g_pData->GetMyUser()->GetUserData().m_wstrTooniLandID.empty() )
	{
		strURL_ = "https://cert.tooniland.com/billing/INIsecureNewStart.tl?type=etc&price=1000&userId=";

		string strUserID;
		ConvertWCHARToChar( strUserID, g_pData->GetMyUser()->GetUserData().m_wstrTooniLandID );
		strURL_ += strUserID;
	}
	// ���Ϸ��� ���̵� ������ ���� �޾Ҵٸ�
	else
	{
		strURL_ = "https://member.tooniland.com/membersMember/checkIdPopup2.tl";
	}

	rcWebPage_.left = 250;
	rcWebPage_.top	= 250;
	rcWebPage_.right	= 530;
	rcWebPage_.bottom	= 340;
}

//}} kimhc // 2011-08-10 // ä�θ� SOAP

//{{ ������ // 2012-04-09 // �Ǹ�����
//}} ������ // 2012-04-09 // �Ǹ�����

bool CX2CashShop::GetCahBuyExpandQuickSlotItem( int /*itemID*/ )
{
	if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( EXPAND_QUICK_SLOT_ITEM_ID, true ) != NULL )
	{
		return false;
	}

	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		g_pData->GetMyUser()->GetSelectUnit()->IsExpandQuickSlot() == true )
	{
		return false;
	}

	return true;
}
CX2CashShop::CashShopSlot::CashShopSlot( CX2Slot::SLOT_TYPE cashShopSlotType, int slotIndex )
{
	m_ItemID			= 0;
	m_pDLGCashShopSlot	= NULL;

	m_SlotType = cashShopSlotType;
	m_SlotIndex = slotIndex;

	//{{ 2009.01.19 ���¿� : �ڵ�����
	switch(cashShopSlotType)
	{
	case CX2Slot::ST_CASH_SHOP_MAIN_NEW:
		{
			m_pDLGCashShopSlot = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Mainpage_Itemslot.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCashShopSlot );
			m_pDLGCashShopSlot->SetShowEnable( false, false );

			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(373,138,0) );
			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(582,138,0) );
			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(797,138,0) );

		} break;
	case CX2Slot::ST_CASH_SHOP_MAIN_HOT:
		{
			m_pDLGCashShopSlot = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Mainpage_Itemslot.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCashShopSlot );
			m_pDLGCashShopSlot->SetShowEnable( false, false );

			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(373,303,0) );
			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(582,303,0) );
			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(797,303,0) );

		} break;
	case CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND:
		{
			m_pDLGCashShopSlot = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Mainpage_Itemslot.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCashShopSlot );
			m_pDLGCashShopSlot->SetShowEnable( false, false );

			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(373,469,0) );
			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(582,469,0) );
			m_pDLGCashShopSlot->AddDummyPos( D3DXVECTOR3(797,469,0) );

		} break;
	case CX2Slot::ST_CASH_SHOP_SUB:
		{
			m_pDLGCashShopSlot = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Subpage_Itemslot.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCashShopSlot );
		} break;
	case CX2Slot::ST_CASH_SHOP_SUB_SPECIAL:
		{
			m_pDLGCashShopSlot = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Subpage_Hot_Itemslot.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCashShopSlot );
		} break;
	default:
		break;
	}
	//}}

}

CX2CashShop::CashShopSlot::~CashShopSlot()
{
	if ( m_pDLGCashShopSlot != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCashShopSlot, 0, false );

	m_pDLGCashShopSlot = NULL;
}

void CX2CashShop::CashShopSlot::SetShow( bool bShow )
{
	if ( m_pDLGCashShopSlot != NULL )
		m_pDLGCashShopSlot->SetShowEnable( bShow, bShow );
}

//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
//#ifdef	AUTO_PAYMENT
void CX2CashShop::CashShopSlot::SetInfo( bool /*bAutoPayment*/ /*= false*/ )
//#else	AUTO_PAYMENT
//void CX2CashShop::CashShopSlot::SetInfo()
//#endif	AUTO_PAYMENT
//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
{
	if ( m_pDLGCashShopSlot == NULL )
		return;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( m_ItemID );
	if ( pItemTemplet == NULL )
		return;

	CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( m_ItemID );
	if ( pCashItem == NULL )
		return;

	CKTDGUIStatic* pStaticName = (CKTDGUIStatic*)m_pDLGCashShopSlot->GetControl(L"g_pStaticItemName");
	if ( pStaticName != NULL && pStaticName->GetString(0) != NULL )
	{
		//{{ 2009.01.19 ���¿� : �ڵ�����
		switch(m_SlotType)
		{
		case CX2Slot::ST_CASH_SHOP_MAIN_NEW:
		case CX2Slot::ST_CASH_SHOP_MAIN_HOT:
		case CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND:
			{
				pStaticName->GetString(0)->msg = GetCutString( pItemTemplet->GetName(), 17 );
			}break;
		case CX2Slot::ST_CASH_SHOP_SUB:
			{
				pStaticName->GetString(0)->msg = GetCutString( pItemTemplet->GetName(), 24 );
			}break;
		case CX2Slot::ST_CASH_SHOP_SUB_SPECIAL:
			{
				pStaticName->GetString(0)->msg = GetCutString( pItemTemplet->GetName(), 16 );
			}break;
		default:
			{
                pStaticName->GetString(0)->msg = pItemTemplet->GetName();
			}break;
		}
		//}}
	}

	CKTDGUIStatic* pStaticPrice = (CKTDGUIStatic*)m_pDLGCashShopSlot->GetControl(L"g_pStaticCashNum");
	if ( pStaticPrice != NULL && pStaticPrice->GetString(0) != NULL )
	{
		//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
		pStaticPrice->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_38, "L", g_pMain->GetEDString( pCashItem->GetCash() ) ) );
		//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
		 // ���� ������ 2���� �� ��
		/*const KBillProductInfo* pkKBillProductInfo = pCashItem->GetGateProduct();
		if ( pkKBillProductInfo != NULL && m_ItemID == ITEM_ID_EVENT_NEW_SKILL_NOTE &&
			NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( ITEM_ID_EVENT_NEW_SPECIAL ) )
		{
			pStaticPrice->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_38, "L", g_pMain->GetEDString( 3000 ) ) );
		}*/
		const KBillProductInfo* pkKBillProductInfo = pCashItem->GetGateProduct();
		std::vector<int> TempList;
		int TempProDuctNo = 0;
		if( pkKBillProductInfo != NULL && (g_pData->GetItemManager()->DisCountItemID_Find(m_ItemID,TempList,TempProDuctNo) == true) )
		{
			for(int i = 0; i < static_cast<int>( TempList.size() ); ++i)
			{
				if( NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( TempList[i] ) )
				{
					int nPrice = g_pData->GetItemManager()->GetChangeCashPoint(TempProDuctNo);
					pStaticPrice->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_38, "L", g_pMain->GetEDString( nPrice ) ) );
					break;
				}
			}
		}
	}

	if ( m_SlotType == CX2Slot::ST_CASH_SHOP_SUB_SPECIAL )
	{
		CKTDGUIStatic* pStaticPrice = (CKTDGUIStatic*)m_pDLGCashShopSlot->GetControl(L"g_pStaticHotitem_Slot_Num");
		if ( pStaticPrice != NULL  )
		{
			for ( int i = 0; i < 5; i++ )
			{
				if ( pStaticPrice->GetPicture(i) != NULL )
					pStaticPrice->GetPicture(i)->SetShow( false );
			}

			if ( pStaticPrice->GetPicture(m_SlotIndex) != NULL )
				pStaticPrice->GetPicture( m_SlotIndex )->SetShow( true );
		}
	}
}

wstring CX2CashShop::CashShopSlot::GetCutString( const WCHAR* wszItemName, int maxItemNameLen )
{
	wstring tempName = wszItemName;
	if ( (int)tempName.length() > maxItemNameLen )
	{
		tempName = tempName.substr( 0, maxItemNameLen - 2 );
		tempName += L"..";
	}

	return tempName;
}

//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�

CX2CashShop::CashShopSlotManager::CashShopSlotManager( CX2Slot::SLOT_TYPE cashShopSlotType )
{
	m_NowPage = 1;
	m_MaxPage = 1;

	//{{ 2009.01.19 ���¿� : �ڵ�����
	switch(cashShopSlotType)
	{
	case CX2Slot::ST_CASH_SHOP_MAIN_NEW:
	case CX2Slot::ST_CASH_SHOP_MAIN_HOT:
	case CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND:
		{
			m_MaxSlotNumPerPage = CASH_SHOP_MAIN_SLOT_NUM;
		} break;
	case CX2Slot::ST_CASH_SHOP_SUB:
		{
			m_MaxSlotNumPerPage = 10;
		} break;
	case CX2Slot::ST_CASH_SHOP_SUB_SPECIAL:
		{
			m_MaxSlotNumPerPage = 5;
		} break;
	default:
		{
			ASSERT( !"Unexpected CashShop Slot Type" );
			m_MaxSlotNumPerPage = CASH_SHOP_MAIN_SLOT_NUM;
		}
		break;
	}
	//}}

	for ( int i = 0; i < m_MaxSlotNumPerPage; i++ )
	{
		CX2CashShop::CashShopSlot* pCashShopSlot = new CX2CashShop::CashShopSlot( cashShopSlotType, i );
		if ( pCashShopSlot->m_pDLGCashShopSlot != NULL )
			pCashShopSlot->m_pDLGCashShopSlot->SetPos( D3DXVECTOR2( pCashShopSlot->m_pDLGCashShopSlot->GetDummyPos(i).x, pCashShopSlot->m_pDLGCashShopSlot->GetDummyPos(i).y ) );
		m_vecCashShopSlot.push_back( pCashShopSlot );
	}
	m_bOpen = false;
}
CX2CashShop::CashShopSlotManager::~CashShopSlotManager()
{
	for ( int i = 0; i < (int)m_vecCashShopSlot.size(); i++ )
	{
		CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[i];
		SAFE_DELETE( pCashShopSlot );
	}
	m_vecCashShopSlot.clear();
}

bool CX2CashShop::CashShopSlotManager::IsPossibleUsedByMyBankMembership( int itemID ) const
{
	// ���� �������� �ƴϸ� true�� ����
	switch ( itemID )
	{
	case SILVER_MEMBERSHIP_CARD:
	case GOLD_MEMBERSHIP_CARD:
	case EMERALD_MEMBERSHIP_CARD:
	case DIAMOND_MEMBERSHIP_CARD:
	case PLATINUM_MEMBERSHIP_CARD:
	case COBO_CREDIT_CARD:
		break;

	default:
		return true;
	}

	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		const CX2Inventory&	kInventory	= g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		CX2Inventory::MEMBERSHIP_PRIVATE_BANK	eMembershipGrade	= CX2Inventory::MPB_NORMAL;
		eMembershipGrade	= static_cast< CX2Inventory::MEMBERSHIP_PRIVATE_BANK >( kInventory.GetItemMaxNum( CX2Inventory::ST_BANK ) );

		switch ( eMembershipGrade )
		{
		case CX2Inventory::MPB_NORMAL:
			{
				if ( itemID	== SILVER_MEMBERSHIP_CARD )
					return true;
				else if ( itemID == COBO_CREDIT_CARD
					&&  kInventory.GetItemByTID( _CONST_UI_PRIVATE_BANK_::g_iBankQuestItemID, true ) == NULL )
					return true;
			}
			break;

		case CX2Inventory::MPB_SILVER:
			{
				if ( itemID	== GOLD_MEMBERSHIP_CARD )
					return true;
			}
			break;

		case CX2Inventory::MPB_GOLD:
			{
				if ( itemID	== EMERALD_MEMBERSHIP_CARD )
					return true;
			}
			break;

		case CX2Inventory::MPB_EMERALD:
			{
				if ( itemID	== DIAMOND_MEMBERSHIP_CARD )
					return true;
			}
			break;

		case CX2Inventory::MPB_DIAMOND:
			{
				if ( itemID	== PLATINUM_MEMBERSHIP_CARD
					&& kInventory.GetItemByTID( _CONST_UI_PRIVATE_BANK_::g_iBankQuestItemID, true ) == NULL )
					return true;
			}
			break;

		case CX2Inventory::MPB_PLATINUM:
			{
				return false;
			}
			break;
		}
	}

	return false;
}




// bPossibleItemOnly : ���� ����� �� �ִ� �����۸� ���̰� ����
void CX2CashShop::CashShopSlotManager::Reset( const vector<int>& vecItemID, bool /*bAutoPayment*/ /* = false */, bool bPossibleItemOnly /*= false*/ )
{
	m_NowPage = 1;

//	//{{ kimhc // 2009-08-12 // ���� ���θ޴����� ����ִ� ȸ���Ǹ� ���̵���
//#ifdef	PRIVATE_BANK
	m_vecItemID.resize( 0 );
	for ( UINT i = 0; i < vecItemID.size(); i++ )
	{
		if ( IsPossibleUsedByMyBankMembership( vecItemID[i] ) == false )
			continue;

		if( true == bPossibleItemOnly )
		{
			if( CX2CashShop::IsPossibleUsedByMyCharacter( vecItemID[i] ) == false )
				continue;

			int iClassChangeItemLevel = GetClassChangeItemLevel( vecItemID[i] );
			if( iClassChangeItemLevel > 0 &&															// ���� ĳ�� �������ε�
				1 != iClassChangeItemLevel - g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )	// ���� ĳ���� Ŭ������ ����� �� ���� �������̸�
				continue;

			if( vecItemID[i] == UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID
				&& 2 != g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )
				continue;																				//2�� ���� �ƴϸ� ������ �ʵ��� ��


			if( vecItemID[i] == CASH_SKILL_NOTE_ITEM_ID && GetCanByeSkillNoteItem( vecItemID[i] ) == false )
			{
				continue;
			}


			if( vecItemID[i] == SHARING_BANK_OPEN_CASH_ITEM_ID && GetCahBuySharingBankItem( vecItemID[i] ) == false )
			{
				continue;
			}

			//{{ ����� : [2012/12/5] //		Ư�� ������ ������ �־�� ĳ�ü����� ���� ����
			if( g_pData != NULL )
			{
				if( g_pData->GetItemManager() != NULL )
				{
					bool bContinue = false;
					std::vector<int> TempItemID;
					TempItemID.clear();
					if( g_pData->GetItemManager()->CheckKeepShowItem( vecItemID[i] , TempItemID ) == true )
					{
						for( int i = 0; i < static_cast<int>( TempItemID.size() ); ++i )
						{
							if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( TempItemID[i] ) == NULL )
							{
								bContinue = true;
								break;
							}
						}
						if( bContinue == true)
						{
							continue;
						}
					}
				}
			}

			//{{ ����� : [2012/12/5] //		Ư�� ������ ������ �־�� ĳ�ü����� ���� ����

			int iUnitClassChangeItemLevel = GetUnitClassChangeItemLevel( vecItemID[i] );
			int iMyClassLevel = g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel();
			int iItemUnitType = g_pData->GetItemManager()->GetItemTemplet( vecItemID[i] )->GetUnitType();
			int iUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
			int iItemUnitClass = g_pData->GetItemManager()->GetItemTemplet( vecItemID[i] )->GetUnitClass();
			int iUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();

			if ( (iUnitClassChangeItemLevel > 0 && iItemUnitType != iUnitType) || iUnitClassChangeItemLevel > iMyClassLevel )
				continue;

			if ( iUnitClassChangeItemLevel > 0 && iUnitClassChangeItemLevel < iMyClassLevel)
				continue;

			if ( iUnitClassChangeItemLevel > 0 && iItemUnitClass == iUnitClass )
				continue;
			if( vecItemID[i] == EXPAND_QUICK_SLOT_ITEM_ID && GetCahBuyExpandQuickSlotItem( vecItemID[i] ) == false )
			{
				continue;
			}
		}

		m_vecItemID.push_back( vecItemID[i] );
	}
//#else	PRIVATE_BANK
//	m_vecItemID = vecItemID;
//#endif	PRIVATE_BANK
//	//}} kimhc // 2009-08-12 // ���� ���θ޴����� ����ִ� ȸ���Ǹ� ���̵���






	int checkSize	= 0;
	checkSize		= (int)m_vecItemID.size() - 1;

	if ( checkSize <= 0 )
		checkSize = 0;
	m_MaxPage = (checkSize / m_MaxSlotNumPerPage) + 1;

	for ( int i = 0; i < (int)m_vecCashShopSlot.size(); i++ )
	{
		CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[i];
		if ( pCashShopSlot != NULL )
			pCashShopSlot->SetShow( false );
	}

	if ( m_bOpen == true )
	{
		for ( int i = 0; i < m_MaxSlotNumPerPage && i < (int)m_vecCashShopSlot.size() && i < (int)m_vecItemID.size(); i++ )
		{
			int itemID = m_vecItemID[i];

			if ( IsPossibleUsedByMyBankMembership( itemID ) == false )
			{
			}

			CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[i];
			pCashShopSlot->SetShow( true );
			pCashShopSlot->SetItemID( itemID );

			//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
			pCashShopSlot->SetInfo();
			//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
		}
	}
}

//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
//#ifdef	AUTO_PAYMENT
void CX2CashShop::CashShopSlotManager::SetOpen( bool bOpen, bool /*bAutoPayment*/ /*= false*/ )
//#else	AUTO_PAYMENT
//void void CX2CashShop::CashShopSlotManager::SetOpen( bool bOpen )
//#endif	AUTO_PAYMENT
//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
{
	m_bOpen = bOpen;

	if ( m_bOpen == true )
	{
		int j = 0;
		for ( int i = (m_NowPage - 1) * m_MaxSlotNumPerPage; i < (m_NowPage)*m_MaxSlotNumPerPage && j < (int)m_vecCashShopSlot.size() && i < (int)m_vecItemID.size(); i++, j++ )
		{
			int itemID = m_vecItemID[i];
			CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[j];
			pCashShopSlot->SetShow( true );
			pCashShopSlot->SetItemID( itemID );

			//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
			pCashShopSlot->SetInfo();
			//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�

		}
	}
	else
	{
		for ( int i = 0; i < (int)m_vecCashShopSlot.size(); i++ )
		{
			CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[i];
			if ( pCashShopSlot == NULL )
				continue;

			pCashShopSlot->SetShow( false );
		}
	}
}

bool CX2CashShop::CashShopSlotManager::NextPage( bool /*bAutoPayment*/ /* = false */ )
{
	if ( m_NowPage >= m_MaxPage )
		return false;

	m_NowPage++;

	//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
	ResetNowPage();
	//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��

	return true;
}

bool CX2CashShop::CashShopSlotManager::PrevPage( bool /*bAutoPayment*/ /* = false */ )
{
	if ( m_NowPage <= 1 )
		return false;

	m_NowPage--;

	//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
	ResetNowPage();
	//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��

	return true;
}

vector<int> CX2CashShop::CashShopSlotManager::GetNowShowItemIDList()
{
	vector<int> vecItemID;

	int j = 0;
	for ( int i = (m_NowPage-1) * m_MaxSlotNumPerPage; i < (m_NowPage)*m_MaxSlotNumPerPage && j < (int)m_vecCashShopSlot.size() && i < (int)m_vecItemID.size(); i++, j++)
	{
		int itemID = m_vecItemID[i];
		vecItemID.push_back( itemID );
	}

	return vecItemID;
}

int CX2CashShop::CashShopSlotManager::GetItemID( CKTDGUIDialogType pDialog )
{

	for ( int i = 0; i < (int)m_vecCashShopSlot.size(); i++ )
	{
		CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[i];
		if ( pCashShopSlot == NULL )
			continue;

		if ( pCashShopSlot->m_pDLGCashShopSlot == pDialog )
			return pCashShopSlot->GetItemID();
	}

	return 0;
}

//{{ kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�
//}} kimhc // 2009-10-12 // �ڵ����� �� ����, �� ��ư �Ⱥ��̰� �ϱ� ���� �߰�

//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
//#ifdef	AUTO_PAYMENT
void CX2CashShop::CashShopSlotManager::ResetNowPage( bool /*bAutoPayment*/ )
//#else	AUTO_PAYMENT
//void CX2CashShop::CashShopSlotManager::ResetNowPage()
//#endif	AUTO_PAYMENT
//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
{
	for ( int i = 0; i < (int)m_vecCashShopSlot.size(); i++ )
	{
		CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[i];
		if ( pCashShopSlot == NULL )
			continue;

		pCashShopSlot->SetShow( false );
	}

	int j = 0;
	for ( int i = (m_NowPage-1) * m_MaxSlotNumPerPage; i < (m_NowPage)*m_MaxSlotNumPerPage && j < (int)m_vecCashShopSlot.size() && i < (int)m_vecItemID.size(); i++, j++ )
	{
		int itemID = m_vecItemID[i];
		CX2CashShop::CashShopSlot* pCashShopSlot = m_vecCashShopSlot[j];
		pCashShopSlot->SetShow( true );
		pCashShopSlot->SetItemID( itemID );

		bool bShow = IsPossiblePresent(itemID);
		pCashShopSlot->SetShowPresentButton( bShow );


		//{{ kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
		pCashShopSlot->SetInfo();
		//}} kimhc // 2009-10-12 // �ڵ����� �� ĳ�� - �� ���� ǥ��
	}
}
CX2CashShop::BuyItemPopupSlot::BuyItemPopupSlot()
{
}
CX2CashShop::BuyItemPopupSlot::BuyItemPopupSlot( int index, const WCHAR* pFileName ) :
CX2PageMgrItem::CX2PageSlotItem( index, pFileName )
{
}
CX2CashShop::BuyItemPopupSlot::~BuyItemPopupSlot()
{

}



void CX2CashShop::BuyItemPopupSlot::UpdateUI()
{
	if ( m_pDialog == NULL )
		return;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( GetItemID() );
	if ( pItemTemplet == NULL )
		return;

	CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( GetItemID() );
	if ( pCashItem == NULL )
		return;
	CKTDGUIStatic* pShopImage = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashShop_BuyWindow_Slot_Image" );

	if ( pShopImage != NULL && pShopImage->GetPicture(0) != NULL )
	{
		if ( pShopImage->GetPicture(0)->SetTex( pItemTemplet->GetShopImage() ) == false )
		{
			pShopImage->GetPicture(0)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
		}
	}

	CKTDGUIStatic* pItemName = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStatic_Item_ID_1Line" );
	if ( pItemName != NULL && pItemName->GetString(0) != NULL )

	{
		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetName(), 97, pItemName->GetString(0)->fontIndex );
		if ( tempName.find( L"\n" ) == std::wstring::npos ) //���� ¥��
		{
			pItemName->SetString( 0, tempName.c_str() );
		}
		else
		{
			pItemName->SetString( 0, L"" );
		}

	}

	pItemName = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStatic_Item_ID_2Line" );
	if ( pItemName != NULL && pItemName->GetString(0) != NULL )
	{
			wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetName(), 97, pItemName->GetString(0)->fontIndex );
		if ( tempName.find( L"\n" ) == std::wstring::npos ) //���� ¥��
		{
			pItemName->SetString( 0, L"" );

		}
		else
		{
			pItemName->SetString( 0, tempName.c_str() );
		}
	}
	// ���� ������ 2���� �� ��
	bool bIdexReset = false;
	CKTDGUIComboBox* pComboBox = static_cast<CKTDGUIComboBox*>(m_pDialog->GetControl( L"CashItemList" ));
	if ( pComboBox != NULL )
	{
		pComboBox->RemoveAllItems();
		bool bPeriod = false;
		for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
		{
			KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
			if( kBillProductInfo.m_cPeriod != 0 )
			{
				bPeriod = true;
				break;
			}
		}

		for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
		{
			KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
			wstringstream wstrstm;
			//wstrstm << kBillProductInfo
			/*	if( kBillProductInfo.m_iProductNo == ITEM_ID_EVENT_NEW_SKILL_NOTE_PNO &&
			NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( ITEM_ID_EVENT_NEW_SPECIAL ) )
			{
			bIdexReset = true;
			continue;
			}
			if( kBillProductInfo.m_iProductNo == ITEM_ID_EVENT_NEW_SKILL_NOTE_PNO2 &&
			NULL != g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( ITEM_ID_EVENT_NEW_SPECIAL ) )
			{
			continue;
			}*/
			if( g_pData->GetItemManager()->CheckDisCountItemList( kBillProductInfo.m_iProductNo , bIdexReset ) == false )
			{
				continue;
			}


			if ( (int)kBillProductInfo.m_cPeriod != 0 )
			{
				if ( i == 0 )
					pComboBox->AddItem( GET_STRING( STR_ID_35 ), NULL, false );

				if( bIdexReset == true )
				{
					pComboBox->AddItem( GET_STRING( STR_ID_35 ), NULL, false );
					bIdexReset = false;
				}
				wstrstm << (int)kBillProductInfo.m_cPeriod << GET_STRING( STR_ID_14 ) << L"/";
			}
			else
			{
				if ( i == 0 )
					pComboBox->AddItem( GET_STRING( STR_ID_36 ), NULL, false );

				if( bIdexReset == true )
				{
					pComboBox->AddItem( GET_STRING( STR_ID_35 ), NULL, false );
					bIdexReset = false;
				}

				if( bPeriod == true && kBillProductInfo.m_cQuantity == 1 )
					wstrstm << GET_STRING( STR_ID_13858 ) << L"/";
				else
				wstrstm << (int)kBillProductInfo.m_cQuantity << GET_STRING( STR_ID_24 ) << L"/";
			}

			wstrstm << kBillProductInfo.m_iSalePrice << GET_STRING( STR_ID_34 );

			pComboBox->AddItem( wstrstm.str().c_str(), NULL, false );
		}
	}

}

void CX2CashShop::BuyItemPopupSlot::SetCheck( bool bCheck )
{
	if ( m_pDialog != NULL )
	{
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)m_pDialog->GetControl( L"g_pCheckBoxCashShop_BuyWindow_Slot" );
		if ( pCheckBox != NULL )
		{
			pCheckBox->SetCheckedPure( bCheck );
		}
	}

	//���� ������ �°Բ� �޺� �ڽ� ����Ÿ �������ְ�.. ���� ���õȰſ� ���� ���õ� �׸� ��������..
}

void CX2CashShop::BuyItemPopupSlot::SetCash( int comboBoxIndex )
{
	if ( m_pDialog != NULL )
	{
		CKTDGUIComboBox* pComboBox = static_cast<CKTDGUIComboBox*>(m_pDialog->GetControl( L"CashItemList" ));

		if ( pComboBox != NULL )
		{
			pComboBox->SetSelectedByIndex( comboBoxIndex );
		}
	}
}

CX2CashShop::CashItemToInvenPopupSlot::CashItemToInvenPopupSlot()
{
}
CX2CashShop::CashItemToInvenPopupSlot::CashItemToInvenPopupSlot( int index, const WCHAR* pFileName ) :
CX2PageMgrItem::CX2PageSlotItem( index, pFileName )
{
}
CX2CashShop::CashItemToInvenPopupSlot::~CashItemToInvenPopupSlot()
{

}
void CX2CashShop::CashItemToInvenPopupSlot::UpdateUI()
{
	if ( m_pDialog == NULL )
		return;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( GetItemID() );
	if ( pItemTemplet == NULL )
		return;

	CKTDGUIStatic* pShopImage = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashShop_Move_Inventory_Window_Slot_Image" );
	if ( pShopImage != NULL && pShopImage->GetPicture(0) != NULL )
	{
        if ( pShopImage->GetPicture(0)->SetTex( pItemTemplet->GetShopImage() ) == false )
		{
			pShopImage->GetPicture(0)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
		}
	}

	CKTDGUIStatic* pItemName = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStatic_Item_ID_1Line" );
	if ( pItemName != NULL && pItemName->GetString(0) != NULL )
	{
		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetName(), 224, pItemName->GetString(0)->fontIndex );
		if ( tempName.find( L"\n" ) == std::wstring::npos ) //���� ¥��
		{
			pItemName->SetString( 0, tempName.c_str() );
		}
		else
		{
			pItemName->SetString( 0, L"" );
		}

	}

	pItemName = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStatic_Item_ID_2Line" );
	if ( pItemName != NULL && pItemName->GetString(0) != NULL )
	{
		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetName(), 224, pItemName->GetString(0)->fontIndex );
		if ( tempName.find( L"\n" ) == std::wstring::npos ) //���� ¥��
		{
			pItemName->SetString( 0, L"" );

		}
		else
		{
			pItemName->SetString( 0, tempName.c_str() );
		}
	}


	CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDialog->GetControl( L"Option_ComboBox" );
	if ( pComboBox != NULL )
	{
		pComboBox->SetEnable( true );
		pComboBox->SetShow( true );
		pComboBox->RemoveAllItems();

		AddItemToComboBox_KOG( pComboBox, pItemTemplet );

		if ( pComboBox->GetNumItems() == 0 )
		{
			pComboBox->SetEnable( false );
			pComboBox->SetShow( false );
		}
	}


}

void CX2CashShop::CashItemToInvenPopupSlot::AddItemToComboBox_KOG( CKTDGUIComboBox* pComboBox_, const CX2Item::ItemTemplet* pItemTemplet_ )
{
	if ( NULL != pComboBox_ )
	{
		/// ��Ű�� ������ �̸�
		if ( m_KBillOrderInfo.m_byteProductKind == 1 )
		{
			if (m_KBillPackageInfo.m_vecSocketOption.size() > 0)
				pComboBox_->AddItem( GET_STRING( STR_ID_37 ), NULL, false );

			for( std::vector<int>::iterator it = m_KBillPackageInfo.m_vecSocketOption.begin(); it < m_KBillPackageInfo.m_vecSocketOption.end(); ++it )
			{
				// 0�� ������ �ƿ� ���� �ȵȰ��� �ǹ���
				if (0 != *it)
					AddSocketIdListToComboBox( pComboBox_, pItemTemplet_, *it );
			}
		}
		else	/// ��Ű���� �ƴϸ�
		{
			if ( m_KBillOrderInfo.m_vecSocketOption.size() > 0 )
				pComboBox_->AddItem( GET_STRING( STR_ID_37 ), NULL, false );

			for( std::vector<int>::iterator it = m_KBillOrderInfo.m_vecSocketOption.begin(); it < m_KBillOrderInfo.m_vecSocketOption.end(); ++it )
			{
				// 0�� ������ �ƿ� ���� �ȵȰ��� �ǹ���
				if (0 != *it)
					AddSocketIdListToComboBox( pComboBox_, pItemTemplet_, *it );
			}

		}
	}

}

void CX2CashShop::CashItemToInvenPopupSlot::SetOption( int comboBoxIndex )
{

	if ( m_pDialog != NULL )
	{
		CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDialog->GetControl( L"Option_ComboBox" );
		if ( pComboBox != NULL )
		{
			pComboBox->SetSelectedByIndex( comboBoxIndex );
		}
	}

}

void CX2CashShop::CashItemToInvenPopupSlot::SetCheck( bool bCheck )
{
	if ( m_pDialog != NULL )
	{
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)m_pDialog->GetControl( L"g_pCheckBoxCashShop_MoveInventory_Slot" );
		if ( pCheckBox != NULL )
		{
			pCheckBox->SetCheckedPure( bCheck );
		}
	}
	//���� ������ �°Բ� �޺� �ڽ� ����Ÿ �������ְ�.. ���� ���õȰſ� ���� ���õ� �׸� ��������..
}


/** @function : AddSocketIdListToComboBox (������ �ش�)
	@brief : wszSocketItemGroupID_ �� �ش��ϴ� �׷쿡 �����ִ� SocketID ���� �޺��ڽ��� �ִ� �Լ�
	@param : �޺��ڽ�(pComboBox_), ���� ���������� �ϴ� ������ ���ø�(pItemTemplet_), ĳ�þƹ�Ÿ�� ���� ���ϿɼǱ׷�(wszSocketItemGroupID_)
*/
void CX2CashShop::CashItemToInvenPopupSlot::AddSocketIdListToComboBox( CKTDGUIComboBox* pComboBox_, const CX2Item::ItemTemplet* pItemTemplet_, const int iSocketItemGroupID_ )
{
	{
		CX2SocketItem* pSocketItem = g_pData->GetSocketItem();
		if ( NULL != pSocketItem )
		{
			// ������ ���� ������ 0 �� ������ ������
			// 0���� ũ�� ������ ������ ����
			const int iLevel =
				( 0 < pItemTemplet_->GetUseLevel() ? pItemTemplet_->GetUseLevel() : g_pData->GetSelectUnitLevel() );

			const int iSocketGroupID = iSocketItemGroupID_;

			vector<int> vecSocketIdList;	/// iSocketGroupID �� �ش��ϴ� SocketID ���� ����
			if ( 0 != pSocketItem->GetSocketIdListForCashAvatar( iSocketGroupID, vecSocketIdList, pItemTemplet_->GetItemType() ) )
			{
				// ��������, ���ʷ� �ִ´�.
				for( const int iSocketID : vecSocketIdList )
				{
					const CX2SocketItem::SocketData* pSocketData = pSocketItem->GetSocketData( iSocketID );
					if ( NULL != pSocketData )
						pComboBox_->AddItem( pSocketData->GetSocketDesc( iLevel ).c_str(), NULL, false );
				}
			}
		}
	}
}



void CX2CashShop::BuyItemPopup::SetCheckAllItem( bool bCheck )
{
	if ( bCheck == true )
	{
		m_vecCheckedItem = m_vecItemID;
	}
	else
	{
		m_vecCheckedItem.resize(0);
	}

	ResetNowPage();
}

void CX2CashShop::BuyItemPopup::ChangeCheckItem( CKTDGUIDialogType pDialog, bool bCheck )
{
	if ( pDialog == NULL )
		return;

	CX2CashShop::BuyItemPopupSlot* pBuyItemPopupSlot = (CX2CashShop::BuyItemPopupSlot*)GetPageSlot( pDialog );
	if ( pBuyItemPopupSlot == NULL )
		return;

	int itemIDToFind = pBuyItemPopupSlot->GetItemID();

	if ( bCheck == true )
	{
		bool bFind = false;
		for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
		{
			int itemID = m_vecCheckedItem[i];
			if ( itemID == itemIDToFind )
			{
				bFind = true;
				break;
			}
		}
		if ( bFind == false )
		{
			m_vecCheckedItem.push_back( itemIDToFind );
		}
	}
	else
	{
		for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
		{
			int itemID = m_vecCheckedItem[i];
			if ( itemID == itemIDToFind )
			{
				m_vecCheckedItem.erase( m_vecCheckedItem.begin() + i );
				i--;
				break;
			}
		}
	}

	ResetNowPage();
}

void CX2CashShop::BuyItemPopup::ChangeItemPeriod( CKTDGUIDialogType pDialog, int comboBoxIndex )
{
	CX2CashShop::BuyItemPopupSlot* pBuyItemPopupSlot = (CX2CashShop::BuyItemPopupSlot*)GetPageSlot( pDialog );
	if ( pBuyItemPopupSlot != NULL )
	{
		map< int, int >::iterator mit;
		mit = m_mapSelectedProductNo.find( pBuyItemPopupSlot->GetItemID()  );
		if ( mit != m_mapSelectedProductNo.end() )
		{
			mit->second = comboBoxIndex;
		}
		else
		{
			m_mapSelectedProductNo.insert( std::make_pair( pBuyItemPopupSlot->GetItemID(), comboBoxIndex ) );
		}
	}
	UpdateCashUI();

}

	bool CX2CashShop::BuyItemPopup::GetCheckedProductNoList(OUT vector<unsigned long>& vecProductNoList )
{

	// ���� �ϱ� ���� ���õ� ������ List ��ȸ
	for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
	{
		int itemID = m_vecCheckedItem[i];

		// �Ⱓ(����)�� ���� ���� �˻�
		map< int, int >::iterator mit = m_mapSelectedProductNo.find( itemID );
		if ( mit != m_mapSelectedProductNo.end() )
		{
			if ( mit->second == 0 )
				return false;

			int productIndex = mit->second - 1;

		/*	if( itemID == ITEM_ID_EVENT_NEW_SKILL_NOTE &&
				NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( ITEM_ID_EVENT_NEW_SPECIAL ) )
			{
				productIndex += 1;
			}*/
			std::vector<int> TempVec;
			int TempProDuctNum = 0;
			if(g_pData->GetItemManager()->DisCountItemID_Find(itemID,TempVec,TempProDuctNum) == true)
			{
				for(int i = 0; i < static_cast<int>( TempVec.size() ); ++i)
				{
					if( NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( TempVec[i] ) )
					{
						productIndex += 1;
						break;
					}
				}
			}
			CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
			if ( pCashItem != NULL )
			{
				if ( productIndex >= 0 && (int)pCashItem->m_vecKBillProductInfo.size() > productIndex )
				{
					KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[productIndex];
					vecProductNoList.push_back( kBillProductInfo.m_iProductNo );
				}
			}
		}
		else
		{
			return false;
		}

	}
	return true;
}


void CX2CashShop::BuyItemPopup::UpdateCashUI()
{
	if ( m_pDialog != NULL )
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashshop_Buywindow_Mycash" );
		if ( pStatic != NULL )
		{
			pStatic->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalMainCash() ).c_str() );
		}
		pStatic = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashshop_Buywindow_Myrest" );
		if ( pStatic != NULL )
		{
			//wstringstream wstrstm;
			//wstrstm << L"";
			//pStatic->SetString( 0, g_pMain->GetEDString( -10000 ).c_str() );

			int cashToBuy = 0;
			for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
			{
				int itemID = m_vecCheckedItem[i];
				CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
				if ( pCashItem == NULL )
					continue;

				map< int, int >::iterator mit;
				mit = m_mapSelectedProductNo.find( itemID );
				if ( mit != m_mapSelectedProductNo.end() )
				{
					int productIndex = mit->second - 1;
				/*	if( itemID == ITEM_ID_EVENT_NEW_SKILL_NOTE &&
						NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( ITEM_ID_EVENT_NEW_SPECIAL ) &&
						productIndex != -1 )
					{
						productIndex += 1;
					}*/
					std::vector<int> nTempVec;
					int nTempProDuctNum = 0;
					if(g_pData->GetItemManager()->DisCountItemID_Find(itemID,nTempVec,nTempProDuctNum) == true)
					{
						for(int i = 0; i < static_cast<int>( nTempVec.size() ); ++i)
						{
							if( NULL == g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( nTempVec[i] ) )
							{
								if( productIndex != -1 )
								{
									productIndex += 1;
									break;
								}
							}
						}
					}

					if ( productIndex >= 0 && productIndex < (int)pCashItem->m_vecKBillProductInfo.size() )
					{
						KBillProductInfo& kKBillProductInfo = pCashItem->m_vecKBillProductInfo[productIndex];
						cashToBuy += kKBillProductInfo.m_iSalePrice;
					}
				}
				/*
				else
				{
					if ( pCashItem->m_vecKNXBTProductInfo.empty() == false )
					{
						KNXBTProductInfo& kKNXBTProductInfo = pCashItem->m_vecKNXBTProductInfo[0];
						cashToBuy += kKNXBTProductInfo.m_ulSalePrice;
					}
				}
				*/
			}
			int restCash = 0;
			switch(GetUseCashType())
			{
			case KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH:
				restCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH) - cashToBuy;
				break;
			case KGlobalCashInfo::GCT_PUBLISHER_CASH:
				restCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_PUBLISHER_CASH) - cashToBuy;
				break;
			case KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT:
				restCash = g_pData->GetMyUser()->GetGlobalCash( KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT ) - cashToBuy;
				break;
			default:
				restCash = g_pData->GetMyUser()->GetGlobalMainCash() - cashToBuy;
				break;
			}
			if ( restCash >= 0 )
			{
				pStatic->SetString( 0, g_pMain->GetEDString( restCash ).c_str() );
			}
			else
			{
				restCash = abs( restCash );
				wstringstream wstrstm;
				wstrstm << L"-" << g_pMain->GetEDString( restCash ).c_str();
				pStatic->SetString( 0, wstrstm.str().c_str() );
			}
		}
	}
}



//{{ ����� 2010.11.1
bool CX2CashShop::BuyItemPopup:: RestCashCheck()
{
	if ( m_pDialog != NULL )
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashshop_Buywindow_Mycash" );
		if ( pStatic != NULL )
		{
			pStatic->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalMainCash() ).c_str() );
		}
		if ( pStatic != NULL )
		{
			int cashToBuy = 0;
			for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
			{
				int itemID = m_vecCheckedItem[i];
				CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
				if ( pCashItem == NULL )
					continue;

				map< int, int >::iterator mit;
				mit = m_mapSelectedProductNo.find( itemID );
				if ( mit != m_mapSelectedProductNo.end() )
				{
					int productIndex = mit->second - 1;

					if ( productIndex >= 0 && productIndex < (int)pCashItem->m_vecKBillProductInfo.size() )
					{
						KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[productIndex];
						cashToBuy += kBillProductInfo.m_iSalePrice;	// ������ϴ� �������� ������ �����´�.
					}
				}
			}

			[[maybe_unused]] int restCash = 0;
			switch(GetUseCashType())
			{
			case KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH:
				restCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH) - cashToBuy;
				break;
			case KGlobalCashInfo::GCT_PUBLISHER_CASH:
				restCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_PUBLISHER_CASH) - cashToBuy;
				break;
			case KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT:
				restCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT) - cashToBuy;
				break;
			default:
				restCash = g_pData->GetMyUser()->GetGlobalMainCash() - cashToBuy;
				break;
			}

#if X2_DEV_TOOLS
			return true;
#else // defined(_IN_HOUSE_) && ( defined(SERV_COUNTRY_US) || defined( CLIENT_COUNTRY_ID ) )
			if ( restCash >= 0 )
				return true;
#endif // defined(_IN_HOUSE_) &&  ( defined(SERV_COUNTRY_US) || defined( CLIENT_COUNTRY_ID ) )
		}

	}
	return false;
}
//}} ����� 2010.11.1


void CX2CashShop::Send_EGS_VISIT_CASH_SHOP_NOT( const bool bOpen_ ) const // û�� öȸ ������ �ȿ� �־ �ؿ��� ����
{
	KEGS_VISIT_CASH_SHOP_NOT kPacket;
	kPacket.m_bEnterCashShop = bOpen_;

	g_pData->GetServerProtocol()->SendPacket( EGS_VISIT_CASH_SHOP_NOT, kPacket );
}

void CX2CashShop::BuyItemPopup::InitbyOpen()
{
	m_vecCheckedItem = m_vecItemID;
	m_mapSelectedProductNo.clear();		//itemID, CashData�� KNXBTProductInfo ���� �ε���

	SetCheckAllItem( true );

	CKTDGUICheckBox* pCheckBoxMoveInven = (CKTDGUICheckBox*)m_pDialog->GetControl( L"g_pCheckBoxCashShop_BuyWindow_Slot" );
	if ( pCheckBoxMoveInven != NULL )
	{
		pCheckBoxMoveInven->SetCheckedPure( true );
	}
}


void CX2CashShop::BuyItemPopup::ResetNowPage()
{
	CX2PageMgrItem::ResetNowPage();

	//���⿡ ���� �� ���� ĳ�÷���
	//���� ĳ��

	UpdateCashUI();

	if ( m_pDialog != NULL )
	{

		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashshop_Buywindow_Font" );
		if ( pStatic != NULL )
		{
			wstringstream wstrstm;
			wstrstm << GetNowPage() << L"/" << GetMaxPage();
			pStatic->SetString( 0, wstrstm.str().c_str() );
		}


	}

	int j = 0;
	for ( int i = (m_NowPage-1) * m_MaxSlotNumPerPage; i < (m_NowPage)*m_MaxSlotNumPerPage && j < (int)m_vecPageSlot.size() && i < (int)m_vecItemID.size(); i++, j++ )
	{
		int itemID = m_vecItemID[i];
		CX2CashShop::BuyItemPopupSlot* pPageSlot = (CX2CashShop::BuyItemPopupSlot*)m_vecPageSlot[j];

		bool bCheck = false;
		for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
		{
			int tempItemID = m_vecCheckedItem[i];
			if ( itemID == tempItemID )
			{
				bCheck = true;
				break;
			}
		}

		if ( bCheck == true )
			pPageSlot->SetCheck( true );
		else
			pPageSlot->SetCheck( false );

		map< int, int >::iterator mit;
		mit = m_mapSelectedProductNo.find( itemID );
		if ( mit != m_mapSelectedProductNo.end() )
		{
			pPageSlot->SetCash( mit->second );
		}
		else
		{
			CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );
			if ( pCashItem != NULL )
			{
				pPageSlot->SetCash( 0 );
			}
		}
	}
}

void CX2CashShop::CashItemToInvenPopup::Reset( vector< int >& vecItemID, vector< KBillOrderInfo >& vecKBillOrderInfo,
											  vector< KBillPackageInfo >& vecKBillPackageInfo )
{
	m_vecKBillOrderInfo = vecKBillOrderInfo;
	m_vecKBillPackageInfo = vecKBillPackageInfo;

	CX2PageMgrItem::Reset( vecItemID );
}

void CX2CashShop::CashItemToInvenPopup::ChangeItemOption( CKTDGUIDialogType pDialog, int comboBoxIndex )
{
	CX2CashShop::CashItemToInvenPopupSlot* pItemPopupSlot = (CX2CashShop::CashItemToInvenPopupSlot*)GetPageSlot( pDialog );
	if ( pItemPopupSlot != NULL )
	{
		map< int, int >::iterator mit;
		mit = m_mapSelectedOption.find( pItemPopupSlot->GetItemID()  );
		if ( mit != m_mapSelectedOption.end() )
		{
			mit->second = comboBoxIndex;
		}
		else
		{
			m_mapSelectedOption.insert( std::make_pair( pItemPopupSlot->GetItemID(), comboBoxIndex ) );
		}
	}
}

/** @function : GetSelectedOptionListAndGroupID
	@brief : ������ ���Ͽɼ��� ����Ʈ�� �����ۺ� �׷���̵� ����Ʈ ���� �Լ�
	@param : ������ ���Ͽɼ� ����Ʈ(mapSelectedOptionList), �����ۺ� �׷���̵� ����Ʈ(mapOptionGroupIdList_)
	@return : �����ϸ� true, �����ϸ� false
*/
bool CX2CashShop::CashItemToInvenPopup::GetSelectedOptionListAndGroupID( OUT map<int, int>& mapSelectedOptionList_, OUT map<int, int>& mapOptionGroupIdList_ )
{
	if ( m_vecKBillOrderInfo.empty() == true )
	{
		return false;
	}

	KBillOrderInfo& kCashOrderInfo = m_vecKBillOrderInfo[0];

	if ( kCashOrderInfo.m_byteProductKind == 1 )
	{
		vector< KBillPackageInfo > vecPackageInfo;
		unsigned long ulTranNo = kCashOrderInfo.m_iTransNo;
		GetPackageInfo( ulTranNo, vecPackageInfo );

		for ( int j = 0; j < (int)vecPackageInfo.size(); j++ )
		{
			KBillPackageInfo& kCashPackageInfo = vecPackageInfo[j];
			int itemID = kCashPackageInfo.m_iProductID;

			bool bCheckedItem = false;
			for ( int k = 0; k < (int)m_vecCheckedItem.size(); k++ )
			{
				int checkedItemID = m_vecCheckedItem[k];
				if ( checkedItemID == itemID )
				{
					bCheckedItem = true;
					break;
				}
			}

			if ( bCheckedItem == false )
				continue;

			if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( itemID ) == false )
				continue;


			map< int, int >::iterator mit;
			mit = m_mapSelectedOption.find( itemID );
			if ( mit != m_mapSelectedOption.end() && !kCashPackageInfo.m_vecSocketOption.empty() )
			{
				const UINT uiAttributeIndex = mit->second;

				pair<int,int> pairGroupIdAndSelectedSocketId = GetSelectedOptionList( kCashPackageInfo.m_vecSocketOption[0], uiAttributeIndex, itemID );

				const int iGroupdID = pairGroupIdAndSelectedSocketId.first;
				const int iSocketOptionToBeSelected = pairGroupIdAndSelectedSocketId.second;

				if ( 0 != iSocketOptionToBeSelected && 0 != iGroupdID )
				{
					mapSelectedOptionList_.insert( std::make_pair( itemID, iSocketOptionToBeSelected ) );
					mapOptionGroupIdList_.insert( std::make_pair( iSocketOptionToBeSelected, iGroupdID ) );
				}
				else
					return false;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		int itemID =  kCashOrderInfo.m_kBillProductInfo.m_iProductID;
		if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( itemID ) == true )
		{

			bool bCheckedItem = false;
			for ( int k = 0; k < (int)m_vecCheckedItem.size(); k++ )
			{
				int checkedItemID = m_vecCheckedItem[k];
				if ( checkedItemID == itemID )
				{
					bCheckedItem = true;
					break;
				}
			}

			if ( bCheckedItem == false )
				return true;

			map< int, int >::iterator mit;
			mit = m_mapSelectedOption.find( itemID );
			if ( mit != m_mapSelectedOption.end() && !kCashOrderInfo.m_vecSocketOption.empty() )
			{
				const UINT uiAttributeIndex = mit->second;
				pair<int,int> pairGroupIdAndSelectedSocketId = GetSelectedOptionList( kCashOrderInfo.m_vecSocketOption[0], uiAttributeIndex, itemID );

				const int iGroupdID = pairGroupIdAndSelectedSocketId.first;
				const int iSocketOptionToBeSelected = pairGroupIdAndSelectedSocketId.second;

				if ( 0 != iSocketOptionToBeSelected && 0 !=iGroupdID )
				{
					mapSelectedOptionList_.insert( std::make_pair( itemID, iSocketOptionToBeSelected ) );
					mapOptionGroupIdList_.insert( std::make_pair( iSocketOptionToBeSelected, iGroupdID ) );
				}
				else
					return false;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

/** @function : GetSelectedOptionList
	@brief : ĳ�þƹ�Ÿ�� ������ �� ������ ���� �ɼ��� ������ �Լ�
	@param : ĳ�þƹ�Ÿ�� ���� �׷���̵�(wszSocketItemGroupID_), �޺��ڽ����� ������ �ε���(iSelectedIndex_), ���������� ������ID(iItemID_)
	@return : <GroupID,SocketID>
*/
pair<int,int> CX2CashShop::CashItemToInvenPopup::GetSelectedOptionList( const int iSocketItemGroupID_, const int iSelectedIndex_, const int iItemID_ )
{
	vector<int> vecSocketId_;
	const int iSocketGroupID = iSocketItemGroupID_;
	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( iItemID_ );
	const int iReturnSocketGroupID
		=  NULL != pItemTemplet ? g_pData->GetSocketItem()->GetSocketIdListForCashAvatar( iSocketGroupID, vecSocketId_, pItemTemplet->GetItemType() ) : 0;

	int iSelectedSocketID = 0;
	if ( 0 >= iSelectedIndex_ )		/// iSelectedIndex_�� ��ȿ�� �ɼ��� 1���� ���۵�
		iSelectedSocketID = 0;
	else if ( vecSocketId_.size() >= static_cast<UINT>( iSelectedIndex_ ) )	/// 1���� ���� �ϱ� ������
		iSelectedSocketID = vecSocketId_[iSelectedIndex_ - 1];

	return pair<int,int>( iReturnSocketGroupID, iSelectedSocketID );
}


void CX2CashShop::CashItemToInvenPopup::GetCheckedPackageInfo( vector< KBillPackageInfo >& vecPackageInfo )
{
 	if ( m_vecKBillOrderInfo.empty() == false )
 	{
 		KBillOrderInfo& kKBillOrderInfo = m_vecKBillOrderInfo[0];

 		for ( int i = 0; i < (int)m_vecKBillPackageInfo.size(); i++ )
 		{
 			KBillPackageInfo& kKBillPackageInfo = m_vecKBillPackageInfo[i];
 			if ( //** ��Ű��NO  kKBillPackageInfo.m_iOrderNo == kKBillOrderInfo.m_iOrderNo &&
				kKBillPackageInfo.m_iPackageNo == kKBillOrderInfo.m_iTransNo //&&
 				//kKBillPackageInfo.m_ulProductNo == kKBillOrderInfo.m_kBillProductInfo.m_iProductNo
				)
 			{


 				if ( std::find( m_vecCheckedItem.begin(), m_vecCheckedItem.end(), kKBillPackageInfo.m_iProductID ) != m_vecCheckedItem.end() )
 				{
 					vecPackageInfo.push_back( kKBillPackageInfo );
 				}
 			}
 		}
 	}


 }

void CX2CashShop::CashItemToInvenPopup::SetCheckAllItem( bool bCheck )
{
	if ( bCheck == true )
	{
		m_vecCheckedItem = m_vecItemID;
	}
	else
	{
		m_vecCheckedItem.clear();
	}

	ResetNowPage();
}

void CX2CashShop::CashItemToInvenPopup::ChangeCheckItem( CKTDGUIDialogType pDialog, bool bCheck )
{
	if ( pDialog == NULL )
		return;

	CX2CashShop::CashItemToInvenPopupSlot* pCashItemToInvenPopupSlot = (CX2CashShop::CashItemToInvenPopupSlot*)GetPageSlot( pDialog );
	if ( pCashItemToInvenPopupSlot == NULL )
		return;

	int itemIDToFind = pCashItemToInvenPopupSlot->GetItemID();

	if ( bCheck == true )
	{
		bool bFind = false;
		for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
		{
			int itemID = m_vecCheckedItem[i];
			if ( itemID == itemIDToFind )
			{
				bFind = true;
				break;
			}
		}
		if ( bFind == false )
		{
			m_vecCheckedItem.push_back( itemIDToFind );
		}
	}
	else
	{
		for ( int i = 0; i < (int)m_vecCheckedItem.size(); i++ )
		{
			int itemID = m_vecCheckedItem[i];
			if ( itemID == itemIDToFind )
			{
				m_vecCheckedItem.erase( m_vecCheckedItem.begin() + i );
				i--;
				break;
			}
		}
	}

	ResetNowPage();
}

void CX2CashShop::CashItemToInvenPopup::InitbyOpen()
{
	m_mapSelectedOption.clear();
	m_vecCheckedItem = m_vecItemID;

	SetCheckAllItem( true );

	CKTDGUICheckBox* pCheckBoxMoveInven = (CKTDGUICheckBox*)m_pDialog->GetControl( L"g_pCheckBoxCashShop_MoveInventory" );
	if ( pCheckBoxMoveInven != NULL )
	{
		pCheckBoxMoveInven->SetCheckedPure( true );
	}
}

void CX2CashShop::CashItemToInvenPopup::ResetNowPage()
{
	if ( m_pDialog != NULL )
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDialog->GetControl( L"g_pStaticCashshop_Movewindow_Font" );
		if ( pStatic != NULL )
		{
			wstringstream wstrstm;
			wstrstm << GetNowPage() << L"/" << GetMaxPage();
			pStatic->SetString( 0, wstrstm.str().c_str() );
		}
	}

	int j = 0;
	for ( int i = (m_NowPage-1) * m_MaxSlotNumPerPage; i < (m_NowPage)*m_MaxSlotNumPerPage && j < (int)m_vecPageSlot.size() && i < (int)m_vecItemID.size(); i++, j++ )
	{
		int itemID = m_vecItemID[i];
		CX2CashShop::CashItemToInvenPopupSlot* pPageSlot = (CX2CashShop::CashItemToInvenPopupSlot*)m_vecPageSlot[j];
		if ( pPageSlot != NULL )
		{
			if ( m_vecKBillOrderInfo.empty() == false )
				pPageSlot->SetKBillOrderInfo( m_vecKBillOrderInfo[0] );
		}

		bool bCheck = false;
		for ( int j = 0; j < (int)m_vecCheckedItem.size(); j++ )
		{
			int tempItemID = m_vecCheckedItem[j];
			if ( itemID == tempItemID )
			{
				bCheck = true;
				break;
			}
		}

		if ( bCheck == true )
			pPageSlot->SetCheck( true );
		else
			pPageSlot->SetCheck( false );

		//�˻��ؼ� �� ���δ�Ʈ�� �´� ��Ű�� ��ǰ ������ ����..
//�˻��ؼ� �� ���δ�Ʈ�� �´� ��Ű�� ��ǰ ������ ����..
		//** ��Ű���� Ǯ���� �����ϱ�...�ϴ� ��Ű�� ������ �ּ�ó��. ���߿� ó������
 		if ( m_vecKBillOrderInfo.empty() == false )
 		{
 			for ( int packageIndex = 0; packageIndex < (int)m_vecKBillPackageInfo.size(); packageIndex++ )
 			{
 				KBillPackageInfo& kKBillPackageInfo = m_vecKBillPackageInfo[packageIndex];
 				KBillOrderInfo& kKBillOrderInfo = m_vecKBillOrderInfo[0];
 				//KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( (int)kKBillPackageInfo.m_ulProductNo );
				//CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( kKBillOrderInfo.m_kBillProductInfo.m_iProductID );
				KBillProductInfo* pKBillProductInfo = &kKBillOrderInfo.m_kBillProductInfo;


 				if ( NULL != pKBillProductInfo &&
 					//**OrderNo kKBillPackageInfo.m_ulOrderNo == kKBillOrderInfo.m_ulOrderNo &&
					kKBillPackageInfo.m_iPackageNo == kKBillOrderInfo.m_iTransNo &&
 					//kKBillPackageInfo.m_vecProductNo[packageIndex] == kKBillOrderInfo.m_kBillProductInfo.m_iProductNo &&
 					m_vecItemID[i] == kKBillPackageInfo.m_iProductID
					)
 				{
 					pPageSlot->SetKBillPackageInfo( kKBillPackageInfo );
 					break;
 				}
 			}
 		}

	}

	CX2PageMgrItem::ResetNowPage();

	j = 0;
	for ( int i = (m_NowPage-1) * m_MaxSlotNumPerPage; i < (m_NowPage)*m_MaxSlotNumPerPage && j < (int)m_vecPageSlot.size() && i < (int)m_vecItemID.size(); i++, j++ )
	{
		int itemID = m_vecItemID[i];
		CX2CashShop::CashItemToInvenPopupSlot* pPageSlot = (CX2CashShop::CashItemToInvenPopupSlot*)m_vecPageSlot[j];
		if ( pPageSlot != NULL )
		{
			map< int, int >::iterator mit;
			mit = m_mapSelectedOption.find( itemID );
			if ( mit != m_mapSelectedOption.end() )
			{
				pPageSlot->SetOption( mit->second );
			}
			else
			{
				pPageSlot->SetOption( 0 );
			}
		}
	}
}

void CX2CashShop::CashItemToInvenPopup::GetPackageInfo( unsigned long ulTranNo, vector< KBillPackageInfo >& vecKBillPackageInfo )
{
	for ( int i = 0; i < (int)m_vecKBillPackageInfo.size(); i++ )
	{
		KBillPackageInfo& kKBillPackageInfo = m_vecKBillPackageInfo[i];
		if ( static_cast<size_t>( kKBillPackageInfo.m_iPackageNo ) == ulTranNo )
			vecKBillPackageInfo.push_back( kKBillPackageInfo );
	}
}



CX2CashShop::PresentItemPopup::PresentItemPopup()
{
	m_pDLGPresent = NULL;
	m_ItemID = 0;
	m_bOpen = false;

	m_bChoicedItem = false;
	m_SelectedProductNum = 0;

	m_iUseCashType = KGlobalCashInfo::GCT_PUBLISHER_CASH;
}

CX2CashShop::PresentItemPopup::~PresentItemPopup()
{
	SAFE_DELETE_DIALOG( m_pDLGPresent );
}

void CX2CashShop::PresentItemPopup::SetOpen( bool bOpen, int itemID )
{
	m_bOpen = bOpen;
	m_ItemID = itemID;

	if ( bOpen == true )
	{
		InitUI();
	}
	else
	{
		if ( m_pDLGPresent != NULL )
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGPresent, 0, false );

		m_pDLGPresent = NULL;
	}
}

bool CX2CashShop::PresentItemPopup::GetOpen()
{
	return m_bOpen;
}


void CX2CashShop::PresentItemPopup::UpdateCash()
{
	if ( m_pDLGPresent != NULL )
	{
		CKTDGUIStatic* pStaticMyCash = (CKTDGUIStatic*)m_pDLGPresent->GetControl( L"g_pStaticCashshop_PresentWindow_Mycash" );
		if ( pStaticMyCash != NULL )
		{
			pStaticMyCash->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalMainCash() ).c_str() );
		}
		CKTDGUIStatic* pStaticRemainCash = (CKTDGUIStatic*)m_pDLGPresent->GetControl( L"g_pStaticCashshop_PresentWindow_Myrest" );
		if ( pStaticRemainCash != NULL )
		{
			//���� �ִ� ������ - ���õ� �������� ����


			unsigned long productNo = GetSelectedProductNo();
			if ( productNo == 0 )
			{
				switch(GetUseCashType())
				{
				case KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH:
					pStaticRemainCash->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalCash( KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH ) ).c_str() );
					break;
				case KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT:
					pStaticRemainCash->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalCash( KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT ) ).c_str() );
					break;
				default:
					pStaticRemainCash->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalMainCash() ).c_str() );
					break;
				}
			}
			else
			{
				KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( productNo );
				if ( pKBillProductInfo != NULL )
				{
					wstringstream wstrstm;
					int remainCash = 0;
					switch( GetUseCashType() )
					{
					case KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH:
						remainCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_PUBLISHER_ELSWORD_CASH) - pKBillProductInfo->m_iSalePrice;
						break;
					case KGlobalCashInfo::GCT_PUBLISHER_CASH:
						remainCash = g_pData->GetMyUser()->GetGlobalCash(KGlobalCashInfo::GCT_PUBLISHER_CASH) - pKBillProductInfo->m_iSalePrice;
						break;
					case KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT:
						remainCash = g_pData->GetMyUser()->GetGlobalCash( KGlobalCashInfo::GCT_KOG_ELSWORD_BONUS_POINT ) - pKBillProductInfo->m_iSalePrice;
						break;
					default:
						remainCash = g_pData->GetMyUser()->GetGlobalMainCash() - pKBillProductInfo->m_iSalePrice;
						break;
					}

					if ( remainCash < 0 )
					{
						remainCash = abs( remainCash );
						wstrstm << L"-" << g_pMain->GetEDString( remainCash );
					}
					else
					{
						wstrstm << g_pMain->GetEDString( remainCash );
					}
					pStaticRemainCash->SetString( 0, wstrstm.str().c_str() );
				}
				else
				{
					pStaticRemainCash->SetString( 0, g_pMain->GetEDString( g_pData->GetMyUser()->GetGlobalMainCash() ).c_str() );
				}
			}

		}
	}
}


unsigned long CX2CashShop::PresentItemPopup::GetSelectedProductNo()
{
	if ( m_pDLGPresent != NULL )
	{
		CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDLGPresent->GetControl( L"g_pComboCash_Shop_Present_Option" );
		if ( pComboBox != NULL )
		{
			int selectedIndex = pComboBox->GetSelectedItemIndex();
			if ( selectedIndex <= 0 )
				return 0;
			else
			{
				//���⼭ �Žñ� �غη�����??
				//��Ͽ� �°Բ� �ε����ؼ� �������������~~~


				int productIndex = selectedIndex - 1;

				CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( m_ItemID );
				if ( pCashItem != NULL )
				{
					if ( productIndex >= 0 && (int)pCashItem->m_vecKBillProductInfo.size() > productIndex )
					{
						KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[productIndex];
						return kBillProductInfo.m_iProductNo;
					}
				}
			}
		}
	}

	return 0;
}

void CX2CashShop::PresentItemPopup::SetSelectFriend( int index )
{
	if ( index == 0 )
		return;

	if ( m_pDLGPresent != NULL )
	{
		CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDLGPresent->GetControl( L"g_pComboBoxPresent_Select_User" );
		if ( pComboBox != NULL )
		{
			CKTDGUIComboBox::ComboBoxItem* pItem = pComboBox->GetItem( index );
			if ( pItem != NULL )
			{
				// ������ ���� �ؽ�Ʈ �ڽ����ٰ� �ɾ����� �ش� str..pItem->strText

				CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*)m_pDLGPresent->GetControl( L"IME_Nicname" );
				if ( pIMEEditBox != NULL )
				{
					return pIMEEditBox->SetText( pItem->strText );
				}
			}
		}
	}
}

wstring CX2CashShop::PresentItemPopup::GetNickNameToPresent()
{
	if ( m_pDLGPresent != NULL )
	{
		CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*)m_pDLGPresent->GetControl( L"IME_Nicname" );
		if ( pIMEEditBox != NULL )
		{
			wstring tempString = pIMEEditBox->GetText();
			tempString = tempString.substr( 0, 20 );

			return tempString.c_str();
		}
	}

	return L"";
}

wstring CX2CashShop::PresentItemPopup::GetPresentMsg()
{
	if ( m_pDLGPresent != NULL )
	{
		CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*)m_pDLGPresent->GetControl( L"IME_Present_Message" );
		if ( pIMEEditBox != NULL )
		{
			wstring tempString = pIMEEditBox->GetText();
			tempString = tempString.substr( 0, 20 );

			return tempString.c_str();
		}
	}

	return L"";
}

wstring CX2CashShop::PresentItemPopup::GetPassword()
{
	if ( m_pDLGPresent != NULL )
	{
		CKTDGUIEditBox* pIMEEditBox = (CKTDGUIEditBox*)m_pDLGPresent->GetControl( L"IME_Password" );
		if ( pIMEEditBox != NULL )
		{
			wstring tempString = pIMEEditBox->GetText();
			tempString = tempString.substr( 0, 20 );

			return tempString.c_str();
		}
	}

	return L"";
}

void CX2CashShop::PresentItemPopup::InitUI()
{
	if ( m_pDLGPresent != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGPresent, 0, false );

	m_pDLGPresent = NULL;
	m_pDLGPresent = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cash_Shop_Popup_Present.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGPresent );


	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( m_ItemID );
    bool    bItemTempletValid = pItemTemplet != NULL;

	SetUseCashType((int)KGlobalCashInfo::GCT_PUBLISHER_CASH);

	//������ �̸� ����
	CKTDGUIStatic* pItemName = (CKTDGUIStatic*)m_pDLGPresent->GetControl( L"g_pStatic_Item_ID_1Line" );
	if ( pItemName != NULL && pItemName->GetString(0) != NULL && bItemTempletValid == true )
	{
		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetName(), 214, pItemName->GetString(0)->fontIndex );
		pItemName->SetString( 0, tempName.c_str() );
	}

	CKTDGUIStatic* pStaticSlot = (CKTDGUIStatic*)m_pDLGPresent->GetControl( L"g_pStaticCashShop_PresentWindow_ItemSlot" );
	if ( pStaticSlot != NULL && pStaticSlot->GetPicture(0) != NULL && bItemTempletValid == true )
	{
        pStaticSlot->GetPicture(0)->SetTex( pItemTemplet->GetShopImage() );
	}

	CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDLGPresent->GetControl( L"g_pComboCash_Shop_Present_Option" );
	if ( pComboBox != NULL )
	{
		pComboBox->RemoveAllItems();
		CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( m_ItemID );

		bool bPeriod = false;
		for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
		{
			KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
			if( kBillProductInfo.m_cPeriod != 0 )
			{
				bPeriod = true;
				break;
			}
		}

		for ( int i = 0; i < (int)pCashItem->m_vecKBillProductInfo.size(); i++ )
		{
			KBillProductInfo& kBillProductInfo = pCashItem->m_vecKBillProductInfo[i];
			wstringstream wstrstm;
			//wstrstm << kBillProductInfo


			if ( kBillProductInfo.m_cPeriod != 0 )
			{
				if ( i == 0 )
					pComboBox->AddItem( GET_STRING( STR_ID_35 ), NULL, false );

				wstrstm << (int)kBillProductInfo.m_cPeriod << GET_STRING( STR_ID_14 ) << L"/";
			}
			else
			{
				if ( i == 0 )
					pComboBox->AddItem( GET_STRING( STR_ID_36 ), NULL, false );

				if( bPeriod == true && kBillProductInfo.m_cQuantity == 1 )
					wstrstm << GET_STRING( STR_ID_13858 ) << L"/";
				else
				wstrstm << (int)kBillProductInfo.m_cQuantity << GET_STRING( STR_ID_24 ) << L"/";
			}

			wstrstm << kBillProductInfo.m_iSalePrice << GET_STRING( STR_ID_34 );

			pComboBox->AddItem( wstrstm.str().c_str(), NULL, false );
		}
	}


	pComboBox = (CKTDGUIComboBox*)m_pDLGPresent->GetControl( L"g_pComboBoxPresent_Select_User" );
	if ( pComboBox != NULL )
	{
		pComboBox->RemoveAllItems();
		pComboBox->AddItem( GET_STRING( STR_ID_39 ), NULL );

		vector< wstring > vecFriendNameList;
		g_pData->GetMessenger()->GetFriendNameList( vecFriendNameList );
		for ( int i = 0; i < (int)vecFriendNameList.size(); i++ )
		{
			wstring friendName = vecFriendNameList[i];
			pComboBox->AddItem( friendName.c_str(), NULL );
		}
	}

	UpdateCash();
}


void CX2CashShop::AddDelayedMail( KEGS_RECEIVE_LETTER_NOT& kEvent )
{
	m_vecDelayedMail.push_back( kEvent );
}

void CX2CashShop::PushDelayedMails()
{
	if( g_pMain && g_pMain->GetPostBox() != NULL )
	{
		for( int i = 0; i < (int)m_vecDelayedMail.size(); ++i )
		{
			KSerBuffer buff;
			Serialize( &buff, &m_vecDelayedMail[i] );

			g_pMain->GetPostBox()->Handler_EGS_RECEIVE_LETTER_NOT( 0, 0, 0, (LPARAM)&buff );
		}
		m_vecDelayedMail.resize( 0 );
	}
}


void CX2CashShop::MyUniyHandling( double fTime, float fElapsedTime )
{
	if( m_pUnitViewerUI != NULL )
	{
		float fRotSpeed = 13.0f;


		if ( CX2UnitViewerUI::GetUnitClicked() == true )
		{
			D3DXVECTOR3 RotateY = D3DXVECTOR3(0, -((float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXDelta() * m_fMouseSensitivity), 0);
			m_pUnitViewerUI->GetMatrix().RotateRel(RotateY);
			m_bRotateReturn = false;

			fRotSpeed = 13.0f;
		}
		else
		{
			m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );



			m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
			int addSpeed = 20;

			if ( m_bRotateReturn == false )
			{
				fRotSpeed -= (fElapsedTime * addSpeed);

				if ( fRotSpeed <= 0 )
				{
					fRotSpeed = fElapsedTime;
				}

				if (m_fRemainRotY >= 0.0f &&  m_fRemainRotY < 3.14f )
				{

					if ( m_fRemainRotY <= 0.0f )
					{
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						m_fRemainRotY = 0.0f;
						m_bRotateReturn = true;
					}
					else
					{
						m_fRemainRotY -= (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY <= 0.0f )
						{
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
							m_fRemainRotY = 0.0f;
							m_bRotateReturn = true;
						}
					}
				}
				else if ( m_fRemainRotY > 3.14f && m_fRemainRotY <= 6.18f  )
				{
					if ( m_fRemainRotY >= 6.18f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY += (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY >= 6.18f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
				else if ( m_fRemainRotY >= -3.14f && m_fRemainRotY <= 0.0f )
				{
					if ( m_fRemainRotY >= 0.0f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY += (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY >= 0.0f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
				else if (  m_fRemainRotY >= -6.28f && m_fRemainRotY <= -3.14f )
				{
					if ( m_fRemainRotY <= -6.28f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY -= (fElapsedTime*fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY <= -6.28f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}

				}
			}
		}

		m_pUnitViewerUI->OnFrameMove( fTime, fElapsedTime );

		if ( m_pUnitViewerUI->GetXSkinAnim()->GetState() == CKTDGXSkinAnim::XAS_WAIT )
		{
			m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_WAIT );
		}
	}
}

/** @function : PetHandling
	@breif : �� ��� ������ ���� �����ӹ��� ����
*/
void CX2CashShop::PetHandling( double fTime, float fElapsedTime )
{
	m_pNPCViewerUI->OnFrameMove( fTime, fElapsedTime );
}

/** @UpdateDLGNpcViewerUI : UpdateDLGNpcViewerUI
	@biref	: �� ���ֺ�� ��ü ���� �� ���ֺ�� ������ ���̾�α� ����
*/
void CX2CashShop::UpdateDLGPetViewerUI()
{
	if( NULL == m_pNPCViewerUI )
	{
		m_pNPCViewerUI = CX2NPCUnitViewerUI::Create();
	}

	if( NULL == m_pDLGPetPreview )
	{
		m_pDLGPetPreview = new CKTDGUIDialog( g_pMain->GetNowState(), NULL);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGPetPreview );
		m_pDLGPetPreview->SetLayer(XDL_NORMAL_2);
		m_pDLGPetPreview->SetFront(true);
		if( NULL != m_pNPCViewerUI )
		{
			m_pDLGPetPreview->SetHasUnit(static_cast<CKTDGObject*>(m_pNPCViewerUI));
		}
	}
	ResetPetViewer();
}
void CX2CashShop::PetPreview( int iItemID_ )
{
	if ( NULL != g_pData->GetItemManager() )
	{
		int iPetID = g_pData->GetItemManager()->GetPreviewPetIdByItemID( iItemID_ );

		CX2ItemManager::PREVIEW_PET_TYPE ePreviewPetType_ = CX2ItemManager::PPT_PET;
		// �Ϲ� �� �����߿� ���ٸ� ���̵������� üũ
		if( -1 == iPetID )
		{
			iPetID = g_pData->GetItemManager()->GetPreviewRidingPetIdByItemID( iItemID_ );
			// ���̵��굵 �ƴ϶�� �ҷ����� �ʱ�.
			if( -1 == iPetID )
				return;

			ePreviewPetType_ = CX2ItemManager::PPT_RIDING_PET;
		}

		ResetPetViewer( iPetID, iItemID_, ePreviewPetType_);
	}
}

/** @function : ResetPetViewer
	@breif : ������ ������ �� ���� ��� ����
	@param : �� ID (iPetID_), ����� ������ ID (iItemID)
*/
void CX2CashShop::ResetPetViewer(int iPetID_/* = -1*/, int iItemID /*= -1*/, CX2ItemManager::PREVIEW_PET_TYPE ePreviewPetType_/* = PPT_PET*/)
{
	if( NULL != m_pNPCViewerUI )
	{
		if( iPetID_ < 0 || m_iPrePetItemID == iItemID )
		{
			m_pNPCViewerUI->SetShowObject(false);
			m_iPrePetItemID = -1;
			return;
		}
		else
		{
			m_iPrePetItemID = iItemID; //������ ���� �ѹ� �� ������ ������Ű�� ���� ��
			switch( ePreviewPetType_ )
			{
			case CX2ItemManager::PPT_RIDING_PET:
				{
					m_pNPCViewerUI->ResetRidingPet( iPetID_ );
					m_pNPCViewerUI->SetScale( D3DXVECTOR3(0.5f,0.5f,0.5f));
				} break;
			case CX2ItemManager::PPT_PET:
			default:
				{
					CX2PetManager::PetTemplet *pTemplet = g_pData->GetPetManager()->GetPetTemplet( (CX2PetManager::PET_UNIT_ID)iPetID_ );
					m_pNPCViewerUI->ResetPet( iPetID_ , pTemplet->m_Evolution_All_Step );
					m_pNPCViewerUI->SetScale( D3DXVECTOR3(1.f,1.f,1.f));
				} break;
			}
			m_pNPCViewerUI->SetShowObject( true );
			m_pNPCViewerUI->PlayAnim( L"Wait", CKTDGXSkinAnim::XAP_LOOP, true );
			m_pNPCViewerUI->SetOverUI(true);
			m_pNPCViewerUI->SetPosition(D3DXVECTOR3(-270,0,0));
		}
	}
}

/** @function : SetShowOtherUI
	@param : ĳ���� ���� ���� ���¿� ���� show���� �����ؾ� �Ǵ� UI ����
*/
void CX2CashShop::SetShowOtherUI( bool bShowCashop_ )
{
	if( NULL != g_pInstanceData && NULL != g_pInstanceData->GetMiniMapUI() )
		g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( CX2MiniMapUI::MMT_DUNGEON, !bShowCashop_ );

	if( NULL != g_pX2Game && NULL != g_pX2Game->GetDLGMyScore() )
		g_pX2Game->GetDLGMyScore()->SetShow(!bShowCashop_);
}


//�ʵ� �����, �ش� �ʵ� ���� ĳ���� �������� ������ ���� ó��
//���� ������ X2Game�� �����Ǿ� ���� �ʱ� ������ �ӽ� ����
void CX2CashShop::GetEnterCashShopUserList( OUT std::vector< UidType >& EnterCashShopUserList_ )
{
	EnterCashShopUserList_.clear();
	EnterCashShopUserList_ = m_vecEnterCashShopUser;
}

void CX2CashShop::ShowInvenEquipUI( bool bVal_ )
{
	// true : ���� ���̴� �����̹Ƿ� '�����' UI �����ֱ�
	// false : ���� �Ⱥ��̴� �����̹Ƿ� '���̱�' UI �����ֱ�

	if( NULL != m_pDLGFront )
	{
		CKTDGUIButton* pButton = static_cast<CKTDGUIButton*>(m_pDLGFront->GetControl(L"MyAvatar_Show"));
		if( NULL != pButton )
			pButton->SetShow( !bVal_ );

		pButton = static_cast<CKTDGUIButton*>(m_pDLGFront->GetControl(L"MyAvatar_Hide"));
		if( NULL != pButton )
			pButton->SetShow( bVal_ );
	}
}



//=======================================================================================//
//--------------------------------- ES_GLOBAL_CODE_AREA ---------------------------------//
//=======================================================================================//


void CX2CashShop::CashShopSlot::SetShowPresentButton( bool bShow )
{
	switch(m_SlotType)
	{
	case CX2Slot::ST_CASH_SHOP_MAIN_NEW:
	case CX2Slot::ST_CASH_SHOP_MAIN_HOT:
	case CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND:
		{
			CKTDGUIButton* pPresentSmallButton = reinterpret_cast< CKTDGUIButton* >( m_pDLGCashShopSlot->GetControl( L"MainItemSlot_Present" ) );

			if(pPresentSmallButton != NULL)
			{
				pPresentSmallButton->SetShowEnable( true, bShow );
			}
		}
		break;
	case CX2Slot::ST_CASH_SHOP_SUB:
	case CX2Slot::ST_CASH_SHOP_SUB_SPECIAL:
		{
			CKTDGUIButton* pPresentButton = reinterpret_cast< CKTDGUIButton* >( m_pDLGCashShopSlot->GetControl( L"Present" ) );

			if(pPresentButton != NULL)
			{
				pPresentButton->SetShowEnable( true, bShow );
			}
		}
		break;
	default:
		{
			ASSERT( !"Unexpected CashShop Slot Type" );
		}
		break;
	}
}

bool CX2CashShop::CashShopSlotManager::IsPossiblePresent( int itemID )
{
	if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_Level <= 20)
	{
		return false;
	}

	bool bEnableGift = false;
	CX2ItemManager::CashItem* pCashItem = g_pData->GetItemManager()->GetCashItem( itemID );

	if(pCashItem == NULL || pCashItem->m_vecKBillProductInfo.size() < 1)
	{
		return false;
	}

	for(std::vector<KBillProductInfo>::iterator vit = pCashItem->m_vecKBillProductInfo.begin(); vit != pCashItem->m_vecKBillProductInfo.end(); ++vit)
	{
		bEnableGift = (bEnableGift || vit->m_bEnableGift);
	}

	return bEnableGift;
}

void CX2CashShop::CashShopSlotManager::SetShowPresentButton()
{
	CX2CashShop::CashShopSlot* pCashShopSlot = NULL;

	for ( UINT i = 0; i < m_vecCashShopSlot.size(); i++ )
	{
		pCashShopSlot = m_vecCashShopSlot[i];

		if ( pCashShopSlot == NULL )
		{
			continue;
		}

		int nItemID = pCashShopSlot->GetItemID();

		if(IsPossiblePresent(nItemID) == false)
		{
			pCashShopSlot->SetShowPresentButton( false );
		}
		else
		{
			pCashShopSlot->SetShowPresentButton( true );
		}

		pCashShopSlot = NULL;
	}
}


//{{ 2013.03.13 ��ȿ��	ĳ������ ��ư�� ������ ���������� �����

std::wstring CX2CashShop::GetWstrConnectChargePage()
{
	std::wstring wstrURL = L"";

#if !X2_DEV_TOOLS
	switch(g_pMain->GetNationFlag())
	{
	// ����
	case CX2Main::NF_DE:	wstrURL = L"http://de.elsword.gameforge.com/purchase/ingame/";	break;
	case CX2Main::NF_FR:	wstrURL = L"http://fr.elsword.gameforge.com/purchase/ingame/";	break;
	case CX2Main::NF_IT:	wstrURL = L"http://it.elsword.gameforge.com/purchase/ingame/";	break;
	case CX2Main::NF_PL:	wstrURL = L"http://pl.elsword.gameforge.com/purchase/ingame/";	break;
	case CX2Main::NF_ES:	wstrURL = L"http://es.elsword.gameforge.com/purchase/ingame/";	break;
	case CX2Main::NF_EN:	wstrURL = L"http://en.elsword.gameforge.com/purchase/ingame/";	break;
	// �̱�
	case CX2Main::NF_US:
		{
			if( true == g_pInstanceData->IsAeriaChanneling() )
			{
				wstrURL = L"https://api.elswordonline.com/aeria/auth/billing?SID=";
				wstrURL += g_pInstanceData->GetUserSessionID();
			}
			else
			{
				wstrURL = L"https://billing.elswordonline.com/InGame/Main.aspx?userno=";
				wstrURL += g_pInstanceData->GetUserID();
				wstrURL += L"&gamecode=ES&region=";
				if( g_pInstanceData->GetServerGroupID() == 0 ) // �Ϲ�
					wstrURL += L"NA&hkey=";
				else// ����
					wstrURL += L"LA&hkey=";
				// HKEY
				std::string strHkey;
				strHkey = KncUtil::toNarrowString(g_pInstanceData->GetUserID());// + ID
				strHkey += "ES";// + GameCode
				if( g_pInstanceData->GetServerGroupID() == 0 ) // �Ϲ�
					strHkey += "NA";// + region
				else
					strHkey += "LA";// + region
				strHkey += "EA1628437390C1507E315FF9F2AE0909";
				std::string strHkeyMD5 = KncUtil::GetMD5(strHkey);
				// ���� URL
				wstrURL += KncUtil::toWideString(strHkeyMD5);
			}
		}
		break;
	// �븸
	case CX2Main::NF_TW:	wstrURL = L"https://tw.event.beanfun.com/ELSWORD/BuyPoint/intro_main.aspx";	break;
	// ȫ��
	case CX2Main::NF_HK:	wstrURL = L"http://hk.beanfun.com/Default.aspx";	break;
	// �����
	case CX2Main::NF_BR:	wstrURL = L"https://loja.levelupgames.com.br/Multivendas/frmVitrine.aspx?cdProvider=4&cdGame=52";	break;
	//�����ƽþ�
	case CX2Main::NF_PH:	wstrURL = L"http://shop.elsword.garena.ph/";	break;

	// �±�
	case CX2Main::NF_TH:	wstrURL = L"https://secure3.playpark.com/refill/refillplaypark/";	break;
	// �Ϻ�

	default:
		wstrURL = L"http://elsword.com/";
		break;
	}
#else //--- �׼� ---//
	switch(g_pMain->GetNationFlag())
	{
	// ����
	case CX2Main::NF_DE:	wstrURL = L"http://dev.elsword.de/de/purchase/ingame/";	break;
	case CX2Main::NF_FR:	wstrURL = L"http://dev.elsword.de/fr/purchase/ingame/";	break;
	case CX2Main::NF_IT:	wstrURL = L"http://dev.elsword.de/it/purchase/ingame/";	break;
	case CX2Main::NF_PL:	wstrURL = L"http://dev.elsword.de/pl/purchase/ingame/";	break;
	case CX2Main::NF_ES:	wstrURL = L"http://dev.elsword.de/es/purchase/ingame/";	break;
	case CX2Main::NF_EN:	wstrURL = L"http://dev.elsword.co.uk/purchase/ingame/";	break;
	// �̱�
	case CX2Main::NF_US:
		{
			if( true == g_pInstanceData->IsAeriaChanneling() )
			{
				wstrURL = L"http://apitest.elswordonline.com/aeria/auth/billing?SID=";
				wstrURL += g_pInstanceData->GetUserSessionID();
			}
			else
			{
				wstrURL = L"http://billingtest.elswordonline.com:84/InGame/Main.aspx?userno=";
				wstrURL += g_pInstanceData->GetUserID();
				wstrURL += L"&gamecode=ES&region=";
				if( g_pInstanceData->GetServerGroupID() == 0 ) // �Ϲ�
					wstrURL += L"NA&hkey=";
				else// ����
					wstrURL += L"LA&hkey=";
				// HKEY
				std::string strHkey;
				strHkey = KncUtil::toNarrowString(g_pInstanceData->GetUserID());// + ID
				strHkey += "ES";// + GameCode
				if( g_pInstanceData->GetServerGroupID() == 0 ) // �Ϲ�
					strHkey += "NA";// + region
				else
					strHkey += "LA";// + region
				strHkey += "EA1628437390C1507E315FF9F2AE0909";
				std::string strHkeyMD5 = KncUtil::GetMD5(strHkey);
				// ���� URL
				wstrURL += KncUtil::toWideString(strHkeyMD5);
			}
		}
		break;
	// �븸
	case CX2Main::NF_TW:	wstrURL = L"https://ssl.gameastor.com.tw/services/els/intro_main.aspx";	break;
	// ȫ��
	case CX2Main::NF_HK:	wstrURL = L"http://hk.beanfun.com/Default.aspx";	break;
	// �����
	case CX2Main::NF_BR:	wstrURL = L"https://loja.levelupgames.com.br/Multivendas/frmVitrine.aspx?cdProvider=4&cdGame=52";	break;
	//�����ƽþ�
	case CX2Main::NF_PH:	wstrURL = L"http://shoptest.elsword.garena.ph";	break;

		// �±�
	case CX2Main::NF_TH:	wstrURL = L"https://secure3.playpark.com/refill/refillplaypark/";	break;
	// �Ϻ�
	default:
		wstrURL = L"http://elsword.com/";
		break;
	}
#endif

	// UserID �� PurchaseToken ������ ����
	switch(g_pMain->GetNationFlag())
	{
	case CX2Main::NF_DE:
	case CX2Main::NF_FR:
	case CX2Main::NF_IT:
	case CX2Main::NF_PL:
	case CX2Main::NF_ES:
	case CX2Main::NF_EN:
		wstrURL += g_pData->GetMyUser()->GetUserData().userID;
		wstrURL += L"/";
		wstrURL += g_pInstanceData->GetPurchaseToken();

		break;
	default:
		break;
	}

	return wstrURL;
}

//}}

bool CX2CashShop::GetCahBuySharingBankItem( int /*itemID*/ )
{
	if( g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( SHARING_BANK_OPEN_CASH_ITEM_ID, true ) != NULL )
	{
		return false;
	}
	// ���� ���� ������ üũ����.
	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		g_pData->GetMyUser()->IsSharingBankOpen() == true )
	{
		return false;
	}

	return true;
}

void CX2CashShop::JobChangeDirectLink( int iEventID )
{
	// �Һ������� ����
	SetCashShopState( CX2CashShop::CSS_SUB, CX2CashShop::CSC_CONSUME );
	CKTDGUIRadioButton* pRadioButton = NULL;
#ifdef UIRRL_CASH_SHOP
	if( false == m_bCreateDialog )
	{
		break;
	}
#endif //UIRRL_CASH_SHOP
	pRadioButton = static_cast< CKTDGUIRadioButton* >( m_pDLGFront->GetControl( L"tab_Use" ) );

	if ( pRadioButton != NULL )
	{
		pRadioButton->SetChecked( true );
	}

	int itemID = -1;
	switch( iEventID )
	{
	case CX2LVUpEventMgr::LUEMCUM_ELSWORD_SWORD_KNIGHT:
		itemID = CLASS_CHANGE_ELSWORD_KNIGHT_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_ELSWORD_MAGIC_KNIGHT:
		itemID = CLASS_CHANGE_MAGIC_KNIGHT_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_AISHA_HIGH_MAGICIAN:
		itemID = CLASS_CHANGE_HIGH_MAGICIAN_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_AISHA_DARK_MAGICIAN:
		itemID = CLASS_CHANGE_DARK_MAGICIAN_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_RENA_SNIPING_RANGER:
		itemID = CLASS_CHANGE_SNIPING_RANGER_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_RENA_COMBAT_RANGER:
		itemID = CLASS_CHANGE_COMBAT_RANGER_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_RAVEN_SWORD_TAKER:
		itemID = CLASS_CHANGE_SOUL_TAKER_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_RAVEN_OVER_TAKER:
		itemID = CLASS_CHANGE_OVER_TAKER_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_EVE_CODE_EXOTIC:
		itemID = CLASS_CHANGE_EXOTIC_GEAR_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_EVE_CODE_ARCHITECTURE:
		itemID = CLASS_CHANGE_ARCHITECTURE_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_CHUNG_FURY_GUARDIAN:
		itemID = CLASS_CHANGE_FURY_GUARDIAN_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_CHUNG_SHOOTING_GUARDIAN:
		itemID = CLASS_CHANGE_SHOOTER_GUARDIAN_ID;
		break;
		//	1�� �߰� ����
	case CX2LVUpEventMgr::LUEMCUM_ELSWORD_SHEATH_KNIGHT:
		itemID = CLASS_CHANGE_SHEATH_KNIGHT_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_ARME_BATTLE_MAGICIAN:
		itemID = CLASS_CHANGE_BATTLE_MAGICIAN_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_LIRE_TRAPPING_RANGER:
		itemID = CLASS_CHANGE_TRAPPING_RANGER_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_RAVEN_WEAPON_TAKER:
		itemID = CLASS_CHANGE_WEAPON_TAKER_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_EVE_ELECTRA:
		itemID = CLASS_CHANGE_ELECTRA_ITEM_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_CHUNG_SHELLING_GUARDIAN:
		itemID = CLASS_CHANGE_SHELLING_GUARDIAN_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_ARA_LITTLE_HSIEN:
		itemID = CLASS_CHANGE_LITTLE_HSIEN_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_ARA_LITTLE_DEVIL:
		itemID = CLASS_CHANGE_LITTLE_DEVIL_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_ELESIS_SABER_KNIGHT:
		itemID = CLASS_CHANGE_SABER_KNIGHT_ID;
		break;
	case CX2LVUpEventMgr::LUEMCUM_ELESIS_PYRO_KNIGHT:
		itemID = CLASS_CHANGE_PYRO_KNIGHT_ID;
		break;
	default:
		return;
	}

	vector< int > vecItemID;
	if ( itemID != 0 )
		vecItemID.push_back( itemID );
	m_pBuyItemPopUp->Reset( vecItemID );
	m_pBuyItemPopUp->SetOpen( true );
	m_pBuyItemPopUp->SetChoicedItemBuy( false );


}




//=======================================================================================//
//--------------------------------- ES_GLOBAL_CODE_AREA ---------------------------------//
//=======================================================================================//

/** @function : CheckBuyPossibleRelationItem
	@brief : �ش� �������� ��ȥ �������̶�� Ŀ�� ������ ���� �� �� �ֵ��� üũ
	@return : ���� �Ұ����� ��� false ��ȯ
*/
bool CX2CashShop::CheckBuyPossibleRelationItem( int iItemID )
{
	if ( NULL != g_pData->GetRelationshipManager() &&
		 NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
	{
		SEnum::RELATIONSHIP_TYPE eRelationType = g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType;
		if( eRelationType == SEnum::RT_SOLO ) // �ַ� �϶��� üũ�Ѵ�.
		{
			for( int i = 0 ; i < std::ssize(ITEM_ID_WEDDING_CASH_ITEM); ++i )
			{
				// �ش� �������� Wedding Cash Item �̶��
				// false�� ��ȯ�ϰ� MessageBox �� ����.
				if( iItemID == ITEM_ID_WEDDING_CASH_ITEM[i] )
				{
					g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_24674 ),g_pMain->GetNowState() );
					return false;
				}
			}
		}
	}
	return true;



}
/** @function : CheckBuyPossibleRelationItem
	@brief : �ش� �������� ��ȥ �������̶�� Ŀ�� ������ ���� �� �� �ֵ��� üũ
	@return : ���� �Ұ����� ��� false ��ȯ
*/
bool CX2CashShop::CheckBuyPossibleRelationItem( const vector<int>& vecItemIDList )
{
	if ( NULL != g_pData->GetRelationshipManager() &&
		NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
	{
		SEnum::RELATIONSHIP_TYPE eRelationType = g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType;
		if( eRelationType != SEnum::RT_SOLO )
			return true;
	}

	for( int iItemID : vecItemIDList )
	{
		if( false == CheckBuyPossibleRelationItem( iItemID ) )
			return false;
	}

	return true;
}


bool CX2CashShop::ShowPopUpGuideByItemID( const int iItemID_ )
{
	if( GetClassChangeItemLevel( iItemID_ ) > 0 ) // ���� ĳ�� �������̸�
	{
		std::wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_4518, "L", GetClassNameByClassChangeItem( iItemID_ ) ) );
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), g_pMain->GetNowState() );

		return true;
	}
	else if( true == g_pData->GetSkillTree()->IsUnsealSkillItemID( iItemID_ ) )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4519 ), g_pMain->GetNowState() );

		return true;
	}
	else if ( GetUnitClassChangeItemLevel( iItemID_ ) > 0 )
	{
		std::wstring wstrMsg;

		wstrMsg = GET_REPLACED_STRING( ( STR_ID_15234, "L", GetClassNameByUnitClassChangeItem( iItemID_ ) ) );

		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), g_pMain->GetNowState() );

		return true;
	}

	return false;
}


// �ش� ������ ���� �������� ���ϱ� ��Ͽ��� ������.
void CX2CashShop::RemoveUnitClassChangeItemInChoice( const CX2Unit::UNIT_CLASS eUnitClass_ )
{
	const int ItemID = GetUnitClassChangeItemID( eUnitClass_ );

	if( 0 < ItemID )
	{
		if ( std::find( m_vecChoicedItemID.begin(), m_vecChoicedItemID.end(), ItemID ) != m_vecChoicedItemID.end() )
		{
			RemoveChoicedItem( ItemID );
		}
	}
}
// ������ �ش��ϴ� ���� ������ ID�� ��ȯ��.
int CX2CashShop::GetUnitClassChangeItemID( const CX2Unit::UNIT_CLASS eUnitClass_ )
{
	int ItemId = -1;

	switch( eUnitClass_ )
	{
	case CX2Unit::UC_ELSWORD_KNIGHT:
		{
			ItemId = UNIT_CLASS_CHANGE_SWORD_KNIGHT_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
		{
			ItemId = UNIT_CLASS_CHANGE_MAGIC_KNIGHT_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
		{
			ItemId = UNIT_CLASS_CHANGE_SHEATH_KNIGHT_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
		{
			ItemId = UNIT_CLASS_CHANGE_HIGH_MAGICIAN_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ARME_DARK_MAGICIAN:
		{
			ItemId = UNIT_CLASS_CHANGE_DARK_MAGICIAN_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
		{
			ItemId = UNIT_CLASS_CHANGE_BATTLE_MAGICIAN_ITEM_ID;
		}
		break;

	case CX2Unit::UC_LIRE_COMBAT_RANGER:
		{
			ItemId = UNIT_CLASS_CHANGE_COMBAT_RANGER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_LIRE_SNIPING_RANGER:
		{
			ItemId = UNIT_CLASS_CHANGE_SNIPING_RANGER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_LIRE_TRAPPING_RANGER:
		{
			ItemId = UNIT_CLASS_CHANGE_TRAPPING_RANGER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_RAVEN_SOUL_TAKER:
		{
			ItemId = UNIT_CLASS_CHANGE_SOUL_TAKER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_RAVEN_OVER_TAKER:
		{
			ItemId = UNIT_CLASS_CHANGE_OVER_TAKER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_RAVEN_WEAPON_TAKER:
		{
			ItemId = UNIT_CLASS_CHANGE_WEAPON_TAKER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_EVE_EXOTIC_GEAR:
		{
			ItemId = UNIT_CLASS_CHANGE_EXOTIC_GEAR_ITEM_ID;
		}
		break;

	case CX2Unit::UC_EVE_ARCHITECTURE:
		{
			ItemId = UNIT_CLASS_CHANGE_ARCHITECTURE_ITEM_ID;
		}
		break;

	case CX2Unit::UC_EVE_ELECTRA:
		{
			ItemId = UNIT_CLASS_CHANGE_ELECTRA_ITEM_ID;
		}
		break;

	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
		{
			ItemId = UNIT_CLASS_CHANGE_FURY_GUARDIAN_ID;
		}
		break;

	case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
		{
			ItemId = UNIT_CLASS_CHANGE_SHOOTING_GUARDIAN_ID;
		}
		break;

	case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
		{
			ItemId = UNIT_CLASS_CHANGE_SHELLING_GUARDIAN_ID;
		}
		break;

	case CX2Unit::UC_ELESIS_SABER_KNIGHT:
		{
			ItemId = UNIT_CLASS_CHANGE_SABER_KNIGHT_ID;
		}
		break;

	case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
		{
			ItemId = UNIT_CLASS_CHANGE_PYRO_KNIGHT_ID;
		}
		break;

	case CX2Unit::UC_ARA_LITTLE_HSIEN:
		{
			ItemId = UNIT_CLASS_CHANGE_LITTLE_HSIEN_ID;
		}
		break;

	case CX2Unit::UC_ARA_LITTLE_DEVIL:
		{
			ItemId = UNIT_CLASS_CHANGE_LITTLE_DEVIL_ID;
		}
		break;

	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		{
			ItemId = UNIT_CLASS_CHANGE_LORD_KNIGHT_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		{
			ItemId = UNIT_CLASS_CHANGE_RUNE_SLAYER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			ItemId = UNIT_CLASS_CHANGE_INFINITY_SWORD_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{
			ItemId = UNIT_CLASS_CHANGE_VOID_PRINCESS_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			ItemId = UNIT_CLASS_CHANGE_ELEMENTAL_MASTER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			ItemId = UNIT_CLASS_CHANGE_DIMENSION_WITCH_ITEM_ID;
		}
		break;

	case CX2Unit::UC_LIRE_WIND_SNEAKER:
		{
			ItemId = UNIT_CLASS_CHANGE_WIND_SNEAKER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_LIRE_GRAND_ARCHER:
		{
			ItemId = UNIT_CLASS_CHANGE_GRAND_ARCHER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_LIRE_NIGHT_WATCHER:
		{
			ItemId = UNIT_CLASS_CHANGE_NIGHT_WATCHER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_RAVEN_BLADE_MASTER:
		{
			ItemId = UNIT_CLASS_CHANGE_BLADE_MASTER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_RAVEN_RECKLESS_FIST:
		{
			ItemId = UNIT_CLASS_CHANGE_RECKLESS_FIST_ITEM_ID;
		}
		break;

	case CX2Unit::UC_RAVEN_VETERAN_COMMANDER:
		{
			ItemId = UNIT_CLASS_CHANGE_VETERAN_COMMANDER_ITEM_ID;
		}
		break;

	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			ItemId = UNIT_CLASS_CHANGE_NEMESIS_ITEM_ID;
		}
		break;

	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			ItemId = UNIT_CLASS_CHANGE_EMPRESS_ITEM_ID;
		}
		break;

	case CX2Unit::UC_EVE_BATTLE_SERAPH:
		{
			ItemId = UNIT_CLASS_CHANGE_BATTLE_SERAPH_ITEM_ID;
		}
		break;

	case CX2Unit::UC_CHUNG_IRON_PALADIN:
		{
			ItemId = UNIT_CLASS_CHANGE_IRON_PALADIN_ID;
		}
		break;

	case CX2Unit::UC_CHUNG_DEADLY_CHASER:
		{
			ItemId = UNIT_CLASS_CHANGE_DEADLY_CHASER_ID;
		}
		break;

	case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
		{
			ItemId = UNIT_CLASS_CHANGE_TACTICAL_TROOPER_ID;
		}
		break;

	case CX2Unit::UC_ARA_SAKRA_DEVANAM:
		{
			ItemId = UNIT_CLASS_CHANGE_SAKRA_DEVANAM_ID;
		}
		break;

	case CX2Unit::UC_ARA_YAMA_RAJA:
		{
			ItemId = UNIT_CLASS_CHANGE_YAMA_RAJA_ID;
		}
		break;

	case CX2Unit::UC_ELESIS_GRAND_MASTER:
		{
			ItemId = UNIT_CLASS_CHANGE_GRAND_MASTER_ID;
		}
		break;

	case CX2Unit::UC_ELESIS_BLAZING_HEART:
		{
			ItemId = UNIT_CLASS_CHANGE_BLAZING_HEART_ID;
		}
		break;

	case CX2Unit::UC_ADD_LUNATIC_PSYKER:
		{
			ItemId = UNIT_CLASS_CHANGE_LUNATIC_PSYKER_ID;
		}
		break;

	default:
		break;
	}

	return ItemId;
}



#ifdef CASH_INVEN_PICKUP_ALL
bool CX2CashShop::PickUpAll()
{
	// �������� �ϳ� ��������, ������ �ϹǷ� �׻� ù��° �������� ���������� ����.
	CX2SlotItem* pItemSlot = NULL;
	pItemSlot = (CX2SlotItem*) GetSlot( CX2SlotManager::CX2Slot::ST_CASH_SHOP_DEPOSIT, 0 );
	if ( pItemSlot == NULL )
	{
		m_bPickUpAll = false;
		return false;
	}

	if ( pItemSlot->GetKBillOrderInfo().m_byteProductKind == 1 )
	{
		map< int, std::vector< int > > mapItemIDSocketOption;
		vector< KBillOrderInfo > vecKBillTOrderInfo;
		KBillPackageInfo kPackageInfo;
		vector< KBillPackageInfo > vecKBillPackageInfo;

		for ( int i = 0; i < (int)m_vecKBillPackageInfo.size(); i++ )
		{
			KBillPackageInfo& kKBillPackageInfo = m_vecKBillPackageInfo[i];
			if ( /*kKBillPackageInfo.m_ulProductNo == pItemSlot->GetKBillOrderInfo().m_kBillProductInfo.m_iProductNo &&*/
				kKBillPackageInfo.m_iPackageNo == pItemSlot->GetKBillOrderInfo().m_iTransNo )
			{
				//int tempItemID = _wtoi( kKBillPackageInfo.m_wstrProductID.c_str() );
				//int tempItemID = kKBillPackageInfo.m_ulProductNo;
				int tempItemID = kKBillPackageInfo.m_iProductID;
				kPackageInfo = kKBillPackageInfo;
				vecKBillPackageInfo.push_back(kKBillPackageInfo);
				//if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( tempItemID ) == true )
				{
					if ( mapItemIDSocketOption.find(tempItemID) == mapItemIDSocketOption.end() )
						mapItemIDSocketOption.insert( std::pair<int, std::vector< int >>(tempItemID, kKBillPackageInfo.m_vecSocketOption));
				}
			}
		}
		vecKBillTOrderInfo.push_back( pItemSlot->GetKBillOrderInfo() );
		//m_pCashItemToInvenPopup->Reset( vecItemID, vecKBillTOrderInfo, m_vecKBillPackageInfo );
		//m_pCashItemToInvenPopup->SetOpen( true );

		if ( vecKBillTOrderInfo.empty() == false )
		{
			KBillOrderInfo& kKBillOrderInfo = vecKBillTOrderInfo[0];
			map< int, int > mapSelectedOptionList;
			map<int, int> mapOptionGroupIdList;		/// <SocketId, GroupdID>
			// ���⼭ ���� ���� ���� ���ָ� �ȴ�.

			for(map<int, std::vector< int >>::iterator mit = mapItemIDSocketOption.begin(); mit != mapItemIDSocketOption.end(); mit++)
			{
				if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( mit->first ) == true )
				{
					std::vector< int > vecSocket = mit->second;
					if( vecSocket[0] == 0 )
					{
						// ���� �ɼǱ׷��� ó������ 0 �̴�?
						MessageBox(NULL, L"���� �ɼ� �׷� ID �� 0 �Դϴ�. ���� üũ �ʿ�", L"Error", MB_OK);
					}
					else
					{
						const CX2Item::ItemTemplet* pItemTemplate = g_pData->GetItemManager()->GetItemTemplet( pItemSlot->GetItemTID() );
						// ù��° ���� �ɼ��� ������ ������
						vector<int> vecSocketIdList;	/// iSocketGroupID �� �ش��ϴ� SocketID ���� ����
						if ( 0 != g_pData->GetSocketItem()->GetSocketIdListForCashAvatar( vecSocket[0], vecSocketIdList, pItemTemplate->GetItemType() ) )
						{
							mapSelectedOptionList.insert( std::make_pair( pItemTemplate->GetItemID(), vecSocketIdList[0]));
							mapOptionGroupIdList.insert( std::make_pair( vecSocketIdList[0], m_TempCashItemToInven.m_vecSocketOption[0]));
						}
						else
						{
							// �׷쿡 �ش��ϴ� ������ ����?
							MessageBox(NULL, L"���� �ɼ� �׷� �� �ش��ϴ� ���� ID�� �����ϴ�. ���� üũ �ʿ�", L"Error", MB_OK);
						}
					}
				}
			}

			if ( vecKBillPackageInfo.empty() == false )
			{
				m_bPickUpAll = true;
				Handler_EGS_GET_PURCHASED_PACKAGE_CASH_ITEM_REP( kKBillOrderInfo, mapSelectedOptionList, mapOptionGroupIdList, vecKBillPackageInfo );
				return false;
			}
			else
			{
				g_pMain->KTDGUIOKMsgBox(D3DXVECTOR2(250,300), GET_STRING( STR_ID_8 ), g_pMain->GetNowState() );
			}
		}
	}
	else
	{
		const CX2Item::ItemTemplet* pItemTemplate = g_pData->GetItemManager()->GetItemTemplet( pItemSlot->GetItemTID() );
		if ( pItemTemplate != NULL )
		{
			m_bUsePShopAgencyItem = false;
			if( pItemTemplate->GetItemID() == 233210 )
			{
				m_bUsePShopAgencyItem = true;
				//m_TempCashItemToInven = pItemSlot->GetKNXBTOrderInfo();
				m_TempCashItemToInven = pItemSlot->GetKBillOrderInfo();
				m_bPickUpAll = true;
				map< int, int > tempMap;
				Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( m_TempCashItemToInven, tempMap, tempMap);
				return false;
			}
			if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( pItemTemplate->GetItemID() ) == true )
			{
				m_TempCashItemToInven = pItemSlot->GetKBillOrderInfo();
				if(m_TempCashItemToInven.m_vecSocketOption[0] == 0 )
				{
					// ���� �ɼǱ׷��� ó������ 0 �̴�?
					MessageBox(NULL, L"���� �ɼ� �׷� ID �� 0 �Դϴ�. ���� üũ �ʿ�", L"Error", MB_OK);
				}
				else
				{
					// ù��° ���� �ɼ��� ������ ������
					vector<int> vecSocketIdList;	/// iSocketGroupID �� �ش��ϴ� SocketID ���� ����
					if ( 0 != g_pData->GetSocketItem()->GetSocketIdListForCashAvatar( m_TempCashItemToInven.m_vecSocketOption[0], vecSocketIdList, pItemTemplate->GetItemType() ) )
					{
						map< int, int > mapSelectedOptionList;
						map< int, int > mapSelectedOptionGroupList;
						mapSelectedOptionList.insert( std::make_pair( pItemTemplate->GetItemID(), vecSocketIdList[0]));
						mapSelectedOptionGroupList.insert( std::make_pair( vecSocketIdList[0], m_TempCashItemToInven.m_vecSocketOption[0]));
						m_bPickUpAll = true;
						Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( m_TempCashItemToInven, mapSelectedOptionList, mapSelectedOptionGroupList );
						return false;
					}
					else
					{
						// �׷쿡 �ش��ϴ� ������ ����?
						MessageBox(NULL, L"���� �ɼ� �׷� �� �ش��ϴ� ���� ID�� �����ϴ�. ���� üũ �ʿ�", L"Error", MB_OK);
					}
					return false;
				}
			}
			else
			{
				m_TempCashItemToInven = pItemSlot->GetKBillOrderInfo();
				//KBillProductInfo* pKBillProductInfo = g_pData->GetItemManager()->GetCashItemProduct( m_TempCashItemToInven.m_iProductNo );
				//if( NULL != pKBillProductInfo )
				{
					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( m_TempCashItemToInven.m_kBillProductInfo.m_iProductID );
					if ( pItemTemplet != NULL )
					{
						m_bPickUpAll = true;
						map< int, int > tempMap;
						Handler_EGS_BILL_GET_PURCHASED_CASH_ITEM_REP( m_TempCashItemToInven, tempMap, tempMap );
						return false;
					}
				}
			}
		}
	}

	m_bPickUpAll = false;

	return false;
}
#endif // CASH_INVEN_PICKUP_ALL




//=======================================================================================//
//--------------------------------- ES_GLOBAL_CODE_AREA ---------------------------------//
//=======================================================================================//
