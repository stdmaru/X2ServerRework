#include "stdafx.h"
#include <ddraw.h>
#include "./KTDXApp.h"
#include "../X2Lib/X2Define.h"
//#include "KFileVerifier.h" // seojt // 2008-1-5, 14:44


// #define DXUT_AUTOLIB to automatically include the libs needed for DXUT

//????��??
#ifdef DXUT_AUTOLIB
	#if defined(DEBUG) || defined(_DEBUG)
	#else
	#endif
#endif


#if defined(DEBUG) || defined(_DEBUG)
#else
#endif


#ifdef CONVERSION_VS
#else
#endif // CONVERSION_VS



double		g_NowTime;
CKTDXApp*	g_pKTDXApp = NULL;

KLuabinder* CKTDXApp::ms_pLuaBinder = NULL;

HMODULE hUnicowsModule;
HMODULE hMsvcp71Module;
HMODULE hMsvcr71Module;
HMODULE hUsp10Module;

HMODULE LoadSystemDLL(void)
{
	hUnicowsModule = LoadLibraryA("unicows.dll");
	hUsp10Module = LoadLibraryA("usp10.dll");
	//hMsvcp71Module = LoadLibraryA("msvcp71.dll");
	//hMsvcr71Module = LoadLibraryA("msvcr71.dll");
	return hUnicowsModule;
}

void FreeSystemDLL(void)
{
	FreeLibrary( hUnicowsModule );
	FreeLibrary( hUsp10Module );
	//FreeLibrary( hMsvcp71Module );
	//FreeLibrary( hMsvcr71Module );
}

#ifdef _cplusplus
extern "C" {
#endif
	extern FARPROC _PfnLoadSystemDLL = (FARPROC) &LoadSystemDLL;
#ifdef _cplusplus
}
#endif

CRITICAL_SECTION g_csGameMessage;

bool KTDXSendGameMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, bool bDirectProcess )
{
	return g_pKTDXApp->SendGameMessage( uMsg, wParam, lParam, bDirectProcess );
}



void CKTDXApp::StartUp()
{
	_PfnLoadSystemDLL();
}

void CKTDXApp::Exit( EXIT_CODE code )
{
	FreeSystemDLL();
}

bool IncludeLua_( const char* pFileName )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );

	if( g_pKTDXApp->LoadLuaTinker( fileName.c_str() ) == false )
	{
		ErrorLogMsg( KEM_ERROR68, pFileName );
		return false;
	}

	return true;
}


CKTDXApp::CKTDXApp( HWND hWnd, HINSTANCE hInstance, IDirect3DDevice9* pd3dDevice )
{


   // 090416. jseop. ????? ????? ????.
	g_pKTDXApp = this;

    std::wstring wstrGeneralStringFileName( L"General.ess" );
	std::wstring wstrScriptStringFileName( L"Script.ess" );

    SiCKTDXStringTable()->InitializeStringTable( wstrGeneralStringFileName , wstrScriptStringFileName);
	//std::wstring wstrScriptStringFileName( L"Script.ess" );
	//SiCKTDXStringTable()->InitializeStringTable( wstrScriptStringFileName, true );
#if defined(DLL_BUILD)
	HRESULT hr;
#else
	HRESULT hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );
	if ( hr == S_FALSE )
	{
		ErrorLog( KEM_ERROR349 );
	}
	else if ( hr == RPC_E_CHANGED_MODE )
	{
		ErrorLog( KEM_ERROR350 );
	}
#endif // DLL_BUILD

	srand((unsigned int)time(NULL));

#ifndef INACTIVATION_MINIMIZE_ALL_WINDOW_AT_START		/// X2.exe ????? ??? ??? ???? ??? ??? ????
#if !X2_DEV_TOOLS

	{
		// ???? ?????? ???? ?? ?? ?????? ????? ????, ?????? ??? ???? ????? ????.
		// ??? ???? ??? ???????? ?????? ???? ?????? Mimize.txt?? ?????? ?????? ???? ????? ????? ???????.
		const string strFileName = "Inactivation_Mimize.txt";
		FILE* file = NULL;
		file = fopen( strFileName.c_str(), "r" );
		if( NULL == file )
		{
			IShellDispatch *pDisp;
			hr = CoCreateInstance(CLSID_Shell,NULL,CLSCTX_INPROC_SERVER ,IID_IShellDispatch,(void**)&pDisp);
			if( SUCCEEDED(hr) )
			{
				pDisp->MinimizeAll();
				pDisp->Release();
			}
		}
		else
		{
			fclose(file);
		}
	}

#endif // _SERVICE_
#endif // INACTIVATION_MINIMIZE_ALL_WINDOW_AT_START

	InitializeCriticalSection( &g_csGameMessage );

	ASSERT( ms_pLuaBinder == NULL );
	SAFE_DELETE( ms_pLuaBinder );
	ms_pLuaBinder = new KLuabinder();
	ASSERT( ms_pLuaBinder != NULL );




	m_pd3dDevice		= pd3dDevice;
#ifdef CONVERSION_VS
#else
	m_lpDD				= NULL;
#endif // CONVERSION_VS
	m_hWnd				= hWnd;
	m_hInstance			= hInstance;
    //{{ seojt // 2009-1-14, 20:50
    m_dwMainThreadID    = ::GetCurrentThreadId();
    //}} seojt // 2009-1-14, 20:50
	m_bAppDestroy		= false;
	m_fConstElapsedTime	= (1.0f/85.0f);

	m_dTime				= 0.0f;
	m_BeforeFrameTime	= (float)DXUTGetGlobalTimer()->GetTime();
	m_NowFrameTime		= 0.0f;
	m_RemainTime		= 0.0f;

	m_bSkipFrame		= false;
	m_StopFrame			= 0;
	m_StopFrameDelay	= 0;
	m_fSlowFactor		= 1.0f;
	m_fSlowTime			= 0.0f;
	m_fSlowReserveTime	= 0.0f;
	m_bFirstFrame = false;
    m_bFinalFrame = false;

	m_FrameMoveCount	= 0;
	m_RenderCount		= 0;


	m_fFrameMoveFPS		= 0.0f;
	m_fElapsedTimeAdd	= 0.0f;
	m_FrameMoveCountAdd	= 0;

	m_ViewMatChangeCount	= 0;
	m_ResolutionScale.x	= 1.0f;
	m_ResolutionScale.y	= 1.0f;

//{{ robobeg : 2008-10-24
	//m_pBeforeTexture0	= NULL;
	//m_pBeforeTexture1	= NULL;
	//m_pBeforeTexture2	= NULL;
//}} robobeg : 2008-10-24

	ZeroMemory( &m_ViewPort, sizeof(D3DVIEWPORT9) );

	m_pCollision		= new CKTDXCollision();

	m_pDeviceManager	= new CKTDXDeviceManager( m_pd3dDevice );
	m_pDGManager		= new CKTDGManager( m_hWnd, m_pd3dDevice );
	m_pDSManager		= new CKTDSManager( m_hWnd, m_pd3dDevice );
	m_pDIManager		= new CKTDIManager( m_hWnd, m_hInstance );
#ifndef DLL_BUILD
#ifdef CONVERSION_VS
#else
	m_pDNManager		= new CKTDNManager();
#endif // CONVERSION_VS
#endif

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
	m_pDVBManager       = new CKTDGDynamicVBManager();
//#endif


	m_vecStageChain.reserve( 16 );



	m_pDIManager->CreateKeyboard();
	m_pDIManager->CreateMouse(TRUE);
	m_pDIManager->CreateJoystic();
	m_pDIManager->SetGamePadDefaultMap();

	m_pDIManager->SetDefaultMap();
	InsertStage( m_pDSManager );

//#ifdef  DYNAMIC_VERTEX_BUFFER_OPT
	InsertStage( m_pDVBManager );
//#endif

	InsertStage( m_pDeviceManager );
	InsertStage( m_pDGManager );
	InsertStage( m_pDIManager );
#ifndef DLL_BUILD
#ifdef CONVERSION_VS
#else
	InsertStage( m_pDNManager );
#endif // CONVERSION_VS
#endif

	RegisterLuabind( ms_pLuaBinder );


	// dmlee: 2008-11-3, .kom ?????? ?????? ??? .kom?????? ??? ??? ???? enum.lua ?????? ????? ?? ????, x2main?????? ??? ???????
	////{{ robobeg : 2008-10-28
	//hr = LoadKLuaManagerTemplate();
	//ASSERT( SUCCEEDED( hr ) );
	////}} robobeg : 2008-10-28




	m_bNowVeryfy = false;
	m_fVeryfyTime = 0.0f;
	m_bFindHacking = false;

	m_bGameGuardFindHacking = false;

	m_FindHackingDetailMsg	= L"";
	m_bCannotStretchRect	= false;

	InitEnumToString();


#ifdef CONVERSION_VS
#else
	hr = DirectDrawCreateEx(NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL );
	if (FAILED(hr))
		return;
#endif // CONVERSION_VS

//{{ robobeg : 2008-10-28
// ??? ????? ?��??? ???? ???????? ??????? ??? ?????? ?????? ??? ??????? ???? ???
    m_iLuaThreadCounter = 0;
//}} robobeg : 2008-10-28

}

//{{ robobeg : 2011-01-19
//}} robobeg : 2011-01-19

CKTDXApp::~CKTDXApp(void)
{

	m_bAppDestroy = true;

	while( m_GameMsgQ.empty() == false )
	{
		m_GameMsgQ.pop();
	}

	//CoUninitialize();

	int i = (int)m_vecStageChain.size() - 1;
	for( ; i >= 0; i-- )
	{
//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
		if ( m_pDeviceManager != NULL && m_vecStageChain[i] == m_pDeviceManager )
		{
			if ( m_pDSManager != NULL )
				m_pDSManager->ClearAllDevice();

			if ( m_pDVBManager != NULL )
				m_pDVBManager->ClearAllDevice();
		}
//#endif
		SAFE_DELETE( m_vecStageChain[i] );

//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
		if ( m_vecStageChain[i] == m_pDSManager )
			m_pDSManager = NULL;
		else if ( m_vecStageChain[i] == m_pDVBManager )
			m_pDVBManager = NULL;
		else if ( m_vecStageChain[i] == m_pDeviceManager )
			m_pDeviceManager = NULL;
		else if ( m_vecStageChain[i] == m_pDGManager )
			m_pDGManager = NULL;
		else if ( m_vecStageChain[i] == m_pDIManager )
			m_pDIManager = NULL;
//#endif
		m_vecStageChain[i] = NULL;
	}
	m_vecStageChain.clear();

	SAFE_DELETE( m_pCollision );

	SAFE_DELETE( ms_pLuaBinder );

	g_pKTDXApp = NULL;

#ifdef CONVERSION_VS
#else
	if(m_lpDD)
		m_lpDD->Release();
#endif // CONVERSION_VS

	DeleteCriticalSection( &g_csGameMessage );

    SiCKTDXStringTable()->ReleaseInstance();
}

//{{ robobeg : 2008-10-28
// ??? ????? ?��??? ???? ???????? ??????? ??? ?????? ?????? ??? ??????? ???? ???
void    CKTDXApp::EnterLuaThread()
{
    m_iLuaThreadCounter++;
}//CKTDXApp::EnterLuaThread()


void    CKTDXApp::ExitLuaThread()
{
    ASSERT( m_iLuaThreadCounter > 0 );
    if ( m_iLuaThreadCounter > 0 )
        m_iLuaThreadCounter--;
}//CKTDXApp::ExitLuaThread()
//}} robobeg : 2008-10-28



//{{ robobeg : 2008-10-28
// KLuaManager?? ?????? ?? ?? global environment ?? ?????? lua ???? ???? ??????? ???.
// ??????? ?��??? ????��??, global variables ???? ???? ???? ?��????? ???? new env = true ????? ??? ??????
// KLuaManager ?? lua state?????? ???????? ???????.

// HRESULT CKTDXApp::LoadKLuaManagerTemplate()
// {
//     HRESULT hr = S_OK;
//
// 	// luamanager?? enum.lua ?? ???????? ????? ?????? ?��??? CKTDXApp::loadkluamanagertemplate() ???????
// 	// luabinder?? enum.lua ?? ???????? ????? ?????? ?��??? cx2main::loadluaenum() ???????
//
//     KLuaManager kLuaManager( m_LuaBinder.GetLuaState(), 0, true );
//     luaopen_base( kLuaManager.GetLuaState() );
//     bool bLoaded;
//     bLoaded = LoadAndDoMemory( &kLuaManager, L"Enum.lua" );
//     ASSERT( bLoaded );
//     if ( !bLoaded )
//         hr = E_FAIL;
//     bLoaded = LoadAndDoMemory( &kLuaManager, L"StringID_def.lua" );
//     ASSERT( bLoaded );
//     if ( !bLoaded )
//         hr = E_FAIL;
//     if ( FAILED( kLuaManager.SetAsTemplateEnvironment() ) )
//         hr = E_FAIL;
//
//     return hr;
// }//CKTDXApp::LoadKLuaManagerTemplate()

//}} robobeg : 2008-10-28


// #ifdef BACKGROUND_LOADING_TEST // 2008-10-17
//
//void CKTDXApp::OnPrePresent()
//{
//	GetDeviceManager()->OnPrePresent();
//}//CKTDXApp::OnPrePresent()
//
//
//void CKTDXApp::OnPostPresent()
//{
//	GetDeviceManager()->OnPostPresent();
//}//CKTDXApp::OnPostPresent()
//
// #endif // BACKGROUND_LOADING_TEST // 2008-10-17

HRESULT CKTDXApp::OnFrameMove( double dTime, float fElapsedTime )
{
	KLagCheck( eUnKnown_LagCheckType_Simulation );
	KTDXPROFILE();
	m_fElapsedTimeAdd += fElapsedTime;

	m_pd3dDevice->SetTransform( D3DTS_WORLD,		&m_WorldMatrix		);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,			&m_ViewMatrix		);
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION,	&m_ProjectionMatrix	);

	m_bFirstFrame = true;


	m_NowFrameTime = (float)DXUTGetGlobalTimer()->GetTime();
	float fRealElapsedTime = m_NowFrameTime - m_BeforeFrameTime;
	fRealElapsedTime += m_RemainTime;

	if( m_bSkipFrame == true )
	{
		m_BeforeFrameTime	= m_NowFrameTime;
		m_RemainTime		= 0.0f;
		m_bSkipFrame		= false;
		return S_OK;
	}

	if( fRealElapsedTime <= m_fConstElapsedTime )
	{
		//???? ?��? ??? ??????.
		float fWaitTime = m_fConstElapsedTime - fRealElapsedTime;
		if( fWaitTime > 0.f )
		{
// #ifdef BACKGROUND_LOADING_TEST // 2008-10-20
// 			OnPrePresent();
// 			Sleep( (DWORD)(fWaitTime*1000.0f) );
// 			OnPostPresent();
// #else // BACKGROUND_LOADING_TEST // 2008-10-20
			Sleep( (DWORD)(fWaitTime*1000.0f) );
//#endif // BACKGROUND_LOADING_TEST // 2008-10-20
		}

		fWaitTime = 0.0f;

		m_NowFrameTime = (float)DXUTGetGlobalTimer()->GetTime();
		m_BeforeFrameTime = m_NowFrameTime;

		if( m_StopFrameDelay > 0 )
		{
			m_StopFrameDelay--;
		}

		if( m_StopFrame > 0 && m_StopFrameDelay <= 0 )
		{
			m_StopFrame--;
		}
		else
		{
			m_bFinalFrame = true;
			FrameProcess();
			m_bFirstFrame = false;
		}

		m_RemainTime = fWaitTime;

		m_FrameMoveCount++;
		m_FrameMoveCountAdd++;
	}
	else
	{
		m_BeforeFrameTime = m_NowFrameTime;

		while( fRealElapsedTime > m_fConstElapsedTime )
		{
			if( m_bSkipFrame == true )
				break;

			float fActualNowTime = (float)DXUTGetGlobalTimer()->GetTime();
			if ( fActualNowTime - m_NowFrameTime > 1.f )
			{
				fRealElapsedTime = 0.f;
				m_BeforeFrameTime = m_NowFrameTime = fActualNowTime;
				break;
			}

			if( m_StopFrame > 0 )
			{
				m_StopFrame--;
			}
			else
			{
				//?????? ??????? ??????????...
				if( fRealElapsedTime - m_fConstElapsedTime < m_fConstElapsedTime )
					m_bFinalFrame = true;
				else
					m_bFinalFrame = false;
				FrameProcess();
				m_bFirstFrame = false;
			}

			m_FrameMoveCount++;
			m_FrameMoveCountAdd++;

			fRealElapsedTime -= m_fConstElapsedTime;

		}
		m_RemainTime = fRealElapsedTime;
	}

	if( m_FrameMoveCountAdd >= 100 )
	{
		m_fFrameMoveFPS = (1.0f/m_fElapsedTimeAdd) * m_FrameMoveCountAdd;
		m_fElapsedTimeAdd	= 0.0f;
		m_FrameMoveCountAdd	= 0;
	}



	return S_OK;
}

void CKTDXApp::FrameProcess()
{
	KTDXPROFILE();
	m_dTime += m_fConstElapsedTime;
	g_NowTime = m_dTime;


	THEMIDA_ENCODE_START

		m_bNowVeryfy = false;
	m_fVeryfyTime -= m_fConstElapsedTime;

	if ( m_fVeryfyTime <= 0.0f )
	{
		m_fVeryfyTime = (float)((rand() % 3) + 1);
		m_bNowVeryfy = true;
	}

	THEMIDA_ENCODE_END


		while( true )
		{
			EnterCriticalSection( &g_csGameMessage );
			if( true == m_GameMsgQ.empty() )
			{
				LeaveCriticalSection( &g_csGameMessage );
				break;
			}
			GameMessage gameMessage = m_GameMsgQ.front();
			m_GameMsgQ.pop();
			LeaveCriticalSection( &g_csGameMessage );

#ifdef TEMP_CRASH_LOG
			if( gameMessage.uMsg == XGM_QUIT_GAME )
			{
				KLOG("CrashLog.txt")  << L"[FrameProcess]XGM_QUIT_GAME Send" << fileout;
			}
#endif // TEMP_CRASH_LOG

			bool bMsgHandle = false;
			for( UINT i = 0; i < m_vecStageChain.size(); i++ )
			{
				CKTDXStage* pCKTDXStage = m_vecStageChain[i];
				if( pCKTDXStage->GetStageState() == CKTDXStage::SS_ACTIVE )
				{
					if( pCKTDXStage->MsgProc( gameMessage.hWnd, gameMessage.uMsg, gameMessage.wParam, gameMessage.lParam ) == true )
					{
						bMsgHandle = true;
						break;
					}
				}
			}
			if( gameMessage.uMsg == KM_RECEIVE_FROM_P2P && bMsgHandle == false && gameMessage.lParam != NULL )
			{
				CKTDNPacket::PacketData* pPacketData = (CKTDNPacket::PacketData*)gameMessage.lParam;
				SAFE_DELETE( pPacketData );
			}
		}


		if( m_fSlowTime > 0.0f )
		{
			if( m_fSlowReserveTime > 0.0f )
			{
				m_fSlowReserveTime -= m_fConstElapsedTime;
				if( m_fSlowReserveTime < 0.0f )
					m_fSlowReserveTime = 0.0f;
			}
			else
			{
				m_fSlowTime -= m_fConstElapsedTime;

				if( m_fSlowTime < 0.0f )
					m_fSlowTime = 0.0f;
			}
		}

		for( UINT i = 0; i < m_vecStageChain.size(); i++ )
		{
			CKTDXStage* pCKTDXStage = m_vecStageChain[i];
			if( pCKTDXStage != NULL && pCKTDXStage->GetStageState() == CKTDXStage::SS_ACTIVE )
			{
				if( m_fSlowTime > 0.0f && m_fSlowReserveTime <= 0.0f )
					pCKTDXStage->OnFrameMove( m_dTime, m_fConstElapsedTime * m_fSlowFactor );
				else
					pCKTDXStage->OnFrameMove( m_dTime, m_fConstElapsedTime );
			}
		}
		//???????? ????
		for( UINT i = 0; i < m_vecStageChain.size(); i++ )
		{
			CKTDXStage* pCKTDXStage = m_vecStageChain[i];
			if( pCKTDXStage != NULL && pCKTDXStage->GetStageState() == CKTDXStage::SS_READY_TO_DELETE )
			{
				SAFE_DELETE( pCKTDXStage );
				m_vecStageChain.erase(m_vecStageChain.begin()+i);
				--i;
			}
		}
		//LeaveCriticalSection( &m_GameLock );

		//{{ robobeg : 2008-10-21

		LuaPerFrameGarbageCollection();
}

/*static*/
void    CKTDXApp::LuaPerFrameGarbageCollection( int iNumSteps )
{
	//{{ robobeg : 2008-10-21

	if ( ms_pLuaBinder != NULL && ms_pLuaBinder->GetLuaState() != NULL )
	{
		lua_gc( ms_pLuaBinder->GetLuaState(), LUA_GCSTEP, iNumSteps );
	}//if
	//}} robobeg : 2008-10-21
}//CKTDXApp::LuaPerFrameGarbageCollection()

HRESULT CKTDXApp::OnFrameRender()
{
	KLagCheck( eUnKnown_LagCheckType_Rendering );
	KTDXPROFILE();



	m_RenderCount++;

//{{ robobeg : 2008-10-24
	//m_pBeforeTexture0	= NULL;
	//m_pBeforeTexture1	= NULL;
	//m_pBeforeTexture2	= NULL;
//}} robobeg : 2008-10-24

	m_pd3dDevice->SetTransform( D3DTS_WORLD,		&m_WorldMatrix		);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,			&m_ViewMatrix		);
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION,	&m_ProjectionMatrix	);


	if( m_pDGManager->BeginRender() == false )
		return S_OK;

	for( UINT i = 0; i < m_vecStageChain.size(); i++ )
	{
		CKTDXStage* pCKTDXStage = m_vecStageChain[i];
		if( pCKTDXStage != NULL && pCKTDXStage->GetStageState() == CKTDXStage::SS_ACTIVE )
		{
			pCKTDXStage->OnFrameRender();
		}
	}

	m_pDGManager->EndRender();

	return S_OK;
}

bool CKTDXApp::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	//CKTDXThread::CLocker locker( g_csGameMessage );
	CSLock CriticalSectionLock( g_csGameMessage );

	KTDXPROFILE();
	bool retval = false;
	if( uMsg >= KM_SYSTEM_BASE )
	{
		GameMessage gameMessage;
		gameMessage.hWnd		= hWnd;
		gameMessage.uMsg		= uMsg;
		gameMessage.wParam		= wParam;
		gameMessage.lParam		= lParam;

#ifdef TEMP_CRASH_LOG
		if( gameMessage.uMsg == XGM_QUIT_GAME )
		{
			KLOG("CrashLog.txt")  << L"[MsgProc]XGM_QUIT_GAME Send" << fileout;
		}
#endif // TEMP_CRASH_LOG

		m_GameMsgQ.push( gameMessage );
		retval = true;
	}
	else
	{
		for( UINT i = 0; i < m_vecStageChain.size(); i++ )
		{
			if( NULL == m_vecStageChain[i] )
				continue;

			if( m_vecStageChain[i]->GetStageState() == CKTDXStage::SS_ACTIVE )
			{
				if( m_vecStageChain[i]->MsgProc( hWnd, uMsg, wParam, lParam ) == true )
				{
					retval = true;
					break;
				}
			}
		}
	}

	return retval;
}

bool CKTDXApp::SendGameMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, bool bDirectProcess )
{
#ifdef TEMP_CRASH_LOG
	switch ( uMsg )
	{
	case XGM_STATE_CHANGE:
		KLOG("CrashLog.txt")  << L"[Send]XGM_STATE_CHANGE " << fileout;
		break;
	case XGM_DELETE_DIALOG:
		KLOG("CrashLog.txt")  << L"[Send]XGM_DELETE_DIALOG" << fileout;
		break;
	case XGM_REQUEST_FOCUS_CONTROL:
		KLOG("CrashLog.txt")  << L"[Send]XGM_REQUEST_FOCUS_CONTROL" << fileout;
		break;
	case XGM_RECEIVE_FROM_SERVER:
		KLOG("CrashLog.txt")  << L"[Send]XGM_RECEIVE_FROM_SERVER" << fileout;
		break;
	case XGM_SERVER_PACKET_TIMEOUT:
		KLOG("CrashLog.txt")  << L"[Send]XGM_SERVER_PACKET_TIMEOUT" << fileout;
		break;
	case XGM_NEXON_CALLBACK:
		KLOG("CrashLog.txt")  << L"[Send]XGM_NEXON_CALLBACK" << fileout;
		break;
	case XGM_QUIT_GAME:
		KLOG("CrashLog.txt")  << L"[Send]XGM_QUIT_GAME" << fileout;
		break;
	case XGM_RECEIVE_FROM_COLLECTSERVER:
		KLOG("CrashLog.txt")  << L"[Send]XGM_RECEIVE_FROM_COLLECTSERVER" << fileout;
		break;
	case KM_WEB_BROWSER_KEY_EVENT:
		KLOG("CrashLog.txt")  << L"[Send]KM_WEB_BROWSER_KEY_EVENT" << fileout;
		break;
	default:
		KLOG("CrashLog.txt")  << L"[Send]Other Message :" << uMsg << fileout;
		break;
	}
#endif // TEMP_CRASH_LOG

	//CKTDXThread::CLocker locker( g_csGameMessage );
	CSLock CriticalSectionLock( g_csGameMessage );

    ASSERT( uMsg != XGM_DELETE_DIALOG || wParam != 0xcdcdcdcd );

	KTDXPROFILE();
	bool bHandle = false;
	if( bDirectProcess == true )
	{
		for( UINT i = 0; i < m_vecStageChain.size(); i++ )
		{
			CKTDXStage* pStage = m_vecStageChain[i];
			if( pStage->GetStageState() == CKTDXStage::SS_ACTIVE )
			{
				bHandle = pStage->MsgProc( GetHWND(), uMsg, wParam, lParam );
				if( bHandle == true )
				{
					break;
					//return true;
				}
			}
		}
		if( uMsg == KM_RECEIVE_FROM_P2P && lParam != NULL && bHandle == false )
		{
			CKTDNPacket::PacketData* pPacketData = (CKTDNPacket::PacketData*)lParam;
			if( pPacketData->senderID < 0 || pPacketData->packetType < 0 )
				return bHandle;

			SAFE_DELETE( pPacketData );
		}
		return bHandle;
	}
	else
	{
		GameMessage gameMessage;
		gameMessage.hWnd		= GetHWND();
		gameMessage.uMsg		= uMsg;
		gameMessage.wParam		= wParam;
		gameMessage.lParam		= lParam;

		m_GameMsgQ.push( gameMessage );
	}
	return true;
}


//{{ seojt // 2009-8-19, 16:46
bool CKTDXApp::SendGameDlgMessage(UINT uMsg, CKTDGUIDialogType wParam, LPARAM lParam, bool bDirectProcess /*=true*/)
{
	//return SendGameMessage( uMsg, (WPARAM)wParam.GetHandle().GetHandlePart(), lParam, bDirectProcess );

	// added by wonpok. 20090820.
	return SendGameMessage( uMsg, (WPARAM)wParam.GetHandle().GetHandlePart(), (LPARAM)wParam.GetHandle().GetMagicPart(), bDirectProcess );
}//CKTDXApp::SendGameDlgMessage()
//}} seojt // 2009-8-19, 16:46


//{{ robobeg : 2008-10-24
// CKTDGStateManager::SetTexture ?? ???

//bool CKTDXApp::IsSameBeforeTexture( int stage, LPDIRECT3DTEXTURE9 tex )
//{
//	KTDXPROFILE();
//	switch( stage )
//	{
//		case 0:
//			if( m_pBeforeTexture0 == tex )
//				return true;
//			else
//				return false;
//			break;
//
//		case 1:
//			if( m_pBeforeTexture1 == tex )
//				return true;
//			else
//				return false;
//			break;
//
//		case 2:
//			if( m_pBeforeTexture2 == tex )
//				return true;
//			else
//				return false;
//			break;
//	}
//	return false;
//}
//
//void CKTDXApp::SetBeforeTexture( int stage, LPDIRECT3DTEXTURE9 tex )
//{
//	KTDXPROFILE();
//	switch( stage )
//	{
//		case 0:
//			m_pBeforeTexture0 = tex;
//			break;
//
//		case 1:
//			m_pBeforeTexture1 = tex;
//			break;
//
//		case 2:
//			m_pBeforeTexture2 = tex;
//			break;
//	}
//}
//
//void CKTDXApp::SetNULLTexture( int stage )
//{
//	KTDXPROFILE();
//	m_pd3dDevice->SetTexture( stage, NULL );
//
//	switch( stage )
//	{
//		case 0:
//			m_pBeforeTexture0 = NULL;
//			break;
//
//		case 1:
//			m_pBeforeTexture1 = NULL;
//			break;
//
//		case 2:
//			m_pBeforeTexture2 = NULL;
//			break;
//	}
//}

//}} robobeg : 2008-10-24

HRESULT CKTDXApp::OnResetDevice( const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
	if ( m_pDeviceManager != NULL )
		m_pDeviceManager->BeginFullSync();

	SetViewport( 0, 0, pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height,
		0.f, 1.f );

	m_pDGManager->SetProjection( GetDGManager()->GetNear(), GetDGManager()->GetFar(), m_pDGManager->GetPerspective(), m_pDGManager->GetOrthoScale() );

	for( UINT i = 0; i < m_vecStageChain.size(); i++ )
	{
		if ( m_vecStageChain[i] != NULL )
			m_vecStageChain[i]->OnResetDevice();
	}

	if ( m_pDeviceManager != NULL )
		m_pDeviceManager->EndFullSync();

	SkipFrame();

	return S_OK;
}

HRESULT CKTDXApp::OnLostDevice()
{
	if ( m_pDeviceManager != NULL )
		m_pDeviceManager->BeginFullSync();

	for( UINT i = 0; i < m_vecStageChain.size(); i++ )
	{
		if ( m_vecStageChain[i] != NULL )
			m_vecStageChain[i]->OnLostDevice();
	}

	if ( m_pDeviceManager != NULL )
		m_pDeviceManager->EndFullSync();

	SkipFrame();

	return S_OK;
}

void CKTDXApp::InsertStage( CKTDXStage* stage )
{
	m_vecStageChain.push_back( stage );
}

bool CKTDXApp::RemoveStage( CKTDXStage* stage, bool bImmediate )
{
	for( UINT i = 0; i < m_vecStageChain.size(); i++ )
	{
		if( m_vecStageChain[i] == stage )
		{
			m_vecStageChain[i]->SetStageState( CKTDXStage::SS_READY_TO_DELETE );
			if( true == bImmediate )
			{
				//???????? ????
				CKTDXStage* pCKTDXStage = m_vecStageChain[i];
				SAFE_DELETE( pCKTDXStage );
				m_vecStageChain.erase(m_vecStageChain.begin()+i);
			}
			return true;
		}
	}
	return false;
}

bool CKTDXApp::CheckStage( CKTDXStage* stage )
{
	for( UINT i = 0; i < m_vecStageChain.size(); i++ )
	{
		if( m_vecStageChain[i] == stage )
		{
			return true;
		}
	}

	return false;
}

CKTDXStage* CKTDXApp::GetStage( const WCHAR* wszStageName )
{
	CKTDXStage* pStage = NULL;

	for( UINT i = 0; i < m_vecStageChain.size(); i++ )
	{
		if( m_vecStageChain[i]->GetStageName().compare( wszStageName ) == 0 )
		{
			return m_vecStageChain[i];
		}
	}

	return pStage;
}

void CKTDXApp::RegisterLuabind( KLuabinder* pKLuabinder )
{
	if ( pKLuabinder == NULL )
		return;

// 	pKLuabinder->Destroy();
// 	pKLuabinder->Initialize();
// 	pKLuabinder->SetLuaTinker();


	//D3D ?????
	lua_State* L = pKLuabinder->GetLuaState();

	lua_tinker::class_add<D3DXVECTOR2>( L, "D3DXVECTOR2" );
	lua_tinker::class_con<D3DXVECTOR2>( L, lua_tinker::constructor<float, float>() );
	lua_tinker::class_mem<D3DXVECTOR2>( L, "x", &D3DXVECTOR2::x );
	lua_tinker::class_mem<D3DXVECTOR2>( L, "y", &D3DXVECTOR2::y );

	lua_tinker::class_add<D3DXVECTOR3>( L, "D3DXVECTOR3" );
	lua_tinker::class_con<D3DXVECTOR3>( L, lua_tinker::constructor<float, float, float>() );
	lua_tinker::class_mem<D3DXVECTOR3>( L, "x", &D3DXVECTOR3::x );
	lua_tinker::class_mem<D3DXVECTOR3>( L, "y", &D3DXVECTOR3::y );
	lua_tinker::class_mem<D3DXVECTOR3>( L, "z", &D3DXVECTOR3::z );

	lua_tinker::class_add<D3DXCOLOR>( L, "D3DXCOLOR" );
	lua_tinker::class_con<D3DXCOLOR>( L, lua_tinker::constructor<float,float,float,float>() );
	lua_tinker::class_mem<D3DXCOLOR>( L, "a", &D3DXCOLOR::a);
	lua_tinker::class_mem<D3DXCOLOR>( L, "r", &D3DXCOLOR::r);
	lua_tinker::class_mem<D3DXCOLOR>( L, "g", &D3DXCOLOR::g);
	lua_tinker::class_mem<D3DXCOLOR>( L, "b", &D3DXCOLOR::b);





	lua_tinker::class_add<CKTDGXRenderer::RenderParam>( L, "RenderParam" );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "renderType",		&CKTDGXRenderer::RenderParam::renderType );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "cartoonTexType",	&CKTDGXRenderer::RenderParam::cartoonTexType );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "lightPos",			&CKTDGXRenderer::RenderParam::lightPos );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "fOutLineWide",		&CKTDGXRenderer::RenderParam::fOutLineWide );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "outLineColor",		&CKTDGXRenderer::RenderParam::outLineColor );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "color",				&CKTDGXRenderer::RenderParam::color );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "fLightFlowWide",	&CKTDGXRenderer::RenderParam::fLightFlowWide );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "fLightFlowImpact",	&CKTDGXRenderer::RenderParam::fLightFlowImpact );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "lightFlowPoint",	&CKTDGXRenderer::RenderParam::lightFlowPoint );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "texOffsetStage0",	&CKTDGXRenderer::RenderParam::texOffsetStage0 );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "texOffsetStage1",	&CKTDGXRenderer::RenderParam::texOffsetStage1 );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "texOffsetStage2",	&CKTDGXRenderer::RenderParam::texOffsetStage2 );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "bAlphaBlend",		&CKTDGXRenderer::RenderParam::bAlphaBlend );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "bZWriteEnable",		&CKTDGXRenderer::RenderParam::bZWriteEnable );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "bZEnable",			&CKTDGXRenderer::RenderParam::bZEnable );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "cullMode",			&CKTDGXRenderer::RenderParam::cullMode );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "srcBlend",			&CKTDGXRenderer::RenderParam::srcBlend );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "destBlend",			&CKTDGXRenderer::RenderParam::destBlend );
	lua_tinker::class_mem<CKTDGXRenderer::RenderParam>( L, "bAlphaTestEnable",	&CKTDGXRenderer::RenderParam::bAlphaTestEnable );

	lua_tinker::class_add<CKTDGUIControl::UIStringData>( L, "UIStringData" );
	lua_tinker::class_def<CKTDGUIControl::UIStringData>( L, "SetMsg",			&CKTDGUIControl::UIStringData::SetMsg_LUA );
	lua_tinker::class_mem<CKTDGUIControl::UIStringData>( L, "fontIndex",		&CKTDGUIControl::UIStringData::fontIndex );
	lua_tinker::class_mem<CKTDGUIControl::UIStringData>( L, "pos",				&CKTDGUIControl::UIStringData::pos );
	lua_tinker::class_mem<CKTDGUIControl::UIStringData>( L, "color",			&CKTDGUIControl::UIStringData::color );
	lua_tinker::class_mem<CKTDGUIControl::UIStringData>( L, "outlineColor",		&CKTDGUIControl::UIStringData::outlineColor );
	lua_tinker::class_mem<CKTDGUIControl::UIStringData>( L, "sortFlag",			&CKTDGUIControl::UIStringData::sortFlag );
	lua_tinker::class_mem<CKTDGUIControl::UIStringData>( L, "fontStyle",		&CKTDGUIControl::UIStringData::fontStyle );




	lua_tinker::class_add<CKTDXTimer>( L, "CKTDXTimer" );
	lua_tinker::class_def<CKTDXTimer>( L, "restart",					&CKTDXTimer::restart );
	lua_tinker::class_def<CKTDXTimer>( L, "elapsed",					&CKTDXTimer::elapsed );



	lua_tinker::class_add<CKTDGMatrix>( L, "CKTDGMatrix" );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetXPos",					&CKTDGMatrix::GetXPos );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetZPos",					&CKTDGMatrix::GetZPos );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetXRotate",				&CKTDGMatrix::GetXRotate );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetYRotate",				&CKTDGMatrix::GetYRotate );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetZRotate",				&CKTDGMatrix::GetZRotate );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetXRotateDegree",			&CKTDGMatrix::GetXRotateDegree );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetYRotateDegree",			&CKTDGMatrix::GetYRotateDegree );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetZRotateDegree",			&CKTDGMatrix::GetZRotateDegree );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetXScale",					&CKTDGMatrix::GetXScale );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetYScale",					&CKTDGMatrix::GetYScale );
	lua_tinker::class_def<CKTDGMatrix>( L, "GetZScale",					&CKTDGMatrix::GetZScale );




	//XET
	lua_tinker::class_add<CKTDXCollision>( L, "CKTDXCollision" );

	lua_tinker::class_add<CKTDXCollision::CollisionData>( L, "CollisionData" );
	lua_tinker::class_def<CKTDXCollision::CollisionData>( L, "SetPoint1",			&CKTDXCollision::CollisionData::SetPoint1_LUA );
	lua_tinker::class_def<CKTDXCollision::CollisionData>( L, "SetPoint2",			&CKTDXCollision::CollisionData::SetPoint2_LUA );


    CKTDXDeviceTET_Preprocessing::RegisterLuabind( L );
    CKTDXDeviceXET_Preprocessing::RegisterLuabind( L );








	lua_tinker::class_add<CKTDGObject>( L, "CKTDGObject" );
	lua_tinker::class_def<CKTDGObject>( L, "SetLayer",					&CKTDGObject::SetLayer );
	lua_tinker::class_def<CKTDGObject>( L, "SetForceLayer",				&CKTDGObject::SetForceLayer );
	lua_tinker::class_def<CKTDGObject>( L, "SetAlphaObject",			&CKTDGObject::SetAlphaObject );
	lua_tinker::class_def<CKTDGObject>( L, "SetScale",					&CKTDGObject::SetScale );
	lua_tinker::class_def<CKTDGObject>( L, "SetOverUI",					&CKTDGObject::SetOverUI );


	//??????
	lua_tinker::class_add<CKTDGSkyDome>( L, "CKTDGSkyDome" );
	lua_tinker::class_inh<CKTDGSkyDome, CKTDGObject>(L);
	lua_tinker::class_def<CKTDGSkyDome>( L, "GetRenderParam",			&CKTDGSkyDome::GetRenderParam );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetTex0UVSpeed",			&CKTDGSkyDome::SetTex0UVSpeed );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetTex1UVSpeed",			&CKTDGSkyDome::SetTex1UVSpeed );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetTex2UVSpeed",			&CKTDGSkyDome::SetTex2UVSpeed );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetRotationSpeed",			&CKTDGSkyDome::SetRotationSpeed );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetDomeMesh",				&CKTDGSkyDome::SetDomeMesh_LUA );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetMultiTexXET",			&CKTDGSkyDome::SetMultiTexXET_LUA );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetChangeTexXET",			&CKTDGSkyDome::SetChangeTexXET_LUA );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetViewTrace",				&CKTDGSkyDome::SetViewTrace );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetViewTraceXZ",			&CKTDGSkyDome::SetViewTraceXZ );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetViewRotation",			&CKTDGSkyDome::SetViewRotation );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetPos",					&CKTDGSkyDome::SetPos );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetUseWorldColor",			&CKTDGSkyDome::SetUseWorldColor );
	lua_tinker::class_def<CKTDGSkyDome>( L, "SetSkyDomeColor",			&CKTDGSkyDome::SetSkyDomeColor );

    lua_tinker::class_def<CKTDGSkyDome>( L, "SetFog",					&CKTDGSkyDome::SetFog );


	lua_tinker::class_add<CKTDGLensFlare>( L, "CKTDGLensFlare" );
	lua_tinker::class_inh<CKTDGLensFlare, CKTDGObject>(L);
	lua_tinker::class_def<CKTDGLensFlare>( L, "SetLightPosition",			&CKTDGLensFlare::SetLightPosition );



	//???��?
	lua_tinker::class_add<CKTDGLineMap>( L, "CKTDGLineMap" );
	lua_tinker::class_def<CKTDGLineMap>( L, "SetLandHeight",				&CKTDGLineMap::SetLandHeight);
	lua_tinker::class_def<CKTDGLineMap>( L, "SetTeamStartPos",				&CKTDGLineMap::SetTeamStartPos);
	lua_tinker::class_def<CKTDGLineMap>( L, "AddStartPos",					&CKTDGLineMap::AddStartPos);
	lua_tinker::class_def<CKTDGLineMap>( L, "AddCameraData_LUA",			&CKTDGLineMap::AddCameraData_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "AddLine_LUA",					&CKTDGLineMap::AddLine_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "AddLineAnim_LUA",				&CKTDGLineMap::AddLineAnim_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetRandomPosition_LUA",		&CKTDGLineMap::GetRandomPosition_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetRandomPosition",			&CKTDGLineMap::GetRandomPosition);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetRandomPositionDir",			&CKTDGLineMap::GetRandomPositionDir);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetNearest_LUA",	        	&CKTDGLineMap::GetNearest_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetStartPosition",				&CKTDGLineMap::GetStartPosition);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetLandPosition_LUA",			&CKTDGLineMap::GetLandPosition_LUA);

//{{ kimhc // 2010.7.6 // ????? ?????? ???��?
	lua_tinker::class_def<CKTDGLineMap>( L, "EnableLineData_LUA",				&CKTDGLineMap::EnableLineData_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "SetStartEndPosAtThisLineMap_LUA",	&CKTDGLineMap::SetStartEndPosAtThisLineMap_LUA);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetNumLineData_LUA",				&CKTDGLineMap::GetNumLineData);
//}} kimhc // 2010.7.6 // ????? ?????? ???��?
	lua_tinker::class_def<CKTDGLineMap>( L, "AddTrigger_LUA",					&CKTDGLineMap::AddTrigger_LUA);

	lua_tinker::class_def<CKTDGLineMap>( L, "GetStartLineIndex",				&CKTDGLineMap::GetStartLineIndex);
	lua_tinker::class_def<CKTDGLineMap>( L, "GetRandomStartPosIndex_LUA",		&CKTDGLineMap::GetRandomStartPosIndex);

	//???
	lua_tinker::class_add<CKTDGFontManager>( L, "CKTDGFontManager" );
	lua_tinker::class_def<CKTDGFontManager>( L, "CreateNewUIFont",			&CKTDGFontManager::CreateNewUIFont_LUA );
	lua_tinker::class_def<CKTDGFontManager>( L, "CreateNewUIUKFont",		&CKTDGFontManager::CreateNewUIUKFont_LUA );
	lua_tinker::class_def<CKTDGFontManager>( L, "MapUIFontToUKFont",		&CKTDGFontManager::MapUIFontToUKFont );
#ifdef DLL_BUILD
	lua_tinker::class_def<CKTDGFontManager>( L, "InstallFont",		&CKTDGFontManager::InstallFont );
#endif // DLL_BUILD

	lua_tinker::decl( L, "g_pFontManager", m_pDGManager->GetFontManager() );

	//UI
	lua_tinker::def( L,  "IncludeLua",		&IncludeLua_  );
	lua_tinker::def( L,  "GetDistance_LUA", &GetDistance_LUA  );
	lua_tinker::def( L,  "RandomFloat_LUA", &RandomFloat );
	lua_tinker::def( L,  "RandomInt_LUA",	&RandomInt );

	//{{ JHKang / ??????/ 2010/12/21
	lua_tinker::def( L,  "GetDirVecToDegree",	&GetDirVecToDegree );
	//{{ JHKang / ??????/ 2010/12/21

	//lua_tinker::class_add<CKTDGUIControl>( L, "CKTDGUIControl" );				// ??? ?? ???? control??....
	lua_tinker::class_add<CKTDGUIDialog>( L, "CKTDGUIDialog" );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetName",							&CKTDGUIDialog::SetName_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetPos",							&CKTDGUIDialog::SetPos_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetModal",						&CKTDGUIDialog::SetModal );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreatePicture",					&CKTDGUIDialog::CreatePicture );


// #ifdef MOVIE_TEST	 	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateMovie",						&CKTDGUIDialog::CreateMovie );

// #endif //  MOVIE_TEST	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateButton",					&CKTDGUIDialog::CreateButton );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateCheckBox",					&CKTDGUIDialog::CreateCheckBox );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateEditBox",					&CKTDGUIDialog::CreateEditBox );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateIMEEditBox",				&CKTDGUIDialog::CreateIMEEditBox );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateStatic",					&CKTDGUIDialog::CreateStatic );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateRadioButton",				&CKTDGUIDialog::CreateRadioButton );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateComboBox",					&CKTDGUIDialog::CreateComboBox );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateListBox",					&CKTDGUIDialog::CreateListBox );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateSlider",					&CKTDGUIDialog::CreateSlider );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateNavigation",				&CKTDGUIDialog::CreateNavigation );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateControlList",				&CKTDGUIDialog::CreateControlList );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateSlot",						&CKTDGUIDialog::CreateSlot );
	lua_tinker::class_def<CKTDGUIDialog>( L, "CreateItem",						&CKTDGUIDialog::CreateItem );
	lua_tinker::class_def<CKTDGUIDialog>( L, "AddControl",						&CKTDGUIDialog::AddControl );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetFront",						&CKTDGUIDialog::SetFront );
	lua_tinker::class_def<CKTDGUIDialog>( L, "Move",							&CKTDGUIDialog::Move_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetEnableMoveByDrag",				&CKTDGUIDialog::SetEnableMoveByDrag_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "AddDummyPos",						&CKTDGUIDialog::AddDummyPos );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetDummyPos",						&CKTDGUIDialog::GetDummyPos );
	lua_tinker::class_def<CKTDGUIDialog>( L, "AddDummyString",					&CKTDGUIDialog::AddDummyString_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetDummyString",					&CKTDGUIDialog::GetDummyString );
	lua_tinker::class_def<CKTDGUIDialog>( L, "AddDummyInt",						&CKTDGUIDialog::AddDummyInt );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetDummyInt",						&CKTDGUIDialog::GetDummyInt );
	//lua_tinker::class_def<CKTDGUIDialog>( L, "GetControl",						&CKTDGUIDialog::GetControl_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetButton",						&CKTDGUIDialog::GetButton_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetCheckBox",						&CKTDGUIDialog::GetCheckBox_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetComboBox",						&CKTDGUIDialog::GetComboBox_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetEditBox",						&CKTDGUIDialog::GetEditBox_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetIMEEdieBox",					&CKTDGUIDialog::GetIMEEdieBox_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetListBox",						&CKTDGUIDialog::GetListBox_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetRadioButton",					&CKTDGUIDialog::GetRadioButton_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "GetStatic",						&CKTDGUIDialog::GetStatic_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetLayer",						&CKTDGUIDialog::SetLayer );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetColor",						&CKTDGUIDialog::SetColor );

//{{ robobeg : 2009-01-07
	//lua_tinker::class_def<CKTDGUIDialog>( L, "GetKeyID",						&CKTDGUIDialog::GetKeyID );
//}} robobeg : 2009-01-07
//{{ robobeg : 2009-01-07
	//lua_tinker::class_def<CKTDGUIDialog>( L, "SetDockDialogKeyID",				&CKTDGUIDialog::SetDockDialogKeyID );
//}} robobeg : 2009-01-07
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCloseOnFocusOut",				&CKTDGUIDialog::SetCloseOnFocusOut );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCloseOnFocusOutEvent",			&CKTDGUIDialog::SetCloseOnFocusOutEvent );
	lua_tinker::class_def<CKTDGUIDialog>( L, "ChangeSequenceByName",			&CKTDGUIDialog::ChangeSequenceByName_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "ChangeSequenceByNameAndPivot",	&CKTDGUIDialog::ChangeSequenceByNameAndPivot_LUA );

	lua_tinker::class_def<CKTDGUIDialog>( L, "SetSize",							&CKTDGUIDialog::SetSize );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetEnableDraggingRange", 			&CKTDGUIDialog::SetEnableDraggingRange );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetDraggingRangeCenter", 			&CKTDGUIDialog::SetDraggingRangeCenter );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetDraggingRange",				&CKTDGUIDialog::SetDraggingRange );

	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCloseCustomUIEventID",			&CKTDGUIDialog::SetCloseCustomUIEventID );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCustomMsgStopDragging",		&CKTDGUIDialog::SetCustomMsgStopDragging );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCustomMsgStartDragging",		&CKTDGUIDialog::SetCustomMsgStartDragging );

	lua_tinker::class_def<CKTDGUIDialog>( L, "SetTimeForSelfDelete",			&CKTDGUIDialog::SetTimeForSelfDelete );

	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCustomMsgRMouseUp",			&CKTDGUIDialog::SetCustomMsgRMouseUp );

	// ?????? Dialog ?? ?????? ????? ???? ??????? ???????.
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCustomButtonMouseOverSndFile",			&CKTDGUIDialog::SetCustomButtonMouseOverSndFile_LUA );
	lua_tinker::class_def<CKTDGUIDialog>( L, "SetCustomButtonMouseUpSndFile",			&CKTDGUIDialog::SetCustomButtonMouseUpSndFile_LUA );






	lua_tinker::class_add<CKTDGUIDialogManager>( L, "CKTDGUIDialogManager" );
	lua_tinker::class_def<CKTDGUIDialogManager>( L, "GetDialog",				&CKTDGUIDialogManager::GetDialog_LUA );
	lua_tinker::class_def<CKTDGUIDialogManager>( L, "SetShow",					&CKTDGUIDialogManager::SetShow );


	lua_tinker::class_add<CKTDGUIControl>( L, "CKTDGUIControl" );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetName",						&CKTDGUIControl::SetName_LUA );
	//lua_tinker::class_def<CKTDGUIControl>( L, "GetName",						&CKTDGUIControl::GetName_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetTabControlName",				&CKTDGUIControl::SetTabControlName_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetPrevTabControlName",			&CKTDGUIControl::SetPrevTabControlName_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetOffsetPos",					&CKTDGUIControl::SetOffsetPos_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "GetOffsetPos",					&CKTDGUIControl::GetOffsetPos );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetColor",						&CKTDGUIControl::SetColor );
	lua_tinker::class_def<CKTDGUIControl>( L, "Move",							&CKTDGUIControl::Move_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetEnable",						&CKTDGUIControl::SetEnable );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetShow",						&CKTDGUIControl::SetShow );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetShowEnable",					&CKTDGUIControl::SetShowEnable );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetSound",						&CKTDGUIControl::SetSound );
	lua_tinker::class_def<CKTDGUIControl>( L, "RequestFocus",					&CKTDGUIControl::RequestFocus );
	lua_tinker::class_def<CKTDGUIControl>( L, "GetMouseOver",					&CKTDGUIControl::GetMouseOver );
	lua_tinker::class_def<CKTDGUIControl>( L, "MoveStop",						&CKTDGUIControl::MoveStop );
	lua_tinker::class_def<CKTDGUIControl>( L, "GetIsMoving",					&CKTDGUIControl::GetIsMoving );
	lua_tinker::class_def<CKTDGUIControl>( L, "AddDummyPos",					&CKTDGUIControl::AddDummyPos );
	lua_tinker::class_def<CKTDGUIControl>( L, "GetDummyPos",					&CKTDGUIControl::GetDummyPos );
	lua_tinker::class_def<CKTDGUIControl>( L, "AddDummyString",					&CKTDGUIControl::AddDummyString_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "GetDummyString",					&CKTDGUIControl::GetDummyString );
	lua_tinker::class_def<CKTDGUIControl>( L, "AddDummyInt",					&CKTDGUIControl::AddDummyInt );
	lua_tinker::class_def<CKTDGUIControl>( L, "GetDummyInt",					&CKTDGUIControl::GetDummyInt );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetOffsetMaximize",				&CKTDGUIControl::SetOffsetMaximize );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetEnableLeftTop",				&CKTDGUIControl::SetEnableLeftTop );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetEnableRightBottom",			&CKTDGUIControl::SetEnableRightBottom );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetGuideDesc",					&CKTDGUIControl::SetGuideDesc_LUA );
    lua_tinker::class_def<CKTDGUIControl>( L, "SetGuideDescByString",			&CKTDGUIControl::SetGuideDescByString_LUA );
    lua_tinker::class_def<CKTDGUIControl>( L, "SetGuideDescAbsolutePos",		&CKTDGUIControl::SetGuideDescAbsolutePos );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetGuideDescOffsetPos",			&CKTDGUIControl::SetGuideDescOffsetPos );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetNameStrID",					&CKTDGUIControl::SetNameStrID_LUA );
	lua_tinker::class_def<CKTDGUIControl>( L, "SetShowDescriptionWhenDisable",		&CKTDGUIControl::SetShowDescriptionWhenDisable );

	lua_tinker::class_add<CKTDGUIControl::CPictureData>( L, "CPictureData" );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetTex2",			&CKTDGUIControl::CPictureData::SetTex_LUA2 );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetTex",			&CKTDGUIControl::CPictureData::SetTex_LUA );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetPoint",			&CKTDGUIControl::CPictureData::SetPoint_LUA );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetShow",			&CKTDGUIControl::CPictureData::SetShow );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetPos",			&CKTDGUIControl::CPictureData::SetPos );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetMaximizeType",	&CKTDGUIControl::CPictureData::SetMaximizeType );
	lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetEdge",			&CKTDGUIControl::CPictureData::SetEdge );
    lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "SetIndex",			&CKTDGUIControl::CPictureData::SetIndex );
    lua_tinker::class_def<CKTDGUIControl::CPictureData>( L, "GetIndex",			&CKTDGUIControl::CPictureData::GetIndex );

// #ifdef MOVIE_TEST	 	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	lua_tinker::class_add<CKTDGUIControl::CMovieData>( L, "CMovieData" );
	lua_tinker::class_def<CKTDGUIControl::CMovieData>( L, "SetMovieFileName",		&CKTDGUIControl::CMovieData::SetMovieFileName_LUA );
	lua_tinker::class_def<CKTDGUIControl::CMovieData>( L, "SetRect",				&CKTDGUIControl::CMovieData::SetRect_LUA );
	lua_tinker::class_def<CKTDGUIControl::CMovieData>( L, "SetShow",				&CKTDGUIControl::CMovieData::SetShow );
	//lua_tinker::class_def<CKTDGUIControl::CMovieData>( L, "SetMoivePoint",			&CKTDGUIControl::CMovieData::SetMoivePoint_LUA );
	lua_tinker::class_def<CKTDGUIControl::CMovieData>( L, "SetLoop",				&CKTDGUIControl::CMovieData::SetLoop );

// #endif //  MOVIE_TEST	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	lua_tinker::class_add<CKTDGUIButton>( L, "CKTDGUIButton" );
	lua_tinker::class_inh< CKTDGUIButton, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetNormalTex",				&CKTDGUIButton::SetNormalTex_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetOverTex",					&CKTDGUIButton::SetOverTex_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDownTex",					&CKTDGUIButton::SetDownTex_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDisableTex",				&CKTDGUIButton::SetDisableTex_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetNormalAnimTex",			&CKTDGUIButton::SetNormalAnimTex_LUA);
	lua_tinker::class_def<CKTDGUIButton>( L, "SetOverAnimTex",				&CKTDGUIButton::SetOverAnimTex_LUA);
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDownAnimTex",				&CKTDGUIButton::SetDownAnimTex_LUA);
	lua_tinker::class_def<CKTDGUIButton>( L, "SetNormalStatic",				&CKTDGUIButton::SetNormalStatic_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetOverStatic",				&CKTDGUIButton::SetOverStatic_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDownStatic",				&CKTDGUIButton::SetDownStatic_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDisableStatic",			&CKTDGUIButton::SetDisableStatic_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetNormalPoint",				&CKTDGUIButton::SetNormalPoint_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetOverPoint",				&CKTDGUIButton::SetOverPoint_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDownPoint",				&CKTDGUIButton::SetDownPoint_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetDisablePoint",				&CKTDGUIButton::SetDisablePoint_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetString",					&CKTDGUIButton::SetString_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomMsgMouseOut",		&CKTDGUIButton::SetCustomMsgMouseOut );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomMsgMouseOver",		&CKTDGUIButton::SetCustomMsgMouseOver );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomMsgMouseDown",		&CKTDGUIButton::SetCustomMsgMouseDown );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomMsgMouseUp",			&CKTDGUIButton::SetCustomMsgMouseUp );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomMsgMouseRightUp",	&CKTDGUIButton::SetCustomMsgMouseRightUp );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomFuncMouseOver",		&CKTDGUIButton::SetCustomFuncMouseOver );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomFuncMouseDown",		&CKTDGUIButton::SetCustomFuncMouseDown );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomFuncMouseUp",		&CKTDGUIButton::SetCustomFuncMouseUp );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetCustomMsgMouseDblClk",		&CKTDGUIButton::SetCustomMsgMouseDblClk );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetEnableClick",				&CKTDGUIButton::SetEnableClick );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetEdge",						&CKTDGUIButton::SetEdge );
	lua_tinker::class_def<CKTDGUIButton>( L, "SetEdge2",					&CKTDGUIButton::SetEdge2 );

	lua_tinker::class_def<CKTDGUIButton>( L, "ChangeMouseUpSound_LUA",		&CKTDGUIButton::ChangeMouseUpSound_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "ChangeMouseDownSound_LUA",	&CKTDGUIButton::ChangeMouseDownSound_LUA );
	lua_tinker::class_def<CKTDGUIButton>( L, "ChangeMouseOverSound_LUA",	&CKTDGUIButton::ChangeMouseOverSound_LUA );



	lua_tinker::class_add<CKTDGUIStatic>( L, "CKTDGUIStatic" );
	lua_tinker::class_inh< CKTDGUIStatic, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIStatic>( L, "AddPicture",					&CKTDGUIStatic::AddPicture );
	lua_tinker::class_def<CKTDGUIStatic>( L, "AddString",					&CKTDGUIStatic::AddString_LUA );

// #ifdef MOVIE_TEST	 	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	lua_tinker::class_def<CKTDGUIStatic>( L, "AddMovieData",				&CKTDGUIStatic::AddMovieData_LUA );

// #endif //  MOVIE_TEST	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	lua_tinker::class_def<CKTDGUIStatic>( L, "GetPicture",					&CKTDGUIStatic::GetPicture );
	lua_tinker::class_def<CKTDGUIStatic>( L, "GetString",					&CKTDGUIStatic::GetString );
	lua_tinker::class_def<CKTDGUIStatic>( L, "SetCustomMsgMouseDblClk",		&CKTDGUIStatic::SetCustomMsgMouseDblClk ); // static ???????
	lua_tinker::class_def<CKTDGUIStatic>( L, "SetCustomMsgMouseUp",			&CKTDGUIStatic::SetCustomMsgMouseUp );



	lua_tinker::class_add<CKTDGUICheckBox>( L, "CKTDGUICheckBox" );
	lua_tinker::class_inh< CKTDGUICheckBox, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetNormalTex",				&CKTDGUICheckBox::SetNormalTex_LUA );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetOverTex",				&CKTDGUICheckBox::SetOverTex_LUA );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetDisableTex",				&CKTDGUICheckBox::SetDisableTex_LUA );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetCheckedTex",				&CKTDGUICheckBox::SetCheckedTex_LUA );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetBGPoint",				&CKTDGUICheckBox::SetBGPoint_LUA );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetCheckPoint",				&CKTDGUICheckBox::SetCheckPoint_LUA );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetCustomMsgChecked",		&CKTDGUICheckBox::SetCustomMsgChecked );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetCustomMsgUnChecked",		&CKTDGUICheckBox::SetCustomMsgUnChecked );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetChecked",				&CKTDGUICheckBox::SetChecked );
	lua_tinker::class_def<CKTDGUICheckBox>( L, "SetEmptyBGWhenChecked",		&CKTDGUICheckBox::SetEmptyBGWhenChecked );


	lua_tinker::class_add<CKTDGUIEditBox>( L, "CKTDGUIEditBox" );
	lua_tinker::class_inh< CKTDGUIEditBox, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetNormalTex",				&CKTDGUIEditBox::SetNormalTex_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetDisableTex",				&CKTDGUIEditBox::SetDisableTex_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetNormalPoint",				&CKTDGUIEditBox::SetNormalPoint_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetDisablePoint",			&CKTDGUIEditBox::SetDisablePoint_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetTextPoint",				&CKTDGUIEditBox::SetTextPoint_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetFont",					&CKTDGUIEditBox::SetFont_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetCustomMsgChange",			&CKTDGUIEditBox::SetCustomMsgChange );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetCustomMsgEnter",			&CKTDGUIEditBox::SetCustomMsgEnter );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetTextColor",				&CKTDGUIEditBox::SetTextColor_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetSelectedTextColor",		&CKTDGUIEditBox::SetSelectedTextColor_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetSelectedBackColor",		&CKTDGUIEditBox::SetSelectedBackColor_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetCaretColor",				&CKTDGUIEditBox::SetCaretColor_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetOutlineColor",			&CKTDGUIEditBox::SetOutlineColor_LUA );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetHideChar",				&CKTDGUIEditBox::SetHideChar_ );
	lua_tinker::class_def<CKTDGUIEditBox>( L, "SetByteLimit",				&CKTDGUIEditBox::SetByteLimit_LUA );


	lua_tinker::class_add<CKTDGUIIMEEditBox>( L, "CKTDGUIIMEEditBox" );
	lua_tinker::class_inh< CKTDGUIIMEEditBox, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetNormalTex",				&CKTDGUIIMEEditBox::SetNormalTex_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetDisableTex",				&CKTDGUIIMEEditBox::SetDisableTex_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetNormalPoint",				&CKTDGUIIMEEditBox::SetNormalPoint_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetDisablePoint",				&CKTDGUIIMEEditBox::SetDisablePoint_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetTextPoint",				&CKTDGUIIMEEditBox::SetTextPoint_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetFont",						&CKTDGUIIMEEditBox::SetFont_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetText",						&CKTDGUIIMEEditBox::SetText_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetCustomMsgChange",			&CKTDGUIIMEEditBox::SetCustomMsgChange );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetCustomMsgEnter",			&CKTDGUIIMEEditBox::SetCustomMsgEnter );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetCustomMsgFocusOut",		&CKTDGUIIMEEditBox::SetCustomMsgFocusOut );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetCustomMsgFocusIn",			&CKTDGUIIMEEditBox::SetCustomMsgFocusIn );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetForceSendCustomMsgChange",	&CKTDGUIIMEEditBox::SetForceSendCustomMsgChange );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetTextColor",				&CKTDGUIIMEEditBox::SetTextColor_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetSelectedTextColor",		&CKTDGUIIMEEditBox::SetSelectedTextColor_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetSelectedBackColor",		&CKTDGUIIMEEditBox::SetSelectedBackColor_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetCaretColor",				&CKTDGUIIMEEditBox::SetCaretColor_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetOutlineColor",				&CKTDGUIIMEEditBox::SetOutlineColor_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetLengthLimit",				&CKTDGUIIMEEditBox::SetLengthLimit_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetOneCharMode",				&CKTDGUIIMEEditBox::SetOneCharMode_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetOnlyNumMode",				&CKTDGUIIMEEditBox::SetOnlyNumMode_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetDefTextAlign",				&CKTDGUIIMEEditBox::SetDefTextAlign_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetMultiLineOption",			&CKTDGUIIMEEditBox::SetMultiLineOption_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetWidthLimit",				&CKTDGUIIMEEditBox::SetWidthLimit_LUA );
    lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetLineLimit",				&CKTDGUIIMEEditBox::SetLineLimit_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetOnlyNumAndEnglishMode",	&CKTDGUIIMEEditBox::SetOnlyNumAndEnglishMode_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetByteLimit",				&CKTDGUIIMEEditBox::SetByteLimit_LUA );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetAutoChangeBigEnglish", 	&CKTDGUIIMEEditBox::SetAutoChangeBigEnglish );
	lua_tinker::class_def<CKTDGUIIMEEditBox>( L, "SetEnablePaste", 				&CKTDGUIIMEEditBox::SetEnablePaste );

	lua_tinker::class_add<CKTDGUIRadioButton>( L, "CKTDGUIRadioButton" );
	lua_tinker::class_inh< CKTDGUIRadioButton, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetNormalTex",				&CKTDGUIRadioButton::SetNormalTex_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetOverTex",					&CKTDGUIRadioButton::SetOverTex_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetDisableTex",				&CKTDGUIRadioButton::SetDisableTex_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCheckedTex",				&CKTDGUIRadioButton::SetCheckedTex_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetDisabledPoint",			&CKTDGUIRadioButton::SetDisabledPoint_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetBGPoint",					&CKTDGUIRadioButton::SetBGPoint_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetBGMouseOverPoint",		&CKTDGUIRadioButton::SetBGMouseOverPoint_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetRBPoint",					&CKTDGUIRadioButton::SetRBPoint_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCheckPoint",				&CKTDGUIRadioButton::SetCheckPoint_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCustomMsgChecked",		&CKTDGUIRadioButton::SetCustomMsgChecked );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCustomMsgUnChecked",		&CKTDGUIRadioButton::SetCustomMsgUnChecked );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCustomMsgDoubleClicked",	&CKTDGUIRadioButton::SetCustomMsgDoubleClicked );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCustomMsgRButtonUp",		&CKTDGUIRadioButton::SetCustomMsgRButtonUp );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetGroupID",					&CKTDGUIRadioButton::SetGroupID_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetString",					&CKTDGUIRadioButton::SetString_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetChecked",					&CKTDGUIRadioButton::SetChecked_LUA );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetShowOffBGByCheck",		&CKTDGUIRadioButton::SetShowOffBGByCheck );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetFixOverByCheck",			&CKTDGUIRadioButton::SetFixOverByCheck );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "GetCheckedControl",			&CKTDGUIRadioButton::GetCheckedControl );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetEnableDoubleClick",		&CKTDGUIRadioButton::SetEnableDoubleClick );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetEnableCheckToggle",		&CKTDGUIRadioButton::SetEnableCheckToggle );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetGroupMaxCheckCount",		&CKTDGUIRadioButton::SetGroupMaxCheckCount );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCustomMsgMouseOut",		&CKTDGUIRadioButton::SetCustomMsgMouseOut );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCustomMsgMouseOver",		&CKTDGUIRadioButton::SetCustomMsgMouseOver );
	lua_tinker::class_def<CKTDGUIRadioButton>( L, "SetCheckedEdge",				&CKTDGUIRadioButton::SetCheckedEdge );



	lua_tinker::class_add<CKTDGUIComboBox>( L, "CKTDGUIComboBox" );
	lua_tinker::class_inh< CKTDGUIComboBox, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetTextTex",						&CKTDGUIComboBox::SetTextTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetDropdownTex",					&CKTDGUIComboBox::SetDropdownTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetDropdownTextTex",				&CKTDGUIComboBox::SetDropdownTextTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetButtonTex",						&CKTDGUIComboBox::SetButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetButtonMouseOverTex",				&CKTDGUIComboBox::SetButtonMouseOverTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetButtonMouseDownTex",				&CKTDGUIComboBox::SetButtonMouseDownTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetScollBarDownButtonTex",			&CKTDGUIComboBox::SetScollBarDownButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetScollBarDownButtonDownTex",		&CKTDGUIComboBox::SetScollBarDownButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetScollBarThumbTex",				&CKTDGUIComboBox::SetScollBarThumbTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetScollBarTrackTex",				&CKTDGUIComboBox::SetScollBarTrackTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetScollBarUpButtonTex",			&CKTDGUIComboBox::SetScollBarUpButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetScollBarUpButtonDownTex",		&CKTDGUIComboBox::SetScollBarUpButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetPos",							&CKTDGUIComboBox::SetPos_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetSize",							&CKTDGUIComboBox::SetSize_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetTextStringOffsetPos",			&CKTDGUIComboBox::SetTextStringOffsetPos );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetTextPos",						&CKTDGUIComboBox::SetTextPos_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetTextSize",						&CKTDGUIComboBox::SetTextSize_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetButtonPos",						&CKTDGUIComboBox::SetButtonPos_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetButtonSize",						&CKTDGUIComboBox::SetButtonSize_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetDropHeight",						&CKTDGUIComboBox::SetDropHeight );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetStaticHeight",					&CKTDGUIComboBox::SetStaticHeight );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetButtonAddSize",					&CKTDGUIComboBox::SetButtonAddSize );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetString",							&CKTDGUIComboBox::SetString_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "AddItem",							&CKTDGUIComboBox::AddItem_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "AddControl",						&CKTDGUIComboBox::AddControl );
	//lua_tinker::class_def<CKTDGUIComboBox>( L, "SetItemScriptName",					&CKTDGUIComboBox::SetItemScriptName );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "InitScrollBar",						&CKTDGUIComboBox::InitScrollBar_LUA );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetCustomMsgSelectionChanged",		&CKTDGUIComboBox::SetCustomMsgSelectionChanged );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetCustomMsgOpenDropdown",			&CKTDGUIComboBox::SetCustomMsgOpenDropdown );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetCustomMsgCloseDropdown",			&CKTDGUIComboBox::SetCustomMsgCloseDropdown );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetSelectedByIndex",				&CKTDGUIComboBox::SetSelectedByIndex );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetUnSelected",						&CKTDGUIComboBox::SetUnSelected );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "SetReverseDropdown",				&CKTDGUIComboBox::SetReverseDropdown );
    lua_tinker::class_def<CKTDGUIComboBox>( L, "SetSelectTextShow",				    &CKTDGUIComboBox::SetSelectTextShow );
	lua_tinker::class_def<CKTDGUIComboBox>( L, "AddItemStrID",						&CKTDGUIComboBox::AddItemStrID_LUA );



	lua_tinker::class_add<CKTDGUIListBox>( L, "CKTDGUIListBox" );
	lua_tinker::class_inh< CKTDGUIListBox, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetButtonMouseOverTex",				&CKTDGUIListBox::SetButtonMouseOverTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetTextTex",							&CKTDGUIListBox::SetTextTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetBoundingBoxTex",					&CKTDGUIListBox::SetBoundingBoxTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScrollBarWidth",					&CKTDGUIListBox::SetScrollBarWidth );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScrollBarPos",					&CKTDGUIListBox::SetScrollBarPos );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScrollBarSize",					&CKTDGUIListBox::SetScrollBarSize );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScollBarDownButtonTex",			&CKTDGUIListBox::SetScollBarDownButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScollBarDownButtonDownTex",		&CKTDGUIListBox::SetScollBarDownButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScollBarThumbTex",				&CKTDGUIListBox::SetScollBarThumbTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScollBarTrackTex",				&CKTDGUIListBox::SetScollBarTrackTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScollBarUpButtonTex",				&CKTDGUIListBox::SetScollBarUpButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScollBarUpButtonDownTex",			&CKTDGUIListBox::SetScollBarUpButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetPos",								&CKTDGUIListBox::SetPos_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetSize",							&CKTDGUIListBox::SetSize_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetStaticHeight",					&CKTDGUIListBox::SetStaticHeight );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScrollBarTrackRange",				&CKTDGUIListBox::SetScrollBarTrackRange );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetString",							&CKTDGUIListBox::SetString_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetGroupString",						&CKTDGUIListBox::SetGroupString_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "AddItem",							&CKTDGUIListBox::AddItem_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetCustomMsgSelection",				&CKTDGUIListBox::SetCustomMsgSelection );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetCustomMsgSelection_End",			&CKTDGUIListBox::SetCustomMsgSelection_End );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetCustomMsgItemDoubleClick",		&CKTDGUIListBox::SetCustomMsgItemDoubleClick );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetCustomMsgSelectionByRButton",		&CKTDGUIListBox::SetCustomMsgSelectionByRButton );
	lua_tinker::class_def<CKTDGUIListBox>( L, "InitScrollBar",						&CKTDGUIListBox::InitScrollBar_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetMultiSelection",					&CKTDGUIListBox::SetMultiSelection_LUA );
	//lua_tinker::class_def<CKTDGUIListBox>( L, "SetItemScriptName",					&CKTDGUIListBox::SetItemScriptName_LUA );
	lua_tinker::class_def<CKTDGUIListBox>( L, "AddControl",							&CKTDGUIListBox::AddControl );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetIgnoreListClick",					&CKTDGUIListBox::SetIgnoreListClick );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetUseItemDisappear",				&CKTDGUIListBox::SetUseItemDisappear );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetUpdateScrollBarOnUpdate",			&CKTDGUIListBox::SetUpdateScrollBarOnUpdate );
	lua_tinker::class_def<CKTDGUIListBox>( L, "SetScrollToEndOnUpdateRects",		&CKTDGUIListBox::SetScrollToEndOnUpdateRects );

	lua_tinker::class_def<CKTDGUIListBox>( L, "SetSelectItemAtMouseMove",			&CKTDGUIListBox::SetSelectItemAtMouseMove );


	lua_tinker::class_add<CKTDGUIScrollBar>( L, "CKTDGUIScrollBar" );
	lua_tinker::class_inh< CKTDGUIScrollBar, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIScrollBar>( L, "SetEnableTrack",					&CKTDGUIScrollBar::SetEnableTrack );
	lua_tinker::class_def<CKTDGUIScrollBar>( L, "SetEnableThumb",					&CKTDGUIScrollBar::SetEnableThumb );



	lua_tinker::class_add<CKTDGUISlider>( L, "CKTDGUISlider" );
	lua_tinker::class_inh< CKTDGUISlider, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetButtonTex",						&CKTDGUISlider::SetButtonTex_LUA );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetBGTex",							&CKTDGUISlider::SetBGTex_LUA );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetPos",								&CKTDGUISlider::SetPos );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetSize",								&CKTDGUISlider::SetSize );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetButtonSize",						&CKTDGUISlider::SetButtonSize );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetRange",							&CKTDGUISlider::SetRange );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetValue",							&CKTDGUISlider::SetValue );
	lua_tinker::class_def<CKTDGUISlider>( L, "SetCustomMsgValueChanged",			&CKTDGUISlider::SetCustomMsgValueChanged );




	lua_tinker::class_add<CKTDGUINavigation>( L, "CKTDGUINavigation" );
	lua_tinker::class_inh< CKTDGUINavigation, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUINavigation>( L, "SetBGTex_LUA",					&CKTDGUINavigation::SetBGTex_LUA );
	lua_tinker::class_def<CKTDGUINavigation>( L, "SetWindowTex_LUA",				&CKTDGUINavigation::SetWindowTex_LUA );
	lua_tinker::class_def<CKTDGUINavigation>( L, "SetBGPoint_LUA",					&CKTDGUINavigation::SetBGPoint_LUA );
	lua_tinker::class_def<CKTDGUINavigation>( L, "SetWindowPoint_LUA",				&CKTDGUINavigation::SetWindowPoint_LUA );
	lua_tinker::class_def<CKTDGUINavigation>( L, "SetCustomMsgLButtonDown",			&CKTDGUINavigation::SetCustomMsgLButtonDown );
	lua_tinker::class_def<CKTDGUINavigation>( L, "SetCustomMsgMouseDragging",		&CKTDGUINavigation::SetCustomMsgMouseDragging );


	lua_tinker::class_add<CKTDGUIContolList>( L, "CKTDGUIContolList" );
	lua_tinker::class_inh< CKTDGUIContolList, CKTDGUIControl>( L );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScollBarDownButtonTex",			&CKTDGUIContolList::SetXScollBarDownButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScollBarDownButtonDownTex",		&CKTDGUIContolList::SetXScollBarDownButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScollBarThumbTex",					&CKTDGUIContolList::SetXScollBarThumbTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScollBarTrackTex",					&CKTDGUIContolList::SetXScollBarTrackTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScollBarUpButtonTex",				&CKTDGUIContolList::SetXScollBarUpButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScollBarUpButtonDownTex",			&CKTDGUIContolList::SetXScollBarUpButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScrollBarPos",				&CKTDGUIContolList::SetXScrollBarPos );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetXScrollBarSize",				&CKTDGUIContolList::SetXScrollBarSize );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetShowEnableXScroolBar",			&CKTDGUIContolList::SetShowEnableXScroolBar );

	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScollBarDownButtonTex",			&CKTDGUIContolList::SetYScollBarDownButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScollBarDownButtonDownTex",		&CKTDGUIContolList::SetYScollBarDownButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScollBarThumbTex",					&CKTDGUIContolList::SetYScollBarThumbTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScollBarTrackTex",					&CKTDGUIContolList::SetYScollBarTrackTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScollBarUpButtonTex",				&CKTDGUIContolList::SetYScollBarUpButtonTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScollBarUpButtonDownTex",			&CKTDGUIContolList::SetYScollBarUpButtonDownTex_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScrollBarPos",				&CKTDGUIContolList::SetYScrollBarPos );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetYScrollBarSize",				&CKTDGUIContolList::SetYScrollBarSize );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetShowEnableYScroolBar",			&CKTDGUIContolList::SetShowEnableYScroolBar );

	lua_tinker::class_def<CKTDGUIContolList>( L, "InitXScrollBar",						&CKTDGUIContolList::InitXScrollBar_LUA );
	lua_tinker::class_def<CKTDGUIContolList>( L, "InitYScrollBar",						&CKTDGUIContolList::InitYScrollBar_LUA );

	lua_tinker::class_def<CKTDGUIContolList>( L, "SetPos",						&CKTDGUIContolList::SetPos_LUA );

	lua_tinker::class_def<CKTDGUIContolList>( L, "InsertItem",							&CKTDGUIContolList::InsertItem );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetScrollOffset",							&CKTDGUIContolList::SetScrollOffset );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetViewSize",							&CKTDGUIContolList::SetViewSize );
	lua_tinker::class_def<CKTDGUIContolList>( L, "SetCustomMsgScroll",							&CKTDGUIContolList::SetCustomMsgScroll );


	lua_tinker::class_add<CKTDGUISlot>( L, "CKTDGUISlot" );
	lua_tinker::class_inh< CKTDGUISlot, CKTDGUIControl>( L );

	lua_tinker::class_def<CKTDGUISlot>( L, "SetDraggingSlotColor",							&CKTDGUISlot::SetDraggingSlotColor );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetDraggedSlotColor",							&CKTDGUISlot::SetDraggedSlotColor  );

	lua_tinker::class_def<CKTDGUISlot>( L, "SetDraggingSlotColor",							&CKTDGUISlot::SetDraggingSlotColor );

	lua_tinker::class_def<CKTDGUISlot>( L, "GetDroppedSlot",							&CKTDGUISlot::GetDroppedSlot );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetCustomMsgMouseOut",							&CKTDGUISlot::SetCustomMsgMouseOut );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetCustomMsgMouseOver",							&CKTDGUISlot::SetCustomMsgMouseOver );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetCustomMsgMouseDown",							&CKTDGUISlot::SetCustomMsgMouseDown );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetCustomMsgMouseUp",							&CKTDGUISlot::SetCustomMsgMouseUp );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetCustomMsgMouseRightUp",							&CKTDGUISlot::SetCustomMsgMouseRightUp );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetCustomMsgDropped",							&CKTDGUISlot::SetCustomMsgDropped );

	lua_tinker::class_def<CKTDGUISlot>( L, "SetItemTex",								&CKTDGUISlot::SetItemTex_LUA );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetItemTex2",								&CKTDGUISlot::SetItemTex_LUA2 );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetMouseOverTex",							&CKTDGUISlot::SetMouseOverTex_LUA );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetString",									&CKTDGUISlot::SetString_LUA );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetPoint",									&CKTDGUISlot::SetPoint_LUA );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetMouseOverPoint",							&CKTDGUISlot::SetMouseOverPoint_LUA );

	lua_tinker::class_def<CKTDGUISlot>( L, "SetDragable",									&CKTDGUISlot::SetDragable );
	lua_tinker::class_def<CKTDGUISlot>( L, "GetDragable",									&CKTDGUISlot::GetDragable );
	lua_tinker::class_def<CKTDGUISlot>( L, "SetID",											&CKTDGUISlot::SetID );




	lua_tinker::class_add<CKTDGUIItem>( L, "CKTDGUIItem" );
	lua_tinker::class_inh<CKTDGUIItem, CKTDGUIStatic>( L );


	lua_tinker::class_add<CKTDGParticleSystem>( L, "CKTDGParticleSystem" );
	lua_tinker::class_def<CKTDGParticleSystem>( L, "CreateSequence_LUA",			&CKTDGParticleSystem::CreateSequence_LUA );
	lua_tinker::class_def<CKTDGParticleSystem>( L, "GameUnitCreateSequence_LUA",	&CKTDGParticleSystem::GameUnitCreateSequence_LUA );
    lua_tinker::class_def<CKTDGParticleSystem>( L, "DestroyInstance",				&CKTDGParticleSystem::DestroyInstance_LUA );
	lua_tinker::class_def<CKTDGParticleSystem>( L, "IsLiveInstance",				&CKTDGParticleSystem::IsLiveInstance );


	lua_tinker::class_add<CKTDGParticleSystem::CParticleEventSequence>( L, "CParticleEventSequence" );
	lua_tinker::class_inh<CKTDGParticleSystem::CParticleEventSequence, CKTDGObject>( L );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetLandPosition",		&CKTDGParticleSystem::CParticleEventSequence::SetLandPosition );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "GetPosition",			&CKTDGParticleSystem::CParticleEventSequence::GetPosition );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetBlackHolePosition",	&CKTDGParticleSystem::CParticleEventSequence::SetBlackHolePosition );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "UseLookPoint",			&CKTDGParticleSystem::CParticleEventSequence::UseLookPoint );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetLookPoint",			&CKTDGParticleSystem::CParticleEventSequence::SetLookPoint );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetPosition",			&CKTDGParticleSystem::CParticleEventSequence::SetPosition );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetAutoDie",			&CKTDGParticleSystem::CParticleEventSequence::SetAutoDie );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetAddRotate",			&CKTDGParticleSystem::CParticleEventSequence::SetAddRotate );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetAxisAngle",			&CKTDGParticleSystem::CParticleEventSequence::SetAxisAngle );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "ChangeTexForce_LUA",	&CKTDGParticleSystem::CParticleEventSequence::ChangeTexForce_LUA );
	//lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "ChangeTexForce_LUA",	&CKTDGParticleSystem::CParticleEventSequence::ChangeTexForce_LUA );
    lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetScaleFactor",	    &CKTDGParticleSystem::CParticleEventSequence::SetScaleFactor );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "GetHandle",			    &CKTDGParticleSystem::CParticleEventSequence::GetHandle );

	//{{ JHKang / ??????/ 2010/12/15
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetDrawCount",		    &CKTDGParticleSystem::CParticleEventSequence::SetDrawCount);
	//}} JHKang / ?????? / 2010/12/15
//#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "SetPerFrameSimulation",		    &CKTDGParticleSystem::CParticleEventSequence::SetPerFrameSimulation);
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequence>( L, "GetPerFrameSimulation",		    &CKTDGParticleSystem::CParticleEventSequence::GetPerFrameSimulation);
//endf  X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION

	lua_tinker::class_add<CKTDGParticleSystem::CParticleEventSequenceHandle>( L, "CParticleEventSequenceHandle" );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleEventSequenceHandle>( L, "IsEqual", &CKTDGParticleSystem::CParticleEventSequenceHandle::IsEqual );
	lua_tinker::class_add<CKTDGParticleSystem::CParticleHandle>( L, "CParticleHandle" );
	lua_tinker::class_def<CKTDGParticleSystem::CParticleHandle>( L, "IsEqual", &CKTDGParticleSystem::CParticleHandle::IsEqual );


	lua_tinker::class_add<CKTDGXMeshPlayer>( L, "CKTDGXMeshPlayer" );
	lua_tinker::class_def<CKTDGXMeshPlayer>( L, "CreateInstance_LUA",					&CKTDGXMeshPlayer::CreateInstance_LUA );
	lua_tinker::class_def<CKTDGXMeshPlayer>( L, "DestroyInstance",						&CKTDGXMeshPlayer::DestroyInstance_LUA );
	lua_tinker::class_def<CKTDGXMeshPlayer>( L, "IsLiveInstanceHandle",					&CKTDGXMeshPlayer::IsLiveInstanceHandle );
	lua_tinker::class_def<CKTDGXMeshPlayer>( L, "IsLiveInstance",						&CKTDGXMeshPlayer::IsLiveInstance );




	lua_tinker::class_add<CKTDGXMeshPlayer::CXMeshInstance>( L, "CKTDGXMeshPlayer::CXMeshInstance" );
	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetPos",					&CKTDGXMeshPlayer::CXMeshInstance::SetPos );
	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetRotateDegree",			&CKTDGXMeshPlayer::CXMeshInstance::SetRotateDegree );
	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetMoveAxisAngleDegree",	&CKTDGXMeshPlayer::CXMeshInstance::SetMoveAxisAngleDegree );
    lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetManualSize",			&CKTDGXMeshPlayer::CXMeshInstance::SetManualSize ); // added by wonpok

	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetVelocity",			&CKTDGXMeshPlayer::CXMeshInstance::SetVelocity ); // added by lucidash
	//{{ JHKang / ??????/ 2010/12/21
	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetScale_LUA",			&CKTDGXMeshPlayer::CXMeshInstance::SetScale_LUA );
	//}} JHKang / ?????? / 2010/12/21
	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "ChangeAnim_LUA",		&CKTDGXMeshPlayer::CXMeshInstance::ChangeAnim_LUA );
//#ifdef  X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION
    lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "SetPerFrameSimulation",		&CKTDGXMeshPlayer::CXMeshInstance::SetPerFrameSimulation );
    lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstance>( L, "GetPerFrameSimulation",		&CKTDGXMeshPlayer::CXMeshInstance::GetPerFrameSimulation );
//#endif  X2OPTIMIZE_PARTICLE_AND_ETC_SIMULATION

	lua_tinker::class_add<CKTDGXMeshPlayer::CXMeshInstanceHandle>( L, "CXMeshInstanceHandle" );
	lua_tinker::class_def<CKTDGXMeshPlayer::CXMeshInstanceHandle>( L, "IsEqual", &CKTDGXMeshPlayer::CXMeshInstanceHandle::IsEqual );


	lua_tinker::class_add<CKTDGCamera>( L, "CKTDGCamera" );
	lua_tinker::class_def<CKTDGCamera>( L, "DownCrashCamera",					&CKTDGCamera::DownCrashCamera );
	lua_tinker::class_def<CKTDGCamera>( L, "UpCrashCamera",						&CKTDGCamera::UpCrashCamera );
	lua_tinker::class_def<CKTDGCamera>( L, "UpDownCrashCamera",					&CKTDGCamera::UpDownCrashCamera );
	lua_tinker::class_def<CKTDGCamera>( L, "UpDownCrashCameraNoReset",			&CKTDGCamera::UpDownCrashCameraNoReset );
	lua_tinker::class_def<CKTDGCamera>( L, "RightCrashCamera",					&CKTDGCamera::RightCrashCamera );
	lua_tinker::class_def<CKTDGCamera>( L, "LeftCrashCamera",					&CKTDGCamera::LeftCrashCamera );
	lua_tinker::class_def<CKTDGCamera>( L, "LeftRightCrashCamera",				&CKTDGCamera::LeftRightCrashCamera );
	lua_tinker::class_def<CKTDGCamera>( L, "LeftRightCrashCameraNoReset",		&CKTDGCamera::LeftRightCrashCameraNoReset );






	lua_tinker::class_add<CKTDGXSkinAnim>( L, "CKTDGXSkinAnim" );
	lua_tinker::class_def<CKTDGXSkinAnim>( L, "SetUseTex",						&CKTDGXSkinAnim::SetUseTex );
	//lua_tinker::class_def<CKTDGXSkinAnim>( L, "GetMatrix",						&CKTDGXSkinAnim::GetMatrix );	// warning!! do not use in lua script when GetMatrix() returns shared_ptr<>


	lua_tinker::class_add<CKTDXApp>( L, "CKTDXApp" );
	lua_tinker::class_def<CKTDXApp>( L, "GetElapsedTime",						&CKTDXApp::GetElapsedTime );
	lua_tinker::class_def<CKTDXApp>( L, "Random_LUA",							&CKTDXApp::Random_LUA );
	lua_tinker::class_def<CKTDXApp>( L, "GetDialogManager",						&CKTDXApp::GetDialogManager_LUA );
	lua_tinker::class_def<CKTDXApp>( L, "SendGameMessage",						&CKTDXApp::SendGameMessage );
	lua_tinker::class_def<CKTDXApp>( L, "GetProj3DPos",							&CKTDXApp::GetProj3DPos );
	lua_tinker::class_def<CKTDXApp>( L, "SlowFrame_LUA",						&CKTDXApp::SlowFrame_LUA );
	lua_tinker::class_def<CKTDXApp>( L, "PlaySound2D_LUA",						&CKTDXApp::PlaySound2D_LUA );
	lua_tinker::class_def<CKTDXApp>( L, "PlaySound_LUA",						&CKTDXApp::PlaySound_LUA );
	lua_tinker::class_def<CKTDXApp>( L, "StopSound_LUA",						&CKTDXApp::StopSound_LUA );
	lua_tinker::class_def<CKTDXApp>( L, "GetVectorByRotate",					&CKTDXApp::GetVectorByRotate );
	lua_tinker::class_def<CKTDXApp>( L, "GetVec3",								&CKTDXApp::GetVec3 );

	lua_tinker::class_add<CKTDXStage>( L, "CKTDXStage" );
	//lua_tinker::class_def<CKTDXStage>( L, "SetLuaFrameMoveFunc",				&CKTDXStage::SetLuaFrameMoveFunc_LUA );


	//{{ 2010.04.28 / ????? / ?????? ??? ???? ????
	lua_tinker::class_add<CKTDGManager>( L, "CKTDGManager" );
	lua_tinker::class_def<CKTDGManager>( L, "StartFadeIn",						&CKTDGManager::StartFadeIn );
	lua_tinker::class_def<CKTDGManager>( L, "StartFadeOut",						&CKTDGManager::StartFadeOut );
	lua_tinker::class_def<CKTDGManager>( L, "GetIsFading",						&CKTDGManager::GetIsFading );

	lua_tinker::class_add<KGCMassFileManager>( L, "KGCMassFileManager");
	lua_tinker::class_def<KGCMassFileManager>( L, "AddMappingList",				&KGCMassFileManager::AddMappingList_LUA);
	lua_tinker::class_def<KGCMassFileManager>( L, "AddMappingTime",				&KGCMassFileManager::AddMappingTime_LUA);

	lua_tinker::decl( L, "g_pDGManager", GetDGManager() );
	//}}
}


void CKTDXApp::SetEnumToString( wstring wstrType, int enumIndex, wstring wstrEnum )
{
	m_mapEnumToString.insert( make_pair( make_pair(wstrType, enumIndex), wstrEnum) );
}

const wstring& CKTDXApp::GetEnumToString( wstring wstrType, int enumIndex )
{
	return m_mapEnumToString[ make_pair(wstrType, enumIndex) ];
}

void CKTDXApp::InitEnumToString()
{
	SetEnumToString( L"SLOT_STATE", 1, L"SS_EMPTY" );
	SetEnumToString( L"SLOT_STATE", 2, L"SS_CLOSE" );
	SetEnumToString( L"SLOT_STATE", 3, L"SS_WAIT" );
	SetEnumToString( L"SLOT_STATE", 4, L"SS_LOADING" );
	SetEnumToString( L"SLOT_STATE", 5, L"SS_PLAY" );


	SetEnumToString( L"GAME_UNIT_TYPE", 0, L"GUT_USER" );
	SetEnumToString( L"GAME_UNIT_TYPE", 1, L"GUT_NPC" );

	SetEnumToString( L"UNIT_CLASS", 0, L"UC_NONE" );
	SetEnumToString( L"UNIT_CLASS", 1, L"????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 2, L"????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 3, L"???? ??????" );
	SetEnumToString( L"UNIT_CLASS", 4, L"????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 5, L"??? ??????" );
#if X2_DEV_TOOLS //JHKang
	SetEnumToString( L"UNIT_CLASS", 6, L"? ??????" );
	SetEnumToString( L"UNIT_CLASS", 7, L"??? ??????" );
	SetEnumToString( L"UNIT_CLASS", 8, L"??????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 9, L"??? ??????" );

	SetEnumToString( L"UNIT_CLASS", 10, L"??? ?????" );
	SetEnumToString( L"UNIT_CLASS", 11, L"???? ?????" );
	SetEnumToString( L"UNIT_CLASS", 12, L"??? ??????" );
	SetEnumToString( L"UNIT_CLASS", 13, L"???????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 14, L"???? ??????" );
	SetEnumToString( L"UNIT_CLASS", 15, L"??? ??????" );
	SetEnumToString( L"UNIT_CLASS", 16, L"??? ????��" );
	SetEnumToString( L"UNIT_CLASS", 17, L"???? ????��" );
	SetEnumToString( L"UNIT_CLASS", 18, L"????? ???" );
	SetEnumToString( L"UNIT_CLASS", 19, L"??????" );
	SetEnumToString( L"UNIT_CLASS", 20, L"??? ?????" );
	SetEnumToString( L"UNIT_CLASS", 21, L"???? ?????" );

	SetEnumToString( L"UNIT_CLASS", 22, L"??? ?????" );
	SetEnumToString( L"UNIT_CLASS", 23, L"??? ??????" );
	SetEnumToString( L"UNIT_CLASS", 24, L"????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 25, L"???? ????��" );
	SetEnumToString( L"UNIT_CLASS", 26, L"??????" );
	SetEnumToString( L"UNIT_CLASS", 27, L"???? ?????" );
	SetEnumToString( L"UNIT_CLASS", 28, L"???" );
	SetEnumToString( L"UNIT_CLASS", 29, L"????? ?????" );
	SetEnumToString( L"UNIT_CLASS", 30, L"????? ?????" );
	SetEnumToString( L"UNIT_CLASS", 31, L"???" );
	SetEnumToString( L"UNIT_CLASS", 32, L"????? ??????" );

	SetEnumToString( L"UNIT_CLASS", 100, L"?��? ?????" );
	SetEnumToString( L"UNIT_CLASS", 101, L"?? ???????" );
	SetEnumToString( L"UNIT_CLASS", 102, L"???? ????��" );
	SetEnumToString( L"UNIT_CLASS", 103, L"????? ???" );
	SetEnumToString( L"UNIT_CLASS", 104, L"??????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 105, L"????? ????????" );
	SetEnumToString( L"UNIT_CLASS", 106, L"??????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 107, L"??????? ????" );
	SetEnumToString( L"UNIT_CLASS", 108, L"?????" );
	SetEnumToString( L"UNIT_CLASS", 109, L"????????" );
	SetEnumToString( L"UNIT_CLASS", 110, L"????? ????" );
	SetEnumToString( L"UNIT_CLASS", 111, L"???? ????" );

	SetEnumToString( L"UNIT_CLASS", 112, L"?????? ???" );
	SetEnumToString( L"UNIT_CLASS", 113, L"???? ???" );
	SetEnumToString( L"UNIT_CLASS", 114, L"????? ???" );
	SetEnumToString( L"UNIT_CLASS", 115, L"????? ��???" );
	SetEnumToString( L"UNIT_CLASS", 116, L"??? ??????" );
	SetEnumToString( L"UNIT_CLASS", 117, L"????? ?????" );
	SetEnumToString( L"UNIT_CLASS", 118, L"???" );
	SetEnumToString( L"UNIT_CLASS", 119, L"????" );
	SetEnumToString( L"UNIT_CLASS", 120, L"????? ??????" );
	SetEnumToString( L"UNIT_CLASS", 121, L"??????�� ???" );
	SetEnumToString( L"UNIT_CLASS", 122, L"??? ????��" );
#endif //UPGRADE_SKILL_SYSTEM_2013

	SetEnumToString( L"WORLD_ID", 0, L"WI_NONE" );
	SetEnumToString( L"WORLD_ID", 1, L"WI_FOREST_OF_EL" );
	SetEnumToString( L"WORLD_ID", 2, L"WI_TREE_OF_LIGHT_TOP" );
	SetEnumToString( L"WORLD_ID", 3, L"WI_EL_FOREST_MAIN_STAGE2" );
	SetEnumToString( L"WORLD_ID", 4, L"WI_EL_FOREST_MAIN_STAGE3" );
	SetEnumToString( L"WORLD_ID", 5, L"WI_EL_FOREST_MAIN_STAGE4" );
	SetEnumToString( L"WORLD_ID", 6, L"WI_END" );

	SetEnumToString( L"DUNGEON_ID", 0,	L"DI_NONE" );
	SetEnumToString( L"DUNGEON_ID", 30000, L"DI_EL_FOREST_MAIN_NORMAL" );
	SetEnumToString( L"DUNGEON_ID", 30001, L"DI_EL_FOREST_MAIN_HARD" );
	SetEnumToString( L"DUNGEON_ID", 30002, L"DI_EL_FOREST_MAIN_EXPERT" );
	SetEnumToString( L"DUNGEON_ID", 30003, L"DI_END" );

	SetEnumToString( L"DIFFICULTY_LEVEL", 0, L"DL_NORMAL" );
	SetEnumToString( L"DIFFICULTY_LEVEL", 1, L"DL_HARD" );
	SetEnumToString( L"DIFFICULTY_LEVEL", 2, L"DL_EXPERT" );


	SetEnumToString( L"ROOM_STATE", 1, L"RS_INIT" );
	SetEnumToString( L"ROOM_STATE", 2, L"RS_CLOSED" );
	SetEnumToString( L"ROOM_STATE", 3, L"RS_WAIT" );
	SetEnumToString( L"ROOM_STATE", 4, L"RS_LOADING" );
	SetEnumToString( L"ROOM_STATE", 5, L"RS_PLAY" );

	SetEnumToString( L"ROOM_TYPE", 0, L"RT_NONE" );
	SetEnumToString( L"ROOM_TYPE", 1, L"RT_PVP" );
	SetEnumToString( L"ROOM_TYPE", 2, L"RT_DUNGEON" );

	SetEnumToString( L"TEAM_NUM", 0, L"TN_RED" );
	SetEnumToString( L"TEAM_NUM", 1, L"TN_BLUE" );
	SetEnumToString( L"TEAM_NUM", 2, L"TN_MONSTER" );




	SetEnumToString( L"NPC_UNIT_ID",  0, L"NUI_NONE" );
	SetEnumToString( L"NPC_UNIT_ID",  1, L"NUI_BEE" );
	SetEnumToString( L"NPC_UNIT_ID",  2, L"NUI_BEEBOMB" );
	SetEnumToString( L"NPC_UNIT_ID",  3, L"NUI_BEEBOMBHOUSE" );
	SetEnumToString( L"NPC_UNIT_ID",  4, L"NUI_BEEHOUSE" );
	SetEnumToString( L"NPC_UNIT_ID",  5, L"NUI_BENDERS_NORMAL" );
	SetEnumToString( L"NPC_UNIT_ID",  6, L"NUI_BENDERS_HARD" );
	SetEnumToString( L"NPC_UNIT_ID",  7, L"NUI_BENDERS_EXPERT" );
	SetEnumToString( L"NPC_UNIT_ID",  8, L"NUI_CHAINGATE_GREEN" );
	SetEnumToString( L"NPC_UNIT_ID",  9, L"NUI_CHAINGATE_RED" );
	SetEnumToString( L"NPC_UNIT_ID", 10, L"NUI_CHAINGATE_YELLOW" );
	SetEnumToString( L"NPC_UNIT_ID", 11, L"NUI_CHAINGATE_POST" );
	SetEnumToString( L"NPC_UNIT_ID", 12, L"NUI_MONKEY_A" );
	SetEnumToString( L"NPC_UNIT_ID", 13, L"NUI_MONKEY_B" );
	SetEnumToString( L"NPC_UNIT_ID", 14, L"NUI_MONKEY_APPLE" );
	SetEnumToString( L"NPC_UNIT_ID", 15, L"NUI_SMALLGATE_GREEN" );
	SetEnumToString( L"NPC_UNIT_ID", 16, L"NUI_SMALLGATE_RED" );
	SetEnumToString( L"NPC_UNIT_ID", 17, L"NUI_SMALLGATE_YELLOW" );
	SetEnumToString( L"NPC_UNIT_ID", 18, L"NUI_SMALLGATE_POST" );
	SetEnumToString( L"NPC_UNIT_ID", 19, L"NUI_CHEST" );
	SetEnumToString( L"NPC_UNIT_ID", 20, L"NUI_BOX" );


}

D3DXVECTOR2 CKTDXApp::GetProj3DPos( const D3DXVECTOR3& pos )
{
	D3DXVECTOR3 vOut;
	D3DXMATRIX matProjection;
	D3DXMATRIX matView;
	D3DXMATRIX matWorld;

	GetProjectionTransform( &matProjection );
	GetViewTransform( &matView );
	D3DXMatrixIdentity( &matWorld );
	SetWorldTransform( &matWorld );
	D3DXVec3Project( &vOut, &pos, &GetViewport(), &matProjection, &matView, &matWorld );

	// Cull
	if ( 0.f > vOut.z || vOut.z > 1.f )
	{
		vOut.x = 99999.f;
		vOut.y = 99999.f;
	}

	D3DXVECTOR2 retVal;
	retVal.x	= vOut.x;
	retVal.y	= vOut.y;



	return retVal;
}


void CKTDXApp::NoticeQuitType( KTDX_QUIT_TYPE quitType )
{
#if X2_DEV_TOOLS

	if ( quitType == KQT_CLOSE_WINDOW )
	{
		MessageBox( GetHWND(), L"?????? ???", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_FIND_HACKING )
	{
		MessageBox( GetHWND(), L"??? ???", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_ESC_GAME )
	{
	}
	else if ( quitType == KQT_ALREADY_GAME_PLAY )
	{
		MessageBox( GetHWND(), L"??? ?????? ???? ????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_INVALID_ID_OR_PASSWORD )
	{
		MessageBox( GetHWND(), L"??? ???? ?????? ?��???????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_CONNECT_FAILED )
	{
		MessageBox( GetHWND(), L"???? ????? ????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_NEXON_AUTH_FAILED )
	{
		MessageBox( GetHWND(), L"??? ???? ????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_CONNECTION_LOST )
	{
		MessageBox( GetHWND(), L"???????? ?????? ????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_VERIFY_ACCOUNT )
	{
		MessageBox( GetHWND(), L"???? ???? ????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_IMEEditBOX_ERROR )
	{
		MessageBox( GetHWND(), L"IMEEditBox ????", L"???? ???", MB_OK );
	}
	else if ( quitType == KQT_DOUBLE_CONNECT)
	{
		MessageBox( GetHWND(), L"???????? ????", L"???? ???", MB_OK );
	}
	else
	{
		MessageBox( GetHWND(), L"?????? ????", L"???? ???", MB_OK );
	}
#endif
}


void CKTDXApp::PlaySound2D_LUA( char* pFileName )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );
	GetDeviceManager()->PlaySound( fileName.c_str(), false, false );
}


void CKTDXApp::PlaySound_LUA( char* pFileName )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );
	GetDeviceManager()->PlaySound( fileName.c_str() );
}

void CKTDXApp::StopSound_LUA( char* pFileName )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );
	GetDeviceManager()->StopSound( fileName.c_str() );
}


bool CKTDXApp::FindProcess( wstring wstrExeFile )
{
	//NOTE : ???????? ???��??? ????? ?��??? ???? ??????? ????? ???��????? ????? ??????.

	MakeUpperCase( wstrExeFile );

	HANDLE hSnapShot;
	PROCESSENTRY32W pEntry;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL,NULL);

	pEntry.dwSize =sizeof(pEntry);

	// ???????? ???��??????? ????? ?????? ?????��?.
	Process32First( hSnapShot, &pEntry );

	while(true)
	{
		//?????? ???��????? ?????? ?????��?.
		BOOL hRes = Process32Next( hSnapShot, &pEntry );

		if( hRes == FALSE )
			break;

		wstring wstrTemp = pEntry.szExeFile;
		MakeUpperCase( wstrTemp );

		if( wstrTemp.find( wstrExeFile ) != -1 )
			return true;
	}

	return false;
}







void CKTDXApp::GetVideoRam( DWORD& dwTotal, DWORD& dwFree )
{
#ifdef CONVERSION_VS
	if( NULL != GetDevice() )
	{
		GetDevice()->EvictManagedResources();
		UINT texMem = GetDevice()->GetAvailableTextureMem();

		dwTotal = (DWORD) texMem;
		dwFree = (DWORD) texMem;
	}
	return;
#else
	if( NULL == m_lpDD )
	{
		// directdraw7?? ???? ?????? dx9device?? ?????? ???? ????????
		// note!! ?? ????? ??????? ???? ????��???? ???? ??????? ????
		// ????? ???????? ?? ????��?? ???? ?????? ??? ???.
		if( NULL != GetDevice() )
		{
			UINT texMem = GetDevice()->GetAvailableTextureMem();

			dwTotal = (DWORD) texMem;
			dwFree = (DWORD) texMem;
		}
		return;
	}

	DDSCAPS2      ddsCaps2;

	// Initialize the structure.
	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
	HRESULT hr = m_lpDD->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree);
	if (FAILED(hr))
		return;
#endif // CONVERSION_VS
}


void CKTDXApp::SetFindHacking( bool bCheck, const wstring& findHackingDetailMsg /*= L"" */ )
{


	THEMIDA_VM_START

	m_bFindHacking = bCheck;


	THEMIDA_CHECK_PROTECTION( hackingCheckVariable, 0x226560ba )

	m_FindHackingDetailMsg = findHackingDetailMsg;



	THEMIDA_VM_END
}
