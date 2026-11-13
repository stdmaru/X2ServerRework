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

#include "x2/platform/process.h"

#include <SDL3/SDL_main.h>

#include <memory>

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
// Only one client runs at a time (the original's "ElswordX2" mutex).
static std::unique_ptr<x2::platform::SingleInstance> g_pSingleInstance;
//{{ robobeg : 2008-10-10
CKTDGEffectStateManager* g_pEffectStateManager = NULL;
//}} robobeg : 2008-10-10


static bool g_bRenderEtc = false;	// whether to render the debug text (toggle fullscreen and other buttons in the original)

double			g_fTotalFrame = 0.0f;
int				g_RenderCount = 0;

#if X2_DEV_TOOLS
// The memory lines of the debug text, refreshed every 10 seconds.
static float	g_fLastUpdateTime_MemInfo = 10000.f;
DWORD			g_iAvailableTextureMem = 0;
DWORD			g_iTotalTextureMem = 0;
DWORD			g_iAvailableTextureMem2 = 0;
// Physical memory, and this process's memory (in place of the 32-bit virtual address space the original showed).
std::size_t		g_uiTotalPhysicalMem = 0;
std::size_t		g_uiAvailablePhysicalMem = 0;
std::size_t		g_uiProcessMem = 0;
std::size_t		g_uiPeakProcessMem = 0;
#endif // X2_DEV_TOOLS
//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3



//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
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



//--------------------------------------------------------------------------------------
// Entry point to the program (WinMain and its VirtualWinMain in the original; SDL_main.h provides WinMain on
// Windows). Initializes everything and goes into a message processing loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int main( int argc, char* argv[] )
{
	DXUTSetCommandLine( argc, argv );

#if !X2_DEV_TOOLS
	// The patcher puts PATCHER_RUN_ONLY last on the command line.
	if( argc < 2 || strcmp( argv[argc - 1], PATCHER_RUN_ONLY ) != 0 )
	{
		return 0;
	}

	g_pSingleInstance = x2::platform::SingleInstance::Acquire( "ElswordX2" );
	if( NULL == g_pSingleInstance )
	{
		return 0;
	}
#endif // !X2_DEV_TOOLS

	CKTDXApp::StartUp();

	// Set the callback functions. These functions allow DXUT to notify
	// the application about device changes, user input, and windows messages.
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

	InitApp();

	// Initialize DXUT and create the window and the Direct3D device for the application.
	DXUTInit( false, false, true ); // Parse the command line, handle the default hotkeys, and show msgboxes

	//{{ JHKang // 2010-8-27 // Multi-monitor: the window can move between monitors
	DXUTSetMultimonSettings(false);
	//}} JHKang // 2010-8-27

	// The window takes the executable's icon (x2_windows_icon).
	if( SUCCEEDED( DXUTCreateWindow( L"X2" ) ) &&
		SUCCEEDED( DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 1280, 720 ) ) )
	{
		// Pass control to DXUT for handling the message pump and
		// dispatching render calls. DXUT will call your FrameMove
		// and FrameRender callback when there is idle time between handling window messages.
		DXUTMainLoop();
	}

	// Perform any application-level cleanup here. Direct3D device resources are released within the
	// appropriate callback functions and therefore don't require any cleanup code here.
	CKTDXApp::Exit();

	g_pSingleInstance.reset();

	return DXUTGetExitCode();
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
// This callback function will be called immediately after the Direct3D device has been
// created, which will happen during application initialization and windowed/full screen
// toggles. This is the best location to create D3DPOOL_MANAGED resources since these
// resources need to be reloaded whenever the device is destroyed. Resources created
// here should be released in the OnDestroyDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* /*pBackBufferSurfaceDesc*/, void* /*pUserContext*/ )
{
	// Initialize the font

	g_pd3dDevice = pd3dDevice;

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
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* /*pd3dDevice*/,
							   const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* /*pUserContext*/ )
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
void CALLBACK OnFrameMove( IDirect3DDevice9* /*pd3dDevice*/, double fTime, float fElapsedTime, void* /*pUserContext*/ )
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
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double /*fTime*/, float fElapsedTime, void* /*pUserContext*/ )
{
	//KTDXPROFILE();


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



		pd3dDevice->EndScene();
	}
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText(float /*fElapsedTime*/)
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
	StringCchCopy( appData, std::size( appData ), DXUTGetFrameStats() );
	wcscat( appData, L"\n" );
	wcscat( appData, DXUTGetDeviceStats() );
	wcscat( appData, L", 평균 프레임 : ");
	_itow_s( (int)g_fTotalFrame / g_RenderCount, appData3, 10 );
	wcscat( appData, appData3 );
	if ( g_pX2Game != NULL && g_pX2Game->GetWorld() != NULL )
	{
		swprintf_s( wszText, L", 오브젝트 LOD : %d %%", g_pX2Game->GetWorld()->GetLOD() );
		wcscat( appData, wszText );
	}

	if ( g_pData != NULL )
	{
		swprintf_s( wszText, L", 캐릭터 LOD : %d %%", g_pData->GetModelDetailPercent() );
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
        default:
        	break;
        }//switch

	    swprintf_s( wszText, L"\nDynamicVBMode : %s", pDynamicVBMode );

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
	swprintf_s( appData2, L"프레임무브 FPS: %f 지연시간: %f  프레임무브: %d회  랜더: %d회 \n",
		g_pKTDXApp->GetFrameMoveFPS(), g_pKTDXApp->GetElapsedTime(), g_pKTDXApp->GetFrameMoveCount(), g_pKTDXApp->GetRenderCount() );
	wcscat( appData, appData2 );
#endif

	swprintf_s( appData2, L"동접: %d명, 대전방: <팀:%d, 팀데스:%d, 서바:%d>, 파티: %d, 게임중인파티: %d, 현재라인위치: %d",
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
						swprintf_s( timeBuf, L" / StateID: %d", iStateID );
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
			swprintf_s( timeBuf, L"HostUser: %s, NumOfNpc: %d, NumOfUser: %d AvgPingToHost: %d(%d)",
				g_pX2Game->GetHostGameUnit()->GetUnitName().c_str(), g_pX2Game->GetNPCUnitNum(), g_pX2Game->GetUserUnitNum(), iAvgPingTime, iAvgPingStd );

			if( g_pMain->GetFontForDebuf() != NULL )
				g_pMain->GetFontForDebuf()->OutTextXY( 40, 460, timeBuf, D3DXCOLOR(1,1,0,1),
				CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
		}


		// 캐릭터 좌표
		const D3DVECTOR vMyUnitPos = g_pX2Game->GetMyUnit()->GetPos();
		swprintf_s( timeBuf, L"MyUnitPos X:%d Y:%d, Z:%d", static_cast<int>(vMyUnitPos.x), static_cast<int>(vMyUnitPos.y), static_cast<int>(vMyUnitPos.z) );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 480, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );

		// 카메라 좌표
		const D3DXVECTOR3* vCameraPos = (true == g_pX2Game->GetFreeCamera() && NULL != g_pX2Game->GetFPSCamera()) ? g_pX2Game->GetFPSCamera()->GetEyePt() : NULL;
		if( NULL != vCameraPos )
		{
			swprintf_s( timeBuf, L"FreeCameraPos X:%d Y:%d, Z:%d", static_cast<int>(vCameraPos->x), static_cast<int>(vCameraPos->y), static_cast<int>(vCameraPos->z) );
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
				swprintf_s( timeBuf, L"FreeCameraPos X:%d Y:%d, Z:%d", static_cast<int>(vCameraPos->x), static_cast<int>(vCameraPos->y), static_cast<int>(vCameraPos->z) );
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
				swprintf_s( timeBuf, L"FreeCameraPos X:%d Y:%d, Z:%d", static_cast<int>(vCameraPos->x), static_cast<int>(vCameraPos->y), static_cast<int>(vCameraPos->z) );
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

			x2::platform::PhysicalMemory( g_uiTotalPhysicalMem, g_uiAvailablePhysicalMem );
			x2::platform::CurrentProcessMemory( g_uiProcessMem, g_uiPeakProcessMem );

			g_pKTDXApp->GetVideoRam( g_iTotalTextureMem, g_iAvailableTextureMem );

			g_iAvailableTextureMem2 = g_pKTDXApp->GetDevice()->GetAvailableTextureMem();
		}
		swprintf_s( timeBuf, L"PM : %d/%d (MB)", static_cast<int>( (g_uiTotalPhysicalMem-g_uiAvailablePhysicalMem)/1024/1024 ),
			static_cast<int>( g_uiTotalPhysicalMem/1024/1024 ) );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 540, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );

		swprintf_s( timeBuf, L"Process : %d/%d (MB, peak)", static_cast<int>( g_uiProcessMem/1024/1024 ),
			static_cast<int>( g_uiPeakProcessMem/1024/1024 ) );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 560, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
		swprintf_s( timeBuf, L"Video M : %d/%d (MB) TM : %d(MB)", (g_iTotalTextureMem-g_iAvailableTextureMem)/1024/1024, g_iTotalTextureMem/1024/1024, g_iAvailableTextureMem2/1024/1024 );
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
				swprintf_s( timeBuf, L"stage : %d-%d", iStageIndex, iSubStageIndex );
				if( g_pMain->GetFontForDebuf() != NULL )
					g_pMain->GetFontForDebuf()->OutTextXY( 40, 620, timeBuf, D3DXCOLOR(1,1,0,1),
					CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
			}
		}
	}

	if( g_pX2Game != NULL && g_pX2Game->GetMyUnit() != NULL )
	{
		int iLineIndex = (int)g_pX2Game->GetMyUnit()->GetSyncData().lastTouchLineIndex;

		swprintf_s( timeBuf, L"line : %d", iLineIndex );
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
		swprintf_s( timeBuf, L"obj chain(a/n): %d(%d,%d), Cull: %d", nNum, nAlpha, nNonAlpha, nCull );
//}} robobeg : 2008-10-18


		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 660, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMajorParticle() )
	{
        int nMajorSeqNum =  g_pData->GetGameMajorParticle()->EstimateParticleSequenceNum();

		swprintf_s( timeBuf, L"major p: %d", nMajorSeqNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 680, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMinorParticle() )
	{
        int nMinorSeqNum =  g_pData->GetGameMinorParticle()->EstimateParticleSequenceNum();

		swprintf_s( timeBuf, L"minor p: %d", nMinorSeqNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 700, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMajorXMeshPlayer() )
	{
        int nMajorInstanceNum = g_pData->GetGameMajorXMeshPlayer()->EstimateInstanceNum();

		swprintf_s( timeBuf, L"major m: %d", nMajorInstanceNum );
		if( g_pMain->GetFontForDebuf() != NULL )
			g_pMain->GetFontForDebuf()->OutTextXY( 40, 720, timeBuf, D3DXCOLOR(1,1,0,1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_LEFT );
	}

	if( NULL != g_pData &&
		NULL != g_pData->GetGameMinorXMeshPlayer() )
	{
        int nMinorInstanceNum =  g_pData->GetGameMinorXMeshPlayer()->EstimateInstanceNum();

		swprintf_s( timeBuf, L"minor m: %d", nMinorInstanceNum );
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
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* /*pUserContext*/ )
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
void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool /*bAltDown*/, void* /*pUserContext*/ )
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
void CALLBACK OnLostDevice( void* /*pUserContext*/ )
{

	g_pKTDXApp->OnLostDevice();


}


//--------------------------------------------------------------------------------------
// This callback function will be called immediately after the Direct3D device has
// been destroyed, which generally happens as a result of application termination or
// windowed/full screen toggles. Resources created in the OnCreateDevice callback
// should be released here, which generally includes all D3DPOOL_MANAGED resources.
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* /*pUserContext*/ )
{

	SAFE_DELETE( g_pKTDXApp );





//{{ robobeg : 2008-10-10
    SAFE_RELEASE( g_pEffectStateManager );
	g_pd3dDevice = NULL;
//}} robobeg : 2008-10-10

}
