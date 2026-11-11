#include "stdafx.h"
#include "x2/platform/process.h"
#include "./X2StateMenu.h"


CX2StateMenu::CX2StateMenu(void) :
// 2009.01.09 김태완 : 초기화 코드정리
m_pDLG2014Confirm( NULL ),
m_pDLGMenu(NULL),
m_pDLGMenuWindow(NULL),
m_pDLGFindUser(NULL),
m_pDLGUserInfo(NULL),
m_pDLGF1Help(NULL),
m_bShowWindowMenu(false),
m_bShowSystemMenu(false),
m_pCashShop(NULL),
m_wstrFindUserName(L""),
m_FindUserUid(0),
m_pDLGInviteNot(NULL),
m_InviterName(L""),
m_eSearchUnitConnectionState(CX2Unit::CUS_NONE),
m_iSearchUnitChannelID(0),
m_iSearchUnitDungeonID(0),
//m_mapHideMouseClickUI;
m_iSearchUnitRoomUID(0),
m_fTimeRefreshMenuInfo(0.0f),
m_CashItemPage(1),
m_pDLGCashItemUpdate(NULL),
m_pDLGPresentArrival(NULL),
m_pDLGSystem(NULL),
m_bShowMenu(true),
m_pDLGLeavePartyOrLounge( NULL ),
m_pDLGGuideBookButton( NULL ),
m_pPicCharMenuPlusInfo(NULL),
m_pPicCharMenuPlusInfoRed(NULL),
m_TotalExp(0),
m_TotalED(0),
m_TotalLevel(0)
, m_TotalAP(0)
, m_fMenuMaxRemainTime( 6.f )
, m_bShowMainMenuButton(true)
, m_bShowUI ( true )
, m_pDLGWarpDestination( NULL )
, m_iSelectedWarpDest(0)
,m_SumDelta(0)
,m_iSelectedWarpDestIndex(-1)
,m_iCurrentVillageWarpIndex(-1)
,m_bIsEdConsumptionWarp(false)
//}}
,m_fTimeCheckWarpVip(0.f)
{
	g_pKTDXApp->GetDGManager()->GetCamera().Point( 0,0,-1300, 0,0,0 );
	g_pKTDXApp->GetDGManager()->SetProjection( g_pKTDXApp->GetDGManager()->GetNear(), g_pKTDXApp->GetDGManager()->GetFar(), false );

	InitUI();
	InitETC();

	// fix!! 일단은 메뉴가 있는 곳에서 파티UI가 항상 있다고 보고 없으면 새로 생성, 게임 들어갈 때 파티 UI삭제하는 것으로 처리해놓자
	if( NULL == g_pMain->GetPartyUI() )
	{
		g_pMain->ResetPartyUI();
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"MenuInfoPicChar", 0.0f, 0.0f, 0.0f,  0.0f, 0.0f );
	pSeq->SetOverUI( true );
	m_pPicCharMenuPlusInfo = new CKTDGPicChar( g_pData->GetPicChar(), pSeq );
	m_pPicCharMenuPlusInfo->SetWidth( 10 );

	pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"MenuInfoPicCharRed", 0.0f, 0.0f, 0.0f,  0.0f, 0.0f );
	pSeq->SetOverUI( true );
	m_pPicCharMenuPlusInfoRed = new CKTDGPicChar( g_pData->GetPicChar(), pSeq );
	m_pPicCharMenuPlusInfoRed->SetWidth( 10 );

	m_TotalExp = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_EXP;
	m_TotalED = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED;
	m_TotalLevel = g_pData->GetSelectUnitLevel();

	m_TotalAP = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint;
}

CX2StateMenu::~CX2StateMenu(void)
{
	//SAFE_DELETE_DIALOG( m_pDLGMenu );
	if ( m_pDLGMenu != NULL )
	{
		m_pDLGMenu->SetShow( false );
		m_pDLGMenu->SetEnable( false );
	}
	// 2009.01.29 김태완 : 퀵슬롯 SetShow 타이밍 바꿈
	g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUICK_SLOT, false);

	SAFE_DELETE_DIALOG( m_pDLGMenuWindow );
	SAFE_DELETE_DIALOG( m_pDLGFindUser );
	SAFE_DELETE_DIALOG( m_pDLGUserInfo );

	SAFE_DELETE_DIALOG( m_pDLGF1Help );
	SAFE_DELETE_DIALOG( m_pDLGCashItemUpdate );

	SAFE_DELETE_DIALOG( m_pDLGPresentArrival );

	SAFE_DELETE_DIALOG( m_pDLGSystem );
	SAFE_DELETE_DIALOG( m_pDLGLeavePartyOrLounge );
	SAFE_DELETE_DIALOG( m_pDLG2014Confirm );


	CX2Community* pMessenger = g_pData->GetMessenger();
	if ( pMessenger != NULL )
		pMessenger->SetOpen( false );

	if( NULL != g_pMain->GetCouponBox() )
	{
		g_pMain->GetCouponBox()->OpenCouponBoxDLG( false );
		g_pMain->GetCouponBox()->OpenCouponNumberInputDLG( false );
	}

	g_pMain->GetInformerManager()->CheckInfo( true );


	//{{ 09.03.18 김태완
	SAFE_DELETE_DIALOG(m_pDLGGuideBookButton);
	//}}
	SAFE_DELETE( m_pPicCharMenuPlusInfo );
	SAFE_DELETE( m_pPicCharMenuPlusInfoRed );
	m_vecpButtonMainMenu.clear();

	SAFE_DELETE_DIALOG( m_pDLGWarpDestination );
}

HRESULT CX2StateMenu::OnFrameMove( double fTime, float fElapsedTime )
{
	KLagCheck( eKnown_LagCheckType_StateMenu_FrameMove );
	KTDXPROFILE();
	CX2StateCommonBG::OnFrameMove( fTime, fElapsedTime );


    CheckAndSendingPlayStatus( fElapsedTime );



	//{{ 09.03.18 태완
	// 꼭 FrameMove에 들어가 있을 이유가 없는데..
	// 그런데 옵션에서 설정될 때 여기로 당기는 것도 좀 애매하긴 하다-_-;
	if( ( (g_pTFieldGame != NULL && false == g_pTFieldGame->GetJoinNpc() ) ||
		  (g_pSquareGame != NULL ) )							// NPC와 대화중이 아니고
		)
	{
		CKTDGUIStatic* pPartyOnly = (CKTDGUIStatic*)m_pDLGMenu->GetControl( L"Static_MyPartyOnly" );
		if(pPartyOnly != NULL && g_pSquareGame == NULL)
		{
			pPartyOnly->SetShow(true);
			bool bCurrentOption = g_pMain->GetGameOption().GetFieldParty();
			pPartyOnly->GetPicture(0)->SetShow(!bCurrentOption);	// off picture
			pPartyOnly->GetPicture(1)->SetShow(bCurrentOption);		// on picture
		}
		else if(pPartyOnly != NULL && g_pSquareGame != NULL)
		{
			pPartyOnly->SetShow(false);
		}

		CKTDGUIStatic* pSDMode = (CKTDGUIStatic*)m_pDLGMenu->GetControl( L"Static_SDMode" );
		if(pSDMode != NULL)
		{
			pSDMode->SetShow(true);
			bool bCurrentOption = g_pMain->GetGameOption().GetFieldSD();
			pSDMode->GetPicture(0)->SetShow(!bCurrentOption);	// off picture
			pSDMode->GetPicture(1)->SetShow(bCurrentOption);		// on picture
		}
	}
	else
	{
		CKTDGUIControl* pPartyOnly = m_pDLGMenu->GetControl( L"Static_MyPartyOnly" );
		if(pPartyOnly != NULL)
			pPartyOnly->SetShow(false);
		CKTDGUIControl* pSDMode = m_pDLGMenu->GetControl( L"Static_SDMode" );
		if(pSDMode != NULL)
			pSDMode->SetShow(false);

		if( NULL != g_pX2Game )
		{
			if( NULL != g_pData && NULL != g_pData->GetSlideShot() && true == g_pData->GetSlideShot()->IsPresentNow() )
			{
				g_pData->GetUIManager()->SetShowUI( false );
			}
			else
			{
				g_pData->GetUIManager()->SetShowUI( m_bShowUI );//, false );
			}
		}

	}
	//}}
	if( NULL != m_pDLGMenu && NULL != g_pMain)
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDLGMenu->GetControl(L"Static_ShowHelp");
		if( NULL != pStatic )
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_BATTLE_FIELD:
			case CX2Main::XS_DUNGEON_GAME:
				{
					pStatic->GetPicture(0)->SetShow( true );
						pStatic->GetPicture(1)->SetShow( g_pMain->GetGameOption().GetOptionList().m_bPlayGuide );
				}break;
			default:
				{
					pStatic->GetPicture(0)->SetShow( false );
					pStatic->GetPicture(1)->SetShow( false );
				}break;
			}

		}
	/*
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDLGMenu->GetControl(L"Static_ShowHelp");
			if( NULL != g_pTFieldGame && true == g_pTFieldGame->GetJoinNpc() )
			{
				pStatic->GetPicture(0)->SetShow( false );
				pStatic->GetPicture(1)->SetShow( false );
			}
			else
			{
				if( NULL != pStatic && NULL != g_pMain )
				{
					pStatic->GetPicture(0)->SetShow( true );
					pStatic->GetPicture(1)->SetShow( g_pMain->GetGameOption().GetOptionList().m_bPlayGuide );
				}
			}*/

	}
	if ( m_pCashShop != NULL && m_pCashShop->GetOpen() == false )
	{
		SetShowDlgMenu(true);
	}
	else
	{
		SetShowDlgMenu(false);
	}

	if ( m_pCashShop != NULL && m_pCashShop->GetOpen() == false && m_bShowUI == true )
		g_pMain->GetMemoryHolder()->UpdateNewYear2014Event();
	else
		g_pMain->GetMemoryHolder()->GetNewYear2014EventDLG( this )->SetShow( false );


	//ChatProcess();
	//m_pCharacterRoom->OnFrameMove( fTime, fElapsedTime );
    if( m_pCashShop != NULL )
    {
	    m_pCashShop->OnFrameMove( fTime, fElapsedTime );
    }//if

	if ( g_pMain->GetLVUpEventMgr() != NULL && g_pTFieldGame != NULL)
		g_pMain->GetLVUpEventMgr()->OnFrameMove( fTime, fElapsedTime );

	//{{ kimhc // 2009-11-02 // 길드 인포머는 던전 플레이 중에도 출력되야 함으로 수정
	// OnFrameMove 내에서 막는 것으로 변경 함
	//}} kimhc // 2009-11-02 // 길드 인포머는 던전 플레이 중에도 출력되야 함으로 수정
		g_pMain->GetInformerManager()->OnFrameMove( fTime, fElapsedTime );

// #ifdef NEW_VILLAGE_UI
// 	if(g_pData->GetUIManager() != NULL )
// 		g_pData->GetUIManager()->OnFrameMove( fTime, fElapsedTime );
// #endif
// X2State로 이동!

	if( g_pMain->GetPostBox() != NULL)
	{
		g_pMain->GetPostBox()->OnFrameMove( fTime, fElapsedTime );
	}


	// 새광장 메뉴버튼 new 아이콘 표시
	if( NULL != m_pDLGMenu )
	{
		if(g_pMain->GetNowStateID() != CX2Main::XS_DUNGEON_GAME)
		{
			CKTDGUIStatic* pStaticInven = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"Inventory_New" );
			if( NULL != pStaticInven )
			{
				if( g_pData != NULL && g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIInventory() != NULL &&
					g_pData->GetUIManager()->GetUIInventory()->GetNewItem() == true)
				{
					pStaticInven->SetShowEnable( true, false );
				}
				else
				{
					pStaticInven->SetShowEnable( false, false );
				}
			}


			CKTDGUIStatic* pStaticSkill = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"Skill_New" );
			if( NULL != pStaticSkill )
			{
				if( g_pData->GetMyUser() != NULL &&
					g_pData->GetMyUser()->GetSelectUnit() != NULL )
				{
					if ( 0 < g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSPoint + g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iCSPoint )
					{
						pStaticSkill->SetShowEnable( true, false );
					}
					else
					{
						pStaticSkill->SetShowEnable( false, false );
					}
				}
			}

		}



		// 캐시샵 버튼위에 New 표시
		CKTDGUIStatic *pStaticShop = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"Cash_New" );
		if( NULL != pStaticShop )
		{
			pStaticShop->SetShowEnable( true, false );
		}

		m_fTimeCheckWarpVip -= fElapsedTime;
		if( m_fTimeCheckWarpVip <= 0.f )
		{
			CKTDGUIStatic *pStaticWarpVip = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"Warp_VIP" );
			//vip 를 사용 하였으면 뜬다. 버튼의 디스크립션에 시간을 표시한다.
			if( pStaticWarpVip != NULL )
			{
				// 1분뒤에 다시 체크
				m_fTimeCheckWarpVip = 60.f;

				if( g_pData->GetMyUser() != NULL &&
					g_pData->GetMyUser()->GetSelectUnit() != NULL )
				{
					CX2Unit::UnitData& kUnitData = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
					__int64 trWarpVipEndDate = kUnitData.m_trWarpVipEndDate;

					CKTDGUIButton *pButtonWarp = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl( L"Warp" ));
					if( trWarpVipEndDate > g_pData->GetServerCurrentTime64() )
					{
						// VIP 기간이니 아이콘 표시해주자
						pStaticWarpVip->SetShowEnable( true, false );
						// VIP 기간인 것을 셋팅도 해주자
						kUnitData.m_bWarpVip = true;

						// 기간도 표시해주자
						if( NULL != pButtonWarp	)
						{
							wstring wstrRwstrRemainTime = GetExpirationDateDesc( trWarpVipEndDate );
							wstrRwstrRemainTime += GET_REPLACED_STRING( ( STR_ID_24618, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_WARP ) ) );
							SetGuideString( pButtonWarp, wstrRwstrRemainTime.c_str() );
						}
					}
					else
					{
						pStaticWarpVip->SetShowEnable( false, false );
						kUnitData.m_bWarpVip = false;

						// 기간도 제거하자
						if( NULL != pButtonWarp )
						{
							wstring wstrRwstrRemainTime =GET_REPLACED_STRING( ( STR_ID_24618, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_WARP ) ) );
							SetGuideString( pButtonWarp, wstrRwstrRemainTime.c_str() );
						}
					}
				}
			}
		}

		// 급하니 여기서 같이 체크 하자
		// 2013.12.17 darkstarbt_조성욱 // 캐쉬 이벤트 임으로 1분단위로 위에서 체크 하던곳에서 실시간 체크로 뺀다. 이벤트가 종료되고 1분뒤에 할인 태그가 빠질수 있다.
		CKTDGUIStatic *pStaticDiscountTag = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"DiscountTag" );
		if( NULL != pStaticDiscountTag )
		{
			IF_EVENT_ENABLED( CEI_CHRISTMAS_DISCOUNT_EVENT_2013 )
			{
				switch(g_pMain->GetNowStateID())
				{
				case CX2Main::XS_PVP_LOBBY:
				case CX2Main::XS_PVP_ROOM:
				case CX2Main::XS_PVP_GAME:
					pStaticDiscountTag->SetShowEnable(false, false);
					break;
				default:
					pStaticDiscountTag->SetShowEnable(true, true);
					break;
				}
			}
			ELSE
			{
				pStaticDiscountTag->SetShowEnable(false, false);
			}
		}

		bool bMark = false;

		// 펫 진화가능 알림 버튼
		CKTDGUIStatic *pStaticPet = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"Pat_Inventory_Evolution" );
		CKTDGUIStatic *pStaticPetNew = (CKTDGUIStatic*) m_pDLGMenu->GetControl( L"Pat_Inventory_New" );
		if( pStaticPetNew != NULL )
			pStaticPetNew->SetShowEnable( false, false );

		if( NULL != pStaticPet )
		{
			if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->CanMyPetEvolution() == true )
			{
				pStaticPet->SetShowEnable( true, false );
				bMark = true;
			}
			else
				pStaticPet->SetShowEnable( false, false );
		}

		if( bMark == false )
		{
			if( NULL != pStaticPetNew )
			{
				if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetNerverSummonPet() == true )
				{
					pStaticPetNew->SetShowEnable( true, false );
				}
				else
					pStaticPetNew->SetShowEnable( false, false );
			}
		}
	}

	if( g_pMain->GetCouponBox() != NULL )
		g_pMain->GetCouponBox()->OnFrameMove( fTime, fElapsedTime );

	// 오현빈 // 메신저 프레임무브 2번 수행 하는 것에 대한 예외처리로 NULL == g_pX2Game 구문 추가
	// StateMenu를 상속받지 않는 Game에서 수행하는것을 대비하여 게임 객체에 대한 예외처리 함.
	if ( g_pData->GetMessenger() != NULL && NULL == g_pX2Game)
		g_pData->GetMessenger()->OnFrameMove( fTime, fElapsedTime );
	//상황에 따라 메뉴 버튼 갯수 갱신
	UpdateMenuPosistion();

	m_fTimeRefreshMenuInfo += fElapsedTime;
	if ( m_fTimeRefreshMenuInfo > 0.1f )
	{
		m_fTimeRefreshMenuInfo = 0.0f;
		RefreshMenuInfo();
	}

	if ( m_pCashShop != NULL && m_pCashShop->GetOpen() == false && m_pCashShop->GetAutoOpen() == true )
	{
		m_pCashShop->SetAutoOpen( false );
		ToggleCashShop();
	}

	switch(g_pMain->GetNowStateID())
	{
	case CX2Main::XS_PVP_LOBBY:
	case CX2Main::XS_PVP_ROOM:
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_SQUARE_GAME:
	case CX2Main::XS_TRAINING_SCHOOL:
	case CX2Main::XS_BATTLE_FIELD:
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_WEDDING_GAME:
		{
			CheckMyInfoChange();
		} break;
	default:
		{

		} break;
	}

	CheckPresentArrival();

	//마우스 동작 있을 때만 메인메뉴 보여주도록 수정
	m_fMenuMaxRemainTime -= fElapsedTime;
	if( true == m_bShowMainMenuButton )
	{
		if( m_fMenuMaxRemainTime < 0.3f)
		{
			m_bShowMainMenuButton = false;
		}
		vector<CKTDGUIButton*>::iterator it = m_vecpButtonMainMenu.begin();
		for( ; it!= m_vecpButtonMainMenu.end(); it++)
		{
			if( NULL != (*it))
			{
				(*it)->SetColor( D3DXCOLOR(1,1,1,m_fMenuMaxRemainTime ));
			}
		}
	}
	//x2game이 생성되어 있을 때는 x2game에서 쿨타임 UI 관리
	if ( NULL == g_pX2Game )
	{
		g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillCoolTime(fElapsedTime);
		CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_UserSkillTree;
		refUserSkillTree.OnFrameMove(fTime, fElapsedTime);
	}
	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
	{
		g_pData->GetPlayGuide()->OnFrameMove(fTime, fElapsedTime);
	}

	if ( NULL != g_pData->GetPremiumBuffManager() )
	{
		if ( true == g_pData->GetPremiumBuffManager()->IsFirstComeBackBuffDesc() )
		{
			g_pData->GetPremiumBuffManager()->SetShowFirstComBackUI( true );
		}
	}

	return S_OK;
}

HRESULT CX2StateMenu::OnFrameRender()
{
	CX2State::OnFrameRender();

	return S_OK;
}



bool CX2StateMenu::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{


	//if( g_pMain->GetPostBox() != NULL && g_pMain->GetPostBox()->MsgProc( hWnd, uMsg, wParam, lParam ) == true)
	//{
	//	return true;
	//}

	// 주의! Itemslot 드래깅이 필요한 UI라면 UIManager에 MsgProc이 들어가기 전에 먼저 MsgProc을 받을 것! (ex : postbox )
	// UIManager의 MsgProc에서 ItemSlot의 드래그 종료 처리를 함
	// MouseUp 타이밍에
	//if ( g_pData->GetUIManager() != NULL )
	//{
	//	g_pData->GetUIManager()->MsgProc( hWnd, uMsg, wParam, lParam);
	//}
	// X2State로 이동!

	// 워프 휠 처리는 다른 것들보다 먼저 처리 합니다.
	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			if( m_pDLGWarpDestination != NULL && m_pDLGWarpDestination->GetShow() && m_pDLGWarpDestination->GetIsMouseOver() )
			{
				CKTDGUIContolList* pControlList = (CKTDGUIContolList*) m_pDLGWarpDestination->GetControl( L"WarpDestControlList" );
				if( pControlList == NULL )
					return false;
				CKTDGUIScrollBar* pScroolBarY = pControlList->GetYScroolBar();
				if(pScroolBarY == NULL)
					return false;

				short zDelta = (short)HIWORD(wParam);
				m_SumDelta += zDelta;

				while (abs(m_SumDelta) >= WHEEL_DELTA)
				{
					if(m_SumDelta>0)
					{
						pScroolBarY->Scroll(-1);
						m_SumDelta -= WHEEL_DELTA;
					}
					else
					{
						pScroolBarY->Scroll(1);
						m_SumDelta += WHEEL_DELTA;
					}
				}
				return true;
			}
		}break;
	}

	bool bRetVal = false;

	bRetVal = CX2State::MsgProc( hWnd, uMsg, wParam, lParam );

	if ( bRetVal == true )
	{
		PopUpMenuClose( hWnd, uMsg, wParam, lParam );
		return true;
	}

	if ( uMsg == WM_LBUTTONDOWN )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMenuWindow, NULL, false );
		m_pDLGMenuWindow = NULL;
		m_bShowWindowMenu = false;
	}
	if ( uMsg == WM_MOUSEMOVE )
		ShowMainMenuButton();

	if ( m_pCashShop != NULL &&
		m_pCashShop->GetOpen() == true &&
		m_pCashShop->MsgProc( hWnd, uMsg, wParam, lParam ) == true )
	{
		return true;
	}

	if( NULL != g_pMain->GetCouponBox() &&
		true == g_pMain->GetCouponBox()->MsgProc( hWnd, uMsg, wParam, lParam ) )
	{
		return true;
	}

	if ( g_pMain->GetInformerManager() != NULL && g_pMain->GetInformerManager()->MsgProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	if( true == m_Book.GetShow() &&
		true == m_Book.MsgProc( hWnd, uMsg, wParam, lParam ) )
	{
		return true;
	}

	return false;
}

void CX2StateMenu::OnResetStateAndEquip()
{
	if ( m_pCashShop != NULL )
    {
		m_pCashShop->ResetUnitViewerEquip();
    }//if
}


bool CX2StateMenu::Handler_EGS_SEARCH_UNIT_REQ( const WCHAR* wszNickName )
{
	KEGS_SEARCH_UNIT_REQ kPacket;
	kPacket = wszNickName;

	if ( g_pMain->GetStringFilter()->CheckIsKoreanNEnglishNNumber( wszNickName ) == false )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_546 ), this );
		return true;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_SEARCH_UNIT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_SEARCH_UNIT_ACK );

	return true;
}


bool CX2StateMenu::Handler_EGS_SEARCH_UNIT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SEARCH_UNIT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_SEARCH_UNIT_ACK ) == true )
	{
		//{{ oasis907 : 김상윤 [2010.5.19] // 대전 던전 서버군 통합
		if ( kEvent.m_iOK == NetError::ERR_SEARCH_UNIT_03 || kEvent.m_iOK == NetError::ERR_SEARCH_UNIT_04)
		{
			wstring wstrServerName = L"";
			wstrServerName = g_pInstanceData->GetServerGroupName();
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300), GET_REPLACED_STRING( ( STR_ID_5132, "L", wstrServerName ) ), g_pMain->GetNowState() );
			return true;
		}
		//}}
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMessenger()->OpenSearchFriendResultWindow( kEvent );

			/*
			// user 정보를 찾았다
			m_wstrFindUserName = kEvent.m_kCUnitInfo.m_wstrCharName;
			m_FindUserUid = kEvent.m_kCUnitInfo.m_iUnitUID;

			//m_iSearchUnitChannelID = (int) kEvent.m_kCUnitInfo.m_uiChannelID;			// fix!!

			m_iSearchUnitDungeonID = (int) kEvent.m_kCUnitInfo.m_iDungeonID;
			m_iSearchUnitRoomUID = kEvent.m_kCUnitInfo.m_iRoomUID;


			// fix!!
			//m_eSearchUnitConnectionState = (CX2Unit::CONNECT_UNIT_STATE) kEvent.m_kCUnitInfo.m_cConnectionState;


			SAFE_DELETE_DIALOG( m_pDLGFindUser );
			SAFE_DELETE_DIALOG( m_pDLGUserInfo );
			m_pDLGUserInfo = new CKTDGUIDialog( this, L"DLG_User_Info.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUserInfo );

			UpdateUserInfoUI(kEvent);
			*/
		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"유저를 찾지 못했습니다.", this );
		}

		//{{ 2007. 11. 13  최육사  안쓰는 통계정리
		//g_pData->GetServerProtocol()->SendID( EGS_CLOSE_WND_FINDUSER_NOT );
		//}}

		return true;
	}

	return false;
}


bool CX2StateMenu::Handler_EGS_GET_CONNECTION_UNIT_INFO_REQ( const WCHAR* wszNickName )
{
	KEGS_GET_CONNECTION_UNIT_INFO_REQ kPacket;
	kPacket = wszNickName;

	if ( g_pMain->GetStringFilter()->CheckIsKoreanNEnglishNNumber( wszNickName ) == false )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_546 ), this );
		return true;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_CONNECTION_UNIT_INFO_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK );

	return true;
}


bool CX2StateMenu::Handler_EGS_GET_CONNECTION_UNIT_INFO_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_CONNECTION_UNIT_INFO_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_GET_CONNECTION_UNIT_INFO_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// user 정보를 찾았다
			m_wstrFindUserName = kEvent.m_kCUnitInfo.m_wstrCharName;
			m_FindUserUid = kEvent.m_kCUnitInfo.m_iUnitUID;

			//m_iSearchUnitChannelID = (int) kEvent.m_kCUnitInfo.m_uiChannelID;			// fix!!

			m_iSearchUnitDungeonID = (int) kEvent.m_kCUnitInfo.m_iDungeonID;
			m_iSearchUnitRoomUID = kEvent.m_kCUnitInfo.m_iRoomUID;


			// fix!!
			//m_eSearchUnitConnectionState = (CX2Unit::CONNECT_UNIT_STATE) kEvent.m_kCUnitInfo.m_cConnectionState;


			SAFE_DELETE_DIALOG( m_pDLGFindUser );
			SAFE_DELETE_DIALOG( m_pDLGUserInfo );
			m_pDLGUserInfo = new CKTDGUIDialog( this, L"DLG_User_Info_Ver1.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUserInfo );

			UpdateUserInfoUI(kEvent);
		}
		else
		{
			//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), L"유저를 찾지 못했습니다.", this );
		}

		//{{ 2007. 11. 13  최육사  안쓰는 통계정리
		//g_pData->GetServerProtocol()->SendID( EGS_CLOSE_WND_FINDUSER_NOT );
		//}}

		return true;
	}

	return false;
}


bool CX2StateMenu::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2StateOption::UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	if ( NULL != g_pData->GetPremiumBuffManager() )
	{
		if ( true == g_pData->GetPremiumBuffManager()->UICustomEventProc(hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if ( NULL != g_pMain->GetKeyPad() )
	{
		if ( true == g_pMain->GetKeyPad()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if(m_Book.UICustomEventProc(hWnd, uMsg, wParam, lParam) == true)
	{
		return true;
	}

	if(g_pData != NULL && g_pData->GetUserMenu() != NULL)
	{
		if( g_pData->GetUserMenu()->UICustomEventProc(hWnd, uMsg, wParam, lParam) == true)
			return true;
	}

// X2State로 이동
// 	if(g_pData != NULL && g_pData->GetUIManager() != NULL)
// 	{
// 		if( g_pData->GetUIManager()->UICustomEventProc(hWnd, uMsg, wParam, lParam) == true)
// 			return true;
// 	}


	if( NULL != g_pMain->GetCouponBox() )
	{
		if( true == g_pMain->GetCouponBox()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}


    if( NULL != g_pMain->GetPostBox() )
    {
        if( true == g_pMain->GetPostBox()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) )
            return true;
    }

	if( NULL != g_pMain->GetSecurityPad() )
	{
		if( true == g_pMain->GetSecurityPad()->UICustomEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}



	//if ( m_pCharacterRoom != NULL && m_pCharacterRoom->UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
	//	return true;
	if ( m_pCashShop != NULL && m_pCashShop->UICustomEventProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;



	switch(wParam)
	{
	case SMUCM_BACK:
		{
		}
		break;

	case SMUCM_STATE_CHANGE_UNIT_SELECT:
		{
			return ProcessStateChangeUnitSelectWithOptionWindow();
		}
		break;


	case SMUCM_STATE_CHANGE_PVP_LOBBY:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_PVP_LOBBY:
			case CX2Main::XS_PVP_ROOM:
			case CX2Main::XS_DUNGEON_ROOM:
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_SQUARE_GAME:
			case CX2Main::XS_TRAINING_SCHOOL:
			case CX2Main::XS_WEDDING_GAME:
				{
					if( NULL != g_pData->GetMyUser() &&
						g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_DEV &&
						g_pData->GetSelectUnitLevel() < 3 )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_657 ), this );
						return true;
					}

					CX2StateAutoChanger::TARGET_DETAIL targetDetail;

					g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), (int)CX2Main::XS_PVP_LOBBY, targetDetail );

				} break;
			default:
				break;
			}

			return true;
		}
		break;

	case SMUCM_STATE_CHANGE_VILLAGE:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_PVP_LOBBY:
			case CX2Main::XS_PVP_ROOM:
			case CX2Main::XS_DUNGEON_ROOM:
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_SQUARE_GAME:
			case CX2Main::XS_TRAINING_SCHOOL:
			case CX2Main::XS_WEDDING_GAME:
				{
					CX2StateAutoChanger::TARGET_DETAIL targetDetail;
					g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), (int)CX2Main::XS_VILLAGE_MAP, targetDetail );

				} break;
			default:
				break;
			}

			return true;
		}
		break;

	case SMUCM_MENU:
		{
			//return ToggleSystemMenu();
			//return ToggleLobbyMenuWindow();
		}
		break;

	case SMUCM_MY_INFO:
		{
			if( false == static_cast<CX2State*>(g_pMain->GetNowState())->GetEnableShortCutKey() )
				return true;

			g_pMain->GetInformerManager()->GetInvenInformer()->InvalidInfo();
            if(g_pData->GetUIManager() != NULL)
                return g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_CHARINFO);
		}
		break;
		// 2008.11.17 김태완
	case SMUCM_COMMUNITY:
		{
			// 커뮤니티 창
			if(g_pData->GetUIManager() != NULL)
				return g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_COMMUNITY);
		}
		break;

	case SMUCM_INVENTORY:
		{
			if( false == static_cast<CX2State*>(g_pMain->GetNowState())->GetEnableShortCutKey() )
				return true;

 			if(g_pData->GetUIManager() != NULL)
 				return g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN);
		}
		break;

	case SMUCM_SYSTEM:
		{
			// 지헌 : 은행 열려있으면 시스템 매뉴 못 열도록
			if ( g_pData->GetUIManager() != NULL &&
				g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_PRIVATE_BANK ) == true )
			{
			}
			else
			{
				return ToggleSystemMenu(true);
			}
		}


	case SMUCM_VIEW_STUDENT_CANDIDATE_LIST:
		{
			// 체험 아이디 제한
			if( true == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(270,350), GET_STRING( STR_ID_40 ), g_pMain->GetNowState() );
				return true;
			}

			if( g_pData->GetSelectUnitLevel() >= 20 )
			{
				if( NULL != g_pMain->GetTutorSystem() )
				{
					wstring wstrNickName = g_pData->GetMyUser()->GetSelectUnit()->GetNickName();
					g_pMain->GetTutorSystem()->OpenStudentCandidateList( true );
					g_pMain->GetTutorSystem()->UpdateStudentCandidateListPage();
					g_pMain->GetTutorSystem()->SetLastViewDateToday( wstrNickName );
				}
			}
			else
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_658 ), this );
			}
			return true;
		}
		break;

	case SMUCM_CASH_SHOP:
		{

			if( g_pData->GetPartyManager()->GetProcessPvpMatch() == true )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_13599 ), this );
				return true;
			}

			if(g_pTFieldGame != NULL)
			{
				g_pTFieldGame->CloseFieldName(0.f);
				g_pTFieldGame->CloseBuffNameForce();
			}
			ToggleCashShop();
			return true;
		} break;

	case SMUCM_OPTION:
		{
			//ToggleSystemMenu();
			ToggleSystemMenu(false);
			if(GetOptionWindowOpen() == true)
				return CloseOptionWindow();
			else
				return OpenOptionWindow();
		}
		break;
	case SMUCM_SECURITY:
		{
			ToggleSystemMenu(false);
			if(GetOptionWindowOpen() == true)
				return CloseOptionWindow();
			else
				return OpenSecurityOptionWindow();
		}
		break;

	case SMUCM_TUTORIAL:
		{
			ASSERT( !"invalid code block" );
			// fix!! 이 코드 사용 안되는 듯 함




			// 랜덤 로딩화면 설정
			CKTDGUIDialogType pDLGLoadingState = new CKTDGUIDialog( this, L"DLG_PVP_Game_Loading_State.lua", 0.0f );
			const int nLoadingBG = pDLGLoadingState->GetDummyInt(0);
			int iRandBG = rand()%nLoadingBG;
			g_pMain->SetTutorialRandomLoadingBG( iRandBG );
			SAFE_DELETE_DIALOG( pDLGLoadingState );


			g_pMain->SetStateIDBeforeTutorial( g_pMain->GetNowStateID() );
			switch( g_pMain->GetNowStateID() )
			{
				// fix!! 만약에 이 코드 사용된다면 상태별로 tutorial 입장하는 코드 만들어야 함
			case CX2Main::XS_PVP_LOBBY:
				{
					CX2StatePvPLobby* pStatePVPLobby = (CX2StatePvPLobby*)g_pMain->GetNowState();
					return pStatePVPLobby->EnterTutorial();
				} break;

			case CX2Main::XS_PVP_ROOM:
				{
					CX2StatePVPRoom* pStatePVPRoom = (CX2StatePVPRoom*)g_pMain->GetNowState();
					return pStatePVPRoom->EnterTutorial();
				} break;
			default:
				break;
			}
		}
		break;

	case SMUCM_MY_INFO_MY_STATE:		// fix!!
		{
			//return m_pCharacterRoom->SetCharacterRoomState( CX2CharacterRoom::CRS_MY_STATE );
		}
		break;

	case SMUCM_MY_INFO_INVENTORY:		// fix!!
		{
			//return m_pCharacterRoom->SetCharacterRoomState( CX2CharacterRoom::CRS_INVENTORY );
		}
		break;

	case SMUCM_MY_INFO_QUEST:
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUEST_NEW);
			return true;
		}
		break;

	case SMUCM_MY_INFO_SKILL_TREE:
		{
			if( false == static_cast<CX2State*>(g_pMain->GetNowState())->GetEnableShortCutKey() )
				return true;

			OnClickedMenuSkillButton();
			return true;
		} break;

	case SMUCM_STATE_CHANGE_SERVER_SELECT:
		{
			return ProcessStateChangeServerSelectWithOptionWindow();
		}
		break;

	case SMUCM_PARTY:
		{
			// 08.12.14 UIManager 연결
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PARTY);
// 			if( NULL != g_pMain->GetPartyUI() )
// 			{
// 				g_pMain->GetPartyUI()->OpenPartyDLG( true );
// 				g_pMain->GetPartyUI()->UpdatePartyDLG();
// 			}

			return true;

			//if ( g_pPartyMsgerUI->GetOpenUserListWindow() == false )
			//	return g_pPartyMsgerUI->Handler_CLICKED_PARTY_BUTTON();
			//else
			//{
			//	g_pPartyMsgerUI->CloseUserListWindow();
			//	//g_pPartyMsgerUI->CloseChatWindow();
			//	return true;
			//}
		}
		break;

	case SMUCM_FIND_USER:
		{
			OpenFindUserDLG();

			return true;
		}
		break;

	case SMUCM_FIND_USER_OK:
		{
			if( NULL != m_pDLGFindUser )
			{
				CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*) m_pDLGFindUser->GetControl( L"IMEEditBox_NickName" );
				if( NULL != pIMEEditBox )
				{
					m_wstrFindUserName = pIMEEditBox->GetText();
					if( m_wstrFindUserName.length() > 0 )
					{
						const int MAGIC_MAX_NICKNAME_LENGTH = 20;
						m_wstrFindUserName = m_wstrFindUserName.substr( 0, MAGIC_MAX_NICKNAME_LENGTH );

						Handler_EGS_GET_CONNECTION_UNIT_INFO_REQ( m_wstrFindUserName.c_str() );
						return true;
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_660 ), this );
					}
				}
			}
			return true;
		}
		break;

	case SMUCM_FIND_USER_CANCEL:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGFindUser, NULL, false );
			m_pDLGFindUser = NULL;

			//{{ 2007. 11. 13  최육사  안쓰는 통계정리
			//g_pData->GetServerProtocol()->SendID( EGS_CLOSE_WND_FINDUSER_NOT );
			//}}
			return true;
		}
		break;

	case SMUCM_USER_INFO_OK:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGUserInfo, NULL, false );
			m_pDLGUserInfo = NULL;

			return true;
		}
		break;

	case SMUCM_USER_INFO_PLAY_TOGETHER:
		{
			// 해당 user가 있는 곳으로 이동해야 한다
			CX2StateAutoChanger::TARGET_DETAIL targetDetail;
			targetDetail.m_iChannelID = m_iSearchUnitChannelID;
			targetDetail.m_iDungeonID = m_iSearchUnitDungeonID;
			targetDetail.m_iRoomUID = m_iSearchUnitRoomUID;

			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGUserInfo, NULL, false );
			m_pDLGUserInfo = NULL;

			return true;
		}
		break;

	case SMUCM_USER_INFO_REFRESH:
		{
			Handler_EGS_GET_CONNECTION_UNIT_INFO_REQ( m_wstrFindUserName.c_str() );

		} break;


	case SMUCM_INVITE_NOT_CANCEL:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGInviteNot, NULL, false );
			m_pDLGInviteNot = NULL;
		}
		break;

	case SMUCM_FIND_USER_SELECT:
		{
			if ( m_pDLGFindUser != NULL )
			{
				CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDLGFindUser->GetControl( L"ComboBoxUser_Search_Window" );
				CKTDGUIIMEEditBox* pIMEEditBox = (CKTDGUIIMEEditBox*)m_pDLGFindUser->GetControl(L"IMEEditBox_NickName");
				if ( pComboBox->GetSelectedItem() != NULL )
				{
					pIMEEditBox->SetText( pComboBox->GetSelectedItem()->strText );
				}
			}
		}
		break;

	//case SMUCM_SELL_ITEM_NUM_CHANGE:
	//	{
	//		CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
	//		m_pCharacterRoom->SellItemNumChange( pControl->GetDummyInt( 0 ) );
	//	}
	//	break;

	//case SMUCM_SELL_ITEM_NUM_OK:
	//	{
	//		m_pCharacterRoom->DecideSellItemNum( true );
	//		return true;
	//	}
	//	break;

	//case SMUCM_SELL_ITEM_NUM_CANCEL:
	//	{
	//		m_pCharacterRoom->DecideSellItemNum( false );
	//		return true;
	//	}
	//	break;

	case SMUCM_CHANGE_SELL_MOUSE_CURSOR:
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN, true);
			m_pCursor->ChangeCursorState( CX2Cursor::XCS_SELL );
		}
		break;

	case SMUCM_CHANGE_REPAIR_MOUSE_CURSOR:
		{
			//g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN, true);
			//g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTab( CX2Inventory::ST_EQUIP );
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_CHARINFO, true);

			m_pCursor->ChangeCursorState( CX2Cursor::XCS_REPAIR );
		}
		break;

	case SMUCM_COUPON_BOX:
		{
			if( NULL != g_pMain->GetCouponBox() )
			{
				g_pMain->GetCouponBox()->OpenCouponBoxDLG( true );
			}

			return true;
		}
		break;

	case SMUCM_COUPON_INPUT_BOX:
		{
			// 체험 아이디 제한
			if( true == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(270,350), GET_STRING( STR_ID_40 ), g_pMain->GetNowState() );
				return true;
			}

			if( NULL != g_pMain->GetCouponBox() )
			{
				g_pMain->GetCouponBox()->OpenCouponNumberInputDLG( true );
			}

			return true;
		}
		break;

	case SMUCM_CHANGE_DECOMPOSITION_MOUSE_CURSOR:
		{
			m_pCursor->ChangeCursorState( CX2Cursor::XCS_DECOMPOSITION );
		}
		break;

	case SMUCM_CHANGE_ENCHANT_MOUSE_CURSOR:
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN, true);
			g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTab( CX2Inventory::ST_EQUIP );
			m_pCursor->ChangeCursorState( CX2Cursor::XCS_ENCHANT );
		}
		break;

	case SMUCM_CHANGE_SOCKET_MOUSE_CURSOR:
		{
			if( g_pData->GetUIManager()->GetUIInventory()->GetSortType() != CX2Inventory::ST_AVARTA )
				g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTab( CX2Inventory::ST_EQUIP );
			m_pCursor->ChangeCursorState( CX2Cursor::XCS_SOCKET );
			//{{ 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
			//}} 최민철 [2013/1/4]  게임내 정보 스트링을 엑셀파일로 출력
		}
		break;

	case SMUCM_CHANGE_ATTRIB_ENCHANT_MOUSE_CURSOR:
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_INVEN, true);
			g_pData->GetUIManager()->GetUIInventory()->ChangeInventoryTab( CX2Inventory::ST_EQUIP );
			m_pCursor->ChangeCursorState( CX2Cursor::XCS_ATTRIB_ENCHANT );
		}
		break;

	case SMUCM_CHANGE_EXCHANGE_ITEM_MOUSE_CURSOR:
		{
			//{{ dmlee 2008.12.2 구아이템 교환 이벤트기간 종료로 기능삭제
			//m_pCursor->ChangeCursorState( CX2Cursor::XCS_ITEM_EXCHANGE );
			return true;
			//}} dmlee 2008.12.2 구아이템 교환 이벤트기간 종료로 기능삭제
		} break;

	case SMUCM_USER_INFO_FRIEND_REQ:
		{
			// 체험 아이디 제한
			if( true == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(270,350), GET_STRING( STR_ID_40 ), g_pMain->GetNowState() );
				return true;
			}

			if ( g_pData->GetMessenger() != NULL )
			{
				wstring addFriendMessage = GET_STRING( STR_ID_74 );
				return g_pData->GetMessenger()->Handler_EGS_REQUEST_FRIEND_REQ( m_wstrFindUserName.c_str(), addFriendMessage );
			}
		}
		break;

	//case SMUCM_USER_INFO_PARTY_INVITE:
	//	{
	//		if ( g_pPartyMsgerUI != NULL )
	//			return g_pPartyMsgerUI->Handler_EGS_INVITE_PARTY_REQ( m_FindUserUid, m_wstrFindUserName.c_str(), false );
	//	}
	//	break;

	case SMUCM_PRESENT_ARRIVAL_CLOSE:
		{
			if ( m_pDLGPresentArrival != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGPresentArrival, NULL, false );

			m_pDLGPresentArrival = NULL;
		}
		break;


	//case SMUCM_STATE_CHANGE_ARCADE_LOBBY:
	//	{
	//		switch( g_pMain->GetNowStateID() )
	//		{
	//		case CX2Main::XS_VILLAGE_MAP:
	//			{
	//				Handler_EGS_OPEN_ARCADE_ROOM_LIST_REQ();


	//			} break;

	//		default:
	//			{
	//				CX2StateAutoChanger::TARGET_DETAIL targetDetail;
	//				g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), (int)CX2Main::XS_ARCADE_LOBBY, targetDetail );

	//			} break;
	//		}
	//		return true;
	//	} break;

    case SMUCM_POSTBOX:
        {
            // 우편함을 열고 닫는다.
            //if(!g_pMain->GetPostBox()->GetDialogShow() == true)
            //{
            //    if( true == GetCharacterRoom()->GetOpen() )
            //    {
            //        g_pMain->GetPostBox()->SetSwitchState(true);
            //        ToggleMyInfo( GetCharacterRoom()->GetCharRoomState() );
            //    }
            //}
            g_pMain->GetPostBox()->SetDialogShow( !g_pMain->GetPostBox()->GetDialogShow() );
        }
        break;


	case SMUCM_EXIT_SYSTEM:
		{
			return ToggleSystemMenu(false);
		}
		break;

	case SMUCM_LEAVE_DUNGEON_LOUNGE_OK:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGLeavePartyOrLounge, NULL, false );
			m_pDLGLeavePartyOrLounge = NULL;
			g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, CX2PartyUI::PUCM_PARTY_LEAVE_DUNGEON_LOUNGE, NULL, false );

			return true;
		} break;

	case SMUCM_LEAVE_DUNGEON_LOUNGE_CANCEL:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGLeavePartyOrLounge, NULL, false );
			m_pDLGLeavePartyOrLounge = NULL;

			return true;
		} break;


	case SMUCM_LEAVE_PARTY_OK:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGLeavePartyOrLounge, NULL, false );
			m_pDLGLeavePartyOrLounge = NULL;

			g_pKTDXApp->SendGameMessage( CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT, CX2PartyUI::PUCM_PARTY_LEAVE_PARTY, NULL, false );

			return true;
		} break;

	case SMUCM_LEAVE_PARTY_CANCEL:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGLeavePartyOrLounge, NULL, false );
			m_pDLGLeavePartyOrLounge = NULL;

			return true;
		} break;
		//{{ 09.03.18 김태완
	case SMUCM_OPEN_ELLIOS_GUIDE:
		{
			if(false == m_Book.GetShow())
			{
				m_Book.SetBookTable( "BT_ELLIOS_GUIDE");
				m_Book.SetShow(true);
			}
// 			else
// 			{
// 				m_Book.SetShow(false);
// 			}

		} break;
		//}}

	//  [11/9/2009 김상윤]
	case SMUCM_OPEN_ELLIOS_GUIDE_GO_TO_PAGE:
		{
			if(false == m_Book.GetShow())
			{
				m_Book.SetBookTable( "BT_ELLIOS_GUIDE");
				m_Book.SetShow(true);
			}
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iTargetPage = pButton->GetDummyInt(0);
			m_Book.GotoPage(iTargetPage);


		} break;

	//  [11/9/2009 김상윤]

	case SMUCM_PET_LIST:
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PET_LIST);
			return true;
		}
		break;

	case SMUCM_DUNGEON_MATCH_GAME_ACCEPT:
		{
			if ( NULL != g_pX2Game )
			{
				CX2GUUser* pMyUser = g_pX2Game->GetMyUnit();

				// 게임 중에 죽은 경우에는
				if ( NULL != pMyUser && ( pMyUser->GetGameUnitState() == CX2GameUnit::GUSI_DIE || !( pMyUser->GetNowHp() > 0 ) ) )
					g_pX2Game->CheckAndWarningBusyStateNow();
				else // 게임중에 살아있는 경우
					g_pData->GetPartyManager()->Handler_EGS_AUTO_PARTY_MAKING_SUCCESS_REPLY_NOT( true );
			}
			else
				g_pData->GetPartyManager()->Handler_EGS_AUTO_PARTY_MAKING_SUCCESS_REPLY_NOT( true );

			//매칭 확인 이후 단축키 사용 못하도록 변경
			CX2State* pNowState = static_cast<CX2State*>( g_pMain->GetNowState() );
			if ( NULL != pNowState )
				pNowState->SetEnableShortCutKey(false);

			return true;
		} break;

	case SMUCM_DUNGEON_MATCH_GAME_DENY:
		{
			g_pData->GetPartyManager()->Handler_EGS_AUTO_PARTY_MAKING_SUCCESS_REPLY_NOT( false );
			return true;
		} break;
	case SMUCM_DISABLE_QUICK_SLOT_MOUSE_UP:
		{
			//구매했다면 패스
			if( true == g_pData->GetMyUser()->GetSelectUnit()->IsExpandQuickSlot() )
			{
				return true;
			}

			//마을이 아니면 채팅 로그 추가
			if( g_pX2Game != NULL && g_pMain->GetNowStateID() != CX2Main::XS_BATTLE_FIELD )
			{
				g_pChatBox->AddChatLog(  GET_STRING( STR_ID_16478 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
				return false;
			}
			g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_MENU_QUICK_SLOT );
			ToggleCashShop();
			return true;
		} break;
	case SMUCM_DISABLE_SKILL_SLOT_MOUSE_UP:
		{
			//구매했다면 패스
			if( true == g_pData->GetUIManager()->GetUISkillTree()->IsExpandSkillSlot() )
			{
				return true;
			}

			//던전/대전 게임중에는 캐쉬샵 열지 않기.
			if( g_pX2Game != NULL && g_pMain->GetNowStateID() != CX2Main::XS_BATTLE_FIELD )
			{
				g_pChatBox->AddChatLog(  GET_STRING( STR_ID_16478 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
				return true;
			}
			g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_SKILL_SLOT );
			ToggleCashShop();
			return true;
		}
		break;
	case SMUCM_GUILD:
		{	// 길드 창
			if(g_pData->GetUIManager() != NULL)
				return g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_GUILD);
		}
		break;
	case SMUCM_NOTICE_CLOSE:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			if( NULL != pButton && NULL != g_pMain && NULL != g_pMain->GetNoticeManager())
				g_pMain->GetNoticeManager()->CloseItemNotice( pButton->GetDialog()->GetDummyInt(0) );
			return true;
		}
		break;
	case SMUCM_PLAYGUIDE_CLOSE:
		{
			if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
			{
				g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_NONE, false);
			}
		}
		break;


	case SMUCM_OPEN_KEYPAD:
		{
			ToggleSystemMenu(false);

			if ( NULL != g_pMain->GetKeyPad() )
				g_pMain->GetKeyPad()->ShowKeyPad( true );
		}
		break;
	case SMUCM_RAID_FIELD_EXIT_OK:
		{
			if( CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() )
				static_cast<CX2StateBattleField*>(g_pMain->GetNowState())->MoveToBeforePlaceAtRaidField();

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
		} break;
	case SMUCM_RAID_FIELD_EXIT_CANCEL:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
		} break;
	case SMUCM_SHOW_ADVERTISEMENT:
		{
			//현재 이벤트로 돌아가는 것만 보여줘야할 경우 아래 함수 쓰면 됨.
			//Handler_EGS_GET_ADVERTISEMENT_EVENT_REQ();
			if( false == g_pMain->GetBrowserWrapper()->IsClosed() )
			{
				g_pMain->GetBrowserWrapper()->CloseBrowser();
				g_pKTDXApp->GetDevice()->SetDialogBoxMode( FALSE );
			}

			if( true == g_pMain->GetBrowserWrapper()->IsClosed() )
			{
				g_pKTDXApp->GetDevice()->SetDialogBoxMode( TRUE );

				RECT rcWebPage;
				GetWindowRect( g_pKTDXApp->GetHWND(), &rcWebPage );

				POINT ptCenter;
				ptCenter.x = rcWebPage.left + ( rcWebPage.right - rcWebPage.left ) / 2;
				ptCenter.y = rcWebPage.top + ( rcWebPage.bottom - rcWebPage.top ) / 2;

				rcWebPage.left = ptCenter.x - 415;
				rcWebPage.right = ptCenter.x + 415;
				rcWebPage.top = ptCenter.y - 235;
				rcWebPage.bottom = ptCenter.y + 235;

				// 해외팀 각국가 전용 디파인
				std::wstring wstrURL = L"http://en.elswordonline.com/ingameinfo/";// 북미 광고 주소
				if( 1 == g_pInstanceData->GetServerGroupID() )// 남미면 주소 바꾼다
					wstrURL = L"http://es.elswordonline.com/ingameinfo/";
				OpenAdvertisementDialog( wstrURL );
			}
		}
		break;
	case SMUCM_WARP_BUTTON:
		{
			// 캐쉬 소모 없이 하고 싶으면, 여기서 조건 검사를 하자
			if( NULL != g_pData->GetMyUser()
				&& NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				if( true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_bWarpVip )
					ClickWarpButton( false );
				else
					ClickWarpButton( true );
			}
		}
		break;
	case SMUCM_WARP_DEST_CLOSE:
		{
			if( m_pDLGWarpDestination != NULL )
			{
				m_pDLGWarpDestination->SetShowEnable(false, false);
				m_iSelectedWarpDest = 0;
			}
		}
		break;
	case SMUCM_WARP_DEST_SELECT:
		{
			CKTDGUIButton *pButton = (CKTDGUIButton*)lParam;
			if( pButton == NULL )
				return true;

			m_iSelectedWarpDest = 0;
			m_iSelectedWarpDest = pButton->GetDummyInt(0);

			for(UINT i=0; i<m_vecWarpListButton.size(); ++i)
			{
				CKTDGUIButton *pDestButton = m_vecWarpListButton[i];
				if(pDestButton != NULL)
				{
					pDestButton->SetDownStateAtNormal(false);
					if( pDestButton->GetDummyInt(0) == m_iSelectedWarpDest )
					{
						m_iSelectedWarpDestIndex = i;
					}
				}
			}

			pButton->SetDownStateAtNormal(true);
		}
		break;
	case SMUCM_WARP_DEST:
		{
			if( m_iCurrentVillageWarpIndex == -1 )
				return true;

			if( CX2Main::XS_VILLAGE_MAP == g_pMain->GetNowStateID() &&
				m_iSelectedWarpDest== g_pData->GetLocationManager()->GetCurrentVillageID() )
				return true;

			UseWarpPopup(m_bIsEdConsumptionWarp);
			return true;
		}
		break;
	case SMUCM_WARP_DEST_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			// 여기 워프 처리 핸들러 넣자
			Handler_EGS_WARP_BY_BUTTON_REQ( m_iSelectedWarpDest );

			m_iSelectedWarpDest = 0;

			if( m_pDLGWarpDestination != NULL )
			{
				m_pDLGWarpDestination->SetShowEnable(false, false);
			}

			return true;
		}
		break;
case SMUCM_FACEBOOK_MOVE:
		{
			wstring wstrURL = L"http://www.facebook.com/ElswordOnlineNA";
			x2::platform::OpenUrl( wstrURL );
			return true;
		}
		break;
	case SMUCM_STEAM_COMMUNITY_MOVE:
		{
			wstring wstrURL = L"https://www.facebook.com/Elsword/app_112813808737465";
			x2::platform::OpenUrl( wstrURL );
			return true;
		}
		break;
	case SMUCM_COMPLETE_SPECIAL_MISSION:
		{
			Handler_EGS_2013_EVENT_MISSION_COMPLETE_REQ( false );
			return true;
		} break;
	case SMUCM_COMPLETE_SUPPORT_MISSION:
		{
			Handler_EGS_2013_EVENT_MISSION_COMPLETE_REQ( true );
			return true;
		} break;
	case SMUCM_COMPLETE_NEW_YEAR_MISSION:
		{
			if( g_pData == NULL || g_pData->GetMyUser() == NULL || g_pData->GetMyUser()->GetSelectUnit() == NULL )
			{
				return false;
			}

			CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
			if( pUnit->GetNewYearMissionStepID() == 0 )
			{
				m_pDLG2014Confirm = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29054 ), SMUCM_COMPLETE_NEW_YEAR_MISSION_OK , g_pMain->GetNowState(), SMUCM_COMPLETE_NEW_YEAR_MISSION_CANCEL );
			}
			else if( pUnit->GetNewYearMissionStepID() > 0 )
			{
				Handler_EGS_2014_EVENT_MISSION_COMPLETE_REQ();
			}
			return true;
		} break;
	case SMUCM_COMPLETE_NEW_YEAR_MISSION_OK:
		{
			if ( m_pDLG2014Confirm != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLG2014Confirm, NULL, false );

			Handler_EGS_2014_EVENT_MISSION_COMPLETE_REQ();

			m_pDLG2014Confirm		= NULL;
			return true;
		} break;
	case SMUCM_COMPLETE_NEW_YEAR_MISSION_CANCEL:
		{
			if ( m_pDLG2014Confirm != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLG2014Confirm, NULL, false );

			m_pDLG2014Confirm		= NULL;
			return true;
		} break;
	case SMUCM_NEW_YEAR_2014_UI_TOGGLE:
		{
			g_pMain->GetMemoryHolder()->SetShowNewYear2014EventBoard( !g_pMain->GetMemoryHolder()->GetShowNewYear2014EventBoard() );
			return true;
		} break;
	case SMUCM_NEW_YEAR_2014_UI_CLOSE:
		{
			g_pMain->GetMemoryHolder()->SetShowNewYear2014EventBoard( false );
			return true;
		} break;
	case SMUCM_SHOW_AERIA_USER_SUPPORT:
		{
#if !X2_DEV_TOOLS // 본섭
			std::wstring wstrURL = L"https://api.elswordonline.com/aeria/auth/gamesupport?SID=";
#else // 테섭
			std::wstring wstrURL = L"http://apitest.elswordonline.com/aeria/auth/gamesupport?SID=";
#endif // defined (_SERVICE_) && !defined (_OPEN_TEST_)
			wstrURL += g_pInstanceData->GetUserSessionID();
			x2::platform::OpenUrl( wstrURL );
			return true;
		} break;

	}

	return false;
}

bool CX2StateMenu::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2StateCommonBG::UIServerEventProc( hWnd, uMsg, wParam, lParam) == true )
		return true;

	if ( NULL !=g_pData->GetPremiumBuffManager() )
	{
		if ( true == g_pData->GetPremiumBuffManager()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if( NULL != g_pMain->GetCouponBox() )
	{
		if( true == g_pMain->GetCouponBox()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

	if ( m_pCashShop != NULL )
	{
		if ( m_pCashShop->UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
			return true;
	}

//#ifndef NEW_VILLAGE_UI
	//if ( NULL != m_pCharacterRoom )
	//{
	//	if( m_pCharacterRoom->UIServerEventProc( hWnd, uMsg, wParam, lParam ) == true )
	//		return true;
	//}
//#endif

	if( g_pMain->GetKeyPad() != NULL )
	{
		if( true == g_pMain->GetKeyPad()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}

    if( g_pMain->GetPostBox() != NULL )
    {
        if( true == g_pMain->GetPostBox()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
            return true;
    }

	if( g_pMain->GetSecurityPad() != NULL )
	{
		if( true == g_pMain->GetSecurityPad()->UIServerEventProc( hWnd, uMsg, wParam, lParam ) )
			return true;
	}


	switch ( wParam )
	{

		case EGS_SEARCH_UNIT_ACK:
			{
				return Handler_EGS_SEARCH_UNIT_ACK( hWnd, uMsg, wParam, lParam );

			}
			break;
		case EGS_GET_CONNECTION_UNIT_INFO_ACK:
			{
				return Handler_EGS_GET_CONNECTION_UNIT_INFO_ACK( hWnd, uMsg, wParam, lParam );
			}
			break;
		case EGS_BILL_PRODUCT_INFO_ACK:
			{
				return Handler_EGS_BILL_PRODUCT_INFO_ACK( hWnd, uMsg, wParam, lParam );
			}
			break;
//{{ kimhc	// 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리
		case EGS_CLEAR_TEMP_INVENTORY_NOT:
			{
				return Handler_EGS_CLEAR_TEMP_INVENTORY_NOT( hWnd, uMsg, wParam, lParam );
			} break;
//}} kimhc	// 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리

		//case EGS_OPEN_ARCADE_ROOM_LIST_ACK:
		//	return Handler_EGS_OPEN_ARCADE_ROOM_LIST_ACK( hWnd, uMsg, wParam, lParam );
		//	break;
		case EGS_GET_ADVERTISEMENT_EVENT_ACK:
			{
				return Handler_EGS_GET_ADVERTISEMENT_EVENT_ACK( hWnd, uMsg, wParam, lParam );
			} break;
		case EGS_WARP_BY_BUTTON_ACK:
			{
				return Handler_EGS_WARP_BY_BUTTON_ACK( hWnd, uMsg, wParam, lParam );
			} break;
		case EGS_UPDATE_WARP_VIP_NOT:
			{
				return Handler_EGS_UPDATE_WARP_VIP_NOT( hWnd, uMsg, wParam, lParam );
			} break;
		case EGS_2013_EVENT_MISSION_COMPLETE_ACK:
			{
				return Handler_EGS_2013_EVENT_MISSION_COMPLETE_ACK( hWnd, uMsg, wParam, lParam );
			} break;
		case EGS_2014_EVENT_MISSION_COMPLETE_ACK:
			{
				return Handler_EGS_2014_EVENT_MISSION_COMPLETE_ACK( hWnd, uMsg, wParam, lParam );
			} break;
	}

	return false;
}







bool CX2StateMenu::Handler_EGS_BILL_PRODUCT_INFO_REQ()
{
	KEGS_BILL_PRODUCT_INFO_REQ kPacket;
	kPacket.m_iPage = m_CashItemPage;

	g_pData->GetServerProtocol()->SendPacket( EGS_BILL_PRODUCT_INFO_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_BILL_PRODUCT_INFO_ACK );

	SAFE_DELETE_DIALOG( m_pDLGCashItemUpdate );
	m_pDLGCashItemUpdate = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_663 ), g_pMain->GetNowState() );

	return true;
}

bool CX2StateMenu::Handler_EGS_BILL_PRODUCT_INFO_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_BILL_PRODUCT_INFO_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	SAFE_DELETE_DIALOG( m_pDLGMsgBox );
	if( g_pMain->DeleteServerPacket( EGS_BILL_PRODUCT_INFO_ACK ) == true )
	{
		SAFE_DELETE_DIALOG( m_pDLGCashItemUpdate );
		g_pData->GetItemManager()->AddCashItem( kEvent.m_vecProductInfo );
		g_pData->GetItemManager()->SetKeepShowItemList( kEvent.m_mapKeepShowItem );
		g_pData->GetItemManager()->SetDisCountList( kEvent.m_DisCountInfoMap );

		if ( kEvent.m_bFinal == true )
		{
			m_CashItemPage = 1;
			g_pData->GetItemManager()->AddCashItemEx();
			if( m_pCashShop != NULL )
			{
				m_pCashShop->Handler_EGS_GET_WISH_LIST_REQ();
				m_pCashShop->SetOpen( true );
			}//if
			ChangeByToggleCashShop();
			//정렬 한번 해주자.. 무기부터..
		}
		else
		{
			m_CashItemPage++;
			Handler_EGS_BILL_PRODUCT_INFO_REQ();
		}

		return true;
	}
	return false;
}

bool CX2StateMenu::Handler_EGS_CASH_PRODUCT_INFO_CHANGED_NOT()
{
	if ( m_pCashShop != NULL )
	{
		if ( m_pCashShop->GetOpen() == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_664 ), this);
			ToggleCashShop();
		}
	}

	CX2State::Handler_EGS_CASH_PRODUCT_INFO_CHANGED_NOT();

	return true;
}

//{{ kimhc	// 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리
bool CX2StateMenu::Handler_EGS_CLEAR_TEMP_INVENTORY_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/ )
{
	g_pData->GetUIManager()->DestoryUITempInventory();

	return true;
}
//}} kimhc	// 실시간 엘소드 중 실시간 아이템 획득 관련 임시 인벤토리


bool CX2StateMenu::HandleMsgByESCImp()
{
	if ( g_pChatBox != NULL && g_pChatBox->GetFocus() == true )
	{
		g_pChatBox->HideChatEditBox();
		g_pChatBox->SetChatEnteredBefore( false );
		if ( g_pX2Game != NULL )
			g_pX2Game->SetOpenChatBox( false );
		if(g_pChatBox != NULL)
			g_pChatBox->SetFocusChatEditBox(false);
		return true;
	}

	if ( PutOffPopUpDialog() == true )
		return true;

	if ( m_pCashShop != NULL &&
		m_pCashShop->GetOpen() == true )
	{
		ToggleCashShop();
		return true;
	}
    else if( g_pMain->GetPostBox() != NULL && g_pMain->GetPostBox()->GetDialogShow() == true)
    {
        g_pMain->GetPostBox()->SetDialogShow(false);
        return true;
    }

	// 필드내 esc키 조건부 처리
	if(g_pTFieldGame != NULL )
	{
		if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PARTY) == true)
		{
			if( g_pData->GetLocationManager()->IsDungeonGate( g_pData->GetLocationManager()->GetCurrentVillageID() ) == false
				&& g_pData->GetPartyManager()->GetProcessPvpMatch() == false
				)
			{
				g_pMain->GetPartyUI()->OpenPartyDLG(false);
				return true;
			}
		}

		if(g_pTFieldGame->GetShowMarketList() == true)
		{
			g_pTFieldGame->CloseMarketList();
			return true;
		}

		if(g_pTFieldGame->GetJoinNpc() == true)
		{
			g_pTFieldGame->SetShowNpcMessage(false);
			return true;
		}

		if(g_pData->GetUIManager()->GetUIQuestNew() != NULL)
		{
			if(g_pData->GetUIManager()->GetUIQuestNew()->GetShowClearDLG() == true)
				return true;

		}

	}

	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
		{
			if( true == g_pData->GetLocationManager()->IsDungeonLounge( g_pData->GetLocationManager()->GetCurrentVillageID() ) )
			{
				SAFE_DELETE_DIALOG( m_pDLGLeavePartyOrLounge );
				m_pDLGLeavePartyOrLounge = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_665 ), SMUCM_LEAVE_DUNGEON_LOUNGE_OK, g_pMain->GetNowState(), SMUCM_LEAVE_DUNGEON_LOUNGE_CANCEL );
			}
			else
			{
				if ( NULL != g_pMain->GetKeyPad() && false == g_pMain->GetKeyPad()->GetShowKeyPad() )
					ToggleSystemMenu(); // 시스템UI 띄운다.
			}
		} break;

	case CX2Main::XS_BATTLE_FIELD:
		{
			if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PARTY) == true)
			{
				g_pMain->GetPartyUI()->OpenPartyDLG(false);
				return true;
			}

			if(g_pData->GetUIManager()->GetUIQuestNew() != NULL)
			{
				if(g_pData->GetUIManager()->GetUIQuestNew()->GetShowClearDLG() == true)
					return true;
			}

			if( true == g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
			{
				g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_29048 ),
					SMUCM_RAID_FIELD_EXIT_OK, g_pMain->GetNowState(), SMUCM_RAID_FIELD_EXIT_CANCEL );
				return true;
			}
			if ( NULL != g_pMain->GetKeyPad() && false == g_pMain->GetKeyPad()->GetShowKeyPad() )
				ToggleSystemMenu(); // 시스템UI 띄운다.
		} break;

	default:
		LastMsgByESC();
		break;
	}

	if ( NULL != g_pMain->GetKeyPad() )
		g_pMain->GetKeyPad()->ShowKeyPad( false );

	return false;
}

bool CX2StateMenu::ToggleLobbyMenuWindow()
{
	m_bShowWindowMenu = !m_bShowWindowMenu;

	if ( m_bShowWindowMenu == true )
	{
		SAFE_DELETE_DIALOG( m_pDLGMenuWindow );
		m_pDLGMenuWindow = new CKTDGUIDialog( this, L"DLG_Menu_Window.lua", 0.1f);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMenuWindow );
	}
	else
	{
		SAFE_DELETE_DIALOG( m_pDLGMenuWindow );
		//g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMenuWindow, NULL, false );
		//m_pDLGMenuWindow = NULL;
	}

	return true;
}


CX2Inventory::SORT_TYPE CX2StateMenu::GetInventorySortType( const WCHAR* pRadioButtonName )
{
	CX2Inventory::SORT_TYPE sortType = CX2Inventory::ST_NONE;

	if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_Equip" ) == 0 )
	{
		sortType = CX2Inventory::ST_EQUIP;
	}
	else if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_Accessory" ) == 0 )
	{
		sortType = CX2Inventory::ST_ACCESSORY;
	}
	else if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_UseItem" ) == 0 )
	{
		sortType = CX2Inventory::ST_QUICK_SLOT;
	}
	else if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_Material" ) == 0 )
	{
		sortType = CX2Inventory::ST_MATERIAL;
	}
	else if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_Card" ) == 0 )
	{
		sortType = CX2Inventory::ST_QUEST;
	}
	else if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_Special" ) == 0 )
	{
		sortType = CX2Inventory::ST_SPECIAL;
	}
	else if ( wcscmp( pRadioButtonName, L"RadioButtonUnit_Inventory_Avatar" ) == 0 )
	{
		sortType = CX2Inventory::ST_AVARTA;
	}


	return sortType;
}



void CX2StateMenu::UpdateUserInfoUI( KEGS_SEARCH_UNIT_ACK &kEvent )
{

	if( m_pDLGUserInfo == NULL )
		return;


	CKTDGUIStatic* pStatic_UnitFace = (CKTDGUIStatic*) m_pDLGUserInfo->GetControl( L"Static_UnitFace" );
	CKTDGUIStatic* pStatic_UnitInfo = (CKTDGUIStatic*) m_pDLGUserInfo->GetControl( L"Static_User_Info" );

	// 캐릭터 클래스 그림
	wstring fileName;
	wstring pieceName;
	if( true == CX2Data::GetCharacterImageName( fileName, pieceName, (CX2Unit::UNIT_CLASS) kEvent.m_kCUnitInfo.m_cUnitClass, CX2Data::CIT_Party ) )
	{
		pStatic_UnitFace->GetPicture(0)->SetTex( fileName.c_str(), pieceName.c_str() );
	}
	else
	{
		ASSERT( !"NO" );
	}




	// 캐릭터 레벨 이름
	pStatic_UnitInfo->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_667, "i", ( int )kEvent.m_kCUnitInfo.m_ucLevel ) );
	pStatic_UnitInfo->GetString(1)->msg = kEvent.m_kCUnitInfo.m_wstrCharName.c_str();

	// 전적
	pStatic_UnitInfo->GetString(3)->msg = GET_REPLACED_STRING( ( STR_ID_669, "i", kEvent.m_kCUnitInfo.m_iWin ) );

	CKTDGUIStatic* pStaticServerGroup = (CKTDGUIStatic*)m_pDLGUserInfo->GetControl( L"Static_SERVER" );

	if( pStaticServerGroup != NULL )
		pStaticServerGroup->SetShow( false );

	//엠블렘 표시 해줘야하는데 서버쪽에서 아직 작업이 덜 되었다.
	CKTDGUIStatic* pStaticEmblem = (CKTDGUIStatic*)m_pDLGUserInfo->GetControl( L"UnitEmblem" );
	if ( pStaticEmblem != NULL && pStaticEmblem->GetPicture(0) != NULL )
	{
		char cRank = kEvent.m_kCUnitInfo.m_cRank;
		CX2PVPEmblem::PVPEmblemData* pPVPEmblemData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( static_cast<CX2PVPEmblem::PVP_RANK>(cRank) );

		if ( pPVPEmblemData != NULL )
		{
			pStaticEmblem->GetPicture(0)->SetTex( pPVPEmblemData->m_TextureName.c_str(), pPVPEmblemData->m_TextureKey.c_str() );
		}
	}

	// 같이하기 버튼
	CKTDGUIButton* pButton_PlayTogether = (CKTDGUIButton*) m_pDLGUserInfo->GetControl( L"Button_Play_Together" );


	// fix!!!!! 접속 상황
	//switch( kEvent.m_kCUnitInfo.m_cConnectionState )
	//{
	//default:
	//case CX2Unit::CUS_NONE:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"알수 없음";
	//		pButton_PlayTogether->SetEnable( false );
	//		pButton_PlayTogether->SetShow( false );

	//	} break;
	//case CX2Unit::CUS_ONLINE:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"온라인";
	//		pButton_PlayTogether->SetEnable( false );
	//		pButton_PlayTogether->SetShow( false );

	//	} break;
	//case CX2Unit::CUS_SQUARE:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"광장";
	//		pButton_PlayTogether->SetEnable( false );
	//		pButton_PlayTogether->SetShow( false );

	//	} break;
	//case CX2Unit::CUS_PVP_WAIT:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"대전방 대기중";

	//	} break;

	//case CX2Unit::CUS_PVP_PLAY:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"대전 게임중";

	//	} break;

	//case CX2Unit::CUS_DUNGEON_WAIT:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"던전방 대기중";

	//	} break;

	//case CX2Unit::CUS_DUNGEON_PLAY:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"던전 게임중";
	//		pButton_PlayTogether->SetEnable( false );
	//		pButton_PlayTogether->SetShow( false );
	//	} break;

	//case CX2Unit::CUS_ARCADE_PLAY:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"아케이드 게임중";
	//		pButton_PlayTogether->SetEnable( false );
	//		pButton_PlayTogether->SetShow( false );
	//	} break;

	//case CX2Unit::CUS_ARCADE_WAIT:
	//	{
	//		pStatic_UnitInfo->GetString(6)->msg = L"아케이드 대기중";
	//	} break;
	//}

}



bool CX2StateMenu::InitUI()
{
	m_pDLGMenu = g_pMain->GetMemoryHolder()->GetMenuDLG( this );
	m_vecpButtonMainMenu.clear();
	UpdateMenuPosistion();
	UpdateMenuString();



	if( NULL != g_pMain->GetCouponBox() )
	{
		g_pMain->GetCouponBox()->SetNowState( (CX2State*) this );
	}

    if( NULL != g_pMain->GetPostBox() )
    {
        g_pMain->GetPostBox()->SetNowState( (CX2State*) this );
    }

	switch(g_pMain->GetNowStateID())
	{
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_PVP_GAME:
		{
			for( int i=0; i<m_pDLGMenu->GetControlSize(); i++ )
			{
				CKTDGUIControl* pControl = m_pDLGMenu->GetControl( i );
				if( NULL == pControl )
					continue;

				if( CKTDGUIControl::UCT_BUTTON != pControl->GetType() )
					continue;

				CKTDGUIButton* pButton = (CKTDGUIButton*) pControl;
				switch(	pButton->GetCustomMsgMouseUp() )
				{
					// 퀘스트랑 커뮤니티만 활성화
				case CX2StateMenu::SMUCM_MY_INFO_QUEST:
				case CX2StateMenu::SMUCM_COMMUNITY:
		//{{ kimhc // 실시간 엘소드 중 실시간 인벤토리 열기 및 갱신
				case CX2StateMenu::SMUCM_MY_INFO:
				case CX2StateMenu::SMUCM_INVENTORY:
		//}} kimhc // 실시간 엘소드 중 실시간 인벤토리 열기 및 갱신
				case CX2StateMenu::SMUCM_MY_INFO_SKILL_TREE:
				case CX2StateMenu::SMUCM_PET_LIST:

					{
						pButton->SetEnable(true);
					} break;
				default:
					{
						pButton->SetEnable(false);

					} break;
				}
			}
		} break;

	case CX2Main::XS_PVP_ROOM:
	case CX2Main::XS_PVP_LOBBY:
	case CX2Main::XS_SQUARE_GAME:
	case CX2Main::XS_WEDDING_GAME:
		{
			for( int i=0; i<m_pDLGMenu->GetControlSize(); i++ )
			{
				CKTDGUIControl* pControl = m_pDLGMenu->GetControl( i );
				if( NULL == pControl )
					continue;

				if( CKTDGUIControl::UCT_BUTTON != pControl->GetType() )
					continue;

				CKTDGUIButton* pButton = (CKTDGUIButton*) pControl;
				switch(	pButton->GetCustomMsgMouseUp() )
				{
					// 파티만 비활성화
				case CX2StateMenu::SMUCM_PARTY:
				case CX2StateMenu::SMUCM_PET_LIST:
				case CX2StateMenu::SMUCM_SYSTEM:
					{
						pButton->SetEnable(false);

					} break;
				default:
					{
						pButton->SetEnable(true);

					} break;
				}
			}

		} break;

	default:
		{
			for( int i=0; i<m_pDLGMenu->GetControlSize(); i++ )
			{
				CKTDGUIControl* pControl = m_pDLGMenu->GetControl( i );
				if( NULL == pControl )
					continue;

				if( CKTDGUIControl::UCT_BUTTON != pControl->GetType() )
					continue;

				CKTDGUIButton* pButton = (CKTDGUIButton*) pControl;


					pControl->SetEnable(true);
			}

		} break;
	}
	if( NULL != g_pData && NULL != g_pData->GetWorldMissionManager() &&
		NULL != g_pData->GetWorldMissionManager()->GetUIWorldMission() )
		g_pData->GetWorldMissionManager()->GetUIWorldMission()->SetForceUpdateCrystalUI();

	RefreshMenuInfo();
	return true;
}

bool CX2StateMenu::InitETC()
{

	//if ( g_pData->GetCharacterRoom() == NULL )
	//{
	//	g_pData->ResetCharacterRoom( this, L"DLG_Unit_State_SlotList.lua" );
	//}

	//m_pCharacterRoom = g_pData->GetCharacterRoom();
	//m_pCharacterRoom->SetNowStage( this );
	//m_pCharacterRoom->SetOpen( false );


	if ( g_pData->GetCashShop() == NULL )
	{
		g_pData->ResetCashShop( this, L"DLG_Cash_Shop_Slot_List.lua" );
	}


	m_pCashShop = g_pData->GetCashShop();
    ASSERT( m_pCashShop != NULL );
    if( m_pCashShop != NULL && m_pCashShop->GetOpen() )
    {
	    m_pCashShop->SetOpen( false );
    }//if

	CX2Community* pMessenger = g_pData->GetMessenger();
	if ( pMessenger != NULL )
	{
		pMessenger->SetStage( this );
		pMessenger->SetOpenFriendReqMsgBox( true );
	}

	g_pMain->GetInformerManager()->CheckInfo( true );


	return true;
}


void CX2StateMenu::PopUpMenuClose( HWND /*hWnd*/, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if  ( ( uMsg == CKTDXApp::KM_UI_CONTROL_EVENT ||
		uMsg == CKTDXApp::KM_UI_CONTROL_CUSTOM_EVENT ||
		uMsg == CKTDXApp::KM_UI_CONTROL_CUSTOM_FUNC ) && ( (STATE_MENU_UI_CUSTOM_MSG)wParam != SMUCM_MENU ) )
	{
		CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
		if( NULL != pControl )
		{
			CKTDGUIDialogType pDialog = pControl->GetDialog();
			if ( pDialog != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGMenuWindow, NULL, false );
				m_pDLGMenuWindow = NULL;

				m_bShowWindowMenu = false;
			}
		}
	}





}

bool CX2StateMenu::GetShowMsgBox()
{
	CKTDGUIDialogType pDialog = NULL;

	if(g_pKTDXApp->GetDGManager()->GetDialogManager() != NULL)
	{
		pDialog = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetDialog(L"DLG_OKMsgBox");
		if(pDialog != NULL)
		{
			return pDialog->GetShow();
		}

		pDialog = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetDialog(L"DLG_MsgBox");
		if(pDialog != NULL)
		{
			return pDialog->GetShow();
		}

		pDialog = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetDialog(L"OkAndCancelMsgBox");
		if(pDialog != NULL)
		{
			return pDialog->GetShow();
		}
	}

	return false;
}

void CX2StateMenu::OpenFindUserDLG()
{
	SAFE_DELETE_DIALOG( m_pDLGFindUser );
	m_pDLGFindUser = new CKTDGUIDialog( this, L"DLG_Find_User.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGFindUser );


	CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*)m_pDLGFindUser->GetControl( L"ComboBoxUser_Search_Window" );

	if ( g_pData->GetMessenger() != NULL && pComboBox != NULL )
	{
		vector< wstring > vecFriendNameList;
		g_pData->GetMessenger()->GetFriendNameList( vecFriendNameList );
		for ( int i = 0; i < (int)vecFriendNameList.size(); i++ )
		{
			wstring friendName = vecFriendNameList[i];
			pComboBox->AddItem( friendName.c_str(), NULL );
		}
	}

	//{{ 2007. 11. 13  최육사  안쓰는 통계정리
	//g_pData->GetServerProtocol()->SendID( EGS_OPEN_WND_FINDUSER_NOT );
	//}}
}

void CX2StateMenu::SetShowDlgMenu(bool val)
{
	if( val )
	{
		if ( m_pDLGMenu != NULL )
			m_pDLGMenu->SetShowEnable(m_bShowMenu,m_bShowMenu);

		if( NULL != g_pData && NULL != g_pData->GetUIManager() )
		{
			if( NULL != CX2RidingPetManager::GetInstance()->GetMyRidingPet() )
			{
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_SLOT, false);
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_RIDING_SKILL_SLOT, g_pData->GetUIManager()->GetShowRidingPetSkillslot());
			}
			else
			{
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_SLOT, g_pData->GetUIManager()->GetShowSkillslot());
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_RIDING_SKILL_SLOT, false);
			}
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUICK_SLOT, g_pData->GetUIManager()->GetShowQuickSlot() );
		}
	}
	else
	{
		if ( m_pDLGMenu != NULL )
			m_pDLGMenu->SetShowEnable(false,false);

		if( NULL != g_pData && NULL != g_pData->GetUIManager() )
		{
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_SLOT, false);
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUICK_SLOT, false);
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_RIDING_SKILL_SLOT, false);
		}
	}
}

void CX2StateMenu::SetShowMenu()
{
	if ( m_pDLGMenu != NULL )
	{
		m_pDLGMenu->SetShow( true );
		m_pDLGMenu->SetEnable( true );
	}
	// 2009.01.29 김태완 : 퀵슬롯 SetShow 타이밍 바꿈
	g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUICK_SLOT, true);
}



void CX2StateMenu::OnClickedMenuSkillButton()
{
	// 스킬 트리랑 콤보 트리 둘다 동시에 열려있는 경우는 없어야 한다
	ASSERT( false == ( g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_SKILL ) && g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_COMBO_TREE ) ) );
	if( false == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_SKILL ) &&
		false == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_COMBO_TREE ) )
	{
		g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_SKILL );
	}
	else
	{
		if( true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_SKILL ) )
		{
			g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_SKILL );
		}

		if( true == g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_COMBO_TREE ) )
		{
			g_pData->GetUIManager()->ToggleUI( CX2UIManager::UI_MENU_COMBO_TREE );
		}
	}
}

bool CX2StateMenu::ShortCutKeyProcess()
{


	if ( CX2State::ShortCutKeyProcess() == true )
		return true;

#if !X2_DEV_TOOLS /// 사내에서는 제외
	if( NULL != g_pMain && true == g_pMain->GetIsPlayingFreeTraining() )
	{
		CameraZoomInAndOutKeyProcess();
		return true;		/// 카메라 줌 기능 검사 후, 여부에 상관없이 바로 탈출
	}
#endif // _IN_HOUSE_

	//스킬

	bool bHideDialog = false;
	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		bHideDialog = true;


	if(NULL != g_pMain && NULL != g_pMain->GetPartyUI() && false == g_pMain->GetPartyUI()->GetShowLocalMap())
	if( bHideDialog == false && GET_KEY_STATE( GA_SKILL_TREE ) == TRUE )
	{
		OnClickedMenuSkillButton();
		return true;
	}



	if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F5 ) == TRUE )
	{
		if( g_pSquareGame == NULL )
		{
			CX2GameOption* pGameOption = &g_pMain->GetGameOption();
			pGameOption->SetFieldParty( !pGameOption->GetFieldParty() );
		}
	}

	// temp!!! 새마을 NPC 보이기, 안보이기
	//if ( g_pData->GetMyUser()->GetAuthLevel() == CX2User::XUAL_DEV )
	//{
	//	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F11 ) == TRUE )
	//	{
	//		if( CX2Main::XS_VILLAGE_MAP == g_pMain->GetNowStateID() &&
	//			NULL != g_pTFieldGame )
	//		{
	//			g_pTFieldGame->ToggleEnableNPC();
	//		}
	//	}
	//}


	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F12) == TRUE
		&& false == g_pMain->GetIsPlayingTutorial()
		)
	{
		if(false == m_Book.GetShow())
		{
			m_Book.SetBookTable( "BT_ELLIOS_GUIDE");
			m_Book.SetShow(true);
		}
	}

	// 게임가이드 on/off
	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F1) == TRUE )
	{
		wstring wstrSystemMessageColor = L"#CFF3F4D";								// 255, 63, 77
		D3DXCOLOR coSystemMessageTextColor(1.f, 0.24705f, 0.30196f, 1.f);			// 255, 63, 77

		g_pMain->GetGameOption().SetPlayGuide( !g_pMain->GetGameOption().GetOptionList().m_bPlayGuide );
		g_pMain->GetGameOption().SaveScriptFile();
		if( NULL !=g_pData && NULL != g_pData->GetPlayGuide() && NULL != g_pData->GetPlayGuide()->GetDlgPlayGuide())
		{
			bool bShow = g_pMain->GetGameOption().GetOptionList().m_bPlayGuide;
			g_pData->GetPlayGuide()->GetDlgPlayGuide()->SetShowEnable(bShow, bShow);
		}
		if( m_bIsOptionWindowOpen == true )
		{
			InitOtherOption();
		}

		if( true == g_pMain->GetGameOption().GetOptionList().m_bPlayGuide )
		{
			if( NULL != g_pChatBox )
			{

				g_pChatBox->AddChatLog( GET_STRING( STR_ID_567 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
			}
		}
		else
		{
			if( NULL != g_pChatBox )
			{

				g_pChatBox->AddChatLog( GET_STRING( STR_ID_568 ), KEGS_CHAT_REQ::CPT_SYSTEM, coSystemMessageTextColor, wstrSystemMessageColor );
			}
		}

		return true;
	}

	// 전캐릭 SD캐릭터 표시
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_SQUARE_GAME:
		{
			if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F6 ) == TRUE )
			{
				CX2GameOption* pGameOption = &g_pMain->GetGameOption();
				if( NULL != pGameOption )
					pGameOption->SetFieldSD( !pGameOption->GetFieldSD() );
			}

			if ( CameraZoomInAndOutKeyProcess() )
				return true;
		} break;
	default:
		break;
	}
	// 공용키처리
	return g_pMain->KeyProcess();
}

void CX2StateMenu::ChatProcess()
{
}

void CX2StateMenu::RefreshMenuInfo()
{
	if ( m_pDLGMenu != NULL )
	{
		UpdateExpGageBar();
		CKTDGUIStatic* pStatic;
		pStatic = (CKTDGUIStatic*)m_pDLGMenu->GetControl( L"StaticMenu_Button_Tired_Gage" );
		float gauagePercentage = (g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpirit / (float)g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpiritMax );

		if ( pStatic != NULL && pStatic->GetPicture(0) != NULL )
		{
			float _width = (g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpirit / (float)g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpiritMax ) * pStatic->GetPicture(0)->GetOriginalSize().x;
			pStatic->GetPicture(0)->SetSizeX( _width );
		}

		//마우스 오버 시 근성도 퍼센트
		CKTDGUIButton* pSpiritInvisibleButton = static_cast< CKTDGUIButton* >(m_pDLGMenu->GetControl( L"Button_Invisible_SpiritGuage" ));
		if( NULL != pSpiritInvisibleButton )
		{
			WCHAR buff[256] = L"";
			float fNowSpirit = static_cast<float>( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpirit );
			fNowSpirit = fNowSpirit / static_cast<float>( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpiritMax ) * 100.0f;
			fNowSpirit = floor(fNowSpirit * 10.0f + 0.5f) / 10.0f;
			StringCchPrintfW( buff, ARRAY_SIZE(buff), L"%.1f%%", fNowSpirit );
			pSpiritInvisibleButton->SetGuideDesc(buff);
			pSpiritInvisibleButton->SetShowEnable(true,true);
		}
		//}}
		if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIInventory() )
		{
			g_pData->GetUIManager()->GetUIInventory()->SetEDString( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED );
			g_pData->GetUIManager()->GetUIInventory()->SetAPString( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint );
		}

	}
}


void CX2StateMenu::UpdateExpGageBar()
{
	if( NULL == g_pData->GetMyUser()->GetSelectUnit() )
		return;

//{{ kimhc // 실시간 엘소드 중 실시간 경험치 획득

	float	nowExp		= static_cast< float >( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_EXP );
	int		iOrgLevel	= g_pData->GetSelectUnitLevel();			// 캐릭터 정보에 저장된 level (Level up 전)


//{{ 오현빈 // 2012-04-16 // UI개편_하단메뉴

	float fTotalExpRate		= 0.0f;
	float nowBaseExp		= 0;

	nowBaseExp		= static_cast< float >( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_NowBaseLevelEXP );
	fTotalExpRate	= ( nowExp - nowBaseExp ) / g_pData->GetEXPTable()->GetEXPData( iOrgLevel + 1 ).m_nNeedExp;

	if( fTotalExpRate > 1.f )
	{
		fTotalExpRate = 1.f;
	}
	else if( fTotalExpRate < 0.f )
	{
		fTotalExpRate = 0.f;
	}

	CKTDGUIStatic* pStatic;
	//EXP게이지
	pStatic = static_cast< CKTDGUIStatic* >( m_pDLGMenu->GetControl( L"StaticMenu_Button_EXP_Gauge" ) );

	if ( pStatic != NULL && pStatic->GetPicture(0) != NULL )
	{
		pStatic->GetPicture(0)->SetSizeX( fTotalExpRate * pStatic->GetPicture(0)->GetOriginalSize().x );
	}

	//마우스 오버 시 경험치 퍼센트
	CKTDGUIButton* pExpInvisibleButton = static_cast< CKTDGUIButton* >(m_pDLGMenu->GetControl( L"Button_Invisible_ExpGuage" ));
	if( NULL != pExpInvisibleButton )
	{
		WCHAR buff[256] = L"";
		StringCchPrintfW( buff, ARRAY_SIZE(buff), L"%.1f%%", fTotalExpRate * 100.f );
		pExpInvisibleButton->SetGuideDesc(buff);
		pExpInvisibleButton->SetShowEnable(true, true);
	}
	//}}
//}} 오현빈 // 2012-04-16 // UI개편_하단메뉴
//}} kimhc // 실시간 엘소드 중 실시간 경험치 획득
}


void CX2StateMenu::ToggleCashShop()
{
	CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
	if( true == battleFieldManager.GetIsBossRaidCurrentField() )
	{
		g_pChatBox->AddChatLog(  GET_STRING( STR_ID_28379 ), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
		return;
	}

	if( NULL != g_pData->GetPartyManager() &&
		true == g_pData->GetPartyManager()->GetProcessPvpMatch() )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_13599 ), this );
		return;
	}




	if( NULL != g_pData->GetUIManager() &&
		(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE) ||
		true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP)) )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(270,350), GET_STRING( STR_ID_659 ), g_pMain->GetNowState() );
		return;
	}

	// 체험 아이디 제한
	if( true == g_pData->GetMyUser()->GetUserData().m_bIsGuestUser )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(270,350), GET_STRING( STR_ID_40 ), g_pMain->GetNowState() );
		return;
	}

    ASSERT( m_pCashShop != NULL );
    if( m_pCashShop == NULL )
    {
        return;
    }//if

	if ( m_pCashShop->GetOpen() == false )
	{
		//Open!!

		// 게임내에서 바쁜 상태인 경우 캐시샵을 열 수 없음
		if ( NULL != g_pX2Game && g_pX2Game->CheckAndWarningBusyStateNow() )
			return ;


		//{{ kimhc // 2009-12-19 // 다른 서버군에 접속해서 캐시샵 버튼 클릭시 다른 리스트 받아오도록
		if ( g_pInstanceData->GetServerGroupIDCashShop() != g_pInstanceData->GetServerGroupID() )
		{
			g_pData->GetItemManager()->ClearCashItemList();
			g_pInstanceData->SetServerGroupIDCashShop( g_pInstanceData->GetServerGroupID() );
		}
		//}} kimhc // 2009-12-19 // 다른 서버군에 접속해서 캐시샵 버튼 클릭시 다른 리스트 받아오도록


		if ( g_pData->GetItemManager()->GetIsUpdateCashItem() == false )
		{

			Handler_EGS_BILL_PRODUCT_INFO_REQ();
		}
		else
		{
			CX2State* pNowState = (CX2State*)g_pMain->GetNowState();
			pNowState->Handler_EGS_CHECK_BALANCE_REQ();
			m_pCashShop->SetOpen( true );
			ChangeByToggleCashShop();
		}
		g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUICK_SLOT, false);
		g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_SLOT, false);
		if( NULL != g_pData->GetUIManager() )
			g_pData->GetUIManager()->SetShowRidingPetSkillSlot(false);
	}
	else
	{
		//close!!
		m_pCashShop->SetOpen( false );
		ChangeByToggleCashShop();
		g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_QUICK_SLOT, true);
		if( NULL != g_pData->GetUIManager() && NULL != CX2RidingPetManager::GetInstance() && true == CX2RidingPetManager::GetInstance()->GetRidingOn() )
		{
			g_pData->GetUIManager()->SetShowRidingPetSkillSlot(true);
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_SLOT, false );
		}
		else
		{
			g_pData->GetUIManager()->SetShowRidingPetSkillSlot(false);
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_SLOT, true );
		}
	}
}

void CX2StateMenu::ChangeByToggleCashShop()
{
    ASSERT( m_pCashShop != NULL );
    if( m_pCashShop == NULL )
    {
        return;
    }

	SetShowStateDLG( !m_pCashShop->GetOpen() );

	if ( g_pData->GetMessenger() != NULL )
		g_pData->GetMessenger()->SetOpen( false );



	g_pData->GetUIManager()->CloseAll();

}

void CX2StateMenu::CheckPresentArrival()
{
	PresentArrival presentArrival;
	if ( m_pDLGPresentArrival == NULL && g_pMain->GetPresentArrival( presentArrival ) == true )
	{
		m_pDLGPresentArrival = new CKTDGUIDialog( this, L"DLG_Present_Arrival.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGPresentArrival );

		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( presentArrival.m_kBillProductInfo.m_iProductID );
		if ( pItemTemplet != NULL )
		{
			CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDLGPresentArrival->GetControl( L"g_pStaticCashShop_PresentWindow_Accept_Slot" );
			if ( pStatic != NULL && pStatic->GetPicture( 0 ) != NULL )
			{
				pStatic->GetPicture( 0 )->SetTex(
                    pItemTemplet->GetShopImage()
                    );
			}

			pStatic = (CKTDGUIStatic*)m_pDLGPresentArrival->GetControl( L"g_pStatic_Item_ID_1Line" );
			if ( pStatic != NULL && pStatic->GetString(0) != NULL )
			{
                wstring tempName = pItemTemplet->GetName();
				wstringstream wstrstm;

				if ( presentArrival.m_kBillProductInfo.m_cPeriod != 0 )
				{
					wstrstm << L" (" << static_cast<int>(presentArrival.m_kBillProductInfo.m_cPeriod) << GET_STRING( STR_ID_14 ) << L")";
				}
				else
				{
					wstrstm << L" (" << static_cast<int>(presentArrival.m_kBillProductInfo.m_cQuantity) << GET_STRING( STR_ID_24 ) << L")";
				}
				tempName += wstrstm.str().c_str();
				tempName = CWordLineHandler::GetStrByLineBreakInX2Main( tempName.c_str(), 214, pStatic->GetString(0)->fontIndex );
				pStatic->SetString( 0, tempName.c_str() );
			}
		}


		CKTDGUIStatic* pStaticMsg = (CKTDGUIStatic*)m_pDLGPresentArrival->GetControl( L"Present_Message" );
		if ( pStaticMsg != NULL )
		{
			pStaticMsg->SetString( 0, presentArrival.m_PresentMsg.c_str() );
		}

		pStaticMsg = (CKTDGUIStatic*)m_pDLGPresentArrival->GetControl( L"User_Name" );
		if ( pStaticMsg != NULL  )
		{
			pStaticMsg->SetString( 0, GET_REPLACED_STRING( ( STR_ID_672, "L", presentArrival.m_SenderNickName ) ) );
		}
	}
}


bool CX2StateMenu::ToggleSystemMenu(bool bShow)
{
	// bShow가 true면 자동 토글
	// bShow가 false면 강제로 끈다.

	if(bShow == true)
	{
		m_bShowSystemMenu = !m_bShowSystemMenu;

		if ( m_bShowSystemMenu == true )
		{
			if(m_pDLGSystem == NULL)
			{
				//SAFE_DELETE_DIALOG( m_pDLGSystem );
				if( true == g_pInstanceData->IsAeriaChanneling() )
					m_pDLGSystem = new CKTDGUIDialog( this, L"DLG_UI_System_New_For_Aeria.lua", 0.1f);
				else
				m_pDLGSystem = new CKTDGUIDialog( this, L"DLG_UI_Entry_Point_System_New.lua", 0.1f);
				g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGSystem );
				m_pDLGSystem->SetDisableUnderWindow(true);

				//m_pDLGSystem->SetLayer(XDL_POP_UP);
				//m_pDLGSystem->SetFront(true);
				//m_pDLGSystem->SetPos(D3DXVECTOR2(434, 256));
			}
			else
			{
				m_pDLGSystem->SetShowEnable(true, true);
				m_pDLGSystem->RequestFocus( m_pDLGSystem->GetControl(L"SystemMenuOption") );
				m_pDLGSystem->GetControl(L"SystemMenuOption")->OnFocusIn();
			}
		}
		else
		{
			m_pDLGSystem->SetShowEnable(false, false);
			//SAFE_DELETE_DIALOG( m_pDLGSystem );
		}
	}
	else
	{
		if(m_bShowSystemMenu == false)
			return true;

		m_bShowSystemMenu = false;

		if(m_pDLGSystem != NULL)
		{
			m_pDLGSystem->SetShowEnable(false, false);
			//SAFE_DELETE_DIALOG( m_pDLGSystem );
		}
	}

	// 09.03.06 김태완 : 시스템창 켜질때 파티창 disable
	if(g_pMain->GetPartyUI() != NULL)
	{
		g_pMain->GetPartyUI()->EnableDialog( !m_bShowSystemMenu );
	}

	return true;
}



//{{ 09.03.24 김태완 : 구 CharacterRoom에서 이사온 코드
void CX2StateMenu::CheckMyInfoChange()
{
	if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIInventory() &&
		true == g_pData->GetUIManager()->GetUIInventory()->GetShow() )
	{
		if ( m_TotalED < g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED )
		{
			int changeED = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED - m_TotalED;
			wstringstream wstrstm;
			wstrstm << L"ED +" << changeED;
			m_pPicCharMenuPlusInfo->DrawText( wstrstm.str().c_str(), D3DXVECTOR3( 789,687,0 ), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_CENTER );
		}

		if ( m_TotalED > g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED )
		{
			int changeED = m_TotalED - g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED;
			wstringstream wstrstm;
			wstrstm << L"ED -" << changeED;
			m_pPicCharMenuPlusInfoRed->DrawText( wstrstm.str().c_str(), D3DXVECTOR3( 789,687,0 ), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_CENTER );
		}

		m_TotalED = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED;

		//{{ kimhc // 실시간 엘소드 중 실시간 ED 획득 관련
		// 던전 이탈시 ED에 패널티가 적용된 메뉴 효과를 보여주기위한 용도
		if ( g_pInstanceData != NULL &&
			g_pInstanceData->GetInstanceED() != 0 )
		{
			if( g_pInstanceData->GetInstanceED() > m_TotalED )
			{
				int changeED = g_pInstanceData->GetInstanceED() - m_TotalED;
				wstringstream wstrstm;
				wstrstm << L"ED -" << changeED;
				m_pPicCharMenuPlusInfoRed->DrawText( wstrstm.str().c_str(), D3DXVECTOR3( 789,687,0 ), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_CENTER );

				g_pInstanceData->SetInstanceED( 0 );
			}
			else
			{
				// oasis907 : 김상윤 [2011.12.28] 그 외의 경우. ED페널티 없는 던전( 자유훈련 )
				g_pInstanceData->SetInstanceED( 0 );
			}
		}
		//}} kimhc // 실시간 엘소드 중 실시간 ED 획득 관련
	}
	else
	{
		if( NULL != m_pPicCharMenuPlusInfo )
			m_pPicCharMenuPlusInfo->Clear();
		if( NULL != m_pPicCharMenuPlusInfoRed)
			m_pPicCharMenuPlusInfoRed->Clear();
	}
	if ( m_TotalAP < g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint )
	{
		int changeAP = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint - m_TotalAP;
		wstringstream wstrstm;
		wstrstm << L"AP +" << changeAP;
		m_pPicCharMenuPlusInfo->DrawText( wstrstm.str().c_str(), D3DXVECTOR3( 940,687,0 ), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_CENTER );
	}

	if ( m_TotalAP > g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint )
	{
		int changeAP = m_TotalAP - g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint;
		wstringstream wstrstm;
		wstrstm << L"AP -" << changeAP;
		m_pPicCharMenuPlusInfoRed->DrawText( wstrstm.str().c_str(), D3DXVECTOR3( 940,687,0 ), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_CENTER );
	}
	m_TotalAP = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint;

	if ( m_TotalLevel < g_pData->GetSelectUnitLevel() )
	{

		D3DXVECTOR2 vPos(698, 730);

		for ( int i = 0; i < 3; i++ )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
			pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"HyperBallTick",
				vPos.x, vPos.y,
				0.0f, 1000,1000, -1, 10 );
			if( pSeq != NULL )
			{
				pSeq->SetOverUI( true );
				pSeq->SetBlackHolePosition( pSeq->GetPosition() );
				pSeq->UseLookPoint( true );
				pSeq->SetLookPoint( pSeq->GetPosition() );
			}
			pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"HyperBallRing",
				vPos.x, vPos.y,
				0.0f, 1000,1000, -1, 1 );
			if( pSeq != NULL )
				pSeq->SetOverUI( true );
		}
	}

	m_TotalLevel = g_pData->GetSelectUnitLevel();


}
//}}

void CX2StateMenu::AddStringRemainTime( OUT wstringstream& wstreamDesc_, IN const int iRemainTimeWorldBuff_ )
{
	wstreamDesc_ << '\n' << '\n';
	wstreamDesc_ << GET_STRING( STR_ID_11537 );

	// 60초 보다 크면 분으로 표시
	if ( 60 < iRemainTimeWorldBuff_ )
	{
		wstreamDesc_ << iRemainTimeWorldBuff_ / 60 << GET_STRING( STR_ID_311  );
	}
	// 60초 보다 작으면 초로 표시
	else
	{
		wstreamDesc_ << iRemainTimeWorldBuff_ << GET_STRING( STR_ID_180  );
	}

	wstreamDesc_ << '\n';
}
//}} kimhc // 2011-04-22 // 디펜스던전 월드 버프 UI

//{{ JHKang / 강정훈 / 2011.6.26 / 프리미엄 버프
/** @function 	: UpdateGuideDescPremiumBuffUI
	@brief 		: 프리미엄 버프(마우스 업 이벤트 시 설명)
	@param		: iRemainTimeBuff_(남은 시간)
*/
void CX2StateMenu::UpdateGuideDescPremiumBuffUI()
{
	if ( NULL != g_pData && NULL != g_pData->GetPremiumBuffManager() )
		return;

	if ( g_pData->GetPremiumBuffManager()->GetPremiumBuffList().empty() == false )
	{
		for( CX2PremiumBuffIconPtr pPremiumBuffPtr : g_pData->GetPremiumBuffManager()->GetPremiumBuffList() )
		{
			if( pPremiumBuffPtr->GetPremiumBuffID() != 16 )
				UpdateGuideDescBuffTemplet( pPremiumBuffPtr );
		}
	}
}
//}}

//{{ kimhc // 2011-08-08 // 투니 랜드 채널링 이벤트
//}} kimhc // 2011-08-08 // 투니 랜드 채널링 이벤트

bool CX2StateMenu::Handler_EGS_ADD_ON_STAT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ADD_ON_STAT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( NULL != g_pX2Game )
		return g_pX2Game->Handler_EGS_ADD_ON_STAT_ACK( kEvent );
	else
		return false;
}

bool CX2StateMenu::Handler_EGS_ADD_ON_STAT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ADD_ON_STAT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( NULL != g_pX2Game )
		return g_pX2Game->Handler_EGS_ADD_ON_STAT_NOT( kEvent );
	else
		return false;
}

//{{ 2010. 12. 23 김현철	장비 변경 패킷 타입

bool CX2StateMenu::Handler_EGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	CX2GUUser*	pUser = ( NULL != g_pX2Game ? g_pX2Game->GetUserUnitByUID( kEvent.m_UnitUID ) : NULL );
	if ( pUser == NULL )
		return false;

	CX2Unit* pUnit = pUser->GetUnit();
	if ( pUser->GetUnit() == NULL )
		return false;

	CX2Unit::UnitData*	pUnitData	= &pUnit->AccessUnitData();

	pUser->SetGameStat( kEvent.m_kGameStat );
	pUnitData->m_GameStat.SetKStat( kEvent.m_kGameStat );

	pUser->SetMaxHp( static_cast< const float >( kEvent.m_kGameStat.m_iBaseHP ) );
	if ( pUser->GetNowHp() > pUser->GetMaxHp() )
	{
		pUser->SetNowHp( pUser->GetMaxHp() );
	}

	switch ( kEvent.m_cChangeEquippedPacketType )
	{
		// 타이틀이 변경 되었을 경우
	case KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT::CEPT_CHANGE_TITLE:
		{
			pUnit->SetTitleId( kEvent.m_iEquippedTitleID );
			pUser->UpdateEquippedEmblem();
			pUser->NotifyShowObjectChanged();
		} break;

		// 장비가 변경 되었을 경우
	case KEGS_CHANGE_EQUIPPED_ITEM_IN_ROOM_NOT::CEPT_CHANGE_EQUIP:
		{
			CX2Inventory& kInventory = pUnit->AccessInventory();

			const UidType myUID = g_pData->GetMyUser()->GetSelectUnit()->GetUID();

			// 파티원이면
			if ( myUID != kEvent.m_UnitUID )
			{
				for( KInventoryItemInfo& kInventoryItemInfo : kEvent.m_vecInventorySlotInfo )
				{
					CX2Item* pItemToRemove = kInventory.GetItem( static_cast< CX2Inventory::SORT_TYPE >( kInventoryItemInfo.m_cSlotCategory ),
						kInventoryItemInfo.m_sSlotID );

					CX2Item* pItemToAdd = NULL;

					if ( kInventoryItemInfo.m_iItemUID > 0 )
					{
						CX2Item::ItemData kItemDataToAdd( kInventoryItemInfo );
						pItemToAdd = new CX2Item( kItemDataToAdd, pUnit );

						if ( pItemToAdd == NULL )
						{
							ASSERT( false );
							return false;
						} // if
					} // if

					// iventoryItemInfo.m_iItemUID 가 0일때 pItemToAdd는 NULL이 되는데 이 경우엔 장착할 장비가 없는 경우임
					if ( pUser->UpdateEquipViewListInfo( pItemToAdd, pItemToRemove ) == false )
					{
						ASSERT( false );
						return false;
					} // if

					// 장비 제거
					if ( pItemToRemove != NULL )
					{
						// 파티원일 경우 이곳에서 처리(유저 자신일 경우에는 CXGUUser의 UpdateEquip에서 처리)
						g_pX2Game->RemoveSpecialAbilityInEquip( pItemToRemove->GetItemTemplet(), pUser );

						pUnit->RemoveEqip( pItemToRemove->GetUID() );
						kInventory.RemoveItem( pItemToRemove->GetUID() );
					} // if

					if ( pItemToAdd != NULL )
					{
						pUnit->AddEqip( pItemToAdd );
						kInventory.AddItem( static_cast< CX2Inventory::SORT_TYPE >( kInventoryItemInfo.m_cSlotCategory ),
							kInventoryItemInfo.m_sSlotID, pItemToAdd );

						g_pX2Game->SetSpecialAbilityInEquip( pItemToAdd->GetItemTemplet(), pUser );
					} // if
				} // BOOST

				//{{ kimhc // 2010.12.11 // 2010-12-23 New Character CHUNG
				pUser->SetShowViewList( true );
				pUser->UpdateNotRenderingOtherEquipList();
				pUser->UpdateEquipCollisionData();

				pUser->SetEnableAttackBox( L"Rfoot", false );
				pUser->SetEnableAttackBox( L"Lfoot", false );
				pUser->SetEnableAttackBox( L"Lhand2", false );
				//}} kimhc // 2010.12.11 //  2010-12-23 New Character CHUNG

				for( KInventoryItemInfo& kInventoryItemInfo : kEvent.m_vecInventorySlotInfo )
				{
					if ( kInventoryItemInfo.m_cSlotCategory == CX2Inventory::ST_E_EQUIP &&
						( kInventoryItemInfo.m_sSlotID == _CONST_UIINVENTORY_::g_iEquippedNormalWeaponSlotID
						|| kInventoryItemInfo.m_sSlotID == _CONST_UIINVENTORY_::g_iEquippedFashionWeaponslotID ) )
					{
						pUser->UpdateWeaponEnchantEffectAndExtraDamageType();
						break;
					} // if
				} // BOOST

			} // if

			pUser->UpdateAfterImageEquip();
		} break;

	default:
		{
			bool bTest = false;
			bTest = true;
		}
		break;

	} // switch


	pUser->UpdateSocketDataAndEnchantData();
	pUser->UpdatePassiveAndActiveSkillState();

	pUser->ResetMaxMP();
	if( pUser->GetNowMp() > pUser->GetMaxMp() )
	{
		pUser->SetNowMp( pUser->GetMaxMp() );
	}

	return true;
}



bool CX2StateMenu::Hander_EGS_ZERO_ENDURANCE_ITEM_IN_ROOM_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ZERO_ENDURANCE_ITEM_IN_ROOM_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( NULL == g_pX2Game )
	{
		ASSERT( g_pX2Game );
		return false;
	}

	CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( kEvent.m_UnitUID );

	if ( pUser == NULL ||
		pUser->GetUnit() == NULL )
		return false;

	CX2Unit* pUnit = pUser->GetUnit();
	CX2Unit::UnitData* pUnitData = &pUnit->AccessUnitData();


	pUser->SetGameStat( kEvent.m_kGameStat );
	pUnitData->m_GameStat.SetKStat( kEvent.m_kGameStat );

	for (size_t i = 0; i < kEvent.m_vecInventorySlotInfo.size(); i++ )
	{
		CX2Item* pItemToRemoveInView = pUser->GetUnit()->GetInventory().GetItem( kEvent.m_vecInventorySlotInfo[i].m_iItemUID );

		if ( pItemToRemoveInView == NULL ||
			pItemToRemoveInView->GetItemTemplet() == NULL )
		{
			ASSERT( pItemToRemoveInView );
			return false;
		}

		pItemToRemoveInView->AccessItemData().m_Endurance		= 0;

		g_pX2Game->RemoveSpecialAbilityInEquip( pItemToRemoveInView->GetItemTemplet(), pUser );
		//kEvent.m_vecInventorySlotInfo[i].m_iItemUID = 0;

		//if ( pUser->UpdateEquip( &kEvent.m_vecInventorySlotInfo[i] ) == false )
		if ( pUser->UpdateEquipViewListInfo( NULL, pItemToRemoveInView ) == false )
		{
			ASSERT( false );
			return false;
		}


		//{{ kimhc // 2010.12.11 // 2010-12-23 New Character CHUNG
		pUser->SetShowViewList( true );
		pUser->UpdateNotRenderingOtherEquipList();
		pUser->UpdateEquipCollisionData();

		pUser->SetEnableAttackBox( L"Rfoot", false );
		pUser->SetEnableAttackBox( L"Lfoot", false );
		pUser->SetEnableAttackBox( L"Lhand2", false );
		//}} kimhc // 2010.12.11 //  2010-12-23 New Character CHUNG

		// kimhc // SetShowViewList를 수행하기 전에 수행
		if ( pItemToRemoveInView->GetItemTemplet()->GetEqipPosition() == CX2Unit::EP_WEAPON_HAND )
		{
			pUser->UpdateWeaponEnchantEffectAndExtraDamageType();
		}

		if( g_pChatBox != NULL)
		{
			if ( g_pData->GetMyUser()->GetSelectUnit()->GetUID() == kEvent.m_UnitUID )		// 유저 자신이 면
			{


				std::wstringstream strStream;
				strStream <<  GET_REPLACED_STRING( ( STR_ID_2639, "SS", pItemToRemoveInView->GetItemTemplet()->GetNameColor_(),
					pItemToRemoveInView->GetItemTemplet()->GetFullName_() ) );

				wstring wstrColor = L"#CECEC88";							// (236, 236, 136)
				D3DXCOLOR coTextColor(0.92549f, 0.92549f, 0.53333f, 1.f);	// (236, 236, 136)

				g_pChatBox->AddChatLog( strStream.str().c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );
			}
		}

		pItemToRemoveInView = NULL;
	}

	pUser->UpdateSocketDataAndEnchantData();
	pUser->UpdatePassiveAndActiveSkillState();

	return true;
}

/*virtual*/ bool CX2StateMenu::Handler_EGS_CHAR_LEVEL_UP_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHAR_LEVEL_UP_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( NULL == g_pX2Game )
	{
		ASSERT( g_pX2Game );
		return false;
	}

	CX2GUUser*	pUser = g_pX2Game->GetUserUnitByUID( kEvent.m_iUnitUID );

	if ( pUser == NULL ||
		pUser->GetUnit() == NULL )
		return false;

	CX2Unit::UnitData* pUnitData = &pUser->GetUnit()->AccessUnitData();


	if ( pUser->GetNowHp() > 0 )
	{
		// 유닛 데이타 변경
		pUnitData->m_GameStat.SetKStat( kEvent.m_kGameStat );
		pUnitData->m_Stat.SetKStat( kEvent.m_kBaseStat );

		pUnitData->m_Level					= kEvent.m_ucLevel;
		pUnitData->m_NowBaseLevelEXP		= g_pData->GetEXPTable()->GetEXPData( kEvent.m_ucLevel ).m_nTotalExp;
		pUnitData->m_NextBaseLevelEXP		= g_pData->GetEXPTable()->GetEXPData( kEvent.m_ucLevel + 1 ).m_nTotalExp;

		// 유저 변경
		pUser->SetGameStat( kEvent.m_kGameStat );

		pUser->SetMaxHp( (float)kEvent.m_kGameStat.m_iBaseHP );



		pUser->SetNowHp( pUser->GetMaxHp() );
		pUser->SetNowMp( pUser->GetMaxMp() );
		pUser->GetUnit()->SetIsLevelUp( true );

		DisplayLevelUpEffect( pUser );

		//{{ kimhc // 2010.12.24 // 2010-12-23 New Character CHUNG
		if ( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() )
		{
			CX2GageData* pGageData = pUser->GetGageData();
			// 청인 경우 레벨업 시에 캐논볼을 충전 해 준다.
			if ( NULL != pGageData )
			{
				CX2ChungGageData* pChungGageData = static_cast<CX2ChungGageData*>( pGageData );
				pChungGageData->SetNowCannonBallCount( pChungGageData->GetNowCannonBallCount() );
			}
		}
		//}} kimhc // 2010.12.24 //  2010-12-23 New Character CHUNG

		if ( CX2Unit::UT_ARA == pUser->GetUnit()->GetType() )
		{
			CX2GageData* pGageData = pUser->GetGageData();

			// 아라인 경우 레벨업 시에 기력을 충전
			if ( NULL != pGageData )
			{
				CX2AraGageData* pAraGageData = reinterpret_cast<CX2AraGageData*>( pGageData );
				pAraGageData->SetMaxForcePower( min( pUser->GetUnitLevel() / 10, 6 ) + 4 );
				pAraGageData->SetNowForcePower( pAraGageData->GetMaxForcePower() );
			}
		}



		if( g_pChatBox != NULL)
		{
			std::wstringstream strStream;
			if ( g_pData->GetMyUser()->GetSelectUnit()->GetUID() == kEvent.m_iUnitUID )		// 유저 자신이 면
			{
				strStream << GET_REPLACED_STRING( ( STR_ID_2637, "i", kEvent.m_ucLevel ) );

				g_pX2Game->UpdateNpcNameColor( kEvent.m_ucLevel );

				if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
				{	//레벨업 가이드 출력
					g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_LEVEL_UP, true, pUnitData->m_Level);
				}
			}
			else
			{
				strStream << GET_REPLACED_STRING( ( STR_ID_2638, "Si", pUser->GetUnit()->GetNickName(), kEvent.m_ucLevel ) );
			}

			wstring wstrColor = L"#C4EB2D2";								// 78, 178, 210
			D3DXCOLOR coTextColor(0.30588f, 0.69803f, 0.823529f, 1.f);		// 78, 178, 210

			g_pChatBox->AddChatLog( strStream.str().c_str(), KEGS_CHAT_REQ::CPT_TOTAL, coTextColor, wstrColor );
		}
	}

	return true;
}

/*virtual*/ bool CX2StateMenu::Handler_EGS_DECREASE_ENDURANCE_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_DECREASE_ENDURANCE_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( NULL == g_pX2Game )
	{
		ASSERT( g_pX2Game );
		return false;
	}

	CX2GUUser*	pUser = g_pX2Game->GetMyUnit();

	if ( pUser == NULL ||
		pUser->GetUnit() == NULL )
		return false;

	const CX2Inventory& kInventory = pUser->GetUnit()->GetInventory();

	for( const KDecreaseEnduranceInfo& enduranceInfo : kEvent.m_vecEnduranceUpdate )
	{
		CX2Item* pItem = kInventory.GetItem( enduranceInfo.m_iItemUID );
		if ( NULL == pItem )
			continue;

		CX2Item::ItemData& kItemData = pItem->AccessItemData();

		kItemData.m_Endurance = enduranceInfo.m_iEndurance;

		if(g_pInstanceData != NULL && g_pInstanceData->GetMiniMapUI() != NULL)
		{
			g_pInstanceData->GetMiniMapUI()->UpdateEnduranceNotice();
		}
	}

	return true;
}

void CX2StateMenu::DisplayLevelUpEffect( CX2GUUser* pGUUser_ )
{
	if ( NULL != pGUUser_ )
	{
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"LevelUp.ogg", false, false );
		g_pData->GetUIEffectSet()->PlayEffectSet( L"EffectSet_LevelUp_LandPosition", pGUUser_ );
		g_pData->GetUIEffectSet()->PlayEffectSet( L"EffectSet_LevelUp", pGUUser_ );
		g_pData->GetPicCharBlue()->DrawText( L"LEVEL UP", pGUUser_->GetHeadBonePos(), pGUUser_->GetDirVector(), CKTDGPicChar::AT_CENTER );
		pGUUser_->GetUnit()->SetIsLevelUp( false );
	}
}

bool CX2StateMenu::CameraZoomInAndOutKeyProcess()
{
	if( GET_KEY_STATE( GA_ZOOMIN ) == TRUE
		)
	{
		g_pMain->GetGameOption().CameraZoomIn( 1 );
		return true;
	}
	else if( GET_KEY_STATE( GA_ZOOMOUT ) == TRUE
			 )
	{
		g_pMain->GetGameOption().CameraZoomIn( -1 );
		return true;
	}
	else
		return false;
}

void CX2StateMenu::ShowMainMenuButton()
{
	//마우스 동작 있을 때만 메인메뉴 보여주도록 수정
	if( false == m_bShowMainMenuButton )
	{
		m_bShowMainMenuButton = true;
		m_fMenuMaxRemainTime = 6.0f;
	}
}
void CX2StateMenu::UpdateMenuString()
{
	if( NULL == m_pDLGMenu )
		return;

	CKTDGUIStatic* pStaticMenu = static_cast<CKTDGUIStatic*>(m_pDLGMenu->GetControl(L"Static"));
	pStaticMenu->GetString(0)->msg = GET_STRING(STR_ID_19974);
	pStaticMenu->GetString(1)->msg = GET_STRING(STR_ID_19975);

	//메뉴 버튼
	wstring strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19976, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_INFO ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Charac")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19977, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_INVENTORY ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Inven")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19978, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_SKILL_TREE ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Skill")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19979, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_QUEST ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Quest")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19980, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_PET ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Pet")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19981, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_GUILD ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Guild")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19982, "LL", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_COMMUNITY ), g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_FRIEND ) ));
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Comm")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_19983, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_OPTION ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"System")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_STRING(STR_ID_19984);
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Cash")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_STRING(STR_ID_19985);
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Guide")), strButtonDesc.c_str() );

	strButtonDesc.clear();
	strButtonDesc = GET_REPLACED_STRING( ( STR_ID_24618, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_WARP ) ) );
	SetGuideString( static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Warp")), strButtonDesc.c_str() );
}
void CX2StateMenu::UpdateMenuPosistion()
{
	if( NULL == m_pDLGMenu )
		return;
	//마우스 동작 없을 때 메인메뉴 사라지도록 하기 위해 객체 저장

	//메뉴 버튼
	CKTDGUIButton* pButtonChar = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Charac"));	//캐릭터
	CKTDGUIButton* pButtonInven = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Inven"));	//인벤토리
	CKTDGUIButton* pButtonSkill = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Skill"));	//스킬
	CKTDGUIButton* pButtonQuest = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Quest"));	//퀘스트
	CKTDGUIButton* pButtonPet = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Pet"));		//펫
	CKTDGUIButton* pButtonGuild = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Guild"));	//길드
	CKTDGUIButton* pButtonComm = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Comm"));		//커뮤니티
	CKTDGUIButton* pButtonSystem = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"System"));	//시스템
	CKTDGUIButton* pButtonCash = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Cash"));		//캐쉬샵
	CKTDGUIButton* pButtonGuide = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Guide"));		//가이드
	CKTDGUIButton* pButtonWarp = static_cast<CKTDGUIButton*>(m_pDLGMenu->GetControl(L"Warp"));		//이동버튼


	if( true == m_vecpButtonMainMenu.empty() )
	{
		m_vecpButtonMainMenu.push_back(pButtonChar);
		m_vecpButtonMainMenu.push_back(pButtonInven);
		m_vecpButtonMainMenu.push_back(pButtonSkill);
		m_vecpButtonMainMenu.push_back(pButtonQuest);
		m_vecpButtonMainMenu.push_back(pButtonPet);
		m_vecpButtonMainMenu.push_back(pButtonGuild);
		m_vecpButtonMainMenu.push_back(pButtonComm);
		m_vecpButtonMainMenu.push_back(pButtonSystem);
		m_vecpButtonMainMenu.push_back(pButtonCash);
		m_vecpButtonMainMenu.push_back(pButtonGuide);
		m_vecpButtonMainMenu.push_back(pButtonWarp);
	}

	//메뉴 버튼 new
	CKTDGUIStatic* pButtonPetEvolNew = (CKTDGUIStatic*)m_pDLGMenu->GetControl(L"Pat_Inventory_Evolution"); //펫진화
	CKTDGUIStatic* pButtonPetNew = (CKTDGUIStatic*)m_pDLGMenu->GetControl(L"Pat_Inventory_New"); //펫소환
	CKTDGUIStatic* pButtonInvenNew = (CKTDGUIStatic*)m_pDLGMenu->GetControl(L"Inventory_New"); //인벤
	CKTDGUIStatic* pButtonSkillNew = (CKTDGUIStatic*)m_pDLGMenu->GetControl(L"Skill_New"); //스킬
	CKTDGUIStatic* pButtonWarpVIP = (CKTDGUIStatic*)m_pDLGMenu->GetControl(L"Warp_VIP"); //스킬
	//스킬, 펫, 길드 버튼을 사용 할 수 있을 때만 출력 해 주기.
	if( NULL != g_pData )
	{
		//초기화
		D3DXVECTOR2 offset(0,0);
		//메인메뉴
		pButtonGuild->SetShowEnable( true, true );
		pButtonPet->SetShowEnable( true, true );
		pButtonSkill->SetShowEnable( true, true );
		pButtonWarp->SetShowEnable( true, true );
		pButtonChar->SetOffsetPos( offset );
		pButtonInven->SetOffsetPos( offset );
		pButtonSkill->SetOffsetPos( offset );
		pButtonQuest->SetOffsetPos( offset );
		pButtonPet->SetOffsetPos( offset );
		pButtonGuide->SetOffsetPos(offset);
		pButtonWarp->SetOffsetPos( offset );
		//메인메뉴 new
		pButtonPetEvolNew->SetOffsetPos( offset );
		pButtonPetNew->SetOffsetPos( offset );
		pButtonInvenNew->SetOffsetPos( offset );
		pButtonSkillNew->SetOffsetPos( offset );
		pButtonWarpVIP->SetOffsetPos( offset );
	//알림말풍선
		g_pMain->GetInformerManager()->GetInvenInformer()->SetTalkBoxOffsetPos(offset);
		g_pMain->GetInformerManager()->GetGuildInformer()->SetTalkBoxOffsetPos(offset);
		g_pMain->GetInformerManager()->GetSkillInformer()->SetTalkBoxOffsetPos(offset);
		g_pMain->GetInformerManager()->GetQuestInformer()->SetTalkBoxOffsetPos(offset);



		//길드가 없다면
		if( NULL != g_pData->GetGuildManager() && false == g_pData->GetGuildManager()->DidJoinGuild() )
		{
			offset.x += 30;

			pButtonGuild->SetShowEnable(false, false);

			//메인메뉴
			pButtonGuide->SetOffsetPos(offset);
			pButtonChar->SetOffsetPos( offset );
			pButtonInven->SetOffsetPos( offset );
			pButtonPet->SetOffsetPos( offset );
			pButtonSkill->SetOffsetPos( offset );
			pButtonQuest->SetOffsetPos( offset );
			pButtonWarp->SetOffsetPos( offset );
			//가이드 오프셋
			pButtonGuide->SetGuideDescOffsetPos(offset);
			pButtonChar->SetGuideDescOffsetPos( offset );
			pButtonInven->SetGuideDescOffsetPos( offset );
			pButtonPet->SetGuideDescOffsetPos( offset );
			pButtonSkill->SetGuideDescOffsetPos( offset );
			pButtonQuest->SetGuideDescOffsetPos( offset );
			pButtonWarp->SetGuideDescOffsetPos( offset );
			//메인메뉴 new
			pButtonPetEvolNew->SetOffsetPos( offset );
			pButtonPetNew->SetOffsetPos( offset );
			pButtonInvenNew->SetOffsetPos( offset );
			pButtonSkillNew->SetOffsetPos( offset );
			pButtonWarpVIP->SetOffsetPos( offset );
			//알림말풍선
			g_pMain->GetInformerManager()->GetInvenInformer()->SetTalkBoxOffsetPos(offset);
			g_pMain->GetInformerManager()->GetGuildInformer()->SetTalkBoxOffsetPos(offset);
			g_pMain->GetInformerManager()->GetSkillInformer()->SetTalkBoxOffsetPos(offset);
		}

		//펫 리스트에 펫이 하나도 없거나, 소환중인 펫이 없으면
		if( (NULL != g_pData->GetUIManager() &&	NULL != g_pData->GetUIManager()->GetUIPetList() &&	0 == g_pData->GetUIManager()->GetUIPetList()->GetPetListSize() )
			&& ( NULL != g_pData->GetPetManager() && NULL == g_pData->GetPetManager()->GetMyPet() )
			&& ( NULL != CX2RidingPetManager::GetInstance() && false == CX2RidingPetManager::GetInstance()->GetHasRidingPet() )
			)
		{
			offset.x += 30;
			pButtonPet->SetShowEnable(false, false);

			//메인메뉴
			pButtonGuide->SetOffsetPos(offset);
			pButtonChar->SetOffsetPos( offset );
			pButtonInven->SetOffsetPos( offset );
			pButtonWarp->SetOffsetPos( offset );
			//가이드 오프셋
			pButtonGuide->SetGuideDescOffsetPos(offset);
			pButtonChar->SetGuideDescOffsetPos( offset );
			pButtonInven->SetGuideDescOffsetPos( offset );
			pButtonPet->SetGuideDescOffsetPos( offset );
			pButtonWarp->SetGuideDescOffsetPos( offset );
			//메인메뉴 new
			pButtonInvenNew->SetOffsetPos( offset );
			pButtonSkillNew->SetOffsetPos( offset );
			pButtonWarpVIP->SetOffsetPos( offset );
			//알림말풍선
			g_pMain->GetInformerManager()->GetInvenInformer()->SetTalkBoxOffsetPos(offset);
			g_pMain->GetInformerManager()->GetGuildInformer()->SetTalkBoxOffsetPos(offset);
			g_pMain->GetInformerManager()->GetSkillInformer()->SetTalkBoxOffsetPos(offset);
			g_pMain->GetInformerManager()->GetQuestInformer()->SetTalkBoxOffsetPos(offset);
		}

		//스킬 포인트가 없다면
		if( NULL != g_pData->GetMyUser() &&
			NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{

			int iUsedCP, iUsedSP;
			g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.CalcUsedSPointAndCSPoint( iUsedSP, iUsedCP );
			int iTotalUsedSP = iUsedCP + iUsedSP;

			int iNewSP = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSPoint + g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iCSPoint;
			if( iNewSP == 0 && iTotalUsedSP == 2 )
			{
				offset.x += 30;
				pButtonSkill->SetShowEnable(false, false);

				pButtonChar->SetOffsetPos( offset );
				pButtonGuide->SetOffsetPos(offset);
				pButtonInven->SetOffsetPos( offset );
				pButtonPet->SetOffsetPos( offset );
				pButtonWarp->SetOffsetPos( offset );
				//가이드 오프셋
				pButtonGuide->SetGuideDescOffsetPos(offset);
				pButtonChar->SetGuideDescOffsetPos( offset );
				pButtonInven->SetGuideDescOffsetPos( offset );
				pButtonPet->SetGuideDescOffsetPos( offset );
				pButtonWarp->SetGuideDescOffsetPos( offset );
				//메인메뉴 new
				pButtonPetEvolNew->SetOffsetPos( offset );
				pButtonPetNew->SetOffsetPos( offset );
				pButtonInvenNew->SetOffsetPos( offset );
				pButtonWarpVIP->SetOffsetPos( offset );
				//알림말풍선
				g_pMain->GetInformerManager()->GetInvenInformer()->SetTalkBoxOffsetPos(offset);
			}
		}
	}

}

/*virtual*/ bool CX2StateMenu::ProcessStateChangeUnitSelectWithOptionWindow()
{

	if( NULL != CX2RidingPetManager::GetInstance() )
		CX2RidingPetManager::GetInstance()->Handler_EGS_UNSUMMON_RIDING_PET_REQ();

	ToggleSystemMenu(false);
	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_PVP_LOBBY:
	case CX2Main::XS_PVP_ROOM:
	case CX2Main::XS_DUNGEON_ROOM:
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_SQUARE_GAME:
	case CX2Main::XS_TRAINING_SCHOOL:
	case CX2Main::XS_BATTLE_FIELD:
	case CX2Main::XS_WEDDING_GAME:
		{
			g_pData->GetUIManager()->CloseAll();

			//{{ kimhc // 2010.9.29 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트
			//}} kimhc // 2010.9.29 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트

			//{{ kimhc // 2009-10-12 // 서버 선택시 길드 정보 삭제
			g_pData->GetGuildManager()->SafeDeleteGuildInfo();
			//}} kimhc // 2009-10-12 // 서버 선택시 길드 정보 삭제

			g_pData->ResetGlobalMissionManager();

			CX2StateAutoChanger::TARGET_DETAIL targetDetail;
			g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), (int)CX2Main::XS_SERVER_SELECT, targetDetail );

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			KOGGamePerformanceCheck::GetInstance()->End();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		} break;
	default:
		break;
	}

	return true;
}

/*virtual*/ bool CX2StateMenu::ProcessStateChangeServerSelectWithOptionWindow()
{
	if( NULL != CX2RidingPetManager::GetInstance() )
		CX2RidingPetManager::GetInstance()->Handler_EGS_UNSUMMON_RIDING_PET_REQ();

	ToggleSystemMenu(false);

	//{{ kimhc // 2010.9.29 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트
	//}} kimhc // 2010.9.29 // 홈페이지 개편에서 아바타 이미지 표현으로 사용될 캐릭터 이미지 캡쳐 기능 테스트

	g_pData->GetUIManager()->CloseAll();

	//{{ kimhc // 2009-10-12 // 서버 선택시 길드 정보 삭제
	g_pData->GetGuildManager()->SafeDeleteGuildInfo();
	//}} kimhc // 2009-10-12 // 서버 선택시 길드 정보 삭제

	CX2StateServerSelect::m_sbSelectedServerSet = false;
	CX2StateAutoChanger::TARGET_DETAIL targetDetail;
	g_pMain->GetStateAutoChanger().StartStateChange( (int)g_pMain->GetNowStateID(), (int)
		CX2Main::XS_SERVER_SELECT, targetDetail );

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->End();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	return true;
	//return Handler_EGS_STATE_CHANGE_SERVER_SELECT_REQ();
}


void CX2StateMenu::UpdateGuideDescBuffTemplet( CX2PremiumBuffIconPtr pPremiumBuffPtr )
{
	CKTDGUIDialogType BuffIconDialogType	= pPremiumBuffPtr->GetBuffIconDialogType();

	CKTDGUIButton*	  pButton				= static_cast<CKTDGUIButton*>( BuffIconDialogType->GetControl( L"Button_Buff_Icon" ) );	//임시 설정

	if( pButton->IsOnMouseOver() == true )
	{
		wstringstream						wstreamDesc;
		wstring								strBufftempletDesc	= L"";
		wstring								wstrEndTime			= pPremiumBuffPtr->GetBuffEndTime();											//버프 종료 시간
		CX2PremiumBuffTemplet::BUFF_ID		ePremiumBuffID		= pPremiumBuffPtr->GetPremiumBuffID();											//버프 ID

		CX2PremiumBuffTempletPtr			ptrTemplet			= g_pData->GetPremiumBuffManager()->GetPremiumBuffTempletPtr( ePremiumBuffID );	//버프 템플릿
		UINT								uiBuffName			= ptrTemplet->GetStringID();													//버프 이름 String ID
		int									BuffPeriodSecond	= static_cast<int>( ptrTemplet->GetBuffPeriodSecond() / 60 );					//버프 유지 시간
		int									iBuffTimeAllSec		= ConvertTimeForBuffTemplet( wstrEndTime );										//남은 버프 유지 시간 ( 실제 초 )
		int									iBuffTimeMin		= ConvertTimeForBuffTemplet( wstrEndTime ) / 60;								//남은 버프 유지 시간 ( 분 )
		int									iBuffTimeSec		= ConvertTimeForBuffTemplet( wstrEndTime ) % 60;								//남은 버프 유지 시간 ( 초 )
		bool								bShowEndTime		= true;																			//남은 버프 시간 표시 여부

		wstreamDesc << GET_STRING( uiBuffName ) << '\n';		/// 버프 이름

		switch( ePremiumBuffID )
		{
		case CX2PremiumBuffTemplet::BI_PROTECTION_OF_NUT:			// 누트의 가호   ( 비약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_16564 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16601, "i", 100 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17099 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_PROTECTION_OF_PUTA:			// 프타의 가호   ( 비약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17130 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16602, "i",25 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17099 );

				strBufftempletDesc += wstreamDesc.str().c_str();

			}break;

		case CX2PremiumBuffTemplet::BI_PROTECTION_OF_LAHELL:		// 라헬의 가호   ( 비약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17131 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16603, "i", 20 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17099 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_PROTECTION_OF_JAKIELL:		// 자키엘의 가호 ( 비약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17132 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16604, "f", 1.5 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17099 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_BELSSING_OF_SERAPHIM:		// 세라핌의 축복	 ( 영약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_16565 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16605, "ii", 100 , 2 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17100 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_BELSSING_OF_GEV:				// 게브의 축복	 ( 영약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17133 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16606, "i", 5 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17100 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_BELSSING_OF_AMON:			// 아몬의 축복	 ( 영약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17134 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16607, "i", 5 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17100 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_BELSSING_OF_CRONOS:			// 크로노스의 축복 ( 영약 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17135 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_16608, "ii",50, 2 ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_17100 );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_1:	// 어둠의 문 개인 보상 버프 Level 1
		case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_2:	// 어둠의 문 개인 보상 버프 Level 2
		case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_3:	// 어둠의 문 개인 보상 버프 Level 3
		case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_4:	// 어둠의 문 개인 보상 버프 Level 4
			{
				//위에서 처리한 문자들 저장
				strBufftempletDesc += wstreamDesc.str().c_str();

				//개인 보상 버프 레벨 표시
				wstringstream wstreamBuffLevelDesc;

				int iBuffLevel = 0;

				switch( ePremiumBuffID )
				{
				case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_1:	// 어둠의 문 개인 보상 버프 Level 1
					{
						iBuffLevel = 1;
					}break;
				case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_2:	// 어둠의 문 개인 보상 버프 Level 2
					{
						iBuffLevel = 2;
					}break;
				case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_3:	// 어둠의 문 개인 보상 버프 Level 3
					{
						iBuffLevel = 3;
					}break;
				case CX2PremiumBuffTemplet::BI_SAVE_CRYSTAL_BUFF_LEVEL_4:	// 어둠의 문 개인 보상 버프 Level 4
					{
						iBuffLevel = 4;
					}break;
				default:
					{
						ASSERT( "Save Crystal Buff is Nothing!" );
					}break;
				}
				wstreamBuffLevelDesc << GET_REPLACED_STRING( ( STR_ID_16598, "i", iBuffLevel ) ) << '\n' << '\n'
				<< GET_STRING( STR_ID_16599 ) << '\n' << '\n';

				strBufftempletDesc += wstreamBuffLevelDesc.str().c_str();

				//소켓 정보를 가져와서 출력
				vector<int> vSocketOptionList = ptrTemplet->GetSocketOptionsList();
				CX2Unit*	pkUnit			  = g_pData->GetMyUser()->GetSelectUnit();

				for ( int i = 0; i < (int)vSocketOptionList.size(); i++ )
				{
					const CX2SocketItem::SocketData* pSocketData = g_pData->GetSocketItem()->GetSocketData( vSocketOptionList[i] );

					if( pSocketData != NULL )
					{
						strBufftempletDesc += L"#CFFFF00";

						//소켓 정보 출력
						strBufftempletDesc += pSocketData->GetSocketDesc( pkUnit->GetUnitData().m_Level );

						strBufftempletDesc += L"#CX";

						strBufftempletDesc += L"\n";
					}

				}

				//개인 보상 버프 안내 표시
				wstringstream wstreamSaveBuffDesc;

				wstreamSaveBuffDesc << '\n' << GET_STRING( STR_ID_16600 );

				strBufftempletDesc += wstreamSaveBuffDesc.str().c_str();

				//어둠의 문 개인 보상 버프는 남은 버프 시간을 표시하지 않음
				//bShowEndTime = false;
			}break;

		case CX2PremiumBuffTemplet::BI_STRENGTHENING_BODY:	// 강화된 신체 ( 비약 사용 제한 디버프 )
			{
				wstreamDesc << '\n' << GET_STRING( STR_ID_17097 ) << '\n' << '\n'
				<< GET_REPLACED_STRING( ( STR_ID_17098, "i", 5 ) );

				strBufftempletDesc += wstreamDesc.str().c_str();
			}break;

		default:
			{
			}break;
		}

		//유지 시간 및 남은 시간 표시
		if( bShowEndTime == true )
		{
			/*
			wstringstream wstreamTimeDesc;

			wstreamTimeDesc << '\n' << GET_REPLACED_STRING( ( STR_ID_16566, "i", iBuffTimeMin ) ) << L":";

			//한자리 숫자일 때, 앞에 0을 붙임 ( 예 :7 -> 07 )
			if( iBuffTimeSec < 10 )
				wstreamTimeDesc << L"0" << iBuffTimeSec << '\n';
			else
				wstreamTimeDesc << iBuffTimeSec << '\n';

			//위에서 처리한 문자들 저장
			strBufftempletDesc += wstreamTimeDesc.str().c_str();
			*/
			wstringstream wstreamTimeDesc;

			AddStringRemainTime( wstreamTimeDesc, iBuffTimeAllSec );

			strBufftempletDesc += wstreamTimeDesc.str().c_str();
		}

		pButton->SetGuideDescAbsolutePos( D3DXVECTOR2( 15.f, 685.f) );
		pButton->SetGuideDesc( strBufftempletDesc.c_str() );

		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( BuffIconDialogType );
	}
}

int CX2StateMenu::ConvertTimeForBuffTemplet( wstring wstrEndTime )
{
	CTime cEndTime;

	//std::wstring tempString = kEvent.m_wstrElixirUseTime.substr(0, 19);
	KncUtil::ConvertStringToCTime( wstrEndTime, cEndTime );
	if( wstrEndTime.length() > 0 )
	{
		CTime cCurrentTime = g_pData->GetServerCurrentTime();	//현재 시간

		CTimeSpan cBuffTime = cEndTime - cCurrentTime;


		if( cBuffTime.GetTotalSeconds() > 0 )					//효과 시간이 남아있을 때
			return static_cast<int>(cBuffTime.GetTotalSeconds());
	}
	return 0;
}

//버프 표시 여부
void CX2StateMenu::SetShowBuffTempletIcon( bool  bVal)
{
	if( g_pData->GetPremiumBuffManager()->GetPremiumBuffList().empty() != true )
	{
		for( CX2PremiumBuffIconPtr pPremiumBuffPtr : g_pData->GetPremiumBuffManager()->GetPremiumBuffList() )
		{
			CX2PremiumBuffTemplet::BUFF_ID		ePremiumBuffID		= pPremiumBuffPtr->GetPremiumBuffID();													//버프 ID

			//버프 아이콘이 있는 버프일 때,
			if( pPremiumBuffPtr->GetUsingIcon() == true )
			{
				CX2PremiumBuffTempletPtr		ptrTemplet			= g_pData->GetPremiumBuffManager()->GetPremiumBuffTempletPtr( ePremiumBuffID );			//버프 템플릿

				CKTDGUIDialogType				BuffIconDialogType	= pPremiumBuffPtr->GetBuffIconDialogType();

				CKTDGUIButton*					pButton				= static_cast<CKTDGUIButton*>( BuffIconDialogType->GetControl( L"Button_Buff_Icon" ) );

				pButton->SetShow( bVal );
			}
		}
	}
	g_pData->GetPremiumBuffManager()->SetShowBuffTempletUI( bVal );
}







bool CX2StateMenu::Handler_EGS_GET_ADVERTISEMENT_EVENT_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_GET_ADVERTISEMENT_EVENT_REQ );
	g_pMain->AddServerPacket( EGS_GET_ADVERTISEMENT_EVENT_ACK );

	return true;
}

bool CX2StateMenu::Handler_EGS_GET_ADVERTISEMENT_EVENT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_GET_ADVERTISEMENT_EVENT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_GET_ADVERTISEMENT_EVENT_ACK ) == true )
	{
		if( kEvent.m_vecAdvertisementURL.size() > 0 )
		{
			if( false == g_pMain->GetBrowserWrapper()->IsClosed() )
			{
				g_pMain->GetBrowserWrapper()->CloseBrowser();
			}

			RECT rcWebPage;
			GetWindowRect( g_pKTDXApp->GetHWND(), &rcWebPage );

			POINT ptCenter;
			ptCenter.x = rcWebPage.left + ( rcWebPage.right - rcWebPage.left ) / 2;
			ptCenter.y = rcWebPage.top + ( rcWebPage.bottom - rcWebPage.top ) / 2;

			rcWebPage.left = ptCenter.x - 415;
			rcWebPage.right = ptCenter.x + 415;
			rcWebPage.top = ptCenter.y - 235;
			rcWebPage.bottom = ptCenter.y + 235;

			g_pMain->GetBrowserWrapper()->Create( rcWebPage, g_pKTDXApp->GetHWND() );

			std::string strURL;
			ConvertWCHARToChar( strURL, kEvent.m_vecAdvertisementURL[0] );
			g_pMain->GetBrowserWrapper()->Navigate( strURL.c_str(), NULL );
		}
	}

	return false;
}

void CX2StateMenu::ClickWarpButton( bool bIsEdConsumption_ )
{
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_SQUARE_GAME:
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_BATTLE_FIELD:
		{
			m_bIsEdConsumptionWarp = bIsEdConsumption_;
			CreateWarpDest();
		} break;

	default:
		{
			// 마을/거래광장/배틀필드 이외에서는 사용안됨...
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4495 ),g_pMain->GetNowState() );
		} break;
	}	/// switch
}

void CX2StateMenu::UseWarpPopup(bool bIsEdConsumption_)
{
	if( NULL != g_pData && NULL != g_pData->GetLocationManager() &&
		NULL != g_pData->GetLocationManager()->GetVillageMapTemplet(static_cast<SEnum::VILLAGE_MAP_ID>(m_iSelectedWarpDest)) )
	{
		wstring strVillageName = g_pData->GetLocationManager()->GetVillageMapTemplet(static_cast<SEnum::VILLAGE_MAP_ID>(m_iSelectedWarpDest))->m_Name;

		if( true == strVillageName.empty() )
			return;

		if( true == bIsEdConsumption_ )
		{
			int iWarpDistance = abs(m_iSelectedWarpDestIndex - m_iCurrentVillageWarpIndex);
			int iBasePrice = 6000;
			float fDecreaseFactor = 0.6f;
			if( NULL !=  g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				float fLevel = static_cast<float>(g_pData->GetSelectUnitLevel());
				if( fLevel > 0 )
				{
					int iWarpCommission = static_cast<int>((fLevel/60) * (iBasePrice + static_cast<int>(iBasePrice * fDecreaseFactor) * (iWarpDistance-1) )); //이동 수수료

					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
						GET_REPLACED_STRING((STR_ID_20266, "iL", iWarpCommission, strVillageName)),
						SMUCM_WARP_DEST_OK, g_pMain->GetNowState() );
				}
			}
		}
		else
		{
			g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
				GET_REPLACED_STRING((STR_ID_20297, "L", strVillageName)),
				SMUCM_WARP_DEST_OK, g_pMain->GetNowState() );
		}
	}
}

void CX2StateMenu::AddWarpList(CKTDGUIContolList *pControlList, const int row, const int iVillageId)
{
	if( pControlList == NULL )
		return;

	int iCurrentVillageID = -1;
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_SQUARE_GAME:
		{
			iCurrentVillageID = static_cast<int>(g_pData->GetLocationManager()->GetCurrentVillageID());
		} break;
	case CX2Main::XS_BATTLE_FIELD:
		{
			iCurrentVillageID = g_pData->GetBattleFieldManager().GetReturnVillageId();
		} break;
	default:
		break;
	}

	if( iVillageId == iCurrentVillageID )
	{
		m_iCurrentVillageWarpIndex = row;
	}

	WCHAR wVillageId[256] = {0,};
	//wsprintf(wVillageId, L"Village_%d", iVillageId);
	StringCchPrintf(wVillageId, 256, L"Village_%d", iVillageId);
	// 마을리스트 추가
	CKTDGUIStatic *pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(wVillageId);
	pControl->SetShow(true);
	pControlList->InsertItem( pControl, row, 0, false );

	if( m_pDLGWarpDestination != NULL )
	{
		m_pDLGWarpDestination->OpenScriptFile(L"DLG_Destination_Control_byButton.lua");

		CKTDGUIButton *pButton = (CKTDGUIButton*)m_pDLGWarpDestination->GetControl( L"ListControl_Button" );
		//wsprintf(wVillageId, L"Button_%d", iVillageId);
		StringCchPrintf(wVillageId, 256, L"Button_%d", iVillageId);
		pButton->SetName( wVillageId );
		pButton->AddDummyInt(iVillageId);
		pControlList->InsertItem( pButton, row, 1 );
		pButton->SetShowEnable(true, true);

		pButton->SetDownStateAtNormal(false);

		m_vecWarpListButton.push_back( pButton );
	}
}

void CX2StateMenu::InitWarpList()
{
	CKTDGUIStatic *pControl = NULL;
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20000");	//VMI_RUBEN
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20001");	//VMI_ELDER
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1000");	//VMI_BATTLE_FIELD_ELDER_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20002");	//VMI_BESMA
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1001");	//VMI_BATTLE_FIELD_BESMA_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20003");	//VMI_ALTERA
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1002");	//VMI_BATTLE_FIELD_ALTERA_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20005");	//VMI_PEITA
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20004");	//VMI_VELDER
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1004");	//VMI_BATTLE_FIELD_VELDER_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20006");	//VMI_HAMEL
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1005");	//VMI_BATTLE_FIELD_HAMEL_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20007");	//VMI_SANDER
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1006");	//VMI_BATTLE_FIELD_SANDER_REST_00
	pControl->SetShow(false);
}

void CX2StateMenu::CreateWarpDest()
{
	m_vecWarpListButton.clear();
	SAFE_DELETE_DIALOG( m_pDLGWarpDestination );

	if( m_pDLGWarpDestination == NULL )
	{
		m_pDLGWarpDestination = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Destination_byButton.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGWarpDestination );
	}

	if( m_pDLGWarpDestination != NULL )
	{
		m_pDLGWarpDestination->SetPos( g_pMain->GetWindowPos( D3DXVECTOR2(300, 400), D3DXVECTOR2(75,20)) );
		m_pDLGWarpDestination->SetShowEnable(true, true);

		CKTDGUIContolList *pControlList = (CKTDGUIContolList*)m_pDLGWarpDestination->GetControl(L"WarpDestControlList");
		if( pControlList != NULL )
		{
			CKTDGUIStatic *pControl = NULL;
			CX2LocationManager::LocalMapTemplet* pLocalMapTemplet = NULL;

			int row = 0;
			InitWarpList();
			// 루벤지역
			{
				// 루벤마을
				pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20000");	//VMI_RUBEN
				pControl->SetShow(false);
				AddWarpList( pControlList, row, SEnum::VMI_RUBEN );
				++row;
			}

			// 엘더지역
			const int iMyUnitLevel = g_pData->GetSelectUnitLevel();
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_ELDER );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 엘더마을
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20001");	//VMI_ELDER
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_ELDER );
					++row;

					// 엘더 휴식처(월리성 관문)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1000");	//VMI_BATTLE_FIELD_ELDER_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_ELDER_REST_00 );
					++row;
				}
			}

			// 베스마지역
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_BESMA );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 베스마마을
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20002");	//VMI_BESMA
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BESMA );
					++row;

					// 베스마 휴식처(아슬아슬 하늘길)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1001");	//VMI_BATTLE_FIELD_BESMA_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_BESMA_REST_00 );
					++row;
				}
			}

			// 알테라지역
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_ALTERA_ISLAND );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 알테라마을
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20003");	//VMI_ALTERA
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_ALTERA );
					++row;

					// 알테라 휴식처(풍고족 은신처)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1002");	//VMI_BATTLE_FIELD_ALTERA_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00 );
					++row;
				}
			}


			// 페이타지역
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_PEITA );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 페이타(파견 텐트)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20005");	//VMI_PEITA
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_PEITA );
					++row;
				}
			}


			// 벨더지역
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_VELDER );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 벨더마을
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20004");	//VMI_VELDER
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_VELDER );
					++row;

					// 벨더 휴식처 (시계탑 광장)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1004");	//VMI_BATTLE_FIELD_VELDER_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_VELDER_REST_00 );
					++row;
				}
			}

			//{{ JHKang / 강정훈 / 2011/01/11 / 하멜 추가
			// 하멜지역
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_HAMEL );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 수도 하멜
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20006");	//VMI_HAMEL
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_HAMEL );
					++row;

					// 하멜 휴식처(치유의 샘터)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1005");	//VMI_BATTLE_FIELD_HAMEL_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00 );
					++row;
				}
			}
			//}} JHKang / 강정훈 / 2011/01/11 / 하멜 추가


			// 샌더지역
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_SANDER );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// 샌더 마을
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20007");	//VMI_HAMEL
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_SANDER );
					++row;

					// 샌더 휴식처(윈드스톤 유적지)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1006");	//VMI_BATTLE_FIELD_HAMEL_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_SANDER_REST_00 );
					++row;
				}
			}
			pControlList->SetIndex(0, 0);
		}
	}
}

void CX2StateMenu::SetShowWarpDest(bool val)
{
	if( m_pDLGWarpDestination != NULL )
		m_pDLGWarpDestination->SetShowEnable(val, val);
}

bool CX2StateMenu::GetShowWarpDest()
{
	if( m_pDLGWarpDestination != NULL )
		return m_pDLGWarpDestination->GetShow();

	return false;
}

bool CX2StateMenu::Handler_EGS_WARP_BY_BUTTON_REQ( int iWarpDest_ )
{
	KEGS_WARP_BY_BUTTON_REQ kPacket;
	kPacket.m_iCurrentVillageWarpIndex = iWarpDest_;
	kPacket.m_bFreeWarp = !(m_bIsEdConsumptionWarp);// ! 에 주의

	g_pData->GetServerProtocol()->SendPacket( EGS_WARP_BY_BUTTON_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_WARP_BY_BUTTON_ACK );

	return true;
}

bool CX2StateMenu::Handler_EGS_WARP_BY_BUTTON_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_WARP_BY_BUTTON_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_WARP_BY_BUTTON_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED = kEvent.m_iED;

			if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
			{
				g_pData->GetUIManager()->GetUICharInfo()->ResetEDnCashnVP();
			}

			// 마을간 워프 이동 검사
			if( kEvent.m_iWarpPointMapID > 0 && g_pData->GetLocationManager()->IsValidWarpField(kEvent.m_iWarpPointMapID) )
			{
				m_iCurrentVillageWarpIndex = -1;

				if(g_pMain->GetNowStateID() == CX2Main::XS_SQUARE_GAME)
				{
					// 거래광장에서 워프
					g_pData->SetStateArg(kEvent.m_iWarpPointMapID);
					CX2StateSquareGame* pState = (CX2StateSquareGame*) g_pMain->GetNowState();
					pState->LeaveSquareGame();
				}
				else
				{
					// 이동티켓을 이용하여 마을 이동
					g_pData->SetStateArg(kEvent.m_iWarpPointMapID);
					CX2State* pState = (CX2State*) g_pMain->GetNowState();
					pState->Handler_EGS_STATE_CHANGE_FIELD_REQ(kEvent.m_iWarpPointMapID);
				}
			}

			return true;
		}
	}

	return false;
}


bool CX2StateMenu::Handler_EGS_UPDATE_WARP_VIP_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UPDATE_WARP_VIP_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		if( g_pData->GetMyUser() != NULL &&
			g_pData->GetMyUser()->GetSelectUnit() != NULL )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_trWarpVipEndDate = kEvent.m_trWarpVipEndDate;
			m_fTimeCheckWarpVip = 0.f;
			return true;
		}
	}

	return false;
}


wstring CX2StateMenu::GetExpirationDateDesc( __int64 trTime_ )		// 주의!! KTDXCommonFunc.h에 정의되어 있는것과 같은 이름이나 쓰임이 다름
{
	wstring tempDesc = L"";

	tempDesc = GET_STRING( STR_ID_309 );

	CTime cTime = trTime_;

	wstringstream wstrstm;

	wstrstm.str( L"" );

	__time64_t t0 = g_pData->GetServerCurrentTime();

	double fDiffTime = difftime( KncUtil::TimeToInt( cTime ), (time_t)t0 );

	int remainDay =  (int)(fDiffTime / ( 60 * 60 * 24 ) );
	if ( remainDay > 0 )
		wstrstm << remainDay << GET_STRING( STR_ID_14 ) << L" ";

	int remainHour = ((time_t)fDiffTime % ( 60 * 60 * 24 )) / (60 * 60);
	if ( remainHour < 0 )
		remainHour = 0;

	wstrstm << remainHour << GET_STRING( STR_ID_310 ) << L" ";

	int remainMinute = ((time_t)fDiffTime % ( 60 * 60 )) / (60 );

	if ( remainDay <= 0 )
	{
		if ( remainMinute <= 0 )
			remainMinute = 0;
		wstrstm << remainMinute << GET_STRING( STR_ID_311 ) << L" ";
	}

	tempDesc += wstrstm.str().c_str();

	tempDesc += L"\n";

	return tempDesc;
}





bool CX2StateMenu::Handler_EGS_2013_EVENT_MISSION_COMPLETE_REQ( IN bool bLevelUpEvent )
{
	KEGS_2013_EVENT_MISSION_COMPLETE_REQ kPacket;
	kPacket.m_bLevelUpEvent = bLevelUpEvent;

	g_pData->GetServerProtocol()->SendPacket( EGS_2013_EVENT_MISSION_COMPLETE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_2013_EVENT_MISSION_COMPLETE_ACK );

	return true;
}
bool CX2StateMenu::Handler_EGS_2013_EVENT_MISSION_COMPLETE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_2013_EVENT_MISSION_COMPLETE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_2013_EVENT_MISSION_COMPLETE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// 보상받은 레벨 적용
			if( g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetSelectUnit() != NULL )
			{
				if( g_pData->GetMyUser()->GetSelectUnit()->GetOldYearMissionRewardedLevel() < kEvent.m_iRewardedLevel )
				{
					g_pData->GetMyUser()->GetSelectUnit()->SetOldYearMissionRewardedLevel( kEvent.m_iRewardedLevel );
					g_pMain->GetMemoryHolder()->UpdateNewYear2014Event();
				}
			}

			// 삭제된 인벤토리 적용
			if( kEvent.m_vecInventorySlotInfo.empty() == false )
			{
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				if( NULL != g_pData->GetUIManager()->GetUIInventory() )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				}
			}

			return true;
		}
	}

	return false;
}

bool CX2StateMenu::Handler_EGS_2014_EVENT_MISSION_COMPLETE_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_2014_EVENT_MISSION_COMPLETE_REQ );
	g_pMain->AddServerPacket( EGS_2014_EVENT_MISSION_COMPLETE_ACK );

	return true;
}
bool CX2StateMenu::Handler_EGS_2014_EVENT_MISSION_COMPLETE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// 패킷 처리
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_2014_EVENT_MISSION_COMPLETE_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_2014_EVENT_MISSION_COMPLETE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			// 미션 단계 적용
			if( g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetSelectUnit() != NULL )
			{
				int iStepID = g_pData->GetMyUser()->GetSelectUnit()->GetNewYearMissionStepID();
				g_pData->GetMyUser()->GetSelectUnit()->SetNewYearMissionStepID( iStepID + 1 );
			}

			g_pMain->GetMemoryHolder()->UpdateNewYear2014Event();
			return true;
		}
	}

	return false;
}
