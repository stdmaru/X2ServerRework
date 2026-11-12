#include "stdafx.h"
#include "./X2SquareUnit.h"

//#pragma region CX2SquareUnit
CX2SquareUnit::CX2SquareUnit( bool bMyUnit, CX2Unit* pUnit, const WCHAR* pScriptName, bool bTFieldUnit
	, char cWeddingStatus/* = 0*/
	)
//{{ robobeg : 2008-10-28
: m_LuaManager( g_pKTDXApp->GetLuaBinder()->GetLuaState(), 0, true )
, m_pXSkinAnim(NULL)
, m_pMesh(NULL)
//}} robobeg : 2008-10-28
{
	SetAlphaObject( false );
	SetLayer( XL_UNIT_0 );

	m_bFirstGetPacket		= true;
	m_bInit					= false;
	m_bMyUnit				= bMyUnit;
	m_UnitUID				= pUnit->GetUID();
	m_pUnit					= pUnit;
	m_ScriptFileName		= pScriptName;

	m_fTime					= 0.0f;
	m_fElapsedTime			= 0.0f;
	m_bShow					= true;

	m_pUnitViewer			= NULL;

	m_Scale					= D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
	m_DirDegree				= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_DirVector				= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	//m_bFall					= false;
	m_bFootOnLine			= false;
	m_LandPosition			= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_fPacketTime			= 1.0f;

	m_iActID				= SUS_DUMY;
	m_iOUFrameCount			= 0;
	m_iOUNextFrameCount		= 0;
	m_iNowStateIndex		= 0;
	m_SyncIsRight			= true;
	m_bFirstSync			= false;

	m_bTFieldUnit			= bTFieldUnit;

	m_iPartyUID				= -1;

	m_vecSyncUnitState.reserve( 16 );

	if(m_bTFieldUnit == false)
	{
		if ( 0 != cWeddingStatus )
		{
			if( NULL != g_pWeddingGame )
				g_pWeddingGame->GetStartPosByWeddingStatus( static_cast<SEnum::WEDDING_STATUS>(cWeddingStatus), m_SyncData.m_Pos );

			m_eWeddingStatus = static_cast<SEnum::WEDDING_STATUS>(cWeddingStatus);
		}
		else
		{
			CX2World* pWorld = g_pSquareGame->GetWorld();
			if( NULL != pWorld && NULL != pWorld->GetLineMap() )
				m_SyncData.m_Pos = pWorld->GetLineMap()->GetStartPosition( pWorld->GetLineMap()->GetRandomStartPosIndex() );
		}
	}
	else
	{
		SEnum::VILLAGE_MAP_ID villageId;
		int iLastTouchLineIndex = 0;
		g_pData->GetLocationManager()->GetVillage(villageId, m_SyncData.m_Pos, &iLastTouchLineIndex );
		m_SyncData.m_LastTouchLineIndex = (char) iLastTouchLineIndex;
		//��/�� �ٶ󺸴� ������ ����� ���� �̿� �ϵ��� ����
		m_SyncData.m_bIsRight = g_pData->GetLocationManager()->GetVillageIsRight();
	}


	m_bPlanRender			= false;
	//m_pPlanMesh				= g_pKTDXApp->GetDeviceManager()->OpenXMesh( L"SquarePlan.Y" );
	//m_pPlanMeshXET			= NULL;

	m_hHeadMarkerMy					= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hHeadMarkerPartyMy			= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hHeadMarkerParty				= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hSeqEmblem					= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hHeadMarkerParticleMy			= INVALID_PARTICLE_HANDLE;
	m_hHeadMarkerParticlePartyMy	= INVALID_PARTICLE_HANDLE;
	m_hHeadMarkerParticleParty		= INVALID_PARTICLE_HANDLE;

	m_hPart_Emblem_200				= INVALID_PARTICLE_HANDLE;

	m_PersonalShopState				= CX2SquareUnit::PSS_NONE;

	m_eShopType						= SEnum::AST_NORMAL;

	m_PersonalShopName				= L"";

	m_hShop = INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hPremiumShop = INVALID_PARTICLE_SEQUENCE_HANDLE;

    m_pParticleSystem = NULL;

	if(m_bTFieldUnit == false)
	{
		if( NULL != g_pSquareGame )
		{
			m_pParticleSystem = g_pSquareGame->GetMajorParticle();
		}
		else if ( NULL != g_pWeddingGame )
		{
			m_pParticleSystem = g_pWeddingGame->GetMajorParticle();
		}
	}
	else
	{
		if( NULL != g_pTFieldGame )
			m_pParticleSystem = g_pTFieldGame->GetUiParticle();
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	m_hShop	= m_pParticleSystem->CreateSequenceHandle( NULL,  L"SquareShop", 0,0,0 );
	pSeq	= m_pParticleSystem->GetInstanceSequence( m_hShop );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( false );
	}

	m_hPremiumShop	= m_pParticleSystem->CreateSequenceHandle( NULL,  L"PremiumSquareShop", 0,0,0 );
	pSeq = m_pParticleSystem->GetInstanceSequence( m_hPremiumShop );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( false );
	}


    m_pMesh = NULL;

	// note!! ���� �߰��Ǹ� ����, Ȥ�� unit_type����
	switch( m_pUnit->GetType() )
	{
		case CX2Unit::UT_ELSWORD:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Elsword_Base_SD.x" );
			} break;
		case CX2Unit::UT_ARME:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Arme_Base_SD.x" );
			} break;

		case CX2Unit::UT_LIRE:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Lire_Base_SD.x" );
			} break;

		case CX2Unit::UT_RAVEN:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Raven_Base_SD.x" );
			} break;

		case CX2Unit::UT_EVE:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Eve_Base_SD.x" );
			} break;

		//{{ kimhc // 2010.11.24 // 2010-12-23 New Character CHUNG

		case CX2Unit::UT_CHUNG:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_CHUNG_Base_SD.X" );
			} break;

//}} kimhc // 2010.11.24 //  2010-12-23 New Character CHUNG

		case CX2Unit::UT_ARA:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Ara_Base_SD.X" );
			} break;

		case CX2Unit::UT_ELESIS:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_EL_Base_SD.X" );
			} break;

		case CX2Unit::UT_ADD:
			{
				m_pMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Mesh_Add_Base_SD.X" );
			} break;
	default:
		break;
	}

	if( m_pMesh != NULL )
	{
        ASSERT( m_pXSkinAnim == NULL );
		m_pXSkinAnim = CKTDGXSkinAnim::CreateSkinAnim();
		m_pXSkinAnim->SetApplyMotionOffset( true );
		m_pXSkinAnim->SetAnimXSkinMesh( m_pMesh );
		m_pXSkinAnim->AddModelXSkinMesh( m_pMesh );
		m_pXSkinAnim->SetShowObject(false);

		//m_pXSkinAnim->GetMatrix().Move( m_vPos );
		//m_pXSkinAnim->GetMatrix().RotateDegree( m_vRot );
		//m_pXSkinAnim->GetMatrix().Scale( m_vScale );
	}
	else
	{
		ASSERT( ! "m_pXSkinAnim is NULL!!!" );
	}

//#ifdef TITLE_SYSTEM
    m_iTitleId = 0;
//#endif

	m_ePlayedEmotion = CX2Unit::ET_NONE;

	m_pTextureRank = NULL;

	if ( NULL != g_pMain->GetPVPEmblem() )
	{
	CX2PVPEmblem::PVPEmblemData *pPvpEmblemData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( m_pUnit->GetPvpRank() );

		if( pPvpEmblemData != NULL )
			m_pTextureRank = g_pKTDXApp->GetDeviceManager()->OpenTexture( pPvpEmblemData->m_TextureName );
	}

	//{{ oasis907 : ����� [2010.5.17] //
	m_pTextureServer = g_pKTDXApp->GetDeviceManager()->OpenTexture( L"DLG_UI_Common_Texture11.tga" );
	//}}

	for(int i=0; i < AVATAR_EMOTION_NUM; ++i)
		m_bMixedEmotion[i] = false;
	m_pAvatarEmotionSound = NULL;

	m_iED = 0;

	m_SendPacketCount = 0;
	m_SyncIsRight_Old = true;
	//Init();

	m_bForceMoveSpeed = false;
	m_fCanNotInputTime = -1.f;

#ifdef HANABI_VILLAGE
	m_fRedShowTimer = 0.f;
	m_fBlueShowTimer = 0.f;
#endif // HANABI_VILLAGE


	m_wstrLobbyWaitMotionName = L"";
	m_ElapsedTimeRidingWaitHabit = 5.f;

	m_bIsPlayAvatarEmotionSoundWithoutEmotion = false;
}
//#pragma endregion ������

//#pragma region AppendToDeviceList
//{{ robobeg : 2011-03-18
/*static*/
void    CX2SquareUnit::AppendToDeviceList( CKTDXDeviceDataList& listInOut_, CX2Unit* pUnit, bool bTField, CX2UnitViewerUI::UNIT_VIEWER_STATE eState )
{
	if ( pUnit == NULL )
		return;

	CX2UnitViewerUI::AppendToDeviceList( listInOut_, pUnit, eState );

	switch( pUnit->GetType() )
	{
	case CX2Unit::UT_ELSWORD:
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Mesh_Elsword_Base_SD.x" );
		} break;
	case CX2Unit::UT_ARME:
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Mesh_Arme_Base_SD.x" );
		} break;

	case CX2Unit::UT_LIRE:
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Mesh_Lire_Base_SD.x" );
		} break;

	case CX2Unit::UT_RAVEN:
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Mesh_Raven_Base_SD.x" );
		} break;

	case CX2Unit::UT_EVE:
		{
			g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Mesh_Eve_Base_SD.x" );
		} break;
	default:
		break;
	}

	if ( NULL != g_pMain->GetPVPEmblem() )
	{
		const CX2PVPEmblem::PVPEmblemData* pData = g_pMain->GetPVPEmblem()->GetPVPEmblemData( pUnit->GetPvpRank() );
		if ( pData != NULL && pData->m_TextureName.empty() == false )
		{
			g_pKTDXApp->GetDeviceManager()->AppendTexture( listInOut_, pData->m_TextureName );
		}//if
		g_pKTDXApp->GetDeviceManager()->AppendTexture( listInOut_, L"DLG_UI_Common_Texture11.tga" );
	}

	CKTDGParticleSystem* pParticleSystem = NULL;
	if( bTField == false)
	{
		if ( g_pSquareGame != NULL )
			pParticleSystem = g_pSquareGame->GetMajorParticle();
		else if ( NULL != g_pWeddingGame )
		{
			pParticleSystem = g_pWeddingGame->GetMajorParticle();
		}
	}
	else
	{
		if ( g_pTFieldGame != NULL )
			pParticleSystem = g_pTFieldGame->GetUiParticle();
	}//if.. else..

	if ( pParticleSystem != NULL )
	{
		pParticleSystem->AppendToDeviceList( listInOut_,   L"HeadMarkerRedMy3D" );
		pParticleSystem->AppendToDeviceList( listInOut_,   L"HeadMarkerBlueMy3D" );
		pParticleSystem->AppendToDeviceList( listInOut_,   L"SquareShop" );
		pParticleSystem->AppendToDeviceList( listInOut_,   L"PremiumSquareShop" );
	}//if

//#ifdef TITLE_SYSTEM
	CKTDGParticleSystem* pUIParticleSystem = g_pData->GetUIMajorParticle();
	if ( pUIParticleSystem != NULL )
	{
		int titleId = pUnit->GetTitleId();
		pUIParticleSystem->AppendToDeviceList( listInOut_, g_pData->GetTitleManager()->GetTitleModel(titleId) );
	}
//#else   TITLE_SYSTEM
//	if ( pParticleSystem != NULL )
//	{
//		pParticleSystem->AppendToDeviceList( listInOut_,   "UnitEmblem200" );
//		pParticleSystem->AppendToDeviceList( listInOut_,   "UnitEmblemGoldMedal" );
//	}//if
//#endif	TITLE_SYSTEM
}//CX2SquareUnit::AppendToDeviceList()
//}} robobeg : 2011-03-18
//#pragma endregion ����̽� ����Ʈ�� SD ���, ������ ���� ��ƼŬ, �Ӹ� �� �ﰢ�� ��ƼŬ �� ÷��

//#pragma region ~CX2SquareUnit
CX2SquareUnit::~CX2SquareUnit(void)
{
	if( NULL != m_pUnitViewer )
	{
		m_pUnitViewer->SetManagerObject( NULL );
	}
	SAFE_DELETE_KTDGOBJECT( m_pUnitViewer );

	SAFE_CLOSE( m_pMesh );
	SAFE_CLOSE( m_pTextureRank );

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		g_pData->GetPetManager()->RemovePet( GetUnit()->GetUnitData().m_UnitUID );
	}

	if ( NULL != CX2RidingPetManager::GetInstance() )
		CX2RidingPetManager::GetInstance()->RemoveRidingPet( GetUnit()->GetUnitData().m_UnitUID );

	SAFE_CLOSE( m_pTextureServer );

	SAFE_DELETE_KTDGOBJECT( m_pXSkinAnim );

	if( m_bMyUnit == false )
	{
		SAFE_DELETE( m_pUnit );
	}

	// CX2SquareUnit�� shared_ptr�� ���Ǳ� ������ ���� delete���� �� �� ���� ������ m_pParticleSystem�� �̹� invalid�� ������ �� �ִ�. Ư�� ������ ������ ��.
	bool bValidParticleSystem = false;
	if( true == m_bTFieldUnit && NULL != g_pTFieldGame )
	{
		bValidParticleSystem = true;
	}
	else if( false == m_bTFieldUnit )
	{
		if( NULL != g_pSquareGame
			|| NULL != g_pWeddingGame
			)
			bValidParticleSystem = true;
	}

	if( true == bValidParticleSystem )
	{
		m_pParticleSystem->DestroyInstanceHandle( m_hHeadMarkerMy );
		m_pParticleSystem->DestroyInstanceHandle( m_hHeadMarkerPartyMy );
		m_pParticleSystem->DestroyInstanceHandle( m_hHeadMarkerParty );

		m_pParticleSystem->DestroyInstanceHandle( m_hShop );
		m_pParticleSystem->DestroyInstanceHandle( m_hPremiumShop );
	}

//#ifdef TITLE_SYSTEM
    g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqEmblem );
//#else
//	m_pParticleSystem->DestroyInstanceHandle( m_hSeqEmblem );
//#endif

	SAFE_CLOSE( m_pAvatarEmotionSound );

}
//#pragma endregion �Ҹ���

//#pragma region NotifyShowObjectChanged
/*virtual*/
void CX2SquareUnit::NotifyShowObjectChanged()
{
//{{ robobeg : 2008-10-17
	if ( m_pUnitViewer != NULL )
	    m_pUnitViewer->NotifyShowObjectChanged();

    bool bShow = GetShowObject();
//}}  robobeg : 2008-10-17

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerMy );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

	pSeq = m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerPartyMy );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

	pSeq = m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerMy );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

	pSeq = m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerPartyMy );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

	pSeq = m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerParty );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

//#ifdef TITLE_SYSTEM
	pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#else
//	pSeq = m_pParticleSystem->GetInstanceSequence( m_hSeqEmblem );
//#endif TITLE_SYSTEM


	if( NULL != pSeq )
	{
		if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
			pSeq->SetShowObject( false );
		else
			pSeq->SetShowObject( bShow );

	}

	pSeq = m_pParticleSystem->GetInstanceSequence( m_hShop );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

	pSeq = m_pParticleSystem->GetInstanceSequence( m_hPremiumShop );
	if( NULL != pSeq )
	{
		pSeq->SetShowObject( bShow );
	}

	if( g_pData != NULL && g_pData->GetPetManager() != NULL )
	{
		g_pData->GetPetManager()->NotifyShowObjectChanged( GetUnit()->GetUnitData().m_UnitUID, bShow );
	}

	if ( NULL != CX2RidingPetManager::GetInstance() )
		CX2RidingPetManager::GetInstance()->NotifyShowObjectChanged( GetUnit()->GetUnitData().m_UnitUID, bShow );
}
//#pragma endregion ������ ��ü �뺸

//#pragma region Init
void CX2SquareUnit::Init()
{
	m_bInit = true;		/// �ʱ�ȭ ��

	InitSystem();
	InitViewer();
	InitState();

	// 2013.03.28 ��ƼŬ ���� �ʱ�ȭ �ڵ� --> �Լ��� ���� InitParticle()
	InitParticle();
	InitWaitMotion();


	UpdateEquippedEmblem();

//	SendPacketP2P();
}
//#pragma endregion �ʱ�ȭ

void CX2SquareUnit::InitSystem()
{
	//��ũ��Ʈ ������ �ε��Ѵ�
//{{ robobeg : 2008-10-28
	//g_pKTDXApp->LoadLuaTinker( m_ScriptFileName.c_str() );
	//g_pKTDXApp->LoadAndDoMemory( &m_LuaManager, L"Enum.lua" );
	g_pKTDXApp->LoadAndDoMemory( &m_LuaManager, m_ScriptFileName.c_str() );
	m_LuaManager.ExportFunctionsToGlobalEnv();
//}} robobeg : 2008-10-28

	if( m_LuaManager.BeginTable( "INIT_PHYSIC" ) == true )
	{
		m_LuaManager.GetValue( "RELOAD_ACCEL",		m_OrgPhysicParam.fReloadAccel );
		m_LuaManager.GetValue( "G_ACCEL",			m_OrgPhysicParam.fGAccel );
		m_LuaManager.GetValue( "MAX_G_SPEED",		m_OrgPhysicParam.fMaxGSpeed );

		m_LuaManager.GetValue( "WALK_SPEED",		m_OrgPhysicParam.fWalkSpeed );
		m_LuaManager.GetValue( "RUN_SPEED",			m_OrgPhysicParam.fRunSpeed );
		m_LuaManager.GetValue( "JUMP_SPEED",		m_OrgPhysicParam.fJumpSpeed );
		m_LuaManager.GetValue( "DASH_JUMP_SPEED",	m_OrgPhysicParam.fDashJumpSpeed );

		m_PhysicParam = m_OrgPhysicParam;

		m_LuaManager.EndTable();
	}
}

void CX2SquareUnit::InitViewer()
{
	SAFE_DELETE_KTDGOBJECT( m_pUnitViewer );
	m_pUnitViewer = CX2UnitViewerUI::CreateUnitViewerUI();
//{{ robobeg : 2008-10-21
// UnitViewer �� ���� part���� KTDGManager ���� manage�ϱ� ���� �ݵ�� �ʿ�!
    ASSERT( m_pUnitViewer );
    m_pUnitViewer->SetManagerObject( this );
//}} robobeg : 2008-10-21

	m_pUnitViewer->SetUnit( m_pUnit, CX2UnitViewerUI::UVS_FIELD );

	//{{ ����� : [2009/10/30] //	���ǹ� �߰�
	if( m_pUnitViewer->GetXSkinAnim() == NULL || m_pUnitViewer->GetXSkinAnim()->GetAnimXSkinMesh() == NULL )
		return;

	SetCenter( m_pUnitViewer->GetXSkinAnim()->GetAnimXSkinMesh()->GetCenter() );
	SetBoundingRadius( m_pUnitViewer->GetXSkinAnim()->GetAnimXSkinMesh()->GetBoundingRadius() );

	if( g_pData != NULL && g_pData->GetPetManager() != NULL && true == m_bTFieldUnit )
	{
		if( m_pUnit->GetPetInfo() != NULL )
		{
			KPetInfo *pPetInfo = m_pUnit->GetPetInfo();
			CX2PET::PetInfo petInfo;

			petInfo.m_PetUid = pPetInfo->m_iPetUID;
			petInfo.m_PetId = pPetInfo->m_iPetID;
			petInfo.m_Evolution_Step = pPetInfo->m_cEvolutionStep;
			petInfo.m_wstrPetName = pPetInfo->m_wstrPetName;
			petInfo.m_Intimacy = (float)pPetInfo->m_iIntimacy;
			petInfo.m_Satiety = pPetInfo->m_sSatiety;
			petInfo.m_Emotion = (int)pPetInfo->m_sEmotion;
			petInfo.m_Extroversion = (int)pPetInfo->m_sExtroversion;
			petInfo.m_bIsDropItemPickup = pPetInfo->m_bAutoLooting;
			petInfo.m_bSummon = true;

			g_pData->GetPetManager()->CreatePet( GetUnit()->GetUnitData().m_UnitUID, petInfo );
		}
		else
		{
			g_pData->GetPetManager()->RemovePet( GetUnit()->GetUnitData().m_UnitUID );
		}
	}

	if( NULL != CX2RidingPetManager::GetInstance() && NULL == g_pSquareGame )
	{
		if ( NULL != m_pUnit->GetRidingPetInfo() )
		{
			KRidingPetInfo *pRidingPetInfo = m_pUnit->GetRidingPetInfo();
			ProcessRidingOn( true, *pRidingPetInfo );
		}
		else
		{
			ProcessRidingOn( false );
		}


	}

}

void CX2SquareUnit::InitState()
{
	if( m_LuaManager.BeginTable( "INIT_STATE" ) == true )
	{
		m_StateNameMap.clear();
		m_StateIDMap.clear();

		int index = 1;
		std::string stateName = "";
		while( m_LuaManager.GetValueUtf8( index, stateName ) == true )
		{
			m_StateNameMap.insert( std::make_pair( index, stateName.c_str() ) );
			m_StateIDMap.insert( std::make_pair( stateName.c_str(), index ) );
			index++;
		}

		LUA_GET_VALUE_UTF8( m_LuaManager, "WAIT", stateName, "" );
		m_StateID.m_Wait = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "WALK", stateName, "" );
		m_StateID.m_Walk = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_UP", stateName, "" );
		m_StateID.m_JumpUp = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "JUMP_DOWN", stateName, "" );
		m_StateID.m_JumpDown = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH", stateName, "" );
		m_StateID.m_Dash = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_JUMP_UP", stateName, "" );
		m_StateID.m_DashJumpUp = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "DASH_JUMP_DOWN", stateName, "" );
		m_StateID.m_DashJumpDown = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_ON", stateName, "" );
		m_StateID.m_iRidingOn = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_OFF", stateName, "" );
		m_StateID.m_iRidingOff = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_WAIT", stateName, "" );
		m_StateID.m_iRidingWait = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_WAIT_HABIT", stateName, "" );
		m_StateID.m_iRidingWaitHabit = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_WALK", stateName, "" );
		m_StateID.m_iRidingWalk = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_JUMP_UP", stateName, "" );
		m_StateID.m_iRidingJumpUp = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_JUMP_DOWN", stateName, "" );
		m_StateID.m_iRidingJumpDown = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_DASH", stateName, "" );
		m_StateID.m_iRidingDash = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_DASH_JUMP_UP", stateName, "" );
		m_StateID.m_iRidingDashJumpUp = GetStateID( stateName.c_str() );

		LUA_GET_VALUE_UTF8( m_LuaManager, "RIDING_DASH_JUMP_DOWN", stateName, "" );
		m_StateID.m_iRidingDashJumpDown = GetStateID( stateName.c_str() );

		m_LuaManager.EndTable(); // INIT_STATE
	}
}

void CX2SquareUnit::InitParticle()
{
	m_hHeadMarkerMy			= m_pParticleSystem->CreateSequenceHandle( NULL,  L"HeadMarkerRedMy3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	m_hHeadMarkerPartyMy	= m_pParticleSystem->CreateSequenceHandle( NULL,  L"HeadMarkerBlueMy3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	m_hHeadMarkerParty		= m_pParticleSystem->CreateSequenceHandle( NULL,  L"HeadMarkerBlue3D", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
    m_hHeadMarkerParticleMy = INVALID_PARTICLE_HANDLE;
    m_hHeadMarkerParticleParty = INVALID_PARTICLE_HANDLE;
    m_hHeadMarkerParticlePartyMy = INVALID_PARTICLE_HANDLE;

	CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarkerMy		= m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerMy	);
	CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarkerPartyMy	= m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerPartyMy );
	CKTDGParticleSystem::CParticleEventSequence* pSeqHeadMarkerParty	= m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerParty	 );

	if( pSeqHeadMarkerMy != NULL )
	{
		pSeqHeadMarkerMy->SetTrace( true );
		pSeqHeadMarkerMy->SetBillBoardType( CKTDGMatrix::BT_Y );
        m_hHeadMarkerParticleMy = pSeqHeadMarkerMy->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
		pSeqHeadMarkerMy->SetShowObject(false);
	}

	if( pSeqHeadMarkerParty != NULL )
	{
		pSeqHeadMarkerParty->SetTrace( true );
		pSeqHeadMarkerParty->SetBillBoardType( CKTDGMatrix::BT_Y );
    	m_hHeadMarkerParticleParty = pSeqHeadMarkerParty->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
		pSeqHeadMarkerParty->SetShowObject(false);
	}

	if( pSeqHeadMarkerPartyMy != NULL )
	{
		pSeqHeadMarkerPartyMy->SetTrace( true );
		pSeqHeadMarkerPartyMy->SetBillBoardType( CKTDGMatrix::BT_Y );
		m_hHeadMarkerParticlePartyMy = pSeqHeadMarkerPartyMy->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
		pSeqHeadMarkerPartyMy->SetShowObject(false);
	}
}

/** @function 	: InitWaitMotion
	@brief 		: SquareLua �ϳ��� �����
*/
void CX2SquareUnit::InitWaitMotion()
{
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_LIRE_ELVEN_RANGER:
	case CX2Unit::UC_RAVEN_FIGHTER:
	case CX2Unit::UC_EVE_NASOD:
	case CX2Unit::UC_CHUNG_IRON_CANNON:
	case CX2Unit::UC_ARA_MARTIAL_ARTIST:
	case CX2Unit::UC_ELESIS_KNIGHT:
	case CX2Unit::UC_ADD_NASOD_RULER:
		m_wstrLobbyWaitMotionName = L"LobbyWait";
		break;

	case CX2Unit::UC_ELSWORD_KNIGHT:
		m_wstrLobbyWaitMotionName = L"Knight_LobbyWait";
		break;
	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
		m_wstrLobbyWaitMotionName = L"MagicKnight_LobbyWait";
		break;
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
		m_wstrLobbyWaitMotionName = L"ETK_LobbyWait";
		break;
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		m_wstrLobbyWaitMotionName = L"LK_LobbyWait";
		break;
	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		m_wstrLobbyWaitMotionName = L"RS_LobbyWait";
		break;
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		m_wstrLobbyWaitMotionName = L"EIS_LobbyWait";
		break;

	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
		m_wstrLobbyWaitMotionName = L"HM_LobbyWait";
		break;
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
		m_wstrLobbyWaitMotionName = L"DM_LobbyWait";
		break;
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
		m_wstrLobbyWaitMotionName = L"BM_LobbyWait";
		break;
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		m_wstrLobbyWaitMotionName = L"EM_LobbyWait";
		break;
	case CX2Unit::UC_ARME_VOID_PRINCESS:
		m_wstrLobbyWaitMotionName = L"VP_LobbyWait";
		break;
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		m_wstrLobbyWaitMotionName = L"DW_LobbyWait";
		break;

	case CX2Unit::UC_LIRE_COMBAT_RANGER:
		m_wstrLobbyWaitMotionName = L"CR_LobbyWait";
		break;
	case CX2Unit::UC_LIRE_SNIPING_RANGER:
		m_wstrLobbyWaitMotionName = L"SR_LobbyWait";
		break;
	case CX2Unit::UC_LIRE_TRAPPING_RANGER:
		m_wstrLobbyWaitMotionName = L"RTR_LobbyWait";
		break;
	case CX2Unit::UC_LIRE_WIND_SNEAKER:
		m_wstrLobbyWaitMotionName = L"LWS_LobbyWait";
		break;
	case CX2Unit::UC_LIRE_GRAND_ARCHER:
		m_wstrLobbyWaitMotionName = L"LGA_LobbyWait";
		break;
	case CX2Unit::UC_LIRE_NIGHT_WATCHER:
		m_wstrLobbyWaitMotionName = L"RNW_LobbyWait";
		break;

	case CX2Unit::UC_RAVEN_SOUL_TAKER:
		m_wstrLobbyWaitMotionName = L"ST_LobbyWait";
		break;
	case CX2Unit::UC_RAVEN_OVER_TAKER:
		m_wstrLobbyWaitMotionName = L"OT_LobbyWait";
		break;
	case CX2Unit::UC_RAVEN_WEAPON_TAKER:
		m_wstrLobbyWaitMotionName = L"RWT_LobbyWait";
		break;
	case CX2Unit::UC_RAVEN_BLADE_MASTER:
		m_wstrLobbyWaitMotionName = L"RBM_LobbyWait";
		break;
	case CX2Unit::UC_RAVEN_RECKLESS_FIST:
		m_wstrLobbyWaitMotionName = L"RRF_LobbyWait";
		break;
	case CX2Unit::UC_RAVEN_VETERAN_COMMANDER:
		m_wstrLobbyWaitMotionName = L"RVC_LobbyWait";
		break;

	case CX2Unit::UC_EVE_EXOTIC_GEAR:
		m_wstrLobbyWaitMotionName = L"EG_LobbyWait";
		break;
	case CX2Unit::UC_EVE_ARCHITECTURE:
		m_wstrLobbyWaitMotionName = L"AT_LobbyWait";
		break;
	case CX2Unit::UC_EVE_ELECTRA:
		m_wstrLobbyWaitMotionName = L"PR_LobbyWait";
		break;
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		m_wstrLobbyWaitMotionName = L"NS_LobbyWait";
		break;
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		m_wstrLobbyWaitMotionName = L"EP_LobbyWait";
		break;
	case CX2Unit::UC_EVE_BATTLE_SERAPH:
		m_wstrLobbyWaitMotionName = L"BS_LobbyWait";
		break;

	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
		m_wstrLobbyWaitMotionName = L"CFG_LobbyWait";
		break;
	case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
		m_wstrLobbyWaitMotionName = L"CSG_LobbyWait";
		break;
	case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
		m_wstrLobbyWaitMotionName = L"CHG_LobbyWait";
		break;
	case CX2Unit::UC_CHUNG_IRON_PALADIN:
		m_wstrLobbyWaitMotionName = L"CIP_LobbyWait";
		break;
	case CX2Unit::UC_CHUNG_DEADLY_CHASER:
		m_wstrLobbyWaitMotionName = L"CDC_LobbyWait";
		break;
	case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
		m_wstrLobbyWaitMotionName = L"CTT_LobbyWait";
		break;

	case CX2Unit::UC_ARA_LITTLE_HSIEN:
		m_wstrLobbyWaitMotionName = L"ALH_LobbyWait";
		break;
	case CX2Unit::UC_ARA_SAKRA_DEVANAM:
		m_wstrLobbyWaitMotionName = L"ASD_LobbyWait";
		break;

	case CX2Unit::UC_ELESIS_SABER_KNIGHT:
		m_wstrLobbyWaitMotionName = L"SSK_LobbyWait";
		break;
	case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
		m_wstrLobbyWaitMotionName = L"SPK_LobbyWait";
		break;
	case CX2Unit::UC_ELESIS_GRAND_MASTER:
		m_wstrLobbyWaitMotionName = L"SGM_LobbyWait";
		break;
	case CX2Unit::UC_ELESIS_BLAZING_HEART:
		m_wstrLobbyWaitMotionName = L"SBH_LobbyWait";
		break;


	case CX2Unit::UC_ARA_LITTLE_DEVIL:
		m_wstrLobbyWaitMotionName = L"ALD_lobbyWait";
		break;
	case CX2Unit::UC_ARA_YAMA_RAJA:
		m_wstrLobbyWaitMotionName = L"AYR_LobbyWait";
		break;

	case CX2Unit::UC_ADD_PSYCHIC_TRACER:
		m_wstrLobbyWaitMotionName = L"APT_LobbyWait";
		break;

	case CX2Unit::UC_ADD_LUNATIC_PSYKER:
		m_wstrLobbyWaitMotionName = L"ALP_LobbyWait";
		break;
	default:
		break;
	}
}

/** @function 	: RidingOn
	@brief 		:
*/
void CX2SquareUnit::ProcessRidingOn( IN bool bVal_, IN const KRidingPetInfo& _Info/* = KRidingPetInfo()*/ )
{
	if ( NULL != g_pData && NULL != CX2RidingPetManager::GetInstance() )
	{
		if ( bVal_ )
		{
			//m_ptrRidingPet = CX2RidingPetManager::GetInstance()->CreateRidingPet( GetUID(), _Info,D3DXVECTOR3( 0, 0, 0) );
			CX2RidingPetManager::GetInstance()->CreateRidingPet( GetUID(), _Info,D3DXVECTOR3( 0, 0, 0) );
		}
		else
		{
			SetRidingOn( bVal_ );

			CX2RidingPetManager::GetInstance()->RemoveRidingPet( GetUID() );
		}

		if ( IsMyUnit() && NULL != g_pData->GetUIManager() )
		{
			g_pData->GetUIManager()->SetShowSkillSlot( !bVal_ );
			g_pData->GetUIManager()->SetShowRidingPetSkillSlot( bVal_ );
		}
	}
}

// X2UnitVIewerUI �� KTDGManager �� ���, ������ �� ���� ��ǰ�鵵 ���� ���, �����ϱ� ���� ����
/*virtual*/
void    CX2SquareUnit::NotifyDGManagerChanged( CKTDGManager& refInManager_ )
{
    if ( m_pUnitViewer != NULL )
        m_pUnitViewer->NotifyDGManagerChanged( refInManager_ );
}//CX2SquareUnit::NotifyDGManagerChanged()
//}} robobeg : 2008-10-17


HRESULT	CX2SquareUnit::OnFrameMove( double fTime, float fElapsedTime )
{
	typedef std::map< float, bool > EventTimeStampMap;


	if ( m_StateID.m_iRidingWait == m_SyncData.m_StateID )
    {
        m_ElapsedTimeRidingWaitHabit.OnFrameMove( fElapsedTime );
    }

//#ifdef TITLE_SYSTEM
    UpdateEquippedEmblem();
    CKTDGParticleSystem::CParticleEventSequence* pSeq_Emblem	= g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );
//#else
//    CKTDGParticleSystem::CParticleEventSequence* pSeq_Emblem	= m_pParticleSystem->GetInstanceSequence( m_hSeqEmblem );
//#endif TITLE_SYSTEM

//#ifdef TITLE_SYSTEM
    if( pSeq_Emblem != NULL )
    {
		pSeq_Emblem->SetShowObject( true );

        if(GetPersonalShopState() == PSS_SHOP)
            pSeq_Emblem->SetShowObject( false );
        else
            pSeq_Emblem->SetShowObject( true );

		if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
			pSeq_Emblem->SetShowObject( false );
    }
//#endif



//{{ robobeg : 2008-10-18

	if( m_bInit == false )
		return S_OK;

	if( m_bFirstGetPacket == false && m_bMyUnit == false )
		return S_OK;

	if ( !GetShowObject() )
		return S_OK;

//}} robobeg : 2008-10-18

	if(m_bPlanRender == true && m_pXSkinAnim != NULL)
	{
		m_pXSkinAnim->SetShowObject(true);

		if(m_pXSkinAnim->GetState() != CKTDGXSkinAnim::XAS_PLAYING )
		{
			m_pXSkinAnim->ChangeAnim( L"Wait", false );
			m_pXSkinAnim->SetPlaySpeed(1.f);
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_LOOP );
		}

		m_pXSkinAnim->GetMatrix().Move( GetPos() );

		D3DXVECTOR3 vRot = GetDirDegree();
		if(GetIsRight() == true)
		{
			vRot.y -= 45.f;
		}
		else
		{
			vRot.y += (180.f + 45.f);
		}
		m_pXSkinAnim->GetMatrix().RotateDegree( vRot );

		m_pXSkinAnim->OnFrameMove(fTime, fElapsedTime);
	}
	else
	{
        if ( m_pXSkinAnim != NULL )
        {
		    m_pXSkinAnim->SetShowObject(false);
		    m_pXSkinAnim->Stop();
            m_pXSkinAnim->UpdateBeforeAnimationTime();
        }
	}



	m_fTime			= (float)fTime;
	m_fElapsedTime	= fElapsedTime;

	m_fPacketTime	-= m_fElapsedTime;
	if( m_fPacketTime < 0.0f )
	{
		m_fPacketTime = 1.0f;
		m_SendPacketCount = 0;
	}


	StateFrameMove();





	if( m_bMyUnit == true )
	{
		AddHistoryUnitState();

		if(m_bTFieldUnit == false)
			StateEventProcess();
	}

	if( m_bMyUnit == false && m_bTFieldUnit == true )
	{
		SetSyncEventProcess();
		++m_iOUFrameCount;
	}

	if( m_bTFieldUnit == true )
		StateEventProcess();

	CKTDGParticleSystem::CParticleEventSequence* pSeq_PartyMy	= m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerPartyMy );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_Party		= m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerParty );
	CKTDGParticleSystem::CParticleEventSequence* pSeq_My		= m_pParticleSystem->GetInstanceSequence( m_hHeadMarkerMy );

	// ���� ������ �� ��Ƽ�� ����̸�
	if( g_pData->GetPartyManager()->GetMyPartyData()->GetPartyMemberData( GetUID() ) != NULL )
	{
		if( NULL != pSeq_PartyMy &&
			NULL != pSeq_Party &&
			NULL != pSeq_My )
		{
			// �ڽ��̸�
			if( m_bMyUnit == true )
			{
				pSeq_PartyMy->SetShowObject( true );

				pSeq_Party->SetShowObject( false );
				pSeq_My->SetShowObject( false );
			}
			else
			{
				pSeq_Party->SetShowObject( true );

				pSeq_PartyMy->SetShowObject( false );
				pSeq_My->SetShowObject( false );
			}
		}
	}
	else
	{
		if( NULL != pSeq_PartyMy &&
			NULL != pSeq_Party &&
			NULL != pSeq_My )
		{
			if( m_bMyUnit == true )
			{
				pSeq_My->SetShowObject( true );

				pSeq_Party->SetShowObject( false );
				pSeq_PartyMy->SetShowObject( false );
			}
			else
			{
				pSeq_My->SetShowObject( false );
				pSeq_Party->SetShowObject( false );
				pSeq_PartyMy->SetShowObject( false );
			}
		}
	}

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
	{
		if( pSeq_PartyMy != NULL )
			pSeq_PartyMy->SetShowObject( false );
		if( pSeq_Party != NULL )
			pSeq_Party->SetShowObject( false );
		if( pSeq_My != NULL )
			pSeq_My->SetShowObject( false );
	}


	CKTDGXSkinAnim* pCKTDGXSkinAnim = m_pUnitViewer->GetXSkinAnim();
	if( pCKTDGXSkinAnim != NULL && pCKTDGXSkinAnim->GetState() == CKTDGXSkinAnim::XAS_PLAYING && m_bPlanRender == false)
	{
//#ifdef TITLE_SYSTEM
		D3DXVECTOR3 vTeamMarkerPos = GetPos();
        D3DXVECTOR3 vHeadPos;
        GetFramePos( &vHeadPos, pCKTDGXSkinAnim->GetCloneFrame( L"Bip01_Head" ) );
        vTeamMarkerPos.y = vHeadPos.y;
        vTeamMarkerPos.y += 70.f;

        //vTeamMarkerPos =+ D3DXVECTOR3( 0.f, 70.f, 0.f );
        D3DXVECTOR3 vSize = D3DXVECTOR3(0.6f, 0.6f, 0.6f);

        if ( m_iTitleId != 0 && (pSeq_Emblem != NULL && pSeq_Emblem->GetShowObject() == true) )
        {
            if ( CKTDGParticleSystem::CParticle* pPart_Emblem_200 = pSeq_Emblem->ValidateParticleHandle( m_hPart_Emblem_200 ) )
            {
                //vTeamMarkerPos.y += 120.0f;
                pPart_Emblem_200->SetPos( vTeamMarkerPos + D3DXVECTOR3( 0, 20, 0 ) );
                vTeamMarkerPos.y += 60.0f;
                vSize = D3DXVECTOR3(0.36f, 0.36f, 0.36f);
            }
        }

        if( CKTDGParticleSystem::CParticle* pHeadMarkerParticleMy = m_pParticleSystem->ValidateParticleHandle( m_hHeadMarkerParticleMy ) )
        {
            pHeadMarkerParticleMy->SetPos( vTeamMarkerPos );
            pHeadMarkerParticleMy->SetSize( vSize );
        }
        if( CKTDGParticleSystem::CParticle* pHeadMarkerParticleParty = m_pParticleSystem->ValidateParticleHandle( m_hHeadMarkerParticleParty ) )
        {
            pHeadMarkerParticleParty->SetPos( vTeamMarkerPos );
            pHeadMarkerParticleParty->SetSize( vSize );
        }
        if( CKTDGParticleSystem::CParticle* pHeadMarkerParticlePartyMy = m_pParticleSystem->ValidateParticleHandle( m_hHeadMarkerParticlePartyMy ) )
        {
            pHeadMarkerParticlePartyMy->SetPos( vTeamMarkerPos );
            pHeadMarkerParticlePartyMy->SetSize( vSize );
        }
//#else
//        //D3DXVECTOR3 vTeamMarkerPos = pCKTDGXSkinAnim->GetCloneFramePosition( L"Bip01_Head" );
//        D3DXVECTOR3 vTeamMarkerPos = GetPos() + D3DXVECTOR3( 0.f, 150.f, 0.f );
//        vTeamMarkerPos.y += 70.0f;
//        if( m_pHeadMarkerParticleMy != NULL )
//            m_pHeadMarkerParticleMy->SetPos( vTeamMarkerPos );
//        if( m_pHeadMarkerParticleParty != NULL )
//            m_pHeadMarkerParticleParty->SetPos( vTeamMarkerPos );
//        if( m_pHeadMarkerParticlePartyMy != NULL )
//            m_pHeadMarkerParticlePartyMy->SetPos( vTeamMarkerPos );
//
//        if( NULL != m_pPart_Emblem_200 )
//            m_pPart_Emblem_200->m_vPos = vTeamMarkerPos + D3DXVECTOR3( 0, -20, 0 );
//
//        if( m_pHeadMarkerParticleMy != NULL )
//            m_pHeadMarkerParticleMy->SetPos( vTeamMarkerPos );
//        if( m_pHeadMarkerParticleParty != NULL )
//            m_pHeadMarkerParticleParty->SetPos( vTeamMarkerPos );
//        if( m_pHeadMarkerParticlePartyMy != NULL )
//            m_pHeadMarkerParticlePartyMy->SetPos( vTeamMarkerPos );
//#endif
	}
	else if( m_pXSkinAnim != NULL && m_pXSkinAnim->GetState() == CKTDGXSkinAnim::XAS_PLAYING && m_bPlanRender == true)
	{
		//D3DXVECTOR3 vTeamMarkerPos = pCKTDGXSkinAnim->GetCloneFramePosition( L"Bip01_Head" );
		D3DXVECTOR3 vTeamMarkerPos = GetPos() + D3DXVECTOR3( 0.f, 150.f, 0.f );
		vTeamMarkerPos.y += 70.0f;

        if( CKTDGParticleSystem::CParticle* pHeadMarkerParticleMy = m_pParticleSystem->ValidateParticleHandle( m_hHeadMarkerParticleMy ) )
            pHeadMarkerParticleMy->SetPos( vTeamMarkerPos );
        if( CKTDGParticleSystem::CParticle* pHeadMarkerParticleParty = m_pParticleSystem->ValidateParticleHandle( m_hHeadMarkerParticleParty ) )
            pHeadMarkerParticleParty->SetPos( vTeamMarkerPos );
        if( CKTDGParticleSystem::CParticle* pHeadMarkerParticlePartyMy = m_pParticleSystem->ValidateParticleHandle( m_hHeadMarkerParticlePartyMy ) )
            pHeadMarkerParticlePartyMy->SetPos( vTeamMarkerPos );

//#ifdef TITLE_SYSTEM
		if(pSeq_Emblem != NULL)
			pSeq_Emblem->SetShowObject(false);
//#endif
	}
	else
	{
		if( pSeq_My != NULL )
			pSeq_My->SetShowObject( false );

		if( pSeq_PartyMy != NULL )
			pSeq_PartyMy->SetShowObject( false );

		if( pSeq_Party != NULL )
			pSeq_Party->SetShowObject( false );

		if( pSeq_Emblem != NULL )
			pSeq_Emblem->SetShowObject( false );

//#ifdef TITLE_SYSTEM
        if(pSeq_Emblem != NULL)
            pSeq_Emblem->SetShowObject(false);
//#endif
	}

//#ifndef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	if(m_bTFieldUnit == false && g_pSquareGame != NULL && m_pUnit->GetUnitData().m_Port != 0)
//	{
//		CKTDNUDP::Peer* pPeer = g_pData->GetGameUDP()->GetPeer( m_pUnit->GetUnitData().m_UnitUID );
//		if( pPeer == NULL )
//		{
//			g_pData->GetGameUDP()->AddPeer( m_pUnit->GetUnitData().m_UnitUID, m_pUnit->GetUnitData().m_IP.c_str(), m_pUnit->GetUnitData().m_Port );
//			g_pData->GetGameUDP()->ConnectTestToPeer();
//		}
//	}
//#endif  SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK


//{{ robobeg : 2008-10-18
	CKTDGParticleSystem::CParticleEventSequence* pSeqShop = m_pParticleSystem->GetInstanceSequence( m_hShop );
	if( pSeqShop != NULL )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 100.0f;
		pos.z += 50.0f;
		pSeqShop->SetPosition( pos );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeqPremiumShop = m_pParticleSystem->GetInstanceSequence( m_hPremiumShop );
	if( pSeqPremiumShop != NULL )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 125.0f;
		pos.z += 45.0f;
		pSeqPremiumShop->SetPosition( pos );
	}

	if( NULL != pSeqShop && NULL != pSeqPremiumShop )
	{
		if( GetPersonalShopState() == PSS_SHOP )
		{
			m_pUnitViewer->SetShowAttribEnchant( false );

			switch( m_eShopType )
			{
			default:
			case SEnum::AST_NORMAL:
				{
					pSeqShop->SetShowObject( true );
					pSeqPremiumShop->SetShowObject( false );
				} break;

			case SEnum::AST_PREMIUM:
				{
					pSeqShop->SetShowObject( false );
					pSeqPremiumShop->SetShowObject( true );
				} break;
			}
		}
		else
		{
			m_pUnitViewer->SetShowAttribEnchant( true );
			pSeqShop->SetShowObject( false );
			pSeqPremiumShop->SetShowObject( false );

		}
	}

    // m_pUnitViewer �� ���������� ���� �� ��ǰ�鵵 ���� ���������� �ʵ���
    // m_pUnitViewer �� ShowObject ���¸� �����Ѵ�.
    // ( m_pUnitViewer�� ��ǰ���� ManagerObject���� �̿� )
    // by robobeg - 2008-10-18

    if ( m_pUnitViewer != NULL )
    {
        if ( GetPersonalShopState() == PSS_SHOP || m_bPlanRender )
            m_pUnitViewer->SetShowObject( false );
        else
            m_pUnitViewer->SetShowObject( true );
    }//if


//}} robobeg : 2008-10-18

#ifdef HANABI_VILLAGE
#if !X2_DEV_TOOLS
	// Ư�� �ð����� 7/1, 7/4 12:00 ~ 20:00 ������
	bool bDay1 = (CTime( 2013, 7, 1, 12, 0, 0 ).GetTime() <= g_pData->GetServerCurrentTime()
		&& g_pData->GetServerCurrentTime() <= CTime( 2013, 7, 1, 20, 0, 0 ).GetTime());
	bool bDay2 = (CTime( 2013, 7, 4, 12, 0, 0 ).GetTime() <= g_pData->GetServerCurrentTime()
		&& g_pData->GetServerCurrentTime() <= CTime( 2013, 7, 4, 20, 0, 0 ).GetTime());
#else
	bool bDay1 = (CTime( 2013, 6, 12, 12, 0, 0 ).GetTime() <= g_pData->GetServerCurrentTime()
		&& g_pData->GetServerCurrentTime() <= CTime( 2013, 6, 12, 20, 0, 0 ).GetTime());
	bool bDay2 = (CTime( 2013, 6, 13, 12, 0, 0 ).GetTime() <= g_pData->GetServerCurrentTime()
		&& g_pData->GetServerCurrentTime() <= CTime( 2013, 6, 13, 20, 0, 0 ).GetTime());
#endif // _SERVICE_
	if( bDay1 || bDay2)
	{
		if( g_pTFieldGame != NULL )
		{
			if( m_bMyUnit == true && g_pTFieldGame->GetMyUnit()->GetUID() == m_UnitUID )
			{
				m_fBlueShowTimer += fElapsedTime;
				m_fRedShowTimer += fElapsedTime;

				if( m_fBlueShowTimer >= 15.f )
				{

					D3DXVECTOR3 MyUnitPos	= g_pTFieldGame->GetMyUnit()->GetPos();

					MyUnitPos.y += 500.f;
					MyUnitPos.x -= 300.f;

					g_pData->GetUIEffectSet()->PlayEffectSetEvent( L"EffectSet_Flame_Blue_Village",  g_pTFieldGame->GetMyUnitPtr(), MyUnitPos );

					m_fBlueShowTimer = 0.f;
				}
				if( m_fRedShowTimer >= 10.f )
				{
					D3DXVECTOR3 MyUnitPos	= g_pTFieldGame->GetMyUnit()->GetPos();

					MyUnitPos.y += 300.f;
					MyUnitPos.x += 400.f;

					g_pData->GetUIEffectSet()->PlayEffectSetEvent( L"EffectSet_Flame_Red_Village",  g_pTFieldGame->GetMyUnitPtr(), MyUnitPos );

					m_fRedShowTimer = 0.f;
				}
			}
		}
	}
#endif // HANABI_VILLAGE

	for( const CX2GameUnit::SoundPlayData& soundData : m_vecSoundPlayData )
	{
		float fTime = soundData.m_SoundPlayTime;
		int iRate	= soundData.m_SoundPlayRate;
        if( m_pXSkinAnim->EventTimerOneshot( fTime ) == true )
		{
			if( rand()%100 < iRate )
			{
				if( soundData.m_bOnlyIfMyUnit == false || m_bMyUnit == true )
					g_pKTDXApp->GetDeviceManager()->PlaySound( soundData.m_SoundPlayName.c_str(), false, false );
			}
		}
	}
	if( m_fCanNotInputTime > 0.f )
		m_fCanNotInputTime -= fElapsedTime;

	return S_OK;
}

//{{ robobeg : 2008-10-18

/*virtual*/ RENDER_HINT CX2SquareUnit::OnFrameRender_Prepare()
{
	if ( m_bInit == false || (m_bFirstGetPacket == false && m_bMyUnit == false) )
		return RENDER_HINT_NORENDER;

	if( GetPersonalShopState() == PSS_SHOP )
        return RENDER_HINT_NORENDER;

    if ( m_bPlanRender )
    {
        if ( m_pMesh != NULL && m_pXSkinAnim != NULL )
		{
            m_pXSkinAnim->SetInstantShow( false );
            if ( !m_pXSkinAnim->GetShowObject() )
                return RENDER_HINT_NORENDER;

			CKTDGXRenderer::RenderParam* pRenderParam = m_pXSkinAnim->GetRenderParam();
            ASSERT( pRenderParam != NULL );

			pRenderParam->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
			if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_MEDIUM )
			{
				pRenderParam->renderType = CKTDGXRenderer::RT_CARTOON;
			}
			else if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_LOW )
			{
				pRenderParam->renderType = CKTDGXRenderer::RT_REAL_COLOR;
			}

			if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_HIGH )
			{

				pRenderParam->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
				pRenderParam->outLineColor		= 0xffffffff;
				pRenderParam->fOutLineWide		= CARTOON_OUTLINE_WIDTH;
				pRenderParam->bAlphaBlend		= false;
			}
			else
			{
				pRenderParam->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
				pRenderParam->fOutLineWide		= CARTOON_OUTLINE_WIDTH;
				pRenderParam->color				= 0xffffffff;
				pRenderParam->bAlphaBlend		= false;
			}

		    if( m_bTFieldUnit == false )
			{
				if( NULL != g_pSquareGame && NULL != g_pSquareGame->GetWorld() )
					pRenderParam->lightPos	= g_pSquareGame->GetWorld()->GetLightPos();
				else if( NULL != g_pWeddingGame && NULL != g_pWeddingGame->GetWorld() )
					pRenderParam->lightPos	= g_pWeddingGame->GetWorld()->GetLightPos();
			}
		    else if( m_bTFieldUnit == true &&
				NULL != g_pTFieldGame &&
				g_pTFieldGame->GetWorld() != NULL )
			    pRenderParam->lightPos	= g_pTFieldGame->GetWorld()->GetLightPos();

			RENDER_HINT renderHint = m_pXSkinAnim->OnFrameRender_Prepare();

            if ( renderHint.m_eHint != RENDER_HINT::NORENDER )
                m_pXSkinAnim->SetInstantShow( true );

            return renderHint;
		}
    }
    else
    {
        if ( m_pUnitViewer != NULL )
        {
            m_pUnitViewer->SetInstantShow( false );
            if ( !m_pUnitViewer->GetShowObject() )
                return RENDER_HINT_NORENDER;
            RENDER_HINT renderHint = m_pUnitViewer->OnFrameRender_Prepare();
            if ( renderHint.m_eHint != RENDER_HINT::NORENDER )
                m_pUnitViewer->SetInstantShow( true );
            return renderHint;
        }//if
    }//if.. else..

    return RENDER_HINT_NORENDER;
}//CX2SquareUnit::OnFrameRender_Prepare()


/*virtual*/ void CX2SquareUnit::OnFrameRender_Draw()
{
	if( m_bPlanRender == true )
	{
        if ( m_pMesh != NULL && m_pXSkinAnim != NULL )
        {
            if ( m_pXSkinAnim->GetShowObject() && m_pXSkinAnim->IsInstantShow() )
			    m_pXSkinAnim->OnFrameRender_Draw();
        }//if

		CX2RidingPetPtr pRidingPetPtr = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( m_UnitUID );
		if ( NULL != pRidingPetPtr )
			pRidingPetPtr->SetShowObject( false, true );
    }
    else
    {
	    if( m_pUnitViewer != NULL && m_pUnitViewer->GetShowObject() && m_pUnitViewer->IsInstantShow() )
	    {
			if(m_bTFieldUnit == false)
			{
				if ( NULL != g_pSquareGame && NULL != g_pSquareGame->GetWorld())
					m_pUnitViewer->SetLightPos( g_pSquareGame->GetWorld()->GetLightPos() );
				else if( NULL != g_pWeddingGame && NULL != g_pWeddingGame->GetWorld() )
					m_pUnitViewer->SetLightPos( g_pWeddingGame->GetWorld()->GetLightPos() );
			}
			else
			{
				if ( g_pTFieldGame != NULL )
					m_pUnitViewer->SetLightPos( g_pTFieldGame->GetWorld()->GetLightPos() );
			}

		    m_pUnitViewer->OnFrameRender_Draw();
			CX2RidingPetPtr pRidingPetPtr = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( m_UnitUID );
			if ( true == GetRidingOn() && NULL != pRidingPetPtr )
				pRidingPetPtr->SetShowObject( true, true );
	    }

    }//if.. else..

}

void CX2SquareUnit::RenderRank(int iFinalLeft, int iFinalTop)
{
	if(m_pTextureRank == NULL)
		return;

	if ( NULL == g_pMain->GetPVPEmblem() )
		return;

	wstring wstrRankKey = g_pMain->GetPVPEmblem()->GetPVPEmblemData( m_pUnit->GetPvpRank() )->m_TextureKey;

	MakeUpperCase( wstrRankKey );
	const CKTDXDeviceTexture::TEXTURE_UV*	pTexUV = m_pTextureRank->GetTexUV( wstrRankKey );
	if(pTexUV == NULL)
		return;

	float fFinalLeft, fFinalTop, fScaleX, fScaleY, fOffsetX, fOffsetY;

	fOffsetX	= 22.f / g_pKTDXApp->GetResolutionScaleX();
	fOffsetY	= 2.f / g_pKTDXApp->GetResolutionScaleY();
	fScaleX		= 20.f / g_pKTDXApp->GetResolutionScaleX();
	fScaleY		= 20.f; // / g_pKTDXApp->GetResolutionScaleY();
	fFinalLeft	= (iFinalLeft) / g_pKTDXApp->GetResolutionScaleX();
	fFinalTop	= (iFinalTop) / g_pKTDXApp->GetResolutionScaleY();

	fFinalLeft -= fOffsetX;
	fFinalTop -= fOffsetY;

	m_pTextureRank->Draw( fFinalLeft, fFinalTop, fScaleX, fScaleY,
		(int)pTexUV->leftTop.x, (int)pTexUV->leftTop.y, (int)pTexUV->rightBottom.x - (int)pTexUV->leftTop.x, (int)pTexUV->rightBottom.y - (int)pTexUV->leftTop.y );
	//m_pTextureRank->Draw( iFinalLeft, iFinalTop, 20.f, 20.f, pTexUV );
}

//{{ oasis907 : ����� [2010.5.17] //
void CX2SquareUnit::RenderServer(int iFinalLeft, int iFinalTop)
{
	if(m_pTextureServer == NULL)
		return;

	const CKTDXDeviceTexture::TEXTURE_UV*	pTexUV = NULL;

#ifdef EXTEND_SERVER_GROUP_MASK

	(int) g_pMain->ExtractServerGroupID(GetUID());
#else
	SERVER_GROUP_ID eServerGroupID	= SGI_INVALID;

	eServerGroupID = (SERVER_GROUP_ID) g_pMain->ExtractServerGroupID(GetUID());
#endif // EXTEND_SERVER_GROUP_MASK

	{
		pTexUV = NULL;
	}

	if(pTexUV == NULL)
		return;

	float fFinalLeft, fFinalTop, fScaleX, fScaleY, fOffsetX, fOffsetY;

	fOffsetX	= 2.f / g_pKTDXApp->GetResolutionScaleX();
	fOffsetY	= -3.f / g_pKTDXApp->GetResolutionScaleY();
	fScaleX		= 21.f ;// g_pKTDXApp->GetResolutionScaleX();
	fScaleY		= 23.f ;// g_pKTDXApp->GetResolutionScaleY();
	fFinalLeft	= (iFinalLeft) / g_pKTDXApp->GetResolutionScaleX();
	fFinalTop	= (iFinalTop) / g_pKTDXApp->GetResolutionScaleY();

	fFinalLeft += fOffsetX;
	fFinalTop += fOffsetY;

	m_pTextureServer->Draw( fFinalLeft, fFinalTop, fScaleX, fScaleY,
		(int)pTexUV->leftTop.x, (int)pTexUV->leftTop.y, (int)pTexUV->rightBottom.x - (int)pTexUV->leftTop.x, (int)pTexUV->rightBottom.y - (int)pTexUV->leftTop.y );
}
//}} oasis907 : ����� [2010.5.17] //

void CX2SquareUnit::RenderName()
{
	KTDXPROFILE();

	if( m_bFirstGetPacket == false && m_bMyUnit == false )
		return;

	if( g_pKTDXApp->GetDGManager()->GetDialogManager()->GetHideDialog() == true )
		return;

	bool bLevel, bPvpRank, bGuild, bNothing;
	bLevel = bPvpRank = bGuild = bNothing = true;

	bLevel		= g_pMain->GetGameOption().GetOptionList().m_bLevel;
	bPvpRank	= g_pMain->GetGameOption().GetOptionList().m_bPvpRank;
	bGuild		= g_pMain->GetGameOption().GetOptionList().m_bGuild;
	bNothing	= g_pMain->GetGameOption().GetOptionList().m_bNothing;

	if(m_bMyUnit == true && bNothing == true)
	{
		bLevel = false;
		bPvpRank = false;
		bGuild = false;
	}

	wstring wstrNickName= m_pUnit->GetNickName();
	WCHAR wcLv[10];
	StringCchPrintf(wcLv, 10, L"Lv.%d ", static_cast<int>( m_pUnit->GetUnitData().m_Level ));
	//wsprintf(wcLv, L"Lv.%d ", m_pUnit->GetUnitData().m_Level);

	wstring wstrName = L"";

	if( bLevel == true )
		wstrName = wcLv + wstrNickName;
	else
		wstrName = wstrNickName;

	if ( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_DEV && g_pData->GetRenderEtc() == true)
	{
		WCHAR wszText[256] = L"";
		swprintf_s( wszText, L"(%5.2f, %5.2f, %5.2f) Rating: %d", GetPos().x, GetPos().y, GetPos().z, static_cast<int>( GetUnit()->GetUnitData().m_iRating ) );
		wstrName += wszText;
	}

	if( m_bMyUnit == true )
	{
		if( m_pUnit->AccessUnitData().IsInvisible() == true )
			wstrName += L" [Invisible]";

		if(m_bTFieldUnit == false)
		{
			if( NULL != g_pSquareGame )
				g_pSquareGame->GetFontForUnitName()->OutProjectionText( m_SyncData.m_Pos, wstrName.c_str(), 0xffffaa77,
					CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
			else if( NULL != g_pWeddingGame )
			{
				CKTDGFontManager::CUKFont* pFont = g_pWeddingGame->GetFontForUnitName();
				if( NULL != pFont )
				{
					pFont->OutProjectionText( m_SyncData.m_Pos, wstrName.c_str(), 0xffffaa77,
											CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
				}
			}
		}
		else
			g_pTFieldGame->GetFontForUnitName()->OutProjectionText( m_SyncData.m_Pos, wstrName.c_str(), 0xffffaa77,
			CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
	}
	else
	{
		if(m_bTFieldUnit == false)
		{
			if( NULL != g_pSquareGame )
				g_pSquareGame->GetFontForUnitName()->OutProjectionText( m_SyncData.m_Pos, wstrName.c_str(), 0xff54ddff,
					CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
			else if( NULL != g_pWeddingGame )
				g_pWeddingGame->GetFontForUnitName()->OutProjectionText( m_SyncData.m_Pos, wstrName.c_str(), 0xff54ddff,
					CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
		}
		else
			g_pTFieldGame->GetFontForUnitName()->OutProjectionText( m_SyncData.m_Pos, wstrName.c_str(), 0xff54ddff,
			CKTDGFontManager::FS_SHELL, 0xff0000ff, NULL, DT_CENTER  );
	}

	D3DXVECTOR2 finalPos = g_pKTDXApp->GetProj3DPos( m_SyncData.m_Pos );
	int iFinalLeft, iFinalTop;

	iFinalTop = (int)finalPos.y;;

	if(m_bTFieldUnit == false)
	{
		if( NULL != g_pSquareGame )
			iFinalLeft = g_pSquareGame->GetFontForUnitName()->GetWidth(wstrName.c_str());
		else if( NULL != g_pWeddingGame )
			iFinalLeft = g_pWeddingGame->GetFontForUnitName()->GetWidth(wstrName.c_str());
	}
	else
		iFinalLeft = g_pTFieldGame->GetFontForUnitName()->GetWidth(wstrName.c_str());

	iFinalLeft = (int)finalPos.x - (iFinalLeft / 2);

	//D3DXVECTOR2 vFinalRankPos = g_pKTDXApp->ConvertByResolution( iFinalLeft, iFinalTop );

	// �������� ��ũ������ �ֱ��������� �Ⱥ��̵��� �Ѵ�.
	if( bPvpRank == true )
		RenderRank(iFinalLeft, iFinalTop);


	if (g_pInstanceData->GetIsDungeonLounge() == true)
	{
		int iWidthUnitName = 0;

		iWidthUnitName = g_pTFieldGame->GetFontForUnitName()->GetWidth(wstrName.c_str());

		iFinalLeft = (int)finalPos.x + (iWidthUnitName / 2);

		iFinalTop = (int)finalPos.y;

		RenderServer(iFinalLeft, iFinalTop);
	}

//{{ ����� : [2009/9/25] //	��� �̸� ǥ��
	if( m_pUnit->GetUnitData().m_wstrGuildName != L"" )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y -= 22.0f;

		if( m_bTFieldUnit == true)
		{
			g_pTFieldGame->GetFontForUnitName()->OutProjectionText( vPos, (g_pData->GetGuildManager()->ConvertDisplayGuildName(m_pUnit->GetUnitData().m_wstrGuildName) ).c_str(), 0xff96ff00,
				CKTDGFontManager::FS_SHELL, 0xff062507, NULL, DT_CENTER  );
		}
		else
		{
			if( NULL != g_pSquareGame )
				g_pSquareGame->GetFontForUnitName()->OutProjectionText( vPos, (g_pData->GetGuildManager()->ConvertDisplayGuildName(m_pUnit->GetUnitData().m_wstrGuildName) ).c_str(), 0xff96ff00,
					CKTDGFontManager::FS_SHELL, 0xff062507, NULL, DT_CENTER  );
			else if( NULL != g_pWeddingGame )
				g_pWeddingGame->GetFontForUnitName()->OutProjectionText( vPos, (g_pData->GetGuildManager()->ConvertDisplayGuildName(m_pUnit->GetUnitData().m_wstrGuildName) ).c_str(), 0xff96ff00,
					CKTDGFontManager::FS_SHELL, 0xff062507, NULL, DT_CENTER  );
		}
	}
//}} ����� : [2009/9/25] //	��� �̸� ǥ��

	IF_EVENT_ENABLED( CEI_RELATIONSHIP_CODE_EVENT )
	{
		if( m_pUnit->GetUnitData().m_bCouple == true && m_pUnit->GetUnitData().m_wstrRelationTargetUserNickname != L"" )
		{
			D3DXVECTOR3 vPos = GetPos();

			if( m_pUnit->GetUnitData().m_wstrGuildName != L"" )
			{
				vPos.y -= 70.0f;
			}
			else
			{
				vPos.y -= 35.0f;
			}

			if( m_bTFieldUnit == true)
			{
				//WCHAR adBuf[200] = {0,};
				//StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"%s �� %s", m_pUnit->GetNickName(), m_pUnit->GetUnitData().m_wstrRelationTargetUserNickname, (int)MAGIC_OXYGEN_GAGE );
				//g_pTFieldGame->GetFontForUnitName()->OutProjectionText( vPos, adBuf, 0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
				wstringstream adBuf;
				adBuf << m_pUnit->GetNickName();
				adBuf << L" �� ";
				adBuf << m_pUnit->GetUnitData().m_wstrRelationTargetUserNickname;
				g_pTFieldGame->GetFontForUnitName()->OutProjectionText( vPos, adBuf.str().c_str(), 0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
			}
			else
			{
				if( NULL != g_pSquareGame )
				{
					//WCHAR adBuf[200] = {0,};
					//StringCchPrintfW( adBuf, ARRAY_SIZE( adBuf ), L"%s �� %s", m_pUnit->GetNickName(), m_pUnit->GetUnitData().m_wstrRelationTargetUserNickname, (int)MAGIC_OXYGEN_GAGE );
					//g_pSquareGame->GetFontForUnitName()->OutProjectionText( vPos, adBuf, 0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
					wstringstream adBuf;
					adBuf << m_pUnit->GetNickName();
					adBuf << L" �� ";
					adBuf << m_pUnit->GetUnitData().m_wstrRelationTargetUserNickname;
					g_pSquareGame->GetFontForUnitName()->OutProjectionText( vPos, adBuf.str().c_str(), 0xffffaa77, CKTDGFontManager::FS_SHELL, 0xffff0000, NULL, DT_CENTER  );
				}
			}
		}
	}


}


void CX2SquareUnit::StateChange( char stateID )
{
	map<char,string>::iterator iter;
	iter = m_StateNameMap.find( stateID );
	if( iter != m_StateNameMap.end() )
	{
		string stateName = iter->second;
		StateChange( stateName.c_str() );
	}
}

void CX2SquareUnit::StateChange( const char* pStateName )
{
	StateEnd();

	m_SyncData.m_StateID = m_StateIDMap[(pStateName) ? pStateName : ""];
	m_SyncData.m_StateChangeNum++;
	m_StateData.m_StateName = pStateName;

	if ( true == GetRidingOn() && NULL != CX2RidingPetManager::GetInstance() )
	{
		CX2RidingPetPtr pRidingPetPtr = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( m_UnitUID );

		if ( NULL != pRidingPetPtr )
		{
			int iState = m_SyncData.m_StateID;
			int iRidingState = 0;

			if ( m_StateID.m_iRidingOn == iState || m_StateID.m_iRidingOff == iState )
				iRidingState = pRidingPetPtr->m_StartState;
			else if ( m_StateID.m_iRidingWait == iState )
				iRidingState = pRidingPetPtr->m_WaitState;
			else if ( m_StateID.m_iRidingWaitHabit == iState )
				iRidingState = pRidingPetPtr->m_WaitHabitState;
			else if ( m_StateID.m_iRidingWalk == iState )
				iRidingState = pRidingPetPtr->m_WalkState;
			else if ( m_StateID.m_iRidingJumpUp == iState )
				iRidingState = pRidingPetPtr->m_JumpUpState;
			else if ( m_StateID.m_iRidingJumpDown == iState )
				iRidingState = pRidingPetPtr->m_JumpDownState;
			else if ( m_StateID.m_iRidingDash == iState )
				iRidingState = pRidingPetPtr->m_DashState;
			else if ( m_StateID.m_iRidingDashJumpUp == iState )
				iRidingState = pRidingPetPtr->m_DashJumpUpState;
			else if ( m_StateID.m_iRidingDashJumpDown == iState )
				iRidingState = pRidingPetPtr->m_DashJumpUpState;

			pRidingPetPtr->StateChange( iRidingState );
		}
	}

	StateStart();
}

void CX2SquareUnit::StateChange_LUA( const char* pStateName )
{
	//wstring stateName;
	//ConvertUtf8ToWCHAR( stateName, pStateName );
	StateChange( pStateName );
}

u_char CX2SquareUnit::GetStateID( const char* pStateName )
{
	return m_StateIDMap[pStateName];
}

void CX2SquareUnit::StateStart()
{
	if( m_LuaManager.BeginTable( m_StateData.m_StateName.c_str() ) == true )
	{
        if ( m_pXSkinAnim != NULL )
            m_pXSkinAnim->ResetOneshotPerformed();

		LUA_GET_VALUE( m_LuaManager, "LAND_CONNECT",		m_StateData.m_bLandConnect,		true );
		LUA_GET_VALUE_UTF8( m_LuaManager, "FUNC_STATE_START",	m_StateData.m_FuncStateStart,	"" );
		LUA_GET_VALUE_UTF8( m_LuaManager, "FUNC_FRAME_MOVE",		m_StateData.m_FuncFrameMove,	"" );
		LUA_GET_VALUE_UTF8( m_LuaManager, "FUNC_EVENT_PROCESS",	m_StateData.m_FuncEventProcess, "" );
		LUA_GET_VALUE_UTF8( m_LuaManager, "FUNC_STATE_END",		m_StateData.m_FuncStateEnd,		"" );

		if( false == m_bForceMoveSpeed )
		{
			m_LuaManager.GetValue( "SPEED_X", m_PhysicParam.nowSpeed.x );
			m_LuaManager.GetValue( "SPEED_Y", m_PhysicParam.nowSpeed.y );
		}


#if X2_DEV_TOOLS

		if ( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
		{
			if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetPureKeyState(DIK_LSHIFT) == TRUE )
			{
				m_PhysicParam.nowSpeed.x *= 6.f;
			}
		}

#endif // VILLAGE_FLASH_MAN_DEV_ONLY



		wstring animName;
		if ( m_StateData.m_StateName == "WAIT" )
			animName = m_wstrLobbyWaitMotionName;
		else
			LUA_GET_VALUE( m_LuaManager, "ANIM_NAME", animName, L"" );

		ASSERT( false == animName.empty() );

		CX2RidingPetPtr pRidingPetPtr = CX2RidingPetPtr();
		if ( true == GetRidingOn() && NULL != CX2RidingPetManager::GetInstance() )
		{
			animName = m_SyncData.m_wstrRidingMotionName;
			pRidingPetPtr = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( m_UnitUID );
		}

		bool bTransition;
		LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );
		CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType;
		LUA_GET_VALUE_ENUM( m_LuaManager, "PLAY_TYPE", playType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );
		float fPlaySpeed;
		LUA_GET_VALUE( m_LuaManager, "PLAY_SPEED", fPlaySpeed, 1.0f );

		if(m_StateData.m_StateName.compare("EMOTION_STATE") == 0)
		{
			if(m_pUnit != NULL)
			{
				if( CX2Unit::IsAvatarEmotion( m_ePlayedEmotion ) )
					m_pUnitViewer->PlayAnim( m_pUnit->GetEmotionAniNameById(m_ePlayedEmotion).c_str(), CKTDGXSkinAnim::XAP_ONE_WAIT, false );
				else
					m_pUnitViewer->PlayAnim( m_pUnit->GetEmotionAniNameById(m_ePlayedEmotion).c_str(), CKTDGXSkinAnim::XAP_ONE_WAIT, true );
			}
			m_pUnitViewer->GetXSkinAnim()->SetPlaySpeed( 1.f );
		}
		else if(m_StateData.m_StateName.compare("EMOTION_SITREADY") == 0)
		{
			m_ePlayedEmotion = CX2Unit::ET_SITREADY;
			m_pUnitViewer->PlayAnim( animName.c_str(), playType, bTransition );
			m_pUnitViewer->GetXSkinAnim()->SetPlaySpeed( fPlaySpeed );

			if ( true == GetRidingOn() && NULL != pRidingPetPtr )
				pRidingPetPtr->StateChange( pRidingPetPtr->m_SitReadyState );
		}
		else if(m_StateData.m_StateName.compare("EMOTION_SITWAIT") == 0)
		{
			m_ePlayedEmotion = CX2Unit::ET_SITWAIT;
			m_pUnitViewer->PlayAnim( animName.c_str(), playType, bTransition );
			m_pUnitViewer->GetXSkinAnim()->SetPlaySpeed( fPlaySpeed );

			if ( true == GetRidingOn() && NULL != pRidingPetPtr )
				pRidingPetPtr->StateChange( pRidingPetPtr->m_SitWaitState );
		}
		else if(m_StateData.m_StateName.compare("EMOTION_STANDUP") == 0)
		{
			m_ePlayedEmotion = CX2Unit::ET_STANDUP;
			m_pUnitViewer->PlayAnim( animName.c_str(), playType, bTransition );
			m_pUnitViewer->GetXSkinAnim()->SetPlaySpeed( fPlaySpeed );

			if ( true == GetRidingOn() && NULL != pRidingPetPtr )
				pRidingPetPtr->StateChange( pRidingPetPtr->m_StandUpState );
		}
		else
		{
			m_ePlayedEmotion = CX2Unit::ET_NONE;
			m_pUnitViewer->PlayAnim( animName.c_str(), playType, bTransition );
			m_pUnitViewer->GetXSkinAnim()->SetPlaySpeed( fPlaySpeed );
		}



		m_vecSoundPlayData.resize(0);
		int iSoundDataIndex = 0;
		while( true == m_LuaManager.BeginTable( "SOUND_PLAY", iSoundDataIndex ) )
		{
			CX2GameUnit::SoundPlayData sndPlayData;

			LUA_GET_VALUE( m_LuaManager, 1, sndPlayData.m_SoundPlayTime,	-1.0f	);
			LUA_GET_VALUE( m_LuaManager, 2, sndPlayData.m_SoundPlayName,	L""		);
			LUA_GET_VALUE( m_LuaManager, 3, sndPlayData.m_SoundPlayRate,	100		);
			LUA_GET_VALUE( m_LuaManager, 4, sndPlayData.m_bOnlyIfMyUnit,	false	);

			m_vecSoundPlayData.push_back( sndPlayData );

			iSoundDataIndex++;
			m_LuaManager.EndTable(); // SOUND_PLAY
		}



		m_LuaManager.EndTable();
	}

	if( false == m_StateData.m_FuncStateStart.empty() )
	{
		//string func;
		//ConvertWCHARToChar( func, m_StateData.m_FuncStateStart.c_str() );
		if(m_bTFieldUnit == false)
		{
			if( NULL != g_pSquareGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncStateStart.c_str(), g_pKTDXApp, g_pMain, g_pSquareGame, this );
			else if( NULL != g_pWeddingGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncStateStart.c_str(), g_pKTDXApp, g_pMain, g_pWeddingGame, this );
		}
		else
			lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncStateStart.c_str(), g_pKTDXApp, g_pMain, g_pTFieldGame, this );
	}

//	SendPacketP2P();
}

void CX2SquareUnit::StateFrameMove()
{
	if( m_LuaManager.BeginTable( m_StateData.m_StateName.c_str() ) == true )
	{

		if( false == m_bForceMoveSpeed )
		{
			m_LuaManager.GetValue( "PASSIVE_SPEED_X", m_PhysicParam.nowSpeed.x );
			m_LuaManager.GetValue( "PASSIVE_SPEED_Y", m_PhysicParam.nowSpeed.y );
		}


#if X2_DEV_TOOLS

		if ( g_pData->GetMyUser()->GetAuthLevel() >= CX2User::XUAL_OPERATOR )
		{
			if( g_pKTDXApp->GetDIManager()->Getkeyboard()->GetPureKeyState(DIK_LSHIFT) == TRUE )
			{
				m_PhysicParam.nowSpeed.x *= 6.f;
			}
		}

#endif // VILLAGE_FLASH_MAN_DEV_ONLY



		bool bRightSync = false;
		m_LuaManager.GetValue( "RIGHT_SYNC", bRightSync );
		if( bRightSync == true )
		{
			if( m_SyncBefore.m_bIsRight != m_SyncData.m_bIsRight
				|| m_InputData.pureLeft != m_BeforeInputData.pureLeft
				|| m_InputData.pureRight != m_BeforeInputData.pureRight )
			{
//				SendPacketRightP2P();
			}
		}
		m_LuaManager.EndTable();
	}

	m_SyncBefore = m_SyncData;
	if( false == m_StateData.m_FuncFrameMove.empty() )
	{
		//string func;
		//ConvertWCHARToChar( func, m_StateData.m_FuncFrameMove.c_str() );
		if(m_bTFieldUnit == false)
		{
			if( NULL != g_pSquareGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncFrameMove.c_str(), g_pKTDXApp, g_pMain, g_pSquareGame, this );
			else if( NULL != g_pWeddingGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncFrameMove.c_str(), g_pKTDXApp, g_pMain, g_pWeddingGame, this );
		}
		else
			lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncFrameMove.c_str(), g_pKTDXApp, g_pMain, g_pTFieldGame, this );
	}

	PhysicProcess();

	if ( true == m_SyncData.m_bRidingOn )
	{
		GetMatrix().Move( m_SyncData.m_vSaddlePos );
		GetMatrix().Rotate( D3DXToRadian(m_SyncData.m_vDirDegree.x), D3DXToRadian(m_SyncData.m_vDirDegree.y), D3DXToRadian(m_SyncData.m_vDirDegree.z) );
	}
	else
	{
		GetMatrix().Move( m_SyncData.m_Pos );
		GetMatrix().Rotate( D3DXToRadian(m_DirDegree.x), D3DXToRadian(m_DirDegree.y), D3DXToRadian(m_DirDegree.z) );
	}
	GetMatrix().Scale( m_Scale );

	if( m_pUnitViewer != NULL )
	{
		if ( true == m_SyncData.m_bRidingOn )
		{
			m_pUnitViewer->GetMatrix().Move( m_SyncData.m_vSaddlePos );
			m_pUnitViewer->GetMatrix().Rotate( D3DXToRadian(m_SyncData.m_vDirDegree.x), D3DXToRadian(m_SyncData.m_vDirDegree.y), D3DXToRadian(m_SyncData.m_vDirDegree.z) );
		}
		else
		{
			m_pUnitViewer->GetMatrix().Move( m_SyncData.m_Pos );
			m_pUnitViewer->GetMatrix().Rotate( D3DXToRadian(m_DirDegree.x), D3DXToRadian(m_DirDegree.y), D3DXToRadian(m_DirDegree.z) );
		}
		m_pUnitViewer->GetMatrix().Scale( m_Scale * m_pUnitViewer->GetScale() );
		if( m_bPlanRender == false && GetPersonalShopState() != PSS_SHOP )
		{
			if ( g_pKTDXApp->GetDGManager()->IsInFrustum( this ) == true )
			{
				m_pUnitViewer->OnFrameMove( m_fTime, m_fElapsedTime );
			}
		}
	}
}

void CX2SquareUnit::StateEventProcess()
{
	if( false == m_StateData.m_FuncEventProcess.empty() )
	{
		//string func;
		//ConvertWCHARToChar( func, m_StateData.m_FuncEventProcess.c_str() );

		if(m_bTFieldUnit == false)
		{
			if( NULL != g_pSquareGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncEventProcess.c_str(), g_pKTDXApp, g_pMain, g_pSquareGame, this );
			else if( NULL != g_pWeddingGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncEventProcess.c_str(), g_pKTDXApp, g_pMain, g_pWeddingGame, this );
		}
		else
			lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncEventProcess.c_str(), g_pKTDXApp, g_pMain, g_pTFieldGame, this );
	}
}

void CX2SquareUnit::StateEnd()
{
	if ( false == m_bIsPlayAvatarEmotionSoundWithoutEmotion )
	{
		if( m_pAvatarEmotionSound != NULL )
		{
			m_pAvatarEmotionSound->Stop();
			SAFE_CLOSE( m_pAvatarEmotionSound );
		}
	}

	if( false == m_StateData.m_FuncStateEnd.empty() )
	{
		//string func;
		//ConvertWCHARToChar( func, m_StateData.m_FuncStateEnd.c_str() );

		if(m_bTFieldUnit == false)
		{
			if( NULL != g_pSquareGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncStateEnd.c_str(), g_pKTDXApp, g_pMain, g_pSquareGame, this );
			else if( NULL != g_pWeddingGame )
				lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncStateEnd.c_str(), g_pKTDXApp, g_pMain, g_pWeddingGame, this );
		}
		else
			lua_tinker::call<void>( g_pKTDXApp->GetLuaBinder()->GetLuaState(), m_StateData.m_FuncStateEnd.c_str(), g_pKTDXApp, g_pMain, g_pTFieldGame, this );
	}
}

void CX2SquareUnit::PhysicProcess()
{
	//���� �Ķ���� ���//////////////////////////////////////////////////////////
	D3DXVECTOR3 position;
	D3DXVECTOR3 positionOrg;
	D3DXVECTOR3 angleDegree;

	//���� ���� �޾ƿ���
	positionOrg = position = m_SyncData.m_Pos;
	angleDegree	= m_DirDegree;

	CX2World *pWorld = NULL;

	if(m_bTFieldUnit == false)
	{
		if( NULL != g_pSquareGame )
			pWorld = g_pSquareGame->GetWorld();
		else if( NULL != g_pWeddingGame )
			pWorld = g_pWeddingGame->GetWorld();
	}
	else
		pWorld = g_pTFieldGame->GetWorld();

	if( NULL == pWorld || NULL == pWorld->GetLineMap() )
		return;

	const CKTDGLineMap::LineData* pLineData;
	pLineData = pWorld->GetLineMap()->GetLineData( m_SyncData.m_LastTouchLineIndex );

	if( pLineData == NULL )
	{
		pLineData = pWorld->GetLineMap()->GetLineData( 0 );
		positionOrg = position = m_SyncData.m_Pos = pLineData->startPos;
	}
	else if( pLineData->bEnable == false )
	{
		int lineIndex = pWorld->GetLineMap()->GetNearestLine( positionOrg );
		if( lineIndex != -1 )
		{
			pLineData = pWorld->GetLineMap()->GetLineData( lineIndex );
			positionOrg = position = m_SyncData.m_Pos = pLineData->startPos;
			m_SyncData.m_LastTouchLineIndex = lineIndex;
			angleDegree	= m_DirDegree = pLineData->dirDegree;
		}
		else
		{
			pLineData = pWorld->GetLineMap()->GetLineData( 0 );
			positionOrg = position = m_SyncData.m_Pos = pLineData->startPos;
		}
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	//X����
	bool positive;
	if( m_PhysicParam.nowSpeed.x >= 0.0f )
		positive = true;
	else
		positive = false;
	if( positive == true )
	{
		m_PhysicParam.nowSpeed.x -= m_PhysicParam.fReloadAccel * m_fElapsedTime;
		if( m_PhysicParam.nowSpeed.x <= 0.0f )
			m_PhysicParam.nowSpeed.x = 0.0f;
	}
	else
	{
		m_PhysicParam.nowSpeed.x += m_PhysicParam.fReloadAccel * m_fElapsedTime;
		if( m_PhysicParam.nowSpeed.x > 0.0f )
			m_PhysicParam.nowSpeed.x = 0.0f;
	}

	//Y����
	if( m_StateData.m_bLandConnect == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed )
			m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;

		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fDownAccel * m_fElapsedTime;
		if( m_PhysicParam.nowSpeed.y <= m_PhysicParam.fMaxGSpeed )
			m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
	}
	else
		m_PhysicParam.nowSpeed.y = 0.0f;

	//X�̵�
	m_DirVector = pLineData->dirVector;
	if( m_bFootOnLine == false )
	{
		D3DXVECTOR3 dirVec = m_DirVector;
		dirVec.y = 0.0f;

		if( m_SyncData.m_bIsRight == true )
			position += dirVec * m_PhysicParam.nowSpeed.x * m_fElapsedTime;
		else
			position -= dirVec * m_PhysicParam.nowSpeed.x * m_fElapsedTime;
	}
	else
	{
		if( m_SyncData.m_bIsRight == true )
			position += m_DirVector * m_PhysicParam.nowSpeed.x * m_fElapsedTime;
		else
			position -= m_DirVector * m_PhysicParam.nowSpeed.x * m_fElapsedTime;
	}






	//��� �̵�
	if( GetPersonalShopState() != PSS_SHOP )
	{
		// wait���¿��� �����̴°� ���� ����
		if( m_SyncData.m_StateID != GetStateID().m_Wait ) //GetStateID(L"WAIT")
		{
			if( m_SyncData.m_bIsRight == true )
			{
				position.x += m_DirVector.x * m_pUnitViewer->GetXSkinAnim()->GetMotionOffset().x;
				position.z += m_DirVector.z * m_pUnitViewer->GetXSkinAnim()->GetMotionOffset().x;
			}
			else
			{
				position.x -= m_DirVector.x * m_pUnitViewer->GetXSkinAnim()->GetMotionOffset().x;
				position.z -= m_DirVector.z * m_pUnitViewer->GetXSkinAnim()->GetMotionOffset().x;
			}
		}
	}

	//Y�̵�
	position.y += m_PhysicParam.nowSpeed.y * m_fElapsedTime;

	// ���� �ε�������
	bool bCanWallJump = false;
	if( true == pWorld->GetLineMap()->CrossedWall( m_SyncData.m_Pos, position, bCanWallJump ) )
	{
		position.x = positionOrg.x;
		position.z = positionOrg.z;
	}

	//���� ���////////////////////////////////////////////////////////////////////
	bool bTouch					= false;
	//m_bFall						= false;
	int lastTouchLineIndex		= m_SyncData.m_LastTouchLineIndex;
	D3DXVECTOR3 landPosition	= m_LandPosition;

	bTouch = pWorld->GetLineMap()->IsOnLine( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex );

	if( bTouch == true ) //�̵� ��ġ�� �� �����
	{
		if( m_PhysicParam.nowSpeed.y > 0.0f )
		{
			//��� ���̸� ���� ��ġ�� ���� ��ũ�Ѵ�.
			position.x = landPosition.x;
			position.z = landPosition.z;
		}
		else
		{
			//�ϰ��̳� �������¸� ��ġ�� ���� ��ũ�Ѵ�.
			position = landPosition;

			//���� ������Ƿ� Y�ӵ��� ���ش�.
			m_PhysicParam.nowSpeed.y = 0.0f;
			m_PhysicParam.fDownAccel = 0.0f;
		}
	}
	else //�̵� ��ġ�� �� ���� �ƴ϶��
	{
		//���� �ݵ�� ���� ��� �־�� �ϴ� ��Ȳ�̶��
		if( m_StateData.m_bLandConnect == true
			&& pWorld->GetLineMap()->IsOnLineConnect( position, &lastTouchLineIndex, LINE_RADIUS, &landPosition ) == true )
		{
			position = landPosition; //���� ��ġ�� ���� ��ũ�Ѵ�.

			//���� ������Ƿ� Y�ӵ��� ���ش�.
			m_PhysicParam.nowSpeed.y = 0.0f;
			m_PhysicParam.fDownAccel = 0.0f;
		}
		else //������ �ϰ��� ���� ���� �ʴ°��� ������ ��Ȳ�̶��
		{
			const CKTDGLineMap::LineData* pDwnLineData = pWorld->GetLineMap()->GetLineData( lastTouchLineIndex );
			if( pDwnLineData == NULL )
			{
				lastTouchLineIndex = 0;
				pDwnLineData = pWorld->GetLineMap()->GetLineData( lastTouchLineIndex );
			}

			bool bMeOnStart = false;
			bool bMeOnEnd = false;
			if( GetDistance(landPosition,pDwnLineData->startPos) < LINE_RADIUS )
				bMeOnStart = true;
			else if( GetDistance(landPosition,pDwnLineData->endPos) < LINE_RADIUS )
				bMeOnEnd = true;

			if( bMeOnStart == true && pDwnLineData->beforeLine == -2 )
			{
				if( position.y < landPosition.y )
					position.y = landPosition.y;

				//��� ����Ÿ ������ -2 ������ �հ� ������ ���󋚹��� ����, ���� �׽�Ʈ ������
				//position.x		= pDwnLineData->startPos.x;
				//position.z		= pDwnLineData->startPos.z;
				//landPosition.x	= pDwnLineData->startPos.x;
				//landPosition.z	= pDwnLineData->startPos.z;
				landPosition = pWorld->GetLineMap()->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else if( bMeOnEnd == true && pDwnLineData->nextLine == -2 )
			{
				if( position.y < landPosition.y )
					position.y = landPosition.y;

				//position.x		= pDwnLineData->endPos.x;
				//position.z		= pDwnLineData->endPos.z;
				//landPosition.x	= pDwnLineData->endPos.x;
				//landPosition.z	= pDwnLineData->endPos.z;
				landPosition = pWorld->GetLineMap()->GetLandPosition( position, LINE_RADIUS, &lastTouchLineIndex );
				position.x = landPosition.x;
				position.z = landPosition.z;
			}
			else
			{
				bool bCanDown = pWorld->GetLineMap()->CanDown( position, LINE_RADIUS, &landPosition, &lastTouchLineIndex, false, false, false );

				if( bCanDown == true ) //�Ʒ��ʿ� ������ �־ ������ ���� �ִٸ�
				{
					//���� ��ġ�� �Ʒ��� ���� ��ũ�Ѵ�.
					position.x = landPosition.x;
					position.z = landPosition.z;

					//if( m_PhysicParam.nowSpeed.y <= 0.0f )
					//{
					//	m_bFall = true;
					//}

					if( position.y < landPosition.y )
						position.y = landPosition.y;
				}
				else //���̻� �Ʒ��ʿ� ������ ���� ������ ��Ȳ�̸�
				{
					//position = positionOrg;	//�̹� �������� �̵��� ĵ���Ѵ�.
					position.x = positionOrg.x;
					position.z = positionOrg.z;
					if( position.y < landPosition.y )
						position.y = landPosition.y;
				}
			}
		}
	}


	const CKTDGLineMap::LineData* pSyncLineData = pWorld->GetLineMap()->GetLineData( m_SyncData.m_LastTouchLineIndex );
	if( pSyncLineData == NULL )
	{
		m_SyncData.m_LastTouchLineIndex = 0;
		pSyncLineData = pWorld->GetLineMap()->GetLineData( m_SyncData.m_LastTouchLineIndex );
	}

	if( lastTouchLineIndex < 0 || pSyncLineData->bEnable == false )
	{
		lastTouchLineIndex = pWorld->GetLineMap()->GetNearestLine( position, &landPosition );
	}

	m_SyncData.m_LastTouchLineIndex	= lastTouchLineIndex;
	m_LandPosition					= landPosition;

	//���������� ������ �´�
	pLineData = pWorld->GetLineMap()->GetLineData( m_SyncData.m_LastTouchLineIndex );

	if( pLineData == NULL )
	{
		pLineData = pWorld->GetLineMap()->GetLineData( 0 );
		positionOrg = position = m_SyncData.m_Pos = pLineData->startPos;
	}
	else if( pLineData->bEnable == false )
	{
		int lineIndex = pWorld->GetLineMap()->GetNearestLine( positionOrg );
		if( lineIndex != -1 )
		{
			pLineData = pWorld->GetLineMap()->GetLineData( lineIndex );
			positionOrg = position = m_SyncData.m_Pos = pLineData->startPos;
			m_SyncData.m_LastTouchLineIndex = lineIndex;
			angleDegree	= m_DirDegree = pLineData->dirDegree;
		}
		else
		{
			pLineData = pWorld->GetLineMap()->GetLineData( 0 );
			positionOrg = position = m_SyncData.m_Pos = pLineData->startPos;
		}
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	//��ũ�� ���� ������ �������ٸ� �ޱ� ������ �����.
	D3DXVECTOR3 dirDegree;
	INIT_VECTOR3( dirDegree, 0.0f, 0.0f, 0.0f );
	if( m_SyncData.m_bIsRight == true )
	{
		dirDegree.x = pLineData->dirDegree.x;
		dirDegree.y = pLineData->dirDegree.y;
		//angle.z = D3DXToRadian(lineData.dirDegree.z);
	}
	else
	{
		dirDegree.x = pLineData->dirDegree.x;
		dirDegree.y = pLineData->dirDegree.y + 180.0f;
		//angle.z = D3DXToRadian(-lineData.dirDegree.z);
	}




	if( true == m_StateData.m_bLandConnect )
	{
		//������ ���� ���ϴ� ���θ��� ��� ���� ����
		switch ( pLineData->m_eDirection )
		{
		case CKTDGLineMap::LDO_LEFT:
			{
				m_SyncData.m_bIsRight = false;
			} break;
		case CKTDGLineMap::LDO_RIGHT:
			{
				m_SyncData.m_bIsRight = true;
			} break;
		default:
			break;
		}

		//���θʿ��� ������ �ð����� Ű �Է� ���ϵ��� ����
		if( pLineData->m_fKeyStopTime > 0.f )
		{
			SetCanNotInputTime( pLineData->m_fKeyStopTime );
			m_InputData.Init();
		}

		//���θʿ��� ������ �ӵ��� ���� �����ִ°Ϳ� ���� ����
		m_bForceMoveSpeed = false;
		switch( pLineData->lineType )
		{
		case CKTDGLineMap::LT_JUMP_UP_REL:
			{
				m_PhysicParam.nowSpeed.x = pLineData->m_vJumpSpeed.x;
				m_PhysicParam.nowSpeed.y = pLineData->m_vJumpSpeed.y;
				m_StateData.m_bLandConnect = false;
				m_bForceMoveSpeed = true;
			}break;
		default:
			{
				if( pLineData->m_fSpeed > 0.f )
				{
					m_PhysicParam.nowSpeed.x = pLineData->m_fSpeed;
					m_bForceMoveSpeed = true;
				}
			}
			break;
		}
	}

	//��ġ ������Ʈ////////////////////////////////////////////////////////////////
	m_SyncData.m_Pos	= position;
	m_DirDegree			= dirDegree;

	if( fabs(m_SyncData.m_Pos.y - m_LandPosition.y) < 1.0f &&
		m_PhysicParam.nowSpeed.y <= 0.0f )
		m_bFootOnLine = true;
	else
		m_bFootOnLine = false;
}

bool CX2SquareUnit::CanDown_LUA()
{
	if(m_bTFieldUnit == false)
	{
		if( NULL != g_pSquareGame)
			return g_pSquareGame->GetWorld()->GetLineMap()->CanDown( m_SyncData.m_Pos, LINE_RADIUS, &m_LandPosition, &m_SyncData.m_LastTouchLineIndex );
		else if( NULL != g_pWeddingGame )
			return g_pWeddingGame->GetWorld()->GetLineMap()->CanDown( m_SyncData.m_Pos, LINE_RADIUS, &m_LandPosition, &m_SyncData.m_LastTouchLineIndex );
	}
	else
		return g_pTFieldGame->GetWorld()->GetLineMap()->CanDown( m_SyncData.m_Pos, LINE_RADIUS, &m_LandPosition, &m_SyncData.m_LastTouchLineIndex );

	return false;
}


void CX2SquareUnit::SendPacket( int sendCount )
{
	if( m_bMyUnit == false || m_SendPacketCount > sendCount )
		return;

	KEGS_SQUARE_UNIT_SYNC_DATA_REQ kEGS_SQUARE_UNIT_SYNC_DATA_REQ;

	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_StateID				= m_SyncData.m_StateID;
	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_StateChangeNum			= m_SyncData.m_StateChangeNum;
	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_vPos.x					= m_SyncData.m_Pos.x;
	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_vPos.y					= m_SyncData.m_Pos.y;
	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_vPos.z					= m_SyncData.m_Pos.z;
	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_LastTouchLineIndex		= m_SyncData.m_LastTouchLineIndex;
	kEGS_SQUARE_UNIT_SYNC_DATA_REQ.m_bIsRight				= m_SyncData.m_bIsRight;

	g_pData->GetServerProtocol()->SendPacket( EGS_SQUARE_UNIT_SYNC_DATA_REQ, kEGS_SQUARE_UNIT_SYNC_DATA_REQ );
	m_SendPacketCount++;
}

void CX2SquareUnit::RecvTRPacket( KEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT& kEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT )
{
	//if(m_bFirstSync == false && m_bMyUnit == false)
	//	Init();

	// �� �κ��� �ڽ��� ������ �ƴ� ��쿡�� ������ ������...���� �׷� �ʿ䰡 ������...?
	m_SyncOUStartPos_Old = m_SyncOUStartPos;
	m_SyncIsRight_Old = m_SyncIsRight;

	m_bFirstSync = true;
	m_bFirstGetPacket = true;

	if( m_bMyUnit == true )
		return;

	m_iNowStateIndex = 0;
	m_iOUFrameCount = 0;
	m_iOUNextFrameCount = 0;
	m_SyncIsRight = kEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT.m_bIsRight;

	m_SyncOUStartPos.x = kEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT.m_vPos.x;
	m_SyncOUStartPos.y = kEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT.m_vPos.y;
	m_SyncOUStartPos.z = kEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT.m_vPos.z;

	//m_vecFieldFrame.clear();
	m_vecFieldFrame = kEGS_FIELD_UNIT_SYNC_DATA_BROAD_NOT.m_vecFieldFrame;

}

void CX2SquareUnit::RecvPacket( KEGS_SQUARE_UNIT_SYNC_DATA_NOT& kEGS_SQUARE_UNIT_SYNC_DATA_NOT )
{
	m_bFirstGetPacket = true;

	if( m_bMyUnit == true )
		return;

	m_SyncData.m_Pos.x					= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_vPos.x;
	m_SyncData.m_Pos.y					= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_vPos.y;
	m_SyncData.m_Pos.z					= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_vPos.z;
	m_SyncData.m_LastTouchLineIndex		= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_LastTouchLineIndex;
	m_SyncData.m_bIsRight				= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_bIsRight;
	if( m_SyncData.m_StateChangeNum	!= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_StateChangeNum || kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_StateID == 1 )
	{
		StateChange( kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_StateID );
		m_SyncData.m_StateChangeNum	= kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_StateChangeNum;
	}

	m_iED = kEGS_SQUARE_UNIT_SYNC_DATA_NOT.m_iED;
}

//void CX2SquareUnit::GetPacketP2P( KXPT_SQUARE_UNIT_SYNC& kXPT_SQUARE_UNIT_SYNC )
//{
//	return;
//
//
//
//
//	if( m_bMyUnit == false )
//		return;
//
//	kXPT_SQUARE_UNIT_SYNC.m_UnitUID					= m_UnitUID;
//	kXPT_SQUARE_UNIT_SYNC.m_StateID					= m_SyncData.m_StateID;
//	kXPT_SQUARE_UNIT_SYNC.m_StateChangeNum			= m_SyncData.m_StateChangeNum;
//	kXPT_SQUARE_UNIT_SYNC.m_PosX					= m_SyncData.m_Pos.x;
//	kXPT_SQUARE_UNIT_SYNC.m_PosY					= m_SyncData.m_Pos.y;
//	kXPT_SQUARE_UNIT_SYNC.m_PosZ					= m_SyncData.m_Pos.z;
//	kXPT_SQUARE_UNIT_SYNC.m_LastTouchLineIndex		= m_SyncData.m_LastTouchLineIndex;
//	kXPT_SQUARE_UNIT_SYNC.m_bIsRight				= m_SyncData.m_bIsRight;
//}
//
//void CX2SquareUnit::SendPacketP2P( UidType unitUID, int sendCount )
//{
//	return;
//
//
//
//	if( m_bMyUnit == false || g_pSquareGame == NULL )
//		return;
//
//	KXPT_SQUARE_UNIT_SYNC kXPT_SQUARE_UNIT_SYNC;
//	GetPacketP2P( kXPT_SQUARE_UNIT_SYNC );
//	KSerBuffer buff;
//	Serialize( &buff, &kXPT_SQUARE_UNIT_SYNC );
//
//	if( g_pSquareGame != NULL )
//	{
//		if( unitUID > 0 )
//		{
//			CX2SquareUnit* pCX2SquareUnit = g_pSquareGame->GetSquareUnitByUID( unitUID );
//			if( pCX2SquareUnit != NULL )
//			{
//				if( g_pSquareGame->GetSendSizePerSec() < 20000 )
//				{
//					g_pData->GetGameUDP()->Send( pCX2SquareUnit->GetUID(), (const char)XPT_SQUARE_UNIT_SYNC, (const char*)buff.GetData(), (int)buff.GetLength() );
//					g_pSquareGame->AddSendSizePerSec( (int)buff.GetLength() );
//				}
//			}
//		}
//		else
//		{
//			int count = 0;
//			for( int i = 0; i < g_pSquareGame->GetSquareUnitNum(); i++ )
//			{
//				CX2SquareUnit* pCX2SquareUnit = g_pSquareGame->GetSquareUnit( i );
//				if( pCX2SquareUnit != NULL )
//				{
//					if( g_pSquareGame->GetSendSizePerSec() < 20000 )
//					{
//						g_pData->GetGameUDP()->Send( pCX2SquareUnit->GetUID(), (const char)XPT_SQUARE_UNIT_SYNC, (const char*)buff.GetData(), (int)buff.GetLength() );
//						g_pSquareGame->AddSendSizePerSec( (int)buff.GetLength() );
//						count++;
//						if( count > sendCount )
//							break;
//					}
//				}
//			}
//		}
//	}
//}
//
//void CX2SquareUnit::RecvPacketP2P( KXPT_SQUARE_UNIT_SYNC& kXPT_SQUARE_UNIT_SYNC )
//{
//	return;
//
//
//
//
//
//	m_bFirstGetPacket = true;
//
//	if( m_bMyUnit == true )
//		return;
//
//	m_SyncData.m_Pos.x					= kXPT_SQUARE_UNIT_SYNC.m_PosX;
//	m_SyncData.m_Pos.y					= kXPT_SQUARE_UNIT_SYNC.m_PosY;
//	m_SyncData.m_Pos.z					= kXPT_SQUARE_UNIT_SYNC.m_PosZ;
//	m_SyncData.m_LastTouchLineIndex		= kXPT_SQUARE_UNIT_SYNC.m_LastTouchLineIndex;
//	m_SyncData.m_bIsRight				= kXPT_SQUARE_UNIT_SYNC.m_bIsRight;
//	if( m_SyncData.m_StateChangeNum	!= kXPT_SQUARE_UNIT_SYNC.m_StateChangeNum )
//	{
//		StateChange( kXPT_SQUARE_UNIT_SYNC.m_StateID );
//		m_SyncData.m_StateChangeNum	= kXPT_SQUARE_UNIT_SYNC.m_StateChangeNum;
//	}
//}


//void CX2SquareUnit::GetPacketRightP2P( KXPT_SQUARE_UNIT_SYNC_RIGHT& kXPT_SQUARE_UNIT_SYNC_RIGHT )
//{
//	return;
//
//
//
//
//
//
//
//	if( m_bMyUnit == false )
//		return;
//
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_UnitUID				= m_UnitUID;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_PosX					= m_SyncData.m_Pos.x;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_PosY					= m_SyncData.m_Pos.y;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_PosZ					= m_SyncData.m_Pos.z;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_LastTouchLineIndex	= m_SyncData.m_LastTouchLineIndex;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_bIsRight				= m_SyncData.m_bIsRight;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_bPureLeft				= m_InputData.pureLeft;
//	kXPT_SQUARE_UNIT_SYNC_RIGHT.m_bPureRight			= m_InputData.pureRight;
//}
//
//void CX2SquareUnit::SendPacketRightP2P()
//{
//	return;
//
//
//
//
//
//
//
//
//
//
//
//	if( m_bMyUnit == false || g_pSquareGame == NULL)
//		return;
//
//	KXPT_SQUARE_UNIT_SYNC_RIGHT kXPT_SQUARE_UNIT_SYNC_RIGHT;
//	GetPacketRightP2P( kXPT_SQUARE_UNIT_SYNC_RIGHT );
//	KSerBuffer buff;
//	Serialize( &buff, &kXPT_SQUARE_UNIT_SYNC_RIGHT );
//
//	if( g_pSquareGame != NULL )
//	{
//		int count = 0;
//		for( int i = 0; i < g_pSquareGame->GetSquareUnitNum(); i++ )
//		{
//			CX2SquareUnit* pCX2SquareUnit = g_pSquareGame->GetSquareUnit( i );
//			if( pCX2SquareUnit != NULL )
//			{
//				if( g_pSquareGame->GetSendSizePerSec() < 20000 )
//				{
//					g_pData->GetGameUDP()->Send( pCX2SquareUnit->GetUID(), (const char)XPT_SQUARE_UNIT_SYNC_RIGHT, (const char*)buff.GetData(), (int)buff.GetLength() );
//					g_pSquareGame->AddSendSizePerSec( (int)buff.GetLength() );
//					count++;
//					if( count > 20 )
//						break;
//				}
//			}
//		}
//	}
//}
//
//void CX2SquareUnit::RecvPacketRightP2P( KXPT_SQUARE_UNIT_SYNC_RIGHT& kXPT_SQUARE_UNIT_SYNC_RIGHT )
//{
//	return;
//
//
//
//
//
//
//
//	m_bFirstGetPacket = true;
//
//	if( m_bMyUnit == true )
//		return;
//
//	m_SyncData.m_Pos.x					= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_PosX;
//	m_SyncData.m_Pos.y					= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_PosY;
//	m_SyncData.m_Pos.z					= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_PosZ;
//	m_SyncData.m_LastTouchLineIndex		= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_LastTouchLineIndex;
//	m_SyncData.m_bIsRight				= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_bIsRight;
//	m_InputData.pureLeft				= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_bPureLeft;
//	m_InputData.pureRight				= kXPT_SQUARE_UNIT_SYNC_RIGHT.m_bPureRight;
//}

void CX2SquareUnit::UpdateEquippedEmblem()
{
	// note!! �������� �䷸�� �ӽ÷� ��ƼŬ�� ǥ��
	if( NULL != m_pUnit )
	{

//#ifdef TITLE_SYSTEM
        int titleId = m_pUnit->GetTitleId();

        if(m_iTitleId == titleId)
            return;
//#endif

//#ifdef TITLE_SYSTEM
		g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqEmblem );
//#else
//		m_pParticleSystem->DestroyInstanceHandle( m_hSeqEmblem );
//#endif




//#ifdef TITLE_SYSTEM
        m_hSeqEmblem			= INVALID_PARTICLE_SEQUENCE_HANDLE;
        m_hPart_Emblem_200		= INVALID_PARTICLE_HANDLE;
        m_iTitleId = titleId;

        if(titleId > 0)
        {
            wstring titleName = g_pData->GetTitleManager()->GetTitleModel(titleId);

            m_hSeqEmblem = g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL,  titleName.c_str(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
			CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem    = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqEmblem );

			if( pSeqEmblem != NULL )
			{
                m_hPart_Emblem_200 =  pSeqEmblem->CreateNewParticleHandle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
                pSeqEmblem->SetShowObject( false );
			}
        }
//#else
//		const int MAGIC_EMBLEM_ITEM_ID				= 129785;
//		const int MAGIC_EMBLEM_ITEM_ID_GOLD_MEDAL	= 130002;
//
//		bool bEquippedEmblem = false;
//
//		const CX2Item::ItemTemplet* pItemTempet = g_pData->GetItemManager()->GetItemTemplet( MAGIC_EMBLEM_ITEM_ID );
//		if( NULL != pItemTempet )
//		{
//			CX2Item* pEmblemItem = m_pUnit->GetInventory().GetEquippingItemByEquipPos( pItemTempet->GetEqipPosition(), false );
//            if( NULL != pEmblemItem && NULL != pEmblemItem->GetItemTemplet()
//                && pEmblemItem->GetItemTemplet()->GetItemID() == pItemTempet->GetItemID() )
//			{
//				bEquippedEmblem = true;
//				m_hSeqEmblem	= m_pParticleSystem->CreateSequenceHandle( NULL,  L"UnitEmblem200", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
//				CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem    = m_pParticleSystem->GetInstanceSequence( m_hSeqEmblem );
//
//				if( pSeqEmblem != NULL )
//				{
//					if( m_pPart_Emblem_200 == NULL )
//					{
//						m_pPart_Emblem_200 = pSeqEmblem->CreateNewParticle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
//					}
//				}
//			}
//		}
//
//		if( false == bEquippedEmblem )
//		{
//			pItemTempet = g_pData->GetItemManager()->GetItemTemplet( MAGIC_EMBLEM_ITEM_ID_GOLD_MEDAL );
//			if( NULL != pItemTempet )
//			{
//				CX2Item* pEmblemItem = m_pUnit->GetInventory().GetEquippingItemByEquipPos( pItemTempet->GetEqipPosition(), false );
//				if( NULL != pEmblemItem && NULL != pEmblemItem->GetItemTemplet() &&
//                    pEmblemItem->GetItemTemplet()->GetItemID() == pItemTempet->GetItemID()
//                    )
//				{
//					bEquippedEmblem = true;
//					m_hSeqEmblem	= m_pParticleSystem->CreateSequenceHandle( NULL,  L"UnitEmblemGoldMedal", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
//					CKTDGParticleSystem::CParticleEventSequence* pSeqEmblem    = m_pParticleSystem->GetInstanceSequence( m_hSeqEmblem );
//
//					if( pSeqEmblem != NULL )
//					{
//						if( m_pPart_Emblem_200 == NULL )
//						{
//							m_pPart_Emblem_200 = pSeqEmblem->CreateNewParticle( D3DXVECTOR3(0.0f,0.0f,0.0f) );
//						}
//					}
//				}
//			}
//		}
//#endif
	}
}

void CX2SquareUnit::AddHistoryUnitState()
{
	bool bDash = false;
	if(m_vecSyncUnitState.size() <= 3)
	{
		// ������¿� ���ؼ� ù�������� �����Ѵ�.
		if( GetNowState() == GetStateID().m_Dash ||
			GetNowState() == GetStateID().m_DashJumpUp ||
			GetNowState() == GetStateID().m_DashJumpDown )
		{
			bDash = true;
		}
	}


	m_iActID = 0;

	if(GetPureLeft_LUA() == true)
	{
		if(bDash == true && GetIsRight() == false)
			m_iActID = m_iActID | SUS_DL;
		else
			m_iActID = m_iActID | SUS_L;
	}
	if(GetPureRight_LUA() == true)
	{
		if(bDash == true && GetIsRight() == true)
			m_iActID = m_iActID | SUS_DR;
		else
			m_iActID = m_iActID | SUS_R;
	}
	if(GetPureDoubleLeft_LUA() == true)
	{
		m_iActID = m_iActID | SUS_DL;
	}
	if(GetPureDoubleRight_LUA() == true)
	{
		m_iActID = m_iActID | SUS_DR;
	}
	if(GetPureUp_LUA() == true)
	{
		m_iActID = m_iActID | SUS_U;
	}
	if(GetPureDown_LUA() == true)
	{
		if(bDash == true)
			m_iActID = m_iActID | SUS_DD;
		else
			m_iActID = m_iActID | SUS_D;
	}
	if(GetPureDoubleUp_LUA() == true)
	{
		m_iActID = m_iActID | SUS_DU;
	}
	if(GetPureDoubleDown_LUA() == true)
	{
		m_iActID = m_iActID | SUS_DD;
	}

	SyncUnitState dataSUS;
	dataSUS.m_eventID = m_iActID;
	dataSUS.m_elapsedFrame = 1;

	m_vecSyncUnitState.push_back(dataSUS);
}

D3DXVECTOR3 CX2SquareUnit::GetHistorySummary(std::vector<SyncUnitState> &vecSyncData)
{
	SyncUnitState dataSUS;
	u_char oldActId = SUS_DUMY;

	for(unsigned i = 0; i < m_vecSyncUnitState.size(); ++i)
	{
		dataSUS = m_vecSyncUnitState[i];	// & ������ �޴� ���� �ξ� ������ ��

		// ���� ������Ʈ�� ���� ������Ʈ�� �ƴϸ�
		if(dataSUS.m_eventID != oldActId)
		{
			vecSyncData.push_back(dataSUS);
			oldActId = dataSUS.m_eventID;

			// �̺�Ʈ �� ����
			if(vecSyncData.size() >= 29)
			{
				// 30�� ����
				dataSUS.m_eventID = SUS_W;
				dataSUS.m_elapsedFrame = 1;
				vecSyncData.push_back(dataSUS);
				break;
			}
		}
		else	// ���� ������Ʈ�� ���� ������Ʈ��
		{
			if(vecSyncData.size() >= 1)
			{
				// ���� ���� ã�Ƽ� �������� �Ѱ� �߰� ��
				vecSyncData[ vecSyncData.size() - 1 ].m_elapsedFrame += 1;
			}
		}
	}

	m_vecSyncUnitState.resize(0);

	return m_LandPosition;
}


void CX2SquareUnit::SetSyncEventId(u_char eventId)
{
	m_InputData.Init();

	if(eventId == SUS_W || eventId == SUS_DUMY)
		return;

	if((eventId & 1) > 0)
	{
		// SUS_L
		m_InputData.pureLeft = true;
	}
	if((eventId & 2) > 0)
	{
		// SUS_R
		m_InputData.pureRight = true;
	}
	if((eventId & 4) > 0)
	{
		// SUS_DL
		m_InputData.pureDoubleLeft = true;
	}
	if((eventId & 8) > 0)
	{
		// SUS_DR
		m_InputData.pureDoubleRight = true;
	}
	if((eventId & 16) > 0)
	{
		// SUS_U
		m_InputData.pureUp = true;
	}
	if((eventId & 32) > 0)
	{
		// SUS_D
		m_InputData.pureDown = true;
	}
	if((eventId & 64) > 0)
	{
		// SUS_DU
		m_InputData.pureDoubleUp = true;
	}
	if((eventId & 128) > 0)
	{
		// SUS_DD
		m_InputData.pureDoubleDown = true;
	}
}

void CX2SquareUnit::SetSyncEventProcess()
{
	int maxFrameCount = m_vecFieldFrame.size();

	if(maxFrameCount <= 0)
	{
		if(m_bFirstSync == true)
		{
			m_SyncData.m_Pos = m_SyncOUStartPos;
			m_bFirstSync = false;
		}

		m_SyncData.m_bIsRight = m_SyncIsRight;
		SetSyncEventId(SUS_W);

		return;
	}

	if(m_iOUFrameCount >= 255)
	{
		m_vecFieldFrame.clear();
		m_SyncData.m_bIsRight = m_SyncIsRight;
		SetSyncEventId(SUS_W);
		return;
	}

	if(m_iOUFrameCount <= 0)
	{
		m_SyncData.m_Pos = m_SyncOUStartPos_Old;
		m_SyncData.m_bIsRight = m_SyncIsRight_Old;
	}



	KFieldFrameInfo frameNode;

	if(m_iOUFrameCount == 0 && m_iOUNextFrameCount == 0)
	{
		m_iNowStateIndex = 0;
		frameNode = m_vecFieldFrame[m_iNowStateIndex];
		m_iOUNextFrameCount += frameNode.m_FrameCount;

		SetSyncEventId(frameNode.m_StateID);

		return;
	}

	if(m_iNowStateIndex+1 >= maxFrameCount)
	{
		//m_vecFieldFrame.clear();
		frameNode = m_vecFieldFrame[m_vecFieldFrame.size()-1];
		//m_SyncData.m_Pos = m_SyncOUStartPos;
		m_SyncData.m_bIsRight = m_SyncIsRight;
		SetSyncEventId(frameNode.m_StateID);

		return;
	}

	if(m_iOUFrameCount >= m_iOUNextFrameCount)
	{
		++m_iNowStateIndex;
		frameNode = m_vecFieldFrame[m_iNowStateIndex];
		m_iOUNextFrameCount += frameNode.m_FrameCount;

		SetSyncEventId(frameNode.m_StateID);
	}
	else
	{
		frameNode = m_vecFieldFrame[m_iNowStateIndex];
		SetSyncEventId(frameNode.m_StateID);
	}
}

void CX2SquareUnit::SetInputData( CX2GUUser::InputData* pInputData )
{
	if( m_fCanNotInputTime > 0.f )
	{
		pInputData->Init();
	}

	if(m_bTFieldUnit == true && g_pTFieldGame != NULL && g_pTFieldGame->GetEnableKey() == false )
	{
		pInputData->Init();
	}

	m_OrgInputData		= *pInputData;
	m_BeforeInputData	= m_InputData;
	m_InputData			= *pInputData;	//FieldFix: �ڱ� �ڽ��� ���� �ڱ� �ڽſ��� �� ���� ������ ���µ�...
}

void CX2SquareUnit::DoJumpUp()
{
	m_pXSkinAnim->ChangeAnim( L"JumpUp", true );
	m_pXSkinAnim->SetPlaySpeed(1.f);
	m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
}

bool CX2SquareUnit::GetNowJump()
{
	if(m_pXSkinAnim != NULL)
	{
		if( m_pXSkinAnim->GetNowAnimName().compare( L"JumpUp" ) == 0 ||
			m_pXSkinAnim->GetNowAnimName().compare( L"JumpDown" ) == 0 ||
			m_pXSkinAnim->GetNowAnimName().compare( L"DashJump" ) == 0 )
		{
			return true;
		}
	}
	return false;
}





CKTDXDeviceSound* CX2SquareUnit::PlaySound_LUA( const char* szSoundFileName, bool b3DSound )
{
	if( NULL == szSoundFileName )
		return NULL;


	wstring wstrSoundFileName = L"";
	ConvertUtf8ToWCHAR( wstrSoundFileName, szSoundFileName );
	if( true == wstrSoundFileName.empty() )
		return NULL;


	// fix!! opensound ���� �ʰ� devicemanager���� ���� ã�Ƽ� �÷����ϵ��� �ٲ��� ������ reference count�� ��� �����Ѵ�. ��. closesound ���ִ� �κ��� ����.
	CKTDXDeviceSound* pSound = g_pKTDXApp->GetDeviceManager()->OpenSound( wstrSoundFileName );
	if( NULL != pSound )
	{
		pSound->Set3DPosition( GetPos() );
		pSound->Play( false, b3DSound );
	}

	return pSound;
}

bool CX2SquareUnit::PlayEmotion(CX2Unit::EMOTION_TYPE eEmotionId)
{

	if ( false == IsPlayAvatarEmotionSoundWithouEmotion( eEmotionId ) )
		return false;


	if ( true == CX2Unit::IsAvatarEmotion( eEmotionId )
		&& false == m_bIsPlayAvatarEmotionSoundWithoutEmotion
		)
	{

		wstring wstrEmotionName = L"";
		CX2Unit::EMOTION_TYPE eEmotion = CX2Unit::ET_NONE;
		bool bMixEmotion = m_pUnit->GetAvatarEmotion(wstrEmotionName, eEmotion);
		if( bMixEmotion == false )
			return false;
		if( eEmotionId != eEmotion )
			return false;

		if( bMixEmotion == true && wstrEmotionName.empty() != true &&
			( (eEmotion == CX2Unit::ET_EMOTION_AVATAR1 && m_bMixedEmotion[0] == false) ||
			  (eEmotion == CX2Unit::ET_EMOTION_AVATAR2 && m_bMixedEmotion[1] == false) ||
			  (eEmotion == CX2Unit::ET_EMOTION_AVATAR3 && m_bMixedEmotion[2] == false) ||
			  (eEmotion == CX2Unit::ET_EMOTION_AVATAR4 && m_bMixedEmotion[3] == false) ||
			  (eEmotion == CX2Unit::ET_EMOTION_AVATAR5 && m_bMixedEmotion[4] == false)
				|| (eEmotion == CX2Unit::ET_EMOTION_AVATAR6 && m_bMixedEmotion[5] == false)
			))
		//if( bMixEmotion == true && m_bMixedEmotion == false && wstrEmotionName.empty() != true )
		{
			if( m_pUnitViewer != NULL && m_pUnitViewer->GetXSkinAnimPtr() != NULL )
			{
				CKTDXDeviceXSkinMesh *pMixMotion = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( wstrEmotionName );
				ASSERT( pMixMotion );
				m_pUnitViewer->GetXSkinAnimPtr()->MixAnim( pMixMotion );
				switch( eEmotion )
				{
				case CX2Unit::ET_EMOTION_AVATAR1:
					m_bMixedEmotion[0] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR2:
					m_bMixedEmotion[1] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR3:
					m_bMixedEmotion[2] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR4:
					m_bMixedEmotion[3] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR5:
					m_bMixedEmotion[4] = true;
					break;
				case CX2Unit::ET_EMOTION_AVATAR6:
					m_bMixedEmotion[5] = true;
					break;
				default:
					break;
				}
				SAFE_CLOSE( pMixMotion );
			}
		}
	}

	if(eEmotionId == CX2Unit::ET_NONE)
		return false;

	if ( true == GetRidingOn() )
		if( ( eEmotionId != CX2Unit::ET_SITREADY && eEmotionId != CX2Unit::ET_STANDUP ) )
			return false;

	if( eEmotionId == CX2Unit::ET_SITREADY &&
		(m_ePlayedEmotion == CX2Unit::ET_SITREADY || m_ePlayedEmotion == CX2Unit::ET_SITWAIT) )
		return false;

	if( eEmotionId == CX2Unit::ET_STANDUP && m_ePlayedEmotion != CX2Unit::ET_SITWAIT )
		return false;


	if(m_pUnit != NULL)
	{
		const char* pszEmotionStateName = m_pUnit->GetEmotionName(eEmotionId);
		if( pszEmotionStateName == NULL || pszEmotionStateName[0] == '\0' )
			return false;

		if(m_bMyUnit == false)
		{
			m_SyncData.m_Pos = m_SyncOUStartPos;
			m_vecFieldFrame.clear();
		}

		m_ePlayedEmotion = eEmotionId;

		StateChange( pszEmotionStateName );

		if( eEmotionId == CX2Unit::ET_EMOTION_AVATAR1 || eEmotionId == CX2Unit::ET_EMOTION_AVATAR3 ||
			eEmotionId == CX2Unit::ET_EMOTION_AVATAR4 || eEmotionId == CX2Unit::ET_EMOTION_AVATAR5
			|| eEmotionId == CX2Unit::ET_EMOTION_AVATAR6
			|| eEmotionId == CX2Unit::ET_EMOTION_AVATAR7
			)
		{
			wstring wstrEmotionSoundName = L"";
			switch( eEmotionId )
			{
			case CX2Unit::ET_EMOTION_AVATAR1:
				wstrEmotionSoundName = L"Emotion_BigBangDance.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR3:
				wstrEmotionSoundName = L"Emotion_APinkDance.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR4:
				wstrEmotionSoundName = L"Emotion_APink_Love.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR5:
				wstrEmotionSoundName = L"DancingQueen_Music.ogg";
				break;
			case CX2Unit::ET_EMOTION_AVATAR6:
				wstrEmotionSoundName = L"Emotion_Bbabbabba.ogg";
				break;

			case CX2Unit::ET_EMOTION_AVATAR7:
				wstrEmotionSoundName = L"Emotion_Crayonpop_NoMotion.ogg";
				break;
			default:
				break;
			}

			if( m_pAvatarEmotionSound != NULL )
			{
				m_pAvatarEmotionSound->Stop();
				SAFE_CLOSE( m_pAvatarEmotionSound );
			}
			const float MAGIC_NUMBER = 500.f;
			const float fMaxDist = g_pKTDXApp->GetDGManager()->GetCamera().GetCameraDistance() + MAGIC_NUMBER;
			m_pAvatarEmotionSound = g_pKTDXApp->GetDeviceManager()->OpenSound( wstrEmotionSoundName, 10, true, fMaxDist );
			if( m_pAvatarEmotionSound != NULL )
			{
				switch( g_pMain->GetNowStateID() )
				{
				case CX2Main::XS_SQUARE_GAME:
				case CX2Main::XS_PVP_ROOM:
					{	// note!! �ŷ����忡���� �ϴ� 2D �����
						m_pAvatarEmotionSound->Play( false, false );
					} break;
				default:
					{
						if( true == g_pKTDXApp->GetDSManager()->GetCapable3DSound() &&
							true == g_pKTDXApp->GetDSManager()->GetEnable3DSound() &&
							true == g_pMain->GetGameOption().GetOptionList().m_bEnable3DSound )
						{
							m_pAvatarEmotionSound->Set3DPosition( GetPos() );
							m_pAvatarEmotionSound->Play( false );
						}
					} break;
				}
			}
		}
		else
		{
			if( NULL != g_pChatBox )
			{
				g_pChatBox->PlayEmotionSound( m_pUnit->GetType(), eEmotionId, GetPos(), true );
			}
		}

		if ( eEmotionId == CX2Unit::ET_EMOTION_AVATAR4 )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Head" );
			g_pData->GetUIMajorParticle()->CreateSequenceHandle( NULL, L"Emotion_APINK_LOVE_P01", vBonePos );
		}

		return true;
	}

	return false;
}

bool CX2SquareUnit::IsAnimationEnd_LUA()
{
	if ( true == GetRidingOn() )
	{
		CX2RidingPetPtr pRidingPetPtr = CX2RidingPetManager::GetInstance()->GetRidingPetPtr( m_UnitUID );

		if ( NULL != pRidingPetPtr )
		{
			if ( NULL != pRidingPetPtr->GetXSkinAnimPtr() )
				return pRidingPetPtr->GetXSkinAnimPtr()->IsAnimationEnd();
		}
	}
	else
	{
		if( GetUnitViewer() != NULL && GetUnitViewer()->GetXSkinAnim() != NULL )
			return GetUnitViewer()->GetXSkinAnim()->IsAnimationEnd();
	}

	return false;
}


/*static*/ const WCHAR* CX2SquareUnit::GetSquareUnitScriptFileName( CX2Unit::UNIT_CLASS /*eUnitClass*/ )
{
	return L"SQUARE_Unit.lua";
}



void CX2SquareUnit::SendPacketP2PForWedding( IN vector<UidType>& vecUnitUIDList_ )
{
	if( false == m_bMyUnit || NULL == g_pWeddingGame )
		return;

	KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL kPacket;
	GetPacketP2PForWedding( kPacket );

//#ifdef SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
	g_pData->GetGameUDP()->BroadCast( vecUnitUIDList_, XPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL, (char*)&kPacket, sizeof(KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL) );
//#else//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
//	KSerBuffer buff;
//	Serialize( &buff, &kPacket );
//
//	g_pData->GetGameUDP()->BroadCast( vecUnitUIDList_, XPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL, (char*)buff.GetData(), buff.GetLength() );
//#endif//SERV_KTDX_OPTIMIZE_UDP_PACKET_PACK
}
void CX2SquareUnit::GetPacketP2PForWedding( OUT KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL& kPacket_ )
{
	kPacket_.m_iUnitUID			= m_UnitUID;
	kPacket_.m_StateID			= m_SyncData.m_StateID;
	kPacket_.m_StateChangeNum	= m_SyncData.m_StateChangeNum;
	kPacket_.m_fPosX			= m_SyncData.m_Pos.x;
	kPacket_.m_fPosY			= m_SyncData.m_Pos.y;
	kPacket_.m_fPosZ			= m_SyncData.m_Pos.z;
	kPacket_.m_bIsRight			= m_SyncData.m_bIsRight ;
	kPacket_.m_LastTouchLineIndex = m_SyncData.m_LastTouchLineIndex;
}
void CX2SquareUnit::RecvPacketP2PForWedding( OUT KXPT_UNIT_USER_SYNC_PACK_FOR_WEDDING_HALL& kPacket_ )
{
	if( true == m_bMyUnit || NULL == g_pWeddingGame )
		return;

	m_SyncData.m_Pos.x					= kPacket_.m_fPosX;
	m_SyncData.m_Pos.y					= kPacket_.m_fPosY;
	m_SyncData.m_Pos.z					= kPacket_.m_fPosZ;
	m_SyncData.m_LastTouchLineIndex		= kPacket_.m_LastTouchLineIndex;
	m_SyncData.m_bIsRight				= kPacket_.m_bIsRight;

	if( m_SyncData.m_StateChangeNum	!= kPacket_.m_StateChangeNum ||
		m_SyncData.m_StateID != kPacket_.m_StateID)
	{
		StateChange( kPacket_.m_StateID );
		m_SyncData.m_StateChangeNum	= kPacket_.m_StateChangeNum;
	}

	if( false == m_bFirstGetPacket )
		m_bFirstGetPacket = true;
}

// �߰�, �������� 0, 0, 0 ��ǥ�� Ŀ�� ��ƼŬ ����Ʈ��
// ���̴� ��� ����

void CX2SquareUnit::SetPlanRender( bool bPlanRender )
{

	if ( m_bPlanRender != bPlanRender )		// PlanRender ���� ������ ���� ��,
	{
		if ( NULL != g_pData->GetRelationshipEffectManager() )
		{
			// PlanRender ���� ��� ����Ʈ�� �����
			if ( true == bPlanRender )
			{
				g_pData->GetRelationshipEffectManager()->DeleteRelationshipAttachEffectHandleByUID( m_UnitUID );
			}
			// �ƴ� ���, ����Ʈ�� �Ҵ�.
			else
			{
				if( NULL != GetUnit() && 0 < GetUnit()->GetLoverUnitUID() )
				{
					CX2RelationshipEffectManager::RelationEffectInfo RelationEffectInfo_;
					RelationEffectInfo_ = ( *(g_pData->GetRelationshipEffectManager()->GetRelationEffectInfoIndex ( 3 ) ) );
					RelationEffectInfo_.PrepareDrawRelationshipEffect( CX2RelationshipEffectManager::RelationEffectInfo::RET_ATTACH_EFFECT, m_UnitUID );
				}
			}
		}
	}

	// �� �κ��� �߰��� �κ�, �Ʒ� �κ��� ����
	m_bPlanRender = bPlanRender;
}


bool CX2SquareUnit::IsPlayAvatarEmotionSoundWithouEmotion ( CX2Unit::EMOTION_TYPE eEmotionId )
{
	m_bIsPlayAvatarEmotionSoundWithoutEmotion = false;
	switch ( eEmotionId )
	{
		case CX2Unit::ET_EMOTION_AVATAR7 :
			for( int i=0; i<std::ssize( ITEM_ID_EMOTION_AVATAR1 ); ++i )
			{
				const CX2Item::ItemTemplet* pItemTempet = g_pData->GetItemManager()->GetItemTemplet( ITEM_ID_EMOTION_AVATAR1[i] );

				if( NULL != pItemTempet )
				{
					CX2Item* pEquipedItem = m_pUnit->GetInventory().GetEquippingItemByEquipPos( CX2Unit::EP_DEFENCE_BODY, true );
					if ( NULL != pEquipedItem && NULL != pEquipedItem->GetItemTemplet() )
					if ( pEquipedItem->GetItemTemplet()->GetItemID () == pItemTempet->GetItemID() )
					{
						m_bIsPlayAvatarEmotionSoundWithoutEmotion = true;
						return true;
					}
				}
			}
			break;
		default :
			return true;
			break;
	}

	return false;
}
