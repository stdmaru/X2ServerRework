#include "stdafx.h"
#include "./X2UIComboTree.h"




CX2UIComboTree::CX2UIComboTree()
: m_hDLGComboTree( NULL )
, m_eUnitClass( CX2Unit::UC_NONE )
, m_eComboCategory( CX2ComboTree::CC_NORMAL )
, m_bShow( false )
, m_DLGSize( 0, 0 )
, m_iOpacity( 255 )
, m_iNumMouseOverButton( 0 )
{
}

CX2UIComboTree::~CX2UIComboTree()
{
	SAFE_DELETE_DIALOG( m_hDLGComboTree );
	SAFE_DELETE_DIALOG( m_pDLGComboTreeDesc );

}


bool CX2UIComboTree::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case CTUCM_OPEN_SKILL_TREE:
		{
			{
				if( true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_COMBO_TREE ) )
				{
					g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_COMBO_TREE );
				}

				if( false == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_SKILL ) )
				{
					g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_SKILL );
				}
			}
			return true;
		} break;

	case CTUCM_CLOSE_COMBO_TREE:
		{
			if( true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_COMBO_TREE ) )
			{
				g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_COMBO_TREE );
			}
			return true;

		} break;

	case CTUCM_CATEGORY_NORMAL_COMBO:
		{
			UpdateComboTree( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), CX2ComboTree::CC_NORMAL );
			return true;
		} break;
	case CTUCM_CATEGORY_JUMP_COMBO:
		{
			UpdateComboTree( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), CX2ComboTree::CC_JUMP );
			return true;

		} break;
	case CTUCM_CATEGORY_DASH_COMBO:
		{
			UpdateComboTree( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), CX2ComboTree::CC_DASH );
			return true;

		} break;
	case CTUCM_CATEGORY_DASH_JUMP_COMBO:
		{
			UpdateComboTree( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), CX2ComboTree::CC_DASH_JUMP );
			return true;

		} break;

	case CTUCM_CATEGORY_SPECIAL_COMBO:		/// 특수 콤보 트리
		{
			UpdateComboTree( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), CX2ComboTree::CC_SPECIAL );
			return true;

		} break;


	case CTUCM_SLIDE_OPACITY:
		{

			CKTDGUISlider* pSlider = (CKTDGUISlider*) lParam;

			if( NULL != m_hDLGComboTree )
			{
				CKTDGUIStatic* pStaticBG = (CKTDGUIStatic*) m_hDLGComboTree->GetControl( L"COMBO_TREE_BG" );
				if( NULL != pStaticBG )
				{
					D3DXCOLOR currColor = pStaticBG->GetColor();
					currColor.a = (float)pSlider->GetValue() / 255.f;
					pStaticBG->SetColor( currColor );
				}

				//D3DXCOLOR currColor = m_hDLGComboTree->GetColor();
				//currColor.a = (float)pSlider->GetValue() / 255.f;
				//m_hDLGComboTree->SetColor( currColor );
			}
			return true;
		} break;
	case CTUCM_COMBO_ICON_MOUSE_OVER:
		{
			CKTDGUIButton* pControl = (CKTDGUIButton*)lParam;
			D3DXVECTOR2 vControlPos(0.f, 0.f);
			if( pControl->GetDialog() != NULL )
			{
				vControlPos = pControl->GetPos() + pControl->GetDialog()->GetPos();
			}


			wstringstream wstrm;
			int iDummyInt = pControl->GetDummyInt(0);

			switch ( iDummyInt )
			{
			case 0:		// 파괴
				if( CX2Unit::UT_ELESIS == g_pData->GetSelectUnitType() )
				{ // 엘리시스는 섬멸 가이드
					wstrm << L"#CFF4444" << L"\n" << GET_STRING(STR_ID_26113) << L"#CX" << L"\n";
				}
				else
				{
					wstrm << L"#CFF4444" << L"\n" << GET_STRING(STR_ID_14560) << L"#CX" << L"\n";
				}
				break;
			case 1:		// 활력
				if( CX2Unit::UT_ELESIS == g_pData->GetSelectUnitType() )
				{ // 엘리시스는 질풍 가이드
					wstrm << L"#C4488FF" << L"\n" << GET_STRING(STR_ID_26112) << L"#CX" << L"\n";
				}
				else
				{
					wstrm << L"#C4488FF" << L"\n" << GET_STRING(STR_ID_14559) << L"#CX" << L"\n";
				}
				break;

			case 2:		// 오버히트
				wstrm << L"#CFF4444" << L"\n" << GET_STRING(STR_ID_15438) << L"#CX" << L"\n";
				break;

			case 3:
				wstrm << L"#CFF4444" << L"\n" << GET_STRING(STR_ID_22453) << L"#CX" << L"\n";
				break;
			case 4:
				wstrm << L"#CFF4444" << L"\n" << GET_STRING(STR_ID_26861) << L"#CX" << L"\n";
				break;
			case 5:
				wstrm << L"#CD4B5FF" << L"\n" << GET_STRING(STR_ID_29882) << L"#CX" << L"\n";
				break;
			}

			// 출력
			DrawComboTreeDesc( vControlPos, wstrm.str() );

		} break;
	case CTUCM_COMBO_ICON_MOUSE_OUT:
		{
			if( m_pDLGComboTreeDesc != NULL )
				m_pDLGComboTreeDesc->SetShow( false );
		} break;
	}
	return false;
}


void CX2UIComboTree::OnStateChange()
{
	UpdateOpacitySlider();
}


void CX2UIComboTree::UpdateOpacitySlider()
{
	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_TRAINING_GAME:
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_PVP_GAME:
		{
			// 슬라이더 컨트롤을 활성화 시킨다
			UpdateOpacitySlider( true );
		} break;


	default:
		{
			// 슬라이더 컨트롤을 비활성화 시킨다
			UpdateOpacitySlider( false );
		} break;
	}
}

void CX2UIComboTree::UpdateOpacitySlider( bool bEnable )
{
	if( NULL == m_hDLGComboTree )
		return;

	CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_hDLGComboTree->GetControl( L"g_pStatic_Opacity" );
	CKTDGUISlider* pSlider = (CKTDGUISlider*) m_hDLGComboTree->GetControl( L"Slider_Opacity" );

	if( NULL == pStatic )
		return;

	if( NULL == pSlider )
		return;

	if( pStatic->GetShow() == bEnable )
		return;


	pStatic->SetShowEnable( bEnable, bEnable );
	pSlider->SetShowEnable( bEnable, bEnable );


	if( true == bEnable )
	{
		ASSERT( m_iOpacity > 50 );
		pSlider->SetValue( m_iOpacity );

		CKTDGUIStatic* pStaticBG = (CKTDGUIStatic*) m_hDLGComboTree->GetControl( L"COMBO_TREE_BG" );
		if( NULL != pStaticBG )
		{
			D3DXCOLOR currColor = pStaticBG->GetColor();
			currColor.a = (float)pSlider->GetValue() / 255.f;
			pStaticBG->SetColor( currColor );
		}

		//D3DXCOLOR currColor = m_hDLGComboTree->GetColor();
		//currColor.a = (float)pSlider->GetValue() / 255.f;
		//m_hDLGComboTree->SetColor( currColor );
	}
	else
	{
		m_iOpacity = pSlider->GetValue();
		ASSERT( m_iOpacity > 50 );

		CKTDGUIStatic* pStaticBG = (CKTDGUIStatic*) m_hDLGComboTree->GetControl( L"COMBO_TREE_BG" );
		if( NULL != pStaticBG )
		{
			D3DXCOLOR currColor = pStaticBG->GetColor();
			currColor.a = 1.f;
			pStaticBG->SetColor( currColor );
		}
		//D3DXCOLOR currColor = m_hDLGComboTree->GetColor();
		//currColor.a = 1.f;
		//m_hDLGComboTree->SetColor( currColor );
	}

}


void CX2UIComboTree::OpenComboTree( bool bOpen )
{
	if( false == bOpen )
	{
		if( NULL != m_hDLGComboTree )
		{
			m_hDLGComboTree->SetShowEnable( bOpen, bOpen );
			UpdateOpacitySlider();
		}
	}
	else
	{
		if( NULL == m_hDLGComboTree )
		{
			m_hDLGComboTree = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Command_Skill.lua", 0.f );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_hDLGComboTree );
			m_hDLGComboTree->SetDisableUnderWindow(true);

			D3DXVECTOR3 tmp;
			tmp = m_hDLGComboTree->GetDummyPos(1);
			m_DLGSize.x = tmp.x;
			m_DLGSize.y = tmp.y;
		}

		m_hDLGComboTree->SetShowEnable( bOpen, bOpen );
		UpdateOpacitySlider();
	}
}



void CX2UIComboTree::UpdateComboTree( CX2Unit::UNIT_CLASS eUnitClass, CX2ComboTree::COMBO_CATEGORY eComboCategory /*= CX2ComboTree::CC_INVALID */ )
{
	if( m_hDLGComboTree == NULL )
		return;

	const wstring staticControlName[5] =
	{
		L"Static_Normal_Combo_Created",
		L"Static_Jump_Combo_Created",
		L"Static_Dash_Combo_Created",
		L"Static_DashJump_Combo_Created",
		L"Static_Special_Created",			/// 특수 콤보 트리
	};

	if( m_eUnitClass != eUnitClass )
	{
		/// 타 캐릭터로 처음 갱신시, 일반 페이지 표시
		eComboCategory = CX2ComboTree::CC_NORMAL;

		const CX2ComboTree::ComboSet* pComboSet = g_pData->GetComboTree()->GetUnitComboSet( eUnitClass );
		ASSERT( NULL != pComboSet );
		if( NULL == pComboSet )
			return;

		m_eUnitClass = eUnitClass;


		std::wstring textureName = L"";
		std::wstring pieceName = L"";

		if( m_iNumMouseOverButton > 0 )
		{
			for( int i=0; i< m_iNumMouseOverButton; i++ )
			{
				m_hDLGComboTree->DeleteControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"MouseOverButton" ), i ) ) );
			}
			m_vecMouseOverButton[0].clear();
			m_vecMouseOverButton[1].clear();
			m_vecMouseOverButton[2].clear();
			m_vecMouseOverButton[3].clear();
			m_vecMouseOverButton[4].clear();
		}
		m_iNumMouseOverButton = 0;

		for( int i=0; i<std::ssize( staticControlName ); i++ )
		{
			m_hDLGComboTree->DeleteControl( staticControlName[i].c_str() );

			CKTDGUIStatic* pStaticCombo = m_hDLGComboTree->CreateStatic();
			m_hDLGComboTree->AddControl( pStaticCombo );
			pStaticCombo->SetName( staticControlName[i].c_str() );


			const std::vector<CX2ComboTree::ComboIcon>* pvecCombo = NULL;
			switch( i )
			{
			case 0:
				{
					pvecCombo = &pComboSet->m_vecNormalCombo;
				} break;

			case 1:
				{
					pvecCombo = &pComboSet->m_vecJumpCombo;
				} break;

			case 2:
				{
					pvecCombo = &pComboSet->m_vecDashCombo;
				} break;

			case 3:
				{
					pvecCombo = &pComboSet->m_vecDashJumpCombo;
				} break;
			case 4:		/// 특수 콤보 트리
				{
					pvecCombo = &pComboSet->m_vecSpecialCombo;
				} break;
			}


			for( const CX2ComboTree::ComboIcon& comboIcon : *pvecCombo )
			{
				if( false == GetComboIconTextureName( textureName, pieceName, comboIcon.m_eComboIcon ) )
				{
					ASSERT( !"invalid combo icon" );
					continue;
				}

				CKTDGUIControl::CPictureData* pPicture = m_hDLGComboTree->CreatePicture();
				pPicture->SetTex( textureName.c_str(), pieceName.c_str() );

				CKTDGUIControl::UIPointData* pPointData = new CKTDGUIControl::UIPointData();
				pPointData->leftTopPoint = CalcPicturePosition( comboIcon.m_eComboIcon, comboIcon.m_vLeftTop );
				pPicture->SetPoint( pPointData );

				pStaticCombo->AddPicture( pPicture );

				CreateMouseOverButton( comboIcon, i );
			}
		}

		/// 특수 콤보 트리 표기 설정
		CKTDGUIRadioButton* pRadio_Special = (CKTDGUIRadioButton*) m_hDLGComboTree->GetControl( L"RadioButton_Special" );

		if ( NULL != g_pData && NULL != pRadio_Special )
		{
			/// 특수 콤보 설정이 되어 있다면, 표기
			bool bShow = ( true == pComboSet->m_vecSpecialCombo.empty() ) ? false : true;

			pRadio_Special->SetShow( bShow );
		}
	}


	m_eComboCategory = eComboCategory;

	for( int i=0; i<std::ssize( staticControlName ); i++ )
	{
		CKTDGUIStatic* pStaticCombo	= (CKTDGUIStatic*) m_hDLGComboTree->GetControl( staticControlName[i].c_str() );
		pStaticCombo->SetShowEnable( false, false );
		for(UINT j=0; j< m_vecMouseOverButton[i].size(); j++ )
		{
			CKTDGUIButton* pButton	= m_vecMouseOverButton[i][j];
			pButton->SetShowEnable( false, false );
		}
	}



	switch( eComboCategory )
	{
	case CX2ComboTree::CC_NORMAL:
		{
			CKTDGUIStatic* pStaticCombo	= (CKTDGUIStatic*) m_hDLGComboTree->GetControl( staticControlName[0].c_str() );
			pStaticCombo->SetShowEnable( true, true );

			CKTDGUIRadioButton* pRadio_Normal = (CKTDGUIRadioButton*) m_hDLGComboTree->GetControl( L"RadioButton_Basic" );
			pRadio_Normal->SetChecked( true );
			for(UINT i=0; i< m_vecMouseOverButton[0].size(); i++ )
			{
				CKTDGUIButton* pButton	= m_vecMouseOverButton[0][i];
				pButton->SetShowEnable( true, true );
			}
		} break;
	case CX2ComboTree::CC_JUMP:
		{
			CKTDGUIStatic* pStaticCombo	= (CKTDGUIStatic*) m_hDLGComboTree->GetControl( staticControlName[1].c_str() );
			pStaticCombo->SetShowEnable( true, true );

			CKTDGUIRadioButton* pRadio_Jump = (CKTDGUIRadioButton*) m_hDLGComboTree->GetControl( L"RadioButton_Jump" );
			pRadio_Jump->SetChecked( true );
			for(UINT i=0; i< m_vecMouseOverButton[1].size(); i++ )
			{
				CKTDGUIButton* pButton	= m_vecMouseOverButton[1][i];
				pButton->SetShowEnable( true, true );
			}
		} break;
	case CX2ComboTree::CC_DASH:
		{
			CKTDGUIStatic* pStaticCombo	= (CKTDGUIStatic*) m_hDLGComboTree->GetControl( staticControlName[2].c_str() );
			pStaticCombo->SetShowEnable( true, true );

			CKTDGUIRadioButton* pRadio_Dash = (CKTDGUIRadioButton*) m_hDLGComboTree->GetControl( L"RadioButton_Dash" );
			pRadio_Dash->SetChecked( true );
			for(UINT i=0; i< m_vecMouseOverButton[2].size(); i++ )
			{
				CKTDGUIButton* pButton	= m_vecMouseOverButton[2][i];
				pButton->SetShowEnable( true, true );
			}
		} break;
	case CX2ComboTree::CC_DASH_JUMP:
		{
			CKTDGUIStatic* pStaticCombo	= (CKTDGUIStatic*) m_hDLGComboTree->GetControl( staticControlName[3].c_str() );
			pStaticCombo->SetShowEnable( true, true );

			CKTDGUIRadioButton* pRadio_DashJump = (CKTDGUIRadioButton*) m_hDLGComboTree->GetControl( L"RadioButton_Dash_Jump" );
			pRadio_DashJump->SetChecked( true );
			for(UINT i=0; i< m_vecMouseOverButton[3].size(); i++ )
			{
				CKTDGUIButton* pButton	= m_vecMouseOverButton[3][i];
				pButton->SetShowEnable( true, true );
			}
		} break;
	case CX2ComboTree::CC_SPECIAL:		/// 특수 콤보 트리
		{
			CKTDGUIStatic* pStaticCombo	= (CKTDGUIStatic*) m_hDLGComboTree->GetControl( staticControlName[4].c_str() );

			pStaticCombo->SetShowEnable( true, true );

			CKTDGUIRadioButton* pRadio_Special = (CKTDGUIRadioButton*) m_hDLGComboTree->GetControl( L"RadioButton_Special" );
			pRadio_Special->SetChecked( true );
			for(UINT i=0; i< m_vecMouseOverButton[4].size(); i++ )
			{
				CKTDGUIButton* pButton	= m_vecMouseOverButton[4][i];
				pButton->SetShowEnable( true, true );
			}
		} break;
	}

}


D3DXVECTOR2 CX2UIComboTree::CalcPicturePosition( CX2ComboTree::COMBO_TREE_ICON eIcon, const D3DXVECTOR2& vGridPosition )
{

	const D3DXVECTOR2 vLeftTopOffset( 35, 100 );
	const D3DXVECTOR2 vGridSize( 60, 60 );

	const D3DXVECTOR2 vOffsetIcon( 7, 7 );
	const D3DXVECTOR2 vOffsetAttackDecision( -3, -1 );

	const D3DXVECTOR2 vOffsetRightArrow( 48, 20 );
	const D3DXVECTOR2 vOffsetUpArrow( 20, 16 );
	const D3DXVECTOR2 vOffsetDownArrow( 20, -20 );

	const D3DXVECTOR2 vOffsetOneKey( 31, 31 );
	const D3DXVECTOR2 vOffsetTwoKey( 6, 31 );

//#ifdef BALANCE_ELEMENTAL_MASTER_20130117
	const D3DXVECTOR2 vOffsetDoubleUpArrow( 20, -40 );
//#endif BALANCE_ELEMENTAL_MASTER_20130117



	D3DXVECTOR2 vReturnPosition( vGridPosition.x * vGridSize.x, vGridPosition.y * vGridSize.y );
	vReturnPosition += vLeftTopOffset;


	switch( eIcon )
	{
	default:
	case CX2ComboTree::CTI_LEFT:
	case CX2ComboTree::CTI_RIGHT:
		{
			ASSERT( !"not yet" );
		} break;

	case CX2ComboTree::CTI_Z:
	case CX2ComboTree::CTI_X:
 	case CX2ComboTree::CTI_Z_BLUE:
	case CX2ComboTree::CTI_X_BLUE:
	case CX2ComboTree::CTI_Z_RED:
	case CX2ComboTree::CTI_X_RED:
	case CX2ComboTree::CTI_X_CONTINUE_OVERHEAT:
	case CX2ComboTree::CTI_UP:
	case CX2ComboTree::CTI_DOWN:
	case CX2ComboTree::CTI_DASH:
	case CX2ComboTree::CTI_PUSH_Z_OVERHEAT:
		case CX2ComboTree::CTI_X_DRAGON_TOOTH:
		case CX2ComboTree::CTI_Z_WOLF_TOOTH:
		case CX2ComboTree::CTI_RIGHT_Z_PURPLE:
		case CX2ComboTree::CTI_RIGHT_X_PURPLE:
		case CX2ComboTree::CTI_RIGHT_Z_PURPLE_CONTINUE:
		case CX2ComboTree::CTI_RIGHT_X_PURPLE_CHARGE:
		{
			vReturnPosition += vOffsetOneKey;
		} break;

	case CX2ComboTree::CTI_Z_CONTINUE:
	case CX2ComboTree::CTI_X_CONTINUE:
	case CX2ComboTree::CTI_Z_CONTINUE_BLUE:
	case CX2ComboTree::CTI_X_CONTINUE_BLUE:
	case CX2ComboTree::CTI_Z_CONTINUE_RED:
	case CX2ComboTree::CTI_X_CONTINUE_RED:
	case CX2ComboTree::CTI_DASH_UP:
	case CX2ComboTree::CTI_LEFT_Z:
	case CX2ComboTree::CTI_LEFT_X:
	case CX2ComboTree::CTI_RIGHT_Z:
	case CX2ComboTree::CTI_RIGHT_X:
	case CX2ComboTree::CTI_UP_Z:
	case CX2ComboTree::CTI_UP_X:
	case CX2ComboTree::CTI_DOWN_Z:
	case CX2ComboTree::CTI_DOWN_X:
	case CX2ComboTree::CTI_LEFT_Z_BLUE:
	case CX2ComboTree::CTI_LEFT_X_BLUE:
	case CX2ComboTree::CTI_RIGHT_Z_BLUE:
	case CX2ComboTree::CTI_RIGHT_X_BLUE:
	case CX2ComboTree::CTI_UP_Z_BLUE:
	case CX2ComboTree::CTI_UP_X_BLUE:
	case CX2ComboTree::CTI_DOWN_Z_BLUE:
	case CX2ComboTree::CTI_DOWN_X_BLUE:
	case CX2ComboTree::CTI_LEFT_Z_RED:
	case CX2ComboTree::CTI_LEFT_X_RED:
	case CX2ComboTree::CTI_RIGHT_Z_RED:
	case CX2ComboTree::CTI_RIGHT_X_RED:
	case CX2ComboTree::CTI_UP_Z_RED:
	case CX2ComboTree::CTI_UP_X_RED:
	case CX2ComboTree::CTI_DOWN_Z_RED:
	case CX2ComboTree::CTI_DOWN_X_RED:
	case CX2ComboTree::CTI_PUSH_Z:
	case CX2ComboTree::CTI_PUSH_X:
		{
			vReturnPosition += vOffsetTwoKey;
		} break;


	case CX2ComboTree::CTI_UP_DECISION:
	case CX2ComboTree::CTI_DOWN_DECISION:
		{
			vReturnPosition += vOffsetAttackDecision;
		} break;



		// 꺽인선
	case CX2ComboTree::CTI_LINE_RIGHT:
		{
			vReturnPosition += vOffsetRightArrow;
		} break;

	case CX2ComboTree::CTI_LINE_UP_RIGHT:
		{
			vReturnPosition += vOffsetUpArrow;
		} break;

//#ifdef BALANCE_ELEMENTAL_MASTER_20130117		/// 꺽인선 추가
	case CX2ComboTree::CTI_LINE_DOUBLE_UP_RIGHT:
		{
			vReturnPosition += vOffsetDoubleUpArrow;
		} break;
//#endif BALANCE_ELEMENTAL_MASTER_20130117

	case CX2ComboTree::CTI_LINE_DOWN:
	case CX2ComboTree::CTI_LINE_DOWN_RIGHT:
	case CX2ComboTree::CTI_LINE_DOUBLE_DOWN_RIGHT:
//#ifdef BALANCE_ELEMENTAL_MASTER_20130117		/// 꺽인선 추가
	case CX2ComboTree::CTI_LINE_DOWN_DOUBLE_RIGHT:
	case CX2ComboTree::CTI_LINE_THREEWAY_INTERSECTION:
//#endif BALANCE_ELEMENTAL_MASTER_20130117
		{
			vReturnPosition += vOffsetDownArrow;
		} break;

	case CX2ComboTree::CTI_ICON_JUMP:
	case CX2ComboTree::CTI_ICON_DOUBLE_JUMP:
	case CX2ComboTree::CTI_ICON_DASH:
	case CX2ComboTree::CTI_ICON_DASH_JUMP:
	case CX2ComboTree::CTI_ICON_SWORD:
	case CX2ComboTree::CTI_ICON_STAFF:
	case CX2ComboTree::CTI_ICON_FIST:
	case CX2ComboTree::CTI_ICON_KICK:
	case CX2ComboTree::CTI_ICON_GEAR:
	case CX2ComboTree::CTI_ICON_FIRE_BALL:
	case CX2ComboTree::CTI_ICON_ARROW:
	case CX2ComboTree::CTI_ICON_TRIPLE_ARROW:
	case CX2ComboTree::CTI_ICON_MAGIC_AREA:
	case CX2ComboTree::CTI_ICON_DARK_FIRE_BALL:
	case CX2ComboTree::CTI_ICON_ELECTRON_BALL:
	case CX2ComboTree::CTI_ICON_EXPLOSION:
	case CX2ComboTree::CTI_ICON_TRIANGLE:
	case CX2ComboTree::CTI_ICON_RUNE_AREA:
	case CX2ComboTree::CTI_ICON_SPARK:

	//{{ kimhc // 2010.12.20 // 2010-12-23 New Character CHUNG
	case CX2ComboTree::CTI_ICON_CANNON:
	case CX2ComboTree::CTI_ICON_CANNON_BALL:
	case CX2ComboTree::CTI_ICON_CANNON_BALL_CHARGE:
//}} kimhc // 2010.12.20 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011.1.24 // 청 1차 전직
	case CX2ComboTree::CTI_ICON_HANDGUN:
		//}} kimhc // 2011.1.24 // 청 1차 전직
	case CX2ComboTree::CTI_ICON_LANDING:
	case CX2ComboTree::CTI_ICON_BEAM:
	case CX2ComboTree::CTI_ICON_BLACKHOLE:
	case CX2ComboTree::CTI_ICON_ACCEL_DASH:
	case CX2ComboTree::CTI_ICON_EXPLOSIVE_ARROW:
	case CX2ComboTree::CTI_ICON_SIEGE:
	case CX2ComboTree::CTI_ICON_SIEGE_SHELLING:
	case CX2ComboTree::CTI_ICON_SPEAR:
	case CX2ComboTree::CTI_ICON_HURRICANE:					/// 엘리멘탈 마스터 허리케인
	case CX2ComboTree::CTI_ICON_DYNAMO_Z:
	case CX2ComboTree::CTI_ICON_DYNAMO_X:
	case CX2ComboTree::CTI_ICON_DYNAMO_ROLLING:
	case CX2ComboTree::CTI_ICON_DYNAMO_TELEPORT:
	case CX2ComboTree::CTI_ICON_FIST_NA:
	case CX2ComboTree::CTI_ICON_KICK_NA:
	case CX2ComboTree::CTI_ICON_NA:
		{
			vReturnPosition += vOffsetIcon;
		} break;

	case CX2ComboTree::CTI_BACKGROUND:
		{
			// nothing
		} break;
	}

	return vReturnPosition;

}


bool CX2UIComboTree::GetComboIconTextureName( std::wstring& textureName, std::wstring& pieceName, CX2ComboTree::COMBO_TREE_ICON eIcon )
{
	textureName = L"DLG_UI_COMMON_TEXTURE10.TGA";

	switch( eIcon )
	{
	default:
	case CX2ComboTree::CTI_LEFT:
	case CX2ComboTree::CTI_RIGHT:
		{
			textureName = L"";
			pieceName = L"";
			ASSERT( !"not yet" );
			return false;
		} break;



	case CX2ComboTree::CTI_Z:
		{
			pieceName = L"Z_OVER";
		} break;
	case CX2ComboTree::CTI_X:
		{
			pieceName = L"X_OVER";
		} break;

	case CX2ComboTree::CTI_Z_CONTINUE:                   textureName = L"DLG_UI_COMMON_TEXTURE11.TGA";			pieceName = L"Z_CONTINUE_OVER";		 break;
	case CX2ComboTree::CTI_X_CONTINUE:                   textureName = L"DLG_UI_COMMON_TEXTURE11.TGA";			pieceName = L"X_CONTINUE_OVER";		 break;

	case CX2ComboTree::CTI_UP:
		{
			pieceName = L"JUMP_OVER";
		} break;
	case CX2ComboTree::CTI_DASH:
		{
			pieceName = L"DASH_OVER";
		} break;
	case CX2ComboTree::CTI_DASH_UP:
		{
			pieceName = L"DASH_JUMP_OVER";
		} break;
	case CX2ComboTree::CTI_LEFT_Z:
		{
			pieceName = L"L_Z_OVER";
		} break;
	case CX2ComboTree::CTI_LEFT_X:
		{
			pieceName = L"L_X_OVER";
		} break;

	case CX2ComboTree::CTI_RIGHT_Z:
		{
			pieceName = L"R_Z_OVER";
		} break;

	case CX2ComboTree::CTI_RIGHT_X:
		{
			pieceName = L"R_X_OVER";
		} break;

	case CX2ComboTree::CTI_UP_Z:
		{
			pieceName = L"JUMP_Z_OVER";
		} break;

	case CX2ComboTree::CTI_DOWN:
		{
			textureName = L"DLG_UI_COMMON_TEXTURE10.TGA";			pieceName = L"down_over";
		} break;

	case CX2ComboTree::CTI_UP_X:			textureName = L"DLG_UI_COMMON_TEXTURE11.TGA";			pieceName = L"JUMP_X_OVER";		 break;
	case CX2ComboTree::CTI_DOWN_Z:			textureName = L"DLG_UI_COMMON_TEXTURE11.TGA";			pieceName = L"DOWN_Z_OVER";		 break;
	case CX2ComboTree::CTI_DOWN_X:			textureName = L"DLG_UI_COMMON_TEXTURE11.TGA";			pieceName = L"DOWN_X_OVER";		 break;

	case CX2ComboTree::CTI_Z_BLUE:			textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_B";		break;
	case CX2ComboTree::CTI_X_BLUE:			textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_B";		break;
	case CX2ComboTree::CTI_Z_CONTINUE_BLUE:	textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"ZZ_B";		break;
	case CX2ComboTree::CTI_X_CONTINUE_BLUE:	textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"XX_B";		break;
	case CX2ComboTree::CTI_LEFT_Z_BLUE:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_BACK_B";	break;
	case CX2ComboTree::CTI_LEFT_X_BLUE:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_BACK_B";	break;
	case CX2ComboTree::CTI_RIGHT_Z_BLUE:	textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_FRONT_B";	break;
	case CX2ComboTree::CTI_RIGHT_X_BLUE:	textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_FRONT_B";	break;
	case CX2ComboTree::CTI_UP_Z_BLUE:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_UP_B";		break;
	case CX2ComboTree::CTI_UP_X_BLUE:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_UP_B";		break;
	case CX2ComboTree::CTI_DOWN_Z_BLUE:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_DOWN_B";	break;
	case CX2ComboTree::CTI_DOWN_X_BLUE:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_DOWN_B";	break;

	case CX2ComboTree::CTI_Z_RED:			textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_R";		break;
	case CX2ComboTree::CTI_X_RED:			textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_R";		break;
	case CX2ComboTree::CTI_Z_CONTINUE_RED:	textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"ZZ_R";		break;
	case CX2ComboTree::CTI_X_CONTINUE_RED:	textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"XX_R";		break;
	case CX2ComboTree::CTI_LEFT_Z_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_BACK_R";	break;
	case CX2ComboTree::CTI_LEFT_X_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_BACK_R";	break;
	case CX2ComboTree::CTI_RIGHT_Z_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_FRONT_R";	break;
	case CX2ComboTree::CTI_RIGHT_X_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_FRONT_R";	break;
	case CX2ComboTree::CTI_UP_Z_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_UP_R";		break;
	case CX2ComboTree::CTI_UP_X_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_UP_R";		break;
	case CX2ComboTree::CTI_DOWN_Z_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_DOWN_R";	break;
	case CX2ComboTree::CTI_DOWN_X_RED:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_DOWN_R";	break;
	case CX2ComboTree::CTI_X_CONTINUE_OVERHEAT:		textureName = L"DLG_UI_Common_Texture21.tga";			pieceName = L"XX_R";		break;
	case CX2ComboTree::CTI_PUSH_Z:					textureName = L"DLG_UI_Common_Texture43.tga";			pieceName = L"PRESS_NORMAL";	break;
	case CX2ComboTree::CTI_PUSH_Z_OVERHEAT:			textureName = L"DLG_UI_Common_Texture43.tga";			pieceName = L"PRESS_NORMAL_RED";	break;

	case CX2ComboTree::CTI_PUSH_X:					textureName = L"DLG_UI_Common_Texture43.tga";			pieceName = L"PRESS_NORMAL_X";		break;

		case CX2ComboTree::CTI_X_DRAGON_TOOTH:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"X_R";		break;
		case CX2ComboTree::CTI_Z_WOLF_TOOTH:		textureName = L"DLG_UI_Common_Texture16_NEW.tga";	pieceName = L"Z_R";		break;

		case CX2ComboTree::CTI_ICON_DYNAMO_Z:			textureName = L"DLG_UI_Common_Texture80_NEW.tga";	pieceName = L"COMMAND_ICON_Add_Z";				break;
		case CX2ComboTree::CTI_ICON_DYNAMO_X:			textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"COMMAND_ICON_Add_X";				break;
		case CX2ComboTree::CTI_ICON_DYNAMO_ROLLING:		textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"COMMAND_ICON_Add_Wheel";			break;
		case CX2ComboTree::CTI_ICON_DYNAMO_TELEPORT:	textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"COMMAND_ICON_Add_Teleport";		break;
		case CX2ComboTree::CTI_ICON_FIST_NA:			textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_PUNCH";				break;
		case CX2ComboTree::CTI_ICON_KICK_NA:			textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_KICK";				break;
		case CX2ComboTree::CTI_RIGHT_Z_PURPLE:			textureName = L"DLG_UI_Common_Texture80_NEW.tga";	pieceName = L"COMMAND_ICON_Z_Purple";			break;
		case CX2ComboTree::CTI_RIGHT_X_PURPLE:			textureName = L"DLG_UI_Common_Texture80_NEW.tga";	pieceName = L"COMMAND_ICON_X_Purple";			break;
		case CX2ComboTree::CTI_RIGHT_Z_PURPLE_CONTINUE:	textureName = L"DLG_UI_Common_Texture80_NEW.tga";	pieceName = L"COMMAND_ICON_Z_Purple_CONTINUE";	break;
		case CX2ComboTree::CTI_RIGHT_X_PURPLE_CHARGE:	textureName = L"DLG_UI_Common_Texture80_NEW.tga";	pieceName = L"COMMAND_ICON_X_Purple_CHARGE";	break;
		case CX2ComboTree::CTI_ICON_NA:					textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"ICON_NA";							break;

		// 띄우기 공격인지 다운공격인지 표시
	case CX2ComboTree::CTI_UP_DECISION:
		{
			pieceName = L"UP";
		} break;

	case CX2ComboTree::CTI_DOWN_DECISION:
		{
			pieceName = L"DOWN";
		} break;



		// 꺽인선
	case CX2ComboTree::CTI_LINE_RIGHT:
		{
			pieceName = L"TREE";
		} break;
	case CX2ComboTree::CTI_LINE_UP_RIGHT:
		{
			pieceName = L"TREE_A";
		} break;

	case CX2ComboTree::CTI_LINE_DOWN:
		{
			pieceName = L"TREE_B_VERTICAL";
		} break;

	case CX2ComboTree::CTI_LINE_DOWN_RIGHT:
		{
			pieceName = L"TREE_B";
		} break;

//#ifdef BALANCE_ELEMENTAL_MASTER_20130117		/// 꺽인선 추가
	case CX2ComboTree::CTI_LINE_DOUBLE_UP_RIGHT:
		{
			textureName = L"DLG_UI_Common_Texture26_NEW.tga";
			pieceName = L"LINE_DOUBLE_UP_RIGHT";
		} break;

	case CX2ComboTree::CTI_LINE_DOWN_DOUBLE_RIGHT:
		{
			textureName = L"DLG_UI_Common_Texture26_NEW.tga";
			pieceName = L"LINE_DOWN_DOUBLE_RIGHT";
		} break;

	case CX2ComboTree::CTI_LINE_THREEWAY_INTERSECTION:
		{
			textureName = L"DLG_UI_Common_Texture26_NEW.tga";
			pieceName = L"LINE_THREEWAY_INTERSECTION";
		} break;
//#endif BALANCE_ELEMENTAL_MASTER_20130117

	case CX2ComboTree::CTI_LINE_DOUBLE_DOWN_RIGHT:
		{
			pieceName = L"TREE_B_DOUBLE";
		} break;

	case CX2ComboTree::CTI_ICON_JUMP:				textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_JUMP";           	break;
	case CX2ComboTree::CTI_ICON_DOUBLE_JUMP:        textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_DOUBLE_JUMP";    	break;
	case CX2ComboTree::CTI_ICON_DASH:               textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_DASH";           	break;
	case CX2ComboTree::CTI_ICON_DASH_JUMP:          textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_DASH_JUMP";      	break;
	case CX2ComboTree::CTI_ICON_SWORD:              textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_SWORD";          	break;
	case CX2ComboTree::CTI_ICON_STAFF:              textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_STAFF";          	break;
	case CX2ComboTree::CTI_ICON_FIST:               textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_PUNCH";           	break;
	case CX2ComboTree::CTI_ICON_KICK:               textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_KICK";           	break;
	case CX2ComboTree::CTI_ICON_GEAR:               textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_GEAR";           	break;
	case CX2ComboTree::CTI_ICON_FIRE_BALL:          textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_FIRE_BALL";      	break;
	case CX2ComboTree::CTI_ICON_ARROW:              textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_ARROW";          	break;
	case CX2ComboTree::CTI_ICON_TRIPLE_ARROW:		textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_TRIPLE_ARROW";       break;
	case CX2ComboTree::CTI_ICON_MAGIC_AREA:         textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_MAGIC_AREA";     	break;
	case CX2ComboTree::CTI_ICON_DARK_FIRE_BALL:     textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_DARK_FIRE_BALL"; 	break;
	case CX2ComboTree::CTI_ICON_ELECTRON_BALL:      textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_ELECTRON_BALL";  	break;
	case CX2ComboTree::CTI_ICON_EXPLOSION:          textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_EXPLOSION";      	break;
	case CX2ComboTree::CTI_ICON_TRIANGLE:           textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_TRIANGLE";       	break;
	case CX2ComboTree::CTI_ICON_RUNE_AREA:          textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_RUNE_AREA";      	break;
	case CX2ComboTree::CTI_ICON_SPARK:				textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"COMMAND_ICON_SPARK";			 	break;

	//{{ kimhc // 2010.12.20 // 2010-12-23 New Character CHUNG
	case CX2ComboTree::CTI_ICON_CANNON:				textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_CANNON";       				break;
	case CX2ComboTree::CTI_ICON_CANNON_BALL:        textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_CANNON_BALL";      			break;
	case CX2ComboTree::CTI_ICON_CANNON_BALL_CHARGE:	textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_CANNON_BALL_CHARGE";			break;
//}} kimhc // 2010.12.20 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011.1.24 // 청 1차 전직
	case CX2ComboTree::CTI_ICON_HANDGUN:			textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_HANDGUN";					break;
		//}} kimhc // 2011.1.24 // 청 1차 전직
	case CX2ComboTree::CTI_ICON_LANDING:			textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_LANDING";					break;
	case CX2ComboTree::CTI_ICON_BEAM:				textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_EVE_LASER";					break;

	case CX2ComboTree::CTI_BACKGROUND:					pieceName = L"ICON_BG";      						break;
	case CX2ComboTree::CTI_ICON_BLACKHOLE:
		textureName = L"DLG_UI_Common_Texture18.tga";		pieceName = L"ICON_BLACKHOLE";					break;
	case CX2ComboTree::CTI_ICON_ACCEL_DASH:			textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"COMMAND_ICON_ACCEL_DASH";			break;
	case CX2ComboTree::CTI_ICON_EXPLOSIVE_ARROW:	textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"COMMAND_ICON_EXPLOSIVE_ARROW";	break;
	case CX2ComboTree::CTI_ICON_SIEGE:				textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"ICON_SIEGE";			break;
	case CX2ComboTree::CTI_ICON_SIEGE_SHELLING:		textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"ICON_SIEGE_SHELLING";	break;
	case CX2ComboTree::CTI_ICON_SPEAR:				textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"ICON_SPEAR";			break;
	case CX2ComboTree::CTI_ICON_HURRICANE:			textureName = L"DLG_UI_Common_Texture20.tga";		pieceName = L"ICON_HURRICAIN";		break;
	}

	return true;
}

void CX2UIComboTree::SetShow( bool bShow )
{
	m_bShow = bShow;

	OpenComboTree( bShow );
	if( true == bShow )
	{
		ASSERT( NULL != g_pData->GetMyUser()->GetSelectUnit() );
		if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			UpdateComboTree( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), m_eComboCategory );
		}


		g_pData->GetUIManager()->UIOpened( CX2UIManager::UI_MENU_COMBO_TREE );
	}
	else
	{
		g_pData->GetUIManager()->UIClosed( CX2UIManager::UI_MENU_COMBO_TREE );
		if( m_pDLGComboTreeDesc != NULL )
			m_pDLGComboTreeDesc->SetShow( false );
	}

}


void CX2UIComboTree::SetPosition( D3DXVECTOR2 vPos )
{
	if( m_hDLGComboTree != NULL )
	{
		m_hDLGComboTree->SetPos( vPos );
	}
}

void CX2UIComboTree::SetLayer( X2_DIALOG_LAYER layer )
{
	if(m_hDLGComboTree != NULL)
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_hDLGComboTree, layer );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_hDLGComboTree, false );
	}
}

void CX2UIComboTree::CreateMouseOverButton( const CX2ComboTree::ComboIcon& eIcon_, int staticComboIndex_ )
{
	bool bCheck = false;
	float fAddSizeX = 0.f;
	float fAddSizeY = 0.f;
	int iDummyInt = 0;
	switch( eIcon_.m_eComboIcon )
	{
	case CX2ComboTree::CTI_Z_BLUE:
	case CX2ComboTree::CTI_X_BLUE:
	case CX2ComboTree::CTI_Z_RED:
	case CX2ComboTree::CTI_X_RED:
	case CX2ComboTree::CTI_X_DRAGON_TOOTH:
	case CX2ComboTree::CTI_Z_WOLF_TOOTH:
	case CX2ComboTree::CTI_ICON_FIST_NA:
	case CX2ComboTree::CTI_ICON_KICK_NA:
	case CX2ComboTree::CTI_RIGHT_Z_PURPLE:
	case CX2ComboTree::CTI_RIGHT_X_PURPLE:
	case CX2ComboTree::CTI_RIGHT_Z_PURPLE_CONTINUE:
	case CX2ComboTree::CTI_RIGHT_X_PURPLE_CHARGE:
	case CX2ComboTree::CTI_ICON_NA:
		{
			bCheck = true;
		} break;
	case CX2ComboTree::CTI_Z_CONTINUE_BLUE:
	case CX2ComboTree::CTI_X_CONTINUE_BLUE:
	case CX2ComboTree::CTI_LEFT_Z_BLUE:
	case CX2ComboTree::CTI_LEFT_X_BLUE:
	case CX2ComboTree::CTI_RIGHT_Z_BLUE:
	case CX2ComboTree::CTI_RIGHT_X_BLUE:
	case CX2ComboTree::CTI_UP_Z_BLUE:
	case CX2ComboTree::CTI_UP_X_BLUE:
	case CX2ComboTree::CTI_DOWN_Z_BLUE:
	case CX2ComboTree::CTI_DOWN_X_BLUE:
	case CX2ComboTree::CTI_Z_CONTINUE_RED:
	case CX2ComboTree::CTI_X_CONTINUE_RED:
	case CX2ComboTree::CTI_LEFT_Z_RED:
	case CX2ComboTree::CTI_LEFT_X_RED:
	case CX2ComboTree::CTI_RIGHT_Z_RED:
	case CX2ComboTree::CTI_RIGHT_X_RED:
	case CX2ComboTree::CTI_UP_Z_RED:
	case CX2ComboTree::CTI_UP_X_RED:
	case CX2ComboTree::CTI_DOWN_Z_RED:
	case CX2ComboTree::CTI_DOWN_X_RED:
	case CX2ComboTree::CTI_X_CONTINUE_OVERHEAT:
	case CX2ComboTree::CTI_PUSH_Z_OVERHEAT:
		{
			bCheck = true;
			fAddSizeX = 22.f;
		}
		break;
	}

	switch( eIcon_.m_eComboIcon )
	{
	case CX2ComboTree::CTI_Z_BLUE:
	case CX2ComboTree::CTI_X_BLUE:
	case CX2ComboTree::CTI_Z_CONTINUE_BLUE:
	case CX2ComboTree::CTI_X_CONTINUE_BLUE:
	case CX2ComboTree::CTI_LEFT_Z_BLUE:
	case CX2ComboTree::CTI_LEFT_X_BLUE:
	case CX2ComboTree::CTI_RIGHT_Z_BLUE:
	case CX2ComboTree::CTI_RIGHT_X_BLUE:
	case CX2ComboTree::CTI_UP_Z_BLUE:
	case CX2ComboTree::CTI_UP_X_BLUE:
	case CX2ComboTree::CTI_DOWN_Z_BLUE:
	case CX2ComboTree::CTI_DOWN_X_BLUE:
		{
			iDummyInt = 1;
		} break;
	case CX2ComboTree::CTI_Z_RED:
	case CX2ComboTree::CTI_X_RED:
	case CX2ComboTree::CTI_Z_CONTINUE_RED:
	case CX2ComboTree::CTI_X_CONTINUE_RED:
	case CX2ComboTree::CTI_LEFT_Z_RED:
	case CX2ComboTree::CTI_LEFT_X_RED:
	case CX2ComboTree::CTI_RIGHT_Z_RED:
	case CX2ComboTree::CTI_RIGHT_X_RED:
	case CX2ComboTree::CTI_UP_Z_RED:
	case CX2ComboTree::CTI_UP_X_RED:
	case CX2ComboTree::CTI_DOWN_Z_RED:
	case CX2ComboTree::CTI_DOWN_X_RED:
		{
			iDummyInt = 0;
		} break;
	case CX2ComboTree::CTI_X_CONTINUE_OVERHEAT:
	case CX2ComboTree::CTI_PUSH_Z_OVERHEAT:
		{
			iDummyInt = 2;
		} break;
	case CX2ComboTree::CTI_X_DRAGON_TOOTH:
		{
			iDummyInt = 3;
		} break;
	case CX2ComboTree::CTI_Z_WOLF_TOOTH:
		{
			iDummyInt = 4;
		} break;
	case CX2ComboTree::CTI_ICON_FIST_NA:
	case CX2ComboTree::CTI_ICON_KICK_NA:
	case CX2ComboTree::CTI_RIGHT_Z_PURPLE:
	case CX2ComboTree::CTI_RIGHT_X_PURPLE:
	case CX2ComboTree::CTI_RIGHT_Z_PURPLE_CONTINUE:
	case CX2ComboTree::CTI_RIGHT_X_PURPLE_CHARGE:
	case CX2ComboTree::CTI_ICON_NA:
		{
			iDummyInt = 5;
			fAddSizeX = 20.f;
			fAddSizeY = 20.f;
		} break;
	}


	if( bCheck == false )
		return;

	CKTDGUIButton* pMouseOverButton = m_hDLGComboTree->CreateButton();
	m_hDLGComboTree->AddControl( pMouseOverButton );

	pMouseOverButton->SetName( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"MouseOverButton" ), m_iNumMouseOverButton ) ) );
	m_iNumMouseOverButton++;

	pMouseOverButton->SetNormalTex( L"DLG_UI_Common_Texture13.tga", L"Black" );
	pMouseOverButton->SetOverTex( L"DLG_UI_Common_Texture13.tga", L"Black" );
	pMouseOverButton->SetDownTex( L"DLG_UI_Common_Texture13.tga", L"Black" );

	CKTDGUIControl::UIPointData* pNormalPointData = new CKTDGUIControl::UIPointData();
	pNormalPointData->leftTopPoint = CalcPicturePosition( eIcon_.m_eComboIcon, eIcon_.m_vLeftTop );
	pNormalPointData->addSize.y = 20.f + fAddSizeY;
	pNormalPointData->addSize.x = 20.f + fAddSizeX;
	pNormalPointData->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

	CKTDGUIControl::UIPointData* pOverPointData = new CKTDGUIControl::UIPointData();
	pOverPointData->leftTopPoint = CalcPicturePosition( eIcon_.m_eComboIcon, eIcon_.m_vLeftTop );
	pOverPointData->addSize.y = 20.f + fAddSizeY;
	pOverPointData->addSize.x = 20.f + fAddSizeX;
	pOverPointData->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

	CKTDGUIControl::UIPointData* pDownPointData = new CKTDGUIControl::UIPointData();
	pDownPointData->leftTopPoint = CalcPicturePosition( eIcon_.m_eComboIcon, eIcon_.m_vLeftTop );
	pDownPointData->addSize.y = 20.f + fAddSizeY;
	pDownPointData->addSize.x = 20.f + fAddSizeX;
	pDownPointData->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

	pMouseOverButton->SetNormalPoint( pNormalPointData );
	pMouseOverButton->SetOverPoint( pOverPointData );
	pMouseOverButton->SetDownPoint( pDownPointData );
	pMouseOverButton->SetCustomMsgMouseOver( CTUCM_COMBO_ICON_MOUSE_OVER );
	pMouseOverButton->SetCustomMsgMouseOut( CTUCM_COMBO_ICON_MOUSE_OUT );
	pMouseOverButton->SetShowEnable( false, false );
	pMouseOverButton->AddDummyInt(iDummyInt);

	if(staticComboIndex_ >= 0 && staticComboIndex_ < 5 )
	{
		m_vecMouseOverButton[staticComboIndex_].push_back(pMouseOverButton);
	}
	return;
}

void CX2UIComboTree::DrawComboTreeDesc( D3DXVECTOR2 vDrawPos_, wstring wstrDesc )
{
	if( m_pDLGComboTreeDesc == NULL)
	{
		m_pDLGComboTreeDesc = new CKTDGUIDialog( g_pMain->GetNowState(), L"", 0.07f, XDL_POP_UP );
		m_pDLGComboTreeDesc->SetFront(true);
		m_pDLGComboTreeDesc->SetCloseOnFocusOut(true);

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGComboTreeDesc );
		m_pDLGComboTreeDesc->SetFront( true );
		m_pDLGComboTreeDesc->SetModal( true );

		CKTDGUIStatic* pStatic = new CKTDGUIStatic();
		pStatic->SetName( L"ItemDesc" );
		m_pDLGComboTreeDesc->AddControl( pStatic );
		CKTDGUIControl::CPictureData* pPicture = new CKTDGUIControl::CPictureData();
		pStatic->AddPicture( pPicture );
		pPicture->SetEdge( false, 2, D3DXCOLOR(1,1,1,1) );
		pPicture->SetTex( L"FadeInOut.dds" );

		CKTDGUIControl::UIStringData* pString = new CKTDGUIControl::UIStringData();
		pStatic->AddString( pString );
		pString->fontIndex = SLOT_MANAGER_FONT_INDEX;
		pString->color = D3DXCOLOR( 1,1,1,1 );
		pString->outlineColor = D3DXCOLOR( 1,1,1,1 );
		pString->msg = L"";
		pString->fontStyle = CKTDGFontManager::FS_NONE;
		pString->sortFlag = DT_LEFT;
		pString->pos = D3DXVECTOR2( 20, 20 );
	}


	int itemDescPosX = (int)vDrawPos_.x;
	int itemDescPosY = (int)vDrawPos_.y;

	m_pDLGComboTreeDesc->SetShow( true );
	CKTDGUIStatic* pItemDesc = (CKTDGUIStatic*)m_pDLGComboTreeDesc->GetControl(L"ItemDesc");

	//const CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pItemDesc->GetString(0)->fontIndex );

	const int MAGIC_ELSWORD_DESC_WIDTH = 330;
	wstrDesc = CWordLineHandler::GetStrByLineBreakColorInX2Main(wstrDesc.c_str(), (int)((float)MAGIC_ELSWORD_DESC_WIDTH*g_pKTDXApp->GetResolutionScaleX()), SLOT_MANAGER_FONT_INDEX);

	CKTDGFontManager::CUKFont* pItemDescFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( SLOT_MANAGER_FONT_INDEX );
	int itemDescWidth = (int)( pItemDescFont->GetWidth( wstrDesc.c_str() ) / g_pKTDXApp->GetResolutionScaleX() ) + ITEM_DESC_ADD_SIZE_X;
	int itemDescHeight = (int)( pItemDescFont->GetHeight( wstrDesc.c_str() ) / g_pKTDXApp->GetResolutionScaleY() ) + ITEM_DESC_ADD_SIZE_Y;


	m_pDLGComboTreeDesc->SetPos( D3DXVECTOR2( (float)itemDescPosX, (float)itemDescPosY ) );
	m_pDLGComboTreeDesc->GetStatic_LUA( "ItemDesc" )->GetPicture(0)->SetSize( D3DXVECTOR2( (float)itemDescWidth, (float)itemDescHeight) );
	m_pDLGComboTreeDesc->GetStatic_LUA( "ItemDesc" )->GetPicture(0)->pPoint->color = D3DXCOLOR( 1,1,1,0.7f );
	m_pDLGComboTreeDesc->GetStatic_LUA( "ItemDesc" )->GetString(0)->msg = wstrDesc.c_str();


	return;
}
