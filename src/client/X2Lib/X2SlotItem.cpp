#include "stdafx.h"
#include "./X2SlotItem.h"

CX2SlotItem::CX2SlotItem(void)
{
	m_SortType	= CX2Inventory::ST_NONE;
	m_SlotID	= -1;

	m_bFashion	= false;
	m_EquipPos	= CX2Unit::EP_NONE;

	m_SlotDesc	= L"";
	m_Period	= 0;
	m_ItemUID	= -1;
	m_ItemTID	= -1;

	m_bReset = false;
	m_hEffectExchangeReady = INVALID_PARTICLE_SEQUENCE_HANDLE;
}

CX2SlotItem::~CX2SlotItem(void)
{

}




void CX2SlotItem::ResetPos()
{
	if ( m_pDLGSlot != NULL )
	{
		m_pDLGSlot->SetPos( m_Pos + m_pSlotManager->GetPos() );

		/*
		if ( m_SlotType == CX2Slot::ST_INVENTORY )
		{
			m_pDLGSlot->SetPos( D3DXVECTOR2( m_Pos.x+ , m_Pos.y + 5 ) );
		}
		else
		{
			m_pDLGSlot->SetPos( D3DXVECTOR2( m_Pos ) );
		}
		*/
	}

}

void CX2SlotItem::LuaGetValue( KLuaManager& luaManager )
{
	CX2Slot::LuaGetValue( luaManager );

	LUA_GET_VALUE_ENUM( luaManager, "SORT_TYPE", m_SortType, CX2Inventory::SORT_TYPE, CX2Inventory::ST_NONE );

	LUA_GET_VALUE( luaManager, "FASHION", m_bFashion, false );
	LUA_GET_VALUE_ENUM( luaManager, "EQUIP_POS", m_EquipPos, CX2Unit::EQIP_POSITION, CX2Unit::EP_NONE );

    int iIndex;
	LUA_GET_VALUE( luaManager, "SLOT_DESC", iIndex, STR_ID_EMPTY );
    m_SlotDesc = GET_STRING( iIndex );
}

bool CX2SlotItem::CreateItemUI( CX2Item* pItem, int itemQuantity /* = -1 */ )
{
	KTDXPROFILE();


	if ( pItem == NULL ||
        pItem->GetItemTemplet() == NULL
        )
		return false;

	DestroyItemUI();

	D3DXVECTOR2 imageSize = D3DXVECTOR2( 0, 0 );
	imageSize = m_Size;


	CKTDGUIDialogType pDialog = CreateItemUIDialog();
	SetDialog( pDialog );

	CKTDGUIStatic* pStatic = new CKTDGUIStatic();
	pStatic->SetName( L"StaticRoot_ItemSlot" );
	pDialog->AddControl( pStatic );

	// 아이템 이미지
	CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
    const wchar_t* pwszShopImage = pItem->GetItemTemplet()->GetShopImage();
    if ( pwszShopImage[0]	 != L'\0' &&
		g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile( pwszShopImage ) == true )
	{
		pPicture->SetTex( pwszShopImage );
	}
	else
	{
		pPicture->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
	}

	pPicture->SetSize( imageSize );
	pStatic->AddPicture( pPicture );

	CKTDGUIControl::CPictureData* pPicture2 = new CKTDGUIControl::CPictureData();
	// 기본 텍스쳐
    if ( pPicture2 != NULL )
    {
	    pPicture2->SetTex( L"HQ_ImpossibleEqip.tga" );
	    pPicture2->SetSize( imageSize );
	    pPicture2->SetShow( false );
        if ( pPicture2->pPoint != NULL )
	        pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
	    pStatic->AddPicture( pPicture2 );
    }


	if ( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_WEAPON ||
		pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_DEFENCE ||
		pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY )
	{
		if( false == g_pData->GetMyUser()->GetSelectUnit()->EqipAbility( pItem ) &&
			false == pItem->GetItemTemplet()->GetNoEquip() )
		{
			if( true == pItem->IsDisabled() )
			{ // 파괴된 아이템 텍스쳐
				pPicture2->SetTex( L"DLG_UI_Common_Texture07.tga", L"Disabled_Item" );
                if ( pPicture2->pPoint != NULL )
				    pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,1 );
			}
			else if(  false == pItem->GetIsEvaluation() )
			{ // 미감정 아이템 텍스쳐
				pPicture2->SetTex( L"Unvalued_Icon.tga", L"Unvalued_Icon" );
                if ( pPicture2->pPoint != NULL )
				    pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,1 );
			}

			pPicture2->SetShow( true );
		}
	}

	if (
        pItem->GetItemTemplet() != NULL  &&
        pItem->GetItemTemplet()->GetPeriodType() == CX2Item::PT_QUANTITY  )
	{
		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		pStatic->AddString( pString );
		pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
		pString->color = D3DXCOLOR( 1,1,1,1 );
		if ( pItem->GetItemData().m_Quantity >= pItem->GetItemTemplet()->GetQuantity() )
			pString->color = D3DXCOLOR( 1,0,0,1 );
		pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
		WCHAR buff[32] = {0};
		if ( itemQuantity == -1 )
		{
			/// 몬스터 카드 퀵슬롯이 비활성 이거나, 몬스터 카드 퀵슬롯이 활성중이고, 퀵슬롯 장착 아이템이 아닐 때 수량 표시
			if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
			{
				if( false == g_pData->GetUIManager()->GetUIQuickSlot()->GetIsSummonCardSlot() ||
					( true == g_pData->GetUIManager()->GetUIQuickSlot()->GetIsSummonCardSlot() && CX2Inventory::ST_E_QUICK_SLOT != GetSortType() ) )
				{
					StringCchPrintf( buff, 32, L"%d", pItem->GetItemData().m_Quantity );
				}
			}

		}
		else if ( itemQuantity == -2 )
			StringCchPrintf( buff, 32, L"" );
			//wsprintf( buff, L"" );
		else
			StringCchPrintf( buff, 32, L"%d", itemQuantity );
			//wsprintf( buff, L"%d", itemQuantity );
		pString->msg = buff;
		pString->fontStyle = CKTDGFontManager::FS_SHELL;
		pString->sortFlag = DT_RIGHT;
		//수량성 아이템 수량 표시 위치 수정
		pString->pos = D3DXVECTOR2( imageSize.x - 15 - 4, imageSize.y - 15 - 1 );
	}

	//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘
	CreateSealedStatic();

	// 봉인 상태인지 아닌지를 알 수 있으므로
	{
		if ( pItem->GetItemData().m_bIsSealed == true )
			SetShowSealedImage( true );
		else
			SetShowSealedImage( false );
	}
	//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘

	/// 개인 상점에 등록중인 아이템 표시 아이콘 생성
	CreateSellWaitingStatic();

	/// 개인 상점에 아이템 등록 대기중 상태라면, 아이콘에 표시 해주자
	if ( NULL != g_pData &&
		 NULL != g_pData->GetUIManager() &&
		 NULL != g_pData->GetUIManager()->GetUIPersonalShop() )

	{
		const int iPickedShopItemIndex			= g_pData->GetUIManager()->GetUIPersonalShop()->GetPickedShopItemIndex();
		/// 등록 여부
		const bool bPersonalShopSellWaitingItem = g_pData->GetUIManager()->GetUIPersonalShop()->CheckDoubleShopItem( pItem->GetUID(), iPickedShopItemIndex + 1 );

		if ( true == bPersonalShopSellWaitingItem )
			SetShowWaitingSellImage( true );
		else
			SetShowWaitingSellImage( false );
	}
	else
	{
		SetShowWaitingSellImage( false );
	}

	CreateSocketUseStatic();
	SetShowSocketUseImage(pItem->GetUID());
	SetShowExchangeUseImage(
        pItem->GetItemTemplet()->GetItemID()
        );
	SetShowExchangeSelectImage(false);

	{
		m_Period = pItem->GetItemData().m_Period;
	}

	SetItemUID( pItem->GetUID() );
    SetItemTID( pItem->GetItemTemplet()->GetItemID() );

	m_bReset = true;

	return true;
}

bool CX2SlotItem::CreateItemUI( int itemID, int itemQuantity /* = -1 */, bool bUseImpossibleImg /* = false */, bool bQuantityStr /* = false */ )
{
    const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	return CreateItemUI( pItemTemplet, itemQuantity, bUseImpossibleImg, bQuantityStr );
}


bool CX2SlotItem::CreateItemUI(
    const CX2Item::ItemTemplet* pItemTemplet,
    int itemQuantity /* = -1 */, bool bUseImpossibleImg /* = false */, bool bQuantityStr /* = false */  )
{

	KTDXPROFILE();

	if ( pItemTemplet == NULL )
		return false;
	DestroyItemUI();

	D3DXVECTOR2 imageSize = D3DXVECTOR2( 0, 0 );

	imageSize = m_Size;

	CKTDGUIDialogType pDialog = CreateItemUIDialog();
	SetDialog( pDialog );

	CKTDGUIStatic* pStatic = new CKTDGUIStatic();
	pStatic->SetName( L"StaticRoot_ItemSlot" );
	pDialog->AddControl( pStatic );

	// 아이템 이미지
	CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
	if ( g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile(
        pItemTemplet->GetShopImage()
        ) == true )
	{
        pPicture->SetTex( pItemTemplet->GetShopImage() );
	}
	else
	{
		pPicture->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
	}

	pPicture->SetSize( imageSize );



	pStatic->AddPicture( pPicture );

	if ( itemQuantity == -1 || bUseImpossibleImg == true )
	{
		CKTDGUIControl::CPictureData* pPicture2 = new CKTDGUIControl::CPictureData();
		pPicture2->SetTex( L"HQ_ImpossibleEqip.tga" );
		pPicture2->SetSize( imageSize );
		pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
		pStatic->AddPicture( pPicture2 );
		pPicture2->SetShow( false );

        switch(  pItemTemplet->GetItemType() )
        {
        case CX2Item::IT_WEAPON:
        case CX2Item::IT_DEFENCE:
        case CX2Item::IT_ACCESSORY:
		    {
			    //{{ 허상형 : [2009/7/29] //	조건에 pItemTemplet->m_bNoEquip == false 추가. 착용불가 아이템은 밝게 표시하게 하기 위함
			    if ( g_pData->GetMyUser()->GetSelectUnit()->EqipAbility(
                    pItemTemplet->GetItemID()
                    ) == false && pItemTemplet->GetNoEquip() == false )
			    {
				    pPicture2->SetShow( true );
			    }
			    //}} 허상형 : [2009/7/29] //	조건에 pItemTemplet->m_bNoEquip == false 추가. 착용불가 아이템은 밝게 표시하게 하기 위함
		    }
        default:
        	break;
        }//switch

	}

	if ( ( pItemTemplet->GetPeriodType() == CX2Item::PT_QUANTITY &&
		itemQuantity != -1 )  || ( bQuantityStr == true && itemQuantity != -1 )  )
	{
		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		pStatic->AddString( pString );
		pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
		pString->color = D3DXCOLOR( 1,1,1,1 );
		pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
		WCHAR buff[32] = {0};
		StringCchPrintf( buff, 32, L"%d", itemQuantity );

		//wsprintf( buff, L"%d", itemQuantity );
		pString->msg = buff;
		pString->fontStyle = CKTDGFontManager::FS_SHELL;
		pString->sortFlag = DT_RIGHT;
		pString->pos = D3DXVECTOR2( imageSize.x - 15 - 4, imageSize.y - 15 - 4 );
	}

	//** 선물 리본 표시해주는 부분..
	if( m_KBillOrderInfo.m_iTransNo != -1 )
	{
		/*
		//if ( m_KBillOrderInfo. .m_byteIsRead == 0 && m_KBillOrderInfo.m_byteIsPresent == 1 )
		{
			CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
			pPicture->SetTex( "DLG_Common_New_Texture57.tga", "Gift_Small_Slot_Glow" );

			pPicture->SetPoint();
			pPicture->SetPos( D3DXVECTOR2( -4, -4 ) );
			pPicture->SetTwinkle( 0.7f, true );
			pStatic->AddPicture( pPicture );
		}*/

		if ( !m_KBillOrderInfo.m_wstrSenderNickName.empty() )
		{
			CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
			pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Ribbon" );

			pPicture->SetPoint();
			pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
			pStatic->AddPicture( pPicture );
		}
	}



	if ( m_SlotType == CX2Slot::ST_CASH_SHOP_MAIN_NEW ||
		m_SlotType == CX2Slot::ST_CASH_SHOP_MAIN_HOT ||
		m_SlotType == CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND ||
		m_SlotType == CX2Slot::ST_CASH_SHOP_SUB )
	{
		CreateRibbonPicture( pItemTemplet, pStatic );
	}

	//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘
	CreateSealedStatic();
	//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘

	SetItemUID( -1 );
    SetItemTID( pItemTemplet->GetItemID() );

	m_bReset = true;

	return true;
}


bool CX2SlotItem::CreateMaterialItemUI(
    const CX2Item::ItemTemplet* pItemTemplet,
    int itemQuantity, int needItemQuantity )
{
	if ( pItemTemplet == NULL )
		return false;

	DestroyItemUI();

	D3DXVECTOR2 imageSize = D3DXVECTOR2( 0, 0 );

	imageSize = m_Size;

	CKTDGUIDialogType pDialog = CreateItemUIDialog();
	SetDialog( pDialog );

	CKTDGUIStatic* pStatic = new CKTDGUIStatic();
	pStatic->SetName( L"StaticRoot_ItemSlot" );
	pDialog->AddControl( pStatic );

	// 아이템 이미지
	CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
    const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
	if ( g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile( pwszShopImage ) == true )
	{
		pPicture->SetTex( pwszShopImage );
	}
	else
	{
		pPicture->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
	}

	pPicture->SetSize( imageSize );
	pStatic->AddPicture( pPicture );



	if ( itemQuantity < needItemQuantity )
	{
		CKTDGUIControl::CPictureData* pPicture2 = new CKTDGUIControl::CPictureData();
		pPicture2->SetTex( L"HQ_ImpossibleEqip.tga" );
		pPicture2->SetSize( imageSize );
		pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
		pStatic->AddPicture( pPicture2 );
		pPicture2->SetShow( true );
	}

	if ( pItemTemplet->GetPeriodType() == CX2Item::PT_QUANTITY )
	{
		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		pStatic->AddString( pString );
		pString->fontIndex = XUF_DODUM_11_NORMAL;
		pString->color = D3DXCOLOR( 1,1,1,1 );
		pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
		WCHAR buff[32] = {0};
		StringCchPrintf( buff, 32, L"%d/%d", itemQuantity, needItemQuantity );
		//wsprintf( buff, L"%d/%d", itemQuantity, needItemQuantity );
		pString->msg = buff;
		pString->fontStyle = CKTDGFontManager::FS_SHELL;
		pString->sortFlag = DT_RIGHT;
// 09.03.06 김태완 : 재료템 글자 위치 변경
		pString->pos = D3DXVECTOR2( imageSize.x - 15 +2 , imageSize.y - 15 + 2);
	}

	CreateSealedStatic();

	SetItemUID( -1 );
	SetItemTID(
        pItemTemplet->GetItemID()
        );

	m_bReset = true;

	return true;
}


bool CX2SlotItem::IsResetItemUI()
{
	return m_bReset;
}


bool CX2SlotItem::ResetItemUI( CX2Item* pItem )
{
	KTDXPROFILE();



	if ( pItem == NULL
        || pItem->GetItemTemplet() == NULL
        || GetDialog() == NULL )
		return false;


	D3DXVECTOR2 imageSize = D3DXVECTOR2( 0, 0 );
	imageSize = m_Size;

	CKTDGUIStatic* pStaticRoot = (CKTDGUIStatic*)m_pDLGSlot->GetControl( L"StaticRoot_ItemSlot" );
	if ( pStaticRoot != NULL )
	{
		CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 0 );
		if ( pPicture != NULL )
		{
            const wchar_t* pwszShopImage = pItem->GetItemTemplet()->GetShopImage();
			if ( pwszShopImage[0] != L'\0' && g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile( pwszShopImage ) == true )
			{
				pPicture->SetTex( pwszShopImage );
			}
			else
			{
				pPicture->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

			pPicture->SetSize( imageSize );
		}

		CKTDGUIControl::CPictureData* pPicture2 = pStaticRoot->GetPicture( 1 );

		if ( pPicture2 != NULL )
		{
			pPicture2->SetSize( imageSize );
			pPicture2->SetShow( false );

			if ( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_WEAPON ||
				pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_DEFENCE ||
				pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY )
			{

				if ( false == g_pData->GetMyUser()->GetSelectUnit()->EqipAbility( pItem ) &&
					 false == pItem->GetItemTemplet()->GetNoEquip() )
				{
					// 기본 텍스쳐
					pPicture2->SetTex( L"HQ_ImpossibleEqip.tga" );
					pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
					pPicture2->SetShow(true);

					if( true == pItem->IsDisabled() )
					{// 파괴된 아이템 텍스쳐
						pPicture2->SetTex( L"DLG_UI_Common_Texture07.tga", L"Disabled_Item" );
						pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,1 );
					}
					else if(  false == pItem->GetIsEvaluation() )
					{ // 미감정 아이템 텍스쳐
						pPicture2->SetTex( L"Unvalued_Icon.tga", L"Unvalued_Icon" );
						pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,1 );
					}
				}
			}
		}

		if (
            pItem->GetItemTemplet() != NULL  &&
            pItem->GetItemTemplet()->GetPeriodType() == CX2Item::PT_QUANTITY  )
		{
			if ( pStaticRoot->GetString(0) != NULL )
			{
				WCHAR buff[32] = {0};
				StringCchPrintf( buff, 32, L"%d", pItem->GetItemData().m_Quantity );
				//wsprintf( buff, L"%d", pItem->GetItemData().m_Quantity );
				pStaticRoot->GetString(0)->msg = buff;

				pStaticRoot->GetString(0)->color = D3DXCOLOR( 1,1,1,1 );
				pStaticRoot->GetString(0)->outlineColor = D3DXCOLOR( 0,0,0,1 );

				if ( pItem->GetItemData().m_Quantity >= pItem->GetItemTemplet()->GetQuantity() )
					pStaticRoot->GetString(0)->color = D3DXCOLOR( 1,0,0,1 );
			}
			else
			{
				CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
				pStaticRoot->AddString( pString );
				pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
				pString->color = D3DXCOLOR( 1,1,1,1 );
				pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
				WCHAR buff[32] = {0};

				if ( pItem->GetItemData().m_Quantity >= pItem->GetItemTemplet()->GetQuantity() )
					pString->color = D3DXCOLOR( 1,0,0,1 );

				StringCchPrintf( buff, 32, L"%d", pItem->GetItemData().m_Quantity );
				//wsprintf( buff, L"%d", pItem->GetItemData().m_Quantity );
				pString->msg = buff;
				pString->fontStyle = CKTDGFontManager::FS_SHELL;
				pString->sortFlag = DT_RIGHT;
				pString->pos = D3DXVECTOR2( imageSize.x - 15 - 5, imageSize.y - 15 - 5 );
			}
		}
		else
		{
			if ( pStaticRoot->GetString(0) != NULL )
			{
				pStaticRoot->GetString(0)->msg = L"";
			}
		}

		//** 리본 표시해주는 부분
		if( m_KBillOrderInfo.m_iTransNo != -1 )
		{
			/*if ( m_KBillOrderInfo.m_kBillProductInfo.r m_byteIsRead == 0 && m_KBillOrderInfo.m_kBillProductInfo.m_bEnableGift == 1 )
			{

				CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
				if ( pPicture != NULL )
				{
					pPicture->SetShow( true );
					pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Glow" );

					pPicture->SetPoint();
					pPicture->SetPos( D3DXVECTOR2( -4, -4 ) );
					pPicture->SetTwinkle( 0.7f, true );
				}
				else
				{
					CKTDGUIControl::CPictureData* pPictureGlow = new CKTDGUIControl::CPictureData();
					pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Glow" );

					pPicture->SetPoint();
					pPicture->SetPos( D3DXVECTOR2( -4, -4 ) );
					pPicture->SetTwinkle( 0.7f, true );
					pStaticRoot->AddPicture( pPictureGlow );
				}
			}
			else
			{
				CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
				if ( pPicture != NULL )
				{
					pPicture->SetShow( false );
				}
			}
			*/

			if ( !m_KBillOrderInfo.m_wstrSenderNickName.empty() )
			{
				CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
				if ( pPicture != NULL )
				{
					pPicture->SetShow( true );
					pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Ribbon" );
					pPicture->SetPoint();
					pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
				}
				else
				{
					CKTDGUIControl::CPictureData* pPictureRibbon = new CKTDGUIControl::CPictureData();
					pPictureRibbon->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Ribbon" );
					pPictureRibbon->SetPoint();
					pPictureRibbon->SetPos( D3DXVECTOR2( -1, -1 ) );
					pStaticRoot->AddPicture( pPictureRibbon );
				}
			}
			else
			{
				CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
				if ( pPicture != NULL )
				{
					pPicture->SetShow( false );
				}
			}
		}

	}

	//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘
	//CreateSealedStatic();

	{
		if ( pItem->GetItemData().m_bIsSealed == true )
		{
			SetShowSealedImage( true );
		}
		else
		{
			SetShowSealedImage( false );
		}
	}
	//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘

	/// 개인 상점에 아이템 등록 대기중 상태라면, 아이콘에 표시 해주자
	if ( NULL != g_pData &&
		 NULL != g_pData->GetUIManager() &&
		 NULL != g_pData->GetUIManager()->GetUIPersonalShop() )

	{
		const int iPickedShopItemIndex = g_pData->GetUIManager()->GetUIPersonalShop()->GetPickedShopItemIndex();
		/// 등록 여부
		const bool bPersonalShopSellWaitingItem = g_pData->GetUIManager()->GetUIPersonalShop()->CheckDoubleShopItem( pItem->GetUID(), iPickedShopItemIndex + 1 );

		if ( true == bPersonalShopSellWaitingItem )
			SetShowWaitingSellImage( true );
		else
			SetShowWaitingSellImage( false );
	}
	else
	{
		SetShowWaitingSellImage( false );
	}

	CreateSocketUseStatic();
	SetShowSocketUseImage(pItem->GetUID());
	SetShowExchangeUseImage(
        pItem->GetItemTemplet()->GetItemID()
        );
	SetShowExchangeSelectImage(false);

	{
		m_Period = pItem->GetItemData().m_Period;
	}

	SetItemUID( pItem->GetUID() );
	SetItemTID(
        pItem->GetItemTemplet()->GetItemID()
        );

	m_pDLGSlot->SetShow( true );
	m_pDLGSlot->SetEnable( true );

	m_bReset = true;

	return true;
}

bool CX2SlotItem::ResetItemUI( int itemID, int itemQuantity, bool bUseImpossibleImg /* = false */, bool bQuantityStr /* = false */ )
{
    const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );

	if (
        pItemTemplet == NULL ||
        GetDialog() == NULL )
		return false;

	D3DXVECTOR2 imageSize = D3DXVECTOR2( 0, 0 );

	imageSize = m_Size;


	// 아이템 이미지

	CKTDGUIStatic* pStaticRoot = (CKTDGUIStatic*)GetDialog()->GetControl( L"StaticRoot_ItemSlot" );

	if ( pStaticRoot == NULL )
		return false;

	CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture(0);

	if ( pPicture != NULL )
	{
        const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
		if ( pwszShopImage[0] != L'\0' && g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile( pwszShopImage ) == true )
		{
			pPicture->SetTex( pwszShopImage );
		}
		else
		{
			pPicture->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
		}

		pPicture->SetSize( imageSize );
	}


	if ( itemQuantity == -1 || bUseImpossibleImg == true )
	{

		CKTDGUIControl::CPictureData* pPicture2 = pStaticRoot->GetPicture(1);

		if ( pPicture2 != NULL )
		{
			pPicture2->SetTex( L"HQ_ImpossibleEqip.tga" );
			pPicture2->SetSize( imageSize );
			pPicture2->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
			pPicture2->SetShow( false );

            CX2Item::ITEM_TYPE itemType = pItemTemplet->GetItemType();
			if ( itemType == CX2Item::IT_WEAPON ||
				itemType == CX2Item::IT_DEFENCE ||
				itemType == CX2Item::IT_ACCESSORY )
			{
				//{{ 허상형 : [2009/7/29] //	조건에 pItemTemplet->m_bNoEquip == false 추가. 착용불가 아이템은 밝게 표시하게 하기 위함
				if ( g_pData->GetMyUser()->GetSelectUnit()->EqipAbility(
                    pItemTemplet->GetItemID()
                    ) == false && pItemTemplet->GetNoEquip() == false )
				{
					pPicture2->SetShow( true );
				}
				//}} 허상형 : [2009/7/29] //	조건에 pItemTemplet->m_bNoEquip == false 추가. 착용불가 아이템은 밝게 표시하게 하기 위함
			}
		}
	}

	if ( ( pItemTemplet->GetPeriodType() == CX2Item::PT_QUANTITY &&
		itemQuantity != -1 ) || ( bQuantityStr == true && itemQuantity != -1 ) )
	{

		if ( pStaticRoot->GetString(0) != NULL )
		{
			CKTDGUIControl::UIStringData* pString = pStaticRoot->GetString(0);
			pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
			pString->color = D3DXCOLOR( 1,1,1,1 );
			pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
			WCHAR buff[32] = {0};
			StringCchPrintf( buff, 32, L"%d", itemQuantity );
			//wsprintf( buff, L"%d", itemQuantity );
			pString->msg = buff;
			pString->fontStyle = CKTDGFontManager::FS_SHELL;
			pString->sortFlag = DT_RIGHT;
			pString->pos = D3DXVECTOR2( imageSize.x - 15 - 4, imageSize.y - 15 - 4 );
		}
		else
		{
			CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
			pStaticRoot->AddString( pString );
			pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
			pString->color = D3DXCOLOR( 1,1,1,1 );
			pString->outlineColor = D3DXCOLOR( 0,0,0,1 );
			WCHAR buff[32] = {0};
			StringCchPrintf( buff, 32, L"%d", itemQuantity );
			/*wsprintf( buff, L"%d", itemQuantity );*/
			pString->msg = buff;
			pString->fontStyle = CKTDGFontManager::FS_SHELL;
			pString->sortFlag = DT_RIGHT;
			pString->pos = D3DXVECTOR2( imageSize.x - 15 - 4, imageSize.y - 15 - 4 );
		}
	}
	else
	{
		if ( pStaticRoot->GetString(0) != NULL )
		{
			pStaticRoot->GetString(0)->msg = L"";
		}
	}


	//** 리본 표시하는 부분
	if( m_KBillOrderInfo.m_iTransNo != -1 )
	{
		/*
		if ( m_KNXBTOrderInfo.m_byteIsRead == 0 && m_KNXBTOrderInfo.m_byteIsPresent == 1 )
		{

			CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
			if ( pPicture != NULL )
			{
				pPicture->SetShow( true );
				pPicture->SetTex( "DLG_Common_New_Texture57.tga", "Gift_Small_Slot_Glow" );

				pPicture->SetPoint();
				pPicture->SetPos( D3DXVECTOR2( -4, -4 ) );
				pPicture->SetTwinkle( 0.7f, true );
			}
			else
			{
				CKTDGUIControl::CPictureData* pPictureGlow = new CKTDGUIControl::CPictureData();
				pPicture->SetTex( "DLG_Common_New_Texture57.tga", "Gift_Small_Slot_Glow" );

				pPicture->SetPoint();
				pPicture->SetPos( D3DXVECTOR2( -4, -4 ) );
				pPicture->SetTwinkle( 0.7f, true );
				pStaticRoot->AddPicture( pPictureGlow );
			}
		}
		else
		{
			CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
			if ( pPicture != NULL )
			{
				pPicture->SetShow( false );
			}
		}
		*/

		if ( !m_KBillOrderInfo.m_wstrSenderNickName.empty() )
		{
			CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 3 );
			if ( pPicture != NULL )
			{
				pPicture->SetShow( true );
				pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Ribbon" );

				pPicture->SetPoint();
				pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
			}
			else
			{
				CKTDGUIControl::CPictureData* pPictureRibbon = new CKTDGUIControl::CPictureData();
				pPictureRibbon->SetTex( L"DLG_Common_New_Texture57.tga", L"Gift_Small_Slot_Ribbon" );


				pStaticRoot->AddPicture( pPictureRibbon );
				pPictureRibbon->SetPoint();
				pPictureRibbon->SetPos( D3DXVECTOR2( -1, -1 ) );
			}
		}
		else
		{
			CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
			if ( pPicture != NULL )
			{
				pPicture->SetShow( false );
			}
		}
	}

	if ( m_SlotType == CX2Slot::ST_CASH_SHOP_MAIN_NEW ||
		m_SlotType == CX2Slot::ST_CASH_SHOP_MAIN_HOT ||
		m_SlotType == CX2Slot::ST_CASH_SHOP_MAIN_RECOMMEND ||
		m_SlotType == CX2Slot::ST_CASH_SHOP_SUB )
	{
		CKTDGUIControl::CPictureData* pPicture = pStaticRoot->GetPicture( 2 );
		if ( pPicture != NULL )
		{
			pPicture->SetShow( false );
            DWORD   dwItemID = pItemTemplet->GetItemID();
			if( true == g_pData->GetItemManager()->CheckLimitedCashItem( dwItemID ) )
			{
				pPicture->SetShow( true );
				pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Limited_Slot_Ribbon" );

				pPicture->SetPoint();
				pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
			}
			else if( true == g_pData->GetItemManager()->CheckEventCashItem( dwItemID ) )
			{
				pPicture->SetShow( true );
				pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Event_Ribbon" );

				pPicture->SetPoint();
				pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
			}
			else if( true == g_pData->GetItemManager()->CheckHotIconCashItem( dwItemID ) )
			{
				pPicture->SetShow( true );
				pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Hot_Ribbon" );

				pPicture->SetPoint();
				pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
			}
		}
		else
		{
			CreateRibbonPicture( pItemTemplet, pStaticRoot );
		}
	}

	//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘
	//CreateSealedStatic();
	//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘

	m_Period = 0;


	SetItemUID( -1 );
    SetItemTID( pItemTemplet->GetItemID() );

	if ( m_pDLGSlot != NULL )
	{
		m_pDLGSlot->SetShow( true );
		m_pDLGSlot->SetEnable( true );
	}


	m_bReset = true;

	return true;
}

bool CX2SlotItem::LostItemUI()
{
	m_bReset = false;

	if ( m_pDLGSlot != NULL )
	{
		m_pDLGSlot->SetShow( false );
		m_pDLGSlot->SetEnable( false );

		CKTDGUIStatic* pStaticRoot = (CKTDGUIStatic*)m_pDLGSlot->GetControl( L"StaticRoot_ItemSlot" );
		if ( pStaticRoot != NULL )
		{
			if ( pStaticRoot->GetString(0) != NULL )
			{
				pStaticRoot->GetString(0)->msg = L"";
			}

			if ( pStaticRoot->GetPicture(0) != NULL )
			{
				pStaticRoot->GetPicture(0)->SetTex( L"HQ_Invisible.tga" );
			}

			if ( pStaticRoot->GetPicture(1) != NULL )
			{
				pStaticRoot->GetPicture(1)->SetShow( false );
			}
		}

		//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘
		CKTDGUIStatic*	pStaticSealed	= NULL;
		pStaticSealed					= static_cast< CKTDGUIStatic* >( m_pDLGSlot->GetControl( L"static_SealedItemSlot" ) );

		if ( pStaticSealed != NULL )
		{
			pStaticSealed->SetShow( false );
		}
		//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘
	}

	m_Period = 0;
	ExchangeReadyEffectEnd();

	SetItemUID( -1 );
	SetItemTID( -1 );

	return true;
}

void CX2SlotItem::DestroyItemUI()
{
	CKTDGUIDialogType pkDiag = GetDialog();
	SAFE_DELETE_DIALOG(pkDiag);
	SetDialog( NULL );

	m_Period = 0;
	ExchangeReadyEffectEnd();

	SetItemUID( -1 );
	SetItemTID( -1 );


	m_bReset = false;
}

void CX2SlotItem::SetKBillOrderInfo( KBillOrderInfo& kKBillOrderInfo )
{
	m_KBillOrderInfo = kKBillOrderInfo;
}



CKTDGUIDialogType CX2SlotItem::CreateItemUIDialog()
{
	KTDXPROFILE();


	CKTDGUIDialogType pDialog = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_SlotManager.lua", 0.0f, m_pSlotManager->GetSlotManagerLayer() );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );
	if ( m_pDLGSlotFixBack != NULL )
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGSlotFixBack, pDialog, false );

	return pDialog;
}




// 캐시샵에서 "한정판매", "이벤트" 등의 리본이미지 생성
void CX2SlotItem::CreateRibbonPicture(
    const CX2Item::ItemTemplet* pItemTemplet,
    CKTDGUIStatic* pStatic )
{
	if( NULL == pItemTemplet )
		return;
    DWORD   dwItemID = pItemTemplet->GetItemID();

	if( NULL == pStatic )
		return;


	if( true == g_pData->GetItemManager()->CheckLimitedCashItem( dwItemID ) )
	{
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Limited_Slot_Ribbon" );

		pPicture->SetPoint();
		pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
		pStatic->AddPicture( pPicture );
	}
	else if( true == g_pData->GetItemManager()->CheckEventCashItem( dwItemID ) )
	{
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Event_Ribbon" );

		pPicture->SetPoint();
		pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
		pStatic->AddPicture( pPicture );
	}
	else if( true == g_pData->GetItemManager()->CheckHotIconCashItem( dwItemID ) )
	{
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"Hot_Ribbon" );

		pPicture->SetPoint();
		pPicture->SetPos( D3DXVECTOR2( -1, -1 ) );
		pStatic->AddPicture( pPicture );
	}
}

void CX2SlotItem::SetShowImpossiblePicture(bool bShow)
{
	if( NULL != GetDialog() )
	{
		CKTDGUIStatic* pStaticRoot = static_cast<CKTDGUIStatic*>(GetDialog()->GetControl( L"StaticRoot_ItemSlot"));
		if( NULL != pStaticRoot && NULL != pStaticRoot->GetPicture(1) )
		{
			pStaticRoot->GetPicture(1)->SetShow(bShow);
		}
	}
}

	//{{ kimhc // 2009-08-26 // 아이템 봉인 아이콘

void CX2SlotItem::CreateSealedStatic()				// 봉인아이콘Picture를 가진 static 생성
{

	if ( m_pDLGSlot == NULL )
	{
		ASSERT( !"m_pDLGSlot is NULL" );
		return;
	}

	CKTDGUIStatic* pStatic	= NULL;

	//if ( m_pDLGSlot->GetControl( L"static_SealedItemSlot" ) == NULL )
	//{
		pStatic = new CKTDGUIStatic();
		pStatic->SetName( L"static_SealedItemSlot" );
		m_pDLGSlot->AddControl( pStatic );

		// 이미지
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		pPicture->SetTex( L"DLG_Icon_Sealed_Item.tga", L"Sealed_Item" );
		//pPicture->SetTex( L"DLG_Common_New_Texture57.tga", L"GIFT_SMALL_SLOT_GLOW" );
		pPicture->SetSize( m_Size );
		pStatic->AddPicture( pPicture );
	/*}
	else
	{
		pStatic = static_cast< CKTDGUIStatic* >( m_pDLGSlot->GetControl( L"static_SealedItemSlot" ) );
		if ( pStatic->GetPicture( 0 ) != NULL )
		{
			pStatic->GetPicture( 0 )->SetTex( L"DLG_Icon_Sealed_Item.tga", L"Sealed_Item" );
			pStatic->GetPicture( 0 )->SetSize( m_Size );
		}
	}*/

	pStatic->SetShow( false );

}

void CX2SlotItem::SetShowSealedImage( bool bShow )	// 봉인아이콘을 보일것인가 말것인가?
{
	CKTDGUIStatic* pStatic	= NULL;

	pStatic	= static_cast< CKTDGUIStatic* > ( GetDialog()->GetControl( L"static_SealedItemSlot" ) );

	if ( pStatic == NULL )
	{
		ASSERT( !"static_SealedItemSlot is NULL" );
		return;
	}

	pStatic->SetShow( bShow );
}

	//}} kimhc // 2009-08-26 // 아이템 봉인 아이콘



//{{ oasis907 : 김상윤 [2010.5.11] // 소켓 강화 사용중인 아이템 표시
void CX2SlotItem::CreateSocketUseStatic()
{

	if ( m_pDLGSlot == NULL )
	{
		ASSERT( !"m_pDLGSlot is NULL" );
		return;
	}

	CKTDGUIStatic* pStatic = new CKTDGUIStatic();
	pStatic->SetName( L"static_SocketUseItemSlot" );
	m_pDLGSlot->AddControl( pStatic );

	// 이미지
	CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
	pPicture->SetTex( L"DLG_UI_Common_Texture13.tga", L"Check" );
	pPicture->SetSize( m_Size );
	pStatic->AddPicture( pPicture );


	pStatic->SetShow( false );

}

void CX2SlotItem::SetShowSocketUseImage( UidType Uid )
{
	CKTDGUIStatic* pStatic	= static_cast< CKTDGUIStatic* > ( GetDialog()->GetControl( L"static_SocketUseItemSlot" ) );

	if ( pStatic == NULL )
	{
		ASSERT( !"static_SocketUseItemSlot is NULL" );
		return;
	}

	if(g_pData->GetUIManager()->GetUIInventory() != NULL &&
		g_pData->GetUIManager()->GetUIInventory()->GetUISocketItem() != NULL &&
		g_pData->GetUIManager()->GetUIInventory()->GetUISocketItem()->GetShow() == true &&
		g_pData->GetUIManager()->GetUIInventory()->GetUISocketItem()->CheckSocketUseItem( Uid ) )
	{

		pStatic->SetShow( true );
	}
	else
	{
		pStatic->SetShow( false );

	}
}
//}} oasis907 : 김상윤 [2010.5.11] //

void CX2SlotItem::SetShowExchangeSelectImage( bool bShow )
{
	CKTDGUIStatic* pStatic	= static_cast< CKTDGUIStatic* > ( GetDialog()->GetControl( L"static_SocketUseItemSlot" ) );
	if ( pStatic == NULL )
	{
		ASSERT( !"static_SocketUseItemSlot is NULL" );
		return;
	}
	if(bShow)
	{
		pStatic->SetShow( true );
	}
	else
	{
		pStatic->SetShow( false );
	}
}

void CX2SlotItem::SetShowExchangeUseImage( int itemID )
{
	ExchangeReadyEffectEnd();

	if(g_pData->GetUIManager()->GetUIItemExchangeShop() != NULL &&
		g_pData->GetUIManager()->GetUIItemExchangeShop()->GetShow() == true &&
		g_pData->GetUIManager()->GetUIItemExchangeShop()->CheckExchangeUseItem( itemID ) )
	{
		ExchangeReadyEffectStart();
	}
}
void CX2SlotItem::ExchangeReadyEffectStart()
{
	D3DXVECTOR3 vecEffectPos;

	vecEffectPos.x = m_Pos.x +  m_Size.x / 2.0f;
	vecEffectPos.y = m_Pos.y +  m_Size.y / 2.0f;

	m_hEffectExchangeReady = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL, L"UI_Exchange_StartEffect03", vecEffectPos.x, vecEffectPos.y, 0 );
	CKTDGParticleSystem::CParticleEventSequence* pParticle = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hEffectExchangeReady );
	if( pParticle != NULL )
	{
		pParticle->SetOverUI( true );
		pParticle->SetResolutionConvert(true);
	}
	return;
}
void CX2SlotItem::ExchangeReadyEffectEnd()
{
	if( m_hEffectExchangeReady != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pParticle = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hEffectExchangeReady );
		if( pParticle != NULL )
		{
			pParticle->SetShowObject(false);
		}
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hEffectExchangeReady );
	}
	return;
}


/** @function	: CreateSellWaitingStatic
	@brief		: 개인 상점에 등록중인 아이템 표시 아이콘 생성
*/
void CX2SlotItem::CreateSellWaitingStatic()				// 봉인아이콘Picture를 가진 static 생성
{

	if ( m_pDLGSlot == NULL )
	{
		ASSERT( !"m_pDLGSlot is NULL" );
		return;
	}

	CKTDGUIStatic* pStatic	= NULL;
	pStatic = new CKTDGUIStatic();

	if ( NULL != pStatic )
	{
		pStatic->SetName( L"static_SellWaitingItemSlot" );
		m_pDLGSlot->AddControl( pStatic );

		// 이미지
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();

		if ( NULL != pPicture )
		{
			pPicture->SetTex( L"DLG_Icon_Regist_Item.tga", L"Regist_Item" );
			pPicture->SetSize( m_Size );
			pStatic->AddPicture( pPicture );

			pStatic->SetShow( false );
		}
	}

}

/** @function	: SetShowWaitingSellImage
	@brief		: 개인 상점에 등록중인 아이템 표시 아이콘 활성 여부
	@param		: 활성 여부
*/
void CX2SlotItem::SetShowWaitingSellImage( IN bool bShow_ )	// 봉인아이콘을 보일것인가 말것인가?
{
	CKTDGUIStatic* pStatic	= NULL;

	pStatic	= static_cast< CKTDGUIStatic* > ( GetDialog()->GetControl( L"static_SellWaitingItemSlot" ) );

	if ( pStatic == NULL )
	{
		ASSERT( !"static_SealedItemSlot is NULL" );
		return;
	}

	pStatic->SetShow( bShow_ );
}
