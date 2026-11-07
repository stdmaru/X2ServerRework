//--------------------------------------------------------------------------------------
// File: X2.cpp
//
// Basic starting point for new Direct3D samples
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------



// wiselog 남기는 위치
//게임 맨 처음 시작했을 때							chceck1
//방화벽 예외 처리하고, 버전파일 체크하고 난 후 	chceck2
//100% 로딩 시작하면서 								chceck3
//100% 로딩 끝난 직후 								chceck4
//게임서버에 계정인증 직후							chceck5




#include "dxstdafx.h"
#include "resource.h"

//#define DEBUG_VS   // Uncomment this line to debug vertex shaders
//#define DEBUG_PS   // Uncomment this line to debug pixel shaders

#if defined(DEBUG) || defined(_DEBUG)
#else
#endif




//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
bool                    g_bShowHelp = true;     // If true, it renders the UI control text

LPDIRECT3DDEVICE9		g_pd3dDevice;
LPDIRECT3D9				g_pD3D;
HANDLE					g_hMutex = NULL;		// Mutex of game client program.
//{{ robobeg : 2008-10-10
CKTDGEffectStateManager* g_pEffectStateManager = NULL;
//}} robobeg : 2008-10-10


static bool g_bRenderEtc = false;	// toggle fullscreen, 따위의 버튼을 렌더링 할지 말지

static float	g_fLastUpdateTime_MemInfo = 10000.f;
DWORD			g_iAvailableTextureMem = 0;
DWORD			g_iTotalTextureMem = 0;
DWORD			g_iAvailableTextureMem2 = 0;
MEMORYSTATUS	g_MemStatus;

double			g_fTotalFrame = 0.0f;
int				g_RenderCount = 0;

#if X2_DEV_TOOLS
#endif // _SERVICE_
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3



//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
bool    CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
bool    CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void    CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void    CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );
void    CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void    CALLBACK OnLostDevice( void* pUserContext );
void    CALLBACK OnDestroyDevice( void* pUserContext );

void    InitApp();
void    RenderText(float fElapsedTime);

bool	ObtainSeDebugPrivilege();


//{{ 허상형 : [2009/8/4] //	핵실드
//}} 허상형 : [2009/8/4] //	핵실드




//--------------------------------------------------------------------------------
// 2011.4.8 조효진 핵실드 사용 시 포커스 넘어 가는 문제 때문에 해당 함수 사용
//--------------------------------------------------------------------------------
void ReallySetForegroundWindow( HWND hWnd )
{
	DWORD foregroundThreadID; // foreground window thread
	DWORD ourThreadID; // our active thread
	// If the window is in a minimized state, maximize now
	if (GetWindowLong(hWnd, GWL_STYLE) & WS_MINIMIZE)
	{
		ShowWindow(hWnd, SW_MAXIMIZE);
		UpdateWindow(hWnd);
	}
	// Check to see if we are the foreground thread
	foregroundThreadID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
	ourThreadID = GetCurrentThreadId();
	// If not, attach our thread's 'input' to the foreground thread's
	if (foregroundThreadID != ourThreadID)
		AttachThreadInput(foregroundThreadID, ourThreadID, TRUE);
	// Bring our window to the foreground
	SetForegroundWindow(hWnd);
	// If we attached our thread, detach it now
	if (foregroundThreadID != ourThreadID)
		AttachThreadInput(foregroundThreadID, ourThreadID, FALSE);
	// Force our window to redraw
	InvalidateRect(hWnd, NULL, TRUE);
}
//--------------------------------------------------------------------------------






//--------------------------------------------------------------------------------------
//클라이언트 메모리 해킹방지를 위한 메인처리 함수
//--------------------------------------------------------------------------------------
INT WINAPI VirtualWinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	ObtainSeDebugPrivilege();


    //{{ seojt // 2008-10-15, 16:35
#ifdef _DEBUG // 2008-10-15
    /** CRT 함수의 메모리 관리자가 보고하는 memory leak을 검사하기 위해
        debug break해야할 메모리 블록 번호를 명시한다.
        - jintaeks on 2008-10-15, 16:35 */
    // qff
    //BREAK_ALLOC( 1272439 );
    //INIT_CRTDEBUG( 0 );
#endif // _DEBUG // 2008-10-15
    //}} seojt // 2008-10-15, 16:35









#if !X2_DEV_TOOLS


	// 패쳐에서는 arg 젤 마지막에 PATCHER_RUN_ONLY 를 붙이는데 여기선 젤 앞에걸로 비교를 하네...
	char* tempArgv = __argv[__argc - 1];
	if( tempArgv == NULL )
	{
		return 0;
	}
	else if( strcmp(tempArgv, PATCHER_RUN_ONLY ) != 0 )
	{
		return 0;
	}


//#ifndef	_OPEN_TEST_		// 해외팀 제거
	g_hMutex = ::CreateMutex( NULL, TRUE, L"ElswordX2" );
	if ( ( NULL == g_hMutex ) || ( ERROR_ALREADY_EXISTS == ::GetLastError() ) )
	{
		return 0;
	}
//#endif	_OPEN_TEST_

#endif // _SERVICE_







	CKTDXApp::StartUp();

	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Set the callback functions. These functions allow DXUT to notify
	// the application about device changes, user input, and windows messages.  The
	// callbacks are optional so you need only set callbacks for events you're interested
	// in. However, if you don't handle the device reset/lost callbacks then the sample
	// framework won't be able to reset your device since the application must first
	// release all device resources before resetting.  Likewise, if you don't handle the
	// device created/destroyed callbacks then DXUT won't be able to
	// recreate your device resources.
	DXUTSetCallbackDeviceCreated( OnCreateDevice );
	DXUTSetCallbackDeviceReset( OnResetDevice );
	DXUTSetCallbackDeviceLost( OnLostDevice );
	DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackKeyboard( KeyboardProc );
	DXUTSetCallbackFrameRender( OnFrameRender );
	DXUTSetCallbackFrameMove( OnFrameMove );

	// Show the cursor and clip it when in full screen
	DXUTSetCursorSettings( true, true );


	// XTRAP 클라이언트 - X-TRAP 클라 실행


	InitApp();

	// Initialize DXUT and create the desired Win32 window and Direct3D
	// device for the application. Calling each of these functions is optional, but they
	// allow you to set several options which control the behavior of the framework.
	DXUTInit( false, false, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

	//{{ JHKang // 2010-8-27 // 다중 모니터 창 이동 가능
	DXUTSetMultimonSettings(false);
	//}} JHKang // 2010-8-27 // 다중 모니터 창 이동 가능

	HICON hIcon = ::LoadIcon( hInst, MAKEINTRESOURCE( IDI_ICON1 ) );

	DXUTCreateWindow( L"X2", NULL, hIcon );
	DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 1024, 768, IsDeviceAcceptable, ModifyDeviceSettings );


	//{{ 2010.12.1 조효진 핵쉴드 사용 시 포커스 넘어가는 문제 해결
	ReallySetForegroundWindow(DXUTGetHWND());
	//}}

	// Pass control to DXUT for handling the message pump and
	// dispatching render calls. DXUT will call your FrameMove
	// and FrameRender callback when there is idle time between handling window messages.
	DXUTMainLoop();

	// Perform any application-level cleanup here. Direct3D device resources are released within the
	// appropriate callback functions and therefore don't require any cleanup code here.
	CKTDXApp::Exit();


	if ( g_hMutex != NULL )
	{
		::ReleaseMutex( g_hMutex );
		::CloseHandle( g_hMutex );
		g_hMutex = NULL;
	}




	return DXUTGetExitCode();
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd )
{
	return VirtualWinMain( hInst, hPrev, lpCmdLine, nShowCmd );
}


//--------------------------------------------------------------------------------------
// Initialize the app
//--------------------------------------------------------------------------------------
void InitApp()
{
	// Initialize dialogs



	/*
	TODO: add UI controls as needed
	g_SampleUI.AddComboBox( 19, 35, iY += 24, 125, 22 );
	g_SampleUI.GetComboBox( 19 )->AddItem( L"Text1", NULL );
	g_SampleUI.GetComboBox( 19 )->AddItem( L"Text2", NULL );
	g_SampleUI.GetComboBox( 19 )->AddItem( L"Text3", NULL );
	g_SampleUI.GetComboBox( 19 )->AddItem( L"Text4", NULL );
	g_SampleUI.AddCheckBox( 21, L"Checkbox1", 35, iY += 24, 125, 22 );
	g_SampleUI.AddCheckBox( 11, L"Checkbox2", 35, iY += 24, 125, 22 );
	g_SampleUI.AddRadioButton( 12, 1, L"Radio1G1", 35, iY += 24, 125, 22 );
	g_SampleUI.AddRadioButton( 13, 1, L"Radio2G1", 35, iY += 24, 125, 22 );
	g_SampleUI.AddRadioButton( 14, 1, L"Radio3G1", 35, iY += 24, 125, 22 );
	g_SampleUI.GetRadioButton( 14 )->SetChecked( true );
	g_SampleUI.AddButton( 17, L"Button1", 35, iY += 24, 125, 22 );
	g_SampleUI.AddButton( 18, L"Button2", 35, iY += 24, 125, 22 );
	g_SampleUI.AddRadioButton( 15, 2, L"Radio1G2", 35, iY += 24, 125, 22 );
	g_SampleUI.AddRadioButton( 16, 2, L"Radio2G3", 35, iY += 24, 125, 22 );
	g_SampleUI.GetRadioButton( 16 )->SetChecked( true );
	g_SampleUI.AddSlider( 20, 50, iY += 24, 100, 22 );
	g_SampleUI.GetSlider( 20 )->SetRange( 0, 100 );
	g_SampleUI.GetSlider( 20 )->SetValue( 50 );
	g_SampleUI.AddEditBox( 20, L"Test", 35, iY += 24, 125, 32 );
	*/
}


//--------------------------------------------------------------------------------------
// Called during device initialization, this code checks the device for some
// minimum set of capabilities, and rejects those that don't pass by returning false.
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat,
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	// Skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3DObject();

	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
		return false;

	return true;
}


//--------------------------------------------------------------------------------------
// This callback function is called immediately before a device is created to allow the
// application to modify the device settings. The supplied pDeviceSettings parameter
// contains the settings that the framework has selected for the new device, and the
// application can make any desired changes directly to this structure.  Note however that
// DXUT will not correct invalid device settings so care must be taken
// to return valid device settings, otherwise IDirect3D9::CreateDevice() will fail.
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
	// then switch to SWVP.
	if( (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
		pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
	{
		pDeviceSettings->BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// Debugging vertex shaders requires either REF or software vertex processing
	// and debugging pixel shaders requires REF.
#ifdef DEBUG_VS
	if( pDeviceSettings->DeviceType != D3DDEVTYPE_REF )
	{
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		pDeviceSettings->BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
#endif
#ifdef DEBUG_PS
	pDeviceSettings->DeviceType = D3DDEVTYPE_REF;
#endif



	// depth/stencil buffer format을 지정해주자
	CD3DEnumeration *pEnum = DXUTGetEnumeration();
	CD3DEnumDeviceSettingsCombo *pCombo;

	pCombo = pEnum->GetDeviceSettingsCombo( pDeviceSettings );

	if( pCombo->depthStencilFormatList.Contains( D3DFMT_D24X8 ) == true )
		pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D24X8;
	else if( pCombo->depthStencilFormatList.Contains( D3DFMT_D32 ) == true )
		pDeviceSettings->pp.AutoDepthStencilFormat = D3DFMT_D32;


	//{{ dmlee 2008.03.25 fullscreen 상태에서 child window를 띄우기 위해
	pDeviceSettings->pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pDeviceSettings->pp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	//}} dmlee 2008.03.25 fullscreen 상태에서 child window를 띄우기 위해






//{{ seojt // 2008-10-14, 15:13
    // 그래픽 카드 드라이버가 아닌 DirectX가 리소스를 매니지 하도록 한다.
    pDeviceSettings->BehaviorFlags |= D3DCREATE_DISABLE_DRIVER_MANAGEMENT_EX;
    // DirectX가 생성된 쓰레드가 아닌 다른 쓰레드에서도 D3D Device를 접근할 수 있도록
    // 한다. background resource loading을 위해 처음으로 추가하다.
    // - jintaeks on 2008-10-14, 15:20
    pDeviceSettings->BehaviorFlags |= D3DCREATE_MULTITHREADED;
//}} seojt // 2008-10-14, 15:13

    pDeviceSettings->BehaviorFlags |= D3DCREATE_FPU_PRESERVE;

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		if( pDeviceSettings->DeviceType == D3DDEVTYPE_REF )
			DXUTDisplaySwitchingToREFWarning();
	}

	return true;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	// Initialize the font

	g_pd3dDevice = pd3dDevice;
	g_pD3D = DXUTGetD3DObject();

//{{ robobeg : 2008-10-10
    g_pEffectStateManager = new CKTDGEffectStateManager;
    ASSERT( g_pEffectStateManager != NULL );
//}} robobeg : 2008-10-10


	g_pKTDXApp	= new CKTDXApp( DXUTGetHWND(), DXUTGetHINSTANCE(), pd3dDevice );
	g_pMain		= new CX2Main();

	g_pKTDXApp->InsertStage( g_pMain );








	return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has been
// reset, which will happen after a lost device scenario. This is the best location to
// create D3DPOOL_DEFAULT resources since these resources need to be reloaded whenever
// the device is lost. Resources created here should be released in the OnLostDevice
// callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice,
							   const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{



	// Create a sprite to help batch calls when drawing many lines of text
	g_pKTDXApp->OnResetDevice( pBackBufferSurfaceDesc );



	//{{ dmlee 2009/11/16 alt_tab 기능 막기 코드 인듯.
	//if ( DXUTGetDeviceSettings().pp.Windowed == TRUE )
	//{
	//	UnregisterHotKey(DXUTGetHWND(), 0);
	//}
	//else
	//{
	//	RegisterHotKey(DXUTGetHWND(), 0, MOD_ALT, VK_TAB);
	//}
	//}} dmlee 2009/11/16 alt_tab 기능 막기 코드 인듯.



	return S_OK;
}


//--------------------------------------------------------------------------------------
// This callback function will be called once at the beginning of every frame. This is the
// best location for your application to handle updates to the scene, but is not
// intended to contain actual rendering calls, which should instead be placed in the
// OnFrameRender callback.
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{

	KTDXPROFILE();

	g_pKTDXApp->OnFrameMove( fTime, fElapsedTime );
}


//--------------------------------------------------------------------------------------
// This callback function will be called at the end of every frame to perform all the
// rendering calls for the scene, and it will also be called if the window needs to be
// repainted. After this function has returned, DXUT will call
// IDirect3DDevice9::Present to display the contents of the next buffer in the swap chain
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	//KTDXPROFILE();

	HRESULT hr;

	// If the settings dialog is being shown, then
	// render it instead of rendering the app's scene


	// Render the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		KTDXPROFILE_BEGIN("KTDXRender");
		g_pKTDXApp->OnFrameRender();
		KTDXPROFILE_END();


		if( true == g_bRenderEtc )
		{
			KTDXPROFILE_BEGIN("DXUTRender");
			DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" ); // These events are to help PIX identify what the code is doing
			KTDXPROFILE_BEGIN("RenderText");
			RenderText(fElapsedTime);
			KTDXPROFILE_END();

			DXUT_EndPerfEvent();
			KTDXPROFILE_END();
		}

#if X2_DEV_TOOLS // 2008-10-23
        _KHelpSystem.DisplayHelp();
#endif // KHELP_SYSTEM_TEST // 2008-10-23

#if X2_DEV_TOOLS
		if( g_pMain != NULL && g_pData != NULL && g_pData->GetMyUser() != NULL &&
			g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR &&
			g_pData->GetDungeonResultInfo() != NULL && g_pX2Room != NULL )
		{
			CX2DungeonGame::DungeonResultInfo* pDungeonResultInfo = g_pData->GetDungeonResultInfo();

			int playTimeAllSec = g_pData->GetDungeonResultInfo()->m_Sec;

			int playTimeHour = playTimeAllSec/3600;
			int	playTimeMinute = ( playTimeAllSec%3600 ) / 60;
			int playTimeSec = ( playTimeAllSec%3600 ) % 60;

			int peopleNum = 0;
			for ( int i = 0; i < (int)pDungeonResultInfo->m_DungeonResultUnitInfoList.size(); i++ )
			{
				CX2DungeonGame::DungeonResultUnitInfo* pDungeonResultUnitInfo = pDungeonResultInfo->m_DungeonResultUnitInfoList[i];
				if ( pDungeonResultUnitInfo == NULL )
					continue;

				CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( pDungeonResultUnitInfo->m_UnitUID );

				if ( pSlotData == NULL || pSlotData->m_pUnit == NULL )
					continue;

				WCHAR buff[256] = {0};
				wstring tempResultString;

				tempResultString = pSlotData->m_pUnit->GetNickName();
				tempResultString += L"\n";

				StringCchPrintf( buff, 256, L"Exp = %d,  m_nClearBonusEXP = %d, m_nRankBonusEXP = %d \n m_nPartyBonusEXP = %d, m_nMvpBonusEXP = %d m_nGameBangBonusEXP = %d, \n m_nPremiumBonusEXP = %d, m_nEventBonusExp = %d, ED = %d, KillNum = %d, PlayTime = %d:%d:%d\n",
					pDungeonResultUnitInfo->m_nEXP,
					pDungeonResultUnitInfo->m_nClearBonusEXP,
					pDungeonResultUnitInfo->m_nRankBonusEXP,
					pDungeonResultUnitInfo->m_nPartyBonusEXP,
					pDungeonResultUnitInfo->m_nMvpBonusEXP,
					pDungeonResultUnitInfo->m_nGameBangBonusEXP,
					pDungeonResultUnitInfo->m_nPremiumBonusEXP,
					pDungeonResultUnitInfo->m_nEventBonusExp,
					pDungeonResultUnitInfo->m_nED, pDungeonResultUnitInfo->m_nNPCKillNum, playTimeHour, playTimeMinute, playTimeSec );
				/*wsprintf( buff, L"Exp = %d,  m_nClearBonusEXP = %d, m_nRankBonusEXP = %d \n m_nPartyBonusEXP = %d, m_nMvpBonusEXP = %d m_nGameBangBonusEXP = %d, \n m_nPremiumBonusEXP = %d, m_nEventBonusExp = %d, ED = %d, KillNum = %d, PlayTime = %d:%d:%d\n",
					pDungeonResultUnitInfo->m_nEXP,
					pDungeonResultUnitInfo->m_nClearBonusEXP,
					pDungeonResultUnitInfo->m_nRankBonusEXP,
					pDungeonResultUnitInfo->m_nPartyBonusEXP,
					pDungeonResultUnitInfo->m_nMvpBonusEXP,
					pDungeonResultUnitInfo->m_nGameBangBonusEXP,
					pDungeonResultUnitInfo->m_nPremiumBonusEXP,
					pDungeonResultUnitInfo->m_nEventBonusExp,
					pDungeonResultUnitInfo->m_nED, pDungeonResultUnitInfo->m_nNPCKillNum, playTimeHour, playTimeMinute, playTimeSec );*/
				tempResultString += buff;

				StringCchPrintf( buff, 256,  L"TotalScore = %d, ComboScore = %d, TechnicalScore = %d, Damaged = %d, \nRankBonus = %d PartyBonus = %d\n",
					pDungeonResultUnitInfo->m_nTotalScore, pDungeonResultUnitInfo->m_nComboScore, pDungeonResultUnitInfo->m_nTechnicalScore, pDungeonResultUnitInfo->m_nDamaged, (int)(pDungeonResultUnitInfo->m_fRankBonus * 100 ), (int)(pDungeonResultUnitInfo->m_fPartyBonus * 100) );
				/*wsprintf( buff, L"TotalScore = %d, ComboScore = %d, TechnicalScore = %d, Damaged = %d, \nRankBonus = %d PartyBonus = %d\n",
					pDungeonResultUnitInfo->m_nTotalScore, pDungeonResultUnitInfo->m_nComboScore, pDungeonResultUnitInfo->m_nTechnicalScore, pDungeonResultUnitInfo->m_nDamaged, (int)(pDungeonResultUnitInfo->m_fRankBonus * 100 ), (int)(pDungeonResultUnitInfo->m_fPartyBonus * 100) );*/
				tempResultString += buff;

				StringCchPrintf( buff, 256, L"ComboRank = %s, TechnicalRank = %s, DamageRank = %s, TimeRank = %s, TotalRank = %s",
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cComboRank ).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cTechnicalRank ).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cDamagedRank).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cTimeRank).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cTotalRank).c_str() );
				/*wsprintf( buff, L"ComboRank = %s, TechnicalRank = %s, DamageRank = %s, TimeRank = %s, TotalRank = %s",
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cComboRank ).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cTechnicalRank ).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cDamagedRank).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cTimeRank).c_str(),
					g_pData->GetComboString( pDungeonResultUnitInfo->m_cTotalRank).c_str() );*/
				tempResultString += buff;

				if( g_pMain->GetFontForDebuf() != NULL )
					g_pMain->GetFontForDebuf()->OutTextMultiline( 40, (peopleNum * 120) + 70, tempResultString.c_str(), D3DXCOLOR(1,1,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), 1.0f, NULL, DT_LEFT );

				peopleNum++;
			}
		}
#endif



		V( pd3dDevice->EndScene() );
	}
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText(float fElapsedTime)
{
	// The helper object simply helps keep track of text position, and color
	// and then it calls pFont->DrawText( m_pSprite, strMsg, -1, &rc, DT_NOCLIP, m_clr );
	// If NULL is passed in as the sprite object, then it will work however the
	// pFont->DrawText() will not be batched together.  Batching calls will improves performance.
	//   CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

	g_fTotalFrame += DXUTGetFPS();
	g_RenderCount += 1;

	// Output statistics
	WCHAR appData[512] = {0,};
	WCHAR appData3[512] = {0,};
	WCHAR wszText[512] = L"";
	swprintf( appData, DXUTGetFrameStats() );
	wcscat( appData, L"\n" );
	wcscat( appData, DXUTGetDeviceStats() );
	wcscat( appData, L", 평균 프레임 : ");
	wcscat( appData, _itow( (int)g_fTotalFrame / g_RenderCount, appData3, 10 ) );
	if ( g_pX2Game != NULL && g_pX2Game->GetWorld() != NULL )
	{
		swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L", 오브젝트 LOD : %d %%", g_pX2Game->GetWorld()->GetLOD() );
		wcscat( appData, wszText );
	}

	if ( g_pData != NULL )
	{
		swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L", 캐릭터 LOD : %d %%", g_pData->GetModelDetailPercent() );
		wcscat( appData, wszText );
	}


//#ifdef  DYNAMIC_VERTEX_BUFFER_OPT

    if ( g_pKTDXApp != NULL && g_pKTDXApp->GetDVBManager() != NULL )
    {
        const WCHAR*  pDynamicVBMode = L"";
        switch( g_pKTDXApp->GetDVBManager()->GetDynamicVBMode() )
        {
        case CKTDGDynamicVBManager::DYNAMIC_VB_MODE_MANAGED:
            pDynamicVBMode = L"MANAGED";
            break;
        case CKTDGDynamicVBManager::DYNAMIC_VB_MODE_PRIMITIVEUP:
            pDynamicVBMode = L"PrimitiveUP";
            break;
        case CKTDGDynamicVBManager::DYNAMIC_VB_MODE_DYNAMICVB:
            pDynamicVBMode = L"DynamicVB";
            break;
        case CKTDGDynamicVBManager::DYNAMIC_VB_MODE_DYNAMICVB_WITH_TRY_CATCH:
            pDynamicVBMode = L"DynamicVBWithTC";
            break;
        }//switch

	    swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L"\nDynamicVBMode : %s", pDynamicVBMode );

	    wcscat( appData, wszText );

	    wcscat( appData, wszText );
    }//if
//#endif  DYNAMIC_VERTEX_BUFFER_OPT


	wcscat( appData, L"\n" );

	WCHAR appData2[256] = {0,};

	int iSimultaneousUserCount = 0;
	int iPVPRoomTeamCount = 0;
	int iPVPRoomTeamDeathCount = 0;
	int iPVPRoomSurvivalCount = 0;
	int iCurPartyCount = 0;
	int iPlayGamePartyCount = 0;
	int iCurrentOnLineIndex = -1;

	if( NULL != g_pMain )
	{
		iSimultaneousUserCount	= g_pMain->GetServerUserCount();
		std::map<int, int>& mapPVPRoomCount = g_pMain->GetMapServerPVPRoomCount();
		if( mapPVPRoomCount.size() > 0 )
		{
			iPVPRoomTeamCount		= mapPVPRoomCount[ (int)CX2PVPRoom::PGT_TEAM ];
			iPVPRoomTeamDeathCount	= mapPVPRoomCount[ (int)CX2PVPRoom::PGT_TEAM_DEATH ];
			iPVPRoomSurvivalCount	= mapPVPRoomCount[ (int)CX2PVPRoom::PGT_SURVIVAL ];
		}

		iCurPartyCount				= g_pMain->GetCurPartyCount();
		iPlayGamePartyCount			= g_pMain->GetPlayGamePartyCount();

		if ( NULL != g_pTFieldGame && NULL != g_pTFieldGame->GetMyUnit() )
			iCurrentOnLineIndex = g_pTFieldGame->GetMyUnit()->GetLastTouchLineIndex();

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
			iCurrentOnLineIndex = g_pX2Game->GetMyUnit()->GetLastTouchLineIndex();
	}

#if X2_DEV_TOOLS
	swprintf( appData2, L"프레임무브 FPS: %f 지연시간: %f  프레임무브: %d회  랜더: %d회 \n",
		g_pKTDXApp->GetFrameMoveFPS(), g_pKTDXApp->GetElapsedTime(), g_pKTDXApp->GetFrameMoveCount(), g_pKTDXApp->GetRenderCount() );
	wcscat( appData, appData2 );
#endif

	swprintf( appData2, L"동접: %d명, 대전방: <팀:%d, 팀데스:%d, 서바:%d>, 파티: %d, 게임중인파티: %d, 현재라인위치: %d",
		iSimultaneousUserCount, iPVPRoomTeamCount, iPVPRoomTeamDeathCount, iPVPRoomSurvivalCount, iCurPartyCount, iPlayGamePartyCount, iCurrentOnLineIndex );
	wcscat( appData, appData2 );



	/*
	txtHelper.Begin();
	txtHelper.SetInsertionPos( 5, 5 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	txtHelper.DrawTextLine( appData );
	*/

	WCHAR timeBuf[200] = {0,};
	g_pMain->GetFontForDebuf()->OutTextMultiline( 5, 5, appData, D3DXCOLOR( 1, 1, 0, 1),
		CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );


	if ( NULL != g_pX2Room )
	{
		int iLineNumber = 0;
		const int iObserverlSlotNum = g_pX2Room->GetObserverSlotNum();
		for ( int i = 0; i < iObserverlSlotNum; i++ )
		{
			CX2Room::SlotData* pSlotData = g_pX2Room->GetObserverSlotData( i );
			if ( NULL != pSlotData && pSlotData->GetSlotState() != CX2Room::SS_EMPTY && pSlotData->GetSlotState() != CX2Room::SS_CLOSE )
			{
				wstring wstrNetworkInfo = L"Observer";
				wstrNetworkInfo += g_pX2Room->GetStringNetworkInfo( pSlotData );

				if( g_pMain->GetFontForDebuf() != NULL )
					g_pMain->GetFontForDebuf()->OutTextXY( 40, 200 + iLineNumber * 15, wstrNetworkInfo.c_str(), D3DXCOLOR(1,1,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
				++iLineNumber;
			}
		}

		const int iNormalSlotNum = g_pX2Room->GetSlotNum();
		for ( int i = 0; i < iNormalSlotNum; i++ )
		{
			CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( i );
			if ( NULL != pSlotData && pSlotData->GetSlotState() != CX2Room::SS_EMPTY && pSlotData->GetSlotState() != CX2Room::SS_CLOSE )
			{
				wstring wstrNetworkInfo =
					( NULL != pSlotData->m_pUnit ? pSlotData->m_pUnit->GetNickName() : L"" );
				wstrNetworkInfo += g_pX2Room->GetStringNetworkInfo( pSlotData );

				if ( NULL != g_pX2Game )
				{
					const CX2GUUser* pGUUser = g_pX2Game->GetUserUnitByUID( pSlotData->m_UnitUID );
					if ( NULL != pGUUser )
					{
						const int iStateID = pGUUser->GetNowStateID();
						swprintf( timeBuf, L" / StateID: %d", iStateID );
						wstrNetworkInfo += timeBuf;
					}
				}

				if( g_pMain->GetFontForDebuf() != NULL )
					g_pMain->GetFontForDebuf()->OutTextXY( 40, 200 + iLineNumber * 15, wstrNetworkInfo.c_str(), D3DXCOLOR(1,1,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
				++iLineNumber;
			}
		}
	}

	if( g_pX2Game != NULL && g_pX2Game->GetMyUnit() != NULL )
	{
		if( g_pX2Game != NULL && g_pX2Game->GetHostGameUnit() != NULL )
		{
            int iAvgPingTime = 0;
            int iAvgPingStd = 0;
            if ( g_pX2Room != NULL )
            {
               const CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( g_pX2Game->GetHostGameUnit()->GetUnitIndex() );
               if ( pSlotData != NULL )
               {
                   iAvgPingTime = (int) ( pSlotData->m_fAvgPingTime * 1000 );
                   iAvgPingStd = (int) ( pSlotData->m_fAvgPingStd * 1000 );
               }
            }
			swprintf( timeBuf, L"HostUser: %s, NumOfNpc: %d, NumOfUser: %d AvgPingToHost: %d(%d)",
				g_pX2Game->GetHostGameUnit()->GetUnitName().c_str(), g_pX2Game->GetNPCUnitNum(), g_pX2Game->GetUserUnitNum(), iAvgPingTime, iAvgPingStd );

			if( g_pMain->GetFontForDebuf() != NULL )
				g_pMain->GetFontForDebuf()->OutTextXY( 40, 460, timeBuf, D3DXCOLOR(1,1,0,1),
				CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
		}


		// 캐릭터 좌표
		const D3DVECTOR vMyUnitPos = g_pX2Game->GetMyUnit()->GetPos();
		swprintf( timeBuf, L"MyUnitPos X:%d Y:%d, Z:%d", static_cast<int>(vMyUnitPos.x), static_cast<int>(vMyUnitPos.y), static_cast<int>(vMyUnitPos.z) );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 480, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );

		// 카메라 좌표
		const D3DXVECTOR3* vCameraPos = (true == g_pX2Game->GetFreeCamera() && NULL != g_pX2Game->GetFPSCamera()) ? g_pX2Game->GetFPSCamera()->GetEyePt() : NULL;
		if( NULL != vCameraPos )
		{
			swprintf( timeBuf, L"FreeCameraPos X:%d Y:%d, Z:%d", static_cast<int>(vCameraPos->x), static_cast<int>(vCameraPos->y), static_cast<int>(vCameraPos->z) );
			if( g_pMain->GetFontForDebuf() != NULL )
				g_pMain->GetFontForDebuf()->OutTextXY( 40, 500, timeBuf, D3DXCOLOR(1,1,0,1),
				CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
		}
	}
	else
	{
		// 카메라 좌표
		if( NULL != g_pTFieldGame && true == g_pTFieldGame->GetFreeCamera() )
		{
			const D3DXVECTOR3* vCameraPos = ( NULL != g_pTFieldGame->GetFPSCamera() ) ? g_pTFieldGame->GetFPSCamera()->GetEyePt() : NULL;
			if( NULL != vCameraPos )
			{
				swprintf( timeBuf, L"FreeCameraPos X:%d Y:%d, Z:%d", static_cast<int>(vCameraPos->x), static_cast<int>(vCameraPos->y), static_cast<int>(vCameraPos->z) );
				if( g_pMain->GetFontForDebuf() != NULL )
				g_pMain->GetFontForDebuf()->OutTextXY( 40, 500, timeBuf, D3DXCOLOR(1,1,0,1),
				CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
			}
		}
#ifdef ADD_STATE_WEDDING_GAME
		else if ( NULL != g_pWeddingGame && true == g_pWeddingGame->GetFreeCamera() )
		{
			const D3DXVECTOR3* vCameraPos = ( NULL != g_pWeddingGame->GetFPSCamera() ) ? g_pWeddingGame->GetFPSCamera()->GetEyePt() : NULL;
			if( NULL != vCameraPos )
			{
				swprintf( timeBuf, L"FreeCameraPos X:%d Y:%d, Z:%d", static_cast<int>(vCameraPos->x), static_cast<int>(vCameraPos->y), static_cast<int>(vCameraPos->z) );
				if( g_pMain->GetFontForDebuf() != NULL )
					g_pMain->GetFontForDebuf()->OutTextXY( 40, 500, timeBuf, D3DXCOLOR(1,1,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
			}
		}
#endif // ADD_STATE_WEDDING_GAME

	}



#if X2_DEV_TOOLS

	if( g_pKTDXApp != NULL && g_pKTDXApp->GetDevice() != NULL )
	{
		g_fLastUpdateTime_MemInfo += g_pKTDXApp->GetElapsedTime();
		if( g_fLastUpdateTime_MemInfo > 10.f )
		{
			g_fLastUpdateTime_MemInfo = 0.f;

			g_MemStatus.dwLength = sizeof( g_MemStatus );
			GlobalMemoryStatus( &g_MemStatus );

			g_pKTDXApp->GetVideoRam( g_iTotalTextureMem, g_iAvailableTextureMem );

			g_iAvailableTextureMem2 = g_pKTDXApp->GetDevice()->GetAvailableTextureMem();
		}
		swprintf( timeBuf, L"PM : %d/%d (MB)", (g_MemStatus.dwTotalPhys-g_MemStatus.dwAvailPhys)/1024/1024,
			g_MemStatus.dwTotalPhys/1024/1024 );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 540, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );

		swprintf( timeBuf, L"VM : %d/%d (MB)", (g_MemStatus.dwTotalVirtual-g_MemStatus.dwAvailVirtual)/1024/1024,
			g_MemStatus.dwTotalVirtual/1024/1024 );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 560, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
		swprintf( timeBuf, L"Video M : %d/%d (MB) TM : %d(MB)", (g_iTotalTextureMem-g_iAvailableTextureMem)/1024/1024, g_iTotalTextureMem/1024/1024, g_iAvailableTextureMem2/1024/1024 );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 600, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( g_pData->GetDungeonRoom() != NULL && g_pX2Game != NULL && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
	{
		CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
		if( pDungeonGame->GetDungeon() != NULL )
		{
			int iStageIndex = pDungeonGame->GetDungeon()->GetNowStageIndex();

			if( pDungeonGame->GetDungeon()->GetNowStage() != NULL )
			{
				int iSubStageIndex = pDungeonGame->GetDungeon()->GetNowStage()->GetNowSubStageIndex();
				swprintf( timeBuf, L"stage : %d-%d", iStageIndex, iSubStageIndex );
				if( g_pMain->GetFontForDebuf() != NULL )
					g_pMain->GetFontForDebuf()->OutTextXY( 40, 620, timeBuf, D3DXCOLOR(1,1,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
			}
		}
	}

	if( g_pX2Game != NULL && g_pX2Game->GetMyUnit() != NULL )
	{
		int iLineIndex = (int)g_pX2Game->GetMyUnit()->GetSyncData().lastTouchLineIndex;

		swprintf( timeBuf, L"line : %d", iLineIndex );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 640, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( g_pKTDXApp->GetDGManager() != NULL )
	{
		int nNum = g_pKTDXApp->GetDGManager()->GetRenderObjectChainSize();
		int nAlpha = g_pKTDXApp->GetDGManager()->GetAlphaObjectChainSize();
		int nNonAlpha = g_pKTDXApp->GetDGManager()->GetNonAlphaObjectChainSize();
//{{ robobeg : 2008-10-18
        int nCull = g_pKTDXApp->GetDGManager()->GetCullCount();
		swprintf( timeBuf, L"obj chain(a/n): %d(%d,%d), Cull: %d", nNum, nAlpha, nNonAlpha, nCull );
//}} robobeg : 2008-10-18


		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 660, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMajorParticle() )
	{
        int nMajorSeqNum =  g_pData->GetGameMajorParticle()->EstimateParticleSequenceNum();

		swprintf( timeBuf, L"major p: %d", nMajorSeqNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 680, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMinorParticle() )
	{
        int nMinorSeqNum =  g_pData->GetGameMinorParticle()->EstimateParticleSequenceNum();

		swprintf( timeBuf, L"minor p: %d", nMinorSeqNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 700, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMajorXMeshPlayer() )
	{
        int nMajorInstanceNum = g_pData->GetGameMajorXMeshPlayer()->EstimateInstanceNum();

		swprintf( timeBuf, L"major m: %d", nMajorInstanceNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 720, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMinorXMeshPlayer() )
	{
        int nMinorInstanceNum =  g_pData->GetGameMinorXMeshPlayer()->EstimateInstanceNum();

		swprintf( timeBuf, L"minor m: %d", nMinorInstanceNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 740, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

#endif

	/*
	TODO: add UI text as needed
	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	txtHelper.DrawTextLine( L"Put whatever misc status here" );

	// Draw help
	const D3DSURFACE_DESC* pd3dsdBackBuffer = DXUTGetBackBufferSurfaceDesc();
	if( g_bShowHelp )
	{
	txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-15*6 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 0.75f, 0.0f, 1.0f ) );
	txtHelper.DrawTextLine( L"Controls (F1 to hide):" );

	txtHelper.SetInsertionPos( 40, pd3dsdBackBuffer->Height-15*5 );
	txtHelper.DrawTextLine( L"Quit: ESC" );
	}
	else
	{
	txtHelper.SetInsertionPos( 10, pd3dsdBackBuffer->Height-15*2 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	txtHelper.DrawTextLine( L"Press F1 for help" );
	}
	*/
	//   txtHelper.End();
}


//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows
// messages to the application through this callback function. If the application sets
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext )
{
	// Always allow dialog resource manager calls to handle global messages
	// so GUI state is updated correctly
	//*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
	//if( *pbNoFurtherProcessing )
	//    return 0;

	if( true == g_bRenderEtc )
	{
	}

	if( g_pKTDXApp != NULL )
		*pbNoFurtherProcessing = g_pKTDXApp->MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
		return 0;

	return 0;
}




//--------------------------------------------------------------------------------------
// As a convenience, DXUT inspects the incoming windows messages for
// keystroke messages and decodes the message parameters to pass relevant keyboard
// messages to the application.  The framework does not remove the underlying keystroke
// messages, which are still passed to the application's MsgProc callback.
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{

	if( bKeyDown )
	{
		switch( nChar )
		{
			//case VK_F1: g_bShowHelp = !g_bShowHelp; break;
		case VK_HOME:
			{
				if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_SPECIAL_USER )
				{
					g_bRenderEtc = !g_bRenderEtc;
					g_pData->ToggleRenderEtc();
					SiCKTDXStringTable()->GetInstance()->SetEnableDebugInfo(g_bRenderEtc);
					if( true == g_bRenderEtc )
					{
						g_pChatBox->HideChatLog();
					}
				}
			}
			break;

#if X2_DEV_TOOLS
        case VK_PRIOR:
			if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
			{
                // Shift+End 키를 누른 경우 KHelpSystem을 토글한다.
                if ( GetAsyncKeyState(VK_LSHIFT) & 0x8000 )
                {
                    // toggle help system
                    _KHelpSystem.EnableDisplayHelp( !_KHelpSystem.IsDisplayHelp() );
                }
                else
                {
                    // KHelpSystem이 활성화된 상태에서 End키를 누르면 다음 help를 활성화시킨다.
                    if ( _KHelpSystem.IsDisplayHelp() )
                    {
                        _KHelpSystem.PrevHelp();
                    }//if
                }//if.. else..
			}//if
            break;

        case VK_NEXT:
			if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
			{
                // Shift+End 키를 누른 경우 KHelpSystem을 토글한다.
                if ( GetAsyncKeyState(VK_LSHIFT) & 0x8000 )
                {
                    // toggle help system
                    _KHelpSystem.EnableDisplayHelp( !_KHelpSystem.IsDisplayHelp() );
                }
                else
                {
                    // KHelpSystem이 활성화된 상태에서 End키를 누르면 다음 help를 활성화시킨다.
                    if ( _KHelpSystem.IsDisplayHelp() )
                    {
                        _KHelpSystem.NextHelp();
                    }//if
                }//if.. else..
			}//if
            break;
#endif // KHELP_SYSTEM_TEST

//#ifdef NEW_VILLAGE
//				else if( g_pTFieldGame != NULL && g_pTFieldGame->GetCamera() != NULL )
//				{
//					g_pTFieldGame->GetCamera()->SetUseLineCamera( !g_pTFieldGame->GetCamera()->GetUseLineCamera() );
//				}
//#endif
//			} break;
//
//#endif NEW_VILLAGE_RENDERING_TEST
		}
	}
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// entered a lost state and before IDirect3DDevice9::Reset is called. Resources created
// in the OnResetDevice callback should be released here, which generally includes all
// D3DPOOL_DEFAULT resources. See the "Lost Devices" section of the documentation for
// information about lost devices.
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{

	g_pKTDXApp->OnLostDevice();


}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{

	SAFE_DELETE( g_pKTDXApp );





//{{ robobeg : 2008-10-10
    SAFE_RELEASE( g_pEffectStateManager );
	g_pd3dDevice = NULL;
//}} robobeg : 2008-10-10

#if !X2_DEV_TOOLS
	HWND hWeb = ::FindWindow(NULL, L"Elsword - Windows Internet Explorer");
	if( hWeb != NULL )
	{
		::ShowWindow(hWeb, SW_RESTORE);
	}
#endif // _SERVICE_
}

//--------------------------------------------------------------------------------------
// HackShield 가 가동되면 HackShield 가 그 권한을 올려 주기 때문에 별도의 작업이 필요없지만
// HackShield 없는 상태에서 테스트 목적으로 리부팅을 해보시려면 SeDebug 권한을 얻는 작업을 진행해야 합니다.
// 아래 ObtainSeDebugPrivilege() 함수를 선언해두시고 게임 클라이언트 초기화 루틴에서 한번만 불러 주면
// 별도의 include 도 필요 없고 함수로 모듈화 되어있으니 그대로 복사하시고 사용하시면 됨.
//--------------------------------------------------------------------------------------
bool ObtainSeDebugPrivilege()
{
	BOOL Result;
	TOKEN_PRIVILEGES TokenPrivileges;
	TOKEN_PRIVILEGES PreviousTokenPrivileges;
	LUID luid;
	HANDLE hToken;
	DWORD dwPreviousTokenPrivilegesSize = sizeof(TOKEN_PRIVILEGES);

	Result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	if (Result == FALSE)
		return false;

	Result = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	if (Result == FALSE)
	{
		CloseHandle(hToken);
		return false;
	}

	TokenPrivileges.PrivilegeCount            = 1;
	TokenPrivileges.Privileges[0].Luid        = luid;
	TokenPrivileges.Privileges[0].Attributes  = 0;

	AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES),
		&PreviousTokenPrivileges, &dwPreviousTokenPrivilegesSize);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	PreviousTokenPrivileges.PrivilegeCount             = 1;
	PreviousTokenPrivileges.Privileges[0].Luid         = luid;
	PreviousTokenPrivileges.Privileges[0].Attributes  |= SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &PreviousTokenPrivileges,
		dwPreviousTokenPrivilegesSize, NULL, NULL);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	CloseHandle(hToken);

	return true;
}
