#include "stdafx.h"

//{{ kimhc // 2009-08-04 // ĳ���ͺ� ����

#include "./X2UIPrivateBank.h"

CX2UIPrivateBank::CX2UIPrivateBank( const CKTDXStage* const pNowStage, const WCHAR* const pFileName )
	: CX2UIDragable(  const_cast< CKTDXStage* >( pNowStage ), NULL )
	, m_pDlgPrivateBank( NULL )
	, m_bShow( false )
	, m_vDlgSize( 0, 0 )
	, m_iMaxNumOfSlot( 0 )
	, m_iUsedNumOfSlot( 0 )
	, m_pDLGBuyConfirm( NULL )
{
	m_bReserveResetDialog = false;
	//	�� ĳ���ͷ� �ʱ�ȭ
	wstring wstrMyName = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_NickName;
	SetShareNickName(wstrMyName);
	SetIsShareBank(false);
	SetShareBankSize(CX2Inventory::MPB_NORMAL);

	m_pDlgWantBuySharingBank = NULL;
	m_ShareUnitUID = 0;

	m_pDLGWaitingMsgBox = NULL;

	m_wstrDlgFileName = pFileName;

	InitUIPrivateBank( pNowStage, pFileName );
}

CX2UIPrivateBank::~CX2UIPrivateBank()
{
	DestoryPrivateBank();
}

bool		CX2UIPrivateBank::InitUIPrivateBank( const CKTDXStage* const pNowStage, const WCHAR* const pFileName )
{
	const CX2Inventory* pInventory	= NULL;
	pInventory	= GetMyInventory();

	if ( pInventory == NULL )
		return false;					// ASSERT ó���� ���� GetInventory���� ����

	RegisterLuaBind();

	//m_eMembershipGrade	= static_cast< CX2Inventory::MEMBERSHIP_PRIVATE_BANK >( pInventory->GetItemMaxNum( CX2Inventory::ST_BANK ) );

	// SetDummyValue ���� �ľƷ��� ���̾�α� ������ lua���� dummyValue ����
	//SetDummyValue( static_cast < int >( m_eMembershipGrade ) );
	if( GetIsShareBank() == true )
	{
		SetDummyValue( GetShareBankSize() );
	}
	else
	{
		SetDummyValue( static_cast<int>( pInventory->GetBankMembershipGrade() ) );
	}


	m_pDlgPrivateBank		= new CKTDGUIDialog( const_cast< CKTDXStage* >( pNowStage ), pFileName );

	if ( m_pDlgPrivateBank == NULL )
	{
		ASSERT( !"DlgPrivateBank is NULL" );
		return false;
	}

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDlgPrivateBank );
	m_pDlgPrivateBank->SetDisableUnderWindow( true );

	//	���� ����Ʈ �ҷ�����
	if(m_pDlgPrivateBank != NULL && g_pData != NULL && g_pData->GetMyUser() != NULL)
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();

		CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDlgPrivateBank->GetControl(L"ComboUserList");
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDlgPrivateBank->GetControl(L"Static_Select_User");
		if(pComboBox != NULL && pStatic != NULL )
		{
			// ���� : ���� ���� �վ��� ���� Ȱ��ȭ ��Ű��
			if(g_pData->GetMyUser()->IsSharingBankOpen() == true
				&& pUnit->IsTradeBlockUnit() == false
			)
			{
				pComboBox->RemoveAllItems();

				//	��ĳ���� �������
				wstring wstrMyName = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_NickName;
				pComboBox->AddItem(wstrMyName.c_str(), NULL);

				//	������ ���
				for(int i=0; i<g_pData->GetMyUser()->GetUnitNum(); ++i)
				{
					if(wstrMyName != g_pX2Game->GetUserName(i))
						pComboBox->AddItem(g_pX2Game->GetUserName(i).c_str(), NULL);
				}

				CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDlgPrivateBank->GetControl(L"Static_Select_User");
				if (  pStatic->GetString(0) != NULL )
				{
					pStatic->GetString(0)->msg = m_wstrShareNickName.c_str();
				}
			}
			else
			{
				pComboBox->SetEnable(false);
				pStatic->SetEnable(false);
				// ���� ��ư�� ��Ȱ��ȭ ��Ű������
				//CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDlgPrivateBank->GetControl(L"Button_User_Select");
				//pButton->SetEnable(false);
			}
		}
	}

	return true;
}

void		CX2UIPrivateBank::DestoryPrivateBank()
{
	SAFE_DELETE_DIALOG( m_pDLGBuyConfirm );
	SAFE_DELETE_DIALOG( m_pDlgPrivateBank );
	SAFE_DELETE_DIALOG( m_pDlgWantBuySharingBank );
}

HRESULT		CX2UIPrivateBank::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	if( m_bReserveResetDialog == true )
	{
		ResetPrivateBank();

		m_bReserveResetDialog = false;
	}

	if ( m_pDlgPrivateBank != NULL && m_pDlgPrivateBank->GetIsMouseOver() == true )
	{
		return CX2ItemSlotManager::OnFrameMove( fTime, fElapsedTime );
	}
	else
	{
		CX2ItemSlotManager::InvalidSelectedItem();
		CX2ItemSlotManager::InvalidSlotDesc();
	}

	return	S_OK;
}

bool		CX2UIPrivateBank::MsgProc( HWND /*hWnd*/, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam )
{
	// �տ� ��� ���̾�αװ� ������ �޽����� ó������ �ʴ´�
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDlgPrivateBank ) )
		return false;

	bool bFlag = false;

	switch( uMsg )
	{
		// ���콺 �޽����鿡 ����
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		//case WM_MOUSEWHEEL:
		{
			if ( false == m_pDlgPrivateBank->GetIsMouseOver() )
			{
				return false;
			}
			bFlag = true;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			if ( false == m_pDlgPrivateBank->GetIsMouseOver() )
			{
				return false;
			}
			g_pData->GetUIManager()->SetUILayerIndex( CX2UIManager::UI_MENU_PRIVATE_BANK, true );
			bFlag = true;
		}
		break;
	default:
		break;
	}

	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			MouseUp( mousePos );	// ������ �ؾ���

		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			if ( MouseDown( mousePos) == true )	// ������ �ؾ���
				return true;

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

	case WM_RBUTTONDOWN:
		{
			SetNowDragItemReturn();
		}
		break;

	case WM_RBUTTONUP:
		{
			SetNowDragItemReturn();

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );

			MouseRButtonUp( mousePos );

		}
		break;
	}

	return bFlag;

}

bool		CX2UIPrivateBank::UICustomEventProc( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case UPBCM_EXIT:
		{
			SetShow( false );

			if ( g_pTFieldGame != NULL )
			{
				CX2TFieldNpc *pJoinNpc = g_pTFieldGame->GetFieldNPC( g_pTFieldGame->GetJoinNpcIndex() );
				if ( pJoinNpc != NULL )
				{
					if ( pJoinNpc->GetNpcShop() != NULL )
						pJoinNpc->GetNpcShop()->SetKeyEvent();
				}
			}

			return true;
		}
		break;

	case UPBCM_CASHSHOP:
		{
			// CX2StateMenu�� UICustomEventProc�� SMUCM_CASH_SHOP�� ���� ����
			// �ش��ڵ� ����� ���� �ڵ嵵 ���� �����ؾ��ϴ��� ���� �ʿ�
			CX2StateField*	pStateField	=	NULL;
			pStateField		=  static_cast< CX2StateField* >( g_pMain->GetNowState() );

			if ( pStateField != NULL )
			{
				if ( g_pTFieldGame != NULL )
				{
					g_pTFieldGame->CloseFieldName( 0.0f );
					g_pTFieldGame->CloseBuffNameForce();
				}

				g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_MENU_PRIVATE_BANK );
				pStateField->ToggleCashShop();

			}

			return true;
		}
		break;

	case UPBCM_USER_CHANGE:
		{
			if( m_pDlgPrivateBank == NULL )
			{
				return true;
			}

			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)lParam;
			if ( pComboBox != NULL && pComboBox->GetNumItems() > 0)
			{
				CKTDGUIComboBox::ComboBoxItem *pItem = pComboBox->GetItem(pComboBox->GetSelectedItemIndex());
				if(pItem != NULL)
				{
					CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDlgPrivateBank->GetControl(L"Static_Select_User");
					if ( pStatic != NULL && pStatic->GetString(0) != NULL )
					{
						pStatic->GetString(0)->msg = pItem->strText;
					}
				}
			}
			return true;
		}
		break;

	case UPBCM_USER_SELECT:
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
			if( pUnit->IsTradeBlockUnit() == false )
			{

			if(g_pData->GetMyUser()->IsSharingBankOpen() == true)
			{
				wstring wstrMyName = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_NickName;
				wstring wstrSelectName;

				CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDlgPrivateBank->GetControl(L"Static_Select_User");
				if ( pStatic != NULL && pStatic->GetString(0) != NULL )
				{
					wstrSelectName = pStatic->GetString(0)->msg;
				}

				//	�ٲ��� ���� ��� �н�
				if( wstrSelectName == GetShareNickName() )
				{
					return true;
				}

				if( wstrMyName == wstrSelectName )	//	����?
				{
					//	���� ���̱�
					SetShareNickName(wstrSelectName);
					SetIsShareBank(false);

					//	���⼭ �ٷ� Dialog���� �� ��� MsgProc���� ���� ����ų �� �����Ƿ� OnFrameMove���� Reset�ϵ��� ����
					m_bReserveResetDialog = true;

					//g_pData->GetServerProtocol()->SendID( EGS_CLOSE_BANK_NOT );	//	������ �ٲﳻ�� �����ϱ�
				}
				else	//	�ƴϸ�?
				{
					//	��Ŷ ������
					KEGS_GET_SHARE_BANK_REQ kPacket;
					kPacket.m_wstrNickName = wstrSelectName;

					g_pData->GetServerProtocol()->SendPacket( EGS_GET_SHARE_BANK_REQ, kPacket );
					g_pMain->AddServerPacket( EGS_GET_SHARE_BANK_ACK );

					if(m_pDLGWaitingMsgBox == NULL)
					{
						//m_pDLGWaitingMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_815 ), g_pMain->GetNowState() );
						//�輮��. [13.07.16] ������� �������� ����. ���� �κ��� �������� �ʴ�.
					}
				}
			}
			else
			{				// ���� : ���� ���� �ȶվ��ٸ�, ���� �޽����� �������
				m_pDlgWantBuySharingBank = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_14840 ), UPBCM_WANT_BUY_SHARING_CASH_ITEM, g_pMain->GetNowState() );

			}

			}

			return true;
		}
		break;
	case UPBCM_WANT_BUY_SHARING_CASH_ITEM:
		{
			if ( m_pDlgWantBuySharingBank != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDlgWantBuySharingBank, 0, false );

			m_pDlgWantBuySharingBank = NULL;
			// ���� : ĳ�� ������ �����ش�.( �������� ���� ���� ������ �ִ� ������ )
			CX2StateField*	pStateField	=	NULL;
			pStateField		=  static_cast< CX2StateField* >( g_pMain->GetNowState() );

			if ( pStateField != NULL )
			{
				if ( g_pTFieldGame != NULL )
				{
					g_pTFieldGame->CloseFieldName( 0.0f );
					g_pTFieldGame->CloseBuffNameForce();
				}
				// ĳ���� ��ġ��
				g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_MENU_PRIVATE_BANK );
				pStateField->ToggleCashShop();

			}
			return true;
		}
		break;
	case UPBCM_CHANGE_INVENTORY_SLOT_ITEM_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), 0, false );
			}

			g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, g_pData->GetUIManager()->GetUIInventory()->m_kTempChangeSlotInfo );
			g_pMain->AddServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK );

			return true;
		}
		break;
	}

	return false;
}

bool		CX2UIPrivateBank::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case EGS_GET_MY_BANK_INFO_ACK:
		{
			return Handler_EGS_GET_MY_BANK_INFO_ACK( hWnd, uMsg, wParam, lParam );

		} break;
	case EGS_GET_SHARE_BANK_ACK:
		{
			return Handler_EGS_GET_SHARE_BANK_ACK( hWnd, uMsg, wParam, lParam );

		} break;
	case EGS_SHARE_BANK_UPDATE_OK_NOT:
		{
			return Handler_EGS_SHARE_BANK_UPDATE_OK_NOT();
		}
		break;
	}

	return false;
}


void CX2UIPrivateBank::SetShow( bool bShow )
{
	m_bShow	= bShow;

	if ( bShow )
	{
		g_pData->GetUIManager()->UIOpened( CX2UIManager::UI_MENU_PRIVATE_BANK );
		UpdateUIStrMembership();
		UpdateUI();
	}
	else
	{
		g_pData->GetUIManager()->UIClosed( CX2UIManager::UI_MENU_PRIVATE_BANK );
		LostItemSlotList();
		CX2ItemSlotManager::InvalidSelectedItem();
		CX2ItemSlotManager::InvalidSlotDesc();
	}

	m_pDlgPrivateBank->SetShowEnable( bShow, bShow );
}

void CX2UIPrivateBank::SetLayer( X2_DIALOG_LAYER layer )
{
	SetSlotManagerLayer( layer );
	SetSlotManagerChangeSequence( false );
	if( m_pDlgPrivateBank != NULL )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDlgPrivateBank, layer );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDlgPrivateBank, false );
	}

}

void CX2UIPrivateBank::SetPosition( D3DXVECTOR2 /*vec*/, bool /*bAbsolute*/ /* = true */ )
{

}

const CX2Inventory*	CX2UIPrivateBank::GetMyInventory() const
{
	if ( g_pData->GetMyUser() == NULL )
	{
		ASSERT( !"User Is NULL");
		return NULL;
	}

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
	{
		ASSERT( !"SelectUnit Is NULL");
		return NULL;
	}

	return &g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
}

CX2Inventory*	CX2UIPrivateBank::AccessMyInventory()
{
	if ( g_pData->GetMyUser() == NULL )
	{
		ASSERT( !"User Is NULL");
		return NULL;
	}

	if ( g_pData->GetMyUser()->GetSelectUnit() == NULL )
	{
		ASSERT( !"SelectUnit Is NULL");
		return NULL;
	}

	return &g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();
}

std::wstring	CX2UIPrivateBank::GetStrMembership( CX2Inventory::MEMBERSHIP_PRIVATE_BANK membershipGrade ) const
{
	std::wstring	wstrMembership;

	switch ( membershipGrade )
	{
	case CX2Inventory::MPB_NORMAL:
		wstrMembership = GET_STRING( STR_ID_3848 );
		break;

	case CX2Inventory::MPB_SILVER:
		wstrMembership = GET_STRING( STR_ID_3849 );
		break;

	case CX2Inventory::MPB_GOLD:
		wstrMembership = GET_STRING( STR_ID_3850 );
		break;

	case CX2Inventory::MPB_EMERALD:
		wstrMembership = GET_STRING( STR_ID_3851 );
		break;

	case CX2Inventory::MPB_DIAMOND:
		wstrMembership = GET_STRING( STR_ID_3852 );
		break;

	case CX2Inventory::MPB_PLATINUM:
		wstrMembership = GET_STRING( STR_ID_3853 );
		break;

	default:
		ASSERT( !"Wrong path" );
		wstrMembership = L"";
		break;
	}


	return	wstrMembership;
}

UINT	CX2UIPrivateBank::GetGradeSize( IN CX2Inventory::MEMBERSHIP_PRIVATE_BANK membershipGrade_ ) const
{
	switch ( membershipGrade_ )
	{
	case CX2Inventory::MPB_NORMAL:
		return 8;
		break;

	case CX2Inventory::MPB_SILVER:
		return 16;
		break;

	case CX2Inventory::MPB_GOLD:
		return 24;
		break;

	case CX2Inventory::MPB_EMERALD:
		return 32;
		break;

	case CX2Inventory::MPB_DIAMOND:
		return 40;
		break;

	case CX2Inventory::MPB_PLATINUM:
		return 48;
		break;

	default:
		ASSERT( !"Wrong Grade" );
		return 0;
		break;
	}
}

void	CX2UIPrivateBank::UpdateUICashShopButton()
{
	if ( m_pDlgPrivateBank != NULL )
	{
		CKTDGUIButton*	pCashShopButton		= NULL;
		CKTDGUIStatic*	pMembershipIcon		= NULL;

		pCashShopButton		=	reinterpret_cast< CKTDGUIButton* >( m_pDlgPrivateBank->GetControl( L"g_pButton_Upgrade" ) );

		if ( pCashShopButton == NULL )
		{
			ASSERT( !"Cashshop Button error" );
			return;
		}


		pMembershipIcon		=	reinterpret_cast< CKTDGUIStatic* >( m_pDlgPrivateBank->GetControl( L"g_pMembershipIcon" ) );

		if ( pMembershipIcon == NULL )
		{
			ASSERT( !"MembershipIcon error" );
			return;
		}

		CX2Inventory::MEMBERSHIP_PRIVATE_BANK	membershipGrade = CX2Inventory::MPB_NORMAL;

		for ( int i = 0; i < pMembershipIcon->GetPictureNum(); i++ )
		{
			pMembershipIcon->GetPicture( i )->SetShow( false );
		}

		//	���� ���� �����϶��� Ȯ�� UI ǥ�� �����ʴ´�
		if( GetIsShareBank() == true )
		{
			pCashShopButton->SetGuideDesc( L"" );
			pCashShopButton->SetShowEnable( false, false );

			return;
		}

        const CX2Inventory* pMyInventory = GetMyInventory();
        if ( pMyInventory != NULL )
        {
		    switch ( pMyInventory->GetBankMembershipGrade() )
		    {
		    case CX2Inventory::MPB_NORMAL:
			    {
				    pMembershipIcon->GetPicture( 0 )->SetShow( true );
				    membershipGrade	= CX2Inventory::MPB_SILVER;
			    }
			    break;

		    case CX2Inventory::MPB_SILVER:
			    {
				    pMembershipIcon->GetPicture( 1 )->SetShow( true );
				    membershipGrade	= CX2Inventory::MPB_GOLD;
			    }
			    break;

		    case CX2Inventory::MPB_GOLD:
			    {
				    pMembershipIcon->GetPicture( 2 )->SetShow( true );
				    membershipGrade	= CX2Inventory::MPB_EMERALD;
			    }
			    break;

		    case CX2Inventory::MPB_EMERALD:
			    {
				    pMembershipIcon->GetPicture( 3 )->SetShow( true );
				    membershipGrade	= CX2Inventory::MPB_DIAMOND;
			    }
			    break;

		    case CX2Inventory::MPB_DIAMOND:
			    {
				    if ( pMyInventory->GetItemByTID( _CONST_UI_PRIVATE_BANK_::g_iBankQuestItemID, true ) != NULL )
				    {
					    pMembershipIcon->SetShow( false );
					    pCashShopButton->SetShowEnable( false, false );
				    }
				    else
				    {
					    pMembershipIcon->GetPicture( 4 )->SetShow( true );
					    membershipGrade	= CX2Inventory::MPB_PLATINUM;
				    }
			    }
			    break;

		    default:
		    	break;
		    }

		    switch ( pMyInventory->GetBankMembershipGrade() )
		    {
		    case CX2Inventory::MPB_NORMAL:
		    case CX2Inventory::MPB_SILVER:
		    case CX2Inventory::MPB_GOLD:
		    case CX2Inventory::MPB_EMERALD:
		    case CX2Inventory::MPB_DIAMOND:
			    {
				    // ���� ����� Ÿ������ ���� MPB_NORMAL�� ����
					pCashShopButton->SetGuideDesc( GET_REPLACED_STRING( ( STR_ID_28663, "L", GetStrMembership( membershipGrade ) ) ) );
					pCashShopButton->SetGuideDescOffsetPos( D3DXVECTOR2( 100, 0 ) );
			    }
			    break;

		    case CX2Inventory::MPB_PLATINUM:
			    {
					pCashShopButton->SetGuideDesc( L"" );
					pCashShopButton->SetShowEnable( false, false );
			    }
			    break;

		    default:
			    ASSERT( !"Wrong path UpdateUICashShopButton" );
			    break;
		    }
        }
        else
        {
            ASSERT( !"NULL Inventory" );
        }
	}
}

void	CX2UIPrivateBank::UpdateUIStrMembership()
{
	if ( m_pDlgPrivateBank != NULL )
	{
		std::wstring	wstrNickName;

		if (	g_pData->GetMyUser() != NULL
			&&	g_pData->GetMyUser()->GetSelectUnit() != NULL )
			wstrNickName	=	g_pData->GetMyUser()->GetSelectUnit()->GetNickName();

		CKTDGUIStatic* pStatic	=	NULL;

		pStatic = static_cast< CKTDGUIStatic* >( m_pDlgPrivateBank->GetControl( L"Static_Notice" ) );
		if( GetIsShareBank() == true )
		{
			pStatic->SetString( 0, GET_REPLACED_STRING( ( STR_ID_3865, "LL", GetShareNickName(), GetStrMembership( static_cast<CX2Inventory::MEMBERSHIP_PRIVATE_BANK>(GetShareBankSize()) ) ) ) );
		}
		else if ( GetMyInventory() != NULL )
        {
		    pStatic->SetString( 0, GET_REPLACED_STRING( ( STR_ID_3865, "LL", wstrNickName, GetStrMembership( GetMyInventory()->GetBankMembershipGrade() ) ) ) );
        }
	}
}

void	CX2UIPrivateBank::UpdateUIUsedAndMaxNumOfSlot()
{
	if ( m_pDlgPrivateBank != NULL )
	{
		const CX2Inventory*	pInventory	= NULL;
		CKTDGUIStatic*	pStatic		= NULL;

		pInventory	= GetMyInventory();
		pStatic		= static_cast< CKTDGUIStatic* >( m_pDlgPrivateBank->GetControl( L"Staticnumber" ) );

		if ( pInventory != NULL )
		{
			if( GetIsShareBank() == true )
			{
				m_iUsedNumOfSlot	= pInventory->GetUsedShareBankNum();
				m_iMaxNumOfSlot		= GetShareBankSize();
			}
			else
			{
				m_iUsedNumOfSlot	= pInventory->GetUsedSlotNum( CX2Inventory::ST_BANK );
				m_iMaxNumOfSlot		= pInventory->GetItemMaxNum( CX2Inventory::ST_BANK );
			}
		}

		if ( pStatic != NULL )
		{
			wstringstream wstrstm;
			wstrstm << m_iUsedNumOfSlot
				<< L"/" << m_iMaxNumOfSlot;
			pStatic->SetString( 0, wstrstm.str().c_str() );
		}
	}
}

void	CX2UIPrivateBank::UpdateSlotList()
{
	CX2Item*		pItem			=	NULL;
	CX2SlotItem*	pSlotItem		=	NULL;
	const CX2Inventory*	pInventory		=	NULL;

	pInventory		=	GetMyInventory();

	if ( pInventory == NULL )
	{
		ASSERT( !"Inventory Is NULL " );
		return;
	}

	if( GetIsShareBank() == true )
	{
		for ( UINT slotID = 0; slotID < m_SlotList.size(); slotID++ )
		{
			pSlotItem	=	static_cast< CX2SlotItem * >( GetSlot( slotID ) );
			if ( pSlotItem == NULL )
				continue;

			pItem	= pInventory->GetItem( CX2Inventory::ST_SHARE_BANK, slotID );

			if ( pItem != NULL )
			{
				if ( pSlotItem->IsCreatedItemUI() == true )
					pSlotItem->ResetItemUI( pItem );
				else
					pSlotItem->CreateItemUI( pItem );
			}
			else
			{
				pSlotItem->DestroyItemUI();
			}

			pSlotItem->SetEnable( true );
			pSlotItem->SetShow( true );
			pSlotItem->SetSortType( CX2Inventory::ST_SHARE_BANK );
			//pSlotItem->SetSlotType( CX2SlotManager::ST_SHARE_BANK );

			pItem		= NULL;
			pSlotItem	= NULL;
		}

		return;
	}

	for ( UINT slotID = 0; slotID < m_SlotList.size(); slotID++ )
	{
		pSlotItem	=	static_cast< CX2SlotItem * >( GetSlot( CX2Slot::ST_BANK, slotID ) );
		if ( pSlotItem == NULL )
			continue;

		pItem	= pInventory->GetItem( CX2Inventory::ST_BANK, slotID );

		if ( pItem != NULL )
		{
			if ( pSlotItem->IsCreatedItemUI() == true )
				pSlotItem->ResetItemUI( pItem );
			else
				pSlotItem->CreateItemUI( pItem );
		}
		else
		{
			pSlotItem->DestroyItemUI();
		}

		pSlotItem->SetEnable( true );
		pSlotItem->SetShow( true );

		pItem		= NULL;
		pSlotItem	= NULL;
	}
}

void	CX2UIPrivateBank::UpdateUI()
{
	UpdateUICashShopButton();
	UpdateUIUsedAndMaxNumOfSlot();
	UpdateSlotList();
}

void	CX2UIPrivateBank::LostItemSlotList( bool bShow /* = false */, bool bEnable /* = false */ )
{
	for ( UINT i = 0; i < m_SlotList.size(); i++ )
	{
		CX2SlotItem* pSlotItem = static_cast< CX2SlotItem* >( m_SlotList[i] );
		if ( pSlotItem != NULL )
		{
			pSlotItem->LostItemUI();
			pSlotItem->SetShow( bShow );
			pSlotItem->SetEnable( bEnable );
		}
	}
}

bool	CX2UIPrivateBank::MouseDown( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pSlotItem	= NULL;
	pSlotItem				= static_cast< CX2SlotItem* >( GetSlotInMousePos( mousePos ) );

	if ( pSlotItem == NULL )	// ���콺�� �������� �ִ��� �˻�
		return false;

	if ( pSlotItem->IsResetItemUI() == false )
	{
		return false;
	}

	if ( pSlotItem->GetSlotType() != CX2SlotItem::ST_BANK )
		return false;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( pSlotItem->GetItemTID() );
	if ( pItemTemplet == NULL )
		return false;

	CX2State* pState = static_cast< CX2State* >( g_pMain->GetNowState() );
	if( pState != NULL )
	{
		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_NORMAL )
		{
			return true;
		}
	}

	*m_pSlotBeforeDragging = pSlotItem;
	g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), GetDraggingItemLayer() );
	(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,0.7f) );

	*m_DraggingItemUID = pSlotItem->GetItemUID();

	if ( g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_INVEN ) == false )
	{
		g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_INVEN, true );
	}

	if ( GetMyInventory() != NULL
        && g_pData->GetUIManager()->GetUIInventory()->GetSortType() != GetMyInventory()->GetSortTypeByItemTemplet( pItemTemplet ) )
	{
		g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTab( GetMyInventory()->GetSortTypeByItemTemplet( pItemTemplet ) );
	}


	g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );

	return true;
}

bool	CX2UIPrivateBank::MouseUp( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pSlot	= NULL;
	pSlot				= static_cast< CX2SlotItem* >( GetSlotInMousePos( mousePos ) );

	if ( pSlot == NULL )
		return false;

	if ( *m_pSlotBeforeDragging	!= NULL )	// �巡�� �� ��� �϶�
	{
		bool			bCheck		= false;

		bCheck			=	OnDropAnyItem( mousePos );	// ������ �ؾ���

		if ( *m_pSlotBeforeDragging	!= NULL && (*m_pSlotBeforeDragging)->GetDialog() != NULL )
		{
			g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer(
				(*m_pSlotBeforeDragging)->GetDialog(), (*m_pSlotBeforeDragging)->GetNormalLayer() );
			(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR( 1, 1, 1, 1 ) );
		}

		if ( bCheck )
		{
			*m_pSlotBeforeDragging	= NULL;
			*m_DraggingItemUID		= 1;
			return true;
		}
	}
	else									// �巡�� �� ����� �ƴҶ�
	{
		const CX2Inventory*	pInventory	= NULL;
		CX2Item*		pItem		= NULL;

		pInventory		= GetMyInventory();

		if ( pInventory == NULL )
			return false;

		pItem			= pInventory->GetItem( pSlot->GetItemUID(), true );

		if ( pItem == NULL )
			return false;

		CX2State*	pState	=	NULL;
		pState				=	static_cast< CX2State* >( g_pMain->GetNowState() );

		if ( pState	!= NULL )
		{
			CX2Cursor*	pCursor	= NULL;
			pCursor				=	pState->GetCursor();

			if ( pCursor !=	NULL &&	pCursor->GetCurorState() !=	CX2Cursor::XCS_NORMAL )
			{

				switch ( pCursor->GetCurorState() )
				{
				case CX2Cursor::XCS_SOCKET:			// ����
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_3862 ), g_pMain->GetNowState() );
					}
					break;

				case CX2Cursor::XCS_DELETE_ITEM:	// ������
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_3866 ), g_pMain->GetNowState() );
					}
					break;

				case CX2Cursor::XCS_DECOMPOSITION:	// ����
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_3861 ), g_pMain->GetNowState() );
					}
					break;

				default:
					break;
				}
			}
		}
	}

	return false;
}

void CX2UIPrivateBank::DrawSlotMouseOverImage()
{
	if ( GetCheckOperationCondition() == false )
	{
		m_pDLGSelectedItem->SetShow( false );
		return;
	}

	bool bCheck = false;

	if ( m_pNowOverItemSlot != NULL )
	{
		m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
		m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

		bCheck = true;
	}

	if ( bCheck == true )
	{
		m_pDLGSelectedItem->SetShow( true );
	}
	else
	{
		m_pDLGSelectedItem->SetShow( false );
	}

}

bool CX2UIPrivateBank::OnDropAnyItem( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pSlotItem = NULL;
	pSlotItem	= static_cast< CX2SlotItem* >( GetSlotInMousePos( mousePos ) );

	if ( pSlotItem == NULL || pSlotItem->GetEnable() == false )
		return false;

	if ( pSlotItem == *m_pSlotBeforeDragging )
	{
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );
		SetNowDragItemReturn();
		return true;
	}

	if ( (*m_pSlotBeforeDragging)->IsItem() == false )
		return false;

	const CX2Inventory* pInventory = GetMyInventory();
	if ( pInventory == NULL )
		return false;

	switch ( (*m_pSlotBeforeDragging)->GetSlotType() )
	{
	case CX2Slot::ST_INVENTORY:
	case CX2Slot::ST_BANK:
	case CX2Slot::ST_EQUIPPED:
		{
			const CX2Item::ItemTemplet*	pFromItemTemplet		= NULL;
			const CX2Item::ItemTemplet*	pToItemTemplet			= NULL;
			pFromItemTemplet	= g_pData->GetItemManager()->GetItemTemplet( pSlotItem->GetItemTID() );
			pToItemTemplet		= g_pData->GetItemManager()->GetItemTemplet( static_cast< CX2SlotItem* >( (*m_pSlotBeforeDragging) )->GetItemTID() );

			// pFromItemTemplet�� NULL �� �� ����(�󽽷��ΰ��)
			if ( pToItemTemplet == NULL )
			{
				ASSERT( !"X2UIPrivateBank OnDropAnyItem" );
				return false;
			}


			//{{ kimhc // 2010-01-05 // PC�� �����̾� ����
			if ( pToItemTemplet->GetIsPcBang() == true )
			{
				g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
				SetNowDragItemReturn();
				return true;
			}
			//}} kimhc // 2010-01-05 // PC�� �����̾� ����

			// ���κ��丮�� �����κ��丮, ����� ���� �κ��丮 ��, ���� �̵� ����
			if ( (*m_pSlotBeforeDragging)->GetSlotType() == CX2Slot::ST_INVENTORY &&
				pToItemTemplet->GetItemID() == UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID )
			{
				// ���� ������ ������ ���� �κ��丮�� �ƴ� ���
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29779 ), g_pMain->GetNowState() ); //18422
				SetNowDragItemReturn();
				return true;
			}

			if ( ( pInventory->GetSortTypeByItemTemplet( pFromItemTemplet ) == pInventory->GetSortTypeByItemTemplet( pToItemTemplet ) )
				|| ( ( pSlotItem->GetSlotType() == CX2SlotItem::ST_BANK ) && ( (*m_pSlotBeforeDragging)->GetSlotType() == CX2SlotItem::ST_BANK ) )
				|| ( pInventory->IsPossibleAddItem( pInventory->GetSortTypeByItemTemplet( pFromItemTemplet ) ) == true ) )
			{
				if( g_pData->GetUIManager()->GetUIInventory()->Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( static_cast< CX2SlotItem* >( *m_pSlotBeforeDragging ),
					pSlotItem ) == true )	// Ȯ���� ���� �����Ƿ� ����ó�� ����
				{
					CX2SlotItem*	pSlotBeforeDragging = static_cast< CX2SlotItem* >( *m_pSlotBeforeDragging );
					pSlotBeforeDragging->DestroyItemUI();
				}
				else
				{
					SetNowDragItemReturn();
				}
			}
			else
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_3860 ), g_pMain->GetNowState() );
				SetNowDragItemReturn();
			}

		}
		return true;
	default:
		break;
	}

	return false;
}

bool CX2UIPrivateBank::OnRClickedItem( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pSlotItem = NULL;
	pSlotItem = static_cast< CX2SlotItem* >( GetSlotInMousePos( mousePos ) );

	if ( pSlotItem == NULL )
		return false;

	if ( pSlotItem->IsResetItemUI() == false )
		return false;

	const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( pSlotItem->GetItemTID() );
	if ( pItemTemplet == NULL )
		return false;

	if ( g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_INVEN ) == false )
	{
		g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_INVEN, true );
	}
	//g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTabByUid( pSlotItem->GetItemUID() );

	CX2Inventory::SORT_TYPE	nowInvenSortType	= CX2Inventory::ST_NONE;
	const CX2Inventory*			pInventory			= NULL;

	//nowInvenSortType	=	g_pData->GetUIManager()->GetUIInventory()->GetSortType();
	pInventory			=	GetMyInventory();

	if ( pInventory == NULL )
		return false;

	nowInvenSortType	= pInventory->GetSortTypeByItemTemplet( pItemTemplet );
	g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTabByUid( pSlotItem->GetItemUID() );


	CX2Item*	pItem = NULL;
	for ( int i = 0; i < pInventory->GetItemMaxNum( nowInvenSortType ); i++ )
	{
		pItem	=	pInventory->GetItem( nowInvenSortType, i );
		if ( pItem	== NULL )
		{
			if( g_pData->GetUIManager()->GetUIInventory()->Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( pSlotItem->GetSortType(),
				pSlotItem->GetSlotID(), nowInvenSortType, i ) == true )
			{
				pSlotItem->DestroyItemUI();
				InvalidSlotDesc();
			}
			else
			{
				SetNowDragItemReturn();
			}
			return true;
		}
		pItem	= NULL;
	}

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_3860 ), g_pMain->GetNowState() );
	return false;	// ��...���̾�α�??
}

bool CX2UIPrivateBank::Handler_EGS_GET_MY_BANK_INFO_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*) lParam;
	KEGS_GET_MY_BANK_INFO_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_GET_MY_BANK_INFO_ACK );
	{
		if ( kEvent.m_mapItem.size() == 0 )
			return false;

		CX2Inventory*	pInventory		=	NULL;
		pInventory		= AccessMyInventory();

		if ( pInventory != NULL )
		{
			pInventory->SetBank( kEvent.m_mapItem );
			UpdateUI();

			return true;
		}
	}

	return false;
}

void CX2UIPrivateBank::ResetPrivateBank()
{
	LostItemSlotList();
	m_SlotList.clear();

	DestoryPrivateBank();
	InitUIPrivateBank( g_pMain->GetNowState(), L"DLG_Private_Bank_Reform.lua" );

	UpdateUIStrMembership();
	UpdateUI();
	SetSlotManagerLayer( static_cast<int>(XDL_NORMAL_3) );
}

bool CX2UIPrivateBank::Handler_EGS_GET_SHARE_BANK_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*) lParam;
	KEGS_GET_SHARE_BANK_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_GET_SHARE_BANK_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Inventory*	pInventory		=	NULL;
			pInventory		= AccessMyInventory();

			if ( pInventory != NULL )
			{
				pInventory->SetShareBank( kEvent.m_mapItem );
			}

			SetShareUnitUID( kEvent.m_iUnitUID );
			SetShareNickName( kEvent.m_wstrNickName );
			SetShareBankSize( kEvent.m_iBankSize );
			SetIsShareBank(true);

			//	���� UI �ݱ�� ���ο���
			ResetPrivateBank();

			return true;
		}
	}

	return false;
}

bool CX2UIPrivateBank::Handler_EGS_SHARE_BANK_UPDATE_OK_NOT()
{
	if( m_pDLGWaitingMsgBox != NULL )
	{
		SAFE_DELETE_DIALOG(m_pDLGWaitingMsgBox);
		m_pDLGWaitingMsgBox = NULL;
	}

	return true;
}

wstring CX2UIPrivateBank::GetSlotItemDesc()
{

	wstring itemDesc=  L"";

	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->IsResetItemUI() == false )
		{
			itemDesc = m_pNowOverItemSlot->GetSlotDesc();
		}
		else
		{
			if( GetIsShareBank() )
			{
				CX2Inventory* pInventory	= AccessMyInventory();

				if ( pInventory == NULL )
				{
					ASSERT( !"Inventory Is NULL " );
					return L"";
				}

				CX2Item*		pItem			=	pInventory->GetItem( CX2Inventory::ST_SHARE_BANK, m_pNowOverItemSlot->GetSlotID() );

				if ( pItem == NULL )
				{
					ASSERT( !"Item Is NULL" );
					return L"";
				}

				itemDesc = GetSlotItemDescByTID( pItem, m_pNowOverItemSlot->GetItemTID() );
			}
			else
			{
				itemDesc = GetSlotItemDescByUID( m_pNowOverItemSlot->GetItemUID() );
			}
		}
	}

	return itemDesc;
}

//}} kimhc // 2009-08-04 // ĳ���ͺ� ����

//#pragma region GOOD_ELSWORD
//#pragma endregion ED�� ���� Ȯ�� ��Ŷ �Լ�
