#include "stdafx.h"
#include "./X2Main.h"

#include <ShlObj.h>

#include "MachineIdLib.h"


#include <boost/bind.hpp>
using boost::bind;



#include "X2DLLManager.h"

#if defined(DEBUG) || defined(_DEBUG)
#ifdef CLIENT_COUNTRY_KR
#endif // CLIENT_COUNTRY_KR
#else // DEBUG
#ifdef CLIENT_COUNTRY_KR
#endif // CLIENT_COUNTRY_KR
#endif // DEBUG


#ifdef BUG_TRAP
    #include "BugTrap.h"

    // Link with one of BugTrap libraries.
    #ifdef _UNICODE
    #else
    #endif
#endif

//{{ kimhc // 2010.3.3 // ???? ???? ????
//}} kimhc // 2010.3.3 // ???? ???? ????








	// ????? ??? ??? ????? ???? ???? ????????.
	THEMIDA_VM_START
	THEMIDA_CODEREPLACE_START
	THEMIDA_ENCODE_START





CX2Main*			g_pMain			= NULL;
CX2Data*			g_pData			= NULL;
CX2InstanceData*	g_pInstanceData = NULL;
CX2Game*			g_pX2Game		= NULL;
CX2Room*			g_pX2Room		= NULL;
CX2SquareGame*		g_pSquareGame	= NULL;

CX2TFieldGame*		g_pTFieldGame   = NULL;
CX2WeddingGame*		g_pWeddingGame	= NULL;

CX2ChatBox*			g_pChatBox		= NULL;

FILE*				g_fpStream		= NULL;
FILE*				g_fpStreamError = NULL;

HINTERNET			g_hInternet		= NULL;


extern		CX2SMTPMail*			g_pX2SMTPMail;



BOOL CALLBACK EnumCheckThreadWndProc(HWND hwnd, LPARAM lParam)
{
	ELSWORD_VIRTUALIZER_START

	if( hwnd )
	{
		vector<THREAD_WND_INFO>* vecThreadWndInfo = (vector<THREAD_WND_INFO>*)lParam;

		WCHAR wTextWindow[_MAX_PATH] = L"";
		WCHAR wTextClass[_MAX_PATH] = L"";
		int hr1 = GetWindowText(hwnd, wTextWindow, _MAX_PATH);
		int hr2 = GetClassName(hwnd,wTextClass,_MAX_PATH);

		THREAD_WND_INFO threadWndInfo;

		if( hr1 > 0 )
			threadWndInfo.m_wstrName = wTextWindow;
		else
			threadWndInfo.m_wstrName = L"NameEmpty";

		if( hr2 > 0 )
			threadWndInfo.m_wstrClass = wTextClass;
		else
			threadWndInfo.m_wstrClass = L"ClassEmpty";

		if( hr1 > 0 )
		{
			if( threadWndInfo.m_wstrName.compare( L"MSCTFIME UI") != 0 &&
				threadWndInfo.m_wstrName.compare( L"Default IME") != 0 )
				//StrStrW( threadWndInfo.m_wstrClass.c_str(), L"HwndlessEditorFakeHwndWrapper") &&
				//threadWndInfo.m_wstrClass.compare L"tooltips_class32") != 0 &&
				//threadWndInfo.m_wstrClass.compare L"tooltips_class32") != 0 &&
				//threadWndInfo.m_wstrClass.compare L"WorkerW") != 0 )
			{
				vecThreadWndInfo->push_back(threadWndInfo);
			}
		}
	}

	ELSWORD_VIRTUALIZER_END

	return TRUE;
}

vector<THREAD_WND_INFO> GetThreadWnd( DWORD threadId )
{
	ELSWORD_VIRTUALIZER_START

	vector<THREAD_WND_INFO>	vecThreadWndInfo;
	EnumThreadWindows(threadId, EnumCheckThreadWndProc, (LPARAM)&vecThreadWndInfo);

#if X2_DEV_TOOLS
	FILE *fp = fopen("threadInfo.info", "w");
	for(UINT i=0; i<vecThreadWndInfo.size(); ++i)
	{
		wstring threadinfoWindow = vecThreadWndInfo[i].m_wstrName;
		wstring threadinfoClass = vecThreadWndInfo[i].m_wstrClass;
		fwprintf(fp, L"%s [%s]\n", threadinfoWindow.c_str(), threadinfoClass.c_str());
	}
	fclose(fp);
#endif

	ELSWORD_VIRTUALIZER_END

	return vecThreadWndInfo;
}

BOOL CALLBACK EnumWindowsProc_Thread(HWND hwnd , LPARAM lp)
{
    CX2Main::CheckWindowInfo* pkCheckWindowInfo = (CX2Main::CheckWindowInfo*) lp;
    if ( pkCheckWindowInfo == NULL )
        return FALSE;

#if !X2_DEV_TOOLS
	// checked
	ELSWORD_VIRTUALIZER_START
#endif



	WCHAR textWindow[_MAX_PATH] = L"";
	WCHAR textClass[_MAX_PATH] = L"";

	if(GetParent(hwnd))
		return TRUE;

	//THEMIDA_VM_START


	int hr1 = GetWindowText(hwnd,textWindow,_MAX_PATH);
	int hr2 = GetClassName(hwnd,textClass,_MAX_PATH);

	MakeUpperCase(textWindow);
	MakeUpperCase(textClass);

	//if( g_pMain != NULL && g_pMain->GetCheckWindowInfo() != NULL )
	{
		KHackingToolInfo windowInfo;
		if( hr1 > 0 )
			windowInfo.m_wstrProcessName = textWindow;
		else
			windowInfo.m_wstrProcessName = L"EMPTY";
		if( hr2 > 0 )
			windowInfo.m_wstrWindowClassName = textClass;
		else
			windowInfo.m_wstrWindowClassName = L"EMPTY";

		pkCheckWindowInfo->PushWindowInfo_Thread(windowInfo);
	}

	//THEMIDA_VM_END


#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif

	return TRUE;


}

BOOL Pesudo_IsDebuggerPresent()
{

	// TODO : ???? VM_START ??? ENCODE_START ??????? ?????? ???? ?????? ????
	// Error: There is at least a nested VM/CodeReplace macros. VM/CodeReplace macros cannot be nested.
	//THEMIDA_VM_START
	//THEMIDA_ENCODE_START

#if !X2_DEV_TOOLS
	// checked
	ELSWORD_VIRTUALIZER_START
#endif

	BOOL Retval = 0;
	__asm
	{
		push eax
		mov eax,dword ptr fs:[0x18]
		mov eax,dword ptr ds:[eax+0x30]
		movzx eax,byte ptr ds:[eax+0x2]
		mov Retval,eax
		pop eax
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif

	//THEMIDA_ENCODE_END
	//THEMIDA_VM_END

#if X2_DEV_TOOLS
	return false;
#else
	return Retval;
#endif

}


// NOTE : ???? ?? ????? ?????? ???? ???.
int CheckDebugger()
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	_asm
	{
		mov eax, fs:[30h]
		mov eax, [eax+68h]
		and eax, 0x70
		test eax, eax
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
}





#ifdef BUG_TRAP
void MakeScreenShot(std::string strfilename)
{
	try
	{

		HBITMAP hBitmap;
		RECT rect;
		HDC hMemDC;
		HDC hScrDC;
		HBITMAP hOldBitmap;

		//DC?? ??��?.

		HWND pDesktop = GetDesktopWindow();

		// ??��???? ???? ???????? ????? ???????.
		GetClientRect( pDesktop, &rect );


		// ?????? ??? DC?? ??? DC?? ????? ??? ??????? ???????.
		hScrDC = GetDC( pDesktop ); CreateDC( L"DISPLAY", NULL, NULL, NULL );
		hMemDC = CreateCompatibleDC( hScrDC );
		hBitmap = CreateCompatibleBitmap( hScrDC, rect.right - rect.left, rect.bottom - rect.top );
		hOldBitmap = (HBITMAP)SelectObject( hMemDC, hBitmap );

		// ????? ??? ????????? ???????.
		BitBlt( hMemDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
			hScrDC, rect.left, rect.top, SRCCOPY );
		SelectObject( hMemDC, hOldBitmap );

		CxImage cimage( CXIMAGE_FORMAT_JPG );

		if( cimage.CreateFromHBITMAP( hBitmap ) )
		{
			cimage.SetJpegQuality( 75 );
			cimage.Save( strfilename.c_str(), CXIMAGE_FORMAT_JPG );
		}

		DeleteDC( hMemDC );
		DeleteDC( hScrDC );
		DeleteObject( hBitmap );
		DeleteObject( hOldBitmap );
	}
	catch (...)
	{
	}
}

void CALLBACK CrashPreCallBack(INT_PTR nErrHandlerParam)
{
    BT_SetTerminate();

	try
	{
		MakeScreenShot("Crash_ScreenShot.jpg");
		stringstream wstrstm;
		if( g_pMain != NULL )
		{
			g_pMain->AddMemoryInfo( wstrstm );
			g_pMain->AddSystemInfo( wstrstm );
			// DLL ????
			g_pMain->AddDLLInfo( wstrstm );
			// ???��??? ????
			g_pMain->AddProcessInfo(wstrstm, false);
		}
		g_CKTDXLog.StateLogToFile( "ErrorLog.txt" );
		g_CKTDXLog.ErrorLogToFile( "ErrorLog.txt", wstrstm.str().c_str() );

		//FileEncrypt( "ErrorLog.txt" );
		//FileEncrypt( "State.log" );

		if ( g_pMain != NULL )
		{
			g_pMain->SetQuitType( NetError::ERR_CLIENT_QUIT_02 );
			g_pMain->SendQuitMsgToServer();
		}

		ShowWindow(g_pKTDXApp->GetHWND(), SW_HIDE);

		SiCX2DLLManager()->ReportEvent();
	}
	catch( ... )
	//catch(std::exception *e)
	{
	}

}

void CALLBACK CrashPostCallBack(INT_PTR nErrHandlerParam)
{
	DeleteFileA( "ErrorLog.txt");
	DeleteFileA( "State.txt");
	DeleteFileA( "Crash_ScreenShot.jpg");
    //HWND hDlg;

}
#endif

// crashreport???? ????? ??????
inline BOOL WINAPI CrashReportCallback(LPVOID lpvState)
{
	SIZE_T currMemSize=0, peakMemSize=0;
	GetUsingMemorySize( L"X2.exe", currMemSize, peakMemSize );
	WCHAR wszText[64] = L"";
	swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L"mem: curr-%d, peak-%d", currMemSize, peakMemSize );
	StateLog( wszText );



#if !X2_DEV_TOOLS
	g_CKTDXLog.ErrorLogToFile( "ErrorLog.txt" );
	g_CKTDXLog.StateLogToFile( "State.log" );
	//FileEncrypt( "ErrorLog.txt" );
	//FileEncrypt( "State.log" );
#endif

	IDirect3DSurface9* pBackBuffer = 0;
	if( NULL != g_pKTDXApp &&
		NULL != g_pKTDXApp->GetDevice() )
	{
		g_pKTDXApp->GetDevice()->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		if( NULL != pBackBuffer )
		{
			D3DXSaveSurfaceToFile( L"Crash_ScreenShot.jpg", D3DXIFF_JPG, pBackBuffer, NULL, NULL );
			pBackBuffer->Release();
		}
	}
	AddFile(lpvState, (LPCTSTR)"Crash_ScreenShot.jpg",		(LPCTSTR)"screen shot"  );

	AddFile(lpvState, (LPCTSTR)"log.htm",		(LPCTSTR)"log file"  );
	AddFile(lpvState, (LPCTSTR)"ErrorLog.txt",	(LPCTSTR)"ErrorLog File" );
	AddFile(lpvState, (LPCTSTR)"DialogLog.txt",	(LPCTSTR)"DialogLog File" );
	AddFile(lpvState, (LPCTSTR)"P2PLog.txt",	(LPCTSTR)"P2PLog File" );
	AddFile(lpvState, (LPCTSTR)"State.log",		(LPCTSTR)"StateLog File" );

	if ( g_pMain != NULL )
	{
		g_pMain->SetQuitType( NetError::ERR_CLIENT_QUIT_02 );
		g_pMain->SendQuitMsgToServer();
	}

	return TRUE;
}



//#ifndef _SERVICE_
//	extern CX2SMTPMail* g_pX2SMTPMail; // x2.cpp?? ??????????
//	LRESULT CALLBACK SendDebugMailDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_INITDIALOG:
//		{
//			g_pX2SMTPMail->SetReceiverEMail( "carl79@kogstudios.com" );
//
//			g_pX2SMTPMail->SetSenderName( "?ͧ?????????" );
//			g_pX2SMTPMail->SetSubject( "[?ͧ ???? ?��??? ??????]" );
//			g_pX2SMTPMail->SetDesc( "^^" );
//
//			// ???? ��??
//			g_pX2SMTPMail->ClearFile();
//			std::vector< std::string > vecFiles;
//
//			char* buffer = NULL;
//			if( (buffer = _getcwd( NULL, 255 )) == NULL )
//			{
//				// error
//				return TRUE;
//			}
//
//			//
//
//
//			//stringstream strmFolder;
//			//strmFolder << buffer << "\\GameGuard";
//			//free(buffer);
//
//			//// npgl.erl?? ?? ��??? ???? ??�� ??? ?????? ??????? ???????? ???
//			//stringstream strmSrcNPGL;
//			//strmSrcNPGL << strmFolder.str() << "\\npgl.erl";
//			//stringstream strmDestNPGL;
//			//strmDestNPGL << strmFolder.str() << "\\npgl_copy.erl";
//			//CopyFileA( strmSrcNPGL.str().c_str(), strmDestNPGL.str().c_str(), FALSE );
//
//
//
//			//g_pX2SMTPMail->FindFilesByExt( vecFiles, strmFolder.str().c_str(), ".erl" );
//			////g_pX2SMTPMail->FindFilesByExt( vecFiles, "C:\\Nexon\\ElSword\\Data\\GameGuard", ".erl" );
//
//			for( UINT i=0; i<vecFiles.size(); i++ )
//			{
//				g_pX2SMTPMail->AddFile( vecFiles[i].c_str());
//			}
//
//			bool bResult = g_pX2SMTPMail->SendMail( hDlg );
//
//			return TRUE;
//		} break;
//
//	case WM_COMMAND:
//		{
//			return TRUE;
//		} break;
//	}
//	return FALSE;
//}
//
//#endif // _SERVICE_








CX2Main::CX2Main() :
m_bEnabledDeveloperCheatCode( true ),
m_pData( NULL ),
m_pStringFilter( NULL ),
m_pGameEdit( NULL ),
m_pTextManager( NULL ),
m_pNoticeManager( NULL ),
m_pCheckSumManager( NULL ),
m_pLVUpEventMgr( NULL ),
m_pTutorSystem( NULL ),
m_pCouponBox( NULL ),

m_pX2PVPEmblem( NULL ),
m_pPostBox( NULL ),
m_pSecurityPad( NULL ),
m_pInformerManager( NULL ),
m_pMemoryHolder( NULL )
, m_bIsPopUpOnChat( false )
,m_pSystemInfo( NULL )
, m_DefaultChannelServerInfo()
#ifdef REFORM_UI_KEYPAD
, m_pKeyPad( NULL )
#endif
, m_dwNationFlag( NF_NONE )
, m_iOldCaptureFileSize(-1)
, m_fFileSizeCheckTime(2.f)
, m_iGameHour( 0 )							//??????? ???? ?? ???? ?��? ???
, m_pDlgPlayTimeInformation( NULL )			//?��??? ?��? ????? Dialog
, m_pDlgShutDownInformation( NULL )			//?��??? ?��? ????? Dialog

, m_fPlayTimeInfoShowTime( 0.f )			//???? ???? ?��?
, m_fShutDownInfoShowTime( 0.f )			//???? ???? ?��?

, m_bProcessPlayTimeInformation( false )	//???? Fade ??? ???? ????
, m_bProcessShutDownInformation( false )	//???? Fade ??? ???? ????

, m_wstrShutDownString( L"" )				//???? ?????? ????
, m_bSurveyUser( false )
//{{ ???? [2013/1/4]  ????? ???? ??????? ????????? ???
, m_bIsValideCheckKomScript ( true )
, m_bPlayIntroMovie( true )
{

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	m_bUdpPacketOverlap = true;

    for( int i = 0; i < CX2Game::GT_NUMS; i++ )
    {
        m_aeUDPMode[ i ] = CKTDNUDP::FORCE_CONNECT_MODE_DEFAULT;
    }
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK




	m_strMailAddress = "";
	ConvertWCHARToChar( m_strMailAddress, GET_STRING( STR_ID_18009 ) );
	if ( NULL != g_pX2SMTPMail )
		g_pX2SMTPMail->SetSMTPAddress( m_strMailAddress.c_str() );


#ifdef BUG_TRAP
	wstring clientVersion;
	wstring szAppName;

#if defined(JOB_NAME)
	szAppName = JOB_NAME;
#endif


#if defined(BUILD_ID)
	clientVersion = BUILD_ID;
#endif

	BT_InstallSehFilter();

	BT_SetAppName((LPCTSTR)szAppName.c_str());
	BT_SetAppVersion((LPCTSTR)clientVersion.c_str());

 	BT_SetFlags( BTF_DETAILEDMODE | BTF_ATTACHREPORT ); // BTF_DETAILEDMODE || BTF_SCREENCAPTURE

	BT_SetDumpType( MiniDumpNormal ); //( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory );
	BT_SetReportFormat(BTRF_XML);

	wchar_t szFilePath[1024] = _T("");
	wchar_t strStateLogBuffer[1024], strErrorLogBuffer[1024];
	GetCurrentDirectory( _MAX_PATH, szFilePath );

	BT_SetSupportServer( L"Crash-getter.kog.co.kr", 9999 );
	//BT_SetSupportServer( L"116.120.238.16", 9999 );
	BT_SetActivityType( BTA_SENDREPORT );

	StringCchPrintf(strStateLogBuffer, 1024,  _T("%s\\State.log"), szFilePath);
	//wsprintf(strStateLogBuffer, _T("%s\\State.log"), szFilePath);
	BT_AddLogFile((LPCTSTR)strStateLogBuffer);

	StringCchPrintf(strErrorLogBuffer, 1024,  _T("%s\\ErrorLog.txt"), szFilePath);
	//wsprintf(strErrorLogBuffer, _T("%s\\ErrorLog.txt"), szFilePath);
	BT_AddLogFile((LPCTSTR)strErrorLogBuffer);

	StringCchPrintf(strErrorLogBuffer, 1024, _T("%s\\Crash_ScreenShot.jpg"), szFilePath);
	//wsprintf(strErrorLogBuffer, _T("%s\\Crash_ScreenShot.jpg"), szFilePath);
	BT_AddLogFile((LPCTSTR)strErrorLogBuffer);

	BT_SetPreErrHandler(CrashPreCallBack, -1);
	BT_SetPostErrHandler(CrashPostCallBack, -1);
#endif


	RegisterHotKey( NULL, NULL, MOD_ALT, VK_F4 );


	g_pMain					= this;



#if !X2_DEV_TOOLS
#endif









	WCHAR wszKomFileName[256] = L"";


	const int KOM_FILE_COUNT = 150;
	for( int i=1; i<=KOM_FILE_COUNT; i++ )
	{
		if( i < 10 )
		{
			StringCchPrintfW( wszKomFileName, ARRAY_SIZE(wszKomFileName), L"data00%d.kom", i );
		}
		else if( i < 100 )
		{
			StringCchPrintfW( wszKomFileName, ARRAY_SIZE(wszKomFileName), L"data0%d.kom", i );
		}
		else
		{
			StringCchPrintfW( wszKomFileName, ARRAY_SIZE(wszKomFileName), L"data%d.kom", i );
		}

		m_KOMList.push_back( wszKomFileName );
	}

	const int DATAI_KOM_FILE_COUNT = 23;
	for (int i = 1; i <= DATAI_KOM_FILE_COUNT; i++)
	{
		if (i < 10)
		{
			StringCchPrintfW(wszKomFileName, ARRAY_SIZE(wszKomFileName), L"datai00%d.kom", i);
		}
		else if (i < 100)
		{
			StringCchPrintfW(wszKomFileName, ARRAY_SIZE(wszKomFileName), L"datai0%d.kom", i);
		}
		else
		{
			StringCchPrintfW(wszKomFileName, ARRAY_SIZE(wszKomFileName), L"datai%d.kom", i);
		}

		m_KOMList.push_back(wszKomFileName);
	}




#ifdef	CONVERSION_VS
	m_DLLList.push_back( L"d3d9.dll" );
	m_DLLList.push_back( L"d3dx9_43.dll" );
 	m_DLLList.push_back( L"d3dcsx_43.dll" );
	m_DLLList.push_back( L"D3DCompiler_43.dll" );
	m_DLLList.push_back( L"xactengine3_7.dll" );
	m_DLLList.push_back( L"XAPOFX1_5.dll" );
	m_DLLList.push_back( L"XAudio2_7.dll" );
	#ifdef _DEBUG
		m_DLLList.push_back( L"d3dx9d_43.dll");
	#endif // _DEBUG
#else
	m_DLLList.push_back( L"d3dx9_30.dll" );
#endif // CONVERSION_VS
	m_DLLList.push_back( L"dbghelp.dll" );
	m_DLLList.push_back( L"FindH.dll" );
	m_DLLList.push_back( L"KncCrashRpt.dll" );
	m_DLLList.push_back( L"KncFirewall.dll" );
	//m_DLLList.push_back( L"libhoard.dll" );
	m_DLLList.push_back( L"MailMIME.dll" );
	m_DLLList.push_back( L"MailSMTP.dll" );
#ifdef	CONVERSION_VS
	m_DLLList.push_back( L"msvcp100.dll" );
	m_DLLList.push_back( L"msvcr100.dll" );
#ifdef _DEBUG
	m_DLLList.push_back( L"msvcp100d.dll" );
	m_DLLList.push_back( L"msvcr100d.dll" );
#endif // _DEBUG
#else
 	m_DLLList.push_back( L"msvcp71.dll" );
 	m_DLLList.push_back( L"msvcr71.dll" );
#endif // CONVERSION_VS
	//m_DLLList.push_back( L"nmcogame.dll" );		// ????? ????? dll ????
	m_DLLList.push_back( L"usp10.dll" );
	//m_DLLList.push_back( L"EndingFlash.DLL" );	// ????? ????? dll ????
	//m_DLLList.push_back( L"winhoard.dll" );

	//{{ 081001.hoons.???????? ????? ?????? ��?��?? ?????? ?????.
	m_DLLList.push_back( L"bdcap32.dll" );
	m_DLLList.push_back( L"bdcore32.dll" );
	m_DLLList.push_back( L"vcomp90.dll" );
	//}}

    m_DLLList.push_back( L"BugTrapU.dll" );
	m_DLLList.push_back( L"BugTrap.dll" );
	m_DLLList.push_back( L"fmodex.dll" );

	//{{ ????? : [2009/9/14] //	????? dll ???
	m_DLLList.push_back( L"asc_com.dll" );
	m_DLLList.push_back( L"asc_dh.dll" );
	m_DLLList.push_back( L"asc_fse.dll" );
	m_DLLList.push_back( L"asc_intg.dll" );
	m_DLLList.push_back( L"asc_mmgr.dll" );
	m_DLLList.push_back( L"asc_unp.dll" );
	m_DLLList.push_back( L"fse_base.dll" );
	m_DLLList.push_back( L"fse_fact.dll" );
	m_DLLList.push_back( L"fse_pe.dll" );
	m_DLLList.push_back( L"gfs_base.dll" );
	m_DLLList.push_back( L"gfs_fact.dll" );
	m_DLLList.push_back( L"gfs_file.dll" );
	m_DLLList.push_back( L"gfs_mem.dll" );
	m_DLLList.push_back( L"gfs_os.dll" );
	m_DLLList.push_back( L"gfs_proc.dll" );
	m_DLLList.push_back( L"gfs_util.dll" );
	m_DLLList.push_back( L"ehsvc.dll" );
	m_DLLList.push_back( L"psapi.dll" );
	m_DLLList.push_back( L"v3pro32s.dll" );

	m_DLLList.push_back( L"AhnUpCtl.dll" );
	m_DLLList.push_back( L"AhnUpGS.dll" );
	m_DLLList.push_back( L"HSInst.dll" );
	m_DLLList.push_back( L"V3Hunt.dll" );
	m_DLLList.push_back( L"V3InetGS.dll" );

		//}} ????? : [2009/9/14] //	????? dll ???

// #ifdef MOVIE_TEST	 	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????


	m_DLLList.push_back( L"bdvid32.dll");

// #endif //  MOVIE_TEST	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????


	// ???? ??? ??? 09.11.26 ???? ???? dll ???? ????


	//{{ ???? : XTRAP - DLL ???

	m_DLLList.push_back( L"mfc100u.dll" );
#ifdef _DEBUG
	m_DLLList.push_back( L"mfc100ud.dll" );
#endif // _DEBUG


	m_DLLList.push_back( L"steam.dll" );
	m_DLLList.push_back( L"steam_api.dll" );
	m_DLLList.push_back( L"steamclient.dll" );
	m_DLLList.push_back( L"sdkencryptedappticket.dll" );

	//{{ ????? : [2009/10/12] //	???? ???? ????? ???????
#if !X2_DEV_TOOLS
	DeleteFile( L"1.docx" );
	DeleteFile( L"unomap01.tga" );
	DeleteFile( L"unomap02.tga" );
	DeleteFile( L"unomap03.tga" );
	DeleteFile( L"nui_kenage_hair.tga" );
	DeleteFile( L"nui_kenage_cloth.tga" );
	DeleteFile( L"nui_kenage_body.tga" );
	DeleteFile( L"motion_unohound.x" );
	DeleteFile( L"motion_elemental_kenaz.x" );
#endif
	//}} ????? : [2009/10/12] //	???? ???? ????? ???????

	DeleteFile( L"bdcap64.dll" );

	DeleteFile( L"item.kim" );

	DeleteOtherFile( L"KOM", m_KOMList );
	DeleteOtherFile( L"KOM_", m_KOMList );
	DeleteOtherFile( L"DLL", m_DLLList );

	m_QuitType				= NetError::ERR_CLIENT_QUIT_00;


	m_pCrashReportState		= NULL;

	m_X2Publisher			= XP_K3RCOMBO_US;

	m_bManualLogin			= true;
	m_fServerDataTime		= 0.0f;
	m_bCenterServerDie		= false;

	m_iServerUserCount			= 0;
	m_iCurPartyCount			= 0;
	m_iPlayGamePartyCount		= 0;

	m_bAutoQuit			= false;
	m_fAutoQuitTime		= 0.0f;
	m_fAutoQuitCoolTime = (float)RandomNumber( 5, 10 );
	m_fTimeToUpdateProcessList = 0;

	m_pPartyUI = NULL;

	m_fInfoBoxLifeTime = -1.f;
	m_pDlgInfoBox = NULL;





	m_hInternet = NULL;
	// HTTP Open
	if ( NULL == ( m_hInternet = ::InternetOpen( L"HTTP_WRAPPER_KOG",
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL,
		NULL,
		0 ) ) )
	{
		ErrorLog( XEM_ERROR106 );
	}

	m_pCheckSumManager		= new CX2CheckSumManager();

#if !X2_DEV_TOOLS
	dbg::logfile::SetLogLevel(-1);

		m_bManualLogin = false;

#endif

	switch( GetPublisher() )
	{
		case XP_CJ_ID:
		case XP_GIANT_CN:
		case XP_GAMEFORGE_EU:
		case XP_ASIASOFT_TH:
		case XP_LEVELUP_BR:
			{
				m_bManualLogin = true;
			}
			break;
		case XP_K3RCOMBO_US:
			{

				if( CX2Steam::GetSteamUserIDAndPass(m_wstrID, m_wstrPass) )
					m_bManualLogin = false;
				else if( __argc == 3 )
				{
					m_bManualLogin = false;
					// ????ID -> ????ID?? ????
					char* tempArgv = __argv[1];
					wstring wstrID;
					wstrID.clear();
					char IDString[2000] = {0,};
					strncpy( IDString, tempArgv , 1999 );
					ConvertCharToWCHAR(m_wstrID, IDString, strlen(IDString));
					// ?????? ???? ??? -> ???? PW?? ????
					m_wstrPass = L"81621XZJ2380DE";
				}
				else
					m_bManualLogin = true;
			}
			break;

		default:
			break;
	}
	m_iChannelServerPort	= 9400;
	m_iPickedChannelServerIPIndex = -1;
	m_iDefaultChannelServerIPIndex = 0;


	m_ConnectedGameServerIP 	= L"";
	m_iConnectedGameServerPort	= 0;
	m_ServerUDPPort				= 0;
	m_GameP2PPort				= rand()%1000 + 8000;
	m_fServerTimeOut		= 5.0f;
	m_FrameBufferNum		= 7;

	CX2PacketLog::SetEnable( false );

	m_pData					= new CX2Data();
	g_pData					= m_pData;
	g_pInstanceData			= new CX2InstanceData;

	if(m_bManualLogin == false)
	{
		g_pInstanceData->SetUserID( m_wstrID );
		g_pInstanceData->SetUserPassword( m_wstrPass );

		if( __argc == 3 )
			// ???? ID?? ??? ?????? ??? ?????? ????.
			g_pInstanceData->SetUserSessionID(m_wstrID);
	}


	m_pNowState				= NULL;
	m_NowStateID			= XS_INVALID;

	m_SquareMaxUserNum	= 0;

	g_pKTDXApp->GetDeviceManager()->AddHighTexName( L"DLG_" );
	g_pKTDXApp->GetDeviceManager()->AddHighTexName( L"HQ_" );


	for( int i = 0; i < (int)m_KOMList.size(); i++ )
	{
		wstring komFileName = L"./";
		komFileName += m_KOMList[i];
#if !X2_DEV_TOOLS
		bool bIsDuplictaionError = false;
		if( NULL == g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->AddMassFile( komFileName.c_str(), bIsDuplictaionError ) )
		{
			// kom ?????? ?��????? ???????. ??????? ????????.
			int *x = NULL;
			*x = 1;

			if( true == bIsDuplictaionError )
			{
				// ????? ????
				//CX2MailLogManager::GetInstance()->AddLog(CX2MailLogManager::MLI_MASS_FILE_DUPLICATION, komFileName.c_str() );
			}
		}
#else

		bool bIsDuplictaionError = false;
		if( NULL == g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->AddMassFile( komFileName.c_str(), bIsDuplictaionError) &&
			true == bIsDuplictaionError )
		{
			// ????? ????
			//CX2MailLogManager::GetInstance()->AddLog(CX2MailLogManager::MLI_MASS_FILE_DUPLICATION, komFileName.c_str() );
		}

#endif // _SERVICE_
	}

//#ifdef	X2OPTIMIZE_REMOVE_MASS_FILE_MANAGER_LOCK
	// ?? ???��??? AddMassFile ???!
	g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LockMassFileMap();
//#endif	X2OPTIMIZE_REMOVE_MASS_FILE_MANAGER_LOCK




	//{{ ????? : [2010/05/05]	//	???????? ????
	SiCX2DLLManager()->Init();
	//}}

	m_bAutoStart = false;
	m_AutoLoginUnitIndex = 0;


	m_bIsPlayFieldDungeon	= false;
	m_bIsExitFieldDungeon   = false;
	m_bIsPlayingTutorial 	= false;
	m_bIsExitingTutorial 	= false;
	m_StateIDBeforeTutorial = XS_INVALID;
	m_iRandLoadingBG		= 0;

	m_iTrainingRewardED = 0;
	m_iTrainingRewardEXP = 0;
	m_bTrainingGameSuccess = false;

	m_bLastStageWasTrainingGame = false;
	m_bIsPlayingFreeTraining = false;

	m_bAutoStartNextTraining = false;
	m_iNowTrainingID = -1;

	m_pDLGStateChange = NULL;



	// dmlee: 2008-11-3, ???? ktdxapp?? ??? ??? ????? ???????
	//{{ robobeg : 2008-10-28
// 	HRESULT hr = g_pKTDXApp->LoadKLuaManagerTemplate();
// 	ASSERT( SUCCEEDED( hr ) );
	//}} robobeg : 2008-10-28


	RegisterLuabind();
	LoadLuaEnum();

//#ifndef SERV_MASSFILE_MAPPING_FUNCTION
	g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->MassFileMapping();
//#endif SERV_MASSFILE_MAPPING_FUNCTION

	//{{ megagame / ????? / 2010.05.12 / ????? ????
	m_pSystemInfo = new CX2SystemInfo();
	//}} megagame / ????? / 2010.05.12 / ????? ????

	InstallFont( FT_DEFAULT_FONTNAME, "Gulim.ttc" );
	//InstallFont( "2002L_KOG", "2002LKOG.ttf" );

	//{{ 2013.02.27 ?????	??? ?? ???? ??? (SERV_GLOBAL_BASE)
	std::wstring wstrBasicConfigFile = GetWstrBasicConfigFile();
	OpenScriptFile(wstrBasicConfigFile.c_str());
	//}}





//#ifndef OPEN_TEST_1_NO_MESSENGER_CASHSHOP
	g_pData->ResetMessenger();
//#endif OPEN_TEST_1_NO_MESSENGER_CASHSHOP


//{{????? ?????��? ??????



#ifndef BUG_TRAP
	string clientVersion = "";

#if !X2_DEV_TOOLS
		clientVersion = "[CRASH_REPORT_SERVICE] ";
#else
	clientVersion = "[CRASH_REPORT_TEST] ";
#endif // _SERVICE_


	string clientMainVersion = "";
	ConvertWCHARToChar( clientMainVersion, GetClientVersion() );
	clientVersion += "_(";
	clientVersion += clientMainVersion;
	string subClientVersion = "";
	ConvertWCHARToChar( subClientVersion, GetSubClientVersion_LUA() );
	if( subClientVersion.length() > 0 )
	{
		clientVersion += "_";
		clientVersion += subClientVersion;
	}
	clientVersion += ")_";

#ifndef _DEBUG
	m_pCrashReportState = Install( CrashReportCallback, (LPCTSTR)"wonpok@kogstudios.com", (LPCTSTR)clientVersion.c_str() );
#endif // _DEBUG

#endif // BUG_TRAP

	m_pStringFilter			= new CX2StringFilter();
	m_pGameEdit				= new CX2GameEdit();
	m_pTextManager			= new CX2TextManager();
	m_pNoticeManager		= new CX2NoticeManager( 150 );

	m_pCouponBox			= new CX2CouponBox( NULL );

	m_pTutorSystem			= new CX2TutorSystem( NULL );

	m_pX2PVPEmblem			= new CX2PVPEmblem();
	m_pInformerManager		= new CX2InformerManager();
	m_pMemoryHolder			= new CX2MemoryHolder();




	m_pLVUpEventMgr			= new CX2LVUpEventMgr();

    m_pPostBox              = new CX2PostBox(NULL);

	m_pSecurityPad			= new CX2SecurityPad();

	m_pKeyPad		= new CX2KeyPad();



	m_pFontForDebug = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_15_BOLD );



	//m_vecTimedMessagePopUp.reserve( 64 );




#if !X2_DEV_TOOLS
		wstringstream wstrmWindowTitle;
		wstrmWindowTitle << L"Elsword - " << m_ClientVersion.c_str() << L"_" << m_SubClientVersion.c_str();
		SetWindowText( g_pKTDXApp->GetHWND(), wstrmWindowTitle.str().c_str() );
#else

	wstringstream wstrmWindowTitle;
	wstrmWindowTitle << L"X2_NEW_VILLAGE_" << m_ClientVersion.c_str() << L"; compiled at " << __DATE__ << L", " << __TIME__;


	SetWindowText( g_pKTDXApp->GetHWND(), wstrmWindowTitle.str().c_str() );

#endif // _SERVICE_


	m_BrowserWrapper.Init();

	g_pKTDXApp->GetDIManager()->SetEnable( true );

	FreeConsole();
	if ( g_fpStream != NULL )
		fclose( g_fpStream );
	if ( g_fpStreamError != NULL )
		fclose( g_fpStreamError );

	g_pKTDXApp->InsertStage( m_pData );
    g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, XS_START_UP, NULL, false );

	m_bHackingUser = false;

	m_wstrPublicIp = L"";

	m_fCheckHackToolTime = 0.f;

#	if !X2_DEV_TOOLS
	// checked
	ELSWORD_VIRTUALIZER_START
#	endif

	m_fCheckDebugeTimer = 0.f;


	m_threadCheckWindowInfo = NULL;



		m_threadCheckWindowInfo = new CheckWindowInfo(g_pKTDXApp,g_pInstanceData
            );
		m_threadCheckWindowInfo->BeginThread();




#	if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#	endif

g_pKTDXApp->GetDeviceManager()->ResumeThread();
 	CSystemInfo* pSystemInfo = new CSystemInfo();
 	pSystemInfo->Init();
 	const unsigned char* pMachineId = pSystemInfo->GetMachineId();
 	std::string enMachineId = base64_encode(pMachineId, 16);
	delete pSystemInfo;

#if X2_DEV_TOOLS
 	FILE *fp = NULL;
	fp = fopen("MachineId.info", "w");
	if( fp != NULL )
	{
		fprintf(fp, "%s", enMachineId.c_str() );
		fclose(fp);
	}
#endif

	ErrorLogMsg( XEM_ERROR1, enMachineId.c_str() );

#if X2_DEV_TOOLS
	DeleteFileA("OpenScriptFile.log");
#endif //LOG_LOAD_DLG_SCRIPT

#if X2_DEV_TOOLS
#endif

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	_InitKOGGamePerformanceCheck();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK


}




CX2Main::~CX2Main(void)
{
// #ifdef MOVIE_TEST	 	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	if ( NULL != GetCheckWindowInfo() )
	{
		GetCheckWindowInfo()->SetNullToKtdxappAndInstanceData();
		GetCheckWindowInfo()->SetForceExitThread( false );
	}


// #endif //  MOVIE_TEST	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

#ifdef APPLY_THEMIDA
	CODEREPLACE_START
#endif

	DllManagerReport();

#ifdef APPLY_THEMIDA
	CODEREPLACE_END
#endif



	SAFE_DELETE(m_threadCheckWindowInfo);






	//{{ 2011.9.16	?????  ???? ?????? ��?? ????

// #ifdef MOVIE_TEST	 	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	if ( NULL != g_pData )
		g_pData->DeleteUIManager();

// #endif //  MOVIE_TEST	// 13-11-11, ???? ???? ????, kimjh, MOVIE_TEST ?? ??�� ????? Define ?? MOVIE_TEST_BASE ?? ????

	m_BandiCaptureLibrary.Destroy();
	//}}

	DeletePlayTimeInformation();		//?��??? ?��? ???? ????
	DeleteShutDownInformation();		//???? ???? ????



	m_BrowserWrapper.UnInit();
	SendQuitMsgToServer();

	for( int i = 0; i < (int)m_WaitServerPacketList.size(); i++ )
	{
		WaitServerPacket* pWaitServerPacket = m_WaitServerPacketList[i];
		SAFE_DELETE( pWaitServerPacket );
	}
	m_WaitServerPacketList.clear();
	m_mapWaitServerPacketList.clear();


	SAFE_DELETE( g_pInstanceData );


	SAFE_DELETE( m_pStringFilter );
	SAFE_DELETE( m_pGameEdit );
	SAFE_DELETE( m_pTextManager );
	SAFE_DELETE( m_pNoticeManager );
	SAFE_DELETE( g_pChatBox );
	SAFE_DELETE( m_pCheckSumManager );

	SAFE_DELETE( m_pTutorSystem );

	SAFE_DELETE( m_pCouponBox );

	SAFE_DELETE( m_pX2PVPEmblem );
	SAFE_DELETE( m_pInformerManager );
	SAFE_DELETE( m_pMemoryHolder );
	SAFE_DELETE( m_pLVUpEventMgr );

	SAFE_DELETE( m_pPartyUI );

    SAFE_DELETE( m_pPostBox );

	SAFE_DELETE( m_pSecurityPad	);

	SAFE_DELETE( m_pKeyPad );

	g_pData->DeleteMessenger();

	switch( GetPublisher() )
	{
		default:
			break;
	}

	if( m_hInternet != NULL )
		InternetCloseHandle( m_hInternet );


	for ( int i = 0; i < (int)m_UseFontNameList.size(); i++ )
	{
		wstring fontName = m_UseFontNameList[i];
		//RemoveFontResourceEx( fontName.c_str(), FR_PRIVATE, NULL );
		if( RemoveFontResource( fontName.c_str() ) == false )
		{
			int i = 0;
		}
	}
/*
	InvalidateRect( g_pKTDXApp->GetHWND(), NULL, TRUE );
	SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0 );
*/
	m_UseFontNameList.clear();

#if !X2_DEV_TOOLS
#endif //_SERVICE_

	//{{ ????? : [2010/03/24]	//	???????? ????
	SiCX2DLLManager()->ReleaseInstance();
	//}}

//{{ megagame / ????? / 2010.05.12 / ????? ????
	SAFE_DELETE( m_pSystemInfo );
//}} megagame / ????? / 2010.05.12 / ????? ????

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck_CallbackFuncTable::DeleteInstance();
	KOGGamePerformanceCheck::DeleteInstance();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	CX2EmblemManager::DestroyInstance();

	CX2MailLogManager::GetInstance()->SendMail();
	CX2MailLogManager::DestroyInstance();

	CX2BossRaidManager::DestroyInstance();


	g_pMain = NULL;

}

//{{ 2013.02.27 ?????	??? ?? ???? ??? (SERV_GLOBAL_BASE)
std::wstring CX2Main::GetWstrBasicConfigFile()
{
	std::wstring wstrBasicConfigFile = L"Config_INTERNAL.lua";
//	std::wstring wstrBasicConfigFile = L"Config.lua";

#if !X2_DEV_TOOLS
#if CLIENT_COUNTRY_US
	wstrBasicConfigFile = L"Config_US_SERVICE.lua";
#else
	wstrBasicConfigFile = L"Config_SERVICE.lua";
#endif
#else // INTERNAL
#if CLIENT_COUNTRY_US
	wstrBasicConfigFile = L"Config_US_INTERNAL.lua";
#else
	wstrBasicConfigFile = L"Config_INTERNAL.lua";
#endif
#endif // _SERVICE_

	return wstrBasicConfigFile;
}

HRESULT CX2Main::OnFrameMove( double fTime, float fElapsedTime )
{

#if !X2_DEV_TOOLS
	// checked
	ELSWORD_VIRTUALIZER_START
#endif



		switch( GetNowStateID() )
		{
		case XS_DUNGEON_GAME:
		case XS_VILLAGE_MAP:
		case XS_SQUARE_GAME:
		case XS_WEDDING_GAME:
			{
				if( (g_pData != NULL && ( g_pData->GetCashShop() == NULL || (g_pData->GetCashShop() != NULL && g_pData->GetCashShop()->GetOpen() == false) ) ) )
				{
					m_fCheckHackToolTime += fElapsedTime;
					if(	m_fCheckHackToolTime > 60.f )
					{
						m_fCheckHackToolTime = 0.f;
					}
				}
				else
				{
					m_fCheckHackToolTime = 0.f;
				}
			} break;
		default:
			break;
		}

#if !X2_DEV_TOOLS

	m_fCheckDebugeTimer += fElapsedTime;
	if( m_fCheckDebugeTimer > 20.f )
	{
		if( Pesudo_IsDebuggerPresent() ) //|| CheckDebugger() )
		{
			if( g_pData != NULL && g_pData->GetServerProtocol() != NULL &&
				g_pKTDXApp->GetFindHacking() == false )
			{
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );

				if( g_pData != NULL && g_pData->GetMyUser() != NULL )
					g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
			}

			SendHackMail_DamageHistory( ANTI_HACK_STRING_AntiHacking_IsDebuggerPresent );



			g_pKTDXApp->SetFindHacking( true );
		}

		m_fCheckDebugeTimer = 0.f;
	}

#endif // _SERVICE_




	//if( IsSamef(m_fCheckDebugeTimer) == true )
	{
		// 20??? ????? ??????????? ????? ???
		if( m_threadCheckWindowInfo == NULL )
		{
			m_threadCheckWindowInfo = new CheckWindowInfo(g_pKTDXApp,g_pInstanceData
                );
		}

		bool bIsvalidCheckThread = true;
		if( m_threadCheckWindowInfo != NULL )
		{
 			if( m_threadCheckWindowInfo->GetThreadHandle() != NULL )
 			{
 				DWORD dwExitCode = 0;
 				::GetExitCodeThread( m_threadCheckWindowInfo->GetThreadHandle(), &dwExitCode );
 				if( dwExitCode != STILL_ACTIVE )
 					bIsvalidCheckThread = false;
 			}
			if( m_threadCheckWindowInfo->GetThreadHandle() == NULL || m_threadCheckWindowInfo->GetIsLoop() == false )
			{
				bIsvalidCheckThread = false;
			}
		}

		if( m_threadCheckWindowInfo != NULL && bIsvalidCheckThread == false )
		{
			m_threadCheckWindowInfo->BeginThread();

			if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			{
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
			}
			SendHackMail_DamageHistory( ANTI_HACK_STRING_UserID_AntiHacking_CheckWindowInfo_Fail );

		}
	}





#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif



	for( int i = 0; i < (int)m_WaitServerPacketList.size(); i++ )
	{
		WaitServerPacket* pWaitServerPacket = m_WaitServerPacketList[i];
		pWaitServerPacket->fReamainTimeOut -= fElapsedTime;
		if( pWaitServerPacket->fReamainTimeOut <= 0.0f )
		{
#if X2_DEV_TOOLS
			std::map<int, int>::iterator it;
			it = m_mapWaitServerPacketList.find( (int) pWaitServerPacket->waitACK );
			if( it != m_mapWaitServerPacketList.end() )
			{
				it->second -= 1;

				if( it->second <= 0 )
				{
					m_mapWaitServerPacketList.erase( it );
				}
			}
#endif // _SERVICE_

			g_pKTDXApp->SendGameMessage( XGM_SERVER_PACKET_TIMEOUT, pWaitServerPacket->waitACK, NULL, false );
			SAFE_DELETE( pWaitServerPacket );
			m_WaitServerPacketList.erase( m_WaitServerPacketList.begin() + i );
			i--;
		}
	}


	if( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		m_fServerDataTime += fElapsedTime;
		if( m_fServerDataTime > 60.0f )
		{
			m_fServerDataTime = 0.0f;

			if( g_pData->GetServerProtocol() != NULL )
			{
				g_pData->GetServerProtocol()->SendID( EGS_ADMIN_SERVER_STATUS_REQ );
			}
		}
	}




	if ( NULL != m_pNoticeManager )
		m_pNoticeManager->OnFrameMove( fTime, fElapsedTime );




	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_SYSRQ) == TRUE )
	{
		if( CX2Main::XS_PVP_GAME == GetNowStateID() &&
			KPVPChannelInfo::PCC_TOURNAMENT == GetPVPChannelClass( GetConnectedChannelID() ) )
		{
			if ( g_pChatBox != NULL )
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_346 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
		else
		{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			KOGGamePerformanceCheck::GetInstance()->Pause();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

			bool bSuccessCapture = false;
			//{{ 2011.9.16	?????  ???? ??????? ��?? ????
			bSuccessCapture = Bandi_ScreenCapture();
			//}}

			if ( bSuccessCapture )
			{
				if ( g_pChatBox != NULL )
					g_pChatBox->AddChatLog( GET_STRING( STR_ID_347 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
			}
			else
			{
				if ( g_pChatBox != NULL )
					g_pChatBox->AddChatLog( GET_STRING( STR_ID_348 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
			}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			KOGGamePerformanceCheck::GetInstance()->Resume();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

            if ( g_pData->GetGameUDP() != NULL )
                g_pData->GetGameUDP()->RemoveAllPendingPingSends();
		}
	}





#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif


#ifndef _DEBUG
	if ( g_pKTDXApp->GetFindHacking() == true )
	{
		static bool staticExitCheck = false;
		if ( staticExitCheck == false )
		{
			staticExitCheck = true;
			/*
			char buff[256] = {0};
			string cmdString;
			ConvertWCHARToChar( cmdString, g_pKTDXApp->GetFindHackingDetailMsg() );
			sprintf( buff, "FindH.dll %s", cmdString.c_str() );
			WinExec( buff, SW_HIDE );
			*/


			//g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_FIND_HACKING );

			m_bAutoQuit = true;
		}

	}

	if ( g_pKTDXApp->GetGameGuardFindHacking() == true )
	{
		static bool staticGameGuardExitCheck = false;
		if ( staticGameGuardExitCheck == false )
		{
			staticGameGuardExitCheck = true;
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_27366 ), NULL );
			m_bAutoQuit = true;
		}
	}

	if ( m_bAutoQuit == true )
	{
		m_fAutoQuitTime += fElapsedTime;
		if ( m_fAutoQuitTime >= m_fAutoQuitCoolTime )
		{
			SetQuitType( NetError::ERR_CLIENT_QUIT_01 );
			PostQuitMessage(0);
		}
	}
#endif // _DEBUG


#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif





#ifndef _DEBUG
	//nProtect ?? ?????? ???? ???? ????????
	if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
		KNP_GetFindGameMon() == false )
	{
		SetQuitType( NetError::ERR_CLIENT_QUIT_01 );
		g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_FIND_HACKING );
		PostQuitMessage(0);
	}
#endif // _DEBUG


	if( NULL != GetPartyUI() )
	{
		GetPartyUI()->OnFrameMove( fTime, fElapsedTime );
	}

	//{{ kimhc // 2010.01.22 // list?? ????
	list<TimedMessagePopUp>::iterator ItrTimedPopUp = m_listTimedMessagePopUp.begin();

	while ( ItrTimedPopUp != m_listTimedMessagePopUp.end() )
	{
		if ( ItrTimedPopUp->m_fTimeLeft <= 0.f )
		{
			if ( -1 != ItrTimedPopUp->m_iTimeOutMsg )
			{
				g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, ItrTimedPopUp->m_iTimeOutMsg, NULL, false );
			}

			m_listTimedMessagePopUp.erase( ItrTimedPopUp++ );
			continue;
		}

		ItrTimedPopUp->OnFrameMove( fTime, fElapsedTime );

		ItrTimedPopUp++;
	}
// 	for( int i=0; i< (int)m_listTimedMessagePopUp.size(); i++ )
// 	{
// 		TimedMessagePopUp& timedPopUp = m_vecTimedMessagePopUp[i];
//
// 		if( timedPopUp.m_fTimeLeft <= 0.f )
// 		{
// 			if( -1 != timedPopUp.m_iTimeOutMsg )
// 			{
// 				g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, timedPopUp.m_iTimeOutMsg, NULL, false );
// 			}
//
// 			m_vecTimedMessagePopUp.erase( m_vecTimedMessagePopUp.begin() + i );
// 			i--;
// 			continue;
// 		}
//
// 		timedPopUp.OnFrameMove( fTime, fElapsedTime );
// 	}
	//}} kimhc // 2010.01.22 // list?? ????

	if( m_pDlgInfoBox != NULL && m_fInfoBoxLifeTime > 0.f )
	{
		m_fInfoBoxLifeTime -= fElapsedTime;
		if( m_fInfoBoxLifeTime <= 0.f && m_fInfoBoxLifeTime > -1.f )
		{
			g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg(m_pDlgInfoBox);
			m_fInfoBoxLifeTime = -1.f;
		}

		if( m_fInfoBoxLifeTime <= -1.f )
			m_fInfoBoxLifeTime = -1.f;
	}

	m_fCaptureDelay -= fElapsedTime;
	if( m_fCaptureDelay < 0.0f )
		m_fCaptureDelay = 0.0f;

	Bandi_EnoughDiskSpace(fElapsedTime);

	return S_OK;
}



bool CX2Main::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case XGM_STATE_CHANGE:
			{
				X2_STATE state = (X2_STATE)wParam;
				int iDetailStateID = (int)lParam;

				return StateChange( state, iDetailStateID );
			}
			break;

		case XGM_DELETE_DIALOG:
			{
                //CKTDGUIDialogType pDialog( CKTDGUIDialog::MakeValidHandle( wParam ) );
				// added by wonpok. 20090820.
				CKTDGUIDialogType pDialog( CKTDGUIDialog::MakeValidHandle( wParam, lParam ) );

				if ( pDialog == NULL || g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( pDialog ) == false )
				{
					//g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg( pDialog );
					return true;
				}

				wstring MsgBoxName = pDialog->GetName();

				if ( MsgBoxName == L"OkAndCancelMsgBox" )
				{
					g_pKTDXApp->GetDIManager()->SetEnable(true);
				}

				D3DXCOLOR tempColor = pDialog->GetColor();
				tempColor.a = 0.0f;
				pDialog->Move(pDialog->GetPos(), tempColor, 0.15f, true, true);
				return true;
			}
			break;

		case XGM_REQUEST_FOCUS_CONTROL:
			{
				CKTDGUIControl*	pControl = (CKTDGUIControl*)wParam;
				pControl->RequestFocus();
				return true;
			}
			break;

		case XGM_QUIT_GAME:
			{
			//{{ kimhc // 2010.3.10 //	???????? ????��?? ????
			//}} kimhc // 2010.3.10 //	???????? ????��?? ????
#ifdef TEMP_CRASH_LOG
				KLOG("CrashLog.txt")  << L"XGM_QUIT_GAME Received" << fileout;
#endif // TEMP_CRASH_LOG


				PostQuitMessage(0);
				return true;
			}
			break;
		case WM_SYSCOMMAND:
			if( wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER )
			{
				return true;
			}
			break;

		case CKTDXApp::KM_WEB_BROWSER_KEY_EVENT:
			{
				if( NULL != GetBrowserWrapper() && false == GetBrowserWrapper()->IsClosed() )
				{
					GetBrowserWrapper()->OnKeyboardEvents( (LPMSG) lParam );
				}
				return true;
			}
			break;
	}

	switch(wParam)
	{
		//{{????? ?????��? ??????
	}

	if( GetPartyUI() != NULL && GetPartyUI()->GetShow() == true &&
		GetPartyUI()->MsgProc(hWnd, uMsg, wParam, lParam) )
	{
		return true;
	}

// oasis907 : ????? [2010.6.17] //  ?????? ??? ??? ?????????.
	if( NULL != g_pData->GetEventScene() &&
		true == g_pData->GetEventScene()->MsgProc(hWnd, uMsg, wParam, lParam ) )
	{
		return true;
	}


	return false;
}



void CX2Main::RegisterLuabind()
{

	lua_State* L= g_pKTDXApp->GetLuaBinder()->GetLuaState();


	lua_tinker::decl( L,  "g_pMain", this );

	lua_tinker::class_add<CX2Room>( L, "CX2Room" );
	lua_tinker::class_def<CX2Room>( L, "GetSlotData",							&CX2Room::GetSlotData );
	lua_tinker::class_def<CX2Room>( L, "GetSlotNum",							&CX2Room::GetSlotNum );
	lua_tinker::class_def<CX2Room>( L, "GetRoomPublic",							&CX2Room::GetRoomPublic );
	lua_tinker::class_def<CX2Room>( L, "GetRoomState",							&CX2Room::GetRoomState_LUA );
	lua_tinker::class_def<CX2Room>( L, "GetMySlot",								&CX2Room::GetMySlot );



	lua_tinker::class_add<CX2EventScene>( L, "CX2EventScene" );
	lua_tinker::class_def<CX2EventScene>( L, "AddEventSceneObject_LUA",			&CX2EventScene::AddEventSceneObject_LUA );
	lua_tinker::class_def<CX2EventScene>( L, "AddEventScene_LUA",				&CX2EventScene::AddEventScene_LUA );



	lua_tinker::class_add<CX2SlideShot>( L, "CX2SlideShot" );
	lua_tinker::class_def<CX2SlideShot>( L, "ScenStart_LUA",					&CX2SlideShot::ScenStart_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "ScenWait",							&CX2SlideShot::ScenWait);
	lua_tinker::class_def<CX2SlideShot>( L, "ScenEnd",							&CX2SlideShot::ScenEnd);
	lua_tinker::class_def<CX2SlideShot>( L, "GoNextScen",						&CX2SlideShot::GoNextScen);
	lua_tinker::class_def<CX2SlideShot>( L, "GetNowTime",						&CX2SlideShot::GetNowTime);
	lua_tinker::class_def<CX2SlideShot>( L, "EventTimer",						&CX2SlideShot::EventTimer);
	lua_tinker::class_def<CX2SlideShot>( L, "CannotInput",						&CX2SlideShot::CannotInput );
	lua_tinker::class_def<CX2SlideShot>( L, "AddText_LUA",						&CX2SlideShot::AddText_LUA );
	lua_tinker::class_def<CX2SlideShot>( L, "AddText2_LUA",						&CX2SlideShot::AddText2_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "AddSeqMap_LUA",					&CX2SlideShot::AddSeqMap_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "CrashSeq_LUA",						&CX2SlideShot::CrashSeq_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "ChangeColor_LUA",					&CX2SlideShot::ChangeColor_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "ChangeTex_LUA",					&CX2SlideShot::ChangeTex_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "ChangeSize_LUA",					&CX2SlideShot::ChangeSize_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "DeleteSeq_LUA",					&CX2SlideShot::DeleteSeq_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "SetTextBoxShow",					&CX2SlideShot::SetTextBoxShow);
	lua_tinker::class_def<CX2SlideShot>( L, "BlackLayOutMove",					&CX2SlideShot::BlackLayOutMove );
	lua_tinker::class_def<CX2SlideShot>( L, "SetTextSpread",					&CX2SlideShot::SetTextSpread);
	lua_tinker::class_def<CX2SlideShot>( L, "IsTextSpreading",					&CX2SlideShot::IsTextSpreading );
	lua_tinker::class_def<CX2SlideShot>( L, "GetElapsedTimeAfterTextSpread",	&CX2SlideShot::GetElapsedTimeAfterTextSpread );
	lua_tinker::class_def<CX2SlideShot>( L, "IsPresentNow",						&CX2SlideShot::IsPresentNow);
	//{{ 2010.04.28 / ????? / ?????? ??? ????
	lua_tinker::class_def<CX2SlideShot>( L, "SetNameTabShow",					&CX2SlideShot::SetNameTabShow);
	lua_tinker::class_def<CX2SlideShot>( L, "SetSpeechBallonShow",				&CX2SlideShot::SetSpeechBallonShow);
	lua_tinker::class_def<CX2SlideShot>( L, "SetEnterShow",						&CX2SlideShot::SetEnterShow);
	//}}
	lua_tinker::class_def<CX2SlideShot>( L, "PlaySound2D_LUA",					&CX2SlideShot::PlaySound2D_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "StopAllSound_LUA",					&CX2SlideShot::StopAllSound_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "SetSlideBGM_LUA",					&CX2SlideShot::SetSlideBGM_LUA);
	lua_tinker::class_def<CX2SlideShot>( L, "ResetBGM",							&CX2SlideShot::ResetBGM);
    lua_tinker::class_def<CX2SlideShot>( L, "SetNPC",                           &CX2SlideShot::SetNPC);
    lua_tinker::class_def<CX2SlideShot>( L, "ResetNPC",                         &CX2SlideShot::ResetNPC);
    lua_tinker::class_def<CX2SlideShot>( L, "GetNPC",                           &CX2SlideShot::GetNPC);
	lua_tinker::class_def<CX2SlideShot>( L, "GetElapsedTimeAfterVoiceStopped",	&CX2SlideShot::GetElapsedTimeAfterVoiceStopped );


	lua_tinker::class_add<CX2TrainingGameUI>( L, "CX2TrainingGameUI" );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ScenStart_LUA",						&CX2TrainingGameUI::ScenStart_LUA);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ScenWait",							&CX2TrainingGameUI::ScenWait);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ScenEnd",								&CX2TrainingGameUI::ScenEnd);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GoNextScen",							&CX2TrainingGameUI::GoNextScen);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetNowTime",							&CX2TrainingGameUI::GetNowTime);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "EventTimer",							&CX2TrainingGameUI::EventTimer);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddText_LUA",							&CX2TrainingGameUI::AddText_LUA );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsKeyPressed",						&CX2TrainingGameUI::IsKeyPressed);

	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearWaitingKeyPress",				&CX2TrainingGameUI::ClearWaitingKeyPress );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddWaitingKeyPress",					&CX2TrainingGameUI::AddWaitingKeyPress );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddWaitingKeyPressScaled",			&CX2TrainingGameUI::AddWaitingKeyPressScaled );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "SetTextSpread",						&CX2TrainingGameUI::SetTextSpread);
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsTextSpreading",						&CX2TrainingGameUI::IsTextSpreading );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetElapsedTimeAfterTextSpread",		&CX2TrainingGameUI::GetElapsedTimeAfterTextSpread );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "SetEnableWaitingUserState",			&CX2TrainingGameUI::SetEnableWaitingUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddWaitingUserState_LUA",				&CX2TrainingGameUI::AddWaitingUserState_LUA );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsWaitingUserState",					&CX2TrainingGameUI::IsWaitingUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearWaitingUserState",				&CX2TrainingGameUI::ClearWaitingUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddSimpleWaitingUserState",			&CX2TrainingGameUI::AddSimpleWaitingUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsWaitingSimpleUserState",			&CX2TrainingGameUI::IsWaitingSimpleUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearSimpleWaitingUserState",			&CX2TrainingGameUI::ClearSimpleWaitingUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetElapsedTimeAfterCompletingSimpleUserState",		&CX2TrainingGameUI::GetElapsedTimeAfterCompletingSimpleUserState );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearGatherItem",						&CX2TrainingGameUI::ClearGatherItem );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddGatherItem_LUA",					&CX2TrainingGameUI::AddGatherItem_LUA );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsWaitingGatherItem",					&CX2TrainingGameUI::IsWaitingGatherItem );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearWaitingSpecialAttack",			&CX2TrainingGameUI::ClearWaitingSpecialAttack );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddWaitingSpecialAttack_LUA",			&CX2TrainingGameUI::AddWaitingSpecialAttack_LUA );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsWaitingSpecialAttack",				&CX2TrainingGameUI::IsWaitingSpecialAttack );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddWaitingSeqUserState_LUA",			&CX2TrainingGameUI::AddWaitingSeqUserState_LUA );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "SetWaitingSeqUserState",				&CX2TrainingGameUI::SetWaitingSeqUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearWaitingSeqUserState",			&CX2TrainingGameUI::ClearWaitingSeqUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "IsWaitingSeqUserState",				&CX2TrainingGameUI::IsWaitingSeqUserState );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearKeySequence",					&CX2TrainingGameUI::ClearKeySequence );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "ClearTrainingObjective",				&CX2TrainingGameUI::ClearTrainingObjective );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddTrainingObjective",				&CX2TrainingGameUI::AddTrainingObjective );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddTrainingObjectivePicture",			&CX2TrainingGameUI::AddTrainingObjectivePicture );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "AddTrainingObjectivePictureScaled",	&CX2TrainingGameUI::AddTrainingObjectivePictureScaled );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "SetShowEnterMark",					&CX2TrainingGameUI::SetShowEnterMark );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "SetTwinkleEnterMark",					&CX2TrainingGameUI::SetTwinkleEnterMark );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "SetShowSuccessCountEffect",			&CX2TrainingGameUI::SetShowSuccessCountEffect );

	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetFTNPCID",							&CX2TrainingGameUI::GetFTNPCID );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetFTNPCLevel",						&CX2TrainingGameUI::GetFTNPCLevel );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetFTNPCCount",						&CX2TrainingGameUI::GetFTNPCCount );
	lua_tinker::class_def<CX2TrainingGameUI>( L, "GetFTEnableNPCAttack",				&CX2TrainingGameUI::GetFTEnableNPCAttack );




	lua_tinker::class_add<CX2Room::SlotData>( L, "CX2Room::SlotData" );
	//lua_tinker::class_def<TEST_CLASS>( L, lua_tinker::constructor<>( L ); );
	//.def_readwrite("m_bHost", &CX2Room::SlotData::m_bHost );
	lua_tinker::class_def<CX2Room::SlotData>( L, "GetIsHost",				&CX2Room::SlotData::GetIsHost );
	lua_tinker::class_def<CX2Room::SlotData>( L, "GetIsReady",				&CX2Room::SlotData::GetIsReady );
	lua_tinker::class_def<CX2Room::SlotData>( L, "GetIsPitIn",				&CX2Room::SlotData::GetIsPitIn );
	lua_tinker::class_def<CX2Room::SlotData>( L, "GetSlotState",			&CX2Room::SlotData::GetSlotState );
	lua_tinker::class_def<CX2Room::SlotData>( L, "GetTeamNum",				&CX2Room::SlotData::GetTeamNum );
	lua_tinker::class_def<CX2Room::SlotData>( L, "IsMyUnit",				&CX2Room::SlotData::IsMyUnit );


	lua_tinker::class_add<CX2User>( L, "CX2User" );
	lua_tinker::class_def<CX2User>( L, "GetUnitNum",		&CX2User::GetUnitNum );
	lua_tinker::class_def<CX2User>( L, "GetUnitByIndex",	&CX2User::GetUnitByIndex );



	lua_tinker::class_add<CX2Unit>( L, "CX2Unit" );
	lua_tinker::class_def<CX2Unit>( L, "GetClass",			&CX2Unit::GetClass );
	lua_tinker::class_def<CX2Unit>( L, "GetNickName",		&CX2Unit::GetNickName );





	lua_tinker::class_add<CX2PVPRoom>( L, "CX2PVPRoom" );
	lua_tinker::class_inh< CX2PVPRoom, CX2Room>( L );
	lua_tinker::class_def<CX2PVPRoom>( L, "GetPVPGameType",				&CX2PVPRoom::GetPVPGameType_LUA );
	lua_tinker::class_def<CX2PVPRoom>( L, "GetWinKillNum",				&CX2PVPRoom::GetWinKillNum );
	lua_tinker::class_def<CX2PVPRoom>( L, "GetPlayTime",				&CX2PVPRoom::GetPlayTimeLimit );
	lua_tinker::class_def<CX2PVPRoom>( L, "GetCanIntrude",				&CX2PVPRoom::GetCanIntrude );
	lua_tinker::class_def<CX2PVPRoom>( L, "GetWorldID",					&CX2PVPRoom::GetWorldID );
	lua_tinker::class_def<CX2PVPRoom>( L, "SetPVPGameType",				&CX2PVPRoom::SetPVPGameType );
	lua_tinker::class_def<CX2PVPRoom>( L, "SetWinKillNum",				&CX2PVPRoom::SetWinKillNum );
	lua_tinker::class_def<CX2PVPRoom>( L, "SetPlayTime",				&CX2PVPRoom::SetPlayTime );
	lua_tinker::class_def<CX2PVPRoom>( L, "SetCanIntrude",				&CX2PVPRoom::SetCanIntrude );
	lua_tinker::class_def<CX2PVPRoom>( L, "SetWorldID",					&CX2PVPRoom::SetWorldID );


	lua_tinker::class_add<CX2DungeonRoom>( L, "CX2DungeonRoom" );
	lua_tinker::class_inh< CX2DungeonRoom, CX2Room>( L );
	lua_tinker::class_def<CX2DungeonRoom>( L, "GetDifficulty",			&CX2DungeonRoom::GetDifficulty );
	lua_tinker::class_def<CX2DungeonRoom>( L, "GetDungeonID",			&CX2DungeonRoom::GetDungeonID );
	lua_tinker::class_def<CX2DungeonRoom>( L, "SetDifficulty",			&CX2DungeonRoom::SetDifficulty );
	lua_tinker::class_def<CX2DungeonRoom>( L, "SetDungeonID",			&CX2DungeonRoom::SetDungeonID );


	lua_tinker::class_add<CX2Data>( L, "CX2Data" );
	lua_tinker::class_def<CX2Data>( L, "GetPVPRoom",					&CX2Data::GetPVPRoom );
	lua_tinker::class_def<CX2Data>( L, "GetDungeonRoom",				&CX2Data::GetDungeonRoom );
	lua_tinker::class_def<CX2Data>( L, "GetMyUser",						&CX2Data::GetMyUser );
	lua_tinker::class_def<CX2Data>( L, "GetUIMajorParticle",			&CX2Data::GetUIMajorParticle );
	lua_tinker::class_def<CX2Data>( L, "GetUIMajorXMeshPlayer",			&CX2Data::GetUIMajorXMeshPlayer );
	lua_tinker::class_def<CX2Data>( L, "GetUIMinorParticle",			&CX2Data::GetUIMinorParticle );
	lua_tinker::class_def<CX2Data>( L, "GetUIMinorXMeshPlayer",			&CX2Data::GetUIMinorXMeshPlayer );
	lua_tinker::class_def<CX2Data>( L, "GetUIUnitManager",				&CX2Data::GetUIUnitManager );
	lua_tinker::class_def<CX2Data>( L, "LoadPicCharAll_LUA",			&CX2Data::LoadPicCharAll_LUA );


	lua_tinker::class_add<CX2State>( L, "CX2State" );
	lua_tinker::class_inh<CX2State, CKTDXStage>( L );
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_NOTIFY_MSG_REQ",					&CX2State::Handler_EGS_NOTIFY_MSG_REQ_LUA );
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_MODIFY_UNIT_LEVEL_REQ",		&CX2State::Handler_EGS_ADMIN_MODIFY_UNIT_LEVEL_REQ_LUA );
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_CHANGE_ED_REQ",				&CX2State::Handler_EGS_ADMIN_CHANGE_ED_REQ_LUA );
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_KICK_USER_REQ",				&CX2State::Handler_EGS_ADMIN_KICK_USER_REQ_LUA );
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_SET_PC_BANG_REQ",			&CX2State::Handler_EGS_ADMIN_SET_PC_BANG_REQ );

//	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_REMOVE_HERO_PVP_USER_REQ",		&CX2State::Handler_EGS_ADMIN_REMOVE_HERO_PVP_USER_REQ );
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ_LUA",	&CX2State::Handler_EGS_ADMIN_NOTIFY_HERO_PVP_USER_REQ_LUA );


	//{{ 2012. 12. 5	??????		?????? ???? ???? ???
	lua_tinker::class_def<CX2State>( L, "Handler_EGS_ADMIN_OBSERVER_MODE_REQ",			&CX2State::Handler_EGS_ADMIN_OBSERVER_MODE_REQ );
	//}}

	lua_tinker::class_add<CX2StatePVPRoom>( L, "CX2StatePVPRoom" );
	lua_tinker::class_inh<CX2StatePVPRoom, CX2StateMenu>( L );
	lua_tinker::class_def<CX2StatePVPRoom>( L, "Handler_EGS_CHANGE_SLOT_OPEN_REQ",		&CX2StatePVPRoom::Handler_EGS_CHANGE_SLOT_OPEN_REQ );




	lua_tinker::class_add<CX2Main>( L, "CX2Main" );
	lua_tinker::class_def<CX2Main>( L, "SetChannelServerPort",			&CX2Main::SetChannelServerPort );
	lua_tinker::class_def<CX2Main>( L, "AddChannelServerIP_LUA",		&CX2Main::AddChannelServerIP_LUA );
	lua_tinker::class_def<CX2Main>( L, "SetDefaultChannelServerIPIndex",&CX2Main::SetDefaultChannelServerIPIndex );
	lua_tinker::class_def<CX2Main>( L, "SetServerTimeOut",				&CX2Main::SetServerTimeOut_LUA );
	lua_tinker::class_def<CX2Main>( L, "SetAutoLoginUnitIndex",			&CX2Main::SetAutoLoginUnitIndex );
	lua_tinker::class_def<CX2Main>( L, "SetSafetyKey",					&CX2Main::SetSafetyKey_LUA );
	lua_tinker::class_def<CX2Main>( L, "ResetFullScreenResolution",		&CX2Main::ResetFullScreenResolution );
	lua_tinker::class_def<CX2Main>( L, "KTDGUIOKMsgBox",				&CX2Main::KTDGUIOKMsgBox_LUA );
	lua_tinker::class_def<CX2Main>( L, "KTDGUIOkAndCancelMsgBox",		&CX2Main::KTDGUIOkAndCancelMsgBox_LUA );
	lua_tinker::class_def<CX2Main>( L, "InstallFont",					&CX2Main::InstallFont );
	lua_tinker::class_def<CX2Main>( L, "SetClientVersion_LUA",			&CX2Main::SetClientVersion_LUA );

	lua_tinker::class_def<CX2Main>( L, "DeleteFile",					&CX2Main::DeleteFile_LUA );
	lua_tinker::class_def<CX2Main>( L, "SetNationFlag",					&CX2Main::SetNationFlag_LUA );

	lua_tinker::decl( L, "NF_NONE",										CX2Main::NF_NONE );
	lua_tinker::decl( L, "NF_DE",										CX2Main::NF_DE );
	lua_tinker::decl( L, "NF_FR",										CX2Main::NF_FR );
	lua_tinker::decl( L, "NF_IT",										CX2Main::NF_IT );
	lua_tinker::decl( L, "NF_PL",										CX2Main::NF_PL );
	lua_tinker::decl( L, "NF_ES",										CX2Main::NF_ES );
	lua_tinker::decl( L, "NF_TR",										CX2Main::NF_TR );
	lua_tinker::decl( L, "NF_EN",										CX2Main::NF_EN );

	lua_tinker::decl( L, "NF_KR",										CX2Main::NF_KR );
	lua_tinker::decl( L, "NF_JP",										CX2Main::NF_JP );
	lua_tinker::decl( L, "NF_TW",										CX2Main::NF_TW );
	lua_tinker::decl( L, "NF_HK",										CX2Main::NF_HK );
	lua_tinker::decl( L, "NF_US",										CX2Main::NF_US );
	lua_tinker::decl( L, "NF_CN",										CX2Main::NF_CN );
	lua_tinker::decl( L, "NF_VN",										CX2Main::NF_VN );
	lua_tinker::decl( L, "NF_TH",										CX2Main::NF_TH );
	lua_tinker::decl( L, "NF_ID",										CX2Main::NF_ID );
	lua_tinker::decl( L, "NF_BR",										CX2Main::NF_BR );
	lua_tinker::decl( L, "NF_PH",										CX2Main::NF_PH );
	lua_tinker::class_def<CX2Main>( L, "BuyItemSuper_LUA",				&CX2Main::BuyItemSuper_LUA );
	lua_tinker::class_def<CX2Main>( L, "BuySuperEqipLevel_LUA",			&CX2Main::BuySuperEqipLevel_LUA );
	lua_tinker::class_def<CX2Main>( L, "SetAuthLevel_LUA",				&CX2Main::SetAuthLevel_LUA );
	lua_tinker::class_def<CX2Main>( L, "GetAuthUserList_LUA",			&CX2Main::GetAuthUserList_LUA );
	lua_tinker::class_def<CX2Main>( L, "SetSubClientVersion_LUA",		&CX2Main::SetSubClientVersion_LUA );
	lua_tinker::class_def<CX2Main>( L, "InitMyUnitSkillTree",			&CX2Main::InitMyUnitSkillTree );
	lua_tinker::class_def<CX2Main>( L, "SetMyUnitSP",					&CX2Main::SetMyUnitSP );
	lua_tinker::class_def<CX2Main>( L, "SendStatisticsLocLog",			&CX2Main::SendStatisticsLocLog );
	lua_tinker::class_def<CX2Main>( L, "SendStatisticsDBLog",			&CX2Main::SendStatisticsDBLog );
	lua_tinker::class_def<CX2Main>( L, "IntentionalCrash",				&CX2Main::IntentionalCrash );
	lua_tinker::class_def<CX2Main>( L, "SetFreeCamera",					&CX2Main::SetFreeCamera );
	lua_tinker::class_def<CX2Main>( L, "SetWorldCameraEdit",			&CX2Main::SetWorldCameraEdit );
	lua_tinker::class_def<CX2Main>( L, "PlayWorldCamera",				&CX2Main::PlayWorldCamera );
	lua_tinker::class_def<CX2Main>( L, "EndWorldCamera",				&CX2Main::EndWorldCamera );
	lua_tinker::class_def<CX2Main>( L, "SetProjectionFov",				&CX2Main::SetProjectionFov );

	//{{ 2008. 1. 2  ??????  ????? ??
	lua_tinker::class_def<CX2Main>( L, "ChangeSpirit",					&CX2Main::ChangeSpirit );
	lua_tinker::class_def<CX2Main>( L, "ChangeVSPoint",					&CX2Main::ChangeVSPoint );
	//}}
	lua_tinker::class_def<CX2Main>( L, "SetShowUnitViewerUI",			&CX2Main::SetShowUnitViewerUI );
	//{{ 2008. 2. 18  ??????  ???? ????? ??
	lua_tinker::class_def<CX2Main>( L, "ChangeUnitClass",				&CX2Main::ChangeUnitClass );
	lua_tinker::class_def<CX2Main>( L, "ModifyChangeUnitClass",			&CX2Main::ModifyChangeUnitClass );
	//}}

	// kimhc // 2011-05-30 // ????? ?? ???? ??
	lua_tinker::class_def<CX2Main>( L, "ChangeCashSkillPointDate",		&CX2Main::ChangeCashSkillPointDate );

	lua_tinker::class_def<CX2Main>( L, "SetSquareMaxUserNum",			&CX2Main::SetSquareMaxUserNum );
	//{{ 2008. 10. 24  ??????	???? ??
	lua_tinker::class_def<CX2Main>( L, "GetTitleCheat",					&CX2Main::GetTitleCheat );
	//}}
	//{{ 09.07.03 ?��? : ?ͧ???? ????? ???? ??? ???????
#if X2_DEV_TOOLS
	lua_tinker::class_def<CX2Main>( L, "SetGameServerPortForCreateID",			&CX2Main::SetGameServerPortForCreateID );
	lua_tinker::class_def<CX2Main>( L, "AddGameServerIPForCreateID_LUA",		&CX2Main::AddGameServerIPForCreateID_LUA );
#endif //!defined(_SERVICE_) || defined (SERV_JOIN_IN_CLIENT_FOR_TW_TEST_SERVER)
	lua_tinker::class_def<CX2Main>( L, "SetEnabledDeveloperCheatCode",			&CX2Main::SetEnabledDeveloperCheatCode );

	//{{ 2009. 7. 26  ??????	party fever cheat
	lua_tinker::class_def<CX2Main>( L, "IncreasePartyFeverPoint",				&CX2Main::IncreasePartyFeverPoint );
	//}}
	//{{ 2009. 8. 5  ??????		???ID??
	lua_tinker::class_def<CX2Main>( L, "SetGuestUserMode",						&CX2Main::SetGuestUserMode );
	//}}

	//{{ ????? : [2010/3/12/] //	???? ??? ???? ??
	lua_tinker::class_def<CX2Main>( L, "WarpByCommand",							&CX2Main::WarpByCommand );
	//}} ????? : [2010/3/12/] //


	lua_tinker::class_def<CX2Main>( L, "FullMyUnitSkillTree",					&CX2Main::FullMyUnitSkillTree );


	lua_tinker::class_def<CX2Main>( L, "FullMyUnitSkillTreeByLevel",			&CX2Main::FullMyUnitSkillTreeByLevel );
	lua_tinker::class_def<CX2Main>( L, "SetMyUnitSkillLevel",					&CX2Main::SetMyUnitSkillLevel );

	lua_tinker::class_def<CX2Main>( L, "FieldWarpByCommand",					&CX2Main::FieldWarpByCommand );




#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	lua_tinker::class_def<CX2Main>( L, "OccurLag_LUA",					&CX2Main::OccurLag_LUA );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	lua_tinker::class_def<CX2Main>( L, "ForceHost_LUA",				&CX2Main::ForceHost_LUA );
	lua_tinker::class_def<CX2Main>( L, "SwitchConnect_LUA",		&CX2Main::SwitchConnect_LUA );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

#ifdef UDP_PACKET_ANALYSIS_LOG
	lua_tinker::class_def<CX2Main>( L, "UdpPacketAnalysisLog_LUA",		&CX2Main::UdpPacketAnalysisLog_LUA );
#endif //UDP_PACKET_ANALYSIS_LOG

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	lua_tinker::class_def<CX2Main>( L, "UdpPacketOverlapTest_LUA",		&CX2Main::UdpPacketOverlapTest_LUA );
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK




//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    lua_tinker::class_def<CX2Main>( L, "SetUDPMode",       &CX2Main::SetUDPMode_LUA );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK



	lua_tinker::class_add<CX2QuestManager>( L, "CX2QuestManager" );
	lua_tinker::class_def<CX2QuestManager>( L, "AddQuestTemplet_LUA",			&CX2QuestManager::AddQuestTemplet_LUA );
	lua_tinker::class_def<CX2QuestManager>( L, "AddSubQuestTemplet_LUA",		&CX2QuestManager::AddSubQuestTemplet_LUA );
	//??????//2012-12-17// ???? ???? ??????? ??? ??????? ?��? ??? ???? ????? ?????????
						// ????? ?????? ???? ?????? AddQuestTemplet_LUA ??? ??????, ???? ???? ???? ????????? ???? ?????? ?????? ???? ???????.
	lua_tinker::class_def<CX2QuestManager>( L, "AddAccountQuestTemplet_LUA",			&CX2QuestManager::AddQuestTemplet_LUA );


	lua_tinker::class_def<CX2QuestManager>( L, "AddAccountQuestDate",					&CX2QuestManager::AddAccountQuestDate_LUA );

    // ?????? ????? ??????? proxy ?????? ???��?, robobeg
    CX2ItemManager::RegisterProxyLuabind( L );

	lua_tinker::class_add<CX2ItemManager>( L, "CX2ItemManager" );
	lua_tinker::class_def<CX2ItemManager>( L, "AddShopItemList_LUA",		&CX2ItemManager::AddShopItemList_LUA );
	lua_tinker::class_def<CX2ItemManager>( L, "AddShopItemGroup_LUA",		&CX2ItemManager::AddShopItemGroup_LUA );
	lua_tinker::class_def<CX2ItemManager>( L, "AddRandomItemTemplet",		&CX2ItemManager::AddRandomItemTemplet_LUA );
	lua_tinker::class_def<CX2ItemManager>( L, "AddManufactureTemplet",		&CX2ItemManager::AddManufactureTemplet_LUA );
	lua_tinker::class_def<CX2ItemManager>( L, "AddManufactureVillageData",	&CX2ItemManager::AddManufactureVillageData_LUA );
	lua_tinker::class_def<CX2ItemManager>( L, "AddManufactureResultGroup",	&CX2ItemManager::AddManufactureResultGroup );

	lua_tinker::class_def<CX2ItemManager>( L, "AddPetPreViewData",			&CX2ItemManager::AddPetPreViewData_LUA);

//{{ 2013.01.26 ???? ?????????????? ?????????
	lua_tinker::class_def<CX2ItemManager>(  L, "AddCubePackageData",			&CX2ItemManager::AddCube_PackageData_LUA );
//}}

	//{{ 2008. 4. 24  ??????
	lua_tinker::class_def<CX2ItemManager>( L, "AddPackageItemData",			&CX2ItemManager::AddPackageItemData_LUA);
	//}}

	//{{ 2008. 11. 18  ??????
	lua_tinker::class_def<CX2ItemManager>( L, "AddItemExchangeGroup",		&CX2ItemManager::AddItemExchangeGroup_LUA);
	//}}
	lua_tinker::class_def<CX2ItemManager>( L, "AddItemExchangeData",		&CX2ItemManager::AddItemExchangeData );

	lua_tinker::class_def<CX2ItemManager>( L, "AddCoolTimeGroupItem",		&CX2ItemManager::AddCoolTimeGroupItem_LUA );

	//{{ 2011. 10. 18	??????	?????? ?????? ???? ?????
	lua_tinker::class_def<CX2ItemManager>( L, "SetItemLevelEnchantData",	&CX2ItemManager::SetItemLevelEnchantData_LUA );
	//}}
	lua_tinker::class_def<CX2ItemManager>( L, "AddIceHeaterItemData",	&CX2ItemManager::AddIceHeaterItemData_LUA);

	lua_tinker::class_def<CX2ItemManager>( L, "AddItemConvertGroup", &CX2ItemManager::AddItemConvertGroup_LUA);
	lua_tinker::class_def<CX2ItemManager>( L, "AddItemConvertInfo",	&CX2ItemManager::AddItemConvertInfo_LUA);

	//{{ 2009. 6. 24  ??????	?????????
	lua_tinker::class_add<CX2RewardTable>( L, "CX2RewardTable" );
	lua_tinker::class_def<CX2RewardTable>( L, "AddRewardData",			&CX2RewardTable::AddRewardData_LUA );
	//}}

	//{{ 2011. 06. 22    ??��?    ??? ???? ?? - NEXON ???? ????? ????
	//}}
	//{{ 2011. 10.26    ??��?	��???? ?????? ????
	//}}

	lua_tinker::class_add<CX2UnitManager>( L, "CX2UnitManager" );
	lua_tinker::class_def<CX2UnitManager>( L, "AddUnitTemplet",			&CX2UnitManager::AddUnitTemplet_LUA );
	lua_tinker::class_def<CX2UnitManager>( L, "AddNPCTemplet",			&CX2UnitManager::AddNPCTemplet_LUA );
	lua_tinker::class_def<CX2UnitManager>( L, "AddNPCStat",				&CX2UnitManager::AddNPCStat_LUA );
	lua_tinker::class_def<CX2UnitManager>( L, "AddNPCExtraAbility_LUA", &CX2UnitManager::AddNPCExtraAbility_LUA );


	lua_tinker::class_add<CX2UIUnitManager>( L, "CX2UIUnitManager" );
	lua_tinker::class_def<CX2UIUnitManager>( L, "ClearAllUIUnit",		&CX2UIUnitManager::ClearAllUIUnit );


	lua_tinker::class_add<CX2WorldManager>( L, "CX2WorldManager" );
	lua_tinker::class_def<CX2WorldManager>( L, "AddWorldData",			&CX2WorldManager::AddWorldData_LUA );
	//{{ seojt // 2008-10-21, 15:20
	lua_tinker::class_def<CX2WorldManager>( L, "SetPreprocessingData",  &CX2WorldManager::SetPreprocessingData_LUA );
	//}} seojt // 2008-10-21, 15:20


	lua_tinker::class_add<CX2World>( L, "CX2World" );
	lua_tinker::class_def<CX2World>( L, "SetOriginColor_LUA",			&CX2World::SetOriginColor_LUA );
	lua_tinker::class_def<CX2World>( L, "SetLightColor_LUA",			&CX2World::SetLightColor_LUA );
	lua_tinker::class_def<CX2World>( L, "SetLightPos",					&CX2World::SetLightPos );
	lua_tinker::class_def<CX2World>( L, "SetLineMap",					&CX2World::SetLineMap_LUA );
	lua_tinker::class_def<CX2World>( L, "SetBGM",						&CX2World::SetBGM_LUA );
	lua_tinker::class_def<CX2World>( L, "PlayBGM",						&CX2World::PlayBGM );
	//{{08.12.15.hoons.
	lua_tinker::class_def<CX2World>( L, "StopBGM",						&CX2World::StopBGM );
	//}}
	lua_tinker::class_def<CX2World>( L, "AddEffectBGM_LUA",				&CX2World::AddEffectBGM_LUA );
	lua_tinker::class_def<CX2World>( L, "Add3DEffectBGM_LUA",			&CX2World::Add3DEffectBGM_LUA );

	lua_tinker::class_def<CX2World>( L, "CreateSkyDome",				&CX2World::CreateSkyDome );
	lua_tinker::class_def<CX2World>( L, "CreateObjectMesh",				&CX2World::CreateObjectMesh );
	lua_tinker::class_def<CX2World>( L, "CreateLensFlare",				&CX2World::CreateLensFlare );
	lua_tinker::class_def<CX2World>( L, "CreateObjectSkinMesh",			&CX2World::CreateObjectSkinMesh );
	lua_tinker::class_def<CX2World>( L, "CreateObjectLightFlow",		&CX2World::CreateObjectLightFlow );
	lua_tinker::class_def<CX2World>( L, "CreateObjectParticle",			&CX2World::CreateObjectParticle );
	lua_tinker::class_def<CX2World>( L, "CreateObjectParticleDelay",	&CX2World::CreateObjectParticleDelay );
    lua_tinker::class_def<CX2World>( L, "AddSyncObject",			    &CX2World::AddSyncObject );

    lua_tinker::class_def<CX2World>( L, "CreateWorldLayer",				&CX2World::CreateWorldLayer );
	lua_tinker::class_def<CX2World>( L, "SetWorldColor",				&CX2World::SetWorldColor );
	lua_tinker::class_def<CX2World>( L, "GetRenderLineMap",				&CX2World::GetRenderLineMap );
	lua_tinker::class_def<CX2World>( L, "SetRenderLineMap",				&CX2World::SetRenderLineMap );
    lua_tinker::class_def<CX2World>( L, "SetFogWorld",				    &CX2World::SetFogWorld );
	lua_tinker::class_def<CX2World>( L, "SetLookAt",				    &CX2World::SetLookAt );
	lua_tinker::class_def<CX2World>( L, "SetEye",						&CX2World::SetEye );
	lua_tinker::class_def<CX2World>( L, "SetSquareCameraSetting",		&CX2World::SetSquareCameraSetting );
	lua_tinker::class_def<CX2World>( L, "SetSquareCameraType",			&CX2World::SetSquareCameraType );
	lua_tinker::class_def<CX2World>( L, "SetGravityAndSpeedRate",		&CX2World::SetGravityAndSpeedRate );
	lua_tinker::class_def<CX2World>( L, "SetUnitAnimSpeedRate",			&CX2World::SetUnitAnimSpeedRate );
//#ifdef MARIO_LIKE_BLOCK_TEST
	lua_tinker::class_def<CX2World>( L, "AddWorldMonster_LUA",			&CX2World::AddWorldMonster_LUA );
//#endif MARIO_LIKE_BLOCK_TEST

	// oasis907 : ????? [2010.12.13] // ???? ??? ???? ?????? ???? ????
	lua_tinker::class_def<CX2World>( L, "SetWorldCamera",				&CX2World::SetWorldCamera_LUA );
	lua_tinker::class_def<CX2World>( L, "SetProjection",				&CX2World::SetProjection );
	lua_tinker::class_def<CX2World>( L, "SetTriggerCamera",				&CX2World::SetTriggerCamera );

	lua_tinker::class_add<CX2WorldObject>( L, "CX2WorldObject" );
	lua_tinker::class_inh<CX2WorldObject, CKTDGObject>( L );
	lua_tinker::class_def<CX2WorldObject>( L, "SetName",					&CX2WorldObject::SetName );
	lua_tinker::class_def<CX2WorldObject>( L, "SetPos",						&CX2WorldObject::SetPos );
	lua_tinker::class_def<CX2WorldObject>( L, "SetAngleDegree",				&CX2WorldObject::SetAngleDegree );
	lua_tinker::class_def<CX2WorldObject>( L, "SetColorFactor",				&CX2WorldObject::SetColorFactor );
	lua_tinker::class_def<CX2WorldObject>( L, "SetScale",					&CX2WorldObject::SetScale );
	lua_tinker::class_def<CX2WorldObject>( L, "SetUseWorldColor",			&CX2WorldObject::SetUseWorldColor );
	lua_tinker::class_def<CX2WorldObject>( L, "SetHideDistance",			&CX2WorldObject::SetHideDistance );
	lua_tinker::class_def<CX2WorldObject>( L, "SetHideNearDistance",		&CX2WorldObject::SetHideNearDistance );
	lua_tinker::class_def<CX2WorldObject>( L, "SetForceShow",				&CX2WorldObject::SetForceShow );

	lua_tinker::class_def<CX2WorldObject>( L, "SetOriginalColorFactor",		&CX2WorldObject::SetOriginalColorFactor );



    lua_tinker::class_add<CX2WorldLayer>( L, "CX2WorldLayer" );
    lua_tinker::class_inh<CX2WorldLayer, CX2WorldObject>(L);
    lua_tinker::class_def<CX2WorldLayer>( L, "SetTex",						&CX2WorldLayer::SetTex );
    lua_tinker::class_def<CX2WorldLayer>( L, "SetColor",					&CX2WorldLayer::SetColor );
    lua_tinker::class_def<CX2WorldLayer>( L, "SetPos",						&CX2WorldLayer::SetPos );
    lua_tinker::class_def<CX2WorldLayer>( L, "SetBlendType",				&CX2WorldLayer::SetBlendType );
    lua_tinker::class_def<CX2WorldLayer>( L, "SetTexUV",    				&CX2WorldLayer::SetTexUV );
    lua_tinker::class_def<CX2WorldLayer>( L, "SetFeedBackUV",    			&CX2WorldLayer::SetFeedBackUV );
	lua_tinker::class_def<CX2WorldLayer>( L, "SetLayerId",    				&CX2WorldLayer::SetLayerId );
	lua_tinker::class_def<CX2WorldLayer>( L, "SetInitShow",    				&CX2WorldLayer::SetInitShow );

	lua_tinker::class_add<CX2WorldObjectMesh>( L, "CX2WorldObjectMesh" );
	lua_tinker::class_inh<CX2WorldObjectMesh, CX2WorldObject>( L );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetXMesh",				&CX2WorldObjectMesh::SetXMesh_LUA );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetXMeshLOD",			&CX2WorldObjectMesh::SetXMeshLOD_LUA );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetMultiTexXET",			&CX2WorldObjectMesh::SetMultiTexXET_LUA );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetChangeTexXET",		&CX2WorldObjectMesh::SetChangeTexXET_LUA );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetCanHide",				&CX2WorldObjectMesh::SetCanHide );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetNotRenderWhenHidden",	&CX2WorldObjectMesh::SetNotRenderWhenHidden );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetBillboardType",		&CX2WorldObjectMesh::SetBillboardType );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "GetRenderParam",			&CX2WorldObjectMesh::GetRenderParam );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetTex0UVSpeed",			&CX2WorldObjectMesh::SetTex0UVSpeed );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetTex1UVSpeed",			&CX2WorldObjectMesh::SetTex1UVSpeed );
	lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetTex2UVSpeed",			&CX2WorldObjectMesh::SetTex2UVSpeed );
    lua_tinker::class_def<CX2WorldObjectMesh>( L, "SetFog",			        &CX2WorldObjectMesh::SetFog );


	lua_tinker::class_add<CX2WorldObjectSkinMesh>( L, "CX2WorldObjectSkinMesh" );
	lua_tinker::class_inh<CX2WorldObjectSkinMesh, CX2WorldObject>( L );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "PlayAnim",				&CX2WorldObjectSkinMesh::PlayAnim_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "AddAnim",				&CX2WorldObjectSkinMesh::AddAnim_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetAnimSpeed",			&CX2WorldObjectSkinMesh::SetAnimSpeed );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetAnimAniXET",			&CX2WorldObjectSkinMesh::SetAnimAniXET_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetXSkinMotion",			&CX2WorldObjectSkinMesh::SetXSkinMotion_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetMeshAniXET",			&CX2WorldObjectSkinMesh::SetMeshAniXET_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetMultiTexXET",			&CX2WorldObjectSkinMesh::SetMultiTexXET_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetChangeTexXET",		&CX2WorldObjectSkinMesh::SetChangeTexXET_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "AddXSkinModel",			&CX2WorldObjectSkinMesh::AddXSkinModel_LUA );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetCanHide",				&CX2WorldObjectSkinMesh::SetCanHide );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetBillboardType",		&CX2WorldObjectSkinMesh::SetBillboardType );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "GetRenderParam",			&CX2WorldObjectSkinMesh::GetRenderParam );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetOccasionalEventTime",	&CX2WorldObjectSkinMesh::SetOccasionalEventTime );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetOccasionalPlayRate",	&CX2WorldObjectSkinMesh::SetOccasionalPlayRate );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetTex0UVSpeed",			&CX2WorldObjectSkinMesh::SetTex0UVSpeed );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetTex1UVSpeed",			&CX2WorldObjectSkinMesh::SetTex1UVSpeed );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetTex2UVSpeed",			&CX2WorldObjectSkinMesh::SetTex2UVSpeed );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetFog",			        &CX2WorldObjectSkinMesh::SetFog );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "SetCartoonRender",		&CX2WorldObjectSkinMesh::SetCartoonRender );
	lua_tinker::class_def<CX2WorldObjectSkinMesh>( L, "PlayLocalInterval",		&CX2WorldObjectSkinMesh::SetAnimPlayLocalInterval );

	lua_tinker::class_add<CX2WorldObjectLightFlow>( L, "CX2WorldObjectLightFlow" );
	lua_tinker::class_inh<CX2WorldObjectLightFlow, CX2WorldObject>( L );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetXMesh",				&CX2WorldObjectLightFlow::SetXMesh_LUA );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetFlowMin",			&CX2WorldObjectLightFlow::SetFlowMin );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetFlowMax",			&CX2WorldObjectLightFlow::SetFlowMax );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetFlowSpeed",			&CX2WorldObjectLightFlow::SetFlowSpeed );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetWide",				&CX2WorldObjectLightFlow::SetWide );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetNowPoint",			&CX2WorldObjectLightFlow::SetNowPoint );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "SetRenderType",			&CX2WorldObjectLightFlow::SetRenderType );
	lua_tinker::class_def<CX2WorldObjectLightFlow>( L, "GetRenderParam",		&CX2WorldObjectLightFlow::GetRenderParam );


	lua_tinker::class_add<CX2WorldObjectParticle>( L, "CX2WorldObjectParticle" );
	lua_tinker::class_inh<CX2WorldObjectParticle, CX2WorldObject>( L );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetCanHide",				&CX2WorldObjectParticle::SetCanHide );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetParticlePos",			&CX2WorldObjectParticle::SetParticlePos );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetEmitMinMax",			&CX2WorldObjectParticle::SetEmitMinMax );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetTriggerCount",		&CX2WorldObjectParticle::SetTriggerCount );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetTriggerTime",			&CX2WorldObjectParticle::SetTriggerTime );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetDrawCount",			&CX2WorldObjectParticle::SetDrawCount );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetCullCheck",			&CX2WorldObjectParticle::SetCullCheck );
	lua_tinker::class_def<CX2WorldObjectParticle>( L, "SetParticleType",		&CX2WorldObjectParticle::SetParticleType );


	lua_tinker::class_add<CX2Dungeon>( L, "CX2Dungeon" );
	lua_tinker::class_def<CX2Dungeon>( L, "GetNowStage",						&CX2Dungeon::GetNowStage );


	lua_tinker::class_add<CX2DungeonStage>( L, "CX2DungeonStage" );
	lua_tinker::class_def<CX2DungeonStage>( L, "GetNowSubStage",				&CX2DungeonStage::GetNowSubStage );
	lua_tinker::class_def<CX2DungeonStage>( L, "StartGoEffect",					&CX2DungeonStage::StartGoEffect );
	lua_tinker::class_def<CX2DungeonStage>( L, "StopGoEffect",					&CX2DungeonStage::StopGoEffect );
	lua_tinker::class_def<CX2DungeonStage>( L, "IsGoEffectOn",					&CX2DungeonStage::IsGoEffectOn );


	lua_tinker::class_add<CX2DungeonSubStage>( L, "CX2DungeonSubStage" );
	lua_tinker::class_def<CX2DungeonSubStage>( L, "ClearSubStage",				&CX2DungeonSubStage::ClearSubStage );
	lua_tinker::class_def<CX2DungeonSubStage>( L, "GetWinWhenTimeOut",			&CX2DungeonSubStage::GetWinWhenTimeOut );
	lua_tinker::class_def<CX2DungeonSubStage>( L, "SetWinWhenTimeOut",			&CX2DungeonSubStage::SetWinWhenTimeOut );
	lua_tinker::class_def<CX2DungeonSubStage>( L, "GetStarted",					&CX2DungeonSubStage::GetStarted );
	lua_tinker::class_def<CX2DungeonSubStage>( L, "GetCheckedClearSubStage",	&CX2DungeonSubStage::GetCheckedClearSubStage );
	lua_tinker::class_def<CX2DungeonSubStage>( L, "GetDoneClearSubStage",		&CX2DungeonSubStage::GetDoneClearSubStage );



	lua_tinker::class_add<CX2DungeonManager>( L, "CX2DungeonManager" );
	lua_tinker::class_def<CX2DungeonManager>( L, "AddDungeonData",				&CX2DungeonManager::AddDungeonData_LUA );
	lua_tinker::class_def<CX2DungeonManager>( L, "AddDefaultRoomTitle_LUA",		&CX2DungeonManager::AddDefaultRoomTitle_LUA );





	lua_tinker::class_add<CX2Camera>( L, "CX2Camera" );
	lua_tinker::class_def<CX2Camera>( L, "NomalTrackingCamera_LUA",				&CX2Camera::NomalTrackingCamera_LUA );
	lua_tinker::class_def<CX2Camera>( L, "PartsLookTrackingCamera_LUA",			&CX2Camera::PartsLookTrackingCamera_LUA );
	lua_tinker::class_def<CX2Camera>( L, "NomalDirectCamera_LUA",				&CX2Camera::NomalDirectCamera_LUA );
	lua_tinker::class_def<CX2Camera>( L, "PartsLookDirectCamera_LUA",			&CX2Camera::PartsLookDirectCamera_LUA );
	lua_tinker::class_def<CX2Camera>( L, "PartsLookTrackingCameraByNpc_LUA",	&CX2Camera::PartsLookTrackingCameraByNpc_LUA );
	lua_tinker::class_def<CX2Camera>( L, "GetCamera",							&CX2Camera::GetCamera );

	lua_tinker::class_def<CX2Camera>( L, "AddWorldCameraData_LUA",				&CX2Camera::AddWorldCameraData_LUA );

	lua_tinker::class_add<CX2Game>( L, "CX2Game" );
	lua_tinker::class_def<CX2Game>( L, "GetGameType",							&CX2Game::GetGameType );
	lua_tinker::class_def<CX2Game>( L, "IsHost",								&CX2Game::IsHost );
	lua_tinker::class_def<CX2Game>( L, "GetMajorXMeshPlayer",					&CX2Game::GetMajorXMeshPlayer );
	lua_tinker::class_def<CX2Game>( L, "GetMinorXMeshPlayer",					&CX2Game::GetMinorXMeshPlayer );
	lua_tinker::class_def<CX2Game>( L, "GetMajorParticle",						&CX2Game::GetMajorParticle );
	lua_tinker::class_def<CX2Game>( L, "GetMinorParticle",						&CX2Game::GetMinorParticle );
	lua_tinker::class_def<CX2Game>( L, "GetDamageEffect",						&CX2Game::GetDamageEffect );
	lua_tinker::class_def<CX2Game>( L, "GetEffectSet",							&CX2Game::GetEffectSet );
	lua_tinker::class_def<CX2Game>( L, "GetX2Camera",							&CX2Game::GetX2Camera );
	lua_tinker::class_def<CX2Game>( L, "GetFocusUnitPos_LUA",					&CX2Game::GetFocusUnitPos_LUA );
	lua_tinker::class_def<CX2Game>( L, "LiveUserUnitNum",						&CX2Game::LiveUserUnitNum );
	lua_tinker::class_def<CX2Game>( L, "LiveActiveNPCNum",						&CX2Game::LiveActiveNPCNum );
	lua_tinker::class_def<CX2Game>( L, "LiveNPCNumType_LUA",					&CX2Game::LiveNPCNumType_LUA );
	lua_tinker::class_def<CX2Game>( L, "IsKillNPCKeyCode_LUA",					&CX2Game::IsKillNPCKeyCode_LUA );
	lua_tinker::class_def<CX2Game>( L, "LiveKeyCodeNPCNum",						&CX2Game::LiveKeyCodeNPCNum );
	lua_tinker::class_def<CX2Game>( L, "GetKeyCodeNPC",							&CX2Game::GetKeyCodeNPC );
	lua_tinker::class_def<CX2Game>( L, "SetWorldColor_LUA",						&CX2Game::SetWorldColor_LUA );
	lua_tinker::class_def<CX2Game>( L, "GetLineMap",							&CX2Game::GetLineMap );
	lua_tinker::class_def<CX2Game>( L, "CreateNPCReq_LUA",						&CX2Game::CreateNPCReq_LUA );
	lua_tinker::class_def<CX2Game>( L, "CreateNPCReq_LUA2",						&CX2Game::CreateNPCReq_LUA2 );
	lua_tinker::class_def<CX2Game>( L, "CreateNPCReqElite",						&CX2Game::CreateNPCReqElite );
	lua_tinker::class_def<CX2Game>( L, "CreateNPCReqAlly",						&CX2Game::CreateNPCReqAlly );
	lua_tinker::class_def<CX2Game>( L, "PushCreateNPCReq_Lua",					&CX2Game::PushCreateNPCReq_Lua );
	lua_tinker::class_def<CX2Game>( L, "FlushCreateNPCReq",						&CX2Game::FlushCreateNPCReq );
	lua_tinker::class_def<CX2Game>( L, "CreateNPCReq_LUA3",						&CX2Game::CreateNPCReq_LUA3 );
	lua_tinker::class_def<CX2Game>( L, "PushCreateNPCReq_LUA3",					&CX2Game::PushCreateNPCReq_LUA3 );
	lua_tinker::class_def<CX2Game>( L, "GetMyUnit",								&CX2Game::GetMyUnit );
	lua_tinker::class_def<CX2Game>( L, "IsKeyPressed",							&CX2Game::IsKeyPressed );
	lua_tinker::class_def<CX2Game>( L, "IsSkipKeyPressed",						&CX2Game::IsSkipKeyPressed );
	lua_tinker::class_def<CX2Game>( L, "GetGameTime",							&CX2Game::GetGameTime );
	lua_tinker::class_def<CX2Game>( L, "GetElapsedTimeAfterLastKeyboardInput",	&CX2Game::GetElapsedTimeAfterLastKeyboardInput );
	lua_tinker::class_def<CX2Game>( L, "PauseNPCAI",							&CX2Game::PauseNPCAI );
	lua_tinker::class_def<CX2Game>( L, "KillAllNPC_LUA",						&CX2Game::KillAllNPC_LUA );
	lua_tinker::class_def<CX2Game>( L, "KillNPC",								&CX2Game::KillNPC );
	lua_tinker::class_def<CX2Game>( L, "EnableAllNPCAI",						&CX2Game::EnableAllNPCAI );
	lua_tinker::class_def<CX2Game>( L, "SetFreeCamera",							&CX2Game::SetFreeCamera);
	lua_tinker::class_def<CX2Game>( L, "GoStage",								&CX2Game::GoStage);
	lua_tinker::class_def<CX2Game>( L, "GetWorld",								&CX2Game::GetWorld);
	lua_tinker::class_def<CX2Game>( L, "SetShowAllUnitGageBar",					&CX2Game::SetShowAllUnitGageBar);
	lua_tinker::class_def<CX2Game>( L, "GetUnitNum",							&CX2Game::GetUnitNum);
	lua_tinker::class_def<CX2Game>( L, "GetUnit",								&CX2Game::GetUnit);
	lua_tinker::class_def<CX2Game>( L, "GetUserUnitListSize",					&CX2Game::GetUserUnitListSize);
	lua_tinker::class_def<CX2Game>( L, "GetUserUnit",							&CX2Game::GetUserUnit);
	lua_tinker::class_def<CX2Game>( L, "GetUserUnitByUID",						&CX2Game::GetUserUnitByUID);
	lua_tinker::class_def<CX2Game>( L, "GetNPCUnitListSize",					&CX2Game::GetNPCUnitListSize);
	lua_tinker::class_def<CX2Game>( L, "GetNPCUnit",							&CX2Game::GetNPCUnit);
    lua_tinker::class_def<CX2Game>( L, "GetNPCUnitByUID",						&CX2Game::GetNPCUnitByUID);
	lua_tinker::class_def<CX2Game>( L, "GetNPCUnitByKeyCode",					&CX2Game::GetNPCUnitByKeyCode);
	lua_tinker::class_def<CX2Game>( L, "GetNPCUnitByType",						&CX2Game::GetNPCUnitByType);
	lua_tinker::class_def<CX2Game>( L, "GetGameLevel",							&CX2Game::GetGameLevel);
	lua_tinker::class_def<CX2Game>( L, "GetDungeonType",						&CX2Game::GetDungeonType );
	lua_tinker::class_def<CX2Game>( L, "GetSlideShot",							&CX2Game::GetSlideShot);
	lua_tinker::class_def<CX2Game>( L, "SetEnableAllNPCConditionTable",			&CX2Game::SetEnableAllNPCConditionTable);
	lua_tinker::class_def<CX2Game>( L, "EnableAllNPCEventProcess",				&CX2Game::EnableAllNPCEventProcess);
	lua_tinker::class_def<CX2Game>( L, "SetEnableAllKeyProcess",				&CX2Game::SetEnableAllKeyProcess);
	lua_tinker::class_def<CX2Game>( L, "SetEnableCommandKeyProcess",			&CX2Game::SetEnableCommandKeyProcess);
	lua_tinker::class_def<CX2Game>( L, "GetDropItemManager",					&CX2Game::GetDropItemManager);
	lua_tinker::class_def<CX2Game>( L, "GetNPCStartPos",						&CX2Game::GetNPCStartPos);
	lua_tinker::class_def<CX2Game>( L, "AnyUserUnitInRange",					&CX2Game::AnyUserUnitInRange);
	lua_tinker::class_def<CX2Game>( L, "GetNearNPCUnitUIDInStart_LUA",			&CX2Game::GetNearNPCUnitUIDInStart_LUA);
	lua_tinker::class_def<CX2Game>( L, "GetNearNPCUnitInStart_LUA",				&CX2Game::GetNearNPCUnitInStart );
	lua_tinker::class_def<CX2Game>( L, "GetNearNPCUnitUID_LUA",	        		&CX2Game::GetNearNPCUnitUID_LUA);
	lua_tinker::class_def<CX2Game>( L, "GetNearestNpcInSpecificRangeByNpcId_LUA",	&CX2Game::GetNearestNpcInSpecificRangeByNpcId);

	lua_tinker::class_def<CX2Game>( L, "GetNearestUserUnit_LUA",	       		&CX2Game::GetNearestUserUnit );

//{{oasis:?????////2009-10-7////
	lua_tinker::class_def<CX2Game>( L, "IsNearNPCUnitUID_LUA",					&CX2Game::IsNearNPCUnitUID_LUA);
	lua_tinker::class_def<CX2Game>( L, "GetNearestUserUnitUID_LUA",				&CX2Game::GetNearestUserUnitUID_LUA );
	lua_tinker::class_def<CX2Game>( L, "GetFixedDistancePosition_LUA",			&CX2Game::GetFixedDistancePosition_LUA );
	lua_tinker::class_def<CX2Game>( L, "GetDist_LUA",							&CX2Game::GetDist_LUA );
	lua_tinker::class_def<CX2Game>( L, "GetNearestUserUnitPos_LUA",				&CX2Game::GetNearestUserUnitPos_LUA );
//}}oasis:?????////2009-10-7////
	lua_tinker::class_def<CX2Game>( L, "GetNearestNPCUnitPos_LUA",				&CX2Game::GetNearestNPCUnitPos_LUA );
	//{{ oasis907 : ????? [2010.7.30] //
	lua_tinker::class_def<CX2Game>( L, "GetFarthestUserUnitPos_LUA",			&CX2Game::GetFarthestUserUnitPos_LUA );
	lua_tinker::class_def<CX2Game>( L, "GetUserUnitNum_LUA",					&CX2Game::GetUserUnitNum );
	//}}
	lua_tinker::class_def<CX2Game>( L, "ChangeRebirthStateNPCUnit_LUA",			&CX2Game::ChangeRebirthStateNPCUnit_LUA);
	lua_tinker::class_def<CX2Game>( L, "GetEmptyStartPos",						&CX2Game::GetEmptyStartPos);
	lua_tinker::class_def<CX2Game>( L, "SetAllNPCMPFull",						&CX2Game::SetAllNPCMPFull);
	lua_tinker::class_def<CX2Game>( L, "SetLastKillCheck",						&CX2Game::SetLastKillCheck);
	lua_tinker::class_def<CX2Game>( L, "SetLastkillShotTime",					&CX2Game::SetLastkillShotTime);
	lua_tinker::class_def<CX2Game>( L, "BuffAllNPC",							&CX2Game::BuffAllNPC);
	lua_tinker::class_def<CX2Game>( L, "GetEnableMonsterFly",					&CX2Game::GetEnableMonsterFly);

	lua_tinker::class_def<CX2Game>( L, "Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA",	&CX2Game::Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA );

#if X2_DEV_TOOLS
	lua_tinker::class_def<CX2Game>( L, "EnableManualCameraPosition",			&CX2Game::EnableManualCameraPosition);
#endif // IN_GAME_MANUAL_CAMERA_POSITION_TEST

	// oasis907 : ????? [2010.12.13] // ???? ??? ???? ?????? ???? ????
	lua_tinker::class_def<CX2Game>( L, "PlayWorldCamera",						&CX2Game::PlayWorldCamera );

	lua_tinker::class_def<CX2Game>( L, "GetNearestUserUnitInSpecificRange_LUA",		&CX2Game::GetNearestUserUnitInSpecificRange );
	//{{ ????? : [2011/3/4/] //	???? ???
	lua_tinker::class_def<CX2Game>( L, "GetNPCUnitNum",							&CX2Game::GetNPCUnitNum );
	lua_tinker::class_def<CX2Game>( L, "GetNPCUnitNumByNPCID",					&CX2Game::GetNPCUnitNumByNPCID );
	lua_tinker::class_def<CX2Game>( L, "rand",									&CX2Game::GetRandomValue );

	lua_tinker::class_def<CX2Game>( L, "GetDungeonProgress",					&CX2Game::GetDungeonProgress );
	//}} ????? : [2011/3/4/] //	???? ???

	lua_tinker::class_def<CX2Game>( L, "GetEliteKeyCodeNPCUnitNum",				&CX2Game::GetEliteKeyCodeNPCUnitNum );

	lua_tinker::class_def<CX2Game>( L, "GetFarthestUserUnitUID_LUA",			&CX2Game::GetFarthestUserUnitUID_LUA );
	lua_tinker::class_def<CX2Game>( L, "AddReverseGravityRegion",				&CX2Game::AddReverseGravityRegion );

	lua_tinker::class_def<CX2Game>( L, "GetStartSecretStageEnteringEvent",		&CX2Game::GetStartSecretStageEnteringEvent );
	lua_tinker::class_def<CX2Game>( L, "SecretDungeonStageLoadReq",				&CX2Game::SecretDungeonStageLoadReq );

	lua_tinker::class_def<CX2Game>( L, "GetNowStageIndex",	&CX2Game::GetNowStageIndex);
	lua_tinker::class_def<CX2Game>( L, "ShowDangerAlarm_LUA",	&CX2Game::ShowDangerAlarm_LUA);
	lua_tinker::class_def<CX2Game>( L, "GetNearestUnit_LUA",	&CX2Game::GetNearestUnit);

	lua_tinker::class_def<CX2Game>( L, "Send_EGS_DEFENCE_DUNGEON_WAVE_ID_NOT",	&CX2Game::Send_EGS_DEFENCE_DUNGEON_WAVE_ID_NOT);


	lua_tinker::class_def<CX2Game>( L, "IsEnableCodeEvent_LUA",		&CX2Game::IsEnableCodeEvent_LUA );

	lua_tinker::class_def<CX2Game>( L, "GetDungeonID",		&CX2Game::GetDungeonID );




	/// ???? ???? ???? ??? ???? ???? NPC ???
	lua_tinker::class_def<CX2Game>( L, "LiveKeyCodeNPCNumFreeTeam",	&CX2Game::LiveKeyCodeNPCNumFreeTeam );


	lua_tinker::class_def<CX2Game>( L, "GetNearestNPCUnitPosOnSameDirection_LUA",	&CX2Game::GetNearestNPCUnitPosOnSameDirection_LUA );
	lua_tinker::class_def<CX2Game>( L, "GetNearestNPCUnitOnSameDirection_LUA",		&CX2Game::GetNearestNPCUnitOnSameDirection_LUA );

	lua_tinker::class_def<CX2Game>( L, "GetNowSubStageIndex",	&CX2Game::GetNowSubStageIndex);

	// ?????? ???? ???? UnitUID ?? ?????? ???? ????? NPC ?? ??????
	// ??????, ????��? ??? NPC ???? ???, kimjh
	lua_tinker::class_def<CX2Game>( L, "GetNearestNpcInSpecificRangeAndExceptUnitUID_LUA",				&CX2Game::GetNearestNpcInSpecificRangeAndExceptUnitUID );


	lua_tinker::class_def<CX2Game>( L, "ChatBoxLog",				&CX2Game::ChatBoxLog );


	lua_tinker::class_add<CX2DungeonGame>( L, "CX2DungeonGame" );
	lua_tinker::class_inh<CX2DungeonGame, CX2Game>( L );
	lua_tinker::class_def<CX2DungeonGame>( L, "TextOutTutorial",					&CX2DungeonGame::TextOutTutorial_LUA );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetIsThereTutorialMessage",			&CX2DungeonGame::GetIsThereTutorialMessage );
	lua_tinker::class_def<CX2DungeonGame>( L, "IsTutorialMessage",					&CX2DungeonGame::IsTutorialMessage_LUA );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetDungeon",							&CX2DungeonGame::GetDungeon );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetGameLevel",						&CX2DungeonGame::GetGameLevel );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetDungeonType",						&CX2DungeonGame::GetDungeonType );
	lua_tinker::class_def<CX2DungeonGame>( L, "ClearSubStage",						&CX2DungeonGame::ClearSubStage );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetStartRealTraining",				&CX2DungeonGame::GetStartRealTraining );
	lua_tinker::class_def<CX2DungeonGame>( L, "SetStartRealTraining",				&CX2DungeonGame::SetStartRealTraining );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetRemainPlayTime",					&CX2DungeonGame::GetRemainPlayTime );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetTrainingGameUI",					&CX2DungeonGame::GetTrainingGameUI );
	lua_tinker::class_def<CX2DungeonGame>( L, "SetRemainPlayTime",					&CX2DungeonGame::SetRemainPlayTime );
	lua_tinker::class_def<CX2DungeonGame>( L, "ShowBossName_LUA",					&CX2DungeonGame::ShowBossName_LUA );
	lua_tinker::class_def<CX2DungeonGame>( L, "ResetUnitPositionAtTrainingGame",	&CX2DungeonGame::ResetUnitPositionAtTrainingGame );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetTimerManyPurpose1",				&CX2DungeonGame::GetTimerManyPurpose1 );
	lua_tinker::class_def<CX2DungeonGame>( L, "GetTimerManyPurpose2",				&CX2DungeonGame::GetTimerManyPurpose2 );
	lua_tinker::class_def<CX2DungeonGame>( L, "SetClearEffect",						&CX2DungeonGame::SetClearEffect );



	lua_tinker::class_add<CX2SquareGame>( L, "CX2SquareGame" );
	lua_tinker::class_def<CX2SquareGame>( L, "SetFreeCamera",						&CX2SquareGame::SetFreeCamera );


	lua_tinker::class_add<CX2GameUnit>( L, "CX2GameUnit" );
	lua_tinker::class_def<CX2GameUnit>( L, "PlaySound_LUA",				&CX2GameUnit::PlaySound_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "StopSound_LUA",				&CX2GameUnit::StopSound_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetPos",					&CX2GameUnit::GetPos );
	lua_tinker::class_def<CX2GameUnit>( L, "ClearHitUnitList_LUA",		&CX2GameUnit::ClearHitUnitList_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "SetSpeedX",					&CX2GameUnit::SetSpeedX );
	lua_tinker::class_def<CX2GameUnit>( L, "SetSpeedY",					&CX2GameUnit::SetSpeedY );
	lua_tinker::class_def<CX2GameUnit>( L, "SetDamageData_LUA",			&CX2GameUnit::SetDamageData_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetNowHP",					&CX2GameUnit::GetNowHp );
	lua_tinker::class_def<CX2GameUnit>( L, "GetMaxHP",					&CX2GameUnit::GetMaxHp );
	lua_tinker::class_def<CX2GameUnit>( L, "SetNowHP",					&CX2GameUnit::SetNowHp );
	lua_tinker::class_def<CX2GameUnit>( L, "GetNowMP",					&CX2GameUnit::GetNowMp );
	lua_tinker::class_def<CX2GameUnit>( L, "SetNowMP",					&CX2GameUnit::SetNowMp );
	lua_tinker::class_def<CX2GameUnit>( L, "GetMaxMP",					&CX2GameUnit::GetMaxMp );
	lua_tinker::class_def<CX2GameUnit>( L, "SetColor",					&CX2GameUnit::SetColor );
	lua_tinker::class_def<CX2GameUnit>( L, "GetColor",					&CX2GameUnit::GetColor );
	lua_tinker::class_def<CX2GameUnit>( L, "GetImpactPoint_LUA",		&CX2GameUnit::GetImpactPoint_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetAttackerUnit_LUA",		&CX2GameUnit::GetAttackerUnit_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetDistanceFrom",			&CX2GameUnit::GetDistanceFrom );
	lua_tinker::class_def<CX2GameUnit>( L, "GetXDistanceFrom",			&CX2GameUnit::GetXDistanceFrom );
	lua_tinker::class_def<CX2GameUnit>( L, "GetWalkSpeed",				&CX2GameUnit::GetWalkSpeed );
	lua_tinker::class_def<CX2GameUnit>( L, "SetShowGage",				&CX2GameUnit::SetShowGage );
	lua_tinker::class_def<CX2GameUnit>( L, "SetCanNotInputTime_LUA",	&CX2GameUnit::SetCanNotInputTime_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "SetUseWorldColor",			&CX2GameUnit::SetUseWorldColor );
	lua_tinker::class_def<CX2GameUnit>( L, "GetDirVector",				&CX2GameUnit::GetDirVector );
	lua_tinker::class_def<CX2GameUnit>( L, "GetTeam",					&CX2GameUnit::GetTeam );
	lua_tinker::class_def<CX2GameUnit>( L, "MultiplyStat",				&CX2GameUnit::MultiplyStat );
	lua_tinker::class_def<CX2GameUnit>( L, "SetDamageReducePercent",	&CX2GameUnit::SetDamageReducePercent );
	//lua_tinker::class_def<CX2GameUnit>( L, "TwinkleMPGageBar",			&CX2GameUnit::TwinkleMPGageBar );
	lua_tinker::class_def<CX2GameUnit>( L, "SetMajorMeshPlayer_LUA",	&CX2GameUnit::SetMajorMeshPlayer_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "ClearMajorMeshPlayer_LUA",	&CX2GameUnit::ClearMajorMeshPlayer_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetMajorMeshPlayer_LUA",	&CX2GameUnit::GetMajorMeshPlayer_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetMajorMeshPlayerHandle",	&CX2GameUnit::GetMajorMeshPlayerHandle );
	lua_tinker::class_def<CX2GameUnit>( L, "SetMinorMeshPlayer_LUA",	&CX2GameUnit::SetMinorMeshPlayer_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "ClearMinorMeshPlayer_LUA",	&CX2GameUnit::ClearMinorMeshPlayer_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetMinorMeshPlayer_LUA",	&CX2GameUnit::GetMinorMeshPlayer_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "GetMinorMeshPlayerHandle",	&CX2GameUnit::GetMinorMeshPlayerHandle );
	lua_tinker::class_def<CX2GameUnit>( L, "AddOnStatReq",				&CX2GameUnit::AddOnStatReq );
	lua_tinker::class_def<CX2GameUnit>( L, "GetWeaponBonePos_LUA",		&CX2GameUnit::GetWeaponBonePos_LUA);
	lua_tinker::class_def<CX2GameUnit>( L, "SetWeaponEnable",			&CX2GameUnit::SetWeaponEnable);
	lua_tinker::class_def<CX2GameUnit>( L, "AttachWeaponParticle_LUA",	&CX2GameUnit::AttachWeaponParticle_LUA);
	lua_tinker::class_def<CX2GameUnit>( L, "GetExtraDamagePack",		&CX2GameUnit::GetExtraDamagePack);
	lua_tinker::class_def<CX2GameUnit>( L, "SetExtraDamagePack",		&CX2GameUnit::SetExtraDamagePack);
	lua_tinker::class_def<CX2GameUnit>( L, "GetGameUnitType",			&CX2GameUnit::GetGameUnitType);
	lua_tinker::class_def<CX2GameUnit>( L, "SetExtraDamageDataTime",	&CX2GameUnit::SetExtraDamageDataTime);
	lua_tinker::class_def<CX2GameUnit>( L, "DeleteEffectSetOnDie",		&CX2GameUnit::DeleteEffectSetOnDie);
	lua_tinker::class_def<CX2GameUnit>( L, "SetLockOnNearTarget_LUA",	&CX2GameUnit::SetLockOnNearTarget_LUA);
	//{{ JHKang / ?????? / 2010/12/07 / ?�O?? ???? ????? ??? ???? ?? ????? ???? ????
	lua_tinker::class_def<CX2GameUnit>( L, "GetScaleByUnit",			&CX2GameUnit::GetVec3ScaleByUnit);
	lua_tinker::class_def<CX2GameUnit>( L, "SetShieldDamageReduce",		&CX2GameUnit::SetShieldDamageReduce);
	//}} JHKang / ?????? / 2010/12/07 / ?�O?? ???? ????? ??? ???? ?? ????? ???? ????

	lua_tinker::class_def<CX2GameUnit>( L, "SetRemainHyperModeTime",	&CX2GameUnit::SetRemainHyperModeTime);
	lua_tinker::class_def<CX2GameUnit>( L, "GetTimeToReturnBasicScale_LUA",		&CX2GameUnit::GetTimeToReturnBasicScale);


	lua_tinker::class_def<CX2GameUnit>( L, "PulseWeaponRenderEffect",	&CX2GameUnit::PulseWeaponRenderEffect);
	lua_tinker::class_def<CX2GameUnit>( L, "SetEnableWeaponRenderEffect",	&CX2GameUnit::SetEnableWeaponRenderEffect);

	lua_tinker::class_def<CX2GameUnit>( L, "GetAllyTeam",				&CX2GameUnit::GetAllyTeam );
	lua_tinker::class_def<CX2GameUnit>( L, "GetUnitUID",				&CX2GameUnit::GetUnitUID );

	lua_tinker::class_def<CX2GameUnit>( L, "SetBuffFactorToGameUnitByBuffFactorID_LUA",
		&CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorID );

	lua_tinker::class_def<CX2GameUnit>( L, "SetBuffFactorToGameUnitByBuffFactorIDAndCreatorID_LUA",
		&CX2GameUnit::SetBuffFactorToGameUnitByBuffFactorIDAndCreatorID );

	lua_tinker::class_def<CX2GameUnit>( L, "EraseBuffTempletFromGameUnit_LUA",
		&CX2GameUnit::EraseBuffTempletFromGameUnit );

	lua_tinker::class_def<CX2GameUnit>( L, "SetBuffFactorToDamageData",
		&CX2GameUnit::SetBuffFactorToDamageData );

	lua_tinker::class_def<CX2GameUnit>( L, "TransformScale_LUA",			&CX2GameUnit::TransformScale_LUA );
	lua_tinker::class_def<CX2GameUnit>( L, "IsChangedGameUnitScaleByBuff",	&CX2GameUnit::IsChangedGameUnitScaleByBuff );

	lua_tinker::class_def<CX2GameUnit>( L, "GetChangeUnitScaleByBuff",	&CX2GameUnit::GetChangeUnitScaleByBuff );

	lua_tinker::class_def<CX2GameUnit>( L, "WriteTraceLog_LUA",			&CX2GameUnit::WriteTraceLog_LUA );

	lua_tinker::class_def<CX2GameUnit>( L, "ShowIncreaseValue",			&CX2GameUnit::ShowIncreaseValue );

	lua_tinker::class_def<CX2GameUnit>( L, "HaveThisBuff",				&CX2GameUnit::HaveThisBuff );

#if X2_DEV_TOOLS // ?????
	lua_tinker::class_def<CX2GameUnit>( L, "WriteLogByConsoleWindow",	&CX2GameUnit::WriteLogByConsoleWindow_LUA );
#endif // WRITE_SCRIPT_LOG_AT_CONSOLE_WINDOW


	//{{ GUNPC Start
	lua_tinker::class_add<CX2GUNPC>( L, "CX2GUNPC" );
	lua_tinker::class_inh<CX2GUNPC, CX2GameUnit>( L );
	lua_tinker::class_def<CX2GUNPC>( L, "AnimEventTimer_LUA",			&CX2GUNPC::AnimEventTimer_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "AnimTimer_LUA",				&CX2GUNPC::AnimTimer_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNowAnimTime_LUA",			&CX2GUNPC::GetNowAnimTime_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "EventTimer",					&CX2GUNPC::EventTimer);

	lua_tinker::class_def<CX2GUNPC>( L, "GetLandPosition_LUA",			&CX2GUNPC::GetLandPosition_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetPosition",					&CX2GUNPC::SetPosition);
	lua_tinker::class_def<CX2GUNPC>( L, "GetPos",						&CX2GUNPC::GetPos);
	lua_tinker::class_def<CX2GUNPC>( L, "GetRotateDegree",				&CX2GUNPC::GetRotateDegree);
	lua_tinker::class_def<CX2GUNPC>( L, "GetXRotateDegree",				&CX2GUNPC::GetXRotateDegree);
	lua_tinker::class_def<CX2GUNPC>( L, "GetYRotateDegree",				&CX2GUNPC::GetYRotateDegree);
	lua_tinker::class_def<CX2GUNPC>( L, "GetZRotateDegree",				&CX2GUNPC::GetZRotateDegree);
    lua_tinker::class_def<CX2GUNPC>( L, "SetIsRight",					&CX2GUNPC::SetIsRightNow);
	lua_tinker::class_def<CX2GUNPC>( L, "GetIsRight",					&CX2GUNPC::GetIsRight);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDirPos_LUA",				&CX2GUNPC::GetDirPos_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetAddRotateDegree_LUA",		&CX2GUNPC::SetAddRotateDegree_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetXSkinAnim",					&CX2GUNPC::GetXSkinAnim_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetBonePos_LUA",				&CX2GUNPC::GetBonePos_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetUnitCondition_bHit",		&CX2GUNPC::GetUnitCondition_bHit);
	lua_tinker::class_def<CX2GUNPC>( L, "SetKeyCode",					&CX2GUNPC::SetKeyCode);
	lua_tinker::class_def<CX2GUNPC>( L, "GetKeyCode",					&CX2GUNPC::GetKeyCode);
	lua_tinker::class_def<CX2GUNPC>( L, "SetFlag_LUA",					&CX2GUNPC::SetFlag_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetFlag_LUA",					&CX2GUNPC::GetFlag_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetInt_LUA",					&CX2GUNPC::SetInt_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetInt_LUA",					&CX2GUNPC::GetInt_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetVector_LUA",				&CX2GUNPC::SetVector_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetVector_LUA",				&CX2GUNPC::GetVector_LUA);
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
	lua_tinker::class_def<CX2GUNPC>( L, "SetNumber_LUA",				&CX2GUNPC::SetNumber_LUA );
	lua_tinker::class_def<CX2GUNPC>( L, "GetNumber_LUA",				&CX2GUNPC::GetNumber_LUA );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
	lua_tinker::class_def<CX2GUNPC>( L, "SetAIEnable",					&CX2GUNPC::SetAIEnable);
	lua_tinker::class_def<CX2GUNPC>( L, "SetTimePauseNPCAI",			&CX2GUNPC::SetTimePauseNPCAI);
	lua_tinker::class_def<CX2GUNPC>( L, "SetMajorParticle_LUA",			&CX2GUNPC::SetMajorParticle_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetMinorParticle_LUA",			&CX2GUNPC::SetMinorParticle_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetMajorParticle_LUA",			&CX2GUNPC::GetMajorParticle_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetMinorParticle_LUA",			&CX2GUNPC::GetMinorParticle_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "ClearMinorParticle_LUA",		&CX2GUNPC::ClearMinorParticle_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "ClearMajorParticle_LUA",		&CX2GUNPC::ClearMajorParticle_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDamageEffect_LUA",			&CX2GUNPC::GetDamageEffect_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetDamageEffect_LUA",			&CX2GUNPC::SetDamageEffect_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "ClearDamageEffect_LUA",		&CX2GUNPC::ClearDamageEffect_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetInvincibleTime",			&CX2GUNPC::SetInvincibleTime);
	lua_tinker::class_def<CX2GUNPC>( L, "SetSuperArmor",				&CX2GUNPC::SetSuperArmorStateParam);
	lua_tinker::class_def<CX2GUNPC>( L, "CallOtherNPC",					&CX2GUNPC::CallOtherNPC);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNowAnimationTime",			&CX2GUNPC::GetNowAnimationTime);
	lua_tinker::class_def<CX2GUNPC>( L, "SetPassiveSpeed",				&CX2GUNPC::SetPassiveSpeed);
	lua_tinker::class_def<CX2GUNPC>( L, "SetSpeedX",					&CX2GUNPC::SetSpeedX);
	lua_tinker::class_def<CX2GUNPC>( L, "SetSpeedY",					&CX2GUNPC::SetSpeedY);
	lua_tinker::class_def<CX2GUNPC>( L, "GetSpeedY",					&CX2GUNPC::GetSpeedY);
	lua_tinker::class_def<CX2GUNPC>( L, "GetSpeedX",					&CX2GUNPC::GetSpeedX);
	lua_tinker::class_def<CX2GUNPC>( L, "SetGAccel",					&CX2GUNPC::SetGAccel);
	lua_tinker::class_def<CX2GUNPC>( L, "SetReloadAccel",				&CX2GUNPC::SetReloadAccel);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDamagedThisFrame",			&CX2GUNPC::GetDamagedThisFrame);
	lua_tinker::class_def<CX2GUNPC>( L, "HasTarget",					&CX2GUNPC::HasTarget);
	lua_tinker::class_def<CX2GUNPC>( L, "SetDieSeq",					&CX2GUNPC::SetDieSeq);
	lua_tinker::class_def<CX2GUNPC>( L, "GetHardLevel",					&CX2GUNPC::GetHardLevel);
	lua_tinker::class_def<CX2GUNPC>( L, "GetBoneRotateDegree_LUA",		&CX2GUNPC::GetBoneRotateDegree_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetBoneScale_LUA",				&CX2GUNPC::SetBoneScale_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetHyperModeCount",			&CX2GUNPC::GetHyperModeCount);
	lua_tinker::class_def<CX2GUNPC>( L, "ChangeHyperMode_LUA",			&CX2GUNPC::ChangeHyperMode_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetLockOnTarget",				&CX2GUNPC::SetLockOnTarget);
	lua_tinker::class_def<CX2GUNPC>( L, "SetLockOnNearTarget",			&CX2GUNPC::SetLockOnNearTarget);
	lua_tinker::class_def<CX2GUNPC>( L, "SetLockOn",					&CX2GUNPC::SetLockOn);
	lua_tinker::class_def<CX2GUNPC>( L, "SetLockOnPos",					&CX2GUNPC::SetLockOnPos);
//2013-08-29??? : <?????> uid?? ??????? ?????? ???????? ??? ???. ??? ??????? ????.
	lua_tinker::class_def<CX2GUNPC>( L, "SetLockOnUnitUIDFix",			&CX2GUNPC::SetLockOnUnitUIDFix);

	lua_tinker::class_def<CX2GUNPC>( L, "SetSiegeMode",					&CX2GUNPC::SetSiegeMode);
	lua_tinker::class_def<CX2GUNPC>( L, "ReleaseSiegeMode",				&CX2GUNPC::ReleaseSiegeMode);
	lua_tinker::class_def<CX2GUNPC>( L, "GetUID",						&CX2GUNPC::GetUID);
	lua_tinker::class_def<CX2GUNPC>( L, "SetTimerRestart",				&CX2GUNPC::SetTimerRestart);
	lua_tinker::class_def<CX2GUNPC>( L, "GetTimerElapsedTime",			&CX2GUNPC::GetTimerElapsedTime);
	lua_tinker::class_def<CX2GUNPC>( L, "SetTimerResume",				&CX2GUNPC::SetTimerResume);
    lua_tinker::class_def<CX2GUNPC>( L, "SetTimerPause",				&CX2GUNPC::SetTimerPause);
    lua_tinker::class_def<CX2GUNPC>( L, "GetTimerResumed",				&CX2GUNPC::GetTimerResumed);
    lua_tinker::class_def<CX2GUNPC>( L, "TimerEventTrigger",			&CX2GUNPC::TimerEventTrigger);
    lua_tinker::class_def<CX2GUNPC>( L, "StateEventInterval_LUA",		&CX2GUNPC::StateEventInterval_LUA);
    lua_tinker::class_def<CX2GUNPC>( L, "GetMotionEnd",		            &CX2GUNPC::GetMotionEnd);
	lua_tinker::class_def<CX2GUNPC>( L, "GetFollowHitter",				&CX2GUNPC::GetFollowHitter);
	lua_tinker::class_def<CX2GUNPC>( L, "GetFollowHitterLost",			&CX2GUNPC::GetFollowHitterLost);
	lua_tinker::class_def<CX2GUNPC>( L, "GetLineGroupStartPos",			&CX2GUNPC::GetLineGroupStartPos);
	lua_tinker::class_def<CX2GUNPC>( L, "GetLineGroupEndPos",			&CX2GUNPC::GetLineGroupEndPos);
	lua_tinker::class_def<CX2GUNPC>( L, "GetStateTime",					&CX2GUNPC::GetStateTime);
	lua_tinker::class_def<CX2GUNPC>( L, "GetZVector",					&CX2GUNPC::GetZVector);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNowHP_LUA",					&CX2GUNPC::GetNowHP_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetNowHP_LUA",					&CX2GUNPC::SetNowHP_LUA);
//{{ robobeg : 2008-10-21
	//lua_tinker::class_def<CX2GUNPC>( L, "SetShow",						&CX2GUNPC::SetShow);
    lua_tinker::class_def<CX2GUNPC>( L, "SetShow",						&CX2GUNPC::SetShowObject);
//}} robobeg : 2008-10-21
	lua_tinker::class_def<CX2GUNPC>( L, "GetAITargetPos",				&CX2GUNPC::GetAITargetPos);
	lua_tinker::class_def<CX2GUNPC>( L, "SetEnableLuaTargetingFunc",	&CX2GUNPC::SetEnableLuaTargetingFunc);
	lua_tinker::class_def<CX2GUNPC>( L, "SetLuaTargetingFunc",			&CX2GUNPC::SetLuaTargetingFunc);
	lua_tinker::class_def<CX2GUNPC>( L, "SetTargetUnit",				&CX2GUNPC::SetTargetUnit);
	lua_tinker::class_def<CX2GUNPC>( L, "SetTargetUnitUID",				&CX2GUNPC::SetTargetUnitUID);
	lua_tinker::class_def<CX2GUNPC>( L, "SetTargetUnitType",			&CX2GUNPC::SetTargetUnitType);
	lua_tinker::class_def<CX2GUNPC>( L, "GetTargetUnitUID",				&CX2GUNPC::GetTargetUnitUID);
	lua_tinker::class_def<CX2GUNPC>( L, "GetTargetUser",				&CX2GUNPC::GetTargetUser);
	lua_tinker::class_def<CX2GUNPC>( L, "GetTargetNPC",					&CX2GUNPC::GetTargetNPC);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNearestUserPos",			&CX2GUNPC::GetNearestUserPos);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNearestNPCPos",				&CX2GUNPC::GetNearestNPCPos);
	lua_tinker::class_def<CX2GUNPC>( L, "IsUnitID",			        	&CX2GUNPC::IsUnitID);

	lua_tinker::class_def<CX2GUNPC>( L, "GetStartState",				&CX2GUNPC::GetStartState);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDisabledStateID",			&CX2GUNPC::GetDisabledStateID);
	lua_tinker::class_def<CX2GUNPC>( L, "StateChangeForce",				&CX2GUNPC::StateChangeForce);
	lua_tinker::class_def<CX2GUNPC>( L, "SetRageForce",					&CX2GUNPC::SetRageForce);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNowStateID",				&CX2GUNPC::GetNowStateID);
	lua_tinker::class_def<CX2GUNPC>( L, "GetStateIDByKeyword",		&CX2GUNPC::GetStateIDByKeyword);
	lua_tinker::class_def<CX2GUNPC>( L, "StateChange_LUA",				&CX2GUNPC::StateChange_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "IsSelfDestructing",			&CX2GUNPC::IsSelfDestructing);
	lua_tinker::class_def<CX2GUNPC>( L, "StartSelfDestruction",			&CX2GUNPC::StartSelfDestruction);
	lua_tinker::class_def<CX2GUNPC>( L, "StopSelfDestruction",			&CX2GUNPC::StopSelfDestruction);
	lua_tinker::class_def<CX2GUNPC>( L, "AttachParticle_LUA",			&CX2GUNPC::AttachParticle_LUA);
    lua_tinker::class_def<CX2GUNPC>( L, "GetRandVal",			        &CX2GUNPC::GetRandomInt);
    lua_tinker::class_def<CX2GUNPC>( L, "GetAttackFront",			    &CX2GUNPC::GetAttackFront);
    lua_tinker::class_def<CX2GUNPC>( L, "SetShowBossGage",			    &CX2GUNPC::SetShowBossGage);
	lua_tinker::class_def<CX2GUNPC>( L, "GetShowBossName",			    &CX2GUNPC::GetShowBossName);
    lua_tinker::class_def<CX2GUNPC>( L, "SetCanPassUnit",			    &CX2GUNPC::SetCanPassUnit);
    lua_tinker::class_def<CX2GUNPC>( L, "SetFocusCamera",			    &CX2GUNPC::SetFocusCamera);
	//lua_tinker::class_def<CX2GUNPC>( L, "ChangeNPC",					&CX2GUNPC::ChangeNPC);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUnitFadeStart",				&CX2GUNPC::SetUnitFadeStart);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUnitFadeMove",				&CX2GUNPC::SetUnitFadeMove);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUnitFadeScale",				&CX2GUNPC::SetUnitFadeScale);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUnitColor",					&CX2GUNPC::SetUnitColor);
	lua_tinker::class_def<CX2GUNPC>( L, "CreateArmagedonBlade",			&CX2GUNPC::CreateArmagedonBlade);
	lua_tinker::class_def<CX2GUNPC>( L, "CreateArmagedonBladeByBot",	&CX2GUNPC::CreateArmagedonBladeByBot);
	lua_tinker::class_def<CX2GUNPC>( L, "ActiveSkillShow_LUA",			&CX2GUNPC::ActiveSkillShow_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "AddConsultNpc",				&CX2GUNPC::AddConsultNpc);
	lua_tinker::class_def<CX2GUNPC>( L, "GetConsultNpc",				&CX2GUNPC::GetConsultNpc);
	lua_tinker::class_def<CX2GUNPC>( L, "GetEntangleTrapAttackDuration",	&CX2GUNPC::GetEntangleTrapAttackDuration);
	lua_tinker::class_def<CX2GUNPC>( L, "GetClassType",					&CX2GUNPC::GetClassType);

	lua_tinker::class_def<CX2GUNPC>( L, "GetRealatedSkillLevel",		&CX2GUNPC::GetRealatedSkillLevel );
	lua_tinker::class_def<CX2GUNPC>( L, "SetRealatedSkillLevel",		&CX2GUNPC::SetRealatedSkillLevel );

	//{{ kimhc // 2010.8.7 // ?????? NPC???? ???? ?????? ??? ???
	lua_tinker::class_def<CX2GUNPC>( L, "GetFocusCameraForce",			&CX2GUNPC::GetFocusCameraForce);
	lua_tinker::class_def<CX2GUNPC>( L, "SetFocusCameraForce",			&CX2GUNPC::SetFocusCameraForce);
	//}} kimhc // 2010.8.7 // ?????? NPC???? ???? ?????? ??? ???

//{{oasis:?????////2009-10-7////
	lua_tinker::class_def<CX2GUNPC>( L, "GetTargetUnitUID_LUA",						&CX2GUNPC::GetTargetUnitUID_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "SetGroupAICommand_LUA",							&CX2GUNPC::SetGroupAICommand_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetGroupAICommand_LUA",							&CX2GUNPC::GetGroupAICommand_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "COMMAND_HOLD_LUA",									&CX2GUNPC::COMMAND_HOLD_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "COMMAND_ATTACK_MOVE_LUA",							&CX2GUNPC::COMMAND_ATTACK_MOVE_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "COMMAND_FORCED_MOVE_TARGET_LUA",					&CX2GUNPC::COMMAND_FORCED_MOVE_TARGET_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "COMMAND_FORCED_MOVE_GROUND_LUA",					&CX2GUNPC::COMMAND_FORCED_MOVE_GROUND_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "COMMAND_SKILL_LUA",								&CX2GUNPC::COMMAND_SKILL_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "GetOnlyConsultNPC_LUA",							&CX2GUNPC::GetOnlyConsultNPC_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetOnlyConsultedNPC_LUA",							&CX2GUNPC::GetOnlyConsultedNPC_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetConsultNearNPC_LUA",							&CX2GUNPC::GetConsultNearNPC_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "IsRightTarget_LUA",								&CX2GUNPC::IsRightTarget_LUA);


	lua_tinker::class_def<CX2GUNPC>( L, "SetAllFlag_LUA",									&CX2GUNPC::SetAllFlag_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "SetGroupAISelfCommandTransit_LUA",					&CX2GUNPC::SetGroupAISelfCommandTransit_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "SetPrevAIMessage_LUA",									&CX2GUNPC::SetPrevAIMessage_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "SetAItoMainAIData_LUA",							&CX2GUNPC::SetAItoMainAIData_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetAItoSubAIData_LUA",								&CX2GUNPC::SetAItoSubAIData_LUA);
//}}oasis:?????////2009-10-7////

//{{ oasis907 : ????? [2010.4.19] //
	lua_tinker::class_def<CX2GUNPC>( L, "SetReflectDamageOnPhysicalDamage_LUA",				&CX2GUNPC::SetReflectDamageOnPhysicalDamage);
	lua_tinker::class_def<CX2GUNPC>( L, "SetReflectDamageOnMagicalDamage_LUA",				&CX2GUNPC::SetReflectDamageOnMagicalDamage);
//}}

	lua_tinker::class_def<CX2GUNPC>( L, "GetRideUnit_LUA",									&CX2GUNPC::GetRideUnit);
	lua_tinker::class_def<CX2GUNPC>( L, "RideOff_LUA",										&CX2GUNPC::RideOff);
	lua_tinker::class_def<CX2GUNPC>( L, "IsMoveableState_LUA",								&CX2GUNPC::IsMoveableState);
	lua_tinker::class_def<CX2GUNPC>( L, "JumpDownFromLinemap_LUA",							&CX2GUNPC::JumpDownFromLinemap);

	//{{ megagame / ????? / [2010-04-06] ??? ???? ??? ???
	lua_tinker::class_def<CX2GUNPC>( L, "GetHitCount_LUA",									&CX2GUNPC::GetHitCount_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetHitCount_LUA",									&CX2GUNPC::SetHitCount_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "GetStateID_LUA",									&CX2GUNPC::GetStateID_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetNextStateID_LUA",								&CX2GUNPC::GetNextStateID_LUA);
	//}} megagame / ????? / [2010-04-06]

	//{{ megagame / ????? / [2010-04-29] ?????? ??? ???
	lua_tinker::class_def<CX2GUNPC>( L, "GetSiegeMode",										&CX2GUNPC::GetSiegeMode);
	lua_tinker::class_def<CX2GUNPC>( L, "SetSiegeMode",										&CX2GUNPC::SetSiegeMode);
	//}} megagame / ????? / [2010-04-29] ?????? ??? ???

	//{{ kimhc // 2010-04-16 // ?????, ???? ?? ??? ?? ?? ????? ??
	lua_tinker::class_def<CX2GUNPC>( L, "SetAtkPhysic_LUA",									&CX2GUNPC::SetAtkPhysic_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetAtkMagic_LUA",									&CX2GUNPC::SetAtkMagic_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetDefPhysic_LUA",									&CX2GUNPC::SetDefPhysic_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetDefMagic_LUA",									&CX2GUNPC::SetDefMagic_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "GetAtkPhysic_LUA",									&CX2GUNPC::GetAtkPhysic_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetAtkMagic_LUA",									&CX2GUNPC::GetAtkMagic_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDefPhysic_LUA",									&CX2GUNPC::GetDefPhysic_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDefMagic_LUA",									&CX2GUNPC::GetDefMagic_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "ReInitAtkAndDef_LUA",								&CX2GUNPC::ReInitAtkAndDef_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "SetEffectSet_LUA",									&CX2GUNPC::SetEffectSet_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetEffectSet_LUA",									&CX2GUNPC::GetEffectSet_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "ClearEffectSet_LUA",								&CX2GUNPC::ClearEffectSet_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "SetDefaultAnimSpeed_LUA",							&CX2GUNPC::SetDefaultAnimSpeed_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "GetDefaultAnimSpeed_LUA",							&CX2GUNPC::GetDefaultAnimSpeed_LUA);

	//}} kimhc // 2010-04-16 // ?????, ???? ?? ??? ?? ?? ????? ??

	//{{ kimhc // 2010.6.29 // ?????? ????????? ????? ??? ?????? ????????? ??? ????????? ????
	lua_tinker::class_def<CX2GUNPC>( L, "ChangeDamageStateID_LUA",							&CX2GUNPC::ChangeDamageStateID_LUA );
	lua_tinker::class_def<CX2GUNPC>( L, "ChangeDyingStateID_LUA",							&CX2GUNPC::ChangeDyingStateID_LUA );
	//}} kimhc // 2010.6.29 // small_front, big_back ???? ???? ??????? ?? ????? A??? stateID?? ???????? B??? stateID?? ????

//{{ kimhc // 2010.7.7 // ????? ?????? ???��?
	lua_tinker::class_def<CX2GUNPC>( L, "InsertMonsterLineMapIndex_LUA",					&CX2GUNPC::InsertMonsterLineMapIndex_LUA );
	lua_tinker::class_def<CX2GUNPC>( L, "GetNumOfMonsterLineMap_LUA",						&CX2GUNPC::GetNumOfMonsterLineMap_LUA );
	lua_tinker::class_def<CX2GUNPC>( L, "GetMonsterLineMapIndex_LUA",						&CX2GUNPC::GetMonsterLineMapIndex_LUA );

//}} kimhc // 2010.7.7 // ????? ?????? ???��?
	//{{ JHKang / ?????? / 2010/12/20 / ?? ???? ?????(SetDie ????)
	lua_tinker::class_def<CX2GUNPC>( L,	"SetDrainHPSeq",									&CX2GUNPC::SetDrainHPSeq );
	lua_tinker::class_def<CX2GUNPC>( L,	"GetReflectMagic_LUA",								&CX2GUNPC::GetReflectMagic );
	lua_tinker::class_def<CX2GUNPC>( L,	"GetDamageTypeThisFrame_LUA",						&CX2GUNPC::GetDamageTypeThisFrame );
	lua_tinker::class_def<CX2GUNPC>( L,	"InitDamageTypeThisFrame_LUA",						&CX2GUNPC::InitDamageTypeThisFrame );
	//}} JHKang / ?????? / 2010/12/20 / ?? ???? ?????(SetDie ????)

	lua_tinker::class_def<CX2GUNPC>( L,	"SetCriticalRate",									&CX2GUNPC::SetCriticalRate );
	lua_tinker::class_def<CX2GUNPC>( L,	"SetMPChangeRate",									&CX2GUNPC::SetCriticalRate );
	lua_tinker::class_def<CX2GUNPC>( L, "GetNearestUserLandPos",							&CX2GUNPC::GetNearestUserLandPos);

//#ifdef ADDED_GET_MONSTER_LAST_TOUCH_LINE_INDEX
	lua_tinker::class_def<CX2GUNPC>( L, "GetLastTouchLineIndex",							&CX2GUNPC::GetLastTouchLineIndex);
// #endif // 	ADDED_GET_MONSTER_LAST_TOUCH_LINEMAP_INDEX

	lua_tinker::class_def<CX2GUNPC>( L, "SetFogNPC_LUA",									&CX2GUNPC::SetFogNPC);

	lua_tinker::class_def<CX2GUNPC>( L, "SetEntryMonsterMessage",							&CX2GUNPC::SetEntryMonsterMessage );

	//}} GUNPC End


///////////////////////////////////////////////

    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowType",			    &CX2GUNPC::SetLightFlowType);
    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowMin",			    &CX2GUNPC::SetLightFlowMin);
    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowMax",			    &CX2GUNPC::SetLightFlowMax);
    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowSpeed",			&CX2GUNPC::SetLightFlowSpeed);
    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowWide",			    &CX2GUNPC::SetLightFlowWide);
    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowColor",			&CX2GUNPC::SetLightFlowColor);
    lua_tinker::class_def<CX2GUNPC>( L, "SetLightFlowPoint",			&CX2GUNPC::SetLightFlowPoint);
	lua_tinker::class_def<CX2GUNPC>( L, "SetBonusDrop",					&CX2GUNPC::SetBonusDrop);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUserHoldReset",				&CX2GUNPC::SetUserHoldReset);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUserHold",					&CX2GUNPC::SetUserHold);
	lua_tinker::class_def<CX2GUNPC>( L, "GetUserHold",					&CX2GUNPC::GetUserHold);


	lua_tinker::class_def<CX2GUNPC>( L, "IsLostGrap",				&CX2GUNPC::IsLostGrap);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUserGrapReset",				&CX2GUNPC::SetUserGrapReset);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUserGrap",					&CX2GUNPC::SetUserGrap);
	lua_tinker::class_def<CX2GUNPC>( L, "GetUserGrap",					&CX2GUNPC::GetUserGrap);

//{{ kimhc // 2010.7.27 // ???????? ?????? HOLD ?? ?? ????? ??
	lua_tinker::class_def<CX2GUNPC>( L, "SetUnHoldWhenRevenged",		&CX2GUNPC::SetUnHoldWhenRevenged);
//}} kimhc // 2010.7.27 // ???????? ?????? HOLD ?? ?? ????? ??
	lua_tinker::class_def<CX2GUNPC>( L, "GetLastAttackGameUnit",		&CX2GUNPC::GetLastAttackGameUnit);
	lua_tinker::class_def<CX2GUNPC>( L, "GetLastAttackUnit",			&CX2GUNPC::GetLastAttackUnit);
	lua_tinker::class_def<CX2GUNPC>( L, "GetHitter",					&CX2GUNPC::GetHitter);

	//{{ oasis907 : ????? [2010.7.28] //
	lua_tinker::class_def<CX2GUNPC>( L, "SetRideToUnit_LUA",			&CX2GUNPC::SetRideToUnit );
	lua_tinker::class_def<CX2GUNPC>( L, "StopRideToUnit_LUA",			&CX2GUNPC::StopRideToUnit );
	lua_tinker::class_def<CX2GUNPC>( L, "SetPositionForce",				&CX2GUNPC::SetPositionForce);

	lua_tinker::class_def<CX2GUNPC>( L, "SetCloaking",					&CX2GUNPC::SetCloaking);
	lua_tinker::class_def<CX2GUNPC>( L, "SetUnCloaking",				&CX2GUNPC::SetUnCloaking);

	//}} oasis907 : ????? [2010.7.28] //

	lua_tinker::class_def<CX2GUNPC>( L, "SetApplyMotionOffset",				&CX2GUNPC::SetApplyMotionOffset);

	lua_tinker::class_def<CX2GUNPC>( L, "GetNearestGUUser",		&CX2GUNPC::GetNearestGUUser );
	lua_tinker::class_def<CX2GUNPC>( L, "SetNearestGUUser",	&CX2GUNPC::SetNearestGUUser );

	lua_tinker::class_def<CX2GUNPC>( L,	"SetRageCountMax",				&CX2GUNPC::SetRageCountMax );
	lua_tinker::class_def<CX2GUNPC>( L,	"SetEnableAfterImageAlways",	&CX2GUNPC::SetEnableAfterImageAlways );
	lua_tinker::class_def<CX2GUNPC>( L,	"ChangeAfterImageColor",		&CX2GUNPC::ChangeAfterImageColor );

	lua_tinker::class_def<CX2GUNPC>( L, "GetOwnerUserUnitUID",		&CX2GUNPC::	GetOwnerGameUnitUID );
	lua_tinker::class_def<CX2GUNPC>( L, "GetOwnerGameUnit",		&CX2GUNPC::	GetOwnerGameUnit );

	lua_tinker::class_def<CX2GUNPC>( L, "IsOnSomething_LUA",		&CX2GUNPC::IsOnSomething);

	lua_tinker::class_def<CX2GUNPC>( L, "CreateAllyNpcByMonster_LUA",		&CX2GUNPC::CreateAllyNpcByMonster_LUA);
	lua_tinker::class_def<CX2GUNPC>( L, "SetCoolTimeSummonCard_LUA",		&CX2GUNPC::SetCoolTimeSummonCard_LUA);

	lua_tinker::class_def<CX2GUNPC>( L, "GetNearestUnitPosByOwnerUnit", &CX2GUNPC::GetNearestUnitPosByOwnerUnit );		/// NPC?? ????? ?????? ???? ?????? ??? ?? ?????? ???
	lua_tinker::class_def<CX2GUNPC>( L, "GetNearestActiveNPCPos",		&CX2GUNPC::GetNearestActiveNPCPos );			/// ???? Active ???? NPC?? ????? ???

	lua_tinker::class_def<CX2GUNPC>( L, "SetFollowTargetUnitBone",		&CX2GUNPC::SetFollowTargetUnitBone);
	lua_tinker::class_def<CX2GUNPC>( L, "ActiveFollowTargetUnitBone",	&CX2GUNPC::ActiveFollowTargetUnitBone);
	lua_tinker::class_def<CX2GUNPC>( L, "CreateEltrionEvenlyLockonMissile",	&CX2GUNPC::CreateEltrionEvenlyLockonMissile);


	lua_tinker::class_add<CX2GUUser>( L, "CX2GUUser" );
	lua_tinker::class_inh<CX2GUUser, CX2GameUnit>( L );
	lua_tinker::class_def<CX2GUUser>( L, "GetHyperModeUsed",			&CX2GUUser::GetHyperModeUsed );
	lua_tinker::class_def<CX2GUUser>( L, "SetDevAddDamage",				&CX2GUUser::SetDevAddDamage );
	lua_tinker::class_def<CX2GUUser>( L, "ForceChangeHyperMode",		&CX2GUUser::ForceChangeHyperMode );
	lua_tinker::class_def<CX2GUUser>( L, "GetNowSpecialAttack",			&CX2GUUser::GetNowSpecialAttack );
	lua_tinker::class_def<CX2GUUser>( L, "GetHoldSpecialAttack",		&CX2GUUser::GetHoldSpecialAttack );
	lua_tinker::class_def<CX2GUUser>( L, "GetHoldingSpecialAttack",		&CX2GUUser::GetHoldingSpecialAttack );
	lua_tinker::class_def<CX2GUUser>( L, "GetAttackSuccess",			&CX2GUUser::GetAttackSuccess );
	lua_tinker::class_def<CX2GUUser>( L, "GetRotateDegree",				&CX2GUUser::GetRotateDegree );
	lua_tinker::class_def<CX2GUUser>( L, "SetPos",						&CX2GUUser::SetPos );
	lua_tinker::class_def<CX2GUUser>( L, "SetHold",						&CX2GUUser::SetHold );
	lua_tinker::class_def<CX2GUUser>( L, "GetHold",						&CX2GUUser::GetHold );

//#ifdef TRANSFORMER_TEST
//	lua_tinker::class_def<CX2GUUser>( L, "TransformIntoMonster",		&CX2GUUser::TransformIntoMonster );
//	lua_tinker::class_def<CX2GUUser>( L, "TransformIntoUser",			&CX2GUUser::TransformIntoUser );
//#endif TRANSFORMER_TEST

	//{{ oasis907 : ????? [2010.8.2] //
	lua_tinker::class_def<CX2GUUser>( L, "GetLandPosition_LUA",			&CX2GUUser::GetLandPos );
	//}}

	//{{ kimhc // 2010.8.10 // ??? ?????? ??? ???? ??? ????? ???
	lua_tinker::class_def<CX2GUUser>( L, "InitPosByMonsterPos",			&CX2GUUser::InitPosByMonsterPos );
	//}} kimhc // 2010.8.10 // ??? ?????? ??? ???? ??? ????? ???
	//{{ JHKang / ?????? / 2010/12/16 / ???? ??? ????????
	lua_tinker::class_def<CX2GUUser>( L, "GetNowHit",					&CX2GUUser::GetNowHit );
	lua_tinker::class_def<CX2GUUser>( L, "SetNowHit",					&CX2GUUser::SetNowHit );
	//}} JHKang / ?????? / 2010/12/16

	//{{ JHKang / ?????? / 2011/01/17 / ???? ?????? ???? ?????? ??? ???
	lua_tinker::class_def<CX2GUUser>( L, "GetRealDamage",		&CX2GUUser::GetRealDamage );
	lua_tinker::class_def<CX2GUUser>( L, "SetAnimSpeed_LUA",	&CX2GUUser::SetAnimSpeed_LUA );
	//}} JHKang / ?????? / 2011/01/17 / ???? ?????? ???? ?????? ??? ???

	// oasis907 : ????? [2010.12.21] // ? ?????
	lua_tinker::class_def<CX2GUUser>( L, "SetHyperModeCount",			&CX2GUUser::SetHyperModeCount );

	lua_tinker::class_def<CX2GUUser>( L, "GetIsRight",					&CX2GUUser::GetIsRight);

	lua_tinker::class_def<CX2GUUser>( L, "StartHenirBuffFirst",			&CX2GUUser::StartHenirBuffFirst);
	lua_tinker::class_def<CX2GUUser>( L, "StartHenirBuffDark",			&CX2GUUser::StartHenirBuffDark);
	lua_tinker::class_def<CX2GUUser>( L, "EndHenirBuff",				&CX2GUUser::EndHenirBuff);

	lua_tinker::class_def<CX2GUUser>( L, "CreateTeleportEffect",		&CX2GUUser::CreateTeleportEffect);

	lua_tinker::class_def<CX2GUUser>( L, "SetPositionOnLine",			&CX2GUUser::SetPositionOnLine);
	lua_tinker::class_def<CX2GUUser>( L, "SetShow",						&CX2GUUser::SetShowObject);




	lua_tinker::class_def<CX2GUUser>( L, "SetDPValue",					&CX2GUUser::SetDPValue );
	lua_tinker::class_def<CX2GUUser>( L, "SetMutationCount",			&CX2GUUser::SetMutationCount );



	lua_tinker::class_add<CX2DamageEffect>( L, "CX2DamageEffect" );
	lua_tinker::class_def<CX2DamageEffect>( L, "CreateInstance_LUA",		&CX2DamageEffect::CreateInstance_LUA );
	lua_tinker::class_def<CX2DamageEffect>( L, "CreateInstance_LUA2",		&CX2DamageEffect::CreateInstance_LUA2 );
	lua_tinker::class_def<CX2DamageEffect>( L, "CreateInstanceByPet_LUA",		&CX2DamageEffect::CreateInstanceByPet_LUA );
	lua_tinker::class_def<CX2DamageEffect>( L, "CreateInstanceParabolic_LUA",	&CX2DamageEffect::CreateInstanceParabolic_LUA );
	lua_tinker::class_def<CX2DamageEffect>( L, "IsLiveInstance",			&CX2DamageEffect::IsLiveInstance );
    lua_tinker::class_def<CX2DamageEffect>( L, "DestroyInstance",			&CX2DamageEffect::DestroyInstance_LUA );

	lua_tinker::class_add<CX2EffectSet>( L, "CX2EffectSet" );
	lua_tinker::class_def<CX2EffectSet>( L, "AddEffectSetTemplet_LUA",		&CX2EffectSet::AddEffectSetTemplet_LUA );
	//{{ kimhc // 2010.4.19 // ??��??? ???
	lua_tinker::class_def<CX2EffectSet>( L, "PlayEffectSet_LUA",				&CX2EffectSet::PlayEffectSet_LUA );
	lua_tinker::class_def<CX2EffectSet>( L, "PlayEffectSetWithCustomPos_LUA",	&CX2EffectSet::PlayEffectSetWithCustomPos_LUA );
	lua_tinker::class_def<CX2EffectSet>( L, "StopEffectSet_LUA",				&CX2EffectSet::StopEffectSet_LUA );
	//}} kimhc // 2010.4.19 // ??��??? ???

	//{{ JHKang / ?????? / 2011/01/19
	lua_tinker::class_def<CX2EffectSet>( L, "PlayEffectSetTraceTarget_LUA",	&CX2EffectSet::PlayEffectSetTraceTarget_LUA );
	//}} JHKang / ?????? / 2011/01/19

	lua_tinker::class_def<CX2EffectSet>( L, "PlayEffectSetByPet_LUA",			&CX2EffectSet::PlayEffectSetByPet_LUA );
	lua_tinker::class_def<CX2EffectSet>( L, "SetEffectSetInstPos",			&CX2EffectSet::SetEffectSetInstPos );
	lua_tinker::class_def<CX2EffectSet>( L, "SetEffectScale_LUA",			&CX2EffectSet::SetEffectScale_LUA );

	lua_tinker::class_def<CX2EffectSet>( L, "GetEffectPosition_LUA",		&CX2EffectSet::GetEffectPosition_LUA );

//#ifdef SERV_HERO_PVP_MANAGE_LIST
//	lua_tinker::class_def<CX2UIHeroMatch>( L, "AddHelperText_Hero",		&CX2UIHeroMatch::AddHelperText_Hero );
//#endif //SERV_HERO_PVP_MANAGE_LIST

	lua_tinker::class_add<CX2EffectSet::Handle>( L, "CX2EffectSetHandle" );
	lua_tinker::class_def<CX2EffectSet::Handle>( L, "IsEqual",					&CX2EffectSet::Handle::IsEqual );


	lua_tinker::class_add<CX2GameEdit>( L, "CX2GameEdit" );
	lua_tinker::class_def<CX2GameEdit>( L, "AddCommandChange",			&CX2GameEdit::AddCommandChange );
	lua_tinker::class_def<CX2GameEdit>( L, "AddString",					&CX2GameEdit::AddString_LUA );
	lua_tinker::class_def<CX2GameEdit>( L, "AddHelperText",				&CX2GameEdit::AddHelperText );
	lua_tinker::class_def<CX2GameEdit>( L, "ToggleCommandList",			&CX2GameEdit::ToggleCommandList );
	lua_tinker::class_def<CX2GameEdit>( L, "SaveFileLoadedDevices",		&CX2GameEdit::SaveFileLoadedDevices  );
	lua_tinker::class_def<CX2GameEdit>( L, "ChangeSpirit",				&CX2GameEdit::SaveFileLoadedDevices  );
	lua_tinker::class_def<CX2GameEdit>( L, "GetLastCreatedMonster",		&CX2GameEdit::GetLastCreatedMonster  );
	lua_tinker::class_def<CX2GameEdit>( L, "SetLastCreatedMonster",		&CX2GameEdit::SetLastCreatedMonster  );
	lua_tinker::class_def<CX2GameEdit>( L, "MonsterStateChange",		&CX2GameEdit::MonsterStateChange  );
	//{{ 2010. 8. 10	??????	?? ?????
	lua_tinker::class_def<CX2GameEdit>( L, "PetCheat",					&CX2GameEdit::PetCheat_LUA );
	lua_tinker::class_def<CX2GameEdit>( L, "PetActionCheat",					&CX2GameEdit::PetActionCheat_LUA );
	//}}
	//{{ 2010. 9. 29	??????	ED ?????? ??
	lua_tinker::class_def<CX2GameEdit>( L, "UserEdCheat",				&CX2GameEdit::UserEdCheat_LUA );
	//}}
	//{{ 2011. 04. 16	??????	?�� ????
//#ifdef SERV_PSHOP_AGENCY
	lua_tinker::class_def<CX2GameEdit>( L, "ShopCheat",					&CX2GameEdit::ShopCheat_LUA );
//#endif SERV_PSHOP_AGENCY
	//}}
	//{{ 2011. 08. 11	??????	???? ???? ??��?
	lua_tinker::class_def<CX2GameEdit>( L, "ChannelingCodeCheat",		&CX2GameEdit::ChannelingCodeCheat_LUA );
	//}}
	//{{ 2011. 08. 12   ??��?      ???? ????
	lua_tinker::class_def<CX2GameEdit>( L, "HenirRewardCountCheat",		&CX2GameEdit::HenirRewardCountCheat_LUA );
		//}}
	lua_tinker::class_def<CX2GameEdit>( L, "HenirBuffCheat",			&CX2GameEdit::HenirBuffCheat_LUA );

	lua_tinker::class_def<CX2GameEdit>( L, "ElswordWSPCheat",			&CX2GameEdit::ElswordWSPCheat_LUA );

	lua_tinker::class_def<CX2GameEdit>( L, "RefreshDailyQuest",			&CX2GameEdit::RefreshDailyQuest );

	//{{ 2011. 09. 20  ??��?	????? ED ???? ??????
	lua_tinker::class_def<CX2GameEdit>( L, "EDMonitoringModeCheat",		&CX2GameEdit::EDMonitoringModeCheat_LUA );
	//}}

	//{{ 2009-11-30  ??????
	lua_tinker::class_def<CX2GameEdit>( L, "AdminGetGuildSPoint",		&CX2GameEdit::AdminGetGuildSPoint_LUA );
	//}}

	//{{ lucidash : 2010.11.24
#if X2_DEV_TOOLS
	//lua_tinker::class_def<CX2GameEdit>( L, "ItemViewCheat",		&CX2GameEdit::ItemViewCheat );
#endif // ITEM_VIEW_CHEAT
	//}} lucidash : 2010.11.24

	lua_tinker::class_def<CX2GameEdit>( L, "RequireGetDangerousValue",		&CX2GameEdit::RequireGetDangerousValue_LUA );

	lua_tinker::class_def<CX2GameEdit>( L, "IncreaseFieldDangerousValue",		&CX2GameEdit::IncreaseFieldDangerousValue_LUA );



		lua_tinker::class_def<CX2GameEdit>( L, "DeleteGUildAdd",	&CX2GameEdit::DeleteGUildAdd_LUA );


		//{{ 2011.03.04 ???? ???? ??? ???? ?? ( ????,?????? ???? )
		lua_tinker::class_def<CX2GameEdit>( L, "DeleteHenirRankingRank",	&CX2GameEdit::DeleteHenirRankingRank_LUA );
		lua_tinker::class_def<CX2GameEdit>( L, "DeleteHenirRankingNickName",	&CX2GameEdit::DeleteHenirRankingNickName_LUA );
		lua_tinker::class_def<CX2GameEdit>( L, "DeleteHenirRankingUnitUID",	&CX2GameEdit::DeleteHenirRankingUnitUID_LUA );
		//}}
		lua_tinker::class_def<CX2GameEdit>( L, "ToggleInvisible",			&CX2GameEdit::ToggleInvisible_LUA );

		//{{ kimhc // 2012-10-16 // ???? ???? ?????? ?????? ??????? ???
		lua_tinker::class_def<CX2GameEdit>( L, "AveragePingTime",		&CX2GameEdit::AveragePingTime_LUA );	/// Host?? ?????? ????? ?????????? ????
		//}} kimhc // 2012-10-16 // ???? ???? ?????? ?????? ??????? ???


	lua_tinker::class_def<CX2GameEdit>( L, "IncreaseLoverDate_LUA",	&CX2GameEdit::IncreaseLoverDate_LUA );

	lua_tinker::class_def<CX2GameEdit>( L, "FullChargeRidingStamina",	&CX2GameEdit::FullChargeRidingStamina );
#if X2_DEV_TOOLS
	lua_tinker::class_def<CX2GameEdit>( L, "SetEffectLogLevel",	&CX2GameEdit::SetEffectLogLevel );
#endif //EFFECT_USE_LOG

	lua_tinker::class_def<CX2GameEdit>( L, "OpenRaidGate",	&CX2GameEdit::OpenRaidGate );
	lua_tinker::class_def<CX2GameEdit>( L, "CloseRateGate",	&CX2GameEdit::CloseRateGate );
	lua_tinker::class_def<CX2GameEdit>( L, "RequireGetTotalDangerousValue",	&CX2GameEdit::RequireGetTotalDangerousValue );
	lua_tinker::class_def<CX2GameEdit>( L, "IncreaseTotalDangerousValue",	&CX2GameEdit::IncreaseTotalDangerousValue );


	lua_tinker::class_add<CX2TextManager::CX2Text>( L, "CX2Text" );
	lua_tinker::class_def<CX2TextManager::CX2Text>( L, "Move",				&CX2TextManager::CX2Text::Move_LUA );


	lua_tinker::class_add<CX2TextManager>( L, "CX2TextManager" );
	lua_tinker::class_def<CX2TextManager>( L, "AddText",					&CX2TextManager::AddText_LUA );


	lua_tinker::class_add<CX2SlotManager>( L, "CX2SlotManager" );
	lua_tinker::class_def<CX2SlotManager>( L, "AddSlot",					&CX2SlotManager::AddSlot );
	lua_tinker::class_def<CX2SlotManager>( L, "CreateItemSlot",				&CX2SlotManager::CreateItemSlot );
	lua_tinker::class_def<CX2SlotManager>( L, "GetDummyValue",				&CX2SlotManager::GetDummyValue_LUA );


	lua_tinker::class_add<CX2CouponBox>( L, "CX2CouponBox" );
	lua_tinker::class_inh< CX2CouponBox, CX2SlotManager>( L );



    lua_tinker::class_add<CX2PostBox>( L, "CX2PostBox" );
    lua_tinker::class_inh< CX2PostBox, CX2SlotManager>( L );


	lua_tinker::class_add<CX2SlotManager::CX2Slot>( L, "CX2Slot" );
	lua_tinker::class_def<CX2SlotManager::CX2Slot>( L, "SetSlotData",		&CX2SlotManager::CX2Slot::SetSlotData_LUA );
	lua_tinker::class_def<CX2SlotManager::CX2Slot>( L, "SetClickable",		&CX2SlotManager::CX2Slot::SetClickable );


	lua_tinker::class_add<CX2SlotItem>( L, "CX2SlotItem" );
	lua_tinker::class_inh< CX2SlotItem, CX2SlotManager::CX2Slot>( L );



	lua_tinker::class_add<CX2SquareUnit>( L, "CX2SquareUnit" );
	lua_tinker::class_def<CX2SquareUnit>( L, "StateChange_LUA",				&CX2SquareUnit::StateChange_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "SetRight_LUA",				&CX2SquareUnit::SetRight_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetRight_LUA",				&CX2SquareUnit::GetRight_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureLeft_LUA",				&CX2SquareUnit::GetPureLeft_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneLeft_LUA",				&CX2SquareUnit::GetOneLeft_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureDoubleLeft_LUA",		&CX2SquareUnit::GetPureDoubleLeft_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneDoubleLeft_LUA",		&CX2SquareUnit::GetOneDoubleLeft_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureRight_LUA",			&CX2SquareUnit::GetPureRight_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneRight_LUA",				&CX2SquareUnit::GetOneRight_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureDoubleRight_LUA",		&CX2SquareUnit::GetPureDoubleRight_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneDoubleRight_LUA",		&CX2SquareUnit::GetOneDoubleRight_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureUp_LUA",				&CX2SquareUnit::GetPureUp_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneUp_LUA",				&CX2SquareUnit::GetOneUp_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureDoubleUp_LUA",			&CX2SquareUnit::GetPureDoubleUp_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneDoubleUp_LUA",			&CX2SquareUnit::GetOneDoubleUp_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureDown_LUA",				&CX2SquareUnit::GetPureDown_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneDown_LUA",				&CX2SquareUnit::GetOneDown_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetPureDoubleDown_LUA",		&CX2SquareUnit::GetPureDoubleDown_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetOneDoubleDown_LUA",		&CX2SquareUnit::GetOneDoubleDown_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetFootOnLine_LUA",			&CX2SquareUnit::GetFootOnLine_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "CanDown_LUA",					&CX2SquareUnit::CanDown_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "AddPosY_LUA",					&CX2SquareUnit::AddPosY_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetYSpeed_LUA",				&CX2SquareUnit::GetYSpeed_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "SetXSpeed_LUA",				&CX2SquareUnit::SetXSpeed_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "SetYSpeed_LUA",				&CX2SquareUnit::SetYSpeed_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetRiding_LUA",						&CX2SquareUnit::GetRidingOn );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetCheckWaitHabitElapsedTime_LUA",	&CX2SquareUnit::GetCheckWaitHabitElapsedTime );
	lua_tinker::class_def<CX2SquareUnit>( L, "ResetWaitHabitElapsedTime_LUA",		&CX2SquareUnit::ResetWaitHabitElapsedTime );
	lua_tinker::class_add<CX2RidingPetManager>( L, "CX2RidingPetManager" );
	lua_tinker::class_def<CX2RidingPetManager>( L, "AddRidingPetTemplet",	&CX2RidingPetManager::AddRidingPetTemplet_LUA );
	lua_tinker::class_def<CX2RidingPetManager>( L, "SetEnableStamina", &CX2RidingPetManager::SetEnableStamina_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "GetFieldUnit_LUA",			&CX2SquareUnit::GetFieldUnit_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "IsMyUnit",					&CX2SquareUnit::IsMyUnit );
	lua_tinker::class_def<CX2SquareUnit>( L, "PlaySound_LUA",				&CX2SquareUnit::PlaySound_LUA );
	lua_tinker::class_def<CX2SquareUnit>( L, "IsAnimationEnd_LUA",			&CX2SquareUnit::IsAnimationEnd_LUA );



	lua_tinker::class_add<CX2SkillTree>( L, "CX2SkillTree" );
	lua_tinker::class_def<CX2SkillTree>( L, "AddSkillTemplet_LUA",			&CX2SkillTree::AddSkillTemplet_LUA );
	lua_tinker::class_def<CX2SkillTree>( L, "AddSkillTreeTemplet_LUA",		&CX2SkillTree::AddSkillTreeTemplet_LUA );
	lua_tinker::class_def<CX2SkillTree>( L, "AddSkillPowerRate_LUA",		&CX2SkillTree::AddSkillPowerRate_LUA );
	lua_tinker::class_def<CX2SkillTree>( L, "AddDefaultSkill_LUA",			&CX2SkillTree::AddDefaultSkill_LUA );

	lua_tinker::class_def<CX2SkillTree>( L, "IsMyClassSkillID_LUA",			&CX2SkillTree::IsMyClassSkillID );
	lua_tinker::class_def<CX2SkillTree>( L, "GetMaxLimitedSkillLevel_LUA",	&CX2SkillTree::GetMaxLimitedSkillLevel );
	//{{ 2009. 8. 5  ??????		???��??
	lua_tinker::class_def<CX2SkillTree>( L, "AddSealSkillInfo",				&CX2SkillTree::AddSealSkillInfo_LUA );
	//}}

// oasis907 : ????? [2009.12.1] // ???? ??? ???? ?????? ????
//#ifdef GUILD_SKILL
	//{{ oasis907 : ????? //// 2009-11-12 ////
	lua_tinker::class_def<CX2SkillTree>( L, "AddGuildSkillTreeTemplet_LUA",		&CX2SkillTree::AddGuildSkillTreeTemplet_LUA );
	//}} oasis907 : ????? //// 2009-11-12 ////
//#endif GUILD_SKILL


	lua_tinker::class_add<CX2EXPTable>( L, "CX2EXPTable" );
	lua_tinker::class_def<CX2EXPTable>( L, "AddExpTable_LUA",				&CX2EXPTable::AddExpTable_LUA );



	lua_tinker::class_add<CX2DamageEffect::CEffect>( L, "CEffect" );
	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "SetLockOnNPCUID",					&CX2DamageEffect::CEffect::SetLockOnNPCUID );
	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "GetLockOnUnitUID_LUA",				&CX2DamageEffect::CEffect::GetLockOnUnitUID );
	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "GetMainEffect",					&CX2DamageEffect::CEffect::GetMainEffect );


	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "SetPos",			&CX2DamageEffect::CEffect::SetPos );

	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "GetDamageData",					&CX2DamageEffect::CEffect::GetDamageData );
	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "GetDamageDataPtr",					&CX2DamageEffect::CEffect::GetDamageDataPtr );

	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "GetPos",					&CX2DamageEffect::CEffect::GetPos );

	lua_tinker::class_def<CX2DamageEffect::CEffect>( L, "SetLockOnNearstTarget",			&CX2DamageEffect::CEffect::SetLockOnNearstTarget );

	lua_tinker::class_add<CX2DamageEffect::CEffectHandle>( L, "CX2DamageEffectHandle" );
	lua_tinker::class_def<CX2DamageEffect::CEffectHandle>( L, "IsEqual",					&CX2DamageEffect::CEffectHandle::IsEqual );


	lua_tinker::class_add<CX2TrainingCenterTable>( L, "KTrainingCenterTable" );
	lua_tinker::class_def<CX2TrainingCenterTable>( L, "AddTCTemplet",						&CX2TrainingCenterTable::AddTCTemplet_LUA );
	lua_tinker::class_def<CX2TrainingCenterTable>( L, "SetTrainingListByUnitClass_LUA",		&CX2TrainingCenterTable::SetTrainingListByUnitClass_LUA );


	lua_tinker::class_add<CX2DropItemManager>( L, "CX2DropItemManager" );
	lua_tinker::class_def<CX2DropItemManager>( L, "AddDropItem_LUA",						&CX2DropItemManager::AddDropItem_LUA );
	lua_tinker::class_def<CX2DropItemManager>( L, "CountDropItemByID",						&CX2DropItemManager::CountDropItemByID );
	lua_tinker::class_def<CX2DropItemManager>( L, "DeleteAllItem",							&CX2DropItemManager::DeleteAllItem );
    lua_tinker::class_def<CX2DropItemManager>( L, "GetDropItemPosition_LUA",					&CX2DropItemManager::GetDropItemPosition_LUA );



	lua_tinker::class_add<CX2PVPEmblem>( L, "CX2PVPEmblem" );
	lua_tinker::class_def<CX2PVPEmblem>( L, "AddEmblemData_LUA",							&CX2PVPEmblem::AddEmblemData_LUA );



	lua_tinker::class_add<CX2SocketItem>( L, "CX2SocketItem" );
	lua_tinker::class_def<CX2SocketItem>( L, "AddSocketData_LUA",							&CX2SocketItem::AddSocketData_LUA );

	lua_tinker::class_def<CX2SocketItem>( L, "AddSocektGroupDataForCashAvatar_LUA",			&CX2SocketItem::AddSocketGroupDataForCashAvatar_LUA );

	lua_tinker::class_def<CX2SocketItem>( L, "AddSkillLevelUpSocketGroup",					&CX2SocketItem::AddSkillLevelUpSocketGroup_LUA );

	lua_tinker::class_add<CX2StringFilter>( L, "CXSLStringFilter" );
	lua_tinker::class_def<CX2StringFilter>( L, "AddNickNameFilter",				&CX2StringFilter::AddNickNameFilter_LUA );
	lua_tinker::class_def<CX2StringFilter>( L, "AddNickNameWordFilter",			&CX2StringFilter::AddNickNameWordFilter_LUA );
	lua_tinker::class_def<CX2StringFilter>( L, "AddWordFilter",					&CX2StringFilter::AddWordFilter_LUA );
	lua_tinker::class_def<CX2StringFilter>( L, "AddPostWordFilter",				&CX2StringFilter::AddPostWordFilter_LUA );

	lua_tinker::class_add<CX2LVUpEventMgr>( L, "CX2LVUpEventMgr" );
	lua_tinker::class_def<CX2LVUpEventMgr>( L, "AddLevelUpEvent_LUA",			&CX2LVUpEventMgr::AddLevelUpEvent_LUA);

	lua_tinker::class_add<CX2StatePVPRoom::CX2MapSelectWindow>( L, "CX2MapSelectWindow" );
	lua_tinker::class_def<CX2StatePVPRoom::CX2MapSelectWindow>( L, "AddMapInfo_LUA",	&CX2StatePVPRoom::CX2MapSelectWindow::AddMapInfo_LUA );

	lua_tinker::class_add<CX2EnchantItem>( L, "CX2EnchantItem" );
	lua_tinker::class_def<CX2EnchantItem>( L, "AddEnchantData_LUA",			&CX2EnchantItem::AddEnchantData_LUA);
	lua_tinker::class_def<CX2EnchantItem>( L, "AddEnchantRequire_LUA",		&CX2EnchantItem::AddEnchantRequire_LUA);

	//{{ 2008. 10. 1  ??????
//#ifdef TITLE_SYSTEM
	lua_tinker::class_add<CX2TitleManager>( L, "CX2TitleManager" );
	lua_tinker::class_def<CX2TitleManager>( L, "AddTitleInfo_LUA",			&CX2TitleManager::AddTitleInfo_LUA );
    lua_tinker::class_def<CX2TitleManager>( L, "AddTitleMissionInfo_LUA",	&CX2TitleManager::AddTitleMissionInfo_LUA );
    lua_tinker::class_def<CX2TitleManager>( L, "AddSubTitleMissionInfo_LUA",	&CX2TitleManager::AddSubTitleMissionInfo_LUA );
//#endif
	//}}

	//{{ kimhc // 2009-10-31 // ?????
	lua_tinker::class_add< CX2GuildManager >( L, "CX2GuildManager" );
	lua_tinker::class_def< CX2GuildManager >( L, "SetLimitGuildLevel",			&CX2GuildManager::SetLimitGuildLevel_LUA );
	lua_tinker::class_def< CX2GuildManager >( L, "AddGuildEXP",					&CX2GuildManager::AddGuildEXP_LUA );
	lua_tinker::class_def< CX2GuildManager >( L, "SetCharLevelFactor",			&CX2GuildManager::EmptyFuncUsedParsing_LUA );
	lua_tinker::class_def< CX2GuildManager >( L, "SetGuildMemberGradeFactor",	&CX2GuildManager::EmptyFuncUsedParsing_LUA );
	lua_tinker::class_def< CX2GuildManager >( L, "SetPcBangFactor",				&CX2GuildManager::EmptyFuncUsedParsing_LUA );
	lua_tinker::class_def< CX2GuildManager >( L, "SetPvpMemberFactor",			&CX2GuildManager::EmptyFuncUsedParsing_LUA );
	//}} kimhc // 2009-10-31 // ?????

	lua_tinker::class_add<CX2PET>( L, "CX2PET" );
	lua_tinker::class_def<CX2PET>( L, "AnimEventTimer",			&CX2PET::AnimEventTimer_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetNowAnimTime",			&CX2PET::GetNowAnimTime_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetLandPosition_LUA",	&CX2PET::GetLandPosition_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetPos",					&CX2PET::GetPos);
	lua_tinker::class_def<CX2PET>( L, "GetBonePos_LUA",			&CX2PET::GetBonePos_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetRotateDegree",		&CX2PET::GetRotateDegree);
	lua_tinker::class_def<CX2PET>( L, "IsAttacking",			&CX2PET::IsAttacking);
	lua_tinker::class_def<CX2PET>( L, "GetDistTarget",			&CX2PET::GetDistTarget);
	lua_tinker::class_def<CX2PET>( L, "IsTargetFront",			&CX2PET::IsTargetFront);
	lua_tinker::class_def<CX2PET>( L, "InitOtherTarget",		&CX2PET::InitOtherTarget);
	lua_tinker::class_def<CX2PET>( L, "GetMaster",				&CX2PET::GetMaster);
	lua_tinker::class_def<CX2PET>( L, "GetUIMajorParticle",		&CX2PET::GetUIMajorParticle);
	lua_tinker::class_def<CX2PET>( L, "GetUIEffectSet",			&CX2PET::GetUIEffectSet);
	lua_tinker::class_def<CX2PET>( L, "SetIsRight",				&CX2PET::SetIsRight);
	lua_tinker::class_def<CX2PET>( L, "SetStateEffect",			&CX2PET::SetStateEffect);
	lua_tinker::class_def<CX2PET>( L, "GetNowStateTimer",		&CX2PET::GetNowStateTimer);
	lua_tinker::class_def<CX2PET>( L, "GetTargetUnit",			&CX2PET::GetTargetUnit);

	lua_tinker::class_def<CX2PET>( L, "SetLockOnDamageEffect",	&CX2PET::SetLockOnDamageEffect);	/// ???? ???? ????? ???? ?????? ?????? ??????? ??? ?????.
	lua_tinker::class_def<CX2PET>( L, "GetRandomInt",			&CX2PET::GetRandomInt);				/// ???? ??? ???
	lua_tinker::class_def<CX2PET>( L, "GetDirVector",			&CX2PET::GetDirVector);				/// ?? ???? ???? ??? ???
	lua_tinker::class_def<CX2PET>( L, "GetIsRight",				&CX2PET::GetIsRight);				/// ?? ?��??? ???? ??? ???

	lua_tinker::class_def<CX2PET>( L, "GetHitUnitListSize",		&CX2PET::GetHitUnitListSize);		/// ??? ????? ???
	lua_tinker::class_def<CX2PET>( L, "GetDamageData_LUA",		&CX2PET::GetDamageData_LUA);		/// ?????? ?????? ???
	lua_tinker::class_def<CX2PET>( L, "SetDamageData_LUA",		&CX2PET::SetDamageData_LUA);		/// ?????? ?????? ????
	lua_tinker::class_def<CX2PET>( L, "ShowIncreaseValue",		&CX2PET::ShowIncreaseValue);		/// HP or MP ??? ??? ???

	lua_tinker::class_def<CX2PET>( L, "SetFlag_LUA",			&CX2PET::SetFlag_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetFlag_LUA",			&CX2PET::GetFlag_LUA);
	lua_tinker::class_def<CX2PET>( L, "SetInt_LUA",				&CX2PET::SetInt_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetInt_LUA",				&CX2PET::GetInt_LUA);
	lua_tinker::class_def<CX2PET>( L, "SetVector_LUA",			&CX2PET::SetVector_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetVector_LUA",			&CX2PET::GetVector_LUA);
	lua_tinker::class_def<CX2PET>( L, "SetNumber_LUA",			&CX2PET::SetNumber_LUA );
	lua_tinker::class_def<CX2PET>( L, "GetNumber_LUA",			&CX2PET::GetNumber_LUA );

	lua_tinker::class_def<CX2PET>( L, "SetEffectSet_LUA",		&CX2PET::SetEffectSet_LUA);
	lua_tinker::class_def<CX2PET>( L, "GetEffectSet_LUA",		&CX2PET::GetEffectSet_LUA);
	lua_tinker::class_def<CX2PET>( L, "ClearEffectSet_LUA",		&CX2PET::ClearEffectSet_LUA);

	lua_tinker::class_add<CX2PetManager>( L, "CX2PetManager" );
	lua_tinker::class_def<CX2PetManager>( L, "AddPetTemplet",			&CX2PetManager::AddPetTemplet_LUA);

	lua_tinker::class_add<CX2BattleFieldManager>( L, "CX2BattleFieldManager" );
	lua_tinker::class_def<CX2BattleFieldManager>( L, "AddBattleFieldData",	&CX2BattleFieldManager::AddBattleFieldData_LUA );
	lua_tinker::class_def<CX2BattleFieldManager>( L, "SetMonsterRespawnFactorByUserCount",	&CX2BattleFieldManager::SetMonsterRespawnFactorByUserCount_LUA );
	lua_tinker::class_def<CX2BattleFieldManager>( L, "SetMonsterRespawnTimeMinMax",			&CX2BattleFieldManager::SetMonsterRespawnTimeMinMax_LUA );
	lua_tinker::class_def<CX2BattleFieldManager>( L, "SetBattleFieldFactor",				&CX2BattleFieldManager::SetBattleFieldFactor_LUA );

	lua_tinker::class_add<CX2PremiumBuffManager>( L, "CX2PremiumBuffManager" );
	lua_tinker::class_def<CX2PremiumBuffManager>( L, "AddBuffInfo",			&CX2PremiumBuffManager::AddBuffInfo_LUA );

	lua_tinker::class_add<CX2BuffTempletManager>( L, "CX2BuffTempletManager" );
	lua_tinker::class_def<CX2BuffTempletManager>( L, "AddBuffTemplet_LUA",			&CX2BuffTempletManager::AddBuffTemplet_LUA );
	lua_tinker::class_def<CX2BuffTempletManager>( L, "AddBuffFactor_LUA",			&CX2BuffTempletManager::AddBuffFactor_LUA );


	lua_tinker::class_add<CX2CubeOpenImageManager>( L, "CX2CubeOpenImageManager" );
	lua_tinker::class_def<CX2CubeOpenImageManager>( L, "AddImageInfo_LUA",			&CX2CubeOpenImageManager::AddImageInfo_LUA );


	lua_tinker::class_add<CX2WeddingManager>( L, "CX2WeddingManager" );
	lua_tinker::class_def<CX2WeddingManager>( L, "AddOfficiantTemplet",			&CX2WeddingManager::AddOfficiantTemplet_LUA );
	lua_tinker::class_def<CX2WeddingManager>( L, "AddWeddingHallTemplet",		&CX2WeddingManager::AddWeddingHallTemplet_LUA );

	lua_tinker::class_add<CX2UIInventory>( L, "CX2UIInventory" );
	lua_tinker::class_def<CX2UIInventory>( L, "AddIceHeaterEventInfo",			&CX2UIInventory::AddIceHeaterEventInfo_LUA );

#if X2_DEV_TOOLS // ??????, ?????? ?????? ??? ??? ???
	lua_tinker::class_add<DeveloperScriptSet>( L, "CX2DeveloperScriptSet" );
	lua_tinker::class_def<DeveloperScriptSet>( L, "AddDeveloperScript",						&DeveloperScriptSet::AddDeveloperScript_LUA );
	lua_tinker::class_def<DeveloperScriptSet>( L, "AddDamageEffectDevFileList_LUA",			&DeveloperScriptSet::AddDamageEffectDevFileList_LUA );
	lua_tinker::class_def<DeveloperScriptSet>( L, "AddEffectSetDevFileList_LUA",			&DeveloperScriptSet::AddEffectSetDevFileList_LUA );
	lua_tinker::class_def<DeveloperScriptSet>( L, "AddGameMajorXMeshPlayerDevFileList_LUA",	&DeveloperScriptSet::AddGameMajorXMeshPlayerDevFileList_LUA );
#endif // EXPAND_DEVELOPER_SCRIPT  // ??????, ?????? ?????? ??? ??? ???

	lua_tinker::class_add<CX2UIPersonalShopBoard>( L, "CX2UIPersonalShopBoard" );
	lua_tinker::class_def<CX2UIPersonalShopBoard>( L, "AddBanItemNameList",					&CX2UIPersonalShopBoard::AddBanItemNameList_LUA );

}



bool CX2Main::StateChange( int stateID, int iDetailStateID )
{


	if ( stateID > XS_INVALID && stateID < XS_END )
	{


		if(g_pData->GetEventScene() != NULL &&
			g_pData->GetEventScene()->GetIsPlayingEventScene() == true)
		{
			g_pData->GetEventScene()->EndEventScene(true);
		}
		g_pKTDXApp->RemoveStage( m_pNowState, true );
		m_pNowState = NULL;

//#ifdef	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER
		if ( g_pKTDXApp->GetDeviceManager() != NULL )
			g_pKTDXApp->GetDeviceManager()->ReleaseAllMemoryBuffers();
//#endif	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER

		m_NowStateID = (X2_STATE)stateID;
		StateInLog();
		AllDeleteServerPacket();
		g_pKTDXApp->SetFrameRate( 75.f );

		//{{ ????? : [2009/7/21] //	????? ????? ? ??????
		if(g_pChatBox != NULL)
		{
			g_pChatBox->ShowNasodMessageDlg( false );
		}
		//}} ????? : [2009/7/21] //	????? ????? ? ??????


		if ( NULL != g_pData->GetRelationshipManager() )
		{
			g_pData->GetRelationshipManager()->ResetRelationPartnerInVillage();
			g_pData->GetRelationshipManager()->ResetRelationPartnerInGame();
		}

		CX2EmblemManager::GetInstance()->InitEmbelmPlayInfo();

		switch( stateID )
		{
			case XS_START_UP:
				{
					m_pNowState = new CX2StateStartUp();
				}
				break;
			case XS_LOGIN:
				{
					m_pNowState = new CX2StateLogin();
				}
				break;

			case XS_JOIN:
				{
					m_pNowState = new CX2StateJoin();
				}
				break;

			case XS_LOADING:
				{
 					m_pNowState = new CX2StateLoading();
				}
				break;

			case XS_SERVER_SELECT:
				{
					g_pData->DeleteAllRooms();
					m_pNowState = new CX2StateServerSelect;


				}
				break;

			case XS_CREATE_UNIT:
				{
					m_pNowState = new CX2StateCreateUnit();
				}
				break;

			case XS_PVP_LOBBY:
				{
					g_pData->DeleteAllRooms();

					SAFE_DELETE(g_pTFieldGame);
					m_pNowState = new CX2StatePvPLobby();
				}
				break;

			case XS_PVP_ROOM:
				{
					m_pNowState = new CX2StatePVPRoom();
				}
				break;

			case XS_PVP_GAME:
				{

					g_pData->DeleteBattleFieldRoom();
					g_pData->DeleteDungeonRoom();	/// ??? ??? ??? ???? ?? ??????...

					if ( NULL == g_pData->GetPVPRoom() )
						g_pData->ResetPVPRoom();

					if ( NULL != g_pX2Room && CX2Room::IsNewDataSet() )		/// ???��? ??????? ???? ???????
						g_pX2Room->ApplyRoomPacketData();

					m_pNowState = new CX2StatePVPGame();
				}
				break;

			case XS_PVP_RESULT:
				{
					m_pNowState = new CX2StatePVPResult();
				}
				break;

			case XS_DUNGEON_RESULT:
				{
					m_pNowState = new CX2StateDungeonResult();
				}
				break;

			case XS_VILLAGE_MAP:
				{


					//g_pData->GetEventScene()->BlackLayOutMove( false, 1.0f, false, true, 3.0f, 1.0f );

					g_pKTDXApp->SetFrameRate( 60.f ); // Originally 40.0f

					g_pData->DeleteAllRooms();

					m_pNowState = new CX2StateField();

					int iMapId = g_pData->GetLocationManager()->GetCurrentVillageID();
					CX2World::WORLD_ID	eWorldId;
					SEnum::VILLAGE_MAP_ID villageId;
					D3DXVECTOR3 vPos;

// 					if(g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL)
// 						iMapId = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_nMapID;

					if(g_pData->GetLocationManager()->GetVillageMapTemplet((SEnum::VILLAGE_MAP_ID)iMapId) == NULL)
					{
						// ????? ?????? ????? ????
						KEGS_STATE_CHANGE_FIELD_REQ kPacket;

						iMapId = SEnum::VMI_RUBEN;
						kPacket.m_iMapID =  (int)iMapId;


						D3DXVECTOR3 vStartPos = g_pData->GetLocationManager()->GetStartPosLoc( 1 );	// ???????? ?????? ???? ???
						g_pData->GetLocationManager()->SetLastPos( vStartPos );


						g_pData->GetLocationManager()->SetVillage( (SEnum::VILLAGE_MAP_ID)kPacket.m_iMapID, vStartPos );

						g_pData->GetServerProtocol()->SendPacket( EGS_STATE_CHANGE_FIELD_REQ, kPacket );
						AddServerPacket( EGS_STATE_CHANGE_FIELD_ACK );

						//SAFE_DELETE_DIALOG( m_pDLGMsgBox );
						//m_pDLGMsgBox = KTDGUIMsgBox( D3DXVECTOR2(250,300), L"?????? ?????? ??????.", this );
					}
					else
					{
						g_pData->GetLocationManager()->GetVillage(villageId, vPos);
						if( true == g_pData->GetLocationManager()->IsDungeonLounge( villageId ) )
						{
							eWorldId = g_pInstanceData->GetDungeonLoungeWorldID();
						}
						else
						{
							eWorldId = (CX2World::WORLD_ID)g_pData->GetLocationManager()->GetWorldID(villageId);
						}


						SAFE_DELETE(g_pTFieldGame);
						g_pTFieldGame = new CX2TFieldGame(eWorldId);

						if(g_pTFieldGame != NULL)
						{
							g_pTFieldGame->AddUnit( g_pData->GetMyUser()->GetSelectUnit(), true, true );
							g_pTFieldGame->SetAddedMyUnit(true);
						}
					}

					if( NULL != g_pData && NULL != g_pData->GetUIManager() )
						g_pData->GetUIManager()->SetShowUI(true, true);

				}
				break;



			case XS_DUNGEON_ROOM:
				{
					m_pNowState = new CX2StateDungeonRoom();
				}
				break;

			case XS_DUNGEON_GAME:
				{
					g_pKTDXApp->SetFrameRate( 70.f );

					g_pData->DeleteBattleFieldRoom();
					g_pData->DeletePVPRoom();	/// ??? ??? ??? ???? ?? ??????...

					if ( NULL == g_pData->GetDungeonRoom() )
						g_pData->ResetDungeonRoom();

					if ( NULL != g_pX2Room && CX2Room::IsNewDataSet() )		/// ???��? ??????? ???? ???????
						g_pX2Room->ApplyRoomPacketData();

					m_pNowState = new CX2StateDungeonGame();
				}
				break;


			case XS_SQUARE_GAME:
				{
					g_pKTDXApp->SetFrameRate( 40.f );

					SAFE_DELETE(g_pTFieldGame);
					m_pNowState = new CX2StateSquareGame();
				}
				break;

			case XS_TRAINING_SCHOOL:
				{
					m_pNowState = new CX2StateTrainingSchool();
				}
				break;
			case XS_TRAINING_GAME:
				{
					m_pNowState = new CX2StateTrainingGame();
				}
				break;

			case XS_TRAINING_RESULT:
				{
					m_pNowState = new CX2StateTrainingResult();
				}
				break;
			case XS_BATTLE_FIELD:
				{
					g_pKTDXApp->SetFrameRate( 72.f );

					g_pData->DeleteAllRooms();
					g_pData->ResetBattleFieldRoom();
					if ( NULL != g_pX2Room && CX2Room::IsNewDataSet() )		/// ???��? ??????? ???? ???????
						g_pX2Room->ApplyRoomPacketData();

					if( NULL != g_pData && NULL != g_pData->GetUIManager() )
						g_pData->GetUIManager()->SetShowUI(true, true);

					m_pNowState = new CX2StateBattleField();
				} break;

			case XS_WEDDING_GAME:
				{
					g_pKTDXApp->SetFrameRate( 40.f );
					//SAFE_DELETE(g_pTFieldGame);
					m_pNowState = new CX2StateWeddingGame();

					if( NULL != g_pData && NULL != g_pData->GetUIManager() )
						g_pData->GetUIManager()->SetShowUI(false, true);

				} break;
			default:
				{
					StateLog( "This state does not exist" );
					ASSERT( !"This state does not exist" );
				} break;
		}

		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetStage( m_pNowState );
		g_pKTDXApp->InsertStage( m_pNowState );

		CX2GageManager::StateChange( stateID );
        //{{ seojt // 2008-10-21, 18:08
        // ???? ????? ???? ?? g_pKTDXApp->PostChangeState()?? ??????.
        //g_pKTDXApp->PostChangeState( m_pNowState );
        //}} seojt // 2008-10-21, 18:08

		PostStateChange();

//#ifdef	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER
		if ( g_pKTDXApp->GetDeviceManager() != NULL )
			g_pKTDXApp->GetDeviceManager()->ReleaseAllMemoryBuffers();
//#endif	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER


		return true;
	}
	else
	{
		return false;
	}
}

void CX2Main::PostStateChange()
{

	// 08.12.14 ???��? : State Change??? ???? ??? ??? UI ????
	if ( NULL != g_pData->GetUIManager() )
	{
		CX2UIManager* pUIManager = g_pData->GetUIManager();
		pUIManager->OnStateChange();

		//{{ kimhc	// ??��? ????? ?? ??��? ?????? ??? ???? ??? ?��???
		if ( pUIManager->GetUITempInventory() != NULL &&
			pUIManager->GetUITempInventory()->GetNumberOfItem() > 0 )
			pUIManager->ToggleUI( CX2UIManager::UI_TEMP_INVENTORY, true );
		//}} kimhc	// ??��? ????? ?? ??��? ?????? ??? ???? ??? ?��???
	}


#if X2_DEV_TOOLS
	g_pInstanceData->GetItemCheatPopup().SetStage( (CKTDXStage*) GetNowState() );
	if( true == g_pInstanceData->GetItemCheatPopup().IsOpened() )
	{
		g_pInstanceData->GetItemCheatPopup().OpenItemList( false );
	}
#endif // ITEM_CHEAT_POPUP_TEST


	ReservedServerEventProc();

	// ??? UI ???? ????? ????
	if( NULL != GetPartyUI() )
	{
		GetPartyUI()->OnChangeState();
	}


	if( NULL != g_pInstanceData->GetMiniMapUI() )
	{
		g_pInstanceData->GetMiniMapUI()->SetStage( (CKTDXStage*) GetNowState() );

		switch ( GetNowStateID() )
		{
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_SQUARE_GAME:
		case CX2Main::XS_VILLAGE_MAP:
		case CX2Main::XS_BATTLE_FIELD:
			{
			} break;
		default:
			{
				g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( CX2MiniMapUI::MMT_DUNGEON, false );
			}
			break;
		}
	}



	if( true == g_pData->GetClassChangePopup().GetShow() )
	{
		g_pData->GetClassChangePopup().SetShow( false );
	}



	switch( GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_BATTLE_FIELD:
		{
			GetMemoryHolder()->GetNewYear2014EventDLG( (CKTDXStage*)GetNowState() );
			GetMemoryHolder()->SetShowNewYear2014Event( true );
			GetMemoryHolder()->SetShowNewYear2014EventBoard( false );
		}break;
	default:
		{
			GetMemoryHolder()->GetNewYear2014EventDLG( (CKTDXStage*)GetNowState() );
			GetMemoryHolder()->SetShowNewYear2014Event( false );
		}break;
	}

}



void CX2Main::ReservedServerEventProc()
{
	for( int i=0; i<(int)m_vecReservedServerPacket.size(); i++ )
	{
		ReservedServerPacket& reservedServerPacket = m_vecReservedServerPacket[i];
		switch( reservedServerPacket.m_eServerEventID )
		{
		case EGS_EXPIRE_CASH_SKILL_POINT_NOT:
			{
				switch( GetNowStateID() )
				{
				case CX2Main::XS_DUNGEON_GAME:
				case CX2Main::XS_DUNGEON_RESULT:
				case CX2Main::XS_PVP_GAME:
				case CX2Main::XS_PVP_RESULT:
					{
						continue;	// continue for loop
					} break;
				}


				KEGS_EXPIRE_CASH_SKILL_POINT_NOT* pEvent = (KEGS_EXPIRE_CASH_SKILL_POINT_NOT*) reservedServerPacket.m_pEvent;
				if( NULL != g_pData->GetSkillTree() )
				{
					g_pData->GetSkillTree()->On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( *pEvent );
				}

				SAFE_DELETE( pEvent );
				m_vecReservedServerPacket.erase( m_vecReservedServerPacket.begin() + i );
				i--;
			} break;
//{{ oasis907 : ????? [2009.12.17] //
		case EGS_INVITE_GUILD_NOT:
			{
				switch( GetNowStateID() )
				{
				case CX2Main::XS_DUNGEON_GAME:
				case CX2Main::XS_DUNGEON_RESULT:
				case CX2Main::XS_PVP_GAME:
				case CX2Main::XS_PVP_RESULT:
					{
						continue;	// continue for loop
					} break;
				}



				KEGS_INVITE_GUILD_NOT* pEvent = (KEGS_INVITE_GUILD_NOT*) reservedServerPacket.m_pEvent;
				g_pData->GetGuildManager()->Handler_RESERVED_EGS_INVITE_GUILD_NOT( *pEvent );
				SAFE_DELETE( pEvent );
				m_vecReservedServerPacket.erase( m_vecReservedServerPacket.begin() + i );
				i--;
			} break;
//}} oasis907 : ????? [2009.12.17] //
		}
	}
}

bool CX2Main::DeleteFile( const WCHAR* pFileName )
{
	WCHAR wszFileName[MAX_PATH] = {0};

	wcscpy( wszFileName, pFileName );

	g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( wszFileName );
	if ( wszFileName != NULL )
	{
		::DeleteFile( wszFileName );
		return true;
	}

	return false;
}



			CKTDGUIDialogType CX2Main::KTDGUIOKMsgBox( D3DXVECTOR2 pos, const WCHAR* pText, CKTDXStage* pNowState, int iOKMsg /*= -1*/, float fTimeLeft /*= -1.f*/, wstring wstrFileName /* = L""  */, IN const D3DXVECTOR2 vSize_/* = D3DXVECTOR2( 0, 0 )*/, wstring wstrPopupSoundFileName /* = L"" */ )
{
	if( pNowState == NULL )
		pNowState = m_pNowState;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( true );

		bool bHide = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog();
		if( g_pInstanceData->GetMiniMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( g_pInstanceData->GetMiniMapUI()->GetCurrentMiniMapType(), !bHide );
		}
		if( g_pInstanceData->GetMiniMapUI() != NULL && g_pInstanceData->GetMiniMapUI()->GetWorldMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenWorldMap(false);
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenFieldMap(false);
		}
	}

	DialogLog( pText );


	CKTDGUIDialogType pDialog = NULL;
	if ( wstrFileName != L"")
	{
		pDialog = new CKTDGUIDialog( pNowState, wstrFileName.c_str(), 0.07f, XDL_POP_UP );
		if ( pDialog == NULL )
		{
			ASSERT ( !L"pDialog Is Nothing" );
			pDialog = new CKTDGUIDialog( pNowState,  L"DLG_UI_OKMsgBox.lua", 0.07f, XDL_POP_UP );
		}
	}
	else
	{
		pDialog = new CKTDGUIDialog( pNowState,  L"DLG_UI_OKMsgBox.lua", 0.07f, XDL_POP_UP );
	}
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );

	if ( pos.x == -999 && pos.y == -999 )
	{
		pos = GetWindowPos( D3DXVECTOR2( 430, 148 ), D3DXVECTOR2( 216, 77 ) );
	}

	//debugPrintf( "%f %f", pos.x, pos.y );
	pos.y -= 50.f;
	pDialog->SetPos( pos );
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)pDialog->GetControl( L"StaticOKMsgBoxBG" );
	if( NULL == pStatic )
	{
		ASSERT( !L"pStatic Is Nothing");

		SAFE_DELETE_DIALOG( pDialog );
		return NULL;
	}

	wstring tempText = pText;

	int lineNum = 1;

	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * 366);
	//bool checkEnterChar = false;
	int nowTextLen = 0;

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStatic->GetString(0)->fontIndex );
	if( pFont == NULL )
	{
		SAFE_DELETE_DIALOG( pDialog );
		return NULL;
	}
	lineNum = CWordLineHandler::LineBreakInX2MainMsgBox( tempText, pFont, constTextMaxLen );

	pStatic->GetString(0)->msg = tempText;

	if ( lineNum == 1 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight();
	}
	else if ( lineNum == 2 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight();
	}
	else if ( lineNum == 3 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight()/2.0f;
	}

	CKTDGUIButton* pButtonOK = (CKTDGUIButton*) pDialog->GetControl( L"ButtonMsgBoxOK" );
	if( NULL != pButtonOK )
	{
		if( -1 != iOKMsg )
		{
			pButtonOK->SetCustomMsgMouseUp( iOKMsg );
		}
		pButtonOK->RequestFocus();
	}
	else
	{
		if ( NULL != pButtonOK )
			ASSERT( !L"ButtonMsgBoxOK Is Nothing" );
	}

//#pragma region ??? ???? ??? ????
	if ( !IsSamef( vSize_.x ) || !IsSamef( vSize_.y ) )
	{
		CKTDGUIStatic* pStatic = reinterpret_cast<CKTDGUIStatic*>( pDialog->GetControl( L"General_Popup_Window" ) );

		if ( pStatic )
		{
			CKTDGUIControl::CPictureData* pPictureData = pStatic->GetPicture( 1 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );

			pPictureData = pStatic->GetPicture( 2 );
			if ( pPictureData )
			{
				D3DXVECTOR2 vTemp = pPictureData->GetOriginalPos();
				vTemp.y = vTemp.y + vSize_.y;
				pPictureData->SetSizeX( pPictureData->GetOriginalSize().x + vSize_.x );
				pPictureData->SetPos( vTemp );
			}

			pPictureData = pStatic->GetPicture( 3 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );

			pPictureData = pStatic->GetPicture( 4 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );

			pPictureData = pStatic->GetPicture( 5 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );
		}

		CKTDGUIButton* pButton = reinterpret_cast<CKTDGUIButton*>( pDialog->GetControl( L"ButtonMsgBoxOK" ) );

		if ( pButton )
		{
			D3DXVECTOR2 vTemp = pButton->GetOffsetPos();
			vTemp = vTemp + vSize_;
			pButton->SetOffsetPos( vTemp );
		}
	}
//#pragma endregion X?? ??? ?????? ???????? ????

	if( fTimeLeft > 0.f )
	{
		pDialog->Move( pDialog->GetPos(), D3DXCOLOR( 1, 1, 1, 1 ), fTimeLeft, false, true );
	}

	if( wstrPopupSoundFileName != L"")
		g_pKTDXApp->GetDeviceManager()->PlaySound( wstrPopupSoundFileName.c_str() );
	return pDialog;
}


CKTDGUIDialogType CX2Main::KTDGUIOKMsgBoxPlus( D3DXVECTOR2 pos, const WCHAR* pText, CKTDXStage* pNowState, int iOKMsg /*= -1*/, float fTimeLeft /*= -1.f*/ )
{
	if( pNowState == NULL )
		pNowState = m_pNowState;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( true );

		bool bHide = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog();
		if( g_pInstanceData->GetMiniMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( g_pInstanceData->GetMiniMapUI()->GetCurrentMiniMapType(), !bHide );
		}
		if( g_pInstanceData->GetMiniMapUI() != NULL && g_pInstanceData->GetMiniMapUI()->GetWorldMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenWorldMap(false);
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenFieldMap(false);
		}
	}

	DialogLog( pText );

	//g_pKTDXApp->GetDIManager()->SetEnable(false);
	CKTDGUIDialogType pDialog = new CKTDGUIDialog( pNowState, L"DLG_UI_OKMsgBoxPlus.lua", 0.07f, XDL_POP_UP );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );


	if ( pos.x == -999 && pos.y == -999 )
	{
		pos = GetWindowPos( D3DXVECTOR2( 430, 148 ), D3DXVECTOR2( 216, 77 ) );
	}

	//debugPrintf( "%f %f", pos.x, pos.y );
	pos.y -= 50.f;
	pDialog->SetPos( pos );
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)pDialog->GetControl( L"StaticOKMsgBoxBG" );
	if( NULL == pStatic )
	{
		SAFE_DELETE_DIALOG( pDialog );
		return NULL;
	}

	wstring tempText = pText;

	int lineNum = 1;

	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * 366);
	//bool checkEnterChar = false;
	int nowTextLen = 0;

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStatic->GetString(0)->fontIndex );
	if( pFont == NULL )
	{
		SAFE_DELETE_DIALOG( pDialog );
		return NULL;
	}
	//	CKTDGUIUniBuffer uniBuffer;
	//	uniBuffer.SetFontNode( pFont );

	//{{ 2011/1/12	??????	WordWrap(????) ??? - ???? ??? ????
	lineNum = CWordLineHandler::LineBreakInX2MainMsgBox( tempText, pFont, constTextMaxLen );

	pStatic->GetString(0)->msg = tempText;

	if ( lineNum == 1 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight();
	}
	else if ( lineNum == 2 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight();
	}
	else if ( lineNum == 3 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight()/2.0f;
	}

	CKTDGUIButton* pButtonOK = (CKTDGUIButton*) pDialog->GetControl( L"ButtonMsgBoxOK" );
	if( NULL != pButtonOK )
	{
		if( -1 != iOKMsg )
		{
			pButtonOK->SetCustomMsgMouseUp( iOKMsg );
		}
		pButtonOK->RequestFocus();
	}


	if( fTimeLeft > 0.f )
	{
		pDialog->Move( pDialog->GetPos(), D3DXCOLOR( 1, 1, 1, 1 ), fTimeLeft, false, true );
	}

	return pDialog;
}


CKTDGUIDialogType CX2Main::KTDGUIMsgBox( D3DXVECTOR2 pos, const WCHAR* pText, CKTDXStage* pNowState, wstring wstrFileName /* = L"" */, wstring wstrPopupSoundFileName /* = L"" */ )


{
	if( pNowState == NULL )
		pNowState = m_pNowState;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( true );

		bool bHide = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog();
		if( g_pInstanceData->GetMiniMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( g_pInstanceData->GetMiniMapUI()->GetCurrentMiniMapType(), !bHide );
		}
		if( g_pInstanceData->GetMiniMapUI() != NULL && g_pInstanceData->GetMiniMapUI()->GetWorldMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenWorldMap(false);
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenFieldMap(false);
		}
	}

	DialogLog( pText );

	CKTDGUIDialogType pDialog = NULL;

	if ( wstrFileName != L"")
	{
		pDialog = new CKTDGUIDialog( pNowState, wstrFileName.c_str(), 0.07f, XDL_POP_UP );
		ASSERT(!L"pDialog Is Nothing");
		pDialog = new CKTDGUIDialog( pNowState,  L"DLG_UI_MsgBox.lua", 0.07f, XDL_POP_UP );
	}
	else
	{
		pDialog = new CKTDGUIDialog( pNowState,  L"DLG_UI_MsgBox.lua", 0.07f, XDL_POP_UP );
	}
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );
	pDialog->SetColor( D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	pos.y -= 50.f;
	pDialog->SetPos( pos );
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)pDialog->GetControl( L"StaticMsgBoxBG" );


	if ( NULL == pStatic )
		ASSERT( !L"StaticMsgBoxBG Is Nothing" );


	wstring tempText = pText;

	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * 366);


	//bool checkEnterChar = false;
	int nowTextLen = 0;

//	CKTDGFontManager::CKTDGFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFont( pStatic->GetString(0)->fontIndex );
	//CKTDGUIUniBuffer uniBuffer;
	//uniBuffer.SetFontNode( pFont );

	CKTDGFontManager::CUKFont* pFont = NULL;
	if(pStatic != NULL && pStatic->GetString(0) != NULL) //2011.11.23 lygan_?????? //????? ???????? ?????? ?? ???????
	{
		pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStatic->GetString(0)->fontIndex );
	}
	else
	{
		return NULL;
	}
	int lineNum = CWordLineHandler::LineBreakInX2MainMsgBox( tempText, pFont, constTextMaxLen );

	if ( lineNum == 1 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight();
	}
	else if ( lineNum == 2 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight();
	}
	else if ( lineNum == 3 )
	{
		pStatic->GetString(0)->pos.y += pFont->GetHeight()/2.0f;
	}

	pStatic->GetString(0)->msg = tempText;
	if( wstrPopupSoundFileName != L"")
		g_pKTDXApp->GetDeviceManager()->PlaySound( wstrPopupSoundFileName.c_str() );
	return pDialog;
}



CKTDGUIDialogType CX2Main::KTDGUIOkAndCancelMsgBox( D3DXVECTOR2 pos, const WCHAR* pText, int okMsg, CKTDXStage* pNowState, int cancelMsg /*= -1*/,
													wstring wstrFileName /* = L"" */, IN const D3DXVECTOR2 vSize_ /*= D3DXVECTOR2( 0, 0 )*/, wstring wstrPopupSoundFileName /* = L"" */ )


{
	if ( pNowState == NULL )
		pNowState = m_pNowState;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( true );

		bool bHide = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog();
		if( g_pInstanceData->GetMiniMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( g_pInstanceData->GetMiniMapUI()->GetCurrentMiniMapType(), !bHide );
		}
		if( g_pInstanceData->GetMiniMapUI() != NULL && g_pInstanceData->GetMiniMapUI()->GetWorldMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenWorldMap(false);
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenFieldMap(false);
		}
	}

	DialogLog( pText );

	//g_pKTDXApp->GetDIManager()->SetEnable(false);

	CKTDGUIDialogType pDialog = NULL;
	if ( wstrFileName != L"")
	{
		pDialog = new CKTDGUIDialog( pNowState, wstrFileName.c_str(), 0.07f, XDL_POP_UP );
		if ( pDialog == NULL )
		{
			ASSERT ( !L"pDialog Is Nothing" );
			pDialog = new CKTDGUIDialog( pNowState,  L"DLG_UI_OkAndCancelMsgBox.lua", 0.07f, XDL_POP_UP );
		}
	}
	else
	{
		pDialog = new CKTDGUIDialog( pNowState,  L"DLG_UI_OkAndCancelMsgBox.lua", 0.07f, XDL_POP_UP );
	}


	pDialog->SetOKAndCancelMsgBox( true );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );

	if ( pos.x == -999 && pos.y == -999 )
	{
		pos = GetWindowPos( D3DXVECTOR2( 430, 148 ), D3DXVECTOR2( 217, 71 ) );
	}

	pos.y -= 50.f;
	pDialog->SetPos( pos );
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)pDialog->GetControl( L"StaticMsgBoxBG" );

	if ( NULL == pStatic )
	{
		ASSERT ( !L"StaticMsgBoxBG Is Nothing" );
	}

	wstring tempText = pText;
	//wstring tempText = L"??????? ??��???";

	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * 366);
	//bool checkEnterChar = false;
	int nowTextLen = 0;

	//CKTDGFontManager::CKTDGFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFont( pStatic->GetString(0)->fontIndex );
	//CKTDGUIUniBuffer uniBuffer;
	//uniBuffer.SetFontNode( pFont );
	if ( NULL != pStatic->GetString(0) )

	{
		CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStatic->GetString(0)->fontIndex );
	int lineNum = CWordLineHandler::LineBreakInX2MainMsgBox( tempText, pFont, constTextMaxLen );
		pStatic->GetString(0)->msg = tempText;

		if ( lineNum == 1 )
		{
			pStatic->GetString(0)->pos.y += pFont->GetHeight();
		}
		else if ( lineNum == 2 )
		{
			pStatic->GetString(0)->pos.y += pFont->GetHeight();
		}
		else if ( lineNum == 3 )
		{
			pStatic->GetString(0)->pos.y += pFont->GetHeight()/2.0f;
		}
	}

	CKTDGUIButton* pOkButton = (CKTDGUIButton*)pDialog->GetControl( L"MsgBoxOkAndCancelOkButton" );
	if ( pOkButton == NULL )
	{
		ASSERT( !L"pOkButton Is Nothing");
	}
	pOkButton->SetCustomMsgMouseUp( okMsg );
	pOkButton->RequestFocus();

	if( -1 != cancelMsg )
	{
		CKTDGUIButton* pCancelButton = (CKTDGUIButton*)pDialog->GetControl( L"MsgBoxOkAndCancelCancelButton" );
		if ( pCancelButton == NULL )
		{
			ASSERT( !L"pCancelButton Is Nothing");
		}
		pCancelButton->SetCustomMsgMouseUp( cancelMsg );
	}

	//{{ JHKang // ?????? // 2010.10.04
	if ( NULL != g_pChatBox && true == g_pChatBox->GetFocusChatEditBox() )
	{
		pDialog->SetModal( true );
		g_pChatBox->OpenChatWindow();

		m_bIsPopUpOnChat = true;
	}
	//}} JHKang // ?????? // 2010.10.04

//#pragma region ??? ???? ??? ????
	if ( !IsSamef( vSize_.x ) || !IsSamef( vSize_.y ) )
	{
		CKTDGUIStatic* pStatic = reinterpret_cast<CKTDGUIStatic*>( pDialog->GetControl( L"General_Popup_Window" ) );

		if ( pStatic )
		{
			CKTDGUIControl::CPictureData* pPictureData = pStatic->GetPicture( 1 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );

			pPictureData = pStatic->GetPicture( 2 );
			if ( pPictureData )
			{
				D3DXVECTOR2 vTemp = pPictureData->GetOriginalPos();
				vTemp.y = vTemp.y + vSize_.y;
				pPictureData->SetSizeX( pPictureData->GetOriginalSize().x + vSize_.x );
				pPictureData->SetPos( vTemp );
			}

			pPictureData = pStatic->GetPicture( 3 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );

			pPictureData = pStatic->GetPicture( 4 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );

			pPictureData = pStatic->GetPicture( 5 );
			if ( pPictureData )
				pPictureData->SetSize( pPictureData->GetOriginalSize() + vSize_ );
		}

		CKTDGUIButton* pButton = reinterpret_cast<CKTDGUIButton*>( pDialog->GetControl( L"MsgBoxOkAndCancelOkButton" ) );

		if ( pButton )
		{
			D3DXVECTOR2 vTemp = pButton->GetOffsetPos();
			vTemp = vTemp + vSize_;
			pButton->SetOffsetPos( vTemp );
		}

		pButton = reinterpret_cast<CKTDGUIButton*>( pDialog->GetControl( L"MsgBoxOkAndCancelCancelButton" ) );

		if ( pButton )
		{
			D3DXVECTOR2 vTemp = pButton->GetOffsetPos();
			vTemp = vTemp + vSize_;
			pButton->SetOffsetPos( vTemp );
		}
	}
//#pragma endregion X?? ??? ?????? ???????? ????

	if( wstrPopupSoundFileName != L"")
	g_pKTDXApp->GetDeviceManager()->PlaySound( wstrPopupSoundFileName.c_str() );

	return pDialog;
}

CKTDGUIDialogType	CX2Main::KTDGUIOkAndCancelEditBox( D3DXVECTOR2 vPos, const WCHAR* pText, int iOkMsg, CKTDXStage* pNowState, int iLimitLength, bool bIsByte, int iCancelMsg )
{
	if ( pNowState == NULL )
		pNowState = m_pNowState;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( true );

		bool bHide = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog();
		if( g_pInstanceData->GetMiniMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( g_pInstanceData->GetMiniMapUI()->GetCurrentMiniMapType(), !bHide );
		}
		if( g_pInstanceData->GetMiniMapUI() != NULL && g_pInstanceData->GetMiniMapUI()->GetWorldMapUI() != NULL )
		{
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenWorldMap(false);
			g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenFieldMap(false);
		}
	}

	DialogLog( pText );

	CKTDGUIDialogType pDialog = new CKTDGUIDialog( pNowState, L"DLG_UI_OkAndCancelEditBox.lua", 0.07f ,XDL_POP_UP );

	if( pDialog == NULL )
	{
		return NULL;
	}

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );

	if ( vPos.x == -999 && vPos.y == -999 )
	{
		vPos = GetWindowPos( D3DXVECTOR2( 430, 148 ), D3DXVECTOR2( 217, 71 ) );
	}

	vPos.y -= 50.f;
	pDialog->SetPos( vPos );

	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)pDialog->GetControl( L"Static_Notice" );

	if( pStatic == NULL )
	{
		return NULL;
	}

	pStatic->SetString(0, pText);

	CKTDGUIIMEEditBox* pEditBox = (CKTDGUIIMEEditBox*) pDialog->GetControl( L"IMEEditBoxMessage" );

	if( pEditBox == NULL )
	{
		return NULL;
	}

	if( true == bIsByte )	//	????? ????
	{
		pEditBox->SetByteLimit_LUA( iLimitLength );
	}
	else	//	????????
	{
		pEditBox->SetLengthLimit_LUA( iLimitLength );
	}

	pEditBox->SetCustomMsgEnter( iOkMsg );

	//	OK, Cancel ???? ????
	CKTDGUIButton* pOkButton = (CKTDGUIButton*) pDialog->GetControl( L"Button_Check" );
	if( pOkButton == NULL )
	{
		return NULL;
	}
	pOkButton->SetCustomMsgMouseUp( iOkMsg );

	CKTDGUIButton* pCancelButton = (CKTDGUIButton*) pDialog->GetControl( L"Cancel" );
	if( pCancelButton == NULL )
	{
		return NULL;
	}

	pCancelButton->SetCustomMsgMouseUp( iCancelMsg );

	pDialog->SetCloseCustomUIEventID( iCancelMsg );

	return pDialog;
}




CKTDGUIDialogType CX2Main::KTDGUIOkAndCancelEditBox2( D3DXVECTOR2 vPos, const WCHAR* pText, int iOkMsg, CKTDXStage* pNowState, int iLimitLength, bool bIsByte, int iCancelMsg )
{
	CKTDGUIDialogType pDialog = new CKTDGUIDialog( pNowState, L"DLG_UI_OkAndCancelEditBox2.lua", 0.07f ,XDL_POP_UP );

	if( pDialog == NULL )
	{
		return NULL;
	}

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );

	if ( vPos.x == -999 && vPos.y == -999 )
	{
		vPos = GetWindowPos( D3DXVECTOR2( 430, 148 ), D3DXVECTOR2( 217, 71 ) );
	}

	vPos.y -= 50.f;
	pDialog->SetPos( vPos );

	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)pDialog->GetControl( L"Static_Notice" );

	if( pStatic == NULL )
	{
		return NULL;
	}

	pStatic->SetString(0, pText);

	CKTDGUIIMEEditBox* pEditBox = (CKTDGUIIMEEditBox*) pDialog->GetControl( L"IMEEditBoxMessage" );

	if( pEditBox == NULL )
	{
		return NULL;
	}

	if( true == bIsByte )	//	????? ????
	{
		pEditBox->SetByteLimit_LUA( iLimitLength );
	}
	else	//	????????
	{
		pEditBox->SetLengthLimit_LUA( iLimitLength );
	}

	pEditBox->SetCustomMsgEnter( iOkMsg );

	//	OK, Cancel ???? ????
	CKTDGUIButton* pOkButton = (CKTDGUIButton*) pDialog->GetControl( L"Button_Check" );
	if( pOkButton == NULL )
	{
		return NULL;
	}
	pOkButton->SetCustomMsgMouseUp( iOkMsg );

	CKTDGUIButton* pCancelButton = (CKTDGUIButton*) pDialog->GetControl( L"Cancel" );
	if( pCancelButton == NULL )
	{
		return NULL;
	}

	pCancelButton->SetCustomMsgMouseUp( iCancelMsg );

	pDialog->SetCloseCustomUIEventID( iCancelMsg );

	return pDialog;
}

CKTDGUIDialog* CX2Main::KTDGUIOKMsgBox_LUA( D3DXVECTOR2 pos, const char* pText, CKTDXStage* pNowState )
{
	wstring text;
	ConvertUtf8ToWCHAR( text, pText );
	CKTDGUIDialogType pDlg = KTDGUIOKMsgBox( pos, text.c_str(), pNowState, -1 );
    return CKTDGUIDialog::GetObjectByUID( pDlg.GetHandle() );
}

CKTDGUIDialog* CX2Main::KTDGUIOkAndCancelMsgBox_LUA( D3DXVECTOR2 pos, const char* pText, int okMsg, CKTDXStage* pNowState )
{
	wstring text;
	ConvertUtf8ToWCHAR( text, pText );
	CKTDGUIDialogType pDlg = KTDGUIOkAndCancelMsgBox( pos, text.c_str(), okMsg, pNowState );
    return CKTDGUIDialog::GetObjectByUID( pDlg.GetHandle() );
}

	bool CX2Main::IsWaitingServerPacket( const SERVER_EVENT_ID waitACK )
	{
		for( UINT i=0; i<m_WaitServerPacketList.size(); i++ )
		{
			WaitServerPacket* pPacket = m_WaitServerPacketList[i];
			if( NULL != pPacket &&
				waitACK == pPacket->waitACK )
			{
				return true;
			}
		}

		return false;
	}

void CX2Main::AddServerPacket( SERVER_EVENT_ID waitACK, float fReamainTimeOut /* = 0 */ )
{

#ifdef _DEBUG
	fReamainTimeOut += 99999.0f;
#endif

	if ( fReamainTimeOut == 0 )
	{
		fReamainTimeOut = GetServerTimeOut();
	}

	WaitServerPacket* pWaitServerPacket	= new WaitServerPacket;
	pWaitServerPacket->waitACK			= waitACK;
	pWaitServerPacket->fReamainTimeOut	= fReamainTimeOut;

	m_WaitServerPacketList.push_back( pWaitServerPacket );



#if X2_DEV_TOOLS

	map<int, int>::iterator it;
	it = m_mapWaitServerPacketList.find( (int) waitACK );
	if( it != m_mapWaitServerPacketList.end() )
	{
		if( it->second > 0 )
		{
			WCHAR wszMsg[512] = L"";
			swprintf( wszMsg, sizeof(wszMsg)/sizeof(WCHAR)-1, L"??? %d.%d", it->first, it->second );
			StateLog( wszMsg );
		}

		it->second += 1;
	}
	else
	{
		m_mapWaitServerPacketList[ (int) waitACK ] = 1;
	}

#endif // _SERVICE_

}

bool CX2Main::DeleteServerPacket( SERVER_EVENT_ID waitACK )
{
	for( int i = 0; i < (int)m_WaitServerPacketList.size(); i++ )
	{
		WaitServerPacket* pWaitServerPacket = m_WaitServerPacketList[i];
		if( pWaitServerPacket->waitACK == waitACK )
		{

#if X2_DEV_TOOLS

			map<int, int>::iterator it;
			it = m_mapWaitServerPacketList.find( (int) waitACK );
			if( it != m_mapWaitServerPacketList.end() )
			{
				it->second -= 1;
			}

#endif // _SERVICE_

			SAFE_DELETE( pWaitServerPacket );
			m_WaitServerPacketList.erase( m_WaitServerPacketList.begin() + i );
			return true;
		}
	}
	return false;
}

void CX2Main::AllDeleteServerPacket()
{
	for( int i = 0; i < (int)m_WaitServerPacketList.size(); i++ )
	{
		WaitServerPacket* pWaitServerPacket = m_WaitServerPacketList[i];
		SAFE_DELETE( pWaitServerPacket );
	}
	m_WaitServerPacketList.clear();
}

void CX2Main::AddChannelServerIP( const WCHAR* pServerIP )
{
	if( NULL == pServerIP )
		return;

	m_vecChannelServerIP.push_back( pServerIP );
};

void CX2Main::AddChannelServerIP_LUA( const char* pServerIP, int iServerPort /*= 9400*/ )
{
	if( NULL == pServerIP )
		return;

	struct hostent* host = NULL;
	wstring wstrChannelServerIP = L"";
	host = gethostbyname(pServerIP);

	if( host != NULL )
	{
		ConvertUtf8ToWCHAR( wstrChannelServerIP, inet_ntoa(*(struct in_addr*)host->h_addr_list[0]) );
	}
	else
	{
		ConvertUtf8ToWCHAR( wstrChannelServerIP, pServerIP );
	}

	m_DefaultChannelServerInfo.m_kServerIP = wstrChannelServerIP;

	// Config Lua ???????? ???????? ???? ??? ??????
	// ?? ?????? ???????? 0???? ??????? ???????? ??????
	// AddChannelServerIP_LUA ????? ?????? ??????? ?????? ???? ?????? ???? ??
	// ?????? ??????? ?��?? ???? ???
	static int iServerSetID = 0;

	KServerGroupInfo kTmpServerGroupInfo;
	kTmpServerGroupInfo.m_iOrder = iServerSetID;
	kTmpServerGroupInfo.m_bEnable = true;
	kTmpServerGroupInfo.m_iServerGroupUID = iServerSetID;
	kTmpServerGroupInfo.m_kServerIP = wstrChannelServerIP;
	kTmpServerGroupInfo.m_usMasterPort = iServerPort;
	g_pInstanceData->m_mapServerGroupInfo.insert(std::make_pair(iServerSetID, kTmpServerGroupInfo));

	iServerSetID++;
}

void CX2Main::SetSafetyKey_LUA( const char* pKey )
{
	ConvertUtf8ToWCHAR( m_SafetyKey, pKey );
}

void CX2Main::SetClientVersion_LUA( char* pVersion )
{
	ConvertUtf8ToWCHAR( m_ClientVersion, pVersion );
}

void CX2Main::SetSubClientVersion_LUA( char* pVersion )
{
	ConvertUtf8ToWCHAR( m_SubClientVersion, pVersion );
}

void CX2Main::ChangeKoreanAndEnglishKey( bool bKorean )
{
	HIMC haneng;
	haneng = ImmGetContext(g_pKTDXApp->GetHWND());
	ImmSetConversionStatus(haneng,bKorean,0);
}

//{{ kimhc // 2010.3.12 //	???? ????
bool CX2Main::GetNativeAndEnglisKeyState() //???? ??? ??? ??????	// TRUE ?? ???
{
	bool bRet = false;
	HIMC hIMC = NULL;
	if ( hIMC = ImmGetContext( g_pKTDXApp->GetHWND() ) )
	{
		DWORD fdwConversion, fdwSentence;

		BOOL bRet2 = ImmGetConversionStatus(hIMC, &fdwConversion, &fdwSentence);

		if( bRet2 )
		{
			bRet = ( fdwConversion & IME_CMODE_NATIVE ? true : false );
		}

		ImmReleaseContext( g_pKTDXApp->GetHWND(), hIMC );
	}

	return bRet;
}
//}} kimhc // 2010.3.12 //	???? ????

//{{ kimhc // 2010.9.14 // ??????? ???????? ???? ????? ??????? ???? ��???? ????? ��?? ??? ????
//}} kimhc // 2010.9.14 // ??????? ???????? ???? ????? ??????? ???? ��???? ????? ��?? ??? ????

void CX2Main::ResetCrashReport( const WCHAR* wszAddMsg )
{

	string clientVersion = "";

#if !X2_DEV_TOOLS
		clientVersion = "[CRASH_REPORT_SERVICE] ";
#else
	clientVersion = "[CRASH_REPORT_TEST] ";
#endif

	string clientMainVersion = "";
	ConvertWCHARToChar( clientMainVersion, GetClientVersion() );
	clientVersion += "_(";
	clientVersion += clientMainVersion;
	string subClientVersion = "";
	ConvertWCHARToChar( subClientVersion, GetSubClientVersion_LUA() );
	if( subClientVersion.length() > 0 )
	{
		clientVersion += "_";
		clientVersion += subClientVersion;
	}
	clientVersion += ")_";

	string userID = "";
	if( NULL != g_pData && NULL != g_pData->GetMyUser() )
	{
		ConvertWCHARToChar( userID, g_pData->GetMyUser()->GetUserData().userID.c_str() );
	}

	string unitName = "";
	if( NULL != g_pData && NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit()->GetNickName() )
	{
		ConvertWCHARToChar( unitName, g_pData->GetMyUser()->GetSelectUnit()->GetNickName() );
	}

	clientVersion += "_";
	clientVersion += userID;
	clientVersion += "_";
	clientVersion += unitName;
	clientVersion += "_";

	if( NULL != wszAddMsg )
	{
		string szAddMsg;
		ConvertWCHARToChar( szAddMsg, wszAddMsg );
		clientVersion += szAddMsg;
	}

	if( NULL != m_pCrashReportState )
		SetEMailSubjectPrefix( m_pCrashReportState, (LPCTSTR)clientVersion.c_str() );
}

bool CX2Main::SendQuitMsgToServer()
{
	char szErrorLogFileName[256] = "ErrorLog.txt";
	KEGS_CLIENT_QUIT_REQ kPacket;


	if ( g_pData != NULL && g_pData->GetServerProtocol() != NULL )
	{
		kPacket.m_iReason = m_QuitType;
		g_pData->GetServerProtocol()->SendPacket( EGS_CLIENT_QUIT_REQ, kPacket );
	}

	return true;
}

bool CX2Main::DeleteFile_LUA( const char* pFileName )
{
	wstring fileName;
	ConvertUtf8ToWCHAR( fileName, pFileName );

	return DeleteFile( fileName.c_str() );
}


void CX2Main::ResetFullScreenResolution(int iWidth, int iHeight)
{
	// DXUTSetFullscreenClientRectAtModeChange()?? ???? DXUT?? ???? ?????. ????? ???????.
	// fix! ?�� ????? ????? ?????. ??? createdevice() ??? changedevice()?? ?????? ?? ?? ????.
	// ?????? ?????? ?? fullscreen???? ??????? ?? ????? ??? ????? ????. winmain???? createdevice?? ?? ???? ???????.

	if( DXUTIsWindowed() )
	{
		DXUTSetFullscreenClientRectAtModeChange(iWidth, iHeight);
	}
	else
	{
		//DXUTSetWindowClientRectAtModeChange(iWidth,iHeight);
		//DXUTDeviceSettings deviceSettings = DXUTGetDeviceSettings();
		//deviceSettings.pp.Windowed = !deviceSettings.pp.Windowed;
		//DXUTSetFullscreenClientRectAtModeChange(iWidth, iHeight);
	}
}

bool CX2Main::IsValidPacket( int enumID )
{
	//////////////////////////////////////////////////////////////////////////
	/// ????? ??? ???? ?????? ?????
	//////////////////////////////////////////////////////////////////////////
	switch( enumID )
	{
	//// true ??????? ???.
	case NetError::NET_OK:
	case NetError::ERR_SLOT_07:
	case NetError::NOT_LEAVE_FIELD_REASON_00: //??? ???? ?????? ???? ???????.
	case NetError::NOT_LEAVE_ROOM_REASON_27: //?????? ?????? ???? ?? ??? ????.
		return true;
		break;

	//// false ??????? ???
	case NetError::ERR_REFRESH_01:
	case NetError::ERR_PARTY_03:		// ????????? ???????????.
	case NetError::ERR_ROOM_04:			// ??��?? ????? ?? ???? ??????????
	case NetError::ERR_ROOM_21:			// ??? ???? NPC ???? ????? ??��?.
	case NetError::ERR_KNM_06:			// ?????? ??��????????? ??? ????
	case NetError::ERR_RESURRECT_01:	// ?????? '??? ??????????' ????? ?????????
	case NetError::ERR_WRONG_STATE_00:	// ???? ???? ??? : GSFSM ???��? ????
	case NetError::ERR_ITEM_12:			// ??? ?��??? ???????? ?��????? ???? ?????? ????????.
	case NetError::ERR_BUY_CASH_ITEM_42:
	case NetError::ERR_TITLE_01:
	case NetError::ERR_PERSONAL_SHOP_37:
	case NetError::ERR_BUY_CASH_ITEM_67:
	case NetError::ERR_FIELD_05:		//??? ???? ???? ????????? ???? ???
	case NetError::ERR_FIELD_06:
	case NetError::ERR_BATTLEFIELD_13:
	case NetError::ERR_BATTLEFIELD_14:
	case NetError::ERR_RELATIONSHIP_25:
	case NetError::ERR_DELETE_UNIT_02:
	case NetError::ERR_RESTORE_UNIT_02:
		return false;

	case NetError::ERR_ROOM_27:			// ?????��?? ?????? ???? ????? ????? ???
		{
			if( NULL != g_pData &&
				NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() &&
				true == g_pData->GetMyUser()->GetSelectUnit()->GetMyInfoInDungeonRoom())
			{
				return false;
			}

			// ???????? ????? ?????????? ???? ??? ???????? ????.
		}
		break;
	case NetError::ERR_SORT_CATEGORY_01:
		{
			g_pChatBox->AddChatLog( NetError::GetErrStrF( NetError::ERR_SORT_CATEGORY_01 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
			return true;
		} break;
	// ??? ?????? ??????? ???? ????? ???????? ??? ?????? default???? return ??? ???.
	case NetError::ERR_VERIFY_00:
	case NetError::ERR_VERIFY_01:
		{
			//????? ????? ??????? ??????? ???? ???? ????? ???? ???? ??????? ????? ????? ????? ???? ??????.
			return false;
		}break;
	default:
		break;
	}

	//////////////////////////////////////////////////////////////////////////
	/// ????? ??? ?????? ?????
	//////////////////////////////////////////////////////////////////////////
	std::wstringstream wstrStream;
	wstrStream << L"IsValidPacket" << L"_" << NetError::GetErrStrF(enumID);

#if X2_DEV_TOOLS
	if( enumID >= NetError::ERR_SENTINEL || enumID < 0 )
		wstrStream << L"(" << enumID << L")";
#endif //_SERVICE_

	string strErr;
	ConvertWCHARToChar( strErr, wstrStream.str().c_str() );

	switch( enumID )
	{
		//// true ??????? ???.
	case NetError::ERR_SHUT_DOWN_00:
	case NetError::ERR_SHUT_DOWN_01:
	case NetError::ERR_SHUT_DOWN_02:
	case NetError::ERR_SHUT_DOWN_03:
	case NetError::ERR_SHUT_DOWN_04:
	case NetError::ERR_SHUT_DOWN_05:
		{
			CKTDGUIDialogType pDLGOK =
				KTDGUIOKMsgBox( D3DXVECTOR2(250,300), NetError::GetErrStrF(enumID),
								m_pNowState, CX2StateCreateUnit::SCUUCM_SHUT_DOWN_OK );
			return true;
		} break;
	case NetError::ERR_NX_AUTH_00:
		{
			CKTDGUIDialogType pDLGOK =
				KTDGUIOKMsgBox( D3DXVECTOR2(250,300), NetError::GetErrStrF(enumID),
								m_pNowState, CX2CashShop::CSUCM_CHECK_AUTHORITY_OK );
			return true;
		} break;



		//// false ??????? ???.
	case NetError::ERR_ROOM_09:
		{
			ErrorLogMsg( XEM_ERROR114, strErr.c_str() );
			return false;
		}
		break;
	case NetError::ERR_USE_ITEM_IN_INVENTORY_06:
		{
			if( NULL != g_pData->GetUIManager() &&
				NULL != g_pData->GetUIManager()->GetUIInventory() )
			{
				CKTDGUIDialogType pDLGOK =
					KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_16527 ),
					CX2UIInventory::UIM_ITEM_USE_DEFENCE_DUNGEON_POTION,
					GetNowState() );
			}
			return false;
		} break;
	case NetError::ERR_UDP_01:
		{
			KTDGUIOKMsgBoxPlus( D3DXVECTOR2(250,300), GET_STRING(STR_ID_24041), m_pNowState );
			return false;
		} break;

	case NetError::ERR_RELATIONSHIP_05 :
		{
			CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
			userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE, userData );
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING ( STR_ID_24603 ), m_pNowState );
		}
		break;

	case NetError::ERR_RELATIONSHIP_40 :
		{
			return true;
		}
		break;

	case NetError::ERR_RELATIONSHIP_06 :
		{
			CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
			userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE, userData );
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING ( STR_ID_24455 ), m_pNowState );

		}
		break;
	case NetError::ERR_RELATIONSHIP_03 :
		{
			CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
			userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE, userData );
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING ( STR_ID_24456 ), m_pNowState );
		}
		break;
	case NetError::ERR_RELATIONSHIP_00 :
		{
			CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
			userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE, userData );
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING ( STR_ID_24458 ), m_pNowState );
		}
		break;
	case NetError::ERR_RELATIONSHIP_24 :
		{
			return true;
		}
		break;
	case NetError::ERR_RELATIONSHIP_08 :
	case NetError::ERR_RELATIONSHIP_02 :
	case NetError::ERR_RELATIONSHIP_04 :
	case NetError::ERR_RELATIONSHIP_01 :
	case NetError::ERR_RELATIONSHIP_07 :
		{
			CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
			userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE, userData );
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), NetError::GetErrStrF(enumID), m_pNowState );
			return false;
		} break;

	case NetError::ERR_RELATIONSHIP_47 :
		{
			CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
			userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			RemoveTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE, userData );
			KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING ( STR_ID_26240 ), m_pNowState );
		} break;

	case NetError::ERR_SECOND_SECURITY_DIFFER_PW:
	case NetError::ERR_GUILD_35:
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), NetError::GetErrStrF(enumID), m_pNowState, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
			ErrorLogMsg( XEM_ERROR77, strErr.c_str() );
		} break;

	case NetError::ERR_CONNECT_04 :
		{
			return false;
		} break;

	default:
		{
			if ( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
			{
				std::wstring wstrErrorMsg = boost::str( boost::wformat( L"NetError(%1%)\n%2%\n%3%" ) % enumID % NetError::GetErrStrF(enumID) % NetError::GetErrStr(enumID) );
				KTDGUIOKMsgBox( D3DXVECTOR2(250,300), wstrErrorMsg.c_str(), m_pNowState );
			}
			else
			{
				KTDGUIOKMsgBox( D3DXVECTOR2(250,300), NetError::GetErrStrF(enumID), m_pNowState );
			}
			ErrorLogMsg( XEM_ERROR77, strErr.c_str() );
			return false;
		} break;
	}

	return false;
}


bool CX2Main::InstallFont( const char* pValueName, const char* pFontFileName )
{
	g_pKTDXApp->GetDGManager()->GetFontManager()->InstallFont( pValueName, pFontFileName );

	WCHAR FontPath[MAX_PATH] = {0,};
	WCHAR tempFontFileName[MAX_PATH] = {0,};
	WCHAR FontFileName[MAX_PATH] = {0,};
	WCHAR FilePath[MAX_PATH] = {0,};

	wstring fontFileName;
	wstring realFontFileName;
	ConvertUtf8ToWCHAR( fontFileName, pFontFileName );
	ConvertUtf8ToWCHAR( realFontFileName, pFontFileName );

	StringCchPrintf( FontFileName, MAX_PATH , L"%s", fontFileName.c_str() );
	StringCchPrintf( tempFontFileName, MAX_PATH , L"%s", fontFileName.c_str() );
	/*wsprintf( FontFileName, L"%s", fontFileName.c_str() );
	wsprintf( tempFontFileName, L"%s", fontFileName.c_str() );*/

	if ( g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( tempFontFileName ) == NULL )
		return false;

	StringCchPrintf( FilePath, MAX_PATH , g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( FontFileName ) );
	/*wsprintf( FilePath, g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->GetPullPath( FontFileName ) );*/
	/*

	GetWindowsDirectory( FontPath, MAX_PATH );
	wcscat( FontPath, L"\\Fonts\\" );
	wcscat( FontPath, realFontFileName.c_str() );

	string strFontPath;
	ConvertWCHARToChar( strFontPath, FontPath );

	if ( _access( strFontPath.c_str(), 0 ) != 0 )
	{
		if ( CopyFile( FilePath, FontPath, FALSE ) == FALSE )
		{
			//InvalidateRect( g_pKTDXApp->GetHWND(), NULL, TRUE );
			return false;
		}
	}
	*/

	if ( AddFontResource( FilePath ) == FALSE )
	{
		//KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"Font Install Fail", m_pNowState );
		ErrorLog(XEM_ERROR16);
		return false;

	}

//	SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0 );

	m_UseFontNameList.push_back( FilePath );


/*

	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	WCHAR szKey[255] = {0};
	GetVersionEx( &vi );

	debugWPrintf( L"InstallFont8" );

	if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		wcscpy( szKey, L"SoftWare\\Microsoft\\Windows NT\\CurrentVersion\\Fonts" );
	}
	else
	{
		wcscpy( szKey, L"SoftWare\\Microsoft\\Windows\\CurrentVersion\\Fonts" );
	}

	debugWPrintf( L"InstallFont9" );

	//SHRegWriteString()
	//SHRegSetUSValue( szKey, , REG_SZ, , , HKEY_LOCAL_MACHINE );

	HKEY key;

	DWORD dwDisp;
	WCHAR lpData[256];
	wsprintf( lpData, L"%s", fontFileName.c_str() );

	wstring valueName;
	ConvertCharToWCHAR( valueName, pValueName );

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, szKey,0,NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)
		!=ERROR_SUCCESS)
	{
		KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"Font Install Fail", m_pNowState );
		ErrorLog(XEM_ERROR17);
		return false;
	}



	if(RegSetValueEx(key, valueName.c_str(), 0, REG_SZ, (LPBYTE)lpData,  sizeof(WCHAR) * (int)wcslen(lpData) )
		!=ERROR_SUCCESS)
	{
		ErrorLog(XEM_ERROR18);
		KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"Font Install Fail", m_pNowState );
		return false;
	}

	RegCloseKey(key);
*/
	return true;

}

void CX2Main::BuyItemSuper_LUA( int itemID, int iQuantity /*= 1*/, short sPeriod /*= 0*/ )
{
	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{

		switch( GetNowStateID() )
		{
		case CX2Main::XS_VILLAGE_MAP: // ???????
		case CX2Main::XS_BATTLE_FIELD:
			{
				if( iQuantity <= 0 )
					iQuantity = 1;

				KEGS_BUY_ED_ITEM_REQ kEGS_BUY_GP_ITEM_REQ;
				kEGS_BUY_GP_ITEM_REQ.m_bDebug = true;
				kEGS_BUY_GP_ITEM_REQ.m_sPeriod = sPeriod;
				kEGS_BUY_GP_ITEM_REQ.m_mapItem.insert( std::make_pair( itemID, iQuantity ) );

				// 09.07.03 ?��? : ACK ?????? Class?? UIShop???? ??? ?????? ?????????.
				if(g_pData->GetUIManager()->GetUIShop() == NULL)
				{
					g_pData->GetUIManager()->CreateUIShop();
				}

				g_pData->GetServerProtocol()->SendPacket( EGS_BUY_ED_ITEM_REQ, kEGS_BUY_GP_ITEM_REQ );
				AddServerPacket( EGS_BUY_ED_ITEM_ACK );

			} break;
		}
	}
}


void CX2Main::SetAuthLevel_LUA( const CHAR* userID, CX2User::X2_USER_AUTH_LEVEL level )
{
#if X2_DEV_TOOLS
	if( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_OPERATOR )
		return;

	wstring wstrUserID = L"";
	ConvertUtf8ToWCHAR( wstrUserID, userID );
	CX2State* pNowState = (CX2State*) GetNowState();
	pNowState->Handler_EGS_ADMIN_CHANGE_AUTH_LEVEL_REQ( wstrUserID.c_str(), level );
#endif
}

void CX2Main::GetAuthUserList_LUA( CX2User::X2_USER_AUTH_LEVEL level )
{

	CX2State* pNowState = (CX2State*) GetNowState();
	pNowState->Handler_EGS_ADMIN_GET_AUTH_LEVEL_LIST_REQ( level );

}

void CX2Main::AddDungeonRewardItem( std::vector<KItemInfo>& vecDungeonRewardItem )
{
	m_vecDungeonRewardItem.insert( m_vecDungeonRewardItem.end(),
		vecDungeonRewardItem.begin(), vecDungeonRewardItem.end() );
}

void CX2Main::Handler_EGS_CHECK_SUM_REQ( bool bFileCheck /* = false */ )
{
	if ( m_pCheckSumManager != NULL )
		m_pCheckSumManager->Handler_EGS_CHECK_SUM_REQ( bFileCheck );
}
//{{????? ?????��? ??????

wstring CX2Main::GetEDString( int edToChange )
{
	WCHAR buff[256] = {0};
	_itow( edToChange, buff, 10 );

	return GetEDString( buff );
}
wstring CX2Main::GetEDString( const WCHAR* pEDToChange )
{
	wstringstream edString;
	wstring edToChange = pEDToChange;

	int commaCount = 0;
	for( int i = edToChange.size()-1; i >=0; i-- )
	{
		if( commaCount == 3 )
		{
			edString << L",";
			commaCount = 0;
		}
		edString << edToChange.at(i);
		commaCount++;
	}
	edToChange = edString.str().c_str();


	std::reverse( edToChange.begin(), edToChange.end() );

	return edToChange;
}


// ED ??????? ????? ???, ????? ','?? ????
int CX2Main::GetEDFromString( const WCHAR* wszED )
{
	if( NULL == wszED )
		return 0;

	wstring wstrED = wszED;
	for( UINT i=0; i<wstrED.size(); ++i )
	{
		if( wstrED[i] == ',' )
		{
			wstrED.erase( wstrED.begin() + i );
			i--;
		}
	}

	return (int) wcstol( wstrED.c_str(), NULL, 10 );
}

//{{ ????? : [2009/11/5] //	???? ???
D3DXCOLOR	CX2Main::GetPriceColor( __int64 iPrice )
{
	D3DXCOLOR color;

	if( iPrice < 10000 )	//	??
	{
		color = D3DXCOLOR( 0.9f, 1.0f, 0.1f, 1.0f );
	}
	else if ( iPrice < 100000 )	//	??
	{
		color = D3DXCOLOR( 0.5f, 1.0f, 0.0f, 1.0f );
	}
	else if ( iPrice < 1000000 )	//	???
	{
		color = D3DXCOLOR( 1.0f, 0.5f, 0.0f, 1.0f );
	}
	else if ( iPrice < 10000000 )	// ??
	{
		color = D3DXCOLOR( 0.0f, 1.0f, 1.0f, 1.0f );
	}
	else if ( iPrice < 100000000 )	// ????
	{
		color = D3DXCOLOR( 0.55f, 0.55f, 0.96f, 1.0f );
	}
	else	//	????
	{
		color = D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f );
	}

	return color;
}

//}} ????? : [2009/11/5] //	???? ??? ???




void CX2Main::InitMyUnitSkillTree()
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		CX2State* pState = (CX2State*) GetNowState();
		pState->Handler_EGS_ADMIN_INIT_SKILL_TREE_REQ();
	}
}


void CX2Main::SetMyUnitSP( int iSP )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}


	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		CX2State* pState = (CX2State*) GetNowState();
		pState->Handler_EGS_ADMIN_CHANGE_SKILL_POINT_REQ( iSP );
	}
}


void CX2Main::FullMyUnitSkillTree()
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		CX2State* pState = (CX2State*) GetNowState();
		pState->Handler_EGS_ADMIN_CHEAT_GET_ALL_SKILL_REQ();
	}
}

/** @function	: FullMyUnitSkillTree
	@brief		: ???? ?????? ?��? ?? ??? ??? ????
*/
void CX2Main::FullMyUnitSkillTreeByLevel()
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		CX2State* pState = (CX2State*) GetNowState();
		pState->Handler_EGS_ADMIN_AUTO_GET_ALL_SKILL_REQ();
	}
}

/** @function	: SetMyUnitSkillLevel
	@brief		: ??? ????? ?????? ????
*/
void CX2Main::SetMyUnitSkillLevel( const int iSkillID, const int iSkillLevel )
{
	if( NULL == g_pData ||
		NULL == g_pData->GetMyUser() ||
		NULL == g_pData->GetMyUser()->GetSelectUnit() )
	{
		return;
	}

	if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
		CX2State* pState = (CX2State*) GetNowState();
		pState->Handler_EGS_ADMIN_GET_SKILL_REQ( iSkillID, iSkillLevel );
	}
}

void CX2Main::SendStatisticsLocLog()
{
	if ( g_pData == NULL )
		return;

	if ( g_pData->GetMyUser() == NULL )
		return;


	if ( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_DEV )
		return;

	g_pData->GetServerProtocol()->SendID( EGS_SEND_STATISTICS_LOCLOG_REQ );
}

void CX2Main::SendStatisticsDBLog()
{
	if ( g_pData == NULL )
		return;

	if ( g_pData->GetMyUser() == NULL )
		return;


	if ( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_DEV )
		return;

	g_pData->GetServerProtocol()->SendID( EGS_SEND_STATISTICS_DBLOG_REQ );
}

void CX2Main::BuySuperEqipLevel_LUA( int level )
{
	if ( g_pData == NULL )
		return;

	if ( g_pData->GetMyUser() == NULL )
		return;


	if ( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_OPERATOR )
		return;

	CX2Unit* pCX2Unit = g_pData->GetMyUser()->GetSelectUnit();
	if( pCX2Unit != NULL )
	{
        CX2Unit::UNIT_CLASS eUnitClass = pCX2Unit->GetClass();
        unsigned uNumItemTemplets = g_pData->GetItemManager()->GetItemNum();
        for( unsigned int i = 0; i != uNumItemTemplets; ++i )
        {
            const CX2Item::ItemTemplet* pTemplet = g_pData->GetItemManager()->GetItemTempletFromIndex( i );
            if ( pTemplet == NULL || pTemplet->GetUnitClass() != eUnitClass || pTemplet->GetUseLevel() != level )
                continue;
			BuyItemSuper_LUA( pTemplet->GetItemID() );
        }
	}
}


void CX2Main::StateInLog()
{
	if( NULL == g_pMain )
		return;

	switch( m_NowStateID )
	{
	default:
	case XS_INVALID:
		StateLog( L"XS_INVALID");
		break;

	case XS_START_UP:
		StateLog( L"XS_START_UP");
		break;

	case XS_LOGIN:
		StateLog( L"XS_LOGIN");
		break;

	case XS_JOIN:
		StateLog( L"XS_JOIN");
		break;

	case XS_LOADING:
		StateLog( L"XS_LOADING");
		break;

	case XS_SERVER_SELECT:
		StateLog( L"XS_SERVER_SELECT");
		break;

	case XS_CREATE_UNIT:
		StateLog( L"XS_CREATE_UNIT");
		break;

	case XS_PVP_LOBBY:
		StateLog( L"XS_PVP_LOBBY");
		break;

	case XS_PVP_ROOM:
		StateLog( L"XS_PVP_ROOM");
		break;

	case XS_PVP_GAME:
		StateLog( L"XS_PVP_GAME");
		break;

	case XS_DUNGEON_ROOM:
		StateLog( L"XS_DUNGEON_ROOM");
		break;

	case XS_DUNGEON_GAME:
		StateLog( L"XS_DUNGEON_GAME");
		break;

	case XS_PVP_RESULT:
		StateLog( L"XS_PVP_RESULT");
		break;

	case XS_DUNGEON_RESULT:
		StateLog( L"XS_DUNGEON_RESULT");
		break;

	case XS_VILLAGE_MAP:
		StateLog( L"XS_VILLAGE_MAP");
		break;

	case XS_SQUARE_GAME:
		StateLog( L"XS_SQUARE_GAME");
		break;

	case XS_TRAINING_SCHOOL:
		StateLog( L"XS_TRAINING_SCHOOL");
		break;

	case XS_TRAINING_GAME:
		StateLog( L"XS_TRAINING_GAME");
		break;

	case XS_TRAINING_RESULT:
		StateLog( L"XS_TRAINING_RESULT");
		break;

	case XS_BATTLE_FIELD:
		StateLog( L"XS_BATTLE_FIELD");
		break;

	case XS_WEDDING_GAME:
		StateLog( L"XS_WEDDING_GAME");
		break;
	}
}




void CX2Main::IntentionalCrash()
{
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
        int* x = NULL;
		*x = 1;
	}
}

void CX2Main::SetFreeCamera( bool bEnable )
{

	switch( GetNowStateID() )
	{
	case XS_VILLAGE_MAP:
		{
			if( g_pTFieldGame != NULL )
			{
				g_pTFieldGame->SetFreeCamera( bEnable );
			}

		} break;

	case XS_SQUARE_GAME:
		{
			if( g_pSquareGame != NULL )
			{
				g_pSquareGame->SetFreeCamera( bEnable );
			}
		} break;
	case XS_WEDDING_GAME:
		{
			if( g_pWeddingGame != NULL )
			{
				g_pWeddingGame->SetFreeCamera( bEnable );
			}
		} break;

	case XS_TRAINING_GAME:
	case XS_PVP_GAME:
	case XS_DUNGEON_GAME:
	case XS_BATTLE_FIELD:
		{
			if( g_pX2Game != NULL )
			{
				g_pX2Game->SetFreeCamera( bEnable );
			}

		} break;
	}
}
bool CX2Main::GetFreeCamera() const
{
	switch( GetNowStateID() )
	{
	case XS_VILLAGE_MAP:
		{
			if( g_pTFieldGame != NULL )
			{
				return g_pTFieldGame->GetFreeCamera();
			}

		} break;

	case XS_SQUARE_GAME:
		{
			if( g_pSquareGame != NULL )
			{
				return g_pSquareGame->GetFreeCamera();
			}
		} break;
	case XS_WEDDING_GAME:
		{
			if( g_pWeddingGame != NULL )
			{
				return g_pWeddingGame->GetFreeCamera();
			}
		} break;

	case XS_TRAINING_GAME:
	case XS_PVP_GAME:
	case XS_DUNGEON_GAME:
	case XS_BATTLE_FIELD:
		{
			if( g_pX2Game != NULL )
			{
				return g_pX2Game->GetFreeCamera();
			}
		} break;
	}

	return false;
}


void CX2Main::SetWorldCameraEdit( bool bEnable)
{
	switch( GetNowStateID() )
	{
	case XS_VILLAGE_MAP:
		{
			if( g_pTFieldGame != NULL )
			{
				//g_pTFieldGame->SetFreeCamera( bEnable );
			}

		} break;

	case XS_SQUARE_GAME:
		{
			if( g_pSquareGame != NULL )
			{
				//g_pSquareGame->SetFreeCamera( bEnable );
			}

		} break;

	case XS_TRAINING_GAME:
	case XS_PVP_GAME:
	case XS_DUNGEON_GAME:
		{
			if( g_pX2Game != NULL )
			{
				g_pX2Game->SetWorldCameraEdit( bEnable );
			}
		} break;
	}


	return;
}



void CX2Main::PlayWorldCamera( int iWorldCameraID )
{
	switch( GetNowStateID() )
	{
	case XS_VILLAGE_MAP:
		{
			if( g_pTFieldGame != NULL )
			{
				//g_pTFieldGame->SetFreeCamera( bEnable );
			}

		} break;

	case XS_SQUARE_GAME:
		{
			if( g_pSquareGame != NULL )
			{
				//g_pSquareGame->SetFreeCamera( bEnable );
			}

		} break;

	case XS_TRAINING_GAME:
	case XS_PVP_GAME:
	case XS_DUNGEON_GAME:
		{
			if( g_pX2Game != NULL )
			{
				g_pX2Game->PlayWorldCamera(iWorldCameraID );
			}
		} break;
	}


}
void CX2Main::EndWorldCamera()
{
	if( g_pX2Game != NULL && g_pX2Game->GetX2Camera() != NULL)
	{
		if(g_pX2Game->GetX2Camera()->GetEnableWorldCamera() == true)
		{
			g_pX2Game->GetX2Camera()->EndWorldCamera();
		}
	}
	return;
}

void CX2Main::SetProjectionFov( float fFov )
{
	g_pKTDXApp->GetDGManager()->SetProjectionFov( g_pKTDXApp->GetDGManager()->GetNear(),
		g_pKTDXApp->GetDGManager()->GetFar(), fFov );
	return;
}


D3DXVECTOR2 CX2Main::GetWindowPos( D3DXVECTOR2 windowSize, D3DXVECTOR2 wantMousePoint )
{
	/*
	D3DXVECTOR2 resultPos = D3DXVECTOR2( 0, 0 );

	POINT point;
	GetCursorPos( &point );
	ScreenToClient( g_pKTDXApp->GetHWND(), &point );


	windowSize = g_pKTDXApp->ConvertByResolution( windowSize );
	wantMousePoint = g_pKTDXApp->ConvertByResolution( wantMousePoint );

	resultPos.x = (float)point.x;
	resultPos.y = (float)point.y;

	resultPos = g_pKTDXApp->MouseConvertByResolution( resultPos );

	resultPos -= wantMousePoint;




	if ( resultPos.x < 0 )
	{
		resultPos.x = 0;
	}
	else if ( resultPos.x + windowSize.x > 1024 )
	{
		resultPos.x = 1024 - windowSize.x;
	}

	if ( resultPos.y < 0 )
	{
		resultPos.y = 0;
	}
	else if ( resultPos.y + windowSize.y > 768 )
	{
		resultPos.y = 768 - windowSize.y;
	}

	return resultPos;
	*/


	D3DXVECTOR2 resultPos = D3DXVECTOR2( 0, 0 );

	POINT point;
	GetCursorPos( &point );
	ScreenToClient( g_pKTDXApp->GetHWND(), &point );

	resultPos.x = (float)point.x;
	resultPos.y = (float)point.y;

	resultPos = g_pKTDXApp->MouseConvertByResolution( resultPos );

	resultPos -= wantMousePoint;




	if ( resultPos.x < 0 )
	{
		resultPos.x = 0;
	}
	else if ( resultPos.x + windowSize.x > 1024 )
	{
		resultPos.x = 1024 - windowSize.x;
	}

	if ( resultPos.y < 0 )
	{
		resultPos.y = 0;
	}
	else if ( resultPos.y + windowSize.y > 768 )
	{
		resultPos.y = 768 - windowSize.y;
	}

	return resultPos;
}

void CX2Main::OpenURL( WCHAR* pURL )
{
	// ?????? ??? URL?? ??????��?.
	HINTERNET hURLFile;
	if ( NULL == ( hURLFile = ::InternetOpenUrl( m_hInternet,
		pURL,
		NULL,
		0,
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE,
		0 ) ) )
	{
		ErrorLog( XEM_ERROR107 );
		return;
	}

	::InternetCloseHandle( hURLFile );
}
void CX2Main::CreateStateChangeDLG( const WCHAR* szExplanation, wstring wstrCustomLuaFileName/* = L""*/, wstring wstrPlaySoundFileName /*= L""*/ )
{
	SAFE_DELETE_DIALOG( m_pDLGStateChange );

	if ( wstrCustomLuaFileName != L"" )
	{
		if ( GetNowStateID() == CX2Main::XS_SERVER_SELECT ) // ???? ????????? ???? ??? ????? ???
			m_pDLGStateChange = KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), szExplanation, GetNowState(), -1, -1.f, wstrCustomLuaFileName, D3DXVECTOR2 ( 0, 0 ), wstrPlaySoundFileName );
		else if ( GetNowStateID() == CX2Main::XS_CREATE_UNIT ) // ???? ????????? ???? ??? ????? ???
			m_pDLGStateChange = KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), szExplanation, GetNowState(), -1, -1.f, wstrCustomLuaFileName, D3DXVECTOR2 ( 0, 0 ), wstrPlaySoundFileName );
		else
			m_pDLGStateChange = KTDGUIOKMsgBox( D3DXVECTOR2(250,300), szExplanation, GetNowState(), -1, -1.f, wstrCustomLuaFileName );
	}
	else
	{
		m_pDLGStateChange = KTDGUIMsgBox( D3DXVECTOR2(250,300), szExplanation, GetNowState() );
	}


	m_pDLGStateChange->MoveStop();
	m_pDLGStateChange->SetColor( D3DXCOLOR( 1,1,1,1 ) );

	if ( m_pNowState != NULL )
	{
		if ( DXUTGetDeviceLost() == false && DXUTGetDeviceObjectsReset() == true )
		{
			g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x32323232, 1.0f, 0);
			g_pKTDXApp->GetDevice()->BeginScene();
			m_pNowState->OnFrameRender();
			g_pKTDXApp->GetDevice()->EndScene();
			HRESULT hr = g_pKTDXApp->GetDevice()->Present( NULL, NULL, NULL, NULL );

			if ( FAILED(hr) )
			{
				if ( D3DERR_DEVICELOST == hr )
				{
					DXUTSetDeviceLost( true );
				}
				else if ( D3DERR_DRIVERINTERNALERROR == hr )
				{
					// When D3DERR_DRIVERINTERNALERROR is returned from Present(),
					// the application can do one of the following:
					//
					// - End, with the pop-up window saying that the application cannot continue
					//   because of problems in the display adapter and that the user should
					//   contact the adapter manufacturer.
					//
					// - Attempt to restart by calling IDirect3DDevice9::Reset, which is essentially the same
					//   path as recovering from a lost device. If IDirect3DDevice9::Reset fails with
					//   D3DERR_DRIVERINTERNALERROR, the application should end immediately with the message
					//   that the user should contact the adapter manufacturer.
					//
					// The framework attempts the path of resetting the device
					//
					DXUTSetDeviceLost( true );
				}
			}
		}
	}

}

void CX2Main::DeleteStateChangeDLG()
{
	if ( m_pDLGStateChange != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGStateChange, NULL, false );

	m_pDLGStateChange = NULL;
}



void CX2Main::DeleteOtherFile( WCHAR* pExtention, vector<wstring>& fileList )
{
//#ifdef _SERVICE_

	WCHAR buff[MAX_PATH] = {0};
	GetCurrentDirectory( MAX_PATH, buff );
	DeleteOtherFile( pExtention, fileList, buff );

//#endif
}


void CX2Main::DeleteOtherFile( WCHAR* pExtention, vector<wstring>& fileList, WCHAR* wstrFolder )
{
//#ifdef _SERVICE_

	HANDLE				hSearch;
	WIN32_FIND_DATA		fd;
	WCHAR				wstrSearchPath[256];


	wcscpy( wstrSearchPath, wstrFolder);
	wcscat( wstrSearchPath, L"\\*.*");

	hSearch = FindFirstFile( wstrSearchPath, &fd );

	if(hSearch == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if( wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..") )
		{
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				WCHAR	wstrNewSearchDir[256];

				wcscpy( wstrNewSearchDir, wstrFolder );
				wcscat( wstrNewSearchDir, L"\\");
				wcscat( wstrNewSearchDir, fd.cFileName );

				DeleteOtherFile( pExtention, fileList, wstrNewSearchDir );
			}
			else
			{
				WCHAR			wstrNewSearchFile[256];
				std::wstring	wstrFile;
				std::wstring	wstrFileName;

				wcscpy( wstrNewSearchFile, wstrFolder );
				wcscat( wstrNewSearchFile, L"\\");
				wcscat( wstrNewSearchFile, fd.cFileName );

				wstrFile = wstrNewSearchFile;
				wstrFileName = fd.cFileName;

				bool bCheck = false;

				wstring nowFileName = wstrFileName;
				MakeUpperCase( nowFileName );

				for ( int i = 0; i < (int)fileList.size(); i++ )	//?????????? ???????? ???
				{
					wstring tempFileName = fileList[i];
					MakeUpperCase( tempFileName );

					if ( tempFileName == nowFileName )
					{
						bCheck = true;
						break;
					}
				}

				if ( bCheck == false )	//???????? ??????
				{
					//????? kom???? ????��?(?????? ??? ???��?) ???? ?��???
					int strIndex = wstrFileName.rfind( L"." );

					if ( strIndex != -1 )
					{
						wstring extName = wstrFileName.substr( strIndex + 1 );


						for( std::wstring::iterator i = extName.begin(); i != extName.end(); i++ )
							*i = (WCHAR)towupper(*i);

						wstring tempExtName = pExtention;
						MakeUpperCase( tempExtName );

						if ( extName == tempExtName )
						{
							//wstring tempNewFile = wstrNewSearchFile;
							::DeleteFile( wstrNewSearchFile );
						}
					}
				}
			}
		}

	}while( FindNextFile( hSearch, &fd ) );

	FindClose(hSearch);

//#endif
}

wstring CX2Main::GetStrByLienBreak( const WCHAR* pOrgStr, int width, int fontIndex )
{
	if ( pOrgStr == NULL )
		return L"";

	wstring tempText = pOrgStr;
	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * width);

	int nowTextLen = 0;

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( fontIndex );
	if( pFont != NULL )
	{
		for ( int i = 0; i < (int)tempText.size(); i++ )
		{
			WCHAR tempWchar = tempText[i];
			if ( nowTextLen < constTextMaxLen && tempWchar == L'\n' )
			{
				nowTextLen = 0;
				continue;
			}

			if ( tempText[i] == L'#' && i + 1 < (int)tempText.size() && ( tempText[i+1] == L'C' || tempText[i+1] == L'c' ) )
			{
				if ( i + 2 < (int)tempText.size() )
				{
					if( tempText[i+2] == L'x' || tempText[i+2] == L'X' )
					{
						i += 2;
					}
					else
					{
						i += 7;
					}
					continue;
				}
			}

			int iUniCharSize = 0;

			iUniCharSize = pFont->GetWidth( tempWchar );
			nowTextLen += iUniCharSize;
			if( tempWchar == L' ')
				iUniCharSize += iUniCharSize;

			if ( nowTextLen >= constTextMaxLen )
			{
				nowTextLen = 0;
				wstring enterChar = L"\n";
				if ( i + 1 < (int)tempText.size() )
					tempText.insert( tempText.begin() + i + 1, enterChar.begin(), enterChar.end() );
				else
					tempText.insert( tempText.begin() + i, enterChar.begin(), enterChar.end() );
				i++;
			}

			if ( i >= (int)tempText.size() )
			{
				break;
			}
		}


	}

	return tempText;
}

wstring CX2Main::GetStrByLineBreakColor( const WCHAR* pOrgStr, int width, int fontIndex )
{
	if ( pOrgStr == NULL )
		return L"";

	wstring tempText = pOrgStr;
	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * width);

	int nowTextLen = 0;

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( fontIndex );
	if( pFont != NULL )
	{
		wstring wstrPrevColorkey = L"";
		for ( int i = 0; i < (int)tempText.size(); i++ )
		{
			WCHAR tempWchar = tempText[i];
			if ( nowTextLen <= constTextMaxLen && tempWchar == L'\n' )
			{
				nowTextLen = 0;
				tempText.insert( tempText.begin() + i + 1, wstrPrevColorkey.begin(), wstrPrevColorkey.end() );
				wstrPrevColorkey = L"";
				continue;
			}

			if ( tempText[i] == L'#' && i + 1 < (int)tempText.size() && ( tempText[i+1] == L'C' || tempText[i+1] == L'c' ) )
			{
				if ( i + 2 < (int)tempText.size() )
				{
					if( tempText[i+2] == L'x' || tempText[i+2] == L'X' )
					{
						wstrPrevColorkey = L"";
						i += 2;
					}
					else
					{
						wstrPrevColorkey = tempText.substr(i, 8);
						i += 7;
					}
					continue;
				}
			}

			int iUniCharSize = 0;

			iUniCharSize = pFont->GetWidth( tempWchar );
			nowTextLen += iUniCharSize;
			if( tempWchar == L' ')
				iUniCharSize += iUniCharSize;

			if ( nowTextLen >= constTextMaxLen )
			{
				wstring enterChar = L"\n";
				nowTextLen = 0;

				if ( i + 1 <= (int)tempText.size() )
				{
					if ( tempText[i + 1] != L'\n' )
					{
						tempText.insert( tempText.begin() + i + 1, enterChar.begin(), enterChar.end() );

						// ???? ???? ? ????? ?????? ??? ????
						if ( i + 2 < (int)tempText.size() && tempText[i + 2] == L' ' )
						{
							tempText.erase( i + 2, 1 );
						}
					}

					if(wstrPrevColorkey != L"")
						{
							tempText.insert( tempText.begin() + i + 2, wstrPrevColorkey.begin(), wstrPrevColorkey.end() );
							wstrPrevColorkey = L"";
						}
				}
				else
				{
					if ( tempText[i] != L'\n' && tempText[i] != L')' && tempText[i] != L'.' )
					{
						tempText.insert( tempText.begin() + i, enterChar.begin(), enterChar.end() );

						if(wstrPrevColorkey != L"")
						{
							tempText.insert( tempText.begin() + i + 1, wstrPrevColorkey.begin(), wstrPrevColorkey.end() );
							wstrPrevColorkey = L"";
						}
					}
					nowTextLen = 0;
				}
				i++;
			}

			if ( i >= (int)tempText.size() )
			{
				break;
			}
		}
	}

	return tempText;
}


//void CX2Main::ResetLuaBinder( KLuabinder* pKLuabinder )
//{
//	if ( pKLuabinder == NULL )
//		return;
//
//	g_pKTDXApp->RegisterLuabind( pKLuabinder );
//	RegisterLuabind( pKLuabinder );
//	LoadLuaEnum( pKLuabinder );
//
//}


void CX2Main::UpdateProcessList()
{
    set<wstring>    setProcessNameToFindHack;

	wstring wstrTemp;
	DWORD aProcesses[1024], cbNeeded;

	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return;

#if X2_DEV_TOOLS
	FILE *fp = NULL;
	fp = fopen("ProcessList.Log", "w");
#endif

	DWORD cProcesses = cbNeeded / sizeof(DWORD);
	for ( DWORD PidFor = 0x0c; PidFor < 0xFFFF; PidFor +=4 )
	{
		if( PidFor == GetCurrentProcessId() )
			continue;

		HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, PidFor );

		if (hProcess)
		{
			BOOL sHide = false;
			unsigned int i;
			WCHAR szName[MAX_PATH]= L"<Unknown>";
			HMODULE hModule;

			for ( i = 1; i <= cProcesses; i++ )
			{
				if (PidFor == aProcesses[i])
				{
					sHide = true;
					break;
				}

			}

			if(EnumProcessModules(hProcess,&hModule,sizeof(hModule),&cbNeeded))
			{
				GetModuleFileNameEx(hProcess,hModule,szName,ARRAY_SIZE(szName));
				wstrTemp = szName;
				MakeUpperCase( wstrTemp );

				if(!sHide)
					wstrTemp = wstrTemp + L" [Hidden] ";
                setProcessNameToFindHack.insert( wstrTemp );

#if X2_DEV_TOOLS
				fwprintf(fp, L"%-5d - %16s  %s\n", PidFor, szName, (sHide) ? L"" : L"--[Hidden]--");
#endif
			}
			else
			{
				GetProcessImageFileName(hProcess,szName,ARRAY_SIZE(szName));
				wstrTemp = szName;
				MakeUpperCase( wstrTemp );

				wstrTemp = L"[Zombie] " + wstrTemp + L" \r\n ";
                setProcessNameToFindHack.insert( wstrTemp );

#if X2_DEV_TOOLS
				fwprintf(fp, L"%-5d - %16s  %s\n", PidFor, szName, L"--[Zombie]--");
#endif
			}
		}

		CloseHandle( hProcess );
	}

#if X2_DEV_TOOLS
	fclose(fp);
#endif




    if ( setProcessNameToFindHack.empty() == false )
    {
        CSLock lock(m_csProcessNameToFindHack);
        m_setProcessNameToFindHack_CS.insert( setProcessNameToFindHack.begin(), setProcessNameToFindHack.end() );
    }

}






void CX2Main::SendHackMail()
{
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1  );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//#ifdef PROCESSLIST
//	m_setProcessNameToFindHack.clear();
//#endif
//	if ( m_setProcessNameToFindHack.empty() == false )
//	{
//		for( int i = 0; i < 2; ++i )
//		{
//			//g_pX2SMTPMail->SetSenderEMail( "demiahun@kogstudios.com" );
//			//g_pX2SMTPMail->SetReceiverEMail( "demiahun@kogstudios.com" );
//			g_pX2SMTPMail->SetSenderEMail( szMail[i] );
//			g_pX2SMTPMail->SetReceiverEMail( szMail[i] );
//
//			if( strlen( szID ) > 0 )
//				g_pX2SMTPMail->SetSenderName( szID );
//			else
//				g_pX2SMTPMail->SetSenderName( "????? ??? ???? ??????" ); // TODO : ?? string always.h ?? ????
//
//			string mailName;
//			ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//			g_pX2SMTPMail->SetSubject( (char*)mailName.c_str() );
//			stringstream wstrstm;
//
//			set<wstring>::iterator sit;
//			sit = m_setProcessNameToFindHack.begin();
//			for ( ; sit != m_setProcessNameToFindHack.end(); sit++ )
//			{
//				m_setSendedProcessList.insert( *sit );
//				string processName;
//				ConvertWCHARToChar( processName, (*sit).c_str() );
//				wstrstm << processName.c_str() << "\n";
//			}
//			g_pX2SMTPMail->SetDesc( (char*)wstrstm.str().c_str() );
//			g_pX2SMTPMail->SendMail( g_pKTDXApp->GetHWND() );
//		}
//
//
////{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//		if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//		{
//			// Unable to initialize winsock2
//			return;
//		}
//
//#ifdef CHANGE_MAILSTRING
//		g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//		g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//		g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//		g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//		if( strlen( szID ) > 0 )
//			g_pX2SMTPMail->SMTPSetSenderName( szID );
//		else
//			g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" ); // TODO : ?? string always.h ?? ????
//
//
//		g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//		g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//		string mailName;
//		ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//		g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//		stringstream wstrstm;
//
//		// ???? ??? ????? ????
//		//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//		g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//		//g_pX2SMTPMail->SMTPAddRecipient("oasis907@naver.com");
//		//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//		//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//
//		g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//		g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//#ifdef MORE_INFO_ABOUT_HACK
//		// ????? ????
//		AddSystemInfo(wstrstm);
//
//		// DLL ????
//		AddDLLInfo(wstrstm);
//
//		// ???��??? ????
//		AddProcessInfo(wstrstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//		set<wstring>::iterator sit;
//		sit = m_setProcessNameToFindHack.begin();
//		for ( ; sit != m_setProcessNameToFindHack.end(); sit++ )
//		{
//			m_setSendedProcessList.insert( *sit );
//			string processName;
//			ConvertWCHARToChar( processName, (*sit).c_str() );
//			wstrstm << processName.c_str() << "\n";
//		}
//
//		g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//		string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//		wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//		if( m_strUserIdToFindHack.empty() == false )
//		{
//			ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//			wstrUserId = m_strUserIdToFindHack;
//		}
//		strUserId = strUserId + ".jpg";
//		wstrUserId = wstrUserId + L".jpg";
//
//		MakeHackScreenShot( strUserId.c_str() );
//		// ???? ��?? ???? ????
//		//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//		//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//		wchar_t szFilePath[1024] = _T("");
//		wchar_t strHackImageBuffer[1024];
//		GetCurrentDirectory( _MAX_PATH, szFilePath );
//		StringCchPrintf(strHackImageBuffer, 1024,  _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//		//wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//		string strNickName = "";
//		ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//		g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//		if( g_pX2SMTPMail->SMTPSend() )
//		{
//			//The mail was send successfully
//		}
//		else
//		{
//			//Unable to send the mail
//		}
//		// ????? ???, ��?????? ????
//		g_pX2SMTPMail->SMTPClearRecipient();
//
//
//		DeleteFile( wstrUserId.c_str() );
//
////}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//	}
//
//	m_setProcessNameToFindHack.clear();
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END
}

void CX2Main::SendHackMail_DamageHistory(const char *strComm)
{
////#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
///*
//#ifdef PROCESSLIST
//	m_setProcessNameToFindHack.clear();
//#endif
//	if( m_setProcessNameToFindHack.empty() == true )
//		UpdateProcessList();
//
//
//	for( int i = 0; i < 2; ++i )
//	{
//		//g_pX2SMTPMail->SetSenderEMail( "demiahun@kogstudios.com" );
//		//g_pX2SMTPMail->SetReceiverEMail( "demiahun@kogstudios.com" );
//		g_pX2SMTPMail->SetSenderEMail( szMail[i] );
//		g_pX2SMTPMail->SetReceiverEMail( szMail[i] );
//
//		if( strlen( szID ) > 0 )
//			g_pX2SMTPMail->SetSenderName( szID );
//		else
//			g_pX2SMTPMail->SetSenderName( "????? ??? ???? ??????" );
//
//		string mailName;
//		ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//		g_pX2SMTPMail->SetSubject( (char*)mailName.c_str() );
//		stringstream wstrstm;
//
//		set<wstring>::iterator sit;
//		sit = m_setProcessNameToFindHack.begin();
//		for ( ; sit != m_setProcessNameToFindHack.end(); sit++ )
//		{
//			m_setSendedProcessList.insert( *sit );
//			string processName;
//			ConvertWCHARToChar( processName, (*sit).c_str() );
//			wstrstm << processName.c_str() << "\n";
//		}
//		g_pX2SMTPMail->SetDesc( (char*)wstrstm.str().c_str() );
//		g_pX2SMTPMail->SendMail( g_pKTDXApp->GetHWND() );
//	}*/
//
//
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("oasis907@naver.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//#ifdef _IN_HOUSE_
//	wstrstm << "[IN HOUSE] " << strComm <<"\n\n";
//#else
//	wstrstm << strComm <<"\n\n";
//#endif
//
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	wstrstm << "- Loaded Dll -";
//	const char *dlllist = SiCX2DLLManager()->GetPreLoadedDllList();
//	wstrstm << dlllist;
//
//	// DLL ????
//	AddDLLInfo(wstrstm);
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024,  _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//    //wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END
////#endif
}

void CX2Main::SendHackMail_HackUserNot(const char *strComm)
{
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("k4interq@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//	wstrstm << strComm <<"\n\n";
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	wstrstm << "- Loaded Dll -";
//	const char *dlllist = SiCX2DLLManager()->GetPreLoadedDllList();
//	wstrstm << dlllist;
//
//	// DLL ????
//	wstring wstrDllInfo = SiCX2DLLManager()->GetReportEvent();
//	string strDllInfo = "";
//	ConvertWCHARToChar( strDllInfo, wstrDllInfo );
//	wstrstm << strDllInfo << "\n\n";
//
//	AddDLLInfo(wstrstm);
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	//wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		DisplayErrorMessage( L"mail error" );
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END

}


void CX2Main::SendHackMail_AttachFile(const char *strComm, const string& userId)
{
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//
//	static bool bSend = false;
//
//	if( bSend == true )
//		return;
//
//
//	if( userId != L"" && userId.compare( m_strUserIdToFindHack ) != 0 )
//		return;
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com",  MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" ,  MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//	wstrstm << "Attach File" <<"\n\n";
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	// DLL ????
//	wstring wstrDllInfo = SiCX2DLLManager()->GetReportEvent();
//	string strDllInfo = "";
//	ConvertWCHARToChar( strDllInfo, wstrDllInfo );
//	wstrstm << strDllInfo << "\n\n";
//
//	AddDLLInfo(wstrstm);
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//#if 0
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	/*wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());*/
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//#endif
//
//	g_pX2SMTPMail->SMTPAddAttachment( strComm );
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	//DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	bSend = true;
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END

}



void CX2Main::SendHackMail_FileList(const char *strComm, const string& userId)
{
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//
//	static bool bSend = false;
//
//	if( bSend == true )
//		return;
//
//
//	if( userId != L"" && userId.compare( m_strUserIdToFindHack ) != 0 )
//		return;
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//	wstrstm << strComm <<"\n\n";
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	//DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	bSend = true;
//
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END
//
//
}

void CX2Main::SendHackMail_VERIFYVALUE(const char *strComm)
{
//#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_START
//#endif
//	THEMIDA_VM_START
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("oasis907@naver.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//	wstrstm << strComm <<"\n\n";
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	wstrstm << "- Loaded Dll -";
//	const char *dlllist = SiCX2DLLManager()->GetPreLoadedDllList();
//	wstrstm << dlllist;
//
//	// DLL ????
//	AddDLLInfo(wstrstm);
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	//wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	THEMIDA_VM_END
//
//#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_END
//#endif

}

void CX2Main::SendHackMailGameStart(int iVal, bool bVal)
{
//#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_START
//#endif
//
//	THEMIDA_VM_START
//
//	static int iCheckHackCnt[2] = {0,};
//
//	if( bVal == false )
//	{
//		++iCheckHackCnt[iVal];
//		return;
//	}
//
//	if( iCheckHackCnt[iVal] <= 0 )
//		return;
//
//	iCheckHackCnt[iVal] = 0;
//
//	char *strComm = NULL;
//	switch( iVal )
//	{
//	case 0:
//		strComm = ANTI_HACK_STRING_AntiHacking_CheckSubStageGoNext;
//		break;
//	case 1:
//		strComm = ANTI_HACK_STRING_AntiHacking_Dungeon_ClearScore;
//		break;
//	default:
//		return;
//	}
//
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("oasis907@naver.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//	wstrstm << strComm <<"\n\n";
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	wstrstm << "- Loaded Dll -";
//	const char *dlllist = SiCX2DLLManager()->GetPreLoadedDllList();
//	wstrstm << dlllist;
//
//	// DLL ????
//	AddDLLInfo(wstrstm);
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	//wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	THEMIDA_VM_END
//
//#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_END
//#endif
}

void CX2Main::AddMemoryInfo( stringstream& strstm )
{
	PROCESS_MEMORY_COUNTERS_EX pmcex;
	GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmcex, sizeof(pmcex));

	const UINT MEGA = 1024 * 1024;

	string strNewLine = "\n";
	/// ???? ???��??? ??? ????
	strstm << "- Memory Information - " << strNewLine;
	strstm << "PageFaultCount: " << pmcex.PageFaultCount << strNewLine;
	strstm << "PeakWorkingSetSize(???): " << pmcex.PeakWorkingSetSize / MEGA << strNewLine;
	strstm << "WorkingSetSize(????): " << pmcex.WorkingSetSize / MEGA << strNewLine;
	strstm << "QuotaPeakPagedPoolUsage: " << pmcex.QuotaPeakPagedPoolUsage << strNewLine;
	strstm << "QuotaPagedPoolUsage: " << pmcex.QuotaPagedPoolUsage << strNewLine;
	strstm << "QuotaPeakNonPagedPoolUsage: " << pmcex.QuotaPeakNonPagedPoolUsage << strNewLine;
	strstm << "QuotaNonPagedPoolUsage: " << pmcex.QuotaNonPagedPoolUsage << strNewLine;
	strstm << "PrivateUsage(????): " << pmcex.PrivateUsage / MEGA << strNewLine;

	MEMORYSTATUSEX MemStatusEx;
	MemStatusEx.dwLength = sizeof( MEMORYSTATUSEX );
	if ( GlobalMemoryStatusEx( &MemStatusEx ) )
	{
		/// ????? ??? ????
		strstm << "dwMemoryLoad: " << MemStatusEx.dwMemoryLoad / MEGA << strNewLine;
		strstm << "ullTotalPhys: " << MemStatusEx.ullTotalPhys / MEGA << strNewLine;
		strstm << "ullAvailPhys: " << MemStatusEx.ullAvailPhys / MEGA << strNewLine;
		strstm << "ullTotalPageFile: " << MemStatusEx.ullTotalPageFile << strNewLine;
		strstm << "ullAvailPageFile: " << MemStatusEx.ullAvailPageFile << strNewLine;
		strstm << "ullTotalVirtual: " << MemStatusEx.ullTotalVirtual / MEGA << strNewLine;
		strstm << "ullAvailVirtual: " << MemStatusEx.ullAvailVirtual / MEGA << strNewLine;
	}

	/// ????? ?????? ??? ??? ?�X
	const UINT uiStrSize = MAX_PATH + 1;
	char szSystemDirectory[uiStrSize] = { 0, };
	const UINT uiReturnValue = GetSystemDirectoryA( szSystemDirectory, uiStrSize );
	if ( uiReturnValue <= uiStrSize && uiReturnValue != 0 )
	{
		char drive[_MAX_DRIVE] = { 0, };
		char dir[_MAX_DIR] = { 0, };
		char fname[_MAX_FNAME] = { 0, };
		char ext[_MAX_EXT] = { 0, };

		_splitpath( szSystemDirectory, drive, dir, fname, ext );
		if ( DRIVE_FIXED == GetDriveTypeA( drive ) )
		{
			ULARGE_INTEGER ulAvail, ulTotal;
			if ( GetDiskFreeSpaceExA( drive, &ulAvail, &ulTotal, NULL ) )
			{
				strstm << "Hard Drive: " << drive << strNewLine;
				strstm << "Available Space: " << static_cast<int>( ulAvail.QuadPart / MEGA )<< "MB" << strNewLine;
				strstm << "Total Space: " << static_cast<int>( ulTotal.QuadPart / MEGA )<< "MB" << strNewLine;
			}
		}
	}

	/// ???????? ????
	IDirect3D9* pD3D9 = DXUTGetD3DObject();
	UINT dwAdapterCount = pD3D9->GetAdapterCount();

	for( UINT iAdapter = 0; iAdapter < dwAdapterCount; iAdapter++ )
	{
		D3DADAPTER_IDENTIFIER9 id;
		ZeroMemory( &id, sizeof( D3DADAPTER_IDENTIFIER9 ) );
		pD3D9->GetAdapterIdentifier( iAdapter, 0, &id );
		strstm << "D3D9 Adapter: " << iAdapter << strNewLine;
		strstm << "Driver: " << id.Driver << strNewLine;
		strstm	<< "Description: " << id.Description << strNewLine;
	}

	strstm << strNewLine << strNewLine;
}

void CX2Main::AddSystemInfo(stringstream& strstm)
{
	string strNewLine = "\n";

	strstm << "- System Information -" << strNewLine;
	strstm << "OS: ";
	strstm << GetSystemInfo()->GetOSVersion();
	strstm << " " << GetSystemInfo()->GetOSName();
	if(0 != GetSystemInfo()->GetServicePack())
		strstm << " Service Pack " << GetSystemInfo()->GetServicePack();
	strstm << strNewLine;

	strstm << "Install Locale: " << GetSystemInfo()->GetInstallLocale() << strNewLine;

	strstm << "System Locale: " << GetSystemInfo()->GetSystemLocale() << strNewLine;

	const vector<string>& vecInputLocale = GetSystemInfo()->GetInputLocale();
	for(vector<string>::const_iterator iter = vecInputLocale.begin(); iter != vecInputLocale.end(); ++iter)
	{
		if(iter == vecInputLocale.begin())
			strstm << "Input Locales: ";
		else
            strstm << "                    ";

		strstm << *iter << strNewLine;
	}

	strstm << strNewLine << GetMachineId().c_str() << strNewLine;


	char cWallPaperPath[ MAX_PATH ];
	if( GetWallPaperDirectory( cWallPaperPath, MAX_PATH ) == true )
		strstm << cWallPaperPath << strNewLine;

	strstm << strNewLine << strNewLine;
}

void CX2Main::AddDLLInfo(stringstream& strstm)
{
	string strNewLine = "\n";

	strstm << "- DLL Information -" << strNewLine;


	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, GetCurrentProcessId() );
	if (NULL == hProcess)
	{
		strstm << "Cannot access to process." << strNewLine;
		return;
	}

	HMODULE hMods[1024];
	DWORD cbNeeded;
	if(EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
	{
		int numProcess = cbNeeded / sizeof(HMODULE);
		for (int i = 0; i < numProcess; i++ )
		{
			char szModName[MAX_PATH];
			ZeroMemory( szModName, MAX_PATH );
			// Get the full path to the module's file.
			if (!GetModuleFileNameExA( hProcess, hMods[i], szModName, ARRAY_SIZE(szModName) ))
			{
				strncpy(szModName, "[Hidden]" , MAX_PATH - 1 );
				//strcpy(szModName, "[Hidden]");
			}

			strstm << boost::str(boost::format("[0x%08X] %s") % hMods[i] % szModName);

			HANDLE hFile = CreateFileA( szModName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
			DWORD dwFileSize = GetFileSize( hFile, NULL );

			// system32 ???? ????? ?????? ?? ?????? ????? ????????
			if ( INVALID_FILE_SIZE != dwFileSize )
				strstm << ", Size: " << dwFileSize << strNewLine;
			else
				strstm << strNewLine;

			CloseHandle( hFile );
		}
	}

	CloseHandle( hProcess );

	strstm << strNewLine << strNewLine;
}

void CX2Main::AddProcessInfo(stringstream& strstm, bool bThreadInfo)
{
	string strNewLine = "\n";

	strstm << "- Process Information -" << strNewLine;

    {
        CSLock lock(m_csProcessNameToFindHack);
        m_setProcessNameToFindHack_CS.clear();
    }
	UpdateProcessList();
    set<wstring>    setSendedProcessList;
    {
        CSLock lock(m_csSendedProcessList);
        setSendedProcessList = 			m_setSendedProcessList_CS;
    }
    BOOST_FOREACH( const std::wstring& wstrProcessName, setSendedProcessList )
    {
		string processName;
		ConvertWCHARToChar( processName, wstrProcessName );
		strstm << processName.c_str() << strNewLine;
    }
    setSendedProcessList.clear();
    set<wstring>    setProcessNameToFindHack;
    {
        CSLock lock(m_csProcessNameToFindHack);
        setProcessNameToFindHack = m_setProcessNameToFindHack_CS;
    }
    BOOST_FOREACH( const std::wstring& wstrProcessName, setProcessNameToFindHack )
    {
		setSendedProcessList.insert( wstrProcessName );
		string processName;
		ConvertWCHARToChar( processName, wstrProcessName );
		strstm << processName.c_str() << strNewLine;
    }
    if ( setSendedProcessList.empty() == false )
    {
        CSLock lock(m_csSendedProcessList);
        m_setSendedProcessList_CS.insert( setSendedProcessList.begin(), setSendedProcessList.end() );
    }


	if( bThreadInfo == true )
	{
		strstm << strNewLine << "- TInfo -" << strNewLine;

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		DWORD ThreadId = GetWindowThreadProcessId(NULL,NULL);
		vector<THREAD_WND_INFO> vecThreadInfo = GetThreadWnd(ThreadId);
		for(UINT i=0; i<vecThreadInfo.size(); ++i)
		{
			string threadWndName;
			ConvertWCHARToChar( threadWndName, vecThreadInfo[i].m_wstrName.c_str() );

			if( !vecThreadInfo[i].m_wstrClass.empty() )
			{
				string threadClassName;
				ConvertWCHARToChar( threadClassName, vecThreadInfo[i].m_wstrClass.c_str() );
				strstm << threadWndName.c_str() << "   [" << threadClassName.c_str() << "]" << strNewLine;
			}
			else
			{
				strstm << threadWndName.c_str() << "   [EMPTY]" << strNewLine;
			}
		}

		strstm << strNewLine << strNewLine;
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		if( g_pKTDXApp->GetHWND() != NULL )
		{
			DWORD ThreadId = GetWindowThreadProcessId(g_pKTDXApp->GetHWND(),NULL);

			vector<THREAD_WND_INFO> vecThreadInfo = GetThreadWnd(ThreadId);

			for(UINT i=0; i<vecThreadInfo.size(); ++i)
			{
				string threadWndName;
				ConvertWCHARToChar( threadWndName, vecThreadInfo[i].m_wstrName.c_str() );

				if( !vecThreadInfo[i].m_wstrClass.empty() )
				{
					string threadClassName;
					ConvertWCHARToChar( threadClassName, vecThreadInfo[i].m_wstrClass.c_str() );
					strstm << threadWndName.c_str() << "   [" << threadClassName.c_str() << "]" << strNewLine;
				}
				else
				{
					strstm << threadWndName.c_str() << "   [EMPTY]" << strNewLine;
				}

			}
		}
	}

	strstm << strNewLine << strNewLine;
}


//{{ ????? : [2010/05/05]	//	DLL ?????? ?????
void CX2Main::SendHackMail_DLLInjection(const char *strComm)
{
////#ifdef _SERVICE_
//
//
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//
//
//#ifdef _SERVICE_
//		if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
//		{
//			if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetUserData() != NULL &&
//				g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
//				return;
//		}
//#endif
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//
//	// ???? ??? ????? ????
//	g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//
//
//	stringstream strstm;
//	strstm << ANTI_HACK_STRING_UserID_AntiHacking_Event_DLL_Injection << std::endl << std::endl;
//
//	// ????
//	strstm << strComm <<"\n\n";
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(strstm);
//
//	// DLL ????
//	AddDLLInfo(strstm);
//
//	// ???��??? ????
//	AddProcessInfo(strstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody(strstm.str().c_str());
//
//
//
//
//
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	//wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END
//
////#endif
}

void CX2Main::SendUserMonitoringMail(const char *strComm)
{
//	//#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//	char szMail[2][MAX_PATH];
//	strncpy( szMail[0], "demiahun@gmail.com" , MAX_PATH - 1 );		// ????
//	strncpy( szMail[1], "elswordmkt@gmail.com" , MAX_PATH - 1 );	// ?????
//	//strcpy( szMail[0], "demiahun@gmail.com" );		// ????
//	//strcpy( szMail[1], "elswordmkt@gmail.com" );	// ?????
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("wonpok@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("comicprince@kog.co.kr");
//	//g_pX2SMTPMail->SMTPAddRecipient("oasis907@naver.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("itexpertkim@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("JHKang@kog.co.kr");
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//	if( strcmp(strComm, ANTI_HACK_IP_MONITORING) == 0 )
//	{
//		string strIp;
//		ConvertWCHARToChar( strIp, m_wstrPublicIp.c_str() );
//#ifdef _IN_HOUSE_
//		wstrstm << "[IN HOUSE] " << strComm << " : " << strIp << "\n\n";
//#else
//		wstrstm << strComm << " : " << strIp << "\n\n";
//#endif
//	}
//	else
//	{
//#ifdef _IN_HOUSE_
//		wstrstm << "[IN HOUSE] " << strComm << "\n\n";
//#else
//		wstrstm << strComm <<"\n\n";
//#endif
//	}
//
//
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	// DLL ????
//	AddDLLInfo(wstrstm);
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024,  _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	//wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END
//	//#endif
}

void CX2Main::SendMail_MoneyMonitoring(const char *strComm)
{
////#ifdef _SERVICE_
//	ELSWORD_VIRTUALIZER_START
//	THEMIDA_VM_START
//
//	char szID[MAX_PATH] = "";
//	if( g_pInstanceData != NULL &&
//		g_pInstanceData->GetUserID().size() > 0 )
//	{
//		WCHAR wszID[MAX_PATH] = L"";
//		wcscpy( wszID, g_pInstanceData->GetUserID().c_str() );
//		::MultiByteToWideChar( CP_ACP, 0, szID, -1, wszID, MAX_PATH );
//	}
//
//	if( g_pX2SMTPMail == NULL )
//	{
//		g_pX2SMTPMail = new CX2SMTPMail;
//	}
//
//	//{{ oasis907 : ????? [2009.12.21] // ?? CSmtp
//	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
//	{
//		// Unable to initialize winsock2
//		return;
//	}
//
//#ifdef CHANGE_MAILSTRING
//	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
//#else
//	g_pX2SMTPMail->SMTPSetServer( "mail.kog.co.kr", 25 );
//#endif
//	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
//	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
//
//	if( strlen( szID ) > 0 )
//		g_pX2SMTPMail->SMTPSetSenderName( szID );
//	else
//		g_pX2SMTPMail->SMTPSetSenderName( "????? ??? ???? ??????" );
//
//
//	g_pX2SMTPMail->SMTPSetSenderMail("escrash@kog.co.kr");
//	g_pX2SMTPMail->SMTPSetReplyTo("escrash@kog.co.kr"); // ??? ???
//
//	string mailName;
//	ConvertWCHARToChar( mailName, m_MailNameToFindHack.c_str() );
//	g_pX2SMTPMail->SMTPSetSubject( (char*)mailName.c_str() );
//	stringstream wstrstm;
//
//	// ???? ??? ????? ????
//	//g_pX2SMTPMail->SMTPAddRecipient("demiahun@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("eshack@kogstudios.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("carl79@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("comicprince@kog.co.kr");
//	g_pX2SMTPMail->SMTPAddRecipient("operator@kog.co.kr");
//
//	//g_pX2SMTPMail->SMTPAddRecipient("oasis907@naver.com");
//	//g_pX2SMTPMail->SMTPAddRecipient("megagame@kog.co.kr");
//
//
//	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
//	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
//
//	set<wstring>::iterator sit;
//
//#ifdef _IN_HOUSE_
//	wstrstm << "[IN HOUSE] " << strComm <<"\n\n";
//#else
//	wstrstm << strComm <<"\n\n";
//#endif
//
//
//#ifdef MORE_INFO_ABOUT_HACK
//	// ????? ????
//	AddSystemInfo(wstrstm);
//
//	string strIp;
//	ConvertWCHARToChar( strIp, m_wstrPublicIp.c_str() );
//	wstrstm << strComm << " : " << strIp << "\n\n";
//
//
//	// ???��??? ????
//	AddProcessInfo(wstrstm);
//
//#endif // MORE_INFO_ABOUT_HACK
//
//	g_pX2SMTPMail->SMTPSetMessageBody((char*)wstrstm.str().c_str());
//
//	string strUserId = ANTI_HACK_STRING_UserID_Hack_ScreenShot;
//	wstring wstrUserId = ANTI_HACK_WSTRING_UserID_Hack_ScreenShot;
//
//	if( m_strUserIdToFindHack.empty() == false )
//	{
//		ConvertWCHARToChar( strUserId, m_strUserIdToFindHack.c_str() );
//		wstrUserId = m_strUserIdToFindHack;
//	}
//	strUserId = strUserId + ".jpg";
//	wstrUserId = wstrUserId + L".jpg";
//
//	MakeHackScreenShot( strUserId.c_str() );
//	// ???? ��?? ???? ????
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\mailtest2.exe");
//	//g_pX2SMTPMail->SMTPAddAttachment("c:\\test2.jpg");
//	wchar_t szFilePath[1024] = _T("");
//	wchar_t strHackImageBuffer[1024];
//	GetCurrentDirectory( _MAX_PATH, szFilePath );
//	StringCchPrintf(strHackImageBuffer, 1024,  _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//    //wsprintf(strHackImageBuffer, _T("%s\\%s"), szFilePath, wstrUserId.c_str());
//	string strNickName = "";
//	ConvertWCHARToChar( strNickName, strHackImageBuffer );
//
//	g_pX2SMTPMail->SMTPAddAttachment(strNickName.c_str());
//
//	if( g_pX2SMTPMail->SMTPSend() )
//	{
//		//The mail was send successfully
//	}
//	else
//	{
//		//Unable to send the mail
//	}
//	// ????? ???, ��?????? ????
//	g_pX2SMTPMail->SMTPClearRecipient();
//
//	DeleteFile( wstrUserId.c_str() );
//
//	//}} oasis907 : ????? [2009.12.21] // ?? CSmtp
//
//	THEMIDA_VM_END
//	ELSWORD_VIRTUALIZER_END
////#endif
}

bool CX2Main::GetPresentArrival( CX2StateMenu::PresentArrival& presentArrival )
{
	if ( m_quePresentArrival.empty() == true )
		return false;

	presentArrival = m_quePresentArrival.front();
	m_quePresentArrival.pop();

	return true;
}

void CX2Main::PushPresentArrival( KEGS_PRESENT_MESSAGE_TO_RECEIVER_NOT& kEvent )
{
	CX2StateMenu::PresentArrival presentArrival;
	presentArrival.m_kBillProductInfo = kEvent.m_kBillProductInfo;
	presentArrival.m_PresentMsg = kEvent.m_wstrMessage;
	presentArrival.m_SenderNickName = kEvent.m_wstrSenderNickName;

	m_quePresentArrival.push( presentArrival );
}

void CX2Main::DeleteNMCachData()
{
	HANDLE				hSearch;
	WIN32_FIND_DATA		fd;

	wstring nmDataFolder = L"C:\\Documents and Settings\\All Users\\Application Data\\Nexon\\Common\\NMDATA";
	wstring tempAllFile = nmDataFolder + L"\\*.*";
	hSearch = FindFirstFile( tempAllFile.c_str(), &fd );

	vector< wstring > vecFilePathToDelete;

	if(hSearch == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if( wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..") )
		{
			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{

			}
			else
			{
				WCHAR			wstrNewSearchFile[256];
				std::wstring	wstrFile;
				std::wstring	wstrFileName;

				wcscpy( wstrNewSearchFile, nmDataFolder.c_str() );
				wcscat( wstrNewSearchFile, L"\\");
				wcscat( wstrNewSearchFile, fd.cFileName );

				if ( ::DeleteFile( wstrNewSearchFile ) != FALSE )
				{
					int i = 0;
				}
			}
		}

	}while( FindNextFile( hSearch, &fd ) );

	FindClose(hSearch);

	if ( ::DeleteFile( L"C:\\Documents and Settings\\All Users\\Application Data\\Nexon\\Common\\nmservice.ini" ) != FALSE )
	{
		int i = 0;
	}
}

void CX2Main::LoadLuaEnum()
{
	// luamanager?? enum.lua ?? ???????? ????? ?????? ?��??? CKTDXApp::loadkluamanagertemplate() ???????
	// luabinder?? enum.lua ?? ???????? ????? ?????? ?��??? cx2main::loadluaenum() ???????

	OpenScriptFile( L"Enum.lua" );
	OpenScriptFile( L"DungeonEnum.lua" );
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "KGCMassFileManager", g_pKTDXApp->GetDeviceManager()->GetMassFileManager() );
	OpenScriptFile( L"MassFileMappingList.lua" );
    OpenScriptFile( L"StringID_def.lua" );
}

//{{ 2008. 1. 2  ??????  ????? ??
void CX2Main::ChangeSpirit( int iChangeSpirit )
{
	if( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR &&
		g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{

		KEGS_ADMIN_CHANGE_SPIRIT_REQ kEGS_ADMIN_CHANGE_SPIRIT_REQ;
		kEGS_ADMIN_CHANGE_SPIRIT_REQ.m_iSpirit = iChangeSpirit;

		g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CHANGE_SPIRIT_REQ, kEGS_ADMIN_CHANGE_SPIRIT_REQ );
		AddServerPacket( EGS_ADMIN_CHANGE_SPIRIT_ACK );
	}
}
//}}

//{{ 2008. 1. 25  ??????  VP ??
void CX2Main::ChangeVSPoint( int iRating, int iRPoint, int iAPoint )
{
	if( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR &&
		g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{

		KEGS_ADMIN_CHANGE_VSPOINT_REQ kEGS_ADMIN_CHANGE_VSPOINT_REQ;
		kEGS_ADMIN_CHANGE_VSPOINT_REQ.m_iRating	= iRating;
		kEGS_ADMIN_CHANGE_VSPOINT_REQ.m_iRPoint = iRPoint;
		kEGS_ADMIN_CHANGE_VSPOINT_REQ.m_iAPoint = iAPoint;

		g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CHANGE_VSPOINT_REQ, kEGS_ADMIN_CHANGE_VSPOINT_REQ );
		AddServerPacket( EGS_ADMIN_CHANGE_VSPOINT_ACK );
	}
}
//}}

//{{ 2008. 2. 18  ??????  ???? ????? ??
void CX2Main::ChangeUnitClass( char cUnitClass )
{
	if( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR &&
		g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{

		KEGS_ADMIN_CHANGE_UNIT_CLASS_REQ kEGS_ADMIN_CHANGE_UNIT_CLASS_REQ;
		kEGS_ADMIN_CHANGE_UNIT_CLASS_REQ.m_cUnitClass = cUnitClass;

		g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_CHANGE_UNIT_CLASS_REQ, kEGS_ADMIN_CHANGE_UNIT_CLASS_REQ );
		AddServerPacket( EGS_ADMIN_CHANGE_UNIT_CLASS_ACK );
	}
}
void CX2Main::ModifyChangeUnitClass( const char* UnitClassName )
{
	wstring wsUnitClassName;
	ConvertCharToWCHAR( wsUnitClassName, UnitClassName );

	if ( wsUnitClassName.compare( L"?????" ) == 0 )		{ChangeUnitClass(CX2Unit::UC_ELSWORD_SWORDMAN);	}
	else if ( wsUnitClassName.compare( L"??????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELSWORD_KNIGHT);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELSWORD_MAGIC_KNIGHT);}
	else if ( wsUnitClassName.compare( L"????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELSWORD_SHEATH_KNIGHT);}
	else if ( wsUnitClassName.compare( L"?��????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELSWORD_LORD_KNIGHT);}
	else if ( wsUnitClassName.compare( L"???????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELSWORD_RUNE_SLAYER);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELSWORD_INFINITY_SWORD);}

	else if ( wsUnitClassName.compare( L"?????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_VIOLET_MAGE);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_HIGH_MAGICIAN);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_DARK_MAGICIAN);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_BATTLE_MAGICIAN);}
	else if ( wsUnitClassName.compare( L"?????????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_ELEMENTAL_MASTER);}
	else if ( wsUnitClassName.compare( L"?????????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_VOID_PRINCESS);}
	else if ( wsUnitClassName.compare( L"???????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARME_DIMENSION_WITCH);}

	else if ( wsUnitClassName.compare( L"????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_ELVEN_RANGER);}
	else if ( wsUnitClassName.compare( L"???????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_COMBAT_RANGER);}
	else if ( wsUnitClassName.compare( L"???????��?????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_SNIPING_RANGER);}
	else if ( wsUnitClassName.compare( L"????��?????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_TRAPPING_RANGER);}
	else if ( wsUnitClassName.compare( L"??????��" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_WIND_SNEAKER);}
	else if ( wsUnitClassName.compare( L"???????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_GRAND_ARCHER);}
	else if ( wsUnitClassName.compare( L"????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_LIRE_NIGHT_WATCHER);}

	else if ( wsUnitClassName.compare( L"?????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_FIGHTER);}
	else if ( wsUnitClassName.compare( L"???????��" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_SOUL_TAKER);}
	else if ( wsUnitClassName.compare( L"????????��" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_OVER_TAKER);}
	else if ( wsUnitClassName.compare( L"????????��" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_WEAPON_TAKER);}
	else if ( wsUnitClassName.compare( L"??????��????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_BLADE_MASTER);}
	else if ( wsUnitClassName.compare( L"???????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_RECKLESS_FIST);}
	else if ( wsUnitClassName.compare( L"?????��???" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_RAVEN_VETERAN_COMMANDER);}


	else if ( wsUnitClassName.compare( L"???" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_NASOD);}
	else if ( wsUnitClassName.compare( L"???????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_EXOTIC_GEAR);}
	else if ( wsUnitClassName.compare( L"????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_ARCHITECTURE);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_ELECTRA);}
	else if ( wsUnitClassName.compare( L"???????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_CODE_NEMESIS);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_CODE_EMPRESS);}
	else if ( wsUnitClassName.compare( L"???????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_EVE_BATTLE_SERAPH);}

	else if ( wsUnitClassName.compare( L"?" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_IRON_CANNON);}
	else if ( wsUnitClassName.compare( L"????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_FURY_GUARDIAN);}
	else if ( wsUnitClassName.compare( L"???��????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_SHELLING_GUARDIAN);}
	else if ( wsUnitClassName.compare( L"?????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_IRON_PALADIN);}
	else if ( wsUnitClassName.compare( L"????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_DEADLY_CHASER);}
	else if ( wsUnitClassName.compare( L"??????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_CHUNG_TACTICAL_TROOPER);}

	else if ( wsUnitClassName.compare( L"???" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARA_MARTIAL_ARTIST);}
	else if ( wsUnitClassName.compare( L"???" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARA_LITTLE_HSIEN);}
	else if ( wsUnitClassName.compare( L"???" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ARA_SAKRA_DEVANAM);}

	// ?? ???
	else if ( wsUnitClassName.compare( L"???????" ) == 0 )		{ChangeUnitClass(CX2Unit::UC_ELESIS_KNIGHT);	}
	else if ( wsUnitClassName.compare( L"????��????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELESIS_GRAND_MASTER);}
	else if ( wsUnitClassName.compare( L"??????????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELESIS_SABER_KNIGHT);}
	else if ( wsUnitClassName.compare( L"????��????" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELESIS_PYRO_KNIGHT);}
	else if ( wsUnitClassName.compare( L"??????��???" ) == 0 ) {ChangeUnitClass(CX2Unit::UC_ELESIS_BLAZING_HEART);}

}
//}}

//{{ 2008. 10. 24  ??????	???? ??
void CX2Main::GetTitleCheat( int iTitleID, int iPeriod /*= 0*/ )
{
	if( g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR &&
		g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{

		KEGS_ADMIN_GET_TITLE_CHEAT_REQ kEGS_ADMIN_GET_TITLE_CHEAT_REQ;
		kEGS_ADMIN_GET_TITLE_CHEAT_REQ.m_iTitleID = iTitleID;
		kEGS_ADMIN_GET_TITLE_CHEAT_REQ.m_iPeriod  = iPeriod;

		g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_GET_TITLE_CHEAT_REQ, kEGS_ADMIN_GET_TITLE_CHEAT_REQ );
		AddServerPacket( EGS_ADMIN_GET_TITLE_CHEAT_ACK );
	}
}
//}}

void CX2Main::PopupReservedMessage()
{
	D3DXVECTOR2 vOffsetPos(0, 0);
	for( UINT i=0; i<m_vecReservedMessagePopUp.size(); i++ )
	{
		const ReservedMessagePopUp& reservedMsg = m_vecReservedMessagePopUp[i];

		for( UINT j=0; j<reservedMsg.m_vecTargetState.size(); j++ )
		{
			CX2Main::X2_STATE eState = reservedMsg.m_vecTargetState[j];
			if( GetNowStateID() == CX2Main::XS_SERVER_SELECT ) // ???? ????????? ???? ??? ????? ???
			{
				if( vOffsetPos.x < 100 && vOffsetPos.y < 100 )
					vOffsetPos += D3DXVECTOR2( 10, 10 );

				KTDGUIOKMsgBox( D3DXVECTOR2(250, 300) + vOffsetPos, reservedMsg.m_Message.c_str(), (CKTDXStage*) GetNowState(),
					-1 );
				m_vecReservedMessagePopUp.erase( m_vecReservedMessagePopUp.begin() + i );
				i--;
				break;
			}
			else if( GetNowStateID() == eState )
			{
				if( vOffsetPos.x < 100 && vOffsetPos.y < 100 )
					vOffsetPos += D3DXVECTOR2( 10, 10 );

				KTDGUIOKMsgBox( D3DXVECTOR2(250, 300) + vOffsetPos, reservedMsg.m_Message.c_str(), (CKTDXStage*) GetNowState(),
					reservedMsg.m_iOKMsg );
				m_vecReservedMessagePopUp.erase( m_vecReservedMessagePopUp.begin() + i );
				i--;
				break;
			}
		}
	}
}



// @timeOutMsg : ?��??????? ?? ????? ?? ??????? ?????? UI custom message
void CX2Main::AddTimedMessagePopup( TimedMessagePopUp::MESSAGE_TYPE eMessageType, TimedMessagePopUp::MESSAGE_BOX_TYPE eBoxType,
								   const TimedMessagePopUp::TimedPopupUserData& userData, const float fTime, const wstring& wstrMsg,
								   CKTDXStage* pStage, int okMsg, int cancelMsg, int timeOutMsg /*= -1*/ )
{
	TimedMessagePopUp timedPopup;
	timedPopup.m_UserData		= userData;
	timedPopup.m_eMessageType	= eMessageType;
	timedPopup.m_fTimeLeft		= fTime;
	timedPopup.m_wstrMsg		= wstrMsg;
	timedPopup.m_iTimeOutMsg	= timeOutMsg;

	//{{ kimhc // 2010.01.22 // list?? ????
	m_listTimedMessagePopUp.insert( m_listTimedMessagePopUp.begin(), timedPopup );

	list< TimedMessagePopUp >::iterator ItrTimedPopUp = m_listTimedMessagePopUp.begin();

	D3DXVECTOR2 vPos(270.f,145.f);

	wstring wstrCustomLuaFIleName = L"";
	if ( eMessageType == CX2Main::TimedMessagePopUp::MT_AGREE_COUPLE )
	{
		wstrCustomLuaFIleName = L"DLG_UI_Couple_Accept.lua";
		vPos = D3DXVECTOR2( 388, 282 );
	}


	switch ( eBoxType )
	{
	default:
	case TimedMessagePopUp::MBT_NO_BUTTON:
		{
			if ( L"" != wstrCustomLuaFIleName )
			{
				ItrTimedPopUp->m_pDialog = KTDGUIMsgBox( vPos, wstrMsg.c_str(), pStage, wstrCustomLuaFIleName );
			}
			else
			{
				ItrTimedPopUp->m_pDialog = KTDGUIMsgBox( vPos, wstrMsg.c_str(), pStage );
			}
		}
		break;


	case TimedMessagePopUp::MBT_OK:
		{

			if ( L"" != wstrCustomLuaFIleName )
			{
				ItrTimedPopUp->m_pDialog = KTDGUIOKMsgBox( vPos, wstrMsg.c_str(), pStage, okMsg, fTime, wstrCustomLuaFIleName );
			}
			else
			{
				ItrTimedPopUp->m_pDialog = KTDGUIOKMsgBox( vPos, wstrMsg.c_str(), pStage, okMsg );
			}
		}
		break;

	case TimedMessagePopUp::MBT_OK_CANCEL:
	{
		if ( L"" != wstrCustomLuaFIleName )
		{
			ItrTimedPopUp->m_pDialog = KTDGUIOkAndCancelMsgBox( vPos, wstrMsg.c_str(), okMsg, pStage, cancelMsg, wstrCustomLuaFIleName );
		}
		else
		{
		ItrTimedPopUp->m_pDialog = KTDGUIOkAndCancelMsgBox( vPos, wstrMsg.c_str(), okMsg, pStage, cancelMsg );
		}
	}
	break;

	}

	ItrTimedPopUp->m_pDialog->SetModal(true);
	ItrTimedPopUp->m_pDialog->ClearFocus();
// 	m_vecTimedMessagePopUp.push_back( timedPopup );
//
//
// 	// note!! TimedMessagePopUp ???????? m_pDialog?? ????? ?????? ?????? reference ?? ????? ??????? ?????
// 	// fix!!!! vector?? ???????? ??? ???? ??????? ?????? ?????? ???????????
// 	TimedMessagePopUp& _timedPopup = m_vecTimedMessagePopUp[ m_vecTimedMessagePopUp.size() - 1 ];
// 	switch( eBoxType )
// 	{
// 	default:
// 	case TimedMessagePopUp::MBT_NO_BUTTON:
// 		{
// 			_timedPopup.m_pDialog = KTDGUIMsgBox( D3DXVECTOR2(250,300), wstrMsg.c_str(), pStage );
// 		} break;
//
// 	case TimedMessagePopUp::MBT_OK:
// 		{
// 			_timedPopup.m_pDialog = KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300), wstrMsg.c_str(), pStage, okMsg );
// 		} break;
//
// 	case TimedMessagePopUp::MBT_OK_CANCEL:
// 		{
// 			_timedPopup.m_pDialog = KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 250, 300 ), wstrMsg.c_str(), okMsg, pStage, cancelMsg );
// 		} break;
// 	}
	//}} kimhc // 2010.01.22 // list?? ????
}



void CX2Main::RemoveTimedMessagePopup( TimedMessagePopUp::MESSAGE_TYPE eMessageType, const TimedMessagePopUp::TimedPopupUserData& userData )
{
	//{{ kimhc // 2010.01.22 // list?? ????
	list< TimedMessagePopUp >::iterator ItrTimedPopUp = m_listTimedMessagePopUp.begin();

	while ( ItrTimedPopUp != m_listTimedMessagePopUp.end() )
	{
		if ( ItrTimedPopUp->m_eMessageType == eMessageType && ItrTimedPopUp->m_UserData == userData )
		{
			m_listTimedMessagePopUp.erase( ItrTimedPopUp );
			break;
		}

		ItrTimedPopUp++;
	}
// 	for( UINT i=0; i<m_vecTimedMessagePopUp.size(); i++ )
// 	{
// 		TimedMessagePopUp& timedPopup = m_vecTimedMessagePopUp[i];
// 		if( timedPopup.m_UserData == userData )
// 		{
// 			m_vecTimedMessagePopUp.erase( m_vecTimedMessagePopUp.begin() + i );
// 			break;
// 		}
// 	}
	//}} kimhc // 2010.01.22 // list?? ????
}

std::string CX2Main::GetMachineId()
{
	ELSWORD_VIRTUALIZER_START

	CSystemInfo* pSystemInfo = new CSystemInfo();
	if( pSystemInfo == NULL )
		return "";

	pSystemInfo->Init();
	const unsigned char* pMachineId = pSystemInfo->GetMachineId();

	std::string wstrMachineId = base64_encode(pMachineId, 16);
	delete pSystemInfo;

	ELSWORD_VIRTUALIZER_END

	return wstrMachineId;
}


//void CX2Main::ClearTimedMessagePopup()
//{
//	m_vecTimedMessagePopUp.resize(0);
//}


void CX2Main::DllManagerReport()
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	//THEMIDA_VM_START

	SiCX2DLLManager()->ReportEvent();

	//THEMIDA_VM_END

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
}


CX2Main::TimedMessagePopUp* CX2Main::GetTimedMessagePopup( CKTDGUIDialogType pDialog )
{
	//{{ kimhc // 2010.01.22 // list?? ????
	list< TimedMessagePopUp >::iterator ItrTimedPopUp = m_listTimedMessagePopUp.begin();

	while ( ItrTimedPopUp != m_listTimedMessagePopUp.end() )
	{
		if ( ItrTimedPopUp->m_pDialog == pDialog )
		{
			return &(*ItrTimedPopUp);
		}

		ItrTimedPopUp++;
	}
// 	for( UINT i=0; i<m_vecTimedMessagePopUp.size(); i++ )
// 	{
// 		if( m_vecTimedMessagePopUp[i].m_pDialog == pDialog )
// 		{
// 			return &m_vecTimedMessagePopUp[i];
// 		}
// 	}
	return NULL;
}


bool CX2Main::KeyProcess()
{


	if( NULL == GetNowState() )
	{
		return false;
	}

#if X2_DEV_TOOLS
#endif // defined( _IN_HOUSE_ ) || defined( _OPEN_TEST_ )




	//{{ 2011.9.16	?????  ???? ?????? ��?? ????
	if( g_pData != NULL )
	{
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_SCROLL ) == TRUE )
		{
			if( IsCapturing() == false )
			{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
				KOGGamePerformanceCheck::GetInstance()->Pause();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

				Bandi_StartVideoCapture();
			}
			else
			{
				Bandi_EndVidioCapture();

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
				KOGGamePerformanceCheck::GetInstance()->Resume();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

                if ( g_pData->GetGameUDP() != NULL )
                    g_pData->GetGameUDP()->RemoveAllPendingPingSends();
			}

			return true;
		}
	}
	//}}

	bool bHideDialog = false;
	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		bHideDialog = true;


	// ????: stateoption ??????? ?????? ????? KeyProcess?? ???????? ???.
	if( g_pData != NULL && g_pData->GetCashShop() != NULL && g_pData->GetCashShop()->GetOpen() == false
		&& g_pData != NULL && g_pData->GetUIManager() != NULL &&
		g_pData->GetUIManager()->GetUIInventory() != NULL && g_pData->GetUIManager()->GetUIInventory()->GetPlayAttractionItem() == false
		)
	{


		if( g_pInstanceData != NULL &&
			GetNowStateID() == CX2Main::XS_VILLAGE_MAP ||
			GetNowStateID() == CX2Main::XS_BATTLE_FIELD &&
			g_pInstanceData->GetIsDungeonLounge() == false )
		{
			// ???
			if ( bHideDialog == false && GET_KEY_STATE( GA_PVE ) == TRUE ) //DIK_P
			{
				/// ????? ????? ??? ???? ??? ????
				const UINT uiBattleFieldId = g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam();

				if( true == g_pData->GetBattleFieldManager().GetIsBossRaidFieldByFieldID( uiBattleFieldId ) )
					return true;

				if( g_pData->GetPartyManager()->DoIHaveParty() == true && g_pData->GetPartyManager()->AmIPartyLeader() == false )
					return true;

				if( true == g_pData->GetPartyManager()->GetProcessDungeonMatch() || true == g_pData->GetPartyManager()->GetProcessPvpMatch() )
					return true;

				if( g_pData->GetPartyManager()->DoIHaveParty() == true &&
					g_pData->GetPartyManager()->GetMyPartyData()->m_bPvpParty == true &&
					g_pData->GetPartyManager()->GetProcessPvpMatch() == false  )
				{
					// ????????? ????
					if( g_pData->GetPartyManager()->ChangePartyType(false) == false )
					{
						// ??????? ??????
						return true;
					}
				}

				GetPartyUI()->SetDungeonPartyMenu(true);
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PARTY);
				return true;
			}
			// ???????
			if ( bHideDialog == false && GET_KEY_STATE( GA_PVP ) == TRUE ) // DIK_L
			{
				/// ????? ????? ??? ????? ??? ????
				const UINT uiBattleFieldId = g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam();

				if( true == g_pData->GetBattleFieldManager().GetIsBossRaidFieldByFieldID( uiBattleFieldId ) )
					return true;

				if ( (NULL != g_pTFieldGame && false == g_pTFieldGame->GetJoinNpc()) || CX2Main::XS_BATTLE_FIELD == GetNowStateID())
				{

					if( true == g_pData->GetPartyManager()->GetProcessDungeonMatch() || true == g_pData->GetPartyManager()->GetProcessPvpMatch() )
						return true;


					if( g_pData->GetPartyManager()->DoIHaveParty() == true && g_pData->GetPartyManager()->GetMyPartyData()->m_bPvpParty == false )
					{
						if( g_pData->GetPartyManager()->DoIHaveParty() == true && g_pData->GetPartyManager()->AmIPartyLeader() == false )
						{
							return true;
						}
						if( g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberCount() >= 4 )
						{
							KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_12715 ), (CKTDXStage*)GetNowState() );
							return true;
						}

						for(int iParty=0; iParty<g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberCount(); ++iParty)
						{
							if( g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( iParty ) != NULL &&
								g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( iParty )->m_iUnitLevel < 10 )
							{
								// 10?? ????? ??????? ??? ????????? ?????? ???????.
								KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_13548 ), (CKTDXStage*)GetNowState() );
								return true;
							}
						}
						// ????????? ????
						if( g_pData->GetPartyManager()->ChangePartyType(true) == false )
						{
							// ??????? ??????
							return true;
						}
					}
					else if( g_pData->GetSelectUnitLevel() < 10 )
					{
						// 10?? ????? ????????? ????? ?? ???????.
						KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_13549 ), (CKTDXStage*)GetNowState() );
						return true;
					}

					GetPartyUI()->SetPvpPartyMenu(true);
					g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PARTY);
					return true;
				}
			}
		}

		if( true == GetPartyUI()->GetShowDungeonMenu() )
		{//??????? ???????? ???? ????? ????????? ????
			return true;
		}
		wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
		D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77
		CX2StateOption *pStateOption = (CX2StateOption*)GetNowState();

		// ?????
		if ( bHideDialog == false && GET_KEY_STATE( GA_QUEST ) == TRUE )
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUEST_NEW);
			return true;
		}

#if X2_DEV_TOOLS
		if ( bHideDialog == false && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
		{
			if ( bHideDialog == false && g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_0) == TRUE )
				g_pInstanceData->SetFrameScale( !g_pInstanceData->GetFrameScale() );
		}
#endif
		// ?????
		/*
		if ( bHideDialog == false && GET_KEY_STATE( GA_MAP ) == TRUE ) //DIK_M
		{
			switch(GetNowStateID())
			{
			case CX2Main::XS_SQUARE_GAME:
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_BATTLE_FIELD:
				{
					CX2WorldMapUI *pWorldMapUI = g_pInstanceData->GetMiniMapUI()->GetWorldMapUI();

					if( pWorldMapUI != NULL )
					{
						if( true == pWorldMapUI->GetShow() )
						{
							pWorldMapUI->OpenWorldMap( false );
							pWorldMapUI->OpenFieldMap( false );
							pWorldMapUI->CloseAllToolTip();
							return true;
						}
						else
						{
							pWorldMapUI->OpenWorldMap( true );
							pWorldMapUI->UpdateWorldMap();

							if(GetNowStateID() == CX2Main::XS_VILLAGE_MAP)
							{
								int villageID = g_pData->GetLocationManager()->GetCurrentVillageID();
								pWorldMapUI->OpenFieldMap( true, villageID );
								pWorldMapUI->UpdateFieldMap();
							}
							else if( GetNowStateID() == CX2Main::XS_BATTLE_FIELD )
							{
								CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
								int iFiledID = battleFieldManager.GetBattleFieldIdWhereIam();
								pWorldMapUI->OpenFieldMap( true, iFiledID );
								pWorldMapUI->UpdateFieldMap();
							}
						}
					}
				} break;

			case CX2Main::XS_DUNGEON_GAME:
				{
					if( NULL != g_pX2Game && g_pX2Game->GetOpenChatBox() == false )
					{
						if( GetIsPlayingTutorial() == false )
						{
							CX2StateDungeonGame* pState = (CX2StateDungeonGame*) GetNowState();
							pState->ToggleDungeonMapUI();
						}
						return true;
					}

				} break;
			default:
				break;
			}
			return true;
		}
		*/
		// ��????????
		if ( bHideDialog == false && GET_KEY_STATE( GA_INFO ) == TRUE )

		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_CHARINFO);
			return true;
		}

		// ?��???
		if ( bHideDialog == false && GET_KEY_STATE( GA_INVENTORY ) == TRUE )
		{
			if( g_pData->GetCubeOpenImageManager() != NULL && g_pData->GetCubeOpenImageManager()->IsNowResultEventCubePlaying() == true)
				return true;
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN);
			return true;
		}

		// ??
		if ( bHideDialog == false && GET_KEY_STATE( GA_PET ) == TRUE )
		{
			// oasis907 : ????? [2010.9.15] // ?? ???? ? ????
			switch(GetNowStateID())
			{
			case XS_SQUARE_GAME:
			case XS_PVP_LOBBY:
			case XS_PVP_ROOM:
			case XS_WEDDING_GAME:
				break;
			default:
				{
					g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PET_LIST);
					return true;
				}
			}
		}

		// ???
		if ( bHideDialog == false && GET_KEY_STATE( GA_OPTION ) == TRUE )
		{
			//g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_OPTION);
			if(pStateOption->GetOptionWindowOpen() == true)
				pStateOption->CloseOptionWindow();
			else
				pStateOption->OpenOptionWindow();
			return true;
		}

		// ?��???
		if ( bHideDialog == false && GET_KEY_STATE( GA_WARP ) == TRUE )
		{
			if( g_pData->GetCubeOpenImageManager() != NULL && g_pData->GetCubeOpenImageManager()->IsNowResultEventCubePlaying() == true)
				return true;
			// ???? UI ??????
			if( NULL != g_pData->GetMyUser()
				&& NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				CX2StateMenu* pStateMenu = (CX2StateMenu*) GetNowState();
				if( pStateMenu != NULL )
				{
					if( pStateMenu->GetShowWarpDest() == false )
					{
						if( true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_bWarpVip )
							pStateMenu->ClickWarpButton( false );
						else
							pStateMenu->ClickWarpButton( true );
					}
					else
					{
						// ??????
						pStateMenu->SetShowWarpDest( false );
					}
				}
			}

			return true;
		}

		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_DELETE) == TRUE &&
			(	g_pData->GetUIManager()->GetUIPersonalTrade() == NULL ||
				g_pData->GetUIManager()->GetUIPersonalTrade() != NULL && g_pData->GetUIManager()->GetUIPersonalTrade()->IsTrading() == false
			)
		)
		{

			switch ( GetNowStateID() )
			{
			case CX2Main::XS_PVP_GAME:
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_BATTLE_FIELD:
			case CX2Main::XS_TRAINING_GAME:
				{
					if( g_pTFieldGame == NULL || ( g_pTFieldGame != NULL && g_pTFieldGame->GetTalkNpc() == false ) )
					{
						bool bShow = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetShow();
						g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( !bShow );


						bool bHide = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog();
						if( g_pInstanceData->GetMiniMapUI() != NULL )
						{
							g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( g_pInstanceData->GetMiniMapUI()->GetCurrentMiniMapType(), !bHide );
						}
						if( g_pInstanceData->GetMiniMapUI() != NULL && g_pInstanceData->GetMiniMapUI()->GetWorldMapUI() != NULL )
						{
							g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenWorldMap(false);
							g_pInstanceData->GetMiniMapUI()->GetWorldMapUI()->OpenFieldMap(false);
						}

					}
					return true;
				}break;
			case CX2Main::XS_WEDDING_GAME:
				{
					bool bShow = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetShow();
					g_pKTDXApp->GetDGManager()->GetDialogManager()->SetShow( !bShow );

					return true;
				} break;

			default:
				break;
			}
		}




		// ��?��??(?????)
		// ????/???? ????????? ??????? ???? ??????? ???.
		if(GetNowStateID() != CX2Main::XS_DUNGEON_GAME && GetNowStateID() != CX2Main::XS_PVP_GAME)
		{
			if ( bHideDialog == false &&
				GET_KEY_STATE( GA_COMMUNITY ) == TRUE
				)
			{
				if( NULL != g_pData->GetMessenger() )
				{
					g_pData->GetMessenger()->SetFriendTab(false);
				}
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_COMMUNITY);
				return true;
			}
		}

		// ��?��??(?????)
		if ( bHideDialog == false && GET_KEY_STATE( GA_FRIEND ) == TRUE )
		{
			g_pData->GetMessenger()->SetFriendTab(true);
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_COMMUNITY);
			return true;
		}




		//{{ kimhc // 2009-10-13 // ??? ??UI ????? ????
		// ��?��??(?????)
		if ( bHideDialog == false && GET_KEY_STATE( GA_GUILD ) == TRUE )
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_GUILD);
			return true;
		}
		//}} kimhc // 2009-10-13 // ??? ??UI ????? ????


		if ( bHideDialog == false && GET_KEY_STATE( GA_RELATION ) == TRUE )
		{
			switch ( GetNowStateID() )
			{
			case XS_PVP_GAME:
			case XS_WEDDING_GAME:
				break;

			default:	/// pvp ??? ?????? ??? ??��?? ??? ??
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_RELATION);
				return true;
				break;
			}
		}
		// ???????(???)
		if ( bHideDialog == false && g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_LWIN ) == TRUE )
		{
			//ToggleLobbyMenuWindow(); // ????????��? ????? ????
			return true;
		}

		// ??????? ????
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F9) == TRUE )
		{
			GetGameOption().SetDynamicCamera( !GetGameOption().GetOptionList().m_bDynamicCamera );
			GetGameOption().SaveScriptFile();
			if( pStateOption != NULL && pStateOption->GetOptionWindowOpen() == true )
			{
				pStateOption->InitOtherOption();
			}

			if( true == GetGameOption().GetOptionList().m_bDynamicCamera )
			{
				if( NULL != g_pChatBox )
				{

					g_pChatBox->AddChatLog( GET_STRING( STR_ID_354 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				}
			}
			else
			{
				if( NULL != g_pChatBox )
				{

					g_pChatBox->AddChatLog( GET_STRING( STR_ID_355 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				}
			}
			return true;
		}

		// ?????
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F10) == TRUE )
		{
			GetGameOption().SetSound( !GetGameOption().GetOptionList().m_bSound );
			GetGameOption().SaveScriptFile();
			if( pStateOption != NULL && pStateOption->GetOptionWindowOpen() == true )
			{
				pStateOption->InitOtherOption();
			}

			if( true == GetGameOption().GetOptionList().m_bSound )
			{
				if( NULL != g_pChatBox )
				{

					g_pChatBox->AddChatLog( GET_STRING( STR_ID_356 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				}
			}

			else
			{
				if( NULL != g_pChatBox )
				{

					g_pChatBox->AddChatLog( GET_STRING( STR_ID_357 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				}

			}
		}

		// ?????
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F11) == TRUE )
		{
			GetGameOption().SetMusic( !GetGameOption().GetOptionList().m_bMusic );
			GetGameOption().SaveScriptFile();
			if( pStateOption != NULL && pStateOption->GetOptionWindowOpen() == true )
			{
				pStateOption->InitSoundOption();
			}


			if( true == GetGameOption().GetOptionList().m_bMusic )
			{
				if( NULL != g_pChatBox )
				{

					g_pChatBox->AddChatLog( GET_STRING( STR_ID_358 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				}
			}
			else
			{
				if( NULL != g_pChatBox )
				{

					g_pChatBox->AddChatLog( GET_STRING( STR_ID_359 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
				}
			}
			return true;
		}


// ????, [2014.12.06] J ? ?????? ??, ?????? UI ????
//		if( NULL != g_pData->GetMyUser() )
//			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_SPECIAL_USER )
//			{
//				if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_J) == TRUE )
//				{
//					g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_HERO);
//				}
//			}

		if( NULL != g_pData && NULL != g_pData->GetMyUser() && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
		{
			if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_NUMPAD0 ) == TRUE )
			{
				g_pData->GetMyUser()->ChangeHideSetDesc();
				return true;
			}
		}

	}

	return false;
}







// @wstrTextColor: ????? ????? ?????? ??????? #Cxxxxxx ?��????
// @bEnableColor : ??????? ????????? #Cxxxxxx ?��???? ??????? ????
/*static*/ int CX2Main::LineBreak( wstring& wstrText, const CKTDGFontManager::CUKFont* pFont, const int iTextWidth, const wstring& wstrTextColor, const bool bEnableColor, bool bCutRedundantEmptyLine /*= true*/)
{
	KTDXPROFILE();

	if( NULL == pFont )
		return 1;

	enum CHAR_STATE
	{
		Q_NORMAL,
		Q_SHARP,
		Q_SHARP_C,
	};


	int iNowCharWidth = 0;
	int iNowLineWidth = 0;
	int iNowLineCount = 0;
	int iNowCharIndex = 0;
	wstring wstrLineBuf = L"";
	wstring wstrColorCode = L"";

	CHAR_STATE eNowCharState = Q_NORMAL;
	bool bIsFirstCharOfLine = true;


	if( false == wstrTextColor.empty() )
	{
		wstrLineBuf += wstrTextColor;
	}

	while( iNowCharIndex < (int) wstrText.length() )
	{
		WCHAR wszCharBuf = wstrText[iNowCharIndex];
		wstring wstrCharBuf = wstrText.substr( iNowCharIndex, 1 );

		switch( eNowCharState )
		{
		case Q_NORMAL:
			{
				if( 0 == wstrCharBuf.compare( L"#" ) )
				{
					if( true == bEnableColor )
					{
						wstrLineBuf += wstrCharBuf;
						eNowCharState = Q_SHARP;
					}
					else
					{
						wstrLineBuf += L"*";
						iNowCharWidth = pFont->GetWidth( L"*" );
						iNowLineWidth += iNowCharWidth;
					}
				}
				else if( 0 == wstrCharBuf.compare( L"\n" ) )
				{
					// 09.05.30 ?��? : 2?? ????? \n?? ???????? ???? ??? ????? ???? ????
					if( bIsFirstCharOfLine == false || bCutRedundantEmptyLine == false )
					{
						wstrLineBuf += wstrCharBuf;
						//wstrLineBuf += L" ";
						iNowLineCount++;
						iNowLineWidth = 0;
						bIsFirstCharOfLine = true;

						if( 0 != wstrTextColor.length() )
						{
							wstrLineBuf += wstrTextColor;
						}

						if( 0 != wstrColorCode.length() )
						{
							wstrLineBuf += wstrColorCode;
						}
					}
				}
				else
				{
					if( 0 != wstrCharBuf.compare( L" " ) || false == bIsFirstCharOfLine )
					{
						if( true == bIsFirstCharOfLine )
							bIsFirstCharOfLine = false;
						wstrLineBuf += wstrCharBuf;
						iNowCharWidth = pFont->GetWidth( wszCharBuf );
						iNowLineWidth += iNowCharWidth;
					}
				}

			} break;

		case Q_SHARP:
			{
				if( 0 == wstrCharBuf.compare( L"C" ) )
				{
					wstrLineBuf += wstrCharBuf;
					eNowCharState = Q_SHARP_C;
				}
				else
				{
					if( true == bIsFirstCharOfLine )
						bIsFirstCharOfLine = false;

					wstrLineBuf += wstrCharBuf;
					iNowCharWidth = pFont->GetWidth( wszCharBuf );
					iNowLineWidth += iNowCharWidth;

					eNowCharState = Q_NORMAL;
				}
			} break;

		case Q_SHARP_C:
			{
				if( 0 == wstrCharBuf.compare( L"X" ) )
				{
					wstrLineBuf += wstrCharBuf;
					wstrColorCode = L"";

					if( 0 != wstrTextColor.length() )
					{
						wstrLineBuf += wstrTextColor;
					}

					eNowCharState = Q_NORMAL;
				}
				else // RGB
				{
					// note!! color code?? ?????? ????? ??? ???? ??. ??? color code?? ?????? ?????????? ????
					wstrColorCode = L"#C";
					wstrColorCode += wstrText.substr( iNowCharIndex, 6 );
					wstrLineBuf += wstrText.substr( iNowCharIndex, 6 );
					iNowCharIndex += 5;
					eNowCharState = Q_NORMAL;
				}

			} break;
		}

		if( iNowLineWidth >= iTextWidth && Q_NORMAL == eNowCharState )
		{
			wstrLineBuf += L"\n";
			//wstrLineBuf += L" ";
			iNowLineCount++;
			iNowLineWidth = 0;
			bIsFirstCharOfLine = true;

			if( 0 != wstrTextColor.length() )
			{
				wstrLineBuf += wstrTextColor;
			}

			if( 0 != wstrColorCode.length() )
			{
				wstrLineBuf += wstrColorCode;
			}
		}

		iNowCharIndex++;
	}


	wstrText = wstrLineBuf;

	return iNowLineCount;
}

/*static*/ bool CX2Main::IsInvalidNickName( const wstring& wstrNickName, wstring& wstrErrorMessage )
{
	if( true == wstrNickName.empty() )
	{
		wstrErrorMessage = GET_STRING( STR_ID_360 );
		return true;
	}

	string strNickName = "";
	ConvertWCHARToChar( strNickName, wstrNickName.c_str() );

	if ( (int)strNickName.size() < 1 )
	{
		wstrErrorMessage = GET_STRING( STR_ID_360 );
		return true;
	}
	else if ( (int)strNickName.size() > 12  )
	{
		wstrErrorMessage = GET_STRING( STR_ID_361 );
		return true;
	}
	else if( NULL != g_pData->GetMyUser() &&
		g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_OPERATOR &&
		false == g_pMain->GetStringFilter()->CheckIsValidString( CX2StringFilter::FT_NICKNAME, wstrNickName.c_str() ) )
	{
		wstrErrorMessage = GET_STRING( STR_ID_362 );
		return true;
	}


	return false;
}


//{{ 2009. 7. 26  ??????	party fever cheat
void CX2Main::IncreasePartyFeverPoint( int iCount )
{

	for( int i = 0; i < iCount; ++i )
	{
		g_pData->GetServerProtocol()->SendID( EGS_ADMIN_INCREASE_PARTY_FEVER_POINT_NOT );
	}
}
//}}

//{{ 2009. 8. 5  ??????		???ID??
void CX2Main::SetGuestUserMode( bool bVal )
{

	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_GUEST_USER_MODE_NOT, bVal );
}
//}}

//{{ ????? : [2010/3/12/] //	???? ??? ???? ??
void CX2Main::WarpByCommand( int iPosition )
{
	int iWorldID;

	switch( iPosition )
	{
	case 0: iWorldID = SEnum::VMI_RUBEN;						break;
	case 1: iWorldID = SEnum::VMI_ELDER;						break;
	case 2: iWorldID = SEnum::VMI_BATTLE_FIELD_ELDER_REST_00;	break;
	case 3: iWorldID = SEnum::VMI_BESMA;						break;
	case 4: iWorldID = SEnum::VMI_BATTLE_FIELD_BESMA_REST_00;	break;
	case 5: iWorldID = SEnum::VMI_ALTERA;						break;
	case 6: iWorldID = SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00;	break;
	case 7: iWorldID = SEnum::VMI_PEITA;						break;
	case 8: iWorldID = SEnum::VMI_BATTLE_FIELD_PEITA_REST_00;	break;
	case 9: iWorldID = SEnum::VMI_VELDER;						break; // ????
	case 10: iWorldID = SEnum::VMI_BATTLE_FIELD_VELDER_REST_00;	break; // ???? ????
	case 11: iWorldID = SEnum::VMI_HAMEL;						break; // ???
	case 12: iWorldID = SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00;	break; // ??? ????
	case 13: iWorldID = SEnum::VMI_SANDER;						break; // ????
	case 14: iWorldID = SEnum::VMI_BATTLE_FIELD_SANDER_REST_00; break; // ???? ????
	default: iWorldID = -1;							break;
	}

	//	??????????? ???? ???
	if( iWorldID != -1 && GetNowStateID() != CX2Main::XS_SQUARE_GAME )
	{
		g_pData->SetStateArg( iWorldID );
		CX2State* pState = (CX2State*) GetNowState();
		pState->Handler_EGS_STATE_CHANGE_FIELD_REQ( iWorldID );
	}
}
//}} ????? : [2010/3/12/] //

void CX2Main::FieldWarpByCommand( int iPosition )
{
	int iWorldID = SEnum::VMI_INVALID;
	switch( iPosition )
	{
	case 1: iWorldID = SEnum::VMI_BATTLE_FIELD_RUBEN_FIELD_01;			break; //?? ???1

	case 2: iWorldID = SEnum::VMI_BATTLE_FIELD_ELDER_FIELD_01;			break; //???? ???1
	case 3: iWorldID = SEnum::VMI_BATTLE_FIELD_ELDER_FIELD_02;			break; //???? ???2

	case 4: iWorldID = SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_01;			break; //?????? ???1
	case 5: iWorldID = SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_02;			break; //?????? ???2
	case 6: iWorldID = SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_03;			break; //?????? ???3

	case 7: iWorldID = SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_01;			break; //????? ???1
	case 8: iWorldID = SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_02;			break; //????? ???2
	case 9: iWorldID = SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_03;			break; //????? ???3

	case 10: iWorldID = SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_01;			break; //????? ???1
	case 11: iWorldID = SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_02;			break; //????? ???2
	case 12: iWorldID = SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_03;			break; //????? ???3

	case 13: iWorldID = SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_01;		break; //???? ???1
	case 14: iWorldID = SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_02;		break; //???? ???2
	case 15: iWorldID = SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_03;		break; //???? ???3
	case 16: iWorldID = SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_04;		break; //???? ???4
	case 17: iWorldID = SEnum::VMI_BATTLE_FIELD_VELDER_SHIP_STAGE;		break; // ???? ??

	case 18: iWorldID = SEnum::VMI_BATTLE_FIELD_HAMEL_SHIP_STAGE;		break; // ??? ??
	case 19: iWorldID = SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_01;			break; //??? ???1
	case 20: iWorldID = SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_02;			break; //??? ???2
	case 21: iWorldID = SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_03;			break; //??? ???3
	case 22: iWorldID = SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_04;			break; //??? ???4

	case 23: iWorldID = SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_01;		break; //???? ????? ???
	case 24: iWorldID = SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_02;		break; //???? ???2
	case 25: iWorldID = SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_03;		break; // ??????? ??? ????
	case 26: iWorldID = SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_04;		break; // ?????? ???????
	default: iWorldID = SEnum::VMI_INVALID;								break;
	}

	CX2State* pState = (CX2State*) GetNowState();
	pState->Handler_EGS_JOIN_BATTLE_FIELD_REQ(iWorldID, 1 );
	return;
}

void CX2Main::MakeHackScreenShotTemp( const char *pFileName )
{
	try
	{
		HBITMAP hBitmap;
		RECT rect;
		HDC hMemDC;
		HDC hScrDC;
		HBITMAP hOldBitmap;

		//DC?? ??��?.

		HWND pDesktop = GetDesktopWindow();

		// ??��???? ???? ???????? ????? ???????.
		GetClientRect( pDesktop, &rect );


		// ?????? ??? DC?? ??? DC?? ????? ??? ??????? ???????.
		hScrDC = GetDC( pDesktop ); CreateDC( L"DISPLAY", NULL, NULL, NULL );
		hMemDC = CreateCompatibleDC( hScrDC );
		hBitmap = CreateCompatibleBitmap( hScrDC, rect.right - rect.left, rect.bottom - rect.top );
		hOldBitmap = (HBITMAP)SelectObject( hMemDC, hBitmap );

		// ????? ??? ????????? ???????.
		BitBlt( hMemDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
			hScrDC, rect.left, rect.top, SRCCOPY );
		SelectObject( hMemDC, hOldBitmap );

		CxImage cimage( CXIMAGE_FORMAT_JPG );

		if( cimage.CreateFromHBITMAP( hBitmap ) )
		{
			cimage.SetJpegQuality( 75 );
			cimage.Save( pFileName, CXIMAGE_FORMAT_JPG );
		}

		DeleteDC( hMemDC );
		DeleteDC( hScrDC );
		DeleteObject( hBitmap );
		DeleteObject( hOldBitmap );
	}
	catch (...)
	{


	}
}
void CX2Main::MakeHackScreenShot(const char *pFileName)
{
	try
	{
		// ?? ???? ???? ???? ?????? default ???? //
		HRESULT hr = BCERR_FILE_CREATE;

		// ��??? ????????.
		if(m_BandiCaptureLibrary.IsCapturing()==FALSE)
		{
			// ???? ????? DLL ?? ?��???? ???? ?��?????? ????????.
			if(m_BandiCaptureLibrary.IsCreated()==FALSE)
			{
				// ???��? DLL?? .H?????? ?????? ???????? ???��??? ????? ????? ??????.
				if(FAILED(m_BandiCaptureLibrary.Create(BANDICAP_RELEASE_DLL_FILE_NAME)))
				{
					MakeHackScreenShotTemp(pFileName);
					return;
				}

				// ?????? ???? ?????? ??????? ?????? ???? ????? ??????.
				// ???��????? ?????? ??? ???????, ??????????? ????????? ?????? ??? ?????? ?????? ??????.
				if(FAILED(m_BandiCaptureLibrary.Verify("KOG_ELSWORD_20110825", "675a5521")))
				{
					MakeHackScreenShotTemp(pFileName);
					return;
				}
			}

			if(m_BandiCaptureLibrary.IsCreated())
			{
				//TCHAR pathName[MAX_PATH];
				//m_BandiCaptureLibrary.MakePathnameByDate(_T("ScreenCapture\\"), _T("SC_"), _T("jpg"), pathName, MAX_PATH);
				HWND pDesktop = GetDesktopWindow();
				hr = m_BandiCaptureLibrary.CaptureImage(pFileName, BCAP_IMAGE_JPG , 50, BCAP_MODE_GDI, true,(LONG_PTR)pDesktop);

				if(FAILED(hr))
				{
					MakeHackScreenShotTemp(pFileName);
					return;
				}
			}
		}
	}
	catch (...)
	{


	}
}

CKTDGUIDialogType CX2Main::KTDGUIInfoBox( D3DXVECTOR2 vPos, const WCHAR* pText, CKTDXStage* pNowState, float fLifeTime )
{
	if( pNowState == NULL )
		pNowState = m_pNowState;


	DialogLog( pText );

	if( m_pDlgInfoBox != NULL )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg( m_pDlgInfoBox );
		m_fInfoBoxLifeTime = -1.f;
		m_pDlgInfoBox = NULL;
	}

	m_pDlgInfoBox = new CKTDGUIDialog( pNowState, L"DLG_Common_Info.lua", 0.07f, XDL_POP_UP );
	m_pDlgInfoBox->SetColor( D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f) );
	m_pDlgInfoBox->SetPos( vPos );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDlgInfoBox );
	m_fInfoBoxLifeTime = fLifeTime;

	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDlgInfoBox->GetControl( L"Static_Item_Name1" );
	CKTDGUIStatic* pStaticInfo = (CKTDGUIStatic*)m_pDlgInfoBox->GetControl( L"InfoBox" );


	CKTDGUIControl::CPictureData *pPicture = pStaticInfo->GetPictureIndex(1);

	wstring tempText = pText;

	const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * 390);


	//bool checkEnterChar = false;
	int nowTextLen = 0;

	//	CKTDGFontManager::CKTDGFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFont( pStatic->GetString(0)->fontIndex );
	//CKTDGUIUniBuffer uniBuffer;
	//uniBuffer.SetFontNode( pFont );

	CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStatic->GetString(0)->fontIndex );
	int lineNum = CWordLineHandler::LineBreakInX2MainMsgBox( tempText, pFont, constTextMaxLen );
	if ( lineNum == 2 )
	{
		pStatic->GetString(0)->pos.y -= 12.f;
		pStatic->GetString(1)->pos.y -= 12.f;
	}
	else if ( lineNum == 3 )
	{
		pStatic->GetString(0)->pos.y -= pFont->GetHeight() + 3.f;
		pStatic->GetString(1)->pos.y -= pFont->GetHeight() + 3.f;
	}

	pStatic->GetString(0)->msg = tempText;
	pStatic->GetString(1)->msg = tempText;

	if( pPicture != NULL )
	{
		D3DXVECTOR2 vInfoPos = pPicture->GetPos();
// ??? ????? ????? ?????? -9 ????.
		vInfoPos.y = pStatic->GetString(0)->pos.y - 9;
		pPicture->SetPos(vInfoPos);
	}

	return m_pDlgInfoBox;
}

void CX2Main::DeleteInfoBox()
{
	if( m_pDlgInfoBox != NULL )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->DeleteDlg( m_pDlgInfoBox );
	}
	m_fInfoBoxLifeTime = -1.f;
	m_pDlgInfoBox = NULL;
}



















//{{ ????? : [2010/03/24]	// ???��??? ?? ???
struct PROCESS_INFO
{
	wstring			ProcessName;
	DWORD			ProcessID;
};
//}}


BOOL CALLBACK EnumChildProc_Thread(HWND hwnd , LPARAM lp)
{
	g_pMain->GetCheckWindowInfo()->SetChildWindow_Thread( true );
	return FALSE;
}



bool CX2Main::CheckWindowInfo::CheckProcess_Thread()
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

//  #ifndef _SERVICE_
//  	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
//  		return false;
//  #endif

	if( g_pKTDXApp != NULL && g_pKTDXApp->GetHWND() != NULL )
	{
		m_bFindChild_Thread = false;
		EnumChildWindows( g_pKTDXApp->GetHWND(), EnumChildProc_Thread, NULL );
		if( m_bFindChild_Thread == true )
		{
			// ??? ?????�� ???????.
			m_bSendedMail_Thread = true;
			if( m_bSendedMail_Thread == false )
			{
				g_pMain->SendHackMail_DamageHistory(ANTI_HACK_STRING_AntiHacking_CHILD_WINDOW);
			}
		}
	}




	static float fCheckTime = 0.f;
	DWORD ThreadId = GetWindowThreadProcessId(NULL,NULL);
	//vector<THREAD_WND_INFO> vecDeskThreadInfo = GetThreadWnd(ThreadId);

	fCheckTime += g_pKTDXApp->GetElapsedTime();
	if( fCheckTime >= 3.f * 60.f)
	{
		fCheckTime = 0.f;
		m_vecDeskThreadInfo_Thread.clear();
	}

	if( m_vecDeskThreadInfo_Thread.size() == 0 )
	{
		m_vecDeskThreadInfo_Thread = GetThreadWnd(ThreadId);

		m_vecTopWndInfo_Thread.clear();
		{
			HWND hWnd = ::GetTopWindow(NULL);
			while(hWnd)
			{
				string strWndText = " ";
				string strWndClass = " ";
				WCHAR wTextWindow[_MAX_PATH] = L"";
				WCHAR wTextClass[_MAX_PATH] = L"";
				int hr1 = GetWindowText(hWnd, wTextWindow, _MAX_PATH);
				int hr2 = GetClassName(hWnd,wTextClass,_MAX_PATH);

				THREAD_WND_INFO threadWndInfo;

				if( hr1 > 0 )
					threadWndInfo.m_wstrName = wTextWindow;
				else
					threadWndInfo.m_wstrName = L"NameEmpty";

				if( hr2 > 0 )
					threadWndInfo.m_wstrClass = wTextClass;
				else
					threadWndInfo.m_wstrClass = L"ClassEmpty";

				if( hr1 > 0 )
				{
					if( strWndText.compare( "MSCTFIME UI") != 0 && strWndText.compare( "Default IME") != 0 )
					{
						m_vecTopWndInfo_Thread.push_back(threadWndInfo);
					}
				}

				hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
			}

			if( m_vecDeskThreadInfo_Thread.size() == 0 || m_vecTopWndInfo_Thread.size() == 0 )
			{
			}
		}


		// x2 ???��????? ??? ?????�J???? ???? (formmain window?? ?????? ???)
		if( g_pKTDXApp->GetHWND() != NULL )
		{
			DWORD X2ThreadId = GetWindowThreadProcessId(g_pKTDXApp->GetHWND(),NULL);

			vector<THREAD_WND_INFO> vecThreadWndInfo = GetThreadWnd(X2ThreadId);
			for( UINT itw=0; itw<vecThreadWndInfo.size(); ++itw )
			{
				THREAD_WND_INFO twi = vecThreadWndInfo[itw];

				char *strDeFilter = NULL;
				wstring wstrtwname = L"";
				char strNewFilter1[100] = { 68, -59, -118, -85, -111, -54, 46, 72, 0, };
				strDeFilter = XORDecrypt( strNewFilter1, strlen(strNewFilter1) );
				ConvertCharToWCHAR( wstrtwname, strDeFilter );

				if( twi.m_wstrName.compare(wstrtwname) == 0 )
				{
				}
				SAFE_DELETE_ARRAY( strDeFilter );
			}
		}
	}


	HWND hWndForeGround = GetForegroundWindow();
	if( hWndForeGround != NULL && g_pKTDXApp->GetHWND() != hWndForeGround &&
		GetActiveWindow() == g_pKTDXApp->GetHWND() )
	{
		WCHAR wTextWindow[_MAX_PATH] = L"";
		int hr1 = GetWindowText(hWndForeGround, wTextWindow, _MAX_PATH);

		if( hr1 > 0 )
		{
			string strForeGroundWndName = "";
			ConvertWCHARToChar(strForeGroundWndName, wTextWindow);
		}
		else
		{
		}
	}

	if( m_bForceExitThread == true )
		return true;

	std::vector<PROCESS_INFO> vecZombieProcessInfo;
	std::vector<PROCESS_INFO> vecProcessInfo;
	PROCESS_INFO tempProcessInfo;
	DWORD aProcesses[1024], cbNeeded;
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
		return false;
	DWORD cProcesses = cbNeeded / sizeof(DWORD);

	DWORD PidFor;
	for ( PidFor = 0x0c; PidFor < 0xFFFF; PidFor +=4 )
	{
		HANDLE hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, PidFor );

		if (hProcess)
		{
			BOOL sHide = false;
			unsigned int i;
			WCHAR szName[MAX_PATH]= L"<Unknown>";
			HMODULE hModule;

			for ( i = 0; i < cProcesses; ++i )
			{
				if (PidFor == aProcesses[i])
				{
					sHide = true;
					break;
				}

			}

			if(EnumProcessModules(hProcess,&hModule,sizeof(hModule),&cbNeeded))
			{
				GetModuleFileNameEx(hProcess,hModule,szName,ARRAY_SIZE(szName));

				tempProcessInfo.ProcessID = PidFor;
				tempProcessInfo.ProcessName = szName;
				MakeUpperCase( tempProcessInfo.ProcessName );

				// ???? ???��?????? ??????? ??��?
				if(tempProcessInfo.ProcessID != GetCurrentProcessId())
				{
#ifdef CONVERSION_VS
					// Find (_1.ProcessID == tempProcessInfo.ProcessID)
					std::vector<PROCESS_INFO>::iterator it = find_if(vecProcessInfo.begin(), vecProcessInfo.end(),
						boost::bind(std::equal_to<DWORD>(), boost::bind(&PROCESS_INFO::ProcessID, _1), tempProcessInfo.ProcessID));
#else
					// Find (_1.ProcessID == tempProcessInfo.ProcessID)
					std::vector<PROCESS_INFO>::iterator it = find_if(vecProcessInfo.begin(), vecProcessInfo.end(),
						bind(std::equal_to<DWORD>(), bind(&PROCESS_INFO::ProcessID, _1), tempProcessInfo.ProcessID));
#endif // CONVERSION_VS
					if ( it == vecProcessInfo.end() )
						vecProcessInfo.push_back( tempProcessInfo );
				}
			}
			else
			{
				GetProcessImageFileName(hProcess,szName,ARRAY_SIZE(szName));
				tempProcessInfo.ProcessID = PidFor;
				tempProcessInfo.ProcessName = szName;
				MakeUpperCase( tempProcessInfo.ProcessName );

				// ???? ???��?????? ??????? ??��?
				if(tempProcessInfo.ProcessID != GetCurrentProcessId())
				{
#ifdef CONVERSION_VS
					// Find (_1.ProcessID == tempProcessInfo.ProcessID)
					std::vector<PROCESS_INFO>::iterator it = find_if(vecZombieProcessInfo.begin(), vecZombieProcessInfo.end(),
						boost::bind(std::equal_to<DWORD>(), boost::bind(&PROCESS_INFO::ProcessID, _1), tempProcessInfo.ProcessID));
#else
					// Find (_1.ProcessID == tempProcessInfo.ProcessID)
					std::vector<PROCESS_INFO>::iterator it = find_if(vecZombieProcessInfo.begin(), vecZombieProcessInfo.end(),
						bind(std::equal_to<DWORD>(), bind(&PROCESS_INFO::ProcessID, _1), tempProcessInfo.ProcessID));
#endif // CONVERSION_VS
					if ( it == vecZombieProcessInfo.end() )
						vecZombieProcessInfo.push_back( tempProcessInfo );
				}
			}

		}

		CloseHandle( hProcess );

		if( m_bForceExitThread == true )
			return true;
	}

	wstring wstrDeFilter = L"";
	char *strDeFilter = NULL;
	KHackingToolInfo khti;

	char strNewFilter1[100] = { 70, -17, -76, -106, -108, -30, 116, 20, -63, -2, 88, -35, 0, };
	strDeFilter = XORDecrypt( strNewFilter1, strlen(strNewFilter1) );
	ConvertCharToWCHAR( wstrDeFilter, strDeFilter );
	khti.m_cFlag = 1;
	khti.m_wstrProcessName = wstrDeFilter;

    m_vecHackList_Thread.push_back( khti );
	SAFE_DELETE_ARRAY( strDeFilter );

	char strNewFilter2[100] = { 76, -6, -68, -118, -112, -123, 35, 74, -125, 0, };
	strDeFilter = XORDecrypt( strNewFilter2, strlen(strNewFilter2) );
	ConvertCharToWCHAR( wstrDeFilter, strDeFilter );
	khti.m_cFlag = 7;
	khti.m_wstrProcessName = wstrDeFilter;
    m_vecHackList_Thread.push_back( khti );
	SAFE_DELETE_ARRAY( strDeFilter );

	bool bFind = false;
	ClearWindowInfo_Thread();
	EnumWindows( (WNDENUMPROC)EnumWindowsProc_Thread, (LPARAM)this );

	//// Module
	//X2AntiHackHelper::ModuleVector vecModuleName;
	//X2AntiHackHelper::EnumProcessModules(vecModuleName);

	WCHAR tempModuleName[MAX_PATH] = L"";

	if( m_pInstanceData != NULL && m_pKTDXApp != NULL )
	{
        if ( m_vecHackList_Thread.empty() == false )
		{

            for(int i=0; i< (int) m_vecHackList_Thread.size(); ++i)
			{
                KHackingToolInfo hackInfo = m_vecHackList_Thread[i];
				if( hackInfo.m_cFlag != 16 && hackInfo.m_cFlag != 25 &&
					hackInfo.m_cFlag != 26 && hackInfo.m_cFlag != 27 && hackInfo.m_cFlag != 28 && hackInfo.m_cFlag != 29 &&
					hackInfo.m_cFlag != 32 && hackInfo.m_cFlag != 33 && hackInfo.m_cFlag != 34 && hackInfo.m_cFlag != 35 )
				{
					MakeUpperCase( hackInfo.m_wstrProcessName );
					MakeUpperCase( hackInfo.m_wstrWindowClassName );
				}

				switch(hackInfo.m_cFlag)
				{
				case ANTIHACKING_ID::ANTIHACKING_FILGER_01: // Process
					{
						for(UINT j=0; j<vecProcessInfo.size(); ++j)
						{
							wstring processName = vecProcessInfo[j].ProcessName;
							MakeUpperCase(processName);

							if( StrStrW(processName.c_str(), hackInfo.m_wstrProcessName.c_str()) )
							{

								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
								{
									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
									{
										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
									}
								}

								string strProcessName;
								ConvertWCHARToChar( strProcessName, processName.c_str() );


								string strMailTitle = ANTI_HACK_STRING_AntiHacking_Process;
								strMailTitle = strMailTitle + strProcessName;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								m_pKTDXApp->SetFindHacking( true );
								return true;
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_02: // WindowText
					{
						for(UINT j=0; j<m_vecWindowInfo_Thread.size(); ++j)
						{
							KHackingToolInfo windowInfo = m_vecWindowInfo_Thread[j];

							/// kimhc // 20130826 // WINDOWS INTERNET EXPLORER?? ?????? ??? ????? ?????? ????? ??
							if( StrStrW(windowInfo.m_wstrProcessName.c_str(), hackInfo.m_wstrProcessName.c_str()) &&
								NULL == StrStrW(windowInfo.m_wstrProcessName.c_str(), L"WINDOWS INTERNET EXPLORER" ) )
							{

								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
								{
									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
									{
										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
									}
								}

								string strProcessName;
								ConvertWCHARToChar( strProcessName, windowInfo.m_wstrProcessName.c_str() );


								string strMailTitle = ANTI_HACK_STRING_AntiHacking_WindowText;
								strMailTitle = strMailTitle + strProcessName;


								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								m_pKTDXApp->SetFindHacking( true );
								return true;
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_03: // WindowClass
					for(UINT j=0; j<m_vecWindowInfo_Thread.size(); ++j)
					{
						KHackingToolInfo windowInfo = m_vecWindowInfo_Thread[j];
						if( StrStrW(windowInfo.m_wstrWindowClassName.c_str(), hackInfo.m_wstrWindowClassName.c_str()) )
						{
							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							{
								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
								{
									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
								}
							}

							string strProcessName;
							ConvertWCHARToChar( strProcessName, windowInfo.m_wstrWindowClassName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_WindowClass;
							strMailTitle = strMailTitle + strProcessName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
							m_pKTDXApp->SetFindHacking( true );
							return true;
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_04: // WindowText & WindowClass
					{
						for(UINT j=0; j<m_vecWindowInfo_Thread.size(); ++j)
						{
							KHackingToolInfo windowInfo = m_vecWindowInfo_Thread[j];
							if( StrStrW(windowInfo.m_wstrProcessName.c_str(), hackInfo.m_wstrProcessName.c_str()) &&
								StrStrW(windowInfo.m_wstrWindowClassName.c_str(), hackInfo.m_wstrWindowClassName.c_str()) )
							{
								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
								{
									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
									{
										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
									}
								}

								string strWindowName, strWindowClass;
								ConvertWCHARToChar( strWindowName, windowInfo.m_wstrProcessName.c_str() );
								ConvertWCHARToChar( strWindowClass, windowInfo.m_wstrWindowClassName.c_str() );


								string strMailTitle = ANTI_HACK_STRING_AntiHacking_WindowText_WindowClass;
								strMailTitle = strMailTitle + strWindowName + ", " + strWindowClass;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								m_pKTDXApp->SetFindHacking( true );
								return true;
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_05: // Event
					{
						for(UINT j=0; j<m_vecWindowInfo_Thread.size(); ++j)
						{
							KHackingToolInfo windowInfo = m_vecWindowInfo_Thread[j];
							if(OpenEvent( EVENT_ALL_ACCESS, FALSE, hackInfo.m_wstrProcessName.c_str() ))
							{
								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
								{
									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
									{
										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
									}
								}

								string strProcessName;
								ConvertWCHARToChar( strProcessName, hackInfo.m_wstrProcessName.c_str() );


								string strMailTitle = ANTI_HACK_STRING_AntiHacking_Event;
								strMailTitle = strMailTitle + strProcessName;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								m_pKTDXApp->SetFindHacking( true );
								return true;
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_06: // Mutex
					{
						for(UINT j=0; j<m_vecWindowInfo_Thread.size(); ++j)
						{
							KHackingToolInfo windowInfo = m_vecWindowInfo_Thread[j];
							HANDLE hMutex = CreateMutex( NULL, FALSE, hackInfo.m_wstrProcessName.c_str() );
							if(GetLastError() == ERROR_ALREADY_EXISTS)
							{
								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
								{
									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
									{
										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
									}
								}

								string strProcessName;
								ConvertWCHARToChar( strProcessName, hackInfo.m_wstrProcessName.c_str() );



								string strMailTitle = ANTI_HACK_STRING_AntiHacking_Mutex;
								strMailTitle = strMailTitle + strProcessName;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								m_pKTDXApp->SetFindHacking( true );
								return true;
							}
							else
							{
								ReleaseMutex(hMutex);
								CloseHandle(hMutex);
							}
						}
 					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_07: // Injected DLL Module in current process
					{
						//for(int index=0; index<vecModuleName.size(); ++index)
						//{
						//	if(StrStrW(vecModuleName[index].c_str(), hackInfo.m_wstrProcessName.c_str()))
						//	{
						//		if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
						//		{
						//			g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
						//			if( g_pData != NULL && g_pData->GetMyUser() != NULL )
						//				g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
						//		}

						//		string strProcessName;
						//		ConvertWCHARToChar( strProcessName, hackInfo.m_wstrProcessName.c_str() );
						//		string strMailTitle = "AntiHacking Module - ";
						//		strMailTitle = strMailTitle + strProcessName;

						//		//g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
						//		m_pKTDXApp->SetFindHacking( true );
						//		return true;
						//	}
						//}

						// ?????
						HMODULE hMod = GetModuleHandle( hackInfo.m_wstrProcessName.c_str() );
						if( hMod )
						{

							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							{
								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
								{
									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
								}
							}

							string strModuleName;
							ConvertWCHARToChar( strModuleName, hackInfo.m_wstrProcessName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_Module;
							strMailTitle = strMailTitle + strModuleName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
							m_pKTDXApp->SetFindHacking( true );
							return true;
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_08:	//	File
					{
						// ???????????? ???
						OFSTRUCT of;
						string strFileName;
						ConvertWCHARToChar( strFileName, hackInfo.m_wstrProcessName.c_str() );

						if( OpenFile(strFileName.c_str(), &of, OF_EXIST) != HFILE_ERROR )
						{
							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							{
								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
								{
									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
								}
							}


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_FileExist;
							strMailTitle = strMailTitle + strFileName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
							m_pKTDXApp->SetFindHacking( true );

							//::DeleteFile(hackInfo.m_wstrProcessName.c_str());

							return true;
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_09:	//	DLL Manager & not exit game
					{
						if( SiCX2DLLManager()->FindModule( hackInfo.m_wstrProcessName.c_str() ) == true )
						{
							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							{
								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
								{
									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
								}
							}

							string strDllName;
							ConvertWCHARToChar( strDllName, hackInfo.m_wstrProcessName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_DLL_Find;
							strMailTitle = strMailTitle + strDllName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_10:	//	DLL Manager & exit game
					{
						if( SiCX2DLLManager()->FindModule( hackInfo.m_wstrProcessName.c_str() ) == true )
						{


							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							{
								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
								{
									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
								}
							}

							string strDllName;
							ConvertWCHARToChar( strDllName, hackInfo.m_wstrProcessName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_DLL_Find_Exit;
							strMailTitle = strMailTitle + strDllName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
							m_pKTDXApp->SetFindHacking( true );
							return true;
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_11:
					{
						for(UINT j=0; j<vecZombieProcessInfo.size(); ++j)
						{
							wstring processName = vecZombieProcessInfo[j].ProcessName;
							MakeUpperCase(processName);

							if( StrStrW(processName.c_str(), hackInfo.m_wstrProcessName.c_str()) )
							{


								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
								{
									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
									{
										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
									}
								}

								string strProcessName;
								ConvertWCHARToChar( strProcessName, processName.c_str() );



								string strMailTitle = ANTI_HACK_STRING_AntiHacking_ZombiProcess;
								strMailTitle = strMailTitle + strProcessName;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								m_pKTDXApp->SetFindHacking( true );
								return true;
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_13:
					{
						if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
							g_pData->GetMyUser()->GetUserData().hackingUserType == CX2User::HUT_AGREE_HACK_USER )
						{
							// ???????????? ???
							OFSTRUCT of;
							string strFileName;
							ConvertWCHARToChar( strFileName, hackInfo.m_wstrProcessName );

							if( OpenFile(strFileName.c_str(), &of, OF_EXIST) != HFILE_ERROR )
							{
                                std::string strWindowClassName;
                                ConvertWCHARToChar( strWindowClassName, hackInfo.m_wstrWindowClassName );
                                g_pMain->SendHackMail_AttachFile(strFileName.c_str(), strWindowClassName );
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_14:
					{
						if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
							g_pData->GetMyUser()->GetUserData().hackingUserType == CX2User::HUT_AGREE_HACK_USER )
						{
							bool			bFind = false;
							HANDLE          hFind;
							WIN32_FIND_DATA dataFind;
							BOOL            bMoreFile = TRUE;
							wstring wstrMailTitle = hackInfo.m_wstrProcessName;
							wstrMailTitle = wstrMailTitle + L"\n";

							string strDirName;
							ConvertWCHARToChar( strDirName, hackInfo.m_wstrProcessName.c_str() );

							hFind = FindFirstFile(hackInfo.m_wstrProcessName.c_str(), &dataFind);
							while(hFind != INVALID_HANDLE_VALUE && bMoreFile == TRUE)
							{
								if(dataFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
								{
									if(!(wcscmp(dataFind.cFileName, L".") == 0 || wcscmp(dataFind.cFileName, L"..") == 0))
									{
										wstrMailTitle = wstrMailTitle + dataFind.cFileName;
										wstrMailTitle = wstrMailTitle + L"   [DIR]\n";
									}
								}
								else
								{
									wstrMailTitle = wstrMailTitle + dataFind.cFileName;
									wstrMailTitle = wstrMailTitle + L"   [FILE]\n";
								}

								bFind = true;
								bMoreFile = FindNextFile(hFind, &dataFind);
							}

							if( bFind == true )
							{
								string strMailTitle;
								ConvertWCHARToChar( strMailTitle, wstrMailTitle );
                                string strWindowClassName;
                                ConvertWCHARToChar( strWindowClassName, hackInfo.m_wstrWindowClassName );
                                g_pMain->SendHackMail_FileList(strMailTitle.c_str(), strWindowClassName );
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_15: // Process & Not Exit Game
					{
						for(UINT j=0; j<vecProcessInfo.size(); ++j)
						{
							wstring processName = vecProcessInfo[j].ProcessName;
							MakeUpperCase(processName);

							if( StrStrW(processName.c_str(), hackInfo.m_wstrProcessName.c_str()) )
							{
// 								if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
// 								{
// #ifdef FIX_HACKUSER_REG01
// 									if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
// 										g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
// 									{
// 										g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
// 										g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
// 									}
// #endif
// 								}

								string strProcessName;
								ConvertWCHARToChar( strProcessName, processName.c_str() );


								string strMailTitle = ANTI_HACK_STRING_AntiHacking_Process;
								strMailTitle = strMailTitle + strProcessName;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
								//m_pKTDXApp->SetFindHacking( true );
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_16:	// ??? ???? ??????
					{
						if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
							g_pInstanceData != NULL && g_pInstanceData->GetUserID() == hackInfo.m_wstrProcessName )
						{
							// ?????? ???? ???
							g_pMain->SendUserMonitoringMail(ANTI_HACK_USER_MONITORING);
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_17: // find module & not exit
					{
						// ?????
						HMODULE hMod = GetModuleHandle( hackInfo.m_wstrProcessName.c_str() );
						if( hMod )
						{
// 							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
// 							{
// #ifdef FIX_HACKUSER_REG01
// 								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
// 									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
// 								{
// 									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
// 									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
// 								}
// #endif
// 							}

							string strModuleName;
							ConvertWCHARToChar( strModuleName, hackInfo.m_wstrProcessName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_Module;
							strMailTitle = strMailTitle + strModuleName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_18:	// ??? ???? ??????
					{
						if( g_pMain->m_wstrPublicIp == hackInfo.m_wstrProcessName )
						{
							// ?????? ???? ???
							g_pMain->SendUserMonitoringMail(ANTI_HACK_IP_MONITORING);
						}
					}
					break;

				case ANTIHACKING_ID::ANTIHACKING_FILGER_19:	//	DLL Manager In & not exit game
					{
						if( SiCX2DLLManager()->FindModuleIn( hackInfo.m_wstrProcessName.c_str() ) == true )
						{
// 							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
// 							{
// #ifdef FIX_HACKUSER_REG01
// 								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
// 									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
// 								{
// 									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
// 									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
// 								}
// #endif
// 							}

							string strDllName;
							ConvertWCHARToChar( strDllName, hackInfo.m_wstrProcessName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_DLL_Find;
							strMailTitle = strMailTitle + strDllName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_20: // WindowText & not Exit
					{
						for(UINT j=0; j<m_vecWindowInfo_Thread.size(); ++j)
						{
							KHackingToolInfo windowInfo = m_vecWindowInfo_Thread[j];
							if( StrStrW(windowInfo.m_wstrProcessName.c_str(), hackInfo.m_wstrProcessName.c_str()) )
							{
								string strProcessName;
								ConvertWCHARToChar( strProcessName, windowInfo.m_wstrProcessName.c_str() );


								string strMailTitle = ANTI_HACK_STRING_AntiHacking_WindowText;
								strMailTitle = strMailTitle + strProcessName;

								g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_21: // ???? ??? ????
					{
						HMODULE hMod = GetModuleHandle( hackInfo.m_wstrProcessName.c_str() );
						if( hMod )
						{
							FreeLibrary(hMod);
						}
					}
					break;

				case ANTIHACKING_ID::ANTIHACKING_FILGER_22: // dll manager?? ???? ??? ????
					{
						SiCX2DLLManager()->ForceFreeModule( hackInfo.m_wstrProcessName );
					}
					break;


				case ANTIHACKING_ID::ANTIHACKING_FILGER_23: // machine id?? ??????
					{
						if( g_pInstanceData != NULL && g_pInstanceData->GetMachineId().empty() == false )
						{
							wstring wstrMachineId = L"";
							ConvertUtf8ToWCHAR( wstrMachineId, g_pInstanceData->GetMachineId().c_str() );
							MakeUpperCase(wstrMachineId);
							if( hackInfo.m_wstrProcessName == wstrMachineId )
							{
								// ?????? ???? ???
								g_pMain->SendUserMonitoringMail(ANTI_HACK_MACHINE_MONITORING);
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_24: // findwindow
					{
						if( FindWindow(NULL, hackInfo.m_wstrProcessName.c_str()) != NULL )
						{

							if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							{
								if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
									g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
								{
									g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
									g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
								}
							}

							string strProcessName;
							ConvertWCHARToChar( strProcessName, hackInfo.m_wstrProcessName.c_str() );


							string strMailTitle = ANTI_HACK_STRING_AntiHacking_FindWindow;
							strMailTitle = strMailTitle + strProcessName;

							g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
							m_pKTDXApp->SetFindHacking( true );
							return true;
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_26:	// ?????? ?????? ???? ??? (not exit)
				case ANTIHACKING_ID::ANTIHACKING_FILGER_27:	// exit
					{
						for( UINT iThreadWindow = 0; iThreadWindow<m_vecDeskThreadInfo_Thread.size(); ++iThreadWindow )
						{
							if( hackInfo.m_wstrProcessName.empty() == false && m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrName.empty() == false )
							{
								if( m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrName.compare( hackInfo.m_wstrProcessName.c_str()) == 0
									&& ( hackInfo.m_wstrWindowClassName.empty() ||	/// ??????? ????? ????????? ?????
										/// ??? ??��?? ????? ????????? ????? ??
									   ( m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrClass.compare( hackInfo.m_wstrWindowClassName.c_str() ) == 0 ) )
								  )
								{
									string strProcessName;
									ConvertWCHARToChar( strProcessName, m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrName.c_str() );


									string strMailTitle = ANTI_HACK_STRING_AntiHacking_TWindowText;
									strMailTitle = strMailTitle + strProcessName;

									if ( !hackInfo.m_wstrWindowClassName.empty() )
									{
										string strClassName;
										ConvertWCHARToChar( strClassName, m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrClass.c_str() );

										strMailTitle += strClassName;
									}

									g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());

									if( hackInfo.m_cFlag == ANTIHACKING_ID::ANTIHACKING_FILGER_27 )
									{
										m_pKTDXApp->SetFindHacking( true );
										return true;
									}
									break;
								}
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_28:	// ?????? ?????? ????? ??? (not exit)
				case ANTIHACKING_ID::ANTIHACKING_FILGER_29:	// exit
					{
						for( UINT iThreadWindow = 0; iThreadWindow<m_vecDeskThreadInfo_Thread.size(); ++iThreadWindow )
						{
							if( hackInfo.m_wstrWindowClassName.empty() == false && m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrClass.empty() == false )
							{
								if( m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrClass.compare(hackInfo.m_wstrWindowClassName.c_str()) == 0 )
								{
									string strProcessName;
									ConvertWCHARToChar( strProcessName, m_vecDeskThreadInfo_Thread[iThreadWindow].m_wstrClass.c_str() );


									string strMailTitle = ANTI_HACK_STRING_AntiHacking_TWindowClass;
									strMailTitle = strMailTitle + strProcessName;

									g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());

									if( hackInfo.m_cFlag == ANTIHACKING_ID::ANTIHACKING_FILGER_29 )
									{
										m_pKTDXApp->SetFindHacking( true );
										return true;
									}
									break;
								}
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_32:	// ?????? ?????? ???? ??? (not exit)
				case ANTIHACKING_ID::ANTIHACKING_FILGER_33:	// exit
					{
						for( UINT iThreadWindow = 0; iThreadWindow<m_vecTopWndInfo_Thread.size(); ++iThreadWindow )
						{
							if( hackInfo.m_wstrProcessName.empty() == false && m_vecTopWndInfo_Thread[iThreadWindow].m_wstrName.empty() == false )
							{
								if( m_vecTopWndInfo_Thread[iThreadWindow].m_wstrName.compare( hackInfo.m_wstrProcessName.c_str()) == 0 )
								{
									string strProcessName;
									ConvertWCHARToChar( strProcessName, m_vecTopWndInfo_Thread[iThreadWindow].m_wstrName.c_str() );


									string strMailTitle = ANTI_HACK_STRING_AntiHacking_TWindowText;
									strMailTitle = strMailTitle + strProcessName;

									g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());

									if( hackInfo.m_cFlag == ANTIHACKING_ID::ANTIHACKING_FILGER_33 )
									{
										m_pKTDXApp->SetFindHacking( true );
										return true;
									}
									break;
								}
							}
						}
					}
					break;
				case ANTIHACKING_ID::ANTIHACKING_FILGER_34:	// ?????? ?????? ????? ??? (not exit)
				case ANTIHACKING_ID::ANTIHACKING_FILGER_35:	// exit
					{
						for( UINT iThreadWindow = 0; iThreadWindow<m_vecTopWndInfo_Thread.size(); ++iThreadWindow )
						{
							if( hackInfo.m_wstrWindowClassName.empty() == false && m_vecTopWndInfo_Thread[iThreadWindow].m_wstrClass.empty() == false )
							{
								if( m_vecTopWndInfo_Thread[iThreadWindow].m_wstrClass.compare(hackInfo.m_wstrWindowClassName.c_str()) == 0 )
								{
									string strProcessName;
									ConvertWCHARToChar( strProcessName, m_vecTopWndInfo_Thread[iThreadWindow].m_wstrClass.c_str() );


									string strMailTitle = ANTI_HACK_STRING_AntiHacking_TWindowClass;
									strMailTitle = strMailTitle + strProcessName;

									g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());

									if( hackInfo.m_cFlag == ANTIHACKING_ID::ANTIHACKING_FILGER_35 )
									{
										m_pKTDXApp->SetFindHacking( true );
										return true;
									}
									break;
								}
							}
						}
					}
					break;

				default:
					{
					}
					break;
				}

				if( m_bForceExitThread == true )
					return true;
			}
		}
		else
		{
			// ?????? ??? ???????? ?????? ????!
			// g_pMain->SendHackMail_DamageHistory("-- EmptyList --");
			g_pMain->SendHackMail_DamageHistory("-- EmptyList --");
			return false;
		}
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
	return false;
}

//////////////////////////////////////////////////////////////////////////
void CX2Main::TimedMessagePopUp::OnFrameMove( double fTime, float fElapsedTime )
{
	if( NULL == m_pDialog )
	{
		// ?????��?? ?????? ?��? 0???? ????
		m_fTimeLeft = 0.f;
		return;
	}

	if( 0.f == m_fTimeLeft )
		return;


	float fOldTimeLeft = m_fTimeLeft;
	m_fTimeLeft -= fElapsedTime;
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDialog ) )
	{
		if( m_fTimeLeft < 0.f )
		{
			m_pDialog->SetModal(false);
			m_pDialog->SetShowEnable( false, false );
			if( NULL != m_pDialog->GetUIDialogManager() )
				m_pDialog->GetUIDialogManager()->CountModalDialog();

			m_fTimeLeft = 0.f;

			//if( NULL != CBOnTimeOut )
			//{
			//	CBOnTimeOut( m_pUserData );
			//}
		}
		else
		{
			float fTimeLeftInt = (float) (int) fOldTimeLeft;
			if( fOldTimeLeft > fTimeLeftInt && fTimeLeftInt >= m_fTimeLeft )
			{
				CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDialog->GetControl( L"StaticMsgBoxBG" );

				ASSERT( NULL != pStatic );
				if( NULL == pStatic )
				{
					WCHAR wszErrorMsg[1024] = L"";
					StringCchPrintfW( wszErrorMsg, ARRAY_SIZE( wszErrorMsg ), L"timed popup error, %d, %s, %s", (int)m_eMessageType, m_pDialog->GetName(), m_wstrMsg.c_str() );
					StateLog( wszErrorMsg );
					m_fTimeLeft = 0.f;

					// m_pDialog?? ???? ?????? ???? ?? dialog?? ????? ???????? ??? NULL?? ????.
					m_pDialog = NULL;
				}

				if( NULL != pStatic )
				{
					WCHAR wszText[256] = L"";
					//????? ???? ???
					switch(m_eMessageType)
					{
						case MT_NONE :		/// ??? ??????? ??????? ????, ???? ?��??? ??????? ????
							{
								StringCchPrintfW( wszText, ARRAY_SIZE(wszText), m_wstrMsg.c_str() );
							}
							break;
						case MT_AGREE_COUPLE :
							{

								if ( NULL != g_pData->GetRelationshipManager() &&
									NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
								{

									wstring wstrText = GET_REPLACED_STRING( ( STR_ID_24447, "Li", m_wstrMsg, static_cast<int>( m_fTimeLeft ) ) );	/// %d?? ?? ??? ???
									StringCchPrintfW( wszText, ARRAY_SIZE(wszText), wstrText.c_str() );
								}
							} break;
						case MT_PARTY_READY_TO_PLAY_DUNGEON_REQUEST:
						case MT_DUNGEON_MATCH_START_REQUEST:
						case MT_DUNGEON_MATCH_REMAKING:
						case MT_WAIT_CHOICE_COUPLE:
							{

								wstring wstrText = GET_REPLACED_STRING( ( STR_ID_24447, "Li", m_wstrMsg, static_cast<int>( m_fTimeLeft ) ) );	/// %d?? ?? ??? ???
								StringCchPrintfW( wszText, ARRAY_SIZE(wszText), wstrText.c_str() );

							} break;
						case MT_ENTER_GATE_OF_DARKNESS:
							{
								wstring wstrText = GET_REPLACED_STRING( ( STR_ID_24446, "Li", m_wstrMsg, static_cast<int>( m_fTimeLeft ) ) );	/// %d?? ?? ??? ????
								StringCchPrintfW( wszText, ARRAY_SIZE(wszText), wstrText.c_str() );
							} break;

						case MT_WAIT_SUMMON_MARRIAGED_USER:
						case MT_SELECT_SUMMON_TO_MARRIAGED_USER:
							{
								wstring wstrText = GET_REPLACED_STRING( ( STR_ID_24520, "Li", m_wstrMsg, static_cast<int>( m_fTimeLeft ) ) );	/// ???? ?��? : %d??
								StringCchPrintfW( wszText, ARRAY_SIZE(wszText), wstrText.c_str() );

								if ( NULL != g_pMain )		/// ???? ???? ?? ???? ???? ???? ??????, ??? ???? ???
								{
									CX2State* pNowState = static_cast<CX2State*>( g_pMain->GetNowState() );

									if ( NULL != pNowState && true == pNowState->GetWaitGameStart() )
										m_fTimeLeft = 0.f;
								}
							} break;
						default:
							{
								StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"%s (%d%s)", m_wstrMsg.c_str(), static_cast<int>( m_fTimeLeft ), GET_STRING( STR_ID_180 ) );	/// ??
							} break;
					}
					wstring tempText = wszText;

					const int constTextMaxLen = (int)(g_pKTDXApp->GetResolutionScaleX() * 366);
					//bool checkEnterChar = false;
					int nowTextLen = 0;
					int lineNum = 1;

					CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStatic->GetString(0)->fontIndex );
					if( NULL != pFont )
					{
						lineNum = CWordLineHandler::LineBreakInX2MainMsgBox( tempText, pFont, constTextMaxLen );
					}
					pStatic->GetString(0)->msg = tempText;
				}
			}
		}
	}
	else
	{
		m_fTimeLeft = 0.f;
	}
}


//{{ 2011.9.16	?????  ???? ?????? ��?? ????
bool CX2Main::Bandi_StartVideoCapture()
{
	if( m_fCaptureDelay > 0.0f )
	{
		if( g_pChatBox != NULL )
		{
			g_pChatBox->AddChatLog( GET_STRING( STR_ID_17538 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		}
		return false;
	}

	m_fCaptureDelay = 3.0f;

	IDirect3DDevice9* pd3d9Device = DXUTGetD3DDevice();

	// ��??? ????????.
	if(FALSE == m_BandiCaptureLibrary.IsCapturing())
	{
		// ???? ????? DLL ?? ?��???? ???? ?��?????? ????????.
		if(FALSE ==m_BandiCaptureLibrary.IsCreated())
		{
			// ???��? DLL?? .H?????? ?????? ???????? ???��??? ????? ????? ??????.
			if(FAILED(m_BandiCaptureLibrary.Create(BANDICAP_RELEASE_DLL_FILE_NAME)))
				return false;

			// ?????? ???? ?????? ??????? ?????? ???? ????? ??????.
			// ???��????? ?????? ??? ???????, ??????????? ????????? ?????? ??? ?????? ?????? ??????.
			if(FAILED(m_BandiCaptureLibrary.Verify("KOG_ELSWORD_20110825", "675a5521")))
				return false;
		}

		if(m_BandiCaptureLibrary.IsCreated())//3.?????? ?????
		{
			BCAP_CONFIG cfg;

			if(GetGameOption().GetUserSetting()==false)
			{
				int iVal=GetGameOption().GetGeneralVideoType();
				if(iVal>14)
					iVal+=4;//??? ??? 4?????? ????? zoom?? ??????

				if(BCAP_PRESET_VIDEO_EDITING!=21)
				{
					ASSERT(false);//?????????? ???? ??? ????? ????�� ?????? ?????
				}
				BCapConfigPreset(&cfg, (BCAP_PRESET)iVal);
			}
			else
			{
				//BCAP_PRESET bp;
				BCapConfigPreset(&cfg, (BCAP_PRESET)0);
				cfg.VideoSizeH=GetGameOption().GetVideoSizeH();
				cfg.VideoSizeW=GetGameOption().GetVideoSizeW();
				cfg.VideoFPS=GetGameOption().GetVideoFPS();
				cfg.VideoQuality=GetGameOption().GetVideoQuality();
				cfg.AudioChannels=GetGameOption().GetAudioChannels();
				cfg.AudioSampleRate=GetGameOption().GetAudioSampleRate();
				cfg.VideoCodec=GetGameOption().GetVideoFormat();
			}
			m_BandiCaptureLibrary.CheckConfig(&cfg);       // ????? ???? ??? ???
			m_BandiCaptureLibrary.SetConfig(&cfg);         // ???? ????

			m_BandiCaptureLibrary.SetMinMaxFPS(30, 60);    // ???, ??? ?????? ????

			// ????��??? ?????? ??????? ??????.
			TCHAR pathName[MAX_PATH];
			m_BandiCaptureLibrary.MakePathnameByDate(_T("ScreenCapture\\"), _T("SC_"), _T("avi"), pathName, MAX_PATH);

			// ��??? ????????.
			HRESULT hr = m_BandiCaptureLibrary.Start(pathName, NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)pd3d9Device);
			if(FAILED(hr))
			{
				switch( hr )
				{
				case BCERR_VIDEO_CAPTURE:
					break;
				case BCERR_AUDIO_CAPTURE:
					{
						m_BandiCaptureLibrary.GetConfig(&cfg);		   // ???? ?????? ???????
						cfg.AudioCodec = WAVETAG_NULL;				   // ???? ????
						m_BandiCaptureLibrary.CheckConfig(&cfg);       // ????? ???? ??? ???
						m_BandiCaptureLibrary.SetConfig(&cfg);         // ???? ????

						//???? ???? ?? ?????? ??? ????
						HRESULT hr = m_BandiCaptureLibrary.Start(pathName, NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)pd3d9Device);
						if( FAILED(hr) )
						{
							break;
						}
						if( g_pChatBox != NULL )
						{
							g_pChatBox->AddChatLog( GET_STRING( STR_ID_17528 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
							g_pChatBox->AddChatLog( GET_STRING( STR_ID_17529 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
						}
						return true;
					}
					break;
				}
				return false;
			}
			else
			{
				if ( g_pChatBox != NULL )
				{
					g_pChatBox->AddChatLog( GET_STRING( STR_ID_14371 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
				}
			}

		}
	}

	return true;
}

void CX2Main::Bandi_Pause_Restart_VidieCapture(bool bPause)
{
	// ??? ????, ???? ?????? ?????? ???? ?��? ???? (IsCapturing = true ????)
	if( TRUE == m_BandiCaptureLibrary.IsCapturing() )
	{
		if( true == bPause && NULL != g_pChatBox )
			g_pChatBox->AddChatLog( GET_STRING( STR_ID_14372 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		else
			g_pChatBox->AddChatLog( GET_STRING( STR_ID_14373 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

		m_BandiCaptureLibrary.Pause(bPause);
	}
}

void CX2Main::Bandi_EndVidioCapture()
{
	if( TRUE == m_BandiCaptureLibrary.IsCapturing() )
	{
		m_BandiCaptureLibrary.Stop();
		if( NULL != g_pChatBox )
			g_pChatBox->AddChatLog( GET_STRING( STR_ID_14374 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		m_iOldCaptureFileSize = -1;
	}
}

void CX2Main::Bandi_EnoughDiskSpace(float fElapsedTime)
{
	//?????? ��?? ???? ???��? ??? ??? ???�� ????��???? work????? ?????
	//????? ?�X ?????? ??? ?? ?? ???? ??????, ??��? ????? ?�X ?? ???.

	//???? ??? ???????? ????? ?????? ????.
	if( IsCapturing() == false )
		return;

	//?? ?????? ???
	m_fFileSizeCheckTime -= fElapsedTime;
	if( m_fFileSizeCheckTime < 0 )
	{
		m_fFileSizeCheckTime = 2.f;
		if( m_iOldCaptureFileSize != m_BandiCaptureLibrary.GetCaptureFileSize())
		{
			m_iOldCaptureFileSize = m_BandiCaptureLibrary.GetCaptureFileSize();
		}
		else
		{	//2?????? ???????????? ????, ???? ???????????? ??? ?????,
			//????�X ???????? ??????? ????? ???.
			g_pMain->Bandi_EndVidioCapture();
			g_pChatBox->AddChatLog( GET_STRING(STR_ID_17540), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			KOGGamePerformanceCheck::GetInstance()->Resume();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

            if ( g_pData->GetGameUDP() != NULL )
                g_pData->GetGameUDP()->RemoveAllPendingPingSends();

		}
	}
	return;
}

//}}

//{{ 2011.9.16	?????  ???? ??????? ��?? ????
bool CX2Main::Bandi_ScreenCapture()
{
	if( m_fCaptureDelay > 0.0f )
		return false;

	m_fCaptureDelay = 3.0f;

	// ?? ???? ???? ???? ?????? default ???? //
	HRESULT hr = BCERR_FILE_CREATE;

	IDirect3DDevice9* pd3d9Device = DXUTGetD3DDevice();

	if( NULL == pd3d9Device )
		return false;

	// ��??? ????????.
	// ???? ????? DLL ?? ?��???? ???? ?��?????? ????????.
	if(m_BandiCaptureLibrary.IsCreated()==FALSE)
	{
		// ???��? DLL?? .H?????? ?????? ???????? ???��??? ????? ????? ??????.
		if(FAILED(m_BandiCaptureLibrary.Create(BANDICAP_RELEASE_DLL_FILE_NAME)))
			return false;

		// ?????? ???? ?????? ??????? ?????? ???? ????? ??????.
		// ???��????? ?????? ??? ???????, ??????????? ????????? ?????? ??? ?????? ?????? ??????.
		if(FAILED(m_BandiCaptureLibrary.Verify("KOG_ELSWORD_20110825", "675a5521")))
			return false;

	}

	if(m_BandiCaptureLibrary.IsCreated())
	{
		TCHAR pathName[MAX_PATH];
		m_BandiCaptureLibrary.MakePathnameByDate(_T("ScreenCapture\\"), _T("SC_"), _T("jpg"), pathName, MAX_PATH);

		hr = m_BandiCaptureLibrary.CaptureImage(pathName, BCAP_IMAGE_JPG , 80, BCAP_MODE_D3D9, true,(LONG_PTR)pd3d9Device);

		if(FAILED(hr))
			return false;
	}


	return true;
}
//}}
KServerGroupInfo CX2Main::GetPickedChannelServer() const
{
	map<int, KServerGroupInfo>::iterator iter = g_pInstanceData->m_mapServerGroupInfo.find(m_iPickedChannelServerIPIndex);
	if(iter == g_pInstanceData->m_mapServerGroupInfo.end())
		return m_DefaultChannelServerInfo;

	return iter->second;
}



bool CX2Main::GetWallPaperDirectory( char * pszDir, int iDirSize )
{
	LPITEMIDLIST  psttList;

	if( SHGetSpecialFolderLocation( NULL, CSIDL_DESKTOP, &psttList ) == S_OK )
	{
		LPMALLOC  psttMalloc;

		SHGetPathFromIDListA( psttList, pszDir );

		SHGetMalloc( &psttMalloc );
		psttMalloc->Free( psttList );
		psttMalloc->Release();

		return true;
	}

	return false;
}


void CX2Main::CreatePlayTimeInformation()						//??? ?????��? ???
{
	if( NULL != m_pDlgPlayTimeInformation )
		DeletePlayTimeInformation();

	m_bProcessPlayTimeInformation = false;
	m_fPlayTimeInfoShowTime		  = 8.f;

	m_pDlgPlayTimeInformation = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_POPUP_SHUTDOWN.lua" );

	if( NULL == m_pDlgPlayTimeInformation )
		return;

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDlgPlayTimeInformation );

	//???? ??? @1?��??? ?????????.\n?????? ???? ????? ???????? ??? ?????\n?????? ?? ?? ??????.\n??? ?? ??? ???????��? ???????????.
	CKTDGUIStatic* pStatic = ( CKTDGUIStatic*)m_pDlgPlayTimeInformation ->GetControl( L"SHUTDOWN_Text" );
	pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_17723, "i", m_iGameHour ) );
}

void CX2Main::CreateShutDownInformation()						//???? ?????��? ???
{
	if( NULL != m_pDlgShutDownInformation )
		DeleteShutDownInformation();

	m_bProcessShutDownInformation = false;
	m_fShutDownInfoShowTime		  = 8.f;

	m_pDlgShutDownInformation = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_POPUP_SHUTDOWN.lua" );

	if( NULL == m_pDlgShutDownInformation )
		return;

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDlgShutDownInformation );

	//??????? ?? 16?? ??????? ??��?? ?? 12????? ???? 6????? ?????? ?????? ?? ???????. ??�� ?????? ????��???.
	//??????? ?? 18?? ????? ?????? ?????????? @1?? ????? ??? ???? ?????? ?????? ?? ???????. ?????? ???????? ???? ??? ???? ?��?��? ???? ?��??? ??????? ?? ??????.
	//@1?? ?? ????????? ???????.
	CKTDGUIStatic* pStatic = ( CKTDGUIStatic*)m_pDlgShutDownInformation ->GetControl( L"SHUTDOWN_Text" );
	pStatic->GetString(0)->msg = m_wstrShutDownString;
}

void CX2Main::FadePlayTimeInformation( float fShowTime )		//??? ?????��? Fade ???
{
	if( NULL != m_pDlgPlayTimeInformation )
	{
		if( 7.5f < fShowTime )		//Fade In
			m_pDlgPlayTimeInformation->Move( m_pDlgPlayTimeInformation->GetPos(), D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ), 0.5f, true );
		else if( 0.5f > fShowTime )	//Fade Out
			m_pDlgPlayTimeInformation->Move( m_pDlgPlayTimeInformation->GetPos(), D3DXCOLOR( 1.f, 1.f, 1.f, 0.f ), 0.5f, true );
	}
}

void CX2Main::DeletePlayTimeInformation()						//??? ?????��? ????
{
	if( NULL == m_pDlgPlayTimeInformation )
		return;

	SAFE_DELETE_DIALOG( m_pDlgPlayTimeInformation );
	m_pDlgPlayTimeInformation = NULL;
}

void CX2Main::FadeShutDownInformation( float fShowTime )		//???? ?????��? Fade ???
{
	if( NULL != m_pDlgShutDownInformation )
	{
		if( 7.5f < fShowTime )		//Fade In
			m_pDlgShutDownInformation->Move( m_pDlgShutDownInformation->GetPos(), D3DXCOLOR( 1.f, 1.f, 1.f, 1.f ), 0.5f, true );
		else if( 0.5f > fShowTime )	//Fade Out
			m_pDlgShutDownInformation->Move( m_pDlgShutDownInformation->GetPos(), D3DXCOLOR( 1.f, 1.f, 1.f, 0.f ), 0.5f, true );
	}
}

void CX2Main::DeleteShutDownInformation()						//???? ?????��? ????
{
	if( NULL == m_pDlgShutDownInformation )
		return;

	SAFE_DELETE_DIALOG( m_pDlgShutDownInformation );
	m_pDlgShutDownInformation = NULL;
}

/** @function : ChangeCashSkillPointDate
	@brief : ????? ?? ???? ??? ????
	@param : ????? ??, ??, ??, ?��?, ?? ????
*/
void CX2Main::ChangeCashSkillPointDate( const int iYear_, const int iMonth_, const int iDay_, const int iHour_, const int iMinute_ )
{
	g_pData->GetSkillTree()->Hander_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ( iYear_, iMonth_, iDay_, iHour_, iMinute_ );
}
/** @function : SendCheckMail
	@brief : ????? ??? ?��??? ??????????? ????? ???? ?��?? ???? ???? ???
	@param : ??? ??? ???????, ????, ????
*/
void CX2Main::SendCheckMail( const char* pRecipient_, const char* pSubject_, const char* pMessageBody_ )
{
	if ( NULL == g_pX2SMTPMail )
		g_pX2SMTPMail = new CX2SMTPMail;

	if(g_pX2SMTPMail->SMTPGetLastError() != CX2SMTPMail::CSMTP_NO_ERROR)
	{
		// Unable to initialize winsock2
		return;
	}

	g_pX2SMTPMail->SMTPSetServer( m_strMailAddress.c_str(), 25 );
	g_pX2SMTPMail->SMTPSetLogin("escrash");            // ???? ????
	g_pX2SMTPMail->SMTPSetPassword("@Els.123");
	g_pX2SMTPMail->SMTPSetSenderName( "Check Mail" );
	g_pX2SMTPMail->SMTPSetSenderMail("itexpertkim@kog.co.kr");
	g_pX2SMTPMail->SMTPSetReplyTo("itexpertkim@kog.co.kr"); // ??? ???

	string strSubject = "[ES_KR_CLIENT_CHECK_MAIL]";
	strSubject += pSubject_;
	g_pX2SMTPMail->SMTPSetSubject( strSubject.c_str() );
	g_pX2SMTPMail->SMTPAddRecipient( pRecipient_ );
	g_pX2SMTPMail->SMTPSetXPriority(CX2SMTPMail::XPRIORITY_NORMAL);
	g_pX2SMTPMail->SMTPSetXMailer("The Bat! (v3.02) Professional");
	g_pX2SMTPMail->SMTPSetMessageBody( pMessageBody_ );

	if( g_pX2SMTPMail->SMTPSend() )
	{
		//The mail was send successfully
	}
	else
	{

		//Unable to send the mail
	}

	g_pX2SMTPMail->SMTPClearRecipient();
}





#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
void CX2Main::OccurLag_LUA( float fLagTime, int iLagType )
{
#if X2_DEV_TOOLS
	if( KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->m_pfnGetGameSpace )
	{
		int iGameSpace = KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->m_pfnGetGameSpace();

		if( KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->m_pfnIsValidGameSpace_Lag( iGameSpace ) )
		{
			KOGGamePerformanceCheck::GetInstance()->GetLagCheckMgr()->SetLagTestActive( true );
			KOGGamePerformanceCheck::GetInstance()->GetLagCheckMgr()->SetLagTestTime( fLagTime );
			KOGGamePerformanceCheck::GetInstance()->GetLagCheckMgr()->SetLagTestType( iLagType );
		}
	}
#endif //_SERVICE_
}

void CX2Main::_InitKOGGamePerformanceCheck()
{
	//??? ??????, ?? ???? ????
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetGameSpace( KOGGamePerformanceCheckPort_Function::GetGameSpace );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsGameSpaceVillage( KOGGamePerformanceCheckPort_Function::IsGameSpaceVillage );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsGameSpaceField( KOGGamePerformanceCheckPort_Function::IsGameSpaceField );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsGameSpaceDungeon( KOGGamePerformanceCheckPort_Function::IsGameSpaceDungeon );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsGameSpacePvP( KOGGamePerformanceCheckPort_Function::IsGameSpacePvP );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetVillagePartyType( KOGGamePerformanceCheckPort_Function::GetVillagePartyType );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetVillagePartyUserNum( KOGGamePerformanceCheckPort_Function::GetVillagePartyUserNum );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetVillageMapID( KOGGamePerformanceCheckPort_Function::GetVillageMapID );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetVillageUserNum( KOGGamePerformanceCheckPort_Function::GetVillageUserNum );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetFieldPartyType( KOGGamePerformanceCheckPort_Function::GetFieldPartyType );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetFieldPartyUserNum( KOGGamePerformanceCheckPort_Function::GetFieldPartyUserNum );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetFieldMapID( KOGGamePerformanceCheckPort_Function::GetFieldMapID );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetFieldUserNum( KOGGamePerformanceCheckPort_Function::GetFieldUserNum );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetDungeonPartyType( KOGGamePerformanceCheckPort_Function::GetDungeonPartyType );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetDungeonPartyUserNum( KOGGamePerformanceCheckPort_Function::GetDungeonPartyUserNum );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetDungeonMapID( KOGGamePerformanceCheckPort_Function::GetDungeonMapID );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetDungeonDifficulty( KOGGamePerformanceCheckPort_Function::GetDungeonDifficulty );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetDungeonStage( KOGGamePerformanceCheckPort_Function::GetDungeonStage );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsDungeonEscape( KOGGamePerformanceCheckPort_Function::IsDungeonEscape );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetDungeonBlock( KOGGamePerformanceCheckPort_Function::GetDungeonBlock );

	//KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPPartyType( KOGGamePerformanceCheckPort_Function::GetPvPPartyType );
	//KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPPartyUserNum( KOGGamePerformanceCheckPort_Function::GetPvPPartyUserNum );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPMapID( KOGGamePerformanceCheckPort_Function::GetPvPMapID );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPUserNum( KOGGamePerformanceCheckPort_Function::GetPvPUserNum );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPRuleType( KOGGamePerformanceCheckPort_Function::GetPvPRuleType );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsPvPEscape( KOGGamePerformanceCheckPort_Function::IsPvPEscape );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPRound( KOGGamePerformanceCheckPort_Function::GetPvPRound );
	//KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetPvPObserverNum( KOGGamePerformanceCheckPort_Function::GetPvPObserverNum );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_GetFramePerSecond( KOGGamePerformanceCheckPort_Function::GetFramePerSecond );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_ResetFramePerSecond( KOGGamePerformanceCheckPort_Function::ResetFramePerSecond );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsGameHost( KOGGamePerformanceCheckPort_Function::IsGameHost );

	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_SendServer_Village( KOGGamePerformanceCheckPort_Function::SendServer_Village );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_SendServer_Field( KOGGamePerformanceCheckPort_Function::SendServer_Field );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_SendServer_Dungeon( KOGGamePerformanceCheckPort_Function::SendServer_Dungeon );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_SendServer_PvP( KOGGamePerformanceCheckPort_Function::SendServer_PvP );

	//??? ?????? ??
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsValidGameSpace_AvgFps( KOGGamePerformanceCheckPort_Function::IsValidGameSpace_AvgFps );

	//?? ???? ??
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsUnKnownLagType_Lag( KOGGamePerformanceCheckPort_Function::IsUnKnownLagType_Lag );
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsValidGameSpace_Lag( KOGGamePerformanceCheckPort_Function::IsValidGameSpace_Lag );

	//ping ???? ??
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsValidGameSpace_Ping( KOGGamePerformanceCheckPort_Function::IsValidGameSpace_Ping );

	//UDP ???????? ??
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsValidGameSpace_UDPTrans( KOGGamePerformanceCheckPort_Function::IsValidGameSpace_UDPTrans );

	//?????? ??? NPC ????? ???? ??
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsValidGameSpace_UDPTrans( KOGGamePerformanceCheckPort_Function::IsValidGameSpace_NonHostNpcReaction );

	//memory usage ???? ??
	KOGGamePerformanceCheck_CallbackFuncTable::GetInstance()->RegisterCallbackFunc_IsValidGameSpace_MemUsage( KOGGamePerformanceCheckPort_Function::IsValidGameSpace_MemUsage );

	//
	KOGGamePerformanceCheck::GetInstance()->Init( 6.0f, 5, 6000, 10, 6.0f, 200, eLagCheckType_Max );
}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
void CX2Main::ForceHost_LUA( bool bEnable )
{
#if X2_DEV_TOOLS
	if ( NULL != g_pX2Room )
		g_pX2Room->ForceHostForGameEdit( bEnable );
#endif //_SERVICE_
}

void CX2Main::SwitchConnect_LUA( int iMode )
{
#if X2_DEV_TOOLS
	if( GetX2Data()->GetGameUDP() )
		GetX2Data()->GetGameUDP()->SetForceConnectMode( CKTDNUDP::EForceConnectMode( iMode ) );
#endif //_SERVICE_
}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK


#ifdef UDP_PACKET_ANALYSIS_LOG
void CX2Main::UdpPacketAnalysisLog_LUA( bool bEnable )
{
#if X2_DEV_TOOLS
	GetX2Data()->GetGameUDP()->SetEnablePacketAnalysisLog( bEnable );
#endif //_SERVICE_
}
#endif //UDP_PACKET_ANALYSIS_LOG

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
void CX2Main::UdpPacketOverlapTest_LUA( bool bEnable )
{
#if X2_DEV_TOOLS
	m_bUdpPacketOverlap = bEnable;
#endif //_SERVICE_
}
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK





void    CX2Main::SetUDPMode_LUA( const char* pszGameType, const char* pszUDPMode )
{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    if ( false == ( pszGameType != NULL && pszGameType[0] != NULL
        && pszUDPMode != NULL && pszUDPMode[0] != NULL ) )
        return;

    const char* apszGameType[] =
    {
        "GT_NONE",
        "GT_PVP",
        "GT_DUNGEON",
        "GT_BATTLE_FIELD",
    };
    const char* apszUDPMode[] =
    {
        "UDP_MODE_DEFAULT",
        "UDP_MODE_P2P",
        "UDP_MODE_RELAY",
    };
    BOOST_STATIC_ASSERT( ARRAY_SIZE( apszGameType ) == CX2Game::GT_NUMS );
    BOOST_STATIC_ASSERT( ARRAY_SIZE( apszUDPMode ) == CKTDNUDP::FORCE_CONNECT_MODE_NUMS );

    CX2Game::GAME_TYPE          eGameType = CX2Game::GT_NONE;
    CKTDNUDP::EForceConnectMode eUDPMode = CKTDNUDP::FORCE_CONNECT_MODE_DEFAULT;

    for( int i = 0; i <  CX2Game::GT_NUMS; ++i )
    {
        if ( stricmp( pszGameType, apszGameType[ i ] ) == 0 )
        {
            eGameType = CX2Game::GAME_TYPE( i );
            goto udpmode;
        }
    }//for
    return;
udpmode:
    for( int i = 0; i < CKTDNUDP::FORCE_CONNECT_MODE_NUMS; ++i )
    {
        if ( stricmp( pszUDPMode, apszUDPMode[ i ] ) == 0 )
        {
            eUDPMode = CKTDNUDP::EForceConnectMode( i );
            goto setmode;
        }
    }//for
    return;
setmode:

    m_aeUDPMode[ eGameType ] = eUDPMode;

//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

CKTDNUDP::EForceConnectMode CX2Main::GetUDPMode( CX2Game::GAME_TYPE eGameType )
{
    if ( eGameType < CX2Game::GAME_TYPE( 0 ) || eGameType >= CX2Game::GT_NUMS )
        return CKTDNUDP::FORCE_CONNECT_MODE_DEFAULT;
    return  m_aeUDPMode[ eGameType ];
}


void    CX2Main::SetUDPMode( CX2Game::GAME_TYPE eGameType, CKTDNUDP::EForceConnectMode eUDPMode )
{
    if ( eGameType < CX2Game::GAME_TYPE( 0 ) || eGameType >= CX2Game::GT_NUMS )
        return;
    if ( eUDPMode < CKTDNUDP::EForceConnectMode( 0 ) || eUDPMode >= CKTDNUDP::FORCE_CONNECT_MODE_NUMS )
        return;

    m_aeUDPMode[ eGameType ] = eUDPMode;
}

//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


#if X2_DEV_TOOLS // ??????, ?????? ?????? ??? ??? ???
// ?????? ?????? ??? ??? ???
// DevScriptTable.lua ?????? ?��?��?.

bool CX2Main::ResetDeveloperScriptSet ( const WCHAR* pFileName )	// DevScriptTable.lua ???
{
	m_DeveloperScriptSet.Clear();


	if ( false == g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile ( pFileName ) )
		return false;

	if ( false == m_DeveloperScriptSet.OpenScriptFile( pFileName ) )
		return false;

	DeleteFile ( L"DevScriptTable_Log.txt");
	KLOG("DevScriptTable_Log.txt")  << L"-----DevScriptTool ??? ???? ????-----" << fileout;


	vector<DeveloperScriptSet::DeveloperScript> & pVecDeveloperScript = m_DeveloperScriptSet.GetDeveloperScriptSet();

	BOOST_FOREACH ( DeveloperScriptSet::DeveloperScript devScript, pVecDeveloperScript )
	{
		if ( true == devScript.m_wstrDeveloperScriptFileName.empty() )
		{
			ASSERT (!L"Can't Find DevScriptSet Script Name" );
			KLOG("DevScriptTable_Log.txt")  << L"Error : " << L" : NAME ??????? ??? ?? ???????." << fileout;
		}
		switch ( devScript.m_eDeveloperScriptType )
		{
		case DeveloperScriptSet::DST_GAME_EFFECT_SET:
			if ( false == ResetDeveloperScriptSet_GameEffectSet ( devScript ) )
			{
				ASSERT (!L"Can't Find DevScriptSet GameEffectSet Script" );
			}
			break;
		case DeveloperScriptSet::DST_DAMAGE_EFFECT:
			if ( false == ResetDeveloperScriptSet_DamageEffect( devScript ) )
			{
				ASSERT (!L"Can't Find DevScriptSet DamageEffect Script" );
			}
			break;
		case DeveloperScriptSet::DST_GAME_MAJOR_XMESH_PLAYER:
			if ( false == ResetDeveloperScriptSet_GameMajorXMeshPlayer( devScript ) )
			{
				ASSERT (!L"Can't Find DevScriptSet GameMajorXMeshPlayer Script" );
			}
			break;
		default :
			ASSERT (!L"Can't Find DevScriptSet Type" );
		}
 	}
	KLOG("DevScriptTable_Log.txt")  << L"-----DevScriptTool ??? ???? ???-----" << fileout;

	return true;
}

bool CX2Main::ResetDeveloperScriptSet_GameEffectSet ( DeveloperScriptSet::DeveloperScript devScript ) // DevScriptTable.lua ???
{
	/// ???? ?? ??? ??? ?��?, Swap ?? ???, Merge ?? ??? ?��? ??? ?? ??????.
	vector<CX2EffectSet*> pVecGameEffectSet = g_pData->m_vecGameEFfectSet;
	BOOST_FOREACH ( CX2EffectSet * pEffectSet, pVecGameEffectSet )
	{
		SAFE_DELETE( pEffectSet );
	}

	if ( devScript.m_eDeveloperScriptRefreshType == DeveloperScriptSet::DSCRT_SWAP )
	{
		g_pData->GetGameEffectSet()->ClearTempletAndInstance();
		if ( NULL == g_pData->ResetGameEffectSet( devScript.m_wstrDeveloperScriptFileName.c_str() ) )
		{
			KLOG("DevScriptTable_Log.txt")  << L"Error : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: EffectSet ?????? ??? ?? ???????." << fileout;
			return false;
		}
		KLOG("DevScriptTable_Log.txt")  << L"Swap : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: EffectSet ???? ?????? ???? ????????." << fileout;
	}
	else if ( devScript.m_eDeveloperScriptRefreshType == DeveloperScriptSet::DSCRT_MERGE )
	{
		CX2EffectSet * pGameEffectSet = new CX2EffectSet ();
		if ( false == pGameEffectSet->OpenScriptFile ( devScript.m_wstrDeveloperScriptFileName ) )
		{
			KLOG("DevScriptTable_Log.txt")  << L"Error : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: EffectSet ?????? ??? ?? ???????." << fileout;
			return false;
		}
		pVecGameEffectSet.push_back ( pGameEffectSet );

		KLOG("DevScriptTable_Log.txt")  << L"Merge : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: ????? ???? ??? ???? ????" << fileout;

		CX2EffectSet::EffectSetDataMap & mapEffectSetTemplet = pGameEffectSet->GetMapEffectSetTemplet();
		// ???? ?? ??????? ???��????? ????? ?????? ???????.
		BOOST_TEST_FOREACH( CX2EffectSet::EffectSetDataMap::value_type&, value, mapEffectSetTemplet )
		{
			CX2EffectSet::EffectSetData * pEffectSetData = g_pData->GetGameEffectSet()->GetEffectSetTemplet(value.first);
			if ( false == g_pData->GetGameEffectSet()->MergeEffectSetTemplet( value.second, pEffectSetData) )
			{
				KLOG("DevScriptTable_Log.txt")  << L"Merge : " << value.second->m_wstrEffectSetName.c_str() << L"\t: EffectSetTemplet ?? ????? ??????." << fileout;
			}
		}

		KLOG("DevScriptTable_Log.txt")  << L"Merge : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: ????? ???? ??? ???? ????\n" << fileout;
	}
	return true;
}
bool CX2Main::ResetDeveloperScriptSet_DamageEffect ( DeveloperScriptSet::DeveloperScript devScript ) // DevScriptTable.lua ???
{
	if ( NULL != g_pX2Game )
	{
		CX2DamageEffect * pDamageEffect = g_pX2Game->GetDamageEffect();
		if ( NULL != pDamageEffect )
		{
			/// Step 1. ???? ?? ??? ??? ?��?, Swap ?? ???, Merge ?? ??? ?��? ??? ?? ??????.
			if ( devScript.m_eDeveloperScriptRefreshType == DeveloperScriptSet::DSCRT_SWAP )
			{
				SAFE_DELETE( pDamageEffect );
				pDamageEffect	= new CX2DamageEffect();
				pDamageEffect->OpenScriptFile( devScript.m_wstrDeveloperScriptFileName.c_str() );
			}
			else if ( devScript.m_eDeveloperScriptRefreshType == DeveloperScriptSet::DSCRT_MERGE )
			{
				pDamageEffect->OpenScriptFile ( devScript.m_wstrDeveloperScriptFileName.c_str() );
			}
			return true;
		}
	}
	return false;
}
bool CX2Main::ResetDeveloperScriptSet_GameMajorXMeshPlayer ( DeveloperScriptSet::DeveloperScript devScript ) // DevScriptTable.lua ???
{
	/// Step 1. ???? ?? ??? ??? ?��?, Swap ?? ???, Merge ?? ??? ?��? ??? ?? ??????.
	vector<CKTDGXMeshPlayer*> m_vecGameMajorXMeshPlayer = g_pData->m_vecGameMajorXMeshPlayer;
	BOOST_FOREACH ( CKTDGXMeshPlayer * pMeshPlayer, m_vecGameMajorXMeshPlayer )
	{
		SAFE_DELETE( pMeshPlayer );
	}

	if ( devScript.m_eDeveloperScriptRefreshType == DeveloperScriptSet::DSCRT_SWAP )
	{
		if ( NULL == g_pData->ResetGameMajorXMeshPlayer ( devScript.m_wstrDeveloperScriptFileName.c_str() ) )
		{
			KLOG("DevScriptTable_Log.txt")  << L"Error : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: GameMajorXMeshPlayer ?????? ??? ?? ???????." << fileout;
			return false;
		}
		KLOG("DevScriptTable_Log.txt")  << L"Swap : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: GameMajorXMeshPlayer ???? ?????? ???? ????????." << fileout;
	}
	else if ( devScript.m_eDeveloperScriptRefreshType == DeveloperScriptSet::DSCRT_MERGE )
	{
		CKTDGXMeshPlayer* pGameMajorXMeshPlayer = new CKTDGXMeshPlayer();
		pGameMajorXMeshPlayer->OpenScriptFile ( devScript.m_wstrDeveloperScriptFileName.c_str() );
		m_vecGameMajorXMeshPlayer.push_back ( pGameMajorXMeshPlayer );

		// ?? ??????? ???? ???��??? ???��?.

		KLOG("DevScriptTable_Log.txt")  << L"Merge : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: ????? ???? ??? ???? ????" << fileout;

		CKTDGXMeshPlayer::XMeshTempletMap & mapXMeshTemplet = pGameMajorXMeshPlayer->GetTempletMap();

		// ???? ?? ??????? ???��????? ????? ?????? ??????.
		BOOST_TEST_FOREACH( CKTDGXMeshPlayer::XMeshTempletMap::value_type&, value, mapXMeshTemplet )
		{
			CKTDGXMeshPlayer::XMeshTemplet * pXMeshTemplet = const_cast<CKTDGXMeshPlayer::XMeshTemplet * > ( g_pData->GetGameMajorXMeshPlayer()->GetTempletByName( value.first ) );
			if ( false == g_pData->GetGameMajorXMeshPlayer()->MergeXMeshTemplet( value.second, pXMeshTemplet ) )
			{
				KLOG("DevScriptTable_Log.txt")  << L"Merge : " << value.second->templetName.c_str() << L"\t: XMeshPlayer Templet ?? ????? ??????." << fileout;
			}
		}
		KLOG("DevScriptTable_Log.txt")  << L"Merge : " << devScript.m_wstrDeveloperScriptFileName.c_str() << L"\t: ????? ???? ??? ???? ????\n" << fileout;

	}
	return true;
}

#endif // EXPAND_DEVELOPER_SCRIPT  // ??????, ?????? ?????? ??? ??? ???

// ???? ?? ????? ??????? @1 ?????? ????????? ???
// ??? ?????? GET_REPLACED_STRING ????? ?? ??????
wstring CX2Main::GetReplacedWstring( wstring & wstrModify, char* szParamTypeList, ... )
{
	wstring wstrReplacedString = wstrModify;
	int iParamTypeListSize = strlen( szParamTypeList );

	va_list marker;
	va_start( marker, szParamTypeList );

	for( int i = 0; i < iParamTypeListSize; i++ )
	{
		std::wstringstream wstmReplaceSymbol;
		wstmReplaceSymbol << L"@" << ( i + 1 );

		std::wstringstream wstmReplaceValue;

		// ????? ???? ????????? ?????.
		switch( szParamTypeList[i] )
		{
		case 'h':       // ????? ?????? ?????? ???
			{
				int iParam = va_arg( marker, int );
				if( iParam > 0 )
				{
					wstmReplaceValue << L"+";
				}
				wstmReplaceValue << iParam;
			}
			break;
		case 'i':       // ?????? ???
			{
				int iParam = va_arg( marker, int );
				wstmReplaceValue << iParam;
			}
			break;
		case 'e':       // ????? ?????? ????? ???
			{
				// ????? ?? ??��????????? ??????, ???? 0??? 0?? ?????. 1.50 -> 1.5
				double dParam = va_arg( marker, double );
				wchar_t szNumber[64];
				StringCchPrintfW( szNumber, ARRAY_SIZE( szNumber ), L"%.2f", dParam );
				dParam = _wtof( szNumber );
				if( dParam > 0.0 )
				{
					wstmReplaceValue << L"+";
				}
				wstmReplaceValue << dParam;
			}
			break;
		case 'f':       // ????? ???
			{
				// ????? ?? ??��????????? ??????, ???? 0??? 0?? ?????. 1.50 -> 1.5
				double dParam = va_arg( marker, double );
				wchar_t szNumber[64];
				StringCchPrintfW( szNumber, ARRAY_SIZE( szNumber ), L"%.2f", dParam );
				dParam = _wtof( szNumber );
				wstmReplaceValue << dParam;
			}
			break;

		case 'I':       // ID?? ??????? ???
			{
				int iParam = va_arg( marker, int );
				wstmReplaceValue << GET_STRING ( iParam );
			}
			break;
		case 's':       // ??????? ???
			{
				std::string strValue = va_arg( marker, char* );
				wstmReplaceValue << KncUtil::toWideString( strValue );
			}
			break;
		case 'S':
			{
				wstmReplaceValue << va_arg( marker, wchar_t* );
			}
			break;
		case 'l':       // std::string ???? ???
			{
				std::string strValue = va_arg( marker, std::string );
				wstmReplaceValue << KncUtil::toWideString( strValue );
			}
			break;
		case 'L':       // std::wstring ???? ???
			{
				wstmReplaceValue << va_arg( marker, std::wstring );
			}
			break;
		case 'M':       // ????? ?????? ??? ???
		case 'm':
			{
			}
			break;
		case 'o':       // ????? ?????? ???? ???
		case 'O':
			{
			}
			break;
		case 'n':       // ????? ?????? ???? ???
		case 'N':
			{
			}
			break;
		}

		std::wstring wstrReplaceSymbol = wstmReplaceSymbol.str().c_str();
		std::wstring wstrReplaceValue = wstmReplaceValue.str().c_str();

		size_t iFindPos = 0;
		size_t iCurrentPos = 0;
		while( ( iFindPos = wstrReplacedString.find( wstrReplaceSymbol, iCurrentPos ) ) != std::wstring::npos )
		{
			wstrReplacedString.replace( iFindPos, wstrReplaceSymbol.size(), wstrReplaceValue );
			iCurrentPos = iFindPos + wstrReplaceValue.length();
		}
	}
	va_end( marker );

	return wstrReplacedString;
}
