#include "stdafx.h"
#include "./X2MiniMapUI.h"


CX2MiniMapUI::CX2MiniMapUI(void) :
m_pDungeonMiniMap( NULL ),
m_pFieldMiniMap( NULL ),
m_pWorldMapUI( NULL ),
m_pDLGFrameWindow(NULL),
m_QuestVillageMapID(-1),
m_TopQuestID(-1),
m_bCreateNewQuestEffectWhenOpen(false),
m_bCreateNewMailEffectWhenOpen(false),
m_iNewMail(0),
m_bShow(false),
m_iChangeChannelId(0),
m_pDLGQuery(NULL),
m_fChannelInfoTime(10.f),
m_fChangeChannelTime(5.f),
m_eCurrentMiniMapType( MMT_DUNGEON )
//{{ 허상형 : [2011/3/14/] //	월드 미션
, m_bIsOpenDefenceMinimap(false)
//}} 허상형 : [2011/3/14/] //	월드 미션
{
	m_pDLGFrameWindow		= new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Mini_Map_NEW.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGFrameWindow );
	m_pDLGFrameWindow->SetShow(false);




}

CX2MiniMapUI::~CX2MiniMapUI(void)
{
	CX2MiniMap::DeleteKTDGObject( m_pDungeonMiniMap );
	CX2FieldMiniMap::DeleteKTDGObject( m_pFieldMiniMap );
	SAFE_DELETE_DIALOG( m_pDLGFrameWindow );
	//CX2WorldMapUI::DeleteKTDGObject( m_pWorldMapUI );
	SAFE_DELETE( m_pWorldMapUI );

	//{{ kimhc // 2009-12-16 // 서버군 추가 작업
	m_vecChannelInfoInMinimap.clear();
	//}} kimhc // 2009-12-16 // 서버군 추가 작업



}

/*virtual*/ HRESULT	CX2MiniMapUI::OnFrameMove( double fTime, float fElapsedTime )
{
	m_fChannelInfoTime += fElapsedTime;
	m_fChangeChannelTime += fElapsedTime;

	if(m_fChannelInfoTime > 10.f)
		m_fChannelInfoTime = 10.f;

	if(m_fChangeChannelTime > 5.f)
		m_fChangeChannelTime = 5.f;




	return S_OK;
}

/*virtual*/ bool CX2MiniMapUI::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	if( NULL != m_pWorldMapUI )
	{
		if( true == m_pWorldMapUI->UICustomEventProc(hWnd, uMsg, wParam, lParam) )
			return true;
	}

	switch(wParam)
	{
	case MMUCM_ENDURANCE_BUTTON_OVER:
		{
			SetShowEnduranceUI( true );
		} break;
	case MMUCM_ENDURANCE_BUTTON_OUT:
		{
			SetShowEnduranceUI( false );
		} break;
	case MMUCM_DUNGEON_ZOOM_IN:
	case MMUCM_FIELD_ZOOM_IN:
		{
			g_pMain->GetGameOption().CameraZoomIn( 1 );
			return true;
		} break;

	case MMUCM_DUNGEON_ZOOM_OUT:
	case MMUCM_FIELD_ZOOM_OUT:
		{
			g_pMain->GetGameOption().CameraZoomIn( -1 );
			return true;
		} break;

	case MMUCM_ZOOM_BUTTON:
		{
			int iZoomLevel = g_pMain->GetGameOption().GetOptionList().m_iZoomLevel;
			iZoomLevel = iZoomLevel - 1;
			if( iZoomLevel < -1 )
				iZoomLevel = 1;
			g_pMain->GetGameOption().GetOptionList().m_iZoomLevel = iZoomLevel;

		} break;

	case MMUCM_FIELD_OPEN_WORLD_MAP:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_DUNGEON_GAME:
				{

					//{{ 2009.02.12 김태완
					if( g_pMain->GetIsPlayingTutorial() == false )
					{
						CX2StateDungeonGame* pStateDungeonGame = (CX2StateDungeonGame*) g_pMain->GetNowState();
						pStateDungeonGame->ToggleDungeonMapUI();
					}
					//}}

				} break;
			case CX2Main::XS_SQUARE_GAME:
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_BATTLE_FIELD:
				{
					if( m_pWorldMapUI != NULL )
					{
						if( true == m_pWorldMapUI->GetShow() )
						{
							m_pWorldMapUI->OpenWorldMap( false );
							m_pWorldMapUI->OpenFieldMap( false );
							m_pWorldMapUI->CloseAllToolTip();
							return true;
						}
						else
						{
							m_pWorldMapUI->OpenWorldMap( true );
							m_pWorldMapUI->UpdateWorldMap();

							if(g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP)
							{
								int villageID = g_pData->GetLocationManager()->GetCurrentVillageID();
								m_pWorldMapUI->OpenFieldMap( true, villageID );
								m_pWorldMapUI->UpdateFieldMap();
							}
							else if( g_pMain->GetNowStateID() == CX2Main::XS_BATTLE_FIELD )
							{
								CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
								int iFiledID = battleFieldManager.GetBattleFieldIdWhereIam();
								m_pWorldMapUI->OpenFieldMap( true, iFiledID );
								m_pWorldMapUI->UpdateFieldMap();
							}
						}
					} break;
				}
			}
			return true;

		} break;
	case MMUCM_FIELD_OPEN_NEW_QUEST_MAP:
		{
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_SQUARE_GAME:
			case CX2Main::XS_BATTLE_FIELD:
				{
					if( NULL == m_pWorldMapUI )
					{
						m_pWorldMapUI = new CX2WorldMapUI;
					}

					if( NULL != m_pWorldMapUI && NULL != m_pFieldMiniMap )
					{
						m_pWorldMapUI->OpenWorldMap( true );
						m_pWorldMapUI->UpdateWorldMap();

						m_pWorldMapUI->OpenFieldMap( true, m_QuestVillageMapID );
						m_pWorldMapUI->UpdateFieldMap();

						if(g_pData->GetQuestManager()->GetQuestTemplet( m_TopQuestID ) != NULL)
						{
							int NPCID = (int)g_pData->GetQuestManager()->GetQuestTemplet( m_TopQuestID )->m_eStartNPCID;
							m_pWorldMapUI->GetWorldFieldMap()->MakeQuestNoticeEffect( NPCID );
						}

					}
				} break;
			}
			return true;
		} break;
	case MMUCM_CHANGE_CHANNEL:
		{
			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );
			if( m_pDLGQuery == NULL && pComboBox != NULL && g_pInstanceData != NULL)
			{
				g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDLGFrameWindow, XDL_NORMAL );



				//{{ kimhc // 2009-12-18 // 서버군 추가에 따른 채널 ID 얻어오는 법 변경
				// m_vecChannelInfoInMinimap의 순서와 comboBox의 순서가 같음
				m_iChangeChannelId = m_vecChannelInfoInMinimap[ pComboBox->GetSelectedItemIndex() ].m_iChannelID;
				//}} kimhc // 2009-12-18 // 서버군 추가에 따른 채널 ID 얻어오는 법 변경

				if(m_iChangeChannelId == g_pInstanceData->GetConnectChannelServerID())
					return false;




				for(UINT i=0; i<g_pInstanceData->m_vecChannelInfo.size(); ++i)
				{
					KChannelInfo channelInfo = g_pInstanceData->m_vecChannelInfo[i];
					if(channelInfo.m_iChannelID == m_iChangeChannelId)
					{
						float fRate = (float)channelInfo.m_iCurrentUser / (float)channelInfo.m_iMaxUser;
						if(fRate >= 0.95f)
						{
							return false;
						}
						break;
					}
				}

				if( g_pData->GetPartyManager()->GetProcessPvpMatch() == true )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300),GET_STRING( STR_ID_13646 ), g_pMain->GetNowState() );
				}
				else
				{
					wstring wstrBuf = GET_REPLACED_STRING( ( STR_ID_2651, "S", g_pInstanceData->GetChannelServerName(m_iChangeChannelId).c_str() ) );
					m_pDLGQuery = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), wstrBuf.c_str(),
						MMUCM_CHANGE_CHANNEL_ACCEPT, g_pMain->GetNowState(), MMUCM_CHANGE_CHANNEL_CANCEL );
				}

// 				wstring wstrBuf = GET_REPLACED_STRING( ( STR_ID_2651, "S", g_pInstanceData->GetChannelServerName(m_iChangeChannelId).c_str() ) );
// 				if( g_pData->GetPartyManager()->GetMyPartyData()->m_bPvpParty == true )
// 				{
// 					if( g_pData->GetPartyManager()->DoIHaveParty() == true )
// 					{
// 						if( g_pData->GetPartyManager()->GetProcessPvpMatch() == true )
// 						{
// 							wstrBuf = GET_REPLACED_STRING( ( STR_ID_13551, "S", g_pInstanceData->GetChannelServerName(m_iChangeChannelId).c_str() ) );
// 						}
// 						else
// 						{
// 							wstrBuf = GET_REPLACED_STRING( ( STR_ID_13550, "S", g_pInstanceData->GetChannelServerName(m_iChangeChannelId).c_str() ) );
// 						}
// 					}
// 					else if( g_pData->GetPartyManager()->GetProcessPvpMatch() == true )
// 					{
// 						wstrBuf = GET_REPLACED_STRING( ( STR_ID_13552, "S", g_pInstanceData->GetChannelServerName(m_iChangeChannelId).c_str() ) );
// 					}
// 				}
// 				else if( g_pData->GetPartyManager()->GetProcessPvpMatch() == true && g_pData->GetPartyManager()->DoIHaveParty() == false )
// 				{
// 					wstrBuf = GET_REPLACED_STRING( ( STR_ID_13552, "S", g_pInstanceData->GetChannelServerName(m_iChangeChannelId).c_str() ) );
// 				}
//
// 				m_pDLGQuery = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), wstrBuf.c_str(),
// 					MMUCM_CHANGE_CHANNEL_ACCEPT, g_pMain->GetNowState(), MMUCM_CHANGE_CHANNEL_CANCEL );
			}
		}
		break;
	case MMUCM_CHANGE_CHANNEL_ACCEPT:
		{
			if ( m_pDLGQuery != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGQuery, NULL, false );
				m_pDLGQuery = NULL;
			}

			m_fChangeChannelTime = 0.f;

			// 채널이동승낙
			CX2State* pState = (CX2State*)g_pMain->GetNowState();
			pState->Handler_EGS_CHECK_CHANNEL_CHANGE_REQ(m_iChangeChannelId);
		}
		break;
	case MMUCM_CHANGE_CHANNEL_CANCEL:
		{
			if ( m_pDLGQuery != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGQuery, NULL, false );
				m_pDLGQuery = NULL;
			}

			// 채널이동취소
			m_iChangeChannelId = 0;
			UpdateChannelIndex();
		}
		break;
	case MMUCM_CHANGE_CHANNEL_OPEN:
		{
			// 채널 혼잡도 갱신
			if(m_fChannelInfoTime >= 10.f)
			{
				CX2State* pState = (CX2State*)g_pMain->GetNowState();
				pState->Handler_EGS_GET_CHANNEL_LIST_REQ();
				m_fChannelInfoTime = 0.f;
			}

			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );
			if( pComboBox != NULL )
			{
				g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDLGFrameWindow, XDL_MESSENGER );
			}


		}
		break;
	case MMUCM_CHANGE_CHANNEL_CLOSE:
		{
			CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );
			if( pComboBox != NULL )
				g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( m_pDLGFrameWindow, XDL_NORMAL );
		}
		break;
	}

	return false;
}

void CX2MiniMapUI::UpdateChannelIndex()
{
	int ChannelIndex = g_pInstanceData->GetConnectChannelServerID();
	CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );
	if(pComboBox != NULL)
	{
		//{{ kimhc // 2009-12-18 // 서버군 추가에 따른 ComboList의 Index 얻어오는 법 변경
		int channelListIndex = GetListIndexFromChannelID( ChannelIndex );

		pComboBox->SetSelectedByIndex( channelListIndex );
		//}} kimhc // 2009-12-18 // 서버군 추가에 따른 ComboList의 Index 얻어오는 법 변경
	}
}

void CX2MiniMapUI::SetShowMiniMap( MINI_MAP_TYPE eMiniMapType, bool bShow )
{

	if(bShow == false)
	{
		if(m_pDLGQuery != NULL)
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGQuery, NULL, false );
		m_pDLGQuery = NULL;
	}

	switch( eMiniMapType )
	{
	case MMT_DUNGEON:
		{
			// 퀘스트 버튼을 disable 시킬 것
			// 몬스터 표시를 켤 것 <- 해당 던전의 FrameMove에서 해 준다..
			// 이 던전에서는 경험치 안나옴 일단 끌 것
			// 나머지는 그냥 ㄱㄱ
			if(m_pDungeonMiniMap == NULL && bShow == true)
			{
				CreateDungeonMiniMap();
			}
		} break;

	case MMT_FIELD:
		{
		} break;
	}

	if(bShow)
	{
		// 켜질때는 무조건 켜주고 상태도 바꿔주고
		m_bShow = bShow;
		m_eCurrentMiniMapType = eMiniMapType;
		m_pDLGFrameWindow->SetShowEnable( bShow, bShow );

		UpdateEnduranceNotice();

		CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );

		if(pComboBox != NULL)
		{
			pComboBox->SetDrawTextLimit(8);

			int ChannelIndex = 0;
			if( NULL != g_pInstanceData )
			{
				ChannelIndex = g_pInstanceData->GetConnectChannelServerID();


				//{{ kimhc // 2009-12-18 // 서버군 추가에 따른 ComboList의 Index 얻어오는 법 변경
				int channelListIndex = GetListIndexFromChannelID( ChannelIndex );

				if( channelListIndex != pComboBox->GetSelectedItemIndex() )
				{
					pComboBox->SetSelectedByIndex( channelListIndex );
				}
				//}} kimhc // 2009-12-18 // 서버군 추가에 따른 ComboList의 Index 얻어오는 법 변경

				if( m_fChangeChannelTime >= 5.f &&
					g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE) == false &&
					g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP &&
					(true == g_pData->GetLocationManager()->IsVillage( g_pData->GetLocationManager()->GetCurrentVillageID() )
					 )
				)
				{
					pComboBox->SetEnable(true);
				}
				else
				{
					pComboBox->SetEnable(false);
				}

			}

		}
	}
	else
	{
		// 꺼질때는 현재 상태와 들어온 상태가 같을 때만 꺼준다
		if(m_eCurrentMiniMapType == eMiniMapType)
		{
			SetShowEnduranceUI( false );
			m_pDLGFrameWindow->SetShowEnable( bShow, bShow );
			m_bShow = bShow;
		}
	}

}

void CX2MiniMapUI::SetVillageMapID( int iMapID )
{
	if( NULL != m_pDLGFrameWindow )
	{
		switch( m_eCurrentMiniMapType )
		{
		case MMT_DUNGEON:
			{
				switch( g_pMain->GetNowStateID() )
				{
				case CX2Main::XS_BATTLE_FIELD:
					{
						CKTDGUIStatic* pStatic_MapName = (CKTDGUIStatic*) m_pDLGFrameWindow->GetControl( L"mini_map_title" );
						if( NULL != pStatic_MapName && NULL != pStatic_MapName->GetString(0) )
						{
							CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();

							if ( NULL != battleFieldManager.GetBattleFieldNameByBattleFieldId( iMapID ) )
							{
								bool bEllipse = false;
								wstring tempName = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(battleFieldManager.GetBattleFieldNameByBattleFieldId( iMapID ), 150, pStatic_MapName->GetString(0)->fontIndex, 1, bEllipse);

								if(bEllipse == true)
								{
									CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"miniMapTip" );

									if(pButton != NULL)
									{
										pButton->SetGuideDesc(battleFieldManager.GetBattleFieldNameByBattleFieldId( iMapID ));
										pButton->SetShow(true);
									}
								}
								else
								{
									CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"miniMapTip" );

									if(pButton != NULL)
									{
										pButton->SetShow(false);
									}
								}

								pStatic_MapName->GetString(0)->msg = tempName;
							}
						}
					} break;
				case CX2Main::XS_VILLAGE_MAP:
					{
						if(g_pTFieldGame != NULL)
						{
							CKTDGUIStatic* pStatic_MapName = (CKTDGUIStatic*) m_pDLGFrameWindow->GetControl( L"mini_map_title" );
							if( NULL != pStatic_MapName && NULL != pStatic_MapName->GetString(0) )
							{
								CX2LocationManager::VillageTemplet* pVillageTemplet =
									g_pData->GetLocationManager()->GetVillageMapTemplet( (SEnum::VILLAGE_MAP_ID) iMapID );
								if( NULL != pVillageTemplet )
								{
									bool bEllipse = false;
									wstring tempName = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse( pVillageTemplet->m_Name.c_str(), 150, pStatic_MapName->GetString(0)->fontIndex, 1, bEllipse);

									if(bEllipse == true)
									{
										CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"miniMapTip" );

										if(pButton != NULL)
										{
											pButton->SetGuideDesc( pVillageTemplet->m_Name.c_str() );
											pButton->SetShow(true);
										}
									}
									else
									{
										CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"miniMapTip" );

										if(pButton != NULL)
										{
											pButton->SetShow(false);
										}
									}

									pStatic_MapName->GetString(0)->msg = tempName;
								}
								else
								{
									ASSERT( !"village templet is null" );
									pStatic_MapName->GetString(0)->msg = L"";
								}
							}
						}
					} break;
				}

				if( NULL != m_pFieldMiniMap )
				{
					m_pFieldMiniMap->SetVillageID( iMapID );
				}
/*
				if(g_pTFieldGame != NULL)
				{
					CKTDGUIStatic* pStatic_MapName = (CKTDGUIStatic*) m_pDLGFrameWindow->GetControl( L"mini_map_title" );
					if( NULL != pStatic_MapName )
					{

						CX2LocationManager::VillageTemplet* pVillageTemplet =
							g_pData->GetLocationManager()->GetVillageMapTemplet( (SEnum::VILLAGE_MAP_ID) iMapID );
						if( NULL != pVillageTemplet )
						{
							pStatic_MapName->GetString(0)->msg = pVillageTemplet->m_Name;
						}
						else
						{
							ASSERT( !"village templet is null" );
							pStatic_MapName->GetString(0)->msg = L"";
						}
					}

					if(m_pFieldMiniMap != NULL)
					{
						m_pFieldMiniMap->SetVillageID( iMapID );
					}
				}*/


			} break;
		case MMT_FIELD:
			{
			} break;
		default:
			{
				ASSERT( !"Error!" );
			} break;
		}	// switch
	}
}

void CX2MiniMapUI::SetTitle(const WCHAR* val)
{
	CKTDGUIStatic* pStatic = (CKTDGUIStatic*) m_pDLGFrameWindow->GetControl( L"mini_map_title" );
	if( pStatic != NULL )
	{
		bool bEllipse = false;
		wstring tempName = CWordLineHandler::GetStrByLineBreakInX2MainWithEllipse(val, 140, pStatic->GetString(0)->fontIndex, 1, bEllipse);

		if(bEllipse == true)
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"miniMapTip" );

			if(pButton != NULL)
			{
				pButton->SetGuideDesc(val);
				pButton->SetShow(true);
			}
		}
		else
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"miniMapTip" );

			if(pButton != NULL)
			{
				pButton->SetShow(false);
			}
		}

		pStatic->GetString(0)->msg = tempName;
	}
}

void CX2MiniMapUI::SetStage( CKTDXStage* pStage )
{
	if( NULL != m_pDLGFrameWindow )
	{
		m_pDLGFrameWindow->SetStage( pStage );
	}

	if( NULL != m_pDungeonMiniMap )
	{
		m_pDungeonMiniMap->SetStage( pStage );
	}

	if( NULL != m_pFieldMiniMap )
	{
		m_pFieldMiniMap->SetStage( pStage );
	}

	if( NULL != m_pWorldMapUI )
	{
		m_pWorldMapUI->SetStage( pStage );
	}
}

CX2WorldMapUI* CX2MiniMapUI::GetWorldMapUI()
{
	if( m_pWorldMapUI == NULL )
		m_pWorldMapUI = new CX2WorldMapUI;

	return m_pWorldMapUI;
}




//{{
void CX2MiniMapUI::UpdateQuestNotice( int iQuestNum, int TopQuestID, int VID )
{

	CKTDGUIButton* pButton = NULL;
	if(NULL != m_pDLGFrameWindow)
	{
		pButton = (CKTDGUIButton*) m_pDLGFrameWindow->GetControl( L"Button_NewQuest" );
	}
	else
	{
		return;
	}


	// 1. 퀘스트가 몇개인지 확인해서 0개면 끄고 끝 / 1개 이상이면 켜고 계속 진행
	if(iQuestNum > 0)
	{
		// 3. TopQuestID를 저장된 값과 비교해서 이펙트를 표시할지 결정하고
		// 4. TopQuest를 저장하고 끗

		if(NULL != pButton)
		{
			pButton->SetGuideDesc( GET_REPLACED_STRING( ( STR_ID_363, "i", iQuestNum ) ) );
			pButton->SetGuideDescOffsetPos(D3DXVECTOR2(28.f, 122.f));
			pButton->SetShowEnable(true, true);
		}

		if(m_TopQuestID != TopQuestID )
		{
			// 이펙트 표시 해주고
			if( m_bShow )
			{
				m_bCreateNewQuestEffectWhenOpen = false;
				CreateNewQuestEffect();
			}
			else
			{
				m_bCreateNewQuestEffectWhenOpen = true;
			}


			m_TopQuestID = TopQuestID;
		}

		m_QuestVillageMapID = VID;
	}
	else
	{
		m_bCreateNewQuestEffectWhenOpen = false;
		m_TopQuestID = 0;
		m_QuestVillageMapID = SEnum::VMI_RUBEN;

		if(NULL != pButton)
		{
			pButton->SetShow(true);
			pButton->SetEnable(false);
		}
	}
}

void CX2MiniMapUI::ZoomDLGSetting()
{
	if ( m_pDLGFrameWindow != NULL )
	{
		CKTDGUIStatic* pStaticZoomGrade = (CKTDGUIStatic*)m_pDLGFrameWindow->GetControl( L"Zoom_num" );
		if ( pStaticZoomGrade != NULL )
		{
			WCHAR wszText[8] = L"";
			// +2를 해 주는 이유 : 옵션은 -1~1까지라서 =3=
			StringCchPrintf( wszText, 8, L"%d", g_pMain->GetGameOption().GetOptionList().m_iZoomLevel+2 );
			//wsprintf( wszText, L"%d", g_pMain->GetGameOption().GetOptionList().m_iZoomLevel+2 );

			pStaticZoomGrade->GetString(0)->msg = wszText;
			// 			for ( int i = 0; i < 3; i++ )
			// 			{
			// 				if ( pStaticZoomGrade->GetPicture(i) != NULL )
			// 				{
			// 					pStaticZoomGrade->GetPicture(i)->SetShow( false );
			// 				}
			// 			}
			//
			// 			switch( g_pMain->GetGameOption().GetOptionList().m_iZoomLevel )
			// 			{
			// 			default:
			// 			case 0:
			// 				{
			// 					if ( pStaticZoomGrade->GetPicture(1) != NULL )
			// 						pStaticZoomGrade->GetPicture(1)->SetShow( true );
			// 				} break;
			//
			// 			case 1:
			// 				{
			// 					if ( pStaticZoomGrade->GetPicture(2) != NULL )
			// 						pStaticZoomGrade->GetPicture(2)->SetShow( true );
			// 				} break;
			//
			// 			case -1:
			// 				{
			// 					if ( pStaticZoomGrade->GetPicture(0) != NULL )
			// 						pStaticZoomGrade->GetPicture(0)->SetShow( true );
			// 				} break;
			// 			}
		}
	}
}


void CX2MiniMapUI::CreateNewQuestEffect()
{
	if(NULL == m_pDLGFrameWindow)
	{
		return;
	}

	g_pKTDXApp->GetDeviceManager()->PlaySound( L"QuestNew.ogg", false, false );

	CKTDGUIButton* pButton = (CKTDGUIButton*) m_pDLGFrameWindow->GetControl( L"Button_NewQuest" );
	ASSERT( NULL != pButton );
	if( NULL == pButton )
		return;


	D3DXVECTOR2 pos = pButton->GetDialog()->GetPos() + pButton->GetPos();  // 717,92
	//const float MAGIC_EFFECT_POSITION_X = 744.f;
	//const float MAGIC_EFFECT_POSITION_Y = 119.f;
// 	const float MAGIC_EFFECT_OFFSET_X = 27.f;
// 	const float MAGIC_EFFECT_OFFSET_Y = 27.f;
// 	pos.x += MAGIC_EFFECT_OFFSET_X;
// 	pos.y += MAGIC_EFFECT_OFFSET_Y;

	const float fButtonSizeX = (float)pButton->GetWidth();
	const float fButtonSizeY = (float)pButton->GetHeight();
	pos.x += fButtonSizeX/2;
	pos.y += fButtonSizeY/2;

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"HyperBallTick", pos.x, pos.y, 0.0f, 1000,1000, -1, 10 );
	if( pSeq != NULL )
	{
		pSeq->SetOverUI( true );
		pSeq->SetBlackHolePosition( pSeq->GetPosition() );
		pSeq->UseLookPoint( true );
		pSeq->SetLookPoint( pSeq->GetPosition() );
		pSeq->SetLatency(0.3f);
	}
	pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"HyperBallRing", pos.x, pos.y, 0.0f, 1000,1000, -1, 1 );
	if( pSeq != NULL )
	{
		pSeq->SetOverUI( true );
		pSeq->SetLatency(0.3f);
	}

}


void CX2MiniMapUI::UpdateMailNotice( std::vector<KPostSmallTitleInfo> &vecUnReadMailTitle )
{

	CKTDGUIButton* pButton = NULL;
	if(NULL != m_pDLGFrameWindow)
	{
		pButton = (CKTDGUIButton*) m_pDLGFrameWindow->GetControl( L"Button_NewMail" );
	}
	else
	{
		return;
	}

	int iNewMailCount = g_pMain->GetPostBox()->GetUnReadMail();

	if(iNewMailCount > 0)
	{
		pButton->SetShowEnable(true, true);

		wstring wstrUnReadMailInfo = L"";

		wstrUnReadMailInfo = GET_REPLACED_STRING( ( STR_ID_364, "i", iNewMailCount ) );
		wstrUnReadMailInfo += L"\n";

		for(UINT i=0; i<vecUnReadMailTitle.size(); ++i)
		{
			KPostSmallTitleInfo unReadMailInfo = vecUnReadMailTitle[i];
			wstring wstrTitle = L"";

			if(unReadMailInfo.m_cScriptType == KPostItemInfo::LT_POST_OFFICE || unReadMailInfo.m_cScriptType == KPostItemInfo::LT_NONE )
			{
				wstrTitle = GET_STRING( STR_ID_260 );
			}

			else if(unReadMailInfo.m_cScriptType == KPostItemInfo::LT_WEDDING_INVITATION || unReadMailInfo.m_cScriptType == KPostItemInfo::LT_WEDDING_RESERVE
				|| unReadMailInfo.m_cScriptType == KPostItemInfo::LT_WEDDING_REWARD  )
			{
				wstrTitle = GET_STRING ( STR_ID_24667 );
			}
			else
			{
				wstrTitle = GET_STRING( STR_ID_2613 );
			}


			wstrTitle += unReadMailInfo.m_wstrLetterTitle;

			if( wstrTitle.length() > 15 )
			{
				wstrTitle = wstrTitle.substr( 0, 15 );
			}



			wstrUnReadMailInfo += L"\n";
			wstrUnReadMailInfo += wstrTitle;
		}
		wstrUnReadMailInfo += L"\n";

		pButton->SetGuideDesc( wstrUnReadMailInfo.c_str() );
		pButton->SetGuideDescOffsetPos(D3DXVECTOR2(25.f, 113.f));

		if(iNewMailCount > m_iNewMail)
		{
			// 이펙트 표시 해주고
			if(m_bShow)
			{
				m_bCreateNewMailEffectWhenOpen = false;
				CreateNewMailEffect();
			}
			else
			{
				m_bCreateNewMailEffectWhenOpen = true;
			}
		}
	}
	else
	{
		m_bCreateNewMailEffectWhenOpen = false;
		pButton->SetShowEnable(true, false);
	}
	m_iNewMail = iNewMailCount;

}




void CX2MiniMapUI::CreateNewMailEffect()
{
	if(NULL == m_pDLGFrameWindow)
	{
		return;
	}

	g_pKTDXApp->GetDeviceManager()->PlaySound( L"PostLetterNew.ogg", false, false );

	CKTDGUIButton* pButton = (CKTDGUIButton*) m_pDLGFrameWindow->GetControl( L"Button_NewMail" );
	D3DXVECTOR2 pos = pButton->GetDialog()->GetPos() + pButton->GetPos();  // 717,92
	//const float MAGIC_EFFECT_POSITION_X = 744.f;
	//const float MAGIC_EFFECT_POSITION_Y = 119.f;
	const float fButtonSizeX = (float)pButton->GetWidth();
	const float fButtonSizeY = (float)pButton->GetHeight();
	//const float MAGIC_EFFECT_OFFSET_Y = 27.f;
// 	pos.x += MAGIC_EFFECT_OFFSET_X;
// 	pos.y += MAGIC_EFFECT_OFFSET_Y;
	pos.x += fButtonSizeX/2;
	pos.y += fButtonSizeY/2;

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"HyperBallTick", pos.x, pos.y, 0.0f, 1000,1000, -1, 10 );
	if( pSeq != NULL )
	{
		pSeq->SetOverUI( true );
		pSeq->SetBlackHolePosition( pSeq->GetPosition() );
		pSeq->UseLookPoint( true );
		pSeq->SetLookPoint( pSeq->GetPosition() );
		pSeq->SetLatency(0.3f);
	}
	pSeq = g_pData->GetUIMajorParticle()->CreateSequence( NULL,  L"HyperBallRing", pos.x, pos.y, 0.0f, 1000,1000, -1, 1 );
	if( pSeq != NULL )
	{
		pSeq->SetOverUI( true );
		pSeq->SetLatency(0.3f);
	}

}

void CX2MiniMapUI::SetMonsterNum( bool bShow, int iNum )
{
	CKTDGUIStatic* pStatic_Monster = (CKTDGUIStatic*) m_pDLGFrameWindow->GetControl( L"Static_Monster" );

	if(pStatic_Monster != NULL)
	{
		if(bShow)
		{
			WCHAR wszText[32] = L"";
			StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"%d", iNum );

			pStatic_Monster->GetString(0)->msg = wszText;
		}

		pStatic_Monster->SetShow(bShow);
	}

}


// 미니맵 하단에 "이 던전에서는 경험치를 얻을 수 없습니다"
void CX2MiniMapUI::SetNoExpAtThisDungeon(bool val)
{
//{{오현빈 // 2012-07-18 // 경험치 얻을 수 없다는 것을 표현하는 방법을 기획적으로 변경할 예정, 요청에 의해 제거만 하기 위해 일단 함수 내부만 제거
//}}오현빈 // 2012-07-18 // 경험치 얻을 수 없다는 것을 표현하는 방법을 기획적으로 변경할 예정, 요청에 의해 제거만 하기 위해 일단 함수 내부만 제거
}

//{{ kimhc // 2010.5.2 // 비밀던전 작업(경험치)
void CX2MiniMapUI::SetWarningForGettingExp( bool bVal )		// 비밀던전에서 경험치를 얻지 못할 수 도 있는 경우에 true
{
	//{{오현빈 // 2012-07-18 // 경험치 얻을 수 없다는 것을 표현하는 방법을 기획적으로 변경할 예정, 요청에 의해 제거만 하기 위해 일단 함수 내부만 제거
	//}}오현빈 // 2012-07-18 // 경험치 얻을 수 없다는 것을 표현하는 방법을 기획적으로 변경할 예정, 요청에 의해 제거만 하기 위해 일단 함수 내부만 제거
}
//}} kimhc // 2010.5.2 // 비밀던전 작업(경험치)

void CX2MiniMapUI::UpdateEnduranceNotice()
{
	CKTDGUIButton* pButton_Endurance = (CKTDGUIButton*) m_pDLGFrameWindow->GetControl( L"Button_Endurance" );
	if(NULL != pButton_Endurance )
	{
		pButton_Endurance->SetShowEnable(true, m_UIEnduranceChecker.Update());
		pButton_Endurance->SetGuideDesc( GET_STRING( STR_ID_365 ) );
		pButton_Endurance->SetGuideDescOffsetPos(D3DXVECTOR2(62.f, 0.f));
	}
}

void CX2MiniMapUI::SetShowEnduranceUI( bool bShow )
{
	m_UIEnduranceChecker.SetShow( bShow );
}


void CX2MiniMapUI::UpdateEventNotice()
{
	//{{ kimhc // 2009-12-04 // 크리스마스 이벤트
	CKTDGUIStatic* pStatic_EventSign = (CKTDGUIStatic*) m_pDLGFrameWindow->GetControl( L"Event_Sign" );
	CKTDGUIButton* pButton_EventTimer = (CKTDGUIButton*)m_pDLGFrameWindow->GetControl( L"Mini_Event" );

	// 미니맵 보이면 같이 보이는 거고 아니면 안 보이는 거고.. State 같은 건 신경 끄자
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		false == g_pData->GetMyUser()->GetSelectUnit()->GetReceivedGuestUserReward() /*&&
																					 (float)g_pData->GetMyUser()->GetSelectUnit()->GetTimerGuestUser().elapsed() < g_pData->GetMyUser()->GetSelectUnit()->GetMarketingEventTime()*/ )
	{
		CX2Unit*	pMyUnit		= g_pData->GetMyUser()->GetSelectUnit();
		std::multimap<int, wstring> mapMarketing; // < 남은 시간, 이벤트명 > // 남은시간은 같을 수 있음으로 멀티맵 사용
		std::wstringstream wstrStream;

		for( CX2Unit::MarketingEventMap::value_type& value : pMyUnit->m_mapEventData )
		{
			float fTime = value.second.m_kTimeEventSimpleInfo.m_fEventTime * 60.f - static_cast<float>( value.second.m_pElapsedTimer.elapsed() );
			int iMinute = static_cast<int>( (fTime+59.f)/ 60.f);
			if( iMinute < 0 )
				iMinute = 0;

			mapMarketing.insert( std::make_pair( iMinute, value.second.m_kTimeEventSimpleInfo.m_wstrEventName ) );
		}

		if(pStatic_EventSign != NULL)
		{
			pStatic_EventSign->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_366, "i", mapMarketing.rbegin()->first ) );
		}

		std::multimap<int, wstring>::reverse_iterator rMultimapItr = mapMarketing.rbegin(); // 많이 남은 시간 부터

		wstrStream << GET_REPLACED_STRING( ( STR_ID_366, "i", rMultimapItr->first ) ) << L" : " << rMultimapItr->second;
		rMultimapItr++;

		if ( rMultimapItr != mapMarketing.rend() )
		{
			do
			{
				wstrStream << L'\n' << GET_REPLACED_STRING( ( STR_ID_366, "i", rMultimapItr->first ) ) << L" : " << rMultimapItr->second;
				rMultimapItr++;

			} while ( rMultimapItr != mapMarketing.rend() );
		}

		if(pButton_EventTimer != NULL)
		{
			pButton_EventTimer->SetGuideDesc( wstrStream.str().c_str() );
			pButton_EventTimer->SetGuideDescOffsetPos(D3DXVECTOR2(25.f, 63.f));
			pButton_EventTimer->SetShowEnable(true, true);
		}
	}
	else
	{
		// 끈다
		if( NULL != pStatic_EventSign )
		{
			pStatic_EventSign->SetShow(false);
		}

		if( NULL != pButton_EventTimer )
		{
			pButton_EventTimer->SetShowEnable(true, false);
		}
	}
	//}} kimhc // 2009-12-04 // 크리스마스 이벤트
}


void CX2MiniMapUI::UpdateChannelInfo()
{
	if(m_bShow == false || g_pInstanceData == NULL)
		return;

	float CHANNEL_CONGESTION_SCOPE_TEMP[]	= { 0.1f, 0.2f, 0.5f, 0.99f, };
	CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );
	if(pComboBox != NULL)
	{
		//{{ kimhc // 2009-12-16 // 서버군 추가
		SetChannelComboBox();
		for( const KChannelInfo& channelInfo : g_pInstanceData->m_vecChannelInfo )
		{
			wstring wstrChannelName( L" " );
			wstrChannelName			+=	channelInfo.m_wstrChannelName;
			wstrChannelName			+=	L" ";

			std::map<int, KChannelBonusInfo>::const_iterator mit = g_pInstanceData->m_mapChannelBonusInfo.find(channelInfo.m_iChannelID);
			if(mit != g_pInstanceData->m_mapChannelBonusInfo.end() )
			{
				wstring wstrChannelLv = KncUtil::toWideString( x2::str(x2::format("(%1%-%2%)") % mit->second.m_iBeginLv % mit->second.m_iEndLv) );
				wstrChannelName += wstrChannelLv;
				wstrChannelName += L" ";
			}

			if ( g_pInstanceData == NULL )
			{
				ASSERT( L"Wrong Path!" );
				return;
			}


			float fChannelCongestionRate = static_cast< float >( channelInfo.m_iCurrentUser ) / static_cast< float >( channelInfo.m_iMaxUser );



			if(fChannelCongestionRate >= CHANNEL_CONGESTION_SCOPE_TEMP[3])
			{
				wstrChannelName += L"#Cff383f"; // FULL
				wstrChannelName += GET_STRING(STR_ID_724);
			}
			else if(fChannelCongestionRate > CHANNEL_CONGESTION_SCOPE_TEMP[2])
			{
				wstrChannelName += L"#Cf7941d"; // 혼잡
				wstrChannelName += GET_STRING(STR_ID_723);
			}
			else if(fChannelCongestionRate > CHANNEL_CONGESTION_SCOPE_TEMP[1])
			{
				wstrChannelName += L"#Cfff200"; // 보통
				wstrChannelName += GET_STRING(STR_ID_722);
			}
			else if(fChannelCongestionRate > CHANNEL_CONGESTION_SCOPE_TEMP[0])
			{
				wstrChannelName += L"#C93fe20"; // 원활
				wstrChannelName += GET_STRING(STR_ID_721);
			}
			else
			{
				wstrChannelName += L"#C93fe20"; // 원활
				wstrChannelName += GET_STRING(STR_ID_721);
			}

			//{{ kimhc // 2009-12-18 // 서버군 추가에 따른 ComboList의 Index 얻어오는 법 변경
			int iListId = GetListIndexFromChannelID( channelInfo.m_iChannelID );
			ZeroMemory( pComboBox->GetItem( iListId )->strText, sizeof(WCHAR)*256 );
			StringCchCopy( pComboBox->GetItem( iListId )->strText, 256, wstrChannelName.c_str() );
			//}} kimhc // 2009-12-18 // 서버군 추가에 따른 ComboList의 Index 얻어오는 법 변경
		}
		//}} kimhc // 2009-12-16 // 서버군 추가
	}
}

//{{ kimhc // 2009-12-15 // 서버군 추가 작업
void CX2MiniMapUI::SetChannelComboBox()
{
	m_vecChannelInfoInMinimap.resize( 0 );

	for( const KChannelInfo& kChannelInfo : g_pInstanceData->m_vecChannelInfo )
	{
		CHANNEL_INFO_IN_MINIMAP info( kChannelInfo.m_iChannelID, kChannelInfo.m_wstrChannelName );

		m_vecChannelInfoInMinimap.push_back( info );
	}
	std::sort( m_vecChannelInfoInMinimap.begin(), m_vecChannelInfoInMinimap.end() );

	// 테스트 용
	for( const CHANNEL_INFO_IN_MINIMAP& channelInfo : m_vecChannelInfoInMinimap )
	{
		int i = channelInfo.m_iChannelID;
		wstring wstr( channelInfo.m_wstrChannelName );
	}

	CKTDGUIComboBox* pComboBox = static_cast< CKTDGUIComboBox* >( m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" ) );
	pComboBox->RemoveAllItems();

	// 콤보박스에 아이템 한개당 높이 2.5로 셋팅

	const float fScaleResolutionY = g_pMain->GetGameOption().GetOptionList().m_vResolution.y / 768;

	UINT nHeight = (UINT)(m_vecChannelInfoInMinimap.size() * 3.5f * fScaleResolutionY );
	pComboBox->SetDropHeight(nHeight);
	//pComboBox->SetDropHeight( static_cast< UINT >( m_vecChannelInfoInMinimap.size() * 2.5 ) );

	for ( UINT i = 0; i < m_vecChannelInfoInMinimap.size(); ++i )
	{
		wstring temp = L" ";
		temp += m_vecChannelInfoInMinimap[i].m_wstrChannelName;

		pComboBox->AddItem( temp.c_str(), NULL, false );
	}

}

int	CX2MiniMapUI::GetListIndexFromChannelID( int iChannelID )
{
	for ( UINT i = 0; i < m_vecChannelInfoInMinimap.size(); i++ )
	{
		if ( m_vecChannelInfoInMinimap[i].m_iChannelID == iChannelID )
			return i;
	}

	return 0;
}

int CX2MiniMapUI::CHANNEL_INFO_IN_MINIMAP::GetNumRelatedWithChannelName() const
{
	// 밑의 채널 스트링은 정상동작 테스트 후 STRING_ID로 변경해야함!!!!
	// 리턴 하는 숫자들은 정렬에 사용되기 위한 값일 뿐

	// 임시 변경

	if ( m_wstrChannelName.find( L"루벤1", 0 ) != wstring::npos )
		return 0;
	else if ( m_wstrChannelName.find( L"루벤2", 0 ) != wstring::npos )
		return 1;
	else if ( m_wstrChannelName.find( L"루벤3", 0 ) != wstring::npos )
		return 2;
	else if ( m_wstrChannelName.find( L"엘더1", 0 ) != wstring::npos )
		return 3;
	else if ( m_wstrChannelName.find( L"엘더2", 0 ) != wstring::npos )
		return 4;
	else if ( m_wstrChannelName.find( L"베스마1", 0 ) != wstring::npos )
		return 5;
	else if ( m_wstrChannelName.find( L"베스마2", 0 ) != wstring::npos )
		return 6;
	else if ( m_wstrChannelName.find( L"알테라1", 0 ) != wstring::npos )
		return 7;
	else if ( m_wstrChannelName.find( L"알테라2", 0 ) != wstring::npos )
		return 8;
	else if ( m_wstrChannelName.find( L"벨더1", 0 ) != wstring::npos )
		return 9;
	else if ( m_wstrChannelName.find( L"벨더2", 0 ) != wstring::npos )
		return 10;
	else if ( m_wstrChannelName.find( L"벨더3", 0 ) != wstring::npos )
		return 11;
	else if ( m_wstrChannelName.find( L"하멜1", 0 ) != wstring::npos )
		return 12;
	else if ( m_wstrChannelName.find( L"하멜2", 0 ) != wstring::npos )
		return 13;
	else if ( m_wstrChannelName.find( L"하멜3", 0 ) != wstring::npos )
		return 14;

// 	if ( m_wstrChannelName.find( L"루벤", 0 ) != wstring::npos )
// 		return 0;
// 	else if ( m_wstrChannelName.find( L"엘더", 0 ) != wstring::npos )
// 		return 1;
// 	else if ( m_wstrChannelName.find( L"베스마", 0 ) != wstring::npos )
// 		return 2;
// 	else if ( m_wstrChannelName.find( L"알테라", 0 ) != wstring::npos )
// 		return 3;
// 	else if ( m_wstrChannelName.find( L"벨더", 0 ) != wstring::npos )
// 		return 4;

	return -1;		// 사내채널 등...
}

//}}  kimhc // 2009-12-15 // 서버군 추가 작업





void CX2MiniMapUI::SetEnableChannelComboBox( bool bVal_ )
{
	CKTDGUIComboBox* pComboBox = (CKTDGUIComboBox*) m_pDLGFrameWindow->GetControl( L"g_pComboBoxPresent_Select_User" );
	if( NULL != pComboBox )
	{
		pComboBox->SetEnable(bVal_);
	}
}
