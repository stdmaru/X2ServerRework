#pragma once

// One input device (keyboard, mouse or joystick) with the original CKTDIDevice API. The original read DirectInput 8
// devices in immediate mode (foreground, non-exclusive); the port reads the same state from SDL3 through the DXUT
// replacement (DXUT/SdlInput.h). See docs/porting/client-input.md.

#include "x2/platform/win32.h"

#include "KTDIGameAction.h"

#include <map>
#include <memory>

namespace x2::dxut {
class JoystickReader;
}

///////////////////////////////////////////////////////////////////////
// Mouse and Joystick definitions
///////////////////////////////////////////////////////////////////////

#define AXIS_THRESHOLD_TRIGGER_LEFT  50559
#define AXIS_THRESHOLD_TRIGGER_RIGHT  1433

#define AXIS_THRESHOLD					500

#define BUTTON_RELEASED    FALSE
#define BUTTON_PRESSED      TRUE

#define BUTTON_UNLOCKED    FALSE
#define BUTTON_LOCKED       TRUE

#define MOUSE_LBUTTON          0
#define MOUSE_RBUTTON          1
#define MOUSE_MBUTTON          2

#define JOYSTICK_BUTTON0       0
#define JOYSTICK_BUTTON1       1
#define JOYSTICK_BUTTON2       2
#define JOYSTICK_BUTTON3       3
#define JOYSTICK_BUTTON4       4
#define JOYSTICK_BUTTON5       5

// Enumerated list of of device types
enum InputDevices
{
	NONE = 0,
	KEYBOARD,
	MOUSE,
	JOYSTICK
};

class CKTDIDevice
{
	public:
		struct DoubleKey
		{
			bool	bFirstDown;
			bool	bKeyUp;
			float	fTimeOut;

			bool	bDoubleOK;
			bool	bDoubleKeyLock;

			void Init()
			{
				bFirstDown		= false;
				bKeyUp			= false;
				fTimeOut		= 0.0f;

				bDoubleOK		= false;
				bDoubleKeyLock	= false;
			}

			DoubleKey()
			{
				Init();
			}
		};

	public:
		CKTDIDevice();
		virtual ~CKTDIDevice();
		CKTDIDevice( const CKTDIDevice& ) = delete;
		CKTDIDevice& operator=( const CKTDIDevice& ) = delete;

		// Generic functions - all devices
		BOOL Create( short Type, BOOL Windowed = TRUE );
		BOOL Free();

		BOOL Clear();
		BOOL Read( float fElapsedTime );
		BOOL Acquire( BOOL Active = TRUE );

		BOOL GetLock( unsigned char Num );
		BOOL SetLock( unsigned char Num, BOOL State = TRUE );
		BOOL GetExtraEnter() { return m_bExtraEnter; }
		void SetExtraEnter( bool val ) { m_bExtraEnter = val; }

		long GetXPos();
		BOOL SetXPos( long XPos );
		long GetYPos();
		BOOL SetYPos( long YPos );
		long GetZPos();	// mouse wheel
		long GetXDelta();
		long GetYDelta();
		DWORD GetPOV();

		// Keyboard specific functions
		BOOL  GetKeyState( unsigned char Num );
		BOOL  SetKeyState( unsigned char Num, BOOL State );
		BOOL  GetPureKeyState( unsigned char Num );
		BOOL  GetDoubleKeyPureState( unsigned char Num );
		BOOL  GetDoubleKeyState( unsigned char Num );
		void  SetDoubleKeyReset( unsigned char Num );
		long  GetNumKeyPresses();
		long  GetNumPureKeyPresses();

		// Mouse/Joystick specific functions
		BOOL  GetButtonState( unsigned char Num );
		BOOL  SetButtonState( unsigned char Num, BOOL State );
		BOOL  GetPureButtonState( unsigned char Num );
		long  GetNumButtonPresses();
		long  GetNumPureButtonPresses();

		void	UpdateMouseDeltaPosition();
		POINT	GetMouseDeltaPosition();

		bool GetActionState( int Action, long* lpData = NULL);
		void SetAction( int Action, unsigned char Key );
		unsigned char GetActionKey( int Action );
		bool ListenDeviceInput( unsigned char& ucoutUserInput );
		void GetActionKeys( OUT std::map<short, short>& mapMappingInfo );
		void SetActionKeys( IN const std::map<short, short>& mapMappingInfo );


		BOOL	ReleaseAllButtonsAndKeys();

	public:
		short                 m_Type;

		BOOL                  m_Windowed;

		// GetDeviceState buffers: keyboard (DIK_* index, 0x80 when down), mouse and joystick. The original aliased the
		// three on m_State; the joystick action map still indexes the bytes of DIJOYSTATE (JoystickByte).
		char                  m_State[256];
		DIMOUSESTATE          m_MouseState;
		DIJOYSTATE            m_JoystickState;

		BOOL                  m_Locks[256];

		unsigned char		  m_ActionMap[GAMEACTION_END];

		POINT				  m_kMousePrevPos;
		POINT				  m_kMouseDeltaPos;

		// Z is the mouse wheel: one notch is 120, +120 pushed away and -120 pulled back.
		long                  m_XPos, m_YPos, m_ZPos;

		// The IME's Enter lock keeps Enter from reaching the chat box, so the chat box has its own flag.
		BOOL m_bExtraEnter;

	private:
		char			JoystickByte( unsigned char Offset ) const;

		std::unique_ptr<x2::dxut::JoystickReader>	m_pJoystick;
		DoubleKey		m_DoubleKeyCheck[256];

};
