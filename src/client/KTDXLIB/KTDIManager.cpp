#include "KTDIManager.h"

#include <algorithm>
#include <iterator>

#define DOUBLE_INPUT_GAP 0.4f

CKTDIManager::CKTDIManager( HWND /*hWnd*/, HINSTANCE /*hInst*/ )
{
	m_KTDIKeyboard	= NULL;
	m_KTDIMouse		= NULL;
	m_KTDIJoystic	= NULL;

	m_bEnabled		= true;

	m_bJoyEnable	= false;
	m_bKeyChanging = false;
	std::fill( std::begin( m_State ), std::end( m_State ), false );
	std::fill( std::begin( m_Locks ), std::end( m_Locks ), false );	// left uninitialized by the original

	m_bActivated = false;
}

CKTDIManager::~CKTDIManager()
{
	DestroyKeyboard();
	DestroyMouse();
	DestroyJoystic();
}

HRESULT CKTDIManager::OnFrameMove( double /*fTime*/, float fElapsedTime )
{
	UpdateData( fElapsedTime );

	return S_OK;
}

void CKTDIManager::CreateKeyboard()
{
	DestroyKeyboard();
	m_KTDIKeyboard = new CKTDIDevice();
	m_KTDIKeyboard->Create( KEYBOARD );
}

void CKTDIManager::CreateMouse( BOOL windowed )
{
	DestroyMouse();
	m_KTDIMouse = new CKTDIDevice();
	m_KTDIMouse->Create( MOUSE, windowed );
}

void CKTDIManager::CreateJoystic()
{
	DestroyJoystic();
	m_KTDIJoystic = new CKTDIDevice();
	m_KTDIJoystic->Create( JOYSTICK );
}

void CKTDIManager::DestroyKeyboard()
{
	delete m_KTDIKeyboard;
	m_KTDIKeyboard = NULL;
}

void CKTDIManager::DestroyMouse()
{
	delete m_KTDIMouse;
	m_KTDIMouse = NULL;
}

void CKTDIManager::DestroyJoystic()
{
	delete m_KTDIJoystic;
	m_KTDIJoystic = NULL;
}

void CKTDIManager::UpdateData( float fElapsedTime )
{

	if( m_bEnabled )
	{
		std::fill( std::begin( m_State ), std::end( m_State ), false );

		if( m_KTDIKeyboard != NULL )
		{
			m_KTDIKeyboard->Read( fElapsedTime );
		}

		if( m_KTDIMouse != NULL )
		{
			m_KTDIMouse->Read( fElapsedTime );

			// Update Mouse Delta Position
			m_KTDIMouse->UpdateMouseDeltaPosition();
		}

		if( m_KTDIJoystic != NULL )
			m_KTDIJoystic->Read( fElapsedTime );

		for(int i=0; i<GAMEACTION_TOTAL; ++i)
		{
			if( i == GAMEACTION_END )
				continue;

			// joystick
			if( m_bJoyEnable && m_KTDIJoystic != NULL )
			{
				bool bKeyChanging = false;
				if( true == m_KTDIJoystic->GetActionState(i) && m_bKeyChanging == true )
				{
					bKeyChanging = true;
				}

				if( bKeyChanging == true )
				{
				}
				else
				{
					HandleAction( i, m_KTDIJoystic->GetActionState( i ) );
				}
			}

			// keyboard
			if (m_KTDIKeyboard != NULL )
			{
				HandleAction(i, m_KTDIKeyboard->GetActionState(i) );
			}
		}

		for(int i=0; i<GAMEACTION_END; ++i)
		{
			ApplyActionInput((GAME_ACTION)i, fElapsedTime );
		}

	}
}

BOOL  CKTDIManager::GetActionState( GAME_ACTION Action )
{
	// Check if key/button is pressed
	if( m_State[Action] && m_Locks[Action] == false )
	{
		m_Locks[Action] = true;
		return TRUE;
	}

	return FALSE;
}
BOOL  CKTDIManager::SetActionState( GAME_ACTION Action, BOOL State )
{
	if( State == TRUE )
		m_State[Action] = true;
	else
		m_State[Action] = false | m_State[Action];		// keep true: the state is reset every frame and one action may be set several times

	return TRUE;
}


BOOL  CKTDIManager::GetPureActionState( GAME_ACTION Action )
{
	return (m_State[Action]) ? TRUE : FALSE;
}
BOOL  CKTDIManager::GetDoubleActionPureState( GAME_ACTION Action )
{
	if( m_DoubleKeyCheck[Action].bDoubleOK != true )
	{
		if( m_DoubleKeyCheck[Action].bFirstDown == false )
		{
			if( m_State[Action] && m_Locks[Action] == false )
			{
				m_DoubleKeyCheck[Action].bFirstDown = true;
				m_DoubleKeyCheck[Action].fTimeOut = DOUBLE_INPUT_GAP;
			}
			return FALSE;

		}
		else
		{
			if( m_DoubleKeyCheck[Action].bKeyUp == false )
			{
				if( GetPureActionState( Action ) == FALSE )
				{
					m_DoubleKeyCheck[Action].bKeyUp = true;
				}
				return FALSE;
			}
			else if( m_State[Action] && m_Locks[Action] == false )
			{
				m_DoubleKeyCheck[Action].bDoubleOK = true;
			}
			else
				return FALSE;
		}
	}

	if( m_DoubleKeyCheck[Action].bDoubleOK == true
		&& GetPureActionState( Action ) == TRUE )
		return TRUE;
	else
	{
		m_DoubleKeyCheck[Action].Init();
		return FALSE;
	}

}
BOOL  CKTDIManager::GetDoubleActionState( GAME_ACTION Action )
{
	if( m_DoubleKeyCheck[Action].bDoubleOK != true )
	{
		if( m_DoubleKeyCheck[Action].bFirstDown == false )
		{
			if( m_State[Action] && m_Locks[Action] == false )
			{
				m_DoubleKeyCheck[Action].bFirstDown = true;
				m_DoubleKeyCheck[Action].fTimeOut = DOUBLE_INPUT_GAP;
			}
			return FALSE;

		}
		else
		{
			if( m_DoubleKeyCheck[Action].bKeyUp == false )
			{
				if( GetPureActionState( Action ) == FALSE )
				{
					m_DoubleKeyCheck[Action].bKeyUp = true;
				}
				return FALSE;
			}
			else if( m_State[Action] && m_Locks[Action] == false )
			{
				m_DoubleKeyCheck[Action].bDoubleOK = true;
			}
			else
				return FALSE;
		}
	}

	if( m_DoubleKeyCheck[Action].bDoubleOK == true
		&& m_DoubleKeyCheck[Action].bDoubleKeyLock == false
		&& GetPureActionState( Action ) == TRUE )
	{
		m_DoubleKeyCheck[Action].bDoubleKeyLock = true;
		return TRUE;
	}
	else
	{
		m_DoubleKeyCheck[Action].Init();
		return FALSE;
	}

}
void CKTDIManager::SetDoubleActionReset( GAME_ACTION Action )
{
	m_DoubleKeyCheck[Action].bDoubleOK			= false;
	m_DoubleKeyCheck[Action].bDoubleKeyLock	= false;
	m_DoubleKeyCheck[Action].bFirstDown		= false;
	m_DoubleKeyCheck[Action].bKeyUp			= false;
	m_DoubleKeyCheck[Action].fTimeOut			= 0.0f;
}

BOOL CKTDIManager::GetLock( GAME_ACTION Action )
{
	if( m_Locks[Action] )
		return TRUE;
	else
		return FALSE;

}
BOOL CKTDIManager::SetLock( GAME_ACTION Action, BOOL State )
{
	if(State == TRUE)
		m_Locks[Action] = true;
	else
		m_Locks[Action] = false;

	return TRUE;
}

void CKTDIManager::HandleAction(UINT nAction, bool bState, long /*lData*/ )
{
	SetActionState((GAME_ACTION)nAction, bState);
}

void CKTDIManager::ApplyActionInput( GAME_ACTION Action, float fElapsedTime )
{
	if( !m_State[Action] )
		m_Locks[Action] = false;

	if( m_DoubleKeyCheck[Action].bFirstDown == true
		&& m_DoubleKeyCheck[Action].bDoubleOK == false )
	{
		m_DoubleKeyCheck[Action].fTimeOut -= fElapsedTime;
		if( m_DoubleKeyCheck[Action].fTimeOut < 0.0f )
		{
			m_DoubleKeyCheck[Action].Init();
		}
	}

}

bool CKTDIManager::ListenDeviceInput( unsigned char& ucoutUserInput, InputDevices eType )
{
	switch(eType)
	{
	case KEYBOARD:
		{
			if(m_KTDIKeyboard != NULL )
			{
				return m_KTDIKeyboard->ListenDeviceInput( ucoutUserInput );
			}
		}
		break;
	case MOUSE:
		break;
	case JOYSTICK:
		{
			if( m_KTDIJoystic != NULL )
			{
				return m_KTDIJoystic->ListenDeviceInput( ucoutUserInput );
			}
		}
		break;
	case NONE:
		break;
	}
	return false;
}

void CKTDIManager::SetKeyChanging( bool bKeyChanging )
{
	m_bKeyChanging = bKeyChanging;
}
void CKTDIManager::SetGamePadDefaultMap()
{
	// Default Mapping
	if(m_KTDIJoystic != NULL)
	{
		m_KTDIJoystic->SetAction( GAMEACTION_UP,					JB_UP );
		m_KTDIJoystic->SetAction( GAMEACTION_DOWN,					JB_DOWN );
		m_KTDIJoystic->SetAction( GAMEACTION_LEFT,					JB_LEFT );
		m_KTDIJoystic->SetAction( GAMEACTION_RIGHT,					JB_RIGHT );

		m_KTDIJoystic->SetAction( GAMEACTION_ATTACK_FAST,			JB_BUTTON2 );
		m_KTDIJoystic->SetAction( GAMEACTION_ATTACK_STRONG,			JB_BUTTON3 );
		m_KTDIJoystic->SetAction( GAMEACTION_SKILL1,				JB_BUTTON0 );
		m_KTDIJoystic->SetAction( GAMEACTION_SKILL2,				JB_BUTTON1 );
		m_KTDIJoystic->SetAction( GAMEACTION_SKILL3,				JB_BUTTON6 );
		m_KTDIJoystic->SetAction( GAMEACTION_SKILL4,				JB_BUTTON4 );
		m_KTDIJoystic->SetAction( GAMEACTION_AWAKE,					JB_BUTTON5 );
		m_KTDIJoystic->SetAction( GAMEACTION_SLOT_CHANGE,			JB_BUTTON7 );

		m_KTDIJoystic->SetAction( GAMEACTION_RETURN,				JB_BUTTON9 );
		m_KTDIJoystic->SetAction( GAMEACTION_ESCAPE,				JB_BUTTON8 );
		m_KTDIJoystic->SetAction( GAMEACTION_PARTYREADY,			JB_BUTTON10 );

		m_KTDIJoystic->SetAction( GAMEACTION_QUICKSLOT1,			JB_BUTTON11 );
		m_KTDIJoystic->SetAction( GAMEACTION_QUICKSLOT2,			JB_BUTTON12 );
		m_KTDIJoystic->SetAction( GAMEACTION_QUICKSLOT3,			JB_BUTTON13 );
		m_KTDIJoystic->SetAction( GAMEACTION_QUICKSLOT4,			JB_BUTTON14 );
		m_KTDIJoystic->SetAction( GAMEACTION_QUICKSLOT5,			JB_BUTTON15 );
		m_KTDIJoystic->SetAction( GAMEACTION_QUICKSLOT6,			JB_BUTTON16 );
	}
}

void CKTDIManager::SetDefaultMap()
{
	// Default Mapping

	if( m_KTDIKeyboard != NULL )
	{
		m_KTDIKeyboard->SetAction( GAMEACTION_ATTACK_FAST, DIK_Z );
		m_KTDIKeyboard->SetAction( GAMEACTION_ATTACK_STRONG, DIK_X );

		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL1, DIK_A );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL2, DIK_S );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL3, DIK_D );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL4, DIK_C );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL_EX1, DIK_Q );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL_EX2, DIK_W );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL_EX3, DIK_E );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL_EX4, DIK_R );

		//m_KTDIKeyboard->SetAction( GAMEACTION_SCREENSHOT_MODE, DIK_DELETE );
		m_KTDIKeyboard->SetAction( GAMEACTION_SLOT_CHANGE, DIK_SPACE );
		m_KTDIKeyboard->SetAction( GAMEACTION_PVE, DIK_LBRACKET );
		m_KTDIKeyboard->SetAction( GAMEACTION_PVP, DIK_RBRACKET );
		m_KTDIKeyboard->SetAction( GAMEACTION_ZOOMIN, DIK_EQUALS );
		m_KTDIKeyboard->SetAction( GAMEACTION_ZOOMOUT, DIK_MINUS );
		m_KTDIKeyboard->SetAction( GAMEACTION_QUEST, DIK_L );
		m_KTDIKeyboard->SetAction( GAMEACTION_MAP, DIK_M );
		m_KTDIKeyboard->SetAction( GAMEACTION_COMMUNITY, DIK_T );
		m_KTDIKeyboard->SetAction( GAMEACTION_INFO, DIK_U );
		m_KTDIKeyboard->SetAction( GAMEACTION_INVENTORY, DIK_I );
		m_KTDIKeyboard->SetAction( GAMEACTION_OPTION, DIK_O );
		m_KTDIKeyboard->SetAction( GAMEACTION_PET, DIK_P );
		m_KTDIKeyboard->SetAction( GAMEACTION_GUILD, DIK_G );
		m_KTDIKeyboard->SetAction( GAMEACTION_SKILL_TREE, DIK_K );
		m_KTDIKeyboard->SetAction( GAMEACTION_NAME, DIK_N );
		m_KTDIKeyboard->SetAction( GAMEACTION_FRIEND, DIK_Y );
		m_KTDIKeyboard->SetAction( GAMEACTION_AWAKE, DIK_LCONTROL );

		m_KTDIKeyboard->SetAction( GAMEACTION_QUICKSLOT1, DIK_1 );
		m_KTDIKeyboard->SetAction( GAMEACTION_QUICKSLOT2, DIK_2 );
		m_KTDIKeyboard->SetAction( GAMEACTION_QUICKSLOT3, DIK_3 );
		m_KTDIKeyboard->SetAction( GAMEACTION_QUICKSLOT4, DIK_4 );
		m_KTDIKeyboard->SetAction( GAMEACTION_QUICKSLOT5, DIK_5 );
		m_KTDIKeyboard->SetAction( GAMEACTION_QUICKSLOT6, DIK_6 );

		m_KTDIKeyboard->SetAction( GAMEACTION_UP,		DIK_UP );
		m_KTDIKeyboard->SetAction( GAMEACTION_DOWN,		DIK_DOWN );
		m_KTDIKeyboard->SetAction( GAMEACTION_LEFT,		DIK_LEFT );
		m_KTDIKeyboard->SetAction( GAMEACTION_RIGHT,	DIK_RIGHT );
		m_KTDIKeyboard->SetAction( GAMEACTION_SIT,		DIK_V );

		m_KTDIKeyboard->SetAction( GAMEACTION_RIDING,		DIK_F );
		m_KTDIKeyboard->SetAction( GAMEACTION_RELATION,		DIK_H );
		m_KTDIKeyboard->SetAction( GAMEACTION_WARP,		DIK_B );

	}

}




bool CKTDIManager::IsActivated()
{
	return m_bActivated;
}
void CKTDIManager::ActivateApp()
{
	m_bActivated = true;
}
void CKTDIManager::DeactivateApp()
{
	m_bActivated = false;

	if ( m_KTDIKeyboard != NULL )
		m_KTDIKeyboard->ReleaseAllButtonsAndKeys();

	if ( m_KTDIMouse != NULL )
		m_KTDIMouse->ReleaseAllButtonsAndKeys();

	if ( m_KTDIJoystic != NULL )
		m_KTDIJoystic->ReleaseAllButtonsAndKeys();

}
