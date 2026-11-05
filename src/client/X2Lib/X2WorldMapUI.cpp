#include "stdafx.h"
#include "./X2WorldMapUI.h"





CX2WorldMapUI::CX2WorldMapUI(void) :
m_pStage( NULL ),
m_pDLGWorldMap( NULL ),
m_pDLGWorldMapFront( NULL ),
m_pDLGFieldMap( NULL ),
m_pWorldFieldMap( NULL ),
m_pDLGNPCToolTip( NULL ),
m_pDLGPartyToolTip(NULL),
m_pDLGMiscToolTip( NULL ),
m_iCurrFieldMapID( 0 )
, m_bZoomed( false )
, m_bBeforeField( false )
{
}

CX2WorldMapUI::~CX2WorldMapUI(void)
{
	SAFE_DELETE_DIALOG( m_pDLGWorldMap );
	SAFE_DELETE_DIALOG( m_pDLGWorldMapFront );
	// SAFE_DELETE_DIALOG( m_pDLGFieldMap );	// m_mapDLGFieldMap�� SAFE_DELETE_DIALOG�� �����ϱ� ������ �ʿ� ����
	SAFE_DELETE_DIALOG( m_pDLGNPCToolTip );
	SAFE_DELETE_DIALOG( m_pDLGPartyToolTip );
	SAFE_DELETE_DIALOG( m_pDLGMiscToolTip );

	CX2WorldFieldMap::DeleteKTDGObject( (CKTDGObject*) m_pWorldFieldMap );

	map< int, CKTDGUIDialogType >::iterator it;
	for( it = m_mapDLGFieldMap.begin(); it != m_mapDLGFieldMap.end(); it++ )
	{
		SAFE_DELETE_DIALOG( it->second );
	}
	m_mapDLGFieldMap.clear();
}

/*virtual*/ bool CX2WorldMapUI::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(wParam)
	{
	case WMUCM_VILLAGE_CLICKED:
	case WMUCM_DUNGEON_GATE_CLICKED:
	case WMUCM_PREV_FIELD_MAP:
	case WMUCM_NEXT_FIELD_MAP:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iFieldID = pButton->GetDummyInt(0);

			OpenFieldMap( true, iFieldID );
			m_bZoomed = true;
			UpdateFieldMap();
			return true;
		} break;

	case WMUCM_CLOSE_WORLD_MAP:
		{
			OpenWorldMap( false );
			OpenFieldMap( false );
			CloseAllToolTip();

			m_bZoomed = true;
			return true;
		} break;

	case WMUCM_FIELD_RIGHT_CLICK:
		{
			int iFieldID = -1;
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_SQUARE_GAME:
				iFieldID = g_pData->GetLocationManager()->GetCurrentVillageID();
				break;
			case CX2Main::XS_BATTLE_FIELD:
				iFieldID = g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam();
				break;
			}

			OpenFieldMap( false, iFieldID );
			CloseAllToolTip();
			m_bZoomed = false;
			UpdateFieldMap();

			CKTDGUIDialogType pDialog = (CKTDGUIDialog*) lParam;

			if ( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( pDialog ) )
				m_bBeforeField = true;
			else
				m_bBeforeField = false;

			return true;
		}
		break;

	case WMUCM_WORLD_RIGHT_CLICK:
		{
			if ( false == m_bBeforeField )
			{
				OpenWorldMap( false );
				OpenFieldMap( false );
				CloseAllToolTip();

				m_bBeforeField = true;
				m_bZoomed = true;
			}
			else
			{
				m_bBeforeField = false;
			}

			return true;
		} break;

	case WMUCM_CLOSE_FIELD_MAP:
		{
			int iFieldID = -1;
			switch( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_VILLAGE_MAP:
			case CX2Main::XS_SQUARE_GAME:
				iFieldID = g_pData->GetLocationManager()->GetCurrentVillageID();
				break;
			case CX2Main::XS_BATTLE_FIELD:
				iFieldID = g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam();
				break;
			}

			OpenFieldMap( false, iFieldID );
			OpenWorldMap( false );
			CloseAllToolTip();
			m_bZoomed = false;
			UpdateFieldMap();
			return true;
		} break;

	case WMUCM_NPC_MOUSE_OVER:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iNPCID = pButton->GetDummyInt(0);

			OpenNPCToolTip( true, iNPCID, pButton->GetPos() + pButton->GetDialog()->GetPos() );
			return true;
		} break;

	case WMUCM_NPC_MOUSE_OUT:
		{
			OpenNPCToolTip( false );
			return true;
		} break;
		//{{ 2009.2.13 ���¿� : ��Ƽ�� ����ǥ��
	case WMUCM_MYCHAR_MOUSE_OVER:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;

			//			D3DXVECTOR2 vPos;
			// 			vPos.x = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos();
			// 			vPos.y = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos();
			// 			vPos = g_pKTDXApp->MouseConvertByResolution( vPos );

			OpenPartyMemberToolTip( true, -1, pButton->GetPos() + pButton->GetDialog()->GetPos() );

		} break;
	case WMUCM_PARTY_MEMBER_MOUSE_OVER:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iPartyIndex = pButton->GetDummyInt(0);

			OpenPartyMemberToolTip( true, iPartyIndex, pButton->GetPos() + pButton->GetDialog()->GetPos());

		} break;
	case WMUCM_MYCHAR_MOUSE_OUT:
	case WMUCM_PARTY_MEMBER_MOUSE_OUT:
		{
			OpenPartyMemberToolTip(false);

		} break;
		//}}
		//{{ 2009.02.12 ���¿�
		// Navi ���� �ڵ�
	case WMUCM_NAVI_LEFT_CLICKED:
		{
			CKTDGUINavigation* pNavi	= (CKTDGUINavigation*)lParam;
			D3DXVECTOR2 vRelativePos = pNavi->GetRelativeWindowPos();

			D3DXVECTOR2 vPos;

			if ( true == m_bZoomed )
			{
				map< int, CKTDGUIDialogType >::iterator it = m_mapDLGFieldMap.find( m_iCurrFieldMapID );
				if( it != m_mapDLGFieldMap.end() )
				{
					vPos = it->second->GetSize();

					vPos.x *= vRelativePos.x;
					vPos.y *= vRelativePos.y;

					vPos.x = 0.f - vPos.x;
					vPos.y = 0.f - vPos.y;

					it->second->SetPos( vPos );
				}
			}
			else
			{
				vPos = m_pDLGWorldMap->GetSize();

				vPos.x *= vRelativePos.x;
				vPos.y *= vRelativePos.y;

				vPos.x = 0.f - vPos.x;
				vPos.y = 0.f - vPos.y;

				m_pDLGWorldMap->SetPos( vPos );
			}

			return true;

		} break;
	case WMUCM_NAVI_LEFT_DRAGGING:
		{
			CKTDGUINavigation* pNavi	= (CKTDGUINavigation*)lParam;
			D3DXVECTOR2 vRelativePos = pNavi->GetRelativeWindowPos();

			D3DXVECTOR2 vPos;

			if ( true == m_bZoomed )
			{
				map< int, CKTDGUIDialogType >::iterator it = m_mapDLGFieldMap.find( m_iCurrFieldMapID );
				if( it != m_mapDLGFieldMap.end() )
				{
					vPos = it->second->GetSize();

					vPos.x *= vRelativePos.x;
					vPos.y *= vRelativePos.y;

					vPos.x = 0.f - vPos.x;
					vPos.y = 0.f - vPos.y;

					it->second->SetPos( vPos );
				}
			}
			else
			{
				vPos = m_pDLGWorldMap->GetSize();

				vPos.x *= vRelativePos.x;
				vPos.y *= vRelativePos.y;

				vPos.x = 0.f - vPos.x;
				vPos.y = 0.f - vPos.y;

				m_pDLGWorldMap->SetPos( vPos );
			}

			return true;

		} break;
	case WMUCM_NAVI_FIELD_MAP_STOP_DRAGGING:
		{
			if( m_pDLGWorldMapFront != NULL && m_pDLGWorldMap != NULL )
			{

				CKTDGUINavigation* pNavi = (CKTDGUINavigation*) m_pDLGWorldMapFront->GetControl( L"WorldMap_Navi" );
				if( NULL != pNavi )
				{
					D3DXVECTOR2 vPos;

					UpdateNaviMap( vPos, pNavi );

					pNavi->SetRelativeWindowPos( vPos );
				}
			}

			return true;

		} break;
	case WMUCM_NAVI_WORLD_MAP_STOP_DRAGGING:
		{
			if( m_pDLGWorldMapFront != NULL && m_pDLGWorldMap != NULL )
			{

				CKTDGUINavigation* pNavi = (CKTDGUINavigation*) m_pDLGWorldMapFront->GetControl( L"WorldMap_Navi" );
				if( NULL != pNavi )
				{
					D3DXVECTOR2 vPos( 0, 0 );
					UpdateNaviMap( vPos, pNavi );

					pNavi->SetRelativeWindowPos( vPos );
				}
			}

			return true;

		} break;
		//}}
	case WMUCM_POSTBOX_MOUSE_OVER:
	case WMUCM_MARKET_MOUSE_OVER:
		//{{ kimhc // 2010.5.4 // ��д��� �۾�(��������Ʈ)
		//}} kimhc // 2010.5.4 // ��д��� �۾�(��������Ʈ)

		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			OpenMiscToolTip( true, pButton->GetPos() + pButton->GetDialog()->GetPos(), (WORLD_MAP_UI_CUSTOM_MSG)wParam );
			return true;
		} break;
	case WMUCM_OTHER_FIELD_MOUSE_OVER:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iFieldID = pButton->GetDummyInt(0);
			OpenMiscToolTip( true, pButton->GetPos() + pButton->GetDialog()->GetPos(), (WORLD_MAP_UI_CUSTOM_MSG)wParam, iFieldID );
			return true;
		} break;

	case WMUCM_VILLAGE_MOUSE_OVER:
		{
			CKTDGUIButton* pButton = (CKTDGUIButton*) lParam;
			int iFieldID = pButton->GetDummyInt(0);
// 			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos(),
// 												(float)g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos() );
			OpenMiscToolTip( true, pButton->GetPos() + pButton->GetDialog()->GetPos(), (WORLD_MAP_UI_CUSTOM_MSG)wParam, iFieldID );

			return true;
		} break;
	case WMUCM_VILLAGE_MOUSE_OUT:
	case WMUCM_POSTBOX_MOUSE_OUT:
	case WMUCM_MARKET_MOUSE_OUT:
	case WMUCM_OTHER_FIELD_MOUSE_OUT:
		//{{ kimhc // 2010.5.4 // ��д��� �۾�(��������Ʈ)
		//}} kimhc // 2010.5.4 // ��д��� �۾�(��������Ʈ)

		{
			OpenMiscToolTip(false);
			return true;
		} break;
	}


	return false;
}

void CX2WorldMapUI::OpenWorldMap( bool bOpen )
{
	if( false == bOpen )
	{
		if( m_pDLGWorldMap != NULL )
		{
			m_pDLGWorldMap->SetShowEnable( bOpen, bOpen );
		}

		if( m_pDLGWorldMapFront != NULL )
		{
			m_pDLGWorldMapFront->SetShowEnable( bOpen, bOpen );
		}
		CloseAllToolTip();

		// ä��â ����
		g_pChatBox->SetChatBoxLayerUp(false);
		g_pChatBox->OpenChatWindow();
		g_pChatBox->CloseChatWindowButton();;
		g_pChatBox->HideChatEditBox();
	}
	else
	{
		if( NULL == m_pStage )
		{
			m_pStage = (CKTDXStage*) g_pMain->GetNowState();
		}


		if( m_pDLGWorldMap == NULL )
		{
			m_pDLGWorldMap = new CKTDGUIDialog( m_pStage, L"DLG_UI_World_Map_New.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGWorldMap );
		}

		if( m_pDLGWorldMapFront == NULL )
		{
			m_pDLGWorldMapFront = new CKTDGUIDialog( m_pStage, L"DLG_UI_World_Map_Front_NEW.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGWorldMapFront );
		}

		m_pDLGWorldMap->SetPos( D3DXVECTOR2( 0, 0 ) );
		m_pDLGWorldMap->SetShowEnable( bOpen, bOpen );
		m_pDLGWorldMapFront->SetShowEnable( bOpen, bOpen );

		// ä��â ����
		g_pChatBox->SetChatBoxLayerUp(true);
		g_pChatBox->HideChatWindowFrame();
		g_pChatBox->HideChatWindow();
		g_pChatBox->HideChatEditBox();
		g_pChatBox->CloseChatWindow();
		g_pChatBox->OpenChatWindowButton();

		// �� �˾� ����
		if ( NULL != g_pData->GetPetManager() )
		{
			CX2PET *pPet = g_pData->GetPetManager()->GetMyPet();

			if ( NULL != pPet && true == pPet->GetOnPopup() )
			{
				g_pData->GetPetManager()->ClosePetPopupMenu();
				pPet->SetOnPopup( false );
			}
		}
	}
}

void CX2WorldMapUI::UpdateWorldMap()
{
	// ���� �� �� ���� ������ Ŭ�� �ȵǰ�
	if( m_pDLGWorldMap != NULL )
	{
		//{{ 2009.02.12 ���¿� : ��Ƽ�� / �� ��ġ ǥ�� ����
		CKTDGUIButton* pButton_MyChar = (CKTDGUIButton*) m_pDLGWorldMap->GetControl( L"Button_Mychar" );
		CKTDGUIButton* pButton_PartyMember0 = (CKTDGUIButton*) m_pDLGWorldMap->GetControl( L"Button_PartyMember0" );
		CKTDGUIButton* pButton_PartyMember1 = (CKTDGUIButton*) m_pDLGWorldMap->GetControl( L"Button_PartyMember1" );
		CKTDGUIButton* pButton_PartyMember2 = (CKTDGUIButton*) m_pDLGWorldMap->GetControl( L"Button_PartyMember2" );
		if(pButton_MyChar != NULL)
			pButton_MyChar->SetShow(false);
		if(pButton_PartyMember0 != NULL)
			pButton_PartyMember0->SetShow(false);
		if(pButton_PartyMember1 != NULL)
			pButton_PartyMember1->SetShow(false);
		if(pButton_PartyMember2 != NULL)
			pButton_PartyMember2->SetShow(false);
		//}}

		for( int i=0; i<m_pDLGWorldMap->GetControlSize(); i++ )
		{
			CKTDGUIControl* pControl = m_pDLGWorldMap->GetControl( i );
			if( NULL == pControl )
				continue;

			if( CKTDGUIControl::UCT_BUTTON != pControl->GetType() )
				continue;

			CKTDGUIButton* pButton = (CKTDGUIButton*) pControl;
			if( WMUCM_VILLAGE_CLICKED != pButton->GetCustomMsgMouseUp() &&
				WMUCM_DUNGEON_GATE_CLICKED != pButton->GetCustomMsgMouseUp() )
			{
				continue;
			}

			if( -1 == pControl->GetDummyInt( 0 ) )
				continue;


			CX2LocationManager::LOCAL_MAP_ID eLocalMapID = g_pData->GetLocationManager()->GetLocalMapID( (SEnum::VILLAGE_MAP_ID) pControl->GetDummyInt( 0 ) );
			if( CX2LocationManager::LMI_INVALID == eLocalMapID )
				continue;

			CX2LocationManager::LocalMapTemplet* pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( eLocalMapID );
			if( NULL == pLocalMapTemplet )
			{
				continue;
			}



			float PosOffset = 0.0f;
			SEnum::VILLAGE_MAP_ID eCurrentMapID = SEnum::VMI_INVALID;

			if ( CX2Main::XS_VILLAGE_MAP == g_pMain->GetNowStateID() )
				eCurrentMapID = g_pData->GetLocationManager()->GetCurrentVillageID();
			else if ( CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() )
				eCurrentMapID = static_cast<SEnum::VILLAGE_MAP_ID>( g_pData->GetBattleFieldManager().GetBattleFieldIdWhereIam() );

			if( pControl->GetDummyInt(0) == eCurrentMapID )
			{
				D3DXVECTOR3 DummyPos = pControl->GetDummyPos(0);
				D3DXVECTOR2 CharMarkPos;
				CharMarkPos.x = DummyPos.x;
				CharMarkPos.y = DummyPos.y;
				pButton_MyChar->SetOffsetPos( CharMarkPos );
				pButton_MyChar->SetShow(true);
				PosOffset += (pButton_MyChar->GetWidth() + 1.0f);
			}

			// ��Ƽ�� ������
			if( g_pData->GetPartyManager()->DoIHaveParty() )
			{
				// ��Ƽ������ �ϳ��� ã�ƺ���..
				for(int i=0; i<3; i++ )
				{
					CX2PartyManager::PartyMemberData* pMemberData =
						g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( i, g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
					if(pMemberData != NULL)
					{
						// �ش� ��ID�� ��Ƽ���� ���� ��ID�� ���ؼ�
						SEnum::VILLAGE_MAP_ID eCurrentVillageID = (SEnum::VILLAGE_MAP_ID) pMemberData->m_iStateCode;
						// ����������� ��������Ʈ�� ��¦ �ٲ��ش�
						if(g_pData->GetLocationManager()->IsDungeonLounge(eCurrentVillageID))
						{
							CX2LocationManager::LOCAL_MAP_ID eLocalMapID = g_pData->GetLocationManager()->GetLocalMapID( eCurrentVillageID );
							eCurrentVillageID = g_pData->GetLocationManager()->GetDungeonGateID( eLocalMapID );
						}

						if( pControl->GetDummyInt(0) == eCurrentVillageID )
						{
							CKTDGUIButton* pButton_PartyMember;
							switch(i)
							{
							case 0:
								{
									pButton_PartyMember = pButton_PartyMember0;
								} break;
							case 1:
								{
									pButton_PartyMember = pButton_PartyMember1;
								} break;
							case 2:
								{
									pButton_PartyMember = pButton_PartyMember2;
								} break;

							}
							// ��ư�� �ش��ϴ� ������ ��Ƽ���� ������
							// �ش� ��ư�� �پ��ִ� DummyPos�� �����ͼ� �ű⿡ ĳ���� ��ũ�� �׸���
							D3DXVECTOR3 DummyPos = pControl->GetDummyPos(0);
							D3DXVECTOR2 CharMarkPos;
							CharMarkPos.x = DummyPos.x;
							CharMarkPos.y = DummyPos.y;
							CharMarkPos.x += PosOffset;
							// 							D3DXVECTOR2 CharMarkPos = pControl->GetNowPoint().leftTopPoint;
							// 							CharMarkPos.y -= ( pButton_PartyMember->GetHeight() + 1.0f);
							// 							CharMarkPos.x += PosOffset;
							pButton_PartyMember->SetOffsetPos( CharMarkPos );
							pButton_PartyMember->SetShow(true);
							PosOffset += (pButton_PartyMember->GetWidth() + 1.0f);
						}
					}
				}
			}
			//}}


			//if( pLocalMapTemplet->m_vecRequireClearDungeonID.size() > 0 )
			{
				if( /*false == g_pData->GetMyUser()->GetSelectUnit()->IsClearDungeon( pLocalMapTemplet->m_vecRequireClearDungeonID ) ||*/
					g_pData->GetSelectUnitLevel() < pLocalMapTemplet->m_RequireUnitLevel )
				{
					// �� �� ���� �����̴�..
					pControl->SetShow( false );
					// ���� ID�� ���� ����ƽ�� �� �ش�
					for( int i = 0; i < m_pDLGWorldMap->GetControlSize(); ++i )
					{
						CKTDGUIControl* pStaticControl = m_pDLGWorldMap->GetControl( i );
						if( NULL == pStaticControl )
							continue;

						if( CKTDGUIControl::UCT_STATIC != pStaticControl->GetType() )
							continue;

						if( pStaticControl->GetDummyInt(0) != pControl->GetDummyInt(0) )
							continue;

						if ( pStaticControl->GetDummyInt(1) == 1 )
						{
							pStaticControl->SetShow( true );
							continue;
						}

						pStaticControl->SetShow(false);

					}

				}
				else
				{
					// �� �� �ִ� �����̴�
					pControl->SetShow( true );
					// ���� ID�� ���� ����ƽ�� �� �ش�
					for( int i=0; i<m_pDLGWorldMap->GetControlSize(); i++ )
					{
						CKTDGUIControl* pStaticControl = m_pDLGWorldMap->GetControl( i );
						if( NULL == pStaticControl )
							continue;

						if( CKTDGUIControl::UCT_STATIC != pStaticControl->GetType() )
							continue;

						if( pStaticControl->GetDummyInt(0) != pControl->GetDummyInt(0) )
							continue;

						if ( pStaticControl->GetDummyInt(1) == 1 )
						{
							pStaticControl->SetShow( false );
							continue;
						}

						pStaticControl->SetShow(true);

					}
				}
			}


		}
	}

	//{{ 2009.02.12 ���¿�
	// �׺�����͸� �� ���� �� �ش���ġ�� �°� ������Ʈ
	if( m_pDLGWorldMapFront != NULL && m_pDLGWorldMap != NULL )
	{

		CKTDGUINavigation* pNavi = (CKTDGUINavigation*) m_pDLGWorldMapFront->GetControl( L"WorldMap_Navi" );
		if( NULL != pNavi )
		{
			D3DXVECTOR2 vPos( 0, 0 );
			UpdateNaviMap( vPos, pNavi );

			pNavi->SetRelativeWindowPos( vPos );
		}
	}
	//}}
}

CKTDGUIDialogType CX2WorldMapUI::GetFieldMapDlg( int iFieldID )
{
	if( iFieldID < 0 )
		return NULL;

	m_iCurrFieldMapID = iFieldID;
	m_pStage = g_pMain->GetNowState();

	map< int, CKTDGUIDialogType >::iterator it = m_mapDLGFieldMap.find( iFieldID );
	if( it != m_mapDLGFieldMap.end() )
	{
		return it->second;
	}

	CKTDGUIDialogType pDialog = NULL;

	// note!! �ϴ��� �ϵ��ڵ�
	switch( iFieldID )
	{
//#pragma region �纥
	case SEnum::VMI_RUBEN:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Ruben_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_RUBEN_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Ruben_01_NEW.lua" );
		break;
//#pragma endregion ����� Lua

//#pragma region ����
	case SEnum::VMI_ELDER:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Elder_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ELDER_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Elder_01_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ELDER_REST_00:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Elder_Rest_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ELDER_FIELD_02:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Elder_02_NEW.lua" );
		break;
//#pragma endregion ����� Lua

//#pragma region ������
	case SEnum::VMI_BESMA:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Besma_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Besma_01_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_BESMA_REST_00:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Besma_Rest_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_02:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Besma_02_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_03:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Besma_03_NEW.lua" );
		break;
//#pragma endregion ����� Lua

//#pragma region ���׶�
	case SEnum::VMI_ALTERA:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Altera_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Altera_01_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_02:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Altera_02_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Altera_Rest_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_03:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Altera_03_NEW.lua" );
		break;
//#pragma endregion ����� Lua

//#pragma region ����Ÿ
	case SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Peita_01_NEW.lua" );
		break;
	case SEnum::VMI_PEITA:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Peita_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_02:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Peita_02_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_03:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Peita_03_NEW.lua" );
		break;
//#pragma endregion ����� Lua

//#pragma region ����
	case SEnum::VMI_VELDER:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_01_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_02:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_02_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_VELDER_REST_00:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_Rest_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_03:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_03_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_04:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_04_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_VELDER_SHIP_STAGE:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Velder_Ship_NEW.lua" );
		break;
//#pragma endregion ����� Lua

//#pragma region �ϸ�
	case SEnum::VMI_BATTLE_FIELD_HAMEL_SHIP_STAGE:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_Ship_NEW.lua" );
		break;
	case SEnum::VMI_HAMEL:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_Village_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_01:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_01_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_02:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_02_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_Rest_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_03:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_03_NEW.lua" );
		break;
	case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_04:
		pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Hamel_04_NEW.lua" );
		break;
//#pragma endregion ����� Lua

	case SEnum::VMI_SANDER:
		{
			pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Sander_Village_NEW.lua" );
		} break;
	case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_01:
		{
			pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Sander_01_NEW.lua" );
		} break;
	case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_02:
		{
			pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Sander_02_NEW.lua" );
		} break;
	case SEnum::VMI_BATTLE_FIELD_SANDER_REST_00:
		{
			pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Sander_Rest_NEW.lua" );
		} break;

	case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_03:
		{
			pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Sander_03_NEW.lua" );
		} break;
	case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_04:
		{
			pDialog = new CKTDGUIDialog( m_pStage, L"DLG_UI_FIELD_MAP_Sander_04_NEW.lua" );
		} break;

	default:
		ASSERT( !L"iFieldID is Invalid" );
		pDialog = NULL;
		break;
	}

	if( pDialog != NULL )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( pDialog );
		m_mapDLGFieldMap.insert( make_pair( iFieldID, pDialog ) );
	}

	ASSERT( pDialog != NULL );

	m_bZoomed = true;

	return pDialog;
}

void CX2WorldMapUI::OpenFieldMap( bool bOpen, int iFieldID /*= -1*/ )
{
	// 09.03.10 ���¿� : ����������� ��� ���� ����
	if(true == g_pData->GetLocationManager()->IsDungeonLounge( (SEnum::VILLAGE_MAP_ID)iFieldID ) )
	{
		bOpen = false;
	}


	if( bOpen == true && NULL == m_pWorldFieldMap )
	{
		m_pWorldFieldMap = CX2WorldFieldMap::CreateWorldFieldMap();
		g_pKTDXApp->GetDGManager()->AddObjectChain( (CKTDGObject*) m_pWorldFieldMap );
	}


	if( NULL != m_pWorldFieldMap )
	{
		m_pWorldFieldMap->SetVillageID( iFieldID );
		m_pWorldFieldMap->SetShowObject( bOpen );
	}

	CloseAllToolTip();

	if( false == bOpen )
	{
		if( m_pDLGFieldMap != NULL )
		{
			m_pDLGFieldMap->SetShowEnable( bOpen, bOpen );
		}
	}
	else
	{
		if( iFieldID == m_iCurrFieldMapID &&
			m_pDLGFieldMap != NULL )
		{
			m_pDLGFieldMap->SetShowEnable( bOpen, bOpen );
		}
		else
		{
			if( m_pDLGFieldMap != NULL )
			{
				m_pDLGFieldMap->SetShowEnable( false, false );
				m_pDLGFieldMap = NULL;
			}

			m_pDLGFieldMap = GetFieldMapDlg( iFieldID );
			if( m_pDLGFieldMap != NULL )
			{
				m_pDLGFieldMap->SetShowEnable( bOpen, bOpen );
			}
		}
	}

	g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGWorldMapFront, true );
	if(m_pWorldFieldMap != NULL)
	{
		m_pWorldFieldMap->DestoryQuestNoticeEffect();

		if( m_pDLGFieldMap != NULL )
		{
			m_pWorldFieldMap->SetFieldDLG( m_pDLGFieldMap );
			m_pWorldFieldMap->InitCharacterMaker();
		}
	}
}

void CX2WorldMapUI::UpdateFieldMap()
{
	// ���� �� �� ���� ������ Ŭ�� �ȵǰ�
	if( m_pDLGFieldMap != NULL )
	{
		//{{ 2009.02.13 ���¿� : ��Ƽ�� / �� ��ġ ǥ�� ����
		/*CKTDGUIButton* pButton_MyChar = (CKTDGUIButton*) m_pDLGFieldMap->GetControl( L"Button_Mychar" );
		CKTDGUIButton* pButton_PartyMember0 = (CKTDGUIButton*) m_pDLGFieldMap->GetControl( L"Button_PartyMember0" );
		CKTDGUIButton* pButton_PartyMember1 = (CKTDGUIButton*) m_pDLGFieldMap->GetControl( L"Button_PartyMember1" );
		CKTDGUIButton* pButton_PartyMember2 = (CKTDGUIButton*) m_pDLGFieldMap->GetControl( L"Button_PartyMember2" );
		if(pButton_MyChar != NULL)
		pButton_MyChar->SetShow(false);
		if(pButton_PartyMember0 != NULL)
		pButton_PartyMember0->SetShow(false);
		if(pButton_PartyMember1 != NULL)
		pButton_PartyMember1->SetShow(false);
		if(pButton_PartyMember2 != NULL)
		pButton_PartyMember2->SetShow(false);*/
		//}}

		for( int i=0; i<m_pDLGFieldMap->GetControlSize(); i++ )
		{
			CKTDGUIControl* pControl = m_pDLGFieldMap->GetControl( i );
			if( NULL == pControl )
				continue;

			if( CKTDGUIControl::UCT_BUTTON != pControl->GetType() )
				continue;

			CKTDGUIButton* pButton = (CKTDGUIButton*) pControl;
			if( WMUCM_PREV_FIELD_MAP != pButton->GetCustomMsgMouseUp() &&
				WMUCM_NEXT_FIELD_MAP != pButton->GetCustomMsgMouseUp() &&
				WMUCM_DUNGEON_GATE_CLICKED != pButton->GetCustomMsgMouseUp() &&
				WMUCM_VILLAGE_CLICKED != pButton->GetCustomMsgMouseUp()
				)
			{
				continue;
			}

			if( -1 == pControl->GetDummyInt( 0 ) )
				continue;


			CX2LocationManager::LOCAL_MAP_ID eLocalMapID = g_pData->GetLocationManager()->GetLocalMapID( (SEnum::VILLAGE_MAP_ID) pControl->GetDummyInt( 0 ) );
			if( CX2LocationManager::LMI_INVALID == eLocalMapID )
			{
				continue;
			}

			CX2LocationManager::LocalMapTemplet* pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( eLocalMapID );
			if( NULL == pLocalMapTemplet )
			{
				continue;
			}


			//if( pLocalMapTemplet->m_vecRequireClearDungeonID.size() > 0 )
			{
				if( /*false == g_pData->GetMyUser()->GetSelectUnit()->IsClearDungeon( pLocalMapTemplet->m_vecRequireClearDungeonID ) ||*/
					g_pData->GetSelectUnitLevel() < pLocalMapTemplet->m_RequireUnitLevel )
				{
					pControl->SetEnable( false );
				}
				else
				{
					pControl->SetEnable( true );
				}
			}

			//{{ ��Ƽ��ǥ��
			/*if( WMUCM_DUNGEON_GATE_CLICKED == pButton->GetCustomMsgMouseUp() ||
				WMUCM_VILLAGE_CLICKED == pButton->GetCustomMsgMouseUp() )
			{
				float PosOffset = 0.0f;
				SEnum::VILLAGE_MAP_ID eCurrentVillageID = g_pData->GetLocationManager()->GetCurrentVillageID();
				// ����������� ��������Ʈ�� ��¦ �ٲ��ش�
				{
					CX2LocationManager::LOCAL_MAP_ID eLocalMapID = g_pData->GetLocationManager()->GetLocalMapID( eCurrentVillageID );
					eCurrentVillageID = g_pData->GetLocationManager()->GetDungeonGateID( eLocalMapID );
				}

				if(pControl->GetDummyInt(0) == eCurrentVillageID)
				{
					// ��ư�� �ش��ϴ� �����ʿ� ���� ������
					D3DXVECTOR2 CharMarkPos = pControl->GetNowPoint().leftTopPoint;
					CharMarkPos.y -= ( pButton_MyChar->GetHeight() + 1.0f);
					pButton_MyChar->SetOffsetPos( CharMarkPos );
					pButton_MyChar->SetShow(true);
					PosOffset += (pButton_MyChar->GetWidth() + 1.0f);
				}

				// ��Ƽ�� ������
				if( g_pData->GetPartyManager()->DoIHaveParty() )
				{
					// ��Ƽ������ �ϳ��� ã�ƺ���..
					for(int i=0; i<3; i++ )
					{
						CX2PartyManager::PartyMemberData* pMemberData =
							g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( i, g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
						if(pMemberData != NULL)
						{
							// �ش� ��ID�� ��Ƽ���� ���� ��ID�� ���ؼ�
							SEnum::VILLAGE_MAP_ID eCurrentVillageID = (SEnum::VILLAGE_MAP_ID) pMemberData->m_iStateCode;
							// ����������� ��������Ʈ�� ��¦ �ٲ��ش�
							if(g_pData->GetLocationManager()->IsDungeonLounge(eCurrentVillageID))
							{
								CX2LocationManager::LOCAL_MAP_ID eLocalMapID = g_pData->GetLocationManager()->GetLocalMapID( eCurrentVillageID );
								eCurrentVillageID = g_pData->GetLocationManager()->GetDungeonGateID( eLocalMapID );
							}
							if( pControl->GetDummyInt(0) == eCurrentVillageID )
							{
								CKTDGUIButton* pButton_PartyMember;
								switch(i)
								{
								case 0:
									{
										pButton_PartyMember = pButton_PartyMember0;
									} break;
								case 1:
									{
										pButton_PartyMember = pButton_PartyMember1;
									} break;
								case 2:
									{
										pButton_PartyMember = pButton_PartyMember2;
									} break;

								}
								D3DXVECTOR2 CharMarkPos = pControl->GetNowPoint().leftTopPoint;
								CharMarkPos.y -= ( pButton_PartyMember->GetHeight() + 1.0f);
								CharMarkPos.x += PosOffset;
								pButton_PartyMember->SetOffsetPos( CharMarkPos );
								pButton_PartyMember->SetShow(true);
								PosOffset += (pButton_PartyMember->GetWidth() + 1.0f);
							}
						}
					}
				}

			}*/
			//}}
		}

		if( m_pDLGWorldMapFront != NULL && m_pDLGWorldMap != NULL )
		{

			CKTDGUINavigation* pNavi = (CKTDGUINavigation*) m_pDLGWorldMapFront->GetControl( L"WorldMap_Navi" );
			if( NULL != pNavi )
			{
				D3DXVECTOR2 vPos;

				UpdateNaviMap( vPos, pNavi );
				pNavi->SetRelativeWindowPos( vPos );
			}
		}
	}
}

void CX2WorldMapUI::SetNpcToolTipImage( int iNPCID_)
{
	if( NULL == m_pDLGNPCToolTip )
		return;

	CKTDGUIStatic* pStatic_Image = (CKTDGUIStatic*) m_pDLGNPCToolTip->GetControl( L"Npc_Image" );
	if( NULL == pStatic_Image )
		return;

	if( NULL == pStatic_Image->GetPicture(0))

	pStatic_Image->SetShow(true);
	switch( iNPCID_ )
	{
	case CX2UnitManager::NUI_ANNE:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ANN" );
		break;
	case CX2UnitManager::NUI_HAGERS:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"HAGUS" );
		break;
	case CX2UnitManager::NUI_LOW:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ROU" );
		break;
	case CX2UnitManager::NUI_ADAMS:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ADAMS" );
		break;
	case CX2UnitManager::NUI_ECHO:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ECO" );
		break;
	case CX2UnitManager::NUI_HOFMANN:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"HOFMAN" );
		break;
	case CX2UnitManager::NUI_LUICHEL:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"RUICHEL" );
		break;
	case CX2UnitManager::NUI_LENPAD:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"RENPAT" );
		break;
	case CX2UnitManager::NUI_STELLA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"STELRA" );
		break;
	case CX2UnitManager::NUI_CHACHABUK:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"CHACHABUK" );
		break;
	case CX2UnitManager::NUI_RICHANG:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"RICHIANG" );
		break;
	case CX2UnitManager::NUI_TOMA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"TOMA" );
		break;
	case CX2UnitManager::NUI_ADEL:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ADEL" );
		break;
	case CX2UnitManager::NUI_AGATHA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"AGATA" );
		break;
	case CX2UnitManager::NUI_AIDA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"AIDA" );
		break;
	case CX2UnitManager::NUI_AMOS:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"AMOSE" );
		break;
	case CX2UnitManager::NUI_MOON:
		//pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ANN" );
		break;
	case CX2UnitManager::NUI_CAMILLA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"KAMILRA" );
		break;
	case CX2UnitManager::NUI_ARIEL:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ARIEL" );
		break;
	case CX2UnitManager::NUI_HELLEN:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"HELREN" );
		break;
	case CX2UnitManager::NUI_ALLEGRO:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"ALREGRO" );
		break;
	case CX2UnitManager::NUI_LENTO:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"RENTO" );
		break;
	case CX2UnitManager::NUI_GLAVE:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"Glave" );
		break;
	case CX2UnitManager::NUI_LURIEL:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"LURIEL" );
		break;
		//{{ ����� : [2009/10/27] //	NPC �� ����
	case CX2UnitManager::NUI_GRAIL:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"GRAIL" );
		break;
	case CX2UnitManager::NUI_NOEL:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"NOEL" );
		break;
	case CX2UnitManager::NUI_VANESSA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"VANESSA" );
		break;
	case CX2UnitManager::NUI_PRAUS:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"PRAUS" );
		break;
	case CX2UnitManager::NUI_HANNA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"HANNA" );
		break;
		// oasis907 : ����� [2011.1.27] // �ϸ�
	case CX2UnitManager::NUI_HORATIO:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"HORATIO" );
		break;
	case CX2UnitManager::NUI_LUCY:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"LUCY" );
		break;
	case CX2UnitManager::NUI_DAISY:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"DAISY" );
		break;
	case CX2UnitManager::NUI_PENENSIO:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"PENENSIO" );
		break;
	case CX2UnitManager::NUI_DENKA:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"DENKA" );
		break;
	case CX2UnitManager::NUI_LORDROS:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"LORDROS" );
		break;
	case CX2UnitManager::NUI_HELLPUTT:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"Default" );
		break;
	case CX2UnitManager::NUI_MU:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"MU" );
		break;
	case CX2UnitManager::NUI_LUTO_VILLAGE:
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"RUTO" );
		break;


	case CX2UnitManager::NUI_ROSEANG: // �Ǽ��縮 �����
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"ROSEANG" );
		break;

	case CX2UnitManager::NUI_EMIRATE: // ���� ���̸�Ʈ
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"EMIRATE" );
		break;

	case CX2UnitManager::NUI_VAPOR: // ���ݼ��� ������
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"VAPOR" );
		break;

	case CX2UnitManager::NUI_DAPPAR: // ����� ���ĸ�
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"DAPPAR" );
		break;

	case CX2UnitManager::NUI_EVENT_MOON_RABBIT: // �߼��̺�Ʈ ���䳢
		pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.tga", L"MoonRabbit" );
		break;
	default:
		{
			// NUI_PPORU_TOWN,		// �������� �̺�Ʈ NPC
			// NUI_BENDERS_TOWN,	// �������� �̺�Ʈ NPC

			//{{ kimhc // 2010.5.4 // ��д��� �۾�(����Ʈ)
			pStatic_Image->GetPicture(0)->SetTex( L"DLG_UI_Npc_Face01.TGA", L"Default" );
			//}} kimhc // 2010.5.4 // ��д��� �۾�(����Ʈ)
		} break;

	}
}
void CX2WorldMapUI::OpenNPCToolTip( bool bOpen, int iNPCID /*= -1*/, D3DXVECTOR2 vPos /*= D3DXVECTOR2(0, 0) */ )
{
	if( -1 == iNPCID )
		bOpen = false;

	const CX2UnitManager::NPCUnitTemplet* pNPCTemplet = g_pData->GetUnitManager()->GetNPCUnitTemplet( (CX2UnitManager::NPC_UNIT_ID) iNPCID );

	if( true == bOpen )
	{
		if( NULL == m_pStage )
			m_pStage = (CKTDXStage*) g_pMain->GetNowState();

		if( m_pDLGNPCToolTip == NULL )
		{
			m_pDLGNPCToolTip = new CKTDGUIDialog( m_pStage, L"DLG_UI_Npc_Info.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGNPCToolTip );
		}

		// ���� update
		SetNpcToolTipImage(iNPCID); //���� NPC �ʻ�ȭ ����


		CKTDGUIStatic* pStatic_Name 	= (CKTDGUIStatic*) m_pDLGNPCToolTip->GetControl( L"Npc_Name" );
		if( CX2UnitManager::NUI_NONE != iNPCID)
		{
			if( NULL != pNPCTemplet)
				pStatic_Name->GetString(0)->msg = pNPCTemplet->m_Name;
		}
		else//NPCID�� NONE�ϰ�� �������� �Ǵ�
		{
			pStatic_Name->GetString(0)->msg = GET_STRING(STR_ID_4521);
		}


		CKTDGUIStatic* pStatic_Desc 	= (CKTDGUIStatic*) m_pDLGNPCToolTip->GetControl( L"Npc_Desc" );

		// ����Ʈ ���� �ޱ�.
		wstring wstrDesc = L"";
		if( NULL != pNPCTemplet)
		{
			wstrDesc += pNPCTemplet->m_Description;
			wstrDesc += GetNPCQuestDesc( pNPCTemplet->m_nNPCUnitID );
		}
		else//NPCID�� NONE�ϰ�� �������� �Ǵ�
			wstrDesc += GET_STRING(STR_ID_4340);

		pStatic_Desc->GetString(0)->msg = wstrDesc;
		//{{ 2009.02.11 ���¿�
		// â ũ�� ������ ó������...
		CKTDGUIControl::UIStringData* pString = NULL;
		int ukFontID = 0;

		if ( pStatic_Desc != NULL )
		{
			pString = pStatic_Desc->GetString(0);
			if ( pString != NULL )
			{
				ukFontID = pString->fontIndex;
			}
		}

		const int MAGIC_DESC_ADD_SIZE_X = 25;
		// Y���� �� �� �þ ���� ������ ADD_SIZE_Y�� �� �˳��ϰ� ��� �д� : �̸� ����� �þ�� ���� �������� �ʾ���
		const int MAGIC_DESC_ADD_SIZE_Y = 40;
		CKTDGFontManager::CUKFont* pDescFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( ukFontID );
		int DescWidth = (int)( pDescFont->GetWidth( wstrDesc.c_str() ) / g_pKTDXApp->GetResolutionScaleX() ) + MAGIC_DESC_ADD_SIZE_X;
		int DescHeight = (int)( pDescFont->GetHeight( wstrDesc.c_str() ) / g_pKTDXApp->GetResolutionScaleY() )+ MAGIC_DESC_ADD_SIZE_Y;

		// �� ���� �ּҰ��� ������ �ش�. MinSize�� ������ Lua ���Ͽ� �ִ� ũ��
		CKTDGUIStatic* pStatic_BG	= (CKTDGUIStatic*) m_pDLGNPCToolTip->GetControl( L"Npc_Info" );
		const int MinSizeX = 130;
		const int MinSizeY = 52;

		// ���� ����
		int enterNum = 1;
		// ������ ���п�
		{
			int stringIndex = 0;
			wstring stringData = wstrDesc;


			while( 1 )
			{
				stringIndex = stringData.find( L"\n", stringIndex );
				if ( stringIndex == -1 )
					break;

				stringIndex++;
				enterNum++;
			}
		}


		DescWidth = max(DescWidth, MinSizeX);
		DescHeight = max(DescHeight+enterNum*2, MinSizeY);

		pStatic_BG->GetPicture(0)->SetSize( D3DXVECTOR2( (float)DescWidth, (float)DescHeight ) );

		// �׵θ� �� �׸���~
		pStatic_BG->GetPicture(1)->pPoint->leftTopPoint = D3DXVECTOR2(0,0);
		pStatic_BG->GetPicture(1)->SetSize( D3DXVECTOR2( (float)DescWidth+1, 2.0f));

		pStatic_BG->GetPicture(2)->pPoint->leftTopPoint = D3DXVECTOR2(0, (float)DescHeight);
		pStatic_BG->GetPicture(2)->SetSize( D3DXVECTOR2( (float)DescWidth+1, 2.0f));

		pStatic_BG->GetPicture(3)->pPoint->leftTopPoint = D3DXVECTOR2(0, 2.0f);
		pStatic_BG->GetPicture(3)->SetSize( D3DXVECTOR2( 2.0f, (float)DescHeight - 2));

		pStatic_BG->GetPicture(4)->pPoint->leftTopPoint = D3DXVECTOR2((float)DescWidth-1, 2.0f );
		pStatic_BG->GetPicture(4)->SetSize( D3DXVECTOR2( 2.0f, (float)DescHeight - 2 ));
		//}}

		// ��ġ�� ���� ����
		m_pDLGNPCToolTip->SetShowEnable( bOpen, bOpen );

		vPos = vPos + D3DXVECTOR2( 25,25 );
		if(vPos.x + DescWidth > 1024 )
		{
			vPos.x = (float)(1024 - DescWidth);
		}
		if(vPos.y + DescHeight > 768 )
		{
			vPos.y = (float)(768 - DescHeight);
		}
		m_pDLGNPCToolTip->SetPos( vPos );

	}

	if( m_pDLGNPCToolTip != NULL )
	{
		m_pDLGNPCToolTip->SetShowEnable( bOpen, bOpen );
	}

}

void CX2WorldMapUI::SetStage( CKTDXStage* pStage )
{
	m_pStage = pStage;

	if( m_pDLGWorldMap != NULL )
	{
		m_pDLGWorldMap->SetStage( pStage );
	}

	if( m_pDLGWorldMapFront != NULL )
	{
		m_pDLGWorldMapFront->SetStage( pStage );
	}

	if( m_pDLGFieldMap != NULL )
	{
		m_pDLGFieldMap->SetStage( pStage );
	}
	if( m_pDLGNPCToolTip != NULL )
	{
		m_pDLGNPCToolTip->SetStage( pStage );
	}
}

//{{ 2009.2.13 ���¿�
void CX2WorldMapUI::OpenPartyMemberToolTip( bool bOpen, int PartyMemberIndex /* = -1*/, D3DXVECTOR2 vPos /*= D3DXVECTOR2(0,0)*/ )
{
	if( false == bOpen )
	{
		if( NULL != m_pDLGPartyToolTip )
		{
			m_pDLGPartyToolTip->SetShowEnable( bOpen, bOpen );
		}
	}
	else
	{

		int iLevel = 0;
		wstring NickName = L"";
		CX2Unit::UNIT_CLASS eUnitClass = CX2Unit::UC_ELSWORD_KNIGHT;
		bool bPartyLeader = false;

		if(PartyMemberIndex == -1)
		{
			// ������
			const CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->GetUnitData();

			iLevel = pUnitData->m_Level;
			NickName = pUnitData->m_NickName;
			eUnitClass = pUnitData->m_UnitClass;
			if(g_pData->GetPartyManager()->DoIHaveParty())
			{
				CX2PartyManager::PartyMemberData* pMemberData = g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
				if( NULL != pMemberData )
				{
					bPartyLeader = pMemberData->m_bPartyLeader;
				}
			}
		}
		else
		{
			if(g_pData->GetPartyManager()->DoIHaveParty())
			{
				// ���� ����
				CX2PartyManager::PartyMemberData* pMemberData = g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( PartyMemberIndex );
				if(NULL != pMemberData)
				{
					iLevel = pMemberData->m_iUnitLevel;
					NickName = pMemberData->m_wstrNickName;
					eUnitClass = pMemberData->m_eUnitClass;
					bPartyLeader = pMemberData->m_bPartyLeader;
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}
		}


		if( NULL == m_pStage )
		{
			m_pStage = (CKTDXStage*) g_pMain->GetNowState();
		}

		if( m_pDLGPartyToolTip == NULL )
		{
			m_pDLGPartyToolTip = new CKTDGUIDialog( m_pStage, L"DLG_UI_Party_Comrade_Tool_Tip.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGPartyToolTip );
		}

		// ��ġ ���ϰ�, ���� update
		m_pDLGPartyToolTip->SetShowEnable( bOpen, bOpen );
		m_pDLGPartyToolTip->SetPos( vPos + D3DXVECTOR2( 25, 25 ) );


		CKTDGUIStatic* pStatic_Image	= (CKTDGUIStatic*) m_pDLGPartyToolTip->GetControl( L"Cha_Image" );
		CKTDGUIStatic* pStatic_Name 	= (CKTDGUIStatic*) m_pDLGPartyToolTip->GetControl( L"Cha_Name" );
		CKTDGUIStatic* pStatic_Level10 	= (CKTDGUIStatic*) m_pDLGPartyToolTip->GetControl( L"Cha_Level_10" );
		CKTDGUIStatic* pStatic_Level1 	= (CKTDGUIStatic*) m_pDLGPartyToolTip->GetControl( L"Cha_Level_1" );


		pStatic_Name->GetString(0)->msg = NickName.c_str();


		wstring fileName;
		wstring pieceName;
		if( true == CX2Data::GetCharacterImageName( fileName, pieceName, eUnitClass, CX2Data::CIT_20by20 ) )
		{
			pStatic_Image->GetPicture(0)->SetTex( fileName.c_str(), pieceName.c_str() );
		}
		else
		{
			ASSERT( !"NO" );
		}


		for(int i=0; i<10; i++)
		{
			if(iLevel/10 == i)
			{
				pStatic_Level10->GetPicture(i)->SetShow(true);
			}
			else
			{
				pStatic_Level10->GetPicture(i)->SetShow(false);
			}

			if(iLevel%10 == i)
			{
				pStatic_Level1->GetPicture(i)->SetShow(true);
			}
			else
			{
				pStatic_Level1->GetPicture(i)->SetShow(false);
			}
		}
	}

}
//}}

bool CX2WorldMapUI::GetShow()
{
	if(m_pDLGWorldMap != NULL)
	{
		return m_pDLGWorldMap->GetShow();
	}
	return false;
}

// 2009.03.11 ���¿�
// msg : �ش��ϴ� MouseUp Msg�� �̿��ؼ� � message�� �־���� �� �� �����Ѵ�.
void CX2WorldMapUI::OpenMiscToolTip(bool bOpen, D3DXVECTOR2 vPos, WORLD_MAP_UI_CUSTOM_MSG msg, int iFieldID )
{

	wstring wstrName = L"";
	wstring wstrDesc = L"";

	switch(msg)
	{
	case WMUCM_POSTBOX_MOUSE_OVER:
		{
			wstrName += GET_STRING( STR_ID_754 );
			wstrDesc += GET_STRING( STR_ID_942 );
		} break;
	case WMUCM_MARKET_MOUSE_OVER:
		{
			wstrName += GET_STRING( STR_ID_943 );
			wstrDesc += GET_STRING( STR_ID_944 );
		} break;
	case WMUCM_OTHER_FIELD_MOUSE_OVER:
		{
			wstrName += GET_STRING( STR_ID_945 );

			CX2LocationManager::VillageTemplet* pVillageTemplet = g_pData->GetLocationManager()->GetVillageMapTemplet( (SEnum::VILLAGE_MAP_ID) iFieldID );

			if ( NULL != pVillageTemplet )
			{
				wstrDesc += GET_REPLACED_STRING( ( STR_ID_948, "LI",
					pVillageTemplet->m_Name,
					KHanSungPostWordUnicodeWrapper( pVillageTemplet->m_Name.c_str(), STR_ID_946, STR_ID_947 ) ) );
			}
			else
			{
				CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();

				if ( NULL != battleFieldManager.GetBattleFieldNameByBattleFieldId( iFieldID ) )
				{
					wstring wstr = battleFieldManager.GetBattleFieldNameByBattleFieldId( iFieldID );

					wstrDesc += GET_REPLACED_STRING( ( STR_ID_948, "LI", wstr, KHanSungPostWordUnicodeWrapper( wstr.c_str(), STR_ID_946, STR_ID_947 ) ) );
				}
			}

		} break;
		//{{ kimhc // 2010.5.4 // ��д��� �۾�(��������Ʈ)
	case WMUCM_BILLBOARD_MOUSE_OVER:
		{
			wstrName += GET_STRING( STR_ID_3723 );
			wstrDesc += GET_STRING( STR_ID_3844 );
		} break;

		//}} kimhc // 2010.5.4 // ��д��� �۾�(��������Ʈ)

	case WMUCM_VILLAGE_MOUSE_OVER:
		{
			CX2LocationManager::VillageTemplet* pVillageTemplet = g_pData->GetLocationManager()->GetVillageMapTemplet( (SEnum::VILLAGE_MAP_ID) iFieldID );

			if ( NULL != pVillageTemplet )
			{
				wstrName += pVillageTemplet->m_Name.c_str();
				wstrDesc += pVillageTemplet->m_Name.c_str();
			}

			CX2BattleFieldManager& battleFieldManager = g_pData->GetBattleFieldManager();
			const WCHAR* pName = battleFieldManager.GetBattleFieldNameByBattleFieldId( iFieldID );

			if ( NULL != pName )
			{
				wstrName += pName;
				wstrDesc += pName;
			}

		} break;

	default:
		{
			bOpen = false;
		} break;
	}

	if( false == bOpen )
	{
		if( m_pDLGMiscToolTip != NULL )
		{
			m_pDLGMiscToolTip->SetShowEnable( bOpen, bOpen );
		}
	}
	else
	{
		if( NULL == m_pStage )
		{
			m_pStage = (CKTDXStage*) g_pMain->GetNowState();
		}

		if( m_pDLGMiscToolTip == NULL )
		{
			m_pDLGMiscToolTip = new CKTDGUIDialog( m_pStage, L"DLG_UI_Misc_Info.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMiscToolTip );
		}

		CKTDGUIStatic* pStatic_Name 	= (CKTDGUIStatic*) m_pDLGMiscToolTip->GetControl( L"Misc_Name" );
		CKTDGUIStatic* pStatic_Desc 	= (CKTDGUIStatic*) m_pDLGMiscToolTip->GetControl( L"Misc_Desc" );

		pStatic_Name->GetString(0)->msg = wstrName.c_str();
		pStatic_Desc->GetString(0)->msg = wstrDesc.c_str();
		//{{ 2009.02.11 ���¿�
		// â ũ�� ������ ó������...
		CKTDGUIControl::UIStringData* pString = NULL;
		int ukFontID = 0;

		if ( pStatic_Desc != NULL )
		{
			pString = pStatic_Desc->GetString(0);
			if ( pString != NULL )
			{
				ukFontID = pString->fontIndex;
			}
		}

		const int MAGIC_DESC_ADD_SIZE_X = 25;
		// Y���� �� �� �þ ���� ������ ADD_SIZE_Y�� �� �˳��ϰ� ��� �д� : �̸� ����� �þ�� ���� �������� �ʾ���
		const int MAGIC_DESC_ADD_SIZE_Y = 40;
		CKTDGFontManager::CUKFont* pDescFont = g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( ukFontID );
		int DescWidth = (int)( pDescFont->GetWidth( wstrDesc.c_str() ) / g_pKTDXApp->GetResolutionScaleX() ) + MAGIC_DESC_ADD_SIZE_X;
		int DescHeight = (int)( pDescFont->GetHeight( wstrDesc.c_str() ) / g_pKTDXApp->GetResolutionScaleY() )+ MAGIC_DESC_ADD_SIZE_Y;

		// �� ���� �ּҰ��� ������ �ش�. MinSize�� ������ Lua ���Ͽ� �ִ� ũ��
		CKTDGUIStatic* pStatic_BG	= (CKTDGUIStatic*) m_pDLGMiscToolTip->GetControl( L"Misc_Info" );
		const int MinSizeX = 130;
		const int MinSizeY = 52;
		DescWidth = max(DescWidth, MinSizeX);
		DescHeight = max(DescHeight, MinSizeY);

		pStatic_BG->GetPicture(0)->SetSize( D3DXVECTOR2( (float)DescWidth, (float)DescHeight ) );

		// �׵θ� �� �׸���~
		pStatic_BG->GetPicture(1)->pPoint->leftTopPoint = D3DXVECTOR2(0,0);
		pStatic_BG->GetPicture(1)->SetSize( D3DXVECTOR2( (float)DescWidth+1, 2.0f));

		pStatic_BG->GetPicture(2)->pPoint->leftTopPoint = D3DXVECTOR2(0, (float)DescHeight);
		pStatic_BG->GetPicture(2)->SetSize( D3DXVECTOR2( (float)DescWidth+1, 2.0f));

		pStatic_BG->GetPicture(3)->pPoint->leftTopPoint = D3DXVECTOR2(0, 2.0f);
		pStatic_BG->GetPicture(3)->SetSize( D3DXVECTOR2( 2.0f, (float)DescHeight - 2));

		pStatic_BG->GetPicture(4)->pPoint->leftTopPoint = D3DXVECTOR2((float)DescWidth-1, 2.0f );
		pStatic_BG->GetPicture(4)->SetSize( D3DXVECTOR2( 2.0f, (float)DescHeight - 2 ));
		//}}

		// Position ��ġ ����ó�� �� �ش�.
		if(vPos.x + DescWidth > 1024.f)
		{
			vPos.x = 1024.f - DescWidth - 30.f;
		}
		if(vPos.y + DescHeight > 768.f)
		{
			vPos.y = 768.f - DescWidth - 30.f;
		}

		// ��ġ ���ϰ�, ���� update
		m_pDLGMiscToolTip->SetShowEnable( bOpen, bOpen );
		m_pDLGMiscToolTip->SetPos( vPos + D3DXVECTOR2( 25, 25 ) );
	}

}

void CX2WorldMapUI::CloseAllToolTip()
{
	OpenMiscToolTip(false);
	OpenNPCToolTip(false);
	OpenPartyMemberToolTip(false);
}

wstring CX2WorldMapUI::GetNPCQuestDesc( CX2UnitManager::NPC_UNIT_ID eNPCID )
{
	std::multimap<int, const CX2QuestManager::QuestTemplet*> mapQuest;

	vector<int> vecQuestID;
	g_pData->GetQuestManager()->GetAvailableQuest( eNPCID, vecQuestID );

	// ���� ���� �ʴ� ����Ʈ ����� ��������
	vector<int> vecChangeJobQuestID;
	vector<int> vecEventQuestID;
	vector<int> vecNotAssignedQuestID;

	for( UINT i=0; i<vecQuestID.size(); i++ )
	{
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( vecQuestID[i] );
		if( NULL == pQuestTemplet )
			continue;

		const CX2QuestManager::QuestInst* pQuestInst = g_pData->GetQuestManager()->GetUnitQuest( vecQuestID[i] );

		//{{ kimhc // 2010.2.1 //	PC���� �ƴѰ������� ���� ���� PC������Ʈ ��Ͽ��� ������ �ʵ���
		if ( pQuestTemplet->m_bIsPcBang == true && g_pData->GetMyUser()->GetIsPCRoom() == false
			&& pQuestInst == NULL )
			continue;
		//}} kimhc // 2010.2.1 //	PC���� �ƴѰ������� ���� ���� PC������Ʈ ��Ͽ��� ������ �ʵ���

		//{{ kimhc // 2010.02.09 // �����ð����� ����Ʈ�� ������Ʈ �Ǵ� �ý��� ����
		if ( pQuestTemplet->m_bIsTimeEvent == true )
			continue;
		//}} kimhc // 2010.02.09 // �����ð����� ����Ʈ�� ������Ʈ �Ǵ� �ý��� ����


		if( CX2QuestManager::QT_CHANGE_JOB == pQuestTemplet->m_eQuestType && NULL == pQuestInst )
		{
			vecChangeJobQuestID.push_back( vecQuestID[i] );
		}
		else if( ( CX2QuestManager::QT_EVENT == pQuestTemplet->m_eQuestType
			//{{ kimhc // 2010.2.1 //	PC�� ����Ʈ
			|| CX2QuestManager::QT_PCBANG == pQuestTemplet->m_eQuestType
			//}} kimhc // 2010.2.1 //	PC�� ����Ʈ
			)
			&& NULL == pQuestInst )
		{
			vecEventQuestID.push_back( vecQuestID[i] );
		}
		else
		{
			int iLevelDiff = g_pData->GetSelectUnitLevel() - pQuestTemplet->m_iFairLevel;
			if( iLevelDiff > 5 )
			{
				//{{ kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
				switch ( pQuestTemplet->m_eRepeatType )
				{
				case CX2QuestManager::QRT_REPEAT:
				case CX2QuestManager::QRT_DAY:
					vecNotAssignedQuestID.push_back( vecQuestID[i] );
					break;

				default:
					break;
				}
				//}} kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
			}
			else
			{
				vecNotAssignedQuestID.push_back( vecQuestID[i] );
			}
		}
	}

	// ���� ǥ������ ����Ʈ�� �����ϴ��� Ȯ���Ѵ�
	if( vecNotAssignedQuestID.empty() == true &&
		vecEventQuestID.empty() == true &&
		vecChangeJobQuestID.empty() == true )
	{
		return L"";
	}


	// NotAssignedQuestID�� �ٽ� �������ش�
	for(vector<int>::iterator it = vecNotAssignedQuestID.begin(); it < vecNotAssignedQuestID.end(); ++it)
	{
		const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( *it );
		if( pQuestTemplet != NULL )
		{
			int DungeonID;

			// �ش� ���� ���̵� ���ؿ��� : ù��° ��������Ʈ�� �������� ��. ���� ���� �������� �����ؾ� �ϴ� ���� �������� ����.
			// ������ or �̺�Ʈ���� Ư���� �з��Ѵ�
			if( pQuestTemplet->m_vecSubQuest.size() > 0 &&
				( pQuestTemplet->m_eQuestType == CX2QuestManager::QT_NORMAL || pQuestTemplet->m_eQuestType == CX2QuestManager::QT_SKILL ) )
			{
				int SubQuestID = pQuestTemplet->m_vecSubQuest[0];
				const CX2QuestManager::SubQuestTemplet* pSubQuestTemplet = g_pData->GetQuestManager()->GetSubQuestTemplet( SubQuestID );
				if(pSubQuestTemplet != NULL )
				{
					switch( pSubQuestTemplet->m_eClearType )
					{
					case CX2QuestManager::SQT_NPC_HUNT:
					case CX2QuestManager::SQT_QUEST_ITEM_COLLECTION:
					case CX2QuestManager::SQT_DUNGEON_TIME:
					case CX2QuestManager::SQT_DUNGEON_RANK:
					case CX2QuestManager::SQT_DUNGEON_DAMAGE:
					case CX2QuestManager::SQT_DUNGEON_CLEAR_COUNT:
					case CX2QuestManager::SQT_WITH_DIF_SERV_USER:
					case CX2QuestManager::SQT_SUITABLE_LEVEL_DUNGEON_CLEAR:
						{
							// DID�� normal-hard-expert�� �پ� �ְ�, 30000���� �����ϰ�, �� �������� 10������ �з��Ǿ� �ִ�.
							// ���� �׳� 10�ڸ��� �ڸ� ���� �־� �ش�.
							// �ش� ������ �����ϴ� �����̸� ���� ID�� �����ְ�
							if( true == pSubQuestTemplet->m_ClearCondition.m_setDungeonID.empty() )
							{
								std::set<SEnum::DUNGEON_ID>::iterator it = pSubQuestTemplet->m_ClearCondition.m_setDungeonID.begin();
								if( NULL != g_pData->GetDungeonManager()->GetDungeonData( *it ) )
								{
									DungeonID = (int)(*it / 10) * 10;
								}
								else
								{
									DungeonID = SEnum::DI_END;
								}
							}
							else
							{
								// �ƴϸ� �Ϲ����� ���´�
								DungeonID = SEnum::DI_END;
							}
						}
						break;
					case CX2QuestManager::SQT_NPC_TALK:
					case CX2QuestManager::SQT_ITEM_COLLECTION:
					case CX2QuestManager::SQT_PVP_PLAY:
					case CX2QuestManager::SQT_PVP_WIN:
					case CX2QuestManager::SQT_PVP_KILL:
					case CX2QuestManager::SQT_PVP_NPC_HUNT:
					case CX2QuestManager::SQT_PVP_HERO_NPC_KILL:
					case CX2QuestManager::SQT_PVP_HERO_NPC_PLAY:

					default:
						{
							// ����
							DungeonID = SEnum::DI_END;
						} break;
					}
				}
				else
				{
					// �ƿ� �ش� ��������Ʈ�� ����
					DungeonID = SEnum::DI_END;
				}
			}
			else
			{
				DungeonID = SEnum::DI_NONE;
			}
			mapQuest.insert(std::make_pair(DungeonID, pQuestTemplet));
		}
	}

	// ������� ������ ����Ʈ�� ������/�������� �з��� �Ǿ�����

	wstring returnStr = L"";
	returnStr += GET_STRING( STR_ID_949 );
	int CurrentDID = -1;
	for( std::multimap<int, const CX2QuestManager::QuestTemplet*>::iterator it = mapQuest.begin(); it != mapQuest.end(); ++it )
	{
		if(CurrentDID != it->first)
		{
			CurrentDID = it->first;

			if(CurrentDID == SEnum::DI_END)
			{
				returnStr += L"+ ";
				returnStr += GET_STRING( STR_ID_950 );
				returnStr += L"\n";
			}
			else if(CurrentDID ==SEnum::DI_NONE)
			{
				returnStr += L"+ ";
				returnStr += GET_STRING( STR_ID_890 );
				returnStr += L"\n";
			}
			else if(NULL != g_pData->GetDungeonManager()->GetDungeonData((SEnum::DUNGEON_ID)CurrentDID) )
			{
				returnStr += L"+ ";
				returnStr += g_pData->GetDungeonManager()->GetDungeonData((SEnum::DUNGEON_ID)CurrentDID)->m_DungeonName;
				returnStr += L"\n";
			}
			else
			{
				ASSERT( !"���� �� ã����" );
			}

		}
		const CX2QuestManager::QuestTemplet* pQuestTemplet = it->second;
		returnStr += L" ";
		returnStr += g_pData->GetQuestManager()->GetQuestColorString( pQuestTemplet->m_iID, g_pData->GetSelectUnitLevel());
		returnStr += pQuestTemplet->m_wstrTitle;

		//{{ kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
		switch ( pQuestTemplet->m_eRepeatType )
		{
		case CX2QuestManager::QRT_REPEAT:
			{
				returnStr += L" (";
				returnStr += GET_STRING( STR_ID_951 );
				returnStr += L")";
			}
			break;

		case CX2QuestManager::QRT_DAY:
			{
				returnStr += L" (";
				returnStr += GET_STRING( STR_ID_5114 );
				returnStr += L")";
			}
			break;

		default:
			break;
		}

		returnStr += L"#CX\n";
		//}} kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
	}

	if(false == vecEventQuestID.empty())
	{
		returnStr += L"+ ";
		returnStr += GET_STRING( STR_ID_952 );
		returnStr += L"\n";
		for(vector<int>::iterator it = vecEventQuestID.begin(); it < vecEventQuestID.end(); ++it)
		{
			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( *it );
			returnStr += L" ";
			//returnStr += g_pData->GetQuestManager()->GetQuestColorString( pQuestTemplet->m_iID, g_pData->GetSelectUnitLevel());
			returnStr += L"#CFF8000";
			returnStr += pQuestTemplet->m_wstrTitle;

			//{{ kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
			switch ( pQuestTemplet->m_eRepeatType )
			{
			case CX2QuestManager::QRT_REPEAT:
				{
					returnStr += L" (";
					returnStr += GET_STRING( STR_ID_951 );
					returnStr += L")";
				}
				break;

			case CX2QuestManager::QRT_DAY:
				{
					returnStr += L" (";
					returnStr += GET_STRING( STR_ID_5114 );
					returnStr += L")";
				}
				break;

			default:
				break;
			}

			returnStr += L"#CX\n";
			//}} kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
		}
	}

	if(false == vecChangeJobQuestID.empty())
	{
		returnStr += L"+ ";
		returnStr += GET_STRING( STR_ID_953 );
		returnStr += L"\n";
		for(vector<int>::iterator it = vecChangeJobQuestID.begin(); it < vecChangeJobQuestID.end(); ++it)
		{
			const CX2QuestManager::QuestTemplet* pQuestTemplet = g_pData->GetQuestManager()->GetQuestTemplet( *it );
			returnStr += L" ";
			//returnStr += g_pData->GetQuestManager()->GetQuestColorString( pQuestTemplet->m_iID, g_pData->GetSelectUnitLevel());
			returnStr += L"#C6E6EFF";
			returnStr += pQuestTemplet->m_wstrTitle;

			//{{ kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)
			switch ( pQuestTemplet->m_eRepeatType )
			{
			case CX2QuestManager::QRT_REPEAT:
				{
					returnStr += L" (";
					returnStr += GET_STRING( STR_ID_951 );
					returnStr += L")";
				}
				break;

			case CX2QuestManager::QRT_DAY:
				{
					returnStr += L" (";
					returnStr += GET_STRING( STR_ID_5114 );
					returnStr += L")";
				}
				break;

			default:
				break;
			}

			returnStr += L"#CX\n";
			//}} kimhc // 2010.3.30 // ��д��� �۾�(��������Ʈ �ý���)

		}
	}

	return returnStr;

}

void CX2WorldMapUI::UpdateNaviMap( IN D3DXVECTOR2& vPos_, IN CKTDGUINavigation* pNavi_ )
{
	CKTDGUIStatic* pStaticWorld = NULL;
	CKTDGUIStatic* pStaticField = NULL;

	if ( NULL != m_pDLGWorldMapFront )
	{
		pStaticWorld = (CKTDGUIStatic*)m_pDLGWorldMapFront->GetControl( L"Static_World_Remarks" );
		pStaticField = (CKTDGUIStatic*)m_pDLGWorldMapFront->GetControl( L"Static_Field_Remarks" );
	}

	if ( true == m_bZoomed )
	{
		if ( NULL != pStaticWorld )
			pStaticWorld->SetShow( false );

		if ( NULL != pStaticField )
			pStaticField->SetShow( true );

		map< int, CKTDGUIDialogType >::iterator it = m_mapDLGFieldMap.find( m_iCurrFieldMapID );
		if( it != m_mapDLGFieldMap.end() )
		{
			vPos_ = it->second->GetPos();
			vPos_.x /= -it->second->GetSize().x;
			vPos_.y /= -it->second->GetSize().y;

			switch ( m_iCurrFieldMapID )
			{
//#pragma region �纥
			case SEnum::VMI_RUBEN:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Ruben" );
				break;

			case SEnum::VMI_BATTLE_FIELD_RUBEN_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Ruben_01" );
				break;
//#pragma endregion �׺���̼�

//#pragma region ����
			case SEnum::VMI_ELDER:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Elder" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ELDER_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Elder_01" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ELDER_REST_00:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Rest.TGA", L"FieldMap_Navigation_Elder_R" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ELDER_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Elder_02" );
				break;
//#pragma endregion �׺���̼�

//#pragma region ������
			case SEnum::VMI_BESMA:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Besma" );
				break;
			case SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Besma_01" );
				break;
			case SEnum::VMI_BATTLE_FIELD_BESMA_REST_00:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Rest.TGA", L"FieldMap_Navigation_Besma_R" );
				break;
			case SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Besma_02" );
				break;
			case SEnum::VMI_BATTLE_FIELD_BESMA_FIELD_03:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Besma_03" );
				break;
//#pragma endregion �׺���̼�

//#pragma region ���׶�
			case SEnum::VMI_ALTERA:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Altera" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Altera_01" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_00.TGA", L"FieldMap_Navigation_Altera_02" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Rest.TGA", L"FieldMap_Navigation_Altera_R" );
				break;
			case SEnum::VMI_BATTLE_FIELD_ALTERA_FIELD_03:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Altera_03" );
				break;
//#pragma endregion �׺���̼�

//#pragma region ����Ÿ
			case SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Peita_01" );
				break;
			case SEnum::VMI_PEITA:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Peita" );
				break;
			case SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Peita_02" );
				break;
			case SEnum::VMI_BATTLE_FIELD_PEITA_FIELD_03:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Peita_03" );
				break;
//#pragma endregion �׺���̼�

//#pragma region ����
			case SEnum::VMI_VELDER:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Velder" );
				break;
			case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Velder_01" );
				break;
			case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Velder_02" );
				break;
			case SEnum::VMI_BATTLE_FIELD_VELDER_REST_00:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Rest.TGA", L"FieldMap_Navigation_Velder_R" );
				break;
			case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_03:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Velder_03" );
				break;
			case SEnum::VMI_BATTLE_FIELD_VELDER_FIELD_04:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_01.TGA", L"FieldMap_Navigation_Velder_04" );
				break;
			case SEnum::VMI_BATTLE_FIELD_VELDER_SHIP_STAGE:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Velder_Ship" );
				break;
//#pragma endregion �׺���̼�

//#pragma region �ϸ�
			case SEnum::VMI_BATTLE_FIELD_HAMEL_SHIP_STAGE:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Hamel_Ship" );
				break;
			case SEnum::VMI_HAMEL:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Hamel" );
				break;
			case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Hamel_01" );
				break;
			case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Hamel_02" );
				break;
			case SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Rest.TGA", L"FieldMap_Navigation_Hamel_R" );
				break;
			case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_03:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Hamel_03" );
				break;
			case SEnum::VMI_BATTLE_FIELD_HAMEL_FIELD_04:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Hamel_04" );
				break;
//#pragma endregion �׺���̼�

//#pragma region ����
			case SEnum::VMI_SANDER:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Village.TGA", L"TownMap_Navigation_Sander" );
				break;
			case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_01:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Sander_01" );
				break;
			case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_02:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_02.TGA", L"FieldMap_Navigation_Sander_02" );
				break;
			case SEnum::VMI_BATTLE_FIELD_SANDER_REST_00:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Rest.TGA", L"FieldMap_Navigation_Sander_R" );
				break;
			case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_03:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_03.TGA", L"FieldMap_Navigation_Sander_03" );
				break;
			case SEnum::VMI_BATTLE_FIELD_SANDER_FIELD_04:
				pNavi_->SetBGTex( L"DLG_UI_Common_Texture_Field_03.TGA", L"FieldMap_Navigation_Sander_04" );
				break;
//#pragma endregion �׺���̼�

			default:
				break;
			}

			CKTDGUIControl::UIPointData* pNewPoint_Navi = new CKTDGUIControl::UIPointData();
			pNewPoint_Navi->bUseTextureSize		= false;
			pNewPoint_Navi->fChangeTime			= 0.0f;
			if ( vPos_.x > 0.24 )
			{
				pNewPoint_Navi->leftTopPoint		= D3DXVECTOR2( 763 - 750, 606 );
				pNewPoint_Navi->leftBottomPoint		= D3DXVECTOR2( 763 - 750, 732 );
				pNewPoint_Navi->rightTopPoint		= D3DXVECTOR2( 1019 - 750, 606 );
				pNewPoint_Navi->rightBottomPoint	= D3DXVECTOR2( 1019 - 750, 732 );
			}
			else
			{
				pNewPoint_Navi->leftTopPoint		= D3DXVECTOR2( 763, 606 );
				pNewPoint_Navi->leftBottomPoint		= D3DXVECTOR2( 763, 732 );
				pNewPoint_Navi->rightTopPoint		= D3DXVECTOR2( 1019, 606 );
				pNewPoint_Navi->rightBottomPoint	= D3DXVECTOR2( 1019, 732 );
			}
			pNavi_->SetBGPoint( pNewPoint_Navi );

			CKTDGUIControl::UIPointData* pNewPoint_NaviWindow = new CKTDGUIControl::UIPointData();
			pNewPoint_NaviWindow->bUseTextureSize	= false;
			pNewPoint_NaviWindow->fChangeTime		= 0.0f;
			pNewPoint_NaviWindow->leftTopPoint		= D3DXVECTOR2( 763, 606 );
			pNewPoint_NaviWindow->leftBottomPoint	= D3DXVECTOR2( 763, 732 );
			pNewPoint_NaviWindow->rightTopPoint		= D3DXVECTOR2( 909, 606 );
			pNewPoint_NaviWindow->rightBottomPoint	= D3DXVECTOR2( 909, 732 );
			pNavi_->SetWindowPoint( pNewPoint_NaviWindow );
		}
	}
	else
	{
		if ( NULL != pStaticWorld )
			pStaticWorld->SetShow( true );

		if ( NULL != pStaticField )
			pStaticField->SetShow( false );

		vPos_ = m_pDLGWorldMap->GetPos();
		vPos_.x /= -m_pDLGWorldMap->GetSize().x;
		vPos_.y /= -m_pDLGWorldMap->GetSize().y;

		pNavi_->SetBGTex( L"DLG_UI_Common_Texture70_NEW.TGA", L"WorldMap_Navigation" );

		CKTDGUIControl::UIPointData* pNewPoint_Navi = new CKTDGUIControl::UIPointData();
		pNewPoint_Navi->bUseTextureSize		= false;
		pNewPoint_Navi->fChangeTime			= 0.0f;
		pNewPoint_Navi->leftTopPoint		= D3DXVECTOR2( 753, 524 );
		pNewPoint_Navi->leftBottomPoint		= D3DXVECTOR2( 753, 732 );
		pNewPoint_Navi->rightTopPoint		= D3DXVECTOR2( 1019, 524 );
		pNewPoint_Navi->rightBottomPoint	= D3DXVECTOR2( 1019, 732 );
		pNavi_->SetBGPoint( pNewPoint_Navi );

		CKTDGUIControl::UIPointData* pNewPoint_NaviWindow = new CKTDGUIControl::UIPointData();
		pNewPoint_NaviWindow->bUseTextureSize	= false;
		pNewPoint_NaviWindow->fChangeTime		= 0.0f;
		pNewPoint_NaviWindow->leftTopPoint		= D3DXVECTOR2( 758, 529 );
		pNewPoint_NaviWindow->leftBottomPoint	= D3DXVECTOR2( 758, 644 );
		pNewPoint_NaviWindow->rightTopPoint		= D3DXVECTOR2( 911, 529 );
		pNewPoint_NaviWindow->rightBottomPoint	= D3DXVECTOR2( 911, 644 );
		pNavi_->SetWindowPoint( pNewPoint_NaviWindow );
	}
}

bool CX2WorldMapUI::GetDragNaviMap()
{
	if ( NULL != m_pDLGWorldMapFront )
	{
		CKTDGUINavigation* pNavi = (CKTDGUINavigation*) m_pDLGWorldMapFront->GetControl( L"WorldMap_Navi" );

		if( NULL != pNavi )
		{
			return pNavi->GetDraggingWindow();
		}
	}

	return false;
}
