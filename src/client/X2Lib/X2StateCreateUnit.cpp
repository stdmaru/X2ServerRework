#include "stdafx.h"
#include "./X2StateCreateUnit.h"




static const int ELSWORD_PROMOTION_ITEM_ID[]	= { 128000, 128001, 128002, 128003, 128004, };
static const int AISHA_PROMOTION_ITEM_ID[]		= { 128010, 128011, 128012, 128013, 128014, };
static const int RENA_PROMOTION_ITEM_ID[]		= { 128005, 128006, 128007, 128008, 128009, };
static const int RAVEN_PROMOTION_ITEM_ID[]		= { 128072, 128073, 128074, 128075, 128076, };
static const int EVE_PROMOTION_ITEM_ID[]		= { 130134, 130135, 130136, 130137, 130138, };

//{{ kimhc // 2010.12.8 // 2010-12-23 New Character CHUNG
static const int CHUNG_PROMOTION_ITEM_ID[]		= { 133125, 133126, 133127, 133128, 133129, };	// ĳ���� ���� â���� ���� û�� ���θ�� ���� ID
//}} kimhc // 2010.12.8 //  2010-12-23 New Character CHUNG

static const int ARA_PROMOTION_ITEM_ID[]		= { 41460, 41470, 41480, 41490, 41500, };

static const int EL_PROMOTION_ITEM_ID[]			= { 41720, 41721, 41722, 41723, 41724, };

static const int ADD_PROMOTION_ITEM_ID[]		= { 41757, 41758, 41759, 41760, 41761 };


#ifdef FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
const int TODAY_CREATE_UNIT_MAX_NUMBER_SERVER = 50;	// ���� �� �Ϸ翡 ���� ������ ���� �ִ� ����
#else // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
const int TODAY_CREATE_UNIT_MAX_NUMBER_SERVER = 4;	// ���� �� �Ϸ翡 ���� ������ ���� �ִ� ����
#endif // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED

CX2StateCreateUnit::CX2StateCreateUnit(void) :
m_pDLGCreateUnitBack( NULL ),
m_pDLGCreateUnitFront( NULL ),
m_SelectUnitClass( CX2Unit::UC_NONE ),
m_NickName( L"" ),
m_UnitUID( -1 ),
m_pDummyElsword( NULL ),
m_pDummyArme( NULL ),
m_pDummyLire( NULL ),
m_pDummyRaven( NULL ),
m_pDummyEve( NULL ),
//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
m_pDummyChung( NULL ),
//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG
m_pDummyAra( NULL ),
m_pDummyEL ( NULL ) ,
m_pDummyAdd ( NULL ) ,
m_pUnitViewerUI( NULL ),
m_bSelectUnit( false ),
m_fMouseSensitivity( 0.01f ),
m_fRemainRotY( 0.f ),
m_bRotateReturn( true ),
m_fRotSpeed( 0.f ),
m_pDLGMSGOkBoxEnterTutorial( NULL )
, m_SelectUnitType ( CX2Unit::UT_NONE )
, m_pDLGCreateUnitChangeClassInfo ( NULL )
, m_pDLGCreateUnitServerSelect ( NULL )
, m_pDLGCreateUnitCheckInfo ( NULL )
, m_hStandEffect( INVALID_EFFECTSET_HANDLE )
, m_eSelectServerGroupID ( SEnum::SGI_INVALID )
, m_iGaiaServerCreateAbleUnitMany ( 0 )
, m_iSolesServerCreateAbleUnitMany ( 0 )
, m_fFadeTitleControlValue ( 0.f )
, m_sSumDelta( 0 )
, m_sZoomState( 1 )
, m_pCreateUnitSelectSound( NULL )
{
	// variable assign

	m_pDLGCreateUnitBack = new CKTDGUIDialog( this, L"DLG_Unit_Create_Back_New.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCreateUnitBack );

	m_pDLGCreateUnitFront = new CKTDGUIDialog( this, L"DLG_Unit_Create_Front_New.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCreateUnitFront );

	g_pKTDXApp->GetDGManager()->GetCamera().Point( 0, 232.83f, -758.49f, 0, 100.f, 0 );
	g_pMain->SetProjectionFov( 4.f );

	InitDummyUnits();
	CX2UnitViewerUI::SetUnitClicked( false );

	if( NULL != m_pDLGCreateUnitFront->GetControl( L"NoticeNewCharacter") )
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"NoticeNewCharacter") )->SetShowEnable( false, false );

	// �������� Ŭ���̾�Ʈ���� ĳ���� ���´�.
	const CX2Unit::UNIT_TYPE eLimitType = static_cast< CX2Unit::UNIT_TYPE >( _CONST_LIMIT_UNIT_::iLimitType );
	if( eLimitType < CX2Unit::UT_RAVEN && NULL != m_pDLGCreateUnitFront->GetControl( L"Raven_Slot") )
	{
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"Raven_Slot") )->SetShowEnable( false, false );
	}
	if( eLimitType < CX2Unit::UT_EVE && NULL != m_pDLGCreateUnitFront->GetControl( L"Eve_Slot") )
	{
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"Eve_Slot") )->SetShowEnable( false, false );
	}
	if( eLimitType < CX2Unit::UT_CHUNG && NULL != m_pDLGCreateUnitFront->GetControl( L"Chung_Slot") )
	{
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"Chung_Slot") )->SetShowEnable( false, false );
	}
	if( eLimitType < CX2Unit::UT_ARA && NULL != m_pDLGCreateUnitFront->GetControl( L"Ara_Slot") )
	{
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"Ara_Slot") )->SetShowEnable( false, false );
	}
	if( eLimitType < CX2Unit::UT_ELESIS && NULL != m_pDLGCreateUnitFront->GetControl( L"Elesis_Slot") )
	{
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"Elesis_Slot") )->SetShowEnable( false, false );
	}
	if( eLimitType < CX2Unit::UT_ADD && NULL != m_pDLGCreateUnitFront->GetControl( L"AddCreateUnitSlot") )
	{
		static_cast<CKTDGUIRadioButton *>( m_pDLGCreateUnitFront->GetControl( L"AddCreateUnitSlot") )->SetShowEnable( false, false );
	}

	if ( NULL != g_pData->GetMyUser() )
	{
		if ( g_pData->GetMyUser()->GetUnitNum() > 0 )
		{
			CKTDGUIButton* pButtonBackToUnitSelect = static_cast<CKTDGUIButton*> ( m_pDLGCreateUnitFront->GetControl( L"ButtonBack" ) );
			if ( NULL != pButtonBackToUnitSelect )
			{
				pButtonBackToUnitSelect->SetShowEnable( true, true );
			}

			CKTDGUIButton* pButtonEndGame = static_cast<CKTDGUIButton*> ( m_pDLGCreateUnitFront->GetControl( L"ButtonEndGame" ) );
			if ( NULL != pButtonEndGame )
			{
				pButtonEndGame->SetShowEnable( true, true );
			}
		}
		else
		{
			CKTDGUIButton* pButtonBackToUnitSelect = static_cast<CKTDGUIButton*> ( m_pDLGCreateUnitFront->GetControl( L"ButtonBack" ) );
			if ( NULL != pButtonBackToUnitSelect )
			{
				pButtonBackToUnitSelect->SetShowEnable( false, false );
			}

			CKTDGUIButton* pButtonEndGame = static_cast<CKTDGUIButton*> ( m_pDLGCreateUnitFront->GetControl( L"ButtonEndGame" ) );
			if ( NULL != pButtonEndGame )
			{
				pButtonEndGame->SetShowEnable( true, true );
			}
		}
	}
	m_pDLGCreateUnitBack->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
	m_pDLGCreateUnitFront->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
	Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ();

	// ��ĳ���Ͱ� �� ��� �ش� ĳ���͸� �⺻���� ����
	switch( eLimitType )
	{
	case CX2Unit::UT_RAVEN:
		{
			SelectRaven();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Raven_Slot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"Raven_Slot") )->SetChecked( true );
		} break;
	case CX2Unit::UT_EVE:
		{
			SelectEve();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Eve_Slot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"Eve_Slot") )->SetChecked( true );
		} break;
	case CX2Unit::UT_CHUNG:
		{
			SelectChung();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Chung_Slot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"Chung_Slot") )->SetChecked( true );
		} break;
	case CX2Unit::UT_ARA:
		{
			SelectAra();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Ara_Slot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"Ara_Slot") )->SetChecked( true );
		} break;
	case CX2Unit::UT_ELESIS:
		{
			SelectElesis();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ELESIS_Slot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"ELESIS_Slot") )->SetChecked( true );
		} break;
	case CX2Unit::UT_ADD:
		{
			SelectAdd();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"AddCreateUnitSlot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"AddCreateUnitSlot") )->SetChecked( true );
		} break;
	default:
		{
			// ��ĳ���Ͱ� ���ٸ� ���ҵ带 �����ϴ°� ���� �ڿ�������. by ������
			SelectElSword();
			if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Elsword_Slot") )
				static_cast<CKTDGUIRadioButton *> ( m_pDLGCreateUnitFront->GetControl( L"Elsword_Slot") )->SetChecked( true );
		} break;
	}




	g_pKTDXApp->SkipFrame();
}

CX2StateCreateUnit::~CX2StateCreateUnit(void)
{
	SAFE_DELETE_DIALOG( m_pDLGCreateUnitBack );
	SAFE_DELETE_DIALOG( m_pDLGCreateUnitFront );
	SAFE_DELETE_DIALOG( m_pDLGMSGOkBoxEnterTutorial );
	SAFE_DELETE_DIALOG( m_pDLGCreateUnitChangeClassInfo );
	SAFE_DELETE_DIALOG( m_pDLGCreateUnitServerSelect );
	SAFE_DELETE_DIALOG( m_pDLGCreateUnitCheckInfo );
	SAFE_DELETE( m_pDummyElsword );
	SAFE_DELETE( m_pDummyArme );
	SAFE_DELETE( m_pDummyLire );
	SAFE_DELETE( m_pDummyRaven );
	SAFE_DELETE( m_pDummyEve );
	SAFE_DELETE( m_pDummyChung );
	SAFE_DELETE( m_pDummyAra );
	SAFE_DELETE( m_pDummyEL );
	SAFE_DELETE( m_pDummyAdd );

	if ( NULL != g_pData->GetUIMajorXMeshPlayer() )
	{
		for( CKTDGXMeshPlayer::CXMeshInstanceHandle hStandMeshInst : m_vecStandMeshInst )
		{
			g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( hStandMeshInst );
		}
		m_vecStandMeshInst.clear();
	}
	if( NULL != g_pData->GetUIEffectSet() )
	{
		g_pData->GetUIEffectSet()->StopEffectSet( m_hStandEffect );
	}
	CX2UnitViewerUI::DeleteKTDGObject( m_pUnitViewerUI );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
}

HRESULT CX2StateCreateUnit::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2State::OnFrameMove( fTime, fElapsedTime );
	g_pKTDXApp->GetDGManager()->GetCamera().UpdateCamera( fElapsedTime );

	if ( NULL != g_pData->GetPremiumBuffManager() )
	{
		g_pData->GetPremiumBuffManager()->OnFrameMove( fTime, fElapsedTime );
	}

	if( m_pUnitViewerUI != NULL )
	{
		if ( CX2UnitViewerUI::GetUnitClicked() == true )
		{
			D3DXVECTOR3 RotateY = D3DXVECTOR3(0, -((float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXDelta() * m_fMouseSensitivity), 0);
			m_pUnitViewerUI->GetMatrix().RotateRel(RotateY);
			m_bRotateReturn = false;
			m_fRotSpeed = 13.0f;
		}
		else
		{
			float fRotY = g_pData->RotateYSelectCharacterType( m_SelectUnitClass );
			m_pUnitViewerUI->GetMatrix().Rotate( D3DXToRadian(0),D3DXToRadian(fRotY),D3DXToRadian(0) );
			int addSpeed = 20;

			if ( m_bRotateReturn == false )
			{
				m_fRotSpeed -= (fElapsedTime * addSpeed);

				if ( m_fRotSpeed <= 0 )
				{
					m_fRotSpeed = fElapsedTime;
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
						m_fRemainRotY -= (fElapsedTime*m_fRotSpeed);
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
						m_fRemainRotY += (fElapsedTime*m_fRotSpeed);
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
						m_fRemainRotY += (fElapsedTime*m_fRotSpeed);
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
						m_fRemainRotY -= (fElapsedTime*m_fRotSpeed);
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

		if ( m_pUnitViewerUI->GetXSkinAnim()->GetState() == CKTDGXSkinAnim::XAS_WAIT )
		{
			//m_pUnitViewerUI->PlayAnim( L"LobbyWait", CKTDGXSkinAnim::XAP_LOOP, true );
			//m_SelectUnitClass
			m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_WAIT );
		}

		m_pUnitViewerUI->OnFrameMove( fTime, fElapsedTime );
	}

	bool bCheckHaveFocus = false;

	if ( NULL != m_pDLGCreateUnitServerSelect )
	{
		CKTDGUIControl* pControl = m_pDLGCreateUnitServerSelect->GetControl(L"IMEEditBoxUnitCreateNickName");
		if ( pControl != NULL )
		{
			if ( pControl->GetHaveFocusIn() == true )
			{
				bCheckHaveFocus = true;
			}
		}
	}

	if ( NULL != m_pDLGCreateUnitServerSelect )
	{
		CKTDGUIControl* pControl = m_pDLGCreateUnitServerSelect->GetControl(L"StaticInputNicknameNotice");
		CKTDGUIIMEEditBox * pIMEEditBox = static_cast <CKTDGUIIMEEditBox *> ( m_pDLGCreateUnitServerSelect->GetControl(L"IMEEditBoxUnitCreateNickName") );

		if ( pControl != NULL && pIMEEditBox != NULL )
		{
			if ( bCheckHaveFocus == true || pIMEEditBox->GetTextLength() > 0 )
			{
				pControl->SetShow ( false );
			}
			else
			{
				pControl->SetShow ( true );
			}
		}
	}

	// The daily creation notice, under the character, was a picture of "Creative_Character" (the panel on the right);
	// it is a static of its own in the client_data so that it stays centered (T44), shown with the panel.
	if ( NULL != m_pDLGCreateUnitFront )
	{
		CKTDGUIControl* pPanel = m_pDLGCreateUnitFront->GetControl( L"Creative_Character" );
		CKTDGUIControl* pNotice = m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Notice" );
		if ( NULL != pPanel && NULL != pNotice && pNotice->GetShow() != pPanel->GetShow() )
			pNotice->SetShow( pPanel->GetShow() );
	}

	if ( NULL != m_pDLGCreateUnitFront && NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
	{
		if ( true == m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->GetShow () )
		{
			m_fFadeTitleControlValue += fElapsedTime / 3;

			if ( m_fFadeTitleControlValue > 2.f )
			{
				m_fFadeTitleControlValue = 0.f;
			}
			float m_fFadeTitlePercentage = m_fFadeTitleControlValue;
			if ( m_fFadeTitleControlValue > 1.f )
			{
				m_fFadeTitlePercentage = 2.f - m_fFadeTitleControlValue;
			}
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetColor( D3DXCOLOR ( 1.f, 1.f, 1.f, m_fFadeTitlePercentage ) );
		}
		else
		{
			m_fFadeTitleControlValue = 0.f;
		}
	}

	return S_OK;
}

bool CX2StateCreateUnit::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_LBUTTONUP:
			{
				if ( CX2UnitViewerUI::GetUnitClicked() == true && m_pUnitViewerUI != NULL )
				{
					D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();
					int degree = (int)D3DXToDegree( rot.y );
					m_fRemainRotY = D3DXToRadian( degree % 360 );
				}
				CX2UnitViewerUI::SetUnitClicked( false );
			}
			break;

		case WM_MOUSEWHEEL:
			{
				short zDelta = static_cast<short>(HIWORD(wParam));
				m_sSumDelta += zDelta;
				while ( abs( m_sSumDelta ) >= WHEEL_DELTA )
				{
					if( m_sSumDelta > 0 )
					{
						m_sSumDelta -= WHEEL_DELTA;
						if ( m_sZoomState < 3 )
						{
							D3DXVECTOR3 vMove = g_pKTDXApp->GetDGManager()->GetCamera().GetLookVec() - g_pKTDXApp->GetDGManager()->GetCamera().GetEye();
							D3DXVec3Normalize( &vMove, &vMove );
							g_pKTDXApp->GetDGManager()->GetCamera().MoveRel( vMove * 100.f );
							++m_sZoomState;
						}
					}
					else
					{
						m_sSumDelta += WHEEL_DELTA;

						if ( m_sZoomState > 1 )
						{
							D3DXVECTOR3 vMove = g_pKTDXApp->GetDGManager()->GetCamera().GetLookVec() - g_pKTDXApp->GetDGManager()->GetCamera().GetEye();
							D3DXVec3Normalize( &vMove, &vMove );
							g_pKTDXApp->GetDGManager()->GetCamera().MoveRel( vMove * -100.f );
							--m_sZoomState;
						}
					}
				}
			} break;
	}

	return CX2State::MsgProc( hWnd, uMsg, wParam, lParam );
}

bool CX2StateCreateUnit::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2State::UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	if ( NULL != g_pData->GetPremiumBuffManager() )
	{
		if ( true == g_pData->GetPremiumBuffManager()->UICustomEventProc(hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if( NULL != g_pMain->GetSecurityPad() )
	{
		if( true == g_pMain->GetSecurityPad()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}


	switch(wParam)
	{
		case SCUUCM_ELSWORD_BUTTON_UP:
			{
				SelectElSword();
			} break;

		case SCUUCM_LIRE_BUTTON_UP:
			{
				SelectLire();
			} break;

		case SCUUCM_ARME_BUTTON_UP:
			{
				SelectArme();
			} break;

		case SCUUCM_RAVEN_BUTTON_UP:
			{
				SelectRaven();
			} break;

		case SCUUCM_EVE_BUTTON_UP:
			{
				SelectEve();
			} break;

		//{{ kimhc // 2010.11.29 // 2010-12-23 New Character CHUNG
		case SCUUCM_CHUNG_BUTTON_UP:
			{
				SelectChung();
			} break;
		//}} kimhc // 2010.11.29 //  2010-12-23 New Character CHUNG

		case SCUUCM_ARA_BUTTON_UP:
			{
				SelectAra();
			} break;
		case SCUUCM_CREATE_UNIT:
			{
				if ( m_bSelectUnit == true && m_eSelectServerGroupID > SEnum::SGI_INVALID )
					Handler_EGS_CREATE_UNIT_REQ();
				else
				{
					if ( false == m_bSelectUnit )
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_559 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ),  L"UI_PopUp_Negative_01.ogg" );
					else if ( m_eSelectServerGroupID == SEnum::SGI_INVALID )
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_700 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Negative_01.ogg" );
				}
			} break;

		case SCUUCM_BACK_TO_UNIT_SELECT:
			{
				//{{ kimhc // 2009-12-23 // ������ �߰��� ������ ���� ��� ä�� ����â���� �̵� �Ѵٴ� �˾�
				if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetUnitNum() == 0 )
					g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_4837 )
						, L"UI_PopUp_Positive_01.ogg"
					);
				else
				{
				//}} kimhc // 2009-12-23 // ������ �߰��� ������ ���� ��� ä�� ����â���� �̵� �Ѵٴ� �˾�
					g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_552 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg" );

					if( m_pCreateUnitSelectSound != NULL )
					{
						m_pCreateUnitSelectSound->Stop();
						SAFE_CLOSE( m_pCreateUnitSelectSound );
					}
				}
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/CX2Main::XS_SERVER_SELECT, 0, false );
			}
			break;

		case SCUUCM_UNIT_ANIMATION_DOWN:
			CX2UnitViewerUI::SetUnitClicked( true );
			break;

		case SCUUCM_UNIT_ANIMATION_UP:
			CX2UnitViewerUI::SetUnitClicked( false );
			break;

		case SCUUCM_UNIT_ANIMATION_OVER:
			break;

		case SCUUCM_ENTER_TUTORIAL_OK:
			{
				g_pMain->SetIsPlayingTutorial( true );
				if( m_pDLGMSGOkBoxEnterTutorial != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMSGOkBoxEnterTutorial, 0, false );
					m_pDLGMSGOkBoxEnterTutorial = NULL;
				}
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_552 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );

				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/CX2Main::XS_SERVER_SELECT, 0, false );
			}
			break;

		case SCUUCM_ENTER_TUTORIAL_CANCEL:
			{
				g_pMain->SetIsPlayingTutorial( false );
				if( m_pDLGMSGOkBoxEnterTutorial != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMSGOkBoxEnterTutorial, 0, false );
					m_pDLGMSGOkBoxEnterTutorial = NULL;
				}


				CX2StateAutoChanger::TARGET_DETAIL targetDetail;
				g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), CX2Main::XS_VILLAGE_MAP, targetDetail, m_UnitUID );

			} break;

		case SCUUCM_SHUT_DOWN_OK:
			{
				CX2State::QuitGame();
			} break;
		case SCUUCM_ELESIS_BUTTON_UP:
			{
				SelectElesis();
			} break;
		case SCUUCM_ADD_BUTTON_UP :
			{
				SelectAdd();
			} break;
		case SCUUCM_FIRST_LINE_FIRST_CLASS_CHANGE_BUTTON_UP :
			{
				SelectUpperClass ( m_SelectUnitType, CX2Unit::UCT_FIRST_CLASS, CX2Unit::UCL_FIRST );
			}
			break;
		case SCUUCM_SECOND_LINE_FIRST_CLASS_CHANGE_BUTTON_UP :
			{
				SelectUpperClass ( m_SelectUnitType, CX2Unit::UCT_FIRST_CLASS, CX2Unit::UCL_SECOND );
			}
			break;
		case SCUUCM_THIRD_LINE_FIRST_CLASS_CHANGE_BUTTON_UP :
			{
				SelectUpperClass ( m_SelectUnitType, CX2Unit::UCT_FIRST_CLASS, CX2Unit::UCL_THIRD );
			}
			break;
		case SCUUCM_FIRST_LINE_SECOND_CLASS_CHANGE_BUTTON_UP :
			{
				SelectUpperClass ( m_SelectUnitType, CX2Unit::UCT_SECOND_CLASS, CX2Unit::UCL_FIRST );
			}
			break;
		case SCUUCM_SECOND_LINE_SECOND_CLASS_CHANGE_BUTTON_UP :
			{
				SelectUpperClass ( m_SelectUnitType, CX2Unit::UCT_SECOND_CLASS, CX2Unit::UCL_SECOND );
			}
			break;
		case SCUUCM_THIRD_LINE_SECOND_CLASS_CHANGE_BUTTON_UP :
			{
				SelectUpperClass ( m_SelectUnitType, CX2Unit::UCT_SECOND_CLASS, CX2Unit::UCL_THIRD );
			}
			break;

		case SCUUCM_CREATE_UNIT_SERVER_SELECT_STEP_BUTTON_UP :
			{
				if ( m_SelectUnitClass != CX2Unit::UC_NONE )
				{
					SAFE_DELETE_DIALOG ( m_pDLGCreateUnitServerSelect );
					CX2Unit::UNIT_TYPE eSelectUnitType = m_SelectUnitType;
					UnSelect();

					switch(eSelectUnitType)
					{
					case CX2Unit::UT_ELSWORD:
						{
							SelectElSword();
						} break;
					case CX2Unit::UT_ARME:
						{
							SelectArme();
						} break;
					case CX2Unit::UT_LIRE:
						{
							SelectLire();
						} break;
					case CX2Unit::UT_RAVEN:
						{
							SelectRaven();
						} break;
					case CX2Unit::UT_EVE:
						{
							SelectEve();
						} break;
					case CX2Unit::UT_CHUNG:
						{
							SelectChung();
						} break;
					case CX2Unit::UT_ARA:
						{
							SelectAra();
						} break;
					case CX2Unit::UT_ELESIS:
						{
							SelectElesis();
						} break;
					case CX2Unit::UT_ADD:
						{
							SelectAdd();
						} break;

					default:
						{
							SelectElSword();
						} break;
					}
					if( m_pCreateUnitSelectSound != NULL )
					{
						m_pCreateUnitSelectSound->Stop();
						SAFE_CLOSE( m_pCreateUnitSelectSound );
					}
					m_pDLGCreateUnitServerSelect = new CKTDGUIDialog( this, L"DLG_Unit_Create_Front_Server_Select_New.lua" );
					g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCreateUnitServerSelect );
					if ( NULL != m_pDLGCreateUnitServerSelect )
					{
						CKTDGUIStatic * pStaticCreateGaiaMany = static_cast<CKTDGUIStatic *> ( m_pDLGCreateUnitServerSelect->GetControl( L"StaticGaiaCreateMany" ) );

						if ( NULL != pStaticCreateGaiaMany )
						{
							wstringstream wstrstreamCreateGaiaMany;
							int iNowUserUnit = g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_GAIA );
							int iMaxUserUnit = g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_GAIA );
							wstrstreamCreateGaiaMany << GET_STRING(STR_ID_29400) <<	L" " << iNowUserUnit << " / " << iMaxUserUnit;
							if( iNowUserUnit == iMaxUserUnit )
							{
								if( iMaxUserUnit < 30 )
									wstrstreamCreateGaiaMany << L"   " << GET_STRING(STR_ID_29413);
								else
									wstrstreamCreateGaiaMany << L"   " << GET_STRING(STR_ID_29414);
							}
						}

						CKTDGUIStatic * pStaticCreateSolesMany = static_cast<CKTDGUIStatic *> ( m_pDLGCreateUnitServerSelect->GetControl( L"StaticSolesCreateMany" ) );
						if ( NULL != pStaticCreateSolesMany )
						{
							wstringstream wstrstreamCreateSolesMany;
							int iNowUserUnit = g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_SOLES );
							int iMaxUserUnit = g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_SOLES );
							wstrstreamCreateSolesMany << GET_STRING(STR_ID_24417) << GET_STRING(STR_ID_29400) << L" " <<  iNowUserUnit << " / " << iMaxUserUnit;
							if( iNowUserUnit == iMaxUserUnit )
							{
								if( iMaxUserUnit < 30 )
									wstrstreamCreateSolesMany << L"   " << GET_STRING(STR_ID_29413);
								else
									wstrstreamCreateSolesMany << L"   " << GET_STRING(STR_ID_29414);
							}
							pStaticCreateSolesMany->GetString( 0 )->msg = wstrstreamCreateSolesMany.str().c_str();
						}

					}
					if ( NULL != m_pDLGCreateUnitServerSelect->GetControl( L"ButtonGaiaServer" ) && NULL != m_pDLGCreateUnitServerSelect->GetControl( L"ButtonSolesServer" )  )
					{
						bool bIsCreateAbleSolesServer = true;

						if ( g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_GAIA ) >= g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_GAIA ) ||
							 m_iGaiaServerCreateAbleUnitMany >= TODAY_CREATE_UNIT_MAX_NUMBER_SERVER )
						{
							static_cast <CKTDGUIRadioButton *> ( m_pDLGCreateUnitServerSelect->GetControl( L"ButtonGaiaServer" ) )->SetEnable( false );
						}

						if ( g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_SOLES ) >= g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_SOLES ) ||
							m_iSolesServerCreateAbleUnitMany >= TODAY_CREATE_UNIT_MAX_NUMBER_SERVER )
						{
							static_cast <CKTDGUIRadioButton *> ( m_pDLGCreateUnitServerSelect->GetControl( L"ButtonSolesServer" ) )->SetEnable( false );
							bIsCreateAbleSolesServer = false;
						}
						static_cast <CKTDGUIRadioButton *> ( m_pDLGCreateUnitServerSelect->GetControl( L"ButtonGaiaServer" ) )->SetShowEnable( false, false );
						static_cast <CKTDGUIRadioButton *> ( m_pDLGCreateUnitServerSelect->GetControl( L"ButtonSolesServer" ) )->SetShowEnable( false, false );

						if( bIsCreateAbleSolesServer == true )
						{
							m_eSelectServerGroupID = SEnum::SGI_SOLES;
						}
						else
						{
							m_eSelectServerGroupID = SEnum::SGI_INVALID;
						}
					}
//					m_pDLGCreateUnitServerSelect = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(0,0), L"", SCUUCM_NICKNAME_CHECK, this, -1, L"DLG_Unit_Create_Front_Server_Select_New.lua", D3DXVECTOR2( 0, 0 ), L"UI_PopUp_Positive_01.ogg" );
				}
				else
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING(STR_ID_29401), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Negative_01.ogg" );
				}
			}
			break;

		case SCUUCM_SELECT_GAIA_SERVER :
			{
				m_eSelectServerGroupID = SEnum::SGI_GAIA;
			}
			break;

		case SCUUCM_SELECT_SOLES_SERVER :
			{
				m_eSelectServerGroupID = SEnum::SGI_SOLES;
			}
			break;

		case SCUUCM_PASS_NICKNAME_CHECK_OK :
			{
				CX2Unit* pUnit = new CX2Unit( m_kUnitInfo );

				pUnit->SetOwnerUserUID( g_pData->GetMyUser()->GetUID() );
				g_pData->GetMyUser()->AddUnit( pUnit );
				m_UnitUID = pUnit->GetUID();
				g_pMain->SetIsPlayingTutorial( true );
				if( m_pDLGMSGOkBoxEnterTutorial != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMSGOkBoxEnterTutorial, 0, false );
					m_pDLGMSGOkBoxEnterTutorial = NULL;
				}
				if ( m_pDLGCreateUnitCheckInfo != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCreateUnitCheckInfo, 0, false );
					m_pDLGCreateUnitCheckInfo = NULL;
				}

				if ( m_pDLGCreateUnitServerSelect != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCreateUnitServerSelect, 0, false );
					m_pDLGCreateUnitServerSelect = NULL;
				}
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_552 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/CX2Main::XS_SERVER_SELECT, 0, false );
			}
			break;

		case SCUUCM_PASS_NICKNAME_CHECK_CANCEL :
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCreateUnitCheckInfo, 0, false );
			break;

		case SCUUCM_NICKNAME_CHECK :
			if ( m_eSelectServerGroupID == SEnum::SGI_INVALID )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_30413 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Negative_01.ogg" );

			}
			else
			{

				CKTDGUIIMEEditBox* pIDEditBox = reinterpret_cast<CKTDGUIIMEEditBox*> ( m_pDLGCreateUnitServerSelect->GetControl( L"IMEEditBoxUnitCreateNickName" ) );
				if ( NULL != pIDEditBox )
				{
					wstring wstrCreateUnitNickname = pIDEditBox->GetText();

					KEGS_ENTRY_POINT_CHECK_NICK_NAME_REQ kPacket;
					kPacket.m_iServerGroup = m_eSelectServerGroupID;
					kPacket.m_wstrNickName = wstrCreateUnitNickname;



					m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING(STR_ID_29402), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Positive_01.ogg" );

					if ( g_pInstanceData->GetNowUserUnitManyInServer( m_eSelectServerGroupID ) >= g_pInstanceData->GetMaxUserUnitManyInServer( m_eSelectServerGroupID ) )
					{
						SAFE_DELETE_DIALOG ( m_pDLGMsgBox );
						m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING ( STR_ID_29886 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Positive_01.ogg" );
					}

					g_pData->GetServerProtocol()->SendPacket( EGS_ENTRY_POINT_CHECK_NICK_NAME_REQ, kPacket );
					g_pMain->AddServerPacket( EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK );
				}
			}
			break;
	}

	return false;
}

bool CX2StateCreateUnit::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2State::UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	if( g_pMain->GetSecurityPad() != NULL )
	{
		if( true == g_pMain->GetSecurityPad()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	switch( wParam )
	{
		case EGS_CREATE_UNIT_ACK:
			return Handler_EGS_CREATE_UNIT_ACK( hWnd, uMsg, wParam, lParam );
			break;

		//case EGS_GET_SKILL_ACK:
		//	//return Handler_EGS_GET_SKILL_ACK( hWnd, uMsg, wParam, lParam );
		//	break;

		//case EGS_CHANGE_SKILL_SLOT_ACK:
		//	//return Handler_EGS_CHANGE_SKILL_SLOT_ACK( hWnd, uMsg, wParam, lParam );
		//	break;

		case EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK :
			return Handler_EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK( hWnd, uMsg, wParam, lParam );

		case EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK :
			return Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK( hWnd, uMsg, wParam, lParam );
	}
	return false;
}





void CX2StateCreateUnit::EquipPromotionItem( CX2Unit::UNIT_CLASS eUnitClass )
{
	switch( eUnitClass )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
		{
			for( int i=0; i<std::ssize(ELSWORD_PROMOTION_ITEM_ID); i++ )
			{
				m_pUnitViewerUI->AddEquipByTID( ELSWORD_PROMOTION_ITEM_ID[i] );
			}
		} break;

	case CX2Unit::UC_ARME_VIOLET_MAGE:
		{
			for( int i=0; i<std::ssize(AISHA_PROMOTION_ITEM_ID); i++ )
			{
				m_pUnitViewerUI->AddEquipByTID( AISHA_PROMOTION_ITEM_ID[i] );
			}
		} break;

	case CX2Unit::UC_LIRE_ELVEN_RANGER:
		{
			for( int i=0; i<std::ssize(RENA_PROMOTION_ITEM_ID); i++ )
			{
				m_pUnitViewerUI->AddEquipByTID( RENA_PROMOTION_ITEM_ID[i] );
			}
		} break;

	case CX2Unit::UC_RAVEN_FIGHTER:
		{
			for( int i=0; i<std::ssize(RAVEN_PROMOTION_ITEM_ID); i++ )
			{
				m_pUnitViewerUI->AddEquipByTID( RAVEN_PROMOTION_ITEM_ID[i] );
			}
		} break;

	case CX2Unit::UC_EVE_NASOD:
		{
			for( int i=0; i<std::ssize(EVE_PROMOTION_ITEM_ID); i++ )
			{
				m_pUnitViewerUI->AddEquipByTID( EVE_PROMOTION_ITEM_ID[i] );
			}
		} break;

//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
	case CX2Unit::UC_CHUNG_IRON_CANNON:
		{
			for( int i=0; i<std::ssize(CHUNG_PROMOTION_ITEM_ID); i++ )
			{
				m_pUnitViewerUI->AddEquipByTID( CHUNG_PROMOTION_ITEM_ID[i] );
			}
		} break;
//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

	case CX2Unit::UC_ARA_MARTIAL_ARTIST:
		{
			for( int i = 0; i < std::ssize( ARA_PROMOTION_ITEM_ID ); ++i )
			{
				m_pUnitViewerUI->AddEquipByTID( ARA_PROMOTION_ITEM_ID[i] );
			}
		} break;
	case CX2Unit::UC_ELESIS_KNIGHT:
		{
			for( int i = 0; i < std::ssize( EL_PROMOTION_ITEM_ID ); ++i )
			{
				m_pUnitViewerUI->AddEquipByTID( EL_PROMOTION_ITEM_ID[i] );
			}
		} break;
	case CX2Unit::UC_ADD_NASOD_RULER:
		{
			for( int i = 0; i < std::ssize( ADD_PROMOTION_ITEM_ID ); ++i )
			{
				m_pUnitViewerUI->AddEquipByTID( ADD_PROMOTION_ITEM_ID[i] );
			}
		} break;
	default:
		break;
	}

}

void CX2StateCreateUnit::UnSelect()
{
	CX2UnitViewerUI::DeleteKTDGObject( m_pUnitViewerUI );
	m_pUnitViewerUI = NULL;

	m_SelectUnitClass = CX2Unit::UC_NONE;
	m_bSelectUnit = false;
	m_SelectUnitType = CX2Unit::UT_NONE;
}

void CX2StateCreateUnit::SelectElSword()
{
	if( m_SelectUnitClass == CX2Unit::UC_ELSWORD_SWORDMAN )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";

	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_ELSWORD ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_elsword.tga" );
		}
	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}

		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}
	}
	m_pDummyElsword->ResetUnitClass ( CX2Unit::UC_ELSWORD_SWORDMAN );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( m_pDummyElsword, CX2UnitViewerUI::UVS_CREATEUNIT );

	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_ELSWORD_SWORDMAN;
	m_SelectUnitType = CX2Unit::UT_ELSWORD;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_ELSWORD );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_ELSWORD );

	//m_pUnitViewerUI->RemoveAllEquips();
	EquipPromotionItem( m_SelectUnitClass );
	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyElsword->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_ELSWORD, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"ElswordVoice_Introduce.ogg", false, false );
}

void CX2StateCreateUnit::SelectArme()
{
	if( m_SelectUnitClass == CX2Unit::UC_ARME_VIOLET_MAGE )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_ARME ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_aisha.tga" );
		}

	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}

		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}
	}
	m_pDummyArme->ResetUnitClass ( CX2Unit::UC_ARME_VIOLET_MAGE );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( m_pDummyArme );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_ARME_VIOLET_MAGE;
	m_SelectUnitType = CX2Unit::UT_ARME;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_ARME );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_ARME );

	//m_pUnitViewerUI->RemoveAllEquips();
	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );
	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyArme->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_ARME, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"AishaVoice_Introduce.ogg", false, false );
}

void CX2StateCreateUnit::SelectLire()
{
	if( m_SelectUnitClass == CX2Unit::UC_LIRE_ELVEN_RANGER )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_LIRE ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_rena.tga" );
		}

	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}
	}
	m_pDummyLire->ResetUnitClass ( CX2Unit::UC_LIRE_ELVEN_RANGER );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( m_pDummyLire );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_LIRE_ELVEN_RANGER;
	//m_pUnitViewerUI->RemoveAllEquips();

	m_SelectUnitType = CX2Unit::UT_LIRE;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_LIRE );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_LIRE );
	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );
	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyLire->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_LIRE, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"RenaVoice_Introduce.ogg", false, false );
}

void CX2StateCreateUnit::SelectRaven()
{
	if( m_SelectUnitClass == CX2Unit::UC_RAVEN_FIGHTER )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_RAVEN ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_raven.tga" );
		}

	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}
	}
	m_pDummyRaven->ResetUnitClass ( CX2Unit::UC_RAVEN_FIGHTER );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
	m_pUnitViewerUI->SetUnit( m_pDummyRaven );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_RAVEN_FIGHTER;
	m_SelectUnitType = CX2Unit::UT_RAVEN;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_RAVEN );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_RAVEN );

	//m_pUnitViewerUI->RemoveAllEquips();
	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyRaven->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_RAVEN, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"RavenVoice_Introduce.ogg", false, false );
}

void CX2StateCreateUnit::SelectEve()
{
	if( m_SelectUnitClass == CX2Unit::UC_EVE_NASOD )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_EVE ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_eve.tga" );
		}
	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}
	}
	m_pDummyEve->ResetUnitClass ( CX2Unit::UC_EVE_NASOD );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( m_pDummyEve );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_EVE_NASOD;
	m_SelectUnitType = CX2Unit::UT_EVE;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_EVE );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_EVE );

	//m_pUnitViewerUI->RemoveAllEquips();
	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyEve->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_EVE, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"EveVoice_Introduce.ogg", false, false );
}

//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
/** @function : SelectChung
	@brief : û�� �������� �� �����ϴ� �Լ��� �г��ӹڽ�, ĳ���� UnitViewer ���� �����ش�.
*/
void CX2StateCreateUnit::SelectChung()
{
	if( m_SelectUnitClass == CX2Unit::UC_CHUNG_IRON_CANNON )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_CHUNG ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_chung.tga" );
		}
	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}

	}
	m_pDummyChung->ResetUnitClass ( CX2Unit::UC_CHUNG_IRON_CANNON );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
	m_pUnitViewerUI->SetUnit( m_pDummyChung );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_CHUNG_IRON_CANNON;
	m_SelectUnitType = CX2Unit::UT_CHUNG;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_CHUNG );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_CHUNG );
	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );


	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyChung->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_CHUNG, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"ChungVoice_Introduce.ogg", false, false );
}

//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

//#pragma region SelectAra
/** @function : SelectAra
	@brief : "�ƶ�"�� �������� �� �����ϴ� �Լ��� �г��ӹڽ�, ĳ���� UnitViewer ���� ������
*/
void CX2StateCreateUnit::SelectAra()
{

	if( m_SelectUnitClass == CX2Unit::UC_ARA_MARTIAL_ARTIST )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_ARA ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_ara.tga" );
		}

	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}


	}
	m_pDummyAra->ResetUnitClass ( CX2Unit::UC_ARA_MARTIAL_ARTIST );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( m_pDummyAra );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_ARA_MARTIAL_ARTIST;
	m_SelectUnitType = CX2Unit::UT_ARA;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_ARA );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_ARA );

	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );


	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyAra->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_ARA, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"AraVoice_Introduce.ogg", false, false );
}
//#pragma endregion �Լ�

void CX2StateCreateUnit::SelectElesis()
{
	if( m_SelectUnitClass == CX2Unit::UC_ELESIS_KNIGHT )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_ELESIS ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_elesis.tga" );
		}

	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}

	}
	m_pDummyEL->ResetUnitClass ( CX2Unit::UC_ELESIS_KNIGHT );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
	m_pUnitViewerUI->SetUnit( m_pDummyEL );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_ELESIS_KNIGHT;
	m_SelectUnitType = CX2Unit::UT_ELESIS;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_ELESIS );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_ELESIS );

	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );


	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyEL->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_ELESIS, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"ElesisVoice_Introduce.ogg", false, false );
}

void CX2StateCreateUnit::SelectAdd()
{
	if( m_SelectUnitClass == CX2Unit::UC_ADD_NASOD_RULER )
		return;

	UnSelect();

	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	m_wstrIntroMovieName = L"";
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}
	}
	if ( NULL != m_pDLGCreateUnitBack )
	{
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "SideFlag" ) )
		{
			SetRightSideFlagColor ( GetUnitKeyColorByUnitType( CX2Unit::UT_ADD ) );
		}
		if ( NULL != m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" ) )
		{
			m_pDLGCreateUnitBack->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background_add.tga" );
		}
	}
	if ( NULL != m_pDLGCreateUnitFront )
	{
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character" )->SetShow( true );
		}
		if ( NULL != m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" ) )
		{
			m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center" )->SetShow( true );
		}

	}
	m_pDummyAdd->ResetUnitClass ( CX2Unit::UC_ADD_NASOD_RULER );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
	m_pUnitViewerUI->SetUnit( m_pDummyAdd );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) );
	m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
	m_pUnitViewerUI->PlayAnim( L"LobbyDownLanding", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_SelectUnitClass = CX2Unit::UC_ADD_NASOD_RULER;
	m_SelectUnitType = CX2Unit::UT_ADD;
	ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UT_ADD );
	ChangeCreateUnitStandMeshInstance ( CX2Unit::UT_ADD );

	EquipPromotionItem( m_SelectUnitClass );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
	if ( NULL != pMovieStatic )
	{
		CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie ( 0 );
		if ( NULL != pIntroMovieData )
		{
			const CX2Unit::UnitTemplet *pTemplet = m_pDummyAdd->GetUnitTemplet();
			if ( NULL != pTemplet )
			{
				pIntroMovieData->SetMovieFileName ( pTemplet->m_IntroMovieFileName.c_str() );
				pIntroMovieData->SetShow ( true );
				pIntroMovieData->SetLoop( true );
			}
		}
		pMovieStatic->SetShow ( true );
	}
	ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UT_ADD, CX2Unit::UCT_BASIC_CLASS, CX2Unit::UCL_FIRST );

	if( m_pCreateUnitSelectSound != NULL )
	{
		m_pCreateUnitSelectSound->Stop();
		SAFE_CLOSE( m_pCreateUnitSelectSound );
	}
	m_pCreateUnitSelectSound = g_pKTDXApp->GetDeviceManager()->PlaySound(L"AddVoice_Introduce.ogg", false, false );
}

bool CX2StateCreateUnit::Handler_EGS_CREATE_UNIT_REQ()
{
	CKTDGUIIMEEditBox* pIDEditBox = reinterpret_cast<CKTDGUIIMEEditBox*> ( m_pDLGCreateUnitServerSelect->GetControl( L"IMEEditBoxUnitCreateNickName" ) );
	m_NickName = pIDEditBox->GetText();

	if( CreateErrorCheck() == false )
		return true;

	KEGS_CREATE_NEW_UNIT_REQ kPacket;
	kPacket.m_iServerGroup		= m_eSelectServerGroupID;
	kPacket.m_iClass			= m_SelectUnitType;
	kPacket.m_wstrNickName		= m_NickName;
	g_pData->GetServerProtocol()->SendPacket( EGS_CREATE_NEW_UNIT_REQ, kPacket );

	g_pMain->AddServerPacket( EGS_CREATE_UNIT_ACK );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_555 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Positive_01.ogg" );

	return true;
}

bool CX2StateCreateUnit::Handler_EGS_CREATE_UNIT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CREATE_UNIT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
	CX2PacketLog::PrintLog( &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_CREATE_UNIT_ACK ) == true )
	{
		bool bCheck = false;

		//{{ 2009.01.19 ���¿� : �ڵ����� elseif -> switch
		switch(kEvent.m_iOK)
		{
		case NetError::ERR_CREATE_UNIT_01:
			{
				SetAlarmAboutName( GET_STRING( STR_ID_556 ) );
				bCheck = true;
			} break;
		case NetError::ERR_CREATE_UNIT_04:
			{

				SetAlarmAboutName( GET_STRING( STR_ID_557 ) );
				bCheck = true;
			} break;
		case NetError::ERR_CREATE_UNIT_06:
			{
				wstringstream wstrstm;
                wstrstm << GET_REPLACED_STRING( ( STR_ID_558, "L", kEvent.m_wstrEnableDate ) );

				SetAlarmAboutName( wstrstm.str().c_str() );
				bCheck = true;
			} break;
		case NetError::ERR_CREATE_UNIT_08:
			{
				SetAlarmAboutName( GET_STRING( STR_ID_362 ) );
				bCheck = true;
			} break;
		default:
			break;
		}//}}


		if ( bCheck == true )
		{
			wstring wstrName( L"Emotion_No" );
			m_pUnitViewerUI->PlayEmotion( CX2Unit::ET_NO, wstrName );

			return true;
		}

		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			//{{ 2009. 7. 6  ������		������� �ʴ� ����̹Ƿ� �ּ�ó���Ͽ����ϴ�~
			// ó�� ĳ���͸� �����Ѱ����� �˻�
			//if(kEvent.m_bIsFirstUnit == true)
			//{
			//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_2633 ), g_pMain->GetNowState() );
			//}
			//}}

			//���� ���� �޾ƿ���, �κ��丮 �޾ƿ���, ���� ����Ʈ�� �̵��ϱ�

			KUnitInfo kUnitInfo = kEvent.m_kUnitInfo;
			CX2Unit* pUnit = new CX2Unit( kUnitInfo );

			if( NULL != pUnit )
			{
				pUnit->SetOwnerUserUID( g_pData->GetMyUser()->GetUID() );
				pUnit->SetServerGroupID ( m_eSelectServerGroupID );
				g_pData->GetMyUser()->AddUnit( pUnit );
				m_UnitUID = pUnit->GetUID();

				PlaySoundCreateUnit( pUnit->GetType() );

			}

			SAFE_DELETE_DIALOG( m_pDLGMSGOkBoxEnterTutorial );

			//m_pDLGMSGOkBoxEnterTutorial = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250, 300), L"Ʃ�丮���� �����Ͻðڽ��ϱ�?",
			//	SCUUCM_ENTER_TUTORIAL_OK, this, SCUUCM_ENTER_TUTORIAL_CANCEL );

			{

				g_pMain->SetIsPlayingTutorial( true );

				if( m_pDLGMSGOkBoxEnterTutorial != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMSGOkBoxEnterTutorial, 0, false );
					m_pDLGMSGOkBoxEnterTutorial = NULL;
				}
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_552 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );

				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/CX2Main::XS_SERVER_SELECT, 0, false );

			}
		}

	}
	return true;
}

bool CX2StateCreateUnit::CreateErrorCheck()
{
	string multyByteString = "";
	ConvertWCHARToChar( multyByteString, m_NickName.c_str() );

	wstring wstrErrorMessage = L"";



	if ( m_SelectUnitClass == CX2Unit::UC_NONE )
	{
		SetAlarmAboutName(  GET_STRING( STR_ID_559 ) );
		return false;
	}
	else if( true == CX2Main::IsInvalidNickName( m_NickName, wstrErrorMessage ) )
	{
		SetAlarmAboutName( wstrErrorMessage.c_str() );
		wstring wstrName( L"Emotion_No" );
		m_pUnitViewerUI->PlayEmotion( CX2Unit::ET_NO, wstrName );

		return false;
	}

	return true;
}

void CX2StateCreateUnit::InitDummyUnits()
{
	KUnitInfo kUnitInfo;
	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyElsword";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_ELSWORD_SWORDMAN;
	m_pDummyElsword = new CX2Unit( kUnitInfo );
	//ReadyUnitResources( m_pDummyElsword );

	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyArme";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_ARME_VIOLET_MAGE;
	m_pDummyArme = new CX2Unit( kUnitInfo );
	//ReadyUnitResources( m_pDummyArme );

	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyLire";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_LIRE_ELVEN_RANGER;
	m_pDummyLire = new CX2Unit( kUnitInfo );
	//ReadyUnitResources( m_pDummyLire );

	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyRaven";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_RAVEN_FIGHTER;
	m_pDummyRaven = new CX2Unit( kUnitInfo );
	//ReadyUnitResources( m_pDummyRaven );



	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyEve";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_EVE_NASOD;
	m_pDummyEve = new CX2Unit( kUnitInfo );
	//ReadyUnitResources( m_pDummyEve );


//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyChung";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_CHUNG_IRON_CANNON;
	m_pDummyChung = new CX2Unit( kUnitInfo );
	XSkinMeshReady(L"CDC_MiniGun_L_Start.X");
	XSkinMeshReady(L"CDC_MiniGun_R_Start.X");
//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyAra";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_ARA_MARTIAL_ARTIST;
	m_pDummyAra = new CX2Unit( kUnitInfo );
	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyEL";
	kUnitInfo.m_cUnitClass = CX2Unit::UC_ELESIS_KNIGHT;
	m_pDummyEL = new CX2Unit( kUnitInfo );
//#ifdef NEW_CHARACTER_EL // �ؿ��� ����
	kUnitInfo.Init();
	kUnitInfo.m_wstrNickName = L"DummyAdd";		/// ������ �ʿ� �ϴ�.
	kUnitInfo.m_cUnitClass = CX2Unit::UC_ADD_NASOD_RULER;
	m_pDummyAdd = new CX2Unit( kUnitInfo );
//#endif // NEW_CHARACTER_EL

	ReadyPromotionUnitResources();

}


void CX2StateCreateUnit::ReadyPromotionUnitResources()
{
    const CX2Item::ItemTemplet* pTemplet = NULL;

	for( int i=0; i<std::ssize(ELSWORD_PROMOTION_ITEM_ID); i++ )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( ELSWORD_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}

	for( int i=0; i<std::ssize(AISHA_PROMOTION_ITEM_ID); i++ )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( AISHA_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}



	for( int i=0; i<std::ssize(RENA_PROMOTION_ITEM_ID); i++ )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( RENA_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}

	for( int i=0; i<std::ssize(RAVEN_PROMOTION_ITEM_ID); i++ )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( RAVEN_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}


	for( int i=0; i<std::ssize(EVE_PROMOTION_ITEM_ID); i++ )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( EVE_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}

	//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG

	for( int i=0; i<std::ssize(CHUNG_PROMOTION_ITEM_ID); i++ )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( CHUNG_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}

//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

	for( int i = 0; i < std::ssize( ARA_PROMOTION_ITEM_ID ); ++i )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( ARA_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}
	for( int i = 0; i < std::ssize( EL_PROMOTION_ITEM_ID ); ++i )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( EL_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}
	for( int i = 0; i < std::ssize( ADD_PROMOTION_ITEM_ID ); ++i )
	{
		pTemplet = g_pData->GetItemManager()->GetItemTemplet( ADD_PROMOTION_ITEM_ID[i] );
		if( pTemplet != NULL )
			XSkinMeshReady( pTemplet->GetModelName(0) );
	}

}


bool CX2StateCreateUnit::ReadyUnitResources( CX2Unit* pUnit )
{
	if ( pUnit == NULL )
		return false;

	for( int j =0; j < (int)pUnit->GetUnitData().m_NowEqipItemUIDList.size(); j++ )
	{
		CX2Item* pItem = pUnit->GetInventory().GetItem( pUnit->GetUnitData().m_NowEqipItemUIDList[j] );
        XSkinMeshReady( pItem->GetItemTemplet()->GetModelName(0) );
	}

    const CX2Item::ItemTemplet* pTemplet = NULL;
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicHairItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicFaceItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicBodyItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicLegItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicHandItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicFootItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_BasicWeaponItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );

	// ���̺��� �ٸ� ĳ���Ϳ� �ٸ��� ����, ������ �޽ø� ����� ����Ѵ�
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_RavenLeftArmItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );
	// dmlee 2008.12.12 - �� ��° ����
	pTemplet = g_pData->GetItemManager()->GetItemTemplet( pUnit->GetUnitTemplet()->m_SecondWeaponItemID );
	if( pTemplet != NULL )
		XSkinMeshReady( pTemplet->GetModelName(0) );

	return true;
}


void CX2StateCreateUnit::StaticControlAnimate()
{
	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_Info_Window" )->SetOffsetPos( D3DXVECTOR2( -350, 0 ) );
	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_ID_box" )->SetColor( D3DXCOLOR(0,0,0,0) );
	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_ID_box" )->SetOffsetPos( D3DXVECTOR2( -10, 10 ) );
	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_ID_box" )->SetShow( true );
//	m_pDLGCreateUnitFront->GetControl( L"StaticCreateUnitNickName" )->SetOffsetPos( D3DXVECTOR2(-350, 0) );
	m_pDLGCreateUnitFront->GetControl( L"IMEEditBoxUnitCreateNickName" )->SetOffsetPos( D3DXVECTOR2(-10, 10) );


	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_ID_box" )->Move(D3DXVECTOR2(2,-2), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0.05f, true );
	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_ID_box" )->Move(D3DXVECTOR2(0,0), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0.05f, false );
	m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_Info_Window" )->Move(D3DXVECTOR2(0,0), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0.2f, false );
//	m_pDLGCreateUnitFront->GetControl( L"StaticCreateUnitNickName" )->Move(D3DXVECTOR2(0,0), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0.2f, true );
	m_pDLGCreateUnitFront->GetControl( L"IMEEditBoxUnitCreateNickName" )->Move(D3DXVECTOR2(2,-2), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0.05f, true );
	m_pDLGCreateUnitFront->GetControl( L"IMEEditBoxUnitCreateNickName" )->Move(D3DXVECTOR2(0,0), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 0.05f, false );
}

void CX2StateCreateUnit::OpenNickNameBox()
{
	m_pDLGCreateUnitFront->GetControl( L"Character_Create_ID" )->SetShow( true );
	//m_pDLGCreateUnitFront->GetControl( L"Character_Create")->SetShow( true );
	m_pDLGCreateUnitFront->GetControl( L"ButtonCreate" )->SetShow( true );

	m_pDLGCreateUnitFront->GetControl( L"IMEEditBoxUnitCreateNickName" )->SetEnable( true );
	m_pDLGCreateUnitFront->GetControl( L"IMEEditBoxUnitCreateNickName" )->RequestFocus();
}

void CX2StateCreateUnit::SetAlarmAboutName( const WCHAR* /*pContent*/ )
{
}




/*virtual*/ bool CX2StateCreateUnit::LastMsgByESC()
{
	// oasis907 : ����� [2011.6.8] �����е� Ȱ���� ���� ����â���� ESCŰ ����
	if(g_pMain->GetSecurityPad() != NULL &&
		g_pMain->GetSecurityPad()->GetShow() == true)
	{
		return true;
	}


	//{{ kimhc // 2009-12-23 // ������ �߰��� ������ ���� ��� ä�� ����â���� �̵� �Ѵٴ� �˾�
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetUnitNum() == 0 )
		OpenLastMsgPopUp( GET_STRING( STR_ID_736 ) );
	else
		//}} kimhc // 2009-12-23 // ������ �߰��� ������ ���� ��� ä�� ����â���� �̵� �Ѵٴ� �˾�
		if( NULL == m_pDLGCreateUnitServerSelect )
		{
			OpenLastMsgPopUp( GET_STRING( STR_ID_560 ), L"DLG_UI_Selection_MessageBox_Ok_Exit_Button_New.lua", L"UI_PopUp_Positive_01.ogg" );
		}
		else
		{
			SAFE_DELETE_DIALOG ( m_pDLGCreateUnitServerSelect );
			m_pDLGCreateUnitServerSelect = NULL;
		}
	return true;
}

/*virtual*/ void CX2StateCreateUnit::LastMsgPopOk()
{
	if ( m_pDLGLastMsgPopUp != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGLastMsgPopUp, 0, false );
	m_pDLGLastMsgPopUp = NULL;

	//{{ kimhc // 2009-12-23 // ������ �߰��� ������ ���� ��� ä�� ����â���� �̵� �Ѵٴ� �˾�
	if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetUnitNum() == 0 )
		g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_4837 ) );
	else

		g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_552 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );

	g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, 0, false );
}


// ĳ���� ���� ����â�� ��ũ��Ʈ�� ���� Ŭ������ �����Ѵ�.
void CX2StateCreateUnit::ChangeCreateUnitTypeChangeInfoDlg ( CX2Unit::UNIT_TYPE eUnitType_ )
{
	SAFE_DELETE_DIALOG ( m_pDLGCreateUnitChangeClassInfo );
	wstring wstrCharacterClassChangeInfoDlgName = L"";
	switch ( eUnitType_ )
	{
	case CX2Unit::UT_ELSWORD :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Elsword.lua";
		break;
	case CX2Unit::UT_ARME :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Arme.lua";
		break;
	case CX2Unit::UT_LIRE :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Lire.lua";
		break;
	case CX2Unit::UT_RAVEN :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Raven.lua";
		break;
	case CX2Unit::UT_EVE :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Eve.lua";
		break;
	case CX2Unit::UT_CHUNG :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Chung.lua";
		break;

	case CX2Unit::UT_ARA :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Ara.lua";
		break;

	case CX2Unit::UT_ELESIS :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Elesis.lua";
		break;

	case CX2Unit::UT_ADD :
		wstrCharacterClassChangeInfoDlgName = L"DLG_Unit_Create_ClassChange_Button_Add.lua";
		break;
	default :
		ASSERT ( !L"Wrong Unit Type");
		return ;
	}

	m_pDLGCreateUnitChangeClassInfo = new CKTDGUIDialog( this, wstrCharacterClassChangeInfoDlgName.c_str() );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCreateUnitChangeClassInfo );
	ShowCreateUnitClassChangeInfoDlg ( true );
}

void CX2StateCreateUnit::ChangeCreateUnitClassChangeInfoDlg ( CX2Unit::UNIT_TYPE eUnitType_, CX2Unit::UNIT_CLASS_TYPE eUpperClassType_, CX2Unit::UNIT_CLASS_LINE eClassLine_ )
{
	// Type ���� Texture �̸��� ���ϰ� Class Type, Class Line ���� tet �̸��� ���Ѵ�.
	// pCreateInfoStatic �� pChangeInfoStatic �� �ű� ĳ���� Ȥ�� ���� �߰� �� ���ҽ��� �߰��ϴ��� �ƴϴ��� �� ���� �̴�.

	// �ٲ��� �ʴ�, ����̳� ���������� Static
	CKTDGUIStatic * pInfoNoChangeStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitBack->GetControl( L"StaticUnit_NoChange_Unit_Info" ) );
	if ( NULL != pInfoNoChangeStatic )
		pInfoNoChangeStatic->SetShow ( true );
	// The right-hand pieces of these statics are statics of their own in the client_data, so that they follow the
	// panel to the right (T44): the NoChange lines, and the Create graph (the original's picture 3).
	if ( CKTDGUIControl* pInfoNoChangeRight = m_pDLGCreateUnitBack->GetControl( L"StaticUnit_NoChange_Unit_Info_Right" ) )
		pInfoNoChangeRight->SetShow( true );

	// �߰��ؾ� �� ���ҽ��� ��� ���⿡ �����Ѵ�.
	CKTDGUIStatic * pCreateInfoStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitBack->GetControl( L"StaticUnit_Create_Unit_Info" ) );
	CKTDGUIStatic * pCreateInfoStatic2 = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"StaticUnit_Create_Unit_Info" ) );
	if ( NULL != pCreateInfoStatic && NULL != pCreateInfoStatic2 )
	{
		pCreateInfoStatic->SetShow ( true );
		pCreateInfoStatic2->SetShow( true );
		wstring wstrCreateStaticTexName = GetUnitClassCreateInfoTextureNameByUnitType ( eUnitType_ );

		for ( int i = 0; i < pCreateInfoStatic->GetPictureNum(); ++i )
		{
			if ( NULL != pCreateInfoStatic->GetPicture( i ) )
			{
				wstring wstrKeyName = GetUnitClassCreateInfoKeyNameByPictureIndex ( i, eUpperClassType_, eClassLine_ );
 				if ( wstrKeyName == L"")
 					pCreateInfoStatic->GetPicture( i )->SetShow ( false );
 				else
 				{
 					pCreateInfoStatic->GetPicture( i )->SetShow ( true );
 					pCreateInfoStatic->GetPicture( i )->SetTex( wstrCreateStaticTexName.c_str(), wstrKeyName.c_str() );
 				}
			}
		}

		CKTDGUIStatic* pCreateGraphStatic = static_cast<CKTDGUIStatic*>( m_pDLGCreateUnitBack->GetControl( L"StaticUnit_Create_Unit_Graph" ) );
		if ( NULL != pCreateGraphStatic && NULL != pCreateGraphStatic->GetPicture( 0 ) )
		{
			const wstring wstrKeyName = GetUnitClassCreateInfoKeyNameByPictureIndex( 3, eUpperClassType_, eClassLine_ );
			pCreateGraphStatic->SetShow( wstrKeyName.empty() == false );
			if ( wstrKeyName.empty() == false )
				pCreateGraphStatic->GetPicture( 0 )->SetTex( wstrCreateStaticTexName.c_str(), wstrKeyName.c_str() );
		}

		if( NULL != pCreateInfoStatic2->GetPicture(0) )
		{
			wstring wstrKeyName = L"";
			if ( eUpperClassType_ == CX2Unit::UCT_BASIC_CLASS )
				wstrKeyName = L"text_noclass";
			else
			{
				WCHAR wBuf1[256] = {0,};
				StringCchPrintfW( wBuf1, ARRAY_SIZE(wBuf1), L"text_class_2_%d", static_cast<int> ( eClassLine_ ) );
				wstrKeyName = wBuf1;
			}

			pCreateInfoStatic2->GetPicture(0)->SetTex( wstrCreateStaticTexName.c_str(), wstrKeyName.c_str() );
		}
	}



	CX2Unit::UNIT_CLASS eUnitClass = GetUnitClassByUnitClassTypeAndLineType ( eUnitType_, eUpperClassType_, eClassLine_ );
	// ���游 �ص� �� ���ҽ��� ��� ���⿡ �����Ѵ�.
	CKTDGUIStatic * pChangeInfoStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitBack->GetControl( L"StaticUnit_Change_Unit_Info" ) );
	if ( NULL != pChangeInfoStatic )
	{
		pChangeInfoStatic->SetShow ( true );
		wstring wstrChangeStaticTexName = L"DLG_UI_Common_Texture_GUI_FLOW_01.tga";

		for ( int i = 0; i < pChangeInfoStatic->GetPictureNum(); ++i )
		{
			if ( NULL != pChangeInfoStatic->GetPicture( i ) )
			{
				wstring wstrKeyName = GetUnitClassChangeInfoKeyNameByPictureIndex ( i, eUnitClass );
 				if ( wstrKeyName == L"")
 					pChangeInfoStatic->GetPicture( i )->SetShow ( false );
 				else
 				{
 					pChangeInfoStatic->GetPicture( i )->SetShow ( true );
 					pChangeInfoStatic->GetPicture( i )->SetTex( wstrChangeStaticTexName.c_str(), wstrKeyName.c_str() );
 				}
			}
		}
	}
}


void CX2StateCreateUnit::ChangeCreateUnitStandMeshInstance ( CX2Unit::UNIT_TYPE eUnitType_ )
{

	if ( NULL != g_pData->GetUIMajorXMeshPlayer() && NULL != g_pData->GetUIEffectSet() )
	{
		wstring wstrName = L"";
		std::vector<wstring> vecWstrName;

		if ( NULL != g_pData->GetUIMajorXMeshPlayer() )
		{
			for( CKTDGXMeshPlayer::CXMeshInstanceHandle hStandMeshInst : m_vecStandMeshInst )
			{
				g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( hStandMeshInst );
			}
			m_vecStandMeshInst.clear();
		}
		g_pData->GetUIEffectSet()->StopEffectSet( m_hStandEffect );

		switch ( eUnitType_ )
		{
		case CX2Unit::UT_ELSWORD :
			vecWstrName.push_back(L"Dummy_Village_Board1");
			wstrName = L"EffectSet_Elsword_Stand_Aura";
			break;
		case CX2Unit::UT_ARME :
			vecWstrName.push_back(L"Dummy_Village_Board2");
			wstrName = L"EffectSet_Aisha_Stand_Aura";
			break;
		case CX2Unit::UT_LIRE :
			vecWstrName.push_back(L"Dummy_Village_Board3");
			vecWstrName.push_back(L"Dummy_Village_Board10");
			wstrName = L"EffectSet_Rena_Stand_Aura";
			break;
		case CX2Unit::UT_RAVEN :
			vecWstrName.push_back(L"Dummy_Village_Board4");
			wstrName = L"EffectSet_Revan_Stand_Aura";
			break;
		case CX2Unit::UT_EVE :
			vecWstrName.push_back(L"Dummy_Village_Board5");
			wstrName = L"EffectSet_Eve_Stand_Aura";
			break;
		case CX2Unit::UT_CHUNG :
			vecWstrName.push_back(L"Dummy_Village_Board6");
			wstrName = L"EffectSet_Chung_Stand_Aura";
			break;

		case CX2Unit::UT_ARA :
			vecWstrName.push_back(L"Dummy_Village_Board7");
			wstrName = L"EffectSet_Ara_Stand_Aura";
			break;

		case CX2Unit::UT_ELESIS :
			vecWstrName.push_back(L"Dummy_Village_Board8");
			wstrName = L"EffectSet_Elsword_Stand_Aura";
			break;

		case CX2Unit::UT_ADD :
			vecWstrName.push_back(L"Dummy_Village_Board9");
			wstrName = L"EffectSet_Add_Stand_Aura";
			break;
		default :
			ASSERT ( !L"Wrong Unit Type");
			return ;
		}


		for( wstring wstrName : vecWstrName )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshStandInst = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( m_pUnitViewerUI,
				wstrName.c_str(), m_pUnitViewerUI->GetMatrix().GetPos(),
				D3DXVECTOR3 ( 0, 0, 0 ), D3DXVECTOR3 ( 0, 0, 0 ) );

			if( NULL != pMeshStandInst )
				m_vecStandMeshInst.push_back(pMeshStandInst->GetHandle());
		}

		if( m_vecStandMeshInst.empty() == false )
			m_hStandEffect = g_pData->GetUIEffectSet()->PlayEffectSetByMeshPlayer(wstrName.c_str(), m_vecStandMeshInst[0], CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR );
	}
}


// ĳ���� ���� ���� DLG �� ���� ����
void CX2StateCreateUnit::ShowCreateUnitClassChangeInfoDlg( bool bIsShow_ )
{
	if ( NULL != m_pDLGCreateUnitChangeClassInfo )
	{
		// �ӽ� Static, ���߿� UI ������ ���� ���
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChange_ID" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChange_ID" )->SetShow( bIsShow_ );

		// ù��° ���� 1�� ���� ��ư
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeFirstLineFirstClass" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeFirstLineFirstClass" )->SetShow( bIsShow_ );

		// �ι�° ���� 1�� ����	��ư
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeSecondLineFirstClass" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeSecondLineFirstClass" )->SetShow( bIsShow_ );

		// ����° ���� 1�� ���� ��ư
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeThirdLineFirstClass" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeThirdLineFirstClass" )->SetShow( bIsShow_ );

		// ù��° ���� 2�� ���� ��ư
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeFirstLineSecondClass" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeFirstLineSecondClass" )->SetShow( bIsShow_ );

		// �ι�° ���� 2�� ���� ��ư
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeSecondLineSecondClass" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeSecondLineSecondClass" )->SetShow( bIsShow_ );

		// ����° ���� 2�� ���� ��ư
		if ( NULL != m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeThirdLineSecondClass" ) )
			m_pDLGCreateUnitChangeClassInfo->GetControl( L"ClassChangeThirdLineSecondClass" )->SetShow( bIsShow_ );
	}
}

// ���� ���ø��� ���θ�� �ƹ�Ÿ ����Ʈ�� �޾ƿͼ� �ش� �������� ����
void CX2StateCreateUnit::EquipPromotionItemByUnitTemplet ( const CX2Unit::UnitTemplet * pUnitTemplet_ )
{
	if ( NULL != pUnitTemplet_ )
	{
		m_pUnitViewerUI->AddEquipByTID( pUnitTemplet_->m_PromotionWeaponItemID );	// ����
		m_pUnitViewerUI->AddEquipByTID( pUnitTemplet_->m_PromotionBodyItemID );		// ����
		m_pUnitViewerUI->AddEquipByTID( pUnitTemplet_->m_PromotionLegItemID );		// ����
		m_pUnitViewerUI->AddEquipByTID( pUnitTemplet_->m_PromotionHandItemID );		// �尩
		m_pUnitViewerUI->AddEquipByTID( pUnitTemplet_->m_PromotionFootItemID );		// �Ź�
	}
}

// ���� Ŭ���� ���� �޽���
void CX2StateCreateUnit::SelectUpperClass ( CX2Unit::UNIT_TYPE eUnitType_, CX2Unit::UNIT_CLASS_TYPE eUpperClassType_, CX2Unit::UNIT_CLASS_LINE eClassLine_ )
{
	if( NULL == m_pDLGCreateUnitFront )
		return;

	CX2Unit::UNIT_CLASS eNowSelectUnitClass_= GetUnitClassByUnitClassTypeAndLineType ( eUnitType_, eUpperClassType_, eClassLine_ );
	if ( eNowSelectUnitClass_ == CX2Unit::UC_NONE )
		return;

	if( m_SelectUnitClass == eNowSelectUnitClass_ )
		return;

	m_SelectUnitClass = CX2Unit::UC_NONE;
	m_bSelectUnit = false;
	m_SelectUnitType = CX2Unit::UT_NONE;


	if ( m_bSelectUnit == false )
		m_bSelectUnit = true;

	CKTDGUIButton* pButton = static_cast<CKTDGUIButton*>(m_pDLGCreateUnitFront->GetControl( L"ButtonCreate"));
	if( NULL != pButton )
		pButton->SetShow(true);

	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDLGCreateUnitFront->GetControl( L"Creative_Character"));
	if( NULL != pStatic )
		pStatic->SetShow(true);

	pStatic = static_cast<CKTDGUIStatic*>(m_pDLGCreateUnitFront->GetControl( L"Creative_Character_Center"));
	if( NULL != pStatic )
		pStatic->SetShow(true);


	if( NULL == m_pUnitViewerUI )
	{
		m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
		g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );
	}

	if( NULL != m_pUnitViewerUI )
	{
		CX2Unit* pUnit = GetViewerUnitByUnitType ( eUnitType_);

		if ( NULL != pUnit )
		{
			m_SelectUnitType = eUnitType_;
			m_SelectUnitClass = eNowSelectUnitClass_;
			pUnit->ResetUnitClass ( eNowSelectUnitClass_ );
			const CX2Unit::UnitTemplet *pUnitTemplet = pUnit->GetUnitTemplet();
			if ( NULL != pUnitTemplet )
			{
				m_pUnitViewerUI->SetUnit( pUnit, CX2UnitViewerUI::UVS_CLASSCHANGE );
				m_pUnitViewerUI->SetClassChange ( true );
				m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( 0, -50, 200 ) );
				m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3(1.2f, 1.2f, 1.2f) );
				m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ĳ���ͺ�� ����Ʈ ��ġ ����
				m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVVMT_DOWN_LANDING );
				EquipPromotionItemByUnitTemplet( pUnitTemplet );

				m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

				// ���� ������ ������ �ߺ��ؼ� ��� !�ϴ� ���� �ƴ϶��, ������ ��� �ϱ�
				if( 0 != m_wstrIntroMovieName.compare( pUnitTemplet->m_IntroMovieFileName.c_str() ) )
				{
					m_wstrIntroMovieName = pUnitTemplet->m_IntroMovieFileName;

					CKTDGUIStatic * pMovieStatic = static_cast<CKTDGUIStatic*> ( m_pDLGCreateUnitFront->GetControl( L"IntroMovie" ) );
					if ( NULL != pMovieStatic &&
						NULL != pMovieStatic->GetMovie(0) )
					{
						pMovieStatic->GetMovie(0)->SetMovieFileName ( pUnitTemplet->m_IntroMovieFileName.c_str() );
						pMovieStatic->GetMovie(0)->SetShow ( true );
						pMovieStatic->SetShow ( true );
					}
				}
			}
		}
		ChangeCreateUnitClassChangeInfoDlg ( eUnitType_, eUpperClassType_, eClassLine_ );
	}
}

// UnitType �� �޾ƿ� �ش� ������ ��ȯ��
CX2Unit * CX2StateCreateUnit::GetViewerUnitByUnitType ( CX2Unit::UNIT_TYPE eUnitType )
{
	switch ( eUnitType )
	{
	case CX2Unit::UT_ELSWORD :
		return m_pDummyElsword;
		break;
	case CX2Unit::UT_ARME :
		return m_pDummyArme;
		break;
	case CX2Unit::UT_LIRE :
		return m_pDummyLire;
		break;
	case CX2Unit::UT_RAVEN :
		return m_pDummyRaven;
		break;
	case CX2Unit::UT_EVE :
		return m_pDummyEve;
		break;

	case CX2Unit::UT_CHUNG :
		return m_pDummyChung;
		break;

	case CX2Unit::UT_ARA :
		return m_pDummyAra;
		break;

	case CX2Unit::UT_ELESIS :
		return m_pDummyEL;
		break;

	case CX2Unit::UT_ADD :
		return m_pDummyAdd;
		break;

	default :
		break;
	}

	ASSERT( !L"Unit Type Is Wrong !!");
	return NULL;
}


void CX2StateCreateUnit::SetRightSideFlagColor ( D3DXCOLOR colorFlag )
{
	if ( NULL != m_pDLGCreateUnitBack )
	{
		CKTDGUIStatic* pStaticSideFlag = m_pDLGCreateUnitBack->GetStatic_LUA("SideFlag");
		// CKTDGUIControl::CPictureData* pPictureBattleFieldName = pStaticBattleFieldName->GetPicture( 0 );
		if ( NULL != pStaticSideFlag )
		{
			pStaticSideFlag->SetShow ( true );
			CKTDGUIControl::CPictureData* pSideFlagPictureData = pStaticSideFlag->GetPicture ( 0 );
			if ( NULL != pSideFlagPictureData )
			{
				pSideFlagPictureData->SetColor ( colorFlag );
			}
		}
	}
}


wstring CX2StateCreateUnit::GetUnitClassCreateInfoTextureNameByUnitType ( CX2Unit::UNIT_TYPE eUnitType_ )
{
	switch ( eUnitType_ )
	{
	case CX2Unit::UT_ELSWORD :
		return L"DLG_UI_Common_Texture_GUI_character_info_01.tga";
	case CX2Unit::UT_ARME :
		return L"DLG_UI_Common_Texture_GUI_character_info_02.tga";
	case CX2Unit::UT_LIRE :
		return L"DLG_UI_Common_Texture_GUI_character_info_03.tga";
	case CX2Unit::UT_RAVEN :
		return L"DLG_UI_Common_Texture_GUI_character_info_04.tga";
	case CX2Unit::UT_EVE :
		return L"DLG_UI_Common_Texture_GUI_character_info_05.tga";

	case CX2Unit::UT_CHUNG :
		return L"DLG_UI_Common_Texture_GUI_character_info_06.tga";

	case CX2Unit::UT_ARA :
		return L"DLG_UI_Common_Texture_GUI_character_info_07.tga";

	case CX2Unit::UT_ELESIS :
		return L"DLG_UI_Common_Texture_GUI_character_info_08.tga";

	case CX2Unit::UT_ADD :
		return L"DLG_UI_Common_Texture_GUI_character_info_09.tga";
	default:
		break;
	}

	return L"";
}

wstring CX2StateCreateUnit::GetUnitClassCreateInfoKeyNameByPictureIndex ( int iIndex_, CX2Unit::UNIT_CLASS_TYPE eUpperClassType_, CX2Unit::UNIT_CLASS_LINE eClassLine_ )
{
	// �� ĳ���͸� ������ ��, �ش� Index ������ Ʋ���� ���� �۵� ���� �ʽ��ϴ�.
	// �ε��� ��ȣ 0 : ĳ���� ��, 1 : ĳ���� ���� ��, 2 : ĳ���� ���� ���� Ư¡, 3 : ĳ���� �ɷ�ġ �Ķ���� ����
	//  2 : ĳ���� Ư¡ �� front�� �̵�.
	switch ( iIndex_ )
	{
		case 0 : // ĳ���� �̸�
			{
				return L"name_noclass";
			}
			break;

		case 1 : // ĳ���� ���� ��
			{
				if ( eUpperClassType_ == CX2Unit::UCT_BASIC_CLASS )
				{
					return L"";
				}

				else
				{
					wstringstream wstringstreamKeyName;
					wstringstreamKeyName << L"name_class_" << static_cast<int> ( eUpperClassType_ ) << L"_" << static_cast<int> ( eClassLine_ );
					return wstringstreamKeyName.str().c_str();
				}
			}
			break;


		//case 2 : // ĳ���� Ư¡
		//	{
		//		if ( eUpperClassType_ == CX2Unit::UCT_BASIC_CLASS )
		//		{
		//			return L"text_noclass";
		//		}

		//		else
		//		{
		//			wstringstream wstringstreamKeyName;
		//			wstringstreamKeyName << L"text_class_2_" << static_cast<int> ( eClassLine_ );
		//			return wstringstreamKeyName.str().c_str();
		//		}
		//	}
		//	break;

		case 2 : // ĳ���� ���� ���� Ư¡
			{
				if ( eUpperClassType_ == CX2Unit::UCT_BASIC_CLASS )
				{
					return L"text_noclass_under";
				}

				else
				{
					wstringstream wstringstreamKeyName;
					wstringstreamKeyName << L"text_class_under_2_" << static_cast<int> ( eClassLine_ );
					return wstringstreamKeyName.str().c_str();
				}
			}
			break;

		case 3 : // ĳ���� �ɷ�ġ �Ķ���� ����
			{
				if ( eUpperClassType_ == CX2Unit::UCT_BASIC_CLASS )
				{
					return L"star_graph_noclass";
				}

				else
				{
					wstringstream wstringstreamKeyName;
					wstringstreamKeyName << L"star_graph_class_2_" << static_cast<int> ( eClassLine_ );
					return wstringstreamKeyName.str().c_str();
				}
			}
			break;
	}


	return L"";

}


wstring CX2StateCreateUnit::GetUnitClassChangeInfoKeyNameByPictureIndex ( int iIndex_, CX2Unit::UNIT_CLASS eUnitClass_ )
{
	// �� ĳ���͸� ������ ��, �ش� Index ������ Ʋ���� ���� �۵� ���� �ʽ��ϴ�.
	// �ε��� ��ȣ 0 : ĳ���� ��, 1 : ĳ���� ���� ��, 2 : ĳ���� Ư¡, 3 : ĳ���� ���� ���� Ư¡, 4 : ĳ���� �ɷ�ġ �Ķ���� ����
	UNIT_STAT_INFOMATION_CHART sChart = GetUnitStatInfomationChart ( eUnitClass_ );
	wstringstream wstringstreamKeyName;

	switch ( iIndex_ )
	{
	case 0 : // �ӵ� ( ���� )
		{
			wstringstreamKeyName << L"Bg_Slow";
			if ( USICL_LOW == sChart.m_eSpeed )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;

	case 1 : // �ӵ� ( ���� )
		{
			wstringstreamKeyName << L"Bg_Basic";
			if ( USICL_MIDDLE == sChart.m_eSpeed )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;


	case 2 : // �ӵ� ( ���� )
		{
			wstringstreamKeyName << L"Bg_Fast";
			if ( USICL_HIGH == sChart.m_eSpeed )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;

	case 3 : // ���� �Ÿ� ( �ٰŸ� )
		{
			wstringstreamKeyName << L"Bg_ShortRange";
			if ( USICL_LOW == sChart.m_eAttackDistance )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;

	case 4 : // ���� �Ÿ� ( �߰Ÿ� )
		{
			wstringstreamKeyName << L"Bg_MiddleRange";
			if ( USICL_MIDDLE == sChart.m_eAttackDistance )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;

	case 5 : // ���� �Ÿ� ( ���Ÿ� )
		{
			wstringstreamKeyName << L"Bg_LongRange";
			if ( USICL_HIGH == sChart.m_eAttackDistance )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;
	case 6 : // ���̵� ( ���� )
		{
			wstringstreamKeyName << L"Bg_Easy";
			if ( USICL_LOW == sChart.m_eDifficultLevel )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;
	case 7 : // ���̵� ( ���� )
		{
			wstringstreamKeyName << L"Bg_Basic";
			if ( USICL_MIDDLE == sChart.m_eDifficultLevel )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;
	case 8 : // ���̵� ( ����� )
		{
			wstringstreamKeyName << L"Bg_Difficult";
			if ( USICL_HIGH == sChart.m_eDifficultLevel )
				wstringstreamKeyName << L"_Active";

			return wstringstreamKeyName.str().c_str();
		}
		break;
	}
	return L"";

}
CX2StateCreateUnit::UNIT_STAT_INFOMATION_CHART CX2StateCreateUnit::GetUnitStatInfomationChart ( CX2Unit::UNIT_CLASS eUnitClass_ )
{
	switch ( eUnitClass_ )
	{
		case CX2Unit::UC_ELSWORD_SWORDMAN :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_LOW );
		case CX2Unit::UC_ELSWORD_KNIGHT :
		case CX2Unit::UC_ELSWORD_LORD_KNIGHT :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_LOW );
		case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT :
		case CX2Unit::UC_ELSWORD_RUNE_SLAYER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_LOW );
		case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT :
		case CX2Unit::UC_ELSWORD_INFINITY_SWORD :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_LOW );

		case CX2Unit::UC_ARME_VIOLET_MAGE :
			return UNIT_STAT_INFOMATION_CHART ( USICL_LOW, USICL_MIDDLE, USICL_HIGH );
		case CX2Unit::UC_ARME_HIGH_MAGICIAN :
		case CX2Unit::UC_ARME_ELEMENTAL_MASTER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_LOW, USICL_HIGH, USICL_MIDDLE );
		case CX2Unit::UC_ARME_DARK_MAGICIAN :
		case CX2Unit::UC_ARME_VOID_PRINCESS :
			return UNIT_STAT_INFOMATION_CHART ( USICL_LOW, USICL_MIDDLE, USICL_HIGH );
		case CX2Unit::UC_ARME_BATTLE_MAGICIAN :
		case CX2Unit::UC_ARME_DIMENSION_WITCH :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_MIDDLE );


		case CX2Unit::UC_LIRE_ELVEN_RANGER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_HIGH, USICL_LOW );
		case CX2Unit::UC_LIRE_COMBAT_RANGER :
		case CX2Unit::UC_LIRE_WIND_SNEAKER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_MIDDLE );
		case CX2Unit::UC_LIRE_SNIPING_RANGER :
		case CX2Unit::UC_LIRE_GRAND_ARCHER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_HIGH, USICL_LOW );
		case CX2Unit::UC_LIRE_TRAPPING_RANGER :
		case CX2Unit::UC_LIRE_NIGHT_WATCHER:
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_HIGH );


		case CX2Unit::UC_RAVEN_FIGHTER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_MIDDLE );
		case CX2Unit::UC_RAVEN_SOUL_TAKER :
		case CX2Unit::UC_RAVEN_BLADE_MASTER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_LOW, USICL_HIGH );
		case CX2Unit::UC_RAVEN_OVER_TAKER :
		case CX2Unit::UC_RAVEN_RECKLESS_FIST :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_MIDDLE );
		case CX2Unit::UC_RAVEN_WEAPON_TAKER :
		case CX2Unit::UC_RAVEN_VETERAN_COMMANDER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_HIGH );


		case CX2Unit::UC_EVE_NASOD :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_MIDDLE );
		case CX2Unit::UC_EVE_EXOTIC_GEAR :
		case CX2Unit::UC_EVE_CODE_NEMESIS :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_LOW, USICL_HIGH );
		case CX2Unit::UC_EVE_ARCHITECTURE :
		case CX2Unit::UC_EVE_CODE_EMPRESS :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_MIDDLE );
		case CX2Unit::UC_EVE_ELECTRA :
		case CX2Unit::UC_EVE_BATTLE_SERAPH :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_HIGH, USICL_LOW );

		case CX2Unit::UC_CHUNG_IRON_CANNON :
			return UNIT_STAT_INFOMATION_CHART ( USICL_LOW, USICL_MIDDLE, USICL_LOW );
		case CX2Unit::UC_CHUNG_FURY_GUARDIAN :
		case CX2Unit::UC_CHUNG_IRON_PALADIN :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_LOW );
		case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN :
		case CX2Unit::UC_CHUNG_DEADLY_CHASER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_HIGH, USICL_MIDDLE );
		case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN :
		case CX2Unit::UC_CHUNG_TACTICAL_TROOPER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_LOW, USICL_MIDDLE, USICL_MIDDLE );

		case CX2Unit::UC_ARA_MARTIAL_ARTIST :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_LOW, USICL_MIDDLE );
		case CX2Unit::UC_ARA_LITTLE_HSIEN :
		case CX2Unit::UC_ARA_SAKRA_DEVANAM :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_MIDDLE, USICL_MIDDLE );
		case CX2Unit::UC_ARA_LITTLE_DEVIL :
		case CX2Unit::UC_ARA_YAMA_RAJA :
			return UNIT_STAT_INFOMATION_CHART ( USICL_HIGH, USICL_HIGH, USICL_HIGH );


		case CX2Unit::UC_ELESIS_KNIGHT :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_MIDDLE );
		case CX2Unit::UC_ELESIS_SABER_KNIGHT :

		case CX2Unit::UC_ELESIS_GRAND_MASTER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_LOW, USICL_MIDDLE );
		case CX2Unit::UC_ELESIS_PYRO_KNIGHT :

		case CX2Unit::UC_ELESIS_BLAZING_HEART :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_MIDDLE );

		case CX2Unit::UC_ADD_NASOD_RULER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_HIGH, USICL_MIDDLE );
		case CX2Unit::UC_ADD_PSYCHIC_TRACER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_MIDDLE );

		case CX2Unit::UC_ADD_LUNATIC_PSYKER :
			return UNIT_STAT_INFOMATION_CHART ( USICL_MIDDLE, USICL_MIDDLE, USICL_MIDDLE );

	default:
		break;
	}
	return UNIT_STAT_INFOMATION_CHART ( USICL_NONE, USICL_NONE, USICL_NONE );
}


bool CX2StateCreateUnit::Handler_EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ENTRY_POINT_CHECK_NICK_NAME_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_ENTRY_POINT_CHECK_NICK_NAME_ACK ) == true )
	{
		switch( kEvent.m_iOK )
		{
		case NetError::ERR_RESTORE_NICK_NAME_00:
		case NetError::ERR_RESTORE_NICK_NAME_01:
		case NetError::ERR_RESTORE_NICK_NAME_02:
		case NetError::ERR_RESTORE_NICK_NAME_03:
		case NetError::ERR_RESTORE_NICK_NAME_04:
		case NetError::ERR_RESTORE_NICK_NAME_05:
		case NetError::ERR_RESTORE_NICK_NAME_06:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), NetError::GetErrStrF( kEvent.m_iOK ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				return true;
			} break;
		}

		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{

			wstringstream wstrstreamCreateUnitInfo;
			wstring wstrServerName = GET_STRING( STR_ID_4835 ); //���̾�
			if ( m_eSelectServerGroupID == SEnum::SGI_SOLES )
				wstrServerName = GET_STRING( STR_ID_4836 ); //�ַ���
			wstrstreamCreateUnitInfo << GET_STRING( STR_ID_28992 ) << L" : " << wstrServerName << " / " << GET_STRING( STR_ID_1127 ) << L" : " << kEvent.m_wstrNickName << endl;
			m_pDLGCreateUnitCheckInfo = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(305, 375), wstrstreamCreateUnitInfo.str().c_str(), SCUUCM_CREATE_UNIT, this, SCUUCM_PASS_NICKNAME_CHECK_CANCEL,
				L"DLG_UI_Character_Selection_Creat_Unit_New.lua", D3DXVECTOR2( 0, 0 ), L"UI_PopUp_Positive_01.ogg" );
		}
	}
	return true;
}
bool CX2StateCreateUnit::Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ ()
{
	g_pData->GetServerProtocol()->SendID( EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ );
	g_pMain->AddServerPacket( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK );
	return true;
}
bool CX2StateCreateUnit::Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_CREATE_UNIT_TODAY_COUNT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK ) == true )
	{
		typedef std::map< int, int > TodayCreateUnitCount;

		for( const TodayCreateUnitCount::value_type& CreateAbleUnitSet : kEvent.m_mapCreateCharCountToday )
		{
			switch ( CreateAbleUnitSet.first )
			{
			case SEnum::SGI_SOLES :
				m_iSolesServerCreateAbleUnitMany = static_cast<int> ( CreateAbleUnitSet.second );
				break;

			case SEnum::SGI_GAIA :
				m_iGaiaServerCreateAbleUnitMany = static_cast<int> ( CreateAbleUnitSet.second );
				break;

			default :
				break;
			}
		}
	}
	return true;
}


void CX2StateCreateUnit::PlaySoundCreateUnit( CX2Unit::UNIT_TYPE eUnitType )
{
	if( g_pKTDXApp->GetDeviceManager() == NULL )
		return;

	if( eUnitType <= CX2Unit::UT_NONE || eUnitType >= CX2Unit::UT_END )
		return;

	wstring wstrSoundName = L"";

	switch ( eUnitType )
	{

	case CX2Unit::UT_ELSWORD:
		{
			wstrSoundName = L"ElswordVoice_Create0";
		} break;

	case CX2Unit::UT_ARME:
		{
			wstrSoundName = L"AishaVoice_Create0";
		} break;

	case CX2Unit::UT_LIRE:
		{
			wstrSoundName = L"LenaVoice_Create0";
		} break;

	case CX2Unit::UT_RAVEN:
		{
			wstrSoundName = L"RavenVoice_Create0";
		} break;

	case CX2Unit::UT_EVE:
		{
			wstrSoundName = L"EveVoice_Create0";
		} break;

	case CX2Unit::UT_CHUNG:
		{
			wstrSoundName = L"ChungVoice_Create0";
		} break;

	case CX2Unit::UT_ARA:
		{
			wstrSoundName = L"AraVoice_Create0";
		} break;

	case CX2Unit::UT_ELESIS:
		{
			wstrSoundName = L"ElesisVoice_Create0";
		} break;

	case CX2Unit::UT_ADD:
		{
			wstrSoundName = L"AddVoice_Create0";
		} break;

	default:
		break;
	}

	WCHAR buff[6];
	int iRandomSelect = RandomInt() % 2 + 1;
	StringCchPrintf( buff, ARRAY_SIZE(buff), L"%d.ogg ", iRandomSelect );
	wstrSoundName += std::wstring(buff);

	if( wstrSoundName.compare(L"") != 0 )
		g_pKTDXApp->GetDeviceManager()->PlaySound(wstrSoundName.c_str(), false, false );
}
