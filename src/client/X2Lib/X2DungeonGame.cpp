#include "stdafx.h"
#include "./X2DungeonGame.h"






CX2DungeonGame::CX2DungeonGame(void) :
m_pDungeonRoom( NULL ),
m_pDungeon( NULL ),
m_WinTeam( CX2Room::TN_NONE ),
m_bGoNextPacketSended( false ),
m_bShowClear( false ),
m_fClearEffDelayTime( 0.f ),
m_fDungeonClearLimitTimeOut( 0.f ),
m_bDungeonClearLimitTimeOut( false ),
m_bShowDungeonClearLimitTimeOut( false ),	// 초기화 안 해 주던 버그 수정 by 박진웅
m_bCountTimeLeftAfterSubStageOpen( false ),
m_fTimeLeftAfterSubStageOpen( 0.f ),
m_pFontForTime( NULL ),
m_bIsSendSkipContinueNot( false ),
m_bIsThereTutorialMsg( false ),
m_pFontForTutorialMsg( NULL ),
m_wstrTutorialMsg( L"" ),
m_fTimeLeftForTutorialMessage( 0.f ),
m_coTutorialMsg( D3DXCOLOR(1,1,1,1) ),
m_coTutorialMsgIncrement( D3DXCOLOR(-0.04f, -0.04f, -0.04f, 0.f) ),
m_iTutorialMsgFlashCount( 0 ),
m_bShowTutorialUI( false ),
m_bEnableRemainTime( false ),
m_fRemainPlayTime( 0.f ),
m_bStartRealTraining( false ),
m_pTrainingGameUI( NULL ),
m_bShowDungeonRemainTime( false ),
m_bShowDungeonPlayTimeForQuest( false ),
m_bHideDungeonPlayTimeAfterLoading( false ),
//m_ReservedPacket;
//m_StopContinueAlarmText[256];
//m_TimerManyPurpose1;
//m_TimerManyPurpose2;
m_pMonsterIndicator( NULL ),
m_pDamageDataForDungeonTimeOut( NULL ),
m_bKilledBoss( false ),
m_hBossName(INVALID_PARTICLE_SEQUENCE_HANDLE)
, m_iDefenceDungeonWaveID( -1 )		// 어둠의 문 서브스테이지( Wave )
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
, m_bEscape( true )
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
, m_bShowStageReaminTime( false )
{
	// super class member init
	m_GameType = GT_DUNGEON;

	// variable assign
	m_pFontForTime			= g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_HEADLINE_30_NORMAL );
	m_pFontForTutorialMsg	= g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_20_BOLD );
	m_pDLGContinue	= new CKTDGUIDialog( this, L"DLG_Dungeon_Game_Continue_New.lua" );
	m_pDLGContinue->SetShow( false );
	m_pDLGContinue->SetEnable( false );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGContinue );

#ifdef _GAMANIA_HK_
	CKTDGUIStatic* pStaticRestore_Continue_For_Cash = (CKTDGUIStatic*)m_pDLGContinue->GetControl( L"Restore_Continue_Cash" );
	if( NULL != pStaticRestore_Continue_For_Cash )
	{
		pStaticRestore_Continue_For_Cash->GetPicture(0)->SetTex_LUA( "DLG_Common_New_Texture24_A.tga", "CONTINUE_MESSAGE_2" );
	}
#endif // _GAMANIA_HK_

	m_pSoundContinue			= g_pKTDXApp->GetDeviceManager()->OpenSound( L"Continue.ogg" );

	m_bShowDungeonPlayTimeForQuest		= DetermineShowDungeonPlayTime();

	memset( m_StopContinueAlarmText, 0, sizeof(WCHAR)*256 );

	//{{ kimhc // 2010.3.26 // 무한 스킬 버그 수정
	if ( g_pInstanceData != NULL )
	{
		g_pInstanceData->SetSendGameStartReqPacket( false );		// packet을 전송하지 않은 상태로 초기화
	}
	//}} kimhc // 2010.3.26 // 무한 스킬 버그 수정

	m_bCheckSubStageGoNext = false;

	m_pDLGTitle			= NULL;
	m_fDungeonNameTime	= 0.f;

}

CX2DungeonGame::~CX2DungeonGame(void)
{
	/*
	Release() 에서 수행
	*/
}

/** @function : Release
	@brief : X2Game의 소멸자에 실행할 것들을 이곳에서 실행함
	(이렇게 따로 만들어 준 이유는 X2Game 소멸자가 X2GameUnit 등을 소멸할 때
	X2GameUnit 등의 소멸자 내부 또는 호출되는 함수 들에 내부에서 다시 X2Game을 참조하는 경우
	가 많아서 다 찾아내기 힘들기에 이렇게 Release로 만들었습니다.
	X2Game이 소멸되는 경우는 현재 Pvp, Dungeon, BattleField의 스테이트가 소멸될 때 이므로
	해당 스테이트의 소멸자에서 Release를 호출 하도록 하였습니다.
	더 좋은 방법이 있으신 분은 알려주세요 - 2012-11-11 (김현철)
*/
/*virtual*/ void CX2DungeonGame::Release()
{
	SAFE_DELETE( m_pMonsterIndicator );
	SAFE_DELETE( m_pDungeon );

	SAFE_DELETE( m_pTrainingGameUI );


	SAFE_DELETE_DIALOG( m_pDLGContinue );

	SAFE_CLOSE( m_pSoundContinue );

	SAFE_DELETE_DIALOG( m_pDLGTitle );

//{{ robobeg : 2013-06-12
	//g_pData->GetUnitManager()->UnloadAllNPCInitData();
//}} robobeg : 2013-06-12

	if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
	{
		g_pData->GetPlayGuide()->DeleteQuestGuideParticle();
		g_pData->GetPlayGuide()->DeleteInputGuideParticle();
	}

	if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hBossName )
		GetMinorParticle()->DestroyInstanceHandle(m_hBossName);

	CX2Game::Release();	/// 상위 클래스의 소멸자가 나중에 호출
}

HRESULT CX2DungeonGame::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	CX2Game::OnFrameMove( fTime, fElapsedTime );

	if( m_GameState == GS_PLAY )
	{
		//{{ oasis907 : 김상윤 [2010.4.26] //
		DungeonTimeOutFrameMove( fTime, fElapsedTime );
		//}}
		CountTimeSubStageFrameMove( fTime, fElapsedTime );

		if( m_pDungeon != NULL )
			m_pDungeon->OnFrameMove( fTime, fElapsedTime );

		if( NULL != m_pMonsterIndicator )
		{
			m_pMonsterIndicator->OnFrameMove();
		}


		if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
		{
			if( m_fRemainPlayTime > 0.0f )
				m_fRemainPlayTime -= fElapsedTime;

			if( m_fRemainPlayTime <= 0.0f )
				m_fRemainPlayTime = 0.0f;
		}

		if ( m_pDLGTitle != NULL )
		{
			m_fDungeonNameTime += fElapsedTime;
			if(m_fDungeonNameTime >= 2.f && m_fDungeonNameTime <= 3.f)
			{
				CloseDungeonName(m_fDungeonNameTime);
			}
			else if(m_fDungeonNameTime > 3.f)
			{
				m_fDungeonNameTime = 0.f;
				SAFE_DELETE_DIALOG( m_pDLGTitle );
			}
		}

		if( m_bShowClear == true )
		{
			if( m_fClearEffDelayTime <= 0.0f )
			{
				m_bShowClear = false;
				m_fClearEffDelayTime = 0.0f;

				ShowClearEffect();
			}
			else
			{
				m_fClearEffDelayTime -= m_fElapsedTime;
			}
		}

		if( m_bGoNextPacketSended == false && m_pDungeon->GetNowStage()->GetNowSubStage() != NULL )
		{
			CX2GUUser* pCX2GUUser = GetMyUnit();
			if( pCX2GUUser != NULL && pCX2GUUser->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
			{
				const CX2GUUser::SyncData& kSyncData = pCX2GUUser->GetSyncData( false );
				if( m_pDungeon->GetNowStage()->GetNowSubStage()->TouchStartLineMapSet( kSyncData.lastTouchLineIndex ) == true )
				{
					m_bCheckSubStageGoNext = true;

					//CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();
					Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ( m_pDungeon->GetNowStage()->GetBeforeSubStageIndex(), m_pDungeon->GetNowStage()->GetNowSubStageIndex() );
					m_bGoNextPacketSended = true;

					m_pDungeon->GetNowStage()->CloseBeforeSubStage();
				}
			}
		}

		if( false == m_bEnableNPCTalkBox )
		{
			m_fElapsedTimeAfterDisableNPCTalkBox += fElapsedTime;
			if( m_fElapsedTimeAfterDisableNPCTalkBox > 2.f )
			{
				m_bEnableNPCTalkBox = true;
			}
		}


		m_pDLGContinue->SetShow( false );
		m_pDLGContinue->SetEnable( false );

		if( GetResurrectionOperationCond() == true )
		{
			ResurrectionFrameMove( fTime, fElapsedTime );
		}


		if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() )
		{
			if( NULL != m_pTrainingGameUI )
			{
				m_pTrainingGameUI->OnFrameMove( fTime, fElapsedTime );
			}

			if( true == m_bEnableRemainTime )
			{
				if( m_fRemainPlayTime > 0.0f )
					m_fRemainPlayTime -= fElapsedTime;

				if( m_fRemainPlayTime <= 0.0f )
					m_fRemainPlayTime = 0.0f;
			}
		}
	}

	return S_OK;
}

HRESULT CX2DungeonGame::OnFrameRender()
{
	CX2Game::OnFrameRender();

	if( m_GameState == GS_PLAY )
	{

		if( true == m_bCountTimeLeftAfterSubStageOpen )
		{
			// 던전 substage 진행 경고 남은 시간
			WCHAR timeBuf[200] = {0,};
			swprintf_s( timeBuf, L"%d", (int)m_fTimeLeftAfterSubStageOpen );
			m_pFontForTime->OutTextXY( 512, 100, timeBuf, D3DXCOLOR(1,0.6f,0,1),
									CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );
		}
		else
		{
			//TextOutCurrQuestState();
		}



		if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() && true == m_bStartRealTraining &&
			false == g_pMain->GetIsPlayingFreeTraining() )
		{


			// 훈련 게임 남은 시간
			float fTime = GetRemainPlayTime();

			WCHAR wszText[64] = L"";
			int iMinute = (int)(fTime / 60.f);
			int iSecond = (int)(fTime-(float)iMinute*60.f);

			if( 0 != iMinute )
			{
				if( iSecond < 10 )
				{
					StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"%2d:0%d", iMinute, iSecond );
				}
				else
				{
					StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"%2d:%2d", iMinute, iSecond );
				}
			}
			else
			{
				if( iSecond < 10 )
				{
					StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"0%d", iSecond );
				}
				else
				{
					StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"%2d", iSecond );
				}
			}

			m_pFontForTime->OutTextXY( 512, 100, wszText, D3DXCOLOR(1,0.6f,0,1),
							CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );
		}

		if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() && false == g_pMain->GetIsPlayingTutorial() )
		{
			if( false == m_bHideDungeonPlayTimeAfterLoading &&
				false == m_bCountTimeLeftAfterSubStageOpen )
			{

				float fTime = -1.f;
				if( true == m_bShowDungeonRemainTime )					// 던전 진행 남은 시간 표시
				{
					fTime = m_fRemainPlayTime;
				}
				else if( true == m_bShowDungeonPlayTimeForQuest )		// 던전 진행 시간 표시
				{
					fTime = m_pDungeonRoom->GetPlayTimeLimit() - m_fRemainPlayTime;
				}
				//{{ oasis907 : 김상윤 [2010.5.6] // 특정 시간 지나면 특정한 결과에 처하는 던전
				else if( true == m_bShowDungeonClearLimitTimeOut )
				{
					fTime = m_fDungeonClearLimitTimeOut;
				}
				//}}
				else if( m_bShowStageReaminTime == true )
				{
					fTime = m_pDungeon->GetNowStage()->GetNowSubStage()->GetRemainTimeInClearCondition();
				}

				if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == false )
				{
					if( fTime > 0.f )
					{
						WCHAR wszText[64] = L"";
						int iMinute = (int)(fTime / 60.f);
						int iSecond = (int)(fTime-(float)iMinute*60.f);

						if( 0 != iMinute )
						{
							if( iSecond < 10 )
							{
								StringCchPrintf(wszText, 64, L"%2d:0%d", iMinute, iSecond );
								//wsprintf( wszText, L"%2d:0%d", iMinute, iSecond );
							}
							else
							{
								StringCchPrintf( wszText, 64, L"%2d:%2d", iMinute, iSecond );
								//wsprintf( wszText, L"%2d:%2d", iMinute, iSecond );
							}
						}
						else
						{
							if( iSecond < 10 )
							{
								StringCchPrintf( wszText, 64 ,  L"0%d", iSecond );
								//wsprintf( wszText, L"0%d", iSecond );
							}
							else
							{
								StringCchPrintf( wszText, 64 ,  L"%2d", iSecond );
								//wsprintf( wszText, L"%2d", iSecond );
							}
						}

						m_pFontForTime->OutTextXY( 512, 100, wszText, D3DXCOLOR(1,1,0,1),
							CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );
					}
				}
			}
		}
	}

	return S_OK;
}


bool CX2DungeonGame::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( CX2Game::MsgProc( hWnd, uMsg, wParam, lParam ) == true )
		return true;

	return false;
}

void CX2DungeonGame::GameLoading( CX2Room* pRoom )
{
	const int LEVEL_DEFFERENCE_BETWEEN_DUNGEON_AND_USER = 20;
	const int LEVEL_DEFFERENCE_BETWEEN_OTHERUSER_AND_ME = 13;

	m_pDungeonRoom	= (CX2DungeonRoom*) pRoom;
	m_pRoom			= pRoom;

	SystemLoading();
	DungeonLoading();
	UnitLoading();
	EtcLoading();


	if( NULL != g_pInstanceData->GetMiniMapUI() )
	{
		// 09.03.23 태완 : 미니맵 UI 코드변경
		//g_pInstanceData->GetMiniMapUI()->GetDungeonMiniMap()->SetTitle( dungeonName.c_str() );
		if( false == g_pMain->GetIsPlayingTutorial() &&
			CX2Main::XS_TRAINING_GAME != g_pMain->GetNowStateID() )
		{
			g_pInstanceData->GetMiniMapUI()->SetShowMiniMap( CX2MiniMapUI::MMT_DUNGEON, true );

			wstring dungeonName = L"";
			if( NULL != m_pDungeonRoom )
			{
				dungeonName = g_pData->GetDungeonManager()->MakeDungeonNameString( m_pDungeonRoom->GetDungeonID(), m_pDungeonRoom->GetDifficulty(),
					(CX2Dungeon::DUNGEON_MODE) g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode );
			}

			g_pInstanceData->GetMiniMapUI()->SetTitle( dungeonName.c_str() );



	//{{ kimhc // 2010.5.2 // 비밀던전 작업(경험치)
			g_pInstanceData->GetMiniMapUI()->SetNoExpAtThisDungeon( false );
			g_pInstanceData->GetMiniMapUI()->SetWarningForGettingExp( false );

			switch ( g_pData->GetDungeonManager()->GetDungeonType( m_pDungeon->GetDungeonData()->m_DungeonID ) )
			{
			case CX2Dungeon::DT_NORMAL:
				{
					//{{ kimhc // 2011-04-28 // 디펜스던전
					if ( false == g_pData->GetDungeonManager()->IsDefenceDungeon( m_pDungeon->GetDungeonData()->m_DungeonID ) )
					//}} kimhc // 2011-04-28 // 디펜스던전
					{
						int levelDifference = abs( m_pDungeon->GetDungeonData()->m_MinLevel - g_pData->GetSelectUnitLevel() );
						if( levelDifference >= LEVEL_DEFFERENCE_BETWEEN_DUNGEON_AND_USER )
							g_pInstanceData->GetMiniMapUI()->SetNoExpAtThisDungeon( true );
					}
				} break;

			case CX2Dungeon::DT_SECRET:
				{
					int		iLevel	= ( GetMyUnit() != NULL ) ? GetMyUnit()->GetUnitLevel() : 0;
					int		iSize	= GetUserUnitListSize();
					bool	bShow	= false;

					if ( iLevel > m_pDungeon->GetDungeonData()->m_MaxLevel
						 && iLevel - m_pDungeon->GetDungeonData()->m_MaxLevel >= LEVEL_DEFFERENCE_BETWEEN_DUNGEON_AND_USER )
							bShow = true;
					else if ( iLevel < m_pDungeon->GetDungeonData()->m_MinLevel
							  && m_pDungeon->GetDungeonData()->m_MinLevel - iLevel >= LEVEL_DEFFERENCE_BETWEEN_OTHERUSER_AND_ME )
							bShow = true;
					else
					{
						for ( int i = 0; i < iSize; i++ )
						{
							CX2GUUser* pUser = GetUserUnit( i );

							if ( pUser != NULL )
							{
								int iOtherLevel = pUser->GetUnitLevel();

								if ( iOtherLevel > iLevel &&
									iOtherLevel - iLevel >= LEVEL_DEFFERENCE_BETWEEN_OTHERUSER_AND_ME )
								{
									bShow = true;
									break;
								}
							}
						}
					}

					g_pInstanceData->GetMiniMapUI()->SetWarningForGettingExp( bShow );

				} break;

			default:
			case CX2Dungeon::DT_HENIR:
				break;
			}

	//}} kimhc // 2010.5.2 // 비밀던전 작업(경험치)

		}




	}

	m_fRemainPlayTime = m_pDungeonRoom->GetPlayTimeLimit();




	if( NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
	{
		g_pData->GetUIManager()->GetUIQuickSlot()->ResetQuickSlotUI();
	}

	SAFE_DELETE( m_pMonsterIndicator );
	if( false == g_pMain->GetIsPlayingTutorial() )		// 튜토리얼에서는 남은 몬스터 표시 화살표 안나오게
	{
		if( g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
			m_pMonsterIndicator = new CX2MonsterIndicator();
	}

//#ifdef	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER
	if ( g_pKTDXApp->GetDeviceManager() != NULL )
		g_pKTDXApp->GetDeviceManager()->ReleaseAllMemoryBuffers();
//#endif	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER

}

void CX2DungeonGame::GameStart()
{
	//PVP에서는 바로 시작하면 되지만
	//던전에서는 아직 스테이지, 서브 스테이지가 로드되지 않았으므로
	//스테이지 로드를 시작한다
	if( IsHost() == true )
	{
		//스테이지 로드를 요청한다
	}

	//{{ JHKang / 강정훈 / 2011.6.26 / 휴면 복귀 유저( 돌아온 용사 )
	SAFE_DELETE_DIALOG( m_pDLGTitle );

	m_pDLGTitle = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Buff_Title.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGTitle );
	CKTDGUIStatic *pStaticDlg = m_pDLGTitle->GetStatic_LUA("Buff_Title_Field_Created");

	CKTDGUIControl::CPictureData *pFieldComeBackUser = pStaticDlg->GetPictureIndex( 123 );
	if(pFieldComeBackUser != NULL)
	{
		pFieldComeBackUser->SetShow( false );
		if ( NULL != g_pData->GetPremiumBuffManager() && true == g_pData->GetPremiumBuffManager()->IsComeBackUser() || true == g_pData->GetPremiumBuffManager()->IsComeBackUserInParty() )
		{
			pFieldComeBackUser->SetShow( true );
		}
	}

	//}}
}

void CX2DungeonGame::StageLoading( int stageNum )
{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->Pause();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	wstringstream dungeonStageIndex;
	dungeonStageIndex << L"Stage Loading : " << stageNum;
	StateLog( dungeonStageIndex.str().c_str() );

	m_GameState			= GS_LOADING;
	m_bStopUnitChain	= true;

	//스테이지를 로딩하라는 NOT가 오면 로딩을 시작한다
	for( int i = 0; i < (int)m_CreateNPCDataList.size(); i++ )
	{
		CreateNPCData* pCreateNPCData = m_CreateNPCDataList[i];
		SAFE_DELETE( pCreateNPCData );
	}


	m_CreateNPCDataList.resize(0);

	DeleteAllNPCUnit();
	m_pDropItemManager->DeleteAllItem();

	ClearSpirit();


	CX2DungeonStage* pCX2DungeonStage = m_pDungeon->CreateStage( stageNum );
	CX2DungeonStage::StageData* pStageData = pCX2DungeonStage->GetStageData();


	SAFE_DELETE( m_pWorld );

	//m_pWorld = pCX2DungeonStage->FlushWorld();
	if( pStageData != NULL )
    {
//#ifdef BACKGROUND_LOADING_TEST // 2008-12-12
//        m_pWorld = g_pData->GetWorldManager()->ThreadRequest_CreateWorld( pStageData->m_WorldID );
//#else // BACKGROUND_LOADING_TEST // 2008-12-12
		m_pWorld = g_pData->GetWorldManager()->CreateWorld(	pStageData->m_WorldID );

		for( CX2GUUser* pUser : m_UserUnitList )
		{
			if( NULL == pUser )
				continue;

			pUser->ResetPhysicParam();
			pUser->InitPassiveSkillState();
		}

		// 첫 스테이지 이면 해당 던전의 default 보스 스테이지 월드 ID를 dungeon lounge world id로 설정한다
		if( 0 == stageNum )
		{
			g_pInstanceData->SetDungeonLoungeWorldID( m_pDungeon->GetDungeonData()->m_eDefaultDungeonLoungeWorldID );
		}

//#endif // BACKGROUND_LOADING_TEST // 2008-12-12


		// note!! dmlee 2008.12.21 일단 포탈 라인맵위치에 포탈 표시 파티클은 여기서 생성하는 걸로 하드 코딩
		if( NULL != m_pWorld &&
			NULL != m_pWorld->GetLineMap() )
		{
			for( int i=0; i<m_pWorld->GetLineMap()->GetNumLineData(); i++ )
			{
				const CKTDGLineMap::LineData* pLineData = m_pWorld->GetLineMap()->GetLineData( i );
				if( NULL != pLineData &&
					CKTDGLineMap::LT_POTAL == pLineData->lineType )
				{
					CX2WorldObjectParticle* pParticle = m_pWorld->CreateObjectParticle( GetMajorParticle(), "Peita_Teleport_MagicSquare01" );
					if( NULL != pParticle )
					{
						pParticle->SetParticlePos( ( pLineData->startPos + pLineData->endPos ) * 0.5f + D3DXVECTOR3(0, 3, 0 ) );
						pParticle->SetLayer( XL_EFFECT_0 );
					}
					if( true == g_pMain->GetIsPlayingTutorial() )
					{
						CX2WorldObjectParticle* pParticle = m_pWorld->CreateObjectParticle( GetMajorParticle(), "CompleteQuest" );
						if( NULL != pParticle )
						{
							pParticle->SetParticlePos( ( pLineData->startPos + pLineData->endPos ) * 0.5f + D3DXVECTOR3(0, 250, 0 ) );
							pParticle->SetLayer( XL_EFFECT_0 );
						}
					}
				}
			}
		}

    }//if

	// dungeon script에서 지정된 color를 world에 적용
	if( m_pDungeon->GetWorldColor().r != -1.f )
	{
		m_pWorld->SetOriginColor_LUA( m_pDungeon->GetWorldColor().r, m_pDungeon->GetWorldColor().g, m_pDungeon->GetWorldColor().b );
	}
	else if( pStageData->m_WorldColor.r != -1.f )
	{
		m_pWorld->SetOriginColor_LUA( pStageData->m_WorldColor.r, pStageData->m_WorldColor.g, pStageData->m_WorldColor.b );
	}

	if( m_pDungeon->GetUnitColor().r != -1.f )
	{
		m_pWorld->SetLightColor_LUA( m_pDungeon->GetUnitColor().r, m_pDungeon->GetUnitColor().g, m_pDungeon->GetUnitColor().b );
	}

	else if( pStageData->m_UnitColor.r != -1.f )
	{
		m_pWorld->SetLightColor_LUA( pStageData->m_UnitColor.r, pStageData->m_UnitColor.g, pStageData->m_UnitColor.b );
	}





	// 훈련소 게임이면 linemap의 landheight를 낮게 변경
	if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() )
	{

		if( NULL != m_pWorld &&
			NULL != m_pWorld->GetLineMap() )
		{
			m_pWorld->GetLineMap()->SetLandHeight( m_pWorld->GetLineMap()->GetLandHeight() - 300.f );
		}
	}



	// user unit들을 재배치

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = (CX2GUUser*)m_UserUnitList[i];
		if( pCX2GUUser == NULL )
			continue;

		pCX2GUUser->ClearRecieveSyncData();
		pCX2GUUser->InitPosition( true );
		if( pCX2GUUser->GetNowHp() > 0.0f || pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
		{
			if( pStageData->m_bStartMotion == true ) // 첫번째 또는 마지막 stage에만 start motion을 취한다.
			{
				if ( pCX2GUUser->GetRidingOn() )
					pCX2GUUser->StateChange( pCX2GUUser->GetRidingStartStateID(), true );
				else
					pCX2GUUser->StateChange( pCX2GUUser->GetStartStateID(), true );
			}
			else
			{
				if ( pCX2GUUser->GetRidingOn() )
					pCX2GUUser->StateChange( pCX2GUUser->GetRidingWaitStateID(), true );
				else
					pCX2GUUser->StateChange( pCX2GUUser->GetWaitStateID(), true );
			}
		}

		pCX2GUUser->SetPepperRunTime( 0.f );

		//GetX2Camera()->NomalDirectCamera( (CX2GameUnit*) pCX2GUUser );
	}

	m_pCamera->SetLandHeight( m_pWorld->GetLineMap()->GetLandHeight() );
	m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), g_pMain->GetGameOption().GetCameraDistance() );

//#ifdef	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER
	if ( g_pKTDXApp->GetDeviceManager() != NULL )
		g_pKTDXApp->GetDeviceManager()->ReleaseAllMemoryBuffers();
//#endif	X2OPTIMIZE_MASS_FILE_BUFFER_MANAGER

}

void CX2DungeonGame::StageStart()
{
	//호스트가 서브 스테이지를 로딩하라는 명령을 보낸다

	if( m_bBGMOn == true )
		m_pWorld->PlayBGM();

	if( m_pDungeon->GetNowStage()->GetStageData()->m_bStartMotion == false )
		g_pKTDXApp->GetDGManager()->StartFadeIn( 99999 );

	m_GameState = GS_PLAY;
	m_pWorld->GetLineMap()->DisableAllLineData();
	m_bKilledBoss = false;

	m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), g_pMain->GetGameOption().GetCameraDistance() );
	m_pCamera->OnFrameMove( 0, 0 );

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = m_UserUnitList[i];
		if( pCX2GUUser != NULL )
		{
			pCX2GUUser->InitPosition( true );
		}
	}

#if X2_DEV_TOOLS
	dbg::clog << L"StageStart! Index: " << m_pDungeon->GetNowStageIndex() << L" WORLD ID : " << (int)m_pDungeon->GetNowStage()->GetStageData()->m_WorldID << dbg::endl;
#endif
	wstringstream dungeonStageIndex;
	dungeonStageIndex << L"Stage Start : " << m_pDungeon->GetNowStageIndex();
	StateLog( dungeonStageIndex.str().c_str() );

	if( IsHost() == true )
	{
		Handler_EGS_DUNGEON_SUB_STAGE_OPEN_REQ( 0 );
	}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->Resume();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

    if ( g_pData->GetGameUDP() != NULL )
        g_pData->GetGameUDP()->RemoveAllPendingPingSends();

	if( NULL != m_pDungeon && NULL != m_pDungeon->GetDungeonData() &&
		m_pDungeon->GetDungeonData()->m_bShowStageTime == true )
	{
		m_bShowStageReaminTime = true;
	}

	if ( NULL != g_pData->GetRelationshipEffectManager() )
	{
		g_pData->GetRelationshipEffectManager()->SetMyRelationCloseEffectCooltime( 0.f );
	}

}


void CX2DungeonGame::SubStageOpen( int subStageNum )
{
	for( int i = 0; i < (int)m_CreateNPCDataList.size(); i++ )
	{
		CreateNPCData* pCreateNPCData = m_CreateNPCDataList[i];
		SAFE_DELETE( pCreateNPCData );
	}
	m_CreateNPCDataList.resize(0);


	CX2DungeonSubStage* pCX2DungeonSubStage				= m_pDungeon->GetNowStage()->CreateSubStage( subStageNum );

	CX2DungeonSubStage* pBeforeSubStage					= m_pDungeon->GetNowStage()->GetBeforeSubStage();

	if( pBeforeSubStage != NULL )
	{
		// 오현빈 // 2013-09-04 //
		// HOST가 보내는 SUB_STAEG_OPEN_NOT로 인해,
		// HOST가 아닌 유저는 클리어 조건을 체크 할 수 없기 때문에.
		// 클리어 시 수행해야 하는 구문 추가 처리
		if( false == IsHost() )
		{
			pBeforeSubStage->ProcessAfterSubStageClear_NotHost();
		}
		pBeforeSubStage->ToggleEndLineMapSet( false );
	}
	if( pCX2DungeonSubStage != NULL )
		pCX2DungeonSubStage->ToggleStartLineMapSet( true );

	m_bGoNextPacketSended	= false;


	if ( NULL != g_pX2Room && NULL != m_pDungeon )	// null check
	{
		if ( g_pX2Room->GetUserNum() > 1 )			// 파티 중일 때
		{
			if ( subStageNum <= 0 && m_pDungeon->GetNowStageIndex() <= 0 ) // 첫번째 스테이지, 첫번째 서브 스테이지 일 경우
			{
				m_pDungeon->SetBeforeStageIndexUsingPacket(0);				// Index 를 초기화 시켜준다.
				m_pDungeon->SetBeforeSubStageIndexUsingPacket(0);
			}
			else
			{
				// 첫번째 스테이지, 첫번째 서브 스테이지가 아닐 경우, 해당 유저의 정보를 서버에 전송한다.
				Send_EGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT (m_pDungeon->GetBeforeStageIndexUsingPacket(),
														   m_pDungeon->GetBeforeSubStageIndexUsingPacket() );
			}
		}
	}


	if( subStageNum != 0 )
	{
		if( false == g_pMain->GetIsPlayingTutorial() &&
			g_pX2Room->GetUserNum() > 1 )
		{
			CX2GUUser* pCX2GUUser = GetMyUnit();
			if( pCX2GUUser != NULL && CX2GameUnit::GUSI_DIE != pCX2GUUser->GetGameUnitState() )
			{
				m_bCountTimeLeftAfterSubStageOpen = true;
				m_fTimeLeftAfterSubStageOpen = 20.0f;
			}
		}
	}
	else
	{
		m_bCheckSubStageGoNext = true;
		//CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();

		Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ( m_pDungeon->GetNowStage()->GetBeforeSubStageIndex(), m_pDungeon->GetNowStage()->GetNowSubStageIndex() );
		m_bGoNextPacketSended = true;

		m_pDungeon->GetNowStage()->CloseBeforeSubStage();
	}





	m_bEnableNPCTalkBox = false;
}

void CX2DungeonGame::SubStageLoading( int subStageNum )
{

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->Pause();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	wstringstream dungeonSubStageIndex;
	dungeonSubStageIndex << L"Sub Stage Loading : " << subStageNum;
	StateLog( dungeonSubStageIndex.str().c_str() );
	// 던전 지도
	m_pDungeon->SetDungeonMapOpen( m_pDungeon->GetNowStageIndex(), subStageNum, true );
	CX2StateDungeonGame* pStateDungeonGame = (CX2StateDungeonGame*) g_pMain->GetNowState();
	if( NULL != pStateDungeonGame )
	{
		if( pStateDungeonGame->GetDungeonMapUI() != NULL )
			pStateDungeonGame->GetDungeonMapUI()->CreateDungeonMapUI();
	}

	g_pKTDXApp->SkipFrame();
}

void CX2DungeonGame::SubStageStart()
{
#if !X2_DEV_TOOLS
#endif
	//장막을 열고 서브 스테이지를 시작한다
	m_bLastKillCheck		= false;
	m_fLastKillWaitTime		= 0.05f;

	m_bShowClear			= false;
	m_fClearEffDelayTime	= 0.0f;

	CX2DungeonSubStage* pBeforeSubStage = m_pDungeon->GetNowStage()->GetBeforeSubStage();
	CX2DungeonSubStage* pNowSubStage	= m_pDungeon->GetNowStage()->GetNowSubStage();
	if( pBeforeSubStage != NULL )
		pBeforeSubStage->BreakCurtain();

	if( m_pDungeon == NULL || m_pDungeon->GetNowStage() == NULL || m_pDungeon->GetNowStage()->GetNowSubStage() == NULL )
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

		string strMailTitle = ANTI_HACK_STRING_AntiHacking_Process_Dungeon_Is_NULL_SubStageStart;
		g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
		g_pKTDXApp->SetFindHacking( true );
		return;
	}




	m_pDungeon->GetNowStage()->DeleteBeforeSubStage();
	m_pDungeon->GetNowStage()->GetNowSubStage()->Start();
	m_pDungeon->GetNowStage()->StopGoEffect();
	m_bCountTimeLeftAfterSubStageOpen = false;


	pNowSubStage->SwapToGameNPCUnit();

	wstringstream dungeonSubStageIndex;
	dungeonSubStageIndex << L"Sub Stage Start : " << pNowSubStage->GetSubStageIndex();
	StateLog( dungeonSubStageIndex.str().c_str() );



	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( pCX2GUNPC != NULL )
		{
			pCX2GUNPC->SetAIEnable( true );

			if( NULL != pCX2GUNPC->GetNPCAI() &&
				CX2NPCAI::NAT_ALLY == pCX2GUNPC->GetNPCAI()->GetAIType() )
			{
				// nothing
			}
			else
			{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                if ( pCX2GUNPC->GetNowStateID() != CX2GameUnit::GUSI_DIE )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    				pCX2GUNPC->StateChangeForce( pCX2GUNPC->GetStartState() );
			}

			for( int i = 0; i < (int)pNowSubStage->GetSubStageData()->m_NPCDataList.size(); i++ )
			{
				CX2DungeonSubStage::NPCData* pNPCData = pNowSubStage->GetSubStageData()->m_NPCDataList[i];
				if( pNPCData != NULL && pCX2GUNPC->GetUID() == pNPCData->m_UID )
				{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                    if ( pCX2GUNPC->GetNowStateID() != CX2GameUnit::GUSI_DIE )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                    {
					    pCX2GUNPC->SetSiegeModeForce( false );
					    pCX2GUNPC->SetSiegeModeForce( pNPCData->m_bSiegeMode );
                    }
				}
			}


			//엘리트 몬시타가 될것인지 보자잉~~~~~~~~~~~?

			map< int, KAttribEnchantNpcInfo  >::const_iterator mit
				= GetDungeon()->GetNowStage()->GetStageData()->m_mapAttribEnchantNpcInfo.find( pCX2GUNPC->GetUID() );
			if ( mit != GetDungeon()->GetNowStage()->GetStageData()->m_mapAttribEnchantNpcInfo.end() )
			{
				const KAttribEnchantNpcInfo& kAttribEnchantNpcInfo = mit->second;
				pCX2GUNPC->ApplyEnchantMonster( kAttribEnchantNpcInfo );
			}
		}
	}


	if( NULL != pNowSubStage )
	{
		pNowSubStage->SetMonsterCountForSubStageClearCheck();
	}

	CX2StateDungeonGame* pStateDungeonGame = (CX2StateDungeonGame*) g_pMain->GetNowState();
	if( NULL != pStateDungeonGame )
	{
		int subStageNum = pNowSubStage->GetSubStageIndex();
		if( subStageNum == 0 )
		{
			// 스테이지 로딩중에 Z키를 누르고 있다가 스테이지 시작하자 마자 놓으면 필살기 나가는 것을 막기 위해서
			if( NULL != GetMyUnit() )
				GetMyUnit()->SetNowChargeMpForDetonation( 0.f );

			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_DUNGEON_GAME:
				{
					if( false == g_pMain->GetIsPlayingTutorial() )
					{
						const map< std::pair<int,int>, CX2Dungeon::DungeonMapData >& mapDungeonMapData = m_pDungeon->GetMapDungeonMapData();
						map< std::pair<int,int>, CX2Dungeon::DungeonMapData >::const_iterator it;
						it = mapDungeonMapData.find( std::make_pair( m_pDungeon->GetNowStageIndex(), subStageNum ) );
						if( it != mapDungeonMapData.end() )
						{
							const CX2Dungeon::DungeonMapData& dungeonMapData = it->second;
							if( false == dungeonMapData.m_bIsBossStage )
							{
								pStateDungeonGame->GetDungeonMapUI()->OnFirstSubStage();
							}
						}
					}
				} break;
			default:
				break;
			}
		}
	}

	// 이브의 소환 몬스터를 캐릭터 옆으로 이동시켜 준다
	for( int i=0; i< GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = GetUnit(i);
		if( NULL == pUnit )
			continue;

		if( CX2GameUnit::GUT_USER != pUnit->GetGameUnitType() )
			continue;

		CX2GUUser* pUser = (CX2GUUser*) pUnit;
		if( NULL != pUser->GetUnit() )
		{
			pUser->CallSummonedNPC();

			// 아토믹 쉴드 데미지 감소 문제 수정
			pUser->SetShieldDamageReduce( 1.f );
		}
	}




	if( true == m_bShowTeleportEffectOnStageStart )
	{
		m_bShowTeleportEffectOnStageStart = true; // FieldFix: 이건 없어도 될 듯
		CreateStageLoadingTeleportEffectForAllUser( false );
	}


	m_bStopUnitChain = false;

	m_fElapsedTimeAfterDisableNPCTalkBox = 0.f;

	CanNotInputAndDisalbeNPCAI(false);

	g_pKTDXApp->SkipFrame();

#if !X2_DEV_TOOLS
#endif


#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	KOGGamePerformanceCheck::GetInstance()->Resume();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

    if ( g_pData->GetGameUDP() != NULL )
        g_pData->GetGameUDP()->RemoveAllPendingPingSends();

	if( NULL != GetMyUnit() && GetMyUnit()->GetUnitType() == CX2Unit::UT_ADD )
	{
		GetMyUnit()->InitPhaseShift();

		/// 스테이지 전환시, 각성중인데 각성 버프는 걸려있지 않았다면 해제 시켜주자
		if ( true == GetMyUnit()->GetIsFormationMode() && false == GetMyUnit()->IsApplyBuffByBuffTempletID( BTI_FORMATION_MODE ) )
			GetMyUnit()->SetIsFormationMode( false );

		/// 스테이지 전환시, 각성은 되어 있지 않은데 각성 버프는 걸려있다면 해제 시켜 주자
		if ( false == GetMyUnit()->GetIsFormationMode() && true == GetMyUnit()->IsApplyBuffByBuffTempletID( BTI_FORMATION_MODE ) )
			GetMyUnit()->EraseBuffTempletFromGameUnit( BTI_FORMATION_MODE );
	}
}





void CX2DungeonGame::DungeonLoading(bool bIsNpcLoad)
{
	SEnum::DUNGEON_ID dungeonID = (SEnum::DUNGEON_ID)(m_pDungeonRoom->GetDungeonID() + m_pDungeonRoom->GetDifficulty());
	SAFE_DELETE( m_pDungeon );
	m_pDungeon = g_pData->GetDungeonManager()->CreateDungeon( dungeonID, bIsNpcLoad );


	if( NULL != m_pDungeon &&
		NULL != m_pDungeon->GetDungeonData() &&
		m_pDungeon->GetDungeonData()->m_fTimeLimit > 0.f )	// 시간 제한이 있는 던전이라면 던전 플레이 남은 시간 표시
	{
		m_bShowDungeonRemainTime = true;
	}


}

bool CX2DungeonGame::DetermineLastKill()
{
	KTDXPROFILE();

	if( m_bLastKillCheck == true )
		return false;

	int remainRedTeam = 0;	// 유저팀에 남아있는 사람 수
	int dyingRedTeam = 0;	// 죽은 사람 수

	int remainMonster = 0;  // 몬스팀에 남아있는 몬스터 수
	int dyingMonster = 0;	// 몬스터 팀에 죽은 몬스터 수

	//{{ oasis907 : 김상윤 [2010.4.29] //
	bool bIsNotLastKill = false;
	//}}

	CX2GameUnit* pLastRedTeam = NULL;
	CX2GameUnit* pLastMonster = NULL;
	CX2GameUnit* pLastUnit = NULL;
	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];

		if ( pCX2GameUnit == NULL )
		{
			continue;
		}

		// 유저 팀이고 아직 죽는 스테이트가 아니면
		if( pCX2GameUnit->GetTeam() == CX2Room::TN_RED &&
			pCX2GameUnit->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			// 유저팀 남아있는 사람 수 증가
			remainRedTeam++;
			if( pCX2GameUnit->GetNowHp() <= 0.0f )
			{
				if( pLastRedTeam != NULL )
				{
					// 누군가가 이미 있으면 죽은 후 지난 시간 체크 해서 나중에 죽은 사람으로 지정
					if( pLastRedTeam->GetDieAfterTime() > pCX2GameUnit->GetDieAfterTime() )
						pLastRedTeam = pCX2GameUnit;
				}
				else
				{
					pLastRedTeam = pCX2GameUnit;
				}

				// 유저팀 죽은 사람 수 증가
				dyingRedTeam++;
			}
		}
		// 몬스터 팀이고 아직 죽는 스테이트가 아니면
		else if( pCX2GameUnit->GetTeam() == CX2Room::TN_MONSTER &&
				 pCX2GameUnit->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			// 몬스터 라면 액티브 몬스터 인지 체크
			if( pCX2GameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
			{
				CX2GUNPC* pCX2GUNPC = (CX2GUNPC*)pCX2GameUnit;
				if( pCX2GUNPC->IsActiveMonster() == false )
					continue;
			}

			// 남아있는 몬스터 수
			remainMonster++;
			if( pCX2GameUnit->GetNowHp() <= 0.f )
			{
				// HP가 0이더라도 NEVER_DIE_THIS_STATE가 true 면 패스
				//{{ kimhc // 2010-07-21 // 현재 스테이트에서 HP가 0이 되어도 죽었다고 판단하지 않게 할 수 있는 기능
				if ( static_cast< CX2GUNPC* >( pCX2GameUnit )->GetNeverDieThisState() == true )
					continue;
					//}} kimhc // 2010-07-21 // 현재 스테이트에서 HP가 0이 되어도 죽었다고 판단하지 않게 할 수 있는 기능

				if( pLastMonster != NULL )
				{
					// 나중에 죽은 유닛으로 지정
					if( pLastMonster->GetDieAfterTime() > pCX2GameUnit->GetDieAfterTime() )
						pLastMonster = pCX2GameUnit;
				}
				else
				{
					// 지정되어 있지 않으면 지정
					pLastMonster = pCX2GameUnit;
				}

				// 몬스터 팀의 죽은 유닛 수 증가
				dyingMonster++;
			}
		}
	}

	// 남은 유저 수가 죽은 유저 수보다 작거나 같으면 또는 남은 몬스터의 수가 죽은 몬스터의 수보다 작거나 같으면
	// 생각해보면
	if( remainRedTeam <= dyingRedTeam || remainMonster <= dyingMonster )
	{
		m_fLastKillWaitTime -= m_fElapsedTime;		// 타임을 돌린다
	}

	//{{ oasis907 : 김상윤 [2010.4.29] // 같은 서브 스테이지 내의 중복 보스 몬스터 킬연출 위해
	else
	{
		// 이거 좀 잘 못 된듯..
		// 밑에 m_fLastKillWaitTimeAfterRebirth 도 같이 0보다 작을때 bIsNotLastKill 이 true여야 하는거 아닌가?
		bIsNotLastKill = true;
	}
	//}}

	if( remainMonster <= dyingMonster )
	{
		pLastUnit = pLastMonster;
	}
	else
	{
		pLastUnit = pLastRedTeam;
	}

	//
	// 모든 사람이 죽은 후에 부활할 때 방장 컴에서 스테이지 클리어 안되는 문제 해결하기 위한 임시 방편, 부활 후에 1초 정도 기다린다.
	// 현상:
	// 부활 직후에 부활한 사람의 HP가 0보다 작은 값이 되어서 방장컴에서는 m_bLastKillCheck 가 true로 되어버리고,
	// 실제 보스몬스터를 잡았을 때는 스테이지 클리어 처리가 안되게 됨
	if( m_fLastKillWaitTimeAfterRebirth > 0.f )
	{
		m_fLastKillWaitTimeAfterRebirth -= m_fElapsedTime;
		if( m_fLastKillWaitTimeAfterRebirth < 0.f )
		{
			m_fLastKillWaitTimeAfterRebirth = 0.f;
		}
	}


	if( false == m_bLastKillCheck && // FieldFix: 이건 의미 없는 확인 (위에서 true인지를 판단하여 retur시킴)
		m_fLastKillWaitTime <= 0.0f &&
		m_fLastKillWaitTimeAfterRebirth <= 0.f )
	{
		// pLastUnit이 지정되어 있으면
		if( pLastUnit != NULL )
		{
			bool bLastKillSlowMotion = true;

			// pLastUnit 이 자기 자신이면
			if( pLastUnit == GetMyUnit() )
			{
				// 패킷 보내고
				((CX2GUUser*)pLastUnit)->SendFrameData();

				// NPC 중에 focusCamera를 가진 NPC가 존재하면
				CX2GUNPC* pCX2GUNPC = GetFoucusCameraNPC();
				if( pCX2GUNPC != NULL )
				{
					// 100초 동안 그 NPC한테 focus를..
					SetTimerFocusUnit( pCX2GUNPC, 100.0f );
				}
				else
				{
					// 1초 동안 pLastUnit(자기자신) 한테 포커스를...
					SetTimerFocusUnit( pLastUnit, 1.0f );
				}
			}
			// pLastUnit 이 NPC면
			else if( pLastUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
			{
				// 호스트인 경우
				if( IsHost() == true )
				{


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
                    if ( g_pData->GetGameUDP()->GetNonRelayUIDs().empty() == false
                        || g_pData->GetGameUDP()->GetRelayUIDs().empty() == false )
                    {
					    for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
					    {
						    CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
						    if( pCX2GUNPC != NULL )
							    pCX2GUNPC->SendPacketImmediateForce( m_kFrameUDPPack );
					    }
                    }
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//					//아래는 의미없는 코드인 것 같다( by hcsung )
//					// 전체 NPC의 패킷을 보내고... (pLastUnit만 보내면 되는거 아닌가??)
//					KXPT_UNIT_NPC_SYNC_PACK kXPT_UNIT_NPC_SYNC_PACK;
//
//					for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
//					{
//						CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
//						if( pCX2GUNPC != NULL )
//							pCX2GUNPC->SendPacketImmediateForce( kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList );
//					}
//
//
//
//					BroadCast_XPT_UNIT_NPC_SYNC_PACK();
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

					//KSerBuffer buff;
					//Serialize( &buff, &kXPT_UNIT_NPC_SYNC_PACK );
					//g_pData->GetGameUDP()->BroadCast( GetVecUserUIDforSyncPacket(), XPT_UNIT_NPC_SYNC_PACK, (char*)buff.GetData(), buff.GetLength() );

				}

				//클리어 처리

				//{{ 허상형 : [2011/3/22/] //	월드 미션
				if( NULL != g_pData->GetDungeonRoom() && g_pData->GetDungeonRoom()->IsDefenceDungeon( g_pData->GetDungeonRoom()->GetDungeonID() ) == false )
				//}} 허상형 : [2011/3/22/] //	월드 미션
				{
					// 디펜스 던전인 경우에는 Clear가 뜨는데.. 이거 분리해야 할듯...
					SetClearEffect( 1.0f );
				}

				//{{ dmlee 2008.06.03 보스, 중간 보스만 죽을 때 슬로우 모션
				CX2GUNPC* pLastDyingNPC = (CX2GUNPC*) pLastUnit;
				if( false == pLastDyingNPC->GetShowBossName() &&
					false == pLastDyingNPC->GetShowSubBossName() &&
					false == pLastDyingNPC->GetHasBossGage() )
				{
					// 이런 애들은 슬로우모션 안함
					bLastKillSlowMotion = false;
				}

				// 헤니르도 분리하는게 좋을듯...
				// HENIR 던전인 경우에 보스를 죽인걸로 체크되면 EGS_PLAY_TIME_OUT_NOT가 오면 게임이 안 끝나기 때문에.
				if( CX2Dungeon::IsHenirDungeon( m_pDungeon->GetDungeonData()->m_DungeonID ) )
				{
					// 헤니르 던전이 아닌 경우 보스게이지 있는 애가 죽으면
					if( true == pLastDyingNPC->GetHasBossGage() )
					{
						m_bKilledBoss = true;
					}
				}

				if( true == bLastKillSlowMotion )
				//}} dmlee 2008.06.03 보스, 중간 보스만 죽을 때 슬로우 모션
				{
					// 이것도.. 분리 하는게 좋을텐뎅...
					if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() )
					{
						SetTimerFocusUnit( GetMyUnit(), 1.0f );
					}
					else
					{
						// 보스에게 1.5초 동안, 카메라 거리 800
						SetTimerFocusUnit( pLastUnit, 1.5f, 800.f );
					}
				}

				// 보스가 죽은 거면
				if( true == m_bKilledBoss )
				{
					// 피니쉬 이펙트!!
					if( NULL != pLastUnit )
					{
						D3DXVECTOR3 vPos = pLastDyingNPC->GetPos();
						vPos.y += pLastDyingNPC->GetUnitHeight( false ) * 0.5f;

						CKTDGParticleSystem::CParticleEventSequence* pSeq1 =
							GetMinorParticle()->CreateSequence( NULL,  L"Finish_Particle01", vPos, -1, -1 );

						CKTDGParticleSystem::CParticleEventSequence* pSeq2 =
							GetMinorParticle()->CreateSequence( NULL,  L"Finish_Particle02", vPos, -1, -1 );

						if( NULL != pSeq1 && NULL != pSeq2 )
						{
							CX2GUNPC* pNPC = (CX2GUNPC*) pLastUnit;
							pSeq1->SetLandPosition( pNPC->GetLandPosition_LUA().y );
							pSeq2->SetLandPosition( pNPC->GetLandPosition_LUA().y );
						}

						CKTDGParticleSystem::CParticleEventSequence* pSeq3 =
							GetMinorParticle()->CreateSequence( NULL,  L"Finish_Particle03", D3DXVECTOR3(0, 0, 0), -1, -1 );
						if( NULL != pSeq3 )
						{
							pSeq3->CreateNewParticle( D3DXVECTOR3(0, 0, 0) );
							g_pKTDXApp->GetDeviceManager()->PlaySound( L"Boss_Finish.ogg", false, false );
						}

						GetMinorParticle()->CreateSequence( NULL,  L"Dungeon_Finish_FadeIn", D3DXVECTOR3(0, 0, 0), -1, -1 );

						//{{ oasis907 : 김상윤 [2010.4.29] //
						pLastDyingNPC->SetPlayBossKillEffect(false);
						//}}

					}
				}

			}

			// 슬로모션이면 프레임도 느려지게
			if( true == bLastKillSlowMotion )
			{
				g_pKTDXApp->StopFrame( 80 );
				g_pKTDXApp->GetDGManager()->ClearScreen( 5 );
				g_pKTDXApp->SlowFrame( 0.2f, 1.5f );

				/// 슬로우 모션이면, 로딩용 캡처 타이밍을 뒤로 미룬다.
				m_fLastkillShotTime = 0.3f;
			}
			else
				m_fLastkillShotTime = 0.05f;

			m_bLastKillCheck = true;

			return true;
		}
	}

	// oasis907 : 김상윤 [2010.4.29] //
	CX2GUNPC* pLastDyingNPC = NULL;
	if( pLastUnit != NULL && pLastUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
		pLastDyingNPC = (CX2GUNPC*) pLastUnit;

	if(pLastDyingNPC != NULL && pLastDyingNPC->IsPlayBossKillEffect() == true && bIsNotLastKill == true)
	{

		// 서브 보스가 죽을 때도 이런 처리.. 하지만 lastKill은 아니도록...
		if( true == pLastDyingNPC->GetShowBossName() ||
			true == pLastDyingNPC->GetShowSubBossName() ||
			true == pLastDyingNPC->GetHasBossGage() )
		{
			SetTimerFocusUnit( pLastUnit, 1.5f, 800.f );

			D3DXVECTOR3 vPos = pLastDyingNPC->GetPos();
			vPos.y += pLastDyingNPC->GetUnitHeight( false ) * 0.5f;

			CKTDGParticleSystem::CParticleEventSequence* pSeq1 =
				GetMinorParticle()->CreateSequence( NULL,  L"Finish_Particle01", vPos, -1, -1 );

			CKTDGParticleSystem::CParticleEventSequence* pSeq2 =
				GetMinorParticle()->CreateSequence( NULL,  L"Finish_Particle02", vPos, -1, -1 );

			if( NULL != pSeq1 && NULL != pSeq2 )
			{
				CX2GUNPC* pNPC = (CX2GUNPC*) pLastUnit;
				pSeq1->SetLandPosition( pNPC->GetLandPosition_LUA().y );
				pSeq2->SetLandPosition( pNPC->GetLandPosition_LUA().y );
			}

			CKTDGParticleSystem::CParticleEventSequence* pSeq3 =
				GetMinorParticle()->CreateSequence( NULL,  L"Finish_Particle03", D3DXVECTOR3(0, 0, 0), -1, -1 );
			if( NULL != pSeq3 )
			{
				pSeq3->CreateNewParticle( D3DXVECTOR3(0, 0, 0) );
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Boss_Finish.ogg", false, false );
			}

			GetMinorParticle()->CreateSequence( NULL,  L"Dungeon_Finish_FadeIn", D3DXVECTOR3(0, 0, 0), -1, -1 );


			g_pKTDXApp->StopFrame( 80 );
			g_pKTDXApp->GetDGManager()->ClearScreen( 5 );
			g_pKTDXApp->SlowFrame( 0.2f, 1.5f );

			pLastDyingNPC->SetPlayBossKillEffect(false);
		}
	}

	return false;
}


void CX2DungeonGame::ShowClearEffect()
{
	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == false )
	{
		GetMajorParticle()->CreateSequence( NULL,  L"Clear_C", 0,0,0, 1000, 1000, 1, 1 );
		GetMajorParticle()->CreateSequence( NULL,  L"Clear_L", 0,0,0, 1000, 1000, 1, 1 );
		GetMajorParticle()->CreateSequence( NULL,  L"Clear_E", 0,0,0, 1000, 1000, 1, 1 );
		GetMajorParticle()->CreateSequence( NULL,  L"Clear_A", 0,0,0, 1000, 1000, 1, 1 );
		GetMajorParticle()->CreateSequence( NULL,  L"Clear_R", 0,0,0, 1000, 1000, 1, 1 );
	}

	if( NULL != m_pDungeon &&
		NULL != m_pDungeon->GetNowStage() &&
		NULL != m_pDungeon->GetNowStage()->GetNowSubStage() )
	{
		if( true == m_pDungeon->GetNowStage()->GetNowSubStage()->GetIsBossStage() )
		{
			// 무조건 게임이 끝나는 보스스테이지의 경우 (일반적인 던전) 던전엔딩연출을 보스가 죽을 때 시작하고
			if( false == m_pDungeon->GetNowStage()->GetNowSubStage()->GetBossStageHaveManyNextStage() )
			{
				// 보스 스테이지 클리어 이벤트 연출을 시작한다
				CX2StateDungeonGame* pState = (CX2StateDungeonGame*) g_pMain->GetNowState();
				pState->GetDungeonEndingEvent()->m_fElapsedTime_ShowClearEffect = 0.f;
			}
			else // 보스 스테이지에서 게임이 끝나지 않을 수도 있다면 clearsubstage(CT_GAME)이 호출될때 던전엔딩연출이 시작되도록 한다
			{
				// nothing;
			}
		}
	}
}


bool CX2DungeonGame::Handler_EGS_LEAVE_ROOM_ACK( KEGS_LEAVE_ROOM_ACK& kEGS_LEAVE_ROOM_ACK )
{
	return CX2Game::Handler_EGS_LEAVE_ROOM_ACK( kEGS_LEAVE_ROOM_ACK );
}


bool CX2DungeonGame::Handler_EGS_LEAVE_ROOM_NOT( KEGS_LEAVE_ROOM_NOT& kEGS_LEAVE_ROOM_NOT )
{
	CX2Game::Handler_EGS_LEAVE_ROOM_NOT( kEGS_LEAVE_ROOM_NOT );

	return true;
}


bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_CLEAR_REQ( int iClearConditionIndex )
{
	KEGS_DUNGEON_SUB_STAGE_CLEAR_REQ kPacket;
	kPacket.m_iClearConditionIndex = iClearConditionIndex;

	g_pData->GetServerProtocol()->SendPacket( EGS_DUNGEON_SUB_STAGE_CLEAR_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DUNGEON_SUB_STAGE_CLEAR_ACK, 60.0f );

	return true;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_CLEAR_ACK( KEGS_DUNGEON_SUB_STAGE_CLEAR_ACK& kEGS_DUNGEON_SUB_STAGE_CLEAR_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_DUNGEON_SUB_STAGE_CLEAR_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEGS_DUNGEON_SUB_STAGE_CLEAR_ACK.m_iOK ) == true )
		{
			// 서버쪽에 셋팅이 완료 됐으니 실제 처리를 하자
			GetDungeon()->GetNowStage()->GetNowSubStage()->ClearSubStage( kEGS_DUNGEON_SUB_STAGE_CLEAR_ACK.m_iClearType, kEGS_DUNGEON_SUB_STAGE_CLEAR_ACK.m_iStageIndex, kEGS_DUNGEON_SUB_STAGE_CLEAR_ACK.m_iSubStageIndex );
		}
		return g_pMain->IsValidPacket( kEGS_DUNGEON_SUB_STAGE_CLEAR_ACK.m_iOK );
	}
	return false;
}


bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_OPEN_REQ( int subStageNum )
{


	KEGS_DUNGEON_SUB_STAGE_OPEN_REQ kPacket;
	kPacket.m_iSubStageID = subStageNum;
	CX2PacketLog::PrintLog( &kPacket );

	g_pData->GetServerProtocol()->SendPacket( EGS_DUNGEON_SUB_STAGE_OPEN_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DUNGEON_SUB_STAGE_OPEN_ACK, 60.0f );

	return true;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_OPEN_ACK( KEGS_DUNGEON_SUB_STAGE_OPEN_ACK& kEGS_DUNGEON_SUB_STAGE_OPEN_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_DUNGEON_SUB_STAGE_OPEN_ACK ) == true )
	{
		return g_pMain->IsValidPacket( kEGS_DUNGEON_SUB_STAGE_OPEN_ACK.m_iOK );
	}
	return false;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_OPEN_NOT( KEGS_DUNGEON_SUB_STAGE_OPEN_NOT& kEGS_DUNGEON_SUB_STAGE_OPEN_NOT )
{
	SubStageOpen( kEGS_DUNGEON_SUB_STAGE_OPEN_NOT.m_iSubStageID );
	return true;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ( int beforeStage, int nextStage )
{


	KEGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ kPacket;
	kPacket.m_iBeforeStageID	= beforeStage;
	kPacket.m_iNextStageID		= nextStage;

	g_pData->GetServerProtocol()->SendPacket( EGS_DUNGEON_SUB_STAGE_GO_NEXT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK, 60.0f );

#	if !X2_DEV_TOOLS
	// checked
#	endif

	if( m_bCheckSubStageGoNext == false || m_bCheckSubStageGoNext.Verify() == false )
	{
		// 핵유저 의심
		if(g_pData != NULL && g_pData->GetServerProtocol() != NULL )
		{
			if( g_pData != NULL && g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			{
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
				g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
			}
		}

		g_pMain->SendHackMailGameStart(0);
		//g_pKTDXApp->SetFindHacking( true );
	}

	m_bCheckSubStageGoNext = false;

#	if !X2_DEV_TOOLS
#	endif



	return true;


}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK( KEGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK& kEGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK.m_iOK ) == true )
		{
			CX2DungeonSubStage* pCX2DungeonSubStage	= m_pDungeon->GetNowStage()->GetNowSubStage();
			if( pCX2DungeonSubStage != NULL )
				pCX2DungeonSubStage->CreateCurtain();

			m_bCountTimeLeftAfterSubStageOpen = false;
			m_fTimeLeftAfterSubStageOpen = 0.f;


		}
		return g_pMain->IsValidPacket( kEGS_DUNGEON_SUB_STAGE_GO_NEXT_ACK.m_iOK );
	}
	return false;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT( KEGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT& kEGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT )
{
	SubStageLoading( kEGS_DUNGEON_SUB_STAGE_GO_NEXT_ALL_NOT.m_iNextStageID );
	Handler_EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ();
	return true;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ()
{
#if !X2_DEV_TOOLS
#endif


	if( m_pDungeon == NULL || m_pDungeon->GetNowStage() == NULL || m_pDungeon->GetNowStage()->GetNowSubStage() == NULL )
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

		string strMailTitle = ANTI_HACK_STRING_AntiHacking_Process_Dungeon_Is_NULL_SubStageLoadComplete;
		g_pMain->SendHackMail_DamageHistory(strMailTitle.c_str());
		g_pKTDXApp->SetFindHacking( true );
		return true;
	}

	KEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ kPacket;
	kPacket.m_iSubStageID		= m_pDungeon->GetNowStage()->GetNowSubStage()->GetSubStageIndex();

	CX2PacketLog::PrintLog( &kPacket );

	g_pData->GetServerProtocol()->SendPacket( EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK, 60.0f );


#if !X2_DEV_TOOLS
#endif

	return true;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK( KEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK& kEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK ) == true )
	{
		return g_pMain->IsValidPacket( kEGS_DUNGEON_SUB_STAGE_LOAD_COMPLETE_ACK.m_iOK );
	}
	return false;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_SUB_STAGE_START_NOT( KEGS_DUNGEON_SUB_STAGE_START_NOT& kEGS_DUNGEON_SUB_STAGE_START_NOT )
{
	SubStageStart();
	if( m_pDungeon->GetNowStage()->GetStageData()->m_bStartMotion == false )
	{
		if( kEGS_DUNGEON_SUB_STAGE_START_NOT.m_iSubStageID == 0 )
			g_pKTDXApp->GetDGManager()->StartFadeIn();
	}

	m_mapKilledNPCKeyCode.clear();

	if ( m_pWorld != NULL )
	{
		if ( m_pWorld->GetLineMap() != NULL )
			m_pWorld->GetLineMap()->UpdateLineVB();
	}

	g_pKTDXApp->SkipFrame();

	//{{ kimhc // 게임 로딩이 끝날때 까지 ESC 처리 안되게 하는 작업 // 2009-06-08
	if ( m_pDungeon->GetNowStage()->GetNowSubStageIndex() == 0 )
	{
		SetCanUseEscFlag( true );
	}
	//}} kimhc // 게임 로딩이 끝날때 까지 ESC 처리 안되게 하는 작업 // 2009-06-08


	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		g_pData->GetPetManager()->InitPos();
	}





	return true;
}

bool CX2DungeonGame::Handler_EGS_END_GAME_DUNGEON_RESULT_DATA_NOT( KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT& kPacket )
{
	g_pData->ResetDungeonResultInfo( kPacket );

// 	if( g_pData != NULL &&
// 		g_pData->GetUIManager() != NULL &&
// 		g_pData->GetUIManager()->GetUIQuickSlot() != NULL )
// 	{
//         g_pData->GetUIManager()->GetUIQuickSlot()->ResetCoolTime();
	// 	}

	if ( NULL != g_pData && NULL != g_pData->GetPremiumBuffManager() &&
		false == g_pData->GetPremiumBuffManager()->IsComeBackUser())
	{
		g_pData->GetPremiumBuffManager()->SetComeBackUserInParty( false );
	}
	return true;
}

bool CX2DungeonGame::Handler_EGS_STATE_CHANGE_RESULT_NOT( KEGS_STATE_CHANGE_RESULT_NOT& kEGS_STATE_CHANGE_RESULT_NOT )
{
	CX2Game::Handler_EGS_STATE_CHANGE_RESULT_NOT( kEGS_STATE_CHANGE_RESULT_NOT );

	DialogLog( "XGM_STATE_CHANGE, CX2Main::XS_DUNGEON_RESULT, NULL, false" );
	g_pMain->CreateStateChangeDLG( GET_STRING( STR_ID_135 ) );
	g_pKTDXApp->SendGameMessage( XGM_STATE_CHANGE, CX2Main::XS_DUNGEON_RESULT, NULL, false );

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	m_bEscape = false;

	KOGGamePerformanceCheck::GetInstance()->End();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	return true;

}

/*virtual*/ bool CX2DungeonGame::Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT& kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT )
{
	CX2Game::Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT );

	// note!! 부활했는데 몬스터가 다 죽고 없으면 스테이지 클리어, 테스트 필요
	if( 0 == LiveActiveNPCNum() )
	{
		SetClearEffect( 1.f );
	}

	// 이벤트 던전에서 킹 나소드 스테이지이고, 클리어했으면 죽은 유저들이 부활될 때 던전 라운지를 킹 나소드 스테이지로 지정해주기 위한 예외처리
	if( NULL != m_pDungeon &&
		NULL != m_pDungeon->GetDungeonData() )
	{
		switch( m_pDungeon->GetDungeonData()->m_DungeonID )
		{
		case SEnum::DI_EVENT_KIDDAY_RUBEN:
		case SEnum::DI_EVENT_KIDDAY_ELDER:
		case SEnum::DI_EVENT_KIDDAY_BESMA:
		case SEnum::DI_EVENT_KIDDAY_ALTERA:

		case SEnum::DI_EVENT_TREE_DAY_ELDER:
		case SEnum::DI_EVENT_TREE_DAY_BESMA:
		case SEnum::DI_EVENT_TREE_DAY_ALTERA:
		case SEnum::DI_EVENT_TREE_DAY_PEITA:
		case SEnum::DI_EVENT_TREE_DAY_VELDER:
		case SEnum::DI_EVENT_TREE_DAY_HAMEL:
			{
				if( NULL != m_pDungeon->GetNowStage() && NULL != m_pDungeon->GetNowStage()->GetStageData() )

				{
					g_pInstanceData->SetDungeonLoungeWorldID( m_pDungeon->GetNowStage()->GetStageData()->m_WorldID );
				}
			} break;

		case SEnum::DI_ELDER_HENIR_SPACE:
// 		case SEnum::DI_BESMA_HENIR_SPACE:
// 		case SEnum::DI_ALTERA_HENIR_SPACE:
// 		case SEnum::DI_FEITA_HENIR_SPACE:
// 		// kimhc // 벨더 헤니르 // 2009-10-27
// 		case SEnum::DI_VELDER_HENIR_SPACE:
// 		case SEnum::DI_HAMEL_HENIR_SPACE:
			{
				if( NULL != m_pDungeon->GetNowStage() &&
					NULL != m_pDungeon->GetNowStage()->GetStageData() &&
					(CX2Dungeon::DUNGEON_MODE) g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode == CX2Dungeon::DM_HENIR_CHALLENGE )
				{
					g_pInstanceData->SetDungeonLoungeWorldID( m_pDungeon->GetNowStage()->GetStageData()->m_WorldID );
				}
			} break;
		default:
			break;
		}
	}



	return true;
}



bool CX2DungeonGame::Handler_EGS_REMAINING_PLAY_TIME_NOT( KEGS_REMAINING_PLAY_TIME_NOT& kEGS_REMAINING_PLAY_TIME_NOT )
{
	CX2Game::Handler_EGS_REMAINING_PLAY_TIME_NOT( kEGS_REMAINING_PLAY_TIME_NOT );

	float   fRemainingPlayTime = kEGS_REMAINING_PLAY_TIME_NOT;

	SetRemainPlayTime( fRemainingPlayTime );

	if( m_pDungeon != NULL
		&& m_pDungeon->GetNowStage() != NULL
		&& m_pDungeon->GetNowStage()->GetNowSubStage() != NULL
		&& m_pDungeon->GetNowStage()->GetNowSubStage()->GetSubStageTime() > 10.0f )
	{
		m_kGameStatistics.GatherPeriodicStatistics();

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		CX2Game::StartTcpPingCheck();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	}

	const float MAGIC_TIME = 60.f;	// 60초 남았을 때 부터 화면이 어두워지기 시작함
	if( fRemainingPlayTime <= MAGIC_TIME )
	{
		const D3DXCOLOR MAGIC_WORLD_COLOR( 0.f, 0.f, 0.f, 0.f );
		const D3DXCOLOR MAGIC_UNIT_COLOR( 0.19f, 0.19f, 0.19f, 1.f );

		if( NULL != GetWorld() )
		{
			GetWorld()->FadeOriginWorldColor( MAGIC_WORLD_COLOR, fRemainingPlayTime );
			GetWorld()->FadeLightColor( MAGIC_UNIT_COLOR, fRemainingPlayTime );
		}
	}

	return true;
}

void CX2DungeonGame::TextOutTutorial_LUA( int nMsg, float fTime )
{
	m_bIsThereTutorialMsg = true;
	m_fTimeLeftForTutorialMessage = fTime;
	m_iTutorialMsgFlashCount = 0;


	m_wstrTutorialMsg = GET_STRING( nMsg );

}

/*virtual*/ void CX2DungeonGame::UpdateUnitPointer()
{
	CX2Game::UpdateUnitPointer();
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( pCX2GUNPC != NULL )
		{
			//던전 스크립트에 의한 동작
			if( true == pCX2GUNPC->GetStopAtStartState() )
			{
				if( pCX2GUNPC->GetStartState() == static_cast<int>(pCX2GUNPC->GetGameUnitState()) )
				{
					CanNotInputAndDisalbeNPCAI(true);
				}
				else
				{
					CanNotInputAndDisalbeNPCAI(false);
				}
			}

			//NPC 스크립트에 의한 동작
			if( true == pCX2GUNPC->GetStopAtCurrentState() )
			{//키입력 제한 해제는 NPC의 스테이트 종료시 해제. (C ommonStateEnd() )
				CanNotInputAndDisalbeNPCAI(true);
			}

			if( true == pCX2GUNPC->GetFocusCamera() && pCX2GUNPC->GetStartState() == (int)pCX2GUNPC->GetGameUnitState()
				|| true == pCX2GUNPC->GetFocusCameraAtCurrentState() )
			{
				if( true == pCX2GUNPC->GetApplyCinematicUI() && true == g_pMain->IsInheritStateMenu())
				{
					CX2StateMenu* pStateMenu = static_cast<CX2StateMenu*>( g_pMain->GetNowState() );
					if( NULL != pStateMenu )
					{	//포커스 카메라 동작 시에 유저 UI도 같이 숨기도록 설정
						pStateMenu->SetShowUI(false);
					}
				}
				m_optrFocusUnit = pCX2GUNPC;
				ResetFocusUnit();
				return;
			}
			else
			{
				CX2StateMenu* pStateMenu = static_cast<CX2StateMenu*>( g_pMain->GetNowState() );
				if( NULL != pStateMenu )
				{
					pStateMenu->SetShowUI(true);
				}
			}

			//{{ kimhc // 2010.8.7 // 무조건 NPC에게 카메라가 가도록 하는 기능
			if( true == pCX2GUNPC->GetFocusCameraForce() )
			{
				m_optrFocusUnit = pCX2GUNPC;
				ResetFocusUnit();
				return;
			}
			//}} kimhc // 2010.8.7 // 무조건 NPC에게 카메라가 가도록 하는 기능
			if( m_optrFocusUnit.GetObservable() == pCX2GUNPC && pCX2GUNPC->GetStartState() != (int)pCX2GUNPC->GetGameUnitState() )
			{
				if( GetMyUnit() != NULL )
					m_optrFocusUnit = GetMyUnit();
				else
					m_optrFocusUnit.Reset();
				ResetFocusUnit();
				return;
			}
		}
	}
}
//{{ oasis907 : 김상윤 [2010.5.6] //
bool CX2DungeonGame::Handler_EGS_DUNGEON_EFFECT_REMAINING_TIME_NOT( KEGS_DUNGEON_EFFECT_REMAINING_TIME_NOT& kEGS_DUNGEON_EFFECT_REMAINING_TIME_NOT )
{
	m_fDungeonClearLimitTimeOut = kEGS_DUNGEON_EFFECT_REMAINING_TIME_NOT.m_fRemainingTime;
	m_bShowDungeonClearLimitTimeOut = true;
	m_bDungeonClearLimitTimeOut = false;
	return true;
}

bool CX2DungeonGame::Handler_EGS_DUNGEON_EFFECT_TIME_OUT_NOT( KEGS_DUNGEON_EFFECT_TIME_OUT_NOT& /*kEGS_DUNGEON_EFFECT_TIME_OUT_NOT*/ )
{
	if( NULL != g_pData->GetDungeonRoom() )
	{
		switch(g_pData->GetDungeonRoom()->GetDungeonID())
		{
		case SEnum::DI_ALTERA_SECRET_COMMON:
		case SEnum::DI_ALTERA_SECRET_HELL:
			{
				m_bDungeonClearLimitTimeOut = true;
				m_bShowDungeonClearLimitTimeOut = false;
				CreateDamageDataForDungeonTimeOut();
			}
			break;
		default:
			{
				;
			}
			break;
		}
	}
	return true;
}
//}} oasis907 : 김상윤 [2010.5.6] //


//{{ oasis907 : 김상윤 [2010.4.26] //
void CX2DungeonGame::CreateDamageDataForDungeonTimeOut()
{
	SAFE_DELETE( m_pDamageDataForDungeonTimeOut );

	// 알테라 비밀던전에서 쓰이도록 하드 코딩
	m_pDamageDataForDungeonTimeOut = new CX2DamageManager::DamageData;
	m_pDamageDataForDungeonTimeOut->SimpleInit();
	m_pDamageDataForDungeonTimeOut->reActType		= CX2DamageManager::RT_DUMMY_DAMAGE;
	m_pDamageDataForDungeonTimeOut->reActResult		= CX2DamageManager::RT_DUMMY_DAMAGE;
	m_pDamageDataForDungeonTimeOut->fHitAddMP		= 0.0f;

	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.Init();
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fRate = 1.0f;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fPureRate = 1.0f;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_ExtraDamageType = CX2DamageManager::EDT_POISON;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fTime = 1.f;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_DamagePerSec = 2000.f;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_Accumulation = 0;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fElapsedTimeToEffectDamage = 0.f;

	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_iLevel = -1;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_iAddLevel = 0;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_bIgnoreResist = true;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_bDefenderLevel = false;

	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fDamageRate		= 0.f;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fRunJumpRate	= 0.f;
	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fAnimSpeedRate	= 0.f;

	m_pDamageDataForDungeonTimeOut->m_ExtraDamage.m_fJumpRate	= 0.f;
	return;
}




void CX2DungeonGame::DungeonTimeOutFrameMove( double /*fTime*/, float fElapsedTime )
{
	// oasis907 : 김상윤 [2010.5.6] // 타이머 관련
	if( m_bShowDungeonClearLimitTimeOut == true )
	{
		m_fDungeonClearLimitTimeOut -= fElapsedTime;
		if( m_fDungeonClearLimitTimeOut <= 0.f )
		{
			m_fDungeonClearLimitTimeOut = 0.f;
		}
	}

	if( true == m_bDungeonClearLimitTimeOut && !GetKilledBoss())
	{
		for ( int i = 0; i < GetUserUnitListSize(); i++ )
		{
			CX2GUUser* pGUUser = GetUserUnit(i);
			if ( pGUUser != NULL && CX2GameUnit::GUSI_DIE != pGUUser->GetGameUnitState()  )
			{
				pGUUser->DamageReact_ExtraDamage(m_pDamageDataForDungeonTimeOut, false);
			}
		}

		const D3DXCOLOR MAGIC_WORLD_COLOR( 0.f, 0.f, 0.f, 0.f );
		const D3DXCOLOR MAGIC_UNIT_COLOR( 0.19f, 0.19f, 0.19f, 1.f );

		if( NULL != GetWorld() )
		{
			GetWorld()->FadeOriginWorldColor( MAGIC_WORLD_COLOR, 0.f );
			GetWorld()->FadeLightColor( MAGIC_UNIT_COLOR, 0.f );
		}
	}
	return;
}
//}} oasis907 : 김상윤 [2010.4.26] //

void CX2DungeonGame::CountTimeSubStageFrameMove( double /*fTime*/, float fElapsedTime )
{
	if( true == m_bCountTimeLeftAfterSubStageOpen )
	{
		CX2GUUser* pCX2GUUser = GetMyUnit();
		if( pCX2GUUser != NULL && CX2GameUnit::GUSI_DIE == pCX2GUUser->GetGameUnitState() )
		{
			return;
			//m_bCountTimeLeftAfterSubStageOpen = false;
		}

		m_fTimeLeftAfterSubStageOpen -= fElapsedTime;
		if( m_fTimeLeftAfterSubStageOpen <= 0.0f )
		{
			m_fTimeLeftAfterSubStageOpen = 0.0f;
			if( GetMyUnit() != NULL )
			{
				int crashPercent = RandomNumber( 0, 100 );
				if ( GetMyUnit()->GetNowHp() > 0.f && crashPercent < 2 )
				{
					CX2GageManager::GetInstance()->CrashMyStateGageUI( D3DXVECTOR2( -7, -7 ), D3DXVECTOR2( 7, 7 ), D3DXCOLOR( 1, 0, 0, 1) );
				}
				const float fHpIncrement = -GetMyUnit()->GetMaxHp() * 0.05f * fElapsedTime;
				GetMyUnit()->UpNowHp( fHpIncrement );

			}
		}
	}
}

void CX2DungeonGame::TutorialFrameMove( double /*fTime*/, float fElapsedTime )
{
	if( true == g_pMain->GetIsPlayingTutorial() )
	{
		if( false == m_bShowTutorialUI
			&& GetMyUnit() != NULL
			&& GetMyUnit()->GetWaitStateID() == GetMyUnit()->GetNowStateID() )
		{
			m_bShowTutorialUI = true;
		}



		if( true == m_bShowTutorialUI && true == m_bIsThereTutorialMsg )
		{
			if( m_fTimeLeftForTutorialMessage > 0.f )
			{
				m_fTimeLeftForTutorialMessage -= fElapsedTime;

				if( m_iTutorialMsgFlashCount < 1 )
				{
					m_coTutorialMsg += m_coTutorialMsgIncrement;
					if( m_coTutorialMsg.r > 1.f )
					{
						m_coTutorialMsgIncrement = -m_coTutorialMsgIncrement;
						m_coTutorialMsg = D3DXCOLOR(1,1,1,1);
						m_iTutorialMsgFlashCount += 1;
					}
					else if( m_coTutorialMsg.r < 0.f )
					{
						m_coTutorialMsgIncrement = -m_coTutorialMsgIncrement;
						m_coTutorialMsg = D3DXCOLOR(0,0,0,1);
					}
				}
			}
			else
			{
				if( -1.f != m_fTimeLeftForTutorialMessage )
				{
					m_bIsThereTutorialMsg = false;
					m_wstrTutorialMsg = L"";
				}
			}
		}
	}
}



bool CX2DungeonGame::GetResurrectionOperationCond()
{
	if ( CX2Main::XS_TRAINING_GAME != g_pMain->GetNowStateID() && m_bGameEnd == false )
	{
// 09.04.27 태완 : 특정던전에서 부활 못하게
		if( NULL != g_pData->GetDungeonRoom() )
		{
			switch(g_pData->GetDungeonRoom()->GetDungeonID())
			{
			case SEnum::DI_EVENT_KIDDAY_RUBEN:
			case SEnum::DI_EVENT_KIDDAY_ELDER:
			case SEnum::DI_EVENT_KIDDAY_BESMA:
			case SEnum::DI_EVENT_KIDDAY_ALTERA:
				m_fResurrectionRemainTime = 0.0f;
				return false;
			case SEnum::DI_ELDER_HENIR_SPACE:
// 			case SEnum::DI_BESMA_HENIR_SPACE:
// 			case SEnum::DI_ALTERA_HENIR_SPACE:
// 			case SEnum::DI_FEITA_HENIR_SPACE:
// 				// kimhc // 벨더 헤니르 // 2009-10-27
// 			case SEnum::DI_VELDER_HENIR_SPACE:
// 			case SEnum::DI_HAMEL_HENIR_SPACE:
				{
					if( (CX2Dungeon::DUNGEON_MODE) g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode == CX2Dungeon::DM_HENIR_CHALLENGE )
					{
						m_fResurrectionRemainTime = 0.0f;
						return false;
					}
				} break;
			default:
				break;
			}
		}
		return true;
	}
	else
		return false;
}

bool CX2DungeonGame::DetermineShowDungeonPlayTime()
{

//#ifdef BUBBLE_BOBBLE_TEST
//	if( CX2Room::RT_ARCADE == g_pX2Room->GetRoomType() )
//	{
//		return true;
//	}
	//#endif BUBBLE_BOBBLE_TEST
	//

	if ( g_pData->GetDungeonRoom() == NULL )
		return false;


	// 헤니르 시공 던전이면 던전 진행 시간 표시
	if( true == CX2Dungeon::IsHenirDungeon( g_pData->GetDungeonRoom()->GetDungeonID() ) )
	{
		return true;
	}


	// 던전 시간 제한 퀘스트 가지고 있고, 그 해당 던전이면 던전 진행 시간 표시
	const map<int,CX2QuestManager::QuestInst*> mapUnitQuest = g_pData->GetQuestManager()->GetUnitQuestMap();
	map<int,CX2QuestManager::QuestInst*>::const_iterator cmit;
	for ( cmit = mapUnitQuest.begin(); cmit != mapUnitQuest.end(); cmit++ )
	{
		CX2QuestManager::QuestInst* pQuestInst = cmit->second;
		if ( pQuestInst == NULL )
			continue;

		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( pQuestInst->m_iID );
		if( pQuestTemplet == NULL )
			continue;

		for ( int i = 0; i < (int)pQuestTemplet->m_vecSubQuest.size(); i++ )
		{
			const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( pQuestTemplet->m_vecSubQuest[i] );
			if( NULL == pSubQuestTemplet )
				continue;

			const CX2QuestManager::SubQuestInst* pSubQuestInst = pQuestInst->GetSubQuest( pSubQuestTemplet->m_iID );
			if( NULL == pSubQuestInst )
				continue;

			if ( pSubQuestTemplet->m_eClearType != CX2QuestManager::SQT_DUNGEON_TIME )
				continue;

			if( pSubQuestInst->IsComplete() == true )
				continue;

			if( false == pSubQuestTemplet->m_ClearCondition.m_setDungeonID.empty() )
			{
				std::set<SEnum::DUNGEON_ID>::iterator sit = pSubQuestTemplet->m_ClearCondition.m_setDungeonID.begin();
				for( ; sit != pSubQuestTemplet->m_ClearCondition.m_setDungeonID.end(); ++sit)
				{
					if( ((*sit) - g_pData->GetDungeonManager()->GetDungeonData(*sit)->m_eDifficulty)  == g_pData->GetDungeonRoom()->GetDungeonID() )
					{
						if( CX2Dungeon::DT_SECRET == g_pData->GetDungeonManager()->GetDungeonType( (int) g_pData->GetDungeonRoom()->GetDungeonID() ) )
						{
							if( true == pSubQuestTemplet->m_ClearCondition.m_bUpperDifficulty )
							{
								if( pSubQuestTemplet->m_ClearCondition.m_eDungeonMode <= g_pData->GetDungeonRoom()->GetDungeonMode() &&
									pSubQuestTemplet->m_ClearCondition.m_eDungeonMode != CX2Dungeon::DM_INVALID )
								{
									return true;
								}
							}
							else
							{
								if( pSubQuestTemplet->m_ClearCondition.m_eDungeonMode == g_pData->GetDungeonRoom()->GetDungeonMode() &&
									pSubQuestTemplet->m_ClearCondition.m_eDungeonMode != CX2Dungeon::DM_INVALID )
								{
									return true;
								}
							}
						}
						else
						{
							if( true == pSubQuestTemplet->m_ClearCondition.m_bUpperDifficulty )
							{
								if( g_pData->GetDungeonManager()->GetDungeonData(*sit)->m_eDifficulty <= g_pData->GetDungeonRoom()->GetDifficulty() )
								{
									return true;
								}
							}
							else
							{
								if( g_pData->GetDungeonManager()->GetDungeonData(*sit)->m_eDifficulty == g_pData->GetDungeonRoom()->GetDifficulty() )
								{
									return true;
								}
							}

						}
					}
				}
			}
		}
	}

	return false;
}

CX2DungeonGame::DungeonResultUnitInfo::DungeonResultUnitInfo()
{
    m_UnitUID = 0;
	m_bIsDie = false;

    m_nTotalScore = 0;
    m_nComboScore = 0;
    m_nTechnicalScore = 0;
    m_nDamaged = 0;
    m_nNPCKillNum = 0;
    m_nED = 0;
    m_nEXP  = 0;

	//{{ kimhc // 실시간 엘소드 중 실시간 경험치 및 ED 획득
	m_nOldED	= 0;
	m_nOldEXP	= 0;
	//}} kimhc // 실시간 엘소드 중 실시간 경험치 및 ED 획득

    m_nClearBonusEXP        = 0;
    m_nRankBonusEXP         = 0;
    m_nPartyBonusEXP        = 0;
    m_nMvpBonusEXP          = 0;
    m_nGameBangBonusEXP     = 0;
    m_nPremiumBonusEXP      = 0;
    m_nEventBonusExp        = 0;
	m_nChannelBonusEXP		= 0;

    m_fRankBonus        = 0.0f;
    m_fPartyBonus       = 0.0f;

    m_cComboRank        = CX2DungeonRoom::RT_NONE;
    m_cTechnicalRank    = CX2DungeonRoom::RT_NONE;
    m_cDamagedRank      = CX2DungeonRoom::RT_NONE;
    m_cTimeRank         = CX2DungeonRoom::RT_NONE;
    m_cTotalRank        = CX2DungeonRoom::RT_NONE;

	m_bIsMVPPlayer		= false;

}

UidType CX2DungeonGame::DungeonResultInfo::GetMvpPlayerUID ()
{
	vector< CX2DungeonGame::DungeonResultUnitInfo* >::iterator it = m_DungeonResultUnitInfoList.begin();
	for ( ; it != m_DungeonResultUnitInfoList.end(); ++it )
	{
		if ( (*it)->m_bIsMVPPlayer == true )
		{
			return (*it)->m_UnitUID;
		}
	}
	return 0;
}

int CX2DungeonGame::DungeonResultUnitInfo::GetTotalExp()
{
    int totalBounsExp = 0;

    totalBounsExp = m_nEXP + m_nClearBonusEXP + m_nRankBonusEXP + m_nPartyBonusEXP +
        m_nMvpBonusEXP + m_nPremiumBonusEXP + m_nGameBangBonusEXP + m_nEventBonusExp + m_nChannelBonusEXP;

    return totalBounsExp;
}

//{{ 2009. 7. 1  최육사		아케이드 결과처리와 구분
CX2DungeonGame::DungeonResultInfo::DungeonResultInfo( KEGS_END_GAME_DUNGEON_RESULT_DATA_NOT& kPacket )
{
	m_Sec	= kPacket.m_nSec;
	m_bWin	= kPacket.m_bIsWin;
		//{{ kimhc // 2011-04-18 // 국내, 디펜스 던전
	m_ucNumOfProtectedSmallCrystals = static_cast<unsigned char>( kPacket.m_iProtectedSmallCrystal );
		//}} kimhc // 2011-04-18 // 국내, 디펜스 던전

	for ( int i = 0; i < (int)kPacket.m_vecDungeonUnitInfo.size(); i++ )
	{
		KDungeonUnitResultInfo kDungeonUnitInfo = kPacket.m_vecDungeonUnitInfo[i];

		CX2DungeonGame::DungeonResultUnitInfo* pDungeonResultUnitInfo	= new CX2DungeonGame::DungeonResultUnitInfo();
		pDungeonResultUnitInfo->m_UnitUID			= kDungeonUnitInfo.m_UnitUID;
		pDungeonResultUnitInfo->m_bIsDie			= kDungeonUnitInfo.m_bIsDie;

		pDungeonResultUnitInfo->m_nTotalScore		= kDungeonUnitInfo.m_nTotalScore;
		pDungeonResultUnitInfo->m_nComboScore		= kDungeonUnitInfo.m_nComboScore;
		pDungeonResultUnitInfo->m_nTechnicalScore	= kDungeonUnitInfo.m_nTechnicalScore;
		pDungeonResultUnitInfo->m_nDamaged			= kDungeonUnitInfo.m_nDamaged;
		pDungeonResultUnitInfo->m_nNPCKillNum		= kDungeonUnitInfo.m_nKillNPCNum;
		pDungeonResultUnitInfo->m_nED				= kDungeonUnitInfo.m_nED;
		pDungeonResultUnitInfo->m_nEXP				= kDungeonUnitInfo.m_nEXP;

		//{{ kimhc // 실시간 엘소드 중 실시간 ED 및 경험치 획득
		pDungeonResultUnitInfo->m_nOldED			= kDungeonUnitInfo.m_nOldED	;
		pDungeonResultUnitInfo->m_nOldEXP			= kDungeonUnitInfo.m_nOldEXP;
		//}} kimhc // 실시간 엘소드 중 실시간 ED 및 경험치 획득

		pDungeonResultUnitInfo->m_nClearBonusEXP		= kDungeonUnitInfo.m_nClearBonusEXP;
		pDungeonResultUnitInfo->m_nRankBonusEXP			= kDungeonUnitInfo.m_nRankBonusEXP;
		pDungeonResultUnitInfo->m_nPartyBonusEXP		= kDungeonUnitInfo.m_nPartyBonusEXP;
		pDungeonResultUnitInfo->m_nMvpBonusEXP			= kDungeonUnitInfo.m_nMvpBonusEXP;
		pDungeonResultUnitInfo->m_nGameBangBonusEXP		= kDungeonUnitInfo.m_nGameBangBonusEXP;
		pDungeonResultUnitInfo->m_nPremiumBonusEXP		= kDungeonUnitInfo.m_nPremiumBonusEXP;
        pDungeonResultUnitInfo->m_nEventBonusExp		= kDungeonUnitInfo.m_nEventEXP;

		pDungeonResultUnitInfo->m_nChannelBonusEXP		= kDungeonUnitInfo.m_nChannelBonusEXP;
		pDungeonResultUnitInfo->m_cComboRank		= (CX2DungeonRoom::RANK_TYPE)kDungeonUnitInfo.m_cComboRank;
		pDungeonResultUnitInfo->m_cTechnicalRank	= (CX2DungeonRoom::RANK_TYPE)kDungeonUnitInfo.m_cTechnicalRank;
		pDungeonResultUnitInfo->m_cDamagedRank		= (CX2DungeonRoom::RANK_TYPE)kDungeonUnitInfo.m_cDamagedRank;
		pDungeonResultUnitInfo->m_cTimeRank			= (CX2DungeonRoom::RANK_TYPE)kDungeonUnitInfo.m_cTimeRank;
		pDungeonResultUnitInfo->m_cTotalRank		= (CX2DungeonRoom::RANK_TYPE)kDungeonUnitInfo.m_cTotalRank;
		m_DungeonResultUnitInfoList.push_back( pDungeonResultUnitInfo );
		pDungeonResultUnitInfo->m_bIsMVPPlayer		=	kDungeonUnitInfo.m_bIsMVP;
	}

}

CX2DungeonGame::DungeonResultInfo::~DungeonResultInfo()
{
	for ( int i = 0; i < (int)m_DungeonResultUnitInfoList.size(); i++ )
	{
		CX2DungeonGame::DungeonResultUnitInfo* pDungeonResultUnitInfo = m_DungeonResultUnitInfoList[i];
		SAFE_DELETE( pDungeonResultUnitInfo );
	}
	m_DungeonResultUnitInfoList.clear();
}


void CX2DungeonGame::ShowSubStageOpenGuide()
{
}



void CX2DungeonGame::ClearSubStage( int clearType, int nextStageNum, int nextSubStageNum )
{
	GetDungeon()->GetNowStage()->GetNowSubStage()->ClearSubStage( clearType, nextStageNum, nextSubStageNum );
}

void CX2DungeonGame::CreateTrainingGameUI( CX2State* pNowState )
{
	SAFE_DELETE( m_pTrainingGameUI );
	m_pTrainingGameUI = new CX2TrainingGameUI( pNowState );
}

void CX2DungeonGame::SetStartRealTraining( bool val )
{
	if( CX2Main::XS_TRAINING_GAME == g_pMain->GetNowStateID() )
	{
		if( m_bStartRealTraining != val )
		{
			CX2StateTrainingGame* pState = (CX2StateTrainingGame*) g_pMain->GetNowState();
			if( NULL != pState )
			{
				pState->Handler_EGS_SET_TC_REMAINING_TIME_REQ( val );
			}
		}

		m_bStartRealTraining = val;

	}
}

void CX2DungeonGame::ShowBossName( const WCHAR* wszFileName )
{
	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = GetMinorParticle()->CreateSequence( NULL,  L"BOSS_NAME", D3DXVECTOR3(0,0,0) );
	if( pSeq != NULL )
	{
		pSeq->ChangeTexForce( wszFileName );
		m_hBossName = pSeq->GetHandle();
	}
}

void CX2DungeonGame::ShowBossName_LUA( char* pFileName )
{
	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = GetMinorParticle()->CreateSequence( NULL,  L"BOSS_NAME", D3DXVECTOR3(0,0,0) );
	if( pSeq != NULL )
	{
		wstring texFileName;
		ConvertUtf8ToWCHAR( texFileName, pFileName );
		pSeq->ChangeTexForce( texFileName.c_str() );
	}
}

// 훈련소에서 캐릭터 위치와 몬스터 위치를 원위치로
void CX2DungeonGame::ResetUnitPositionAtTrainingGame()
{
	if( g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
		return;

	CX2GUUser* pGUUser = GetMyUnit();
	if( NULL != pGUUser )
	{
		pGUUser->InitPosition( true, 0 );
		pGUUser->StateChange( pGUUser->GetWaitStateID(), true );
	}


	int iCount = 1;
	for( UINT i=0; i < m_NPCUnitList.size() ; i++ )
	{
		CX2GUNPC* pGUNPC = GetNPCUnit( i );
		if( NULL == pGUNPC || pGUNPC->GetNowHp() <= 0.f ||
			pGUNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE ||
			pGUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_INVISIBLE_DUMMY )
		{
			continue;
		}

		pGUNPC->InitPosition( false, iCount );
		pGUNPC->StateChange( pGUNPC->GetWaitStateID(), true );
		iCount += 1;
	}

}

int	CX2DungeonGame::GetDungeonID()
{
	return (int)m_pDungeon->GetDungeonData()->m_DungeonID;
}

int	CX2DungeonGame::GetDungeonType()
{
	return (int)m_pDungeon->GetDungeonData()->m_eDungeonType;
}









	//{{ 허상형 : [2011/3/4/] //	월드 미션
	float CX2DungeonGame::GetPlayTimeLimit()
	{
		if( m_pDungeonRoom != NULL )
		{
			return m_pDungeonRoom->GetPlayTimeLimit();
		}

		return 0.0f;
	}
	//}} 허상형 : [2011/3/4/] //	월드 미션

	void CX2DungeonGame::CloseDungeonName( float /*fTime*/ )
	{
		m_pDLGTitle->Move(m_pDLGTitle->GetPos(), D3DXCOLOR(1.f, 1.f, 1.f, 0.f), 0.75f, true);
	}


/** @function : IsNearPortalLineMap() const
	@brief : 포탈 라인맵에 내 유닛이 닿았는지를 검사
	@param : void
	@return : bool (닿았으면 true)
*/
bool CX2DungeonGame::IsNearPortalLineMap()
{
	if( NULL != GetMyUnit() && NULL != GetWorld() && NULL != GetWorld()->GetLineMap() )
	{
		CX2GUUser* pMyGUUser = GetMyUnit();

		// 유저가 죽은 상태면 포탈 근처에서 죽었어도 처리 안함
		if ( pMyGUUser->GetGameUnitState() == CX2GameUnit::GUSI_DIE || !( pMyGUUser->GetNowHp()	> 0 ) )
			return false;

		const int iLastTouchedLineMapIndex = pMyGUUser->GetLastTouchLineIndex( true );
		const CKTDGLineMap::LineData* pLastTouchedLineData = GetLineMap()->GetLineData( iLastTouchedLineMapIndex );

		ASSERT( pLastTouchedLineData );
		if ( NULL != pLastTouchedLineData )
		{
			// Portal LineMap 이면 true 리턴
			return ( CKTDGLineMap::LT_POTAL == pLastTouchedLineData->lineType );
		}
		else
		{
			StateLog( L"lastTouchedLineData is wrong!" );
			return false;
		}
	}
	else
	{
		return false;
	}
}


void CX2DungeonGame::ResurrectOtherUser()
{
	if ( m_bGameEnd == false
		&& GetMyUnit() != NULL &&
		m_bIsSendResurrectionReq == false &&
		m_pDLGResurrect == NULL &&
		m_pDLGCheckResurrectByUseCash == NULL)
	{

		UidType uidSelectUserUID = -1;

		if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F2 ) == TRUE )
		{
			CX2GageManager* pGageManager = CX2GageManager::GetInstance();

			if ( NULL != pGageManager )
				uidSelectUserUID = pGageManager->GetPartyGageUIPositionIndexByUserList( 2 );
		}
		else if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F3 ) == TRUE )
		{
			CX2GageManager* pGageManager = CX2GageManager::GetInstance();

			if ( NULL != pGageManager )
				uidSelectUserUID = pGageManager->GetPartyGageUIPositionIndexByUserList( 1 );
		}
		else if ( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_F4 ) == TRUE )
		{
			CX2GageManager* pGageManager = CX2GageManager::GetInstance();

			if ( NULL != pGageManager )
				uidSelectUserUID = pGageManager->GetPartyGageUIPositionIndexByUserList( 0 );
		}

		if ( uidSelectUserUID != - 1 )
		{
			for ( int i = 0; i < GetUserUnitListSize(); i++ )
			{
				CX2GUUser* pUserUnit = GetUserUnit(i);

				if ( NULL != pUserUnit )
				{
					UidType uidNowUserUID = pUserUnit->GetUnitUID();

					if ( uidNowUserUID == uidSelectUserUID && pUserUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
					{
						m_UnitUIDToResurrect = uidNowUserUID;
						wstring wstrResurrectText = L"";

						wstrResurrectText = GET_REPLACED_STRING( ( STR_ID_140, "S", pUserUnit->GetUnit()->GetNickName() ) );
						SAFE_DELETE_DIALOG( m_pDLGResurrect );
						m_pDLGResurrect = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250, 300), wstrResurrectText.c_str(),
							CX2State::SUCM_RESURRECT_OK, g_pMain->GetNowState(),
							CX2State::SUCM_RESURRECT_CANCEL );

						break;
					}
				}
			}
		}

	}
}



bool CX2DungeonGame::Handler_EGS_BAD_ATTITUDE_USER_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast <KSerBuffer*> (lParam);	// lParam 을 KSerBuffer 로
	KEGS_BAD_ATTITUDE_USER_NOT kEvent;
	DeSerialize( pBuff, &kEvent );									// KSerBuffer 를 복원

	CX2GageManager* pGageManager = CX2GageManager::GetInstance();	// GageManager Instance 를 얻어옴
	if ( NULL != pGageManager )
	{
		pGageManager->ChangeButtonType ( kEvent.m_iUserUID );		// Button Type 을 변경
	}
	return true;
}

bool CX2DungeonGame::Send_EGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT ( const int iStageIndex_, const int iSubStageIndex_ )
{
	if  (  NULL != m_pDungeon
		&& NULL != m_pDungeon->GetNowStage()
		&& NULL != m_pDungeon->GetNowStage()->GetNowSubStage() )
	{
		KEGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT kEvent;

		kEvent.m_iStageID	 = iStageIndex_;			// 이전 스테이지의 Index 를 얻어와서 패킷 데이터에 넣는다.
		kEvent.m_iSubStageID = iSubStageIndex_;	// 이전 서브 스테이지의 Index 를 얻어와서 패킷 데이터에 넣는다.


		if ( NULL != g_pData )
		{
			CX2DungeonRoom * pDungeonRoom = g_pData->GetDungeonRoom();	// 던전 방 데이터를 받아옴
			if ( NULL != pDungeonRoom )
			{
				if ( false == pDungeonRoom->IsDefenceDungeon( m_pDungeon->GetDungeonData()->m_DungeonID ) )	// Defence 던전이 아닐 경우
				{
					m_pDungeon->SetBeforeStageIndexUsingPacket ( m_pDungeon->GetNowStageIndex() );						// 지금 스테이지의 Index 를 얻어와서 저장한다.
					m_pDungeon->SetBeforeSubStageIndexUsingPacket( m_pDungeon->GetNowStage()->GetNowSubStageIndex() );	// 지금 서브 스테이지의 Index 를 얻어와서 저장한다.
				}

				int iComboScore = 0;				// 현재 서브 스테이지의 콤보 스코어
				CX2GUUser * pMyUnit = GetMyUnit ();	// 콤보 매니저를 불러오기 위해
				if ( NULL != pMyUnit )
				{
					CX2ComboManager * pComboManager = pMyUnit->GetComboManager();		// 콤보 매니저를 얻어온다.
					if ( NULL != pComboManager )
					{
						iComboScore = pComboManager->GetSubStageComboScore();			// 콤보 매니저로부터 현재 서브 스테이지의 콤보 점수를 얻어온다.
						pComboManager->ResetSubStageComboScore ();
					}
				}

				const int iTechScore = GetSubStageTechPoint();	// 현재 서브 스테이지의 테크닉 점수를 얻어온다.
				ResetSubStageTechPoint();


				kEvent.m_iComboScore = iComboScore;
				kEvent.m_iTechScore = iTechScore;		// 패킷 데이터에 넣는다.
				g_pData->GetServerProtocol()->SendPacket( EGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT, kEvent );
			}
		}
	}
	return true;
}

/*virtual */bool CX2DungeonGame::Send_EGS_DEFENCE_DUNGEON_WAVE_ID_NOT ( const int iDungeonWaveID_ )
{
	if( 0 == iDungeonWaveID_ )
	{
		if( g_pMain->IsInheritStateMenu() )
		{
			CX2StateMenu* pStateMenu = (CX2StateMenu*) g_pMain->GetNowState();
			if( pStateMenu != NULL )
			{
				CX2State::MouseClickUI::MOUSE_CLICK_STYLE arrowStyle = CX2State::MouseClickUI::MCS_FROM_RIGHT_TOP_NORMAL;
				D3DXVECTOR2 vPos = D3DXVECTOR2(107, 763);
				pStateMenu->CreateMouseClickUI( (CKTDXStage*) pStateMenu, vPos, 0.5f, 1000, arrowStyle );
			}
		}
	}

	if ( g_pX2Room->GetUserNum() > 1 )			// 파티 중일 때
	{
		if( iDungeonWaveID_ != m_iDefenceDungeonWaveID )
		{
			if ( iDungeonWaveID_ > 0 )
			{
				Send_EGS_BAD_ATTITUDE_USER_CHECK_INFO_NOT ( 0, iDungeonWaveID_ );
			}

			KEGS_DEFENCE_DUNGEON_WAVE_ID_NOT kPacket;
			kPacket.m_iWaveID = iDungeonWaveID_ + 1;			// 1 라운드부터 시작
			m_iDefenceDungeonWaveID = iDungeonWaveID_;

			g_pData->GetServerProtocol()->SendPacket( EGS_DEFENCE_DUNGEON_WAVE_ID_NOT, kPacket );

			return true;
		}
	}
	return false;
}
