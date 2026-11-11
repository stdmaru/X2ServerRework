#include "stdafx.h"


#include "./X2StateLoading.h"


//extern bool g_bIsDXUTDeveloper;
//extern bool g_bIsDXUTSpecialUser;




CX2StateLoading::CX2StateLoading(void) :
// 2009.01.09 김태완 : 초기화 코드정리
m_pDLGLoadingFront(NULL),
m_fLogoTime(0.0f),
m_hCompanyLogo(INVALID_PARTICLE_SEQUENCE_HANDLE),
m_hLogoBack(INVALID_PARTICLE_SEQUENCE_HANDLE)
//{{AFX
//m_hNexonLogo(INVALID_PARTICLE_SEQUENCE_HANDLE),
//m_hKOGBack(INVALID_PARTICLE_SEQUENCE_HANDLE),
//m_hKOGLogo(INVALID_PARTICLE_SEQUENCE_HANDLE)
//m_hKOGLogoMini(INVALID_PARTICLE_SEQUENCE_HANDLE),
//m_hNexonLogoMini(INVALID_PARTICLE_SEQUENCE_HANDLE),
//m_hPPoruSurprise(INVALID_PARTICLE_SEQUENCE_HANDLE),
//m_hPPoruLogo(INVALID_PARTICLE_SEQUENCE_HANDLE)
//m_pWillium(NULL),
//m_pElSword(NULL),
//m_pLena(NULL),
//m_pAisha(NULL),
//m_pBenders(NULL)
//m_pTextureTestMark;	// Commented in Header file
//}}AFX
{
	//{{AFX
	// 초기화 ////////////////////////////////////////////////////////////////////////
	//}}AFX

#if !X2_DEV_TOOLS
#endif
	//////////////////////////////////////////////////////////////////////////

//{{ robobeg : 2008-10-28
    g_pKTDXApp->EnterLuaThread();
//}} robobeg : 2008-10-28


	TextureReady(L"HQ_DeliberationLogo.dds");
	TextureReady(L"HQ_CompanyLogo.dds");
	//{{AFX
	// KOG 겨울 새 로고
	//TextureReady( L"HQ_KOGLogoWinter.dds" );
	//TextureReady( L"HQ_KOGLogo.dds" );
	//TextureReady( L"HQ_PPoruLogo.dds" );
	//TextureReady( L"HQ_NexonLogoMini.tga" );
	//TextureReady( L"HQ_KOGLogoMini.tga" );
	//}}AFX
	SoundReady( L"NexonLogo.ogg" );
	//g_pKTDXApp->GetDeviceManager()->PlaySound( L"NexonLogo.ogg" );

	m_pDLGLoadingFront = new CKTDGUIDialog( this, L"DLG_First_Loading_Front.lua", 0.0f );

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGLoadingFront );

	wstring wstrClientVersion = g_pMain->GetClientVersion();
	wstrClientVersion += L".";
	wstrClientVersion += g_pMain->GetSubClientVersion_LUA();
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDLGLoadingFront->GetControl( L"Static_Client_Version" );
	if( pStatic != NULL )
	{
		pStatic->GetString(0)->msg = wstrClientVersion;
	}
	m_pDLGLoadingFront->SetShow( false );



	g_pKTDXApp->GetDGManager()->GetCamera().Point( 0,0,-1500, 0,0,0 );
	g_pKTDXApp->GetDGManager()->GetCamera().UpdateCamera( 1.0f );

	SetLoadingGageBar( 0 );

	//m_pTextureTestMark = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_Test_Mark.tga" );
	m_hLogoBack		= g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  L"LogoBack", 512, 384, 0, 9999, 9999, -1, 1 );
	m_hCompanyLogo	= g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  L"CompanyLogo", 512, 384, 0, 9999, 9999, -1, 1 );

//{{AFX

//}}AFX


	g_pData->ResetServerProtocol();



#if !X2_DEV_TOOLS
	// checked
#endif

#ifdef EXTEND_SERVER_GROUP_MASK
#else
	if ( g_pInstanceData->GetServerGroupID() == SGI_INVALID )
    {
		//OpenScriptServerGroupFile();
        g_pInstanceData->OpenScriptServerGroupFile();
    }
	g_pMain->SetPickedChannelServerIPIndex( g_pInstanceData->GetServerGroupID() );
#endif // EXTEND_SERVER_GROUP_MASK

	ConnectToChannelServer();

#if !X2_DEV_TOOLS
#endif


	g_pKTDXApp->SkipFrame();

}

CX2StateLoading::~CX2StateLoading(void)
{
	SAFE_DELETE_DIALOG(m_pDLGLoadingFront);

	g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hCompanyLogo );
	g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hLogoBack );

//{{AFX


//}}AFX

//{{ robobeg : 2008-10-28
    g_pKTDXApp->ExitLuaThread();
//}} robobeg : 2008-10-28


}

HRESULT CX2StateLoading::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2State::OnFrameMove( fTime, fElapsedTime );

//{{AFX
//}}AFX

	m_fLogoTime += fElapsedTime;

	//# 회사로고 4초 후 심의등급 로고로 전환
	if( m_hCompanyLogo != INVALID_PARTICLE_SEQUENCE_HANDLE && m_fLogoTime > 5.0f )
	{
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hCompanyLogo );
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hLogoBack );
		m_hCompanyLogo	= INVALID_PARTICLE_SEQUENCE_HANDLE;
		m_hLogoBack		= INVALID_PARTICLE_SEQUENCE_HANDLE;

		// 심의등급 로고 생성
//{{AFX
//}}AFX
		//m_fLogoTime = 0.0f;	// 해외팀 주석 처리

		{
			// # 테스트를 위해 지정한 txt파일이 게임 폴더 내 존재할 때만 처리
			const string strFileName = "Inactivation_Mimize.txt";
			FILE* file = NULL;
			file = fopen( strFileName.c_str(), "r" );
			if( NULL != file )
			{
				// #x2 윈도우의 위치가 정렬되지 않은 문제를 해결하기 위해 해상도 변경 작업 수행
				if( NULL != g_pMain )
				{
					const D3DXVECTOR2& vRect = g_pMain->GetGameOption().GetResolution();
					// # 기본 해상도이거나, 전체화면일 때 수행하지 않기
					if( !(true == IsSamef( 1024.f, vRect.x) && true == IsSamef(768.f, vRect.y)) &&
						false == g_pMain->GetGameOption().GetIsFullScreen() )
					{
						g_pMain->GetGameOption().SetResolution( static_cast<DWORD>(vRect.x), static_cast<DWORD>(vRect.y) );
					}
				}
				fclose(file);
			}
		}


		g_pKTDXApp->SkipFrame();

//{{AFX
//}}AFX

		if( m_fLogoTime > 5.0f )
		{

			//# 모든 로그 랜더링이 끝나면

			//{{AFX
			//}}AFX


			//# 게임관련 사항 초기화 시작
			m_pDLGLoadingFront->SetShow( true );



			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadUserUnitMotion1();

			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 5 ) ) );
			SetLoadingGageBar( 5 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadUserUnitMotion2();

			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 10 ) ) );
			SetLoadingGageBar( 10 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadUserUnitMotion3();
			g_pData->LoadUserUnitMotion4();


			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 15 ) ) );
			SetLoadingGageBar( 15 );



			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadUserUnitMotion5();


			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 20 ) ) );
			SetLoadingGageBar( 20 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadUserUnitMotion6();
			g_pData->LoadUserUnitMotion7();
			g_pData->LoadUserUnitMotion8();
			g_pData->LoadUserUnitMotion9();


			g_pData->LoadCommonDevice();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 30 ) ) );
			SetLoadingGageBar( 30 );

			g_pKTDXApp->RenderStageNow( this );

			//g_pData->LoadAishaDevice();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 40 ) ) );
			SetLoadingGageBar( 40 );

			g_pKTDXApp->RenderStageNow( this );

			//g_pData->LoadLenaDevice();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 50 ) ) );
			SetLoadingGageBar( 50 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar1();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 54 ) ) );
			SetLoadingGageBar( 54 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar2();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 58 ) ) );
			SetLoadingGageBar( 58 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar3();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 62 ) ) );
			SetLoadingGageBar( 62 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar4();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 66 ) ) );
			SetLoadingGageBar( 66 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar5();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 70 ) ) );
			SetLoadingGageBar( 70 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar6();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 74 ) ) );
			SetLoadingGageBar( 74 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar7();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 78 ) ) );
			SetLoadingGageBar( 78 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar8();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 82 ) ) );
			SetLoadingGageBar( 82 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar9();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 86 ) ) );
			SetLoadingGageBar( 86 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->LoadPicChar10();

			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 90 ) ) );
			SetLoadingGageBar( 90 );

			g_pKTDXApp->RenderStageNow( this );

			// 훈련소 데이터 로드
			g_pData->ResetTrainingCenterTable( L"TrainingCenterTemplet.lua" );

			//아이템 데이타 로드
			g_pData->ResetItemManager(); // 80

			//{{ 2008. 10. 1  최육사
			g_pData->ResetTitleManager();
			//}}

			//{{ 2009. 6. 24  최육사	보상테이블
			g_pData->ResetRewardTable();
			//}}

			//{{ 허상형 : [2009/9/16] //	길드 매니저 생성
			g_pData->ResetGuildManager();
			//}} 허상형 : [2009/9/16] //	길드 매니저 생성

			g_pData->ResetUIManager();
			g_pData->ResetUserMenu();

			g_pData->ResetUserListPopUp();

			g_pData->ResetPetManager();

			g_pData->ResetRelationshipManager();
			g_pData->ResetRelationshipEffectManager();

			g_pData->ResetRidingPetManager();

			g_pData->ResetItemStatCalculator();

			//PVPResult테이블 로드
			g_pData->LoadPVPResultTable();
			//EXP테이블 로드
			g_pData->LoadEXPTable();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 92 ) ) );
			SetLoadingGageBar( 92 );

			g_pKTDXApp->RenderStageNow( this );

			//월드 데이타 로드
			g_pData->ResetWorldManager( L"WorldData.lua" );
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 93 ) ) );
			SetLoadingGageBar( 93 );

			g_pKTDXApp->RenderStageNow( this );

			//던전 데이타 로드
			g_pData->ResetDungeonManager( L"DungeonData.lua" );
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 94 ) ) );
			SetLoadingGageBar( 94 );

			//{{ 허상형 : [2011/3/17/] //	월드미션
			g_pData->ResetWorldMissionManager();
			//}} 허상형 : [2011/3/17/] //	월드미션

			//{{ JHKang / 강정훈 / 2011.6.8 / 휴면 복귀 유저
			g_pData->ResetPremiumBuffManager();
			//}}

			g_pData->GetPremiumBuffManager()->OpenScriptFile( L"BuffTemplet.lua" );

			g_pData->ResetGlobalMissionManager();

			//{{ 오현빈 // 2012-05-15 // 플레이 가이드 Class
			g_pData->ResetPlayGuide();
			//}}
			g_pKTDXApp->RenderStageNow( this );


			//퀘스트 데이타 로드
			g_pData->ResetQuestManager( L"FieldQuest.lua", L"FieldSubQuest.lua", L"AccountQuest.lua" );

			g_pData->OpenDayOfWeekQuestScript( L"AccountQuestDate.lua" );

			g_pData->ResetCubeOpenImageManager();

			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 95 ) ) );
			SetLoadingGageBar( 95 );

			g_pKTDXApp->RenderStageNow( this );

			//스킬트리 데이타 로드

			g_pData->ResetSkillTree( L"NewSkillTempletVer2.lua", L"SkillData.lua" ); // 10

			g_pData->ResetComboTree( L"ComboTree.lua" );

			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 96 ) ) );
			SetLoadingGageBar( 96 );

			g_pKTDXApp->RenderStageNow( this );

			//데미지 매니져
			g_pData->ResetDamageManager();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 97 ) ) );
			SetLoadingGageBar( 97 );

			g_pKTDXApp->RenderStageNow( this );

			g_pData->ResetEventScene(L"EventSceneObject.lua", L"EventScene.lua");

			// slide shot
			g_pData->ResetSlideShot();


			g_pInstanceData->ResetMiniMapUI(); // 3

			CX2EmblemManager::ResetEmblemManager();

			CX2BossRaidManager::ResetBossRaidManager();

			//드랍아이템 리소스
			DropItemLoading();
			SetLoadingString( GET_REPLACED_STRING( ( STR_ID_655, "i", 100 ) ) );
			SetLoadingGageBar( 100 );

			g_pKTDXApp->RenderStageNow( this );

//{{ kimhc // 2010.3.10 //	광고노출도 초기화
//}} kimhc // 2010.3.10 //	광고노출도 초기화

			//Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ();
			g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, NULL, false );


#if !X2_DEV_TOOLS
#endif

		}
	}

	if( ( GET_KEY_STATE(GA_RETURN) || g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_RETURN) == TRUE ) ||
		GET_KEY_STATE(GAMEACTION_SLOT_CHANGE)
		|| g_pKTDXApp->GetDIManager()->GetMouse()->GetButtonState(MOUSE_LBUTTON) == TRUE )
	{
		if( m_hCompanyLogo != INVALID_PARTICLE_SEQUENCE_HANDLE )
			m_fLogoTime = 5.0f;
	}

	return S_OK;
}

HRESULT CX2StateLoading::OnFrameRender()
{
	//CX2State::OnFrameRender();

	g_pKTDXApp->GetDGManager()->ObjectChainSort();
//{{ robobeg : 2008-10-17
    g_pKTDXApp->GetDGManager()->ObjectChainNonAlphaRender();
    g_pKTDXApp->GetDGManager()->ObjectChainAlphaRender();

//}} robobeg : 2008-10-17
	g_pKTDXApp->GetDGManager()->FrontUIRender();

	g_pMain->GetTextManager()->OnFrameRender();

	if ( g_pMain->GetGameEdit() )
		g_pMain->GetGameEdit()->OnFrameRender();

	//{{AFX
	//}}AFX

	m_pCursor->OnFrameRender();

	return S_OK;
}

bool CX2StateLoading::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2State::UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

#if !X2_DEV_TOOLS
#endif

	switch( wParam )
	{
	case ECH_VERIFY_ACCOUNT_ACK:
		return Handler_ECH_VERIFY_ACCOUNT_ACK(hWnd, uMsg, wParam, lParam);
	case ECH_DISCONNECT_ACK:
		return Handler_ECH_DISCONNECT_ACK( hWnd, uMsg, wParam, lParam );
	}

#if !X2_DEV_TOOLS
#endif

	//{{AFX
	//switch( wParam )
	//{
	//case EGS_STATE_CHANGE_SERVER_SELECT_ACK:
	//	{
	//		return Handler_EGS_STATE_CHANGE_SERVER_SELECT_ACK( hWnd, uMsg, wParam, lParam );
	//	}
	//	break;
	//}
	//}}AFX

	return false;
}
//{{AFX
//bool CX2StateLoading::Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ()
//{
//	g_pData->GetServerProtocol()->SendID( EGS_STATE_CHANGE_SERVER_SELECT_REQ );
//	g_pMain->AddServerPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK , 500.0f );
//
//	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
//
//#ifndef HIDE_SYSTEM_MESSAGE
//
//	m_pDLGMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), L"서버 선택 화면으로 이동하고 \n있습니다.", this );
//
//#endif //HIDE_SYSTEM_MESSAGE
//
//	return true;
//}
//
//bool CX2StateLoading::Handler_EGS_STATE_CHANGE_SERVER_SELECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//	KSerBuffer* pBuff = (KSerBuffer*)lParam;
//	KEGS_STATE_CHANGE_SERVER_SELECT_ACK kEvent;
//	DeSerialize( pBuff, &kEvent );
//
//	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
//	if( g_pMain->DeleteServerPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK ) == true )
//	{
//		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
//		{
//#ifndef HIDE_SYSTEM_MESSAGE
//
//			g_pMain->CreateStateChangeDLG( L"서버 선택창으로 이동중입니다." );
//
//#endif // HIDE_SYSTEM_MESSAGE
//
//			g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, NULL, false );
//		}
//
//	}
//	return true;
//}
//}}AFX

void CX2StateLoading::SetLoadingGageBar( int iLoadingPercent )
{
	if( NULL == m_pDLGLoadingFront )
		return;

	CKTDGUIStatic* pStatic_Loading = m_pDLGLoadingFront->GetStatic_LUA( "First_Loading" );
	CKTDGUIControl::CPictureData* pPicture_OrangeBar = pStatic_Loading->GetPicture(0);
	const float MAGIC_GAGE_BAR_FULL_LENGTH = 1024;

	const float fNowBarLength = MAGIC_GAGE_BAR_FULL_LENGTH * (float)iLoadingPercent / 100.f;
	pPicture_OrangeBar->SetSizeX( fNowBarLength );

}


void CX2StateLoading::SetLoadingString( const WCHAR* szText )
{
	if( NULL == m_pDLGLoadingFront )
		return;

	CKTDGUIStatic* pStatic_Loading = m_pDLGLoadingFront->GetStatic_LUA( "First_Loading" );
	CKTDGUIControl::UIStringData* pString_Loading = pStatic_Loading->GetString(0);
	pString_Loading->msg = szText;
}

void CX2StateLoading::DropItemLoading()
{
//{{ robobeg : 2008-10-28
	//KLuaManager luaManager;
    KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//}} robobeg : 2008-10-28

	g_pKTDXApp->LoadAndDoMemory( &luaManager, L"DropItemManager.lua" );

	if( luaManager.BeginTable( "DROP_XFILE_LIST" ) == true )
	{
		int			index = 1;
		wstring		str;
		while( luaManager.GetValue( index, str ) == true )
		{
			g_pData->XMeshReady( str.c_str() );
			index++;
		}

		luaManager.EndTable();
	}
}







//#ifdef	ADD_SERVER_GROUP
//	bool CX2StateLoading::OpenScriptServerGroupFile()
//	{
//		string			strFileName;
//		SERVER_GROUP_ID eServerGroupID	= SGI_INVALID;
//		bool			bParsingOK		= false;
//
//		ConvertWCHARToChar( strFileName, g_pData->GetSavedServerGroupFileName() );
//
//		ConvertFileAnsiToUTF8( strFileName, strFileName );
//
//		KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//
//		KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
//		Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile_LocalFile( g_pData->GetSavedServerGroupFileName() );
//		if( Info != NULL )
//		{
//			if( true == g_pKTDXApp->LoadLuaTinker_LocalFile( g_pData->GetSavedServerGroupFileName().c_str() ) )
//			{
//				if( true == g_pKTDXApp->LoadAndDoMemory_LocalFile( &luaManager, g_pData->GetSavedServerGroupFileName().c_str() ) )
//				{
//					LUA_GET_VALUE_ENUM( luaManager, "SERVER_GROUP", 			eServerGroupID,			SERVER_GROUP_ID,		SGI_INVALID	);
//				}
//			}
//		}
//
//		switch ( eServerGroupID )
//		{
//		case SGI_SOLES:
//		case SGI_GAIA:
//			{
//				g_pInstanceData->SetServerGroupID( eServerGroupID );
//				bParsingOK = true;
//			}
//			break;
//
//		default:
//			{
//#ifdef RANDOM_SERVER
//				if( g_pMain->GetDefaultChannelServerIPIndex() == SGI_INVALID )
//				{
//					g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( (rand() % 2) ) );
//				}
//				else
//				{
//					g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( g_pMain->GetDefaultChannelServerIPIndex() ) );
//				}
//#else
//				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( g_pMain->GetDefaultChannelServerIPIndex() ) );
//#endif
//			}
//			break;
//
//		}
//
//		return bParsingOK;
//
//	}
//#endif
	bool CX2StateLoading::ConnectToChannelServer()
	{
		ASSERT( NULL != g_pData->GetServerProtocol() );

		if( g_pData->GetServerProtocol()->ConnectedToChannelServer( g_pMain->GetPickedChannelServer().m_kServerIP.c_str(), g_pMain->GetPickedChannelServer().m_usMasterPort ) == true )
		{
			//{{ 09.08. 태완 : 서버-클라 접속시 패킷 변경.
			//#ifdef SERV_KOG_OTP_VERIFY
			return Handler_ECH_VERIFY_ACCOUNT_REQ();
			// #else
			// 		return Handler_ECH_GET_CHANNEL_LIST_REQ();
			// #endif
			//}}
		}
		else
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_735 ), this );

			g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_CONNECT_FAILED );									// fix!! id 변경
			g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
		}

		return false;
	}

	bool CX2StateLoading::Handler_ECH_VERIFY_ACCOUNT_REQ()
	{
		KECH_VERIFY_ACCOUNT_REQ kPacket;
		kPacket.m_wstrID = g_pInstanceData->GetUserID();
		kPacket.m_wstrPassword = g_pInstanceData->GetUserPassword();


		kPacket.m_iChannelingCode = g_pInstanceData->GetChannelingCode();

		kPacket.m_bLogin = true;		// 해외에서 m_bLogin 사용안함
		kPacket.m_bIDVerified = g_pInstanceData->IsIDVerified();	// 변수명 명확하게 구성함


		if( g_pMain != NULL )
			kPacket.m_strMachineID = g_pMain->GetMachineId();



		g_pData->GetServerProtocol()->SendChPacket( ECH_VERIFY_ACCOUNT_REQ, kPacket );
		g_pMain->AddServerPacket( ECH_VERIFY_ACCOUNT_ACK );

		return true;
	}

	bool CX2StateLoading::Handler_ECH_VERIFY_ACCOUNT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
	{
		KSerBuffer* pBuff = (KSerBuffer*)lParam;
		KECH_VERIFY_ACCOUNT_ACK kEvent;
		DeSerialize( pBuff, &kEvent );

		if( g_pMain->DeleteServerPacket( ECH_VERIFY_ACCOUNT_ACK ) == true )
		{
			if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
			{

				//{{김준환 서버시간 받아오기
				g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->SetServerCurrentTime( kEvent.m_wstrCurrentTime );
				g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->MassFileMapping();

				// KOG_OTP 로 인증 성공하는 부분
				if( false == kEvent.m_wstrPassport.empty() )
				{
					g_pInstanceData->SetIDVerified(true);
					g_pInstanceData->SetUserID( kEvent.m_wstrUserID );
					g_pInstanceData->SetUserPassword( kEvent.m_wstrPassport );
					// Token 값은 메뉴얼 로그인(클라이언트 로그인) 시
					// 최초 성공 부분(퍼블리셔 인증 성공)에서 채워짐
				}

				// 첫 아이디 패스워드 입력 후에 접속을 끊을 필요가 없음
				// Handler_ECH_DISCONNECT_REQ();

				return true;
			}
		}

		return false;
	}

	bool CX2StateLoading::Handler_ECH_DISCONNECT_REQ()
	{

	    if(
    #ifdef  X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
                g_pData->GetServerProtocol() != NULL
    #else
                 g_pData->GetServerProtocol()->GetChUserProxy() != NULL
    #endif // X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
            )
	    {
    #ifdef  X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
            g_pData->GetServerProtocol()->ChUserProxy_SetIntendedDisconnect( true );
    #else
		    g_pData->GetServerProtocol()->GetChUserProxy()->SetIntendedDisconnect( true );
    #endif // X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
	    }


		g_pData->GetServerProtocol()->SendChID( ECH_DISCONNECT_REQ );
		g_pMain->AddServerPacket( ECH_DISCONNECT_ACK, 100.0f );


		return true;
	}


	bool CX2StateLoading::Handler_ECH_DISCONNECT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/ )
	{
		if( g_pMain->DeleteServerPacket( ECH_DISCONNECT_ACK ) == true )
		{
			g_pData->GetServerProtocol()->DisconnectFromChannelServer();
		}

		return true;
	}
