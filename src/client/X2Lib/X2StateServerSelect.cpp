#include "stdafx.h"
#include "./X2StateServerSelect.h"

bool CX2StateServerSelect::m_sbSelectedServerSet = false;
bool CX2StateServerSelect::m_bCanNotConenctToBusyServer = false;		// ??? ?????? ?????? ?? ?? ????? ????? ????? ??? ??????

bool CX2StateServerSelect::m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection = false;

#ifdef FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
const int TODAY_CREATE_UNIT_MAX_NUMBER = 500;	// ??? ???? ?????? ???? ??? ???? ( ?????, ????? ???? ??? ???? )
#else // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
const int TODAY_CREATE_UNIT_MAX_NUMBER = 8;	// ??? ???? ?????? ???? ??? ???? ( ?????, ????? ???? ??? ???? )
#endif // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED

#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
const float MAX_WAITING_TIME_RECEIVE_CHARACTER_LIST = 5.f;
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????



const int MAX_SERVERGROUP_SIZE = 5;


// dmlee 2009.03.11
// ?????? ????? ?????? ????? -> ??????? ???? -> ???????? -> ??? 100% -> ???? ????
// ???? ??? ????? ?????? ????? -> ??? 100% -> �????? ???? -> ??????????? -> ??????????? -> ???????? -> ????

// ??????? ???? ???? ??? ????
// connect_req( nexon passport?? ?????�?) -> verify_account -> port check -> state change server select req -> current time req -> Handler_EGS_SELECT_SERVER_SET_REQ -> Handler_EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ


// ????????? ?????? ??? �????? ?????? ???? ???? REQ?? ??????. �??????? ??????????? ??????? ?????? ???? ???? REQ?? ??????

float CHANNEL_CONGESTION_SCOPE_TEMP[] = {0.1f, 0.2f, 0.3f, 0.9f, };
// ?? ????? ????, ???? ???? ?? * CHANNEL_CONGESTION_SCOPE_TEMP[0] = ????
// ?? ????? ????, ???? ???? ?? * CHANNEL_CONGESTION_SCOPE_TEMP[1] = ????
// ?? ????? ????, ???? ???? ?? * CHANNEL_CONGESTION_SCOPE_TEMP[2] = ???
// ?? ????? ????, ???? ???? ?? * CHANNEL_CONGESTION_SCOPE_TEMP[3] = FULL

CX2StateServerSelect::CX2StateServerSelect(void) :
m_pDLGServerSelectBack(NULL),
m_pDLGCheckHack(NULL),
//m_vecUnitSlot;
m_pDLGUnitSelectBack(NULL),
m_pDLGUnitSelectFront(NULL),
m_pDLGDeleteUnitCheck(NULL),
m_pDLGLoadingState(NULL),
m_pDLGRenameMsgBox(NULL),
m_pDLGRenameConfirmMsgBox( NULL ),
m_pDLGRenameSuccessMsgBox( NULL ),
m_pDLGTutorialMsgBox(NULL),
m_pDLGGoToFirstDungeon(NULL),
m_pDLGRecommend(NULL),
m_pDLGRecommendSuccess(NULL),
//m_UnitButtonList;
//m_UnitViewerUIList;
m_pSelectUnit(NULL),
m_pSelectButton(NULL),
m_bUnitClicked(false),
m_fMouseSensitivity(0.01f),
m_fRemainRotY(0.0f),
m_bRotateReturn(true),
m_fRotSpeed(false),
m_pOveredButton(NULL),
m_pOveredName(NULL),
m_bOneTimeInit(true),
m_bStateChangingToUnitCreate(false),
m_bPlayDownAnim(false),
m_fDownAnimTime(0.0f),
m_MaxUnitNum(0),
m_bReserveEnterTutorial(false),
m_bCheckRecommend(false),
m_bCheckRecommendSuccess(false),
m_NowPage(1),
m_MaxPage(1),		// ???????? ??? ????
m_bPortCheckWait( false ),
m_fPortCheckWaitTime( 0.f ),
m_PortCheckRetryTime( 0 ),
m_bENX_USER_LOGIN_NOT( false ),
m_bEGS_VERIFY_ACCOUNT_ACK( false ),
m_bIsGameBang( false ),
m_bPortCheckReq( false ),
m_bEGS_VERIFY_ACCOUNT_REQ( false ),
m_fEGS_VERIFY_ACCOUNT_ACK_WaitTime( 0.f ),
m_bReceivedServerGroupList( false ),
m_pDLGServerGroupSelectBack(NULL),
//{{ //2012.02.28 lygan_?????? // ????? unitUID?? ??????? ????? ???? ??? ????? ??
m_iSelectUnitUID( 0 ),
m_bMaintainNowUnitButton( false ),
//}}
m_bReceivedChannelList( false ),
m_bSentEGS_CONNECT_REQ( false ),
m_bReceivedEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK( false ),
m_bReservedConnectionToChannelServer( false ),
m_bWaiting_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK( false ),
m_iTryConnectChannelID( -1 ),
m_bJustChangedNickName( false ),
m_NickNameChangeUnitUID( -1 ),
m_NickNameChangeUnitNickName( L"" ),
m_bRemoveUnitAfterReceiveNMSerialNum( false )
//{{ kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???
, m_pDLGChannelSelectBack( NULL )
, m_iChannelNum( 0 )
//{{ ????? : [2011/6/20] //        �?? ???? ???????? ????
, m_bSelectChannel( false )
//}} ????? : [2011/6/20] //        �?? ???? ???????? ????
//}}  kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???
, m_bEnableUnitSelectBySecondSecurity( false )
, m_bRecivedKeyMap( false )
, m_pDLGATLLimitCheckMsgBox (NULL)
, m_pUnitViewerUI ( NULL )
, m_iChannelNowPage ( 1 )
, m_iChannelMaxPage ( 1 )
, m_hStandEffect( INVALID_EFFECTSET_HANDLE )
, m_iCreateUnitTodayCount ( 0 )

, m_fFadeTitleControlValue ( 0.f )
, m_iReservedEntryPointServerChannelIndex ( -1 )
, m_bSend_EGS_SELECT_UNIT_REQ_For_Auto_Connection ( false )
, m_pSelectChannelButton ( NULL )
, m_bIsAbledStateCreateUnitButton ( true )
, m_pSound( NULL )
, m_bOrgSound( false )
, m_bOrgMusic( false )

, m_pDLGReConnectChannelServer ( NULL )
, m_bReservedSecurityPad ( false )

, m_bWaitingEGS_CONNECT_REQ_ChannelButtonUp ( false )

, m_bReservedHackingUserAgreeDlg ( false )


#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ???? ???? ????, 10?? ???? ????
, m_fElapsedTime_WaitingCharacterList ( 0.f )
#endif // FIX_REFORM_ENTRY_POINT_10TH

{
	m_bReConnectChannelServer = true;

	g_pMain->SoundReady( L"UI_MouseClick_01.ogg" );
	g_pMain->SoundReady( L"UI_MouseClick_Keypad.ogg" );
	g_pMain->SoundReady( L"UI_MouseOver_01.ogg" );
	g_pMain->SoundReady( L"UI_PopUp_Negative_01.ogg" );
	g_pMain->SoundReady( L"UI_PopUp_Positive_01.ogg" );

	m_pDLGUnitSelectBack = new CKTDGUIDialog( this, L"DLG_Unit_Select_Back_New.lua" );

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUnitSelectBack );


	m_pDLGServerSelectBack = new CKTDGUIDialog( this, L"DLG_UI_Server_Chanal_Selection_New.lua" );



	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGServerSelectBack );


	wstring wstrClientVersion = g_pMain->GetClientVersion();
	wstrClientVersion += L".";
	wstrClientVersion += g_pMain->GetSubClientVersion_LUA();
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDLGServerSelectBack->GetControl( L"Static_Client_Version" );
	if( pStatic != NULL )
	{
		pStatic->GetString(0)->msg = wstrClientVersion;
	}

	//g_pKTDXApp->GetDGManager()->GetCamera().Point( 0.f, 232.83f, -848.49f, 0, 100.f, 0 );

	//g_pMain->SetProjectionFov( 4.f );

	//GetServerSetDataReq();

	m_bOrgSound = g_pMain->GetGameOption().GetOptionList().m_bSound;
	m_bOrgMusic = g_pMain->GetGameOption().GetOptionList().m_bMusic;

	g_pMain->GetGameOption().SetSound( false );
	g_pMain->GetGameOption().SetMusic( false );

	g_pData->PlayLobbyBGM( L"Lobby.ogg", true );

	if ( g_pData->GetMessenger() != NULL )
	{
		//g_pData->GetMessenger()->ClearChatSessionList();
		g_pData->GetMessenger()->Reset();
	}





	if ( g_pMain->GetInformerManager() )
		g_pMain->GetInformerManager()->InvalidInfo();

	g_pMain->GetMemoryHolder()->ReleaseLocalMapDLG();

	//g_pData->DeleteCharacterRoom();
	g_pMain->GetMemoryHolder()->RelaseCommonDLG();

	m_pDLGUnitSelectFront = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Back_New.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUnitSelectFront );


	//????? ???? ?????? ??????, ??????? ????? ??? ???. ????????? ???? ??.
	if( NULL != m_pDLGUnitSelectFront )
	{
		CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectFront->GetControl(L"createUnit"));
		if( NULL != pStatic )
		{
			pStatic->SetShowEnable(true,true);
		}
	}





	if( NULL != g_pData->GetMyUser() )
	{
		m_MaxUnitNum	= g_pData->GetMyUser()->GetUserData().maxUnitCount;

		m_NowPage		= 1;
		const int SLOT_COUNT_FOR_CREATE_UNIT = 1;

		if( g_pData->GetMyUser()->GetUnitNum() < m_MaxUnitNum )
		{
			m_MaxPage		= (g_pData->GetMyUser()->GetUnitNum()+SLOT_COUNT_FOR_CREATE_UNIT+UNIT_SELECT_UI_MAX_UNIT_NUM-1) / UNIT_SELECT_UI_MAX_UNIT_NUM;
		}
		else
		{
			m_MaxPage		= (g_pData->GetMyUser()->GetUnitNum()+UNIT_SELECT_UI_MAX_UNIT_NUM-1) / UNIT_SELECT_UI_MAX_UNIT_NUM;
		}
	}


	//CreateUnitButton();

	if ( m_sbSelectedServerSet == true )
	{
		ClearBeforeUnit( m_pSelectUnit );

		Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ();
		Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ ();
		Handler_EGS_CHARACTER_LIST_REQ ();


		m_bReceivedEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK = true;

		if(m_pDLGServerGroupSelectBack != NULL)
			m_pDLGServerGroupSelectBack->SetShowEnable(false, false);
		m_pDLGServerSelectBack->SetShowEnable(false, false);
		m_pDLGUnitSelectFront->SetShowEnable(true, true);
		// oasis907 : ????? [2011.6.28] ???? ?????? ??? ??? ????????? 2?? ???? �????? ???????.
		m_bEnableUnitSelectBySecondSecurity = true;


	}
	m_pDLGShowBeforeConnect = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Before_Connect.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGShowBeforeConnect );


	if ( true == m_bCanNotConenctToBusyServer )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING ( STR_ID_29885 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
		m_bCanNotConenctToBusyServer = false;
	}
	if( NULL != g_pMain->GetTutorSystem() )
	{
		g_pMain->GetTutorSystem()->SetBeenUnitSelect( true );
	}

	if( true == g_pMain->GetIsExitingTutorial() )
	{
		switch( g_pMain->GetStateIDBeforeTutorial() )
		{
		case CX2Main::XS_SERVER_SELECT:
			{
				g_pMain->SetIsExitingTutorial( false );
			} break;
		}
	}

	if( true == g_pMain->GetIsPlayingTutorial() )
	{
		m_pDLGLoadingState = new CKTDGUIDialog( this, L"DLG_PVP_Game_Loading_State.lua" );
		m_pDLGLoadingState->SetColor( D3DXCOLOR(1.0f,1.0f,1.0f,1.0f) );
		// ???? ??? ???
		wstringstream wstmStaticName;
		wstmStaticName << L"Static_Loading_BG";
		wstmStaticName << g_pMain->GetTutorialRandomLoadingBG();
		m_pDLGLoadingState->GetControl( wstmStaticName.str().c_str() )->SetShow( true );

		m_pSelectUnit = g_pData->GetMyUser()->GetUnitByIndex( g_pData->GetMyUser()->GetUnitNum() - 1 );
		g_pMain->SetStateIDBeforeTutorial( g_pMain->GetNowStateID() );
		m_bCheckRecommend = true;
		EnterTutorial();
	}



	g_pData->ResetPartyManager(); // ???????�???? ?????? ???? ??? ???????? ????

	if( NULL != g_pData->GetMyUser() )
	{
		g_pData->GetMyUser()->SelectUnit( NULL );
	}

	//{{ kimhc // 2009-12-15 // ?????? ?�?????? �?? ??????
#ifdef EXTEND_SERVER_GROUP_MASK
#else
	if ( g_pInstanceData->GetServerGroupID() == SGI_INVALID )
    {
		//OpenScriptServerGroupFile();
        g_pInstanceData->OpenScriptServerGroupFile();
    }
#endif // EXTEND_SERVER_GROUP_MASK

	UpdateServerGroupListUI();
	UpdateServerSelectButton( g_pInstanceData->GetServerGroupID() );
	//}} kimhc // 2009-12-15 // ?????? ?�?????? �?? ??????


//#ifdef _SERVICE_
//	OpenGlobalURL( L"http://Elsword.nexon.com/Elsword/etc/gameinstall.aspx?Section=chceck5" );
//#endif

	//if ( g_pData->GetMyUser()->GetUserData().hackingUserType == CX2User::HUT_DISAGREE_HACK_USER )
	//{
	//	m_pDLGCheckHack = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300),
	//	L"??? ???????? ????? ?????? ???? ?????? ???, ???? ??? ?????? ???? ?????? ?????? ?? ?????? ????? ??? ?? ????�?? ???? ???? ????? ????? ?? ??????. ???? ??�??????",
	//	SSSUCM_HACK_USER_PROCESS_CHECK_OK, this, SSSUCM_HACK_USER_PROCESS_CHECK_CANCEL );
	//}
	//else if ( g_pData->GetMyUser()->GetUserData().hackingUserType == CX2User::HUT_AGREE_HACK_USER )
	//{
	//	g_pMain->UpdateProcessList();
	//	g_pMain->SendHackMail();
	//}



	g_pData->GetServerProtocol()->SendTRID( ETR_UNIT_DISCONNECT );



	// ????????? ???????? ?�??? ????? "????????" ????? ?????? ????? ??? ???? ??????????? ?????? ???????? ????.
	if( false == m_sbSelectedServerSet &&
		g_pData->GetServerProtocol()->IsConnected() == true )
	{
		m_bENX_USER_LOGIN_NOT		= false;
		m_bEGS_VERIFY_ACCOUNT_REQ	= false;
		m_bEGS_VERIFY_ACCOUNT_ACK	= false;
		m_bPortCheckReq				= false;

		m_bSentEGS_CONNECT_REQ 		= false;
		m_bReceivedChannelList 		= false;

		// ??????????? ?????? ???�?
		Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_REQ();
	}


	if( false == m_sbSelectedServerSet )
	{
		ConnectToChannelServer();
		SetShowServerStateButtonSet ( false, false );
	}
	else
	{
		SetShowServerStateButtonSet ( true, false );
	}



// ?????? title ???? ???
// #ifdef _SERVICE_
// 	WCHAR wTextWindow[_MAX_PATH] ;
// 	GetWindowText(g_pKTDXApp->GetHWND(),wTextWindow,_MAX_PATH);
// 	if( StrCmpW( wTextWindow, GET_STRING( STR_ID_1 ) ) != 0 )
// 	{
// 		// ?????? �??? ?????
// 		g_pMain->SendHackMail_DamageHistory(ANTI_HACK_STRING_MODIFY_TITLE  );
// 		//m_pKTDXApp->SetFindHacking( true );
// 	}
// #endif //_SERVIC

// robobeg : CheckProcess ?? ???? thread ???? ???? ?????? ???!!!, 2013-12-31
//#if 0 // ??? ?????? ?? ????? ?? ???????.
//#ifdef CHECKFILTER_IN_MAIN
//	if( g_pInstanceData->GetChangeHackList() == false )
//		g_pMain->GetCheckWindowInfo()->CheckProcess();
//#endif
//#endif


	g_pKTDXApp->SkipFrame();

	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
		g_pData->GetPlayGuide()->SetShowFirstEpicQuestGuide(false);



}

CX2StateServerSelect::~CX2StateServerSelect(void)
{
	//{{ kimhc // 2009-12-15 // ?????? ?�?????? �?? ??????
	SAFE_DELETE_DIALOG( m_pDLGChannelSelectBack );
	//}} kimhc // 2009-12-15 // ?????? ?�?????? �?? ??????

	SAFE_DELETE_DIALOG( m_pDLGServerGroupSelectBack );
	SAFE_DELETE_DIALOG( m_pDLGServerSelectBack );

	SAFE_DELETE_DIALOG( m_pDLGCheckHack );

	ClearUnitButton();

	SAFE_DELETE_DIALOG( m_pDLGUnitSelectBack );
	SAFE_DELETE_DIALOG( m_pDLGUnitSelectFront );

    SAFE_DELETE_DIALOG_HANDLE( m_pDLGLoadingState );


	SAFE_DELETE_DIALOG( m_pDLGRenameMsgBox );
	SAFE_DELETE_DIALOG( m_pDLGRenameConfirmMsgBox );
	SAFE_DELETE_DIALOG( m_pDLGRenameSuccessMsgBox );

	SAFE_DELETE_DIALOG( m_pDLGTutorialMsgBox );
	SAFE_DELETE_DIALOG( m_pDLGGoToFirstDungeon );
	SAFE_DELETE_DIALOG( m_pDLGRecommend );



	SAFE_DELETE_DIALOG ( m_pDLGReConnectChannelServer );


	SAFE_DELETE_DIALOG ( m_pDLGShowBeforeConnect );

	if ( NULL != g_pData->GetUIMajorXMeshPlayer() )
	{
		for( CKTDGXMeshPlayer::CXMeshInstanceHandle hStandMeshInst : m_vecStandMeshInst )
		{
			g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( hStandMeshInst );
		}
		m_vecStandMeshInst.clear();
	}
	if( NULL != g_pData->GetUIEffectSet() )
	{
		g_pData->GetUIEffectSet()->StopEffectSet( m_hStandEffect );
	}
	CX2UnitViewerUI::DeleteKTDGObject( m_pUnitViewerUI );
	m_pUnitViewerUI = NULL;

	ClearChannelButton ();

	if( NULL != m_pSound )
	{
		m_pSound->Stop();
		SAFE_CLOSE( m_pSound );
	}
}

HRESULT CX2StateServerSelect::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2State::OnFrameMove( fTime, fElapsedTime );

	g_pKTDXApp->GetDGManager()->GetCamera().UpdateCamera( fElapsedTime );

	OnFrameMove_GameServerConnect( fTime, fElapsedTime );

	if( true == m_bReceivedEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK &&
		true == m_bOneTimeInit )
	{
		m_bOneTimeInit = false;

		if( NULL != g_pData->GetMyUser() && 0 == g_pData->GetMyUser()->GetUnitNum() )
		{
			//{{ kimhc // 2009-12-23 // ???????? ?????? ???? ????? ??? ????
			//{{ ????? : [2011/6/20] //        �?? ???? ???????? ????
			if( false == m_bSelectChannel )
			{
				UnitSelectExit();
				m_bReceivedEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK = false;
				m_bOneTimeInit = true;
			}
			else
			{
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_701 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );

				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_CREATE_UNIT, NULL, false );
				m_bStateChangingToUnitCreate = true;
			}
			//}} ????? : [2011/6/20] //        �?? ???? ???????? ????
			//}} kimhc // 2009-12-23 // ???????? ?????? ???? ????? ??? ????

		}
	}
	UnitHandling( fTime, fElapsedTime );

	if ( m_pDLGUnitSelectFront != NULL )
	{
		if ( m_sbSelectedServerSet == true )
		{
			if(m_pDLGServerGroupSelectBack != NULL)
				m_pDLGServerGroupSelectBack->SetShowEnable(false, false);
			m_pDLGServerSelectBack->SetShowEnable(false, false);
			m_pDLGUnitSelectFront->SetShowEnable( true, true );
			//{{ kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???
			m_pDLGChannelSelectBack->SetShowEnable( false, false );
			//}}  kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???

		}
		else
		{
			if(m_pDLGServerGroupSelectBack != NULL)
				m_pDLGServerGroupSelectBack->SetShowEnable(true, true);

			{
				m_pDLGServerSelectBack->SetShowEnable(true, true);
				m_pDLGUnitSelectFront->SetShowEnable( false, false );
				//{{ kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???
				m_pDLGChannelSelectBack->SetShowEnable( true, true );

				SetShowServerStateButtonSet( false, false );
				SetBackgroundImageBySelectUnitType ( CX2Unit::UT_NONE );
				//}}  kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???

			}
		}

		CKTDGUIStatic * pMovieStatic = reinterpret_cast<CKTDGUIStatic*>( m_pDLGUnitSelectBack->GetControl( L"PromotionMovie" ) );

		if ( NULL != pMovieStatic )
		{

			CKTDGUIControl::CMovieData* pIntroMovieData = pMovieStatic->GetMovie( 0 );

			if ( NULL != pIntroMovieData )
			{
				if ( g_pMain->m_bPlayIntroMovie == true )
				{
					BVL_STATUS status;
					pIntroMovieData->m_Bvl.GetStatus( status );
					pIntroMovieData->SetLoop( false );

					if ( status == BVL_STATUS_READY )
					{
						pIntroMovieData->SetShow( false );
						pMovieStatic->SetShow( false );
						g_pMain->GetGameOption().SetSound( m_bOrgSound );
						g_pMain->GetGameOption().SetMusic( m_bOrgMusic );
						g_pMain->m_bPlayIntroMovie = false;

						ProcessIntroMovieEnd();		// ????? ???????? ?????? ?? �??????? ?? ??

					}
					else if( status == BVL_STATUS_ERROR )
					{
						pIntroMovieData->m_Bvl.Stop();
						pMovieStatic->SetShow( false );
						g_pMain->GetGameOption().SetSound( m_bOrgSound );
						g_pMain->GetGameOption().SetMusic( m_bOrgMusic );
						g_pMain->m_bPlayIntroMovie = false;
					}
				}
				else
				{
					pIntroMovieData->m_Bvl.Stop();
					pMovieStatic->SetShow( false );
					g_pMain->GetGameOption().SetSound( m_bOrgSound );
					g_pMain->GetGameOption().SetMusic( m_bOrgMusic );
				}
			}
		}
	}

	static bool bAutoStart = true;

	if ( bAutoStart == true && g_pMain->m_bAutoStart == true && NULL != g_pData->GetMyUser() )
	{
		bAutoStart = false;

		CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByIndex( g_pMain->m_AutoLoginUnitIndex );

		if( pUnit == NULL )
			return true;

		UnitButtonUp( pUnit );


		Handler_EGS_SELECT_UNIT_REQ( pUnit->GetUID(), pUnit->GetUnitData().m_Level );
	}

	//	g_pKTDXApp->GetDGManager()->GetCamera().UpdateCamera( fElapsedTime );





	if ( m_pOveredButton != NULL && m_pOveredName != NULL )
	{
		if ( m_pOveredButton->GetMouseOver() == false )
		{
			if ( m_pOveredButton->GetButtonControlState() != CKTDGUIButton::BCS_MOUSEDOWN )
			{
				//SetShowCharacterName( false );
				m_pOveredName->Move( m_pOveredName->GetOffsetPos(), D3DXCOLOR(0,0,0,0), 0.2f, true, false );
				m_pOveredButton = NULL;
				m_pOveredName = NULL;
			}
		}
	}

	//SetShowCharacterNameForSelectedUnit();



	if ( g_pData->GetMessenger() != NULL )
		g_pData->GetMessenger()->CheckFriendReq();




	//if ( m_bCheckPostItem == true )
	//{
	//	if ( g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGRecommendReward ) == false &&
	//		g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGTutorReward ) == false &&
	//		g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGGuestUserReward ) == false )
	//	{
	//		m_bCheckPostItem = false;
	//		Handler_EGS_STATE_CHANGE_VILLAGE_MAP_REQ();
	//	}
	//}

	if ( m_bCheckRecommendSuccess == true )
	{
		if ( g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGRecommendSuccess ) == false )
		{
			m_bCheckRecommendSuccess = false;
			//Handler_EGS_GET_POST_LETTER_LIST_REQ();
			Handler_EGS_STATE_CHANGE_VILLAGE_MAP_REQ();
		}
	}

	if ( NULL != g_pData->GetMyUser() )
	{
		// �?? ????�?????? ????? ???????? ????? ????
		if( false == IsChannelSelectMode() )
		{
			if( GET_KEY_STATE( GA_LEFT ) == TRUE )
			{
				if ( m_NowPage > 1 )
				{

					ClearUnitButton( false );
					m_NowPage--;
					ChangeUnitButtonInfo();
				}
			}
			else if( GET_KEY_STATE( GA_RIGHT ) == TRUE )
			{
				if ( m_NowPage < m_MaxPage )
				{
					if( (m_NowPage * UNIT_SELECT_UI_MAX_UNIT_NUM) <= g_pData->GetMyUser()->GetUnitNum()+1 )
					{

						ClearUnitButton( false );
						m_NowPage++;
						ChangeUnitButtonInfo();
					}
				}
			}
		}
	}
	if ( NULL != m_pDLGUnitSelectBack )
	{
		CKTDGUIStatic * pFadeStatic = NULL;
		if ( NULL != m_pDLGUnitSelectBack->GetControl( L"FadeNotice1" ) && true == m_pDLGUnitSelectBack->GetControl( L"FadeNotice1" )->GetShow() )
		{
			pFadeStatic = static_cast<CKTDGUIStatic *> ( m_pDLGUnitSelectBack->GetControl( L"FadeNotice1" ) );
		}
		if ( NULL != m_pDLGUnitSelectBack->GetControl( L"FadeNotice2" ) && true == m_pDLGUnitSelectBack->GetControl( L"FadeNotice2" )->GetShow() )
		{
			pFadeStatic = static_cast<CKTDGUIStatic *> ( m_pDLGUnitSelectBack->GetControl( L"FadeNotice2" ) );
		}

		if ( NULL != pFadeStatic )
		{
			m_fFadeTitleControlValue += fElapsedTime / 3;

			if ( m_fFadeTitleControlValue > 2.f )
			{
				m_fFadeTitleControlValue = 0.f;
			}
			float m_fFadeTitlePercentage = m_fFadeTitleControlValue;
			if ( m_fFadeTitleControlValue > 1.f )
			{
				m_fFadeTitlePercentage = 2.f - m_fFadeTitleControlValue;
			}
			pFadeStatic->SetColor( D3DXCOLOR ( 1.f, 1.f, 1.f, m_fFadeTitlePercentage ) );
		}
		else
		{
			m_fFadeTitleControlValue = 0.f;
		}
	}

	if ( false == m_bIsAbledStateCreateUnitButton )
	{
		if ( true == m_pDLGUnitSelectFront->CheckControl( L"createUnit" ) )
		{
			CKTDGUIButton * pCreateUnitButton = static_cast<CKTDGUIButton *> ( m_pDLGUnitSelectFront->GetControl ( L"createUnit" ) );
			if ( NULL != pCreateUnitButton )
			{
				if ( pCreateUnitButton->GetShow () == true )
					pCreateUnitButton->ChangeState( CKTDGUIButton::BCS_DISABLE, true );
			}
		}
	}


#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????

	if ( NULL != m_pDLGWaitingCharacterList && true == m_pDLGWaitingCharacterList->GetShow() )
	{
		m_fElapsedTime_WaitingCharacterList += fElapsedTime;
		if ( m_fElapsedTime_WaitingCharacterList >= MAX_WAITING_TIME_RECEIVE_CHARACTER_LIST )
		{
			m_pDLGReConnectChannelServer = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ),  GET_STRING ( STR_ID_29884 ),
				SUSUCM_RE_CONNECT_CHANNEL_SERVER_OK, this, SUCM_EXIT, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
			SAFE_DELETE_DIALOG ( m_pDLGWaitingCharacterList );
		}
	}
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????


	return S_OK;
}



void CX2StateServerSelect::OnFrameMove_GameServerConnect( double fTime, float fElapsedTime )
{
	if ( m_bPortCheckReq == false &&
		g_pData->GetMyUser() != NULL &&
		m_bENX_USER_LOGIN_NOT == true &&
		m_bEGS_VERIFY_ACCOUNT_ACK == true )
	{
		m_bPortCheckReq = true;


		g_pData->GetMyUser()->SetPCRoom( m_bIsGameBang );

		if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
		{

#if X2_DEV_TOOLS
			g_pData->GetServerProtocol()->SendID( E_DISABLE_HB_CHECK_REQ );

			if( NULL == g_fpStream && NULL == g_fpStreamError )
			{
				AllocConsole();
				g_fpStream = freopen("CONOUT$", "a", stdout); // redirect printf to console
				g_fpStreamError = freopen("CONOUT$", "a", stderr); // redirect printf to console
				dbg::logfile::SetLogLevel(2);
			}
#endif
		}
		else
		{
#if X2_DEV_TOOLS
			FreeConsole();
			if ( g_fpStream != NULL )
				fclose( g_fpStream );
			if ( g_fpStreamError != NULL )
				fclose( g_fpStreamError );
#endif
		}


		Handler_KXPT_PORT_CHECK_REQ();
	}


	if ( m_bEGS_VERIFY_ACCOUNT_REQ == true && m_bEGS_VERIFY_ACCOUNT_ACK == false )
	{
		m_fEGS_VERIFY_ACCOUNT_ACK_WaitTime += fElapsedTime;
		if ( m_fEGS_VERIFY_ACCOUNT_ACK_WaitTime > 180.f )
		{
			//{{ kimhc // 2010-05-07 // ????�? ???? ???? ???? ????? ????
			// 	 int* x = NULL;
			// 	 *x = 1;
			g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
			//}} kimhc // 2010-05-07 // ????�? ???? ???? ???? ????? ????
		}
	}



	if( m_bPortCheckWait == true )
	{
		m_fPortCheckWaitTime -= fElapsedTime;
		if( m_fPortCheckWaitTime < 0.0f )
		{
			m_PortCheckRetryTime++;
			if( m_PortCheckRetryTime < 10 )
			{
				Handler_KXPT_PORT_CHECK_REQ();
			}
			else
			{
				m_bPortCheckWait			= false;
				m_fPortCheckWaitTime		= 0.0f;
				m_PortCheckRetryTime		= 0;

				if(
					g_pData->GetGameUDP()->IncreaseMyIPIndex() == true )
				{
					Handler_KXPT_PORT_CHECK_REQ();
				}
				else
				{
					//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"??? �? ?????? ???????", this );
					ErrorLogMsg( XEM_ERROR89, L"PortCheckFail" );


					KXPT_PORT_CHECK_ACK kXPT_PORT_CHECK_ACK;
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                    kXPT_PORT_CHECK_ACK.m_IPAddress	= g_pData->GetGameUDP()->GetMyIPAddress();
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//					kXPT_PORT_CHECK_ACK.m_IP	= g_pData->GetGameUDP()->GetMyIP();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
					kXPT_PORT_CHECK_ACK.m_Port	= g_pData->GetGameUDP()->GetMyPort();


					Handler_KXPT_PORT_CHECK_ACK( kXPT_PORT_CHECK_ACK );
				}
			}
		}
	}

	CKTDNUDP::RecvData* pRecvData = g_pData->GetGameUDP()->PopRecvData();
	while( pRecvData != NULL )
	{
		switch( pRecvData->m_ID )
		{
		//case XPT_CONNECT_SERVER_ACK:
		//	{
		//		//if( m_bLoadBalancerWait == true )
		//		//{
		//		//	KSerBuffer ksBuff;
		//		//	ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
		//		//	KXPT_CONNECT_SERVER_ACK kXPT_CONNECT_SERVER_ACK;
		//		//	DeSerialize( &ksBuff, &kXPT_CONNECT_SERVER_ACK );
		//		//	Handler_KXPT_CONNECT_SERVER_ACK( &kXPT_CONNECT_SERVER_ACK );
		//		//
		//		//}
		//	}
		//	break;

		case XPT_PORT_CHECK_ACK:
			{
				if( m_bPortCheckWait == true )
				{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                    if ( pRecvData->m_Size == sizeof(KXPT_PORT_CHECK_ACK) )
                    {
                        Handler_KXPT_PORT_CHECK_ACK( *( (const KXPT_PORT_CHECK_ACK*) pRecvData->m_pRecvBuffer ) );
                    }
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//					KSerBuffer ksBuff;
//					ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//					KXPT_PORT_CHECK_ACK kXPT_PORT_CHECK_ACK;
//					DeSerialize( &ksBuff, &kXPT_PORT_CHECK_ACK );
//					Handler_KXPT_PORT_CHECK_ACK( kXPT_PORT_CHECK_ACK );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
				}
			}
			break;
		}

		SAFE_DELETE( pRecvData );
		pRecvData = g_pData->GetGameUDP()->PopRecvData();
	}


}

HRESULT CX2StateServerSelect::OnFrameRender()
{
	if( true == m_bStateChangingToUnitCreate )
	{
		return S_OK;
	}

	CX2State::OnFrameRender();

	return S_OK;
}


bool CX2StateServerSelect::EnterTutorial()
{
	if( NULL != m_pSelectUnit )
	{
		m_bReserveEnterTutorial = true;

		m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection = true;
		m_bSend_EGS_SELECT_UNIT_REQ_For_Auto_Connection = true;
		m_bENX_USER_LOGIN_NOT		= false;
		m_bEGS_VERIFY_ACCOUNT_REQ	= false;
		m_bEGS_VERIFY_ACCOUNT_ACK	= false;
		m_bPortCheckReq				= false;

		m_bSentEGS_CONNECT_REQ 		= false;
		m_bReceivedChannelList 		= false;
		m_iReservedEntryPointServerChannelIndex = -2;

		Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_REQ();

		return true;
	}

	return false;
}


bool CX2StateServerSelect::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2State::UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	if( NULL != g_pMain->GetSecurityPad() )
	{
		if( true == g_pMain->GetSecurityPad()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if ( NULL != g_pData->GetPremiumBuffManager() )
	{
		if ( true == g_pData->GetPremiumBuffManager()->UICustomEventProc(hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if ( g_pMain != NULL && g_pMain->m_bPlayIntroMovie == true )
		return true;

	switch(wParam)
	{
		case SUSUCM_UNIT_BUTTON_OVER:
			{
				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;
				return false;
				//return AnimateCharacterName( pButton );
			}
			break;

		case SUSUCM_UNIT_BUTTON_UP:
			{
				if(m_bEnableUnitSelectBySecondSecurity == false)
				{
					return true;
				}

				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;

				if ( NULL == pButton )
					break;


				for ( UINT i = 0; i < m_vecUnitSlot.size(); ++i )
				{
					CKTDGUIDialogType pUnitSlot =  m_vecUnitSlot[i];

					if ( pUnitSlot == NULL )
						break;

					CKTDGUIButton* pCharButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 0 ) );

					if ( NULL != pCharButton )
						pCharButton->SetDownStateAtNormal( false );
				}

				pButton->SetDownStateAtNormal( true );

				wstring tempButtonName = pButton->GetName();
				tempButtonName = tempButtonName.substr( 12 );
				UidType unitUID = _wtoi64( tempButtonName.c_str() );
				CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( unitUID );



				if( pUnit == NULL )
					return true;

				UnitButtonUp( pUnit );

				//SetShowCharacterName( false );

				//SetShowCharacterNameForSelectedUnit();

			}
			break;

		case SUSUCM_BACK_TO_LOGIN:
			{

			}
			break;

		case SUSUCM_CREATE_UNIT:

			if(m_bEnableUnitSelectBySecondSecurity == false)
			{
				return true;
			}

			if ( m_sbSelectedServerSet == false )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_700 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				return true;
			}
#ifdef FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
			if ( 1 )
#else // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
			if ( g_pData->GetMyUser()->GetUnitNum() < m_MaxUnitNum )
#endif // FIX_REFORM_ENTRY_POINT_CREATE_UNIT_UNLIMITED
			{
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_701 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );
				//Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ();


				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_CREATE_UNIT, NULL, false );
			}
			else
			{

				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_702 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				//}
			}
			break;

		case SUSUCM_SELECT_UNIT:
			{

				if(m_bEnableUnitSelectBySecondSecurity == false)
				{
					return true;
				}


				if ( NULL != m_pSelectChannelButton )
				{
					vector< KChannelInfo > * vecChannelInfo = NULL;
					if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
					{
						vecChannelInfo = &m_vecGaiaChannelInfo;
					}
					else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
					{
						vecChannelInfo = &m_vecSolesChannelInfo;
					}
					else
					{
						ASSERT (!L"Can't Find Server Group ID !! ");
					}

					int iCheckEntryPointServerChannelIndex = m_pSelectChannelButton->GetDummyInt( 0 );

					if ( (*vecChannelInfo)[ iCheckEntryPointServerChannelIndex ].m_iCurrentUser >= (*vecChannelInfo)[ iCheckEntryPointServerChannelIndex ].m_iMaxUser )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING ( STR_ID_29885 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Negative_01.ogg" );
					}

					else
					{
						m_iReservedEntryPointServerChannelIndex = m_pSelectChannelButton->GetDummyInt( 0 );
						m_bReserveEnterTutorial = false;
						m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection = true;
						m_bSend_EGS_SELECT_UNIT_REQ_For_Auto_Connection = true;
						m_bENX_USER_LOGIN_NOT		= false;
						m_bEGS_VERIFY_ACCOUNT_REQ	= false;
						m_bEGS_VERIFY_ACCOUNT_ACK	= false;
						m_bPortCheckReq				= false;

						m_bSentEGS_CONNECT_REQ 		= false;
						m_bReceivedChannelList 		= false;

						// ??????????? ?????? ???�?
						Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_REQ();
					}

				}
				else
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING(STR_ID_29403), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Negative_01.ogg" );
				}

			}


			break;
		case SUSUCM_DELETE_UNIT:
			{
				if ( m_pSelectUnit != NULL )
				{
					//DeleteUnitReq();

					wstring tempNickName = m_pSelectUnit->GetNickName();
					if(tempNickName.length() != 0 )
					{
						if ( tempNickName.find( L"_") != -1 )
						{
							g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(260, 275), GET_STRING( STR_ID_713 ), this, -1, -1.f, L"DLG_UI_OKMsgBoxPlusNew.lua", D3DXVECTOR2 ( 0, -130 ),  L"UI_PopUp_Negative_01.ogg" );
							return false;
						}
						else if( m_pSelectUnit->GetUnitData().m_bDeleted == true )		// ???? ??? ????
						{
							g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(260, 275), GET_STRING( STR_ID_30402 ), this, -1, -1.f, L"DLG_UI_OKMsgBoxPlusNew.lua", D3DXVECTOR2 ( 0, -130 ),  L"UI_PopUp_Negative_01.ogg" );
							return false;
						}
						else
						{
							m_pDLGDeleteUnitCheck = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(305, 375), L"", SUSUCM_DELETE_UNIT_CHECK, this, -1, L"DLG_UI_Character_Selection_Delete_Unit_New.lua" );
						}
					}
				}
				else
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_704 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				}

			}

			break;

		case SUSUCM_DELETE_UNIT_CHECK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGDeleteUnitCheck, NULL, false );
				m_pDLGDeleteUnitCheck = NULL;

				//{{AFX
				return Handler_EGS_DELETE_UNIT_REQ();
				//}}AFX

			}
			break;

			//{{ 2012.02.20 ?????	????? ???? ??????? ???? (???? ??? ?? ????)
		case SUSUCM_FINAL_DELETE_UNIT:
			{
				if( m_pSelectUnit == NULL )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_704 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
					return true;
				}

				wstring tempEx = GET_REPLACED_STRING( ( STR_ID_16107, "SI", m_pSelectUnit->GetNickName(), KHanSungPostWordUnicodeWrapper( (WCHAR*) m_pSelectUnit->GetNickName(), STR_ID_198, STR_ID_199 ) ) );
				m_pDLGDeleteUnitCheck = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ), tempEx.c_str(), SUSUCM_FINAL_DELETE_UNIT_CHECK ,this, -1, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
			}
			break;
		case SUSUCM_FINAL_DELETE_UNIT_CHECK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGDeleteUnitCheck, NULL, false );
				m_pDLGDeleteUnitCheck = NULL;

				return Handler_EGS_FINAL_DELETE_UNIT_REQ();
			}
			break;
		case SUSUCM_RESTORE_UNIT:
			{
				if( m_pSelectUnit == NULL )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_705 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
					return true;
				}

				m_pDLGDeleteUnitCheck = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_16106 ), SUSUCM_RESTORE_UNIT_CHECK, this, -1, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
			}
			break;
		case SUSUCM_RESTORE_UNIT_CHECK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGDeleteUnitCheck, NULL, false );
				m_pDLGDeleteUnitCheck = NULL;

				return Handler_EGS_RESTORE_UNIT_REQ();
			}
			break;
			//}}

		case SUSUCM_UNIT_ANIMATION_DOWN:
			m_bUnitClicked = true;
			break;

		case SUSUCM_UNIT_ANIMATION_UP:
			{
				m_bUnitClicked = false;
			}
			break;

		case SUSUCM_UNIT_ANIMATION_OVER:
			break;

		case SUSUCM_TUTORIAL:
			{
				if( NULL != m_pSelectUnit )
				{
					g_pMain->SetStateIDBeforeTutorial( g_pMain->GetNowStateID() );
					EnterTutorial();
				}
				else
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_705 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

				}
			}
			break;

		case SUSUCM_RENAME_OK:
			{
				if( m_pSelectUnit != NULL && NULL != m_pDLGRenameMsgBox )
				{
					CKTDGUIIMEEditBox* pEditBox = (CKTDGUIIMEEditBox*) m_pDLGRenameMsgBox->GetControl( L"IME_NewNick" );
					if( NULL != pEditBox )
					{
						m_NickNameChangeUnitUID = m_pSelectUnit->GetUID();
						m_NickNameChangeUnitNickName = pEditBox->GetText();

						wstring wstrErrorMessage	= L"";
						if( true == CX2Main::IsInvalidNickName( m_NickNameChangeUnitNickName, wstrErrorMessage ) )
						{
							SetRenameMsgBoxError( wstrErrorMessage );
							return true;
						}
						Handler_EGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ ( m_NickNameChangeUnitUID, m_NickNameChangeUnitNickName.c_str(), true, m_pSelectUnit->GetServerGroupID() );
					}
				}
				return true;

			} break;

		case SUSUCM_RENAME_CANCEL:
			{
				OpenRenameMsgBox( false );
				return true;
			} break;

		case SUSUCM_RENAME_CONFIRM_OK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRenameConfirmMsgBox, NULL, false );
				m_pDLGRenameConfirmMsgBox = NULL;

				Handler_EGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ ( m_NickNameChangeUnitUID, m_NickNameChangeUnitNickName.c_str(), false, m_pSelectUnit->GetServerGroupID() );
				return true;
			} break;


		case SUSUCM_RENAME_CONFIRM_CANCEL:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRenameConfirmMsgBox, NULL, false );
				m_pDLGRenameConfirmMsgBox = NULL;
				return true;
			} break;


		case SUSUCM_RENAME_SUCCESS_OK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRenameSuccessMsgBox, NULL, false );
				m_pDLGRenameSuccessMsgBox = NULL;


				m_bJustChangedNickName = true;

				const int MAGIC_UNIT_LEVEL = -1;

				Handler_EGS_CHARACTER_LIST_REQ ();
				return true;
			} break;

		case SUSUCM_START_TUTORIAL_OK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGTutorialMsgBox, NULL, false );
				m_pDLGTutorialMsgBox = NULL;


				g_pMain->SetStateIDBeforeTutorial( g_pMain->GetNowStateID() );
				EnterTutorial();
				return true;
			} break;

		case SUSUCM_START_TUTORIAL_CANCEL:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGTutorialMsgBox, NULL, false );
				m_pDLGTutorialMsgBox = NULL;

				g_pMain->SetIsPlayingTutorial( false );

				m_pDLGGoToFirstDungeon = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_706 ),
					SUSUCM_START_FIRST_DUNGEON_OK, this, SUSUCM_START_FIRST_DUNGEON_CANCEL, L"DLG_UI_Selection_MessageBox_Ok_Exit_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Positive_01.ogg" );


				return true;
			} break;


		case SUSUCM_START_FIRST_DUNGEON_OK:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGGoToFirstDungeon, NULL, false );
				m_pDLGGoToFirstDungeon = NULL;

				if( NULL == m_pSelectUnit )
					return true;

				CX2StateAutoChanger::TARGET_DETAIL targetDetail;
				targetDetail.m_iChannelID = (int) 104; // fix!!! �???? ??? ??????
				targetDetail.m_iDungeonID = (int) SEnum::DI_EL_FOREST_GATE_NORMAL;
				targetDetail.m_iRoomUID = -1;

				g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), CX2Main::XS_DUNGEON_GAME, targetDetail, m_pSelectUnit->GetUID() );

				return true;
			} break;

		case SUSUCM_START_FIRST_DUNGEON_CANCEL:
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGGoToFirstDungeon, NULL, false );
				m_pDLGGoToFirstDungeon = NULL;


				if( NULL == m_pSelectUnit )
					return true;

				CX2StateAutoChanger::TARGET_DETAIL targetDetail;
				g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), CX2Main::XS_VILLAGE_MAP, targetDetail, m_pSelectUnit->GetUID() );

				return true;

			} break;

		case SUSUCM_RECOMMEND_OK:
			{
				//REQ????????
				CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*)m_pDLGRecommend->GetControl( L"g_pIMEEditBoxName" );
				if ( pIMEEditBox != NULL )
				{
					return Handler_EGS_RECOMMEND_USER_REQ( pIMEEditBox->GetText() );
				}
			}
			break;

		case SUSUCM_RECOMMEND_CANCEL:
			{
				//??? ??? ??????????.
				OpenRecommendMsgBox( false );
				//Handler_EGS_GET_POST_LETTER_LIST_REQ();

				return Handler_EGS_RECOMMEND_USER_REQ( L"", true );
			}
			break;



		case SUSUCM_PREV:
			{
				if ( m_NowPage > 1 )
				{

					ClearUnitButton( false );
					--m_NowPage;
					ChangeUnitButtonInfo();




				}
				//else
				//{
				//	ClearBeforeUnit( m_pSelectUnit );
				//	ClearUnitButton();

				//	m_NowPage = m_MaxPage;

				//	CreateUnitButton();
				//}
			}
			break;

		case SUSUCM_NEXT:
			{
				if ( m_NowPage < m_MaxPage )
				{
					if( (m_NowPage * UNIT_SELECT_UI_MAX_UNIT_NUM) <= g_pData->GetMyUser()->GetUnitNum() + 1)
					{

						ClearUnitButton( false );
						++m_NowPage;
						ChangeUnitButtonInfo();




					}
				}
				//else
				//{
				//	ClearBeforeUnit( m_pSelectUnit );
				//	ClearUnitButton();

				//	m_NowPage = 1;

				//	CreateUnitButton();
				//}
			}
			break;

		case SSSUCM_SERVER_CONNECT0:	// ???? ???? ????
			{
				// ??? ??????????? ?????? ???????? ?????? ?????? ????????? ???????? ??? ???????? ???.
				if( true == m_bWaiting_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK )
					return true;


				CKTDGUIButton* pButton = (CKTDGUIButton*)lParam;
				int iChannelIndex = pButton->GetDummyInt(0)-1;

				const KChannelInfo& channelInfo = g_pInstanceData->m_vecChannelInfo[ iChannelIndex ];
				m_iTryConnectChannelID = channelInfo.m_iChannelID;

				//{{ ????? : [2011/6/20] //        �?? ???? ???????? ????
				m_bSelectChannel = true;
				//}} ????? : [2011/6/20] //        �?? ???? ???????? ????

				return Handler_EGS_CONNECT_REQ( channelInfo.m_wstrIP, channelInfo.m_usMasterPort );

			} break;

		case SSSUCM_SELECT_SERVER_GROUP:  // �?? ???? ???? ?? ????
			{
				CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;							// fix!! ???? ??????? ???? ?????
				int iNewChannelServerIndex = pButton->GetDummyInt(0);

				//{{ kimhc // 2009-12-15 // ???? ??? ???
				// ?????? ???????? ??????? ????
#ifdef EXTEND_SERVER_GROUP_MASK
				if ( g_pInstanceData->GetServerGroupID() != iNewChannelServerIndex )
#else
				if ( g_pInstanceData->GetServerGroupID() != SERVER_GROUP_ID( iNewChannelServerIndex ) )
#endif // EXTEND_SERVER_GROUP_MASK
				{
#ifdef EXTEND_SERVER_GROUP_MASK
					g_pInstanceData->SetServerGroupID( iNewChannelServerIndex );
#else
					g_pInstanceData->SetServerGroupID( SERVER_GROUP_ID( iNewChannelServerIndex ) );
#endif // EXTEND_SERVER_GROUP_MASK
					UpdateServerSelectButton( g_pInstanceData->GetServerGroupID() );

					if(
#ifdef  X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
                        g_pData->GetServerProtocol() != NULL && g_pData->GetServerProtocol()->IsChUserProxyValid() == true
#else
                        g_pData->GetServerProtocol()->GetChUserProxy() != NULL
#endif // X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
                        )
					{
						// ?????? ????? �?? ???? ?????? ????
						// ???? ???�? �?? ?????? ???????.
						Handler_ECH_DISCONNECT_REQ();
						m_bReservedConnectionToChannelServer = true;
					}
					else
					{
						ConnectToChannelServer();
					}
				}
				//}} kimhc // 2009-12-15 // ???? ??? ???

				return true;

			} break;

			//??????? ??? ????.
		//case SSSUCM_SERVER_CONNECT1:
		//	{
		//		return Handler_EGS_SELECT_SERVER_SET_REQ();
		//	}
		//	break;

		//case SSSUCM_SERVER_CONNECT2:
		//	{
		//		return Handler_EGS_SELECT_SERVER_SET_REQ();
		//	}
		//	break;

		case SSSUCM_SERVER_SELECT_EXIT:
			{
				CX2State::QuitGame();
			}
			break;

		case SSSUCM_UNIT_SELECT_EXIT:
			{
				UnitSelectExit();
			}
			break;
		case SSSUCM_ATL_LIMIT_CHECK_OK:
			{
				ShellExecute(NULL, L"open", L"https://member.nexon.com/verify/realname.aspx", L"",L"", SW_SHOWNORMAL );
				if( NULL != m_pDLGATLLimitCheckMsgBox )
				{
					g_pKTDXApp->SendGameDlgMessage(XGM_DELETE_DIALOG, m_pDLGATLLimitCheckMsgBox, NULL, false);
				}
				g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, 0, 0, false );
			}
			break;




		case SSSUCM_UNIT_VIEWER_UP :
			{
				CX2UnitViewerUI::SetUnitClicked( false );
			}
			break;

		case SSSUCM_UNIT_VIEWER_DOWN :
			{
				CX2UnitViewerUI::SetUnitClicked( true );
			}
			break;
		case SSSUCM_UNIT_VIEWER_OVER:
			{
			}
			break;
		case SUSUCM_SELECT_CHANNEL_LIST_STEP:
			{
				if ( NULL != m_pSelectUnit )
				{
					wstring tempNickName = m_pSelectUnit->GetNickName();
					if ( tempNickName.find( GET_STRING(STR_ID_29404) ) != -1 )
					{
						//??????? ???? ?????? ??????? ???? ??????? ???�?? ????.
						OpenRenameMsgBox( true );
						SetRenameMsgBoxError( GET_STRING(STR_ID_29411) );
					}
					else
					{

						if( NULL != m_pSelectUnit && m_pSelectUnit->GetUnitData().m_bDeleted == true )
						{
							g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(260, 275), GET_STRING( STR_ID_30401 ), this, -1, -1.f, L"DLG_UI_OKMsgBoxPlusNew.lua", D3DXVECTOR2 ( 0, -130 ),  L"UI_PopUp_Negative_01.ogg" );
							break;
						}

						Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ();
						m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_READY );
						m_bPlayDownAnim = false;

	// 					SetShowServerStateButtonSet ( false, true );
	// 					ClearUnitButton ( false );
	// 					CreateChannelButton ( );

						if( m_pSelectUnit != NULL )
							PlaySoundSelect( m_pSelectUnit->GetType(), SPT_SERVER_SELECT );
	//					SAFE_DELETE_DIALOG ( m_pDLGMsgBox );
	//					m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), L"�?? ????? ?????? ?? ????.", this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
	//					m_bWaiting_EGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK = true;
	//					Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ();
					}
				}
				return true;
			}
			break;

		case SUSUCM_SECURITY_PAD:
			{
				if( NULL != g_pMain && NULL != g_pMain->GetSecurityPad() )
					g_pMain->GetSecurityPad()->SetShow(true, CX2SecurityPad::SPC_SELECT_STATE);
			}
			break;

		case SUSUCM_SELECT_UNIT_LIST_STEP:
			{
				MoveToCharacterSelectUI();
			}
			break;


		case SUSUCM_PREV_CHANNEL_LIST :
			{
				if ( m_iChannelNowPage > 1 )
				{
					ClearChannelButton();
					--m_iChannelNowPage;
					CreateChannelButton();
				}
			}
			break;

		case SUSUCM_NEXT_CHANNEL_LIST :
			{
				if ( m_iChannelNowPage < m_iChannelMaxPage )
				{
					ClearChannelButton();
					++m_iChannelNowPage;
					CreateChannelButton();
				}
			}
			break;

		case SUSUCM_CHANNEL_BUTTON_UP :
			{
				CKTDGUIButton* pButton = reinterpret_cast<CKTDGUIButton*> ( lParam );
				if ( NULL != pButton )
				{
					return ChannelButtonUp( pButton );
				}
			}
			break;

		case SUSUCM_SECURITY_PAD_CREATE :
			{
				if( NULL != g_pMain && NULL != g_pMain->GetSecurityPad() )
					g_pMain->GetSecurityPad()->SetShow(true, CX2SecurityPad::SPS_CREATE_PASSWORD );
			}
			break;

		case SUSUCM_RE_CONNECT_CHANNEL_SERVER_OK :
			{
				if ( g_pMain->GetIsPlayingTutorial() == true )
				{
					g_pMain->SetIsPlayingTutorial( false );
				}
				m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection = false;
				m_sbSelectedServerSet = false;
				if ( true == g_pData->GetServerProtocol()->IsConnected() )
					g_pData->GetServerProtocol()->DisconnectFromGameServer();
				if ( true == g_pData->GetServerProtocol()->IsChConnected() )
					g_pData->GetServerProtocol()->DisconnectFromChannelServer();
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, NULL, false );
			} break;

		case SSSUCM_DOUBLE_CONNECT:
			{
				g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_DOUBLE_CONNECT );
				g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
			}break;
	}

	return false;
}

bool CX2StateServerSelect::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2State::UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

    if( g_pMain->GetPostBox() != NULL)
    {
        if( g_pMain->GetPostBox()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
            return true;
    }

	if( g_pMain->GetSecurityPad() != NULL )
	{
		if( true == g_pMain->GetSecurityPad()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	switch( wParam )
	{
	case EGS_SELECT_UNIT_1_NOT:		/// ?????? ????, ?????, ???
		return Handler_EGS_SELECT_UNIT_1_NOT( hWnd, uMsg, wParam, lParam );
		break;
	case EGS_SELECT_UNIT_2_NOT:		/// ?????
		return Handler_EGS_SELECT_UNIT_2_NOT( hWnd, uMsg, wParam, lParam );
		break;
	case EGS_SELECT_UNIT_3_NOT:		/// �???, ??
		return Handler_EGS_SELECT_UNIT_3_NOT( hWnd, uMsg, wParam, lParam );
		break;
	case EGS_SELECT_UNIT_4_NOT:		/// ???? ????, ????, ????, ???
		return Handler_EGS_SELECT_UNIT_4_NOT( hWnd, uMsg, wParam, lParam );
		break;
	case EGS_SELECT_UNIT_5_NOT:		/// ETC, 1??? ????
		return Handler_EGS_SELECT_UNIT_5_NOT( hWnd, uMsg, wParam, lParam );
		break;
	case EGS_SELECT_UNIT_ACK:		/// m_iOK ??
		return Handler_EGS_SELECT_UNIT_ACK( hWnd, uMsg, wParam, lParam );
		break;

	case EGS_DELETE_UNIT_ACK:
		return Handler_EGS_DELETE_UNIT_ACK( hWnd, uMsg, wParam, lParam );
		break;


	case EGS_CHANGE_NICK_NAME_ACK:
		{
			return Handler_EGS_CHANGE_NICK_NAME_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_RECOMMEND_USER_ACK:
		{
			return Handler_EGS_RECOMMEND_USER_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	//case EGS_GET_POST_LETTER_LIST_ACK:
	//	{
	//		return Handler_EGS_GET_POST_LETTER_LIST_ACK( hWnd, uMsg, wParam, lParam );
	//	}
	//	break;

	// remove!!
	//case EGS_ITEM_EXPIRATION_NOT:
	//	{
	//		return Handler_EGS_ITEM_EXPIRATION_NOT( hWnd, uMsg, wParam, lParam );
	//	}
	//	break;



	case EGS_SELECT_SERVER_SET_ACK:
		return Handler_EGS_SELECT_SERVER_SET_ACK( hWnd, uMsg, wParam, lParam );
		break;

	case EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK:
		return Handler_EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK( hWnd, uMsg, wParam, lParam );
		break;

	case EGS_AGREE_HACK_USER_ACK:
		return Handler_EGS_AGREE_HACK_USER_ACK( hWnd, uMsg, wParam, lParam );
		break;

	case EGS_STATE_CHANGE_FIELD_ACK:
		return Handler_EGS_STATE_CHANGE_FIELD_ACK( hWnd, uMsg, wParam, lParam );
		break;

	case EGS_CHANGE_USER_LIST_IN_SECTOR_NOT:
		return Handler_EGS_CHANGE_USER_LIST_IN_SECTOR_NOT( hWnd, uMsg, wParam, lParam );
		break;

	case EGS_CREATE_TUTORIAL_ROOM_ACK:
		return Handler_EGS_CREATE_TUTORIAL_ROOM_ACK( hWnd, uMsg, wParam, lParam );
		break;

//#ifdef SERV_KOG_OTP_VERIFY -- 2009-08-31  ?????? : ???�????
	case ECH_VERIFY_ACCOUNT_ACK:
		return Handler_ECH_VERIFY_ACCOUNT_ACK(hWnd, uMsg, wParam, lParam);
		break;
//#endif


	case EGS_CONNECT_ACK:
		{
			return Handler_EGS_CONNECT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_VERIFY_ACCOUNT_ACK:
		{
			return Handler_EGS_VERIFY_ACCOUNT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case ENX_USER_LOGIN_NOT:
		{
			return Handler_ENX_USER_LOGIN_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CURRENT_TIME_ACK:
		{
			return Handler_EGS_CURRENT_TIME_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case ECH_GET_SERVERGROUP_LIST_ACK:
		{
			return Handler_ECH_GET_SERVERGROUP_LIST_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case ECH_GET_SERVERGROUP_LIST_NOT:
		{
			return Handler_ECH_GET_SERVERGROUP_LIST_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case ECH_GET_CHANNEL_LIST_ACK:
		{
			return Handler_ECH_GET_CHANNEL_LIST_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case ECH_GET_CHANNEL_LIST_NOT:
		{
#ifdef FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
			if ( true == Handler_ECH_GET_CHANNEL_LIST_NOT( hWnd, uMsg, wParam, lParam ) )
				return true;

			// m_bCanNotConenctToBusyServer = true;
			if ( true == g_pData->GetServerProtocol()->IsConnected() )
				g_pData->GetServerProtocol()->DisconnectFromGameServer();
			g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, NULL, false );
			return true;
#else //  FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
			return Handler_ECH_GET_CHANNEL_LIST_NOT( hWnd, uMsg, wParam, lParam );
#endif // FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
		} break;


	case ECH_DISCONNECT_ACK:
		{
			return Handler_ECH_DISCONNECT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_DISCONNECT_FOR_SERVER_SELECT_ACK:
		{
#ifdef FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
			if ( true == Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK( hWnd, uMsg, wParam, lParam ) )
				return true;

			m_bCanNotConenctToBusyServer = true;

			if ( true == g_pData->GetServerProtocol()->IsConnected() )
				g_pData->GetServerProtocol()->DisconnectFromGameServer();
			g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, NULL, false );
			return true;
#else //  FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
			return Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK( hWnd, uMsg, wParam, lParam );
#endif // FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????

		} break;

	case EGS_GET_MY_MESSENGER_SN_ACK:
		{
			return Handler_EGS_GET_MY_MESSENGER_SN_ACK( hWnd, uMsg, wParam, lParam );
		} break;

		//{{ kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????
	case EGS_SELECT_UNIT_INVENTORY_INFO_NOT:
		{
			return Handler_EGS_SELECT_UNIT_INVENTORY_INFO_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_GET_MY_INVENTORY_ACK:
		{
			return Handler_EGS_GET_MY_INVENTORY_ACK( hWnd, uMsg, wParam, lParam );
		} break;
		//}} kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????

	case EGS_SECOND_SECURITY_INFO_NOT:
		{
			return Handler_EGS_SECOND_SECURITY_INFO_NOT( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_KEYBOARD_MAPPING_INFO_NOT:
		{
			return Handler_EGS_KEYBOARD_MAPPING_INFO_NOT( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_CHAT_OPTION_INFO_NOT:
		{
			return Handler_EGS_CHAT_OPTION_INFO_NOT( hWnd, uMsg, wParam, lParam );
		} break;





	case EGS_CHARACTER_LIST_ACK :
		{
			return Handler_EGS_CHARACTER_LIST_ACK ( hWnd, uMsg, wParam, lParam);
		} break;

	case EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK :
		{
			Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK ( hWnd, uMsg, wParam, lParam);
		} break;

	case EGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK :
		{
			Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK ( hWnd, uMsg, wParam, lParam);
		} break;

		//{{ //2012.03.05 lygan_?????? // ??????? ??? ???????? ??? ???
	case EGS_FINAL_DELETE_UNIT_ACK:
		{
			return Handler_EGS_FINAL_DELETE_UNIT_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_RESTORE_UNIT_ACK:
		{
			return Handler_EGS_RESTORE_UNIT_ACK( hWnd, uMsg, wParam, lParam );
		} break;
		//}}
	}
	return false;
}



void CX2StateServerSelect::CreateChannelButton( bool bSelectTopChannelButton /* = false */ )
{
	CKTDGUIDialogType pChannelSlot = NULL;
	CKTDGUIButton* pButton = NULL;

	D3DXVECTOR3 dummyPos;

	ClearChannelButton();



	vector< KChannelInfo > * vecChannelInfo = NULL;
	if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
	{

		m_iChannelMaxPage = static_cast <int> ( m_vecGaiaChannelInfo.size() ) / CHANNEL_SELECT_UI_MAX_CHANNEL_NUM + 1;

		if ( static_cast <int> ( m_vecGaiaChannelInfo.size() ) > 0 )
		{
			if ( ( static_cast <int> ( m_vecGaiaChannelInfo.size() ) % CHANNEL_SELECT_UI_MAX_CHANNEL_NUM )  == 0 )
			{
				if ( m_iChannelMaxPage > 1 )
					m_iChannelMaxPage -= 1;
			}
		}
		vecChannelInfo = &m_vecGaiaChannelInfo;
	}
	else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
	{
		m_iChannelMaxPage = static_cast <int> ( m_vecSolesChannelInfo.size() ) / CHANNEL_SELECT_UI_MAX_CHANNEL_NUM + 1;

		if ( static_cast <int> ( m_vecSolesChannelInfo.size() ) > 0 )
		{
			if ( ( static_cast <int> ( m_vecSolesChannelInfo.size() ) % CHANNEL_SELECT_UI_MAX_CHANNEL_NUM ) == 0 )
			{
				if ( m_iChannelMaxPage > 1 )
					m_iChannelMaxPage -= 1;
			}
		}
		vecChannelInfo = &m_vecSolesChannelInfo;
	}
	else
	{
		ASSERT (! "Worng Server Group ID !! ");
		return ;
	}

	CKTDGUIStatic* pStaticUnitPage = ( CKTDGUIStatic* ) m_pDLGUnitSelectFront->GetControl( L"number" );
	WCHAR buff[256] = {0,};
	StringCchPrintf( buff, 256, L"%d / %d", m_iChannelNowPage, m_iChannelMaxPage );
	pStaticUnitPage->GetString(0)->msg = buff;


	int i = (m_iChannelNowPage - 1) * CHANNEL_SELECT_UI_MAX_CHANNEL_NUM;
	int j = 0;

	for( ; i < static_cast<int> ( vecChannelInfo->size() ) && j < CHANNEL_SELECT_UI_MAX_CHANNEL_NUM; i++, j++ )
	{
		const KChannelInfo& channelInfo = (*vecChannelInfo)[i];
		pChannelSlot = new CKTDGUIDialog( this, L"DLG_UI_Channel_Selection_Slot_New.lua" );

		dummyPos = m_pDLGUnitSelectFront->GetDummyPos( 8 + j );

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pChannelSlot );
		pChannelSlot->SetShowEnable(true, true);
		pChannelSlot->SetPos ( D3DXVECTOR2 ( dummyPos.x, dummyPos.y ) );

		wstring wstrChannelButtonName = g_pInstanceData->GetChannelButtonNameByChannelName ( channelInfo.m_wstrChannelName );

		CKTDGUIButton * pSelectChannelButton = static_cast<CKTDGUIButton *> ( pChannelSlot->GetControl( wstrChannelButtonName.c_str() ) );
		if ( NULL != pSelectChannelButton )
		{
			pSelectChannelButton->SetShowEnable( true, true );
			pSelectChannelButton->AddDummyInt(i);
		}

		float fChannelCongestionRate = static_cast< float >( channelInfo.m_iCurrentUser ) / static_cast< float >( channelInfo.m_iMaxUser );

		wstring wstrChannelCongestion = L"";
		if(fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[3])
		{
			wstrChannelCongestion += L"#Cff383f"; // FULL
			wstrChannelCongestion += GET_STRING(STR_ID_724);
		}
		else if(fChannelCongestionRate > CHANNEL_CONGESTION_SCOPE_TEMP[2])
		{
			wstrChannelCongestion += L"#Cf7941d"; // ???
			wstrChannelCongestion += GET_STRING(STR_ID_723);
		}
		else if(fChannelCongestionRate > CHANNEL_CONGESTION_SCOPE_TEMP[1])
		{
			wstrChannelCongestion += L"#Cfff200"; // ????
			wstrChannelCongestion += GET_STRING(STR_ID_722);
		}
		else if(fChannelCongestionRate > CHANNEL_CONGESTION_SCOPE_TEMP[0])
		{
			wstrChannelCongestion += L"#C93fe20"; // ???
			wstrChannelCongestion += GET_STRING(STR_ID_721);
		}
		else
		{
			wstrChannelCongestion += L"#C93fe20"; // ???
			wstrChannelCongestion += GET_STRING(STR_ID_721);
		}
		std::map<int, KChannelBonusInfo>::const_iterator mit;

		if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
		{
			mit = m_mapGaiaChannelBonusInfo.find(channelInfo.m_iChannelID);
			if ( mit == m_mapGaiaChannelBonusInfo.end() )
			{
				ASSERT ( !L"Can't Find Server Info !!");
				return ;
			}
		}
		else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
		{
			mit = m_mapSolesChannelBonusInfo.find(channelInfo.m_iChannelID);
			if ( mit == m_mapSolesChannelBonusInfo.end() )
			{
				ASSERT ( !L"Can't Find Server Info !!");
				return ;
			}
		}

		else
		{
			ASSERT ( !L"Wrong Server Group ID ! ");
			return ;
		}
		wstringstream wstrChannelButtonLevelInfo;
		wstrChannelButtonLevelInfo << L" (Lv." << mit->second.m_iBeginLv << L"~" << mit->second.m_iEndLv << L")";

		CKTDGUIStatic * pChannelInfoStatic = static_cast<CKTDGUIStatic *> ( pChannelSlot->GetControl( L"ChannelButtonInfo" ) );
		if ( NULL != pChannelInfoStatic )
		{
			pChannelInfoStatic->SetString ( 0, channelInfo.m_wstrChannelName.c_str() );
			pChannelInfoStatic->SetString ( 1, wstrChannelButtonLevelInfo.str().c_str() );
			pChannelInfoStatic->SetString ( 2, wstrChannelCongestion.c_str() );
		}

		m_vecChannelSlot.push_back(pChannelSlot);
		pChannelSlot->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

		if ( true == bSelectTopChannelButton )
		{
			if ( j == 0 )
			{
				ChannelButtonUp ( pSelectChannelButton );
			}
		}
	}
}


void CX2StateServerSelect::CreateUnitButton()
{
	//???? ?????? ??????.
	//?? ???? ?????? ???? UnitButton?? ???????.
	//??? ????? ???? ????????? ????.
	//??????? ???? ????? ????.
	CKTDGUIDialogType pUnitSlot = NULL;
	CKTDGUIButton* pButton = NULL;

	D3DXVECTOR3 dummyPos;
	bool bAllSlotOccupiedInAPage = false;	// ????? ?? ?? ??? ????? ????? ??? ?????? ??? ?????, ????? ?????? ????? ????? ?????????? ????? ?????


	for(UINT i=0; i<m_vecUnitSlot.size(); ++i)
	{
		CKTDGUIDialogType pUnitSlot =  m_vecUnitSlot[i];
		SAFE_DELETE_DIALOG(pUnitSlot);
	}
	m_vecUnitSlot.resize(0);

	//if(g_pData->GetMyUser()->GetUnitNum() > 0)
	//{
	//	m_MaxPage = ((g_pData->GetMyUser()->GetUnitNum()-1) / UNIT_SELECT_UI_MAX_UNIT_NUM) + 1;
	//}
	//else
	//{
	//	m_MaxPage = 1;
	//}

	CKTDGUIStatic* pStaticUnitPage = (CKTDGUIStatic*)m_pDLGUnitSelectFront->GetControl( L"number" );
	WCHAR buff[256] = {0,};
	//wsprintf( buff, L"%d / %d", m_NowPage, m_MaxPage );
	StringCchPrintf( buff, 256, L"%d / %d", m_NowPage, m_MaxPage );
	pStaticUnitPage->GetString(0)->msg = buff;


#ifdef CONVERSION_VS
	int i = (m_NowPage - 1)*UNIT_SELECT_UI_MAX_UNIT_NUM;
	int j = 0;
	for( ; i < g_pData->GetMyUser()->GetUnitNum() && j < UNIT_SELECT_UI_MAX_UNIT_NUM; i++, j++ )
#else
	for( int i = (m_NowPage - 1)*UNIT_SELECT_UI_MAX_UNIT_NUM, j = 0; i < g_pData->GetMyUser()->GetUnitNum() && j < UNIT_SELECT_UI_MAX_UNIT_NUM; i++, j++ )
#endif // CONVERSION_VS
	{
		CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByIndex( i );
		if( pUnit == NULL )
			continue;

		dummyPos = m_pDLGUnitSelectFront->GetDummyPos( j );

		pUnitSlot = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Slot_New.lua" );

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pUnitSlot );
		pUnitSlot->SetShowEnable(true, true);
		m_vecUnitSlot.push_back(pUnitSlot);

		CKTDGUIStatic* pStaticEmblem = (CKTDGUIStatic*)pUnitSlot->GetControl( L"UnitEmblem" );
		pStaticEmblem->GetPicture(0)->SetShow( true );

		if ( NULL != g_pMain->GetPVPEmblem() )
		{
		CX2PVPEmblem::PVPEmblemData* pPVPEmblemData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( pUnit->GetPvpRank() );
			if ( pPVPEmblemData != NULL )
			{
				pStaticEmblem->GetPicture(0)->SetTex( pPVPEmblemData->m_TextureName.c_str(), pPVPEmblemData->m_TextureKey.c_str() );
			}
		}

		pButton = (CKTDGUIButton*)pUnitSlot->GetControl( L"character1" );

		switch( pUnit->GetType() )
		{
		case CX2Unit::UT_ELSWORD:
		case CX2Unit::UT_LIRE:
		case CX2Unit::UT_ARME:
		case CX2Unit::UT_RAVEN:
		case CX2Unit::UT_EVE:
	//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
		case CX2Unit::UT_CHUNG:
	//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG
		case CX2Unit::UT_ARA:
		case CX2Unit::UT_ELESIS:

		case CX2Unit::UT_ADD:
			break;


		default:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_707 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
			}break;
		}
		XSkinMeshReady( pUnit->GetUnitTemplet()->m_MotionFile.c_str() );


		wstringstream buttonName;
		buttonName << L"SLOT_BUTTON_" << pUnit->GetUID();
		if( NULL != pButton )
		{
			pButton->SetName( buttonName.str().c_str() );
		}

		WCHAR buff[256] = {0,};
		//wsprintf( buff, L"%d", (int)pUnit->GetUnitData().m_Level );
		StringCchPrintf( buff, 256, L"%d", (int)pUnit->GetUnitData().m_Level );
		CKTDGUIStatic* pStaticLVNum = (CKTDGUIStatic*)pUnitSlot->GetControl( L"StaticUnitSelectStringLVNum" );
		pStaticLVNum->GetString(0)->msg = buff;





		CKTDGUIStatic* pStaticID = (CKTDGUIStatic*)pUnitSlot->GetControl( L"StaticUnitSelectStringID" );
		CKTDGUIStatic* pStaticChangeNickEmblem = (CKTDGUIStatic*) pUnitSlot->GetControl( L"ChangeNick" );
		CKTDGUIStatic* pStaticChangeNickMessage = (CKTDGUIStatic*) pUnitSlot->GetControl( L"standby" );

		wstring tempNickName = pUnit->GetNickName();
		if ( tempNickName.find( L"__DELETED__") != -1 )
		{
			pStaticID->GetString(0)->msg = GET_STRING(STR_ID_29404);

			pStaticChangeNickEmblem->SetShowEnable( true, true );
			pStaticChangeNickMessage->SetShowEnable( true, true );

		}
		else
		{
			pStaticID->GetString(0)->msg = pUnit->GetNickName();

			pStaticChangeNickEmblem->SetShowEnable( false, false );
			pStaticChangeNickMessage->SetShowEnable( false, false );
		}

		CKTDGUIStatic* pStaticLastPlace = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"StaticUnitSelectStringID_LastPlace" ) );
		wstring wstringLastPlace = L"??�??????????????????????????";



		wstringLastPlace = wstringLastPlace.substr(0, 11);
		wstringLastPlace += L"..";

		pStaticLastPlace->GetString(0)->msg = wstringLastPlace.c_str();

		CKTDGUIStatic* pStaticUnitFace = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"UnitFace" ) );

		wstring fileName, pieceName;
		if ( true == CX2Data::GetCharacterImageName( fileName, pieceName, pUnit->GetClass(), CX2Data::CIT_UnitSlot ) )
		{
			CKTDGUIControl::CPictureData* pPictureData = pUnitSlot->CreatePicture();
			pStaticUnitFace->AddPicture ( pPictureData );
			pPictureData->SetTex ( fileName.c_str(), pieceName.c_str() );
			CKTDGUIControl::UIPointData* pPointData = new CKTDGUIControl::UIPointData();
			pPointData->leftTopPoint = D3DXVECTOR2 ( 32, 8 );
			pPointData->color = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 1.0f );
			pPictureData->SetPoint ( pPointData );
			pPictureData->SetSize( D3DXVECTOR2 ( 38, 38 ) );
		}


		D3DXVECTOR2 buttonPos = D3DXVECTOR2( dummyPos.x, dummyPos.y );

		/*
		D3DXVECTOR2 buttonPos = D3DXVECTOR2( 0, 0 );
		if ( i == 0 )
		{
		buttonPos = D3DXVECTOR2( 378, 123 );
		}
		else if ( i == 1 )
		{
		buttonPos = D3DXVECTOR2( 586, 123 );
		}
		else if ( i == 2 )
		{
		buttonPos = D3DXVECTOR2( 795, 123 );
		}
		else if ( i == 3 )
		{
		buttonPos = D3DXVECTOR2( 378, 404 );
		}
		else if ( i == 4 )
		{
		buttonPos = D3DXVECTOR2( 586, 404 );
		}
		else if ( i == 5 )
		{
		buttonPos = D3DXVECTOR2( 795, 404 );
		}
		*/
		/*
		m_pDLGUnitSelectFront->OpenScriptFile( L"DLG_Unit_Select_Unit_Info.lua" );
		wstring tempName;
		tempName = pUnit->GetNickName();
		tempName += L"UnitSelectUnitInfo";

		CKTDGUIStatic* pStaticUnitInfo = (CKTDGUIStatic*)m_pDLGUnitSelectFront->GetControl( L"UnitSelectUnitInfo" );
		pStaticUnitInfo->SetName( tempName.c_str() );
		pStaticUnitInfo->GetString(0)->msg = pUnit->GetNickName();
		pStaticUnitInfo->GetString(1)->msg = L"??????????";

		WCHAR arLV[8] = {0,};
		//wsprintf(arLV, 7, L"%d", pUnit->GetUnitData().m_Level );
		StringCchPrintf( buff, 8, L"%d", pUnit->GetUnitData().m_Level );
		pStaticUnitInfo->GetString(2)->msg = arLV;
		*/
		//		buttonPos.x -= 12;
		//		buttonPos.y -= 13;

		//if( NULL != pButton )
		//	pButton->SetOffsetPos( buttonPos );
		//D3DXVECTOR2 staticPos( buttonPos.x + 5, buttonPos.y );

		if( pUnitSlot != NULL)
			pUnitSlot->SetPos(buttonPos);
		if ( j == UNIT_SELECT_UI_MAX_UNIT_NUM - 1 )
			bAllSlotOccupiedInAPage = true;
		//		pStaticUnitInfo->SetOffsetPos( staticPos );
// #ifdef REFORM_ENTRY_POINT	 	// 13-11-11, ???? ???? ????, kimjh
// 		if ( pUnit == m_pSelectUnit )
// 		{
// 			CKTDGUIButton* pCharButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 0 ) );
// 			pCharButton->SetDownStateAtNormal( true );
// 		}
//
// #endif // REFORM_ENTRY_POINT	// 13-11-11, ???? ???? ????, kimjh
	}



	dummyPos = m_pDLGUnitSelectFront->GetDummyPos( j );
	D3DXVECTOR2 buttonPos = D3DXVECTOR2( dummyPos.x, dummyPos.y );
	/*
	D3DXVECTOR2 buttonPos = D3DXVECTOR2( 0, 0 );
	if ( i == 0 )
	{
	buttonPos = D3DXVECTOR2( 378, 123 );
	}
	else if ( i == 1 )
	{
	buttonPos = D3DXVECTOR2( 586, 123 );
	}
	else if ( i == 2 )
	{
	buttonPos = D3DXVECTOR2( 795, 123 );
	}
	else if ( i == 3 )
	{
	buttonPos = D3DXVECTOR2( 378, 404 );
	}
	else if ( i == 4 )
	{
	buttonPos = D3DXVECTOR2( 586, 404 );
	}
	else if ( i == 5 )
	{
	buttonPos = D3DXVECTOR2( 795, 404 );
	}
	*/

	if( false == bAllSlotOccupiedInAPage &&
		g_pData->GetMyUser()->GetUnitNum() < m_MaxUnitNum )
	{
		if ( TODAY_CREATE_UNIT_MAX_NUMBER > m_iCreateUnitTodayCount )
			pUnitSlot = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Make_New.lua" );
		else
			pUnitSlot = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Dont_Make_New.lua" );

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pUnitSlot );
		pUnitSlot->SetShowEnable(true, true);
		m_vecUnitSlot.push_back(pUnitSlot);

		//m_pDLGUnitSelectBack->OpenScriptFile( L"DLG_UI_Character_Selection_Slot.lua" );
		pButton = (CKTDGUIButton*)pUnitSlot->GetControl( L"character2" );
		pButton->SetName( L"EmptySlot" );
		pUnitSlot->SetPos(buttonPos);

		for ( j = j + 1; j < UNIT_SELECT_UI_MAX_UNIT_NUM; j++ )
		{
			pUnitSlot = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Empty_Slot_New.lua" );
			dummyPos = m_pDLGUnitSelectFront->GetDummyPos( j );
			D3DXVECTOR2 buttonPos = D3DXVECTOR2( dummyPos.x, dummyPos.y );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pUnitSlot );
			pUnitSlot->SetShowEnable(true, true);
			m_vecUnitSlot.push_back(pUnitSlot);
			pUnitSlot->SetPos(buttonPos);
		}
	}

	PlayDownAnim();

}
bool CX2StateServerSelect::ClearUnitButton( bool bIsClearUnitViewer_ /* = true */ )
{
	for(UINT i=0; i<m_vecUnitSlot.size(); ++i)
	{
		CKTDGUIDialogType pUnitSlot =  m_vecUnitSlot[i];
		CKTDGUIButton* pSelectUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 0 ) );
		CKTDGUIButton* pCreateUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 1 ) );
		CKTDGUIButton* pNoCreateUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 2 ) );
		CKTDGUIButton* pEmptyUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 3 ) );
		CKTDGUIButton* pFullUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 4 ) );
		if ( NULL != pSelectUnitButton && NULL != pCreateUnitButton && NULL != pNoCreateUnitButton && NULL != pEmptyUnitButton && NULL != pFullUnitButton )
		{
			pSelectUnitButton->SetShowEnable( false, false );
			pSelectUnitButton->SetName ( L"ButtonSelectUnit");
			pCreateUnitButton->SetShowEnable( false, false );
			pNoCreateUnitButton->SetShowEnable( false, false );
			pEmptyUnitButton->SetShowEnable( false, false );
			pFullUnitButton->SetShowEnable( false, false );
		}

		CKTDGUIStatic* pStaticEmblem = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"UnitEmblem" ) );
		if ( NULL != pStaticEmblem )
			pStaticEmblem->SetShow ( false );

		CKTDGUIStatic* pStaticLV = (CKTDGUIStatic*)pUnitSlot->GetControl( L"lv" );
		if ( NULL != pStaticLV )
			pStaticLV->SetShow ( false );

		CKTDGUIStatic* pStaticLVNum = (CKTDGUIStatic*)pUnitSlot->GetControl( L"StaticUnitSelectStringLVNum" );
		if ( NULL != pStaticLVNum )
			pStaticLVNum->SetShow ( false );

		CKTDGUIStatic* pStaticID = (CKTDGUIStatic*)pUnitSlot->GetControl( L"StaticUnitSelectStringID" );
		if ( NULL != pStaticID )
			pStaticID->SetShow ( false );

		CKTDGUIStatic* pStaticLastPlace = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"StaticUnitSelectStringID_LastPlace" ) );
		if ( NULL != pStaticLastPlace )
			pStaticLastPlace->SetShow ( false );

		CKTDGUIStatic* pStaticUnitFace = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"UnitFace" ) );
		if ( NULL != pStaticUnitFace )
			pStaticUnitFace->SetShow ( false );

		CKTDGUIStatic* pStaticChangeNickEmblem = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"ChangeNick" ) );
		if ( NULL != pStaticChangeNickEmblem )
		{
			pStaticChangeNickEmblem->SetShowEnable ( false, false );
		}

		CKTDGUIStatic* pStaticChangeNickMessage = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"standby" ) );
		if ( NULL != pStaticChangeNickMessage )
		{
			pStaticChangeNickMessage->SetShowEnable ( false, false );
		}

	}


	if ( true == bIsClearUnitViewer_ )
	{
		ClearUnitViewerList();
	}

	m_iChannelNowPage = 1;
	m_iChannelMaxPage = 1;


	return true;
}

void CX2StateServerSelect::ClearBeforeUnit( CX2Unit* pUnit )
{
	if( pUnit == NULL )
		return;

	m_pSelectUnit = NULL;
}

void CX2StateServerSelect::UnitButtonUp( CX2Unit* pUnit )
{
	if ( m_pSelectUnit == pUnit )
	{
		wstring tempNickName = m_pSelectUnit->GetNickName();
		if ( tempNickName.find( GET_STRING(STR_ID_29404) ) != -1 ||
			 tempNickName.find( L"__DELETED__") != -1 )
		{
			//??????? ???? ?????? ??????? ???? ??????? ???�?? ????.
			OpenRenameMsgBox( true );
		}
		else
		{
			CX2Unit::UnitData* pUnitData = &(pUnit->AccessUnitData());
			if( pUnitData->m_bDeleted != true)
			{
				m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_2558 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );
				Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ();
				m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_READY );
				m_bPlayDownAnim = false;

				if( m_pSelectUnit != NULL )
					PlaySoundSelect( m_pSelectUnit->GetType(), SPT_SERVER_SELECT );
			}
			else
			{
				m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(260, 275), GET_STRING( STR_ID_30401 ), this, -1, -1.f, L"DLG_UI_OKMsgBoxPlusNew.lua", D3DXVECTOR2 (0, -130), L"UI_PopUp_Negative_01.ogg" );
			}
		}

		return;
	}
	ClearBeforeUnit( m_pSelectUnit );



	m_pSelectUnit = pUnit;

/*
	CKTDGUIStatic* pStaticCharBG = (CKTDGUIStatic*)m_pDLGUnitSelectBack->GetControl(L"CharBG" );
	if ( pStaticCharBG != NULL )
	{
		for ( int i = 0; i < CX2Unit::UT_END - 1; i++ )
		{
			if ( pStaticCharBG->GetPicture(i) != NULL )
				pStaticCharBG->GetPicture(i)->SetShow( false );
		}

		// note!! ???? ?????? ????, ??? unit_type????
		switch( pUnit->GetClass() )
		{
		case CX2Unit::UC_ELSWORD_SWORDMAN:
		case CX2Unit::UC_ELSWORD_KNIGHT:
		case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
			{
				if ( pStaticCharBG->GetPicture(0) != NULL )
					pStaticCharBG->GetPicture(0)->SetShow( true );
			}
			break;

		case CX2Unit::UC_LIRE_ELVEN_RANGER:
		case CX2Unit::UC_LIRE_COMBAT_RANGER:
		case CX2Unit::UC_LIRE_SNIPING_RANGER:
			{
				if ( pStaticCharBG->GetPicture(2) != NULL )
					pStaticCharBG->GetPicture(2)->SetShow( true );
			}
			break;

		case CX2Unit::UC_ARME_VIOLET_MAGE:
		case CX2Unit::UC_ARME_HIGH_MAGICIAN:
		case CX2Unit::UC_ARME_DARK_MAGICIAN:
			{
				if ( pStaticCharBG->GetPicture(1) != NULL )
					pStaticCharBG->GetPicture(1)->SetShow( true );
			}
			break;

		case CX2Unit::UC_RAVEN_FIGHTER:
			{
				if ( pStaticCharBG->GetPicture(3) != NULL )
					pStaticCharBG->GetPicture(3)->SetShow( true );
			}
			break;

		}
	}
*/

	CreateServerSelectUnitViewerUI ( pUnit );
	SetRightSideFlagColor ( GetUnitKeyColorByUnitType ( pUnit->GetType() ) );
	SetShowSelectUnitInfo ( pUnit );
	ChangeSelectUnitStandMeshInstance ( pUnit->GetType() );
	SetBackgroundImageBySelectUnitType ( pUnit->GetType() );
	PlaySoundSelect( pUnit->GetType(), SPT_UNIT_SELECT );

	/*
	D3DXVECTOR3 unitSize = D3DXVECTOR3( 1.6f, 1.6f, 1.6f );


	//	CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDLGUnitSelectBack->GetControl( L"StaticUnit_Select_UnitInfo" );
	//	pStatic->SetOffsetPos( D3DXVECTOR2( -200, 200 ) );
	//	pStatic->Move( D3DXVECTOR2( 0, 0 ), D3DXCOLOR(1,1,1,1), 0.1f, true );

	switch( pUnit->GetClass() )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
	//m_pDLGUnitSelectBack->OpenScriptFile( L"DLG_Unit_Select_ElSword_BG.lua" );

	//pStatic->GetPicture(0)->SetShow( true );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
	pUnitViewerUI->SetLightPos( 300, 300, -500 );	// ??????? ????? ??? ????
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( pUnit );
	m_pUnitViewerUI->GetMatrix().Move(  D3DXVECTOR3( -250, -70, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( unitSize );
	m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
	m_pUnitViewerUI->PlayAnim( L"LobbyFullDown", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );

	break;

	case CX2Unit::UC_LIRE_ELVEN_RANGER:
	//m_pDLGUnitSelectBack->OpenScriptFile( L"DLG_Unit_Select_Lire_BG.lua" );
	//pStatic->GetPicture(2)->SetShow( true );

	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
	pUnitViewerUI->SetLightPos( 300, 300, -500 );	// ??????? ????? ??? ????
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( pUnit );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( -250, -70, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( unitSize );
	m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
	m_pUnitViewerUI->PlayAnim( L"LobbyFullDown", CKTDGXSkinAnim::XAP_ONE_WAIT );
	m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
	break;

	case CX2Unit::UC_ARME_VIOLET_MAGE:
	//m_pDLGUnitSelectBack->OpenScriptFile( L"DLG_Unit_Select_Arme_BG.lua" );
	//pStatic->GetPicture(1)->SetShow( true );
	m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
	//m_pUnitViewerUI->SetLightPos( 1000, 1000, -200 );
	pUnitViewerUI->SetLightPos( 300, 300, -500 );	// ??????? ????? ??? ????
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

	m_pUnitViewerUI->SetUnit( pUnit );
	m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3( -250, -70, 200 ) );
	m_pUnitViewerUI->GetMatrix().Scale( unitSize );
	m_pUnitViewerUI->GetMatrix().Rotate(D3DXToRadian(0),D3DXToRadian(0),D3DXToRadian(0));
	m_pUnitViewerUI->PlayAnim( L"LobbyFullDown", CKTDGXSkinAnim::XAP_ONE_WAIT );

	m_pUnitViewerUI->GetXSkinAnim()->SetApplyMotionOffset( false );

	m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
	break;

	}




	g_pKTDXApp->SkipFrame();
	*/
}


bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_REQ( UidType unitUID, int iUnitLevel )

{

	if( false == g_pMain->GetIsValideCheckKomScript() )
	{
		g_pMain->SetQuitType( NetError::ERR_CLIENT_QUIT_03 );
		g_pMain->SendQuitMsgToServer();
	}


	if(m_bEnableUnitSelectBySecondSecurity == false)
	{
		return true;
	}

	// �?? ????? ????
	if( true == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser &&
		iUnitLevel >= 20 )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING( STR_ID_40 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
		return true;
	}

	KEGS_SELECT_UNIT_REQ	kPacket;
	kPacket.m_iUnitUID		= unitUID;

	g_pData->GetServerProtocol()->SendPacket( EGS_SELECT_UNIT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_SELECT_UNIT_ACK, 30.0f );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_709 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

	CX2EmblemManager::GetInstance()->InitEmbelmPlayInfo(true);

#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
	// ???? ???? ??, ??????? �???? ????? ???? ???.
	g_pInstanceData->ResetConnectedChannelID();
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????
	return true;
}


/// ?????? ????, ?????, ???
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_1_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_1_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	if( NULL != g_pMain && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
		battleFieldManager.SetBattleFieldIdWhereIam( 0 );

		if( NULL != g_pData && NULL != g_pData->GetMyUser() )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( kEvent.m_kUnitInfo.m_nUnitUID );

			if( NULL != pUnit )
			{
				g_pData->GetMyUser()->SortFirstByUID(pUnit->GetUID());

				g_pData->GetMyUser()->SelectUnit( pUnit );
				g_pData->GetItemStatCalculator().SetUnitData( pUnit->GetType(), pUnit->GetClass() );

				//{{ ?????? // 2012-04-06 // ???? ?????? ?? �??? ???? ??? ?? ????? ????
				pUnit->m_mapEventData.clear();
				pUnit->SetReceivedGuestUserReward( true );
				//}} ?????? // 2012-04-06 // ???? ?????? ?? �??? ???? ??? ?? ????? ????

				pUnit->Reset( kEvent.m_kUnitInfo, true );

				//{{ kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????
				pUnit->ResetInventorySize( kEvent.m_mapInventorySlotSize );
				pUnit->ResetInventoryItems( kEvent.m_mapItem ); // ???????? ??? ???? Reset
				//}} kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????

				pUnit->SetResurrectionStoneNum( kEvent.m_iNumResurrectionStone );

				//{{ ????? : [2009/10/14] //	??? ???? ?????
				//}} ????? : [2009/10/14] //	??? ???? ?????


				// ?? ?????? ??? ???? ?????? ???? ???? ????!!
				if ( NULL != g_pData->GetMessenger() )
				{
					g_pData->GetMessenger()->ResetRelationDummyUnit ();
				}

				CX2RelationshipManager * pRelationshipManager = g_pData->ResetRelationshipManager();
				CX2RelationshipEffectManager * pRelationshipEffectManager = g_pData->ResetRelationshipEffectManager();

				if ( NULL != pRelationshipManager )
				{
					CX2RelationshipManager::RelationshipInfo * pMyRelationshipInfo = pRelationshipManager->GetMyRelationshipInfo();
					if ( NULL != pMyRelationshipInfo )
					{
						pMyRelationshipInfo->ClearRelationInfo();
						pMyRelationshipInfo->m_eRelationStateType				= (SEnum::RELATIONSHIP_TYPE) kEvent.m_kRelationshipInfo.m_cRelationshipType;
						pMyRelationshipInfo->m_iRelationTargetUserUid			= kEvent.m_kRelationshipInfo.m_iOtherUnitUID;

						CTime ctCurrentTime( g_pData->GetServerCurrentTime() );
						CTime ctCoupleStartTime = static_cast<CTime> ( kEvent.m_kRelationshipInfo.m_tDate );
						CTimeSpan ctsGapTime = ctCurrentTime - ctCoupleStartTime;


						pMyRelationshipInfo->m_ctsDurationDay					= ctsGapTime;
						pMyRelationshipInfo->m_wstrNotifyLoveMessage			= kEvent.m_kRelationshipInfo.m_wstrLoveWord;
						pMyRelationshipInfo->m_wstrRelationTargetUserNickname	= kEvent.m_kRelationshipInfo.m_wstrOtherNickName;
					}
				}

				// ??? ???!!
				pRelationshipEffectManager->SetMyRelationEffect ( 1 );




				// skill list
				{
					pUnit->AccessUnitData().m_UserSkillTree.SetUsingPage(
						static_cast<USHORT>( kEvent.m_kUnitInfo.m_UnitSkillData.m_nActiveSkillPagesNumber ) );
					pUnit->AccessUnitData().m_UserSkillTree.SetOpenedPage(
						static_cast<USHORT>( kEvent.m_kUnitInfo.m_UnitSkillData.m_nTheNumberOfSkillPagesAvailable ) );
					pUnit->AccessUnitData().m_UserSkillTree.SetSelectPage(
						static_cast<USHORT>( kEvent.m_kUnitInfo.m_UnitSkillData.m_nActiveSkillPagesNumber ) );

					pUnit->AccessUnitData().m_UserSkillTree.SetAcquiredSkill( kEvent.m_vecUserSkillPageData );
					pUnit->AccessUnitData().m_UserSkillTree.SetUnsealedSkill( kEvent.m_vecSkillUnsealed );
					pUnit->AccessUnitData().m_UserSkillTree.SetEquippedSkill( kEvent.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill,
						kEvent.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB );

					if ( NULL != g_pData->GetRelationshipManager() &&
						NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
					{
						int iSkillLevel = static_cast<int>(g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType);
						pUnit->AccessUnitData().m_UserSkillTree.SetSkillLevelAndCSP( CX2SkillTree::SI_ETC_WS_COMMON_LOVE,
							iSkillLevel, 0, pUnit->AccessUnitData().m_UserSkillTree.GetUsingPage() );
					}
					pUnit->SetSkillNote( kEvent.m_mapSkillNote, kEvent.m_cSkillNoteMaxPageNum );
					pUnit->AccessUnitData().m_UserSkillTree.SetEqipSkillMemo( kEvent.m_kUnitInfo.m_UnitSkillData.m_vecSkillNote );

//#ifdef TITLE_SYSTEM
					pUnit->AccessUnitData().m_iTitleId = kEvent.m_kUnitInfo.m_iTitleID;
//#endif TITLE_SYSTEM
				}

				if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
				{
					g_pData->GetUIManager()->CreateUISkillTree();
					g_pData->GetUIManager()->GetUISkillTree()->InitSkillTreeUI();
				}

				//{{  [11/11/2009 : oasis907 ]

				// ???: ????????? GuildUserSkillTree ?????? ???? ????
				/*
				// skill list
				pUnit->AccessUnitData().m_UserSkillTree.SetGuildClass(0);
				pUnit->AccessUnitData().m_UserSkillTree.SetAcquiredGuildSkill( kEvent.m_vecSkillAcquired ); // ??? ??? ????
				*/

				//pUnit->AccessUnitData().m_GuildUserSkillTree.SetUnsealedSkill( kEvent.m_vecSkillUnsealed );


				// ??? ??? ????
				//pUnit->AccessUnitData().m_UserSkillTree.SetEquippedSkill( kEvent.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkill, kEvent.m_kUnitInfo.m_UnitSkillData.m_aEquippedSkillSlotB );

				//pUnit->AccessUnitData().m_GuildUserSkillTree.SetSkillSlotBExpirationState( (CX2GuildUserSkillTree::SKILL_SLOT_B_EXPIRATION_STATE) kEvent.m_kUnitInfo.m_UnitSkillData.m_cSkillSlotBExpirationState );
				//pUnit->AccessUnitData().m_GuildUserSkillTree.SetSkillSlotBEndDateString( kEvent.m_kUnitInfo.m_UnitSkillData.m_wstrSkillSlotBEndDate );

				/*
				if( g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree() != NULL)
				{
					g_pData->GetGuildManager()->GetUIGuild()->CreateUIGuildSkillTree();
					g_pData->GetGuildManager()->GetUIGuild()->GetUIGuildSkillTree()->InitSkillTreeUI();
				}
				//}}  [11/11/2009 : oasis907 ]
				*/

				pUnit->AccessUnitData().m_iTitleId = kEvent.m_kUnitInfo.m_iTitleID;

				//{{ kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????
				pUnit->ResetInventorySize( kEvent.m_mapInventorySlotSize );
				pUnit->ResetInventoryItems( kEvent.m_mapItem ); // ???????? ??? ???? Reset
				//}} kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????

				pUnit->SetResurrectionStoneNum( kEvent.m_iNumResurrectionStone );

				//{{ ????? : [2009/10/14] //	??? ???? ?????
				//}} ????? : [2009/10/14] //	??? ???? ?????

				pUnit->SetNewUnitTradeBlockDay( kEvent.m_iNewUnitTradeBlockDay );
				pUnit->SetNewUnitTradeBlockUnitClass( kEvent.m_iNewUnitTradeBlockUnitClass );
				pUnit->SetUnitCreateDate( kEvent.m_wstrUnitCreateDate );

				if( true == kEvent.m_bIsNewUnitTradeBlock )
				{
					CTime tReleaseTradeBlockTime;
					KncUtil::ConvertStringToCTime( kEvent.m_wstrUnitCreateDate, tReleaseTradeBlockTime );
					tReleaseTradeBlockTime += CTimeSpan( kEvent.m_iNewUnitTradeBlockDay, 0, 0, 0 );
					CTime tCurTime = g_pData->GetServerCurrentTime();

					if( kEvent.m_iNewUnitTradeBlockDay != 0 && tCurTime < tReleaseTradeBlockTime )
					{
						wstring wstrMonth = static_cast< const wchar_t * >( tReleaseTradeBlockTime.Format( _T("%m") ) );
						wstring wstrDay = static_cast< const wchar_t * >( tReleaseTradeBlockTime.Format( _T("%d") ) );
						wstring wstrHour = static_cast< const wchar_t * >( tReleaseTradeBlockTime.Format( _T("%H") ) );
						wstring wstrMinute = static_cast< const wchar_t * >( tReleaseTradeBlockTime.Format( _T("%M") ) );
						wstring wstrNotice = GET_REPLACED_STRING( ( STR_ID_14586 , "LLLL", wstrMonth, wstrDay , wstrHour , wstrMinute ) );

						pUnit->SetTradeBlockUnit( true );
						pUnit->SetTradeBlockChatMsg( wstrNotice );
					}
					else if( pUnit->GetNewUnitTradeBlockUnitClass() != 0
						&& pUnit->GetClass() < pUnit->GetNewUnitTradeBlockUnitClass() )
					{
						// ???? ??? ???????????.
						wstring wstrNotice =  GET_STRING( STR_ID_24523 );

						pUnit->SetTradeBlockUnit( true );
						pUnit->SetTradeBlockChatMsg( wstrNotice );
					}
				}
			}
		}

		if( g_pData->GetUIManager() != NULL )
		{
			g_pData->GetUIManager()->CreateUISkillTree();
			g_pData->GetUIManager()->GetUISkillTree()->InitSkillTreeUI();
		}


		//{{ kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????
		g_pData->GetServerProtocol()->SendID( EGS_GET_MY_INVENTORY_REQ );
		g_pMain->AddServerPacket( EGS_GET_MY_INVENTORY_ACK  );
		//}} kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????

		if ( NULL != g_pData &&
			 NULL != g_pData->GetMyUser() &&
			 NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			/// ??? ??? ???? ??? ????
			g_pData->GetMyUser()->GetSelectUnit()->ResetIncreaseSkillLevelBySocket();
		}

		return true;
		}

	return false;
}

/// ?????
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_2_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_2_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( NULL != g_pMain && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		//{{ kimhc // 2010.4.12 // ?????? ???(????????? ?�???)
		if( NULL != g_pInstanceData )
			g_pInstanceData->SetPreCheckTimeForDailyQuest( g_pData->GetServerCurrentTime() );
		//}} kimhc // 2010.4.12 // ?????? ???(????????? ?�???)

		if( NULL != g_pData && NULL != g_pData->GetQuestManager() )
			g_pData->GetQuestManager()->SetUnitQuest( kEvent.m_vecQuest, kEvent.m_vecCompletQuest );

		//{{ kimhc // GiveUpForbiddenQuest()?? EGS_GET_MY_INVENTORY_ACK?? ?????
		//}} kimhc // GiveUpForbiddenQuest()?? EGS_GET_MY_INVENTORY_ACK?? ?????

		// oasis907 : ????? [2010.7.21] // ??????? � ???? ????
		if(	g_pData != NULL && g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIQuestNew() != NULL)
			g_pData->GetUIManager()->GetUIQuestNew()->InitUserShowQuickQuestDLG(true);

		if( NULL != g_pData && NULL != g_pData->GetMyUser() )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
			if( NULL != pUnit && NULL != g_pMain && NULL != g_pMain->GetInformerManager() && NULL != g_pMain->GetLVUpEventMgr() )
			{
				g_pMain->GetInformerManager()->Reset();
				g_pMain->GetLVUpEventMgr()->Reset( pUnit->GetUnitData().m_Level );
			}

			if ( g_pData->GetCashShop() != NULL )
				g_pData->GetCashShop()->ResetUnitViewer( pUnit );

			g_pMain->ResetCrashReport( L"" );
		}

		if( NULL != g_pData && NULL != g_pData->GetQuestManager() )
			g_pData->GetQuestManager()->SetRandomQuestInfo( kEvent.m_mapRandomQuestList );

		if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuestNew() )
		{
			g_pData->GetUIManager()->GetUIQuestNew()->SetOpenEpicCategory();
			g_pData->GetUIManager()->GetUIQuestNew()->InitNewQuestList();
		}

		return true;
	}

	return false;
}

/// �???, ??
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_3_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_3_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{

		if( NULL != g_pData && NULL != g_pData->GetMyUser() )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();

			if( NULL != pUnit )
			{
//#ifdef TITLE_SYSTEM
				if(g_pData != NULL && g_pData->GetTitleManager() != NULL)
				{
					g_pData->GetTitleManager()->InitRecord();
					g_pData->GetTitleManager()->AttachTitle(pUnit->GetUnitData().m_iTitleId);

					g_pData->GetTitleManager()->TakeTitle(kEvent.m_vecTitle);
					g_pData->GetTitleManager()->UpdateMission(kEvent.m_vecMission);
				}
//#endif


				pUnit->SetSummonPetUid( kEvent.m_iSummonedPetUID );
				//if( g_pData != NULL && g_pData->GetPetManager() != NULL )
				//	g_pData->GetPetManager()->Handler_EGS_SUMMON_PET_REQ( kEvent.m_iSummonedPetUID );
				if( kEvent.m_vecPetList.size() > 0 )
					pUnit->SetFullPetInfo(kEvent.m_vecPetList[0]);

				if( g_pData->GetPetManager() != NULL )
				{
					g_pData->GetPetManager()->AddMyPetList( kEvent.m_vecCanEvolutionPetList );
					g_pData->GetPetManager()->AddNeverSummonPetList( kEvent.m_vecNeverSummonPetList, kEvent.m_iSummonedPetUID );

					// oasis907 : ????? [2010.9.7] //
					if( kEvent.m_vecPetList.size() > 0 )
					{
						KPetInfo kPetInfo = kEvent.m_vecPetList[0];
						int EvolutionStep = kPetInfo.m_cEvolutionStep;
						int iInvenSlotSize = g_pData->GetPetManager()->GetPetInventorySlotSize((CX2PetManager::PET_UNIT_ID) kPetInfo.m_iPetID, EvolutionStep);

						pUnit->AccessInventory().SetItemMaxNum(CX2Inventory::ST_PET, iInvenSlotSize);

						// oasis907 : ????? [2010.9.8] // ??? ???? ??? ???
						if( g_pData->GetUIManager()->GetUIInventory() != NULL)
						{
							g_pData->GetUIManager()->GetUIInventory()->SetAutoFeed(kPetInfo.m_bAutoFeed);
						}
						//}}
					}
					//}}
					else
					{
						g_pData->GetMyUser()->GetSelectUnit()->ClearPetInfo();
					}
				}

			}
		}

		return true;
	}

	return false;
}

/// ???? ????, ????, ????, ???
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_4_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_4_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( NULL != g_pMain && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		/// ?????
		if( NULL != g_pMain->GetTutorSystem() )
		{
			g_pMain->GetTutorSystem()->ResetTutorStudent( kEvent.m_vecTutorialDBUnitInfo );
		}

		if ( m_bReserveEnterTutorial == true )
		{
			g_pMain->SetIsPlayingTutorial( true );
		}

		if( NULL != g_pData && NULL != g_pData->GetMyUser() )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();

			if( NULL != pUnit )
			{
				/// ???? ?????
				pUnit->SetBlackList( kEvent.m_vecChatBlackList );

				CX2GageManager* pGageManager = CX2GageManager::GetInstance();

				if( NULL != pGageManager )
				{
					pGageManager->CreateMyGageUI( pUnit->GetClass(), pUnit->GetUID() );
					pGageManager->InitMySkillCoolTimeList( pUnit->GetUnitData().m_UserSkillTree );
					//pGageManager->InitMyQuickSlotCoolTimeList( pUnit->GetInventory().GetItemMaxNum( CX2Inventory::ST_E_QUICK_SLOT ) );


					ASSERT( !kEvent.m_kGamePlayStatus.m_vecGamePlayStatus.empty() );
					if ( !kEvent.m_kGamePlayStatus.m_vecGamePlayStatus.empty() )
					{
						pGageManager->SetMyPlayStatusFromPacket( kEvent.m_kGamePlayStatus.m_vecGamePlayStatus[0] );
						pGageManager->UpdateNowHpPercent();
						pGageManager->UpdateNowMpPercent();
					}

					pUnit->AccessUnitData().m_UserSkillTree.InitSkillCoolTimeFromGageManager( pGageManager->GetSkillCoolTimeList() );

					if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
						g_pData->GetUIManager()->GetUIQuickSlot()->InitMyQuickSlotCoolTimeFromGageManager( pGageManager->GetQuickSlotCoolTimeList() );
					//		const float fResultMaxMp = pUnit->ResetMaxMp( ONE_CHARGE * 3.0f );
					//		pGageManager->SetMaxMp( pUnit->GetUID(), fResultMaxMp );
					// GameStat???? ???? ??? ??, ????? ????? ???? ?? ??? ????...
					// 		pGageManager->SetMaxHp( pUnit->GetUID(), static_cast<const float>( kEvent.m_kUnitInfo.m_kStat.m_iBaseHP ) );
					// 		pGageManager->SetNowHp( pUnit->GetUID(), static_cast<const float>( kEvent.m_kUnitInfo.m_kStat.m_iBaseHP ) );
					// 		pGageManager->SetMaxSoul( pUnit->GetUID(), 100.0f );

					// ??????????? ????? ????? ????. -- ???? ?????? ??? ????
					//switch(kEvent.m_iSelectUnitEvent)
					//{
					//case 0:	// ????
					//	break;
					//case 1:	// ???????
					//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_2634 ), g_pMain->GetNowState() );
					//	break;
					//case 2:	// ?????? ???????? ????
					//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_2635 ), g_pMain->GetNowState() );
					//	break;
					//case 3:	// ???????? ????
					//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_2636 ), g_pMain->GetNowState() );
					//	break;
					//case 4: // ??? ??????
					//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_2640 ), g_pMain->GetNowState() );
					//	break;
					//default:
					//	break;
					//}
				}
			}
		}

		{
			g_pMain->GetGameOption().SetDenyInviteGuild( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyInviteGuild );
			g_pMain->GetGameOption().SetDenyPersonalTrade( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyPersonalTrade );
			g_pMain->GetGameOption().SetDenyFriendship( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyFriendShip );
			g_pMain->GetGameOption().SetDenyParty( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyParty );

			g_pMain->GetGameOption().SetRefuseParty( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyParty );
			g_pMain->GetGameOption().SetRefuseFriend( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyFriendShip );
			g_pMain->GetGameOption().SetRefusePersonalTrade( (CX2GameOption::BlackListDenyState) kEvent.m_kDenyOptions.m_cDenyPersonalTrade );
			//{{ kimhc // 2009-10-12 // ??? ??? ??? ???
			g_pMain->GetGameOption().SetRefuseGuildInvitation( static_cast< CX2GameOption::BlackListDenyState >( kEvent.m_kDenyOptions.m_cDenyInviteGuild ) );
			//}} kimhc // 2009-10-12 // ??? ??? ??? ???

			g_pMain->GetGameOption().SetRefuseRequestCouple( static_cast< CX2GameOption::BlackListDenyState >( kEvent.m_kDenyOptions.m_cDenyRequestCouple ) );

			g_pMain->GetGameOption().SetRefuseInvitePVP( static_cast< CX2GameOption::BlackListDenyState >( kEvent.m_kDenyOptions.m_cDenyInvitePracticePVP ) );

		}

		// ???�??� ?????? ???? ?? ???????? ??? ????
		if(g_pData != NULL && g_pData->GetMessenger() != NULL)
		{
			g_pData->GetMessenger()->ClearChatSessionList();
			g_pData->GetMessenger()->Reset();
			g_pData->GetMessenger()->ResetHistory();
			g_pData->GetMessenger()->SetFriendList( kEvent.m_kMessengerInfo );
		}

		if ( NULL != g_pData->GetRelationshipManager() &&
			NULL != g_pData->GetRelationshipManager()->GetUIRelationship() )
		{
			g_pData->GetRelationshipManager()->GetUIRelationship()->GetFriendListForInvitingWeddingUser();
		}


		return true;
	}

	return false;
}

/// ETC, 1??? ????
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_5_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_5_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( NULL != g_pMain && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{

		if( g_pInstanceData != NULL )
		{
			g_pInstanceData->SetPShopAgencyInfo( kEvent.m_kPShopAgencyInfo.m_bIsPShopOpen,
												 kEvent.m_kPShopAgencyInfo.m_wstrAgencyExpirationDate,
												 static_cast<SEnum::AGENCY_SHOP_TYPE>( kEvent.m_kPShopAgencyInfo.m_cShopType ) );
		}

		if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			g_pData->GetMyUser()->GetSelectUnit()->SetHenirRewardCountInfo( kEvent.m_PacketHenirRewardCount );


		//{{ kimhc // 2009-12-04 // ????????? ????
		//}} kimhc // 2009-12-04 // ????????? ????


		/// ??????? � ???????
		if ( 1 == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_Level &&
			0 == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_EXP )
		{
			m_bReserveEnterTutorial = true;
			g_pMain->SetIsPlayingTutorial( true );	/// ????? ????
		}



		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().SetCouple( kEvent.m_bCouple );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().SetRelationTargetUserNickname( kEvent.m_wstrRelationTargetUserNickname );
		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().SetRelationTargetUserUID( kEvent.m_iRelationTargetUserUid );


		return true;
	}

	return false;
}


/// ????? ???�? ????? ??????? ??? ???? ???? ?? m_iOK �??
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );


	if( NULL != g_pMain && false == g_pMain->DeleteServerPacket( EGS_SELECT_UNIT_ACK ) )
	{
		StateLog( L"invalid sel char ack" );
		return false;
	}


	if( NULL != g_pMain && g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{

		if( NULL != g_pData && NULL != g_pInstanceData && g_pInstanceData->GetServerGroupID() != g_pInstanceData->GetServerGroupIDScript() )
		{
			g_pData->LoadItemManager();
			g_pInstanceData->SetServerGroupIDScript( g_pInstanceData->GetServerGroupID() );
		}

		if( NULL != g_pMain->GetPostBox() )
			g_pMain->GetPostBox()->InitPostBox();

		if( NULL != g_pMain->GetPartyUI() && NULL != g_pMain->GetPartyUI()->GetLocalMapUI() )
		{
			g_pMain->GetPartyUI()->GetLocalMapUI()->ResetLocalMapID();
		}
		// ??????? ????????? ???? ???? ?????? ?????? ??? ????
		if( NULL != g_pData && NULL != g_pData->GetPartyManager() && NULL != g_pData->GetPartyManager()->GetMyPartyData() &&
			NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() &&
			NULL != g_pData->GetDungeonManager())
		{
			g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID = g_pData->GetMyUser()->GetSelectUnit()->GetLastClearDungeonID();

			// ??????? ?????? ????? ???? ??? ???? ???? ???? normal?? ?????????
			switch( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID )
			{
			case -1:
			case SEnum::DI_NONE:
			case SEnum::DI_EL_FOREST_GATE_NORMAL:
				//{{ kimhc // 2010-07-09 // ?????? ???????? ???
			case SEnum::DI_EL_FOREST_NORTH_NORMAL:
			case SEnum::DI_EL_FOREST_WEST_NORMAL:
			case SEnum::DI_EL_FOREST_HELL_NORMAL:
			case SEnum::DI_RUBEN_SECRET_COMMON:
				//}} kimc	// 2010-07-09 // ?????? ???????? ???
				{
					g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID = SEnum::DI_RUBEN_EL_TREE_NORMAL;
				} break;
			}

			//{{ kimhc // 2010.5.2 // ?????? ???(????????)
			switch ( ( g_pData->GetDungeonManager()->GetDungeonType( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ) )
			{
			case CX2Dungeon::DT_SECRET:
				{
					g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode = CX2Dungeon::DM_SECRET_NORMAL;
				} break;

			default:
				break;
			}
			//}} kimhc // 2010.5.2 // ?????? ???(????????)
		}

		if( NULL != g_pData && NULL != g_pData->GetMyUser() )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();

			if( NULL != pUnit )
			{
				wstringstream wstreamNickNameAndUid;
				wstreamNickNameAndUid << L"UserUID: " << pUnit->GetOwnerUserUID();
				wstreamNickNameAndUid << L" NickName: " << pUnit->GetNickName();
				wstreamNickNameAndUid << L" UnitUID: " << pUnit->GetUID();
				StateLog( wstreamNickNameAndUid.str().c_str() );

			}
		}

		Handler_EGS_CHECK_BALANCE_REQ();

		if ( g_pInstanceData != NULL )
		{
			g_pInstanceData->SetShowCSPandSlotBDate( true );
            g_pInstanceData->ReSetHackList_MainThread();
		}

		//{{ ????? : [2009/11/3] //	�?? ??? ????
		if( g_pChatBox != NULL )
		{
			g_pChatBox->ClearAllChatLog();
			g_pChatBox->ChangeChatWindowAndIMEEditBox( CX2ChatBox::CWM_TOTAL );
		}

		{
			g_pMain->GetGameOption().SetIsSkillUITypeA( g_pMain->GetGameOption().GetIsSkillUITypeA(), false );
		}

	}
	else if( kEvent.m_iOK == NetError::ERR_SELECT_UNIT_06 )
	{
		//??????? ???? ?????? ??????? ???? ??????? ???�?? ????.
		OpenRenameMsgBox( true );
	}
	//{{ 2009. 4. 29  ??????	????? ???�??? SELECT_UNIT_REQ?? ?????? ??????? ACK ???? ??? ???? �??
	else if( kEvent.m_iOK == NetError::ERR_WRONG_STATE_00 )
	{
		// ????�???
		return true;
	}
	//}} 2009. 4. 29  ??????
	else
	{
		if( true == g_pMain->GetStateAutoChanger().GetIsAutoChangingState() )
		{
			g_pMain->GetStateAutoChanger().EndStateChange();
		}
	}


	m_bJustChangedNickName = false;


	// kimhc // 2010-02-03 // PC???? ??? PC?? ???? ?????? ????? ????
	if ( g_pInstanceData->m_bFirstSelect == false &&
		 g_pData->GetMyUser() != NULL &&
		 g_pData->GetMyUser()->GetIsPCRoom() == true
		 && g_pData->GetPremiumBuffManager() != NULL
		 && g_pData->GetPremiumBuffManager()->IsPcBangAvatar() == true
		 )
	{
		if ( g_pData->GetUIManager() != NULL &&
			g_pData->GetUIManager()->GetUIInventory() != NULL )
			g_pData->GetUIManager()->GetUIInventory()->SetSortType( CX2Inventory::ST_PC_BANG );
	}

	g_pInstanceData->m_bFirstSelect = true;


	//{{ kimhc // 2009-01-12 // ????? ???????? ??�?? ????? ????? ???
	//}} kimhc // 2009-01-12 // ????? ???????? ??�?? ????? ????? ???


	//????? UI?? ???? ??? ?? ???? ????? ??? ??? ???? ???
	if( NULL != g_pData && NULL != g_pData->GetUIManager() )
		g_pData->GetUIManager()->CreatePetList();

	if( NULL != g_pData )
	{
		if( NULL != g_pData->GetUIManager() &&
			NULL != g_pData->GetUIManager()->GetUIPetList() )
			g_pData->GetUIManager()->GetUIPetList()->Handler_EGS_GET_RIDING_PET_LIST_REQ(0);
	}




	if ( NULL != g_pData &&
		 NULL != g_pData->GetMessenger() &&
		 NULL != g_pData->GetRelationshipManager() &&
		 NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
	{
		SEnum::RELATIONSHIP_TYPE eType = g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType;
		g_pData->GetMessenger()->SetRelationshipUI( eType );
	}



	g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID> ( m_pSelectUnit->GetServerGroupID() ) );
	g_pData->GetMyUser()->AccessUserData().maxUnitCount = g_pInstanceData->GetMaxUserUnitManyInServer( m_pSelectUnit->GetServerGroupID() );
	return true;
}

//bool CX2StateUnitSelect::SelectUnitTimeOutAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//SAFE_DELETE_DIALOG( m_pDLGMsgBox );
//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"?????? ?????? ???????\n?????? ?????? ?? ???????", this );
//return true;
//}

bool CX2StateServerSelect::Handler_EGS_DELETE_UNIT_REQ()
{
	if( m_pSelectUnit == NULL )
		return true;

	KEGS_ENTRY_POINT_DELETE_UNIT_REQ kPacket;
	kPacket.m_iUnitUID		= m_pSelectUnit->GetUID();
	kPacket.m_iServerGroup	= m_pSelectUnit->GetServerGroupID();
	g_pData->GetServerProtocol()->SendPacket( EGS_ENTRY_POINT_DELETE_UNIT_REQ, kPacket );

	g_pMain->AddServerPacket( EGS_DELETE_UNIT_ACK );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_712 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_DELETE_UNIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_DELETE_UNIT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_DELETE_UNIT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( kEvent.m_iUnitUID );
			if ( pUnit == NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_713 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );

				return true;
			}


			pUnit->AccessUnitData().m_bDeleted = true;
			pUnit->AccessUnitData().m_trDelAbleDate = kEvent.m_tDelAbleDate;

			CTime tDeleAbleDate(kEvent.m_tDelAbleDate);
			std::wstring wstrYear = (CStringW)( tDeleAbleDate.Format(_T( "%Y" )) );
			std::wstring wstrMonth = (CStringW)( tDeleAbleDate.Format(_T( "%m" )) );
			std::wstring wstrDay = (CStringW)( tDeleAbleDate.Format(_T( "%d" )) );
			std::wstring wstrHour = (CStringW)( tDeleAbleDate.Format(_T( "%H" )) );
			std::wstring wstrMinute = (CStringW)( tDeleAbleDate.Format(_T( "%M" )) );
			std::wstring wstrMessage = GET_REPLACED_STRING( ( STR_ID_16103, "SSSSS", wstrDay.c_str(), wstrMonth.c_str(), wstrYear.c_str(), wstrHour.c_str(), wstrMinute.c_str()));
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), wstrMessage.c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
			// ChangeUnitButtonInfo();

			if ( g_pData->GetMyUser()->GetUnitNum() <= 0 )
			{
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_701 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_CREATE_UNIT, NULL, false );
			}
			else
			{
				m_bMaintainNowUnitButton = true;
				m_iSelectUnitUID = pUnit->GetUID();		// ????? ??????? ???? ???? ???? m_pSelectUnit?? ???? ???? ?? ??????? ?????? ????

				Handler_EGS_CHARACTER_LIST_REQ();
			}
		}
		else if( kEvent.m_iOK == NetError::ERR_DELETE_UNIT_02)
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( kEvent.m_iUnitUID );
			if( pUnit == NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_713 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );

				return true;
			}

			pUnit->AccessUnitData().m_trDelAbleDate = kEvent.m_tReDelAbleDate;

			CTime tReDeleAbleDate(kEvent.m_tReDelAbleDate);
			std::wstring wstrYear = (CStringW)( tReDeleAbleDate.Format(_T( "%Y" )) );
			std::wstring wstrMonth = (CStringW)( tReDeleAbleDate.Format(_T( "%m" )) );
			std::wstring wstrDay = (CStringW)( tReDeleAbleDate.Format(_T( "%d" )) );
			std::wstring wstrHour = (CStringW)( tReDeleAbleDate.Format(_T( "%H" )) );
			std::wstring wstrMinute = (CStringW)( tReDeleAbleDate.Format(_T( "%M" )) );
			std::wstring wstrMessage = GET_REPLACED_STRING( ( STR_ID_16109, "SSSSS", wstrDay.c_str(), wstrMonth.c_str(), wstrYear.c_str(), wstrHour.c_str(), wstrMinute.c_str()));
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), wstrMessage.c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );
		}
	}
	return true;
}

//bool CX2StateUnitSelect::DeleteUnitTimeOutAck( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
//{
//SAFE_DELETE_DIALOG( m_pDLGMsgBox );
//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"?????? ?????? ???????\n?????? ???? ?? ???????.", this );
//
//return true;
//}


bool CX2StateServerSelect::Handler_EGS_STATE_CHANGE_VILLAGE_MAP_REQ()
{
	if ( g_pMain->GetIsPlayingTutorial() == true )
	{
		SAFE_DELETE_DIALOG( m_pDLGMsgBox );
		m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_716 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

		Handler_EGS_CREATE_TUTORIAL_ROOM_REQ();
	}
	else
	{
		SAFE_DELETE_DIALOG( m_pDLGMsgBox );
		m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_545 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

		Handler_EGS_STATE_CHANGE_FIELD_REQ();
	}

	return true;
}

bool CX2StateServerSelect::Handler_EGS_STATE_CHANGE_VILLAGE_MAP_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
//#if 0
//	KSerBuffer* pBuff = (KSerBuffer*)lParam;
//	KStateChangeAck kEvent;
//	DeSerialize( pBuff, &kEvent );
//
//	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
//	if( g_pMain->DeleteServerPacket( EGS_STATE_CHANGE_VILLAGE_MAP_ACK ) == true )
//	{
//		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
//		{
//			g_pMain->CreateStateChangeDLG( L"?????????? ?????????." );
//			g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_VILLAGE_MAP, kEvent.m_nMapID, false );
//		}
//	}
//#endif


	return true;
}






bool CX2StateServerSelect::Handler_EGS_CHANGE_NICK_NAME_REQ( UidType uid, const WCHAR* wszNickName, bool bCheckOnly )
{
	if( NULL == wszNickName )
		return false;

	KEGS_CHANGE_NICK_NAME_REQ kPacket;
	kPacket.m_iUnitUID		= uid;
	kPacket.m_wstrNickName	= wszNickName;
	kPacket.m_bCheckOnly	= bCheckOnly;

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_NICK_NAME_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CHANGE_NICK_NAME_ACK );

	return true;
}


bool CX2StateServerSelect::Handler_EGS_CHANGE_NICK_NAME_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_NICK_NAME_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_CHANGE_NICK_NAME_ACK ) == true )
	{
		m_bJustChangedNickName = false;



		if( true == kEvent.m_bCheckOnly )
		{
			switch( kEvent.m_iOK )
			{
			case NetError::ERR_RESTORE_NICK_NAME_00:
			case NetError::ERR_RESTORE_NICK_NAME_01:
			case NetError::ERR_RESTORE_NICK_NAME_02:
			case NetError::ERR_RESTORE_NICK_NAME_03:
			case NetError::ERR_RESTORE_NICK_NAME_04:
			case NetError::ERR_RESTORE_NICK_NAME_05:
			case NetError::ERR_RESTORE_NICK_NAME_06:
				{
					SetRenameMsgBoxError( NetError::GetErrStrF( kEvent.m_iOK ) );
					return true;
				} break;
			}


			if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
			{
				// ????? ???? ???? ??? ???. ??� ????? ??? ???� ????
				SAFE_DELETE_DIALOG( m_pDLGRenameConfirmMsgBox );

				m_pDLGRenameConfirmMsgBox = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(305, 375), GET_REPLACED_STRING( ( STR_ID_2556, "S", m_NickNameChangeUnitNickName.c_str() ) ), SUSUCM_RENAME_CONFIRM_OK, this, SUSUCM_RENAME_CONFIRM_CANCEL, L"DLG_UI_Selection_MessageBox_Ok_Exit_Button_New.lua", D3DXVECTOR2 ( 0, 0 ),  L"UI_PopUp_Positive_01.ogg" );
			}
		}
		else
		{
			if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
			{
				// ????? ???? ???. ????? ??????? ????????.
				OpenRenameMsgBox( false );

				m_pDLGRenameSuccessMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_REPLACED_STRING( ( STR_ID_2557, "S", kEvent.m_wstrNickName.c_str() ) ), this, SUSUCM_RENAME_SUCCESS_OK, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Positive_01.ogg" );

			}
		}
	}

	return true;
}

//{{ kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????
bool CX2StateServerSelect::Handler_EGS_SELECT_UNIT_INVENTORY_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_UNIT_INVENTORY_INFO_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	if ( g_pData == NULL || g_pData->GetMyUser() == NULL )
	{
		ASSERT( !L"g_pData or g_pData->GetMyUser In EGS_SELECT_UNIT_INVENTORY_INFO_NOT is WRONG" );
		return false;
	}

	CX2Unit* pMyUnit = g_pData->GetMyUser()->GetSelectUnit();

	if ( pMyUnit != NULL )
	{
		pMyUnit->AccessInventory().ResetItems( kEvent.m_mapItem );
		return true;
	}
	if( g_pData != NULL )
	{
		g_pData->ResetPlayGuide();
	}
	ASSERT( !L"pMyUnit or GetInventory In EGS_SELECT_UNIT_INVENTORY_INFO_NOT is WRONG" );
	return false;
}

bool CX2StateServerSelect::Handler_EGS_GET_MY_INVENTORY_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_MY_INVENTORY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	if( false == g_pMain->DeleteServerPacket( EGS_GET_MY_INVENTORY_ACK ) )
	{
		StateLog( L"Invalid EGS_GET_MY_INVENTORY_ACK" );
		return false;
	}

	if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		// �?? ????? ????
		if( false == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser &&
			kEvent.m_bIsRecommend == false && m_bCheckRecommend == true )
		{
			Handler_EGS_RECOMMEND_USER_REQ( L"", true );
		}
		else
		{
			if ( g_pMain->GetIsPlayingTutorial() == true )
			{
				// ????? ?????�
				Handler_EGS_CREATE_TUTORIAL_ROOM_REQ();
			}
			else
			{
				const CX2Unit::UnitData* pUnitData = 	&g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();

				if ( SEnum::VMI_BATTLE_FIELD_RUBEN_FIELD_01 <= pUnitData->m_nMapID &&
					 SEnum::VMI_BATTLE_FIELD_END > pUnitData->m_nMapID )
				{
					CX2GageManager* pGageManager = CX2GageManager::GetInstance();
					if ( pGageManager->GetMyGageData()->GetNowHp() > 0.0f )
						Handler_EGS_JOIN_BATTLE_FIELD_REQ( pUnitData->m_nMapID, pUnitData->m_ucLastTouchLineIndex, pUnitData->m_usLastPosValue  );
					else
					{
						CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
						CX2State::Handler_EGS_STATE_CHANGE_FIELD_REQ( battleFieldManager.GetReturnVillageId( pUnitData->m_nMapID ) );	// ???????? ?????�
					}
				}
				else
					Handler_EGS_STATE_CHANGE_FIELD_REQ();	// ???????? ?????�
			}
		}

		//{{ kimhc // GiveUpForbiddenQuest()?? EGS_SELECT_UNIT_ACK???? ?????
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
		//}} kimhc // GiveUpForbiddenQuest()?? EGS_SELECT_UNIT_ACK???? ?????

		return true;
	}

	return false;

}
//}} kimhc // 2010-01-04 // ?????? EGS_SELECT_UNIT_ACK?? ????????? ?????? ???? ?? ????


void CX2StateServerSelect::SetRenameMsgBoxError( const wstring& wstrErrorMessage )
{

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), wstrErrorMessage.c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
}


void CX2StateServerSelect::OpenRenameMsgBox( bool bOpen )
{
	if( true == bOpen )
	{
		if( NULL == m_pDLGRenameMsgBox )
		{

			m_pDLGRenameMsgBox = new CKTDGUIDialog( this, L"DLG_Change_Nickname_New.lua" );

			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGRenameMsgBox );
		}

		m_pDLGRenameMsgBox->SetShowEnable( true, true );
	}
	else
	{
		if( NULL != m_pDLGRenameMsgBox )
		{
			m_pDLGRenameMsgBox->SetShowEnable( false, false );
		}
	}
}


bool CX2StateServerSelect::Handler_EGS_RECOMMEND_USER_REQ( const WCHAR* wszNickName, bool bCancel )
{
	KEGS_RECOMMEND_USER_REQ kPacket;
	kPacket.m_wstrNickName = wszNickName;
	kPacket.m_wstrNickName = kPacket.m_wstrNickName.substr( 0, 20 );
	kPacket.m_bCancel = bCancel;

	if ( kPacket.m_bCancel == false && g_pMain->GetStringFilter()->CheckIsValidString( CX2StringFilter::FT_NICKNAME, kPacket.m_wstrNickName.c_str() ) == false )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_717 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
		return true;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_RECOMMEND_USER_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RECOMMEND_USER_ACK );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_RECOMMEND_USER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RECOMMEND_USER_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_RECOMMEND_USER_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			OpenRecommendMsgBox( false );
			g_pData->GetMyUser()->GetSelectUnit()->SetResurrectionStoneNum( g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum() + kEvent.m_iRewardResurrectionStone );
			if ( kEvent.m_iRewardResurrectionStone > 0 )
			{
				m_bCheckRecommendSuccess = true;
				wstringstream wstrstm;
				wstrstm << GET_REPLACED_STRING( ( STR_ID_718, "i", kEvent.m_iRewardResurrectionStone ) );

				m_pDLGRecommendSuccess = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), wstrstm.str().c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );
			}
			else
			{
				Handler_EGS_STATE_CHANGE_VILLAGE_MAP_REQ();
				//Handler_EGS_GET_POST_LETTER_LIST_REQ();
			}

			return true;
		}
	}

	return false;
}

void CX2StateServerSelect::OpenRecommendMsgBox( bool bOpen )
{
	if ( bOpen == true )
	{
		if ( m_pDLGRecommend != NULL )
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRecommend, NULL, false );

		m_pDLGRecommend = NULL;

		m_pDLGRecommend = new CKTDGUIDialog( this, L"DLG_Recommend.lua" );

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGRecommend );

		CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*)m_pDLGRecommend->GetControl( L"g_pIMEEditBoxName" );
		if ( pIMEEditBox != NULL )
		{
			pIMEEditBox->RequestFocus();
		}
	}
	else
	{
		if ( m_pDLGRecommend != NULL )
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRecommend, NULL, false );

		m_pDLGRecommend = NULL;
	}
}





bool CX2StateServerSelect::Handler_EGS_SELECT_SERVER_SET_REQ()
{
	if ( m_sbSelectedServerSet == true )
		return true;

	KEGS_SELECT_SERVER_SET_REQ kPacket;
	//kPacket.m_ServerSetID = iServerSetIndex;

	g_pData->GetServerProtocol()->SendPacket( EGS_SELECT_SERVER_SET_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_SELECT_SERVER_SET_ACK );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_SELECT_SERVER_SET_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SELECT_SERVER_SET_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	if( g_pMain->DeleteServerPacket( EGS_SELECT_SERVER_SET_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			m_sbSelectedServerSet = true;
			Handler_EGS_CHARACTER_LIST_REQ();

		}
		else
		{
			SAFE_DELETE_DIALOG( m_pDLGMsgBox );

			switch( g_pData->GetMyUser()->GetAuthLevel() )
			{
			case -2:
			case -3:
				//{{ 2010. 06. 30  ??????	???????????
			case -5:
			case -6:
				//}}
				//{{ ???? ???? ??? ???? ??? - ????
//#ifdef SERV_ADD_BLOCK_DAY
			case -7:
			case -8:
			case -9:
//#endif SERV_ADD_BLOCK_DAY
				//}}
				{
                    wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_719, "L", kEvent.m_kAccountBlockInfo.m_wstrEndTime ) );
					wstrMsg += kEvent.m_kAccountBlockInfo.m_wstrBlockReason;

					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), wstrMsg.c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				} break;

			case -4:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), kEvent.m_kAccountBlockInfo.m_wstrBlockReason.c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );


				} break;
			default:
				break;
			}
		}
	}

	// ???????? ?? �?? ???�? ????/�?????�?? ??????? ???????�?? ???�?.
	if(m_pDLGServerGroupSelectBack != NULL)
		m_pDLGServerGroupSelectBack->SetShowEnable(false, false);
	m_pDLGServerSelectBack->SetShowEnable(false, false);
	m_pDLGUnitSelectFront->SetShowEnable(true, true);

	//{{ kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???
	m_pDLGChannelSelectBack->SetShowEnable( false, false );
	//}}  kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???



	return true;
}

bool CX2StateServerSelect::Handler_EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ()
{
	/*
	KEVENT_X2_SERVER_UNIT_INFO_LIST_REQ	kEVENT_X2_SERVER_UNIT_INFO_LIST_REQ;
	kEVENT_X2_SERVER_UNIT_INFO_LIST_REQ.userUID = g_pData->GetMyUser()->GetUID();
*/

	g_pData->GetServerProtocol()->SendID( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_REQ );
	g_pMain->AddServerPacket( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_720 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Positive_01.ogg" );


	return true;
}

bool CX2StateServerSelect::Handler_EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	// ???? : ???? ???? ??????? ??? ????????.
	g_pData->GetMyUser()->SetSharingBankOpen(kEvent.m_bSharingBank);
	// ???? ??????? ????????? ??????? ???? ?????
	// 			if( NULL != g_pData->GetQuestManager() && g_pData->GetMyUser()->IsSharingBankOpen() == true )
	// 			{
	// 				g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	// 			}


	if( g_pMain->DeleteServerPacket( EGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			ClearBeforeUnit( m_pSelectUnit );
			ClearUnitButton();
			SetShowSelectUnitInfo( NULL );
			if ( NULL != g_pData->GetUIMajorXMeshPlayer() )
			{
				for( CKTDGXMeshPlayer::CXMeshInstanceHandle hStandMeshInst : m_vecStandMeshInst )
				{
					g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( hStandMeshInst );
				}
				m_vecStandMeshInst.clear();
			}
			if( NULL != g_pData->GetUIEffectSet() )
			{
				g_pData->GetUIEffectSet()->StopEffectSet( m_hStandEffect );
			}
			g_pData->GetMyUser()->DeleteAllUnit();

			g_pData->GetMyUser()->AccessUserData().maxUnitCount = kEvent.m_nUnitSlot; //???????? ???? ?????

			m_MaxUnitNum	= kEvent.m_nUnitSlot;

            for( int i =0; i < (int)kEvent.m_vecUnitInfo.size(); i++ )
			{

				KUnitInfo kUnitInfo = kEvent.m_vecUnitInfo[i];
				CX2Unit* pUnit = new CX2Unit( kUnitInfo );

				pUnit->SetOwnerUserUID( g_pData->GetMyUser()->GetUID() );
				wstring nickNameToChange = L"__DELETED__";

				if ( nickNameToChange == pUnit->GetUnitData().m_NickName )
				{
					wstringstream tempNickName;
					tempNickName << nickNameToChange.c_str() << i;
					pUnit->AccessUnitData().m_NickName = tempNickName.str().c_str();
				}
				g_pData->GetMyUser()->AddUnit( pUnit );
			}

			m_NowPage		= 1;
			const int SLOT_COUNT_FOR_CREATE_UNIT = 1;
			if( g_pData->GetMyUser()->GetUnitNum() < m_MaxUnitNum )
			{
				m_MaxPage		= (g_pData->GetMyUser()->GetUnitNum()+SLOT_COUNT_FOR_CREATE_UNIT+UNIT_SELECT_UI_MAX_UNIT_NUM-1) / UNIT_SELECT_UI_MAX_UNIT_NUM;
			}
			else
			{
				m_MaxPage		= (g_pData->GetMyUser()->GetUnitNum()+UNIT_SELECT_UI_MAX_UNIT_NUM-1) / UNIT_SELECT_UI_MAX_UNIT_NUM;
			}



			CreateUnitButtonNew();
			ClearUnitButton();
			ChangeUnitButtonInfo();
			int iTopCharacterIndex = (m_NowPage - 1) * UNIT_SELECT_UI_MAX_UNIT_NUM;
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByIndex( iTopCharacterIndex );
			if ( NULL != pUnit )
			{
				CreateServerSelectUnitViewerUI ( pUnit );
				SetFocusUnitButtonByIndex ( 0 );
				m_pSelectUnit = pUnit;
				SetRightSideFlagColor ( GetUnitKeyColorByUnitType ( m_pSelectUnit->GetType() ) );
				SetShowSelectUnitInfo ( pUnit );
				ChangeSelectUnitStandMeshInstance ( pUnit->GetType () );
				SetBackgroundImageBySelectUnitType ( pUnit->GetType() );
			}
			SetShowServerStateButtonSet ( true, false );

			m_bReceivedEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK = true;

		}
		else
		{
			g_pData->DeleteServerProtocol();
			g_pData->DeleteMyUser();
		}
	}

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	return true;
}




bool CX2StateServerSelect::Handler_EGS_AGREE_HACK_USER_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_AGREE_HACK_USER_REQ );
	g_pMain->AddServerPacket( EGS_AGREE_HACK_USER_ACK, 500.0f );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_AGREE_HACK_USER_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_AGREE_HACK_USER_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	if( g_pMain->DeleteServerPacket( EGS_AGREE_HACK_USER_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMyUser()->AccessUserData().hackingUserType = (CX2User::HACKING_USER_TYPE)kEvent.m_cHackingUserType;
			SAFE_DELETE_DIALOG( m_pDLGCheckHack );
		}
		else
		{
			g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, 0, 0, false );
		}
	}
	return true;
}
//{{ kimhc // 2009-12-15 // ?????? ?�?????? �?? ??????

void CX2StateServerSelect::UpdateChannelListUI()
{

	SAFE_DELETE_DIALOG( m_pDLGChannelSelectBack );

	int ServerGroupNum = g_pInstanceData->GetServerGroupID();
	m_pDLGChannelSelectBack = new CKTDGUIDialog( this, L"DLG_UI_Channel_Selection_New.lua" );


	if ( m_pDLGChannelSelectBack == NULL )
		return;

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGChannelSelectBack );

	// ???? ???? �?? ????? ????
	const int MAX_CHANNEL_COUNT = m_iChannelNum;	// �?? ????

	// �?? ????? ??? ????
	WCHAR wszControlName[32] = L"";

	// �?? ??? ??????
	CKTDGUIStatic* pStaticCHCongestionGage	= static_cast< CKTDGUIStatic* >( m_pDLGChannelSelectBack->GetControl( L"chanal_line" ) );
	// �?? ??? ???
	CKTDGUIStatic* pStaticCHBox				= static_cast< CKTDGUIStatic* >( m_pDLGChannelSelectBack->GetControl( L"chanal_state" ) );
	// �????? ex)??1
	CKTDGUIStatic* pStaticCHName			= static_cast< CKTDGUIStatic* >( m_pDLGChannelSelectBack->GetControl( L"chanal_num" ) );

	CKTDGUIStatic* pStaticCHBar			= static_cast< CKTDGUIStatic* >( m_pDLGChannelSelectBack->GetControl( L"chanal_bar" ) );

	// ??� �?? ?? ?????? ????? ????
	for ( int i = 0; i < MAX_CHANNEL_COUNT; ++i )
	{
		StringCchPrintf( wszControlName, ARRAY_SIZE(wszControlName), L"chanal%d", i + 1 );
		CKTDGUIButton *pChannelButton = static_cast< CKTDGUIButton* >( m_pDLGChannelSelectBack->GetControl( wszControlName ) );

		if( pChannelButton != NULL )
			pChannelButton->SetShowEnable(false, false);

		// �?? ??? ????? ex) ????, ???
		StringCchPrintf( wszControlName, ARRAY_SIZE(wszControlName), L"statechanal%d", i + 1);
		CKTDGUIStatic* pStaticCHCongestionStr = static_cast< CKTDGUIStatic* >( m_pDLGChannelSelectBack->GetControl( wszControlName ) );

		if(pStaticCHCongestionStr != NULL)
			pStaticCHCongestionStr->SetShowEnable(false, false);

		const int	iCntChannelGageMode = 4;
		for( int j=0; j< iCntChannelGageMode; ++j )
		{
            if ( pStaticCHCongestionGage != NULL &&
                pStaticCHCongestionGage->GetPicture( (i * iCntChannelGageMode) + j ) != NULL )
			    pStaticCHCongestionGage->GetPicture( (i * iCntChannelGageMode) + j )->SetShow(false);
		}

        if ( pStaticCHBox->GetPicture( i * 3 ) != NULL )
		    pStaticCHBox->GetPicture( i * 3 )->SetShow(false);
        if ( pStaticCHBox->GetPicture( i * 3 + 1 ) != NULL )
		    pStaticCHBox->GetPicture( i * 3 + 1 )->SetShow(false);
        if ( pStaticCHBox->GetPicture( i * 3 + 2 ) != NULL )
		    pStaticCHBox->GetPicture( i * 3 + 2 )->SetShow(false);

        if ( pStaticCHBar->GetPicture( i + 1 ) != NULL )
		    pStaticCHBar->GetPicture( i + 1 )->SetShow(false);
	}

	if( false == m_bReceivedChannelList )	// �???????? ???? ??????? ????
		return;

	if( g_pInstanceData == NULL || g_pInstanceData->m_vecChannelInfo.empty() == true)
		return;

	float			CHANNEL_CONGESTION_SCOPE_TEMP[] = { 0.1f,	0.2f,	0.5f,	0.99f, };
	const float		CHANNEL_CONGESTION_DISPLAY[]	= { 0.25f,	0.5f,	0.5f,	0.75f,	};

	// �?? ????? ???? ????
	for( int i = 0; i < static_cast< int >( g_pInstanceData->m_vecChannelInfo.size() )&& i < MAX_CHANNEL_COUNT; i++ )
	{


		StringCchPrintf( wszControlName, ARRAY_SIZE(wszControlName), L"chanal%d", i + 1 );
		CKTDGUIButton *pChannelButton = static_cast< CKTDGUIButton* >( m_pDLGChannelSelectBack->GetControl( wszControlName ) );

		// �?? ??? ????? ex) ????, ???
		StringCchPrintf( wszControlName, ARRAY_SIZE(wszControlName), L"statechanal%d", i + 1);
		CKTDGUIStatic* pStaticCHCongestionStr = static_cast< CKTDGUIStatic* >( m_pDLGChannelSelectBack->GetControl( wszControlName ) );
		// Show ????
		pChannelButton->SetShowEnable(true, true);
		pStaticCHCongestionStr->SetShowEnable(true, true);
		pStaticCHBox->GetPicture( i * 3 )->SetShow(true);
		pStaticCHBox->GetPicture( i * 3 + 1 )->SetShow(true);
		pStaticCHBox->GetPicture( i * 3 + 2 )->SetShow(true);

		pStaticCHBar->GetPicture( i + 1 )->SetShow(true);

		const KChannelInfo& channelInfo = g_pInstanceData->m_vecChannelInfo[i];
		float fChannelCongestionRate = static_cast< float >( channelInfo.m_iCurrentUser ) / static_cast< float >( channelInfo.m_iMaxUser );
		float fChannelCongestionRateDisplay = 0.f;

		if(fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[3])	// 99%~100%
		{
			// FULL ?????? , ?????? ???? 100%
			pStaticCHCongestionGage->GetPicture( i * 4 + 3 )->SetSizeX(pStaticCHCongestionGage->GetPicture(i*4+3)->GetOriginalSize().x );
			pStaticCHCongestionGage->GetPicture( i * 4 + 3 )->SetShow( true );
			pStaticCHCongestionStr->GetString( 0 )->msg = GET_STRING( STR_ID_724 );

			pChannelButton->SetShowEnable(true, false);
		}
		else if(fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[2]) // 50%~98%
		{
			// ??? ????? , ?????? ???? 75%
			fChannelCongestionRateDisplay = CHANNEL_CONGESTION_DISPLAY[3];
			pStaticCHCongestionGage->GetPicture( i * 4 + 2 )->SetSizeX(pStaticCHCongestionGage->GetPicture(i*4+2)->GetOriginalSize().x * fChannelCongestionRateDisplay );
			pStaticCHCongestionGage->GetPicture( i * 4 + 2 )->SetShow( true );
			pStaticCHCongestionStr->GetString( 0 )->msg = GET_STRING( STR_ID_723 );
		}
		else if(fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[1]) // 20%~49%
		{
			// ???? ??? , ?????? ???? 50%
			fChannelCongestionRateDisplay = CHANNEL_CONGESTION_DISPLAY[2];
			pStaticCHCongestionGage->GetPicture( i * 4 + 1 )->SetSizeX(pStaticCHCongestionGage->GetPicture(i*4+1)->GetOriginalSize().x * fChannelCongestionRateDisplay );
			pStaticCHCongestionGage->GetPicture( i * 4 + 1)->SetShow( true );
			pStaticCHCongestionStr->GetString( 0 )->msg = GET_STRING( STR_ID_722 );
		}
		else	//	0%~19%
		{
			// ??? ????? , ?????? ???? 25% ( ???? 10%???? ) , 50%
			if( fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[0] )	// 10%~19%
				fChannelCongestionRateDisplay = CHANNEL_CONGESTION_DISPLAY[1];
			else																// 0%~9%
				fChannelCongestionRateDisplay = CHANNEL_CONGESTION_DISPLAY[0];
			pStaticCHCongestionGage->GetPicture( i * 4 )->SetSizeX(pStaticCHCongestionGage->GetPicture(i*4)->GetOriginalSize().x * fChannelCongestionRateDisplay );
			pStaticCHCongestionGage->GetPicture( i * 4 )->SetShow( true );
			pStaticCHCongestionStr->GetString( 0 )->msg = GET_STRING( STR_ID_721 );
		}
	}


	for(int j = 0; j < static_cast< int >( g_pInstanceData->m_vecChannelInfo.size() ) && j < MAX_CHANNEL_COUNT; ++j)
	{
		wstring wstrtempChannelName = L"";
		wstring wstrtempFieldName = L"";
		wstring wstrtempFieldNameNum = L"";

		int iFieldNum = 0;

		wstrtempChannelName = g_pInstanceData->GetChannelServerName(g_pInstanceData->m_vecChannelInfo[j].m_iChannelID);

		int iCharNum = 0;
		ASSERT(wstrtempChannelName.empty() == false);

		if(wstrtempChannelName.empty() == false)
		{
			for(int k = 0; k < (int)wstrtempChannelName.size(); k++ )
			{
				WCHAR tempChar = wstrtempChannelName[k];
				if ( tempChar < '0' || tempChar > '9' )
					iCharNum++;
			}
			wstrtempFieldName = wstrtempChannelName.substr(0,iCharNum);

			iFieldNum = _wtoi(wstrtempChannelName.substr(iCharNum, (wstrtempChannelName.size()-iCharNum)).c_str());

			if(iFieldNum < 10)
			{
				wstrtempFieldNameNum = L"0";
			}
			wstrtempFieldNameNum += wstrtempChannelName.substr(iCharNum, (wstrtempChannelName.size()-iCharNum));

			CKTDGUIControl::CPictureData* pPicture = NULL;
			pPicture = new CKTDGUIControl::CPictureData();
			CKTDGUIControl::UIPointData* pPointData = new CKTDGUIControl::UIPointData();

			if(pStaticCHName == NULL && pPicture == NULL && pPointData == NULL)
				return;

			int iChannelNameOffset = 0; // ?? ???????? �?? UI ?????? ????? ?????? ???? ??????

			if(wstrtempFieldName == GET_STRING( STR_ID_17731 ))
			{
				wstring wstrtempField = L"RUBEN"; //2011.11.22 lygan_?????? //DLG_UI_Title02_A.tet ?? ??? ????? ??????
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());
				iChannelNameOffset = 8;
			}
			else if(wstrtempFieldName == GET_STRING( STR_ID_17732 ))
			{

				wstring wstrtempField = L"ELDER";
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());
				iChannelNameOffset = 8;
			}
			else if(wstrtempFieldName == GET_STRING( STR_ID_17733 ))
			{
				wstring wstrtempField = L"BESMA";
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());

			}
			else if(wstrtempFieldName == GET_STRING( STR_ID_17734 ))
			{
				wstring wstrtempField = L"ALTERA";
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());
			}
			else if(wstrtempFieldName == GET_STRING( STR_ID_23098 ))
			{
				wstring wstrtempField = L"PEITA";
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());

				iChannelNameOffset = 8;
			}
			else if(wstrtempFieldName == GET_STRING( STR_ID_17735 ))
			{

				wstring wstrtempField = L"VELDER";
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());
				iChannelNameOffset = 7;

			}
			else if(wstrtempFieldName == GET_STRING( STR_ID_17736 ))
			{
				wstring wstrtempField = L"HAMEL";
				wstrtempField += wstrtempFieldNameNum;
				pPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrtempField.c_str());
				iChannelNameOffset = 7;
			}

			if (iFieldNum >= 10)
				iChannelNameOffset -= 5;

			// ????? ??? ???? ??? ???????.
			iChannelNameOffset = 0;

			if((j+1)%2 == 0)
			{
				pPointData->leftTopPoint = D3DXVECTOR2(678.f+iChannelNameOffset,126 + static_cast< float >( (j/2) * 48));
			}
			else
			{
				pPointData->leftTopPoint = D3DXVECTOR2(355.f+iChannelNameOffset,132 + static_cast< float >( (j/2) * 48));
			}

			pPicture->SetPoint(pPointData);
			pPicture->SetShow(true);
			pStaticCHName->AddPicture(pPicture);
			pStaticCHName->SetShow(true);
		}

		std::map<int, KChannelBonusInfo>::const_iterator mit2 = g_pInstanceData->m_mapChannelBonusInfo.find(g_pInstanceData->m_vecChannelInfo[j].m_iChannelID);

		//ASSERT(mit2 != g_pInstanceData->m_mapChannelBonusInfo.end());

		if(mit2 != g_pInstanceData->m_mapChannelBonusInfo.end() )
		{
			wstring wstrChannelLv = KncUtil::toWideString( x2::str(x2::format("(LV%1%~%2%)") % mit2->second.m_iBeginLv % mit2->second.m_iEndLv) );
			float fRightChannelStrPos = 678.f;
			float fLeftChannelStrPos = 355.f;
			wstring wstrLvWordCheck = L"";
			bool bStrLvWordCheck = false;

			for ( int istrNumCheck = 0; istrNumCheck< wstrChannelLv.length(); ++istrNumCheck )
			{
				CKTDGUIControl::CPictureData* pLvPicture = new CKTDGUIControl::CPictureData();
				CKTDGUIControl::UIPointData* pLvPointData = new CKTDGUIControl::UIPointData();

				wstring wstrsubChannelLv = L"";

				if(pStaticCHName == NULL && pLvPicture == NULL && pLvPointData == NULL)
					continue;

				wstrsubChannelLv = wstrChannelLv[istrNumCheck];

				if ( wstrsubChannelLv == L"L" || wstrsubChannelLv == L"V" )
				{
					wstrLvWordCheck += wstrChannelLv[istrNumCheck];
					if ( wstrLvWordCheck == L"LV")
					{
						bStrLvWordCheck = true;
					}
					else
					{
						continue;
					}
				}

				if((j+1)%2 == 0)
				{
					pLvPointData->leftTopPoint = D3DXVECTOR2(fRightChannelStrPos,126 +20 + static_cast< float >( (j/2) * 48));
					if ( bStrLvWordCheck == true)
					{
						fRightChannelStrPos += 19.f;
					}
					else
					{
						fRightChannelStrPos += 7.f;
					}

				}
				else
				{
					pLvPointData->leftTopPoint = D3DXVECTOR2(fLeftChannelStrPos,126 +20 + static_cast< float >( (j/2) * 48));
					if ( bStrLvWordCheck == true)
					{
						fLeftChannelStrPos += 19.f;
					}
					else
					{
						fLeftChannelStrPos += 7.f;
					}

				}

				if ( bStrLvWordCheck == false )
				{
					pLvPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrsubChannelLv.c_str() );
				}
				else
				{
					pLvPicture->SetTex(L"DLG_UI_Title02_A.TGA", wstrLvWordCheck.c_str() );
					bStrLvWordCheck = false;
				}

				pLvPicture->SetPoint(pLvPointData);
				pLvPicture->SetShow(true);
				pStaticCHName->AddPicture(pLvPicture);
			}

			pStaticCHName->SetShow(true);
		}
	}
}

// kimhc // 2009-12-15 // ?????? ?�?????? �?? ??????


void CX2StateServerSelect::UnitHandling( double fTime, float fElapsedTime )
{
	if ( NULL != m_pUnitViewerUI )
	{
		if ( CX2UnitViewerUI::GetUnitClicked() == true )
		{
			D3DXVECTOR3 RotateY = D3DXVECTOR3(0, -((float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXDelta() * m_fMouseSensitivity), 0);
			m_pUnitViewerUI->GetMatrix().RotateRel(RotateY);
			m_bRotateReturn = false;

			m_fRotSpeed = 13.0f;
		}
		else
		{
			D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();
			if ( g_pData != NULL && m_pUnitViewerUI->GetUnit() != NULL && NULL != m_pUnitViewerUI->GetUnit()->GetUnitTemplet()  )
			{
				float fRotY = g_pData->RotateYSelectCharacterType( m_pUnitViewerUI->GetUnit()->GetClass() );
				m_pUnitViewerUI->GetMatrix().Rotate( D3DXToRadian(0),D3DXToRadian(fRotY),D3DXToRadian(0) );
			}
			int addSpeed = 20;

			if ( m_bRotateReturn == false )
			{
				m_fRotSpeed -= (fElapsedTime * addSpeed);

				if ( m_fRotSpeed <= 0 )
				{
					m_fRotSpeed = fElapsedTime;
				}

				if (m_fRemainRotY >= 0.0f &&  m_fRemainRotY < 3.14f )
				{

					if ( m_fRemainRotY <= 0.0f )
					{
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						m_fRemainRotY = 0.0f;
						m_bRotateReturn = true;
					}
					else
					{
						m_fRemainRotY -= (fElapsedTime*m_fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY <= 0.0f )
						{
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
							m_fRemainRotY = 0.0f;
							m_bRotateReturn = true;
						}
					}
				}
				else if ( m_fRemainRotY > 3.14f && m_fRemainRotY <= 6.18f  )
				{
					if ( m_fRemainRotY >= 6.18f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY += (fElapsedTime*m_fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY >= 6.18f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
				else if ( m_fRemainRotY >= -3.14f && m_fRemainRotY <= 0.0f )
				{
					if ( m_fRemainRotY >= 0.0f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY += (fElapsedTime*m_fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY >= 0.0f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
				else if (  m_fRemainRotY >= -6.28f && m_fRemainRotY <= -3.14f )
				{
					if ( m_fRemainRotY <= -6.28f )
					{
						m_bRotateReturn = true;
						m_fRemainRotY = 0.0f;
						m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
					}
					else
					{
						m_fRemainRotY -= (fElapsedTime*m_fRotSpeed);
						m_pUnitViewerUI->GetMatrix().Rotate(D3DXVECTOR3(0,m_fRemainRotY,0));

						if ( m_fRemainRotY <= -6.28f )
						{
							m_bRotateReturn = true;
							m_fRemainRotY = 0.0f;
							m_pUnitViewerUI->GetMatrix().Rotate( D3DXVECTOR3(0,0,0) );
						}
					}
				}
			}
		}

		CX2Unit::UnitData* pUnitData = &m_pUnitViewerUI->GetUnit()->AccessUnitData();

		m_pUnitViewerUI->OnFrameMove( fTime, fElapsedTime );
		if( true == pUnitData->m_bDeleted && m_pUnitViewerUI->GetXSkinAnim()->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
		{
			if( false == pUnitData->m_bDeletedMotionPlay)
			{
				m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_WAIT_DELETE );
				pUnitData->m_bDeletedMotionPlay = true;
			}
		}
		else if( m_pUnitViewerUI->GetXSkinAnim()->GetState() == CKTDGXSkinAnim::XAP_ONE_WAIT )
		{
			if( true == m_bPlayDownAnim )
			{
				if( false == pUnitData->m_bDeleted )
				{
					m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_WAIT );
				}

				m_bPlayDownAnim = false;
			}
		}
	}
}




void CX2StateServerSelect::ClearUnitViewerList()
{
	if( m_pUnitViewerUI != NULL )
	{
		m_pUnitViewerUI->GetUnit()->AccessUnitData().m_bDeletedMotionPlay = false;
	}

	CX2UnitViewerUI::DeleteKTDGObject( m_pUnitViewerUI );
	m_pUnitViewerUI = NULL;
}

CX2UnitViewerUI* CX2StateServerSelect::GetUnitViewer( CX2Unit* pUnit )
{
	for ( int i = 0; i < (int)m_UnitViewerUIList.size(); i++ )
	{
		CX2UnitViewerUI* pUnitViewerUI = m_UnitViewerUIList[i];
		if ( pUnitViewerUI->GetUnit() == pUnit )
			return pUnitViewerUI;
	}

	return NULL;
}




void CX2StateServerSelect::DownAnimFrameMove( double fTime, float fElapsedTime )
{
	if ( m_bPlayDownAnim == true )
	{
		m_fDownAnimTime += fElapsedTime;

		if ( m_pUnitViewerUI != NULL && m_pUnitViewerUI->GetShowObject() == false )
		{
			if ( m_fDownAnimTime > 0.f )
			{
				m_pUnitViewerUI->SetShowObject( true );
				m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVVMT_DOWN_LANDING );
				m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
				m_bPlayDownAnim = false;
			}
		}
	}
}

void CX2StateServerSelect::PlayDownAnim()
{
	m_bPlayDownAnim = true;
	m_fDownAnimTime = 0.0f;
}




bool CX2StateServerSelect::Handler_EGS_STATE_CHANGE_FIELD_REQ(bool bForceMove)
{
	KEGS_STATE_CHANGE_FIELD_REQ kPacket;
	D3DXVECTOR3 vStartPos(0, 0, 0);
	int startPos = 0;

	const CX2Unit::UnitData* pUnitData = NULL;
	if(g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL)
	{
		pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();
	}

	if( NULL != pUnitData )
	{
		int iMapId = pUnitData->m_nMapID;

		if( g_pData->GetLocationManager()->GetVillageMapTemplet((SEnum::VILLAGE_MAP_ID)iMapId) == NULL ||
			bForceMove == true )
		{
			// ???? ?????
			iMapId = SEnum::VMI_RUBEN;
			//vStartPos = D3DXVECTOR3(-16.f, 1445.f, -2873.f);
			startPos = 1;
		}
		else
		{
			if( pUnitData->m_EXP <= 0 )
			{
				// ??????????? ????.

				// ???? ?????
				iMapId = SEnum::VMI_RUBEN;
				startPos = 1;
			}
			else
			{
				startPos = g_pData->GetLocationManager()->GetLoginPos( iMapId );

				// ???????? ????? ??????.
				if( false == pUnitData->m_bIgnoreLastTouch  )
				{
					const CX2World::WorldData* pWorldData = g_pData->GetWorldManager()->GetWorldData( (CX2World::WORLD_ID) g_pData->GetLocationManager()->GetWorldID( (SEnum::VILLAGE_MAP_ID) iMapId ) );
					if( NULL != pWorldData )
					{
						CX2World* pWorld = g_pData->GetWorldManager()->CreateWorld( pWorldData->worldID ); // fix!! ???? ???????? ??? ????? ???? ?????? ?? ??? ?????????
						if( NULL != pWorld )
						{
							pWorld->SetShowObject( false );
							CKTDGLineMap* pLineMap = pWorld->GetLineMap();
							if( NULL != pLineMap )
							{
								const CKTDGLineMap::LineData* pLineData = pLineMap->GetLineData( pUnitData->m_ucLastTouchLineIndex );
								if( NULL != pLineData )
								{
									float fLastPosValue = halfToFloat( pUnitData->m_usLastPosValue );
									if( fLastPosValue < 0.f )
										fLastPosValue = 0.f;
									if( fLastPosValue > 1.f )
										fLastPosValue = 1.f;

									vStartPos = pLineData->startPos * (1.f - fLastPosValue) + pLineData->endPos * fLastPosValue;
									vStartPos = pLineMap->GetLandPosition( vStartPos, LINE_RADIUS, (int*) NULL );

									startPos = -1;
								}
							}
							SAFE_DELETE( pWorld );
						}
					}
				}
			}
		}


		if( startPos >= 0 )
		{
			vStartPos = g_pData->GetLocationManager()->GetStartPosLoc(startPos);
		}

		g_pData->GetLocationManager()->SetLastPos(vStartPos);
		//g_pData->GetLocationManager()->SetLastPosLineIndex();


		kPacket.m_iMapID =  (int)iMapId;


		g_pData->GetLocationManager()->SetVillage( (SEnum::VILLAGE_MAP_ID)kPacket.m_iMapID, vStartPos );

		g_pData->GetServerProtocol()->SendPacket( EGS_STATE_CHANGE_FIELD_REQ, kPacket );
		g_pMain->AddServerPacket( EGS_STATE_CHANGE_FIELD_ACK );

	}

	return true;
}

bool CX2StateServerSelect::Handler_EGS_STATE_CHANGE_FIELD_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_STATE_CHANGE_FIELD_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	if( g_pMain->DeleteServerPacket( EGS_STATE_CHANGE_FIELD_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_541 ), L"DLG_UI_Selection_MessageBox_No_Button.lua" );

			bool bCanCreateWorld = true;
			CX2World::WORLD_ID	m_iWorldId = CX2World::WI_NONE;
			SEnum::VILLAGE_MAP_ID m_villageId = SEnum::VMI_INVALID;;
			D3DXVECTOR3 m_vPos( 0.0f, 0.0f, 0.0f );

			// ???? ?????? village?? ???????.
			if(g_pData != NULL && g_pData->GetLocationManager() != NULL)
			{
				g_pData->GetLocationManager()->GetVillage(m_villageId, m_vPos);
				m_iWorldId = (CX2World::WORLD_ID)g_pData->GetLocationManager()->GetWorldID(m_villageId);
			}
			else
			{
				// ?????? ?????? ?? ????.
				bCanCreateWorld = false;
			}

			if(m_iWorldId <= CX2World::WI_NONE || m_iWorldId >= CX2World::WI_END)
			{
				// ?????? ?????? ?? ????
				bCanCreateWorld = false;
			}

			// ???? ????
			if(bCanCreateWorld == true)
			{
//#if 0
//				g_pTFieldGame = new CX2TFieldGame(m_iWorldId);
//
//				if(g_pTFieldGame != NULL)
//				{
//					g_pTFieldGame->AddUnit( g_pData->GetMyUser()->GetSelectUnit(), true, true );
//					g_pTFieldGame->SetAddedMyUnit(true);
//				}
//#endif

				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_VILLAGE_MAP, NULL, false );
			}
		}
		else
		{
			switch ( kEvent.m_iOK )
			{
			case NetError::ERR_FIELD_05:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_REPLACED_STRING((STR_ID_18892, "i",kEvent.m_iLimitLevel)), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );


				} break;
			case NetError::ERR_FIELD_06:
				{
					if( NULL != g_pData && NULL != g_pData->GetDungeonManager() )
					{
						wstring DungeonName = g_pData->GetDungeonManager()->MakeDungeonNameString( static_cast<SEnum::DUNGEON_ID>(kEvent.m_iRequireDungeonID));
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_REPLACED_STRING((STR_ID_18893, "L", DungeonName )), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );

					}
				} break;
			}

			// ???? ?????
			Handler_EGS_STATE_CHANGE_FIELD_REQ(true);

			// ????????? ????
			//g_pData->GetLocationManager()->RestoreVillage();
		}

	}
	return true;
}

bool CX2StateServerSelect::Handler_EGS_CHANGE_USER_LIST_IN_SECTOR_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_USER_LIST_IN_SECTOR_NOT kEvent;

	DeSerialize( pBuff, &kEvent );

	if(g_pTFieldGame != NULL)
	{
		for(int i=0; i<(int)kEvent.m_vecNewUserList.size(); ++i)
		{
			g_pTFieldGame->JoinFieldUnit( &kEvent.m_vecNewUserList[i] );
		}
	}

	return true;
}







void CX2StateServerSelect::Handler_EGS_CREATE_TUTORIAL_ROOM_REQ()
{
	KEGS_CREATE_TUTORIAL_ROOM_REQ kEGS_CREATE_ROOM_REQ;

	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_RoomType			= CX2Room::RT_DUNGEON;
	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_RoomName			= L"tutorial";
	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_bPublic			= false;
	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_wstrPassword		= L"a*h97lk2";
	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_MaxSlot			= 1;

	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_DifficultyLevel	= CX2Dungeon::DL_NORMAL;
	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_fPlayTime			= 19999.0f;
	kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_cGetItemType		= CX2DungeonRoom::DGIT_RANDOM;

	switch( g_pData->GetMyUser()->GetSelectUnit()->GetType() )
	{
	default:
	case CX2Unit::UT_ELSWORD:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_ELSWORD;
		} break;

	case CX2Unit::UT_ARME:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_AISHA;
		} break;

	case CX2Unit::UT_LIRE:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_LENA;
		} break;

	case CX2Unit::UT_RAVEN:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_RAVEN;
		} break;

	case CX2Unit::UT_EVE:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_EVE;
		} break;

		//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG
	case CX2Unit::UT_CHUNG:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_CHUNG;
		} break;
		//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

	case CX2Unit::UT_ARA:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_ARA;
		} break;
	case CX2Unit::UT_ELESIS:
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_EL;
		} break;
	case CX2Unit::UT_ADD:		/// ?????? ??? ???.
		{
			kEGS_CREATE_ROOM_REQ.m_RoomInfo.m_iDungeonID	= SEnum::DI_BATTLE_FIELD_TUTORIAL_ADD;
		} break;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_CREATE_TUTORIAL_ROOM_REQ, kEGS_CREATE_ROOM_REQ );
	g_pMain->AddServerPacket( EGS_CREATE_TUTORIAL_ROOM_ACK, 40.f );
}

bool CX2StateServerSelect::Handler_EGS_CREATE_TUTORIAL_ROOM_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CREATE_TUTORIAL_ROOM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
	//CX2PacketLog::PrintLog( &kEvent );

	//SAFE_DELETE_DIALOG( m_pDLGCreateDungeonRoom );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_CREATE_TUTORIAL_ROOM_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
/*
			int startPos = 0;
			int iMapId;
			if(g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL)
				iMapId = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_nMapID;

			if(g_pData->GetLocationManager()->GetVillageMapTemplet((SEnum::VILLAGE_MAP_ID)iMapId) == NULL)
			{
				// ???? ?????
				iMapId = SEnum::VMI_RUBEN;
				//vStartPos = D3DXVECTOR3(-16.f, 1445.f, -2873.f);
				startPos = 1;
			}
			else
			{
				startPos = g_pData->GetLocationManager()->GetLoginPos( iMapId );
			}

			D3DXVECTOR3 vStartPos = g_pData->GetLocationManager()->GetStartPosLoc(startPos);
			g_pData->GetLocationManager()->SetLastPos(vStartPos);
			//g_pData->GetLocationManager()->SetLastPosLineIndex();

			g_pData->GetLocationManager()->SetVillage((SEnum::VILLAGE_MAP_ID)iMapId, vStartPos);
*/

			CX2Room::InitializeRoomPacketData();
			CX2Room::SetRoomPacketData( kEvent.m_RoomInfo, kEvent.m_vecSlot,
				vector<KRoomSlotInfo>(), kEvent.m_wstrCNIP );

			g_pData->DeleteAllRooms();
			g_pData->ResetDungeonRoom();
			if ( NULL != g_pX2Room && CX2Room::IsNewDataSet() )		/// ????? ??????? ???? ???????
				g_pX2Room->ApplyRoomPacketData();


			//?????? ??????? ???
			if(g_pInstanceData != NULL)
				g_pInstanceData->SetFirstJoinVillage(true);
			g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_645 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg" );
			g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_DUNGEON_ROOM, NULL, false );

			//???? ??????? ????
			if( NULL != g_pData && NULL != g_pData->GetPartyManager() )
				g_pData->GetPartyManager()->RememberComeBackInfoBeforeInstanceGameStart();

			if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
			{	//????? ??????? ?????
				g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_NOVICE_GUIDE_01, true );
			}
			return true;
		}
	}

	if( true == g_pMain->GetPlayFieldDungeon() )
		g_pMain->SetPlayFieldDungeon( false );

	return false;
}








//////////////////////////////////////////////////////////////////////////
// statelogin?? ??? ??? ???? ??
bool CX2StateServerSelect::Handler_EGS_CONNECT_REQ( const wstring& wstrGameServerIP, const int iGameServerPort )
{

	if( true == wstrGameServerIP.empty() )
		return false;

	if( true == m_bSentEGS_CONNECT_REQ )
		return false;


	// game server?? ??????????? nexon passport?? ??????? ???????.
	if( false == g_pMain->GetManualLogin() )
	{
		switch( g_pMain->GetPublisher() )
		{
		}
	}



	if( LoginErrorCheck() == true )
	{
		//????�?
		//g_pData->ResetServerProtocol();
		if ( g_pData->GetServerProtocol()->IsConnected () )
		{
			g_pData->GetServerProtocol()->DisconnectFromGameServer();
		}

		if( true == g_pData->GetServerProtocol()->ConnectedToGameServer( wstrGameServerIP.c_str(), iGameServerPort ) )
		{
			g_pMain->SetConnectedGameServerIP( wstrGameServerIP );
			g_pMain->SetConnectedGameServerPort( iGameServerPort );

			KEGS_CONNECT_REQ kPacket;
			kPacket.m_wstrVersion = g_pMain->GetClientVersion();

			g_pData->GetServerProtocol()->SendPacket( EGS_CONNECT_REQ, kPacket );
			g_pMain->AddServerPacket( EGS_CONNECT_ACK, 60.0f );

			m_bSentEGS_CONNECT_REQ = true;

			SAFE_DELETE_DIALOG( m_pDLGMsgBox );
			m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_725 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );


			g_pInstanceData->SetConnectChannelServerID( m_iTryConnectChannelID );
			m_iTryConnectChannelID = -1;


			// ???? ?????????? game_server ip?? ???????. (????????? TRServer ?????)
			if(g_pMain != NULL)
			{
				g_pMain->SetTRServerIp( wstrGameServerIP );
			}

			// �?? ???????? ?????? ???�?.
			Handler_ECH_DISCONNECT_REQ();
		}
		else
		{
#if !X2_DEV_TOOLS
#else

#endif // _SERVICE_

			if ( NULL == m_pDLGReConnectChannelServer )
			{

				if ( true == m_bWaitingEGS_CONNECT_REQ_ChannelButtonUp )
				{
					m_bCanNotConenctToBusyServer = true;
					m_sbSelectedServerSet = false;
					// ConnectToChannelServer(true);
					g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/ CX2Main::XS_SERVER_SELECT, NULL, false );

					if ( g_pData->GetServerProtocol()->IsConnected() == true )
					{
						g_pData->GetServerProtocol()->DisconnectFromGameServer();
					}
				}
				else
				{
					m_pDLGReConnectChannelServer = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ),  GET_STRING ( STR_ID_29884 ),
						SUSUCM_RE_CONNECT_CHANNEL_SERVER_OK, this, SUCM_EXIT, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
				}
				if ( true == g_pMain->m_bPlayIntroMovie )
					m_pDLGReConnectChannelServer->SetShow ( false );
			}
			else
			{
				m_pDLGReConnectChannelServer = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ),  GET_STRING ( STR_ID_29884 ),
					SUSUCM_RE_CONNECT_CHANNEL_SERVER_OK, this, SUCM_EXIT, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
			}

		}
	}
	else
	{
		//g_pData->DeleteServerProtocol();
		//g_pData->DeleteMyUser();
		g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_INVALID_ID_OR_PASSWORD );
		g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
	}

	m_bWaitingEGS_CONNECT_REQ_ChannelButtonUp = false;
	return true;
}

bool CX2StateServerSelect::Handler_EGS_CONNECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CONNECT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	if( g_pMain->DeleteServerPacket( EGS_CONNECT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
//#ifdef _SERVICE_
//			OpenGlobalURL( L"http://Elsword.nexon.com/Elsword/etc/gameinstall.aspx?Section=chceck3" );
//#endif
			g_pMain->SetServerUDPPort( kEvent.m_UDPPort );

			Handler_EGS_VERIFY_ACCOUNT_REQ();
		}
		else
		{
			//g_pData->DeleteServerProtocol();
			//g_pData->DeleteMyUser();
			switch( kEvent.m_iOK )
			{
			default:
				{
					g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_CONNECT_FAILED );
					g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
				} break;

			case NetError::ERR_CONNECT_00:
				{
					MessageBox( g_pKTDXApp->GetHWND(), GET_STRING( STR_ID_727 ), L"Quit", MB_OK );
					g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
				} break;

			case NetError::ERR_CONNECT_04:
				{


					m_bReservedConnectionToChannelServer = true;
					if ( NULL == m_pDLGReConnectChannelServer )
					{
						SAFE_DELETE_DIALOG ( m_pDLGMsgBox );

						m_bCanNotConenctToBusyServer = true;
						m_sbSelectedServerSet = false;
						// ConnectToChannelServer(true);
						g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/ CX2Main::XS_SERVER_SELECT, NULL, false );

						if ( g_pData->GetServerProtocol()->IsConnected() == true )
						{
							g_pData->GetServerProtocol()->DisconnectFromGameServer();
						}

						if ( true == g_pMain->GetIsPlayingTutorial() )
						{
							g_pMain->SetIsPlayingTutorial( false );
						}


						if ( true == g_pMain->m_bPlayIntroMovie )
							m_pDLGReConnectChannelServer->SetShow ( false );

					}



					//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"????????? ????? ???? ?? ?? ???????.", this );
				} break;
			}
		}
	}
	return true;
}

bool CX2StateServerSelect::Handler_EGS_VERIFY_ACCOUNT_REQ()
{
	KEGS_VERIFY_ACCOUNT_REQ kPacket;

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	kPacket.m_wstrUserID = g_pInstanceData->GetUserID();
	kPacket.m_wstrPasswd = g_pInstanceData->GetUserPassword();



	// ChannelingCode ?? ????
	// 0 : ??? 1 : ???? 2 : ???? ??? 3: ???? ????
	kPacket.m_iChannelingCode = g_pInstanceData->GetChannelingCode();

	//{{ kimhc // 2010-06-24 // OTP ???
	kPacket.m_wstrHWID = g_pInstanceData->GetNxHWID();
	//}} kimhc // 2010-06-24 // OTP ???

	kPacket.m_bDebugAuth = g_pMain->GetManualLogin();

	kPacket.m_strMachineID = g_pMain->GetMachineId();

	if( g_pInstanceData != NULL )
		g_pInstanceData->SetMachineId( kPacket.m_strMachineID );




	m_bEGS_VERIFY_ACCOUNT_REQ = true;

	kPacket.m_bSteamClient = false;  // no Steam client in the port; the field stays in the protocol

	kPacket.m_bAeriaClient = g_pInstanceData->IsAeriaChanneling();

	g_pData->GetServerProtocol()->SendPacket( EGS_VERIFY_ACCOUNT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_VERIFY_ACCOUNT_ACK, 60 );

	if( kPacket.m_strMachineID.empty() == true )
	{
		g_pMain->SendHackMail_VERIFYVALUE( ANTI_HACK_STRING_AntiHacking_MACHINE_ID_EMPTY );
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif

	return true;
}

bool CX2StateServerSelect::Handler_EGS_VERIFY_ACCOUNT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_VERIFY_ACCOUNT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_VERIFY_ACCOUNT_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{

			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(260, 270), GET_STRING( STR_ID_29369 ), g_pMain->GetNowState(), -1, -1.f, L"DLG_UI_OKMsgBoxPlusNew.lua", D3DXVECTOR2(0, -100));


			ELSWORD_VIRTUALIZER_START
			Handler_EGS_CHECK_MACHINE_ID_REQ();
			ELSWORD_VIRTUALIZER_END

//#ifdef _SERVICE_
//			OpenGlobalURL( L"http://Elsword.nexon.com/Elsword/etc/gameinstall.aspx?Section=chceck4" );
//#endif

			// ???????? ?????? OTP?? ????????? ???????.
			if( false == kEvent.m_kAccountInfo.m_wstrOTP.empty() )
			{
				g_pInstanceData->SetIDVerified(true);
				g_pInstanceData->SetUserPassword( kEvent.m_kAccountInfo.m_wstrOTP );
			}

#if !X2_DEV_TOOLS
#endif
			//////////////////////////////////////////////////////////////////////////
			//{{ 2009. 7. 25  ??????	�??????
			if( kEvent.m_kAccountInfo.m_iAuthLevel == CX2User::XUAL_DEV )
			{
				//MessageBox( g_pKTDXApp->GetHWND(), L"???? ?�? ???? ???????! ???? ?????? ?????????? ???? ???? ????????!", L"???!", MB_OK );
			}
			//}}
			//////////////////////////////////////////////////////////////////////////

			wstringstream wstrstm;
			wstrstm << GET_REPLACED_STRING( ( STR_ID_728, "L", kEvent.m_kAccountInfo.m_wstrID ) );
			g_pMain->SetMailNameToFindHack( wstrstm.str().c_str() );
            {
                std::string strID;
                ConvertWCHARToChar( strID, kEvent.m_kAccountInfo.m_wstrID );
                g_pMain->SetUserIdToFindHack_ThreadSafe( strID );
            }
			g_pMain->SetPublicIp( kEvent.m_wstrClientIP );
			if(kEvent.m_cHackingUserType == CX2User::HUT_AGREE_HACK_USER)
				g_pMain->SetHackingUser(true);

			m_bEGS_VERIFY_ACCOUNT_ACK = true;
			if ( false == m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection )
            {
			    CX2User::UserData kUserData;
			    kUserData = kEvent;
			    g_pData->ResetMyUser( kUserData );
				Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ ();
				//Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ();
				//Handler_EGS_CHARACTER_LIST_REQ();

            }

			//m_MaxUnitNum	= g_pData->GetMyUser()->GetUserData().maxUnitCount;
			//const int SLOT_COUNT_FOR_CREATE_UNIT = 1;
			//m_MaxPage		= (g_pData->GetMyUser()->GetUnitNum()+SLOT_COUNT_FOR_CREATE_UNIT+UNIT_SELECT_UI_MAX_UNIT_NUM-1) / UNIT_SELECT_UI_MAX_UNIT_NUM;




			/// 2014-01-17 // itexpertkim // ????� // ????? �??? ????? ??? ???




			g_pMain->GetGameOption().SetPlayGuide( kEvent.m_kAccountInfo.m_kAccountOption.m_bPlayGuide, false );





#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			//???? pc ??? ???? ????
			_SendSystemInfo();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK


		}
		else
		{
			StateLog( L"???" );

			//g_pData->DeleteServerProtocol();
			//g_pData->DeleteMyUser();
			switch ( kEvent.m_iOK )
			{
			case NetError::ERR_VERIFY_00:
			case NetError::ERR_VERIFY_01:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(270,350), NetError::GetErrStrF( NetError::ERR_USER_REGISTRATION_00 ), g_pMain->GetNowState(),SSSUCM_DOUBLE_CONNECT);
				} break;
			case NetError::ERR_VERIFY_24:
				{
					m_pDLGATLLimitCheckMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_17745 ), this, SSSUCM_ATL_LIMIT_CHECK_OK, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );



				} break;
			default:
				{
					g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_VERIFY_ACCOUNT );
					g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
				} break;
			}

		}
	}


#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif

	return true;
}

bool CX2StateServerSelect::Handler_ENX_USER_LOGIN_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KENX_USER_LOGIN_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	m_bENX_USER_LOGIN_NOT = true;
	m_bIsGameBang = kEvent.m_bIsGameBang;

	if ( NULL != g_pData && NULL != g_pData->GetPremiumBuffManager() )
		g_pData->GetPremiumBuffManager()->SetPcBangReward( kEvent.m_kPcBangReward );

	if ( NULL != g_pData && NULL != g_pData->GetPremiumBuffManager() )
		g_pData->GetPremiumBuffManager()->SetPremiumPC( m_bIsGameBang );

	return true;
}

bool CX2StateServerSelect::Handler_KXPT_PORT_CHECK_REQ()
{
	m_bPortCheckWait			= true;
	m_fPortCheckWaitTime		= 0.5f;

	KXPT_PORT_CHECK_REQ kXPT_PORT_CHECK_REQ;
	kXPT_PORT_CHECK_REQ.m_UserUID = g_pData->GetMyUser()->GetUID();
	//{{ 2013. 1. 9	?????	Merge ????IP ???? ????? ????IP?? ?�?( ?????? )
//#ifdef SERV_KTDX_RETRY_USING_INTERNAL_IP
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    kXPT_PORT_CHECK_REQ.m_InternalIPAddress = g_pData->GetGameUDP()->GetMyIPAddress();
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	kXPT_PORT_CHECK_REQ.m_wstrInternalIP = g_pData->GetGameUDP()->GetMyIP();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	kXPT_PORT_CHECK_REQ.m_usInternalPort = g_pData->GetGameUDP()->GetMyPort();
//#endif SERV_KTDX_RETRY_USING_INTERNAL_IP
	//}}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    return g_pData->GetGameUDP()->SendToIP( CKTDNUDP::ConvertIPToAddress( g_pMain->GetConnectedGameServerIP() ), g_pMain->GetServerUDPPort(), XPT_PORT_CHECK_REQ,
        &kXPT_PORT_CHECK_REQ, sizeof(kXPT_PORT_CHECK_REQ) );
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	KSerBuffer buff;
//	Serialize( &buff, &kXPT_PORT_CHECK_REQ );
//	return g_pData->GetGameUDP()->Send( g_pMain->GetConnectedGameServerIP(), g_pMain->GetServerUDPPort(), XPT_PORT_CHECK_REQ, (char*)buff.GetData(), buff.GetLength() );
//	//return g_pData->GetGameUDP()->Send( g_pMain->GetServerIP(), g_pMain->GetServerUDPPort(), XPT_PORT_CHECK_REQ, (char*)buff.GetData(), buff.GetLength() );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}

bool CX2StateServerSelect::Handler_KXPT_PORT_CHECK_ACK( const KXPT_PORT_CHECK_ACK& kXPT_PORT_CHECK_ACK )
{
	dbg::clog << L"MY IP : "
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        << CKTDNUDP::ConvertAddressToIP( kXPT_PORT_CHECK_ACK.m_IPAddress )
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//        << kXPT_PORT_CHECK_ACK.m_IP.c_str()
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        << dbg::endl;
	dbg::clog << L"MY Port : " << g_pMain->GetGameP2PPort() << dbg::endl;
	dbg::clog << L"MY Ext Port : " << kXPT_PORT_CHECK_ACK.m_Port << dbg::endl;

	m_bPortCheckWait			= false;
	m_fPortCheckWaitTime		= 0.0f;
	m_PortCheckRetryTime		= 0;

	g_pData->GetGameUDP()->SetMyExtPort( kXPT_PORT_CHECK_ACK.m_Port );

	Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ();
	return true;
}



/*virtual*/ bool CX2StateServerSelect::Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ()
{
	return CX2State::Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ();
}

/*virtual*/ bool CX2StateServerSelect::Handler_EGS_STATE_CHANGE_SERVER_SELECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_STATE_CHANGE_SERVER_SELECT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_STATE_CHANGE_SERVER_SELECT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			Handler_EGS_CURRENT_TIME_REQ();
			return true;
		}
	}
	return true;
}



bool CX2StateServerSelect::Handler_EGS_CURRENT_TIME_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_CURRENT_TIME_REQ );
	g_pMain->AddServerPacket( EGS_CURRENT_TIME_ACK );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_CURRENT_TIME_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CURRENT_TIME_ACK kEvent;
	DeSerialize( pBuff, &kEvent );
	//CX2PacketLog::PrintLog( &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_CURRENT_TIME_ACK ) == true )
	{
		g_pData->SetServerCurrentTime( kEvent.m_wstrCurrentTime );
		g_pData->SetServerCurrentTimeInt64(kEvent.m_tCurrentTime);
		//StateChangeLoading();

		if ( true == m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection &&
			 true == m_bSend_EGS_SELECT_UNIT_REQ_For_Auto_Connection )
		{
			Handler_EGS_SELECT_UNIT_REQ( m_pSelectUnit->GetUID(), m_pSelectUnit->GetUnitData().m_Level );

		}
		else
		{
			Handler_EGS_SELECT_SERVER_SET_REQ();
		}


		return true;
	}

	return false;
}


bool CX2StateServerSelect::LoginErrorCheck()
{

	//if( m_UserID.size() < 3 )
	//{
	//	//ID?? 4?? ??????? ????? ???
	//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"????? 4?? ??? ????????.", this );
	//	return false;
	//}
	//if( m_UserPassword.size() < 4 )
	//{
	//	//Password?? 4?? ??????? ????? ???
	//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"??????? 4?? ??? ????????.", this );
	//	return false;
	//}


	return true;
}



bool CX2StateServerSelect::Handler_ECH_GET_CHANNEL_LIST_REQ()
{
	g_pData->GetServerProtocol()->SendChID( ECH_GET_CHANNEL_LIST_REQ );
	g_pMain->AddServerPacket( ECH_GET_CHANNEL_LIST_ACK, 100.0f );

	return true;
}


bool CX2StateServerSelect::Handler_ECH_GET_CHANNEL_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_GET_CHANNEL_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( ECH_GET_CHANNEL_LIST_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// nothing;
		}
	}
	return true;
}

bool CX2StateServerSelect::Handler_ECH_GET_CHANNEL_LIST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_GET_CHANNEL_LIST_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	m_bReceivedChannelList = true;

#ifdef EXTEND_SERVER_GROUP_MASK
	g_pInstanceData->SetServerGroupID( kEvent.m_iServerGroupID );
#else
	g_pInstanceData->SetServerGroupID( (SERVER_GROUP_ID)kEvent.m_iServerGroupID );
#endif // EXTEND_SERVER_GROUP_MASK

	UpdateServerSelectButton( g_pInstanceData->GetServerGroupID() );

	g_pInstanceData->m_vecChannelInfo.resize(0);

	std::map< int, KChannelInfo >::const_iterator cit;
	for( cit = kEvent.m_mapChannelList.begin() ; cit != kEvent.m_mapChannelList.end(); cit++ )
	{
		const KChannelInfo& channelInfo = cit->second;

		if(g_pInstanceData != NULL)
			g_pInstanceData->m_vecChannelInfo.push_back( channelInfo );
	}


	g_pInstanceData->m_mapChannelBonusInfo.clear();

	std::map< int, KChannelBonusInfo >::const_iterator cmit;
	for( cmit = kEvent.m_mapChannelBonusList.begin() ; cmit != kEvent.m_mapChannelBonusList.end(); ++cmit )
	{
		//const KChannelBonusInfo& ChannelBonusInfo = cmit->second;
		if(g_pInstanceData != NULL)
			g_pInstanceData->m_mapChannelBonusInfo.insert(std::make_pair(cmit->first, cmit->second));
	}

	//std::sort( m_vecChannelInfo.begin(), m_vecChannelInfo.end(), LessChannelIndex() );

	//{{ kimhc // 2009-12-15 // ?????? ??? ???
	g_pInstanceData->GetMiniMapUI()->SetChannelComboBox();
	m_bReservedConnectionToChannelServer = false;


	// ??? ??????????? ?????? ???????? ?????? ?????? ????????? ???????? ??? ???????? ???.
	if( true == m_bWaiting_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK )
		return true;

	int iLowerUserChannelIndex = -1;
	int iLowerUserChannelCount = INT_MIN;

	for ( int i = 0; i < static_cast<int> ( g_pInstanceData->m_vecChannelInfo.size() ); ++i )
	{
		KChannelInfo channelInfo = g_pInstanceData->m_vecChannelInfo[ i ];
		if ( iLowerUserChannelCount < channelInfo.m_iMaxUser - channelInfo.m_iCurrentUser )
		{
#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
			if ( false == g_pInstanceData->IsConnectedChannel( channelInfo.m_iServerUID ) )
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????
			{
				iLowerUserChannelCount = channelInfo.m_iMaxUser - channelInfo.m_iCurrentUser;
				iLowerUserChannelIndex = i;
#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
				g_pInstanceData->SetConnectedChannelID( channelInfo.m_iServerUID );
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????
			}
		}
	}
	if ( iLowerUserChannelIndex == -1 || iLowerUserChannelCount == INT_MIN )
	{
		ASSERT( !L"Can't Find Auto Connecting Channel Index" );
		iLowerUserChannelIndex = 0;
#ifdef FIX_REFORM_ENTRY_POINT_10TH			// kimjh, ????? ????? ???????? ?????? ????
		return false;
#endif // FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
	}

	const KChannelInfo& channelInfo = g_pInstanceData->m_vecChannelInfo[ iLowerUserChannelIndex ];

	m_iTryConnectChannelID = channelInfo.m_iChannelID;

	m_bSelectChannel = true;
	Handler_EGS_CONNECT_REQ( channelInfo.m_wstrIP, channelInfo.m_usMasterPort );


	return true;
}


bool CX2StateServerSelect::Handler_ECH_DISCONNECT_REQ()
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


bool CX2StateServerSelect::Handler_ECH_DISCONNECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_pMain->DeleteServerPacket( ECH_DISCONNECT_ACK ) == true )
	{
		g_pData->GetServerProtocol()->DisconnectFromChannelServer();
	}

	if( true == m_bReservedConnectionToChannelServer )
	{
		ConnectToChannelServer(true);

	}

	return true;
}


bool CX2StateServerSelect::Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_REQ()
{

	if(
#ifdef  X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
        g_pData->GetServerProtocol() != NULL
#else
		g_pData->GetServerProtocol()->GetUserProxy() != NULL
#endif // X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
        )
	{
#ifdef  X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
        g_pData->GetServerProtocol()->UserProxy_SetIntendedDisconnect( true );
#else
		g_pData->GetServerProtocol()->GetUserProxy()->SetIntendedDisconnect( true );
#endif // X2OPTIMIZE_X2SERVERPROTOCOL_MULTITHREAD_CRASH_BUG_FIX
	}

	m_bWaiting_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK = true;

	g_pData->GetServerProtocol()->SendID( EGS_DISCONNECT_FOR_SERVER_SELECT_REQ );
	g_pMain->AddServerPacket( EGS_DISCONNECT_FOR_SERVER_SELECT_ACK, 100.0f );


	return true;
}


bool CX2StateServerSelect::Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( g_pMain->DeleteServerPacket( EGS_DISCONNECT_FOR_SERVER_SELECT_ACK ) == true )
	{
		m_bWaiting_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK = false;

		g_pData->GetServerProtocol()->DisconnectFromGameServer();


		g_pMain->SetConnectedGameServerIP( L"" );
		g_pMain->SetConnectedGameServerPort( 0 );
	}

	if ( true == m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection )
	{
		if ( -1 == m_iReservedEntryPointServerChannelIndex )
		{
			int iLowerUserChannelIndex = -1;
			int iLowerUserChannelCount = INT_MIN;
			vector< KChannelInfo > * vecChannelInfo = NULL;
			if ( m_pSelectUnit != NULL )
			{
				if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
					vecChannelInfo = &m_vecGaiaChannelInfo;
				else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
					vecChannelInfo = &m_vecSolesChannelInfo;
			}

			if ( vecChannelInfo == NULL )
			{
				ASSERT (!L"Can't Find Server Group ID !" );
#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
				return false;
#else // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????
				vecChannelInfo = &m_vecGaiaChannelInfo;
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????

			}

			for ( int i = 0; i < static_cast<int> ( vecChannelInfo->size() ); ++i )
			{
				KChannelInfo channelInfo = (*vecChannelInfo)[ i ];
#ifdef FIX_REFORM_ENTRY_POINT_10TH			// kimjh, ????? ????? ???????? ?????? ????
				if ( false == g_pInstanceData->IsConnectedChannel( channelInfo.m_iServerUID ) )
#endif // FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
				{
					if ( iLowerUserChannelCount < channelInfo.m_iMaxUser - channelInfo.m_iCurrentUser )
					{
						iLowerUserChannelCount = channelInfo.m_iMaxUser - channelInfo.m_iCurrentUser;
						iLowerUserChannelIndex = i;
#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
						g_pInstanceData->SetConnectedChannelID( channelInfo.m_iServerUID  );
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????
					}
				}
			}
			if ( iLowerUserChannelIndex == -1 || iLowerUserChannelCount == INT_MIN )
			{
				ASSERT( !L"Can't Find Auto Connecting Channel Index" );
#ifdef FIX_REFORM_ENTRY_POINT_10TH			// kimjh, ????? ????? ???????? ?????? ????
				return false;
#else // FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
				iLowerUserChannelIndex = 0;
#endif // FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
			}

			const KChannelInfo& channelInfo = (*vecChannelInfo)[ iLowerUserChannelIndex ];
			m_iTryConnectChannelID = channelInfo.m_iChannelID;
			m_bSelectChannel = true;
			Handler_EGS_CONNECT_REQ ( channelInfo.m_wstrIP.c_str(), channelInfo.m_usMasterPort );
		}

		else if ( -2 == m_iReservedEntryPointServerChannelIndex )
		{

			if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
			{
				m_iTryConnectChannelID = FindAndConnectMatchingLevelGameServer( 1, m_mapGaiaChannelBonusInfo, m_vecGaiaChannelInfo);
				BOOST_FOREACH ( const KChannelInfo & kChannelInfo , m_vecGaiaChannelInfo )
				{
					if ( m_iTryConnectChannelID == kChannelInfo.m_iChannelID )
					{
						Handler_EGS_CONNECT_REQ ( kChannelInfo.m_wstrIP.c_str(), kChannelInfo.m_usMasterPort );
					}
				}
			}
			else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
			{
				m_iTryConnectChannelID = FindAndConnectMatchingLevelGameServer( 1, m_mapSolesChannelBonusInfo, m_vecSolesChannelInfo);
				BOOST_FOREACH ( const KChannelInfo & kChannelInfo , m_vecSolesChannelInfo )
				{
					if ( m_iTryConnectChannelID == kChannelInfo.m_iChannelID )
					{
						Handler_EGS_CONNECT_REQ ( kChannelInfo.m_wstrIP.c_str(), kChannelInfo.m_usMasterPort );
					}
				}
			}
			else
			{
				ASSERT( !L"Can't Find Server Group ID !" );
				return false;
			}
			m_bSelectChannel = true;
		}

		else
		{
			vector< KChannelInfo > * vecChannelInfo = NULL;
			if ( m_pSelectUnit != NULL )
			{
				if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
					vecChannelInfo = &m_vecGaiaChannelInfo;
				else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
					vecChannelInfo = &m_vecSolesChannelInfo;
			}

			if ( vecChannelInfo == NULL )
			{
				ASSERT( ! L"Can't Find Server Group ID ! ");
				vecChannelInfo = &m_vecGaiaChannelInfo;
			}

			const KChannelInfo& channelInfo = (*vecChannelInfo)[ m_iReservedEntryPointServerChannelIndex ];
			m_iTryConnectChannelID = channelInfo.m_iChannelID;
			m_bSelectChannel = true;
			Handler_EGS_CONNECT_REQ ( channelInfo.m_wstrIP.c_str(), channelInfo.m_usMasterPort );
		}
	}


	return true;
}







bool CX2StateServerSelect::Handler_EGS_GET_MY_MESSENGER_SN_REQ( UidType unitUID )
{
	KEGS_GET_MY_MESSENGER_SN_REQ kPacket;
	kPacket = unitUID;



	g_pData->GetServerProtocol()->SendPacket( EGS_GET_MY_MESSENGER_SN_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_MY_MESSENGER_SN_ACK );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_2558 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_GET_MY_MESSENGER_SN_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_MY_MESSENGER_SN_ACK kEvent;
	DeSerialize( pBuff, &kEvent );


	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	if( g_pMain->DeleteServerPacket( EGS_GET_MY_MESSENGER_SN_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if( true == m_bRemoveUnitAfterReceiveNMSerialNum )
			{
				m_bRemoveUnitAfterReceiveNMSerialNum = false;


			}
		}
		else
		{
			if( true == m_bRemoveUnitAfterReceiveNMSerialNum )
			{
				m_bRemoveUnitAfterReceiveNMSerialNum = false;
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_2561 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
			}
		}
		return true;
	}

	return false;
}


bool CX2StateServerSelect::ConnectToChannelServer( bool bIsReversingConnect /* = false */ )
{
	ASSERT( NULL != g_pData->GetServerProtocol() );

	if( g_pData->GetServerProtocol()->IsChConnected() == true )
	{
		g_pData->GetServerProtocol()->DisconnectFromChannelServer();
	}

	m_bSentEGS_CONNECT_REQ = false;
	g_pData->GetServerProtocol()->DisconnectFromChannelServer();

	if( g_pData->GetServerProtocol()->ConnectedToChannelServer( g_pMain->GetPickedChannelServer().m_kServerIP.c_str(), g_pMain->GetPickedChannelServer().m_usMasterPort ) == true )
	{
		//{{ 09.08. ?�? : ????-??? ????? ??? ????.
//#ifdef SERV_KOG_OTP_VERIFY
		return Handler_ECH_VERIFY_ACCOUNT_REQ();
// #else
// 		return Handler_ECH_GET_CHANNEL_LIST_REQ();
// #endif
		//}}
	}
	else
	{

		{
			if ( NULL == m_pDLGReConnectChannelServer )
			{
				m_pDLGReConnectChannelServer = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 305, 375 ),  GET_STRING ( STR_ID_29884 ),
					SUSUCM_RE_CONNECT_CHANNEL_SERVER_OK, this, SUCM_EXIT, L"DLG_UI_Selection_MessageBox_Ok_Cancle_Button_New.lua" );
				if ( true == g_pMain->m_bPlayIntroMovie )
					m_pDLGReConnectChannelServer->SetShow ( false );

			}

		}
	}

	return false;
}

bool CX2StateServerSelect::Handler_ECH_VERIFY_ACCOUNT_REQ()
{
	KECH_VERIFY_ACCOUNT_REQ kPacket;

	kPacket.m_wstrID = g_pInstanceData->GetUserID();
	kPacket.m_wstrPassword = g_pInstanceData->GetUserPassword();

	kPacket.m_bLogin = !(g_pInstanceData->IsIDVerified());
	kPacket.m_bIDVerified = g_pInstanceData->IsIDVerified();
	if( g_pMain != NULL )
		kPacket.m_strMachineID = g_pMain->GetMachineId();

	kPacket.m_iChannelingCode = g_pInstanceData->GetChannelingCode();

	g_pData->GetServerProtocol()->SendChPacket( ECH_VERIFY_ACCOUNT_REQ, kPacket );
	g_pMain->AddServerPacket( ECH_VERIFY_ACCOUNT_ACK );

	return true;
}

bool CX2StateServerSelect::Handler_ECH_VERIFY_ACCOUNT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_VERIFY_ACCOUNT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( ECH_VERIFY_ACCOUNT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// ???? Gash ???? ???? ???????? ?????? Pass?? ??? ?????? ????(??????? ?????), ??? ?????? ??????.
			g_pInstanceData->SetIDVerified(true);
			g_pInstanceData->SetUserPassword( kEvent.m_wstrPassport );

	//{{????? ?????�? ??????
			g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->SetServerCurrentTime( kEvent.m_wstrCurrentTime );
			g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->MassFileMapping();

			Handler_ECH_GET_SERVERGROUP_LIST_REQ();
			Handler_ECH_GET_CHANNEL_LIST_REQ();

			return true;
		}
		else
		{
			switch( kEvent.m_iOK )
			{
			case NetError::ERR_VERIFY_20:
				{
					g_pKTDXApp->NoticeQuitType( CKTDXApp::KQT_CONNECT_FAILED );
					g_pKTDXApp->SendGameMessage( XGM_QUIT_GAME, NULL, NULL, false );
				} break;
			default:
				{

				} break;
			}
		}
	}

	return false;
}



bool CX2StateServerSelect::Handler_EGS_SECOND_SECURITY_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SECOND_SECURITY_INFO_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( kEvent.m_iOK == NetError::ERR_SECOND_SECURITY_AUTH_FAILED_LIMIT )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_12352 ), this, CX2State::SUCM_EXIT, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );


	}
	else if ( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
		if ( g_pMain->GetSecurityPad()->GetIsAutoAuthoritySecondSecurityBegin() == true  )
		{
			g_pMain->GetSecurityPad()->AutoAuthoritySecondSecurity();
		}
		else
#else // FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
			if ( m_bEnableUnitSelectBySecondSecurity == true
				&& g_pMain->GetSecurityPad()->GetIsAutoAuthoritySecondSecurityBegin() == true
				)
			{
				g_pMain->GetSecurityPad()->AutoAuthoritySecondSecurity();
			}

			else
#endif // FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
		{
			if(kEvent.m_bUseSecondPW == true)
			{
				if ( g_pMain->m_bPlayIntroMovie == true )
				{
					m_bReservedSecurityPad = true;
				}
				else
				{
					g_pMain->GetSecurityPad()->SetShow(true, CX2SecurityPad::SPS_GAME_START);
					g_pMain->GetSecurityPad()->SetUseSecondPW(true);
				}
			}
			else
			{
				m_bEnableUnitSelectBySecondSecurity = true;
			}
		}
	}
	return true;
}




/*virtual*/ bool CX2StateServerSelect::LastMsgByESC()
{
	// oasis907 : ????? [2011.6.8] ??????? ????? ???? ????�???? ESC? ????
	if(g_pMain->GetSecurityPad() != NULL &&
		g_pMain->GetSecurityPad()->GetShow() == true)
	{
		return true;
	}

		if ( NULL != m_pDLGCheckHack && m_pDLGCheckHack->GetShow() == true )
			return true;
		ProcessIntroMovieEnd();		// ????? ???????? ?????? ?? �??????? ?? ??

	// ????? ???? ??????, ESC ?? ?????? ?????
	if ( true == m_bWaitingEGS_CONNECT_REQ_ChannelButtonUp )
	{
		return true;
	}

	if ( NULL != m_pDLGMsgBox )
	{
		return true;
	}

	if( true == CX2StateServerSelect::m_sbSelectedServerSet )
	{
		// �?? ????�???? ESC ??? ?? ????? ????�???? ???
		if( true == IsChannelSelectMode() )
		{
			MoveToCharacterSelectUI();
		}
		else if ( g_pMain->m_bPlayIntroMovie == true )
		{
			g_pMain->m_bPlayIntroMovie = false;
			// ????????? ?????? State ?? ??? ????�?.
			if ( NULL != m_pDLGReConnectChannelServer && false == m_pDLGReConnectChannelServer->GetShow() )
			{
				m_sbSelectedServerSet = false;
				m_bCanNotConenctToBusyServer = false;
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, /*CX2Main::XS_UNIT_SELECT*/ CX2Main::XS_SERVER_SELECT, NULL, false );
			}
		}
		else
		{
			QuitGame();
		}
	}
	else
	{
		return QuitGame();
	}

	return true;
}

/*virtual*/ void CX2StateServerSelect::LastMsgPopOk()
{
	if ( m_pDLGLastMsgPopUp != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGLastMsgPopUp, NULL, false );
	m_pDLGLastMsgPopUp = NULL;

	g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, SSSUCM_UNIT_SELECT_EXIT, NULL, false );
}

void CX2StateServerSelect::UnitSelectExit()
{
	ClearBeforeUnit( m_pSelectUnit );
	ClearUnitButton();
	if(m_pDLGServerGroupSelectBack != NULL)
		m_pDLGServerGroupSelectBack->SetShowEnable(true, true);
	m_pDLGServerSelectBack->SetShowEnable(true, true);
	m_pDLGUnitSelectFront->SetShowEnable(false, false);

	//{{ kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???
	m_pDLGChannelSelectBack->SetShowEnable( true, true );
	//}}  kimhc // 2009-12-15 // ?????? ??? ????? ???? �?? ??????? ???

	m_pSelectUnit = NULL;
	SetRightSideFlagColor ( D3DXCOLOR ( 0.f, 0.f, 0.f, 0.f ) );
	m_iSelectUnitUID = 0;
	m_sbSelectedServerSet = false;

	m_bENX_USER_LOGIN_NOT		= false;
	m_bEGS_VERIFY_ACCOUNT_REQ	= false;
	m_bEGS_VERIFY_ACCOUNT_ACK	= false;
	m_bPortCheckReq				= false;

	m_bSentEGS_CONNECT_REQ 		= false;
	m_bReceivedChannelList 		= false;
	if( NULL != g_pData->GetServerProtocol() )
	{
		g_pData->GetServerProtocol()->SendTRID( ETR_UNIT_DISCONNECT );
	}
	// ??????????? ?????? ???�?
	Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_REQ();

	// �??????? ???????.
	ConnectToChannelServer();

	if(g_pMain->GetSecurityPad() != NULL &&
		g_pMain->GetSecurityPad()->GetShow() == true)
	{
		g_pMain->GetSecurityPad()->SetShow(false);
	}

	ClearChannelButton();
	g_pKTDXApp->SkipFrame();
}

//{{ kimhc // 2009-12-15 // ?????? ?�??? ??? ?????? ???
//bool CX2StateServerSelect::OpenScriptServerGroupFile()
//{
//	string			strFileName;
//	SERVER_GROUP_ID eServerGroupID	= SGI_INVALID;
//	bool			bParsingOK		= false;
//
//	ConvertWCHARToChar( strFileName, g_pData->GetSavedServerGroupFileName() );
//
//	ConvertFileAnsiToUTF8( strFileName, strFileName );
//
//	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true );
//
//	KGCMassFileManager::CMassFile::MASSFILE_MEMBERFILEINFO_POINTER Info;
//	Info = g_pKTDXApp->GetDeviceManager()->GetMassFileManager()->LoadDataFile_LocalFile( g_pData->GetSavedServerGroupFileName() );
//	if( Info != NULL )
//	{
//		if( true == g_pKTDXApp->LoadLuaTinker_LocalFile( g_pData->GetSavedServerGroupFileName().c_str() ) )
//		{
//			if( true == g_pKTDXApp->LoadAndDoMemory_LocalFile( &luaManager, g_pData->GetSavedServerGroupFileName().c_str() ) )
//			{
//				LUA_GET_VALUE_ENUM( luaManager, L"SERVER_GROUP", 			eServerGroupID,			SERVER_GROUP_ID,		SGI_INVALID	);
//			}
//		}
//	}
//
//	switch ( eServerGroupID )
//	{
//	case SGI_SOLES:
//	case SGI_GAIA:
//		{
//			g_pInstanceData->SetServerGroupID( eServerGroupID );
//			bParsingOK = true;
//		}
//		break;
//
//	default:
//		{
//#ifdef RANDOM_SERVER
//			if( g_pMain->GetDefaultChannelServerIPIndex() == SGI_INVALID )
//			{
//				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( (rand() % 2) ) );
//			}
//			else
//			{
//				g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( g_pMain->GetDefaultChannelServerIPIndex() ) );
//			}
//#else
//			g_pInstanceData->SetServerGroupID( static_cast<SERVER_GROUP_ID>( g_pMain->GetDefaultChannelServerIPIndex() ) );
//#endif
//		}
//		break;
//
//	}
//
//	return bParsingOK;
//
//}

//{{ 2012.02.20 ?????	????? ???? ??????? ???? (???? ??? ?? ????)
bool CX2StateServerSelect::Handler_EGS_FINAL_DELETE_UNIT_REQ()
{
	KEGS_FINAL_DELETE_UNIT_REQ kPacket;
	kPacket.m_iUnitUID = m_pSelectUnit->GetUID();

	g_pData->GetServerProtocol()->SendPacket( EGS_FINAL_DELETE_UNIT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_FINAL_DELETE_UNIT_ACK  );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_16104 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Positive_01.ogg" );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_FINAL_DELETE_UNIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_FINAL_DELETE_UNIT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_FINAL_DELETE_UNIT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( kEvent.m_iUnitUID );
			if ( pUnit == NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_713 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua",D3DXVECTOR2 (0, 0),  L"UI_PopUp_Negative_01.ogg" );

				return true;
			}

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305,375), GET_STRING( STR_ID_714 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
			// ChangeUnitButtonInfo();

			if ( g_pData->GetMyUser()->GetUnitNum() <= 0 )
			{
				g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_701 ), L"DLG_UI_Selection_MessageBox_No_Button.lua", L"UI_PopUp_Positive_01.ogg"  );
				g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_CREATE_UNIT, NULL, false );
			}
			else
			{
				Handler_EGS_CHARACTER_LIST_REQ();
			}
/*
			ClearBeforeUnit( pUnit );
			ClearUnitButton();
			g_pData->GetMyUser()->DeleteUnitByUID(kEvent.m_iUnitUID);
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_16108 ), this );
			CreateUnitButtonNew();
			ChangeUnitButtonInfo();
*/
		}
	}
	return true;
}

bool CX2StateServerSelect::Handler_EGS_RESTORE_UNIT_REQ()
{
	KEGS_RESTORE_UNIT_REQ kPacket;
	kPacket.m_iUnitUID = m_pSelectUnit->GetUID();

	g_pData->GetServerProtocol()->SendPacket( EGS_RESTORE_UNIT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESTORE_UNIT_ACK  );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305,375), GET_STRING( STR_ID_16105 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );

	return true;
}

bool CX2StateServerSelect::Handler_EGS_RESTORE_UNIT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESTORE_UNIT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_RESTORE_UNIT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( kEvent.m_iUnitUID );
			if( pUnit == NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305,375), GET_STRING( STR_ID_713 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				return true;
			}

			m_bMaintainNowUnitButton = true;
			Handler_EGS_CHARACTER_LIST_REQ();
		}
		else if(kEvent.m_iOK == NetError::ERR_RESTORE_UNIT_02)
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByUID( kEvent.m_iUnitUID );
			if ( pUnit == NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305,375), GET_STRING( STR_ID_713 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
				return true;
			}

			pUnit->AccessUnitData().m_trRestoreAbleDate = kEvent.m_tRestoreAbleDate;

			CTime tRestoreAbleDate(kEvent.m_tRestoreAbleDate);
			std::wstring wstrYear = (CStringW)( tRestoreAbleDate.Format(_T( "%Y" )) );
			std::wstring wstrMonth = (CStringW)( tRestoreAbleDate.Format(_T( "%m" )) );
			std::wstring wstrDay = (CStringW)( tRestoreAbleDate.Format(_T( "%d" )) );
			std::wstring wstrHour = (CStringW)( tRestoreAbleDate.Format(_T( "%H" )) );
			std::wstring wstrMinute = (CStringW)( tRestoreAbleDate.Format(_T( "%M" )) );
			std::wstring wstrMessage = GET_REPLACED_STRING( ( STR_ID_16113, "SSSSS", wstrDay.c_str(), wstrMonth.c_str(), wstrYear.c_str(), wstrHour.c_str(), wstrMinute.c_str()));
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305,375), wstrMessage.c_str(), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Negative_01.ogg" );
		}
	}
	return true;
}
//}}

bool CX2StateServerSelect::Handler_ECH_GET_SERVERGROUP_LIST_REQ()
{
	g_pData->GetServerProtocol()->SendChID( ECH_GET_SERVERGROUP_LIST_REQ );
	g_pMain->AddServerPacket( ECH_GET_SERVERGROUP_LIST_ACK, 100.0f );

	return true;
}

bool CX2StateServerSelect::Handler_ECH_GET_SERVERGROUP_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_GET_CHANNEL_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( ECH_GET_SERVERGROUP_LIST_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// nothing;
		}
	}
	return true;
}

bool CX2StateServerSelect::Handler_ECH_GET_SERVERGROUP_LIST_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KECH_GET_SERVERGROUP_LIST_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	m_bReceivedServerGroupList = true;

#ifdef CRASH_PROBE_LOG
	CrashLog( "{E46CACFD-654A-4c77-9FD7-FABCBA0E703C}" );
#endif // CRASH_PROBE_LOG

	g_pInstanceData->m_mapServerGroupInfo.clear();
	g_pInstanceData->m_mapServerGroupInfo = kEvent.m_mapServerGroupList;

	// �?? ????? UI?? Update???
	UpdateServerGroupListUI();

#ifdef CRASH_PROBE_LOG
	CrashLog( "{A45C0907-23CF-41d1-89BF-0D9F3594DA74}" );
#endif // CRASH_PROBE_LOG

	return true;
}

bool CompareServerGroup(const KServerGroupInfo& a, const KServerGroupInfo& b)
{
	return a.m_iOrder < b.m_iOrder;
}

void CX2StateServerSelect::UpdateServerGroupListUI()
{
	const D3DXVECTOR2 posDLG(20, 60);
	const D3DXVECTOR2 posButton(18, 18);

	// ????
	SAFE_DELETE_DIALOG( m_pDLGServerGroupSelectBack );

    m_pDLGServerGroupSelectBack =  new CKTDGUIDialog( this, L"DLG_UI_ServerGroup_Selection.lua" );

	if ( m_pDLGServerGroupSelectBack == NULL )
		return;

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGServerGroupSelectBack );
	m_pDLGServerGroupSelectBack->SetPos( posDLG );

	// ???????
	int iServerGroupSize = 0;
	vector<KServerGroupInfo> vecSortedServerGroup;
	map<int, KServerGroupInfo>::iterator iterEnable = g_pInstanceData->m_mapServerGroupInfo.begin();
	while(iterEnable != g_pInstanceData->m_mapServerGroupInfo.end())
	{
		//if(iterEnable->second.m_bEnable == true)
		{
			++iServerGroupSize;
			vecSortedServerGroup.push_back(iterEnable->second);
		}

		++iterEnable;
	}
	std::sort( vecSortedServerGroup.begin(), vecSortedServerGroup.end(), CompareServerGroup);

	int iMaxServergroup = min(iServerGroupSize, MAX_SERVERGROUP_SIZE);
	int i = 0;

	// ???
	const float button_height = 61.f-1.f;
	const float ADD_Y = button_height*iMaxServergroup - 13;

	CKTDGUIStatic* pStaticBG = (CKTDGUIStatic*)m_pDLGServerGroupSelectBack->GetControl(L"BG");
	if(pStaticBG)
	{
		CKTDGUIStatic::CPictureData* pPictureMiddle = pStaticBG->GetPicture(1);
		CKTDGUIStatic::CPictureData* pPictureBottom = pStaticBG->GetPicture(2);

		if(pPictureMiddle != NULL && pPictureMiddle->pPoint != NULL)
		{
			pPictureMiddle->pPoint->addSize.y = ADD_Y;
			pPictureMiddle->pPoint->SetAutoPointByTextureSize();
		}
		if(pPictureBottom != NULL && pPictureBottom->pPoint != NULL)
		{
			D3DXVECTOR2 pos = pPictureBottom->GetPos();
			pos.y += ADD_Y;
			pPictureBottom->SetPos( pos );
		}
	}

	// ???
	int button_index = 0;
	vector<KServerGroupInfo>::iterator iter = vecSortedServerGroup.begin();
	for(; iter != vecSortedServerGroup.end(); ++iter)
	{
		ASSERT(button_index < MAX_SERVERGROUP_SIZE);

		if(i >= iMaxServergroup)
			break;

		//if(false == iter->second.m_bEnable)
		//	continue;

		x2::wformat fmtButtonName(L"server_button_%1%");
		CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGServerGroupSelectBack->GetControl( x2::str(fmtButtonName % button_index).c_str() );
		if(pButton)
		{
			pButton->AddDummyInt(iter->m_iServerGroupUID);
			pButton->SetShow(true);
		}

		x2::wformat fmtButtonCaption(L"server_name_%1%");
		CKTDGUIStatic* pStaticName = (CKTDGUIStatic*)m_pDLGServerGroupSelectBack->GetControl( x2::str(fmtButtonCaption % button_index).c_str() );
		if(pStaticName)
		{
			pStaticName->SetShow(true);

			CKTDGUIControl::UIStringData* pStringName = pStaticName->GetString(0);
			if(pStringName != NULL)
				pStringName->msg = iter->m_wstrServerGroupName;
			CKTDGUIControl::UIStringData* pStringShadow = pStaticName->GetString(1);
			if(pStringShadow != NULL)
				pStringShadow->msg = iter->m_wstrServerGroupName;
			CKTDGUIControl::UIStringData* pStringPostfix = pStaticName->GetString(2);
			if(pStringPostfix != NULL)
			{
				CKTDGFontManager::CUKFont* pFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( pStringName->fontIndex );
				if(pFont)
				{
					int width = pFont->GetWidth( pStringName->msg.c_str() );
					pStringPostfix->pos.x += width + 10;
				}
			}
		}

		++button_index;
	}

	UpdateServerSelectButton( g_pMain->GetPickedChannelServerIPIndex() );
}

void CX2StateServerSelect::UpdateServerSelectButton( int iID )
{
	if(m_pDLGServerGroupSelectBack != NULL)
	{
		x2::wformat fmtButtonName(L"server_button_%1%");
		for(int i=0; i<MAX_SERVERGROUP_SIZE; ++i)
		{
			CKTDGUIButton* pServerButton = (CKTDGUIButton*)m_pDLGServerGroupSelectBack->GetControl( x2::str(fmtButtonName % i).c_str() );
			if(pServerButton)
			{
				int iServerGroup = pServerButton->GetDummyInt(0);
				if(iServerGroup == iID)
				{
					pServerButton->SetDownStateAtNormal(true);
					//m_iChannelNum = pServerButton->GetDummyInt( 1 );		// �?? ????
				}
				else
					pServerButton->SetDownStateAtNormal(false);
			}
		}
		m_iChannelNum = 24;
		g_pMain->SetPickedChannelServerIPIndex( iID );
	}
}
//}}  kimhc // 2009-12-15 // ?????? ?�??? ??? ?????? ???


bool CX2StateServerSelect::Handler_EGS_KEYBOARD_MAPPING_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_KEYBOARD_MAPPING_INFO_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	m_bRecivedKeyMap = true;

	if ( NULL != g_pKTDXApp->GetDIManager() && NULL != g_pKTDXApp->GetDIManager()->Getkeyboard()
		 && true == g_pMain->IsValidPacket( kEvent.m_iOK ) )
	{
		if ( kEvent.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.empty() )
		{
			g_pKTDXApp->GetDIManager()->SetDefaultMap();

			if ( NULL != g_pMain && NULL != g_pMain->GetKeyPad() )
			{
				g_pMain->GetKeyPad()->LoadSlotData();
				g_pMain->GetKeyPad()->SaveSlotData( true );

				if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
				{
					g_pData->GetUIManager()->GetUIQuickSlot()->ResetQuickSlotUI();
				}
			}
		}
		else if ( GAMEACTION_RETURN == kEvent.m_kKeyboardMappingInfo.m_mapKeyboardMappingInfo.size() )
		{
			SettingUserKeyboardMappingInfo( kEvent.m_kKeyboardMappingInfo );
		}
		// ?????? ????? ????? ??? ????? ??? ???�?
		else
		{
			if ( NULL != g_pMain && NULL != g_pMain->GetKeyPad() )
				g_pMain->GetKeyPad()->InitKeySlotData();

			SettingUserKeyboardMappingInfo( kEvent.m_kKeyboardMappingInfo );
		}

	}

	if ( NULL != g_pKTDXApp->GetDIManager() && NULL != g_pKTDXApp->GetDIManager()->GetJoystic()
		&& true == g_pMain->IsValidPacket( kEvent.m_iOK ) )
	{
		if ( kEvent.m_kKeyboardMappingInfo.m_mapGamePadMappingInfo.empty() )
		{
			g_pKTDXApp->GetDIManager()->SetGamePadDefaultMap();
		}
		else
		{
			SettingUserGamePadMappingInfo( kEvent.m_kKeyboardMappingInfo );
		}
	}

	return true;
}

void CX2StateServerSelect::SettingUserKeyboardMappingInfo( const KKeyboardMappingInfo& kKeyboardMappingInfo_ )
{
	std::map< short, short >::const_iterator constIter;

	for ( constIter = kKeyboardMappingInfo_.m_mapKeyboardMappingInfo.begin() ;
		constIter != kKeyboardMappingInfo_.m_mapKeyboardMappingInfo.end(); ++constIter )
	{
		// DB?? ????? Key Setting ???? ?�? ????? ???? ??????
		if ( constIter->second < CX2KeyPad::USE_KEY_NUMBER )
			g_pKTDXApp->GetDIManager()->Getkeyboard()->SetAction( GAME_ACTION( constIter->first ), CX2KeyPad::USE_DIK_LIST[constIter->second] );
		else
			g_pKTDXApp->GetDIManager()->Getkeyboard()->SetAction( GAME_ACTION( constIter->first ), 0x00 );
	}

	if ( NULL != g_pMain && NULL != g_pMain->GetKeyPad() )
	{
		g_pMain->GetKeyPad()->LoadSlotData();

		if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
		{
			g_pData->GetUIManager()->GetUIQuickSlot()->ResetQuickSlotUI();
		}
	}
}

void CX2StateServerSelect::SettingUserGamePadMappingInfo( const KKeyboardMappingInfo& kKeyboardMappingInfo_ )
{
	std::map< short, short >::const_iterator constIter;

	for ( constIter = kKeyboardMappingInfo_.m_mapGamePadMappingInfo.begin() ;
		constIter != kKeyboardMappingInfo_.m_mapGamePadMappingInfo.end(); ++constIter )
	{
		g_pKTDXApp->GetDIManager()->GetJoystic()->SetAction( GAME_ACTION( constIter->first ), constIter->second );
	}
}

bool CX2StateServerSelect::Handler_EGS_CHAT_OPTION_INFO_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHAT_OPTION_INFO_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( true == g_pMain->IsValidPacket( kEvent.m_iOK ) && NULL != g_pChatBox )
	{
		g_pChatBox->SetOptionData(kEvent.m_mapChatOptionSetInfo);
	}
	return true;
}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
void CX2StateServerSelect::_SendSystemInfo()
{
	//?????? ????
	KEGS_SYSTEM_INFO kPacket;
	kPacket.m_iAccountUID = g_pData->GetMyUser()->GetUserData().userUID;
	kPacket.m_iSystemKey = g_pMain->GetSystemInfo()->GetMacAddress();
	kPacket.m_wstrCPU = g_pMain->GetSystemInfo()->GetCpuString();
	kPacket.m_wstrMemory = g_pMain->GetSystemInfo()->GetMemString();
	kPacket.m_wstrGPU = g_pMain->GetSystemInfo()->GetDisplayString(DXUTGetD3DObject());
	kPacket.m_wstrOS = g_pMain->GetSystemInfo()->GetOsString();

	//
	g_pData->GetServerProtocol()->SendPacket( EGS_SYSTEM_INFO, kPacket );
}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK





void CX2StateServerSelect::HideAllUnitViewerUI ()
{
	for ( int i = 0; i < (int)m_UnitViewerUIList.size(); i++ )
	{
		CX2UnitViewerUI* pUnitViewerUI = m_UnitViewerUIList[i];
		pUnitViewerUI->SetShowObject( false );
		pUnitViewerUI->SetShowUnitViewerUI( false );
	}
}

void CX2StateServerSelect::CreateServerSelectUnitViewerUI ( CX2Unit * pUnit_ )
{
	if ( NULL != m_pUnitViewerUI )
	{
		CX2UnitViewerUI::DeleteKTDGObject ( m_pUnitViewerUI );
		m_pUnitViewerUI = NULL;
	}

	if ( NULL != pUnit_ )
	{
		m_pUnitViewerUI = CX2UnitViewerUI::CreateUnitViewerUI();
		m_pUnitViewerUI->SetLightPos( -250, 100, -600 );	// ??????? ????? ??? ????
		g_pKTDXApp->GetDGManager()->AddObjectChain( m_pUnitViewerUI );

		if ( NULL != m_pDLGUnitSelectFront )
		{
			D3DXVECTOR3 vUnitPos	= m_pDLGUnitSelectFront->GetDummyPos( 18 );
			D3DXVECTOR3 vEye		= m_pDLGUnitSelectFront->GetDummyPos( 19 );
			D3DXVECTOR3 vLookAt		= m_pDLGUnitSelectFront->GetDummyPos( 20 );
			D3DXVECTOR3 vFov		= m_pDLGUnitSelectFront->GetDummyPos( 21 );

			pUnit_->AccessInventory().RemovePCBangEquips();

			m_pUnitViewerUI->SetClassChange( false );
			m_pUnitViewerUI->SetUnit( pUnit_, CX2UnitViewerUI::UVS_SELECTUNIT );

			g_pKTDXApp->GetDGManager()->GetCamera().Point( vEye.x, vEye.y, vEye.z, vLookAt.x, vLookAt.y, vLookAt.z );
			g_pMain->SetProjectionFov( vFov.x );

			m_pUnitViewerUI->GetMatrix().Move( D3DXVECTOR3 ( vUnitPos.x, vUnitPos.y, vUnitPos.z ) );
			m_pUnitViewerUI->GetMatrix().Scale( D3DXVECTOR3( 1.2f, 1.2f, 1.2f ) * m_pUnitViewerUI->GetScale() );
			m_pUnitViewerUI->OnFrameMove( g_pKTDXApp->GetTime(),g_pKTDXApp->GetElapsedTime() );
			m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVVMT_DOWN_LANDING );
			m_bPlayDownAnim = true;

			CX2Unit::UnitData* pUnitData = &(pUnit_->AccessUnitData());
			/* ???? ????? ??? ???
			CKTDGUIStatic* pStatic_UnitDeleteInfo = (CKTDGUIStatic*)(m_pDLGUnitSelectFront->GetControl( L"StaticUnitSelectDeleteInfo" ));
			if( NULL != pStatic_UnitDeleteInfo )
			{
				if( pUnitData->m_bDeleted )
				{
					CTime tDeleAbleDate( pUnitData->m_trDelAbleDate );

					// ???? ???? ???? ?????
					std::wstring wstrYear = (CStringW)( tDeleAbleDate.Format(_T("%Y")) );
					std::wstring wstrMonth = (CStringW)( tDeleAbleDate.Format(_T("%m")) );
					std::wstring wstrDay = (CStringW)( tDeleAbleDate.Format(_T("%d")) );
					std::wstring wstrHour = (CStringW)( tDeleAbleDate.Format(_T("%H")) );
					std::wstring wstrMinute = (CStringW)( tDeleAbleDate.Format(_T("%M")) );

#if defined(CLIENT_GLOBAL_LINEBREAK)
					pStatic_UnitDeleteInfo->GetString(0)->msg = CWordLineHandler::GetStrByLineBreakInX2Main(GET_REPLACED_STRING( ( STR_ID_16109, "SSSSS", wstrDay.c_str(), wstrMonth.c_str(), wstrYear.c_str(), wstrHour.c_str(), wstrMinute.c_str() ) ), 200, XUF_DODUM_15_BOLD );
#elif defined(CLIENT_GLOBAL_LINEBREAK_FOR_CN)
					pStatic_UnitDeleteInfo->GetString(0)->msg = g_pMain->GetStrByLienBreak(GET_REPLACED_STRING( ( STR_ID_16109, "SSSSS", wstrDay.c_str(), wstrMonth.c_str(), wstrYear.c_str(), wstrHour.c_str(), wstrMinute.c_str() ) ), 180, pStatic_UnitDeleteInfo->GetString(0)->fontIndex );
#else
					pStatic_UnitDeleteInfo->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_16109, "SSSSS", wstrDay.c_str(), wstrMonth.c_str(), wstrYear.c_str(), wstrHour.c_str(), wstrMinute.c_str() ) );
#endif //defined(CLIENT_GLOBAL_LINEBREAK)
					pStatic_UnitDeleteInfo->SetShow( true );
				}
				else
				{
					pStatic_UnitDeleteInfo->SetShow( false );
				}
			}
			*/
			// ????? ???? ???? ???
			CKTDGUIButton* pBtn_FinalDelete = (CKTDGUIButton*)(m_pDLGUnitSelectFront)->GetControl( L"final_delete_unit" );
			if( NULL != pBtn_FinalDelete )
			{
				if( pUnitData->m_bDeleted )
				{
					//CTime cClientTime = CTime::GetCurrentTime();
					//__int64 tint64TimeGap = g_pData->GetServerCurrentTime() - cClientTime.GetTime();
					//if(pUnitData->m_trDelAbleDate <  ( g_pData->GetServerCurrentTime() + tint64TimeGap) ) // 2013.01.30 lygan_?????? // ?????? ??? ???????? ?????? ????? ????? ???? ??? �?????
					if( pUnitData->m_trDelAbleDate <  g_pData->GetServerCurrentTime64() )
					{
						pBtn_FinalDelete->SetShowEnable( true, true );
					}
					else
					{
						pBtn_FinalDelete->SetShowEnable( true, false );
					}
				}
				else
				{
					pBtn_FinalDelete->SetShowEnable( false, false );
				}
			}

			// ????? ???? ???
			CKTDGUIButton* pBtn_Restore = (CKTDGUIButton*)(m_pDLGUnitSelectFront)->GetControl( L"restore_unit" );
			if( NULL != pBtn_Restore )
			{
				if( pUnitData->m_bDeleted )
				{
					if( pUnitData->m_trRestoreAbleDate < g_pData->GetServerCurrentTime64() )
					{
						pBtn_Restore->SetShowEnable( true, true );
					}
					else
					{
						pBtn_Restore->SetShowEnable( true, false );
					}
				}
				else
				{
					pBtn_Restore->SetShowEnable( false, false );
				}
			}
		}
	}
}

// ????? ?�? ?? ????????.
void CX2StateServerSelect::SetFocusUnitButtonByIndex ( const int iIndex_ )
{
	for ( UINT i = 0; i < m_vecUnitSlot.size(); ++i )
	{
		CKTDGUIDialogType pUnitSlot =  m_vecUnitSlot[i];

		if ( pUnitSlot == NULL )
			continue;

		CKTDGUIButton* pCharButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 0 ) );
		if ( i == iIndex_ )
			pCharButton->SetDownStateAtNormal( true );
		else
			pCharButton->SetDownStateAtNormal( false );
	}
}

void CX2StateServerSelect::SetRightSideFlagColor ( D3DXCOLOR colorFlag )
{
	if ( NULL != m_pDLGUnitSelectFront )
	{
		CKTDGUIStatic* pStaticSideFlag = m_pDLGUnitSelectFront->GetStatic_LUA("SideFlag");
		// CKTDGUIControl::CPictureData* pPictureBattleFieldName = pStaticBattleFieldName->GetPicture( 0 );
		if ( NULL != pStaticSideFlag )
		{
			CKTDGUIControl::CPictureData* pSideFlagPictureData = pStaticSideFlag->GetPicture ( 0 );
			if ( NULL != pSideFlagPictureData )
			{
				pSideFlagPictureData->SetColor ( colorFlag );
			}
		}
	}
}

void CX2StateServerSelect::SetShowServerStateButtonSet ( bool bIsShowUnitListState, bool bIsShowChannelListState )
{
	if( NULL != m_pDLGUnitSelectFront )
	{
		CKTDGUIButton* pEndGameButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"new_finish_game") );

		CKTDGUIStatic* pSlotNumberStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectFront->GetControl(L"number"));
		if ( false == bIsShowUnitListState && false == bIsShowChannelListState )
		{
			if( NULL != pEndGameButton )
			{
				pEndGameButton->SetShowEnable(false,false);
			}

			if( NULL != pSlotNumberStatic )
			{
				pSlotNumberStatic->SetShowEnable( false, false );
			}
			SetShowSelectUnitInfo ( NULL );
		}
		else
		{
			if( NULL != pEndGameButton )
			{
				pEndGameButton->SetShowEnable(true, true);
			}

			if( NULL != pSlotNumberStatic )
			{
				pSlotNumberStatic->SetShowEnable( true, true );
			}
		}

		// �???, ????? ???? ???, �?? ????, ???? ???, ???? ????, ????? ????, ?�? ???? ( ???? ???? ), "???? ???? ???? ?????? / ??? ???? ???? ??????" ?? ???? ???
		CKTDGUIStatic* pSelectUnitTitleStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectBack->GetControl(L"charac") );
		if ( NULL != pSelectUnitTitleStatic )
		{
			pSelectUnitTitleStatic->SetShow ( bIsShowUnitListState );
		}

		for ( UINT i = 0; i < m_vecUnitSlot.size(); ++i )
		{
			CKTDGUIDialogType pUnitSlot =  m_vecUnitSlot[i];

			if ( pUnitSlot == NULL )
				break;
			pUnitSlot->SetShowEnable( bIsShowUnitListState, bIsShowUnitListState );
		}

		CKTDGUIButton* pCreateUnitButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"createUnit"));
		if( NULL != pCreateUnitButton )
		{
			pCreateUnitButton->SetShowEnable(bIsShowUnitListState,bIsShowUnitListState);
		}

		CKTDGUIStatic* pSecurityPadStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectFront->GetControl(L"new_Securitypad"));
		CKTDGUIStatic* pSecurityPadCreateStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectFront->GetControl(L"new_Securitypad_Create"));

		if( NULL != pSecurityPadStatic && NULL != pSecurityPadCreateStatic )
		{
			if ( NULL != g_pMain && NULL != g_pMain->GetSecurityPad() )
			{
				if ( true == g_pMain->GetSecurityPad()->GetUseSecondPW() )
				{
					pSecurityPadStatic->SetShowEnable(bIsShowUnitListState,bIsShowUnitListState);
					pSecurityPadCreateStatic->SetShowEnable( false,false );
				}
				else
				{
					pSecurityPadStatic->SetShowEnable(false , false );
					pSecurityPadCreateStatic->SetShowEnable( bIsShowUnitListState,bIsShowUnitListState);
				}
			}
		}
		CKTDGUIStatic* pFadeNotice1 = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectBack->GetControl(L"FadeNotice1"));
		if ( NULL != pFadeNotice1 )
		{
			pFadeNotice1->SetShow ( bIsShowUnitListState );
		}




		CKTDGUIStatic* pGoToChannelListStateStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectFront->GetControl(L"new_channel_select_bt"));
		if( NULL != pGoToChannelListStateStatic )
		{
			pGoToChannelListStateStatic->SetShowEnable(bIsShowUnitListState,bIsShowUnitListState);
		}

		CKTDGUIStatic* pDeleteUnitStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectFront->GetControl(L"new_ChaDelete"));
		if( NULL != pDeleteUnitStatic )
		{
			pDeleteUnitStatic->SetShowEnable(bIsShowUnitListState,bIsShowUnitListState);
		}

		CKTDGUIButton* pUnitSlotListLeftArrowButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"ButtonUnitListLeftArrow"));
		if( NULL != pUnitSlotListLeftArrowButton )
		{
			pUnitSlotListLeftArrowButton->SetShowEnable(bIsShowUnitListState,bIsShowUnitListState);
		}

		CKTDGUIButton* pUnitSlotListRightArrowButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"ButtonUnitListRightArrow"));
		if( NULL != pUnitSlotListRightArrowButton )
		{
			pUnitSlotListRightArrowButton->SetShowEnable(bIsShowUnitListState,bIsShowUnitListState);
		}

		// �???, ????? ????????, ???? ????, ?????, ????????, ?�? ???? ( �?? ???? ) ?? ???? ???
		CKTDGUIStatic* pChannelListStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectBack->GetControl(L"ChannelListStatic") );
		if ( NULL != pChannelListStatic )
		{
			pChannelListStatic->SetShow ( bIsShowChannelListState );
		}



		CKTDGUIButton* pTutorialButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"new_Tutorial"));
		if( NULL != pTutorialButton )
		{
			pTutorialButton->SetShowEnable(bIsShowChannelListState,bIsShowChannelListState);
		}

		CKTDGUIButton* pBackToUnitSelectButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"new_BackToUnitSelect"));
		if( NULL != pBackToUnitSelectButton )
		{
			pBackToUnitSelectButton->SetShowEnable(bIsShowChannelListState,bIsShowChannelListState);
		}


		CKTDGUIButton* pStartGameButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"new_channel_select_start"));
		if( NULL != pStartGameButton )
		{
			pStartGameButton->SetShowEnable(bIsShowChannelListState,bIsShowChannelListState);
		}

		CKTDGUIButton* pChannelSlotListLeftArrowButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"ButtonChannelListLeftArrow"));
		if( NULL != pChannelSlotListLeftArrowButton )
		{
			pChannelSlotListLeftArrowButton->SetShowEnable(bIsShowChannelListState,bIsShowChannelListState);
		}

		CKTDGUIButton* pChannelSlotListRightArrowButton = static_cast<CKTDGUIButton*>(m_pDLGUnitSelectFront->GetControl(L"ButtonChannelListRightArrow"));
		if( NULL != pChannelSlotListRightArrowButton )
		{
			pChannelSlotListRightArrowButton->SetShowEnable(bIsShowChannelListState,bIsShowChannelListState);
		}

		CKTDGUIStatic* pFadeNotice2 = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectBack->GetControl(L"FadeNotice2"));
		if ( NULL != pFadeNotice2 )
		{
			pFadeNotice2->SetShow ( bIsShowChannelListState );
		}
	}
}

/* virtual */ bool CX2StateServerSelect::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
			if ( CX2UnitViewerUI::GetUnitClicked() == true && m_pUnitViewerUI != NULL )
			{
				D3DXVECTOR3 rot = m_pUnitViewerUI->GetMatrix().GetRotate();
				int degree = (int)D3DXToDegree( rot.y );
				m_fRemainRotY = D3DXToRadian( degree % 360 );
			}
			CX2UnitViewerUI::SetUnitClicked( false );
		}
		break;
	}

	return CX2State::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CX2StateServerSelect::SetShowSelectUnitInfo( CX2Unit * pUnit )
{
	if ( NULL != m_pDLGUnitSelectBack )
	{
		if ( NULL != static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectBack->GetControl(L"SelectUnitInfo") ) )
		{
			CKTDGUIStatic* pGameStartStatic = static_cast<CKTDGUIStatic*>(m_pDLGUnitSelectBack->GetControl(L"SelectUnitInfo") );

			if ( NULL != pUnit )
			{
				wstringstream wstringstreamLevelInfo;
				wstringstreamLevelInfo << L"Lv. " << pUnit->GetUnitData().m_Level;
				pGameStartStatic->SetShow( true );
				pGameStartStatic->SetString( 0, wstringstreamLevelInfo.str().c_str() );
				wstring wstrNickname = pUnit->GetNickName();
				if ( -1 != wstrNickname.find ( L"__DELETED__") )
				{
					wstrNickname = GET_STRING(STR_ID_29404);//????? ????
				}
				pGameStartStatic->SetString( 1, wstrNickname.c_str() );
			}
			else
			{
				pGameStartStatic->SetShow( false );
			}
		}
	}
}

void CX2StateServerSelect::ClearChannelButton()
{
	for(UINT i=0; i<m_vecChannelSlot.size(); ++i)
	{
		CKTDGUIDialogType pChannelSlot =  m_vecChannelSlot[i];
		SAFE_DELETE_DIALOG(pChannelSlot);
	}
	m_vecChannelSlot.resize(0);
	m_pSelectChannelButton = NULL;
}

void CX2StateServerSelect::ChangeSelectUnitStandMeshInstance ( CX2Unit::UNIT_TYPE eUnitType_ )
{

	if ( NULL != g_pData->GetUIMajorXMeshPlayer() && NULL != g_pData->GetUIEffectSet() )
	{
		wstring wstrName = L"";
		std::vector<wstring> vecWstrName;

		if ( NULL != g_pData->GetUIMajorXMeshPlayer() )
		{
			for( CKTDGXMeshPlayer::CXMeshInstanceHandle hStandMeshInst : m_vecStandMeshInst )
			{
				g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( hStandMeshInst );
			}
			m_vecStandMeshInst.clear();
		}
		g_pData->GetUIEffectSet()->StopEffectSet( m_hStandEffect );

		switch ( eUnitType_ )
		{
		case CX2Unit::UT_ELSWORD :
			vecWstrName.push_back(L"Dummy_Village_Board1");
			wstrName = L"EffectSet_Elsword_Stand_Aura";
			break;
		case CX2Unit::UT_ARME :
			vecWstrName.push_back(L"Dummy_Village_Board2");
			wstrName = L"EffectSet_Aisha_Stand_Aura";
			break;
		case CX2Unit::UT_LIRE :
			vecWstrName.push_back(L"Dummy_Village_Board3");
			vecWstrName.push_back(L"Dummy_Village_Board10");
			wstrName = L"EffectSet_Rena_Stand_Aura";
			break;
		case CX2Unit::UT_RAVEN :
			vecWstrName.push_back(L"Dummy_Village_Board4");
			wstrName = L"EffectSet_Revan_Stand_Aura";
			break;
		case CX2Unit::UT_EVE :
			vecWstrName.push_back(L"Dummy_Village_Board5");
			wstrName = L"EffectSet_Eve_Stand_Aura";
			break;
		case CX2Unit::UT_CHUNG :
			vecWstrName.push_back(L"Dummy_Village_Board6");
			wstrName = L"EffectSet_Chung_Stand_Aura";
			break;

		case CX2Unit::UT_ARA :
			vecWstrName.push_back(L"Dummy_Village_Board7");
			wstrName = L"EffectSet_Ara_Stand_Aura";
			break;

		case CX2Unit::UT_ELESIS :
			vecWstrName.push_back(L"Dummy_Village_Board8");
			wstrName = L"EffectSet_Elsword_Stand_Aura";
			break;

		case CX2Unit::UT_ADD :
			vecWstrName.push_back(L"Dummy_Village_Board9");
			wstrName = L"EffectSet_Add_Stand_Aura";
			break;
		default :
			ASSERT ( !L"Wrong Unit Type");
			return ;
		}
		for( wstring wstrName : vecWstrName )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshStandInst = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( m_pUnitViewerUI,
				wstrName.c_str(), m_pUnitViewerUI->GetMatrix().GetPos(),
				D3DXVECTOR3( 0, 0, 0 ), D3DXVECTOR3( 0, 0, 0 ) );

			if( NULL != pMeshStandInst )
			{
				D3DXVECTOR3 vRot = pMeshStandInst->GetRotateLocalDegree();
				//vRot.y -= 7.45f;
				//vRot.z -= 2.75f;
				//pMeshStandInst->SetRotateLocalDegree( vRot );
				m_vecStandMeshInst.push_back(pMeshStandInst->GetHandle());
			}
		}
		if( m_vecStandMeshInst.empty() == false )
			m_hStandEffect = g_pData->GetUIEffectSet()->PlayEffectSetByMeshPlayer(wstrName.c_str(), m_vecStandMeshInst[0], CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR );
	}
}
/** @function : ChangeToCharacterSelectUI
	@brief : �?? ????�???? '????? ????�'???? ???
*/
void CX2StateServerSelect::MoveToCharacterSelectUI()
{
	SetShowServerStateButtonSet ( true, false );
	ClearUnitButton( false );
	ChangeUnitButtonInfo();
	ClearChannelButton ();
	if( NULL != m_pUnitViewerUI )
		m_pUnitViewerUI->PlayByMotionType( CX2UnitViewerUI::UVUMT_WAIT );
	m_bPlayDownAnim = false;
}
bool CX2StateServerSelect::Handler_EGS_CHARACTER_LIST_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_CHARACTER_LIST_REQ );
	g_pMain->AddServerPacket( EGS_CHARACTER_LIST_ACK );
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );

	m_pDLGMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_720 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_No_Button.lua", D3DXVECTOR2 (0, 0), L"UI_PopUp_Positive_01.ogg" );
#ifdef FIX_REFORM_ENTRY_POINT_10TH		// kimjh, ????? ????? ???????? ?????? ????
	m_pDLGWaitingCharacterList = m_pDLGMsgBox;
#endif // FIX_REFORM_ENTRY_POINT_10TH	// kimjh, ????? ????? ???????? ?????? ????
	return true;
}


bool CX2StateServerSelect::Handler_EGS_CHARACTER_LIST_ACK ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHARACTER_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( true == g_pMain->DeleteServerPacket( EGS_CHARACTER_LIST_ACK ) )
	{

		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			{
				ClearBeforeUnit( m_pSelectUnit );
				ClearUnitButton();

				SetShowSelectUnitInfo( NULL );
				if ( NULL != g_pData->GetUIMajorXMeshPlayer() )
				{
					for( CKTDGXMeshPlayer::CXMeshInstanceHandle hStandMeshInst : m_vecStandMeshInst )
					{
						g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( hStandMeshInst );
					}
					m_vecStandMeshInst.clear();
				}
				if( NULL != g_pData->GetUIEffectSet() )
				{
					g_pData->GetUIEffectSet()->StopEffectSet(m_hStandEffect);
				}

				g_pData->GetMyUser()->DeleteAllUnit();
				g_pInstanceData->ClearUserUnitDataInServer ();

				typedef std::map< int, int > ServerGroupUnitMaxMany;
				map < SEnum::SERVER_GROUP_ID, int > mapServerMaxUnitNumber;
				int iMaxUnitNumberAllServerGroup = 0;
				for( const ServerGroupUnitMaxMany::value_type& UnitSlotData : kEvent.m_mapServerGroupUnitSlot )
				{
					switch ( UnitSlotData.first )
					{
					case SEnum::SGI_SOLES :
						mapServerMaxUnitNumber.insert( make_pair( SEnum::SGI_SOLES , UnitSlotData.second ) );
						break;
					case SEnum::SGI_GAIA :
						mapServerMaxUnitNumber.insert( make_pair( SEnum::SGI_GAIA , UnitSlotData.second ) );
						break;
					}
					iMaxUnitNumberAllServerGroup += UnitSlotData.second;
				}


				typedef std::map< int, std::vector<KUnitInfo>  > ServerGroupUnitNowMany;
				map < SEnum::SERVER_GROUP_ID, int > mapServerNowUnitNumber;
				int iNowUnitNumberAllServerGroup = 0;
				for( const ServerGroupUnitNowMany::value_type& UnitInfo : kEvent.m_mapServerGroupUnitInfo )
				{
					int iGroupInfoSize = static_cast<int> ( UnitInfo.second.size() );
					switch ( UnitInfo.first )
					{
					case SEnum::SGI_SOLES :
						mapServerNowUnitNumber.insert( make_pair( SEnum::SGI_SOLES , iGroupInfoSize ) );
						break;
					case SEnum::SGI_GAIA :
						mapServerNowUnitNumber.insert( make_pair( SEnum::SGI_GAIA , iGroupInfoSize ) );
						break;
					}
					iNowUnitNumberAllServerGroup += iGroupInfoSize;
				}


				typedef std::map< int, vector<KUnitInfo> >				ServerGroupUnitInfoBeforeSort;
				typedef std::multimap< UnitInfoAddData, KUnitInfo, greater<UnitInfoAddData> >		ServerGroupUnitInfoAfterSort;

				ServerGroupUnitInfoAfterSort mapServerGroupUnitInfo;

				for( const ServerGroupUnitInfoBeforeSort::value_type& UnitinfoData : kEvent.m_mapServerGroupUnitInfo )
				{
					for( int i =0; i < (int)UnitinfoData.second.size(); i++ )
					{
						CTime ctLastLoginTime;
						KncUtil::ConvertStringToCTime( UnitinfoData.second[i].m_wstrLastDate, ctLastLoginTime );

						double dwLastLoginTime = KncUtil::TimeToInt( ctLastLoginTime );
						SEnum::SERVER_GROUP_ID eServerGroupID = static_cast<SEnum::SERVER_GROUP_ID> ( UnitinfoData.first );

						UnitInfoAddData sUnitInfoAddData ( dwLastLoginTime, eServerGroupID );
						mapServerGroupUnitInfo.insert ( std::make_pair( sUnitInfoAddData, UnitinfoData.second[i] ) );
					}
				}

				for( const ServerGroupUnitInfoAfterSort::value_type & ServerGroupUnitInfo : mapServerGroupUnitInfo )
				{
					KUnitInfo kUnitInfo = ServerGroupUnitInfo.second;
					CX2Unit* pUnit = new CX2Unit( kUnitInfo );
					pUnit->SetServerGroupID ( ServerGroupUnitInfo.first.m_eServerGroupID );
					pUnit->SetOwnerUserUID( g_pData->GetMyUser()->GetUID() );
					wstring nickNameToChange = L"__DELETED__";
					if ( nickNameToChange == pUnit->GetUnitData().m_NickName )
					{
						wstringstream tempNickName;
						tempNickName << nickNameToChange.c_str(); // << i;
						pUnit->AccessUnitData().m_NickName = GET_STRING(STR_ID_29404); //????? ????
					}
					g_pData->GetMyUser()->AddUnit( pUnit );
				}


				m_MaxUnitNum	= iMaxUnitNumberAllServerGroup;
				if( m_bMaintainNowUnitButton == false )
				{
					m_NowPage = 1;
				}
				const int SLOT_COUNT_FOR_CREATE_UNIT = 1;

				if( iNowUnitNumberAllServerGroup < m_MaxUnitNum )
				{
					m_MaxPage		= (iNowUnitNumberAllServerGroup + SLOT_COUNT_FOR_CREATE_UNIT+UNIT_SELECT_UI_MAX_UNIT_NUM-1) / UNIT_SELECT_UI_MAX_UNIT_NUM;
				}
				else
				{
					m_MaxPage		= (iNowUnitNumberAllServerGroup + UNIT_SELECT_UI_MAX_UNIT_NUM - 1 ) / UNIT_SELECT_UI_MAX_UNIT_NUM;
				}

				typedef std::map< SEnum::SERVER_GROUP_ID, int > ServerGroupUnitMany;



				for( const ServerGroupUnitMany::value_type & UnitManyMaxData : mapServerMaxUnitNumber )
				{
					ServerGroupUnitMany::const_iterator cit = mapServerNowUnitNumber.find ( UnitManyMaxData.first );
					if ( cit != mapServerMaxUnitNumber.end() )
					{
						g_pInstanceData->AddUserUnitDataInServer ( UnitManyMaxData.first, cit->second, UnitManyMaxData.second );
					}
					else
					{
						ASSERT ( !L"Can't Find Server Group ID !");
					}
				}

				CreateUnitButtonNew();
				ClearUnitButton();
				ChangeUnitButtonInfo();
				int iTopCharacterIndex = (m_NowPage - 1) * UNIT_SELECT_UI_MAX_UNIT_NUM;
				int iSelectedCharacterIndex = 0;
				CX2Unit* pUnit = NULL;

				if( m_bMaintainNowUnitButton == false )
				{
					pUnit = g_pData->GetMyUser()->GetUnitByIndex( iTopCharacterIndex );
				}
				else
				{
					iSelectedCharacterIndex = g_pData->GetMyUser()->GetIndexByUID( m_iSelectUnitUID );

					if( iSelectedCharacterIndex < 0 )
					{
						iSelectedCharacterIndex = iTopCharacterIndex;
					}

					pUnit = g_pData->GetMyUser()->GetUnitByIndex( iSelectedCharacterIndex );
				}
				if ( NULL != pUnit )
				{
					CreateServerSelectUnitViewerUI ( pUnit );
					if( m_bMaintainNowUnitButton == false )
					{
						SetFocusUnitButtonByIndex( 0 );
					}
					else
					{
						SetFocusUnitButtonByIndex( iSelectedCharacterIndex );
					}
					m_pSelectUnit = pUnit;
					SetRightSideFlagColor ( GetUnitKeyColorByUnitType ( m_pSelectUnit->GetType() ) );
					SetShowSelectUnitInfo ( pUnit );
					ChangeSelectUnitStandMeshInstance ( pUnit->GetType () );
					SetBackgroundImageBySelectUnitType ( pUnit->GetType() );

					SAFE_DELETE_DIALOG ( m_pDLGShowBeforeConnect );
				}
				SetShowServerStateButtonSet ( true, false );
				m_bReceivedEGS_MY_UNIT_AND_INVENTORY_INFO_LIST_ACK = true;

			}
			m_bMaintainNowUnitButton = false;
			m_iSelectUnitUID = 0;
#ifdef FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
			g_pInstanceData->ResetConnectedChannelID();
#endif // FIX_REFORM_ENTRY_POINT_10TH // kimjh, ????? ????? ???????? ?????? ????
		}
		else
		{
			g_pData->DeleteServerProtocol();
			g_pData->DeleteMyUser();
		}
	}
	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	return true;
}

void CX2StateServerSelect::SetBackgroundImageBySelectUnitType ( CX2Unit::UNIT_TYPE eUnitType_ )
{
	if ( NULL != m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" ) &&
		 NULL != m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0) )
	{
		m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( L"DLG_Unit_Background.tga", L"Unit_Background" );
	}
	m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" )->SetEnableLeftTop( D3DXVECTOR2 ( 0.f, 0.f ) );
	wstring wstrFileName = L"";
	switch ( eUnitType_ )
	{
		case CX2Unit::UT_NONE :
			return ;
		case CX2Unit::UT_ELSWORD :
			wstrFileName = L"DLG_Unit_Background_elsword.tga";
			break;
		case CX2Unit::UT_LIRE :
			wstrFileName = L"DLG_Unit_Background_rena.tga";
			break;
		case CX2Unit::UT_ARME :
			wstrFileName = L"DLG_Unit_Background_aisha.tga";
			break;
		case CX2Unit::UT_RAVEN :
			wstrFileName = L"DLG_Unit_Background_raven.tga";
			break;
		case CX2Unit::UT_EVE :
			wstrFileName = L"DLG_Unit_Background_eve.tga";
			break;
		case CX2Unit::UT_CHUNG :
			wstrFileName = L"DLG_Unit_Background_chung.tga";
			break;

		case CX2Unit::UT_ARA :
			wstrFileName = L"DLG_Unit_Background_ara.tga";
			break;

		case CX2Unit::UT_ELESIS :
			wstrFileName = L"DLG_Unit_Background_elesis.tga";
			break;

		case CX2Unit::UT_ADD :
			wstrFileName = L"DLG_Unit_Background_add.tga";
			break;
	}

	if ( NULL != m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" ) &&
		 NULL != m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0) )
	{
		m_pDLGUnitSelectFront->GetStatic_LUA( "Unit_Select_background" )->GetPicture(0)->SetTex ( wstrFileName.c_str() );
	}

}


void CX2StateServerSelect::CreateUnitButtonNew ()
{

	CKTDGUIDialogType pUnitSlot = NULL;
	CKTDGUIButton* pButton = NULL;

	for(UINT i=0; i<m_vecUnitSlot.size(); ++i)
	{
		CKTDGUIDialogType pUnitSlot =  m_vecUnitSlot[i];
		SAFE_DELETE_DIALOG(pUnitSlot);
	}
	m_vecUnitSlot.resize(0);


	for( int j = 0; j < UNIT_SELECT_UI_MAX_UNIT_NUM; j++ )
	{
		pUnitSlot = new CKTDGUIDialog( this, L"DLG_UI_Character_Selection_Slot_New.lua" );

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pUnitSlot );
		pUnitSlot->SetShowEnable(true, true);
		m_vecUnitSlot.push_back(pUnitSlot);
	}
}
void CX2StateServerSelect::ChangeUnitButtonInfo ()
{

	if ( NULL != m_pDLGUnitSelectFront && true != m_vecUnitSlot.empty() )
	{
		CKTDGUIStatic* pStaticUnitPage = static_cast<CKTDGUIStatic*> ( m_pDLGUnitSelectFront->GetControl( L"number" ) );
		if ( NULL != pStaticUnitPage )
		{
			WCHAR buff[256] = {0,};
			StringCchPrintf( buff, 256, L"%d / %d", m_NowPage, m_MaxPage );
			pStaticUnitPage->GetString(0)->msg = buff;
		}


		m_bIsAbledStateCreateUnitButton = true;
		if ( g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_SOLES ) <= g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_SOLES ) )
		{
			m_bIsAbledStateCreateUnitButton = false;
		}
		else if ( m_iCreateUnitTodayCount >= TODAY_CREATE_UNIT_MAX_NUMBER )
		{
			m_bIsAbledStateCreateUnitButton = false;
		}

		bool bIsNoCreateUnitButtonDrow = false;
		int i = (m_NowPage - 1)*UNIT_SELECT_UI_MAX_UNIT_NUM;
		int j = 0;
		for( ; j < UNIT_SELECT_UI_MAX_UNIT_NUM; i++, j++ )
		{
			CX2Unit* pUnit = g_pData->GetMyUser()->GetUnitByIndex( i );

			CKTDGUIDialogType pUnitSlot = m_vecUnitSlot[j];

			D3DXVECTOR3 dummyPos = m_pDLGUnitSelectFront->GetDummyPos( j );
			D3DXVECTOR2 buttonPos = D3DXVECTOR2( dummyPos.x, dummyPos.y );
			pUnitSlot->SetPos(buttonPos);

			CKTDGUIButton* pSelectUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 0 ) );
			CKTDGUIButton* pCreateUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 1 ) );
			CKTDGUIButton* pNoCreateUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 2 ) );
			CKTDGUIButton* pEmptyUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 3 ) );
			CKTDGUIButton* pFullUnitButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 4 ) );

			if ( NULL != pSelectUnitButton && NULL != pCreateUnitButton && NULL != pNoCreateUnitButton && NULL != pEmptyUnitButton && NULL != pFullUnitButton )
			{
				pSelectUnitButton->SetDownStateAtNormal( false );

				if( NULL == pUnit )
				{
					if ( false == bIsNoCreateUnitButtonDrow )
					{
						if ( g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_GAIA ) <= g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_GAIA )
							&& g_pInstanceData->GetMaxUserUnitManyInServer( SEnum::SGI_SOLES ) <= g_pInstanceData->GetNowUserUnitManyInServer( SEnum::SGI_SOLES ) )
						{
							pFullUnitButton->SetShowEnable ( true, true );
						}
						else if ( m_iCreateUnitTodayCount >= TODAY_CREATE_UNIT_MAX_NUMBER )
						{
							pNoCreateUnitButton->SetShowEnable( true, true );
						}
						else
						{
							pCreateUnitButton->SetShowEnable ( true, true );
						}

						bIsNoCreateUnitButtonDrow = true;
					}
					else
					{
						pEmptyUnitButton->SetShowEnable( true, true );
					}

					continue;
				}
				else
				{
					pSelectUnitButton->SetShowEnable( true, true );
				}
			}


			switch( pUnit->GetType() )
			{
			case CX2Unit::UT_ELSWORD:
			case CX2Unit::UT_LIRE:
			case CX2Unit::UT_ARME:
			case CX2Unit::UT_RAVEN:
			case CX2Unit::UT_EVE:

			case CX2Unit::UT_CHUNG:

			case CX2Unit::UT_ARA:

			case CX2Unit::UT_ELESIS:

			case CX2Unit::UT_ADD:
				break;


			default:
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375), GET_STRING( STR_ID_707 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 (0, 0),L"UI_PopUp_Negative_01.ogg" );
				}break;
			}
			XSkinMeshReady( pUnit->GetUnitTemplet()->m_MotionFile.c_str() );


			wstringstream buttonName;
			buttonName << L"SLOT_BUTTON_" << pUnit->GetUID();
			if( NULL != pSelectUnitButton )
			{
				pSelectUnitButton->SetName( buttonName.str().c_str() );
			}

			CKTDGUIStatic* pStaticLV = (CKTDGUIStatic*)pUnitSlot->GetControl( L"lv" );
			if ( NULL != pStaticLV)
			{
				pStaticLV->SetShow ( true );
			}

			WCHAR buff[256] = {0,};
			StringCchPrintf( buff, 256, L"%d", static_cast<int> ( pUnit->GetUnitData().m_Level ) );
			CKTDGUIStatic* pStaticLVNum = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"StaticUnitSelectStringLVNum" ) );

			if ( NULL != pStaticLVNum )
			{
				pStaticLVNum->SetShow ( true );
				pStaticLVNum->GetString(0)->msg = buff;
			}

			CKTDGUIStatic* pStaticID = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"StaticUnitSelectStringID" ) );
			if ( NULL != pStaticID )
			{
				pStaticID->SetShow ( true );
			}

			CKTDGUIStatic* pStaticChangeNickEmblem = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"ChangeNick" ) );


			CKTDGUIStatic* pStaticChangeNickMessage = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"standby" ) );

			if ( pStaticChangeNickEmblem != NULL && pStaticChangeNickMessage != NULL )
			{
				wstring tempNickName = pUnit->GetNickName();

				if ( tempNickName.find( L"__DELETED__") != -1
					|| tempNickName.find( GET_STRING(STR_ID_29404) ) != -1
					)
				{
					if( NULL != pStaticID )
					{
						pStaticID->GetString(0)->msg = GET_STRING(STR_ID_29412);
						pStaticID->SetOffsetPos(D3DXVECTOR2(-22,-3));
					}

					pStaticChangeNickEmblem->SetShowEnable( true, true );
					if( NULL != pStaticLVNum && NULL != pStaticLV )
					{
						pStaticLVNum->SetShow ( false );
						pStaticLV->SetShow ( false );
					}
				}
				else
				{
					pStaticID->GetString(0)->msg = pUnit->GetNickName();
					pStaticID->SetOffsetPos(D3DXVECTOR2(0,0));
					pStaticChangeNickEmblem->SetShowEnable( false, false );
					pStaticChangeNickMessage->SetShowEnable( false, false );
				}
			}
			CKTDGUIStatic* pStaticLastPlace = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"StaticUnitSelectStringID_LastPlace" ) );
			if ( NULL != pStaticLastPlace )
			{
				pStaticLastPlace->SetShow ( true );

				wstring wstringLastPlace = L"";
				if ( pUnit->GetServerGroupID() == SEnum::SGI_GAIA )
				{
					wstringLastPlace = L"#C007000";
					wstringLastPlace += GET_STRING(STR_ID_4835);
					wstringLastPlace += L"#CX";
					wstringLastPlace += L" : ";
				}
				else
				{
					wstringLastPlace = L"#CFF0000";
					wstringLastPlace += GET_STRING(STR_ID_4836);
					wstringLastPlace += L"#CX";
					wstringLastPlace += L" : ";
				}

				if ( NULL != g_pData->GetLocationManager() )
				{
					int iMapID = pUnit->GetUnitData().m_nMapID;

					wstring wstrLastPlace = L"";

					if ( iMapID >= SEnum::VMI_BATTLE_FIELD_RUBEN_FIELD_01 && SEnum::VMI_BATTLE_FIELD_END > iMapID )
					{
						CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
						wstrLastPlace = battleFieldManager.GetBattleFieldNameByBattleFieldId( iMapID );
					}

					else if ( iMapID == SEnum::VMI_TUTORIAL )
					{
						wstrLastPlace = GET_STRING(STR_ID_2761);
					}
					else
					{
						CX2LocationManager::VillageTemplet* pVillageTemplet = g_pData->GetLocationManager()->GetVillageMapTemplet( (SEnum::VILLAGE_MAP_ID) iMapID );
						if ( NULL != pVillageTemplet )
						{
							wstrLastPlace = pVillageTemplet->m_Name;
						}

					}

					wstringLastPlace += wstrLastPlace.c_str();
					if ( wstringLastPlace.length() >= 40 )
					{
						wstringLastPlace = wstringLastPlace.substr(0, 38);
						wstringLastPlace += L"..";
					}
					pStaticLastPlace->GetString(0)->msg = wstringLastPlace.c_str();
				}



			}

			CKTDGUIStatic* pStaticUnitFace = static_cast<CKTDGUIStatic*> ( pUnitSlot->GetControl( L"UnitFace" ) );
			if ( NULL != pStaticUnitFace )
			{
				pStaticUnitFace->SetShow ( true );
				wstring fileName, pieceName;
				if ( true == CX2Data::GetCharacterImageName( fileName, pieceName, pUnit->GetClass(), CX2Data::CIT_UnitSlot ) )
				{
					CKTDGUIControl::CPictureData* pPictureData = pUnitSlot->CreatePicture();
					pStaticUnitFace->AddPicture ( pPictureData );
					pPictureData->SetTex ( fileName.c_str(), pieceName.c_str() );
					CKTDGUIControl::UIPointData* pPointData = new CKTDGUIControl::UIPointData();
					pPointData->leftTopPoint = D3DXVECTOR2 ( 23, 11 );
					if( pUnit->GetUnitData().m_bDeleted == true )
					{
						pPointData->color = D3DXCOLOR ( 0.5f, 0.5f, 0.5f, 0.5f );
					}
					else
					{
						pPointData->color = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 1.0f );
					}
					pPictureData->SetPoint ( pPointData );
					pPictureData->SetSize( D3DXVECTOR2 ( 38, 38 ) );
				}
			}

			if ( pUnit == m_pSelectUnit )
			{
				CKTDGUIButton* pCharButton = reinterpret_cast<CKTDGUIButton*>( pUnitSlot->GetControl( 0 ) );
				pCharButton->SetDownStateAtNormal( true );
			}
		}
		PlayDownAnim();
	}
}

bool CX2StateServerSelect::Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_CREATE_UNIT_TODAY_COUNT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK ) == true )
	{
		typedef std::map< int, int > TodayCreateUnitCount;

		for( const TodayCreateUnitCount::value_type& Count : kEvent.m_mapCreateCharCountToday )
		{
			switch ( Count.first )
			{
				case SEnum::SGI_SOLES :
					m_iCreateUnitTodayCount += Count.second;
					break;

				case SEnum::SGI_GAIA :
					m_iCreateUnitTodayCount += Count.second;
					break;

				default :
					break;
			}
		}
	}
	return true;
}

bool CX2StateServerSelect::Handler_EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ ()
{
	g_pData->GetServerProtocol()->SendID( EGS_GET_CREATE_UNIT_TODAY_COUNT_REQ );
	g_pMain->AddServerPacket( EGS_GET_CREATE_UNIT_TODAY_COUNT_ACK );
	return true;
}


void CX2StateServerSelect::Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ ()
{
	g_pData->GetServerProtocol()->SendID( EGS_ENTRY_POINT_GET_CHANNEL_LIST_REQ );
	g_pMain->AddServerPacket( EGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK );
}
bool CX2StateServerSelect::Handler_EGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ENTRY_POINT_GET_CHANNEL_LIST_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG ( m_pDLGMsgBox );
	ClearUnitButton ( false );

//#pragma region Auto Keyword
	//////////////////////////////////////////////////////////////////////////
	// 1. ????? �?? ???????,
	// 1. �?? ????? ?????? ??????, ??� ?????? ?????? �???? ID???
	auto SetChannelBonusInfo = [](  IN const int iSelectedUnitLevel_,
									IN const std::map< int, KChannelBonusInfo >& kPacketChannelBonus_,
									OUT vector<int>& vecRecommandedLevelChannelID_,
									OUT map< int, KChannelBonusInfo >& mapChannelBonusInfo_ )
	{
		std::map< int, KChannelBonusInfo >::const_iterator mitChannelBonusInfo = // key : �?? ID, second : ????? ????
			kPacketChannelBonus_.begin();

		for( ; mitChannelBonusInfo != kPacketChannelBonus_.end(); ++mitChannelBonusInfo )
		{
			const int& iChannelID = mitChannelBonusInfo->first;

			if ( mitChannelBonusInfo->second.m_iBeginLv <= iSelectedUnitLevel_ && iSelectedUnitLevel_ <= mitChannelBonusInfo->second.m_iEndLv )
				vecRecommandedLevelChannelID_.push_back( iChannelID );

			/*# TODO : ??? ?????? ??????? ????.
			???? ??? 1 ) ??� ?????? ????? ?????? ??? ??????? ????.
						2 ) ??� ?????? ????? ???? ??? ???? , ???? ???? ??????.*/

			g_pInstanceData->m_mapChannelBonusInfo.insert( std::make_pair(iChannelID, mitChannelBonusInfo->second) );
			mapChannelBonusInfo_.insert( std::make_pair ( iChannelID, mitChannelBonusInfo->second ) );
		}

	};

	//////////////////////////////////////////////////////////////////////////
	// �?? ????? ????(??� ?????? ?????? �???? ?????? ???)
	auto SetChannelList = []( IN const std::map< int, KChannelInfo >& kPacketChannelList_,
							IN const vector<int>& vecRecommandedLevelChannelID_,
							OUT vector< KChannelInfo >& vecChannelInfo )
	{

		// #1 ??� ?????? ?????? �?? ???? ??????
		std::map< int, KChannelInfo >::const_iterator mitChannelInfo;
		for( const int& iChannelID : vecRecommandedLevelChannelID_ )
		{
			mitChannelInfo = kPacketChannelList_.find( iChannelID );
			if( mitChannelInfo != kPacketChannelList_.end() )
				vecChannelInfo.push_back( mitChannelInfo->second );
		}

		// #2 ??� ???? ??? �?? ??????

		mitChannelInfo = kPacketChannelList_.begin();
		for( ; mitChannelInfo != kPacketChannelList_.end(); ++ mitChannelInfo )
		{
			bool bAredyInsert = false;
			for( const int& iChannelID : vecRecommandedLevelChannelID_ )
			{
				if( iChannelID == mitChannelInfo->first )
					bAredyInsert = true;
			}

			if( false == bAredyInsert )
				vecChannelInfo.push_back( mitChannelInfo->second );

			// ?????? ?? ??? ?????? g_pInstanceData->m_vecChannelInfo?? ???? �??
			g_pInstanceData->m_vecChannelInfo.push_back( mitChannelInfo->second );
		}
	};
//#pragma endregion Auto Keyword

	SEnum::SERVER_GROUP_ID eSelectedCharacterServerGroupID = SEnum::SGI_INVALID;
	if ( NULL != m_pSelectUnit )
	{
		eSelectedCharacterServerGroupID = m_pSelectUnit->GetServerGroupID();
	}
	else
	{
		// ?????? ???�??? ????? ?? ??? ???? ??? ?? ??.
		return true;
	}
	ASSERT( SEnum::SGI_INVALID != eSelectedCharacterServerGroupID );

	InitChannelInfo();

	vector<int> vecRecommandedLevelChannelID;
	switch( eSelectedCharacterServerGroupID )
	{
	case SEnum::SGI_SOLES:
		{
			// �?? ????? ?????? ??????, ??� ?????? ?????? �???? ID???
			// m_pSelectUnit ???? ??�???? ?????? �??
			SetChannelBonusInfo( m_pSelectUnit->GetUnitData().m_Level, kEvent.m_mapSolesChannelBonusList,
								vecRecommandedLevelChannelID, m_mapSolesChannelBonusInfo );

			// ??� ???? �???? ?????? ??? ??? �?? ??? ???
			m_vecSolesChannelInfo.reserve( kEvent.m_mapSolesChannelList.size() );
			SetChannelList( kEvent.m_mapSolesChannelList, vecRecommandedLevelChannelID, m_vecSolesChannelInfo );
		} break;
	case SEnum::SGI_GAIA:
		{
			// �?? ????? ?????? ??????, ??� ?????? ?????? �???? ID???
			SetChannelBonusInfo( m_pSelectUnit->GetUnitData().m_Level, kEvent.m_mapGaiaChannelBonusList,
								vecRecommandedLevelChannelID, m_mapGaiaChannelBonusInfo );

			// ??� ???? �???? ?????? ??? ??? �?? ??? ???
			m_vecGaiaChannelInfo.reserve( kEvent.m_mapGaiaChannelList.size() );
			SetChannelList( kEvent.m_mapGaiaChannelList, vecRecommandedLevelChannelID, m_vecGaiaChannelInfo );
		} break;
	default:
		break;
	}

	m_bReserveEnterTutorial = false;
	m_bReservedConnectionToChannelServer = false;

	if( NULL != g_pInstanceData->GetMiniMapUI() )
		g_pInstanceData->GetMiniMapUI()->SetChannelComboBox();

	if( g_pMain->GetIsPlayingTutorial() == false )
		CreateChannelButton ( true );

	SetShowServerStateButtonSet ( false, true );


	return true;
}
void CX2StateServerSelect::InitChannelInfo()
{
	// �???? ????? ???? ????
	m_mapGaiaChannelBonusInfo.clear();
	m_mapSolesChannelBonusInfo.clear();
	g_pInstanceData->m_mapChannelBonusInfo.clear();
	// �?? ???? ????
	m_vecGaiaChannelInfo.clear();
	m_vecSolesChannelInfo.clear();
	g_pInstanceData->m_vecChannelInfo.clear();
}

bool CX2StateServerSelect::Handler_EGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ ( UidType uid, const WCHAR* wszNickName, bool bCheckOnly, SEnum::SERVER_GROUP_ID eGroupID )
{
	KEGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ kPacket;

	kPacket.m_iUnitUID		= uid;
	kPacket.m_wstrNickName	= wszNickName;
	kPacket.m_bCheckOnly	= bCheckOnly;
	kPacket.m_iServerGroup	= eGroupID;

	g_pData->GetServerProtocol()->SendPacket( EGS_ENTRY_POINT_CHANGE_NICK_NAME_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_CHANGE_NICK_NAME_ACK );
	return true;
}

bool CX2StateServerSelect::ChannelButtonUp( CKTDGUIButton * pButton )
{

	if(m_bEnableUnitSelectBySecondSecurity == false)
	{
		return true;
	}

	if ( NULL == m_pSelectUnit )
		return true;

	for ( int iChannel = 0;  iChannel < static_cast<int> ( m_vecChannelSlot.size() ); ++iChannel )
	{
		CKTDGUIDialogType pChannelSlot =  m_vecChannelSlot[iChannel];

		if ( NULL != pChannelSlot )
		{
			pChannelSlot->SetDownStateToAllButtonType ();
		}
	}

	if ( NULL != pButton )
		pButton->SetDownStateAtNormal( true );

	if ( m_pSelectChannelButton == pButton )
	{

		vector< KChannelInfo > * vecChannelInfo = NULL;
		if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_GAIA )
		{
			vecChannelInfo = &m_vecGaiaChannelInfo;
		}
		else if ( m_pSelectUnit->GetServerGroupID() == SEnum::SGI_SOLES )
		{
			vecChannelInfo = &m_vecSolesChannelInfo;
		}
		else
		{
			ASSERT (!L"Can't Find Server Group ID !! ");
		}

		int iCheckEntryPointServerChannelIndex = m_pSelectChannelButton->GetDummyInt( 0 );

		float fChannelCongestionRate =
			static_cast< float >(  (*vecChannelInfo)[ iCheckEntryPointServerChannelIndex ].m_iCurrentUser ) / static_cast< float >(  (*vecChannelInfo)[ iCheckEntryPointServerChannelIndex ].m_iMaxUser );

		if ( fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[3] )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(305, 375 ), GET_STRING ( STR_ID_29885 ), this, -1, -1.f, L"DLG_UI_Selection_MessageBox_Ok_Button_New.lua", D3DXVECTOR2 ( 0, 0 ), L"UI_PopUp_Negative_01.ogg" );
		}

		else
		{
			m_bReserveEnterTutorial = false;
			m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection = true;
			m_bSend_EGS_SELECT_UNIT_REQ_For_Auto_Connection = true;
			m_bENX_USER_LOGIN_NOT		= false;
			m_bEGS_VERIFY_ACCOUNT_REQ	= false;
			m_bEGS_VERIFY_ACCOUNT_ACK	= false;
			m_bPortCheckReq				= false;

			m_bSentEGS_CONNECT_REQ 		= false;
			m_bReceivedChannelList 		= false;
			m_bWaitingEGS_CONNECT_REQ_ChannelButtonUp = true;

			if ( NULL != pButton )
			{
				int iChIndex = pButton->GetDummyInt ( 0 );// - 1 ) + ( m_iChannelNowPage - 1 ) * CHANNEL_SELECT_UI_MAX_CHANNEL_NUM;
				m_iReservedEntryPointServerChannelIndex = iChIndex;
			}


			// ??????????? ?????? ???�?

			Handler_EGS_DISCONNECT_FOR_SERVER_SELECT_REQ();
		}
	}
	else
	{
		m_pSelectChannelButton = pButton;
	}
	return true;
}
void CX2StateServerSelect::ConnectToLowerUserCountGameServer ( bool bIsConnectAnotherServerGroup /* = false */  )
{
	SEnum::SERVER_GROUP_ID eConnectingServerGroupID = SEnum::SGI_INVALID;
	if ( NULL != m_pSelectUnit )
	{
		eConnectingServerGroupID = m_pSelectUnit->GetServerGroupID();
	}
	else
	{
		eConnectingServerGroupID = static_cast<SEnum::SERVER_GROUP_ID> ( g_pInstanceData->GetServerGroupID() );
	}

	if ( true == bIsConnectAnotherServerGroup )
	{
		// ???????? 3?? ??? ?? ??, ??? ????? ??????? ????.
		if ( eConnectingServerGroupID == SEnum::SGI_GAIA)
			eConnectingServerGroupID = SEnum::SGI_SOLES;
		else if ( eConnectingServerGroupID == SEnum::SGI_SOLES)
			eConnectingServerGroupID = SEnum::SGI_GAIA;
		else
			ASSERT ( !L"Can not Find Server Group ID !");
	}
	ASSERT ( eConnectingServerGroupID != SEnum::SGI_INVALID );

	if ( SEnum::SGI_GAIA == eConnectingServerGroupID )
	{
		FindAndConnectLowerUserCountGameServer ( m_vecGaiaChannelInfo );
	}

	else if ( SEnum::SGI_SOLES == eConnectingServerGroupID )
	{
		FindAndConnectLowerUserCountGameServer ( m_vecSolesChannelInfo );
	}
	else
	{
		ASSERT( !L"Can't Find Server Group ID !");
	}


}

void CX2StateServerSelect::PlaySoundSelect( CX2Unit::UNIT_TYPE eUnitType_, SOUND_PLAY_TYPE eSoundPlayType_ )
{
	if( g_pKTDXApp->GetDeviceManager() == NULL )
		return;

	wstring wstrSoundName = L"";

	switch( eUnitType_ )
	{
	case CX2Unit::UT_ELSWORD:
		{
			wstrSoundName = L"Elsword";
		} break;
	case CX2Unit::UT_ARME:
		{
			wstrSoundName = L"Aisha";
		} break;
	case CX2Unit::UT_LIRE:
		{
			wstrSoundName = L"Rena";
		} break;
	case CX2Unit::UT_RAVEN:
		{
			wstrSoundName = L"Raven";
		} break;
	case CX2Unit::UT_EVE:
		{
			wstrSoundName = L"Eve";
		} break;
	case CX2Unit::UT_CHUNG:
		{
			wstrSoundName = L"Chung";
		} break;
	case CX2Unit::UT_ARA:
		{
			wstrSoundName = L"Ara";
		} break;
	case CX2Unit::UT_ELESIS:
		{
			wstrSoundName = L"Elesis";
		} break;
	case CX2Unit::UT_ADD:
		{
			wstrSoundName = L"Add";
		} break;

	default:
		break;
	}

	switch( eSoundPlayType_)
	{
	case SPT_UNIT_SELECT:
		{
			wstrSoundName += L"Voice_Character_Select.ogg";
		} break;
	case SPT_SERVER_SELECT:
		{
			wstrSoundName += L"Voice_GameStart.ogg";
		} break;

	default:
		break;
	}

	if( StrCmp( wstrSoundName.c_str(), L"" ) != 0 )
	{
		if( NULL != m_pSound )
		{
			m_pSound->Stop();
			SAFE_CLOSE( m_pSound );
		}
		m_pSound = g_pKTDXApp->GetDeviceManager()->PlaySound(wstrSoundName.c_str(), false, false );
	}
}

/** @function : IsCharacterSelectUIMode
	@brief : ???? �????? �???? ???.
			 ????? ???? ??????? ??? ???? �???????? ??? ??
*/
bool CX2StateServerSelect::IsChannelSelectMode()
{
	// m_vecChannelSlot?? ??????? ??? ???�? �?? ????�???? ???? ??
	if( false == m_vecChannelSlot.empty() )
		return true;

	return false;
}

void CX2StateServerSelect::FindAndConnectLowerUserCountGameServer ( const vector< KChannelInfo > & vecChannelInfo )
{
	int iLowerUserChannelCount = INT_MIN;
	int iLowerUserChannelIndex = -1;
	int iLowerUserChannelID = -1;
	if ( false == vecChannelInfo.empty () )
	{
		int iIndex = 0;
		BOOST_FOREACH ( const KChannelInfo & kChannelInfo, vecChannelInfo )
		{
			if ( iLowerUserChannelCount < kChannelInfo.m_iMaxUser - kChannelInfo.m_iCurrentUser )
			{
#ifdef FIX_REFORM_ENTRY_POINT_10TH		//	kimjh,  ????? ????? ???????? ?????? ????
				if ( false == g_pInstanceData->IsConnectedChannel( kChannelInfo.m_iServerUID ) )
#endif // FIX_REFORM_ENTRY_POINT_10TH	//	kimjh,  ????? ????? ???????? ?????? ????
				{
					iLowerUserChannelCount = kChannelInfo.m_iMaxUser - kChannelInfo.m_iCurrentUser;
					iLowerUserChannelIndex = iIndex;
					m_iTryConnectChannelID = kChannelInfo.m_iChannelID;
					g_pMain->SetConnectedGameServerIP( kChannelInfo.m_wstrIP );
					g_pMain->SetConnectedGameServerPort( kChannelInfo.m_usMasterPort );

#ifdef FIX_REFORM_ENTRY_POINT_10TH		//	kimjh,  ????? ????? ???????? ?????? ????
					g_pInstanceData->SetConnectedChannelID( kChannelInfo.m_iServerUID  );
#endif // FIX_REFORM_ENTRY_POINT_10TH	//	kimjh,  ????? ????? ???????? ?????? ????
				}
			}
			++iIndex;
		}
		if ( iLowerUserChannelIndex < 0 || iLowerUserChannelCount < INT_MIN )
		{
			ASSERT ( L"Can't Find Channel Index !");
			m_bReConnectChannelServer = true;
			return ;
		}

		m_bSelectChannel = true;
		m_bSentEGS_CONNECT_REQ = false;
		m_bSend_EGS_SELECT_UNIT_REQ_For_Auto_Connection = false;
		m_bSend_EGS_DISCONNECT_FOR_SERVER_SELECT_ACK_For_Auto_Connection = false;
		m_sbSelectedServerSet = false;


		g_pData->GetServerProtocol()->DisconnectFromGameServer();
		g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_SERVER_SELECT, NULL, false );
	}
}


int CX2StateServerSelect::FindAndConnectMatchingLevelGameServer ( int iMatchingUnitLevel, const map<int, KChannelBonusInfo > & mapChannelBonusInfo, const vector< KChannelInfo > & vecChannelInfo )
{
	if ( false == mapChannelBonusInfo.empty () )
	{
		vector <int> vecRecommandedLevelChannelID;

		std::map< int, KChannelBonusInfo >::const_iterator cmit;
		for( cmit = mapChannelBonusInfo.begin() ; cmit != mapChannelBonusInfo.end(); ++cmit )
		{
			if ( cmit->second.m_iBeginLv <= iMatchingUnitLevel && iMatchingUnitLevel <= cmit->second.m_iEndLv )
				vecRecommandedLevelChannelID.push_back ( cmit->first );
		}
		if ( true == vecRecommandedLevelChannelID.empty() )
			return -1;

		int iLowerUserChannelCount = INT_MIN;
		int iPickedChannelID = -1;

		BOOST_FOREACH ( const KChannelInfo & kChannelInfo, vecChannelInfo )
		{
			BOOST_FOREACH ( const int & iRecommandedLevelChannelID, vecRecommandedLevelChannelID )
			{
				if ( iRecommandedLevelChannelID == kChannelInfo.m_iChannelID )
				{
					if ( iLowerUserChannelCount < kChannelInfo.m_iMaxUser - kChannelInfo.m_iCurrentUser )
					{
						iLowerUserChannelCount = kChannelInfo.m_iMaxUser - kChannelInfo.m_iCurrentUser;
						iPickedChannelID = iRecommandedLevelChannelID;
					}
				}
			}
		}
		return iPickedChannelID;
	}
	return -1;
}



void CX2StateServerSelect::ProcessIntroMovieEnd ()		// ????? ???? ( ??? ??? ?????? ?? ) ?? ?????? ???? �??
{
	// ???? ??? ?????? ??? ????? ??�???? ???.
	if ( true == m_bReservedSecurityPad )
	{
		m_bReservedSecurityPad = false;
		g_pMain->GetSecurityPad()->SetShow(true, CX2SecurityPad::SPS_GAME_START);
		g_pMain->GetSecurityPad()->SetUseSecondPW(true);
	}

	// ??? ???? ???? ????�?? ?????? ??? ????? ??�???? ???.
	if ( true == m_bReservedHackingUserAgreeDlg )
	{
		m_bReservedHackingUserAgreeDlg = false;
		m_pDLGCheckHack = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(305, 375),
			GET_STRING( STR_ID_729 ),
			SSSUCM_HACK_USER_PROCESS_CHECK_OK, this, SSSUCM_HACK_USER_PROCESS_CHECK_CANCEL, L"DLG_UI_Selection_MessageBox_Ok_Exit_Button_New.lua", D3DXVECTOR2( 0, 10 ), L"UI_PopUp_Negative_01.ogg" );

		// MsgBox ?? ESC ?? ?????? ????? ?????? ???, ?????? ?? �?? ??? ???????? ?????
		// ??? ???? ???? false ?? ???????.	kimjh
		if ( NULL != m_pDLGCheckHack )
			m_pDLGCheckHack->SetOKAndCancelMsgBox( false );

	}
}
