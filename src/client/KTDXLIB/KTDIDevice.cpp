#include "KTDIDevice.h"

#include "DXUT.h"
#include "SdlInput.h"

#include <algorithm>
#include <cstring>
#include <iterator>

#define DOUBLE_INPUT_GAP 0.4f

CKTDIDevice::CKTDIDevice()
{
	// Setup device to none
	m_Type = NONE;

	// Set windowed usage to TRUE
	m_Windowed = TRUE;

	m_kMousePrevPos.x = 0;
	m_kMousePrevPos.y = 0;

	m_kMouseDeltaPos.x = 0;
	m_kMouseDeltaPos.y = 0;

	for ( int i = 0; i < 256; ++i )
		m_Locks[i] = false;

	// Clear the device variables
	Clear();
}

CKTDIDevice::~CKTDIDevice()
{
	// Free a prior install
	Free();
}

// Generic functions - all devices
BOOL CKTDIDevice::Create( short Type, BOOL Windowed )
{
	// Free a prior device
	Free();

	switch(Type)
	{
	case KEYBOARD:
	case MOUSE:
		break;

	case JOYSTICK:
		// The original failed here without an attached joystick (KEM_ERROR54) and never looked again; the reader opens
		// the first joystick on each Read until one is there.
		m_pJoystick = std::make_unique<x2::dxut::JoystickReader>();
		break;

	default:
		return FALSE;
	}

	// Set the windowed usage
	m_Windowed = Windowed;

	// Set the device type
	m_Type = Type;

	// Clear the device information
	Clear();

	// Return a success
	return TRUE;
}
BOOL CKTDIDevice::Free()
{
	m_pJoystick.reset();

	// Set to no device installed
	m_Type = NONE;

	// Clear the data
	Clear();

	return TRUE;
}

BOOL CKTDIDevice::Clear()
{
	short i;

	std::memset( m_State, 0, sizeof( m_State ) );
	m_MouseState = DIMOUSESTATE{};
	m_JoystickState = x2::dxut::JoystickReader::Neutral();
	std::memset( m_ActionMap, 0, sizeof( m_ActionMap ) );

	for( i = 0; i < 256; i++ )
		m_Locks[i]  = FALSE;

	m_XPos = 0;
	m_YPos = 0;
	m_ZPos = 0;

	m_bExtraEnter = false;

	return TRUE;
}

BOOL CKTDIDevice::ReleaseAllButtonsAndKeys()
{
	std::memset( m_State, 0, sizeof( m_State ) );
	m_MouseState = DIMOUSESTATE{};
	m_JoystickState = x2::dxut::JoystickReader::Neutral();	// centered POV, as the original set rgdwPOV[0]

	for( short i = 0; i < 256; i++ )
		m_Locks[i]  = FALSE;

	m_bExtraEnter = false;

	return TRUE;
}

BOOL CKTDIDevice::Read( float fElapsedTime )
{
	short i;

	// Make sure device type if in range
	if(m_Type < 1 || m_Type > 3)
	{
		return FALSE;
	}

	// Foreground devices: without focus DirectInput could not acquire them and the read failed, keeping the last state.
	x2::dxut::InputState& input = DXUTGetInputState();
	if( !input.Focused() )
	{
		return FALSE;
	}

	switch(m_Type)
	{
	case KEYBOARD:
		std::memcpy( m_State, input.Keys(), sizeof( m_State ) );
		break;

	case MOUSE:
		m_MouseState = input.ReadMouse();
		break;

	case JOYSTICK:
		if( !m_pJoystick->Read( m_JoystickState ) )
		{
			return FALSE;
		}
		break;
	}

	// Since only the mouse coordinates are relative, you'll
	// have to deal with them now
	if(m_Type == MOUSE)
	{
		// If windowed usage, ask windows for coordinates
		if( m_Windowed == TRUE )
		{
			POINT pt = input.Cursor();
			m_XPos = pt.x;
			m_YPos = pt.y;
			m_ZPos = m_MouseState.lZ;	// mouse wheel
		}
		else
		{
			m_XPos += m_MouseState.lX;
			m_YPos += m_MouseState.lY;
			m_ZPos  = m_MouseState.lZ;	// mouse wheel
		}
	}

	// Released keys and button need to be unlocked
	switch(m_Type)
	{
		case KEYBOARD:
			for( i = 0; i < 256; i++ )
			{
				if( !( m_State[i] & 0x80 ) )
					m_Locks[i] = FALSE;

				if( m_DoubleKeyCheck[i].bFirstDown == true
					&& m_DoubleKeyCheck[i].bDoubleOK == false )
				{
					m_DoubleKeyCheck[i].fTimeOut -= fElapsedTime;
					if( m_DoubleKeyCheck[i].fTimeOut < 0.0f )
					{
						m_DoubleKeyCheck[i].Init();
					}
				}
			}
			break;

		case MOUSE:
			for( i = 0; i < 4; i++ )
			{
				if(!(m_MouseState.rgbButtons[i]))
					m_Locks[i] = FALSE;
			}
			break;

		case JOYSTICK:
			for( i = 0; i < 32; i++ )
			{
				if(!(m_JoystickState.rgbButtons[i]))
					m_Locks[i] = FALSE;
			}
			break;
	}

	// return a success
	return TRUE;
}
BOOL CKTDIDevice::Acquire( BOOL /*Active*/ )
{
	// SDL's state needs no acquisition; Read follows the window focus instead.
	return m_Type != NONE ? TRUE : FALSE;
}

BOOL CKTDIDevice::GetLock( unsigned char Num )
{
	return m_Locks[Num];
}
BOOL CKTDIDevice::SetLock( unsigned char Num, BOOL State )
{
	m_Locks[Num] = State;
	return TRUE;
}

long CKTDIDevice::GetXPos()
{
	// Update coordinates if a joystick
	if(m_Type == JOYSTICK)
		m_XPos = m_JoystickState.lX;

	return m_XPos;
}
BOOL CKTDIDevice::SetXPos( long XPos )
{
	m_XPos = XPos;
	return TRUE;
}
long CKTDIDevice::GetYPos()
{
	// Update coordinates if a joystick
	if(m_Type == JOYSTICK)
		m_YPos = m_JoystickState.lY;

	return m_YPos;
}
BOOL CKTDIDevice::SetYPos( long YPos )
{
	m_YPos = YPos;
	return TRUE;
}
long CKTDIDevice::GetZPos()
{
	// Update coordinates if a joystick
	if(m_Type == JOYSTICK)
		m_ZPos = m_JoystickState.lZ;

	return m_ZPos;
}
long CKTDIDevice::GetXDelta()
{
	switch(m_Type)
	{
	case MOUSE:
		return m_MouseState.lX;
		//return GetMouseDeltaPosition().x;

	case JOYSTICK:
		return m_JoystickState.lX - m_XPos;

	default:
		return 0;
	}
}
long CKTDIDevice::GetYDelta()
{
	switch(m_Type)
	{
	case MOUSE:
		return m_MouseState.lY;
		//return GetMouseDeltaPosition().y;

	case JOYSTICK:
		return m_JoystickState.lY - m_YPos;

	default:
		return 0;
	}
}

DWORD CKTDIDevice::GetPOV()
{
	if(m_Type != JOYSTICK)
		return static_cast<DWORD>( -1 );

	return m_JoystickState.rgdwPOV[0];
}


// Keyboard specific functions
BOOL  CKTDIDevice::GetKeyState( unsigned char Num )
{
	// Check if key/button is pressed
	if( m_State[Num] & 0x80 && m_Locks[Num] == FALSE )
	{
		m_Locks[Num] = TRUE;
		return TRUE;
	}

	return FALSE;
}
BOOL  CKTDIDevice::SetKeyState( unsigned char Num, BOOL State )
{
	m_State[Num] = State;
	return TRUE;
}
BOOL  CKTDIDevice::GetPureKeyState( unsigned char Num )
{
	return ((m_State[Num] & 0x80) ? TRUE : FALSE);
}

BOOL  CKTDIDevice::GetDoubleKeyPureState( unsigned char Num )
{
	if( m_DoubleKeyCheck[Num].bDoubleOK != true )
	{
		if( m_DoubleKeyCheck[Num].bFirstDown == false )
		{
			if( m_State[Num] & 0x80 && m_Locks[Num] == FALSE )
			{
				m_DoubleKeyCheck[Num].bFirstDown = true;
				m_DoubleKeyCheck[Num].fTimeOut = DOUBLE_INPUT_GAP;
			}
			return FALSE;

		}
		else
		{
			if( m_DoubleKeyCheck[Num].bKeyUp == false )
			{
				if( GetPureKeyState( Num ) == FALSE )
				{
					m_DoubleKeyCheck[Num].bKeyUp = true;
				}
				return FALSE;
			}
			else if( m_State[Num] & 0x80 && m_Locks[Num] == FALSE )
			{
				m_DoubleKeyCheck[Num].bDoubleOK = true;
			}
			else
				return FALSE;
		}
	}

	if( m_DoubleKeyCheck[Num].bDoubleOK == true
		&& GetPureKeyState( Num ) == TRUE )
		return TRUE;
	else
	{
		m_DoubleKeyCheck[Num].Init();
		return FALSE;
	}
}

BOOL  CKTDIDevice::GetDoubleKeyState( unsigned char Num )
{
	if( m_DoubleKeyCheck[Num].bDoubleOK != true )
	{
		if( m_DoubleKeyCheck[Num].bFirstDown == false )
		{
			if( m_State[Num] & 0x80 && m_Locks[Num] == FALSE )
			{
				m_DoubleKeyCheck[Num].bFirstDown = true;
				m_DoubleKeyCheck[Num].fTimeOut = DOUBLE_INPUT_GAP;
			}
			return FALSE;

		}
		else
		{
			if( m_DoubleKeyCheck[Num].bKeyUp == false )
			{
				if( GetPureKeyState( Num ) == FALSE )
				{
					m_DoubleKeyCheck[Num].bKeyUp = true;
				}
				return FALSE;
			}
			else if( m_State[Num] & 0x80 && m_Locks[Num] == FALSE )
			{
				m_DoubleKeyCheck[Num].bDoubleOK = true;
			}
			else
				return FALSE;
		}
	}

	if( m_DoubleKeyCheck[Num].bDoubleOK == true
		&& m_DoubleKeyCheck[Num].bDoubleKeyLock == false
		&& GetPureKeyState( Num ) == TRUE )
	{
		m_DoubleKeyCheck[Num].bDoubleKeyLock = true;
		return TRUE;
	}
	else
	{
		m_DoubleKeyCheck[Num].Init();
		return FALSE;
	}
}

void  CKTDIDevice::SetDoubleKeyReset( unsigned char Num )
{
	m_DoubleKeyCheck[Num].bDoubleOK			= false;
	m_DoubleKeyCheck[Num].bDoubleKeyLock	= false;
	m_DoubleKeyCheck[Num].bFirstDown		= false;
	m_DoubleKeyCheck[Num].bKeyUp			= false;
	m_DoubleKeyCheck[Num].fTimeOut			= 0.0f;
}

long  CKTDIDevice::GetNumKeyPresses()
{
	long i, Num = 0;

	for(i=0;i<256;i++)
	{
		if(m_State[i] & 0x80 && m_Locks[i] == FALSE)
			Num++;
	}

	return Num;
}
long  CKTDIDevice::GetNumPureKeyPresses()
{
	long i, Num = 0;

	for(i=0;i<256;i++)
	{
		if(m_State[i] & 0x80)
			Num++;
	}

	return Num;
}

// Mouse/Joystick specific functions
BOOL  CKTDIDevice::GetButtonState( unsigned char Num )
{
	char State = 0;

	if(m_Type == MOUSE)
		State = m_MouseState.rgbButtons[Num];

	if(m_Type == JOYSTICK)
		State = m_JoystickState.rgbButtons[Num];

	// Check if key/button is pressed
	if(State & 0x80 && m_Locks[Num] == FALSE)
	{
		m_Locks[Num] = TRUE;
		return TRUE;
	}

	return FALSE;
}
BOOL  CKTDIDevice::SetButtonState( unsigned char Num, BOOL State )
{
	if(m_Type == MOUSE)
	{
		m_MouseState.rgbButtons[Num] = State;
		return TRUE;
	}

	if(m_Type == JOYSTICK)
	{
		m_JoystickState.rgbButtons[Num] = State;
		return TRUE;
	}

	return FALSE;
}
BOOL  CKTDIDevice::GetPureButtonState( unsigned char Num )
{
	char State = 0;

	if(m_Type == MOUSE)
		State = m_MouseState.rgbButtons[Num];

	if(m_Type == JOYSTICK)
		State = m_JoystickState.rgbButtons[Num];

	if(State & 0x80)
	{
		return TRUE;
	}

	return FALSE;
}
long  CKTDIDevice::GetNumButtonPresses()
{
	long i, Num = 0;

	if(m_Type == MOUSE)
	{
		for(i=0;i<4;i++)
		{
			if(m_MouseState.rgbButtons[i] & 0x80 && m_Locks[i] == FALSE)
				Num++;
		}
	}
	else
	{
		if(m_Type == JOYSTICK)
		{
			for(i=0;i<32;i++)
			{
				if(m_JoystickState.rgbButtons[i] & 0x80 && m_Locks[i] == FALSE)
					Num++;
			}
		}
	}

	return Num;
}
long  CKTDIDevice::GetNumPureButtonPresses()
{
	long i, Num = 0;

	if(m_Type == MOUSE)
	{
		for(i=0;i<4;i++)
		{
			if(m_MouseState.rgbButtons[i] & 0x80)
				Num++;
		}
	}
	else
	{
		if(m_Type == JOYSTICK)
		{
			for(i=0;i<32;i++)
			{
				if(m_JoystickState.rgbButtons[i] & 0x80)
					Num++;
			}
		}
	}

	return Num;
}

void CKTDIDevice::UpdateMouseDeltaPosition()
{
	POINT pt = DXUTGetInputState().Cursor();

	m_kMouseDeltaPos.x = pt.x - m_kMousePrevPos.x;
	m_kMouseDeltaPos.y = pt.y - m_kMousePrevPos.y;

	m_kMousePrevPos = pt;
}

POINT CKTDIDevice::GetMouseDeltaPosition()
{
	return m_kMouseDeltaPos;
}

bool CKTDIDevice::GetActionState( int Action, long* /*lpData*/ )
{
	if( m_Type == JOYSTICK )
	{
		switch( m_ActionMap[Action] )
		{
		case JB_UP:
			{
				if( GetYPos() < -AXIS_THRESHOLD || GetPOV() == 31500 || GetPOV() == 0 || GetPOV() == 4500 )
					return true;
			}
			return false;
		case JB_DOWN:
			{
				if( GetYPos() > AXIS_THRESHOLD || GetPOV() == 13500 || GetPOV() == 18000 || GetPOV() == 22500 )
					return true;
			}
			return false;
		case JB_LEFT:
			{
				if( GetXPos() < -AXIS_THRESHOLD || GetPOV() == 22500 || GetPOV() == 27000 || GetPOV() == 31500 )
					return true;
			}
			return false;
		case JB_RIGHT:
			{
				if( GetXPos() > AXIS_THRESHOLD || GetPOV() == 4500 || GetPOV() == 9000 || GetPOV() == 13500 )
					return true;
			}
			return false;
		case JB_LEFT_TRIGGER:
			{
				if( GetZPos() > AXIS_THRESHOLD_TRIGGER_LEFT )
					return true;
			}
			return false;
		case JB_RIGHT_TRIGGER:
			{
				if( GetZPos() < AXIS_THRESHOLD_TRIGGER_RIGHT )
					return true;
			}
			return false;
		case 0:
			return false;
		default:
			break;
		}
	}

	if( m_Type == JOYSTICK )
		return JoystickByte( m_ActionMap[Action] ) ? true : false;

	return (m_State[m_ActionMap[Action]] ? true : false);
}

// A byte of the joystick state at a DIJOFS_* offset, which is how the joystick action map refers to buttons (0 past
// the end of DIJOYSTATE). The original read these bytes from m_State, which held the DIJOYSTATE.
char CKTDIDevice::JoystickByte( unsigned char Offset ) const
{
	if( Offset >= sizeof( DIJOYSTATE ) )
		return 0;

	char Byte;
	std::memcpy( &Byte, reinterpret_cast<const unsigned char*>( &m_JoystickState ) + Offset, 1 );
	return Byte;
}

void CKTDIDevice::SetAction( int Action, unsigned char Key )
{
	for(int i = 0; i < GAMEACTION_END; ++i)
	{
		if(i == Action)
			continue;
		if(m_ActionMap[i] == Key)
		{
			switch(m_Type)
			{
			case JOYSTICK:
				m_ActionMap[i] = DIK_UP;
				break;
			case KEYBOARD :
				m_ActionMap[i] = 0;
				break;
			}
		}
	}
	m_ActionMap[Action] = Key;
	return;
}

unsigned char CKTDIDevice::GetActionKey( int Action )
{
	return m_ActionMap[Action];
}

bool CKTDIDevice::ListenDeviceInput( unsigned char& ucoutUserInput )
{
	switch( m_Type )
	{
	case KEYBOARD:
		{
			for( int iKeyIndex = 0; iKeyIndex < 256; ++iKeyIndex )
			{
				if( GetKeyState( iKeyIndex ) == TRUE )
				{
					ucoutUserInput = ( unsigned char )iKeyIndex;
					return true;
				}
			}
		} return false;
	case JOYSTICK:
		{
			for( int iButtonIndex = 0; iButtonIndex < 32; ++iButtonIndex )		// 32: buttons of DIJOYSTATE
			{
				if( GetButtonState( iButtonIndex ) == TRUE )
				{
					ucoutUserInput = ( unsigned char )DIJOFS_BUTTON( iButtonIndex );
					return true;
				}
			}

			if( GetZPos() > AXIS_THRESHOLD_TRIGGER_LEFT )
			{
				ucoutUserInput = (unsigned char)JB_LEFT_TRIGGER;
				return true;
			}
			if( GetZPos() < AXIS_THRESHOLD_TRIGGER_RIGHT && GetZPos() != 0 )
			{
				ucoutUserInput = (unsigned char)JB_RIGHT_TRIGGER;
				return true;
			}
			if( GetXPos() < -AXIS_THRESHOLD || GetPOV() == 22500 || GetPOV() == 27000 || GetPOV() == 31500 )
			{
				ucoutUserInput = (unsigned char)JB_LEFT;
				return true;
			}
			if( GetXPos() > AXIS_THRESHOLD || GetPOV() == 4500 || GetPOV() == 9000 || GetPOV() == 13500 )
			{
				ucoutUserInput = (unsigned char)JB_RIGHT;
				return true;
			}
			if( GetYPos() < -AXIS_THRESHOLD || GetPOV() == 31500 || GetPOV() == 0 || GetPOV() == 4500 )
			{
				ucoutUserInput = (unsigned char)JB_UP;
				return true;
			}
			if( GetYPos() > AXIS_THRESHOLD || GetPOV() == 13500 || GetPOV() == 18000 || GetPOV() == 22500 )
			{
				ucoutUserInput = (unsigned char)JB_DOWN;
				return true;
			}
		} return false;

	default:
		return false;
	}
}
void CKTDIDevice::GetActionKeys( OUT std::map<short, short>& mapMappingInfo )
{
	for( short sGameAction = 0; sGameAction < GAMEACTION_END; ++sGameAction )
	{
		mapMappingInfo[ sGameAction ]	= m_ActionMap[ sGameAction ];
	}
}
void CKTDIDevice::SetActionKeys( IN const std::map<short, short>& mapMappingInfo )
{
	for( const std::pair< const short, short >& pAction : mapMappingInfo )
	{
		m_ActionMap[ pAction.first ] = static_cast<unsigned char>( pAction.second );
	}
}
