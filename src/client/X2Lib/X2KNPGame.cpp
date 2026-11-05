#include "stdafx.h"
#include "./X2KNPGame.h"

#include <direct.h>
#include "../X2/resource.h"
#include "./X2SMTPMail.h"

CNPGameLib*	pNpgl = NULL;

extern CX2SMTPMail* g_pX2SMTPMail; // x2.cpp에 정의되어있음
bool g_bCheckHackUser = true;

bool CheckNPHackUser()
{
    return true;
}

LRESULT CALLBACK SendGameGuardMailDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			//g_pX2SMTPMail->SetReceiverEMail( "Game1@inca.co.kr" );
			g_pX2SMTPMail->SetReceiverEMail( "" );

			g_pX2SMTPMail->SetSenderName( "엘소드유저" );
			g_pX2SMTPMail->SetSubject( "게임가드 오류레포트입니다." );
			g_pX2SMTPMail->SetDesc( "게임가드 폴더 내의 .erl 파일을 첨부하였습니다." );

			g_pX2SMTPMail->ClearFile();
			std::vector< std::string > vecFiles;

			char* buffer = NULL;
			if( (buffer = _getcwd( NULL, 255 )) == NULL )
			{
				// error
				return TRUE;
			}

			stringstream strmFolder;
			strmFolder << buffer << "\\GameGuard";
			free(buffer);

			// npgl.erl이 멜에 첨부가 안되는 경우가 있어서 파일을 복사해서 보내도록 한다
			stringstream strmSrcNPGL;
			strmSrcNPGL << strmFolder.str() << "\\npgl.erl";
			stringstream strmDestNPGL;
			strmDestNPGL << strmFolder.str() << "\\npgl_copy.erl";
			CopyFileA( strmSrcNPGL.str().c_str(), strmDestNPGL.str().c_str(), FALSE );



			g_pX2SMTPMail->FindFilesByExt( vecFiles, strmFolder.str().c_str(), ".erl" );
			//g_pX2SMTPMail->FindFilesByExt( vecFiles, "C:\\Nexon\\ElSword\\Data\\GameGuard", ".erl" );

			for( UINT i=0; i<vecFiles.size(); i++ )
			{
				g_pX2SMTPMail->AddFile( vecFiles[i].c_str());
			}

			bool bResult = g_pX2SMTPMail->SendMail( hDlg );

			return TRUE;
		} break;

	case WM_COMMAND:
		{
			return TRUE;
		} break;
	}
	return FALSE;
}



bool KNP_Init( HINSTANCE hInst )
{
	return true;

	if ( CheckNPHackUser() == false )
	{
		return true;
	}

	// 해외팀 각국가 전용 디파인


	//nProtect 생성실패.?
	if( pNpgl == NULL )
		return false;

	DWORD dwResult = pNpgl->Init();

	//게임가드 초기화시 에러가 있으면 종료처리를 해준다.
	if( dwResult != NPGAMEMON_SUCCESS )
	{
		WCHAR wstrErrMSG[MAX_PATH] = L"";

		// 주요에러코드’를 참조하여 상황에 맞는 메시지를 출력해줍니다.
		switch( dwResult )
		{
		case NPGAMEMON_ERROR_EXIST:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_320 ) );
			break;
		case NPGAMEMON_ERROR_GAME_EXIST:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_321 ) );
			break;
		case NPGAMEMON_ERROR_INIT:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_322 ) );
			break;
		case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
		case NPGAMEMON_ERROR_NFOUND_GG:
		case NPGAMEMON_ERROR_AUTH_INI:
		case NPGAMEMON_ERROR_NFOUND_INI:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_323 ) );
			break;
		case NPGAMEMON_ERROR_CRYPTOAPI:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_324 ) );
			break;
		case NPGAMEMON_ERROR_EXECUTE:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_325 ) );
			break;
		case NPGAMEMON_ERROR_ILLEGAL_PRG:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_326 ) );
			break;
		case NPGMUP_ERROR_ABORT:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_327 ) );
			break;
		case NPGMUP_ERROR_CONNECT:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_328 ) );
			break;
		case NPGAMEMON_ERROR_GAMEGUARD:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_329 ) );
			break;
		case NPGMUP_ERROR_PARAM:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_330 ) );
			break;
		case NPGMUP_ERROR_INIT:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_331 ) );
			break;
		case NPGMUP_ERROR_DOWNCFG:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_332 ) );
			break;
		case NPGMUP_ERROR_AUTH:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_333 ) );
			break;
		case NPGAMEMON_ERROR_NPSCAN:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_334 ) );
			break;
		case NPGG_ERROR_COLLISION:
			wcscpy( wstrErrMSG, GET_STRING( STR_ID_335 ) );
			break;
		default:
			{
				// 적절한 종료 메시지 출력
				wcscpy( wstrErrMSG, GET_STRING( STR_ID_336 ) );
				DialogBox( hInst, (LPCTSTR)IDD_SEND_MAIL_DIALOG, NULL, (DLGPROC)SendGameGuardMailDlgProc );
			}
			break;
		}

		wcscat( wstrErrMSG, GET_REPLACED_STRING( ( STR_ID_337, "i", ( int )dwResult ) ) );

		// 게임 종료코드
		MessageBox( NULL, wstrErrMSG, GET_STRING( STR_ID_338 ), MB_OK | MB_ICONERROR );

		// 에러 코드를 넘겨주면 에러 발생시 자동으로 게임가드 FAQ 페이지를 띄워줌
		//pNpgl->RunFAQ(dwResult);

		return false;
	}

	return true;
}

void KNP_Close()
{
	return;

	if ( CheckNPHackUser() == false )
	{
		return;
	}

	if( pNpgl != NULL )
	{
		SAFE_DELETE( pNpgl );
	}
}

void KNP_SetHwnd(HWND hWnd )
{

	return;

	if ( CheckNPHackUser() == false )
		return;


	if( pNpgl == NULL )
	{
		ASSERT( pNpgl != NULL );
		return;
	}

	pNpgl->SetHwnd( hWnd );
}

DWORD	dwCheckNPTime	= 0;
DWORD	dwTime			= 0;
bool	bIsKNPSuccess	= true;

bool KNP_GetFindGameMon()
{
	return true;

	if ( CheckNPHackUser() == false )
	{
		return true;
	}


	if( g_pKTDXApp == NULL )
	{
		ASSERT( g_pKTDXApp != NULL );
		return true;
	}

	if( pNpgl == NULL )
	{
		//ASSERT( pNpgl != NULL );
		return true;
	}

	if( bIsKNPSuccess == false )
	{
		return bIsKNPSuccess;
	}

	dwCheckNPTime += GetTickCount() - dwTime;
	//일정 시간주기로 체크를 하게 한다.(5~10s 간격으로),
	if( dwCheckNPTime > CHECK_KNP_TIME )
	{
		if( pNpgl->Check() != NPGAMEMON_SUCCESS )
		{
			g_pKTDXApp->SetFindHacking( true, GET_STRING( STR_ID_339 ) );

			bIsKNPSuccess = false;

			return bIsKNPSuccess;
		}

		dwCheckNPTime = 0;
	}

	dwTime = GetTickCount();

	return bIsKNPSuccess;
}

DWORD KNP_Send( LPCWSTR wstrMSG )
{
	return 0;

	if ( CheckNPHackUser() == false )
	{
		return 0;
	}

	if( pNpgl == NULL )
	{
		ASSERT( pNpgl != NULL );
		return 0;
	}

	return pNpgl->Send( wstrMSG );
}

void KNP_AUTH2( GG_AUTH_DATA* pAuthData )
{
	return;

	if ( CheckNPHackUser() == false )
	{
		return;
	}

	if( pNpgl == NULL )
	{
		ASSERT( pNpgl != NULL );
		return;
	}

	pNpgl->Auth2( pAuthData );
}


BOOL CALLBACK NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	return TRUE;

	if ( CheckNPHackUser() == false )
	{
		return TRUE;
	}


	if( g_pKTDXApp == NULL )
	{
		return TRUE;
	}

	if( pNpgl == NULL )
	{
		ASSERT( pNpgl != NULL );
		return FALSE;
	}

	if ( g_pData == NULL || g_pData->GetServerProtocol() == NULL )
	{
		return FALSE;
	}

	if (g_pData->GetServerProtocol()->IsConnected() == false)
	{
		return FALSE;
	}

	WCHAR wstrErrMSG[MAX_PATH] = L"";

	switch (dwMsg)
	{

	case NPGAMEMON_CHECK_CSAUTH2:
		{
			KEGS_KNPROTECT_AUTH_CHECK_NOT kPacket;
			memcpy( &kPacket, (PVOID)dwArg, sizeof(GG_AUTH_DATA) );
			g_pData->GetServerProtocol()->SendPacket( EGS_KNPROTECT_AUTH_CHECK_NOT, kPacket );
		}
		return TRUE;

	case NPGAMEMON_COMM_ERROR:
	case NPGAMEMON_COMM_CLOSE:
		{
			g_pData->GetServerProtocol()->SendID( EGS_NPROTECT_HACKING_USER_NOT );
			g_pKTDXApp->SetFindHacking( true, wstrErrMSG );
			g_pKTDXApp->SetGameGuardFindHacking(true);
		}
		return FALSE;

	case NPGAMEMON_INIT_ERROR:
		{
			g_pData->GetServerProtocol()->SendID( EGS_NPROTECT_HACKING_USER_NOT );
			g_pKTDXApp->SetFindHacking( true, GET_REPLACED_STRING( ( STR_ID_340, "i", ( int )dwArg ) ) );
		}
		return FALSE;

	case NPGAMEMON_SPEEDHACK:
		{
			g_pData->GetServerProtocol()->SendID( EGS_NPROTECT_HACKING_USER_NOT );
			g_pKTDXApp->SetFindHacking( true, GET_STRING( STR_ID_341 ) );
			g_pKTDXApp->SetGameGuardFindHacking(true);
		}
		return FALSE;

	case NPGAMEMON_GAMEHACK_KILLED:
		{

			g_pData->GetServerProtocol()->SendID( EGS_NPROTECT_HACKING_USER_NOT );
			g_pKTDXApp->SetFindHacking( true, GET_REPLACED_STRING( ( STR_ID_342, "S", pNpgl->GetInfo() ) ) );
			g_pKTDXApp->SetGameGuardFindHacking(true);

		}
		return FALSE;

	case NPGAMEMON_GAMEHACK_DETECT:
		{

			g_pData->GetServerProtocol()->SendID( EGS_NPROTECT_HACKING_USER_NOT );
			g_pKTDXApp->SetFindHacking( true, GET_REPLACED_STRING( ( STR_ID_342, "S", pNpgl->GetInfo() ) ) );
			g_pKTDXApp->SetGameGuardFindHacking(true);

		}
		return FALSE;

	case NPGAMEMON_GAMEHACK_DOUBT:
		{
			g_pData->GetServerProtocol()->SendID( EGS_NPROTECT_HACKING_USER_NOT );
			g_pKTDXApp->SetFindHacking( true, GET_STRING( STR_ID_343 ) );

			g_pKTDXApp->SetGameGuardFindHacking(true);

		}
		return FALSE;

	case NPGAMEMON_GAMEHACK_REPORT:
		{
			DWORD	dwHackInfoSize = 0;
			LPBYTE	pHackInfo = NULL;
			//pHackInfo = GetHackInfoFromGameMon(&dwHackInfoSize);
			pHackInfo = pNpgl->GetHackInfo(&dwHackInfoSize); // C++ 일 경우.
			if( pHackInfo && dwHackInfoSize > 0 )
			{
				// 아래 함수는 게임가드에서 제공하는 함수가 아닙니다.
				//SendToHackLog(pHackInfo, dwHackInfoSize); // 서버로 데이터 전송.

			}
		}
		return TRUE;

	}

	return TRUE; // 계속 진행
}
