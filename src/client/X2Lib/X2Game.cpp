#include "stdafx.h"
#include "./X2Game.h"






#define X2GAME_NPC_ROBUST_PACKET_SEND_INTERVAL  (3*70)


bool CX2Game::m_sbRenderNPCName = true;







#if X2_DEV_TOOLS
IMPLEMENT_HELP_SYSTEM( CX2Game )
{
	WCHAR   wbuffer[1024];
	int     ix = 16;
	int     iy = 16;
	StringCchPrintfW( wbuffer, ARRAY_SIZE(wbuffer), L"%S", __FUNCTION__ );

	if ( g_pMain && g_pX2Game )
	{
		g_pMain->GetFontForDebuf()->OutTextXY( ix, iy, wbuffer, D3DXCOLOR( 1, 1, 0, 1),
			CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
		g_pX2Game->Debug_RenderHelp( ix, iy );
	}//if
}
#endif // KHELP_SYSTEM_TEST



void CX2Game::Debug_RenderHelp( int ix_, int iy_ )
{
	int     ix = ix_ + 16;
	int     iy = iy_ + 16;

	wstringstream wstrm;


	if( NULL != GetFocusUnit() )
	{

		D3DXVECTOR3 vOriginalCameraPosition = GetX2Camera()->CalcNormalCameraPosition( GetFocusUnit(), g_pMain->GetGameOption().GetCameraDistance(), 0.f, 0.f, 200.f, 0.f );
		D3DXVECTOR3 vCurrentCameraPosition = GetX2Camera()->GetCamera().GetEye();
		D3DXVECTOR3 vCameraPositionDesplace = vCurrentCameraPosition - vOriginalCameraPosition;

		D3DXVECTOR3 vDirVector = GetFocusUnit()->GetDirVector();
		D3DXVECTOR3 vZVector = GetFocusUnit()->GetZVector();
		D3DXVec3Normalize( &vDirVector, &vDirVector );
		D3DXVec3Normalize( &vZVector, &vZVector );

		D3DXVECTOR3 vRelativeCameraPosition;
		vRelativeCameraPosition.x = D3DXVec3Dot( &vDirVector, &vCameraPositionDesplace );
		vRelativeCameraPosition.y = vCameraPositionDesplace.y;
		vRelativeCameraPosition.z = D3DXVec3Dot( &vZVector, &vCameraPositionDesplace );


		D3DXVECTOR3 vDistance = vCurrentCameraPosition - GetFocusUnit()->GetPos();
		float fCurrentCameraDistance = D3DXVec3Length( &vDistance );


		wstrm << L"CAMERA_DISTANCE: " << (int)fCurrentCameraDistance << std::endl;
		wstrm << L"CAMERA_DISPLACE: ( " << (int)vRelativeCameraPosition.x << ", "
			<< (int)vRelativeCameraPosition.y << ", "
			<< (int)vRelativeCameraPosition.z << " ), " << std::endl;

		wstrm << L"CAMERA_POSITION: ( " << (int)vCurrentCameraPosition.x << ", "
			<< (int) vCurrentCameraPosition.y << ", "
			<< (int) vCurrentCameraPosition.z << " ), " << std::endl;

		wstrm << std::endl;

	}




	for( UINT i=0; i<m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pUser = m_UserUnitList[i];
		if( NULL == pUser )
			continue;

		wstrm << L"idx: " << i << ", "
			<< L"uid: " << pUser->GetUnitUID() << ", "
			<< L"pos: ("
			<< (int)pUser->GetPos().x << ", "
			<< (int)pUser->GetPos().y << ", "
			<< (int)pUser->GetPos().z << "), "
			<< L"HP: " << pUser->GetNowHp() << ", "
			<< L"상태:" << (int) pUser->GetGameUnitState() << ", "
			<< std::endl;
	}


	for( UINT i=0; i<m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = m_NPCUnitList[i];
		if( NULL == pNPC )
			continue;

        std::wstring    wstrTemp;

		wstrm << L"idx: " << i << ", "
			<< pNPC->GetNPCTemplet().m_Name << L", "
			<< L"id: " << pNPC->GetNPCTemplet().m_nNPCUnitID << L", "
			<< L"uid: " << pNPC->GetUID() << L", "
			<< L"pos: ("
			<< (int)pNPC->GetPos().x << ", "
			<< (int)pNPC->GetPos().y << ", "
			<< (int)pNPC->GetPos().z << "), "
			<< L"HP: " << pNPC->GetNowHp() << ", "
			<< L"상태:" << ConvertUtf8ToWCHAR( wstrTemp, pNPC->GetStateNameByStateID( static_cast<int>(pNPC->GetGameUnitState() ) ) ) << ", "
			<< std::endl;
	}

	g_pMain->GetFontForDebuf()->OutTextMultiline( ix, iy, wstrm.str().c_str(), D3DXCOLOR( 1, 1, 0, 1),
		CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ) );
	iy += 16 * 3;
}




CX2Game::CX2Game() :
//{{ kimhc // 게임 로딩이 끝날때 까지 ESC 처리 안되게 하는 작업
m_bCanUseESC( false ),
//}} kimhc // 게임 로딩이 끝날때 까지 ESC 처리 안되게 하는 작업
m_GameType( GT_NONE ),
m_GameState( GS_INIT ),
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
m_uidMySlotUID( 0 ),
m_uidHostSlotUID( 0 ),
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//m_bHost( false ),
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
m_pRoom( NULL ),
m_fTime( 0.f ),
m_fElapsedTime( 0.f ),
m_fGameTime( 0.f ),
m_bGameWait( false ),
m_optrMyUnit(),
m_optrFocusUnit(),
m_fFocusTime( 0.f ),
m_pWorld( NULL ),
m_pCamera( NULL ),
m_pDamageEffect( NULL ),
//m_pMajorParticle( NULL ),
//m_pMinorParticle( NULL ),
//m_pMajorXMeshPlayer( NULL ),
//m_pMinorXMeshPlayer( NULL ),
m_bBGMOn( true ),
m_bLastKillCheck( false ),
m_fLastKillWaitTime( 0.05f ),
m_fLastKillWaitTimeAfterRebirth( 1.f ),
m_bEnableCommandKeyProcess( true ),
m_bEnableAllKeyProcess( true ),
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
m_iPacketSendFrameMoveCount( 0 ),
m_iNPCRobustPacketSendIndex( 0 ),
m_dwNPCRobustPacketSendBaseFrameMoveCount( 0 ),
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//m_NPCPacketSendInterval( 0.f ),
//m_NPCPacketSendIntervalNow( 0.f ),
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
m_LoadingPercent( 0 ),
m_pDropItemManager( NULL ),
m_pFontForUnitName( NULL ),
m_bStopUnitChain( false ),
m_fLastkillShotTime( 0.f ),
m_pRenderTargetLastkillShot( NULL ),
m_bOpenChatBox( false ),
m_pTalkBoxMgr( NULL ),
m_fElapsedTimeAfterLastKeyboardInput( 0.f ),
m_RedTeamKill( 0 ),
m_BlueTeamKill( 0 ),
m_bRenderUserName( true ),
m_fSkipTime( 0.f ),
m_bShowF1Help( false ),
m_pTextureF1Help( NULL ),
m_pFontF1Help( NULL ),
m_bEnableNPCTalkBox( true ),
m_fElapsedTimeAfterDisableNPCTalkBox( 0.f ),
m_GameScore( -1 ),
m_GameScoreBack( -1 ),
m_fTechPointViewTime( 0.f ),
m_TechPoint( 0 ),
m_TechPointFull( 0 ),
m_hSeqTech( INVALID_PARTICLE_SEQUENCE_HANDLE ),
m_pTextGuideHelp( NULL ),
m_fTimePauseNPCAI( 0.f ),
m_bFreeCamera( false ),
m_pDLGMyScore( NULL ),
m_pDLGSkillSlot( NULL ),
m_TeamScoreAdvRed( 0 ),
m_TeamScoreAdvBlue( 0 ),
m_TeamVPMaxAdvRed( 0 ),
m_TeamVPMaxAdvBlue( 0 ),
m_AllowFrameCount( 0 ),
m_SpeedHackCheck( 0 ),
m_GetEXPInGame( 0 ),
m_bGameEnd( false ),
m_bShowAllUnitGageBar( true ),
m_StateString( L"" ),
m_bSkipKeyPressed( false ),
m_pNPCFunc( NULL ),
m_pDLGMissionOpening( NULL ),
m_pDLGMissionProgress( NULL ),
m_bFixedCamera( false ),
m_bEnableMonsterFlyTest( false )
,
m_bSkillSlotJustChanged(true)
, m_bShowTeleportEffectOnStageStart( false )

, m_bSendLeaveRoom(false)
, m_bWorldCameraEdit(false)
//{{ kimhc // 2011-07-28 // 옵션수치화
, m_bIsDamageFreeGame( false ) // 1레벨 보정 던전인가?
//}} kimhc // 2011-07-28 // 옵션수치화
, m_optrHostGameUnit()
, m_fResurrectionRemainTime( 0.f )
, m_bIsSendResurrectionReq( false )
, m_bResurrectionCountStart( false )
, m_pDLGResurrect( NULL )
, m_UnitUIDToResurrect( -1 )
, m_pDLGContinue( NULL )
, m_pSoundContinue( NULL )
, m_pDLGCheckResurrectByUseCash( NULL )
, m_fTimeWaitStopContinue( 0.f )
, m_bStopRebirthCount( false )
, m_fTimeEnableKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ( 0.f )
, m_bEnableSendKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ( true )
, m_fAutoResurrectionTime (0.f)	/// 필드에서 DIE상태로 5분이상 경과시 자동으로 마을로 이동하기 위한 카운트
, m_pCinematicUI ( NULL )
, m_bCanNotInputAndPauseNPCAI( false )
, m_uiPositionIndexRed( 0 )
, m_uiPositionIndexBlue( 0 )
, m_iSubStageTechPoint ( 0 )				// SubStage 의 테크닉 포인트를 초기화한다.


, m_bIsAlreadySendingFrame ( false )
{
    CX2GUNPC::ResetReactionSimulationCounter();

// #ifndef _SERVICE_
// 	g_pData->GetUnitManager()->OpenNPCStatScriptFile( L"NPCStat.lua" );
// #endif

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_kFrameUDPPack.ResetFrameUDPPack( false );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	m_kGameStatistics.Init();

	// variable init

	for( int i=0; i< ARRAY_SIZE( m_arraySkillCoolTime ); i++ )
	{
		m_arraySkillCoolTime[i] = 0.f;
	}

	m_CreateNPCDataList.reserve( 32 );
	m_vecF1HelpString.reserve( 16 );


	m_LineSyncPacketSendInterval	= 3.f;
	m_LineSyncPacketSendIntervalNow	= 0.0f;





	// variable assign
	g_pX2Game					= this;
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
    m_iPacketSendFrameMoveCount = 0;
    m_iNPCRobustPacketSendIndex = 0;
    m_dwNPCRobustPacketSendBaseFrameMoveCount = 0;
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//	m_NPCPacketSendInterval		= (1.0f/8.0f);
//	m_NPCPacketSendIntervalNow	= 0.0f;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND

	m_fLastKillWaitTime				= 0.05f;	//FieldFix: 이 코드는 필요 없음
	m_fLastKillWaitTimeAfterRebirth	= 1.f;


	m_UnitList.reserve( 64 );
	m_UserUnitList.resize(MAX_GAME_USER_UNIT_NUM);
	m_NPCUnitList.resize(MAX_GAME_NPC_UNIT_NUM);

	fill( m_UserUnitList.begin(), m_UserUnitList.end(), (CX2GUUser*)NULL );
	fill( m_NPCUnitList.begin(), m_NPCUnitList.end(), (CX2GUNPC*)NULL );


	m_pFontForUnitName			= g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_15_BOLD );
	m_pRenderTargetLastkillShot = g_pKTDXApp->GetDeviceManager()->OpenRenderTargetTexture( L"LastKillShot", 1024, 768 );

	for( int i = 0; i < g_pX2Room->GetSlotNum(); i++ )
	{
		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData (i );
		if( pSlotData != NULL && pSlotData->m_pUnitViewer != NULL )
		{
			pSlotData->m_pUnitViewer->SetShowObject( false );
		}
	}

	m_pTalkBoxMgr = CX2TalkBoxManager::CreateTalkBoxManger();
	m_pTextureF1Help		= g_pKTDXApp->GetDeviceManager()->OpenTexture( L"FadeInOut.dds" );
	m_pFontF1Help			= g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_15_BOLD );


	if ( g_pX2Room != NULL )
	{
		if ( g_pX2Room->GetMySlot() != NULL )
		{
			if ( g_pX2Room->GetMySlot()->m_bObserver == false )
			{
				m_pDLGMyScore			= new CKTDGUIDialog( this, L"DLG_Game_My_Score_NEW.lua" );
				g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMyScore );
				//마을에서도 스킬창을 보여 주기 위해 UI매니져에서 관리하도록 수정
				m_pDLGSkillSlot			= g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot();
			}
		}
	}
	ResetReBirthStoneNumUI();

	m_pNPCFunc = new CX2GUNPCFunc();


	m_AllowFrameCount		= (int)(85 * 5.3);



	m_vDropItemPos = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_optrItemDamageEffectGameUnit.Reset();
	m_fItemDamageEffectTime = 0.f;
	m_iItemDamageEffectIndex = 0;

	InitF1HelpString();

	g_pKTDXApp->GetDIManager()->SetEnable( true );

	if ( g_pData->GetMessenger() != NULL )
	{
		g_pData->GetMessenger()->SetOpen( false );
	}

	if ( g_pMain->GetInformerManager() != NULL )
		g_pMain->GetInformerManager()->ClearAllInfo();

	//g_pMain->SendHackMail();
	//if( g_pData != NULL && g_pData->GetMyUser()->GetUserData().hackingUserType == CX2User::HUT_AGREE_HACK_USER )
	//	g_pMain->SendHackMail_DamageHistory("Game Start");

	// variable init
	ZeroMemory( m_StopContinueAlarmText, sizeof(WCHAR) * 256 );




	if( g_pInstanceData != NULL )
		g_pInstanceData->SetSendHackMail_AutoHack(false);

	if( g_pData != NULL && g_pData->GetPetManager() != NULL && g_pData->GetPetManager()->GetTalkBoxManager() != NULL)
	{
		g_pData->GetPetManager()->GetTalkBoxManager()->Clear();
	}

	m_iNpcDiePacket = 0;

	m_fShowAlarmTime = 0.f;
	m_pShowAlarm = NULL;
	m_hDangerAlarm = INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_vecRemoveDamageEffect.clear();


	if( g_pMain != NULL )
	{
		for(int i=0; i<2; ++i)
			g_pMain->SendHackMailGameStart( i, true );
	}



	m_pCinematicUI = new CinematicUI();

#if X2_DEV_TOOLS
	if( NULL != GetMajorXMeshPlayer() )
		GetMajorXMeshPlayer()->SetShowAttackBox( false );
#endif //SHOW_ATTACK_BOX_DUMMY

	if( NULL != g_pData &&
		NULL != g_pData->GetUIManager() &&
		NULL != g_pData->GetUIManager()->GetUIQuickSlot() )
	{
		g_pData->GetUIManager()->GetUIQuickSlot()->SetWaiting_EGS_USE_QUICK_SLOT_ACK( false );
	}

	/// 해로운 효과라고 정의된 공식적인 디버프 리스트 설정
	SetOfficiallyDebuffList();
}


CX2Game::~CX2Game()
{
	/*
	Release() 에서 수행
	*/
    CX2GUNPC::ResetReactionSimulationCounter();
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
/*virtual*/ void CX2Game::Release()
{

    FlushSendFrameAverage();


	//m_NPCUnitList.clear();


	//m_UserUnitList.clear();

	CX2GageManager* pGageManager = CX2GageManager::GetInstance();
	// 	if( NULL != pGageManager )
	// 	{
	// 		pGageManager->ClearBuffList();
	// 	}

	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];
		if ( NULL != pCX2GameUnit )
			pCX2GameUnit->FinishAndClearAllBuff();
		if ( pCX2GameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			CX2GUUser* pCX2GUUser = static_cast<CX2GUUser*>( pCX2GameUnit );
			if ( pCX2GUUser->GetUnitIndex() >= 0 && pCX2GUUser->GetUnitIndex() < (int) m_UserUnitList.size()
				&& m_UserUnitList[ pCX2GUUser->GetUnitIndex() ] == pCX2GUUser )
			{
				m_UserUnitList[ pCX2GUUser->GetUnitIndex() ] = NULL;
			}
		}
		else
		{
			CX2GUNPC* pCX2GUNPC = static_cast<CX2GUNPC*>( pCX2GameUnit );
			if ( pCX2GUNPC->GetUnitIndex() >= 0 && pCX2GUNPC->GetUnitIndex() < (int) m_NPCUnitList.size()
				&& m_NPCUnitList[ pCX2GUNPC->GetUnitIndex() ] == pCX2GUNPC )
			{
				m_NPCUnitList[ pCX2GUNPC->GetUnitIndex() ] = NULL;
			}
		}
		m_UnitList[i] = NULL;	// note!! 이브는 GUEve객체가 지워질 때 자신이 소환한 몬스터를 참조해서 자폭하도록 하는데, 이때 m_UnitList에서 검색하기 때문에 이미 지워진 unit은 NULL로 설정해준다.
		SAFE_DELETE_KTDGOBJECT( pCX2GameUnit );
	}
	m_UnitList.clear();
	m_UserUnitList.clear();
	m_NPCUnitList.clear();


	for ( UINT i = 0; i < m_CreateNPCDataList.size(); i++ )
	{
		CreateNPCData* pCreateNPCData = m_CreateNPCDataList[i];
		SAFE_DELETE(pCreateNPCData);
	}
	m_CreateNPCDataList.clear();



	//if( NULL != m_pMajorParticle )
	//{
	//	m_pMajorParticle->DestroyInstanceHandle( m_hSeqTech );
	//}

	SAFE_DELETE_DIALOG( m_pDLGResurrect );
	SAFE_DELETE_DIALOG( m_pDLGCheckResurrectByUseCash );
	ZeroMemory( &m_ReservedPacket, sizeof(KEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ) );
	ClearDangerAlarm();
	if( NULL != GetMajorParticle() )
	{
		GetMajorParticle()->DestroyInstanceHandle( m_hSeqTech );
		GetMajorParticle()->DestroyInstanceHandle( m_hDangerAlarm );
	}

	SAFE_DELETE( m_pWorld );
	SAFE_DELETE( m_pCamera );
	SAFE_DELETE( m_pDamageEffect );

	GetEffectSet()->StopEffectSetAll();


	SAFE_DELETE( m_pDropItemManager );

	//SAFE_DELETE( m_pMajorParticle );
	//SAFE_DELETE( m_pMinorParticle );
	//SAFE_DELETE( m_pMajorXMeshPlayer );
	//SAFE_DELETE( m_pMinorXMeshPlayer );


	if( NULL != g_pData->GetPicChar() )
		g_pData->GetPicChar()->Clear();
	if( NULL != g_pData->GetPicCharRed() )
		g_pData->GetPicCharRed()->Clear();
	if( NULL != g_pData->GetPicCharBlue() )
		g_pData->GetPicCharBlue()->Clear();
	if( NULL != g_pData->GetPicCharGreen() )
		g_pData->GetPicCharGreen()->Clear();
	if( NULL != g_pData->GetPicCharYellow() )
		g_pData->GetPicCharYellow()->Clear();
	if( NULL != g_pData->GetPicCharGameScore() )
		g_pData->GetPicCharGameScore()->Clear();
	if( NULL != g_pData->GetPicCharTechPoint() )
		g_pData->GetPicCharTechPoint()->Clear();






	SAFE_CLOSE( m_pRenderTargetLastkillShot );

	SAFE_DELETE_KTDGOBJECT( m_pTalkBoxMgr );

	SAFE_CLOSE( m_pTextureF1Help );

	SAFE_DELETE_DIALOG( m_pDLGMyScore );
	SAFE_DELETE( m_pNPCFunc );

	SAFE_DELETE_DIALOG( m_pDLGMissionOpening );
	SAFE_DELETE_DIALOG( m_pDLGMissionProgress );

	for( int i = 0; i < (int)m_ScoreTimeList.size(); i++ )
	{
		ScoreTime* pScoreTime = m_ScoreTimeList[i];
		SAFE_DELETE( pScoreTime );
	}
	m_ScoreTimeList.clear();

	//	g_pKTDXApp->GetDGManager()->GetFontManager()->DestroyUKFont( m_pFontForUnitName );
	//	g_pKTDXApp->GetDGManager()->GetFontManager()->DestroyUKFont( m_pFontF1Help );

	g_pData->GetSlideShot()->ScenEnd();

	g_pX2Game	= NULL;

	//RemoveAllGuideText();

	g_pKTDXApp->GetDIManager()->SetEnable( true );
	g_pKTDXApp->GetDGManager()->StartFadeIn( 0.1f );

	if( NULL != g_pData->GetMessenger() )
	{
		g_pData->GetMessenger()->SetOpen( false );
	}
	if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree()
		&& NULL != g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot() )
	{
		g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot()->SetEnable(true);
	}
	m_vecRemoveDamageEffect.clear();

	bool bLoop      = true;
	DWORD ret       = NULL;


	//g_pMain->SendHackMail();
	//if( g_pData != NULL && g_pData->GetMyUser()->GetUserData().hackingUserType == CX2User::HUT_AGREE_HACK_USER )
	//	g_pMain->SendHackMail_DamageHistory("Game End");
	if( g_pMain != NULL )
	{
		g_pMain->DllManagerReport();
	}

	if( g_pData->GetPartyManager() != NULL )
	{
		g_pData->GetPartyManager()->SetGameStartFlag(false);
	}

	SAFE_DELETE(m_pCinematicUI);

	CX2GageManager::GetInstance()->ClearPvpMemberUI();

	GetMajorParticle()->DestroyAllInstances();
	GetMinorParticle()->DestroyAllInstances();
	GetMajorXMeshPlayer()->DestroyAllInstances();
	GetMinorXMeshPlayer()->DestroyAllInstances();


	if ( NULL != g_pData->GetRelationshipEffectManager() )
	{
		g_pData->GetRelationshipEffectManager()->SetShowRelationshipAttachEffect(CX2RelationshipEffectManager::REST_HIDE_ALL);
	}

//{{ robobeg : 2013-06-12
	g_pData->GetUnitManager()->UnloadAllNPCInitData();
//}} robobeg : 2013-06-12

    CX2GUNPC::ResetReactionSimulationCounter();
}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

WORD    CX2Game::Handler_P2P_UnitUserSyncPack( const BYTE* pbyData, BYTE byNumPacks, WORD wMaxSize, UidType unitUID, DWORD dwFrameMoveCount )
{
	CX2GUUser* pUserUnit = GetUserUnitByUID( unitUID );

	if( pUserUnit != NULL )
	{
		//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
		// PVP 이고, 보낸이 유닛UID가 0이 아니고, UDP 체크를 시작한 후에
		if( GetGameType() == GT_PVP && m_pRoom != NULL && unitUID > 0 )
		{
			// 시간 갱신부
			std::map< UidType, boost::timer >::iterator mitFirst = m_mapCheckUDPTimer.find( unitUID );
			if( mitFirst == m_mapCheckUDPTimer.end() )
			{
				// 2012.06.17 lygan_조성욱 // 처음 싱크를 주고 받았다 등록하자
				boost::timer timerFirst;
				timerFirst.restart();
				m_mapCheckUDPTimer.insert( std::make_pair( unitUID, timerFirst ) );
			}
			else
			{
				mitFirst->second.restart();
			}

			// 검사부
			std::map< UidType, boost::timer >::iterator mit;
			for ( mit = m_mapCheckUDPTimer.begin(); mit != m_mapCheckUDPTimer.end(); mit++ )
			{
				// 2012.06.17 lygan_조성욱 // 두번째 싱크 지연시간이 5초 이상이다 이상하다
				// 2012.06.17 lygan_조성욱 // 내가 지연 된게 아닌지 체크
				if( mit->second.elapsed() > 1.0f &&
					NULL != GetMyUnit() &&
					mit->first != GetMyUnit()->GetUnitUID() )
				{
					// 해당 유저가 현재 게임내에 존재하고,유저의 상태가 USI_LOSE 상태 값 보다 큰 경우면(p2p 패킷을 정상적으로 보내야 하는 상태들) 랙 유발 의심유저로 등록해 달라고 요청함
					CX2GUUser* pGUUserToBeChecked = GetUserUnitByUID( mit->first );
					if ( NULL != pGUUserToBeChecked && pGUUserToBeChecked->GetNowStateID() > static_cast<int>( CX2GUUser::USI_LOSE ) )
					{
						// 서버에서 검증하므로 바로 찌릅니다. 상대의 통신을 릴레이로 변경해 달라고 요청
						KEGS_FORCE_RELAY_NOT kPacket;
						kPacket.m_iTargetUnitUID		= mit->first;

						if( g_pData != NULL && g_pData->GetServerProtocol() != NULL )
							g_pData->GetServerProtocol()->SendPacket( EGS_FORCE_RELAY_NOT, kPacket );
					}
					// 한번 신고가 들어갔음으로 시간 초기화 해준다.
					mit->second.restart();
				}
			}
		}
		//}}


		IncreaseUserPacketCountToReceive( unitUID );

		// 검사 후 정상 처리

        const BYTE* pbyBegin = pbyData;
        const BYTE* pbyEnd = pbyData + wMaxSize;
        KDYNAMIC_UNIT_USER_SYNC kUserSync;
        kUserSync.Reset();
//#ifdef  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
        kUserSync.m_usDataFieldFlag = KDYNAMIC_UNIT_USER_SYNC::eUnitUserSync_DataField_All;
//#endif  X2OPTIMIZE_USER_USER_PUSH_PASS_SYNC
        for( unsigned u = 0; u != byNumPacks; ++u )
        {
            if ( kUserSync.Deserialize( pbyData, pbyEnd ) == false )
                return 0;
            pUserUnit->ReceiveFrameData( ( dwFrameMoveCount + u + 1 >= byNumPacks ) ? ( dwFrameMoveCount + u + 1 - byNumPacks ) : 0, kUserSync );
        }//for
        return (WORD) ( pbyData - pbyBegin );
	}
    else
    {
        const BYTE* pbyBegin = pbyData;
        const BYTE* pbyEnd = pbyData + wMaxSize;
        KDYNAMIC_UNIT_USER_SYNC kUserSync;
        kUserSync.Reset();
        for( unsigned u = 0; u != byNumPacks; ++u )
        {
            if ( kUserSync.Deserialize( pbyData, pbyEnd ) == false )
                return 0;
        }//for
        return (WORD) ( pbyData - pbyBegin );
    }//if.. else..

    return 0;
}


WORD    CX2Game::Handler_P2P_UnitNpcMiniSyncPack( const BYTE* pbyData, BYTE byNumNPCs, WORD wMaxSize, UidType pidUnitUID, DWORD dwFrameMoveCount )
{
    if ( pbyData == NULL || byNumNPCs == 0 || wMaxSize == 0 )
        return 0;

    const BYTE* pbyBegin = pbyData;
    const BYTE* pbyEnd = pbyData + wMaxSize;
    KDYNAMIC_UNIT_NPC_MINISYNC  kNPCMIniSync;
    for( unsigned u = 0; u != byNumNPCs; u++ )
    {
        if ( kNPCMIniSync.Deserialize( pbyData, pbyEnd ) == false )
            return 0;
        if ( pidUnitUID != 0 )
        {
            CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( kNPCMIniSync.m_iUnitUID );
            if ( pCX2GUNPC != NULL )
            {
                pCX2GUNPC->ReceiveMiniSyncData( pidUnitUID, dwFrameMoveCount, kNPCMIniSync );
            }
        }
    }//for

    return  (WORD) ( pbyData - pbyBegin );
}


WORD    CX2Game::Handler_P2P_UnitNpcSyncPack( const BYTE* pbyData, BYTE byNumNPCs, WORD wMaxSize, UidType pidUnitUID, DWORD dwFrameMoveCount )
{
    if ( pbyData == NULL || byNumNPCs == 0 || wMaxSize == 0 )
        return 0;

    const BYTE* pbyBegin = pbyData;
    const BYTE* pbyEnd = pbyData + wMaxSize;
    KDYNAMIC_UNIT_NPC_SYNC  kNPCSync;
    for( unsigned u = 0; u != byNumNPCs; u++ )
    {
        if ( kNPCSync.Deserialize( pbyData, pbyEnd ) == false )
            return 0;
        if ( pidUnitUID != 0 )
        {
            CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( kNPCSync.m_iUnitUID );
            if ( pCX2GUNPC != NULL )
            {
                pCX2GUNPC->ReceiveFrameData( pidUnitUID, dwFrameMoveCount, kNPCSync );
//			    if ( bFirst_ )
//			    {
//#ifdef FIX_NPC_SHOW_OBJECT
//				    if( true == pCX2GUNPC->GetShowObjectCurrentState() )
//					    pCX2GUNPC->SetShowObject( true );
//#endif //FIX_NPC_SHOW_OBJECT
//			    }
            }
        }
    }//for

    return  (WORD) ( pbyData - pbyBegin );
}

WORD    CX2Game::Handler_P2P_UnitPetSyncPack( const BYTE* pbyData, BYTE byNumNPCs, WORD wMaxSize )
{
    if ( pbyData == NULL || byNumNPCs == 0 || wMaxSize < sizeof(KXPT_UNIT_PET_SYNC) * byNumNPCs )
        return 0;

    KXPT_UNIT_PET_SYNC  kPetSync;
	for( unsigned u = 0; u != byNumNPCs; u++, pbyData += sizeof(KXPT_UNIT_PET_SYNC) )
	{
        const KXPT_UNIT_PET_SYNC* pkSync = (const KXPT_UNIT_PET_SYNC*) pbyData;
		if( g_pData != NULL && g_pData->GetPetManager() != NULL )
		{
			CX2PET *pPet = g_pData->GetPetManager()->GetPet( pkSync->unitUID );
			if( pPet != NULL )
			{
				pPet->ReceiveFrameData( *pkSync );
			}
		}
	}

    return  (WORD) ( sizeof(KXPT_UNIT_PET_SYNC) * byNumNPCs );
}

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
///*virtual*/ void CX2Game::Handler_P2P_UnitNpcSyncPack( CKTDNUDP::RecvData* pRecvData, bool bFirst_ /*= false*/ )
//{
//	KSerBuffer ksBuff;
//	ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//	KXPT_UNIT_NPC_SYNC_PACK kXPT_UNIT_NPC_SYNC_PACK;
//	DeSerialize( &ksBuff, &kXPT_UNIT_NPC_SYNC_PACK );
//
//	for( UINT i = 0; i < kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size(); ++i )
//	{
//		KXPT_UNIT_NPC_SYNC* pXPT_UNIT_NPC_SYNC = &kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList[i];
//		CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( pXPT_UNIT_NPC_SYNC->unitUID );
//		if( pCX2GUNPC != NULL )
//		{
//			pCX2GUNPC->ReceiveFrameData( pXPT_UNIT_NPC_SYNC );
//
//			if ( bFirst_ )
//			{
//#ifdef FIX_NPC_SHOW_OBJECT
//				if( true == pCX2GUNPC->GetShowObjectCurrentState() )
//					pCX2GUNPC->SetShowObject( true );
//#endif //FIX_NPC_SHOW_OBJECT
//			}
//		}
//// 		else
//// 		{
//// 			if ( bFirst_ )
//// 				DISPLAY_ERROR( L"NPC doesn't exist" );
//// 		}
//	}
//}
//
///*virtual*/ void CX2Game::Handler_P2P_UnitPetSyncPack( CKTDNUDP::RecvData* pRecvData )
//{
//	KSerBuffer ksBuff;
//	ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//	KXPT_UNIT_PET_SYNC_PACK kXPT_UNIT_PET_SYNC_PACK;
//	DeSerialize( &ksBuff, &kXPT_UNIT_PET_SYNC_PACK );
//
//	for( UINT i = 0; i < kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList.size(); i++ )
//	{
//		KXPT_UNIT_PET_SYNC* pXPT_UNIT_PET_SYNC = &kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList[i];
//
//		if( g_pData != NULL && g_pData->GetPetManager() != NULL )
//		{
//			CX2PET *pPet = g_pData->GetPetManager()->GetPet( pXPT_UNIT_PET_SYNC->unitUID );
//			if( pPet != NULL )
//			{
//				pPet->ReceiveFrameData( pXPT_UNIT_PET_SYNC );
//			}
//		}
//	}
//}
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK




void CX2Game::P2PPacketHandler()
{
	KTDXPROFILE();

	CKTDNUDP::RecvData* pRecvData = g_pData->GetGameUDP()->PopRecvData();
	while( pRecvData != NULL )
	{
		P2PPacketHandler( pRecvData );
		SAFE_DELETE( pRecvData );
		pRecvData = g_pData->GetGameUDP()->PopRecvData();
	}

    if ( g_pX2Room != NULL )
        g_pX2Room->SetSkipPingSendCheck( false );
}

bool CX2Game::P2PPacketHandler( CKTDNUDP::RecvData* pRecvData )
{
	KTDXPROFILE();

	if( NULL == pRecvData )
		return false;

	if( GS_PLAY != m_GameState )
		return false;


//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	switch( pRecvData->m_ID )
    {
    case XPT_PORT_CHECK_REQ:
        return true;
    case XPT_PORT_CHECK_ACK:
        return true;
	case XPT_PING_TEST_REQ:
		{
            if ( pRecvData->m_Size != sizeof(KXPT_PING_TEST_REQ) )
                return false;

            const KXPT_PING_TEST_REQ* pkReq = (const KXPT_PING_TEST_REQ*) pRecvData->m_pRecvBuffer;
			KXPT_PING_TEST_ACK  kKXPT_PING_TEST_ACK;
			if( GetMyUnit() != NULL )
				kKXPT_PING_TEST_ACK.m_UnitUID	=  GetMyUnit()->GetUnitUID();
			else
				kKXPT_PING_TEST_ACK.m_UnitUID	=  g_pData->GetMyUser()->GetSelectUnit()->GetUID();
			kKXPT_PING_TEST_ACK.m_SendTime	= pkReq->m_SendTime;
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			kKXPT_PING_TEST_ACK.m_bRelay = pkReq->m_bRelay;
			kKXPT_PING_TEST_ACK.m_uiSendCounter	= pkReq->m_uiSendCounter;
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
            kKXPT_PING_TEST_ACK.m_bIgnoreAck = g_pX2Room != NULL && g_pX2Room->GetSkipPingSendCheck();
            g_pData->GetGameUDP()->SendToPeer( pkReq->m_UnitUID, XPT_PING_TEST_ACK, (char*) &kKXPT_PING_TEST_ACK, sizeof(KXPT_PING_TEST_ACK) );
		}
        return true;
    case XPT_PING_TEST_ACK:
        {
            if ( pRecvData->m_Size != sizeof(KXPT_PING_TEST_ACK) )
                return false;
            const KXPT_PING_TEST_ACK* pkAck = (const KXPT_PING_TEST_ACK*) pRecvData->m_pRecvBuffer;
			CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( pkAck->m_UnitUID );
			if( pSlotData != NULL )
			{
                DWORD   dwCurrTime = timeGetTime();
                LONG    lTimeDiff = (LONG) ( dwCurrTime - pkAck->m_SendTime );
                DWORD   dwPingTime = ( lTimeDiff >= 0 ) ? lTimeDiff : 0;
                if ( dwPingTime > 5000 )
                    dwPingTime = 5000;
                bool    bValidPingTime = false;
                switch( pSlotData->GetSlotState() )
                {
                case CX2Room::SS_CLOSE:
                case CX2Room::SS_EMPTY:
                    break;
                default:
                    if ( pSlotData->m_uPingSendCount > 0 )
                    {
                        unsigned u = 0;
                        unsigned uMax = pSlotData->m_uPingSendCount;
                        unsigned uBase = 0;
                        if ( uMax > ARRAY_SIZE( pSlotData->m_adwPingSendTime ) )
                        {
                            uBase = uMax - ARRAY_SIZE( pSlotData->m_adwPingSendTime );
                            uMax = ARRAY_SIZE( pSlotData->m_adwPingSendTime );
                        }
                        for( ; u < uMax; u++ )
                        {
                            if ( pSlotData->m_adwPingSendTime[u] == pkAck->m_SendTime )
                                break;
                        }
                        if ( u < uMax )
                        {
                            for( unsigned v = u + 1; v < uMax; v++ )
                            {
                                pSlotData->m_adwPingSendTime[ v - 1 ] = pSlotData->m_adwPingSendTime[v];
                            }
                            if ( ( g_pX2Room == NULL || g_pX2Room->GetSkipPingSendCheck() == false ) && pkAck->m_bIgnoreAck == false )
                            {
                                lTimeDiff = (LONG) ( dwCurrTime - pkAck->m_SendTime );
                                pSlotData->m_PingTime = (lTimeDiff > 0 ) ? lTimeDiff * ( uBase + 1 ) : 0;
                                if ( pSlotData->m_PingTime > 5000 )
                                    pSlotData->m_PingTime = 5000;
                                bValidPingTime = true;
                            }
                            pSlotData->m_uPingSendCount -= ( uBase + 1 );
                        }
                    }
                    break;
                }//switch
                if ( bValidPingTime == true )
                {
                    float   fInvWeight = 1.f/(float) ( pSlotData->m_dwPingCount + 1 );
                    float   fCutPingTime = __min(pSlotData->m_PingTime,2000) * 0.001f;
                    pSlotData->m_fAvgPingTime = ( pSlotData->m_dwPingCount * pSlotData->m_fAvgPingTime + fCutPingTime ) * fInvWeight;
                    pSlotData->m_fAvgPingTimeSqr = ( pSlotData->m_dwPingCount * pSlotData->m_fAvgPingTimeSqr + fCutPingTime * fCutPingTime ) * fInvWeight;
                    float fPingStd = pSlotData->m_fAvgPingTimeSqr - pSlotData->m_fAvgPingTime * pSlotData->m_fAvgPingTime;
                    pSlotData->m_fAvgPingStd = ( fPingStd <= 0.f ) ? 0.f : sqrtf( fPingStd );
                    if ( pSlotData->m_dwPingCount < 12 )
                        pSlotData->m_dwPingCount++;
                }
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
				if( !pkAck->m_bRelay )
					KOGGamePerformanceCheck::GetInstance()->GetP2PPingCheckMgr()->ReceivePingCheckPacket( dwPingTime, pkAck->m_uiSendCounter );
				else
					KOGGamePerformanceCheck::GetInstance()->GetRelayPingCheckMgr()->ReceivePingCheckPacket( dwPingTime, pkAck->m_uiSendCounter );

				KOGGamePerformanceCheck::GetInstance()->GetUdpPingCheckMgr()->ReceivePingCheckPacket( dwPingTime, pkAck->m_uiSendCounter );

				if( pkAck->m_UnitUID == m_uidHostSlotUID )
				{
					//비호스트  NPC리액션 측정시 PingTime은 왕복 기준이다.
					KOGGamePerformanceCheck::GetInstance()->GetNonHostNpcReactionCheckMgr()->UpdateHostUdpPingTime( dwPingTime );
				}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			}
        }
        return true;
    case XPT_UDP_PACKET_PACK:
        {
            if ( pRecvData->m_Size < sizeof(XPT_UDP_PACKET_PACK_PREFIX) + sizeof(WORD) )
                return false;
        }
        break;

    default:
        return false;
    }//switch

    WORD    wPackSize = pRecvData->m_Size;
    const XPT_UDP_PACKET_PACK_PREFIX* pkPrefix = ((const XPT_UDP_PACKET_PACK_PREFIX*) &pRecvData->m_pRecvBuffer[0] );
    UidType pidUnitUID = pkPrefix->m_iUnitUID;
    DWORD   dwFrameMoveCount = pkPrefix->m_dwFrameMoveCount;
    BYTE    pbyUncompressBuffer[CKTDNUDP_MTU_MAX];
    WORD    wSize = 0;
    const BYTE* pbyData = NULL;
    WORD    wDataSize = 0;
    for( WORD wOffset = sizeof(XPT_UDP_PACKET_PACK_PREFIX); wOffset + sizeof(WORD) < wPackSize; wOffset += sizeof(WORD) + wSize)
    {
        wSize = *((const WORD*) &pRecvData->m_pRecvBuffer[wOffset] );
        bool    bCompressed = ( wSize & 0x8000 ) != 0;
        wSize &= 0x7fff;
        if ( wSize == 0 )
        {
            continue;
        }//if
        if ( bCompressed == true )
        {
            uLongf  uUncompressedSize = CKTDNUDP_MTU_MAX;
            if ( uncompress( &pbyUncompressBuffer[0], &uUncompressedSize, (const BYTE*) &pRecvData->m_pRecvBuffer[wOffset+sizeof(WORD)], wSize ) != Z_OK )
            {
                continue;
            }
            pbyData = pbyUncompressBuffer;
            wDataSize = (WORD) uUncompressedSize;
        }
        else
        {
            pbyData = (const BYTE*) &pRecvData->m_pRecvBuffer[wOffset + sizeof(WORD)];
            wDataSize = wSize;
        }
        for( WORD wDataOffset = 0; wDataOffset < wDataSize; )
        {
            BYTE byID = pbyData[ wDataOffset ];
            if ( byID <= XPT_UDP_PACKET_PACK )
                break;
            wDataOffset++;
            switch( byID )
            {
            case XPT_UNIT_NPC_MINISYNC_PACK:
                {
                    ASSERT( wDataOffset + 1 <= wDataSize );
                    if ( wDataOffset + 1 > wDataSize )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    BYTE byNumPack = pbyData[ wDataOffset ];
                    wDataOffset++;
        			WORD wProcessed = Handler_P2P_UnitNpcMiniSyncPack( &pbyData[ wDataOffset ], byNumPack
                        , wDataSize - wDataOffset, ( GetHostSlotUID() == pidUnitUID ) ? pidUnitUID : 0, dwFrameMoveCount );
                    if ( wProcessed == 0 )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    wDataOffset += wProcessed;
					break;
                }
                break;
            case XPT_UNIT_NPC_SYNC_PACK:
                {
                    ASSERT( wDataOffset + 1 <= wDataSize );
                    if ( wDataOffset + 1 > wDataSize )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    BYTE byNumPack = pbyData[ wDataOffset ];
                    wDataOffset++;
        			WORD wProcessed = Handler_P2P_UnitNpcSyncPack( &pbyData[ wDataOffset ], byNumPack
                        , wDataSize - wDataOffset,  ( GetHostSlotUID() == pidUnitUID ) ? pidUnitUID : 0, dwFrameMoveCount );
                    if ( wProcessed == 0 )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    wDataOffset += wProcessed;
		        }
                break;
            case XPT_UNIT_PET_SYNC_PACK:
                {
                    ASSERT( wDataOffset + 1 <= wDataSize );
                    if ( wDataOffset + 1 > wDataSize )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    BYTE byNumPack = pbyData[ wDataOffset ];
                    wDataOffset++;
                    WORD wProcessed = Handler_P2P_UnitPetSyncPack( &pbyData[ wDataOffset ], byNumPack
                        , wDataSize - wDataOffset );
                    if ( wProcessed == 0 )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    wDataOffset += wProcessed;
                }
                break;
            case XPT_UNIT_USER_SYNC_PACK:
                {
                    ASSERT( wDataOffset + 1 <= wDataSize );
                    if ( wDataOffset + 1 > wDataSize )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    BYTE byNumPack = pbyData[ wDataOffset ];
                    wDataOffset++;
        			WORD wProcessed = Handler_P2P_UnitUserSyncPack( &pbyData[ wDataOffset ], byNumPack
                        , wDataSize - wDataOffset, pidUnitUID, dwFrameMoveCount );
                    if ( wProcessed == 0 )
                    {
                        wDataOffset = wDataSize;
                        break;
                    }
                    wDataOffset += wProcessed;
                }
                break;
            default:
                wDataOffset = wDataSize;

                break;
            }
        }

    }//for
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//	switch( pRecvData->m_ID )
//	{
//	case XPT_UNIT_USER_SYNC_PACK:
//		{
//			KSerBuffer ksBuff;
//			ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//			KXPT_UNIT_USER_SYNC_PACK kXPT_UNIT_USER_SYNC_PACK;
//			DeSerialize( &ksBuff, &kXPT_UNIT_USER_SYNC_PACK );
//			CX2GUUser* pUserUnit = GetUserUnitByUID( kXPT_UNIT_USER_SYNC_PACK.m_iUnitUID );
//
//			if( pUserUnit != NULL )
//			{
//				//{{ 2013. 2. 1	박세훈	랜선렉 방지 코드2
//#ifdef SERV_FIX_SYNC_PACKET_USING_RELAY
//				if ( GetGameType() == GT_PVP )
//					AbuserUserCheck( pUserUnit );
//#endif SERV_FIX_SYNC_PACKET_USING_RELAY
//				//}}
//
//#ifdef UDP_CAN_NOT_SEND_USER_KICK
//				AbuserUserCheck( kXPT_UNIT_USER_SYNC_PACK.m_iUnitUID ); //랜선렉 악용 유저 체크
//#endif //UDP_CAN_NOT_SEND_USER_KICK
//
//#ifdef SHOW_UDP_NETWORK_INFO_IN_ROOM
//				IncreaseUserPacketCountToReceive( kXPT_UNIT_USER_SYNC_PACK.m_iUnitUID );
//#endif // SHOW_UDP_NETWORK_INFO_IN_ROOM
//
//				// 검사 후 정상 처리
//				pUserUnit->ReceiveFrameData( &kXPT_UNIT_USER_SYNC_PACK );
//			}
//		} break;
//
//#ifdef UNIT_SYNC_PACKET_DUNGEON_FOR_TEST
//		//case XPT_UNIT_USER_SYNC_PACK_FOR_DUNGEON:
//		//	{
//		//		KSerBuffer ksBuff;
//		//		ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//		//		KXPT_UNIT_USER_SYNC_PACK_FOR_DUNGEON kXPT_UNIT_USER_SYNC_PACK;
//		//		DeSerialize( &ksBuff, &kXPT_UNIT_USER_SYNC_PACK );
//		//		CX2GUUser* pUserUnit = GetUserUnitByUID( kXPT_UNIT_USER_SYNC_PACK.unitUID );
//		//		if( pUserUnit != NULL )
//		//			pUserUnit->ReceiveFrameData( &kXPT_UNIT_USER_SYNC_PACK );
//		//	} break;
//#endif UNIT_SYNC_PACKET_DUNGEON_FOR_TEST
//
//	case XPT_UNIT_NPC_FIRST_SYNC_PACK_BY_BATTLE_FIELD:
//		{
//			if ( false == IsHost() )
//			{
//				Handler_P2P_UnitNpcSyncPack( pRecvData, true );
//
//				g_pData->GetServerProtocol()->SendID( EGS_BATTLE_FIELD_NPC_P2P_SYNC_COMPLETE_NOT );
//			}
//		} break;
//
//	case XPT_UNIT_NPC_SYNC_PACK:
//		{
//			Handler_P2P_UnitNpcSyncPack( pRecvData );
//		} break;
//
//
//	case XPT_UNIT_NPC_SYNC:
//		{
//			KSerBuffer ksBuff;
//			ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//			KXPT_UNIT_NPC_SYNC kXPT_UNIT_NPC_SYNC;
//			DeSerialize( &ksBuff, &kXPT_UNIT_NPC_SYNC );
//
//			CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( kXPT_UNIT_NPC_SYNC.unitUID );
//			if( pCX2GUNPC != NULL )
//				pCX2GUNPC->ReceiveFrameData( &kXPT_UNIT_NPC_SYNC );
//		} break;
//
//	case XPT_PING_TEST_REQ:
//		{
//			KSerBuffer ksBuff;
//			ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//			KXPT_PING_TEST_REQ kKXPT_PING_TEST_REQ;
//			DeSerialize( &ksBuff, &kKXPT_PING_TEST_REQ );
//
//			KXPT_PING_TEST_ACK kKXPT_PING_TEST_ACK;
//			if( GetMyUnit() != NULL )
//				kKXPT_PING_TEST_ACK.m_UnitUID	=  GetMyUnit()->GetUnitUID();
//			else
//			{
//				kKXPT_PING_TEST_ACK.m_UnitUID	=  g_pData->GetMyUser()->GetSelectUnit()->GetUID();
//			}
//			kKXPT_PING_TEST_ACK.m_SendTime	=  kKXPT_PING_TEST_REQ.m_SendTime;
//#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
//			kKXPT_PING_TEST_ACK.m_bRelay = kKXPT_PING_TEST_REQ.m_bRelay;
//			kKXPT_PING_TEST_ACK.m_uiSendCounter	=  kKXPT_PING_TEST_REQ.m_uiSendCounter;
//#ifdef  SERV_OPTIMIZE_CHOOSE_FASTEST_HOST_ENHANCE
//           kKXPT_PING_TEST_ACK.m_bIgnoreAck = g_pX2Room != NULL && g_pX2Room->GetSkipPingSendCheck();
//#endif  SERV_OPTIMIZE_CHOOSE_FASTEST_HOST_ENHANCE
//#endif//ACTIVE_KOG_GAME_PERFORMANCE_CHECK
//
//			KSerBuffer buff;
//			Serialize( &buff, &kKXPT_PING_TEST_ACK );
//			g_pData->GetGameUDP()->Send( kKXPT_PING_TEST_REQ.m_UnitUID, XPT_PING_TEST_ACK, (char*)buff.GetData(), buff.GetLength() );
//		} break;
//
//	case XPT_PING_TEST_ACK:
//		{
//			KSerBuffer ksBuff;
//			ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//			KXPT_PING_TEST_ACK kKXPT_PING_TEST_ACK;
//			DeSerialize( &ksBuff, &kKXPT_PING_TEST_ACK );
//
//			CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( kKXPT_PING_TEST_ACK.m_UnitUID );
//			if( pSlotData != NULL )
//			{
//              LONG    lTimeDiff = (LONG) ( timeGetTime() - kKXPT_PING_TEST_ACK.m_SendTime );
//               DWORD   dwPingTime = ( lTimeDiff >= 0 ) ? lTimeDiff : 0;
//                if ( dwPingTime > 5000 )
//                    dwPingTime = 5000;
//				pSlotData->m_PingTime = dwPingTime;
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//                float   fInvWeight = 1.f/(float) ( pSlotData->m_dwPingCount + 1 );
//               float   fCutPingTime = __min(pSlotData->m_PingTime,2000) * 0.001f;
//                pSlotData->m_fAvgPingTime = ( pSlotData->m_dwPingCount * pSlotData->m_fAvgPingTime + fCutPingTime ) * fInvWeight;
//#ifdef  SERV_OPTIMIZE_CHOOSE_FASTEST_HOST_ENHANCE
//                pSlotData->m_fAvgPingTimeSqr = ( pSlotData->m_dwPingCount * pSlotData->m_fAvgPingTimeSqr + fCutPingTime * fCutPingTime ) * fInvWeight;
//                float fPingStd = pSlotData->m_fAvgPingTimeSqr - pSlotData->m_fAvgPingTime * pSlotData->m_fAvgPingTime;
//                pSlotData->m_fAvgPingStd = ( fPingStd <= 0.f ) ? 0.f : sqrtf( fPingStd );
//#endif  SERV_OPTIMIZE_CHOOSE_FASTEST_HOST_ENHANCE
//                if ( pSlotData->m_dwPingCount < 12 )
//                    pSlotData->m_dwPingCount++;
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//
//#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
//				if( !kKXPT_PING_TEST_ACK.m_bRelay )
//					KOGGamePerformanceCheck::GetInstance()->GetP2PPingCheckMgr()->ReceivePingCheckPacket( dwPingTime, kKXPT_PING_TEST_ACK.m_uiSendCounter );
//				else
//					KOGGamePerformanceCheck::GetInstance()->GetRelayPingCheckMgr()->ReceivePingCheckPacket( dwPingTime, kKXPT_PING_TEST_ACK.m_uiSendCounter );
//
//				KOGGamePerformanceCheck::GetInstance()->GetUdpPingCheckMgr()->ReceivePingCheckPacket( dwPingTime, kKXPT_PING_TEST_ACK->m_uiSendCounter );
//#endif//ACTIVE_KOG_GAME_PERFORMANCE_CHECK
//			}
//		} break;
//
//
//#ifdef SERV_PET_SYSTEM
//	case XPT_UNIT_PET_SYNC:
//		{
//			KSerBuffer ksBuff;
//			ksBuff.Write( pRecvData->m_pRecvBuffer, pRecvData->m_Size );
//			KXPT_UNIT_PET_SYNC kXPT_UNIT_PET_SYNC;
//			DeSerialize( &ksBuff, &kXPT_UNIT_PET_SYNC );
//
//			if( g_pData != NULL && g_pData->GetPetManager() != NULL )
//			{
//				CX2PET *pPet = g_pData->GetPetManager()->GetPet( kXPT_UNIT_PET_SYNC.unitUID );
//				if( pPet != NULL )
//				{
//					pPet->ReceiveFrameData( &kXPT_UNIT_PET_SYNC );
//				}
//			}
//		} break;
//
//	case XPT_UNIT_PET_SYNC_PACK:
//		{
//			if ( false == IsHost() )
//				Handler_P2P_UnitPetSyncPack( pRecvData );
//		} break;
//
//	case XPT_UNIT_PET_FIRST_SYNC_PACK_BY_BATTLE_FIELD:
//		{
//			Handler_P2P_UnitPetSyncPack( pRecvData );
//		} break;
//#endif
//	default:
//		break;
//
//	}
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	return true;
}



void CX2Game::GameLoading( CX2Room* pRoom )
{
	m_pRoom = pRoom;


	HRESULT hr;
	if ( FAILED ( hr = g_pKTDXApp->GetDevice()->EvictManagedResources() ) )
	{
		switch( hr )
		{
		case D3DERR_OUTOFVIDEOMEMORY:
			ErrorLogMsg( XEM_ERROR3, "D3DERR_OUTOFVIDEOMEMORY" );
			break;

		default:
			ErrorLogMsg( XEM_ERROR3, "D3DERR_UNKNOWN" );
			break;
		}
	}

	SystemLoading();
	WorldLoading();
	UnitLoading();
	EtcLoading();



	if( true == IsHost() )
	{
		StateLog( L"i'm game HOST" );
	}

	wstringstream wstrmText;
	if( NULL != m_pRoom )
		wstrmText << L"Room User: " << m_pRoom->GetUserNum() << std::endl;
	wstrmText << L"Game User: " << GetUserUnitNum() << std::endl;
	StateLog( wstrmText.str().c_str() );
}

void CX2Game::SystemLoading()
{
	m_GameState = GS_LOADING;

	srand((unsigned int)time(NULL));

 	/*m_pMajorParticle	= new CKTDGParticleSystem( g_pKTDXApp->GetDevice() );
 	m_pMinorParticle	= new CKTDGParticleSystem( g_pKTDXApp->GetDevice() );
 	m_pMajorXMeshPlayer	= new CKTDGXMeshPlayer();
 	m_pMinorXMeshPlayer	= new CKTDGXMeshPlayer();

 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle2.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle3.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle4.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle5.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle6.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle7.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle8.txt" );
 	m_pMajorParticle->OpenScriptFile( L"GameMajorParticle9.txt" );

 	m_pMinorParticle->OpenScriptFile( L"GameMinorParticle.txt" );

	m_pMajorXMeshPlayer->OpenScriptFile( L"GameMajorXMeshPlayer.txt" );
	m_pMinorXMeshPlayer->OpenScriptFile( L"GameMinorXMeshPlayer.txt" );*/

#if X2_DEV_TOOLS
	switch( g_pMain->GetNowStateID() )
	{
	case CX2Main::XS_TRAINING_GAME:
	case CX2Main::XS_DUNGEON_GAME:
		{
			//게임 파티클 시스템 생성
			g_pData->ResetGameMajorParticle( L"GameMajorParticle.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle2.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle3.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle4.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle5.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle6.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle7.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle8.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle9.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle10.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle11.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle12.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle13.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle14.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle15.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle16.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle17.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle18.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle19.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle20.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle21.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle22.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle23.txt" );

			g_pData->ResetGameMinorParticle( L"GameMinorParticle.txt" );
			g_pData->GetGameMajorParticle()->OpenScriptFile( L"GameMajorParticle_INT.txt" );
			g_pData->GetGameMinorParticle()->OpenScriptFile( L"GameMinorParticle_INT.txt" );
		} break;
	default:
		break;
	}
#endif //REALTIME_SCRIPT_PATCH

	m_pDamageEffect		= new CX2DamageEffect();
	m_pDamageEffect->OpenScriptFile( L"DamageEffect.lua" );
	m_pDamageEffect->OpenScriptFile( L"DamageEffect_INT.lua" );
//

//
// 	m_EffectSet.OpenScriptFile( L"EffectSet.lua" );


	if( NULL != g_pData->GetPicChar() )
		g_pData->GetPicChar()->Clear();
	if( NULL != g_pData->GetPicCharRed() )
		g_pData->GetPicCharRed()->Clear();
	if( NULL != g_pData->GetPicCharBlue() )
		g_pData->GetPicCharBlue()->Clear();
	if( NULL != g_pData->GetPicCharGreen() )
		g_pData->GetPicCharGreen()->Clear();
	if( NULL != g_pData->GetPicCharYellow() )
		g_pData->GetPicCharYellow()->Clear();
	if( NULL != g_pData->GetPicCharGameScore() )
		g_pData->GetPicCharGameScore()->Clear();
	if( NULL != g_pData->GetPicCharTechPoint() )
		g_pData->GetPicCharTechPoint()->Clear();


	m_hDangerAlarm = GetMajorParticle()->CreateSequenceHandle( NULL, L"danger_alarm_01", 0.0f, 0.0f, 0.0f );
	CKTDGParticleSystem::CParticleEventSequence* pSeq = GetMajorParticle()->GetInstanceSequence( m_hDangerAlarm );
	if( pSeq != NULL && pSeq->GetShowObject() == true )
	{
		pSeq->SetShowObject(false);
	}

	//P2PSetPeer();
	//P2PConnectTest();
}

void CX2Game::UnitLoading()
{
	//{{ kimhc // 2011-07-28 // 옵션수치화
	// 여기에는 필요 없는듯...
	if ( true == CX2Dungeon::IsDamageFreeGame() )
		SetIsDamageFreeGame( true ); // 1레벨 보정 던전인가?
	//}} kimhc // 2011-07-28 // 옵션수치화

	AddUserUnit();
	UpdateUnitPointer();

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2UserUnit = m_UserUnitList[i];

		if( pCX2UserUnit != NULL )
		{
			// 입장 시
			if ( NULL != g_pData->GetRelationshipManager() &&
				 NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() &&
				 NULL != g_pData->GetRelationshipEffectManager() )
			{
				CX2RelationshipManager * pRelationshipManager = g_pData->GetRelationshipManager();
				CX2RelationshipManager::RelationshipInfo * pRelationshipInfo = pRelationshipManager->GetMyRelationshipInfo();

				if ( pCX2UserUnit->GetRelationshipUnitUID() > 0 )
				{
					CX2RelationshipEffectManager::RelationEffectInfo relationEffectInfo;
					relationEffectInfo = ( *(g_pData->GetRelationshipEffectManager()->GetRelationEffectInfoIndex ( 1 ) ) );
					relationEffectInfo.PrepareDrawRelationshipEffect( CX2RelationshipEffectManager::RelationEffectInfo::RET_ATTACH_EFFECT, pCX2UserUnit->GetUnit()->GetUID() );
				}

				if ( pRelationshipInfo->m_iRelationTargetUserUid == pCX2UserUnit->GetUnit()->GetUID() )
				{
					pRelationshipManager->SetRelationPartnerInGame ( CX2GameUnitoPtr( pCX2UserUnit ) );
				}
			}
			pCX2UserUnit->PreRenderForce();
		}
	}

	if ( NULL != GetMyUnit() )
		m_optrFocusUnit	= GetMyUnit();
}

void CX2Game::EtcLoading()
{
	SAFE_DELETE( m_pCamera );
	m_pCamera		= new CX2Camera();
	if( m_pWorld != NULL )
		m_pCamera->SetLandHeight( m_pWorld->GetLineMap()->GetLandHeight() );

	// FieldFix: 이부분 오버라이딩 등으로 수정 하자
	switch ( GetGameType() )
	{
	case CX2Game::GT_BATTLE_FIELD:
		m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), 1800.0f );
		break;

	default:
		m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), g_pMain->GetGameOption().GetCameraDistance() );
		break;
	}

	SAFE_DELETE(m_pDropItemManager);
	m_pDropItemManager	= new CX2DropItemManager();
	//m_pDropItemManager->OpenScriptFile( L"DropItemManager.lua" );

	TextureReady( L"HQ_clear_c.dds" );
	TextureReady( L"HQ_clear_l.dds" );
	TextureReady( L"HQ_clear_e.dds" );
	TextureReady( L"HQ_clear_a.dds" );
	TextureReady( L"HQ_clear_r.dds" );

	TextureReady( L"MagicSquare_2.dds" );
	TextureReady( L"Blocking_Net2.dds" );


	if( NULL == GetMyUnit() )
		return;

	CX2GUUser* pMyGUUser = GetMyUnit();
	if( NULL != pMyGUUser )
	{
		pMyGUUser->UpdateSocketDataAndEnchantData();
		pMyGUUser->UpdatePassiveAndActiveSkillState();

		UpdateSkillSlotUI();
	}

// #ifdef NEW_SKILL_TREE
// 	if( NULL != GetMyUnit() )
// 	{
// 		GetMyUnit()->GetGageManager()->UpdateMyManaGuageUI();
// 	}
// #endif

	D3DXVECTOR3 vEye = m_pCamera->GetCamera().GetEye();
	D3DXVECTOR3 vLookAt = m_pCamera->GetCamera().GetLookAt();
	m_FPSCamera.SetViewParams( &vEye, &vLookAt );
	m_FPSCamera.SetProjParams( D3DX_PI/4, 1.f, g_pKTDXApp->GetDGManager()->GetNear(), g_pKTDXApp->GetDGManager()->GetFar() );
	m_FPSCamera.SetRotateButtons( false, false, true, false );

	SoundReady( L"Nasod_Healer_Heal.ogg" );
	SoundReady( L"Item_Take.ogg" );
	SoundReady( L"Mark_Fail.ogg" );
	SoundReady( L"Mark_Success.ogg" );
	SoundReady( L"Medicine_Fail.ogg" );
	SoundReady( L"Medicine_Sprinter.ogg" );
	SoundReady( L"Summon_Dryad_Hit.wav" );
	SoundReady( L"Summon_Fail.ogg" );
	SoundReady( L"Summon_Oread_Hit.wav" );

	TextureReady( L"Summon_Dryad01.tga" );
	TextureReady( L"Summon_Elifea01.tga" );
	TextureReady( L"Summon_Oread01.tga" );
	TextureReady( L"Summon_Raul01.tga" );
	TextureReady( L"Moon_MagicSquare01.dds" );
	TextureReady( L"Marker_Sprinter_Posion01.dds" );
	TextureReady( L"Marker_Antimagic_Symbol01.dds" );
	TextureReady( L"Marker_Brave_Symbol01.dds" );
	TextureReady( L"Marker_Giant_Potion01.dds" );
	TextureReady( L"Marker_Hyper_Potion01.dds" );
	TextureReady( L"Marker_Inviolable_Symbol01.dds" );
	TextureReady( L"Marker_Magic_Symbol01.dds" );
	TextureReady( L"Marker_Pigmy_Potion01.dds" );
	TextureReady( L"Marker_Protect_Symbol01.dds" );
	TextureReady( L"Marker_Side_Effect_Potion01.dds" );

	XMeshReady( L"Blur_Light_Mesh01.Y" );
	XMeshReady( L"Explosion_Light_Mesh01.Y" );

	XSkinMeshReady( L"Motion_WilliamPPoru.X" );
	XSkinMeshReady( L"Summon_Dryad_Effect_Vine.X" );
	XSkinMeshReady( L"Summon_Dryad_Effect_Vine_Bottom.X" );
	//XSkinMeshReady( L"Summon_Oread_Effect_Bottom.X" );
	XSkinMeshReady( L"Summon_Oread_Effect_Rock.X" );
	XSkinMeshReady( L"Antimagic_Symbol_Motion01.X" );
	XSkinMeshReady( L"Brave_Symbol_Motion01.X" );
	XSkinMeshReady( L"Magic_Symbol_Motion01.X" );
	XSkinMeshReady( L"Protect_Symbol_Motion01.X" );
	XSkinMeshReady( L"Wrong_Symbol_Motion01.X" );

}

void CX2Game::AddUserUnit()
{
	for( int i = 0; i <  m_pRoom->GetSlotNum(); i++ )
	{
		CX2Room::SlotData* pSlotData = m_pRoom->GetSlotData( i );

		if( pSlotData == NULL
			|| pSlotData->m_SlotState == CX2Room::SS_CLOSE
			|| pSlotData->m_SlotState == CX2Room::SS_EMPTY
			|| pSlotData->m_SlotState == CX2Room::SS_WAIT )
			continue;

		if( pSlotData->m_bNpc == false )
		{
			AddUserUnit( pSlotData, i );
		}
	}

	if ( GT_PVP == GetGameType() )
	{
		for( int i = 0; i <  m_pRoom->GetSlotNum(); i++ )
		{
			CX2Room::SlotData* pSlotData = m_pRoom->GetSlotData( i );

			if( pSlotData == NULL
				|| pSlotData->m_SlotState == CX2Room::SS_CLOSE
				|| pSlotData->m_SlotState == CX2Room::SS_EMPTY
				|| pSlotData->m_SlotState == CX2Room::SS_WAIT )
				continue;

			if ( false == pSlotData->IsMyUnit() )
			{
				CX2GageManager* pGageManager = CX2GageManager::GetInstance();

				if ( NULL != pGageManager )
				{
					CX2GUUser* pGameUnitMember = GetUserUnitByUID( pSlotData->m_UnitUID );

					pGageManager->InsertPvpMemberUI( *pSlotData, pGameUnitMember );
					pGageManager->UpdatePvpMemberGageData( pSlotData->m_UnitUID, pGameUnitMember );
				}
			}
		}
	}
}

/*virtual*/ void CX2Game::AddUserUnit( CX2Room::SlotData* pSlotData, int slotIndex )
{
	// note!! 캐릭터 추가되면 수정
	CX2GUUser* pCX2UserUnit = CreateGUUser( pSlotData, slotIndex );

	DeleteUserUnit( slotIndex );
	m_UnitList.push_back( (CX2GameUnit*)pCX2UserUnit );
	m_UserUnitList[slotIndex] = pCX2UserUnit;


	if ( pSlotData->m_RelationshipTargetUID > 0 )
	{
		pCX2UserUnit->SetRelationshipUnitUID( pSlotData->m_RelationshipTargetUID );
	}

	//UpdateUnitPointer();
	if ( pSlotData->IsMyUnit() )
	{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        UpdateMyGameUnit( pSlotData, pCX2UserUnit );
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		m_optrMyUnit = pCX2UserUnit;
//		CheckAmIHost();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	}

	CX2GageManager::GetInstance()->SetPartyMemberGameUnit( pCX2UserUnit );

	pCX2UserUnit->Init();
	if ( pCX2UserUnit->IsMyUnit() )
		pCX2UserUnit->SetKBuffFactorFromGageManagerToGameUnit();

	g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2UserUnit );


	if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME &&
		true == pCX2UserUnit->IsMyUnit() ) // 오현빈 // 2013-08-22 // 쿨타임은 자기 캐릭터일 때만 갱신하도록 적용
	{
		pCX2UserUnit->ResetLeftSkillCoolTimeBySkillType( CX2SkillTree::ST_HYPER_ACTIVE_SKILL, 180.f, true, BCT_FIX_VALUE );
	}

}

CX2GUUser* CX2Game::AddUserUnitIntruder( CX2Room::SlotData* pSlotData, int slotIndex )
{
	// note!! 전직 추가되면 수정, 혹은 unit_type으로
	CX2GUUser* pCX2UserUnit = NULL;

	const CX2UnitManager::UnitTypeLuaTemplet& kUnitTypeLuaTemplet =
		g_pData->GetUnitManager()->GetUnitTypeLuaTemplet( pSlotData->m_pUnit->GetType() );
	lua_State*	script_info = ( kUnitTypeLuaTemplet.m_pLuaManager != NULL ) ? kUnitTypeLuaTemplet.m_pLuaManager->GetLuaState() : NULL;

	switch( pSlotData->m_pUnit->GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			pCX2UserUnit = CX2GUElsword_SwordMan::CreateGUElsword( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData->m_pUnit );
		}
		break;

	case CX2Unit::UT_ARME:
		{
			pCX2UserUnit = CX2GUArme_VioletMage::CreateGUArme( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData->m_pUnit );
		}
		break;

	case CX2Unit::UT_LIRE:
		{
			pCX2UserUnit = CX2GULire_ElvenRanger::CreateGULire( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData->m_pUnit );
		}
		break;

	case CX2Unit::UT_RAVEN:
		{
			pCX2UserUnit = CX2GURaven::CreateGURaven( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData->m_pUnit );
		}
		break;


	case CX2Unit::UT_EVE:
		{
			pCX2UserUnit = CX2GUEve::CreateGUEve( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData->m_pUnit );
		}
		break;

		//{{ kimhc // 2010.11.18 // 2010-12-23 New Character CHUNG
	case CX2Unit::UT_CHUNG:
		{
			pCX2UserUnit = CX2GUChung::CreateGUChung( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData->m_pUnit );
		}
		break;
		//}} kimhc // 2010.11.18 //  2010-12-23 New Character CHUNG

	case CX2Unit::UT_ARA:
		{
			pCX2UserUnit = CX2GUAra::CreateGUAra( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(),
												  pSlotData->m_pUnit );
		}
		break;
	case CX2Unit::UT_ELESIS:
		{
			pCX2UserUnit = CX2GUEL::CreateGUEL( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(),
				pSlotData->m_pUnit );
		}
		break;
	case CX2Unit::UT_ADD:
		{
			pCX2UserUnit = CX2GUAdd::CreateGUAdd( slotIndex, pSlotData->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(),
				pSlotData->m_pUnit );
		}
		break;
	}


	pCX2UserUnit->Init();
	if ( pCX2UserUnit->IsMyUnit() )
		pCX2UserUnit->SetKBuffFactorFromGageManagerToGameUnit();
	//{{ robobeg : 2008-10-21
	//pCX2UserUnit->SetShow( false );
	pCX2UserUnit->SetShowObject( false );
	//}} robobeg : 2008-10-21


	if ( pSlotData->m_RelationshipTargetUID > 0 )
	{
		pCX2UserUnit->SetRelationshipUnitUID ( pSlotData->m_RelationshipTargetUID );
	}

	if ( NULL != g_pData->GetRelationshipManager() &&
		NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() &&
		NULL != g_pData->GetRelationshipEffectManager() )
	{
		CX2RelationshipManager * pRelationshipManager = g_pData->GetRelationshipManager();
		CX2RelationshipManager::RelationshipInfo * pRelationshipInfo = pRelationshipManager->GetMyRelationshipInfo();

		if ( pRelationshipInfo->m_eRelationStateType >	SEnum::RT_SOLO )
		{
			if ( pRelationshipInfo->m_iRelationTargetUserUid == pCX2UserUnit->GetUnit()->GetUID() )
			{
				pRelationshipManager->SetRelationPartnerInGame ( CX2GameUnitoPtr( pCX2UserUnit ) );
			}
		}
	}

	return pCX2UserUnit;
}

void CX2Game::AddUserUnitIntruderByLoader( CX2Unit* pUnit )
{

	bool	bLoadPet = true;
	if( g_pMain->GetNowStateID() == CX2Main::XS_PVP_LOBBY
		|| g_pMain->GetNowStateID() == CX2Main::XS_PVP_ROOM
		|| g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME
		|| g_pMain->GetNowStateID() == CX2Main::XS_PVP_RESULT
		|| g_pMain->GetNowStateID() == CX2Main::XS_SQUARE_GAME
		|| g_pMain->GetNowStateID() == CX2Main::XS_WEDDING_GAME
		 )
	{
		bLoadPet = false;
	}

	m_UnitLoader.AddUnitLoadDataForGame( pUnit, bLoadPet );


}


void	CX2Game::IntruderStart( CX2GUUser* pIntruder, int startPosIndex )
{
	if ( pIntruder == NULL )
		return;

	DeleteUserUnit( pIntruder->GetUnitIndex() );

	m_UnitList.push_back( (CX2GameUnit*)pIntruder );
	m_UserUnitList[pIntruder->GetUnitIndex()] = pIntruder;
	//{{ robobeg : 2008-10-21
	//pIntruder->SetShow( true );
	pIntruder->SetShowObject( true );
	//}} robobeg : 2008-10-21

	//{{ dmlee 2008.11.8 SetShowObject하면서 장비가 갱신되면서, 어택박스(손, 발)에 있는 것도 켜진다. 꺼야됨.
	pIntruder->SetEnableAttackBox( L"Rfoot", false );
	pIntruder->SetEnableAttackBox( L"Lfoot", false );
	pIntruder->SetEnableAttackBox( L"Lhand2", false );
	//}} dmlee 2008.11.8 SetShowObject하면서 장비가 갱신되면서, 어택박스(손, 발)에 있는 것도 켜진다. 꺼야됨.


	UpdateUnitPointer();
	if( startPosIndex != -1 )
    {
		pIntruder->InitPosition( false, startPosIndex );
    }

	pIntruder->StateChange( pIntruder->GetStartIntrudeStateID(), true );
	g_pKTDXApp->GetDGManager()->AddObjectChain( pIntruder );
	pIntruder->SetForceInvincible( 5.0f );
	pIntruder->SetShowInvincible( 5.0f );

	m_UnitLoader.DeleteUnitLoadData( pIntruder->GetUnitUID() );





	if ( pIntruder->GetRelationshipUnitUID() > 0 )
	{
		if ( NULL != g_pData->GetRelationshipEffectManager() )
		{
			CX2RelationshipEffectManager::RelationEffectInfo RelationEffectInfo_;
			RelationEffectInfo_ = ( *(g_pData->GetRelationshipEffectManager()->GetRelationEffectInfoIndex ( 1 ) ) );
			RelationEffectInfo_.PrepareDrawRelationshipEffect( CX2RelationshipEffectManager::RelationEffectInfo::RET_ATTACH_EFFECT, pIntruder->GetUnit()->GetUID() );
		}
	}
}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

void    CX2Game::UpdateMyGameUnit( const CX2Room::SlotData* pSlotData_, CX2GUUser* pUser )
{
    if ( pSlotData_ == NULL || pSlotData_->IsMyUnit() == false || pUser == NULL )
        return;
    m_uidMySlotUID = pSlotData_->m_UnitUID;
    m_optrMyUnit = pUser;
    if ( pSlotData_->m_bHost == true )
    {
        m_uidHostSlotUID = pSlotData_->m_UnitUID;
        SetHostGameUnit( pUser );
    }
    else
    {
        UpdateHostGameUnit();
    }
}

void    CX2Game::UpdateHostGameUnit()
{
	if( m_pRoom == NULL )
    {
        m_uidHostSlotUID = 0;
        ResetHostGameUnit();
    }
    else
    {
        CX2Room::SlotData* pHostSlot = m_pRoom->GetHostSlot();
        if ( pHostSlot != NULL )
        {
            m_uidHostSlotUID = pHostSlot->m_UnitUID;
            CX2GUUser* pHostGUUser = GetUserUnitByUID( m_uidHostSlotUID );
            if ( pHostGUUser != NULL )
                SetHostGameUnit( pHostGUUser );
            else
                ResetHostGameUnit();
        }
        else
        {
            m_uidHostSlotUID = 0;
            ResetHostGameUnit();
        }
    }
}

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//void CX2Game::CheckAmIHost()
//{
//	if( m_pRoom == NULL )
//		return;
//
//
//	if( m_pRoom->GetMySlot() == NULL )
//		return;
//
//	// Unit의 UID가 아닌 User 계정의 UID 이다.
//
//	m_bHost = m_pRoom->GetMySlot()->GetIsHost();
//
//}
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

/*virtual*/ void CX2Game::UpdateUnitPointer()
{
	KTDXPROFILE();

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    UpdateHostGameUnit();
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	CheckAmIHost();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

// 	::EnterCriticalSection( &m_csGameIntruder );
// 	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
// 	{
// 		CX2GUUser* pCX2UserUnit = m_UserUnitList[i];
//
// 		if ( pCX2UserUnit == NULL )
// 		{
// 			continue;
// 		}
//
// 		if( pCX2UserUnit->GetUnitUID() == m_pRoom->GetMySlot()->m_pUnit->GetUID() )
// 			m_pMyUnit = pCX2UserUnit;
//
// 	}
// 	::LeaveCriticalSection( &m_csGameIntruder );

	if( m_fFocusTime > 0.0f )
	{
		m_fFocusTime -= m_fElapsedTime;
		if( m_fFocusTime <= 0.0f )
			m_fFocusTime = 0.0f;

		if ( NULL != GetMyUnit() )
			m_optrFocusUnit = GetMyUnit();
	}

	ResetFocusUnit();
}

void CX2Game::GameStart()
{
	if( m_bBGMOn == true )
	{
		ASSERT( NULL != m_pWorld );
		if( NULL != m_pWorld )
		{
			m_pWorld->PlayBGM();
		}
		else
		{
			StateLog( L"gamestart null world" );
		}
	}

	g_pData->GetGameUDP()->ClearRecvBuffer();
	m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), g_pMain->GetGameOption().GetCameraDistance() );

	m_GameState = GS_PLAY;

	if( g_pChatBox != NULL )
	{
		wstring wstrColor = L"#CFFFF00";
		D3DXCOLOR coTextColor(1.f, 1.f, 0.f, 1.f);
		g_pChatBox->AddChatLog( GET_STRING( STR_ID_188 ), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );
		g_pChatBox->AddChatLog( GET_STRING( STR_ID_22137 ), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );
		g_pChatBox->AddChatLog( GET_STRING( STR_ID_22138 ), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );
	}

	m_fElapsedTimeAfterLastKeyboardInput = 0.f;

}

void CX2Game::GameEnd()
{
	m_GameState = GS_END;

}



HRESULT	CX2Game::OnFrameMove( double fTime, float fElapsedTime )
{
	KTDXPROFILE();

	m_fTime			= (float)fTime;
	m_fElapsedTime	= fElapsedTime;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
    m_kFrameUDPPack.IncreaseFrameMoveCount();



//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	Verify();

	if ( m_bGameEnd == true )
	{
		if ( m_pDLGResurrect != NULL )
		{
			SAFE_DELETE_DIALOG( m_pDLGResurrect );
			m_UnitUIDToResurrect = -1;
		}
	}

	if ( g_pData->GetMessenger() != NULL )
		g_pData->GetMessenger()->OnFrameMove( fTime, fElapsedTime );


	ReverseGravityFrameMove( fTime, fElapsedTime );




#if X2_DEV_TOOLS
	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_RCONTROL) == TRUE && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		if( m_bGameWait == true )
			m_bGameWait = false;
		else
			m_bGameWait = true;
	}

	/*
	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_NUMPADPERIOD) == TRUE && g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV )
	{
	g_pData->ResetUIMajorParticle( L"UIMajorParticle.txt" );
	g_pData->ResetUIMinorParticle( L"UIMinorParticle.txt" );
	}
	*/
#endif // _SERVICE_



//#ifdef SHOOTING_TEST
//	if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_PERIOD) == TRUE )
//	{
//		g_pMain->SetShootingLevel( g_pMain->GetShootingLevel() + 1 );
//		if( g_pMain->GetShootingLevel() > 6 )
//		{
//			g_pMain->SetShootingLevel( 0 );
//		}
//	}
//#endif SHOOTING_TEST





	if( m_bGameWait == true )
	{
#if X2_DEV_TOOLS
		if( null != m_optrFocusUnit )
			m_optrFocusUnit->OnCameraMove();

		if( false == m_bFreeCamera )
		{
			m_pCamera->OnFrameMove( fTime, fElapsedTime );

			if(m_pCamera->GetEnableWorldCameraManualPlay() == true)
			{
				m_pCamera->WorldCameraKeyProcess();
			}


		}
		else
		{
			m_FPSCamera.SetEnablePositionMovement( true );

			m_FPSCamera.FrameMove( fElapsedTime * 300.f );
			m_pCamera->GetCamera().Move( m_FPSCamera.GetEyePt()->x, m_FPSCamera.GetEyePt()->y, m_FPSCamera.GetEyePt()->z );
			D3DXVECTOR3 vLookAt = *m_FPSCamera.GetWorldAhead() * 500.f + *m_FPSCamera.GetEyePt();
			m_pCamera->GetCamera().LookAt( vLookAt.x, vLookAt.y, vLookAt.z );
			m_pCamera->GetCamera().UpdateCamera( fElapsedTime );

			m_pCamera->WorldCameraKeyProcess();
		}
#endif
		return S_OK;
	}

	if( m_iItemDamageEffectIndex != 0 && null != m_optrItemDamageEffectGameUnit && m_fItemDamageEffectTime > 0.f )
	{
		m_fItemDamageEffectTime -= m_fElapsedTime;

		if( m_fItemDamageEffectTime < 0.f )
		{
			// 지헌 : 던전 사용 아이템 - 바람 -
			// 정령 소환이랑 같은 방식이니(발밑에 이펙트 만들기) 이펙트 생성도 따라 가자.
			if(m_iItemDamageEffectIndex == CX2Item::SST_WIND_SPHERE)	// 바람 구슬
			{
				SpecialItemEffect(m_iItemDamageEffectIndex);
			}
			else
			{
				SummonSpiritEffect(m_iItemDamageEffectIndex);

			}
			m_optrItemDamageEffectGameUnit.Reset();
			m_fItemDamageEffectTime = 0.f;
			m_iItemDamageEffectIndex = 0;
		}
	}
	else
	{
		m_optrItemDamageEffectGameUnit.Reset();
		m_fItemDamageEffectTime = 0.f;
		m_iItemDamageEffectIndex = 0;
	}

	m_fSkipTime += m_fElapsedTime;
	if( m_fSkipTime > 300.0f )
	{
		g_pKTDXApp->SkipFrame();
		HRESULT hr;
		if( FAILED( hr = g_pKTDXApp->GetDevice()->EvictManagedResources() ) )
		{
			switch( hr )
			{
			case D3DERR_OUTOFVIDEOMEMORY:
				ErrorLogMsg( XEM_ERROR4, "D3DERR_OUTOFVIDEOMEMORY" );
				break;

			default:
				ErrorLogMsg( XEM_ERROR4, "D3DERR_UNKNOWN" );
				break;
			}
		}

		m_fSkipTime = 0.0f;
	}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	{
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK
		KLagCheck( eKnown_LagCheckType_PacketProcess );
		P2PPacketHandler();
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	if( m_GameState == GS_PLAY )
	{
		m_fGameTime += m_fElapsedTime;

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

        if ( m_pRoom != NULL )
        {
            m_kFrameUDPPack.SetUnitUID( GetMySlotUID() );
        }
        m_kFrameUDPPack.ResetFrameUDPPack( true );

//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
        m_iPacketSendFrameMoveCount++;
        if ( m_iPacketSendFrameMoveCount >= g_pMain->GetFrameBufferNum() )
            m_iPacketSendFrameMoveCount = 0;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND



		UpdateSkillCoolTime( fElapsedTime );

		if( true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
			true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() )
		{
			D3DXVECTOR3 vLookVec = m_pCamera->GetCamera().GetLookVec();
			D3DXVec3Normalize( &vLookVec, &vLookVec );
			D3DXVECTOR3 vUpVec = m_pCamera->GetCamera().GetUpVec();
			D3DXVec3Normalize( &vUpVec, &vUpVec );

			g_pKTDXApp->GetDSManager()->SetListenerData( m_pCamera->GetCamera().GetEye(), vLookVec, vUpVec );
		}

		switch ( g_pMain->GetNowStateID() )
		{
		case CX2Main::XS_TRAINING_GAME:
		case CX2Main::XS_BATTLE_FIELD:
			break;

		default:
			{
				m_AllowFrameCount--;
				if( m_AllowFrameCount < 0 )
				{
					m_AllowFrameCount = 0;
				}
			} break;
		}

		UpdateUnitPointer();



		if( false == m_bFreeCamera && false == m_bWorldCameraEdit )
		{
			KLagCheck( eKnown_LagCheckType_LocalInputProcess );

			KeyProcess();
			if( NULL != g_pX2Room && NULL != g_pX2Room->GetMySlot() && true == g_pX2Room->GetMySlot()->m_bObserver )
			{
				// observer인 경우에 카메라
				if( m_InputData.oneLCtr == true )
				{
					ChangeFocusUnit();
				}

				if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_1 ) == TRUE )
				{
					if( NULL != GetUserUnit(0) )
					{
						SetTimerFocusUnit( GetUserUnit(0) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_2 ) == TRUE )
				{
					if( NULL != GetUserUnit(1) )
					{
						SetTimerFocusUnit( GetUserUnit(1) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_3 ) == TRUE )
				{
					if( NULL != GetUserUnit(2) )
					{
						SetTimerFocusUnit( GetUserUnit(2) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_4 ) == TRUE )
				{
					if( NULL != GetUserUnit(3) )
					{
						SetTimerFocusUnit( GetUserUnit(3) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_5 ) == TRUE )
				{
					if( NULL != GetUserUnit(4) )
					{
						SetTimerFocusUnit( GetUserUnit(4) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_6 ) == TRUE )
				{
					if( NULL != GetUserUnit(5) )
					{
						SetTimerFocusUnit( GetUserUnit(5) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_7 ) == TRUE )
				{
					if( NULL != GetUserUnit(6) )
					{
						SetTimerFocusUnit( GetUserUnit(6) );
					}
				}
				else if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_8 ) == TRUE )
				{
					if( NULL != GetUserUnit(7) )
					{
						SetTimerFocusUnit( GetUserUnit(7) );
					}
				}

			}
			else if ( NULL != GetMyUnit() )
			{
				CX2GUUser* pMyGUUser = GetMyUnit();
				// 죽었을 때 카메라
				if ( NULL != pMyGUUser && pMyGUUser->GetNowHp() <= 0.0f && pMyGUUser->GetShowObject() == false )
				{
					switch( GetGameType() )
					{
					case GT_DUNGEON:
						{
							if( m_InputData.oneX == true )
							{
								ChangeFocusUnit();
							}
						} break;

					case GT_PVP:
						{
							if( m_InputData.oneZ == true ||
								m_InputData.oneX == true )
							{
								ChangeFocusUnit();
							}
						} break;

					default:
						break;
					}
				}
			}
		}


		m_pTalkBoxMgr->OnFrameMove( fTime, fElapsedTime );
		//GuideTextFrameMove();


		m_UnitLoader.OnFrameMove( fElapsedTime );

		for( int i = 0; i < (int)m_NewGameUserList.size(); i++ )
		{
			NewGameUser* pPVPGameNewUser = &m_NewGameUserList[i];
			if( pPVPGameNewUser->m_bStart == true )
			{
				pPVPGameNewUser->m_fNewUserTime += fElapsedTime;

				if ( pPVPGameNewUser->m_fNewUserTime > 5.0f )
				{
					m_NewGameUserList.erase( m_NewGameUserList.begin() + i );
					i--;
				}
			}
			else
			{
				//백그라운드 로딩과 즉시로딩을 모두 지원하자 일단!
				if(
					g_pKTDXApp->IsFirstFrameOfSimulationLoop() &&
					m_UnitLoader.IsUnitLoadComplete( pPVPGameNewUser->m_UnitUID ) == true )
				{
					pPVPGameNewUser->m_bStart = true;
					CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID(pPVPGameNewUser->m_UnitUID);
					if( pSlotData != NULL )
					{

						CX2GUUser* pAddGameUser = AddUserUnitIntruder( pSlotData, pSlotData->m_Index );
						IntruderStart( pAddGameUser, pPVPGameNewUser->m_StartPosIndex );

						switch ( GetGameType() )
						{
						case GT_PVP:
							{
								CX2GageManager* pGageManager = CX2GageManager::GetInstance();
								if ( NULL != pGageManager )
								{
									pGageManager->InsertPvpMemberUI( *pSlotData, pAddGameUser );
									pGageManager->UpdatePvpMemberGageData( pSlotData->m_UnitUID, pAddGameUser );
								}

								GetMinorParticle()->CreateSequence( NULL,  L"INTRUDE_NOT", 0,0,0, 1000,1000, -1, 1 );
								GetMinorParticle()->CreateSequence( NULL,  L"INTRUDE_NOT_ADD", 0,0,0, 1000,1000, -1, 1 );
							}
							break;

						case GT_BATTLE_FIELD:
							{
								CX2GageManager::GetInstance()->SetPartyMemberGameUnit( pAddGameUser );

								KEGS_BATTLE_FIELD_JOIN_USER_LOAD_COMPLETE_NOT kPacket;
								kPacket.m_iLoadedUnitUID = pPVPGameNewUser->m_UnitUID;
								g_pData->GetServerProtocol()->SendPacket( EGS_BATTLE_FIELD_JOIN_USER_LOAD_COMPLETE_NOT, kPacket );
							} break;

						default:
							break;
						}
					}
				}
			}
		}






		// user, npc sync packet을 전송할 User uid 리스트 갱신
//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		m_vecUserUIDforSyncPacket.resize(0);
//#ifndef	X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD
//		::EnterCriticalSection( &m_csGameIntruder );
//#endif	X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD
//		for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
//		{
//			CX2GUUser* pCX2GUUser = m_UserUnitList[i];
//			if( pCX2GUUser != NULL )
//			{
//				m_vecUserUIDforSyncPacket.push_back( pCX2GUUser->GetUnitUID() );
//			}
//		}
//#ifndef	X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD
//		::LeaveCriticalSection( &m_csGameIntruder );
//#endif	X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD
//
//		if( g_pX2Room != NULL )
//		{
//			for( int i = 0; i < (int)g_pX2Room->GetObserverSlotNum(); i++ )
//			{
//				CX2Room::SlotData* pSlotData = g_pX2Room->GetObserverSlotData( i );
//				if( pSlotData != NULL
//					&& pSlotData->m_SlotState != CX2Room::SS_CLOSE
//					&& pSlotData->m_SlotState != CX2Room::SS_EMPTY )
//				{
//					m_vecUserUIDforSyncPacket.push_back( pSlotData->m_UnitUID );
//				}
//			}
//		}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


		if( m_bStopUnitChain == false )
		{
			//////////////////////////////////////////////////////////////////////////
			// GUSI_DIE 상태인 NPC 객체 삭제, 몬스터가 죽었다는 sync packet 전송
//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//			m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.resize(0);
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
			{
				CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
				if( NULL == pCX2GUNPC )
					continue;

				if( CX2GameUnit::GUSI_DIE != pCX2GUNPC->GetGameUnitState() )
					continue;


				if( pCX2GUNPC->IsPvpBot() == false )
					DeleteNPCUnitByUID( pCX2GUNPC->GetUID() );
			}

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//			if( IsHost() == true )
//			{
//				if( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > 0 )
//				{
//					BroadCast_XPT_UNIT_NPC_SYNC_PACK();
//				}
//			}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

#ifdef PVP_ZOOM_CAMERA
			bool bZoomOut = false;
			g_pMain->GetGameOption().DoSubZoomCameraTimer( fElapsedTime );
#endif


#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			{
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

				KLagCheck( eKnown_LagCheckType_GameUnit_FrameMove );

                for( int iUnitType = 0; iUnitType <= 1; iUnitType++ )
                {

				    for( UINT i = 0; i < m_UnitList.size(); i++ )
				    {
					    CX2GameUnit* pCX2GameUnit = m_UnitList[i];

					    if ( pCX2GameUnit != NULL )
					    {
                            if ( iUnitType == 0 && pCX2GameUnit->GetGameUnitType() != CX2GameUnit::GUT_USER
                                || iUnitType == 1 && pCX2GameUnit->GetGameUnitType() != CX2GameUnit::GUT_NPC )
                                continue;

						    // user의 경우 unit이나 unitdata가 유효하지 않은 데이타가 들어가있는 경우가 있다.
						    // 이럴경우 크래쉬방지를 위해 framemove 돌리지 않도록한다.
						    // 근본적으로 unit/unitdata가 왜 유효하지 않은지 원인파악 후 근본적인 해결이 있어야한다.
						    bool bCanDoFrameMove = true;
						    if( pCX2GameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
						    {
							    CX2GUUser *pUser = GetUserUnitByUID( pCX2GameUnit->GetUnitUID() );
							    if( pUser == NULL || pUser->GetUnit() == NULL )
							    {
								    bCanDoFrameMove = false;
							    }
						    }

						    if( bCanDoFrameMove == true )
						    {
							    pCX2GameUnit->OnFrameMove( fTime, fElapsedTime );

	    // 						if ( pCX2GameUnit != GetMyUnit()  )
	    // 						{
	    // 							pCX2GameUnit->GetGageManager()->OnFrameMove(fTime, fElapsedTime);
	    // 						}
#ifdef PVP_ZOOM_CAMERA
							    if( null != m_optrFocusUnit &&
								    g_pMain->GetGameOption().GetAutoCamera() == true &&
								    g_pMain->GetGameOption().GetSubZoomCameraTimer() <= 0.f)
							    {
								    if( IsValidUnit(pCX2GameUnit) == false )
									    continue;
								    if( pCX2GameUnit->GetNowHp() <= 0.f )
									    continue;
								    if( m_optrFocusUnit->GetTeam() == pCX2GameUnit->GetTeam() )
									    continue;
								    if( pCX2GameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
								    {
									    CX2GUNPC *pNpc = (CX2GUNPC *)pCX2GameUnit;
									    if( pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
										    continue;
								    }

								    D3DXVECTOR3 vDist = m_optrFocusUnit->GetPos() - pCX2GameUnit->GetPos();

								    float fDist = D3DXVec3LengthSq(&vDist);
								    if( fDist > 800.f * 800.f )
								    {
									    bZoomOut = true;
								    }
							    }
#endif //PVP_ZOOM_CAMERA
						    }
					    }
				    }
//#ifdef  X2OPTIMIZE_NPC_NONHOST_SIMULATION
//#ifndef X2OPTIMIZE_PUSH_PASS_BUG_FIX
//
//				    for( UINT i = 0; i < m_UnitList.size(); i++ )
//				    {
//					    CX2GameUnit* pCX2GameUnit = m_UnitList[i];
//					    if ( pCX2GameUnit != NULL )
//                            pCX2GameUnit->BackupPosition();
//                    }
//
//#endif  X2OPTIMIZE_PUSH_PASS_BUG_FIX
//#endif  X2OPTIMIZE_NPC_NONHOST_SIMULATION
                }

//#ifdef  X2OPTIMIZE_PUSH_PASS_BUG_FIX

				for( UINT i = 0; i < m_UnitList.size(); i++ )
				{
					CX2GameUnit* pCX2GameUnit = m_UnitList[i];
					if ( pCX2GameUnit != NULL )
                    {
                        pCX2GameUnit->OnFrameMove_PostProcess();
                    }
                }

//#endif  X2OPTIMIZE_PUSH_PASS_BUG_FIX

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
			}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

			CX2RidingPetManager* pRidingPetManager = CX2RidingPetManager::GetInstance();
			if ( NULL != pRidingPetManager )
				pRidingPetManager->OnFrameMove( fTime, fElapsedTime );

			if( g_pData != NULL && g_pData->GetPetManager() != NULL )
			{
				g_pData->GetPetManager()->OnFrameMove( fTime, fElapsedTime );
			}

#ifdef PVP_ZOOM_CAMERA
			if( g_pMain->GetGameOption().GetAutoCamera() == true && g_pMain->GetGameOption().GetSubZoomCameraTimer() <= 0.f )
			{
				if( bZoomOut == true)
				{
					g_pMain->GetGameOption().SetSubZoomCameraLv(1);
				}
				else
				{
					g_pMain->GetGameOption().SetSubZoomCameraLv(0);
				}
			}
#endif
		}



		ASSERT( NULL != m_pWorld );
		if( NULL != m_pWorld )
		{
			m_pWorld->OnFrameMove( fTime, fElapsedTime );
		}

		CX2GUUser* pMyGUUser = GetMyUnit();
		if ( NULL != pMyGUUser )
		{
			if( NULL != pMyGUUser && pMyGUUser->GetNowHp() > 0.0f )
			{
				UidType itemUID = m_pDropItemManager->TryGetDropItem( pMyGUUser->GetPos() );
				if( itemUID > 0 )
					Handler_EGS_GET_ITEM_REQ( itemUID );
			}
		}

 		//m_pMajorParticle->OnFrameMove( fTime, fElapsedTime );
 		//m_pMinorParticle->OnFrameMove( fTime, fElapsedTime );

 		//m_pMajorXMeshPlayer->OnFrameMove( fTime, fElapsedTime );
 		//m_pMinorXMeshPlayer->OnFrameMove( fTime, fElapsedTime );

		m_pDamageEffect->OnFrameMove( fTime, fElapsedTime );

/*		m_EffectSet.OnFrameMove( fTime, fElapsedTime );*/


		m_pDropItemManager->OnFrameMove( fTime, fElapsedTime );


		if ( NULL == g_pData->GetCashShop() ||	!g_pData->GetCashShop()->GetOpen() )
		{
			if( null != m_optrFocusUnit )
				m_optrFocusUnit->OnCameraMove();
		}





		if( false == m_bFreeCamera )
		{
			m_pCamera->OnFrameMove( fTime, fElapsedTime );

			if(m_pCamera->GetEnableWorldCameraManualPlay() == true)
			{
				m_pCamera->WorldCameraKeyProcess();
			}

		}
		else
		{
			m_FPSCamera.SetEnablePositionMovement( true );

			m_FPSCamera.FrameMove( fElapsedTime * 300.f );
			m_pCamera->GetCamera().Move( m_FPSCamera.GetEyePt()->x, m_FPSCamera.GetEyePt()->y, m_FPSCamera.GetEyePt()->z );
			D3DXVECTOR3 vLookAt = *m_FPSCamera.GetWorldAhead() * 500.f + *m_FPSCamera.GetEyePt();
			m_pCamera->GetCamera().LookAt( vLookAt.x, vLookAt.y, vLookAt.z );
			m_pCamera->GetCamera().UpVec( m_FPSCamera.GetWorldUp()->x, m_FPSCamera.GetWorldUp()->y, m_FPSCamera.GetWorldUp()->z);
			if(true == GetWorldCameraEdit())
				m_pCamera->GetCamera().SetTrackingTime(0.f);
			m_pCamera->GetCamera().UpdateCamera( fElapsedTime );

			m_pCamera->WorldCameraKeyProcess();
		}

		//g_pKTDXApp->GetDSManager()->SetListenerData( m_pCamera->GetCamera().GetEye(), m_pCamera->GetCamera().GetLookVec(), m_pCamera->GetCamera().GetUpVec() );

		if( g_pData->GetPicChar() != NULL )
			g_pData->GetPicChar()->OnFrameMove( fElapsedTime );
		if( g_pData->GetPicCharRed() != NULL )
			g_pData->GetPicCharRed()->OnFrameMove( fElapsedTime );
		if( g_pData->GetPicCharBlue() != NULL )
			g_pData->GetPicCharBlue()->OnFrameMove( fElapsedTime );
		if( g_pData->GetPicCharGreen() != NULL )
			g_pData->GetPicCharGreen()->OnFrameMove( fElapsedTime );
		if( g_pData->GetPicCharYellow() != NULL )
			g_pData->GetPicCharYellow()->OnFrameMove( fElapsedTime );
		if( g_pData->GetPicCharTechPoint() != NULL )
			g_pData->GetPicCharTechPoint()->OnFrameMove( fElapsedTime );

		for( int i = 0; i < (int)m_ScoreTimeList.size(); i++ )
		{
			ScoreTime* pScoreTime = m_ScoreTimeList[i];
			pScoreTime->m_fTime -= m_fElapsedTime;
			if( pScoreTime->m_fTime < 0.0f )
			{
				AddGameScore( pScoreTime->m_Score );
				SAFE_DELETE( pScoreTime );
				m_ScoreTimeList.erase( m_ScoreTimeList.begin() + i );
				i--;
			}
		}

		if( m_fTechPointViewTime > 0.0f )
		{
			m_fTechPointViewTime -= m_fElapsedTime;
			if( m_fTechPointViewTime <= 0.0f )
			{

				AddGameScore( m_TechPoint, 0.4f );
				m_TechPoint = 0;
				m_fTechPointViewTime = 0.0f;
			}
		}


		DetermineLastKill();


		if( IsHost() == true )
		{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			if( g_pData->GetGameUDP()->GetNonRelayUIDs().empty() == false
				|| g_pData->GetGameUDP()->GetRelayUIDs().empty() == false )
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			{
				KLagCheck( eKnown_LagCheckType_UDPProcess_Host );

				bool bRegularSend = false;

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
				bRegularSend = IsPacketSendFrame();
//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//				m_NPCPacketSendIntervalNow += m_fElapsedTime;
//				if( m_NPCPacketSendIntervalNow >= m_NPCPacketSendInterval )	// 1.0/8.0 == ( 0.125 )
//				{
//					m_NPCPacketSendIntervalNow = 0.0f;
//					bRegularSend = true;
//				}//if
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

				if ( bRegularSend == true )
				{
					if ( m_iNPCRobustPacketSendIndex >= (int) m_NPCUnitList.size()
						&& m_kFrameUDPPack.GetFrameMoveCount() >= m_dwNPCRobustPacketSendBaseFrameMoveCount + X2GAME_NPC_ROBUST_PACKET_SEND_INTERVAL )
					{
						m_iNPCRobustPacketSendIndex = 0;
						m_dwNPCRobustPacketSendBaseFrameMoveCount = m_kFrameUDPPack.GetFrameMoveCount();
					}//if
				}//if

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
				bool    bRegularNPCSent = false;
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND

				for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
				{
					CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
					if( NULL == pCX2GUNPC
						|| pCX2GUNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
					{
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
						if ( bRegularSend == true && bRegularNPCSent == false && m_iNPCRobustPacketSendIndex == i )
						{
							m_iNPCRobustPacketSendIndex++;
						}
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
						continue;
					}
//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
					if ( bRegularSend == true && bRegularNPCSent == false && m_iNPCRobustPacketSendIndex == i )
					{
						if( g_pMain->IsEnableUdpPacketOverlap() )
							pCX2GUNPC->SendReserve( 1 );
						m_iNPCRobustPacketSendIndex++;
						bRegularNPCSent = true;
					}
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
					pCX2GUNPC->SendPacket( m_kFrameUDPPack, ( bRegularSend == true ) ? false : true );
				}

                if ( bRegularSend == true )
                {
				    for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
				    {
					    CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
					    if( NULL == pCX2GUNPC
						    || pCX2GUNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
						    continue;
					    pCX2GUNPC->SendMiniSyncPacket( m_kFrameUDPPack );
				    }
                }

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//				// 일정시간 간격으로 sync 패킷 전송
//				m_NPCPacketSendIntervalNow += m_fElapsedTime;
//				if( m_NPCPacketSendIntervalNow >= m_NPCPacketSendInterval )	// 1.0/8.0 == ( 0.125 )
//				{
//					m_NPCPacketSendIntervalNow = 0.0f;
//
//					m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.resize(0);
//					for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
//					{
//						CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
//						if( NULL == pCX2GUNPC )
//							continue;
//
//						pCX2GUNPC->SendPacket( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList );
//
//						if( sizeof(KXPT_UNIT_NPC_SYNC) * m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > MTU_SIZE )
//						{
//							BroadCast_XPT_UNIT_NPC_SYNC_PACK();
//						}
//					}
//
//					if( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > 0 )
//					{
//						BroadCast_XPT_UNIT_NPC_SYNC_PACK();
//					}
//				}
//
//
//
//				// 즉시 보내야 하는 데이터인 경우 즉시 sync 패킷 전송
//				m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.resize(0);
//				for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
//				{
//					CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
//					if( NULL == pCX2GUNPC )
//						continue;
//
//					pCX2GUNPC->SendPacketImmediate( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList );
//
//					if( sizeof(KXPT_UNIT_NPC_SYNC) * m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > MTU_SIZE )
//					{
//						BroadCast_XPT_UNIT_NPC_SYNC_PACK();
//					}
//				}
//
//				if( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > 0 )
//				{
//					BroadCast_XPT_UNIT_NPC_SYNC_PACK();
//				}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

				for( UINT i=0; i<g_pData->GetPetManager()->GetPetNum(); ++i )
				{
					CX2PET *pPet = g_pData->GetPetManager()->GetPetInx(i);
					if( pPet != NULL )
					{
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
						pPet->SendPacketImmediateForce( m_kFrameUDPPack );
//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//						pPet->SendPacketImmediateForce( m_kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList );
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
					}

//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//					if( m_kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList.size() > 0 )
//					{
//						KSerBuffer buff;
//						Serialize( &buff, &m_kXPT_UNIT_PET_SYNC_PACK );
//						g_pData->GetGameUDP()->BroadCast( m_vecUserUIDforSyncPacket, XPT_UNIT_PET_SYNC_PACK, (char*)buff.GetData(), buff.GetLength() );
//
//						m_kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList.resize(0);
//					}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
				}
			}
//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
			else
			{
				for( UINT i=0; i<g_pData->GetPetManager()->GetPetNum(); ++i )
				{
					CX2PET *pPet = g_pData->GetPetManager()->GetPetInx(i);
					if( pPet != NULL )
					{
						pPet->UpdateRandomTableIndex();
					}
				}
			}
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		}




//#ifdef TRANSFORMER_TEST
//
//		if( NULL != GetMyUnit() &&
//			true == GetMyUnit()->GetTransformed() &&
//			NULL != GetMyUnit()->GetTransformer() &&
//			true == GetMyUnit()->GetTransformer()->IsLocalUnit() )
//		{
//			KLagCheck( eKnown_LagCheckType_UDPProcess_Transformer );
//
//			CX2GUNPC* pNPC = GetMyUnit()->GetTransformer();
//
////#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//			pNPC->SendPacket( m_kFrameUDPPack );
//			pNPC->SendPacketImmediate( m_kFrameUDPPack );
////#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
////			pNPC->SendPacket( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList );
////
////			if( sizeof(KXPT_UNIT_NPC_SYNC) * m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > MTU_SIZE )
////			{
////				BroadCast_XPT_UNIT_NPC_SYNC_PACK();
////			}
////
////			pNPC->SendPacketImmediate( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList );
////
////			if( sizeof(KXPT_UNIT_NPC_SYNC) * m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() > MTU_SIZE )
////			{
////				BroadCast_XPT_UNIT_NPC_SYNC_PACK();
////			}
////#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		}
//
//#endif TRANSFORMER_TEST
//


		if( m_bLastKillCheck == false )
		{
			for( int i = 0; i < (int)m_CreateNPCDataList.size(); i++ )
			{
				CreateNPCData* pCreateNPCData = m_CreateNPCDataList[i];
				pCreateNPCData->m_DelayTime -= m_fElapsedTime;
				if( pCreateNPCData->m_DelayTime <= 0.0f )
				{

					if( CX2GUNPC::NCT_MONSTER_CARD == pCreateNPCData->m_eCreateMonsterType )
					{
						CreateNPC( pCreateNPCData->m_NPCID, pCreateNPCData->m_Level, pCreateNPCData->m_bActive, pCreateNPCData->m_UID,
							pCreateNPCData->m_UnitPos, pCreateNPCData->m_bRight, pCreateNPCData->m_DelayTime, pCreateNPCData->m_iKeyCode,
							pCreateNPCData->m_eTeamNum, pCreateNPCData->m_eAIType, pCreateNPCData->m_AllyUID, 0, CX2Room::TN_RED, pCreateNPCData->m_eCreateMonsterType );
					}
					else
					{
						CreateNPC( pCreateNPCData->m_NPCID, pCreateNPCData->m_Level, pCreateNPCData->m_bActive, pCreateNPCData->m_UID,
							pCreateNPCData->m_UnitPos, pCreateNPCData->m_bRight, pCreateNPCData->m_DelayTime, pCreateNPCData->m_iKeyCode,
							pCreateNPCData->m_eTeamNum, pCreateNPCData->m_eAIType, pCreateNPCData->m_AllyUID);
					}

					SAFE_DELETE( pCreateNPCData );
					m_CreateNPCDataList.erase( m_CreateNPCDataList.begin() + i );
				}
			}
		}

		if( m_fLastkillShotTime != 0.0f )
		{
			m_fLastkillShotTime -= m_fElapsedTime;

			/// m_fLastkillShotTime이 완료 되었을 때, UI가 제거된 스크린샷을 찍도록 수정
			if( m_fLastkillShotTime < 0.0f )
			{
				m_fLastkillShotTime = 0.0f;
				if( NULL != m_pRenderTargetLastkillShot &&
					true == m_pRenderTargetLastkillShot->IsOk() )
				{
					g_pKTDXApp->GetDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x32323232, 1.0f, 0);
                    CKTDGParticleSystem::EnableParticleCulling( true );
					g_pKTDXApp->GetDGManager()->ObjectChainSort();
					g_pKTDXApp->GetDGManager()->ObjectChainNonAlphaRender();
					g_pKTDXApp->GetDGManager()->ObjectChainAlphaRender();
                    CKTDGParticleSystem::EnableParticleCulling( false );
					m_pRenderTargetLastkillShot->CopyBackBufferToSurface();
				}
			}
		}

		m_fElapsedTimeAfterLastKeyboardInput += fElapsedTime;


		if( m_fTimePauseNPCAI > 0.f )
		{
			m_fTimePauseNPCAI -= fElapsedTime;
			if( m_fTimePauseNPCAI <= 0.f )
			{
				m_fTimePauseNPCAI = 0.f;

				for( UINT i=0; i<m_NPCUnitList.size(); i++ )
				{
					CX2GUNPC* pGUNPC = m_NPCUnitList[i];
					if( NULL == pGUNPC )
						continue;

					pGUNPC->SetAIEnable( true );
				}
			}
		}

//#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        m_kFrameUDPPack.FlushFrameUDPPack();
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	}

	if( m_pShowAlarm != NULL )
	{
		m_fShowAlarmTime += fElapsedTime;
		if( m_fShowAlarmTime > m_pShowAlarm->m_fShowTime )
		{
			m_pShowAlarm = NULL;
			m_fShowAlarmTime = 0.f;

			CKTDGParticleSystem::CParticleEventSequence* pSeq = GetMajorParticle()->GetInstanceSequence( m_hDangerAlarm );
			if( pSeq != NULL && pSeq->GetShowObject() == true )
			{
				pSeq->SetShowObject(false);
			}
		}
	}

	m_pCinematicUI->OnFrameMove(fTime, fElapsedTime);

	return S_OK;
}

HRESULT	CX2Game::OnFrameRender()
{

	if( m_GameState == GS_PLAY )
	{
        CKTDGParticleSystem::EnableParticleCulling( true );
		if( m_pWorld != NULL )
		{
			m_pWorld->OnFrameRender();
		}

		g_pKTDXApp->GetDGManager()->ObjectChainSort();

		g_pKTDXApp->GetDGManager()->ObjectChainNonAlphaRender();
		g_pKTDXApp->GetDGManager()->ObjectChainAlphaRender();
        CKTDGParticleSystem::EnableParticleCulling( false );


		switch( GetGameType() )
		{
		case GT_PVP:
			{
				if( NULL == g_pData->GetPVPRoom() )
					break;

				switch( g_pData->GetPVPRoom()->GetPVPGameType() )
				{
				case CX2PVPRoom::PGT_SURVIVAL:
					{
						CX2OldGageManager::ResetTeamIndex(0,0);
					} break;
				default:
					{
						if ( g_pX2Room == NULL )
							break;

						if ( g_pX2Room->GetMySlot() == NULL )
							break;

						if ( g_pX2Room->GetMySlot()->m_bObserver == true )
						{
							CX2OldGageManager::ResetTeamIndex( 0, 4 );
						}
						else
						{
							CX2OldGageManager::ResetTeamIndex();
						}
					} break;
				}
			} break;

		case GT_DUNGEON:
			{
				if ( g_pX2Room == NULL )
					break;

				if ( g_pX2Room->GetMySlot() == NULL )
					break;

				if ( g_pX2Room->GetMySlot()->m_bObserver == true )
				{
					CX2OldGageManager::ResetTeamIndex(4,0);
				}
				else
				{
					CX2OldGageManager::ResetTeamIndex(3,0);
				}
			} break;
		}



		m_pDropItemManager->RenderName();

		for( UINT i = 0; i < m_UnitList.size(); i++ )
		{
			CX2GameUnit* pGameUnit = m_UnitList[i];

			if ( pGameUnit == NULL ||
				( GetGameType() == CX2Game::GT_BATTLE_FIELD && false == pGameUnit->DidReceiveFirstSyncPacket() ) )
			{
				continue;
			}

			//pGameUnit->GetGageManager()->UIFrameMove( g_pKTDXApp->GetTime(), g_pKTDXApp->GetElapsedTime() );

			if( pGameUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
				continue;

			pGameUnit->RenderGageUI();

			//pGameUnit->GetGageManager()->OnFrameRender();

			if( pGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
			{
				if( true == m_bRenderUserName )
				{
					pGameUnit->RenderName();
				}
			}
			else if( pGameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
			{
				// 원형맵상의 먼거리에 있는 유닛 이름은 랜더하지 않는다.
				if( true == m_sbRenderNPCName &&
					(pGameUnit->GetDistanceToCamera() < 4800
						|| true == pGameUnit->GetIsBosRaidNPC() // 레이드 몬스터는 예외처리
					))
					pGameUnit->RenderName();

				//#ifdef MAKE_GGOBONG_TEST
				//				((CX2GUNPC*)pGameUnit)->RenderCrazyMode();
				//#endif //MAKE_GGOBONG_TEST
			}
			else
			{
				// 원형맵상의 먼거리에 있는 유닛 이름은 랜더하지 않는다.
				if( pGameUnit->GetDistanceToCamera() < 4800 )
					pGameUnit->RenderName();
			}
		}


		if( NULL != m_pTalkBoxMgr )
			m_pTalkBoxMgr->OnFrameRender();

		if( g_pData != NULL && g_pData->GetPetManager() != NULL )
		{
			g_pData->GetPetManager()->OnFrameRender();
		}

		if ( NULL != CX2RidingPetManager::GetInstance() )
		{
			CX2RidingPetManager::GetInstance()->OnFrameRender();
		}

		if( NULL != g_pChatBox )
			g_pChatBox->OnFrameRender();

		//g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFont(0)->Flush();

		g_pKTDXApp->GetDGManager()->FrontUIRender();

		RenderF1Help();

		if( m_AllowFrameCount < 0 )
		{
			//g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFont(0)->Draw2DText( L"시스템 시간이 너무 빠릅니다.", 300, 300, 0xffff0000, 0xff000000, DT_CENTER, CKTDGFontManager::FS_SHELL );
		}

		if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == false )
		{
			if( m_StateString.size() > 1 )
			{	//스킬 사용 시 출력되는 스트링
			// 해외팀 수정
				g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_15_BOLD )->OutTextXY( 8, 689-20, m_StateString.c_str(), 0xffffffff,  CKTDGFontManager::FS_SHADOW, 0x00000000 );
			}
		}

		if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == false &&
			g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
		{
			if( m_pShowAlarm != NULL )
			{
				D3DXCOLOR cColor = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
				int iAlarmX = 0;
				int iAlarmY = 160;

				if( GT_DUNGEON == GetGameType() )
				{

					if( m_pShowAlarm->m_bDanger == true )
					{
						iAlarmY += 20;
					}
					switch( m_pShowAlarm->m_eColor )
					{
					case ACT_BLACK:
						cColor = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
						break;
					case ACT_RED:
						cColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
						break;
					case ACT_BLUE:
						cColor = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
						break;
					case ACT_YELLOW:
						cColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);
						break;
					case ACT_ORANGE:
						cColor = D3DXCOLOR(1.f, 0.38f, 0.1059f, 1.f);
						break;
					case ACT_MAGENTA:
						cColor = D3DXCOLOR(0.972f, 0.157f, 0.933f, 1.f);
						break;
					default:
						break;
					}


					RECT rt;
					rt.left = 0;
//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
					// OutTextMultiline안에서 GetResolutionScaleX 곱해주므로 여기서 곱해서 넘길 필요 없다.
					rt.right =  (LONG)1024;
//#else
//					rt.right =  (LONG)(1024 * g_pKTDXApp->GetResolutionScaleX());
//#endif

					rt.top = iAlarmY;
					rt.bottom = (LONG)(iAlarmY + 40);


					g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_20_BOLD )->OutTextMultiline( iAlarmX, iAlarmY,
						m_pShowAlarm->m_wstrMessage.c_str(), cColor, CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ),
						1.f, &rt, DT_CENTER|DT_TOP);
				}
				else if( GT_BATTLE_FIELD == GetGameType() )
				{
					if( true == g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
					{
						if( m_pShowAlarm->m_bDanger == true )
						{
							iAlarmY += 20;
						}
						switch( m_pShowAlarm->m_eColor )
						{
						case ACT_BLACK:
							cColor = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
							break;
						case ACT_RED:
							cColor = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
							break;
						case ACT_BLUE:
							cColor = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
							break;
						case ACT_YELLOW:
							cColor = D3DXCOLOR(1.f, 1.f, 0.f, 1.f);
							break;
						case ACT_ORANGE:
							cColor = D3DXCOLOR(1.f, 0.38f, 0.1059f, 1.f);
							break;
						case ACT_MAGENTA:
							cColor = D3DXCOLOR(0.972f, 0.157f, 0.933f, 1.f);
							break;
						default:
							break;
						}


						RECT rt;
						rt.left = 0;
//#ifdef DYNAMIC_VERTEX_BUFFER_OPT
						// OutTextMultiline안에서 GetResolutionScaleX 곱해주므로 여기서 곱해서 넘길 필요 없다.
						rt.right =  (LONG)1024;
//#else
//						rt.right =  (LONG)(1024 * g_pKTDXApp->GetResolutionScaleX());
//#endif

						rt.top = iAlarmY;
						rt.bottom = (LONG)(iAlarmY + 40);

						g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_20_BOLD )->OutTextMultiline( iAlarmX, iAlarmY,
							m_pShowAlarm->m_wstrMessage.c_str(), cColor, CKTDGFontManager::FS_SHELL, D3DXCOLOR( 0, 0, 0, 1 ),
							1.f, &rt, DT_CENTER|DT_TOP);
					}
				}
			}
		}

		//WCHAR adBuf[200] = {0,};
		//swprintf( adBuf, L"스테미너 %d", (int)GetMyUnit()->GetGageManager()->GetAttackDelayGage()->fNow );
		//m_pFontForTime->Draw2DText( timeBuf, 512, 100, D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, CKTDGFontManager::FS_SHELL );
		//g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_11_BOLD )->OutTextXY( 200, 400, adBuf, D3DXCOLOR(1,1,0,1), CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );
		//swprintf( adBuf, L"강제다운 %d", (int)GetMyUnit()->GetGageManager()->GetForceDownGage()->fNow );
		//g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_DODUM_11_BOLD )->OutTextXY( 200, 430, adBuf, D3DXCOLOR(1,1,0,1), CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );

	}

	if( true == g_pData->GetUIManager()->GetUISkillTree()->GetShowEquipSkillGuide() )
	{
		g_pKTDXApp->GetDGManager()->GetDialogManager()->GetUKFont( XUF_HEADLINE_30_NORMAL )->OutTextXY(
			512, 300, GET_STRING(STR_ID_17274), D3DXCOLOR(1,0.6f,0,1), 	CKTDGFontManager::FS_SHELL, D3DXCOLOR(0,0,0,1), NULL, DT_CENTER );
	}
	return S_OK;
}


bool CX2Game::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{


//마우스 동작 없을 때 메인메뉴 희미하게 보이도록 설정
	if ( uMsg == WM_MOUSEMOVE )
	{
		if( g_pMain->IsInheritStateMenu() )
		{
			CX2StateMenu* pStateMenu = static_cast<CX2StateMenu*>( g_pMain->GetNowState() );
			if( NULL != pStateMenu )
			{
				pStateMenu->ShowMainMenuButton();
			}
		}
	}
	if( true == m_bFreeCamera )
	{
		m_FPSCamera.HandleMessages(hWnd, uMsg, wParam, lParam);
	}

#if !X2_DEV_TOOLS
	if( uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN || uMsg == WM_NCMBUTTONDOWN )
		return true;
#endif

	return false;
}

HRESULT	CX2Game::OnResetDevice()
{
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];
		if ( pCX2GameUnit != NULL )
			pCX2GameUnit->OnResetDevice();
	}

	if( m_pWorld != NULL )
		m_pWorld->OnResetDevice();

 	//if( m_pMajorParticle != NULL )
 	//	m_pMajorParticle->OnResetDevice();
 	//if( m_pMinorParticle != NULL )
 	//	m_pMinorParticle->OnResetDevice();

	return S_OK;
}

HRESULT	CX2Game::OnLostDevice()
{
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];
		if ( pCX2GameUnit != NULL )
			pCX2GameUnit->OnLostDevice();
	}

	if( m_pWorld != NULL )
		m_pWorld->OnLostDevice();

 	//if( m_pMajorParticle != NULL )
 	//	m_pMajorParticle->OnLostDevice();
 	//if( m_pMinorParticle != NULL )
 	//	m_pMinorParticle->OnLostDevice();

	return S_OK;
}

void CX2Game::StopOtherUnit( CX2GameUnit* pNonStopUnit, float fStopTime, D3DXVECTOR3* pPos, float fDistance, bool bOtherTeam )
{
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pUnit = m_UnitList[i];
		if( pUnit == pNonStopUnit )
			continue;

		if( bOtherTeam == true || m_GameType == GT_DUNGEON || m_GameType == GT_BATTLE_FIELD )
		{
			if( pUnit->GetTeam() == pNonStopUnit->GetTeam() )
				continue;
		}



		if( pPos == NULL || fDistance <= 0 )
			pUnit->SetStopTime( fStopTime );
		else
		{
			if( GetDistance( *pPos, pUnit->GetPos() ) < fDistance )
				pUnit->SetStopTime( fStopTime );
		}
	}
}

void CX2Game::StopAllUnit( float fStopTime, D3DXVECTOR3* pPos, float fDistance, CX2GameUnit* pMyUnit, float fMyStopTime )
{
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pUnit = m_UnitList[i];
		if( pUnit == NULL )
			continue;

		if( pUnit == pMyUnit && fMyStopTime > 0.0f )
		{
			pUnit->SetStopTime( fMyStopTime );
			continue;
		}


		if( m_GameType == GT_DUNGEON && pUnit->GetTeam() != pMyUnit->GetTeam() )
		{
			// 위치가 없거나, 거리가 - 또는 0이면 모두 Stop
			if( pPos == NULL || fDistance <= 0 )
				pUnit->SetStopTime( fStopTime );
			else
			{
				// 위치와 거리가 있으면 거리내에 있는 유저만 타임 스탑
				if( GetDistance( *pPos, pUnit->GetPos() ) < fDistance )
					pUnit->SetStopTime( fStopTime );
			}
		}
		else
		{
			if( pPos == NULL || fDistance <= 0 )
				pUnit->SetStopTime( fStopTime );
			else
			{
				if( GetDistance( *pPos, pUnit->GetPos() ) < fDistance )
					pUnit->SetStopTime( fStopTime );
			}
		}
	}
}




void CX2Game::Stop2OtherUnit( CX2GameUnit* pNonStopUnit, float fStopTime, D3DXVECTOR3* pPos, float fDistance, bool bOtherTeam )
{
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pUnit = m_UnitList[i];
		if( pUnit == pNonStopUnit )
			continue;

		if( bOtherTeam == true || m_GameType == GT_DUNGEON || m_GameType == GT_BATTLE_FIELD )
		{
			if( pUnit->GetTeam() == pNonStopUnit->GetTeam() )
				continue;
		}


		if( pPos == NULL || fDistance <= 0 )
			pUnit->SetStop2Time( fStopTime );
		else
		{
			if( GetDistance( *pPos, pUnit->GetPos() ) < fDistance )
				pUnit->SetStop2Time( fStopTime );
		}
	}

}

void CX2Game::Stop2AllUnit( float fStopTime, D3DXVECTOR3* pPos, float fDistance, CX2GameUnit* pMyUnit, float fMyStopTime )
{
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		CX2GameUnit* pUnit = m_UnitList[i];
		if( pUnit == NULL )
			continue;

		if( pUnit == pMyUnit && fMyStopTime > 0.0f )
		{
			pUnit->SetStop2Time( fMyStopTime );
			continue;
		}



		if( m_GameType == GT_DUNGEON && pUnit->GetTeam() != pMyUnit->GetTeam() )
		{
			if( pPos == NULL || fDistance <= 0 )
				pUnit->SetStop2Time( fStopTime );
			else
			{
				if( GetDistance( *pPos, pUnit->GetPos() ) < fDistance )
					pUnit->SetStop2Time( fStopTime );
			}
		}
		else
		{
			if( pPos == NULL || fDistance <= 0 )
				pUnit->SetStop2Time( fStopTime );
			else
			{
				if( GetDistance( *pPos, pUnit->GetPos() ) < fDistance )
					pUnit->SetStop2Time( fStopTime );
			}
		}
	}
}

void CX2Game::ChangeFocusUnit(CX2GameUnit* pGameUnit /* = NULL */)
{
	bool bNext = false;
	bool bFind = false;
	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UserUnitList[i];

		if ( pCX2GameUnit == NULL )
		{
			continue;
		}

		if( bNext == true && pCX2GameUnit->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			m_optrFocusUnit = pCX2GameUnit;
			bFind = true;
			break;
		}
		if( m_optrFocusUnit.GetObservable() == pCX2GameUnit )
		{
			bNext = true;
		}
	}

	if( bFind == false )
	{

		for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
		{
			CX2GameUnit* pCX2GameUnit = m_UserUnitList[i];

			if ( pCX2GameUnit == NULL )
			{
				continue;
			}

			if( pCX2GameUnit->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
			{
				if ( pCX2GameUnit != pGameUnit )
				{
					m_optrFocusUnit = pCX2GameUnit;
					return;
				}
			}
		}

		m_optrFocusUnit = GetFoucusCameraNPC();
	}
}

void CX2Game::ResetFocusUnit()
{
	if( null == m_optrFocusUnit )
	{
		ChangeFocusUnit();
		return;
	}

	if( NULL != GetMyUnit() && m_optrFocusUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
	{
		if( m_optrFocusUnit == GetMyUnit() && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
		{
			ChangeFocusUnit();
		}
		else
		{
			m_optrFocusUnit = GetMyUnit();
		}
	}
}
void CX2Game::InitFocusUnit()
{
	if ( NULL != GetMyUnit() )
		m_optrFocusUnit = GetMyUnit();
}

void CX2Game::SetTimerFocusUnit( CX2GameUnit* pFocusUnit, float fFocusTime /*= 0.f*/, float fDistance /*= -1.f*/  )
{
	if ( NULL == pFocusUnit )
		return;

	m_optrFocusUnit = pFocusUnit;
	m_fFocusTime = fFocusTime;

	if( -1.f != fDistance )
	{
		m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), fDistance );
	}
	else
	{
		m_pCamera->NomalDirectCamera( m_optrFocusUnit.GetObservable(), g_pMain->GetGameOption().GetCameraDistance() );
	}

	m_pCamera->OnFrameMove( 0.0f, 0.0f );
}

D3DXVECTOR3 CX2Game::GetFocusUnitPos_LUA()
{
	if ( null != m_optrFocusUnit )
		return m_optrFocusUnit->GetPos();
	else
		return D3DXVECTOR3( -9999, -9999, -9999 );
}


CX2GUUser* CX2Game::GetUserUnitByUID( UidType unitUID )
{
	KTDXPROFILE();

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GameUnit = m_UserUnitList[i];
		if( pCX2GameUnit != NULL && pCX2GameUnit->GetUnitUID() == unitUID )
		{

			return pCX2GameUnit;
		}
	}

	return NULL;
}

CX2GUNPC* CX2Game::GetNPCUnitByType( int iNPCID )
{
	KTDXPROFILE();

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( pCX2GUNPC != NULL &&
			(int)pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == iNPCID )
		{
			return pCX2GUNPC;
		}
	}
	return NULL;
}

CX2GUNPC* CX2Game::GetNPCUnitByUID( int npcUID )
{
	KTDXPROFILE();

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( pCX2GUNPC != NULL && pCX2GUNPC->GetUID() == npcUID )
			return pCX2GUNPC;
	}
	return NULL;
}

CX2GUNPC* CX2Game::GetNPCUnitByKeyCode( int iKeyCode )
{
	KTDXPROFILE();

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( pCX2GUNPC != NULL && pCX2GUNPC->GetKeyCode() == iKeyCode )
			return pCX2GUNPC;
	}
	return NULL;
}


CX2GameUnit* CX2Game::GetUnit( int index )
{
	if ( index < 0 || index >= (int)m_UnitList.size() )
	{
		return NULL;
	}


	return m_UnitList[index];
}

CX2GUUser* CX2Game::GetUserUnit( int index )
{
	if ( index < 0 || index >= GetUserUnitListSize() )
	{
		return NULL;
	}

	return m_UserUnitList[index];
}
CX2GUNPC* CX2Game::GetNPCUnit( int index )
{
	if ( index < 0 || index >= MAX_GAME_NPC_UNIT_NUM )
	{
		return NULL;
	}
    ASSERT( index < (int) m_NPCUnitList.size() );

	return m_NPCUnitList[index];
}


void CX2Game::KillUnit( bool bNPCKiller, UidType killerUID, UidType maxDamageKillerUID, bool bNPCKilled, UidType killedUID )
{
	if( bNPCKiller == false )
	{
		CX2GameUnit* pKiller = NULL;
		if( killerUID < 0 )
			pKiller = GetNPCUnitByUID( (int)killerUID );
		else
			pKiller = GetUserUnitByUID( killerUID );
		if( pKiller != NULL )
		{
			if( bNPCKilled == false )
			{
				pKiller->m_KillUserUnitList.push_back( killedUID );
			}
			else
			{
				pKiller->m_KillNPCList.push_back( (int)killedUID );
			}
		}
		if( maxDamageKillerUID < 0 )
			pKiller = GetNPCUnitByUID( (int)maxDamageKillerUID );
		else
			pKiller = GetUserUnitByUID( maxDamageKillerUID );
		if( pKiller != NULL )
		{
			if( bNPCKilled == false )
			{
				pKiller->m_MaxDamageKillUserUnitList.push_back( killedUID );
			}
			else
			{
				pKiller->m_MaxDamageKillNPCList.push_back( (int)killedUID );
			}
		}

	}
	else
	{
		CX2GUNPC* pKiller = GetNPCUnitByUID( (int)killerUID );
		if( pKiller != NULL )
		{
			if( bNPCKilled == false )
			{
				pKiller->m_KillUserUnitList.push_back( killedUID );
			}
			else
			{
				pKiller->m_KillNPCList.push_back( (int)killedUID );
			}
		}
	}
}


int CX2Game::GetKillAssistCount( UidType userUnitUID )
{
	CX2GameUnit *pUnit = NULL;
	if( userUnitUID < 0 )
	{
		pUnit = GetNPCUnitByUID( (int)userUnitUID );
	}
	else
	{
		pUnit = GetUserUnitByUID( userUnitUID );
	}
	if( pUnit == NULL )
		return -1;

	return (int)pUnit->m_MaxDamageKillUserUnitList.size();
}

int CX2Game::GetKillCount( UidType userUnitUID )
{
	CX2GameUnit *pUnit = NULL;
	if( userUnitUID < 0 )
	{
		pUnit = GetNPCUnitByUID( (int)userUnitUID );
	}
	else
	{
		pUnit = GetUserUnitByUID( userUnitUID );
	}
	if( pUnit == NULL )
		return -1;
	return (int) pUnit->m_KillUserUnitList.size();
}

void CX2Game::SetKillScore( UidType userUnitUID, int killNum, int killedNum, int mdKillNum )
{
	CX2GameUnit* pCX2GUUser = NULL;
	if( userUnitUID < 0 )
		pCX2GUUser = GetNPCUnitByUID( (int)userUnitUID );
	else
		pCX2GUUser = GetUserUnitByUID( userUnitUID );
	if( pCX2GUUser != NULL )
	{
		//걍킬
		if( ((int)pCX2GUUser->m_KillUserUnitList.size()) > killNum )
		{
			int removeKill = pCX2GUUser->m_KillUserUnitList.size() - killNum;
			for( int i = 0; i < removeKill; i++ )
				pCX2GUUser->m_KillUserUnitList.erase( pCX2GUUser->m_KillUserUnitList.begin() );
		}
		else if( ((int)pCX2GUUser->m_KillUserUnitList.size()) < killNum )
		{
			int addKill = killNum - pCX2GUUser->m_KillUserUnitList.size();
			for( int i = 0; i < addKill; i++ )
				pCX2GUUser->m_KillUserUnitList.push_back( -1 );
		}

		//MD 킬
		if( ((int)pCX2GUUser->m_MaxDamageKillUserUnitList.size()) > mdKillNum )
		{
			int removeKill = pCX2GUUser->m_MaxDamageKillUserUnitList.size() - mdKillNum;
			for( int i = 0; i < removeKill; i++ )
				pCX2GUUser->m_MaxDamageKillUserUnitList.erase( pCX2GUUser->m_MaxDamageKillUserUnitList.begin() );
		}
		else if( ((int)pCX2GUUser->m_MaxDamageKillUserUnitList.size()) < mdKillNum )
		{
			int addKill = killNum - pCX2GUUser->m_MaxDamageKillUserUnitList.size();
			for( int i = 0; i < addKill; i++ )
				pCX2GUUser->m_MaxDamageKillUserUnitList.push_back( -1 );
		}
	}
}


bool CX2Game::DeleteUserUnitByData( CX2GameUnit* pGameUnit )
{
	//gamelog << __FUNCTION__ << fileout;

	if ( pGameUnit == NULL )
		return false;

	if( m_optrFocusUnit.GetObservable() == pGameUnit )
		ChangeFocusUnit(pGameUnit);

	vector< CX2GameUnit* >::iterator i;
	for ( i = m_UnitList.begin(); i != m_UnitList.end(); i++ )
	{
		CX2GameUnit* tempUnit = (*i);
		if ( pGameUnit ==  tempUnit )
		{
			m_UnitList.erase( i );
			break;
		}
	}

	for ( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GameUnit* tempUnit = m_UserUnitList[i];
		if ( pGameUnit == tempUnit )
		{
			m_UserUnitList[i] = NULL;
			break;
		}
	}

	pGameUnit->FinishAndClearAllBuff();
	m_pDamageEffect->UnitDeleteProcess( pGameUnit );
	CX2GameUnit::DeleteKTDGObject( pGameUnit );

	return true;
}

bool CX2Game::DeleteUserUnit( int index )
{

	if ( index < 0 || index >= GetUserUnitListSize() )
		return false;


	CX2GUUser* pCX2GUUser = m_UserUnitList[index];

	if( pCX2GUUser == NULL )
		return false;

	return DeleteUserUnitByUID( pCX2GUUser->GetUnitUID() );
}

bool CX2Game::DeleteUserUnitByUID( UidType unitUID )
{
	// 로딩중에 튕기거나 서버와 통신이 끊기는 등의 이유로 uid가 0이하의 값이 들어올 수 있는 것 같다. 자세한것은 확인해봐야 함.
	if( unitUID <= 0 )
		return false;


	if ( NULL != g_pData->GetRelationshipManager() &&
		NULL != g_pData->GetRelationshipManager()->GetMyRelationshipInfo() &&
		NULL != g_pData->GetRelationshipEffectManager() )
	{
		CX2RelationshipManager * pRelationshipManager = g_pData->GetRelationshipManager();
		CX2RelationshipManager::RelationshipInfo * pRelationshipInfo = pRelationshipManager->GetMyRelationshipInfo();
		if ( pRelationshipInfo->m_eRelationStateType !=	SEnum::RT_SOLO )
		{
			if ( pRelationshipInfo->m_iRelationTargetUserUid == unitUID )
			{
				pRelationshipManager->ResetRelationPartnerInGame();
				g_pData->GetRelationshipEffectManager()->DeleteRelationshipAttachEffectHandleByUID ( unitUID );
			}
		}
		else
		{
			g_pData->GetRelationshipEffectManager()->DeleteRelationshipAttachEffectHandleByUID ( unitUID );
		}
	}


// 	CX2GUUser* pCX2GUUser = GetUserUnitByUID( unitUID );
// 	if( pCX2GUUser == NULL )
// 	{
// 		// 서버에서 접속을 끊거나 하는 경우 게임상태에서 바로 leave_room_not를 받을 수 있기 때문에 leave_room_not를 받았을 때도
// 		// 유저 삭제를 시도한다. 그래서 정상적으로 게임을 빠져나와서 방을 나가는 경우 uid로 user 검색을 실패하게 되기 때문에 항상 에러는 아님.
// 		wstringstream wstrstm;
// 		wstrstm << unitUID;
// 		ErrorLogMsg( XEM_ERROR94, wstrstm.str().c_str() );
// 		return false;
// 	}
//

// 	if( m_optrFocusUnit.GetObservable() == pCX2GUUser )
// 		ChangeFocusUnit( pCX2GUUser );


	CX2GameUnit* pUserGameUnit = NULL;
	for( UINT i = 0; i < m_UnitList.size(); i++ )
	{
		// 오현빈 // 2013-04-08 // 해외 팀 수정사항 머지
		// UserUID와 동일한 값의 NPCUID가 있을 경우 크래쉬 발생하는 문제에 대한 예외처리
		if( NULL != m_UnitList[i] && CX2GameUnit::GUT_USER == m_UnitList[i]->GetGameUnitType() )
		{
			if ( m_UnitList[i]->GetUnitUID() == unitUID )
			{
				pUserGameUnit = m_UnitList[i];
				m_UnitList.erase( m_UnitList.begin() + i );
				break;
			}
		}
	}

	if ( NULL != pUserGameUnit )
	{
		for ( UINT i = 0; i < m_UserUnitList.size(); i++ )
		{
			CX2GUUser* pCX2GUUserTemp = m_UserUnitList[i];

			if ( pCX2GUUserTemp == NULL )
			{
				continue;
			}

			if ( pCX2GUUserTemp->GetUnitUID() == unitUID )
			{
				m_UserUnitList[i] = NULL;
				break;
			}
		}

		if( m_optrFocusUnit.GetObservable() == pUserGameUnit )
			ChangeFocusUnit( pUserGameUnit );

		pUserGameUnit->FinishAndClearAllBuff();

		// note!! 일단 npc의 경우 damageeffect에서 owner unit을 참조하는 경우가 잘 없으므로 따로 삭제 처리는 해주지 않는걸로 해두고, 나중에 수정하자
		if ( m_pDamageEffect != NULL )
			m_pDamageEffect->UnitDeleteProcess( pUserGameUnit );

		//SAFE_DELETE( pCX2GUUser );
		CX2GUUser::DeleteKTDGObject( pUserGameUnit );
	}



	// note!! 일단 npc의 경우 damageeffect에서 owner unit을 참조하는 경우가 잘 없으므로 따로 삭제 처리는 해주지 않는걸로 해두고, 나중에 수정하자
	//if ( m_pDamageEffect != NULL )
	//	m_pDamageEffect->UnitDeleteProcess( pCX2GUUser );

	//SAFE_DELETE( pCX2GUUser );
	//CX2GUUser::DeleteKTDGObject( pCX2GUUser );

	m_UnitLoader.DeleteUnitLoadData( unitUID );

	return true;
}

bool CX2Game::DeleteNPCUnit( int index )
{
	if ( index < 0 || index >= GetNPCUnitListSize() )
		return false;

	CX2GameUnit* temp = GetNPCUnit(index);

	bool bCheckUnitDelete = false;

	vector< CX2GameUnit* >::iterator i;
	for ( i = m_UnitList.begin(); i != m_UnitList.end(); i++ )
	{
		CX2GameUnit* tempUnit = (*i);
		if ( temp == tempUnit )
		{
			m_UnitList.erase( i );
			bCheckUnitDelete = true;
			break;
		}
	}

	bool bCheckNPCDelete = false;

	for ( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GameUnit* tempUnit = m_NPCUnitList[i];
		if ( tempUnit == NULL )
		{
			continue;
		}

		if ( temp == tempUnit )
		{
			m_NPCUnitList[i] = NULL;
			bCheckNPCDelete = true;
			break;
		}
	}

	if ( bCheckUnitDelete == false )
	{
		ErrorLog( XEM_ERROR116 );
	}

	if ( bCheckNPCDelete == false )
	{
		ErrorLog( XEM_ERROR116 );
	}


	CX2GUNPC* pCX2GUNPC = (CX2GUNPC*) temp;

	if( m_optrFocusUnit.GetObservable() == pCX2GUNPC )
		ChangeFocusUnit( pCX2GUNPC );


	if ( NULL != pCX2GUNPC )
	{
		pCX2GUNPC->FinishAndClearAllBuff();
		pCX2GUNPC->SetUserGrapReset();
	}


	//#ifdef HEAVY_LOG_TEST
	//	if ( pCX2GUNPC != NULL )
	//	{
	//		wstringstream tempLogStm;
	//		tempLogStm << L"DeleteNPCUnit, NPC ID : " << pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID;
	//		StateLog( tempLogStm.str().c_str() );
	//	}
	//#endif HEAVY_LOG_TEST



	//if( m_pDamageEffect != NULL )
	//	m_pDamageEffect->UnitDeleteProcess( (CX2GameUnit*) pCX2GUNPC );

	//SAFE_DELETE( pCX2GUNPC );
	CX2GUNPC::DeleteKTDGObject( pCX2GUNPC );


	return true;
}

bool CX2Game::DeleteNPCUnitByUID( const UINT uiNpcUid_ )
{
	CX2GUNPC* pCX2NPCUnit = GetNPCUnitByUID( uiNpcUid_ );
	if( pCX2NPCUnit == NULL )
		return false;

	bool bCheckUnitDelete = false;

	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];
		if( pCX2GameUnit == pCX2NPCUnit )
		{
			m_UnitList.erase( m_UnitList.begin() + i );
			bCheckUnitDelete = true;
			break;
		}
	}

	bool bCheckNPCDelete = false;

	for ( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2NPCUnitTemp = m_NPCUnitList[i];
		if ( pCX2NPCUnitTemp == NULL )
		{
			continue;
		}

		if ( pCX2NPCUnit == pCX2NPCUnitTemp )
		{
			m_NPCUnitList[i] = NULL;
			bCheckNPCDelete = true;
			break;
		}
	}

	if ( bCheckUnitDelete == false )
	{
		ErrorLog( XEM_ERROR115 );
	}

	if ( bCheckNPCDelete == false )
	{
		ErrorLog( XEM_ERROR115 );
	}

	if( m_optrFocusUnit.GetObservable() == pCX2NPCUnit )
		ChangeFocusUnit( pCX2NPCUnit );

	if ( NULL != pCX2NPCUnit )
	{
		pCX2NPCUnit->FinishAndClearAllBuff();
		pCX2NPCUnit->SetUserGrapReset();
	}

	//#ifdef HEAVY_LOG_TEST
	//	if ( pCX2NPCUnit != NULL )
	//	{
	//		wstringstream tempLogStm;
	//		tempLogStm << L"DeleteNPCUnitByUID, NPC ID : " << pCX2NPCUnit->GetNPCTemplet().m_nNPCUnitID;
	//		StateLog( tempLogStm.str().c_str() );
	//	}
	//#endif HEAVY_LOG_TEST


	//if( m_pDamageEffect != NULL )
	//	m_pDamageEffect->UnitDeleteProcess( (CX2GameUnit*) pCX2NPCUnit );

	//SAFE_DELETE( pCX2NPCUnit );
	CX2GUNPC::DeleteKTDGObject( pCX2NPCUnit );

	return true;
}

bool CX2Game::DeleteAllNPCUnit()
{
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( NULL == pCX2GUNPC )
			continue;

		//{{ mauntain : 김태환 [2012.06.14] 몬스터 카드 소환 기능 - 소환 몬스터에 한해 삭제 Pass

		if( CX2GUNPC::NCT_MONSTER_CARD == pCX2GUNPC->GetNPCCreateType()
			|| CX2GUNPC::NCT_CHANGESTAGE_REMAIN == pCX2GUNPC->GetNPCCreateType()
			)
		{
			continue;	//삭제 Pass
		}
		//}}



		pCX2GUNPC->SetUserGrapReset();

		vector<CX2GameUnit*>::iterator vItrGameUnit = m_UnitList.begin();
		while ( m_UnitList.end() != vItrGameUnit )
		{
			CX2GameUnit* pGameUnit = *vItrGameUnit;
			if ( pCX2GUNPC == pGameUnit )
			{
				pGameUnit->FinishAndClearAllBuff();
				CX2GUNPC::DeleteKTDGObject( pGameUnit );
				vItrGameUnit = m_UnitList.erase( vItrGameUnit );
				break;
			}
			else
				++vItrGameUnit;
		}

		m_NPCUnitList[i] = NULL;

		if ( null == m_optrFocusUnit )
			ChangeFocusUnit();

// 	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
// 	{
// 		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
// 		if( NULL == pCX2GUNPC )
// 			continue;
//
// 		::EnterCriticalSection( &m_csGameIntruder );
// 		for( int j = 0; j < (int)m_UnitList.size(); j++ )
// 		{
// 			CX2GameUnit* pCX2GameUnit = m_UnitList[j];
// 			if( pCX2GameUnit == pCX2GUNPC )
// 			{
// 				m_UnitList.erase( m_UnitList.begin() + j );
// 				break;
// 			}
// 		}
// 		::LeaveCriticalSection( &m_csGameIntruder );
// 		m_NPCUnitList[i] = NULL;
// 		if( m_optrFocusUnit.GetObservable() == pCX2GUNPC )
// 			ChangeFocusUnit( pCX2GUNPC );
//
// 		CX2GUNPC::DeleteKTDGObject( pCX2GUNPC );
//
 	}

    CX2GUNPC::ResetReactionSimulationCounter();

    if ( g_pKTDXApp->GetLuaBinder() != NULL )
        g_pKTDXApp->GetLuaBinder()->FlushJitCompileMode();

	return true;
}

void    CX2Game::AddNPCResourcesToDeviceList( CX2UnitManager::NPC_UNIT_ID npcID )
{
	m_UnitLoader.AddNPCUnitIDResource( npcID );
}//CX2Game::AddNPCResourcesToDeviceList()

void    CX2Game::RemoveAllNPCResourcesFromUnitLoader()
{
	m_UnitLoader.RemoveAllNPCResources();
}//CX2Game::RemoveAllNPCResourcesFromUnitLoader()

int CX2Game::GetUserUnitNum()
{
	int num = 0;
	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		if( m_UserUnitList[i] != NULL )
		{
			num++;
		}
	}

	return num;
}


int CX2Game::GetNPCUnitNum()
{
	int num = 0;
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		if( m_NPCUnitList[i] != NULL )
		{
			num++;
		}
	}

	return num;
}

int CX2Game::GetEmptyNPCIndex()
{
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		if( m_NPCUnitList[i] == NULL )
		{
			return i;
		}
	}

	return -1;
}








void CX2Game::KeyProcess()
{
	KTDXPROFILE();

	/// 캐시샵이 열려있는 경우에는 키입력 받지 않음
	if ( NULL != g_pData->GetCashShop() &&
		 g_pData->GetCashShop()->GetOpen() )
		 return;

	CX2GUUser* pMyGUUser = static_cast<CX2GUUser*>( GetMyUnit() );
	if ( NULL == pMyGUUser )
		return;

	m_InputData.Init();

	// 게임중 현재 포커스가 editbox에 있다면 움직이거나 단축스킬 등이 먹지 않도록 한다.
	bool bFocusEditBox = false;
	CKTDGUIControl* pControl = CKTDGUIDialog::GetControlFocus();
	if ( pControl != NULL )
	{
		if ( pControl->GetType() == CKTDGUIControl::UCT_IME_EDITBOX ||
			pControl->GetType() == CKTDGUIControl::UCT_EDITBOX )
		{
			bFocusEditBox = true;
		}
	}

	bool bEnableKeyProcessMisc = true;

//#ifdef PVP_BOSS_COMBAT_TEST
//	if( GetMyUnit()->GetNowStateID() == GetMyUnit()->GetFrozenState() ||
//		GetMyUnit()->GetTeam() != GetMyUnit()->GetTeamNumOriginal() )
//	{
//		bEnableKeyProcessMisc = false;
//	}
//#endif PVP_BOSS_COMBAT_TEST

	if( g_pX2Room->GetNumOfPlayers() > 1 )
	{
		if( NULL != g_pData->GetGameUDP() )
		{
			const UINT MAGIC_FRAME_COUNT_WITHOUT_P2P_PACKET_RECEIVE = 1000;
			if( g_pData->GetGameUDP()->GetElapsedFrameAfterPacketReceive() > MAGIC_FRAME_COUNT_WITHOUT_P2P_PACKET_RECEIVE )
			{
				bEnableKeyProcessMisc = false;
			}
		}
		else
		{
			bEnableKeyProcessMisc = false;
		}
	}
	if ( GetEnableAllKeyProcess() && m_AllowFrameCount > 0 /*&& true == bEnableKeyProcessMisc*/ )
	{
		InputData( GA_LEFT, m_InputData.pureDoubleLeft, m_InputData.oneDoubleLeft, m_InputData.pureLeft, m_InputData.oneLeft );
		InputData( GA_RIGHT, m_InputData.pureDoubleRight, m_InputData.oneDoubleRight, m_InputData.pureRight, m_InputData.oneRight );
		InputData( GA_UP, m_InputData.pureDoubleUp, m_InputData.oneDoubleUp, m_InputData.pureUp, m_InputData.oneUp );
		InputData( GA_DOWN, m_InputData.pureDoubleDown, m_InputData.oneDoubleDown, m_InputData.pureDown, m_InputData.oneDown );
		if ( CheckEnableKeyProcess() == true )
		{
			if( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
			{
				if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F5) == TRUE )
				{
					pMyGUUser->SetNowHp( pMyGUUser->GetMaxHp() );
				}

				if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F6) == TRUE )
				{
					pMyGUUser->ChargeMpAndEtcInTrainingGame();

#if X2_DEV_TOOLS
					if( NULL != GetMajorXMeshPlayer() )
					{
						GetMajorXMeshPlayer()->ToggleShowAttackBox();
						if( true == GetMajorXMeshPlayer()->GetShowAttackBox() )
						{
							g_pChatBox->AddChatLog( L"어택박스 보기", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR( 1, 0, 0, 1));
						}
						else
						{
							g_pChatBox->AddChatLog( L"어택박스 숨기기", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR( 1, 0, 0, 1));
						}
					}
#endif // SHOW_ATTACK_BOX_DUMMY
				}
			}

#if X2_DEV_TOOLS
			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_SPECIAL_USER && g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F6) == TRUE )
			{
				if ( g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
				{
					pMyGUUser->SetNowHp( pMyGUUser->GetMaxHp() );
				}

#if X2_DEV_TOOLS
				if( NULL != GetMajorXMeshPlayer() )
				{
					GetMajorXMeshPlayer()->ToggleShowAttackBox();
					if( true == GetMajorXMeshPlayer()->GetShowAttackBox() )
					{
						g_pChatBox->AddChatLog( L"어택박스 보기", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR( 1, 0, 0, 1));
					}
					else
					{
						g_pChatBox->AddChatLog( L"어택박스 숨기기", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR( 1, 0, 0, 1));
					}
				}
#endif // SHOW_ATTACK_BOX_DUMMY
			}

			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_SPECIAL_USER && g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F7) == TRUE )
			{
				pMyGUUser->SetNowMp( pMyGUUser->GetMaxMp() );

			//{{ kimhc // 2010.12.15 // 2010-12-23 New Character CHUNG
				if ( CX2Unit::UT_CHUNG == pMyGUUser->GetUnit()->GetType() )
				{
					CX2ChungGageData* pChungGageData = static_cast<CX2ChungGageData*>( pMyGUUser->GetGageData() );
					if ( NULL != pChungGageData )
						pChungGageData->SetNowCannonBallCount( pChungGageData->GetMaxCannonBallCount() );
						if( CX2Unit::UC_CHUNG_TACTICAL_TROOPER == pMyGUUser->GetUnitClass() )
						{
//							pChungGageData->SetExtraCannonBallCount( pChungGageData->GetMaxCannonBallCount() );
						}
				}
			//}} kimhc // 2010.12.15 //  2010-12-23 New Character CHUNG

				if ( CX2Unit::UT_ARA == pMyGUUser->GetUnit()->GetType() )
				{
					CX2AraGageData*	pAraGageData = static_cast<CX2AraGageData*>( pMyGUUser->GetGageData() );
					if ( NULL != pAraGageData )
						pAraGageData->SetNowForcePower( pAraGageData->GetMaxForcePower() );
				}


			}

			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_SPECIAL_USER )
			{
				if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F8) == TRUE )
				{
					if ( pMyGUUser->GetHyperModeCount() == pMyGUUser->GetFrameData().syncData.m_HyperModeCount )
					{
						/// 애드일 때만, DP 게이지를 풀로 채워주자.
						if ( CX2Unit::UT_ADD == pMyGUUser->GetUnit()->GetType() )
						{
							static_cast<CX2GUAdd*>( pMyGUUser )->SetDPValue( MAX_DP_GAGE_VALUE );
						}
						else
						{
							int iHyperModeCount = pMyGUUser->GetHyperModeCount();
							if ( static_cast<int>( CX2GageUI::PGUWO_ORB3 ) == ++iHyperModeCount )
								iHyperModeCount = static_cast<int>( CX2GageUI::PGUWO_ORB3 );

							pMyGUUser->SetHyperModeCount( iHyperModeCount );
						}
					}
				}
	#if X2_DEV_TOOLS
				switch( g_pMain->GetNowStateID() )
				{
				case CX2Main::XS_DUNGEON_GAME:
				case CX2Main::XS_TRAINING_GAME:
				case CX2Main::XS_BATTLE_FIELD:
					{
						if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F2) == TRUE )
						{
							//UI 메시플레이어
							g_pData->ResetUIMajorXMeshPlayer( L"UIMajorXMeshPlayer.txt" );
							g_pData->ResetUIMinorXMeshPlayer( L"UIMinorXMeshPlayer.txt" );

							//게임 메시플레이어
							g_pData->ResetGameMajorXMeshPlayer( L"GameMajorXMeshPlayer.txt" );
							g_pData->ResetGameMinorXMeshPlayer( L"GameMinorXMeshPlayer.txt" );

							//EffectSet
							g_pData->ResetUIEffectSet( L"UIEffectSet.lua" );
							g_pData->ResetGameEffectSet( L"EffectSet.lua" );

							//DamageEffect
							SAFE_DELETE( m_pDamageEffect );
							m_pDamageEffect	= new CX2DamageEffect();
							m_pDamageEffect->OpenScriptFile( L"DamageEffect.lua" );

							g_pData->GetUIMajorXMeshPlayer()->OpenScriptFile( L"UIMajorXMeshPlayer_INT.txt" );
							//g_pData->GetUIMinorXMeshPlayer()->OpenScriptFile( L"UIMinorXMeshPlayer_INT.txt" ); 현재 쓸일 없음
							g_pData->GetGameMajorXMeshPlayer()->OpenScriptFile( L"GameMajorXMeshPlayer_INT.txt" );
							//g_pData->GetGameMinorXMeshPlayer()->OpenScriptFile( L"GameMinorXMeshPlayer_INT.txt" ); 현재 쓸일 없음
							g_pData->GetUIEffectSet()->OpenScriptFile( L"UIEffectSet_INT.lua" );
							g_pData->GetGameEffectSet()->OpenScriptFile( L"EffectSet_INT.lua" );
							m_pDamageEffect->OpenScriptFile( L"DamageEffect_INT.lua" );

							//스킬트리 데이타 로드
							g_pData->ResetSkillTree( L"NewSkillTempletVer2.lua", L"SkillData.lua" ); // 10

							g_pChatBox->AddChatLog( L"GameXMeshPlayer,EffectSet, DamageEffect\nNewSkillTemplet",
								KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

							g_pChatBox->AddChatLog( L"스크립트 실시간 패치 완료 ",
								KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

							g_pChatBox->AddChatLog( L"GameParticle, 유저 스크립트 파일은 자유훈련소 재입장해야 갱신됩니다.",
								KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

							if( true == GetMyUnit()->GetRidingOn() )
							{
								CX2RidingPetManager::GetInstance()->Handler_EGS_UNSUMMON_RIDING_PET_REQ(true);
							}
							CX2RidingPetManager::GetInstance()->RefreshRidingPetScript();

#if X2_DEV_TOOLS // 김종훈, 개발자 스크립트 확장 기능 추가
							if ( g_pMain->ResetDeveloperScriptSet( L"DevScriptTable.lua" ) == true )
							{
								g_pChatBox->AddChatLog( L"DevScriptTable 세팅 완료", KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );
							}
#endif // EXPAND_DEVELOPER_SCRIPT  // 김종훈, 개발자 스크립트 확장 기능 추가
						}
					} break;
				default:
					break;
				}
	#endif //REALTIME_SCRIPT_PATCH
			}
#endif

#ifdef PVP_ZOOM_CAMERA
			if( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_SPECIAL_USER && g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState(DIK_F2) == TRUE )
			{
				g_pMain->GetGameOption().ResetAutoCamera();
				g_pMain->GetGameOption().SetAutoCamera( !g_pMain->GetGameOption().GetAutoCamera() );
			}
#endif

			if( ( true == m_bEnableCommandKeyProcess ) &&
				( g_pX2Room != NULL && g_pX2Room->GetMySlot() != NULL && g_pX2Room->GetMySlot()->m_bObserver == false ) &&
				( bFocusEditBox == false )
				)
			{
				InputData( GA_ATTACK_FAST, m_InputData.pureDoubleZ, m_InputData.oneDoubleZ, m_InputData.pureZ, m_InputData.oneZ ); //DIK_Z
				InputData( GA_ATTACK_STRONG, m_InputData.pureDoubleX, m_InputData.oneDoubleX, m_InputData.pureX, m_InputData.oneX ); //DIK_X

				if( NULL != CX2RidingPetManager::GetInstance() && true == CX2RidingPetManager::GetInstance()->GetRidingOn() )
				{
					InputData( GA_SKILL4, m_InputData.pureDoubleC, m_InputData.oneDoubleC, m_InputData.pureC, m_InputData.oneC );
				}
				else
				{

				if( true == g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() )
				{
					InputData( GA_SKILL_EX1, m_InputData.pureDoubleA, m_InputData.oneDoubleA, m_InputData.pureA, m_InputData.oneA ); //DIK_A
					InputData( GA_SKILL_EX2, m_InputData.pureDoubleS, m_InputData.oneDoubleS, m_InputData.pureS, m_InputData.oneS ); //DIK_S
					InputData( GA_SKILL_EX3, m_InputData.pureDoubleD, m_InputData.oneDoubleD, m_InputData.pureD, m_InputData.oneD ); //DIK_D
					InputData( GA_SKILL_EX4, m_InputData.pureDoubleC, m_InputData.oneDoubleC, m_InputData.pureC, m_InputData.oneC ); //DIK_C

					InputData( GA_SKILL1, m_InputData.pureDoubleQ, m_InputData.oneDoubleQ, m_InputData.pureQ, m_InputData.oneQ ); //DIK_Q
					InputData( GA_SKILL2, m_InputData.pureDoubleW, m_InputData.oneDoubleW, m_InputData.pureW, m_InputData.oneW ); //DIK_W
					InputData( GA_SKILL3, m_InputData.pureDoubleE, m_InputData.oneDoubleE, m_InputData.pureE, m_InputData.oneE ); //DIK_E
					InputData( GA_SKILL4, m_InputData.pureDoubleR, m_InputData.oneDoubleR, m_InputData.pureR, m_InputData.oneR ); //DIK_R
				}
				else
				{
					InputData( GA_SKILL1, m_InputData.pureDoubleA, m_InputData.oneDoubleA, m_InputData.pureA, m_InputData.oneA );
					InputData( GA_SKILL2, m_InputData.pureDoubleS, m_InputData.oneDoubleS, m_InputData.pureS, m_InputData.oneS );
					InputData( GA_SKILL3, m_InputData.pureDoubleD, m_InputData.oneDoubleD, m_InputData.pureD, m_InputData.oneD );
					InputData( GA_SKILL4, m_InputData.pureDoubleC, m_InputData.oneDoubleC, m_InputData.pureC, m_InputData.oneC );

					InputData( GA_SKILL_EX1, m_InputData.pureDoubleQ, m_InputData.oneDoubleQ, m_InputData.pureQ, m_InputData.oneQ );
					InputData( GA_SKILL_EX2, m_InputData.pureDoubleW, m_InputData.oneDoubleW, m_InputData.pureW, m_InputData.oneW );
					InputData( GA_SKILL_EX3, m_InputData.pureDoubleE, m_InputData.oneDoubleE, m_InputData.pureE, m_InputData.oneE );
					InputData( GA_SKILL_EX4, m_InputData.pureDoubleR, m_InputData.oneDoubleR, m_InputData.pureR, m_InputData.oneR );
				}

				}


				if( NULL != g_pChatBox && false == g_pChatBox->GetFocusChatEditBox() && GET_KEY_STATE( GA_RIDING ) == TRUE &&
					( NULL != g_pMain->GetNowState() && static_cast<CX2State*>(g_pMain->GetNowState())->GetEnableShortCutKey() ))
				{
					CX2RidingPetManager* pRidingPetManager = CX2RidingPetManager::GetInstance();
					if( NULL != pRidingPetManager && true == pRidingPetManager->GetHasRidingPet() )
					{
						if( true == pRidingPetManager->GetRidingOnState() )
						{
							if ( true == pRidingPetManager->CanPushSummonButton() )
							{
								pRidingPetManager->RefreshSummonButtonElpasedTime();

								if( false == pMyGUUser->GetRidingOn() )
									pRidingPetManager->Handler_EGS_SUMMON_RIDING_PET_REQ( pRidingPetManager->GetFirstRidingPetUID() );
								else
									pRidingPetManager->Handler_EGS_UNSUMMON_RIDING_PET_REQ();
							}
						}
						else
						{   //탑승 할 수 없을때에 대한 안내 문구
							int iStringID = ( false == pMyGUUser->GetRidingOn() ) ? STR_ID_24526 : STR_ID_24527;
							GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( iStringID ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
						}
					}
				}

			}
			if( bFocusEditBox == false )
			{
				InputData( GA_SLOT_CHANGE, m_InputData.pureDoubleSpace, m_InputData.oneDoubleSpace, m_InputData.pureSpace, m_InputData.oneSpace );	 //DIK_SPACE
			}


			InputData( GA_AWAKE, m_InputData.pureDoubleLCtr, m_InputData.oneDoubleLCtr, m_InputData.pureLCtr, m_InputData.oneLCtr ); //DIK_LCONTROL
		}
	}
	if( g_pMain != NULL && g_pData != NULL )
	{
		if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_SCROLL ) == TRUE )
		{
			if( g_pMain->IsCapturing() == false )
			{
				g_pMain->Bandi_StartVideoCapture();
			}
			else
			{
				g_pMain->Bandi_EndVidioCapture();
			}
		}
	}

	KTDXPROFILE_BEGIN( "KEYPRESS_CHECK" );
	if( true == g_pMain->GetIsPlayingTutorial() )
	{
		if( true == m_InputData.IsKeyPressed() )
		{
			m_fElapsedTimeAfterLastKeyboardInput = 0.f;
		}
	}
	KTDXPROFILE_END();

	if( g_pData->GetSlideShot()->IsPresentNow() == true )
	{
		m_InputData.Init();
	}
	if ( pMyGUUser != NULL && pMyGUUser->GetNowHp() <= 0.0f )
		return;

	if ( pMyGUUser != NULL )
		pMyGUUser->SetInputData( &m_InputData );
}

int CX2Game::LiveUserUnitNum()
{
	int iCount = 0;

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = (CX2GUUser*)m_UserUnitList[i];
		if( pCX2GUUser == NULL )
			continue;

		if( pCX2GUUser->GetGameUnitState() != CX2GameUnit::GUSI_DIE &&
			pCX2GUUser->GetNowHp() > 0.f )
		{
			iCount++;
		}
	}

	return iCount;
}

int CX2Game::LiveActiveNPCNum()
{
	int iCount = 0;
	for( unsigned i=0; i<m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*) m_NPCUnitList[i];
		if( NULL == pNPC )
			continue;

		if( pNPC->GetTeam() != CX2Room::TN_MONSTER )
			continue;

		if( pNPC->IsActiveMonster() == true )
		{
			iCount++;
		}
	}
	return iCount;
}

int	CX2Game::LiveNPCNumType_LUA( CX2UnitManager::NPC_UNIT_ID unitID )
{
	int iCount = 0;
	for( unsigned i=0; i<m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( NULL == pNPC )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == unitID )
		{
			iCount++;
		}
	}
	return iCount;
}

bool CX2Game::IsKillNPCKeyCode_LUA( int keyCode )
{
	map<int, int>::iterator it;
	it = m_mapKilledNPCKeyCode.find( keyCode );
	if( it != m_mapKilledNPCKeyCode.end() )
	{
		if( it->second > 0 )
			return true;
	}

	return false;
}


int CX2Game::GetKilledKeyCodeNPCCount( int keyCode )
{
	map<int, int>::iterator it;
	it = m_mapKilledNPCKeyCode.find( keyCode );
	if( it != m_mapKilledNPCKeyCode.end() )
	{
		return it->second;
	}

	return 0;
}

int CX2Game::LiveKeyCodeNPCNum( int keyCode )
{
	int iCount = 0;
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetTeam() != CX2Room::TN_MONSTER )
			continue;

		if( pNPC->GetKeyCode() == keyCode )
		{
			iCount++;
		}
	}
	return iCount;
}


/** @function	: LiveKeyCodeNPCNumFreeTeam
@brief			: 팀과 관계없이 해당 키코드를 NPC 지닌 개체수 반환
@param			: 조사할 키코드
@return			: 해당 키코드를 지닌 NPC 개체수
*/
int CX2Game::LiveKeyCodeNPCNumFreeTeam( int keyCode )
{
	int iCount = 0;
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetKeyCode() == keyCode )
		{
			iCount++;
		}
	}
	return iCount;
}


int CX2Game::GetAliveActiveNPCCount()
{
	int iCount = 0;
	for( unsigned i=0; i<m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*) m_NPCUnitList[i];
		if( NULL == pNPC )
			continue;

		if( pNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE || pNPC->GetNowHp() <= 0.f )
			continue;

		if( pNPC->GetTeam() != CX2Room::TN_MONSTER )
			continue;

		if( pNPC->IsActiveMonster() == true )
		{
			iCount++;
		}
	}
	return iCount;
}


int CX2Game::GetEliteNPCUnitNum()
{
	int iCount = 0;
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		if( m_NPCUnitList[i] != NULL )
		{
			CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
			if(true == m_NPCUnitList[i]->GetIsEliteMonster())
				iCount++;
		}
	}

	return iCount;
}

int CX2Game::GetEliteKeyCodeNPCUnitNum( int keyCode )
{
	int iCount = 0;
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		if( m_NPCUnitList[i] != NULL )
		{
			CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];

			if( pNPC->GetTeam() != CX2Room::TN_MONSTER )
				continue;

			if( pNPC->GetKeyCode() != keyCode )
				continue;

			if(true == m_NPCUnitList[i]->GetIsEliteMonster())
				iCount++;
		}
	}

	return iCount;
}



CX2GUNPC* CX2Game::GetKeyCodeNPC( int keyCode )
{
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetKeyCode() == keyCode )
		{
			return pNPC;
		}
	}
	return NULL;
}

CX2GUNPC* CX2Game::GetFoucusCameraNPC()
{
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetFocusCamera() == true )
		{
			return pNPC;
		}
	}
	return NULL;
}

bool CX2Game::IsValidUnit( CX2GameUnit* pUnit )
{
	if ( NULL == pUnit )
		return false;

	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];
		if( pCX2GameUnit == pUnit )
		{

			return true;
		}
	}

	return false;
}


bool CX2Game::IsAllUserHPPercentMoreThan( float fHPPercent )
{

	for ( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = m_UserUnitList[i];
		if( pCX2GUUser == NULL )
			continue;

		if( pCX2GUUser->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
			return false;

		if( 0.f != pCX2GUUser->GetMaxHp() )
		{
			if( pCX2GUUser->GetNowHp() / pCX2GUUser->GetMaxHp() < fHPPercent * 0.01f )
			{
				return false;
			}
		}

	}

	return true;
}


bool CX2Game::IsAllUserDead()
{
	for ( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = m_UserUnitList[i];
		if( pCX2GUUser == NULL )
			continue;

		if( pCX2GUUser->GetNowHp() > 0.f )
		{
			return false;
		}
	}


	return true;
}

// 모든 유저를 각성시키고, MP를 full로
void CX2Game::PartyFeverAllUser( float fAddHyperModeTime )
{
	{
		BOOST_TEST_FOREACH( CX2GUUser*, pUser, m_UserUnitList )
		{
			if( NULL == pUser )
				continue;

			if( CX2GameUnit::GUSI_DIE == pUser->GetGameUnitState() )
				continue;

			if( pUser->GetNowHp() <= 0.f )
				continue;

			/// 애드일 땐, DP 150 회복
			if ( CX2Unit::UT_ADD == pUser->GetUnitType() )
				pUser->UpNowDPValue( 3000.f );
			else
				pUser->ForceChangeHyperModeWithoutMotion( fAddHyperModeTime );

			pUser->SetNowMp( pUser->GetMaxMp() );

		}
	}



	if( NULL != GetFocusUnit() )
		GetFocusUnit()->PlaySound( L"Alarm_Fever.ogg" );
	else if( NULL != GetMyUnit() )
		GetMyUnit()->PlaySound( L"Alarm_Fever.ogg" );



	GetMajorParticle()->CreateSequence( NULL,  L"Fever_Mode", 0,0,0 );
	GetMajorParticle()->CreateSequence( NULL,  L"Fever_Mode01", 0,0,0 );
	GetMajorParticle()->CreateSequence( NULL,  L"Fever_Mode02", 0,0,0 );
	GetMajorParticle()->CreateSequence( NULL,  L"Flare_Fever_Mode03", 512,384,0 );

}



bool CX2Game::RebirthUserUnit( UidType unitUID, int startPosIndex )
{
	if( g_pX2Room->IsNpcSlot( unitUID ) == true )
	{
		CX2GUNPC* pNpc = GetNPCUnitByUID( (int)unitUID );
		if( pNpc == NULL )
			return false;

		//pNpc->ReInit(false, startPosIndex );
		pNpc->CX2GameUnit::ReInit(false, startPosIndex);
//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        pNpc->ReInitExtraWork();
//#endif SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		pNpc->InitPosition( false, startPosIndex );
		pNpc->SetNowHp( pNpc->GetMaxHp() );
		pNpc->SetShowObject( true );


		pNpc->SetForceInvincible( 5.f, 5.f );
//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
        pNpc->StateChangeForce( pNpc->GetStartState() );
//#else  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		pNpc->StateChange( pNpc->GetStartState() );
//#endif SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
		pNpc->SetAIEnable(true);

		pNpc->PlaySound( L"Revival.ogg" );
		return true;
	}

	CX2GUUser* pCX2GUUser = GetUserUnitByUID( unitUID );
	if( pCX2GUUser == NULL )
		return false;

	pCX2GUUser->ReInit( false, startPosIndex );

	pCX2GUUser->SetForceInvincible( 5.0f );
	pCX2GUUser->SetShowInvincible( 5.0f );
	pCX2GUUser->StateChange( pCX2GUUser->GetStartIntrudeStateID(), true );

	UpdateUnitPointer();

	if( GetMyUnit() == pCX2GUUser )
	{
		m_optrFocusUnit = GetMyUnit();
	}

	if( NULL != pCX2GUUser )
	{
		pCX2GUUser->PlaySound( L"Revival.ogg" );
	}

	return true;
}

CX2GUNPC*

CX2Game::CreateNPC( CX2UnitManager::NPC_UNIT_ID unitID, int level, bool bActive, int uid,
						const D3DXVECTOR3& vPos, bool bRight, float fDelayTime, int iKeyCode,
						CX2Room::TEAM_NUM eTeam /*= CX2Room::TN_MONSTER*/, CX2NPCAI::NPC_AI_TYPE eAIType /*= CX2NPCAI::NAT_NORMAL*/,
						UidType iAllyUID /*= -1*/ , KAttribEnchantNpcInfo* _kAttribEnchantNpcInfo /* = NULL */
						, CX2Room::TEAM_NUM eAllyTeam /*= CX2Room::TN_NONE */

						, CX2GUNPC::NPC_CREATE_TYPE eCreateMonsterType /*= NCT_NONE */		/// 몬스터 생성 타입

						)
{
	CX2GUNPC* pNPC = NULL;

	if( m_bLastKillCheck == true )
	{

		if(  CX2GUNPC::NCT_NONE == eCreateMonsterType
			)


			return NULL;
	}

	if( fDelayTime <= 0.0f )
	{
		int index;
		for( index = 0; index < (int)m_NPCUnitList.size(); index++ )
		{
			if( m_NPCUnitList[index] == NULL )
				break;
		}
		ASSERT( index < (int)m_NPCUnitList.size() );		// 만약에 빈자리가 없으면???




		CX2GUUser* pOwnerGUUser = NULL;
		if( CX2NPCAI::NAT_ALLY == eAIType && -1 != iAllyUID )
			pOwnerGUUser = GetUserUnitByUID( iAllyUID );

		// 새로 몬스터 생성
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
		const CX2UnitManager::NPCUnitInfo*	pNPCUnitInfo = g_pData->GetUnitManager()->GetNPCUnitInfo( unitID );
		ASSERT( pNPCUnitInfo != NULL );
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//		const CX2UnitManager::NPCUnitTemplet* ptemplet = g_pData->GetUnitManager()->GetNPCUnitTemplet( unitID );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING

//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
		if ( pNPCUnitInfo != NULL )
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//		if ( ptemplet != NULL )
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
		{
//#ifndef SERV_CREATE_ELITE_FOR_ADMIN
//			CX2GUNPC*
//#endif SERV_CREATE_ELITE_FOR_ADMIN
//#ifdef	X2OPTIMIZE_NPC_LUASPACE_SHARING
			CX2GUNPC* pNPC = CX2GUNPC::CreateGUNPC( index, eTeam, uid, pNPCUnitInfo, eAIType, pOwnerGUUser );
//#else	X2OPTIMIZE_NPC_LUASPACE_SHARING
//			CX2GUNPC* pNPC = CX2GUNPC::CreateGUNPC( index, eTeam, ptemplet->m_LuaFileName.c_str(), uid, ptemplet, eAIType, pOwnerGUUser );
//#endif	X2OPTIMIZE_NPC_LUASPACE_SHARING
			if( NULL == pNPC )
			{
				ASSERT( "CreateNPC out of memory" );
				return NULL;
			}


				pNPC->SetNPCCreateType( eCreateMonsterType );


			if( NULL != pOwnerGUUser )
			{

				if(  CX2GUNPC::NCT_NONE == eCreateMonsterType )

				{
					pNPC->SetScaleByUnit( pOwnerGUUser->GetScaleByUnit() );
				}

				//{{ JHKang / 강정훈 / 2011/02/21 / NPC에 소환한 Unit UID 저장
				pNPC->SetOwnerGameUnit( pOwnerGUUser );
				//}}
			}

			if( eAllyTeam != CX2Room::TN_NONE )
			{
				pNPC->SetOwnerGameUnit( pOwnerGUUser );
				pNPC->SetAllyTeam( eAllyTeam );
			}


			pNPC->Init();
			if( false == pNPC->SetPosition( vPos, bRight ) )
			{
				// 몬스터 위치를 제대로 지정할 수 없다면 지워버리자.
				ASSERT( !"CreateNPC SetPosition error" );
				SAFE_DELETE_KTDGOBJECT( pNPC );
				return NULL;
			}


			if ( GetGameType() == CX2Game::GT_DUNGEON )
			{
				CX2DungeonGame* pDungeonGame = (CX2DungeonGame*)this;
				CX2Dungeon* pDungeon = pDungeonGame->GetDungeon();

				ASSERT( NULL != pDungeon );
				ASSERT( NULL != pDungeon->GetNowStage() );
				ASSERT( NULL != pDungeon->GetNowStage()->GetNowSubStage() );
				ASSERT( NULL != pDungeon->GetNowStage()->GetNowSubStage()->GetSubStageData() );

				if( NULL != pDungeon )
				{
					map< int, string >::iterator mit;
					if( NULL != pDungeon->GetNowStage() &&
						NULL != pDungeon->GetNowStage()->GetNowSubStage() &&
						NULL != pDungeon->GetNowStage()->GetNowSubStage()->GetSubStageData() )
					{
						mit = pDungeon->GetNowStage()->GetNowSubStage()->GetSubStageData()->m_mapKeyCodeNStartState.find( iKeyCode );
						if ( mit != pDungeon->GetNowStage()->GetNowSubStage()->GetSubStageData()->m_mapKeyCodeNStartState.end() )
						{
							pNPC->StateChange( mit->second.c_str() );
						}
					}
				}
			}
			else if ( GetGameType() == CX2Game::GT_PVP )
			{
				CX2Room::RoomNpcSlot* pSlotData = g_pX2Room->GetNpcSlotData(pNPC->GetUID());
				if( NULL != pSlotData )
				{
					CX2GageManager* pGageManager = CX2GageManager::GetInstance();
					if ( NULL != pGageManager )
					{
						CX2GameUnit* pGameUnitMember
							= GetNPCUnitByUID( static_cast<int>( pSlotData->m_iNpcUid ) );
						pGageManager->InsertPvpMemberUI( *pSlotData, pNPC );
						pGageManager->UpdatePvpMemberGageData( pSlotData->m_iNpcUid, pNPC );
					}
				}
			}

			SetNpcHardLevel( pNPC, level );

			pNPC->InitFullName();

			if( NULL != GetMyUnit() )
				pNPC->InitNameColor( GetMyUnit()->GetUnitLevel() );

			pNPC->SetActiveMonster( bActive );
			pNPC->SetFocusCamera( false );
			pNPC->SetKeyCode( iKeyCode );







			if( CX2GUNPC::NCT_NONE == pNPC->GetNPCCreateType()
				|| CX2GUNPC::NCT_CHANGESTAGE_REMAIN == pNPC->GetNPCCreateType()
				)

				// 유저가 소환한 몬스터의 stat은 예외적으로 처리한다
				SetUserSummonedNPCInfo(eAIType, iAllyUID, pNPC, level, unitID);





//#ifdef TRANSFORMER_TEST
//
//			if( CX2NPCAI::NAT_NO_BRAIN == eAIType )
//			{
//				CX2GUUser* pUser = GetUserUnitByUID( iAllyUID );
//				if( NULL != pUser )
//				{
//					pUser->SetTransformed( true );
//					pUser->SetTransformer( pNPC );
//					pUser->StateChange( pUser->GetCommonState().m_Transformed );
//					pUser->SetShowObject( false );
//
//					pNPC->SetTransformed( true );
//					pNPC->SetTransformerOwnerUID( iAllyUID );
//					pNPC->SetNowHp( pUser->GetNowHp() );
//
//				}
//				else
//				{
//					pNPC->SetNowHp( 0.f );
//				}
//			}
//#endif TRANSFORMER_TEST


			if(_kAttribEnchantNpcInfo != NULL)
			{
				pNPC->ApplyEnchantMonster( *_kAttribEnchantNpcInfo );
			}


			//{{ mauntain : 김태환 [2012.06.14] 몬스터 카드 소환 기능 - 소환한 NPC의 UID 저장

			if( NULL != pNPC && CX2GUNPC::NCT_MONSTER_CARD == pNPC->GetNPCCreateType() )
			{
                CX2GUUser* pUser = pNPC->GetOwnerGameUnit();

				if ( pNPC->GetNPCTemplet().m_nNPCUnitID != CX2UnitManager::NUI_SHASHA_STAGE0 &&
					 pNPC->GetNPCTemplet().m_nNPCUnitID != CX2UnitManager::NUI_SHASHA_STAGE1 &&
					 pNPC->GetNPCTemplet().m_nNPCUnitID != CX2UnitManager::NUI_SHASHA_STAGE2 )
				{
					if( NULL != pUser )		//USer에게 소환된 NPC UID 저장
					{
					// 4마리를 동시에 삭제 시켜주기위해 벡터에 따로 담는다
					if( static_cast<CX2UnitManager::NPC_UNIT_ID>( EVENT_MONSTER_CARD_ALLY_ID ) == unitID )
					{
						pUser->AccessSummonMonsterCardData().SetSummonMonsterUIDInVec( pNPC->GetUID() );
					}
						pUser->AccessSummonMonsterCardData().SetSummonMonsterUID( pNPC->GetUID() );

						pUser->AccessSummonMonsterCardData().SetSummonMonsterTime( pNPC->GetSummonTime() );
						if( NULL != g_pData && NULL != g_pData->GetUIEffectSet() )
						{
							CX2EffectSet::Handle hEffect =  g_pData->GetUIEffectSet()->PlayEffectSet( L"EffectSet_Summon_Pet", (CX2GameUnit*)pNPC );

							if( hEffect != INVALID_EFFECTSET_HANDLE )
						    {
                                if ( CX2EffectSet::EffectSetInstance* pEffectSetInst = g_pData->GetUIEffectSet()->GetEffectSetInstance( hEffect ) )
                                    pEffectSetInst->m_vPosition = pNPC->GetLandPos();
                            }
						}
					}
					else	//소환 요청 후, NOT가 오기 전 User가 이탈했다면 해당 NPC 죽임
					{
						pNPC->SetNowHp( 0.f );
					}
				}
			}
			//}}

			AddUnit( (CX2GameUnit*)pNPC );
			SetNPCUnit( index, pNPC );
			g_pKTDXApp->GetDGManager()->AddObjectChain( pNPC );
		}
	}
	else
	{
		CreateNPCData* pCreateNPCData	= new CreateNPCData;
		pCreateNPCData->m_NPCID			= unitID;
		pCreateNPCData->m_Level			= level;
		pCreateNPCData->m_bActive		= bActive;
		pCreateNPCData->m_UID			= uid;
		pCreateNPCData->m_UnitPos		= vPos;
		pCreateNPCData->m_bRight		= bRight;
		pCreateNPCData->m_DelayTime		= fDelayTime;
		pCreateNPCData->m_iKeyCode		= iKeyCode;
		pCreateNPCData->m_eTeamNum		= eTeam;
		pCreateNPCData->m_eAIType		= eAIType;
		pCreateNPCData->m_AllyUID		= iAllyUID;

		pCreateNPCData->m_eCreateMonsterType = eCreateMonsterType;



		m_CreateNPCDataList.push_back( pCreateNPCData );
	}

	return pNPC;
}


void CX2Game::CreateNPCReq( CX2UnitManager::NPC_UNIT_ID unitID, int level, bool bActive,
						   D3DXVECTOR3 vPos, bool bRight, float fDelayTime, bool bNoDrop, int iKeyCode,
						   CX2Room::TEAM_NUM eTeam /*= CX2Room::TN_NONE*/, CX2NPCAI::NPC_AI_TYPE eAIType /*= CX2NPCAI::NAT_NORMAL*/,
						   UidType iAllyUID /*= -1*/, bool bElite /* = false */
						   , CX2Room::TEAM_NUM eAllyTeam /*= CX2Room::TN_NONE */

						   , CX2GUNPC::NPC_CREATE_TYPE eCreateMonsterType /*= false */			//몬스터 카드로 소환되었는지에 대한 여부

						   )
{
	if( IsHost() == false )
		return;

	if ( m_bGameEnd == true )
		return;

	KNPCUnitReq kNPCUnitReq;
	kNPCUnitReq.Init();

	kNPCUnitReq.m_NPCID			= unitID;
	kNPCUnitReq.m_Level			= level;
	kNPCUnitReq.m_bActive		= bActive;
	kNPCUnitReq.m_vPos.x		= vPos.x;
	kNPCUnitReq.m_vPos.y		= vPos.y;
	kNPCUnitReq.m_vPos.z		= vPos.z;
	kNPCUnitReq.m_bIsRight		= bRight;
	kNPCUnitReq.m_bNoDrop		= bNoDrop;
	kNPCUnitReq.m_KeyCode		= (char)iKeyCode;
	kNPCUnitReq.m_cTeamNum		= (char) eTeam;
	kNPCUnitReq.m_cAIType		= (char) eAIType;
	kNPCUnitReq.m_iAllyUID		= iAllyUID;
	kNPCUnitReq.m_cAllyTeam		= static_cast<char>( eAllyTeam );

	kNPCUnitReq.m_cCreateMonsterType |= eCreateMonsterType;		//몬스터 카드로 소환되었는지에 대한 여부


	if( eTeam != CX2Room::TN_MONSTER )
		kNPCUnitReq.m_bAllyNpc = true;

	kNPCUnitReq.m_bAttributeNpc	= bElite;

	kNPCUnitReq.m_fDelayTime	= fDelayTime;

	KEGS_NPC_UNIT_CREATE_REQ kPacket;
	kPacket.m_vecNPCUnitReq.push_back( kNPCUnitReq );

	g_pData->GetServerProtocol()->SendPacket( EGS_NPC_UNIT_CREATE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_NPC_UNIT_CREATE_ACK, 60.f );
}

void CX2Game::CreateNPCReq_LUA( int unitID, int level, bool bActive,
							   D3DXVECTOR3 vPos, bool bRight, float fDelayTime, bool bNoDrop )
{
	if( IsHost() == false )
		return;


	CreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, fDelayTime, bNoDrop );
}

void CX2Game::CreateNPCReqElite( int unitID, int level, bool bActive,
								D3DXVECTOR3 vPos, bool bRight, D3DXVECTOR3 vfDelayTimeNKeyCode, bool bNoDrop, bool bElite /* = false */ )
{
	if( IsHost() == false )
		return;
	CreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, vfDelayTimeNKeyCode.x, bNoDrop, (int)vfDelayTimeNKeyCode.y, CX2Room::TN_MONSTER, CX2NPCAI::NAT_NORMAL, -1, bElite );
}

void CX2Game::CreateNPCReqAlly( int unitID, int level, bool bActive,
	D3DXVECTOR3 vPos, bool bRight, D3DXVECTOR3 vfDelayTimeNKeyCode, bool bNoDrop, CX2Room::TEAM_NUM eTeam/* = CX2Room::TN_RED*/ )
{
	if( IsHost() == false )
		return;
	CreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, vfDelayTimeNKeyCode.x, bNoDrop, (int)vfDelayTimeNKeyCode.y, eTeam, CX2NPCAI::NAT_ALLY, -1, false );
}


void CX2Game::CreateNPCReq_LUA2( int unitID, int level, bool bActive,
								D3DXVECTOR3 vPos, bool bRight, D3DXVECTOR3 vfDelayTimeNKeyCode, bool bNoDrop )
{
	if( IsHost() == false )
		return;

	CreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, vfDelayTimeNKeyCode.x, bNoDrop, (int)vfDelayTimeNKeyCode.y );
}


void CX2Game::PushCreateNPCReq( CX2UnitManager::NPC_UNIT_ID unitID, int level, bool bActive,
							   D3DXVECTOR3 vPos, bool bRight, float fDelayTime, bool bNoDrop, int iKeyCode /*= 0*/,
							   CX2Room::TEAM_NUM eTeam /*= CX2Room::TN_MONSTER*/, CX2NPCAI::NPC_AI_TYPE eAIType /*= CX2NPCAI::NAT_NORMAL*/,
							   UidType iAllyUID /*= -1*/ )
{
	if( IsHost() == false )
		return;

	KNPCUnitReq kNPCUnitReq;
	kNPCUnitReq.Init();

	kNPCUnitReq.m_NPCID			= unitID;
	kNPCUnitReq.m_Level			= level;
	kNPCUnitReq.m_bActive		= bActive;
	kNPCUnitReq.m_vPos.x		= vPos.x;
	kNPCUnitReq.m_vPos.y		= vPos.y;
	kNPCUnitReq.m_vPos.z		= vPos.z;
	kNPCUnitReq.m_bIsRight		= bRight;
	kNPCUnitReq.m_fDelayTime	= fDelayTime;
	kNPCUnitReq.m_bNoDrop		= bNoDrop;
	kNPCUnitReq.m_KeyCode		= (char) iKeyCode;
	kNPCUnitReq.m_cTeamNum		= (char) eTeam;
	kNPCUnitReq.m_cAIType		= (char) eAIType;
	kNPCUnitReq.m_iAllyUID		= iAllyUID;

	if( eTeam != CX2Room::TN_MONSTER )
		kNPCUnitReq.m_bAllyNpc = true;


	m_PushCreateNPCReqList.m_vecNPCUnitReq.push_back( kNPCUnitReq );
}

void CX2Game::PushCreateNPCReq_Lua( int unitID, int level, bool bActive,
								   D3DXVECTOR3 vPos, bool bRight, float fDelayTime, bool bNoDrop, int iKeyCode )
{
	if( IsHost() == false )
		return;

	PushCreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, fDelayTime, bNoDrop, iKeyCode );
}

void CX2Game::FlushCreateNPCReq()
{
	if( IsHost() == false )
		return;

	g_pData->GetServerProtocol()->SendPacket( EGS_NPC_UNIT_CREATE_REQ, m_PushCreateNPCReqList );
	g_pMain->AddServerPacket( EGS_NPC_UNIT_CREATE_ACK, 60.f );

	m_PushCreateNPCReqList.m_vecNPCUnitReq.resize(0);
}

void CX2Game::CreateNPCReq_LUA3( int unitID, int level, bool bActive, D3DXVECTOR3 vPos, bool bRight, D3DXVECTOR3 vfDelayTimeNKeyCode, bool bNoDrop, int iTeam, int iAIType, UidType iAllyUID )
{
	if( IsHost() == false )
		return;

	CreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, vfDelayTimeNKeyCode.x, bNoDrop, (int)vfDelayTimeNKeyCode.y, (CX2Room::TEAM_NUM)iTeam, (CX2NPCAI::NPC_AI_TYPE)iAIType, iAllyUID );
}

void CX2Game::PushCreateNPCReq_LUA3( int unitID, int level, bool bActive, D3DXVECTOR3 vPos, bool bRight, D3DXVECTOR3 vfDelayTimeNKeyCode, bool bNoDrop, int iTeam, int iAIType, UidType iAllyUID )
{
	if( IsHost() == false )
		return;

	PushCreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)unitID, level, bActive, vPos, bRight, vfDelayTimeNKeyCode.x, bNoDrop, (int)vfDelayTimeNKeyCode.y, (CX2Room::TEAM_NUM)iTeam, (CX2NPCAI::NPC_AI_TYPE)iAIType, iAllyUID );
}





bool CX2Game::Handler_EGS_NPC_FIELD_MIDDLE_BOSS_UNIT_CREATE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 중간 보스 등장에 대한 ACK
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KPacketOK kEvent;
	DeSerialize( pBuff, &kEvent );
	CX2PacketLog::PrintLog( &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_NPC_UNIT_CREATE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			return true;
		}
	}

	return false;
}

bool CX2Game::Handler_EGS_NPC_UNIT_CREATE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KPacketOK kEvent;
	DeSerialize( pBuff, &kEvent );
	CX2PacketLog::PrintLog( &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_NPC_UNIT_CREATE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			return true;
		}
	}

	return false;
}

bool CX2Game::Handler_EGS_NPC_UNIT_CREATE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_NPC_UNIT_CREATE_NOT kEvent;
	DeSerialize( pBuff, &kEvent );
	CX2PacketLog::PrintLog( &kEvent );


	for( int i = 0; i < (int)kEvent.m_vecNPCUnitAck.size(); i++ )
	{
		KNPCUnitNot* pKNPCUnitNot = &kEvent.m_vecNPCUnitAck[i];
		CreateNPC( (CX2UnitManager::NPC_UNIT_ID)pKNPCUnitNot->m_kNPCUnitReq.m_NPCID,
			pKNPCUnitNot->m_kNPCUnitReq.m_Level,
			pKNPCUnitNot->m_kNPCUnitReq.m_bActive,
			pKNPCUnitNot->m_kNPCUnitReq.m_UID,
			D3DXVECTOR3( pKNPCUnitNot->m_kNPCUnitReq.m_vPos.x, pKNPCUnitNot->m_kNPCUnitReq.m_vPos.y, pKNPCUnitNot->m_kNPCUnitReq.m_vPos.z ),
			pKNPCUnitNot->m_kNPCUnitReq.m_bIsRight,
			pKNPCUnitNot->m_kNPCUnitReq.m_fDelayTime,
			(int) pKNPCUnitNot->m_kNPCUnitReq.m_KeyCode,
			(CX2Room::TEAM_NUM) pKNPCUnitNot->m_kNPCUnitReq.m_cTeamNum,
			(CX2NPCAI::NPC_AI_TYPE) pKNPCUnitNot->m_kNPCUnitReq.m_cAIType,
			pKNPCUnitNot->m_kNPCUnitReq.m_iAllyUID
			, false
			, static_cast<CX2Room::TEAM_NUM>(pKNPCUnitNot->m_kNPCUnitReq.m_cAllyTeam)

			, static_cast<CX2GUNPC::NPC_CREATE_TYPE>( pKNPCUnitNot->m_kNPCUnitReq.m_cCreateMonsterType )		//몬스터 카드로 소환되었는지에 대한 여부

			);

	}

//{{ kimhc // 2010.4.23 // 비밀던전 작업(Extra 모드)
	// 던전에서 몬스터를 임의로 생성해서 성공했을 때
	// 해당 몬스터가 substage clear 조건에 있는 경우
	// 클리어 조건의 해당 몬스터 original count를 update 시켜줌
	if ( GetGameType() == GT_DUNGEON )
	{
		CX2Dungeon* pDungeon = static_cast< CX2DungeonGame* >( this )->GetDungeon();

		if ( pDungeon != NULL &&
			 pDungeon->GetNowStage() != NULL &&
			 pDungeon->GetNowStage()->GetNowSubStage() != NULL )
		{
			pDungeon->GetNowStage()->GetNowSubStage()->SetMonsterCountForSubStageClearCheck();
		}
	}
//}} kimhc // 2010.4.23 // 비밀던전 작업(Extra 모드)
	return true;
}

bool CX2Game::Handler_EGS_USE_FINALITY_SKILL_REQ()
{
	KEGS_USE_FINALITY_SKILL_REQ kPacket;

	kPacket.m_iItemUID = 0;
	kPacket.m_bNoConsume = true;

	g_pData->GetServerProtocol()->SendPacket( EGS_USE_FINALITY_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_USE_FINALITY_SKILL_ACK );

	return true;
}

bool CX2Game::Handler_EGS_USE_FINALITY_SKILL_REQ( UidType itemUID_ )
{
	KEGS_USE_FINALITY_SKILL_REQ kPacket;

	kPacket.m_iItemUID = itemUID_;
	kPacket.m_bNoConsume = false;

	g_pData->GetServerProtocol()->SendPacket( EGS_USE_FINALITY_SKILL_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_USE_FINALITY_SKILL_ACK );

	return true;
}

bool CX2Game::Handler_EGS_USE_FINALITY_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_USE_FINALITY_SKILL_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_USE_FINALITY_SKILL_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			if( true == kEvent.m_bNoConsume )
				return true;

			g_pData->GetUIManager()->GetUIInventory()->UpdateInventorySlotList( kEvent.m_vecKInventorySlotInfo );

			const int iItemNum = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );

			switch ( iItemNum )
			{
			case 100:
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_26389 ), KEGS_CHAT_REQ::CPT_TOTAL, D3DXCOLOR(0,1,0,1), L"#C00FF00", false );
				break;

			case 50:
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_26390 ), KEGS_CHAT_REQ::CPT_TOTAL, D3DXCOLOR(0,1,0,1), L"#C00FF00", false );
				break;

			case 10:
				g_pChatBox->AddChatLog( GET_STRING( STR_ID_26391 ), KEGS_CHAT_REQ::CPT_TOTAL, D3DXCOLOR(0,1,0,1), L"#C00FF00", false );
				break;

			default:
				break;
			}
			return true;
		}
	}

	return false;
}

bool CX2Game::Handler_EGS_CREATE_ATTRIB_NPC_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_CREATE_ATTRIB_NPC_NOT kEvent;
	DeSerialize( pBuff, &kEvent );
	//CX2PacketLog::PrintLog( &kEvent );


	for( int i = 0; i < (int)kEvent.m_vecAttribNPCUnit.size(); i++ )
	{
		KAttribNpcUnitInfo * pKAttribNpcUnitNot = &kEvent.m_vecAttribNPCUnit[i];
		CreateNPC( (CX2UnitManager::NPC_UNIT_ID)pKAttribNpcUnitNot->m_sNPCUnitReq.m_NPCID,
			pKAttribNpcUnitNot->m_sNPCUnitReq.m_Level,
			pKAttribNpcUnitNot->m_sNPCUnitReq.m_bActive,
			pKAttribNpcUnitNot->m_sNPCUnitReq.m_UID,
			D3DXVECTOR3( pKAttribNpcUnitNot->m_sNPCUnitReq.m_vPos.x, pKAttribNpcUnitNot->m_sNPCUnitReq.m_vPos.y, pKAttribNpcUnitNot->m_sNPCUnitReq.m_vPos.z ),
			pKAttribNpcUnitNot->m_sNPCUnitReq.m_bIsRight,
			pKAttribNpcUnitNot->m_sNPCUnitReq.m_fDelayTime,
			(int) pKAttribNpcUnitNot->m_sNPCUnitReq.m_KeyCode,
			(CX2Room::TEAM_NUM) pKAttribNpcUnitNot->m_sNPCUnitReq.m_cTeamNum,
			(CX2NPCAI::NPC_AI_TYPE) pKAttribNpcUnitNot->m_sNPCUnitReq.m_cAIType,
			pKAttribNpcUnitNot->m_sNPCUnitReq.m_iAllyUID, &pKAttribNpcUnitNot->m_sAttribEnchantNpcInfo );

	}

	//{{ kimhc // 2010.4.23 // 비밀던전 작업(Extra 모드)
	// 던전에서 몬스터를 임의로 생성해서 성공했을 때
	// 해당 몬스터가 substage clear 조건에 있는 경우
	// 클리어 조건의 해당 몬스터 original count를 update 시켜줌
	if ( GetGameType() == GT_DUNGEON )
	{
		CX2Dungeon* pDungeon = static_cast< CX2DungeonGame* >( this )->GetDungeon();

		if ( pDungeon != NULL &&
			pDungeon->GetNowStage() != NULL &&
			pDungeon->GetNowStage()->GetNowSubStage() != NULL )
		{
			pDungeon->GetNowStage()->GetNowSubStage()->SetMonsterCountForSubStageClearCheck();
		}
	}
	//}} kimhc // 2010.4.23 // 비밀던전 작업(Extra 모드)
	return true;
}



bool CX2Game::Handler_EGS_NPC_UNIT_DIE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KPacketOK kEvent;
	DeSerialize( pBuff, &kEvent );
	CX2PacketLog::PrintLog( &kEvent );

ELSWORD_VIRTUALIZER_START
	//if( g_pMain->DeleteServerPacket( EGS_NPC_UNIT_DIE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			return true;
		}
	}

	--m_iNpcDiePacket;

ELSWORD_VIRTUALIZER_END

	return false;
}

/*virtual*/ bool CX2Game::Handler_EGS_NPC_UNIT_DIE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_NPC_UNIT_DIE_NOT kEvent;
	DeSerialize( pBuff, &kEvent );
	CX2PacketLog::PrintLog( &kEvent );

	CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( kEvent.m_nDieNPCUID );
	if( pCX2GUNPC != NULL )
	{
        pCX2GUNPC->MarkNPCUnitDieNotReceived();

		map<int, int>::iterator it;
		it = m_mapKilledNPCKeyCode.find( pCX2GUNPC->GetKeyCode() );
		if( it != m_mapKilledNPCKeyCode.end() )
		{
			m_mapKilledNPCKeyCode[ pCX2GUNPC->GetKeyCode() ] += 1;
		}
		else
		{
			m_mapKilledNPCKeyCode[ pCX2GUNPC->GetKeyCode() ] = 1;
		}

		if( GetMyUnit() != NULL && pCX2GUNPC->IsPvpBot() == false )
		{
			ProcessExpListByNpcUnitDie( kEvent );

		}

		ProcessIKillThisNpc( kEvent );

		if( pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_EVOKE_TRAP_LIRE
			|| pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_EVOKE_TRAP_LIRE_MEMO
			)
		{
			CX2GUUser* pUser = static_cast<CX2GUUser*>( pCX2GUNPC->GetOwnerGameUnit() );
			if( pUser != NULL && pUser->GetUnit()->GetType() == CX2Unit::UT_LIRE )
			{
				CX2GULire_ElvenRanger* pLire = static_cast<CX2GULire_ElvenRanger*>( pUser );
				pLire->DeleteEvokeNPCUID( (UidType) pCX2GUNPC->GetUID() );
			}
		}

		if( pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_RENA_ANGER_OF_ELF_DELAY
			||  pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_RENA_ANGER_OF_ELF )
		{
			CX2GUUser* pUser = static_cast<CX2GUUser*>( pCX2GUNPC->GetOwnerGameUnit() );
			if( pUser != NULL && pUser->GetUnit()->GetType() == CX2Unit::UT_LIRE )
			{
				CX2GULire_ElvenRanger* pLire = static_cast<CX2GULire_ElvenRanger*>( pUser );
				if( NULL != pLire )
					pLire->DeleteAngerOfElfTrapUID( static_cast<UidType>( pCX2GUNPC->GetUID() ) );
			}
		}

		if( pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_REMOTE_MORTAR )
		{
			CX2GUUser* pUser = static_cast<CX2GUUser*>( pCX2GUNPC->GetOwnerGameUnit() );
			if( pUser != NULL && pUser->GetUnit()->GetType() == CX2Unit::UT_CHUNG )
			{
				CX2GUChung* pChung = static_cast<CX2GUChung*>(pUser);

				if( NULL != pChung )
				{
					if( pChung->GetRemoteMortarNPCUID() == pCX2GUNPC->GetUnitUID() )	/// 패킷 전송 전에 아이디 등록되었다면, 아이디 덮어씌우는 예외 수정
					{
						pChung->DeleteRemoteMortarNPCUID();
					}
				}
			}
		}

	//	ProcessSummonNPCDieEvent( kEvent );

	}

	return true;
}


bool CX2Game::CheckEnableKeyProcess()
{
	if( GetOpenChatBox() == true )
		return false;

	// edit box에 포커스가 있는지 검사한다.
	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetFocusInEditBox() == true )
		return false;

	//if ( g_pData->GetMessenger() != NULL && g_pData->GetMessenger()->GetOpen() == true )
	//	return false;

	if( g_pData->GetMessenger()->GetOpenChatBox() == true)
	{
		CKTDGUIControl* pControl = CKTDGUIDialog::GetControlFocus();
		if ( pControl != NULL )
		{
			if ( pControl->GetType() == CKTDGUIControl::UCT_IME_EDITBOX ||
				pControl->GetType() == CKTDGUIControl::UCT_EDITBOX )
			{
				return false;
			}
		}
	}

	return true;
}


bool CX2Game::NPCUnitDieReq( const KEGS_NPC_UNIT_DIE_REQ& kPacket_ )
{
	// checked
	ELSWORD_VIRTUALIZER_START

	if( IsHost() == false )
		return false;

	THEMIDA_ENCODE_START

	g_pData->GetServerProtocol()->SendPacket( EGS_NPC_UNIT_DIE_REQ, kPacket_ );

	THEMIDA_ENCODE_END

	ELSWORD_VIRTUALIZER_END
}

bool CX2Game::NPCUnitDieReq( char cAttUnitType, int iNPCUID, UidType uiAttUnit, UidType uiMDAttUnit, D3DXVECTOR3 diePos, bool bIsAllyNPC, bool bBounsDrop /*= false*/ )
{

	// checked
	ELSWORD_VIRTUALIZER_START


	if( bIsAllyNPC == true )
	{
		return NPCUnitDieReq( cAttUnitType, iNPCUID, uiAttUnit, uiMDAttUnit, diePos, KEGS_NPC_UNIT_DIE_REQ::NDS_ALLY );
	}
	else if ( bBounsDrop == true )
	{
		return NPCUnitDieReq( cAttUnitType, iNPCUID, uiAttUnit, uiMDAttUnit, diePos, KEGS_NPC_UNIT_DIE_REQ::NDS_BONUS_DROP );
	}
	else
	{
		return NPCUnitDieReq( cAttUnitType, iNPCUID, uiAttUnit, uiMDAttUnit, diePos, KEGS_NPC_UNIT_DIE_REQ::NDS_KILL_BY_USER );
	}

	++m_iNpcDiePacket;

	ELSWORD_VIRTUALIZER_END
}

bool CX2Game::NPCUnitDieReq( char cAttUnitType, int iNPCUID, UidType uiAttUnit, UidType uiMDAttUnit, D3DXVECTOR3 diePos, KEGS_NPC_UNIT_DIE_REQ::NPC_DIE_STATE eDieState )
{

	// checked
	ELSWORD_VIRTUALIZER_START



	if( IsHost() == false )
		return false;

	THEMIDA_ENCODE_START

	KEGS_NPC_UNIT_DIE_REQ kPacket;
	kPacket.m_cAttUnitType	= cAttUnitType;
	kPacket.m_nDieNPCUID	= iNPCUID;
	kPacket.m_uiAttUnit		= uiAttUnit;
	kPacket.m_uiMDAttUnit	= uiMDAttUnit;
	kPacket.m_DiePos.x		= diePos.x;
	kPacket.m_DiePos.y		= diePos.y;
	kPacket.m_DiePos.z		= diePos.z;
	kPacket.m_cNpcDieState	= eDieState;

	g_pData->GetServerProtocol()->SendPacket( EGS_NPC_UNIT_DIE_REQ, kPacket );
	//g_pMain->AddServerPacket( EGS_NPC_UNIT_DIE_ACK, 60.f );

	++m_iNpcDiePacket;

	THEMIDA_ENCODE_END

	ELSWORD_VIRTUALIZER_END


	return true;

}


/*virtual*/ void CX2Game::UserUnitDieNot( KEGS_USER_UNIT_DIE_NOT& kEGS_USER_UNIT_DIE_NOT )
{
	return;
}

bool CX2Game::Handler_EGS_USER_UNIT_DIE_COMPLETE_NOT( KEGS_USER_UNIT_DIE_COMPLETE_NOT& kEGS_USER_UNIT_DIE_COMPLETE_NOT )
{
	CX2GUUser* pCX2GUUser = GetUserUnitByUID( kEGS_USER_UNIT_DIE_COMPLETE_NOT.m_UnitUID );
	if( pCX2GUUser != NULL )
	{
		//{{ robobeg : 2008-10-21
		//pCX2GUUser->SetShow( false );
		/// kimhc // 2012-11-27 // 유저가 부활한 이후에 이 패킷을 받는 경우가 있을 것으로 보여 아래와 같이 처리함
		if ( pCX2GUUser->GetNowStateID() == CX2GameUnit::GUSI_DIE && !(pCX2GUUser->GetNowHp() > 0.0f) )
			pCX2GUUser->SetShowObject( false );
 		//}} robobeg : 2008-10-21

	}
	return true;
}

bool CX2Game::Handler_EGS_UPDATE_UNIT_INFO_NOT( KEGS_UPDATE_UNIT_INFO_NOT& kEGS_UPDATE_UNIT_INFO_NOT  )
{
	if ( g_pX2Room != NULL )
	{
		for ( int i = 0; i < g_pX2Room->GetSlotNum(); i++ )
		{
			CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotData( i );
			if ( pSlotData == NULL || pSlotData->m_pUnit == NULL )
				continue;

			if ( pSlotData->m_pUnit->GetUID() == kEGS_UPDATE_UNIT_INFO_NOT.m_kUnitInfo.m_nUnitUID )
			{
				pSlotData->m_pUnit->Reset( kEGS_UPDATE_UNIT_INFO_NOT.m_kUnitInfo );
				pSlotData->m_pUnit->AccessInventory().UpdateInventorySlotList( kEGS_UPDATE_UNIT_INFO_NOT.m_vecKInventorySlotInfo );


				// 던전게임 끝나면서 경험치 업데이트되었기 때문에 게임중 실시간 경험치 임시로 저장하던 데이터를 초기화한다.
				m_GetEXPInGame = 0;


				return true;
			}
		}
	}

	return false;
}

void CX2Game::ProcessExpListByNpcUnitDie( const KEGS_NPC_UNIT_DIE_NOT& kEvent )
{
	if ( NULL != GetMyUnit() )
	{
		std::map<UidType,KEXPData>::const_iterator iter = kEvent.m_EXPList.find( GetMyUnit()->GetUnitUID() );
		if( iter != kEvent.m_EXPList.end() )
		{
			KEXPData kEXPData = iter->second;

			if( 0 == kEXPData.m_iEXP )
			{
				g_pChatBox->AddChatLogObtain(GET_STRING(STR_ID_24594), CX2ChatBox::COT_EXP);
				return;
			}
			//{{ kimhc // 실시간 엘소드 중 실시간 경험치 획득
			CX2Unit::UnitData* pMyUnitData = NULL;
			wstringstream strstm;

			int iMaxEXP = g_pData->GetEXPTable()->GetEXPData( _CONST_X2GAME_::g_iMaxLevel ).m_nTotalExp;
			if( g_pInstanceData != NULL )
				iMaxEXP = g_pData->GetEXPTable()->GetEXPData( g_pInstanceData->GetMaxLevel() ).m_nTotalExp;
			pMyUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();

			if ( pMyUnitData->m_EXP >= iMaxEXP )
			{
				strstm << L" " << GET_STRING( STR_ID_2688 );	// fix! STR_ID 새로 만들어야 함
			}
			else
			{
				int iExp			= 0;
				int iPartyExp		= 0;
				int iSocketOptExp	= 0;

				iExp			= kEXPData.m_iEXP;
				iPartyExp		= kEXPData.m_iPartyEXP;
				iSocketOptExp	= kEXPData.m_iSocketOptEXP + kEXPData.m_iEventBonusEXP;		/// 소켓 경험치 + 이벤트 보너스 경험치

				pMyUnitData->m_EXP += iExp;

				if ( pMyUnitData->m_EXP >= iMaxEXP )
				{
					iPartyExp			= 0;
					iSocketOptExp		= 0;
					iExp				= iExp - ( pMyUnitData->m_EXP - iMaxEXP );
					pMyUnitData->m_EXP	= iMaxEXP;
				}
				else
				{
					pMyUnitData->m_EXP += iPartyExp;

					if ( pMyUnitData->m_EXP >= iMaxEXP )			/// 일반 경험치 + 파티 경험치
					{
						iSocketOptExp		= 0;
						iPartyExp			= iPartyExp - ( pMyUnitData->m_EXP - iMaxEXP );
						pMyUnitData->m_EXP	= iMaxEXP;
					}
					else
					{
						pMyUnitData->m_EXP += iSocketOptExp;

						if ( pMyUnitData->m_EXP >= iMaxEXP )		/// 일반 경험치 + 파티 경험치 + 보너스 경험치
						{
							iSocketOptExp		= iSocketOptExp - ( pMyUnitData->m_EXP - iMaxEXP );
							pMyUnitData->m_EXP	= iMaxEXP;
						}
					}
				}

				const CX2UnitManager::NPCUnitTemplet* pNpcTemplet
					= g_pData->GetUnitManager()->GetNPCUnitTemplet( static_cast<CX2UnitManager::NPC_UNIT_ID>( kEvent.m_iNPCID ) );

				if ( NULL != pNpcTemplet )
					strstm << pNpcTemplet->m_Name.c_str();

				if( kEXPData.m_iEXP >= 0 )
				{
					strstm << L" " << GET_STRING( STR_ID_189 ) << L" " << iExp + iPartyExp + iSocketOptExp;
				}

				if( kEXPData.m_iPartyEXP > 0 )
				{
					strstm << L" " << GET_REPLACED_STRING( ( STR_ID_190, "i", iPartyExp ) );
				}

				if ( ( kEXPData.m_iSocketOptEXP + kEXPData.m_iEventBonusEXP ) > 0)
				{
					strstm << L" " << GET_REPLACED_STRING( ( STR_ID_2625, "i", iSocketOptExp ) );
				}
			}

			//}} kimhc // 실시간 엘소드 중 실시간 경험치 획득

			if( NULL != g_pChatBox )
			{
				if( false == strstm.str().empty() )
				{
					g_pChatBox->AddChatLogObtain(strstm.str().c_str(), CX2ChatBox::COT_EXP);
				}
			}
		}

	}

}

//void CX2Game::ProcessSummonNPCDieEvent( const KEGS_NPC_UNIT_DIE_NOT& kEvent )
//{
//	CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( kEvent.m_nDieNPCUID );
//	if( pCX2GUNPC != NULL )
//	{
//		if( pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_SI_HA_FERDINAND )
//		{
//			CX2GUUser* pUser = static_cast<CX2GUUser*>( pCX2GUNPC->GetOwnerGameUnit() );
//			if( pUser != NULL && pUser->GetUnit()->GetType() == CX2Unit::UT_EVE )
//			{
//				CX2GUEve* pEve = static_cast<CX2GUEve*>( pUser );
//				if( NULL != pEve )
//				{
//					pEve->ResetLinkOverChargeIllusion();
//				}
//			}
//		}
//	}
//}

//bool CX2Game::Handler_EGS_NOTIFY_MSG_NOT( KEGS_NOTIFY_MSG_NOT& kEGS_NOTIFY_MSG_NOT )
//{
//	g_pMain->GetNoticeManager()->AddNotice( kEGS_NOTIFY_MSG_NOT.m_wstrMSG.c_str(), (int)kEGS_NOTIFY_MSG_NOT.m_Count );
//
//	return true;
//}

bool CX2Game::CheckNewUser( UidType unitUID )
{
	for ( int i = 0; i < (int)m_NewGameUserList.size(); i++ )
	{
		NewGameUser* pNewGameUser = &m_NewGameUserList[i];
		if( pNewGameUser->m_UnitUID == unitUID && pNewGameUser->m_bStart == true )
		{
			return true;
		}
	}

	return false;
}

bool CX2Game::Handler_EGS_JOIN_ROOM_NOT( KEGS_JOIN_ROOM_NOT& kEGS_JOIN_ROOM_NOT )
{
	if( kEGS_JOIN_ROOM_NOT.m_JoinSlot.m_kRoomUserInfo.m_nUnitUID == g_pX2Room->GetMySlot()->m_UnitUID )
		return true;

	wstringstream wstrstream;
	wstrstream << L"Handler_EGS_JOIN_ROOM_NOT-";
	StateLog( wstrstream.str().c_str() );

	g_pX2Room->Set_KRoomSlotInfo( kEGS_JOIN_ROOM_NOT.m_JoinSlot );

	if ( GetGameType() == GT_BATTLE_FIELD )
	{
		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( kEGS_JOIN_ROOM_NOT.m_JoinSlot.m_kRoomUserInfo.m_nUnitUID );
		if( pSlotData != NULL )
		{

			CX2Unit* pCX2Unit = pSlotData->m_pUnit;
			if( pCX2Unit != NULL )
			{
				//P2PSetPeer( pCX2Unit );
				//P2PConnectTest();

				//백그라운드 로딩과 즉시로딩을 모두 지원하자 일단!
				//AddUserUnitIntruder( pSlotData, pSlotData->m_Index );
				AddUserUnitIntruderByLoader( pSlotData->m_pUnit );

				if( g_pX2Room->GetMySlot()->m_UnitUID != pSlotData->m_UnitUID )
				{
					//난입자가 내가 아니면
					NewGameUser pvpGameNewUser;
					pvpGameNewUser.m_UnitUID			= pSlotData->m_UnitUID;
					pvpGameNewUser.m_StartPosIndex		= kEGS_JOIN_ROOM_NOT.m_StartPosIndex;
					m_NewGameUserList.push_back( pvpGameNewUser );
				}
				else
				{
					g_pX2Room->SetIntrudeGame( false );
					if( GetMyUnit() != NULL )
					{
						GetMyUnit()->SetForceInvincible( 5.0f );
						GetMyUnit()->SetShowInvincible( 5.0f );
						GetMyUnit()->StateChange( GetMyUnit()->GetStartIntrudeStateID(), true );
					}
					GameStart();
					g_pKTDXApp->SkipFrame();

					// 난입시 esc 안먹히는 현상 수정
					SetCanUseEscFlag( true );
				}

				InsertGUUserToCheckUDPTimer( pSlotData->m_UnitUID );
			}
		}
	}



	return true;
}

bool CX2Game::Handler_EGS_LEAVE_ROOM_REQ( int leaveRoomReason /* = NetError::NOT_LEAVE_ROOM_REASON_00 */ )
{
	if( true == g_pMain->IsWaitingServerPacket( EGS_LEAVE_ROOM_ACK ) )
	{
		return false;
	}

	if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME )
	{
		//통계를 위해서 음허허헝허어허헣어허어헝
		Handler_EGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT();
	}

	KEGS_LEAVE_ROOM_REQ kPacket;
	kPacket.m_iReason = leaveRoomReason;

	CX2GUUser* pCX2GUUser = GetMyUnit();
	if( NULL != pCX2GUUser )
	{
		kPacket.m_kResultInfo.m_fHP				= pCX2GUUser->GetNowHp();
		kPacket.m_kResultInfo.m_nDamageCount	= pCX2GUUser->GetDamageCount();
		kPacket.m_kResultInfo.m_TotalScore		= m_GameScore;
		//kPacket.m_ComboScore	= pCX2GUUser->GetComboManager()->GetComboScoreFull();

		if( NULL != pCX2GUUser->GetComboManager() )
		{
			kPacket.m_kResultInfo.m_ComboScore	= pCX2GUUser->GetComboManager()->GetComboScoreFull();
		}
		else
		{
			kPacket.m_kResultInfo.m_ComboScore	= 0;
		}
		kPacket.m_kResultInfo.m_TechScore		= m_TechPointFull;

		kPacket.m_kResultInfo.m_iTotalAttackedDamage	= pCX2GUUser->GetTotalAttackedDamage();
		kPacket.m_kResultInfo.m_iTotalRecoveryHP		= pCX2GUUser->GetTotalRecoveryHP();
		kPacket.m_kResultInfo.m_iUsingKindOfSkill		= pCX2GUUser->GetUsingSkillCount();
		kPacket.m_kResultInfo.m_iTotalGivenDamamge		= pCX2GUUser->GetTotalGivenDamamge();
		kPacket.m_kResultInfo.m_iDashCount				= pCX2GUUser->GetUsingDashCount();
			kPacket.m_kResultInfo.m_bSkillSlotType = g_pMain->GetGameOption().GetIsSkillUITypeA();

	}
	kPacket.m_kResultInfo.m_iFrame = static_cast<int>(m_kGameStatistics.m_kAverageFps.m_fAverageFps);

	// 맵에 가지고 있는 GamePlayNetworkInfo 를 vector에 담는다.
	if ( NULL != g_pX2Room )
		g_pX2Room->MoveGamePlayNetworkInfoFromMapToVector( kPacket.m_kGamePlayNetworkInfo.m_vecGamePlayNetworkInfo );

// #ifdef ADD_CHKOPT_ABNORMAL_FUNCTION_CALL_V2
// 	AHNHS_PROTECT_FUNCTION
// #endif

	g_pData->GetServerProtocol()->SendPacket( EGS_LEAVE_ROOM_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_LEAVE_ROOM_ACK );
	return true;
}

bool CX2Game::Handler_EGS_LEAVE_ROOM_ACK( KEGS_LEAVE_ROOM_ACK& kEGS_LEAVE_ROOM_ACK )
{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
	//게임중에 강제로 나갔을때...
	KOGGamePerformanceCheck::GetInstance()->End();
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

	g_pMain->DeleteServerPacket( EGS_LEAVE_ROOM_ACK );

	// 방 이탈이 성공했을 경우
	const bool bRet = g_pMain->IsValidPacket( kEGS_LEAVE_ROOM_ACK.m_iOK );
	if( bRet == true )
	{
		if ( kEGS_LEAVE_ROOM_ACK.m_iReason == NetError::NOT_LEAVE_ROOM_REASON_22 )
		{
			std::wstringstream wstrStream;
            wstrStream << L"'" << g_pData->GetItemManager()->GetItemTemplet(kEGS_LEAVE_ROOM_ACK.m_iItemID)->GetFullName_()
                << NetError::GetErrStrF(NetError::NOT_LEAVE_ROOM_REASON_22);
			g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), wstrStream.str().c_str(), g_pMain->GetNowState() );
		}

		if ( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME &&
			NULL != g_pData && NULL != g_pData->GetPremiumBuffManager() &&
			false == g_pData->GetPremiumBuffManager()->IsComeBackUser() )
		{
			g_pData->GetPremiumBuffManager()->SetComeBackUserInParty( false );
		}

	}
	return bRet;
}

bool CX2Game::Handler_EGS_LEAVE_ROOM_NOT( KEGS_LEAVE_ROOM_NOT& kEGS_LEAVE_ROOM_NOT )
{
	DialogLog( "Handler_EGS_LEAVE_ROOM_NOT_1" );
	//P2PDeletePeer( kEGS_LEAVE_ROOM_NOT.m_iUnitUID );

	DialogLog( "Handler_EGS_LEAVE_ROOM_NOT_2" );
	//{{ 2012. 10. 31	박세훈	Merge 랜선랙 방지 작업-릴레이를 기본적으로 사용한다.
	m_mapCheckUDPTimer.erase( kEGS_LEAVE_ROOM_NOT.m_iUnitUID );
	//}}
	DeleteUserUnitByUID( kEGS_LEAVE_ROOM_NOT.m_iUnitUID );


	DialogLog( "Handler_EGS_LEAVE_ROOM_NOT_4" );
	g_pX2Room->SetRoomState( (CX2Room::ROOM_STATE)kEGS_LEAVE_ROOM_NOT.m_cRoomState );

	DialogLog( "Handler_EGS_LEAVE_ROOM_NOT_5" );
	g_pX2Room->Set_KRoomSlotInfoList( kEGS_LEAVE_ROOM_NOT.m_vecSlot );
	DialogLog( "Handler_EGS_LEAVE_ROOM_NOT_6" );
	UpdateUnitPointer();

	DialogLog( "Handler_EGS_LEAVE_ROOM_NOT_7" );
	return true;
}

bool CX2Game::Handler_EGS_LEAVE_GAME_REQ()
{
	g_pData->GetServerProtocol()->SendID( EGS_LEAVE_GAME_REQ );
	g_pMain->AddServerPacket( EGS_LEAVE_GAME_ACK );
	return true;
}

bool CX2Game::Handler_EGS_LEAVE_GAME_ACK( KEGS_LEAVE_GAME_ACK& kEGS_LEAVE_GAME_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_LEAVE_GAME_ACK ) == true )
	{
		return g_pMain->IsValidPacket( kEGS_LEAVE_GAME_ACK.m_iOK );
	}
	return false;
}

bool CX2Game::Handler_EGS_LEAVE_GAME_NOT( KEGS_LEAVE_GAME_NOT& kEGS_LEAVE_GAME_NOT )
{
	//P2PDeletePeer( kEGS_LEAVE_GAME_NOT.m_iUnitUID );

	//{{ dmlee 2010/01/07 게임중에 ESC 눌러서 나갈 때 이 패킷 받고나서 EGS_NPC_UNIT_DIE_NOT 받으면 뻗는 문제 때문에 수정
	if( NULL != GetMyUnit() )
	{
		if( GetMyUnit()->GetUnitUID() == kEGS_LEAVE_GAME_NOT.m_iUnitUID )
		{
			m_optrMyUnit.Reset();
		}
	}
	//}} dmlee 2010/01/07 게임중에 ESC 눌러서 나갈 때 이 패킷 받고나서 EGS_NPC_UNIT_DIE_NOT 받으면 뻗는 문제 때문에 수정

	//{{ 2012. 10. 31	박세훈	Merge 랜선랙 방지 작업-릴레이를 기본적으로 사용한다.
	m_mapCheckUDPTimer.erase( kEGS_LEAVE_GAME_NOT.m_iUnitUID );
	//}}

	DeleteUserUnitByUID( kEGS_LEAVE_GAME_NOT.m_iUnitUID );


	g_pX2Room->SetRoomState( (CX2Room::ROOM_STATE)kEGS_LEAVE_GAME_NOT.m_cRoomState );
	g_pX2Room->Set_KRoomSlotInfoList( kEGS_LEAVE_GAME_NOT.m_vecSlot );
	g_pX2Room->Set_KRoomObserverSlotInfoList( kEGS_LEAVE_GAME_NOT.m_vecObserverSlot );



	UpdateUnitPointer();
//#ifndef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
////#ifdef	BATTLE_FIELD_TEST
//	//FieldTest
//	BOOST_FOREACH( const KRoomSlotInfo& kRoomSlotInfo, kEGS_LEAVE_GAME_NOT.m_vecSlot )
//	{
//		if ( true == kRoomSlotInfo.m_bHost )
//		{
//			CX2GUUser* pHostGUUser = GetUserUnitByUID( kRoomSlotInfo.m_kRoomUserInfo.m_nUnitUID );
//			SetHostGameUnit( pHostGUUser );
//		}
//	}
////#endif	BATTLE_FIELD_TEST
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	return true;
}

bool CX2Game::Handler_EGS_STATE_CHANGE_RESULT_REQ()
{
	KEGS_STATE_CHANGE_RESULT_REQ kEGS_STATE_CHANGE_RESULT_REQ;

	g_pData->GetServerProtocol()->SendPacket( EGS_STATE_CHANGE_RESULT_REQ, kEGS_STATE_CHANGE_RESULT_REQ );
	g_pMain->AddServerPacket( EGS_STATE_CHANGE_RESULT_ACK );

	return true;
}

bool CX2Game::Handler_EGS_STATE_CHANGE_RESULT_ACK( KEGS_STATE_CHANGE_RESULT_ACK& kEGS_STATE_CHANGE_RESULT_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_STATE_CHANGE_RESULT_ACK ) == true )
	{
		return g_pMain->IsValidPacket( kEGS_STATE_CHANGE_RESULT_ACK.m_iOK );
	}
	return false;
}

bool CX2Game::Handler_EGS_STATE_CHANGE_RESULT_NOT( KEGS_STATE_CHANGE_RESULT_NOT& kEGS_STATE_CHANGE_RESULT_NOT )
{
	g_pX2Room->SetRoomState( (CX2Room::ROOM_STATE)kEGS_STATE_CHANGE_RESULT_NOT.m_cRoomState );
	g_pX2Room->Set_KRoomSlotInfoList( kEGS_STATE_CHANGE_RESULT_NOT.m_vecSlot );

	return true;
}



bool CX2Game::Handler_EGS_PLAY_START_NOT( KEGS_PLAY_START_NOT& kEGS_PLAY_START_NOT )
{
	g_pX2Room->SetRoomState( (CX2Room::ROOM_STATE)kEGS_PLAY_START_NOT.m_RoomState );
	g_pX2Room->Set_KRoomSlotInfoList( kEGS_PLAY_START_NOT.m_vecSlot );

	if( g_pMain->GetConnectedChannelID() == KPVPChannelInfo::PCC_OFFICIAL )
	{
		g_pX2Room->DeleteNpcSlot();
	}
	else	// 공식대전이 아닌 경우에만
	{
		//{{ kimhc // 게임 로딩이 끝날때 까지 ESC 처리 안되게 하는 작업 // 2009-06-08
		SetCanUseEscFlag( true );
		//}} kimhc // 게임 로딩이 끝날때 까지 ESC 처리 안되게 하는 작업 // 2009-06-08
	}

	GameStart();






	CKTDGLineMap* pLineMap = NULL;
	if( m_pWorld != NULL )
		pLineMap = m_pWorld->GetLineMap();

	if( pLineMap != NULL )
	{
		int numNpc = 0;
		std::vector< CX2Room::RoomNpcSlot > &vecNpcSlot = g_pX2Room->GetNpcSlot();
		for(int i=0; i<(int)vecNpcSlot.size(); ++i)
		{
			CX2Room::RoomNpcSlot &npcSlot = vecNpcSlot[i];

			int index = i - 4;
			if( index < 0 )
				index = 0;

			D3DXVECTOR3	startPos	= pLineMap->GetBlueTeamStartPosition( index );
			bool bRight				= pLineMap->GetBlueTeamStartRight( index );
			int lineIndex			= pLineMap->GetBlueTeamStartLineIndex( index );

			PushCreateNPCReq( (CX2UnitManager::NPC_UNIT_ID)npcSlot.m_iNpcId, npcSlot.m_iLevel, true,
				startPos, bRight, 0.f, true, PVP_NPC_KEY_CODE, (CX2Room::TEAM_NUM)CX2Room::TN_BLUE, CX2NPCAI::NAT_NORMAL, -1 );
			++numNpc;

		}

		if( numNpc > 0 )
			FlushCreateNPCReq();
	}





	g_pKTDXApp->SkipFrame();

	return true;
}

bool CX2Game::Handler_EGS_STATE_CHANGE_GAME_INTRUDE_NOT( KEGS_STATE_CHANGE_GAME_INTRUDE_NOT& kEvent )
{
	g_pX2Room->SetRoomState( (CX2Room::ROOM_STATE)kEvent.m_RoomState );
	g_pX2Room->Set_KRoomSlotInfo( kEvent.m_IntruderSlot );


	if( g_pX2Room->GetMySlot()->m_UnitUID != kEvent.m_IntruderSlot.m_kRoomUserInfo.m_nUnitUID )
	{
		//난입자가 내가 아니면
		CX2Room::SlotData* pSlotData = g_pX2Room->GetSlotDataByUnitUID( kEvent.m_IntruderSlot.m_kRoomUserInfo.m_nUnitUID );
		if( pSlotData != NULL )
		{
			CX2Unit* pCX2Unit = pSlotData->m_pUnit;
			if( pCX2Unit != NULL )
			{
				//P2PSetPeer( pCX2Unit );
				//P2PConnectTest();

				//백그라운드 로딩과 즉시로딩을 모두 지원하자 일단!
				//AddUserUnitIntruder( pSlotData, pSlotData->m_Index );
				AddUserUnitIntruderByLoader( pSlotData->m_pUnit );
			}
		}
	}
	return true;
}

bool CX2Game::Handler_EGS_INTRUDE_START_REQ()
{
	KEGS_INTRUDE_START_REQ kEGS_INTRUDE_START_REQ;
	if( NULL != GetWorld() &&
		NULL != GetWorld()->GetLineMap() )
	{
		kEGS_INTRUDE_START_REQ.m_StartPosIndex = GetWorld()->GetLineMap()->GetRandomStartPosIndex();
	}
	else
	{
		kEGS_INTRUDE_START_REQ.m_StartPosIndex = 0;
	}

	if( kEGS_INTRUDE_START_REQ.m_StartPosIndex < 0 )
		kEGS_INTRUDE_START_REQ.m_StartPosIndex = 0;

	if( GetMyUnit() != NULL )
		GetMyUnit()->InitPosition( false, kEGS_INTRUDE_START_REQ.m_StartPosIndex );

	g_pData->GetServerProtocol()->SendPacket( EGS_INTRUDE_START_REQ, kEGS_INTRUDE_START_REQ );
	g_pMain->AddServerPacket( EGS_INTRUDE_START_ACK );

	return true;
}

bool CX2Game::Handler_EGS_INTRUDE_START_ACK( KEGS_INTRUDE_START_ACK& kEGS_INTRUDE_START_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_INTRUDE_START_ACK ) == true )
	{
		bool bResult = g_pMain->IsValidPacket( kEGS_INTRUDE_START_ACK.m_iOK );
		if( false == bResult )
		{
			StateLog( L"Can not Intrude ack" );
		}
		return bResult;
	}
	return false;
}

bool CX2Game::Handler_EGS_INTRUDE_START_NOT( KEGS_INTRUDE_START_NOT& kEGS_INTRUDE_START_NOT )
{

	ASSERT( NULL != g_pX2Room );
	if( NULL == g_pX2Room )
	{
		StateLog( L"intrude room null" );
		return false;
	}


	g_pX2Room->SetRoomState( (CX2Room::ROOM_STATE)kEGS_INTRUDE_START_NOT.m_RoomState );
	g_pX2Room->Set_KRoomSlotInfo( kEGS_INTRUDE_START_NOT.m_IntruderSlot );

	ASSERT( NULL != g_pX2Room->GetMySlot() );
	if( NULL == g_pX2Room->GetMySlot() )
	{
		StateLog( L"intrude my slot null" );
		return false;
	}

	if( g_pX2Room->GetMySlot()->m_UnitUID != kEGS_INTRUDE_START_NOT.m_IntruderSlot.m_kRoomUserInfo.m_nUnitUID )
	{
		//난입자가 내가 아니면
		NewGameUser pvpGameNewUser;
		pvpGameNewUser.m_UnitUID			= kEGS_INTRUDE_START_NOT.m_IntruderSlot.m_kRoomUserInfo.m_nUnitUID;
		pvpGameNewUser.m_StartPosIndex		= kEGS_INTRUDE_START_NOT.m_StartPosIndex;
		m_NewGameUserList.push_back( pvpGameNewUser );

		//{{ 2012. 12. 5	최육사		옵저버 대전 난입 모드
		if( NULL != g_pChatBox  &&  kEGS_INTRUDE_START_NOT.m_bObserver == false )
		//}}
		{
			wstring wstrColor = L"#CFF3F4D";								// 255, 63, 77
			D3DXCOLOR coTextColor(1.f, 0.24705f, 0.30196f, 1.f);		// 255, 63, 77

			g_pChatBox->AddChatLog( GET_REPLACED_STRING( ( STR_ID_196, "L", kEGS_INTRUDE_START_NOT.m_IntruderSlot.m_kRoomUserInfo.m_wstrNickName ) ), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );

		}

		//백그라운드 로딩과 즉시로딩을 모두 지원하자 일단!
		//IntruderStart( kEGS_INTRUDE_START_NOT.m_IntruderSlot.m_kRoomUserInfo.m_kUnitInfo.m_nUnitUID, kEGS_INTRUDE_START_NOT.m_StartPosIndex );
	}
	else
	{
		g_pX2Room->SetIntrudeGame( false );
		if( GetMyUnit() != NULL )
		{
			GetMyUnit()->SetForceInvincible( 5.0f );
			GetMyUnit()->SetShowInvincible( 5.0f );
			GetMyUnit()->StateChange( GetMyUnit()->GetStartIntrudeStateID(), true );
		}
		GameStart();
		g_pKTDXApp->SkipFrame();

		// 난입시 esc 안먹히는 현상 수정
		SetCanUseEscFlag( true );
	}

	return true;
}


void CX2Game::Handler_EGS_CURRENT_TEAM_KILL_SCORE_NOT( KEGS_CURRENT_TEAM_KILL_SCORE_NOT& kEGS_CURRENT_TEAM_KILL_SCORE_NOT )
{
	m_RedTeamKill	= kEGS_CURRENT_TEAM_KILL_SCORE_NOT.m_RedTeamKill;
	m_BlueTeamKill	= kEGS_CURRENT_TEAM_KILL_SCORE_NOT.m_BlueTeamKill;
}


/*virtual*/ bool CX2Game::Handler_EGS_END_GAME_NOT( KEGS_END_GAME_NOT& kKEGS_END_GAME_NOT )
{
	m_bGameEnd = true;

	SAFE_DELETE_DIALOG( m_pDLGCheckResurrectByUseCash );

// 	if( g_pData != NULL &&
// 		g_pData->GetUIManager() != NULL &&
// 		g_pData->GetUIManager()->GetUIQuickSlot() != NULL )
// 	{
// 		g_pData->GetUIManager()->GetUIQuickSlot()->ResetCoolTime();
// 	}

	return true;
}


bool CX2Game::Handler_EGS_DROP_ITEM_NOT( KEGS_DROP_ITEM_NOT& kEGS_DROP_ITEM_NOT )
{
	for( int i = 0; i < (int)kEGS_DROP_ITEM_NOT.m_DropItemDataList.size(); i++ )
	{
		// themida_checked
		//THEMIDA_VM_START

		const KDropItemData& dropData = kEGS_DROP_ITEM_NOT.m_DropItemDataList[i];

		if( true == g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
		{
			m_pDropItemManager->AddDropItem( dropData.m_iItemID, dropData.m_iDropItemUID,
				D3DXVECTOR3( kEGS_DROP_ITEM_NOT.m_CreatePos.x, kEGS_DROP_ITEM_NOT.m_CreatePos.y, kEGS_DROP_ITEM_NOT.m_CreatePos.z ),
				dropData.m_bLeft, dropData.m_fSpeed, 60.f, dropData.m_cEnchantLevel );
		}
		else
		{
			m_pDropItemManager->AddDropItem( dropData.m_iItemID, dropData.m_iDropItemUID, D3DXVECTOR3( kEGS_DROP_ITEM_NOT.m_CreatePos.x, kEGS_DROP_ITEM_NOT.m_CreatePos.y, kEGS_DROP_ITEM_NOT.m_CreatePos.z ), dropData.m_bLeft, dropData.m_fSpeed, 20.f, dropData.m_cEnchantLevel );
		}

		//THEMIDA_VM_END

	}
	return true;
}





// TODO: 이 함수를 컴파일 최적화를 안되게 했고 themida 로 가상화했기 때문에 다수의 아이템을 동시에 먹었을 경우에 framerate가 떨어질 수 있다 문제가 되면 THEMIDA_ENCODE로 바꾸자
//#ifdef THEMIDA_BY_TOOL_TEAM
//#	pragma optimize("",off)
//#endif THEMIDA_BY_TOOL_TEAM
bool CX2Game::Handler_EGS_GET_ITEM_REQ( UidType itemUID )
{



	// checked
	ELSWORD_VIRTUALIZER_START



	THEMIDA_ENCODE_START

	int iItemTID = m_pDropItemManager->GetDropItemID( itemUID );

	KItemInfo kItemInfo;
	kItemInfo.m_iItemID = iItemTID;
	kItemInfo.m_iQuantity = 1;
	kItemInfo.m_cUsageType = 0;

	KEGS_GET_ITEM_REQ kKEGS_GET_ITEM_REQ;

	//{{ kimhc // 실시간 엘소드 중 실시간 아이템 획득
	kKEGS_GET_ITEM_REQ.m_iDropItemUID = static_cast<int>( itemUID );
	//}} kimhc // 실시간 엘소드 중 실시간 아이템 획득

	kKEGS_GET_ITEM_REQ.m_kItemInfo = kItemInfo;

	g_pData->GetServerProtocol()->SendPacket( EGS_GET_ITEM_REQ, kKEGS_GET_ITEM_REQ );
	//g_pMain->AddServerPacket( EGS_GET_ITEM_ACK );

	//펫 오토 루팅 기능 동작 하지 않을 때만 체크 하도록 변경
	if( false == m_pDropItemManager->GetIsSummonItemPickupPet() )
	{
#ifdef RECHECK_GETIETMREQ_OVERDIST
		D3DXVECTOR3 vItemPos( 0.f, 0.f, 0.f );
		if( IsHost() == true &&
			GetUserUnitNum() == 1 &&
			m_pDropItemManager->GetDropItemPositionByUid( (int)itemUID, vItemPos ) == true )
		{
			D3DXVECTOR3 vDist = GetMyUnit()->GetPos() - vItemPos;
			if( D3DXVec3LengthSq( &vDist ) >= 200.f * 200.f )
			{
				if( g_pData != NULL && g_pData->GetServerProtocol() != NULL &&
					g_pKTDXApp->GetFindHacking() == false )
				{
					g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );

					if( g_pData != NULL && g_pData->GetMyUser() != NULL )
						g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
				}

				if( g_pKTDXApp->GetFindHacking() == false )
					g_pMain->SendHackMail_DamageHistory( ANTI_HACK_STRING_AntiHacking_Get_Item_Req_OverDist );
				g_pKTDXApp->SetFindHacking( true );
			}
		}
#endif
	}



	THEMIDA_ENCODE_END

	ELSWORD_VIRTUALIZER_END

	return true;

}
//#ifdef THEMIDA_BY_TOOL_TEAM
//#	pragma optimize("",on)
//#endif THEMIDA_BY_TOOL_TEAM




bool CX2Game::Handler_EGS_GET_ITEM_ACK( KEGS_GET_ITEM_ACK& kEGS_GET_ITEM_ACK )
{
	return true;
}

//{{ kimhc // 실시간 엘소드 중 실시간 ED 획득, 실시간 아이템 획득

bool CX2Game::Handler_EGS_GET_ITEM_NOT( KEGS_GET_ITEM_NOT& kEGS_GET_ITEM_NOT )
{
//#ifndef NOT_USE_DICE_ROLL
//	bool bDiceRolling = false;
//#endif //NOT_USE_DICE_ROLL

	int itemID = -1;
	const CX2Item::ItemTemplet* pItemTemplet = NULL;
	if( KEGS_GET_ITEM_NOT::GIT_QUEST_ITEM == kEGS_GET_ITEM_NOT.m_cGetItemType )
	{
		itemID = kEGS_GET_ITEM_NOT.m_iItemID;
		pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	}
	else
	{
		m_pDropItemManager->GetDropItem( kEGS_GET_ITEM_NOT.m_iDropItemUID, kEGS_GET_ITEM_NOT.m_GetUnitUID,
										itemID, pItemTemplet);// OUT인자로 ItemID와 pItemTemplet얻기

//#ifndef NOT_USE_DICE_ROLL
//
//		const CX2Item::ItemTemplet* pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
//		if( NULL == pItemTemplet )
//			return false;
//
//		if( NULL != g_pX2Room &&
//			false == CX2ItemManager::IsEDItem( itemID ) &&
//			//NULL != pItemTemplet &&
//			CX2Item::IT_OUTLAY != pItemTemplet->GetItemType() &&
//			LiveUserUnitNum() > 1 ) // user가 2명 이상이면
//		{
//			CX2DungeonRoom* pDungeonRoom = (CX2DungeonRoom*) g_pX2Room;
//			switch( pDungeonRoom->GetDungeonGetItemType() )
//			{
//			case CX2DungeonRoom::DGIT_RANDOM:
//				{
//#ifndef	X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD
//					CSLock locker( m_csGameIntruder );
//#endif	X2OPTIMIZE_GAME_CHARACTER_BACKGROUND_LOAD
//					for( UINT i=0; i<m_UserUnitList.size(); i++ )
//					{
//						CX2GUUser* pGUUser = m_UserUnitList[i];
//						if( NULL != pGUUser && CX2GameUnit::GUSI_DIE != pGUUser->GetGameUnitState() )
//						{
//							if( NULL != pGUUser->GetDiceRoll() )
//							{
//								bDiceRolling = true;
//								if( pGUUser->GetUnitUID() == kEGS_GET_ITEM_NOT.m_GetUnitUID )
//								{
//									pGUUser->GetDiceRoll()->PushDiceRoll( true );
//								}
//								else
//								{
//									pGUUser->GetDiceRoll()->PushDiceRoll( false );
//								}
//							}
//						}
//					}
//
//				} break;
//			}
//		}
//#endif //NOT_USE_DICE_ROLL
	}
	if( -1 == itemID )
	{
		itemID = kEGS_GET_ITEM_NOT.m_iItemID;
		pItemTemplet = g_pData->GetItemManager()->GetItemTemplet( itemID );
	}

	if( NULL == pItemTemplet )
		return false;

	CX2GUUser* pCX2GUUser = GetUserUnitByUID( kEGS_GET_ITEM_NOT.m_GetUnitUID );
	if( pCX2GUUser != NULL )
	{
		bool bIsEDItem = CX2ItemManager::IsEDItem(kEGS_GET_ITEM_NOT.m_iItemID);
		AddChatLogByGetItem( pItemTemplet, pCX2GUUser, bIsEDItem );

		if( pCX2GUUser->GetNowHp() <= 0.0f || pCX2GUUser->GetNowStateID() == CX2GameUnit::GUSI_DIE )
		{
			return true;
		}

		ProcessGetOutlayItem(pItemTemplet, pCX2GUUser,
							kEGS_GET_ITEM_NOT.m_iDropItemUID, kEGS_GET_ITEM_NOT.m_bIsItemSuccess);
		ProcessGetEquipItem(pItemTemplet);
		if( NULL != GetMyUnit() )
		{
			ProcessGetEDItem(pCX2GUUser, bIsEDItem,
							kEGS_GET_ITEM_NOT.m_mapGetED[GetMyUnit()->GetUnitUID()],
							kEGS_GET_ITEM_NOT.m_GetUnitUID);
		}
	}

	return true;
}
//}} kimhc // 실시간 엘소드 중 실시간 ED 획득, 실시간 아이템 획득

bool CX2Game::Handler_EGS_REMAINING_PLAY_TIME_NOT( KEGS_REMAINING_PLAY_TIME_NOT& kEGS_REMAINING_PLAY_TIME_NOT )
{
	if( m_AllowFrameCount < 0 )
	{
		m_SpeedHackCheck++;
		if( m_SpeedHackCheck > 10 )
		{
			g_pKTDXApp->SetFindHacking( true );
		}
	}
	SetAllowFrameCount( (int)(85 * 5.3) );

	return true;
}

bool CX2Game::Handler_EGS_MY_USER_UNIT_INFO_TO_SERVER_REQ()
{
	KEGS_MY_USER_UNIT_INFO_TO_SERVER_REQ kPacket;

	CX2GUUser* pCX2GUUser = GetMyUnit();
	if( NULL == pCX2GUUser )
		return false;

	//{{ 2011. 01. 21	최육사	던전 랭크 개편
	kPacket.m_kMyPlayResult.m_fHP					= pCX2GUUser->GetNowHp();
	kPacket.m_kMyPlayResult.m_nDamageCount			= pCX2GUUser->GetDamageCount();
	kPacket.m_kMyPlayResult.m_TotalScore	= m_GameScore;
	//kPacket.m_ComboScore	= pCX2GUUser->GetComboManager()->GetComboScoreFull();

	if( NULL != pCX2GUUser->GetComboManager() )
	{
		kPacket.m_kMyPlayResult.m_ComboScore	= pCX2GUUser->GetComboManager()->GetComboScoreFull();
	}
	else
	{
		kPacket.m_kMyPlayResult.m_ComboScore	= 0;
	}
	kPacket.m_kMyPlayResult.m_TechScore		= m_TechPointFull;

	kPacket.m_kMyPlayResult.m_iTotalAttackedDamage	= pCX2GUUser->GetTotalAttackedDamage();
	kPacket.m_kMyPlayResult.m_iTotalRecoveryHP		= pCX2GUUser->GetTotalRecoveryHP();
	kPacket.m_kMyPlayResult.m_iUsingKindOfSkill		= pCX2GUUser->GetUsingSkillCount();
	kPacket.m_kMyPlayResult.m_iTotalGivenDamamge	= pCX2GUUser->GetTotalGivenDamamge();
	kPacket.m_kMyPlayResult.m_iDashCount = pCX2GUUser->GetUsingDashCount();
		kPacket.m_kMyPlayResult.m_bSkillSlotType = g_pMain->GetGameOption().GetIsSkillUITypeA();
	kPacket.m_kMyPlayResult.m_iFrame = static_cast<int>(m_kGameStatistics.m_kAverageFps.m_fAverageFps);
	//}}

	g_pData->GetServerProtocol()->SendPacket( EGS_MY_USER_UNIT_INFO_TO_SERVER_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_MY_USER_UNIT_INFO_TO_SERVER_ACK, 60.0f );

	return true;
}

bool CX2Game::Handler_EGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT()
{
	KEGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT kPacket;

	CX2GUUser* pCX2GUUser = GetMyUnit();
	if( NULL == pCX2GUUser )
		return false;

	kPacket.m_nDamageCount	= pCX2GUUser->GetDamageCount();

	//kPacket.m_ComboScore	= pCX2GUUser->GetComboManager()->GetComboScoreFull();

	if( NULL != pCX2GUUser->GetComboManager() )
	{
		kPacket.m_ComboScore	= pCX2GUUser->GetComboManager()->GetComboScoreFull();
	}
	else
	{
		kPacket.m_ComboScore	= 0;
	}
	kPacket.m_TechScore		= m_TechPointFull;

	//kPacket.m_sSkillUseLv1 = pCX2GUUser->GetGageManager()->GetSkillCountForStatistics(1);
	//kPacket.m_sSkillUseLv2 = pCX2GUUser->GetGageManager()->GetSkillCountForStatistics(2);
	//kPacket.m_sSkillUseLv3 = pCX2GUUser->GetGageManager()->GetSkillCountForStatistics(3);

	g_pData->GetServerProtocol()->SendPacket( EGS_DUNGEON_PLAY_INFO_TO_SERVER_NOT, kPacket );


	return true;
}

bool CX2Game::Handler_EGS_ADD_ON_STAT_ACK( KEGS_ADD_ON_STAT_ACK& kEGS_ADD_ON_STAT_ACK )
{
	return true;
}

bool CX2Game::Handler_EGS_ADD_ON_STAT_NOT( KEGS_ADD_ON_STAT_NOT& kEGS_ADD_ON_STAT_NOT )
{
	if( kEGS_ADD_ON_STAT_NOT.m_bIsUnit == true )
	{
		CX2GUUser* pCX2GUUser = GetUserUnitByUID( kEGS_ADD_ON_STAT_NOT.m_UID );
		if( NULL != pCX2GUUser )
		{
			pCX2GUUser->SetAddOnStat( (KEGS_ADD_ON_STAT_REQ::ADD_ON_STAT)kEGS_ADD_ON_STAT_NOT.m_cStatType, kEGS_ADD_ON_STAT_NOT.m_fStatVal, kEGS_ADD_ON_STAT_NOT.m_fTime );
		}
	}
	else
	{
		CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( (int)kEGS_ADD_ON_STAT_NOT.m_UID );

		//{{ JHKang / 강정훈 / 2010/12/07 // 특정 NPC 클래스 타입 버프 적용 안하기
		if ( NULL != pCX2GUNPC )
		{
			if ( pCX2GUNPC->GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_THING_NOBUFF )
				return true;
		}
		//}} JHKang / 강정훈 / 2010/12/07 // 특정 NPC 클래스 타입 버프 적용 안하기

		if( NULL != pCX2GUNPC )
		{
			pCX2GUNPC->SetAddOnStat( (KEGS_ADD_ON_STAT_REQ::ADD_ON_STAT)kEGS_ADD_ON_STAT_NOT.m_cStatType, kEGS_ADD_ON_STAT_NOT.m_fStatVal, kEGS_ADD_ON_STAT_NOT.m_fTime );
		}
	}
	return true;
}

void CX2Game::InitF1HelpString()
{
	m_vecF1HelpString.resize(0);
	wstring wstrMessage;

	wstrMessage = GET_STRING( STR_ID_202 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_203 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_204 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_205 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_206 );
	m_vecF1HelpString.push_back( wstrMessage );


	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_207 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_208 );
	m_vecF1HelpString.push_back( wstrMessage );

    wstrMessage = L"    ";
	wstrMessage += GET_STRING( STR_ID_209 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_210 );
	m_vecF1HelpString.push_back( wstrMessage );

    wstrMessage = L"    ";
	wstrMessage += GET_STRING( STR_ID_211 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_212 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_213 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"                ";
    wstrMessage += GET_STRING( STR_ID_214 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"                ";
    wstrMessage = GET_STRING( STR_ID_215 );
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = L"";
	m_vecF1HelpString.push_back( wstrMessage );

	wstrMessage = GET_STRING( STR_ID_216 );
	m_vecF1HelpString.push_back( wstrMessage );
}



void CX2Game::RenderF1Help()
{
	if( true == m_bShowF1Help )
	{
		const int MAGIC_OFFSET_X = 1024 / 8;
		const int MAGIC_OFFSET_Y = 768 / 8;

		const int MAGIC_WIDTH_X = 1024 / 8 * 6;
		const int MAGIC_WIDTH_Y = 768 / 8 * 6;


		const int MAGIC_TEXT_OFFSET_X = 20;
		const int MAGIC_TEXT_OFFSET_Y = 5;


		m_pTextureF1Help->Draw( MAGIC_OFFSET_X, MAGIC_OFFSET_Y, MAGIC_WIDTH_X, MAGIC_WIDTH_Y, D3DCOLOR_RGBA( 0, 0, 0, 200 ) );

		for( UINT i=0; i<m_vecF1HelpString.size(); i++ )
		{
			int iPosX = MAGIC_OFFSET_X + MAGIC_TEXT_OFFSET_X;
			int iPosY = (int)( MAGIC_OFFSET_X + MAGIC_TEXT_OFFSET_Y + i * 19 );

			m_pFontF1Help->OutTextXY( iPosX, iPosY, m_vecF1HelpString[i].c_str(), D3DCOLOR_RGBA(255,255,255,255),
				CKTDGFontManager::FS_SHELL, D3DCOLOR_RGBA(0,0,0,255) );
		}
	}
}

void CX2Game::Verify()
{
	if( g_pKTDXApp->GetIsNowVeryfy() == true )
	{
		bool bVerify = true;

		if( m_GameScore.Verify()		== false
			|| m_GameScoreBack.Verify()	== false
			|| m_TechPoint.Verify()		== false
			|| m_TechPointFull.Verify()	== false

			|| m_iSubStageTechPoint.Verify() == false
			)
		{
			bVerify = false;

			m_GameScore = -1;
			m_GameScoreBack = -1;
			m_TechPoint = 0;
			m_TechPointFull = 0;
		}

		for( int i = 0; i < (int)m_ScoreTimeList.size(); i++ )
		{
			ScoreTime* pScoreTime = m_ScoreTimeList[i];
			if( pScoreTime->m_Score.Verify() == false )
			{
				bVerify = false;

				pScoreTime->m_Score = 0;
			}
		}

		if( bVerify == false )
		{
			if( g_pData != NULL && g_pData->GetServerProtocol() != NULL &&
				g_pData->GetMyUser() != NULL &&
				g_pData->GetMyUser()->GetUserData().hackingUserType != CX2User::HUT_AGREE_HACK_USER )
			{
				g_pData->GetServerProtocol()->SendID( EGS_REPORT_HACK_USER_NOT );
				g_pData->GetMyUser()->AccessUserData().hackingUserType = CX2User::HUT_AGREE_HACK_USER;
			}

			g_pMain->SendHackMail_DamageHistory( ANTI_HACK_STRING_GAME_VERIFY );
			//g_pKTDXApp->SetFindHacking( true );
		}

		if( m_iNpcDiePacket.Verify() == false || m_iNpcDiePacket < 0 )
		{
			static bool bSend = false;

			if( bSend == false )
			{
				g_pMain->SendHackMail_DamageHistory( ANTI_HACK_STRING_NPC_DIE_PACKET_VERIFY );
				bSend = true;
			}
		}
	}
}

void CX2Game::UseItemSpecialAbility( const CX2Item::ItemTemplet* pItemTemplet, CX2GUUser* pCX2GUUser )		// 던전내에서 바로 사용되는 특별한 아이템 등을 먹었을때
{
	float	fIncreaseRateFromMasterOfAlchemistSkill							= 1.0f;
	//{{ kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)
	bool	bDoIHaveMasterOfAlchemistSkill			= false;
	int		iSkillLvMasterOfAlchemist				= 0;

	if ( pCX2GUUser->GetUnit() != NULL )
	{
		BYTE byMemberShipGrade = pCX2GUUser->GetUnit()->GetUnitData().m_byMemberShipGrade;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pCX2GUUser->GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_GP_COMMON_MASTER_OF_ALCHEMIST, byMemberShipGrade);

		if ( pSkillTemplet != NULL )
		{
			bDoIHaveMasterOfAlchemistSkill  = true;

			const CX2UserSkillTree& userSkillTree = pCX2GUUser->GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// 스킬 레벨

			fIncreaseRateFromMasterOfAlchemistSkill = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillTempletLevel );
		}

	}
	//}} kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)


	ProcSpecialItem(pItemTemplet, pCX2GUUser);


    unsigned int    uNumSA = pItemTemplet->GetNumSpecialAbility();
	for( int i = 0; i < (int)uNumSA ; i++ )
	{
		const CX2Item::SpecialAbility* pSa = &pItemTemplet->GetSpecialAbility(i);
		switch( pSa->GetType() )
		{
		case CX2Item::SAT_HP_UP:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					float fIncreateRateHpUp = fIncreaseRateFromMasterOfAlchemistSkill;
					float fValue = static_cast< float >( pSa->m_Value1 );

//{{ kimhc // 2011.1.14 // 청 1차 전직
					// 퓨리가디언 신진대사 촉진
					if ( CX2Unit::UC_CHUNG_FURY_GUARDIAN == pCX2GUUser->GetUnitClass() || CX2Unit::UC_CHUNG_IRON_PALADIN == pCX2GUUser->GetUnitClass() )
					{
						const CX2UserSkillTree& userSkillTree = pCX2GUUser->GetUnit()->GetUnitData().m_UserSkillTree;

						int iSkillLv = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

						if ( 0 < iSkillLv )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet =
								g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );

							if ( NULL != pSkillTemplet )
							{
								float fIncreaseRateMetabolism = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLv );

								fIncreaseRateMetabolism = CalculateIncreasingRate( fIncreaseRateMetabolism );
								fIncreateRateHpUp += fIncreaseRateMetabolism;
							}
						} // if
					}
//}} kimhc // 2011.1.14 // 청 1차 전직

					//{{ kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)
					fValue *= fIncreateRateHpUp;
					//}} kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)

					if( pCX2GUUser->IsMyUnit() == true )
					{

			//{{ kimhc // 2011-04-26 // 1랩 보정 던전에서 고정 수치 물약의 회복양이 너무 많아서 수정
						if ( GT_DUNGEON == GetGameType() )
						{
							CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(this);
							if( NULL != pDungeonGame &&
								NULL != pDungeonGame->GetDungeon() &&
								NULL != pDungeonGame->GetDungeon()->GetDungeonData())
							{
								const SEnum::DUNGEON_ID eDungeonId = pDungeonGame->GetDungeon()->GetDungeonData()->m_DungeonID;
								if( CX2Dungeon::IsEventDungeon( eDungeonId ) )
								{
									const int iBasicHP = 10000;
									const int iHpByLevel = 1500;

									fValue = iBasicHP * fValue / ( iBasicHP + ( pCX2GUUser->GetUnitLevel() * iHpByLevel ) );
								}
							}
						}
			//}} kimhc // 2011-04-26 // 1랩 보정 던전에서 고정 수치 물약의 회복양이 너무 많아서 수정

						pCX2GUUser->UpNowHp( fValue );

						//{{ JHKang / 강정훈 / 2011/02/14 / 던전 랭크 개선 관련
						pCX2GUUser->AddRecoveryHP( static_cast<int>(fValue) );
						//}} JHKang / 강정훈 / 2011/02/14 / 던전 랭크 개선 관련
					}
					std::wstringstream strStream;
					strStream << L"HP+ " << static_cast< int >( fValue );
					D3DXVECTOR3 pos = pCX2GUUser->GetPos();
					pos.y += 50.0f + (i * 30.0f);
					if( NULL != g_pData->GetPicCharGreen() )
						g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );


				}
			}
			break;
		case CX2Item::SAT_MP_UP:
			{
				float fIncreaseRateMpUp = fIncreaseRateFromMasterOfAlchemistSkill;
				float fValue = static_cast< float >( pSa->m_Value1 );
				//{{ kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)
				fValue *= fIncreaseRateMpUp;
				//}} kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)

				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->UpNowMp( fValue );
				}
				std::wstringstream strStream;
				strStream << L"MP+ " << static_cast< int >( fValue );
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
					g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );

			}
			break;
		case CX2Item::SAT_REMOVE_CURSE:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_CURSE);


				}
				//std::wstringstream strStream;
				//strStream << L"MP+ " << pSa->m_Value1;
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
				{
					g_pData->GetPicCharBlue()->DrawText( L"REMOVE CURSE", pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
			break;
		case CX2Item::SAT_REMOVE_SLOW:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_SLOW);
				}
				//std::wstringstream strStream;
				//strStream << L"MP+ " << pSa->m_Value1;
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
				{
					g_pData->GetPicCharBlue()->DrawText( L"REMOVE SLOW", pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
			break;
		case CX2Item::SAT_REMOVE_FIRE:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_FIRE);
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_ENCHANT_BLAZE);


				}
				//std::wstringstream strStream;
				//strStream << L"MP+ " << pSa->m_Value1;
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
				{
					g_pData->GetPicCharBlue()->DrawText( L"REMOVE FIRE", pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
			break;
		case CX2Item::SAT_REMOVE_POISON:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_POISON);
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_ENCHANT_POISON);


				}
				//std::wstringstream strStream;
				//strStream << L"MP+ " << pSa->m_Value1;
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
				{
					g_pData->GetPicCharBlue()->DrawText( L"REMOVE POISON", pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
			break;

		case CX2Item::SAT_HYPER_MODE:
			{
				if ( pCX2GUUser->IsMyUnit() == true &&
					m_GameType == GT_PVP ) // 오현빈 // 2012-10-15 필드런칭 이후에는 각성반지 효과가 대전에서만 먹히도록 수정
				{
					pCX2GUUser->SetHasHyperModeItem(true);
				}
			}
			break;

		case CX2Item::SAT_SOUL_GAGE_UP:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					/// 애드일 땐, DP 게이지를 올려 주세요
					if ( NULL != pCX2GUUser->GetUnit() && CX2Unit::UT_ADD == pCX2GUUser->GetUnit()->GetType() )
						pCX2GUUser->UpNowDPValue( 2000.f );
					else
					{
						int iNowSoul = static_cast<int>(pCX2GUUser->GetNowSoul()) + pSa->m_Value1;
						int iNowHyper = pCX2GUUser->GetHyperModeCount();
						int iPrevHyper = iNowHyper;
						while( iNowSoul >= 100 )
						{
							iNowSoul -= 100;
							++iNowHyper;
						}

						if( iNowHyper > 3 )
						{
							iNowHyper = 3;
						}

						if( iPrevHyper < iNowHyper )
						{
							iNowSoul = 0;
						}

						pCX2GUUser->SetHyperModeCount( iNowHyper );
						pCX2GUUser->SetNowSoul( iNowSoul );
					}
				}

				std::wstringstream strStream;
				strStream << L"SOUL+ " << pSa->m_Value1;
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharRed() )
					g_pData->GetPicCharRed()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
			break;

		case CX2Item::SAT_WAY_OF_SWORD_GAUGE_UP:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->SetWayOfSwordPoint( pCX2GUUser->GetWayOfSwordPoint() + pSa->m_Value1 );
				}
			}
			break;

		case CX2Item::SAT_SHOW_OPPONENT_MP:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->AccessCashItemAbility().m_bShowOppnentMP = true;
				}
			}
			break;

		case CX2Item::SAT_UP_MP_AT_ATTACK_OR_DAMAGE:
			{
				pCX2GUUser->AccessCashItemAbility().m_fUpMPAtAttackOrDamage = (float) pSa->m_Value1;	// 증가량, // fix!! 중첩이 안되는 문제가 있음.
			}
			break;

		case CX2Item::SAT_HP_PERCENT_UP:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					float fIncreateRateHpPercentUp = fIncreaseRateFromMasterOfAlchemistSkill;
					float fValue = pSa->m_Value1 / 100.0f;

					//{{ kimhc // 2011.1.14 // 청 1차 전직
					// 퓨리가디언 신진대사 촉진
					if ( CX2Unit::UC_CHUNG_FURY_GUARDIAN == pCX2GUUser->GetUnitClass() || CX2Unit::UC_CHUNG_IRON_PALADIN == pCX2GUUser->GetUnitClass() )
					{
						const CX2UserSkillTree& userSkillTree = pCX2GUUser->GetUnit()->GetUnitData().m_UserSkillTree;

						int iSkillLv = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

						if ( 0 < iSkillLv )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet =
								g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );

							if ( NULL != pSkillTemplet )
							{
								float fIncreaseRateMetabolism = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLv );

								fIncreaseRateMetabolism = CalculateIncreasingRate( fIncreaseRateMetabolism );
								fIncreateRateHpPercentUp += fIncreaseRateMetabolism;
							}
						} // if
					}
					//}} kimhc // 2011.1.14 // 청 1차 전직

					//{{ kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)
					if ( bDoIHaveMasterOfAlchemistSkill == true &&
                        EGG_ITEM_ID == pItemTemplet->GetItemID()
                         )
					{
						fValue += ( iSkillLvMasterOfAlchemist / 100.0f );
					}
					//}} kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)
					fValue *= fIncreateRateHpPercentUp;

					if( pCX2GUUser->IsMyUnit() == true )
					{
						pCX2GUUser->UpNowHp( pCX2GUUser->GetMaxHp() * fValue );
					}
						//{{ JHKang / 강정훈 / 2011/02/14 / 던전 랭크 개선 관련
						// 상자에서 나온 HP 회복 템이 아닌 경우 적용
						if ( pItemTemplet->GetEqipPosition() != CX2Unit::EP_NONE )
							pCX2GUUser->AddRecoveryHP( static_cast<int>(fValue) );
						//}} JHKang / 강정훈 / 2011/02/14 / 던전 랭크 개선 관련

					std::wstringstream strStream;
					strStream << L"HP+ " << static_cast< int >( fValue * 100 ) << L" PERCENT";

					D3DXVECTOR3 pos = pCX2GUUser->GetPos();
					pos.y += 50.0f + (i * 30.0f);
					if( NULL != g_pData->GetPicCharGreen() )
						g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );

				}
			}
			break;
//{{ 09.06.08 태완 : MP 퍼센트로 채우는 스킬
		case CX2Item::SAT_MP_PERCENT_UP:
			{
				float fValue = pSa->m_Value1 / 100.0f;
				//{{ kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)
				if ( bDoIHaveMasterOfAlchemistSkill == true )
					fValue *= fIncreaseRateFromMasterOfAlchemistSkill;
				//}} kimhc // 2009-11-17 //길드 스킬 중 연금술의 달인(포션, 음식, 던전 회복 아이템의 효과가 X%증가)

				if( pCX2GUUser->IsMyUnit() == true )
					pCX2GUUser->UpNowMp( pCX2GUUser->GetMaxMp() * fValue );

				std::wstringstream strStream;
				strStream << L"MP+ " << static_cast< int >( pSa->m_Value1 * fIncreaseRateFromMasterOfAlchemistSkill ) << L" PERCENT";
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
					g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
			break;
//}}

		case CX2Item::SAT_TEAM_HP_UP:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					for( UINT i=0; i<m_UserUnitList.size(); i++ )
					{
						CX2GUUser* pGUUser = m_UserUnitList[i];
						if( NULL != pGUUser &&
							pGUUser->GetNowHp() > 0.f &&
							pGUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
						{
							if( pGUUser->GetTeam() == pCX2GUUser->GetTeam() )
							{
								pGUUser->UpNowHp( static_cast<float>( pSa->m_Value1 ) );

								std::wstringstream strStream;
								strStream << L"HP+ " << pSa->m_Value1 << L" PERCENT";
								D3DXVECTOR3 pos = pGUUser->GetPos();
								pos.y += 50.0f + (i * 30.0f);
								if( NULL != g_pData->GetPicCharGreen() )
								{
										g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pGUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
								}
							}
						}
					}
				}
			} break;


		case CX2Item::SAT_TEAM_MP_UP:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					for( UINT i=0; i<m_UserUnitList.size(); i++ )
					{
						CX2GUUser* pGUUser = m_UserUnitList[i];
						if( NULL != pGUUser &&
							pGUUser->GetNowHp() > 0.f &&
							pGUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
						{
							if( pGUUser->GetTeam() == pCX2GUUser->GetTeam() )
							{
								pGUUser->UpNowMp( static_cast<float>( pSa->m_Value1 ) );

								std::wstringstream strStream;
								strStream << L"MP+ " << pSa->m_Value1 << L" PERCENT";
								D3DXVECTOR3 pos = pGUUser->GetPos();
								pos.y += 50.0f + (i * 30.0f);
								if( NULL != g_pData->GetPicCharGreen() )
								{
									g_pData->GetPicCharGreen()->DrawText( strStream.str().c_str(), pos, pGUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
								}
							}
						}
					}
				}
			} break;

		case CX2Item::SAT_PEPPER_RUN:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					pCX2GUUser->StartPepperRun( (float)pSa->m_Value1 );
				}
			} break;

		case CX2Item::SAT_MP_REGENERATION:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					pCX2GUUser->SetChangeRateByItem( static_cast<float>( pSa->m_Value1 ), static_cast<float>( pSa->m_Value2 ) );
				}
			}
			break;
		case CX2Item::SAT_USE_HYPER:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					/// 애드일 땐, DP 게이지를 올려 주세요
					if ( NULL != pCX2GUUser->GetUnit() && CX2Unit::UT_ADD == pCX2GUUser->GetUnit()->GetType() )
						pCX2GUUser->UpNowDPValue( 2000.f );
					else
						pCX2GUUser->ForceChangeHyperModeWithoutMotion( (float)pSa->m_Value1 );

					if( GetFocusUnit() != NULL )
						GetFocusUnit()->PlaySound( L"Alarm_Fever.ogg" );
					else if( GetMyUnit() != NULL )
						GetMyUnit()->PlaySound( L"Alarm_Fever.ogg" );
				}
			}
			break;
		case CX2Item::SAT_SUPERARMOR:
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					pCX2GUUser->SetSuperArmor(true, (float)pSa->m_Value1 );
				}
			}
			break;
		case CX2Item::SAT_SUMMON_SPIRIT:	// 정령소환
			{
				if( pCX2GUUser != NULL
					&& pCX2GUUser->GetNowHp() > 0.0f
					&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
				{
					// 정령소환시 타임스탑
					//StopAllUnit(1.f, &m_vDropItemPos, 3000.f, pCX2GUUser, 1.f);

					wstring wstrSpiritName = SummonSpiritEffectName( pSa->m_Value1 );
					CX2EffectSet::Handle hSpirit = GetEffectSet()->PlayEffectSet( wstrSpiritName.c_str(), (CX2GameUnit*) pCX2GUUser, NULL, false, -1.f, -1.f, D3DXVECTOR3(1.f, 1.f, 1.f), true, m_vDropItemPos );
					m_vechEffectSetSpirit.push_back(hSpirit);

					switch( pSa->m_Value1 )
					{
					case 1:	// 오리에드의 분노
						SetWorldColor_LUA( D3DXCOLOR(1.f,0.2f,0.2f,1.f) );
						pCX2GUUser->PlaySound(L"Summon_Oread.ogg");
						break;
					case 2:	// 드라이어드의 분노
						SetWorldColor_LUA( D3DXCOLOR(0.2f,1.f,0.2f,1.f) );
						pCX2GUUser->PlaySound(L"Summon_Dryad.ogg");
						break;
					case 3:	// 라울의 분노
						SetWorldColor_LUA( D3DXCOLOR(0.2f,0.2f,1.f,1.f) );
						pCX2GUUser->PlaySound(L"Summon_Raul.ogg");
						break;
					case 4:	// 일리피아의 손길
						SetWorldColor_LUA( D3DXCOLOR(0.5f,0.5f,0.2f,1.f) );
						pCX2GUUser->PlaySound(L"Summon_Elifea1.ogg");
						break;
					default:
						break;
					}

					if( pSa->m_Value1 == 1 || pSa->m_Value1 == 3 )
					{
						// 라울의 분노 데미지 이펙트 생성
						m_vecEffectPos.clear();
						m_optrItemDamageEffectGameUnit = pCX2GUUser;
						m_fItemDamageEffectTime = 0.85f;
						m_iItemDamageEffectIndex = pSa->m_Value1;
					}
				}
			}
			break;
		case CX2Item::SAT_REMOVE_FROZEN:
			{
				if( pCX2GUUser->IsMyUnit() == true )
				{
					pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_COLD);
					//pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_FROZEN);
					//pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_WATER_HOLD);
					//pCX2GUUser->AccessExtraDamagePack().Init(CX2DamageManager::EDT_ENCHANT_FROZEN);

					pCX2GUUser->CureExtraDamage( CX2DamageManager::EDT_FROZEN );
					pCX2GUUser->CureExtraDamage( CX2DamageManager::EDT_WATER_HOLD );
					pCX2GUUser->CureExtraDamage( CX2DamageManager::EDT_ENCHANT_FROZEN );

				}
				D3DXVECTOR3 pos = pCX2GUUser->GetPos();
				pos.y += 50.0f + (i * 30.0f);
				if( NULL != g_pData->GetPicCharBlue() )
					g_pData->GetPicCharBlue()->DrawText( L"REMOVE FROZEN", pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
			break;
		case CX2Item::SAT_SPECIAL_SKILL:
			{
				g_pData->GetUIManager()->GetUIQuickSlot()->SetWaiting_EGS_USE_QUICK_SLOT_ACK( false );

				if ( pSa->m_Value2 == CX2Item::SSMT_THROW )
				{
					pCX2GUUser->SetThrowItemValue( pSa->m_Value1 );
					pCX2GUUser->StateChange( CX2GUUser::USI_THROW_ITEM, true );
					//pCX2GUUser->StateChange( CX2GUUser::USI_THROW_ITEM, pSa->m_Value1, true );
					//pCX2GUUser->StateChange( CX2GUElsword_SwordMan::ESSI_THROW_WOODEN_PIECE, pSa->m_Value1 , true );
				}
				else if ( pSa->m_Value2 == CX2Item::SSMT_INSTALL )
				{
					pCX2GUUser->StateChange( CX2GUUser::USI_INSTALL_ITEM, true );
					//pCX2GUUser->StateChange( CX2GUElsword_SwordMan::ESSI_SUMMON_MAGIC_SPEAR, true );
				}
				else if ( pSa->m_Value2 == CX2Item::SSMT_DIRECT_USE )//지헌 : 특수 아이템 사용 - 바람구슬
				{
					if( pSa->m_Value1 == CX2Item::SST_FIRE_SPHERE )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"SPECIAL_ITEM_EFFECT_FIRE", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_WATER_SPHERE )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"SPECIAL_ITEM_EFFECT_WATER", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_NATURE_SPHERE )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"SPECIAL_ITEM_EFFECT_NATURE", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_WIND_SPHERE )
					{
						// 지헌 : 바람은 유저 전체 발 밑에 이펙트를 한번 뿌려주고(1), 버프를 걸어 줘야 한다(2)
						// 여기서는 이펙트 만들지 맙시다.

						// 라울의 분노 데미지 이펙트 생성
						m_vecEffectPos.clear();
						m_optrItemDamageEffectGameUnit = pCX2GUUser;
						m_fItemDamageEffectTime = 0.1f;
						m_iItemDamageEffectIndex = pSa->m_Value1;
						//GetDamageEffect()->CreateInstance( pCX2GUUser, L"SPECIAL_ITEM_EFFECT_WIND", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_LIGHT_SPHERE )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"SPECIAL_ITEM_EFFECT_LIGHT", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_DARK_SPHERE )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"SPECIAL_ITEM_EFFECT_DARK", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_LOVE_LV1 )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"RELATIONSHIP_SKILL_THROW_LV1", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
					else if( pSa->m_Value1 == CX2Item::SST_LOVE_LV2 )
					{
						GetDamageEffect()->CreateInstance( pCX2GUUser, L"RELATIONSHIP_SKILL_THROW_LV2", 1, pCX2GUUser->GetPos(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetRotateDegree(), pCX2GUUser->GetPos().y );
					}
				}

				SetEnableAllKeyProcess( true );
			}
			break;

//{{ mauntain : 김태환 [2012.06.14] 몬스터 카드 소환 기능 - 카드 사용시 해당 몬스터 소환
		case CX2Item::SAT_SUMMON_MONSTER:
			{
				switch ( g_pMain->GetNowStateID() )
				{
				case CX2Main::XS_DUNGEON_GAME:
                    CreateSummonMonsterCardNPC( pSa, pCX2GUUser );
					break;

				default:
					break;
				}
			}
			break;
			//}}
		case CX2Item::SAT_ARA_FORCE_POWER_PERCENT_UP:
			{
				if( pCX2GUUser != NULL &&
					pCX2GUUser->GetNowHp() > 0.0f &&
					pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE &&
					pCX2GUUser->GetUnit() != NULL &&
					pCX2GUUser->GetUnit()->GetType() == CX2Unit::UT_ARA &&
					pCX2GUUser->IsMyUnit() == true )
				{
					CX2AraGageData*	pAraGageData = static_cast<CX2AraGageData*>( pCX2GUUser->GetGageData() );

					float fValue = static_cast<float>(pSa->m_Value1) / 100.0f;
					int iNowForcePower = pAraGageData->GetNowForcePower();
					int iMaxForcePower = pAraGageData->GetMaxForcePower();
					iNowForcePower += static_cast<int>(iMaxForcePower * fValue);

					if( iNowForcePower > iMaxForcePower )
					{
						iNowForcePower = iMaxForcePower;
					}

					pAraGageData->SetForcePowerChanged( true );
					pAraGageData->SetNowForcePower( iNowForcePower );
				}
			}
			break;
		case CX2Item::SAT_RIDINGPET_STAMINA_PERCENT_UP:
			{
				if( pCX2GUUser != NULL &&
					pCX2GUUser->GetNowHp() > 0.0f &&
					pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE &&
					pCX2GUUser->GetUnit() != NULL &&
					pCX2GUUser->IsMyUnit() == true )
				{
					if(!CX2RidingPetManager::GetInstance()->SetRidingPetStaminaPercentUP(pSa->m_Value1))
					{
						// 에러메시지 출력예정
					}
				}
			}
			break;
		}
	}
}

void CX2Game::UseItemPetAbility( const CX2Item::ItemTemplet* pItemTemplet, CX2GUUser* pCX2GUUser )
{
	if( pCX2GUUser != NULL
		&& pCX2GUUser->GetNowHp() > 0.0f
		&& pCX2GUUser->GetNowStateID() != CX2GameUnit::GUSI_DIE )
	{
		if( g_pData != NULL && g_pData->GetPetManager() != NULL )
		{
			CX2PET *pPet = g_pData->GetPetManager()->GetMyPet();
			if( pPet != NULL && pPet->GetXSkinAnim() != NULL )
			{
				if( pItemTemplet->GetItemID() == PET_ATTACK_SKILL_GM_ITEM_ID )
				{
					g_pKTDXApp->GetDeviceManager()->PlaySound( L"Common_Item_PetWhistle_Attack.ogg", false, false );
					pPet->StateChange( pPet->m_AttackState, true, false );
				}
				else if( pItemTemplet->GetItemID() == PET_CHEER_SKILL_GM_ITEM_ID )
				{
					g_pKTDXApp->GetDeviceManager()->PlaySound( L"Common_Item_PetWhistle_CheerUp.ogg", false, false );
					pPet->StateChange( pPet->m_CheerState, true, false );
				}
			}
		}
	}
}

//{{ kimhc // 실시간 엘소드 중 실시간 장비 및 아바타 교체

bool CX2Game::RemoveSpecialAbilityInEquip( const CX2Item::ItemTemplet* const pItemTemplet, CX2GUUser* const pCXGUUser )
{

	if ( pItemTemplet == NULL || pCXGUUser == NULL )
		return false;

	//if ( pCXGUUser->GetCashItemAbility() == NULL )
	//	return false;

	const CX2Item::SpecialAbility* pSa = NULL;
    unsigned    uNumSA = pItemTemplet->GetNumSpecialAbility();
	for( size_t i = 0; i < uNumSA; i++ )
	{
        pSa = &pItemTemplet->GetSpecialAbility( i );

		switch( pSa->GetType() )
		{
		case CX2Item::SAT_SHOW_OPPONENT_MP:
			{
				pCXGUUser->AccessCashItemAbility().m_bShowOppnentMP = false;
			}
			break;

		case CX2Item::SAT_UP_MP_AT_ATTACK_OR_DAMAGE:
			{
				pCXGUUser->AccessCashItemAbility().m_fUpMPAtAttackOrDamage = 0.0f;	// 증가량
			}
			break;
		}
	}

	return true;
}

bool CX2Game::SetSpecialAbilityInEquip( const CX2Item::ItemTemplet* const pItemTemplet, CX2GUUser* const pCXGUUser )
{
	if ( pItemTemplet == NULL || pCXGUUser == NULL )
		return false;

	//if ( pCXGUUser->GetCashItemAbility() == NULL )
	//	return false;

	const CX2Item::SpecialAbility* pSa = NULL;
    unsigned    uNumSA = pItemTemplet->GetNumSpecialAbility();
	for( size_t i = 0; i < uNumSA; i++ )
	{
        pSa = &pItemTemplet->GetSpecialAbility( i );

		switch( pSa->GetType() )
		{
		case CX2Item::SAT_SHOW_OPPONENT_MP:
			{
				pCXGUUser->AccessCashItemAbility().m_bShowOppnentMP = true;
			}
			break;

		case CX2Item::SAT_UP_MP_AT_ATTACK_OR_DAMAGE:
			{
				pCXGUUser->AccessCashItemAbility().m_fUpMPAtAttackOrDamage = static_cast< float >( pSa->m_Value1 );	// 증가량
			}
			break;
		}
	}

	return true;
}

//}} kimhc // 실시간 엘소드 중 실시간 장비 및 아바타 교체

void CX2Game::SetGameScore()
{
	if ( g_pX2Room != NULL && g_pX2Room->GetMySlot() != NULL && g_pX2Room->GetMySlot()->m_bObserver == false &&
		g_pX2Room->GetRoomType() != CX2Room::RT_TRAININGCENTER
		)
	{
		WCHAR texBuf[30] = {0,};
		StringCchPrintf( texBuf, 30, L"%09d", m_GameScore );
		//wsprintf( texBuf, L"%09d", m_GameScore );
		if( NULL != g_pData->GetPicCharGameScore() )
		{
			g_pData->GetPicCharGameScore()->Clear();
			g_pData->GetPicCharGameScore()->DrawText( texBuf, D3DXVECTOR3(290,-40,0), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_LEFT, 0.03f );
		}
	}
}

void CX2Game::SetGameScore( int gameScore )
{

	m_GameScoreBack	= m_GameScore;
	m_GameScore		= gameScore;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		if( g_pData->GetPicCharGameScore() != NULL )
			g_pData->GetPicCharGameScore()->Clear();

		return;
	}

	if( m_GameScoreBack != m_GameScore )
	{
		if ( g_pX2Room != NULL && g_pX2Room->GetMySlot() != NULL && g_pX2Room->GetMySlot()->m_bObserver == false &&
			g_pX2Room->GetRoomType() != CX2Room::RT_TRAININGCENTER
			)
		{
			WCHAR texBuf[30] = {0,};
			StringCchPrintf( texBuf, 30, L"%09d", m_GameScore );
			//wsprintf( texBuf, L"%09d", m_GameScore );
			if( NULL != g_pData->GetPicCharGameScore() )
			{
				g_pData->GetPicCharGameScore()->Clear();
				g_pData->GetPicCharGameScore()->DrawText( texBuf, D3DXVECTOR3(290,-40,0), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_LEFT, 0.03f );

			}
		}
	}
}
void CX2Game::AddGameScore( int gameScore )
{
	m_GameScoreBack = m_GameScore;
	m_GameScore		+= gameScore;

	return;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		if( g_pData->GetPicCharGameScore() != NULL )
			g_pData->GetPicCharGameScore()->Clear();

		return;
	}


	if( m_GameScoreBack != m_GameScore )
	{
		if ( g_pX2Room != NULL && g_pX2Room->GetMySlot() != NULL && g_pX2Room->GetMySlot()->m_bObserver == false &&
			g_pX2Room->GetRoomType() != CX2Room::RT_TRAININGCENTER
			)
		{
			WCHAR texBuf[30] = {0,};
			StringCchPrintf( texBuf, 30, L"%09d", m_GameScore );
			//wsprintf( texBuf, L"%09d", m_GameScore );
			if( NULL != g_pData->GetPicCharGameScore() )
			{
				//g_pData->GetPicCharGameScore()->Clear();
				//g_pData->GetPicCharGameScore()->DrawText( texBuf, D3DXVECTOR3(290,-40,0), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_LEFT, 0.03f );
			}
		}

	}
}

void CX2Game::AddGameScore( int gameScore, float fTime )
{

	ScoreTime* pScoreTime = new ScoreTime();
	pScoreTime->m_Score = gameScore;
	pScoreTime->m_fTime = fTime;

	m_ScoreTimeList.push_back( pScoreTime );
}






int CX2Game::GetSubStageTechPoint () const
{
	return m_TechPointFull - m_iSubStageTechPoint;		// 누적된 TechPoint 에 현재 SubStage 를 제외한 TechPoint 를 빼서 갖고 있는다.

}

void CX2Game::ResetSubStageTechPoint()
{
	m_iSubStageTechPoint = m_TechPointFull;
}




void CX2Game::SetTechPoint( int techPoint )
{

	m_TechPoint			= techPoint;
	m_TechPointFull		= m_TechPoint;
	if( NULL != g_pData->GetPicCharTechPoint() )
	{
		g_pData->GetPicCharTechPoint()->Clear();
		g_pData->GetPicCharTechPoint()->DrawText( m_TechPoint, D3DXVECTOR3(185,310,0), D3DXVECTOR3(1,0,0), CKTDGPicChar::AT_LEFT );
	}

	if( m_fTechPointViewTime <= 0.0f )
	{
	}

	m_fTechPointViewTime = 1.5f;
}

void CX2Game::AddTechPoint( int techPoint )
{

	m_TechPoint			+= techPoint;
	m_TechPointFull		+= techPoint;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		m_fTechPointViewTime = 1.5f;
		return;
	}


	m_fTechPointViewTime = 1.5f;
}


void CX2Game::PauseNPCAI( float fTime )
{
	m_fTimePauseNPCAI = fTime;
	for( UINT i=0; i<m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pGUNPC = m_NPCUnitList[i];
		if( NULL == pGUNPC )
			continue;

		pGUNPC->SetAIEnable( false );
	}
}


// 운영자 이상 치트용으로만 사용
void CX2Game::KillAllNPC_LUA( bool bActiveOnly /* = true */ )
{
#if X2_DEV_TOOLS
	if ( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		if( NULL != GetMyUnit() )
		{
			KillAllNPC( bActiveOnly, GetMyUnit(), true );
		}
		else
		{
			KillAllNPC( bActiveOnly, NULL, true );
		}
	}
#endif
}


void CX2Game::KillAllNPC( bool bActiveOnly /* = true */, CX2GameUnit* pAttackerGameUnit_ /*= NULL*/, bool bExceptGate /*= false*/ )
{
#if X2_DEV_TOOLS
	if( IsHost() == true )
	{
		for( int i=0; i<GetNPCUnitListSize(); i++ )
		{
			CX2GUNPC* pCX2GUNPC = GetNPCUnit(i);
			if( pCX2GUNPC != NULL )
			{
				if( pCX2GUNPC->GetNPCTemplet().m_ClassType == CX2UnitManager::NCT_THING_CHECKER )
					continue;

				if( true == bExceptGate )
				{
					//if( NULL != pCX2GUNPC->GetNPCTemplet() )
					{
						if( CX2UnitManager::NCT_THING_GATE == pCX2GUNPC->GetNPCTemplet().m_ClassType )
						{
							continue;
						}
					}
				}

				if( bActiveOnly == true &&
					pCX2GUNPC->IsActiveMonster() == false )
					continue;


				pCX2GUNPC->SetGameUnitWhoAttackedMe( pAttackerGameUnit_ );
				pCX2GUNPC->SetSelfResurrection( false );

				pCX2GUNPC->SetNowHp(0.f);


			}
		}
	}
#endif
}

void CX2Game::KillNPC( const CX2UnitManager::NPC_UNIT_ID eNPCID, int nCount )
{


	if( false == IsHost() )
		return;

	if( nCount == 0 )
		return;


	THEMIDA_ENCODE_START

	int iCount = 0;
	for( int i=0; i<GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = GetNPCUnit(i);
		if( NULL == pCX2GUNPC )
			continue;

		//if( NULL == pCX2GUNPC->GetNPCTemplet() )
		//	continue;

		if( eNPCID != pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID )
			continue;


		pCX2GUNPC->ResetGameUnitWhoAttackedMe();
		pCX2GUNPC->SetNowHp( 0.f );
		++iCount;

		if( iCount >= nCount )
			break;
	}


	THEMIDA_ENCODE_END
}

void CX2Game::EnableAllNPCAI( bool bEnable )
{
	if( IsHost() == true )
	{

		for( int i=0; i<GetNPCUnitListSize(); i++ )
		{
			CX2GUNPC* pNPC = (CX2GUNPC*) GetNPCUnit(i);
			if( NULL == pNPC )
				continue;

			pNPC->SetAIEnable( bEnable );
		}
	}
}


void CX2Game::EnableAllNPCEventProcess( bool bEnable )
{
	//if( IsHost() == true )
	{
		for( int i=0; i<GetNPCUnitListSize(); i++ )
		{
			CX2GUNPC* pNPC = (CX2GUNPC*) GetNPCUnit(i);
			if( NULL == pNPC )
				continue;

			pNPC->SetEnableEventProcess( bEnable );
		}
	}
}


void CX2Game::SetAllNPCMPFull()
{
	if( NULL != g_pData->GetMyUser() && g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_DEV )
		return;


	if( IsHost() == true )
	{
		for( int i=0; i<GetNPCUnitListSize(); i++ )
		{
			CX2GUNPC* pNPC = (CX2GUNPC*) GetNPCUnit(i);
			if( NULL == pNPC )
				continue;

			pNPC->SetNowMp( pNPC->GetMaxMp() );
		}
	}
}

void CX2Game::SetEnableAllNPCConditionTable( bool bEnable )
{
	for( UINT i=0; i< m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pGUNPC = m_NPCUnitList[i];
		if( pGUNPC == NULL )
			continue;

		pGUNPC->SetEnableCondtionTableEventProcess( bEnable );
	}

}
void CX2Game::SetFreeCamera( bool bFreeCamera )
{
	m_bFreeCamera = bFreeCamera;
	if(true == bFreeCamera)
	{
		GetX2Camera()->GetCamera().SetFixedUpVec(false);
	}
	else
	{
		GetX2Camera()->GetCamera().SetFixedUpVec(true);
	}



	if( true == m_bFreeCamera )
	{
		D3DXVECTOR3 vEye	= m_pCamera->GetCamera().GetEye();
		D3DXVECTOR3 vLookAt = m_pCamera->GetCamera().GetLookAt();
		D3DXVECTOR3 vUpVec	= m_pCamera->GetCamera().GetUpVec();
		m_FPSCamera.SetViewParams( &vEye, &vLookAt, &vUpVec );

	}
#if X2_DEV_TOOLS
	else
	{
		GetX2Camera()->CalcManualCameraPosition( GetFocusUnit(), *m_FPSCamera.GetEyePt() );
	}
#endif // IN_GAME_MANUAL_CAMERA_POSITION_TEST
}

void CX2Game::SetWorldCameraEdit( bool bEnable)
{
	m_bWorldCameraEdit = bEnable;
	GetX2Camera()->SetWorldCameraEdit(bEnable);
	return;
}
// oasis907 : 김상윤 [2010.12.13] // 본섭 파싱 위해 디파인 싸지 않음
void CX2Game::PlayWorldCamera( int iWorldCameraID )
{
	GetX2Camera()->StartWorldCamera(iWorldCameraID);
	return;
}

bool CX2Game::GoStage( int iStageIndex, int iSubStageIndex /*= 0*/  )
{
	return false;
}

int CX2Game::GetNowStageIndex()
{
	if( GetGameType() != GT_DUNGEON )
		return false;

	CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(this);
	if( NULL != pDungeonGame )
	{
		CX2Dungeon* pDungeon = pDungeonGame->GetDungeon();

		if( NULL != pDungeon && NULL != pDungeon->GetNowStage() )
			return pDungeon->GetNowStageIndex();
	}

	return -1;
}


void CX2Game::UpdateSkillSlotUI(bool bUpdateEquipSkill /*= false*/)
{

	if( NULL == GetMyUnit() )
	{
		StateLog( L"upd skill my unit null" );
		return;
	}

	if( NULL == GetMyUnit()->GetUnit() )
	{
		StateLog( L"upd skill unit null" );
		return;
	}

	if( true == bUpdateEquipSkill )
	{
		//아이템에 의한 장착스킬 레벨 갱신을 위해 추가bool bUpdateEquipSkill = false
		if( NULL != g_pData && NULL != g_pData->GetUIManager() &&
			NULL != g_pData->GetUIManager()->GetUISkillTree() )
			g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillEquipedSlotUI();
	}

	//검의길 스킬아이콘 이펙트
	UpdateElswordSkillSlotEffect();

	//코드일렉트라 스킬아이콘 갱신
	UpdateEveElectraSkillSlotIcon();

	UpdateNightWatcherSkillSlotIcon();

	UpdateVeteranCommanderSkillSlotIcon();

	UpdateCodeEmpressSkillSlotIcon();

	UpdateAddNasodRulerSkillSlotIcon();
}



void CX2Game::ResetReBirthStoneNumUI()
{
	if ( m_pDLGMyScore != NULL )
	{
		CKTDGUIStatic* pStatic = (CKTDGUIStatic*)m_pDLGMyScore->GetControl( L"Static_My_Stone" );

		if( pStatic != NULL && g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
		{
			pStatic->SetShow(false);
			return;
		}

		if ( pStatic != NULL && pStatic->GetString(0) != NULL )
		{
			if( NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				wstringstream wstrstm;
				wstrstm << g_pData->GetMyUser()->GetSelectUnit()->GetResurrectionStoneNum();
				//{{ 허상형 : [2009/10/15] //	자동결제 부활석 처리
				//}} 허상형 : [2009/10/15] //	자동결제 부활석 처리코드 추가
				pStatic->GetString(0)->msg = wstrstm.str().c_str();
			}
		}
	}
}
void CX2Game::UpdateElswordSkillSlotEffect()
{
	if( NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		NULL != GetMyUnit() &&
		NULL != g_pX2Room && NULL != g_pX2Room->GetMySlot() && g_pX2Room->GetMySlot()->m_bObserver == false &&
		( CX2Unit::UT_ELSWORD == g_pData->GetMyUser()->GetSelectUnit()->GetType()
		|| CX2Unit::UT_ELESIS == g_pData->GetMyUser()->GetSelectUnit()->GetType()
		) )
	{
		const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
		if ( m_pDLGSkillSlot != NULL )
		{
			CKTDGUIStatic* pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_B_B" );
			CKTDGUIStatic* pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_R_B" );

			if( NULL == pStatic_ElSkillVigor || NULL == pStatic_ElSkillDest )
				return;

			pStatic_ElSkillVigor->SetShow(true);
			pStatic_ElSkillDest->SetShow(true);

			pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_B" );
			pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_R" );

			if( NULL == pStatic_ElSkillVigor || NULL == pStatic_ElSkillDest )
				return;

			pStatic_ElSkillVigor->SetShow(true);
			pStatic_ElSkillDest->SetShow(true);

			for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT*2; i++ )
			{
				bool bSlotB = (i > 3) ? true : false;
				int iSlotIndex = (i > 3) ? i-4 : i;
				const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB);

				if( true == bSlotB )
				{
					pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_R_B" );
					if( pStatic_ElSkillDest == NULL )
						break;

					pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_B_B" );
					if( pStatic_ElSkillVigor == NULL )
						break;
				}

				pStatic_ElSkillVigor->GetPicture(iSlotIndex)->SetShow(false);
				pStatic_ElSkillDest->GetPicture(iSlotIndex)->SetShow(false);

				//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
				if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
					&& bSlotB == true)
				{
					pStatic_ElSkillDest->SetShow(false);
					pStatic_ElSkillVigor->SetShow(false);
					break;
				}

				if(pSkillData != NULL && pSkillData->m_eID != CX2SkillTree::SI_NONE )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillData->m_eID );

					if( pSkillTemplet != NULL )
					{
						int iWayOfSwordType = pSkillTemplet->m_iWOSType;

						if( iWayOfSwordType == 2 )
						{
							if( GetMyUnit()->GetWayOfSwordState() == 2)  // CX2GUElsword_SwordMan::WSS_VIGOR
							{
								pStatic_ElSkillVigor->GetPicture(iSlotIndex)->SetShow(true);
								pStatic_ElSkillVigor->GetPicture(iSlotIndex)->SetFlicker( 0.4f, 1.0f, 0.1f );
								pStatic_ElSkillDest->GetPicture(iSlotIndex)->SetShow(false);
							}
						}
						else if (iWayOfSwordType == 1)
						{
							if( GetMyUnit()->GetWayOfSwordState() == -2)  // CX2GUElsword_SwordMan::WSS_DESTRUCTION
							{
								pStatic_ElSkillDest->GetPicture(iSlotIndex)->SetShow(true);
								pStatic_ElSkillDest->GetPicture(iSlotIndex)->SetFlicker( 0.4f, 1.0f, 0.1f );
								pStatic_ElSkillVigor->GetPicture(iSlotIndex)->SetShow(false);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		CKTDGUIStatic* pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_B_B" );
		if( NULL != pStatic_ElSkillVigor )
			pStatic_ElSkillVigor->SetShow(false);
		CKTDGUIStatic* pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_R_B" );
		if( NULL != pStatic_ElSkillDest )
			pStatic_ElSkillDest->SetShow(false);

		pStatic_ElSkillVigor = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_B" );
		if( NULL != pStatic_ElSkillVigor )
			pStatic_ElSkillVigor->SetShow(false);
		pStatic_ElSkillDest = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_EL_SKILL_R" );
		if( NULL != pStatic_ElSkillDest )
			pStatic_ElSkillDest->SetShow(false);
	}
	return;
}
void CX2Game::UpdateEveElectraSkillSlotIcon()
{
	const CX2Unit::UNIT_CLASS eUnitClass = g_pData->GetMyUser()->GetSelectUnit()->GetClass();

	if (
		GetMyUnit() != NULL &&
		GetMyUnit()->GetUnit() != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = GetMyUnit()->GetUnit()->GetUnitData().m_UserSkillTree;
		//B슬롯도 같이 하기 위해서 EQUIPPED_SKILL_SLOT_COUNT*2
		for(int i=0; i<EQUIPPED_SKILL_SLOT_COUNT * 2; ++i)
		{
			bool bSlotB = (i > 3) ? true : false;
			int iSlotIndex = (i > 3) ? i-4 : i;

			//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
			if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
				&& bSlotB == true)
			{
				break;
			}

			const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB );
			if( pSkillData != NULL && pSkillData->m_eID != CX2SkillTree::SI_NONE )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillData->m_eID );
				if(pSkillTemplet != NULL)
				{
					wstring wstrTextureName			= pSkillTemplet->m_wstrIconName;
					wstring wstrTexturePieceName	= pSkillTemplet->m_wstrIconPieceName;
					if( pSkillData->m_eID == CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL )
					{
						CX2GUEve* pEve = static_cast< CX2GUEve* >( GetMyUnit() );
						if( pEve->GetEnableElectraSystem() == true && pEve->GetElectraFormationType() == CX2GUEve::PFT_AMPLIFIER  )
						{
							wstrTextureName = L"DLG_SKILL_EveActive_03.tga";
							wstrTexturePieceName = L"SI_A_EEL_SPECTRO_EL_CRYSTAL_01";
						}
						else if( pEve->GetEnableElectraSystem() == true && pEve->GetElectraFormationType() == CX2GUEve::PFT_SPECTRUM  )
						{
							wstrTextureName = L"DLG_SKILL_EveActive_03.tga";
							wstrTexturePieceName = L"SI_A_EEL_SPECTRO_EL_CRYSTAL_02";
						}
						else if ( pEve->GetEnableElectraSystem() == true && pEve->GetElectraFormationType() == CX2GUEve::PFT_INDUCTION )
						{
							wstrTextureName = L"DLG_SKILL_EveActive_04.tga";
							wstrTexturePieceName = L"SI_A_EBS_SPECTRO_EL_CRYSTAL_INDUCTION";
						}
						else if ( pEve->GetEnableElectraSystem() == true && pEve->GetElectraFormationType() == CX2GUEve::PFT_FUSION )
						{
							wstrTextureName = L"DLG_SKILL_EveActive_04.tga";
							wstrTexturePieceName = L"SI_A_EBS_SPECTRO_EL_CRYSTAL_FUSION";
						}

						CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
						pSlot->SetItemTex(wstrTextureName.c_str(), wstrTexturePieceName.c_str() );
					}
					else if( pSkillData->m_eID == CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS )
					{
						CX2GUEve* pEve = static_cast< CX2GUEve* >( GetMyUnit() );
						if( pEve->GetThousandStarCount() > 0 &&  pEve->GetThousandStarEntireFired() == false  )
						{
							wstrTextureName = L"DLG_SKILL_EveActive_03.tga";
							wstrTexturePieceName = L"SI_SA_EEL_THOUSANDS_OF_STARS_FIRE";
						}

						CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
						pSlot->SetItemTex(wstrTextureName.c_str(), wstrTexturePieceName.c_str() );
					}
				}	// if
			}	//if
		}//for
	}
}


void CX2Game::UpdateNightWatcherSkillSlotIcon()
{
	if(	g_pData->GetMyUser()->GetSelectUnit()->GetClass() == CX2Unit::UC_LIRE_NIGHT_WATCHER &&
		GetMyUnit() != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = GetMyUnit()->GetUnit()->GetUnitData().m_UserSkillTree;
		//B슬롯도 같이 하기 위해서 EQUIPPED_SKILL_SLOT_COUNT*2
		for(int i=0; i<EQUIPPED_SKILL_SLOT_COUNT * 2; ++i)
		{
			bool bSlotB = (i > 3) ? true : false;
			int iSlotIndex = (i > 3) ? i-4 : i;

			//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
			if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
				&& bSlotB == true)
			{
				break;
			}

			const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB );
			if( pSkillData != NULL && pSkillData->m_eID != CX2SkillTree::SI_NONE )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillData->m_eID );
				if(pSkillTemplet != NULL)
				{
					wstring wstrTextureName			= pSkillTemplet->m_wstrIconName;
					wstring wstrTexturePieceName	= pSkillTemplet->m_wstrIconPieceName;
					if( pSkillData->m_eID == CX2SkillTree::SI_A_LNW_DELAYED_FIRING )
					{
						CX2GULire_ElvenRanger* pRena = static_cast< CX2GULire_ElvenRanger* >( GetMyUnit() );

						if( pRena->GetActiveStartOfDelayedFiring() == true )			/// 전체 폭발 모드 아이콘
						{
							wstrTextureName = L"DLG_SKILL_LenaActive_03.tga";
							wstrTexturePieceName = L"SI_A_LNW_DELAYED_FIRE_ACTIVE";
						}
						else if( pRena->GetActiveStartOfDelayedFiring() == false )		/// 지연의 신호탄 모드 아이콘
						{
							wstrTextureName = L"DLG_SKILL_LenaActive_03.tga";
							wstrTexturePieceName = L"SI_A_LNW_DELAYED_FIRE";
						}

						CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
						pSlot->SetItemTex(wstrTextureName.c_str(), wstrTexturePieceName.c_str() );
					}
				}	// if
			}	//if
		}//for
	}
}

void CX2Game::UpdateVeteranCommanderSkillSlotIcon()
{
	if(	g_pData->GetMyUser()->GetSelectUnit()->GetClass() == CX2Unit::UC_RAVEN_VETERAN_COMMANDER &&
		GetMyUnit() != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = GetMyUnit()->GetUnit()->GetUnitData().m_UserSkillTree;
		//B슬롯도 같이 하기 위해서 EQUIPPED_SKILL_SLOT_COUNT*2
		for(int i=0; i<EQUIPPED_SKILL_SLOT_COUNT * 2; ++i)
		{
			bool bSlotB = (i > 3) ? true : false;
			int iSlotIndex = (i > 3) ? i-4 : i;

			//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
			if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
				&& bSlotB == true)
			{
				break;
			}

			const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB );
			if( pSkillData != NULL && pSkillData->m_eID != CX2SkillTree::SI_NONE )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillData->m_eID );
				if(pSkillTemplet != NULL)
				{
					wstring wstrTextureName			= pSkillTemplet->m_wstrIconName;
					wstring wstrTexturePieceName	= pSkillTemplet->m_wstrIconPieceName;
					if( pSkillData->m_eID == CX2SkillTree::SI_A_RF_SHADOW_STEP )
					{
						CX2GURaven* pRena = static_cast< CX2GURaven* >( GetMyUnit() );

						if( pRena->GetIsShadowStepIcon() == true )				/// 섀도우 스텝 아이콘으로 변경
						{
							wstrTextureName = L"DLG_SKILL_RavenActive_01.tga";
							wstrTexturePieceName = L"SI_A_RF_SHADOW_STEP";
						}
						else if( pRena->GetIsShadowStepIcon() == false )		/// 섀도우 백슬라이드 아이콘으로 변경
						{
							wstrTextureName = L"DLG_SKILL_Passive_04.tga";
							wstrTexturePieceName = L"SI_P_RVC_SHADOW_BACK_SLIDE";
						}

						CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
						pSlot->SetItemTex(wstrTextureName.c_str(), wstrTexturePieceName.c_str() );
					}
				}	// if
			}	//if
		}//for
	}
}

void CX2Game::UpdateCodeEmpressSkillSlotIcon()
{
	if(	g_pData->GetMyUser()->GetSelectUnit()->GetClass() == CX2Unit::UC_EVE_CODE_EMPRESS &&
		GetMyUnit() != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = GetMyUnit()->GetUnit()->GetUnitData().m_UserSkillTree;
		//B슬롯도 같이 하기 위해서 EQUIPPED_SKILL_SLOT_COUNT*2
		for(int i=0; i<EQUIPPED_SKILL_SLOT_COUNT * 2; ++i)
		{
			bool bSlotB = (i > 3) ? true : false;
			int iSlotIndex = (i > 3) ? i-4 : i;

			//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
			if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
				&& bSlotB == true)
			{
				break;
			}

			const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB );
			if( pSkillData != NULL && pSkillData->m_eID != CX2SkillTree::SI_NONE )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillData->m_eID );
				if(pSkillTemplet != NULL)
				{
					wstring wstrTextureName			= pSkillTemplet->m_wstrIconName;
					wstring wstrTexturePieceName	= pSkillTemplet->m_wstrIconPieceName;
					if( pSkillData->m_eID == CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION )
					{
						CX2GUEve* pEve = static_cast< CX2GUEve* >( GetMyUnit() );

						if( pEve->GetActiveLinkOverChargeIllusion() == true )				/// 엠프레스 궁극기 - 링크 오버차지 썬더볼트 활성화
						{
							wstrTextureName = L"DLG_SKILL_Hyper_Active_02.tga";
							wstrTexturePieceName = L"SI_HA_EEP_LINK_OVERCHARGE_ILLUSION_01";
						}
						else if( pEve->GetActiveLinkOverChargeIllusion() == false )			/// 엠프레스 궁극기 - 링크 오버차지 썬더볼트 비활성화
						{
							wstrTextureName = L"DLG_SKILL_Hyper_Active_01.tga";
							wstrTexturePieceName = L"SI_HA_EEP_LINK_OVERCHARGE_ILLUSION";
						}

						CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
						pSlot->SetItemTex(wstrTextureName.c_str(), wstrTexturePieceName.c_str() );
					}
				}	// if
			}	//if
		}//for
	}
}

void CX2Game::UpdateAddNasodRulerSkillSlotIcon()
{
	if(	NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() &&
		CX2Unit::UT_ADD == g_pData->GetMyUser()->GetSelectUnit()->GetType() &&
		NULL != g_pData->GetSkillTree() )
	{
		CX2GUAdd* pAdd = static_cast< CX2GUAdd* >( GetMyUnit() );

		if ( NULL == pAdd )
			return;

		const CX2UserSkillTree& refUserSkillTree = GetMyUnit()->GetUnit()->GetUnitData().m_UserSkillTree;

		//B슬롯도 같이 하기 위해서 EQUIPPED_SKILL_SLOT_COUNT*2
		for( int i = 0; i<EQUIPPED_SKILL_SLOT_COUNT * 2; ++i )
		{
			bool bSlotB = (i > 3) ? true : false;
			int iSlotIndex = (i > 3) ? i-4 : i;

			//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
			if( false == refUserSkillTree.GetEnabledSkillSlotB() && bSlotB == true )
				break;

			const CX2UserSkillTree::SkillSlotData* pSkillData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB );

			if( pSkillData == NULL || pSkillData->m_eID == CX2SkillTree::SI_NONE )
				continue;

			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillData->m_eID );

			if( pSkillTemplet == NULL)
				continue;

			const wchar_t* pwstrTextureName			= pSkillTemplet->m_wstrIconName.c_str();
			const wchar_t* pwstrTexturePieceName	= pSkillTemplet->m_wstrIconPieceName.c_str();

			switch( pSkillData->m_eID )
			{
			case CX2SkillTree::SI_SA_APT_STASIS_FIELD:
				{
					if( true == pAdd->GetIsActiveStasisfield() )		/// 스테이시스 필드 활성화
					{
						pwstrTextureName = L"DLG_SKILL_ADD_Active_01.tga";
						pwstrTexturePieceName = L"SI_SA_APT_STASIS_FIELD_01";
					}

					CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
					if( NULL != pSlot )
						pSlot->SetItemTex(pwstrTextureName, pwstrTexturePieceName );
				} break;
			case CX2SkillTree::SI_A_AN_PHASE_SHIFT:
				{
					if( true == pAdd->GetIsActivePhaseShift() )
					{
						pwstrTextureName = L"DLG_SKILL_ADD_Active_01.tga";
						pwstrTexturePieceName = L"SI_A_AN_PHASE_SHIFT_01";
					}
					else
					{
						/// 환상 추적자 활성 여부
						if ( true == pAdd->GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PHASE_SHIFT ) )
						{
							pwstrTextureName = L"DLG_SKILL_Passive_07.tga";
							pwstrTexturePieceName = L"SI_P_ALP_FANTASY_TRACER";
						}
					}

					CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
					if( NULL != pSlot )
						pSlot->SetItemTex(pwstrTextureName, pwstrTexturePieceName );
				} break;
			case CX2SkillTree::SI_A_AN_PULSE_BULLET:
				{
					if ( true == pAdd->GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PULSE_BULLET ) )
					{
						pwstrTextureName = L"DLG_SKILL_Passive_07.tga";
						pwstrTexturePieceName = L"SI_P_ALP_FANTASY_TRACER";
					}

					CKTDGUISlot* pSlot = g_pData->GetUIManager()->GetUISkillTree()->GetEquippedSlot(i);
					if( NULL != pSlot )
						pSlot->SetItemTex(pwstrTextureName, pwstrTexturePieceName );
				} break;
			default:
				break;
			}
		}
	}
}

void CX2Game::UpdateSkillCoolTime( float fElapsedTime )
{
	if( g_pData != NULL &&
		g_pData->GetMyUser() != NULL &&
		g_pData->GetMyUser()->GetSelectUnit() != NULL &&
		GetMyUnit() != NULL )
	{
		const CX2UserSkillTree& refUserSkillTree = g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree;
		if ( m_pDLGSkillSlot != NULL )
		{
			CKTDGUIStatic* pStaticCoolTime = (CKTDGUIStatic*)m_pDLGSkillSlot->GetControl( L"Static_Skill_Cool_Time" );
			CKTDGUIStatic* pStatic_Skill_Cool_Time_Fade = (CKTDGUIStatic*)m_pDLGSkillSlot->GetControl( L"Static_Skill_Cool_Time_Fade" );
			CKTDGUIStatic* pStaticSlot_A = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_SKILL_SLOT_A" );
			CKTDGUIStatic* pStaticSlot_B = (CKTDGUIStatic*) m_pDLGSkillSlot->GetControl( L"Static_SKILL_SLOT_B" );
			for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT; i++ )
			{
				pStaticSlot_A->GetPicture(i)->SetColor( D3DXCOLOR(1.f,1.f,1.f,0.3f));
				pStaticSlot_B->GetPicture(i)->SetColor( D3DXCOLOR(1.f,1.f,1.f,0.3f));
			}

			//슬롯 B 활성화 여부
			bool bEnableSlotB = false;
			if( NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit() )
			{
				if( true == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB() )
					bEnableSlotB = true;
			}

			//스킬슬롯 UI 타입 체크
			bool bIsSlotUITypeA = false;
			bool bIsSkillSlotSwap = false;
			if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )

			{
				bIsSlotUITypeA = g_pData->GetUIManager()->GetUISkillTree()->GetIsSkillUiTypeA();
				bIsSkillSlotSwap = g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap();
			}



			if ( pStaticCoolTime != NULL && pStatic_Skill_Cool_Time_Fade != NULL )
			{
				for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT*2; i++ )
				{
					bool bSlotB = (i > 3) ? true : false;
					int iSlotIndex = (i > 3) ? i-4 : i;

					if( true == bSlotB )
					{
						//슬롯 B가 활성화 되지 않으면 슬롯 A만 확인하도록 break;
						if( false == bEnableSlotB )
						{
							break;
						}

						pStatic_Skill_Cool_Time_Fade = (CKTDGUIStatic*)m_pDLGSkillSlot->GetControl( L"Static_Skill_Cool_Time_Fade_B" );
						if( NULL == pStatic_Skill_Cool_Time_Fade )
							break;

						pStaticCoolTime = (CKTDGUIStatic*)m_pDLGSkillSlot->GetControl( L"Static_Skill_Cool_Time_B" );
						if( NULL == pStaticCoolTime )
							break;
					}
					const CX2UserSkillTree::SkillSlotData* pSlotData = refUserSkillTree.GetSkillSlot(iSlotIndex, bSlotB);
					if(pSlotData != NULL)
					{
						if( NULL != pStaticSlot_A && NULL != pStaticSlot_B )
						{
							if( CX2SkillTree::SI_NONE != pSlotData->m_eID )
							{// 0~3 : B슬롯, 4~8 : A슬롯
								if( true == bSlotB )
								{
									pStaticSlot_B->GetPicture(iSlotIndex)->SetColor( D3DXCOLOR(1.f,1.f,1.f,1.f));
								}
								else
								{
									pStaticSlot_A->GetPicture(iSlotIndex)->SetColor( D3DXCOLOR(1.f,1.f,1.f,1.f));
								}
							}
						}
						if(pSlotData->m_eID == CX2SkillTree::SI_NONE )
						{
							//장착된 스킬이 없다면 활성상태로 변경
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = false;

							pStaticCoolTime->SetString( iSlotIndex, L"" );

							continue;
						}
						CX2GageData* pMyGageData = GetMyUnit()->GetGageData();

						// 위상변화 MP변수 값이 설정되었다면
						// 현재 슬롯은 위상변화 슬롯이며,  위상변화 위치 이동이 가능한 상태
						bool bShowCoolTime = true;
						float fRequiredMpForPhaseShift = 0.f;
						if( pSlotData->m_eID == CX2SkillTree::SI_A_AN_PHASE_SHIFT )
						{
							CX2GUAdd* pAdd = static_cast< CX2GUAdd* >( GetMyUnit() );
							if( NULL != pAdd )
							{
								if( pAdd->GetIsActivePhaseShift() )
								{
									fRequiredMpForPhaseShift = pAdd->GetPhaseShiftMPConsume();
									bShowCoolTime = false;
								}
							}
						}

						// 오현빈 // MP 소모량에 대한 예외처리를 위해  GetActualMPConsume로 MP 소모량 얻어오기
						float fRequiredMPConsumption = GetMyUnit()->GetActualMPConsume( pSlotData->m_eID, pSlotData->m_iSkillLevel + pSlotData->m_iSkillLevelPlus );
						if( CX2Unit::UT_ELSWORD == GetMyUnit()->GetUnit()->GetType()
							)
						{
							int iWayOfSwordState = GetMyUnit()->GetWayOfSwordState();

							if( iWayOfSwordState == 2 ) // CX2GUElsword_SwordMan::WSS_VIGOR
							{
								const CX2SkillTree::SkillTemplet* pSkillTempletUsing = GetMyUnit()->GetEquippedActiveSkillTemplet( pSlotData->m_eID );
								if( pSkillTempletUsing != NULL )
								{
									const int iWayOfSwordType = pSkillTempletUsing->m_iWOSType;

									if( iWayOfSwordType == 2 )
									{
										fRequiredMPConsumption *= 0.7f;
									}
								}
							}
						}

						if( pSlotData->m_eID == CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION )		/// 링크 오버차지 일루젼 npc 페르디난도 피니쉬 공격
						{
							CX2GUEve* pEve = static_cast< CX2GUEve* >( GetMyUnit() );

							if( true == pEve->GetActiveLinkOverChargeIllusion() )
							{
								fRequiredMPConsumption = 0.f;
							}
						}

						if( pSlotData->m_eID == CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL )
						{
							CX2GUEve* pEve = static_cast< CX2GUEve* >( GetMyUnit() );
							if( pEve->GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO13 ) == true )
							{
								fRequiredMPConsumption -= 10.f;
								if( fRequiredMPConsumption < 0.f )
									fRequiredMPConsumption = 0.f;
							}
							if( pEve->GetEnableElectraSystem() == true )
							{
								fRequiredMPConsumption = 0.f;
							}
						}
						else if( pSlotData->m_eID == CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS )
						{
							CX2GUEve* pEve = static_cast< CX2GUEve* >( GetMyUnit() );
							if( pEve->GetThousandStarCount() > 0 &&  pEve->GetThousandStarEntireFired() == false  )
							{
								fRequiredMPConsumption = 0.f;
							}

						}

						if( pSlotData->m_eID == CX2SkillTree::SI_A_LNW_DELAYED_FIRING )		/// 지연의 신호탄 전체 폭발
						{
							CX2GULire_ElvenRanger* pRena = static_cast< CX2GULire_ElvenRanger* >( GetMyUnit() );

							if( true == pRena->GetActiveStartOfDelayedFiring() )
							{
								fRequiredMPConsumption = 0.f;
							}
						}

						if( pSlotData->m_eID == CX2SkillTree::SI_A_RF_SHADOW_STEP )						/// 섀도우 스텝 콤보
						{
							CX2GURaven* pRaven = static_cast< CX2GURaven* >( GetMyUnit() );

							if( NULL != pRaven && true == pRaven->GetActiveShadowStep() )
							{
								fRequiredMPConsumption = 10.f;
							}
						}

						/// 스테이시스 필드 동작 중
						else if( pSlotData->m_eID == CX2SkillTree::SI_SA_APT_STASIS_FIELD )
						{
							CX2GUAdd* pAdd = static_cast< CX2GUAdd* >( GetMyUnit() );

							if( NULL != pAdd && true == pAdd->GetIsActiveStasisfield() )
							{
								fRequiredMPConsumption = 0.f;
								bShowCoolTime = false;
							}
						}

						/// 위상 변화 동작 중
						else if( 0.f < fRequiredMpForPhaseShift )
						{
							fRequiredMPConsumption = fRequiredMpForPhaseShift;
						}

						/// 파일런 연속 소환 가능한 조건이라면, 쿨타임 설정 않함
						else if ( CX2Unit::UT_ADD == GetMyUnit()->GetUnit()->GetType() &&
								  pSlotData->m_eID == CX2SkillTree::SI_A_APT_PYLON_DYNAMO &&
								  true == static_cast< CX2GUAdd* >( GetMyUnit() )->GetPossibleNonstopSummonPylon() )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = false;

							bShowCoolTime = false;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}

						/// 환상 추적자 아이콘으로 변경시엔, 비활성 처리 해제
						else if ( CX2Unit::UT_ADD == GetMyUnit()->GetUnit()->GetType() &&
								  true == static_cast< CX2GUAdd* >( GetMyUnit() )->GetIsFantasyTracerTargetSkillID( pSlotData->m_eID ) )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = false;

							bShowCoolTime = false;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}

						/// 퀵실버 액샐 동작중엔, 계속 활성 처리
						else if ( CX2Unit::UT_ADD == GetMyUnit()->GetUnit()->GetType() &&
								  pSlotData->m_eID == CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL &&
								  CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == static_cast< CX2GUAdd* >( GetMyUnit() )->GetFutureStateSkillID() )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = false;

							bShowCoolTime = false;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}

						const int iElEssence = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );
						const CX2SkillTree::SkillTemplet* pSkillTemplet = refUserSkillTree.GetUserSkillTemplet( pSlotData->m_eID );

						if( true == GetMyUnit()->GetEntangled() ||
							GetMyUnit()->GetNoSkillTime() > 0.f )	// 스킬을 사용할 수 없는 silence 상태일 때
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
						else if(pSlotData->m_fCoolTimeLeft > 0.0f
								&& true == bShowCoolTime
								)
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							wstringstream wstrstm;
							wstrstm << (int)( ceil(pSlotData->m_fCoolTimeLeft) );
							pStaticCoolTime->SetString( iSlotIndex, wstrstm.str().c_str() );
						}
						else if ( NULL != GetMyUnit() && fRequiredMPConsumption > GetMyUnit()->GetNowMp() )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
				//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
						// 캐릭터가 청인데, 광폭화모드가 아닌 상태에서 캐논볼 보유량이 부족 하면
						else if ( CX2Unit::UT_CHUNG == GetMyUnit()->GetUnit()->GetType()
								  && ( false == GetMyUnit()->IsFullHyperMode()
								        && static_cast<CX2GUChung*>(GetMyUnit())->GetCannonBallCount() < pSlotData->m_usCBConsumption )
                                 )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							// 캐논볼이 부족하도고 표시해줘야 하지 않을까?
							pStaticCoolTime->SetString( iSlotIndex, L"" );
						} // if
				//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG


						/// 아라일 때, 기력 소비량 검사
						else if ( CX2Unit::UT_ARA == GetMyUnit()->GetUnit()->GetType() &&
							static_cast< CX2GUAra* >( GetMyUnit() )->GetForcePower() <
							static_cast< CX2GUAra* >( GetMyUnit() )->GetActualFPConsume( pSlotData->m_eID, pSlotData->m_usFPConsumption ) )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
						else if ( pSkillTemplet != NULL && pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL &&
							iElEssence <= 0
							&& false == g_pData->GetMyUser()->GetSelectUnit()->IsInfinityElEssence()
							)
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
						/// DP 검사
						else if ( CX2Unit::UT_ADD == GetMyUnit()->GetUnit()->GetType() &&
								  static_cast< CX2GUAdd* >( GetMyUnit() )->GetDPValue() <
								  static_cast< CX2GUAdd* >( GetMyUnit() )->GetActualDPConsume( pSlotData )  )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
						/// 변이 포인트 검사
						else if ( CX2Unit::UT_ADD == GetMyUnit()->GetUnit()->GetType() &&
								  static_cast< CX2GUAdd* >( GetMyUnit() )->GetMutationCount() <
								  static_cast< CX2GUAdd* >( GetMyUnit() )->GetMutationPointConsume( pSlotData ) )
						{
							if (pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = true;

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
						else
						{
							if ( pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex) != NULL )
							{
								if(true == pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->GetShow() && m_bSkillSlotJustChanged == false	)
								{
									bool bIsShowEffect = true;

									if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
										bIsShowEffect = false;


									if( false == bIsSlotUITypeA ) //UI 타입 B일때는 해당 슬롯이 선택되어져 있을 때만 이펙트 노출
									{
										if( true == bIsSkillSlotSwap && false == bSlotB ) //현재 B슬롯이 선택 되어 있고, B슬롯에 대한 체크가 아니라면
										{
											bIsShowEffect = false;
										}
										if( false == bIsSkillSlotSwap && true == bSlotB ) //현재 A슬롯이 선택 되어 있고, A슬롯에 대한 체크가 아니라면
										{
											bIsShowEffect = false;
										}
									}

									if( true == bIsShowEffect )
									{
										if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() &&
											NULL != g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot() &&
											true == g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot()->GetShow() )
										{
											// 스킬을 쓸 수 있게 되는 순간. 이펙트를 표시해 준다.
											D3DXVECTOR2 EffectPos = pStatic_Skill_Cool_Time_Fade->GetDialog()->GetPos() + pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->GetPos() + pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->GetSize()/2;

											if( false == bIsSlotUITypeA && true == bSlotB )
												EffectPos.y+=51.f;

											CKTDGParticleSystem::CParticleEventSequence* pParticle =
												g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"OutBox_Skill_Recharge01", EffectPos.x, EffectPos.y, 0 );
											if( pParticle != NULL )
												pParticle->SetOverUI( true );

											CKTDGParticleSystem::CParticleEventSequence* pParticle2 =
												g_pData->GetUIMajorParticle()->CreateSequence( NULL, L"OutBox_Skill_Recharge02", EffectPos.x, EffectPos.y, 0 );
											if( pParticle2 != NULL )
											{
												pParticle2->SetOverUI( true );
												pParticle2->SetLatency( 0.1f );
											}
										}
									}
									g_pKTDXApp->GetDeviceManager()->PlaySound( L"Skill_Refresh.ogg", false, false );

								}
								pStatic_Skill_Cool_Time_Fade->GetPicture(iSlotIndex)->bShow = false;
							}

							pStaticCoolTime->SetString( iSlotIndex, L"" );
						}
					}
				}
			}
		}
	}
	m_bSkillSlotJustChanged = false;


}
void CX2Game::InitExpandSkillCoolTime()
{
	if ( m_pDLGSkillSlot != NULL )
	{
		CKTDGUIStatic* pStaticCoolTime = (CKTDGUIStatic*)m_pDLGSkillSlot->GetControl( L"Static_Skill_Cool_Time" );
		CKTDGUIStatic* pStaticCoolTime_B = (CKTDGUIStatic*)m_pDLGSkillSlot->GetControl( L"Static_Skill_Cool_Time_B" );

		for ( int i = 0; i < EQUIPPED_SKILL_SLOT_COUNT; i++ )
		{
			if( NULL != pStaticCoolTime )
			{
				pStaticCoolTime->SetString( i , L"");
			}
			if( NULL != pStaticCoolTime_B )
			{
				pStaticCoolTime->SetString( i , L"");
			}
		}
	}
}
void CX2Game::ResurrectionFrameMove( double fTime, float fElapsedTime )
{
	CKTDGUIStatic* pStaticRestore_Continue_End = static_cast<CKTDGUIStatic*>( m_pDLGContinue->GetControl( L"Restore_Continue_End" ) );
	if ( pStaticRestore_Continue_End != NULL )
		pStaticRestore_Continue_End->SetShow( false );

	// 자신이 죽었고 부활 카운트가 시작되지 않았으면
	if ( m_bResurrectionCountStart == false && GetMyUnit() != NULL && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE
		)
	{
		//스트링 설정
		if( NULL != g_pMain && NULL != g_pMain->GetKeyPad() )
		{
			//평타 공격(Z) 기준으로 텍스트 수정  --부활하려면 #CFFFF00'@1'#CX키를 눌러주세요.
			pStaticRestore_Continue_End->GetString(0)->msg = GET_REPLACED_STRING( (STR_ID_17325, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_ATTACK_FAST ) ) );

			//부활석 소지 여부에 따른 텍스트 수정
			if ( GetMyUnit()->GetUnit()->GetResurrectionStoneNum() > 0
				)
				pStaticRestore_Continue_End->GetString(1)->msg = GET_STRING(STR_ID_17326); //-- (부활석 1개가 소모됩니다)
			else
				pStaticRestore_Continue_End->GetString(1)->msg = GET_STRING(STR_ID_17327); //-- (150캐시가 소모됩니다)

			if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
				pStaticRestore_Continue_End->GetString(2)->msg = GET_REPLACED_STRING( (STR_ID_17329, "L", g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_ATTACK_STRONG ) )); //--#CFFFF00'@1'키로 마을에서 재시작
		}
		//== 카운트 설정

		if( g_pMain->GetNowStateID() == CX2Main::XS_DUNGEON_GAME  ) //필드에서는 카운터 하지 않도록 수정
		{
			NoticeContinue();	// m_fResurrectionRemainTime을 10으로 셋팅 하고 SetResurrectionTimeOut을 false로 만든다.
		}
		else if ( g_pMain->GetNowStateID() == CX2Main::XS_BATTLE_FIELD  )
		{
			m_fAutoResurrectionTime = 300.f;
		}
		m_bResurrectionCountStart = true;	// 시작
	}
	if( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree()
		&& NULL != g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot()
		&& NULL != CX2RidingPetManager::GetInstance() && NULL != CX2RidingPetManager::GetInstance()->GetSkillSlot()
		)
	{
		if( GetMyUnit() != NULL && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
		{
			CX2RidingPetManager::GetInstance()->GetSkillSlot()->SetShowRidingSkillSlot(false);
			g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot()->SetEnable(false);
			g_pData->GetUIManager()->GetUISkillTree()->ShowSkillDesc(false);
		}
		else
		{
			if( true == g_pData->GetUIManager()->GetShowRidingPetSkillslot() )
			{
				CX2RidingPetManager::GetInstance()->GetSkillSlot()->SetShowRidingSkillSlot(true);
				g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot()->SetEnable(false);
			}
			else
			{
				CX2RidingPetManager::GetInstance()->GetSkillSlot()->SetShowRidingSkillSlot(false);
				g_pData->GetUIManager()->GetUISkillTree()->GetDLGGameSkillSLot()->SetEnable(true);
			}
		}
	}

	// 카운트 시간이 남아있따면 카운트 처리.
	if ( m_fResurrectionRemainTime > 0.0f )
	{
		if ( (int)m_fResurrectionRemainTime > (int)(m_fResurrectionRemainTime - fElapsedTime) )
		{
			if ( NULL != m_pSoundContinue )
				m_pSoundContinue->Play( false, false );
		}

		// 부활 카운트가 스탑되지 않았으면 부활하는데 남은 시간을
		if ( false == m_bStopRebirthCount )
		{
			m_fResurrectionRemainTime -= fElapsedTime;

			if ( m_fResurrectionRemainTime <= 0.0f )
				m_fResurrectionRemainTime = 0.0f;
		}
	}

	//FieldFix: 이건 계속 수행 되겠군...
	m_fTimeEnableKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ -= fElapsedTime;
	if ( m_fTimeEnableKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ <= 0.0f )
	{
		m_fTimeEnableKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ = 0.0f;
		m_bEnableSendKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ = true;
	}

	if ( m_pDLGCheckResurrectByUseCash != NULL )
	{
		if( g_pKTDXApp->GetDGManager()->GetDialogManager()->CheckDlg( m_pDLGCheckResurrectByUseCash ) == true )
		{
			swprintf( m_StopContinueAlarmText, 256,
				GET_REPLACED_STRING( ( STR_ID_139, "i", static_cast<int>( m_fTimeWaitStopContinue ) ) ),
				static_cast<int>( m_fTimeWaitStopContinue ) );

			CKTDGUIStatic* pStatic = static_cast<CKTDGUIStatic*>( m_pDLGCheckResurrectByUseCash->GetControl( L"StaticMsgBoxBG" ) );
			if( pStatic != NULL )
				pStatic->GetString(0)->msg = m_StopContinueAlarmText;
		}

		//FieldFix: 이것의 의도는...?
		m_fTimeWaitStopContinue -= fElapsedTime;

		if ( m_fTimeWaitStopContinue <= 0.0f )
		{
			m_fTimeWaitStopContinue = 0.0f;

			SAFE_DELETE_DIALOG( m_pDLGCheckResurrectByUseCash );
			Handler_EGS_STOP_DUNGEON_CONTINUE_TIME_REQ( false );
		}
	}


	if ( GetMyUnit() != NULL && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE &&
		m_fResurrectionRemainTime <= 0.0f && GetMyUnit()->GetResurrectionTimeOut() == false )
	{
		GetMyUnit()->SetResurrectionTimeOut(true);
	}

	ResurrectionKeyProcess();

	CKTDGUIStatic* pStaticRestore_Continue = static_cast<CKTDGUIStatic*>( m_pDLGContinue->GetControl( L"Restore_Continue" ) );
	if ( pStaticRestore_Continue != NULL )
		pStaticRestore_Continue->SetShow( false );

	CKTDGUIStatic* pStaticRestore_Continue_For_Cash = static_cast<CKTDGUIStatic*>( m_pDLGContinue->GetControl( L"Restore_Continue_Cash" ) );
	if ( pStaticRestore_Continue_For_Cash != NULL )
		pStaticRestore_Continue_For_Cash->SetShow( false );

	CKTDGUIStatic* pStaticCount = static_cast<CKTDGUIStatic*>( m_pDLGContinue->GetControl( L"Restore_Continue_NUM" ) );
	if ( pStaticCount != NULL )
		pStaticCount->SetShow( false );

	if ( m_fResurrectionRemainTime > 0.0f && GetMyUnit() != NULL )
	{
		WCHAR timeBuf[200] = {0,};
		int coundDown = static_cast<int>( m_fResurrectionRemainTime ) - 1;
		if ( coundDown < 0 )
			coundDown = 0;
		else if( coundDown > 8) //최대 8초로 제한
			coundDown = 8;
		m_pDLGContinue->SetShowEnable( true, true);

		if ( pStaticCount != NULL )
		{
			pStaticCount->SetShow( true );

			for ( int i = 0; i < 9; i++ )
			{
				if ( pStaticCount->GetPicture( i ) != NULL )
				{
					if ( i == coundDown )
						pStaticCount->GetPicture( i )->SetShow( true );
					else
						pStaticCount->GetPicture( i )->SetShow( false );
				}
			}
		}

		//{{ 허상형 : [2009/10/20] //	자동결제 부활석 체크 추가
		if ( GetMyUnit()->GetUnit()->GetResurrectionStoneNum() > 0 )
		//}} 허상형 : [2009/10/20] //	자동결제 부활석 체크 추가
		{
			if ( pStaticRestore_Continue != NULL )
			{
				pStaticRestore_Continue->SetShow( true );
				if ( pStaticRestore_Continue->GetString(0) != NULL )
				{
					wstringstream wstrstm;

					wstrstm << L"x" << GetMyUnit()->GetUnit()->GetResurrectionStoneNum();

					//{{ 허상형 : [2009/10/15] //	자동결제 부활석 처리
					//}} 허상형 : [2009/10/15] //	자동결제 부활석 처리코드 추가

					pStaticRestore_Continue->GetString(0)->msg = wstrstm.str().c_str();
				}
			}
		}
		else
		{
//부활석이 없을 때
			if ( pStaticRestore_Continue_For_Cash != NULL )
			{
				pStaticRestore_Continue_For_Cash->SetShow( true );
				wstringstream wstrstm;
				if ( pStaticRestore_Continue_For_Cash->GetString(0) != NULL )
				{

					wstrstm << L"x" << GetMyUnit()->GetUnit()->GetResurrectionStoneNum();
					//{{ 허상형 : [2009/10/15] //	자동결제 부활석 처리
					//}} 허상형 : [2009/10/15] //	자동결제 부활석 처리코드 추가
					pStaticRestore_Continue_For_Cash->GetString(0)->msg = wstrstm.str().c_str();
				}

				if ( pStaticRestore_Continue_For_Cash->GetString(1) != NULL )
				{
					wstrstm.str( L"" );
					wstrstm << g_pData->GetMyUser()->GetGlobalCashForRescurrection();
					pStaticRestore_Continue_For_Cash->GetString(1)->msg = wstrstm.str().c_str();
				}
			}
		}

	}//카운트 지속시간이 종료되었을 때
	else if ( GetMyUnit() != NULL && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE &&
		m_bGameEnd == false )
	{
		m_pDLGContinue->SetShow( true );
		m_pDLGContinue->SetEnable( true );
		if(  NULL != pStaticRestore_Continue_End )
		{
			if(CX2Main::XS_BATTLE_FIELD == g_pMain->GetNowStateID() )
			{
				if( 0.f != m_fAutoResurrectionTime )
				{
					m_fAutoResurrectionTime -= fElapsedTime;
					int iMin = (int)m_fAutoResurrectionTime / 60;
					int iSecond = (int)m_fAutoResurrectionTime % 60;
					static int iOldSecond;

					if( 0.f >= m_fAutoResurrectionTime )
					{
						g_pMain->GetPartyUI()->CancelToWaitForDungeonPartyMatching();
						//Handler_EGS_SKIP_CONTINUE_DUNGEON_NOT();
						if ( NULL != g_pData->GetBattleFieldRoom() )
						{
							UINT uiBattleFieldId = g_pData->GetBattleFieldRoom()->GetBattleFieldId();
							UINT uiVillageId = g_pData->GetBattleFieldManager().GetReturnVillageId( uiBattleFieldId );
							CX2State* pState = static_cast<CX2State*>( g_pMain->GetNowState() );
							if ( NULL != pState )
								pState->Handler_EGS_STATE_CHANGE_FIELD_REQ( uiVillageId );
						}
					}
					else
					{
						if( iOldSecond != iSecond )
						{
							iOldSecond = iSecond;
							pStaticRestore_Continue_End->GetString(2)->msg = GET_REPLACED_STRING( (STR_ID_17328, "iiL", (int)iMin ,(int)iSecond, std::wstring( L"X") )); // -- #CFFFF00@1:@2후 또는 @3키로 마을에서 재시작
						}
					}
				}

			}
			else
			{
				pStaticRestore_Continue_End->GetString(2)->msg = L"";
			}

			pStaticRestore_Continue_End->SetShow(true);
		}
	}
//========================================================================
	ResurrectOtherUser();
}

void CX2Game::ResurrectOtherUserOK()
{
	Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ( m_UnitUIDToResurrect );
	if ( m_pDLGResurrect != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGResurrect, NULL, false );

	m_pDLGResurrect = NULL;
}

void CX2Game::ResurrectOtherUserCancel()
{
	if ( m_pDLGResurrect != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGResurrect, NULL, false );

	m_pDLGResurrect = NULL;
}

bool CX2Game::Handler_EGS_STOP_DUNGEON_CONTINUE_TIME_REQ( bool bStop )
{
	if ( bStop == true )
	{
		if ( m_bEnableSendKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ == false )
			return true;

		if ( m_pDLGCheckResurrectByUseCash != NULL )
			return true;
	}

	KEGS_STOP_DUNGEON_CONTINUE_TIME_REQ kPacket;
	kPacket.m_bIsStop = bStop;

	g_pData->GetServerProtocol()->SendPacket( EGS_STOP_DUNGEON_CONTINUE_TIME_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_STOP_DUNGEON_CONTINUE_TIME_ACK );

	return true;
}

bool CX2Game::Handler_EGS_STOP_DUNGEON_CONTINUE_TIME_ACK( KEGS_STOP_DUNGEON_CONTINUE_TIME_ACK& kEGS_STOP_DUNGEON_CONTINUE_TIME_ACK )
{
	if ( kEGS_STOP_DUNGEON_CONTINUE_TIME_ACK.m_bIsStop == true )
	{
		SAFE_DELETE_DIALOG( m_pDLGCheckResurrectByUseCash );
		m_pDLGCheckResurrectByUseCash = g_pMain->KTDGUIOkAndCancelMsgBox( D3DXVECTOR2(250,300),
			GET_REPLACED_STRING( ( STR_ID_139, "S", L"9" ) ),
			CX2State::SUCM_RESURRECT_USE_CASH_OK, g_pMain->GetNowState(),
			CX2State::SUCM_RESURRECT_USE_CASH_CANCEL );

		m_bStopRebirthCount = true;
		m_fTimeWaitStopContinue = 9.0f;
	}
	else
	{
		// 캐시 부활석 사용 창만 없앰
		SAFE_DELETE_DIALOG( m_pDLGCheckResurrectByUseCash );
		m_bStopRebirthCount = false;
		m_bEnableSendKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ = false;
		m_fTimeEnableKEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ = 2.0f;

	}

	return true;
}

bool CX2Game::Handler_EGS_SKIP_CONTINUE_DUNGEON_NOT()
{
	g_pData->GetServerProtocol()->SendID( EGS_SKIP_CONTINUE_DUNGEON_NOT );
	return true;
}

bool CX2Game::Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ( UidType unitUID, bool bAllowUseCash /* = false */ )
{
	if ( NULL == GetMyUnit() )
		return false;

	// 캐시 부활석 사용을 선택함
	if ( bAllowUseCash == true )
	{
		unitUID = m_ReservedPacket.m_iUnitUID;

		Handler_EGS_STOP_DUNGEON_CONTINUE_TIME_REQ( false );
	}
	else
	{
		m_ReservedPacket.m_iUnitUID = unitUID;

		// 부활석 사용을 선택 햇는데 부활석이 없는 경우
		//{{ 허상형 : [2009/10/20] //	자동결제 부활석 체크 추가
		if( GetMyUnit()->GetUnit()->GetResurrectionStoneNum() <= 0 )
			//}} 허상형 : [2009/10/20] //	자동결제 부활석 체크 추가
		{
			Handler_EGS_STOP_DUNGEON_CONTINUE_TIME_REQ( true );

			return true;
		}
	}

	m_bIsSendResurrectionReq = true;

	KEGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ kPacket;
	kPacket.m_iUnitUID = unitUID;

	//{{ 2008. 6. 10  최육사  [참고] 실시간 부활석을 사용하려면 아래 bool값을 true로 주면됨

	//{{ 허상형 : [2009/10/20] //	자동결제 부활석 체크 추가
	if ( GetMyUnit()->GetUnit()->GetResurrectionStoneNum() > 0 )
		//}} 허상형 : [2009/10/20] //	자동결제 부활석 체크 추가
	{
		kPacket.m_bUseCash = false;
	}
	else
	{
		kPacket.m_bUseCash = true;
	}

	m_bStopRebirthCount = false;

	//SAFE_DELETE_DIALOG( m_pDLGCheckResurrectByUseCash );
	if ( m_pDLGCheckResurrectByUseCash != NULL )
		g_pKTDXApp->SendGameDlgMessage( XGM_DELETE_DIALOG, m_pDLGCheckResurrectByUseCash, NULL, false );

	m_pDLGCheckResurrectByUseCash = NULL;

	g_pData->GetServerProtocol()->SendPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK );


	return true;
}

bool CX2Game::Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK& kEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK )
{
	if( g_pMain->DeleteServerPacket( EGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK ) == true )
	{
		if ( g_pMain->IsValidPacket( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK.m_iOK ) == true )
		{
			if ( GetMyUnit() != NULL && GetMyUnit()->GetUnit() != NULL )
			{
				GetMyUnit()->GetUnit()->SetResurrectionStoneNum( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK.m_iNumResurrectionStone );
				//{{ 허상형 : [2009/10/14] //	자동 결제 부활석
				//}} 허상형 : [2009/10/14] //	자동 결제 부활석
				ResetReBirthStoneNumUI();

				if ( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_ACK.m_bUseCash == true )
				{
					((CX2State*)g_pMain->GetNowState())->Handler_EGS_CHECK_BALANCE_REQ();
				}
			}
		}
		m_bIsSendResurrectionReq = false;
	}

	return true;
}

/*virtual*/ bool CX2Game::Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT( KEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT& kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT )
{
	RebirthUserUnit( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT.m_iUnitUID, kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT.m_iStartPosIndex );

	CX2GUUser* pGUUser = GetUserUnitByUID( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT.m_iUnitUID );

	if ( pGUUser != NULL &&
		pGUUser->GetUnit() != NULL )
	{
		if ( GetGameType() == GT_DUNGEON && GetMyUnit() != NULL && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
			m_optrFocusUnit = pGUUser;

		pGUUser->SetDamageCount( 0 );

		//{{ JHKang / 강정훈 / 2011/02/21 / 부활한 유닛이 받은 데미지 누적 값 초기화
		pGUUser->SetTotalAttackedDamage( 0 );
		//}} JHKang / 강정훈 / 2011/02/21 / 부활한 유닛이 받은 데미지 누적 값 초기화

		wstring wstrText;

		if( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT.m_iUnitUID == kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT.m_iUsedByUnitUID )
		{
			wstrText = GET_REPLACED_STRING( ( STR_ID_136, "S", pGUUser->GetUnit()->GetNickName() ) );
		}
		else
		{
			CX2GUUser* pGUUser_Healer = GetUserUnitByUID( kEGS_RESURRECT_TO_CONTINUE_DUNGEON_NOT.m_iUsedByUnitUID );

			if( NULL != pGUUser_Healer && NULL != pGUUser_Healer->GetUnit() )
			{
				wstrText = GET_REPLACED_STRING( ( STR_ID_137, "SS", pGUUser_Healer->GetUnit()->GetNickName(), pGUUser->GetUnit()->GetNickName() ) );
			}
			else
			{
				wstrText = GET_REPLACED_STRING( ( STR_ID_138, "S", pGUUser->GetUnit()->GetNickName() ) );
			}
		}

		if( NULL != g_pChatBox )
		{
			wstring wstrColor = L"#CFF3F4D";							// 255, 63, 77
			D3DXCOLOR coTextColor(1.f, 0.24705f, 0.30196f, 1.f);		// 255, 63, 77

			g_pChatBox->AddChatLog( wstrText.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );
		}
	}

	if ( GetMyUnit() != NULL && GetMyUnit() == pGUUser )
	{
		m_bResurrectionCountStart = false;
		m_fResurrectionRemainTime = 0.0f;
		GetMyUnit()->SetResurrectionTimeOut( false );
		m_bStopRebirthCount = false;



	}

	m_bLastKillCheck				= false;
	m_fLastKillWaitTime				= 0.05f;	//FieldFix: 이 코드는 필요 없음
	m_fLastKillWaitTimeAfterRebirth	= 1.f;

	//몬스터 쓰러트리기
	if( IsHost() == true )
	{
		for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
		{
			CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
			if( pCX2GUNPC != NULL )
			{
				if( pCX2GUNPC->GetIsFallDownMonster() == true )
				{
					if( true == pCX2GUNPC->GetIsFallDownState() )
					{
						pCX2GUNPC->StateChange( pCX2GUNPC->GetDownDamageLandFront(), true );
					}
				}
				//{{ oasis907 : 김상윤 [2011.5.23] 유저 부활시에 SPRIGGAN_LIGHT_BALL 제거
				if( CX2UnitManager::NUI_SPRIGGAN_LIGHT_BALL == pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID )
				{
					pCX2GUNPC->ResetGameUnitWhoAttackedMe();
					pCX2GUNPC->SetNowHp( 0.f );
				}
				//}} oasis907 : 김상윤 [2011.5.23]
			}
		}
	}


	return true;
}

//{{ kimhc // 2010.12.17 // 2010-12-23 New Character CHUNG
/** @function : GetLockOnNearstTarget
	@brief : 자신의 유닛을 기준으로 앞뒤 구분없이 가장 가까운 유닛을 타겟으로 설정
	@param : CX2Room::TEAM_NUM eTeamNum_(자신의 팀Number), OUT bool& bUserUnit_ (유저인지, NPC인지를 얻어옴)
	@return : UidType (찾으면UID, 못찾았으면 -1)
*/
UidType CX2Game::GetLockOnNearstTarget( CX2Room::TEAM_NUM eTeamNum_, IN const D3DXVECTOR3& vMyPos_, OUT bool& bUserUnit_ )
{

	const CX2GameUnit* pNearstUnit = NULL;
	float fNearstDistance = 30000.0f;	// 30000만이라는 값은 최대 수치

	for( UINT uIndex = 0; uIndex < m_UnitList.size(); ++uIndex )
	{
		const CX2GameUnit* pCX2GameUnit = m_UnitList[uIndex];

		if( pCX2GameUnit == NULL )
			continue;

		if( pCX2GameUnit->GetTeam() == eTeamNum_ )
			continue;

		if( true == pCX2GameUnit->GetInvincible() )
			continue;

		if( 0.f >= pCX2GameUnit->GetNowHp() )
			continue;

		if( CX2GameUnit::GUT_NPC == pCX2GameUnit->GetGameUnitType() )
		{
			if( true == pCX2GameUnit->GetIsBosRaidNPC() )
				continue;

			const CX2GUNPC* pNPC = static_cast<const CX2GUNPC*>( pCX2GameUnit );
			switch( pNPC->GetNPCTemplet().m_ClassType )
			{
			case CX2UnitManager::NCT_THING_TRAP:
			case CX2UnitManager::NCT_THING_CHECKER:
				{
					continue;
				} break;
			} // switch
		} // if

		float fDistance = GetDistance( vMyPos_, pCX2GameUnit->GetPos() );

		// 더 가까운 애를 찾으면
		if ( fDistance < fNearstDistance )
		{
			pNearstUnit		= pCX2GameUnit;
			fNearstDistance = fDistance;
		} // if
	} // for


	// 타겟이 없으면
	if( NULL == pNearstUnit )
	{
		return -1;
	} // if
	// 타켓이 있으면
	else
	{
		if( pNearstUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			bUserUnit_ = true;
			const CX2GUUser* pCX2GUUser = static_cast<const CX2GUUser*>( pNearstUnit );
			return pCX2GUUser->GetUnitUID();
		} // if
		else
		{
			bUserUnit_ = false;
			const CX2GUNPC* pCX2GUNPC = static_cast<const CX2GUNPC*>( pNearstUnit );
			return pCX2GUNPC->GetUnitUID();
		} // else
	} // else
}

//}} kimhc // 2010.12.17 //  2010-12-23 New Character CHUNG

UidType CX2Game::GetLockOnNearstTargetInRange( CX2Room::TEAM_NUM eTeamNum_, IN const D3DXVECTOR3& vMyPos_, IN D3DXVECTOR2& vRange_, OUT bool& bUserUnit_
												, bool bIsNotTargetPreLockOnTarget/* = false*/
											)
{

	const CX2GameUnit* pNearstUnit = NULL;
	float fNearstDistance = vRange_.y;

	for( UINT uIndex = 0; uIndex < m_UnitList.size(); ++uIndex )
	{
		const CX2GameUnit* pCX2GameUnit = m_UnitList[uIndex];

		if( pCX2GameUnit == NULL )
			continue;

		if( pCX2GameUnit->GetTeam() == eTeamNum_ )
			continue;

		if( true == pCX2GameUnit->GetInvincible() )
			continue;

		if( 0.f >= pCX2GameUnit->GetNowHp() )
			continue;

		if( CX2GameUnit::GUT_NPC == pCX2GameUnit->GetGameUnitType() )
		{
			if( true == pCX2GameUnit->GetIsBosRaidNPC() )
				continue;

			const CX2GUNPC* pNPC = static_cast<const CX2GUNPC*>( pCX2GameUnit );
			switch( pNPC->GetNPCTemplet().m_ClassType )
			{
			case CX2UnitManager::NCT_THING_TRAP:
			case CX2UnitManager::NCT_THING_CHECKER:
				{
					continue;
				} break;
			} // switch
		} // if

		if( true == bIsNotTargetPreLockOnTarget &&
			false == m_setPreLockOnTargetList.empty() &&
			m_setPreLockOnTargetList.end() != m_setPreLockOnTargetList.find( pCX2GameUnit->GetUnitUID() ) )
			continue;

		float fDistance = GetDistance( vMyPos_, pCX2GameUnit->GetPos() );

		// 더 가까운 애를 찾으면
		if ( fDistance < fNearstDistance && fDistance > vRange_.x )
		{
			pNearstUnit		= pCX2GameUnit;
			fNearstDistance = fDistance;
		} // if
	} // for


	// 타겟이 없으면
	if( NULL == pNearstUnit )
	{
		if( true == bIsNotTargetPreLockOnTarget )
			m_setPreLockOnTargetList.clear();
		return -1;
	} // if
	// 타켓이 있으면
	else
	{
		if( true == bIsNotTargetPreLockOnTarget )
			m_setPreLockOnTargetList.insert( pNearstUnit->GetUnitUID() );
		if( pNearstUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			bUserUnit_ = true;
			const CX2GUUser* pCX2GUUser = static_cast<const CX2GUUser*>( pNearstUnit );
			return pCX2GUUser->GetUnitUID();
		} // if
		else
		{
			bUserUnit_ = false;
			const CX2GUNPC* pCX2GUNPC = static_cast<const CX2GUNPC*>( pNearstUnit );
			return pCX2GUNPC->GetUnitUID();
		} // else
	} // else
}


UidType CX2Game::GetLockOnNearstTargetOnlyBasic( CX2Room::TEAM_NUM eTeamNum_, IN const D3DXVECTOR3& vMyPos_, OUT bool& bUserUnit_ )
{

	const CX2GameUnit* pNearstUnit = NULL;
	float fNearstDistance = 30000.0f;	// 30000만이라는 값은 최대 수치

	for( UINT uIndex = 0; uIndex < m_UnitList.size(); ++uIndex )
	{
		const CX2GameUnit* pCX2GameUnit = m_UnitList[uIndex];

		if( pCX2GameUnit == NULL )
			continue;

		if( pCX2GameUnit->GetTeam() == eTeamNum_ )
			continue;

		if( true == pCX2GameUnit->GetInvincible() )
			continue;

		if( 0.f >= pCX2GameUnit->GetNowHp() )
			continue;

		if( CX2GameUnit::GUT_NPC == pCX2GameUnit->GetGameUnitType() )
		{
			if( true == pCX2GameUnit->GetIsBosRaidNPC() )
				continue;

			const CX2GUNPC* pNPC = static_cast<const CX2GUNPC*>( pCX2GameUnit );
			if( pNPC->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
				continue;
		} // if

		float fDistance = GetDistance( vMyPos_, pCX2GameUnit->GetPos() );

		// 더 가까운 애를 찾으면
		if ( fDistance < fNearstDistance )
		{
			pNearstUnit		= pCX2GameUnit;
			fNearstDistance = fDistance;
		} // if
	} // for


	// 타겟이 없으면
	if( NULL == pNearstUnit )
	{
		return -1;
	} // if
	// 타켓이 있으면
	else
	{
		if( pNearstUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			bUserUnit_ = true;
			const CX2GUUser* pCX2GUUser = static_cast<const CX2GUUser*>( pNearstUnit );
			return pCX2GUUser->GetUnitUID();
		} // if
		else
		{
			bUserUnit_ = false;
			const CX2GUNPC* pCX2GUNPC = static_cast<const CX2GUNPC*>( pNearstUnit );
			return pCX2GUNPC->GetUnitUID();
		} // else
	} // else
}

UidType	CX2Game::GetLockOnTarget( CX2Room::TEAM_NUM teamNum, int seedNum, bool& bUserUnit, int iModulusFactor /*= 30*/ )
{
	int keyCount = (seedNum % iModulusFactor) + 1;

	bool bTargetIn = false;
	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2GameUnit* pCX2GameUnit = m_UnitList[i];
		if( pCX2GameUnit == NULL )
			continue;

		if( teamNum == pCX2GameUnit->GetTeam() )
			continue;

		if( pCX2GameUnit->GetInvincible() == true )
			continue;

		if( pCX2GameUnit->GetNowHp() <= 0.f )
			continue;

		if( pCX2GameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
		{
			if( true == pCX2GameUnit->GetIsBosRaidNPC() )
				continue;

			CX2GUNPC* pNPC = (CX2GUNPC*) pCX2GameUnit;
			switch( pNPC->GetNPCTemplet().m_ClassType )
			{
			case CX2UnitManager::NCT_THING_TRAP:
			case CX2UnitManager::NCT_THING_CHECKER:
				{
					continue;
				} break;
			}
		}

		bTargetIn = true;
		break;
	}
	if( bTargetIn == false )
	{
		return -1;
	}

	CX2GameUnit* pRetUnit = NULL;

	while( keyCount > 0 )
	{
		for( int i = 0; i < (int)m_UnitList.size(); i++ )
		{
			CX2GameUnit* pCX2GameUnit = m_UnitList[i];
			if( pCX2GameUnit == NULL )
				continue;

			if( teamNum == pCX2GameUnit->GetTeam() )
				continue;

			if( pCX2GameUnit->GetInvincible() == true )
				continue;

			if( pCX2GameUnit->GetNowHp() <= 0.f )
				continue;

			if( pCX2GameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
			{

				if( true == pCX2GameUnit->GetIsBosRaidNPC() )
					continue;

				CX2GUNPC* pNPC = (CX2GUNPC*) pCX2GameUnit;
				switch( pNPC->GetNPCTemplet().m_ClassType )
				{
				case CX2UnitManager::NCT_THING_TRAP:
				case CX2UnitManager::NCT_THING_CHECKER:
					{
						continue;
					} break;
				}

				keyCount--;
				pRetUnit = pCX2GameUnit;
			}
			else
			{
				keyCount--;
				pRetUnit = pCX2GameUnit;
			}

			if( keyCount <= 0 )
				break;
		}

	}
	if( pRetUnit != NULL )
	{
		if( pRetUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
		{
			bUserUnit = true;
			CX2GUUser* pCX2GUUser = (CX2GUUser*)pRetUnit;
			return pCX2GUUser->GetUnitUID();
		}
		else
		{
			bUserUnit = false;
			CX2GUNPC* pCX2GUNPC = (CX2GUNPC*)pRetUnit;
			return pCX2GUNPC->GetUnitUID();
		}
	}

	return -1;
}

CX2SlideShot* CX2Game::GetSlideShot()
{
	return g_pData->GetSlideShot();
}




void CX2Game::SetShowAllUnitGageBar( bool bShow )
{
	m_bShowAllUnitGageBar = bShow;
	if ( m_pDLGSkillSlot != NULL )
		if( NULL != g_pData && NULL != g_pData->GetUIManager() )
		{
			if( true == g_pData->GetUIManager()->GetShowRidingPetSkillslot() )
			{
				CX2RidingPetManager::GetInstance()->GetSkillSlot()->SetShowRidingSkillSlot(bShow);
				m_pDLGSkillSlot->SetShow( false );
			}
			else
			{
				CX2RidingPetManager::GetInstance()->GetSkillSlot()->SetShowRidingSkillSlot(false);
				m_pDLGSkillSlot->SetShow(bShow);
			}
		}


	for ( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2GameUnit* pGameUnit = m_UnitList[i];
		if ( pGameUnit != NULL && false == pGameUnit->IsMyUnit() && pGameUnit->GetGageData() != NULL )
			pGameUnit->SetShowGage( m_bShowAllUnitGageBar );
	}

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		CX2PET *pPet = g_pData->GetPetManager()->GetMyPet();
		if( pPet != NULL )
		{
			pPet->SetShowGageBar( m_bShowAllUnitGageBar );
		}
	}
}



bool CX2Game::AnyUserUnitInRange( D3DXVECTOR3 pos, float fRange )
{
	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = m_UserUnitList[i];
		if( pCX2GUUser != NULL )
		{
			if( pCX2GUUser->GetNowHp() <= 0.f )
				continue;

			if( GetDistance( pos, pCX2GUUser->GetPos() ) < fRange )
			{

				return true;
			}
		}
	}


	return false;
}


bool CX2Game::IsThereAnyUserUnitOnLine( int iLineIndex )
{

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = m_UserUnitList[i];
		if( pCX2GUUser == NULL )
			continue;

		if( CX2GameUnit::GUSI_DIE == pCX2GUUser->GetGameUnitState() )
			continue;

		if( pCX2GUUser->GetNowHp() <= 0.f )
			continue;

		if( iLineIndex == pCX2GUUser->GetSyncData().lastTouchLineIndex &&
			true == pCX2GUUser->GetUnitCondition().bFootOnLine )
		{
			return true;
		}

	}


	return false;
}




bool CX2Game::IsThereAnyUserUnitUsingSkill( CX2SkillTree::SKILL_ID eSkillID )
{

	BOOST_TEST_FOREACH( CX2GUUser*, pUser, m_UserUnitList )
	{
		if( NULL == pUser )
			continue;

		if( CX2GameUnit::GUSI_DIE == pUser->GetGameUnitState() )
			continue;

		if( pUser->GetNowHp() <= 0.f )
			continue;

		if( pUser->GetNowStateSkillID() == eSkillID )
			return true;
	}

	return false;
}





// @bEnterPortal : 텔레포트로 들어가는건지 나오는 건지; 캐릭터를 보이거나 안보이거나
void CX2Game::CreateStageLoadingTeleportEffectForAllUser( bool bEnterPortal )
{
	bool bShowUser = true;
	if( true == bEnterPortal )
	{
		m_bShowTeleportEffectOnStageStart = true;	// 다음 스테이지 시작할 때 텔레포트 효과 보이도록 하기
		bShowUser = false;
	}




	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pCX2GUUser = m_UserUnitList[i];
		if( pCX2GUUser == NULL )
			continue;

		if( CX2GameUnit::GUSI_DIE == pCX2GUUser->GetGameUnitState() )
			continue;

		if( pCX2GUUser->GetNowHp() <= 0.f )
			continue;

		GetEffectSet()->PlayEffectSet( L"EffectSet_StageLoadingTeleport", (CX2GameUnit*)pCX2GUUser );

		pCX2GUUser->SetShowObject( bShowUser );
		pCX2GUUser->PlaySound( L"Nasod_Element_Teleport.ogg" );

		if( true == bEnterPortal )
		{
			pCX2GUUser->SetCanNotInputTime_LUA( 5.f );
			pCX2GUUser->AccessPhysicParam().nowSpeed		= D3DXVECTOR2( 0, 0 );
			pCX2GUUser->AccessPhysicParam().passiveSpeed	= D3DXVECTOR2( 0, 0 );
		}
		else
		{
			pCX2GUUser->SetCanNotInputTime_LUA( 0.f );
		}
	}
}


//{{oasis:김상윤////2009-10-7////
bool CX2Game::IsNearNPCUnitUID_LUA( D3DXVECTOR3 pos, CX2UnitManager::NPC_UNIT_ID unitID, float fMaxRange )
{
	bool answer = false;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == unitID && pNPC->GetGroupAICommand_LUA() == CX2GUNPC::GAI_CMD_OFF)
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance )
			{
				answer = true;
				break;
			}
		}
	}
	return answer;
}

CX2GUNPC* CX2Game::GetNearNoneAINPCUnitUID( D3DXVECTOR3 pos, CX2UnitManager::NPC_UNIT_ID unitID, float fMaxRange )
{
	CX2GUNPC* retUnit = NULL;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == unitID &&
			pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() &&
			pNPC->GetGroupAICommand_LUA() == CX2GUNPC::GAI_CMD_OFF) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance )
			{
				retUnit = pNPC;
				fDistance = fTempDist;
			}
		}
	}

	return retUnit;
}


D3DXVECTOR3	CX2Game::GetFixedDistancePosition_LUA( D3DXVECTOR3 pvPos, float fDist, bool bIsRight )
{
	return GetWorld()->GetLineMap()->GetFixedDistancePosition(&pvPos, fDist, bIsRight);
}



float CX2Game::GetDist_LUA( const D3DXVECTOR3& pos1, const D3DXVECTOR3& pos2 )
{
	D3DXVECTOR3 temp = pos2 - pos1;
	return D3DXVec3Length( &temp );
}

CX2GUUser* CX2Game::GetNearestUserUnit( const D3DXVECTOR3& vPos_ )
{
	CX2GUUser* pNearestGUUser = NULL;
	float fNearestDistance3Sq = 0;

	for( UINT i = 0; i < m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pGUUser = m_UserUnitList[i];
		if( NULL == pGUUser )
			continue;

		if ( pGUUser->GetNowHp() <= 0.f || CX2GameUnit::GUSI_DIE == pGUUser->GetGameUnitState() )
			continue;

		const float fDistance3Sq = GetDistance3Sq( vPos_, pGUUser->GetPos() );
		if ( NULL == pNearestGUUser || fDistance3Sq < fNearestDistance3Sq )
		{
			pNearestGUUser		= pGUUser;
			fNearestDistance3Sq = fDistance3Sq;
		}
	}

	return pNearestGUUser;
}

UidType CX2Game::GetNearestUserUnitUID_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange )
{
	UidType retUID = 0;

	wstringstream wstream;
	wstream << L"GetNearestUserUnitUID_LUA 사용 금지";
	ASSERT( !"GetTargetUnitUID_LUA 사용 금지" );
	StateLog( wstream.str().c_str() );
// 	float fDistance = fMaxRange;
//
// 	for( UINT i = 0; i < m_UserUnitList.size(); i++ )
// 	{
// 		CX2GUUser* pGUUser = m_UserUnitList[i];
// 		if( NULL == pGUUser )
// 			continue;
//
// 		if( pGUUser->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pGUUser->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
// 		{
// 			float fTempDist = GetDistance( pos, pGUUser->GetPos() );
// 			if( fTempDist < fDistance && fTempDist > fMinRange )
// 			{
// 				retUID = pGUUser->GetUnitUID();
// 				fDistance = fTempDist;
// 			}
// 		}
// 	}

	return retUID;
}

//{{oasis:김상윤////2009-10-8////
D3DXVECTOR3 CX2Game::GetNearestUserUnitPos_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange )
{
	D3DXVECTOR3 retPos(0, 0, 0);
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pNPC = (CX2GUUser*)m_UserUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance && fTempDist > fMinRange )
			{
				retPos = pNPC->GetPos();
				fDistance = fTempDist;
			}
		}
	}
	return retPos;
}
//}}oasis:김상윤////2009-10-8////

D3DXVECTOR3 CX2Game::GetNearestNPCUnitPos_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange )
{
	D3DXVECTOR3 retPos(0, 0, 0);
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( true == pNPC->GetInvincible() )
			continue;

		if( 0.f >= pNPC->GetNowHp() )
			continue;

		if( pNPC->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
			continue;

		float fTempDist = GetDistance( pos, pNPC->GetPos() );
		if( fTempDist < fDistance && fTempDist > fMinRange )
		{
			retPos = pNPC->GetPos();
			fDistance = fTempDist;
		}
	}
	return retPos;
}

//}}oasis:김상윤////2009-10-7////

UidType CX2Game::GetFarthestUserUnitUID_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange )
{
	UidType retUID = 0;
	float fDistance = fMinRange;

	for( UINT i = 0; i < m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pGUUser = m_UserUnitList[i];
		if( NULL == pGUUser )
			continue;

		if( pGUUser->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pGUUser->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pGUUser->GetPos() );
			if( fTempDist > fDistance && fTempDist < fMaxRange )
			{
				retUID = pGUUser->GetUnitUID();
				fDistance = fTempDist;
			}
		}
	}

	return retUID;
}


//{{ oasis907 : 김상윤 [2010.7.30] //
D3DXVECTOR3 CX2Game::GetFarthestUserUnitPos_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange )
{
	D3DXVECTOR3 retPos(0, 0, 0);
	float fDistance = fMinRange;

	for( int i = 0; i < (int)m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pNPC = (CX2GUUser*)m_UserUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pNPC->GetLandPos() );
			if( fTempDist > fDistance && fTempDist < fMaxRange )
			{
				retPos = pNPC->GetLandPos();
				fDistance = fTempDist;
			}
		}
	}
	return retPos;
}
//}}

int CX2Game::GetNearNPCUnitUIDInStart_LUA( D3DXVECTOR3 pos, CX2UnitManager::NPC_UNIT_ID unitID, float fMaxRange )
{
	int retUID = 0;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == unitID && pNPC->GetNPCSyncData().nowState == pNPC->GetStartState() )
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance )
			{
				retUID = (int)pNPC->GetUnitUID();
				fDistance = fTempDist;
			}
		}
	}

	return retUID;
}

CX2GUNPC*	CX2Game::GetNearNPCUnitInStart( const D3DXVECTOR3 vPos_, const CX2UnitManager::NPC_UNIT_ID eUnitID_, const float fMaxRange /*= 9999*/ )
{
	CX2GUNPC* pNearNpc = NULL;
	float fDistance3Sq = fMaxRange * fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == eUnitID_ && pNPC->GetNPCSyncData().nowState == pNPC->GetStartState() )
		{
			float fTempDistance3Sq = GetDistance3Sq( vPos_, pNPC->GetPos() );
			if( fTempDistance3Sq < fDistance3Sq )
			{
				pNearNpc		= pNPC;
				fDistance3Sq	= fTempDistance3Sq;
			}
		}
	}

	return pNearNpc;
}


int CX2Game::GetNearNPCUnitUID_LUA( D3DXVECTOR3 pos, CX2UnitManager::NPC_UNIT_ID unitID, float fMaxRange )
{
	int retUID = 0;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == unitID &&
			pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance )
			{
				retUID = (int)pNPC->GetUnitUID();
				fDistance = fTempDist;
			}
		}
	}

	return retUID;
}

CX2GUNPC*	CX2Game::GetNearestNpcInSpecificRangeByNpcId( const D3DXVECTOR3& vPos_, const CX2UnitManager::NPC_UNIT_ID eUnitID_, const float fMaxRange_ /*= 9999*/ )
{
	CX2GUNPC* pNearestNpc = NULL;
	float fDistance3Sq = fMaxRange_ * fMaxRange_;

	for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = m_NPCUnitList[i];
		if( NULL == pNPC )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == eUnitID_ &&
			pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDistance3Sq = GetDistance3Sq( vPos_, pNPC->GetPos() );
			if( fTempDistance3Sq < fDistance3Sq )
			{
				pNearestNpc		= pNPC;
				fDistance3Sq	= fTempDistance3Sq;
			}
		}
	}

	return pNearestNpc;
}

int CX2Game::GetNearNPCUnitUIDEx( D3DXVECTOR3 pos, CX2UnitManager::NPC_UNIT_ID unitID, int npcUIDToExcept, float fMaxRange )
{
	int retUID = 0;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if ( pNPC->GetUID() == npcUIDToExcept )
			continue;

		if( pNPC->GetNPCTemplet().m_nNPCUnitID == unitID &&
			pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance )
			{
				retUID = (int)pNPC->GetUnitUID();
				fDistance = fTempDist;
			}
		}
	}

	return retUID;
}

int CX2Game::GetNearNPCUnitUIDEx( D3DXVECTOR3 pos, const vector< CX2UnitManager::NPC_UNIT_ID >& vecUnitID, int npcUIDToExcept, float fMaxRange )
{
	int retUID = 0;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if ( pNPC->GetUID() == npcUIDToExcept )
			continue;

		if ( std::find( vecUnitID.begin(), vecUnitID.end(), pNPC->GetNPCTemplet().m_nNPCUnitID ) != vecUnitID.end() )
		{
			if( pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
			{
				float fTempDist = GetDistance( pos, pNPC->GetPos() );
				if( fTempDist < fDistance )
				{
					retUID = (int)pNPC->GetUnitUID();
					fDistance = fTempDist;
				}
			}
		}
	}

	return retUID;
}


int CX2Game::GetNearestNPCUnitUID( D3DXVECTOR3 pos, int npcUIDToExcept ,float fMaxRange /*= 9999*/ )
{
	int retUID = 0;
	float fDistance = fMaxRange;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if ( (int)pNPC->GetUnitUID() == npcUIDToExcept )
			continue;

		if( pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDist = GetDistance( pos, pNPC->GetPos() );
			if( fTempDist < fDistance )
			{
				retUID = (int)pNPC->GetUnitUID();
				fDistance = fTempDist;
			}
		}
	}

	return retUID;
}

CX2GUUser* CX2Game::GetNearestUserUnitInSpecificRange( const D3DXVECTOR3& vPos_, const float fMinRange_, const float fMaxRange_ /*= 9999 */ )
{
	CX2GUUser* pNearestGUUser = NULL;
	float fDistance3Sq = fMaxRange_ * fMaxRange_;

	for( UINT i = 0; i < m_UserUnitList.size(); i++ )
	{
		CX2GUUser* pGUUser = m_UserUnitList[i];
		if( NULL == pGUUser )
			continue;

		if ( pGUUser->GetNowHp() <= 0.f || CX2GameUnit::GUSI_DIE == pGUUser->GetGameUnitState() )
			continue;

		float fTempDistance3Sq = GetDistance3Sq( vPos_, pGUUser->GetPos() );
		if( fTempDistance3Sq < fDistance3Sq && fTempDistance3Sq > fMinRange_ )
		{
			pNearestGUUser = pGUUser;
			fDistance3Sq = fTempDistance3Sq;
		}
	}

	return pNearestGUUser;
}

// @bOnFront : 상대 유닛이 내 캐릭터 앞쪽에 있는지
CX2GameUnit* CX2Game::GetFrontNearestEnemyUnit( CX2GameUnit* pMyUnit, float& fDistance )
{
	KTDXPROFILE();


	CX2GameUnit* pNearestGameUnit = NULL;
	fDistance = -1.f;
	D3DXVECTOR3 pos = pMyUnit->GetPos();


	for( int i = 0; i < (int)m_UnitList.size(); i++ )
	{
		CX2GameUnit* pGameUnit = m_UnitList[i];
		if( NULL == pGameUnit )
			continue;

		if( pMyUnit->GetTeam() != pGameUnit->GetTeam() &&
			pGameUnit->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pGameUnit->GetGameUnitState() )
		{
			D3DXVECTOR3 vDir = pGameUnit->GetPos() - pos;
            float fY = vDir.y;
            vDir.y = 0.f;
			if( D3DXVec3Dot( &vDir, &pGameUnit->GetDirVector() ) > 0.f )
			{
				if( false == pMyUnit->GetIsRight() )
					continue;
			}
			else
			{
				if( true == pMyUnit->GetIsRight() )
					continue;
			}
            vDir.y = fY;

			float fTempDistSquare = D3DXVec3LengthSq( &vDir );
			if( fDistance == -1.f || fTempDistSquare < fDistance )
			{
				fDistance = fTempDistSquare;
				pNearestGameUnit = pGameUnit;
			}
		}
	}

	if( NULL != pNearestGameUnit )
	{
		fDistance = sqrt( fDistance );
	}

	return pNearestGameUnit;
}






bool CX2Game::ChangeRebirthStateNPCUnit_LUA( int NPCUID )
{
	CX2GUNPC* pCX2GUNPC = GetNPCUnitByUID( NPCUID );
	if( pCX2GUNPC != NULL && pCX2GUNPC->GetNowHp() > 0.0f )
	{
		pCX2GUNPC->StateChange( pCX2GUNPC->GetRebirthState() );
		return true;
	}
	return false;
}

int CX2Game::GetNPCCountAt( D3DXVECTOR3 vPosition, float fRadius /*= 30.f*/, CX2UnitManager::NPC_UNIT_ID eNPCID /*= CX2UnitManager::NUI_NONE*/ )
{
	int iNPCCount = 0;

	for( UINT i=0; i<m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pGUNPC = m_NPCUnitList[i];
		if( NULL == pGUNPC )
			continue;

		if( eNPCID != CX2UnitManager::NUI_NONE && eNPCID != pGUNPC->GetNPCTemplet().m_nNPCUnitID )
			continue;

		if( GetDistance( vPosition, pGUNPC->GetPos() ) < fRadius )
			iNPCCount += 1;
	}

	return iNPCCount;

}


D3DXVECTOR3 CX2Game::GetEmptyStartPos()
{
	if( m_pWorld == NULL )
		return D3DXVECTOR3(0,0,0);

	if( m_pWorld->GetLineMap() == NULL )
		return D3DXVECTOR3(0, 0, 0 );

	CKTDGLineMap* pLineMap = m_pWorld->GetLineMap();

    const CKTDGLineMap::StartInfoMap& mapStartInfo = pLineMap->GetStartInfoMap();
	CKTDGLineMap::StartInfoMap::const_iterator it;
	for( it = mapStartInfo.begin(); it != mapStartInfo.end(); it++ )
	{
		const D3DXVECTOR3& vPos = it->second.m_vStartPos;
		int nNpcCount = GetNPCCountAt( vPos, 500.f );
		if( nNpcCount == 0 )
		{
			return vPos;
		}
	}

	if( mapStartInfo.empty() == false )
	{
        return  mapStartInfo.begin()->second.m_vStartPos;
	}
	else
	{
		return pLineMap->GetLineData(0)->startPos;
	}
}





// 몬스터 소환 필살기는 스테이지가 끝난 후에 사용할 수 없다는 경고 메세지를 보여주기 위해서 IsSummonSpecialAttack 에 하드코딩 해야 합니다.
void CX2Game::SetUserSummonedNPCInfo( CX2NPCAI::NPC_AI_TYPE eAIType, UidType iAllyUID, CX2GUNPC* pNPC, int iHyperMode, CX2UnitManager::NPC_UNIT_ID unitID )
{
	// 이브의 소환 필살기에 의해 소환된 몬스터의 경우 npc stat을 직접 설정 해준다
	// warning!! 일단은 allynpc로 소환되는 NPC는 이브 밖에 없으므로 이렇게 하드코딩
	// level을 일단 이브의 필살기 레벨, 각성에 의한 증가 수치를 담는 변수로 사용. 나중에는 뭔가...

	if( (CX2NPCAI::NAT_ALLY == eAIType && -1 != iAllyUID) || CX2Room::TN_NONE != pNPC->GetAllyTeam() )
	{
		CX2GUUser* pUser = GetUserUnitByUID( iAllyUID );
		if( NULL != pUser )
		{
			// 소환 몬스터 레벨은 소환사의 레벨과 같게
			pNPC->SetHardLevel( pUser->GetUnit()->GetUnitData().m_Level, false );

			float fStatAtkRate = 1.f;
			fStatAtkRate = pUser->GetStatAtkRateOfSummoned();

			//{{ megagame / 박교현 / 2010.04.30 / NPC 스탯 추가
			// 소환 몬스터 크리확률, 명중률, 속도, 이동속도, 점프속도는 소환사의 것과 같게
			CX2UnitManager::NPCUnitStat NPCStat;
			pNPC->GetNPCStat(NPCStat);

			//{{ kimhc // 2011-07-28 // 옵션수치화
			NPCStat.m_fPercentCritical = pUser->GetCriticalRate();
			NPCStat.m_fAccuracy = pUser->GetAccuracyPercent();
				//}} kimhc // 2011-07-28 // 옵션수치화



			pNPC->SetNPCStat(NPCStat);
			//}}

			// 유저의 스탯으로 npc 스탯을 설정한다.
			CX2Stat::Stat npcStat = pUser->GetStat().GetStat();
			npcStat.m_fAtkPhysic	= pUser->GetPhysicAttackValue();
			npcStat.m_fAtkMagic		= pUser->GetMagicAttackValue();
			npcStat.m_fDefPhysic	= pUser->GetPhysicDefenceValue();
			npcStat.m_fDefMagic		= pUser->GetMagicDefenceValue();
			{
				npcStat.m_fAtkPhysic += npcStat.m_fAtkPhysic * pUser->GetSocketData().m_Stat.m_ExtraStat.m_fIncreaseAtkPhysicRate;
				npcStat.m_fAtkMagic += npcStat.m_fAtkMagic * pUser->GetSocketData().m_Stat.m_ExtraStat.m_fIncreaseAtkMagicRate;

				pNPC->SetAdditionalAttack ( pUser->GetAdditionalAttack() );


			}

// 			if( pUser->GetStat().GetAddOnTime().m_fAtkPhysic > 0.f )
// 				npcStat.m_fAtkPhysic += pUser->GetStat().GetAddOnStat().m_fAtkPhysic;
// 			if( pUser->GetStat().GetAddOnTime().m_fAtkMagic > 0.f )
// 				npcStat.m_fAtkMagic += pUser->GetStat().GetAddOnStat().m_fAtkMagic;
// 			if( pUser->GetStat().GetAddOnTime().m_fDefPhysic > 0.f )
// 				npcStat.m_fDefPhysic += pUser->GetStat().GetAddOnStat().m_fDefPhysic;
// 			if( pUser->GetStat().GetAddOnTime().m_fDefMagic > 0.f )
// 				npcStat.m_fDefMagic += pUser->GetStat().GetAddOnStat().m_fDefMagic;




			float fStatScale = 1.f;
			float fHyperModeStatScale = 1.f;
			if( iHyperMode > 0 ) // 각성하고 필살기를 사용한 경우
			{
				fHyperModeStatScale = 1.2f;
			}


			switch( unitID )
			{

			default:
				{
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );
				} break;


			case CX2UnitManager::NUI_NASOD_WATCH_EVE:
			case CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO:
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EN_DIMENSION_LINK );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EN_DIMENSION_LINK ) );	/// 스킬 레벨

						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
					}

					fStatScale *= fHyperModeStatScale;
					if( NULL != pUser )
					{
						fStatScale *= (1.f + pUser->GetSocketData().m_fSkillDamageUpRate);
					}


					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 0.15f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f * fStatAtkRate,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f * fStatAtkRate,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 0.8f );
				} break;

			case CX2UnitManager::NUI_SI_HA_FERDINAND:
				{
					if( NULL != pUser->GetUnit() && CX2Unit::UT_EVE == pUser->GetUnit()->GetType() )
					{
						float fPowerRate = 1.f;
						const int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION );	//엠프레스 궁극기 - 링크 오버차지 썬더볼트
						if( iSkillLevel > 0 )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION );

							if( NULL != pSkillTemplet )
							{
								if ( NULL == pUser->GetUnit() )
									return;

								const int iSkillTempletLevel = max( 1, iSkillLevel );	/// 스킬 레벨

								fPowerRate *= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
							}
						}
						/// AT_SPECIAL 공격 배율 증가 버프 적용
						fPowerRate *= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );

						fStatScale *= fHyperModeStatScale;

						pNPC->SetNPCStat(
							npcStat.m_fBaseHP		* fStatScale	* 1.f,
							npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
							npcStat.m_fAtkMagic		* fStatScale	* 1.f,
							npcStat.m_fDefPhysic	* fStatScale	* 1.f,
							npcStat.m_fDefMagic		* fStatScale	* 1.f );

						pNPC->SetDefaultPowerRate( fPowerRate );
						pNPC->SetNearestGUUser(pUser);
						const int iNpcUid = pNPC->GetUID();
						CX2GUEve* pEve = static_cast<CX2GUEve*>( pUser );
						if( NULL != pEve )
							pEve->SaveFerdinandNPCUID(iNpcUid);
					}
				} break;

			case CX2UnitManager::NUI_WALLY_9TH_EVE:
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EN_DIMENSION_LINK_BLADER );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// 스킬 레벨

						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
					}

					fStatScale *= fHyperModeStatScale;
					if( NULL != pUser )
					{
						fStatScale *= (1.f + pUser->GetSocketData().m_fSkillDamageUpRate);
					}

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.2f * fStatAtkRate,
						npcStat.m_fAtkMagic		* fStatScale	* 0.8f * fStatAtkRate,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );

					/// AT_SPECIAL 공격 배율 증가 버프 적용
					float fSummonMonsterPowerRate	= 1.f;
					fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );
					pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

				} break;


			case CX2UnitManager::NUI_NASOD_ELEMENT_FIRE_EVE:
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EN_DIMENSION_LINK_GUARDIAN );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_EN_DIMENSION_LINK_GUARDIAN ) );	/// 스킬 레벨

						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
					}


					fStatScale *= fHyperModeStatScale;
					if( NULL != pUser )
					{
						fStatScale *= (1.f + pUser->GetSocketData().m_fSkillDamageUpRate);
					}

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 0.f  * fStatAtkRate,
						npcStat.m_fAtkMagic		* fStatScale	* 1.2f * fStatAtkRate,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );

					/// AT_SPECIAL 공격 배율 증가 버프 적용
					float fSummonMonsterPowerRate	= 1.f;
					fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );
					pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

				} break;

				//case CX2UnitManager::NUI_GUARDIAN_EVE:
			case CX2UnitManager::NUI_GUARDIAN_SP1_EVE:
			case CX2UnitManager::NUI_GUARDIAN_SP1_EVE_MEMO:
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EAT_SURFACE_CUTING );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// 스킬 레벨

						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
					}

					fStatScale *= fHyperModeStatScale;
					if( NULL != pUser )
					{
						fStatScale *= (1.f + pUser->GetSocketData().m_fSkillDamageUpRate);
					}


					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.2f * fStatAtkRate,
						npcStat.m_fAtkMagic		* fStatScale	* 0.8f * fStatAtkRate,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );

					/// AT_SPECIAL 공격 배율 증가 버프 적용
					float fSummonMonsterPowerRate	= 1.f;
					fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );
					pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

				} break;

			case CX2UnitManager::NUI_GUARDIAN_SP2_EVE:
			case CX2UnitManager::NUI_GUARDIAN_SP2_EVE_MEMO:
				{

					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EAT_SONIC_WAVE );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_EAT_SONIC_WAVE ) );	/// 스킬 레벨

						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
					}

					fStatScale *= fHyperModeStatScale;
					if( NULL != pUser )
					{
						fStatScale *= (1.f + pUser->GetSocketData().m_fSkillDamageUpRate);
					}

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.2f * fStatAtkRate,
						npcStat.m_fAtkMagic		* fStatScale	* 0.8f * fStatAtkRate,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );

					/// AT_SPECIAL 공격 배율 증가 버프 적용
					float fSummonMonsterPowerRate	= 1.f;
					fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );
					pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

				} break;

				//{{ JHKang / 강정훈 / 2010/11/22 / 이브 아토믹 쉴드
 			case CX2UnitManager::NUI_ATOMIC_SHIELD_EVE:
 				{
 					float fMaxHpRel = 1.f;
 					float fShieldSize = 1.f;
 					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD );
 					if( NULL != pSkillTemplet )
 					{
						if ( NULL == pUser->GetUnit() )
						return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// 스킬 레벨

						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
						/// 쉴드 전체 HP 크기
						fMaxHpRel = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_REL, iSkillTempletLevel );

						switch( g_pMain->GetNowStateID() )
						{
						case CX2Main::XS_DUNGEON_GAME:
						case CX2Main::XS_BATTLE_FIELD:
							{
								// 기존 HP 배율 1.5배 증가에서 2.0배 증가로 변경
								fMaxHpRel *= 2.0f;
							} break;
						default:
							break;
						}

						/// 쉴드 크기
						fShieldSize = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS, iSkillTempletLevel );
 					}

 						fStatScale *= fHyperModeStatScale;

 						pNPC->SetNPCStat(
 							pUser->GetMaxHp() * fMaxHpRel,
 							npcStat.m_fAtkPhysic	* fStatScale	* 1.f * fStatAtkRate,
 							npcStat.m_fAtkMagic		* fStatScale	* 1.f * fStatAtkRate,
 							npcStat.m_fDefPhysic	* fStatScale	* 1.f,
 							npcStat.m_fDefMagic		* fStatScale	* 0.8f );

						/// AT_SPECIAL 공격 배율 증가 버프 적용
						float fSummonMonsterPowerRate	= 1.f;
						fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );
						pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

 						const D3DXVECTOR3 vScale( pUser->GetScaleByUnit().x * fShieldSize / 450.f, pUser->GetScaleByUnit().x * fShieldSize / 450.f,
 							pUser->GetScaleByUnit().x * fShieldSize / 450.f);
 						pNPC->SetScaleByUnit( vScale );
						pNPC->SetDamageOutScreen( false );	// 데미지 숫자 표시 생략
						// 스킬레벨에 따라 다른 버프팩터를 적용하기 위해 레벨값 추가
						int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD );
						pNPC->SetRealatedSkillLevel(iSkillLevel);
 				} break;
				//}} JHKang / 강정훈 / 2010/11/22 / 이브 아토믹 쉴드





				// 레나 인탱글 트랩
			case CX2UnitManager::NUI_ENTANGLE_TRAP_RENA:
				{
					if( NULL != pUser->GetUnit() &&
						CX2Unit::UT_LIRE == pUser->GetUnit()->GetType() )
					{
						CX2GULire_ElvenRanger* pRena = (CX2GULire_ElvenRanger*) pUser;
						pNPC->SetEntangleTrapAttackDuration( pRena->GetEntangleTrapAttackDuration() );
						pRena->AddEntangleTrap( pNPC );
					}
				} break;

				// 레나 구속의 씨앗 소환 NPC
			case CX2UnitManager::NUI_RENA_ANGER_OF_ELF:
			case CX2UnitManager::NUI_RENA_ANGER_OF_ELF_DELAY:
				{
					float fNpcHp = 1000000.0f;
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						fNpcHp,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_LIRE == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_LIRE == pUser->GetUnit()->GetType() )
					{
						float fPowerRate = 1.f;
						int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LTR_HUNTERS_ABILITY, true );	//	사냥꾼의 재능
						if( iSkillLevel > 0 )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LTR_HUNTERS_ABILITY );

							if( NULL != pSkillTemplet )
							{
								fPowerRate *= ( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel ) );
							}
						}


						iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_RNW_ANGER_OF_ELF );	// 엘드랏실의 분노
						if( iSkillLevel > 0 )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_RNW_ANGER_OF_ELF );

							if( NULL != pSkillTemplet )
							{
								if ( NULL == pUser->GetUnit() )
									return;

								const int iSkillTempletLevel = max( 1, iSkillLevel );	/// 스킬 레벨

								fPowerRate *= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
							}
						}
						/// AT_SPECIAL 공격 배율 증가 버프 적용
						fPowerRate *= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );

						pNPC->SetDefaultPowerRate( fPowerRate );
						CX2GULire_ElvenRanger* pRena = static_cast<CX2GULire_ElvenRanger*>( pUser );
						pRena->AddAngerOfElfTrap( pNPC );
					}

				} break;






			case CX2UnitManager::NUI_UFO_AISHA:
				{
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADM_HELL_DROP );
						if( NULL != pSkillTemplet )
						{

							if ( NULL == pUser->GetUnit() )
								return;

							const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

							const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ADM_HELL_DROP ) );	/// 스킬 레벨

							float fSummonMonsterPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );		/// 스킬 레벨에 따른 공격 배율

							fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );	/// AT_SPECIAL 공격 배율 증가 버프 적용
							pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

							CX2GUArme_VioletMage* pAisha = (CX2GUArme_VioletMage*) pUser;
							pAisha->SetUFO( pNPC );
							CX2EffectSet::Handle hUfoEffect = GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Hell_Drop_Ring_And_Light", pNPC );
							pNPC->MakeEffectSetBeDeletedWhenDie( hUfoEffect );

							pNPC->ScaleMoveSpeed( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_REMOTE_SPEED_ABS, iSkillTempletLevel ) );
						}
					}
					else
					{
						pNPC->SetNowHp( 0.f );
					}
				} break;

			case CX2UnitManager::NUI_UFO_AISHA_DROPPER_MEMO:
				{
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADM_HELL_DROP );
						if( NULL != pSkillTemplet )
						{

							if ( NULL == pUser->GetUnit() )
								return;

							const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

							const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ADM_HELL_DROP ) );	/// 스킬 레벨

							float fSummonMonsterPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );		/// 스킬 레벨에 따른 공격 배율

							fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );	/// AT_SPECIAL 공격 배율 증가 버프 적용
							pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

							CX2GUArme_VioletMage* pAisha = (CX2GUArme_VioletMage*) pUser;
							pAisha->SetUFO( pNPC );
							CX2EffectSet::Handle hUfoEffect = GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Hell_Drop_Ring_And_Light", pNPC );
							pNPC->MakeEffectSetBeDeletedWhenDie( hUfoEffect );
						}
					}
					else
					{
						pNPC->SetNowHp( 0.f );
					}
				} break;

			//case CX2UnitManager::NUI_PIG_BAT_AISHA:
			//	{
			//		ASSERT( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() );
			//		if( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() )
			//		{
			//			const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AVP_SUMMON_BAT );
			//			if( NULL != pSkillTemplet )
			//			{
			//				pNPC->SetDefaultPowerRate( pSkillTemplet->m_fPowerRate * fHyperModeStatScale );

			//				CX2GUArme_VioletMage* pAisha = (CX2GUArme_VioletMage*) pUser;
			//				pAisha->AddPigBat( pNPC->GetUID(), pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME ) );
			//			}
			//		}
			//		else
			//		{
			//			pNPC->SetNowHP( 0.f );
			//		}
			//	} break;

			case CX2UnitManager::NUI_STONE_WALL_AISHA:
				{
					float fHPScale = 1.f;
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AEM_STONE_WALL );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
						return;

						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AEM_STONE_WALL ) );	/// 스킬 레벨

						fHPScale = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_ABS, iSkillTempletLevel );
						fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );

						fHPScale *= fHyperModeStatScale;
						fStatScale *= fHyperModeStatScale;

						float fNpcHp = pUser->GetUnitLevel()	* fHPScale;
						if( GetGameType() == GT_PVP )
						{
							fNpcHp = pNPC->GetStat().GetStat().m_fBaseHP;
						}

						pNPC->SetNPCStat(
							fNpcHp,
							npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
							npcStat.m_fAtkMagic		* fStatScale	* 1.f,
							pNPC->GetStat().GetStat().m_fDefPhysic	* 1.f,
							pNPC->GetStat().GetStat().m_fDefMagic		* 1.f );

						ASSERT( CX2Unit::UT_ARME == pUser->GetUnit()->GetUnitTemplet()->m_UnitType );
						if( CX2Unit::UT_ARME == pUser->GetUnit()->GetUnitTemplet()->m_UnitType )
						{
							CX2GUArme_VioletMage* pAisha = (CX2GUArme_VioletMage*) pUser;
							pAisha->AddStoneWall( pNPC->GetUID(), pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) );
						}
						else
						{
							pNPC->SetNowHp( 0.f );
						}
					}
				} break;


			case CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER :
				{
					// 아틸러리 스트라이크 퀀텀 발리스타 용 NPC
					// 해당 NPC 는 Targeting_Chung.lua 파일을 사용 합니다.
					// Flag 1 이 False 일 때는 락 온 이후가 상대방에게 보이고
					// True 일 때는 안보인다.

					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA );

						if( NULL != pSkillTemplet )
						{
							if ( NULL == pUser->GetUnit() )
								return;

							const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
							const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA ) );
							float fSummonMonsterPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
							fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );
							pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );
							CX2GUChung* pChung = static_cast<CX2GUChung*> ( pUser );
							pChung->SetShot( pNPC->GetUID() );
							pNPC->SetFlag_LUA( 1, true );
						}
					}
				} break;

				case CX2UnitManager::NUI_TARGETING_CHUNG:
				{
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() )
					{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE );
						if( NULL != pSkillTemplet )
						{

							if ( NULL == pUser->GetUnit() )
								return;

							const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

							const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE ) );	/// 스킬 레벨

							float fSummonMonsterPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );		/// 스킬 레벨에 따른 공격 배율

							fSummonMonsterPowerRate			*= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );	/// AT_SPECIAL 공격 배율 증가 버프 적용
							pNPC->SetDefaultPowerRate( fSummonMonsterPowerRate );

							CX2GUChung* pChung = (CX2GUChung*) pUser;
							pChung->SetShot( pNPC->GetUID() );

							// 해당 NPC 는 Targeting_Chung.lua 파일을 사용 합니다.
							// Flag 1 이 False 일 때는 락 온 이후가 상대방에게 보이고
							// True 일 때는 안보인다.
							pNPC->SetFlag_LUA( 1, false );
						}
					}
					else
					{
						pNPC->SetNowHp( 0.f );
					}
				} break;

			case CX2UnitManager::NUI_EVOKE_TRAP_LIRE:
			case CX2UnitManager::NUI_EVOKE_TRAP_LIRE_MEMO:
				{
					float fNpcHp = 1000000.0f;
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						fNpcHp,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_LIRE == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_LIRE == pUser->GetUnit()->GetType() )
					{
						int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_LTR_HUNTERS_ABILITY, true );	//	사냥꾼의 재능

						float fPowerRate = 1.f;

						if( iSkillLevel > 0 )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_LTR_HUNTERS_ABILITY );

							if( NULL != pSkillTemplet )
							{
								fPowerRate *= ( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel ) );
							}
						}

						iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_LTR_EVOKE );	//	이보크

						if( iSkillLevel > 0 )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_LTR_EVOKE );
							if( NULL != pSkillTemplet )
							{
								if ( NULL == pUser->GetUnit() )
									return;

								const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

								const int iSkillLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_LTR_EVOKE ) );	/// 스킬 레벨

								fPowerRate *= pSkillTemplet->GetSkillPowerRateValue( iSkillLevel );
							}
						}

						/// AT_SPECIAL 공격 배율 증가 버프 적용
						fPowerRate *= pUser->GetAdditionalAttackByType( CX2DamageManager::AT_SPECIAL );

						pNPC->SetDefaultPowerRate( fPowerRate );
						CX2GULire_ElvenRanger* pLire = static_cast<CX2GULire_ElvenRanger*>( pUser );
						pLire->InsertEvokeNpc( pNPC );
					}
					else
					{
						pNPC->SetNowHp( 0.f );
					}
				} break;
			case CX2UnitManager::NUI_ADW_DISTORTION:
				{
					fStatScale *= fHyperModeStatScale;

					pNPC->SetNPCStat(
						npcStat.m_fBaseHP		* fStatScale	* 1.f,
						npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
						npcStat.m_fAtkMagic		* fStatScale	* 1.f,
						npcStat.m_fDefPhysic	* fStatScale	* 1.f,
						npcStat.m_fDefMagic		* fStatScale	* 1.f );


					ASSERT( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_ARME == pUser->GetUnit()->GetType() )
					{
						const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADW_DISTORTION );
						if( NULL != pSkillTemplet )
						{
							if ( NULL == pUser->GetUnit() )
								return;

							const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

							const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ADW_DISTORTION ) );	/// 스킬 레벨

							pNPC->SetDefaultPowerRate( pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel ) );

							// 몬스터 소환 유지 시간
							float fRemainTime = static_cast<float>(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ));
							pNPC->StartSelfDestruction(fRemainTime);


						}
					}
					else
					{
						pNPC->SetNowHp( 0.f );
					}
				} break;

			case CX2UnitManager::NUI_REMOTE_MORTAR:		/// 청 2차 전직 택티컬 트루퍼 - 박격포
				{
					fStatScale *= fHyperModeStatScale;

					/// 공격력만 유저의 능력치를 받아옴
					pNPC->SetNPCStat(
						pNPC->GetStat().GetStat().m_fBaseHP,
						npcStat.m_fAtkPhysic * fStatScale	* 1.f,
						npcStat.m_fAtkMagic * fStatScale	* 1.f,
						pNPC->GetStat().GetStat().m_fDefPhysic,
						pNPC->GetStat().GetStat().m_fDefMagic);

					ASSERT( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() );
					if( CX2Unit::UT_CHUNG == pUser->GetUnit()->GetType() )
					{
						float fPowerRate = 1.f;
						int   iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_CTT_AUTOMATIC_MORTAR );

						if( iSkillLevel > 0 )
						{
							const CX2SkillTree::SkillTemplet* pSkillTemplet = pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CTT_AUTOMATIC_MORTAR );

							if( NULL != pSkillTemplet )
							{
								if ( NULL == pUser->GetUnit() )
									return;

								const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;

								const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_CTT_AUTOMATIC_MORTAR ) );	/// 스킬 레벨

								fPowerRate *= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );

								fPowerRate -= 1.f;
							}

						}

						pNPC->SetDefaultPowerRate( fPowerRate );

						pNPC->SetNowHp( pNPC->GetMaxHp() );

						CX2GUChung* pChung = static_cast<CX2GUChung*>( pUser );

						if( NULL != pChung )
							pChung->InsertRemoteMortarNPCUID( static_cast<UidType>( pNPC->GetUID() ) );
					}
					else
					{
						pNPC->SetNowHp( 0.f );
					}
				} break;

			case CX2UnitManager::NUI_ELESIS_FIRE_BLOSSOM:		/// 블레이징 하트 - 불꽃 개화.
				{
					if( NULL != pUser->GetUnit() && pUser->GetUnitClass() == CX2Unit::UC_ELESIS_BLAZING_HEART )
					{
						const int iSkillLevel = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_SBH_FIRE_BLOSSOMS, true );
						pNPC->SetRealatedSkillLevel(iSkillLevel);
					}
				} break;
			case CX2UnitManager::NUI_PARTICLE_PRISM_START: // 파티클 프리즘
			case CX2UnitManager::NUI_PARTICLE_PRISM_END:
				{
					float fHPScale = 1.f;
					const CX2SkillTree::SkillTemplet* pSkillTemplet =
						pUser->GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AN_PARTICLE_PRISM );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == pUser->GetUnit() )
							return;
						const CX2UserSkillTree& userSkillTree = pUser->GetUnit()->GetUnitData().m_UserSkillTree;
						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_AN_PARTICLE_PRISM ) );

						// 스탯 설정
						{
							/*fHPScale = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_HP_ABS, iSkillTempletLevel );
							if( 0 <= fHPScale )
							fHPScale = 1.f;

							fStatScale = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
							fHPScale *= fHyperModeStatScale;
							fStatScale *= fHyperModeStatScale;
							float fNpcHp = pUser->GetUnitLevel() * fHPScale;
							if( GetGameType() == GT_PVP )
							{
								fNpcHp = pNPC->GetStat().GetStat().m_fBaseHP;
							}	*/

							const float fHPRate = 0.25f;	/// 시전자 체력의 25%

							pNPC->SetNPCStat(
								pUser->GetMaxHp() * fHPRate,
								npcStat.m_fAtkPhysic	* fStatScale	* 1.f,
								npcStat.m_fAtkMagic		* fStatScale	* 1.f,
								pNPC->GetStat().GetStat().m_fDefPhysic	* 1.f,
								pNPC->GetStat().GetStat().m_fDefMagic		* 1.f );
						}


						// 몬스터 유지 시간 설정
						{
							const float fNpcDurationTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
							pNPC->StartSelfDestruction( fNpcDurationTime );


							if( NULL != pNPC->GetOwnerGameUnit() &&
								CX2Unit::UT_ADD == pNPC->GetOwnerGameUnit()->GetUnitType() )
							{
								CX2GUAdd* pGUAdd = static_cast<CX2GUAdd*>(pNPC->GetOwnerGameUnit());
								if( NULL != pGUAdd )
								{
									// 이펙트 유지시간 설정
									pNPC->PushEffectSetToDeleteOnDie(pGUAdd->GetParticlePrismHandle());
								}
							}
						}

					}
				} break;
			}
		}
	}
}


//void CX2Game::OpenMissionOpening( bool bOpen )
//{
//	if( true == bOpen )
//	{
//		if( NULL == m_pDLGMissionOpening )
//		{
//			m_pDLGMissionOpening		= new CKTDGUIDialog( this, L"DLG_Mission_Opening_Text.lua" );
//			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMissionOpening );
//		}
//
//		m_pDLGMissionOpening->SetShowEnable( true, true );
//
//		m_pDLGMissionOpening->Move( D3DXVECTOR2( -1024, 0 ), D3DXCOLOR(1,1,1,1), 0.01f, true );
//		m_pDLGMissionOpening->Move( D3DXVECTOR2( 0, 0 ), D3DXCOLOR(1,1,1,1), 1.f, false );
//		m_pDLGMissionOpening->Move( D3DXVECTOR2( 0, 0 ), D3DXCOLOR(1,1,1,1), 5.f, false );
//		m_pDLGMissionOpening->Move( D3DXVECTOR2( 1024, 0 ), D3DXCOLOR(1,1,1,1), 1.f, false );
//	}
//	else
//	{
//		if( NULL != m_pDLGMissionOpening )
//		{
//			m_pDLGMissionOpening->SetShowEnable( false, false );
//		}
//	}
//}
//
//void CX2Game::UpdateMissionOpening( const WCHAR* wszTitle, const WCHAR* wszReward )
//{
//	if( NULL == m_pDLGMissionOpening )
//		return;
//
//	CKTDGUIStatic* pStatic_Title = (CKTDGUIStatic*) m_pDLGMissionOpening->GetControl( L"StaticMission_Title" );
//	CKTDGUIStatic* pStatic_Reward = (CKTDGUIStatic*) m_pDLGMissionOpening->GetControl( L"StaticMission_Compensation" );
//
//
//	if( NULL != pStatic_Title )
//	{
//		if( NULL != wszTitle )
//		{
//			pStatic_Title->GetString(0)->msg = wszTitle;
//		}
//		else
//		{
//			pStatic_Title->GetString(0)->msg = L"";
//		}
//	}
//
//
//	if( NULL != pStatic_Reward )
//	{
//		if( NULL != wszReward )
//		{
//			pStatic_Reward->GetString(0)->msg = wszReward;
//		}
//		else
//		{
//			pStatic_Reward->GetString(0)->msg = L"";
//		}
//	}
//
//}
//
//
//
//void CX2Game::OpenMissionProgress( bool bOpen )
//{
//	if( true == bOpen )
//	{
//		if( NULL == m_pDLGMissionProgress )
//		{
//			m_pDLGMissionProgress		= new CKTDGUIDialog( this, L"DLG_Mission_Opening_Text.lua" );
//			g_pKTDXApp->GetDGManager()->GetDialogManager()->AddDlg( m_pDLGMissionProgress );
//		}
//
//		m_pDLGMissionProgress->SetShowEnable( true, true );
//
//		if( NULL != m_pMiniMap )
//		{
//			if( true == m_pMiniMap->GetMinimized() )
//			{
//				m_pDLGMissionProgress->SetPos( D3DXVECTOR2(-110, 0) );
//			}
//			else
//			{
//				m_pDLGMissionProgress->SetPos( D3DXVECTOR2(0, 0) );
//			}
//		}
//		else
//		{
//			m_pDLGMissionProgress->SetPos( D3DXVECTOR2( 0, 0 ) ); // 대전 게임중
//		}
//	}
//	else
//	{
//		if( NULL != m_pDLGMissionProgress )
//		{
//			m_pDLGMissionProgress->SetShowEnable( false, false );
//		}
//	}
//}
//
//
//void CX2Game::UpdateMissionProgress( bool bComplete, const WCHAR* wszTitle, const WCHAR* wszReward )
//{
//	if( NULL == m_pDLGMissionProgress )
//		return;
//
//	CKTDGUIStatic* pStatic_Title_OneLine = (CKTDGUIStatic*) m_pDLGMissionProgress->GetControl( L"StaticMission_Title_OneLine" );
//	CKTDGUIStatic* pStatic_Title_TwoLine = (CKTDGUIStatic*) m_pDLGMissionProgress->GetControl( L"StaticMission_Title_TwoLine" );
//	CKTDGUIStatic* pStatic_Reward = (CKTDGUIStatic*) m_pDLGMissionProgress->GetControl( L"StaticCompensation" );
//
//	CKTDGUIStatic* pStatic_Success = (CKTDGUIStatic*) m_pDLGMissionProgress->GetControl( L"StaticMission_Clear" );
//	CKTDGUIStatic* pStatic_Fail = (CKTDGUIStatic*) m_pDLGMissionProgress->GetControl( L"StaticMission_Failure" );
//
//
//
//	// fix!! 일단 안보이게
//	if( NULL != pStatic_Title_TwoLine )
//	{
//		pStatic_Title_TwoLine->SetShowEnable( false, false );
//	}
//
//	if( NULL != pStatic_Title_OneLine )
//	{
//		pStatic_Title_OneLine->SetShowEnable( true, true );
//		if( NULL != wszTitle )
//		{
//			pStatic_Title_OneLine->GetString(0)->msg = wszTitle;
//		}
//	}
//
//	if( NULL != pStatic_Reward )
//	{
//		if( NULL != wszReward )
//		{
//			pStatic_Reward->GetString(0)->msg = wszReward;
//		}
//	}
//
//	if( true == bComplete )
//	{
//		if( NULL != pStatic_Success )
//		{
//			pStatic_Success->SetShowEnable( true, true );
//		}
//
//		if( NULL != pStatic_Fail )
//		{
//			pStatic_Fail->SetShowEnable( false, false );
//		}
//	}
//	else
//	{
//		if( NULL != pStatic_Success )
//		{
//			pStatic_Success->SetShowEnable( false, false );
//		}
//
//		if( NULL != pStatic_Fail )
//		{
//			pStatic_Fail->SetShowEnable( true, true );
//		}
//	}
//}


// 던전의 몬스터에만 적용된다.
void CX2Game::BuffAllNPC( int statType, float fStatVal, float fStatTime )
{
	if( false == IsHost() )
		return;



	for( int i=0; i<GetNPCUnitListSize(); i++ )
	{
		CX2GUNPC* pCX2GUNPC = GetNPCUnit(i);
		if( NULL == pCX2GUNPC )
			continue;

		if( pCX2GUNPC->GetNowHp() <= 0.f ||
			pCX2GUNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
		{
			continue;
		}

		// 이브의 소환몬스터와 같이 우리편 NPC에게는 적용되지 않는다.
		if( NULL != pCX2GUNPC->GetNPCAI() &&
			CX2NPCAI::NAT_ALLY == pCX2GUNPC->GetNPCAI()->GetAIType() )
		{
			continue;
		}


		//if( NULL != pCX2GUNPC->GetNPCTemplet() )
		{
			switch( pCX2GUNPC->GetNPCTemplet().m_ClassType )
			{
			case CX2UnitManager::NCT_THING_GATE:
			case CX2UnitManager::NCT_THING_BOX:
			case CX2UnitManager::NCT_THING_TRAP:
			case CX2UnitManager::NCT_THING_DEVICE:
			case CX2UnitManager::NCT_THING_HOUSE:
			case CX2UnitManager::NCT_THING_WORLD_BLOCK:
			case CX2UnitManager::NCT_THING_CHECKER:
			//{{ JHKang / 강정훈 / 2010/12/07 / 버프 및 디버프 받지 않는 NPC TYPE
			case CX2UnitManager::NCT_THING_NOBUFF:		/// 버프/디버프에 영향을 받지 않음()
			//}} JHKang / 강정훈 / 2010/12/07 / 버프 및 디버프 받지 않는 NPC TYPE
			{
					continue;
				} break;

			default:
				{
					// nothing
				} break;
			}
		}





		pCX2GUNPC->AddOnStatReq( (KEGS_ADD_ON_STAT_REQ::ADD_ON_STAT) statType, fStatVal, fStatTime );


		if( fStatVal > 0.f )
		{
			switch( (KEGS_ADD_ON_STAT_REQ::ADD_ON_STAT) statType )
			{
			case KEGS_ADD_ON_STAT_REQ::AOS_ATK_PHYSIC:
				{
					D3DXVECTOR3 mypos = pCX2GUNPC->GetPos();
					mypos.y += 100.0f;
					mypos.z -= 20.f;
					CKTDGParticleSystem::CParticleEventSequence* pBuffPart = GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pCX2GUNPC ),  L"BuffAttackCore", mypos );
					pBuffPart->SetBlackHolePosition( mypos );

				} break;
			case KEGS_ADD_ON_STAT_REQ::AOS_ATK_MAGIC:
				{
					D3DXVECTOR3 mypos = pCX2GUNPC->GetPos();
					mypos.y += 100.0f;
					mypos.z -= 20.f;
					CKTDGParticleSystem::CParticleEventSequence* pBuffPart = GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pCX2GUNPC ),  L"BuffMagickCore", mypos );
					pBuffPart->SetBlackHolePosition( mypos );

				} break;

			case KEGS_ADD_ON_STAT_REQ::AOS_DEF_PHYSIC:
			case KEGS_ADD_ON_STAT_REQ::AOS_DEF_MAGIC:
				{
					D3DXVECTOR3 mypos = pCX2GUNPC->GetPos();
					mypos.y += 100.0f;
					mypos.z -= 20.f;
					CKTDGParticleSystem::CParticleEventSequence* pBuffPart = GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( pCX2GUNPC ),  L"ShieldBuffCore", mypos );
					pBuffPart->SetBlackHolePosition( mypos );

				} break;

			default:
				{
					// nothing
				} break;
			}
		}
	}
}

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//void CX2Game::BroadCast_XPT_UNIT_NPC_SYNC_PACK()
//{
//	KSerBuffer buff;
//	Serialize( &buff, &m_kXPT_UNIT_NPC_SYNC_PACK );
//	g_pData->GetGameUDP()->BroadCast( m_vecUserUIDforSyncPacket, XPT_UNIT_NPC_SYNC_PACK, (char*)buff.GetData(), buff.GetLength() );
//
//	m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.resize(0);
//}
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

#if X2_DEV_TOOLS
	void CX2Game::EnableManualCameraPosition( bool bEnabled )
	{
		if( NULL == GetX2Camera() )
			return;


		GetX2Camera()->SetEnabledManualCameraPosition( bEnabled );
		if( true == bEnabled )
		{
			if( true == m_bFreeCamera )
			{
				GetX2Camera()->CalcManualCameraPosition( GetFocusUnit(), *m_FPSCamera.GetEyePt() );
			}
			else
			{
				GetX2Camera()->CalcManualCameraPosition( GetFocusUnit(), GetX2Camera()->GetCamera().GetEye() );
			}
		}
	}

#endif // IN_GAME_MANUAL_CAMERA_POSITION_TEST








	void CX2Game::ReverseGravityFrameMove( double fTime, float fElapsedTime )
	{
		if( true == m_vecReverseGravityData.empty() )
			return;

		for( int i = 0; i<(int)m_vecReverseGravityData.size(); i++ )
		{
			ReverseGravityData& reverseGravity = m_vecReverseGravityData[i];
			reverseGravity.OnFrameMove( fTime, fElapsedTime );
			if( reverseGravity.m_fTimeLeft <= 0.f )
			{
				m_vecReverseGravityData.erase( m_vecReverseGravityData.begin() + i );
				--i;
				continue;
			}
		}
	}

    bool CX2Game::IsInReverseGravityRegion( float fElapsedTime, const D3DXVECTOR3& vPos, float& fSpeedYDelta, D3DXVECTOR3& vSpeedRotateDelta, int iTeamNumber )
	{
		fSpeedYDelta = 0.f;

		BOOST_TEST_FOREACH( ReverseGravityData&, reverseGravity, m_vecReverseGravityData )
		{
			if( reverseGravity.m_fTimeLeft <= 0.f )
				continue;

            reverseGravity.IsInRange( fElapsedTime, vPos, fSpeedYDelta, vSpeedRotateDelta, iTeamNumber );
		}

		if( fSpeedYDelta > 0.f )
			return true;
		else
			return false;
	}

	void CX2Game::AddReverseGravityRegion( const D3DXVECTOR3& vPos, const float fRange, const float fTime, const float fGravity, int iTeamNumber )
	{
		ASSERT( fGravity >= 0.f );

		ReverseGravityData reverseGravity;
		reverseGravity.m_vCenterPos = vPos;
		reverseGravity.m_fRange = fRange;
		reverseGravity.m_fTimeLeft = fTime;
		reverseGravity.m_fGravity = fGravity;
		reverseGravity.m_iTeamNumber = iTeamNumber;

		m_vecReverseGravityData.push_back( reverseGravity );
	}



	bool CX2Game::ReverseGravityData::IsInRange(
        float fElapsedTime,
        const D3DXVECTOR3& vPos, float& fSpeeYDelta, D3DXVECTOR3& vSpeedRotateDelta, int iTeamNumber )
	{
		if( m_fTimeLeft <= 0.f )
			return false;

		if( iTeamNumber == m_iTeamNumber )
			return false;

		if( g_pKTDXApp->GetCollision()->LinePointDistance( vPos, m_vCenterPos, D3DXVECTOR3(0, 1, 0 ) ) > m_fRange )
		//if( GetDistance( vPos, m_vCenterPos ) > m_fRange )
			return false;

		// oasis907 : 김상윤 [2011.6.29]
		vSpeedRotateDelta.y = vSpeedRotateDelta.y * 0.995f;

        fSpeeYDelta += m_fGravity * fElapsedTime;
		return true;
	}





//////////////////////////////////////////////////////////////////////////
CX2Game::InfoTextManager::InfoTextManager()
{
	m_iMaxLine = 4;
	m_vPos = D3DXVECTOR2( 512, 270 );
}

CX2Game::InfoTextManager::~InfoTextManager()
{
	ClearAllText();
}

void CX2Game::InfoTextManager::ClearAllText()
{
	for( UINT i=0; i<m_vecpText.size(); i++ )
	{
		CX2TextManager::CX2Text* pText = m_vecpText[i];
		g_pMain->GetTextManager()->DeleteText( pText );
	}
	m_vecpText.clear();
}

void CX2Game::InfoTextManager::RemoveText( CX2TextManager::CX2Text* pText )
{
	for( UINT i=0; i<m_vecpText.size(); i++ )
	{
		CX2TextManager::CX2Text* pTempText = m_vecpText[i];
		if( pText == pTempText )
		{
			m_vecpText.erase( m_vecpText.begin() + i );
			g_pMain->GetTextManager()->DeleteText( pText );
			break;
		}
	}
}

void CX2Game::InfoTextManager::MakeEmptyLine( bool bScrollUpside, int nCount )
{
	for( UINT i=0; i<m_vecpText.size(); i++ )
	{
		CX2TextManager::CX2Text* pText = m_vecpText[i];
		if( false == g_pMain->GetTextManager()->CheckText( pText ) )
		{
			m_vecpText.erase( m_vecpText.begin() + i );
			i--;
		}
	}

	//if( (int)m_vecpText.size() < m_iMaxLine )
	//	return;

	int iCountToRemove = nCount - ( m_iMaxLine - (int)m_vecpText.size() );
	while( m_vecpText.size() > 0 && iCountToRemove > 0)
	{
		CX2TextManager::CX2Text* pText = m_vecpText[0];
		g_pMain->GetTextManager()->DeleteText( pText );
		m_vecpText.erase( m_vecpText.begin() );
		iCountToRemove -= 1;
	}


	// 남아있는 텍스트들 위쪽으로 스크롤
	D3DXVECTOR2 vPos = m_vPos;
	for( UINT i=0; i<m_vecpText.size(); i++ )
	{
		CX2TextManager::CX2Text* pText = m_vecpText[i];
		float MAGIC_REMAIN_TIME = 3.f + (float) i / 4.f;
		//float fRemainTime = pText->GetChangeTime() - pText->GetAnimateTime();
		//if( fRemainTime < 0.f )
		//{
		//	fRemainTime = 0.1f;
		//}

		if( true == bScrollUpside )
		{
			vPos.y += pText->GetFont()->GetHeight() * 1.2f * pText->GetLineCount();
		}
		else
		{
			vPos.y -= pText->GetFont()->GetHeight() * 1.2f * pText->GetLineCount();
		}

		pText->Move( vPos, pText->GetTextMsg().c_str(), pText->GetGoalColor(), pText->GetGoalOutlineColor(), 0.01f, true, false );
		pText->Move( vPos, pText->GetTextMsg().c_str(), pText->GetGoalColor(), pText->GetGoalOutlineColor(), MAGIC_REMAIN_TIME, false, true );
	}
}

CX2TextManager::CX2Text* CX2Game::InfoTextManager::PushText( int fontIndex, const WCHAR* pTextMsg, D3DXCOLOR color, D3DXCOLOR outlineColor,
															DWORD dtType /*= DT_LEFT*/, float fLineSpace /*= 1.0f*/, float fLifeTime /*= 1.0f*/ )
{
	bool bScrollUpside = true;
	MakeEmptyLine( bScrollUpside, 1 );

	D3DXVECTOR2 vPos = m_vPos;
	if( m_vecpText.size() > 0 )
	{
		CX2TextManager::CX2Text* pText = m_vecpText[ m_vecpText.size()-1 ];
		vPos = pText->GetGoalPos();

		if( true == bScrollUpside )
		{
			vPos.y += pText->GetFont()->GetHeight() * 1.2f * pText->GetLineCount();
		}
		//else
		//{
		//	vPos.y -= pText->GetFont()->GetHeight() * 1.2f;
		//}
	}

	CX2TextManager::CX2Text* pText = g_pMain->GetTextManager()->AddText( fontIndex, vPos, pTextMsg, color, outlineColor, dtType, fLineSpace );
	if( false == bScrollUpside )
	{
		vPos.y -= pText->GetFont()->GetHeight() * 1.2f * pText->GetLineCount();
		pText->SetPos( vPos );
	}
	pText->Move( vPos, pTextMsg, color, outlineColor, fLifeTime, false, true );
	m_vecpText.push_back( pText );

	return pText;
}
void


CX2Game::ProcSpecialItem(
    const CX2Item::ItemTemplet* pItemTemplet,
    CX2GUUser* pCX2GUUser)
{
	if( pItemTemplet == NULL )
	{
		return;
	}


    switch( pItemTemplet->GetItemID() )
	{
	case DUNGEON_ITEM4_1:	// 몬스터 소환( 식목일 이벤트 )
		{
			if( IsHost() == true )
			{
				for( int i=0; i<GetNPCUnitListSize(); i++ )
				{
					CX2GUNPC* pCX2GUNPC = GetNPCUnit(i);
					if( pCX2GUNPC != NULL &&
						( pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_TREE_KNIGHT_EVENT ||
						pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_ENT_SMALL_EVENT ||
						pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == CX2UnitManager::NUI_ENT_EVENT ) )
					{
						pCX2GUNPC->SetNowHp(0.f);

					}
				}
			}

			if( pCX2GUUser != NULL && GetWorld() != NULL && GetWorld()->GetLineMap() != NULL )
			{
				const D3DXVECTOR3& vPos = pCX2GUUser->GetPos();
				D3DXVECTOR3 vPos1 = GetWorld()->GetLineMap()->GetRandomPosition( &vPos, 500.f, true );
				D3DXVECTOR3 vPos2 = GetWorld()->GetLineMap()->GetRandomPosition( &vPos, 500.f, true );
				D3DXVECTOR3 vPos3 = GetWorld()->GetLineMap()->GetRandomPosition( &vPos, 500.f, true );
				D3DXVECTOR3 vPos4 = GetWorld()->GetLineMap()->GetRandomPosition( &vPos, 300.f, true );
				D3DXVECTOR3 vPos5 = GetWorld()->GetLineMap()->GetRandomPosition( &vPos, 200.f, true );

				SummonMonster(pCX2GUUser, CX2UnitManager::NUI_TREE_KNIGHT_EVENT, vPos1 );
				SummonMonster(pCX2GUUser, CX2UnitManager::NUI_TREE_KNIGHT_EVENT, vPos2 );
				SummonMonster(pCX2GUUser, CX2UnitManager::NUI_TREE_KNIGHT_EVENT, vPos3 );
				SummonMonster(pCX2GUUser, CX2UnitManager::NUI_ENT_SMALL_EVENT, vPos4 );
				SummonMonster(pCX2GUUser, CX2UnitManager::NUI_ENT_EVENT, vPos5 );
			}
		}
		break;


	default:
		break;
	}

}

CX2Item::SPECIAL_ITEM_TYPE CX2Game::GetSpecialItemGroupType(const CX2Item::ItemTemplet* pItemTemplet)
{
	if( pItemTemplet == NULL )
		return CX2Item::SIT_NONE;
    DWORD   dwItemID = pItemTemplet->GetItemID();

	bool bNostrum = false;
	bool bSpirit = false;
	bool bCrest = false;

	// 몬스터 소환
	for(int i=0; i<ARRAY_SIZE(DUNGEON_ITEM_LIST4); ++i)
	{
		if( dwItemID == DUNGEON_ITEM_LIST4[i] )
		{
			return CX2Item::SIT_SUMMON_MONSTER;
		}
	}

	// 비약
	for(int i=0; i<ARRAY_SIZE(DUNGEON_ITEM_LIST1); ++i)
	{
		if( dwItemID == DUNGEON_ITEM_LIST1[i] )
		{
			bNostrum = true;
			return CX2Item::SIT_NOSTRUM;
		}
	}

	// 정령소환
	if(bNostrum == false)
	{
		for(int i=0; i<ARRAY_SIZE(DUNGEON_ITEM_LIST2); ++i)
		{
			if( dwItemID == DUNGEON_ITEM_LIST2[i] )
			{
				bSpirit = true;
				return CX2Item::SIT_GENIUS;
			}
		}
	}

	// 문장
	if(bNostrum == false && bSpirit == false)
	{
		for(int i=0; i<ARRAY_SIZE(DUNGEON_ITEM_LIST3); ++i)
		{
			if( dwItemID == DUNGEON_ITEM_LIST3[i] )
			{
				bCrest = true;
				return CX2Item::SIT_CREST;
			}
		}
	}

	return CX2Item::SIT_NONE;
}

wstring CX2Game::SummonSpiritEffectName(int iSpirit)
{
	wstring wstrSpiritEffectSet = L"";

	switch(iSpirit)
	{
	case 1:
		wstrSpiritEffectSet = L"EffectSet_Spirit_1";	// 오리에드의 분노
		break;
	case 2:
		wstrSpiritEffectSet = L"EffectSet_Spirit_2"; // 드라이어드의 분노
		break;
	case 3:
		wstrSpiritEffectSet = L"EffectSet_Spirit_3";		// 라울의 분노
		break;
	case 4:
		wstrSpiritEffectSet = L"EffectSet_Spirit_4";	// 일리피아의 손길
		break;
	default:
		wstrSpiritEffectSet = L"";
		break;
	}

	return wstrSpiritEffectSet;
}

void CX2Game::ClearSpirit()
{
	for(UINT i=0; i<m_vechEffectSetSpirit.size(); ++i)
	{
		CX2EffectSet::Handle hHandle = m_vechEffectSetSpirit[i];
		if( hHandle != INVALID_EFFECTSET_HANDLE )
			GetEffectSet()->StopEffectSet( hHandle );
	}

	m_vechEffectSetSpirit.clear();

	m_optrItemDamageEffectGameUnit.Reset();
	m_fItemDamageEffectTime = 0.f;
	m_iItemDamageEffectIndex = 0;
	m_vecEffectPos.clear();
}

void CX2Game::SideEffectGenius(CX2GUUser* pCX2GUUser)
{
	if( pCX2GUUser == NULL )
		return;

	GetEffectSet()->PlayEffectSet( L"Effect_Marker_SideEffect_Posion01", (CX2GameUnit*) pCX2GUUser );

	// 뽀루 소환
	int iTeam = pCX2GUUser->GetTeam();

	int iNpcLv = pCX2GUUser->GetUnit()->GetUnitData().m_Level;

	if( GetGameType() == GT_DUNGEON )
	{
		CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(this);
		if( NULL != pDungeonGame &&
			NULL != pDungeonGame->GetDungeon() &&
			NULL != pDungeonGame->GetDungeon()->GetDungeonData() )
		{
			const SEnum::DUNGEON_ID eDungeonId = pDungeonGame->GetDungeon()->GetDungeonData()->m_DungeonID;

			//{{ oasis907 : 김상윤 [2010.10.21] // 이벤트 던전에서는 사이드 이펙트 소환 몹 레벨 1로
			if ( CX2Dungeon::IsEventDungeon( eDungeonId ) )
				iNpcLv = 1;
			//}} oasis907 : 김상윤 [2010.10.21] // 	이벤트 던전에서는 사이드 이펙트 소환 몹 레벨 1로

			CreateNPCReq( CX2UnitManager::NUI_PPORU_WILLIAM_GENIUS, iNpcLv, false, pCX2GUUser->GetPos(),
				true, 0.f, true, -1, CX2Room::TN_MONSTER );
		}
	}
	else
	{
		if( g_pData->GetPVPRoom() != NULL && g_pData->GetPVPRoom()->GetPVPGameType() == CX2PVPRoom::PGT_SURVIVAL )
		{
			//서바이벌 모드에서는 몬스터 팀으로 생성
			CreateNPCReq( CX2UnitManager::NUI_PPORU_WILLIAM_GENIUS, iNpcLv, false, pCX2GUUser->GetPos(),
				true, 0.f, true, -1, CX2Room::TN_MONSTER );
		}
		else
		{
			if(iTeam == 0)
				iTeam = 1; // 블루 팀
			else if(iTeam == 1)
				iTeam = 0; // 레드 팀

			CreateNPCReq( CX2UnitManager::NUI_PPORU_WILLIAM_GENIUS, iNpcLv, false, pCX2GUUser->GetPos(),
				true, 0.f, true, -1, (CX2Room::TEAM_NUM)iTeam, CX2NPCAI::NAT_ALLY );
		}
	}

	pCX2GUUser->PlaySound( L"Summon_Fail.ogg" );

}

void CX2Game::SummonSpiritEffect(int iEffectIndex)
{
	if( null == m_optrItemDamageEffectGameUnit )
		return;

	if( m_optrItemDamageEffectGameUnit->GetNowHp() > 0.f)
	{
		D3DXVECTOR3 vPosEffectUser = m_optrItemDamageEffectGameUnit->GetPos();

		for( UINT i=0; i<m_UserUnitList.size(); i++ )
		{
			CX2GUUser* pUser = m_UserUnitList[i];
			if( pUser == NULL )
				continue;

			if( pUser->GetInvincible() == true || pUser->GetTeam() == m_optrItemDamageEffectGameUnit->GetTeam() )
				continue;

			D3DXVECTOR3 vPos = pUser->GetPos();

			if( GetDistance( vPos, vPosEffectUser ) > 1500.f )
				continue;

			m_vecEffectPos.push_back(vPos);
		}

		for( UINT i=0; i<m_NPCUnitList.size(); i++ )
		{
			CX2GUNPC* pNPC = m_NPCUnitList[i];
			if( pNPC == NULL )
				continue;

			//if( pNPC->GetNPCTemplet() == NULL )
			//	continue;

			if( pNPC->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
				continue;
			if( pNPC->GetInvincible() == true || pNPC->GetTeam() == m_optrItemDamageEffectGameUnit->GetTeam() )
				continue;

			D3DXVECTOR3 vPos = pNPC->GetPos();

			if( GetDistance( vPos, vPosEffectUser ) > 1500.f )
				continue;

			m_vecEffectPos.push_back(vPos);
		}
	}

	float fPowerRate = m_optrItemDamageEffectGameUnit->GetPowerRate();
	D3DXVECTOR3 vRotate = m_optrItemDamageEffectGameUnit->GetRotateDegree();

	wstring wstrEffectName = L"";
	if(iEffectIndex == 1)
	{
		wstrEffectName = L"SPIRIT_1_ATTACK";
	}
	else if(iEffectIndex == 3)
	{
		wstrEffectName = L"SPIRIT_3_ATTACK";
	}

	for(UINT i=0; i<m_vecEffectPos.size(); ++i)
	{
		D3DXVECTOR3 vPos = m_vecEffectPos[i];
		GetDamageEffect()->CreateInstance( static_cast<CX2GameUnit*>( m_optrItemDamageEffectGameUnit.GetObservable() ), wstrEffectName.c_str(), fPowerRate, vPos, vRotate, vRotate );

	}

}

void CX2Game::SpecialItemEffect(int iEffectIndex)
{
	if( null == m_optrItemDamageEffectGameUnit )
		return;

	if( m_optrItemDamageEffectGameUnit->GetNowHp() > 0.f )
	{
		D3DXVECTOR3 vPosEffectUser = m_optrItemDamageEffectGameUnit->GetPos();

		for( UINT i=0; i<m_UserUnitList.size(); i++ )
		{
			CX2GUUser* pUser = m_UserUnitList[i];
			if( pUser == NULL )
				continue;

			if( /*pUser->GetInvincible() == true ||*/ pUser->GetTeam() != m_optrItemDamageEffectGameUnit->GetTeam() )// 아군만
				continue;

			D3DXVECTOR3 vPos = pUser->GetPos();

			// 거리제한 없다
			//if( GetDistance( vPos, vPosEffectUser ) > 1500.f )
			//	continue;

			m_vecEffectPos.push_back(vPos);
			wstring wstrEffectName = L"";
			if(iEffectIndex == CX2Item::SST_WIND_SPHERE)	// 바람 구슬
			{
				wstrEffectName = L"EffectSet_SI_WindSphere";
			}
			GetEffectSet()->PlayEffectSet( wstrEffectName.c_str(), (CX2GameUnit*) pUser, NULL, false, -1.f, -1.f, D3DXVECTOR3(1.f, 1.f, 1.f) );

			// 버프를 걸어준다.
			pUser->StartSpecialItemBuffWind();
		}
	}
}

void CX2Game::SummonMonster(CX2GUUser* pCX2GUUser, CX2UnitManager::NPC_UNIT_ID npcId, D3DXVECTOR3 vPos)
{
	if( pCX2GUUser == NULL )
		return;

	// 몬스터 소환
	int iTeam = pCX2GUUser->GetTeam();

	int iNpcLv = 1; //pCX2GUUser->GetUnit()->GetUnitData().m_Level;
	if( GetGameType() == GT_DUNGEON )
	{
		//PushCreateNPCReq( npcId, iNpcLv, false, vPos, true, 1.f, true, -1, (CX2Room::TEAM_NUM)iTeam, CX2NPCAI::NAT_ALLY );
		CreateNPCReq( npcId, iNpcLv, false, vPos,
			true, 0.1f, true, -1, (CX2Room::TEAM_NUM)iTeam, CX2NPCAI::NAT_ALLY );

		g_pData->GetUIEffectSet()->PlayEffectSet( L"EffectSet_LevelUp_LandPosition", (CX2GameUnit*)pCX2GUUser, NULL, false, -1.f, -1.f, D3DXVECTOR3(1.f, 1.f, 1.f), true, vPos );
		g_pData->GetUIEffectSet()->PlayEffectSet( L"EffectSet_LevelUp",  (CX2GameUnit*)pCX2GUUser, NULL, false, -1.f, -1.f, D3DXVECTOR3(1.f, 1.f, 1.f), true, vPos );
	}
}



bool CX2Game::Handler_EGS_WORLD_TRIGGER_RELOCATION_REQ( int iTriggerId )
{
	if( GetWorld() != NULL && GetWorld()->CanActiveTrigger(iTriggerId) == false )
		return true;

	KEGS_WORLD_TRIGGER_RELOCATION_REQ kEGS_WORLD_TRIGGER_RELOCATION_REQ;
	kEGS_WORLD_TRIGGER_RELOCATION_REQ.m_iWorldTrigger = iTriggerId;

	g_pData->GetServerProtocol()->SendPacket( EGS_WORLD_TRIGGER_RELOCATION_REQ, kEGS_WORLD_TRIGGER_RELOCATION_REQ );
	return true;
}
bool CX2Game::Handler_EGS_WORLD_TRIGGER_RELOCATION_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_WORLD_TRIGGER_RELOCATION_ACK kEvent;
	DeSerialize( pBuff, &kEvent );

	if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
	{
		return true;
	}

	return false;
}
bool CX2Game::Handler_EGS_WORLD_TRIGGER_RELOCATION_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_WORLD_TRIGGER_RELOCATION_NOT kEvent;
	DeSerialize( pBuff, &kEvent );

	if( kEvent.m_iWorldTrigger >= 0 && GetWorld() != NULL )
	{
		GetWorld()->ActiveTrigger(kEvent.m_iWorldTrigger);
	}

	return true;
}

//{{ 허상형 : [2011/3/4/] //	월드 미션
float CX2Game::GetDungeonProgress()
{
	CX2DungeonGame* pDungeonGame = (CX2DungeonGame*)this;

	if( pDungeonGame == NULL )
	{
		return 0.0f;
	}

	float fTimeLimit = pDungeonGame->GetPlayTimeLimit();
	float fTimeRemain = pDungeonGame->GetRemainPlayTime();

	if( fTimeLimit <= 0 )	return 0.0f;

	float fProgress = 1 - ( fTimeRemain / fTimeLimit );

	if( fProgress > 1.0f ) fProgress = 1.0f;
	else if( fProgress < 0 ) fProgress = 0.0f;

	return fProgress;
}

int	CX2Game::GetNPCUnitNumByNPCID( int iNPCID )
{
	int num = 0;
	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		if( m_NPCUnitList[i] != NULL )
		{
			CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
			if( pCX2GUNPC != NULL && pCX2GUNPC->GetNPCTemplet().m_nNPCUnitID == iNPCID )
			{
				num++;
			}
		}
	}

	return num;
}
//}} 허상형 : [2011/3/4/] //	월드 미션

int CX2Game::GetStartSecretStageEnteringEvent()
{
	if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
	{
		CX2StateDungeonGame* pStateDungeonGame = (CX2StateDungeonGame*) g_pMain->GetNowState();
		return pStateDungeonGame->GetStartSecretStageEnteringEvent();
	}
	return 0;
}

bool CX2Game::SecretDungeonStageLoadReq(int iVal)
{
	if( CX2Main::XS_DUNGEON_GAME == g_pMain->GetNowStateID() )
	{
		CX2StateDungeonGame* pStateDungeonGame = (CX2StateDungeonGame*) g_pMain->GetNowState();
		return pStateDungeonGame->SecretDungeonStageLoadReq(iVal);
	}
	return false;
}

bool CX2Game::IsFront( CX2GameUnit *pMyUnit, CX2GameUnit *pTargetUnit )
{
	if( pMyUnit == NULL || pTargetUnit == NULL )
		return false;

	D3DXVECTOR3 vDir = pTargetUnit->GetPos() - pMyUnit->GetPos();
    vDir.y = 0.f;
	if( D3DXVec3Dot( &vDir, &pTargetUnit->GetDirVector() ) > 0.f )
	{
		if( false == pMyUnit->GetIsRight() )
			return false;
	}
	else
	{
		if( true == pMyUnit->GetIsRight() )
			return false;
	}

	return true;
}

void CX2Game::SendNpcUnitFirstSyncPacketImmediateForce( vector<UidType>& vecNonNpcSyncUserList_, OUT KEGS_UPDATE_NPC_UNIT_BUFF_INFO_NOT& kPacket_ )
{
	// 호스트인 경우
	if( IsHost() == true )
	{
		// 전체 NPC의 패킷을 보내고... (pLastUnit만 보내면 되는거 아닌가??)
		//KXPT_UNIT_NPC_SYNC_PACK kXPT_UNIT_NPC_SYNC_PACK;

//#ifdef  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND

		for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
		{
			CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
			if( pCX2GUNPC != NULL )
			{
				pCX2GUNPC->GetBuffInfo( kPacket_.m_vecNpcUnitBuff );
			}
		}

//#else   SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
//
////#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//        CX2FrameUDPPack   kFrameUDPPack;
//        if ( GetMyUnit() != NULL )
//            kFrameUDPPack.SetUnitUID( GetMyUnit()->GetUnitUID() );
//        kFrameUDPPack.SetFrameMoveCount( m_kFrameUDPPack.GetFrameMoveCount() );
//		kFrameUDPPack.ResetFrameUDPPack( true );
//        kFrameUDPPack.UpdateSyncUserListAndUDPMaxSize( vecNonNpcSyncUserList_ );
////#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//		for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
//		{
//			CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
//			if( pCX2GUNPC != NULL )
//			{
////#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//				pCX2GUNPC->SendPacketImmediateForce( kFrameUDPPack );
////#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
////				pCX2GUNPC->SendPacketImmediateForce( m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList );
////#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//				pCX2GUNPC->GetBuffInfo( kPacket_.m_vecNpcUnitBuff );
//			}
//
////#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
////			if ( sizeof(KXPT_UNIT_NPC_SYNC) * (m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.size() + 1) > 500 )
////			{
////				KSerBuffer buff;
////				Serialize( &buff, &m_kXPT_UNIT_NPC_SYNC_PACK );
////				g_pData->GetGameUDP()->BroadCast( vecNonNpcSyncUserList_, XPT_UNIT_NPC_FIRST_SYNC_PACK_BY_BATTLE_FIELD, (char*)buff.GetData(), buff.GetLength() );
////
////				m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.resize(0);
////			}
////#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//		}
//
////#ifdef  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//        kFrameUDPPack.FlushFrameUDPPack();
////#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
////		KSerBuffer buff;
////		Serialize( &buff, &m_kXPT_UNIT_NPC_SYNC_PACK );
////		g_pData->GetGameUDP()->BroadCast( vecNonNpcSyncUserList_, XPT_UNIT_NPC_FIRST_SYNC_PACK_BY_BATTLE_FIELD, (char*)buff.GetData(), buff.GetLength() );
////		m_kXPT_UNIT_NPC_SYNC_PACK.unitNPCSyncList.resize(0);
////#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//
//#endif  SERV_OPTIMIZE_ROBUST_USER_NPC_PACKET_SEND
	}
}

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//void CX2Game::SetPetFirstSyncPacketImmediateForce( vector<UidType>& vecNonNpcSyncUserList_ )
//{
//	// 호스트인 경우
//	if( IsHost() == true )
//	{
//#ifdef SERV_PET_SYSTEM
//		for( UINT i=0; i<g_pData->GetPetManager()->GetPetNum(); ++i )
//		{
//			CX2PET *pPet = g_pData->GetPetManager()->GetPetInx(i);
//			if( pPet != NULL )
//			{
//				pPet->SetSendReserveStateChange( true );
//				pPet->SendPacketImmediateForce( m_kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList );
//			}
//
//			if( m_kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList.size() > 0 )
//			{
//				KSerBuffer buff;
//				Serialize( &buff, &m_kXPT_UNIT_PET_SYNC_PACK );
//				g_pData->GetGameUDP()->BroadCast( vecNonNpcSyncUserList_, XPT_UNIT_PET_FIRST_SYNC_PACK_BY_BATTLE_FIELD, (char*)buff.GetData(), buff.GetLength() );
//
//				m_kXPT_UNIT_PET_SYNC_PACK.unitPetSyncList.resize(0);
//			}
//		}
//#endif
//
//	}
//}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

CX2GameUnit* CX2Game::GetNearestUnit( CX2GameUnit* pGameUnit, int iFlag )
{
	// iFlag 0 : 유저만
	// iFlag 1 : 몬스터만
	// iFlag 2 : 전체유닛대상
	float fDist = 10000.f * 10000.f;
	CX2GameUnit *pNearestUnit = NULL;

	CX2GameUnit *pMyUnit = pGameUnit;

	if( pGameUnit == NULL )
	{
		if ( NULL != GetMyUnit() )
			pMyUnit = GetMyUnit();
		else
			return pNearestUnit;
	}

	int iMyTeam = pMyUnit->GetTeam();
	if( iMyTeam < 0 )
		iMyTeam = pMyUnit->GetAllyTeam();

	for(UINT i=0; i<m_UnitList.size(); ++i)
	{
		CX2GameUnit *pUnit = m_UnitList[i];

		if( pUnit->GetTeam() == iMyTeam )
			continue;
		if( pUnit->GetNowHp() <= 0.f )
			continue;
		if( pUnit->GetTeam() < 0 )
			continue;
		if( pUnit->GetUnitUID() == pMyUnit->GetUnitUID() )
			continue;

		if( pUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
		{
			if( iFlag == 0 )
				continue;

			CX2GUNPC *pNpc = (CX2GUNPC*)pUnit;
			if( pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC &&
				pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_THING_GATE )
				continue;

			if( pNpc->GetInvincible() == true )
				continue;
		}
		else if( iFlag == 1 )
			continue;

		D3DXVECTOR3 vDist = pMyUnit->GetPos() - pUnit->GetPos();
		float fLen = D3DXVec3LengthSq( &vDist );
		if( fLen < fDist )
		{
			pNearestUnit = pUnit;
			fDist = fLen;
		}
	}

	return pNearestUnit;
}


void CX2Game::ShowDangerAlarm_LUA( int iAlarmId, float fShowTime, ALARM_COLOR_TYPE eColor, bool bDanger, int iStringIndex )
{
	if( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
		return;

	int iAlarmIndex = GetDangerAlarm( -1, iAlarmId );
	if( iAlarmIndex < 0 )
	{
		DangerAlarm *pAlarm = new DangerAlarm();
		pAlarm->m_uid = -1;
		pAlarm->m_iStateId = iAlarmId;
		pAlarm->m_fDelayTime = 0.f;
		pAlarm->m_fShowTime = fShowTime;
		pAlarm->m_eColor = eColor;
		pAlarm->m_bDanger = bDanger;
		pAlarm->m_bRepeat = true;

		wstring wstrComment = GET_STRING( iStringIndex );
		pAlarm->m_wstrMessage = wstrComment;

		m_vecDangerAlarm.push_back( pAlarm );
		iAlarmIndex = m_vecDangerAlarm.size() - 1;
	}

	ShowDangerAlarm( iAlarmIndex );
}

void CX2Game::ShowDangerAlarm( int iIndex )
{
	if( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
		return;

	if( iIndex < 0 )
		return;
	DangerAlarm *pAlarm = m_vecDangerAlarm[iIndex];
	if( pAlarm == NULL )
		return;

	CKTDGParticleSystem::CParticleEventSequence* pSeq = GetMajorParticle()->GetInstanceSequence( m_hDangerAlarm );
	if( pSeq != NULL )
	{
		pSeq->SetShowObject(false);
	}

	m_fShowAlarmTime = 0.f;
	m_pShowAlarm = pAlarm;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == false )
	{
		if( m_pShowAlarm->m_bDanger == true )
		{
			if( pSeq != NULL )
			{
				pSeq->SetShowObject(true);
			}
			else
			{
				m_hDangerAlarm = GetMajorParticle()->CreateSequenceHandle( NULL, L"danger_alarm_01", 0.0f, 0.0f, 0.0f );
			}
		}
	}

}

int CX2Game::LoadDangerAlarm( KLuaManager &luaManager, UidType iUid, int iStateId )
{
	int iRet = -1;
	DangerAlarm *pDangerAlarm = NULL;
	if( luaManager.BeginTable( "ALARM" ) == true )
	{
		pDangerAlarm = new DangerAlarm();

		int iStringIndex;

		LUA_GET_VALUE( luaManager, "DANGER",		pDangerAlarm->m_bDanger,	false );
		LUA_GET_VALUE( luaManager, "ALARM_MESSAGE",	iStringIndex,				STR_ID_EMPTY );
		LUA_GET_VALUE( luaManager, "DELAY",			pDangerAlarm->m_fDelayTime,	0.f );
		LUA_GET_VALUE( luaManager, "DISAPPEAR_TIME",pDangerAlarm->m_fShowTime,	3.f );
		LUA_GET_VALUE( luaManager, "REPEAT",		pDangerAlarm->m_bRepeat,	true );

		LUA_GET_VALUE_ENUM( luaManager, "ALARM_COLOR_TYPE", pDangerAlarm->m_eColor, ALARM_COLOR_TYPE, ACT_BLACK );

		pDangerAlarm->m_wstrMessage = GET_STRING( iStringIndex );
		pDangerAlarm->m_uid = iUid;
		pDangerAlarm->m_iStateId = iStateId;

		luaManager.EndTable();
	}

	if( pDangerAlarm != NULL )
	{
		m_vecDangerAlarm.push_back( pDangerAlarm );
		iRet = (int)m_vecDangerAlarm.size() - 1;
	}

	return iRet;
}

int CX2Game::GetDangerAlarm( UidType iUid, int iStateId )
{
	for(UINT i=0; i<m_vecDangerAlarm.size(); ++i)
	{
		DangerAlarm *pAlarm = m_vecDangerAlarm[i];
		if( pAlarm != NULL &&
			pAlarm->m_uid == iUid && pAlarm->m_iStateId == iStateId )
		{
			return i;
		}
	}

	return -1;
}



void    CX2Game::KFPS_STATISTICS::AddNewFps( float fFps )
{
	if ( fFps <= 0.f )
		return;

	if ( m_uNumOfFps == 0 )
	{
		m_fAverageFps = fFps;
		m_uNumOfFps = 1;
	}
	else
	{
		float   fOneWeight = 1.f / (float) ( m_uNumOfFps + 1 );
		float   fSumWeight = (float) m_uNumOfFps * fOneWeight;

		m_fAverageFps = fSumWeight * m_fAverageFps + fOneWeight * fFps;
		m_uNumOfFps++;
	}

}//CX2Game::KFPS_STATISTICS::AddNewFps()

void        CX2Game::KGAME_STATISTICS::GatherPeriodicStatistics()
{
	m_kAverageFps.AddNewFps( DXUTGetFPS() );

}//CX2Game::KGAME_STATISTICS::GatherPeriodicStatistics()


void    CX2Game::KGAME_STATISTICS::Send_EGS_FRAME_AVERAGE_REQ( int iGameType  )
{
	if ( m_kAverageFps.m_uNumOfFps == 0 )
        return;

	KEGS_FRAME_AVERAGE_REQ kPacket;

    kPacket.m_cGameType = iGameType;
	kPacket.m_iFrameAverage = static_cast<int>( m_kAverageFps.m_fAverageFps );

	if( g_pData != NULL && g_pData->GetServerProtocol() != NULL )
		g_pData->GetServerProtocol()->SendPacket( EGS_FRAME_AVERAGE_REQ, kPacket );

}//CX2Game::KGAME_STATISTICS::Send_EGS_FRAME_AVERAGE_REQ()



// cheat 명령어로 npc 생성하는 이벤트를 시뮬레이션 서버로 보냅니다
bool CX2Game::Send_EGS_ADMIN_NPC_UNIT_CREATE_REQ_LUA( int unitID, int level, D3DXVECTOR3 vPos, bool bRight, int iAttrib1, int iAttrib2, int iSA1, int iSA2 )
{
	if ( m_bGameEnd == true )
		return false;

	//if( NULL == GetMyUser() )
	//	return false;

	if( g_pData->GetMyUser()->GetAuthLevel() < CX2User::XUAL_OPERATOR )
		return false;

	int iKeyCode = -1;
	CX2Room::TEAM_NUM eTeam = CX2Room::TN_MONSTER;
	CX2NPCAI::NPC_AI_TYPE eAIType = CX2NPCAI::NAT_NORMAL;
	UidType iAllyUID = -1;
	bool bActive = true;
	bool bNoDrop = false;

	KEGS_ADMIN_NPC_UNIT_CREATE_REQ kPacket;
	kPacket.m_kNPCUnitReq.Init();

	kPacket.m_kNPCUnitReq.m_NPCID		= unitID;
	kPacket.m_kNPCUnitReq.m_Level		= level;
	kPacket.m_kNPCUnitReq.m_bActive		= bActive;
	kPacket.m_kNPCUnitReq.m_vPos.x		= vPos.x;
	kPacket.m_kNPCUnitReq.m_vPos.y		= vPos.y;
	kPacket.m_kNPCUnitReq.m_vPos.z		= vPos.z;
	kPacket.m_kNPCUnitReq.m_bIsRight	= bRight;
	kPacket.m_kNPCUnitReq.m_bNoDrop		= bNoDrop;
	kPacket.m_kNPCUnitReq.m_KeyCode		= (char)iKeyCode;
	kPacket.m_kNPCUnitReq.m_cTeamNum	= (char) eTeam;
	kPacket.m_kNPCUnitReq.m_cAIType		= (char) eAIType;
	kPacket.m_kNPCUnitReq.m_iAllyUID	= iAllyUID;

	kPacket.m_cAttrib1 = static_cast<char>(iAttrib1);
	kPacket.m_cAttrib2 = static_cast<char>(iAttrib2);
	kPacket.m_cSA1 = static_cast<char>(iSA1);
	kPacket.m_cSA2 = static_cast<char>(iSA2);

	g_pData->GetServerProtocol()->SendPacket( EGS_ADMIN_NPC_UNIT_CREATE_REQ, kPacket );
	g_pMain->AddServerPacket( EGS_ADMIN_NPC_UNIT_CREATE_ACK, 60.f );

	return true;
}

bool CX2Game::Handler_EGS_ADMIN_NPC_UNIT_CREATE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KPacketOK kEvent;
	DeSerialize( pBuff, &kEvent );
	//CX2PacketLog::PrintLog( &kEvent );

	if( g_pMain->DeleteServerPacket( EGS_ADMIN_NPC_UNIT_CREATE_ACK ) == true )
	{
		if( g_pMain->IsValidPacket( kEvent.m_iOK ) == true )
		{
			return true;
		}
	}

	return false;
}


bool CX2Game::Handler_EGS_ADMIN_NPC_UNIT_CREATE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	KSerBuffer* pBuff = (KSerBuffer*)lParam;
	KEGS_ADMIN_NPC_UNIT_CREATE_NOT kEvent;
	DeSerialize( pBuff, &kEvent );
	//CX2PacketLog::PrintLog( &kEvent );

	CX2Dungeon* pDungeon = static_cast< CX2DungeonGame* >( this )->GetDungeon();
	if( pDungeon )
	{
		KNPCUnitReq& kNPCUnitReq = kEvent.m_kNPCUnitNot;

		CX2GUNPC* pNPC = CreateNPC( (CX2UnitManager::NPC_UNIT_ID)kNPCUnitReq.m_NPCID,
			kNPCUnitReq.m_Level,
			kNPCUnitReq.m_bActive,
			kNPCUnitReq.m_UID,
			D3DXVECTOR3( kNPCUnitReq.m_vPos.x, kNPCUnitReq.m_vPos.y, kNPCUnitReq.m_vPos.z ),
			kNPCUnitReq.m_bIsRight,
			kNPCUnitReq.m_fDelayTime,
			(int) kNPCUnitReq.m_KeyCode,
			(CX2Room::TEAM_NUM) kNPCUnitReq.m_cTeamNum,
			(CX2NPCAI::NPC_AI_TYPE) kNPCUnitReq.m_cAIType,
			kNPCUnitReq.m_iAllyUID );

		std::map< int, KAttribEnchantNpcInfo >& mapAttribEnchantNpcInfo = pDungeon->GetNowStage()->GetStageData()->m_mapAttribEnchantNpcInfo;
		KAttribEnchantNpcInfo& kAttribEnchantNpcInfo = mapAttribEnchantNpcInfo[kNPCUnitReq.m_UID];
		kAttribEnchantNpcInfo.m_cAttribEnchant1 = kEvent.m_cAttrib1;
		kAttribEnchantNpcInfo.m_cAttribEnchant2 = kEvent.m_cAttrib2;
		kAttribEnchantNpcInfo.m_cExtraSpecialAbility1 = kEvent.m_cSA1;
		kAttribEnchantNpcInfo.m_cExtraSpecialAbility2 = kEvent.m_cSA2;

		if ( pNPC )
		{
			pNPC->ApplyEnchantMonster( kAttribEnchantNpcInfo );
		}

	}

	//{{ kimhc // 2010.4.23 // 비밀던전 작업(Extra 모드)
	// 던전에서 몬스터를 임의로 생성해서 성공했을 때
	// 해당 몬스터가 substage clear 조건에 있는 경우
	// 클리어 조건의 해당 몬스터 original count를 update 시켜줌
	if ( GetGameType() == GT_DUNGEON )
	{
		CX2Dungeon* pDungeon = static_cast< CX2DungeonGame* >( this )->GetDungeon();

		if ( pDungeon != NULL &&
			pDungeon->GetNowStage() != NULL &&
			pDungeon->GetNowStage()->GetNowSubStage() != NULL )
		{
			pDungeon->GetNowStage()->GetNowSubStage()->SetMonsterCountForSubStageClearCheck();
		}

	}
	//}} kimhc // 2010.4.23 // 비밀던전 작업(Extra 모드)
	return true;
}

int CX2Game::GetSummonMonsterCardNPCLevel( CX2GUNPC* pNPC, int iLevel )
{
	if( NULL == g_pData->GetPartyManager() && NULL == g_pData->GetPartyManager()->GetMyPartyData() )
		return iLevel;

	if( NULL != g_pData->GetDungeonRoom() &&  true == g_pData->GetDungeonRoom()->IsDefenceDungeon( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID) )
	{
		return 1;	//어둠의 문은 1레벨 보정
	}

	if( CX2Dungeon::DM_HENIR_CHALLENGE == g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode )
	{
		return 1;	//헤니르 도전 모드는 1레벨 보정
	}

	if( CX2Dungeon::DM_SECRET_HELL == g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonMode
		)
	{
        CX2GUUser* pUser = pNPC->GetOwnerGameUnit();
		if( NULL != pUser )
			return pUser->GetUnitLevel();	//비밀 던전 헬 모드는 User의 레벨과 같게 설정
	}

	if( true == CX2Dungeon::IsEventDungeon( static_cast<SEnum::DUNGEON_ID>( g_pData->GetPartyManager()->GetMyPartyData()->m_iDungeonID ) ) )
	{
		return 1;	/// 이벤트 던전에 대한 레벨 보정
	}

	return iLevel;
}

void CX2Game::CreateSummonMonsterCardNPC( const CX2Item::SpecialAbility* pSa, CX2GUUser* pCX2GUUser )
{
	if( GT_DUNGEON != GetGameType() )
		return;

	if( -1 != pCX2GUUser->GetSummonMonsterCardData().GetSummonMonsterUID() )	//만약 소환중인 NPCID를 가지고 있을 땐 소환 불가
		return;

	wstring wstrGroupMonsterInfo = pSa->GetStringValue1();

	wstring wstrSummonMonsterID		= L"";	//소환할 몬스터 ID
	wstring wstrSummonMonsterRate	= L"";	//해당 몬스터가 소환될 확률

	int iMonsterID = 0;

	int iStartIndex = 0;

	while( wstrGroupMonsterInfo.find( L" " ) )	//ID와 확률을 받아와서 소환 확률이 충족 되면 소환, 소환 확률이 충족 되지 않으면 다음 소환 정보를 받아온다.
	{
		iStartIndex = wstrGroupMonsterInfo.find( L" ", iStartIndex + 1 );

		if( wstrGroupMonsterInfo.compare( L"Group 0" ) != 0 )
		{
			int	iMiddleIndex = wstrGroupMonsterInfo.find( L" ", iStartIndex + 1 );

			if( iMiddleIndex != -1 )						//해당 몬스터 ID에 소환 확률이 포함되어 있을 때, 확률 계산
			{
				int	iEndIndex			= wstrGroupMonsterInfo.find( L" ", iMiddleIndex + 1 );
				wstrSummonMonsterRate	= wstrGroupMonsterInfo.substr( iMiddleIndex + 1, iEndIndex - iMiddleIndex - 1 );	//NPC 소환 확률
				int iSummonMonsterRate	= _wtoi( wstrSummonMonsterRate.c_str() );										//연산을 위해 int로 변환

				if( iSummonMonsterRate > RandomInt() )	//소환 확률을 충족 시킨다면, 소환 ID 설정
				{
					wstrSummonMonsterID = wstrGroupMonsterInfo.substr( iStartIndex + 1, iMiddleIndex - iStartIndex - 1 );//NPC ID

					break;
				}
				else
				{
					iStartIndex = iMiddleIndex;			//아니라면, 다음 인자 검사
				}
			}
			else										//ID에 소환 확률이 포함되어 있지 않다면, 무조건 소환
			{
				wstrSummonMonsterID = wstrGroupMonsterInfo.substr( iStartIndex + 1 );	//Default 몬스터 ID

				break;
			}
		}
		else
		{
			ASSERT( "Summon Value Error!!!" );

			break;
		}
	}

	iMonsterID = _wtoi( wstrSummonMonsterID.c_str() );	//소환 ID 설정

	if( 0 != iMonsterID )
	{
		int iMonsterLevel = pSa->m_Value1;				//소환 몬스터 레벨
		if( 0 == iMonsterLevel )
		{
			iMonsterLevel = pCX2GUUser->GetUnitLevel();	//Default값일 때, 소환 Unit의 레벨로 설정
		}

		int iMonsterTeam  = pSa->m_Value2;				//소환 몬스터 팀


		D3DXVECTOR3					vSummonPos		= pCX2GUUser->GetPos();			/// 생성 위치
		bool						bIsRight		= pCX2GUUser->GetIsRight();		/// 생성 방향
		CX2GUNPC::NPC_CREATE_TYPE	eCreateNPCType	= CX2GUNPC::NCT_MONSTER_CARD;	/// 생성 타입

		if ( NULL != g_pData &&
			 NULL != g_pData->GetUIManager() &&
			 NULL != g_pData->GetUIManager()->GetUIQuickSlot() &&
			 true == g_pData->GetUIManager()->GetUIQuickSlot()->GetIsSummonCardSlot() )
		{
			/// 어둠의 문 이라면, 아군 소환문 위치에 생성한다.
			CX2GUNPC* pSummonGateAlly = GetNPCUnitByType(  CX2UnitManager::NUI_GOD_GATE_ALLY );

			if ( NULL != pSummonGateAlly )
			{
				vSummonPos		= pSummonGateAlly->GetPos();
				bIsRight		= true;
				eCreateNPCType	= CX2GUNPC::NCT_DEFENCE_DUNGEON;
				iMonsterLevel	= 1;
			}
		}
		if( iMonsterID == EVENT_MONSTER_CARD_ALLY_ID )
		{
			dbg::cout << L"지헌로그-테스트 : 의문의 카드 사용 후 몬스터 소환 체크 시작한다!!" << dbg::endl;
			vSummonPos.x -= 200;
			vSummonPos.z -= 200;
			CKTDGLineMap* pLineMap = g_pX2Game->GetWorld()->GetLineMap();
			D3DXVECTOR3 outPos(0, 0, 0);
			//소환할 몬스터 수
			if(  pCX2GUUser->GetRandomInt() < EVENT_MONSTAR_CARD_RATE ) //50보다 낮으면 나쁜놈 소환
			{
				for( int i = 0; i < EVENT_MONSTER_CARD_MAX_ENENY; ++i)
				{
					vSummonPos.x += 100;
					vSummonPos.z += 100;
					if(pLineMap != NULL)
					{
						//가까운 라인맵에 소환한다
						if( -1 != g_pX2Game->GetWorld()->GetLineMap()->GetNearestLine( vSummonPos, &outPos ) )
						{
							vSummonPos = outPos;
							g_pX2Game->CreateNPCReq( static_cast<CX2UnitManager::NPC_UNIT_ID>( EVENT_MONSTER_CARD_ENEMY_ID ), iMonsterLevel, true,
								vSummonPos, bIsRight, 0.f, false, -1, CX2Room::TN_MONSTER,
								CX2NPCAI::NAT_NORMAL, -1,false, CX2Room::TN_NONE, CX2GUNPC::NCT_NONE );
						}
						else //라인맵이 존재 하지 않다면 유저위치에 소환한다.
						{
							g_pX2Game->CreateNPCReq( static_cast<CX2UnitManager::NPC_UNIT_ID>( EVENT_MONSTER_CARD_ENEMY_ID ), iMonsterLevel, true,
								pCX2GUUser->GetPos(), bIsRight, 0.f, false, -1, CX2Room::TN_MONSTER,
								CX2NPCAI::NAT_NORMAL, -1,false, CX2Room::TN_NONE, CX2GUNPC::NCT_NONE );
						}
					}

				}
				return;
			}
			else //50보다 높으면 좋은놈 소환
			{
				for( int i = 0; i < EVENT_MONSTER_CARD_MAX_ENENY; ++i)
				{
					vSummonPos.x += 100;
					vSummonPos.z += 100;
					if(pLineMap != NULL)
					{
						//가까운 라인맵에 소환한다
						if( -1 != g_pX2Game->GetWorld()->GetLineMap()->GetNearestLine( vSummonPos, &outPos ) )
						{
							vSummonPos = outPos;
							g_pX2Game->CreateNPCReq( static_cast<CX2UnitManager::NPC_UNIT_ID>( iMonsterID ), iMonsterLevel, true,
								vSummonPos, bIsRight, 0.f, true, -1, static_cast<CX2Room::TEAM_NUM>( iMonsterTeam ),
								CX2NPCAI::NAT_ALLY, pCX2GUUser->GetUnitUID(),false, CX2Room::TN_NONE, CX2GUNPC::NCT_MONSTER_CARD );
						}
						else   //라인맵이 존재 하지 않다면 유저위치에 소환한다.
						{
							g_pX2Game->CreateNPCReq( static_cast<CX2UnitManager::NPC_UNIT_ID>( iMonsterID ), iMonsterLevel, true,
								pCX2GUUser->GetPos(), bIsRight, 0.f, true, -1, static_cast<CX2Room::TEAM_NUM>( iMonsterTeam ),
								CX2NPCAI::NAT_ALLY, pCX2GUUser->GetUnitUID(),false, CX2Room::TN_NONE, CX2GUNPC::NCT_MONSTER_CARD );
						}
					}

				}
				return;
			}
		}

		CreateNPCReq( static_cast<CX2UnitManager::NPC_UNIT_ID>( iMonsterID ), iMonsterLevel, true,
		vSummonPos, bIsRight, 0.f, true, -1, static_cast<CX2Room::TEAM_NUM>( iMonsterTeam ),
		CX2NPCAI::NAT_ALLY, pCX2GUUser->GetUnitUID(),false, CX2Room::TN_NONE, eCreateNPCType );


	}
}

bool CX2Game::CanCheckAbuser( const CX2GUUser* pCheckUser_, std::map< UidType, boost::timer >::iterator mit_ )
{
 	return ( pCheckUser_->GetNowStateID() > static_cast<int>( CX2GUUser::USI_LOSE )
 		&& mit_->second.elapsed() > 1.0f
		&& NULL != GetMyUnit()
		&& mit_->first != GetMyUnit()->GetUnitUID() );
}

void CX2Game::AbuserUserCheck( const CX2GUUser* pCheckUser_ )
{
	if( m_pRoom != NULL )
	{
		const UidType uidCheckUser = pCheckUser_->GetUnitUID();

		// 시간 갱신부
		std::map< UidType, boost::timer >::iterator mitFirst = m_mapCheckUDPTimer.find( uidCheckUser );
		if( mitFirst == m_mapCheckUDPTimer.end() )
		{
			// 2012.06.17 lygan_조성욱 // 처음 싱크를 주고 받았다 등록하자
			InsertGUUserToCheckUDPTimer( uidCheckUser );
		}
		else
		{
			mitFirst->second.restart();
		}

		// 검사부
		std::map< UidType, boost::timer >::iterator mit;
		for ( mit = m_mapCheckUDPTimer.begin(); mit != m_mapCheckUDPTimer.end(); mit++ )
		{
			// 2012.06.17 lygan_조성욱 // 두번째 싱크 지연시간이 5초 이상이다 이상하다
			// 2012.06.17 lygan_조성욱 // 내가 지연 된게 아닌지 체크
			if( CanCheckAbuser( pCheckUser_, mit ) )
			{
				// 서버에서 검증하므로 바로 찌릅니다. 상대의 통신을 릴레이로 변경해 달라고 요청
				KEGS_FORCE_RELAY_NOT kPacket;
				kPacket.m_iTargetUnitUID = mit->first;

				if( g_pData != NULL && g_pData->GetServerProtocol() != NULL )
					g_pData->GetServerProtocol()->SendPacket( EGS_FORCE_RELAY_NOT, kPacket );
				// 한번 신고가 들어갔음으로 시간 초기화 해준다.
				mit->second.restart();
			}
		}
	}
}

/*FORCEINLINE*/ CKTDGParticleSystem* CX2Game::GetMajorParticle()
{
	return g_pData->GetGameMajorParticle();
}

/*FORCEINLINE*/ CKTDGParticleSystem* CX2Game::GetMinorParticle()
{
	return g_pData->GetGameMinorParticle();
}

/*FORCEINLINE*/ CKTDGXMeshPlayer* CX2Game::GetMajorXMeshPlayer()
{
	return g_pData->GetGameMajorXMeshPlayer();
}

/*FORCEINLINE*/ CKTDGXMeshPlayer* CX2Game::GetMinorXMeshPlayer()
{
	return g_pData->GetGameMinorXMeshPlayer();
}

/*FORCEINLINE*/ CX2EffectSet* CX2Game::GetEffectSet()
{
	return g_pData->GetGameEffectSet();
}


bool CX2Game::CheckAndWarningBusyStateNow( const bool bCheckDead_ /*= true*/ )
{
	if ( NULL != GetMyUnit() && true == GetMyUnit()->IsBusyStateNow() )
	{
		//WCHAR* pwszWarning = NULL;
		wstring wstrWarning;

		if ( GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE || !( GetMyUnit()->GetNowHp() > 0 ) )
		{
			if ( true == bCheckDead_ )
				wstrWarning = GET_STRING( STR_ID_19230 );
		}
		else
		{
			const float fTimeGap = GetMyUnit()->GetRemainElapsedTimeNotBusyState();

			wstrWarning += GET_REPLACED_STRING( (STR_ID_19231, "i", static_cast<int>( fTimeGap ) + 1) );
		}

		if ( wstrWarning.empty() )
			return false;
		else
		{
			GetInfoTextManager().PushText( XUF_DODUM_20_BOLD,
				wstrWarning.c_str(), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			if ( g_pChatBox != NULL )
				g_pChatBox->AddChatLog( wstrWarning.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, D3DXCOLOR(1,1,0,1), L"#CFFFF00" );

			return true;
		}
	}
	else
		return false;
}

void CX2Game::SetShowSmallGageAndNameForAllGameUnit( const bool bShow_ )
{

	BOOST_FOREACH( CX2GameUnit* pGameUnit, m_UnitList )
	{
		pGameUnit->SetShowSmallGageAndName( bShow_ );
	}
}


void CX2Game::ChangeUserUnitIndex( const UINT uiFirstIndex_, const UINT uiSecondIndex_ )
{
	ASSERT( uiFirstIndex_ < m_UserUnitList.size() && uiSecondIndex_ < m_UserUnitList.size() );
	if ( uiFirstIndex_ < m_UserUnitList.size() && uiSecondIndex_ < m_UserUnitList.size() )
	{
		CX2GUUser* pFirstUser = m_UserUnitList[uiFirstIndex_];
		CX2GUUser* pSecondUser = m_UserUnitList[uiSecondIndex_];

		// 스왑시 이게 더 가독성이 좋을 것 같아서 이렇게 함
		m_UserUnitList[uiFirstIndex_] = pSecondUser;
		m_UserUnitList[uiSecondIndex_] = pFirstUser;

		if ( NULL != pFirstUser )
			pFirstUser->SetUnitIndex( uiSecondIndex_ );

		if ( NULL != pSecondUser )
			pSecondUser->SetUnitIndex( uiFirstIndex_ );
	}
}


CX2GUUser* CX2Game::CreateGUUser( const CX2Room::SlotData* pSlotData_, int iSlotIndex_ )
{
	const CX2UnitManager::UnitTypeLuaTemplet& kUnitTypeLuaTemplet =
		g_pData->GetUnitManager()->GetUnitTypeLuaTemplet( pSlotData_->m_pUnit->GetType() );
	lua_State*	script_info = ( kUnitTypeLuaTemplet.m_pLuaManager != NULL ) ? kUnitTypeLuaTemplet.m_pLuaManager->GetLuaState() : NULL;


	switch( pSlotData_->m_pUnit->GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			return CX2GUElsword_SwordMan::CreateGUElsword( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData_->m_pUnit );
		}
		break;

	case CX2Unit::UT_ARME:
		{
			return CX2GUArme_VioletMage::CreateGUArme( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData_->m_pUnit );
		}
		break;

	case CX2Unit::UT_LIRE:
		{
			return CX2GULire_ElvenRanger::CreateGULire( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData_->m_pUnit );
		}
		break;

	case CX2Unit::UT_RAVEN:
		{
			return CX2GURaven::CreateGURaven( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData_->m_pUnit );
		}
		break;

	case CX2Unit::UT_EVE:
		{
			return CX2GUEve::CreateGUEve( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData_->m_pUnit );
		}
		break;

		//{{ kimhc // 2010.11.18 // 2010-12-23 New Character CHUNG
	case CX2Unit::UT_CHUNG:
		{
			return CX2GUChung::CreateGUChung( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(), pSlotData_->m_pUnit );
		}
		break;
		//}} kimhc // 2010.11.18 //  2010-12-23 New Character CHUNG

	case CX2Unit::UT_ARA:
		{
			return CX2GUAra::CreateGUAra( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(),
										  pSlotData_->m_pUnit );
		}
		break;
	case CX2Unit::UT_ELESIS:
		{
			return CX2GUEL::CreateGUEL( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(),
				pSlotData_->m_pUnit );
		} break;
	case CX2Unit::UT_ADD:
		{
			return CX2GUAdd::CreateGUAdd( iSlotIndex_, pSlotData_->m_TeamNum, script_info, g_pMain->GetFrameBufferNum(),
				pSlotData_->m_pUnit );
		} break;

	default:
		{
			ASSERT( !"Create User Object is Fail!! ( Unknown User Type )" );
			return NULL;
		}
		break;
	}
}


/*virtual*/ void CX2Game::ResurrectionKeyProcess()
{
	if ( GetMyUnit() != NULL && GetMyUnit()->GetGameUnitState() == CX2GameUnit::GUSI_DIE &&
		m_bIsSendResurrectionReq == false )
	{
		if ( IsAllUserDead() == false )
		{
			if ( m_bResurrectionCountStart == true )
			{

				if ( m_InputData.oneZ == true
					&& GetOpenChatBox() == false )
				{

					Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ( GetMyUnit()->GetUnit()->GetUID() );
				}

				if ( m_fResurrectionRemainTime > 0.0f
					&& g_pKTDXApp->GetDIManager()->Getkeyboard()->GetKeyState( DIK_X ) == TRUE
					&& GetOpenChatBox() == false )
				{
					m_fResurrectionRemainTime = 0.0f;
				}
			}
		}
		else
		{
			if ( m_bGameEnd == false )
			{
				if ( m_InputData.oneZ == true
					&& GetOpenChatBox() == false )
				{
					Handler_EGS_RESURRECT_TO_CONTINUE_DUNGEON_REQ( GetMyUnit()->GetUnit()->GetUID() );
				}
			}

			if ( m_fResurrectionRemainTime > 0.0f
				&& true == m_InputData.oneX
				&& GetOpenChatBox() == false )
			{
				Handler_EGS_SKIP_CONTINUE_DUNGEON_NOT();
				m_fResurrectionRemainTime = 0.0f;
			}
		}
	}
}

void CX2Game::UpdateNpcNameColor( const int iMyLevel_ )
{
	BOOST_FOREACH( CX2GUNPC* pGUNpc, m_NPCUnitList )
	{
		if ( NULL != pGUNpc )
			pGUNpc->InitNameColor( iMyLevel_ );
	}
}


void CX2Game::WriteBroadCastUser()
{
	WCHAR wLog[512];
	ZeroMemory( wLog, sizeof(WCHAR) * 512 );
	wsprintf( wLog, L"UIDForSyncPacket: " );

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	BOOST_FOREACH( UidType uid, g_pData->GetGameUDP()->GetNonRelayUIDs() )
	{
		WCHAR wBuffer[50];
		ZeroMemory( wBuffer, sizeof(WCHAR) * 50 );
		wsprintf( wBuffer, L"%lld ", uid );
		StringCchCat( wLog, sizeof(WCHAR) * 512, wBuffer );
	}
	BOOST_FOREACH( UidType uid, g_pData->GetGameUDP()->GetRelayUIDs() )
	{
		WCHAR wBuffer[50];
		ZeroMemory( wBuffer, sizeof(WCHAR) * 50 );
		wsprintf( wBuffer, L"%lld ", uid );
		StringCchCat( wLog, sizeof(WCHAR) * 512, wBuffer );
	}

//#else   SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//
//	BOOST_FOREACH( UidType uid, m_vecUserUIDforSyncPacket )
//	{
//		WCHAR wBuffer[50];
//		ZeroMemory( wBuffer, sizeof(WCHAR) * 50 );
//		wsprintf( wBuffer, L"%lld ", uid );
//		StringCchCat( wLog, sizeof(WCHAR) * 512, wBuffer );
//	}
//
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK

	StateLog( wLog );
}

/** @function : SetBuffInfoPacketToNpc
	@brief : 서버로부터 받은 버프정보를 NPC에게 적용
	@param : 서버로부터 받은 NPC 버프 정보 패킷
*/
void CX2Game::SetBuffInfoPacketToNpc( const KEGS_UPDATE_NPC_UNIT_BUFF_INFO_BROAD_NOT& kEvent_ )
{
	BOOST_FOREACH( const KNpcUnitBuffInfo& kNpcUnitBuffInfo, kEvent_.m_vecNpcUnitBuff )
	{
		CX2GUNPC* pGUNpcUnit = GetNPCUnitByUID( kNpcUnitBuffInfo.m_iNpcUID );
		if ( NULL != pGUNpcUnit )
			pGUNpcUnit->SetKBuffFactorPacketToGameUnit( kNpcUnitBuffInfo.m_vecBuffFactor );
	}
}

/** @function : SetBuffInfoPacketToGUUser
	@brief : 서버로부터 받은 버프정보를 유저에게 적용
	@param : 서버로부터 받은 유저 버프 정보 패킷
*/
void CX2Game::SetBuffInfoPacketToGUUser( const KEGS_UPDATE_USER_UNIT_BUFF_INFO_BROAD_NOT& kEvent_ )
{
	/// 내가 보낸 패킷은 처리 안함
	if ( NULL != GetMyUnit() && GetMyUnit()->GetUnitUID() == kEvent_.m_iUnitUID )
		return;

	CX2GUUser* pGUUser = GetUserUnitByUID( kEvent_.m_iUnitUID );
	if ( NULL != pGUUser )
	{
		pGUUser->SetKBuffFactorPacketToGameUnit( kEvent_.m_vecBuffFactor );
	}
	else
	{
		StateLog( L"Buff User is NULL" );
		ASSERT( !L"Buff User is NULL" );
	}
}

CX2Game::CinematicUI::CinematicUI():
    m_hCinematicTopParticle( INVALID_PARTICLE_HANDLE )
	,m_hCinematicBottomParticle( INVALID_PARTICLE_HANDLE )
	,m_hCinematicTop(INVALID_PARTICLE_SEQUENCE_HANDLE)
	,m_hCinematicBottom(INVALID_PARTICLE_SEQUENCE_HANDLE)
	,m_bStart (false )
	,m_bEnd ( false )
	,m_fTimer ( 0.f )
	,m_fDeleteTime ( -1.f )
{
	if( NULL != g_pData->GetUIMajorParticle() )
	{
		m_hCinematicTop = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  L"JoinNpcTop", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicTop)
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicTop );
			if( pSeq != NULL )
			{
                m_hCinematicTopParticle = pSeq->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
				pSeq->SetShowObject(false);
			}
		}

		m_hCinematicBottom = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  L"JoinNpcBottom", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
		if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicBottom )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicBottom );
			if( pSeq != NULL )
			{
                m_hCinematicBottomParticle = pSeq->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
				pSeq->SetShowObject(false);
			}
		}
	}
}

CX2Game::CinematicUI::~CinematicUI()
{
	if( NULL != g_pData->GetUIMajorParticle() )
	{
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hCinematicTop );
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hCinematicBottom );
	}
}

HRESULT CX2Game::CinematicUI::OnFrameMove( double fTime, float fElapsedTime )
{
	m_fTimer += fElapsedTime;

	if( true == m_bStart )
	{
        CKTDGParticleSystem::CParticle* pCinematicTopParticle = NULL;
        CKTDGParticleSystem::CParticle* pCinematicBottomParticle = NULL;

		if( NULL != g_pData->GetUIMajorParticle() )
        {
            if( INVALID_PARTICLE_HANDLE != m_hCinematicTopParticle )
                pCinematicTopParticle = g_pData->GetUIMajorParticle()->ValidateParticleHandle( m_hCinematicTopParticle );
            if( INVALID_PARTICLE_HANDLE != m_hCinematicBottomParticle )
                pCinematicBottomParticle = g_pData->GetUIMajorParticle()->ValidateParticleHandle( m_hCinematicBottomParticle );
        }

		if(m_fTimer <= 1.0f)
		{
			float fAlpha = m_fTimer;
			if ( NULL != pCinematicTopParticle )
			{
				pCinematicTopParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, fAlpha) );
			}
			if ( NULL != pCinematicBottomParticle )
			{
				pCinematicBottomParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, fAlpha) );
			}
		}
		else
		{
			if ( NULL != pCinematicTopParticle )
			{
				pCinematicTopParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, 1.f) );
			}
			if ( NULL != pCinematicBottomParticle )
			{
				pCinematicBottomParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, 1.f) );
			}

// CinematicUI 사용 시, m_fDeleteTime 을 설정하여 생성한다면
// 절반의 시간 동안 Fade-In, 나머지 절반의 시간 동안 Fade-Out 을 실시!

			if ( -1.f != m_fDeleteTime )
			{
				if ( m_fTimer >= m_fDeleteTime / 2 )
				{
					m_bStart = false;
					m_bEnd = true;
				}
			}
			else
			{
				m_fTimer = 1.1f;
			}
		}
	}


	if( true == m_bEnd )
	{
        CKTDGParticleSystem::CParticle* pCinematicTopParticle = NULL;
        CKTDGParticleSystem::CParticle* pCinematicBottomParticle = NULL;

		if( NULL != g_pData->GetUIMajorParticle() )
        {
            if( INVALID_PARTICLE_HANDLE != m_hCinematicTopParticle )
                pCinematicTopParticle = g_pData->GetUIMajorParticle()->ValidateParticleHandle( m_hCinematicTopParticle );
            if( INVALID_PARTICLE_HANDLE != m_hCinematicBottomParticle )
                pCinematicBottomParticle = g_pData->GetUIMajorParticle()->ValidateParticleHandle( m_hCinematicBottomParticle );
        }

		if ( -1.f != m_fDeleteTime  )							// DeleteTime 이 존재한다면
		{
			if ( m_fTimer >= ( m_fDeleteTime - 1.f ) )		// Fade-Out 시간을 1초로..
			{
				float fAlpha = m_fDeleteTime - m_fTimer;
				if ( NULL != pCinematicTopParticle )
				{
					pCinematicTopParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, fAlpha) );
				}
				if ( NULL != pCinematicBottomParticle )
				{
					pCinematicBottomParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, fAlpha) );
				}

				if ( m_fTimer >= m_fDeleteTime )			// Fade-Out 시간을 넘으면
				{
					Init();									// 초기화
				}
			}
		}
		else
		{
			if(m_fTimer <= 0.5f)
			{
				float fAlpha = 1.f - (m_fTimer * 2.f);
				if ( NULL != pCinematicTopParticle )
				{
					pCinematicTopParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, fAlpha) );
				}
				if ( NULL != pCinematicBottomParticle )
				{
					pCinematicBottomParticle->SetColor( D3DXCOLOR(1.f, 1.f, 1.f, fAlpha) );
				}
			}
			else
			{
				if( NULL != g_pData->GetUIManager() )
				{
					if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicTop )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicTop );
						if( pSeq != NULL )
						{
							pSeq->SetShowObject(false);
						}
                        else
                        {
                            m_hCinematicTop = INVALID_PARTICLE_SEQUENCE_HANDLE;
                            m_hCinematicTopParticle = INVALID_PARTICLE_HANDLE;
                        }
					}

					if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicBottom)
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicBottom );
						if( pSeq != NULL )
						{
							pSeq->SetShowObject(false);
						}
                        else
                        {
                            m_hCinematicBottom = INVALID_PARTICLE_SEQUENCE_HANDLE;
                            m_hCinematicBottomParticle = INVALID_PARTICLE_HANDLE;
                        }
					}
				}
				m_bEnd = false;
			}

		}
	}

	return S_OK;
}
void CX2Game::CinematicUI::SetCinematicUI( bool bVal, float DeleteTime_ )
{
	if ( -1.f != DeleteTime_ )					// DeleteTime 이 존재한다면
	{
		Init ();							// 초기화
		m_fDeleteTime = DeleteTime_;
		m_bStart = true;
		m_fTimer = 0.f;

		if( NULL != g_pData->GetUIMajorParticle() )
		{
			// 던전 중간 보스에서 가져온 Danger 연출
			if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicTop )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicTop );
				if( pSeq != NULL )
				{
					pSeq->SetShowObject(bVal);
				}
			}
			if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicBottom )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicBottom );
				if( pSeq != NULL )
				{
					pSeq->SetShowObject(bVal);
				}
			}
		}
	}
	else
	{
		if( true == bVal )
		{
			if( false == m_bStart )
			{
				m_bStart = bVal;
				m_bEnd = !bVal;
				m_fTimer = 0.f;

				if( NULL != g_pData->GetUIMajorParticle() )
				{
					if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicTop )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicTop );
						if( pSeq != NULL )
						{
							pSeq->SetShowObject(bVal);
						}
					}
					if( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hCinematicBottom )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hCinematicBottom );
						if( pSeq != NULL )
						{
							pSeq->SetShowObject(bVal);
						}
					}
				}
			}
		}
		else
		{
			if( true == m_bStart )
			{
				m_bStart = bVal;
				m_bEnd = !bVal;
				m_fTimer = 0.f;
			}
		}
	}
}

void CX2Game::CanNotInputAndDisalbeNPCAI(bool bVal)
{
	if( bVal == m_bCanNotInputAndPauseNPCAI )
		return;

	m_bCanNotInputAndPauseNPCAI= bVal;

	// 키입력 제한
	if( NULL != GetMyUnit() )
	{
		GetMyUnit()->SetCanNotInput( m_bCanNotInputAndPauseNPCAI );
	}

	// NPC AI 비활성화
	for( UINT i = 0; i < m_NPCUnitList.size(); ++i )
	{
		CX2GUNPC* pGUNPC = m_NPCUnitList[i];
		if( NULL != pGUNPC )
		{
			pGUNPC->SetAIEnable( !bVal );
		}
	}
}


void CX2Game::ApplyBuffFactorToGUUser( const CX2Item::ItemTemplet* pItemTemplet, CX2GUUser* pCX2GUUser )
{
    if (unsigned uNumPtr = pItemTemplet->GetNumBuffFactorPtr() )
    {
        for( unsigned u = 0; u != uNumPtr; ++u )
	    {
			CX2BuffFactorPtr ptrBuffFactor = pItemTemplet->GetBuffFactorPtr( u )->GetClonePtr( 0 );
		    if ( NULL != ptrBuffFactor )
				pCX2GUUser->SetBuffFactorToGameUnit( *ptrBuffFactor );
	    }
    }
}


/** @function : SetQuestMonster
	@brief	: 해당 NPCID와 동일한 몬스터는 퀘스트 몬스터로 설정
	@param	: 퀘스트 몬스터로 설정한 NPC ID(eNPCUnitID), 설정 상태(bShow)
*/
void CX2Game::SetQuestMonster( CX2UnitManager::NPC_UNIT_ID eNPCUnitID_ , bool bShow /*= true*/)
{
	BOOST_TEST_FOREACH( CX2GUNPC*, pNPC, m_NPCUnitList )
	{
		if( NULL != pNPC )
		{
			if( eNPCUnitID_ == pNPC->GetNPCTemplet().m_nNPCUnitID )
			{
				pNPC->SetQuestMonster( bShow );
			}
		}
	}
}

/** @function : GetNearUnitUidList
	@brief : 적군 유닛의 Uid를 특정 위치와 가까운 순으로 정렬된 값으로 구하기
	@param : CX2Room::TEAM_NUM eMyTeamNum_(자신의 팀Number), IN const D3DXVECTOR3& vMyPos_(기준 위치)
			OUT std::map<float, UidType> mapNearUnitUid_(정렬된 UID 맵)
	@return : bool 성공 true, 실패 false
*/
bool CX2Game::GetNearUnitUidList( IN CX2Room::TEAM_NUM eMyTeamNum_, IN const D3DXVECTOR3& vMyPos_, OUT std::map<float, UidType>& mapNearUnitUid_, bool bIgnoreDistanceLimit /*= false*/)
{
	mapNearUnitUid_.clear();
	const float fLimitNearDistance = 16000000.f;	// 거리 제한 값 4000 * 4000

	for( UINT uIndex = 0; uIndex < m_UnitList.size(); ++uIndex )
	{
		const CX2GameUnit* pCX2GameUnit = m_UnitList[uIndex];

		if( pCX2GameUnit == NULL )
			continue;

		if( pCX2GameUnit->GetTeam() == eMyTeamNum_ )
			continue;

		if( true == pCX2GameUnit->GetInvincible() )
			continue;

		if( 0.f >= pCX2GameUnit->GetNowHp() )
			continue;

		if( CX2GameUnit::GUT_NPC == pCX2GameUnit->GetGameUnitType() )
		{
			const CX2GUNPC* pNPC = static_cast<const CX2GUNPC*>( pCX2GameUnit );
			switch( pNPC->GetNPCTemplet().m_ClassType )
			{
			case CX2UnitManager::NCT_THING_TRAP:
			case CX2UnitManager::NCT_THING_CHECKER:
				{
					continue;
				} break;
			} // switch
		} // if

		float fDistance = GetDistance3Sq( vMyPos_, pCX2GameUnit->GetPos() );

		if( fLimitNearDistance < fDistance &&
			false == bIgnoreDistanceLimit)
			continue;

		mapNearUnitUid_.insert( std::make_pair( fDistance, pCX2GameUnit->GetUnitUID()) );
	} // for

	if( true == mapNearUnitUid_.empty() )
		return false;

	return true;
}


/** @function : SetNpcHardLevel
	@brief : NPC의 HardLevel 설정 부분 통합
	@param : HardLevel을 설정할 NPC 포인터, NPC Level
*/
void CX2Game::SetNpcHardLevel( CX2GUNPC* pNpcUnit_, const int iLevel_ )
{
	if ( NULL != g_pX2Room )
	{
		switch ( g_pX2Room->GetRoomType() )
		{
		case CX2Room::RT_DUNGEON:
			{
				// note!! 알테라 코어던전만 파티원 수에 따라 몬스터 HP 증가
				CX2DungeonRoom* pDungeonRoom = (CX2DungeonRoom*) g_pX2Room;


				if( NULL != pDungeonRoom && CX2GUNPC::NCT_NONE == pNpcUnit_->GetNPCCreateType()
					&& pNpcUnit_->GetNPCTemplet().m_nNPCUnitID != EVENT_MONSTER_CARD_ENEMY_ID
					)

				{
					switch( pDungeonRoom->GetDungeonID() )
					{
						CASE_DEFENCE_DUNGEON
						{
							pNpcUnit_->SetHardLevel( iLevel_, true, 1.f );
						} break;
					default:
						{
							if ( GetGameType() == CX2Game::GT_DUNGEON )
							{
								CX2DungeonGame* pDungeonGame = (CX2DungeonGame*)this;
								CX2Dungeon* pDungeon = pDungeonGame->GetDungeon();

								// oasis907 : 김상윤 [2011.5.16] 던전 난이도별 몬스터 체력
								switch ( g_pData->GetDungeonManager()->GetDungeonType( pDungeon->GetDungeonData()->m_DungeonID ) )
								{
								case CX2Dungeon::DT_HENIR:
									{
										pNpcUnit_->SetHardLevel( iLevel_, true, 1.f );
									} break;
								case CX2Dungeon::DT_SECRET:
									{
										pNpcUnit_->SetHardLevel( iLevel_, true, 1.f );
									} break;
								default:
									{
										switch(pDungeon->GetDungeonData()->m_eDifficulty)
										{
										case CX2Dungeon::DL_NORMAL:
											{
												pNpcUnit_->SetHardLevel( iLevel_, true, 1.0f );
											} break;
										case CX2Dungeon::DL_HARD:
											{
												pNpcUnit_->SetHardLevel( iLevel_, true, 1.2f );
											} break;
										case CX2Dungeon::DL_EXPERT:
											{
												pNpcUnit_->SetHardLevel( iLevel_, true, 1.5f );
											} break;
										default:
											{
												pNpcUnit_->SetHardLevel( iLevel_, true );
											} break;
										}
									} break;
								}
							}
						} break;
					}

					// oasis907 : 김상윤 [2011.5.16] 던전 플레이어 별 몬스터 강제 다운 수치
					switch( pDungeonRoom->GetNumOfPlayers() )
					{
					default:
					case 1:
						{
							pNpcUnit_->SetMaxForceDown( 130.f );
						} break;
					case 2:
						{
							pNpcUnit_->SetMaxForceDown( 195.f );
						} break;
					case 3:
						{
							pNpcUnit_->SetMaxForceDown( 234.f );
						} break;
					case 4:
						{
							pNpcUnit_->SetMaxForceDown( 260.f );
						} break;
					}
				}
				else
				{

					const int iLevelMonsterCard
						= ( CX2GUNPC::NCT_MONSTER_CARD == pNpcUnit_->GetNPCCreateType()
						|| pNpcUnit_->GetNPCTemplet().m_nNPCUnitID == EVENT_MONSTER_CARD_ENEMY_ID
						) ? GetSummonMonsterCardNPCLevel( pNpcUnit_, iLevel_ ) : iLevel_;

					pNpcUnit_->SetHardLevel( iLevelMonsterCard, true );
				}

			} break;

		default:
			{

				const int iLevelMonsterCard
					= ( CX2GUNPC::NCT_MONSTER_CARD == pNpcUnit_->GetNPCCreateType() ) ? GetSummonMonsterCardNPCLevel( pNpcUnit_, iLevel_ ) : iLevel_;

				pNpcUnit_->SetHardLevel( iLevelMonsterCard, true );
			} break;
		}
	}
}

/** @function : IncreaseUserPacketCountToReceive
	@brief : 유저로 부터 싱크 패킷을 받았을 때 받은 횟수를 기록하는 카운트를 증가 시키는 함수
	@param : 싱크 패킷을 날린 유닛의 UID
*/
void CX2Game::IncreaseUserPacketCountToReceive( const UidType uidGameUnit_ )
{
	if ( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
	{
		if ( NULL != g_pX2Room )
		{
			CX2Room::SlotData* pNormalSlotData =
				g_pX2Room->GetSlotDataByUnitUID( uidGameUnit_ );
			if ( NULL != pNormalSlotData )
				pNormalSlotData->m_uiUserPacketCountToReceive++;
			else
			{
				CX2Room::SlotData* pObserverSlotData =
					g_pX2Room->GetObserverSlotDataByUnitUID( uidGameUnit_ );
				if ( NULL != pObserverSlotData )
					pObserverSlotData->m_uiUserPacketCountToReceive++;
			}
		}
	}
}

void CX2Game::ProcessIKillThisNpc( const KEGS_NPC_UNIT_DIE_NOT& kEvent )
{
	if ( NULL != GetMyUnit() )
	{
		if( NULL != GetMyUnit() &&
			NULL != GetMyUnit()->GetUnit() )
		{
			if( -1 != kEvent.m_uiAttUnit &&
				GetMyUnit()->GetUnitUID() == kEvent.m_uiAttUnit )
			{
				GetMyUnit()->DoProcessWhenKillEnemy( NULL );
			}
		}
	}
}

void CX2Game::ProcessIKillThisUser( const KEGS_USER_UNIT_DIE_NOT& kEvent )
{
	if ( NULL != GetMyUnit() &&
		 NULL != GetMyUnit()->GetUnit() )
	{
		if( GetMyUnit()->GetUnitUID() == kEvent.m_KillerUserUnitUID ||
			GetMyUnit()->GetUnitUID() == kEvent.m_MaxDamageKillerUserUnitUID )
		{
			GetMyUnit()->DoProcessWhenKillEnemy( NULL );
		}
	}
}

void CX2Game::ProcessGetOutlayItem(
    const CX2Item::ItemTemplet* pItemTemplet,
    CX2GUUser* pCX2GUUser,
									int DropItemUID, bool bIsItemSuccess )
{

	if( pItemTemplet->GetNumSpecialAbility() == 0 &&
		pItemTemplet->GetNumBuffFactorPtr() == 0 )
		return;

	if( CX2Item::IT_QICK_SLOT == pItemTemplet->GetItemType() )
		return;


	m_pDropItemManager->GetDropItemPositionByUid(DropItemUID, m_vDropItemPos);

	// true이면 일반아이템이거나 던전아이템이면서 버프 성공한 경우이다.
	bool bNormalItem = true;
	CX2Item::SPECIAL_ITEM_TYPE eType = GetSpecialItemGroupType(pItemTemplet);

	// 던전&대전 아이템인지 검사
	if( eType == CX2Item::SIT_NOSTRUM || eType == CX2Item::SIT_GENIUS || eType == CX2Item::SIT_CREST )
	{
		// 습득 이펙트
		GetEffectSet()->PlayEffectSet( L"TakeSpecialItem1", (CX2GameUnit*) pCX2GUUser );
		GetEffectSet()->PlayEffectSet( L"TakeSpecialItem2", (CX2GameUnit*) pCX2GUUser, NULL, false, -1.f, -1.f, D3DXVECTOR3(1.f, 1.f, 1.f), true, m_vDropItemPos );

		// 성공여부를 검사
		bNormalItem = bIsItemSuccess;
	}

	// 성공/실패 여부에 따라 부작용 발동
	if( bNormalItem )
	{
        if ( pItemTemplet->GetNumBuffFactorPtr() == 0 )
			UseItemSpecialAbility( pItemTemplet, pCX2GUUser );
		else
			ApplyBuffFactorToGUUser( pItemTemplet, pCX2GUUser );
	}
	else
	{
		switch( eType )
		{
		case CX2Item::SIT_NOSTRUM:
			{
				if( pCX2GUUser != NULL )
					pCX2GUUser->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_SIDE_EFFECT_MEDICINE ) );
			} break;

		case CX2Item::SIT_GENIUS:
			{
				if( pCX2GUUser != NULL )
					SideEffectGenius(pCX2GUUser);
			} break;

		case CX2Item::SIT_CREST:
			{
				if( pCX2GUUser != NULL )
					pCX2GUUser->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_SIDE_EFFECT_CREST ) );
			} break;

		default:
			{
		        if ( pItemTemplet->GetNumBuffFactorPtr() == 0 )
			        UseItemSpecialAbility( pItemTemplet, pCX2GUUser );
		        else
			        ApplyBuffFactorToGUUser( pItemTemplet, pCX2GUUser );
			} break;
		}

	}
}

void CX2Game::ProcessGetEquipItem( const CX2Item::ItemTemplet* pItemTemplet)
{
	if ( pItemTemplet->GetItemType() == CX2Item::IT_WEAPON ||
		pItemTemplet->GetItemType() == CX2Item::IT_DEFENCE)
	{
		if( NULL != g_pData && g_pData->GetPlayGuide() )
		{	//장비 아이템을 획득 했을 시에 인벤토리 확인 가이드 출력
			g_pData->GetPlayGuide()->SetPlayGuide(CX2PlayGuide::PGT_OBTAIN_EQUIP_ITEM, true, g_pMain->GetKeyPad()->GetKeyString( GAMEACTION_INVENTORY ));
		}
	}
}
void CX2Game::ProcessGetEDItem( CX2GUUser* pCX2GUUser, bool bIsEDItem,
							KDungeonRewardED DungeonRewardED, UidType uiGetUnitUID)
{
	if( true == bIsEDItem &&
		true == pCX2GUUser->IsMyUnit() )
	{
		if( g_pChatBox != NULL && GetMyUnit() != NULL && GetMyUnit()->GetNowHp() > 0 )
		{
			CX2Unit::UnitData* pUnitData = &g_pData->GetMyUser()->GetSelectUnit()->AccessUnitData();
			std::wstringstream strStream;

			if ( pUnitData->m_ED >= _CONST_X2GAME_::g_iMaxED )
			{
				strStream << GET_STRING( STR_ID_2683 );
			}
			else
			{
				int iSumED		= 0;
				iSumED			= DungeonRewardED.SumED();

				pUnitData->m_ED += iSumED;

				if ( pUnitData->m_ED > _CONST_X2GAME_::g_iMaxED )
				{
					iSumED -= ( pUnitData->m_ED - _CONST_X2GAME_::g_iMaxED );
					pUnitData->m_ED = _CONST_X2GAME_::g_iMaxED;

				}

				strStream << GET_REPLACED_STRING( ( STR_ID_201, "i", iSumED ) );

				if ( DungeonRewardED.m_iBonusED > 0 )
				{
					if ( iSumED - DungeonRewardED.m_iED > 0 )
						strStream << GET_REPLACED_STRING( ( STR_ID_2625, "i", iSumED - DungeonRewardED.m_iED ) );
					else
						strStream << GET_REPLACED_STRING( ( STR_ID_2625, "i", 0 ) );
				}

				if ( iSumED > 0 && GetMyUnit()->GetUnitUID() == uiGetUnitUID )
				{
					std::wstringstream strStreamED;
					strStreamED << L"ED +" << iSumED;

					D3DXVECTOR3 pos = pCX2GUUser->GetPos();
					pos.y += 50.0f;
					if( NULL != g_pData->GetPicCharYellow() )
						g_pData->GetPicCharYellow()->DrawText( strStreamED.str().c_str(), pos, pCX2GUUser->GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
				if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUIInventory() )
				{
					g_pData->GetUIManager()->GetUIInventory()->SetEDString( pUnitData->m_ED );
				}

				if( iSumED == 0 )
					return;
			}
			if( NULL != g_pChatBox )
			{
				g_pChatBox->AddChatLogObtain(strStream.str().c_str(), CX2ChatBox::COT_ED);
			}
		}
	}
}



void CX2Game::AddChatLogByGetItem(
    const CX2Item::ItemTemplet* pItemTemplet,
    CX2GUUser* pCX2GUUser,
								bool bIsEDItem)
{
	if( false == bIsEDItem )
	{
        const wchar_t* pwszColorCode = pItemTemplet->GetNameColor_();

		wstring wstrMsg;
		if ( pCX2GUUser->IsMyUnit() == false )
		{
			// 미국은 을,를을 붙일 필요 없다.
			wstrMsg = GET_REPLACED_STRING( ( STR_ID_197, "SSSS",
				pCX2GUUser->GetUnit()->GetNickName(),
				pwszColorCode,
				pItemTemplet->GetFullName_(),
				L"" ) );
		}
		else
		{
			wstrMsg = GET_REPLACED_STRING( ( STR_ID_200, "SSS",
				pwszColorCode,
				pItemTemplet->GetFullName_(),
				L"" ) );
		}

		if( NULL != g_pChatBox )
		{
			wstring wstrColor = L"#CECEC88";							// (236, 236, 136)
			D3DXCOLOR coTextColor(0.92549f, 0.92549f, 0.53333f, 1.f);	// (236, 236, 136)

//#ifndef NOT_USE_DICE_ROLL
//			if( true == bDiceRolling )
//			{
//				const float MAGIC_DICE_ROLLING_TIME = 1.6f;
//				g_pChatBox->AddChatLog( wstrMsg.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor, true, false, MAGIC_DICE_ROLLING_TIME );
//			}
//			else
//#endif //NOT_USE_DICE_ROLL
			{
				g_pChatBox->AddChatLog( wstrMsg.c_str(), KEGS_CHAT_REQ::CPT_SYSTEM, coTextColor, wstrColor );
			}
		}
	}
}


wstring CX2Game::GetUserName(int inx)
{
	CX2Unit *pUnit = g_pData->GetMyUser()->GetUnitByIndex(inx);
	if(pUnit != NULL)
	{
		return pUnit->GetUnitData().m_NickName;;
	}

	return L"";
}




CX2Game::CinematicUI* CX2Game::GetCinematicUI( )
{
	return ( NULL != m_pCinematicUI ) ? m_pCinematicUI : NULL;
}

#ifdef ACTIVE_KOG_GAME_PERFORMANCE_CHECK
void CX2Game::StartTcpPingCheck()
{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_TCPPINGTIME_CHECK
	UINT uiSequence = KOGGamePerformanceCheck::GetInstance()->GetTcpPingCheckMgr()->SendPingCheckPacket();

	//
	KEGS_TCP_PING& kTcpPing = m_mapTcpPing[ uiSequence ];

	kTcpPing.m_uiSequence = uiSequence;
	kTcpPing.m_dwStartTime = timeGetTime();

	g_pData->GetServerProtocol()->SendPacket( EGS_TCP_PING, kTcpPing );
#endif //ACTIVE_KOG_GAME_PERFORMANCE_TCPPINGTIME_CHECK
}

void CX2Game::Handler_EGS_TCP_PING(  KEGS_TCP_PING& kEvent )
{
#ifdef ACTIVE_KOG_GAME_PERFORMANCE_TCPPINGTIME_CHECK
	KTcpPingMap::iterator it = m_mapTcpPing.find( kEvent.m_uiSequence );
	if( it != m_mapTcpPing.end() )
	{
		KOGGamePerformanceCheck::GetInstance()->GetTcpPingCheckMgr()->ReceivePingCheckPacket( timeGetTime() - it->second.m_dwStartTime, kEvent.m_uiSequence );
		m_mapTcpPing.erase( it );
	}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_TCPPINGTIME_CHECK
}
#endif //ACTIVE_KOG_GAME_PERFORMANCE_CHECK

int CX2Game::GetNowSubStageIndex()
{
	if( GetGameType() != GT_DUNGEON )
		return false;

	CX2DungeonGame* pDungeonGame = static_cast<CX2DungeonGame*>(this);
	if( NULL != pDungeonGame )
	{
		CX2Dungeon* pDungeon = pDungeonGame->GetDungeon();

		if( NULL != pDungeon && NULL != pDungeon->GetNowStage() &&
		    NULL != pDungeon->GetNowStage()->GetNowSubStage() )
		{
			return pDungeon->GetNowStage()->GetNowSubStageIndex();
		}
	}

	return -1;
}



void CX2Game::DeleteRemainingNpcWhenSubStageClear ()
{
	if( GetGameType() != GT_DUNGEON )
		return ;

	for( int i = 0; i < static_cast<int>(m_NPCUnitList.size()); ++i )
	{
		CX2GUNPC* pCX2GUNPC = m_NPCUnitList[i];
		if( NULL == pCX2GUNPC )
			continue;

		if ( true == pCX2GUNPC->GetIsRemainingNpcWhenStageCleard() )
		{
			pCX2GUNPC->SetUserGrapReset();

			vector<CX2GameUnit*>::iterator vItrGameUnit = m_UnitList.begin();
			while ( m_UnitList.end() != vItrGameUnit )
			{
				CX2GameUnit* pGameUnit = *vItrGameUnit;
				if ( pCX2GUNPC == pGameUnit )
				{
					pCX2GUNPC->ResetGameUnitWhoAttackedMe();
					DeleteNPCUnitByUID( static_cast<const UINT> ( pCX2GUNPC->GetUnitUID() ) );
					break;
				}
				else
					++vItrGameUnit;
			}

			m_NPCUnitList[i] = NULL;

			if ( null == m_optrFocusUnit )
				ChangeFocusUnit();

		}
	}
}

/** @function : GetIsExsitancePartyMemberHPLessThanPer
	@brief : 체력 @1 % 이하인 파티원이 존재 여부 체크
	@param : fHPRate_ (체크할 HP 비율), pGUUserWhoCallFunction_ (이 함수를 호출한 CX2GUUser 포인터로 체크 대상에서 제외 및 같은 파티인지 알아낼 용도)
*/
/*virtual*/
bool CX2Game::GetIsExsitancePartyMemberHPLessThanPer( const float fHPRate_, const CX2GUUser* pGUUserWhoCallFunction_ ) const
{
	if ( NULL != g_pData->GetPartyManager() )
	{
		BOOST_FOREACH( CX2GUUser* pGUUser, m_UserUnitList )
		{
			if ( NULL != pGUUser &&
				pGUUser->GetUnitUID() != pGUUserWhoCallFunction_->GetUnitUID() )
			{
				const float fHpPercent =
					pGUUser->GetNowHp() / pGUUser->GetMaxHp();

				if( fHpPercent > 0.f &&
					fHpPercent <= fHPRate_ )
					return true;
			}
		}
	}
	return false;
}



// 지정된 범위 안의 UnitUID 를 제외한 가장 가까운 NPC 를 가져옴
// 범위를 -1 로 지정하면 범위 체크를 하지 않고 모든 NPC 를 검색함
// MyTeam 을 지정하면 해당 Team 과 같으면 검색하지 않음
// Check Invincible 을 지정하면 무적 상태를 검색하지 않음
// 이쿠스, 인쿨로드 카드 NPC 적용 함수, kimjh
CX2GUNPC*	CX2Game::GetNearestNpcInSpecificRangeAndExceptUnitUID( const D3DXVECTOR3& vPos_, const UidType UnitUid_, const float fMaxRange_ = 1000, const int iMyTeam_ = -1, const bool bCheckInvincible_ = false )
{
	CX2GUNPC* pNearestNpc = NULL;
	float fDistance3Sq = fMaxRange_ * fMaxRange_;

	for( UINT i = 0; i < m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = m_NPCUnitList[i];
		if( NULL == pNPC )
			continue;

		if ( true == bCheckInvincible_ && true == pNPC->GetInvincible() )
			continue;

		if ( 0 <= iMyTeam_ && pNPC->GetTeam() == iMyTeam_ )
			continue;

		if( pNPC->GetUnitUID() != static_cast<UidType> ( UnitUid_) &&
			pNPC->GetNowHp() > 0.f && CX2GameUnit::GUSI_DIE != pNPC->GetGameUnitState() ) /*!pNPC->GetDyingStart())*/
		{
			float fTempDistance3Sq = GetDistance3Sq( vPos_, pNPC->GetPos() );
			if( fTempDistance3Sq < fDistance3Sq )
			{
				pNearestNpc		= pNPC;
				fDistance3Sq	= fTempDistance3Sq;
			}
		}
	}

	return pNearestNpc;
}

/** @function	: SetOfficiallyDebuffList
	@brief		: 해로운 효과라고 정의된 공식적인 디버프 리스트 ( 기획 파트에서 제공 )
*/
void CX2Game::SetOfficiallyDebuffList()
{
	m_vecPossibleOfficiallyDebuffList.clear();

	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_FIRE );					/// 화상
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_POISON );				/// 중독
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_SHOCK );				/// 단죄 ( 쇼크 )
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_STUN );					/// 스턴
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_COLD );					/// 동상
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_FROZEN );				/// 빙결
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_WOUND );				/// 상처 (컷텐던)
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_BLEEDING );				/// 출혈
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_MOTION_SLOWDOWN );		/// 동작 둔화 (로우킥 따위)
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_MOVE_JUMP_SLOWDOWN );	/// 이동 둔화
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_BLIND );				/// 시야 방해 (명중률 감소, 샌드 스톰 따위)
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_FLASH_BANG );			/// 섬광탄 (일렉트라 포톤 플레어)
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_PRESS );				/// 압착
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_BLIND_SMOKE );			/// 연무 (회피 감소)
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_BIND );					/// 동작 정지
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_CURSE );				/// 저주
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_CONFUSION );			/// 혼란
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_PANIC );				/// 공황 (판데모니움)
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_ARMOR_BREAK );			/// 무기 파괴
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_MAGIC_DEFFENCE_DROP );	/// 마력의 사슬
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_WEAPON_BREAK );			/// 방어구 파괴
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_ENTANGLE );				/// 인탱글
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_LEG_SPASM );			/// 다리 부상
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_DEATH_SENTENCE );		/// 죽음의 선고
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_AGING );				/// 노화
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_STIGMA );				/// 낙인의 사격
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_MARK_OF_COMMANDER );	/// 지휘관의 표식
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_WATER_HOLD );			/// 물의 결계
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_SIDE_EFFECT_MEDICINE );	/// 대두 ( 물약 부작용 )
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_SIDE_EFFECT_CREST );	/// 쇠약의 문장 ( 문장 부작용 )
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_ANGER_OF_DRYAD );		/// 드라이어드의 분노
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_SUPPRESSION );			/// 제압
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_STIGMA_OF_FIRE );		/// 엘리시스(블레이징 하트) - 불꽃의 낙인 디버프
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_CHANGE_PHYSIC_DEFENCE_AND_PHYSIC_DAMAGE );		/// 엘리시스(그랜드 마스터) - 도발 디버프, 물리 공격력 증가 / 물리 방어력 감소
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_UNEXTINGUISHABLE_FIRE );/// 엘리시스(블레이징 하트) - 꺼지지 않는 불꽃 디버프
	m_vecPossibleOfficiallyDebuffList.push_back( BTI_DEBUFF_HIGH_FEVER );			/// 불속성 저항을 감소시키는 디버프.
}

/** @function	: DisplayAddHPInformation
	@brief		: HP 회복량 표시
	@param		: HP 회복량, 유닛 위치, 유닛 방향 벡터, 퍼센트 표기 여부
*/
void CX2Game::DisplayAddHPInformation( IN const float fAddHPValue_, IN const D3DXVECTOR3 vPos_, IN const D3DXVECTOR3 vDirVector_, IN bool bDisplayPercent/*= true*/ )
{
	/// HP 회복 표기는 좀 더 위로 표시한다.
	D3DXVECTOR3 vPos = vPos_;
	vPos.y += 50.f;

	/// 회복량 표시
	WCHAR wszText[64] = L"";

	if ( true == bDisplayPercent )	/// 퍼센트 표기
		StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"HP+ %d PERCENT", static_cast<int>( fAddHPValue_ ) );
	else							/// 회복량 표기
		StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"HP+ %d", static_cast<int>( fAddHPValue_ ) );

	if( NULL != g_pData && NULL != g_pData->GetPicCharBlue() )
		g_pData->GetPicCharBlue()->DrawText( wszText, vPos, vDirVector_, CKTDGPicChar::AT_CENTER );
}

/** @function	: DisplayAddHPInformation
	@brief		: MP 회복량 표시
	@param		: MP 회복량, 유닛 위치, 유닛 방향 벡터
*/
void CX2Game::DisplayAddMPInformation( IN const float fAddMPValue_, IN const D3DXVECTOR3 vPos_, IN const D3DXVECTOR3 vDirVector_ )
{
	/// 회복량 표시
	WCHAR wszText[64] = L"";
	StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MP+ %d", static_cast<int>( fAddMPValue_ ) );

	if( NULL != g_pData && NULL != g_pData->GetPicCharBlue() )
		g_pData->GetPicCharBlue()->DrawText( wszText, vPos_, vDirVector_, CKTDGPicChar::AT_CENTER );
}

/** @function	: DisplayDamageInformation
	@brief		: 데미지 표시
	@param		: 데미지량, 유닛 위치, 유닛 방향 벡터
*/
void CX2Game::DisplayDamageInformation( IN const float fDamageValue_, IN const D3DXVECTOR3 vHeadBonePos_, IN const D3DXVECTOR3 vDirVector_ )
{
	/// 데미지 출력 위치 설정
	D3DXVECTOR3 vPos = vHeadBonePos_;
	//vPos.y += 70.0f;

	/// 회복량 표시
	if( NULL != g_pData && NULL != g_pData->GetPicChar() )
		g_pData->GetPicChar()->DrawText( static_cast<int>( fDamageValue_ ), vPos, vDirVector_, CKTDGPicChar::AT_CENTER );
}

void CX2Game::ChatBoxLog( const CHAR* pLog, float fLog)
{
#if X2_DEV_TOOLS
	if( NULL != g_pChatBox )
	{
		wstring wstrStateName = L"";
		ConvertUtf8ToWCHAR( wstrStateName, pLog );

		g_pChatBox->AddChatLog( wstrStateName.c_str(), KEGS_CHAT_REQ::CPT_TOTAL, D3DXCOLOR(0.5882f, 1, 0, 1), L"#C96ff00", false );

		if( false == IsSamef(0.f,fLog))
		{
			WCHAR wBuf1[10];
			StringCchPrintfW( wBuf1, 10, L"%f", fLog );
			g_pChatBox->AddChatLog( wBuf1, KEGS_CHAT_REQ::CPT_TOTAL, D3DXCOLOR(0.5882f, 1, 0, 1), L"#C96ff00", false );
		}
	}
#else
	g_pMain->KTDGUIOKMsgBox( D3DXVECTOR2(250,300), L"사내서버 전용 함수가 설정되어있습니다.\nNPC스크립트의 ChatBoxLog()을 제거해주세요", g_pMain->GetNowState() );
#endif // _IN_HOUSE_
}


void CX2Game::FlushSendFrameAverage()
{

    int iGameType = GetGameType();
	if( true == g_pData->GetBattleFieldManager().GetIsBossRaidCurrentField() )
	{
		iGameType = GT_DUNGEON;
	}

	m_kGameStatistics.Send_EGS_FRAME_AVERAGE_REQ( iGameType );
    m_kGameStatistics.Init();
}


bool CX2Game::IsEnableCodeEvent_LUA( int iScriptID )
{
	IF_EVENT_ENABLED( iScriptID )
	{
		return true;
	}

	return false;
}

D3DXVECTOR3 CX2Game::GetNearestNPCUnitPosOnSameDirection_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange, D3DXVECTOR3 vDir )
{
	D3DXVECTOR3 retPos(0, 0, 0);
	float fDistance = fMaxRange;

	// 좌우 방향만 보는 것이므로 y 제거
	vDir.y = 0;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( true == pNPC->GetInvincible() )
			continue;

		if( 0.f >= pNPC->GetNowHp() )
			continue;

		if( pNPC->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
			continue;

		// 방향이 안 맞으면 컨티뉴
		D3DXVECTOR3 vDirB = pNPC->GetPos() - pos;
		vDirB.y = 0;

		if( D3DXVec3Dot( &vDir, &vDirB ) < 0.f )
		{
			continue;
		}

		float fTempDist = GetDistance( pos, pNPC->GetPos() );
		if( fTempDist < fDistance && fTempDist > fMinRange )
		{
			retPos = pNPC->GetPos();
			fDistance = fTempDist;
		}
	}
	return retPos;
}
CX2GameUnit* CX2Game::GetNearestNPCUnitOnSameDirection_LUA( D3DXVECTOR3 pos, float fMinRange, float fMaxRange, D3DXVECTOR3 vDir )
{
	CX2GameUnit* retUnit = NULL;
	float fDistance = fMaxRange;

	// 좌우 방향만 보는 것이므로 y 제거
	vDir.y = 0;

	for( int i = 0; i < (int)m_NPCUnitList.size(); i++ )
	{
		CX2GUNPC* pNPC = (CX2GUNPC*)m_NPCUnitList[i];
		if( pNPC == NULL )
			continue;

		if( true == pNPC->GetInvincible() )
			continue;

		if( 0.f >= pNPC->GetNowHp() )
			continue;

		if( pNPC->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
			continue;

		// 실질적으로 적NPC 만을 타겟하기 위한 기능이므로 추가합니다. by 박진웅 2014.01.27
		if( pNPC->GetTeam() != CX2Room::TN_MONSTER )
			continue;

		// 방향이 안 맞으면 컨티뉴
		D3DXVECTOR3 vDirB = pNPC->GetPos() - pos;
		vDirB.y = 0;

		if( D3DXVec3Dot( &vDir, &vDirB ) < 0.f )
		{
			continue;
		}

		float fTempDist = GetDistance( pos, pNPC->GetPos() );
		if( fTempDist < fDistance && fTempDist > fMinRange )
		{
			retUnit = pNPC;
			fDistance = fTempDist;
		}
	}
	return retUnit;
}
