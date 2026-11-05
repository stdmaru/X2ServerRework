#include "stdafx.h"
#include "./X2DungeonSubStage.h"



CX2DungeonSubStage::CX2DungeonSubStage( int subStageIndex, SubStageData* pSubStageData )
{
	m_pSubStageData		= pSubStageData;
	m_SubStageIndex		= subStageIndex;
	m_fElapsedTime		= 0.0f;
	m_LoadNPCIndex		= 0;
	m_bStart			= false;
	m_fEndGameDelay		= 0.0f;

	m_bWinWhenTimeOut	= false;
	m_bBossNameWasShown = false;
	m_fElapsedTimeSubStage = 0.f;

	m_bIsBossStage			= false;
	m_bBossStageHaveManyNextStage = false;

	m_bDoneClearSubStage	= false;
	m_bCheckedClearSubStage	= false;
	m_bCheckedClearSubStage_NotHost = false;

	m_fDelayTimeClearSubstage = 0.f;

	m_iNextSecretStageIndex = -1;
	m_iNextNormalStageIndex = -1;
	m_iNextEliteStageIndex = -1;


	if( NULL != pSubStageData )
	{
		bool bDone = false;
		BOOST_TEST_FOREACH( const ClearCondtionData&, clearCond, pSubStageData->m_vecClearCondData )
		{
			BOOST_TEST_FOREACH( const NextStageData&, nextStageData, clearCond.m_vecNextStage )
			{
				if( CT_GAME != nextStageData.m_eClearType )
					continue;

				m_bIsBossStage = true;
				if( clearCond.m_vecNextStage.size() > 1 ||
					pSubStageData->m_vecClearCondData.size() > 1 )
				{
					m_bBossStageHaveManyNextStage = true;
				}

				const CX2Dungeon::DungeonData* pDungeonData = g_pData->GetDungeonManager()->GetDungeonData( g_pData->GetDungeonRoom()->GetDungeonID() );
				if( NULL == pDungeonData || pDungeonData->m_vecEndingSpeechSetMap.size() <= 0 )
					continue;

				int iTableIndex = nextStageData.m_iDungeonEndingSpeechTableIndex;
				if( iTableIndex >= (int) pDungeonData->m_vecEndingSpeechSetMap.size() )
				{
					iTableIndex = 0;
				}

				const CX2Dungeon::DungeonData::EndingSpeechSetMap& mapSpeech = pDungeonData->m_vecEndingSpeechSetMap[ iTableIndex ];
				std::map< CX2Unit::UNIT_TYPE, CX2Dungeon::DungeonData::EndingSpeechSet >::const_iterator it;
				it = mapSpeech.find( g_pData->GetMyUser()->GetSelectUnit()->GetType() );
				if( it != mapSpeech.end() )
				{
					CX2Dungeon::DungeonData::EndingSpeechSet endingSpeechSet = it->second;
					if( false == endingSpeechSet.empty() )
					{
						int iRandomSpeechIndex = rand() % (int) endingSpeechSet.size();
						const CX2Dungeon::DungeonData::EndingSpeech& endingSpeech = endingSpeechSet[ iRandomSpeechIndex ];

						CX2StateDungeonGame* pState = (CX2StateDungeonGame*) g_pMain->GetNowState();
						pState->GetDungeonEndingEvent()->m_vecSpeech1 = endingSpeech;

						bDone = true;
						break;
					}
				}
			}

			if( true == bDone )
				break;
		}





		// 비밀 스테이지 입장 대사
		CX2StateDungeonGame* pState = (CX2StateDungeonGame*) g_pMain->GetNowState();
		pState->GetSecretStageEnteringEvent()->m_vecSpeech1 = pSubStageData->m_SecretStageEnteringSpeech;
		pState->GetSecretStageEnteringEvent()->m_eBossNPCID = pSubStageData->m_eSecretStageNPCID;


	}

	m_hBossDanger = INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hBossDangerAdd = INVALID_PARTICLE_SEQUENCE_HANDLE;

}

CX2DungeonSubStage::~CX2DungeonSubStage(void)
{
	DeleteAllNPCUnit();
	DeleteCurtain();

	g_pX2Game->ClearSpirit();

	DeleteNPCBySubStageEnd();

	m_pSubStageData = NULL;

	if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hBossDanger )
		g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hBossDanger );
	if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hBossDangerAdd )
		g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hBossDangerAdd );
}


void CX2DungeonSubStage::OnFrameMove( double fTime, float fElapsedTime )
{
	m_fElapsedTime = fElapsedTime;


	CurtainFrameMove( fTime, fElapsedTime );

	if( NULL == g_pX2Game )
		return;

	if( m_bStart == true )
	{
		m_fElapsedTimeSubStage += fElapsedTime;

		if( true == g_pX2Game->IsHost() )
		{
			CheckIfSubStageCleared();
		}
		else
		{
			CheckIfSubStageCleared_NotHost();
		}

		ShowBossNameAtStart( 0.01f );
		ProcessTrigger();
	}

	if( m_fEndGameDelay > 0.0f )
	{
		m_fEndGameDelay -= fElapsedTime;
		if( m_fEndGameDelay <= 0.f )
		{
			m_fEndGameDelay = 0.f;

			if( g_pX2Game->IsHost() == true )
			{
				switch( g_pMain->GetNowStateID() )
				{
				case CX2Main::XS_DUNGEON_GAME:
					{
						CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();

						if( true == g_pMain->GetIsPlayingTutorial() )
						{
							pCX2StateDungeonGame->EndDungeonGameReq( CX2Room::TN_RED );
						}
						else
						{
							if( true == pCX2StateDungeonGame->GetDungeonEndingEvent()->m_bEndChapter1 )
							{
								CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
								ASSERT( CX2Room::TN_NONE != pDungeonGame->GetWinTeam() );
								pCX2StateDungeonGame->EndDungeonGameReq( (CX2Room::TEAM_NUM) pDungeonGame->GetWinTeam() );
							}
							else
							{
								m_fEndGameDelay = 1.f;
							}
						}
					} break;

				case CX2Main::XS_TRAINING_GAME:
					{
						CX2StateTrainingGame* pStateTrainingGame = (CX2StateTrainingGame*) g_pMain->GetNowState();
						pStateTrainingGame->Handler_EGS_END_TC_GAME_REQ( true );
					} break;
				}
			}
		}
	}
}

void CX2DungeonSubStage::CurtainFrameMove( double fTime, float fElapsedTime )
{
	for( int i = 0; i < (int)m_CurtainList.size(); i++ )
	{
		CX2Curtain* pCX2Curtain = m_CurtainList[i];
		if( pCX2Curtain != NULL )
			pCX2Curtain->OnFrameMove( fTime, fElapsedTime );
	}
}


void CX2DungeonSubStage::ToggleStartLineMapSet( bool bToggle )
{
	g_pX2Game->GetWorld()->GetLineMap()->EnableLineSet( m_pSubStageData->m_StartLineSet, bToggle );
}

void CX2DungeonSubStage::ToggleEndLineMapSet( bool bToggle )
{
	g_pX2Game->GetWorld()->GetLineMap()->EnableLineSet( m_pSubStageData->m_EndLineSet, bToggle );
}

void CX2DungeonSubStage::ToggleMainLineMapSet( bool bToggle )
{
	g_pX2Game->GetWorld()->GetLineMap()->EnableLineSet( m_pSubStageData->m_MainLineSet, bToggle );
}

bool CX2DungeonSubStage::TouchStartLineMapSet( int lineNum )
{
	int lineSet = m_pSubStageData->m_StartLineSet;
	if( lineSet == -1 )
	{
		lineSet = m_pSubStageData->m_MainLineSet;
	}

	if( g_pX2Game->GetWorld() != NULL )
	{
		if( g_pX2Game->GetWorld()->GetLineMap()->GetLineSetIndex( lineNum ) == lineSet )
			return true;
	}

	return false;
}

bool CX2DungeonSubStage::TouchEndLineMapSet( int lineNum )
{
	int lineSet = m_pSubStageData->m_EndLineSet;
	if( lineSet == -1 )
	{
		lineSet = m_pSubStageData->m_MainLineSet;
	}

	if( g_pX2Game->GetWorld() != NULL )
	{
		if( g_pX2Game->GetWorld()->GetLineMap()->GetLineSetIndex( lineNum ) == lineSet )
			return true;
	}

	return false;
}

bool CX2DungeonSubStage::TouchMainLineMapSet( int lineNum )
{
	int lineSet = m_pSubStageData->m_MainLineSet;

	if( g_pX2Game->GetWorld() != NULL )
	{
		if( g_pX2Game->GetWorld()->GetLineMap()->GetLineSetIndex( lineNum ) == lineSet )
			return true;
	}

	return false;
}



bool CX2DungeonSubStage::IsNPCLoadComplete()
{
	if( m_LoadNPCIndex >= (int)m_pSubStageData->m_NPCDataList.size() )
		return true;
	else
		return false;
}

void CX2DungeonSubStage::CreateCurtain()
{
	for( int i = 0; i < (int)m_pSubStageData->m_CurtainDataList.size(); i++ )
	{
		CurtainData* pCurtainData = m_pSubStageData->m_CurtainDataList[i];
		if( pCurtainData == NULL )
			continue;

		CX2Curtain* pCX2Curtain = new CX2Curtain( pCurtainData->m_LineIndex, pCurtainData->m_bAtStart, pCurtainData->m_bLookLeft, m_SubStageIndex );
		m_CurtainList.push_back( pCX2Curtain );
	}
}

void CX2DungeonSubStage::DeleteCurtain()
{
	for( int i = 0; i < (int)m_CurtainList .size(); i++ )
	{
		CX2Curtain* pCX2Curtain = m_CurtainList[i];

		if( pCX2Curtain == NULL )
			continue;

		SAFE_DELETE( pCX2Curtain );
	}

	m_CurtainList.resize(0);
}

void CX2DungeonSubStage::BreakCurtain()
{
	for( int i = 0; i < (int)m_CurtainList.size(); i++ )
	{
		CX2Curtain* pCX2Curtain = m_CurtainList[i];

		if( pCX2Curtain == NULL )
			continue;

		pCX2Curtain->CurtainBreak();
	}
}

void CX2DungeonSubStage::OpenCurtain()
{
	for( int i = 0; i < (int)m_CurtainList.size(); i++ )
	{
		CX2Curtain* pCX2Curtain = m_CurtainList[i];

		if( pCX2Curtain == NULL )
			continue;

		pCX2Curtain->Open();
	}
}



void CX2DungeonSubStage::SwapToGameNPCUnit()
{
	if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
	{
		CX2DungeonGame* pCX2DungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
		if( pCX2DungeonGame == NULL )
		{
			ErrorLog( XEM_ERROR100 );
			return;
		}

		CX2GUNPC* pCX2GUNPC = NULL;
		for( UINT i = 0; i < m_pSubStageData->m_NPCDataList.size(); i++ )
		{
			NPCData* pNPCData = m_pSubStageData->m_NPCDataList[i];
			pCX2GUNPC = CreateGUNPC( pNPCData );

			if ( pCX2GUNPC == NULL || pNPCData == NULL )
			{
				ErrorLog( XEM_ERROR100 );
				continue;
			}

			pCX2GUNPC->SetUID( pNPCData->m_UID );
			pCX2GUNPC->SetUnitIndex( g_pX2Game->GetEmptyNPCIndex() );
			pCX2GUNPC->InitPosition( false, pNPCData->m_StartPos );

			pCX2DungeonGame->SetNpcHardLevel( pCX2GUNPC, pNPCData->m_Level );

			pCX2GUNPC->SetShowObject( true );
            pCX2GUNPC->SetIsRightNow( pNPCData->m_bIsRight );

			pCX2GUNPC->InitFullName();

			pCX2GUNPC->InitNameColor( g_pX2Game->GetMyUnit()->GetUnitLevel() );

			g_pX2Game->AddUnit( (CX2GameUnit*)pCX2GUNPC );
			g_pX2Game->SetNPCUnit( pCX2GUNPC->GetUnitIndex(), pCX2GUNPC );
			g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2GUNPC );
		}
	}

}

void CX2DungeonSubStage::ProcessTrigger()
{
	if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
	{
		for( int i = 0; i < (int)m_pSubStageData->m_TriggerList.size(); i++ )
		{
			Trigger* pTrigger = m_pSubStageData->m_TriggerList[i];
			if( pTrigger->m_bHost == true && g_pX2Game->IsHost() == false )
				continue;

			if( pTrigger->m_bOneTime == true && pTrigger->m_bReacted == true )
				continue;

			if( pTrigger->m_NowInterval >= pTrigger->m_Interval )
			{
				pTrigger->m_NowInterval = 0.0f;

				//string func = "";
				//ConvertWCHARToChar( func, pTrigger->m_ConditionFunc.c_str() );
				LastErrorLog( pTrigger->m_ConditionFunc.c_str() );
				if( lua_tinker::call<bool>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), pTrigger->m_ConditionFunc.c_str(), g_pKTDXApp, static_cast<CX2DungeonGame*>(g_pX2Game), this ) == true )
				{
					//ConvertWCHARToChar( func, pTrigger->m_ReactFunc.c_str() );
					lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), pTrigger->m_ReactFunc.c_str(), g_pKTDXApp, static_cast<CX2DungeonGame*>(g_pX2Game), this );

					pTrigger->m_bReacted = true;
				}
			}
			else
			{
				pTrigger->m_NowInterval += m_fElapsedTime;
			}
		}
	}
}

void CX2DungeonSubStage::PrepareClearSubStage( int iClearConditionIndex )
{
	if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ||
		g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
	{
		CX2DungeonGame* pCX2DungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
		pCX2DungeonGame->Handler_EGS_DUNGEON_SUB_STAGE_CLEAR_REQ( iClearConditionIndex );
	}
}

void CX2DungeonSubStage::ClearSubStage( int clearType, int nextStageNum, int nextSubStageNum )
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif
	if( NULL == g_pX2Game )
		return;

	if( g_pX2Game->IsHost() == false )
		return;


	m_bDoneClearSubStage = true;
	switch( clearType )
	{
		case CT_SUB_STAGE:
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ||
					g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					CX2DungeonGame* pCX2DungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
					pCX2DungeonGame->Handler_EGS_DUNGEON_SUB_STAGE_OPEN_REQ( nextSubStageNum );
				}
			}
			break;


		case CT_STAGE:
		case CT_STAGE_FORCE:
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ||
					g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();
					pCX2StateDungeonGame->DungeonStageLoadReq( nextStageNum );
				}
			}
			break;
		case CT_FAILURE:
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
				{
					CX2StateDungeonGame* pCX2StateDungeonGame = static_cast<CX2StateDungeonGame*>( g_pMain->GetNowState() );
					pCX2StateDungeonGame->EndDungeonGameReq( CX2Room::TN_MONSTER );
				}
			} break;

		case CT_GAME:
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ||
					g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
					//{{ 허상형 : [2011/3/22/] //	월드 미션
					//	디펜스던전인경우 이곳으로오면 실패다
					if( NULL != g_pData->GetDungeonRoom() && g_pData->GetDungeonRoom()->IsDefenceDungeon( g_pData->GetDungeonRoom()->GetDungeonID() ) == true )
					{
						CX2StateDungeonGame* pCX2StateDungeonGame = static_cast<CX2StateDungeonGame*>( g_pMain->GetNowState() );


						// 클리어 했을 때, 아군 소환문이 남아있으면
						if ( NULL != g_pX2Game &&
							0 != g_pX2Game->GetNPCUnitNumByNPCID( CX2UnitManager::NUI_GOD_GATE_ALLY ) )
							 // 유저의 승리
							 pCX2StateDungeonGame->EndDungeonGameReq( CX2Room::TN_RED );
						else
							// 엘의 수정이 남아있지 않으면 몬스터의 승리
							pCX2StateDungeonGame->EndDungeonGameReq( CX2Room::TN_MONSTER );

						return;
					}
					else
					{
						pDungeonGame->SetWindTeam( CX2Room::TN_RED );
					}
					//}} 허상형 : [2011/3/22/] //	월드 미션

					// 서버로 KillAllNpc 처리해도 되는지 검사받는다.
					CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();
					pCX2StateDungeonGame->Handler_EGS_DUNGEON_KILLALLNPC_CHECK_REQ();

				}
			}
			break;

		case CT_STAGE_SECRET:
			{
				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
				{
					CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();

					// 파티원들의 HP를 확인해서 일반 스테이지로 갈지, 비밀 스테이지로 진입 시도할지를 결정한다.
					if( NULL != g_pX2Game &&
						//true == g_pX2Game->IsAllUserHPPercentMoreThan( 30.f ) &&
						true == g_pData->GetPartyManager()->DoIHaveParty() &&
						g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberCount() >= 2 )
					{
						pCX2StateDungeonGame->Handler_EGS_DUNGEON_SECRET_STAGE_ENTER_CHECK_REQ();
					}
					else
					{
						pCX2StateDungeonGame->SecretDungeonStageLoadReq( CX2DungeonSubStage::SSP_NORMAL );
					}
				}
			} break;
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
}


void CX2DungeonSubStage::SetMonsterCountForSubStageClearCheck()
{
	if( NULL == m_pSubStageData )
		return;


	for( UINT i=0; i< m_pSubStageData->m_vecClearCondData.size() ; i++ )
	{
		ClearCondtionData& clearCond = m_pSubStageData->m_vecClearCondData[i];
		switch( clearCond.m_eClearCondition )
		{
		case CC_KILL_KEYCODE_NPC:
			{
				for( UINT i=0; i< clearCond.m_vecNPCKeyCode.size(); i++ )
				{
					ClearCondtionData::MonsterCount& monsterCount = clearCond.m_vecNPCKeyCode[i];
					monsterCount.m_iOriginalCount = g_pX2Game->LiveKeyCodeNPCNum( monsterCount.m_iKey );
				}
			} break;

		case CC_KILL_NPC_TYPE:
			{
				for( UINT i=0; i<clearCond.m_vecNPCID.size(); i++ )
				{
					ClearCondtionData::MonsterCount& monsterCount = clearCond.m_vecNPCID[i];
					monsterCount.m_iOriginalCount = g_pX2Game->LiveNPCNumType_LUA( (CX2UnitManager::NPC_UNIT_ID) monsterCount.m_iKey );
				}
			} break;
		}

	}
}

void CX2DungeonSubStage::SetEnableClearCondition( int iIndex, bool bEnable )
{
	if( iIndex < 0 || iIndex > (int) m_pSubStageData->m_vecClearCondData.size()-1 )
	{
		return;
	}

	ClearCondtionData& clearCond = m_pSubStageData->m_vecClearCondData[iIndex];
	clearCond.m_bEnable = bEnable;
}






// 이 함수 코드가 조금 복잡하게 꼬여있으니 수정할 때 주의해주세요~
void CX2DungeonSubStage::CheckIfSubStageCleared()
{
#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_START
#endif
	if( NULL == g_pX2Game )
		return;

	if( NULL == m_pSubStageData )
		return;

	bool bCleared = false;
	int iCompletedClearCondIndex = 0;
	if( true == m_bCheckedClearSubStage )	// m_bCheckedClearSubStage는 아케이드모드의 원숭이 굴러내려오는 것 잡는 스테이지의 예외 처리, CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC를 위한 예외처리로도 사용됨
	{
		bCleared = true;
	}
	else
	{
		for( UINT iClearCondIndex=0; iClearCondIndex< m_pSubStageData->m_vecClearCondData.size(); iClearCondIndex++ )
		{
			ClearCondtionData& clearCond = m_pSubStageData->m_vecClearCondData[iClearCondIndex];
			if( false == clearCond.m_bEnable )
				continue;

			if( true == clearCond.m_bChecked )
				continue;

			if( true == clearCond.m_vecNextStage.empty() )
				continue;


			//CT_STAGE_FORCE는 다른 clearcond을 만족했더라도 계속해서 체크하도록 한다
			if( true == m_bDoneClearSubStage )
			{
				NextStageData& nextStageData = clearCond.m_vecNextStage[0];
				if(	CT_STAGE_FORCE != nextStageData.m_eClearType )
					continue;
			}


			bCleared = true;
			iCompletedClearCondIndex = iClearCondIndex;

			switch( clearCond.m_eClearCondition )
			{
			case CC_KILL_ALL_ACTIVE_NPC:
				{
					if( 0 != g_pX2Game->LiveActiveNPCNum() )
					{
						bCleared = false;
					}
				} break;

			case CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC:
				{
					if( 0 != g_pX2Game->LiveActiveNPCNum() )
					{
						bCleared = false;
					}
					else
					{
						if( false == clearCond.m_bCreatedPortalEffect )
						{
							clearCond.m_bCreatedPortalEffect = true;

							if( NULL != g_pX2Game &&
								NULL != g_pX2Game->GetWorld() &&
								NULL != g_pX2Game->GetLineMap() )
							{
								BOOST_TEST_FOREACH( int, portalLineIndex, clearCond.m_vecPortalLineIndex )
								{
									const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->GetLineData( portalLineIndex );
									if( NULL != pLineData )
									{
										D3DXVECTOR3 vCenterPos = ( pLineData->startPos + pLineData->endPos ) * 0.5f;

										CX2WorldObjectParticle* pParticle = g_pX2Game->GetWorld()->CreateObjectParticle( g_pX2Game->GetMajorParticle(), "Peita_Teleport_MagicSquare01" );
										if( NULL != pParticle )
										{
											pParticle->SetParticlePos( vCenterPos + D3DXVECTOR3(0, 3, 0 ) );
											pParticle->SetLayer( XL_EFFECT_0 );
										}
									}
								}
							}
						}

						bool bThereIsUserUnitOnLine = false;
						for( int i=0; i<(int) clearCond.m_vecPortalLineIndex.size(); i++ )
						{
							if( true == g_pX2Game->IsThereAnyUserUnitOnLine( clearCond.m_vecPortalLineIndex[i] ) )
							{
								bThereIsUserUnitOnLine = true;
								break;
							}
						}

						if( true == bThereIsUserUnitOnLine )
						{
							bCleared = true;
							g_pX2Game->CreateStageLoadingTeleportEffectForAllUser( true );
						}
						else
						{
							bCleared = false;
						}
					}
				} break;


			case CC_KILL_KEYCODE_NPC:
				{
					for( UINT i=0; i< clearCond.m_vecNPCKeyCode.size(); i++ )
					{
						ClearCondtionData::MonsterCount& monsterCount = clearCond.m_vecNPCKeyCode[i];

						if( 0 == monsterCount.m_iOriginalCount ||
							0 != g_pX2Game->LiveKeyCodeNPCNum( monsterCount.m_iKey ) )
						{
							bCleared = false;
							break;
						}
					}
				} break;

			case CC_KILL_KEYCODE_NPC_ANY:
				{
					for( UINT i=0; i< clearCond.m_vecNPCKeyCode.size(); i++ )
					{
						ClearCondtionData::MonsterCount& monsterCount = clearCond.m_vecNPCKeyCode[i];

						if( false == g_pX2Game->IsKillNPCKeyCode_LUA( monsterCount.m_iKey ) )
						{
							bCleared = false;
							break;
						}
					}
				} break;

			case CC_KILL_KEYCODE_NPC_COUNT:
				{
					for( UINT i=0; i< clearCond.m_vecNPCKeyCode.size(); i++ )
					{
						ClearCondtionData::MonsterCount& monsterCount = clearCond.m_vecNPCKeyCode[i];

						if( monsterCount.m_iObjectiveCount > g_pX2Game->GetKilledKeyCodeNPCCount( monsterCount.m_iKey ) )
						{
							bCleared = false;
							break;
						}
					}

					if( true == bCleared )
					{
						if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
						{
							CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);
							pDungeonGame->SetClearEffect( 1.f );
							pDungeonGame->SetLastKillCheck( true );
							pDungeonGame->SetLastkillShotTime( 0.05f );
						}
					}

				} break;

			case CC_KILL_NPC_TYPE:
				{
					for( UINT i=0; i<clearCond.m_vecNPCID.size(); i++ )
					{
						ClearCondtionData::MonsterCount& monsterCount = clearCond.m_vecNPCID[i];

						if( 0 == monsterCount.m_iOriginalCount ||
							0 != g_pX2Game->LiveNPCNumType_LUA( (CX2UnitManager::NPC_UNIT_ID) monsterCount.m_iKey ) )
						{
							bCleared = false;
							break;
						}
					}
				} break;
			case CC_TIME:
				{
					if( clearCond.m_fClearTime > 0.f && clearCond.m_fClearTime <= m_fElapsedTimeSubStage )
						bCleared = true;
					else
						bCleared = false;
				} break;
			} /// switch

			if( true == bCleared )
			{
				clearCond.m_bChecked = true;
				m_bCheckedClearSubStage = true;
				m_TimerDelayClearSubstage.restart();
				m_fDelayTimeClearSubstage = clearCond.m_fDelayTimeClearSubStage;

				break;
			} /// if
		} /// for
	} /// else

	if( true == bCleared &&
		( false == m_bCheckedClearSubStage || !(m_TimerDelayClearSubstage.elapsed() < m_fDelayTimeClearSubstage) ) )
	{
		ChangeLineTypeAfterStageClear();

		m_bCheckedClearSubStage = false;

		PrepareClearSubStage( iCompletedClearCondIndex );
	}

#if !X2_DEV_TOOLS
	ELSWORD_VIRTUALIZER_END
#endif
}






// 방장 아닌 클라이언트가 이펙트 표시를 위해서 던전 스테이지 클리어 처리 되었는지 체크하도록 하기 위해서
void CX2DungeonSubStage::CheckIfSubStageCleared_NotHost()
{
	if( NULL == m_pSubStageData )
		return;


	if( true == m_bCheckedClearSubStage_NotHost )
		return;




	BOOST_TEST_FOREACH( const ClearCondtionData&, clearCond, m_pSubStageData->m_vecClearCondData )
	{
		if( false == clearCond.m_bEnable )
			continue;

		if( true == clearCond.m_bChecked )
			continue;

		if( true == clearCond.m_vecNextStage.empty() )
			continue;


		bool bCleared = true;
		switch( clearCond.m_eClearCondition )
		{
		case CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC:
			{
				if( 0 != g_pX2Game->LiveActiveNPCNum() )
				{
					bCleared = false;
				}
				else
				{
					bool bThereIsUserUnitOnLine = false;
					for( int i=0; i<(int) clearCond.m_vecPortalLineIndex.size(); i++ )
					{
						if( true == g_pX2Game->IsThereAnyUserUnitOnLine( clearCond.m_vecPortalLineIndex[i] ) )
						{
							bThereIsUserUnitOnLine = true;
							break;
						}
					}

					if( false == bThereIsUserUnitOnLine )
					{
						bCleared = false;
					}
					else
					{
						g_pX2Game->CreateStageLoadingTeleportEffectForAllUser( true );
					}
				}
			} break;
		}

		if( true == bCleared )
		{
			m_bCheckedClearSubStage_NotHost = true;
			break;
		}
	}

}




void CX2DungeonSubStage::DeleteAllNPCUnit()
{
	for( int i = 0; i < (int)m_pSubStageData->m_NPCDataList.size(); i++ )
	{
		NPCData* pNPCData = m_pSubStageData->m_NPCDataList[i];
		CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnitByUID( pNPCData->m_UID );
		if( NULL != pCX2GUNPC )
		{

			if ( true == pCX2GUNPC->GetIsRemainingNpcWhenStageCleard() )
			{
				continue;
			}

			pCX2GUNPC->ResetGameUnitWhoAttackedMe();
			g_pX2Game->DeleteNPCUnitByUID( pNPCData->m_UID );
		}
	}

    if ( g_pKTDXApp->GetLuaBinder() != NULL )
        g_pKTDXApp->GetLuaBinder()->FlushJitCompileMode();

}

/*static*/
void CX2DungeonSubStage::DeleteAllNPCUnit( const vector<NPCData*>& vecNPCData_ )
{
	ASSERT( g_pX2Game != NULL && vecNPCData_.empty() == false );

	if ( g_pX2Game == NULL || vecNPCData_.empty() == true )
		return;

	CX2GUNPC* pCX2GUNPC = NULL;
	BOOST_TEST_FOREACH( const NPCData*, pNpcData, vecNPCData_ )
	{
		pCX2GUNPC = g_pX2Game->GetNPCUnitByUID( pNpcData->m_UID );
		if( NULL != pCX2GUNPC )
		{
			if ( true == pCX2GUNPC->GetIsRemainingNpcWhenStageCleard() )
			{
				continue;
			}
			pCX2GUNPC->ResetGameUnitWhoAttackedMe();
			g_pX2Game->DeleteNPCUnitByUID( pNpcData->m_UID );
		}
	}

    if ( g_pKTDXApp->GetLuaBinder() != NULL )
        g_pKTDXApp->GetLuaBinder()->FlushJitCompileMode();

}

void CX2DungeonSubStage::ShowBossNameAtStart( float fElapsedTimeSubStage )
{
	if( NULL == g_pX2Game )
		return;

	if( false == m_bBossNameWasShown && m_fElapsedTimeSubStage > fElapsedTimeSubStage )
	{
		if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
		{
			CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);

			for( int i = 0; i < (int)g_pX2Game->GetNPCUnitListSize(); i++ )
			{
				CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnit(i);
				if( pCX2GUNPC == NULL )
					continue;

				//보스 이름 보여 주기 위한 대기 시간 추가
				if( true == pCX2GUNPC->GetShowBossName()
					&& m_fElapsedTimeSubStage > pCX2GUNPC->GetShowBossNameDelayTime() )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq =
						g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"BOSS_DANGER",		0, 0, 0, 1000, 1000, -1, 1, -1 );

					if ( NULL != pSeq )
						m_hBossDanger = pSeq->GetHandle();

					pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"BOSS_DANGER_ADD",	0, 0, 0, 1000, 1000, -1, 1, -1 );

					if ( NULL != pSeq )
						m_hBossDangerAdd = pSeq->GetHandle();

					if( pCX2GUNPC->GetBossNameTexture().length() != 0 )
					{
						pDungeonGame->ShowBossName( pCX2GUNPC->GetBossNameTexture().c_str() );
					}

					m_bBossNameWasShown = true;
				}
				else if( true == pCX2GUNPC->GetShowSubBossName() )
				{
					CKTDGParticleSystem::CParticleEventSequence* pSeq =
						g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"BOSS_DANGER",		0, 0, 0, 1000, 1000, -1, 1, -1 );
					if ( NULL != pSeq )
						m_hBossDanger = pSeq->GetHandle();

					pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( NULL,  L"BOSS_DANGER_ADD",	0, 0, 0, 1000, 1000, -1, 1, -1 );

					if ( NULL != pSeq )
						m_hBossDangerAdd = pSeq->GetHandle();

					m_bBossNameWasShown = true;
				}

			}
		}
	}
}

void CX2DungeonSubStage::ClearDungeonGame()
{
	if( NULL != g_pX2Game && CX2Game::GT_DUNGEON == g_pX2Game->GetGameType() )
	{
		CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(g_pX2Game);

		m_fEndGameDelay = 7.0f;

		// 던전 라운지에서 보스 스테이지 월드를 사용하기 위해 마지막 스테이지를 기록해둔다
		ASSERT( pDungeonGame->GetDungeon()->GetNowStage()->GetStageData() != NULL );
		g_pInstanceData->SetDungeonLoungeWorldID( pDungeonGame->GetDungeon()->GetNowStage()->GetStageData()->m_WorldID );


		// 던전엔딩연출이 시작되지 않았다면 시작한다
		CX2StateDungeonGame* pCX2StateDungeonGame = (CX2StateDungeonGame*)g_pMain->GetNowState();
		if( false == pCX2StateDungeonGame->GetDungeonEndingEvent()->m_bStarted )
		{
			pCX2StateDungeonGame->GetDungeonEndingEvent()->Start();
		}
	}

}

void CX2DungeonSubStage::DeleteNPCBySubStageEnd()
{
	for( int i = 0; i < (int)g_pX2Game->GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = g_pX2Game->GetNPCUnit(i);
		if( pCX2GUNPC == NULL )
			continue;

		if( true == pCX2GUNPC->GetDeleteNPCBySubStageEnd() )
		{
			pCX2GUNPC->SetNowHp( 0.f );
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////
bool CX2DungeonSubStage::SubStageData::LoadData( KLuaManager& luaManager, bool bIsNpcLoad )
{
	//이 서브 스테이지에 사용할 라인 그룹
	LUA_GET_VALUE( luaManager, "MAIN_LINE_SET",		m_MainLineSet,		0 );
	LUA_GET_VALUE( luaManager, "START_LINE_SET",	m_StartLineSet,		-1 );
	LUA_GET_VALUE( luaManager, "END_LINE_SET",		m_EndLineSet,		-1 );
	LUA_GET_VALUE_ENUM( luaManager, "GO_TYPE",		m_GoType,			CX2DungeonSubStage::GO_TYPE, CX2DungeonSubStage::GT_NONE );

//{{ kimhc // 2010.8.10 // 특정 몬스터의 위치 값을 부활 위치로 사용
	LUA_GET_VALUE( luaManager, "USE_POS_OF_MONSTER_FOR_REBIRTH",		m_bUsePosOfMonsterForRebirth,		false	);
	LUA_GET_VALUE( luaManager, "KEY_CODE_OF_MONSTER_FOR_REBIRTH",		m_iKeyCodeOfMonsterForRebirth,		-1		);
//}} kimhc // 2010.8.10 // 특정 몬스터의 위치 값을 부활 위치로 사용

	int index = 0;
	while( luaManager.BeginTable( "KEYCODE_N_START_STATE", index ) == true )
	{
		int keyCode = 0;
		string startState;

		LUA_GET_VALUE( luaManager, 1, keyCode,	-1	);
		LUA_GET_VALUE_UTF8( luaManager, 2, startState,	""		);

		m_mapKeyCodeNStartState.insert( std::make_pair( keyCode, startState ) );

		index++;
		luaManager.EndTable();
	}

	int iClearCondIndex = 0;
	int iValue = 0;
	while( true == luaManager.BeginTable( "CLEAR_COND", iClearCondIndex ) )
	{
		ClearCondtionData clearCond;

		LUA_GET_VALUE( luaManager, 1, iValue, (int)CC_KILL_ALL_ACTIVE_NPC );
		clearCond.m_eClearCondition = (CLEAR_CONDITION) iValue;

		LUA_GET_VALUE( luaManager, 2, clearCond.m_bEnable, true );

		LUA_GET_VALUE( luaManager, "DELAY_TIME_CLEAR_SUB_STAGE", clearCond.m_fDelayTimeClearSubStage, 0.0f );

		int iNextStageIndex = 0;
		while( true == luaManager.BeginTable( "NEXT_STAGE", iNextStageIndex ) )
		{
			NextStageData nextStageData;
			LUA_GET_VALUE( luaManager, 1, iValue, (int)CT_GAME );
			nextStageData.m_eClearType = (CLEAR_TYPE) iValue;

			LUA_GET_VALUE( luaManager, 2, nextStageData.m_iStageIndex, -1 );
			LUA_GET_VALUE( luaManager, 3, nextStageData.m_iSubStageIndex, -1 );
			LUA_GET_VALUE( luaManager, 4, nextStageData.m_iRate, 100 );
			LUA_GET_VALUE( luaManager, 5, nextStageData.m_iDungeonEndingSpeechTableIndex, 0 );


			clearCond.m_vecNextStage.push_back( nextStageData );

			iNextStageIndex++;
			luaManager.EndTable(); // NEXT_STAGE(i)
		}


		switch( clearCond.m_eClearCondition )
		{
		case CC_KILL_ALL_ACTIVE_NPC:
			break;

		case CC_PORTAL_AFTER_KILL_ALL_ACTIVE_NPC:
			{
				if( true == luaManager.BeginTable( "PORTAL_LINE_INDEX" ) )
				{
					for( int i=1; ;i++ )
					{
						int iLineIndex = 0;
						LUA_GET_VALUE( luaManager, i, iLineIndex, -1 );
						if( -1 == iLineIndex )
							break;

						clearCond.m_vecPortalLineIndex.push_back( iLineIndex );

					}
					luaManager.EndTable(); // PORTAL_LINE_INDEX
				}
			} break;


		case CC_KILL_KEYCODE_NPC:
			{
				if( true == luaManager.BeginTable( "NPC_KEYCODE" ) )
				{
					for( int i=1; ;i++ )
					{
						ClearCondtionData::MonsterCount monsterCount;

						LUA_GET_VALUE( luaManager, i, monsterCount.m_iKey, -1 );
						if( -1 == monsterCount.m_iKey )
							break;

						clearCond.m_vecNPCKeyCode.push_back( monsterCount );
					}
					luaManager.EndTable(); // NPC_KEYCODE
				}
			} break;

		case CC_KILL_KEYCODE_NPC_ANY:
			{
				if( true == luaManager.BeginTable( "NPC_KEYCODE" ) )
				{
					for( int i=1; ;i++ )
					{
						ClearCondtionData::MonsterCount monsterCount;

						LUA_GET_VALUE( luaManager, i, monsterCount.m_iKey, -1 );
						if( -1 == monsterCount.m_iKey )
							break;

						clearCond.m_vecNPCKeyCode.push_back( monsterCount );
					}
					luaManager.EndTable(); // NPC_KEYCODE
				}
			} break;

		case CC_KILL_KEYCODE_NPC_COUNT:
			{
				if( true == luaManager.BeginTable( "NPC_KEYCODE_AND_COUNT" ) )
				{
					for( int i=1; ;i++ )
					{
						ClearCondtionData::MonsterCount monsterCount;
						LUA_GET_VALUE( luaManager, i, monsterCount.m_iKey, -1 );
						if( -1 == monsterCount.m_iKey )
							break;

						i++;
						LUA_GET_VALUE( luaManager, i, monsterCount.m_iObjectiveCount, -1 );
						if( -1 == monsterCount.m_iObjectiveCount )
							break;

						clearCond.m_vecNPCKeyCode.push_back( monsterCount );
					}
					luaManager.EndTable(); // NPC_KEYCODE
				}

			} break;

		case CC_KILL_NPC_TYPE:
			{
				ClearCondtionData::MonsterCount monsterCount;

				if( true == luaManager.BeginTable( "NPC_ID" ) )
				{
					for( int i=1; ;i++ )
					{
						LUA_GET_VALUE( luaManager, i, monsterCount.m_iKey, -1 );
						if( -1 == monsterCount.m_iKey )
							break;

						clearCond.m_vecNPCID.push_back( monsterCount );
					}
					luaManager.EndTable(); // NPC_ID
				}
			} break;
		case CC_TIME:
			{
				LUA_GET_VALUE( luaManager, "CLEAR_TIME",	clearCond.m_fClearTime,		-1.f );
			}
			break;
		}


		m_vecClearCondData.push_back( clearCond );

		iClearCondIndex++;
		luaManager.EndTable(); // CLEAR_COND(i)
	}

	//이 서브 스테이지에 커튼 데이타 로드
	LoadCurtain( luaManager );

	//이 서브 스테이지에 나올 NPC 데이타 로드

	//이 서브 스테이지에 사용할 트리거 리스트
	LoadTrigger( luaManager );



	// 비밀 스테이지가 있는 경우에 비밀 스테이지 진입 대사
	LoadSecretStageEnteringSpeech( luaManager );

	return true;
}

void CX2DungeonSubStage::SubStageData::LoadCurtain( KLuaManager& luaManager )
{
	if( luaManager.BeginTable( "CURTAIN_GROUP" ) == true )
	{
		int curtainIndex = 1;
		while( luaManager.BeginTable( curtainIndex ) == true )
		{
			CurtainData* pCurtainData = new CurtainData();

			LUA_GET_VALUE( luaManager, "LINE_INDEX",		pCurtainData->m_LineIndex,		-1		);
			LUA_GET_VALUE( luaManager, "AT_START",			pCurtainData->m_bAtStart,		false	);
			LUA_GET_VALUE( luaManager, "LOOK_LEFT",		pCurtainData->m_bLookLeft,		true	);

			curtainIndex++;
			m_CurtainDataList.push_back( pCurtainData );
			luaManager.EndTable(); // CURTAIN(j)
		}
		luaManager.EndTable();
	}
}


void CX2DungeonSubStage::SubStageData::FetchNPCData( KLuaManager& luaManager, CX2DungeonSubStage::NPCData* pNPCData )
{
	LUA_GET_VALUE_ENUM( luaManager, "NPC_ID",	 		pNPCData->m_UnitID,			CX2UnitManager::NPC_UNIT_ID, 	CX2UnitManager::NUI_NONE	);
	LUA_GET_VALUE( luaManager, "KEY_CODE", 			pNPCData->m_KeyCode,		0		);
	LUA_GET_VALUE( luaManager, "FOCUS_CAMERA",			pNPCData->m_bFocusCamera,	false	);


	pNPCData->m_StartPos = -1;
	if( true == luaManager.BeginTable( "START_POS" ) )
	{
		int index	= 1;
		int buf		= -1;
		while( luaManager.GetValue( index, buf ) == true )
		{
			if( buf >= 0 )
				pNPCData->m_vecStartPos.push_back( buf );
			index++;
		}

		luaManager.EndTable();
	}

	LUA_GET_VALUE( luaManager, "ACTIVE",				pNPCData->m_bActive,		true	);
	LUA_GET_VALUE( luaManager, "GAGE_BAR", 			pNPCData->m_bShowGage,		true	);

	LUA_GET_VALUE( luaManager, "ADD_POS_X", 			pNPCData->m_AddPos.x,		0.0f	);
	LUA_GET_VALUE( luaManager, "ADD_POS_Y", 			pNPCData->m_AddPos.y,		0.0f	);
	LUA_GET_VALUE( luaManager, "ADD_POS_Z", 			pNPCData->m_AddPos.z,		0.0f	);

	LUA_GET_VALUE( luaManager, "RATE", 				pNPCData->m_Rate,			100		);
	LUA_GET_VALUE( luaManager, "LEVEL", 				pNPCData->m_Level,			0		);

	LUA_GET_VALUE( luaManager, "HAVE_BOSS_GAGE",		pNPCData->m_bHasBossGage,	false	);
	LUA_GET_VALUE( luaManager, "SHOW_BOSS_NAME",		pNPCData->m_bShowBossName,	false	);
	LUA_GET_VALUE( luaManager, "SHOW_SUB_BOSS_NAME",	pNPCData->m_bShowSubBossName,	false	);

	LUA_GET_VALUE( luaManager, "SIEGE_MODE",			pNPCData->m_bSiegeMode,		false	);
	//{{ 2007. 10. 26  최육사
	LUA_GET_VALUE( luaManager, "IS_RIGHT",				pNPCData->m_bIsRight,		false	);
	//}}
	LUA_GET_VALUE( luaManager, "NO_DROP",				pNPCData->m_bNoDrop,		false	);


	LUA_GET_VALUE( luaManager, "UNIT_SCALE",			pNPCData->m_fUnitScale,		-1.f	);

	//{{ JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
	LUA_GET_VALUE( luaManager, "BOSS_GAUGE_HP_LINES",	pNPCData->m_usBossGaugeHPLines,	1	);
	//}} JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현

	//{{ 2010. 03. 29  손영준	몬스터 팀 설정
	LUA_GET_VALUE( luaManager, "ALLY_NPC",				pNPCData->m_bAllyNpc,		false );

	LUA_GET_VALUE( luaManager, "SHOW_BOSS_NAME_DELAY_TIME", pNPCData->m_fShowBossNameDelayTime,	-1.f	);
	if( -1.f != pNPCData->m_fShowBossNameDelayTime )
	{
		pNPCData->m_bShowBossName = true;
	}

	LUA_GET_VALUE( luaManager, "STOP_AT_START_STATE",	pNPCData->m_bStopAtStartState,	false	);


}




void CX2DungeonSubStage::SubStageData::LoadNPCData( KLuaManager& luaManager )
{
	if( luaManager.BeginTable( "NPC_GROUP" ) == true )
	{
		int npcIndex = 1;
		while( luaManager.BeginTable( npcIndex ) == true )
		{

			int iSubNPCStartIndex = m_NPCDataList.size();
			int iSubNPCEndIndex = iSubNPCStartIndex-1;
			int iSelectedSubNPCIndex = -1;
			int iSubNPCRate = 0;
			int iRandomNumber = rand()%100;
			bool bSubNPCSelected = false;
			bool bThereWasSubNPCScript = false;


			for( int i=0; ; i++ )
			{
				if( true == luaManager.BeginTable( "SUB_NPC", i ) )
				{
					NPCData* pNPCData = new NPCData();
					pNPCData->m_bSubNpc = true;
					FetchNPCData( luaManager, pNPCData );
					m_NPCDataList.push_back( pNPCData );

					bThereWasSubNPCScript = true;


					int iRate = 0;
					LUA_GET_VALUE( luaManager, "SUB_NPC_RATE", iRate, 0 );
					pNPCData->m_Rate = iRate;
					iSubNPCRate += iRate;

					if( iRandomNumber < iSubNPCRate && false == bSubNPCSelected )
					{
						bSubNPCSelected = true;
						iSelectedSubNPCIndex = m_NPCDataList.size() - 1;
					}
					++iSubNPCEndIndex;

					luaManager.EndTable();
				}
				else
				{
					break;
				}
			}


			if(	true == bThereWasSubNPCScript )
			{
				// 선택되지 않은 sub_NPC의 rate를 0으로
				for( int i=iSubNPCStartIndex; i<=iSubNPCEndIndex; i++ )
				{
					if( i != iSelectedSubNPCIndex )
					{
						NPCData* pNPCData = m_NPCDataList[i];
						//pNPCData->m_Rate = 0;
					}
				}
			}


			if( false == bThereWasSubNPCScript )
			{
				NPCData* pNPCData = new NPCData();
				pNPCData->m_bSubNpc = false;
				FetchNPCData( luaManager, pNPCData );
				m_NPCDataList.push_back( pNPCData );
			}

			npcIndex++;
			luaManager.EndTable();

		}
		luaManager.EndTable(); // NPC_GROUP
	}
}

void CX2DungeonSubStage::SubStageData::AddNPCData( KNPCUnitReq& kNPCUnitReq )
{
	NPCData* pNPCData = new NPCData();

	pNPCData->m_UID				= kNPCUnitReq.m_UID;
	pNPCData->m_UnitID			= (CX2UnitManager::NPC_UNIT_ID)kNPCUnitReq.m_NPCID ;
	pNPCData->m_KeyCode			= kNPCUnitReq.m_KeyCode;
	pNPCData->m_bFocusCamera	= kNPCUnitReq.m_bFocusCamera;
	pNPCData->m_StartPos		= kNPCUnitReq.m_nStartPos;
	pNPCData->m_bActive			= kNPCUnitReq.m_bActive;
	pNPCData->m_bShowGage		= kNPCUnitReq.m_bShowGage;
	pNPCData->m_AddPos.x		= kNPCUnitReq.m_AddPos.x;
	pNPCData->m_AddPos.y		= kNPCUnitReq.m_AddPos.y;
	pNPCData->m_AddPos.z		= kNPCUnitReq.m_AddPos.z;
	pNPCData->m_Level			= kNPCUnitReq.m_Level;
	pNPCData->m_bHasBossGage	= kNPCUnitReq.m_bHasBossGage;
	pNPCData->m_bShowSubBossName	= kNPCUnitReq.m_bShowSubBossName;
	pNPCData->m_bSiegeMode		= kNPCUnitReq.m_bSiegeMode;
	//{{ 2007. 10. 26  최육사
	pNPCData->m_bIsRight		= kNPCUnitReq.m_bIsRight;
	//}}
	pNPCData->m_bNoDrop			= kNPCUnitReq.m_bNoDrop;
	pNPCData->m_fUnitScale		= kNPCUnitReq.m_fUnitScale;

	pNPCData->m_usBossGaugeHPLines = kNPCUnitReq.m_usBossGaugeHPLines;

	pNPCData->m_bAllyNpc = kNPCUnitReq.m_bAllyNpc;

	pNPCData->m_fShowBossNameDelayTime	= kNPCUnitReq.m_fShowBossNameDelayTime;
	//m_fShowBossNameDelayTime이 지정되어있으면 m_bShowBossName은 무조건 true
	pNPCData->m_bShowBossName = (-1.f != kNPCUnitReq.m_fShowBossNameDelayTime ) ? true : kNPCUnitReq.m_bShowBossName;
	pNPCData->m_bStopAtStartState		= kNPCUnitReq.m_bStopAtStartState;

	ASSERT( CX2UnitManager::NUI_NONE != pNPCData->m_UnitID );

	m_NPCDataList.push_back( pNPCData );
}



void CX2DungeonSubStage::SubStageData::LoadTrigger( KLuaManager& luaManager )
{
	if( luaManager.BeginTable( "TRIGGER_LIST" ) == true )
	{
		int triggerIndex = 0;
		while( luaManager.BeginTable( "TRIGGER", triggerIndex ) == true )
		{
			Trigger* pTrigger = new Trigger();

			pTrigger->m_NowInterval = 0.0f;
			pTrigger->m_bReacted	= false;

			LUA_GET_VALUE( luaManager, "HOST", 				pTrigger->m_bHost,			false );
			LUA_GET_VALUE( luaManager, "ONE_TIME", 			pTrigger->m_bOneTime,		true );
			LUA_GET_VALUE( luaManager, "INTERVAL", 			pTrigger->m_Interval,		0.0f );
			LUA_GET_VALUE_UTF8( luaManager, "CONDITION_FUNC",		pTrigger->m_ConditionFunc,	"" );
			LUA_GET_VALUE_UTF8( luaManager, "REACT_FUNG",			pTrigger->m_ReactFunc,		"" );

			if( pTrigger->m_ConditionFunc.length() == 0 ||
				pTrigger->m_ReactFunc.length() == 0 )
			{
				SAFE_DELETE( pTrigger );
				triggerIndex++;
				luaManager.EndTable();
				continue;
			}

			triggerIndex++;
			m_TriggerList.push_back( pTrigger );
			luaManager.EndTable();
		}
		luaManager.EndTable();
	}
}












void CX2DungeonSubStage::SubStageData::LoadSecretStageEnteringSpeech( KLuaManager& luaManager )
{

	LUA_GET_VALUE_ENUM( luaManager,	 "SECRET_STAGE_NPC_ID", m_eSecretStageNPCID, CX2UnitManager::NPC_UNIT_ID, CX2UnitManager::NUI_NONE );



	m_SecretStageEnteringSpeech.resize(0);
	if( true == luaManager.BeginTable( "SECRET_STAGE_ENTERING_SPEECH" ) )
	{
		int iStringIndex = STR_ID_EMPTY;
		int iValueIndex = 1;
		while( true )
		{
			LUA_GET_VALUE_RETURN(	luaManager,	 iValueIndex,	iStringIndex, STR_ID_EMPTY, break; );
			m_SecretStageEnteringSpeech.push_back( GET_STRING( iStringIndex ) );
			iValueIndex++;
		}

		luaManager.EndTable(); // SECRET_STAGE_ENTERING_SPEECH
	}


}




// void CX2DungeonSubStage::SubStageData::CreateNPC()
// {
// 	int nActiveNpc = 0;
//
// 	for( int i = 0; i < (int)m_NPCDataList.size(); i++ )
// 	{
// 		NPCData* pNPCData = m_NPCDataList[i];
// 		CX2UnitManager::NPC_UNIT_ID eNPCID = pNPCData->m_UnitID;
//
// #ifdef FUEL_TANK_TEST
// #pragma DMLEE_NOTE( "사내에서 테스트로 박스대신 터지는 기름탱크가 나오게 테스트 중입니다." )
// 		if( eNPCID == CX2UnitManager::NUI_BOX )
// 		{
// 			eNPCID = CX2UnitManager::NUI_FUEL_TANK;
// 		}
// #endif FUEL_TANK_TEST
//
//
// 		const CX2UnitManager::NPCUnitTemplet* pTemplet = g_pData->GetUnitManager()->GetNPCUnitTemplet( eNPCID );
// 		if( pTemplet == NULL )
// 			continue;
//
// #ifdef SERV_ALLY_NPC
// 		CX2Room::TEAM_NUM eTeam = CX2Room::TN_MONSTER;
// 		int iAiType = (int)CX2NPCAI::NAT_NORMAL;
//
// 		if( pNPCData->m_bAllyNpc == true )
// 		{
// 			eTeam = CX2Room::TN_RED;
// 			iAiType = (int)CX2NPCAI::NAT_ALLY;
// 		}
//
// 		CX2GUNPC* pCX2GUNPC = CX2GUNPC::CreateGUNPC( 0, eTeam, pTemplet->m_LuaFileName.c_str(), pNPCData->m_UID,
// 			pTemplet, iAiType, -1, pNPCData->m_fUnitScale );
// #else
// 		CX2GUNPC* pCX2GUNPC = CX2GUNPC::CreateGUNPC( 0, CX2Room::TN_MONSTER, pTemplet->m_LuaFileName.c_str(), pNPCData->m_UID,
// 			pTemplet, (int)CX2NPCAI::NAT_NORMAL, -1, pNPCData->m_fUnitScale );
// #endif
//
// 		pCX2GUNPC->SetActiveMonster( pNPCData->m_bActive );
// 		pCX2GUNPC->SetAddPos( pNPCData->m_AddPos );
// 		pCX2GUNPC->SetKeyCode( pNPCData->m_KeyCode );
// 		pCX2GUNPC->SetFocusCamera( pNPCData->m_bFocusCamera );
// 		pCX2GUNPC->Init( false, 0 );
// 		pCX2GUNPC->SetSiegeMode( pNPCData->m_bSiegeMode );
//         pCX2GUNPC->SetShowObject( false );
// 		pCX2GUNPC->GetGageManager()->SetShow( pNPCData->m_bShowGage );
// 		pCX2GUNPC->GetGageManager()->SetHasBossGage( pNPCData->m_bHasBossGage );
// 		pCX2GUNPC->SetShowBossName( pNPCData->m_bShowBossName );
// 		pCX2GUNPC->SetShowSubBossName( pNPCData->m_bShowSubBossName );
// 		//{{ JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
// #ifdef SERV_BOSS_GAUGE_HP_LINES
// 		pCX2GUNPC->GetGageManager()->SetBossGaugeNumber( pNPCData->m_usBossGaugeHPLines );
// #endif SERV_BOSS_GAUGE_HP_LINES
// 		//}} JHKang / 강정훈 / 2011/01/26 / 보스 HP 바 여러 개로 구현
//
// 		m_NPCList.push_back( pCX2GUNPC );
//
// #ifdef DAMAGE_HISTORY
// 		if( pNPCData->m_bActive == true && pTemplet->m_ClassType == CX2UnitManager::NCT_BASIC )
// 			++nActiveNpc;
// #endif
// 	}
//
//
// #ifdef DAMAGE_HISTORY
// 	// 몬스터 타격 히스토리 설정(핵방지)
// 	// 50%확율로 타격검사 할 표본을 설정한다.
// 	if( g_pX2Game->IsHost() == true)
// 	{
// 		g_pInstanceData->SetSendHackMail(false);
//
// 		int iMaxCheckNpc = 3;
// 		int iCheckNpc = 0;
// 		for( UINT i = 0; i < m_NPCList.size(); ++i )
// 		{
// 			CX2GUNPC *pNpc = m_NPCList[i];
//
// 			if( pNpc != NULL && pNpc->IsActiveMonster() == true && pNpc->GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_BASIC )
// 			{
// 				++iCheckNpc;
// 				if( nActiveNpc <= iCheckNpc )
// 				{
// 					pNpc->SetDamageHistory(true);
// 					--iMaxCheckNpc;
// 				}
// 				else if( iMaxCheckNpc > 0 && rand() % 2 == 0 )
// 				{
// 					pNpc->SetDamageHistory(true);
// 					--iMaxCheckNpc;
// 				}
// 			}
// 		}
// 	}
// #endif
// }

/*static*/
void CX2DungeonSubStage::CreateAndAppendGUNPC( std::vector<CX2GUNPC*>& vecInOutNPCs_, const vector<NPCData*>& vecNPCData_ )
{
	BOOST_TEST_FOREACH( const NPCData*, pNpcData, vecNPCData_ )
	{
		CX2GUNPC*  pCX2GUNPC = CreateGUNPC( pNpcData );

		ASSERT( pCX2GUNPC != NULL );
		if ( pCX2GUNPC != NULL )
			vecInOutNPCs_.push_back( pCX2GUNPC );
	}
}

/*static*/
CX2GUNPC*    CX2DungeonSubStage::CreateGUNPC( const NPCData* pNpcData_ )
{
	CX2UnitManager::NPC_UNIT_ID eNPCID = pNpcData_->m_UnitID;


	const CX2UnitManager::NPCUnitInfo* pInfo = g_pData->GetUnitManager()->GetNPCUnitInfo( eNPCID );
	ASSERT( pInfo != NULL );
	if( pInfo == NULL )
		return NULL;

	if( g_pX2Game != NULL )
		g_pX2Game->ClearDangerAlarm();

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
	CX2GUNPC* pCX2GUNPC = CX2GUNPC::CreateGUNPC( 0, CX2Room::TN_MONSTER, pNpcData_->m_UID,
		pInfo, (int)CX2NPCAI::NAT_NORMAL, NULL, pNpcData_->m_fUnitScale );
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//	CX2GUNPC* pCX2GUNPC = CX2GUNPC::CreateGUNPC( 0, CX2Room::TN_MONSTER, pInfo->m_templet.m_LuaFileName.c_str(), pNpcData_->m_UID,
//		&(pInfo->m_templet), (int)CX2NPCAI::NAT_NORMAL, NULL, pNpcData_->m_fUnitScale );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING

	ASSERT( pCX2GUNPC != NULL );
	if ( pCX2GUNPC == NULL )
		return NULL;

	pCX2GUNPC->SetActiveMonster( pNpcData_->m_bActive );
	pCX2GUNPC->SetAddPos( pNpcData_->m_AddPos );
	pCX2GUNPC->SetKeyCode( pNpcData_->m_KeyCode );
	pCX2GUNPC->SetFocusCamera( pNpcData_->m_bFocusCamera );
	pCX2GUNPC->SetHasBossGage( pNpcData_->m_bHasBossGage );
	pCX2GUNPC->SetNumOfGage( pNpcData_->m_usBossGaugeHPLines );
	pCX2GUNPC->Init( false, 0 );
	pCX2GUNPC->SetSiegeMode( pNpcData_->m_bSiegeMode );
	pCX2GUNPC->SetShowObject( false );
	pCX2GUNPC->SetShowGage( pNpcData_->m_bShowGage );
	pCX2GUNPC->SetShowSubBossName( pNpcData_->m_bShowSubBossName );
	pCX2GUNPC->SetShowBossNameDelayTime( pNpcData_->m_fShowBossNameDelayTime );

	//m_fShowBossNameDelayTime이 지정되어있으면 m_bShowBossName은 무조건 true
	bool bShowBossName = (-1.f != pNpcData_->m_fShowBossNameDelayTime ) ? true : pNpcData_->m_bShowBossName;
	pCX2GUNPC->SetShowBossName( bShowBossName );

	pCX2GUNPC->SetStopAtStartState( pNpcData_->m_bStopAtStartState );

	return pCX2GUNPC;

}// CX2DungeonSubStage::CreateGUNPC()

/** @function : ChangeLineTypeAfterStageClear
	@brief : 스테이지 클리어 이후 라인맵 속성을 변경
*/
void CX2DungeonSubStage::ChangeLineTypeAfterStageClear()
{
	// 현재 스테이지, 서브스테이지 체크
	int iNowStage = 0;
	int iNowSubStage = 0;
	CX2DungeonGame* pDungoenGame = static_cast<CX2DungeonGame*>(g_pX2Game);
	if( NULL != pDungoenGame )
	{
		CX2Dungeon* pDungeon = pDungoenGame->GetDungeon() ;
		if( NULL != pDungeon &&
			NULL != pDungeon->GetNowStage() )
		{
			iNowStage = pDungeon->GetNowStageIndex();
			iNowSubStage = GetSubStageIndex();
		}
	}

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetLineMap() &&
		NULL != g_pX2Game->GetMajorParticle() )
	{
		for( int i=0; i<g_pX2Game->GetLineMap()->GetNumLineData(); ++i )
		{
			CKTDGLineMap::LineData* pLineData = g_pX2Game->GetLineMap()->AccessLineData( i );

			// 변경할 라인타입과 현재 라인타입이 다를 때
			if( NULL != pLineData &&
				CKTDGLineMap::LT_INVALID != pLineData->m_eLineTypeAfterClearStage &&
				pLineData->m_eLineTypeAfterClearStage != pLineData->lineType )
			{
				// 지정되지 않았거나, 현재 스테이지가 지정된 스테이지일 때
				if( -1 == pLineData->m_iChangeAfterClearStage ||
					iNowStage == pLineData->m_iChangeAfterClearStage )
				{
					// 서브스테이지도 같은 조건 체크
					if( -1 == pLineData->m_iChangeAfterClearSubStage ||
						iNowSubStage == pLineData->m_iChangeAfterClearSubStage )
					{
						pLineData->lineType = pLineData->m_eLineTypeAfterClearStage;
						CX2WorldObjectParticle* pParticle = g_pX2Game->GetWorld()->CreateObjectParticle( g_pX2Game->GetMajorParticle(), "Peita_Teleport_MagicSquare01" );
						if( NULL != pParticle )
						{
							pParticle->SetParticlePos( ( pLineData->startPos + pLineData->endPos ) * 0.5f + D3DXVECTOR3(0, 3, 0 ) );
							pParticle->SetLayer( XL_EFFECT_0 );
						}
					}
				}
			}
		}
	}
}
/** @function : ProcessAfterSubStageClear
	@brief : HOST가 아닌 경우, 서브 스테이지 클리 후 처리 해야 하는 동작
*/
void CX2DungeonSubStage::ProcessAfterSubStageClear_NotHost()
{
	// 라인맵 타입 변환
	ChangeLineTypeAfterStageClear();
}

float CX2DungeonSubStage::GetRemainTimeInClearCondition()
{
	if( NULL == m_pSubStageData )
		return 0.0f;

	for( UINT iClearCondIndex=0; iClearCondIndex< m_pSubStageData->m_vecClearCondData.size(); iClearCondIndex++ )
	{
		ClearCondtionData& clearCond = m_pSubStageData->m_vecClearCondData[iClearCondIndex];
		if( false == clearCond.m_bEnable )
			continue;

		if( true == clearCond.m_bChecked )
			continue;

		if( true == clearCond.m_vecNextStage.empty() )
			continue;

		switch( clearCond.m_eClearCondition )
		{
		case CC_TIME:
			return clearCond.m_fClearTime - m_fElapsedTimeSubStage;
		default:
			break;
		}
	}

	return 0.0f;
}
