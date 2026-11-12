#include "stdafx.h"
#include "./X2GuEve.h"

// dmlee 2009-05-23, �̺��� ��ȯ ������ stat�� cx2game::createnpc()�Լ� �ȿ��� ���� �̺��� stat�� �������� �ٽ� �������ְ� �Ǿ� ����.


// ��� ����, ���� ��� �ӵ��� ���ϴ� parameter
static const float s_fDashJumpSpeedScale		= 1.85f;
static const float s_fDashJumpSpeedLowerBound	= 1.f;
static const float s_fMaxDashJumpTime			= 0.65f;

static const float s_fEveDefaultInvisibilityTime = 3.f;




// �ʻ�� �� �� cut in image
static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] =
{
	// �̺�, ����, ��Ű
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_1.dds"),// D3DXVECTOR2( 590.f, 590.f ), D3DXVECTOR3( 230.f, 482.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_2.dds"),// D3DXVECTOR2( 778.f, 778.f ), D3DXVECTOR3( 256.f, 577.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_3.dds"),// D3DXVECTOR2( 820.f, 820.f ), D3DXVECTOR3( 135.f, 582.f, 0.0f ) ),
	},

	//{{ kimhc // 2010.2.17 //	�̺� 2�� ���� ��ų ����
	// �׸޽ý�
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_NM_1.dds"),// D3DXVECTOR2( 667.f, 667.f ), D3DXVECTOR3( 326.f, 446.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_NM_1.dds"),// D3DXVECTOR2( 667.f, 667.f ), D3DXVECTOR3( 326.f, 446.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_NM_1.dds"),// D3DXVECTOR2( 667.f, 667.f ), D3DXVECTOR3( 326.f, 446.f, 0.0f ) ),
	},

	// ��������
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_EP_1.dds"),// D3DXVECTOR2( 668.f, 668.f ), D3DXVECTOR3( 328.f, 432.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_EP_1.dds"),// D3DXVECTOR2( 668.f, 668.f ), D3DXVECTOR3( 328.f, 432.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_EP_1.dds"),// D3DXVECTOR2( 668.f, 668.f ), D3DXVECTOR3( 328.f, 432.f, 0.0f ) ),
	},

		//}} kimhc // 2010.2.17 //	�̺� 2�� ���� ��ų ����

		// ��Ʋ ������
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_BS_1.dds"),// D3DXVECTOR2( 668.f, 668.f ), D3DXVECTOR3( 328.f, 432.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_BS_1.dds"),// D3DXVECTOR2( 668.f, 668.f ), D3DXVECTOR3( 328.f, 432.f, 0.0f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Eve_BS_1.dds"),// D3DXVECTOR2( 668.f, 668.f ), D3DXVECTOR3( 328.f, 432.f, 0.0f ) ),
	},

};

CX2GUEve::CX2GUEve( int unitIndex, int teamNum,
	lua_State*   pScriptLuaState,
	int frameBufferNum, CX2Unit* pUnit )
	:CX2GUUser( unitIndex, teamNum,
	pScriptLuaState,
	frameBufferNum, pUnit )
	, m_bIsWinMotion2( false )
	, m_bIsLoseMotion2( false )
	, m_EveElectraSystem( this )
	, m_fEffectStartTime( 0.f )
	, m_fAddSpectrumTime( 0.f )
	, m_fAddEnergeticHeartTime( 0.f )
	, m_bStartLinkOverChargeIllusion( false )
	, m_iFerdinandNPCUID( -1 )
	, m_fFerdinandLifeTime( -1.f )
	, m_eReserveEndElCrystalBuffID( BTI_NONE )
	, m_bIsCheckEndElCrystal ( false )
	, m_fDecraseInvisibilityMP ( -4.f )
{
	m_bReAttackZ1Right		= false;
	m_fReAttackZ1Time		= 0.0f;
	m_fReDashJumpXTime		= 0.0f;

	m_iJumpFlyCount			= 0;
	m_iDashJumpCount		= 0;
	m_iDashJumpUpCount		= 0;

	m_fCurrDashJumpTime		= 0.f;
	m_fInvisibilityTime		= s_fEveDefaultInvisibilityTime;
	m_fSummonedLifeTime		= 0.f;

	//{{ kimhc // 2010.2.10 //	�̺� 2�� ���� �׸޽ý�
	m_bDoneSpearBurst = false;	// ���Ǿ� ����Ʈ�� ���� ����� �ĺ� ��
	//}} kimhc // 2010.2.10 //	�̺� 2�� ���� �׸޽ý�

	for( int i=0; i<MajorMID_END; i++ )
	{
		m_ahMajorMeshInstance[i] = INVALID_MESH_INSTANCE_HANDLE;
	}

	for( int i=0; i<MinorMID_END; i++ )
	{
		m_ahMinorMeshInstance[i] = INVALID_MESH_INSTANCE_HANDLE;
	}

	InitializeEveMajorParticleArray();
	InitializeEveMinorParticleArray();

	m_hOberonGuard = INVALID_EFFECTSET_HANDLE;

	m_iSuccessiveHit_OneZ	= 0;
	m_vSummonedUnitBonePos = D3DXVECTOR3(0.f, 0.f, 0.f);

	for( int i = 0; i < EASI_END; ++i )
	{
		m_fEquippedActiveSkill[i] = -1;
	}
	m_fElectronBallRangeRate		= 1.f;
	m_fMPConsumeRateElectronBall	= 1.f;
	//#ifdef EVE_SECOND_CLASS_CHANGE
	// oasis907 : ����� [2010.1.29] //
	m_fAttackPowerRateElectronBall = 1.f;
	m_fForceDownValueRateElectronBall = 1.f;
	//#endif EVE_SECOND_CLASS_CHANGE

	m_fThrowStartTime		= 0.8f;
	m_vThrowStartPosOffset	= D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity	= D3DXVECTOR3(1400.f, 400.f, 0.f);

	m_fPlaneZAngle			= 0.f;

	m_bBlinkInvisibility	= false;
	m_fBlinkInvisibilityAlpha	= 0.f;

	m_iPhotonBoosterAdditionalDashJumpCount	= 0;
	m_fPhotonBlinkBackSpeedX				= 0.f;
	m_fPhotonBlinkDummyLifeTime				= 0.f;


	m_hEffectSweepParadeTriangle	= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hEffectSweepParadeLight		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hEffectSweepParadeElectric	= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hEffectElectraDashComboZ		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hEffectElectraDashComboZZ		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hEffectElectraLaserZZZfrontZ	= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hEffectElectraLaserXXZ		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_fSweepParadeTriangleLIfeTime	= 0.f;
	m_fInitDamageSweepParade		= 0.f;

	m_hElectraDashComboZMesh		= INVALID_MESH_INSTANCE_HANDLE;
	//m_hElectraDashComboZMeshZZ	= INVALID_MESH_INSTANCE_HANDLE;

	m_bXXZLaserEnd			= false;
	m_bNotEnoughMPZZZfrontZLaser	= false;
	m_bNotEnoughMPXXZLaser			= false;
	m_bNotEnoughMPDashZZZ			= false;
	m_bLockOnNpc			= false;
	m_iLockOnUid			= -1;

	m_hCountDownThousandStar	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_bDelayAttackThousandStar	= false;
	m_fDelayAttackTime			= 0.f;
	m_fThousandStarAttackTime	= 0.f;
	m_fElapsedTimeThousandStartCreated	= 0.f;
	m_fThousandStarPowerRate	= 0.f;

	m_bEntireFireMode			= false;

	m_fDelayAttackTimeEntireFire	= 0.f;

	m_iThousandStarCount			=  0;

	m_pThousandStar				= NULL;

	m_bAfterJumpDash		= false;
	m_bEnableBoostDash		= false;
	m_eBoostDashState		= BDS_NONE;

	m_bHighPoweredElectrics			= false;
	m_fHighPoweredElectricsSpeed	= 0.f;
	m_iAccumulateAccel				= 0;

	m_bDashJumpXXLaserEnd			= false;
	m_bNotEnoughMPDashJumpXZLaser	= false;
	m_bExceptionPlane				= false;
	m_fExceptioncPlaneZAngle		= 0.f;
    m_hEffectElectraLaserDashJumpXZ	= INVALID_DAMAGE_EFFECT_HANDLE;

	m_hBoostDashParticle	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	//m_hBoostDashMesh		= INVALID_MESH_INSTANCE_HANDLE;

	m_bNotEnoughMPDashJumpZX	= false;
	m_iBoostAttackCount			= 0;
	m_bBoostUpAttack			= false;
	m_iAddBoostAttack			= 0;
	m_hEffectEnergeticHeart		= INVALID_EFFECTSET_HANDLE;

	m_bStartSpectroElCrystal = false;		/// �б������� ��ų�� ���� ���� �Ǿ�����, ������ ���� �Ǿ������� ���� ����

	INIT_VECTOR3( m_vSonicGunRotate, 0.f, 0.f, 0.f );
}

CX2GUEve::~CX2GUEve(void)
{
	ClearThousandStar();

	m_sEGQueensThrone.Reset();

	for( int i=0; i<MajorMID_END; i++ )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_ahMajorMeshInstance[i] );
	}

	for( int i=0; i<MinorMID_END; i++ )
	{
		g_pX2Game->GetMinorXMeshPlayer()->DestroyInstanceHandle( m_ahMinorMeshInstance[i] );
	}

	DeleteEveMajorParticle();
	DeleteEveMinorParticle();

	g_pX2Game->GetEffectSet()->StopEffectSet( m_hOberonGuard );

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );

	//{{ JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );	/// ����� ���� ��ȯ ����
	//}} JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_SI_HA_FERDINAND );

	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_FullCharge.ogg" );

	if ( INVALID_EFFECTSET_HANDLE != m_hEffectEnergeticHeart  )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectEnergeticHeart );
	}

	m_EveElectraSystem.m_fFormationSetElapsedTime = 0.f;

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticlePlane );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMark[0] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMark[1] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMark[2] );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMsg[0] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMsg[1] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMsg[2] );

	m_EveElectraSystem.SetSystemOperationTime( 0.f );
	m_EveElectraSystem.SetExceptionSystemTime( 0.f );
	m_EveElectraSystem.SetPlaneFormation( PFT_NONE );
	m_EveElectraSystem.SetEnableSystem( false );

}

void CX2GUEve::ReInit( bool bRandom, int startPosIndex )
{
	CX2GUUser::ReInit( bRandom, startPosIndex );
	m_fReDashJumpXTime		= 0.0f;

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );

	//{{ JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );	/// ����� ���� ��ȯ ����
	//}} JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_SI_HA_FERDINAND );

	ClearThousandStar();
	if ( m_hEffectSweepParadeTriangle != INVALID_DAMAGE_EFFECT_HANDLE )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectSweepParadeTriangle );

	if ( INVALID_EFFECTSET_HANDLE != m_hEffectEnergeticHeart  )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectEnergeticHeart );
	}

	m_EveElectraSystem.m_fFormationSetElapsedTime = 0.f;

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticlePlane );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMark[0] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMark[1] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMark[2] );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMsg[0] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMsg[1] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleFormationMsg[2] );

	m_EveElectraSystem.SetSystemOperationTime( 0.f );
	m_EveElectraSystem.SetExceptionSystemTime( 0.f );

	m_EveElectraSystem.SetPlaneFormation( PFT_NONE );

	if ( IsMyUnit() )
		g_pX2Game->UpdateSkillSlotUI();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL);
	if( NULL != pSkillTemplet )
		const_cast<CX2SkillTree::SkillTemplet*>(pSkillTemplet)->m_eActiveSkillUseCondtion = CX2SkillTree::ASUT_GROUND;

	m_EveElectraSystem.SetEnableSystem( false );

	m_bStartSpectroElCrystal = false;		/// �б������� ��ų�� ���� ���� �Ǿ�����, ������ ���� �Ǿ������� ���� ����


}


/*static*/
void	CX2GUEve::AppendDeviceToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

void CX2GUEve::DamageReact( CX2DamageManager::DamageData* pDamageData )
{
	KTDXPROFILE();
	CX2GUUser::DamageReact( pDamageData );
	CX2GUUser::DamageReactStateChange( pDamageData );

	if( -1 < m_iFerdinandNPCUID )
	{
		CX2GUNPC* pFerdinand = g_pX2Game->GetNPCUnitByUID( m_iFerdinandNPCUID );
		if( NULL != pFerdinand && pFerdinand->GetDyingStart() != true )
		{
			D3DXVECTOR3 vecMyDir = GetDirVector();
			D3DXVECTOR3 vecMyPos = GetPos();
			bool bMyIsRight = GetIsRight();

			vecMyPos.x += ( vecMyDir.x * 20.f );

			pFerdinand->SetPosition(vecMyPos, bMyIsRight);
			pFerdinand->StateChange( "FERDINAND_ATTACK_A_EMERGENCY", true );
		}
	}

	m_fInvisibilityTime = 0.f;
}

/*virtual*/ void CX2GUEve::AttackResultPlayEffect()
{
	CX2GUUser::AttackResultPlayEffect();

	//{{ kimhc // 2010.02.10 //	�׸޽ý� �нú� ���ҵ� ��� ��ȭ
	if ( DidHitNasodWeapon() == true &&
		g_pX2Game != NULL &&
		g_pX2Game->GetEffectSet() != NULL )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Eve_NasodWeapon", static_cast< CX2GameUnit* >( this ),
			NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ), true, m_DamageData.impactPoint );
		SetHitNasodWeapon( false );
	}

	//}} kimhc // 2010.1.28 //	�׸޽ý� �нú� ���ҵ� ��� ��ȭ
}


void CX2GUEve::Win()
{
	CX2GUUser::Win();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}

void CX2GUEve::Lose()
{
	CX2GUUser::Lose();

	if( IsMyUnit() == true &&
		GetNowHp() > 0.0f )
	{
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			StateChange( USI_RIDING_WAIT );
		else
			StateChange( USI_LOSE );
	}
}

void CX2GUEve::Draw()
{
	CX2GUUser::Draw();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}


void CX2GUEve::ParseCommonRandomState()
{
	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{

		const char* tableName = "";
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_NASOD:
			{
				tableName = "EVE_NASOD";
			} break;

		case CX2Unit::UC_EVE_EXOTIC_GEAR:
			{
				tableName = "EVE_EXOTIC_GEAR";
			} break;


		case CX2Unit::UC_EVE_ARCHITECTURE:
			{
				tableName = "EVE_ARCHITECTURE";
			} break;

		case CX2Unit::UC_EVE_CODE_NEMESIS:
			{
				tableName = "UC_EVE_CODE_NEMESIS";
			} break;


		case CX2Unit::UC_EVE_CODE_EMPRESS:
			{
				tableName = "UC_EVE_CODE_EMPRESS";
			} break;
		case CX2Unit::UC_EVE_ELECTRA:
			{
				tableName = "UC_EVE_ELECTRA";
			} break;
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			{
				tableName = "UC_EVE_BATTLE_SERAPH";
			} break;
		default:
			{
				ASSERT( !"NO" );
			} break;
		}

		CX2GUUser::ParseCommonRandomState(tableName);

		m_LuaManager.EndTable(); // INIT_COMMON_RANDOM_STATE
	}
}



void CX2GUEve::InitStateID()
{
	m_CommonState.m_Wait	= USI_WAIT;
	m_ChargeMpState			= USI_WAIT;

//#ifdef PVP_BOSS_COMBAT_TEST
//
//	m_FrozenState = ENSI_FROZEN;
//
//#endif PVP_BOSS_COMBAT_TEST


//#ifdef TRANSFORMER_TEST
//	m_CommonState.m_Transformed			= ENSI_TRANSFORMED;
//#endif TRANSFORMER_TEST

}

void CX2GUEve::InitStateCommon()
{
	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "ENSI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DIE_BACK_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DIE_BACK_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DIE_BACK_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DIE_BACK_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "ENSI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "ENSI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_WAIT_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_WAIT_Start );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_WAIT_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "ENSI_WALK", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_WALK_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_LANDING_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_LANDING_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "ENSI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, DashStartFuture );	// Override
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "ENSI_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_END_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_END_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_Start );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_EndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_LANDING_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_DASH_JUMP_POWER_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_POWER_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_POWER_LANDING_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_POWER_LANDING_Start );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_POWER_LANDING_EventProcess );
	m_StateList[stateData.stateID] = stateData;




	stateData.Init();
	stateData.stateID			= ENSI_JUMP_FLY_UP;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_FLY_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_UP_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_UP_Start );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_UP_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_UP_EndFuture );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ENSI_JUMP_FLY_DOWN;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_FLY_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_DOWN_StartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_DOWN_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_FLY_DOWN_EndFuture );
	m_StateList[stateData.stateID] = stateData;





	stateData.Init();
	stateData.stateID			= ENSI_JUMP_DASH;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_DASH_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_DASH_Start );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_DASH_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_DASH_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_DASH_EndFuture );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ENSI_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_SMALL_FRONT_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_SMALL_BACK_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_BIG_FRONT_Start );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_BIG_FRONT_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_BIG_BACK_Start );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_BIG_BACK_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_DOWN_FRONT_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_DOWN_FRONT_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_DOWN_BACK_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_DOWN_BACK_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DAMAGE_AIR_DOWN_LANDING_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "ENSI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_STANDUP_ROLLING_FRONT_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_STANDUP_ROLLING_FRONT_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_FRONT_FRONT_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_FRONT_FRONT_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_STANDUP_ROLLING_FRONT_BACK;
	m_LuaManager.MakeTableReference( "ENSI_STANDUP_ROLLING_FRONT_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_FRONT_BACK_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_FRONT_BACK_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_STANDUP_ROLLING_BACK_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_STANDUP_ROLLING_BACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_BACK_FRONT_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_BACK_FRONT_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_STANDUP_ROLLING_BACK_BACK;
	m_LuaManager.MakeTableReference( "ENSI_STANDUP_ROLLING_BACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_BACK_BACK_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ROLLING_BACK_BACK_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_STANDUP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ATTACK_FRONT_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ATTACK_FRONT_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ATTACK_FRONT_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_STANDUP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "ENSI_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ATTACK_BACK_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ATTACK_BACK_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_STANDUP_ATTACK_BACK_End );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_COMBO_Z;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_Z", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_Z_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_Z_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_Z_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_Z_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZ", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZ_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZZ", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZ_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_COMBO_ZZZZ;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZZZ", stateData.stateID );
	//stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZZ_FrameMoveFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZZ_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_COMBO_ZZZX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZZX", stateData.stateID );
	//stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZX_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZX_EventProcess );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ENSI_COMBO_ZZZdownZ;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZZdownZ", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZ_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZ_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_COMBO_X;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_X", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_X_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_X_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_X_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_COMBO_XX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_XX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XX_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XX_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_COMBO_XXX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_XXX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXX_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXX_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_COMBO_XXfrontX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_XXfrontX", stateData.stateID );
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXfrontX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXfrontX_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_COMBO_XXdownX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_XXdownX", stateData.stateID );
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXdownX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXdownX_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_DASH_COMBO_Z;
	m_LuaManager.MakeTableReference( "ENSI_DASH_COMBO_Z", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_Z_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_Z_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_Z_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_Z_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_DASH_COMBO_X;
	m_LuaManager.MakeTableReference( "ENSI_DASH_COMBO_X", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_X_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_X_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_JUMP_COMBO_Z;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_COMBO_Z", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_Z_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_Z_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_Z_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_Z_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_JUMP_COMBO_X;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_COMBO_X", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_JUMP_COMBO_X_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_JUMP_COMBO_X_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_LANDING_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_LANDING_Start );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_JUMP_COMBO_X_LANDING_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_DASH_JUMP_COMBO_Z;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_COMBO_Z", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_Z_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_Z_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_Z_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_Z_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_DASH_JUMP_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_COMBO_ZZ", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZ_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ENSI_DASH_JUMP_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_COMBO_ZZZ", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZZ_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZZZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ENSI_DASH_JUMP_COMBO_X;
	m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_COMBO_X", stateData.stateID );
	//stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_X_Init );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_X_Start );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_X_End );
	//stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_X_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_X_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_X_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_WALL_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "ENSI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "ENSI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "ENSI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "ENSI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ENSI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "ENSI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;

	//#pragma region ENSI_RIDING_START
	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "ENSI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_ON
	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOnEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_OFF
	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_WAIT_HABIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_WAIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_WALK
	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DASH
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DASH_END
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DASH_JUMP
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DASH_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DAMAGE_FRONT
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DAMAGE_BACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_DIE
	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DIE_FRONT_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_JUMP_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_ATTACK_X
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_SPECIAL_ATTACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ENSI_RIDING_SPECIAL_MOVE
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_SPECIAL_MOVE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveEndFutrue );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	stateData.Init();
	stateData.stateID			= USI_RIDING_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ENSI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingHyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, RidingHyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;






//#ifdef PVP_BOSS_COMBAT_TEST
//
//	stateData.Init();
//	stateData.stateID			= ENSI_FROZEN;
//	m_LuaManager.MakeTableReference( "ENSI_FROZEN", stateData.stateID );
//	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, Frozen_StateStart );
//	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, Frozen_EventProcess );
//	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, Frozen_StateEnd );
//	m_StateList[stateData.stateID] = stateData;
//
//	m_FrozenState = ENSI_FROZEN;
//
//#endif PVP_BOSS_COMBAT_TEST






//#ifdef TRANSFORMER_TEST
//
//	stateData.Init();
//	stateData.stateID			= ENSI_TRANSFORMED;
//	m_LuaManager.MakeTableReference( "ENSI_TRANSFORMED", stateData.stateID );
//	m_StateList[stateData.stateID] = stateData;
//	m_CommonState.m_Transformed = ENSI_TRANSFORMED;
//
//
//#endif TRANSFORMER_TEST

	//stateData.Init();
	//stateData.stateID			= ENSI_SUPER_DASH_READY;
	//m_LuaManager.MakeTableReference( "ENSI_SUPER_DASH_READY", stateData.stateID );
	//stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SUPER_DASH_READY_EventProcess );
	//m_StateList[stateData.stateID] = stateData;

	//stateData.Init();
	//stateData.stateID			= ENSI_SUPER_DASH;
	//m_LuaManager.MakeTableReference( "ENSI_SUPER_DASH", stateData.stateID );
	//stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SUPER_DASH_FrameMoveFuture );
	//stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SUPER_DASH_EventProcess );
	//m_StateList[stateData.stateID] = stateData;

	//stateData.Init();
	//stateData.stateID			= ENSI_SUPER_DASH_END;
	//m_LuaManager.MakeTableReference( "ENSI_SUPER_DASH_END", stateData.stateID );
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SUPER_DASH_END_FrameMove );
	//stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SUPER_DASH_END_EventProcess );
	//m_StateList[stateData.stateID] = stateData;
}


void CX2GUEve::InitState()
{
	ASSERT( NULL != GetUnit() );


	const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_EVE_NASOD:
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_ARCHITECTURE:
		{
			m_iSkillCutInSetIndex = 0;
		} break;

		//{{ kimhc // 2010.2.17 //	�̺� 2�� ���� ��ų ����
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_iSkillCutInSetIndex = 1;
		} break;

	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_iSkillCutInSetIndex = 2;
		} break;
	case CX2Unit::UC_EVE_BATTLE_SERAPH:
		{
			m_iSkillCutInSetIndex = 3;
		} break;
		//}} kimhc // 2010.2.17 //	�̺� 2�� ���� ��ų ����
	default:
		break;
	}

	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
		{
			m_iSkillCutInSetSubIndex = 1;
		} break;
	case CX2Unit::UC_EVE_ARCHITECTURE:
		{
			m_iSkillCutInSetSubIndex = 2;
		} break;
	default:
		{
			m_iSkillCutInSetSubIndex = 0;
		}
		break;
	}

	InitStateID();

	// �������� ���� ����
	InitStateCommon();

	// �������� ���� ������ ���� start, win, lose ���� �ʱ�ȭ
	std::string tableNameStartUTF8;
	std::string tableNameWinUTF8;
	std::string tableNameLoseUTF8;
	InitStateCommonRandom( tableNameStartUTF8, tableNameWinUTF8, tableNameLoseUTF8 );


	UserUnitStateData stateData;


	stateData.Init();
	stateData.stateID			= USI_START;
	m_LuaManager.MakeTableReference( tableNameStartUTF8.c_str(), stateData.stateID );
	//{{ kimhc // 2010-03-09 // start ����� effect���� effectset���� �ٲ�
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, StartFrameMove );
	//}} kimhc // 2010-03-09 // start ����� effect���� effectset���� �ٲ�

	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WIN;
	m_LuaManager.MakeTableReference( tableNameWinUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, WinStateStart );
	//{{ kimhc // 2010-03-09 // Win ����� effect���� effectset���� �ٲ�
	//stateData.StateStart		= SET_CB_FUNC( CX2GUEve, WinStart );
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, WinFrameMove );
	//stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, WinEnd );
	//}} kimhc // 2010-03-09 // Win ����� effect���� effectset���� �ٲ�
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_LOSE;
	m_LuaManager.MakeTableReference( tableNameLoseUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, LoseStateStart );
	//{{ kimhc // 2010-03-09 // start ����� effect���� effectset���� �ٲ�
	//stateData.StateStart		= SET_CB_FUNC( CX2GUEve, LoseStart );
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, LoseFrameMove );
	//}} kimhc // 2010-03-09 // start ����� effect���� effectset���� �ٲ�
	m_StateList[stateData.stateID] = stateData;

	if( 0 == tableNameWinUTF8.compare( "ENSI_WIN2" ) || 0 == tableNameWinUTF8.compare( "ENSI_AT_WIN2" ) || 0 == tableNameWinUTF8.compare( "ENSI_EG_WIN2" ))
	{
		m_bIsWinMotion2 = true;
	}

	if( 0 == tableNameLoseUTF8.compare( "ENSI_LOSE2" ) || 0 == tableNameLoseUTF8.compare( "ENSI_AT_LOSE2" ) || 0 == tableNameLoseUTF8.compare( "ENSI_EG_LOSE2" ))
	{
		m_bIsLoseMotion2 = true;
	}





	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_COMBO_ZZZdownZZ;
			m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZZdownZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZZ_Init );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZZ_StartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZZ_Start );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZZ_End );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZZdownZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}

	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_COMBO_XXZ4;
			m_LuaManager.MakeTableReference( "ENSI_COMBO_XXZ4", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXZ4_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXZ4_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXZ4_EventProcess );
			m_StateList[stateData.stateID] = stateData;


		} break;
	default:
		break;
	}

	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_EVE_ELECTRA:
	case CX2Unit::UC_EVE_BATTLE_SERAPH:
		{
			stateData.Init();
			stateData.stateID			= ENSI_EPR_COMBO_ZZZfrontZ;
			m_LuaManager.MakeTableReference( "ENSI_EPR_COMBO_ZZZfrontZ", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_ZZZfrontZ_Start );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_ZZZfrontZ_End );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_ZZZfrontZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_ZZZfrontZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_EPR_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "ENSI_EPR_COMBO_XXZ", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_XXZ_Start );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_XXZ_End );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_XXZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_COMBO_XXZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}


	stateData.Init();
	stateData.stateID			= ENSI_COMBO_XXfrontXX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_XXfrontXX", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXfrontXX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXfrontXX_EventProcess );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ENSI_COMBO_XXdownXX;
	m_LuaManager.MakeTableReference( "ENSI_COMBO_XXdownXX", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXdownXX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_XXdownXX_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_EventProcess );

//#pragma region �ν�Ʈ �뽬 ��ŸƮ
	stateData.Init();
	stateData.stateID			= ENSI_EBS_BOOST_DASH_START;
	m_LuaManager.MakeTableReference( "ENSI_EBS_BOOST_DASH_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_START_StartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_START_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_START_EndFuture );
	m_StateList[stateData.stateID] = stateData;
//#pragma endregion

//#pragma region �ν�Ʈ �뽬
	stateData.Init();
	stateData.stateID			= ENSI_EBS_BOOST_DASH;
	m_LuaManager.MakeTableReference( "ENSI_EBS_BOOST_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_FrameMove );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_End );
	m_StateList[stateData.stateID] = stateData;
//#pragma endregion

//#pragma region �ν�Ʈ �뽬 ��ŸƮ
	stateData.Init();
	stateData.stateID			= ENSI_EBS_BOOST_DASH_AIR_START;
	m_LuaManager.MakeTableReference( "ENSI_EBS_BOOST_DASH_AIR_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_START_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_START_Start );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_START_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_START_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_START_EndFuture );
	m_StateList[stateData.stateID] = stateData;
//#pragma endregion

//#pragma region �ν�Ʈ �뽬 ����
	stateData.Init();
	stateData.stateID			= ENSI_EBS_BOOST_DASH_AIR;
	m_LuaManager.MakeTableReference( "ENSI_EBS_BOOST_DASH_AIR", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_StartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_FrameMove );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_EndFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_BOOST_DASH_AIR_End );
	m_StateList[stateData.stateID] = stateData;
//#pragma endregion

//#pragma region �뽬 ���� �޺� X(Push)Z
	stateData.Init();
	stateData.stateID			= ENSI_EBS_DASH_JUMP_COMBO_XZ;
	m_LuaManager.MakeTableReference( "ENSI_EBS_DASH_JUMP_COMBO_XZ", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_XZ_Start );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_XZ_EndFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_XZ_End );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_XZ_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_XZ_EventProcess );
	m_StateList[stateData.stateID] = stateData;
//#pragma endregion

//#pragma region �̺� �ν�Ʈ ���� ����
	stateData.Init();
	stateData.stateID			= ENSI_EBS_DASH_JUMP_COMBO_ZFrontX;
	m_LuaManager.MakeTableReference( "ENSI_EBS_DASH_JUMP_COMBO_ZFrontX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_EndFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_EBS_DASH_JUMP_COMBO_ZUpX;
	m_LuaManager.MakeTableReference( "ENSI_EBS_DASH_JUMP_COMBO_ZUpX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZUpX_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZUpX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EndFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZUpX_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ENSI_EBS_DASH_JUMP_COMBO_ZDownX;
	m_LuaManager.MakeTableReference( "ENSI_EBS_DASH_JUMP_COMBO_ZDownX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZDownX_Start );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZDownX_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZDownX_EventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZDownX_EndFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_DASH_JUMP_COMBO_ZDownX_End );
	m_StateList[stateData.stateID] = stateData;
//#pragma endregion


	/////////////////////// ��� �޺� //////////////////////////////////////////////////
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_DASH_COMBO_ZZ;
			m_LuaManager.MakeTableReference( "ENSI_DASH_COMBO_ZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZ_Init );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZ_StartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZ_Start );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZ_End );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_DASH_COMBO_ZZZ;
			m_LuaManager.MakeTableReference( "ENSI_DASH_COMBO_ZZZ", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZZ_StartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZZ_Start );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZZ_End );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_DASH_COMBO_XZ;
			m_LuaManager.MakeTableReference( "ENSI_DASH_COMBO_XZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_XZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_XZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
		break;

		//{{ kimhc // 2010.1.27 //	�̺� 2�� ���� �������� ��� �޺�
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_EP_DASH_COMBO_ZZ;
			m_LuaManager.MakeTableReference( "ENSI_EP_DASH_COMBO_ZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_EP_DASH_COMBO_ZZ_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EP_DASH_COMBO_ZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EP_DASH_COMBO_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_EP_DASH_COMBO_ZZZ;
			m_LuaManager.MakeTableReference( "ENSI_EP_DASH_COMBO_ZZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_EP_DASH_COMBO_ZZZ_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EP_DASH_COMBO_ZZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EP_DASH_COMBO_ZZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		}
		break;
		//}} kimhc // 2010.1.27 //	�̺� 2�� ���� �������� ��� �޺�
	case CX2Unit::UC_EVE_ELECTRA:
	case CX2Unit::UC_EVE_BATTLE_SERAPH:
		{
			stateData.Init();
			stateData.stateID			= ENSI_EPR_DASH_COMBO_ZZ;
			m_LuaManager.MakeTableReference( "ENSI_EPR_DASH_COMBO_ZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZ_Init );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZ_StartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZ_Start );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZ_FrameMove );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZ_End );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_EPR_DASH_COMBO_ZZZ;
			m_LuaManager.MakeTableReference( "ENSI_EPR_DASH_COMBO_ZZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_Init );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_StartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_Start );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_FrameMove );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_EndFuture );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EPR_DASH_COMBO_ZZZ_End );
			m_StateList[stateData.stateID] = stateData;

		}
		break;
	default:
		break;
	}

	//{{ kimhc // 2010.1.25 // �̺� 2�� �޺�
	switch ( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_NS_DASH_COMBO_ZX;
			m_LuaManager.MakeTableReference( "ENSI_NS_DASH_COMBO_ZX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZX_Init );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZX_Start );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_NS_DASH_COMBO_ZXX;
			m_LuaManager.MakeTableReference( "ENSI_NS_DASH_COMBO_ZXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZXX_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_NS_DASH_COMBO_ZXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ENSI_NS_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "ENSI_NS_COMBO_XXZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_NS_COMBO_XXZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_NS_COMBO_XXZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_NS_COMBO_XXZZ8;
			m_LuaManager.MakeTableReference( "ENSI_NS_COMBO_XXZZ8", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_NS_COMBO_XXZZ8_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_NS_COMBO_XXZZ8_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_NS_COMBO_XXZZ8Z;
			m_LuaManager.MakeTableReference( "ENSI_NS_COMBO_XXZZ8Z", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_NS_COMBO_XXZZ8Z_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_NS_COMBO_XXZZ8Z_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		}
		break;

	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_EP_JUMP_COMBO_ZX;
			m_LuaManager.MakeTableReference( "ENSI_EP_JUMP_COMBO_ZX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_EP_JUMP_COMBO_ZX_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EP_JUMP_COMBO_ZX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EP_JUMP_COMBO_ZX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ENSI_EP_COMBO_XXdownX;
			m_LuaManager.MakeTableReference( "ENSI_EP_COMBO_XXdownX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_EP_COMBO_XXdownX_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EP_COMBO_XXdownX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EP_COMBO_XXdownX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
		break;
	default:
		break;
	}
	//}} kimhc // 2010.1.25 // �̺� 2�� �޺�

	//#ifdef EVE_COMBO_TEST_TEST
	//	stateData.Init();
	//	stateData.stateID			= ENSI_DASH_COMBO_ZX;
	//	m_LuaManager.MakeTableReference( "ENSI_DASH_COMBO_ZX", stateData.stateID );
	//	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZX_FrameMove );
	//	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_COMBO_ZX_EventProcess );
	//	m_StateList[stateData.stateID] = stateData;
	//#endif EVE_COMBO_TEST_TEST



	//
	//
	//#ifdef EVE_FIRST_CHANGE_JOB
	//	//{{ 09.01.21.hoons. ���޺��� Ű������ [����ƽ���]�� ���.
	//	if( GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR )
	//	{
	//		stateData.Init();
	//		stateData.stateID			= ENSI_DASH_JUMP_COMBO_ZX;
	//		m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_COMBO_ZX", stateData.stateID );
	//		stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZX_FrameMove );
	//		stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZX_EventProcess );
	//		stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_ZX_End );
	//		m_StateList[stateData.stateID] = stateData;
	//	}
	//	//}} hoons.
	//#endif EVE_FIRST_CHANGE_JOB


	// {{ 09.02.02 wonpok
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			stateData.Init();
			stateData.stateID			= ENSI_DASH_JUMP_COMBO_XZ;
			m_LuaManager.MakeTableReference( "ENSI_DASH_JUMP_COMBO_XZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_XZ_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_XZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_XZ_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_DASH_JUMP_COMBO_XZ_End );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}





	//stateData.Init();
	//stateData.stateID			= ENSI_COMBO_ZZX;
	//m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZX", stateData.stateID );
	//stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZX_EventProcess );
	//m_StateList[stateData.stateID] = stateData;

	//stateData.Init();
	//stateData.stateID			= ENSI_COMBO_ZZXX;
	//m_LuaManager.MakeTableReference( "ENSI_COMBO_ZZXX", stateData.stateID );
	//stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZXX_FrameMoveFuture );
	//stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_COMBO_ZZXX_EventProcess );
	//m_StateList[stateData.stateID] = stateData;



	InitPassiveSkillState();
	InitEquippedActiveSkillState();

	CX2GUUser::InitState();

}

/*virtual*/ void CX2GUEve::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
{
	if ( iSlotIndex_ < 0 || iSlotIndex_ >= ARRAY_SIZE( _CONST_GUUSER_::aNormalSkillStateID ) )
		return;

	int iNormalStateID = ( bSlotB_ == false ? _CONST_GUUSER_::aNormalSkillStateID[iSlotIndex_] : _CONST_GUUSER_::aNormalSkillStateIDSlotB[iSlotIndex_] );
	UserUnitStateData& normalStateData = m_StateList[ iNormalStateID ];
	normalStateData.Init();
	normalStateData.stateID = iNormalStateID;

	int iHyperStateID = ( bSlotB_ == false ? _CONST_GUUSER_::aHyperSkillStateID[iSlotIndex_] : _CONST_GUUSER_::aHyperSkillStateIDSlotB[iSlotIndex_] );
	UserUnitStateData& hyperStateData = m_StateList[ iHyperStateID ];
	hyperStateData.Init();
	hyperStateData.m_bHyperState = true;
	hyperStateData.stateID = iHyperStateID;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	if( NULL == pSkillTemplet )
		return;

	switch( pSkillTemplet->m_eType )
	{
	case CX2SkillTree::ST_BUFF:
		{	// ���� �ʻ��
			m_LuaManager.MakeTableReference( "ENSI_SI1_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI1_COMMON_BUFF_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, COMMON_BUFF_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			return;
		} break;
	case CX2SkillTree::ST_ACTIVE:
	case CX2SkillTree::ST_SPECIAL_ACTIVE:
	case CX2SkillTree::ST_HYPER_ACTIVE_SKILL:
		{
			// ST_ACTIVE, ST_SPECIAL_ACTIVE�� �Ʒ� �������� ����
		} break;
	case CX2SkillTree::ST_RELATIONSHIP_SKILL:
		{
			switch( eSkillID_ )
			{
			case CX2SkillTree::SI_ETC_WS_COMMON_LOVE:
				{
					m_LuaManager.MakeTableReference( "ENSI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "ENSI_THROW_ITEM", hyperStateData.stateID );
					hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
					hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
				} break;
			default:
				break;
			}
			return;
		} break;
	default:
		{
			return;
		} break;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// ���⼭ ���� ACTIVE, SPECIAL ACTIVE ��ų//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	switch( eSkillID_ )
	{
	default:
		case CX2SkillTree::SI_A_EN_ILLUSION_STRIKE:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI1_ILLUSION_STRIKE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_ILLUSION_STRIKE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_ILLUSION_STRIKE_FrameMove );
			//normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_ILLUSION_STRIKE_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_ILLUSION_STRIKE_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI1_ILLUSION_STRIKE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_ILLUSION_STRIKE_HYPER_FrameMove );
			//hyperStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_MAXIMUM_CANNON_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_ILLUSION_STRIKE_HYPER_EventProcess );
		} break;

		case CX2SkillTree::SI_A_EN_DIMENSION_LINK:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI1_DIMENSION_LINK", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_FrameMove );
			//normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI1_DIMENSION_LINK_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_HYPER_FrameMove );
			//hyperStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_DIMENSION_LINK_HYPER_EventProcess );

		} break;


	case CX2SkillTree::SI_SA_EN_ILLUSION_STINGER:
		{
			const char* pszStateName = "ENSI_SI2_ILLUSION_STINGER";
			const char* pszHyperStateName = "ENSI_SI2_ILLUSION_STINGER_HYPER";

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
			{
				pszStateName = "ENSI_SI2_ILLUSION_STINGER_MEMO";
				pszHyperStateName = "ENSI_SI2_ILLUSION_STINGER_HYPER_MEMO";
			}

			m_LuaManager.MakeTableReference( pszStateName, normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_Start );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_End );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_EventProcess );

			m_LuaManager.MakeTableReference( pszHyperStateName, hyperStateData.stateID );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_HYPER_Start );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_HYPER_End );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_ILLUSION_STINGER_HYPER_EventProcess );
		} break;

	case CX2SkillTree::SI_SA_EN_DIMENSION_LINK_BLADER:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI2_DIMENSION_LINK_BLADER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_BLADER_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_BLADER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_BLADER_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI2_DIMENSION_LINK_BLADER_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_BLADER_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_BLADER_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_EN_GENERATE_BLACKHOLE:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI3_GENERATE_BLACKHOLE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_Init );
			//normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_EventProcess );
			//normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_End );

			m_LuaManager.MakeTableReference( "ENSI_SI3_GENERATE_BLACKHOLE_HYPER", hyperStateData.stateID );
			//hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_HYPER_EventProcess );
			//hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_GENERATE_BLACKHOLE_HYPER_End );

		} break;

	case CX2SkillTree::SI_SA_EN_DIMENSION_LINK_GUARDIAN:
		{

			m_LuaManager.MakeTableReference( "ENSI_SI3_DIMENSION_LINK_GUARDIAN", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_Init );
			//normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_FrameMove );
			//normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_GUARDIAN_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER", hyperStateData.stateID );
			//hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_FrameMove );
			//hyperStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_DIMENSION_LINK_GUARDIAN_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_EventProcess );
		} break;

		case CX2SkillTree::SI_A_EEG_HORNET_STING_EXPLOSION:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI1_EG_HORNET_STING", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_EG_HORNET_STING_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_EG_HORNET_STING_FrameMove );
			//normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_EG_HORNET_STING_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_EG_HORNET_STING_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI1_EG_HORNET_STING_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_EG_HORNET_STING_HYPER_FrameMove );
			//hyperStateData.OnCameraMove		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_MAXIMUM_CANNON_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_EG_HORNET_STING_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_EEG_EXPLOSION_IMPACT:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI2_EG_EXPLOSION_IMPACT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_Start );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_End );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_EEG_QUEENS_THRONE:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI3_EG_QUEENS_THRONE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_Start );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_End );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI3_EG_QUEENS_THRONE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_EG_QUEENS_THRONE_HYPER_EventProcess );
		} break;

		case CX2SkillTree::SI_A_EAT_SURFACE_CUTING:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI1_AT_SURFACE_CUTING", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_AT_SURFACE_CUTING_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_AT_SURFACE_CUTING_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_AT_SURFACE_CUTING_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI1_AT_SURFACE_CUTING_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI1_AT_SURFACE_CUTING_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI1_AT_SURFACE_CUTING_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_EAT_SONIC_WAVE:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI2_AT_SONIC_WAVE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_AT_SONIC_WAVE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_AT_SONIC_WAVE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_AT_SONIC_WAVE_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI2_AT_SONIC_WAVE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI2_AT_SONIC_WAVE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI2_AT_SONIC_WAVE_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_EAT_GENOCIDE_RIPPER:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI3_AT_GENOCIDE_RIPPER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_Start );
			//normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_EventProcess );
			//normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_End );

			m_LuaManager.MakeTableReference( "ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER", hyperStateData.stateID );
			//hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_EventProcess );
			//hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_End );

		} break;


	case CX2SkillTree::SI_A_EN_CLOAKING:
		{
			m_LuaManager.MakeTableReference( "ENSI_INVISIBILITY_ON", normalStateData.stateID );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_Start );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_End );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_EventProcess );


			m_LuaManager.MakeTableReference( "ENSI_INVISIBILITY_ON", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_Start );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_End );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_INVISIBILITY_ON_EventProcess );

		} break;
		// ������ȯ
	case CX2SkillTree::SI_A_EN_CONVERSION:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI_A_EN_CONVERSION", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_CONVERSION_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_CONVERSION_Start );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_CONVERSION_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SI_A_EN_CONVERSION_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_CONVERSION_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_CONVERSION_Start );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_CONVERSION_EventProcess );
		} break;
		// �ް��Ϸ�Ʈ�к�
	case CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI_A_EN_MEGA_ELECTRONBALL", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_StartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_Start );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_EndFuture );

			m_LuaManager.MakeTableReference( "ENSI_SI_A_EN_MEGA_ELECTRONBALL_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_Init );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_StartFuture );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_Start );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_MEGA_ELECTRONBALL_EndFuture );
		} break;





		case CX2SkillTree::SI_SA_ENS_IRONSCRAPS:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EEG_IRONSCRAPS", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_StateEnd );


			m_LuaManager.MakeTableReference( "ENSI_SA_EEG_IRONSCRAPS_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_HYPER_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_IRONSCRAPS_HYPER_StateEnd );

		} break;

		case CX2SkillTree::SI_SA_EEP_SPACE_WRENCH:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EAT_SPACE_WRENCH", normalStateData.stateID );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_StateEnd );


			m_LuaManager.MakeTableReference( "ENSI_SA_EAT_SPACE_WRENCH_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_SPACE_WRENCH_HYPER_StateEnd );

		} break;

	case CX2SkillTree::SI_A_EEG_METAL_DUST_AURA:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EEG_METAL_DUST_AURA", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_StateEnd );


			m_LuaManager.MakeTableReference( "ENSI_A_EEG_METAL_DUST_AURA_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_HYPER_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_METAL_DUST_AURA_HYPER_StateEnd );

		} break;

	case CX2SkillTree::SI_A_EAT_OBERON_GUARD:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EAT_OBERON_GUARD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_StateEnd );


			m_LuaManager.MakeTableReference( "ENSI_A_EAT_OBERON_GUARD_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_HYPER_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_OBERON_GUARD_HYPER_StateEnd );

		} break;

	case CX2SkillTree::SI_SA_EEG_JUNK_BREAK:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EEG_JUNK_BREAK", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_JUNK_BREAK_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_JUNK_BREAK_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_JUNK_BREAK_EventProcess );


			m_LuaManager.MakeTableReference( "ENSI_A_EEG_JUNK_BREAK_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_JUNK_BREAK_Init );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_JUNK_BREAK_Hyper_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEG_JUNK_BREAK_Hyper_EventProcess );
		} break;

		case CX2SkillTree::SI_SA_EAT_HEAVENS_FIST_PRESSURE:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EAT_HEAVENS_FIST", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_HEAVENS_FIST_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_HEAVENS_FIST_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_HEAVENS_FIST_EventProcess );


			m_LuaManager.MakeTableReference( "ENSI_A_EAT_HEAVENS_FIST_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_HEAVENS_FIST_Init );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_HEAVENS_FIST_Hyper_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EAT_HEAVENS_FIST_Hyper_EventProcess );
		} break;

		//{{ kimhc // 2010.1.27 //	�̺� 2�� ���� ��ų
		case CX2SkillTree::SI_A_ENS_ATOMIC_BLASTER_PLASMAGUN:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_ENS_ATOMIC_BLASTER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_Start );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_End );

			m_LuaManager.MakeTableReference( "ENSI_SA_ENS_ATOMIC_BLASTER_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_Start );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_ATOMIC_BLASTER_End );
		} break;

	case CX2SkillTree::SI_A_ENS_SPEAR_BURST:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_ENS_BURST_SPEAR", normalStateData.stateID );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Start );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_End );

			m_LuaManager.MakeTableReference( "ENSI_A_ENS_BURST_SPEAR_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Hyper_Start );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Hyper_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Hyper_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Hyper_End );

			{
				UserUnitStateData	stateData;

				stateData.Init();
				stateData.stateID				= ENSI_A_ENS_BURST_SPEAR_Landing;
				m_LuaManager.MakeTableReference( "ENSI_A_ENS_BURST_SPEAR_Landing", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Landing_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Landing_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ENSI_A_ENS_BURST_SPEAR_Landing_HYPER;
				m_LuaManager.MakeTableReference( "ENSI_A_ENS_BURST_SPEAR_Landing_HYPER", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Landing_Hyper_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_ENS_BURST_SPEAR_Landing_Hyper_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}

		} break;

		// oasis907 : ����� [2010.1.28] //  �������� ��ų
	case CX2SkillTree::SI_A_EEP_ELECTRONIC_FIELD:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EEP_ELECTRONIC_FIELD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEP_ELECTRONIC_FIELD_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEP_ELECTRONIC_FIELD_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEP_ELECTRONIC_FIELD_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_EEP_ELECTRONIC_FIELD_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_Init );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_EventProcess );
		} break;

		case CX2SkillTree::SI_A_EEP_SPIT_FIRE_FLUSH:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EEP_SPIT_FIRE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_SPIT_FIRE_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_SPIT_FIRE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_SPIT_FIRE_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_A_EEP_SPIT_FIRE_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_SPIT_FIRE_Hyper_Init );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_SPIT_FIRE_Hyper_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_SPIT_FIRE_Hyper_EventProcess );

		} break;




		//}} kimhc // 2010.1.27 //	�̺� 2�� ���� ��ų

		//{{ JHKang / ������ / 2010/11/08 / �Ʈ ���Ǿ�
		case CX2SkillTree::SI_SA_EEP_ASSUALT_SPEAR_BURST:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EPP_ASSUALT_SPEAR", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_ASSUALT_SPEAR_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_ASSUALT_SPEAR_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_ASSUALT_SPEAR_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_EPP_ASSUALT_SPEAR_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_EventProcess );
		} break;
		//}} JHKang / ������ / 2010/11/08 / �Ʈ ���Ǿ�
		//{{ JHKang / ������ / 2010/11/12 / ����� ����
	case CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EEG_ATOMIC_SHIELD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_EEG_ATOMIC_SHIELD_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_EventProcess );

		} break;
		//{{ JHKang / ������ / 2010/11/12 / ����� ����
		case CX2SkillTree::SI_A_EN_PHOTON_BLINK:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EVE_PHOTON_BLINK", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_Start );
			normalStateData.StateStartFuture = SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_StartFuture );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_EventProcess );
			normalStateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_FrameMoveFuture );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_FrameMove );
			normalStateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_EndFuture );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_End );

			m_LuaManager.MakeTableReference( "ENSI_A_EVE_PHOTON_BLINK", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_Start );
			hyperStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_StartFuture );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_EventProcess );
			hyperStateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_FrameMoveFuture );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_FrameMove );
			hyperStateData.StateEndFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_EndFuture );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_A_EVE_PHOTON_BLINK_End );

		} break;

	case CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EPR_SPECTRO_EL_CRYSTAL", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Start );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_A_EPR_SPECTRO_EL_CRYSTAL", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Start );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EPR_SPECTRO_EL_CRYSTAL_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_EEL_PARTICLE_RAY:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_PARTICLE_RAY", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_Start );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_FrameMoveFuture );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_End );

			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_PARTICLE_RAY", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_Start );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_StartFuture );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_FrameMoveFuture );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PARTICLE_RAY_End );
		} break;

		case CX2SkillTree::SI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_SWEEP_ROLLING_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_End );

			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_SWEEP_ROLLING_READY", hyperStateData.stateID );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_Start );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_READY_End );

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ENSI_SA_EPR_SWEEP_ROLLING_CHARGE;
				m_LuaManager.MakeTableReference( "ENSI_SA_EPR_SWEEP_ROLLING_CHARGE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ENSI_SA_EPR_SWEEP_ROLLING_FIRE;
				m_LuaManager.MakeTableReference( "ENSI_SA_EPR_SWEEP_ROLLING_FIRE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_FIRE_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_FIRE_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_FIRE_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_FIRE_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ENSI_SA_EPR_SWEEP_ROLLING_END;
				m_LuaManager.MakeTableReference( "ENSI_SA_EPR_SWEEP_ROLLING_END", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_END_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_END_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_SWEEP_ROLLING_END_End );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	case CX2SkillTree::SI_SA_EEL_PHOTON_FLARE:
		{

			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_PHOTON_FLARE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_StateEnd );

			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_PHOTON_FLARE", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_PHOTON_FLARE_StateEnd );
		} break;

	case CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_THOUSANDS_OF_STARS", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_THOUSANDS_OF_STARS_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_THOUSANDS_OF_STARS_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_THOUSANDS_OF_STARS_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_EPR_THOUSANDS_OF_STARS", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_THOUSANDS_OF_STARS_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_THOUSANDS_OF_STARS_EventProcess );
		} break;

	case CX2SkillTree::SI_SA_EEL_GIGA_STREAM:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_GIGA_STREAM", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_Start );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_FrameMoveFuture );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_End );

			m_LuaManager.MakeTableReference( "ENSI_SA_GIGA_STREAM", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_Start );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_StartFuture );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_FrameMoveFuture );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EPR_GIGA_STREAM_End );
		} break;


//#pragma region ������ �ʶ�
	case CX2SkillTree::SI_A_EBS_TASER_PILUM:
		{
			m_LuaManager.MakeTableReference( "ENSI_SI_A_EBS_TASER_PILUM", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_StartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_Start );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_EndFuture );

			m_LuaManager.MakeTableReference( "ENSI_SI_A_EBS_TASER_PILUM", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_Init );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_StartFuture );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_Start );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_TASER_PILUM_EndFuture );
		} break;
//#pragma endregion

//#pragma region ������ �ϵ齺
	case CX2SkillTree::SI_SA_EBS_ENERGY_NEEDLES:
		{
			m_LuaManager.MakeTableReference( "ENSI_EBS_ENERGY_NEEDLES", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_Start );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_End );

			m_LuaManager.MakeTableReference( "ENSI_EBS_ENERGY_NEEDLES", hyperStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_Start );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGY_NEEDLES_End );
		} break;
//#pragma endregion

//#pragma region ������ƽ ��Ʈ
	case CX2SkillTree::SI_SA_EBS_ENERGETIC_HEART:
		{
			m_LuaManager.MakeTableReference( "ENSI_EBS_ENERGETIC_HEART", normalStateData.stateID );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_Start );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_End );

			m_LuaManager.MakeTableReference( "ENSI_EBS_ENERGETIC_HEART", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_Start );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_EBS_ENERGETIC_HEART_End );
		} break;
//#pragma endregion


	//#pragma region SI_SA_EN_ILLUSION_RAY
	case CX2SkillTree::SI_A_EN_ILLUSION_RAY:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EN_ILLUSION_RAY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_Start );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Illusion_Ray_End );

			m_LuaManager.MakeTableReference( "ENSI_SA_EN_ILLUSION_RAY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		} break;
	//#pragma endregion �Ϸ��� ����

	//#pragma region SI_A_EN_TESLA_SHOCK
	case CX2SkillTree::SI_A_EN_TESLA_SHOCK:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EN_TESLA_SHOCK", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Tesla_Shock_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Tesla_Shock_StateStart );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Tesla_Shock_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEve, ENSI_A_EN_Tesla_Shock_StateEnd );

			m_LuaManager.MakeTableReference( "ENSI_A_EN_TESLA_SHOCK", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;
		} break;
	//#pragma endregion �׽��� ��ũ

	//#pragma region SI_SA_EEG_HORNET_STING_SHAPED_CHARGE
	case CX2SkillTree::SI_A_EEG_HORNET_STING_SHAPED_CHARGE:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EEG_HORNET_STING_SHAPED_CHARGE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_EEG_HORNET_STING_SHAPED_CHARGE", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

		} break;
	//#pragma endregion ȣ�� ���� - ������Ʈ ����

	//#pragma region SI_SA_ENS_ATOMIC_BLASTER_SONICGUN
	case CX2SkillTree::SI_A_ENS_ATOMIC_BLASTER_SONICGUN:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_ENS_ATOMIC_BLASTER_SONICGUN", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Atomic_Blaster_SonicGun_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Atomic_Blaster_SonicGun_Start );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Atomic_Blaster_SonicGun_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Atomic_Blaster_SonicGun_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_ENS_ATOMIC_BLASTER_SONICGUN", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ����� �������� - �Ҵа�

	//#pragma region SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN
	case CX2SkillTree::SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN_READY", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_FIRE;
				m_LuaManager.MakeTableReference( "ENSI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN_FIRE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END;
				m_LuaManager.MakeTableReference( "ENSI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN_END", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_Start );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	//#pragma endregion ������ �Ѹ� - Ʈ���� ��ĭ

	//#pragma region SI_SA_EAT_HEAVENS_FIST_SWEEPER
	case CX2SkillTree::SI_SA_EAT_HEAVENS_FIST_SWEEPER:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EAT_HEAVENS_FIST_SWEEPER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_Heavens_Fist_Sweeper_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_Heavens_Fist_Sweeper_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EAT_Heavens_Fist_Sweeper_EventProcess );


			m_LuaManager.MakeTableReference( "ENSI_A_EAT_HEAVENS_FIST_SWEEPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ����� �ǽ�Ʈ - ������

	//#pragma region SI_SA_ENS_ASSUALT_SPEAR
	case CX2SkillTree::SI_SA_ENS_ASSUALT_SPEAR_ANNIHILATOR:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_ENS_ASSUALT_SPEAR", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Assualt_Spear_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Assualt_Spear_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_SA_ENS_Assualt_Spear_EventProcess );


			m_LuaManager.MakeTableReference( "ENSI_SA_ENS_ASSUALT_SPEAR", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion �Ʈ ���Ǿ� - �׸޽ý�

	//#pragma region SI_A_EEP_SPIT_FIRE_GRENADE
	case CX2SkillTree::SI_A_EEP_SPIT_FIRE_GRENADE:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EEP_SPIT_FIRE_GRENADE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_Spit_Fire_Grenade_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_Spit_Fire_Grenade_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEve, ENSI_A_EEP_Spit_Fire_Grenade_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_A_EEP_SPIT_FIRE_GRENADE", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

		} break;
	//#pragma endregion ���� ���̾� - �׷����̵�

	//#pragma region SI_A_EBS_KUGELBLITZ
	case CX2SkillTree::SI_A_EBS_KUGELBLITZ:
		{
			m_LuaManager.MakeTableReference( "ENSI_A_EBS_KUGELBLITZ", normalStateData.stateID );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_Kugelblitz_StartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_Kugelblitz_Start );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_Kugelblitz_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_Kugelblitz_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_Kugelblitz_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUEve, ENSI_A_EBS_Kugelblitz_EndFuture );

			m_LuaManager.MakeTableReference( "ENSI_A_EBS_KUGELBLITZ", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEndFuture		= normalStateData.StateEndFuture;
		} break;
	//#pragma endregion ���ۺ�����

	//#pragma region SI_SA_EEL_LINEAR_DIVIDER
	case CX2SkillTree::SI_SA_EEL_LINEAR_DIVIDER:
		{
			m_LuaManager.MakeTableReference( "ENSI_SA_EEL_LINEAR_DIVIDER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_Start );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_SA_EEL_LINEAR_DIVIDER_End );

			m_LuaManager.MakeTableReference( "ENSI_SA_EEL_LINEAR_DIVIDER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		} break;
	//#pragma endregion �巹�� ��ũ


	case CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION:
		{
			m_LuaManager.MakeTableReference( "ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_End );

			m_LuaManager.MakeTableReference( "ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION", hyperStateData.stateID );
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		} break;

	case CX2SkillTree::SI_HA_ENS_LUNATIC_SCUD:
		{
			m_LuaManager.MakeTableReference( "ENSI_HA_ENS_LUNATIC_SCUD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_HA_ENS_LUNATIC_SCUD_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_HA_ENS_LUNATIC_SCUD_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_HA_ENS_LUNATIC_SCUD", hyperStateData.stateID );
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_HA_EBS_PSYCHIC_ARTILLERRY :
		{
			m_LuaManager.MakeTableReference( "ENSI_HA_EBS_PSYCHIC_ARTILLERY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EBS_PSYCHIC_ARTILLERY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EBS_PSYCHIC_ARTILLERY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_HA_EBS_PSYCHIC_ARTILLERY_EventProcess );

			m_LuaManager.MakeTableReference( "ENSI_HA_EBS_PSYCHIC_ARTILLERY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEve, ENSI_HA_EBS_PSYCHIC_ARTILLERY_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEve, ENSI_HA_EBS_PSYCHIC_ARTILLERY_EventProcess );
		} break;

	}
}

/*virtual*/ void CX2GUEve::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& normalStateData = m_StateList[ ENSI_SA_EPR_SWEEP_ROLLING_CHARGE ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ENSI_SA_EPR_SWEEP_ROLLING_CHARGE ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;

			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ENSI_SA_EPR_SWEEP_ROLLING_FIRE ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ENSI_SA_EPR_SWEEP_ROLLING_FIRE ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ENSI_SA_EPR_SWEEP_ROLLING_END ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ENSI_SA_EPR_SWEEP_ROLLING_END ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& normalStateData = m_StateList[ ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_FIRE ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_FIRE ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}
		}
	}
}

/*static*/
void	CX2GUEve::AppendComponentToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}



void CX2GUEve::InitComponent()
{
	CX2GUUser::InitComponent();

	if( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
	{
		AddUnitSlashData( CX2UnitSlashTraceManager::STC_NONE );

		m_LuaManager.EndTable();
	}
}




void CX2GUEve::InitEquippedActiveSkillState(bool bOnlySkillLevel /* = false  */)
{
	CX2GUUser::InitEquippedActiveSkillState(bOnlySkillLevel);

	// �ް��Ϸ�Ʈ�к� ���Ծ��̵� �˻�.
	if( GetUnit() != NULL )
	{
		m_sMegaElectronBallData.SetSlotID( -1 );
		for( int iSlot = 0; iSlot < EQUIPPED_SKILL_SLOT_COUNT*2; ++iSlot )	// ���� �ε���.
		{
			const bool bSlotB = (iSlot > 3) ? true : false;
			const int iSlotIndex = (iSlot > 3) ? iSlot - 4 : iSlot;
			//���� B�� Ȱ��ȭ ���� ������ ���� A�� Ȯ���ϵ��� break;
			if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
				&& bSlotB == true)
			{
				break;
			}
			const CX2UserSkillTree::SkillSlotData* pSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( iSlotIndex, bSlotB );

			if( pSlotData != NULL )
			{
				if( pSlotData->m_eID == CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL )
				{
					m_sMegaElectronBallData.SetSlotID( iSlot );
					break;
				}
			}
		}

		// ������ �ʶ� ���Ծ��̵� �˻�.
		m_sTaserPilumData.SetSlotID( TASER_PILUM_Data::ST_A, -1 );
		m_sTaserPilumData.SetSlotID( TASER_PILUM_Data::ST_B, -1 );

		for( int iBool = 0; iBool < static_cast<int>(TASER_PILUM_Data::ST_END); ++iBool )	// ���� A,B
		{
			for( int iSlot = 0; iSlot < EQUIPPED_SKILL_SLOT_COUNT * 2; ++iSlot )	// ���� �ε���.
			{
				const bool bSlotB = (iSlot > 3) ? true : false;
				const int iSlotIndex = (iSlot > 3) ? iSlot - 4 : iSlot;
				//���� B�� Ȱ��ȭ ���� ������ ���� A�� Ȯ���ϵ��� break;
				if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
					&& bSlotB == true)
				{
					break;
				}
				const CX2UserSkillTree::SkillSlotData* pSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( iSlotIndex, bSlotB );

				if( pSlotData != NULL )
				{
					if( pSlotData->m_eID == CX2SkillTree::SI_A_EBS_TASER_PILUM )
					{
						m_sTaserPilumData.SetSlotID( static_cast<TASER_PILUM_Data::SLOT_TYPE>(iBool), iSlot );
						break;
					}
				}
			}
		}
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL);
		if( NULL != pSkillTemplet )
			const_cast<CX2SkillTree::SkillTemplet*>(pSkillTemplet)->m_eActiveSkillUseCondtion = CX2SkillTree::ASUT_GROUND;

		// ���ۺ����� ���Ծ��̵� �˻�.
		m_sKugelBlitz_Data.SetSlotID( SKugelBlitz_Data::ST_A, -1 );
		m_sKugelBlitz_Data.SetSlotID( SKugelBlitz_Data::ST_B, -1 );

		for( int iBool = 0; iBool < static_cast<int>( SKugelBlitz_Data::ST_END ); ++iBool )	// ���� A,B
		{
			for( int iSlot = 0; iSlot < EQUIPPED_SKILL_SLOT_COUNT * 2; ++iSlot )	// ���� �ε���.
			{
				const bool bSlotB = (iSlot > 3) ? true : false;
				const int iSlotIndex = (iSlot > 3) ? iSlot - 4 : iSlot;

				if( false == g_pData->GetMyUser()->GetSelectUnit()->GetUnitData().m_UserSkillTree.GetEnabledSkillSlotB()
					&& bSlotB == true)
				{
					break;
				}
				const CX2UserSkillTree::SkillSlotData* pSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( iSlotIndex, bSlotB );

				if( pSlotData != NULL )
				{
					if( pSlotData->m_eID == CX2SkillTree::SI_A_EBS_KUGELBLITZ )
					{
						m_sKugelBlitz_Data.SetSlotID( static_cast<SKugelBlitz_Data::SLOT_TYPE>( iBool ), iSlot );
						break;
					}
				}
			}
		}
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEG_QUEENS_THRONE );
	if( NULL != pSkillTemplet )
	{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_sEGQueensThrone.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
	}
}



void CX2GUEve::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();

	// kimhc // 2009-11-27 // �нú� ��ų �ʱ�ȭ �ÿ� �׵������ �ʱ�ȭ
	m_SkillRelatedData.m_fStatAtkRateOfSummoned = 1.f;

	// ���� ȸ���� ���� �� �ʱ�ȭ.
	SetAdditionalMPChangeRateByPassive(0.f);
	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_REFINED_STEP, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_REFINED_STEP );

		if( NULL != pSkillTemplet )
		{
			const float fMoveSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel );
			m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * fMoveSpeed );
			m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * fMoveSpeed );

			const float fJumpSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_JUMP_SPEED, iSkillLevel );
			m_AdditionalJumpPower.SetPassiveMultiple( m_AdditionalJumpPower.GetPassiveMultiple() * fJumpSpeed );
			m_AdditionalDashJumpPower.SetPassiveMultiple( m_AdditionalDashJumpPower.GetPassiveMultiple() * fJumpSpeed );
		}
	}

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_PHOTON_BOOSTER, true ); // SI_P_EPR_PHOTON_BOOSTER ���� ������

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_PHOTON_BOOSTER );

		if( NULL != pSkillTemplet )
		{
			const float fMoveSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel );
			m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * fMoveSpeed );
			m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * fMoveSpeed );
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_QUEENS_CONTROL, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_QUEENS_CONTROL );
		if( NULL != pSkillTemplet )
		{
			m_SkillRelatedData.m_fStatAtkRateOfSummoned *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SUMMON_ATK_REL, iSkillLevel );
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_PHOTON_BOOSTER, true ); // SI_P_EPR_PHOTON_BOOSTER ���� ������

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_PHOTON_BOOSTER );

		if( NULL != pSkillTemplet )
		{
			m_iPhotonBoosterAdditionalDashJumpCount = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_AIR_MOVE_COUNT, iSkillLevel );
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_REFINED_STEP, true ); // SI_P_COMMON_REFINED_STEP ����� �߰���

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_REFINED_STEP );

		if( NULL != pSkillTemplet )
		{
			m_iPhotonBoosterAdditionalDashJumpCount = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_AIR_MOVE_COUNT, iSkillLevel );
		}
	}

	m_fElectronBallRangeRate = 1.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_ELECTRONBALL_UP, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_ELECTRONBALL_UP );
		if( NULL != pSkillTemplet )
		{
			m_fElectronBallRangeRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel );
		}
	}

	m_EveElectraSystem.m_fLaserRangeScale = 1.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEL_SPECTRUM_DYNAMICS, true );	// SI_P_EPR_SPECTRUM_DYNAMICS �б� ����

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EEL_SPECTRUM_DYNAMICS );
		if( NULL != pSkillTemplet )
		{
			float fRangeScale = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel );
			m_fElectronBallRangeRate *= fRangeScale;
			m_EveElectraSystem.m_fLaserRangeScale *= fRangeScale;
		}
		if( iSkillLevel >= 1 )
			m_EveElectraSystem.SetEnableSpectrumDynamics( true );
	}



	m_fMPConsumeRateElectronBall = 1.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EN_QUEENS_POTENTIAL );
		if( NULL != pSkillTemplet )
		{
			// �� ���� : -0.01~ -.03
			float fMPConsumeRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevel );
			m_fMPConsumeRateElectronBall *= 1.f + fMPConsumeRel;
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR, true ); // SI_P_EPR_ADVANCED_EL_ENERGY_REACTOR

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR );

		if( NULL != pSkillTemplet )
		{
			float fAddMpChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_REGENERATION_ABS, iSkillLevel );

		//{{ mauntain : ����ȯ [2012.07.26] Ŭ��ŷ �޸� ������ ��ٸ� ���� ���� ( �۾��� : ���ҵ� �븸 ������ )
			SetAdditionalMPChangeRateByPassive( fAddMpChangeRate );
			ResetMPChangeRate( GetOriginalMPChangeRate());
		//}}
		}
	}


	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EAT_OBERON_GUARD );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_EAT_OBERON_GUARD  );

		if( NULL != pSkillTemplet )
		{
			m_sATOveronGuardData.SetProjectileSize( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillLevel ) );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO12 ) == true )
				m_sATOveronGuardData.SetEffectiveTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel ) / 2.f );
			else
				m_sATOveronGuardData.SetEffectiveTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel ) );
		}
	}


	// oasis907 : ����� [2010.1.29] //  �������� �Ϸ�Ʈ�к� ������ �нú�
	m_fAttackPowerRateElectronBall = 1.f;
	m_fForceDownValueRateElectronBall = 1.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEP_ELECTRONBALL_MASTER, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EEP_ELECTRONBALL_MASTER );
		if( NULL != pSkillTemplet )
		{
			m_fAttackPowerRateElectronBall *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
			m_fForceDownValueRateElectronBall *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_RANGE_REL, iSkillLevel );
		}
	}

	//{{ kimhc // 2010.11.2 // �������� - ���� ������
	const CX2SkillTree::SkillTemplet* pSkillTempletChargingBooster = userSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER );

	if ( NULL != pSkillTempletChargingBooster )
	{
        if ( !m_ChargingBoosterSkillDataBasePtr )
		{
			const int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER, true );

			m_ChargingBoosterSkillDataBasePtr = CSkillDataBase::CSkillCreateSkillDataBasePtr( CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER,
				pSkillTempletChargingBooster->GetSkillCoolTimeValue( iSkillLevel ) );
		}

		// ó�� ���۽� �ٷ� ����� �� �ֵ��� ��Ÿ�� �ð��� �־���
		m_ChargingBoosterSkillDataBasePtr->SetCoolTime( 15.0f );
		m_ChargingBoosterSkillDataBasePtr->SetTimerBasedOnCoolTime( m_ChargingBoosterSkillDataBasePtr->GetCoolTime() );
	}
	//}} kimhc // 2010.11.2 // �������� - ���� ������
//#pragma region ����� ����ȸ��
	const CX2SkillTree::SkillTemplet* pSkillTempletHighpoweredElectrics = userSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_EBS_HIGHPOWERED_ELECTRICS );

	if ( NULL != pSkillTempletHighpoweredElectrics )
	{
		m_bHighPoweredElectrics = true;
		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EBS_HIGHPOWERED_ELECTRICS, true ) );	/// ��ų ����

		m_fHighPoweredElectricsSpeed = pSkillTempletHighpoweredElectrics->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillTempletLevel );
	}
//#pragma endregion

//#pragma region �ʱ��� ����
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EBS_HYPER_OPTICAL_RESEARCH, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EBS_HYPER_OPTICAL_RESEARCH );
		if( NULL != pSkillTemplet )
		{
			float fScaleRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS, iSkillLevel );
			m_EveElectraSystem.m_fHyperOpticalResearchScale = fScaleRate;
		}

		if( iSkillLevel >= 1 )
			m_EveElectraSystem.SetEnableHyperOpticalResearch( true );
	}
//#pragma endregion

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION, true ); //������ ���

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION );
		if( NULL != pSkillTemplet )
		{
			const float fCriticalRateValueAtkBase = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ATK_BASE, iSkillLevel );
			const float fMixAtk = (GetStat().GetStat().m_fAtkMagic + GetStat().GetStat().m_fAtkPhysic) / 2;

			float fPassiveCriticalValue = fMixAtk * fCriticalRateValueAtkBase;
			m_AdditionalCriticalRate.SetOptionValue( m_AdditionalCriticalRate.GetOptionValue() + fPassiveCriticalValue );
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EBS_AMPLIFIED_EL_ENERGY_REACTOR, true ); // ���� �� ������ ������
	m_fAddSpectrumTime = m_fAddEnergeticHeartTime = 0.f;
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EBS_AMPLIFIED_EL_ENERGY_REACTOR );
		if( NULL != pSkillTemplet )
		{
			const float fCriticalRateValueAtkBase = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ATK_BASE, iSkillLevel );
			const float fMixAtk = (GetStat().GetStat().m_fAtkMagic + GetStat().GetStat().m_fAtkPhysic) / 2;

			float fPassiveCriticalValue = fMixAtk * fCriticalRateValueAtkBase;
			m_AdditionalCriticalRate.SetOptionValue( m_AdditionalCriticalRate.GetOptionValue() + fPassiveCriticalValue );

			float fAddMpChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_REGENERATION_ABS, iSkillLevel );
			SetAdditionalMPChangeRateByPassive( fAddMpChangeRate );
			ResetMPChangeRate( GetOriginalMPChangeRate());

			m_fAddSpectrumTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel );
			m_fAddEnergeticHeartTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME_HYPER, iSkillLevel );
		}
	}
}


/*static*/
void	CX2GUEve::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUEve::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUEve::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForBowToDeviceList( listInOut_, eDamageType_ );
}


void CX2GUEve::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();
}




/*virtual*/ void CX2GUEve::InitEnchantWeaponEffectUnitSpecific()
{
	for( UINT i=0; i<m_vecpWeapon.size(); ++i )
	{
		Weapon* pWeapon = m_vecpWeapon[i];
		pWeapon->InitEnchantParticleForBow();
	}
}

void CX2GUEve::DeleteEveMajorParticle()
{
	CX2GUUser::DeleteGUUserMajorParticle();

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	for ( int index = 0; index < EVE_MAJOR_PII_END; index++ )
	{
		pMajorParticleSystem->DestroyInstanceHandle( m_ahEveMajorParticleInstance[index] );
	}
}

void CX2GUEve::DeleteEveMinorParticle()
{
	CX2GUUser::DeleteGUUserMinorParticle();


	for ( int index = 0; index < EVE_MINOR_PII_END; index++ )
	{
		m_ahEveMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

void CX2GUEve::InitializeEveMajorParticleArray()
{
	CX2GUUser::InitializeGUUserMajorParticleArray();

	for ( int index = 0; index < EVE_MAJOR_PII_END; index++ )
	{
		m_ahEveMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUEve::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMajorParticle() == NULL )
		return;

	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpact" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpactCore" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpactSlash" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpactRed" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpactRingRed" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpactCoreRed" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"DamageImpactSlashRed" );
}


/*virtual*/ void CX2GUEve::CreateMajorParticleArray()
{
	CX2GUUser::CreateMajorParticleArray();

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	// small damage Ÿ���� ���� �� ������ ��ƼŬ
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT,			L"DamageImpact",			2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_CORE,		L"DamageImpactCore",		2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_SLASH,		L"DamageImpactSlash",		2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	// AttackResult�� switch case���� default:�� ó���Ǵ� Ÿ�ݿ� ���� ��ƼŬ (����� small �̿��� ��ƼŬ)
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RED,			L"DamageImpactRed",		2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RING_RED,	L"DamageImpactRingRed",		1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_CORE_RED,	L"DamageImpactCoreRed",		1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_SLASH_RED,	L"DamageImpactSlashRed",	1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUEve::SetEveMajorParticleByEnum( EVE_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleEveMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), 0,0,0, 0, 0, iDrawCount_, 0 );

		SetHandleEveMajorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUEve::InitializeEveMinorParticleArray()
{
	CX2GUUser::InitializeGUUserMinorParticleArray();

	for ( int index = 0; index < EVE_MINOR_PII_END; index++ )
	{
		m_ahEveMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUEve::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}



/*virtual*/ void CX2GUEve::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUEve::SetEveMinorParticleByEnum( EVE_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleEveMinorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), 0,0,0, 0, 0, iDrawCount_, 0 );

		SetHandleEveMinorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleEveMinorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUEve::CommonStateStartFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStartFuture();
}

void CX2GUEve::CommonStateStart()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStart();

	m_fEffectStartTime = 0.f;
}

void CX2GUEve::CommonFrameMoveFuture()
{
	KTDXPROFILE();
	m_fReAttackZ1Time -= m_fElapsedTime;
	if( m_fReAttackZ1Time < 0.0f )
		m_fReAttackZ1Time = 0.0f;

	m_fReDashJumpXTime -= m_fElapsedTime;
	if( m_fReDashJumpXTime < 0.0f )
		m_fReDashJumpXTime = 0.0f;


	CX2GUUser::CommonFrameMoveFuture();
}

void CX2GUEve::CommonFrameMove()
{
	KTDXPROFILE();


	CX2GUUser::CommonFrameMove();




	// ��ȯ�� ���� ó��
	if( 0.f != m_fSummonedLifeTime )
	{
		if( m_timerSummoned.elapsed() > (double) m_fSummonedLifeTime )
		{
			m_fSummonedLifeTime = 0.f;
			SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
			SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );
		}
	}
	//{{ JHKang / ������ / 2010/11/22 / ����� ���� ���� �ð� ó��
	if ( 0.f != m_fAtomicShieldLifeTime )
	{
		if( m_timerAtomicShieldSummoned.elapsed() > (double) m_fAtomicShieldLifeTime )
		{
			m_fAtomicShieldLifeTime = 0.f;
			SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );
		}
	}
	//}} JHKang / ������ / 2010/11/22 / ����� ���� ���� �ð� ó��

	if( 0.f <= m_fFerdinandLifeTime )
	{
		if( m_timerFerdinandSummoned.elapsed() > static_cast<double>( m_fFerdinandLifeTime ) )
		{
			SetSelfDestructSummonedNPC( CX2UnitManager::NUI_SI_HA_FERDINAND );
		}
	}

	// �̺� ����ȭ Ư���� ó��
	if( true == m_bInvisibility )
	{
		if( GetNowHp() <= 0.f )
		{
			m_bInvisibility			= false;
			m_bAbsoluteInvisibility = false;
			m_fInvisibilityAlpha	= 1.f;
		}
		else
		{
			if( m_timerInvisibility.elapsed() > m_fInvisibilityTime || GetNowMp() < 1.f )
			{
				m_bInvisibility			= false;
				m_bAbsoluteInvisibility = false;

				SetAlphaObject( false );
				m_RenderParam.bAlphaBlend = false;

				m_RenderParam.outLineColor.a	= 1.f;
				m_RenderParam.color.a			= 1.f;
			}
			else
			{
				// ���� �Ҹ�, SA ������ ����
				UpNowMp(m_fDecraseInvisibilityMP * m_fElapsedTime);
			}
		}

		if( false == m_bInvisibility )	// ����ȭ�� Ǯ�ȴ�
		{

			// ���� �����̸� ���� ��ƼŬ�� �ٽ� ���̰� �Ѵ�.
			if( GetRemainHyperModeTime() > 0.f )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq_RFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRFoot );
				CKTDGParticleSystem::CParticleEventSequence* pSeq_LFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLFoot );
				CKTDGParticleSystem::CParticleEventSequence* pSeq_RArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRArm );
				CKTDGParticleSystem::CParticleEventSequence* pSeq_LArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLArm );

				if( pSeq_RFoot != NULL )
				{
					pSeq_RFoot->SetShowObject( true );
				}
				if( pSeq_LFoot != NULL )
				{
					pSeq_LFoot->SetShowObject( true );
				}
				if( pSeq_RArm != NULL )
				{
					pSeq_RArm->SetShowObject( true );
				}
				if( pSeq_LArm != NULL )
				{
					pSeq_LArm->SetShowObject( true );
				}
			}

			//{{ kimhc // 2010.6.16 // ���� ����� �ϴ� �Լ��� �����Ͽ� �Լ��� �����
			// ���� �Ӽ� ��æƮ ��ƼŬ�� �ٽ� ���̰� �Ѵ�
			SetShowEnchantWeaponParticle( true );
			//{{ kimhc // 2010.6.16 // ���� ����� �ϴ� �Լ��� �����Ͽ� �Լ��� �����


			// ��� ������ ��ƼŬ �Ⱥ��̰�
			for( CX2EqipPtr pEquip : m_ViewEqipList )
			{
				if( pEquip != NULL )
					pEquip->SetShowAttachedParticle( true );
			}

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO5 ) == true )
			{
				//{{ kimhc // 2010.6.14	// �巡���м�Ʈ ȿ�� ���� ���� ����
				// GetOriginalMPChangeRate() ���� �巡���м�Ʈ ȿ���� ���� ���� �������� �Ǵ���
				ResetMPChangeRate( GetOriginalMPChangeRate() );
				//}} kimhc // 2010.6.14 // �巡���м�Ʈ ȿ�� ���� ���� ����
			}

			RemoveVecMoveSpeedFactor( CX2GameUnit::SFI_SPEED_13 );
		}
	}

	if( false == m_bInvisibility && 1.f != m_fInvisibilityAlpha )
	{
		m_fInvisibilityAlpha += 0.05f;

		if( m_fInvisibilityAlpha > 1.f )
		{
			m_fInvisibilityAlpha = 1.f;

			if ( false == IsMyUnit() && false == IsNullGageUI() )
				m_pGageUI->SetAlpha( 255.0f ); // �������ٸ� �ٽ� ���̰� �Ѵ�.
		}
	}

	if( true == m_bInvisibility )
	{
		{
			if( true == GetStateparam(false).bAttackState ||
                false == m_vecAttackTime.empty()
                )				// ���ݻ����̸�
			{
				m_bAbsoluteInvisibility = false;				// �̺긦 ���̰� �ϰ�
				m_fInvisibilityTime = 0.f;
			}
			else
			{
				m_bAbsoluteInvisibility = true;
			}
		}
	}





	if( m_sEGQueensThrone.m_bEnable == true )
	{
		if( m_sEGQueensThrone.m_dMaxLifeTime >= m_sEGQueensThrone.m_TimerElapsedTime.elapsed() )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != m_sEGQueensThrone.m_ahMajorMeshInstance[0] )
			{
				CKTDXDeviceXSkinMesh::MultiAnimFrame*	pMultiAnimFrame	= m_pXSkinAnim->GetCloneFrame( L"Bip01_Spine1" );
				CKTDGXMeshPlayer::CXMeshInstance*		pMeshInst = NULL;
				D3DXVECTOR3	vBonePos = GetBonePos(L"Bip01_Spine1");

				for( int i = 0; i < std::ssize(m_sEGQueensThrone.m_ahMajorMeshInstance); ++i )
				{
					pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_sEGQueensThrone.m_ahMajorMeshInstance[i] );
					if( NULL != pMeshInst && NULL != pMultiAnimFrame )
					{
						pMeshInst->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );	//ĳ���� ���ۿ� ���߱� ����..
						pMeshInst->SetPos( vBonePos );	//�ø�� ���Ǵ� ��ɿ� ������ �Ȱɸ��� �ϱ�����..
					}
				}

				// 3�� �������� ���� ����Ʈ�� �ٿ�����.(�̷��� �������ؼ� ���̸� �Ф��� �������� �� �ʿ�.)
				// 5���� ��ƼŬ�� ����ϸ� ��ƼŬ���� ���Ǵ�
				// Eve_EG_StateMsg01~5.Y : ���� 580byte
				// Eve_StateMsg01.tga : �������� ���Ǵ� �ؽ��� 64Kbyte
				if( m_TimerEGQTRefreshMsg.elapsed() > 0.6 )
				{
					m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_BODY] = INVALID_PARTICLE_SEQUENCE_HANDLE;
					WCHAR wstrParName[MAX_PATH] = L"";

					StringCchPrintf(wstrParName,MAX_PATH, L"Eve_EG_Sp3a_QT_StateMsg0%d", (rand()%5 + 1) );
					//wsprintf(wstrParName, L"Eve_EG_Sp3a_QT_StateMsg0%d", (rand()%5 + 1) );

					m_sEGQueensThrone.m_iSelectBoneNameID = rand()%5;

					CKTDGParticleSystem::CParticleEventSequence* pSeq =
						g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  wstrParName, GetBonePos( m_sEGQueensThrone.m_wstrBoneName[m_sEGQueensThrone.GetBoneNameID()].c_str() ) );
					if( NULL != pSeq )
					{
						m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_BODY] = pSeq->GetHandle();
					}

					m_TimerEGQTRefreshMsg.restart();
				}
				else
				{
					//��ƼŬ ��ġ ������Ʈ.
					if( m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_BODY] != INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_BODY] );
						if( NULL != pSeq )
						{
							pSeq->SetPosition(GetBonePos(m_sEGQueensThrone.m_wstrBoneName[m_sEGQueensThrone.GetBoneNameID()].c_str()));
						}
					}
					if( m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_WING] != INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_sEGQueensThrone.m_ahMajorMeshInstance[2] );
						if( pMeshInst != NULL )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_WING] );
							if( NULL != pSeq )
							{
								pSeq->SetPosition( pMeshInst->GetBonePos(L"Dummy01") );
							}
						}
					}
				}
			}
		}
		else
		{
			m_sEGQueensThrone.Reset();

			D3DXVECTOR3 vBonePos = GetBonePos(L"Bip01_Spine1");
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"ShockWave_Eve_EG_Sp3a_QT_End02", vBonePos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_EG_Sp3a_QT_End01", vBonePos );

			UpDownCrashCameraNoReset();

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_QT_END_RING", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );

			PlaySound( L"Eve_EG_SP3_QueensThrone_Disappear.ogg" );
		}
	}

	// �ް��Ϸ�Ʈ�к� ������� ����Ʈ ó��..
	if( m_sMegaElectronBallData.m_bEnable == true )
	{
		// �������� ������ �ٲ�� ����..
		if( m_sMegaElectronBallData.GetSkillSlotSwap() != g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() )
			m_sMegaElectronBallData.Reset( false, &AccessFutureFrameData() );
		else
		{


			//��������� ����Ʈ ������..
			if( GetNowHp() > 0 )
			{
				// ������ ����Ʈ.
				if( m_sMegaElectronBallData.m_dMaxTime > m_sMegaElectronBallData.m_timerElapsedTime.elapsed() )
				{
					if( m_sMegaElectronBallData.m_ahMajorParticleInstance[0] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.x += 5.f;
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Hole_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[0] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[0] );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.x += 5.f;
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
					if( m_sMegaElectronBallData.m_ahMajorParticleInstance[1] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[1] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[1] );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
					if( m_sMegaElectronBallData.m_ahMajorParticleInstance[2] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[2] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[2] );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
				}
				//���� �ϼ� ����Ʈ�� ��ȯ..
				else if( m_sMegaElectronBallData.m_dMaxTime <= m_sMegaElectronBallData.m_timerElapsedTime.elapsed() )
				{
					static bool bIsFirstSucCharge = false;

					// ������ �Ϸ�� ù�����̸�.
					if( m_sMegaElectronBallData.m_bIsSucCharge == false )
					{
						m_sMegaElectronBallData.m_bIsSucCharge = true;
						m_sMegaElectronBallData.ResetParticle();

						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );

						// �Ϸ�����Ʈ�� �����ְ�..
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[0] = pSeq->GetHandle();
						}
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[1] = pSeq->GetHandle();
						}

						// �Ϸ�� ���� ����Ʈ�� �����Ͽ� ��ȯ.
						vBonePos = GetBonePos( L"Bip01_R_Finger2" );

						vBonePos.y += 5.f;
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[2] = pSeq->GetHandle();
						}
						vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_MEB_FullCharge01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sMegaElectronBallData.m_ahMajorParticleInstance[3] = pSeq->GetHandle();
						}

						if( IsMyUnit() == true )
						{
							g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
							PlaySound( L"Eve_MEB_Condense_power_charge.ogg" );
						}
						bIsFirstSucCharge = true;

						PlaySoundAtNoAttackState( L"EveVoice_MegaElectronBall.ogg" );
					}
					else
					{
						if( m_sMegaElectronBallData.m_ahMajorParticleInstance[0] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[0] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sMegaElectronBallData.m_ahMajorParticleInstance[1] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[1] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sMegaElectronBallData.m_ahMajorParticleInstance[2] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[2] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								vBonePos.y += 5.f;
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sMegaElectronBallData.m_ahMajorParticleInstance[3] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[3] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}

						if(bIsFirstSucCharge == true)
						{
							bIsFirstSucCharge = false;
							if( IsMyUnit() == true )
								g_pKTDXApp->GetDeviceManager()->PlaySound( L"Eve_MEB_FullCharge.ogg", true, false );
						}
					}
				}
			}
			else
				//��ĳ��Ƽ �׾����� ���� ����..
				m_sMegaElectronBallData.Reset( false, &AccessFutureFrameData() );
		}
		for(int iParticle = 0; iParticle < 4; ++iParticle)
		{
			if( m_sMegaElectronBallData.m_ahMajorParticleInstance[iParticle] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sMegaElectronBallData.m_ahMajorParticleInstance[iParticle] );
				if( pSeq != NULL && m_bInvisibility == true )
				{
					pSeq->SetShowObject(false);
				}
				else if( pSeq != NULL )
				{
					pSeq->SetShowObject(true);
				}
			}
		}
	}

	if( m_sATOveronGuardData.GetEnable() == true && m_sATOveronGuardData.IsLive() == false )
	{
		m_sATOveronGuardData.SetEnable(false);

		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] );
		m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] = INVALID_MESH_INSTANCE_HANDLE;

		g_pX2Game->GetEffectSet()->StopEffectSet(m_hOberonGuard);
		m_hOberonGuard = INVALID_EFFECTSET_HANDLE;
	}

	//{{ kimhc // 2010.11.9 // �������� - ���� ������
	// ���� �����⸦ �����, ���� ��Ÿ�� ���̸�
	if (
        m_ChargingBoosterSkillDataBasePtr
		&& false == m_ChargingBoosterSkillDataBasePtr->IsEndCoolTime() )
		m_ChargingBoosterSkillDataBasePtr->OnFrameMove( m_fElapsedTime );
	//}} kimhc // 2010.11.9 // �������� - ���� ������

    m_EveElectraSystem.ProcessSystem( m_fElapsedTime );
	DoFrameMoveThousandStar();

	// ������ �ʶ� ������� ����Ʈ ó��..
	if( m_sTaserPilumData.m_bEnable == true )
	{
		// �������� ������ �ٲ�� ����..
		if( m_sTaserPilumData.GetSkillSlotSwap() != g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() )
			m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
		else
		{
			//��������� ����Ʈ ������..
			if( GetNowHp() > 0 )
			{
				// ������ ����Ʈ.
				if( m_sTaserPilumData.m_dMaxTime > m_sTaserPilumData.m_timerElapsedTime.elapsed() )
				{
					if( m_sTaserPilumData.m_ahMajorParticleInstance[0] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.x += 5.f;
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Hole_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[0] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[0] );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.x += 5.f;
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
					if( m_sTaserPilumData.m_ahMajorParticleInstance[1] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[1] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[1] );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
					if( m_sTaserPilumData.m_ahMajorParticleInstance[2] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[2] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[2] );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
				}
				//���� �ϼ� ����Ʈ�� ��ȯ..
				else if( m_sTaserPilumData.m_dMaxTime <= m_sTaserPilumData.m_timerElapsedTime.elapsed() )
				{
					static bool bIsFirstSucCharge = false;

					// ������ �Ϸ�� ù�����̸�.
					if( m_sTaserPilumData.m_bIsSucCharge == false )
					{
						m_sTaserPilumData.m_bIsSucCharge = true;
						m_sTaserPilumData.ResetParticle();

						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );

						// �Ϸ�����Ʈ�� �����ְ�..
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[0] = pSeq->GetHandle();
						}
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[1] = pSeq->GetHandle();
						}

						// �Ϸ�� ���� ����Ʈ�� �����Ͽ� ��ȯ.
						vBonePos = GetBonePos( L"Bip01_R_Finger2" );

						vBonePos.y += 5.f;
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[2] = pSeq->GetHandle();
						}
						vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EBS_A_TASER_PILUM_ready_P01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sTaserPilumData.m_ahMajorParticleInstance[3] = pSeq->GetHandle();
						}

						if( IsMyUnit() == true )
						{
							g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
							PlaySound( L"Eve_MEB_Condense_power_charge.ogg" );
						}
						bIsFirstSucCharge = true;

						PlaySoundAtNoAttackState( L"EveVoice_MegaElectronBall.ogg" );
					}
					else
					{
						if( m_sTaserPilumData.m_ahMajorParticleInstance[0] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[0] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sTaserPilumData.m_ahMajorParticleInstance[1] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[1] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sTaserPilumData.m_ahMajorParticleInstance[2] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[2] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								vBonePos.y += 5.f;
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sTaserPilumData.m_ahMajorParticleInstance[3] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[3] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}

						if(bIsFirstSucCharge == true)
						{
							bIsFirstSucCharge = false;
							if( IsMyUnit() == true )
								g_pKTDXApp->GetDeviceManager()->PlaySound( L"Eve_MEB_FullCharge.ogg", true, false );
						}
					}
				}
			}
			else
				//��ĳ��Ƽ �׾����� ���� ����..
				m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
		}

		for(int iParticle = 0; iParticle < 4; ++iParticle)
		{
			if( m_sTaserPilumData.m_ahMajorParticleInstance[iParticle] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sTaserPilumData.m_ahMajorParticleInstance[iParticle] );
				if( pSeq != NULL && m_bInvisibility == true )
				{
					pSeq->SetShowObject(false);
				}
				else if( pSeq != NULL )
				{
					pSeq->SetShowObject(true);
				}
			}
		}

// 		if( m_bInvisibility == true )
// 		{
// 			if( m_GuildSkillData.m_ChangeToReverse.m_fTimeLeft > 0.f )
// 			{
// 				if( m_GuildSkillData.m_ChangeToReverse.m_hEffectSet != INVALID_EFFECTSET_HANDLE )
// 				{
// 					g_pX2Game->GetEffectSet()->StopEffectSet( m_GuildSkillData.m_ChangeToReverse.m_hEffectSet );
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if( m_GuildSkillData.m_ChangeToReverse.m_fTimeLeft > 0.f )
// 			{
// 				if ( m_GuildSkillData.m_ChangeToReverse.m_hEffectSet == INVALID_EFFECTSET_HANDLE )
// 				{
// 					m_GuildSkillData.m_ChangeToReverse.m_hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ChanceToReverse", this, this );
// 				}
// 			}
// 		}
	}

	// ���ۺ����� ������� ����Ʈ ó��..
	if( m_sKugelBlitz_Data.m_bEnable == true )
	{
		// �������� ������ �ٲ�� ����..
		if( m_sKugelBlitz_Data.GetSkillSlotSwap() != g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() )
			m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
		else
		{
			//��������� ����Ʈ ������..
			if( GetNowHp() > 0 )
			{
				// ������ ����Ʈ.
				if( m_sKugelBlitz_Data.m_dMaxTime > m_sKugelBlitz_Data.m_timerElapsedTime.elapsed() )
				{
					if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[0] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.x += 5.f;
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence(
							(CKTDGObject*) this,  L"Light_Hole_Eve_SI_A_EN_Condense_Power01", vBonePos );

						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[0] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence(
							m_sKugelBlitz_Data.m_ahMajorParticleInstance[0] );

						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.x += 5.f;
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
					if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[1] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[1] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence(
							m_sKugelBlitz_Data.m_ahMajorParticleInstance[1] );

						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
					if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[2] == INVALID_PARTICLE_SEQUENCE_HANDLE )
					{
						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						vBonePos.y += 5.f;
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence(
							(CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power02", vBonePos );

						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[2] = pSeq->GetHandle();
						}
					}
					else
					{
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence(
							m_sKugelBlitz_Data.m_ahMajorParticleInstance[2] );

						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );
							D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
							vBonePos.y += 5.f;
							pSeq->SetPosition( vBonePos );
						}
					}
				}
				//���� �ϼ� ����Ʈ�� ��ȯ..
				else if( m_sKugelBlitz_Data.m_dMaxTime <= m_sKugelBlitz_Data.m_timerElapsedTime.elapsed() )
				{
					static bool bIsFirstSucCharge = false;

					// ������ �Ϸ�� ù�����̸�.
					if( m_sKugelBlitz_Data.m_bIsSucCharge == false )
					{
						m_sKugelBlitz_Data.m_bIsSucCharge = true;
						m_sKugelBlitz_Data.ResetParticle();

						D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );

						// �Ϸ�����Ʈ�� �����ְ�..
						CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_Condense_Power01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[0] = pSeq->GetHandle();
						}
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[1] = pSeq->GetHandle();
						}

						// �Ϸ�� ���� ����Ʈ�� �����Ͽ� ��ȯ.
						vBonePos = GetBonePos( L"Bip01_R_Finger2" );

						vBonePos.y += 5.f;
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_SI_A_EN_Condense_Power02", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[2] = pSeq->GetHandle();
						}
						vBonePos = GetBonePos( L"Bip01_R_Finger2" );
						pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EBS_A_TASER_PILUM_ready_P01", vBonePos );
						if( NULL != pSeq )
						{
							pSeq->SetAddRotate( GetRotateDegree() );
							pSeq->SetAxisAngle( GetRotateDegree() );

							m_sKugelBlitz_Data.m_ahMajorParticleInstance[3] = pSeq->GetHandle();
						}

						if( IsMyUnit() == true )
						{
							g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
							PlaySound( L"Eve_MEB_Condense_power_charge.ogg" );
						}
						bIsFirstSucCharge = true;

						PlaySoundAtNoAttackState( L"EveVoice_MegaElectronBall.ogg" );
					}
					else
					{
						if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[0] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sKugelBlitz_Data.m_ahMajorParticleInstance[0] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[1] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sKugelBlitz_Data.m_ahMajorParticleInstance[1] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[2] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sKugelBlitz_Data.m_ahMajorParticleInstance[2] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								vBonePos.y += 5.f;
								pSeq->SetPosition( vBonePos );
							}
						}
						if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[3] != INVALID_PARTICLE_SEQUENCE_HANDLE )
						{
							CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sKugelBlitz_Data.m_ahMajorParticleInstance[3] );
							if( NULL != pSeq )
							{
								pSeq->SetAddRotate( GetRotateDegree() );
								pSeq->SetAxisAngle( GetRotateDegree() );
								D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
								pSeq->SetPosition( vBonePos );
							}
						}

						if(bIsFirstSucCharge == true)
						{
							bIsFirstSucCharge = false;
							if( IsMyUnit() == true )
								g_pKTDXApp->GetDeviceManager()->PlaySound( L"Eve_MEB_FullCharge.ogg", true, false );
						}
					}
				}
			}
			else
				//��ĳ��Ƽ �׾����� ���� ����..
				m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
		}

		for(int iParticle = 0; iParticle < 4; ++iParticle)
		{
			if( m_sKugelBlitz_Data.m_ahMajorParticleInstance[iParticle] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_sKugelBlitz_Data.m_ahMajorParticleInstance[iParticle] );
				if( pSeq != NULL && m_bInvisibility == true )
				{
					pSeq->SetShowObject(false);
				}
				else if( pSeq != NULL )
				{
					pSeq->SetShowObject(true);
				}
			}
		}
	}

	m_EnergyNiddleData.OnFrameMove( this, m_fElapsedTime );

	OnFrameMove_EndElCrystalSystem();


}


/*virtual*/ void CX2GUEve::CommonEventProcess()
{
	KTDXPROFILE();
	CX2GUUser::CommonEventProcess();

	if( m_sMegaElectronBallData.m_bEnable == false )
	{
		m_sMegaElectronBallData.SetSkillSlotSwap( g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() );	//���� ���õ� ������ ����.

		// ���� ���Կ� ��ų�� �ִ��� �˰�..
		if( m_sMegaElectronBallData.GetSkillSlotID() >= 0 && m_sMegaElectronBallData.GetSkillSlotID() < EQUIPPED_SKILL_SLOT_COUNT*2 )
		{
			//�ް��Ϸ�Ʈ�к� ������Ʈ ���̵� ã�Ƽ�.
			int iNowState = -1;
			if( GetRemainHyperModeTime() > 0.f )
			{
				iNowState = _CONST_GUUSER_::aHyperSkillStateID[m_sMegaElectronBallData.GetSkillSlotID()];
				if( iNowState == -1 ) //A��ų���Կ��� ã�� ���ߴٸ�
					iNowState = _CONST_GUUSER_::aHyperSkillStateIDSlotB[m_sMegaElectronBallData.GetSkillSlotID()];
			}
			else
			{
				iNowState = _CONST_GUUSER_::aNormalSkillStateID[m_sMegaElectronBallData.GetSkillSlotID()];
				if( iNowState == -1 ) //A��ų���Կ��� ã�� ���ߴٸ�
					iNowState = _CONST_GUUSER_::aNormalSkillStateIDSlotB[m_sMegaElectronBallData.GetSkillSlotID()];
			}

			//�ް��Ϸ�Ʈ�к� ��ų Ű�� ���������� üũ..
			bool bIsKeyPress = false;
			switch( m_sMegaElectronBallData.GetSkillSlotID() )
			{
			case 0: bIsKeyPress = m_InputData.oneA; break;
			case 1: bIsKeyPress = m_InputData.oneS; break;
			case 2: bIsKeyPress = m_InputData.oneD; break;
			case 3: bIsKeyPress = m_InputData.oneC; break;
			case 4: bIsKeyPress = m_InputData.oneQ; break;
			case 5: bIsKeyPress = m_InputData.oneW; break;
			case 6: bIsKeyPress = m_InputData.oneE; break;
			case 7: bIsKeyPress = m_InputData.oneR; break;
			default :
				break;
			}

			//Ű�� üũ �Ǿ����� ����..
			if( bIsKeyPress == true )
			{
				m_FrameDataFuture.syncData.nowAction = UAI_MEGA_ELECTRONBALL;
			}
		}
	}

	if( m_sTaserPilumData.m_bEnable == false )
	{
		// ������ �ʶ� ������Ʈ ���̵� ã�Ƽ�.

		m_sTaserPilumData.SetSkillSlotSwap( g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() );	//���� ���õ� ������ ����.

		int iSlotIndex = 0;
		bool bSlotB = false;
		CX2UserSkillTree::GetSlotIndexAndSlotB( m_sTaserPilumData.GetSkillSlotID(), OUT iSlotIndex, OUT bSlotB );

		[[maybe_unused]] int iNowState = -1;

		if ( ARRAY_SIZE( _CONST_GUUSER_::aNormalSkillStateID ) > iSlotIndex && 0 < iSlotIndex )
		{
			if( GetRemainHyperModeTime() > 0.f )
			{
				if( bSlotB == true )
					iNowState = _CONST_GUUSER_::aHyperSkillStateIDSlotB[iSlotIndex];
				else
					iNowState = _CONST_GUUSER_::aHyperSkillStateID[iSlotIndex];
			}
			else
			{
				if( bSlotB == true )
					iNowState = _CONST_GUUSER_::aNormalSkillStateIDSlotB[iSlotIndex];
				else
					iNowState = _CONST_GUUSER_::aNormalSkillStateID[iSlotIndex];
			}
		}

		// ������ �ʶ� ��ų Ű�� ���������� üũ..
		bool bIsKeyPress = false;
		switch( m_sTaserPilumData.GetSkillSlotID() )
		{
		case 0: bIsKeyPress = m_InputData.oneA; break;
		case 1: bIsKeyPress = m_InputData.oneS; break;
		case 2: bIsKeyPress = m_InputData.oneD; break;
		case 3: bIsKeyPress = m_InputData.oneC; break;
		case 4: bIsKeyPress = m_InputData.oneQ; break;
		case 5: bIsKeyPress = m_InputData.oneW; break;
		case 6: bIsKeyPress = m_InputData.oneE; break;
		case 7: bIsKeyPress = m_InputData.oneR; break;
		default :
			break;
		}

		//Ű�� üũ �Ǿ����� ����..
		if( bIsKeyPress == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_TASER_PILUM;
		}
	}

	if( m_sKugelBlitz_Data.m_bEnable == false )
	{
		// ������ �ʶ� ������Ʈ ���̵� ã�Ƽ�.

		m_sKugelBlitz_Data.SetSkillSlotSwap( g_pData->GetUIManager()->GetUISkillTree()->GetSkillSlotSwap() );	//���� ���õ� ������ ����.

		int iSlotIndex = 0;
		bool bSlotB = false;
		CX2UserSkillTree::GetSlotIndexAndSlotB( m_sKugelBlitz_Data.GetSkillSlotID(), OUT iSlotIndex, OUT bSlotB );

		[[maybe_unused]] int iNowState = -1;

		if ( ARRAY_SIZE( _CONST_GUUSER_::aNormalSkillStateID ) > iSlotIndex && 0 < iSlotIndex )
		{
			if( GetRemainHyperModeTime() > 0.f )
			{
				if( bSlotB == true )
					iNowState = _CONST_GUUSER_::aHyperSkillStateIDSlotB[iSlotIndex];
				else
					iNowState = _CONST_GUUSER_::aHyperSkillStateID[iSlotIndex];
			}
			else
			{
				if( bSlotB == true )
					iNowState = _CONST_GUUSER_::aNormalSkillStateIDSlotB[iSlotIndex];
				else
					iNowState = _CONST_GUUSER_::aNormalSkillStateID[iSlotIndex];
			}
		}

		// ���ۺ����� ��ų Ű�� ���������� üũ..
		bool bIsKeyPress = false;
		switch( m_sKugelBlitz_Data.GetSkillSlotID() )
		{
		case 0: bIsKeyPress = m_InputData.oneA; break;
		case 1: bIsKeyPress = m_InputData.oneS; break;
		case 2: bIsKeyPress = m_InputData.oneD; break;
		case 3: bIsKeyPress = m_InputData.oneC; break;
		case 4: bIsKeyPress = m_InputData.oneQ; break;
		case 5: bIsKeyPress = m_InputData.oneW; break;
		case 6: bIsKeyPress = m_InputData.oneE; break;
		case 7: bIsKeyPress = m_InputData.oneR; break;
		default :
			break;
		}

		//Ű�� üũ �Ǿ����� ����..
		if( bIsKeyPress == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_KUGELBLITZ;
		}
	}
}


void CX2GUEve::CommonStateEndFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEndFuture();
}

void CX2GUEve::CommonStateEnd()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEnd();
	m_fEffectStartTime = 0.f;

	m_bIsCheckEndElCrystal = true;
}

//{{ robobeg : 2008-10-28
//void CX2GUEve::CommonRender()
/*virtual*/
RENDER_HINT CX2GUEve::CommonRender_Prepare()
	//}} robobeg : 2008-10-28
{
	KTDXPROFILE();
	// 	if( GetRemainHyperModeTime() > 0.0f )
	// 	{
	// 		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
	// 		m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_RED;
	// 		m_RenderParam.outLineColor		= D3DXCOLOR( 1.f, 0.694f, 0.737f, 1.f );
	// 	}
	// 	else
	// 	{
	// 		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
	// 		m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
	// 		m_RenderParam.outLineColor		= 0xffffffff;
	// 	}

	//{{ robobeg : 2008-10-28
	//CX2GUUser::CommonRender();
	return CX2GUUser::CommonRender_Prepare();
	//}} robobeg : 2008-10-28
}

/*virtual*/ bool CX2GUEve::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_ONLY_ON_LINE*/ )
{
	KTDXPROFILE();

	const int INVALID_SKILL_SLOT_INDEX = -1;

	int iPressedSkillSlotIndex = INVALID_SKILL_SLOT_INDEX;
	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = NULL;
	CX2UserSkillTree& cUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;	// ������ ��� ��ų Ʈ��

	if ( false == CommonSpecialAttackEventProcess( cUserSkillTree, pSkillSlotData, iPressedSkillSlotIndex ) )
		return false;

	bool bIsUpSkillSlotMegaElectronBall = IsUpSkillSlotMegaElectronBall();

	//�ް��Ϸ�Ʈ�к� �������� Ű�� ���ȴ��� üũ..
	if ( true == bIsUpSkillSlotMegaElectronBall )
	{
		pSkillSlotData = GetSkillSlotDataMegaElectronBall( cUserSkillTree, iPressedSkillSlotIndex );
	}

	if ( NULL != pSkillSlotData && pSkillSlotData->m_eID == CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL && false == bIsUpSkillSlotMegaElectronBall )
	{
		//�ް��Ϸ�Ʈ�к� ���� enable ���� �ʾƵ� ùŸ�ӿ� �Ѿ���� ��찡 �־ �ܼ�����ó��.
		return false;
	}

	bool bIsUpSkillSlotTaserPilum = IsUpSkillSlotTaserPilum();

	// ������ �ʶ� �������� Ű�� ���ȴ��� üũ..
	if ( true == bIsUpSkillSlotTaserPilum )
	{
		pSkillSlotData = GetSkillSlotDataTaserPilum( cUserSkillTree, iPressedSkillSlotIndex );
	}

	if ( NULL != pSkillSlotData && pSkillSlotData->m_eID == CX2SkillTree::SI_A_EBS_TASER_PILUM && false == bIsUpSkillSlotTaserPilum )
	{
		return false;
	}

	bool bIsUpSkillSlotKugelBlitz = IsUpSkillSlotKugelBlitz();

	// ���ۺ����� �������� Ű�� ���ȴ��� üũ..
	if ( true == bIsUpSkillSlotKugelBlitz )
	{
		pSkillSlotData = GetSkillSlotDataKugelBlitz( cUserSkillTree, iPressedSkillSlotIndex );
	}

	if ( NULL != pSkillSlotData && pSkillSlotData->m_eID == CX2SkillTree::SI_A_EBS_KUGELBLITZ && false == bIsUpSkillSlotKugelBlitz )
	{
		return false;
	}

	if( NULL == pSkillSlotData )
		return false;

	const CX2SkillTree::SkillTemplet* pSkillTempletUsing = GetEquippedActiveSkillTemplet( pSkillSlotData->m_eID );
	if( NULL == pSkillTempletUsing )
		return false;

	if( false == CheckSkillUseCondition( eActiveSkillUseCondition, pSkillTempletUsing->m_eActiveSkillUseCondtion ) )
		return false;

	//��ȯ �ʻ�� ������ ������ ���� ���¿��� ����� �� ����.
	if( false == CheckSummonSpecialAttack( pSkillTempletUsing->m_eID ) )
		return false;

	// ���Ĵ������� ��� ������ ��ų���� �˻�
	if( false == CheckSkillUsePVPOfficial( pSkillTempletUsing->m_eID ) )
		return false;

	const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTempletUsing->m_eID ) );	/// ��ų ����

	float fMPConsume = GetActualMPConsume( pSkillTempletUsing->m_eID, iSkillTempletLevel );

	if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL && GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO13 ) == true )
	{
		fMPConsume -= 10.f;
		if( fMPConsume < 0.f )
			fMPConsume = 0.f;
	}

	if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL && m_EveElectraSystem.GetEnableSystem() == true )
	{
		fMPConsume	= 0.f;
	}
	if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS && m_iThousandStarCount > 0 )
	{
		fMPConsume	= 0.f;
	}

#if X2_DEV_TOOLS
	if( false == g_pMain->IsMyAuthLevelHigherThan( CX2User::XUAL_DEV ) )
#endif // _SERVICE_
	{
		if( pSkillSlotData->m_fCoolTimeLeft > 0.f )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_226 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}

		if ( GetNowMp() < fMPConsume )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}

		if ( pSkillTempletUsing->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME
			&& false == GetActiveLinkOverChargeIllusion() ) //�丣�𳭵� �ǴϽ� ���ݿ� ���� ����ó��)
		{
			const int iItemNum = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );

			if( iItemNum <= 0
				&& false == g_pData->GetMyUser()->GetSelectUnit()->IsInfinityElEssence()
				)
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_26119 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}
		}
	}

	if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_SA_EEG_QUEENS_THRONE )
	{
		//��Ÿ�Ӹ� ������ ���������� �����ϰ� �ٽ� ���ɼ� �ֵ��� �ϱ�����.
		m_sEGQueensThrone.Reset();
	}

	if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EN_CONVERSION )
	{//
		// ������ȯ �ϰ�� HP �� �Ҹ�� �纸�� ������ üũ.
		if( (GetMaxHp() * pSkillTempletUsing->GetSkillAbilityValue( CX2SkillTree::SA_HP_REL_TO_MP_ABS_1, iSkillTempletLevel ) ) >= GetNowHp() )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD,
				GET_STRING( STR_ID_2627 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );

			return false;
		}

		if( GetMaxMp() == GetNowMp() )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD,
				GET_STRING( STR_ID_2662 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );

			return false;
		}
	}

	/// ��ųâ���� ��ų �������̸�, ��ų ��� �Ұ�
	if (	NULL != g_pData &&
			NULL != g_pData->GetUIManager() &&
			NULL != g_pData->GetUIManager()->GetUISkillTree() &&
			true == g_pData->GetUIManager()->GetUISkillTree()->GetNowLearnSkill() )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_25110 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}


	//�ް��Ϸ�Ʈ�к��� ��ü������Ʈ���� ��´�.
	if( pSkillSlotData->m_eID != CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL
		&& pSkillSlotData->m_eID != CX2SkillTree::SI_A_EBS_TASER_PILUM
		&& pSkillSlotData->m_eID != CX2SkillTree::SI_A_EBS_KUGELBLITZ
		)
	{
		UpNowMp( -fMPConsume );
	}

	if ( pSkillTempletUsing->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME
		&& false == GetActiveLinkOverChargeIllusion() ) //�丣�𳭵� �ǴϽ� ���ݿ� ���� ����ó��
	{
		if ( true == g_pData->GetMyUser()->GetSelectUnit()->IsInfinityElEssence() )
			g_pX2Game->Handler_EGS_USE_FINALITY_SKILL_REQ();
		else
		{
		CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );

		if ( NULL != pItem )
			g_pX2Game->Handler_EGS_USE_FINALITY_SKILL_REQ( pItem->GetItemData().m_ItemUID );
		}
	}

	//{{ JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����
	CountUsedSkill( iPressedSkillSlotIndex );
	//}} JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����

	cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) );


		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO6 ) == true && pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EN_CONVERSION )
		{
			cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) + 3.f );

		}
		else if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO2 ) == true && pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EN_DIMENSION_LINK )
		{
			cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) + 5.f );

		}

		else if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO7 ) == true && pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL )
		{
			cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) - 1.f );

		}

		if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL )
		{
			cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, 0.f );
		}

		if ( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL && false == m_EveElectraSystem.GetEnableExceptionSystem()
			&& true != m_FrameDataNow.stateParam.bNoChangSpectro)
		{
			if(	m_EveElectraSystem.GetEnableSystem() == false && m_EveElectraSystem.GetEnableSpectrumDynamics() == true
				&& m_EveElectraSystem.GetEnableHyperOpticalResearch() == false )
			{
				cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, 0.f );

				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			else if( m_EveElectraSystem.GetEnableSystem() == false && m_EveElectraSystem.GetEnableSpectrumDynamics() == false
				&& m_EveElectraSystem.GetEnableHyperOpticalResearch() == false )
			{
				cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			else if (m_EveElectraSystem.GetEnableSystem() == true && m_EveElectraSystem.GetEnableSpectrumDynamics() == true
				&& m_EveElectraSystem.GetEnableHyperOpticalResearch() == false )
			{
				cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, 2.f );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			else if ( m_EveElectraSystem.GetEnableSystem() == false && m_EveElectraSystem.GetEnableHyperOpticalResearch() == true )
			{
				cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, 0.0f );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			else if ( m_EveElectraSystem.GetEnableSystem() == false && m_EveElectraSystem.GetEnableHyperOpticalResearch() == false )
			{
				cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			else if (  m_EveElectraSystem.GetEnableSystem() == true && m_EveElectraSystem.GetEnableHyperOpticalResearch() == true )
			{
				cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, 0.5f );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
		}
		else if ( pSkillTempletUsing->m_eID == CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS )
		{
			cUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, 0.f );

			g_pX2Game->UpdateSkillSlotUI();
		}

		if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION )
		{
			cUserSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION, 0.f  );
		}




		if( false == SpecialAttackNoStageChange( pSkillTempletUsing ) )
		{
			if( GetRemainHyperModeTime() <= 0.f )
			{
				if( iPressedSkillSlotIndex > 3 ) //���� B�϶�
				{
					StateChange( _CONST_GUUSER_::aNormalSkillStateIDSlotB[iPressedSkillSlotIndex-4] );
				}
				else
				{
					StateChange( _CONST_GUUSER_::aNormalSkillStateID[iPressedSkillSlotIndex] );
				}
			}
			else
			{
				if( iPressedSkillSlotIndex > 3 ) //���� B�϶�
				{
					StateChange( _CONST_GUUSER_::aHyperSkillStateIDSlotB[iPressedSkillSlotIndex-4] );
				}
				else
				{
					StateChange( _CONST_GUUSER_::aHyperSkillStateID[iPressedSkillSlotIndex] );
				}
			}
		}

		m_eSpecialAttackKeyPressed = (SPECIAL_ATTACK_KEY_PRESSED) (iPressedSkillSlotIndex + 1);

		m_iNowSpecialAttack = iPressedSkillSlotIndex + 1;
		m_bSpecialAttackEventProcessedAtThisFrame = true;

		//{{ kimhc // 2010.11.2 // �������� - ���� ������
		if ( true == CanUseChargingBooster( pSkillTempletUsing->m_eType, cUserSkillTree ) )
		{
			UseChargingBooster( cUserSkillTree );
			m_ChargingBoosterSkillDataBasePtr->ResetTimerBasedOnCoolTime();
		}
		//}} kimhc // 2010.11.2 // �������� - ���� ������

		Send_SKILL_USE_REQ( pSkillTempletUsing->m_eID );

		return true;

}





// ���º�ȭ�� ���� �ʻ��� �ϵ��ڵ��ؼ� ���� ó�� �Ѵ�
/*virtual*/ bool CX2GUEve::SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet )
{
	if ( pSkillTemplet == NULL )
		return false;

	//{{ kimhc // 2010.11.1 // �Ž�ų
	switch ( pSkillTemplet->m_eID )
	{
	case CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER:	// ���� - ���� ������
		{
			m_FrameDataFuture.syncData.nowAction = UAI_CHARGING_BOOSTER;
		} break;
	case CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS:
		{
			if( m_iThousandStarCount > 0 )
			{
				m_FrameDataFuture.syncData.nowAction = UAI_THOUSANDS_OF_STARS;
				return true;
			}
		} break;
	case CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL:
		{
			if ( true == m_EveElectraSystem.GetEnableSystem() && true == m_EveElectraSystem.GetEnableHyperOpticalResearch() )
			{
				m_FrameDataFuture.syncData.nowAction = UAI_CHANGE_SPECTRO_WITHOUT_MOTION;

				return true;
			}
		} break;
	case CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION:
		{
			if( true == GetActiveLinkOverChargeIllusion() )
			{
				m_FrameDataFuture.syncData.nowAction = UAI_EEP_LINK_OVERCHARGE_ILLUSION;
				return true;
			}
		} break;
	default:
		break;
	}
	//}} kimhc // 2010.11.1 // �Ž�ų

	return false;
}




/*virtual*/ void CX2GUEve::NoStateChangeActionFrameMove()
{
	switch( m_FrameDataNow.syncData.nowAction )
	{
	case UAI_MEGA_ELECTRONBALL:
		{
			if ( false == GetRidingOn() )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL );
				if( NULL != pSkillTemplet )
				{
					m_sMegaElectronBallData.m_bEnable	= true;

					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EN_CONVERSION ) );	/// ��ų ����

					m_sMegaElectronBallData.m_dMaxTime	= static_cast<double>(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGING_TIME, iSkillTempletLevel) );


					m_sMegaElectronBallData.m_timerElapsedTime.restart();

					if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO7 ) == true )
					{
						m_sMegaElectronBallData.m_dMaxTime *= 0.67f;
					}

					if( IsMyUnit() == true )
						g_pKTDXApp->GetDeviceManager()->PlaySound( L"Eve_MEB_Condense_power.ogg", true, false );
				}
			}
		} break;
	case UAI_MEGA_ELECTRONBALL_CHARGE_FAIL:
		{
			m_sMegaElectronBallData.Reset();
			if( IsMyUnit() == true )
				g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
		} break;

		//{{ kimhc // 2010.11.1 // �Ž�ų
	case UAI_CHARGING_BOOSTER:	// ���� - ���� ������
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_P_EEP_CHARGING_BOOSTER", this );
		} break;
		//}} kimhc // 2010.11.1 // �Ž�ų
	case UAI_ELECTRA_PLANE_ANGLE_UP:
		{
			m_fPlaneZAngle = 24.f;
		} break;
	case UAI_ELECTRA_PLANE_ANGLE_DOWN:
		{
			m_fPlaneZAngle = -24.f;
		} break;
	case UAI_THOUSANDS_OF_STARS:
		{
			if ( false == GetRidingOn() )
			{
				if( m_bEntireFireMode == false )
				{
					if( m_iLockOnUid != -1)
					{
						CX2GameUnit* pGameUnit = NULL;

						if( m_bLockOnNpc )
							pGameUnit = g_pX2Game->GetNPCUnitByUID( (int)m_iLockOnUid );
						else
							pGameUnit = g_pX2Game->GetUserUnitByUID( m_iLockOnUid );

						if( pGameUnit != NULL && pGameUnit->GetNowHp() > 0.f )
						{
							m_bEntireFireMode = true;
							if( m_iThousandStarCount % 2 == 0)
							{
								m_iThousandStarCount = (int)( m_iThousandStarCount / 2.f);
							}
							else
							{
								m_iThousandStarCount = (int)( m_iThousandStarCount / 2.f) + 1;
							}
							const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS );
							if( NULL != pSkillTemplet )
							{
								if ( NULL == GetUnit() )
									return;

								CX2UserSkillTree& userSkillTree = GetUnit()->AccessUnitData().m_UserSkillTree;

								const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

								float fRemainCoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - m_fElapsedTimeThousandStartCreated;

								if( fRemainCoolTime < 0.f )
									fRemainCoolTime = 0.f;

								userSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS, fRemainCoolTime  );
								if ( IsMyUnit() )
									g_pX2Game->UpdateSkillSlotUI();
							}
							PlaySound( L"EveVoice_ThousandStar_2.ogg" );

							if ( NULL != g_pX2Game && GetShowCutInAndChangeWorldColor() )
							{
								g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );
								g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 5.0f );
							}

						}
					}
				}
				if( m_bEntireFireMode == false )
				{
					if( IsMyUnit() == true )
						g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_15829 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				}
			}
		} break;

	case UAI_TASER_PILUM:
		{
			if ( false == GetRidingOn() )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EBS_TASER_PILUM );

				if( NULL != pSkillTemplet )
				{
					m_sTaserPilumData.m_bEnable = true;

					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EBS_TASER_PILUM ) );	/// ��ų ����

					m_sTaserPilumData.m_dMaxTime = static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGING_TIME, iSkillTempletLevel) );

					m_sTaserPilumData.m_timerElapsedTime.restart();

					if( IsMyUnit() == true )
						g_pKTDXApp->GetDeviceManager()->PlaySound( L"Eve_MEB_Condense_power.ogg", true, false );
				}
			}
		} break;
	case UAI_TASER_PILUM_CHARGE_FAIL:
		{
			m_sTaserPilumData.Reset();
			if( IsMyUnit() == true )
				g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
		} break;

	case UAI_CHANGE_SPECTRO_WITHOUT_MOTION:
		{
			if ( false == GetRidingOn() )
			{
				if ( true == m_EveElectraSystem.GetEnableSystem() && PFT_FUSION != m_EveElectraSystem.GetPlaneFormation()
					&& true != m_FrameDataNow.stateParam.bNoChangSpectro )
				{
					StartElCrystalSystem();
					//SPECTRO_EL_CRYSTAL_Start();
					m_EveElectraSystem.StartFormationEffect();
					PlaySound( L"Eve_ElCrystal_1.ogg" );
				}
			}
		} break;

	case UAI_KUGELBLITZ:
		{
			if ( false == GetRidingOn() )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EBS_KUGELBLITZ );

				if( NULL != pSkillTemplet )
				{
					m_sKugelBlitz_Data.m_bEnable = true;

					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

					const int iSkillLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EBS_KUGELBLITZ ) );	/// ��ų ����
					m_sKugelBlitz_Data.m_dMaxTime = static_cast<double>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGING_TIME, iSkillLevel ) );
					m_sKugelBlitz_Data.m_timerElapsedTime.restart();

					if( IsMyUnit() == true )
						g_pKTDXApp->GetDeviceManager()->PlaySound( L"Eve_MEB_Condense_power.ogg", true, false );
				}
			}
		} break;

	case UAI_KUGELBLITZ_CHARGE_FAIL:
		{
			m_sKugelBlitz_Data.Reset();
			if( IsMyUnit() == true )
				g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
		} break;

	case UAI_EEP_LINK_OVERCHARGE_ILLUSION:
		{
			if ( false == GetRidingOn() && true == GetActiveLinkOverChargeIllusion() )
			{
				SetSelfDestructSummonedNPC( CX2UnitManager::NUI_SI_HA_FERDINAND );
			}
		} break;
	}
}





/*virtual*/ void CX2GUEve::CreateStepDust()
{
	if( true == GetAbsoluteInvisibility() )
		return;

	CX2GUUser::CreateStepDust();
}


void CX2GUEve::CancelComboWhenChangedDirection( int iFlagCount )
{
	bool bCancel = false;
	if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		bCancel = true;
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		bCancel = true;
	}

	if( true == bCancel )
	{
		for( int i=0; i<iFlagCount;i++ )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[i] = false;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//							������Ʈ �Լ� ����

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//ENSI_DIE_FRONT
void CX2GUEve::ENSI_DIE_FRONT_StartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( 0.5f );
}

void CX2GUEve::ENSI_DIE_FRONT_Start()
{
	CommonStateStart();
	DyingStart();

	m_pXSkinAnim->SetPlaySpeed( 0.5f );


	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );

	//{{ JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );	/// ����� ���� ��ȯ ����
	//}} JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_SI_HA_FERDINAND );

	m_sEGQueensThrone.Reset();

    if ( m_hEffectSweepParadeTriangle != INVALID_DAMAGE_EFFECT_HANDLE )
        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectSweepParadeTriangle );

}

void CX2GUEve::ENSI_DIE_FRONT_FrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

void CX2GUEve::ENSI_DIE_FRONT_EventProcess()
{
	//{{ robobeg : 2008-10-21
	//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
	if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
		//}} robobeg : 2008-10-21
	{
		StateChange( GUSI_DIE );
	}
}


//ENSI_DIE_BACK
void CX2GUEve::ENSI_DIE_BACK_StartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( 0.5f );
}

void CX2GUEve::ENSI_DIE_BACK_Start()
{
	CommonStateStart();
	DyingStart();

	m_pXSkinAnim->SetPlaySpeed( 0.5f );

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );

	//{{ JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );	/// ����� ���� ��ȯ ����
	//}} JHKang / ������ / 2010/11/23 / ����� ����
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_SI_HA_FERDINAND );

	m_sEGQueensThrone.Reset();

    if ( m_hEffectSweepParadeTriangle != INVALID_DAMAGE_EFFECT_HANDLE )
        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectSweepParadeTriangle );

}

void CX2GUEve::ENSI_DIE_BACK_FrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.32f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

void CX2GUEve::ENSI_DIE_BACK_EventProcess()
{
	//{{ robobeg : 2008-10-21
	//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
	if ( GetShowObject() == false  && m_pXSkinAnim->IsAnimationEnd() == true )
		//}} robobeg : 2008-10-21
	{
		StateChange( GUSI_DIE );
	}
}

void CX2GUEve::ENSI_WAIT_StartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );

	m_iJumpFlyCount = 0;
	m_iDashJumpCount = 0;
	m_iDashJumpUpCount = 0;
	m_eBoostDashState = BDS_NONE;
	m_bEnableBoostDash = false;
}

void CX2GUEve::ENSI_WAIT_Start()
{
	CommonStateStart();
	PlayAnimationWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}

void CX2GUEve::ENSI_WAIT_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ENSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		StateChange( USI_WALK );
	}
	else if( m_InputData.pureUp == true )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )


		//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )
	else if( m_InputData.oneDown == true
	&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_WALK_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ENSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.pureRight == false && m_InputData.pureLeft == false )
	{
		StateChange( USI_WAIT );
	}
	else if( m_InputData.pureUp == true )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )

		//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )
	else if( m_InputData.oneDown == true
	&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	LINE_RADIUS,
	&m_FrameDataFuture.unitCondition.landPosition,
	&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}


//ENSI_JUMP_READY
void CX2GUEve::ENSI_JUMP_READY_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_JUMP_COMBO_Z );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;

		m_pComboGuide->ShowComboCommand(L"J", L"JZ");
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_JUMP_COMBO_X );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
		m_pComboGuide->ShowComboCommand(L"J", L"JX");
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true
		|| m_InputData.pureRight == true
		|| m_InputData.pureLeft == true )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_JUMP_UP_EventProcess()
{
	if( m_iDashJumpCount < 2 + m_iPhotonBoosterAdditionalDashJumpCount && m_InputData.pureDown == false &&
		( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true ) )
	{
		StateChangeJumpDashIfPossible();
	}
	else if( m_InputData.oneUp == true && m_iJumpFlyCount == 0 )
	{
		StateChange( ENSI_JUMP_FLY_UP );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_JUMP_COMBO_Z );
		m_pComboGuide->ShowComboCommand(L"J", L"JZ");
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_JUMP_COMBO_X );
		m_pComboGuide->ShowComboCommand(L"J", L"JX");
	}
	else if( m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_JUMP_DOWN_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( true == m_bDownForce )
		{
			StateChange( ENSI_DASH_JUMP_POWER_LANDING );
		}
		else
		{
			StateChange( USI_JUMP_LANDING );
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( m_InputData.oneUp == true && m_iJumpFlyCount == 0 )
	{
		StateChange( ENSI_JUMP_FLY_UP );
	}
	else if( m_iDashJumpCount < 2 + m_iPhotonBoosterAdditionalDashJumpCount && false == m_InputData.pureDown &&
		( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true ) )
	{
		StateChangeJumpDashIfPossible();

	}
	else if( m_sEGQueensThrone.m_bEnable == true && false == m_InputData.pureDown &&
		( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true ) )
	{
		StateChangeJumpDashIfPossible();

	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_JUMP_COMBO_Z );
		m_pComboGuide->ShowComboCommand(L"J", L"JZ");
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_JUMP_COMBO_X );
		m_pComboGuide->ShowComboCommand(L"J", L"JX");
	}
	else if( m_InputData.pureDown == true )
	{
		m_bDownForce = true;
	}
	else if( m_PhysicParam.nowSpeed.y >= 0.f )
	{
		StateChange( USI_JUMP_UP );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

//ENSI_JUMP_LANDING
void CX2GUEve::ENSI_JUMP_LANDING_StartFuture()
{
	CommonStateStartFuture();

	m_iJumpFlyCount = 0;
	m_iDashJumpCount = 0;
	m_iDashJumpUpCount = 0;

	m_eBoostDashState = BDS_NONE;
	m_bEnableBoostDash = false;

}

void CX2GUEve::ENSI_JUMP_LANDING_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ENSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		StateChange( USI_WALK );
	}
	else if( m_InputData.pureUp == true )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
		//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )


	else if( m_InputData.oneDown == true
	&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	LINE_RADIUS,
	&m_FrameDataFuture.unitCondition.landPosition,
	&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		StateChange( USI_WAIT, false );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//ENSI_DASH
/*virtual*/ void CX2GUEve::DashStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.x = 0.0f;
	m_iJumpFlyCount = 0;
	m_iDashJumpCount = 0;
	m_iDashJumpUpCount = 0;

}

void CX2GUEve::ENSI_DASH_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if( m_iDashJumpCount < 2 + m_iPhotonBoosterAdditionalDashJumpCount )
		{
			StateChangeJumpDashIfPossible();

		}
		else
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_DASH_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( m_InputData.oneUp == true && m_iDashJumpUpCount  < 1   )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
		//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )
	else if( m_InputData.pureRight == false && m_InputData.pureLeft == false )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	if( true == m_bHighPoweredElectrics && 1.0f <= m_FrameDataFuture.unitCondition.fStateTime &&
		CX2Unit::UC_EVE_BATTLE_SERAPH == GetUnit()->GetClass() )
	{
		StateChange( ENSI_EBS_BOOST_DASH_START );
	}

	CommonEventProcess();
}


//ENSI_DASH_END
void CX2GUEve::ENSI_DASH_END_StartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUEve::ENSI_DASH_END_Start()
{
	CommonStateStart();

}

void CX2GUEve::ENSI_DASH_END_FrameMove()
{
	if( m_pXSkinAnim->EventTimer( 0.4f ) == true )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_DASH_END_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 8.0f )
		{
			StateChange( ENSI_COMBO_Z );
		}
		else
		{
			StateChange( ENSI_DASH_COMBO_Z );
		}
	}
	else if( m_InputData.oneX == true )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 8.0f )
		{
			StateChange( ENSI_COMBO_X );
		}
		else
		{
			StateChange( ENSI_DASH_COMBO_X );
		}
	}
	else if( SpecialAttackEventProcess() == true)
	{
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	//3Ű ���
	else if( m_InputData.oneRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.oneLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		StateChange( USI_WALK );
	}
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
		//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}



//ENSI_DASH_JUMP
void CX2GUEve::ENSI_DASH_JUMP_StartFuture()
{
	CommonStateStartFuture();

	//m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * 1.2f;
	m_bDownForce					= false;

	m_bDisableGravity = true;

	m_iDashJumpCount += 1;
	m_iJumpFlyCount += 1;
	m_iDashJumpUpCount++;

	// note!! ��� ���� �ӵ� ��ũ��Ʈ���� �׽�Ʈ �� �� ���� �ڵ�
	//float fValue1, fValue2, fValue3;
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_A",		fValue1,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_B",		fValue2,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_C",		fValue3,		1.f );
	//m_fCurrDashJumpTime = 0.65f;

	m_fCurrDashJumpTime	= s_fMaxDashJumpTime;

}


void CX2GUEve::ENSI_DASH_JUMP_Start()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
	{
		CreateStepDust();
	}


	if( false == GetAbsoluteInvisibility() )
	{
		D3DXVECTOR3 vRot = GetRotateDegree();
		vRot.z += 45.f;
		if( false == GetIsRight() )
		{
			vRot.y -= 15.f;
		}
		else
		{
			vRot.y += 15.f;
		}

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( vRot );
			pSeq->SetAxisAngle( vRot );
		}

		vBonePos.y -= 60.f;
		if( true == GetIsRight() )
		{
			vBonePos -= GetDirVector() * 60.f;
		}
		else
		{
			vBonePos += GetDirVector() * 60.f;
		}
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack_Small", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( vRot );
			pSeq->SetAxisAngle( vRot );
		}
	}
}

void CX2GUEve::ENSI_DASH_JUMP_FrameMoveFuture()
{
	m_fCurrDashJumpTime -= m_fElapsedTime;

	//float fValue1, fValue2, fValue3;
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_A",		fValue1,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_B",		fValue2,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_C",		fValue3,		1.f );

	const float fValue = m_fCurrDashJumpTime / s_fMaxDashJumpTime;
	const float NOW_FLY_SPEED_SCALE = s_fDashJumpSpeedScale * powf( fValue, 2 ) + s_fDashJumpSpeedLowerBound;

	if( m_fCurrDashJumpTime < 0.f )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureUp == true &&
		m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight	== true )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed()		* NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y			= GetDashJumpSpeed()	* NOW_FLY_SPEED_SCALE;
	}
	else if( m_InputData.pureUp == true &&
		m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight	== false )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed()		* NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y			= GetDashJumpSpeed()	* NOW_FLY_SPEED_SCALE;
	}
	else
	{
		StateChange( USI_JUMP_DOWN );
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_DASH_JUMP_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( USI_DASH_JUMP );
			m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
			m_bDownForce = false;
		}
		else
		{
			if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_JUMP_LANDING ); // not dash jump landing
			}
			m_bDownForce = false;
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_Z );
	}
	else if( m_InputData.oneX == true && m_fReDashJumpXTime <= 0.0f )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_X );
	}
	else if( m_InputData.pureRight == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == false )
		{
			//if( m_PhysicParam.nowSpeed.y > GetJumpSpeed() )
			//	m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
			{
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();
				StateChange( USI_JUMP_UP );
			}
			else
			{
				StateChange( USI_JUMP_DOWN );
			}
		}
	}
	else if( m_InputData.pureLeft == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			//if( m_PhysicParam.nowSpeed.y > GetJumpSpeed() )
			//	m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
			{
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();
				StateChange( USI_JUMP_UP );
			}
			else
			{
				StateChange( USI_JUMP_DOWN );
			}
		}
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}



	if( m_InputData.oneUp == true && m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	if( m_InputData.oneDown == true )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
		m_bDownForce = true;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}


	CommonEventProcess();
}

void CX2GUEve::ENSI_DASH_JUMP_EndFuture()
{
	/// ���� ���� �ε�
	CommonStateEndFuture();


	if( m_PhysicParam.nowSpeed.x > GetDashSpeed() )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	//m_PhysicParam.fDownAccel	= 0.0f;

	m_bDisableGravity = false;

	m_bAfterJumpDash = false;
}

void CX2GUEve::ENSI_DASH_JUMP_LANDING_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.oneRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.oneLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChangeDashIfPossible();
	}
	else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		StateChange( USI_WALK );
	}
	else if( m_InputData.pureUp == true )
	{
		StateChange( USI_JUMP_READY );
	}
	//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//ENSI_DASH_JUMP_POWER_LANDING
void CX2GUEve::ENSI_DASH_JUMP_POWER_LANDING_StartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x /= 2.0f;
}

void CX2GUEve::ENSI_DASH_JUMP_POWER_LANDING_Start()
{
	CommonStateStart();
	CreateStepDust();
}

void CX2GUEve::ENSI_DASH_JUMP_POWER_LANDING_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	//else if( m_InputData.oneRight == true || m_InputData.oneLeft == true )
	//{
	//	StateChange( ENSI_WALK );
	//}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.01f )
	{
		if( m_InputData.oneZ == true )
		{
			StateChange( ENSI_COMBO_Z );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( ENSI_COMBO_X );
		}
		else if( SpecialAttackEventProcess() == true )
		{
		}
		else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else if( m_InputData.pureUp == true )
		{
			StateChange( USI_JUMP_READY );
		}
		//ELSE_IF_EVE_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 35.f, m_bInvisibility, ENSI_INVISIBILITY_ON )
		else if( m_InputData.oneDown == true
			&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS,
			&m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}

		else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}




void CX2GUEve::ENSI_JUMP_FLY_UP_StartFuture()
{
	CommonStateStartFuture();

	//m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * 1.2f;
	m_bDownForce					= false;
	m_iJumpFlyCount		+= 1;
	m_iDashJumpCount	+= 1;

	m_bAfterJumpDash = false;
	m_bEnableBoostDash = false;

	//m_PhysicParam.nowSpeed.y			= m_PhysicParam.GetJumpSpeed() ;

}

void CX2GUEve::ENSI_JUMP_FLY_UP_Start()
{
	CommonStateStart();

	if( false == GetAbsoluteInvisibility() )
	{
		D3DXVECTOR3 vRot = GetRotateDegree();
		vRot.z += 90.f;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
		vBonePos.y -= 40.f;
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( vRot );
			pSeq->SetAxisAngle( vRot );
		}

		//vBonePos.y -= 60.f;
		//pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack_Small", vBonePos );
		//if( NULL != pSeq )
		//{
		//	pSeq->SetAddRotate( vRot );
		//	pSeq->SetAxisAngle( vRot );
		//}
	}
}


void CX2GUEve::ENSI_JUMP_FLY_UP_FrameMoveFuture()
{
	if( m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight	== true )
	{
		m_PhysicParam.nowSpeed.x			= GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight	== false )
	{
		m_PhysicParam.nowSpeed.x			= GetWalkSpeed();
	}

	if( m_InputData.pureDown == true  && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}
	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_JUMP_FLY_UP_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChange( USI_WALK );
		}
		else
		{
			StateChange( USI_JUMP_LANDING );
		}

		m_bDownForce = false;

	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		m_PhysicParam.nowSpeed.y = 0.f;
		StateChange( ENSI_JUMP_COMBO_Z );

		m_pComboGuide->ShowComboCommand(L"j", L"JJZ");
	}
	else if( m_InputData.oneX == true && m_fReDashJumpXTime <= 0.0f )
	{
		m_PhysicParam.nowSpeed.y = 0.f;
		StateChange( ENSI_JUMP_COMBO_X );

		m_pComboGuide->ShowComboCommand(L"j", L"JJX");
	}
	//else if( m_InputData.pureRight == true )
	//{
	//	if( m_FrameDataFuture.syncData.bIsRight == false )
	//	{
	//		if( m_PhysicParam.nowSpeed.y > GetJumpSpeed() )
	//			m_PhysicParam.nowSpeed.y = GetJumpSpeed();

	//		if( m_PhysicParam.nowSpeed.y > 0.0f )
	//			StateChange( ENSI_JUMP_UP );
	//		else
	//			StateChange( ENSI_JUMP_DOWN );
	//	}
	//}
	//else if( m_InputData.pureLeft == true )
	//{
	//	if( m_FrameDataFuture.syncData.bIsRight == true )
	//	{
	//		if( m_PhysicParam.nowSpeed.y > GetJumpSpeed() )
	//			m_PhysicParam.nowSpeed.y = GetJumpSpeed();

	//		if( m_PhysicParam.nowSpeed.y > 0.0f )
	//			StateChange( ENSI_JUMP_UP );
	//		else
	//			StateChange( ENSI_JUMP_DOWN );
	//	}
	//}
	else if( m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		StateChange( ENSI_JUMP_FLY_DOWN );
	}

	//if( m_InputData.oneUp == true && m_PhysicParam.nowSpeed.y <= 0.0f )
	//{
	//	m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	//}
	//if( m_InputData.oneDown == true )
	//{
	//	m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
	//	m_bDownForce = true;
	//	m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	//}



	CommonEventProcess();
}

void CX2GUEve::ENSI_JUMP_FLY_UP_EndFuture()
{
	CommonStateEnd();

	//m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * -0.8f;
	//m_PhysicParam.fDownAccel	= 0.0f;
}





void CX2GUEve::ENSI_JUMP_FLY_DOWN_StartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * -0.8f;
	m_bDownForce = false;

}

void CX2GUEve::ENSI_JUMP_FLY_DOWN_FrameMoveFuture()
{
	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	if( m_InputData.pureDown == true && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
	}
	CommonFrameMoveFuture();
}


void CX2GUEve::ENSI_JUMP_FLY_DOWN_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( true == m_bDownForce )
		{
			StateChange( ENSI_DASH_JUMP_POWER_LANDING );
		}
		else
		{
			StateChange( USI_JUMP_LANDING );
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_JUMP_COMBO_Z );
		m_pComboGuide->ShowComboCommand(L"j", L"JJZ");
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_JUMP_COMBO_X );
		m_pComboGuide->ShowComboCommand(L"j", L"JJX");
	}
	else if( m_PhysicParam.nowSpeed.y >= 0.0f )
	{
		StateChange( USI_JUMP_UP );
	}
	else if( m_InputData.pureDown == true )
	{
		m_bDownForce = true;
	}

	CommonEventProcess();
}


void CX2GUEve::ENSI_JUMP_FLY_DOWN_EndFuture()
{
	CommonStateEnd();

	m_PhysicParam.fDownAccel	= 0.f;
}



void CX2GUEve::ENSI_JUMP_DASH_StartFuture()
{
	CommonStateStartFuture();

	//m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * 1.2f;
	m_bDownForce					= false;

	m_bDisableGravity	= true;
	m_iDashJumpCount	+= 1;
	m_iJumpFlyCount		+= 1;


	//float fValue1, fValue2, fValue3;
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_A",		fValue1,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_B",		fValue2,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_C",		fValue3,		1.f );
	//m_fCurrDashJumpTime = fValue3;

	m_fCurrDashJumpTime = s_fMaxDashJumpTime;
}

void CX2GUEve::ENSI_JUMP_DASH_Start()
{
	CommonStateStart();

	if( false == GetAbsoluteInvisibility() )
	{
		D3DXVECTOR3 vRot = GetRotateDegree();
		if( false == GetIsRight() )
		{
			vRot.y -= 15.f;
		}
		else
		{
			vRot.y += 15.f;
		}

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( vRot );
			pSeq->SetAxisAngle( vRot );
		}


		if( true == GetIsRight() )
		{
			vBonePos -= GetDirVector() * 60.f;
		}
		else
		{
			vBonePos += GetDirVector() * 60.f;
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack_Small", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( vRot );
			pSeq->SetAxisAngle( vRot );
		}
	}
}

void CX2GUEve::ENSI_JUMP_DASH_FrameMoveFuture()
{
	m_fCurrDashJumpTime -= m_fElapsedTime;


	//float fValue1, fValue2, fValue3;
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_A",		fValue1,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_B",		fValue2,		1.f );
	//LUA_GET_VALUE( m_LuaManager, "TEST_SPEED_C",		fValue3,		1.f );
	//const float fValue = m_fCurrDashJumpTime / fValue3;

	const float fValue = m_fCurrDashJumpTime / s_fMaxDashJumpTime;
	const float NOW_FLY_SPEED_SCALE = s_fDashJumpSpeedScale * powf( fValue, 2 ) + s_fDashJumpSpeedLowerBound;


	if( m_fCurrDashJumpTime < 0.f )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureDown == false &&
		m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y			= 0.f;
	}
	else if( m_InputData.pureDown == false &&
		m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y			= 0.f;
	}
	else
	{
		StateChange( USI_JUMP_DOWN );
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_JUMP_DASH_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( USI_DASH_JUMP );
			m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
			m_bDownForce = false;
		}
		else if( m_bDownForce == true )
		{
			StateChange( ENSI_DASH_JUMP_POWER_LANDING );
			m_bDownForce = false;
		}
		else
		{
			if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
			}
			m_bDownForce = false;
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( m_iDashJumpUpCount < 1 &&
		m_InputData.pureDown == false && m_InputData.pureUp == true )
	{
		StateChange( USI_DASH_JUMP );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_Z );
	}
	else if( m_InputData.oneX == true && m_fReDashJumpXTime <= 0.0f )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_X );
	}
	else if( m_InputData.pureRight == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == false )
		{
			if( m_PhysicParam.nowSpeed.y > GetJumpSpeed() )
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}
	else if( m_InputData.pureLeft == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			if( m_PhysicParam.nowSpeed.y > GetJumpSpeed() )
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}


	if( m_InputData.oneUp == true && m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}


	if( m_InputData.pureDown == true )
	{
		//m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
		m_bDownForce = true;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}


	CommonEventProcess();
}

void CX2GUEve::ENSI_JUMP_DASH_EndFuture()
{
	CommonStateEnd();

	//m_PhysicParam.fDownAccel	= 0.0f;

	if( m_PhysicParam.nowSpeed.x > GetDashSpeed() )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	m_bDisableGravity = false;

	m_bAfterJumpDash = true;
}








//ENSI_HYPER_MODE
void CX2GUEve::HyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.05f, 0.83f );
}


////ENSI_DAMAGE_GROGGY
//void CX2GUEve::ENSI_DAMAGE_GROGGY_FrameMoveFuture()
//{
//	if( m_fGroggyTime > 0.0f )
//	{
//		m_fGroggyTime -= m_fElapsedTime;
//		if( m_fGroggyTime < 0.0f )
//			m_fGroggyTime = 0.0f;
//	}
//	CommonFrameMoveFuture();
//}
//
//void CX2GUEve::ENSI_DAMAGE_GROGGY_EventProcess()
//{
//	if( m_fGroggyTime <= 0.0f )
//	{
//		StateChange( ENSI_WAIT );
//	}
//	CommonEventProcess();
//}

//ENSI_DAMAGE_SMALL_FRONT
void CX2GUEve::ENSI_DAMAGE_SMALL_FRONT_EventProcess()
{
	if( m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y > 200.0f )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//ENSI_DAMAGE_SMALL_BACK
void CX2GUEve::ENSI_DAMAGE_SMALL_BACK_EventProcess()
{
	if( m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y > 200.0f )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//ENSI_DAMAGE_BIG_FRONT
void CX2GUEve::ENSI_DAMAGE_BIG_FRONT_Start()
{
	CommonStateStart();

	CreateStepDust();

}

void CX2GUEve::ENSI_DAMAGE_BIG_FRONT_EventProcess()
{
	if( m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y > 200.0f )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.93f )
		{
			StateChange( USI_WAIT );
		}
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//ENSI_DAMAGE_BIG_BACK
void CX2GUEve::ENSI_DAMAGE_BIG_BACK_Start()
{
	CommonStateStart();

	CreateStepDust();

}

void CX2GUEve::ENSI_DAMAGE_BIG_BACK_EventProcess()
{
	if( m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y > 200.0f )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//ENSI_DAMAGE_DOWN_FRONT
void CX2GUEve::ENSI_DAMAGE_DOWN_FRONT_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
	{

		CreateStepDust();

	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_DAMAGE_DOWN_FRONT_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[4] == true )
		{
			if( GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent )
			{
				StateChange( ENSI_STANDUP_ATTACK_FRONT ); // fix!!
			}
			else
			{
				StateChange( ENSI_STANDUP_ATTACK_FRONT );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ENSI_STANDUP_ATTACK_FRONT );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
			StateChange( ENSI_STANDUP_ROLLING_FRONT_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
			StateChange( ENSI_STANDUP_ROLLING_FRONT_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_FRONT );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66 )
	{
		if( m_InputData.oneX == true )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[4]	= true;
		}
		else if( m_InputData.oneZ == true )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[1]	= true;
			m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
		}
		else if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			if( m_InputData.pureRight == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
			else if( m_InputData.pureLeft == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
		}
		else
		{
			if( m_InputData.pureRight == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
			else if( m_InputData.pureLeft == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
		}
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_DAMAGE_DOWN_BACK_FrameMove()
{
	if( m_pXSkinAnim->EventTimer( 0.32f ) == true )
	{
		CreateStepDust();

	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_DAMAGE_DOWN_BACK_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[4] == true )
		{
			if( GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent )
			{
				StateChange( ENSI_STANDUP_ATTACK_BACK ); // fix!!
			}
			else
			{
				StateChange( ENSI_STANDUP_ATTACK_BACK );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ENSI_STANDUP_ATTACK_BACK );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
			StateChange( ENSI_STANDUP_ROLLING_BACK_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
			StateChange( ENSI_STANDUP_ROLLING_BACK_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_BACK );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66 )
	{
		if( m_InputData.oneX == true )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[4]	= true;
		}
		else if( m_InputData.oneZ == true )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[1]	= true;
			m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
		}
		else if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			if( m_InputData.pureRight == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
			else if( m_InputData.pureLeft == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
		}
		else
		{
			if( m_InputData.pureRight == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
			else if( m_InputData.pureLeft == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= true;
				m_FrameDataFuture.stateParam.bEventFlagList[4]	= false;
			}
		}
	}

	CommonEventProcess();
}

//ENSI_DAMAGE_AIR_DOWN_LANDING
void CX2GUEve::ENSI_DAMAGE_AIR_DOWN_LANDING_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
			StateChange( ENSI_STANDUP_ATTACK_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
			StateChange( ENSI_STANDUP_ROLLING_FRONT_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
			StateChange( ENSI_STANDUP_ROLLING_FRONT_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_FRONT );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
	{
		if( m_InputData.oneZ == true || m_InputData.oneX == true )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
			m_FrameDataFuture.stateParam.bEventFlagList[1]	= true;
		}
		else if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			if( m_InputData.pureRight == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= true;
			}
			else if( m_InputData.pureLeft == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= true;
			}
		}
		else
		{
			if( m_InputData.pureRight == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= true;
			}
			else if( m_InputData.pureLeft == true )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[1]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[3]	= false;
				m_FrameDataFuture.stateParam.bEventFlagList[2]	= true;
			}
		}
	}

	CommonEventProcess();
}

//ENSI_STANDUP_ROLLING_FRONT_FRONT
void CX2GUEve::ENSI_STANDUP_ROLLING_FRONT_FRONT_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_STANDUP_ROLLING_FRONT_FRONT_End()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}


//ENSI_STANDUP_ROLLING_FRONT_BACK
void CX2GUEve::ENSI_STANDUP_ROLLING_FRONT_BACK_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


void CX2GUEve::ENSI_STANDUP_ROLLING_FRONT_BACK_End()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}



//ENSI_STANDUP_ROLLING_BACK_FRONT
void CX2GUEve::ENSI_STANDUP_ROLLING_BACK_FRONT_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_STANDUP_ROLLING_BACK_FRONT_End()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}



//ENSI_STANDUP_ROLLING_BACK_BACK
void CX2GUEve::ENSI_STANDUP_ROLLING_BACK_BACK_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_STANDUP_ROLLING_BACK_BACK_End()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}


//ENSI_STANDUP_ATTACK_FRONT
void CX2GUEve::ENSI_STANDUP_ATTACK_FRONT_FrameMove()
{

	CommonFrameMove();
}

void CX2GUEve::ENSI_STANDUP_ATTACK_FRONT_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_STANDUP_ATTACK_FRONT_End()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}


//ENSI_STANDUP_ATTACK_BACK
void CX2GUEve::ENSI_STANDUP_ATTACK_BACK_FrameMove()
{

	CommonFrameMove();
}

void CX2GUEve::ENSI_STANDUP_ATTACK_BACK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_STANDUP_ATTACK_BACK_End()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}



void CX2GUEve::ENSI_INVISIBILITY_ON_Start()
{
	CommonStateStart();


	float fAddMoveSpeedFactor = 0.f;

	m_fInvisibilityTime = s_fEveDefaultInvisibilityTime;

	const CX2SkillTree::SkillTemplet*	pSkillTemplet	= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EN_CLOAKING );

	if( NULL != pSkillTemplet )
	{
		int iSkillLevel	= GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EN_CLOAKING );
		m_fInvisibilityTime += pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel );
		fAddMoveSpeedFactor = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel );
		// Ŭ��ŷ�� ���Ǵ� MP �Ҹ�, SA �� ����
		// SA ������ ���̳ʽ� ������ ��������
		m_fDecraseInvisibilityMP = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_RETAKE_ABS, iSkillLevel );
	}

	SetVecMoveSpeedFactor( 0.75f + fAddMoveSpeedFactor, m_fInvisibilityTime, CX2GameUnit::SFI_SPEED_13, CX2GameUnit::SFST_UPDATE );


	m_timerInvisibility.restart();
	m_fInvisibilityAlpha	= 1.f;
	m_bInvisibility			= true;
	m_bAbsoluteInvisibility = false;

	SetAlphaObject( true );
	m_RenderParam.bAlphaBlend = true;



	// ���� �����̸� ���� ��ƼŬ�� �Ⱥ��̰� �Ѵ�.
	if( GetRemainHyperModeTime() > 0.f )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq_RFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRFoot );
		CKTDGParticleSystem::CParticleEventSequence* pSeq_LFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLFoot );
		CKTDGParticleSystem::CParticleEventSequence* pSeq_RArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRArm );
		CKTDGParticleSystem::CParticleEventSequence* pSeq_LArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLArm );

		if( pSeq_RFoot != NULL )
		{
			pSeq_RFoot->SetShowObject( false );
		}
		if( pSeq_LFoot != NULL )
		{
			pSeq_LFoot->SetShowObject( false );
		}
		if( pSeq_RArm != NULL )
		{
			pSeq_RArm->SetShowObject( false );
		}
		if( pSeq_LArm != NULL )
		{
			pSeq_LArm->SetShowObject( false );
		}
	}

}

void CX2GUEve::ENSI_INVISIBILITY_ON_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_Sp_Invisible01", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}

	float fAlphaBound = CalcInvisibilityAlphaValue();

	if( m_fInvisibilityAlpha > fAlphaBound )
	{
		m_fInvisibilityAlpha = 1.f - (1.f - fAlphaBound) * m_pXSkinAnim->GetNowAnimationTime() / m_pXSkinAnim->GetMaxAnimationTime();
		if( m_fInvisibilityAlpha < fAlphaBound )
		{
			m_fInvisibilityAlpha = fAlphaBound;
		}
	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_INVISIBILITY_ON_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{

		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


// ���� ���°� �� �� ���İ��� ������ش�, �� ���� ���� �̺갡 �� ȭ�鿡 ������ �� ������ ������.
float CX2GUEve::CalcInvisibilityAlphaValue()
{
	float fAlphaBound = 0.f;

	if( true == IsMyUnit() )
	{
		fAlphaBound = 0.5f;
	}
	else if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetMyUnit() )
	{
		if( g_pX2Game->GetMyUnit()->GetTeam() == GetTeam() )
		{
			fAlphaBound = 0.5f;
		}
	}

	return fAlphaBound;
}

void CX2GUEve::ENSI_INVISIBILITY_ON_End()
{
	m_fInvisibilityAlpha = CalcInvisibilityAlphaValue();
	m_timerInvisibility.restart();
	m_bInvisibility	= true;

	if( 0.f == m_fInvisibilityAlpha )
	{
		m_bAbsoluteInvisibility = true;
	}
	else
	{
		m_bAbsoluteInvisibility = false;
	}



	if( true == GetAbsoluteInvisibility() )
	{
		//{{ kimhc // 2010.6.16 // ���� ����� �ϴ� �Լ��� �����Ͽ� �Լ��� �����
		SetShowEnchantWeaponParticle( false );
		//{{ kimhc // 2010.6.16 // ���� ����� �ϴ� �Լ��� �����Ͽ� �Լ��� �����

		// ��� ������ ��ƼŬ �Ⱥ��̰�
		for( CX2EqipPtr pEquip : m_ViewEqipList )
		{
			if( pEquip != NULL )
				pEquip->SetShowAttachedParticle( false );
		}


	}

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO5 ) == true )
	{
		ResetMPChangeRate( 3.f );
	}



	CommonStateEnd();
}

void CX2GUEve::ENSI_COMBO_Z_Start()
{
	CommonStateStart();

	// To do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_COMBO_Z_End()
{
	// To do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_COMBO_Z_FrameMove()
{
	//if( m_pXSkinAnim->EventTimer( 0.25f ) == true && EventCheck( 0.25f, false ) == true )
	//{
	//	m_DamageData.hitUnitList.resize(0);
	//}

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetPos();
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_COMBO_Z1", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetPos();
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}



void CX2GUEve::ENSI_COMBO_Z_EventProcess()
{
	CancelComboWhenChangedDirection(1);

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, ENSI_COMBO_ZZ )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			//m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}

		CommonEventProcess();


}

//ENSI_COMBO_ZZ
void CX2GUEve::ENSI_COMBO_ZZ_Start()
{
	CommonStateStart();

	// To do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_COMBO_ZZ_End()
{
	// To do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_COMBO_ZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetPos();
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_COMBO_Z2", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z2] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z2] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z2] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetPos();
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z2] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_ZZ_EventProcess()
{
	CancelComboWhenChangedDirection(1);

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, ENSI_COMBO_ZZZ )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			//m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}

		CommonEventProcess();


}

// ENSI_COMBO_ZZZ
void CX2GUEve::ENSI_COMBO_ZZZ_Start()
{
	CommonStateStart();

	// To do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_COMBO_ZZZ_End()
{
	// To do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_COMBO_ZZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetPos();
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_COMBO_Z3", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z3] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z3] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z3] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetPos();
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z3] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_ZZZ_EventProcess()
{
	m_bReAttackZ1Right = m_FrameDataFuture.syncData.bIsRight;
	m_fReAttackZ1Time = 0.2f;

	CancelComboWhenChangedDirection(4);

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange, (m_InputData.oneZ == true && m_InputData.pureDown == true), ENSI_COMBO_ZZZdownZ )
		ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange, m_InputData.oneZ == true &&
		( GetUnit()->GetClass() == CX2Unit::UC_EVE_ELECTRA || GetUnit()->GetClass() == CX2Unit::UC_EVE_BATTLE_SERAPH ) &&
		( ( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) ||
		( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ), ENSI_EPR_COMBO_ZZZfrontZ )
		ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, ENSI_COMBO_ZZZZ )
		ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true, ENSI_COMBO_ZZZX )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			//m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
		CommonEventProcess();
}





//// ENSI_COMBO_ZZZZ
//void CX2GUEve::ENSI_COMBO_ZZZZ_FrameMoveFuture()
//{
//	if( m_pXSkinAnimFuture->EventTimer( 0.6f ) == true && EventCheck( 0.6f, false ) == true )
//	{
//		m_PhysicParam.nowSpeed.x = GetWalkSpeed() * 1.3f;
//	}
//
//	CommonFrameMoveFuture();
//}

void CX2GUEve::ENSI_COMBO_ZZZZ_Start()
{
	CommonStateStart();

	// To do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_COMBO_ZZZZ_End()
{
	// To do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z1] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_COMBO_ZZZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		m_DamageData.bReAttack		= false;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_COMBO_Z4_01", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_01] = pEffect->GetMainEffectHandle();
		}

		vPos = GetBonePos(L"Dummy1_Rhand");
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_COMBO_Z4_02", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_02] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy1_Rhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Combo_Z4_02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_COMBO_ZZZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER_CANNOT_ATTACKTIME( m_fWalkCancelAfter, 0.1f )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();


}


void CX2GUEve::ENSI_COMBO_ZZZX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger01" );
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( vBonePos );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"STR_ChalSSak", projPos.x, projPos.y, 0.f );

		//PlaySound( L"Eve_CheekSlap.ogg" );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_ZZZX_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER_CANNOT_ATTACKTIME( m_fWalkCancelAfter, 0.1f )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();

}

//ENSI_COMBO_ZZZdownZ
void CX2GUEve::ENSI_COMBO_ZZZdownZ_StartFuture()
{
	CommonStateStartFuture();

	//{{ 09.01.21.hoons.
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"EG_ComboZ4Down", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
	//}} hoons
}

void CX2GUEve::ENSI_COMBO_ZZZdownZ_Start()
{
	CommonStateStart();

	m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ]		= INVALID_MESH_INSTANCE_HANDLE;

	//{{ 09.01.21.hoons.
	m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ_EG]	= INVALID_MESH_INSTANCE_HANDLE;

	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnim->ChangeAnim( L"EG_ComboZ4Down", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
	//}} hoons
}


void CX2GUEve::ENSI_COMBO_ZZZdownZ_End()
{
	// 09.01.21.hoons. �������Ŀ��� �������� �޺��� �Ѿ���ֱ⶧����
	m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ]		= INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ_EG]	= INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}




void CX2GUEve::ENSI_COMBO_ZZZdownZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.33f ) )
	{
		//{{ 09.01.20.hoons.
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			{
				//����
				D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );


				CKTDGParticleSystem::CParticleEventSequence* pSeq =
					g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboZ5Down_Particle01", vBonePos );
				if( NULL != pSeq )
				{
					pSeq->SetAddRotate( GetRotateDegree() );
					pSeq->SetAxisAngle( GetRotateDegree() );
					pSeq->SetLandPosition( GetLandPosition().y );
				}

				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_ZZZdownZ", GetPowerRate(),
					vBonePos, GetRotateDegree(), GetRotateDegree() );

				if( NULL != pEffect )
				{
					m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ] = pEffect->GetMainEffectHandle();
				}

				UpDownCrashCameraNoReset( 5.f, 0.15f );

				//����
				vBonePos = GetBonePos( L"Dummy2_Lhand" );

				D3DXVECTOR3 vAngle = GetRotateDegree();
				vAngle.y += 180.f;

				pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboZ5Down_Particle01", vBonePos );
				if( NULL != pSeq )
				{
					pSeq->SetAddRotate( vAngle );
					pSeq->SetAxisAngle( vAngle );
					pSeq->SetLandPosition( GetLandPosition().y );
				}

				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_ZZZdownZ", GetPowerRate(), vBonePos, vAngle, vAngle );

				if( NULL != pEffect )
				{
					m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ_EG] = pEffect->GetMainEffectHandle();
				}
			}
			break;

		default:
			{
				D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );


				CKTDGParticleSystem::CParticleEventSequence* pSeq =
					g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboZ5Down_Particle01", vBonePos );
				if( NULL != pSeq )
				{
					pSeq->SetAddRotate( GetRotateDegree() );
					pSeq->SetAxisAngle( GetRotateDegree() );
					pSeq->SetLandPosition( GetLandPosition().y );
				}

				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_ZZZdownZ", GetPowerRate(),
					vBonePos, GetRotateDegree(), GetRotateDegree() );

				if( NULL != pEffect )
				{
					m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ] = pEffect->GetMainEffectHandle();
				}

				UpDownCrashCameraNoReset( 5.f, 0.15f );
			}
			break;
		}
		//}} hoons.
	}


	//{{ 09.01.20.hoons.
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f &&
				INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ_EG] )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ_EG] );
				if( NULL != pMeshInst )
				{
					D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
					pMeshInst->SetPos( vBonePos );
				}
				else
				{
					m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ_EG] = INVALID_MESH_INSTANCE_HANDLE;
				}
			}
		}
		break;

	default:
		{
			if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f &&
				INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ] )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ] );
				if( NULL != pMeshInst )
				{
					D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
					pMeshInst->SetPos( vBonePos );
				}
				else
				{
					m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ] = INVALID_MESH_INSTANCE_HANDLE;
				}
			}
		}
		break;
	}
	//}} hoons.

	CommonFrameMove();
}


void CX2GUEve::ENSI_COMBO_ZZZdownZ_EventProcess()
{
	CancelComboWhenChangedDirection(1);


	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	//{{ 09.01.21.hoons.
	// ZZZdownZ�޺��� ����Ǵ� �κ� �ϴ� ����
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.53f )
	{
		StateChange( ENSI_COMBO_ZZZdownZZ );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_InputData.pureDown == true &&
		m_pXSkinAnimFuture->GetNowAnimationTime() < 0.53f )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
			break;
		default:
			break;
		}
	}
	//}}
	else if( m_InputData.pureUp == true || m_InputData.pureDown == true
		|| m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		float fSkipTime = 0.55f;
		//if( GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR )
		//	fSkipTime = 0.55f;

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > fSkipTime )
		{
			StateChange( USI_WAIT );
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//ENSI_COMBO_ZZZdwonZZ

void CX2GUEve::ENSI_COMBO_ZZZdownZZ_Init()
{
	switch ( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		XSkinMeshReadyInBackground( L"Eve_AT_ComboZ5a_Mesh.X" );
		break;
	default:
		break;
	}
}

void CX2GUEve::ENSI_COMBO_ZZZdownZZ_StartFuture()
{
	CommonStateStartFuture();

	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"EG_ComboZ5Down", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"AT_ComboZ5a", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
}

void CX2GUEve::ENSI_COMBO_ZZZdownZZ_Start()
{
	CommonStateStart();

	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnim->ChangeAnim( L"EG_ComboZ5Down", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_pXSkinAnim->ChangeAnim( L"AT_ComboZ5a", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
}

void CX2GUEve::ENSI_COMBO_ZZZdownZZ_End()
{
	m_ahMajorMeshInstance[MajorMID_ComboZZZdownZ] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_COMBO_ZZZdownZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			{
				D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Footsteps" );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_COMBO_ZZZdownZZ", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
			}
			break;

		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			{
				//ĳ���� ����
				D3DXVECTOR3 vBonePos	= GetBonePos( L"Dummy1_Rhand" );
				D3DXVECTOR3 vAngle		= GetRotateDegree();
				--vAngle.z += -30.f;

				CKTDGParticleSystem::CParticleEventSequence* pSeq =
					g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboZ5Down_Particle01", vBonePos );
				if( NULL != pSeq )
				{
					pSeq->SetAddRotate( vAngle );
					pSeq->SetAxisAngle( vAngle );
					pSeq->SetLandPosition( GetLandPosition().y );
				}

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_ZZZdownZZ", GetPowerRate(), vBonePos, vAngle, vAngle );

				UpDownCrashCameraNoReset( 5.f, 0.15f );

				//ĳ���� ����.
				vBonePos = GetBonePos( L"Dummy2_Lhand" );

				vAngle = GetRotateDegree();
				vAngle.y += 180.f;
				vAngle.z += -30.f;

				pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboZ5Down_Particle01", vBonePos );
				if( NULL != pSeq )
				{
					pSeq->SetAddRotate( vAngle );
					pSeq->SetAxisAngle( vAngle );
					pSeq->SetLandPosition( GetLandPosition().y );
				}

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_ZZZdownZZ", GetPowerRate(), vBonePos, vAngle, vAngle );
			}
			break;
		default:
			break;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_ZZZdownZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureUp == true || m_InputData.pureDown == true
		|| m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		float fSkipTime = 0.7f;
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			fSkipTime = 0.45f; // 1/3 ����
			break;
		default:
			break;
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > fSkipTime )
		{
			StateChange( USI_WAIT );
		}
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}



void CX2GUEve::ENSI_COMBO_X_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		if( FlushMp( 8.f * m_fMPConsumeRateElectronBall ) == true )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			D3DXVECTOR3 vPos = GetPos();

			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * 100.f;
			}
			else
			{
				vPos -= GetDirVector() * 100.f;
			}
			vPos.y += 120.f;

			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboX1_FireImpact01", vPos );
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_COMBO_X_FIRE_BALL", GetPowerRate() * m_fAttackPowerRateElectronBall,
				vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate );

			if( NULL != pEffect )
				pEffect->SetInheritForceDownRate(true);

			if( NULL != pEffect )
				m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_COMBO_X_FIRE_BALL_AMPLIFY_FILTERED",
				L"EVE_COMBO_X_FIRE_BALL_SPECTRUM_FILTERED", L"EVE_COMBO_X_FIRE_BALL_INDUCTION_FILTERED",
				L"EVE_COMBO_X_FIRE_BALL_FUSION_FILTERED" );




			PlaySound( L"Eve_FireShot1.ogg" );
		}
		else
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( 0, 1.1f, 280.f, 130.f, L"EveProtoPurityXXZPlane" );
	}


	CommonFrameMove();
}



void CX2GUEve::ENSI_COMBO_X_EventProcess()
{
	CancelComboWhenChangedDirection( 2 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true, ENSI_COMBO_XX )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER_CANNOT_ATTACKTIME( m_fWalkCancelAfter, 0.2f )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();

}


void CX2GUEve::ENSI_COMBO_XX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );

		if( FlushMp( 8.f * m_fMPConsumeRateElectronBall ) == true )
		{

			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_COMBO_X_FIRE_BALL", GetPowerRate() * m_fAttackPowerRateElectronBall,
				vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate );

			if( NULL != pEffect )
				pEffect->SetInheritForceDownRate(true);

			if( NULL != pEffect )
				m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_COMBO_X_FIRE_BALL_AMPLIFY_FILTERED",
				L"EVE_COMBO_X_FIRE_BALL_SPECTRUM_FILTERED", L"EVE_COMBO_X_FIRE_BALL_INDUCTION_FILTERED",
				L"EVE_COMBO_X_FIRE_BALL_FUSION_FILTERED" );
			PlaySound( L"Eve_FireShot1.ogg" );
		}
		else
		{
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 100.f;
		}
		else
		{
			vPos -= GetDirVector() * 100.f;
		}
		vPos.y += 120.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_ComboX1_FireImpact01", vPos );
	}

	CommonFrameMove();
}



void CX2GUEve::ENSI_COMBO_XX_EventProcess()
{
	CancelComboWhenChangedDirection( 6 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( m_FrameDataFuture.stateParam.bEventFlagList[5] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.55f )
	{
		m_iSuccessiveHit_OneZ = 0;
		StateChange( ENSI_NS_COMBO_XXZ );
		m_FrameDataFuture.stateParam.bEventFlagList[5] = false;
	}
	else if ( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.55f
		&& GetUnit()->GetClass() == CX2Unit::UC_EVE_CODE_NEMESIS )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[5] = true;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 4, m_fEventTime[4].keyInputStart, m_fEventTime[4].keyInputEnd, m_fEventTime[4].stateChange, m_InputData.oneX == true && m_InputData.pureDown == true && GetUnit()->GetClass() == CX2Unit::UC_EVE_CODE_EMPRESS, ENSI_EP_COMBO_XXdownX )
		ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange, m_InputData.oneX == true && m_InputData.pureDown == true, ENSI_COMBO_XXdownX )
		ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true &&
		( ( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) ||
		( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ), ENSI_COMBO_XXfrontX )
		ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true, ENSI_COMBO_XXX )
		ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange, m_InputData.oneZ == true &&
		(GetUnit()->GetClass() == CX2Unit::UC_EVE_CODE_EMPRESS || GetUnit()->GetClass() == CX2Unit::UC_EVE_ARCHITECTURE), ENSI_COMBO_XXZ4 )
		ELSE_IF_STATE_CHANGE_ON_EX_( 6, m_fEventTime[5].keyInputStart, m_fEventTime[5].keyInputEnd, m_fEventTime[5].stateChange, m_InputData.oneZ == true &&
		( GetUnit()->GetClass() == CX2Unit::UC_EVE_ELECTRA || GetUnit()->GetClass() == CX2Unit::UC_EVE_BATTLE_SERAPH ), ENSI_EPR_COMBO_XXZ )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER_CANNOT_ATTACKTIME( m_fWalkCancelAfter, 0.1f )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
}


void CX2GUEve::ENSI_COMBO_XXZ4_Init()
{
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			XSkinMeshReadyInBackground( L"Eve_AT_ComboX5a_mesh.X" );
			XSkinMeshReadyInBackground( L"Eve_Guardian_BladeWave01.X" );
			XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
		}
		break;
	default:
		break;
	}
}

void CX2GUEve::ENSI_COMBO_XXZ4_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{

		D3DXVECTOR3 vDamagePos = GetPos();

		vDamagePos.y += 125.f;
		if(GetIsRight() == true)
		{
			vDamagePos += (GetDirVector() * 60.f);
		}
		else
		{
			vDamagePos -= (GetDirVector() * 60.f);
		}

		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_AT_ComboX5a_Guardian", GetPos(), GetRotateDegree(), GetRotateDegree() );
		//g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_EVE, 0, true, vBonePos,
		//	GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_COMBO_XXZ4_START", GetPowerRate(), vDamagePos, GetRotateDegree(), GetRotateDegree() );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.07f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 125.f;
		if(GetIsRight() == true)
		{
			vPos.x += 60.f;
		}
		else
		{
			vPos.x -= 60.f;
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Particle_Eve_AT_ComboX5a01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_XXZ4_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	//

	CommonEventProcess();
}

void CX2GUEve::ENSI_COMBO_XXX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine" );
		D3DXVECTOR3 vBonePos1 = vBonePos;
		D3DXVECTOR3 vBonePos2 = vBonePos;
		D3DXVECTOR3 vBonePos3 = vBonePos;
		if( true == GetIsRight() )
		{
			vBonePos1 += GetDirVector() * 10.f;
			vBonePos2 += GetDirVector() * -55.f;
			vBonePos3 += GetDirVector() * 85.f;
		}
		else
		{
			vBonePos1 -= GetDirVector() * 10.f;
			vBonePos2 -= GetDirVector() * -55.f;
			vBonePos3 -= GetDirVector() * 85.f;
		}
		vBonePos1.y += 50.f;
		vBonePos2.y += -80.f;
		vBonePos3.y += -80.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_AttackX3", vBonePos1 );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_AttackX3", vBonePos2 );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_AttackX3", vBonePos3 );


	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		D3DXVECTOR3 vPos = GetPos();

		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 10.f;
		}
		else
		{
			vPos -= GetDirVector() * 10.f;
		}
		vPos.y += 85.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_AttackX3_Triangle01", vPos );

	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.0f, 240.f, 130.f);
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.43f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine" );

		if( FlushMp( 7.0f ) == true )
		{
			D3DXVECTOR3 vBonePos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01_Spine" );

			if(/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true)
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_COMBO_XXX_TRIANGLE", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate );
			}
			else
			{
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_COMBO_XXX_TRIANGLE", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate );

				if( NULL != pEffect )
					m_EveElectraSystem.AddEffectToSystem( pEffect );



			}

			PlaySound( L"Eve_FireShot2_Shot.ogg" );

		}
		else
		{
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_XXX_EventProcess()
{

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER_CANNOT_ATTACKTIME( m_fWalkCancelAfter, 0.1f )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
		{
			if( m_InputData.pureUp == true )
			{
				m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
			}
			else if( m_InputData.pureDown == true )
			{
				m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
			}
		}
		CommonEventProcess();
}




void CX2GUEve::ENSI_COMBO_XXfrontX_EventProcess()
{
	CancelComboWhenChangedDirection( 1 );


	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.57f )
	{
		StateChange( ENSI_COMBO_XXfrontXX );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.26666f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}



	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


void CX2GUEve::ENSI_COMBO_XXfrontXX_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_DamageData.hitUnitList.resize(0);

		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed.y	= 1000.f;
		m_DamageData.bReAttack		= false;
	}


	CommonFrameMove();
}

void CX2GUEve::ENSI_COMBO_XXfrontXX_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 1.0f )

		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
}





void CX2GUEve::ENSI_COMBO_XXdownX_EventProcess()
{
	CancelComboWhenChangedDirection( 1 );


	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.57f )
	{
		StateChange( ENSI_COMBO_XXdownXX );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.3f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}





void CX2GUEve::ENSI_COMBO_XXdownXX_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_DamageData.hitUnitList.resize(0);

		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed.y	= 1000.f;
		m_DamageData.bReAttack		= false;
	}


	CommonFrameMove();
}


void CX2GUEve::ENSI_COMBO_XXdownXX_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 1.0f )

		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
		CommonEventProcess();
}


// ENSI_DASH_COMBO_Z
void CX2GUEve::ENSI_DASH_COMBO_Z_Start()
{
	CommonStateStart();

	m_ahMajorMeshInstance[MajorMID_DashComboZ] = INVALID_MESH_INSTANCE_HANDLE;
    m_hEffectElectraDashComboZ = INVALID_DAMAGE_EFFECT_HANDLE;
}


void CX2GUEve::ENSI_DASH_COMBO_Z_End()
{
	m_ahMajorMeshInstance[MajorMID_DashComboZ] = INVALID_MESH_INSTANCE_HANDLE;

	if ( m_hEffectElectraDashComboZ != INVALID_DAMAGE_EFFECT_HANDLE )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectElectraDashComboZ );

	CommonStateEnd();
}



void CX2GUEve::ENSI_DASH_COMBO_Z_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if( false == GetAbsoluteInvisibility() )
		{
			D3DXVECTOR3 vRot = GetRotateDegree();
			if( false == GetIsRight() )
			{
				vRot.y -= 15.f;
			}
			else
			{
				vRot.y += 15.f;
			}

			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
			if( true == GetIsRight() )
			{
				vBonePos += GetDirVector() * 90.f;
			}
			else
			{
				vBonePos -= GetDirVector() * 90.f;
			}

			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( vRot );
				pSeq->SetAxisAngle( vRot );
			}

			if( true == GetIsRight() )
			{
				vBonePos -= GetDirVector() * 60.f;
			}
			else
			{
				vBonePos += GetDirVector() * 60.f;
			}


			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack_Small", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( vRot );
				pSeq->SetAxisAngle( vRot );
			}
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );

		//{{ 09.01.21.hoons.
		CX2DamageEffect::CEffect* pEffect = NULL;
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_Z", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
			break;
		case CX2Unit::UC_EVE_ARCHITECTURE:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_DASH_COMBO_Z", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
			break;

		case CX2Unit::UC_EVE_CODE_EMPRESS:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EP_DASH_COMBO_Z", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
			break;
		case CX2Unit::UC_EVE_ELECTRA:
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
            m_hEffectElectraDashComboZ = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
                this, L"EVE_PR_DASH_COMBO_Z", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
			break;
		case CX2Unit::UC_EVE_NASOD:
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_DASH_COMBO_Z", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
			break;
		default:
			break;
		}
		//}} hoons.

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_DashComboZ] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.2f &&
		INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_DashComboZ] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_DashComboZ] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_DashComboZ] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

    if ( CX2DamageEffect::CEffect* pEffectElectraDashComboZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraDashComboZ ) )
	{
        if ( CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = pEffectElectraDashComboZ->GetMainEffect() )
        {
		    D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
		    pMeshInstance->SetPos( vBonePos );
        }
	}


	CommonFrameMove();
}




void CX2GUEve::ENSI_DASH_COMBO_Z_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	//{{ 09.01.21.hoons. �뽬 Z �ĺ��� ����ĳ���� 2���� ���
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;

		StateChange( ENSI_DASH_COMBO_ZZ );
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.33f )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
			break;
			// oasis907 : ����� [2010.1.27] //
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			m_FrameDataFuture.stateParam.bEventFlagList[3] = true;
			break;
		case CX2Unit::UC_EVE_ELECTRA:
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			m_FrameDataFuture.stateParam.bEventFlagList[4] = true;
			break;
		default:
			break;
		}
	}
	//}} hoons.

	//{{ kimhc // 2010.1.25 //	�̺� 2�� ���� ��� �޺�
	else if ( m_FrameDataFuture.stateParam.bEventFlagList[2] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[2] = false;

		StateChange( ENSI_NS_DASH_COMBO_ZX );
	}
	else if ( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.33f )
	{
		switch ( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			m_FrameDataFuture.stateParam.bEventFlagList[2] = true;
			break;
		default:
			break;
		}
	}
	//{{ oasis907 : ����� [2010.1.27] //
	else if ( m_FrameDataFuture.stateParam.bEventFlagList[3] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[3] = false;

		StateChange( ENSI_EP_DASH_COMBO_ZZ );
	}
	//}} oasis907 : ����� [2010.1.27] //
	else if ( m_FrameDataFuture.stateParam.bEventFlagList[4] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[4] = false;

		StateChange( ENSI_EPR_DASH_COMBO_ZZ );
	}
	//}} kimhc // 2010.1.25 //	�̺� 2�� ���� ��� �޺�
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.7f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}


	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

// ENSI_DASH_COMBO_ZZ
void CX2GUEve::ENSI_DASH_COMBO_ZZ_Init()
{
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_ARCHITECTURE:
		XSkinMeshReadyInBackground( L"Eve_AT_DashComboZ2_mesh.X" );
		break;
	default:
		break;
	}
}

void CX2GUEve::ENSI_DASH_COMBO_ZZ_StartFuture()
{
	CommonStateStartFuture();

	//To do..
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"EG_DashComboZ2", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;

	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"AT_DashComboZ2", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
}
void CX2GUEve::ENSI_DASH_COMBO_ZZ_Start()
{
	CommonStateStart();

	//To do..
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnim->ChangeAnim( L"EG_DashComboZ2", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;

	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_pXSkinAnim->ChangeAnim( L"AT_DashComboZ2", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
}
void CX2GUEve::ENSI_DASH_COMBO_ZZ_End()
{
	//To do..

	CommonStateEnd();
}
void CX2GUEve::ENSI_DASH_COMBO_ZZ_FrameMove()
{
	//To do..
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		//if( false == GetAbsoluteInvisibility() )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Footsteps" );

			switch( GetUnit()->GetClass() )
			{
			case CX2Unit::UC_EVE_EXOTIC_GEAR:
			case CX2Unit::UC_EVE_CODE_NEMESIS:
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZ", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
				break;
			case CX2Unit::UC_EVE_ARCHITECTURE:
			case CX2Unit::UC_EVE_CODE_EMPRESS:
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_DASH_COMBO_ZZ", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
				break;
			default:
				break;
			}
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_COMBO_ZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true &&
		( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.53f )/* ||
															 (m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f && GetUnit()->GetClass() == CX2Unit::UC_EVE_ARCHITECTURE) )*/ )
	{
		switch ( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			{
				StateChange( ENSI_DASH_COMBO_ZZZ );
				m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
			}
			break;
		default:
			break;
		}
	}
	else if( m_InputData.oneZ == true &&
		(m_pXSkinAnimFuture->GetNowAnimationTime() < 0.53f/* || m_pXSkinAnimFuture->GetNowAnimationTime() < 0.33f*/) )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		float fSkipTime = 0.8f;

		switch ( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			fSkipTime = 0.6f; //1/3 ����
			break;
		default:
			break;
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > fSkipTime )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}


	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}
// ENSI_DASH_COMBO_ZZZ
void CX2GUEve::ENSI_DASH_COMBO_ZZZ_StartFuture()
{
	CommonStateStartFuture();

	//To do..
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"EG_DashComboZ3", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;

	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_pXSkinAnimFuture->ChangeAnim( L"AT_DashComboZ3", false );
			m_pXSkinAnimFuture->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}
}
void CX2GUEve::ENSI_DASH_COMBO_ZZZ_Start()
{
	CommonStateStart();

	//To do..
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			m_pXSkinAnim->ChangeAnim( L"EG_DashComboZ3", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;

	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
			m_pXSkinAnim->ChangeAnim( L"AT_DashComboZ3", false );
			m_pXSkinAnim->Play( CKTDGXSkinAnim::XAP_ONE_WAIT );
		} break;
	default:
		break;
	}

	m_ahMajorMeshInstance[MajorMID_DashComboZZZ] = INVALID_MESH_INSTANCE_HANDLE;
}
void CX2GUEve::ENSI_DASH_COMBO_ZZZ_End()
{
	//To do..

	m_ahMajorMeshInstance[MajorMID_DashComboZZZ] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}
void CX2GUEve::ENSI_DASH_COMBO_ZZZ_FrameMove()
{
	//To do..

	switch ( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
			{
				//if( false == GetAbsoluteInvisibility() )
				{
					D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Footsteps" );

					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ01", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ02", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ03", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ04", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
				}
			}
		}
		break;

	case CX2Unit::UC_EVE_ARCHITECTURE:
	case CX2Unit::UC_EVE_CODE_EMPRESS:
		{
    if( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
			{
				D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );

				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_DASH_COMBO_ZZZ", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );

				if( NULL != pEffect )
				{
					m_ahMajorMeshInstance[MajorMID_DashComboZZZ] = pEffect->GetMainEffectHandle();
				}
			}
		}
		break;
	default:
		break;
	}
	// 	if( m_pXSkinAnim->EventTimer( 0.01f ) == true && EventCheck( 0.01f, false ) == true &&
	// 		GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR )
	// 	{
	// 		//if( false == GetAbsoluteInvisibility() )
	// 		{
	// 			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Footsteps" );
	//
	// 			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ01", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	// 			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ02", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	// 			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ03", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	// 			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_ZZZ04", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	// 		}
	// 	}

	// 	if( m_pXSkinAnim->EventTimer( 0.23f ) == true && EventCheck( 0.23f, false ) == true &&
	// 		GetUnit()->GetClass() == CX2Unit::UC_EVE_ARCHITECTURE )
	// 	{
	// 		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
	//
	// 		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_DASH_COMBO_ZZZ", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	//
	// 		if( NULL != pEffect )
	// 		{
	// 			m_ahMajorMeshInstance[MajorMID_DashComboZZZ] = pEffect->GetMainEffectHandle();
	// 		}
	// 	}

	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.2f &&
		INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_DashComboZZZ] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_DashComboZZZ] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_DashComboZZZ] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_COMBO_ZZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	//else if( SpecialAttackEventProcess() == true )
	//{
	//}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		float fSkipTime = 1.5f;

		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			fSkipTime = 0.75f; //1/2 ����
			break;
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			fSkipTime = 1.f; //1/3 ����
			break;
		default:
			break;
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > fSkipTime )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}
	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_EVE_EXOTIC_GEAR:
	case CX2Unit::UC_EVE_CODE_NEMESIS:
		{
			if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
			{
				StateChange( USI_WAIT );
			}
			ELSE_IF_ZX_PRESSED_AFTER( m_fAttackCancelAfter )
			{
				StateChange( USI_WAIT );
			}
			SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
				BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
				WALK_CANCEL_AFTER( m_fWalkCancelAfter )
				DASH_CANCEL_AFTER( m_fDashCancelAfter )
		}
		break;
	default:
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
		break;
	}
	CommonEventProcess();
}





// ENSI_DASH_COMBO_X
void CX2GUEve::ENSI_DASH_COMBO_X_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 110.f;
		}
		else
		{
			vPos -= GetDirVector() * 110.f;
		}
		vPos.y += 30.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_DashAttackX_FireImpact01", vPos );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.13f ) )
	{
		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Dummy1_Rhand" );
		D3DXVECTOR3 vBonePos2 = GetBonePos( L"Dummy2_Lhand" );
		D3DXVECTOR3 vBonePos = vBonePos1 * 0.8f + vBonePos2 * 0.2f;
		D3DXVECTOR3 vDegree = GetRotateDegree();
		vDegree.z += 70.f;

		if( FlushMp( 8.f * m_fMPConsumeRateElectronBall ) == true )
		{
			switch ( GetUnit()->GetClass() )
			{
			case CX2Unit::UC_EVE_EXOTIC_GEAR:
			case CX2Unit::UC_EVE_CODE_NEMESIS:
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_X_FIRE_BALL", GetPowerRate() * m_fAttackPowerRateElectronBall,
						vBonePos, vDegree, vDegree, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, 1.f );

					if( NULL != pEffect )
						pEffect->SetInheritForceDownRate(true);

					PlaySound( L"Eve_FireBigShot.ogg" );
				}
				break;

			default:
				{
					CX2DamageEffect::CEffect* pEffect = NULL;
					pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_DASH_COMBO_X_FIRE_BALL", GetPowerRate() * m_fAttackPowerRateElectronBall,
						vBonePos, vDegree, vDegree, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, 1.f );
					if( NULL != pEffect )
						pEffect->SetInheritForceDownRate(true);

					if( NULL != pEffect )
						m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_DASH_COMBO_X_FIRE_BALL_AMPLIFY_FILTERED",
						L"EVE_DASH_COMBO_X_FIRE_BALL_SPECTRUM_FILTERED", L"EVE_DASH_COMBO_X_FIRE_BALL_INDUCTION_FILTERED",
						L"EVE_DASH_COMBO_X_FIRE_BALL_FUSION_FILTERED" );


					PlaySound( L"Eve_FireBigShot.ogg" );
				}
				break;
			}
		}
		else
		{
			CreateNotEnoughMPEffect( (vBonePos1+vBonePos2)*0.5f, 0.f, 0.f, vDegree.z );
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.066f ) )
	{
		m_EveElectraSystem.CreatePlane( 70.f, 2.0f, 140.f, 230.f );
	}

	CommonFrameMove();

}




void CX2GUEve::ENSI_DASH_COMBO_X_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		StateChange( ENSI_DASH_COMBO_XZ );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.33f )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
			break;
		default:
			break;
		}
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.43f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}


	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}
// ENDSI_DASH_COMBO_XZ
void CX2GUEve::ENSI_DASH_COMBO_XZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Footsteps" );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_XZ01", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_XZ02", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_XZ03", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_COMBO_XZ04", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_COMBO_XZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		float fSkipTime = 1.2f;
		switch ( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
			fSkipTime = 0.935f;
			break;
		default:
			break;
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > fSkipTime )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}


	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

// ENSI_JUMP_COMBO_Z
void CX2GUEve::ENSI_JUMP_COMBO_Z_Start()
{
	CommonStateStart();

	// to do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] = INVALID_MESH_INSTANCE_HANDLE;
}
void CX2GUEve::ENSI_JUMP_COMBO_Z_End()
{
	// to do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] = INVALID_MESH_INSTANCE_HANDLE;
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}
void CX2GUEve::ENSI_JUMP_COMBO_Z_FrameMove()
{
	// to do..
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_JUMP_ATTACK_Z_01", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] = pEffect->GetMainEffectHandle();
		}
		vPos = GetBonePos(L"Dummy1_Rhand");
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_JUMP_ATTACK_Z_02", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] = pEffect->GetMainEffectHandle();
		}
	}


	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy1_Rhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_Z_02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_JUMP_COMBO_Z_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		//if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		//{
		//	m_fCanNotJumpTime = 0.2f;
		//}
		m_fCanNotAttackTime = 0.1f; // ���ڸ� �����ϸ鼭 ����ؼ� �����ϴ� ���� ���� ���ؼ�

		StateChange( USI_JUMP_LANDING );
	}
	CommonEventProcess();
}


// ENSI_JUMP_COMBO_X
void CX2GUEve::ENSI_JUMP_COMBO_X_Start()
{
	CommonStateStart();

	// to do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] = INVALID_MESH_INSTANCE_HANDLE;
}
void CX2GUEve::ENSI_JUMP_COMBO_X_End()
{
	// to do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] = INVALID_MESH_INSTANCE_HANDLE;
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}
void CX2GUEve::ENSI_JUMP_COMBO_X_FrameMove()
{
	// to do..
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR &&*/ m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_JUMP_ATTACK_X_01", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] = pEffect->GetMainEffectHandle();
		}

		vPos = GetBonePos(L"Dummy1_Rhand");
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_JUMP_ATTACK_X_02", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy1_Rhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Jump_Attack_X_02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_JUMP_COMBO_X_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		//if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		//{
		//	m_fCanNotJumpTime = 0.2f;
		//}
		m_fCanNotAttackTime = 0.2f;

		StateChange( ENSI_JUMP_COMBO_X_LANDING );
	}

	CommonEventProcess();
}



void CX2GUEve::ENSI_JUMP_COMBO_X_LANDING_StartFuture()
{
	CommonStateStartFuture();

	m_iJumpFlyCount = 0;
	m_iDashJumpCount = 0;

	//m_PhysicParam.fDownAccel		= 0.f;
}

void CX2GUEve::ENSI_JUMP_COMBO_X_LANDING_Start()
{
	CommonStateStart();
	CreateStepDust();
}

void CX2GUEve::ENSI_JUMP_COMBO_X_LANDING_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	//else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	//{
	//	StateChange( ENSI_COMBO_Z );
	//}
	//else if( m_InputData.oneX == true )
	//{
	//	StateChange( ENSI_COMBO_X );
	//}
	//else if( SpecialAttackEventProcess() == true )
	//{
	//}
	//else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	//{
	//	StateChangeDashIfPossible();
	//}
	//else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
	//{
	//	StateChange( ENSI_WALK );
	//}
	//else if( m_InputData.pureUp == true )
	//{
	//	StateChange( ENSI_JUMP_UP );
	//	m_PhysicParam.nowSpeed.y = GetJumpSpeed();
	//	m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	//}
	//#ifdef SKILL_SHORT_CUT_KEY_TEST
	//	else if( m_InputData.oneLCtr == true && m_FrameDataFuture.syncData.m_HyperModeCount > 0 && m_fRemainHyperModeTime <= 0.0f )
	//#else
	//	else if( m_InputData.oneA == true && m_FrameDataFuture.syncData.m_HyperModeCount > 0 && m_fRemainHyperModeTime <= 0.0f )
	//#endif
	//	{
	//		StateChange( ENSI_HYPER_MODE );
	//	}
	//#ifdef SKILL_SHORT_CUT_KEY_TEST
	//	else if( m_InputData.pureC == true && FlushMp(30.0f,false) == true ) //Ư��Ű ��ȯ �۾���
	//#else
	//	else if( m_InputData.pureS == true && FlushMp(30.0f,false) == true )
	//#endif
	//	{
	//		//StateChange( ENSI_SUPER_DASH_READY );
	//	}
	//	else if( m_InputData.oneDown == true
	//		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	//		LINE_RADIUS,
	//		&m_FrameDataFuture.unitCondition.landPosition,
	//		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	//	{
	//		StateChange( ENSI_JUMP_DOWN );
	//		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	//	}
	//	else if( m_InputData.pureUp == true			|| m_InputData.pureDown == true
	//		|| m_InputData.pureRight == true		|| m_InputData.pureLeft == true
	//		|| m_InputData.pureZ == true			|| m_InputData.pureX == true
	//#ifndef SKILL_SHORT_CUT_KEY_TEST
	//		|| m_InputData.pureC == true
	//#endif //SKILL_SHORT_CUT_KEY_TEST
	//#ifdef SKILL_SHORT_CUT_KEY_TEST
	//		|| m_InputData.pureLCtr == true
	//		|| m_InputData.pureC == true  //Ư��Ű ��ȯ �۾���
	//#else
	//		|| m_InputData.pureA == true
	//		|| m_InputData.pureS == true
	//#endif
	//		)
	//	{
	//		StateChange( ENSI_WAIT, false );
	//	}



	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

// ENSI_DASH_JUMP_COMBO_Z
void CX2GUEve::ENSI_DASH_JUMP_COMBO_Z_Start()
{
	CommonStateStart();

	// to do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_Z_End()
{
	// to do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] = INVALID_MESH_INSTANCE_HANDLE;
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_Z_FrameMove()
{
    //if( m_pXSkinAnim->EventTimerOneshot( 0) )
	//{
	//	m_DamageData.hitUnitList.resize(0);

	//	m_DamageData.reActType		= CX2DamageManager::RT_DOWN;
	//}
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR && */m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_DASH_JUMP_COMBO_Z1_01", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] = pEffect->GetMainEffectHandle();
		}

		vPos = GetBonePos(L"Dummy1_Rhand");
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_DASH_JUMP_COMBO_Z1_02", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy1_Rhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_Z_02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_Z_EventProcess()
{
	//#ifdef EVE_FIRST_CHANGE_JOB
	//	if( m_InputData.oneZ == true )
	//	{
	//		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	//	}
	//	if( m_InputData.oneX == true && m_FrameDataFuture.stateParam.bEventFlagList[0] == false )
	//	{
	//		switch( GetUnit()->GetClass() )
	//		{
	//		case CX2Unit::UC_EVE_EXOTIC_GEAR:
	//			m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	//			break;
	//		}
	//	}
	//#else
	if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.13333f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	//#endif EVE_FIRST_CHANGE_JOB

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.3f )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_ZZ );
	}
	//#ifdef EVE_FIRST_CHANGE_JOB
	//	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.3f )
	//	{
	//		StateChange( ENSI_DASH_JUMP_COMBO_ZX );
	//	}
	//#endif EVE_FIRST_CHANGE_JOB
	// oasis907 : ����� [2010.1.27] //
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.266f )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			StateChange( ENSI_EP_JUMP_COMBO_ZX );
			break;
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			{
				m_iBoostAttackCount = 0;
				m_bBoostUpAttack = false;
				StateChangeBoostAttackIfPossible();
			}
			break;
		default:
			break;
		}
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.oneX == true ) // 0.266f
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
			break;
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
			break;
		default:
			break;
		}
	}

	CommonEventProcess();
}
// ENSI_DASH_JUMP_COMBO_ZZ
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZ_Start()
{
	CommonStateStart();

	// to do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZ_End()
{
	// to do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] = INVALID_MESH_INSTANCE_HANDLE;
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZ_FrameMove()
{
	//if( m_pXSkinAnim->EventTimer( 0.135f ) == true && EventCheck( 0.135f, false ) == true )
	//{
	//	m_DamageData.hitUnitList.resize(0);

	//	m_DamageData.reActType		= CX2DamageManager::RT_DOWN;
	//}
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR && */m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_DASH_JUMP_COMBO_Z2_01", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] = pEffect->GetMainEffectHandle();
		}

		vPos = GetBonePos(L"Dummy1_Rhand");
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_DASH_JUMP_COMBO_Z2_02", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy1_Rhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZ_02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZ_EventProcess()
{
	if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.1333f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.333f )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_ZZZ );
	}
	// oasis907 : ����� [2010.1.27] //
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.266f )
	{
		StateChange( ENSI_EP_JUMP_COMBO_ZX );
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.oneX == true) // 0.266f
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
			break;
		default:
			break;
		}
	}


	CommonEventProcess();
}
// ENSI_DASH_JUMP_COMBO_ZZZ
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZZ_Start()
{
	CommonStateStart();

	// to do..
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZZ_End()
{
	// to do..
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] = INVALID_MESH_INSTANCE_HANDLE;
	if( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] != INVALID_MESH_INSTANCE_HANDLE )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02]);
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZZ_FrameMove()
{
	//if( m_pXSkinAnim->EventTimer( 0.135f ) == true && EventCheck( 0.135f, false ) == true )
	//{
	//	m_DamageData.hitUnitList.resize(0);

	//	m_DamageData.reActType		= CX2DamageManager::RT_DOWN;
	//}
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		/*GetUnit()->GetClass() == CX2Unit::UC_EVE_EXOTIC_GEAR && */m_sEGQueensThrone.m_bEnable == true )
	{
		D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_DASH_JUMP_COMBO_Z3_01", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] = pEffect->GetMainEffectHandle();
		}

		vPos = GetBonePos(L"Dummy1_Rhand");
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_DASH_JUMP_COMBO_Z3_02", m_sEGQueensThrone.m_fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy2_Lhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f && INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vPos = GetBonePos(L"Dummy1_Rhand");
			pMeshInst->SetPos( vPos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Dash_Jump_Combo_ZZZ_02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZZZ_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		//if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		//{
		//	m_fCanNotJumpTime = 0.2f;
		//}
		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}

// ENSI_DASH_JUMP_COMBO_ZX
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );

		CX2DamageEffect::CEffect* pEffect = NULL;
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_DASH_JUMP_COMBO_ZX", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_DashJumpComboZX] = pEffect->GetMainEffectHandle();
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.01f &&
		INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_DashJumpComboZX] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_DashJumpComboZX] );
		if( NULL != pMeshInst )
		{
			//D3DXVECTOR3 vBonePos	= GetBonePos( L"Dummy2_Lhand" );
			//D3DXVECTOR3 vAngle		= ;
			//m_ahMajorMeshInstance[MajorMID_DashJumpComboZX]->SetPos( vBonePos );
			//m_ahMajorMeshInstance[MajorMID_DashJumpComboZX]->SetAngleDegree( vAngle );
			// 09.01.21.hoons.�̺κ��� �޽��� pos,rotate 2������ �ǽð����� ������ �Ǿ�� �ϱ⶧����..
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
			pMeshInst->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_DashJumpComboZX] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZX_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_DASH_JUMP_COMBO_ZX_End()
{
	m_ahMajorMeshInstance[MajorMID_DashJumpComboZX] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}


void CX2GUEve::ENSI_DASH_JUMP_COMBO_X_Start()
{
	CommonStateStart();

	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
	m_fPlaneZAngle = 0.f;
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_X_End()
{

	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02, INVALID_PARTICLE_SEQUENCE_HANDLE );

	CommonStateEnd();
}


void CX2GUEve::ENSI_DASH_JUMP_COMBO_X_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );

		if( true == GetIsRight() )
		{
			vBonePos += GetDirVector() * -30.f;
		}
		else
		{
			vBonePos -= GetDirVector() * -30.f;
		}
		vBonePos.y += 20.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vBonePos, 80, 80, -1, -1, 0.4f );
		if( NULL != pSeq )
		{
			//pSeq->SetAddRotate( GetRotateDegree() );		// billboard�� ��ƼŬ�� �̷��� ������ ���������� billboard�� �̻��ϰ� �ȴ�.
			//pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01, pSeq->GetHandle() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.266f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );

		if( true == GetIsRight() )
		{
			vBonePos += GetDirVector() * 10.f;
		}
		else
		{
			vBonePos -= GetDirVector() * 10.f;
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_DashJumpattackX01", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02, pSeq->GetHandle() );
		}

		vBonePos = GetBonePos( L"Bip01_Spine" );

		if( FlushMp( 8.0f * m_fMPConsumeRateElectronBall ) == true )
		{
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_COMBO_X_FIRE_BALL", GetPowerRate() * m_fAttackPowerRateElectronBall,
				vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate );

			if( NULL != pEffect )
				pEffect->SetInheritForceDownRate(true);

			if( NULL != pEffect )
				m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_COMBO_X_FIRE_BALL_AMPLIFY_FILTERED",
				L"EVE_COMBO_X_FIRE_BALL_SPECTRUM_FILTERED", L"EVE_COMBO_X_FIRE_BALL_INDUCTION_FILTERED", L"EVE_COMBO_X_FIRE_BALL_FUSION_FILTERED" );


			PlaySound( L"Eve_FireShot1.ogg" );
		}
		else
		{
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}
	}

    else if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.0f, 240.f, 130.f );
	}


	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
			if( true == GetIsRight() )
			{
				vBonePos += GetDirVector() * -30.f;
			}
			else
			{
				vBonePos -= GetDirVector() * -30.f;
			}
			vBonePos.y += 20.f;

			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}






	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );

			if( true == GetIsRight() )
			{
				vBonePos += GetDirVector() * 10.f;
			}
			else
			{
				vBonePos -= GetDirVector() * 10.f;
			}

			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_DASH_JUMP_COMBO_X_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}



	CommonFrameMove();

}


void CX2GUEve::ENSI_DASH_JUMP_COMBO_X_EventProcess()
{
	if( m_InputData.oneX == true && m_FrameDataFuture.stateParam.bEventFlagList[1] == false )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_EXOTIC_GEAR:
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_NEMESIS:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
			break;
		default:
			break;
		}
	}
	if( m_InputData.oneZ == true && m_FrameDataFuture.stateParam.bEventFlagList[0] == false )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
			break;
		default:
			break;
		}
	}

	if( true == IsOnSomethingFuture() )
	{
		//if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		//{
		//	m_fCanNotJumpTime = 0.2f;
		//}
		StateChange( USI_JUMP_LANDING );
	}

	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			StateChange( ENSI_DASH_JUMP_COMBO_XZ );
			break;
		case CX2Unit::UC_EVE_BATTLE_SERAPH:
			StateChange( ENSI_EBS_DASH_JUMP_COMBO_XZ );
			break;
		default:
			break;
		}

	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_XZ_Init()
{
	XSkinMeshReadyInBackground( L"Eve_AT_DashJumpComboXa_Mesh_G.X" );
	XSkinMeshReadyInBackground( L"Eve_Guardian_BladeWave01.X" );

	//TextureReadyInBackground( L"AT_ComboX3a_Tex.tga" );
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_XZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			{
				D3DXVECTOR3 vBonePos = GetPos();
				vBonePos.y += 290.f;

				g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_AT_DashJumpComboXa_Guardian", GetPos(), GetRotateDegree(), GetRotateDegree() );

				CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Particle_Eve_AT_Combo01", vBonePos );
				if( NULL != pSeq )
				{
					pSeq->SetAddRotate( GetRotateDegree() );
					pSeq->SetAxisAngle( GetRotateDegree() );
					pSeq->SetLandPosition( GetLandPosition().y );
				}

				//m_vSummonedUnitBonePos = pMeshInst->GetXSkinAnim()->GetCloneFramePosition(L"Bip01_Neck");
				//m_vSummonedUnitBonePos += vBonePos;
			}
			break;
		default:
			break;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		m_vSummonedUnitBonePos = GetPos();
		m_vSummonedUnitBonePos.y += 150.f;

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_DASH_JUMP_COMBO_XZ", GetPowerRate(), m_vSummonedUnitBonePos, GetRotateDegree(), GetRotateDegree() );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{

		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_AT_DashJumpComboXa_BladeWave01", m_vSummonedUnitBonePos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_XZ_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		//if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		//{
		//	m_fCanNotJumpTime = 0.2f;
		//}
		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_DASH_JUMP_COMBO_XZ_End()
{
	CommonStateEnd();
}

void CX2GUEve::ENSI_SI1_ILLUSION_STRIKE_Init()
{
	XSkinMeshReadyInBackground( L"Eve_IllusionStrike_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_IllusionStrike_Mesh02.X" );
	XSkinMeshReadyInBackground( L"Eve_IllusionStrike_Mesh03.X" );
	XSkinMeshReadyInBackground( L"Eve_IllusionStrike_Mesh04.X" );
	XSkinMeshReadyInBackground( L"Eve_IllusionStrike_Mesh05.X" );
	XSkinMeshReadyInBackground( L"Eve_IllusionStrike_Mesh06.X" );

}

void CX2GUEve::ENSI_SI1_ILLUSION_STRIKE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Bip01_L_Finger2" );
		D3DXVECTOR3 vBonePos2 = GetBonePos( L"Bip01_R_Finger2" );

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_IllusionStrike01", vBonePos1 );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01, pSeq->GetHandle() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_IllusionStrike02", vBonePos2 );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02, pSeq->GetHandle() );
		}



		CKTDXDeviceXSkinMesh* pSkinMesh1 = NULL;
		CKTDXDeviceXSkinMesh* pSkinMesh2 = NULL;
		CKTDXDeviceXET* pXET = NULL;

		// ���� �����ϰ� �ִ� ���� ������ �޽ð� � ������ �����´�.
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			if ( pCX2Eqip == NULL )
				continue;

			if( CX2Unit::EP_WEAPON_HAND != pCX2Eqip->GetEqipPos() )
				continue;

			if( NULL != pCX2Eqip->GetItem() &&
				NULL != pCX2Eqip->GetItem()->GetItemTemplet()
                )
			{
				if( true == pCX2Eqip->GetItem()->GetItemTemplet()->GetFashion() )
				{
					pSkinMesh1 = pCX2Eqip->GetNormalModel(0);
					pSkinMesh2 = pCX2Eqip->GetNormalModel(1);
					pXET = pCX2Eqip->GetNormalTexChangeXET();
					break;
				}
				else
				{
					pSkinMesh1 = pCX2Eqip->GetNormalModel(0);
					pSkinMesh2 = pCX2Eqip->GetNormalModel(1);
					pXET = pCX2Eqip->GetNormalTexChangeXET();
				}
			}
		}

		// ����Ʈ�� ���� �޽ø� ���Ƴ����
		D3DXVECTOR3 vPos = GetPos();
		WCHAR wszText[128] = L"";
		for( int i=1; i<7; i++ )
		{
			StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"EVE_ILLUSION_STRIKE_0%d", i );

			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, wszText,
				GetPowerRate(), vPos,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );




            CKTDGXMeshPlayer::CXMeshInstance *pEffectMainEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL;
			if( NULL == pEffectMainEffect ||
				NULL == pEffectMainEffect->GetXSkinAnim() )
				continue;

			CKTDGXSkinAnim* pSkinAnim = pEffectMainEffect->GetXSkinAnim();
			switch( i )
			{
			case 1:
			case 2:
			case 3:
				{
					if( NULL != pSkinMesh1 )
					{
						pSkinAnim->RemoveModelXSkinMesh( pSkinAnim->GetAnimXSkinMesh() );
						pSkinAnim->AddModelXSkinMesh( pSkinMesh1, NULL, NULL, pXET, false );
					}
				} break;

			case 4:
			case 5:
			case 6:
				{
					if( NULL != pSkinMesh2 )
					{
						pSkinAnim->RemoveModelXSkinMesh( pSkinAnim->GetAnimXSkinMesh() );
						pSkinAnim->AddModelXSkinMesh( pSkinMesh2, NULL, NULL, pXET, false );
					}
				} break;
			}
		} // for(i)


	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.63f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 120.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


		SetEnableAttackBox( L"Creature2", true );
		SetEnableAttackBox( L"Creature1", false );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.f ) )
	{
		SetEnableAttackBox( L"Creature2", false );
		SetEnableAttackBox( L"Creature1", true );

		m_DamageData.hitUnitList.resize(0);
		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed = D3DXVECTOR2( 1500.f, 1000.f );
	}


	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger2" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}


	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}



	CommonFrameMove();
}

void CX2GUEve::ENSI_SI1_ILLUSION_STRIKE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI1_ILLUSION_STRIKE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Bip01_L_Finger2" );
		D3DXVECTOR3 vBonePos2 = GetBonePos( L"Bip01_R_Finger2" );

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_IllusionStrike01", vBonePos1 );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01, pSeq->GetHandle() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_IllusionStrike02", vBonePos2 );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02, pSeq->GetHandle() );
		}



		CKTDXDeviceXSkinMesh* pSkinMesh1 = NULL;
		CKTDXDeviceXSkinMesh* pSkinMesh2 = NULL;
		CKTDXDeviceXET* pXET = NULL;

		// ���� �����ϰ� �ִ� ���� ������ �޽ð� � ������ �����´�.
		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			if ( pCX2Eqip == NULL )
				continue;

			if( CX2Unit::EP_WEAPON_HAND != pCX2Eqip->GetEqipPos() )
				continue;

			if( NULL != pCX2Eqip->GetItem() &&
				NULL != pCX2Eqip->GetItem()->GetItemTemplet()
                )
			{
				if( true == pCX2Eqip->GetItem()->GetItemTemplet()->GetFashion() )
				{
					pSkinMesh1 = pCX2Eqip->GetNormalModel(0);
					pSkinMesh2 = pCX2Eqip->GetNormalModel(1);
					pXET = pCX2Eqip->GetNormalTexChangeXET();
					break;
				}
				else
				{
					pSkinMesh1 = pCX2Eqip->GetNormalModel(0);
					pSkinMesh2 = pCX2Eqip->GetNormalModel(1);
					pXET = pCX2Eqip->GetNormalTexChangeXET();
				}
			}
		}

		// ����Ʈ�� ���� �޽ø� ���Ƴ����
		D3DXVECTOR3 vPos = GetPos();
		WCHAR wszText[128] = L"";
		for( int i=1; i<7; i++ )
		{
			StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"EVE_ILLUSION_STRIKE_0%d", i );

			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, wszText,
				GetPowerRate(), vPos,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );





            CKTDGXMeshPlayer::CXMeshInstance *pEffectMainEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL;
			if( NULL == pEffectMainEffect ||
				NULL == pEffectMainEffect->GetXSkinAnim() )
				continue;

			CKTDGXSkinAnim* pSkinAnim = pEffectMainEffect->GetXSkinAnim();
			switch( i )
			{
			case 1:
			case 2:
			case 3:
				{
					if( NULL != pSkinMesh1 )
					{
						pSkinAnim->RemoveModelXSkinMesh( pSkinAnim->GetAnimXSkinMesh() );
						pSkinAnim->AddModelXSkinMesh( pSkinMesh1, NULL, NULL, pXET, false );
					}
				} break;

			case 4:
			case 5:
			case 6:
				{
					if( NULL != pSkinMesh2 )
					{
						pSkinAnim->RemoveModelXSkinMesh( pSkinAnim->GetAnimXSkinMesh() );
						pSkinAnim->AddModelXSkinMesh( pSkinMesh2, NULL, NULL, pXET, false );
					}
				} break;
			}
		} // for(i)


	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.63f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 120.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


		SetEnableAttackBox( L"Creature2", true );
		SetEnableAttackBox( L"Creature1", false );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.f ) )
	{
		SetEnableAttackBox( L"Creature2", false );
		SetEnableAttackBox( L"Creature1", true );

		m_DamageData.hitUnitList.resize(0);
		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed = D3DXVECTOR2( 1500.f, 1000.f );
	}


	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger2" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}


	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Finger2" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STRIKE_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}



	CommonFrameMove();
}

void CX2GUEve::ENSI_SI1_ILLUSION_STRIKE_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_Init()
{
	XSkinMeshReadyInBackground( L"Eve_Sp2a_IllusionStinger_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_Sp2a_IllusionStinger_Mesh02.X" );
}



void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_Start()
{
	CommonStateStart();

	m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = INVALID_MESH_INSTANCE_HANDLE;

	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
}

void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_End()
{
	m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = INVALID_MESH_INSTANCE_HANDLE;

	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, INVALID_PARTICLE_SEQUENCE_HANDLE );

	CommonStateEnd();
}




void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_vecBoneILLUSIONSTINGER1 = GetBonePos( L"Dummy2_Lhand" );
		m_vecBoneILLUSIONSTINGER2 = GetBonePos( L"Dummy1_Rhand" );

		D3DXVECTOR3 vRotDegree = GetRotateDegree();

		CX2DamageEffect::CEffect* pDamageEffect1 = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_01",
			GetPowerRate(), m_vecBoneILLUSIONSTINGER1,
			vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y );


		CX2DamageEffect::CEffect* pDamageEffect2 = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_02",
			GetPowerRate(), m_vecBoneILLUSIONSTINGER2,
			vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y );


		if( NULL != pDamageEffect1 )
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = pDamageEffect1->GetMainEffectHandle();
		}

		if( NULL != pDamageEffect2 )
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = pDamageEffect2->GetMainEffectHandle();
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.26f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, pSeq->GetHandle() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_IllusionStinger02", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, pSeq->GetHandle() );
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 140.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vPos );
		//pSeq->SetAxisAngle( GetRotateDegree() );
		//pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );



		// �Ҵ� ��
		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -100.f;
		}
		else
		{
			vPos -= GetDirVector() * -100.f;
		}
		vPos.y += 100.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}



		// ���� �Ҵ� ��
		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -150.f;
		}
		else
		{
			vPos -= GetDirVector() * -150.f;
		}
		vPos.y += 100.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack_Small", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		UpDownCrashCamera( 3.f, 0.2f );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		const float fEffTime[] =
		{
			0.833f - 0.6f,
			0.9f - 0.6f,
			0.933f - 0.6f,
			1.033f - 0.6f,
			1.1f - 0.6f,
			1.133f - 0.6f,
			1.133f - 0.6f,
			1.233f - 0.6f,
			1.3f - 0.6f,
			1.4f - 0.6f,
			1.5f - 0.6f,
			1.566f - 0.6f,
			1.6f - 0.6f,
			1.633f - 0.6f,
			1.7f - 0.6f,
		};

		D3DXVECTOR3 vEffPos[] =
		{
			D3DXVECTOR3(	135,	117,	0		),
			D3DXVECTOR3(	183,	62,		-45		),
			D3DXVECTOR3(	249,	149,	38		),
			D3DXVECTOR3(	241,	74,		-31		),
			D3DXVECTOR3(	327,	138,	0		),
			D3DXVECTOR3(	196,	158,	15		),
			D3DXVECTOR3(	306,	52,		38		),
			D3DXVECTOR3(	381,	194,	0		),
			D3DXVECTOR3(	373,	98,		-45		),
			D3DXVECTOR3(	475,	74,		-27		),
			D3DXVECTOR3(	446,	132,	39		),
			D3DXVECTOR3(	375,	33,		17		),
			D3DXVECTOR3(	488,	162,	-50		),
			D3DXVECTOR3(	534,	118,	21		),
			D3DXVECTOR3(	600,	100,	0		),
		};

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
		{
			for(int i=0; i<std::ssize( vEffPos ); ++i)
				vEffPos[i].x = ( vEffPos[i].x - 135.f) * 0.8f + 135.f;
		}

		D3DXVECTOR3 vRotDegree = GetRotateDegree();

		for( int i=0; i<std::ssize( vEffPos ); i++ )
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += vEffPos[i].y;
			vPos += GetZVector() * vEffPos[i].z;
			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * vEffPos[i].x;
			}
			else
			{
				vPos -= GetDirVector() * vEffPos[i].x;
			}

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
				g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_EXPLOSION_MEMO",
				GetPowerRate(), vPos, vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y, true, fEffTime[i] );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_EXPLOSION",
				GetPowerRate(), vPos, vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y, true, fEffTime[i] );

		}

	}




	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_IllusionStinger01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_IllusionStinger01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
			{
				D3DXVECTOR3 vecDir = vBonePos - m_vecBoneILLUSIONSTINGER1;
				vecDir *= 0.8f;
				vBonePos = m_vecBoneILLUSIONSTINGER1 + vecDir;
			}
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}


	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_IllusionStinger02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_IllusionStinger02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
			{
				D3DXVECTOR3 vecDir = vBonePos - m_vecBoneILLUSIONSTINGER2;
				vecDir *= 0.8f;
				vBonePos = m_vecBoneILLUSIONSTINGER2 + vecDir;
			}
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}

	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}


	CommonFrameMove();

}

void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_HYPER_Start()
{
	CommonStateStart();

	m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = INVALID_MESH_INSTANCE_HANDLE;

	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, INVALID_PARTICLE_SEQUENCE_HANDLE );

}

void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_HYPER_End()
{
	m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = INVALID_MESH_INSTANCE_HANDLE;
	m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = INVALID_MESH_INSTANCE_HANDLE;

	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
	SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, INVALID_PARTICLE_SEQUENCE_HANDLE );

	CommonStateEnd();
}



void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_vecBoneILLUSIONSTINGER1 = GetBonePos( L"Dummy2_Lhand" );
		m_vecBoneILLUSIONSTINGER2 = GetBonePos( L"Dummy1_Rhand" );

		D3DXVECTOR3 vRotDegree = GetRotateDegree();

		CX2DamageEffect::CEffect* pDamageEffect1 = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_01",
			GetPowerRate(), m_vecBoneILLUSIONSTINGER1,
			vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y );


		CX2DamageEffect::CEffect* pDamageEffect2 = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_02",
			GetPowerRate(), m_vecBoneILLUSIONSTINGER2,
			vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y );


		if( NULL != pDamageEffect1 )
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = pDamageEffect1->GetMainEffectHandle();
		}

		if( NULL != pDamageEffect2 )
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = pDamageEffect2->GetMainEffectHandle();
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.26f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, pSeq->GetHandle() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_IllusionStinger02", vBonePos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );

			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, pSeq->GetHandle() );
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 140.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_IllusionStrike03", vPos );
		//pSeq->SetAxisAngle( GetRotateDegree() );
		//pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );



		// �Ҵ� ��
		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -100.f;
		}
		else
		{
			vPos -= GetDirVector() * -100.f;
		}
		vPos.y += 100.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}



		// ���� �Ҵ� ��
		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -150.f;
		}
		else
		{
			vPos -= GetDirVector() * -150.f;
		}
		vPos.y += 100.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DashWave01_Eve_DashAttack_Small", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		UpDownCrashCamera( 3.f, 0.2f );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		const float fEffTime[] =
		{
			0.833f - 0.6f,
			0.9f - 0.6f,
			0.933f - 0.6f,
			1.033f - 0.6f,
			1.1f - 0.6f,
			1.133f - 0.6f,
			1.133f - 0.6f,
			1.233f - 0.6f,
			1.3f - 0.6f,
			1.4f - 0.6f,
			1.5f - 0.6f,
			1.566f - 0.6f,
			1.6f - 0.6f,
			1.633f - 0.6f,
			1.7f - 0.6f,
		};

		D3DXVECTOR3 vEffPos[] =
		{
			D3DXVECTOR3(	135,	117,	0		),
			D3DXVECTOR3(	183,	62,		-45		),
			D3DXVECTOR3(	249,	149,	38		),
			D3DXVECTOR3(	241,	74,		-31		),
			D3DXVECTOR3(	327,	138,	0		),
			D3DXVECTOR3(	196,	158,	15		),
			D3DXVECTOR3(	306,	52,		38		),
			D3DXVECTOR3(	381,	194,	0		),
			D3DXVECTOR3(	373,	98,		-45		),
			D3DXVECTOR3(	475,	74,		-27		),
			D3DXVECTOR3(	446,	132,	39		),
			D3DXVECTOR3(	375,	33,		17		),
			D3DXVECTOR3(	488,	162,	-50		),
			D3DXVECTOR3(	534,	118,	21		),
			D3DXVECTOR3(	600,	100,	0		),
		};

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
		{
			for(int i=0; i<std::ssize( vEffPos ); ++i)
				vEffPos[i].x = ( vEffPos[i].x - 135.f) * 0.8f + 135.f;
		}

		D3DXVECTOR3 vRotDegree = GetRotateDegree();

		for( int i=0; i<std::ssize( vEffPos ); i++ )
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += vEffPos[i].y;
			vPos += GetZVector() * vEffPos[i].z;
			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * vEffPos[i].x;
			}
			else
			{
				vPos -= GetDirVector() * vEffPos[i].x;
			}

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
				g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_EXPLOSION_MEMO",
				GetPowerRate(), vPos, vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y, true, fEffTime[i] );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_ILLUSION_STINGER_EXPLOSION",
				GetPowerRate(), vPos, vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y, true, fEffTime[i] );
		}

	}




	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_IllusionStinger01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_IllusionStinger01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
			{
				D3DXVECTOR3 vecDir = vBonePos - m_vecBoneILLUSIONSTINGER1;
				vecDir *= 0.8f;
				vBonePos = m_vecBoneILLUSIONSTINGER1 + vecDir;
			}
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}


	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_IllusionStinger02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_IllusionStinger02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
			{
				D3DXVECTOR3 vecDir = vBonePos - m_vecBoneILLUSIONSTINGER2;
				vecDir *= 0.8f;
				vBonePos = m_vecBoneILLUSIONSTINGER2 + vecDir;
			}
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_IllusionStinger02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_01, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}


	if( INVALID_PARTICLE_SEQUENCE_HANDLE != GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02 ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq =
			g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02 ) );

		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1Nub" );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			SetHandleEveMajorParticleByEnum( EVE_MAJOR_PII_ILLUSION_STINGER_02, INVALID_PARTICLE_SEQUENCE_HANDLE );
		}
	}


	CommonFrameMove();
}

void CX2GUEve::ENSI_SI2_ILLUSION_STINGER_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI3_GENERATE_BLACKHOLE_Init()
{
	XSkinMeshReadyInBackground( L"Eve_Sp3aA_GBH_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_Sp3aA_GBH_Mesh02.X" );
	XSkinMeshReadyInBackground( L"Eve_Sp3aA_GBH_Mesh03.X" );

	TextureReadyInBackground( L"Black_Circle01.tga" );
	TextureReadyInBackground( L"SpreadLight02_Gray.tga" );
	TextureReadyInBackground( L"Eve_Sp3a_BackPack01.tga" );
}

void CX2GUEve::ENSI_SI3_GENERATE_BLACKHOLE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Bip01_Spine1" );
		D3DXVECTOR3 vBonePos2 = GetBonePos( L"Dummy2_Lhand" );
		D3DXVECTOR3 vBonePos3 = GetBonePos( L"Dummy1_Rhand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;

		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_Sp3aA_GBH01", vBonePos1, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole01] = pMeshInst->GetHandle();
		}
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_Sp3aA_GBH02", vBonePos2, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole02] = pMeshInst->GetHandle();
		}
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_Sp3aA_GBH03", vBonePos3, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole03] = pMeshInst->GetHandle();
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{


		// ����� ���θʿ��� ����Ʈ ��ġ�� ���ε��� �ö� �������� ���� ������
		D3DXVECTOR3 vDirVector = GetDirVector();
		if( 0.f != vDirVector.y )
		{
			vDirVector.y = 0.f;
			D3DXVec3Normalize( &vDirVector, &vDirVector );
		}

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += vDirVector * 220.f;
		}
		else
		{
			vPos -= vDirVector * 220.f;
		}
		vPos.y += 120.f;


		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Black_Circle_Eve_Sp3a_GBH03", vPos );
		//if( NULL != pSeq ) // ������ ��ƼŬ�� ��� �Ʒ� �ڵ� �����ϸ� �ȵ�
		//{
		//	pSeq->SetAddRotate( GetRotateDegree() );
		//	pSeq->SetAxisAngle( GetRotateDegree() );
		//}

		float fPowerRate = GetPowerRate();
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO1 ) == true )
		{
			fPowerRate *= 1.1f;
		}

		CX2DamageEffect::CEffect *pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_GENERATE_BLACKHOLE",
			fPowerRate, vPos,
			GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if(pEffect != NULL && GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO1 ) == true )
		{
			pEffect->GetBlackHoleData()->m_fDraggingRange *= 1.3f;
			pEffect->GetBlackHoleData()->m_fDraggingSpeed *= 1.3f;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vDirVector = GetDirVector();
		if( 0.f != vDirVector.y )
		{
			vDirVector.y = 0.f;
			D3DXVec3Normalize( &vDirVector, &vDirVector );
		}


		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += vDirVector * 220.f;
		}
		else
		{
			vPos -= vDirVector * 220.f;
		}
		vPos.y += 120.f;


		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Condenselight_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Condenselight_Eve_Sp3a_GBH02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Condenselight_Eve_Sp3a_GBH03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Spreadlight_Eve_Sp3a_GBH02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Spinliner_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BackLight_Light_Eve_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.57f ) )
	{
		UpDownCrashCamera( 4.f, 3.5f );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.65f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vDirVector = GetDirVector();
		if( 0.f != vDirVector.y )
		{
			vDirVector.y = 0.f;
			D3DXVec3Normalize( &vDirVector, &vDirVector );
		}

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += vDirVector * 220.f;
		}
		else
		{
			vPos -= vDirVector * 220.f;
		}
		vPos.y += 120.f;


		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpreadLight_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpreadLight_Eve_Sp3a_GBH03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Particle_Eve_Sp3a_GBH02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


	}
    else if( m_pXSkinAnim->EventTimerOneshot( 3.27f ) )
	{
		UpDownCrashCamera( 10.f, 0.4f );
	}





	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_GenBlackHole01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_GenBlackHole01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}


	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_GenBlackHole02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_GenBlackHole02] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_GenBlackHole03] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_GenBlackHole03] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole03] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}




	CommonFrameMove();
}

void CX2GUEve::ENSI_SI3_GENERATE_BLACKHOLE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI3_GENERATE_BLACKHOLE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Bip01_Spine1" );
		D3DXVECTOR3 vBonePos2 = GetBonePos( L"Dummy2_Lhand" );
		D3DXVECTOR3 vBonePos3 = GetBonePos( L"Dummy1_Rhand" );


		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;

		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_Sp3aA_GBH01", vBonePos1, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole01] = pMeshInst->GetHandle();
		}
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_Sp3aA_GBH02", vBonePos2, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole02] = pMeshInst->GetHandle();
		}
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_Sp3aA_GBH03", vBonePos3, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole03] = pMeshInst->GetHandle();
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{


		D3DXVECTOR3 vDirVector = GetDirVector();
		if( 0.f != vDirVector.y )
		{
			vDirVector.y = 0.f;
			D3DXVec3Normalize( &vDirVector, &vDirVector );
		}

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += vDirVector * 220.f;
		}
		else
		{
			vPos -= vDirVector * 220.f;
		}
		vPos.y += 120.f;


		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Black_Circle_Eve_Sp3a_GBH03", vPos );
		//if( NULL != pSeq ) // ������ ��ƼŬ�� ��� �Ʒ� �ڵ� �����ϸ� �ȵ�
		//{
		//	pSeq->SetAddRotate( GetRotateDegree() );
		//	pSeq->SetAxisAngle( GetRotateDegree() );
		//}

		float fPowerRate = GetPowerRate();
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO1 ) == true )
		{
			fPowerRate *= 1.1f;
		}
		CX2DamageEffect::CEffect *pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_GENERATE_BLACKHOLE_HYPER",
			fPowerRate, vPos,
			GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if(pEffect != NULL && GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO1 ) == true )
		{
			pEffect->GetBlackHoleData()->m_fDraggingRange *= 1.3f;
			pEffect->GetBlackHoleData()->m_fDraggingSpeed *= 1.3f;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vDirVector = GetDirVector();
		if( 0.f != vDirVector.y )
		{
			vDirVector.y = 0.f;
			D3DXVec3Normalize( &vDirVector, &vDirVector );
		}


		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += vDirVector * 220.f;
		}
		else
		{
			vPos -= vDirVector * 220.f;
		}
		vPos.y += 120.f;


		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Condenselight_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Condenselight_Eve_Sp3a_GBH02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Condenselight_Eve_Sp3a_GBH03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Spreadlight_Eve_Sp3a_GBH02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Spinliner_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"BackLight_Light_Eve_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.57f ) )
	{
		UpDownCrashCamera( 4.f, 3.5f );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.65f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vDirVector = GetDirVector();
		if( 0.f != vDirVector.y )
		{
			vDirVector.y = 0.f;
			D3DXVec3Normalize( &vDirVector, &vDirVector );
		}

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += vDirVector * 220.f;
		}
		else
		{
			vPos -= vDirVector * 220.f;
		}
		vPos.y += 120.f;


		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpreadLight_Eve_Sp3a_GBH01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpreadLight_Eve_Sp3a_GBH03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Particle_Eve_Sp3a_GBH02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


	}
    else if( m_pXSkinAnim->EventTimerOneshot( 3.27f ) )
	{
		UpDownCrashCamera( 10.f, 0.4f );
	}





	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_GenBlackHole01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_GenBlackHole01] );
		if( NULL != pMeshInst )

		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole01] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}


	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_GenBlackHole02] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_GenBlackHole02] );
		if( NULL != pMeshInst )

		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole02] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_GenBlackHole03] )
	{

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_GenBlackHole03] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_GenBlackHole03] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI3_GENERATE_BLACKHOLE_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}







void CX2GUEve::ENSI_SI1_DIMENSION_LINK_Init()
{
	XSkinMeshReadyInBackground( L"Spin_Liner02.X" );
	TextureReadyInBackground( L"Eve_Sp1a_SumonCircle01.tga" );
	TextureReadyInBackground( L"MagicsquareLight02_Red.tga" );



	XSkinMeshReadyInBackground( L"Motion_Nasod_Watch.X" );

}

void CX2GUEve::ENSI_SI1_DIMENSION_LINK_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -110.f;
		}
		else
		{
			vPos -= GetDirVector() * -110.f;
		}

		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -100.f;
		}
		else
		{
			vPos -= GetDirVector() * -100.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 90.f;
		}
		else
		{
			vPos -= GetDirVector() * 90.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}





		// ������ ��ȯ�� ���Ͱ� ������ ���� ��Ű�� �ٽ� ��ȯ�Ѵ�
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );


		// ���ο� ���� ��ȯ
		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 40.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 40.f;
			//}


			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;
				}
			}


			int iHyperMode = 0;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO2 ) == true )
			{
				// �̺� ��ȯ�� ��ũ��Ʈ�� �ϳ��� ���ϵǰ� Ű�ڵ� ���� ���� �޸� ���ΰ� ������
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_NASOD_WATCH_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, 1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );

			}
			else
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_NASOD_WATCH_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
		}

		m_fSummonedLifeTime = 20.f;
		m_timerSummoned.restart();

	}

	CommonFrameMove();

}

void CX2GUEve::ENSI_SI1_DIMENSION_LINK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI1_DIMENSION_LINK_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -110.f;
		}
		else
		{
			vPos -= GetDirVector() * -110.f;
		}

		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -100.f;
		}
		else
		{
			vPos -= GetDirVector() * -100.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 90.f;
		}
		else
		{
			vPos -= GetDirVector() * 90.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}





		// ������ ��ȯ�� ���Ͱ� ������ ���� ��Ű�� �ٽ� ��ȯ�Ѵ�
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE );
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO );


		// ���ο� ���� ��ȯ
		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 40.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 40.f;
			//}


			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;
				}
			}



			int iHyperMode = 1;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO2 ) == true )
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
			else
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_NASOD_WATCH_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}

		}

		m_fSummonedLifeTime = 20.f;
		m_timerSummoned.restart();

	}

	CommonFrameMove();


}

void CX2GUEve::ENSI_SI1_DIMENSION_LINK_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI2_DIMENSION_LINK_BLADER_Init()
{
	XSkinMeshReadyInBackground( L"Spin_Liner02.X" );
	TextureReadyInBackground( L"Eve_Sp1a_SumonCircle01.tga" );
	TextureReadyInBackground( L"MagicsquareLight02_Red.tga" );



	XSkinMeshReadyInBackground( L"Motion_Wally9th_Eve.x" );

}

void CX2GUEve::ENSI_SI2_DIMENSION_LINK_BLADER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -130.f;
		}
		else
		{
			vPos -= GetDirVector() * -130.f;
		}
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -120.f;
		}
		else
		{
			vPos -= GetDirVector() * -120.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 70.f;
		}
		else
		{
			vPos -= GetDirVector() * 70.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Light01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}



		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 100.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 100.f;
			//}



			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;

				}
			}



			int iHyperMode = 0;
			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_WALLY_9TH_EVE, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		}

	}

	CommonFrameMove();

}

void CX2GUEve::ENSI_SI2_DIMENSION_LINK_BLADER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI2_DIMENSION_LINK_BLADER_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -130.f;
		}
		else
		{
			vPos -= GetDirVector() * -130.f;
		}
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -120.f;
		}
		else
		{
			vPos -= GetDirVector() * -120.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 70.f;
		}
		else
		{
			vPos -= GetDirVector() * 70.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Light01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}



		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 100.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 100.f;
			//}



			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;

				}
			}



			int iHyperMode = 1;
			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_WALLY_9TH_EVE, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		}

	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI2_DIMENSION_LINK_BLADER_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI3_DIMENSION_LINK_GUARDIAN_Init()
{
	XSkinMeshReadyInBackground( L"Spin_Liner02.X" );
	TextureReadyInBackground( L"Eve_Sp1a_SumonCircle01.tga" );
	TextureReadyInBackground( L"MagicsquareLight02_Red.tga" );

	XSkinMeshReadyInBackground( L"Motion_Nasod_Element_Fire_Eve.x" );
}

void CX2GUEve::ENSI_SI3_DIMENSION_LINK_GUARDIAN_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}
		vPos.y += 2.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}
		vPos.y += 230.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.66f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Light01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}



		if( true == g_pX2Game->IsHost() )
		{


			vPos = GetPos();
			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * 150.f;
			}
			else
			{
				vPos -= GetDirVector() * 150.f;
			}


			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;
				}
			}


			int iHyperMode = 0;
			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_NASOD_ELEMENT_FIRE_EVE, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		}

	}

	CommonFrameMove();

}

void CX2GUEve::ENSI_SI3_DIMENSION_LINK_GUARDIAN_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}
		vPos.y += 2.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}
		vPos.y += 230.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.66f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 140.f;
		}
		else
		{
			vPos -= GetDirVector() * 140.f;
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Light01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}



		if( true == g_pX2Game->IsHost() )
		{


			vPos = GetPos();
			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * 150.f;
			}
			else
			{
				vPos -= GetDirVector() * 150.f;
			}



			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;
				}
			}



			int iHyperMode = 1;
			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_NASOD_ELEMENT_FIRE_EVE, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		}

	}

	CommonFrameMove();

}

void CX2GUEve::ENSI_SI3_DIMENSION_LINK_GUARDIAN_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : COMMON_BUFF_FrameMove
@brief : ���������� ���� ���� ��ų�� FrameMove
*/
/*virtual*/ void CX2GUEve::COMMON_BUFF_FrameMove()
{


    if( m_pXSkinAnim->EventTimerOneshot( 0.83f ) )
	{
		PlayCommonBuffMinorParticle();
		UpDownCrashCamera( 30.0f, 0.4f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.85f ) )
	{
		int	iSkillSlotIndex = 0;
		bool bSlotB = false;

		if ( true == GetSkillSlotIndexUsed( iSkillSlotIndex, bSlotB ) )
		{
			const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();

			const CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData->m_UserSkillTree.GetSkillSlot( iSkillSlotIndex, bSlotB );
			CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_NONE;

			if( NULL != pSkillSlotData )
				eSkillID = pSkillSlotData->m_eID;

			ProcessAcceleraterBuff( eSkillID, IsHyperState() );
		}
	}

	CommonFrameMove();
}

// ��ȯ�� ���Ͱ� ������ ���� ��Ų��
void CX2GUEve::SetSelfDestructSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID /*= CX2UnitManager::NUI_NONE*/ )
{
	if( NULL == g_pX2Game )
		return;


	for( int i=0; i<g_pX2Game->GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( NULL == pUnit )
			continue;

		if( CX2GameUnit::GUT_NPC != pUnit->GetGameUnitType() )
			continue;


		CX2GUNPC *pNPC = (CX2GUNPC*) pUnit;
		if( CX2UnitManager::NUI_NONE != eNPCID &&
			pNPC->GetNPCTemplet().m_nNPCUnitID != eNPCID )
		{
			continue;
		}

		if( pNPC->GetNowHp() <= 0.f )
			continue;

		// �̹� �������̸�
		if( true == pNPC->GetSelfDestruction() )
			continue;


		if( NULL != pNPC->GetNPCAI() &&
			CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )
		{
			CX2AllyNPCAI* pAllyAI = (CX2AllyNPCAI*) pNPC->GetNPCAI();
			if( GetUnitUID() == pAllyAI->GetAllyUnitUID() )
			{
				switch ( eNPCID )
				{
				case CX2UnitManager::NUI_NASOD_WATCH_EVE:
				case CX2UnitManager::NUI_NASOD_WATCH_EVE_MEMO:
					pNPC->StartSelfDestruction( 3.f );
					pNPC->AttachParticle_LUA( true, "CountDown3", "Bip01_Head", D3DXVECTOR3( 0, 80, 0 ) );
					break;
				case CX2UnitManager::NUI_ATOMIC_SHIELD_EVE:
					pNPC->StartSelfDestruction( 0.f );
					break;
				case CX2UnitManager::NUI_SI_HA_FERDINAND:
					{
						pNPC->StartSelfDestruction( 0.f );
						ResetLinkOverChargeIllusion();
					}
					break;
				default:
					break;
				}
				//}} JHKang / ������ / 2010/11/29
			}
		}
	}
}


// ��ȯ�� ���� ĳ���� ��ġ�� �����´�
void CX2GUEve::CallSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID /*= CX2UnitManager::NUI_NONE*/ )
{
	if( NULL == g_pX2Game )
		return;


	for( int i=0; i<g_pX2Game->GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( NULL == pUnit )
			continue;

		if( CX2GameUnit::GUT_NPC != pUnit->GetGameUnitType() )
			continue;

		CX2GUNPC *pNPC = (CX2GUNPC*) pUnit;

		if( CX2UnitManager::NUI_NONE != eNPCID &&
			pNPC->GetNPCTemplet().m_nNPCUnitID != eNPCID )
		{
			continue;
		}

		if( pNPC->GetNowHp() <= 0.f )
			continue;

		if( NULL != pNPC->GetNPCAI() &&
			CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )
		{
			CX2AllyNPCAI* pAllyAI = (CX2AllyNPCAI*) pNPC->GetNPCAI();
			if( GetUnitUID() == pAllyAI->GetAllyUnitUID() )
			{
				D3DXVECTOR3 vPos = GetPos();
				pNPC->SetPosition( vPos, GetIsRight() );
			}
		}
	}
}

void CX2GUEve::ResetMegaEletronCharge()
{
	if( m_sMegaElectronBallData.m_bEnable == true )
		m_sMegaElectronBallData.Reset(false, &AccessFutureFrameData());
}

void CX2GUEve::ENSI_SI1_AT_SURFACE_CUTING_Init()
{
	XSkinMeshReadyInBackground( L"Motion_Guardian_Eve.x" );
}

void CX2GUEve::ENSI_SI1_AT_SURFACE_CUTING_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -110.f;
		}
		else
		{
			vPos -= GetDirVector() * -110.f;
		}

		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -100.f;
		}
		else
		{
			vPos -= GetDirVector() * -100.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 90.f;
		}
		else
		{
			vPos -= GetDirVector() * 90.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


		// ���ο� ���� ��ȯ
		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 40.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 40.f;
			//}


			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;
				}
			}




			int iHyperMode = 0;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO10 ) == true )
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP1_EVE_MEMO, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			else
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP1_EVE, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		}

		m_fSummonedLifeTime = 20.f;
		m_timerSummoned.restart();

	}

	CommonFrameMove();


}

void CX2GUEve::ENSI_SI1_AT_SURFACE_CUTING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI1_AT_SURFACE_CUTING_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -110.f;
		}
		else
		{
			vPos -= GetDirVector() * -110.f;
		}

		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -100.f;
		}
		else
		{
			vPos -= GetDirVector() * -100.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 90.f;
		}
		else
		{
			vPos -= GetDirVector() * 90.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp1a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}


		// ���ο� ���� ��ȯ
		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 40.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 40.f;
			//}


			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;
				}
			}



			int iHyperMode = 1;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO10 ) == true )
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP1_EVE_MEMO, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			else
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP1_EVE, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );

		}

		m_fSummonedLifeTime = 20.f;
		m_timerSummoned.restart();

	}

	CommonFrameMove();

}

void CX2GUEve::ENSI_SI1_AT_SURFACE_CUTING_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI2_AT_SONIC_WAVE_Init()
{
	XSkinMeshReadyInBackground(L"Motion_Guardian_Eve.x");
}

void CX2GUEve::ENSI_SI2_AT_SONIC_WAVE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 170.f;
		}
		else
		{
			vPos -= GetDirVector() * 170.f;
		}

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Magicsquare01", vPos );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 110.f;

		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			if( true == GetIsRight() )
			{
				vPos -= GetDirVector() * 50.f;
			}
			else
			{
				vPos += GetDirVector() * 50.f;
			}



			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;

				}
			}




			int iHyperMode = 0;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO4 ) == true )
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP2_EVE_MEMO, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
			else
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP2_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
		}

	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_SI2_AT_SONIC_WAVE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_SI2_AT_SONIC_WAVE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 220.f;
		}
		else
		{
			vPos -= GetDirVector() * 220.f;
		}

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Magicsquare01", vPos );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 110.f;

		if( true == g_pX2Game->IsHost() )
		{
			vPos = GetPos();
			//if( true == GetIsRight() )
			//{
			//	vPos += GetDirVector() * 100.f;
			//}
			//else
			//{
			//	vPos -= GetDirVector() * 100.f;
			//}



			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
			D3DXVECTOR3 vNewPos( 0, 0, 0 );
			if( NULL != g_pX2Game->GetLineMap() )
			{
				vNewPos = g_pX2Game->GetLineMap()->GetLandPosition( vPos, LINE_RADIUS, &iNewLineIndex );
				if( -1 != iNewLineIndex )
				{
					vPos.x = vNewPos.x;
					vPos.y = max( vPos.y, vNewPos.y );
					vPos.z = vNewPos.z;

				}
			}




			int iHyperMode = 1;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO4 ) == true )
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP2_EVE_MEMO, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
			else
			{
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_GUARDIAN_SP2_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
		}

	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_SI2_AT_SONIC_WAVE_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI3_AT_GENOCIDE_RIPPER_Init()
{
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian01.X" );
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian02.X" );
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian03.X" );
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian04.X" );
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian05.X" );
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian06.X" );
	XSkinMeshReadyInBackground( L"Eve_AT_sp3a_GCR_Guardian07.X" );
	XSkinMeshReadyInBackground( L"Spin_Liner02.X" );

	TextureReadyInBackground( L"GroundShockWave02.tga" );
	TextureReadyInBackground( L"MagicsquareLight02_Red.tga" );
}

void CX2GUEve::ENSI_SI3_AT_GENOCIDE_RIPPER_Start()
{
	CommonStateStart();

	D3DXVECTOR3 vPos = GetPos();
	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
	pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Magicsquare01", vPos );
	if( NULL != pSeq )
	{
		pSeq->SetAddRotate( GetRotateDegree() );
		pSeq->SetAxisAngle( GetRotateDegree() );
	}
	pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp3a_Light01", vPos );
	if( NULL != pSeq )
	{
		pSeq->SetAddRotate( GetRotateDegree() );
		pSeq->SetAxisAngle( GetRotateDegree() );
	}
	pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_AT_Sp3a_GCR_PinLight01", vPos );
	if( NULL != pSeq )
	{
		pSeq->SetAddRotate( GetRotateDegree() );
		pSeq->SetAxisAngle( GetRotateDegree() );
	}
	pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_AT_Sp3a_GCR_Spinliner01", vPos );
	if( NULL != pSeq )
	{
		pSeq->SetAddRotate( GetRotateDegree() );
		pSeq->SetAxisAngle( GetRotateDegree() );
	}

	m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_SI3_AT_GENOCIDE_RIPPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 2 );

	float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();


	if( m_pXSkinAnim->EventTimerOneshot( 0.533f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}

	if( m_pXSkinAnim->EventTimerOneshot( 1.783f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Final", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_SI3_AT_GENOCIDE_RIPPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.04f, 2 );

	float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();



    if( m_pXSkinAnim->EventTimerOneshot( 0.533f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Hyper", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}

	else if( m_pXSkinAnim->EventTimerOneshot( 1.783f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Hyper_Dummy", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}

	else if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Hyper_Dummy", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}

	else if( m_pXSkinAnim->EventTimerOneshot( 0.866f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Hyper_Dummy", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}
	else if( m_pXSkinAnim->EventTimerOneshot( 1.033f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Hyper_Dummy", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}
	else if( m_pXSkinAnim->EventTimerOneshot( 1.333f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Hyper_Dummy", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}
	else if( m_pXSkinAnim->EventTimerOneshot( 1.783f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if(GetIsRight() == true)
		{
			vPos += GetDirVector() * 375.f;
		}
		else
		{
			vPos -= GetDirVector() * 375.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Genocide_Ripper_Final_Hyper", GetPowerRate() * fStatAtkRateOfSummoned, vPos, GetRotateDegree(), GetRotateDegree() );
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_SI3_AT_GENOCIDE_RIPPER_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//////////////////////////////////////////////////////////////////////////
// begin : EXOTIC GEAR SP
void CX2GUEve::ENSI_SI1_EG_HORNET_STING_Init()
{
	XSkinMeshReadyInBackground( L"EG_SP1a_HornetSting_Bomb.X" );
	XSkinMeshReadyInBackground( L"EG_SP1a_HornetSting_Cannon.X" );
	TextureReadyInBackground( L"Eve_EG_SP1_HornetSting_Piece00.tga" );

}

void CX2GUEve::ENSI_SI1_EG_HORNET_STING_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_EG_SP1a_HornetSting_Cannon", vBonePos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.47f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1" );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_EG_SP1a_HornetSting_01", vBonePos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Expl_EG_SP1a_HornetSting_01", vBonePos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Explosion_Eve_EG_Sp1a_HS02", vBonePos );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO9 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP1A_HORNETSTING_BOMB_MEMO", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP1A_HORNETSTING_BOMB", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI1_EG_HORNET_STING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI1_EG_HORNET_STING_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_EG_SP1a_HornetSting_Cannon", vBonePos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.47f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();
		vBonePos.x -= 20.f;
		vBonePos.y += 130.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_EG_SP1a_HornetSting_01", vBonePos );

		vBonePos = GetPos();
		vBonePos.x += 10.f;
		vBonePos.y += 145.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Expl_EG_SP1a_HornetSting_01", vBonePos );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Expl_EG_SP1a_HornetSting_02", vBonePos );

		vBonePos = GetBonePos( L"Bip01_L_Finger0" );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO9 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP1A_HORNETSTING_BOMB_MEMO", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP1A_HORNETSTING_BOMB", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	}


	CommonFrameMove();
}

void CX2GUEve::ENSI_SI1_EG_HORNET_STING_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

// ENSI_SP2_EXPLOSION_IMPACT
void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_Init()
{
	XSkinMeshReadyInBackground( L"EG_SP2a_ExplosionImpact_Mesh.X" );
}

void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_Start()
{
	m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateStart();
}

void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_End()
{
	m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos(L"Bip01_R_Hand");

		CX2DamageEffect::CEffect* pEffect;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_MEMO", GetPowerRate(),
				vBonePos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT", GetPowerRate(),
				vBonePos, GetRotateDegree(), GetRotateDegree() );
		}

		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = pEffect->GetMainEffectHandle();
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.88f ) )
	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 691.f;
		else
			vPos -= GetDirVector() * 691.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
		//	}
		//	else
		//	{
		//		m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;
		//	}
		//}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.96f ) )

	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 590.f;
		else
			vPos -= GetDirVector() * 590.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
		//	}
		//	else
		//	{
		//		m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;
		//	}
		//}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.04f ) )
	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 480.f;
		else
			vPos -= GetDirVector() * 480.f;


		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
		//	}
		//	else
		//	{
		//		m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;
		//	}
		//}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.12f ) )
	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 370.f;
		else
			vPos -= GetDirVector() * 370.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
		//	}
		//	else
		//	{
		//		m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;
		//	}
		//}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.2f ) )
	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 250.f;
		else
			vPos -= GetDirVector() * 250.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
		//	}
		//	else
		//	{
		//		m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = INVALID_MESH_INSTANCE_HANDLE;
		//	}
		//}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.28f ) )
	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 140.f;
		else
			vPos -= GetDirVector() * 140.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos(L"Bip01_R_Hand");
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos(L"Bip01_R_Hand");

		CX2DamageEffect::CEffect* pEffect;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_MEMO", GetPowerRate(),
				vBonePos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT", GetPowerRate(),
				vBonePos, GetRotateDegree(), GetRotateDegree() );
		}
		if( NULL != pEffect )
		{
			m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] = pEffect->GetMainEffectHandle();
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.88f ) )
	{
		//if( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] != INVALID_MESH_INSTANCE_HANDLE )
		//{
		//	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		//	if( NULL != pMeshInst )
		//	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 691.f;
		else
			vPos -= GetDirVector() * 691.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.96f ) )
	{

		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 590.f;
		else
			vPos -= GetDirVector() * 590.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.04f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 480.f;
		else
			vPos -= GetDirVector() * 480.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.12f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 370.f;
		else
			vPos -= GetDirVector() * 370.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.2f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 250.f;
		else
			vPos -= GetDirVector() * 250.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 2.28f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		if( true == GetIsRight() )
			vPos += GetDirVector() * 140.f;
		else
			vPos -= GetDirVector() * 140.f;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO3 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate() * 1.1f,
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP2A_EXPLOSION_IMPACT_EXP", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 15.0f, 0.3f );
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_ExplosionImpact01] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos(L"Bip01_R_Hand");
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI2_EG_EXPLOSION_IMPACT_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

// ENSI_SI3_QUEENS_THRONE
void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_Init()
{
	XMeshReadyInBackground( L"Eve_EG_Sp3a_QT_Text01.Y" );
	XSkinMeshReadyInBackground( L"Mesh_UpLight01.X" );
	XSkinMeshReadyInBackground( L"EG_SP3a_QueensThrone_Wing.X" );
	XSkinMeshReadyInBackground( L"EG_SP3a_QueensThrone_Wing_Weapon1.X" );
	XSkinMeshReadyInBackground( L"EG_SP3a_QueensThrone_Wing_Weapon2.X" );
	XSkinMeshReadyInBackground( L"EG_SP3a_QueensThrone_Wing_Weapon3.X" );
	XSkinMeshReadyInBackground( L"EG_SP3a_QueensThrone_Wing_Weapon4.X" );
	TextureReadyInBackground( L"GroundShockWave02.tga" );
	TextureReadyInBackground( L"QT_Text01.tga" );

}

void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_Start()
{
	CommonStateStart();

	m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = INVALID_MESH_INSTANCE_HANDLE;
}

void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_End()
{
	m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = INVALID_MESH_INSTANCE_HANDLE;

	CommonStateEnd();
}

void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();
        CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
		pMeshInst  = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_EG_Sp3a_QT_UpLight01", vBonePos, GetRotateDegree(), GetRotateDegree() );

		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Bip01_Spine1" );

		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing", vBonePos1, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = pMeshInst->GetHandle();
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();

		vPos.x -= 20.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_EG_QT02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetPos();
		vPos.x -= 20.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_EG_QT01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetPos();
		vPos.x -= 20.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_EG_Sp3a_QT_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{

		D3DXVECTOR3 vPos = GetPos();

		vPos.y += 80.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_EG_Sp3a_QT_Text01", vPos );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetBonePos(L"Bip01_Pelvis");

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_EG_QT01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetBonePos(L"Bip01_Pelvis");
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"ShockWave_Eve_EG_QT01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_QT_START_SHOCKWAVE", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree() );

		vPos = GetPos();
		vPos.y += 80.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_EG_Sp3a_QT_Text02", vPos );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();

		vPos.x -= 15.f;
		vPos.y += 155.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LED_Eve_EG_QT_Eye01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetPos();
		vPos.x -= 15.f;
		vPos.y += 155.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LED_Eve_EG_QT_Eye02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
		vPos = GetPos();
		vPos.x -= 15.f;
		vPos.y += 155.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LED_Eve_EG_QT_Eye03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 20.f, 0.3f );
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	//3�� �غ� ��ġ�� ������ ������.
    if( m_pXSkinAnim->EventTimerOneshot( 1.9f ) )
	{
		//�غ� ����Ʈ�� ���� �پ��ִ� ������ �����.
		if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] )
		{
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing]);
		}

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;

		m_sEGQueensThrone.Reset();
		m_sEGQueensThrone.m_bEnable = true;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO17 ) == true && m_sEGQueensThrone.m_dMaxLifeTime < 35.f )
			m_sEGQueensThrone.m_dMaxLifeTime += 5;

		// ������ ���δ�.(4����)
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_01", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
        {
			m_sEGQueensThrone.m_ahMajorMeshInstance[0] = pMeshInst->GetHandle();
        }
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_02", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
        {
			m_sEGQueensThrone.m_ahMajorMeshInstance[1] = pMeshInst->GetHandle();
        }
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_03", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_sEGQueensThrone.m_ahMajorMeshInstance[2] = pMeshInst->GetHandle();

			//������ �ٴ� ��ƼŬ �ٿ�����.
			m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_WING] =
				g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Booster_Eve_EG_Sp3a_GCR01", pMeshInst->GetBonePos(L"Dummy01") );
		}
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_04", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
        {
			m_sEGQueensThrone.m_ahMajorMeshInstance[3] = pMeshInst->GetHandle();
        }

		m_sEGQueensThrone.m_TimerElapsedTime.restart();
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_Eve_EG_Sp3a_QT_UpLight01", vBonePos, GetRotateDegree(), GetRotateDegree() );
		D3DXVECTOR3 vBonePos1 = GetBonePos( L"Bip01_Spine1" );



		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing", vBonePos1, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = pMeshInst->GetHandle();
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();

		vPos.x -= 20.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_EG_QT02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetPos();
		vPos.x -= 20.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Eve_EG_QT01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetPos();
		vPos.x -= 20.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_EG_Sp3a_QT_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{

		D3DXVECTOR3 vPos = GetPos();

		vPos.y += 80.f;

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_EG_Sp3a_QT_Text01", vPos );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetBonePos(L"Bip01_Pelvis");

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_EG_QT01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetBonePos(L"Bip01_Pelvis");
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"ShockWave_Eve_EG_QT01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EG_SP3A_QT_START_SHOCKWAVE", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree() );

		vPos = GetPos();
		vPos.y += 80.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_EG_Sp3a_QT_Text02", vPos );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();

		vPos.x -= 15.f;
		vPos.y += 155.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LED_Eve_EG_QT_Eye01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		vPos = GetPos();
		vPos.x -= 15.f;
		vPos.y += 155.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LED_Eve_EG_QT_Eye02", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
		vPos = GetPos();
		vPos.x -= 15.f;
		vPos.y += 155.f;
		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LED_Eve_EG_QT_Eye03", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}

		UpDownCrashCameraNoReset( 20.f, 0.3f );
	}

	if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );
			pMeshInst->SetPos( vBonePos );
		}
		else
		{
			m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}

	//3�� �غ� ��ġ�� ������ ������.
    if( m_pXSkinAnim->EventTimerOneshot( 1.9f ) )
	{
		//�غ� ����Ʈ�� ���� �پ��ִ� ������ �����.
		if( INVALID_MESH_INSTANCE_HANDLE != m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing] )
		{
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(m_ahMajorMeshInstance[MajorMID_EG_SP3_Wing]);
		}

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Spine1" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;

		m_sEGQueensThrone.Reset();
		m_sEGQueensThrone.m_bEnable = true;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO17 ) == true && m_sEGQueensThrone.m_dMaxLifeTime < 35.f )
			m_sEGQueensThrone.m_dMaxLifeTime += 5;

		// ������ ���δ�.(4����)
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_01", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
        {
			m_sEGQueensThrone.m_ahMajorMeshInstance[0] = pMeshInst->GetHandle();
        }
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_02", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
        {
			m_sEGQueensThrone.m_ahMajorMeshInstance[1] = pMeshInst->GetHandle();
        }
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_03", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_sEGQueensThrone.m_ahMajorMeshInstance[2] = pMeshInst->GetHandle();

			//������ �ٴ� ��ƼŬ �ٿ�����.
			m_sEGQueensThrone.m_ahMajorParticleInstance[EG_QueensThroneData::PP_WING] =
				g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Booster_Eve_EG_Sp3a_GCR01", pMeshInst->GetBonePos(L"Dummy01") );
		}
		pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Mesh_EG_SP3a_QueensThrone_Wing_Weapon_04", vBonePos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
        {
			m_sEGQueensThrone.m_ahMajorMeshInstance[3] = pMeshInst->GetHandle();
        }

		m_sEGQueensThrone.m_TimerElapsedTime.restart();
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SI3_EG_QUEENS_THRONE_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

// end : EXOTIC GEAR SP
//////////////////////////////////////////////////////////////////////////

void CX2GUEve::EG_QueensThroneData::Reset()
{
	m_bEnable				= false;
	m_iSelectBoneNameID		= 0;

	for( int i = 0; i < std::ssize(m_ahMajorMeshInstance); ++i )
	{
		if( m_ahMajorMeshInstance[i] != INVALID_MESH_INSTANCE_HANDLE )
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_ahMajorMeshInstance[i] );

		m_ahMajorMeshInstance[i] = INVALID_MESH_INSTANCE_HANDLE;
	}
	for( int i = 0; i < std::ssize(m_ahMajorParticleInstance); ++i )
	{
		if( m_ahMajorParticleInstance[i] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle(m_ahMajorParticleInstance[i]);
		m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}


// ENSI_SI_A_EN_CONVERSION
void CX2GUEve::ENSI_A_EN_CONVERSION_Init()
{
}

void CX2GUEve::ENSI_A_EN_CONVERSION_Start()
{
	CommonStateStart();

	// HP �� �Ҹ��� �ְ� MP�� ä����.
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EN_CONVERSION );
	if( NULL == pSkillTemplet )
	{
		StateChange( USI_WAIT );
		return;
	}

	float fTemp = 0.f;

	if ( NULL == GetUnit() )
		return;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EN_CONVERSION ) );	/// ��ų ����

	// HP ����.
	fTemp = static_cast<float>(GetMaxHp() * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_HP_REL_TO_MP_ABS_1, iSkillTempletLevel) );

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO6 ) == true )
	{
		if( GetRandomFloat() > 0.3f )
		{
			UpNowHp( -fTemp );
		}
	}
	else
	{
		UpNowHp( -fTemp );
	}

	fTemp = 0.f;

	// MP ����.
	fTemp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_HP_REL_TO_MP_ABS_2, iSkillTempletLevel );
	UpNowMp( fTemp );

}

void CX2GUEve::ENSI_A_EN_CONVERSION_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.f )
		{
			StateChange( USI_WAIT );
		}
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
// ENSI_SI_A_EN_MEGA_ELECTRONBALL
void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_Init()
{
}

void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed.x = 0.f;
	m_PhysicParam.nowSpeed.y = 0.f;
}

void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_Start()
{
	CommonStateStart();

	//���� �ð��� å�� ������ũ ����.
	// 1. �Ҹ� MP
	// 2. ������ ����
	// 3. ũ�� ����
	// �� �����.
	if( (m_sMegaElectronBallData.m_dMaxTime * 0.5) >= m_sMegaElectronBallData.m_timerElapsedTime.elapsed() )
		m_sMegaElectronBallData.m_ChargeRank = CR_RANK_1;
	else if( m_sMegaElectronBallData.m_dMaxTime >= m_sMegaElectronBallData.m_timerElapsedTime.elapsed() )
		m_sMegaElectronBallData.m_ChargeRank = CR_RANK_2;
	else if( m_sMegaElectronBallData.m_dMaxTime < m_sMegaElectronBallData.m_timerElapsedTime.elapsed() )
		m_sMegaElectronBallData.m_ChargeRank = CR_RANK_3;
	//�ִ� ����ó��
	else
		m_sMegaElectronBallData.m_ChargeRank = CR_RANK_1;

	//m_sMegaElectronBallData.Reset();

	//g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
	//g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_FullCharge.ogg" );

	m_fPlaneZAngle	= 0.f;
}
void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.66f )
	{
		m_bDisableGravity = false;
	}

	CommonFrameMoveFuture();
}
void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.22f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.0f, 240.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		// MP �Ҹ�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL );

		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL ) );	/// ��ų ����

		float fMPOrg = 15.f;
		float fMPConsumption = 0.f;
		float fPowerRate = 1.f;
		float fScale = 0.f;		// �������� ���� 2�谡 �ȴٸ� 2.f ������ ���޵Ǿ����.(����,��ú� ����Ʈ �ȿ��� OrgSize * fScale �� ����� �ϱ⶧��)
		float fRange = 1.f;

		if( NULL != pSkillTemplet )
		{
			switch( m_sMegaElectronBallData.m_ChargeRank )
			{
			case CR_RANK_1:
				{
					fMPConsumption	= fMPOrg;
					fPowerRate		= GetPowerRate();
					fScale			= 1.f;
					fRange			= 1.f;
				}
				break;
			case CR_RANK_2:
				{
					fMPConsumption	= fMPOrg + ((pSkillTemplet->GetSkillMPConsumptionValue( iSkillLevel ) - fMPOrg) / 2.f);
					fPowerRate		= GetPowerRate() * 1.5f;
					fScale			= 1.f + ((pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillLevel ) - 1) / 2.f );
					fRange			= 1.f + ((pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel ) - 1) / 2.f );
				}
				break;
			case CR_RANK_3:
				{
					fMPConsumption	= pSkillTemplet->GetSkillMPConsumptionValue( iSkillLevel );
					fPowerRate		= GetPowerRate() * 2.f;
					fScale			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillLevel );
					fRange			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel );
				}
				break;
			}
		}
		else
		{
			fMPConsumption = 20.f;
		}

		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_EVE_ARCHITECTURE:
		case CX2Unit::UC_EVE_CODE_EMPRESS:
			fMPConsumption *= m_fMPConsumeRateElectronBall;
			break;
		default:
			break;
		}

		if( FlushMp( fMPConsumption ) == true )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Hand" );

			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_MEB01", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}
			vBonePos.x += 10.f;
			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_MEB02", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}
			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Ring_Eve_SI_A_EN_MEB03", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}


			float fMemoPowerRate = 1.1f;
			fMemoPowerRate = 1.f;

			CX2DamageEffect::CEffect* pEffect = NULL;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO7 ) == true )
			{
				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_MEGA_ELECTRON_BALL", fPowerRate * m_fAttackPowerRateElectronBall * fMemoPowerRate,
					vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate * fRange );
			}
			else
			{
				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_MEGA_ELECTRON_BALL", fPowerRate * m_fAttackPowerRateElectronBall,
					vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, m_fForceDownValueRateElectronBall, m_fElectronBallRangeRate * fRange );
			}
			if( pEffect != NULL )
			{
				pEffect->SetScale(fScale,fScale,fScale);
				pEffect->SetInheritForceDownRate(true);
			}
			if( NULL != pEffect )
				m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_MEGA_ELECTRON_BALL_FILTERED",
				L"EVE_MEGA_ELECTRON_BALL_FILTERED", L"EVE_MEGA_ELECTRON_BALL_FILTERED", L"EVE_MEGA_ELECTRON_BALL_FILTERED" );


			PlaySound( L"Eve_FireShot1.ogg" );
		}
		else
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

	//float fSkipTime;
	//float fChangeTime;

	//LUA_GET_VALUE( m_LuaManager, "SKIP_TIME",	fSkipTime, 0.0f );
	//LUA_GET_VALUE( m_LuaManager, "CHANGE_TIME",	fChangeTime, 0.0f );

	//if( false == IsOnSomethingFuture() && m_pXSkinAnimFuture->GetNowAnimationTime() > fChangeTime )
	//{
	//	StateChange( ENSI_JUMP_DOWN );
	//}
	//ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	//{
	//	if( m_pXSkinAnimFuture->GetNowAnimationTime() > fSkipTime )
	//	{
	//		StateChange( ENSI_WAIT );
	//	}
	//}

	//if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	//{
	//	if( false == IsOnSomethingFuture() )
	//		StateChange( ENSI_JUMP_DOWN );
	//	else
	//		StateChange( ENSI_WAIT );
	//}
    if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		m_bDisableGravity = false;
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_A_EN_MEGA_ELECTRONBALL_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}



void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_Init()
{
	XSkinMeshReadyInBackground( L"GuardianStrike02.X" );
	XMeshReadyInBackground( L"IronScraps_Mesh01.Y" );
	TextureReadyInBackground( L"Whitecircle02.tga" );
	TextureReadyInBackground( L"AeroTornado04.tga" );
	TextureReadyInBackground( L"Condense_Light01.tga" );
	TextureReadyInBackground( L"Explosion_Light02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}
void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}

void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 2 );

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}


void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_HYPER_Init()
{
	XSkinMeshReadyInBackground( L"GuardianStrike02.X" );
	XMeshReadyInBackground( L"IronScraps_Mesh01.Y" );
	TextureReadyInBackground( L"Whitecircle02.tga" );
	TextureReadyInBackground( L"AeroTornado04.tga" );
	TextureReadyInBackground( L"Condense_Light01.tga" );
	TextureReadyInBackground( L"Explosion_Light02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}
void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_HYPER_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}

void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 2 );

	CommonFrameMove();
}



void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


void CX2GUEve::ENSI_SA_EEG_IRONSCRAPS_HYPER_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}




void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}

void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 2 );

	//������ ����� ������� �ֱ� ���� ������ ����Ʈ�� �ڵ�� ����.
	float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

    if( m_pXSkinAnim->EventTimerOneshot( 0.066f ) )
	{
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_SPACE_WRENCH", GetPowerRate() * fStatAtkRateOfSummoned,
			GetPos(), GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}


void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_HYPER_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}

void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 2 );

	//������ ����� ������� �ֱ� ���� ������ ����Ʈ�� �ڵ�� ����.
	float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

    if( m_pXSkinAnim->EventTimerOneshot( 0.066f ) )
	{
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_AT_SPACE_WRENCH_HYPER", GetPowerRate() * fStatAtkRateOfSummoned,
			GetPos(), GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


void CX2GUEve::ENSI_SA_EAT_SPACE_WRENCH_HYPER_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}

//ENSI_SA_EEG_METAL_DUST_AURA
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_Init()
{
	XSkinMeshReadyInBackground( L"SI_A_EEG_METAL_DUST_AURA_Effect.x" );
	TextureReadyInBackground( L"CutTendon_Slash01.tga" );
	TextureReadyInBackground( L"Star.tga" );
	TextureReadyInBackground( L"SlashLight.tga" );
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.333f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EEG_METAL_DUST_AURA );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO11 ) )
			SetBuffFactorToGameUnit( pSkillTemplet, 1 );
		else
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}
//ENSI_A_EEG_METAL_DUST_AURA_HYPER
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_HYPER_Init()
{
	XSkinMeshReadyInBackground( L"SI_A_EEG_METAL_DUST_AURA_Effect.x" );
	TextureReadyInBackground( L"CutTendon_Slash01.tga" );
	TextureReadyInBackground( L"Star.tga" );
	TextureReadyInBackground( L"SlashLight.tga" );
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_HYPER_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_HYPER_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.333f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EEG_METAL_DUST_AURA );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO11 ) )
			SetBuffFactorToGameUnit( pSkillTemplet, 1 );
		else
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUEve::ENSI_A_EEG_METAL_DUST_AURA_HYPER_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}



//ENSI_A_EAT_OBERON_GUARD
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_Init()
{
	//��Ű���ĸ� ����ϴ� ��ų�̶� ����Ŭ���� �˻縦 ���� �ʾƵ� ��.
	XSkinMeshReadyInBackground( L"Eve_SI_A_Oberon_Guard_Oberon_Mesh01.x" );
	XMeshReadyInBackground( L"Eve_Barrier_SideLine01.Y" );
	XMeshReadyInBackground( L"Eve_Barrier_Plane01.Y" );
	XMeshReadyInBackground( L"Eve_Barrier01.Y" );
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_FrameMove()
{
	D3DXVECTOR3 vPos;

	float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

	//ùŸ���� �ϴ� ������ ���� ������ ����Ʈ.
    if( m_pXSkinAnim->EventTimerOneshot( 0.26f ) )
	{
		vPos = GetPos();

		CX2DamageEffect::CEffect* pEffect = NULL;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO12 ) == true )
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DE_EVE_SI_A_OVERON_GUARD01_MEMO", GetPowerRate() * fStatAtkRateOfSummoned,
			vPos, GetRotateDegree(), GetRotateDegree() );
		else
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DE_EVE_SI_A_OVERON_GUARD01", GetPowerRate() * fStatAtkRateOfSummoned,
			vPos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pEffect )
		{
			if( m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] != INVALID_MESH_INSTANCE_HANDLE )
			{
				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] );
				m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] = INVALID_MESH_INSTANCE_HANDLE;
			}

			m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] = pEffect->GetMainEffectHandle();
		}

		m_sATOveronGuardData.m_TimerElapsedTime.restart();
		m_sATOveronGuardData.SetEnable(true);

		if( m_hOberonGuard != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet(m_hOberonGuard);
		}

		float fScale = m_sATOveronGuardData.GetProjectileSize();
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO12 ) == true )
			m_hOberonGuard = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EAT_OBERON_GUARD_MEMO", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, D3DXVECTOR3(fScale, fScale, fScale) );
		else
			m_hOberonGuard = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EAT_OBERON_GUARD", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, D3DXVECTOR3(fScale, fScale, fScale) );
	}
	//���۽� �ߵ� �Ǵ� ��ƼŬ 3��
    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		vPos = GetBonePos( L"Bip01_R_Finger2" );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_SI_A_Oberon_Guard01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}


	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}
//ENSI_A_EAT_OBERON_GUARD_HYPER
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_HYPER_Init()
{
	//��Ű���ĸ� ����ϴ� ��ų�̶� ����Ŭ���� �˻縦 ���� �ʾƵ� ��.
	XSkinMeshReadyInBackground( L"Eve_SI_A_Oberon_Guard_Oberon_Mesh01.x" );
	XMeshReadyInBackground( L"Eve_Barrier_SideLine01.Y" );
	XMeshReadyInBackground( L"Eve_Barrier_Plane01.Y" );
	XMeshReadyInBackground( L"Eve_Barrier01.Y" );
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_HYPER_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_HYPER_FrameMove()
{
	D3DXVECTOR3 vPos;

	float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

	//ùŸ���� �ϴ� ������ ���� ������ ����Ʈ.
    if( m_pXSkinAnim->EventTimerOneshot( 0.26f ) )
	{
		vPos = GetPos();

		CX2DamageEffect::CEffect* pEffect = NULL;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO12 ) == true )
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DE_EVE_SI_A_OVERON_GUARD01_MEMO", GetPowerRate() * fStatAtkRateOfSummoned,
			vPos, GetRotateDegree(), GetRotateDegree() );
		else
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DE_EVE_SI_A_OVERON_GUARD01", GetPowerRate() * fStatAtkRateOfSummoned,
			vPos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pEffect )
		{
			if( m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] != INVALID_MESH_INSTANCE_HANDLE )
			{
				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] );
				m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] = INVALID_MESH_INSTANCE_HANDLE;
			}

			m_ahMajorMeshInstance[MajorMID_AT_Overon_Guard_01] = pEffect->GetMainEffectHandle();
		}

		m_sATOveronGuardData.m_TimerElapsedTime.restart();
		m_sATOveronGuardData.SetEnable(true);


		if( m_hOberonGuard != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet(m_hOberonGuard);
		}

		float fScale = m_sATOveronGuardData.GetProjectileSize();
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO12 ) == true )
			m_hOberonGuard = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EAT_OBERON_GUARD_MEMO", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, D3DXVECTOR3(fScale, fScale, fScale) );
		else
			m_hOberonGuard = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EAT_OBERON_GUARD", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, D3DXVECTOR3(fScale, fScale, fScale) );
	}

	//���۽� �ߵ� �Ǵ� ��ƼŬ 3��
    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		vPos = GetBonePos( L"Bip01_R_Finger2" );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LightParticle_Eve_SI_A_Oberon_Guard01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUEve::ENSI_A_EAT_OBERON_GUARD_HYPER_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}

void CX2GUEve::ENSI_A_EEG_JUNK_BREAK_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_EEG_JUNK_BREAK_Effect_Drill_Main.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EEG_JUNK_BREAK_Effect_Drill_Side.X" );
	XSkinMeshReadyInBackground( L"Spin_Liner03.X" );
	TextureReadyInBackground( L"GroundShockWave.tga" );
	TextureReadyInBackground( L"Condense_Pulse02.tga" );
}

void CX2GUEve::ENSI_A_EEG_JUNK_BREAK_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EEG_JUNK_BREAK_Hyper_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EEG_JUNK_BREAK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 5.33f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EEG_JUNK_BREAK_Hyper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EAT_HEAVENS_FIST_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_King_Hand.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Light01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Light02.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Bottom_Light.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Hole.X" );
}

void CX2GUEve::ENSI_A_EAT_HEAVENS_FIST_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hFist = INVALID_EFFECTSET_HANDLE;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO20 ) == true )
				hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_HEAVENS_FIST_MEMO", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned );
			else
			hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_HEAVENS_FIST", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned );
			if( hFist != INVALID_EFFECTSET_HANDLE )
				m_vecEffectSetToDeleteOnDamageReact.push_back(hFist);
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.241f ) )
	{
		UpDownCrashCamera( 80.f, 1.0f );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EAT_HEAVENS_FIST_Hyper_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hFist = INVALID_EFFECTSET_HANDLE;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO20 ) == true )
				hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_HEAVENS_FIST_MEMO", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned );
			else
			hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_HEAVENS_FIST", (CX2GameUnit*)this, NULL, true, GetPowerRate() * fStatAtkRateOfSummoned );
			if( hFist != INVALID_EFFECTSET_HANDLE )
				m_vecEffectSetToDeleteOnDamageReact.push_back(hFist);
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.25f ) )
	{
		UpDownCrashCamera( 80.f, 1.0f );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EAT_HEAVENS_FIST_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 3.33f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EAT_HEAVENS_FIST_Hyper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

//////////////////////////////////////////////////////////////////////////
void CX2GUEve::MEGA_ELECTRONBALL_Data::Reset(bool bIsChargeTrue /* = true */, FrameData* pFrameDataFuture /* = NULL  */)
{
	if( bIsChargeTrue == false && pFrameDataFuture != NULL )
	{
		pFrameDataFuture->syncData.nowAction = UAI_MEGA_ELECTRONBALL_CHARGE_FAIL;
	}

	m_bEnable		= false;
	m_bIsSucCharge	= false;
	//m_dMaxTime		= 0.;

	ResetParticle();


	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_FullCharge.ogg" );
	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );


}


void CX2GUEve::MEGA_ELECTRONBALL_Data::ResetParticle()
{
	for( int i = 0; i < std::ssize(m_ahMajorParticleInstance); ++i )
	{
		if( m_ahMajorParticleInstance[i] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle(m_ahMajorParticleInstance[i]);
		m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}



void CX2GUEve::StateChangeJumpDashIfPossible()
{
	if( true == GetEnableDash() )
	{
		if ( true == m_bHighPoweredElectrics && CX2Unit::UC_EVE_BATTLE_SERAPH == GetUnit()->GetClass() )
		{
			if ( 0 < m_iDashJumpCount && true == m_bAfterJumpDash && true == m_InputData.pureDoubleRight &&
				BDS_RIGHT == m_eBoostDashState )
			{
				m_eBoostDashState = BDS_RIGHT;
				m_bEnableBoostDash = true;
			}
			else if ( 0 < m_iDashJumpCount && true == m_bAfterJumpDash && true == m_InputData.pureDoubleLeft &&
				BDS_LEFT == m_eBoostDashState)
			{
				m_eBoostDashState = BDS_LEFT;
				m_bEnableBoostDash = true;
			}
			else
			{
				if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureRight )
					m_eBoostDashState = BDS_RIGHT;
				else if ( true == m_InputData.pureDoubleLeft || true == m_InputData.pureLeft )
					m_eBoostDashState = BDS_LEFT;

				m_bEnableBoostDash = false;
			}

			if ( true == m_bEnableBoostDash )
				StateChange( ENSI_EBS_BOOST_DASH_AIR_START );
			else
				StateChange( ENSI_JUMP_DASH );
		}
		else
		{
			StateChange( ENSI_JUMP_DASH );
		}
	}
	else
	{
		StateChange( USI_JUMP_DOWN );
	}
}

//{{ kimhc // 2010.1.25 // �̺� 2�� ���� ��� �޺�

void CX2GUEve::CreateSpearBurst()
{
	m_bDisableGravity = false;
	m_PhysicParam.fDownAccel = m_PhysicParam.fGAccel * 4.f;

	D3DXVECTOR3 vPos = GetPos();
	vPos.y += 150;

	D3DXVECTOR3 vRotateDegree = GetRotateDegree();

	if ( GetIsRight() == false )
		vRotateDegree += D3DXVECTOR3( 0.0f, 180.0f, 0.0f );

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ENS_SPEAR_BURST );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_SPEAR_BURST_SPEAR",
			pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel ), vPos, vRotateDegree, vRotateDegree );

		float fGapY	= vPos.y - GetLandPosition().y;

		if ( pEffect != NULL )
		{
			float fLifeTime = fGapY / 3500;//( pMeshInstance->GetVelocity().y * -1 );
            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
            {
			    pMeshInstance->SetLandPosition( GetLandPosition().y );
			    pMeshInstance->SetNotCheckLandTime( fLifeTime + 0.17f );	// 0.17 ��ŭ�� ���θ� üũ�� �ð��� ����
            }
			m_bDoneSpearBurst	= true;
			//m_ahMajorMeshInstance[MajorPID_SPEAR_BURST01] = pEffect->GetMainEffectHandle();
		}
	}
}


void CX2GUEve::ENSI_NS_DASH_COMBO_ZX_Init()
{
	XSkinMeshReadyInBackground( L"NS_DashComboZX_Spear.x" );
}

void CX2GUEve::ENSI_NS_DASH_COMBO_ZX_Start()
{
	CommonStateStart();
	m_iDashJumpCount++;
}

void CX2GUEve::ENSI_NS_DASH_COMBO_ZX_FrameMove()
{
	float fPowerRate = GetPowerRate();

	if ( m_sEGQueensThrone.m_bEnable == true )
		fPowerRate = m_sEGQueensThrone.m_fPowerRate;

	D3DXVECTOR3 vPos = GetPos();
	vPos.y -= 50;

	if ( GetIsRight() == true )
	{
		vPos.x -= GetDirVector().x * 70;

	}
	else
	{
		vPos.x += GetDirVector().x * 70;
	}

    if ( m_pXSkinAnim->EventTimerOneshot( 0.27f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_DASH_COMBO_ZX_SPEAR", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_NS_DASH_COMBO_ZX_EventProcess()
{
	if( SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) == true )
	{
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.46f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;

		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 900, 900 );
		StateChange( ENSI_NS_DASH_COMBO_ZXX );
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.46f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( 0.46f )
	{
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 900, 900 );
		StateChange( USI_JUMP_DOWN );
	}

	if( true == IsOnSomethingFuture() && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.46f )
	{
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_NS_DASH_COMBO_ZXX_Init()
{
	XSkinMeshReadyInBackground( L"NS_DashComboZXX_Spear.x" );
}

void CX2GUEve::ENSI_NS_DASH_COMBO_ZXX_FrameMove()
{
	float fPowerRate = GetPowerRate();

	if ( m_sEGQueensThrone.m_bEnable == true )
		fPowerRate = m_sEGQueensThrone.m_fPowerRate;

	D3DXVECTOR3 vPos = GetPos();

    if ( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_DASH_COMBO_ZXX_SPEAR", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_NS_DASH_COMBO_ZXX_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		m_FrameDataFuture.syncData.bIsRight = !(m_FrameDataFuture.syncData.bIsRight);
		StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.56f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_NS_COMBO_XXZ_FrameMove()
{
	float fPowerRate = GetPowerRate();

	if ( m_sEGQueensThrone.m_bEnable == true )
		fPowerRate = m_sEGQueensThrone.m_fPowerRate;

	D3DXVECTOR3 vPos = GetPos();

    if ( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZ_SPEAR1", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZ_SPEAR2", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_NS_COMBO_XXZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( m_iSuccessiveHit_OneZ >= 1 && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.53f )
	{
		m_iSuccessiveHit_OneZ = 0;
		StateChange( ENSI_NS_COMBO_XXZZ8 );
	}
	else if ( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.53f )
	{
		m_iSuccessiveHit_OneZ++;
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_NS_COMBO_XXZZ8_FrameMove()
{
	float fPowerRate = GetPowerRate();

	if ( m_sEGQueensThrone.m_bEnable == true )
		fPowerRate = m_sEGQueensThrone.m_fPowerRate;

	D3DXVECTOR3 vPos = GetPos();

	if ( GetIsRight() == true )
		vPos -= D3DXVECTOR3( 40.0f, 0.0f, 0.0f );
	else
		vPos += D3DXVECTOR3( 40.0f, 0.0f, 0.0f );

	// 6Ÿ�� ù��° 1Ÿ
    if ( m_pXSkinAnim->EventTimerOneshot( 0.26f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8_SPEAR1", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	// 6Ÿ�� 2Ÿ
    if ( m_pXSkinAnim->EventTimerOneshot( 0.40f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8_SPEAR2", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	// 6Ÿ�� 3Ÿ
    if ( m_pXSkinAnim->EventTimerOneshot( 0.53f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8_SPEAR3", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	// 6Ÿ�� 4Ÿ
    if ( m_pXSkinAnim->EventTimerOneshot( 0.66f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8_SPEAR4", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	// 6Ÿ�� 5Ÿ
    if ( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8_SPEAR5", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	// 6Ÿ�� 6Ÿ
    if ( m_pXSkinAnim->EventTimerOneshot( 0.93f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8_SPEAR6", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_NS_COMBO_XXZZ8_EventProcess()
{
	if ( m_InputData.oneZ == true )
	{
		m_iSuccessiveHit_OneZ++;
	}

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( m_iSuccessiveHit_OneZ < 1 && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.33f )
	{
		m_iSuccessiveHit_OneZ = 0;
		StateChange( ENSI_NS_COMBO_XXZZ8Z );	// 1~2 Ÿ�� ������ ��Ÿ
	}
	else if ( m_iSuccessiveHit_OneZ >= 1 && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.33f )
	{
		m_iSuccessiveHit_OneZ = 1;
	}
	else if ( m_iSuccessiveHit_OneZ < 2 && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
	{
		//
		m_iSuccessiveHit_OneZ = 0;
		StateChange( ENSI_NS_COMBO_XXZZ8Z );	// 3~4 Ÿ ������ ������ ��Ÿ
	}

	if ( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.05f || m_pXSkinAnimFuture->IsAnimationEnd() == true)
	{
		m_iSuccessiveHit_OneZ = 0;
		StateChange( ENSI_NS_COMBO_XXZZ8Z );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )


	CommonEventProcess();
}

void CX2GUEve::ENSI_NS_COMBO_XXZZ8Z_FrameMove()
{
    if ( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		float fPowerRate = GetPowerRate();

		if ( m_sEGQueensThrone.m_bEnable == true )
			fPowerRate = m_sEGQueensThrone.m_fPowerRate;

		D3DXVECTOR3 vPos = GetPos();

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_COMBO_XXZZ8Z_Final_SPEAR", fPowerRate,
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_NS_COMBO_XXZZ8Z_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.46f )

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_EP_DASH_COMBO_ZZ_Init()
{
	XSkinMeshReadyInBackground( L"Eve_Guardian_BladeWave01.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_DashComboZ3_Mesh01.X" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}

void CX2GUEve::ENSI_EP_DASH_COMBO_ZZ_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EP_DASH_COMBO_ZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.46f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;

		StateChange( ENSI_EP_DASH_COMBO_ZZZ );
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.466667f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	/*
	ELSE_IF_ASDC_PRESSED_AFTER( 0.4f )
	{
	SpecialAttackEventProcess();
	}
	*/
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EP_DASH_COMBO_ZZZ_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_DashComboZ3_Mesh02.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_JumpAttackZX_Mesh02.X" );
}


void CX2GUEve::ENSI_EP_DASH_COMBO_ZZZ_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EP_DASH_COMBO_ZZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.666667f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_EP_JUMP_COMBO_ZX_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_JumpAttackZX_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_JumpAttackZX_Mesh02.X" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}

void CX2GUEve::ENSI_EP_JUMP_COMBO_ZX_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EP_JUMP_COMBO_ZX_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		m_fCanNotAttackTime = 0.1f; // ���ڸ� �����ϸ鼭 ����ؼ� �����ϴ� ���� ���� ���ؼ�

		StateChange( USI_JUMP_LANDING );
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_EP_COMBO_XXdownX_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_ComboX2DX_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_ComboX2DX_Mesh02.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_Grenade_Mesh01.X" );
	TextureReadyInBackground( L"ColorBallFire.tga" );
	TextureReadyInBackground( L"smoke02.tga" );
	TextureReadyInBackground( L"DarkCloud02.tga" );
	TextureReadyInBackground( L"Arme_Damage_Center.tga"	);
}

void CX2GUEve::ENSI_EP_COMBO_XXdownX_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EP_COMBO_XXdownX_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.666667f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Init()
{
	XMeshReadyInBackground( L"Eve_Atomic_Blaster_GunFire01.Y" );
	XSkinMeshReadyInBackground( L"Eve_SI_SA_ENS_Atomic_Blaster_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_SI_SA_ENS_Atomic_Blaster_Mesh02.X" );
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Start()
{
	CommonStateStart();

	for ( int i = MajorPID_ATOMIC_BLASTER_GUN; i <= MajorPID_ATOMIC_BLASTER_BULLET; i++ )
	{
		if ( m_ahMajorMeshInstance[i] != INVALID_MESH_INSTANCE_HANDLE )
		{
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_ahMajorMeshInstance[i] );
			m_ahMajorMeshInstance[i] = INVALID_MESH_INSTANCE_HANDLE;
		}
	}
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 0 );

	D3DXVECTOR3 vUnitPos	= GetPos();
	D3DXVECTOR3	vRotate		= GetRotateDegree();

    if( m_pXSkinAnim->EventTimerOneshot( 0.07f ) )
	{
		if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] == INVALID_MESH_INSTANCE_HANDLE )
		{
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_ATOMIC_BLASTER", GetPowerRate(),
				vUnitPos, vRotate, vRotate );

			if ( pEffect != NULL )
				m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] = pEffect->GetMainEffectHandle();
		}

		if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] == INVALID_MESH_INSTANCE_HANDLE )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this, L"Eve_SI_SA_ENS_Atomic_Blaster_Mesh02", vUnitPos,
				vRotate, vRotate );

			if ( pMeshInst != NULL )
			{
				m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] = pMeshInst->GetHandle();
			}
		}
	}

	if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] != INVALID_MESH_INSTANCE_HANDLE )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] );

		if ( pMeshInst != NULL )
		{
			pMeshInst->SetPos( vUnitPos );
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Pelvis" );

    if ( m_pXSkinAnim->EventTimerOneshot( 0.89f ) &&
				m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] == INVALID_MESH_INSTANCE_HANDLE )
			{
				CX2DamageEffect::CEffect* pEffectDummy = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_ATOMIC_BLASTER_DUMMY", GetPowerRate(),
					vBonePos, vRotate, vRotate );

				if ( pEffectDummy != NULL )
					m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] = pEffectDummy->GetMainEffectHandle();
			}
			else if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] != INVALID_MESH_INSTANCE_HANDLE )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInstDummy = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] );

				if ( pMeshInstDummy != NULL )
				{
					pMeshInstDummy->SetPos( vBonePos );

					pMeshInstDummy->SetRotateDegree( pMeshInstDummy->GetRotateDegree() + D3DXVECTOR3( 0.0f, 0.0f, 1.2f ) );
				}
				else
					m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] = INVALID_MESH_INSTANCE_HANDLE;
			}
		}
		else
			m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] = INVALID_MESH_INSTANCE_HANDLE;
	}

	if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] != INVALID_MESH_INSTANCE_HANDLE && m_pXSkinAnim->GetNowAnimationTime() < 1.62f )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] );

		if ( pMeshInst != NULL )
			pMeshInst->SetPos( vUnitPos );
		else
			m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] = INVALID_MESH_INSTANCE_HANDLE;
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( m_fAttackCancelAfter )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_End()
{
	CommonStateEnd();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_Init()
{
	ENSI_SA_ENS_ATOMIC_BLASTER_Init();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_Start()
{
	ENSI_SA_ENS_ATOMIC_BLASTER_Start();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 0 );

	D3DXVECTOR3 vUnitPos	= GetPos();
	D3DXVECTOR3	vRotate		= GetRotateDegree();

    if( m_pXSkinAnim->EventTimerOneshot( 0.07f ) )
	{
		if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] == INVALID_MESH_INSTANCE_HANDLE )
		{
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_ATOMIC_BLASTER", GetPowerRate(),
				vUnitPos, vRotate, vRotate );

			if ( pEffect != NULL )
				m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] = pEffect->GetMainEffectHandle();
		}

		if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] == INVALID_MESH_INSTANCE_HANDLE )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this, L"Eve_SI_SA_ENS_Atomic_Blaster_Mesh02", vUnitPos,
				vRotate, vRotate );

			if ( pMeshInst != NULL )
			{
				m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] = pMeshInst->GetHandle();
			}
		}
	}

	if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] != INVALID_MESH_INSTANCE_HANDLE )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] );

		if ( pMeshInst != NULL )
		{
			pMeshInst->SetPos( vUnitPos );
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_Pelvis" );

    if ( m_pXSkinAnim->EventTimerOneshot( 0.89f ) &&
				m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] == INVALID_MESH_INSTANCE_HANDLE )
			{
				CX2DamageEffect::CEffect* pEffectDummy = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_ATOMIC_BLASTER_DUMMY_HYPER", GetPowerRate(),
					vBonePos, vRotate, vRotate );

				if ( pEffectDummy != NULL )
					m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] = pEffectDummy->GetMainEffectHandle();
			}
			else if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] != INVALID_MESH_INSTANCE_HANDLE )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pMeshInstDummy = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] );

				if ( pMeshInstDummy != NULL )
				{
					pMeshInstDummy->SetPos( vBonePos );

					pMeshInstDummy->SetRotateDegree( pMeshInstDummy->GetRotateDegree() + D3DXVECTOR3( 0.0f, 0.0f, 1.2f ) );
				}
				else
					m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_DUMMY] = INVALID_MESH_INSTANCE_HANDLE;
			}
		}
		else
			m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_GUN] = INVALID_MESH_INSTANCE_HANDLE;
	}

	if ( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] != INVALID_MESH_INSTANCE_HANDLE && m_pXSkinAnim->GetNowAnimationTime() < 1.62f )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] );

		if ( pMeshInst != NULL )
			pMeshInst->SetPos( vUnitPos );
		else
			m_ahMajorMeshInstance[MajorPID_ATOMIC_BLASTER_BULLET] = INVALID_MESH_INSTANCE_HANDLE;
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( m_fAttackCancelAfter )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_ENS_ATOMIC_BLASTER_Hyper_End()
{
	ENSI_SA_ENS_ATOMIC_BLASTER_End();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Init()
{
	XSkinMeshReadyInBackground( L"NS_BURST_SPEAR_Spear.X" );
	XSkinMeshReadyInBackground( L"NS_BURST_SPEAR_Spear_End.X" );
	XMeshReadyInBackground( L"Hexagon_impact01.Y" );
	XMeshReadyInBackground( L"Lire_Uplight01.Y" );
	TextureReadyInBackground( L"GroundShockWave.tga" );
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Start()
{
	CommonStateStart();

	m_bDoneSpearBurst = false;
	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_FrameMove()
{
	if( IsSuperArmor() )
	{
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		CreateSpearBurst();
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_EventProcess()
{
	if( true == IsOnSomethingFuture() )
		StateChange( ENSI_A_ENS_BURST_SPEAR_Landing );

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_End()
{
	m_bDisableGravity = false;

	CommonStateEnd();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Hyper_Init()
{
	ENSI_A_ENS_BURST_SPEAR_Start();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Hyper_Start()
{
	ENSI_A_ENS_BURST_SPEAR_Start();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Hyper_FrameMove()
{
	if( IsSuperArmor() )
	{
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		CreateSpearBurst();
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Hyper_EventProcess()
{
	if( true == IsOnSomethingFuture() )
		StateChange( ENSI_A_ENS_BURST_SPEAR_Landing_HYPER );

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Hyper_End()
{
	ENSI_A_ENS_BURST_SPEAR_End();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Landing_FrameMove()
{
	if ( m_bDoneSpearBurst == false )
		CreateSpearBurst();

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Landing_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 0.3f )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Landing_Hyper_FrameMove()
{
	if ( m_bDoneSpearBurst == false )
		CreateSpearBurst();

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_ENS_BURST_SPEAR_Landing_Hyper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 0.3f )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

// oasis907 : ����� [2010.1.28] //
void CX2GUEve::ENSI_SA_EEP_ELECTRONIC_FIELD_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Electronic_Field_Mesh01.X" );
	XMeshReadyInBackground( L"Lightning_Liner01.Y");
	TextureReadyInBackground( L"Lightning_Piercing02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
	TextureReadyInBackground( L"Arme_Ring2.tga" );
	TextureReadyInBackground( L"Explosion_Sphere.tga" );
	TextureReadyInBackground( L"GroundShockWave.tga" );
	TextureReadyInBackground( L"Lire_Kick_Impact01.tga" );
	TextureReadyInBackground( L"Nasod_King_Laser03.tga" );
}
void CX2GUEve::ENSI_SA_EEP_ELECTRONIC_FIELD_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hEField = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Eve_ELECTRONIC_FIELD", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned );
			if( hEField != INVALID_EFFECTSET_HANDLE )
				m_vecEffectSetToDeleteOnDamageReact.push_back(hEField);
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.18f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -130.f;
		}
		else
		{
			vPos -= GetDirVector() * -130.f;
		}
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -120.f;
		}
		else
		{
			vPos -= GetDirVector() * -120.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 70.f;
		}
		else
		{
			vPos -= GetDirVector() * 70.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Light01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_SA_EEP_ELECTRONIC_FIELD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 1.33333f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUEve::ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Electronic_Field_Mesh01.X" );
	XMeshReadyInBackground( L"Lightning_Liner01.Y");
	TextureReadyInBackground( L"Lightning_Piercing02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
	TextureReadyInBackground( L"Arme_Ring2.tga" );
	TextureReadyInBackground( L"Explosion_Sphere.tga" );
	TextureReadyInBackground( L"GroundShockWave.tga" );
	TextureReadyInBackground( L"Lire_Kick_Impact01.tga" );
	TextureReadyInBackground( L"Nasod_King_Laser03.tga" );
}
void CX2GUEve::ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hEField = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Eve_ELECTRONIC_FIELD", (CX2GameUnit*)this, NULL, true, GetPowerRate() * fStatAtkRateOfSummoned );
			if( hEField != INVALID_EFFECTSET_HANDLE )
				m_vecEffectSetToDeleteOnDamageReact.push_back(hEField);
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.18f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -130.f;
		}
		else
		{
			vPos -= GetDirVector() * -130.f;
		}
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Spinliner01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotDegree1 = GetRotateDegree();
			D3DXVECTOR3 vRotDegree2 = GetRotateDegree();
			vRotDegree1 += D3DXVECTOR3( -180, 20, 90 );
			vRotDegree2 += D3DXVECTOR3( 180, 20, 90 );

			CKTDGParticleSystem::CParticleEvent_Rotate* pEventRotate = new CKTDGParticleSystem::CParticleEvent_Rotate();
			pEventRotate->SetActualTime( CMinMax<float>( 0.f, 0.f ) );
			pEventRotate->SetRotate( CMinMax<D3DXVECTOR3>( vRotDegree1, vRotDegree2 ) );

			pSeq->m_EventList.push_back( pEventRotate );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -120.f;
		}
		else
		{
			vPos -= GetDirVector() * -120.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Magicsquare01", vPos );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vRotateDegree = GetRotateDegree();
			vRotateDegree += D3DXVECTOR3( 0, -70, -45 );

			pSeq->SetAddRotate( vRotateDegree );
			pSeq->SetAxisAngle( vRotateDegree );
		}


		vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 70.f;
		}
		else
		{
			vPos -= GetDirVector() * 70.f;
		}
		vPos.y += 110.f;

		pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_PinLight01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 110.f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Eve_Sp2a_Light01", vPos );
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( GetRotateDegree() );
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_SA_EEP_ELECTRONIC_FIELD_Hyper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 1.33333f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EEP_SPIT_FIRE_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Spit_Fire_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Spit_Fire_Mesh02.X" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	TextureReadyInBackground( L"GuideArrow02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}

void CX2GUEve::ENSI_A_EEP_SPIT_FIRE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hSpitFire = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Eve_SPIT_FIRE", (CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned );
			if( hSpitFire != INVALID_EFFECTSET_HANDLE )
				m_vecEffectSetToDeleteOnDamageReact.push_back(hSpitFire);
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EEP_SPIT_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.666667f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EEP_SPIT_FIRE_Hyper_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Spit_Fire_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Spit_Fire_Mesh02.X" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	TextureReadyInBackground( L"GuideArrow02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}

void CX2GUEve::ENSI_A_EEP_SPIT_FIRE_Hyper_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hSpitFire = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Eve_SPIT_FIRE", (CX2GameUnit*)this, NULL, true, GetPowerRate() * fStatAtkRateOfSummoned );
			if( hSpitFire != INVALID_EFFECTSET_HANDLE )
				m_vecEffectSetToDeleteOnDamageReact.push_back(hSpitFire);
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EEP_SPIT_FIRE_Hyper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.666667f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

//}} kimhc // 2010.1.25 // �̺� 2�� ���� ��� �޺�


bool CX2GUEve::IsUpSkillSlotMegaElectronBall()
{
	bool bIsElectronBallSuccess = false;
	if( m_sMegaElectronBallData.m_bEnable == true )
	{
		//Ű�� ���ȴ��� üũ.
		switch( m_sMegaElectronBallData.GetSkillSlotID() )
		{
		case 0: if( m_InputData.pureA == false && m_InputData.oneA == false )
					bIsElectronBallSuccess = true;
			break;
		case 1: if( m_InputData.pureS == false && m_InputData.oneS == false )
					bIsElectronBallSuccess = true;
			break;
		case 2: if( m_InputData.pureD == false && m_InputData.oneD == false )
					bIsElectronBallSuccess = true;
			break;
		case 3: if( m_InputData.pureC == false && m_InputData.oneC == false )
					bIsElectronBallSuccess = true;
			break;
		case 4: if( m_InputData.pureQ == false && m_InputData.oneQ == false )
					bIsElectronBallSuccess = true;
			break;
		case 5: if( m_InputData.pureW == false && m_InputData.oneW == false )
					bIsElectronBallSuccess = true;
			break;
		case 6: if( m_InputData.pureE == false && m_InputData.oneE == false )
					bIsElectronBallSuccess = true;
			break;
		case 7: if( m_InputData.pureR == false && m_InputData.oneR == false )
					bIsElectronBallSuccess = true;
			break;

		default: // �����ÿ� ������ ������ ����� �̷�� ���� �ʾ��� ���.
			m_sMegaElectronBallData.Reset( false, &AccessFutureFrameData() );
			break;;
		}
	}

	return bIsElectronBallSuccess;
}


const CX2UserSkillTree::SkillSlotData* CX2GUEve::GetSkillSlotDataMegaElectronBall( IN const CX2UserSkillTree& cUserSkillTree_, OUT int& iSkillSlotIndexPressed_ )
{
	int iSlotIndex = (m_sMegaElectronBallData.GetSkillSlotID() > 3) ? m_sMegaElectronBallData.GetSkillSlotID()-4 : m_sMegaElectronBallData.GetSkillSlotID();
	bool bSlotB = (m_sMegaElectronBallData.GetSkillSlotID() > 3) ? true : false;

	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = cUserSkillTree_.GetSkillSlot( iSlotIndex, bSlotB );
	if( CX2SkillTree::SI_NONE == pSkillSlotData->m_eID ||
		CX2SkillTree::SI_A_EN_MEGA_ELECTRONBALL != pSkillSlotData->m_eID)
	{
		m_sMegaElectronBallData.Reset( false, &AccessFutureFrameData() );
		return NULL;
	}

	// m_sMegaElectronBallData.Reset(); <= ����ó���� �ϱ�����

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillSlotData->m_eID );
	if( NULL == pSkillTemplet )
	{
		m_sMegaElectronBallData.Reset( false, &AccessFutureFrameData() );
		return NULL;
	}

	// �ް��Ϸ�Ʈ�к� ���� ��� �Ϸ�
	m_sMegaElectronBallData.Reset( false, &AccessFutureFrameData() );
	iSkillSlotIndexPressed_ = m_sMegaElectronBallData.GetSkillSlotID();

	return pSkillSlotData;
}


//{{ kimhc // 2010.11.2 // �������� - ���� ������

/** @function : CX2GUEve::CanUseChargingBooster
@brief : ���������� �нú갡 �ߵ� �� �� �ִ����� Ȯ�� �ϴ� �Լ�
@param : ���� ����� ��ų�� Ÿ��, ������ ��� ��ųƮ��
@return : ����� �� �ִ��� ����( ����� �� ������ true )
*/
bool CX2GUEve::CanUseChargingBooster( CX2SkillTree::SKILL_TYPE eSkillType_, const IN CX2UserSkillTree& cUserSkillTree_ )
{
	// ����� ��ų�� Ÿ���� ��������� �Ǵ�
	if ( CX2SkillTree::ST_SPECIAL_ACTIVE != eSkillType_ )
		return false;

	// ���� ������ ��ų�� �ִ��� �Ǵ�
	const CX2SkillTree::SkillTemplet* pChargingBoosterTemplet = cUserSkillTree_.GetUserSkillTemplet( CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER );

	if ( NULL == pChargingBoosterTemplet )
		return false;

	// �ߵ� Ȯ���� �Ǵ� ��

	const int iSkillTempletLevel = max( 1, cUserSkillTree_.GetSkillLevel( CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER, true ) );	/// ��ų ����

	float fRateChargingBooster = pChargingBoosterTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillTempletLevel );

	if ( GetRandomFloat() > fRateChargingBooster )
		return false;

	if( false == m_ChargingBoosterSkillDataBasePtr->IsEndCoolTime() )
		return false;

	// �ߵ�

	return true;
}

/** @function : CX2GUEve::UseChargingBooster
@brief : ���������⸦ �����. ��ų���Կ� ������ ��� ��ų�� ��ߵ� �ð��� �ʱ�ȭ ��
@param : ������ ��� ��ų Ʈ��
@return : ����
*/
void CX2GUEve::UseChargingBooster( CX2UserSkillTree& pcUserSkillTree_ )
{
	const CX2SkillTree::SkillTemplet* pSkillTempletChargingBooster
		= pcUserSkillTree_.GetUserSkillTemplet( CX2SkillTree::SI_P_EEP_CHARGING_BOOSTER );
	SpecialAttackNoStageChange( pSkillTempletChargingBooster );

	m_ChargingBoosterSkillDataBasePtr->SetTimerBasedOnCoolTime( m_ChargingBoosterSkillDataBasePtr->GetCoolTime() );
	pcUserSkillTree_.ResetLeftSkillCoolTimeAll();
}

//}} kimhc // 2010.11.2 // �������� - ���� ������



//{{ JHKang / ������ / 2010/11/08 / �Ʈ ���Ǿ�
/**
@function ENSI_SA_EAT_ASSUALT_SPEAR_Init
@brief ���õ� ���ҽ� �ε�
- �޽� : ���ʸ���, ������, â(�ִϸ��̼� ����), â(�ִϸ��̼� �и�) ��
*/
void CX2GUEve::ENSI_SA_EAT_ASSUALT_SPEAR_Init()
{
	XSkinMeshReadyInBackground( L"Sp2a_Oberon01.X" );
	XSkinMeshReadyInBackground( L"Sp2a_Ophelia01.X" );
	XSkinMeshReadyInBackground( L"Sp2a_Spear.X" );
	XSkinMeshReadyInBackground( L"Sp2a_Spear_Explosion.X" );
	XSkinMeshReadyInBackground( L"Sp2a_Spear_Fly.X" );
	XSkinMeshReadyInBackground( L"Sp2a_Spear_Ready.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"Mesh_ASSUALT_SPEAR.tga" );
	TextureReadyInBackground( L"Eve_Guardian_Ophelia_Face.tga" );
	TextureReadyInBackground( L"Eve_Guardian_Ophelia_Lowbody.tga" );
	TextureReadyInBackground( L"Eve_Guardian_Ophelia_Upbody.tga" );
	TextureReadyInBackground( L"GUARDIAN_EVE_Hair.tga" );
	TextureReadyInBackground( L"GUARDIAN_EVE_Hand.tga" );
	TextureReadyInBackground( L"GUARDIAN_EVE_Lowbody.tga" );
	TextureReadyInBackground( L"GUARDIAN_EVE_Upbody.tga" );
}

/**
@function ENSI_SA_EAT_ASSUALT_SPEAR_FrameMove
@brief �ִϸ��̼� �� ������ ����Ʈ ���(�߿�)
- 1�� : �ִϸ��̼� Ÿ�� ���� �׽�Ʈ(�ڵ� ����)
- 2�� : ���θ� �浹�� ���߷� ����
*/
void CX2GUEve::ENSI_SA_EAT_ASSUALT_SPEAR_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_L_Finger11", 0.01f, 2 );
	if ( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );

		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -135.f * GetScaleByUnit().x;
		}
		else
		{
			vPos -= GetDirVector() * -135.f * GetScaleByUnit().x;
		}
		vPos.y += 435.f * GetScaleByUnit().y;

		vPos.x -= 10.f;
		vPos.y -= 10.f;

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_EAT_ASSUALT_SPEAR_READY", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		vPos = GetPos();

		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -250.f * GetScaleByUnit().x;
		}
		else
		{
			vPos -= GetDirVector() * -250.f * GetScaleByUnit().x;
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_EAT_ASSUALT_SPEAR_EFFECT2", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SI_SA_EAT_ASSUALT_SPEAR_OPHILIA_NO_DAMAGE", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SI_SA_EAT_ASSUALT_SPEAR_OBERON_NO_DAMAGE", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    if ( m_pXSkinAnim->EventTimerOneshot( 1.199f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );
		vPos.y += 285.f * GetScaleByUnit().y;

		/// ������ ����� ������� �ֱ� ���� ������ ����Ʈ�� �ڵ�� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_EAT_ASSUALT_SPEAR", GetPowerRate() * fStatAtkRateOfSummoned,
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

/**
@function ENSI_SA_EAT_ASSUALT_SPEAR_EventProcess
@brief �̺�Ʈ ó��
*/
void CX2GUEve::ENSI_SA_EAT_ASSUALT_SPEAR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 3.0f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

/**
@function ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_FrameMove
@brief ����
*/
void CX2GUEve::ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_L_Finger11", 0.01f, 2 );

    if ( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );

		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -135.f * GetScaleByUnit().x;
		}
		else
		{
			vPos -= GetDirVector() * -135.f * GetScaleByUnit().x;
		}
		vPos.y += 435.f * GetScaleByUnit().y;

		vPos.x -= 10.f;
		vPos.y -= 10.f;

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_EAT_ASSUALT_SPEAR_READY", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		vPos = GetPos();

		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * -250.f * GetScaleByUnit().x;
		}
		else
		{
			vPos -= GetDirVector() * -250.f * GetScaleByUnit().x;
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_EAT_ASSUALT_SPEAR_EFFECT2", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SI_SA_EAT_ASSUALT_SPEAR_OPHILIA_NO_DAMAGE", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SI_SA_EAT_ASSUALT_SPEAR_OBERON_NO_DAMAGE", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    if ( m_pXSkinAnim->EventTimerOneshot( 1.199f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );
		vPos.y += 285.f * GetScaleByUnit().y;

		/// ������ ����� ������� �ֱ� ���� ������ ����Ʈ�� �ڵ�� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_EAT_ASSUALT_SPEAR", GetPowerRate() * fStatAtkRateOfSummoned,
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

/**
@function ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_EventProcess
@brief ����
*/
void CX2GUEve::ENSI_SA_EAT_ASSUALT_SPEAR_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 3.0f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
//}} JHKang / ������ / 2010/11/08 / �Ʈ ���Ǿ�

//{{ JHKang / ������ / 2010/11/12 / ����� ����
/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_Init
@brief ���õ� ���ҽ� �ε�
- 6���� ���� �ӽ�, ��ȣ�� �̹���
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_Init()
{
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod1.X" );
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod_loop1.X" );
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod_loop2.X" );
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod_loop3.X" );
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod_loop4.X" );
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod_loop5.X" );
	XSkinMeshReadyInBackground( L"ATOMIC_SHIELD_nasod_loop6.X" );
	TextureReadyInBackground( L"Mesh_ENERGY_OF_CONCENTRATION.tga" );
	TextureReadyInBackground( L"Arme_smoke.tga" );
	TextureReadyInBackground( L"Barrier_Ball_AS02.tga" );
	TextureReadyInBackground( L"atomic_eve.tga" );
}

/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_StateStart
@brief �̵��ϸ鼭 ��ų ��� �� ����Ʈ �и��� ���� ����
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_StateStart()
{
	CommonStateStart();

	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_FrameMove
@brief 0.02sec : ���� �ʱ� ����Ʈ, 0.8sec : ������ ������ ������, NPC ���� ��ȯ(�� Ÿ�ݿ� ���� ü�� ����)
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_L_Hand", 0.02f, 2 );

    if ( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD);
		if( NULL != pSkillTemplet )
		{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD ) );	/// ��ų ����

		/// ���� ũ��
		float fShieldSize = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS, iSkillTempletLevel );

			D3DXVECTOR3 vPos = GetPos();

			D3DXVECTOR3 vScale( fShieldSize / 450.f * GetScaleByUnit().x, fShieldSize / 450.f * GetScaleByUnit().y,
				fShieldSize /450.f * GetScaleByUnit().z );

			if( true == GetIsRight() )
			{
				vPos += 40 * GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			else
			{
				vPos -= 40 * GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			vPos.y += 220 * GetScaleByUnit().y * fShieldSize / 450.f;

			/// �ʱ� ����Ʈ
			CX2EffectSet::Handle hAtomicShield = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_ATOMIC_SHIELD_INIT", this, NULL );
			if( INVALID_EFFECTSET_HANDLE != hAtomicShield )
			{
				if ( CX2EffectSet::EffectSetInstance* pAtomicShield = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hAtomicShield ) )
				{
					pAtomicShield->SetEffectScale( vScale );
					pAtomicShield->m_bCustomPosition = true;
					pAtomicShield->m_vPosition = vPos;
				}
			}
		}
	}

    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		// ������ ��ȯ�� ���Ͱ� ������ ���� ��Ű�� �ٽ� ��ȯ�Ѵ�
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );


		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD);
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD ) );	/// ��ų ����

			/// ���� ���� �ð� ����(NPC)
			float fEffectTime = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
			/// ���� ũ��
			float fShieldSize = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS, iSkillTempletLevel );

			D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );

			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			else
			{
				vPos -= GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			vPos.y += GetScaleByUnit().y * fShieldSize / 450.f;

			if( true == g_pX2Game->IsHost() )
			{
				int iHyperMode = 0;

				/// NPC
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, int(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillTempletLevel ) * 100),
					(CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID(), false, (CX2Room::TEAM_NUM) GetTeam() );

				m_fAtomicShieldLifeTime = fEffectTime;
				m_timerAtomicShieldSummoned.restart();
			}
		}
	}

	CommonFrameMove();
}

/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_EventProcess
@brief �̺�Ʈ ó��
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_StateStart
@brief ����
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_StateStart()
{
	CommonStateStart();

	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_FrameMove
@brief ����
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_L_Hand", 0.02f, 2 );

    if ( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD);
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD ) );	/// ��ų ����

			/// ���� ũ��
			float fShieldSize = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS, iSkillTempletLevel );
			D3DXVECTOR3 vPos = GetPos();

			D3DXVECTOR3 vScale( fShieldSize / 450.f * GetScaleByUnit().x, fShieldSize / 450.f * GetScaleByUnit().y,
				fShieldSize /450.f * GetScaleByUnit().z );

			if( true == GetIsRight() )
			{
				vPos += 40 * GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			else
			{
				vPos -= 40 * GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			vPos.y += 220 * GetScaleByUnit().y * fShieldSize / 450.f;

			/// �ʱ� ����Ʈ
			CX2EffectSet::Handle hAtomicShield = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_ATOMIC_SHIELD_INIT", this, NULL );
			if( INVALID_EFFECTSET_HANDLE != hAtomicShield )
			{
				if ( CX2EffectSet::EffectSetInstance* pAtomicShield = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hAtomicShield ) )
				{
					pAtomicShield->SetEffectScale( vScale );
					pAtomicShield->m_bCustomPosition = true;
					pAtomicShield->m_vPosition = vPos;
				}
			}
		}
	}

    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		// ������ ��ȯ�� ���Ͱ� ������ ���� ��Ű�� �ٽ� ��ȯ�Ѵ�
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE );


		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_ENS_ATOMIC_SHIELD);
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			/// ���� ���� �ð� ����(NPC)
			float fEffectTime = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
			/// ���� ũ��
			float fShieldSize = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS, iSkillTempletLevel );

			D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );

			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			else
			{
				vPos -= GetDirVector() * GetScaleByUnit().x * fShieldSize / 450.f;
			}
			vPos.y += GetScaleByUnit().y * fShieldSize / 450.f;

			if( true == g_pX2Game->IsHost() )
			{
				int iHyperMode = 1;

				/// NPC
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_ATOMIC_SHIELD_EVE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, int(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillTempletLevel ) * 100),
					(CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID(),false, (CX2Room::TEAM_NUM) GetTeam() );

				m_fAtomicShieldLifeTime = fEffectTime;
				m_timerAtomicShieldSummoned.restart();
			}
		}
	}

	CommonFrameMove();
}

/**
@function ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_EventProcess
@brief ����
*/
void CX2GUEve::ENSI_SA_EEG_ATOMIC_SHIELD_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 2.640f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
//}} JHKang / ������ / 2010/11/12 / ����� ����



CX2GUEve::EveElectraLaserData::EveElectraLaserData( EveElectraSystem* _pEveElectraSystem )
{
	m_pEveElectraSystem	= _pEveElectraSystem;
	m_AmplifyDamageEffectName	= L"";
	m_SpectrumDamageEffectName	= L"";
	m_bPlaneFront		= false;
	m_bDie				= false;
	m_TraceBone			= L"";
	m_fVelocity			= 0.f;
	m_fLaserPropagateTime	= 0.f;
	m_fElapsedTime		= 0.f;
	m_fPenetrateTime	= 0.f;
	m_fLaserWidthScale	= 1.f;
	m_bPenetrate		= false;
	m_bPenetrateGiga[0]	= false;
	m_bPenetrateGiga[1]	= false;
	m_bPenetrateGiga[2]	= false;
	m_bPenetrateGiga[3]	= false;
	m_hParticleLaserCross			= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaLaserCross[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaLaserCross[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaLaserCross[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaLaserCross[3]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_bApplyLaserRangePassive		= false;
	m_InductionDamageEffectName	= L"";
	m_iFireCount		= 0;
}
void CX2GUEve::EveElectraLaserData::OnFrameMove( float fElapsedTime )
{
	D3DXVECTOR3 vRotateDegree;
	m_fElapsedTime += fElapsedTime;

    CX2DamageEffect::CEffect* pDamageEffect0 = ( m_vecDamageEffect.size() > 0 ) ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[0] ) : NULL;
	if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffect0 = ( pDamageEffect0 != NULL ) ? pDamageEffect0->GetMainEffect() : NULL )
	{
		vRotateDegree = pMainEffect0->GetRotateLocalDegree();

		D3DXVECTOR3 vRotationAxis;
		D3DXVec3Cross( &vRotationAxis, &m_vDirVector, &Y_AXIS );
		D3DXVec3Normalize( &vRotationAxis, &vRotationAxis );


		D3DXMATRIX matRotation;
		D3DXMatrixRotationAxis( &matRotation, &vRotationAxis, D3DX_PI / 180.f * vRotateDegree.z );

		D3DXVECTOR4 v4DirVectorLaser;
		D3DXVec3Transform( &v4DirVectorLaser, &m_vDirVector, &matRotation );

		D3DXVECTOR3 vDirVectorLaser( v4DirVectorLaser.x, v4DirVectorLaser.y, v4DirVectorLaser.z );


		pDamageEffect0->SetPos( m_vLaserStartPos );

		D3DXVECTOR3 vLaserNowPos = m_vLaserStartPos + ( m_fVelocity * m_fElapsedTime * vDirVectorLaser );
		D3DXVECTOR3 vLaserEndPos = m_vLaserStartPos + ( m_fVelocity * m_fLaserPropagateTime * m_pEveElectraSystem->m_fLaserRangeScale * vDirVectorLaser );

		if( m_fElapsedTime > m_fLaserPropagateTime * m_pEveElectraSystem->m_fLaserRangeScale)
		{
			vLaserNowPos = vLaserEndPos;
		}

		D3DXVECTOR3 vPlaneIntersectPoint;
		D3DXPlaneIntersectLine( &vPlaneIntersectPoint, &m_pEveElectraSystem->m_PrimaryPlane, &vLaserEndPos, &m_vLaserStartPos );

		if( m_pEveElectraSystem->GetEnablePlane() == false )	// ������ ���� ��
		{

			if( m_pEveElectraSystem->GetEnableGigaPlane() == false )
			{
				const float MAGIC_LASER_LENGTH = 0.01f;
				float fLaserLength = D3DXVec3Length( &( vLaserNowPos - m_vLaserStartPos ) );
				fLaserLength *= MAGIC_LASER_LENGTH;

				pMainEffect0->SetScale( fLaserLength, m_fLaserWidthScale, 1.f );

				if( m_vecDamageEffect.size() > 1 )
				{
					for(UINT i = 1; i < m_vecDamageEffect.size(); i++ )
					{
                        if ( m_vecDamageEffect[i] != INVALID_DAMAGE_EFFECT_HANDLE )
                            g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_vecDamageEffect[i] );
					}
				}
			}
			//////////// GIGA STREAM ////////////////////////////////////////////////////////{{
			else
			{
				D3DXVECTOR3 vGigaPlaneIntersectPoint;
				D3DXPlaneIntersectLine( &vGigaPlaneIntersectPoint, &m_pEveElectraSystem->m_GigaStreamPlane, &vLaserEndPos, &m_vLaserStartPos );

				if( D3DXPlaneDotCoord( &m_pEveElectraSystem->m_GigaStreamPlane, &D3DXVECTOR3( vLaserNowPos ) ) < 0.0f )
				{
					const float MAGIC_LASER_LENGTH = 0.01f;
					float fLaserLength = D3DXVec3Length( &( vLaserNowPos - m_vLaserStartPos ) );
					fLaserLength *= MAGIC_LASER_LENGTH;

					pMainEffect0->SetScale( fLaserLength, m_fLaserWidthScale, 1.f );
				}
				else
				{

					const float MAGIC_LASER_LENGTH = 0.01f;
					float fLaserLength = D3DXVec3Length( &( vGigaPlaneIntersectPoint - m_vLaserStartPos ) );
					fLaserLength *= MAGIC_LASER_LENGTH;

					pMainEffect0->SetScale( fLaserLength, m_fLaserWidthScale, 1.f );

					if( m_bPenetrate == false )
					{
						CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_AmplifyDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vGigaPlaneIntersectPoint, m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree(),
							0, false, -1.f, 1.f, 1.f, -1, -1.f, 0, -1.f, true );
						m_pEveElectraSystem->LaserAttackLineSet( pEffect );
                        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                        {
						    pMeshInstance->SetNowLifeTime( pMainEffect0->GetNowLifeTime() );
						    pMeshInstance->SetMaxLifeTime( pMainEffect0->GetMaxLifeTime() );
                        }
						pEffect->SetLaserGroupID( pDamageEffect0->GetLaserGroupID() );
                        m_vecDamageEffect.push_back( pEffect->GetHandle() );

						CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
						*pDamageData = pDamageEffect0->GetDamageData();
			            pDamageData->hAttackerEffect = pEffect->GetHandle();
						pDamageData->m_iLaserGroupID = pDamageEffect0->GetLaserGroupID();

						pDamageData->damage.fPhysic *= 2.0f;
						pDamageData->damage.fMagic *= 2.0f;


						m_hParticleLaserCross = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pEveElectraSystem->m_pGUEve,  L"eve_spectro_giga_bump_P01", vGigaPlaneIntersectPoint );
						CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleLaserCross );
						if( pSeqLaserCross != NULL )
						{
							pSeqLaserCross->SetScaleFactor( D3DXVECTOR3( m_fLaserWidthScale * 0.7f, m_fLaserWidthScale * 0.7f, m_fLaserWidthScale * 0.7f ) );
							D3DXVECTOR3 vRotateDegree = m_pEveElectraSystem->m_pGUEve->GetRotateDegree();
							vRotateDegree.z = m_pEveElectraSystem->m_fPlaneZAngle;
							pSeqLaserCross->SetAxisAngle( vRotateDegree );
							pSeqLaserCross->SetAddRotate( vRotateDegree );
						}

						m_bPenetrate = true;
					}

                    CX2DamageEffect::CEffect* pDamageEffect1 = ( m_vecDamageEffect.size() > 1 ) ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[1] ) : NULL;
					if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffect1 = ( pDamageEffect1 != NULL ) ? pDamageEffect1->GetMainEffect() : NULL )
					{

						pDamageEffect1->SetPos( vGigaPlaneIntersectPoint );

						D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

						vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle + vSecondRotateDegree.z * 0.8f;

						pMainEffect1->SetRotateLocalDegree( vSecondRotateDegree );

						float fSecondLaserLength = D3DXVec3Length( &( vLaserNowPos - vGigaPlaneIntersectPoint ) );
						fSecondLaserLength *= MAGIC_LASER_LENGTH;
						pMainEffect1->SetScale( fSecondLaserLength, m_fLaserWidthScale * 5.0f, 1.f );

						pMainEffect1->SetMeshAlphaColor( pMainEffect0->GetMeshAlphaColor() );

					}


				}
				CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleLaserCross );
				if( pSeqLaserCross != NULL )
				{
					pSeqLaserCross->SetPosition( vGigaPlaneIntersectPoint );
				}
			}
			//////////// GIGA STREAM ////////////////////////////////////////////////////////}}
		}
		else if( D3DXPlaneDotCoord( &m_pEveElectraSystem->m_PrimaryPlane, &D3DXVECTOR3( vLaserNowPos ) ) < 0.0f )
		{
			const float MAGIC_LASER_LENGTH = 0.01f;
			float fLaserLength = D3DXVec3Length( &( vLaserNowPos - m_vLaserStartPos ) );
			fLaserLength *= MAGIC_LASER_LENGTH;

			pMainEffect0->SetScale( fLaserLength, m_fLaserWidthScale, 1.f );
		}
		else if( m_fPenetrateTime > 0.f )
		{
			m_fPenetrateTime -= fElapsedTime;
			m_fElapsedTime -= fElapsedTime;

			const float MAGIC_LASER_LENGTH = 0.01f;
			float fLaserLength = D3DXVec3Length( &( vPlaneIntersectPoint - m_vLaserStartPos ) );
			fLaserLength *= MAGIC_LASER_LENGTH;

			pMainEffect0->SetScale( fLaserLength, m_fLaserWidthScale, 1.f );

		}
		else	// ������ ���� ��
		{
			const float MAGIC_LASER_LENGTH = 0.01f;
			float fLaserLength = D3DXVec3Length( &( vPlaneIntersectPoint - m_vLaserStartPos ) );
			fLaserLength *= MAGIC_LASER_LENGTH;

			pMainEffect0->SetScale( fLaserLength, m_fLaserWidthScale, 1.f );

			if( m_bPenetrate == false )
			{
				switch( m_pEveElectraSystem->m_ePlaneFormationType )
				{
				case PFT_AMPLIFIER:	// ������ : ����/ ���� ������ 1.2��
					{
						CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_AmplifyDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vPlaneIntersectPoint, m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree(),
							0, false, -1.f, 1.f, 1.f, -1, -1.f, 0, -1.f, true );

                        if ( pEffect != NULL )
                        {
						    m_pEveElectraSystem->LaserAttackLineSet( pEffect );

                            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                            {
						        pMeshInstance->SetNowLifeTime( pMainEffect0->GetNowLifeTime() );
						        pMeshInstance->SetMaxLifeTime( pMainEffect0->GetMaxLifeTime() );
                            }
						    pEffect->SetLaserGroupID( pDamageEffect0->GetLaserGroupID() );
                            m_vecDamageEffect.push_back( pEffect->GetHandle() );

						    CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
						    *pDamageData = pDamageEffect0->GetDamageData();
			                pDamageData->hAttackerEffect = pEffect->GetHandle();
						    pDamageData->m_iLaserGroupID = pDamageEffect0->GetLaserGroupID();

						    pDamageData->damage.fPhysic *= 1.2f;
						    pDamageData->damage.fMagic *= 1.2f;
                        }

					}
					break;
				case PFT_SPECTRUM:	/// �б��� : ����/ ���� ������, ȹ�� ����, �Ӽ�/ ���� �ߵ� Ȯ�� 0.45��, �� ���� ����Ʈ ����
					{
						for(UINT i = 1; i <= 3; i++ )
						{
							CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_SpectrumDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vPlaneIntersectPoint, m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree(),
								0, false, -1.f, 1.f, 1.f, -1, -1.f, 0, -1.f, true );

                            if ( pEffect != NULL )
                            {
							    m_pEveElectraSystem->LaserAttackLineSet( pEffect );

                                if (  CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                                {
							        pMeshInstance->SetNowLifeTime( pMainEffect0->GetNowLifeTime() );
							        pMeshInstance->SetMaxLifeTime( pMainEffect0->GetMaxLifeTime() );
                                }
							    pEffect->SetLaserGroupID( i + ( pDamageEffect0->GetLaserGroupID() - 1 ) * 3 );
                                m_vecDamageEffect.push_back( pEffect->GetHandle() );

							    CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
							    *pDamageData = pDamageEffect0->GetDamageData();
			                    pDamageData->hAttackerEffect = pEffect->GetHandle();
							    pDamageData->m_iLaserGroupID = i + ( pDamageEffect0->GetLaserGroupID() - 1 ) * 3;

							    pDamageData->damage.fPhysic *= 0.45f;
							    pDamageData->damage.fMagic *= 0.45f;

							    pDamageData->fHitAddMP *= 0.45f;

							    pDamageData->m_fRateModifier *= 0.45f;

							    pDamageData->fForceDownValue	*= 0.333f;
                            }
						}
					}
					break;
				case PFT_INDUCTION:	// ������ : ������ / ȹ�� ���� / �Ӽ� / ���� �ߵ� Ȯ�� 0.55, Size 0.8, ����ź, ������ Ÿ�� +1
					{
						++m_iFireCount;
					} break;
				case PFT_FUSION:	// ������ : ������ / ȹ�� ���� / �Ӽ� / ���� �ߵ� Ȯ�� 0.25, Size 0.75, ����ź, ����, ������ Ÿ�� +1
					{
						++m_iFireCount;
					} break;
				default:
					break;
				}

				m_hParticleLaserCross = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pEveElectraSystem->m_pGUEve,  L"eve_spectro_bump_P01", vPlaneIntersectPoint );
				CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleLaserCross );
				if( pSeqLaserCross != NULL )
				{
					pSeqLaserCross->SetScaleFactor( D3DXVECTOR3( m_fLaserWidthScale * 0.7f, m_fLaserWidthScale * 0.7f, m_fLaserWidthScale * 0.7f ) );
				}

				m_bPenetrate = true;
			}



			switch( m_pEveElectraSystem->m_ePlaneFormationType )
			{
			case PFT_AMPLIFIER:
				{
                    CX2DamageEffect::CEffect* pDamageEffect1 = ( m_vecDamageEffect.size() > 1 )
                        ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[1] ) : NULL;
					if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffect1 = ( pDamageEffect1 != NULL ) ? pDamageEffect1->GetMainEffect() : NULL )
					{
						pDamageEffect1->SetPos( vPlaneIntersectPoint );

						D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

						vSecondRotateDegree.z += m_pEveElectraSystem->m_fEveZAngle + m_pEveElectraSystem->m_fPlaneZAngle + vSecondRotateDegree.z * 0.8f;

						pMainEffect1->SetRotateLocalDegree( vSecondRotateDegree );

						float fSecondLaserLength = D3DXVec3Length( &( vLaserNowPos - vPlaneIntersectPoint ) );
						fSecondLaserLength *= MAGIC_LASER_LENGTH;
						pMainEffect1->SetScale( fSecondLaserLength,
							m_fLaserWidthScale * ( 1.5f + m_pEveElectraSystem->m_fHyperOpticalResearchScale ), 1.f );
						pMainEffect1->SetMeshAlphaColor( pMainEffect0->GetMeshAlphaColor() );

					}
				}
				break;
			case PFT_SPECTRUM:
				{
					for(UINT i = 1; i <=3; i++ )
					{
                        CX2DamageEffect::CEffect* pDamageEffecti = ( m_vecDamageEffect.size() > i )
                            ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[i] ) : NULL;
						if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffecti = ( pDamageEffecti != NULL ) ? pDamageEffecti->GetMainEffect() : NULL )
						{
							pDamageEffecti->SetPos( vPlaneIntersectPoint );

							D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

							vSecondRotateDegree.z += m_pEveElectraSystem->m_fEveZAngle + m_pEveElectraSystem->m_fPlaneZAngle + vSecondRotateDegree.z * 0.8f;

							float vSpectrumZRotate = vRotateDegree.z - vSecondRotateDegree.z;
							if( vSpectrumZRotate < 0.f )
								vSpectrumZRotate = -vSpectrumZRotate;

							if( m_pEveElectraSystem->m_fPlaneZAngle == 0.f )
							{
								if( i == 2)
									vSecondRotateDegree.z += (14.f + vSpectrumZRotate * 0.65f );
								else if( i == 3 )
									vSecondRotateDegree.z -= (14.f + vSpectrumZRotate * 0.65f );
							}
							else
							{
								if( i == 2)
									vSecondRotateDegree.z += vSpectrumZRotate * 0.6f;
								else if( i == 3 )
									vSecondRotateDegree.z -= vSpectrumZRotate * 0.6f;
							}

							pMainEffecti->SetRotateLocalDegree( vSecondRotateDegree );

							float fSecondLaserLength = D3DXVec3Length( &( vLaserNowPos - vPlaneIntersectPoint ) );
							fSecondLaserLength *= MAGIC_LASER_LENGTH;
							pMainEffecti->SetScale( fSecondLaserLength, m_fLaserWidthScale * 0.7f, 1.f );

							pMainEffecti->SetMeshAlphaColor( pMainEffect0->GetMeshAlphaColor() );
						}
					}
				} break;

			case PFT_INDUCTION:	// ������ bPenetrate �� ó���� �κ�
				{
					pDamageEffect0->GetDamageData().bReAttack = false;

					if ( m_fElapsedTime < pMainEffect0->GetMaxLifeTime() &&
						m_fElapsedTime <= pDamageEffect0->GetAttackTime().y &&
						m_fElapsedTime > m_iFireCount * pDamageEffect0->GetDamageData().fHitGap )
					{
						++m_iFireCount;

						D3DXVECTOR2 vLockOnRange( 0.f, 0.f );

						CX2DamageEffect::CEffect* pEffect = NULL;

						if ( CX2DamageManager::AT_SPECIAL == pDamageEffect0->GetDamageData().attackType )
						{
							pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
								m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER_SPECIAL",
								m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vPlaneIntersectPoint,
								m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
							ASSERT( pEffect != NULL );
                            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                            {
						        pMeshInstance->SetNowLifeTime( 0.f );
						        pMeshInstance->SetMaxLifeTime( 2.5f );
                            }

							vLockOnRange = D3DXVECTOR2( 0, 1500 );
						}
						else
						{
							pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
								m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER",
								m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vPlaneIntersectPoint,
								m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
							ASSERT( pEffect != NULL );
                            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                            {
						        pMeshInstance->SetNowLifeTime( 0.f );
						        pMeshInstance->SetMaxLifeTime( 1.0f );
                            }

							vLockOnRange = D3DXVECTOR2( 0, 500 );
						}

						pEffect->SetAttackTime( D3DXVECTOR2( 0.f, 100.f) );
						CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();

						D3DXVECTOR2 vBackSpeed = pDamageData->backSpeed;
						*pDamageData = pDamageEffect0->GetDamageData();
			            pDamageData->hAttackerEffect = pEffect->GetHandle();
						pDamageData->damage.fPhysic		*= 0.55f;
						pDamageData->damage.fMagic		*= 0.55f;
						pDamageData->fHitGap			= 0.5f;
						pDamageData->bReAttack			= true;
						pDamageData->fHitAddMP			*= 0.55f;
						pDamageData->m_fRateModifier	*= 0.55f;
						pDamageData->bArrangedFly		= false;
						pDamageData->backSpeed			= vBackSpeed;
						pDamageData->fForceDownValue	*= 0.5f;
						pDamageData->techPoint			= int(pDamageData->techPoint * 0.55);
						pDamageData->m_iLaserGroupID	= -1;

						bool bUserUnit = false;
						UidType gameUnitUID = -1;

						if ( CX2DamageManager::AT_SPECIAL == pDamageEffect0->GetDamageData().attackType )
						{
							pDamageData->m_fPvpRate *= 0.75f;
						}

						if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
						{
							gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
								m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );
						}
						else
						{
							vLockOnRange.y += 300.f;
							gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
								m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );
						}

						if( bUserUnit == true )
							pEffect->SetLockOnUnitUID( gameUnitUID );
						else
							pEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );

						if( m_pEveElectraSystem->GetEnableGigaPlane() == true )
                        {
                            m_vecDamageEffect.push_back( pEffect->GetHandle() );
                        }
					}
				} break;

			case PFT_FUSION:
				{
					pDamageEffect0->GetDamageData().bReAttack = false;

					if ( m_fElapsedTime < pMainEffect0->GetMaxLifeTime() &&
						m_fElapsedTime <= pDamageEffect0->GetAttackTime().y &&
						m_fElapsedTime > m_iFireCount * pDamageEffect0->GetDamageData().fHitGap )
					{
						++m_iFireCount;

						D3DXVECTOR2 vLockOnRange( 0.f, 0.f );

						for( UINT i = 1; i <= 3; ++i )
						{
							CX2DamageEffect::CEffect* pEffect = NULL;

							if ( CX2DamageManager::AT_SPECIAL == pDamageEffect0->GetDamageData().attackType )
							{
								pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
									m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER_SPECIAL",
									m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vPlaneIntersectPoint,
									m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
								ASSERT( pEffect != NULL );
                                if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                                {
								    pMeshInstance->SetNowLifeTime( 0.f );
								    pMeshInstance->SetMaxLifeTime( 2.5f );
                                }
								vLockOnRange = D3DXVECTOR2( 0, 1500 );
							}
							else
							{
								pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
									m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER",
									m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vPlaneIntersectPoint,
									m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
								ASSERT( pEffect != NULL );
                                if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                                {
								    pMeshInstance->SetNowLifeTime( 0.f );
								    pMeshInstance->SetMaxLifeTime( 1.0f );
                                }
								vLockOnRange = D3DXVECTOR2( 0, 500 );
							}

							pEffect->SetAttackTime( D3DXVECTOR2( 0.f, 100.f) );
							CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
							D3DXVECTOR2 vBackSpeed = pDamageData->backSpeed;
							*pDamageData = pDamageEffect0->GetDamageData();
			                pDamageData->hAttackerEffect = pEffect->GetHandle();
							pDamageData->fHitGap			= 0.5f;
							pDamageData->bReAttack			= true;
							pDamageData->damage.fPhysic		*= 0.25f;
							pDamageData->damage.fMagic		*= 0.25f;
							pDamageData->fHitAddMP			*= 0.25f;
							pDamageData->m_fRateModifier	*= 0.25f;
							pDamageData->bArrangedFly		= false;
							pDamageData->backSpeed			= vBackSpeed;
							pDamageData->fForceDownValue	*= 0.166f;
							pDamageData->techPoint			= int(pDamageData->techPoint * 0.25);
							pDamageData->m_iLaserGroupID	= -1;

							bool bUserUnit = false;
							UidType gameUnitUID = -1;

							if ( CX2DamageManager::AT_SPECIAL == pDamageEffect0->GetDamageData().attackType )
							{
								pDamageData->m_fPvpRate *= 0.75f;
							}

							if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
							{
								gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
									m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );
							}
							else
							{
								vLockOnRange.y += 300.f;
								gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
									m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );
							}

							if( bUserUnit == true )
								pEffect->SetLockOnUnitUID( gameUnitUID );
							else
								pEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );

							if( m_pEveElectraSystem->GetEnableGigaPlane() == true )
                            {
                                m_vecDamageEffect.push_back( pEffect->GetHandle() );
                            }
						}
					}
				} break;
			default:
				break;
			}
			CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleLaserCross );
			if( pSeqLaserCross != NULL )
			{
				pSeqLaserCross->SetPosition( vPlaneIntersectPoint );
			}
			//////////// GIGA STREAM ////////////////////////////////////////////////////////{{
			if( m_pEveElectraSystem->GetEnableGigaPlane() == true )
			{
				switch ( m_pEveElectraSystem->m_ePlaneFormationType )
				{
				case PFT_AMPLIFIER:
					{
                        CX2DamageEffect::CEffect* pDamageEffect1 = ( m_vecDamageEffect.size() > 1 )
                            ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[1] ) : NULL;
					    if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffect1 = ( pDamageEffect1 != NULL ) ? pDamageEffect1->GetMainEffect() : NULL )
						{
							D3DXVECTOR3 vRotateDegree = pMainEffect1->GetRotateLocalDegree();

							D3DXVECTOR3 vRotationAxis;
							D3DXVec3Cross( &vRotationAxis, &m_vDirVector, &Y_AXIS );
							D3DXVec3Normalize( &vRotationAxis, &vRotationAxis );


							D3DXMATRIX matRotation;
							D3DXMatrixRotationAxis( &matRotation, &vRotationAxis, D3DX_PI / 180.f * vRotateDegree.z );

							D3DXVECTOR4 v4DirVectorLaser;
							D3DXVec3Transform( &v4DirVectorLaser, &m_vDirVector, &matRotation );

							D3DXVECTOR3 vDirVectorLaser( v4DirVectorLaser.x, v4DirVectorLaser.y, v4DirVectorLaser.z );

							D3DXVECTOR3 vLaserStartPos = vPlaneIntersectPoint;

							D3DXVECTOR3 vLaserNowPos = vLaserStartPos + ( m_fVelocity * m_fElapsedTime * vDirVectorLaser );
							D3DXVECTOR3 vLaserEndPos = vLaserStartPos + ( m_fVelocity * m_fLaserPropagateTime * m_pEveElectraSystem->m_fLaserRangeScale * vDirVectorLaser );

							if( m_fElapsedTime > m_fLaserPropagateTime * m_pEveElectraSystem->m_fLaserRangeScale)
							{
								vLaserNowPos = vLaserEndPos;
							}

							D3DXVECTOR3 vGigaPlaneIntersectPoint;
							D3DXPlaneIntersectLine( &vGigaPlaneIntersectPoint, &m_pEveElectraSystem->m_GigaStreamPlane, &vLaserEndPos, &vLaserStartPos );

							if( D3DXPlaneDotCoord( &m_pEveElectraSystem->m_GigaStreamPlane, &D3DXVECTOR3( vLaserNowPos ) ) < 0.0f )
							{
								const float MAGIC_LASER_LENGTH = 0.01f;
								float fLaserLength = D3DXVec3Length( &( vLaserNowPos - vLaserStartPos ) );
								fLaserLength *= MAGIC_LASER_LENGTH;

								pMainEffect1->SetScale( fLaserLength, m_fLaserWidthScale * 1.5f, 1.f );
							}
							else
							{

								const float MAGIC_LASER_LENGTH = 0.01f;
								float fLaserLength = D3DXVec3Length( &( vGigaPlaneIntersectPoint - vLaserStartPos ) );
								fLaserLength *= MAGIC_LASER_LENGTH;

								pMainEffect1->SetScale( fLaserLength, m_fLaserWidthScale * 1.5f, 1.f );

								if( m_bPenetrateGiga[0] == false )
								{
									CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_AmplifyDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vGigaPlaneIntersectPoint, m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree(),
										0, false, -1.f, 1.f, 1.f, -1, -1.f, 0, -1.f, true );
									ASSERT( pEffect != NULL );
									m_pEveElectraSystem->LaserAttackLineSet( pEffect );
                                    if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                                    {
									    pMeshInstance->SetNowLifeTime( pMainEffect1->GetNowLifeTime() );
									    pMeshInstance->SetMaxLifeTime( pMainEffect1->GetMaxLifeTime() );
                                    }
									pEffect->SetLaserGroupID( pDamageEffect0->GetLaserGroupID() );
                                    m_vecDamageEffect.push_back( pEffect->GetHandle() );

									CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
									*pDamageData = pDamageEffect1->GetDamageData();
			                        pDamageData->hAttackerEffect = pEffect->GetHandle();
									pDamageData->m_iLaserGroupID = pDamageEffect0->GetLaserGroupID();

									pDamageData->damage.fPhysic *= 2.0f;
									pDamageData->damage.fMagic *= 2.0f;

									m_hParticleGigaLaserCross[0] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pEveElectraSystem->m_pGUEve,  L"eve_spectro_giga_bump_P01", vGigaPlaneIntersectPoint );
									CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaLaserCross[0] );
									if( pSeqLaserCross != NULL )
									{
										float fhyperORScale = m_pEveElectraSystem->m_fHyperOpticalResearchScale;
										pSeqLaserCross->SetScaleFactor( D3DXVECTOR3( ( m_fLaserWidthScale + fhyperORScale ) * 1.0f,
											( m_fLaserWidthScale + fhyperORScale ) * 1.0f,
											( m_fLaserWidthScale + fhyperORScale ) * 1.0f ) );
										D3DXVECTOR3 vRotateDegree = m_pEveElectraSystem->m_pGUEve->GetRotateDegree();
										vRotateDegree.z = m_pEveElectraSystem->m_fPlaneZAngle;
										pSeqLaserCross->SetAxisAngle( vRotateDegree );
										pSeqLaserCross->SetAddRotate( vRotateDegree );

									}

									m_bPenetrateGiga[0] = true;
								}

                                CX2DamageEffect::CEffect* pDamageEffect2 = ( m_vecDamageEffect.size() > 2 )
                                    ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[2] ) : NULL;
        					    if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffect2 = ( pDamageEffect2 != NULL ) ? pDamageEffect2->GetMainEffect() : NULL )
								{

									pDamageEffect2->SetPos( vGigaPlaneIntersectPoint );

									D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

									//vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle + vSecondRotateDegree.z * 0.8f;

									pMainEffect2->SetRotateLocalDegree( vSecondRotateDegree );

									float fSecondLaserLength = D3DXVec3Length( &( vLaserNowPos - vGigaPlaneIntersectPoint ) );
									fSecondLaserLength *= MAGIC_LASER_LENGTH;
									pMainEffect2->SetScale( fSecondLaserLength,
										( m_fLaserWidthScale + m_pEveElectraSystem->m_fHyperOpticalResearchScale ) * 7.5f, 1.f );

									pMainEffect2->SetMeshAlphaColor( pMainEffect1->GetMeshAlphaColor() );

								}
							}
							CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaLaserCross[0] );
							if( pSeqLaserCross != NULL )
							{
								pSeqLaserCross->SetPosition( vGigaPlaneIntersectPoint );
							}
						}
					} break;
				case PFT_SPECTRUM:
					{
						for(UINT i=1; i<4; ++i )
						{
                            CX2DamageEffect::CEffect* pDamageEffecti = ( m_vecDamageEffect.size() > i )
                                ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[i] ) : NULL;
						    if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffecti = ( pDamageEffecti != NULL ) ? pDamageEffecti->GetMainEffect() : NULL )
							{

								D3DXVECTOR3 vRotateDegree = pMainEffecti->GetRotateLocalDegree();

								D3DXVECTOR3 vRotationAxis;
								D3DXVec3Cross( &vRotationAxis, &m_vDirVector, &Y_AXIS );
								D3DXVec3Normalize( &vRotationAxis, &vRotationAxis );


								D3DXMATRIX matRotation;
								D3DXMatrixRotationAxis( &matRotation, &vRotationAxis, D3DX_PI / 180.f * vRotateDegree.z );

								D3DXVECTOR4 v4DirVectorLaser;
								D3DXVec3Transform( &v4DirVectorLaser, &m_vDirVector, &matRotation );

								D3DXVECTOR3 vDirVectorLaser( v4DirVectorLaser.x, v4DirVectorLaser.y, v4DirVectorLaser.z );

								D3DXVECTOR3 vLaserStartPos = vPlaneIntersectPoint;

								D3DXVECTOR3 vLaserNowPos = vLaserStartPos + ( m_fVelocity * m_fElapsedTime * vDirVectorLaser );
								D3DXVECTOR3 vLaserEndPos = vLaserStartPos + ( m_fVelocity * m_fLaserPropagateTime * m_pEveElectraSystem->m_fLaserRangeScale * vDirVectorLaser );

								if( m_fElapsedTime > m_fLaserPropagateTime * m_pEveElectraSystem->m_fLaserRangeScale)
								{
									vLaserNowPos = vLaserEndPos;
								}

								D3DXVECTOR3 vGigaPlaneIntersectPoint;
								D3DXPlaneIntersectLine( &vGigaPlaneIntersectPoint, &m_pEveElectraSystem->m_GigaStreamPlane, &vLaserEndPos, &vLaserStartPos );

								if( D3DXPlaneDotCoord( &m_pEveElectraSystem->m_GigaStreamPlane, &D3DXVECTOR3( vLaserNowPos ) ) < 0.0f )
								{
									const float MAGIC_LASER_LENGTH = 0.01f;
									float fLaserLength = D3DXVec3Length( &( vLaserNowPos - vLaserStartPos ) );
									fLaserLength *= MAGIC_LASER_LENGTH;

									pMainEffecti->SetScale( fLaserLength, m_fLaserWidthScale * 0.7f, 1.f );
								}
								else
								{

									const float MAGIC_LASER_LENGTH = 0.01f;
									float fLaserLength = D3DXVec3Length( &( vGigaPlaneIntersectPoint - vLaserStartPos ) );
									fLaserLength *= MAGIC_LASER_LENGTH;

									pMainEffecti->SetScale( fLaserLength, m_fLaserWidthScale * 0.7f, 1.f );

									if( m_bPenetrateGiga[i] == false )
									{
										CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_AmplifyDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vGigaPlaneIntersectPoint, m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree(),
											0, false, -1.f, 1.f, 1.f, -1, -1.f, 0, -1.f, true );
										ASSERT( pEffect != NULL );
										m_pEveElectraSystem->LaserAttackLineSet( pEffect );
                                        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                                        {
										    pMeshInstance->SetNowLifeTime( pMainEffecti->GetNowLifeTime() );
										    pMeshInstance->SetMaxLifeTime( pMainEffecti->GetMaxLifeTime() );
                                        }
										pEffect->SetLaserGroupID( pDamageEffecti->GetLaserGroupID() );
                                        m_vecDamageEffect.push_back( pEffect->GetHandle() );

										CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
										*pDamageData = pDamageEffecti->GetDamageData();
			                            pDamageData->hAttackerEffect = pEffect->GetHandle();
										pDamageData->m_iLaserGroupID = pDamageEffecti->GetLaserGroupID();

										pDamageData->damage.fPhysic *= 2.0f;
										pDamageData->damage.fMagic *= 2.0f;

										m_hParticleGigaLaserCross[i] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pEveElectraSystem->m_pGUEve,  L"eve_spectro_giga_bump_P01", vGigaPlaneIntersectPoint );
										CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaLaserCross[i] );
										if( pSeqLaserCross != NULL )
										{
											pSeqLaserCross->SetScaleFactor( D3DXVECTOR3( m_fLaserWidthScale * 0.4f, m_fLaserWidthScale * 0.4f, m_fLaserWidthScale * 0.4f ) );
											D3DXVECTOR3 vRotateDegree = m_pEveElectraSystem->m_pGUEve->GetRotateDegree();
											vRotateDegree.z = m_pEveElectraSystem->m_fPlaneZAngle;
											pSeqLaserCross->SetAxisAngle( vRotateDegree );
											pSeqLaserCross->SetAddRotate( vRotateDegree );
										}

										m_bPenetrateGiga[i] = true;
									}

                                    CX2DamageEffect::CEffect* pDamageEffecti3 = ( m_vecDamageEffect.size() > i+3 )
                                        ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[i+3] ) : NULL;
						            if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffecti3 = ( pDamageEffecti3 != NULL ) ? pDamageEffecti3->GetMainEffect() : NULL )
									{

										pDamageEffecti3->SetPos( vGigaPlaneIntersectPoint );

										D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

										//vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle + vSecondRotateDegree.z * 0.8f;

										pMainEffecti3->SetRotateLocalDegree( vSecondRotateDegree );

										float fSecondLaserLength = D3DXVec3Length( &( vLaserNowPos - vGigaPlaneIntersectPoint ) );
										fSecondLaserLength *= MAGIC_LASER_LENGTH;
										pMainEffecti3->SetScale( fSecondLaserLength, m_fLaserWidthScale * 3.5f, 1.f );

										pMainEffecti3->SetMeshAlphaColor( pMainEffecti->GetMeshAlphaColor() );

									}
								}
								CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaLaserCross[i] );
								if( pSeqLaserCross != NULL )
								{
									pSeqLaserCross->SetPosition( vGigaPlaneIntersectPoint );
								}
							}
						}
					} break;
				case PFT_INDUCTION:
					{
						for( UINT i = 1; i < m_vecDamageEffect.size(); ++i )
						{
                            CX2DamageEffect::CEffect* pDamageEffecti = ( m_vecDamageEffect.size() > i )
                                ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[i] ) : NULL;
						    if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffecti = ( pDamageEffecti != NULL ) ? pDamageEffecti->GetMainEffect() : NULL )
							{
								D3DXVECTOR3 vEffectNowPos = pMainEffecti->GetPos();	// ����Ʈ�� ���� ��ġ
								D3DXVECTOR2 vLockOnRange( 0.f, 0.f );

								if( false == pDamageEffecti->GetCheckSpectro() &&
									0.f < D3DXPlaneDotCoord( &m_pEveElectraSystem->m_GigaStreamPlane, &D3DXVECTOR3( vEffectNowPos ) ) )
								{
									D3DXVECTOR3 vEffectEndPos;
									D3DXPlaneIntersectLine( &vEffectEndPos, &m_pEveElectraSystem->m_GigaStreamPlane, &vEffectNowPos,
										&vPlaneIntersectPoint );

									CX2DamageEffect::CEffect* pEffect = NULL;
                                    CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
									if ( CX2DamageManager::AT_SPECIAL == pDamageEffecti->GetDamageData().attackType )
									{
										pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
											m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER_SPECIAL",
											m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vEffectEndPos,
											m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
										ASSERT( pEffect != NULL );
                                        if ( (pMeshInstance = pEffect->GetMainEffect()) )
                                        {
										    pMeshInstance->SetNowLifeTime( 0.f );
										    pMeshInstance->SetMaxLifeTime( 2.5f );
                                        }
										vLockOnRange = D3DXVECTOR2( 1, 1500 );
									}
									else
									{
										pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
											m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER",
											m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vEffectEndPos,
											m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
										ASSERT( pEffect != NULL );
                                        if ( (pMeshInstance = pEffect->GetMainEffect()) )
                                        {
										    pMeshInstance->SetNowLifeTime( 0.f );
										    pMeshInstance->SetMaxLifeTime( 1.0f );
                                        }
										vLockOnRange = D3DXVECTOR2( 1, 400 );
									}

									pEffect->SetAttackTime( D3DXVECTOR2( 0.f, 100.f) );
									CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
									*pDamageData = pDamageEffecti->GetDamageData();
			                        pDamageData->hAttackerEffect = pEffect->GetHandle();
									pDamageData->damage.fPhysic		*= 2.0f;
									pDamageData->damage.fMagic		*= 2.0f;
									pDamageData->fHitGap			= 0.5f;
									pDamageData->bReAttack			= true;
									pDamageData->bArrangedFly		= false;
									pDamageData->m_iLaserGroupID	= -1;

									bool bUserUnit = false;
									UidType gameUnitUID = -1;

									if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
										gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
										m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );
									else
										gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
										m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );

									if( bUserUnit == true )
									{
										pEffect->SetLockOnUnitUID( gameUnitUID );
										pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
									}
									else
									{
										pEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
										pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
									}

									pEffect->SetCheckSpectro( true );
									g_pX2Game->GetDamageEffect()->DestroyInstanceSilently( pDamageEffecti );

									CKTDGParticleSystem::CParticleEventSequenceHandle hParticleCross = g_pX2Game->
										GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pEveElectraSystem->m_pGUEve,
										L"eve_spectro_giga_cross_P01", vEffectEndPos );
									CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross =
										g_pX2Game->GetMajorParticle()->GetInstanceSequence( hParticleCross );

									if( pSeqLaserCross != NULL )
									{
                                        if ( pMeshInstance != NULL )
										    pSeqLaserCross->SetScaleFactor( pMeshInstance->GetScale() * 0.3f );
										D3DXVECTOR3 vRotateDegree = m_pEveElectraSystem->m_pGUEve->GetRotateDegree();
										vRotateDegree.z = m_pEveElectraSystem->m_fPlaneZAngle;
										pSeqLaserCross->SetAxisAngle( vRotateDegree );
										pSeqLaserCross->SetAddRotate( vRotateDegree );
									}
								}
							}
						}
					} break;
				case PFT_FUSION: // 30���� ����ź�� �Ⱑ ��Ʈ������
					{
						for( UINT i = 1; i < m_vecDamageEffect.size(); ++i )
						{
                            CX2DamageEffect::CEffect* pDamageEffecti = ( m_vecDamageEffect.size() > i )
                                ? g_pX2Game->GetDamageEffect()->GetInstance( m_vecDamageEffect[i] ) : NULL;
						    if( CKTDGXMeshPlayer::CXMeshInstance *pMainEffecti = ( pDamageEffecti != NULL ) ? pDamageEffecti->GetMainEffect() : NULL )
							{
								D3DXVECTOR3 vEffectNowPos = pMainEffecti->GetPos();	// ����Ʈ�� ���� ��ġ
								D3DXVECTOR2 vLockOnRange( 0.f, 0.f );

								if( false == pDamageEffecti->GetCheckSpectro() &&
									0.f < D3DXPlaneDotCoord( &m_pEveElectraSystem->m_GigaStreamPlane, &D3DXVECTOR3( vEffectNowPos ) ) )
								{
									D3DXVECTOR3 vEffectEndPos;
									D3DXPlaneIntersectLine( &vEffectEndPos, &m_pEveElectraSystem->m_GigaStreamPlane, &vEffectNowPos,
										&vPlaneIntersectPoint );

									CX2DamageEffect::CEffect* pEffect = NULL;
                                    CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
									if ( CX2DamageManager::AT_SPECIAL == pDamageEffecti->GetDamageData().attackType )
									{
										pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
											m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER_SPECIAL",
											m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vEffectEndPos,
											m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
                                        if ( (pMeshInstance = pEffect->GetMainEffect()) )
                                        {
										    pMeshInstance->SetNowLifeTime( 0.f );
										    pMeshInstance->SetMaxLifeTime( 2.5f );
                                        }
										vLockOnRange = D3DXVECTOR2( 1, 1500 );
									}
									else
									{
										pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
											m_pEveElectraSystem->m_pGUEve, L"EVE_EBS_INDUCTION_LASER",
											m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vEffectEndPos,
											m_pEveElectraSystem->m_pGUEve->GetRotateDegree(), m_pEveElectraSystem->m_pGUEve->GetRotateDegree() );
                                        if ( (pMeshInstance = pEffect->GetMainEffect()) )
                                        {
										    pMeshInstance->SetNowLifeTime( 0.f );
										    pMeshInstance->SetMaxLifeTime( 1.0f );
                                        }
										vLockOnRange = D3DXVECTOR2( 1, 400 );
									}

									pEffect->SetAttackTime( D3DXVECTOR2( 0.f, 100.f) );
									CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
									*pDamageData = pDamageEffecti->GetDamageData();
			                        pDamageData->hAttackerEffect = pEffect->GetHandle();
									pDamageData->damage.fPhysic		*= 2.0f;
									pDamageData->damage.fMagic		*= 2.0f;
									pDamageData->fHitGap			= 0.5f;
									pDamageData->bReAttack			= true;
									pDamageData->bArrangedFly		= false;
									pDamageData->m_iLaserGroupID	= -1;

									bool bUserUnit = false;
									UidType gameUnitUID = -1;

									if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
										gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
										m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );
									else
										gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
										m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), vLockOnRange, bUserUnit );

									if( bUserUnit == true )
									{
										pEffect->SetLockOnUnitUID( gameUnitUID );
										pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
									}
									else
									{
										pEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
										pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
									}

									pEffect->SetCheckSpectro( true );
									g_pX2Game->GetDamageEffect()->DestroyInstanceSilently( pDamageEffecti );

									CKTDGParticleSystem::CParticleEventSequenceHandle hParticleCross = g_pX2Game->
										GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pEveElectraSystem->m_pGUEve,
										L"eve_spectro_giga_cross_P01", vEffectEndPos );
									CKTDGParticleSystem::CParticleEventSequence* pSeqLaserCross =
										g_pX2Game->GetMajorParticle()->GetInstanceSequence( hParticleCross );

									if( pSeqLaserCross != NULL )
									{
                                        if ( pMeshInstance != NULL )
										    pSeqLaserCross->SetScaleFactor( pMeshInstance->GetScale() * 0.3f );
										D3DXVECTOR3 vRotateDegree = m_pEveElectraSystem->m_pGUEve->GetRotateDegree();
										vRotateDegree.z = m_pEveElectraSystem->m_fPlaneZAngle;
										pSeqLaserCross->SetAxisAngle( vRotateDegree );
										pSeqLaserCross->SetAddRotate( vRotateDegree );
									}
								}
							}
						}
					} break;
				default:
					break;
				}
			}
			//////////// GIGA STREAM ////////////////////////////////////////////////////////}}
		}
		m_bDie			= false;
	}
	else
	{

		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleLaserCross );
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[0] );
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[1] );
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[2] );
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[3] );

        if ( m_vecDamageEffect.empty() == false )
        {
            m_vecDamageEffect[0]	= INVALID_DAMAGE_EFFECT_HANDLE;
        }
		m_bDie					= true;
	}


	return;
}

void CX2GUEve::EveElectraLaserData::Delete()
{
	if( m_vecDamageEffect.empty() == false )
	{
		for(UINT i = 0; i < m_vecDamageEffect.size(); i++ )
		{
            if ( m_vecDamageEffect[i] != INVALID_DAMAGE_EFFECT_HANDLE )
                g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_vecDamageEffect[i] );
		}
	}
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleLaserCross );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[0] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[1] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[2] );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaLaserCross[3] );

}
CX2GUEve::EveElectraEffectData::EveElectraEffectData( EveElectraSystem* _pEveElectraSystem )
{
	m_pEveElectraSystem	= _pEveElectraSystem;
	m_fElapsedTime		= 0.f;
	m_bPlaneFront		= false;
	m_bDie				= false;
    m_hDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
}

void CX2GUEve::EveElectraEffectData::OnFrameMove( float fElapsedTime )
{
	D3DXVECTOR3 vRotateDegree;
	m_fElapsedTime += fElapsedTime;

    CX2DamageEffect::CEffect* m_pDamageEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageEffect );
    if( CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = ( m_pDamageEffect != NULL ) ? m_pDamageEffect->GetMainEffect() : NULL)
	{
		vRotateDegree = pMainEffect->GetRotateDegree();


		D3DXVECTOR3 vEffectNowPos = pMainEffect->GetPos();


		if( m_pEveElectraSystem->GetEnablePlane() == false )
		{
			m_bPlaneFront = false;
		}
		else if( D3DXPlaneDotCoord( &m_pEveElectraSystem->m_PrimaryPlane, &D3DXVECTOR3( vEffectNowPos ) ) < 0.0f )
		{
			m_vPlaneIntersectPoint = vEffectNowPos;
			m_bPlaneFront = true;
		}
		else if( m_bPlaneFront == true )
		{

			D3DXVECTOR3 vEffectEndPos;
			D3DXPlaneIntersectLine( &vEffectEndPos, &m_pEveElectraSystem->m_PrimaryPlane, &vEffectNowPos, &m_vPlaneIntersectPoint );

			switch( m_pEveElectraSystem->m_ePlaneFormationType )
			{
			case PFT_AMPLIFIER:
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
						m_pEveElectraSystem->m_pGUEve, m_AmplifyDamageEffectName.c_str(), m_pDamageEffect->GetPowerRate(),
						vEffectEndPos, vRotateDegree, vRotateDegree );

					CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
					*pDamageData = m_pDamageEffect->GetDamageData();
			        pDamageData->hAttackerEffect = pEffect->GetHandle();

					int iAddDamageTime = m_pEveElectraSystem->m_pGUEve->GetTaserPilumAddDamageTime();

					if ( iAddDamageTime > 0 )
					{
						m_pEveElectraSystem->m_pGUEve->ResetTaserPilumTinyData();
						pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTime );
					}

					int iAddDamageTimeKugel = m_pEveElectraSystem->m_pGUEve->GetKugelBlitzAddDamageTime();

					if ( iAddDamageTimeKugel > 0 )
					{
						m_pEveElectraSystem->m_pGUEve->ResetKugelBlitzTinyData();
						pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTimeKugel );
					}
                    CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
                    if ( pMeshInstance != NULL )
                    {
					    pMeshInstance->SetNowLifeTime( pMainEffect->GetNowLifeTime() );
					    pMeshInstance->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() + 0.13333f );
                    }

					pDamageData->damage.fPhysic *= 1.2f;
					pDamageData->damage.fMagic *= 1.2f;

					D3DXVECTOR3 vAmplifyScale = m_pDamageEffect->GetMainEffectScaleByUnit();
					D3DXVECTOR3 vScale = pMainEffect->GetScale();
					if ( m_AmplifyDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) != 0
						&& m_AmplifyDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) != 0
						&& m_AmplifyDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) != 0
						&& m_AmplifyDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) != 0
						)
					{
						pEffect->SetPassiveparticleTriggerTimeRate( pMainEffect->GetMaxLifeTime() + 0.13333f );
					}
					if ( m_AmplifyDamageEffectName.compare( L"EVE_MEGA_ELECTRON_BALL_FILTERED" ) == 0 )
					{
						float fNowPowerRate = m_pDamageEffect->GetPowerRate();
						const float fPowerRateModifier = 1.2f;
						fNowPowerRate *= fPowerRateModifier;

						// �ް� �Ϸ�Ʈ�� ���� DieDamageEffect ������ ������ ���� �߰�
						pEffect->SetPowerRate(fNowPowerRate);
					}

					vAmplifyScale.x *= vScale.x;
					vAmplifyScale.y *= vScale.y;
					vAmplifyScale.z *= vScale.z;
					vAmplifyScale *= 1.3f + m_pEveElectraSystem->m_fHyperOpticalResearchScale;

					pEffect->SetScale( vAmplifyScale );

					D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;
					vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle;

					if( vRotateDegree.z != 0.f)
						vSecondRotateDegree.z -= vRotateDegree.z;

                    if ( pMeshInstance != NULL )
                    {
					    pMeshInstance->SetRotateDegree( vSecondRotateDegree );
					    pMeshInstance->SetMoveAxisAngleDegree( vSecondRotateDegree );
                    }


				}
				break;
			case PFT_SPECTRUM:
				{
					for(UINT i = 1; i <= 3; i++ )
					{
						CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
							m_pEveElectraSystem->m_pGUEve, m_SpectrumDamageEffectName.c_str(), m_pDamageEffect->GetPowerRate(),
							vEffectEndPos, vRotateDegree, vRotateDegree );

						CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
						*pDamageData = m_pDamageEffect->GetDamageData();
			            pDamageData->hAttackerEffect = pEffect->GetHandle();
						int iAddDamageTime = m_pEveElectraSystem->m_pGUEve->GetTaserPilumAddDamageTime();

						if ( iAddDamageTime > 0 )
						{
							m_pEveElectraSystem->m_pGUEve->ResetTaserPilumTinyData();
							pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTime );
						}

						int iAddDamageTimeKugel = m_pEveElectraSystem->m_pGUEve->GetKugelBlitzAddDamageTime();

						if ( iAddDamageTimeKugel > 0 )
						{
							m_pEveElectraSystem->m_pGUEve->ResetKugelBlitzTinyData();
							pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTimeKugel );
						}
                        CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
                        if ( pMeshInstance != NULL )
                        {
						    pMeshInstance->SetNowLifeTime( pMainEffect->GetNowLifeTime() );
						    pMeshInstance->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() + 0.13333f );
                        }
						if ( m_SpectrumDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) != 0
							&& m_SpectrumDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) != 0
							&& m_SpectrumDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) != 0
							&& m_SpectrumDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) != 0
							)
						{
							pEffect->SetPassiveparticleTriggerTimeRate( pMainEffect->GetMaxLifeTime() + 0.13333f );
						}
						pDamageData->fForceDownValue	*= 0.333f;

						float fNowPowerRate = m_pDamageEffect->GetPowerRate();
						// ���ݷ� ����
						{
							const float fPowerRateModifier = 0.45f;
							pDamageData->damage.fPhysic *= fPowerRateModifier;
							pDamageData->damage.fMagic *= fPowerRateModifier;
							fNowPowerRate *= fPowerRateModifier;

							// �Ϲ� �Ϸ�Ʈ�� ���� ���帶�� �ٸ� ������ ����Ʈ�� ����Ͽ�
							// �������� �̹� �����Ǿ� �ִ�.
							if ( m_SpectrumDamageEffectName.compare( L"EVE_MEGA_ELECTRON_BALL_FILTERED" ) == 0 )
							{
								// �ް� �Ϸ�Ʈ�� ���� DieDamageEffect ������ ������ ���� �߰�
								pEffect->SetPowerRate(fNowPowerRate);
							}
						}

						D3DXVECTOR3 vSpectrumScale = m_pDamageEffect->GetMainEffectScaleByUnit();
						D3DXVECTOR3 vScale = pMainEffect->GetScale();

						vSpectrumScale.x *= vScale.x;
						vSpectrumScale.y *= vScale.y;
						vSpectrumScale.z *= vScale.z;
						vSpectrumScale *= 0.8f;

						pEffect->SetScale( vSpectrumScale );

						D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

						if( i == 2)
							vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle + 30.f;
						else if( i == 3)
							vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle - 30.f;
						else
							vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle;

						if( vRotateDegree.z != 0.f)
							vSecondRotateDegree.z -= vRotateDegree.z;

                        if ( pMeshInstance != NULL )
                        {
						    pMeshInstance->SetRotateDegree( vSecondRotateDegree );
						    pMeshInstance->SetMoveAxisAngleDegree( vSecondRotateDegree );
                        }
					}
				}
				break;
			case PFT_INDUCTION:		/// �Ϸ�Ʈ�� ��, �ް� �Ϸ�Ʈ�� ��, ������ �ʶ�, ������ �Ѹ�
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
						m_pEveElectraSystem->m_pGUEve, m_InductionDamageEffectName.c_str(), m_pDamageEffect->GetPowerRate(),
						vEffectEndPos, vRotateDegree, vRotateDegree );

					CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
					*pDamageData = m_pDamageEffect->GetDamageData();
			        pDamageData->hAttackerEffect = pEffect->GetHandle();

                    CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
                    if ( pMeshInstance != NULL )
					    pMeshInstance->SetNowLifeTime( pMainEffect->GetNowLifeTime() );

					if ( m_InductionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) != 0
						&& m_InductionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) != 0
						&& m_InductionDamageEffectName.compare( L"EVE_ENERGY_NEEDLES_FILTERED" ) != 0
						&& m_InductionDamageEffectName.compare( L"EVE_TASER_PILUM_CHARGE_0_FILTERED" ) != 0
						&& m_InductionDamageEffectName.compare( L"EVE_TASER_PILUM_CHARGE_1_FILTERED" ) != 0
						&& m_InductionDamageEffectName.compare( L"EVE_TASER_PILUM_CHARGE_2_FILTERED" ) != 0
						&& m_InductionDamageEffectName.compare( L"Damage_KugelBlitz_Charge_FILTERED" ) != 0
						&& m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) != 0
						&& m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) != 0
						)
					{
                        if ( pMeshInstance != NULL )
						    pMeshInstance->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() * 1.1f + 0.13333f );
						pEffect->SetPassiveparticleTriggerTimeRate( pMainEffect->GetMaxLifeTime() * 1.1f + 0.13333f );
						pEffect->SetDamageTime( pEffect->GetDamageTime() + 1 );

						pDamageData->damage.fPhysic *= 0.55f;
						pDamageData->damage.fMagic *= 0.55f;
						pDamageData->fHitGap = 0.3f;

						pDamageData->bReAttack			= true;
						pDamageData->fHitAddMP			*= 0.55f;
						pDamageData->m_fRateModifier	*= 0.55f;
						pDamageData->techPoint			= int(pDamageData->techPoint * 0.55);

						D3DXVECTOR3 vInductionScale = m_pDamageEffect->GetMainEffectScaleByUnit();
						D3DXVECTOR3 vScale = pMainEffect->GetScale();

						vInductionScale.x *= vScale.x;
						vInductionScale.y *= vScale.y;
						vInductionScale.z *= vScale.z;

						vInductionScale *= 0.8f;

						pEffect->SetScale( vInductionScale );
						if ( m_InductionDamageEffectName.compare( L"EVE_MEGA_ELECTRON_BALL_FILTERED" ) == 0 )
						{
							float fNowPowerRate = m_pDamageEffect->GetPowerRate();
							const float fPowerRateModifier = 0.55f;
							fNowPowerRate *= fPowerRateModifier;

							// �ް� �Ϸ�Ʈ�� ���� DieDamageEffect ������ ������ ���� �߰�
							pEffect->SetPowerRate(fNowPowerRate);
						}

					}
					else if ( m_InductionDamageEffectName.compare( L"Damage_KugelBlitz_Charge_FILTERED" ) == 0 )
					{
						int iAddDamageTime = m_pEveElectraSystem->m_pGUEve->GetKugelBlitzAddDamageTime();
						D3DXVECTOR3 vInductionScale = m_pDamageEffect->GetMainEffectScaleByUnit();
						D3DXVECTOR3 vScale = pMainEffect->GetScale();

						vInductionScale.x *= vScale.x;
						vInductionScale.y *= vScale.y;
						vInductionScale.z *= vScale.z;

						vInductionScale *= 0.8f;

						pEffect->SetScale( vInductionScale );


						if ( iAddDamageTime > 0 )
						{
							m_pEveElectraSystem->m_pGUEve->ResetKugelBlitzTinyData();
							pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTime );
						}
					}

					else if ( m_InductionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) == 0 ||
							m_InductionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) == 0 ||
							m_InductionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) == 0 ||
							m_InductionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) == 0 )
					{
						// Ʈ���� ��Ŭ ��ĭ�� ��� �Ʒ��� ��쿡 ���� ���������� ó�� �Ѵ�.
						// ũ�� 0.8�� ����
						D3DXVECTOR3 vInductionScale = m_pDamageEffect->GetMainEffectScaleByUnit();
						D3DXVECTOR3 vScale = pMainEffect->GetScale();

						vInductionScale.x *= vScale.x;
						vInductionScale.y *= vScale.y;
						vInductionScale.z *= vScale.z;

						vInductionScale *= 0.8f;

						pEffect->SetScale( vInductionScale );
					}

					else
					{
						int iAddDamageTime = m_pEveElectraSystem->m_pGUEve->GetTaserPilumAddDamageTime();
						m_pEveElectraSystem->m_pGUEve->ResetTaserPilumTinyData();
						pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTime );
					}

					if ( m_InductionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) == 0
						|| m_InductionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) == 0
						|| m_InductionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) == 0
						|| m_InductionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) == 0
						)
					{

						// Ʈ���� ��Ŭ ��ĭ�� ��� �Ʒ��� ��쿡 ���� ���������� ó�� �Ѵ�.
						// ������ 1.1�� ����, R
						pDamageData->damage.fPhysic *= 1.1f;
						pDamageData->damage.fMagic *= 1.1f;

					}
					D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;
					vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle;

					if( vRotateDegree.z != 0.f)
						vSecondRotateDegree.z -= vRotateDegree.z;

					if ( pMeshInstance != NULL )
					{
						pMeshInstance->SetRotateDegree( vSecondRotateDegree );
						pMeshInstance->SetMoveAxisAngleDegree( vSecondRotateDegree );

						pMeshInstance->SetDirSpeed( pEffect->GetMainEffect()->GetVelocity().x );
						pMeshInstance->SetVelocity( D3DXVECTOR3( 0.f, 0.f, 0.f ) );
					}

					pEffect->GetLockOnData().m_LockOnType = CX2DamageEffect::LOT_NEARST_UID_VECTOR_IN_RANGE;
					pEffect->GetLockOnData().m_fCurveSpeed = 10;
					pEffect->GetLockOnData().m_fTimeUnlockFromStart = 0.1f;

					// Ʈ���� ��Ŭ ��ĭ�� ��� �Ʒ��� ��쿡 ���� ���������� ó�� �Ѵ�.
					// NOT_TARGET_PRE_LOCK_ON_TARGET �߰� ( ���� ������ �ִٸ� ���� ������ ������ ��, ü�� ����Ʈ�� )
					if ( m_InductionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) == 0
						|| m_InductionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) == 0
						)
					{
						pEffect->GetLockOnData().m_bIsNotTargetPreLockOnTarget = true;
					}

					bool bUserUnit = false;
					UidType gameUnitUID = -1;

					if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
						gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
						m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), D3DXVECTOR2(0, 1000), bUserUnit );
					else
						gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
						m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), D3DXVECTOR2(0, 1500), bUserUnit );

					if( bUserUnit == true )
					{
						pEffect->SetLockOnUnitUID( gameUnitUID );
						pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
					}
					else
					{
						pEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
						pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
					}
				} break;

			case PFT_FUSION:
				{
					for(UINT i = 1; i <= 3; i++ )
					{
						CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance(
							m_pEveElectraSystem->m_pGUEve, m_FusionDamageEffectName.c_str(), m_pDamageEffect->GetPowerRate(),
							vEffectEndPos, vRotateDegree, vRotateDegree );

						CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
						*pDamageData = m_pDamageEffect->GetDamageData();
			            pDamageData->hAttackerEffect = pEffect->GetHandle();

                        CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
                        if ( pMeshInstance != NULL )
						    pMeshInstance->SetNowLifeTime( pMainEffect->GetNowLifeTime() );
						float fNowPowerRate = m_pDamageEffect->GetPowerRate();

						if ( m_FusionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) != 0
							&& m_FusionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) != 0
							&& m_FusionDamageEffectName.compare( L"EVE_ENERGY_NEEDLES_FILTERED" ) != 0
							&& m_FusionDamageEffectName.compare( L"EVE_TASER_PILUM_CHARGE_0_FILTERED" ) != 0
							&& m_FusionDamageEffectName.compare( L"EVE_TASER_PILUM_CHARGE_1_FILTERED" ) != 0
							&& m_FusionDamageEffectName.compare( L"EVE_TASER_PILUM_CHARGE_2_FILTERED" ) != 0
							&& m_FusionDamageEffectName.compare( L"Damage_KugelBlitz_Charge_FILTERED" ) != 0
							&& m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) != 0
							&& m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) != 0
							)
						{
                            if ( pMeshInstance != NULL )
							    pMeshInstance->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() * 1.1f + 0.13333f );
							pEffect->SetPassiveparticleTriggerTimeRate( pMainEffect->GetMaxLifeTime() * 1.1f + 0.13333f );

							pEffect->SetDamageTime( pEffect->GetDamageTime() + 1 );

							// ���ݷ� ����
							{
								const float fPowerRateModifier = 0.5f;
								pDamageData->damage.fPhysic *= fPowerRateModifier;
								pDamageData->damage.fMagic *= fPowerRateModifier;
								fNowPowerRate *= fPowerRateModifier;
							}

							pDamageData->fHitGap = 0.3f;
							pDamageData->bReAttack			= true;
							pDamageData->m_fRateModifier	*= 0.25f;


						}
						else
						{
							int iAddDamageTime = m_pEveElectraSystem->m_pGUEve->GetTaserPilumAddDamageTime();

							if ( iAddDamageTime > 0 )
							{
								m_pEveElectraSystem->m_pGUEve->ResetTaserPilumTinyData();
								pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTime );
							}

							int iAddDamageTimeKugel = m_pEveElectraSystem->m_pGUEve->GetKugelBlitzAddDamageTime();

							if ( iAddDamageTimeKugel > 0 )
							{
								m_pEveElectraSystem->m_pGUEve->ResetKugelBlitzTinyData();
								pEffect->SetDamageTime( pEffect->GetDamageTime() + iAddDamageTimeKugel );
							}
						}


						{
							// ���ݷ� ����

							const float fPowerRateModifier = 0.5f;

							pDamageData->damage.fPhysic *= fPowerRateModifier;
							pDamageData->damage.fMagic *= fPowerRateModifier;
							fNowPowerRate *= fPowerRateModifier;

							// �ް� �Ϸ�Ʈ�� ���� DieDamageEffect ������ ������ ���� �߰�
							if ( m_InductionDamageEffectName.compare( L"EVE_MEGA_ELECTRON_BALL_FILTERED" ) == 0 )
							{
								pEffect->SetPowerRate( fNowPowerRate );
							}
						}

						if ( m_FusionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE" ) == 0 ||
							m_FusionDamageEffectName.compare( L"EVE_SWEEP_PARADE_TRIANGLE_MEMO" ) == 0 )
						{
							pDamageData->fHitAddMP *= 0.5f;
						}
						else if ( m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) == 0
								|| m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) == 0
							)
						{
							// �뷱�� �������� Ʈ���� ��ĭ�� �������� ������ ��, Hitgap ���� ����
							pDamageData->fHitAddMP *= 0.25f;
						}
						pDamageData->techPoint = int(pDamageData->techPoint * 0.5);

						D3DXVECTOR3 vFusionScale = m_pDamageEffect->GetMainEffectScaleByUnit();
						D3DXVECTOR3 vScale = pMainEffect->GetScale();

						vFusionScale.x *= vScale.x;
						vFusionScale.y *= vScale.y;
						vFusionScale.z *= vScale.z;

						vFusionScale *= 0.75f;

						pEffect->SetScale( vFusionScale );

						D3DXVECTOR3 vSecondRotateDegree = vRotateDegree;

						if( i == 2)
							vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle + 30.f;
						else if( i == 3)
							vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle - 30.f;
						else
							vSecondRotateDegree.z += m_pEveElectraSystem->m_fPlaneZAngle;

						if( vRotateDegree.z != 0.f)
							vSecondRotateDegree.z -= vRotateDegree.z;

						if ( pMeshInstance != NULL )
						{
							pMeshInstance->SetRotateDegree( vSecondRotateDegree );
							pMeshInstance->SetMoveAxisAngleDegree( vSecondRotateDegree );

							pMeshInstance->SetDirSpeed( pMeshInstance->GetVelocity().x );
							pMeshInstance->SetVelocity( D3DXVECTOR3( 0.f, 0.f, 0.f ) );
						}
						pEffect->GetLockOnData().m_LockOnType = CX2DamageEffect::LOT_NEARST_UID_VECTOR;
						pEffect->GetLockOnData().m_fCurveSpeed = 10;
						pEffect->GetLockOnData().m_fTimeUnlockFromStart = 0.1f;

						if ( m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet" ) == 0
							|| m_FusionDamageEffectName.compare( L"DamageEffect_Triangle_Vulcan_Bullet_MEMO" ) == 0
							)
						{
							// �뷱�� �������� Ʈ���� ��ĭ�� �������� ������ ��, NOT_TARGET_PRE_LOCK_ON_TARGET �ɼ��� ����
							pEffect->GetLockOnData().m_bIsNotTargetPreLockOnTarget = true;
						}
						bool bUserUnit = false;
						UidType gameUnitUID = -1;

						if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
							gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
							m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), D3DXVECTOR2(0, 1000), bUserUnit );
						else
							gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>(
							m_pEveElectraSystem->m_pGUEve->GetTeam() ), pEffect->GetPos(), D3DXVECTOR2(0, 1500), bUserUnit );

						if( bUserUnit == true )
						{
							pEffect->SetLockOnUnitUID( gameUnitUID );
							pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
						}
						else
						{
							pEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
							pEffect->SetLockOnRandomSeed( (int)m_pEveElectraSystem->m_pGUEve->GetRandomInt() );
						}
					}
				} break;
			default:
				break;
			}
            g_pX2Game->GetDamageEffect()->DestroyInstanceHandleSilently( m_hDamageEffect );
			m_bDie = true;

		}
	}
	else
	{
		m_bDie = true;
        m_hDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
	}


	return;
}
CX2GUEve::EveElectraLockonData::EveElectraLockonData( EveElectraSystem* _pEveElectraSystem )
{
	m_pEveElectraSystem	= _pEveElectraSystem;
	m_fElapsedTime		= 0.f;
	m_bPlaneFront		= false;
	m_bDie				= false;
    m_hDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
}

void CX2GUEve::EveElectraLockonData::OnFrameMove( float fElapsedTime )
{
	D3DXVECTOR3 vRotateDegree;
	m_fElapsedTime += fElapsedTime;

    CX2DamageEffect::CEffect* m_pDamageEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageEffect );
    if( CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = ( m_pDamageEffect != NULL ) ? m_pDamageEffect->GetMainEffect() : NULL )
	{
		vRotateDegree = pMainEffect->GetRotateDegree();


		D3DXVECTOR3 vEffectNowPos = pMainEffect->GetPos();


		if( m_pEveElectraSystem->GetEnablePlane() == false )
		{
			m_bPlaneFront = false;
		}
		else if( D3DXPlaneDotCoord( &m_pEveElectraSystem->m_PrimaryPlane, &D3DXVECTOR3( vEffectNowPos ) ) < 0.0f )
		{
			m_vPlaneIntersectPoint = vEffectNowPos;
			m_bPlaneFront = true;
		}
		else if( m_bPlaneFront == true )
		{

			D3DXVECTOR3 vEffectEndPos;
			D3DXPlaneIntersectLine( &vEffectEndPos, &m_pEveElectraSystem->m_PrimaryPlane, &vEffectNowPos, &m_vPlaneIntersectPoint );

			switch( m_pEveElectraSystem->m_ePlaneFormationType )
			{
			case PFT_AMPLIFIER:
				{
					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_AmplifyDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vEffectEndPos, vRotateDegree, vRotateDegree );

					CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
					*pDamageData = m_pDamageEffect->GetDamageData();
			        pDamageData->hAttackerEffect = pEffect->GetHandle();

					pEffect->GetLockOnData().m_bLockOnUserUnit	= m_pDamageEffect->GetLockOnData().m_bLockOnUserUnit;
					pEffect->GetLockOnData().m_LockOnUnitUID	= m_pDamageEffect->GetLockOnData().m_LockOnUnitUID;
					pEffect->GetLockOnData().m_LockOnNPCUID	= m_pDamageEffect->GetLockOnData().m_LockOnNPCUID;

                    CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
                    if ( pMeshInstance != NULL )
                    {
					    pMeshInstance->SetNowLifeTime( pMainEffect->GetNowLifeTime() );
					    pMeshInstance->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() + 0.2f );
                    }

					pDamageData->damage.fPhysic *= 1.2f;
					pDamageData->damage.fMagic *= 1.2f;

					pEffect->SetScale( D3DXVECTOR3( 1.3f, 1.3f, 1.3f ) );

					//D3DXVECTOR3 vManualDirDegree = GetDirVecToDegree( m_pEveElectraSystem->m_vPlaneNormal );

					D3DXVECTOR3 vManualDirDegree;

					vManualDirDegree.x = D3DXToDegree(m_pEveElectraSystem->m_vPlaneNormal.x);
					vManualDirDegree.y = D3DXToDegree(m_pEveElectraSystem->m_vPlaneNormal.y);
					vManualDirDegree.z = D3DXToDegree(m_pEveElectraSystem->m_vPlaneNormal.z);

                    if ( pMeshInstance != NULL )
                    {
					    pMeshInstance->SetManualDirDegree( vManualDirDegree );
					    pMeshInstance->UseManualDirDegree( true );
                    }

				}
				break;
			case PFT_SPECTRUM:
				{
					for(UINT i = 1; i <= 3; i++ )
					{
						CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( m_pEveElectraSystem->m_pGUEve, m_SpectrumDamageEffectName.c_str(), m_pEveElectraSystem->m_pGUEve->GetPowerRate(), vEffectEndPos, vRotateDegree, vRotateDegree );

						CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();
						*pDamageData = m_pDamageEffect->GetDamageData();
			            pDamageData->hAttackerEffect = pEffect->GetHandle();

						pEffect->GetLockOnData().m_bLockOnUserUnit	= m_pDamageEffect->GetLockOnData().m_bLockOnUserUnit;
						pEffect->GetLockOnData().m_LockOnUnitUID	= m_pDamageEffect->GetLockOnData().m_LockOnUnitUID;
						pEffect->GetLockOnData().m_LockOnNPCUID	= m_pDamageEffect->GetLockOnData().m_LockOnNPCUID;

                        CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
                        if ( pMeshInstance != NULL )
                        {
    						pMeshInstance->SetNowLifeTime( pMainEffect->GetNowLifeTime() );
	    					pMeshInstance->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() + 0.2f );
                        }

						pDamageData->damage.fPhysic *= 0.5f;
						pDamageData->damage.fMagic *= 0.5f;

						pEffect->SetScale( D3DXVECTOR3( 0.8f, 0.8f, 0.8f ) );


						D3DXVECTOR3 vRotationAxis;
						D3DXVec3Cross( &vRotationAxis, &m_pEveElectraSystem->m_vPlaneNormal, &Y_AXIS );
						D3DXVec3Normalize( &vRotationAxis, &vRotationAxis );


						D3DXMATRIX matRotation_2;
						D3DXMatrixRotationAxis( &matRotation_2, &vRotationAxis, D3DX_PI / 180.f * 30.f );

						D3DXMATRIX matRotation_3;
						D3DXMatrixRotationAxis( &matRotation_3, &vRotationAxis, D3DX_PI / 180.f * -30.f );


						D3DXVECTOR4 v4DirVectorLockon_2;
						D3DXVec3Transform( &v4DirVectorLockon_2, &m_pEveElectraSystem->m_vPlaneNormal, &matRotation_2 );
						D3DXVECTOR3 vDirVectorLockon_2( v4DirVectorLockon_2.x, v4DirVectorLockon_2.y, v4DirVectorLockon_2.z );

						D3DXVECTOR4 v4DirVectorLockon_3;
						D3DXVec3Transform( &v4DirVectorLockon_3, &m_pEveElectraSystem->m_vPlaneNormal, &matRotation_3 );
						D3DXVECTOR3 vDirVectorLockon_3( v4DirVectorLockon_3.x, v4DirVectorLockon_3.y, v4DirVectorLockon_3.z );

						D3DXVECTOR3 vManualDirDegree1;
						vManualDirDegree1.x = D3DXToDegree(m_pEveElectraSystem->m_vPlaneNormal.x);
						vManualDirDegree1.y = D3DXToDegree(m_pEveElectraSystem->m_vPlaneNormal.y);
						vManualDirDegree1.z = D3DXToDegree(m_pEveElectraSystem->m_vPlaneNormal.z);

						D3DXVECTOR3 vManualDirDegree2;
						vManualDirDegree2.x = D3DXToDegree(vDirVectorLockon_2.x);
						vManualDirDegree2.y = D3DXToDegree(vDirVectorLockon_2.y);
						vManualDirDegree2.z = D3DXToDegree(vDirVectorLockon_2.z);

						D3DXVECTOR3 vManualDirDegree3;
						vManualDirDegree3.x = D3DXToDegree(vDirVectorLockon_3.x);
						vManualDirDegree3.y = D3DXToDegree(vDirVectorLockon_3.y);
						vManualDirDegree3.z = D3DXToDegree(vDirVectorLockon_3.z);


                        if ( pMeshInstance != NULL )
                        {
						    if( i == 2)
							    pMeshInstance->SetManualDirDegree( vManualDirDegree2 );
						    else if( i == 3)
							    pMeshInstance->SetManualDirDegree( vManualDirDegree3 );
						    else
							    pMeshInstance->SetManualDirDegree( vManualDirDegree1 );

						    pMeshInstance->UseManualDirDegree( true );
                        }
					}
				}
				break;
			default:
				break;
			}
            g_pX2Game->GetDamageEffect()->DestroyInstanceHandleSilently( m_hDamageEffect );
			m_bDie = true;

		}
	}
	else
	{
		m_bDie = true;
        m_hDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
	}
	return;
}

CX2GUEve::EveElectraSystem::EveElectraSystem( CX2GUEve* _pGUEve )
{
	m_pGUEve					= _pGUEve;
	m_fPlaneZAngle				= 0.f;
	m_fLastPlaneZAngle			= 0.f;
	m_bSystemEnable				= false;
	m_bPlaneEnable				= false;
	m_bSpectrumDynamicsEnable	= false;
	m_bHyperOpticalResearch			= false;
	m_fHyperOpticalResearchScale	= 0.f;

	m_bExceptionSystem					= false;
	m_fExceptionSystemOperationTime		= 0.f;
	m_fExceptionSystemRemainTime		= 0.f;
	m_fExceptionFormationSetElapsedTime	= 0.f;
	m_ePrevSpectro						= PFT_NONE;

	m_fEveZAngle	= 0.f;
	m_iLaserGroupNum = 1;
	m_fSystemOperationTime		= 0.f;
	m_fFormationSetElapsedTime	= 0.f;
	m_ePlaneFormationType	= PFT_AMPLIFIER;

	m_fLaserRangeScale			= 1.f;

	m_bGigaPlaneEnable			= false;
	m_fGigaPlaneZAngle			= 0.f;

	m_hEffectPlane		= INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectCrystal1	= INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectCrystal2	= INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectCrystal3	= INVALID_MESH_INSTANCE_HANDLE;

	m_hParticlePlane		= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hParticleCrystal[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleCrystal[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleCrystal[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hParticleFormationMark[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleFormationMark[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleFormationMark[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hParticleFormationMsg[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleFormationMsg[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleFormationMsg[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hEffectGigaPlane		= INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectGigaCrystal[0] = INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectGigaCrystal[1] = INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectGigaCrystal[2] = INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectGigaCrystal[3] = INVALID_MESH_INSTANCE_HANDLE;
	m_hEffectGigaCrystal[4] = INVALID_MESH_INSTANCE_HANDLE;

	m_hParticleGigaPlane		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaCrystal[0]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaCrystal[1]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaCrystal[2]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaCrystal[3]	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hParticleGigaCrystal[4]	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hParticleThousandStarFormation = INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_fLaserEndOffsetX	= 0.f;
	m_fLaserEndOffsetY	= 0.f;

	m_iPriorEffectID		= 1 * ( _pGUEve->GetUnitIndex() + 1 );
	m_iPosteriorEffectID	= 1 * ( _pGUEve->GetUnitIndex() + 1 );

	//m_pEffectPlane = NULL;
}

void CX2GUEve::EveElectraSystem::ProcessPlane( float _fElapsedTime )
{

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectPlane );
	if( pMeshInstPlane != NULL )
	{
		if( m_fPlaneZAngle > 0 )
		{
			m_vPlaneNormal	= m_vPlaneNormalUpper;
			m_PrimaryPlane  = m_PlaneUpper;
			D3DXVECTOR3 vRotateDegree = pMeshInstPlane->GetRotateDegree();
			vRotateDegree.z += _fElapsedTime * 120.f;

			if( vRotateDegree.z > m_fPlaneZAngle)
				vRotateDegree.z = m_fPlaneZAngle;

			pMeshInstPlane->SetRotateDegree( vRotateDegree );
			pMeshInstPlane->SetMoveAxisAngleDegree( vRotateDegree );
		}
		else if( m_fPlaneZAngle < 0 )
		{
			m_vPlaneNormal	= m_vPlaneNormalLower;
			m_PrimaryPlane  = m_PlaneLower;

			D3DXVECTOR3 vRotateDegree = pMeshInstPlane->GetRotateDegree();
			vRotateDegree.z += _fElapsedTime * -120.f;

			if( vRotateDegree.z < m_fPlaneZAngle)
				vRotateDegree.z = m_fPlaneZAngle;

			pMeshInstPlane->SetRotateDegree( vRotateDegree );
			pMeshInstPlane->SetMoveAxisAngleDegree( vRotateDegree );
		}
		else
		{
			m_vPlaneNormal	= m_vPlaneNormalCenter;
			m_PrimaryPlane  = m_PlaneCenter;
			pMeshInstPlane->SetRotateDegree( m_vPlaneRotateDegreeCenter );
			pMeshInstPlane->SetMoveAxisAngleDegree( m_vPlaneRotateDegreeCenter );
		}

		CKTDGXMeshPlayer* pMajorMeshPlayer = g_pX2Game->GetMajorXMeshPlayer();
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal1 = pMajorMeshPlayer->GetMeshInstance( m_hEffectCrystal1 );
		pMeshInstCrystal1->SetPos( pMeshInstPlane->GetBonePos( L"Dummy02" ));

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal2 = pMajorMeshPlayer->GetMeshInstance( m_hEffectCrystal2 );
		pMeshInstCrystal2->SetPos( pMeshInstPlane->GetBonePos( L"Dummy03" ));

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal3 = pMajorMeshPlayer->GetMeshInstance( m_hEffectCrystal3 );
		pMeshInstCrystal3->SetPos( pMeshInstPlane->GetBonePos( L"Dummy04" ));


		CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();
		CKTDGParticleSystem::CParticleEventSequence* pSeqPlane = pMajorParticleSystem->GetInstanceSequence( m_hParticlePlane );
		if( pSeqPlane != NULL )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = pMeshInstPlane->GetCloneFrame( L"Dummy01" );
			pSeqPlane->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}


		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal1 = pMajorParticleSystem->GetInstanceSequence( m_hParticleCrystal[0] );
		if( pSeqCrystal1 != NULL )
		{
			pSeqCrystal1->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy02" ) );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal2 = pMajorParticleSystem->GetInstanceSequence( m_hParticleCrystal[1] );
		if( pSeqCrystal2 != NULL )
		{
			pSeqCrystal2->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy03" ) );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal3 = pMajorParticleSystem->GetInstanceSequence( m_hParticleCrystal[2] );
		if( pSeqCrystal3 != NULL )
		{
			pSeqCrystal3->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy04" ) );
		}

		if( pMeshInstPlane->EventTimerGlobal( 0.1f ) == true )
		{
			if( m_ePlaneFormationType == PFT_AMPLIFIER )
				m_hParticlePlane = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_prism_Red_P01", pMeshInstPlane->GetBonePos( L"Dummy01" ) );
			else if( m_ePlaneFormationType == PFT_SPECTRUM )
				m_hParticlePlane = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_prism_Blue_P01", pMeshInstPlane->GetBonePos( L"Dummy01" ) );
			else if( m_ePlaneFormationType == PFT_INDUCTION )
				m_hParticlePlane = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_prism_Green_P01", pMeshInstPlane->GetBonePos( L"Dummy01" ) );
			else if( m_ePlaneFormationType == PFT_FUSION )
				m_hParticlePlane = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"EBS_SA_energeticHeart_prism_White_P01", pMeshInstPlane->GetBonePos( L"Dummy01" ) );

		}
		else if( pMeshInstPlane->EventTimerGlobal( 0.2f ) == true )
		{
			// Plane Loop ����Ʈ ����

			m_hParticleCrystal[0] = pMajorParticleSystem->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy02" ) );
			m_hParticleCrystal[1] = pMajorParticleSystem->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy03" ) );
			m_hParticleCrystal[2] = pMajorParticleSystem->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy04" ) );
		}
		else if( pMeshInstPlane->EventTimerGlobal( pMeshInstPlane->GetMaxLifeTime() - 0.2333f ) == true )
		{
			// Plane End ����Ʈ ����

			pMeshInstCrystal1->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );
			pMeshInstCrystal2->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );
			pMeshInstCrystal3->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );

			pMajorParticleSystem->DestroyInstanceHandle( m_hParticleCrystal[0] );
			pMajorParticleSystem->DestroyInstanceHandle( m_hParticleCrystal[1] );
			pMajorParticleSystem->DestroyInstanceHandle( m_hParticleCrystal[2] );
		}
		else if( pMeshInstPlane->EventTimerGlobal( pMeshInstPlane->GetMaxLifeTime() - 0.1f ) == true )
		{
			pMajorParticleSystem->DestroyInstanceHandle( m_hParticlePlane );
		}
	}
	else
	{
		m_hEffectPlane = INVALID_MESH_INSTANCE_HANDLE;	/// �ð��� ���� �Ҹ� ���� ���̱⿡ INVALID_HANDLE ����
		DestroyPlaneAndCrystalEffect();
		SetEnablePlane( false );
	}

}



void CX2GUEve::EveElectraSystem::ProcessGigaPlane( float /*_fElapsedTime*/ )
{
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectGigaPlane );
	if( pMeshInstPlane != NULL )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal1 = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectGigaCrystal[0] );
		pMeshInstCrystal1->SetPos( pMeshInstPlane->GetBonePos( L"Dummy02" ));

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal2 = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectGigaCrystal[1] );
		pMeshInstCrystal2->SetPos( pMeshInstPlane->GetBonePos( L"Dummy03" ));

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal3 = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectGigaCrystal[2] );
		pMeshInstCrystal3->SetPos( pMeshInstPlane->GetBonePos( L"Dummy04" ));

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal4 = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectGigaCrystal[3] );
		pMeshInstCrystal4->SetPos( pMeshInstPlane->GetBonePos( L"Dummy05" ));

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal5 = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hEffectGigaCrystal[4] );
		pMeshInstCrystal5->SetPos( pMeshInstPlane->GetBonePos( L"Dummy06" ));


		CKTDGParticleSystem::CParticleEventSequence* pSeqPlane = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaPlane );
		if( pSeqPlane != NULL )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = pMeshInstPlane->GetCloneFrame( L"Dummy01" );
			pSeqPlane->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}


		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal1 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaCrystal[0] );
		if( pSeqCrystal1 != NULL )
		{
			pSeqCrystal1->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy02" ) );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal2 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaCrystal[1] );
		if( pSeqCrystal2 != NULL )
		{
			pSeqCrystal2->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy03" ) );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal3 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaCrystal[2] );
		if( pSeqCrystal3 != NULL )
		{
			pSeqCrystal3->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy04" ) );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal4 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaCrystal[3] );
		if( pSeqCrystal4 != NULL )
		{
			pSeqCrystal4->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy05" ) );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqCrystal5 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleGigaCrystal[4] );
		if( pSeqCrystal5 != NULL )
		{
			pSeqCrystal5->SetPosition( pMeshInstPlane->GetBonePos( L"Dummy06" ) );
		}

		if( pMeshInstPlane->EventTimerGlobal( 0.1f ) == true )
		{
			m_hParticleGigaPlane = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_gigaStream_prism_red_P01", pMeshInstPlane->GetBonePos( L"Dummy01" ) );
		}
		else if( pMeshInstPlane->EventTimerGlobal( 0.2f ) == true )
		{
			// Plane Loop ����Ʈ ����
			m_hParticleGigaCrystal[0] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy02" ) );
			m_hParticleGigaCrystal[1] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy03" ) );
			m_hParticleGigaCrystal[2] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy04" ) );
			m_hParticleGigaCrystal[3] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy05" ) );
			m_hParticleGigaCrystal[4] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P01", pMeshInstPlane->GetBonePos( L"Dummy06" ) );
		}
		else if( pMeshInstPlane->EventTimerGlobal( pMeshInstPlane->GetMaxLifeTime() - 0.2333f ) == true )
		{
			// Plane End ����Ʈ ����

			pMeshInstCrystal1->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );
			pMeshInstCrystal2->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );
			pMeshInstCrystal3->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );
			pMeshInstCrystal4->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );
			pMeshInstCrystal5->ChangeAnim( L"end", CKTDGXSkinAnim::XAP_ONE, 1.f );

			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaCrystal[0] );
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaCrystal[1] );
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaCrystal[2] );
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaCrystal[3] );
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaCrystal[4] );
		}
		else if( pMeshInstPlane->EventTimerGlobal( pMeshInstPlane->GetMaxLifeTime() - 0.1f ) == true )
		{
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleGigaPlane );
		}
	}
	else
	{
        m_hParticleGigaPlane = INVALID_PARTICLE_SEQUENCE_HANDLE;	/// �ð��� ���� ������� ���̱⿡ INVALID ó����
		DestroyGigaPlaneAndCrystalEffect();
		SetEnableGigaPlane( false );
	}

}


void CX2GUEve::EveElectraSystem::SetPlaneFormation( PlaneFormationType _ePlaneFormationType )
{
	m_ePlaneFormationType = _ePlaneFormationType;
	return;
}

void CX2GUEve::EveElectraSystem::ChangePlaneFormation()
{
	switch( m_ePlaneFormationType )
	{
	case PFT_AMPLIFIER:
		SetPlaneFormation( PFT_SPECTRUM );
		break;

	case PFT_SPECTRUM:
		{
			if ( GetEnableHyperOpticalResearch() )
				SetPlaneFormation( PFT_INDUCTION );
			else
				SetPlaneFormation( PFT_AMPLIFIER );
		} break;

	case PFT_INDUCTION:
		SetPlaneFormation( PFT_AMPLIFIER );
		break;

	default:
		break;
	}
}

void CX2GUEve::EveElectraSystem::StartFormationEffect()
{
	m_fFormationSetElapsedTime = 0.f;

	DestroyFormationMarkAndMsgParticle();

	switch( m_ePlaneFormationType )
	{
	case PFT_AMPLIFIER:
		{
			m_hParticleFormationMark[0] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_red_01", m_pGUEve->GetBonePos( L"Bip01" ) );
			m_hParticleFormationMark[1] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_red_02", m_pGUEve->GetBonePos( L"Bip01" ) );
			m_hParticleFormationMark[2] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_red_03", m_pGUEve->GetBonePos( L"Bip01" ) );
		} break;
	case PFT_SPECTRUM:
		{
			m_hParticleFormationMark[0] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_Blue_01", m_pGUEve->GetBonePos( L"Bip01" ) );
			m_hParticleFormationMark[1] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_Blue_02", m_pGUEve->GetBonePos( L"Bip01" ) );
			m_hParticleFormationMark[2] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_Blue_03", m_pGUEve->GetBonePos( L"Bip01" ) );
		} break;
	case PFT_INDUCTION:
		{
			m_hParticleFormationMark[0] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_Green_01", m_pGUEve->GetBonePos( L"Bip01" ) );
			m_hParticleFormationMark[1] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_Green_02", m_pGUEve->GetBonePos( L"Bip01" ) );
			m_hParticleFormationMark[2] = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
				L"eve_spectro_MsgMark_Green_03", m_pGUEve->GetBonePos( L"Bip01" ) );
		} break;
	default:
		break;
	}
}

void CX2GUEve::EveElectraSystem::ProcessFormation( float _fElapsedTime )
{
	if ( GetEnableSystem() )
	{
		float fElapsedTimeBefore = m_fFormationSetElapsedTime;
		m_fFormationSetElapsedTime += _fElapsedTime;

		for( UINT i = 0; i < 3; ++i )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMark = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleFormationMark[i] );
			if( pSeqFormationMark != NULL )
			{
				pSeqFormationMark->SetPosition(  m_pGUEve->GetBonePos( L"Bip01" ) );
			}
		}

		for( UINT i = 0; i < 3; ++i )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hParticleFormationMsg[i] );
			if( pSeqFormationMsg != NULL )
			{
				pSeqFormationMsg->SetPosition(  m_pGUEve->GetBonePos( L"Bip01" ), true, true );
			}
		}


		if( fElapsedTimeBefore <= 0.0f && m_fFormationSetElapsedTime > 0.0f )
		{
			if( m_ePlaneFormationType == PFT_AMPLIFIER )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_red_01", m_pGUEve->GetBonePos( L"Bip01" ) );
				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[0] = pSeqFormationMsg->GetHandle();
				}
			}
			else if( m_ePlaneFormationType == PFT_SPECTRUM )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_Blue_01", m_pGUEve->GetBonePos( L"Bip01" ) );
				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[0] = pSeqFormationMsg->GetHandle();
				}
			}
			else if( m_ePlaneFormationType == PFT_INDUCTION )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence(
					(CKTDGObject*) m_pGUEve, L"eve_spectro_Msg_Green_01", m_pGUEve->GetBonePos( L"Bip01" ) );

				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[0] = pSeqFormationMsg->GetHandle();
				}
			}
		}
		else if( fElapsedTimeBefore <= 0.8f && m_fFormationSetElapsedTime > 0.8f )
		{
			if( m_ePlaneFormationType == PFT_AMPLIFIER )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_red_02", m_pGUEve->GetBonePos( L"Bip01" ) );
				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[1] = pSeqFormationMsg->GetHandle();
				}
			}
			else if( m_ePlaneFormationType == PFT_SPECTRUM )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_Blue_02", m_pGUEve->GetBonePos( L"Bip01" ) );
				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[1] = pSeqFormationMsg->GetHandle();
				}
			}
			else if( m_ePlaneFormationType == PFT_INDUCTION )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence(
					(CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_Green_02", m_pGUEve->GetBonePos( L"Bip01" ) );

				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[1] = pSeqFormationMsg->GetHandle();
				}
			}
		}
		else if( fElapsedTimeBefore <= 1.6f && m_fFormationSetElapsedTime > 1.6f )
		{
			if( m_ePlaneFormationType == PFT_AMPLIFIER )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_red_03", m_pGUEve->GetBonePos( L"Bip01" ) );
				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[2] = pSeqFormationMsg->GetHandle();
				}
			}
			else if( m_ePlaneFormationType == PFT_SPECTRUM )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_Blue_03", m_pGUEve->GetBonePos( L"Bip01" ) );
				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[2] = pSeqFormationMsg->GetHandle();
				}
			}
			else if( m_ePlaneFormationType == PFT_INDUCTION )
			{
				CKTDGParticleSystem::CParticleEventSequence* pSeqFormationMsg = g_pX2Game->GetMajorParticle()->CreateSequence(
					(CKTDGObject*) m_pGUEve,  L"eve_spectro_Msg_Green_03", m_pGUEve->GetBonePos( L"Bip01" ) );

				if( pSeqFormationMsg != NULL )
				{
					pSeqFormationMsg->SetAxisAngle( m_pGUEve->GetRotateDegree() );
					pSeqFormationMsg->SetAddRotate( m_pGUEve->GetRotateDegree() );
					m_hParticleFormationMsg[2] = pSeqFormationMsg->GetHandle();
				}
			}
		}
	}
}

void CX2GUEve::EveElectraSystem::ProcessSystem( float fElapsedTime )
{
	//#ifdef SERV_EVE_BATTLE_SERAPH
	//			if ( false == m_bExceptionSystem )
	//				m_bSystemEnable = false;
	//#else
	//			m_bSystemEnable			= false;
	//#endif
	//#ifdef SERV_EVE_BATTLE_SERAPH
	//				if ( true == GetEnableHyperOpticalResearch() )
	//					const_cast<CX2SkillTree::SkillTemplet*>(pSkillTemplet)->m_eActiveSkillUseCondtion = CX2SkillTree::ASUT_GROUND;
	//#endif
	//#ifdef SERV_EVE_BATTLE_SERAPH
	//			if( false == m_bExceptionSystem &&
	//				m_pGUEve->m_pThousandStar != NULL && m_pGUEve->m_pThousandStar->m_pEffect != NULL
	//				&& true == g_pX2Game->GetDamageEffect()->IsLiveInstance( m_pGUEve->m_pThousandStar->m_pEffect ) )
	//#else
	//			if( m_pGUEve->m_pThousandStar != NULL && m_pGUEve->m_pThousandStar->m_pEffect != NULL && true == g_pX2Game->GetDamageEffect()->IsLiveInstance( m_pGUEve->m_pThousandStar->m_pEffect ) )
	//#endif
	//			{
	//				g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleThousandStarFormation );
	//				m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,  L"eve_thousandsOfStars_model_P01", m_pGUEve->m_pThousandStar->m_pEffect->GetPos() );
	//			}

	/*
	#ifdef SERV_EVE_BATTLE_SERAPH
	if ( m_fExceptionSystemRemainTime > 0 )
	{
	m_fExceptionSystemRemainTime -= _fElapsedTime;

	if ( m_fExceptionSystemRemainTime < 0 )
	{
	m_fExceptionSystemRemainTime = 0.f;
	m_bExceptionSystem = false;

	if ( true == IsSamef( m_fSystemRemainTime ) )
	m_bSystemEnable = false;

	m_pGUEve->m_iAddBoostAttack = 0;

	if ( INVALID_EFFECTSET_HANDLE != m_pGUEve->m_hEffectEnergeticHeart  )
	{
	g_pX2Game->GetEffectSet()->StopEffectSet( m_pGUEve->m_hEffectEnergeticHeart );
	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Seraph_Energetic_Heart_End", m_pGUEve );
	}

	if ( false == m_pGUEve->m_FrameDataNow.stateParam.bAttackState )
	{
	SetPlaneFormation( m_ePrevSpectro );
	g_pX2Game->UpdateSkillSlotUI();
	}

	if( m_pGUEve->m_pThousandStar != NULL && m_pGUEve->m_pThousandStar->m_pEffect != NULL
	&& true == g_pX2Game->GetDamageEffect()->IsLiveInstance( m_pGUEve->m_pThousandStar->m_pEffect ) )
	{
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hParticleThousandStarFormation );
	m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) m_pGUEve,
	L"eve_thousandsOfStars_model_P01", m_pGUEve->m_pThousandStar->m_pEffect->GetPos() );
	}
	}
	}
	else
	{
	if ( PFT_FUSION == GetPlaneFormation() && false == m_pGUEve->m_FrameDataNow.stateParam.bAttackState )
	{
	SetPlaneFormation( m_ePrevSpectro );
	g_pX2Game->UpdateSkillSlotUI();
	}
	}
	#endif
	*/

	ProcessPlane( fElapsedTime );

	ProcessGigaPlane( fElapsedTime );

	ProcessFormation( fElapsedTime );

	if ( !m_EveElectraLaserDataList.empty() )
	{
		std::list< EveElectraLaserData >::iterator lit_Laser;
		for( lit_Laser = m_EveElectraLaserDataList.begin(); lit_Laser != m_EveElectraLaserDataList.end(); )
		{
			EveElectraLaserData& kEveElectraLaserData = *lit_Laser;

			if( kEveElectraLaserData.m_TraceBone != L"" )
				kEveElectraLaserData.m_vLaserStartPos = m_pGUEve->GetBonePos( kEveElectraLaserData.m_TraceBone.c_str() );

			kEveElectraLaserData.OnFrameMove( fElapsedTime );
			if( kEveElectraLaserData.m_bDie == true )
				lit_Laser = m_EveElectraLaserDataList.erase( lit_Laser );
			else
				++lit_Laser;
		}
	}

	if ( !m_EveElectraEffectDataList.empty() )
	{
		std::list< EveElectraEffectData >::iterator lit_Effect;
		for( lit_Effect = m_EveElectraEffectDataList.begin(); lit_Effect != m_EveElectraEffectDataList.end(); )
		{
			EveElectraEffectData& kEveElectraEffectData = *lit_Effect;
			kEveElectraEffectData.OnFrameMove( fElapsedTime );
			if( kEveElectraEffectData.m_bDie == true )
				lit_Effect = m_EveElectraEffectDataList.erase( lit_Effect );
			else
				++lit_Effect;
		}
	}

	if ( !m_EveElectraLockonDataList.empty() )
	{
		std::list< EveElectraLockonData >::iterator lit_Lockon;
		for( lit_Lockon = m_EveElectraLockonDataList.begin(); lit_Lockon != m_EveElectraLockonDataList.end(); )
		{
			EveElectraLockonData& kEveElectraLockonData = *lit_Lockon;
			kEveElectraLockonData.OnFrameMove( fElapsedTime );
			if( kEveElectraLockonData.m_bDie == true )
				lit_Lockon = m_EveElectraLockonDataList.erase( lit_Lockon );
			else
				++lit_Lockon;
		}
	}

}

void CX2GUEve::EveElectraSystem::AddLaserToSystem( CX2DamageEffect::CEffect* _pDamageEffect,
	wstring _TraceBone, D3DXVECTOR3& _vLaserStartPos, D3DXVECTOR3&_vDirVector,	float _fVelocity,
	float _fLaserPropagateTime /*= 0.3f*/, float _fLaserWidthScale /*= 1.f*/,
	wstring AmplifyDamageEffectName /*= L""*/, wstring SpectrumDamageEffectName /*= L""*/, wstring InductionDamageEffectName /*= L""*/,
	bool _bApplyLaserRangePassive /*= true*/,  float _fRadiusScale /*= 1.f*/ )
{
	if( _pDamageEffect == NULL )
		return;

	EveElectraLaserData kEveElectraLaserData( this );
    kEveElectraLaserData.m_vecDamageEffect.push_back(_pDamageEffect->GetHandle());
	kEveElectraLaserData.m_vLaserStartPos	= _vLaserStartPos;

	kEveElectraLaserData.m_vDirVector		= _vDirVector;
	kEveElectraLaserData.m_fVelocity		= _fVelocity;
	kEveElectraLaserData.m_fLaserPropagateTime	= _fLaserPropagateTime;
	kEveElectraLaserData.m_TraceBone		= _TraceBone;
	kEveElectraLaserData.m_fElapsedTime		= 0.f;
	kEveElectraLaserData.m_fPenetrateTime	= 0.0f;
	kEveElectraLaserData.m_fLaserWidthScale = _fLaserWidthScale;
	kEveElectraLaserData.m_bApplyLaserRangePassive = _bApplyLaserRangePassive;


    CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = _pDamageEffect->GetMainEffect();
	kEveElectraLaserData.m_vInitRotateLocalDegree = ( pMainEffect != NULL ) ? pMainEffect->GetRotateLocalDegree() : D3DXVECTOR3(0,0,0);


	kEveElectraLaserData.m_AmplifyDamageEffectName	= AmplifyDamageEffectName;
	kEveElectraLaserData.m_SpectrumDamageEffectName = SpectrumDamageEffectName;
	kEveElectraLaserData.m_InductionDamageEffectName = InductionDamageEffectName;
	kEveElectraLaserData.m_iFireCount = 0;

	if( AmplifyDamageEffectName == L"" )
		kEveElectraLaserData.m_AmplifyDamageEffectName = L"EVE_PROTO_PURITY_LASER";

	if( SpectrumDamageEffectName == L"" )
		kEveElectraLaserData.m_SpectrumDamageEffectName = L"EVE_PROTO_PURITY_LASER";

	if( InductionDamageEffectName == L"" )
		kEveElectraLaserData.m_InductionDamageEffectName = L"EVE_PROTO_PURITY_LASER";

	m_EveElectraLaserDataList.push_back( kEveElectraLaserData );

	_pDamageEffect->SetLaserGroupID( m_iLaserGroupNum );
	++m_iLaserGroupNum;
	if ( m_iLaserGroupNum > 2 )
		m_iLaserGroupNum = 1;

	LaserAttackLineSet( _pDamageEffect, _fRadiusScale );
	_pDamageEffect = NULL;
	return;
}

void CX2GUEve::EveElectraSystem::LaserAttackLineSet( CX2DamageEffect::CEffect* _pDamageEffect, float _fRadiusScale /*= 1.0f*/ )
{
	if( _pDamageEffect == NULL )
		return;
    CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = _pDamageEffect->GetMainEffect();
	if ( pMainEffect == NULL || pMainEffect->GetXSkinAnim() == NULL )
		return;
	CKTDXCollision::CollisionDataList* pCollisionDataList = &pMainEffect->GetXSkinAnim()->GetAttackDataListNonConst();
	if ( pCollisionDataList == NULL )
		return;

	for( CKTDXCollision::CollisionData* pColA : *pCollisionDataList )
	{
		ASSERT( pColA != NULL );
		if( pColA->m_bEnable == false )
			continue;

		if( pColA->m_CollisionType == CKTDXCollision::CT_LINE )
		{
			pColA->m_bUseRadiusScaleY = true;
			pColA->m_fRadiusScale = _fRadiusScale;
		}
	}
	return;
}

void CX2GUEve::EveElectraSystem::DeleteLaserFromSystem()
{

	std::list< EveElectraLaserData >::iterator lit_Laser;
	for( lit_Laser = m_EveElectraLaserDataList.begin(); lit_Laser != m_EveElectraLaserDataList.end(); )
	{
		EveElectraLaserData& kEveElectraLaserData = *lit_Laser;
		kEveElectraLaserData.Delete();
		lit_Laser = m_EveElectraLaserDataList.erase( lit_Laser );

	}
	return;
}


void CX2GUEve::EveElectraSystem::AddEffectToSystem( CX2DamageEffect::CEffect* _pDamageEffect, wstring AmplifyDamageEffectName /*= L""*/,
	wstring SpectrumDamageEffectName /*= L""*/, wstring InductionDamageEffectName /*= L""*/,
	wstring FusionDamageEffectName /*= L""*/ )
{
	if( GetEnablePlane() == false )
		return;

	if( _pDamageEffect == NULL )
		return;

	EveElectraEffectData kEveElectraEffectData( this );
    kEveElectraEffectData.m_hDamageEffect =  _pDamageEffect->GetHandle();

	if( AmplifyDamageEffectName == L"" )
		kEveElectraEffectData.m_AmplifyDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraEffectData.m_AmplifyDamageEffectName = AmplifyDamageEffectName;

	if( SpectrumDamageEffectName == L"" )
		kEveElectraEffectData.m_SpectrumDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraEffectData.m_SpectrumDamageEffectName = SpectrumDamageEffectName;

	if( InductionDamageEffectName == L"" )
		kEveElectraEffectData.m_InductionDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraEffectData.m_InductionDamageEffectName = InductionDamageEffectName;

	if( FusionDamageEffectName == L"" )
		kEveElectraEffectData.m_FusionDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraEffectData.m_FusionDamageEffectName = FusionDamageEffectName;

	m_EveElectraEffectDataList.push_back( kEveElectraEffectData );

	_pDamageEffect = NULL;
	return;
}

void CX2GUEve::EveElectraSystem::AddLockonToSystem( CX2DamageEffect::CEffect* _pDamageEffect, wstring AmplifyDamageEffectName /*= L""*/,
	wstring SpectrumDamageEffectName /*= L""*/, wstring InductionDamageEffectName /*= L""*/ )
{
	if( GetEnablePlane() == false )
		return;

	if( _pDamageEffect == NULL )
		return;

	EveElectraLockonData kEveElectraLockonData( this );
    kEveElectraLockonData.m_hDamageEffect =  _pDamageEffect->GetHandle();

	if( AmplifyDamageEffectName == L"" )
		kEveElectraLockonData.m_AmplifyDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraLockonData.m_AmplifyDamageEffectName = AmplifyDamageEffectName;

	if( SpectrumDamageEffectName == L"" )
		kEveElectraLockonData.m_SpectrumDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraLockonData.m_SpectrumDamageEffectName = SpectrumDamageEffectName;

	if( InductionDamageEffectName == L"" )
		kEveElectraLockonData.m_InductionDamageEffectName = _pDamageEffect->GetName();
	else
		kEveElectraLockonData.m_InductionDamageEffectName = SpectrumDamageEffectName;

	m_EveElectraLockonDataList.push_back( kEveElectraLockonData );

	_pDamageEffect = NULL;
	return;
}

void CX2GUEve::ShowActiveSkillCutInAndLight( const WCHAR* szBoneName_, const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ /*= false */ )
{
    if( m_pXSkinAnim->EventTimerOneshot( fTimeToShow_ ) )
	{
		if ( GetShowCutInAndChangeWorldColor() && GetShowActiveSkillShow() )
			ActiveSkillShow( szBoneName_, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].fileName.c_str(), s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].vSize, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].vPosition, IsHyperState(), bOnlyLight_ );
	}
}

void CX2GUEve::EveElectraSystem::CreatePlane( float _fZAngle/* = 0.f*/, float _LifeTime/* = 2.f*/, float _fDistance/* = 300.f*/, float _fHeight/* = 130.f*/, wstring /*PlaneMeshName*/ /*= L""*/ )
{
	if( GetEnableSystem() == false )
		return;

	_fDistance *= m_pGUEve->GetScaleByUnit().x;
	_fHeight *= m_pGUEve->GetScaleByUnit().y;


	D3DXVECTOR3 vPlanePos = m_pGUEve->GetPos();
	vPlanePos.y += _fHeight;
	D3DXVECTOR3 vZeroYDirVector = m_pGUEve->GetDirVector();
	vZeroYDirVector.y = 0.f;
	D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );

	if(m_pGUEve->GetIsRight() == false)
	{
		vZeroYDirVector.x = -vZeroYDirVector.x;
		vZeroYDirVector.z = -vZeroYDirVector.z;
	}

	vPlanePos += (_fDistance * vZeroYDirVector);
	m_vPlanePoint	= vPlanePos;

	///{{

	D3DXVECTOR3 vRotationAxis;
	D3DXVec3Cross( &vRotationAxis, &vZeroYDirVector, &Y_AXIS );
	D3DXVec3Normalize( &vRotationAxis, &vRotationAxis );


	D3DXMATRIX matRotationCenter;
	D3DXMatrixRotationAxis( &matRotationCenter, &vRotationAxis, _fZAngle );
	D3DXVECTOR4 v4PlaneNormalCenter;
	D3DXVec3Transform( &v4PlaneNormalCenter, &vZeroYDirVector, &matRotationCenter );
	D3DXVECTOR3 vPlaneNormalCenter( v4PlaneNormalCenter.x, v4PlaneNormalCenter.y, v4PlaneNormalCenter.z );
	m_vPlaneNormalCenter = vPlaneNormalCenter;

	D3DXMATRIX matRotationUpper;
	D3DXMatrixRotationAxis( &matRotationUpper, &vRotationAxis, D3DXToRadian( _fZAngle ) );
	D3DXVECTOR4 v4PlaneNormalUpper;
	D3DXVec3Transform( &v4PlaneNormalUpper, &vZeroYDirVector, &matRotationUpper );
	D3DXVECTOR3 vPlaneNormalUpper( v4PlaneNormalUpper.x, v4PlaneNormalUpper.y, v4PlaneNormalUpper.z );
	m_vPlaneNormalUpper = vPlaneNormalUpper;

	D3DXMATRIX matRotationLower;
	D3DXMatrixRotationAxis( &matRotationLower, &vRotationAxis,  D3DXToRadian( _fZAngle ) );
	D3DXVECTOR4 v4PlaneNormalLower;
	D3DXVec3Transform( &v4PlaneNormalLower, &vZeroYDirVector, &matRotationLower );
	D3DXVECTOR3 vPlaneNormalLower( v4PlaneNormalLower.x, v4PlaneNormalLower.y, v4PlaneNormalLower.z );
	m_vPlaneNormalLower = vPlaneNormalLower;


	D3DXPlaneFromPointNormal( &m_PlaneCenter, &m_vPlanePoint, &m_vPlaneNormalCenter );
	D3DXPlaneFromPointNormal( &m_PlaneUpper, &m_vPlanePoint, &m_vPlaneNormalUpper );
	D3DXPlaneFromPointNormal( &m_PlaneLower, &m_vPlanePoint, &m_vPlaneNormalLower );

	m_fPlaneZAngle	= _fZAngle;
	if( m_fPlaneZAngle > 0 )
	{
		m_vPlaneNormal	= m_vPlaneNormalUpper;
		m_PrimaryPlane  = m_PlaneUpper;
	}
	else if( m_fPlaneZAngle < 0 )
	{
		m_vPlaneNormal	= m_vPlaneNormalLower;
		m_PrimaryPlane  = m_PlaneLower;
	}
	else
	{
		m_vPlaneNormal	= m_vPlaneNormalCenter;
		m_PrimaryPlane  = m_PlaneCenter;
	}


	D3DXVECTOR3 vPlaneRotateDegree = m_pGUEve->GetRotateDegree();

	///{{
	m_vPlaneRotateDegreeCenter			= vPlaneRotateDegree;
	m_vPlaneRotateDegreeUpper			= vPlaneRotateDegree;
	m_vPlaneRotateDegreeUpper.z			+= 24.f;
	m_vPlaneRotateDegreeLower			= vPlaneRotateDegree;
	m_vPlaneRotateDegreeLower.z			-= 24.f;
	///}}

	vPlaneRotateDegree.z += _fZAngle;

	DestroyPlaneAndCrystalEffect();

	CKTDGXMeshPlayer* pMajorMeshPlayer = g_pX2Game->GetMajorXMeshPlayer();
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityPlane", vPlanePos, vPlaneRotateDegree, vPlaneRotateDegree );
    if ( pMeshInstPlane != NULL )
    {
        pMeshInstPlane->SetPerFrameSimulation( true );
	    m_hEffectPlane = pMeshInstPlane->GetHandle();
	    pMeshInstPlane->SetMaxLifeTime( _LifeTime );
    }
	SetEnablePlane( true );

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal1 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal1 != NULL )
    {
        pMeshInstCrystal1->SetPerFrameSimulation( true );
	    m_hEffectCrystal1 = pMeshInstCrystal1->GetHandle();
	    pMeshInstCrystal1->SetPos( pMeshInstPlane->GetBonePos( L"Dummy02" ));
    }

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal2 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal2 != NULL )
    {
        pMeshInstCrystal2->SetPerFrameSimulation( true );
	    m_hEffectCrystal2 = pMeshInstCrystal2->GetHandle();
	    pMeshInstCrystal2->SetPos( pMeshInstPlane->GetBonePos( L"Dummy03" ));
    }


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal3 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal3 != NULL )
    {
        pMeshInstCrystal3->SetPerFrameSimulation( true );
	    m_hEffectCrystal3 = pMeshInstCrystal3->GetHandle();
	    pMeshInstCrystal3->SetPos( pMeshInstPlane->GetBonePos( L"Dummy04" ));
    }


	// Plane Start ����Ʈ ����
	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();
	if ( pMajorParticleSystem != NULL && pMeshInstPlane != NULL )
	{
		pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy02" ) );
		pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy02" ) );
		pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy03" ) );
		pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy03" ) );
		pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy04" ) );
		pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy04" ) );
	}

	return;
}


// �ñر� ��� ��, ���� ũ�⸦ �ø� �� �ֵ��� _fScale �������� �߰�, ������
void CX2GUEve::EveElectraSystem::CreateGigaPlane( float _fZAngle/* = 0.f*/, float _LifeTime/* = 2.f*/, float _fDistance/* = 300.f*/, float _fHeight/* = 130.f*/, float _fScale /* = 1.0f */ )
{

	_fDistance *= m_pGUEve->GetScaleByUnit().x;
	_fHeight *= m_pGUEve->GetScaleByUnit().y;

	D3DXVECTOR3 vPlanePos = m_pGUEve->GetPos();
	vPlanePos.y += _fHeight;
	D3DXVECTOR3 vZeroYDirVector = m_pGUEve->GetDirVector();
	vZeroYDirVector.y = 0.f;
	D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );

	if(m_pGUEve->GetIsRight() == false)
	{
		vZeroYDirVector.x = -vZeroYDirVector.x;
		vZeroYDirVector.z = -vZeroYDirVector.z;
	}

	D3DXVECTOR3 vRotateDegree = D3DXVECTOR3( 0.f, 0.f, 0.f );
	vRotateDegree.z = _fZAngle;

	D3DXVECTOR3 vRotationAxis;
	D3DXVec3Cross( &vRotationAxis, &vZeroYDirVector, &Y_AXIS );
	D3DXVec3Normalize( &vRotationAxis, &vRotationAxis );
	D3DXMATRIX matRotation;
	D3DXMatrixRotationAxis( &matRotation, &vRotationAxis, D3DX_PI / 180.f * vRotateDegree.z );
	D3DXVECTOR4 v4PlaneNormal;
	D3DXVec3Transform( &v4PlaneNormal, &vZeroYDirVector, &matRotation );


	D3DXVECTOR3 vPlaneNormal( v4PlaneNormal.x, v4PlaneNormal.y, v4PlaneNormal.z );

	vPlanePos += (_fDistance * vZeroYDirVector);

	m_vGigaPlanePoint	= vPlanePos;
	m_vGigaPlaneNormal	= vPlaneNormal;
	m_fGigaPlaneZAngle	= _fZAngle;

	D3DXPlaneFromPointNormal( &m_GigaStreamPlane, &m_vGigaPlanePoint, &m_vGigaPlaneNormal );

	D3DXVECTOR3 vPlaneRotateDegree = m_pGUEve->GetRotateDegree();
	vPlaneRotateDegree.z += _fZAngle;

	DestroyGigaPlaneAndCrystalEffect();

	CKTDGXMeshPlayer* pMajorMeshPlayer = g_pX2Game->GetMajorXMeshPlayer();
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityGigaPlane", vPlanePos, vPlaneRotateDegree, vPlaneRotateDegree );

	if ( NULL != pMeshInstPlane )
    {
        pMeshInstPlane->SetPerFrameSimulation( true );
		pMeshInstPlane->SetScale ( D3DXVECTOR3 ( 1.f, _fScale, _fScale ) );
	    // X ���� �����ϰ� Scale �� Ű���..
    	m_hEffectGigaPlane = pMeshInstPlane->GetHandle();
	    pMeshInstPlane->SetMaxLifeTime( _LifeTime );
    }
	SetEnableGigaPlane( true );

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal1 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityGigaCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal1 != NULL )
    {
	m_hEffectGigaCrystal[0] = pMeshInstCrystal1->GetHandle();
	pMeshInstCrystal1->SetPos( pMeshInstPlane->GetBonePos( L"Dummy02" ));
    }

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal2 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityGigaCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal2 != NULL )
    {
	m_hEffectGigaCrystal[1] = pMeshInstCrystal2->GetHandle();
	pMeshInstCrystal2->SetPos( pMeshInstPlane->GetBonePos( L"Dummy03" ));
    }

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal3 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityGigaCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal3 != NULL )
    {
	m_hEffectGigaCrystal[2] = pMeshInstCrystal3->GetHandle();
	pMeshInstCrystal3->SetPos( pMeshInstPlane->GetBonePos( L"Dummy04" ));
    }

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal4 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityGigaCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal4 != NULL )
    {
	m_hEffectGigaCrystal[3] = pMeshInstCrystal4->GetHandle();
	pMeshInstCrystal4->SetPos( pMeshInstPlane->GetBonePos( L"Dummy05" ));
    }

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstCrystal5 = pMajorMeshPlayer->CreateInstance( (CKTDGObject*) m_pGUEve,  L"EveProtoPurityGigaCrystal", vPlanePos, m_pGUEve->GetRotateDegree(), m_pGUEve->GetRotateDegree() );
    if ( pMeshInstCrystal5 != NULL )
    {
	m_hEffectGigaCrystal[4] = pMeshInstCrystal5->GetHandle();
	pMeshInstCrystal5->SetPos( pMeshInstPlane->GetBonePos( L"Dummy06" ));
    }

	// Plane Start ����Ʈ ����
	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy02" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy02" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy03" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy03" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy04" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy04" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy05" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy05" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P02", pMeshInstPlane->GetBonePos( L"Dummy06" ) );
	pMajorParticleSystem->CreateSequence( (CKTDGObject*) m_pGUEve,  L"eve_spectro_model_P03", pMeshInstPlane->GetBonePos( L"Dummy06" ) );

	return;
}

void CX2GUEve::EveElectraSystem::DestroyFormationMarkAndMsgParticle()
{
	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleFormationMark[0] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleFormationMark[1] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleFormationMark[2] );

	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleFormationMsg[0] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleFormationMsg[1] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleFormationMsg[2] );
}

void CX2GUEve::EveElectraSystem::DestroyPlaneAndCrystalEffect()
{
	CKTDGXMeshPlayer* pMajorMeshPlayer = g_pX2Game->GetMajorXMeshPlayer();
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectCrystal1 );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectCrystal2 );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectCrystal3 );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectPlane );

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleCrystal[0] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleCrystal[1] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleCrystal[2] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticlePlane );


}

void CX2GUEve::EveElectraSystem::DestroyGigaPlaneAndCrystalEffect()
{
	CKTDGXMeshPlayer* pMajorMeshPlayer = g_pX2Game->GetMajorXMeshPlayer();
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectGigaCrystal[0] );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectGigaCrystal[1] );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectGigaCrystal[2] );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectGigaCrystal[3] );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectGigaCrystal[4] );
	pMajorMeshPlayer->DestroyInstanceHandle( m_hEffectGigaPlane );

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleGigaCrystal[0] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleGigaCrystal[1] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleGigaCrystal[2] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleGigaCrystal[3] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleGigaCrystal[4] );
	pMajorParticleSystem->DestroyInstanceHandle( m_hParticleGigaPlane );
}

BUFF_TEMPLET_ID CX2GUEve::EveElectraSystem::GetBuffTempletIdByPlaneFormation( const PlaneFormationType ePlaneFormationType_ ) const
{
	switch ( ePlaneFormationType_ )
	{
	case PFT_AMPLIFIER:
		return BTI_BUFF_AMPLIFICATION_PLACE;
		break;

	case PFT_SPECTRUM:
		return BTI_BUFF_SPECTRUM_PLACE;
		break;

	case PFT_INDUCTION:
		return BTI_BUFF_INDUCTION_PLACE;
		break;

	case PFT_FUSION:
		return BTI_BUFF_SI_SA_EBS_ENERGETIC_HEART;
		break;

	default:
		return BTI_NONE;
		break;
	}
}




CX2GUEve::FlyingImpactData::FlyingImpactData()
{
	m_vOffsetRotate = D3DXVECTOR3(0, 0, 0);
	m_vOffsetPos = D3DXVECTOR3( 0, 0, 0 );
    m_hEffect = INVALID_DAMAGE_EFFECT_HANDLE;
}

CX2GUEve::FlyingImpactData::~FlyingImpactData()
{
    if ( m_hEffect != INVALID_DAMAGE_EFFECT_HANDLE )
        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffect );
}

//{{ robobeg : 2013-08-30
CX2DamageEffect::CEffect*   CX2GUEve::FlyingImpactData::GetLiveDamageEffect()
{
    return  g_pX2Game->GetDamageEffect()->GetInstance( m_hEffect );
}
//}} robobeg : 2013-08-30


void CX2GUEve::ClearThousandStar()
{
    if ( CX2DamageEffect::CEffect* pTSEffect =  ( m_pThousandStar != NULL ) ? m_pThousandStar->GetLiveDamageEffect() : NULL )
	{
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pTSEffect->GetMainEffect() )
        {
		    g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"EveThousandStarDie", pMeshInstance->GetPos(), GetRotateDegree(), GetRotateDegree() );
        }
	}


	if( m_pThousandStar != NULL )
		SAFE_DELETE( m_pThousandStar );

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleThousandStarFormation );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hCountDownThousandStar );
	m_fElapsedTimeThousandStartCreated	 = 0.f;
	m_bLockOnNpc = false;
	m_iLockOnUid = -1;

	m_bEntireFireMode	= false;
	m_fDelayAttackTimeEntireFire	= 0.f;
	m_iThousandStarCount			= 0;

	m_pThousandStar = NULL;


}

void CX2GUEve::CreateThousandStar()
{
	m_fThousandStarPowerRate = 1.f;
	m_fElapsedTimeThousandStartCreated = 0.f;
	ClearThousandStar();
	if( m_pThousandStar == NULL )
	{
		m_pThousandStar = new FlyingImpactData;
		m_pThousandStar->m_vOffsetPos = D3DXVECTOR3( -180*0.9f, 52*3.f, 0.f );
	}
    if ( m_pThousandStar == NULL )
        return;

    if ( m_pThousandStar->GetLiveDamageEffect() == NULL )
	{
		D3DXVECTOR3 vRotDegree = GetRotateDegree();
		D3DXVECTOR3 vDirVec = GetDirVector();
		D3DXVECTOR3 vZVec = GetZVector();
		vDirVec.y = 0.f;

		D3DXVECTOR3 vPos = GetPos();
		if( false == GetIsRight() )
		{
			vRotDegree.y += 180.f;
		}

		CX2DamageEffect::CEffect *pEffectThounsandStar = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"EVE_THOUSAND_STAR", GetPowerRate(), vPos,
			vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y );

		if( pEffectThounsandStar == NULL )
		{
			ClearThousandStar();
			return;
		}

        m_pThousandStar->m_hEffect = pEffectThounsandStar->GetHandle();

        CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectThounsandStar->GetMainEffect();
        if ( pMeshInstance != NULL  )
        {
		    pMeshInstance->SetMoveAxisAngleDegree( GetRotateDegree() );
		    pMeshInstance->SetRotateDegree( GetRotateDegree() + m_pThousandStar->m_vOffsetRotate );
        }

		D3DXVECTOR3 vTargetPos = GetPos();
		if( true == GetIsRight() )
		{
			vTargetPos += m_pThousandStar->m_vOffsetPos.x * vDirVec;
			vTargetPos.y += m_pThousandStar->m_vOffsetPos.y;
			vTargetPos += m_pThousandStar->m_vOffsetPos.z * vZVec;
		}
		else
		{
			vTargetPos -= m_pThousandStar->m_vOffsetPos.x * vDirVec;
			vTargetPos.y += m_pThousandStar->m_vOffsetPos.y;
			vTargetPos += m_pThousandStar->m_vOffsetPos.z * vZVec;
		}

        if ( pMeshInstance != NULL )
		    pMeshInstance->SetPos( vTargetPos );

		m_iThousandStarCount = 100;
	}
}

void CX2GUEve::DoFrameMoveThousandStar()
{
	if( m_pThousandStar != NULL
        && m_pThousandStar->m_hEffect != INVALID_DAMAGE_EFFECT_HANDLE
        )
	{
		m_fElapsedTimeThousandStartCreated += m_fElapsedTime;
	}
	if( m_bEntireFireMode == true )
	{
		m_fDelayAttackTimeEntireFire += m_fElapsedTime;
		if( m_fDelayAttackTimeEntireFire >= 0.033333f )
		{
			PlaySound( L"Raven_NasodCore_Attack.ogg" );
			for(UINT i=0; i<100; i++)
			{
				EntireAttackThousandStar();
				if( m_iThousandStarCount <= 0)
					break;
			}
			m_bEntireFireMode	= false;		//�ӽ÷� ���� ���� ��� ���� ����

			m_fDelayAttackTimeEntireFire			= 0.f;
		}
	}
	else if( m_fElapsedTimeThousandStartCreated >= 55.f && m_fElapsedTimeThousandStartCreated - m_fElapsedTime < 55.f
		&& m_pThousandStar != NULL
        && m_pThousandStar->m_hEffect != INVALID_DAMAGE_EFFECT_HANDLE
        )
	{
        CX2DamageEffect::CEffect*   pTSEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_pThousandStar->m_hEffect );
        if ( pTSEffect != NULL )
			m_hCountDownThousandStar = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"CountDown5", pTSEffect->GetPos() );
	}
	else if( m_fElapsedTimeThousandStartCreated >= 60.f )
	{
		UpNowMp( m_iThousandStarCount * 1.25f );

		if( NULL != g_pData->GetPicCharBlue() )
		{
			WCHAR wszText[64] = L"";
			StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MP+ %d", (int) (m_iThousandStarCount * 1.25f) );

			D3DXVECTOR3 pos = GetPos();
			pos.y += 50.f;

			g_pData->GetPicCharBlue()->DrawText( wszText, pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}


		m_iThousandStarCount = 0;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS );

		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			CX2UserSkillTree& userSkillTree = GetUnit()->AccessUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			float fRemainCoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - m_fElapsedTimeThousandStartCreated;
			if( fRemainCoolTime < 0.f )
				fRemainCoolTime = 0.f;

			userSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS, fRemainCoolTime  );
			if ( IsMyUnit() )
				g_pX2Game->UpdateSkillSlotUI();
		}
		ClearThousandStar();
	}
	else if( m_bDelayAttackThousandStar == true )
	{
		m_fDelayAttackTime += m_fElapsedTime;
		if( m_fDelayAttackTime >= 0.3f )
		{
			AttackThousandStar();
			m_bDelayAttackThousandStar	= false;
			m_fDelayAttackTime			= 0.f;
		}
	}

	D3DXVECTOR3 vDirVec = GetDirVector();


	if( m_pThousandStar != NULL
        && m_pThousandStar->m_hEffect != INVALID_DAMAGE_EFFECT_HANDLE
        )
	{
        if ( CX2DamageEffect::CEffect* pSTEffect = m_pThousandStar->GetLiveDamageEffect() )
		{
			D3DXVECTOR3 vZVec = GetZVector();
			D3DXVec3Normalize( &vDirVec, &vDirVec );

			CKTDGXMeshPlayer::CXMeshInstance* pThousandStarMeshInstance = pSTEffect->GetMainEffect();
			if( NULL != pThousandStarMeshInstance && m_bEntireFireMode == false )
			{
				D3DXVECTOR3 vTargetPos = GetPos();
				if( true == GetIsRight() )
				{
					vTargetPos += m_pThousandStar->m_vOffsetPos.x * vDirVec;
					vTargetPos.y += m_pThousandStar->m_vOffsetPos.y;
					vTargetPos += m_pThousandStar->m_vOffsetPos.z * vZVec;
				}
				else
				{
					vTargetPos -= m_pThousandStar->m_vOffsetPos.x * vDirVec;
					vTargetPos.y += m_pThousandStar->m_vOffsetPos.y;
					vTargetPos += m_pThousandStar->m_vOffsetPos.z * vZVec;
				}

				float coeff = 0.05f;
				D3DXVECTOR3 vRotateDegree = GetRotateDegree() + m_pThousandStar->m_vOffsetRotate;
				pThousandStarMeshInstance->SetRotateDegree( vRotateDegree );
				pThousandStarMeshInstance->SetMoveAxisAngleDegree( GetRotateDegree() );

				vTargetPos = vTargetPos * coeff + pThousandStarMeshInstance->GetPos() * ( 1.f-coeff );
				pThousandStarMeshInstance->SetPos( vTargetPos );

				CKTDGParticleSystem::CParticleEventSequence* pSeqThousandStarFormation = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_EveElectraSystem.m_hParticleThousandStarFormation );
				if( pSeqThousandStarFormation != NULL )
				{
					pSeqThousandStarFormation->SetPosition( pThousandStarMeshInstance->GetPos() );
				}
				CKTDGParticleSystem::CParticleEventSequence* pSeqCountDownThousandStar = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hCountDownThousandStar );
				if( pSeqCountDownThousandStar != NULL )
				{
					D3DXVECTOR3 vCountDownPos = pThousandStarMeshInstance->GetPos();
					vCountDownPos.y += 80.f;
					pSeqCountDownThousandStar->SetPosition( vCountDownPos );
				}
			}

			if( m_iLockOnUid != -1 )
			{
				CX2GameUnit* pGameUnit = NULL;
				if( m_bLockOnNpc )
					pGameUnit = g_pX2Game->GetNPCUnitByUID( (int)m_iLockOnUid );
				else
					pGameUnit = g_pX2Game->GetUserUnitByUID( m_iLockOnUid );

				D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );

				if( pGameUnit != NULL )
				{
					const CKTDXCollision::CollisionDataListSet& AttackListSet = pGameUnit->GetCollisionListSet();
					vPos = pGameUnit->GetPos();
					vPos.y += 100.f;

					for( const CKTDXCollision::CollisionDataList* collisionDataList : AttackListSet )
					{
						for( CKTDXCollision::CollisionData* collisionData : *collisionDataList )
						{
							D3DXVECTOR3 vCollPos = collisionData->GetPointStart();
							float fDist = GetDistance( GetPos(), vCollPos );
							float fOldDist = GetDistance( GetPos(), vPos );

							if ( fDist <= fOldDist )
								vPos = vCollPos;
						}
					}
				}

				if( pGameUnit == NULL ||
					( pGameUnit != NULL && pGameUnit->GetNowHp() <= 0.f) ||
					( pGameUnit != NULL && GetDistance( GetPos(), vPos/*pGameUnit->GetPos()*/ ) > 1000.f ) )
				{
					m_iLockOnUid = -1;
				}

			}

		}
		else
		{
			ClearThousandStar();
		}
	}

}

void CX2GUEve::EntireAttackThousandStar()
{
	if( CX2DamageEffect::CEffect* pTSEffect = ( m_pThousandStar != NULL ) ? m_pThousandStar->GetLiveDamageEffect() : NULL )
	{
		D3DXVECTOR3 vRotateDegree;
		vRotateDegree = GetRotateDegree();

		float fVarRotateZ = GetRandomFloat( m_iThousandStarCount ) * 270.f;
		fVarRotateZ += 45.f;
		float fVarRotateX = GetRandomFloat( m_iThousandStarCount + m_iThousandStarCount ) * 360.f;

		vRotateDegree.z += fVarRotateZ;

		vRotateDegree.x += fVarRotateX;
		wstring wstrDamageEffectName = L"EVE_THOUSANDSTAR_MAGICBALL_ENTIRE";

		CX2DamageEffect::CEffect* pEffect = NULL;
        CKTDGXMeshPlayer::CXMeshInstance* pThousandStarMeshInstance = pTSEffect->GetMainEffect();
		if( m_EveElectraSystem.GetEnableSystem() == true &&
			( m_EveElectraSystem.GetPlaneFormation() == PFT_AMPLIFIER
			|| m_EveElectraSystem.GetPlaneFormation() == PFT_SPECTRUM
			|| m_EveElectraSystem.GetPlaneFormation() == PFT_INDUCTION
			|| m_EveElectraSystem.GetPlaneFormation() == PFT_FUSION
			)
			)
		{
            if ( pThousandStarMeshInstance != NULL )
            {
			    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate, pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

			    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
			    if( m_bLockOnNpc == true )
				    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
			    else
				    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;

			    pEffect->GetDamageData().damage.fPhysic *= 1.2f;
			    pEffect->GetDamageData().damage.fMagic *= 1.2f;

			    D3DXVECTOR3 vAmplifyScale = pEffect->GetMainEffectScaleByUnit();

                if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                {
			        D3DXVECTOR3 vScale = pMeshInstance->GetScale();

			        vAmplifyScale.x *= vScale.x;
			        vAmplifyScale.y *= vScale.y;
			        vAmplifyScale.z *= vScale.z;
			        vAmplifyScale *= 1.3f;

			        pEffect->SetScale( vAmplifyScale );
                }
            }

			// 			if( NULL != pEffect )
			// 				m_EveElectraSystem.AddLockonToSystem( pEffect, L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED", L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED"
		}
		else
		{
            if ( pThousandStarMeshInstance != NULL )
            {
			    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate, pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

			    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
			    if( m_bLockOnNpc == true )
				    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
			    else
				    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;
                }
			// 			if( NULL != pEffect )
			// 				m_EveElectraSystem.AddLockonToSystem( pEffect, L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED", L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED"
		}

        if ( pThousandStarMeshInstance != NULL )
		    g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( this ),  L"eve_thousandsOfStars_model_P02", pThousandStarMeshInstance->GetPos() );




		m_iThousandStarCount--;
		if( m_iThousandStarCount <= 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS );
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				CX2UserSkillTree& cUserSkillTree = GetUnit()->AccessUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				float fRemainCoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - m_fElapsedTimeThousandStartCreated;

				if( fRemainCoolTime < 5.f )
					fRemainCoolTime = 5.f;

				cUserSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS, fRemainCoolTime  );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			ClearThousandStar();
		}

	}
}


void CX2GUEve::AttackThousandStar()
{
	if( CX2DamageEffect::CEffect* pTSEffect = ( m_pThousandStar != NULL ) ? m_pThousandStar->GetLiveDamageEffect() : NULL )
	{

		D3DXVECTOR3 vRotateDegree;
		vRotateDegree = GetRotateDegree();

		float fVarRotateX = GetRandomFloat( m_iThousandStarCount ) * 360.f;
		vRotateDegree.x += fVarRotateX;

		wstring wstrDamageEffectName = L"";

		if( m_bEntireFireMode == true )
		{
			wstrDamageEffectName = L"EVE_THOUSANDSTAR_MAGICBALL_ENTIRE";
		}
		else
		{
			wstrDamageEffectName = L"EVE_THOUSANDSTAR_MAGICBALL";
		}


		CX2DamageEffect::CEffect* pEffect = NULL;
         CKTDGXMeshPlayer::CXMeshInstance* pThousandStarMeshInstance = pTSEffect->GetMainEffect();
		if( m_EveElectraSystem.GetEnableSystem() == true && m_EveElectraSystem.GetPlaneFormation() == PFT_AMPLIFIER )
		{
            if ( pThousandStarMeshInstance != NULL )
            {
			    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate, pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

			    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
			    if( m_bLockOnNpc == true )
				    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
			    else
				    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;

			    pEffect->GetDamageData().damage.fPhysic *= 1.2f;
			    pEffect->GetDamageData().damage.fMagic *= 1.2f;

			    D3DXVECTOR3 vAmplifyScale = pEffect->GetMainEffectScaleByUnit();

                if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                {
			        D3DXVECTOR3 vScale = pMeshInstance->GetScale();

			        vAmplifyScale.x *= vScale.x;
			        vAmplifyScale.y *= vScale.y;
			        vAmplifyScale.z *= vScale.z;
			        vAmplifyScale *= 1.3f;

			        pEffect->SetScale( vAmplifyScale );
                }
            }

			// 			if( NULL != pEffect )
			// 				m_EveElectraSystem.AddLockonToSystem( pEffect, L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED", L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED"
		}
		else if( m_EveElectraSystem.GetEnableSystem() == true && m_EveElectraSystem.GetPlaneFormation() == PFT_SPECTRUM )
		{
			for(UINT i=0; i<3; i++)
			{
				if( i == 1 )
				{
					vRotateDegree.x += 120.f;
				}
				else if( i == 2)
				{
					vRotateDegree.x += 120.f;
				}

                if ( pThousandStarMeshInstance != NULL )
                {
				    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate, pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

				    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
				    if( m_bLockOnNpc == true )
					    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
				    else
					    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;


				    pEffect->GetDamageData().damage.fPhysic *= 0.5f;
				    pEffect->GetDamageData().damage.fMagic *= 0.5f;

				    D3DXVECTOR3 vSpectrumScale = pEffect->GetMainEffectScaleByUnit();
                    if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                    {
				        D3DXVECTOR3 vScale = pMeshInstance->GetScale();

				        vSpectrumScale.x *= vScale.x;
				        vSpectrumScale.y *= vScale.y;
				        vSpectrumScale.z *= vScale.z;
				        vSpectrumScale *= 0.8f;

				        pEffect->SetScale( vSpectrumScale );
                    }
                }


				// 				if( NULL != pEffect )
				// 					m_EveElectraSystem.AddLockonToSystem( pEffect, L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED", L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED"
			}
		}
		else if ( m_EveElectraSystem.GetEnableSystem() == true && m_EveElectraSystem.GetPlaneFormation() == PFT_INDUCTION )
		{
            if ( pThousandStarMeshInstance != NULL )
            {
			    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate,
				    pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

			    CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();

			    pEffect->SetDamageTime( pEffect->GetDamageTime() + 1 );
			    pEffect->GetDamageData().damage.fPhysic *= 0.55f;
			    pEffect->GetDamageData().damage.fMagic *= 0.55f;
			    pEffect->GetDamageData().fHitGap = 1.0f;

			    pDamageData->bReAttack  = true;
			    pDamageData->fHitGap	= 1.f;
			    pDamageData->fHitAddMP			*= 0.55f;
			    pDamageData->m_fRateModifier	*= 0.55f;
			    pDamageData->techPoint			= int(pDamageData->techPoint * 0.55);

			    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
			    if( m_bLockOnNpc == true )
				    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
			    else
				    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;

			    D3DXVECTOR3 vInductionScale = pEffect->GetMainEffectScaleByUnit();
                if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                {
			        D3DXVECTOR3 vScale = pMeshInstance->GetScale();

			        vInductionScale.x *= vScale.x;
			        vInductionScale.y *= vScale.y;
			        vInductionScale.z *= vScale.z;
			        vInductionScale *= 0.8f;

			        pEffect->SetScale( vInductionScale );
                }
            }
		}
		else if ( m_EveElectraSystem.GetEnableSystem() == true && m_EveElectraSystem.GetPlaneFormation() == PFT_FUSION )
		{
			for(UINT i = 0; i < 3; ++i)
			{
				if( i == 1 )
				{
					vRotateDegree.x += 120.f;
				}
				else if( i == 2)
				{
					vRotateDegree.x += 120.f;
				}

                if ( pThousandStarMeshInstance != NULL )
                {
				    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate,
					    pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

				    CX2DamageManager::DamageData* pDamageData = &pEffect->GetDamageData();

				    pEffect->SetDamageTime( pEffect->GetDamageTime() + 1 );
				    pEffect->GetDamageData().damage.fPhysic *= 0.25f;
				    pEffect->GetDamageData().damage.fMagic *= 0.25f;
				    pEffect->GetDamageData().fHitGap = 1.0f;

				    pDamageData->bReAttack  = true;
				    pDamageData->fHitGap	= 1.f;
				    pDamageData->fHitAddMP			*= 0.25f;
				    pDamageData->m_fRateModifier	*= 0.25f;
				    pDamageData->techPoint			= int(pDamageData->techPoint * 0.25);

				    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
				    if( m_bLockOnNpc == true )
					    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
				    else
					    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;

				    D3DXVECTOR3 vInductionScale = pEffect->GetMainEffectScaleByUnit();
                    if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
                    {
				        D3DXVECTOR3 vScale = pMeshInstance->GetScale();

				        vInductionScale.x *= vScale.x;
				        vInductionScale.y *= vScale.y;
				        vInductionScale.z *= vScale.z;
				        vInductionScale *= 0.75f;

				        pEffect->SetScale( vInductionScale );
                    }
                }
			}
		}
		else
		{
            if ( pThousandStarMeshInstance != NULL )
            {
			    pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), m_fThousandStarPowerRate, pThousandStarMeshInstance->GetPos(), vRotateDegree, vRotateDegree );

			    CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();
			    if( m_bLockOnNpc == true )
				    pLockOnData->m_LockOnNPCUID = (int)m_iLockOnUid;
			    else
				    pLockOnData->m_LockOnUnitUID = m_iLockOnUid;
            }
			// 			if( NULL != pEffect )
			// 				m_EveElectraSystem.AddLockonToSystem( pEffect, L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED", L"EVE_THOUSANDSTAR_MAGICBALL_FILTERED"
		}


        if ( pThousandStarMeshInstance != NULL )
		    g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>( this ),  L"eve_thousandsOfStars_model_P02", pThousandStarMeshInstance->GetPos() );


		PlaySound( L"Raven_NasodCore_Attack.ogg" );

		m_iThousandStarCount--;
		if( m_iThousandStarCount <= 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS );

			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				CX2UserSkillTree& cUserSkillTree = GetUnit()->AccessUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				float fRemainCoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - m_fElapsedTimeThousandStartCreated;

				if( fRemainCoolTime < 0.f )
					fRemainCoolTime = 0.f;

				cUserSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_SA_EEL_THOUSANDS_OF_STARS, fRemainCoolTime  );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
			ClearThousandStar();
		}

	}
}

void CX2GUEve::AttackResultByType( CX2DamageManager::DamageData &pDamageData )
{
	CX2GUUser::AttackResultByType( pDamageData );

	// 	if( pDamageData.pDefenderUnit == NULL || pDamageData.pDefenderUnit->GetNowHp() <= 0.f )
	// 		return;

	if( pDamageData.optrDefenderGameUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
	{
		CX2GUNPC *pNpc = static_cast<CX2GUNPC *>( pDamageData.optrDefenderGameUnit.GetObservable() );
		if( pNpc != NULL && pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
			return;
	}

	if( m_bDelayAttackThousandStar == false &&
		pDamageData.m_eDamageTrigger != CX2DamageManager::DTT_EVE_THOUSAND_STAR &&
		pDamageData.m_fHpPercentUp <= 0.f &&
		m_pThousandStar != NULL &&
        m_pThousandStar->m_hEffect != INVALID_DAMAGE_EFFECT_HANDLE
        )
    {
	    if( CX2DamageEffect::CEffect* pTSEffect = m_pThousandStar->GetLiveDamageEffect() )
        {
            CKTDGXMeshPlayer::CXMeshInstance* pThousandStarMeshInstance = pTSEffect->GetMainEffect();
		    if ( pThousandStarMeshInstance != NULL
                && pThousandStarMeshInstance->GetGlobalTime() > 3.f )
	        {
		        // �߻�
		        m_bDelayAttackThousandStar = true;
		        m_fDelayAttackTime = 0.f;
		        m_fThousandStarAttackTime = 0.f;

		        if( CX2DamageManager::AT_UNIT == pDamageData.defenderType )
		        {
			        if( null != pDamageData.optrDefenderGameUnit )
			        {
				        if( CX2GameUnit::GUT_USER == pDamageData.optrDefenderGameUnit->GetGameUnitType() )
				        {
					        m_bLockOnNpc = false;
					        m_iLockOnUid = pDamageData.optrDefenderGameUnit->GetUnitUID();
				        }
				        else if( CX2GameUnit::GUT_NPC == pDamageData.optrDefenderGameUnit->GetGameUnitType() )
				        {
					        m_bLockOnNpc = true;
					        m_iLockOnUid = pDamageData.optrDefenderGameUnit->GetUnitUID();
				        }
			        }
		        }

		        pThousandStarMeshInstance->SetDirSpeed( 0.f );


		        // PlaySound( L"Raven_NasodCore_Trans_Attack.ogg" );
				// �ش� �������� ���� �⵿ �������� ���� �ʰ� �������� �ʴ� ���� ����
				return;
            }
        }
	}
	if( pDamageData.m_fHpPercentUp <= 0.f )
	{
		if( CX2DamageManager::AT_UNIT == pDamageData.defenderType )
		{
			if( null != pDamageData.optrDefenderGameUnit )
			{
				if( CX2GameUnit::GUT_USER == pDamageData.optrDefenderGameUnit->GetGameUnitType() )
				{
					m_bLockOnNpc = false;
					m_iLockOnUid = pDamageData.optrDefenderGameUnit->GetUnitUID();
				}
				else if( CX2GameUnit::GUT_NPC == pDamageData.optrDefenderGameUnit->GetGameUnitType() )
				{
					m_bLockOnNpc = true;
					m_iLockOnUid = pDamageData.optrDefenderGameUnit->GetUnitUID();
				}
			}
		}
	}


}

void CX2GUEve::ENSI_COMBO_X_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}

void CX2GUEve::ENSI_COMBO_XX_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}

void CX2GUEve::ENSI_COMBO_XXX_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}

void CX2GUEve::ENSI_EPR_COMBO_ZZZfrontZ_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
	m_bNotEnoughMPZZZfrontZLaser = false;
    m_hEffectElectraLaserZZZfrontZ = INVALID_DAMAGE_EFFECT_HANDLE;

	m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
	m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
}

void CX2GUEve::ENSI_EPR_COMBO_ZZZfrontZ_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( 0.f, 2.0f, 240.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.3666f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{

		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (130.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_COMBO_ZZZFZ_LASER", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        m_hEffectElectraLaserZZZfrontZ = pEffect->GetHandle();

		m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Finger2Nub", vDamageEffectPos,
			vZeroYDirVector, 5000.f, 0.108f, 0.5f, L"", L"", L"", true, 2.f );
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.4f && m_pXSkinAnim->GetNowAnimationTime() < 1.23333f )
	{
		if( m_bNotEnoughMPZZZfrontZLaser == false && FlushMp(13.3333f * m_fElapsedTime ) == false )
		{
			m_EveElectraSystem.DeleteLaserFromSystem();
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger2Nub" );

			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
			m_bNotEnoughMPZZZfrontZLaser = true;
		}
	}

    if ( CX2DamageEffect::CEffect* pEffectElectraLaserZZZfrontZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraLaserZZZfrontZ ) )
	{
		if( m_pXSkinAnim->GetNowAnimationTime() >= 0.4f && m_pXSkinAnim->GetNowAnimationTime() < 1.0f )
		{
			if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectElectraLaserZZZfrontZ->GetMainEffect() )
			{
				D3DXVECTOR3 vRotateLocalDegree = pMeshInstance->GetRotateLocalDegree();
				vRotateLocalDegree.z += (78.3f * m_fElapsedTime);
				pMeshInstance->SetRotateLocalDegree(vRotateLocalDegree);
			}

			m_EveElectraSystem.m_fLaserEndOffsetY = 15.f;
			m_EveElectraSystem.m_fLaserEndOffsetX = -8.f;
		}
		else
		{
			m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
			m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
		}
	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_EPR_COMBO_ZZZfrontZ_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.3666f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
}

void CX2GUEve::ENSI_EPR_COMBO_ZZZfrontZ_End()
{
	m_EveElectraSystem.DeleteLaserFromSystem();
    m_hEffectElectraLaserZZZfrontZ = INVALID_DAMAGE_EFFECT_HANDLE;

	m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
	m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
	CommonStateEnd();
}

void CX2GUEve::ENSI_EPR_COMBO_XXZ_Start()
{
	CommonStateStart();
	m_bXXZLaserEnd = false;
	m_bNotEnoughMPXXZLaser = false;
	m_fPlaneZAngle	= 0.f;
    m_hEffectElectraLaserXXZ = INVALID_DAMAGE_EFFECT_HANDLE;
	SetNoDetonation(true);

	m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
	m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
}

void CX2GUEve::ENSI_EPR_COMBO_XXZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.4f, 220.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.23333f ) )
	{

		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (130.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_COMBO_XXZ_LASER", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        m_hEffectElectraLaserXXZ = pEffect->GetHandle();

		m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Finger2", vDamageEffectPos, vZeroYDirVector, 5000.f, 0.108f, 0.5f,
			L"EVE_COMBO_XXZ_LASER_NEXT", L"EVE_COMBO_XXZ_LASER_NEXT", L"EVE_COMBO_XXZ_LASER_NEXT", true, 2.f );
	}

    if ( CX2DamageEffect::CEffect* pEffectElectraLaserXXZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraLaserXXZ ) )
	{
		if( m_pXSkinAnim->GetNowAnimationTime() >= 0.83333f && m_pXSkinAnim->GetNowAnimationTime() < 1.93333f )
		{
			if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstanceLaserXXZ = pEffectElectraLaserXXZ->GetMainEffect() )
			{
				D3DXVECTOR3 vRotateLocalDegree = pMeshInstanceLaserXXZ->GetRotateLocalDegree();
				vRotateLocalDegree.z += (41.904f * m_fElapsedTime);
				pMeshInstanceLaserXXZ->SetRotateLocalDegree(vRotateLocalDegree);
				m_EveElectraSystem.m_fLaserEndOffsetY = 2.f;
			}
		}
		else
		{
			m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.76666f ) )
	{
		m_bXXZLaserEnd = true;
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_COMBO_XXZ_END", this );
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.23333f && m_pXSkinAnim->GetNowAnimationTime() < 1.8666f )
	{
		if( m_bNotEnoughMPXXZLaser == false && FlushMp(13.3333f * m_fElapsedTime) == false )
		{
			m_EveElectraSystem.DeleteLaserFromSystem();
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger2Nub" );

			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
			m_bNotEnoughMPXXZLaser = true;
		}
	}


	CommonFrameMove();
}

void CX2GUEve::ENSI_EPR_COMBO_XXZ_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.3f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_InputData.pureZ == false )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			StateChange( USI_WAIT );
		}
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
}

void CX2GUEve::ENSI_EPR_COMBO_XXZ_End()
{
	m_EveElectraSystem.DeleteLaserFromSystem();
	if( m_bXXZLaserEnd == false )
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_COMBO_XXZ_END", this );
    m_hEffectElectraLaserXXZ = INVALID_DAMAGE_EFFECT_HANDLE;
	SetNoDetonation(false);

	m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
	m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
	CommonStateEnd();
}


void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZ_Init()
{
	XSkinMeshReadyInBackground( L"Eve_AT_DashComboZ2_mesh.X" );
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZ_StartFuture()
{
	CommonStateStartFuture();
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZ_Start()
{
	CommonStateStart();

	D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"EvePurityDashComboZBladeLoopEnd", vBonePos, GetRotateDegree(), GetRotateDegree() );
	if( NULL != pMeshInst )
	{
		m_hElectraDashComboZMesh = pMeshInst->GetHandle();
	}
    m_hEffectElectraDashComboZZ = INVALID_DAMAGE_EFFECT_HANDLE;
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZ_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, ENSI_EPR_DASH_COMBO_ZZZ )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
        m_hEffectElectraDashComboZZ = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
            this, L"EVE_PR_DASH_COMBO_ZZ", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );

	}

    if( CX2DamageEffect::CEffect* pEffectElectraDashComboZZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraDashComboZZ ) )
    {
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectElectraDashComboZZ->GetMainEffect() )
	    {
		    D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
		    pMeshInstance->SetPos( vBonePos );
	    }
    }

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hElectraDashComboZMesh );
	if( NULL != pMeshInst )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
		pMeshInst->SetPos( vBonePos );
	}


	CommonFrameMove();
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZ_End()
{
	if ( m_hEffectElectraDashComboZZ != INVALID_DAMAGE_EFFECT_HANDLE )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectElectraDashComboZZ );
	g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hElectraDashComboZMesh );
	CommonStateEnd();
}



void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_Init()
{
	//XSkinMeshReady( L"Eve_AT_DashComboZ2_mesh.X" );
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_StartFuture()
{
	CommonStateStartFuture();
	m_bDisableGravity = true;
}
void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_Start()
{
	CommonStateStart();

	m_bNotEnoughMPDashZZZ = false;

	D3DXVECTOR3 vBonePosL = GetBonePos( L"Dummy2_Lhand" );
    m_hEffectElectraDashComboZ = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
        this, L"EVE_PR_DASH_COMBO_ZZZ_L", GetPowerRate(), vBonePosL, GetRotateDegree(), GetRotateDegree() );

	D3DXVECTOR3 vBonePosR = GetBonePos( L"Dummy1_Rhand" );
    m_hEffectElectraDashComboZZ = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
        this, L"EVE_PR_DASH_COMBO_ZZZ_R", GetPowerRate(), vBonePosR, GetRotateDegree(), GetRotateDegree() );

	m_fBlinkInvisibilityAlpha	= 1.f;
	m_bBlinkInvisibility		= true;

	SetAlphaObject( true );
	m_RenderParam.bAlphaBlend = true;
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_EventProcess()
{

	if( false == IsOnSomethingFuture() )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
		{
			StateChange( USI_JUMP_DOWN );
		}
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_FrameMove()
{
    if ( CX2DamageEffect::CEffect* pEffectElectraDashComboZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraDashComboZ ) )
    {
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectElectraDashComboZ->GetMainEffect() )
	    {
		    D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
		    pMeshInstance->SetPos( vBonePos );
        }
	}

    if ( CX2DamageEffect::CEffect* pEffectElectraDashComboZZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraDashComboZZ ) )
	{
		if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectElectraDashComboZZ->GetMainEffect() )
        {
		    D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy1_Rhand" );
		    pMeshInstance->SetPos( vBonePos );
        }
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.43333f ) &&
		m_bNotEnoughMPDashZZZ == false )
	{
		if( FlushMp( 10.f ) == false )
		{
			m_bNotEnoughMPDashZZZ = true;
		}
		else
		{
			D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy2_Lhand" );
			vDamageEffectPos.y += 0.f;
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
			    pMeshInstance->SetGlobalTime( 0.2f);
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.46666f ) &&
		m_bNotEnoughMPDashZZZ == false )
	{
		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy1_Rhand" );
		vDamageEffectPos.y += 20.f;
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
		    pMeshInstance->SetGlobalTime( 0.16666f);

	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) &&
		m_bNotEnoughMPDashZZZ == false )
	{
		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy2_Lhand" );
		vDamageEffectPos.y += -40.f;
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
		    pMeshInstance->SetGlobalTime( 0.13333f);

	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.53333f ) )
	{
		if( m_bNotEnoughMPDashZZZ == true )
		{
			D3DXVECTOR3 vBonePosR = GetBonePos( L"Dummy2_Lhand" );

			vBonePosR.y += 30.f;
			CreateNotEnoughMPEffect( vBonePosR, 0.f, 180.f, 0.f );
		}
		else
		{
			D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy1_Rhand" );
			vDamageEffectPos.y += 10.f;
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
            if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
			    pMeshInstance->SetGlobalTime( 0.1f);
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.56666f ) &&
		m_bNotEnoughMPDashZZZ == false )
	{
		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy2_Lhand" );
		vDamageEffectPos.y += -20.f;
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
		    pMeshInstance->SetGlobalTime( 0.06666f);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) &&
		m_bNotEnoughMPDashZZZ == false )
	{
		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy1_Rhand" );
		vDamageEffectPos.y += 40.f;
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect() )
		    pMeshInstance->SetGlobalTime( 0.03333f);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.6333f ) &&
		m_bNotEnoughMPDashZZZ == false )
	{
		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Dummy2_Lhand" );
		vDamageEffectPos.y += 10.f;
		[[maybe_unused]] CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PR_DASH_COMBO_ZZZ_EXPLOSION", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.2f && m_pXSkinAnim->GetNowAnimationTime() < 0.43333f)
	{
		m_fBlinkInvisibilityAlpha -= m_fElapsedTime * 20.f;
		if( m_fBlinkInvisibilityAlpha < 0.3333f )
			m_fBlinkInvisibilityAlpha = 0.3333f;
	}
	else if( m_pXSkinAnim->GetNowAnimationTime() > 0.43333f )
	{
		m_fBlinkInvisibilityAlpha += m_fElapsedTime * 20.f;
		if( m_fBlinkInvisibilityAlpha > 1.f )
			m_fBlinkInvisibilityAlpha = 1.f;
	}

	CommonFrameMove();
}


void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}

void CX2GUEve::ENSI_EPR_DASH_COMBO_ZZZ_End()
{
    if ( m_hEffectElectraDashComboZ != INVALID_DAMAGE_EFFECT_HANDLE )
	    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectElectraDashComboZ );
	if ( m_hEffectElectraDashComboZZ != INVALID_DAMAGE_EFFECT_HANDLE )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectElectraDashComboZZ );
	CommonStateEnd();

	m_bBlinkInvisibility			= false;

	if( m_bInvisibility == false )
	{
		SetAlphaObject( false );
		m_RenderParam.bAlphaBlend = false;
		m_RenderParam.outLineColor.a	= 1.f;
		m_RenderParam.color.a			= 1.f;
	}
}




void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_Init()
{
	//XSkinMeshReady( L"Eve_EP_SI_SA_Spit_Fire_Mesh01.X" );
	//XSkinMeshReady( L"Eve_EP_SI_SA_Spit_Fire_Mesh02.X" );
	//XMeshReady( L"Wind_Liner02.Y" );
	//TextureReady( L"GuideArrow02.tga" );
	//TextureReady( L"ColorBallGray.tga" );
}


void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_Start()
{
	CommonStateStart();

	m_fPhotonBlinkBackSpeedX = 0.f;
	m_fPhotonBlinkDummyLifeTime = 0.f;
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EN_PHOTON_BLINK);
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_EN_PHOTON_BLINK ) );	/// ��ų ����

		m_fPhotonBlinkBackSpeedX	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TELEPORT_RANGE_ABS, iSkillTempletLevel );
		m_fPhotonBlinkDummyLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
	}

	m_fBlinkInvisibilityAlpha	= 1.f;
	m_bBlinkInvisibility		= true;

	SetAlphaObject( true );
	m_RenderParam.bAlphaBlend = true;
}

void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_StartFuture()
{
	CommonStateStartFuture();
	m_bDisableGravity = true;
}

void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.1f ) )
	{
		m_PhysicParam.nowSpeed.x = -m_fPhotonBlinkBackSpeedX;
	}
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.4f ) )
	{
		m_PhysicParam.nowSpeed.x = -300.f;
	}
	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_PHOTON_BLINK_DUMMY", GetPowerRate(),
			GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
		{
			pMeshInstance->SetMaxLifeTime( m_fPhotonBlinkDummyLifeTime );
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.03f && m_pXSkinAnim->GetNowAnimationTime() < 0.3f)
	{
		m_fBlinkInvisibilityAlpha -= m_fElapsedTime * 10.f;
		if( m_fBlinkInvisibilityAlpha < 0.f )
			m_fBlinkInvisibilityAlpha = 0.f;
	}
	else if( m_pXSkinAnim->GetNowAnimationTime() > 0.3f && m_pXSkinAnim->GetNowAnimationTime() < 0.8f )
	{
		m_fBlinkInvisibilityAlpha += m_fElapsedTime * 5.f;
		if( m_fBlinkInvisibilityAlpha > 1.f )
			m_fBlinkInvisibilityAlpha = 1.f;
	}
	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.7f )
		{
			StateChange( USI_JUMP_DOWN );
		}
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_EndFuture()
{
	CommonStateEndFuture();
	m_bDisableGravity = false;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUEve::ENSI_A_EVE_PHOTON_BLINK_End()
{
	CommonStateEnd();

	m_bBlinkInvisibility			= false;

	if( m_bInvisibility == false )
	{
		SetAlphaObject( false );
		m_RenderParam.bAlphaBlend = false;
		m_RenderParam.outLineColor.a	= 1.f;
		m_RenderParam.color.a			= 1.f;
	}
}

void CX2GUEve::ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Init()
{
	//XSkinMeshReady( L"Eve_EP_SI_SA_Spit_Fire_Mesh01.X" );
	//XSkinMeshReady( L"Eve_EP_SI_SA_Spit_Fire_Mesh02.X" );
	//XMeshReady( L"Wind_Liner02.Y" );
	//TextureReady( L"GuideArrow02.tga" );
	//TextureReady( L"ColorBallGray.tga" );
}

void CX2GUEve::ENSI_A_EPR_SPECTRO_EL_CRYSTAL_Start()
{
	CommonStateStart();
	StartElCrystalSystem();
}

void CX2GUEve::ENSI_A_EPR_SPECTRO_EL_CRYSTAL_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		if( m_EveElectraSystem.GetEnableSystem() == true )
		{
			m_EveElectraSystem.StartFormationEffect();
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPECTRO_EL_CRYSTAL_COMBO_DAMAGE", GetPowerRate(), GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree() );
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EPR_SPECTRO_EL_CRYSTAL_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_Init()
{
	//TextureReady( L"Nasod_King_Laser03.tga" );
}
void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}
void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}
void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.01f ) )
	{
		m_PhysicParam.nowSpeed.x = 0.f;
	}
	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.4f, 160.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.43333f ) )
	{

		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (100.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = NULL;
		if( GetRemainHyperModeTime() > 0.f )
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO14 ) == true )
			{
				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_MEMO", GetPowerRate(),
					vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

				m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.12f, 1.0f,
					L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_NEXT_MEMO", L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_NEXT_MEMO",
					L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_NEXT_MEMO", false );
			}
			else
			{
				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_PARTICLE_RAY_LASER_HYPER", GetPowerRate(),
					vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

				m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.12f, 1.0f,
					L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_NEXT", L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_NEXT",
					L"EVE_EPR_PARTICLE_RAY_LASER_HYPER_NEXT", false );
			}
		}
		else
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO14 ) == true )
			{
				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_PARTICLE_RAY_LASER_MEMO", GetPowerRate(),
					vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

				m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.12f, 1.0f,
					L"EVE_EPR_PARTICLE_RAY_LASER_NEXT_MEMO", L"EVE_EPR_PARTICLE_RAY_LASER_NEXT_MEMO",
					L"EVE_EPR_PARTICLE_RAY_LASER_NEXT_MEMO", false );
			}
			else
			{
				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_PARTICLE_RAY_LASER", GetPowerRate(),
					vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

				m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.12f, 1.0f,
					L"EVE_EPR_PARTICLE_RAY_LASER_NEXT", L"EVE_EPR_PARTICLE_RAY_LASER_NEXT",
					L"EVE_EPR_PARTICLE_RAY_LASER_NEXT", false );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_PARTICLE_RAY_End()
{
	CommonStateEnd();
	m_EveElectraSystem.DeleteLaserFromSystem();
}


void CX2GUEve::ENSI_SA_EPR_PHOTON_FLARE_Init()
{
	XMeshReadyInBackground( L"eve_SA_photonFlare_lightLine.Y" );
	TextureReadyInBackground( L"eve_SA_photonFlare_lightLine.tga" );
	TextureReadyInBackground( L"WhitePoint.tga" );
	TextureReadyInBackground( L"Explosion_Sphere.tga" );
	TextureReadyInBackground( L"AeroTornado04.tga" );
	TextureReadyInBackground( L"Particle_Blur.tga" );
	TextureReadyInBackground( L"WhiteCircle02.tga" );
	TextureReadyInBackground( L"CenterLight_Gray01.tga" );
}
void CX2GUEve::ENSI_SA_EPR_PHOTON_FLARE_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

}

void CX2GUEve::ENSI_SA_EPR_PHOTON_FLARE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.99f ) )
	{
		const float fBasicEffectRange = 600.f;
		float fEffectRange = 0.f;
		float fEffectiveTime = 0.f;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EEL_PHOTON_FLARE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_EEL_PHOTON_FLARE ) );	/// ��ų ����

			if( GetRemainHyperModeTime() > 0.f )
			{
				fEffectRange = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS_HYPER, iSkillTempletLevel );
				fEffectiveTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME_HYPER, iSkillTempletLevel );
			}
			else
			{
				fEffectRange = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iSkillTempletLevel );
				fEffectiveTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
			}
		}

		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Bip01_Spine" );

		if( IsMyUnit() == true )
			g_pX2Game->GetMajorParticle()->CreateSequence( NULL,  L"WhiteOut", D3DXVECTOR3(0,0,0) );

		CX2DamageEffect::CEffect* pEffectDamage = NULL;

		if( GetRemainHyperModeTime() > 0.f )
			pEffectDamage = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_PHOTON_FLARE_HYPER", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
		else
			pEffectDamage = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_PHOTON_FLARE", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );


		float fEffectScale = fEffectRange / fBasicEffectRange;
		pEffectDamage->SetScale( D3DXVECTOR3( fEffectScale, fEffectScale, fEffectScale ));
		pEffectDamage->GetDamageData().m_ExtraDamage.m_fTime = fEffectiveTime;
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_PHOTON_FLARE_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.7f )
		{
			StateChange( USI_JUMP_DOWN );
		}
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_PHOTON_FLARE_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}


void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_READY_Init()
{
	XMeshReadyInBackground( L"eve_SA_photonFlare_lightLine.Y" );
	TextureReadyInBackground( L"eve_SA_photonFlare_lightLine.tga" );
	TextureReadyInBackground( L"WhitePoint.tga" );
	TextureReadyInBackground( L"Explosion_Sphere.tga" );
	TextureReadyInBackground( L"AeroTornado04.tga" );
	TextureReadyInBackground( L"Particle_Blur.tga" );
	TextureReadyInBackground( L"WhiteCircle02.tga" );
	TextureReadyInBackground( L"CenterLight_Gray01.tga" );
}
void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_READY_Start()
{
	CommonStateStart();
	m_fPlaneZAngle = 0.f;
}

//////// READY ///////////
void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_READY_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.CreatePlane( 0.f, 7.0f, 340.f, 130.f, L"EveProtoPuritySweepParadePlane" );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_READY_EventProcess()
{
	if( m_InputData.pureUp == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
	}
	else if( m_InputData.pureDown == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ENSI_SA_EPR_SWEEP_ROLLING_CHARGE );
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_READY_End()
{
	CommonStateEnd();
}


//////// CHARGE ///////////
void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_Start()
{
	CommonStateStart();
	D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Bip01_R_Hand" );

	if(GetIsRight() == true)
		vDamageEffectPos += (150.f * GetDirVector() );
	else
		vDamageEffectPos -= (150.f * GetDirVector() );

    CX2DamageEffect::CEffect* pEffectSweepParadeTriangle;
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO15 ) == true )
		pEffectSweepParadeTriangle = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_SWEEP_PARADE_TRIANGLE_MEMO", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
	else
		pEffectSweepParadeTriangle = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_SWEEP_PARADE_TRIANGLE", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
    m_hEffectSweepParadeTriangle = pEffectSweepParadeTriangle->GetHandle();

	m_EveElectraSystem.AddEffectToSystem( pEffectSweepParadeTriangle );
    if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectSweepParadeTriangle->GetMainEffect() )
	    pMeshInstance->SetVelocity( D3DXVECTOR3( 0.f, 0.f, 0.f ) );

	m_vInitScaleByUnitSweepParade	= GetVec3ScaleByUnit();

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO15 ) == true )
		m_vInitScaleByUnitSweepParade *= 1.2f;

	m_fSweepParadeTriangleLIfeTime	= m_FrameDataNow.unitCondition.fStateTime;
	m_fInitDamageSweepParade		= pEffectSweepParadeTriangle->GetDamageData().damage.fMagic;

    m_hEffectSweepParadeElectric		= g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
        this, L"EVE_SWEEP_PARADE_ELECTRIC", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
    m_hEffectSweepParadeLight		= g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
        this, L"EVE_SWEEP_PARADE_LIGHT", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_FrameMove()
{
    if ( CX2DamageEffect::CEffect* pEffectSweepParadeTriangle = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectSweepParadeTriangle ) )
	{
		float fScaleEffect = (1.f + m_FrameDataNow.unitCondition.fStateTime * 0.16667f) * m_vInitScaleByUnitSweepParade.x;
		pEffectSweepParadeTriangle->SetScaleByUnit( D3DXVECTOR3( fScaleEffect, fScaleEffect, fScaleEffect ) );

		// ��ġ ���� : 0.06667f -> 0.075
		float fScaleDamage = (1.f + m_FrameDataNow.unitCondition.fStateTime * 0.075f);
		pEffectSweepParadeTriangle->GetDamageData().damage.fMagic = m_fInitDamageSweepParade * fScaleDamage;

		m_fSweepParadeTriangleLIfeTime = m_FrameDataNow.unitCondition.fStateTime;

		float fScaleEffectElectric	= (1.f + m_FrameDataNow.unitCondition.fStateTime * 0.15f) * m_vInitScaleByUnitSweepParade.x;
		float fScaleEffectLight		= (1.f + m_FrameDataNow.unitCondition.fStateTime * 0.1f) * m_vInitScaleByUnitSweepParade.x;

        if ( CX2DamageEffect::CEffect* pEffectSweepParadeElectric = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectSweepParadeElectric ) )
            pEffectSweepParadeElectric->SetScaleByUnit( D3DXVECTOR3( fScaleEffectElectric, fScaleEffectElectric, fScaleEffectElectric ) );
        if ( CX2DamageEffect::CEffect* pEffectSweepParadeLight = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectSweepParadeLight ) )
            pEffectSweepParadeLight->SetScaleByUnit( D3DXVECTOR3( fScaleEffectLight, fScaleEffectLight, fScaleEffectLight ) );


		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Bip01_R_Hand" );

		if(GetIsRight() == true)
			vDamageEffectPos += (150.f * GetDirVector() );
		else
			vDamageEffectPos -= (150.f * GetDirVector() );
		pEffectSweepParadeTriangle->SetPos( vDamageEffectPos );
	}
	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_EventProcess()
{
	float fFireTime = 5.f;
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO15 ) == true )
		fFireTime = 5.5f;
	if( m_InputData.pureUp == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
	}
	else if( m_InputData.pureDown == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
	}

	if( (m_eSpecialAttackKeyPressed == SAKP_A && m_InputData.pureA != true) ||
		(m_eSpecialAttackKeyPressed == SAKP_S && m_InputData.pureS != true) ||
		(m_eSpecialAttackKeyPressed == SAKP_D && m_InputData.pureD != true) ||
		(m_eSpecialAttackKeyPressed == SAKP_C && m_InputData.pureC != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_Q && m_InputData.pureQ != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_W && m_InputData.pureW != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_E && m_InputData.pureE != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_R && m_InputData.pureR != true)
		)
	{
		StateChange( ENSI_SA_EPR_SWEEP_ROLLING_FIRE );
	}
	else if( EventTimer( fFireTime, true ) == true )
	{
		StateChange( ENSI_SA_EPR_SWEEP_ROLLING_END );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_CHARGE_End()
{
    if ( m_hEffectSweepParadeLight != INVALID_DAMAGE_EFFECT_HANDLE )
        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectSweepParadeLight );
	if ( m_hEffectSweepParadeElectric != INVALID_DAMAGE_EFFECT_HANDLE )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEffectSweepParadeElectric );
	CommonStateEnd();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
}

//////// FIRE ///////////
void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_FIRE_Start()
{
    if ( CX2DamageEffect::CEffect* pEffectSweepParadeTriangle = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectSweepParadeTriangle ) )
    {
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectSweepParadeTriangle->GetMainEffect() )
	    {
		    if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO15 ) == true )
		    {
			    pMeshInstance->SetMaxLifeTime( 5.6f );
		    }
		    else
		    {
			    pMeshInstance->SetMaxLifeTime( 5.1f );
		    }
		    pMeshInstance->SetVelocity( D3DXVECTOR3( 400.f, 0.f, 0.f ) );
		    pMeshInstance->SetNowLifeTime( m_fSweepParadeTriangleLIfeTime );
		    pEffectSweepParadeTriangle->GetDamageData().reActType = CX2DamageManager::RT_BIG_DAMAGE;

	    }
    }
	CommonStateStart();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_FIRE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_FIRE_End()
{
	CommonStateEnd();
}

//////// END ///////////
void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_END_Start()
{
	CommonStateStart();
    if ( CX2DamageEffect::CEffect* pEffectSweepParadeTriangle = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectSweepParadeTriangle ) )
	{
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectSweepParadeTriangle->GetMainEffect() )
		{
			pMeshInstance->SetMaxLifeTime( 5.1f );
			pMeshInstance->SetNowLifeTime( 5.0f );
			pEffectSweepParadeTriangle->GetDamageData().reActType = CX2DamageManager::RT_FLY;
			pEffectSweepParadeTriangle->GetDamageData().backSpeed.x = 1500.f;
			pEffectSweepParadeTriangle->GetDamageData().backSpeed.y = 1500.f;
			pEffectSweepParadeTriangle->GetDamageData().hitUnitList.clear();
			pEffectSweepParadeTriangle->GetDamageData().bReAttack = false;
		}
	}
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_END_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_END_EventProcess()
{
	if( EventTimer( 1.0f, true ) == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_SWEEP_ROLLING_END_End()
{
	CommonStateEnd();
}



void CX2GUEve::ENSI_SA_EPR_THOUSANDS_OF_STARS_Init()
{

}

void CX2GUEve::ENSI_SA_EPR_THOUSANDS_OF_STARS_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		CreateThousandStar();
		m_fThousandStarPowerRate = GetPowerRate();
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"THOUSANDS_OF_STARS_COMBO_DAMAGE", m_fThousandStarPowerRate, GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree() );

		if ( IsMyUnit() )
			g_pX2Game->UpdateSkillSlotUI();
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.93333f ) )
	{
		ChangeThousandsOfStarsFormation();
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_THOUSANDS_OF_STARS_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}



void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_Init()
{
	//TextureReady( L"Nasod_King_Laser03.tga" );
}
void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}

void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.01f ) )
	{
		m_PhysicParam.nowSpeed.x = 0.f;
	}
	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( 0.f, 3.0f, 110.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		if( m_fPlaneZAngle > 0 && m_EveElectraSystem.GetEnablePlane() == true )
			m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 2.5f, 110.f + 120.f, 130.f + 69.24f );
		//m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 3.0f, 110.f + 100.f, 130.f + 57.7f );
		else if( m_fPlaneZAngle < 0 && m_EveElectraSystem.GetEnablePlane() == true )
			m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 2.5f, 110.f + 120.f, 130.f - 69.24f );
		//m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 3.0f, 110.f + 100.f, 130.f - 57.7f );
		else
			m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 2.5f, 110.f + 139.56f, 130.f );
		//m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 3.0f, 110.f + 116.3f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{

		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (100.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = NULL;

		if( GetRemainHyperModeTime() > 0.f )
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_GIGA_STREAM_LASER_HYPER", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.3f, 1.0f,
				L"EVE_EPR_GIGA_STREAM_LASER_HYPER_NEXT", L"EVE_EPR_GIGA_STREAM_LASER_HYPER_NEXT",
				L"EVE_EPR_GIGA_STREAM_LASER_HYPER_NEXT", false );
		}
		else
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_GIGA_STREAM_LASER", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.3f, 1.0f,
				L"EVE_EPR_GIGA_STREAM_LASER_NEXT", L"EVE_EPR_GIGA_STREAM_LASER_NEXT",
				L"EVE_EPR_GIGA_STREAM_LASER_NEXT", false );
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EPR_GIGA_STREAM_End()
{
	CommonStateEnd();
	m_EveElectraSystem.DeleteLaserFromSystem();
}

void CX2GUEve::DeleteMinorParticle()
{
	CX2GUUser::DeleteMinorParticle();

	DeleteEveMinorParticle();
}

/** @function : HyperModeBuffEffectStart
@brief : ���� �� ���;��ϴ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUEve::HyperModeBuffEffectStart()
{
	CX2GUUser::HyperModeBuffEffectStart();

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRFoot )
		m_hHyperBoostRFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightEve",		0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLFoot )
		m_hHyperBoostLFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftEve",		0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRArm )
		m_hHyperBoostRArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightEve",		0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLArm )
		m_hHyperBoostLArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftEve",		0, 0, 0, 0, 0);

}

/** @function : GetAdditionalAccelBuffTime
@brief : ���������ͷ� ������ �߰��ð��� ������ �Լ�
@param : � ���������Ͱ� �ߵ��ƴ����� ���� ��ų���̵�(eSkillID_)
@return : �߰��ð�(float)
*/
/*virtual*/ float CX2GUEve::GetAdditionalAccelBuffTime( const CX2SkillTree::SKILL_ID /*eSkillID_*/ ) const
{
	float fAdditionalAccelBuffTime = 0.0f;

	/// ����ƽ, �ݷ��� ���
	const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA, true );

	if ( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA );
		if( NULL != pSkillTemplet )
		{
			fAdditionalAccelBuffTime += pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel );
		}
	}

	return fAdditionalAccelBuffTime;
}

/** @function : ApplyAuraAcceleratorBuffToGameUnit
@brief : �׼��������͹����� Ư�� �������� ���ֿ��� ����, �̺�� �ΰ����� ȿ�� ó���� ���� overriding
@param : �ټ��������ͽ�ų���ø�(pSkillTemplet_), �ܺο���(fAddtionalAccelBuffTime_)
*/
/*virtual*/ void CX2GUEve::ApplyAuraAcceleratorBuffToGameUnit( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const float fAddtionalAccelBuffTime_, const D3DXVECTOR3& vMyPos_ )
{
	CX2BuffFactorPtr ptrEnergyOfConcentrationFactor
		= GetEnergyOfConcentrationBuffFactorClonePtr();

	/// ������ ����� �ȹ������
	if ( NULL == ptrEnergyOfConcentrationFactor )
		CX2GUUser::ApplyAuraAcceleratorBuffToGameUnit( pSkillTemplet_, fAddtionalAccelBuffTime_, vMyPos_ );
	else	/// ������ ����� �������
	{
		if ( false == pSkillTemplet_->m_vecBuffFactorPtr.empty() )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

			CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet_->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );
			if ( NULL != ptrBuffFactorClone )
			{
				ptrBuffFactorClone->SetGameUnitBuffFactor( this );
				ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, fAddtionalAccelBuffTime_ );

				ptrEnergyOfConcentrationFactor->SetGameUnitBuffFactor( this );	/// ������ ��� �������� ����


				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

				ptrEnergyOfConcentrationFactor->ApplyExternalFactorToFinalizer( BFT_TIME, /// �׼��������ͽð� + fAddtionalAccelBuffTime ��ŭ ����
					pSkillTemplet_->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel )
					+ fAddtionalAccelBuffTime_ );

				const WCHAR* pwsCoreParticleName = CX2SkillTree::GetAccelCoreParticleName( pSkillTemplet_->m_eID );
				ASSERT( NULL != pwsCoreParticleName );

				for ( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
				{
					CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
					if( pGameUnit != NULL &&
						pGameUnit->GetTeam() == GetTeam() && pGameUnit->GetGameUnitType() == GUT_USER &&
						GetDistance3Sq( vMyPos_, pGameUnit->GetPos() ) < 490000.f &&
						pGameUnit->CanApplyBuffToGameUnit() )	/// �Ÿ� ����, ���� ��
					{
						if ( g_pX2Game->GetGameType() == CX2Game::GT_PVP || pGameUnit->GetUnitUID() == GetUnitUID() ||
							( NULL != g_pData && NULL != g_pData->GetPartyManager() && true == g_pData->GetPartyManager()->IsMyPartyMember( pGameUnit->GetUnitUID() ) ) )
						{
							if ( pGameUnit->SetBuffFactorToGameUnit( *ptrBuffFactorClone ) && /// �׼���������
								pGameUnit->SetBuffFactorToGameUnit( *ptrEnergyOfConcentrationFactor ) )	/// �����Ǳ��
							{
								D3DXVECTOR3 vGameUnitPos = pGameUnit->GetPos();
								vGameUnitPos.y += 100.0f;
								g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwsCoreParticleName, vGameUnitPos );
							}
						}
					}
				}	// for
			}
		}
	}
}

/** @function : GetEnergyOfConcentrationBuffFactorClonePtr
@brief : �����Ǳ��(�׸�) �нú� ��ų�� ������ �ִ� �������͸� ������ �Լ�
@param : ����� �׼��������ͽ�ų ID(eSkillIdAccelerator_)
@return : �����Ǳ���� ����� �ʾ����� �� ����Ʈ������ ����
*/
CX2BuffFactorPtr CX2GUEve::GetEnergyOfConcentrationBuffFactorClonePtr() const
{
	const int iSkillLevelEnergyOfConcentration = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION, true );

	if( iSkillLevelEnergyOfConcentration > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletEnergyOfConcentration
			= g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ENS_ENERGY_OF_CONCENTRATION );
		if ( NULL != pSkillTempletEnergyOfConcentration &&
			false == pSkillTempletEnergyOfConcentration->m_vecBuffFactorPtr.empty() )
			return pSkillTempletEnergyOfConcentration->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillLevelEnergyOfConcentration );
	}

	return CX2BuffFactorPtr();
}

/** @function : CX2GetActualMPConsume
@brief : ��ų�� MP �Ҹ��� ����
@param : ��ų�� ID, ����� ��ų�� ����
@return : �Ҹ�� MP ��ġ
*/
/*virtual*/ float CX2GUEve::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );

	switch ( eSkillID_ )
	{
	case CX2SkillTree::SI_SA_COMMON_AURA_CRITICAL_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_ADDATK_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SPEED_ACCEL:
		{
			const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA, true );

			if ( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA );
				if( NULL != pSkillTemplet )
				{
					const float fAuraMpConsumptionMultiplier
						= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevel );

					fMpConsumption *= fAuraMpConsumptionMultiplier;
				}
			}
		} break;

	case CX2SkillTree::SI_SA_EN_ILLUSION_STINGER:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO8 ) == true )
			{
				fMpConsumption -= 20.f;
			}
		} break;

		/// �������� �ñر� - ��ũ �������� �����Ʈ �ǴϽ� Ÿ�ݿ��� ���ǰ� �Ҹ���� ����.
	case CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION:
		{
			if( true == GetActiveLinkOverChargeIllusion() )
				fMpConsumption = 0.f;
		} break;
	case CX2SkillTree::SI_SA_EEG_JUNK_BREAK:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO18 ) == true )
				fMpConsumption -= 30.f;
		} break;

	case CX2SkillTree::SI_SA_EAT_GENOCIDE_RIPPER:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO19 ) == true )
				fMpConsumption *= 0.9f;
		} break;

	case CX2SkillTree::SI_SA_EEL_PHOTON_FLARE:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO23 ) == true )
				fMpConsumption -= 20.f;
		} break;

	case CX2SkillTree::SI_SA_EEL_GIGA_STREAM:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO25 ) == true )
				fMpConsumption *= 0.9f;
		} break;
	default:
		break;
	}

	if ( 0.0f > fMpConsumption )
		fMpConsumption = 0.f;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	float fMpDecreaseRate = 1.0f;
	if( NULL != pSkillTemplet )
		fMpDecreaseRate  =  g_pData->GetMyUser()->GetSelectUnit()->GetSkillMpDecreaseRate(eSkillID_, pSkillTemplet->m_eType);

	return fMpConsumption * fMpDecreaseRate;
}

/** @function : AddUnitSlashData
@brief : ���ֿ� Ʈ���̽� ���̱� (������ �ƴ�)
@param : Ʈ���̽� ����(eSlashTraceCondition_)
*/
/*virtual*/ void CX2GUEve::AddUnitSlashData( const CX2UnitSlashTraceManager::SLASH_TRACE_CONDITION eSlashTraceCondition_ )
{
	if( NULL == m_pUnitSlashTraceManager )
		m_pUnitSlashTraceManager = CX2UnitSlashTraceManager::CreateUnitSlashTraceManager( this, GetXSkinAnimPtr() );

	if( NULL != m_pUnitSlashTraceManager )
	{
		const int vertexNum = (IsMyUnit() ? 400 : 150);

		switch ( eSlashTraceCondition_ )
		{
		case CX2UnitSlashTraceManager::STC_NONE:
			{
				m_pUnitSlashTraceManager->AddSlashTraceData( L"Bip01_L_Finger2Nub", L"Bip01_L_Hand",
					D3DXCOLOR(0xcc5555ff), D3DXCOLOR(0xccff3333), D3DXCOLOR(0, 0, 0, 0), D3DXCOLOR(0, 0, 0, 0),
					vertexNum, 0.4f, 1,
					0, 0, 0,
					false, 0.f, L"" );


				m_pUnitSlashTraceManager->AddSlashTraceData( L"Bip01_R_Finger2Nub", L"Bip01_R_Hand",
					D3DXCOLOR(0xcc5555ff), D3DXCOLOR(0xccff3333), D3DXCOLOR(0, 0, 0, 0), D3DXCOLOR(0, 0, 0, 0),
					vertexNum, 0.4f, 1,
					0, 0, 0,
					false, 0.f, L"" );
			} break;

		default:
			break;
		}
	}
}
/*virtual*/ void CX2GUEve::ModifyFinalDamageByGameUnitType( OUT CX2DamageManager::DAMAGE_TYPE& damageType_, OUT float& fAttackerPhysicDamage_, OUT float& fAttackerMagicDamage_ , OUT float& fForceDownValue_ )
{
	if ( GetUnitClass() == CX2Unit::UC_EVE_CODE_NEMESIS && false == DidHitNasodWeapon() )
	{
		if ( NULL != GetUnit() )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet =
				GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_ENS_NASOD_WEAPON );

			if ( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

				if ( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillTempletLevel ) > GetRandomFloat() )
				{
					damageType_		= CX2DamageManager::DT_MIX;
					fAttackerMagicDamage_	+= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillTempletLevel );
					fAttackerPhysicDamage_	*= 2;
					fAttackerMagicDamage_	*= 2;
					SetHitNasodWeapon( true );
					fForceDownValue_ -= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_ABS, iSkillTempletLevel );
				}
			}
		}
	}
}

/** @function : SetSpecificValueByBuffTempletID
@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUEve::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_BUFF_AMPLIFICATION_PLACE:
		ChangePlaneForamtionSystem( PFT_AMPLIFIER );
		break;

	case BTI_BUFF_SPECTRUM_PLACE:
		ChangePlaneForamtionSystem( PFT_SPECTRUM );
		break;

	case BTI_BUFF_INDUCTION_PLACE:
		ChangePlaneForamtionSystem( PFT_INDUCTION );
		break;

	case BTI_BUFF_SI_SA_EBS_ENERGETIC_HEART:
		{

			const CX2SkillTree::SkillTemplet* pSkillTemplet
				= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EBS_ENERGETIC_HEART );

			if ( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				m_iAddBoostAttack = static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel ) );
			}

			m_EveElectraSystem.SetEnableSystem( true );
			m_EveElectraSystem.SetEnableExceptionSystem( true );
			m_EveElectraSystem.m_ePrevSpectro = m_EveElectraSystem.GetPlaneFormation();
			m_EveElectraSystem.SetPlaneFormation( PFT_FUSION );
			if ( IsMyUnit() )
				g_pX2Game->UpdateSkillSlotUI();
		} break;

	default:
		break;
	}
}

/** @function : UnSetSpecificValueByBuffTempletID
@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUEve::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_BUFF_AMPLIFICATION_PLACE:
	case BTI_BUFF_SPECTRUM_PLACE:
	case BTI_BUFF_INDUCTION_PLACE:
	case BTI_BUFF_SI_SA_EBS_ENERGETIC_HEART:
		{
			m_eReserveEndElCrystalBuffID = eBuffTempletId_;
			m_bIsCheckEndElCrystal = true;

			// ���� ���� ���� ���忡 �����ϴ� ��ų�� üũ�Ͽ�, ���� ���̶�� ���� ���� �ð��� �̷��
			if ( false == m_EveElectraSystem.m_EveElectraLaserDataList.empty() )
			{
				std::list< EveElectraLaserData >::iterator lit_Laser;
				for( lit_Laser = m_EveElectraSystem.m_EveElectraLaserDataList.begin(); lit_Laser != m_EveElectraSystem.m_EveElectraLaserDataList.end(); ++lit_Laser )
				{
					if( true == lit_Laser->m_bPenetrate )
					{
						m_bIsCheckEndElCrystal = false;
						break;
					}
				}
			}
		} break;

	default:
		break;
	}
}


/** @function : ApplyRenderParam
	@brief : ������ ���� renderparam�� �Բ� �߰������� �غ��ٽ��� ���� renderparam�� �����ϴ� �Լ�
	@param : SkinAnim�� RenderParam ������(pRenderParam_)

*/
/*virtual*/ void CX2GUEve::ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if ( m_bPassDash )
		return;

	if( true == m_bInvisibility )
	{
		if( 0.f == m_fInvisibilityAlpha )
		{
			// commonframemove���� ���� ���°� �ƴ϶�� setshowobject(false) �Ǽ� �ƿ� �������� ���� �ʴ´�.
			if( true == GetAbsoluteInvisibility() )
			{
				pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
				pRenderParam_->outLineColor.a	= m_fInvisibilityAlpha;
				m_RenderParam.color.a			= m_fInvisibilityAlpha;
			}
			else
			{
				pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
				pRenderParam_->outLineColor.a	= m_fInvisibilityAlpha;
				m_RenderParam.color.a			= m_fInvisibilityAlpha;
			}
		}
		else
		{
			pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
			pRenderParam_->outLineColor.a	= m_fInvisibilityAlpha;
			m_RenderParam.color.a			= m_fInvisibilityAlpha;
		}
	}
	else if( true == m_bBlinkInvisibility )
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
		pRenderParam_->outLineColor.a	= m_fBlinkInvisibilityAlpha;
		m_RenderParam.color.a			= m_fBlinkInvisibilityAlpha;
	}
	else
		CX2GameUnit::ApplyRenderParam( pRenderParam_ );
}

/*virtual*/ void CX2GUEve::ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if( GetRemainHyperModeTime() > 0.0f )
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_RED;
		pRenderParam_->outLineColor		= D3DXCOLOR( 1.f, 0.694f, 0.737f, 1.f );
	}
	else
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
		pRenderParam_->outLineColor		= 0xffffffff;
	}
}


/** @function : ChangePlaneForamtionSystem
	@brief : �����̼��� ���� �ϴ� �Լ�
	@param : ePlaneFormatinoSystem_ (���ڷ� �ѱ� �� �����̼��� �ƴ� ��� ����)
*/
void CX2GUEve::ChangePlaneForamtionSystem( const PlaneFormationType ePlaneFormatinoSystem_ )
{
	if ( true == IsMyUnit() && false == GetStartSpectroElCrystal() )		/// ���� ��ų�� ���� �������� �ʰ� ������ ���� ���� �Ǿ��� ��, �����ؾ� �� ���� ����
	{
		SetStartSpectroElCrystal( true );		/// �б������� ��ų�� ���� ���� �Ǿ�����, ������ ���� �Ǿ������� ���� ����

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL);

		if( NULL != pSkillTemplet && NULL != GetUnit() )
		{
			CX2UserSkillTree& cUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;

			float fAdditionalTime = 0.f;	/// �б����� ����� ����� �߰� ��Ÿ��

			/// �޸� �ִ� ��� 3�� ����
			if ( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO13) )
				fAdditionalTime = 3.0f;

			const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			/// �б����� ������ ���� �� ��Ÿ�� ������ ���� ���ӽð� ����
			m_EveElectraSystem.SetSystemOperationTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) + fAdditionalTime );

			cUserSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL, 0.f  );		/// �б� ���� ���� ���̴�, ��Ÿ�� ����
		}

		m_EveElectraSystem.SetPlaneFormation( ePlaneFormatinoSystem_ );	/// �����ų �б����� Ÿ������ ����
	}

	m_EveElectraSystem.SetEnableSystem( true );
	if ( ePlaneFormatinoSystem_ != m_EveElectraSystem.GetPlaneFormation() )
		m_EveElectraSystem.ChangePlaneFormation();
	ChangeThousandsOfStarsFormation();
}

/** @function : StartElCrystalSystem
@brief : �б����� ��ų ���� ����Ǵ� �Լ��� �б������� Enable �ϰų�, ������ �����Ѵ�.
*/
void CX2GUEve::StartElCrystalSystem()
{
	SetStartSpectroElCrystal( true );		/// �б������� ��ų�� ���� ���� �Ǿ�����, ������ ���� �Ǿ������� ���� ����
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL);

	if( NULL != pSkillTemplet )
	{
	    const CX2UserSkillTree& cUserSkillTree =  GetUnit()->GetUnitData().m_UserSkillTree;

		if ( NULL == GetUnit() )
			return;

		const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		/// �б������� ��� ���̰�, �б����� ��� �� �� ������
		if( m_EveElectraSystem.GetEnableSystem() == true &&
			m_EveElectraSystem.GetEnableSpectrumDynamics() == true )
		{
			switch ( m_EveElectraSystem.GetPlaneFormation() )
			{
			case PFT_AMPLIFIER:	/// ������ �б������� �������� ���
				{
					m_EveElectraSystem.ChangePlaneFormation();

					vector<CX2BuffFinalizerTempletPtr>	vecFinalizerPtr;
					GetBuffFinalizerTempletPtrList( BTI_BUFF_AMPLIFICATION_PLACE, vecFinalizerPtr );

					ASSERT( !vecFinalizerPtr.empty() );
					if ( !vecFinalizerPtr.empty() )
					{
						EraseBuffTempletFromGameUnit( BTI_BUFF_AMPLIFICATION_PLACE );	/// ������ ���� ����

						SetBuffFactorToGameUnit( pSkillTemplet, 1 );	/// �б��� ���� ����
						ChangeBuffFinalizerTempletPtrList( BTI_BUFF_SPECTRUM_PLACE, vecFinalizerPtr );
					}
				} break;

			case PFT_SPECTRUM:	/// ������ �б������� �б����� ���
				{
					m_EveElectraSystem.ChangePlaneFormation();

					vector<CX2BuffFinalizerTempletPtr>	vecFinalizerPtr;
					GetBuffFinalizerTempletPtrList( BTI_BUFF_SPECTRUM_PLACE, vecFinalizerPtr );

					ASSERT( !vecFinalizerPtr.empty() );
					if ( !vecFinalizerPtr.empty() )
					{
						EraseBuffTempletFromGameUnit( BTI_BUFF_SPECTRUM_PLACE );	/// �б��� ���� ����
						if ( m_EveElectraSystem.GetEnableHyperOpticalResearch() )	/// �������� �ִ� ���
						{
							SetBuffFactorToGameUnit( pSkillTemplet, 2 );	/// ������ ����
							ChangeBuffFinalizerTempletPtrList( BTI_BUFF_INDUCTION_PLACE, vecFinalizerPtr );
						}
						else	/// �������� ����� ���� ���
						{
							SetBuffFactorToGameUnit( pSkillTemplet, 0 );	/// ������ ���� ����
							ChangeBuffFinalizerTempletPtrList( BTI_BUFF_AMPLIFICATION_PLACE, vecFinalizerPtr );
						}
					}
				} break;

			case PFT_INDUCTION:	/// �������� ���
				{
					m_EveElectraSystem.ChangePlaneFormation();

					vector<CX2BuffFinalizerTempletPtr>	vecFinalizerPtr;
					GetBuffFinalizerTempletPtrList( BTI_BUFF_INDUCTION_PLACE, vecFinalizerPtr );

					ASSERT( !vecFinalizerPtr.empty() );
					if ( !vecFinalizerPtr.empty() )
					{
						EraseBuffTempletFromGameUnit( BTI_BUFF_INDUCTION_PLACE );	/// ������ ���� ����
						SetBuffFactorToGameUnit( pSkillTemplet, 0 );	/// ������ ���� ����
						ChangeBuffFinalizerTempletPtrList( BTI_BUFF_AMPLIFICATION_PLACE, vecFinalizerPtr );
					}
				} break;

			default:
				ASSERT( !L"This PlaneFormation doesn't exist" );
				break;
			}

			if ( IsMyUnit() )
				g_pX2Game->UpdateSkillSlotUI();
		}
		else if ( !m_EveElectraSystem.GetEnableSystem() )	/// �б������� ��������� ������
		{
			if ( CanApplyBuffToGameUnit() && false == pSkillTemplet->m_vecBuffFactorPtr.empty() )
			{
				CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );
				if ( NULL != ptrBuffFactorClone )
				{
					ptrBuffFactorClone->SetGameUnitBuffFactor( this );	/// ������������ �������� ����

					/// �޸� �ִ� ��� 3�� ����
					if ( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO13) )
					{
						const float fAdditionalTime = 3.0f;
						ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, fAdditionalTime );

						if ( m_fAddSpectrumTime > 0.f )
							ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, m_fAddSpectrumTime );

						/// �б����� ������ ���� �� CoolTime ������ ���� ���ӽð� ����
						m_EveElectraSystem.SetSystemOperationTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) + fAdditionalTime + m_fAddSpectrumTime );
					}
					else
					{
						if ( m_fAddSpectrumTime > 0.f )
							ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, m_fAddSpectrumTime );

						/// �б����� ������ ���� �� CoolTime ������ ���� ���ӽð� ����
						m_EveElectraSystem.SetSystemOperationTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) + m_fAddSpectrumTime );
					}

					m_EveElectraSystem.SetPlaneFormation( PFT_AMPLIFIER );
					SetBuffFactorToGameUnit( *ptrBuffFactorClone );	/// ���� ����

					if ( IsMyUnit() )
						g_pX2Game->UpdateSkillSlotUI();

					if ( true == m_EveElectraSystem.GetEnableHyperOpticalResearch() )
						const_cast<CX2SkillTree::SkillTemplet*>(pSkillTemplet)->m_eActiveSkillUseCondtion = CX2SkillTree::ASUT_ANYTIME;
				}
			}
		}
	}
}

/** @function : EndElCrystalSystem
@brief : �б������� ������ �� ����Ǵ� �Լ�
*/
void CX2GUEve::EndElCrystalSystem( bool bEnergeticHeartEnd_/* = false */ )
{
	/// �б������� ������ ������彺Ÿ�� ��ƼŬ�� ������� �ǵ���
	if( CX2DamageEffect::CEffect* pTSEffect = ( m_pThousandStar != NULL ) ? m_pThousandStar->GetLiveDamageEffect() : NULL )
	{
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleThousandStarFormation );
		m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01", pTSEffect->GetPos() );
	}

	m_EveElectraSystem.DestroyFormationMarkAndMsgParticle();
	m_EveElectraSystem.DestroyPlaneAndCrystalEffect();

	m_EveElectraSystem.SetPlaneFormation( PFT_AMPLIFIER );	/// �ʱ�ȭ
	m_EveElectraSystem.SetEnableSystem( false );

	if ( IsMyUnit() )
	{
		/// �б������� ���ӽð��� ���� �� ���� ��Ÿ�� ǥ�ð� �Ǵµ�,
		/// �б������� ������ ��, ��Ÿ��-���ӽð� ��ŭ ǥ�� ����(ex: ��Ÿ�� 30��, ���ӽð� 25�� �� �б������� ������ ��Ÿ�� 5�ʷ� ����)
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL);
		if( NULL != pSkillTemplet )
		{
			if ( true == m_EveElectraSystem.GetEnableHyperOpticalResearch() )
				const_cast<CX2SkillTree::SkillTemplet*>(pSkillTemplet)->m_eActiveSkillUseCondtion = CX2SkillTree::ASUT_GROUND;
			if( bEnergeticHeartEnd_ == false )
			{
				if ( NULL == GetUnit() )
					return;

		        CX2UserSkillTree& cUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				float fRemainCoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - m_EveElectraSystem.GetSystemOperationTime();

				if( fRemainCoolTime < 0.f )
					fRemainCoolTime = 0.f;

				cUserSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL, fRemainCoolTime  );
			}
		}

		g_pX2Game->UpdateSkillSlotUI();
	}

	m_EveElectraSystem.SetSystemOperationTime( 0.0f );

	SetStartSpectroElCrystal( false );		/// �б������� ��ų�� ���� ���� �Ǿ�����, ������ ���� �Ǿ������� ���� ����

}

/** @function : ChangeThousandsOfStarsFormation
@brief : ������彺Ÿ�� �������� �� �б������� ���۵ǰų� ������ ����Ǹ� ������� ��Ÿ�� ����Ʈ�� ����ǰ� �ϴ� �Լ�
*/
void CX2GUEve::ChangeThousandsOfStarsFormation()
{
    if ( CX2DamageEffect::CEffect* pTSEffect = ( m_pThousandStar != NULL ) ? m_pThousandStar->GetLiveDamageEffect() : NULL )
	{
		g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleThousandStarFormation );

		if( GetEnableElectraSystem() == false )
		{
			m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01", pTSEffect->GetPos() );
		}
		else if( m_EveElectraSystem.GetPlaneFormation() == PFT_AMPLIFIER )
		{
			m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01_2_red", pTSEffect->GetPos() );
		}
		else if( m_EveElectraSystem.GetPlaneFormation() == PFT_SPECTRUM )
		{
			m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01_3_blue",pTSEffect->GetPos() );
		}
		else if( m_EveElectraSystem.GetPlaneFormation() == PFT_INDUCTION )
		{
			m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle(
				(CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01_4_green", pTSEffect->GetPos() );
		}
		else if( m_EveElectraSystem.GetPlaneFormation() == PFT_FUSION )
		{
			m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle(
				(CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01_5_white", pTSEffect->GetPos() );
		}
	}
}
//#pragma region ������ �ʶ�
bool CX2GUEve::IsUpSkillSlotTaserPilum()
{
	bool bIsTaserPilumSuccess = false;
	if( m_sTaserPilumData.m_bEnable == true )
	{
		//Ű�� ���ȴ��� üũ.
		switch( m_sTaserPilumData.GetSkillSlotID() )
		{
		case 0: if( m_InputData.pureA == false && m_InputData.oneA == false )
					bIsTaserPilumSuccess = true;
			break;
		case 1: if( m_InputData.pureS == false && m_InputData.oneS == false )
					bIsTaserPilumSuccess = true;
			break;
		case 2: if( m_InputData.pureD == false && m_InputData.oneD == false )
					bIsTaserPilumSuccess = true;
			break;
		case 3: if( m_InputData.pureC == false && m_InputData.oneC == false )
					bIsTaserPilumSuccess = true;
			break;
		case 4: if( m_InputData.pureQ == false && m_InputData.oneQ == false )
					bIsTaserPilumSuccess = true;
			break;
		case 5: if( m_InputData.pureW == false && m_InputData.oneW == false )
					bIsTaserPilumSuccess = true;
			break;
		case 6: if( m_InputData.pureE == false && m_InputData.oneE == false )
					bIsTaserPilumSuccess = true;
			break;
		case 7: if( m_InputData.pureR == false && m_InputData.oneR == false )
					bIsTaserPilumSuccess = true;
			break;

		default: // �����ÿ� ������ ������ ����� �̷�� ���� �ʾ��� ���.
			m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
			break;;
		}
	}

	return bIsTaserPilumSuccess;
}


const CX2UserSkillTree::SkillSlotData* CX2GUEve::GetSkillSlotDataTaserPilum( IN const CX2UserSkillTree& cUserSkillTree_, OUT int& iSkillSlotIndexPressed_ )
{
	//Űó������ ������ ������ �˻�

	int iSlotIndex = (m_sTaserPilumData.GetSkillSlotID() > 3) ? m_sTaserPilumData.GetSkillSlotID()-4 : m_sTaserPilumData.GetSkillSlotID();
	bool bSlotB = (m_sTaserPilumData.GetSkillSlotID() > 3) ? true : false;


	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = cUserSkillTree_.GetSkillSlot( iSlotIndex, bSlotB );

	if( CX2SkillTree::SI_NONE == pSkillSlotData->m_eID ||
		CX2SkillTree::SI_A_EBS_TASER_PILUM != pSkillSlotData->m_eID)
	{
		m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
		return NULL;
	}


	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillSlotData->m_eID );
	if( NULL == pSkillTemplet )
	{
		m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
		return NULL;
	}

	// ������ �ʶ� ���� ��� �Ϸ�
	m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
	iSkillSlotIndexPressed_ = m_sTaserPilumData.GetSkillSlotID();

	return pSkillSlotData;
}

void CX2GUEve::TASER_PILUM_Data::Reset(bool bIsChargeTrue /* = true */, FrameData* pFrameDataFuture /* = NULL  */)
{
	if( bIsChargeTrue == false && pFrameDataFuture != NULL )
	{
		pFrameDataFuture->syncData.nowAction = UAI_TASER_PILUM_CHARGE_FAIL;
	}

	m_bEnable		= false;
	m_bIsSucCharge	= false;

	ResetParticle();

	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_FullCharge.ogg" );
	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
}


void CX2GUEve::TASER_PILUM_Data::ResetParticle()
{
	for( int i = 0; i < std::ssize( m_ahMajorParticleInstance ); ++i )
	{
		if( m_ahMajorParticleInstance[i] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_ahMajorParticleInstance[i] );
		m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

void CX2GUEve::ResetTaserPilumCharge()
{
	if( m_sTaserPilumData.m_bEnable == true )
		m_sTaserPilumData.Reset( false, &AccessFutureFrameData() );
}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_Init()
{
}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed.x = 0.f;
	m_PhysicParam.nowSpeed.y = 0.f;
}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_Start()
{
	CommonStateStart();

	// ���� �ð��� å�� ������ũ ����.
	// 1. �Ҹ� MP
	// 2. ������ ����
	// 3. ũ�� ����
	// �� �����.
	if( (m_sTaserPilumData.m_dMaxTime * 0.5) >= m_sTaserPilumData.m_timerElapsedTime.elapsed() )
		m_sTaserPilumData.m_ChargeRank = CR_RANK_1;
	else if( m_sTaserPilumData.m_dMaxTime >= m_sTaserPilumData.m_timerElapsedTime.elapsed() )
		m_sTaserPilumData.m_ChargeRank = CR_RANK_2;
	else if( m_sTaserPilumData.m_dMaxTime < m_sTaserPilumData.m_timerElapsedTime.elapsed() )
		m_sTaserPilumData.m_ChargeRank = CR_RANK_3;
	//�ִ� ����ó��
	else
		m_sTaserPilumData.m_ChargeRank = CR_RANK_1;

	m_fPlaneZAngle	= 0.f;

	m_sTaserPilumTinyData.reset();
	m_sTaserPilumTinyData.m_timerElapsedTime = m_sTaserPilumData.m_timerElapsedTime;
	m_sTaserPilumTinyData.m_ChargeRank		 = m_sTaserPilumData.m_ChargeRank;

}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.66f )
	{
		m_bDisableGravity = false;
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.406f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.0f, 240.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.416f ) )
	{
		// MP �Ҹ�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EBS_TASER_PILUM );

		float fMPConsumption = 0.f;
		float fPowerRate = 1.f;
		int iCount = 0;
		wstring wstrDamageEffect = L"";

		if( NULL != pSkillTemplet )
		{
			switch( m_sTaserPilumTinyData.m_ChargeRank )
			{
			case CR_RANK_1:
				{
					wstrDamageEffect	= L"EVE_TASER_PILUM_CHARGE_0";
				}
				break;
			case CR_RANK_2:
				{
					wstrDamageEffect	= L"EVE_TASER_PILUM_CHARGE_1";
				}
				break;
			case CR_RANK_3:
				{
					wstrDamageEffect	= L"EVE_TASER_PILUM_CHARGE_2";
				}
				break;
			}

			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			fMPConsumption	= pSkillTemplet->GetSkillMPConsumptionValue( iSkillTempletLevel );
			fPowerRate		= static_cast<float>( GetPowerRate() * ( min( static_cast<float>( m_sTaserPilumTinyData.m_timerElapsedTime.elapsed() ), 3.f) / 3.f + 1.f ) );
			iCount			= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_PIERCING, iSkillTempletLevel ) *
				min( static_cast<float>( m_sTaserPilumTinyData.m_timerElapsedTime.elapsed() ), 3.f) / 3.f );
			m_sTaserPilumTinyData.m_iAddDamageTime = iCount;
		}
		else
		{
			fMPConsumption = 20.f;
		}

		if( FlushMp( fMPConsumption ) == true )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Hand" );

			if( true == GetIsRight() )
			{
				vBonePos += GetDirVector() * 70.f;
			}
			else
			{
				vBonePos -= GetDirVector() * 70.f;
			}

			CKTDGParticleSystem::CParticleEventSequence* pSeq =
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this, L"EBS_A_TASER_PILUM_fire_P01", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}

			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EBS_A_TASER_PILUM_fire_P02", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}

			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EBS_A_TASER_PILUM_fire_P03", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}

			CX2DamageEffect::CEffect* pEffect = NULL;

			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffect.c_str(), fPowerRate, vBonePos,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, 1.f );

			pEffect->SetDamageTime( pEffect->GetDamageTime() + iCount );
			pEffect->GetDamageData().bReAttack = false;

			if( pEffect != NULL )
				pEffect->SetInheritForceDownRate(true);

			if( NULL != pEffect )
			{
				wstrDamageEffect += L"_FILTERED";

				m_EveElectraSystem.AddEffectToSystem( pEffect, wstrDamageEffect.c_str(), wstrDamageEffect.c_str(),
					wstrDamageEffect.c_str(), wstrDamageEffect.c_str());
			}

			PlaySound( L"Eve_FireShot1.ogg" );
		}
		else
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		m_bDisableGravity = false;
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EBS_TASER_PILUM_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}
//#pragma endregion

//#pragma region �ν�Ʈ �뽬 ��ŸƮ
void CX2GUEve::ENSI_EBS_BOOST_DASH_START_StartFuture()
{
	m_PhysicParam.nowSpeed.x = 0.0f;

	CommonStateStartFuture();

	if ( false == GetAbsoluteInvisibility() )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance(
			(CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ), GetRotateDegree(), GetRotateDegree() );
		//m_hBoostDashMesh = pMeshInstPlane->GetHandle();
	}
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_START_EndFuture()
{
	m_PhysicParam.nowSpeed.x = GetDashSpeed();

	CommonStateEndFuture();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_START_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.3f )
	{
		StateChange( ENSI_EBS_BOOST_DASH );
	}

	if( false == IsOnSomethingFuture() )
	{
		if( m_iDashJumpCount < 2 + m_iPhotonBoosterAdditionalDashJumpCount && false == m_InputData.pureDown &&
			( m_InputData.pureLeft == true || m_InputData.pureRight == true ) )
		{
			if ( true == m_InputData.pureRight )
			{
				m_eBoostDashState = BDS_RIGHT;
				m_bEnableBoostDash = true;
			}
			else if ( true == m_InputData.pureLeft )
			{
				m_eBoostDashState = BDS_LEFT;
				m_bEnableBoostDash = true;
			}

			StateChangeJumpDashIfPossible();
		}
		else
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_DASH_COMBO_X );
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_InputData.oneUp == true && m_iDashJumpUpCount  < 1   )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = m_PhysicParam.GetDashJumpSpeed();
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )

	else if( m_InputData.pureRight == false && m_InputData.pureLeft == false )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region Boost Dash
void CX2GUEve::ENSI_EBS_BOOST_DASH_StartFuture()
{
	CommonStateStartFuture();

	m_iAccumulateAccel = 0;
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_Start()
{
	CommonStateStart();

	m_hBoostDashParticle = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"P_EBS_BoostDash_P01",
		GetBonePos( L"Bip01" ) );
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_FrameMove()
{
	CommonFrameMove();

	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hBoostDashParticle );
	if( NULL != pSeq )
	{
		if ( false == GetAbsoluteInvisibility() )
		{
			D3DXVECTOR3 vPos = GetBonePos( L"Bip01" );

			pSeq->SetPosition( vPos );
			pSeq->SetShowObject( true );
		}
		else
		{
			pSeq->SetShowObject( false );
		}
	}
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_FrameMoveFuture()
{
	// ���� �Ǵ� ������ ����Ű�� �ι� ������ ���� ���� ���¸�
	if( ( true == m_InputData.pureRight && true == m_BeforeInputData.pureRight ) ||
		( true == m_InputData.pureLeft  && true == m_BeforeInputData.pureLeft  ) )
	{
		switch ( m_iAccumulateAccel )
		{
		case 1:
			m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * ( m_fHighPoweredElectricsSpeed + 0.1f );
			break;

		case 2:
			m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * ( m_fHighPoweredElectricsSpeed + 0.2f );
			break;

		case 3:
			m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * ( m_fHighPoweredElectricsSpeed + 0.3f );
			break;

		default:
			m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * m_fHighPoweredElectricsSpeed;
			break;
		}
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if( m_iDashJumpCount < 2 + m_iPhotonBoosterAdditionalDashJumpCount && false == m_InputData.pureDown &&
			( m_InputData.pureLeft == true || m_InputData.pureRight == true ) )
		{
			if ( true == m_InputData.pureRight )
			{
				m_eBoostDashState = BDS_RIGHT;
				m_bEnableBoostDash = true;
			}
			else if ( true == m_InputData.pureLeft )
			{
				m_eBoostDashState = BDS_LEFT;
				m_bEnableBoostDash = true;
			}

			StateChangeJumpDashIfPossible();
		}
		else
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ENSI_DASH_COMBO_X );
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_InputData.oneUp == true && m_iDashJumpUpCount  < 1   )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = m_PhysicParam.GetDashJumpSpeed();
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	else if( m_InputData.pureRight == false && m_InputData.pureLeft == false )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_DASH_END );
	}
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
		LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	if ( 1.0f <= m_FrameDataFuture.unitCondition.fStateTime && 0 == m_iAccumulateAccel )
	{
		++m_iAccumulateAccel;

		if ( false == GetAbsoluteInvisibility() )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance(
				(CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ), GetRotateDegree(), GetRotateDegree() );
			//m_hBoostDashMesh = pMeshInstPlane->GetHandle();
			PlaySound( L"Eve_BoostDash4.ogg" );
		}
	}
	else if ( 2.f <= m_FrameDataFuture.unitCondition.fStateTime && 1 == m_iAccumulateAccel )
	{
		++m_iAccumulateAccel;

		if ( false == GetAbsoluteInvisibility() )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance(
				(CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ), GetRotateDegree(), GetRotateDegree() );
			//m_hBoostDashMesh = pMeshInstPlane->GetHandle();
			PlaySound( L"Eve_BoostDash4.ogg" );
		}
	}
	else if ( 3.f <= m_FrameDataFuture.unitCondition.fStateTime && 2 == m_iAccumulateAccel )
	{
		++m_iAccumulateAccel;

		if ( false == GetAbsoluteInvisibility() )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance(
				(CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ), GetRotateDegree(), GetRotateDegree() );
			//m_hBoostDashMesh = pMeshInstPlane->GetHandle();
			PlaySound( L"Eve_BoostDash4.ogg" );
		}
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_End()
{
	CommonStateEnd();

	m_PhysicParam.nowSpeed.x = GetDashSpeed();

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hBoostDashParticle );
}
//#pragma endregion �ν�Ʈ �뽬

//#pragma region �ν�Ʈ �뽬 ���� ��ŸƮ
void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_START_StartFuture()
{
	CommonStateStartFuture();

	m_bDownForce		= false;

	m_bDisableGravity	= true;

	++m_iDashJumpCount;
	++m_iJumpFlyCount;

	m_bAfterJumpDash = true;

	m_fCurrDashJumpTime	= 0.4f;

	m_PhysicParam.nowSpeed.x = GetDashSpeed() + GetDashSpeed() * m_fHighPoweredElectricsSpeed;
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_START_Start()
{
	CommonStateStart();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_START_FrameMoveFuture()
{
	m_fCurrDashJumpTime -= m_fElapsedTime;

	const float fValue = m_fCurrDashJumpTime / 0.4f ;
	const float NOW_FLY_SPEED_SCALE = ( s_fDashJumpSpeedScale + m_fHighPoweredElectricsSpeed ) * powf( fValue, 2 ) + s_fDashJumpSpeedLowerBound;

	if( m_fCurrDashJumpTime < 0.f )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureDown == false &&
		m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x	= GetDashSpeed() * NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y	= 0.f;
	}
	else if( m_InputData.pureDown == false &&
		m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x	= GetDashSpeed() * NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y	= 0.f;
	}
	else
	{
		StateChange( USI_JUMP_DOWN );
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_START_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.3f )
	{
		StateChange( ENSI_EBS_BOOST_DASH_AIR );
	}

	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( USI_DASH_JUMP );
			m_PhysicParam.nowSpeed.y = m_PhysicParam.GetDashJumpSpeed();
			m_bDownForce = false;
		}
		else if( m_bDownForce == true )
		{
			StateChange( ENSI_DASH_JUMP_POWER_LANDING );
			m_bDownForce = false;
		}
		else
		{
			if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
			}
			m_bDownForce = false;
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( m_iDashJumpUpCount < 1 && m_InputData.pureDown == false && m_InputData.pureUp == true )
	{
		StateChange( USI_DASH_JUMP );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_Z );
	}
	else if( m_InputData.oneX == true && m_fReDashJumpXTime <= 0.0f )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_X );
	}
	else if( m_InputData.pureRight == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == false )
		{
			if( m_PhysicParam.nowSpeed.y > m_PhysicParam.GetJumpSpeed() )
				m_PhysicParam.nowSpeed.y = m_PhysicParam.GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}
	else if( m_InputData.pureLeft == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			if( m_PhysicParam.nowSpeed.y > m_PhysicParam.GetJumpSpeed() )
				m_PhysicParam.nowSpeed.y = m_PhysicParam.GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}

	if( m_InputData.oneUp == true && m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( m_InputData.pureDown == true )
	{
		m_bDownForce = true;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_START_EndFuture()
{
	CommonStateEndFuture();

	if( m_PhysicParam.nowSpeed.x > GetDashSpeed() )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	if ( false == GetAbsoluteInvisibility() )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstPlane = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance(
			(CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ), GetRotateDegree(), GetRotateDegree() );
		//m_hBoostDashMesh = pMeshInstPlane->GetHandle();
	}

	m_bDisableGravity = false;
}
//#pragma endregion

//#pragma region �ν�Ʈ �뽬 ����
void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_StartFuture()
{
	CommonStateStartFuture();

	m_bDownForce		= false;
	m_bDisableGravity	= true;

	m_fCurrDashJumpTime	= s_fMaxDashJumpTime;
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_Start()
{
	CommonStateStart();

	m_hBoostDashParticle = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"P_EBS_BoostDash_P01",
		GetBonePos( L"Bip01" ) );
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_FrameMove()
{
	CommonFrameMove();

	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hBoostDashParticle );
	if( NULL != pSeq )
	{
		if ( false == GetAbsoluteInvisibility() )
		{
			D3DXVECTOR3 vPos = GetBonePos( L"Bip01" );

			pSeq->SetPosition( vPos );
			pSeq->SetShowObject( true );
		}
		else
		{
			pSeq->SetShowObject( false );
		}
	}
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_FrameMoveFuture()
{
	m_fCurrDashJumpTime -= m_fElapsedTime;

	const float fValue = m_fCurrDashJumpTime / s_fMaxDashJumpTime ;
	const float NOW_FLY_SPEED_SCALE = ( s_fDashJumpSpeedScale + m_fHighPoweredElectricsSpeed ) * powf( fValue, 2 ) + s_fDashJumpSpeedLowerBound;

	if( m_fCurrDashJumpTime < 0.f )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureDown == false &&
		m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x	= GetDashSpeed() * NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y	= 0.f;
	}
	else if( m_InputData.pureDown == false &&
		m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x	= GetDashSpeed() * NOW_FLY_SPEED_SCALE;
		m_PhysicParam.nowSpeed.y	= 0.f;
	}
	else
	{
		StateChange( USI_JUMP_DOWN );
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( USI_DASH_JUMP );
			m_PhysicParam.nowSpeed.y = m_PhysicParam.GetDashJumpSpeed();
			m_bDownForce = false;
		}
		else if( m_bDownForce == true )
		{
			StateChange( ENSI_DASH_JUMP_POWER_LANDING );
			m_bDownForce = false;
		}
		else
		{
			if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_JUMP_LANDING );		// not dash_jump_landing
			}
			m_bDownForce = false;
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ENSI_WALL_LANDING );
	}
	else if( m_iDashJumpUpCount < 1 && m_InputData.pureDown == false && m_InputData.pureUp == true )
	{
		StateChange( USI_DASH_JUMP );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ENSI_DASH_JUMP_COMBO_Z );
	}
	else if( m_InputData.oneX == true && m_fReDashJumpXTime <= 0.0f )
	{
		m_iBoostAttackCount = 0;
		m_bBoostUpAttack = false;

		StateChangeBoostAttackIfPossible();
	}
	else if( m_InputData.pureRight == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == false )
		{
			if( m_PhysicParam.nowSpeed.y > m_PhysicParam.GetJumpSpeed() )
				m_PhysicParam.nowSpeed.y = m_PhysicParam.GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}
	else if( m_InputData.pureLeft == true )
	{
		if( m_FrameDataFuture.syncData.bIsRight == true )
		{
			if( m_PhysicParam.nowSpeed.y > m_PhysicParam.GetJumpSpeed() )
				m_PhysicParam.nowSpeed.y = m_PhysicParam.GetJumpSpeed();

			if( m_PhysicParam.nowSpeed.y > 0.0f )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}

	if( m_InputData.oneUp == true && m_PhysicParam.nowSpeed.y <= 0.0f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( m_InputData.pureDown == true )
	{
		m_bDownForce = true;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_EndFuture()
{
	CommonStateEndFuture();

	if( m_PhysicParam.nowSpeed.x > GetDashSpeed() )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	m_bDisableGravity = false;
}

void CX2GUEve::ENSI_EBS_BOOST_DASH_AIR_End()
{
	CommonStateEnd();

	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hBoostDashParticle );
}
//#pragma endregion

//#pragma region �뽬 ���� �޺� X(Push)Z
void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_XZ_Start()
{
	CommonStateStart();

	SetNoDetonation( true );

	m_bDisableGravity = true;
	m_bDashJumpXXLaserEnd = false;
	m_bExceptionPlane = true;
	m_fExceptioncPlaneZAngle = -45.f;
	m_bNotEnoughMPDashJumpXZLaser = false;
	m_fPlaneZAngle = 0.f;
    m_hEffectElectraLaserDashJumpXZ = INVALID_DAMAGE_EFFECT_HANDLE;

	m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
	m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;

	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

	m_EveElectraSystem.m_fEveZAngle = 70.f;
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_XZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle + m_fExceptioncPlaneZAngle, 1.8f, 150.f, -50.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.333f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle + m_fExceptioncPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.533f ) )
	{
		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );

		if( GetIsRight() == false )
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (130.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_DASHJUMP_COMBO_XZ_LASER",
			GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        m_hEffectElectraLaserDashJumpXZ = pEffect->GetHandle();

        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect())
		{
			D3DXVECTOR3 vRotateLocalDegree = pMeshInstance->GetRotateLocalDegree();

			vRotateLocalDegree.z += m_fExceptioncPlaneZAngle;

			pMeshInstance->SetRotateLocalDegree( vRotateLocalDegree );

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Finger2Nub", vDamageEffectPos, vZeroYDirVector, 6000.f, 0.108f, 0.5f,
				L"EVE_DASHJUMP_COMBO_XZ_LASER_NEXT", L"EVE_DASHJUMP_COMBO_XZ_LASER_NEXT", L"EVE_DASHJUMP_COMBO_XZ_LASER_NEXT", true, 2.f );
		}
	}

    if( CX2DamageEffect::CEffect* pEffectElectraLaserDashJumpXZ = g_pX2Game->GetDamageEffect()->GetInstance( m_hEffectElectraLaserDashJumpXZ ) )
	{
        if ( CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffectElectraLaserDashJumpXZ->GetMainEffect() )
		{
			if( m_pXSkinAnim->GetNowAnimationTime() >= 0.733f && m_pXSkinAnim->GetNowAnimationTime() < 1.233f )
			{
				D3DXVECTOR3 vRotateLocalDegree = pMeshInstance->GetRotateLocalDegree();
				vRotateLocalDegree.z += ( 70.904f * m_fElapsedTime );
				pMeshInstance->SetRotateLocalDegree( vRotateLocalDegree );
				m_EveElectraSystem.m_fLaserEndOffsetY = 2.f;
			}
			else if ( m_pXSkinAnim->GetNowAnimationTime() >= 1.233f && m_pXSkinAnim->GetNowAnimationTime() < 1.733f )
			{
				D3DXVECTOR3 vRotateLocalDegree = pMeshInstance->GetRotateLocalDegree();
				vRotateLocalDegree.z += ( 0.904f * m_fElapsedTime );
				pMeshInstance->SetRotateLocalDegree( vRotateLocalDegree );
			}
			else
			{
				m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;
			}
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.833f ) )
	{
		m_bDashJumpXXLaserEnd = true;
		m_bExceptionPlane = false;
		m_EveElectraSystem.m_fEveZAngle = 0.f;
		m_fExceptioncPlaneZAngle = 0.f;
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EVE_COMBO_XXZ_END", this );
		m_EveElectraSystem.DeleteLaserFromSystem();
        m_hEffectElectraLaserDashJumpXZ = INVALID_DAMAGE_EFFECT_HANDLE;
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.600f && m_pXSkinAnim->GetNowAnimationTime() < 1.866f )
	{
		if( m_bNotEnoughMPDashJumpXZLaser == false && FlushMp( 13.3333f * m_fElapsedTime ) == false )
		{
			m_EveElectraSystem.DeleteLaserFromSystem();
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger2Nub" );
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
			m_bNotEnoughMPDashJumpXZLaser = true;
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_XZ_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.3f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

	if( m_InputData.oneZ == true && m_FrameDataFuture.stateParam.bEventFlagList[0] == false )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}

	if( m_InputData.pureZ == false )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			m_fCanNotAttackTime = 0.1f;
			m_bDisableGravity = false;
			m_PhysicParam.nowSpeed.y = -0.0001f;

			StateChange( ENSI_JUMP_FLY_DOWN );
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fCanNotAttackTime = 0.1f;
		m_bDisableGravity = false;

		m_PhysicParam.nowSpeed.y = -0.0001f;

		StateChange( ENSI_JUMP_FLY_DOWN );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_XZ_EndFuture()
{
	m_bDisableGravity = false;

	CommonStateEndFuture();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_XZ_End()
{
	m_EveElectraSystem.DeleteLaserFromSystem();

    m_hEffectElectraLaserDashJumpXZ = INVALID_DAMAGE_EFFECT_HANDLE;

	m_EveElectraSystem.m_fLaserEndOffsetX = 0.f;
	m_EveElectraSystem.m_fLaserEndOffsetY = 0.f;

	m_bExceptionPlane = false;
	m_fExceptioncPlaneZAngle = 0.f;
	m_EveElectraSystem.m_fEveZAngle = 0.f;

	m_bDisableGravity = false;

	SetNoDetonation( false );

	CommonStateEnd();
}
//#pragma endregion

//#pragma region �̺� �ν�Ʈ ���� ����
void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_Start()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_bNotEnoughMPDashZX = false;
	++m_iBoostAttackCount;

    [[maybe_unused]] CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
	pMeshInst  = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ),
		GetRotateDegree(), GetRotateDegree() );

	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Seraph_Boost_Dash_Attack", this );

	m_fBlinkInvisibilityAlpha	= 1.f;
	m_bBlinkInvisibility		= true;

	SetAlphaObject( true );
	m_RenderParam.bAlphaBlend = true;
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_EventProcess()
{
	if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.13333f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}

	if( m_iBoostAttackCount < 3 + m_iAddBoostAttack && true == m_FrameDataFuture.stateParam.bEventFlagList[1] &&
		m_pXSkinAnimFuture->GetNowAnimationTime() > 0.59f && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.833f )
	{
		StateChangeBoostAttackIfPossible();
	}
	else if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_bDisableGravity = false;
		m_PhysicParam.nowSpeed.y = -0.0001f;

		StateChange( ENSI_JUMP_FLY_DOWN );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_EndFuture()
{
	m_bDisableGravity = false;

	CommonStateEndFuture();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZFrontX_End()
{
	m_bDisableGravity = false;

	CommonStateEnd();

	m_bBlinkInvisibility	= false;

	if( m_bInvisibility == false )
	{
		SetAlphaObject( false );
		m_RenderParam.bAlphaBlend = false;
		m_RenderParam.outLineColor.a	= 1.f;
		m_RenderParam.color.a			= 1.f;
	}
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZUpX_Start()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_bNotEnoughMPDashZX = false;
	++m_iBoostAttackCount;
	++m_iJumpFlyCount;
	m_bBoostUpAttack = true;

	D3DXVECTOR3 vRot = GetRotateDegree();
	vRot.z += 45.f;

    [[maybe_unused]] CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
	pMeshInst  = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ),
		vRot, vRot );

	CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Seraph_Boost_Dash_Attack", this );
	if ( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
	{
		if ( true != GetIsRight() )
			vRot.z += 90.f;

		pEffect->SetLocalRotateDegree( vRot );
	}

	m_fBlinkInvisibilityAlpha	= 1.f;
	m_bBlinkInvisibility		= true;

	SetAlphaObject( true );
	m_RenderParam.bAlphaBlend = true;
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZUpX_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EventProcess()
{
	if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.13333f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}

	if( m_iBoostAttackCount < 3 + m_iAddBoostAttack && true == m_FrameDataFuture.stateParam.bEventFlagList[1] &&
		m_pXSkinAnimFuture->GetNowAnimationTime() > 0.59f && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.833f )
	{
		StateChangeBoostAttackIfPossible();
	}
	else if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_bDisableGravity = false;
		m_PhysicParam.nowSpeed.y = -0.0001f;

		StateChange( ENSI_JUMP_FLY_DOWN );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZUpX_EndFuture()
{
	m_bDisableGravity = false;

	CommonStateEndFuture();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZUpX_End()
{
	m_bDisableGravity = false;

	CommonStateEnd();

	m_bBlinkInvisibility	= false;

	if( m_bInvisibility == false )
	{
		SetAlphaObject( false );
		m_RenderParam.bAlphaBlend = false;
		m_RenderParam.outLineColor.a	= 1.f;
		m_RenderParam.color.a			= 1.f;
	}
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZDownX_Start()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_bNotEnoughMPDashZX = false;
	++m_iBoostAttackCount;
	++m_iJumpFlyCount;
	m_bBoostUpAttack = false;

	D3DXVECTOR3 vRot = GetRotateDegree();
	vRot.z -= 45.f;

    [[maybe_unused]] CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
	pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"P_EBS_BoostDash_M01", GetBonePos( L"Bip01_Head" ),
		vRot, vRot );
	CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Seraph_Boost_Dash_Attack", this );
	if ( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
	{
		if ( true != GetIsRight() )
			vRot.z -= 90.f;

		pEffect->SetLocalRotateDegree( vRot );
	}

	m_fBlinkInvisibilityAlpha	= 1.f;
	m_bBlinkInvisibility		= true;

	SetAlphaObject( true );
	m_RenderParam.bAlphaBlend = true;
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZDownX_FrameMove()
{
	CommonFrameMove();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZDownX_EventProcess()
{
	if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.13333f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}

	if( m_iBoostAttackCount < 3 + m_iAddBoostAttack && true == m_FrameDataFuture.stateParam.bEventFlagList[1] &&
		m_pXSkinAnimFuture->GetNowAnimationTime() > 0.59f && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.833f )
	{
		StateChangeBoostAttackIfPossible();
	}
	else if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_bDisableGravity = false;
		m_PhysicParam.nowSpeed.y = -0.0001f;

		StateChange( ENSI_JUMP_FLY_DOWN );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZDownX_EndFuture()
{
	m_bDisableGravity = false;

	CommonStateEndFuture();
}

void CX2GUEve::ENSI_EBS_DASH_JUMP_COMBO_ZDownX_End()
{
	m_bDisableGravity = false;

	CommonStateEnd();

	m_bBlinkInvisibility	= false;

	if( m_bInvisibility == false )
	{
		SetAlphaObject( false );
		m_RenderParam.bAlphaBlend = false;
		m_RenderParam.outLineColor.a	= 1.f;
		m_RenderParam.color.a			= 1.f;
	}
}

void CX2GUEve::StateChangeBoostAttackIfPossible()
{
	if( true == FlushMp( 5.0f ) )
	{
		if ( true == m_InputData.pureRight )
		{
			if ( false == m_FrameDataFuture.syncData.bIsRight )
			{
				m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
				m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
			}
		}
		else if ( true == m_InputData.pureLeft )
		{
			if ( true == m_FrameDataFuture.syncData.bIsRight )
			{
				m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
				m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
			}
		}

		if ( true == m_InputData.pureUp && false == m_bBoostUpAttack )
		{
			StateChange( ENSI_EBS_DASH_JUMP_COMBO_ZUpX );
		}
		else if ( true == m_InputData.pureDown && m_FrameDataFuture.syncData.position.y > m_FrameDataFuture.unitCondition.landPosition.y + LINE_RADIUS )
		{
			StateChange( ENSI_EBS_DASH_JUMP_COMBO_ZDownX );
		}
		else
		{
			StateChange( ENSI_EBS_DASH_JUMP_COMBO_ZFrontX );
		}
	}
	else
	{
		if( true == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_LANDING );
		}
	}
}
//#pragma endregion

//#pragma region ������ �ϵ齺
void CX2GUEve::ENSI_EBS_ENERGY_NEEDLES_Init()
{
	XSkinMeshReadyInBackground( L"EBS_A_TASER_PILUM_laser_attackLine.X" );
	TextureReadyInBackground( L"EBS_A_TASER_PILUM_laser02.tga" );
	TextureReadyInBackground( L"EBS_A_TASER_PILUM_laser02.tga" );
	XMeshReadyInBackground( L"EBS_A_TASER_PILUM_lightning.Y" );
}

void CX2GUEve::ENSI_EBS_ENERGY_NEEDLES_Start()
{
	CommonStateStart();
}

void CX2GUEve::ENSI_EBS_ENERGY_NEEDLES_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 1.766f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EBS_ENERGY_NEEDLES );

		if ( NULL != pSkillTemplet )
		{
			m_EnergyNiddleData.m_bEnable = true;

			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_EnergyNiddleData.m_fMaxTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) - 0.33f;
			m_EnergyNiddleData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );

			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Hand" );

			if ( true == GetIsRight() )
				vBonePos -= GetDirVector() * 40.f;
			else
				vBonePos += GetDirVector() * 40.f;

			vBonePos.y -= 30.f;

			CX2DamageEffect::CEffect* pEffect = NULL;

			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_ENERGY_NEEDLES", GetPowerRate(), vBonePos,
				GetRotateDegree(), GetRotateDegree() );

			if( NULL != pEffect )
			{
				m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_ENERGY_NEEDLES_FILTERED", L"EVE_ENERGY_NEEDLES_FILTERED",
														L"EVE_ENERGY_NEEDLES_FILTERED", L"EVE_ENERGY_NEEDLES_FILTERED" );

			}
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_EBS_ENERGY_NEEDLES_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_ENERGY_NEEDLES_End()
{
	CommonStateEnd();
}

void CX2GUEve::ENERGY_NEEDLES_Data::OnFrameMove( IN CX2GUEve* pGUEve, IN float fElapsedTime_ )
{
	if ( true == m_bEnable )
	{
		if ( 0.f < m_fMaxTime )
		{
			if ( 0.f < m_fIntervalTime )
			{
				m_fIntervalTime -= fElapsedTime_;
			}
			else
			{
				D3DXVECTOR3 vBonePos = pGUEve->GetBonePos( L"Bip01" );

				int iRandVal = RandomInt( 0, 5 );

				if ( true == pGUEve->GetIsRight() )
					vBonePos -= pGUEve->GetDirVector() * m_vEffectPos[iRandVal].x;
				else
					vBonePos += pGUEve->GetDirVector() * m_vEffectPos[iRandVal].x;

				vBonePos.y += m_vEffectPos[iRandVal].y;

				CX2DamageEffect::CEffect* pEffect = NULL;

				pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( pGUEve, L"EVE_ENERGY_NEEDLES", m_fPowerRate, vBonePos,
					pGUEve->GetRotateDegree(), pGUEve->GetRotateDegree() );

				if( NULL != pEffect )
				{
					pGUEve->m_EveElectraSystem.AddEffectToSystem( pEffect, L"EVE_ENERGY_NEEDLES_FILTERED", L"EVE_ENERGY_NEEDLES_FILTERED",
																L"EVE_ENERGY_NEEDLES_FILTERED", L"EVE_ENERGY_NEEDLES_FILTERED" );
				}

				m_fIntervalTime = 0.33f;
				m_fMaxTime -= 0.33f;
			}
		}
		else
		{
			m_bEnable = false;
		}
	}
}
//#pragma endregion

//#pragma region ������ƽ ��Ʈ
void CX2GUEve::ENSI_EBS_ENERGETIC_HEART_Start()
{
	CommonStateStart();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EBS_ENERGETIC_HEART );
	if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
	{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );

		CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );

		if ( NULL != ptrBuffFactorClone )
		{
			ptrBuffFactorClone->SetGameUnitBuffFactor( this );

			if ( m_fAddEnergeticHeartTime > 0.f )
				ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, m_fAddEnergeticHeartTime );

			SetBuffFactorToGameUnit( *ptrBuffFactorClone );
		}
        if( CX2DamageEffect::CEffect* pTSEffect = ( m_pThousandStar != NULL ) ? m_pThousandStar->GetLiveDamageEffect() : NULL )
		{
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_EveElectraSystem.m_hParticleThousandStarFormation );

			if( m_EveElectraSystem.GetPlaneFormation() == PFT_FUSION )
			{
				m_EveElectraSystem.m_hParticleThousandStarFormation = g_pX2Game->GetMajorParticle()->CreateSequenceHandle(
					(CKTDGObject*) this,  L"eve_thousandsOfStars_model_P01_5_white", pTSEffect->GetPos() );
			}
		}
	}

}

void CX2GUEve::ENSI_EBS_ENERGETIC_HEART_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 2 );

	CommonFrameMove();
}

void CX2GUEve::ENSI_EBS_ENERGETIC_HEART_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_EBS_ENERGETIC_HEART_End()
{
	CommonStateEnd();
}

void CX2GUEve::EveElectraSystem::SetExceptionSystemTime( float _fVal )
{
	m_fExceptionSystemOperationTime	= _fVal;
	m_fExceptionSystemRemainTime	= _fVal;
}
//#pragma endregion

//#pragma region ���� ���� Start �Լ� �и�
void CX2GUEve::SPECTRO_EL_CRYSTAL_Start()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_EEL_SPECTRO_EL_CRYSTAL);
	if( NULL != pSkillTemplet )
	{
		if( true == m_EveElectraSystem.GetEnableSystem() )
		{
			m_EveElectraSystem.ChangePlaneFormation();
			if ( NULL != g_pX2Game && IsMyUnit() )
				g_pX2Game->UpdateSkillSlotUI();
		}
		else
		{
			if( m_EveElectraSystem.GetEnableSystem() == false )
			{
				m_EveElectraSystem.SetEnableSystem( true );

			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO13) == true )
				m_EveElectraSystem.SetSystemOperationTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) + 3.f );
			else
				m_EveElectraSystem.SetSystemOperationTime( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) );

				m_EveElectraSystem.SetPlaneFormation( PFT_AMPLIFIER );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();

				if ( true == m_EveElectraSystem.GetEnableHyperOpticalResearch() )
					const_cast<CX2SkillTree::SkillTemplet*>(pSkillTemplet)->m_eActiveSkillUseCondtion = CX2SkillTree::ASUT_ANYTIME;
			}
		}
	}

	ChangeThousandsOfStarsFormation();
}
//#pragma endregion


//#pragma region ���� ������Ʈ�� �ƴ� �� ���� ���� ����ϴ� �Լ�
void CX2GUEve::PlaySoundAtNoAttackState( const wstring& wstrSoundFileName_ )
{
	/// ���� ������Ʈ�� �ƴϸ� �ش� ���� ���
	switch( GetNowStateID() )
	{
	case USI_WAIT:
	case USI_WALK:
	case USI_JUMP_READY:
	case USI_JUMP_UP:
	case USI_JUMP_DOWN:
	case USI_DASH:
	case USI_DASH_END:
	case USI_DASH_JUMP:
	case USI_DASH_JUMP_LANDING:
	case USI_DAMAGE_GROGGY:
	case ENSI_JUMP_FLY_UP:
	case ENSI_JUMP_FLY_DOWN:
	case ENSI_JUMP_DASH:
	case ENSI_EBS_BOOST_DASH:
	case ENSI_EBS_BOOST_DASH_AIR:
		{
			PlaySound( wstrSoundFileName_.c_str() );
		} break;
	}
}
//#pragma endregion

//#pragma region SI_SA_EN_ILLUSION_RAY
void CX2GUEve::ENSI_A_EN_Illusion_Ray_Init()
{
	XSkinMeshReadyInBackground( L"eve_particleRay_laser.X" );
	TextureReadyInBackground( L"eve_particleRay_circle.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"SpreadLight03_Gray.dds" );
	TextureReadyInBackground( L"Ring.dds" );
}


void CX2GUEve::ENSI_A_EN_Illusion_Ray_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}

void CX2GUEve::ENSI_A_EN_Illusion_Ray_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

void CX2GUEve::ENSI_A_EN_Illusion_Ray_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.01f ) )
	{
		m_PhysicParam.nowSpeed.x = 0.f;
	}
	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_A_EN_Illusion_Ray_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EN_Illusion_Ray_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 1.f, 185.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.43333f ) )
	{

		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 110.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (50.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = NULL;

		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_EPR_ILLUSION_RAY_LASER", GetPowerRate(),
			vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

		m_EveElectraSystem.AddLaserToSystem( pEffect, L"", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.12f, 1.0f,
			L"EVE_EPR_ILLUSION_RAY_LASER", L"EVE_EPR_ILLUSION_RAY_LASER",
			L"EVE_EPR_ILLUSION_RAY_LASER", false );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EN_Illusion_Ray_End()
{
	CommonStateEnd();
	m_EveElectraSystem.DeleteLaserFromSystem();
}
//#pragma endregion �Ϸ��� ����

//#pragma region SI_A_EN_TESLA_SHOCK
void CX2GUEve::ENSI_A_EN_Tesla_Shock_Init()
{
	XSkinMeshReadyInBackground( L"SI_A_EEG_METAL_DUST_AURA_Effect.x" );
	TextureReadyInBackground( L"CutTendon_Slash01.tga" );
}
void CX2GUEve::ENSI_A_EN_Tesla_Shock_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EN_TESLA_SHOCK );
	if( NULL != pSkillTemplet )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Pelvis" );
		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Tesla_Shock",
			GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree() );

		if ( NULL != pDamageEffect )
		{
			CX2DamageManager::DamageData* pDamageData = &pDamageEffect->GetDamageData();

			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			//if ( NULL != pDamageData )
				pDamageData->fForceDownValue = -pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_ABS, iSkillTempletLevel );
		}
	}
}

void CX2GUEve::ENSI_A_EN_Tesla_Shock_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EN_Tesla_Shock_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}
//#pragma endregion �׽��� ��ũ

//#pragma region SI_SA_EEG_HORNET_STING_SHAPED_CHARGE
void CX2GUEve::ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_Init()
{
	XSkinMeshReadyInBackground( L"EG_SP1a_HornetSting_Bomb.X" );
	XSkinMeshReadyInBackground( L"EG_SP1a_HornetSting_Cannon.X" );
	TextureReadyInBackground( L"Eve_EG_SP1_HornetSting_Piece00.tga" );
}

void CX2GUEve::ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.02f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();
        [[maybe_unused]] CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = NULL;
		pMeshInst  = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this, L"Mesh_Eve_EG_SP1a_HornetSting_Cannon", vBonePos,
			GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.47f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Finger1" );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this, L"Flare_EG_SP1a_HornetSting_01", vBonePos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this, L"Expl_EG_SP1a_HornetSting_01", vBonePos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this, L"Explosion_Eve_EG_Sp1a_HS02", vBonePos );

		CX2DamageEffect::CEffect* pEffect =
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Hornet_Sting_Shaped_Charge", GetPowerRate(), vBonePos,
			GetRotateDegree(), GetRotateDegree() );

		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO16 ) && NULL != pEffect )
		{
			pEffect->SetDamageTime( pEffect->GetDamageTime() + 2 );
		}

	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EEG_Hornet_Sting_Shaped_Charge_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ȣ�� ���� - ������Ʈ ����

//#pragma region
void CX2GUEve::ENSI_SA_ENS_Atomic_Blaster_SonicGun_Init()
{
	XMeshReadyInBackground( L"Eve_Atomic_Blaster_GunFire01.Y" );
	XSkinMeshReadyInBackground( L"Eve_SI_SA_ENS_Atomic_Blaster_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_SI_SA_ENS_Atomic_Blaster_Mesh02.X" );
}

void CX2GUEve::ENSI_SA_ENS_Atomic_Blaster_SonicGun_Start()
{
	CommonStateStart();

	m_fEffectStartTime = 0.79f;
	m_vSonicGunRotate = GetRotateDegree();
}

void CX2GUEve::ENSI_SA_ENS_Atomic_Blaster_SonicGun_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 0 );

	D3DXVECTOR3 vBonePos	= GetBonePos( L"Bip01_Pelvis" );

	if ( m_fEffectStartTime <= 1.357f
    && m_pXSkinAnim->EventTimerOneshot( m_fEffectStartTime ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"EVE_NS_ATOMIC_BLASTER_SONICGUN_DUMMY", GetPowerRate(), vBonePos,
			m_vSonicGunRotate, m_vSonicGunRotate );

		m_fEffectStartTime += 0.072f;
		m_vSonicGunRotate.z += 6.f;
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_ENS_Atomic_Blaster_SonicGun_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( m_fAttackCancelAfter )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ����� �������� - �Ҵа�


//#pragma region SI_SA_SA_EEL_SWEEP_ROLLING_TRI_VULCAN
void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_Init()
{
	XMeshReadyInBackground( L"eve_SA_photonFlare_lightLine.Y" );
	TextureReadyInBackground( L"eve_SA_photonFlare_lightLine.tga" );
	TextureReadyInBackground( L"WhitePoint.tga" );
	TextureReadyInBackground( L"Explosion_Sphere.tga" );
	TextureReadyInBackground( L"AeroTornado04.tga" );
	TextureReadyInBackground( L"Particle_Blur.tga" );
	TextureReadyInBackground( L"WhiteCircle02.tga" );
	TextureReadyInBackground( L"CenterLight_Gray01.tga" );
}
void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_Start()
{
	CommonStateStart();
	m_fPlaneZAngle = 0.f;
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.CreatePlane( 0.f, 7.0f, 140.f, 130.f, L"EveProtoPuritySweepParadePlane" );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Ready_EventProcess()
{
	if( m_InputData.pureUp == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
	}
	else if( m_InputData.pureDown == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_FIRE );
	}
	CommonEventProcess();
}

//////// FIRE ///////////
void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_Start()
{
	CommonStateStart();

	m_fEffectStartTime = 0.0;
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_FrameMove()
{
	D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Bip01_R_Hand" );

	if(GetIsRight() == true)
		vDamageEffectPos += ( 50.f * GetDirVector() );
	else
		vDamageEffectPos -= ( 50.f * GetDirVector() );

    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		CX2DamageEffect::CEffect* pEffect = NULL;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO22 ) == true )
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Triangle_Vulcan_Bullet_MEMO", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
		else
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Triangle_Vulcan_Bullet", GetPowerRate(),
			vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
			m_EveElectraSystem.AddEffectToSystem( pEffect );

		m_fEffectStartTime += 0.2f;
	}
	else if ( EventTimer( m_fEffectStartTime, false ) == true )
	{
		if ( m_fEffectStartTime > 3.0 )
		{
			if ( GetNowMp() < 10.f )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				StateChange( ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END );
				return;
			}
			else
			{
				UpNowMp( - 10.f );
			}
		}

		CX2DamageEffect::CEffect* pEffect = NULL;

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO22 ) == true )
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Triangle_Vulcan_Bullet_MEMO", GetPowerRate(),
			vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
		else
		pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Triangle_Vulcan_Bullet", GetPowerRate(),
			vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pEffect )
			m_EveElectraSystem.AddEffectToSystem( pEffect );

		m_fEffectStartTime += 0.2f;
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_EventProcess()
{
	if( m_InputData.pureUp == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
	}
	else if( m_InputData.pureDown == true )
	{
		m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
	}

	if( ( (m_eSpecialAttackKeyPressed == SAKP_A && m_InputData.pureA != true) || (m_eSpecialAttackKeyPressed == SAKP_S && m_InputData.pureS != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_D && m_InputData.pureD != true) || (m_eSpecialAttackKeyPressed == SAKP_C && m_InputData.pureC != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_Q && m_InputData.pureQ != true) || (m_eSpecialAttackKeyPressed == SAKP_W && m_InputData.pureW != true)
		|| (m_eSpecialAttackKeyPressed == SAKP_E && m_InputData.pureE != true) || (m_eSpecialAttackKeyPressed == SAKP_R && m_InputData.pureR != true) )
		&& m_fEffectStartTime >= 3.0f )
	{
		StateChange( ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END );
	}
	else if( EventTimer( 4.9f, true ) == true )
	{
		StateChange( ENSI_SA_EEN_SWEEP_ROLLING_TRI_VULCAN_END );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_Fire_End()
{
	CommonStateEnd();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_Start()
{
	CommonStateStart();
}

void CX2GUEve::ENSI_SA_EEL_Sweep_Rolling_Tri_Vulcan_End_EventProcess()
{
	if( EventTimer( 1.0f, true ) == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ������ �Ѹ� - Ʈ���� ��ĭ

//#pragma region SI_SA_EAT_HEAVENS_FIST_SWEEPER
void CX2GUEve::ENSI_SA_EAT_Heavens_Fist_Sweeper_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_King_Hand.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Light01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Light02.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Bottom_Light.X" );
	XSkinMeshReadyInBackground( L"SI_SA_EAT_HEAVENS_FIST_Effect_Hole.X" );
}

void CX2GUEve::ENSI_SA_EAT_Heavens_Fist_Sweeper_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			CX2EffectSet::Handle hFist = INVALID_EFFECTSET_HANDLE;
			//CX2EffectSet::Handle hFistEffect = INVALID_EFFECTSET_HANDLE;
			D3DXVECTOR3 vPos = GetPos();

			if ( GetIsRight() )
			{
				vPos += GetDirVector() * 300.f;
				//#ifdef ADD_MEMO_1ST_CLASS //��â��   - Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_Nodamage ����Ʈ���� �������� ����. ??? �ϴ� �ּ� ó��
				/*hFistEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_Nodamage",
					(CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, GetVec3ScaleByUnit(), true,
					vPos, GetRotateDegree(), GetDirVector() );*/

				if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO21 ) == true )
					hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_Right_MEMO",
					(CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, GetVec3ScaleByUnit(), true,
					vPos, GetRotateDegree(), GetDirVector() );
				else
				hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_Right",
					(CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, GetVec3ScaleByUnit(), true,
					vPos, GetRotateDegree(), GetDirVector() );
			}
 			else
			{
				vPos -= GetDirVector() * 300.f;
				//#ifdef ADD_MEMO_1ST_CLASS //��â��   - Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_Nodamage ����Ʈ���� �������� ����. ??? �ϴ� �ּ� ó��
				/*hFistEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_Nodamage",
					(CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, GetVec3ScaleByUnit(), true,
					vPos, GetRotateDegree() + D3DXVECTOR3( 0, 0, 180 ), GetDirVector() );*/

				if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO21 ) == true )
					hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_EAT_HEAVENS_FIST_SWEEPER_MEMO",
					(CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, GetVec3ScaleByUnit(), true,
					vPos, GetRotateDegree() + D3DXVECTOR3( 0, 180, 0 ), GetDirVector() );
				else
				hFist = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_EAT_HEAVENS_FIST_SWEEPER",
 					(CX2GameUnit*)this, NULL, false, GetPowerRate() * fStatAtkRateOfSummoned, -1.f, GetVec3ScaleByUnit(), true,
 					vPos, GetRotateDegree() + D3DXVECTOR3( 0, 180, 0 ), GetDirVector() );
			}
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.241f ) )
	{
		UpDownCrashCamera( 80.f, 1.0f );
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EAT_Heavens_Fist_Sweeper_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ����� �ǽ�Ʈ - ������

//#pragma region SI_SA_ENS_ASSUALT_SPEAR
void CX2GUEve::ENSI_SA_ENS_Assualt_Spear_Init()
{
	XSkinMeshReadyInBackground( L"Sp2a_Spear_Fly.X" );
}

void CX2GUEve::ENSI_SA_ENS_Assualt_Spear_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Bip01_L_Finger11", 0.01f, 2 );

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_ENS_Assualt_Spear_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion �Ʈ ���Ǿ� - �׸޽ý�

//#pragma region ENSI_A_EEP_SPIT_FIRE_GRENADE
void CX2GUEve::ENSI_A_EEP_Spit_Fire_Grenade_Init()
{
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Spit_Fire_Mesh01.X" );
	XSkinMeshReadyInBackground( L"Eve_EP_SI_SA_Spit_Fire_Mesh02.X" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	TextureReadyInBackground( L"GuideArrow02.tga" );
	TextureReadyInBackground( L"ColorBallGray.tga" );
}

void CX2GUEve::ENSI_A_EEP_Spit_Fire_Grenade_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		//������ ����� �Ŀ���ġ�� ����Ʈ �¿� ����.
		float fStatAtkRateOfSummoned = GetStatAtkRateOfSummoned();

		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
		{
			bool bHyper = false;
			if ( GetRemainHyperModeTime() > 0.f )
				bHyper = true;

			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_EEP_Spit_Fire_Grenade", (CX2GameUnit*)this, NULL, bHyper,
				GetPowerRate() * fStatAtkRateOfSummoned );
		}
	}
	CommonFrameMove();
}
void CX2GUEve::ENSI_A_EEP_Spit_Fire_Grenade_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� ���̾� - �׷����̵�

//#pragma region SI_A_EBS_KUGELBLITZ
bool CX2GUEve::IsUpSkillSlotKugelBlitz()
{
	bool bIsKugelBlitzSuccess = false;
	if( m_sKugelBlitz_Data.m_bEnable == true )
	{
		//Ű�� ���ȴ��� üũ.
		switch( m_sKugelBlitz_Data.GetSkillSlotID() )
		{
		case 0: if( m_InputData.pureA == false && m_InputData.oneA == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 1: if( m_InputData.pureS == false && m_InputData.oneS == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 2: if( m_InputData.pureD == false && m_InputData.oneD == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 3: if( m_InputData.pureC == false && m_InputData.oneC == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 4: if( m_InputData.pureQ == false && m_InputData.oneQ == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 5: if( m_InputData.pureW == false && m_InputData.oneW == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 6: if( m_InputData.pureE == false && m_InputData.oneE == false )
					bIsKugelBlitzSuccess = true;
			break;
		case 7: if( m_InputData.pureR == false && m_InputData.oneR == false )
					bIsKugelBlitzSuccess = true;
			break;

		default: // �����ÿ� ������ ������ ����� �̷�� ���� �ʾ��� ���.
			m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
			break;;
		}
	}

	return bIsKugelBlitzSuccess;
}


const CX2UserSkillTree::SkillSlotData* CX2GUEve::GetSkillSlotDataKugelBlitz( IN const CX2UserSkillTree& cUserSkillTree_, OUT int& iSkillSlotIndexPressed_ )
{
	//Űó������ ������ ������ �˻�

	int iSlotIndex = ( m_sKugelBlitz_Data.GetSkillSlotID() > 3 ) ? m_sKugelBlitz_Data.GetSkillSlotID()-4 : m_sKugelBlitz_Data.GetSkillSlotID();
	bool bSlotB = ( m_sKugelBlitz_Data.GetSkillSlotID() > 3 ) ? true : false;


	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = cUserSkillTree_.GetSkillSlot( iSlotIndex, bSlotB );

	if( CX2SkillTree::SI_NONE == pSkillSlotData->m_eID || CX2SkillTree::SI_A_EBS_KUGELBLITZ != pSkillSlotData->m_eID)
	{
		m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
		return NULL;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( pSkillSlotData->m_eID );

	if( NULL == pSkillTemplet )
	{
		m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
		return NULL;
	}

	m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
	iSkillSlotIndexPressed_ = m_sKugelBlitz_Data.GetSkillSlotID();

	return pSkillSlotData;
}

void CX2GUEve::SKugelBlitz_Data::Reset(bool bIsChargeTrue /* = true */, FrameData* pFrameDataFuture /* = NULL  */)
{
	if( bIsChargeTrue == false && pFrameDataFuture != NULL )
	{
		pFrameDataFuture->syncData.nowAction = UAI_KUGELBLITZ_CHARGE_FAIL;
	}

	m_bEnable		= false;
	m_bIsSucCharge	= false;

	ResetParticle();

	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_FullCharge.ogg" );
	g_pKTDXApp->GetDeviceManager()->StopSound( L"Eve_MEB_Condense_power.ogg" );
}

void CX2GUEve::SKugelBlitz_Data::ResetParticle()
{
	for( int i = 0; i < std::ssize( m_ahMajorParticleInstance ); ++i )
	{
		if( m_ahMajorParticleInstance[i] != INVALID_PARTICLE_SEQUENCE_HANDLE )
			g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_ahMajorParticleInstance[i] );
		m_ahMajorParticleInstance[i] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

void CX2GUEve::ResetKugelBlitzCharge()
{
	if( m_sKugelBlitz_Data.m_bEnable == true )
		m_sKugelBlitz_Data.Reset( false, &AccessFutureFrameData() );
}

void CX2GUEve::ENSI_A_EBS_Kugelblitz_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed.x = 0.f;
	m_PhysicParam.nowSpeed.y = 0.f;
}

void CX2GUEve::ENSI_A_EBS_Kugelblitz_Start()
{
	CommonStateStart();

	if( (m_sKugelBlitz_Data.m_dMaxTime * 0.5) >= m_sKugelBlitz_Data.m_timerElapsedTime.elapsed() )
		m_sKugelBlitz_Data.m_ChargeRank = CR_RANK_1;
	else if( m_sKugelBlitz_Data.m_dMaxTime >= m_sKugelBlitz_Data.m_timerElapsedTime.elapsed() )
		m_sKugelBlitz_Data.m_ChargeRank = CR_RANK_2;
	else if( m_sKugelBlitz_Data.m_dMaxTime < m_sKugelBlitz_Data.m_timerElapsedTime.elapsed() )
		m_sKugelBlitz_Data.m_ChargeRank = CR_RANK_3;
	else
		m_sKugelBlitz_Data.m_ChargeRank = CR_RANK_1;

	m_fPlaneZAngle	= 0.f;
	m_sKugelBlitz_TinyData.reset();
	m_sKugelBlitz_TinyData.m_timerElapsedTime = m_sKugelBlitz_Data.m_timerElapsedTime;
	m_sKugelBlitz_TinyData.m_ChargeRank = m_sKugelBlitz_Data.m_ChargeRank;
}

void CX2GUEve::ENSI_A_EBS_Kugelblitz_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.66f )
	{
		m_bDisableGravity = false;
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_A_EBS_Kugelblitz_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.406f ) )
	{
		m_EveElectraSystem.CreatePlane( m_fPlaneZAngle, 2.0f, 240.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.416f ) )
	{
		// MP �Ҹ�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EBS_KUGELBLITZ );

		float fMPConsumption = 0.f;
		float fPowerRate = 1.f;
		float fScale = 0.f;
		int iCount = 0;
		wstring wstrDamageEffect = L"";

		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			switch( m_sKugelBlitz_TinyData.m_ChargeRank )
			{
			case CR_RANK_1:
				{
					wstrDamageEffect	= L"Damage_KugelBlitz_Charge";
					fScale				= 1.5f;
				} break;
			case CR_RANK_2:
				{
					wstrDamageEffect	= L"Damage_KugelBlitz_Charge";
					fScale				= 1.5f + ((pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillTempletLevel ) - 1) / 2.f );
				} break;
			case CR_RANK_3:
				{
					wstrDamageEffect	= L"Damage_KugelBlitz_Charge";
					fScale				= 1.5f + pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillTempletLevel );
				} break;
			}

			fMPConsumption	= pSkillTemplet->GetSkillMPConsumptionValue( iSkillTempletLevel );
			fPowerRate		= static_cast<float>( GetPowerRate() * ( min( static_cast<float>( m_sKugelBlitz_TinyData.m_timerElapsedTime.elapsed() ), 3.f) / 3.f + 1.f ) );
			iCount			= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_PIERCING, iSkillTempletLevel ) *
							  min( static_cast<float>( m_sKugelBlitz_TinyData.m_timerElapsedTime.elapsed() ), 3.f) / 3.f );
			m_sKugelBlitz_TinyData.m_iAddDamageTime = iCount;
		}
		else
		{
			fMPConsumption = 20.f;
		}

		if( FlushMp( fMPConsumption ) == true )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_R_Hand" );

			if( true == GetIsRight() )
			{
				vBonePos += GetDirVector() * 70.f;
			}
			else
			{
				vBonePos -= GetDirVector() * 70.f;
			}

			CKTDGParticleSystem::CParticleEventSequence* pSeq =
				g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this, L"EBS_A_TASER_PILUM_fire_P01", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}

			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EBS_A_TASER_PILUM_fire_P02", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}

			pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"EBS_A_TASER_PILUM_fire_P03", vBonePos );
			if( NULL != pSeq )
			{
				pSeq->SetAddRotate( GetRotateDegree() );
				pSeq->SetAxisAngle( GetRotateDegree() );
			}

			CX2DamageEffect::CEffect* pEffect = NULL;

			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffect.c_str(), fPowerRate, vBonePos,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, 1.f );

			pEffect->SetDamageTime( pEffect->GetDamageTime() + iCount );
			pEffect->GetDamageData().bReAttack = true;

			if( pEffect != NULL )
			{
				pEffect->SetScale( fScale, fScale, fScale );
				pEffect->SetInheritForceDownRate(true);
			}

			if( NULL != pEffect )
			{
				wstrDamageEffect += L"_FILTERED";

				m_EveElectraSystem.AddEffectToSystem( pEffect, wstrDamageEffect.c_str(), wstrDamageEffect.c_str(),
					wstrDamageEffect.c_str(), wstrDamageEffect.c_str());
			}

			PlaySound( L"Eve_FireShot1.ogg" );
		}
		else
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( vBonePos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_A_EBS_Kugelblitz_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		m_bDisableGravity = false;
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUEve::ENSI_A_EBS_Kugelblitz_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}
//#pragma endregion ���ۺ�����

//#pragma region SI_SA_EEL_LINEAR_DIVIDER
void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_Init()
{
}
void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_Start()
{
	CommonStateStart();
	m_fPlaneZAngle	= 0.f;
}

void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.01f ) )
	{
		m_PhysicParam.nowSpeed.x = 0.f;
	}

	CommonFrameMoveFuture();
}

void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_EveElectraSystem.CreatePlane( 0.f, 3.0f, 110.f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		m_EveElectraSystem.m_fPlaneZAngle = m_fPlaneZAngle;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		if( m_fPlaneZAngle > 0 && m_EveElectraSystem.GetEnablePlane() == true )
			m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 2.5f, 110.f + 120.f, 130.f + 69.24f );
		else if( m_fPlaneZAngle < 0 && m_EveElectraSystem.GetEnablePlane() == true )
			m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 2.5f, 110.f + 120.f, 130.f - 69.24f );
		else
			m_EveElectraSystem.CreateGigaPlane( m_fPlaneZAngle, 2.5f, 110.f + 139.56f, 130.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );

		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (100.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = NULL;
		CX2DamageEffect::CEffect* pEffect_Reverse = NULL;

		if( GetRemainHyperModeTime() > 0.f )
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Linear_Divider_Hyper", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			float fDistance = 7000.f;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO24 ) == true )
				fDistance *= 1.3f;

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, fDistance, 0.3f, 0.7f,
				L"DamageEffect_Linear_Divider_Hyper_Next", L"DamageEffect_Linear_Divider_Hyper_Next",
				L"DamageEffect_Linear_Divider_Hyper_Next", false );

			pEffect_Reverse = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Linear_Divider_Hyper_Reverse", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			vZeroYDirVector.y = -vZeroYDirVector.y;
			m_EveElectraSystem.AddLaserToSystem( pEffect_Reverse, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, fDistance, 0.3f, 0.7f,
				L"DamageEffect_Linear_Divider_Hyper_Next_Reverse", L"DamageEffect_Linear_Divider_Hyper_Next_Reverse",
				L"DamageEffect_Linear_Divider_Hyper_Next_Reverse", false );
		}
		else
		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Linear_Divider", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			float fDistance = 7000.f;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_EVE_MEMO24 ) == true )
				fDistance *= 1.3f;

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, fDistance, 0.3f, 0.8f,
				L"DamageEffect_Linear_Divider_Next", L"DamageEffect_Linear_Divider_Next",
				L"DamageEffect_Linear_Divider_Next", false );

			pEffect_Reverse = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Linear_Divider_Reverse", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			m_EveElectraSystem.AddLaserToSystem( pEffect_Reverse, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, fDistance, 0.3f, 0.8f,
				L"DamageEffect_Linear_Divider_Next_Reverse", L"DamageEffect_Linear_Divider_Next_Reverse",
				L"DamageEffect_Linear_Divider_Next_Reverse", false );
		}
	}

	CommonFrameMove();
}

void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.4f )
	{
		if( m_InputData.pureUp == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_UP;
		}
		else if( m_InputData.pureDown == true )
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ELECTRA_PLANE_ANGLE_DOWN;
		}
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUEve::ENSI_SA_EEL_LINEAR_DIVIDER_End()
{
	CommonStateEnd();
	m_EveElectraSystem.DeleteLaserFromSystem();
}
//#pragma endregion �巹�� ��ũ


//��������, ��ũ �������� �Ϸ���
void CX2GUEve::ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_Init()
{
	XSkinMeshReadyInBackground(L"CHARGING BOOSTER.X");
	TextureReadyInBackground(L"Eve_Sp1a_SumonCircle01.dds");
	TextureReadyInBackground(L"Explosion_Sphere.dds");
}
void CX2GUEve::ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_Start()
{
	CommonStateStart();
}
void CX2GUEve::ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_fFerdinandLifeTime = static_cast<float>(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ));
			m_fFerdinandLifeTime = max( 0.f, m_fFerdinandLifeTime );

			if( m_fFerdinandLifeTime > 0.f )
			{
				SetActiveLinkOverChargeIllusion(true);
				m_timerFerdinandSummoned.restart();

				if( true == g_pX2Game->IsHost() )
				{
					D3DXVECTOR3 vPos = GetPos();
					if( true == GetIsRight() )
					{
						vPos += GetDirVector() * 300.f;
					}
					else
					{
						vPos -= GetDirVector() * 300.f;
					}

					int iHyperMode = GetHyperModeUsed()? 1 : 0;
					g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_SI_HA_FERDINAND, iHyperMode, true, vPos,
						GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID(), false, CX2Room::TN_NONE, CX2GUNPC::NCT_CHANGESTAGE_REMAIN  );
				}
			}
			else
			{
				SetActiveLinkOverChargeIllusion( false );
				if ( IsMyUnit() )
					g_pX2Game->UpdateSkillSlotUI();
			}
		}
	}

	CommonFrameMove();
}
void CX2GUEve::ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUEve::ENSI_HA_EEP_LINK_OVERCHARGE_ILLUSION_End()
{
	if ( IsMyUnit() )
		g_pX2Game->UpdateSkillSlotUI();		/// ��ų ������ ����

	CommonStateEnd();
}
void CX2GUEve::ResetLinkOverChargeIllusion()
{
	if( true == IsMyUnit() )
	{
		m_fFerdinandLifeTime = -1;
		m_iFerdinandNPCUID = -1;

		SetActiveLinkOverChargeIllusion( false );	/// �������� �ñر� - ��ũ �������� �����Ʈ Ȱ��ȭ ����
		g_pX2Game->UpdateSkillSlotUI();				/// ��ų ������ ����

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION );
		if( NULL != pSkillTemplet )
		{
			CX2UserSkillTree& cUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			const float CoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - static_cast<float>( m_timerFerdinandSummoned.elapsed() );

			cUserSkillTree.SetSkillCoolTimeLeft( CX2SkillTree::SI_HA_EEP_LINK_OVERCHARGE_ILLUSION, CoolTime );
		}
	}
}



// �׸޽ý�, �糪ƽ ��Ŀ��
void CX2GUEve::ENSI_HA_ENS_LUNATIC_SCUD_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallGray.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"ColorBallBlue.dds");
	TextureReadyInBackground(L"Condense_Pulse01.dds");
	XSkinMeshReadyInBackground(L"HA_ENS_Lunatic_Scud_Weapon01.X");
	TextureReadyInBackground(L"Lire_Kick_Impact01.dds");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh03.X");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
	XSkinMeshReadyInBackground(L"EG_SP1a_HornetSting_Bomb.X");
	TextureReadyInBackground(L"Arme01_Frost_Ring_LV2_effect_01.dds");
	TextureReadyInBackground(L"eve_thousandsOfStars_gearLight.dds");
	TextureReadyInBackground(L"EEP_servant_call_slash_02.tga");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"Star.dds");
	TextureReadyInBackground(L"SlashLight.dds");
	XSkinMeshReadyInBackground(L"HA_ENS_Lunatic_Scud_Weapon02.X");
	TextureReadyInBackground(L"Lightning_Piercing01.dds");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"Particle_Frozen01.tga");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Fire_Flame01.Tga");
	TextureReadyInBackground(L"GuideArrow01.dds");
	XSkinMeshReadyInBackground(L"HA_ENS_Lunatic_Scud_Weapon03.X");
	TextureReadyInBackground(L"Nasod_King_Laser03.dds");
	TextureReadyInBackground(L"Whitecircle02.dds");
	TextureReadyInBackground(L"smoke02.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"HA_ENS_Lunatic_Scud_Weapon04.X");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"eve_SA_sweepParade_middle.DDS");
	XSkinMeshReadyInBackground(L"HA_ENS_Lunatic_Scud_Weapon05.X");
	TextureReadyInBackground(L"AeroTornado07.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	TextureReadyInBackground(L"Pet_unicorn_lightning_After.DDS");
	TextureReadyInBackground(L"RVC_SA_DeadlyRaid_spark.dds");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"Steam_BP.DDS");
	TextureReadyInBackground(L"White_Pulse01.dds");
	TextureReadyInBackground(L"ran_StartB_light_P.dds");
	TextureReadyInBackground(L"trockBome_0_MagicAttackA_pieceCircle.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50");
}
void CX2GUEve::ENSI_HA_ENS_LUNATIC_SCUD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}



// ��Ʋ ������, ����ű ��ƿ����
void CX2GUEve::ENSI_HA_EBS_PSYCHIC_ARTILLERY_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	XSkinMeshReadyInBackground(L"Dullahan_Special_Laser_Mesh01.X");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	XSkinMeshReadyInBackground(L"HA_EBS_Dragon_Slave_M01.X");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh03.X");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"NEPHILIM_AttackB_02.X");
	TextureReadyInBackground(L"Particle_Blur.DDS");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"Taranvash_SpecialAttackA_Mesh01.X");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"White_Pulse01.dds");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
	TextureReadyInBackground(L"eve_thousandsOfStars_gearLight.DDS");
}

void CX2GUEve::ENSI_HA_EBS_PSYCHIC_ARTILLERY_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		// ���� ���� �ʱ�ȭ ( �ñر�� ������ ������ �ʴ´�. )
		m_EveElectraSystem.m_fPlaneZAngle = 0.f;
		// 1������ ���� ����
		m_EveElectraSystem.CreateGigaPlane( 0.f, 2.5f, 110.f + 139.56f, 130.f, 1.f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		// 1������ ������ �׷� �߻�
		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 100.f;
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos += (100.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = NULL;

		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase1_Before_Plane", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.3f, 1.0f,
				L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase1_1", L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase1_1",
				L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase1_1", false );

		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 2.6f ) )
	{
	}
    if( m_pXSkinAnim->EventTimerOneshot( 2.7f ) )
	{
		// 1������ ���� ����
		m_EveElectraSystem.DestroyGigaPlaneAndCrystalEffect();
		// 2������ ���� ����
		m_EveElectraSystem.CreateGigaPlane( 0.f, 2.5f, 110.f + 139.56f - 90.f, 260.f, 2.f );
	}
    if( m_pXSkinAnim->EventTimerOneshot( 3.1f ) )
	{
		// 2������ ������ �׷� �߻�
		D3DXVECTOR3 vDamageEffectPos = GetBonePos( L"Bip01_L_Hand");
		D3DXVECTOR3 vZeroYDirVector = GetDirVector();
		vZeroYDirVector.y = 0.f;
		D3DXVec3Normalize( &vZeroYDirVector, &vZeroYDirVector );


		if(GetIsRight() == false)
		{
			vZeroYDirVector.x = -vZeroYDirVector.x;
			vZeroYDirVector.z = -vZeroYDirVector.z;
		}
		vDamageEffectPos -= (100.f * vZeroYDirVector);

		CX2DamageEffect::CEffect* pEffect = NULL;

		{
			pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase2_Before_Plane", GetPowerRate(),
				vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );

			m_EveElectraSystem.AddLaserToSystem( pEffect, L"Bip01_L_Hand", vDamageEffectPos, vZeroYDirVector, 10000.f, 0.3f, 1.0f,
				L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase2_1", L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase2_1",
				L"DAMAGE_EFFECT_HA_EBS_Psychic_Artillery_Laser_Phase2_1", false );

		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 5.545f ) )
	{
		// 2������ ���� ����
		m_EveElectraSystem.DestroyGigaPlaneAndCrystalEffect();
	}
	CommonFrameMove();
}

void CX2GUEve::ENSI_HA_EBS_PSYCHIC_ARTILLERY_EventProcess()
{

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}



/*virtual*/ void CX2GUEve::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ )
{
	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", fTimeToShow_, m_iSkillCutInSetSubIndex, bOnlyLight_ );
}

void CX2GUEve::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove(0.f, 0.1f, true);
}
void CX2GUEve::CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ /*= false*/  )
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );
	//g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );

    if( m_pXSkinAnim->EventTimerOneshot( fTime1_ ) )
	{
		if( false == GetAbsoluteInvisibility() )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeqHyperModeChangeLine	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_HYPER_MODE_CHANGE_LINE ) );
			CKTDGParticleSystem::CParticleEventSequence* pSeqHyperModeChangeCenter	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_HYPER_MODE_CHANGE_CENTER ) );

			D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );
			if( pSeqHyperModeChangeLine != NULL )
			{
				pSeqHyperModeChangeLine->SetPosition( pos );
				pSeqHyperModeChangeLine->SetEmitRate( 40.0f, 80.0f );
				pSeqHyperModeChangeLine->SetTriggerCount( 10 );

				pSeqHyperModeChangeLine->SetBlackHolePosition( pos );
				pSeqHyperModeChangeLine->UseLookPoint( true );
				pSeqHyperModeChangeLine->SetLookPoint( pos );
			}
			if( pSeqHyperModeChangeCenter != NULL )
			{
				pSeqHyperModeChangeCenter->SetPosition( pos );
				pSeqHyperModeChangeCenter->SetEmitRate( 20.0f, 40.0f );
				pSeqHyperModeChangeCenter->SetTriggerCount( 10 );
			}
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( fTime2_ ) )
	{
		if( false == GetAbsoluteInvisibility() )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
			D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );
			if( pSeqGroundShockWave != NULL )
			{
				D3DXVECTOR3 posl = m_FrameDataNow.unitCondition.landPosition;
				posl.y += 5.0f;
				pSeqGroundShockWave->SetPosition( posl );
				pSeqGroundShockWave->SetEmitRate( 5.0f, 10.0f );
				pSeqGroundShockWave->SetTriggerCount( 3 );
			}

			CKTDGParticleSystem::CParticleEventSequence* pSeqWhiteShockWave		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_WHITE_SHOCK_WAVE ) );
			if( pSeqWhiteShockWave != NULL )
			{
				pSeqWhiteShockWave->SetPosition( pos );
				pSeqWhiteShockWave->SetEmitRate( 10.0f, 10.0f );
				pSeqWhiteShockWave->SetTriggerCount( 1 );
			}

			CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTick 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_IMPACT_TICK ) );
			if( pSeqImpactTick != NULL )
			{
				pSeqImpactTick->SetPosition( pos );
				pSeqImpactTick->SetEmitRate( 200.0f, 200.0f );
				pSeqImpactTick->SetTriggerCount( 30 );
				pSeqImpactTick->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
			}
		}

		if ( GetShowSmallGageAndName() )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeqSTR_ToKang 		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_STR_TOKANG ) );
			D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( m_FrameDataNow.syncData.position );
			if( pSeqSTR_ToKang != NULL )
			{
				pSeqSTR_ToKang->SetPosition( D3DXVECTOR3( projPos.x, projPos.y, 0.0f ) );
				pSeqSTR_ToKang->SetEmitRate( 100.0f, 100.0f );
				pSeqSTR_ToKang->SetTriggerCount( 1 );
			}

			UpDownCrashCamera( 30.0f, 0.4f );
			g_pKTDXApp->GetDGManager()->ClearScreen();
		}

		//SetBoostPower( 30 );


		CKTDGParticleSystem::CParticleEventSequence* pSeq_RFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRFoot );
		CKTDGParticleSystem::CParticleEventSequence* pSeq_LFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLFoot );
		CKTDGParticleSystem::CParticleEventSequence* pSeq_RArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRArm );
		CKTDGParticleSystem::CParticleEventSequence* pSeq_LArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLArm );




		if( pSeq_RFoot != NULL )
		{
			pSeq_RFoot->SetPosition( GetBonePos( L"Bip01_R_Foot" ) );
			pSeq_RFoot->SetEmitRate( 30, 40 );
		}
		if( pSeq_LFoot != NULL )
		{
			pSeq_LFoot->SetPosition( GetBonePos( L"Bip01_L_Foot" ) );
			pSeq_LFoot->SetEmitRate( 30, 40 );
		}
		if( pSeq_RArm != NULL )
		{
			pSeq_RArm->SetPosition( GetBonePos( L"Bip01_R_UpperArm" ) );
			pSeq_RArm->SetEmitRate( 30, 40 );
		}
		if( pSeq_LArm != NULL )
		{
			pSeq_LArm->SetPosition( GetBonePos( L"Bip01_L_UpperArm" ) );
			pSeq_LArm->SetEmitRate( 30, 40 );
		}

		if( pSeq_RFoot		!= NULL
			&& pSeq_LFoot	!= NULL
			&& pSeq_RArm 	!= NULL
			&& pSeq_LArm 	!= NULL )
		{
			pSeq_RFoot->SetTrace( true );
			pSeq_LFoot->SetTrace( true );
			pSeq_RArm->SetTrace( true );
			pSeq_LArm->SetTrace( true );


			if( true == GetAbsoluteInvisibility() )
			{
				pSeq_RFoot->SetShowObject( false );
				pSeq_LFoot->SetShowObject( false );
				pSeq_RArm->SetShowObject( false );
				pSeq_LArm->SetShowObject( false );
			}
			else
			{
				pSeq_RFoot->SetShowObject( true );
				pSeq_LFoot->SetShowObject( true );
				pSeq_RArm->SetShowObject( true );
				pSeq_LArm->SetShowObject( true );
			}
		}

		ApplyHyperModeBuff();

		if( true == bSound_ )
		{
			PlaySound( L"Energy.ogg" );
			PlaySound( L"Break.ogg" );
			PlaySound( L"Change.ogg" );
		}
	}
	CommonFrameMove();
}

/** @function : OnFrameMove_EndElCrystalSystem
	@brief : ���� ���� ���� �� ó������� �ϴ� ����
			 ���忡 �����ϴ� ��ų�� ���� ��, �ٷ� ���� ���� �ʰ�, ��ų ���� �� ���Ž�Ű�� ���� �Լ�
*/
void CX2GUEve::OnFrameMove_EndElCrystalSystem()
{
	if( true == m_bIsCheckEndElCrystal )
	{
		switch( m_eReserveEndElCrystalBuffID )
		{
		case BTI_BUFF_AMPLIFICATION_PLACE:
			{
				if ( PFT_AMPLIFIER == m_EveElectraSystem.GetPlaneFormation() )
					EndElCrystalSystem();
			} break;

		case BTI_BUFF_SPECTRUM_PLACE:
			{
				if ( PFT_SPECTRUM == m_EveElectraSystem.GetPlaneFormation() )
					EndElCrystalSystem();
			} break;

		case BTI_BUFF_INDUCTION_PLACE:
			{
				if ( PFT_INDUCTION == m_EveElectraSystem.GetPlaneFormation() )
					EndElCrystalSystem();
			} break;

		case BTI_BUFF_SI_SA_EBS_ENERGETIC_HEART:
			{
				if ( PFT_FUSION == m_EveElectraSystem.GetPlaneFormation() )
				{
					m_EveElectraSystem.SetEnableExceptionSystem( false );
					const BUFF_TEMPLET_ID eBuffTempletID =
						m_EveElectraSystem.GetBuffTempletIdByPlaneFormation( m_EveElectraSystem.m_ePrevSpectro );

					if ( BTI_NONE != eBuffTempletID )
					{
						if ( HaveThisBuff( eBuffTempletID ) )	/// ������ �б������� ���� ������ �ʾ�����
						{
							m_EveElectraSystem.SetPlaneFormation( m_EveElectraSystem.m_ePrevSpectro );

							if ( IsMyUnit() )
								g_pX2Game->UpdateSkillSlotUI();
						}
						else	/// ������ �б������� ��������
						{
							EndElCrystalSystem( true );
						}
					}
					else	/// ������ƽ ��Ʈ ������ ���� ���� ������ ������
					{
						EndElCrystalSystem( true );
					}

					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Seraph_Energetic_Heart_End", this );
				}
				m_iAddBoostAttack = 0;
			} break;

		default:
			break;
		}

		m_eReserveEndElCrystalBuffID = BTI_NONE;
		m_bIsCheckEndElCrystal = false;
	}
}

/*virtual*/void CX2GUEve::InitInpuDataProcess()
{
	// #Ư�� Ű�� �����ϰ� �ʱ�ȭ ��Ű�� �Լ�.
	auto InitInputDataExceptMegaBallKey = []( OUT InputData& sInputData_, OUT bool& bInputDataPureKey_)
	{
		if( true == bInputDataPureKey_ )
		{
			sInputData_.Init();
			bInputDataPureKey_ = true;
		}
		else
		{
			sInputData_.Init();
		}
	};

	if( m_fCanNotInputTimeZXArrow > 0.f )
	{
		m_InputData.Init_ZXArrowOnly();
	}

	if( true == IsCanNotIntput() )
	{
		// # �ް� �Ϸ�Ʈ�к� �����߿���, ���� Ű �������� �ֱ�
		if( m_sMegaElectronBallData.m_bEnable == true )
		{
			switch( m_sMegaElectronBallData.GetSkillSlotID() )
			{
			case 0: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureA ); break;
			case 1: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureS ); break;
			case 2: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureD ); break;
			case 3: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureC ); break;
			case 4: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureQ ); break;
			case 5: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureW ); break;
			case 6: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureE ); break;
			case 7: InitInputDataExceptMegaBallKey( m_InputData, m_InputData.pureR ); break;
			default:
				break;
			}
		}
		else
		{
			m_InputData.Init();
		}
	}
}
