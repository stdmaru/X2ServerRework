#pragma once

// Input manager: reads the keyboard, mouse and joystick devices each frame and turns them into game actions through the
// action map. DirectInput is replaced by SDL3 state (DXUT/SdlInput.h); the API is the original's. See
// docs/porting/client-input.md.

#include "x2/platform/win32.h"

#include "KTDIGameAction.h"
#include "KTDXStage.h"

#include "KTDIDevice.h"

//////////////////////////////////////////////////////////////////////////
// 2009-10-21, KEY_MAPPING: outside CKTDIManager, avoid reaching the keyboard directly through Getkeyboard(); go through
// the manager, which reads input through the action map so that keys can be configured. For compatibility with the older
// code, calls go through these macros.
//////////////////////////////////////////////////////////////////////////

// Mapping: define the GAME_ACTION of each function.
#define GET_KEY_STATE( action_id )				g_pKTDXApp->GetDIManager()->GetActionState( action_id )
#define GET_DOUBLEKEY_STATE( action_id )		g_pKTDXApp->GetDIManager()->GetDoubleActionState( action_id )
#define GET_DOUBLEKEYPURE_STATE( action_id )	g_pKTDXApp->GetDIManager()->GetDoubleActionPureState( action_id )
#define GET_PUREKEY_STATE( action_id )			g_pKTDXApp->GetDIManager()->GetPureActionState( action_id )
#define SET_KEYLOCK( action_id , bstate )		g_pKTDXApp->GetDIManager()->SetLock( action_id , bstate )
// Game Actions

#define GA_ATTACK_FAST			GAMEACTION_ATTACK_FAST
#define GA_ATTACK_STRONG		GAMEACTION_ATTACK_STRONG

#define GA_SKILL1				GAMEACTION_SKILL1
#define GA_SKILL2				GAMEACTION_SKILL2
#define GA_SKILL3				GAMEACTION_SKILL3
#define GA_SKILL4				GAMEACTION_SKILL4
#define GA_SKILL_EX1			GAMEACTION_SKILL_EX1
#define GA_SKILL_EX2			GAMEACTION_SKILL_EX2
#define GA_SKILL_EX3			GAMEACTION_SKILL_EX3
#define GA_SKILL_EX4			GAMEACTION_SKILL_EX4

//#define GA_SCREENSHOT_MODE		GAMEACTION_SCREENSHOT_MODE
#define GA_SLOT_CHANGE			GAMEACTION_SLOT_CHANGE
#define GA_PVE					GAMEACTION_PVE
#define GA_PVP					GAMEACTION_PVP
#define GA_ZOOMIN				GAMEACTION_ZOOMIN
#define GA_ZOOMOUT				GAMEACTION_ZOOMOUT
#define GA_QUEST				GAMEACTION_QUEST
#define GA_MAP					GAMEACTION_MAP
#define GA_COMMUNITY			GAMEACTION_COMMUNITY
#define GA_INFO					GAMEACTION_INFO
#define GA_INVENTORY			GAMEACTION_INVENTORY
#define GA_OPTION				GAMEACTION_OPTION
#define GA_PET					GAMEACTION_PET
#define GA_GUILD				GAMEACTION_GUILD
#define GA_SKILL_TREE			GAMEACTION_SKILL_TREE
#define GA_NAME					GAMEACTION_NAME
#define GA_FRIEND				GAMEACTION_FRIEND
#define GA_AWAKE				GAMEACTION_AWAKE

#define GA_QUICKSLOT1			GAMEACTION_QUICKSLOT1
#define GA_QUICKSLOT2			GAMEACTION_QUICKSLOT2
#define GA_QUICKSLOT3			GAMEACTION_QUICKSLOT3
#define GA_QUICKSLOT4			GAMEACTION_QUICKSLOT4
#define GA_QUICKSLOT5			GAMEACTION_QUICKSLOT5
#define GA_QUICKSLOT6			GAMEACTION_QUICKSLOT6

#define GA_UP					GAMEACTION_UP
#define GA_DOWN					GAMEACTION_DOWN
#define GA_LEFT					GAMEACTION_LEFT
#define GA_RIGHT				GAMEACTION_RIGHT

#define GA_SIT					GAMEACTION_SIT

#define GA_RIDING				GAMEACTION_RIDING

#define GA_RELATION				GAMEACTION_RELATION			/// couple/solo community window (marriage system)

#define GA_WARP					GAMEACTION_WARP			// SERV_ADD_WARP_BUTTON

#define GA_RETURN				GAMEACTION_RETURN
#define GA_ESCAPE				GAMEACTION_ESCAPE
#define GA_PARTYREADY			GAMEACTION_PARTYREADY





class CKTDIManager : public CKTDXStage
{
private:
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
		CKTDIManager( HWND hWnd, HINSTANCE hInst );	// the window and instance DirectInput needed; unused now
		~CKTDIManager() override;
		CKTDIManager( const CKTDIManager& ) = delete;
		CKTDIManager& operator=( const CKTDIManager& ) = delete;

		HRESULT OnFrameMove( double fTime, float fElapsedTime ) override;

		void CreateKeyboard();
		void CreateMouse( BOOL windowed );
		void CreateJoystic();

		void DestroyKeyboard();
		void DestroyMouse();
		void DestroyJoystic();

		void SetEnable( bool bEnabled ){ m_bEnabled = bEnabled; }

		CKTDIDevice* Getkeyboard(){ return m_KTDIKeyboard; }
		CKTDIDevice* GetMouse(){ return m_KTDIMouse; }
		CKTDIDevice* GetJoystic(){ return m_KTDIJoystic; }

		void SetDefaultMap();

		BOOL  GetActionState( GAME_ACTION Action );
		BOOL  SetActionState( GAME_ACTION Action, BOOL State );
		BOOL  GetPureActionState( GAME_ACTION Action );
		BOOL  GetDoubleActionPureState( GAME_ACTION Action );
		BOOL  GetDoubleActionState( GAME_ACTION Action );
		void  SetDoubleActionReset( GAME_ACTION Action );

		void SetEnableJoyStick( bool bEnabled ){ m_bJoyEnable = bEnabled; }
		bool GetEnableJoyStick(){ return m_bJoyEnable; }

		BOOL GetLock( GAME_ACTION Action );
		BOOL SetLock( GAME_ACTION Action, BOOL State = TRUE );

		void HandleAction(UINT nAction, bool bState, long lData = 0);

		void ApplyActionInput( GAME_ACTION Action, float fElapsedTime );

		bool ListenDeviceInput( unsigned char& ucoutUserInput, InputDevices eType );

		void SetKeyChanging( bool bKeyChanging );
		void SetGamePadDefaultMap();



		bool IsActivated();
		void ActivateApp();
		void DeactivateApp();

	private:
		void		UpdateData( float fElapsedTime );

		CKTDIDevice*	m_KTDIKeyboard;
		CKTDIDevice*	m_KTDIMouse;
		CKTDIDevice*	m_KTDIJoystic;

		bool			m_bEnabled;

		bool			m_State[GAMEACTION_END];
		bool			m_Locks[GAMEACTION_END];
		DoubleKey		m_DoubleKeyCheck[GAMEACTION_END];

		bool			m_bJoyEnable;	// joypad on/off (2010-05-18)
		bool			m_bKeyChanging;

		bool			m_bActivated;
};
