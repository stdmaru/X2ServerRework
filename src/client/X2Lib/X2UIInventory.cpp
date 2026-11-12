#include "stdafx.h"


#include "./X2UIInventory.h"

CX2UIInventory::CX2UIInventory( CKTDXStage* pNowStage, const WCHAR* /*pFileName*/ )
: CX2UIDragable( pNowStage, NULL ),
//: CX2ItemSlotManager( pNowStage, pFileName )
m_NowInventorySortType(CX2Inventory::ST_EQUIP),
m_MovedPosition(0,0),				// D3DXVECTOR2
m_pDLGUIInventory(NULL),
m_pDLGResolveItem(NULL),
m_pDLGResolveItemResult(NULL),
m_pDLGSocketItem(NULL),
m_pDLGSocketItemPushOk(NULL),
m_pDLGSocketItemCheat(NULL),
m_pDLGCubeCheck(NULL),
m_pDLGElChanger(NULL),
m_pDLGElChangerResult(NULL),
m_pDLGElBreak( NULL ),
m_pDLGCheckPShop( NULL ),
m_pUISocketItem(NULL),
m_pDLGItemEnroll( NULL ),
m_pDLGDeleteItemConfirm( NULL ),
m_iItemEnrollNum( 1 ),					// D3DXVECTOR2
m_EnrollItemUid( 0 ),						// D3DXVECTOR2
m_pDLGSealItemConfirm( NULL ),
m_pSlotItemToBeSealed( NULL ),
m_itemUIDForSeal( 0 ),
m_itemUIDToBeUnSealed( 0 ),
//m_mapResolveResultInsertedItem,
m_pDLGRandomItem(NULL),
m_eRandomItemEventType(RIOET_NONE),
m_iRandomItemUID(0),
m_RandomItemID(0)
,
m_bShow(false),
m_DLGPosition(0,0),
m_DLGSize(0,0),
m_ElChangerItemUID(0)
,
m_ElChangerItemCount(0)
//{{100416 ������ Ʈ���� �ڽ� �߰�
//}}100416 ������ Ʈ���� �ڽ� �߰�
,m_DecompositionItemUID(0),
m_DecompositionWindowNowPageNum(0),
m_SocketItemUID(0)
,
//m_bOpenedAttractionItem(false),
m_SocketSlotIndex(-1),
m_bSocketRareMagicStone(false),
m_bSocketRemove(false),
m_fPlayTime(0),
m_bPlayAttractionItem(false),
m_pDLGOpenAttraction(NULL),
m_pDLGAttractionResult(NULL),
m_pDLGGetAttractionItemBG(NULL),
m_hMeshInstMeltMachine(INVALID_MESH_INSTANCE_HANDLE),
m_hMeshInstMeltMachineStart(INVALID_MESH_INSTANCE_HANDLE),
m_hSeqSpreadLight(INVALID_PARTICLE_SEQUENCE_HANDLE),
m_hSeqEndLight(INVALID_PARTICLE_SEQUENCE_HANDLE),
m_hSeqEndLight2(INVALID_PARTICLE_SEQUENCE_HANDLE)
,
m_hSeqEndLight3(INVALID_PARTICLE_SEQUENCE_HANDLE),
m_CouponUid(0),
m_CouponTargetUid(0),
m_pDLGTreeDay1(NULL),
m_pDLGTreeDay2(NULL)
//m_MapAutoRandomItemGet.clear();
, m_bPlayingTreedayEvent(false)
, m_bAprilSoundPlayed(true)
, m_RecoveryItemUid(0)

//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
,
m_bUsingRandomItemAutoOpen(false),
m_OpenedRandomItemCount(0),
m_pDLGAutoOpenMsgBox(NULL),
m_pDLGCubeOpenEffect(NULL)
//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������

//{{ kimhc // 2009-08-19 // ������ ���� ����� �� Ȯ�� â
, m_fCubeOpenTime(0)
, m_bCubeJustOpen(false)
, m_WarpItem(0)
, m_bIsNewItem(false)
//}} kimhc // 2009-08-19 // ������ ���� ����� �� Ȯ�� â
, m_UseItemTempUID( 0 )
, m_UsedItemUID(0)
, m_TitleItemUID(0)
, m_pDLGWarpDestination( NULL )
, m_iSelectedWarpDest(0)
// oasis907 : ����� [2010.4.19] //
, m_SumDelta(0)
, m_iSelectedWarpDestIndex(-1)
, m_iCurrentVillageWarpIndex(-1)
, m_bIsEdConsumptionWarp(false) // oasis907 : ����� [2010.9.2] // �� �κ��丮
, m_pDLGUSCJob( NULL )
, m_iSelectedUSCJob(0)
,m_UseTransformdItemUID( 0 )
, m_UseTransformItemPetUID( 0 )
, m_pDLGIdentifyStone(NULL)
,m_pUIPetInventory(NULL)
,m_bAutoFeed(false)
,m_pPetFoodSlot( NULL )
, m_eItemObtainResultType ( IORT_NONE )
,m_pDLGItemHighLight(NULL)
,m_bProcessCubeOpen(false)
, m_bHideCubeOpneBar( false )
, m_bIsSptNoneItem(false)
, m_pSptNoneItemDelete(NULL)
, m_pNowIceHeaterEventData( NULL )
, m_hUIEffectSetWait ( INVALID_EFFECTSET_HANDLE )
, m_hUIEffectSetPlay ( INVALID_EFFECTSET_HANDLE )
, m_iConsumeItemUID ( 0 )
, m_iTargetItemUID ( 0 )
, m_iConsumeItemQuantity ( -1 )
, m_iRidingItemUid( 0 )
, m_ElBreakItemCount( 0 )
, m_iConfirmED( 0 )
,m_pDLGBuyConfirm( NULL )
,m_fLeftMouseWhellCoolTime (-1.f)
{
	// �ʱ�ȭ ////////////////////////////////////////////////////////////////////////

	m_mapResolveResultInsertedItem.clear();

	//////////////////////////////////////////////////////////////////////////
	RegisterLuaBind();


		m_pDLGUIInventory = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Inventory_reform_Ver2.lua" );

	m_pDLGUIInventory->SetShow(false);
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUIInventory );
	m_pDLGUIInventory->SetDisableUnderWindow(true);

	CKTDGUIRadioButton* pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Equip" );
	pEquipButton->SetChecked(true);

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		pItemSlot->LostItemUI();
		pItemSlot->SetShow( false );
		pItemSlot->SetEnable( false );
	}

	SetShow(false);

	m_pDLGUIInventory->SetShowEnable(false, false);


	m_pDLGItemHighLight = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Inventory_HighLight_reform.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGItemHighLight );
	m_pDLGItemHighLight->SetShow(false);

	for( int i = 0; i < MY_INVENTORY_MAX_SLOT_HEIGHT; ++i )
	{
		m_NowInventoryPageNum[i] = 1;
	}

	m_mapIceHeaterEventData.clear();

	/// ������  ��� ���� ��ũ��Ʈ �Ľ�
	OpenScriptFile( L"IceHeaterEvent.lua" );
}


CX2UIInventory::~CX2UIInventory(void)
{

	SAFE_DELETE_DIALOG( m_pDLGUIInventory );

	SAFE_DELETE_DIALOG(m_pDLGRandomItem);
	SAFE_DELETE_DIALOG(m_pDLGCubeCheck);
	SAFE_DELETE_DIALOG(m_pDLGElChanger);

	SAFE_DELETE_DIALOG( m_pDLGElBreak );
	SAFE_DELETE_DIALOG( m_pDLGBuyConfirm );

	SAFE_DELETE_DIALOG(m_pDLGResolveItem);
	SAFE_DELETE_DIALOG(m_pDLGResolveItemResult);

	SAFE_DELETE_DIALOG(m_pDLGSocketItem);
	SAFE_DELETE_DIALOG(m_pDLGSocketItemPushOk);
	SAFE_DELETE_DIALOG(m_pDLGSocketItemCheat);

	//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
	SAFE_DELETE_DIALOG(m_pDLGItemEnroll);
	//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������

	SAFE_DELETE_DIALOG( m_pDLGWarpDestination );
	SAFE_DELETE_DIALOG( m_pDLGUSCJob );
	SAFE_DELETE_DIALOG(m_pDLGOpenAttraction);
	SAFE_DELETE_DIALOG(m_pDLGGetAttractionItemBG);
	SAFE_DELETE_DIALOG(m_pDLGAttractionResult);

	if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshInstMeltMachineStart )
	{
		g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachineStart );
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshInstMeltMachine )
	{
		g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachine );
	}

	if ( m_hSeqSpreadLight != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqSpreadLight );
	}

	if ( m_hSeqEndLight != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqEndLight );
	}

	SAFE_DELETE_DIALOG(m_pDLGIdentifyStone);

	SAFE_DELETE_DIALOG(m_pDLGCubeOpenEffect);
	SAFE_DELETE_DIALOG(m_pDLGCheckPShop);
	SAFE_DELETE_DIALOG(m_pDLGItemHighLight);

	SAFE_DELETE_DIALOG( m_pDLGEventCubeResultImage );

	SAFE_DELETE_DIALOG( m_pSptNoneItemDelete );


	/// ������ ��� ������ ��� ����
	for ( map<RANDOM_ITEM_OPEN_EVENT_TYPE, IceHeaterEventData*>::iterator mIt = m_mapIceHeaterEventData.begin();
		  mIt != m_mapIceHeaterEventData.end(); )
	{
		IceHeaterEventData* pIceHeaterEventData = mIt->second;
		SAFE_DELETE( pIceHeaterEventData );
		mIt = m_mapIceHeaterEventData.erase( mIt );
	}

	if ( INVALID_EFFECTSET_HANDLE != m_hUIEffectSetWait )
		g_pData->GetUIEffectSet()->StopEffectSet( m_hUIEffectSetWait );

	if ( INVALID_EFFECTSET_HANDLE != m_hUIEffectSetPlay )
		g_pData->GetUIEffectSet()->StopEffectSet( m_hUIEffectSetPlay );

}



HRESULT CX2UIInventory::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	POINT point;
	RECT rt;

	GetCursorPos( &point );
	if( *m_pSlotBeforeDragging != NULL )
	{
 		if( g_pKTDXApp->GetHWND() != WindowFromPoint( point ) )
 		{
 			SetNowDragItemReturn();
 		}
 		else
		{
			ScreenToClient( g_pKTDXApp->GetHWND(), &point );
			GetClientRect( g_pKTDXApp->GetHWND(), &rt );
			if(	point.x <= rt.left || point.x >= rt.right || point.y <= rt.top || point.y >= rt.bottom )
			{
				SetNowDragItemReturn();
			}
		}
	}

	// oasis907 : ����� [2010.4.5] //
	if ( m_pUISocketItem != NULL && m_pUISocketItem->GetShow() )
		m_pUISocketItem->OnFrameMove( fTime, fElapsedTime );

	// oasis907 : ����� [2010.9.7] //
	if ( m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() )
		m_pUIPetInventory->OnFrameMove( fTime, fElapsedTime );



	if(m_fCubeOpenTime > 0.f)
	{
		ASSERT(g_pData->GetCubeOpenImageManager());
		if( g_pData->GetCubeOpenImageManager()->IsStart() == true )
			m_fCubeOpenTime -= (fElapsedTime);
		else
		m_fCubeOpenTime -= fElapsedTime;

		float fProgressOpen = 1.f;
		fProgressOpen -= (m_fCubeOpenTime / 3.f);

		if( m_pDLGCubeOpenEffect != NULL )
		{
			CKTDGUIStatic *pStatic = (CKTDGUIStatic*)m_pDLGCubeOpenEffect->GetControl(L"g_pStaticNote_Loading");
			CKTDGUIControl::CPictureData *pPicture = pStatic->GetPictureIndex(1);
			pPicture->SetShow(!m_bHideCubeOpneBar);
			float fProgressRate = pPicture->GetOriginalSize().x * fProgressOpen;
			pPicture->SetSizeX( fProgressRate );

			//{{ oasis907 : ����� [2011.3.29] �ĸ��� ť�� ������
			if(m_pDLGCubeOpenEffect->GetDummyInt(0) == 1 && fProgressOpen > 0.5f)
			{
				pStatic->GetPicture(0)->SetTex(L"DLG_UI_Common_Texture51.tga", L"Note");
				m_pDLGCubeOpenEffect->ClearDummyInt();
			}
			//}}
			ASSERT(g_pData->GetCubeOpenImageManager());
			if( g_pData->GetCubeOpenImageManager()->IsStart() == true)
			{
				if( g_pData->GetCubeOpenImageManager()->IsSoundPlaying() == true)
				{
					std::wstring wstrSoundName;
					if(g_pData->GetCubeOpenImageManager()->GetSoundName(m_RandomItemID, wstrSoundName) == true)
					{
						g_pKTDXApp->GetDeviceManager()->PlaySound( wstrSoundName.c_str(), false, false );
					}
					g_pData->GetCubeOpenImageManager()->SetSoundPlaying( false);
				}

				std::wstring wstrName;
				if(g_pData->GetCubeOpenImageManager()->GetCubeImageName(fProgressOpen, m_RandomItemID, wstrName) == true)
				{
					//sin �׷��� �̿�
					if( g_pData->GetCubeOpenImageManager()->IsFadeInOutImage( m_RandomItemID ) == true )
					{
					double fAptime = 0.0;

					fAptime = sin(180 * 3.14 * (m_fCubeOpenTime*2/3 - 1 )  / 180 );
					if( m_fCubeOpenTime > 1.5 )
					{
					}
					else
					{
						fAptime *= -1 ;
					}
					fAptime = min( fAptime * 2 , 1.0 );

					pStatic->GetPicture(0)->pPoint->color.a = (float)fAptime;
					}
					pStatic->GetPicture(0)->SetTex( wstrName.c_str(), L"Note");
				}
			}
		}
	}
	else if(m_bCubeJustOpen)
	{
		SAFE_DELETE_DIALOG( m_pDLGCubeOpenEffect );
		m_pDLGCubeOpenEffect = NULL;
		m_bCubeJustOpen = false;
		if( m_eItemObtainResultType == IORT_RANDOM_EXCHANGE &&
			g_pData->GetUIManager()->GetUIItemExchangeShop() != NULL )
		{
			g_pData->GetUIManager()->GetUIItemExchangeShop()->Handler_EGS_ITEM_EXCHANGE_REQ();
		}
		else if( m_eItemObtainResultType == IORT_CUBE_OPEN )
		Handler_EGS_OPEN_RANDOM_ITEM_REQ();
	}



	if( m_bPlayingTreedayEvent )
	{
		m_fPlayTime += fElapsedTime;

		if(m_bAprilSoundPlayed == false && m_fPlayTime > 0.1f )
		{
			m_bAprilSoundPlayed = true;
			g_pKTDXApp->GetDeviceManager()->PlaySound( L"TreeDay_Event.ogg", false, false );
		}

		if ( (m_pDLGTreeDay1 != NULL && m_pDLGTreeDay1->GetIsMoving() == false)
			&&	( m_pDLGTreeDay2 != NULL && m_pDLGTreeDay2->GetIsMoving() == false) )
		{
			SAFE_DELETE_DIALOG(m_pDLGTreeDay1);
			SAFE_DELETE_DIALOG(m_pDLGTreeDay2);
			m_bPlayingTreedayEvent = false;

			m_fPlayTime = 0.0f;
			UpdateOpenTreeDayItemAck();
		}
	}

	if( g_pData->GetCubeOpenImageManager()->IsPlaying() == true)
	{
		if( g_pData->GetCubeOpenImageManager()->IsNowResultEventCubePlaying() == true)
		{
			if(m_pDLGEventCubeResultImage != NULL && m_pDLGEventCubeResultImage->GetIsMoving() == false)
			{
				SAFE_DELETE_DIALOG(m_pDLGEventCubeResultImage);
				g_pData->GetCubeOpenImageManager()->SetPlaying( false);
				g_pData->GetCubeOpenImageManager()->SetStart( false);
				g_pData->GetCubeOpenImageManager()->SetNowResultEventCubePlaying(false);
				m_fPlayTime = 0.0f;
				UpdateOpenRandomItemAckNew();
			}
		}
		else
		{
			g_pData->GetCubeOpenImageManager()->SetPlaying( false);
			g_pData->GetCubeOpenImageManager()->SetStart( false);
			m_fPlayTime = 0.0f;
			UpdateOpenRandomItemAckNew();
		}
	}


	if ( NULL == g_pData ||
		 NULL == g_pData->GetUIEffectSet() ||
		 NULL == g_pData->GetUIMajorXMeshPlayer() )	/// �⺻ ��ó��
		 return false;

	 /// ������ ���� �� ���� ó��
	if( INVALID_MESH_INSTANCE_HANDLE != m_hMeshInstMeltMachine &&
		 true == g_pData->GetUIMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstMeltMachine ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstMeltMachine );
		if(NULL != pMeshInst)
		{
			if( pMeshInst->GetXSkinAnim() != NULL )
				pMeshInst->GetXSkinAnim()->GetRenderParam()->fOutLineWide = 1.2f;

			/// �ش� �������� ���� �� ����Ʈ ����
			if ( NULL == g_pData->GetUIEffectSet()->GetEffectSetInstance( m_hUIEffectSetWait ) )
			{
				const wstring wstrWaitUIEffectSetName =
					NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrWaitUIEffectSetName : L"EffectSet_Ice_Heater_Wait";

				m_hUIEffectSetWait	= g_pData->GetUIEffectSet()->PlayEffectSetByMeshPlayer( wstrWaitUIEffectSetName.c_str(), pMeshInst->GetHandle(), CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR );
			}
		}
	}
	else
	{
		/// �ش� �������� ���� �� ����Ʈ ����
		if ( INVALID_EFFECTSET_HANDLE != m_hUIEffectSetWait )
			g_pData->GetUIEffectSet()->StopEffectSet( m_hUIEffectSetWait );
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_Start = g_pData->GetUIMajorXMeshPlayer()->GetMeshInstance( m_hMeshInstMeltMachineStart );
	if( NULL != pMeshInst_Start && pMeshInst_Start->GetXSkinAnim() != NULL )
	{
		pMeshInst_Start->GetXSkinAnim()->GetRenderParam()->fOutLineWide = 1.2f;
	}

	//////////////////////////////////////////////////////////////////////////

	/// ������ ���� ó��
	if ( m_bPlayAttractionItem == true )
	{
		m_fPlayTime += fElapsedTime;

		/// ������ ���� �� �Ž�, ����Ʈ�� �����ִٸ�, ����
		if( g_pData->GetUIMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstMeltMachineStart ) == false )
		{
			m_hMeshInstMeltMachineStart = INVALID_MESH_INSTANCE_HANDLE;
			g_pData->GetUIEffectSet()->StopEffectSet( m_hUIEffectSetWait );
		}
		else if( NULL == g_pData->GetUIEffectSet()->GetEffectSetInstance( m_hUIEffectSetWait ) )
		{
            m_hUIEffectSetWait = INVALID_EFFECTSET_HANDLE;
		}

		/// ������ ��� ����Ʈ ����
		if( NULL != pMeshInst_Start &&
			NULL != pMeshInst_Start->GetXSkinAnim() &&
			L"Start" == pMeshInst_Start->GetXSkinAnim()->GetNowAnimName() &&
			true == pMeshInst_Start->EventTimerGlobal( 0.1f ) )
		{
			const wstring wstrPalyEffectSet = NULL !=
				m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrPlayUIEffectSetName : L"FireMachineStart";

			m_hUIEffectSetPlay = g_pData->GetUIEffectSet()->PlayEffectSetByMeshPlayer(  wstrPalyEffectSet.c_str(), pMeshInst_Start->GetHandle(), CX2EffectSet::ET_MESH_PLAYER_UI_MAJOR );
		}

		/// ���� �ð� ����� ���� ��ü �ʱ�ȭ �� ����
		const float fEndPlayTime = NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_fPlayTime : 10.f;

		if( m_fPlayTime >= fEndPlayTime )
		{
			m_bPlayAttractionItem = false;
			m_fPlayTime = 0.0f;

			if ( m_pDLGGetAttractionItemBG != NULL )
			{
				m_pDLGGetAttractionItemBG->Move( D3DXVECTOR2( 0,0 ), D3DXCOLOR( 0,0,0,0 ), 0.5f, true, true );
				m_pDLGGetAttractionItemBG = NULL;
			}

			g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachineStart );
			if( NULL != m_pDLGOpenAttraction )
			{
				m_pDLGOpenAttraction->SetHasUnit(NULL);
			}

			g_pData->GetUIEffectSet()->StopEffectSet( m_hUIEffectSetPlay );

			//{{ 2012. 10. 08  �ҷ��� ������
			if( m_eRandomItemEventType == RIOET_HALLOWEEN_ICE_HEATER )
			{
				UpdateOpenHalloWeenItemAck();
			}
			else
			{
				UpdateOpenAttractionItemAck();
			}
		}

	}



	if( m_fLeftMouseWhellCoolTime > 0.f )
	{
		m_fLeftMouseWhellCoolTime -= fElapsedTime;
	}

	// ���� ���콺�� ��ġ�� �ڽ��� UI�� ������쿡�� ó���Ѵ�.
	// ������� ������ ������ ������ �ڵ� �Ʒ���
	if(m_pDLGUIInventory != NULL && m_pDLGUIInventory->GetIsMouseOver() == true)
	{
		return CX2ItemSlotManager::OnFrameMove( fTime, fElapsedTime );
	}
	else
	{
		CX2ItemSlotManager::InvalidSelectedItem();
		CX2ItemSlotManager::InvalidSlotDesc();
	}
	return S_OK;
}

bool CX2UIInventory::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// �տ� ��� ���̾�αװ� ������ �޽����� ó������ �ʴ´�
	if( true == g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckFrontModalDlg( m_pDLGUIInventory ) )
	{
		switch( uMsg )
		{
		case WM_MOUSEWHEEL:
			{
				if( m_pDLGWarpDestination != NULL && m_pDLGWarpDestination->GetIsMouseOver() )
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
		return false;
	}

	/// �κ��丮 Ȯ�� ��ġ ���� ���� ���� ��, ��� �ȳ�â�� �κ��丮�� �����ų�, �κ��丮�� ���� �������� ������ �־
	/// ��� �ȳ�â�� �������� ��, �κ��丮 ���̾�α׸� Ŭ���ϸ� ��� �ȳ�â ����� �� �ֵ��� �߰�
	switch( uMsg )
	{
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			if( NULL != g_pData &&
				NULL != g_pData->GetUIManager() &&
				NULL != g_pData->GetUIManager()->GetUIPostBoxWarning() &&
				true == g_pData->GetUIManager()->GetUIPostBoxWarning()->GetShow() )
			{
				g_pData->GetUIManager()->GetUIPostBoxWarning()->SetShow(false);
			}
		}
		break;
	}

	// oasis907 : ����� [2010.9.7] //
	if(m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() == true)
	{
		if ( m_pUIPetInventory->MsgProc(hWnd, uMsg, wParam, lParam) == true)
		{
			return true;
		}
	}

	// oasis907 : ����� [2010.4.5] //
	if(m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true)
	{
		if ( m_pUISocketItem->MsgProc(hWnd, uMsg, wParam, lParam) == true)
		{
			return true;
		}
	}


	bool bFlag = false;
	switch( uMsg )
	{
		// ���콺 �޽����鿡 ����
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		//case WM_MOUSEWHEEL:
		{
			if ( false == m_pDLGUIInventory->GetIsMouseOver())
			{
				return false;
			}
			bFlag = true;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			if ( false == m_pDLGUIInventory->GetIsMouseOver())
			{
				return false;
			}
			g_pData->GetUIManager()->SetUILayerIndex(CX2UIManager::UI_MENU_INVEN, true);
			bFlag = true;
		}
		break;
	default:
		break;
	}

	switch( uMsg )
	{
	case WM_LBUTTONUP:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			MouseUp( mousePos );

		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			if ( MouseDown( mousePos) == true )
				return true;

		}
		break;

	case WM_MOUSEMOVE:
		{

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };

			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_NowMousePos = mousePos;

			MouseMove( mousePos );
		}
		break;

	case WM_RBUTTONDOWN:
		{
			SetNowDragItemReturn();
		}
		break;

	case WM_RBUTTONUP:
		{
			SetNowDragItemReturn();

			POINT mousePoint = { short(LOWORD(lParam)), short(HIWORD(lParam)) };
			D3DXVECTOR2 mousePos = D3DXVECTOR2( (float)mousePoint.x, (float)mousePoint.y );
			m_CouponUid = 0;
			m_CouponTargetUid = 0;
			MouseRButtonUp( mousePos );

		}
		break;
	case WM_MOUSEWHEEL:
		{
			return OnMouseWheel(hWnd, uMsg, wParam,lParam);
		} break;

	}

	return bFlag;
}


bool CX2UIInventory::UICustomEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// oasis907 : ����� [2010.4.5] //
	if(m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true)
	{
		if ( m_pUISocketItem->UICustomEventProc(hWnd, uMsg, wParam, lParam) == true)
		{
			return true;
		}
	}

	if(m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() == true)
	{
		if ( m_pUIPetInventory->UICustomEventProc(hWnd, uMsg, wParam, lParam) == true)
		{
			return true;
		}
	}

	switch(wParam)
	{
	case UIM_SKILL_INIT_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			if( g_pData->GetSkillTree() != NULL )
			{
				g_pData->GetSkillTree()->Handler_EGS_INIT_SKILL_TREE_REQ(m_UsedItemUID);
			}
			return true;
		} break;
	case UIM_CHANGE_NICK_NAME_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_UseItemTempUID );
			m_UseItemTempUID = 0;

			// �г��� ���濡 �����ϸ� EGS_DELETE_NICK_NAME_SUCCESS_NOT�� �ް� ĳ���� ����â���� �̵��Ѵ�
			return true;
		} break;
	case UIM_ITEM_USE_TITLE:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_TitleItemUID );
			m_TitleItemUID = 0;
		}
		break;

	case UIM_ITEM_USE_SKILL_UNSEAL:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL && NULL != pControl->GetDialog() )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_UsedItemUID );
			m_UsedItemUID = 0;
		} break;


	case UIM_DELETE_ITEM:
		{
			//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ������
			CX2State*	pState	= static_cast< CX2State* >( g_pMain->GetNowState() );
			CX2Cursor*	pCursor = pState->GetCursor();
			pCursor->ChangeCursorState( CX2Cursor::XCS_DELETE_ITEM );

			return true;
			//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ������
		} break;
#if X2_DEV_TOOLS
	case UIM_SELL_CATEGORY_ALL:
		{
			if( CX2User::XUAL_DEV == g_pData->GetMyUser()->GetAuthLevel()
				|| CX2User::XUAL_OPERATOR == g_pData->GetMyUser()->GetAuthLevel()
				)
			{
				if(g_pData->GetUIManager()->GetUIShop() == NULL)
				{
					g_pData->GetUIManager()->CreateUIShop();
				}
				g_pData->GetUIManager()->GetUIShop()->AuthAutoSellItem( m_NowInventorySortType );
			}
		} break;
#endif
	case UIM_EXIT_RANDOM_ITEM_NEW:
		{
			SAFE_DELETE_DIALOG(m_pDLGCubeOpenEffect);
			m_fCubeOpenTime = 0.f;
			m_bCubeJustOpen = false;
			m_eItemObtainResultType = IORT_NONE;
			return true;
		}
	case UIM_EXIT:
		{
			if( g_pData->GetCubeOpenImageManager() != NULL && g_pData->GetCubeOpenImageManager()->IsNowResultEventCubePlaying() == true)
				return true;

			SetShow(false);
			return true;
		}
	case UIM_EQUIP:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_EQUIP ) );
			return SetInventorySort(CX2Inventory::ST_EQUIP, GetNowInventoryPageNum( CX2Inventory::ST_EQUIP ) );
		}
	case UIM_ACCESSORY:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_ACCESSORY ) );
			return SetInventorySort(CX2Inventory::ST_ACCESSORY, GetNowInventoryPageNum( CX2Inventory::ST_ACCESSORY ) );
		}
	case UIM_QUICK_SLOT:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_QUICK_SLOT ) );
			return SetInventorySort(CX2Inventory::ST_QUICK_SLOT, GetNowInventoryPageNum( CX2Inventory::ST_QUICK_SLOT ) );
		}
	case UIM_MATERIAL:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_MATERIAL ) );
			return SetInventorySort(CX2Inventory::ST_MATERIAL, GetNowInventoryPageNum( CX2Inventory::ST_MATERIAL ) );
		}
	case UIM_QUEST:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_QUEST ) );
			return SetInventorySort(CX2Inventory::ST_QUEST, GetNowInventoryPageNum( CX2Inventory::ST_QUEST ) );
		}
	case UIM_SPECIAL:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_SPECIAL ) );
			return SetInventorySort(CX2Inventory::ST_SPECIAL, GetNowInventoryPageNum( CX2Inventory::ST_SPECIAL ) );
		}
	case UIM_AVATAR:
		{
			UpdatePageUI( GetNowInventoryPageNum( CX2Inventory::ST_AVARTA ) );
			return SetInventorySort(CX2Inventory::ST_AVARTA, GetNowInventoryPageNum( CX2Inventory::ST_AVARTA ) );
		}

		//{{ kimhc // 2010-01-05 // PC�� �����̾� ����
	case UIM_PC_BANG:
		{
			if ( g_pData != NULL &&
				 g_pData->GetMyUser() != NULL &&
				 g_pData->GetMyUser()->GetIsPCRoom() == true
				 && g_pData->GetPremiumBuffManager() != NULL
				 && g_pData->GetPremiumBuffManager()->IsPcBangAvatar() == true
				 )
			{
				UpdatePageUI();
				return SetInventorySort(CX2Inventory::ST_PC_BANG );
			}
			else
			{
				CheckRadioButtonBySortType( m_NowInventorySortType );
				g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4838 ),g_pMain->GetNowState() );
				return true; // �޽��� �ڽ� ó��
			}
		}
		//}} kimhc // 2010-01-05 // PC�� �����̾� ����


	case UIM_PAGE_1:
	case UIM_PAGE_2:
	case UIM_PAGE_3:
	case UIM_PAGE_4:
		{
			/// �κ��丮 ī�װ��� ������ �ƴϸ� �н�
			if( CX2Inventory::ST_AVARTA < m_NowInventorySortType || CX2Inventory::ST_NONE >= m_NowInventorySortType )
				return false;

			/// ���� ������ �κ��丮 ������ �� ����
			const int iNowInvenPageNum = wParam - UIM_PAGE_1+1;
			SetNowInventoryPageNum( m_NowInventorySortType, iNowInvenPageNum );
			SetInventorySort( m_NowInventorySortType, iNowInvenPageNum );
			return true;
		}
	case UIM_OVER_PAGE_1:
	case UIM_OVER_PAGE_2:
	case UIM_OVER_PAGE_3:
	case UIM_OVER_PAGE_4:
		{
			/// �κ��丮 ī�װ��� ������ �ƴϸ� �н�
			if( CX2Inventory::ST_AVARTA < m_NowInventorySortType || CX2Inventory::ST_NONE >= m_NowInventorySortType )
				return false;

			/// �������� �巡�� ���� ���·� ������ ��ư�� ���� �ϸ� �κ��丮 ����
			int iTargetPage = wParam - UIM_OVER_PAGE_1+1;
			if( *m_pSlotBeforeDragging != NULL && GetNowInventoryPageNum( m_NowInventorySortType ) != iTargetPage )
			{
				SetNowInventoryPageNum( m_NowInventorySortType, iTargetPage );
				SetInventorySort( m_NowInventorySortType, iTargetPage );
				UpdatePageUI( iTargetPage );
			}
			return true;
		}

	case UIM_OPEN_RANDOM_ITEM:
		{
			//ť�� ���� �������̶�� ��Ŷ �ٽ� ������ �ʱ�
			if( true == m_bProcessCubeOpen )
				return true;
			m_eItemObtainResultType = IORT_NONE;
			return Handler_EGS_OPEN_RANDOM_ITEM_REQ();
		}
		break;
	case UIM_OPEN_RANDOM_ITEM_NEW:
		{
			OpenRandomItem( m_RandomItemID, IORT_CUBE_OPEN );
			return true;
		}
		break;
	case UIM_CLOSE_RANDOM_ITEM_RESULT:
		{
			if ( m_pDLGCubeCheck != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCubeCheck, NULL, false );

			m_pDLGCubeCheck = NULL;
			m_eRandomItemEventType = RIOET_NONE;
		}
		break;
	case UIM_CREATE_PERSONAL_SHOP:
		{
			if( g_pInstanceData != NULL && g_pInstanceData->IsActiveAgencyShop() == true )
			{
				if( m_pDLGCheckPShop == NULL )
				{
					m_pDLGCheckPShop = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999, -999), GET_STRING( STR_ID_12236 ), UIM_CHECK_PSHOP_OK, g_pMain->GetNowState() );
				}
			}
			else
			{
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PERSONAL_SHOP, true, CX2UIPersonalShop::OPENFORSELL);
			}
		}
		break;
	case UIM_RESOLVE_ITEM_OK:
		{
			if(g_pData->GetUIManager()->GetDLGFlag() == false)
			{
				g_pData->GetUIManager()->SetDLGFlag(true);
				CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
				pControl->RequestFocus();
				return true;
			}
			if ( m_pDLGResolveItem != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGResolveItem, NULL, false );
			}
			m_pDLGResolveItem = NULL;

			m_eItemObtainResultType = IORT_ITEM_RESOLVE;
			return Handler_EGS_RESOLVE_ITEM_REQ();
		}
		break;
	case UIM_RESOLVE_ITEM_CANCLE:
		{
			if ( m_pDLGResolveItem != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGResolveItem, NULL, false );
			}
			m_pDLGResolveItem = NULL;

			return true;
		}
		break;
	case UIM_RESOLVE_ITEM_RESULT_OK:
		{
			if ( m_pDLGResolveItemResult != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGResolveItemResult, NULL, false );
			}
			m_pDLGResolveItemResult = NULL;

//{{ ����� : 2010.11.16
			SAFE_DELETE_DIALOG(m_pDLGCubeOpenEffect);
			m_fCubeOpenTime = 0.f;
			m_bCubeJustOpen = false;
			m_eItemObtainResultType = IORT_NONE;
//}} ����� : 2010.11.16

			return true;
		}
		break;
	case UIM_RESOLVE_ITEM_RESULT_PREV_PAGE:
		{
			ChangeResolveResultPage( false );
			return true;
		}
		break;
	case UIM_RESOLVE_ITEM_RESULT_NEXT_PAGE:
		{
			ChangeResolveResultPage( true );
			return true;
		}
		break;

	case UIM_EL_CHANGER_EXIT:
		{
			if( m_pDLGElChanger != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGElChanger, NULL, false );
			}
			m_pDLGElChanger = NULL;
			return true;
		} break;
	case UIM_EL_CHANGER_NUM_PLUS:
		{
			m_ElChangerItemCount++;
			UpdateElChangerDLG(false);
			return true;
		} break;
	case UIM_EL_CHANGER_NUM_MINUS:
		{
			m_ElChangerItemCount--;
			UpdateElChangerDLG(false);
			return true;
		} break;
	case UIM_EL_CHANGER_NUM_UPDATE:
		{
			UpdateElChangerDLG(true);
			return true;
		} break;
	case UIM_EL_CHANGER_OK:
		{
			if( m_pDLGElChanger != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGElChanger, NULL, false );
			}
			m_pDLGElChanger = NULL;

			// �������� Ư���� �κ� ���� �˻�
			int iMaxSlot = 0;
			int iUnUseSlot = 0;

			if ( g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetSelectUnit() != NULL )
			{
				iMaxSlot = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum( CX2Inventory::ST_SPECIAL );
				iUnUseSlot = iMaxSlot - g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetUsedSlotNum(CX2Inventory::ST_SPECIAL );
			}

			if( m_ElChangerItemCount < 6 )
			{
				if( iUnUseSlot >= m_ElChangerItemCount )
				{
					return Handler_EGS_IDENTIFY_ITEM_REQ();
				}
				else
				{
					// �κ��丮 Ư�� ���� �� ������ X �� Ȯ�� �ؾ� �մϴ�.
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_5058, "i", m_ElChangerItemCount ) ), g_pMain->GetNowState() );
					return true;
				}
			}
			else
			{
				if( iUnUseSlot >= 6 )
				{
					return Handler_EGS_IDENTIFY_ITEM_REQ();
				}
				else
				{
					// �κ��丮 Ư�� ���� �� ������ X �� Ȯ�� �ؾ� �մϴ�.
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_5058, "i", 6 ) ), g_pMain->GetNowState() );
					return true;
				}
			}

			return true;
		} break;
	case UIM_EL_CHANGER_RESULT_OK:
		{
			if( m_pDLGElChangerResult != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGElChangerResult, NULL, false );
			}
			m_pDLGElChangerResult = NULL;
			return true;

		}
		break;
	case UIM_ITEM_RECOVERY_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			Handler_EGS_RESTORE_ITEM_REQ( m_RecoveryItemUid );
			m_RecoveryItemUid = 0;

			return true;
		} break;
	case UIM_ITEM_USE_WARP:
		{
			// ������ ��ȯ��
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )

				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_WarpItem );
			m_WarpItem = 0;
		}
		break;

	case UIM_SKILL_PLUS_ITEM_USE:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )

				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_UsedItemUID );
			m_UsedItemUID = 0;
		}
		break;


	case UIM_ATTRACTION_ITEM_DUMMY:
		{
			// ���� : ESC�� �����ϱ� ����. �ƹ��͵� ���� �ʴ´�.
			return true;
		} break;
	case UIM_ATTRACTION_ITEM_OK:
		{
			//ť�� ���� �������̶�� ��Ŷ �ٽ� ������ �ʱ�
			if( true == m_bProcessCubeOpen )
				return true;

			// ���ư��� �߿��� �޽����� ���� ����~
			if( m_hMeshInstMeltMachineStart != INVALID_MESH_INSTANCE_HANDLE &&
				g_pData->GetUIMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstMeltMachineStart ) == true )
				return true;
			//EGS_OPEN_RANDOM_ITEM_ACK�� �ޱ� ���� m_eRandomItemEventType ������ RIOET_NONE �Ǵ� ���, �������� ���¾��� ������ϴ�.
			//OK�� CANCLE �Ѵ� Ÿ�� �Ǹ� m_eRandomItemEventType ������ RIOET_NONE ���� ������ �Ǿ� �Ѵ� Ż �� ������ �����߽��ϴ�.

			//CANCLE���� Ż ��쿡 ���� ����ó��
			if( RIOET_NONE == m_eRandomItemEventType )
				return true;

			//OK ���� Ż ��쿡 ���� ����ó��
			if( NULL != m_pDLGOpenAttraction )
				m_pDLGOpenAttraction->SetCloseCustomUIEventID(-1);

			return Handler_EGS_OPEN_RANDOM_ITEM_REQ();
		}
		break;
	case UIM_ATTRACTION_ITEM_CANCLE:
		{
			// ���ư��� �߿��� �޽����� ���� ����~
			if( m_hMeshInstMeltMachineStart != INVALID_MESH_INSTANCE_HANDLE &&
				g_pData->GetUIMajorXMeshPlayer()->IsLiveInstanceHandle( m_hMeshInstMeltMachineStart ) == true )
				return true;

			if( m_hMeshInstMeltMachine != INVALID_MESH_INSTANCE_HANDLE )
			{
				g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachine );
				if( NULL != m_pDLGOpenAttraction )
				{
					m_pDLGOpenAttraction->SetHasUnit( NULL );
				}
			}

			if ( m_pDLGOpenAttraction != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOpenAttraction, NULL, false );
				m_pDLGOpenAttraction = NULL;
			}

			m_eRandomItemEventType = RIOET_NONE;


			return true;
		}
		break;
	case UIM_ATTRACTION_ITEM_RESULT_OK:
		{
			if ( m_pDLGAttractionResult != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGAttractionResult, NULL, false );
			m_pDLGAttractionResult = NULL;

			m_eRandomItemEventType = RIOET_NONE;


			return true;
		}
		break;
	case UIM_USE_COUPON_ITEM_OK:
		{
			Handler_EGS_ENCHANT_ATTACH_ITEM_REQ( m_CouponUid, m_CouponTargetUid );

			m_CouponUid = 0;
			m_CouponTargetUid = 0;
			CX2State* pState = (CX2State*)g_pMain->GetNowState();
			if(pState->GetCursor() != NULL)
				pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_NORMAL );

			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		}
		break;
	case UIM_USE_ATTRIBUTE_CHARM_COUPON_ITEM_OK:
		{
			Handler_EGS_ATTRIB_ATTACH_ITEM_REQ( m_CouponUid, m_CouponTargetUid );

			m_CouponUid = 0;
			m_CouponTargetUid = 0;
			CX2State* pState = (CX2State*)g_pMain->GetNowState();
			if(pState->GetCursor() != NULL)
				pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_NORMAL );

			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		}
		break;
		//{{ 2012. 02. 21	��μ�	�κ��丮 ����
	case UIM_SORT_CATEGORY_ITEM:
		{
			Handler_EGS_SORT_CATEGORY_ITEM_REQ(m_NowInventorySortType);

			return true;
		} break;
		//}}
	case UIM_USE_COUPON_ITEM_CANCLE:
		{
			// 			CX2State* pState = (CX2State*)g_pMain->GetNowState();
			// 			if ( pState->GetCursor() != NULL )
			// 			{
			// 				m_CouponUid = 0;
			// 				m_CouponTargetUid = 0;
			// 				if(pState->GetCursor() != NULL)
			// 					pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_NORMAL );
			// 			}

			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			return true;

		}
		break;
	case UIM_OPEN_RANDOM_ITEM_CANCLE:
		{
			m_eRandomItemEventType = RIOET_NONE;
			if ( m_pDLGRandomItem != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRandomItem, NULL, false );
				m_pDLGRandomItem = NULL;
			}


		}
		break;

		//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
	case UIM_REGISTER_QUANTITY_CONFIRM:
		{
			if(m_pDLGItemEnroll != NULL)
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGItemEnroll, NULL, false );

			m_pDLGItemEnroll = NULL;

			if( true == m_bIsSptNoneItem)
			{
				CX2Item* pItem		= NULL;
				pItem				= g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_EnrollItemUid );

				if ( pItem == NULL ||
					pItem->GetItemTemplet() == NULL )
					return false;
				std::wstring wstrMsg = GET_REPLACED_STRING( (STR_ID_23495, "Li", pItem->GetFullName(), m_iItemEnrollNum ) );
				std::wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( wstrMsg.c_str(), 257, 0 );

				m_pSptNoneItemDelete =
					g_pMain->KTDGUIOkAndCancelEditBox2( D3DXVECTOR2( -999, -999 ), tempName.c_str(),
					UIM_SPTNONE_ITEM_DELETE_OK, g_pMain->GetNowState(), 10, false, UIM_SPTNONE_ITEM_DELETE_CANCEL );
				m_bIsSptNoneItem = false;
				pItem = NULL;
			}
			else
			OpenDeleteItemConfirmDLG();
			return true;
		}
		break;

	case UIM_REGISTER_QUANTITY_CANCEL:
		{
			if(m_pDLGItemEnroll != NULL)
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGItemEnroll, NULL, false );

			m_bIsSptNoneItem = false;

			m_pDLGItemEnroll = NULL;
			m_iItemEnrollNum = 1;		// ������ �������� ������ Dialog Text 1�� ����
			return true;
		}
		break;

	case UIM_REGISTER_QUANTITY_PLUS:
		{
			m_iItemEnrollNum++;
			UpdateRegisterQuantityDLG(false);
			return true;
		}
		break;

	case UIM_REGISTER_QUANTITY_MINUS:
		{
			m_iItemEnrollNum--;
			UpdateRegisterQuantityDLG(false);
			return true;
		}
		break;

	case UIM_REGISTER_QUANTITY_UPDATE:
		{
			UpdateRegisterQuantityDLG(true);
			return true;
		}
		break;

	case UIM_DELETE_ITEM_CONFIRM_OK:
		{
			if ( m_pDLGDeleteItemConfirm != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGDeleteItemConfirm, NULL, false );

			Handler_EGS_DELETE_ITEM_REQ( m_EnrollItemUid, m_iItemEnrollNum );

			m_pDLGDeleteItemConfirm		= NULL;
			m_iItemEnrollNum			= 1;		// ������ �������� ������ Dialog Text 1�� ����
			m_EnrollItemUid				= 0;
			return true;
		}
		break;

	case UIM_DELETE_ITEM_CONFIRM_CANCEL:
		{
			if ( m_pDLGDeleteItemConfirm != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGDeleteItemConfirm, NULL, false );

			m_pDLGDeleteItemConfirm		= NULL;
			m_iItemEnrollNum			= 1;		// ������ �������� ������ Dialog Text 1�� ����
			m_EnrollItemUid				= 0;
			return true;
		}
		break;

		//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������

	case UIM_SPTNONE_ITEM_DELETE_OK:
		{
			if(m_pSptNoneItemDelete != NULL)
			{
				CKTDGUIIMEEditBox* pIMEEditBox = static_cast<CKTDGUIIMEEditBox*>(m_pSptNoneItemDelete->GetControl(L"IMEEditBoxMessage"));
				if( NULL != pIMEEditBox )
				{
					wstring wstrMessage = pIMEEditBox->GetText();
					if( 0 == wcscmp(wstrMessage.c_str(), GET_STRING(STR_ID_23608)) )
					{

						//Handler_EGS_DELETE_ITEM_REQ( m_EnrollItemUid, m_iItemEnrollNum );
						g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pSptNoneItemDelete, NULL, false );
						m_pSptNoneItemDelete = NULL;
						m_bIsSptNoneItem = false;
						OpenDeleteItemConfirmDLG();
					}
					else
					{
						CKTDGUIDialogType pMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_23609 ), g_pMain->GetNowState() );
						if( NULL != pMsgBox )
							pMsgBox->SetEnableMoveByDrag_LUA(false);
					}

				}
			}
		}
		break;

	case UIM_SPTNONE_ITEM_DELETE_CANCEL:
		{
			if(m_pSptNoneItemDelete != NULL)
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pSptNoneItemDelete, NULL, false );
				m_pSptNoneItemDelete = NULL;
				m_bIsSptNoneItem = false;
			}
		}
		break;

		//{{ kimhc // 2009-08-19 // �к� Ȯ�� �̳� ��Ҹ� ���� ���
	case UIM_SEAL_ITEM_CONFIRM_OK:
		{
			if ( m_pDLGSealItemConfirm != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGSealItemConfirm, NULL, false );

			if ( g_pData->GetMyUser() == NULL ||
				 g_pData->GetMyUser()->GetSelectUnit() == NULL )
			{
				ASSERT( !"GetMyuser or GetSelectUnit is NULL" );
				m_pSlotItemToBeSealed	= NULL;
				ResetItemIDForSealAndCursorState();
				return true;
			}

			if ( m_pSlotItemToBeSealed == NULL )
			{
				ASSERT( !"m_pSlotItemToBeSealed" );
				ResetItemIDForSealAndCursorState();
				return true;
			}

			Handler_EGS_SEAL_ITEM_REQ( m_pSlotItemToBeSealed->GetItemUID() );

			return true;
		}
		break;

	case UIM_SEAL_ITEM_CONFIRM_CANCEL:
		{
			if ( m_pDLGSealItemConfirm != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGSealItemConfirm, NULL, false );

			m_pDLGSealItemConfirm		= NULL;
			m_pSlotItemToBeSealed		= NULL;

			return true;
		}
		break;
		//}} kimhc // 2009-08-19 // �к� Ȯ�� �̳� ��Ҹ� ���� ���

		//{{ kimhc // 2009-09-03 // ���ε� �������� ��Ŭ�� �������� Ȯ��, ���
	case UIM_UNSEAL_ITEM_CONFIRM_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			Handler_EGS_UNSEAL_ITEM_REQ( m_itemUIDToBeUnSealed );

		}
		break;

	case UIM_UNSEAL_ITEM_CONFIRM_CANCEL:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
		}
		break;
		//}} kimhc // 2009-09-03 // ���ε� �������� ��Ŭ�� �������� Ȯ��, ���

	case UIM_WARP_DEST_CLOSE:
		{
			if( m_pDLGWarpDestination != NULL )
			{
				m_pDLGWarpDestination->SetShowEnable(false, false);
				m_iSelectedWarpDest = 0;
			}
		}
		break;
	case UIM_WARP_DEST_SELECT:
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

	case UIM_WARP_DEST:
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
	case UIM_WARP_DEST_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_WarpItem, m_iSelectedWarpDest );
			m_WarpItem = 0;
			m_iSelectedWarpDest = 0;

			if( m_pDLGWarpDestination != NULL )
			{
				m_pDLGWarpDestination->SetShowEnable(false, false);
			}

			return true;
		}
		break;

	case UIM_UNLIMITED_SECOND_CHANGE_JOB_CLOSE:
		{
			if( m_pDLGUSCJob != NULL )
			{
				m_pDLGUSCJob->SetShowEnable(false, false);
				m_iSelectedUSCJob = 0;
			}
		}
		break;
	case UIM_UNLIMITED_SECOND_CHANGE_JOB_SELECT:
		{
			CKTDGUIButton *pButton = (CKTDGUIButton*)lParam;
			if( pButton == NULL )
				return true;

			m_iSelectedUSCJob = 0;
			m_iSelectedUSCJob = pButton->GetDummyInt(0);

			for(UINT i=0; i<m_vecUSCJobButton.size(); ++i)
			{
				CKTDGUIButton *pUSCJobButton = m_vecUSCJobButton[i];
				if(pUSCJobButton != NULL)
				{
					pUSCJobButton->SetDownStateAtNormal(false);
				}
			}

			pButton->SetDownStateAtNormal(true);
		}
		break;
	case UIM_UNLIMITED_SECOND_CHANGE_JOB_DEST:
		{
			if( m_iSelectedUSCJob >= static_cast<int>( CX2Unit::UC_ELSWORD_LORD_KNIGHT ) &&
				m_iSelectedUSCJob != static_cast<int>( g_pData->GetMyUser()->GetSelectUnit()->GetClass() ) )
			{
				UseUSCJobPopup();
			}

			return true;
		}
		break;
	case UIM_UNLIMITED_SECOND_CHANGE_JOB_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			//���� ��û �Ѵ� �������� üũ�� �ؾ��Ѵ�. �������� �κ����ִ��� ���� 2�� ��������,unit���� üũ����

			if( 2 == g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() &&
				g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID ) != NULL )
			{
				KEGS_UNLIMITED_SECOND_CHANGE_JOB_NOT kPacket;
				kPacket.m_iUnlimitedSecondChangeJob = m_iSelectedUSCJob;

				g_pData->GetServerProtocol()->SendPacket( EGS_UNLIMITED_SECOND_CHANGE_JOB_NOT, kPacket );
			}

			m_iSelectedUSCJob = 0;

			if( m_pDLGUSCJob != NULL )
			{
				m_pDLGUSCJob->SetShowEnable(false, false);
			}

			return true;
		}
		break;

		//{{ oasis907 : ����� [2009.11.30] // ��� ��ų �ʱ�ȭ
	case UIM_GUILD_SKILL_INIT_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			if( g_pData->GetSkillTree() != NULL )
			{
				g_pData->GetSkillTree()->Handler_EGS_INIT_GUILD_SKILL_TREE_REQ(m_UsedItemUID);
			}
			return true;
		} break;
		//}} oasis907 : ����� [2009.11.30] //

	case UIM_SKILL_NOTE:
		{
			if( g_pData != NULL && g_pData->GetUIManager() != NULL )
			{
				g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_NOTE, true);
			}
			return true;
		}
		break;
	case UIM_UNKNOWN_TO_IDENTIFY:
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGIdentifyStone, NULL, false );
			m_pDLGIdentifyStone = NULL;
			return Handler_EGS_IDENTIFY_ITEM_REQ();
		}
		break;
	case UIM_PET_FOOD:
		{
			FeedPet();
		}
		break;
	case UIM_PET_FOOD_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			//if( g_pMain->GetInfoBox() != NULL )
			//	g_pMain->DeleteInfoBox();

			if( g_pData != NULL && g_pData->GetPetManager() != NULL && m_pPetFoodSlot != NULL )
			{
				g_pData->GetPetManager()->Handler_EGS_FEED_PETS_REQ( m_pPetFoodSlot->GetItemUID() );
				m_pPetFoodSlot = NULL;
			}
		}
		break;
	case UIM_PET_INVENTORY_OPEN:
		{
			if(m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() == true)
			{
				m_pUIPetInventory->SetShow(false);
			}
			else
			{
				g_pData->GetUIManager()->PetInventoryForceCloseOtherDlg();
				OpenPetInventory();
			}
		}
		break;

	case UIM_ITEM_USE_DEFENCE_DUNGEON_POTION:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_DefencedungeonPotion, 0, m_DefencedungeonPotionID );
		}
		break;
	case UIM_CHECK_PSHOP_OK:
		{
			if ( m_pDLGCheckPShop != NULL )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCheckPShop, NULL, false );
			m_pDLGCheckPShop = NULL;
			g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_PERSONAL_SHOP, true, CX2UIPersonalShop::OPENFORSELL);
		}
		//{{ 2011.10.24 ������ : ��� Ƽ�� �۾� ( ����ϸ� ĳ�� �ִ� ������ )
		//}}
	case UIM_PET_DROP_ITEM_PICK_UP_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl == NULL )
				return true;

			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_UsedItemUID, m_UseItemTempUID );

			m_UsedItemUID = 0;
			m_UseItemTempUID = 0;
			return true;
		} break;

	case UIM_PET_HALLOWEEN_TRANSFORM_POSION_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl == NULL )
				return true;

			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			g_pData->GetPetManager()->Handler_EGS_SUMMON_PET_REQ( 0 ); // �� ��ȯ ���� ����!!!

			return true;
		} break;

	case UIM_USE_RESTORE_ITEM_EVALUATION_OK:
		{
			Handler_EGS_RESTORE_ITEM_EVALUATE_REQ();

			CX2State* pState = static_cast<CX2State*>(g_pMain->GetNowState());
			if( NULL != pState && NULL != pState->GetCursor() )
				pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_NORMAL );

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		}
		break;
	case UIM_USE_RESTORE_ITEM_EVALUATION_CANCEL:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		} break;
	case UIM_USE_EVALUATE_ITEM_OK:
		{
			Handler_EGS_ITEM_EVALUATE_REQ();

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		}
		break;
	case UIM_USE_EVALUATE_ITEM_CANCEL:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		} break;
	case UIM_EXCHANGE_NEW_ITEM_OK:
		{
			Handler_EGS_ITEM_CONVERT_REQ( m_iTargetItemUID, m_iConsumeItemQuantity );
			m_iTargetItemUID = -1;
			m_iConsumeItemQuantity = -1;

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		}
		break;
	case UIM_EXCHANGE_NEW_ITEM_CANCEL:
		{
			m_iTargetItemUID = -1;
			m_iConsumeItemQuantity = -1;

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		} break;

	case UIM_RIDING_ITEM_USE_OK:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			CX2RidingPetManager::GetInstance()->Handler_EGS_CREATE_RIDING_PET_REQ( m_iRidingItemUid );
			m_iRidingItemUid = 0;

			return true;
		} break;

	case UIM_EL_BREAK_EXIT:
		{
			if( m_pDLGElBreak != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGElBreak, NULL, false );
			}
			m_pDLGElBreak = NULL;
			return true;
		} break;
	case UIM_EL_BREAK_NUM_PLUS:
		{
			++m_ElBreakItemCount;
			UpdateElBreakDLG( false );
			return true;
		} break;
	case UIM_EL_BREAK_NUM_MINUS:
		{
			--m_ElBreakItemCount;
			UpdateElBreakDLG( false );
			return true;
		} break;
	case UIM_EL_BREAK_NUM_UPDATE:
		{
			UpdateElBreakDLG( true );
			return true;
		} break;
	case UIM_EL_BREAK_OK:
		{
			if( m_pDLGElBreak != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGElBreak, NULL, false );
			}
			m_pDLGElBreak = NULL;

			// �������� Ư���� �κ� ���� �˻�
			int iMaxSlot = 0;
			int iUnUseSlot = 0;

			if ( g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetSelectUnit() != NULL )
			{
				iMaxSlot = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum( CX2Inventory::ST_SPECIAL );
				iUnUseSlot = iMaxSlot - g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetUsedSlotNum(CX2Inventory::ST_SPECIAL );
			}

			if( m_ElBreakItemCount < 300 )
			{
				if( iUnUseSlot >= 1 )
				{
					return Handler_EGS_ITEM_EXTRACT_REQ();
				}
				else
				{
					// �κ��丮 Ư�� ���� �� ������ X �� Ȯ�� �ؾ� �մϴ�.
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_5058, "i", 1 ) ), g_pMain->GetNowState() );
					return true;
				}
			}
			else
			{
				if( iUnUseSlot >= 1 )
				{
					return Handler_EGS_ITEM_EXTRACT_REQ();
				}
				else
				{
					// �κ��丮 Ư�� ���� �� ������ X �� Ȯ�� �ؾ� �մϴ�.
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_5058, "i", 6 ) ), g_pMain->GetNowState() );
					return true;
				}
			}

			return true;
		} break;
	case UIM_EL_BREAK_RESULT_OK:
		{
			return true;
		}
		break;

	case UIM_EXPAND_INVENTORY_CASH:
		{
			CX2StateField*	pStateField	=	NULL;
			pStateField		=  static_cast< CX2StateField* >( g_pMain->GetNowState() );

			if ( pStateField != NULL )
			{
				if ( g_pTFieldGame != NULL )
				{
					g_pTFieldGame->CloseFieldName( 0.0f );
				}

				g_pData->GetCashShop()->SetMenuTypeCallingCashShop( CX2UIManager::UI_MENU_PRIVATE_BANK );
				pStateField->ToggleCashShop();
			}

			return true;
		} break;
	case UIM_USE_ADD_SOCKET_SLOT_OK:
		{
			Handler_EGS_SOCKET_EXPAND_ITEM_REQ();

			m_iTargetItemUID = -1;
			m_iConsumeItemQuantity = -1;

			CX2State* pState = static_cast<CX2State*>(g_pMain->GetNowState());
			if( NULL != pState && NULL != pState->GetCursor() )
				pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_NORMAL );

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		} break;
	case UIM_USE_ADD_SOCKET_SLOT_CANCEL:
		{
			m_iTargetItemUID = -1;
			m_iConsumeItemQuantity = -1;

			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( NULL != pControl )
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );

			return true;
		} break;


	case UIM_USE_PROPOSE_ITEM:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			SAFE_DELETE_DIALOG(m_pDLGMarryEventNickName);
			m_pDLGMarryEventNickName = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_MarryEvent_Nickname.lua" );
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMarryEventNickName );

			return true;
		} break;
	case UIM_USE_PROPOSE_ITEM_OK:
		{
			if( m_pDLGMarryEventNickName != NULL )
			{
				CKTDGUIIMEEditBox* pIMEEditID = (CKTDGUIIMEEditBox*)m_pDLGMarryEventNickName->GetControl( L"IMEEditBoxName" );

				if( pIMEEditID != NULL )
				{
					CX2State* pState = (CX2State*)(g_pMain->GetNowState());
					if( pState != NULL )
					{
						wstring wstrNickName = pIMEEditID->GetText();
						Handler_EGS_USE_PROPOSE_ITEM_REQ( m_UsedItemUID, wstrNickName );
					}
				}

				CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
				if ( pControl != NULL )
				{
					g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
				}
			}
			return true;
		} break;
	case UIM_USE_PROPOSE_ITEM_CANCLE:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			return true;
		} break;
	case UIM_USE_DIVORCE_ITEM:
		{
			CKTDGUIControl* pControl = reinterpret_cast<CKTDGUIControl*>(lParam);
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			Handler_EGS_USE_DIVORCE_ITEM_REQ();
			return true;
		} break;


	case UIM_USE_EXPAND_INVENTORY_OK:
		{
			CKTDGUIControl* pControl = (CKTDGUIControl*)lParam;
			if ( pControl != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, pControl->GetDialog(), NULL, false );
			}

			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_UsedItemUID );
			return true;
		} break;
	default:
		break;
	}
	return false;
}

bool CX2UIInventory::UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// oasis907 : ����� [2010.4.5] //
	if(m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true)
	{
		if ( m_pUISocketItem->UIServerEventProc(hWnd, uMsg, wParam, lParam) == true)
		{
			return true;
		}
	}

	// oasis907 : ����� [2010.9.8] // �� �κ��丮
	if(m_pUIPetInventory != NULL )
	{
		if ( m_pUIPetInventory->UIServerEventProc(hWnd, uMsg, wParam, lParam) == true)
		{
			return true;
		}
	}

	switch ( wParam )
	{
	case EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK:
		{
			return Handler_EGS_CHANGE_INVENTORY_SLOT_ACK( hWnd, uMsg, wParam, lParam );
		}break;

		//{{ 2007. 10. 8  ������  ���� ������ ACK
	case EGS_OPEN_RANDOM_ITEM_ACK:
		{
			return Handler_EGS_OPEN_RANDOM_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
		//}}

	case EGS_RESOLVE_ITEM_ACK:
		{
			return Handler_EGS_RESOLVE_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
	case EGS_IDENTIFY_ITEM_ACK:
		{
			return Handler_EGS_IDENTIFY_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_USE_ITEM_IN_INVENTORY_ACK :
		{
			return Handler_EGS_USE_ITEM_IN_INVENTORY_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_ITEM_EXPIRATION_NOT:
		{
			return Handler_EGS_ITEM_EXPIRATION_NOT( hWnd, uMsg, wParam, lParam );
		}
		break;

		//{{ kimhc // 2009-08-10 // ���� ����� �÷��ִ� ����Ʈ �������� ��� ������ ���� ��Ŷ

	case EGS_EXPAND_BANK_SLOT_NOT:	// ���� or �ʵ忡�� ��� ����� ���� �ϵ���???
		{
			return Handler_EGS_EXPAND_BANK_SLOT_NOT( hWnd, uMsg, wParam, lParam );
		}
		break;
		//}} kimhc // 2009-08-10 // ���� ����� �÷��ִ� ����Ʈ �������� ��� ������ ���� ��Ŷ

		//{{ kimhc // 2009-08-27 // ������ ����
	case EGS_SEAL_ITEM_ACK:
		{
			return Handler_EGS_SEAL_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;

	case EGS_UNSEAL_ITEM_ACK:
		{
			return Handler_EGS_UNSEAL_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		}
		break;
		//}} kimhc // 2009-08-27 // ������ ����

	case EGS_RESTORE_ITEM_ACK:
		{
			return Handler_EGS_RESTORE_ITEM_ACK( hWnd, uMsg, wParam, lParam );

		} break;
	case EGS_ENCHANT_ATTACH_ITEM_ACK:
		{
			return Handler_EGS_ENCHANT_ATTACH_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_ATTRIB_ATTACH_ITEM_ACK:
		{
			return Handler_EGS_ATTRIB_ATTACH_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_DELETE_ITEM_ACK:
		{
			return Handler_EGS_DELETE_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;

		//{{ 2012. 02. 21	��μ�	�κ��丮 ����
	case EGS_SORT_CATEGORY_ITEM_ACK:
		{
			return Handler_EGS_SORT_CATEGORY_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;
		//}}

	case EGS_UPDATE_ITEM_POSITION_NOT:
		{
			return Handler_EGS_UPDATE_ITEM_POSITION_NOT( hWnd, uMsg, wParam, lParam );
		} break;


	case EGS_ITEM_EVALUATE_ACK:
		{
			return Handler_EGS_ITEM_EVALUATE_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_RESTORE_ITEM_EVALUATE_ACK:
		{
			return Handler_EGS_RESTORE_ITEM_EVALUATE_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_ITEM_CONVERT_ACK:
		{
			return Handler_EGS_ITEM_CONVERT_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_ITEM_EXTRACT_ACK:
		{
			return Handler_EGS_ITEM_EXTRACT_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_SOCKET_EXPAND_ITEM_ACK:
		{
			return Handler_EGS_SOCKET_EXPAND_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_USE_PROPOSE_ITEM_ACK:
		{
			return Handler_EGS_USE_PROPOSE_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;
	case EGS_USE_DIVORCE_ITEM_ACK:
		{
			return Handler_EGS_USE_DIVORCE_ITEM_ACK( hWnd, uMsg, wParam, lParam );
		} break;

	case EGS_CUBE_IN_ITEM_MAPPING_NOT:
		{
			return Handler_EGS_CUBE_IN_ITEM_MAPPING_NOT( hWnd, uMsg, wParam, lParam );
		}
		break;

	}

	return false;
}


void CX2UIInventory::SetLayer(X2_DIALOG_LAYER layer)
{
	if( NULL != m_pDLGItemHighLight )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer(m_pDLGItemHighLight, layer);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGItemHighLight, false );
	}
	SetSlotManagerLayer(layer);
	SetSlotManagerChangeSequence(false);
	if(m_pDLGUIInventory != NULL)
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer(m_pDLGUIInventory, layer);
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGUIInventory, false );
	}
	//m_pDLGUIInventory->SetLayer(layer);

	//SetSlotManagerLayer(layer);


}

void CX2UIInventory::SetShow(bool val)
{
	m_bShow = val;
	//SetEnable(m_bShow);
	//SetShow(m_bShow);

	if(val)	// ���� �� ó���ؾ� �� �κ�
	{
		D3DXVECTOR3 tmp;
		tmp = m_pDLGUIInventory->GetDummyPos(0);
		m_DLGPosition.x = tmp.x;
		m_DLGPosition.y = tmp.y;
		tmp = m_pDLGUIInventory->GetDummyPos(1);
		m_DLGSize.x = tmp.x;
		m_DLGSize.y = tmp.y;

		UpdateUIButton();

		SetInventorySort(m_NowInventorySortType, GetNowInvenSortTypePageNum());

		if(g_pData != NULL && g_pData->GetUIManager() != NULL)
		{
			g_pData->GetUIManager()->UIOpened(CX2UIManager::UI_MENU_INVEN);
			g_pData->GetUIManager()->GetUIQuestNew()->ShowUIDesc(false);
		}


		// ���ο� �������� ������ ��� �κ��� ������ ���ο� ������ ���翩�� ����
		m_bIsNewItem = false;

		if( NULL != m_pDLGItemHighLight )
			m_pDLGItemHighLight->SetShow(true);

		//���� �Է�
		if( NULL != g_pData && NULL != g_pData->GetPlayGuide() )
		{	//�κ��丮 ������ �� ��� ȹ�� ���̵� ����
			g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_OBTAIN_EQUIP_ITEM, false);
		}
		//������Ʈ �Ϸ� ��ƼŬ�� UI�� ������ �����ϵ��� ����
		if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIQuestNew() )
		{
			g_pData->GetUIManager()->GetUIQuestNew()->ClearQuestCompleteParticle();
		}
		SetEDString( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED );
		SetAPString( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iAPoint );
	}
	else	// ���� �� ó���ؾ� �� �κ�
	{
		if(m_pSptNoneItemDelete != NULL)
		{
			g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pSptNoneItemDelete, NULL, false );
			m_pSptNoneItemDelete = NULL;
			m_bIsSptNoneItem = false;
		}

		for ( int i = 0; i < (int)m_SlotList.size(); i++ )
		{
			CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
			pItemSlot->LostItemUI();
			pItemSlot->SetShow( false );
			pItemSlot->SetEnable( false );
		}

		// ������ ���콺 ���� �̹����� ������
		if(m_pDLGSelectedItem != NULL) m_pDLGSelectedItem->SetShow( false );
		InvalidSlotDesc();


		// Ŀ���� ������� �ٲ�����
		CX2State* pState = (CX2State*)g_pMain->GetNowState();	// DownCast�ϱ� �Ƚ�
		if(pState != NULL)
		{
			CX2Cursor* pCursor = pState->GetCursor();
			if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_NORMAL )
			{
				pCursor->ChangeCursorState(CX2Cursor::XCS_NORMAL);
			}
		}

		//Ȥ�� �����ϰ� ���� �� �ִ� ���� ���̾�α׸� ��� ��������
		// ���� : ���λ����� �� �����
		SAFE_DELETE_DIALOG(m_pDLGResolveItem);
		SAFE_DELETE_DIALOG(m_pDLGResolveItemResult);
		SAFE_DELETE_DIALOG(m_pDLGSocketItem);
		SAFE_DELETE_DIALOG(m_pDLGSocketItemPushOk);
		SAFE_DELETE_DIALOG(m_pDLGSocketItemCheat);
		SAFE_DELETE_DIALOG(m_pDLGCubeCheck);
		SAFE_DELETE_DIALOG(m_pDLGElChanger );
		SAFE_DELETE_DIALOG(m_pDLGElChangerResult);
		SAFE_DELETE_DIALOG(m_pDLGRandomItem);

		SAFE_DELETE_DIALOG(m_pDLGOpenAttraction);
		SAFE_DELETE_DIALOG(m_pDLGAttractionResult);
		SAFE_DELETE_DIALOG(m_pDLGGetAttractionItemBG);
		SAFE_DELETE_DIALOG(m_pDLGEventCubeResultImage);

		//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
		SAFE_DELETE_DIALOG(m_pDLGItemEnroll);
		//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������

		SAFE_DELETE_DIALOG( m_pDLGCheckPShop );

		SAFE_DELETE_DIALOG( m_pDLGElBreak );

		// oasis907 : ����� [2010.4.5] //
		SAFE_DELETE(m_pUISocketItem);

		SAFE_DELETE_DIALOG(m_pDLGCubeOpenEffect);
		m_fCubeOpenTime = 0.f;
		m_bCubeJustOpen = false;
		m_eItemObtainResultType = IORT_NONE;

		// oasis907 : ����� [2010.9.9] //
		if(m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() == true)
		{
			m_pUIPetInventory->SetShow(false);
		}

		g_pData->GetUIManager()->UIClosed(CX2UIManager::UI_MENU_INVEN);

		if( m_pDLGWarpDestination != NULL )
			m_pDLGWarpDestination->SetShowEnable(m_bShow, m_bShow);

		if( m_pDLGUSCJob != NULL )
			m_pDLGUSCJob->SetShowEnable(m_bShow, m_bShow);

		if( NULL != m_pDLGItemHighLight )
			m_pDLGItemHighLight->SetShow(false);

		SAFE_DELETE_DIALOG( m_pDLGBuyConfirm )
	}

	m_pDLGUIInventory->SetShowEnable(m_bShow, m_bShow);
	//g_pKTDXApp->SkipFrame();
}

//{{ kimhc // 2009-08-10 // ���� ���� Ȯ��
bool CX2UIInventory::Handler_EGS_EXPAND_BANK_SLOT_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )		// ���� ���� Ȯ��
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_EXPAND_BANK_SLOT_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	CX2User*	  pUser		 = g_pData->GetMyUser();
	if( pUser == NULL )
	{
		ASSERT( !"User is NULL(X2User)" );
		return false;
	}

	CX2Unit*	  pUnit		 = pUser->GetSelectUnit();
	if( pUnit == NULL)
	{
		ASSERT( !"Unit is NULL(X2Unit)" );
		return false;
	}

	CX2Inventory& kInventory = pUnit->AccessInventory();

	map< int, int >::iterator mit;
	for ( mit = kEvent.m_mapExpandedCategorySlot.begin(); mit != kEvent.m_mapExpandedCategorySlot.end(); mit++ )
	{
		kInventory.SetItemMaxNum( (CX2Inventory::SORT_TYPE)mit->first,
			kInventory.GetItemMaxNum((CX2Inventory::SORT_TYPE)mit->first) + mit->second );

		switch( static_cast< CX2Inventory::SORT_TYPE >( mit->first ) )
		{
		case CX2Inventory::ST_E_QUICK_SLOT:
			if( NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
			{
				g_pData->GetUIManager()->GetUIQuickSlot()->SetExpandQuickSlot( kInventory.GetItemMaxNum( CX2Inventory::ST_E_QUICK_SLOT ) );	// �ƽ� ���� �ø���
			}
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_16083 ),g_pMain->GetNowState() );
			break;
		case CX2Inventory::ST_SPECIAL:
			ChangeInventoryTab( CX2Inventory::ST_SPECIAL );
			break;
		case CX2Inventory::ST_BANK:
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_3868 ),g_pMain->GetNowState() );
			break;
		default:
			break;
		}
	}

	if( NULL != g_pData->GetQuestManager() )
	{
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	}
	return true;
}
//}} kimhc // 2009-08-10 // ���� ���� Ȯ��

bool CX2UIInventory::Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( CX2SlotItem* pFromCX2SlotItem, CX2SlotItem* pToCX2SlotItem )
{
	if ( pFromCX2SlotItem == NULL || pToCX2SlotItem == NULL )
		return false;

	if ( pFromCX2SlotItem->GetSortType() == pToCX2SlotItem->GetSortType() && pFromCX2SlotItem->GetSlotID() == pToCX2SlotItem->GetSlotID() )
		return false;

	//{{ �м� �Ǽ��縮 ������ ���� �ذ��� ���� ���� ó�� �ڵ� : �������� �������� �켱 ���ܹ�����

	if( ( pFromCX2SlotItem->GetSortType() == CX2Inventory::ST_E_EQUIP && pToCX2SlotItem->GetSortType() != CX2Inventory::ST_E_EQUIP ) ||
		( pFromCX2SlotItem->GetSortType() != CX2Inventory::ST_E_EQUIP && pToCX2SlotItem->GetSortType() == CX2Inventory::ST_E_EQUIP ) )
	{
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
		CX2Item* pFromItem = kInventory.GetItem( pFromCX2SlotItem->GetItemUID(), true );
		CX2Item* pToItem = kInventory.GetItem( pToCX2SlotItem->GetItemUID(), true );
		if(pFromItem != NULL && pToItem != NULL )
		{
			const CX2Item::ItemTemplet* pFromItemTemplet = pFromItem->GetItemTemplet();
			const CX2Item::ItemTemplet* pToItemTemplet = pToItem->GetItemTemplet();
			if(pFromItemTemplet != NULL && pToItemTemplet != NULL )
			{
				if( pFromItemTemplet->GetItemType() == CX2Item::IT_ACCESSORY &&
					pFromItemTemplet->GetEqipPosition() == pToItemTemplet->GetEqipPosition() &&
					pFromItemTemplet->GetFashion() != pToItemTemplet->GetFashion() )
				{
					// �������� �������� �켱 ���ܹ��� �ڿ� ���� �Ϸ��� ��ȯ�� ��� �����Ѵ�

					CX2SlotItem* pEquippedItemSlot = NULL;

					if(pFromCX2SlotItem->GetSortType() == CX2Inventory::ST_E_EQUIP )
					{
						pEquippedItemSlot = pFromCX2SlotItem;
					}
					else if( pToCX2SlotItem->GetSortType() == CX2Inventory::ST_E_EQUIP )
					{
						pEquippedItemSlot = pToCX2SlotItem;
					}


					// ������ �������� �����
					{
						CX2Inventory::SORT_TYPE NowInventorySortType = kInventory.GetSortTypeByID( pEquippedItemSlot->GetItemTID() );
						for ( int i = 0; i < kInventory.GetItemMaxNum( NowInventorySortType ); i++ )
						{
							// ���ڸ� ã�Ƽ�
							CX2Item* pItem = kInventory.GetItem( NowInventorySortType, i );
							if ( pItem == NULL )
							{
								Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( pEquippedItemSlot->GetSortType(), pEquippedItemSlot->GetSlotID(),
									NowInventorySortType, i );

								// ���� ������ �����ְ�
								pEquippedItemSlot->DestroyItemUI();
								break;
							}
						}
					}
				}
				// ������ �������� �����
			}

		}

	}
	//}}

	KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ;

	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType	= pFromCX2SlotItem->GetSortType();
	if ( pFromCX2SlotItem->GetSlotType() == CX2Slot::ST_INVENTORY )
	{

		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iFromSlotID	= pFromCX2SlotItem->GetSlotID();

		//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ��ü
		if ( pFromCX2SlotItem->GetSortType() == CX2Inventory::ST_QUICK_SLOT &&
			pToCX2SlotItem->GetSortType() == CX2Inventory::ST_E_QUICK_SLOT )
		{
			g_pData->GetUIManager()->GetUIQuickSlot()->UpdateCoolTime( pFromCX2SlotItem->GetItemTID() );
		}
		//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ������ ��ü
	}
	else
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iFromSlotID	= pFromCX2SlotItem->GetSlotID();
	}


	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType	= pToCX2SlotItem->GetSortType();


	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iToSlotID		= pToCX2SlotItem->GetSlotID();
	}
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = -1;
	if( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIPrivateBank() != NULL )
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = g_pData->GetUIManager()->GetUIPrivateBank()->GetShareUnitUID();
	}
	if( (kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType == CX2Inventory::ST_SHARE_BANK && kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType != CX2Inventory::ST_SHARE_BANK) ||
		(kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType != CX2Inventory::ST_SHARE_BANK && kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType == CX2Inventory::ST_SHARE_BANK) )
	{
		CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();
		CX2Item* pFromItem = kInventory.GetItem( pFromCX2SlotItem->GetItemUID(), true );
		CX2Item* pToItem = kInventory.GetItem( pToCX2SlotItem->GetItemUID(), true );

		// �ŷ� �������� üũ �սô�.
		if(pFromItem != NULL || pToItem != NULL)
		{
			if(CheckCanMoveShareItem(pFromItem, pToItem, kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ) == false)
			{
				return false;
			}
		}
	}
	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK );

	return true;
}


bool CX2UIInventory::Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( CX2Inventory::SORT_TYPE fromSortType, int fromSlotID, CX2Inventory::SORT_TYPE toSortType, int toSlotID )
{
	if ( fromSortType == toSortType && fromSlotID == toSlotID )
		return false;

	KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ;

	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType	= fromSortType;
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iFromSlotID	= fromSlotID;
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType	= toSortType;
	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iToSlotID		= toSlotID;

	kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = -1;
	if( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIPrivateBank() != NULL )
	{
		kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_iShareUnitUID = g_pData->GetUIManager()->GetUIPrivateBank()->GetShareUnitUID();
	}
	if( (kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType == CX2Inventory::ST_SHARE_BANK && kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType != CX2Inventory::ST_SHARE_BANK) ||
		(kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cFromSlotType != CX2Inventory::ST_SHARE_BANK && kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ.m_cToSlotType == CX2Inventory::ST_SHARE_BANK) )
	{
		CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();
		CX2Item* pFromItem = kInventory.GetItem( fromSortType, fromSlotID );
		CX2Item* pToItem = kInventory.GetItem( toSortType, toSlotID );

		// �ŷ� �������� üũ �սô�.
		if(pFromItem != NULL || pToItem != NULL)
		{
			if(CheckCanMoveShareItem(pFromItem, pToItem, kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ) == false)
			{
				return false;
			}
		}
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, kEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_CHANGE_INVENTORY_SLOT_ACK(  HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CHANGE_INVENTORY_SLOT_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK ) == true )
	{
		//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ��� �� �ƹ�Ÿ ��ü
		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_DUNGEON_GAME:
		case CX2Main::XS_BATTLE_FIELD:
		case CX2Main::XS_TRAINING_GAME:
			{
				if ( g_pX2Game == NULL || g_pX2Game->GetMyUnit() == NULL )
					return false;

				CX2GUUser* pMyUser = g_pX2Game->GetMyUnit();

				if ( NULL == pMyUser ||
					 NULL == pMyUser->GetUnit() )
					 return false;

				for ( KInventoryItemInfo& kInventoryItemInfo : kEvent.m_vecInventorySlotInfo )	// ����� ����
				{
					if ( kInventoryItemInfo.m_cSlotCategory == CX2Inventory::ST_E_EQUIP )
					{
						CX2GUUser*	pUser			= g_pX2Game->GetMyUnit();
						CX2Item*	pItemToAdd		= pUser->GetUnit()->GetInventory().GetItem( kInventoryItemInfo.m_iItemUID );	// ������ ���
						CX2Item*	pItemToRemove	= pUser->GetUnit()->GetInventory().GetItem(
							static_cast< CX2Inventory::SORT_TYPE >( kInventoryItemInfo.m_cSlotCategory ),
							kInventoryItemInfo.m_sSlotID );

						if ( pItemToRemove != NULL )
							g_pX2Game->RemoveSpecialAbilityInEquip( pItemToRemove->GetItemTemplet(), pUser );

						if ( pItemToAdd != NULL )
							g_pX2Game->SetSpecialAbilityInEquip( pItemToAdd->GetItemTemplet(), pUser );

						g_pX2Game->GetMyUnit()->UpdateEquipViewListInfo( pItemToAdd, pItemToRemove );

						if ( kInventoryItemInfo.m_sSlotID == _CONST_UIINVENTORY_::g_iEquippedNormalWeaponSlotID ||
							 kInventoryItemInfo.m_sSlotID == _CONST_UIINVENTORY_::g_iEquippedFashionWeaponslotID )
						{
						} // if

						break;
					}
				}

				UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );

				//{{ kimhc // 2010.12.11 // 2010-12-23 New Character CHUNG
				pMyUser->SetShowViewList( true );
				pMyUser->UpdateNotRenderingOtherEquipList();
				pMyUser->UpdateEquipCollisionData();

				pMyUser->SetEnableAttackBox( L"Rfoot", false );
				pMyUser->SetEnableAttackBox( L"Lfoot", false );
				pMyUser->SetEnableAttackBox( L"Lhand2", false );
				//}} kimhc // 2010.12.11 //  2010-12-23 New Character CHUNG


				pMyUser->UpdateWeaponEnchantEffectAndExtraDamageType();
			}
			break;

		default:
			{
				UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
				g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );

				if ( g_pSquareGame != NULL )
				{
					CX2SquareUnit* pSquareUnit = g_pSquareGame->GetSquareUnitByUID( g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
					if ( pSquareUnit != NULL )
					{
						pSquareUnit->GetUnitViewer()->UpdateEqip( true );
					}
				}
				else if( NULL != g_pTFieldGame )
				{
					CX2SquareUnit* pSquareUnit = g_pTFieldGame->GetSquareUnitByUID( g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
					if ( pSquareUnit != NULL )
					{
						pSquareUnit->GetUnitViewer()->UpdateEqip( true );
					}
				}
				if ( g_pX2Room != NULL )
				{
					CX2Room::SlotData* pSlotData = g_pX2Room->GetMySlot();
					if ( pSlotData != NULL && pSlotData->m_pUnitViewer != NULL )
					{
						pSlotData->m_pUnitViewer->UpdateEqip( true );
					}
				}
			}
			break;
		}
		//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ��� �� �ƹ�Ÿ ��ü

		if(g_pData->GetUIManager()->GetUIQuickSlot() != NULL)
			g_pData->GetUIManager()->GetUIQuickSlot()->ResetQuickSlotUI();

		// oasis907 : ����� [2010.11.17] // �κ��丮(��, ����, �κ�) �� ����Ʈ ���� ������ ��ȯ�� �� ����Ʈ â �ǽð� �ݿ�
		g_pData->GetUIManager()->GetUIQuestNew()->UpdateQuickQuestDLG();

		return true;
	}

	return false;
}

//{{ 2012. 02. 21	��μ�	�κ��丮 ����
bool CX2UIInventory::Handler_EGS_SORT_CATEGORY_ITEM_REQ(CX2Inventory::SORT_TYPE NowInventorySortType)
{
	KEGS_SORT_CATEGORY_ITEM_REQ kEGS_SORT_CATEGORY_ITEM_REQ;
	kEGS_SORT_CATEGORY_ITEM_REQ.m_iCategoryType = NowInventorySortType;

	g_pData->GetServerProtocol()->SendPacket( EGS_SORT_CATEGORY_ITEM_REQ, kEGS_SORT_CATEGORY_ITEM_REQ);
	g_pMain->AddServerPacket( EGS_SORT_CATEGORY_ITEM_ACK );

	return true;
}
//}}

bool CX2UIInventory::Handler_EGS_ITEM_EXPIRATION_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ITEM_EXPIRATION_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( true == kEvent.m_vecItemUID.empty() )
	{
		StateLog( L"ExpiredItemEmpty" );
		return true;
	}


	//{{ kimhc // �ǽð� ���ҵ� �� �ǽð� ��� �� �ƹ�Ÿ ��ü

	if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
	{

		if ( g_pX2Game == NULL ||
			g_pX2Game->GetMyUnit() == NULL ||
			g_pX2Game->GetMyUnit()->GetUnit() == NULL )
			return false;

		CX2GUUser*					pMyUser = g_pX2Game->GetMyUnit();
		CX2Unit*					pUnit	= pMyUser->GetUnit();

		if ( kEvent.m_vecItemUID.empty() == false )
		{
			wstring tempString = L"";

			for ( int i = 0; i < (int)kEvent.m_vecItemUID.size(); i++ )
			{
				UidType		itemUID = kEvent.m_vecItemUID[i];
				CX2Item*	pItem	= pUnit->GetInventory().GetItem( itemUID );

				if ( pItem == NULL ||
					 pItem->GetItemTemplet() == NULL
                     )
					continue;

				if ( pUnit->GetInventory().CheckEquippingItem(
                    pItem->GetItemTemplet()->GetItemID()
					, pItem->GetUID()
                    ) == true )
				{
					g_pX2Game->RemoveSpecialAbilityInEquip( pItem->GetItemTemplet(), pMyUser );
					pMyUser->UpdateEquipViewListInfo( NULL, pItem );
				}
			}
		}

		//{{ kimhc // 2010.12.11 // 2010-12-23 New Character CHUNG

		pMyUser->SetShowViewList( true );
		pMyUser->UpdateNotRenderingOtherEquipList();
		pMyUser->UpdateEquipCollisionData();

		pMyUser->SetEnableAttackBox( L"Rfoot", false );
		pMyUser->SetEnableAttackBox( L"Lfoot", false );
		pMyUser->SetEnableAttackBox( L"Lhand2", false );
		//}} kimhc // 2010.12.11 //  2010-12-23 New Character CHUNG
	}

	//}} kimhc // �ǽð� ���ҵ� �� �ǽð� ��� �� �ƹ�Ÿ ��ü

	wstring wstrExpirationMessage = L"";

	for ( std::vector<UidType>::iterator it = kEvent.m_vecItemUID.begin(); it < kEvent.m_vecItemUID.end(); it++ )
	{
		UidType itemUID = *it;



		// 1. �������� ��������� � �������� ���������� �̸� ����~
		CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( itemUID, true );
		if( NULL != pItem )
		{
			wstrExpirationMessage += L"[";
			wstrExpirationMessage += pItem->GetFullName();
			wstrExpirationMessage += L"] ";
		}




		// 2. unitviewer���� ������ Equip���� �����ϰ�, ( �ŷ�����, ����, ����, ��ĳ������â, ĳ�ü�, ��������â�� unitviewerui )
		if ( g_pSquareGame != NULL )
		{
			CX2SquareUnit* pSquareUnit = g_pSquareGame->GetSquareUnitByUID( g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
			if ( pSquareUnit != NULL )
			{
				pSquareUnit->GetUnitViewer()->RemoveEqip( itemUID );
			}
		}
		else if( NULL != g_pTFieldGame )
		{
			CX2SquareUnit* pSquareUnit = g_pTFieldGame->GetSquareUnitByUID( g_pData->GetMyUser()->GetSelectUnit()->GetUID() );
			if ( pSquareUnit != NULL )
			{
				pSquareUnit->GetUnitViewer()->RemoveEqip( itemUID );
			}
		}

		if ( g_pX2Room != NULL )
		{
			CX2Room::SlotData* pSlotData = g_pX2Room->GetMySlot();
			if ( pSlotData != NULL && pSlotData->m_pUnitViewer != NULL )
			{
				pSlotData->m_pUnitViewer->RemoveEqip( itemUID );
			}
		}

		if( NULL != g_pData->GetUIManager()->GetUICharInfo() &&
			NULL != g_pData->GetUIManager()->GetUICharInfo()->GetUnitViewerUI() )
		{
			g_pData->GetUIManager()->GetUICharInfo()->GetUnitViewerUI()->RemoveEqip( itemUID );
		}


		if( NULL != g_pData->GetCashShop() &&
			NULL != g_pData->GetCashShop()->GetUnitViewerUI() )
		{
			g_pData->GetCashShop()->GetUnitViewerUI()->RemoveEqip( itemUID );
		}

		if( CX2Main::XS_SERVER_SELECT == g_pMain->GetNowStateID() )
		{
			CX2StateServerSelect* pState = (CX2StateServerSelect*) g_pMain->GetNowState();
			if( NULL != pState )
			{
				if( NULL != g_pData->GetMyUser()->GetSelectUnit() )
				{
					CX2UnitViewerUI* pUnitViewer = pState->GetUnitViewer( g_pData->GetMyUser()->GetSelectUnit() );
					if( NULL != pUnitViewer )
					{
						pUnitViewer->RemoveEqip( itemUID );
					}
				}
			}
		}


		// 3. �� ���������� ���� ������ �κ��丮���� �������� �����Ѵ�.
		g_pData->GetMyUser()->GetSelectUnit()->RemoveEqip( itemUID );
		g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().RemoveItem( itemUID );



	} // for()



	if ( wstrExpirationMessage.empty() == false )
	{
		//{{ kimhc // �Ⱓ�� �����ٴ� �˸��� ä�� â���� ����
		if( g_pChatBox != NULL)
		{
			std::wstringstream strStream;
			strStream << GET_REPLACED_STRING( ( STR_ID_195, "L", wstrExpirationMessage ) );

			wstring wstrColor = L"#CECEC88";							// (236, 236, 136)
			D3DXCOLOR coTextColor(0.92549f, 0.92549f, 0.53333f, 1.f);	// (236, 236, 136)

			g_pChatBox->AddChatLog( strStream.str().c_str(), KEGS_CHAT_REQ::CPT_TOTAL, coTextColor, wstrColor );

		}

		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_PVP_GAME:
		case CX2Main::XS_PVP_RESULT:
		case CX2Main::XS_DUNGEON_RESULT:
		case CX2Main::XS_TRAINING_GAME:
		case CX2Main::XS_TRAINING_RESULT:
			break;

		default:
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_195, "L", wstrExpirationMessage ) ), g_pMain->GetNowState() );
			break;
		}
		//}} kimhc // �Ⱓ�� �����ٴ� �˸��� ä�� â���� ����
	}


	if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO) )
	{
		g_pData->GetUIManager()->GetUICharInfo()->ResetNowEquipUI();
		g_pData->GetUIManager()->GetUICharInfo()->ResetStat();
	}

	if( NULL != g_pData->GetCashShop() )
	{
		g_pData->GetCashShop()->ResetUnitViewerEquip();
	}

	// �κ��丮 ������Ʈ
	SetInventorySort( m_NowInventorySortType );

	// oasis907 : ����� [2010.9.16] // �� �κ��丮�� �ִ� ������ �Ⱓ �����
	if(m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() == true)
	{
		m_pUIPetInventory->UpdateUI();
	}

	if( g_pData->GetUIManager() == NULL || g_pData->GetUIManager()->GetUIQuestNew() == NULL )
	{
		return true;
	}

	if( g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_QUEST_NEW ) == true )
	{
		g_pData->GetUIManager()->GetUIQuestNew()->ResetQuestUI();
	}

	if( g_pData->GetUIManager()->GetUIQuestNew()->GetShowQuickQuestDLG() == true )
	{
		g_pData->GetUIManager()->GetUIQuestNew()->UpdateQuickQuestDLG();
	}

	return true;
}










bool CX2UIInventory::UpdateInventorySlot()
{

	return SetInventorySort(m_NowInventorySortType, GetNowInvenSortTypePageNum());

}


bool CX2UIInventory::UpdateInventorySlotList( std::vector< KInventoryItemInfo >& vecInventorySlotInfo, bool bInventoryUIUpdate /*= true*/ )
{

	CX2Unit*		pMyUnit = g_pData->GetMyUser()->GetSelectUnit();

	ASSERT( pMyUnit != NULL );
	if( pMyUnit == NULL )
	{
		StateLog( L"updating inven failed - null unit" );
		return false;
	}

	CX2Inventory&	kMyInventory = pMyUnit->AccessInventory();

	//{{ mauntain : ����ȯ [2012.07.10] �������� �Һ� ������ ȹ��� ������ ����
	//}}



	for( const KInventoryItemInfo& invenItemInfo : vecInventorySlotInfo )
	{
		if( invenItemInfo.m_cSlotCategory == CX2Inventory::ST_E_EQUIP )
		{
			// ���� ��� ����
			CX2Item* pItem = kMyInventory.GetItem( (CX2Inventory::SORT_TYPE)invenItemInfo.m_cSlotCategory, invenItemInfo.m_sSlotID );
			if ( pItem != NULL )
			{
				pMyUnit->RemoveEqip( pItem->GetUID() );
			}
		}

		// �κ����� ������ ����
		kMyInventory.RemoveItem( (CX2Inventory::SORT_TYPE) invenItemInfo.m_cSlotCategory, invenItemInfo.m_sSlotID );
	}

	bool bIsUpdate = false;

	for( const KInventoryItemInfo& invenItemInfo : vecInventorySlotInfo )
	{
		if( invenItemInfo.m_iItemUID <= 0 )
			continue;

		// �κ��� ������ �ٽ� �߰�
        {
		    CX2Item::ItemData kItemData( invenItemInfo );
		    kMyInventory.AddItem( (CX2Inventory::SORT_TYPE)invenItemInfo.m_cSlotCategory, invenItemInfo.m_sSlotID, kItemData );
        }

		if( (CX2Inventory::SORT_TYPE)invenItemInfo.m_cSlotCategory == m_NowInventorySortType )
		{
			SetNowInventoryPageNum( m_NowInventorySortType, GetPageNumBySlotID(invenItemInfo.m_sSlotID) );
			bIsUpdate = true;
		}

		// ���� ��� �ٽ� �߰�
		if ( invenItemInfo.m_cSlotCategory == CX2Inventory::ST_E_EQUIP )
		{
			CX2Item* pItem = kMyInventory.GetItem( (CX2Inventory::SORT_TYPE)invenItemInfo.m_cSlotCategory, invenItemInfo.m_sSlotID );
			if ( pItem != NULL )
			{
				pMyUnit->AddEqip( pItem->GetUID() );
			}
		}

		//{{ mauntain : ����ȯ [2012.07.10] �������� �Һ� ������ ȹ��� ������ ����
		//}}
	}

	if( bIsUpdate == true )
	{
		if( UpdatePageUI( GetNowInventoryPageNum( m_NowInventorySortType) ) == true )
		{
			SetInventorySort( m_NowInventorySortType, GetNowInventoryPageNum( m_NowInventorySortType ) );
		}
	}


	// �� ����â ����
	if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
	{
		g_pData->GetUIManager()->GetUICharInfo()->EquipChanged();
	}

	//{{ kimhc // 2009-08-07 // ĳ���ͺ� ����
	if(g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_PRIVATE_BANK) == true )
	{
		g_pData->GetUIManager()->GetUIPrivateBank()->UpdateUI();
	}
	//}} kimhc // 2009-08-07 // ĳ���ͺ� ����

	//{{ oasis907 : ����� [2010.9.6] //
	if(m_pUIPetInventory != NULL && m_pUIPetInventory->GetShow() == true)
	{
		m_pUIPetInventory->UpdateUI();
	}
	//}}

	//{{ mauntain : ����ȯ [2012.07.10] �������� �Һ� ������ ȹ��� ������ ����
	//}}

	g_pMain->GetInformerManager()->GetInvenInformer()->SetNeedCheckInfo( true );

	// �κ��丮 ������Ʈ
	if( true == bInventoryUIUpdate )
	{

		if ( SetInventorySort( m_NowInventorySortType, GetNowInvenSortTypePageNum() ) == false )
			return false;
	}


	if( NULL != g_pData->GetQuestManager() )
	{
		g_pData->GetQuestManager()->GiveUpForbiddenQuest();
	}

	g_pMain->GetMemoryHolder()->UpdateNewYear2014Event();

	return true;
}




bool CX2UIInventory::ChangeInventoryTab( CX2Inventory::SORT_TYPE sortType )
{
	return SetInventorySort( sortType );
}

bool CX2UIInventory::SetInventorySort( CX2Inventory::SORT_TYPE sortType, int pageNum )
{
	if ( m_bShow == false )
		return false;

	if ( CheckRadioButtonBySortType( sortType ) == false )
		return false;

	m_NowInventorySortType = sortType;


	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();


	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		if ( pItemSlot->GetSlotType() == CX2Slot::ST_INVENTORY )
		{
// 			if(true == pItemSlot->IsCreatedItemUI())
// 			{
// 				pItemSlot->LostItemUI();
// 			}
			pItemSlot->SetShow( false );
			pItemSlot->SetEnable( false );
			pItemSlot->SetSortType( sortType );
		}

	}

	//{{ 2011.03.16   �ӱԼ� �ƹ�Ÿ ���� �ý���
	//}}
	//{{ 2011.05.04   �ӱԼ� �ƹ�Ÿ �ռ� �ý���
	//}}
	/// �ش� �κ��丮 ī�װ����� �ִ� ������ �� ����
	int iNowPageSlotNum = GetMaxLineNumPerCategory( m_NowInventorySortType );
	iNowPageSlotNum -= ( ( pageNum - 1 ) * MY_INVENTORY_MAX_SLOT_HEIGHT );

	for( int i = 0; i < MY_INVENTORY_SHOW_SLOT_NUM_PER_PAGE / MY_INVENTORY_MAX_SLOT_WIDTH; i++ )
	{
		WCHAR buff[256] = {0,};

		StringCchPrintf( buff, 256, L"g_pStaticInvenSlot%d", i+1);
		CKTDGUIStatic* pSlotLine = (CKTDGUIStatic*) m_pDLGUIInventory->GetControl( buff );
		if(NULL != pSlotLine)
		{
			if( i < iNowPageSlotNum )
				pSlotLine->SetColor(D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f));
			else
				pSlotLine->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));
		}
	}

	CKTDGUIStatic* pStatic = NULL;
	if( m_pDLGItemHighLight != NULL )
	{
		pStatic = (CKTDGUIStatic*)m_pDLGItemHighLight->GetControl(L"g_pStaticInvenHighLight");
	}
	if( NULL != pStatic )
	{
		for ( int slotIndex = 0; slotIndex < MY_INVENTORY_SHOW_SLOT_NUM_PER_PAGE; ++slotIndex)
		{//New Item ���̶���Ʈ ��� ����
			if( NULL != pStatic->GetPicture(slotIndex) )
			{
				pStatic->GetPicture(slotIndex)->SetShow(false);
			}
		}
	}

	int iLineIndex = 0;
	// slotID : Inventory ���� ���� ID
	// slotIndex : UI���� ����
	for ( int slotID = ((pageNum-1)*MY_INVENTORY_SHOW_SLOT_NUM_PER_PAGE), slotIndex = 0;
		slotID < kInventory.GetItemMaxNum( m_NowInventorySortType ) && slotIndex < MY_INVENTORY_SHOW_SLOT_NUM_PER_PAGE;
		slotID++, slotIndex++ )
	{
		//CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(slotIndex);
		CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(slotID);
		if ( pItemSlot == NULL )
			continue;

		if ( pItemSlot->GetSlotType() == CX2Slot::ST_INVENTORY )
		{
			pItemSlot->SetShow( true, false );
			pItemSlot->SetEnable( true, false );

			if(slotIndex == iLineIndex*MY_INVENTORY_MAX_SLOT_WIDTH )
			{
				WCHAR buff[256] = {0,};
				//wsprintf( buff, L"g_pStaticInvenSlot%d", iLineIndex+1);
				StringCchPrintf( buff, 256, L"g_pStaticInvenSlot%d", iLineIndex+1);
				CKTDGUIStatic* pSlotLine = (CKTDGUIStatic*) m_pDLGUIInventory->GetControl( buff );
				if(NULL != pSlotLine)
				{
					pSlotLine->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
				}
				iLineIndex++;
			}
		}

		CX2Item* pItem = kInventory.GetItem( m_NowInventorySortType, slotID );

		if( NULL != pItem )
		{
			UidType iItemUID = pItem->GetItemData().m_ItemUID;
			bool	bNewItem = kInventory.IsNewItem(m_NowInventorySortType, iItemUID );

			if( true == bNewItem && NULL != pStatic &&  NULL != pStatic->GetPicture(slotIndex) )
			{//������ ��� �ʱ�ȭ ���� �ֱ� ������, true�϶��� ����
				pStatic->GetPicture(slotIndex)->SetShow(bNewItem);
			}
		}

	}

	for ( int slotIndex = 0; slotIndex < MY_INVENTORY_SHOW_MAX_SLOT_NUM; slotIndex++ )
	{
		CX2Item* pItem = kInventory.GetItem( m_NowInventorySortType, slotIndex );
		for ( int j = 0; j < (int)m_SlotList.size(); j++ )
		{
			CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(j);
			if ( pItemSlot == NULL )
				continue;

			if ( pItemSlot->GetSlotType() == CX2Slot::ST_INVENTORY && pItemSlot->GetSlotID() == slotIndex )
			{
				if(pItem != NULL)
				{
					if ( pItemSlot->IsCreatedItemUI() == true )
						pItemSlot->ResetItemUI( pItem );
					else
						pItemSlot->CreateItemUI( pItem );

					CKTDGUIDialogType pkDiag = pItemSlot->GetDialog();
					//	���� �������� �ƴϰ� �巡���ϰ��ִ� �������� �ƴϸ� ������ �ʰ� ó���Ѵ�.
					if( GetPageNumBySlotID( slotIndex ) != pageNum && *m_pSlotBeforeDragging != pItemSlot )
					{
						pkDiag->SetShow(false);
					}
				}
				else
				{
					pItemSlot->DestroyItemUI();
				}

				break;
			}
		}
	}

	//{{ 2011.03.16   �ӱԼ� �ƹ�Ÿ ���� �ý���
	//}}
	//{{ 2011.05.04   �ӱԼ� �ƹ�Ÿ �ռ� �ý���
	//}}
	g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGItemHighLight, true );

	/// �κ��丮 �ִ� ������ �� ����
	int iMaxPageNum = GetMaxLineNumPerCategory( m_NowInventorySortType ) / MY_INVENTORY_MAX_SLOT_HEIGHT;	/// ����( 56�� )���� �� �� ������ �� ����

	if( 0 < GetMaxLineNumPerCategory( m_NowInventorySortType ) % MY_INVENTORY_MAX_SLOT_HEIGHT )				/// �̿��� �� ä���� ���� �������� �ִ��� ����
		++iMaxPageNum;	/// ������ �� �߰�

	SetShowInventoryPageUI( iMaxPageNum );	/// ������ ��ư ǥ�� ó��

	if(g_pData != NULL && g_pData->GetUIManager() != NULL)
	{
		g_pData->GetUIManager()->UIOpened(CX2UIManager::UI_MENU_INVEN);
	}
	return true;
}

bool CX2UIInventory::ChangeInventoryTabByUid( UidType iItemUID )
{
	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
	CX2Item* pItem = kInventory.GetItem( iItemUID, true );

	if( pItem == NULL)
		return false;

	if( kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ) == m_NowInventorySortType &&
		*m_pSlotBeforeDragging != NULL &&
		CX2SlotItem::CX2Slot::ST_INVENTORY == ((*m_pSlotBeforeDragging)->GetSlotType()) )
	{
		return true;
	}

	[[maybe_unused]] int invenPage = 1;

	for ( int i = 0; i < kInventory.GetItemMaxNum( kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ) ); i++ )
	{
		CX2Item* pItemTemp = kInventory.GetItem( kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ), i );
		if ( pItemTemp == NULL )
		{
			int showPage = i / MY_INVENTORY_SHOW_MAX_SLOT_NUM;
			showPage++;

			if ( showPage != 1 )
				invenPage = showPage;
			break;
		}
	}

	CX2Inventory::SORT_TYPE eSortType = kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() );

	if( eSortType == GetSortType() )
	{
		return true;
	}
	else
	{
		UpdatePageUI( GetNowInventoryPageNum( eSortType ) );						// SortType�� ST_AVATAR�� �Ѿ�� �����̹Ƿ� SortType üũ���� �ʵ��� ��.
		return SetInventorySort( eSortType, GetNowInventoryPageNum( eSortType ) );
	}

	//	if ( kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ) != m_NowInventorySortType
	//		|| invenPage != m_NowInvenSortTypePageNum )
	//	{
	//		if ( pItem != NULL && pItem->GetItemTemplet() != NULL )
	//		{
	//			wstring radioButtonName = GetRadioButtonNameByInvenSortType( kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ) );
	//			CKTDGUIRadioButton* pRadioButtonInvenSortType = (CKTDGUIRadioButton*)m_pDLGMyInfoInventory->GetControl( radioButtonName.c_str() );
	//			if ( pRadioButtonInvenSortType != NULL )
	//			{
	//				pRadioButtonInvenSortType->GetCheckedControl()->SetChecked( false );
	//				pRadioButtonInvenSortType->SetChecked( true );
	//
	//
	//
	//				SetInventorySort( kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ), invenPage );
	//			}
	//		}
	//	}

}


int CX2UIInventory::GetNowInvenSortTypePageNum()
{
	if( CX2Inventory::ST_PC_BANG == m_NowInventorySortType )	/// PC�游 ���� ó��
		return 1;
	else
		return GetNowInventoryPageNum( m_NowInventorySortType );
}

bool CX2UIInventory::MouseDown( D3DXVECTOR2 mousePos )
{

	CX2SlotItem* pSlot = (CX2SlotItem*)GetSlotInMousePos( mousePos );

	if ( pSlot == NULL )
		return false;

	if ( pSlot->IsResetItemUI() == false )
	{
		return false;
	}

	if ( pSlot->GetSlotType() != CX2SlotItem::ST_INVENTORY )
		return false;

	CX2State* pState = (CX2State*)g_pMain->GetNowState();
	if(pState != NULL)
	{
		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_NORMAL )
		{
			return true;
		}
	}

	*m_pSlotBeforeDragging = pSlot;
	//g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( (*m_pSlotBeforeDragging)->GetDialog(), true );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), GetDraggingItemLayer() );
	(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,0.7f) );

	*m_DraggingItemUID = pSlot->GetItemUID();
	ChangeInventoryTabByUid(*m_DraggingItemUID);

	g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );


	if (m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true)
	{
		if(m_pUISocketItem->IsStoneItem(*m_DraggingItemUID))
		{
			m_pUISocketItem->SocketEquipReadyEffectStart();
		}
	}

	return true;
}


bool CX2UIInventory::MouseUp( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pSlot = (CX2SlotItem*)GetSlotInMousePos( mousePos );
	if(pSlot == NULL)
	{
		//	���� �������� �ƴ� �巡�� �������̸� ��������(������ �ƴѰ��� ����ؼ� �������� �ǵ�����)
		if( *m_pSlotBeforeDragging != NULL &&
			m_NowInventoryPageNum[m_NowInventorySortType - 1] != GetPageNumBySlotID( (*m_pSlotBeforeDragging)->GetSlotID() ) )
		{
			CX2Slot::SLOT_TYPE eType = (*m_pSlotBeforeDragging)->GetSlotType();
			if( eType != CX2Slot::ST_BANK &&
				eType != CX2Slot::ST_EQUIPPED &&
				eType != CX2Slot::ST_PET )
			{
				(*m_pSlotBeforeDragging)->SetShow(false);
			}
		}

		return false;
	}

	// ���콺 Ŭ�� �� ���� �׼��� ���⿡ ����

	CX2State* pState = (CX2State*)g_pMain->GetNowState();
	if(pState != NULL)
	{
		CX2Cursor* pCursor = pState->GetCursor();
		if ( pCursor != NULL && pCursor->GetCurorState() != CX2Cursor::XCS_NORMAL )
		{
			UidType ItemUID = pSlot->GetItemUID();
			CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( ItemUID );

			if(pItem == NULL )
				return false;


			g_pData->GetUIManager()->SetDLGFlag(false);
			switch(pCursor->GetCurorState())
			{
			case CX2Cursor::XCS_SELL:
				{
					if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP))
						g_pData->GetUIManager()->GetUIShop()->SellItem( pSlot->GetItemUID(), g_pKTDXApp->MouseConvertByResolution(mousePos) );
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_793 ), g_pMain->GetNowState() );

					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}

				}
				return true;
			case CX2Cursor::XCS_REPAIR:
				{
					if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP))
						g_pData->GetUIManager()->GetUIShop()->RepairItem( pSlot->GetItemUID() );
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_794 ), g_pMain->GetNowState() );
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
				}
				return true;
			case CX2Cursor::XCS_DECOMPOSITION:
				if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

					return true;
				}
				if(m_pDLGResolveItem == NULL && m_pDLGResolveItemResult == NULL	)
				{
					OpenResolveWindow( pSlot );
				}
				return true;
			case CX2Cursor::XCS_ENCHANT:
				{
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
					if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP))
						g_pData->GetUIManager()->GetUIShop()->EnchantItem( pSlot->GetItemUID(), g_pKTDXApp->MouseConvertByResolution(mousePos) );
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_795 ), g_pMain->GetNowState() );
				}
				return true;
			case CX2Cursor::XCS_SOCKET:
				if(m_pDLGSocketItem == NULL)
				{
//{{ oasis907 : ����� [2010.4.19] //
					CX2State* pState = (CX2State*)g_pMain->GetNowState();
					if(pState != NULL)
					{
						CX2Cursor* pCursor = pState->GetCursor();
						if ( pCursor != NULL)
						{
							pCursor->ChangeCursorState(CX2Cursor::XCS_NORMAL);
						}
					}

					if ( true == OpenNewSocketWindow( pSlot ) )
						ChangeInventoryTab(CX2Inventory::ST_SPECIAL);

				}
				return true;
			case CX2Cursor::XCS_ATTRIB_ENCHANT:
				{
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
					if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP))
						g_pData->GetUIManager()->GetUIShop()->EnchantAttributeItem( pSlot->GetItemUID(), g_pKTDXApp->MouseConvertByResolution(mousePos) );
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_796 ), g_pMain->GetNowState() );
				}
				return true;
			case CX2Cursor::XCS_ATTACH_ATTRIB_ITEM:
			case CX2Cursor::XCS_ATTACH_ITEM:
				{
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
					// ó���ϰ�
					if( pSlot->GetSortType() == CX2Inventory::ST_EQUIP )
					{
						m_CouponTargetUid = pSlot->GetItemUID();
						CX2Item* pCouponItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_CouponUid );
						CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_CouponTargetUid );

						//{{ kimhc // 2010-01-06 // PC�� �����̾� ����
						if ( pTargetItem != NULL &&
                            pTargetItem->GetItemTemplet() != NULL &&
                            pTargetItem->GetItemTemplet()->GetIsPcBang() == true )
						{
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
							return true;
						}
						//}} kimhc // 2010-01-06 // PC�� �����̾� ����

						if(pCouponItem != NULL || pTargetItem != NULL)
						{
							if( IsAttributeItem( pCouponItem->GetItemData().m_ItemID ) )
							{
								if(pTargetItem->GetItemTemplet()->GetItemType() != CX2Item::IT_WEAPON)
								{
									g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_10324 ), g_pMain->GetNowState() );
								}
								else
								{
									wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_10283, "SSL", pTargetItem->GetItemTemplet()->GetFullName_(), pCouponItem->GetItemTemplet()->GetFullName_(), GetAttributeItemExtraDamageName(pCouponItem->GetItemData().m_ItemID) ) );
									g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), UIM_USE_ATTRIBUTE_CHARM_COUPON_ITEM_OK, g_pMain->GetNowState(), UIM_USE_COUPON_ITEM_CANCLE );
								}
							}
							else
							{
								wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_797, "iSS", pTargetItem->GetItemData().m_EnchantLevel, pTargetItem->GetItemTemplet()->GetFullName_(), pCouponItem->GetItemTemplet()->GetFullName_() ) );
								g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), UIM_USE_COUPON_ITEM_OK, g_pMain->GetNowState(), UIM_USE_COUPON_ITEM_CANCLE );
							}
						}
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_798 ), g_pMain->GetNowState() );
					}
				} return true;
				//case CX2Cursor::XCS_ITEM_EXCHANGE:
			case CX2Cursor::XCS_DELETE_ITEM:
				{
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
					if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
						g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_SELL )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_2650 ), g_pMain->GetNowState() );
						return true;
					}

						//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
						CX2Item* pItem		= NULL;
						m_EnrollItemUid		= pSlot->GetItemUID();
						pItem				= g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_EnrollItemUid );

						if ( pItem == NULL ||
							pItem->GetItemTemplet() == NULL )
							return false;

						if ( g_pData->GetMyUser()->GetAuthLevel() != CX2User::XUAL_DEV )
						{
							//{{ 2010. 11. 29	������	�Ⱓ���� ����


							//}}

			//{{ kimhc // 2009-09-08 // ���ε� ������ ������ �Ұ�
							if ( pItem->GetItemData().m_bIsSealed == true )
							{
								g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4477 ), g_pMain->GetNowState() );
								return true;
							}
			//}} kimhc // 2009-09-08 // ���ε� ������ ������ �Ұ�
						}
						//pc�� ������ ����ó��
						if( true == pItem->GetItemTemplet()->GetIsPcBang() )
						{
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
							return true;
						}

						if ( pItem->GetItemTemplet()->GetShopPriceType() == CX2Item::SPT_NONE )
						{
							if( pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_QUEST )
							{
								CKTDGUIDialogType pMsgBox = g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_23610 ), g_pMain->GetNowState() );
								if( NULL != pMsgBox )
									pMsgBox->SetEnableMoveByDrag_LUA(false);

								return true;
							}
							m_bIsSptNoneItem = true;
						}

						// ���������� �ƴ��� ���� �������̸�
						if( CX2Item::PT_QUANTITY == pItem->GetItemTemplet()->GetPeriodType() )
						{
							D3DXVECTOR2 Pos;
							Pos.x = static_cast< float >( g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos() );
							Pos.y = static_cast< float >( g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos() );

							m_iItemEnrollNum	= pItem->GetItemData().m_Quantity;

							OpenRegisterQuantityDLG( Pos );
						}
						else		// �ƴϸ�
						{
							m_iItemEnrollNum	= 1;
							if( true == m_bIsSptNoneItem)
							{
								std::wstring wstrMsg = GET_REPLACED_STRING( (STR_ID_23495, "Li", pItem->GetFullName(), pItem->GetItemData().m_Quantity) );
								std::wstring tempName = CWordLineHandler::GetStrByLineBreakInX2Main( wstrMsg.c_str(), 257, 0 );

								m_pSptNoneItemDelete =
									g_pMain->KTDGUIOkAndCancelEditBox2( D3DXVECTOR2( -999, -999 ), tempName.c_str(),
									UIM_SPTNONE_ITEM_DELETE_OK, g_pMain->GetNowState(), 10, false, UIM_SPTNONE_ITEM_DELETE_CANCEL );
							}
							else
							OpenDeleteItemConfirmDLG();
						}
						pItem = NULL;
						//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������


				} return true;

				//{{ kimhc // 2009-08-19 // ������ ����(��Ī)�� ��Ŭ�� �� ���� �ϰ��� �ϴ� �������� Ŭ���� ���
			case CX2Cursor::XCS_SEAL_ITEM:
				{
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
					if ( CanSealUpThisItem( pItem ) == true )
					{
						m_pSlotItemToBeSealed	= pSlot;

						// ���� Ȯ�� ���̾�α�
						m_pDLGSealItemConfirm	= g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999, -999 ),
							GET_REPLACED_STRING( ( STR_ID_4400, "L", pItem->GetFullName() ) ),
							UIM_SEAL_ITEM_CONFIRM_OK , g_pMain->GetNowState(), UIM_SEAL_ITEM_CONFIRM_CANCEL );

						m_pDLGSealItemConfirm->SetEnableMoveByDrag_LUA(false);

						CKTDGUIButton* pButton	= static_cast< CKTDGUIButton* >( m_pDLGSealItemConfirm->GetControl( L"MsgBoxOkAndCancelOkButton" ) );

						if ( pButton != NULL )
							m_pDLGSealItemConfirm->RequestFocus( pButton );

					}
					else
					{
						ResetItemIDForSealAndCursorState();
					}

				} return true;
				//}} kimhc // 2009-08-19 // ������ ����(��Ī)�� ��Ŭ�� �� ���� �ϰ��� �ϴ� �������� Ŭ���� ���
			case CX2Cursor::XCS_PET_FOOD:
				{
					m_pPetFoodSlot = pSlot;
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_5352 ),
						UIM_PET_FOOD_OK, (CKTDXStage*) g_pMain->GetNowState() );
				}
				return true;
			case CX2Cursor::XCS_ITEM_EXCHANGE:
				{
					if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
					{
						//g_pData->GetUIManager()->GetUIItemExchangeShop()->InvalidLastSrcItem();
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );

						return true;
					}
					if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_ITEM_EXCHANGE_SHOP))
					{
                        int iSelectedItemID = pItem->GetItemTemplet()->GetItemID();
						if( true == g_pData->GetUIManager()->GetUIItemExchangeShop()->CheckExchangeUseItem(iSelectedItemID) )
						{
							g_pData->GetUIManager()->GetUIItemExchangeShop()->InvalidLastSrcItem();
							if( true == g_pData->GetUIManager()->GetUIItemExchangeShop()->SelectExchangeSrcItem(ItemUID) )
								pSlot->SetShowExchangeSelectImage(true);

							CX2State* pState = (CX2State*)g_pMain->GetNowState();
							if(pState->GetCursor() != NULL)
								pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_NORMAL );
						}
						else
						{
							//g_pData->GetUIManager()->GetUIItemExchangeShop()->InvalidLastSrcItem();
							g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_13860 ), g_pMain->GetNowState() );
						}
					}
					else
					{
						//g_pData->GetUIManager()->GetUIItemExchangeShop()->InvalidLastSrcItem();
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_796 ), g_pMain->GetNowState() );
					}
				}
			case CX2Cursor::XCS_ITEM_EVALUTE:
				{
					// ���� �۾� �� ó�� ���� �ʱ�
					if( NULL != GetUISocketItem() && true == GetUISocketItem()->GetShow() )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );
						return true;
					}

					if( NULL != g_pData->GetMyUser() &&
						NULL != g_pData->GetMyUser()->GetSelectUnit() )
					{
						m_iTargetItemUID = pSlot->GetItemUID();

						const CX2Item* pConsumeItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iConsumeItemUID );
						const CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iTargetItemUID );

						if( NULL != pConsumeItem && NULL != pConsumeItem->GetItemTemplet() &&
							NULL != pTargetItem && NULL != pTargetItem->GetItemTemplet() )
						{
							if( true == pTargetItem->GetItemTemplet()->GetIsPcBang() )
							{
								g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
								return true;
							}

							wstring wstrMsg =
								GET_REPLACED_STRING( ( STR_ID_24613, "SS",
													pTargetItem->GetItemTemplet()->GetFullName_(),
													pConsumeItem->GetItemTemplet()->GetFullName_() ) );

							g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(),
								UIM_USE_RESTORE_ITEM_EVALUATION_OK, g_pMain->GetNowState(), UIM_USE_RESTORE_ITEM_EVALUATION_CANCEL );
						}
					}
				} return true;
			case CX2Cursor::XCS_EXCHANGE_NEW_ITEM:
				{
					if( NULL != pSlot &&
						NULL != g_pData &&
						NULL != g_pData->GetItemManager() )
					{
						const CX2Item::ItemTemplet* pTargetItem = g_pData->GetItemManager()->GetItemTemplet( pSlot->GetItemTID() );
						if( NULL != pTargetItem )
						{
							//////////////////////
							const int iResultItemID = g_pData->GetItemManager()->GetConvertResultItemID( pTargetItem->GetItemID() );
							if( 0 == iResultItemID )
							{
								g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING(STR_ID_25008), g_pMain->GetNowState() );
							}
							else
							{
								const CX2Item::ItemTemplet* pResultItem = g_pData->GetItemManager()->GetItemTemplet( iResultItemID );
								if( NULL != pResultItem )
								{
									m_iTargetItemUID = pSlot->GetItemUID();
									m_iConsumeItemQuantity = 1;
									if( NULL != g_pData->GetMyUser() &&
										NULL != g_pData->GetMyUser()->GetSelectUnit() )
									{
										CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iTargetItemUID );
										if( NULL != pTargetItem )
										{
											m_iConsumeItemQuantity = pTargetItem->GetItemData().m_Quantity;
										}
									}

									wstring wstrMsg = L"";

									if( 1 < m_iConsumeItemQuantity ||
										CX2UnitManager::NUI_LUTO_VILLAGE == g_pTFieldGame->GetJoinNpcId() )
									{ // ������ ������ ����
										wstrMsg =
											GET_REPLACED_STRING( ( STR_ID_26940, "SiSi", pTargetItem->GetFullName_(), m_iConsumeItemQuantity,
											pResultItem->GetFullName_(), m_iConsumeItemQuantity ));
									}
									else
									{
										wstrMsg =
											GET_REPLACED_STRING( ( STR_ID_25009, "SiSi", pTargetItem->GetFullName_(), pTargetItem->GetUseLevel(),
											pResultItem->GetFullName_(), pResultItem->GetUseLevel() ));
									}

									g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(),
										UIM_EXCHANGE_NEW_ITEM_OK, g_pMain->GetNowState(), UIM_EXCHANGE_NEW_ITEM_CANCEL );

								}
							}
						}
					}
					return true;
				} break;
			case CX2Cursor::XCS_ADD_SOCKET_SLOT:
				{
					// ���� �۾� �� ó�� ���� �ʱ�
					if( NULL != GetUISocketItem() && true == GetUISocketItem()->GetShow() )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5111 ), g_pMain->GetNowState() );
						return true;
					}

					if( NULL != g_pData->GetMyUser() &&
						NULL != g_pData->GetMyUser()->GetSelectUnit() )
					{
						m_iTargetItemUID = pSlot->GetItemUID();

						CX2Item* pConsumeItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iConsumeItemUID );
						CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iTargetItemUID );

						if( NULL != pConsumeItem && NULL != pTargetItem )
						{
							if( NULL != pConsumeItem->GetItemTemplet() && NULL != pTargetItem->GetItemTemplet() )
							{
								// �� / ���� Ÿ�� �˻�
								if( ITEM_ID_ADD_WEAPON_SOCKET_SLOT == pConsumeItem->GetItemData().m_ItemID &&
									CX2Item::IT_WEAPON != pTargetItem->GetItemTemplet()->GetItemType() )
								{
									g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_28329 ), g_pMain->GetNowState() );
									return true;
								}
								else if( ITEM_ID_ADD_DEFENCE_SOCKET_SLOT == pConsumeItem->GetItemData().m_ItemID &&
									     CX2Item::IT_DEFENCE != pTargetItem->GetItemTemplet()->GetItemType() )
								{
									g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_28330 ), g_pMain->GetNowState() );
									return true;
								}

								// ��뿡 ���� ������, ���� ��� ���� ����
								wstring wstrMsg =
									GET_REPLACED_STRING( ( STR_ID_24613, "SS",
									pTargetItem->GetItemTemplet()->GetFullName_(),
									pConsumeItem->GetItemTemplet()->GetFullName_() ) );

								g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(),
									UIM_USE_ADD_SOCKET_SLOT_OK, g_pMain->GetNowState(), UIM_USE_ADD_SOCKET_SLOT_CANCEL );
							}
						}
					}
				} break;
			default:
				break;
			}
			return true;
		}
	}

	if ( *m_pSlotBeforeDragging != NULL )
	{
		bool bCheck = false;

		//{{ kimhc // �ǽð� ���ҵ� �� ���������� ������ �׾��ų�, wait, mpCharge ���°� �ƴҶ� ��� ��ü ����
		if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
		{
			if ( g_pX2Game == NULL || g_pX2Game->GetMyUnit() == NULL )
				return false;

			if( g_pX2Game->GetMyUnit()->IsTransformed() )
				return false;

			if ( g_pX2Game->GetMyUnit()->GetNowHp() <= 0 )
			{
				g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2691 ),g_pMain->GetNowState() );

				return false;
			}

			if ( g_pX2Game->GetMyUnit()->GetFutureStateID() != g_pX2Game->GetMyUnit()->GetWaitStateID() &&
				g_pX2Game->GetMyUnit()->GetFutureStateID() != g_pX2Game->GetMyUnit()->GetChargeMpStateID()
				&& g_pX2Game->GetMyUnit()->GetFutureStateID() != g_pX2Game->GetMyUnit()->GetRidingWaitStateID()

				)
				return false;
		}
		//}} kimhc // �ǽð� ���ҵ� �� ���������� ������ �׾��ų�, wait, mpCharge ���°� �ƴҶ� ��� ��ü ����

		bCheck = OnDropAnyItem(mousePos);		// �κ��丮 ���Կ� �������ǿ�

		if ( *m_pSlotBeforeDragging != NULL && (*m_pSlotBeforeDragging)->GetDialog() != NULL )
		{
			g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), (*m_pSlotBeforeDragging)->GetNormalLayer() );
			(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,1) );
		}

		if(bCheck)
		{
			*m_pSlotBeforeDragging = NULL;
			*m_DraggingItemUID = -1;
			return true;
		}
	}

	// 	if ( *m_pSlotBeforeDragging != NULL && (*m_pSlotBeforeDragging)->GetDialog() != NULL )
	// 	{
	// 		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeLayer( (*m_pSlotBeforeDragging)->GetDialog(), GetItemLayer() );
	// 		(*m_pSlotBeforeDragging)->GetDialog()->SetColor( D3DXCOLOR(1,1,1,1) );
	// 	}

	//*m_pSlotBeforeDragging = NULL;
	//*m_DraggingItemUID = -1;

	return false;
}

//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
void CX2UIInventory::OpenDeleteItemConfirmDLG()
{

	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_EnrollItemUid );
	if ( pItem != NULL )
	{
		if ( pItem->GetItemTemplet() != NULL )
		{
			m_pDLGDeleteItemConfirm	= g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999, -999 ),
				GET_REPLACED_STRING( ( STR_ID_2626, "Li", pItem->GetFullName(), m_iItemEnrollNum ) ), UIM_DELETE_ITEM_CONFIRM_OK , g_pMain->GetNowState(), UIM_DELETE_ITEM_CONFIRM_CANCEL );
			m_pDLGDeleteItemConfirm->SetEnableMoveByDrag_LUA(false);

			CKTDGUIButton* pButton	= static_cast< CKTDGUIButton* >( m_pDLGDeleteItemConfirm->GetControl( L"MsgBoxOkAndCancelOkButton" ) );

 			if ( pButton != NULL )
 			{
// 				pButton->ChangeMouseUpSound( L"Item_Sell.ogg" );
 				m_pDLGDeleteItemConfirm->RequestFocus( pButton );

 			}
		}
	}
}

bool CX2UIInventory::GetOpenDeleteItemConfirmDLG()
{
	if(m_pDLGDeleteItemConfirm != NULL && m_pDLGDeleteItemConfirm->GetShow())
		return true;

	return false;

}

bool CX2UIInventory::GetOpenRegisterQuantityDLG()
{
	if ( m_pDLGItemEnroll != NULL && m_pDLGItemEnroll->GetShow() )
	{
		return true;
	}

	return false;
}

void CX2UIInventory::OpenRegisterQuantityDLG( D3DXVECTOR2 /*pos*/ )
{

	SAFE_DELETE_DIALOG(m_pDLGItemEnroll);

	m_pDLGItemEnroll = new CKTDGUIDialog(g_pMain->GetNowState(), L"DLG_UI_An.lua" );

	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGItemEnroll );
	m_pDLGItemEnroll->SetPos( g_pMain->GetWindowPos( D3DXVECTOR2(1024-865, 768-633), D3DXVECTOR2(75,20)) );

	CKTDGUIStatic* pStaticPrice			= static_cast < CKTDGUIStatic* >( m_pDLGItemEnroll->GetControl( L"Static_Price" ) );
	CKTDGUIStatic* pStaticEDMark		= static_cast < CKTDGUIStatic* >( m_pDLGItemEnroll->GetControl( L"Static_EDMark" ) );

	pStaticPrice->SetShow(false);
	pStaticEDMark->SetShow(false);

	CKTDGUIButton*		pButtonOK		= static_cast< CKTDGUIButton* >( m_pDLGItemEnroll->GetControl( L"Button_OK" ) );
	CKTDGUIButton*		pButtonCancle	= static_cast< CKTDGUIButton* >( m_pDLGItemEnroll->GetControl( L"Button_Cancle" ) );
	CKTDGUIButton*		pButtonPlus		= static_cast< CKTDGUIButton* >( m_pDLGItemEnroll->GetControl( L"Button_Plus" ) );
	CKTDGUIButton*		pButtonMinus	= static_cast< CKTDGUIButton* >( m_pDLGItemEnroll->GetControl( L"Button_Minus" ) );
	CKTDGUIIMEEditBox*	pQuantity		= static_cast< CKTDGUIIMEEditBox* >( m_pDLGItemEnroll->GetControl( L"IME_Edit_Quantity" ) );

	pQuantity->SetText( GET_REPLACED_STRING( ( STR_ID_2632, "i", m_iItemEnrollNum ) ), true);
	pButtonOK->SetCustomMsgMouseUp( UIM_REGISTER_QUANTITY_CONFIRM );
	pButtonCancle->SetCustomMsgMouseUp( UIM_REGISTER_QUANTITY_CANCEL );
	pButtonPlus->SetCustomMsgMouseUp( UIM_REGISTER_QUANTITY_PLUS );
	pButtonMinus->SetCustomMsgMouseUp( UIM_REGISTER_QUANTITY_MINUS );
	pQuantity->SetCustomMsgChange( UIM_REGISTER_QUANTITY_UPDATE );
	pQuantity->SetCustomMsgEnter( UIM_REGISTER_QUANTITY_CONFIRM );
	m_pDLGItemEnroll->SetCloseCustomUIEventID( UIM_REGISTER_QUANTITY_CANCEL );
	pQuantity->RequestFocus();

}

void CX2UIInventory::UpdateRegisterQuantityDLG( bool bReadIME )
{
	ASSERT( NULL != m_pDLGItemEnroll );
	if( NULL == m_pDLGItemEnroll )
		return;

	CKTDGUIIMEEditBox* pQuantity = static_cast< CKTDGUIIMEEditBox* >( m_pDLGItemEnroll->GetControl( L"IME_Edit_Quantity" ) );

	if(bReadIME)
	{
		m_iItemEnrollNum = g_pMain->GetEDFromString( pQuantity->GetText() );
	}

	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_EnrollItemUid );

	if ( m_iItemEnrollNum <= 0 )
	{
		if(bReadIME)				// �Է�â���� �޾ƿ� ���
			m_iItemEnrollNum = 0;
		else						// << �� ���� ��� : 0 �Ʒ��� max�� ��ȯ��Ų��.
			m_iItemEnrollNum = pItem->GetItemTemplet()->GetQuantity();
	}

	if ( m_iItemEnrollNum >= pItem->GetItemData().m_Quantity )
		m_iItemEnrollNum = pItem->GetItemData().m_Quantity;

	WCHAR buff[256] = {0};
	_itow_s( m_iItemEnrollNum, buff, 10 );

	wstring wstrNum = buff;
	pQuantity->SetText( wstrNum.c_str() );

}
//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������

bool CX2UIInventory::OnDropAnyItem( D3DXVECTOR2 mousePos )
{
	CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );
	if ( pItemSlot == NULL || pItemSlot->GetEnable() == false )
		return false;

	if ( pItemSlot == *m_pSlotBeforeDragging )
	{
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Item_Pick_And_Drop.ogg", false, false );
		SetNowDragItemReturn();
		g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGItemHighLight, true );
		return true;
	}

	//{{ 2008.11.13 ���¿� : UI ����ó��
	if(!(*m_pSlotBeforeDragging)->IsItem()) return false;
	//*m_DraggingItemUID = ((CX2SlotItem*)(*m_pSlotBeforeDragging))->GetItemUID();
	//}}

	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
	CX2Item* pItem = NULL;
	pItem = kInventory.GetItem( *m_DraggingItemUID, true );

	if ( pItem == NULL )
	{
		ASSERT( !"GetItem Is NULL" );
		return false;
	}

	// ���� ������ �з� Ÿ������ ����..
	if ( m_NowInventorySortType != kInventory.GetSortTypeByItemTemplet( pItem->GetItemTemplet() ) ) return false;

	switch((*m_pSlotBeforeDragging)->GetSlotType())
	{
	case CX2Slot::ST_EQUIPPED:
		//���â���� �Ѿ�� ���
		{
			switch ( g_pMain->GetNowStateID() )
			{
			case CX2Main::XS_DUNGEON_GAME:
			case CX2Main::XS_BATTLE_FIELD:
				{
					if ( g_pX2Game == NULL || g_pX2Game->GetMyUnit() == NULL )
						return false;

					if( g_pX2Game->GetMyUnit()->IsTransformed() )
						return false;

					if ( g_pX2Game->GetMyUnit()->GetNowHp() <= 0 )
					{
						g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2691 ),g_pMain->GetNowState() );
						return false;
					}

					// ���ӳ����� �ٻ� ������ ��� ���ü �Ұ���
					if ( NULL != g_pX2Game && g_pX2Game->CheckAndWarningBusyStateNow() )
						return false;

				} break;

			default:
				break;
			}

			//{{ kimhc // 2010.3.26 // ���� ��ų ���� ����
			if ( g_pMain->GetNowState() != NULL)
			{
				CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
				if ( pX2State->CheckIsReady() == true )
				{
					pX2State->ShowOkAndCancelMsgForUnReadyByEquip();
					SetNowDragItemReturn();
					return true;
				}
			}

			if ( g_pInstanceData->GetSendGameStartReqPacket() == true )
			{
				SetNowDragItemReturn();
				return true;
			}
			//}} kimhc // 2010.3.26 // ���� ��ų ���� ����

			// ����� �� ������ ���
			if ( pItemSlot->IsResetItemUI() == false )
			{
				Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( ((CX2SlotItem*)*m_pSlotBeforeDragging),
					pItemSlot );

				CX2SlotItem* pkSlotBeforeDragging = (CX2SlotItem*) *m_pSlotBeforeDragging;
				pkSlotBeforeDragging->DestroyItemUI();
				// ���â�� ��� ������ ����
				if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
				{
					g_pData->GetUIManager()->GetUICharInfo()->EquipChanged();
				}
				return true;
			}
			// ����� �̹� �ִ� ���, ����մ� ������ �˻�
			else
			{
				for ( int j = 0; j < (int)m_SlotList.size(); j++ )
				{
					CX2SlotItem* pItemSlotToPush = (CX2SlotItem*)GetSlot(j);
					if ( pItemSlotToPush->GetSlotType() == CX2Slot::ST_INVENTORY && pItemSlotToPush->GetDialog() == NULL && pItemSlotToPush->GetEnable())
					{
						Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( ((CX2SlotItem*)*m_pSlotBeforeDragging),
							pItemSlotToPush );

						CX2SlotItem* pkSlotBeforeDragging = (CX2SlotItem*) *m_pSlotBeforeDragging;
						pkSlotBeforeDragging->DestroyItemUI();
						// ���â�� ��� ��ȯ�Ǿ��ٴ� �޽����� ������
						if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
						{
							g_pData->GetUIManager()->GetUICharInfo()->EquipChanged();
						}
						return true;
					}
				}
			}
		}break;
		//{{ kimhc // 2009-08-08 // ���࿡�� �̵��Ǿ��� ���
	case CX2Slot::ST_BANK:
	case CX2Slot::ST_PET:
		//}} kimhc // 2009-08-08 // ���࿡�� �̵��Ǿ��� ���
	case CX2Slot::ST_INVENTORY:
		// �κ��丮���� ��ȯ�� ���
		{
			//{{kimch // 2010-12-18 // ���� �Ǵ� ������ ���� ���¿��� �� �κ� <-> ���� �κ� �ȵǵ��� ����ó�� �߰�
			if ( (*m_pSlotBeforeDragging)->GetSlotType() == CX2Slot::ST_PET )
			{
				CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
				if ( pX2State->CheckIsReady() == true )
				{
					pX2State->ShowOkAndCancelMsgForUnReady( STR_ID_9874 );
					SetNowDragItemReturn();
					return true;
				}
			} // if
			//}}kimch // 2010-12-18 // ���� �Ǵ� ������ ���� ���¿��� �� �κ� <-> ���� �κ� �ȵǵ��� ����ó�� �߰�

			// oasis907 : ����� [2010.9.8] // ���� ��ȭ ��� ������ ���� ó��, �� �κ����� �κ����� �̵� ���ϰ�
			if( GetUISocketItem() != NULL &&
				GetUISocketItem()->GetShow() == true &&
				GetUISocketItem()->CheckSocketUseItem( static_cast< CX2SlotItem* >( (*m_pSlotBeforeDragging) )->GetItemUID() ) )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5112 ), g_pMain->GetNowState() );
				SetNowDragItemReturn();
				return true;
			}

	//{{ 2011.03.16   �ӱԼ� �ƹ�Ÿ ���� �ý���
	//}}
	//{{ 2011.05.04   �ӱԼ� �ƹ�Ÿ �ռ� �ý���
	//}}
			if( Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( ((CX2SlotItem*)*m_pSlotBeforeDragging), pItemSlot ) == true )
			{
				CX2SlotItem* pkSlotBeforeDragging = (CX2SlotItem*) *m_pSlotBeforeDragging;
				pkSlotBeforeDragging->DestroyItemUI();

				return true;
			}
			else
			{
				SetNowDragItemReturn();
				return true;
			}
		}
	default:
		break;
	}

	// ó���� �� �� ���
	return false;

}



bool CX2UIInventory::OnRClickedItem( D3DXVECTOR2 mousePos )
{
	// ���콺���� ��ġ�� �κ� ���� ���
	CX2SlotItem* pItemSlot = (CX2SlotItem*) GetSlotInMousePos( mousePos );
	if ( pItemSlot == NULL )
		return false;

	if ( pItemSlot->GetSlotType() != CX2SlotManager::CX2Slot::ST_INVENTORY || pItemSlot->IsResetItemUI() == false )
		return false;

	const CX2Item::ItemTemplet* pkItemTemplet = g_pData->GetItemManager()->GetItemTemplet( pItemSlot->GetItemTID() );
	if ( pkItemTemplet == NULL )
		return false;

	CX2Unit* pUnit = g_pData->GetMyUser()->GetSelectUnit();
	if(pUnit == NULL)
		return false;

	const CX2Unit::UnitData* pUnitData = &pUnit->GetUnitData();

	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( pItemSlot->GetItemUID() );
	if( NULL == pItem )
		return false;

	//{{ 2011.03.16   �ӱԼ� �ƹ�Ÿ ���� �ý���
	//}}
	//{{ 2011.05.04   �ӱԼ� �ƹ�Ÿ �ռ� �ý���
	//}}
	//{{ kimhc // 2009-08-05 // ����
	// ������ ���� �ִ� ��� ������ ��� �ȵǰ� �������� �̵�
	if ( g_pData->GetUIManager() != NULL &&
		g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_PRIVATE_BANK ) == true )	// ������ �����ִ� ���
	{
		// 2������ ����� �������� �̵� �Ұ�
		if ( pItem->GetItemTemplet()->GetItemID() == UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID )
		{
			// �����̷� �̵� �Ұ�
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_29779 ), g_pMain->GetNowState() ); //18422
			return false;
		}
		//{{ kimhc // 2010-01-05 // PC�� �����̾� ����
		if ( pkItemTemplet->GetIsPcBang() == true )
		{
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
			return false;
		}
		//}} kimhc // 2010-01-05 // PC�� �����̾� ����

		// ���࿡ �������� ���� �ִ� ó��
		const CX2Inventory& kInventory	=	g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		CX2Item*		pEmptySlot	=	NULL;
		int				iItemMaxNum = kInventory.GetItemMaxNum( CX2Inventory::ST_BANK );
		CX2Inventory::SORT_TYPE	eType = CX2Inventory::ST_BANK;

		if( g_pData->GetUIManager()->GetUIPrivateBank()->GetIsShareBank() == true )
		{
			iItemMaxNum = g_pData->GetUIManager()->GetUIPrivateBank()->GetShareBankSize();
			eType = CX2Inventory::ST_SHARE_BANK;
		}

		// SERV_REFORM_INVENTORY_TEST
		for ( int i = 0; i < iItemMaxNum; i++ )
		{
			pEmptySlot		=	kInventory.GetItem( eType, i );

			if ( pEmptySlot == NULL )
			{
				CX2SlotItem* pToSlotItem	= NULL;
				pToSlotItem					= static_cast< CX2SlotItem* >( g_pData->GetUIManager()->GetUIPrivateBank()->GetSlot( i ) );

				if ( pToSlotItem == NULL )
				{
					ASSERT( !"UIPrivateBank's Slot Is NULL" );
					return false;
				}

				if( Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( pItemSlot, pToSlotItem ) == true )
				{
					pItemSlot->DestroyItemUI();
					InvalidSlotDesc();
				}
				else
				{
					SetNowDragItemReturn();
				}
				return true;
			}
			pEmptySlot	= NULL;
		}

		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_3859 ), g_pMain->GetNowState() );
		return false;
	}

	// ������ Ȯ�� �����ִ� ����Ʈ ������ ����
	if ( pItemSlot->GetItemTID() == _CONST_UI_PRIVATE_BANK_::g_iBankQuestItemID )
	{
		const CX2Inventory& kInventory	=	g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

		int iNumOfBankSlot	= kInventory.GetItemMaxNum( CX2Inventory::ST_BANK );

		if ( iNumOfBankSlot	== CX2Inventory::MPB_PLATINUM )		// �̹� �÷�Ƽ�� ��� �̸�
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_3871 ), g_pMain->GetNowState() );
		}
	}


	//}} kimhc // 2009-08-05 // ����


	//{{ kimhc // �ǽð� ���ҵ� �� ���������� ������ �׾��ų�, wait, mpCharge ���°� �ƴҶ� ��� ��ü ����
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_DUNGEON_GAME:
	case CX2Main::XS_BATTLE_FIELD:
		{
			if ( g_pX2Game == NULL || g_pX2Game->GetMyUnit() == NULL )
				return false;

			if( g_pX2Game->GetMyUnit()->IsTransformed() )
				return false;

			if ( g_pX2Game->GetMyUnit()->GetNowHp() <= 0 )
			{
				g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2691 ),g_pMain->GetNowState() );

				return false;
			}

			// ���ӳ����� �ٻ� ������ ��� ���ü �Ұ���
			if ( NULL != g_pX2Game && g_pX2Game->CheckAndWarningBusyStateNow() )
				return false;

		} break;

	default:
		break;
	}
	//}} kimhc // �ǽð� ���ҵ� �� ���������� ������ �׾��ų�, wait, mpCharge ���°� �ƴҶ� ��� ��ü ����

	// �Ǹ����� ���λ����� ���������� ���´�
	if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
		g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_SELL )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_2650 ), g_pMain->GetNowState() );

		return true;
	}

	if( GetUISocketItem() != NULL && GetUISocketItem()->GetShow() == true)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_5112 ), g_pMain->GetNowState() );

		return true;
	}


	// ���Ҵ� �������̶��
	if( pItem != NULL && true == pItem->IsDisabled() )
	{
		// 1. ���� �������� �ִ��� Ȯ���Ѵ�.

		vector<int> vecRecoveryItemIdList;
		// 1-1. ���� �� �������̶�� �� ���� �������� ������ ���� �˻��Ѵ�.
		if(
            pItem->GetItemTemplet() != NULL &&
            pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_DEFENCE)
		{
			GetRequiredArmorOnlyRecoveryStoneID( pkItemTemplet->GetUseLevel(), vecRecoveryItemIdList );
			CheckRequiredRecoveryStoneInInventory( vecRecoveryItemIdList );
		}

		// 1-2. �� ���� �������� �������� �׳� �������� ������ �˻�.
		if ( vecRecoveryItemIdList.empty() )
		{
			GetRequiredRecoveryStoneID( pkItemTemplet->GetUseLevel(), vecRecoveryItemIdList );
			CheckRequiredRecoveryStoneInInventory( vecRecoveryItemIdList );
		}

		// �ش� �������� �־�����
		if( false == vecRecoveryItemIdList.empty() )
		{
            m_RecoveryItemUid = pItem->GetUID();
			const CX2Item::ItemTemplet* pkRecoverItemTemlpet = g_pData->GetItemManager()->GetItemTemplet( vecRecoveryItemIdList.back() );
			if(pkRecoverItemTemlpet != NULL)
			{
				g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_REPLACED_STRING( ( STR_ID_799, "S", pkRecoverItemTemlpet->GetName() ) ), UIM_ITEM_RECOVERY_OK, g_pMain->GetNowState() );
			}
		}
		else
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_800 ), g_pMain->GetNowState() );
		}
		return true;
	}

	// ��������â�� ���������� �� �������� ��������â�� ����Ѵ�
	if(g_pMain != NULL && g_pMain->GetPostBox() != NULL && g_pMain->GetPostBox()->GetOpendSendWindow() == true)
	{
		g_pMain->GetPostBox()->RegisterItem(pItemSlot);
		return true;
	}
	// ������� ���λ����� ���������� �� �������� ���λ����� ����ϴ� â�� ������ ����
	if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
		( g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_WAIT ||
		g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_AGENCY_WAIT  ) )
	{
		g_pData->GetUIManager()->GetUIPersonalShop()->EnrollItemByUid( pItemSlot->GetItemUID() );
		return true;
	}
	// ���ΰŷ�â�� ���������� (��)
	if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE))
	{
		g_pData->GetUIManager()->GetUIPersonalTrade()->OnRegisterMyItem( pItemSlot->GetItemUID() );
		return true;
	}
	// 	// �����̳� ����â�� ���������� �ƹ��͵� ���Ѵ�
	// 	if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP) || true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_MANUFACTURE) )
	// 	{
	// 		return true;
	// 	}


	if( pItem != NULL && false == pItem->GetIsEvaluation() &&
		NULL != pItem->GetItemTemplet() )
	{
		int iConsumeED = static_cast<int>(pItem->GetItemTemplet()->GetPrice() * 0.05f);
		m_iTargetItemUID = pItem->GetUID();
		g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
			GET_REPLACED_STRING( ( STR_ID_24611, "i",  iConsumeED) ),
			UIM_USE_EVALUATE_ITEM_OK, g_pMain->GetNowState(), UIM_USE_EVALUATE_ITEM_CANCEL );
		return true;
	}

	//{{ kimhc // 2009-09-03 // ���ε� �������� ��Ŭ�� ���� �� ó��
	if ( pItem != NULL && pItem->GetItemData().m_bIsSealed == true )
	{
		g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300),
            GET_REPLACED_STRING( ( STR_ID_4472, "S", pkItemTemplet->GetFullName_() ) ),
			UIM_UNSEAL_ITEM_CONFIRM_OK, g_pMain->GetNowState(), UIM_UNSEAL_ITEM_CONFIRM_CANCEL );

		m_itemUIDToBeUnSealed = pItem->GetUID();
		return true;
	}
	//}} kimhc // 2009-09-03 // ���ε� �������� ��Ŭ�� ���� �� ó��

	// �κ��丮 ���� Ŭ�� : ��� ��ȯ
	// �ش��ϴ� ������ �˻�, ���â���� �̵�
	for ( unsigned int iS = 0; iS < m_SlotList.size(); ++iS )
	{
		if(NULL == g_pData->GetUIManager()->GetUICharInfo())
		{
			ASSERT( !"Charinfo NULL" );		// �׻� ����־�� �ϴ� class
			g_pData->GetUIManager()->CreateUICharInfo();
		}

		CX2SlotItem* pItemSlotEquip = (CX2SlotItem*) g_pData->GetUIManager()->GetUICharInfo()->GetSlot(iS);
		if( NULL == pItemSlotEquip )
			continue;

		//		if ( pItemSlotEquip->GetSlotType() != CX2SlotManager::CX2Slot::ST_EQUIPPED )
		//			continue;

		switch( pkItemTemplet->GetItemType() )
		{
		case CX2Item::IT_WEAPON:
		case CX2Item::IT_DEFENCE:
		case CX2Item::IT_ACCESSORY:
			{
				//{{ kimhc // 2010.3.26 // ���� ��ų ���� ����
				if ( g_pMain->GetNowState() != NULL)
				{
					CX2State* pX2State = static_cast<CX2State*>( g_pMain->GetNowState() );
					if ( pX2State->CheckIsReady() == true )
					{
						pX2State->ShowOkAndCancelMsgForUnReadyByEquip();
						return true;
					}
				}

				if ( g_pInstanceData->GetSendGameStartReqPacket() == true )
					return true;
				//}} kimhc // 2010.3.26 // ���� ��ų ���� ����


				//{{ ����� : [2009/7/29] //	���Ұ� ��� ���� �Ұ����ϰ� �ڵ� �Է�
				if(	pkItemTemplet->GetNoEquip() == true )
				{
					continue;
				}
				//}} ����� : [2009/7/29] //	���Ұ� ��� ���� �Ұ����ϰ� �ڵ� �Է�

				if( false == CX2Unit::CanEquipAsParts(
                    pkItemTemplet->GetItemID(),
                    pUnit, pUnitData->m_Level ) )
					continue;

				//��� ������ ������ �� ������� �Žñ� �غ���
				//�巡���ؼ� �Žñ� �ϴºκе� �ذ��غ���

				if ( pItem != NULL )
				{
					if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_ENDURANCE )
					{
						if ( pItem->GetItemData().m_Endurance <= 0 )
						{
							continue;
						}
					}
				}

				//{{ 09.06.02 �¿� : �Ǽ��縮 ���� ��쿡�� fashion ���� üũ�� ���� �ʵ��� ����ó�� :
				// �Ǽ��縮�̸鼭 �м����� �������� �������� �ʴ� �� ������
 				if ( pItemSlotEquip->GetEquipPos() == pkItemTemplet->GetEqipPosition() &&
 					(pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_ACCESSORY || pItemSlotEquip->GetFashion() == pkItemTemplet->GetFashion()) )
				{
					if(false == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
					{
						g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_MENU_CHARINFO, true);
					}

					Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( pItemSlot, pItemSlotEquip );
					pItemSlot->DestroyItemUI();
					InvalidSlotDesc();
					return true;
				}
			} break;
		default:
			break;
		}
	}



	// ������ ���
	if( pItemSlot->GetSortType() == CX2Inventory::ST_QUICK_SLOT )	// �����Կ� ���������� �������̸�
	{
		if(g_pData->GetUIManager()->GetUIQuickSlot() == NULL)
		{
			ASSERT( !"Quickslot NULL" );	// �������� �׻� ����־�� �ϴϱ�..
			g_pData->GetUIManager()->CreateUIQuickSlot();
		}

		CX2SlotItem* pQuickSlot = g_pData->GetUIManager()->GetUIQuickSlot()->GetEmptyslot();
		if( pQuickSlot == NULL )
			return false;

		Handler_EGS_CHANGE_INVENTORY_SLOT_REQ( pItemSlot, pQuickSlot );
		return true;
	}


	for(int i=0; i<std::ssize(PET_ITEM_ID); ++i)
	{
		if(
            pItem->GetItemTemplet()->GetItemID() == PET_ITEM_ID[i]
        )
		{
			if( NULL != g_pMain && CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() && g_pX2Game != NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_6550 ), g_pMain->GetNowState() );
			}
			else if( g_pSquareGame != NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_6551 ), g_pMain->GetNowState() );
			}
			else if( g_pData != NULL && g_pData->GetPetManager() != NULL )
			{
				// �� �̸�����
				g_pData->GetPetManager()->ShowMakePetName(true, pItem->GetUID());
				return true;
			}
		}
	}

	for( int i = 0; i < std::ssize(RIDING_PET_ITEM_ID); ++i )
	{
		if( pItem->GetItemTemplet()->GetItemID() == RIDING_PET_ITEM_ID[i] )
		{
			if( NULL != g_pMain && CX2Main::XS_BATTLE_FIELD != g_pMain->GetNowStateID() && g_pX2Game != NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_24536 ), g_pMain->GetNowState() );
			}
			else if( g_pSquareGame != NULL )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_24537 ), g_pMain->GetNowState() );
			}
			else if( NULL != CX2RidingPetManager::GetInstance() )
			{
				m_iRidingItemUid = pItem->GetUID();
				wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_25115, "L", pItem->GetFullName()) );
				g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), wstrMsg.c_str(), UIM_RIDING_ITEM_USE_OK, g_pMain->GetNowState() );
				return true;
			}
		}
	}


	if(pkItemTemplet->GetItemID() == RIDINGPET_STAMINA_30_PER_ITEM_ID)
	{
		int iPecValue = 0;
		switch(pkItemTemplet->GetItemID())
		{
		case RIDINGPET_STAMINA_30_PER_ITEM_ID:
			iPecValue = 30;
			break;
		}

		if(CX2RidingPetManager::GetInstance()->SetRidingPetStaminaPercentUP(iPecValue))
		{
			Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( pItemSlot->GetItemUID() );
			return true;
		}
		else
		{
			// ���� �޼��� ó�� ���.
			return false;
		}
	}

	// ** ��밡�� ������ �� ������ ��Ŭ�� ó��
	// �κ��� ��밡�� �����ۿ� ���� ó��
	if( pItem->GetItemTemplet()->GetCanUseInventory() == true )
	{
		//m_iItemUID
		if( CheckUseItem(pItem) == true )
		{
			//{{ kimhc // �ǽð� ���ҵ� �� ���������� ������ ���� ���ƾ� �� �͵�
			if( pItem != NULL && pItem->GetItemTemplet()->GetItemType() == CX2Item::IT_SKILL_MEMO )
			{
				if( g_pData == NULL ||
					g_pData->GetMyUser() == NULL ||
					g_pData->GetMyUser()->GetSelectUnit() == NULL ||
					g_pData->GetMyUser()->GetSelectUnit()->GetUnitTemplet() == NULL ||
					pItem->GetItemTemplet() == NULL
                    )
					return true;

				if( GetX2UnitClassCompatibility( g_pData->GetMyUser()->GetSelectUnit()->GetClass(), pItem->GetItemTemplet()->GetUnitClass() ) == false )
				{
					// �޸� ��� ������ ���� ����
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_5013), g_pMain->GetNowState() );
					return true;
				}
				if( pItem->GetItemTemplet()->GetUseLevel() > g_pData->GetSelectUnitLevel() )
				{
					// �޸� ��뷹���� ���� ����
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_5014), g_pMain->GetNowState() );
					return true;
				}

				if( g_pData->GetMyUser()->GetSelectUnit()->GetMaxSkillNoteSlot() <= 0)
				{
					// ����� ��Ʈ ��� �Ұ�����
					g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4988 ),g_pMain->GetNowState() );
					return true;
				}

				if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME || g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					// ������ �޸��� �Ұ�
					g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4989 ),g_pMain->GetNowState() );
					return true;
				}

				if( g_pData->GetUIManager() != NULL )
				{
					if( g_pData->GetUIManager()->GetUISkillNote() != NULL &&
						g_pData->GetUIManager()->GetUISkillNote()->GetUpdateMemo() == true )
					{
						g_pData->GetUIManager()->GetUISkillNote()->UpdateMemoSlot( (int)g_pData->GetUIManager()->GetUISkillNote()->GetUpdatePage() );
						g_pData->GetUIManager()->GetUISkillNote()->ResetSkillNote();

						return true;
					}

					// �޸���
					g_pData->GetUIManager()->ToggleUI(CX2UIManager::UI_SKILL_NOTE, true);
					if( g_pData->GetUIManager()->GetUISkillNote() != NULL )
					{
						CX2State* pState = (CX2State*)g_pMain->GetNowState();
						if(pState->GetCursor() != NULL)
							pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_REGISTER_MEMO );

						g_pData->GetUIManager()->GetUISkillNote()->RegisterMemo( pItem->GetUID() );
					}

					return true;
				}


				// �޸��� �Ұ�
				return true;
			}

			if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
			{
                switch ( pItem->GetItemTemplet()->GetItemID() )
				{
				case WARP_ITEM_RUBEN_ITEM_ID:					// �纥 ���� ��ȯ��
				case WARP_ITEM_ELDER_ITEM_ID:					// ���� ���� ��ȯ��
				case WARP_ITEM_BESMA_ITEM_ID:					// ������ ���� ��ȯ��
				case WARP_ITEM_ALTERA_ITEM_ID:					// ���׶� ���� ��ȯ��
				case WARP_ITEM_PEITA_DUNGEON_GATE_ITEM_ID:		// ����Ÿ ���� ����Ʈ �̵� Ƽ��
				case WARP_ITEM_VELDER_ITEM_ID:					// ���� ���� ��ȯ��
				case NICKNAME_CHANGE_CARD_ITEM_ID:				// ĳ���� �г��� ���� ī�� ID
				case INIT_SKILL_TREE_ITEM:					// ��ų �ʱ�ȭ �޴�
				case INIT_SKILL_TREE_EVENT_ITEM_ID:				// ������ �帵ũ
				case CI_SKILL_INIT_EVENT_ITEM2:				// ������ �帵ũ 1�ϱ�, ����Ʈ �����
				case CI_SKILL_INIT_EVENT_ITEM3:				// ������ �帵ũ 3�ϱ�, ����Ʈ �����
				case CI_SKILL_INIT_EVENT_ITEM4:				// ������ �帵ũ �뷱�� ����
				case INIT_SKILL_TREE_EVENT_ITEM_ID_JP:			// ������ �帵ũ(����)
				case RURIEL_RESET_SKILL_ITEM:					// �縮���� ��ų �ʱ�ȭ �޴�
				case WARP_ITEM_FREE_ITEM_ID:					// �̵�Ƽ�� ���� �̿��

	//{{ kimhc // 2009-08-26 // ������ ���� ���� ������ ��� ����
				case ITEM_FOR_SEAL_NORMAL:								// ������ �� ����(�Ͼ��)
				case ITEM_FOR_SEAL_RARE:								// ������ �� ����(�����)
				case ITEM_FOR_SEAL_ELITE:								// ������ �� ����(�����)
				case ITEM_FOR_SEAL_UNIQUE:								// ������ �� ����(Ȳ�ݻ�)

				case ITEM_FOR_SEAL_NORMAL_EVENT:						// ������ �� ���� �̺�Ʈ(�Ͼ��)
				case ITEM_FOR_SEAL_ELITE_EVENT:							// ������ �� ���� �̺�Ʈ(�����)
				case ITEM_FOR_SEAL_NORMAL_EVENT_RURIEL:
				case ITEM_FOR_SEAL_RARE_EVENT_RURIEL:
				case ITEM_FOR_SEAL_ELITE_EVENT_RURIEL:
				case ITEM_FOR_SEAL_UNIQUE_EVENT_RURIEL:
				case ITEM_FOR_SEAL_NORMAL_EVENT_ARIEL:
				case ITEM_FOR_SEAL_RARE_EVENT_ARIEL:
				case ITEM_FOR_SEAL_ELITE_EVENT_ARIEL:
				case ITEM_FOR_SEAL_UNIQUE_EVENT_ARIEL:
	//}} kimhc // 2009-08-26 // ������ �� ���� ���� ������ ��� ����
				case SKILL_PLUS_ITEM_ID:			// oasis907 : ����� [2010.12.9] // �׳�ý� �ູ
				case SKILL_PLUS_ITEM_ID_JP:
				case SKILL_POINT_10_30DAY_USE_INVEN_JP:
				case SKILL_POINT_10_15DAY_USE_INVEN_JP:
				case SKILL_POINT_30_7DAY_USE_INVEN:
				case SKILL_POINT_60_7DAY_USE_INVEN:
				case SKILL_POINT_60_15DAY_USE_INVEN:
				case SKILL_POINT_60_30DAY_USE_INVEN:
				case SKILL_POINT_60_7DAY_USE_INVEN_2:
				case SKILL_POINT_30_7DAY_USE_INVEN_2:
				case SKILL_POINT_130_1DAY_USE_INVEN:
				case SKILL_POINT_30_1DAY_USE_INVEN:
				case SKILL_POINT_60_1DAY_USE_INVEN:
					//{{ 2011. 11. 30	������	��Ű�� ��ǰ �߰�
				case SKILL_POINT_5_USE_INVEN_ITEM_ID:
				case SKILL_POINT_10_USE_INVEN_ITEM_ID:


//				case WEDDING_RING_ITEM_ID : �Ŀ� �߰����� ��!
//				case INVITE_WEDDING_LETTER_ITEM_ID :
					//}}
				case EVENT_SKILL_POINT_5_USE_INVEN_15_DAY:
				case SKILL_POINT_30_14DAY_USE_INVEN:
					{
						g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2646 ),g_pMain->GetNowState() );
						return true;
					}
				default:
					break;
				}
			}
			//}} kimhc // �ǽð� ���ҵ� �� ���������� ������ ���� ���ƾ� �� �͵�

            switch ( pItem->GetItemTemplet()->GetItemID() )
			{
			case WARP_ITEM_RUBEN_ITEM_ID:					// �纥 ���� ��ȯ��
			case WARP_ITEM_ELDER_ITEM_ID:					// ���� ���� ��ȯ��
			case WARP_ITEM_BESMA_ITEM_ID:					// ������ ���� ��ȯ��
			case WARP_ITEM_ALTERA_ITEM_ID:					// ���׶� ���� ��ȯ��
			case WARP_ITEM_PEITA_DUNGEON_GATE_ITEM_ID:		// ����Ÿ ���� ����Ʈ �̵� Ƽ��
			case WARP_ITEM_VELDER_ITEM_ID:					// ���� ���� ��ȯ��
				{
					switch ( g_pData->GetLocationManager()->GetCurrentVillageID() )
					{
					case SEnum::VMI_HAMEL:
					case SEnum::VMI_SANDER:

						{
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_11176 ),g_pMain->GetNowState() );
							return true;
						} break;

					default:
						break;
					} // if
				} break;

			default:
				break;
			}

			// Īȣ��� ������
			for(int i=0; i<std::ssize(TITLE_ITEM); ++i)
			{
                if( pItem->GetItemTemplet()->GetItemID() == TITLE_ITEM[i] )
				{
					wstring wstrMsg = GET_REPLACED_STRING( ( STR_ID_3712, "L", pItem->GetFullName()) );
					m_TitleItemUID = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), UIM_ITEM_USE_TITLE, g_pMain->GetNowState() );
					return true;
				}
			}


			if( true == g_pData->GetSkillTree()->IsUnsealSkillItemID( pItem->GetItemTemplet()->GetItemID() ) )
			{
				const CX2SkillTree::SKILL_ID iSkillID = g_pData->GetSkillTree()->GetUnsealSkillItemInfo( pItem->GetItemTemplet()->GetItemID() );

				if ( CX2SkillTree::SI_NONE == iSkillID )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_3856 ), g_pMain->GetNowState() );
					return false;
				}

				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( iSkillID );

				ASSERT( NULL != pSkillTemplet );
				if( NULL == pSkillTemplet )
					return false;

				wstring wstrMsg = L"";
				if( true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.IsSkillUnsealed( (CX2SkillTree::SKILL_ID) iSkillID ) )
				{
					wstrMsg = GET_REPLACED_STRING( (STR_ID_3855, "L", pSkillTemplet->m_wstrName) );
				}
				else
				{
					wstrMsg = GET_REPLACED_STRING( (STR_ID_3854, "L", pSkillTemplet->m_wstrName) );
				}
				g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), UIM_ITEM_USE_SKILL_UNSEAL, g_pMain->GetNowState() );		// �������� ����ϸ� *** ��ų�� �������� �մϴ�


				m_UsedItemUID = pItem->GetUID();
				return true;
			}


			// �����߰��� ������ȯ�� �߰�
            switch(pItem->GetItemTemplet()->GetItemID())
			{
			case WARP_ITEM_RUBEN_ITEM_ID: // �纥���� ��ȯ��
				{
					m_WarpItem = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_801 ), UIM_ITEM_USE_WARP, g_pMain->GetNowState() );
					return true;
				} break;
			case WARP_ITEM_ELDER_ITEM_ID: // �������� ��ȯ��
				{
					m_WarpItem = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_802 ), UIM_ITEM_USE_WARP, g_pMain->GetNowState() );
					return true;
				} break;
			case WARP_ITEM_BESMA_ITEM_ID: // ���������� ��ȯ��
				{
					m_WarpItem = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_803 ), UIM_ITEM_USE_WARP, g_pMain->GetNowState() );
					return true;
				} break;
			case WARP_ITEM_ALTERA_ITEM_ID: // ���׶��� ��ȯ��
				{
					m_WarpItem = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_804 ), UIM_ITEM_USE_WARP, g_pMain->GetNowState() );
					return true;
				} break;
			case WARP_ITEM_PEITA_DUNGEON_GATE_ITEM_ID: // ����Ÿ ��������Ʈ �̵�Ƽ��
				{
					m_WarpItem = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2628 ), UIM_ITEM_USE_WARP, g_pMain->GetNowState() );
					return true;
				}
				break;
			case WARP_ITEM_VELDER_ITEM_ID: // �������� ��ȯ��
				{
					m_WarpItem = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4649 ), UIM_ITEM_USE_WARP, g_pMain->GetNowState() );
					return true;
				} break;
			case NICKNAME_CHANGE_CARD_ITEM_ID: // ĳ���� �г��� ���� ī�� ID
				{
					m_UseItemTempUID = pItem->GetUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_805 ), UIM_CHANGE_NICK_NAME_OK, g_pMain->GetNowState() );
					return true;
				} break;

			case INIT_SKILL_TREE_ITEM:			// ��ų �ʱ�ȭ �޴�
			case INIT_SKILL_TREE_EVENT_ITEM_ID:		// ������ �帵ũ
			case CI_SKILL_INIT_EVENT_ITEM2: // ������ �帵ũ 1�ϱ�, ����Ʈ �����
			case CI_SKILL_INIT_EVENT_ITEM3: // ������ �帵ũ 3�ϱ�, ����Ʈ �����
			case CI_SKILL_INIT_EVENT_ITEM4: // ������ �帵ũ �뷱�� ����
			case INIT_SKILL_TREE_EVENT_ITEM_ID_JP:			// ������ �帵ũ(����)
			case RURIEL_RESET_SKILL_ITEM:	// �縮���� ��ų �ʱ�ȭ �޴�
				{
					if( true == pUnitData->m_UserSkillTree.CanIInitSkillTree() )
					{
						m_UsedItemUID = pItem->GetUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_478 ), UIM_SKILL_INIT_OK, g_pMain->GetNowState() );
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_2679 ), g_pMain->GetNowState() );
					}
					return true;
				} break;

			case COUPLE_NICKNAME_CHANGE_ITEM_ID :
			{
				if ( NULL != g_pData->GetRelationshipManager() &&
					NULL != g_pData->GetRelationshipManager()->GetUIRelationship() )
				{
					if ( g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType
						> SEnum::RT_COUPLE )
					{
						g_pData->GetRelationshipManager()->GetUIRelationship()->ShowChangeLoveMessageDlg( true, pItem->GetUID() );
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_24620 ), g_pMain->GetNowState() );
					}
				}
				return true;
			}
			break;

			case WEDDING_RING_ITEM_ID :
			{
				if ( NULL != g_pData->GetRelationshipManager() &&
					 NULL != g_pData->GetRelationshipManager()->GetUIRelationship() &&
					 NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() )
				{

					if ( g_pData->GetRelationshipManager()->GetMyRelationshipInfo()->m_eRelationStateType ==
						SEnum::RT_COUPLE )
					{
						// Ŀ�� ��� �ð� üũ
						if ( true == g_pData->GetRelationshipManager()->CheckWeddingQualification() )
						{
							// ��ȥ ���� ���� ���� üũ
							if ( NULL != g_pData->GetMyUser() &&
								NULL != g_pData->GetMyUser()->GetSelectUnit() &&
								NULL != g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID ( WEDDING_RING_ITEM_ID) )
							{
								const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

								// �ʴ�� ���� ���� üũ
								int iWeddingLetterMany = 0;
								CX2Item * pInviteLetterItem = kInventory.GetItemByTID ( INVITE_WEDDING_ITEM_ID);
								if( NULL != pInviteLetterItem )
								{
									iWeddingLetterMany = pInviteLetterItem->GetItemData().m_Quantity;
								}

								// �����߿� ��������� ������ ����
								g_pData->GetRelationshipManager()->GetUIRelationship()->SetSelecteWeddingHallByPropose( iWeddingLetterMany );
							}
							else
							{// ��ȥ ���� ���� ��
								const CX2Item::ItemTemplet * pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( WEDDING_RING_ITEM_ID );
								if ( NULL != pItemTemplet )
								{
									wstring wstrTextNotice= GET_REPLACED_STRING( ( STR_ID_24451, "S", pItemTemplet->GetName() ) );
									g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), wstrTextNotice.c_str(), g_pMain->GetNowState() );
								}
							}
						}
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(250,300), GET_STRING( STR_ID_28122 ),g_pMain->GetNowState() );
					}
					return true;
				}
			}
			break;

			case COUPLE_RING_ITEM_ID:
			{
				if ( NULL != g_pData->GetRelationshipManager() &&
					 NULL != g_pData->GetRelationshipManager()->GetUIRelationship() )
				{
					g_pData->GetRelationshipManager()->GetUIRelationship()->ShowInviteCoupleDlg( true );
				}
				return true;
			}
			break;

			case RESERVED_WEDDING_LETTER_ITEM_ID :
				{
					if (NULL != g_pData->GetRelationshipManager() &&
						NULL != g_pData->GetRelationshipManager()->GetUIRelationship() )
					{
						g_pData->GetRelationshipManager()->GetUIRelationship()->ShowJoinWeddingDlg ( true, true, pItem->GetUID () );
					}
					return true;
				} break;
			case INVITE_WEDDING_LETTER_ITEM_ID :
				{
					if (NULL != g_pData->GetRelationshipManager() &&
						NULL != g_pData->GetRelationshipManager()->GetUIRelationship()
						)
					{
						g_pData->GetRelationshipManager()->GetUIRelationship()->ShowJoinWeddingDlg ( true, false, pItem->GetUID () );
					}
					return true;
				}
				break;

				//{{ ����� : [2009/7/14] //	���ҵ� �ް��� ����
			case NASOD_SCOPE_ITEM_ID:
			case NASOD_SCOPE_HIGH_ITEM_ID:
				{
					g_pChatBox->SetItemSlot( pItemSlot );

					g_pChatBox->ShowNasodMessageDlg( true );
					return true;
				} break;
				//}} ����� : [2009/7/14] //	���ҵ� �ް��� ����
			case CHANGE_PET_NAME_ITEM_ID:
				{
					if ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
					{
						if( g_pData != NULL && g_pData->GetPetManager() != NULL)
						{
							if(g_pData->GetPetManager()->GetMyPet() == NULL)
							{
								//��ȯ�� ���� �����ϴ�.
								g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), NetError::GetErrStrF(NetError::ERR_PET_06) ,g_pMain->GetNowState() );
							}
							else
							{
								// �� �̸�����
								g_pData->GetPetManager()->ShowChangePetName(true, pItem->GetUID());
							}
							return true;
						}
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4495 ),g_pMain->GetNowState() );
						return true;
					}
				}
				break;

			case ACTIVATION_DROP_ITEM_PICKUP_SKILL:
				{
					if( NULL != g_pData && NULL != g_pData->GetPetManager() )
					{
						//�κ��丮���� ��� �� ���� ������ ��ȯ�� �꿡�� ���� ��Ű�� ���� getmypet�̿�
						CX2PET* pMyPet = g_pData->GetPetManager()->GetMyPet();
						if( NULL != pMyPet )
						{
							CX2PET::PetInfo& pMyPetInfo = pMyPet->GetPetInfo();
							//������ ��� �Ұ� ����ó��
							//1. ���� ��ȭ�ܰ谡 ��ü �̻� �� ���� ��� ����
							//2. ������ �ݱ� ��ų�� ���εǾ� �ִ� ���� �� ���� ��� ����
							CX2PetManager::PetTemplet* pPetTemplet =
								g_pData->GetPetManager()->GetPetTemplet( static_cast<CX2PetManager::PET_UNIT_ID>(pMyPetInfo.m_PetId) );
							UINT uiEvolutionStep = static_cast<UINT>(pMyPetInfo.m_Evolution_Step);//�� ��ȭ �ܰ�
							if( NULL != pPetTemplet &&
								uiEvolutionStep < pPetTemplet->m_vecPetStatus.size() ) //������ ũ�Ⱑ �ε������� Ŭ ���� ����
							{
								int iPetStatus = pPetTemplet->m_vecPetStatus[ uiEvolutionStep ];
								if( 0 == iPetStatus )  //�� �ܰ� �� ��
								{
									//������ �ݱ� ��ų�� ���� ��ȭ �ܰ谡 ��ü �̻� �� ���� ��� ���� �մϴ�.
									g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_21657 ) ,g_pMain->GetNowState() );
									return true;
								}
								else //��ü �̻��̶�� �̹� ���� ���� �Ǿ� �ִ��� Ȯ��
								{
									if( true == pMyPetInfo.m_bIsDropItemPickup )
									{
										//������ �ݱ� ��ų�� �̹� Ȱ��ȭ �Ǿ� �ֱ� ������ ��� �� �� �����ϴ�.
										g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_21658 ) ,g_pMain->GetNowState() );
										return true;
									}
								}
							}
							//������ ��� ������ ���¶��
							m_UsedItemUID = pItem->GetUID();
							m_UseItemTempUID = pMyPetInfo.m_PetUid;
							//���� ��ȯ�� '@1'���� ������ �ݱ� ��ų�� ���� �Ͻðڽ��ϱ�?
							g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
								GET_REPLACED_STRING( ( STR_ID_21659, "L", pMyPetInfo.m_wstrPetName ) ),
								UIM_PET_DROP_ITEM_PICK_UP_OK, g_pMain->GetNowState() );
							return true;
						}
						else
						{	//��ȯ�� ���� �����ϴ�.
							if( NULL != g_pData->GetUIManager() && //�� �̺���
								NULL != g_pData->GetUIManager()->GetUIPetList() &&
								0 == g_pData->GetUIManager()->GetUIPetList()->GetPetListSize() )
							{
								g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_21662 ) ,g_pMain->GetNowState() );
							}
							else
							{
								g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999),
									GET_REPLACED_STRING( ( STR_ID_21660, "L", g_pMain->GetKeyPad()->GetKeyString( GA_PET ) ) ),
									g_pMain->GetNowState() );
							}
							return true;
						}
					}
					return true;
				}
				break;

			case HALLOWEEN_TRANSFORM_POSION:
				{
					if( NULL != g_pData && NULL != g_pData->GetPetManager() )
					{
						//�κ��丮���� ��� �� ���� ������ ��ȯ�� �꿡�� ���� ��Ű�� ���� getmypet�̿�
						CX2PET* pMyPet = g_pData->GetPetManager()->GetMyPet();
						if( NULL != pMyPet )
						{
							CX2PET::PetInfo& pMyPetInfo = pMyPet->GetPetInfo();
							//������ ��� �Ұ� ����ó��
							//1. �ҷ��� ���� ��ȭ�ܰ谡 ����ü �� ���� ��� ����
							//2. ģ�е� 100%�϶��� ��� ����

							if( pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_TANYA || pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_LAEL ||
								pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_LOJETA || pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_NARENEA ||
								pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_TANYA_F || pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_LAEL_F ||
								pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_LOJETA_F || pMyPetInfo.m_PetId == CX2PetManager::PUI_HALLOWEEN_PUMPKIN_NARENEA_F )
							{
								CX2PetManager::PetTemplet* pPetTemplet =
									g_pData->GetPetManager()->GetPetTemplet( static_cast<CX2PetManager::PET_UNIT_ID>(pMyPetInfo.m_PetId) );
								UINT uiEvolutionStep = static_cast<UINT>(pMyPetInfo.m_Evolution_Step);//�� ��ȭ �ܰ�

								if( NULL != pPetTemplet &&
									uiEvolutionStep < pPetTemplet->m_vecPetStatus.size() ) //������ ũ�Ⱑ �ε������� Ŭ ���� ����
								{
									int iPetStatus = pPetTemplet->m_vecPetStatus[ uiEvolutionStep ];
									if( 3 == iPetStatus )  //����ü�϶�
									{
										if( pMyPet->GetNowIntimacy() >= 1.0f )	// ģ�е��� 100% �϶�
										{
											m_UseTransformdItemUID = pItem->GetUID();
											m_UseTransformItemPetUID = pMyPetInfo.m_PetUid;
											g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_809 ), UIM_PET_HALLOWEEN_TRANSFORM_POSION_OK, g_pMain->GetNowState() );
										}
										else // ģ�е��� 100% �ƴϸ� �ȵ�
										{
											g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_28199 ) ,g_pMain->GetNowState() );
										}
									}
									else // ����ü�� �ƴϸ� �ȵ�~
									{
										g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_28197 ) ,g_pMain->GetNowState() );
									}
								}
							}
							else
							{
								g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_28197 ) ,g_pMain->GetNowState() );
							}
						}
						else
						{
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_28199 ), g_pMain->GetNowState() );
						}
					}
					return true;
				}
				break;

				//{{ ����� : [2009/9/8] //		��� ���� ������ ���
			case GUILD_CREATE_ITEM_ID:
				{
					if ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP )
					{
						if ( g_pData->GetGuildManager()->DidJoinGuild() == false )
							g_pData->GetGuildManager()->GetUIGuild()->SetShowCreateGuildDlg( true, pItemSlot->GetItemUID() );
						else
						{
							g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4498 ), g_pMain->GetNowState() );
						}

						return true;
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4495 ),g_pMain->GetNowState() );
						return true;
					}

				} break;
				//}} ����� : [2009/9/8] //		��� ���� ������ ���
			case WARP_ITEM_FREE_ITEM_ID:
				{
					UseWarpItem(pItemSlot, false); //ED�Ҹ���� ����ϴ� �����̿��
					return true;
				}
			case WARP_ITEM_ED_CONSUMPTION_ITEM_ID:
				{
					UseWarpItem(pItemSlot, true);  //ED�Ҹ��ϰ� ����ϴ� �����̿��
					return true;
				}	/// case
				break;

			case UNLIMITED_SECOND_CHANGE_JOB_ITEM_ID:
				{
					UseUSCJobItem(pItemSlot);
					return true;
				}
				break;

				//{{ oasis907 : ����� [2009.11.30] // ��� ��ų �ʱ�ȭ
			case INIT_GUILD_SKILL_TREE_ITEM_ID:
				{
					if( true == pUnitData->m_UserSkillTree.CanIInitGuildSkillTree() )
					{
						m_UsedItemUID = pItem->GetUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4802 ), UIM_GUILD_SKILL_INIT_OK, g_pMain->GetNowState() );
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_2679 ), g_pMain->GetNowState() );
					}
					return true;
				} break;
				//}} oasis907 : ����� [2009.11.30] //

			case GUILD_CASH_SKILL_ITEM_ID:
			case EVENT_GUILD_CASH_SKILL_ITEM_ID:
				{
					//{{ oasis907 : ����� [2009.11.19] // ��帶���Ͱ� �ƴ� ��� ����
					if(g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_byMemberShipGrade != CX2GuildManager::GUG_MASTER)
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(300, 250), GET_STRING( STR_ID_4801 ), g_pMain->GetNowState() );
						return true;
					}
					else
					{
						Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( pItemSlot->GetItemUID() );
						return true;
					}
				}

			case SKILL_PLUS_ITEM_ID: // oasis907 : ����� [2010.12.9] // �׳�ý� �ູ ���
			case SKILL_PLUS_ITEM_ID_JP:
			case SKILL_POINT_10_30DAY_USE_INVEN_JP:
			case SKILL_POINT_10_15DAY_USE_INVEN_JP:
			case SKILL_POINT_30_7DAY_USE_INVEN:
			case SKILL_POINT_60_7DAY_USE_INVEN:
			case SKILL_POINT_60_15DAY_USE_INVEN:
			case SKILL_POINT_60_30DAY_USE_INVEN:
			case SKILL_POINT_60_7DAY_USE_INVEN_2:
			case SKILL_POINT_30_7DAY_USE_INVEN_2:
			case SKILL_POINT_130_1DAY_USE_INVEN:
			case SKILL_POINT_30_1DAY_USE_INVEN:
			case SKILL_POINT_60_1DAY_USE_INVEN:
				//{{ 2011. 11. 30	������	��Ű�� ��ǰ �߰�
			case SKILL_POINT_5_USE_INVEN_ITEM_ID:
			case SKILL_POINT_10_USE_INVEN_ITEM_ID:
			case SKILL_POINT_5_USE_INVEN_ITEM_ID_7_DAY:
				//}}
			case SKILL_POINT_5_USE_INVEN_ITEM_ID_15_DAY :
			case SKILL_POINT_5_USE_INVEN_ITEM_ID_30_DAY :
			case SKILL_POINT_5_USE_INVEN_ITEM_ID_60_DAY :
			case SKILL_POINT_10_USE_INVEN_ITEM_ID_30_DAY :
			case SKILL_POINT_30_USE_INVEN_ITEM_ID_15_DAY :
			case SKILL_POINT_60_USE_INVEN_ITEM_ID_15_DAY :
			case SKILL_POINT_30_USE_INVEN_ITEM_ID_30_DAY :
			case SKILL_POINT_60_USE_INVEN_ITEM_ID_30_DAY :
				//}}
			case EVENT_SKILL_POINT_5_USE_INVEN_15_DAY:
			case SKILL_POINT_30_14DAY_USE_INVEN:
			case LURIEL_GNOSIS_30_15DAY:
			case LURIEL_GNOSIS_30_30DAY:
			case LURIEL_GNOSIS_30_60DAY:
			case LURIEL_GNOSIS_60_15DAY:
			case LURIEL_GNOSIS_60_30DAY:
			case LURIEL_GNOSIS_60_60DAY:
			case LURIEL_GNOSIS_30_7DAY:
			case LURIEL_GNOSIS_60_7DAY:
				{
					m_UsedItemUID = pItemSlot->GetItemUID();
					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_9835 ), UIM_SKILL_PLUS_ITEM_USE, g_pMain->GetNowState() );
					return true;
				} break;

				//��� ��� ��
			case PROTECTION_OF_NUT_ITEM_ID:
			case PROTECTION_OF_PUTA_ITEM_ID:
			case PROTECTION_OF_LAHELL_ITEM_ID:
			case PROTECTION_OF_JAKIELL_ITEM_ID:
				//���� ��� ��
			case BELSSING_OF_SERAPHIM_ITEM_ID:
			case BELSSING_OF_GEV_ITEM_ID:
			case BELSSING_OF_AMON_ITEM_ID:
			case BELSSING_OF_CRONOS_ITEM_ID:

			case ELIXIR_GIANT_POTION:
			case ELIXIR_BLAZING_BOMB:
			case ELIXIR_SPIRIT_OF_CHASER:
			case ELIXIR_CRADLE_OF_LITTLE_FAIRY:
			case ELIXIR_ICE_BALL_OF_DENIPH:
			case ELIXIR_FEATHER_OF_VENTUS:
			case ELIXIR_FLAME_RING_OF_ROSSO:
			case ELIXIR_BIG_HAND_POTION:
			case BUFF_RURIEL_MANA_ENERGIZE_POTION:
				{
					switch ( g_pMain->GetNowStateID() )
					{
					case CX2Main::XS_DUNGEON_GAME:
					case CX2Main::XS_BATTLE_FIELD:
						{
							m_DefencedungeonPotion	 = pItemSlot->GetItemUID();
                            m_DefencedungeonPotionID = pItem->GetItemTemplet()->GetItemID();

							//�������� ���� ������ ���ٸ�, ��� �� ������ ���� ����
							Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_DefencedungeonPotion, 1, m_DefencedungeonPotionID );
						} break;

					default:
						{
							// ������ �ƴϸ� ����� �� ����.
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_16529 ),g_pMain->GetNowState() );
							return true;
						} break;
					}
				} break;
				//{{ 2011.10.24 ������ : ��� Ƽ�� �۾� ( ����ϸ� ĳ�� �ִ� ������ )
				//}}
			case INVENTORY_SLOT_ADD_ITEM_EQUIP_EVENT:
				{

					// ���� : ��� �� �κ� Ȯ���� ���� �Ѱ� �׽�Ʈ�� ����.
					if(g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_EQUIP) < INVENTORY_SLOT_MAX_NUM)
					{
						// ���� : ���� - ��Ʈ�� �־�� �Ѵ�.
						m_UsedItemUID = pItemSlot->GetItemUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_13554 ), UIM_USE_EXPAND_INVENTORY_OK, g_pMain->GetNowState() );
					}
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), NetError::GetErrStrF( NetError::ERR_BUY_CASH_ITEM_19 ), g_pMain->GetNowState() );
				} break;
			case INVENTORY_SLOT_ADD_ITEM_ACCESSORY_EVENT:
				{

					// ���� : ��� �� �κ� Ȯ���� ���� �Ѱ� �׽�Ʈ�� ����.
					if(g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_ACCESSORY) < INVENTORY_SLOT_MAX_NUM)
					{
						// ���� : ���� - ��Ʈ�� �־�� �Ѵ�.
						m_UsedItemUID = pItemSlot->GetItemUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_13554 ), UIM_USE_EXPAND_INVENTORY_OK, g_pMain->GetNowState() );
					}
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), NetError::GetErrStrF( NetError::ERR_BUY_CASH_ITEM_19 ), g_pMain->GetNowState() );
				} break;
			case INVENTORY_SLOT_ADD_ITEM_QUICK_SLOT_EVENT:
				{

					// ���� : ��� �� �κ� Ȯ���� ���� �Ѱ� �׽�Ʈ�� ����.
					if(g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_QUICK_SLOT) < INVENTORY_SLOT_MAX_NUM)
					{
						// ���� : ���� - ��Ʈ�� �־�� �Ѵ�.
						m_UsedItemUID = pItemSlot->GetItemUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_13554 ), UIM_USE_EXPAND_INVENTORY_OK, g_pMain->GetNowState() );
					}
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), NetError::GetErrStrF( NetError::ERR_BUY_CASH_ITEM_19 ), g_pMain->GetNowState() );
				} break;
			case INVENTORY_SLOT_ADD_ITEM_MATERIAL_EVENT:
				{

					// ���� : ��� �� �κ� Ȯ���� ���� �Ѱ� �׽�Ʈ�� ����.
					if(g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_MATERIAL) < INVENTORY_SLOT_MAX_NUM)
					{
						// ���� : ���� - ��Ʈ�� �־�� �Ѵ�.
						m_UsedItemUID = pItemSlot->GetItemUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_13554 ), UIM_USE_EXPAND_INVENTORY_OK, g_pMain->GetNowState() );
					}
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), NetError::GetErrStrF( NetError::ERR_BUY_CASH_ITEM_19 ), g_pMain->GetNowState() );
				} break;
			case INVENTORY_SLOT_ADD_ITEM_QUEST_EVENT:
				{

					// ���� : ��� �� �κ� Ȯ���� ���� �Ѱ� �׽�Ʈ�� ����.
					if(g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_QUEST) < INVENTORY_SLOT_MAX_NUM)
					{
						// ���� : ���� - ��Ʈ�� �־�� �Ѵ�.
						m_UsedItemUID = pItemSlot->GetItemUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_13554 ), UIM_USE_EXPAND_INVENTORY_OK, g_pMain->GetNowState() );
					}
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), NetError::GetErrStrF( NetError::ERR_BUY_CASH_ITEM_19 ), g_pMain->GetNowState() );
				} break;
			case INVENTORY_SLOT_ADD_ITEM_SPECIAL_EVENT:
				{

					// ���� : ��� �� �κ� Ȯ���� ���� �Ѱ� �׽�Ʈ�� ����.
										if(g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemMaxNum(CX2Inventory::ST_SPECIAL) < INVENTORY_SLOT_MAX_NUM)
					{
						// ���� : ���� - ��Ʈ�� �־�� �Ѵ�.
						m_UsedItemUID = pItemSlot->GetItemUID();
						g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_13554 ), UIM_USE_EXPAND_INVENTORY_OK, g_pMain->GetNowState() );
					}
					else
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), NetError::GetErrStrF( NetError::ERR_BUY_CASH_ITEM_19 ), g_pMain->GetNowState() );
				} break;

			case EVENT_PROPOSE_ITEM:
				{
					switch( g_pMain->GetNowStateID() )
					{
					case CX2Main::XS_VILLAGE_MAP:
						{
							m_UsedItemUID = pItemSlot->GetItemUID();
							g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_REPLACED_STRING( ( STR_ID_3712, "L", pItem->GetFullName()) ), UIM_USE_PROPOSE_ITEM, g_pMain->GetNowState() );
						}
						break;
					default:
						{
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_24660 ),g_pMain->GetNowState() );
							return true;
						}
						break;
					}
				}
				break;
			case EVENT_DIVORCE_ITEM:
				{
					switch( g_pMain->GetNowStateID() )
					{
					case CX2Main::XS_VILLAGE_MAP:
						{
							m_UsedItemUID = pItemSlot->GetItemUID();
							g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_REPLACED_STRING( ( STR_ID_3712, "L", pItem->GetFullName()) ), UIM_USE_DIVORCE_ITEM, g_pMain->GetNowState() );
						}
						break;
					default:
						{
							g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_24660 ),g_pMain->GetNowState() );
							return true;
						}
						break;
					}
				}
				break;


			case SKILL_NOTE_ITEM_ID:
			default:
				{
					Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( pItemSlot->GetItemUID() );
				} break;
			}
		}
		return true;
	}





	//{{ 2007. 10. 8  ������  �κ����� �������� ������ Ŭ������ ��� [�ӽ� �׽�Ʈ]
	const CX2Item::ItemData& kItemData = pItem->GetItemData();
	{
        // ������������ ��뷹������ ���� ���������� ���ٸ�.
		if (g_pData->GetSelectUnitLevel() < g_pData->GetItemManager()->GetItemTemplet( kItemData.m_ItemID )->GetUseLevel() && g_pData->GetItemManager()->GetItemTemplet( kItemData.m_ItemID )->GetItemType() == CX2Item::IT_SPECIAL )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),  GET_STRING(STR_ID_5014) , g_pMain->GetNowState() );
            return true;
		}

		bool	bOpenRequiredED = false;
		int		iRequiredED = 0;

		//{{ kimhc // 2009-12-22 // ���� �����ۿ� ���� Ű�� �����ֵ��� �߰�
		std::pair< CX2ItemManager::RandomItemItr, CX2ItemManager::RandomItemItr > pairRandomItem = g_pData->GetItemManager()->GetRandomItemData( kItemData.m_ItemID );

		if ( g_pData->GetItemManager()->IsRandomItem( pairRandomItem ) == true
			//ť�� ���� �������̶�� �������� �ʱ�
			&& false == m_bProcessCubeOpen
			)
		{
			CX2ItemManager::RandomItemItr startItr	= pairRandomItem.first;
			CX2ItemManager::RandomItemItr endItr	= pairRandomItem.second;

			std::multimap< int, CX2ItemManager::RandomItemData* >::difference_type dfCount = 0;

			CX2ItemManager::RandomItemItr tempStartItr = startItr;

			while ( tempStartItr != endItr )
			{
				dfCount++;
				tempStartItr++;
			}

			if ( dfCount == 1 )		// Ű�� �Ѱ���
			{
				//Ű ������, ED�� �� �� �ֵ��� ����
				CX2ItemManager::RandomItemData* pRandomItem = startItr->second;
				bool bEnoughRequiredItem = true;  // ���� �䱸 �������� ������� ����

				wstring wstrMsg = L"";//Ű, �Ǵ� ED�� ������� ���� �� ����ϴ� ���� �˾� �޼���
				const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( kItemData.m_ItemID );
				if( NULL != pItemTemplet )
					wstrMsg = GET_REPLACED_STRING( ( STR_ID_21083, "S", pItemTemplet->GetFullName_() ));

				//���� ���� Ű ������ üũ
				if( 0 < pRandomItem->m_RequiredKeyCount )
				{
					int keyItemNum = pUnit->GetInventory().GetNumItemByTID( pRandomItem->m_KeyItemID );
					if ( keyItemNum < pRandomItem->m_RequiredKeyCount )
					{
						{
							const CX2Item::ItemTemplet* pItemTemplet =
								g_pData->GetItemManager()->GetItemTemplet( pRandomItem->m_KeyItemID );
							if( NULL != pItemTemplet )
							{
								const wchar_t* pwszitemName = pItemTemplet->GetFullName_();
								wstrMsg += GET_REPLACED_STRING( ( STR_ID_808, "SSi", pwszitemName, L"", pRandomItem->m_RequiredKeyCount ) );
								bEnoughRequiredItem = false;
							}
						}
					}
				}

				//���� ���� ED üũ
				iRequiredED = pRandomItem->m_iRequiredED;	//������ �ʿ��� ED
				int iMyED = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_ED; //���� ED
				if( 0 < iRequiredED )
				{
					bOpenRequiredED = true;
					if( iMyED < iRequiredED )
					{
						wstrMsg += GET_REPLACED_STRING( ( STR_ID_21081, "L", g_pMain->GetEDString(iRequiredED) ));
						bEnoughRequiredItem = false;
					}
				}

				//Ű�Ǵ� ED�� ������� �ʴٸ� ���� �˾� ����ϰ� return
				if( false == bEnoughRequiredItem )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), wstrMsg.c_str(), g_pMain->GetNowState() );
					return false;
				}
			}
			else if ( dfCount > 1 )							// �ΰ� �̻��̸�
			{
				bool bHaveKey = false;
				while ( startItr != endItr )
				{
					CX2ItemManager::RandomItemData* pRandomItem = startItr->second;
					int keyItemNum = pUnit->GetInventory().GetNumItemByTID( pRandomItem->m_KeyItemID );

					if ( keyItemNum >= pRandomItem->m_RequiredKeyCount )
					{
						bHaveKey = true;
						break;
					}
					++startItr;
				}

				if ( bHaveKey == false )
				{
					if( 108900 == kItemData.m_ItemID )
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_17700 ), g_pMain->GetNowState() );

					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), g_pData->GetCubeOpenImageManager()->GetEventCubeNoKeyString( kItemData.m_ItemID ), g_pMain->GetNowState() );
					}
					return false;
				}

			}
			else				// 0 �̸�
				return false;

			// ���� �������̴�~
			//{{ 2009.01.20 ���¿�
			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
			{
				if ( m_pDLGUIInventory != NULL )
				{
					CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)m_pDLGUIInventory->GetControl( L"CheckBox_Auth" );
					if ( pCheckBox != NULL )
					{
						if ( pCheckBox->GetChecked() == true )
						{
							m_bUsingRandomItemAutoOpen = true;
						}
					}
				}
			}
			//}}

			m_RandomItemID = kItemData.m_ItemID;
			m_iRandomItemUID = pItemSlot->GetItemUID();

			g_pData->GetCubeOpenImageManager()->SetPlaying( false );
			g_pData->GetCubeOpenImageManager()->SetStart( false );
			g_pData->GetCubeOpenImageManager()->SetSoundPlaying( false );
			m_bHideCubeOpneBar = false;

			bool bAttractionItemCheck = false;
			for(int i=0; i<std::ssize(ATTRACTION_ITEM_ID); ++i)
			{
				if( kItemData.m_ItemID == ATTRACTION_ITEM_ID[i] )
				{
					bAttractionItemCheck = true;

					m_eRandomItemEventType = RIOET_ICE_HEATER;

					break;
				}
			}

			for(int i=0; i<std::ssize(MINI_ATTRACTION_ITEM_ID); ++i)
			{
				if( kItemData.m_ItemID == MINI_ATTRACTION_ITEM_ID[i] )
				{
					bAttractionItemCheck = true;

					m_eRandomItemEventType = RIOET_MINI_ICE_HEATER;

					break;
				}
			}

			m_bIsAllInOne = false;
			if( kItemData.m_ItemID == ALL_IN_ONE_ATTRACTION_ITEM_ID )
			{
				bAttractionItemCheck = true;
				m_bIsAllInOne = true;

				m_eRandomItemEventType = RIOET_ALL_IN_ONE_ICE_HEATER;

			}

			m_bIsRidingGacha = false;
			if( kItemData.m_ItemID == ANCIENT_FOSSIL_RIDING_PET )
			{
				bAttractionItemCheck = true;
				m_bIsRidingGacha = true;

				m_eRandomItemEventType = RIOET_RIDING_PET_GACHA;

			}

			m_bIsHalloweenIceHeater = false;
			for(int i=0; i<std::ssize(HALLOWEEN_ATTRACTION_ITEM_ID); ++i)
			{
				if( kItemData.m_ItemID == HALLOWEEN_ATTRACTION_ITEM_ID[i] )
				{
					bAttractionItemCheck = true;
					m_bIsHalloweenIceHeater = true;

					m_eRandomItemEventType = RIOET_HALLOWEEN_ICE_HEATER;

				}
			}

			bool bGoldAttractionItemCheck = false;
			for(int i=0; i<std::ssize(GOLD_ATTRACTION_ITEM_ID); ++i)
			{
				if( kItemData.m_ItemID == GOLD_ATTRACTION_ITEM_ID[i] )
				{
					bGoldAttractionItemCheck = true;

					m_eRandomItemEventType = RIOET_GOLD_ICE_HEATER;

					break;
				}
			}

			if( bAttractionItemCheck == true || bGoldAttractionItemCheck == true )
			{
				//{{ kimhc // �ǽð� ���ҵ� �� ���������� ������ ���� ���ƾ� �� �͵�
				if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
				{
					g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2646 ),g_pMain->GetNowState() );

					return true;
				}
				//}} kimhc // �ǽð� ���ҵ� �� ���������� ������ ���� ���ƾ� �� �͵�


				/// Ÿ�Կ� ���� ������ ��� ���� ����
				m_pNowIceHeaterEventData = NULL;

				std::map< RANDOM_ITEM_OPEN_EVENT_TYPE, IceHeaterEventData* >::const_iterator mit;

				mit = m_mapIceHeaterEventData.find( m_eRandomItemEventType );

				if( mit != m_mapIceHeaterEventData.end() )
					m_pNowIceHeaterEventData = mit->second;		/// ���� �����ִ� �������� ��� ���� ����

				m_pDLGOpenAttraction = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Ice_Heater_Use.lua" );
				g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGOpenAttraction );


				/// ������ ���� ���̾�α� ����
				if( NULL != m_pDLGOpenAttraction )
				{
					CKTDGUIStatic* pStaticHeaterTitle		= (CKTDGUIStatic*) m_pDLGOpenAttraction->GetControl( L"g_pStaticHeater_Title" );
					CKTDGUIStatic* pStaticHelperString		= (CKTDGUIStatic*) m_pDLGOpenAttraction->GetControl( L"helper" );
					CKTDGUIStatic* pStaticFeel				= (CKTDGUIStatic*) m_pDLGOpenAttraction->GetControl( L"g_pStaticFeel" );

					if( NULL != pStaticHeaterTitle && NULL != pStaticHelperString && NULL != pStaticFeel && NULL != m_pNowIceHeaterEventData )
					{
						wstring wstrMainDialogTitleTexture		= m_pNowIceHeaterEventData->m_wstrMainDialogTitleTexture;
						wstring wstrMainDialogTitleTextureKey	= m_pNowIceHeaterEventData->m_wstrMainDialogTitleTextureKey;
						D3DXVECTOR2 vecIconOffSetPosition		( m_pNowIceHeaterEventData->m_fCautionIconOffSetPositionX,
																  m_pNowIceHeaterEventData->m_fCautionIconOffSetPositionY );
						D3DXVECTOR2 vecStringOffSetPosition		( m_pNowIceHeaterEventData->m_fCautionStringOffSetPositionX,
																  m_pNowIceHeaterEventData->m_fCautionStringOffSetPositionY );
						int iCautionStringID					= m_pNowIceHeaterEventData->m_iCautionStringNum;

						if( NULL != pStaticHeaterTitle->GetPicture(0) )							/// Ÿ��Ʋ�� �ؽ�ó ����
						{
							pStaticHeaterTitle->GetPicture(0)->SetTex( wstrMainDialogTitleTexture.c_str(), wstrMainDialogTitleTextureKey.c_str() );
							pStaticHeaterTitle->GetPicture(0)->SetSizeAsTextureSize();			/// �ؽ��� ����Ǹ� ����� �ؽ��� ũ��� ���� �ؿ��� �ڵ� �߰�
						}

						pStaticHeaterTitle->SetShow( true );
						pStaticHelperString->SetString( 0, GET_STRING( iCautionStringID ) );	/// �ȳ� ��Ʈ�� ����
						pStaticHelperString->SetOffsetPos(vecStringOffSetPosition );			/// �ȳ� ��Ʈ�� ��ġ �̵�
						pStaticFeel->SetOffsetPos( vecIconOffSetPosition );						/// ��Ʈ�� �� ����ǥ ������ ��ġ �̵�
					}
				}


				// �߰��� �� �׷�����
				D3DXVECTOR3 boxPos = m_pDLGOpenAttraction->GetDummyPos( 0 );
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MeltMachine = NULL;


				/// ������ ���� �� ��� ��� ����
				const wstring wstrWaitMeshPlayerName =
					NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrWaitMeshPlayerName : L"FireMachineWait" ;

				pMeshInst_MeltMachine = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  wstrWaitMeshPlayerName.c_str(), boxPos.x, boxPos.y, boxPos.z , 0,0,0, 0,0,0 );


				g_pKTDXApp->GetDGManager()->RemoveObjectChain(pMeshInst_MeltMachine);

				if( NULL != m_pDLGOpenAttraction )
				{
					m_pDLGOpenAttraction->SetHasUnit( pMeshInst_MeltMachine );
				}

				m_hMeshInstMeltMachine = pMeshInst_MeltMachine->GetHandle();


				if(g_pChatBox != NULL)
					g_pChatBox->HideChatEditBox();

			}
			else
			{
				if( IsAprilEventRandomItem(kItemData.m_ItemID) )
				{
					m_eRandomItemEventType = RIOET_APRIL_5TH_TREE_DAY_EVENT;
					m_pDLGRandomItem = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_809 ), UIM_OPEN_RANDOM_ITEM, g_pMain->GetNowState(), UIM_OPEN_RANDOM_ITEM_CANCLE );
				}
				else

					if( g_pData->GetCubeOpenImageManager()->IsEventCube(kItemData.m_ItemID) == true)
					{
						m_eRandomItemEventType = RIOET_EVENT_CUBE;
						m_bHideCubeOpneBar = g_pData->GetCubeOpenImageManager()->IsHideBarCube(kItemData.m_ItemID);
						m_pDLGRandomItem = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), g_pData->GetCubeOpenImageManager()->GetEventCubeOpenString(kItemData.m_ItemID), UIM_OPEN_RANDOM_ITEM_NEW, g_pMain->GetNowState(), UIM_OPEN_RANDOM_ITEM_CANCLE );
					}
					else
					if ( IsMonsterCardSetItem( kItemData.m_ItemID ) == true )
					{
						m_eRandomItemEventType = RIOET_MONSTER_CARD_SET;
						m_pDLGRandomItem = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_809 ), UIM_OPEN_RANDOM_ITEM, g_pMain->GetNowState(), UIM_OPEN_RANDOM_ITEM_CANCLE );
					}
					else
					{
						m_eRandomItemEventType = RIOET_NONE;

						bool bNoCubeOpenItemCheck = false;
						for(int i=0; i<std::ssize(NO_CUBE_OPEN_ITEM_ID); ++i)
						{
							if( kItemData.m_ItemID == NO_CUBE_OPEN_ITEM_ID[i] )
							{
								bNoCubeOpenItemCheck = true;
								break;
							}
						}

						UI_INVENTORY_MSG eOpenMSG = UIM_OPEN_RANDOM_ITEM_NEW;
						if( true == bNoCubeOpenItemCheck)
							eOpenMSG = UIM_OPEN_RANDOM_ITEM;
						//ED���� �������� ��� ��� �� �Ҹ� ED�˸� �߰�
						if( true == bOpenRequiredED )
						{
							const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( kItemData.m_ItemID );
							if( NULL != pItemTemplet )
							{
								m_pDLGRandomItem = 	g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
														GET_REPLACED_STRING( ( STR_ID_21082, "SL", pItemTemplet->GetFullName_(), g_pMain->GetEDString(iRequiredED) )) ,
														eOpenMSG, g_pMain->GetNowState(), UIM_OPEN_RANDOM_ITEM_CANCLE );
							}
						}
						else
						{
							m_pDLGRandomItem = 	g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_809 ), eOpenMSG,
																			g_pMain->GetNowState(), UIM_OPEN_RANDOM_ITEM_CANCLE );
						}
					}
			}
		}
		//}} kimhc // 2009-12-22 // ���� �����ۿ� ���� Ű�� �����ֵ��� �߰�
		else // ���� ť�갡 �ƴϸ�
		{
			if( true == IsAddSocketSlotItem( kItemData.m_ItemID ) )
			{
				UseAddSocketSlotItem( pItemSlot->GetItemUID() );
			}
			if( true == IsRestoreEvaluateItem( kItemData.m_ItemID ) )
			{
				UseRestoreEvaluteItem( pItemSlot->GetItemUID() );
			}
			if( IsAttachItem( kItemData.m_ItemID ) )
			{
				// ��ȭ�� ���� ��
				CX2State* pState = (CX2State*)g_pMain->GetNowState();
				if(pState->GetCursor() != NULL)
					pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_ATTACH_ITEM );
				m_CouponUid = pItemSlot->GetItemUID();

				// �� �ٲ�����..
				ChangeInventoryTab( CX2Inventory::ST_EQUIP );
				return true;
			}

			if( IsAttributeItem( kItemData.m_ItemID ) )
			{
				CX2State* pState = (CX2State*)g_pMain->GetNowState();
				if(pState->GetCursor() != NULL)
					pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_ATTACH_ATTRIB_ITEM );
				m_CouponUid = pItemSlot->GetItemUID();

				ChangeInventoryTab( CX2Inventory::ST_EQUIP );
				return true;
			}


			switch( kItemData.m_ItemID )
			{
			case CX2EnchantItem::ATI_RED:
			case CX2EnchantItem::ATI_BLUE:
			case CX2EnchantItem::ATI_GREEN:
			case CX2EnchantItem::ATI_WIND:
			case CX2EnchantItem::ATI_LIGHT:
			case CX2EnchantItem::ATI_DARK:
				{
					OpenElBreakWindow( pItem );

					return true;
				} break;

			case CX2EnchantItem::ATI_UNKNOWN:
				{
					//�� �Ӽ� �Ǻ��Ⱑ �ִ��� Ȯ��
					if ( pUnit->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_IDENTIFY_STONE ) > 0 )
					{
						OpenElChangerWindow( true, pItem );

						//m_ElChangerItemUID	= pItem->GetItemData().m_ItemUID;
						//m_ElChangerItemCount = 1;
						//if( m_pDLGIdentifyStone == NULL )
						//{
						//	m_pDLGIdentifyStone = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_5017), UIM_UNKNOWN_TO_IDENTIFY, g_pMain->GetNowState() );
						//}
					}
					else
					{
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_811 ), g_pMain->GetNowState() );
					}

					return true;
				} break;




				//case 203800: // ��ü ��ų �ʱ�ȭ ������
				//	{
				//		if( NULL != m_pSkillTreeUI )
				//		{
				//			m_pSkillTreeUI->OpenResetAllSkillPopup( true );
				//		}
				//	} break;
			}
		}
	}
	//}}

	//{{ kimhc // 2009-08-19 // ���ν�Ű�� �������� ������ Ŭ�� ��
    switch ( pItem->GetItemTemplet()->GetItemID() )
	{
	case ITEM_FOR_SEAL_NORMAL:								// ������ �� ����(�Ͼ��)
	case ITEM_FOR_SEAL_RARE:								// ������ �� ����(�����)
	case ITEM_FOR_SEAL_ELITE:								// ������ �� ����(�����)
	case ITEM_FOR_SEAL_UNIQUE:								// ������ �� ����(Ȳ�ݻ�)
	case ITEM_FOR_SEAL_NORMAL_EVENT:						// ������ �� ���� �̺�Ʈ(�Ͼ��)
	case ITEM_FOR_SEAL_ELITE_EVENT:							// ������ �� ���� �̺�Ʈ(�����)
	case ITEM_FOR_SEAL_NORMAL_EVENT_RURIEL:
	case ITEM_FOR_SEAL_RARE_EVENT_RURIEL:
	case ITEM_FOR_SEAL_ELITE_EVENT_RURIEL:
	case ITEM_FOR_SEAL_UNIQUE_EVENT_RURIEL:
	case ITEM_FOR_SEAL_NORMAL_EVENT_ARIEL:
	case ITEM_FOR_SEAL_RARE_EVENT_ARIEL:
	case ITEM_FOR_SEAL_ELITE_EVENT_ARIEL:
	case ITEM_FOR_SEAL_UNIQUE_EVENT_ARIEL:
		{
			RClickItemForSeal( pItem->GetUID() );
		}

		break;

	default:
		break;
	}
	//}} kimhc // 2009-08-19 // ���ν�Ű�� �������� ������ Ŭ�� ��



	return false;
}

//{{ kimhc // 2009-08-19 // ������ ����
void CX2UIInventory::RClickItemForSeal( UidType itemUID )
{
	CX2State*	pState	= NULL;
	pState				= static_cast< CX2State* >( g_pMain->GetNowState() );
	if ( pState == NULL )
	{
		ASSERT( !"GetNowState() is NULL" );
		return;
	}

	CX2Cursor*	pCursor = NULL;
	pCursor				= pState->GetCursor();
	if ( pCursor == NULL )
	{
		ASSERT( !"GetCursor() is NULL" );
		return;
	}

	pCursor->ChangeCursorState( CX2Cursor::XCS_SEAL_ITEM );
	m_itemUIDForSeal	= itemUID;
}

bool CX2UIInventory::CanSealUpThisItem( CX2Item* pItem ) const
{
    if ( pItem == NULL )
    {
		ASSERT( !"pItem is NULL" );
		return false;
    }

    const CX2Item::ItemTemplet*		pItemTemplet	= pItem->GetItemTemplet();
	if ( pItemTemplet == NULL )
	{
		ASSERT( !"GetItemTemplet is NULL" );
		return false;
	}

	const CX2Item::ItemData&		kItemData		= pItem->GetItemData();

	//{{ kimhc // 2010-01-05 // PC�� �����̾� ����
	if ( pItemTemplet->GetIsPcBang() == true )
	{
		g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
		return false;
	}
	//}} kimhc // 2010-01-05 // PC�� �����̾� ����

	// �Ⱓ�� �������� ������ �� �����ϴ�.
	if ( kItemData.m_Period > 0 )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4394 ), g_pMain->GetNowState() );
		return false;
	}

	// ��� �Ұ� ������ �������� ���� �� �� �����ϴ�.
	if ( pItem->IsDisabled() == true )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4395 ), g_pMain->GetNowState() );
		return false;
	}

	// ���(�Ǽ��縮 ����), �ƹ�Ÿ �������� �ƴϸ� ���� �Ұ�
	switch ( pItemTemplet->GetItemType() )
	{
	case CX2Item::IT_WEAPON:
	case CX2Item::IT_DEFENCE:
	case CX2Item::IT_ACCESSORY:
		// ���� ����
		break;

	default:
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4396 ), g_pMain->GetNowState() );
			return false;
		}
		break;
	}

	// �������� �ŷ��� �̹� ������(�ͼӵ��� ����) �������� �������� ���� �Ұ�
	if ( pItemTemplet->GetVested() == false ||
		kItemData.m_bIsSealed == true )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4396 ), g_pMain->GetNowState() );
		return false;
	}

	// ���� Ƚ��
	if ( kItemData.m_ucTimesToBeSealed >= pItemTemplet->GetMaxSealCount()
		&& pItemTemplet->GetMaxSealCount() != CX2Inventory::SEAL_COUNT_MAX
		) // ���ݱ��� ���ε� Ƚ���� �ִ�� ���� �ɼ� �ִ� Ƚ�� �̻��̸�
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4398 ), g_pMain->GetNowState() );
		return false;
	}

	// ���⳪ ���� ��� �������� 100% �̸��� ��� ���� �Ұ�
	if ( pItemTemplet->GetItemType() == CX2Item::IT_WEAPON || pItemTemplet->GetItemType() == CX2Item::IT_DEFENCE )
	{
		if ( pItemTemplet->GetPeriodType() == CX2Item::PT_ENDURANCE && kItemData.m_Endurance != pItemTemplet->GetEndurance() )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4393 ), g_pMain->GetNowState() );
			return false;
		}
	}

	// ������(����ũ ��� ���� ����)
	//// ����ũ ����� �������� ���� �Ұ�
	//if ( pItemTemplet->GetItemGrade() == CX2Item::IG_UNIQUE )
	//{
	//	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4397 ), g_pMain->GetNowState() );
	//	return false;
	//}

	return true;
}

bool CX2UIInventory::IsSealItem( CX2Item* pItem, int &iCanSealCount )
{
	if( pItem == NULL )
	{
		return false;
	}

	const CX2Item::ItemData&		kItemData		=	pItem->GetItemData();
	const CX2Item::ItemTemplet*		pItemTemplet	=	NULL;
	iCanSealCount = 0;

	pItemTemplet			= pItem->GetItemTemplet();

	if ( pItemTemplet == NULL )
	{
		ASSERT( !"GetItemTemplet is NULL" );
		return false;
	}
	//{{ kimhc // 2010-01-05 // PC�� �����̾� ����
	if ( pItemTemplet->GetIsPcBang() == true )
	{
		return false;
	}
	//}} kimhc // 2010-01-05 // PC�� �����̾� ����

	// ���(�Ǽ��縮 ����), �ƹ�Ÿ �������� �ƴϸ� ���� �Ұ�
	switch ( pItemTemplet->GetItemType() )
	{
	case CX2Item::IT_WEAPON:
	case CX2Item::IT_DEFENCE:
	case CX2Item::IT_ACCESSORY:
		// ���� ����
		break;

	default:
		{
			return false;
		}
		break;
	}

	// �������� �ŷ��� �̹� ������(�ͼӵ��� ����) �������� �������� ���� �Ұ�
	if( pItemTemplet->GetVested() == false ||
		kItemData.m_bIsSealed == true )
	{
		return false;
	}

	iCanSealCount = pItemTemplet->GetMaxSealCount() - kItemData.m_ucTimesToBeSealed;

	return true;
}

bool CX2UIInventory::Handler_EGS_UPDATE_ITEM_POSITION_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UPDATE_ITEM_POSITION_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();

	std::vector< KTradeShareItemResult >::iterator vit;
	for(vit = kEvent.m_vecShareItemResult.begin(); vit != kEvent.m_vecShareItemResult.end(); vit++ )
	{
		kInventory.UpdateItemUID( vit->m_iBeforeItemUID, vit->m_iNewItemUID );
	}
	//{{ kimhc // 2009-08-07 // ĳ���ͺ� ����
	if(g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_PRIVATE_BANK) == true )
	{
		g_pData->GetUIManager()->GetUIPrivateBank()->UpdateUI();
	}
	//}} kimhc // 2009-08-07 // ĳ���ͺ� ����
	SetInventorySort( m_NowInventorySortType, GetNowInvenSortTypePageNum() );

	return true;
}

//{{ ���� : ���ణ ��ȯ ������ ���������� üũ�ϴ� �Լ�
bool CX2UIInventory::CheckCanMoveShareItem( CX2Item* pFromItem, CX2Item* pToItem, KEGS_CHANGE_INVENTORY_SLOT_ITEM_REQ &kMoveInfo)
{
	// �Ѵ� NULL �̸� �ȵǴ� ���⼭�� üũ ����.
	if(pFromItem == NULL && pToItem == NULL)
		return false;

	// ���� ���� �������̳�?
	bool bCanSeal = false;

	// FromItem  ���� üũ
	if(pFromItem != NULL)
	{
		// 1. �Ⱓ�� ������ �ŷ� �Ұ�
		if( pFromItem->GetItemData().m_Period > 0 )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14855 ), g_pMain->GetNowState() );
			return false;
		}

		// 2. ���ξ����� üũ
		int iCanSealCount = 0;
		if( IsSealItem( pFromItem, iCanSealCount ) == true )
		{
			if( iCanSealCount <= 0 )
			{
				// ���� Ƚ�� 0. �� �ŷ� �Ұ�
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14844 ), g_pMain->GetNowState() );
				return false;
			}
			else
			{
				bCanSeal = true;
			}
		}
		// 3. �ŷ� �Ұ� ������ üũ
		else if( pFromItem->GetItemData().m_bIsSealed == false && pFromItem->GetItemTemplet()->GetVested() == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14851 ), g_pMain->GetNowState() );
			return false;
		}
	}

	// ToItem
	if(pToItem != NULL)
	{
		// 1. �Ⱓ�� ������ �ŷ� �Ұ�
		if( pToItem->GetItemData().m_Period > 0 )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14855 ), g_pMain->GetNowState() );
			return false;
		}

		// 2. ���ξ����� üũ
		int iCanSealCount = 0;
		if( IsSealItem( pToItem, iCanSealCount ) == true )
		{
			if( iCanSealCount <= 0 )
			{
				// ���� Ƚ�� 0. �� �ŷ� �Ұ�
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14844 ), g_pMain->GetNowState() );
				return false;
			}
			else
			{
				bCanSeal = true;
			}
		}
		// 3. �ŷ� �Ұ� ������ üũ
		else if( pToItem->GetItemData().m_bIsSealed == false && pToItem->GetItemTemplet()->GetVested() == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14851 ), g_pMain->GetNowState() );
			return false;
		}
	}

	if(bCanSeal == true)
	{
		// ���� �����ϴϱ� ��� ��ȯ�Ұ��� �����.
		m_kTempChangeSlotInfo = kMoveInfo;

		const CX2Item::ItemTemplet* pItemTemplet = pFromItem->GetItemTemplet();
		if( pItemTemplet != NULL && pItemTemplet->GetMaxSealCount() == CX2Inventory::SEAL_COUNT_MAX )
		{
			g_pData->GetServerProtocol()->SendPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_REQ, g_pData->GetUIManager()->GetUIInventory()->m_kTempChangeSlotInfo );
			g_pMain->AddServerPacket( EGS_CHANGE_INVENTORY_SLOT_ITEM_ACK );
		}
		else
		g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_14843 ), CX2UIPrivateBank::UPBCM_CHANGE_INVENTORY_SLOT_ITEM_OK, g_pMain->GetNowState() );
		return false;
	}

	return true;
}
//}}

void CX2UIInventory::ResetItemIDForSealAndCursorState()
{
	m_itemUIDForSeal	= 0;

	CX2State* pState	= NULL;
	pState				= static_cast< CX2State* >( g_pMain->GetNowState() );

	if( pState != NULL )
	{
		CX2Cursor* pCursor	= NULL;
		pCursor				= pState->GetCursor();

		if ( pCursor != NULL && pCursor->GetCurorState() == CX2Cursor::XCS_SEAL_ITEM )
		{
			pCursor->ChangeCursorState( CX2Cursor::XCS_NORMAL );
		}
	}
}

bool CX2UIInventory::Handler_EGS_SEAL_ITEM_REQ( UidType itemUID )
{
    if ( g_pData->GetMyUser() == NULL || g_pData->GetMyUser()->GetSelectUnit() == NULL )
    {
		ASSERT( !"MyUser is NULL" );
		m_pSlotItemToBeSealed	= NULL;
		ResetItemIDForSealAndCursorState();
		return true;
    }

	CX2Item*				pItemToBeSealed		= NULL;
	const CX2Inventory&			kInventory	 		= g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
	pItemToBeSealed					= kInventory.GetItem( itemUID );

	if ( pItemToBeSealed == NULL )
	{
		ASSERT( !"GetItem is NULL" );
		m_pSlotItemToBeSealed	= NULL;
		ResetItemIDForSealAndCursorState();
		return true;
	}

    const CX2Item::ItemTemplet*	pItemTemplet		= pItemToBeSealed->GetItemTemplet();

	if ( pItemTemplet == NULL )
	{
		ASSERT( !"GetItemTemplet is NULL" );
		m_pSlotItemToBeSealed	= NULL;
		ResetItemIDForSealAndCursorState();
		return true;
	}


	//if ( pItemData	== NULL )
	//{
	//	ASSERT( !"GetItemData is NULL" );
	//	m_pSlotItemToBeSealed	= NULL;
	//	ResetItemIDForSealAndCursorState();
	//	return true;
	//}

	if ( kInventory.GetItem( m_itemUIDForSeal ) == NULL ||
		 kInventory.GetItem( m_itemUIDForSeal )->GetItemTemplet() == NULL
         )
	{
		ASSERT( !"m_itemUIDForSeal is wrong!" );
		m_pSlotItemToBeSealed	= NULL;
		ResetItemIDForSealAndCursorState();
		return true;
	}


	bool					bCanBeSealed			= true;
	int						iItemIDForSeal			= 0;
    const CX2Item::ItemTemplet*	pItemTempletForSeal = NULL;	// � ������ ������ �˱����� itemTemplet

    iItemIDForSeal			= kInventory.GetItem( m_itemUIDForSeal )->GetItemTemplet()->GetItemID();

	// ������ �������� ��ް� ���ο� ���̴� �������� ���� ����� ��ġ�ϴ��� �˻�
	switch ( pItemTemplet->GetItemGrade() )
	{

	case CX2Item::IG_NORMAL:								// ������ �� ����(�Ͼ��)
		{
			switch ( iItemIDForSeal )
			{
			default:
				{
					bCanBeSealed		= false;
					pItemTempletForSeal	= g_pData->GetItemManager()->GetItemTemplet( ITEM_FOR_SEAL_NORMAL );
				} break;

			case ITEM_FOR_SEAL_NORMAL:
			case ITEM_FOR_SEAL_NORMAL_EVENT:
			case ITEM_FOR_SEAL_NORMAL_EVENT_RURIEL:
			case ITEM_FOR_SEAL_NORMAL_EVENT_ARIEL:
				break;
			}
		}
		break;

	case CX2Item::IG_RARE:								// ������ �� ����(�����)
		{
			switch ( iItemIDForSeal )
			{
			default:
				{
					bCanBeSealed		= false;
					pItemTempletForSeal	= g_pData->GetItemManager()->GetItemTemplet( ITEM_FOR_SEAL_RARE );
				} break;

			case ITEM_FOR_SEAL_RARE:
			case ITEM_FOR_SEAL_RARE_EVENT_RURIEL:
			case ITEM_FOR_SEAL_RARE_EVENT_ARIEL:
				break;
			}
		}
		break;

	case CX2Item::IG_ELITE:								// ������ �� ����(�����)
		{
			switch ( iItemIDForSeal )
			{
			default:
				{
					bCanBeSealed		= false;
					pItemTempletForSeal	= g_pData->GetItemManager()->GetItemTemplet( ITEM_FOR_SEAL_ELITE );
				} break;

			case ITEM_FOR_SEAL_ELITE:
			case ITEM_FOR_SEAL_ELITE_EVENT:
			case ITEM_FOR_SEAL_ELITE_EVENT_RURIEL:
			case ITEM_FOR_SEAL_ELITE_EVENT_ARIEL:
				break;
			}
		}
		break;

	case CX2Item::IG_UNIQUE:								// ������ �� ����(Ȳ�ݻ�)
		{
			switch( iItemIDForSeal )
			{
			default:
				{
					bCanBeSealed		= false;
					pItemTempletForSeal	= g_pData->GetItemManager()->GetItemTemplet( ITEM_FOR_SEAL_UNIQUE );
				} break;

			case ITEM_FOR_SEAL_UNIQUE:
			case ITEM_FOR_SEAL_UNIQUE_EVENT_RURIEL:
			case ITEM_FOR_SEAL_UNIQUE_EVENT_ARIEL:
				break;
			}
		}
		break;

	default:
		{
			ASSERT( !"This path is wrong!" );
			return true;
		}
		break;
	}

	// ��ġ���� ������
	if ( bCanBeSealed == false)
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
            GET_REPLACED_STRING( ( STR_ID_4401, "SS", pItemTemplet->GetFullName_(), pItemTempletForSeal->GetFullName_() ) ),
			g_pMain->GetNowState() );

		m_pSlotItemToBeSealed	= NULL;
		ResetItemIDForSealAndCursorState();
		return true;
	}

	KEGS_SEAL_ITEM_REQ kEGS_SEAL_ITEM_REQ;

	kEGS_SEAL_ITEM_REQ.m_iDestItemUID	= itemUID;
	kEGS_SEAL_ITEM_REQ.m_iSealItemUID	= m_itemUIDForSeal;

	g_pData->GetServerProtocol()->SendPacket( EGS_SEAL_ITEM_REQ, kEGS_SEAL_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_SEAL_ITEM_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_SEAL_ITEM_ACK( HWND /*hwnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SEAL_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	m_pSlotItemToBeSealed = NULL;
	ResetItemIDForSealAndCursorState();

	g_pMain->DeleteServerPacket( EGS_SEAL_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );

			return true;
		}
	}

	return false;
}

bool CX2UIInventory::Handler_EGS_UNSEAL_ITEM_REQ( UidType itemUID )
{
	KEGS_UNSEAL_ITEM_REQ kEGS_UNSEAL_ITEM_REQ;

	kEGS_UNSEAL_ITEM_REQ.m_iDestItemUID	= itemUID;
	m_itemUIDToBeUnSealed				= 0;

	g_pData->GetServerProtocol()->SendPacket( EGS_UNSEAL_ITEM_REQ, kEGS_UNSEAL_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_UNSEAL_ITEM_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_UNSEAL_ITEM_ACK( HWND /*hwnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_UNSEAL_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_UNSEAL_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );

			return true;
		}
	}

	return false;
}

//}} kimhc // 2009-08-19 // ���ν�Ű�� �������� ������ Ŭ�� ��

//�� ����, �ǵ�����
// true�� �Ǻ� ( �Ҹ� -> ���� )
// false�� ȯ�� ( ���� -> �Ҹ� )
void CX2UIInventory::OpenElChangerWindow( bool bIdentify, CX2Item* pItem )
{
	SAFE_DELETE_DIALOG( m_pDLGElChanger );

	if ( pItem == NULL ||
        pItem->GetItemTemplet() == NULL
        )
		return;

	m_ElChangerItemUID		= pItem->GetItemData().m_ItemUID;
	m_ElChangerItemCount	= 1;

	m_pDLGElChanger = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_El_Changer_Window.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGElChanger );

	//������ �̹��� �������ְ�.
	CKTDGUIStatic* pStaticItemImage = (CKTDGUIStatic*)m_pDLGElChanger->GetControl( L"ItemImage" );
	if ( pStaticItemImage != NULL && pStaticItemImage->GetPicture(0) != NULL )
	{
		pStaticItemImage->GetPicture(0)->SetTex(
            pItem->GetItemTemplet()->GetShopImage()
            );
	}

	//������ �̸� �������ְ�.
	CKTDGUIStatic* pStaticItemName = (CKTDGUIStatic*)m_pDLGElChanger->GetControl( L"Identify_Window_Price" );
	if ( pStaticItemName != NULL )
	{
		pStaticItemName->SetString(0,
            pItem->GetItemTemplet()->GetName()
            );
	}

	//�����ϴ°��� �ǵ����°��� �������ְ�.
	//�����ϴ°Ÿ� ������.. �ǵ����°Ÿ� ������ ���������� ����������������������������������...
	// true�� �Ǻ� ( �Ҹ� -> ���� )
	// false�� ȯ�� ( ���� -> �Ҹ� )
	CKTDGUIStatic* pStaticItemNum2 = NULL;
	int itemNum = 0;
	if ( bIdentify == true )
	{
		CKTDGUIControl* pControl = m_pDLGElChanger->GetControl( L"Identify_Tool" );
		if ( pControl != NULL )
			pControl->SetShow( true );

		itemNum = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_IDENTIFY_STONE );

		pStaticItemNum2 = (CKTDGUIStatic*)m_pDLGElChanger->GetControl( L"Identify_Window_Num2" );
	}
	else
	{
		CKTDGUIControl* pControl = m_pDLGElChanger->GetControl( L"Identify_Tool2" );
		if ( pControl != NULL )
			pControl->SetShow( true );

		itemNum = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_UNKNOWN_STONE );

		pStaticItemNum2 = (CKTDGUIStatic*)m_pDLGElChanger->GetControl( L"Identify_Window_Num2" );

	}

	if ( pStaticItemNum2 != NULL )
	{
		wstringstream wstrstm;
		wstrstm << itemNum << GET_STRING( STR_ID_24 );
		pStaticItemNum2->SetString( 0, wstrstm.str().c_str() );
	}


	UpdateElChangerDLG(false);
}



void CX2UIInventory::UpdateElChangerDLG( bool bReadIME )
{
	//before, we must check the type if It is an Identifier or an returner.

	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_ElChangerItemUID );
	if ( pItem == NULL ||
        pItem->GetItemTemplet() == NULL
        )
		return;
	if ( m_pDLGElChanger == NULL )
		return;

	int needStoneQuantity = 0;
    if ( pItem->GetItemTemplet()->GetItemID() == CX2EnchantItem::ATI_UNKNOWN )
	{
		//identifier
		needStoneQuantity = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_IDENTIFY_STONE );
	}
	else
	{
		//returner
		needStoneQuantity = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_UNKNOWN_STONE );
	}
	const int MAGIC_MAX_EL_CHANGE_ONCE = 50;
	int nowQuantity;
	nowQuantity = min( needStoneQuantity, MAGIC_MAX_EL_CHANGE_ONCE );
	nowQuantity = min( pItem->GetItemData().m_Quantity, nowQuantity );


	// IME���� �� �޾ƿ���
	CKTDGUIIMEEditBox* pQuantity = (CKTDGUIIMEEditBox*) m_pDLGElChanger->GetControl( L"IME_Identify_Window" );
	if(bReadIME)
	{
		m_ElChangerItemCount = g_pMain->GetEDFromString( pQuantity->GetText() );
	}

	if ( m_ElChangerItemCount > nowQuantity )
		m_ElChangerItemCount = nowQuantity;

	// Ȯ�����ְ�
	if ( m_ElChangerItemCount <= 0 )
	{
		if(bReadIME)				// �Է�â���� �޾ƿ� ���
			m_ElChangerItemCount = 0;
		else						// << �� ���� ��� : 0 �Ʒ��� max�� ��ȯ��Ų��.
			m_ElChangerItemCount = nowQuantity;
	}

	WCHAR buff[256] = {0};
	_itow_s( m_ElChangerItemCount, buff, 10 );
	wstring wstrNum = buff;
	pQuantity->SetText( wstrNum.c_str() );
}

void CX2UIInventory::OpenElChangerResultWindow( std::map< int, int >& mapInsertedItem, std::vector< KInventoryItemInfo >& vecKInventorySlotInfo )
{

	SAFE_DELETE_DIALOG( m_pDLGElChangerResult );

	m_pDLGElChangerResult = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_El_Changer_Result.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGElChangerResult );

	//	m_pDLGResolveItemResult->SetPos( g_pMain->GetWindowPos( D3DXVECTOR2( 403, 350 ), D3DXVECTOR2( 270, 332 ) ) );

	// 	m_NowMousePos.x = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetXPos();
	// 	m_NowMousePos.y = (float)g_pKTDXApp->GetDIManager()->GetMouse()->GetYPos();
	// 	D3DXVECTOR2 DLGPos = g_pKTDXApp->MouseConvertByResolution( m_NowMousePos ) - D3DXVECTOR2(222, 288);		// Left-top pos
	// 	if(DLGPos.x > 578)
	// 	{
	// 		DLGPos.x = 578;
	// 	}
	// 	if(DLGPos.y > 392)
	// 	{
	// 		DLGPos.y = 392;
	// 	}
	// 	m_pDLGElChangerResult->SetPos( DLGPos );
	m_pDLGElChangerResult->SetPos( g_pMain->GetWindowPos( D3DXVECTOR2( 1024-578, 768-392 ), D3DXVECTOR2(222,288) ));

	CKTDGUIButton* pControl = (CKTDGUIButton*) m_pDLGElChangerResult->GetControl( L"Button_OK" );
	if ( pControl != NULL )
	{
		pControl->RequestFocus();
	}

	CKTDGUIStatic* pStaticSlot = (CKTDGUIStatic*)m_pDLGElChangerResult->GetControl( L"Static_Slot_Pic" );
	CKTDGUIStatic* pStaticName = (CKTDGUIStatic*)m_pDLGElChangerResult->GetControl( L"Static_SlotName" );
	CKTDGUIStatic* pStaticNum = (CKTDGUIStatic*)m_pDLGElChangerResult->GetControl( L"Static_SlotNum" );


	//�ϴ� �ʱ�ȭ ���� ��������Ҳ� ���� �Ĵ���
	const int MAGIC_EL_CHANGER_RESULT_MAX_SLOT_LIST_NUM = 6;
	for ( int slotNum = 0; slotNum < MAGIC_EL_CHANGER_RESULT_MAX_SLOT_LIST_NUM; slotNum++ )
	{
		if ( pStaticSlot != NULL && pStaticSlot->GetPicture(slotNum) != NULL )
		{
			pStaticSlot->GetPicture(slotNum)->SetTex( L"DLG_Common_New_Texture02.dds", L"item_slot1" );
		}

		if ( pStaticName != NULL && pStaticName->GetString(slotNum) != NULL )
		{
			pStaticName->GetString(slotNum)->msg = L"";
		}

		if ( pStaticNum != NULL && pStaticNum->GetString(slotNum) != NULL )
		{
			pStaticNum->GetString(slotNum)->msg = L"";
		}
	}

	std::map< int, int >::iterator i;
	int j = 0;
	for ( i = mapInsertedItem.begin(); i != mapInsertedItem.end(); i++, j++ )
	{
		int resultItemID = i->first;

		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
		if ( pItemTemplet != NULL )
		{
			if ( pStaticSlot != NULL && pStaticSlot->GetPicture(j) != NULL )
			{
                const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
				if ( pwszShopImage[0] != NULL )
					pStaticSlot->GetPicture(j)->SetTex( pwszShopImage );
				else
					pStaticSlot->GetPicture(j)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

			if ( pStaticName != NULL && pStaticName->GetString(j) != NULL )
			{
                pStaticName->GetString(j)->msg = pItemTemplet->GetFullName_();
			}

			if ( pStaticNum != NULL && pStaticNum->GetString(j) != NULL )
			{
				wstringstream wstrstm;
				wstrstm  << i->second << GET_STRING( STR_ID_24 );
				pStaticNum->GetString(j)->msg = wstrstm.str().c_str();
			}
		}
		// �������� ���� ���
		else
		{
			pStaticSlot->GetPicture(j)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			pStaticName->GetString(j)->msg = L"";
			pStaticNum->GetString(j)->msg = L"";
		}
	}

	UpdateInventorySlotList( vecKInventorySlotInfo );
}

bool CX2UIInventory::Handler_EGS_IDENTIFY_ITEM_REQ()
{
	KEGS_IDENTIFY_ITEM_REQ kPacket;

	kPacket.m_iItemUID = m_ElChangerItemUID;
	kPacket.m_iQuantity = m_ElChangerItemCount;

	g_pData->GetServerProtocol()->SendPacket( EGS_IDENTIFY_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_IDENTIFY_ITEM_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_IDENTIFY_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_IDENTIFY_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_IDENTIFY_ITEM_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			OpenElChangerResultWindow( kEvent.m_mapInsertedItem, kEvent.m_vecKInventorySlotInfo );
		}

		return true;
	}

	return false;

}

void CX2UIInventory::DrawSlotMouseOverImage()
{
	if ( GetCheckOperationCondition() == false )
	{
		m_pDLGSelectedItem->SetShow( false );
		return;
	}

	bool bCheck = false;

	if ( *m_pSlotBeforeDragging != NULL )
	{
		if ( IsInEquippedItemSlotReact( m_NowMousePos ) == true && (*m_pSlotBeforeDragging)->IsItem() )
		{

			//*m_DraggingItemUID = ((CX2SlotItem*)(*m_pSlotBeforeDragging))->GetItemUID();

			const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();

			// ����ó�� �߰�
			if( kInventory.GetItem( *m_DraggingItemUID, true ) == NULL ||
				kInventory.GetItem( *m_DraggingItemUID, true )->GetItemTemplet() == NULL
                )
				return;

			CX2Item::ITEM_TYPE itemType = kInventory.GetItem( *m_DraggingItemUID, true )->GetItemTemplet()->GetItemType();
			if (  itemType == CX2Item::IT_WEAPON || itemType == CX2Item::IT_DEFENCE || itemType == CX2Item::IT_ACCESSORY )
			{
				CX2Unit::EQIP_POSITION equipPosition = kInventory.GetItem( *m_DraggingItemUID, true )->GetItemTemplet()->GetEqipPosition();

				for ( int i = 0; i < (int)m_SlotList.size(); i++ )
				{
					CX2SlotItem* pItemSlot = (CX2SlotItem*)GetSlot(i);
					if ( pItemSlot->GetSlotType() == CX2Slot::ST_EQUIPPED && pItemSlot->GetEquipPos() == equipPosition &&
						( itemType == CX2Item::IT_ACCESSORY || pItemSlot->GetFashion() == kInventory.GetItem( *m_DraggingItemUID )->GetItemTemplet()->GetFashion() ))
					{
						m_pDLGSelectedItem->SetPos( pItemSlot->GetPos() );
						m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( pItemSlot->GetSize() );

						bCheck = true;
					}
				}
			}
			else
			{
				if ( m_pNowOverItemSlot != NULL )
				{
					m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
					m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

					bCheck = true;
				}
			}
		}
		else
		{
			if ( m_pNowOverItemSlot != NULL )
			{
				m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
				m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

				bCheck = true;
			}
		}
	}
	else
	{
		if ( m_pNowOverItemSlot != NULL )
		{
			m_pDLGSelectedItem->SetPos( m_pNowOverItemSlot->GetPos() );
			m_pDLGSelectedItem->GetStatic_LUA( "SelectedItem" )->GetPicture(0)->SetSize( m_pNowOverItemSlot->GetSize() );

			bCheck = true;
		}
	}

	if ( bCheck == true )
	{
		m_pDLGSelectedItem->SetShow( true );
	}
	else
	{
		m_pDLGSelectedItem->SetShow( false );
	}

}

wstring CX2UIInventory::GetSlotItemDesc()
{

	wstring itemDesc=  L"";

	if ( m_pNowOverItemSlot != NULL )
	{
		if ( m_pNowOverItemSlot->IsResetItemUI() == false )
		{
			itemDesc = m_pNowOverItemSlot->GetSlotDesc();
		}
		else
		{
			itemDesc = GetSlotItemDescByUID( m_pNowOverItemSlot->GetItemUID() );
			//���콺 ���� �� NewItem ���̶���Ʈ �������� ����

			if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->AccessInventory();
				{
					if( true == kInventory.EraseNewItem(m_pNowOverItemSlot->GetItemUID()) )
					{
						SetInventorySort(m_NowInventorySortType, GetNowInvenSortTypePageNum());
					}
				}
			}
		}
	}

	return itemDesc;
}

bool CX2UIInventory::Handler_EGS_USE_ITEM_IN_INVENTORY_REQ(UidType iUid, UidType iTempCode, int iItemID)
{
	KEGS_USE_ITEM_IN_INVENTORY_REQ kPacket;
	kPacket.m_iItemUID = iUid;

	switch( iItemID )
	{
	case PROTECTION_OF_NUT_ITEM_ID:
	case PROTECTION_OF_PUTA_ITEM_ID:
	case PROTECTION_OF_LAHELL_ITEM_ID:
	case PROTECTION_OF_JAKIELL_ITEM_ID:
	case BELSSING_OF_SERAPHIM_ITEM_ID:
	case BELSSING_OF_GEV_ITEM_ID:
	case BELSSING_OF_AMON_ITEM_ID:
	case BELSSING_OF_CRONOS_ITEM_ID:

	case ELIXIR_GIANT_POTION:
	case ELIXIR_BLAZING_BOMB:
	case ELIXIR_SPIRIT_OF_CHASER:
	case ELIXIR_CRADLE_OF_LITTLE_FAIRY:
	case ELIXIR_ICE_BALL_OF_DENIPH:
	case ELIXIR_FEATHER_OF_VENTUS:
	case ELIXIR_FLAME_RING_OF_ROSSO:
	case ELIXIR_BIG_HAND_POTION:
	case BUFF_RURIEL_MANA_ENERGIZE_POTION:
	case RIDINGPET_STAMINA_30_PER_ITEM_ID:
		{
			//���, ���� ���� �� Ŭ���� �� ������ ���Ǵ� �� ������
			kPacket.m_iTempCode = iTempCode;
		}break;
	default:
		{
			if( iTempCode > 0 )
				kPacket.m_iTempCode = iTempCode;
		}break;
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_USE_ITEM_IN_INVENTORY_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_USE_ITEM_IN_INVENTORY_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_USE_ITEM_IN_INVENTORY_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	// ��Ŷ ó��
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_USE_ITEM_IN_INVENTORY_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_USE_ITEM_IN_INVENTORY_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED		= kEvent.m_iED;
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

			UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

			if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
			{
				g_pData->GetUIManager()->GetUICharInfo()->ResetEDnCashnVP();
			}
			SetInventorySort(m_NowInventorySortType, GetNowInvenSortTypePageNum());

			// ������ ���� �̵� �˻�
			if( kEvent.m_iWarpPointMapID > 0 && g_pData->GetLocationManager()->IsValidWarpField(kEvent.m_iWarpPointMapID) )
			{
				m_iCurrentVillageWarpIndex = -1;
				CX2StateMenu* pStateMenu = (CX2StateMenu*) g_pMain->GetNowState();
				if( NULL != pStateMenu )
				{
					pStateMenu->SetCurrentVillageWarpIndex( -1 );
				}

				if(g_pMain->GetNowStateID() == CX2Main::XS_SQUARE_GAME)
				{
					// �ŷ����忡�� ����
					g_pData->SetStateArg(kEvent.m_iWarpPointMapID);
					CX2StateSquareGame* pState = (CX2StateSquareGame*) g_pMain->GetNowState();
					pState->LeaveSquareGame();
				}
				else
				{
					// �̵�Ƽ���� �̿��Ͽ� ���� �̵�
					g_pData->SetStateArg(kEvent.m_iWarpPointMapID);
					CX2State* pState = (CX2State*) g_pMain->GetNowState();
					pState->Handler_EGS_STATE_CHANGE_FIELD_REQ(kEvent.m_iWarpPointMapID);
				}
			}

			if ( kEvent.m_iUsedItemID == GUILD_EXPANSION_ITEM_ID
				|| kEvent.m_iUsedItemID == EVENT_GUILD_EXPANSION_ITEM_ID
				)
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4593 ), g_pMain->GetNowState() );
			}

			if( g_pData != NULL && kEvent.m_iUsedItemID == SKILL_NOTE_ITEM_ID )
			{
				if( g_pData->GetUIManager() != NULL && g_pData->GetUIManager()->GetUIInventory() != NULL &&
					g_pData->GetUIManager()->GetUIInventory()->GetShow() == true )
				{
					g_pData->GetUIManager()->GetUIInventory()->UpdateUIButton();
				}

				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_5059 ), g_pMain->GetNowState() );
			}

			if( kEvent.m_iUsedItemID == ACTIVATION_DROP_ITEM_PICKUP_SKILL
				)
			{
				SetPetAutoLooting( kEvent.m_iTempCode );
			}

			return true;
		}
	}

	return false;
}


bool CX2UIInventory::CheckUseItem(CX2Item *pItem)
{
//#ifdef TITLE_SYSTEM
	for(int i=0; i<std::ssize(TITLE_ITEM); ++i)
	{
        if( pItem->GetItemTemplet()->GetItemID() == TITLE_ITEM[i] )
		{
			if(g_pData != NULL && g_pData->GetTitleManager() != NULL && g_pData->GetTitleManager()->GetTakeTitle( TITLE_ID[i] ) == true)
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_813 ), g_pMain->GetNowState() );

				return false;
			}
		}
	}
//#endif

	return true;
}



//{{ 2007. 10. 8  ������
bool CX2UIInventory::Handler_EGS_OPEN_RANDOM_ITEM_REQ()
{
	if ( m_iRandomItemUID == 0 )
		return false;
	/*
	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
	for ( int i = CX2Inventory::ST_EQUIP; i <= CX2Inventory::ST_AVARTA; i++ )
	{
	if ( kInventory.GetUsedSlotNum( (CX2Inventory::SORT_TYPE)i ) >= kInventory.GetItemMaxNum( (CX2Inventory::SORT_TYPE)i ) )
	{
	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_814 ), g_pMain->GetNowState() );
	return false;
	}
	}
	*/

	//OPEN_RANDOM_ITEM_REQ�� �̹� ���� ��� �ٽ� ó������ �ʵ���
	if( true == m_bProcessCubeOpen )
		return false;

	m_bProcessCubeOpen = true;

	KEGS_OPEN_RANDOM_ITEM_REQ kEGS_OPEN_RANDOM_ITEM_REQ;

	kEGS_OPEN_RANDOM_ITEM_REQ.m_iItemUID  = m_iRandomItemUID;

	if( true == m_bUsingRandomItemAutoOpen )
	{
		kEGS_OPEN_RANDOM_ITEM_REQ.m_bNotifyGetItem = false;
	}
	else
		kEGS_OPEN_RANDOM_ITEM_REQ.m_bNotifyGetItem = true;

	g_pData->GetServerProtocol()->SendPacket( EGS_OPEN_RANDOM_ITEM_REQ, kEGS_OPEN_RANDOM_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_OPEN_RANDOM_ITEM_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_OPEN_RANDOM_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_OPEN_RANDOM_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( m_pDLGRandomItem != NULL )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRandomItem, NULL, false );
		m_pDLGRandomItem = NULL;
	}

	m_bProcessCubeOpen = false;

	g_pMain->DeleteServerPacket( EGS_OPEN_RANDOM_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
				g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_ED = kEvent.m_iED;

			m_TempOpenRandomItemAck = kEvent;

			//{{ 2009.01.20 ���¿�
			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
			{
				if(m_bUsingRandomItemAutoOpen)	// �ڵ� ������ ������̸�
				{
					if(m_pDLGAutoOpenMsgBox == NULL)
					{
						m_pDLGAutoOpenMsgBox = g_pMain->KTDGUIMsgBox( D3DXVECTOR2(250,300), GET_STRING( STR_ID_815 ), m_pNowState );
					}
					// ���� �������� �����ϰ� �κ��丮 �������� �� �ٷ� ������ Uid�� ���ؼ� Open�� ��û
					UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );

					for(std::map<int, int>::iterator i = kEvent.m_mapInsertedItem.begin(); i != kEvent.m_mapInsertedItem.end(); i++)
					{
						std::map<int, int>::iterator target = m_MapAutoRandomItemGet.find(i->first);
						if(target != m_MapAutoRandomItemGet.end())
						{
							// �̹� ����� ���� �ִ� �������̸� ������ ��������
							target->second = target->second + i->second;
						}
						else
						{
							// ���� ���� �������̸� ���� ����
							m_MapAutoRandomItemGet.insert( std::make_pair(i->first, i->second));
						}
					}

					m_OpenedRandomItemCount++;
					Handler_EGS_OPEN_RANDOM_ITEM_REQ();
					return true;
				}
			}

			switch(m_eRandomItemEventType)
			{
			case RIOET_ICE_HEATER:
			case RIOET_MINI_ICE_HEATER:
			case RIOET_ALL_IN_ONE_ICE_HEATER:
			case RIOET_RIDING_PET_GACHA:
			case RIOET_GOLD_ICE_HEATER:
			case RIOET_HALLOWEEN_ICE_HEATER:
				{
					/// ��� �����⸦ �Ѱ����� ó��
					PlayGetAttractionItem( m_eRandomItemEventType );
				} break;

			case RIOET_APRIL_5TH_TREE_DAY_EVENT:
				{

					// ���� : ���̾�α� �ΰ��� ������� ���� �Ǵϱ� ���⼭ ����������
					m_bPlayingTreedayEvent = true;
					SAFE_DELETE_DIALOG(m_pDLGTreeDay1);
					SAFE_DELETE_DIALOG(m_pDLGTreeDay2);

					m_pDLGTreeDay1 = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Arbor_Day_Event01.lua", 0.0f );
					g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGTreeDay1 );
					m_pDLGTreeDay1->SetColor(D3DXCOLOR(1,1,1,0));

					m_pDLGTreeDay2 = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Arbor_Day_Event02.lua", 0.0f );
					g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGTreeDay2 );
					m_pDLGTreeDay2->SetColor(D3DXCOLOR(1,1,1,0));

					g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGTreeDay1, true );
					m_pDLGTreeDay1->Move( m_pDLGTreeDay1->GetPos(), D3DXCOLOR(1,1,1,1), 0.5f, true );	// ���� 0.5�ʰ� ������� (0~0.5)
					m_pDLGTreeDay1->Move( m_pDLGTreeDay1->GetPos(), D3DXCOLOR(1,1,1,1), 0.7f );			// 0.7�� ���� �� (0.5~1.2)
					m_pDLGTreeDay1->Move( m_pDLGTreeDay1->GetPos(), D3DXCOLOR(1,1,1,0), 0.5f );			// 0.5�ʰ� ������� (1.2~1.5)

					g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGTreeDay2, true );
					m_pDLGTreeDay2->Move( m_pDLGTreeDay2->GetPos(), D3DXCOLOR(1,1,1,0), 1.2f, true );	// ���� 1.2�ʰ��� ������ �ְ�
					m_pDLGTreeDay2->Move( m_pDLGTreeDay2->GetPos(), D3DXCOLOR(1,1,1,1), 0.5f );			// 1���� ����� ���� ��Ÿ���� (1.2~1.7)
					m_pDLGTreeDay2->Move( m_pDLGTreeDay2->GetPos(), D3DXCOLOR(1,1,1,1), 1.0f );			// 1�� ���� �� �� (1.7~2.7)

					// �ð��� ���缭 ���� ���ֱ� ����
					m_bAprilSoundPlayed = false;
					m_fPlayTime = 0.0f;
					//g_pKTDXApp->GetDeviceManager()->PlaySound( L"TreeDay_Event.ogg", false, false );

				} break;

			case RIOET_EVENT_CUBE:
				{
					g_pData->GetCubeOpenImageManager()->SetPlaying( true );
					m_fPlayTime = 0.0f;
					SAFE_DELETE_DIALOG(m_pDLGEventCubeResultImage);

					if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
					{
						std::wstring wstrResultImageNamge;

						std::map<int, int>::iterator mit = m_TempOpenRandomItemAck.m_mapInsertedItem.begin();
						while(mit != m_TempOpenRandomItemAck.m_mapInsertedItem.end())
						{
							// ����� ��¦�̴µ� ����ϱ� ���ؼ� �Ŵ����� ����� ID�� ��Ƶ���
							// ������� ��������?
							g_pData->GetCubeOpenImageManager()->AddResultItemID(mit->first);

							if( g_pData->GetCubeOpenImageManager()->GetResultImageName(m_RandomItemID, mit->first, wstrResultImageNamge) == true )
							{
								// ��� ť���!
								g_pData->GetCubeOpenImageManager()->SetNowResultEventCubePlaying( true );

								// ��� ť�� ��� �߿��� esc �ȸ����� �ؾ� �Ѵ�...
								m_pDLGEventCubeResultImage = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_Event_Cube_Result.lua", 0.0f  );
								CKTDGUIStatic* pStaticCubeOpenLoading = (CKTDGUIStatic*) m_pDLGEventCubeResultImage->GetControl( L"Event_Cube_Result" );
								CKTDGUIControl::CPictureData *pPicture = pStaticCubeOpenLoading->GetPicture(0);
								pPicture->SetTex( wstrResultImageNamge.c_str() , L"Note");

								g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGEventCubeResultImage );
								m_pDLGEventCubeResultImage->SetColor(D3DXCOLOR(1,1,1,0));

								g_pKTDXApp->GetDGManager()->GetDialogManager()->ChangeSequence( m_pDLGEventCubeResultImage, true );
								m_pDLGEventCubeResultImage->Move( m_pDLGEventCubeResultImage->GetPos(), D3DXCOLOR(1,1,1,1), 0.5f, true );	// ���� 0.5�ʰ� ������� (0~0.5)
								m_pDLGEventCubeResultImage->Move( m_pDLGEventCubeResultImage->GetPos(), D3DXCOLOR(1,1,1,1), 1.5f );			// 1.5�� ���� �� (0.5~1.2)
								m_pDLGEventCubeResultImage->Move( m_pDLGEventCubeResultImage->GetPos(), D3DXCOLOR(1,1,1,0), 0.5f );			// 0.5�ʰ� ������� (1.2~1.5)
								break;
							}
							else
							{
								dbg::cerr << L"ť�� �̹���, ��� �̹��� �̸��� ����?? ��� ID : " << mit->second << dbg::endl;
							}
							++mit;
						}
					}
				} break;


			case RIOET_MONSTER_CARD_SET:
				{
					UpdateOpenMonsterCardSet();
				} break;

			default:
			case RIOET_NONE:
				{
					//PlayGetRandomItem();
					UpdateOpenRandomItemAckNew();
				}break;
			}
			return true;
		}
		else
		{
			//{{ 2009.01.20 ���¿�
			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
			{
				if(m_bUsingRandomItemAutoOpen)	// �ڵ� ������ ������̸�
				{
					// �ڵ� ������ �������ϴ�~
					if(m_pDLGAutoOpenMsgBox != NULL)
					{
						SAFE_DELETE_DIALOG(m_pDLGAutoOpenMsgBox);
						m_pDLGAutoOpenMsgBox = NULL;
					}

					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), GET_REPLACED_STRING( ( STR_ID_816, "i", m_OpenedRandomItemCount ) ), m_pNowState );

					UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );
					// ���� : OpenDecompositionResultWindow �ȿ��� �κ��丮 ������ �� �ϱ� ���� �� ���� =3=
					std::vector<KInventoryItemInfo> vecDummyInfo;
					vecDummyInfo.clear();
					//OpenDecompositionResultWindow(m_MapAutoRandomItemGet, vecDummyInfo, false);
					OpenResolveResultWindow( m_MapAutoRandomItemGet, vecDummyInfo, false );
					// �� �� ������ ġ��ô�
					m_bUsingRandomItemAutoOpen = false;
					m_OpenedRandomItemCount = 0;
					m_MapAutoRandomItemGet.clear();
				}

			}

			// ���н� â�� ������~
			if( m_hMeshInstMeltMachine != INVALID_MESH_INSTANCE_HANDLE )
			{
				g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachine );

				if( NULL != m_pDLGOpenAttraction )
				{
					m_pDLGOpenAttraction->SetHasUnit( NULL );
				}
			}

			if ( m_pDLGOpenAttraction != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOpenAttraction, NULL, false );
				m_pDLGOpenAttraction = NULL;
			}
			m_eRandomItemEventType = RIOET_NONE;
		}
	}

	return false;
}
//}}


void CX2UIInventory::UpdateOpenRandomItemAckNew()
{



	m_eRandomItemEventType = RIOET_NONE;

	wstring itemName = L"";
	wstring acquireComment;

	bool bCheck = false;

	if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
	{
		for(std::map<int, int>::iterator i = m_TempOpenRandomItemAck.m_mapInsertedItem.begin(); i != m_TempOpenRandomItemAck.m_mapInsertedItem.end(); i++)
		{
			std::map<int, int>::iterator target = m_mapCubeItemGet.find(i->first);
			if(target != m_mapCubeItemGet.end())
			{
				// �̹� ����� ���� �ִ� �������̸� ������ ��������
				target->second = target->second + i->second;
			}
			else
			{
				// ���� ���� �������̸� ���� ����
				m_mapCubeItemGet.insert( std::make_pair(i->first, i->second));
			}
		}

		bCheck = true;
	}

	if ( m_TempOpenRandomItemAck.m_iRessurectionCount != 0 )
	{
		wstringstream tempString;
		int getStoneNum = m_TempOpenRandomItemAck.m_iRessurectionCount - g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum();

		m_mapCubeItemGet.insert( std::make_pair(127030, getStoneNum));


		g_pData->GetMyUser()->GetSelectUnit()->SetResurrectionStoneNum( m_TempOpenRandomItemAck.m_iRessurectionCount );

		//{{ kimhc // 2009-07-21 // ���� ������ ��Ȱ�� ť�� ���ÿ� ����UI(���� ���)�� ��Ȱ�� ������ ���� �ȵǴ� ����
		if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
		{
			g_pX2Game->ResetReBirthStoneNumUI();
		}

		//}} kimhc // 2009-07-21 // ���� ������ ��Ȱ�� ť�� ���ÿ� ����UI(���� ���)�� ��Ȱ�� ������ ���� �ȵǴ� ����

		if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
		{
			g_pData->GetUIManager()->GetUICharInfo()->ResetResurrectionStoneUI();
		}
		bCheck = true;
	}




	if ( bCheck == true )
	{

		// ���� : OpenDecompositionResultWindow �ȿ��� �κ��丮 ������ �� �ϱ� ���� �� ���� =3=
		std::vector<KInventoryItemInfo> vecDummyInfo;
		vecDummyInfo.clear();
		//OpenDecompositionResultWindow(m_MapAutoRandomItemGet, vecDummyInfo, false);

		OpenResolveResultWindow( m_mapCubeItemGet, vecDummyInfo, false );
		m_mapCubeItemGet.clear();

	}
	else if ( m_TempOpenRandomItemAck.m_iRestoreSpirit != 0 )
	{
		wstringstream tempString;
		float fGetSpirit = static_cast<float>( m_TempOpenRandomItemAck.m_iRestoreSpirit - g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpirit );
		fGetSpirit = fGetSpirit / static_cast<float>( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpiritMax ) * 100.0f;
		fGetSpirit = floor(fGetSpirit * 10.0f + 0.5f) / 10.0f;
		tempString << GET_REPLACED_STRING( ( STR_ID_819, "f", fGetSpirit ) );

		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_iSpirit = m_TempOpenRandomItemAck.m_iRestoreSpirit;
		// �ټ��� ������� UI ������ StateMenu���� ó���ȴ�

		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), tempString.str().c_str(), g_pMain->GetNowState() );
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_820 ), g_pMain->GetNowState() );
	}


	UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );

	if(m_pDLGCubeCheck != NULL)
	{
		CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGCubeCheck->GetControl( L"ButtonCube_Window_Complete" );
		m_pDLGCubeCheck->RequestFocus(pButton);
	}
}

void CX2UIInventory::UpdateOpenRandomItemAck()
{
	m_eRandomItemEventType = RIOET_NONE;

	wstring itemName = L"";
	wstring acquireComment;

	bool bCheck = false;

	if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
	{
		std::map< int, int >::iterator i;

		int nCount = 0;
		wstring getExplanation = L"";
		for ( i = m_TempOpenRandomItemAck.m_mapInsertedItem.begin(); i != m_TempOpenRandomItemAck.m_mapInsertedItem.end(); i++ )
		{
			int resultItemID = i->first;

			//{{ oasis907 : ����� [2010.4.27] // ť�� ������ ���â ���� ǥ��
			int resultNum = i->second;
			//}}

			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
			if ( pItemTemplet != NULL )
				itemName = pItemTemplet->GetFullName_();

			if ( nCount > 0 )
			{
				getExplanation += L", ";
			}

			getExplanation += itemName;

			//{{ oasis907 : ����� [2010.4.27] // ť�� ������ ���â ���� ǥ��
			getExplanation += L" ";

			WCHAR buff[256] = {0};
			_itow_s( resultNum, buff, 10 );
			getExplanation += buff;

			getExplanation += GET_STRING( STR_ID_24 );
			//}}

			nCount++;
		}

		acquireComment = GET_REPLACED_STRING( ( STR_ID_817, "LI", getExplanation, KHanSungPostWordUnicodeWrapper( (WCHAR*)getExplanation.c_str(), STR_ID_198, STR_ID_199 ) ) );
		bCheck = true;
	}

	if ( m_TempOpenRandomItemAck.m_iRessurectionCount != 0 )
	{
		wstringstream tempString;
		int getStoneNum = m_TempOpenRandomItemAck.m_iRessurectionCount - g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum();
		tempString << GET_REPLACED_STRING( ( STR_ID_818, "i", getStoneNum ) );

		g_pData->GetMyUser()->GetSelectUnit()->SetResurrectionStoneNum( m_TempOpenRandomItemAck.m_iRessurectionCount );

		//{{ kimhc // 2009-07-21 // ���� ������ ��Ȱ�� ť�� ���ÿ� ����UI(���� ���)�� ��Ȱ�� ������ ���� �ȵǴ� ����
		if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
		{
			g_pX2Game->ResetReBirthStoneNumUI();
		}

		//}} kimhc // 2009-07-21 // ���� ������ ��Ȱ�� ť�� ���ÿ� ����UI(���� ���)�� ��Ȱ�� ������ ���� �ȵǴ� ����

		if(g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_CHARINFO))
		{
			g_pData->GetUIManager()->GetUICharInfo()->ResetResurrectionStoneUI();
		}

		acquireComment = tempString.str().c_str();
		bCheck = true;
	}




	if ( bCheck == true )
	{
		//g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), tempString.str().c_str(), g_pMain->GetNowState() );
		SAFE_DELETE_DIALOG( m_pDLGCubeCheck );
		m_pDLGCubeCheck = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cube_Result_Window.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCubeCheck );
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDLGCubeCheck->GetControl( L"StaticCube_Window_Text" );
		if ( pStatic != NULL && pStatic->GetString(0) != NULL )
		{
			wstring tpString = CWordLineHandler::GetStrByLineBreakInX2Main(acquireComment.c_str(), 366, XUF_DODUM_15_BOLD );
			pStatic->GetString(0)->msg = tpString;
		}

		if ( m_RandomItemID == 129755 )
		{
			CKTDGUIStatic* pStaticCube = (CKTDGUIStatic*)m_pDLGCubeCheck->GetControl( L"g_pStaticCube_Image" );
			if ( pStaticCube != NULL )
			{
				if ( pStaticCube->GetPicture(0) != NULL )
				{
					pStaticCube->GetPicture(0)->SetTex( L"DLG_Common_New_Texture62.tga", L"CUBE_EVENT_MILITARY" );
				}
			}
		}
	}
	else if ( m_TempOpenRandomItemAck.m_iRestoreSpirit != 0 )
	{
		wstringstream tempString;
		float fGetSpirit = static_cast<float>( m_TempOpenRandomItemAck.m_iRestoreSpirit - g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpirit );
		fGetSpirit = fGetSpirit / static_cast<float>( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_iSpiritMax ) * 100.0f;
		fGetSpirit = floor(fGetSpirit * 10.0f + 0.5f) / 10.0f;
		tempString << GET_REPLACED_STRING( ( STR_ID_819, "f", fGetSpirit ) );

		g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().m_iSpirit = m_TempOpenRandomItemAck.m_iRestoreSpirit;
		// �ټ��� ������� UI ������ StateMenu���� ó���ȴ�

		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), tempString.str().c_str(), g_pMain->GetNowState() );
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_820 ), g_pMain->GetNowState() );
	}


	UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );

	if(m_pDLGCubeCheck != NULL)
	{
		CKTDGUIButton* pButton = (CKTDGUIButton*)m_pDLGCubeCheck->GetControl( L"ButtonCube_Window_Complete" );
		m_pDLGCubeCheck->RequestFocus(pButton);
	}
}

void CX2UIInventory::UpdateOpenMonsterCardSet()
{
	m_eRandomItemEventType = RIOET_NONE;

	wstring itemName = L"";
	wstring acquireComment;

	if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
	{
		std::map< int, int >::iterator i;

		int nCount = 0;
		wstring getExplanation = L"";
		for ( i = m_TempOpenRandomItemAck.m_mapInsertedItem.begin(); i != m_TempOpenRandomItemAck.m_mapInsertedItem.end(); i++ )
		{
			int resultItemID = i->first;

			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
			if ( pItemTemplet != NULL )
				itemName = pItemTemplet->GetFullName_();

			if ( nCount > 0 )
			{
				getExplanation += L", ";
			}

			getExplanation += itemName;

			nCount++;
		}

		acquireComment = GET_REPLACED_STRING( ( STR_ID_817, "LI", getExplanation, KHanSungPostWordUnicodeWrapper( (WCHAR*)getExplanation.c_str(), STR_ID_198, STR_ID_199 ) ) );

		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), acquireComment.c_str(), g_pMain->GetNowState() );

		UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );
	}
}

void CX2UIInventory::OpenResolveWindow( CX2SlotItem* pSlot )
{
	// 	if ( pSlot->GetSlotType() == CX2Slot::ST_EQUIPPED )
	// 	{
	// 		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
	// 			L"�������� �������� ������ �� �����ϴ�.", g_pMain->GetNowState() );
	//
	// 		return;
	// 	}

	if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
		g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_SELL )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_2650 ), g_pMain->GetNowState() );
		return;
	}

	if ( pSlot->GetSlotType() == CX2Slot::ST_INVENTORY )
	{
		CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( pSlot->GetItemUID() );


		if ( pItem == NULL ||
            pItem->GetItemTemplet() == NULL
            )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
				GET_STRING( STR_ID_821 ), g_pMain->GetNowState() );

			return;
		}

		//{{ kimhc // 2010-01-06 // PC�� �����̾� ����
		if ( pItem->GetItemTemplet()->GetIsPcBang() == true )
		{
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
			return;
		}
		//}} kimhc // 2010-01-06 // PC�� �����̾� ����

		//{{ kimhc // 2009-09-08 // ���ε� ������ ���� �Ұ�

		if ( pItem->GetItemData().m_bIsSealed == true )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_4477 ), g_pMain->GetNowState() );
			return;
		}
		//}} kimhc // 2009-09-08 // ���ε� ������ ���� �Ұ�

		switch(pItem->GetItemTemplet()->GetItemType())
		{
		case CX2Item::IT_WEAPON:
		case CX2Item::IT_DEFENCE:
			{
				if ( pItem->GetItemTemplet()->GetFashion() == true )
				{
					if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_INFINITY &&
						pItem->GetItemData().m_Period > 0 )
					{
						//�Ⱓ���� �Ǹ�.
						g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
							GET_STRING( STR_ID_822 ), g_pMain->GetNowState() );

						return;
					}
				}
			} break;
		case CX2Item::IT_ACCESSORY:
			{
				if ( pItem->GetItemTemplet()->GetFashion() != true )
				{
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
						GET_STRING( STR_ID_823 ), g_pMain->GetNowState() );

					return;
				}

				if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_INFINITY &&
					pItem->GetItemData().m_Period > 0 )
				{
					//�Ⱓ���� �Ǹ�.
					g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
						GET_STRING( STR_ID_822 ), g_pMain->GetNowState() );

					return;
				}

			} break;
		default:
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
					GET_STRING( STR_ID_823 ), g_pMain->GetNowState() );

				return;
			}
		}



		if ( pItem == NULL )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
				GET_STRING( STR_ID_821 ), g_pMain->GetNowState() );

			return;
		}

		if(true == pItem->IsDisabled())
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_800 ), g_pMain->GetNowState() );
			return;
		}

		if ( pItem != NULL )
		{
			m_DecompositionItemUID = pItem->GetUID();
			if ( m_pDLGResolveItem != NULL )
			{
				g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGResolveItem, NULL, false );
			}

			m_pDLGResolveItem = new CKTDGUIDialog(g_pMain->GetNowState(), L"DLG_UI_Item_Apart.lua");
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGResolveItem );

			D3DXVECTOR2 DLGPos = g_pMain->GetWindowPos( D3DXVECTOR2(357,243), D3DXVECTOR2(184,160) );
			m_pDLGResolveItem->SetPos( DLGPos );

			CKTDGUIStatic* pStaticSlot = (CKTDGUIStatic*)m_pDLGResolveItem->GetControl( L"Static_Broken_Item_Slot" );
			if ( pStaticSlot != NULL && pStaticSlot->GetPicture(0) != NULL )
			{
                const wchar_t* pwszShopImage = pItem->GetItemTemplet()->GetShopImage();
				if ( pwszShopImage[0] != NULL )
					pStaticSlot->GetPicture(0)->SetTex( pwszShopImage );
				else
					pStaticSlot->GetPicture(0)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

			CKTDGUIControl* pControlButton = m_pDLGResolveItem->GetControl( L"Button_OK" );
			if ( pControlButton != NULL )
				pControlButton->RequestFocus();
		}
	}

}


bool CX2UIInventory::Handler_EGS_RESOLVE_ITEM_REQ()
{
	KEGS_RESOLVE_ITEM_REQ kPacket;

	kPacket.m_iItemUID  = m_DecompositionItemUID;

	g_pData->GetServerProtocol()->SendPacket( EGS_RESOLVE_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESOLVE_ITEM_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_RESOLVE_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESOLVE_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_RESOLVE_ITEM_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{

			OpenResolveResultWindow( kEvent.m_mapInsertedItem, kEvent.m_vecKInventorySlotInfo, kEvent.m_bJackpot );

			return true;
		}
	}

	return false;
}


void CX2UIInventory::OpenResolveResultWindow( std::map< int, int >& mapInsertedItem,
											 std::vector< KInventoryItemInfo >& vecKInventorySlotInfo,
											 bool /*bJackpot*/ )
{
	SAFE_DELETE_DIALOG( m_pDLGResolveItemResult );

	m_DecompositionWindowNowPageNum = 1;
	m_mapResolveResultInsertedItem = mapInsertedItem;

	switch( m_eItemObtainResultType )
	{
	case IORT_CUBE_OPEN:
		{
			if( g_pData->GetCubeOpenImageManager() != NULL && g_pData->GetCubeOpenImageManager()->IsShowResult(m_RandomItemID) == false)
			{
				UpdateInventorySlotList( vecKInventorySlotInfo );
				return;
			}
			if( g_pData->GetCubeOpenImageManager() != NULL && g_pData->GetCubeOpenImageManager()->IsShowResultCustom(m_RandomItemID) == true )
			{
				InitItemObtainResult( L"DLG_UI_Event_Cube_Result_Custom.lua", mapInsertedItem);
			}
			else
			InitItemObtainResult( L"DLG_UI_Cube_Result.lua", mapInsertedItem);
		}
		break;
	case IORT_RANDOM_EXCHANGE:
		{
			InitItemObtainResult( L"DLG_UI_Random_Exchange_Result.lua", mapInsertedItem);
		}
		break;
	case IORT_ITEM_RESOLVE:
		{
			InitItemResolveResult( L"DLG_UI_Apart_Result.lua", mapInsertedItem);
		}
		break;
	default:
		{
			//�������� ȹ���� Ÿ���� �����Ǿ� ���� �ʴٸ� ���â ��������ʰ� return;
			return;
		}
		break;
	}

	//������ ���
	int maxPage = (int)m_mapResolveResultInsertedItem.size() / RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM;
	if ( (int)m_mapResolveResultInsertedItem.size() % RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM != 0 )
	{
		maxPage += 1;
	}

	wstring wstrPage = GET_REPLACED_STRING( (STR_ID_16214, "ii", m_DecompositionWindowNowPageNum, maxPage) );

	CKTDGUIStatic* pStaticPageNum = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl(L"Static_Page_Number");
	if ( pStaticPageNum != NULL && pStaticPageNum->GetString(0) != NULL )
	{
		pStaticPageNum->GetString(0)->msg = wstrPage.c_str();
	}

	if( false == vecKInventorySlotInfo.empty() )
	{
		UpdateInventorySlotList( vecKInventorySlotInfo );
	}

}


void CX2UIInventory::ChangeResolveResultPage( bool bNextPage )
{
	//�Žñ� â�� ����� �ִ� ��쿡��..
	if ( m_pDLGResolveItemResult != NULL )
	{
		if ( bNextPage == false )
		{
			if ( m_DecompositionWindowNowPageNum <= 1 )
				return;

			m_DecompositionWindowNowPageNum--;
		}

		int maxPage = (int)m_mapResolveResultInsertedItem.size() / RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM;
		if ( (int)m_mapResolveResultInsertedItem.size() % RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM != 0 )
		{
			maxPage += 1;
		}

		if ( bNextPage == true )
		{
			if ( m_DecompositionWindowNowPageNum >= maxPage )
				return;

			m_DecompositionWindowNowPageNum++;
		}

		wstring wstrPage = GET_REPLACED_STRING( (STR_ID_16214, "ii", m_DecompositionWindowNowPageNum, maxPage) );
		CKTDGUIStatic* pStaticPageNum = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl(L"Static_Page_Number");
		if ( pStaticPageNum != NULL && pStaticPageNum->GetString(0) != NULL )
		{
			pStaticPageNum->GetString(0)->msg = wstrPage.c_str();
		}

		//����ؾ��� ������ �������� iterator�� ����( ����:it �� :maxiter )
		std::map< int, int >::iterator it = m_mapResolveResultInsertedItem.begin();
		for ( int nextNum = 0; nextNum < (m_DecompositionWindowNowPageNum - 1) * RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM; ++nextNum )
		{
			it++;
		}

		std::map< int, int >::iterator maxiter = m_mapResolveResultInsertedItem.begin();
		for ( int nextNum = 0; nextNum < ( RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM*m_DecompositionWindowNowPageNum ); nextNum++ )
		{
			maxiter++;
		}

		CKTDGUIStatic* pStaticSlot = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_Slot_Pic" );
		CKTDGUIStatic* pStaticName = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_SlotName" );
		CKTDGUIStatic* pStaticNum = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_SlotNum" );

		switch( m_eItemObtainResultType )
		{
		case IORT_CUBE_OPEN:
		case IORT_RANDOM_EXCHANGE:
			{
				//������ ���� �ʱ�ȭ
				for ( int slotNum = 0; slotNum < RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM; ++slotNum )
				{
					if ( pStaticSlot != NULL && pStaticSlot->GetPicture(slotNum) != NULL )
					{
						pStaticSlot->GetPicture(slotNum)->SetTex( L"DLG_Common_New_Texture02.dds", L"item_slot1" );
					}

					pStaticName->GetString(slotNum*2)->msg = L"";
					pStaticName->GetString(slotNum*2 + 1)->msg = L"";

					if ( pStaticNum != NULL && pStaticNum->GetString(slotNum) != NULL )
					{
						pStaticNum->GetString(slotNum)->msg = L"";
					}
				}
				//�ش� ���������� ���� ����
				int index = 0;
				int resultItemID = -1;
				while( it != m_mapResolveResultInsertedItem.end() && it != maxiter )
				{
					resultItemID = it->first;

                    const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
                    if ( pItemTemplet != NULL )
					{
						if ( pStaticSlot != NULL && pStaticSlot->GetPicture(index) != NULL )
						{
                            const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
							if ( pwszShopImage[0] != NULL )
								pStaticSlot->GetPicture(index)->SetTex( pwszShopImage );
							else
								pStaticSlot->GetPicture(index)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
						}

						bool bCheckTwoLine = false;
						wstring wstrFullName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetFullName_(), 145, XUF_DODUM_13_SEMIBOLD );
						if ( wstrFullName.find( L"\n") != -1 )
							bCheckTwoLine = true;

						if(pStaticName->GetString(index*2) != NULL)
						{
							if ( bCheckTwoLine == false )
							{
								pStaticName->GetString(index*2)->msg = wstrFullName;
								pStaticName->GetString(index*2+1)->msg = L"";
							}
							else
							{
								pStaticName->GetString(index*2)->msg = L"";
								pStaticName->GetString(index*2+1)->msg = wstrFullName;
							}
						}

						if ( pStaticNum != NULL && pStaticNum->GetString(index) != NULL )
						{
							wstring wstrCount = GET_REPLACED_STRING( ( STR_ID_865, "i", it->second) );
							pStaticNum->GetString(index)->msg = wstrCount.c_str();
						}
					}
					++it;
					++index;
				}
			}
			break;
		case IORT_ITEM_RESOLVE:
			{
				// ������ ���� �ʱ�ȭ
				for ( int slotNum = 0; slotNum < RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM; ++slotNum )
				{
					if ( pStaticSlot != NULL && pStaticSlot->GetPicture(slotNum) != NULL )
					{
						pStaticSlot->GetPicture(slotNum)->SetTex( L"DLG_Common_New_Texture02.dds", L"item_slot1" );
					}

					if ( pStaticName != NULL && pStaticName->GetString(slotNum) != NULL )
					{
						pStaticName->GetString(slotNum)->msg = L"";
					}

					if ( pStaticNum != NULL && pStaticNum->GetString(slotNum) != NULL )
					{
						pStaticNum->GetString(slotNum)->msg = L"";
					}
				}

				//�ش� ���������� ���� ����
				int index = 0;
				int resultItemID = -1;
				while( it != m_mapResolveResultInsertedItem.end() && it != maxiter )
				{
					resultItemID = it->first;

					const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
					if ( pItemTemplet != NULL )
					{
						if ( pStaticSlot != NULL && pStaticSlot->GetPicture(index) != NULL )
						{
                            const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
							if ( pwszShopImage[0] != NULL )
								pStaticSlot->GetPicture(index)->SetTex( pwszShopImage );
							else
								pStaticSlot->GetPicture(index)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
						}
						if ( pStaticName != NULL && pStaticName->GetString(index) != NULL )
						{
                            pStaticName->GetString(index)->msg = pItemTemplet->GetFullName_();
						}

						if ( pStaticNum != NULL && pStaticNum->GetString(index) != NULL )
						{
							wstring wstrCount = GET_REPLACED_STRING( ( STR_ID_865, "i", it->second) );
							pStaticNum->GetString(index)->msg = wstrCount.c_str();
						}
					}
					++it;
					++index;
				}
			}
			break;
		default:
			//�ش� �׸� ������ ���� �ƴ϶��, ���â�� ��µ� �� ������ ������ return;
			return;
			break;
		}


	}
}


// oasis907 : ����� [2010.4.14] //
bool CX2UIInventory::OpenNewSocketWindow( CX2SlotItem* pSlot )
{
	//{{ oasis907 : ����� [2010.4.5] //  ���� ó��
	if ( g_pData->GetSocketItem()->GetIsPossibleSocketItemByOnlyItemType( pSlot->GetItemTID() ) == false )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ),
			GET_STRING( STR_ID_824 ), g_pMain->GetNowState() , -1, 3.0f );

		return false;
	}

	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( pSlot->GetItemUID() );

	//{{ kimhc // 2010-01-06 // PC�� �����̾� ����
	if ( pItem != NULL &&
        pItem->GetItemTemplet() != NULL &&
        pItem->GetItemTemplet()->GetIsPcBang() == true )
	{
		g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4839 ),g_pMain->GetNowState() );
		return false;
	}
	//}} kimhc // 2010-01-06 // PC�� �����̾� ����

	// ��ȭ ���� �˻� (���Ҵ��ΰ�?)
	if(true == pItem->IsDisabled())
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_800 ), g_pMain->GetNowState() );
		return false;
	}
	else if( false == pItem->GetIsEvaluation() )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_24617 ), g_pMain->GetNowState() );
		return false;
	}

	// kimhc // 2010-10-19 // low ����� ������ ������ ���� ����
	if ( CX2Item::IG_LOW == pItem->GetItemTemplet()->GetItemGrade() )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( -999, -999 ), GET_STRING( STR_ID_9088 ), g_pMain->GetNowState() );
		return false;
	}

	SAFE_DELETE( m_pUISocketItem );
	m_pUISocketItem = new CX2UISocketItem( g_pMain->GetNowState(), NULL );

	m_pUISocketItem->ResetSocketWindow(pItem->GetUID());

	return true;
}
// oasis907 : ����� [2010.4.14] //

//{{ oasis907 : ����� [2010.9.2] // �� �κ��丮
void CX2UIInventory::OpenPetInventory()
{

	if(IsPetInventoryExist() == false)
	{
		return;
	}

	SAFE_DELETE( m_pUIPetInventory );
	m_pUIPetInventory = new CX2UIPetInventory( g_pMain->GetNowState(), m_pDLGUIInventory->GetLayer() );
	m_pUIPetInventory->SetShow(true);
	return;
}
//}}



//{{ 2012. 02. 21	��μ�	�κ��丮 ����
bool CX2UIInventory::Handler_EGS_SORT_CATEGORY_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_SORT_CATEGORY_ITEM_ACK kEGS_SORT_CATEGORY_ITEM_ACK;
	DeSerialize( pBuff, &kEGS_SORT_CATEGORY_ITEM_ACK );

	if( g_pMain->DeleteServerPacket( EGS_SORT_CATEGORY_ITEM_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEGS_SORT_CATEGORY_ITEM_ACK.m_iOK ) == true )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateCategorySlotList((CX2Inventory::SORT_TYPE)kEGS_SORT_CATEGORY_ITEM_ACK.m_iCategoryType, kEGS_SORT_CATEGORY_ITEM_ACK.m_vecUpdatedInventorySlot);
			g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlot();
			return true;
		}
	}

	return false;
}
//}}

void CX2UIInventory::GetRequiredArmorOnlyRecoveryStoneID( IN const int iItemLevel_, OUT vector<int>& vecRecoveryItemIdList_ )
{
	if( iItemLevel_ <= 20 )
	{
		vecRecoveryItemIdList_.push_back( 130545 );	// ������ lv1
	}
	else if( iItemLevel_ <= 30 )
	{
		vecRecoveryItemIdList_.push_back( 130546 );	// ������ lv2
	}
	else if( iItemLevel_ <= 40 )
	{
		vecRecoveryItemIdList_.push_back( 130547 );	// ������ lv3
	}
	else if( iItemLevel_ <= 50 )
	{
		vecRecoveryItemIdList_.push_back( 130548 );	// ������ lv4
	}
	else if ( iItemLevel_ <= 60 )
	{
		vecRecoveryItemIdList_.push_back( 160027 ); // ������ lv5
	}
	else if ( iItemLevel_ <= 70 )
	{
		vecRecoveryItemIdList_.push_back( 160028 ); // ������ lv6
	}
	else if ( iItemLevel_ <= 80 )
	{
		vecRecoveryItemIdList_.push_back( 160029 ); // ������ lv7
	}
	else if ( iItemLevel_ <= 90 )
	{
		vecRecoveryItemIdList_.push_back( 160030 ); // ������ lv8
	}
}

// ������ ������ ���� �ʿ��� ������ �������� ID
void CX2UIInventory::GetRequiredRecoveryStoneID( IN const int iItemLevel_, OUT vector<int>& vecRecoveryItemIdList_ )
{
	if( iItemLevel_ <= 20 )
	{
		vecRecoveryItemIdList_.push_back( 206880 );	// ������ lv1
		vecRecoveryItemIdList_.push_back( 85002096 );	// ������ lv1
		vecRecoveryItemIdList_.push_back( 70001910 );	// ������(�̺�Ʈ) lv1 JP
		vecRecoveryItemIdList_.push_back( 83000150 );	// 2012.07.24 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv1
		vecRecoveryItemIdList_.push_back( 83000300 );	// �縮���� ���� lv1
	}
	else if( iItemLevel_ <= 30 )
	{
		vecRecoveryItemIdList_.push_back( 206890 );	// ������ lv2
		vecRecoveryItemIdList_.push_back( 85002097 );	// ������ lv1
		vecRecoveryItemIdList_.push_back( 70002801 );	// ������(�̺�Ʈ) lv2 JP
		vecRecoveryItemIdList_.push_back( 83000301 );	// �縮���� ���� lv2
	}
	else if( iItemLevel_ <= 40 )
	{
		vecRecoveryItemIdList_.push_back( 206900 );	// ������ lv3
		vecRecoveryItemIdList_.push_back( 85002098 );	// ������ lv1
		vecRecoveryItemIdList_.push_back( 70002802 );	// ������(�̺�Ʈ) lv3 JP
		vecRecoveryItemIdList_.push_back( 83000152 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv3
		vecRecoveryItemIdList_.push_back( 83000302 );	// �縮���� ���� lv3
	}
	else if( iItemLevel_ <= 50 )
	{
		// ���� �߿��մϴ�.
		vecRecoveryItemIdList_.push_back( 206910 );	// ������ lv4
		vecRecoveryItemIdList_.push_back( 160370 );	// ������(�̺�Ʈ) lv4
		vecRecoveryItemIdList_.push_back( 85002099 );	// ������ lv1
		vecRecoveryItemIdList_.push_back( 70002803 );	// ������(�̺�Ʈ) lv4 JP
		vecRecoveryItemIdList_.push_back( 83000153 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv4
		vecRecoveryItemIdList_.push_back( 83000303 );	// �縮���� ���� lv4
	}
	else if( iItemLevel_ <= 60 )
	{
		// ���� �߿��մϴ�.
		vecRecoveryItemIdList_.push_back( 206920 );	// ������ lv5
		vecRecoveryItemIdList_.push_back( 160371 );	// ������(�̺�Ʈ) lv5
		vecRecoveryItemIdList_.push_back( 85002100 );	// ������ lv1
		vecRecoveryItemIdList_.push_back( 70002804 );	// ������(�̺�Ʈ) lv5 JP
		vecRecoveryItemIdList_.push_back( 83000154 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv5
		vecRecoveryItemIdList_.push_back( 83000304 );	// �縮���� ���� lv5
	}
	else if( iItemLevel_ <= 70 )
	{
		vecRecoveryItemIdList_.push_back( 206930 );	// ������ lv6
		vecRecoveryItemIdList_.push_back( 67006183 );	// �Ƹ����� �ູ���� ������ �ֹ��� Lv.6
		vecRecoveryItemIdList_.push_back( 70002805 );	// ������(�̺�Ʈ) lv6 JP
		vecRecoveryItemIdList_.push_back( 83000155 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv6
		vecRecoveryItemIdList_.push_back( 83000305 );	// �縮���� ���� lv6
	}
	else if( iItemLevel_ <= 80 )
	{
		vecRecoveryItemIdList_.push_back( 206940 );	// ������ lv7
		vecRecoveryItemIdList_.push_back( 70002806 );	// ������(�̺�Ʈ) lv7
		vecRecoveryItemIdList_.push_back( 83000156 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv7
		vecRecoveryItemIdList_.push_back( 83000306 );	// �縮���� ���� lv7
	}
	else if( iItemLevel_ <= 90 )
	{
		vecRecoveryItemIdList_.push_back( 206950 );	// ������ lv8
		vecRecoveryItemIdList_.push_back( 70002807 );	// ������(�̺�Ʈ) lv8
		vecRecoveryItemIdList_.push_back( 83000157 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv8
		vecRecoveryItemIdList_.push_back( 83000307 );	// �縮���� ���� lv8
	}
	else if( iItemLevel_ <= 100 )
	{
		vecRecoveryItemIdList_.push_back( 206960 );	// ������ lv9
		vecRecoveryItemIdList_.push_back( 70002808 );	// ������(�̺�Ʈ) lv9
		vecRecoveryItemIdList_.push_back( 83000158 );	// 2012.10.17 lygan_������ // �߱� ���� �Ƹ����� �ູ���� ������ lv9
		vecRecoveryItemIdList_.push_back( 83000308 );	// �縮���� ���� lv9
	}
	else
	{
		ASSERT( !"invalid item level" );
	}

}

void CX2UIInventory::CheckRequiredRecoveryStoneInInventory( IN OUT vector<int>& vecRecoveryItemIdList_ )
{
	if ( false == vecRecoveryItemIdList_.empty() )
	{
		const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();
		vector<int>::const_iterator vItr = vecRecoveryItemIdList_.begin();

		do
		{
			if ( 0 == kInventory.GetNumItemByTID( *vItr ) )
				vItr = vecRecoveryItemIdList_.erase( vItr );
			else
				++vItr;

		} while ( vItr != vecRecoveryItemIdList_.end() );
	}
}

bool CX2UIInventory::Handler_EGS_RESTORE_ITEM_REQ( UidType RecoveryItemUid )
{
	// �ϴ� ������ �ִ��� �ٽ� Ȯ�����ְ�
	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( RecoveryItemUid );
	if(pItem == NULL)
		return false;

	KEGS_RESTORE_ITEM_REQ kEGS_RESTORE_ITEM_REQ;

	kEGS_RESTORE_ITEM_REQ.m_iItemUID = RecoveryItemUid;

	g_pData->GetServerProtocol()->SendPacket( EGS_RESTORE_ITEM_REQ, kEGS_RESTORE_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_RESTORE_ITEM_ACK );

	return true;

}

bool CX2UIInventory::Handler_EGS_RESTORE_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_RESTORE_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_RESTORE_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

			UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

#if X2_DEV_TOOLS
			if( CX2User::XUAL_DEV != g_pData->GetMyUser()->GetAuthLevel() )
			{
				g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_828 ), g_pMain->GetNowState() );
			}
#else
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_828 ), g_pMain->GetNowState() );
#endif

			return true;
		}
	}

	return false;
}



//{{ kimhc // �ǽð� ���ҵ� �� ������ ������ ������
bool CX2UIInventory::Handler_EGS_DELETE_ITEM_REQ( UidType DeleteItemUid, int Quantity )
{
	// �ϴ� ������ �ִ��� �ٽ� Ȯ�����ְ�
	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( DeleteItemUid );
	if(pItem == NULL)
		return false;

	// �������� ��� �������� ������ ������ �ִ� �ͺ��� ������
	if ( pItem->GetItemData().m_PeriodType == CX2Item::PT_QUANTITY &&
		pItem->GetItemData().m_Quantity < m_iItemEnrollNum )
	{
		return false;
	}

	KEGS_DELETE_ITEM_REQ kEGS_DELETE_ITEM_REQ;

	kEGS_DELETE_ITEM_REQ.m_iItemUID = DeleteItemUid;
	kEGS_DELETE_ITEM_REQ.m_iQuantity = Quantity;

	g_pData->GetServerProtocol()->SendPacket( EGS_DELETE_ITEM_REQ, kEGS_DELETE_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_DELETE_ITEM_ACK );

	return true;


}
//}} kimhc // �ǽð� ���ҵ� �� ������ ������ ������

bool CX2UIInventory::Handler_EGS_DELETE_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_DELETE_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_DELETE_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			std::vector< KInventoryItemInfo > veckInventoryItemInfo;
			veckInventoryItemInfo.push_back(kEvent.m_kInventoryItemInfo);

			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( veckInventoryItemInfo );

			UpdateInventorySlotList( veckInventoryItemInfo );

			if ( g_pData->GetUIManager() == NULL ||
				g_pData->GetUIManager()->GetUIQuestNew() == NULL )
				return true;

			if ( g_pData->GetUIManager()->GetShow( CX2UIManager::UI_MENU_QUEST_NEW ) == true )
			{
				g_pData->GetUIManager()->GetUIQuestNew()->ResetQuestUI();
			}

			g_pData->GetUIManager()->GetUIQuestNew()->UpdateQuickQuestDLG();
			return true;
		}
	}

	return false;

}






std::wstring CX2UIInventory::GetSlotItemDescExtra_RBtn( int itemTID, CX2Item* pItem )
{
	std::wstring slotItemDesc = L"";

	if ( m_bShowRBDownMessage == true )
	{
		// ���λ����� ���������� �� �������� ���λ����� ����� �� �ִٰ� ����
		if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
			g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_WAIT )
		{
			slotItemDesc += L"\n";
            slotItemDesc += GET_STRING( STR_ID_830 );
            return slotItemDesc;
        }
		// �븮������ ���������� �� �������� ���λ����� ����� �� �ִٰ� ����
		if( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
			g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_AGENCY_WAIT )
		{
			slotItemDesc += L"\n";
			slotItemDesc += GET_STRING( STR_ID_830 );
			return slotItemDesc;
		}
		// ���ΰŷ�â�� ���������� (��)
		else if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE))
		{
			slotItemDesc += L"\n";
            slotItemDesc += GET_STRING( STR_ID_831 );
            return slotItemDesc;
        }

		// �����̳� ����â�� ���������� �ƹ��͵� ���Ѵ�
		// 		else if(true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_SHOP) || true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_MANUFACTURE) )
		// 		{
		//             return slotItemDesc;
		// 		}

		const CX2Item::ItemTemplet* pkItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemTID );
		if( pkItemTemplet != NULL )
		{
            switch( pkItemTemplet->GetItemType() )
            {
            case CX2Item::IT_WEAPON:
            case CX2Item::IT_DEFENCE:
            case CX2Item::IT_ACCESSORY:
            case CX2Item::IT_QICK_SLOT:
			    {
				    slotItemDesc += L"\n";

				    //{{ kimhc // 2009-08-26 // ������ ���� �� ��ũ���� ����
				    if ( pItem->GetItemData().m_bIsSealed == true )
				    {
					    slotItemDesc += GET_STRING( STR_ID_4422 );
				    }
				    else
				    //}} kimhc // 2009-08-26 // ������ ���� �� ��ũ���� ����
                    slotItemDesc += GET_STRING( STR_ID_19 );
                }
                break;
            default:
            	break;
            }

		}
	}

	return slotItemDesc;
}

// bAbsolute�� true�� ȭ��� ������ǥ. false�� ���� �κ��丮 ��ġ�� �������� �� ��� ��ǥ.
void CX2UIInventory::SetPosition(D3DXVECTOR2 vec, bool bAbsolute )
{
	// �ϴ� ����ġ�� ���ͽ�Ų �� �̵���Ű�� ������ ����� ����

	// ����ġ ����
	if(m_pDLGUIInventory != NULL) m_pDLGUIInventory->SetPos( m_DLGPosition );

	for ( int i = 0; i < (int)m_SlotList.size(); i++ )
	{
		CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
		D3DXVECTOR2 pos;
		pos = pItemSlot->GetPos();
		pos -= m_MovedPosition;
		pItemSlot->SetPos(pos);
	}

	m_MovedPosition = D3DXVECTOR2(0, 0);

	// ���� ��ǥ�� �������� �̵���Ų��

	m_MovedPosition = vec;
	if( bAbsolute ) m_MovedPosition = m_MovedPosition - m_DLGPosition;

	if(m_pDLGUIInventory != NULL) m_pDLGUIInventory->SetPos( m_MovedPosition );
	// ��¦ ����ȭ : 0,0�̸� �� ���� -_-
	if( m_MovedPosition != D3DXVECTOR2(0, 0) )
	{
		for ( int i = 0; i < (int)m_SlotList.size(); i++ )
		{
			CX2SlotItem* pItemSlot = (CX2SlotItem*)m_SlotList[i];
			D3DXVECTOR2 pos;
			pos = pItemSlot->GetPos();
			pos += m_MovedPosition;
			pItemSlot->SetPos(pos);
		}
	}
}

bool CX2UIInventory::GetOverMouseInven()
{
	if( GetShow() == true && m_pDLGUIInventory != NULL && m_pDLGUIInventory->GetIsMouseOver() == true )
		return true;
	return false;
}


void CX2UIInventory::UpdateOpenAttractionItemAck()
{
	m_eRandomItemEventType = RIOET_NONE;

	// ���� â�� �����..
	if ( m_pDLGOpenAttraction != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOpenAttraction, NULL, false );
	m_pDLGOpenAttraction = NULL;

	// ���� ������
	m_pDLGAttractionResult = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Charm_Item_Result.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGAttractionResult );


	const wstring wstrResultDialogTitle		=
		NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrResultDialogTitleTexture : L"DLG_UI_Title02_B.tga";

	const wstring wstrResultDialogTitleKey	=
		NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrResultDialogTitleTextureKey : L"heater_title";


	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>( m_pDLGAttractionResult->GetControl( L"Static_Title" ) );
	if( NULL != pStatic )
	{
		if( NULL != pStatic->GetPicture(0) )
		{
			pStatic->GetPicture(0)->SetTex( wstrResultDialogTitle.c_str(), wstrResultDialogTitleKey.c_str() );
			pStatic->GetPicture(0)->SetSizeAsTextureSize();		// �ؿ��� �ڵ� �߰�
		}
		if( NULL != pStatic->GetPicture(1) )
			pStatic->GetPicture(1)->SetShow(false);
	}

	/// �� ���������, ���� ��ü ����
	m_pNowIceHeaterEventData = NULL;


	if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
	{
		// �׳� ������ + ���ʽ� ������ �ΰ��� ���´ٰ� �����ϰ�..
		std::map< int, int >::iterator i;

		wstring getExplanation = L"";
		for ( i = m_TempOpenRandomItemAck.m_mapInsertedItem.begin(); i != m_TempOpenRandomItemAck.m_mapInsertedItem.end(); i++ )
		{
			int resultItemID = i->first;
			int resultNum	= i->second;
			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
			if ( pItemTemplet == NULL ) continue;

			CKTDGUIStatic* pStatic = NULL;
			if(resultItemID == ATTRACTION_BONUS_ITEM_ID
				|| resultItemID == RID_BATTLEFIELD_COOKIE
				|| resultItemID == RID_BATTLEFIELD_GOLD_COOKIE
				|| resultItemID == ALL_IN_ONE_ATTRACTION_BONUS_ITEM_ID
				|| resultItemID == ANCIENT_FOSSIL_PIECE
				)
			{

				// ���ʽ� ������
				pStatic = (CKTDGUIStatic*) m_pDLGAttractionResult->GetControl( L"Static_Bonus" );
				pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_832, "Si", pItemTemplet->GetFullName_(), resultNum ) );
			}
			else
			{
				// �׳� ������
				wstring itemName = L"";
				pStatic = (CKTDGUIStatic*) m_pDLGAttractionResult->GetControl( L"Static_Item" );
                itemName = pItemTemplet->GetFullName_();

				if( resultNum == 1)
				{
                    pStatic->GetString(0)->msg = itemName;
				}
				else
				{
                    pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_832, "Li", itemName, resultNum ) );
				}
			}

			if( NULL != pStatic &&
				false == pStatic->GetPicture(0)->SetTex(
                pItemTemplet->GetShopImage()
                ) )
			{
				pStatic->GetPicture(0)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

		}
	}

	UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );
}

void CX2UIInventory::PlayGetAttractionItem( RANDOM_ITEM_OPEN_EVENT_TYPE /*eRandomItemEventType*/ /*= RIOET_ICE_HEATER*/ )
{
	if ( m_bPlayAttractionItem == false )
	{
		m_bPlayAttractionItem = true;

		m_pDLGGetAttractionItemBG = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Unit_State_RandomItem_BG.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGGetAttractionItemBG );
		m_pDLGGetAttractionItemBG->SetColor( D3DXCOLOR( 0,0,0,0) );
		m_pDLGGetAttractionItemBG->Move( D3DXVECTOR2(0,0), D3DXCOLOR( 0,0,0,0.7f), 0.5f, true );

		D3DXVECTOR3 boxPos = m_pDLGOpenAttraction->GetDummyPos( 0 );
		if( m_hMeshInstMeltMachine != INVALID_MESH_INSTANCE_HANDLE )
		{
			g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachine );
			if( NULL != m_pDLGOpenAttraction )
			{
				m_pDLGOpenAttraction->SetHasUnit( NULL );
			}
		}


		const wstring wstrPlayMeshPlayerName =
			NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrPlayMeshPlayerName : L"FireMachineStart";

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MeltMachineStart = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL, wstrPlayMeshPlayerName.c_str(),
			boxPos.x, boxPos.y, boxPos.z , 0,0,0, 0,0,0 );


        if ( pMeshInst_MeltMachineStart != NULL )
        {
            pMeshInst_MeltMachineStart->SetPerFrameSimulation( true );
		    g_pKTDXApp->GetDGManager()->RemoveObjectChain(pMeshInst_MeltMachineStart);
		    if( NULL != m_pDLGOpenAttraction )
		    {
			    m_pDLGOpenAttraction->SetHasUnit(pMeshInst_MeltMachineStart);

		    }
		    m_hMeshInstMeltMachineStart = pMeshInst_MeltMachineStart->GetHandle();
        }

		/// ������ ��� ���� ����
		const wstring pSoundName =
			NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrIceHeaterPlaySound : L"Fire_Machine_Start.ogg";

			g_pKTDXApp->GetDeviceManager()->PlaySound( pSoundName.c_str(), false, false );



		// 		m_hSeqKeyTrace = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  L"KeyTrace", m_pMeshInstRandomBoxKey->GetPos() );
		// 		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqKeyTrace );
		// 		if( NULL != pSeq )
		// 		{
		// 			pSeq->SetOverUI( true );
		// 		}
		// 		else
		// 		{
		// 			m_hSeqKeyTrace = INVALID_PARTICLE_SEQUENCE_HANDLE;
		// 		}

	}
}

void CX2UIInventory::PlayGetAttractionItemGold()
{
	if ( m_bPlayAttractionItem == false )
	{
		m_bPlayAttractionItem = true;

		m_pDLGGetAttractionItemBG = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Unit_State_RandomItem_BG.lua" );
		g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGGetAttractionItemBG );
		m_pDLGGetAttractionItemBG->SetColor( D3DXCOLOR( 0,0,0,0) );
		m_pDLGGetAttractionItemBG->Move( D3DXVECTOR2(0,0), D3DXCOLOR( 0,0,0,0.7f), 0.5f, true );

		D3DXVECTOR3 boxPos = m_pDLGOpenAttraction->GetDummyPos( 0 );
		if( m_hMeshInstMeltMachine != INVALID_MESH_INSTANCE_HANDLE )
		{
			g_pData->GetUIMajorXMeshPlayer()->DestroyInstanceHandle( m_hMeshInstMeltMachine );
			if( NULL != m_pDLGOpenAttraction )
			{
				m_pDLGOpenAttraction->SetHasUnit( NULL );
			}
		}

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MeltMachineStart = g_pData->GetUIMajorXMeshPlayer()->CreateInstance( NULL,  L"GoldFireMachineStart", boxPos.x, boxPos.y, boxPos.z , 0,0,0, 0,0,0 );
        if ( pMeshInst_MeltMachineStart != NULL )
        {
            pMeshInst_MeltMachineStart->SetPerFrameSimulation( true );
		    g_pKTDXApp->GetDGManager()->RemoveObjectChain(pMeshInst_MeltMachineStart);
		    if( NULL != m_pDLGOpenAttraction )
		    {
			    m_pDLGOpenAttraction->SetHasUnit(pMeshInst_MeltMachineStart);

		    }
		    m_hMeshInstMeltMachineStart = pMeshInst_MeltMachineStart->GetHandle();
        }

		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Fire_Machine_Start.ogg", false, false );


		// 		m_hSeqKeyTrace = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  L"KeyTrace", m_pMeshInstRandomBoxKey->GetPos() );
		// 		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqKeyTrace );
		// 		if( NULL != pSeq )
		// 		{
		// 			pSeq->SetOverUI( true );
		// 		}
		// 		else
		// 		{
		// 			m_hSeqKeyTrace = INVALID_PARTICLE_SEQUENCE_HANDLE;
		// 		}

	}
}


bool CX2UIInventory::Handler_EGS_ENCHANT_ATTACH_ITEM_REQ( UidType CouponItemUID, UidType TargetItemUID )
{
	// �ϴ� ������ �ִ��� �ٽ� Ȯ�����ְ�
	CX2Item* pCouponItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( CouponItemUID );
	CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( TargetItemUID );

	// ��밡�������� Ȯ���Ѵ�
	if ( pCouponItem == NULL || pTargetItem == NULL ||
		pCouponItem->GetItemTemplet() == NULL )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_833 ), g_pMain->GetNowState() );
		return false;
	}
	// Ȥ�� �� �ƴ� ���� �� ������ �߸��� �ʳ�?
	if( pTargetItem->GetItemTemplet()->GetItemType() != CX2Item::IT_DEFENCE &&
		true == IsDefenceOnlyCoupon(
        pCouponItem->GetItemTemplet()->GetItemID()
        ) )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_2564 ), g_pMain->GetNowState() );
		return false;
	}
	if ( true == CheckLevelIfLevelLimitEnchantCoupon
		( pCouponItem->GetItemTemplet()->GetItemID(), pTargetItem->GetItemTemplet()->GetUseLevel() ) )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_25399 ), g_pMain->GetNowState() );
		return false;
	}

	if( IsEnchantCouponUseable(
		pCouponItem->GetItemTemplet()->GetItemID(),
		pTargetItem->GetItemData().m_EnchantLevel ) )

	{
		KEGS_ENCHANT_ATTACH_ITEM_REQ kEGS_ATTACH_ITEM_REQ;

		kEGS_ATTACH_ITEM_REQ.m_iAttachItemUID = CouponItemUID;
		kEGS_ATTACH_ITEM_REQ.m_iDestItemUID = TargetItemUID;

		g_pData->GetServerProtocol()->SendPacket( EGS_ENCHANT_ATTACH_ITEM_REQ, kEGS_ATTACH_ITEM_REQ );
		g_pMain->AddServerPacket( EGS_ENCHANT_ATTACH_ITEM_ACK );

		return true;
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_834 ), g_pMain->GetNowState() );
	}

	return false;
}
bool CX2UIInventory::Handler_EGS_ENCHANT_ATTACH_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ENCHANT_ATTACH_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_ENCHANT_ATTACH_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

			UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_835 ), g_pMain->GetNowState() );

			return true;
		}
	}

	return false;

}


bool CX2UIInventory::Handler_EGS_ATTRIB_ATTACH_ITEM_REQ( UidType CouponItemUID, UidType TargetItemUID )
{
	// �ϴ� ������ �ִ��� �ٽ� Ȯ�����ְ�
	CX2Item* pCouponItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( CouponItemUID );
	CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( TargetItemUID );

	// ��밡�������� Ȯ���Ѵ�
	if ( pCouponItem == NULL || pTargetItem == NULL ||
		pCouponItem->GetItemTemplet() == NULL )
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_833 ), g_pMain->GetNowState() );
		return false;
	}

	KEGS_ATTRIB_ATTACH_ITEM_REQ kEGS_ATTRIB_ATTACH_ITEM_REQ;

	kEGS_ATTRIB_ATTACH_ITEM_REQ.m_iAttachItemUID = CouponItemUID;
	kEGS_ATTRIB_ATTACH_ITEM_REQ.m_iDestItemUID = TargetItemUID;

	g_pData->GetServerProtocol()->SendPacket( EGS_ATTRIB_ATTACH_ITEM_REQ, kEGS_ATTRIB_ATTACH_ITEM_REQ );
	g_pMain->AddServerPacket( EGS_ATTRIB_ATTACH_ITEM_ACK );

	return true;

}
bool CX2UIInventory::Handler_EGS_ATTRIB_ATTACH_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ATTRIB_ATTACH_ITEM_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	g_pMain->DeleteServerPacket( EGS_ATTRIB_ATTACH_ITEM_ACK );
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );

			UpdateInventorySlotList( kEvent.m_vecInventorySlotInfo );
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_835 ), g_pMain->GetNowState() );
			InvalidSlotDesc();
			return true;
		}
	}

	return false;

}


bool CX2UIInventory::IsEnchantCouponUseable(int CouponItemID, int TargetEnchantLevel )
{
	int CouponLevel = 0;

	for(int i=0; i<std::ssize(INT_ENCHANT_COUPON_ITEM_ID); ++i)
	{
		if( CouponItemID == INT_ENCHANT_COUPON_ITEM_ID[i] )
		{
			return true; //�ؿ��� ��ȭ�������̸� ���� �˻��Ѵ�.
			break;
		}
	}

	for(int i=0; i<std::ssize(ENCHANT_COUPON_ITEM_ID); ++i)
	{
		if( CouponItemID == ENCHANT_COUPON_ITEM_ID[i] )
		{
			CouponLevel = i;
			break;
		}
	}
	if( CouponLevel == 0 )
	{
		for(int i=0; i<std::ssize(DEFENCE_ENCHANT_COUPON_ITEM_ID); ++i)
		{
			if( CouponItemID == DEFENCE_ENCHANT_COUPON_ITEM_ID[i] )
			{
				CouponLevel = i;
				break;
			}
		}
	}

	//{{ kimhc // 2010.6.7 // �ŷ��� �Ұ����� ��ȭ�� �߰�
	if ( CouponLevel == 0 )
	{
		for ( UINT i = 0; i < ENCHANT_COUPON_ITEM_TO_NOT_TRADE_ID.size(); i++ )
		{
			if ( CouponItemID == ENCHANT_COUPON_ITEM_TO_NOT_TRADE_ID[i] )
			{
				CouponLevel = i;
				break;
			}
		}
	}

	if ( CouponLevel == 0 )
	{
		for ( UINT i = 0; i < ARRAY_SIZE(HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_ID); i++ )
		{
			if ( CouponItemID == HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_ID[i] )
			{
				CouponLevel = HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_ENCHANT_VALUE[i];
				// ���� ������ �迭���� ����
				break;
			}
		}
	}
	//}} kimhc // 2010.6.7 // �ŷ��� �Ұ����� ��ȭ�� �߰�

// 	switch(CouponItemID)
// 	{
// 	case 130147:	// 5
// 	case 130543:	// 5 ������
// 		CouponLevel = 5;
// 		break;
// 	case 130148:	// 6
// 	case 130544:	// 6 ������
// 		CouponLevel = 6;
// 		break;
// 	case 130149:	// 7
// 		CouponLevel = 7;
// 		break;
// 	case 130150:	// 8
// 		CouponLevel = 8;
// 		break;
// 	case 130151:	// 9
// 		CouponLevel = 9;
// 		break;
// 	case 130152:	// 10
// 		CouponLevel = 10;
// 		break;
// 	default:
// 		return false;
// 	}

	if(CouponLevel > TargetEnchantLevel) return true;
	return false;
}

void CX2UIInventory::UpdateOpenTreeDayItemAck()
{
	m_eRandomItemEventType = RIOET_NONE;

	// ������ ��������ִ� Attraction Item�� ���â�� ��Ȱ���Ѵ�.
	// �޽��� (�Ϸ���� CustomMsg)� ������ ���â�� �װ��� ������.
	// �����̶� ���ʽ� ������ ����ƽ�� �� ���̰� �����ָ� �� ��.


	// ���� â�� �����..
	if ( m_pDLGOpenAttraction != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOpenAttraction, NULL, false );
	m_pDLGOpenAttraction = NULL;

	// ���� ������
	m_pDLGAttractionResult = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Charm_Item_Result_1slot.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGAttractionResult );

	// "���� ������ ������ ��� ���" ����ƽ �� ���̰� ���ְ�
	CKTDGUIStatic* pStatic_Title = (CKTDGUIStatic*)m_pDLGAttractionResult->GetControl( L"Static_Title" );
	pStatic_Title->SetShow(false);

	// ���ʽ� ������ ����ƽ �� ���̰� ���ְ�
	CKTDGUIStatic* pStatic_BonusMark = (CKTDGUIStatic*)m_pDLGAttractionResult->GetControl( L"Static_Bonus_Item_Mark" );
	pStatic_BonusMark->SetShow(false);

	if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
	{
		// �ĸ��� �̺�Ʈ �������� �ִ� 2�� ���´� ( ��ȹ������ Ȯ���� ������ )
		std::map< int, int >::iterator i;

		bool bFirstItem = true;
		wstring getExplanation = L"";
		for ( i = m_TempOpenRandomItemAck.m_mapInsertedItem.begin(); i != m_TempOpenRandomItemAck.m_mapInsertedItem.end(); i++ )
		{
			int resultItemID = i->first;
			int resultNum	= i->second;
			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
			if ( pItemTemplet == NULL ) continue;

			CKTDGUIStatic* pStatic = NULL;
			if(bFirstItem)
			{
				// ù ��° ������
				pStatic = (CKTDGUIStatic*) m_pDLGAttractionResult->GetControl( L"Static_Item" );
                const wchar_t* pwszitemName = pItemTemplet->GetFullName_();

				if( resultNum == 1)
				{
                    pStatic->GetString(0)->msg = pwszitemName;
                }
				else
				{
                    pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_832, "Si", pwszitemName, resultNum ) );
				}
				bFirstItem = false;
			}
			else
			{
				// �ι�°
				pStatic = (CKTDGUIStatic*) m_pDLGAttractionResult->GetControl( L"Static_Bonus" );
                const wchar_t* pwszitemName = pItemTemplet->GetFullName_();
                pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_832, "Si", pwszitemName, resultNum ) );
			}

			if( NULL != pStatic &&
				NULL != pStatic->GetPicture(0) &&
				false == pStatic->GetPicture(0)->SetTex(
                pItemTemplet->GetShopImage()
                ) )
			{
				pStatic->GetPicture(0)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

		}
	}

	UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );

}

bool CX2UIInventory::IsAprilEventRandomItem( int /*ItemID*/ )
{
	//{{ oasis907 : ����� [2011.3.29] ���� ������ ť�꿡�� �ĸ��� ������ ó��
	return false;
	//}}
}



// kimhc // 2009-08-12 // ���� ī�� ��Ʈ����?
bool CX2UIInventory::IsMonsterCardSetItem( int iItemID )
{
	switch( iItemID )
	{
	case MONSTER_CARD_SET_RUBEN:
	case MONSTER_CARD_SET_ELDER:
	case MONSTER_CARD_SET_BESMA:
	case MONSTER_CARD_SET_PEITA:
	case MONSTER_CARD_SET_ALTERA:
	case MONSTER_CARD_SET_SECRET:
	case MONSTER_CARD_SET_RUBEN_GOLD_EDITION:
	case MONSTER_CARD_SET_ELDER_GOLD_EDITION:
	case MONSTER_CARD_SET_BESMA_GOLD_EDITION:
	case MONSTER_CARD_SET_PEITA_GOLD_EDITION:
	case MONSTER_CARD_SET_ALTERA_GOLD_EDITION:
	case MONSTER_CARD_SET_SECRET_GOLD_EDITION:
	case MONSTER_CARD_SET_ELIOS:
	case MONSTER_CARD_SET_ELIOS_GOLD_EDITION:
	case MONSTER_CARD_SET_ELIOS_ALL_EDITION:
		return true;
		break;

	default:
		return false;
		break;
	}
	return false;
}

bool CX2UIInventory::IsAttachItem( int ItemID )
{
	for(int i=0; i<std::ssize(ATTACH_ITEM_ITEM_ID); ++i)
	{
		if( ItemID == ATTACH_ITEM_ITEM_ID[i] )
			return true;
	}
// 	switch(ItemID)
// 	{
// 	case 130147:		// ��ȭ�� +5
// 	case 130148:
// 	case 130149:
// 	case 130150:
// 	case 130151:
// 	case 130152:		// +10
//
// 	case 130543: // �� ���� ��ȭ�� ���� Lv.5
// 	case 130544: // �� ���� ��ȭ�� ���� Lv.6
// 		{
// 			return true;
// 		} break;
// 	default:
// 		return false;
// 	}
	return false;
}

bool CX2UIInventory::IsDefenceOnlyCoupon( int ItemID )
{
	for(int i=5; i<std::ssize(DEFENCE_ENCHANT_COUPON_ITEM_ID); ++i)
	{
		if( ItemID == DEFENCE_ENCHANT_COUPON_ITEM_ID[i] )
			return true;
	}
// 	switch(ItemID)
// 	{
// 	case 130543: // �� ���� ��ȭ�� ���� Lv.5
// 	case 130544: // �� ���� ��ȭ�� ���� Lv.6
// 		return true;
// 	default:
// 		return false;
// 	}
	return false;
}

bool CX2UIInventory::IsAttributeItem( int ItemID )
{
	for(int i=0; i<std::ssize(ATTRIBUTE_CHARM_COUPON_ITEM_ID); ++i)
	{
		if( ItemID == ATTRIBUTE_CHARM_COUPON_ITEM_ID[i] )
			return true;
	}
	return false;
}

wstring CX2UIInventory::GetAttributeItemExtraDamageName( int itemTID )
{
	wstring enchantDesc = L"";
	CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType = CX2DamageManager::EDT_NONE;
	switch(itemTID)
	{
	case 133365:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER;		break;
	case 133366:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER;		break;
	case 133367:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_POISON_MASTER;		break;
	case 133368:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING;	break;
	case 133369:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_MASTER_SHOCK;		break;
	case 133370:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_MASTER_SNATCH;		break;
	case 133371:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING;		break;
	case 133372:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK;		break;
	case 133373:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH;		break;
	case 133374:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING;	break;
	case 133375:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK;		break;
	case 133376:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH;		break;
	case 133377:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_POISON_PIERCING;	break;
	case 133378:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_POISON_SHOCK;		break;
	case 133379:
		extraDamageType = CX2DamageManager::EDT_ENCHANT_POISON_SNATCH;		break;
	default:
	 	extraDamageType = CX2DamageManager::EDT_NONE;
	}
	enchantDesc = g_pData->GetDamageManager()->GetExtraDamageName( extraDamageType );
	return enchantDesc;
}



void CX2UIInventory::UpdateUIButton()
{
	if( m_bShow == false || m_pDLGUIInventory == NULL )
		return;



	if( g_pData != NULL && g_pData->GetMyUser() != NULL && g_pData->GetMyUser()->GetSelectUnit() != NULL )
	{
		if( g_pData->GetMyUser()->GetSelectUnit()->GetMaxSkillNoteSlot() > 0 )
		{
			if(m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true)
			{
				HideSkillNote(false);
				SetEnableSkillNote(false);
			}
			else
			{
				// ����� ��Ʈ ��밡��
				HideSkillNote(false);
				SetEnableSkillNote(true);

			}
		}
		else
		{
			// ����� ��Ʈ ���Ұ�
			HideSkillNote(true);

		}
	}

	CKTDGUIButton* pSocketButtton	= static_cast< CKTDGUIButton* >( m_pDLGUIInventory->GetControl( L"soket" ) );

	if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME ||
		g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME ||
		g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE) == true
		)
	{
		pSocketButtton->SetShowEnable( false, false );
	}
	// oasis907 : ����� [2010.4.6] //
	else if((m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true) || g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) == true)
	{
		pSocketButtton->SetShowEnable( false, false );
	}
	else
	{
		pSocketButtton->SetShowEnable( true, true );
	}
	//}} kimhc // �ǽð� ���ҵ� �� ���������� ������ ����, ���� ���� ��ư ��Ȱ��ȭ

	// ���λ��� ���������� ����/������ ����
	CKTDGUIButton* pTrashButtton	= static_cast< CKTDGUIButton* >( m_pDLGUIInventory->GetControl( L"trash" ) );
	if(pTrashButtton != NULL)
	{
		if( ( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
			( g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_SELL
			|| g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_AGENCY_WAIT
			) ) ||
			( g_pMain != NULL && g_pMain->GetPostBox() != NULL && g_pMain->GetPostBox()->GetOpendSendWindow() == true) ||
			( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE) == true ) )
		{
			pTrashButtton->SetShowEnable(true, false);

			SetEnableSkillNote(false);
			SetEnableFeedPet( false );
		}
		// oasis907 : ����� [2010.4.6] //
		else if(m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true
			)
		{
			pTrashButtton->SetShowEnable( true, false );

			SetEnableFeedPet( false );
		}
		else
		{
			pTrashButtton->SetShowEnable(true, true);

			SetEnableSkillNote(true);
			SetEnableFeedPet( true );
		}
	}

	CKTDGUIButton* pResolveButtton	= static_cast< CKTDGUIButton* >( m_pDLGUIInventory->GetControl( L"resolve" ) );
	if(pResolveButtton != NULL)
	{
		if( (true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
			( g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_SELL
			|| g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_AGENCY_WAIT
			) ) ||
			( g_pMain != NULL && g_pMain->GetPostBox() != NULL && g_pMain->GetPostBox()->GetOpendSendWindow() == true) ||
			( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE) == true) )
		{
			pResolveButtton->SetShowEnable(false, false);
		}
		// oasis907 : ����� [2010.4.6] //
		else if(m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true
			)
		{
			pResolveButtton->SetShowEnable( false, false );
		}
		else
		{
			pResolveButtton->SetShowEnable(true, true);
		}
	}



	if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetMyPet() != NULL )
	{
		if( ( true == g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_SHOP) &&
			( g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_SELL
			|| g_pData->GetUIManager()->GetUIPersonalShop()->GetPersonalShopState() == CX2UIPersonalShop::XPSS_AGENCY_WAIT
			) ) ||
			( g_pMain != NULL && g_pMain->GetPostBox() != NULL && g_pMain->GetPostBox()->GetOpendSendWindow() == true) ||
			( g_pData->GetUIManager()->GetShow(CX2UIManager::UI_MENU_PERSONAL_TRADE) == true ) ||
			( m_pUISocketItem != NULL && m_pUISocketItem->GetShow() == true )
			)
		{
			SetEnableFeedPet( false );
		}
		else
		{
			SetEnableFeedPet( true );
		}

	}
	else
	{
		SetEnableFeedPet( false );
	}


	UpdateButtonPetInventory();


	CKTDGUIButton* pCashShopButton = NULL;

	pCashShopButton = reinterpret_cast< CKTDGUIButton* >( m_pDLGUIInventory->GetControl( L"g_pButton_Upgrade_Inven" ) );

	if ( pCashShopButton == NULL )
	{
		ASSERT( !"Cashshop Button error" );
		return;
	}
	const CX2Inventory& kInventory = g_pData->GetMyUser()->GetSelectUnit()->GetInventory();


	if ( kInventory.IsAbleToExpandSlot() == true &&
		 ( g_pMain->GetNowStateID() == CX2Main::XS_VILLAGE_MAP || g_pMain->GetNowStateID() == CX2Main::XS_BATTLE_FIELD ) )
	{
		pCashShopButton->SetShowEnable( true, true );
		pCashShopButton->SetGuideDesc( GET_STRING( STR_ID_28665 ) );
		pCashShopButton->SetGuideDescOffsetPos( D3DXVECTOR2( 100, 0 ) );
	}
	else
	{
		pCashShopButton->SetShowEnable( false, false );
	}



	// ��� ��ɵ�
	if( CX2User::XUAL_DEV == g_pData->GetMyUser()->GetAuthLevel() )
	{
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)m_pDLGUIInventory->GetControl( L"CheckBox_Auth" );
		pCheckBox->SetChecked(false);
		pCheckBox->SetShow(true);
		CKTDGUIButton* pButton_AllSell = (CKTDGUIButton*)m_pDLGUIInventory->GetControl( L"Admin_SellAll" );
		pButton_AllSell->SetShow(true);

	}
	else if( CX2User::XUAL_OPERATOR == g_pData->GetMyUser()->GetAuthLevel() )
	{
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)m_pDLGUIInventory->GetControl( L"CheckBox_Auth" );
		pCheckBox->SetChecked(false);
		pCheckBox->SetShow(true);

		CKTDGUIButton* pButton_AllSell = (CKTDGUIButton*)m_pDLGUIInventory->GetControl( L"Admin_SellAll" );
		pButton_AllSell->SetShow(true);
	}
	else
	{
		CKTDGUICheckBox* pCheckBox = (CKTDGUICheckBox*)m_pDLGUIInventory->GetControl( L"CheckBox_Auth" );
		pCheckBox->SetChecked(false);
		pCheckBox->SetShow(false);
		pCheckBox->SetEnable(false);
		CKTDGUIButton* pButton_AllSell = (CKTDGUIButton*)m_pDLGUIInventory->GetControl( L"Admin_SellAll" );
		pButton_AllSell->SetShowEnable(false, false);
	}


}



void CX2UIInventory::AddWarpList(CKTDGUIContolList *pControlList, const int row, const int iVillageId)
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
	// ��������Ʈ �߰�
	CKTDGUIStatic *pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(wVillageId);
	pControl->SetShow(true);
	pControlList->InsertItem( pControl, row, 0, false );

	if( m_pDLGWarpDestination != NULL )
	{
		m_pDLGWarpDestination->OpenScriptFile(L"DLG_Destination_Control.lua");

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

void CX2UIInventory::InitWarpList()
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

void CX2UIInventory::CreateWarpDest()
{
	m_vecWarpListButton.clear();
	SAFE_DELETE_DIALOG( m_pDLGWarpDestination );

	if( m_pDLGWarpDestination == NULL )
	{
		m_pDLGWarpDestination = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Destination.lua" );
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
			// �纥����
			{
			// �纥����
				pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20000");	//VMI_RUBEN
				pControl->SetShow(false);
				AddWarpList( pControlList, row, SEnum::VMI_RUBEN );
				++row;
			}

			// ��������
			const int iMyUnitLevel = g_pData->GetSelectUnitLevel();
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_ELDER );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ��������
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20001");	//VMI_ELDER
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_ELDER );
					++row;

					// ���� �޽�ó(������ ����)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1000");	//VMI_BATTLE_FIELD_ELDER_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_ELDER_REST_00 );
					++row;
				}
			}

			// ����������
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_BESMA );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ����������
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20002");	//VMI_BESMA
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BESMA );
					++row;

					// ������ �޽�ó(�ƽ��ƽ� �ϴñ�)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1001");	//VMI_BATTLE_FIELD_BESMA_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_BESMA_REST_00 );
					++row;
				}
			}

			// ���׶�����
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_ALTERA_ISLAND );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ���׶���
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20003");	//VMI_ALTERA
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_ALTERA );
					++row;

					// ���׶� �޽�ó(ǳ���� ����ó)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1002");	//VMI_BATTLE_FIELD_ALTERA_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_ALTERA_REST_00 );
					++row;
				}
			}

			// ����Ÿ����
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_PEITA );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ����Ÿ(�İ� ��Ʈ)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20005");	//VMI_PEITA
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_PEITA );
					++row;
				}
			}

			// ��������
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_VELDER );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ��������
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20004");	//VMI_VELDER
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_VELDER );
					++row;

					// ���� �޽�ó (�ð�ž ����)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1004");	//VMI_BATTLE_FIELD_VELDER_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_VELDER_REST_00 );
					++row;
				}
			}

			//{{ JHKang / ������ / 2011/01/11 / �ϸ� �߰�
			// �ϸ�����
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_HAMEL );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ���� �ϸ�
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20006");	//VMI_HAMEL
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_HAMEL );
					++row;

					// �ϸ� �޽�ó(ġ���� ����)
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_1005");	//VMI_BATTLE_FIELD_HAMEL_REST_00
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_BATTLE_FIELD_HAMEL_REST_00 );
					++row;
				}
			}
			//}} JHKang / ������ / 2011/01/11 / �ϸ� �߰�

			// ��������
			pLocalMapTemplet = g_pData->GetLocationManager()->GetLocalMapTemplet( CX2LocationManager::LMI_SANDER );
			if( pLocalMapTemplet != NULL )
			{
				if( iMyUnitLevel >= pLocalMapTemplet->m_RequireUnitLevel )
				{
					// ���� ����
					pControl = (CKTDGUIStatic*)m_pDLGWarpDestination->GetControl(L"Village_20007");	//VMI_HAMEL
					pControl->SetShow(false);
					AddWarpList( pControlList, row, SEnum::VMI_SANDER );
					++row;

					// ���� �޽�ó(���彺�� ������)
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

void CX2UIInventory::SetShowWarpDest(bool val)
{
	if( m_pDLGWarpDestination != NULL )
		m_pDLGWarpDestination->SetShowEnable(val, val);
}

bool CX2UIInventory::GetShowWarpDest()
{
	if( m_pDLGWarpDestination != NULL )
		return m_pDLGWarpDestination->GetShow();

	return false;
}

bool CX2UIInventory::AddSecondJobList(CKTDGUIContolList *pControlList, const int row, const CX2Unit::UNIT_CLASS eSecondJobId)
{
	if( pControlList == NULL )
		return false;

	if ( eSecondJobId == g_pData->GetMyUser()->GetSelectUnit()->GetClass() ||
		2 != g_pData->GetMyUser()->GetSelectUnit()->GetClassLevel() )
		return false;

	WCHAR wszSecondJobId[256] = {0,};
	//wsprintf(wVillageId, L"Village_%d", iVillageId);
	StringCchPrintf( wszSecondJobId, 256, L"Class_%d", static_cast<int>(eSecondJobId) );

	if( m_pDLGUSCJob != NULL )
	{
		// ���� ����Ʈ �߰�
		CKTDGUIStatic *pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl( wszSecondJobId );

		if( pControl == NULL )
			return false;

		pControl->SetShow(true);
		pControlList->InsertItem( pControl, row, 0, false );

		m_pDLGUSCJob->OpenScriptFile(L"DLG_Change_Job_Control.lua");

		CKTDGUIButton *pButton = (CKTDGUIButton*)m_pDLGUSCJob->GetControl( L"ListControl_Button" );

		if( pButton == NULL )
			return false;

		//wsprintf(wVillageId, L"Button_%d", iVillageId);
		StringCchPrintf( wszSecondJobId, 256, L"Button_%d", static_cast<int>(eSecondJobId) );
		pButton->SetName( wszSecondJobId );
		pButton->AddDummyInt(static_cast<int>(eSecondJobId));
		pControlList->InsertItem( pButton, row, 1 );
		pButton->SetShowEnable(true, true);
		pButton->SetDownStateAtNormal(false);

		m_vecUSCJobButton.push_back( pButton );
	}

	return true;
}

void CX2UIInventory::InitSecondJobList()
{
	CKTDGUIStatic *pControl = NULL;

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_100");	//VMI_RUBEN
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_101");	//VMI_ELDER
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_102");	//VMI_BATTLE_FIELD_ELDER_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_103");	//VMI_BESMA
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_104");	//VMI_BATTLE_FIELD_BESMA_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_105");	//VMI_ALTERA
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_106");	//VMI_BATTLE_FIELD_ALTERA_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_107");	//VMI_PEITA
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_108");	//VMI_VELDER
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_109");	//VMI_BATTLE_FIELD_VELDER_REST_00
	pControl->SetShow(false);

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_110");	//VMI_HAMEL
	pControl->SetShow(false);

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_111");	//VMI_BATTLE_FIELD_HAMEL_REST_00
	pControl->SetShow(false);

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_112");	//VMI_SANDER
	pControl->SetShow(false);

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_113");	//VMI_BATTLE_FIELD_SANDER_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_114");	//VMI_RUBEN
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_115");	//VMI_ELDER
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_116");	//VMI_BATTLE_FIELD_ELDER_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_117");	//VMI_BESMA
	pControl->SetShow(false);

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_118");	//VMI_BATTLE_FIELD_BESMA_REST_00
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_119");	//VMI_BATTLE_FIELD_BESMA_REST_00
	pControl->SetShow(false);

	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_120");
	pControl->SetShow(false);
	pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_121");
	pControl->SetShow(false);
}

void CX2UIInventory::CreateSecondJobDest()
{
	m_vecWarpListButton.clear();
	SAFE_DELETE_DIALOG( m_pDLGUSCJob );

	if( m_pDLGUSCJob == NULL )
	{
		if( CX2Unit::UT_ARA == g_pData->GetMyUser()->GetSelectUnit()->GetType())
		{
			m_pDLGUSCJob = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Change_Job_Ara.lua" ); //�ƶ��϶��� ���� �� �� �ִ°� ��ĭ�̴�.
		}
		else
		{
			m_pDLGUSCJob = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Change_Job.lua" );
		}

		if( g_pKTDXApp != NULL )
		{
			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGUSCJob );
		}
	}

	if( m_pDLGUSCJob != NULL )
	{
		m_pDLGUSCJob->SetPos( g_pMain->GetWindowPos( D3DXVECTOR2(300, 400), D3DXVECTOR2(75,20)) );
		m_pDLGUSCJob->SetShowEnable(true, true);

		CKTDGUIContolList *pControlList = (CKTDGUIContolList*)m_pDLGUSCJob->GetControl(L"ChangeJobControlList");
		if( pControlList != NULL )
		{
			CKTDGUIStatic *pControl = NULL;

			int row = 0;
			InitSecondJobList();

			CX2Unit::UNIT_TYPE eUnitType = g_pData->GetMyUser()->GetSelectUnit()->GetType();
			switch( eUnitType )
			{
			case CX2Unit::UT_ELSWORD:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_100");	//UC_ELSWORD_LORD_KNIGHT
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ELSWORD_LORD_KNIGHT ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_101");	//UC_ELSWORD_RUNE_SLAYER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ELSWORD_RUNE_SLAYER ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_112");	//UC_ELSWORD_INFINITY_SWORD
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ELSWORD_INFINITY_SWORD ) )
						++row;
				} break;
			case CX2Unit::UT_ARME:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_104");	//UC_ARME_ELEMENTAL_MASTER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ARME_ELEMENTAL_MASTER ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_105");	//UC_ARME_VOID_PRINCESS
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ARME_VOID_PRINCESS ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_113");	//UC_ARME_DIMENSION_WITCH
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ARME_DIMENSION_WITCH ) )
						++row;
				} break;
			case CX2Unit::UT_LIRE:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_102");	//UC_ARME_ELEMENTAL_MASTER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_LIRE_WIND_SNEAKER ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_103");	//UC_ARME_VOID_PRINCESS
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_LIRE_GRAND_ARCHER ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_114");	//UC_ARME_DIMENSION_WITCH
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_LIRE_NIGHT_WATCHER ) )
						++row;
				} break;
			case CX2Unit::UT_RAVEN:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_106");	//UC_ARME_ELEMENTAL_MASTER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_RAVEN_BLADE_MASTER ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_107");	//UC_ARME_VOID_PRINCESS
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_RAVEN_RECKLESS_FIST ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_115");	//UC_ARME_DIMENSION_WITCH
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_RAVEN_VETERAN_COMMANDER ) )
						++row;
				} break;
			case CX2Unit::UT_EVE:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_108");	//UC_ARME_ELEMENTAL_MASTER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_EVE_CODE_NEMESIS ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_109");	//UC_ARME_VOID_PRINCESS
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_EVE_CODE_EMPRESS ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_116");	//UC_ARME_DIMENSION_WITCH
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_EVE_BATTLE_SERAPH ) )
						++row;
				} break;
			case CX2Unit::UT_CHUNG:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_110");	//UC_ARME_ELEMENTAL_MASTER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_CHUNG_IRON_PALADIN ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_111");	//UC_ARME_VOID_PRINCESS
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_CHUNG_DEADLY_CHASER ) )
						++row;

					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_117");	//UC_ARME_DIMENSION_WITCH
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_CHUNG_TACTICAL_TROOPER ) )
						++row;
				} break;

			case CX2Unit::UT_ARA:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_118");	//UC_ARME_ELEMENTAL_MASTER
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ARA_SAKRA_DEVANAM ) )
						++row;
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_119");	//UC_ARME_VOID_PRINCESS
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ARA_YAMA_RAJA ) )
						++row;
				} break;

			case CX2Unit::UT_ELESIS:
				{
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_120");
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ELESIS_GRAND_MASTER ) )
						++row;
					pControl = (CKTDGUIStatic*)m_pDLGUSCJob->GetControl(L"Class_121");
					pControl->SetShow(false);
					if( AddSecondJobList( pControlList, row, CX2Unit::UC_ELESIS_BLAZING_HEART ) )
						++row;
				} break;
			default:
				break;
			}

			pControlList->SetIndex(0, 0);
		}
	}
}

void CX2UIInventory::SetShowSecondJobDest(bool val)
{
	if( m_pDLGUSCJob != NULL )
	{
		m_pDLGUSCJob->SetShowEnable(val, val);
	}
}

bool CX2UIInventory::GetShowSecondJobDest()
{
	if( m_pDLGUSCJob != NULL )
	{
		return m_pDLGUSCJob->GetShow();
	}

	return false;
}

bool CX2UIInventory::CheckRadioButtonBySortType( CX2Inventory::SORT_TYPE eType )
{
	CKTDGUIRadioButton* pEquipButton = NULL;
	switch( eType )
	{
	case CX2Inventory::ST_EQUIP:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Equip" );

		} break;
	case CX2Inventory::ST_ACCESSORY:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Accessory" );

		} break;
	case CX2Inventory::ST_QUICK_SLOT:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_QuickSlot" );

		} break;
	case CX2Inventory::ST_MATERIAL:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Material" );

		} break;
	case CX2Inventory::ST_QUEST:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Quest" );

		} break;
	case CX2Inventory::ST_SPECIAL:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Special" );
		} break;
	case CX2Inventory::ST_AVARTA:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_Avatar" );
		} break;

		//{{ kimhc // 2010-01-05 // PC�� �����̾� ����
	case CX2Inventory::ST_PC_BANG:
		{
			pEquipButton = (CKTDGUIRadioButton*) m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_Menu_PC" );
		} break;
		//}} kimhc // 2010-01-05 // PC�� �����̾� ����

	default:
		{
			return false;
		} break;
	}

	if ( pEquipButton == NULL )
		return false;

	pEquipButton->SetChecked(true);

	return true;
}

void CX2UIInventory::FeedPet()
{
	// ���� dungeon ready�����̸� ��ȯ/��ȯ����/��ȭ ���´�.
	//if( g_pData->GetPartyManager()->DoIHaveParty() == true )
	//{
	//	bool bMyReady = false;
	//	UidType iUnitUid = g_pData->GetMyUser()->GetSelectUnit()->GetUID();
	//	CX2PartyManager::PartyMemberData* pMemberData = g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( iUnitUid );
	//	if( NULL != pMemberData )
	//	{
	//		bMyReady = pMemberData->m_bGameReady;
	//	}
	//	if( bMyReady == true )
	//	{
	//		return;
	//	}
	//}

	if( g_pData->GetPetManager() != NULL )
	{

		CX2State*	pState	= static_cast< CX2State* >( g_pMain->GetNowState() );
		CX2Cursor*	pCursor = pState->GetCursor();

		if( pCursor != NULL && pCursor->GetCurorState() == CX2Cursor::XCS_PET_FOOD )
		{
			pCursor->ChangeCursorState(CX2Cursor::XCS_NORMAL);
			return;
		}

		if( pCursor != NULL )
			pCursor->ChangeCursorState( CX2Cursor::XCS_PET_FOOD );

		if( g_pMain->GetInfoBox() == NULL )
			g_pMain->KTDGUIInfoBox( D3DXVECTOR2( 250, 200 ), GET_STRING( STR_ID_5365 ), pState );

		m_pPetFoodSlot = NULL;
	}
}

void CX2UIInventory::SetEnableFeedPet(bool bVal)
{
	if( m_pDLGUIInventory == NULL )
		return;

	CKTDGUIButton *pButton = (CKTDGUIButton*)m_pDLGUIInventory->GetControl(L"_Food");
	if( pButton != NULL )
	{
		pButton->SetEnable(bVal);
	}
}

void CX2UIInventory::HideSkillNote(bool bVal)
{
	if( m_pDLGUIInventory == NULL )
		return;

	CKTDGUIStatic *pStatic = (CKTDGUIStatic*)m_pDLGUIInventory->GetControl(L"Noactive");
	if( pStatic != NULL )
	{
		pStatic->GetPictureIndex(1)->SetShow(!bVal);
	}

	CKTDGUIButton *pButton = (CKTDGUIButton*)m_pDLGUIInventory->GetControl(L"_Note");
	if( pButton != NULL )
	{
		pButton->SetShowEnable(!bVal, !bVal);
	}

	// ����ǳ�Ʈ ��ư�� �������� ��� �����ֱ� ��ư�� �̵���Ų��.
	bool bMove = false;
	if( pStatic->GetPictureIndex(1)->GetShow() == false )
	{
		bMove = true;
	}

	if( pStatic != NULL )
	{
		D3DXVECTOR2 vPosNote = pStatic->GetPictureIndex(1)->GetOriginalPos();
		D3DXVECTOR2 vPosFood = pStatic->GetPictureIndex(2)->GetOriginalPos();
		if( bMove == true )
		{
			vPosFood.x = vPosNote.x;
		}
		pStatic->GetPictureIndex(2)->SetPos( vPosFood );
	}

	CKTDGUIButton *pButtonFood = (CKTDGUIButton*)m_pDLGUIInventory->GetControl(L"_Food");
	if( pButtonFood != NULL )
	{
		if( bMove == true )
			pButtonFood->SetOffsetPos( D3DXVECTOR2(-81.f, 0.f) );
		else
			pButtonFood->SetOffsetPos( D3DXVECTOR2(0.f, 0.f) );
	}

}

void CX2UIInventory::SetEnableSkillNote(bool bVal)
{
	if( m_pDLGUIInventory == NULL )
		return;

	bool bShow = true;
	CKTDGUIStatic *pStatic = (CKTDGUIStatic*)m_pDLGUIInventory->GetControl(L"Noactive");
	if( pStatic != NULL )
	{
		bShow = pStatic->GetPictureIndex(1)->GetShow();
	}

	if( bShow == true )
	{
		bShow = bVal;
	}

	CKTDGUIButton *pButton = (CKTDGUIButton*)m_pDLGUIInventory->GetControl(L"_Note");
	if( pButton != NULL )
	{
		pButton->SetEnable(bShow);
	}

	if( pStatic->GetPictureIndex(1)->GetShow() == true || pButton->GetShow() == true )
	{
		CKTDGUIButton *pButtonFood = (CKTDGUIButton*)m_pDLGUIInventory->GetControl(L"_Food");
		if( pButtonFood != NULL )
		{
			pButtonFood->SetOffsetPos( D3DXVECTOR2(0.f, 0.f) );
		}
	}
}

bool CX2UIInventory::IsPetInventoryExist()
{
	// oasis907 : ����� [2010.9.11] �� ���� ��� ����ó�� ���⼭ �ѹ���
	if(g_pData->GetPetManager()->GetMyPet() == NULL)
	{
		return false;
	}

	// oasis907 : ����� [2010.9.13] // �� ������ ���� �� �κ��丮 �ȿ�����
	int MyPetID = g_pData->GetPetManager()->GetMyPet()->GetPetInfo().m_PetId;
	int EvolutionStep = g_pData->GetPetManager()->GetMyPet()->GetPetInfo().m_Evolution_Step;

	int Invenslot_Size = g_pData->GetPetManager()->GetPetInventorySlotSize((CX2PetManager::PET_UNIT_ID) MyPetID, EvolutionStep);

	if(Invenslot_Size == 0)
	{
		return false;
	}
	return true;
}

void CX2UIInventory::UpdateButtonPetInventory()
{
	CKTDGUIButton* pPetInventoryButton = static_cast< CKTDGUIButton* >( m_pDLGUIInventory->GetControl( L"PetInventory" ) );
	if(IsPetInventoryExist() == false)
	{
		pPetInventoryButton->SetShowEnable( false, false );
	}
	else if(g_pData->GetUIManager()->PetInventoryForceOpenCondition() == true)
	{
		pPetInventoryButton->SetShowEnable( true, true );
	}
	else
	{
		pPetInventoryButton->SetShowEnable( true, false );
	}
	return;
}

void CX2UIInventory::InitItemObtainResult(const WCHAR* pFileName, std::map< int, int >& mapInsertedItem )
{
	m_pDLGResolveItemResult = new CKTDGUIDialog( g_pMain->GetNowState(), pFileName );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGResolveItemResult );

	//������ ȹ�� ���â�� ��ġ ����
	m_pDLGResolveItemResult->SetPos( D3DXVECTOR2( 320, 230 ) );

	CKTDGUIButton* pControl = (CKTDGUIButton*) m_pDLGResolveItemResult->GetControl( L"Button_OK" );
	if ( pControl != NULL )
	{
		pControl->RequestFocus();
	}

	CKTDGUIStatic* pStaticSlot = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_Slot_Pic" );
	CKTDGUIStatic* pStaticName = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_SlotName" );
	CKTDGUIStatic* pStaticNum = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_SlotNum" );

	//������ ȹ�� ���â�� ������ �̹����� ��Ʈ�� �ʱ�ȭ
	for ( int slotNum = 0; slotNum < RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM; ++slotNum )
	{
		if ( pStaticSlot != NULL && pStaticSlot->GetPicture(slotNum) != NULL )
		{
			pStaticSlot->GetPicture(slotNum)->SetTex( L"DLG_Common_New_Texture02.dds", L"item_slot1" );
		}

		pStaticName->GetString(slotNum*2)->msg = L"";
		pStaticName->GetString(slotNum*2 + 1)->msg = L"";

		if ( pStaticNum != NULL && pStaticNum->GetString(slotNum) != NULL )
		{
			pStaticNum->GetString(slotNum)->msg = L"";
		}
	}


	//ȹ���� ���������� ������ ���� ����
	int index = 0;
	std::map< int, int >::iterator it = mapInsertedItem.begin();
	while( it != mapInsertedItem.end() )
	{
		int resultItemID = it->first;

		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
		if ( pItemTemplet != NULL )
		{
			if ( pStaticSlot != NULL && pStaticSlot->GetPicture(index) != NULL )
			{
                const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
				if ( pwszShopImage[0] != NULL )
					pStaticSlot->GetPicture(index)->SetTex( pwszShopImage );
				else
					pStaticSlot->GetPicture(index)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

			bool bCheckTwoLine = false;
			wstring wstrFullName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetFullName_(), 145, XUF_DODUM_13_SEMIBOLD );
			if ( wstrFullName.find( L"\n") != -1 )
				bCheckTwoLine = true;

			if(pStaticName->GetString(index*2) != NULL)
			{
				if ( bCheckTwoLine == false )
				{
					pStaticName->GetString(index*2)->msg = wstrFullName;
					pStaticName->GetString(index*2+1)->msg = L"";
				}
				else
				{
					pStaticName->GetString(index*2)->msg = L"";
					pStaticName->GetString(index*2+1)->msg = wstrFullName;
				}
			}
			if ( pStaticNum != NULL && pStaticNum->GetString(index) != NULL )
			{
				wstring wstrCount = GET_REPLACED_STRING( ( STR_ID_865, "i", it->second) );
				pStaticNum->GetString(index)->msg = wstrCount.c_str();
			}
		}
		// �������� ���� ���
		else
		{
			pStaticSlot->GetPicture(index)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			pStaticName->GetString(index)->msg = L"";
			pStaticNum->GetString(index)->msg = L"";
		}
		++it;
		++index;
	}
}
void CX2UIInventory::InitItemResolveResult(const WCHAR* pFileName, std::map< int, int >& mapInsertedItem )
{
	m_pDLGResolveItemResult = new CKTDGUIDialog( g_pMain->GetNowState(), pFileName );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGResolveItemResult );
	//������ ȹ�� ���â�� ��ġ ����
	m_pDLGResolveItemResult->SetPos( g_pMain->GetWindowPos( D3DXVECTOR2( 446, 376 ), D3DXVECTOR2( 222, 288) ) );

	CKTDGUIButton* pControl = (CKTDGUIButton*) m_pDLGResolveItemResult->GetControl( L"Button_OK" );
	if ( pControl != NULL )
	{
		pControl->RequestFocus();
	}

	CKTDGUIStatic* pStaticSlot = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_Slot_Pic" );
	CKTDGUIStatic* pStaticName = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_SlotName" );
	CKTDGUIStatic* pStaticNum = (CKTDGUIStatic*)m_pDLGResolveItemResult->GetControl( L"Static_SlotNum" );
	CKTDGUIButton* pStaticToolTip[RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM] = { 0, };
	//������ ȹ�� ���â�� ������ �̹����� ��Ʈ�� �ʱ�ȭ
	for ( int slotNum = 0; slotNum < RESOLVE_RESULT_ITEM_LIST_MAX_SLOT_NUM; slotNum++ )
	{
		if ( pStaticSlot != NULL && pStaticSlot->GetPicture(slotNum) != NULL )
		{
			pStaticSlot->GetPicture(slotNum)->SetTex( L"DLG_Common_New_Texture02.dds", L"item_slot1" );
		}
		if ( pStaticName != NULL && pStaticName->GetString(slotNum) != NULL )
		{
			pStaticName->GetString(slotNum)->msg = L"";
		}
		if ( pStaticNum != NULL && pStaticNum->GetString(slotNum) != NULL )
		{
			pStaticNum->GetString(slotNum)->msg = L"";
		}
		pStaticToolTip[slotNum] =
			static_cast< CKTDGUIButton* >( m_pDLGResolveItemResult->GetControl( GET_REPLACED_STRING( ( STR_ID_3738, "Li", std::wstring( L"DungeonDescriptionToolTip" ), slotNum + 1 ) ) ) );

		if( NULL != pStaticToolTip[slotNum] )
		{
			pStaticToolTip[slotNum]->SetShowEnable( false, false );
		}
	}

	//ȹ���� ���������� ������ ���� ����
	int index = 0;
	std::map< int, int >::iterator it = mapInsertedItem.begin();
	while ( it != mapInsertedItem.end() )
	{
		int resultItemID = it->first;

		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
		if ( pItemTemplet != NULL )
		{
			if ( pStaticSlot != NULL && pStaticSlot->GetPicture(index) != NULL )
			{
                const wchar_t* pwszShopImage = pItemTemplet->GetShopImage();
				if ( pwszShopImage[0] != NULL )
					pStaticSlot->GetPicture(index)->SetTex( pwszShopImage );
				else
					pStaticSlot->GetPicture(index)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}

			if ( pStaticName != NULL && pStaticName->GetString(index) != NULL )
			{
				bool bEllipse = false;
				wstring wstrName = CWordLineHandler::CutStringWithEllipse(pItemTemplet->GetFullName_(), 140, XUF_DODUM_13_SEMIBOLD, 1, bEllipse);
				pStaticName->GetString(index)->msg = wstrName;

				if(bEllipse == true)
				{
					wstring wstrFullName = CWordLineHandler::GetStrByLineBreakInX2Main( pItemTemplet->GetFullName_(), 145, XUF_DODUM_13_SEMIBOLD );
					pStaticToolTip[index]->SetGuideDesc(wstrFullName.c_str());
					pStaticToolTip[index]->SetShowEnable(true, true);
				}
				else
				{
					pStaticToolTip[index]->SetShowEnable(false, false);
				}
                pStaticName->GetString(index)->msg = wstrName;
			}
			if ( pStaticNum != NULL && pStaticNum->GetString(index) != NULL )
			{
				wstring wstrCount = GET_REPLACED_STRING( ( STR_ID_865, "i", it->second) );
				pStaticNum->GetString(index)->msg = wstrCount.c_str();
			}
		}
		// �������� ���� ���
		else
		{
			pStaticSlot->GetPicture(index)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			pStaticName->GetString(index)->msg = L"";
			pStaticNum->GetString(index)->msg = L"";
		}
		++it;
		++index;
	}
}


void CX2UIInventory::UseDefenceDungeonPotion( const UidType /*uidItem_*/, const int /*iItemId_*/ )
{

}

void CX2UIInventory::SetEDString(int iED_)
{
	if( false == m_bShow )
		return;

	if( NULL != m_pDLGUIInventory )
	{
		CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDLGUIInventory->GetControl(L"Static_ED"));
		if( NULL != pStatic && NULL != pStatic->GetString(0))
		{
			pStatic->GetString(0)->msg = g_pMain->GetEDString(iED_);
		}
	}
}
void CX2UIInventory::SetAPString(int iAP_)
{
	if( false == m_bShow )
		return;

	if( NULL != m_pDLGUIInventory )
	{
		CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>(m_pDLGUIInventory->GetControl(L"Static_AP"));
		if( NULL != pStatic && NULL != pStatic->GetString(0))
		{
			pStatic->GetString(0)->msg = g_pMain->GetEDString(iAP_);
		}
	}
}

void CX2UIInventory::UseWarpItem( CX2SlotItem* pItemSlot_, bool bIsEdConsumption_ )
{
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_SQUARE_GAME:
	case CX2Main::XS_VILLAGE_MAP:
	case CX2Main::XS_BATTLE_FIELD:
		{
			/// ���� ���̵� �ʵ� �ȿ����� ����� �� ����.
			if ( NULL != g_pData && g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
			{
				m_WarpItem = 0;
				g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4495 ),g_pMain->GetNowState() );
				break;
			}

			m_WarpItem = pItemSlot_->GetItemUID();
			m_bIsEdConsumptionWarp = bIsEdConsumption_;
			CreateWarpDest();
		} break;

	default:
		{
			// ����/�ŷ�����/��Ʋ�ʵ� �̿ܿ����� ���ȵ�...
			m_WarpItem = 0;
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4495 ),g_pMain->GetNowState() );
		} break;
	}	/// switch
}
void CX2UIInventory::UseWarpPopup(bool bIsEdConsumption_)
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
			if( NULL !=  g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit())
			{
				float fLevel = static_cast<float>(g_pData->GetSelectUnitLevel());
				if( fLevel > 0 )
				{
					int iWarpCommission = static_cast<int>((fLevel/60) * (iBasePrice + static_cast<int>(iBasePrice * fDecreaseFactor) * (iWarpDistance-1) )); //�̵� ������

					g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
						GET_REPLACED_STRING((STR_ID_20266, "iL", iWarpCommission, strVillageName)),
						UIM_WARP_DEST_OK, g_pMain->GetNowState() );
				}
			}
		}
		else
		{
			g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
				GET_REPLACED_STRING((STR_ID_20297, "L", strVillageName)),
				UIM_WARP_DEST_OK, g_pMain->GetNowState() );
		}
	}
}

void CX2UIInventory::UseUSCJobItem( CX2SlotItem* /*pItemSlot_*/ )
{
	switch ( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_VILLAGE_MAP:
		{
			CreateSecondJobDest();
		} break;
	default:
		{
			g_pMain->KTDGUIOKMsgBox ( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_4495 ),g_pMain->GetNowState() );
		} break;
	}
}

void CX2UIInventory::UseUSCJobPopup()
{
	//���� �۾� �Ѵ�. �� Ŭ������ ���� �Ͻðڽ��ϱ�? ����ϴ�.
	if( NULL != g_pData && NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit()->GetClass() )
	{
		CX2Unit::UNIT_CLASS cUnitClass = static_cast<CX2Unit::UNIT_CLASS>(m_iSelectedUSCJob);
		wstring strClassName = g_pData->GetUnitManager()->GetUnitTemplet( cUnitClass )->m_Name;


		if( 0 == strClassName.length() )
			return;

			g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(-999,-999),
				GET_REPLACED_STRING((STR_ID_29778, "L", strClassName)),
				UIM_UNLIMITED_SECOND_CHANGE_JOB_OK, g_pMain->GetNowState() );

	}
}

void CX2UIInventory::SetShowInventoryPageUI( int iMaxPageNum /*= 0*/ )
{
	const int iMaxCreatePageNum = 4;

	if( iMaxCreatePageNum < iMaxPageNum )
		iMaxPageNum = iMaxCreatePageNum;

	CKTDGUIRadioButton* pRadio_Page1	= static_cast<CKTDGUIRadioButton*>( m_pDLGUIInventory->GetControl( L"Inventory_page1" ) );	/// ������ 1
	CKTDGUIRadioButton* pRadio_Page2	= static_cast<CKTDGUIRadioButton*>( m_pDLGUIInventory->GetControl( L"Inventory_page2" ) );	/// ������ 2
	CKTDGUIRadioButton* pRadio_Page3	= static_cast<CKTDGUIRadioButton*>( m_pDLGUIInventory->GetControl( L"Inventory_page3" ) );	/// ������ 3
	CKTDGUIRadioButton* pRadio_Page4	= static_cast<CKTDGUIRadioButton*>( m_pDLGUIInventory->GetControl( L"Inventory_page4" ) );	/// ������ 4

	if( NULL != pRadio_Page1 &&
		NULL != pRadio_Page2 &&
		NULL != pRadio_Page3 &&
		NULL != pRadio_Page4 )
	{

		pRadio_Page1->SetShow( true );
		pRadio_Page2->SetShow( true );
		pRadio_Page3->SetShow( true );
		pRadio_Page4->SetShow( true );

		switch( iMaxPageNum )
		{
		case 1:
			{
				pRadio_Page1->SetShow( false );
				pRadio_Page2->SetShow( false );
			}
		case 2:
			{
				pRadio_Page3->SetShow( false );
			}
		case 3:
			{
				pRadio_Page4->SetShow( false );
			} break;
		default: break;
		}
	}
}

bool CX2UIInventory::UpdatePageUI( int iPage /*= 1*/ )
{
	CKTDGUIRadioButton* pButtonPage1 = static_cast<CKTDGUIRadioButton*>(m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_page1" ));
	CKTDGUIRadioButton* pButtonPage2 = static_cast<CKTDGUIRadioButton*>(m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_page2" ));
	CKTDGUIRadioButton* pButtonPage3 = static_cast<CKTDGUIRadioButton*>(m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_page3" ));
	CKTDGUIRadioButton* pButtonPage4 = static_cast<CKTDGUIRadioButton*>(m_pDLGUIInventory->GetRadioButton_LUA( "Inventory_page4" ));

	switch( iPage )
	{
	case 1:
		if( pButtonPage1 != NULL ) pButtonPage1->SetChecked( true );
		break;
	case 2:
		if( pButtonPage2 != NULL ) pButtonPage2->SetChecked( true );
		break;
	case 3:
		if( pButtonPage3 != NULL ) pButtonPage3->SetChecked( true );
		break;
	case 4:
		if( pButtonPage4 != NULL ) pButtonPage4->SetChecked( true );
		break;
	}

	return true;
}

int CX2UIInventory::GetPageNumBySlotID( int iSlotID )
{
	if( iSlotID < 0 || iSlotID >= MY_INVENTORY_SHOW_MAX_SLOT_NUM )
		return -1;

	return static_cast<int>(iSlotID / MY_INVENTORY_SHOW_SLOT_NUM_PER_PAGE) + 1;
}

int CX2UIInventory::GetMaxLineNumPerCategory( CX2Inventory::SORT_TYPE InventorySortType_ )		/// ���� ī�װ����� �ִ� ���� �� �� ��ȯ
{
	switch( InventorySortType_ )
	{
	case CX2Inventory::ST_SPECIAL:
		{
			return MY_INVENTORY_SPECIAL_MAX_SLOT_WIDTH;		/// 8��
		} break;
	case CX2Inventory::ST_AVARTA:
		{
			return MY_INVENTORY_AVARTA_MAX_SLOT_WIDTH;		/// 21��
		} break;
	case CX2Inventory::ST_PC_BANG:
		{
			return MY_INVENTORY_PC_BANG_MAX_SLOT_WIDTH;		/// 3��
		} break;
	default:
		{
			return MY_INVENTORY_MAX_SLOT_HEIGHT;			/// 7��
		} break;
	}
}


/** function : SetPetAutoLooting
	brief : ���� �ݱ� ���� ��ų Ȱ��ȭ ������ ��� ���� ��,
	          ���� ������ �����Ű�� �Լ�
	param : uiPetUID_ : ���� ��ų ���� UID
*/
void CX2UIInventory::SetPetAutoLooting( UidType uiPetUID_ )
{
	if( uiPetUID_ <= 0 )
		return;

	if( NULL == g_pData->GetPetManager() )
		return;

	CX2PET* pMyPet = g_pData->GetPetManager()->GetPetByUid( uiPetUID_ );
	if( NULL != pMyPet )
	{ //������ �ݱ� ��ų Ȱ��ȭ
		pMyPet->GetPetInfo().m_bIsDropItemPickup = true;
	}

	if( NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIPetList() )
	{
		CX2UIPetInfo* pUIPetInfo = g_pData->GetUIManager()->GetUIPetList();
		//UI���� ��� �ϰ� �ִ� �� ���� ����
		pUIPetInfo->SetPetAutoLooting( uiPetUID_, true);

		//ĳ���Ͱ� ������ �ִ� �� ���� ����
		if( NULL != g_pData->GetMyUser() &&
			NULL != g_pData->GetMyUser()->GetSelectUnit() )
		{
			g_pData->GetMyUser()->GetSelectUnit()->SetPetAutoLooting( uiPetUID_, true);
		}

		if( true == pUIPetInfo->GetShow() )
		{
			pUIPetInfo->SetShow( false );
		}
	}

	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMyUnit() )
	{
		g_pX2Game->GetMyUnit()->UpdateSocketDataAndEnchantData();
		g_pX2Game->GetMyUnit()->UpdatePassiveAndActiveSkillState();
	}

	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2( 250, 300 ), GET_STRING( STR_ID_21670 ), g_pMain->GetNowState() );
}

void CX2UIInventory::UseTransformPetItem()
{
	if( m_UseTransformdItemUID == 0 || m_UseTransformItemPetUID == 0 )
	{
		return;
	}

	Handler_EGS_USE_ITEM_IN_INVENTORY_REQ( m_UseTransformdItemUID, m_UseTransformItemPetUID );

	m_UseTransformdItemUID = 0;
	m_UseTransformItemPetUID = 0;
}


/** @function	: OpenScriptFile
	@brief		: ������ ���� ��ũ��Ʈ ���� �Լ�
	@param		: ��ũ��Ʈ ���ϸ� ( IceHeaterEvent.lua )
*/
void CX2UIInventory::OpenScriptFile( const WCHAR* szScriptFileName_ )
{
	lua_tinker::decl( g_pKTDXApp->GetLuaBinder()->GetLuaState(),  "g_pUIInventory", this );

    if ( g_pKTDXApp->LoadLuaTinker( szScriptFileName_ ) == false )
    {
		ASSERT( !"DoMemory doesn't work!" );
		ErrorLogMsg( XEM_ERROR2, szScriptFileName_ );
    }
}

/** @function	: AddIceHeaterEventInfo_LUA
	@brief		: ������ ��� ���� ���� ��ũ��Ʈ �Ľ� �Լ�
*/
void CX2UIInventory::AddIceHeaterEventInfo_LUA()
{
	KLuaManager luaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState() );

	IceHeaterEventData* pIceHeaterEventData = new IceHeaterEventData;

	/// ����Ʈ ���ڴ� �⺻ ������ ������ ����
	RANDOM_ITEM_OPEN_EVENT_TYPE eIceHeaterEnum = RIOET_NONE;			/// ������ ���̵�

	LUA_GET_VALUE_ENUM( luaManager, "ICE_HEATER_ID",					eIceHeaterEnum,
						RANDOM_ITEM_OPEN_EVENT_TYPE,					RIOET_ICE_HEATER										);

	/// ������ ���� �� ����Ʈ			( UIMajorParticle.txt )
	LUA_GET_VALUE(		luaManager, "WAIT_UI_EFFECT_SET_NAME",			pIceHeaterEventData->m_wstrWaitUIEffectSetName,			L"EffectSet_Ice_Heater_Wait"	);
	/// ������ ���� ����Ʈ				( UIEffectSet.lua )
	LUA_GET_VALUE(		luaManager, "PLAY_UI_EFFECT_SET_NAME",			pIceHeaterEventData->m_wstrPlayUIEffectSetName,			L"EffectSet_Ice_Heater_Start"	);

	/// ����� ���� ������ �Ž� �÷��̾�		( UIMajorXMeshPlayer.txt )
	LUA_GET_VALUE(		luaManager, "WAIT_MESH_PLAYER_NAME",			pIceHeaterEventData->m_wstrWaitMeshPlayerName,			L"FireMachineWait"				);
	/// ����� ����� ������ �Ž� �÷��̾�		( UIMajorXMeshPlayer.txt )
	LUA_GET_VALUE(		luaManager, "PLAY_MESH_PLAYER_NAME",			pIceHeaterEventData->m_wstrPlayMeshPlayerName,			L"FireMachineStart"				);

	/// ������ ���̾�α׿� ǥ�õ� ������ �̸� �ؽ�ó
	LUA_GET_VALUE(		luaManager, "MAIN_DIALOG_TITLE_TEXTURE",		pIceHeaterEventData->m_wstrMainDialogTitleTexture,		L"DLG_UI_Title02.tga"			);
	/// ������ ���̾�α׿� ǥ�õ� ������ �̸� �ؽ�ó Ű
	LUA_GET_VALUE(		luaManager, "MAIN_DIALOG_TITLE_TEXTURE_KEY",	pIceHeaterEventData->m_wstrMainDialogTitleTextureKey,	L"heater_title"					);

	/// ������ ���â ���̾�α׿� ǥ�õ� ������ �̸� �ؽ�ó
	LUA_GET_VALUE(		luaManager, "RESULT_DIALOG_TITLE_TEXTURE",		pIceHeaterEventData->m_wstrResultDialogTitleTexture,	L"DLG_UI_Title02.tga"			);
	/// ������ ���â ���̾�α׿� ǥ�õ� ������ �̸� �ؽ�ó Ű
	LUA_GET_VALUE(		luaManager, "RESULT_DIALOG_TITLE_TEXTURE_KEY",	pIceHeaterEventData->m_wstrResultDialogTitleTextureKey,	L"heater_title"					);

	/// ������ ���̾�α׿� ǥ�õ� ����ǥ �������� ������ X ��ġ
	LUA_GET_VALUE(		luaManager, "CAUTION_ICON_POSITION_X",			pIceHeaterEventData->m_fCautionIconOffSetPositionX,		-20.f							);
	/// ������ ���̾�α׿� ǥ�õ� ����ǥ �������� ������ Y ��ġ
	LUA_GET_VALUE(		luaManager, "CAUTION_ICON_POSITION_Y",			pIceHeaterEventData->m_fCautionIconOffSetPositionY,		0.f								);

	/// ������ ���̾�α��� ����ǥ ������ ���� ǥ�õ� ��Ʈ���� ������ X ��ġ
	LUA_GET_VALUE(		luaManager, "CAUTION_STRING_POSITION_X",		pIceHeaterEventData->m_fCautionStringOffSetPositionX,	0.f								);
	/// ������ ���̾�α��� ����ǥ ������ ���� ǥ�õ� ��Ʈ���� ������ Y ��ġ
	LUA_GET_VALUE(		luaManager, "CAUTION_STRING_POSITION_Y",		pIceHeaterEventData->m_fCautionStringOffSetPositionY,	0.f								);

	// ������ ���̾�α��� ����ǥ ������ ���� ǥ�õ� ��Ʈ��
	LUA_GET_VALUE(		luaManager, "CAUTION_STRING_NUM",				pIceHeaterEventData->m_iCautionStringNum,				STR_ID_EMPTY					);

	/// ������ ���� �����
	LUA_GET_VALUE(		luaManager, "ICE_HEATER_PLAY_SOUND",			pIceHeaterEventData->m_wstrIceHeaterPlaySound,			L"Fire_Machine_Start.ogg"		);

	/// ��� �ð� ( ������ ���� ~ ������� �ð� )
	LUA_GET_VALUE(		luaManager, "PLAY_TIME",						pIceHeaterEventData->m_fPlayTime,						10.f							);

	/// ���� ������� ����
	m_mapIceHeaterEventData.insert( std::make_pair( eIceHeaterEnum, pIceHeaterEventData ) );
}


/** @fucntion : IsRestoreItem
	@brief : "��� �������� �̰������·� �ǵ������ ������" ���� Ȯ�� �ϴ� �Լ�
*/
bool CX2UIInventory::IsRestoreEvaluateItem( const int ItemID_ ) const
{
	for(int i=0; i<std::ssize(ITEM_ID_RESTORE_ITEM_EVALUATE); ++i)
	{
		if( ItemID_ == ITEM_ID_RESTORE_ITEM_EVALUATE[i] )
			return true;
	}
	return false;
}
/** @fucntion : UseRestoreEvaluteItem
	@brief : �̶�Ŭ ť��(�̰��� ���·� �ǵ����� ������) ��� ���� ó��
*/
void CX2UIInventory::UseRestoreEvaluteItem( UidType iConsumeItemUID_ )
{
	if( iConsumeItemUID_ > 0 )
	{
		m_iConsumeItemUID = iConsumeItemUID_;

		// ���콺 Ŀ�� ����
		CX2State* pState = static_cast<CX2State*>(g_pMain->GetNowState());
		if( NULL != pState && NULL != pState->GetCursor() )
		{
			pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_ITEM_EVALUTE );
		}

		ChangeInventoryTab( CX2Inventory::ST_EQUIP );

		return;
	}
}
bool CX2UIInventory::Handler_EGS_RESTORE_ITEM_EVALUATE_REQ()
{
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2Item* pConsumeItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iConsumeItemUID );
		const CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iTargetItemUID );

		if ( pConsumeItem == NULL || pConsumeItem->GetItemTemplet() == NULL ||
			pTargetItem == NULL || pTargetItem->GetItemTemplet() == NULL )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_833 ), g_pMain->GetNowState() );
			return false;
		}

		if( false == g_pData->GetItemStatCalculator().IsRandomSocketOptionItem( pTargetItem->GetItemTemplet() ) )
		{
			// �̰��� ���·� �ǵ����Ⱑ �Ұ����� �����ۿ� ���� ���̵� �˾�
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_24614 ), g_pMain->GetNowState() );
			return false;
		}

		if( false == pTargetItem->GetIsEvaluation() )
		{
			// �̰��� ���·� �ǵ����Ⱑ �Ұ����� �����ۿ� ���� ���̵� �˾�
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_24614 ), g_pMain->GetNowState() );
			return false;
		}
	}

	KEGS_RESTORE_ITEM_EVALUATE_REQ kPacket;
	kPacket.m_iSupportItemUID = m_iConsumeItemUID;
	kPacket.m_iTargetItemUID = m_iTargetItemUID;

	m_iConsumeItemUID = 0;
	m_iTargetItemUID = 0;

	g_pData->GetServerProtocol()->SendPacket( EGS_RESTORE_ITEM_EVALUATE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESTORE_ITEM_EVALUATE_ACK );

	return true;
}
bool CX2UIInventory::Handler_EGS_RESTORE_ITEM_EVALUATE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_RESTORE_ITEM_EVALUATE_ACK kPacket;
	DeSerialize( pBuff, &kPacket );

	if( true == g_pMain->DeleteServerPacket( EGS_RESTORE_ITEM_EVALUATE_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kPacket.m_iOK ) )
		{
			if( NULL != g_pData &&
				NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
 				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kPacket.m_vecInventorySlotInfo);
			}

			UpdateInventorySlotList( kPacket.m_vecInventorySlotInfo );
 			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_835 ), g_pMain->GetNowState() );

			return true;
		}
	}
	return false;
}
bool CX2UIInventory::Handler_EGS_ITEM_EVALUATE_REQ()
{
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2Item* pTargetItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_iTargetItemUID );

		if( pTargetItem == NULL || pTargetItem->GetItemTemplet() == NULL )
		{
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_833 ), g_pMain->GetNowState() );
			return false;
		}

		if( false == g_pData->GetItemStatCalculator().IsRandomSocketOptionItem( pTargetItem->GetItemTemplet() ) )
		{
			// ������ �Ұ����� �����ۿ� ���� ���̵� �˾�
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_833 ), g_pMain->GetNowState() );
			return false;
		}
	}

	KEGS_ITEM_EVALUATE_REQ kPacket;
	kPacket.m_iItemUID = m_iTargetItemUID;
	m_iTargetItemUID = 0;

	g_pData->GetServerProtocol()->SendPacket( EGS_ITEM_EVALUATE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ITEM_EVALUATE_ACK );

	return true;
}
bool CX2UIInventory::Handler_EGS_ITEM_EVALUATE_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_ITEM_EVALUATE_ACK kPacket;
	DeSerialize( pBuff, &kPacket );

	if( true == g_pMain->DeleteServerPacket( EGS_ITEM_EVALUATE_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kPacket.m_iOK ) )
		{
			if( NULL != g_pData &&
				NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kPacket.m_vecInventorySlotInfo);
			}
			UpdateInventorySlotList( kPacket.m_vecInventorySlotInfo );
			g_pData->SetSelectUnitED( kPacket.m_iED );

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_25080 ), g_pMain->GetNowState() );
			return true;
		}
	}
	return true;
}
void CX2UIInventory::Handler_EGS_ITEM_CONVERT_REQ( UidType iItemUID, int iQuantity  )
{
	if( iItemUID < 0 )
		return;

	KEGS_ITEM_CONVERT_REQ kPacket;
	kPacket.m_iItemUID = iItemUID;
	kPacket.m_iQuantity = iQuantity;

	g_pData->GetServerProtocol()->SendPacket( EGS_ITEM_CONVERT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ITEM_CONVERT_ACK );
}
bool CX2UIInventory::Handler_EGS_ITEM_CONVERT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_ITEM_CONVERT_ACK kPacket;
	DeSerialize( pBuff, &kPacket );

	if( true == g_pMain->DeleteServerPacket( EGS_ITEM_CONVERT_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kPacket.m_iOK ) )
		{
			if( NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kPacket.m_vecInventorySlotInfo);
			}
			UpdateInventorySlotList( kPacket.m_vecInventorySlotInfo );
		}

		return true;
	}

	return true;
}

bool CX2UIInventory::CheckLevelIfLevelLimitEnchantCoupon ( const int iCouponItemID_ , const int iTargetItemLevel_ )
{
	for ( UINT i = 0; i < ARRAY_SIZE(HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_ID); i++ )
	{
		if ( iCouponItemID_ == HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_ID[i] )
		{
			if ( i * 2 >= ARRAY_SIZE (HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_LIMIT_LEVEL) )
			{
				ASSERT ( L"HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_ID Array Size Over !" );
				return true;
			}

			else if ( iTargetItemLevel_ > HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_LIMIT_LEVEL[2*i] &&
				 iTargetItemLevel_ <= HAVE_LIMIT_LEVEL_ENCHANT_COUPON_ITEM_LIMIT_LEVEL[2*i+1] )
			{
				return false;
			}

			else
			{
				return true;
			}
		}
	}


	return false;
}

void CX2UIInventory::OpenElBreakWindow( IN CX2Item* pItem_ )
{
	SAFE_DELETE_DIALOG( m_pDLGElBreak );

	if ( pItem_ == NULL )
		return;

	m_ElBreakItemUID	= pItem_->GetItemData().m_ItemUID;
	m_ElBreakItemCount	= 1;

	m_pDLGElBreak = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_UI_El_Break_Window.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGElBreak );

	//������ �̹��� �������ְ�.
	CKTDGUIStatic* pStaticItemImage = (CKTDGUIStatic*)m_pDLGElBreak->GetControl( L"ItemImage" );
	if ( pStaticItemImage != NULL && pStaticItemImage->GetPicture(0) != NULL )
	{
		pStaticItemImage->GetPicture(0)->SetTex( g_pData->GetItemManager()->GetItemTemplet( pItem_->GetItemData().m_ItemID )->GetShopImage() );
	}

	//������ �̸� �������ְ�.
	CKTDGUIStatic* pStaticItemName = (CKTDGUIStatic*)m_pDLGElBreak->GetControl( L"Identify_Window_Price" );
	if ( pStaticItemName != NULL )
	{
		pStaticItemName->SetString( 0, g_pData->GetItemManager()->GetItemTemplet( pItem_->GetItemData().m_ItemID )->GetName() );
	}

	CKTDGUIControl* pControl = m_pDLGElBreak->GetControl( L"Break_Tool" );
	if ( pControl != NULL )
		pControl->SetShow( true );

	UpdateElBreakDLG( false );
}

void CX2UIInventory::UpdateElBreakDLG( IN const bool bReadIME_ )
{
	CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItem( m_ElBreakItemUID );

	if ( pItem == NULL || pItem->GetItemTemplet() == NULL )
		return;

	if ( m_pDLGElBreak == NULL )
		return;

	const int MAGIC_MAX_EL_CHANGE_ONCE = 100;
	int nowQuantity = 0;
	nowQuantity = min( pItem->GetItemData().m_Quantity, MAGIC_MAX_EL_CHANGE_ONCE );

	// IME���� �� �޾ƿ���
	CKTDGUIIMEEditBox* pQuantity = (CKTDGUIIMEEditBox*) m_pDLGElBreak->GetControl( L"IME_Identify_Window" );
	if( bReadIME_ )
	{
		m_ElBreakItemCount = g_pMain->GetEDFromString( pQuantity->GetText() );
	}

	if ( m_ElBreakItemCount > nowQuantity )
		m_ElBreakItemCount = nowQuantity;

	// Ȯ�����ְ�
	if ( m_ElBreakItemCount <= 0 )
	{
		if( bReadIME_ )				// �Է�â���� �޾ƿ� ���
			m_ElBreakItemCount = 0;
		else						// << �� ���� ��� : 0 �Ʒ��� max�� ��ȯ��Ų��.
			m_ElBreakItemCount = nowQuantity;
	}

	WCHAR buff[256] = {0};
	_itow_s( m_ElBreakItemCount, buff, 10 );
	wstring wstrNum = buff;
	pQuantity->SetText( wstrNum.c_str() );
}

bool CX2UIInventory::Handler_EGS_ITEM_EXTRACT_REQ()
{
	KEGS_ITEM_EXTRACT_REQ kPacket;

	kPacket.m_iItemUID = m_ElBreakItemUID;
	kPacket.m_iQuantity = m_ElBreakItemCount;

	g_pData->GetServerProtocol()->SendPacket( EGS_ITEM_EXTRACT_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ITEM_EXTRACT_ACK );

	return true;
}

bool CX2UIInventory::Handler_EGS_ITEM_EXTRACT_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ITEM_EXTRACT_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if ( g_pMain->DeleteServerPacket( EGS_ITEM_EXTRACT_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			wstring itemName = g_pData->GetItemManager()->GetItemTemplet( CX2EnchantItem::ATI_HYPER_SKILL_STONE )->GetFullName_();
			itemName += L" ";
			itemName += g_pMain->GetEDString( m_ElBreakItemCount * 3 );
			itemName += GET_STRING( STR_ID_24 );
			itemName += GET_STRING( STR_ID_426 );
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), itemName.c_str(), g_pMain->GetNowState() );
			UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );
		}

		return true;
	}

	return false;
}

//#pragma region Good Elsword
//#pragma endregion Ȯ�� ���� ó�� ��Ŷ

bool CX2UIInventory::OnMouseWheel( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/ )
{
	// ���콺 Ŀ���� �κ��丮 ���� �ȿ� ���� ���� ó���ϱ�
	if( false == CX2UIInventory::GetIsMouseOver() )
		return true;

	if( NULL != g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFirstFrontModalDlg() )
		return true;

	if( NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUISkillNote() &&
		true == g_pData->GetUIManager()->GetUISkillNote()->GetShow() )
		return true;

	// ������ ������ �ʴ� ������ ��Ÿ�� �߰�
	if( m_fLeftMouseWhellCoolTime > 0.f )
		return true;

	short zDelta = static_cast<short>(HIWORD(wParam));
	m_SumDelta += zDelta;
	while (abs(m_SumDelta) >= WHEEL_DELTA)
	{
		m_fLeftMouseWhellCoolTime = 0.2f;
		const int iNowPageNum = GetNowInventoryPageNum( m_NowInventorySortType );

		int iChangePageNum = 1;
		// �� ��
		if(m_SumDelta>0)
		{
			iChangePageNum = max( (iNowPageNum - 1), 1 );
			m_SumDelta -= WHEEL_DELTA;
		}
		// �� �ٿ�
		else
		{
			/// �κ��丮 �ִ� ������ �� ����
			int iMaxPageNum = GetMaxLineNumPerCategory( m_NowInventorySortType ) / MY_INVENTORY_MAX_SLOT_HEIGHT;	/// ����( 56�� )���� �� �� ������ �� ����
			if( 0 < GetMaxLineNumPerCategory( m_NowInventorySortType ) % MY_INVENTORY_MAX_SLOT_HEIGHT )				/// �̿��� �� ä���� ���� �������� �ִ��� ����
				++iMaxPageNum;	/// ������ �� �߰�

			iChangePageNum = min( (iNowPageNum + 1) , iMaxPageNum );
			m_SumDelta += WHEEL_DELTA;
		}

		// ������ ���ٸ� ó������ �ʱ�
		if( iNowPageNum == iChangePageNum )
			return true;

		WCHAR wBuf[32];
		StringCchPrintfW( wBuf, ARRAY_SIZE(wBuf), L"Inventory_page%d", iChangePageNum );
		CKTDGUIRadioButton* pRadioButton =
			static_cast<CKTDGUIRadioButton*>( m_pDLGUIInventory->GetControl( wBuf ) );	/// ������ 1
		if( NULL != pRadioButton )
			pRadioButton->SetChecked(true);

		SetNowInventoryPageNum( m_NowInventorySortType, iChangePageNum );
		SetInventorySort( m_NowInventorySortType, iChangePageNum );

		break;
	}
	return true;
}
bool CX2UIInventory::GetIsMouseOver()
{
	if( false == GetShow() )
		return false;

	if( NULL != m_pDLGUIInventory )
		return m_pDLGUIInventory->GetIsMouseOver();

	return false;
}

/** @function : IsAddSocketSlotItem
	@brief : ���� ���� Ȯ�� ������ ���� Ȯ��
*/
bool CX2UIInventory::IsAddSocketSlotItem( const int ItemID_ ) const
{
	switch( ItemID_ )
	{
	case ITEM_ID_ADD_WEAPON_SOCKET_SLOT:
	case ITEM_ID_ADD_DEFENCE_SOCKET_SLOT:
		{
			return true;
		} break;
	default:
		return false;
		break;
	}
	return false;
}
/** @fucntion : UseAddSocketSlotItem
	@brief : ���� ���� Ȯ�� ������ ���
*/
void CX2UIInventory::UseAddSocketSlotItem( UidType iConsumeItemUID_ )
{
	if( iConsumeItemUID_ > 0 )
	{
		m_iConsumeItemUID = iConsumeItemUID_;

		// ���콺 Ŀ�� ����
		CX2State* pState = static_cast<CX2State*>(g_pMain->GetNowState());
		if( NULL != pState && NULL != pState->GetCursor() )
		{
			pState->GetCursor()->ChangeCursorState( CX2Cursor::XCS_ADD_SOCKET_SLOT );
		}

		ChangeInventoryTab( CX2Inventory::ST_EQUIP );

		return;
	}
}
void CX2UIInventory::Handler_EGS_SOCKET_EXPAND_ITEM_REQ()
{
	KEGS_SOCKET_EXPAND_ITEM_REQ kPacket;
	kPacket.m_iMaterialItemUID = m_iConsumeItemUID;
	kPacket.m_iTargetItemUID = m_iTargetItemUID;

	m_iConsumeItemUID = -1;
	m_iTargetItemUID = -1;

	g_pData->GetServerProtocol()->SendPacket( EGS_SOCKET_EXPAND_ITEM_REQ, kPacket );
 	g_pMain->AddServerPacket( EGS_SOCKET_EXPAND_ITEM_ACK );
}
bool CX2UIInventory::Handler_EGS_SOCKET_EXPAND_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_SOCKET_EXPAND_ITEM_ACK kPacket;
	DeSerialize( pBuff, &kPacket );

	if( true == g_pMain->DeleteServerPacket( EGS_SOCKET_EXPAND_ITEM_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kPacket.m_iOK ) )
		{
			if( NULL != g_pData &&
				NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList(kPacket.m_vecUpdatedInventorySlot);
			}
			UpdateInventorySlotList( kPacket.m_vecUpdatedInventorySlot );

 			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_835 ), g_pMain->GetNowState() );
			return true;
		}
	}
	return false;
}


void CX2UIInventory::OpenRandomItem( int iItemID, ITEM_OBTAIN_RESULT_TYPE eIORT )
{
	m_fCubeOpenTime = 3.f;
	m_RandomItemID = iItemID;
	m_bHideCubeOpneBar = g_pData->GetCubeOpenImageManager()->IsHideBarCube( iItemID );

	m_eItemObtainResultType = eIORT;
	m_bCubeJustOpen = true;
	SAFE_DELETE_DIALOG(m_pDLGCubeOpenEffect);
	m_pDLGCubeOpenEffect = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Cube_Open.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGCubeOpenEffect );
	m_pDLGCubeOpenEffect->SetFront(true);

	// oasis907 : ����� [2010.9.29] // ITEM ID �� ���� ���� ������ ť�� ���� UI
	CKTDGUIStatic* pStaticCubeOpenLoading = (CKTDGUIStatic*) m_pDLGCubeOpenEffect->GetControl( L"g_pStaticNote_Loading" );
	switch( iItemID )
	{
	case MEMO_RANDOM_CUBE_ITEM_ID:
	case CAPSULE_RANDOM_CUBE_ITEM_ID:
		pStaticCubeOpenLoading->GetPicture(0)->SetTex(L"DLG_UI_Common_Texture43.tga", L"Note");
		break;
		//{{ oasis907 : ����� [2011.3.29] // �ĸ��� ť�� ������
	case TREE_DAY_CUBE_ITEM_ID:
		{
			pStaticCubeOpenLoading->GetPicture(0)->SetTex(L"DLG_UI_Common_Texture50.tga", L"Note");
			m_pDLGCubeOpenEffect->AddDummyInt(1);
		}
		break;
		//}}
	case NEW_HENIR_CUBE_ITEM_ID:
		pStaticCubeOpenLoading->GetPicture(0)->SetTex(L"DLG_UI_Common_Texture59.tga", L"Note");
		break;
	default:
		pStaticCubeOpenLoading->GetPicture(0)->SetTex(L"DLG_UI_Common_Texture40.tga", L"Note");
		break;
	}

	// �ʱ� �̹����� �׷��ִµ�, �����Ӻκ꿡�� �ٷ� ��ü�� ���ִ�, ���⼭�� �̹��� ������ �����൵ ���� �ʳ�?
	if( g_pData->GetCubeOpenImageManager() != NULL && g_pData->GetCubeOpenImageManager()->IsEventCube( iItemID ) == true)
	{
		g_pData->GetCubeOpenImageManager()->SetStart( true);
		g_pData->GetCubeOpenImageManager()->SetSoundPlaying( true );
	}
	if ( m_pDLGRandomItem != NULL )
	{
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGRandomItem, NULL, false );
		m_pDLGRandomItem = NULL;
	}
	InvalidSlotDesc();
}

//{{ 2012. 10. 08  �ҷ��� ������
void CX2UIInventory::UpdateOpenHalloWeenItemAck()
{
	m_eRandomItemEventType = RIOET_NONE;

	// ������ ��������ִ� Attraction Item�� ���â�� ��Ȱ���Ѵ�.
	// �޽��� (�Ϸ���� CustomMsg)� ������ ���â�� �װ��� ������.
	// �����̶� ���ʽ� ������ ����ƽ�� �� ���̰� �����ָ� �� ��.

	// ���� â�� �����..
	if ( m_pDLGOpenAttraction != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGOpenAttraction, NULL, false );
	m_pDLGOpenAttraction = NULL;

	// ���� ������
	m_pDLGAttractionResult = new CKTDGUIDialog( g_pMain->GetNowState(), L"DLG_Charm_Item_Result_1slot.lua" );
	g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGAttractionResult );

	const wstring wstrResultDialogTitle		=
		NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrResultDialogTitleTexture : L"DLG_UI_Title02_B.tga";

	const wstring wstrResultDialogTitleKey	=
		NULL != m_pNowIceHeaterEventData ? m_pNowIceHeaterEventData->m_wstrResultDialogTitleTextureKey : L"heater_title";

	CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>( m_pDLGAttractionResult->GetControl( L"Static_Title" ) );
	if( NULL != pStatic )
	{
		if( NULL != pStatic->GetPicture(0) )
			pStatic->GetPicture(0)->SetTex( wstrResultDialogTitle.c_str(), wstrResultDialogTitleKey.c_str() );

		if( NULL != pStatic->GetPicture(1) )
			pStatic->GetPicture(1)->SetShow(false);
	}

	/// �� ���������, ���� ��ü ����
	m_pNowIceHeaterEventData = NULL;

	// ���ʽ� ������ ����ƽ �� ���̰� ���ְ�
	CKTDGUIStatic* pStatic_BonusMark = (CKTDGUIStatic*)m_pDLGAttractionResult->GetControl( L"Static_Bonus_Item_Mark" );
	pStatic_BonusMark->SetShow(false);

	if ( m_TempOpenRandomItemAck.m_mapInsertedItem.empty() == false )
	{
		// �ĸ��� �̺�Ʈ �������� �ִ� 2�� ���´� ( ��ȹ������ Ȯ���� ������ )
		std::map< int, int >::iterator i;

		bool bFirstItem = true;
		wstring getExplanation = L"";
		for ( i = m_TempOpenRandomItemAck.m_mapInsertedItem.begin(); i != m_TempOpenRandomItemAck.m_mapInsertedItem.end(); i++ )
		{
			int resultItemID = i->first;
			int resultNum	= i->second;
			const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( resultItemID );
			if ( pItemTemplet == NULL ) continue;

			CKTDGUIStatic* pStatic = NULL;
			if(bFirstItem)
			{
				// ù ��° ������
				pStatic = (CKTDGUIStatic*) m_pDLGAttractionResult->GetControl( L"Static_Item" );
				// [NOTE] ���â�� ������ �̸��� ���̾�α׸� �Ѿ�� ��찡 �־ ������(...) ����
				bool bEllipse = false;
				wstring wstrItemName = CWordLineHandler::CutStringWithEllipse(pItemTemplet->GetFullName_(), 290, pStatic->GetString(0)->fontIndex, 1, bEllipse);
				const wchar_t* pwszitemName = wstrItemName.c_str();
				if( resultNum == 1)
				{
					pStatic->GetString(0)->msg = pwszitemName;
				}
				else
				{
					pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_832, "Si", pwszitemName, resultNum ) );
				}
				bFirstItem = false;
			}
			else
			{
				// �ι�°
				pStatic = (CKTDGUIStatic*) m_pDLGAttractionResult->GetControl( L"Static_Bonus" );
				const wchar_t* pwszitemName = pItemTemplet->GetFullName_();
				pStatic->GetString(0)->msg = GET_REPLACED_STRING( ( STR_ID_832, "Si", pwszitemName, resultNum ) );
			}

			if( NULL != pStatic &&
				NULL != pStatic->GetPicture(0) &&
				false == pStatic->GetPicture(0)->SetTex(
				pItemTemplet->GetShopImage()
				) )
			{
				pStatic->GetPicture(0)->SetTex( L"HQ_Shop_Ui_Noimage.dds" );
			}
		}
	}

	UpdateInventorySlotList( m_TempOpenRandomItemAck.m_vecKInventorySlotInfo );
}

void CX2UIInventory::Handler_EGS_USE_PROPOSE_ITEM_REQ( UidType iItemUID, wstring wstrNickName )
{
	KEGS_USE_PROPOSE_ITEM_REQ kPacket;
	kPacket.m_iUsedItemUID = iItemUID;
	kPacket.m_wstrNickName = wstrNickName;

	CX2Main::TimedMessagePopUp::TimedPopupUserData userData;
	userData.iMyUID			= g_pData->GetMyUser()->GetSelectUnit()->GetUID();
	userData.iOpponentUID	= g_pData->GetMyUser()->GetSelectUnit()->GetUID();

	wstring wstrTextNotice= GET_REPLACED_STRING( ( STR_ID_24459, "L", wstrNickName ) );

	IF_EVENT_ENABLED( CEI_RELATIONSHIP_CODE_EVENT )
	{
		g_pMain->AddTimedMessagePopup( CX2Main::TimedMessagePopUp::MT_WAIT_CHOICE_COUPLE,
			CX2Main::TimedMessagePopUp::MBT_NO_BUTTON, userData, 10.0f,
			wstrTextNotice.c_str(),
			(CKTDXStage*) g_pMain->GetNowState(), -1, -1 );
	}

	g_pData->GetServerProtocol()->SendPacket( EGS_USE_PROPOSE_ITEM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_USE_PROPOSE_ITEM_ACK );
}

bool CX2UIInventory::Handler_EGS_USE_PROPOSE_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_USE_PROPOSE_ITEM_ACK kPacket;
	DeSerialize( pBuff, &kPacket );

	if( true == g_pMain->DeleteServerPacket( EGS_USE_PROPOSE_ITEM_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kPacket.m_iOK ) )
		{
			if( NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kPacket.m_vecKInventorySlotInfo );
			}
			UpdateInventorySlotList( kPacket.m_vecKInventorySlotInfo );
		}

		return true;
	}

	return true;
}

void CX2UIInventory::Handler_EGS_USE_DIVORCE_ITEM_REQ()
{
	if( g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().GetCouple() == true )
	{
		g_pData->GetServerProtocol()->SendID( EGS_USE_DIVORCE_ITEM_REQ );
		g_pMain->AddServerPacket( EGS_USE_DIVORCE_ITEM_ACK );
	}
	else
	{
		g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_28094 ), g_pMain->GetNowState() );
	}
}

bool CX2UIInventory::Handler_EGS_USE_DIVORCE_ITEM_ACK( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{
	KSerBuffer* pBuff = reinterpret_cast<KSerBuffer*>(lParam);
	KEGS_USE_DIVORCE_ITEM_ACK kPacket;
	DeSerialize( pBuff, &kPacket );

	if( true == g_pMain->DeleteServerPacket( EGS_USE_DIVORCE_ITEM_ACK ) )
	{
		if( true == g_pMain->IsValidPacket( kPacket.m_iOK ) )
		{
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().SetCouple( false );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().SetRelationTargetUserUID( 0 );
			g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData().SetRelationTargetUserNickname( L"" );

			if( NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				g_pData->GetMyUser()->GetSelectUnit()->AccessInventory().UpdateInventorySlotList( kPacket.m_vecUpdatedInventorySlot );
			}
			UpdateInventorySlotList( kPacket.m_vecUpdatedInventorySlot );

			OpenRandomItem( 60007792, IORT_CUBE_OPEN );

			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(-999,-999), GET_STRING( STR_ID_28093 ), g_pMain->GetNowState() );
			return true;
		}
	}

	return true;
}

bool CX2UIInventory::Handler_EGS_CUBE_IN_ITEM_MAPPING_NOT( HWND /*hWnd*/, UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam )
{

	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CUBE_IN_ITEM_MAPPING_NOT kEvent;
	DeSerialize( pBuff, &kEvent );


	std::map< int,std::map<int, kRandomItemMappingToInfoClient> >		m_mapUseOpenRandomID;		// ���콺 ���� Ŭ������ ������ ������ ť�� �����۰� �׷�ID����
	m_mapUseOpenRandomID.clear();
	m_mapUseOpenRandomID = kEvent.m_mapCubeInItemMapping;

	CX2ItemSlotManager::SetToolTipUseOpenRandomID(m_mapUseOpenRandomID);

	return true;
}
