#include "stdafx.h"
#include "x2/platform/process.h"


#include "./X2StateLogin.h"



//{{ 2011.3.21	조효진	핵쉴드 모니터링 서버 연동
//}}

CX2StateLogin::CX2StateLogin(void) :
m_pDLGLoginBack( NULL ),
m_pDLGLoginFront( NULL )
#ifdef UIRRL_FIRST_SCREEN
,m_pTexutureBG( NULL )
#endif //UIRRL_FIRST_SCREEN
{

#ifdef EXTEND_SERVER_GROUP_MASK
		// 미국은 여기서 파일을 읽어야 합니다. 이전 접속 서버를 알아야 하기 때문에...
	if ( g_pInstanceData->GetServerGroupID() == -1 )
		OpenScriptServerGroupFile();
	g_pMain->SetPickedChannelServerIPIndex( g_pInstanceData->GetServerGroupID() );
#else
	if ( g_pInstanceData->GetServerGroupID() == SGI_INVALID )
		OpenScriptServerGroupFile();
	g_pMain->SetPickedChannelServerIPIndex( g_pInstanceData->GetServerGroupID() );
#endif // EXTEND_SERVER_GROUP_MASK

#if X2_DEV_TOOLS
	if( g_pMain->GetManualLogin() == false )
	{
		TryVerifyAccount();
	}
	else
	{
		InitManualLogin();
	}
#else

	{
		//Service 모드일 때 들어오는 라인
		if( g_pMain->GetManualLogin() )
		{
			InitManualLogin();
		}
		else
		{
			RetrieveUserIDnPassword();
			//LoginNexonKorea();
			TryVerifyAccount();
		}
	}

#endif // _SERVICE_



	/*
	g_pKTDXApp->GetDGManager()->SetProjection(
		g_pKTDXApp->GetDGManager()->GetNear(),
		g_pKTDXApp->GetDGManager()->GetFar(), false );

	g_pKTDXApp->GetDGManager()->GetCamera().Point( 0,0,-1300, 0,0,0 );
	g_pKTDXApp->GetDGManager()->GetCamera().UpdateCamera( 1.0f );
	*/

	//CKTDGXMeshPlayer::CXMeshInstance* m_pMeshInstRandomBox = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  L"RewardItemBox", 0.0f, -100.0f, 0.0f , 0,0,0, 0,0,0 );
	//m_pMeshInstRandomBox->SetOverUI( true );
	//CKTDGParticleSystem::CParticleEventSequence* pTemp = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"BoxJewel", 0,0,-100 );
	//pTemp->SetOverUI( true );






#if X2_DEV_TOOLS

	if ( CheckIsVistaComputer() == true )
	{
		//MessageBox( g_pKTDXApp->GetHWND(), L"비스타 맞음?", L"확인", MB_OK );
	}

#endif











	g_pKTDXApp->SkipFrame();

//{{ kimhc // 2010.5.11 // 사내에서 자동 로그인 할 수 있는 기능
#if X2_DEV_TOOLS
	ReadIDAndPassword();
#endif // AUTO_LOGIN_IN_HOUSE
//}} kimhc // 2010.5.11 // 사내에서 자동 로그인 할 수 있는 기능


}

CX2StateLogin::~CX2StateLogin(void)
{
#ifdef UIRRL_FIRST_SCREEN
	SAFE_CLOSE( m_pTexutureBG );
#endif //UIRRL_FIRST_SCREEN
	SAFE_DELETE_DIALOG( m_pDLGLoginBack );
	SAFE_DELETE_DIALOG( m_pDLGLoginFront );
}

// 09.10.14 태완 : 중복되는 곳이 많아서 함수로 묶어 정리
void CX2StateLogin::InitManualLogin()
{
	SAFE_DELETE_DIALOG(m_pDLGLoginBack);
	SAFE_DELETE_DIALOG(m_pDLGLoginFront);

	m_pDLGLoginBack = new CKTDGUIDialog( this, L"DLG_Login_Back.lua" );

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGLoginBack );



	{
        if ( m_pDLGLoginBack != NULL && m_pDLGLoginBack->GetControl(L"IMELoginID") != NULL )
        {
		    m_pDLGLoginBack->GetControl(L"IMELoginID")->RequestFocus();
        }
	}

}


HRESULT CX2StateLogin::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2State::OnFrameMove(fTime, fElapsedTime);



	if(m_pDLGLoginBack->GetControl(L"IMELoginID")->GetHaveFocusIn() &&
		g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_TAB))
	{
		m_pDLGLoginBack->RequestFocus(m_pDLGLoginBack->GetControl(L"EditBoxLoginPassword"));
	}
	else if(m_pDLGLoginBack->GetControl(L"EditBoxLoginPassword")->GetHaveFocusIn() &&
		g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_TAB))
	{
		m_pDLGLoginBack->RequestFocus(m_pDLGLoginBack->GetControl(L"IMELoginID"));
	}

	return S_OK;
}

HRESULT CX2StateLogin::OnFrameRender()
{
	CX2State::OnFrameRender();



	return S_OK;
}

bool CX2StateLogin::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	if ( CX2State::UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	switch(wParam)
	{
		case SLUCM_LOGIN:
			{
				RetrieveUserIDnPassword();
				TryVerifyAccount();
			}
			break;

		case SLUCM_HOMEPAGE_MOVE :
			{
				wstring wstrURL = L"";
				wstrURL = L"http://www.elswordonline.com";
				x2::platform::OpenUrl( wstrURL );
			}
			break;
		case SLUCM_REGISTER_MOVE :
			{
				wstring wstrURL = L"";
				wstrURL = L"https://myaccount.elswordonline.com/registration";
				x2::platform::OpenUrl( wstrURL );
			}
			break;
		case SLUCM_FACEBOOK_MOVE :
			{
				wstring wstrURL = L"http://www.facebook.com/ElswordOnlineNA";
				x2::platform::OpenUrl( wstrURL );
			}
			break;
		case SLUCM_STEAM_COMMUNITY_MOVE :
			{
				wstring wstrURL = L"http://steamcommunity.com/sharedfiles/filedetails/?id=93245387";
				x2::platform::OpenUrl( wstrURL );
			}
			break;
		case SLUCM_CHANGE_STATE_MEMBER_JOIN:
			{
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_JOIN, NULL, false );
				return true;
			}
			break;

		//case SLUCM_AUTO_LOGIN:
		//	{
		//		//g_pInstanceData->SetPlannerServerLogin( true );
		//	} break;
		//
		//case SLUCM_NOT_AUTO_LOGIN:
		//	{
		//		//g_pInstanceData->SetPlannerServerLogin( false );
		//	} break;

		case SLUCM_NEXON_KOREA_LOGIN:
			{

				if ( g_pMain->GetManualLogin() == true )
				{
					g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, SLUCM_LOGIN, NULL, false );
				}
			}
			break;

		case SLUCM_NEXON_KOREA_LOGIN_COMPLETE:
			{
				/*
				if( lParam == 1 )
				{
					g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, SLUCM_LOGIN, NULL, false );
					g_pMain->SetNexonLogin( true );
				}
				else
				{
					g_pMain->SetNexonLogin( false );
					if( g_pMain->GetManualLogin() == true )
					{
						g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, SLUCM_LOGIN, NULL, false );
					}
					else
					{
						g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, 0, 0, false );
					}
				}
				*/
			}
			break;
	}

	return false;
}

bool CX2StateLogin::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2State::UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	// 2010. 9. 19 조효진   NEW_LOGIN_PROCESS
	switch( wParam )
	{
	case ECH_VERIFY_ACCOUNT_ACK:
		return Handler_ECH_VERIFY_ACCOUNT_ACK( hWnd, uMsg, wParam, lParam );
		break;
	case ECH_SECURITY_INFO_NOT:
		return Handler_ECH_SECURITY_INFO_NOT( hWnd, uMsg, wParam, lParam );
	default:
		break;
	}

	return false;
}


void CX2StateLogin::StateChangeLoading()
{
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );


	g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_656 ) );

	g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_LOADING, NULL, false );
}


// 로그인 방식에 따라 UI 혹은 nexon passport에서 user id와 password를 얻어오고, 서버 선택창에서 GS에 접속할 때 사용된다.
void CX2StateLogin::RetrieveUserIDnPassword()
{
	if( g_pMain->GetManualLogin() == true )
	{
		CKTDGUIIMEEditBox*	pIDEditBox			= NULL;
		CKTDGUIEditBox*		pPasswordEditBox	= NULL;

		{
			pIDEditBox			= (CKTDGUIIMEEditBox*)m_pDLGLoginBack->GetControl( L"IMELoginID" );
			pPasswordEditBox	= (CKTDGUIEditBox*)m_pDLGLoginBack->GetControl( L"EditBoxLoginPassword" );
		}

		g_pInstanceData->SetUserID( pIDEditBox->GetText() );

		g_pInstanceData->SetUserPassword( pPasswordEditBox->GetText() );

	}
	else
	{
		switch( g_pMain->GetPublisher() )
		{
		}
	}



#if X2_DEV_TOOLS
	wstringstream wstrmWindowTitle;
	wstrmWindowTitle << L"X2_" << g_pMain->GetClientVersion() << L"; " << __DATE__ << L", " << __TIME__ << L"; " << g_pInstanceData->GetUserID().c_str() << L"; " << _getpid();
	SetWindowText( g_pKTDXApp->GetHWND(), wstrmWindowTitle.str().c_str() );
#endif // _IN_HOUSE_


}
//{{ kimhc // 2010.5.11 // 사내에서 자동 로그인 할 수 있는 기능
#if X2_DEV_TOOLS
void CX2StateLogin::ReadIDAndPassword()
{
	const wstring wstrFileName( L"LoginKey.lua" );

	if( true == g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile( wstrFileName ) )
	{
		KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

        if ( g_pKTDXApp->LoadAndDoMemory( &luaManager, wstrFileName.c_str() ) == true )
		{
			if ( luaManager.BeginTable( "LOGIN" ) == true )
			{
				wstring wstrID, wstrPassword;

				LUA_GET_VALUE(	luaManager, "ID", wstrID,	L"" );
				LUA_GET_VALUE(	luaManager, "PASSWORD", wstrPassword,	L"" );

				if ( wstrID.empty() == false && wstrPassword.empty() == false )
				{
					g_pInstanceData->SetUserID( wstrID );
					g_pInstanceData->SetUserPassword( wstrPassword );

					// 일단 OTP 넣지 않음

					StateChangeLoading();

					luaManager.EndTable();
					return;
				}
				luaManager.EndTable();
			}
		}
	}

	const wstring wstrFileNameEx( L"LoginKeyEx.lua" );
	if( true == g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->IsValidFile( wstrFileNameEx ) )
	{
		KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

        if ( g_pKTDXApp->LoadAndDoMemory( &luaManager, wstrFileNameEx.c_str() ) == true )
		{
			if ( luaManager.BeginTable( "LOGIN" ) == true )
			{
				wstring wstrID, wstrPassword;

				LUA_GET_VALUE(	luaManager, "ID", wstrID,	L"" );
				LUA_GET_VALUE(	luaManager, "PASSWORD", wstrPassword,	L"" );

				if ( wstrID.empty() == true || wstrPassword.empty() == true )
					return;
				else
				{
					if( 1 != MessageBox( g_pKTDXApp->GetHWND(), L"자동 로그인 기능 사용하시겠습니까?", L"!", MB_OKCANCEL ) )
						return;

					g_pInstanceData->SetUserID( wstrID );
					g_pInstanceData->SetUserPassword( wstrPassword );

					// 일단 OTP 넣지 않음

					StateChangeLoading();
				}
				luaManager.EndTable();
			}
		}
	}
}
#endif // AUTO_LOGIN_IN_HOUSE
//}} kimhc // 2010.5.11 // 사내에서 자동 로그인 할 수 있는 기능

bool CX2StateLogin::OpenScriptServerGroupFile()
{
	return true;

#ifdef EXTEND_SERVER_GROUP_MASK
	g_pInstanceData->SetServerGroupID( 0 );
	bool bParsingOK = true;
#else
	string			strFileName;
	SERVER_GROUP_ID eServerGroupID	= SGI_INVALID;
	bool			bParsingOK		= false;

	ConvertWCHARToChar( strFileName, g_pData->GetSavedServerGroupFileName() );

	ConvertFileAnsiToUTF8( strFileName, strFileName );

	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );

	if ( g_pKTDXApp->LoadLuaTinker( g_pData->GetSavedServerGroupFileName().c_str() ) == false )
	{
		ErrorLogMsg( XEM_ERROR9, strFileName.c_str() );
		return false;
	}
	else
	{
		LUA_GET_VALUE_ENUM( luaManager, "SERVER_GROUP", 			eServerGroupID,			SERVER_GROUP_ID,		SGI_INVALID	);
	}

	switch ( eServerGroupID )
	{
	case SGI_SOLES:
	case SGI_GAIA:
		{
			g_pInstanceData->SetServerGroupID( eServerGroupID );
			bParsingOK = true;
		}
		break;
	default:
		{
			if( g_pMain->GetDefaultChannelServerIPIndex() == SGI_INVALID )
			{
				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( (rand() % 2) ) );
			}
			else
			{
				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( g_pMain->GetDefaultChannelServerIPIndex() ) );
			}
		}
		break;
	}
#endif // EXTEND_SERVER_GROUP_MASK

	return bParsingOK;
}

bool CX2StateLogin::TryVerifyAccount()
{
	if( NULL == g_pData->GetServerProtocol() )
	{
		g_pData->ResetServerProtocol();
	}


	std::wstring wstrChannelServerIP = g_pMain->GetPickedChannelServer().m_kServerIP;
	int iChannelServerPort = g_pMain->GetPickedChannelServer().m_usMasterPort;

	if( g_pData->GetServerProtocol()->IsChConnected() ||
		g_pData->GetServerProtocol()->ConnectedToChannelServer( wstrChannelServerIP.c_str(), iChannelServerPort ) == true )
	{
		return Handler_ECH_VERIFY_ACCOUNT_REQ();
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_735 ), this );
	}

	return false;
}


bool CX2StateLogin::Handler_ECH_VERIFY_ACCOUNT_REQ()
{

	KECH_VERIFY_ACCOUNT_REQ kPacket;
	kPacket.m_wstrID = g_pInstanceData->GetUserID();
	kPacket.m_wstrPassword = g_pInstanceData->GetUserPassword();
	kPacket.m_bLogin = !(g_pInstanceData->IsIDVerified());

	kPacket.m_iChannelingCode = g_pInstanceData->GetChannelingCode();

	// The password Steam logins send: typed by hand without Steam, the client quits (CX2Steam::IsValidSteamUser of the
	// original; there is no Steam client in the port).
	if( kPacket.m_wstrPassword == L"J2380DE81621XZ" )
		g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );

	g_pData->GetServerProtocol()->SendChPacket( ECH_VERIFY_ACCOUNT_REQ, kPacket );
	g_pMain->AddServerPacket( ECH_VERIFY_ACCOUNT_ACK );

	return true;
}

bool CX2StateLogin::Handler_ECH_VERIFY_ACCOUNT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_VERIFY_ACCOUNT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( ECH_VERIFY_ACCOUNT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->SetServerCurrentTime( kEvent.m_wstrCurrentTime );
			g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->MassFileMapping();
			// 메뉴얼 로그인(클라이언트 로그인) 시  최초 성공 부분 (퍼블리셔 인증 성공)
			if( false == kEvent.m_wstrPassport.empty() )
			{
				g_pInstanceData->SetIDVerified(true);
				g_pInstanceData->SetUserID( kEvent.m_wstrUserID );
				g_pInstanceData->SetUserPassword( kEvent.m_wstrPassport );
				g_pInstanceData->SetPurchaseToken( kEvent.m_wstrPurchaseTok );


				//{{ 2011.3.21	조효진	핵쉴드 모니터링 서버 연동
				//}}

				// US의 남미, 북미의 경우 m_iChannelingCode 값이 서버군이 됨
#ifdef EXTEND_SERVER_GROUP_MASK
				if ( kEvent.m_iChannelingCode != -1 )
#else
				if ( kEvent.m_iChannelingCode != SGI_INVALID )
#endif //EXTEND_SERVER_GROUP_MASK
				{
					// 스팀 인증을 통과했는데 실재 클라도 스팀 클라인지 다시 확인
					/* 인증 리턴값으론 확인 불가하다
					if( kEvent.m_iChannelingCode > 1 )
					{
						if( CX2Steam::IsSteamUser() == false )
						{
							// 스팀 클라이언트로 다시 실행시켜 주십시오
							//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4519 ), g_pMain->GetNowState() );
							g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
						}
					}
					*/
					g_pInstanceData->SetChannelingCode(kEvent.m_iChannelingCode);
					g_pInstanceData->SetServerGroupID(kEvent.m_iChannelingCode % 2);
					g_pMain->SetPickedChannelServerIPIndex( g_pInstanceData->GetServerGroupID() );
				}


			}

			StateChangeLoading();

			g_pMain->GetGameOption().SaveScriptFile();
		}
		else
		{
			if( kEvent.m_iOK == NetError::ERR_K3RCOMBO_AUTH_02 )
			{
				x2::platform::OpenUrl( x2::platform::Utf8ToWide( kEvent.m_strAgreementURL ) );
			}

			if( g_pMain->GetManualLogin() == false )
			{
				if(m_pDLGLoginBack == NULL)
				{
					InitManualLogin();
				}
			}

			CKTDGUIEditBox* pPassEditBox = (CKTDGUIEditBox*)m_pDLGLoginBack->GetControl(L"EditBoxLoginPassword");
			if(pPassEditBox != NULL)
				pPassEditBox->ClearText();

			pPassEditBox->RequestFocus();
		}
	}

	return false;
}




bool CX2StateLogin::Handler_ECH_SECURITY_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 인증을 기다리지 않도록 처리
	g_pMain->DeleteServerPacket( ECH_VERIFY_ACCOUNT_ACK );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_SECURITY_INFO_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	return true;
}

bool CX2StateLogin::Handler_ECH_SECURITY_AUTH_REQ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KECH_SECURITY_AUTH_REQ kPacket;

	kPacket.m_wstrUserID = g_pInstanceData->GetUserID();

	g_pData->GetServerProtocol()->SendChPacket( ECH_SECURITY_AUTH_REQ, kPacket );
	g_pMain->AddServerPacket( ECH_VERIFY_ACCOUNT_ACK );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	m_pDLGMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_647 ), this);

	return true;
}



//{{ 2013.03.13 조효진	회원가입 버튼을 누르면 웹페이지를 띄워줌
//}}
