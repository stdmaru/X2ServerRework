#include "stdafx.h"
#include "x2/platform/process.h"
#include "./X2SecurityPad.h"



namespace _CONST_SECURITY_PAD_
{
	const int MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON = 11;
}

CX2SecurityPad::CX2SecurityPad()
{
	m_pDLGSecurityPad	= NULL;
	m_pStaticMemo		= NULL;

	m_bShow				= false;
	m_eState			= SPS_CLOSE;
	m_iPhase			= -1;
	m_bUseSecondPW		= false;

	m_iPasswordIndex	= 0;
	for(int i=0; i<7; i++)
	{
		m_cPassword[i] = 0;
	}

	for(int i=0; i<12; i++)
	{
		m_iButtonSequence[i] = -1;
	}

	m_wstrAutoAuthorityPassword = L"";

	m_bIsAutoAuthorityPassworBegin = false;
}

CX2SecurityPad::~CX2SecurityPad()
{
	SAFE_DELETE_DIALOG(m_pDLGSecurityPad);
}

void CX2SecurityPad::SetShow(bool bShow, SECURITY_PAD_STATE eState /* = SPS_CLOSE */, bool bIsDeleteOldDialog /* = true */ )
{
	m_eState = eState;
	m_bShow = bShow;

	if(m_bShow == false || m_eState == SPS_CLOSE)
	{
		m_iPhase = 0;
		ClearInputPassword();

		m_wstrPassword_Temp1.clear();
		m_wstrPassword_Temp2.clear();
		m_pDLGSecurityPad->SetShowEnable(false, false);

	}
	else
	{
		if ( true == bIsDeleteOldDialog )
		{
			// Test
			if(m_pDLGSecurityPad != NULL)
			{
				SAFE_DELETE_DIALOG(m_pDLGSecurityPad);
			}

			if(m_pDLGSecurityPad == NULL)
			{
				m_pDLGSecurityPad = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Security_Pad_New.lua", 0.07f, XDL_OPTION );
				g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGSecurityPad );
			}
		}
		else
		{
			if ( m_pDLGSecurityPad == NULL )
			{
				m_pDLGSecurityPad = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Security_Pad_New.lua", 0.07f, XDL_OPTION );
			}
		}
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGSecurityPad );


		{
			for(int i=0; i<_CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON; i++)
			{
				CKTDGUIButton* pButtonDialNumber = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl(GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"ButtonDialNumber" ), i ) ) )  );
				if ( NULL != pButtonDialNumber )
				{
					pButtonDialNumber->SetShowEnable( true, true );
				}
			}

			CKTDGUIStatic* pNoticeStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"NoticeStatic" ) );
			if ( NULL != pNoticeStatic )
				pNoticeStatic->SetShow ( false );

			CKTDGUIStatic* pHideNumberPadStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"HideNumberPad" ) );
			if ( NULL != pHideNumberPadStatic )
				pHideNumberPadStatic->SetShow ( false );

			CKTDGUIButton* pChangeButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonChange" ) );
			if ( NULL != pChangeButton )
				pChangeButton->SetShowEnable( false, false );

			CKTDGUIButton* pReleaseButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonRelease" ) );
			if ( NULL != pReleaseButton )
				pReleaseButton->SetShowEnable( false, false );

			CKTDGUIButton* pCancleButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"Button_exit2" ) );
			if ( NULL != pCancleButton )
				pCancleButton->SetShowEnable( false, false );

			CKTDGUIButton* pOkButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonOK" ) );
			if ( NULL != pOkButton )
				pOkButton->SetShowEnable( true, true );

			CKTDGUIButton* pCancelButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonCancel" ) );
			if ( NULL != pCancelButton )
				pCancelButton->SetShowEnable( true, true );

			CKTDGUIButton* pExitButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"Button_exit" ) );
			if ( NULL != pExitButton )
				pExitButton->SetShowEnable( true, true );

			CKTDGUIStatic* pStaticSignal = static_cast< CKTDGUIStatic* >( m_pDLGSecurityPad->GetControl( L"StaticSignal" )  );
			if ( NULL != pStaticSignal )
				pStaticSignal->SetShow ( true );

			CKTDGUIStatic* pInputStateStatic = static_cast< CKTDGUIStatic* >( m_pDLGSecurityPad->GetControl( L"InputStateStatic" )  );
			if ( NULL != pInputStateStatic )
				pInputStateStatic->SetShow ( true );
		}

		m_iPhase = 0;
		m_wstrPassword_Temp1.clear();
		m_wstrPassword_Temp2.clear();
		ClearInputPassword();
		ShuffleNumber();
		m_pDLGSecurityPad->SetShowEnable(true, true);


		switch(m_eState)
		{
		case SPS_GAME_START:
			{

				CKTDGUIStatic* pStaticTitle = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"TitleStatic" ) );
				if ( NULL != pStaticTitle )
				{
					for ( int idx = 0; idx < pStaticTitle->GetPictureNum(); ++idx )
					{
						if ( NULL != pStaticTitle->GetPicture(idx) )
						{
							pStaticTitle->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pStaticTitle->GetPicture(1) )
					{
						pStaticTitle->GetPicture(1) ->SetShow ( true );
					}
				}

				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pInputstateStatic->GetPicture(0) )
						pInputstateStatic->GetPicture(0)->SetShow ( true );
				}

				CKTDGUIButton* pExitButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"Button_exit" ) );
				if ( NULL != pExitButton )
					pExitButton->SetShowEnable( true, true );


			}break;
		case SPS_CREATE_PASSWORD:
			{
				m_pDLGSecurityPad->SetCloseCustomUIEventID( SPUM_EXIT );

				CKTDGUIStatic* pStaticTitle = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"TitleStatic" ) );
				if ( NULL != pStaticTitle )
				{
					for ( int idx = 0; idx < pStaticTitle->GetPictureNum(); ++idx )
					{
						if ( NULL != pStaticTitle->GetPicture(idx) )
						{
							pStaticTitle->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pStaticTitle->GetPicture(0) )
					{
						pStaticTitle->GetPicture(0) ->SetShow ( true );
					}
				}
				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pInputstateStatic->GetPicture(0) )
						pInputstateStatic->GetPicture(0)->SetShow ( true );
				}
			}break;
		case SPS_CHANGE_PASSWORD:
			{

				m_pDLGSecurityPad->SetCloseCustomUIEventID( SPUM_EXIT );
				CKTDGUIStatic* pStaticTitle = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"TitleStatic" ) );
				if ( NULL != pStaticTitle )
				{
					for ( int idx = 0; idx < pStaticTitle->GetPictureNum(); ++idx )
					{
						if ( NULL != pStaticTitle->GetPicture(idx) )
						{
							pStaticTitle->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pStaticTitle->GetPicture(2) )
					{
						pStaticTitle->GetPicture(2) ->SetShow ( true );
					}
				}
				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}

					if ( NULL != pInputstateStatic->GetPicture(2) )
						pInputstateStatic->GetPicture(2)->SetShow ( true );
				}
			}break;
		case SPS_RELEASE_PASSWORD:
			{
				m_pDLGSecurityPad->SetCloseCustomUIEventID( SPUM_EXIT );
				CKTDGUIStatic* pStaticTitle = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"TitleStatic" ) );
				if ( NULL != pStaticTitle )
				{
					for ( int idx = 0; idx < pStaticTitle->GetPictureNum(); ++idx )
					{
						if ( NULL != pStaticTitle->GetPicture(idx) )
						{
							pStaticTitle->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pStaticTitle->GetPicture(3) )
					{
						pStaticTitle->GetPicture(3) ->SetShow ( true );
					}
				}
			}break;


		case SPC_SELECT_STATE :
			{
				CKTDGUIStatic* pHideNumberPadStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"HideNumberPad" ) );
				if ( NULL != pHideNumberPadStatic )
					pHideNumberPadStatic->SetShow ( true );

				m_pDLGSecurityPad->SetCloseCustomUIEventID( SPUM_EXIT );
				CKTDGUIStatic* pStaticTitle = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"TitleStatic" ) );
				if ( NULL != pStaticTitle )
				{
					for ( int idx = 0; idx < pStaticTitle->GetPictureNum(); ++idx )
					{
						if ( NULL != pStaticTitle->GetPicture(idx) )
						{
							pStaticTitle->GetPicture(idx)->SetShow( false );
						}
					}
				}
				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pInputstateStatic->GetPicture(0) )
						pInputstateStatic->GetPicture(0)->SetShow ( true );
				}

				for(int i=0; i<_CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON; i++)
				{
					CKTDGUIButton* pButtonDialNumber = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl(GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"ButtonDialNumber" ), i ) ) )  );
					if ( NULL != pButtonDialNumber )
					{
						pButtonDialNumber->SetShowEnable( true, false );
					}
				}

				CKTDGUIButton* pChangeButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonChange" ) );
				if ( NULL != pChangeButton )
					pChangeButton->SetShowEnable( true, true );
				CKTDGUIButton* pReleaseButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonRelease" ) );
				if ( NULL != pReleaseButton )
					pReleaseButton->SetShowEnable( true, true );
				CKTDGUIButton* pCancleButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"Button_exit2" ) );
				if ( NULL != pCancleButton )
					pCancleButton->SetShowEnable( true, true );

				CKTDGUIStatic* pNoticeStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"NoticeStatic" ) );
				if ( NULL != pNoticeStatic )
					pNoticeStatic->SetShow ( true );

				CKTDGUIButton* pOkButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonOK" ) );
				if ( NULL != pOkButton )
					pOkButton->SetShowEnable( false, false );

				CKTDGUIButton* pExitButton = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"Button_exit" ) );
				if ( NULL != pExitButton )
					pExitButton->SetShowEnable( false, false );

				CKTDGUIStatic* pStaticSignal = static_cast< CKTDGUIStatic* >( m_pDLGSecurityPad->GetControl( L"StaticSignal" )  );
				if ( NULL != pStaticSignal )
					pStaticSignal->SetShow ( false );

				CKTDGUIStatic* pInputStateStatic = static_cast< CKTDGUIStatic* >( m_pDLGSecurityPad->GetControl( L"InputStateStatic" )  );
				if ( NULL != pInputStateStatic )
					pInputStateStatic->SetShow ( false );
			}
			break;
		default:
			{

			}break;
		}
	}
	return;
}


void CX2SecurityPad::ShuffleNumber()
{
	if( m_pDLGSecurityPad == NULL)
		return;

	for(int i=0; i<_CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON; i++)
	{
		m_iButtonSequence[i] = -1;
	}
	bool bSequenceCheck[_CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON] = {false,};
	srand( (unsigned) time( NULL ) );

	for(int i=0; i < _CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON; i++)
	{
		int iRandOffset = rand() % (_CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON - i);

		for(int j=0; j< _CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON; j++)
		{
			if(bSequenceCheck[j] == false)
			{
				if(iRandOffset > 0)
				{
					iRandOffset--;
				}
				else
				{
					bSequenceCheck[j] = true;
					m_iButtonSequence[i] = j;
					break;
				}
			}
		}
	}

	for(int i=0; i<_CONST_SECURITY_PAD_::MAX_PASSWORD_BUTTON_EXCEPT_RESET_BUTTON; i++)
	{
		CKTDGUIButton* pButtonDialNumber = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl(GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"ButtonDialNumber" ), i ) ) )  );
		WCHAR wNormalTextrueKeyName[256] = {0};
		StringCchPrintf( wNormalTextrueKeyName, 256, L"bt_security_%d_N", m_iButtonSequence[i] );


		WCHAR wOverTextrueKeyName[256] = {0};
		StringCchPrintf( wOverTextrueKeyName, 256, L"bt_security_%d_OVER", m_iButtonSequence[i] );

		pButtonDialNumber->SetNormalTex(L"DLG_UI_Button_NEW_GUI_security_01.tga", wNormalTextrueKeyName );
		pButtonDialNumber->SetOverTex(L"DLG_UI_Button_NEW_GUI_security_01.tga", wOverTextrueKeyName );
		pButtonDialNumber->SetDownTex(L"DLG_UI_Button_NEW_GUI_security_01.tga", wOverTextrueKeyName );

	}
	return;
}


HRESULT CX2SecurityPad::OnFrameMove(double /*fTime*/, float /*fElapsedTime*/)
{


	return S_OK;
}
bool CX2SecurityPad::MsgProc(HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if(m_bShow == false)
		return false;

	return false;
}
void CX2SecurityPad::ClearInputPassword()
{
	m_iPasswordIndex = 0;
	for(int i=0; i<7; i++)
	{
		m_cPassword[i] = 0;
	}
	UpdateUIMarble();
	return;
}

void CX2SecurityPad::ProcessPassword()
{
	switch(m_eState)
	{
	case SPS_GAME_START: // Phase Count 1
		{
			m_wstrPassword_Temp1 = m_cPassword;

			// 입력이 없을 경우 무반응
			if(m_wstrPassword_Temp1 == L"")
				return;
			Handler_EGS_AUTH_SECOND_SECURITY_REQ();

		} break;
	case SPS_CREATE_PASSWORD: // Phase Count 2
		{
			if(m_iPhase == 0)
			{
				m_wstrPassword_Temp1 = m_cPassword;

				// 입력이 없을 경우 무반응
				if(m_wstrPassword_Temp1 == L"")
					return;

				m_iPhase = 1;
				ClearInputPassword();
				UpdateUIMarble();
				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pInputstateStatic->GetPicture(1) )
						pInputstateStatic->GetPicture(1)->SetShow ( true );
				}
				ShuffleNumber();
			}
			else if(m_iPhase == 1)
			{
				wstring wstrVerifyPassword = m_cPassword;

				// 입력이 없을 경우 무반응
				if(wstrVerifyPassword == L"")
					return;

				if(m_wstrPassword_Temp1 == wstrVerifyPassword)
				{
					Handler_EGS_CREATE_SECOND_SECURITY_REQ();
				}
				else
				{
					// 일치하지 않습니다.
					m_iPhase = 0;
					m_wstrPassword_Temp1.clear();
					m_wstrPassword_Temp2.clear();
					ClearInputPassword();
					UpdateUIMarble();

					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_12539 ), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
					SetShow(false);
				}
			}
		} break;
	case SPS_RELEASE_PASSWORD:  // Phase Count 1
		{
			m_wstrPassword_Temp1 = m_cPassword;

			// 입력이 없을 경우 무반응
			if(m_wstrPassword_Temp1 == L"")
				return;

			Handler_EGS_DELETE_SECOND_SECURITY_REQ();
		} break;
	case SPS_CHANGE_PASSWORD:  // Phase Count 3
		{
			if(m_iPhase == 0)
			{
				m_wstrPassword_Temp1 = m_cPassword;

				// 입력이 없을 경우 무반응
				if(m_wstrPassword_Temp1 == L"")
					return;

				Handler_EGS_COMPARE_SECOND_SECURITY_PW_REQ();
				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pInputstateStatic->GetPicture(0) )
						pInputstateStatic->GetPicture(0)->SetShow ( true );
				}
			}
			else if(m_iPhase == 1)
			{
				m_wstrPassword_Temp2 = m_cPassword;

				// 입력이 없을 경우 무반응
				if(m_wstrPassword_Temp2 == L"")
					return;

				m_iPhase = 2;
				ClearInputPassword();
				UpdateUIMarble();
				CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
				if ( NULL != pInputstateStatic )
				{
					for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
					{
						if ( NULL != pInputstateStatic->GetPicture(idx) )
						{
							pInputstateStatic->GetPicture(idx)->SetShow( false );
						}
					}
					if ( NULL != pInputstateStatic->GetPicture(1) )
						pInputstateStatic->GetPicture(1)->SetShow ( true );
				}
				ShuffleNumber();
			}
			else if(m_iPhase == 2)
			{
				wstring wstrVerifyPassword = m_cPassword;

				// 입력이 없을 경우 무반응
				if(wstrVerifyPassword == L"")
					return;

				if(m_wstrPassword_Temp2 == wstrVerifyPassword)
				{
					Handler_EGS_CHANGE_SECOND_SECURITY_PW_REQ();
				}
				else
				{
					// 일치하지 않습니다.
					m_iPhase = 0;
					m_wstrPassword_Temp1.clear();
					m_wstrPassword_Temp2.clear();
					ClearInputPassword();
					UpdateUIMarble();

					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_12539 ), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );

					SetShow(false);
				}
			}
		} break;

	case SPC_SELECT_STATE :
		{
			CKTDGUIStatic* pInputstateStatic = static_cast< CKTDGUIStatic* >(m_pDLGSecurityPad->GetControl( L"InputStateStatic" ) );
			if ( NULL != pInputstateStatic )
			{
				for ( int idx = 0; idx < pInputstateStatic->GetPictureNum(); ++idx )
				{
					if ( NULL != pInputstateStatic->GetPicture(idx) )
					{
						pInputstateStatic->GetPicture(idx)->SetShow( false );
					}
				}
				if ( NULL != pInputstateStatic->GetPicture(0) )
					pInputstateStatic->GetPicture(0)->SetShow ( true );
			}
		}
		break;
	default:
		break;
	}

	return;
}

bool CX2SecurityPad::UICustomEventProc( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM wParam, LPARAM lParam )
{

	switch ( wParam )
	{
	case SPUM_EXIT:
		{
			if(m_eState == SPS_GAME_START)
			{
				//CX2State::SUCM_EXIT
				g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ), GET_STRING( STR_ID_12541 ), CX2State::SUCM_EXIT, g_pMain->GetNowState(), -1, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
			}
			else
			{
				SetShow( false );
			}
			return true;
		} break;
	case SPUM_OK:
		{
			ProcessPassword();
			UpdateUIMarble();
			return true;
		} break;
	case SPUM_CANCEL:
		{
			if( m_iPhase == 0 )		// 임규수 일본 추가
			m_wstrPassword_Temp1.clear();
			if( m_iPhase != 2 )		// 임규수 일본 추가
			m_wstrPassword_Temp2.clear();
			ClearInputPassword();
			UpdateUIMarble();
			return true;
		} break;
	case SPUM_DIAL_NUMBER_0:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[0] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[0] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_1:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[1] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[1] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_2:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[2] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[2] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_3:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[3] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[3] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_4:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[4] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[4] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_5:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[5] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[5] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_6:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[6] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[6] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_7:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[7] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[7] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_8:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[8] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[8] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_9:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[9] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[9] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_10:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[10] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[10] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DIAL_NUMBER_11:
		{
			if(m_iPasswordIndex < 6 && m_iButtonSequence[11] < 10)
			{
				m_cPassword[m_iPasswordIndex] = m_iButtonSequence[11] + 48;
				m_iPasswordIndex++;
				UpdateUIMarble();
			}
			return true;
		} break;
	case SPUM_DELETE_PASSWORD_VERIFY_OK:
		{
			CKTDGUIControl* pControl = NULL;
			pControl		= reinterpret_cast< CKTDGUIControl* >( lParam );
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			Handler_EGS_DELETE_SECOND_SECURITY_VERIFY_REQ();
		} break;
	case SPUM_CHANGE_STATE_CHANGE_PASSWORD :
		g_pMain->GetSecurityPad()->SetShow(true, CX2SecurityPad::SPS_CHANGE_PASSWORD, false);
		break;
	case SPUM_CHANGE_STATE_RELEASE_PASSWORD :
		g_pMain->GetSecurityPad()->SetShow(true, CX2SecurityPad::SPS_RELEASE_PASSWORD, false);
		break;

	}
	return false;
}
void CX2SecurityPad::UpdateUIMarble()
{
	CKTDGUIStatic* pStaticSignal = static_cast< CKTDGUIStatic* >( m_pDLGSecurityPad->GetControl( L"StaticSignal" )  );

	for(int i=0; i<6; i++)
	{
		if(i< m_iPasswordIndex)
		{
			if ( NULL != pStaticSignal->GetPicture(i) )
				pStaticSignal->GetPicture(i)->SetTex( L"DLG_UI_Common_Texture_GUI_FLOW_security01.tga", L"PW_DOT_YELLOW" );
		}
		else
		{
			if ( NULL != pStaticSignal->GetPicture(i) )
				pStaticSignal->GetPicture(i)->SetTex( L"DLG_UI_Common_Texture_GUI_FLOW_security01.tga", L"PW_DOT_GRAY" );

		}
	}

	CKTDGUIButton* pButtonOk = static_cast< CKTDGUIButton* >( m_pDLGSecurityPad->GetControl( L"ButtonOK" )  );
	if(m_iPasswordIndex < 4)
	{
		pButtonOk->SetEnable( false );
	}
	else
	{
		pButtonOk->SetEnable( true );
	}
	return;
}

bool CX2SecurityPad::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( wParam )
	{
	case EGS_AUTH_SECOND_SECURITY_ACK:
		{
			return Handler_EGS_AUTH_SECOND_SECURITY_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case EGS_CREATE_SECOND_SECURITY_ACK:
		{
			return Handler_EGS_CREATE_SECOND_SECURITY_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case EGS_DELETE_SECOND_SECURITY_ACK:
		{
			return Handler_EGS_DELETE_SECOND_SECURITY_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case EGS_DELETE_SECOND_SECURITY_VERIFY_ACK:
		{
			return Handler_EGS_DELETE_SECOND_SECURITY_VERIFY_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case EGS_CHANGE_SECOND_SECURITY_PW_ACK:
		{
			return Handler_EGS_CHANGE_SECOND_SECURITY_PW_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case EGS_COMPARE_SECOND_SECURITY_PW_ACK:
		{
			return Handler_EGS_COMPARE_SECOND_SECURITY_PW_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case ECH_SECURITY_AUTH_ACK:
		{
			return Handler_ECH_SECURITY_AUTH_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	}

	return false;
}

bool CX2SecurityPad::Handler_ECH_SECURITY_AUTH_REQ()
{
	// 여긴 미국용 2차 보안 패드 알림 코드임
	KECH_SECURITY_AUTH_REQ kPacket;
	kPacket.m_iSecurityAuthType = KECH_SECURITY_AUTH_REQ::SAT_CHK_SECOND_SECURITY;
	kPacket.m_iUserUID = g_pData->GetMyUser()->GetUID();
	kPacket.m_wstrUserID = g_pInstanceData->GetUserID();

	KServerGroupInfo kInfo = g_pMain->GetPickedChannelServer();

	if( g_pData->GetServerProtocol()->ConnectedToChannelServer( kInfo.m_kServerIP.c_str(), kInfo.m_usMasterPort ) == true )
	{
		g_pData->GetServerProtocol()->SendChPacket( ECH_SECURITY_AUTH_REQ, kPacket );
		g_pMain->AddServerPacket( ECH_SECURITY_AUTH_ACK );
	}

	return true;
}

bool CX2SecurityPad::Handler_ECH_SECURITY_AUTH_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_SECURITY_AUTH_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( ECH_SECURITY_AUTH_ACK ) == true )
	{
		g_pData->GetServerProtocol()->DisconnectFromChannelServer();

		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// 인증 성공
			g_pMain->GetSecurityPad()->SetShow( true, CX2SecurityPad::SPS_CREATE_PASSWORD );
			return true;
		}
		else // 한가지 경우 뿐이므로 일단 코드를 정하지 않습니다.
		{
			// 질문 설정용 홈페이지 띄워주기
			x2::platform::OpenUrl( x2::platform::Utf8ToWide( kEvent.m_strSecondSecurityURL ) );
			return true;
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////
// EGS_AUTH_SECOND_SECURITY ///////////////////////////////////
bool CX2SecurityPad::Handler_EGS_AUTH_SECOND_SECURITY_REQ()  // Phase Count 1
{
	KEGS_AUTH_SECOND_SECURITY_REQ kPacket;

	kPacket.m_iUserUID = g_pData->GetMyUser()->GetUID();

	kPacket.m_wstrSecondPW = m_cPassword;
	g_pData->GetServerProtocol()->SendPacket( EGS_AUTH_SECOND_SECURITY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_AUTH_SECOND_SECURITY_ACK);
	return true;
}
bool CX2SecurityPad::Handler_EGS_AUTH_SECOND_SECURITY_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_AUTH_SECOND_SECURITY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_AUTH_SECOND_SECURITY_ACK ) == true )
	{
		if( kEvent.m_iOK == NetError::ERR_SECOND_SECURITY_AUTH_FAILED)
		{

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_REPLACED_STRING( ( STR_ID_12351, "i", kEvent.m_iFailedCount ) ), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );

			//창을 유지시킨채 계속 물어야한다.
			m_iPhase = 0;
			m_wstrPassword_Temp1.clear();
			m_wstrPassword_Temp2.clear();
			ClearInputPassword();
			UpdateUIMarble();
		}
		else if( kEvent.m_iOK == NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_12352 ), g_pMain->GetNowState(), CX2State::SUCM_EXIT, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
		}
		else if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{

			if ( m_wstrAutoAuthorityPassword.size() < 3 )
			{
				m_wstrAutoAuthorityPassword = m_cPassword;
				m_bIsAutoAuthorityPassworBegin = true;
			}

			// 인증 성공
			SetShow(false);
			CX2StateServerSelect* pStateServerSelect = (CX2StateServerSelect*)g_pMain->GetNowState();
			pStateServerSelect->SetEnableUnitSelect(true);



			return true;
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////
// EGS_CREATE_SECOND_SECURITY ///////////////////////////////////
bool CX2SecurityPad::Handler_EGS_CREATE_SECOND_SECURITY_REQ()  // Phase Count 2
{
	KEGS_CREATE_SECOND_SECURITY_REQ kPacket;
	kPacket.m_wstrSecondPW = m_cPassword;

	g_pData->GetServerProtocol()->SendPacket( EGS_CREATE_SECOND_SECURITY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CREATE_SECOND_SECURITY_ACK);

	return true;
}
bool CX2SecurityPad::Handler_EGS_CREATE_SECOND_SECURITY_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CREATE_SECOND_SECURITY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_CREATE_SECOND_SECURITY_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			SetUseSecondPW(true);

			m_wstrAutoAuthorityPassword = m_cPassword;
			m_bIsAutoAuthorityPassworBegin = true;
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING(STR_ID_12542), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );



			SetShow(false);
			return true;
		}
		else
		{
			SetShow(false);
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////
// EGS_DELETE_SECOND_SECURITY ///////////////////////////////////
bool CX2SecurityPad::Handler_EGS_DELETE_SECOND_SECURITY_REQ()  // Phase Count 1
{
	KEGS_DELETE_SECOND_SECURITY_REQ kPacket;
	kPacket.m_wstrSecondPW = m_cPassword;
	m_wstrPassword_Delete = m_cPassword;

	g_pData->GetServerProtocol()->SendPacket( EGS_DELETE_SECOND_SECURITY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DELETE_SECOND_SECURITY_ACK);

	return true;
}

bool CX2SecurityPad::Handler_EGS_DELETE_SECOND_SECURITY_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_DELETE_SECOND_SECURITY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_DELETE_SECOND_SECURITY_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ), GET_STRING( STR_ID_12543 ), SPUM_DELETE_PASSWORD_VERIFY_OK, g_pMain->GetNowState(), -1, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
			return true;
		}
		else
		{
			SetShow(false);
		}
	}
	return false;
}
bool CX2SecurityPad::Handler_EGS_DELETE_SECOND_SECURITY_VERIFY_REQ()  // Phase Count 1
{
	KEGS_DELETE_SECOND_SECURITY_VERIFY_REQ kPacket;
	kPacket.m_bDelete = true;
	kPacket.m_wstrSecondPW = m_wstrPassword_Delete;
	g_pData->GetServerProtocol()->SendPacket( EGS_DELETE_SECOND_SECURITY_VERIFY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DELETE_SECOND_SECURITY_VERIFY_ACK);

	return true;
}

bool CX2SecurityPad::Handler_EGS_DELETE_SECOND_SECURITY_VERIFY_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_DELETE_SECOND_SECURITY_VERIFY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_DELETE_SECOND_SECURITY_VERIFY_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			SetUseSecondPW(false);

			m_wstrAutoAuthorityPassword = L"";
			m_bIsAutoAuthorityPassworBegin = false;
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING(STR_ID_12544), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );



			SetShow(false);
			return true;
		}
		else
		{
			SetShow(false);
		}
	}
	return false;
}
/////////////////////////////////////////////////////////////////
// EGS_CHANGE_SECOND_SECURITY_PW_VERIFY ////////////////////////////////
bool CX2SecurityPad::Handler_EGS_COMPARE_SECOND_SECURITY_PW_REQ() // Phase Count 3
{
	KEGS_COMPARE_SECOND_SECURITY_PW_REQ kPacket;

	kPacket = m_wstrPassword_Temp1;

	g_pData->GetServerProtocol()->SendPacket( EGS_COMPARE_SECOND_SECURITY_PW_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_COMPARE_SECOND_SECURITY_PW_ACK);

	return true;
}

bool CX2SecurityPad::Handler_EGS_COMPARE_SECOND_SECURITY_PW_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_COMPARE_SECOND_SECURITY_PW_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_COMPARE_SECOND_SECURITY_PW_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			m_iPhase = 1;
			ClearInputPassword();
			UpdateUIMarble();

			ShuffleNumber();
		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_12545 ), g_pMain->GetNowState() );
			SetShow(false);
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////
// EGS_CHANGE_SECOND_SECURITY_PW ////////////////////////////////
bool CX2SecurityPad::Handler_EGS_CHANGE_SECOND_SECURITY_PW_REQ() // Phase Count 3
{
	KEGS_CHANGE_SECOND_SECURITY_PW_REQ kPacket;

	kPacket.m_wstrOldSecondPW = m_wstrPassword_Temp1;
	kPacket.m_wstrNewSecondPW = m_wstrPassword_Temp2;

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_SECOND_SECURITY_PW_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CHANGE_SECOND_SECURITY_PW_ACK);

	return true;
}

bool CX2SecurityPad::Handler_EGS_CHANGE_SECOND_SECURITY_PW_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_SECOND_SECURITY_PW_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_CHANGE_SECOND_SECURITY_PW_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			m_wstrAutoAuthorityPassword = m_cPassword;
			m_bIsAutoAuthorityPassworBegin = true;
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING(STR_ID_12545), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
			SetShow(false);
			return true;
		}
		else
		{
			SetShow(false);
			return true;
		}
	}
	return false;
}




void CX2SecurityPad::AutoAuthoritySecondSecurity ()
{
	KEGS_AUTH_SECOND_SECURITY_REQ kPacket;
	kPacket.m_iUserUID = g_pData->GetMyUser()->GetUID();
	kPacket.m_wstrSecondPW = m_wstrAutoAuthorityPassword;
	g_pData->GetServerProtocol()->SendPacket( EGS_AUTH_SECOND_SECURITY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_AUTH_SECOND_SECURITY_ACK);


}
