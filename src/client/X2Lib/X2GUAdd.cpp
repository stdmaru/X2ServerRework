#include "stdafx.h"
#include "X2GUAdd.h"


namespace _CONST_ADD_
{
	/// �޺� XXZZ �ݺ���
	const float COMBO_XXZZ_MIN_COUNT						= 3.f;		/// �ּ� �ݺ���
	const float COMBO_XXZZ_MAX_COUNT						= 10.f;		/// �ִ� �ݺ���

	/// ��� ���� �޺� XX ���� �ð�
	const float DASH_JUMP_COMBO_X2_MAX_REMAIN_TIME			= 3.f;		/// �ִ� ���� �ð�

	/// ���׳�Ʈ��
	const int	MAGNETRON_MAX_NUM							= 3;		/// �ִ� ����
	const float	MAGNETRON_FIRE_READY_TIME					= 0.3f;		/// �߻� ����

	/// �޽� ĳ��
	const int	PULSE_CANNON_MIN_FIRE_COUNT					= 4;		/// �ּ� �߻��
	const int	PULSE_CANNON_MAX_FIRE_COUNT					= 8;		/// �ִ� �߻��
	const float	PULSE_CANNON_CONSUME_MP_VALUE				= 25.f;		/// �ִ� �߻��
	const float	PULSE_CANNON_CONSUME_DP_VALUE				= 500.f;	/// �ִ� �߻��

	/// ����ź ���� ���
	const float PULSE_BULLET_MAX_CHARGE_TIME				= 5.f;		/// ����ź 5�ʰ� ���� ����������, ���� �ð��� 1�ʱ����� ���� �� ��
	const float PULSE_BULLET_MAX_APPLY_CHARGE_TIME			= 1.f;
	const float PULSE_BULLET_BASE_CURVE_SPEED				= 5.f;

	/// ���Ϸ�
	const float PYLON_REMAIN_TIME							= 20.f;		/// ���� �ð�
	const float PYLON_NONESTOP_SUMMON_COOL_TIME				= 5.f;		/// ���� ��ȯ ��Ÿ��
	const UINT  PYLON_NONESTOP_SUMMON_LIMIT_NUM				= 3;		/// ���� ��ȯ ��ü ���� ��

	/// EMP ��ũ ����
	const float EMP_SHOCK_NEAR_RANGE						= 700.f;	/// ���� Ÿ�� ����
	const float EMP_SHOCK_FAR_RANGE							= 1200.f;	/// ���� Ÿ�� ����

	/// ���� �ξ�
	const float LEVITATION_CONSUME_MP_PER_SECOND			= 2.f;		/// �ʴ� MP �Ҹ�

	/// ���ҵ� �Ƹ�
	const float NASOD_ARMOR_TARGET_RANGE					= 1000.f;	/// Ÿ���� ����
	const float NASOD_ARMOR_TARGET_HEIGHT_RANGE				= 500.f;	/// Ÿ���� ���� ����

	/// ��ƼŬ ������������
	const int	PARTICLE_ACCELERATOR_MAX_USE_MUTATION_COUNT	= 3;		/// �ִ� ���� ������ �Ҹ��

	/// ��Ŀ��
	const int	CONQUEROR_MAX_USE_MUTATION_COUNT			= 3;		/// �ִ� ���� ������ �Ҹ��

	/// ���̵� �극��Ŀ

	/// �����̽ý� �ʵ�
	const float STASIS_FIELD_REMAIN_TIME					= 15.f;		/// ���� �ð�

	/// �̸��� ��ǳ
	const float DUST_STORM_FIRE_COOL_TIME					= 0.1f;		/// ��Ÿ��
	/// ���ҵ� �Ƹ� �޺� ZZFrontZ_LOOP
	const int	NASOD_ARMOR_ZZFrontZ_LOOP_MIN_COUNT			= 1;		/// �ּ� �ݺ���
	const int	NASOD_ARMOR_ZZFrontZ_LOOP_MAX_COUNT			= 10;		/// �ִ� �ݺ���

	/// ���ҵ� �Ƹ� �޺� XDirectionX_LOOP
	const int	NASOD_ARMOR_XDirectionX_LOOP_MAX_COUNT		= 6;		/// �ִ� �ݺ���

	/// ȯ�� ������
	const float	FANTASY_TRACER_TARGET_REMAIN_TIME			= 2.f;		/// Ÿ�� ���� �ð�
	const float	FANTASY_TRACER_TARGET_DELETE_READY_TIME		= 1.f;		/// ���� �� ��� �ð�

	/// ������ ������
	const float	REVERSE_REACTOR_ENABLE_ATTACK_RANGE			= 1200.f;	/// ���� ����
	const float	REVERSE_REACTOR_ENABLE_ATTACK_RANGE_PVP		= 700.f;	/// ���� ���� ( ���� )
	const float	REVERSE_REACTOR_LINK_RANGE					= 1200.f;	/// ���� ����

	/// ���ǹ� ������
	const float	QUICKSILVER_FRENZY_FIRE_INTERVAL_TIME		= 0.3f;		/// �߻� ����
	const float	QUICKSILVER_FRENZY_FLICKER_INTERVAL_TIME	= 1.f;		/// ���� ����� ��¦�̴� ����

	/// ����ű ����
	const float	PSYCHIC_STORM_DEFAULT_LOOP_TIME				= 0.8f;		/// �⺻�ߵ� �ð�
	const float	PSYCHIC_STORM_ATTACK_INTERVAL_TIME			= 0.105f;	/// ���� ����
	const int	PSYCHIC_STORM_MAX_USE_MUTATION_COUNT		= 3;		/// �ִ� ���� ������ �Ҹ��
}

static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] =
{
	{	/// �ֵ� ���ҵ� �귯, ����ű Ʈ���̼�
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add.dds"),
	},

	{	/// �ֵ� ���ҵ� �糪ƽ ����Ŀ
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_ALP.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_ALP.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_ALP.dds"),
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
	},

#ifdef X2_ADD_2ND_PATH
	{	/// Mastermind
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_AMM.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_AMM.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_AMM.dds"),
	},
#endif
#ifdef X2_ADD_3RD_PATH
	{	/// Diabolic Esper
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_ADE.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_ADE.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Add_ADE.dds"),
	},
#endif
//}} 2026-09-04 X2GUAdd reverify LIFE-4 //
#else
	}
#endif
};

/*static */CX2GUAdd* CX2GUAdd::CreateGUAdd( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit )
{
	return new CX2GUAdd( unitIndex, teamNum, pScriptLuaState, frameBufferNum, pUnit );
};

CX2GUAdd::CX2GUAdd( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit )
: CX2GUUser( unitIndex, teamNum, pScriptLuaState, frameBufferNum, pUnit )
, m_iPressedSkillSlotIndex( -1 )
, m_fMPConsume( 0.f )
, m_fDPConsume( 0.f )
, m_iMutationCountConsume( 0 )
, m_bEnableAirDash( true )
, m_bEnableLevitationWait( true )
, m_iComboLoopCount( 0 )
, m_fRenderParamColorA( 1.f )
, m_fChargeTime( 0.f )
, m_iChargeCount( 0 )
, m_fChangeFormationCoolTime( 0.f )
, m_fDPRateBySpecialActive( 1.f )
, m_fDPRateByActive( 1.f )
, m_fDPRateByCommand( 1.f )
, m_fDPRateByNasodArmor( 1.f )
, m_fDPRateByFormationModeAttack ( 0.f )
, m_fDPRateByFormationModeHit ( 0.f )
, m_fDPRateByFormationModeUseMP ( 0.f )
, m_fDPRateByNoFormationModeAttack ( 0.f )
, m_fUseDPByNasodBarrier ( 0.f )
, m_fUseDPByEnableFormationMode( 0.f )
, m_fUseDPByFormationMode ( 0.f )
, m_bPossibleNasodArmorModeState( false )
, m_bSetUpDPByAttackThisFrame( false )
, m_bSetUpDPByDamageThisFrame( false )
, m_hParticlePrsim( INVALID_EFFECTSET_HANDLE )
, m_pLoopSound(NULL)
, m_eCustomRenderState( CRS_NONE )
, m_fGainDPValueChangeRate( 1.f )
, m_fNotUsingDPSkillDamageRate( 1.f )
, m_fUsingDPValueChangeRate( 1.f )


#ifdef X2_ADD_3RD_PATH
, m_fVoidImpactDownAngle( -60.f )
, m_fVoidImpactUpAngle( 60.f )
#endif
, m_fNaCommandDamageRate( 1.f )
, m_fDPValueRateByActiveFormationMode( 1.f )
, m_iLibraryOfLimitlessLevel( 0 )
, m_fDustStormRateByHit(1.f)
, m_fDustStormRateByDamage(1.f)
, m_fDustStormCoolTime(0.f)
, m_fBodyOfTranscendenceActiveRate( 0.f )
, m_fBodyOfTranscendenceMoveSpeed( 1.f )
, m_bEnablePowerExchanger( false )
, m_ePowerExchangerState( PES_NONE )
, m_bEnableFantasyTracer( false )
, m_fFantasyTracerUpMPRateByFormationMode( 0.f )
, m_eNasodArmorAttackActiveSkillID( CX2SkillTree::SI_NONE )
, m_eNasodArmorLandingActiveSkillID( CX2SkillTree::SI_NONE )
, m_fNonstopSummonPylonCoolTime( 0.f )
, m_uiNonstopSummonPylonCount( 0 )
, m_fQuicksilverFrenzyFlickerTime( 0.f )
{
	m_fThrowStartTime		= 0.66f;
	m_vThrowStartPosOffset	= D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity	= D3DXVECTOR3(2000.f, 100.f, 0.f);

	m_vecPylonLinkDamageEffectData.resize( 0 );
	m_vecSummonPylonDamageEffectData.resize( 0 );

	/// �޽� ĳ�� �߻� �ε��� ����
	for( int i = 1; i <= 6; ++i )
	{
		m_vecPulseBulletFireIndex.push_back( i );
	}

	m_vecFantasyTracerTargetInfo.resize( 0 );
	m_vecDustAnatomyTargetInfo.resize( 0 );

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	m_hVoidFieldDE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hVoidFieldLoopES		= INVALID_EFFECTSET_HANDLE;
	m_hATTComboZZXXAttackES	= INVALID_EFFECTSET_HANDLE;
	m_hATTComboZZXXES		= INVALID_EFFECTSET_HANDLE;
	m_bVoidFieldEndBurst	= false;
	m_fVoidFieldLoopTime	= 0.f;
	m_vecMaxStrikeHandles.clear();
	m_fMaxStrikePower		= 0.f;
	m_fMaxStrikeWindowDuration	= 0.f;
	m_fMaxStrikeWindowRemain	= 0.f;	// official this[6618] (the ticking window remaining)
	m_uidMaxStrikeTarget		= 0;	// official this[6622-6623] (the homing-target record)
	m_iMaxStrikeTargetType		= 0;	// official this[6624]
	//}} 2026-09-04 X2GUAdd leftovers LOCK-2b //
	m_hStardustFieldDE		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hStardustLoopES		= INVALID_EFFECTSET_HANDLE;
	m_hStardustMesh			= INVALID_MESH_INSTANCE_HANDLE;
	m_iStardustDummyCounter	= 1;
	m_fStardustSpawnAccum	= 0.f;
	m_fStardustDuration		= 0.f;
	m_fStardustLoopTime		= 0.f;
	m_vStardustSavedNowSpeed	= D3DXVECTOR2( 0.f, 0.f );
	m_hSpaceCrackES			= INVALID_EFFECTSET_HANDLE;
	m_bSpaceCrackActive		= false;
	m_bSpaceCrackFieldOn	= false;
	m_fSpaceCrackPower		= 0.f;
	m_bMoonlightActive		= false;
	m_fMoonlightTimer		= 0.f;
	m_fMoonlightPower		= 0.f;
	m_bMoonlightDPConsumed	= false;
	m_hMoonlightWaveES		= INVALID_EFFECTSET_HANDLE;

	m_bForceNovaHit			= false;
	m_uidStigmaTarget		= 0;
	m_iStigmaTargetType		= 0;
	m_vStigmaMarkPos		= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_fStigmaMarkDuration	= 0.f;
	m_fStigmaMarkTimer		= 0.f;
	m_hStigmaDebuffES		= INVALID_EFFECTSET_HANDLE;
	//}} 

	m_bApocalypseSummoned		= false;
	m_bApocalypseFinisherDone	= false;
	//}} 2026-08-30 MARCH2015 batch5 //

	m_iFactoryMarkedSkillID		= 0;
	m_hFactoryDroneDE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_vFactoryDroneOffset[0]	= -200.f;	// official flt_32D6D600
	m_vFactoryDroneOffset[1]	= 150.f;	// official flt_32D3C9CC
	m_vFactoryDroneOffset[2]	= 0.f;
	m_iFactoryPhase				= 0;
	m_bFactoryFlagWait			= true;
	m_bFactoryModeEntered		= false;
	m_fFactoryDP				= 0.f;
	m_fFactoryMP				= 1.f;
	m_fInstallDmgScale			= 1.f;
	m_fFactoryDroneSpeed		= 1250.f;	// official flt_32D87770
	m_iNewUpdateLevel			= 0;
	m_hInstall9015DE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hInstall9019DE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_fInstall9019Duration		= 0.f;
	m_hInstall9020DE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_fInstall9020Flag			= 1.f;
	m_fInstall9020Duration		= 0.f;
	m_hInstall9022DE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_fInstall9022Power			= 1.f;
	m_fInstall9022Timer			= 0.5f;		// official flt_32D2C9E8
	m_hFactoryMiscES			= INVALID_EFFECTSET_HANDLE;
	m_vFactoryDroneLastPos[0]	= 0.f;
	m_vFactoryDroneLastPos[1]	= 0.f;
	m_vFactoryDroneLastPos[2]	= 0.f;

	m_vecUltFuryDrones.clear();
	m_fUltFuryElapsed			= 0.f;
	m_fUltFuryInterval			= 0.f;
	m_fUltFuryDuration			= 0.f;
	m_fUltFuryPower				= 1.f;
	m_iUltFurySound				= 0;
	//}} 2026-09-06 MARCH2015 Ultimate Fury attack driver (IDA L4) ctor //
	//}} 2026-08-30 MARCH2015 batch6 //

	m_hPanzerAimBG				= INVALID_EFFECTSET_HANDLE;
	m_hPanzerAimDE				= INVALID_DAMAGE_EFFECT_HANDLE;
	m_fPanzerChargeTime			= 0.f;
	m_fPanzerAimAngle			= 0.f;
	m_bPanzerAimSpawned			= false;
	m_bPsionicSpawned			= false;
	m_abRushingDroneFlag[0]		= false;
	m_abRushingDroneFlag[1]		= false;
	m_fFlickDiscScaleMul[0]		= 1.f;
	m_fFlickDiscScaleMul[1]		= 1.f;
	m_fXXDownXLoopSpawnAccum	= 0.f;
	m_hXXDownXLoopDE			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_vXXDownXAnchor			= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_fXXDownXChildSpawnAccum	= 0.f;
	m_iXXDownXChildSpawnCount	= 0;	//}} 2026-09-06 //
	//}} 2026-08-30 MARCH2015 batch7 AT/MM //

	m_hDynamoLockES				= INVALID_EFFECTSET_HANDLE;
	m_uidDynamoLockTarget		= 0;
	m_bDynamoLockIsNPC			= false;
	m_fDynamoLockRemainTime		= -1.f;
	m_bDynamoLockArmed			= false;
	m_fMagnetronStoredPower		= 0.f;
	//}} 2026-09-04 X2GUAdd reverify LOCK-1 //

	m_iPAMemoCharge				= 0;
	m_hPanzerReadySound			= NULL;
	//}} 2026-09-04 X2GUAdd reverify W3.4a //

	m_fMindControlDebuffReduceRate	= 0.f;
	m_fMindControlProcRate			= 0.f;
	m_fMindControlRecoveryValue		= 0.f;
	m_fTimeControlProcRate			= 0.f;
	m_fTimeControlCooldownValue		= 0.f;
	m_fMindCircleMpValue			= 0.f;
	m_fMindCircleDpValue			= 0.f;
	m_iESPMindBreakCount			= 1;
	m_fESPProcRate					= 0.f;
	m_fESPDamagedRel				= 0.f;
	m_bESPMidairPhaseShift			= false;
	m_bOverlimitEnabled				= false;
	m_fOverlimitMpGainOnHit			= 1.0f;
	m_fOverlimitMpGainGetHit		= 1.0f;
	m_fOverlimitDpGain				= 1.0f;
	m_fOverlimitDpUse				= 1.0f;
	m_fOverlimitCooltime			= 1.0f;
	//}} 2026-09-07 //
	//}}

#endif
}

CX2GUAdd::~CX2GUAdd(void)
{
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	if ( ThirdPath_IsApocalypseSummonActive() )
	{
		ThirdPath_ApocalypseFinishSummon();
		ThirdPath_ApocalypseClearMarker();
	}
	//}} 2026-09-04 X2GUAdd reverify LIFE-1 //
#endif
	StopLoopSound();
#ifdef X2_ADD_2ND_PATH

	if ( NULL != m_hPanzerReadySound )
	{
		m_hPanzerReadySound->Stop();
		m_hPanzerReadySound = NULL;
	}
	//}} 2026-09-04 X2GUAdd reverify ATMM-9 //
#endif
}

/*static*/ void CX2GUAdd::AppendDeviceToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

/*static*/ void CX2GUAdd::AppendComponentToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

/*static*/ void CX2GUAdd::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUAdd::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUAdd::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForSwordToDeviceList( listInOut_, eDamageType_ );
}

/*virtual */bool CX2GUAdd::CanRidingState()
{
	if ( GetNowStateID() == CX2GUUser::USI_WAIT || GetNowStateID() == CX2GUUser::USI_WALK ||
		GetNowStateID() == CX2GUUser::USI_DASH || GetNowStateID() == CX2GUUser::USI_DASH_END ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT || GetNowStateID() == CX2GUUser::USI_RIDING_WALK ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT_HABIT || GetNowStateID() == CX2GUUser::USI_RIDING_DASH ||
		GetNowStateID() == CX2GUUser::USI_RIDING_DASH_END || GetNowStateID() == CX2GUAdd::ANSI_HYPER_WAIT )
		return true;

	return false;
}

/*virtual*/ void CX2GUAdd::Init( bool bUseTeamPos_, int iStartPosIndex_ )
{
	CX2GUUser::Init( bUseTeamPos_, iStartPosIndex_ );

	SetMaxSoul( MAX_DP_GAGE_VALUE );

	SetNeedToUpdateWeaponEffect( true );
	UpdateEquipCollisionData();

	/// ���� ��� ����
	if ( NULL != GetAddGageData() )
		GetAddGageData()->SetIsFormationMode( false );

	/// DP, ���� ��� ���� ����, ���� ������ ����
	if ( IsMyUnit() )
	{
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_PVP:
			{
				// TODO :  �ٸ� �������� InitComponent ó����.
				// ������//���� ó���� ���� �̷��� �۾� ������,
				//		   SetHyperModeCount�� ADD�� �°� �����ؾ� �� �Ŷ�� ���� ��.

				/// �г��� ���� �����, DP�� ���� ���� �Ѵ�.
				if( true == GetHasHyperModeItem() )
				{
					SetDPValue( 2500.f );
					SetHasHyperModeItem(false);
				}
				else
				{
					SetDPValue( 0.f );
				}

				SetMutationCount( 0 );
			} break;

		default:
			{
				if ( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					SetDPValue( 0.f );
					SetMutationCount( 0 );
				}
			} break;
		}

		/// DP �� ���� ��ġ ����ȭ
		m_FrameDataFuture.syncData.m_CannonBallCount = GetAbilitySyncData();
	}
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	switch ( GetUnitClass() )
	{
#ifdef X2_ADD_2ND_PATH
		case CX2Unit::UC_ADD_ARC_TRACER:
#endif
#ifdef X2_ADD_2ND_PATH
		case CX2Unit::UC_ADD_MASTERMIND:
#endif
#ifdef X2_ADD_2ND_PATH
			EraseBuffTempletFromGameUnit( BTI_BUFF_DYNAMO_FACTORY );
			break;
#endif
#ifdef X2_ADD_3RD_PATH
		case CX2Unit::UC_ADD_DIABOLIC_ESPER:
			// BLOCKED (documented accepted gap): official f_324EF690 = the DE formation/custom render-object teardown+recreate on member +26336; the DE render-object subsystem has no our-side member/system (C01 DEP ledger) -- no code.
			break;
#endif
		default:
			break;
	}
	//}} 2026-09-05 //
#endif
}
/*virtual*/ void CX2GUAdd::ParseCommonRandomState()
{
	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{
		const char* tableName = "";
		switch( m_pUnit->GetClass() )
		{
		case CX2Unit::UC_ADD_NASOD_RULER:
			{
				tableName = "ADD_NASOD_RULER";
			} break;
		case CX2Unit::UC_ADD_PSYCHIC_TRACER:
			{
				tableName = "ADD_PSYCHIC_TRACER";
			} break;
		case CX2Unit::UC_ADD_LUNATIC_PSYKER:
			{
				tableName = "ADD_LUNATIC_PSYKER";
			} break;
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

#ifdef X2_ADD_2ND_PATH
		case CX2Unit::UC_ADD_ARC_TRACER:
			{
				tableName = "ADD_ARC_TRACER";
			} break;
#endif
#ifdef X2_ADD_2ND_PATH
		case CX2Unit::UC_ADD_MASTERMIND:
			{
				tableName = "ADD_MASTERMIND";
			} break;
#endif
#ifdef X2_ADD_3RD_PATH
		case CX2Unit::UC_ADD_TIME_TRACER:
			{
				tableName = "ADD_TIME_TRACER";
			} break;
#endif
#ifdef X2_ADD_3RD_PATH
		case CX2Unit::UC_ADD_DIABOLIC_ESPER:
			{
				tableName = "ADD_DIABOLIC_ESPER";
			} break;
#endif
#endif
		default:
			{
				ASSERT( !"NO" );
			} break;
		}

		CX2GUUser::ParseCommonRandomState(tableName);
		m_LuaManager.EndTable(); // INIT_COMMON_RANDOM_STATE
	}
}
/*virtual*/ void CX2GUAdd::InitState()
{
	switch ( GetUnitClass() )
	{
	case CX2Unit::UC_ADD_NASOD_RULER:
	case CX2Unit::UC_ADD_PSYCHIC_TRACER:
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH
	case CX2Unit::UC_ADD_ARC_TRACER:
#endif
#ifdef X2_ADD_3RD_PATH
	case CX2Unit::UC_ADD_TIME_TRACER:
#endif
#endif
		{
			m_iSkillCutInSetIndex = 0;
		} break;
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH
	case CX2Unit::UC_ADD_MASTERMIND:
		{
			m_iSkillCutInSetIndex = 2;
		} break;
#endif
#ifdef X2_ADD_3RD_PATH
	case CX2Unit::UC_ADD_DIABOLIC_ESPER:
		{
			m_iSkillCutInSetIndex = 3;
		} break;
#endif
#endif
	default:
	case CX2Unit::UC_ADD_LUNATIC_PSYKER:
		{
			m_iSkillCutInSetIndex = 1;
		} break;
	case CX2Unit::UC_NONE:
		{
			ASSERT( !L"Unit Class Is fault!" );
			m_iSkillCutInSetIndex = 0;
		} break;
	}
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


#ifdef X2_ADD_2ND_PATH
	if ( ThirdPath_IsApocalypseSummonActive() )
	{
		ThirdPath_ApocalypseFinishSummon();
		ThirdPath_ApocalypseClearMarker();
	}
	else
	{
		m_bApocalypseSummoned		= false;
		m_bApocalypseFinisherDone	= false;
	}
#endif


#ifdef X2_ADD_2ND_PATH
	ThirdPath_UltimateFuryClearSet();
#endif
#endif

	InitStateID();
	InitStateCommon();
	InitStateByUnitClass();

	std::string strStartTableNameUTF8;
	std::string strWinTableNameUTF8;
	std::string strLoseTableNameUTF8;

	InitStateCommonRandom( strStartTableNameUTF8, strWinTableNameUTF8, strLoseTableNameUTF8 );

	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_START;
	m_LuaManager.MakeTableReference( strStartTableNameUTF8.c_str(), stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WIN;
	m_LuaManager.MakeTableReference( strWinTableNameUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, WinStateStart );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_LOSE;
	m_LuaManager.MakeTableReference( strLoseTableNameUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, LoseStateStart );
	m_StateList[stateData.stateID] = stateData;

	InitPassiveSkillState();
	InitEquippedActiveSkillState();

	CX2GUUser::InitState();
}
/*virtual*/ void CX2GUAdd::InitComponent()
{
	CX2GUUser::InitComponent();

	/// DP ȹ�� �� �Ҹ� ���� ��ġ ����
	if( m_LuaManager.BeginTable( "INIT_COMPONENT" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_SPECIAL_ACTIVE",			m_fDPRateBySpecialActive,			0.0f );	/// ����� ��Ƽ�� ������ ���� DP ������ ����
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_ACTIVE",					m_fDPRateByActive,					0.0f );	/// ��Ƽ�� ������ ���� DP ������ ����
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_COMMAND",					m_fDPRateByCommand,					0.0f );	/// Ŀ�ǵ� ������ ���� DP ������ ����
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_NASOD_ARMOR",				m_fDPRateByNasodArmor,				0.0f );	/// ���ҵ� �Ƹ� ������ ���� DP ������ ����
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_FORMATION_MODE_ATTACK",	m_fDPRateByFormationModeAttack,		0.0f );	/// ��������� Ÿ�ݽ� DP ���� ����
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_FORMATION_MODE_HIT",		m_fDPRateByFormationModeHit,		0.0f );	/// ��������� �ǰݽ� DP ���� ����
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_FORMATION_MODE_USE_MP",	m_fDPRateByFormationModeUseMP,		0.0f );	/// ��������� MP ���� DP ���� ���� ( ����� ��Ƽ�� �̻� )
		LUA_GET_VALUE( m_LuaManager, "DP_RATE_BY_NO_FORMATION_MODE_ATTACK",	m_fDPRateByNoFormationModeAttack,	0.0f );	/// ������� ������ Ÿ�ݽ� DP ���� ����
		LUA_GET_VALUE( m_LuaManager, "USE_DP_BY_ENABLE_FORMATION_MODE",		m_fUseDPByEnableFormationMode,		0.0f );	/// ���� ��� Ȱ�� ���� �䱸 DP ��ġ
		LUA_GET_VALUE( m_LuaManager, "USE_DP_BY_FORMATION_MODE",			m_fUseDPByFormationMode,			0.0f );	/// ���� ��� ��ȯ�� �Ҹ� DP ��ġ
		LUA_GET_VALUE( m_LuaManager, "USE_DP_BY_NASOD_BARRIER",				m_fUseDPByNasodBarrier,				0.0f );	/// ���ҵ� �踮�� �ߵ��� �Ҹ� DP ��ġ
		LUA_GET_VALUE( m_LuaManager, "CHANGE_FORMATION_MODE_COOL_TIME",		m_fChangeFormationCoolTime,			0.0f );	/// ���� ��� ��ȯ ��Ÿ��
#ifdef X2_ADD_3RD_PATH
		LUA_GET_VALUE( m_LuaManager, "VOID_IMPACT_DOWN_ANGLE",			m_fVoidImpactDownAngle,				-60.f );
		LUA_GET_VALUE( m_LuaManager, "VOID_IMPACT_UP_ANGLE",			m_fVoidImpactUpAngle,				60.f );
#endif

		m_LuaManager.EndTable();
	}
}
/*virtual*/ void CX2GUAdd::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();
	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
 	CX2SkillTree* const pSkillTree = g_pData->GetSkillTree();

	SetAddMagicAttackCriticalDamageRate(0.f);
	SetAddMagicAttackCriticalRate(0.f);
	SetAddAnimSpeedFactor(0.f);

	/// �⺻ ���� ���� �нú�
	//////////////////////////////////////////////////////////////////////////
	{
		/// ���̳ʸ� ���� - ��������
		{
			m_ChargedImpulsarData.ResetChargedImpulsarData();

			const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_AN_CHARGED_IMPULSAR;
			const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
			if ( 0 < iSkillLevel )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
				if( NULL != pSkillTemplet )
				{
					m_ChargedImpulsarData.m_bEnableChargedImpulsar = true;		/// �������� ��� Ȱ��

					m_ChargedImpulsarData.m_fIncreaseLifeTimeMaxRate	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel );
					m_ChargedImpulsarData.m_fIncreaseScaleMaxRate		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillLevel );
					m_ChargedImpulsarData.m_fIncreaseCurveSpeedMaxRate	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_CURVE_SPEED_REL, iSkillLevel );
					m_ChargedImpulsarData.m_fIncreaseDamageRate			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
				}
			}
		}

		/// ���̳ʸ� ���� - ��ü
		{
			m_fGainDPValueChangeRate = 1.f;
			m_fNotUsingDPSkillDamageRate = 1.f;

			const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_AN_RESEARCH_DYNAMO_DISSOLUTION;
			const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
			if ( 0 < iSkillLevel )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
				if( NULL != pSkillTemplet )
				{
					m_fGainDPValueChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DP_GAIN_REL, iSkillLevel );
					m_fNotUsingDPSkillDamageRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
				}
			}
		}

		/// ���̳ʸ� ���� - ����
		{
			m_fUsingDPValueChangeRate = 1.f;
			m_fUsingDPSkillDamageRate = 1.f;

			const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_AN_RESEARCH_DYNAMO_COMPOSITION;
			const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
			if ( 0 < iSkillLevel )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
				if( NULL != pSkillTemplet )
				{
					m_fUsingDPValueChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DP_USE_REL, iSkillLevel );
					m_fUsingDPSkillDamageRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
				}
			}
		}

		/// ���� ��ȭ
		{
			m_PhaseShiftData.m_fReturnMPConsume = 0.f;
			m_PhaseShiftData.m_fEffectLifeTime = 0.f;

			const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_A_AN_PHASE_SHIFT;
			const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
			if ( 0 < iSkillLevel )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
				if( NULL != pSkillTemplet )
				{
					m_PhaseShiftData.m_fReturnMPConsume =
						pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_BURN_ABS, iSkillLevel );

					m_PhaseShiftData.m_fEffectLifeTime =
						pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel );
				}
			}
		}

	}

	// ���ҵ� �Ƹ� ���
	m_fNaCommandDamageRate = 1.f;
	m_fDPValueRateByActiveFormationMode = 1.f;
	// ������ ������
	m_iLibraryOfLimitlessLevel = 0;

#ifdef X2_ADD_3RD_PATH
	m_fMindControlDebuffReduceRate	= 0.f;
	m_fMindControlProcRate			= 0.f;
	m_fMindControlRecoveryValue		= 0.f;
	m_fTimeControlProcRate			= 0.f;
	m_fTimeControlCooldownValue		= 0.f;
	m_fMindCircleMpValue			= 0.f;
	m_fMindCircleDpValue			= 0.f;
	m_iESPMindBreakCount			= 1;
	m_fESPProcRate					= 0.f;
	m_fESPDamagedRel				= 0.f;
	m_bESPMidairPhaseShift			= false;
	m_bOverlimitEnabled				= false;
	m_fOverlimitMpGainOnHit			= 1.0f;
	m_fOverlimitMpGainGetHit		= 1.0f;
	m_fOverlimitDpGain				= 1.0f;
	m_fOverlimitDpUse				= 1.0f;
	m_fOverlimitCooltime			= 1.0f;
	//}} 2026-09-07 //
#else
	/// �ʿ����� ��ü
	m_fBodyOfTranscendenceActiveRate		= 0.f;		/// �ߵ� Ȯ�� �ʱ�ȭ
	m_fBodyOfTranscendenceMoveSpeed			= 1.f;		/// �̵� �ӵ� ������ �ʱ�ȭ
#endif

	/// ��� ��ȯ��
	m_ePowerExchangerState					= PES_NONE;	/// ��� ��ȯ�� ���� �ʱ�ȭ
	m_bEnablePowerExchanger					= false;	/// ���� ���� �ʱ�ȭ

	/// ȯ�� ������
	m_bEnableFantasyTracer					= false;	/// ���� ���� �ʱ�ȭ
	m_fFantasyTracerUpMPRateByFormationMode = 0.f;		/// ���� ��� ��ȯ�� ȸ�� MP ���� �ʱ�ȭ

	switch( GetUnitClass() )
	{
	case CX2Unit::UC_ADD_LUNATIC_PSYKER:
		{
			/// �ʿ����� ��ü
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ALP_BODY_OF_TRANSCENDENCE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						/// �ߵ� Ȯ��
						m_fBodyOfTranscendenceActiveRate	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );

						/// �̵� �ӵ� ����ġ
						m_fBodyOfTranscendenceMoveSpeed		= max( 1.f, pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel ) );
					}
				}
			}

			/// ��� ��ȯ��
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ALP_POWER_EXCHANGER;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					m_bEnablePowerExchanger = true;	/// ��� ��ȯ�� ���� ���� ����
				}
				else	/// ���� ��� ��ȯ�� ���� �����߿� ��ų �ʱ�ȭ �Ҷ��� ����Ͽ�, ������ ���� ��������.
				{
					EraseBuffTempletFromGameUnit( BTI_BUFF_POWER_EXCHANGER_HALF_UP );
					EraseBuffTempletFromGameUnit( BTI_BUFF_POWER_EXCHANGER_HALF_DOWN );
				}
			}

			/// ȯ�� ������
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ALP_FANTASY_TRACER;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_bEnableFantasyTracer					= true;	/// ȯ�� ������ ���� ���� ����
						/// ���� ��� ��ȯ�� ȸ�� MP ���� ����
						m_fFantasyTracerUpMPRateByFormationMode = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
					}
				}
			}
		}/// 1�� ���� �нú긦 �˻��ϱ� ����, break ���� ����
		[[fallthrough]];


	case CX2Unit::UC_ADD_PSYCHIC_TRACER:
		{
			/// ���ҵ� �Ƹ� ���
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_APT_NASOD_ARMOR_MODE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_fNaCommandDamageRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
						m_fDPValueRateByActiveFormationMode = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DP_USE_REL, iSkillLevel );
					}
				}
			}

			/// ������ ������
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_APT_LIBRARY_OF_LIMITLESS;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				m_iLibraryOfLimitlessLevel = iSkillLevel;
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						// m_SkillRelatedData�� �ʱ�ȭ�� CX2GUUser::InitPassiveSkillState()���� ����.
						const float fAddForceDownRate = CalculateIncreasingRate(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_REL, iSkillLevel ));
						m_SkillRelatedData.SetForceDownValueRateOfMeleeAttack( m_SkillRelatedData.GetForceDownValueRateOfMeleeAttack() + fAddForceDownRate );
						m_SkillRelatedData.SetForceDownValueRateOfRangeAttack( m_SkillRelatedData.GetForceDownValueRateOfRangeAttack() + fAddForceDownRate );
					}
				}
			}

			/// �̸��� ��ǳ
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_APT_DUST_STORM;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUAdd, CreateDustStormToAttackResult ) );
						InsertDelegateProcessWithDamageDataToDamageReact( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUAdd, CreateDustStormToDamageReact ) );

						m_fDustStormRateByHit = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_FIRST_REL, iSkillLevel );
						m_fDustStormRateByDamage = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_MAGIC_SECOND_REL, iSkillLevel );
					}
				}
			}
		} break;
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH
	#ifdef X2_ADD_2ND_PATH
	case CX2Unit::UC_ADD_MASTERMIND:	// MM falls through to AT (2nd job of path 2 inherits the 1st-job passives)
	#endif
	case CX2Unit::UC_ADD_ARC_TRACER:
		{

			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_AAT_NASOD_ARMOR_MODE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_fNaCommandDamageRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
					}
				}
			}
		} break;
#endif

#ifdef X2_ADD_3RD_PATH
	case CX2Unit::UC_ADD_DIABOLIC_ESPER:
		{
			/// 9415 Mind Control (official reads: SA_REDUCE_EDT_TIME_REL / SA_EFFECTIVE_RATE / SA_RECOVERY_VALUE_REL)
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ADE_MIND_CONTROL;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_fMindControlDebuffReduceRate	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_REDUCE_EDT_TIME_REL, iSkillLevel );
						m_fMindControlProcRate			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
						m_fMindControlRecoveryValue		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_RECOVERY_VALUE_REL, iSkillLevel );
					}
				}
			}

			/// 9416 Time Control (official: anim-speed member += SA_ANI_SPEED - 1.0, NOT halved; the proc rate + cooldown value x0.5 in the PvP game-type set {1,4} = {PVP, SQUARE} - our engine has no GT_SQUARE (squares run GT_NONE) and squares cannot cast, so GT_PVP carries the live case - same scope note as ThirdPath_GetScanScope)
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ADE_TIME_CONTROL;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						SetAddAnimSpeedFactor( GetAddAnimSpeedFactor() + ( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ANI_SPEED, iSkillLevel ) - 1.0f ) );	// official: base member +25340 += v(103) - 1.0 (the official helper multiplies base 1.0 + the delta; ours folds the delta additively at state start via X2GUUser.cpp:13640/13651/14194 - same net speed)
						m_fTimeControlProcRate		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
						m_fTimeControlCooldownValue	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_COOLTIME_1, iSkillLevel );
						if ( NULL != g_pX2Game && CX2Game::GT_PVP == g_pX2Game->GetGameType() )	// official dbl_32D1C600 = 0.5 halving (byte-verified)
						{
							m_fTimeControlProcRate		*= 0.5f;
							m_fTimeControlCooldownValue	*= 0.5f;
						}
					}
				}
			}

			/// 9417 Mind Circle (official reads: SA_ADD_MP_VALUE_ABS / SA_ADD_DP_VALUE_ABS - the pip-consumption conversion rates)
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ADE_MIND_CIRCLE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_fMindCircleMpValue	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ADD_MP_VALUE_ABS, iSkillLevel );
						m_fMindCircleDpValue	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ADD_DP_VALUE_ABS, iSkillLevel );
					}
				}
			}

			/// 9418 ESP (official: count = round(v(SA_EFFECTIVE_COUNT)); the learned flag byte = 1)
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ADE_ESP;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_iESPMindBreakCount	= (int)( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillLevel ) + 0.5f );
						m_fESPProcRate			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
						m_fESPDamagedRel		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillLevel );
						m_bESPMidairPhaseShift	= true;
					}
				}
			}
		}	// NO break - the official DE case falls through to LABEL_157 (the shared TT/DE reads of 9414/9404)
#endif
#ifdef X2_ADD_3RD_PATH
	case CX2Unit::UC_ADD_TIME_TRACER:
		{
			/// 9414 Nasod Armor Mode - Overlimit (official LABEL_157 reads: flag byte = 1; SA_MP_GAIN_ON_HIT_REL / SA_MP_GAIN_GET_HIT_REL / SA_DP_GAIN_REL / SA_DP_USE_REL / SA_DAMAGE_REL (shared m_fNaCommandDamageRate) / SA_COOLTIME_1)
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_ATT_NASOD_ARMOR_MODE_OVERLIMIT;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						m_bOverlimitEnabled			= true;
						m_fOverlimitMpGainOnHit		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevel );
						m_fOverlimitMpGainGetHit	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_GET_HIT_REL, iSkillLevel );
						m_fOverlimitDpGain			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DP_GAIN_REL, iSkillLevel );
						m_fOverlimitDpUse			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DP_USE_REL, iSkillLevel );
						m_fNaCommandDamageRate		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
						m_fOverlimitCooltime		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_COOLTIME_1, iSkillLevel );
					}
				}
			}

			/// 9404 Infinite Library (official: the level cache is written UNCONDITIONALLY; the force-down pair adjusts via CalculateIncreasingRate when learned - identical shape to the PT case above)
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_APT_LIBRARY_OF_LIMITLESS;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				m_iLibraryOfLimitlessLevel = iSkillLevel;
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						const float fAddForceDownRate = CalculateIncreasingRate(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_REL, iSkillLevel ));
						m_SkillRelatedData.SetForceDownValueRateOfMeleeAttack( m_SkillRelatedData.GetForceDownValueRateOfMeleeAttack() + fAddForceDownRate );
						m_SkillRelatedData.SetForceDownValueRateOfRangeAttack( m_SkillRelatedData.GetForceDownValueRateOfRangeAttack() + fAddForceDownRate );
					}
				}
			}
		} break;
#endif
	//}} 2026-09-07 //
#endif
	default:
		break;
	}
}
/*virtual*/ void CX2GUAdd::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
{
	if ( iSlotIndex_ < 0 || static_cast<size_t>( iSlotIndex_ ) >= ARRAY_SIZE( _CONST_GUUSER_::aNormalSkillStateID ) )
		return;

	int iNormalStateID = ( bSlotB_ == false ? _CONST_GUUSER_::aNormalSkillStateID[iSlotIndex_] : _CONST_GUUSER_::aNormalSkillStateIDSlotB[iSlotIndex_] );
	UserUnitStateData& normalStateData = m_StateList[ iNormalStateID ];
	normalStateData.Init();
	normalStateData.stateID = iNormalStateID;

	int iHyperStateID = ( bSlotB_ == false ? _CONST_GUUSER_::aHyperSkillStateID[iSlotIndex_] : _CONST_GUUSER_::aHyperSkillStateIDSlotB[iSlotIndex_] );
	UserUnitStateData& hyperStateData  = m_StateList[ iHyperStateID ];
	hyperStateData.Init();
	hyperStateData.m_bHyperState = true;
	hyperStateData.stateID = iHyperStateID;

	const CX2SkillTree::SkillTemplet* pSkillTempletEquipped = GetEquippedActiveSkillTemplet( eSkillID_ );
	if( NULL == pSkillTempletEquipped )
		return;

	/// ��� ĳ���� ���� ��ų ����
	//////////////////////////////////////////////////////////////////////////
	switch( pSkillTempletEquipped->m_eType )
	{
	case CX2SkillTree::ST_BUFF:
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH

		switch ( eSkillID_ )
		{
		case CX2SkillTree::SI_SA_COMMON_CREST_OF_TIME:
			{
				m_LuaManager.MakeTableReference( "ANSI_SA_COMMON_CREST_OF_TIME", normalStateData.stateID );
				normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_COMMON_CREST_OF_TIME_StateInit );
				normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_COMMON_CREST_OF_TIME_FrameMove );
				normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_ThirdPath_OneShotGroundPinned_EventProcess );

				m_LuaManager.MakeTableReference( "ANSI_SA_COMMON_CREST_OF_TIME", hyperStateData.stateID );
				hyperStateData.m_bHyperState		= true;
				hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;	/// official: hyper state shares FM+EP copies (no StateInit)
				hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
				return;
			} break;
		default:
			break;
		}
		//}} 2026-09-01 MARCH2015 Crest of Time (DEC_A) //
#endif
		{	// 占쏙옙占쏙옙
#else
		{	// ����
#endif
			m_LuaManager.MakeTableReference( "ANSI_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_COMMON_BUFF", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

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
					m_LuaManager.MakeTableReference( "ANSI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "ANSI_THROW_ITEM", hyperStateData.stateID );
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

	switch ( eSkillID_ )
	{
	/// ����� ��Ƽ��
	//////////////////////////////////////////////////////////////////////////

	/// ���̵� �극��Ŀ
	case CX2SkillTree::SI_SA_AN_VOID_BREAKER:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AN_VOID_BREAKER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_VOID_BREAKER_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_VOID_BREAKER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_VOID_BREAKER_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AN_VOID_BREAKER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// ��ƼŬ ������
	case CX2SkillTree::SI_SA_AN_PARTICLE_PRISM:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AN_PARTICLE_PRISM", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_PARTICLE_PRISM_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_PARTICLE_PRISM_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_PARTICLE_PRISM_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AN_PARTICLE_PRISM", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	/// ������ ��
	case CX2SkillTree::SI_SA_AN_ENERGY_BOOM:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AN_ENERGY_BOOM", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_ENERGY_BOOM_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_ENERGY_BOOM_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_ENERGY_BOOM_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AN_ENERGY_BOOM", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	/// ��ƼŬ �׼���������
	case CX2SkillTree::SI_SA_AN_PARTICLE_ACCELERATOR:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AN_PARTICLE_ACCELERATOR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_PARTICLE_ACCELERATOR_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_PARTICLE_ACCELERATOR_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_PARTICLE_ACCELERATOR_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AN_PARTICLE_ACCELERATOR", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	/// ��Ʈ�� ��
	case CX2SkillTree::SI_SA_AN_NEUTRON_BOMB:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AN_NEUTRON_BOMB", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_NEUTRON_BOMB_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AN_NEUTRON_BOMB_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AN_NEUTRON_BOMB", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	/// �޽� ĳ��
	case CX2SkillTree::SI_SA_APT_PULSE_CANNON:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_APT_PULSE_CANNON_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_READY_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_READY_StateStartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_READY_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_READY_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_APT_PULSE_CANNON_READY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_APT_PULSE_CANNON_LOOP;
				m_LuaManager.MakeTableReference( "ANSI_SA_APT_PULSE_CANNON_LOOP", stateData.stateID );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_LOOP_FrmaeMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_APT_PULSE_CANNON_FINISH;
				m_LuaManager.MakeTableReference( "ANSI_SA_APT_PULSE_CANNON_FINISH", stateData.stateID );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_PULSE_CANNON_FINISH_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	/// EMP ��ũ
	case CX2SkillTree::SI_SA_APT_EMP_SHOCK:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_APT_EMP_SHOCK", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_EMP_SHOCK_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_EMP_SHOCK_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_EMP_SHOCK_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_APT_EMP_SHOCK", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	/// ����ũ ������
	case CX2SkillTree::SI_SA_APT_QUAKE_BUSTER:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_APT_QUAKE_BUSTER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_QUAKE_BUSTER_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_QUAKE_BUSTER_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_QUAKE_BUSTER_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_APT_QUAKE_BUSTER_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_QUAKE_BUSTER_HYPER_EventProcess );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;

	/// �����̽ý� �ʵ�
	case CX2SkillTree::SI_SA_APT_STASIS_FIELD:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_APT_STASIS_FIELD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_APT_STASIS_FIELD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_APT_STASIS_FIELD_ACTIVE;
				m_LuaManager.MakeTableReference( "ANSI_SA_APT_STASIS_FIELD_ACTIVE", stateData.stateID );
				stateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_ACTIVE_StateStart );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_ACTIVE_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_STASIS_FIELD_ACTIVE_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	/// ��Ŀ��
	case CX2SkillTree::SI_SA_APT_CONQUEROR:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_APT_CONQUEROR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_CONQUEROR_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_CONQUEROR_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_APT_CONQUEROR_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_APT_CONQUEROR_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;

		/// ������ ������
	case CX2SkillTree::SI_SA_ALP_REVERSE_REACTOR:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_REVERSE_REACTOR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_REVERSE_REACTOR_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_REVERSE_REACTOR_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_REVERSE_REACTOR_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_REVERSE_REACTOR_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_REVERSE_REACTOR", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// ���ǹ� �׼�
	case CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_QUICKSILVER_ACCEL", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_StateEnd );

			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_QUICKSILVER_ACCEL", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK;
				m_LuaManager.MakeTableReference( "ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK", stateData.stateID );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_EventProcess );
				stateData.StateEnd				= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_StateEnd );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

		/// ���ǹ� ������
	case CX2SkillTree::SI_SA_ALP_QUICKSILVER_FRENZY:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_QUICKSILVER_FRENZY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_FRENZY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_FRENZY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_QUICKSILVER_FRENZY_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_QUICKSILVER_FRENZY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// ����Ʈ �Ƴ����
	case CX2SkillTree::SI_SA_ALP_DUST_ANATOMY:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_DUST_ANATOMY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_DUST_ANATOMY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_DUST_ANATOMY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_DUST_ANATOMY_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_DUST_ANATOMY_StateEnd );

			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_DUST_ANATOMY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		} break;

		/// ����ű ����
	case CX2SkillTree::SI_SA_ALP_PSYCHIC_STORM:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_PSYCHIC_STORM", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_StateStartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_ALP_PSYCHIC_STORM", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_ALP_PSYCHIC_STORM_LOOP;
				m_LuaManager.MakeTableReference( "ANSI_SA_ALP_PSYCHIC_STORM_LOOP", stateData.stateID );
				stateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_LOOP_StateStart );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_LOOP_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_ALP_PSYCHIC_STORM_FINISH;
				m_LuaManager.MakeTableReference( "ANSI_SA_ALP_PSYCHIC_STORM_FINISH", stateData.stateID );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_FINISH_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ALP_PSYCHIC_STORM_FINISH_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}

		} break;


	/// ������ ��Ƽ��
	//////////////////////////////////////////////////////////////////////////
		/// ���ǹ� ������
	case CX2SkillTree::SI_HA_ALP_DOOMS_DAY:
		{
			m_LuaManager.MakeTableReference( "ANSI_HA_ALP_DOOMS_DAY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ALP_DOOMS_DAY_Init );

			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ALP_DOOMS_DAY_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ALP_DOOMS_DAY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ALP_DOOMS_DAY_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ALP_DOOMS_DAY_StateEnd );

			m_LuaManager.MakeTableReference( "ANSI_HA_ALP_DOOMS_DAY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		} break;


	/// ��Ƽ��
	//////////////////////////////////////////////////////////////////////////

	/// ����ź
	case CX2SkillTree::SI_A_AN_PULSE_BULLET:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AN_PULSE_BULLET", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AN_PULSE_BULLET", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY;
				m_LuaManager.MakeTableReference( "ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY", stateData.stateID );
				stateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_StateStartFuture );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_EventProcess );
				stateData.StateEndFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_StateEndFuture );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE;
				m_LuaManager.MakeTableReference( "ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE", stateData.stateID );
				stateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_StateStart );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE;
				m_LuaManager.MakeTableReference( "ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE", stateData.stateID );
				stateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_StateStartFuture );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

		/// ���� �ر�
	case CX2SkillTree::SI_A_AN_MIND_BREAK:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AN_MIND_BREAK", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_MIND_BREAK_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_MIND_BREAK_FrmaeMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_MIND_BREAK_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AN_MIND_BREAK", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// ���� ��ȭ
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AN_PHASE_SHIFT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PHASE_SHIFT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PHASE_SHIFT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PHASE_SHIFT_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AN_PHASE_SHIFT", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_A_AN_PHASE_SHIFT_BLINK;
				m_LuaManager.MakeTableReference( "ANSI_A_AN_PHASE_SHIFT_BLINK", stateData.stateID );
				stateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PHASE_SHIFT_BLINK_StateStart );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AN_PHASE_SHIFT_BLINK_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

		/// ���̳ʸ� ���� - ���׳�Ʈ��
	case CX2SkillTree::SI_A_APT_MAGNETRON_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_APT_MAGNETRON_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_MAGNETRON_DYNAMO_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_MAGNETRON_DYNAMO_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_MAGNETRON_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_APT_MAGNETRON_DYNAMO", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// ���̳ʸ� ���� - ������
	case CX2SkillTree::SI_A_APT_DESOLVER_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_APT_DESOLVER_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_DESOLVER_DYNAMO_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_DESOLVER_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_APT_DESOLVER_DYNAMO_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_DESOLVER_DYNAMO_EventProcess );
		} break;

		/// ���̳ʸ� ���� - ���Ϸ�
	case CX2SkillTree::SI_A_APT_PYLON_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_APT_PYLON_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_PYLON_DYNAMO_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_PYLON_DYNAMO_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_APT_PYLON_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_APT_PYLON_DYNAMO", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// ���̿��� ��Ʈ
	case CX2SkillTree::SI_A_ALP_PSIONIC_BEAT:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_ALP_PSIONIC_BEAT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_ALP_PSIONIC_BEAT_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ALP_PSIONIC_BEAT_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_ALP_PSIONIC_BEAT", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		/// Ʈ�� ����
	case CX2SkillTree::SI_A_ALP_TWIRL_RUSH:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_ALP_TWIRL_RUSH", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_ALP_TWIRL_RUSH_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ALP_TWIRL_RUSH_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_ALP_TWIRL_RUSH_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	default:
		break;

#ifdef X2_ADD_2ND_PATH
	case CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;
	case CX2SkillTree::SI_SA_AAT_PANZER_BUSTER:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PANZER_BUSTER_START", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_START_Init );

			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_START_StateStartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_START_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_START_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PANZER_BUSTER_START", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;

			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_AAT_PANZER_BUSTER_AIMING;
				m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PANZER_BUSTER_AIMING", stateData.stateID );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_AIMING_EventProcess );

				stateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_AIMING_StateStart );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_AIMING_FrameMove );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP;
				m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP", stateData.stateID );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_EventProcess );

				stateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_StateStartFuture );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_FrameMove );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN;
				m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN", stateData.stateID );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_EventProcess );

				stateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_StateStartFuture );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_FrameMove );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID				= ANSI_SA_AAT_PANZER_BUSTER_FIRE;
				m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PANZER_BUSTER_FIRE", stateData.stateID );

			stateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateStartFuture );
			stateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateStart );
			stateData.StateEnd				= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateEnd );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_FIRE_EventProcess );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PANZER_BUSTER_FIRE_FrameMove );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_SA_AAT_PSIONIC_GENERATOR:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PSIONIC_GENERATOR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PSIONIC_GENERATOR_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PSIONIC_GENERATOR_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PSIONIC_GENERATOR_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PSIONIC_GENERATOR", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_AAT_PHANTOM_SEEKER:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PHANTOM_SEEKER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PHANTOM_SEEKER_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AAT_PHANTOM_SEEKER_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_SA_AAT_PHANTOM_SEEKER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_AAT_RUSHING_DRONE_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AAT_RUSHING_DRONE_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AAT_RUSHING_DRONE_DYNAMO_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AAT_RUSHING_DRONE_DYNAMO_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AAT_RUSHING_DRONE_DYNAMO_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AAT_RUSHING_DRONE_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AAT_RUSHING_DRONE_DYNAMO", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;	//{{ per-cast flags must set in hyper too //
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
#endif

#ifdef X2_ADD_2ND_PATH
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_PULSE_GATLING", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_PULSE_GATLING", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_SHOOTING_CHASER:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL:
		{
			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_STAR_FALL", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_STAR_FALL_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_STAR_FALL_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_AMM_INSTALLATION_STAR_FALL_FrameMove );

			m_LuaManager.MakeTableReference( "ANSI_SA_AMM_INSTALLATION_STAR_FALL", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;
	case CX2SkillTree::SI_A_AMM_DIVIDING_MISSILE_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_AMM_FLICK_DISC_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AMM_FLICK_DISC_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_FLICK_DISC_DYNAMO_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_FLICK_DISC_DYNAMO_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_FLICK_DISC_DYNAMO_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_FLICK_DISC_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AMM_FLICK_DISC_DYNAMO", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_AMM_COUNTDOWN_BOMB_DYNAMO:
		{
			m_LuaManager.MakeTableReference( "ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_HA_AMM_APOCALYPSE:
		{
			m_LuaManager.MakeTableReference( "ANSI_HA_AMM_APOCALYPSE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_AMM_APOCALYPSE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_AMM_APOCALYPSE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_HA_AMM_APOCALYPSE_EventProcess );

			m_LuaManager.MakeTableReference( "ANSI_HA_AMM_APOCALYPSE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
#endif

#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#pragma region ADD_3RD_PATH_SKILLS
case CX2SkillTree::SI_SA_ATT_VOID_IMPACT:
	{

		ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ATT_VOID_IMPACT" );
		normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_IMPACT_StateInit );
		normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_IMPACT_FrameMove );
		normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_ThirdPath_OneShotGroundPinned_EventProcess );

		ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ATT_VOID_IMPACT" );
		hyperStateData.m_bHyperState	= true;
		hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_IMPACT_FrameMove );
		hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
	} break;
	case CX2SkillTree::SI_SA_ATT_VOID_FIELD:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ATT_VOID_FIELD" );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_StateInit );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_EventProcess );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ATT_VOID_FIELD" );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_StartFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_EventProcess );

			UserUnitStateData& voidFieldLoopState = m_StateList[ANSI_SA_ATT_VOID_FIELD_LOOP];
			ANSI_ThirdPath_CreateState( voidFieldLoopState, ANSI_SA_ATT_VOID_FIELD_LOOP, "ANSI_SA_ATT_VOID_FIELD_LOOP" );
			voidFieldLoopState.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_LOOP_FrameMove );
			voidFieldLoopState.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_LOOP_EventProcess );

			UserUnitStateData& voidFieldEndState = m_StateList[ANSI_SA_ATT_VOID_FIELD_END];
			ANSI_ThirdPath_CreateState( voidFieldEndState, ANSI_SA_ATT_VOID_FIELD_END, "ANSI_SA_ATT_VOID_FIELD_END" );
			voidFieldEndState.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_END_FrameMove );
			voidFieldEndState.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_VOID_FIELD_END_EventProcess );
		} break;
	case CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ATT_MAXIMUM_STRIKE" );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_MAXIMUM_STRIKE_StateInit );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_MAXIMUM_STRIKE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_MAXIMUM_STRIKE_EventProcess );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ATT_MAXIMUM_STRIKE" );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_MAXIMUM_STRIKE_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_MAXIMUM_STRIKE_EventProcess );
		} break;
	case CX2SkillTree::SI_SA_ATT_STARDUST_SHOWER:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ATT_STARDUST_SHOWER" );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_StateInit );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_StartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_EventProcess );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ATT_STARDUST_SHOWER" );
			hyperStateData.m_bHyperState			= true;
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_StartFuture );
			hyperStateData.StateStart				= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_StateStart );
			hyperStateData.OnEventProcess			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_EventProcess );

			UserUnitStateData& stardustLoopState = m_StateList[ANSI_SA_ATT_STARDUST_SHOWER_LOOP];
			ANSI_ThirdPath_CreateState( stardustLoopState, ANSI_SA_ATT_STARDUST_SHOWER_LOOP, "ANSI_SA_ATT_STARDUST_SHOWER_LOOP" );
			stardustLoopState.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StartFuture );
			stardustLoopState.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StateStart );
			stardustLoopState.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_LOOP_FrameMove );
			stardustLoopState.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_LOOP_EventProcess );
			stardustLoopState.StateEnd				= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StateEnd );

			UserUnitStateData& stardustEndState = m_StateList[ANSI_SA_ATT_STARDUST_SHOWER_END];
			ANSI_ThirdPath_CreateState( stardustEndState, ANSI_SA_ATT_STARDUST_SHOWER_END, "ANSI_SA_ATT_STARDUST_SHOWER_END" );
			stardustEndState.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_END_FrameMove );
			stardustEndState.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_END_EventProcess );
			stardustEndState.StateEnd			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ATT_STARDUST_SHOWER_END_StateEnd );
		} break;
case CX2SkillTree::SI_SA_ADE_REVERSE_CIRCLE:
{

	ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ADE_REVERSE_CIRCLE" ); normalStateData.StateInit = SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_CIRCLE_StateInit );
	ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ADE_REVERSE_CIRCLE" );
	hyperStateData.m_bHyperState = true;
} break;
	case CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ADE_REVERSE_STIGMA" );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_StateInit );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_RETURN_EventProcess );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ADE_REVERSE_STIGMA" );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_RETURN_EventProcess );

			UserUnitStateData& stigmaReturnState = m_StateList[ANSI_SA_ADE_REVERSE_STIGMA_RETURN];
			ANSI_ThirdPath_CreateState( stigmaReturnState, ANSI_SA_ADE_REVERSE_STIGMA_RETURN, "ANSI_SA_ADE_REVERSE_STIGMA_RETURN" );
			stigmaReturnState.OnFrameMove	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_RETURN_FrameMove );
			stigmaReturnState.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_REVERSE_STIGMA_RETURN_EventProcess );
		} break;
	case CX2SkillTree::SI_SA_ADE_MOONLIGHT_RHAPSODY:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ADE_MOONLIGHT_RHAPSODY" );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_MOONLIGHT_RHAPSODY_StateInit );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_MOONLIGHT_RHAPSODY_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_MOONLIGHT_RHAPSODY_EventProcess );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ADE_MOONLIGHT_RHAPSODY" );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_MOONLIGHT_RHAPSODY_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_MOONLIGHT_RHAPSODY_EventProcess );
		} break;
case CX2SkillTree::SI_SA_ADE_GRAVITY_BURSTER:
	{

		ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_SA_ADE_GRAVITY_BURSTER" );
		normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_GRAVITY_BURSTER_StateInit );
		normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_GRAVITY_BURSTER_FrameMove );

		ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_SA_ADE_GRAVITY_BURSTER" );
		hyperStateData.m_bHyperState	= true;
		hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_SA_ADE_GRAVITY_BURSTER_FrameMove );
	} break;
	case CX2SkillTree::SI_HA_ADE_PARANOIA:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_HA_ADE_PARANOIA" );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ADE_PARANOIA_StateInit );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, ANSI_HA_ADE_PARANOIA_FrameMove );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_HA_ADE_PARANOIA" );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
		} break;
	case CX2SkillTree::SI_A_ATT_SPACE_CRACK:
		{

			ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_A_ATT_SPACE_CRACK_START" );
			normalStateData.StateInit				= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_StateInit );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_EventProcess );

			ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_A_ATT_SPACE_CRACK_START" );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_StartFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_EventProcess );

			UserUnitStateData& crackMoveState = m_StateList[ANSI_A_ATT_SPACE_CRACK_MOVE];
			ANSI_ThirdPath_CreateState( crackMoveState, ANSI_A_ATT_SPACE_CRACK_MOVE, "ANSI_A_ATT_SPACE_CRACK_MOVE" );
			crackMoveState.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_MOVE_EventProcess );

			UserUnitStateData& crackWaitState = m_StateList[ANSI_A_ATT_SPACE_CRACK_WAIT];
			ANSI_ThirdPath_CreateState( crackWaitState, ANSI_A_ATT_SPACE_CRACK_WAIT, "ANSI_A_ATT_SPACE_CRACK_WAIT" );
			crackWaitState.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_WAIT_EventProcess );

			UserUnitStateData& crackEndState = m_StateList[ANSI_A_ATT_SPACE_CRACK_END];
			ANSI_ThirdPath_CreateState( crackEndState, ANSI_A_ATT_SPACE_CRACK_END, "ANSI_A_ATT_SPACE_CRACK_END" );
			crackEndState.StateStartFuture		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_END_StartFuture );
			crackEndState.StateStart			= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_END_StateStart );
			crackEndState.OnEventProcess		= SET_CB_FUNC( CX2GUAdd, ANSI_A_ATT_SPACE_CRACK_END_EventProcess );
		} break;
case CX2SkillTree::SI_A_ADE_FORCE_NOVA:
	{

		ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_A_ADE_FORCE_NOVA" );
		normalStateData.StateInit	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_NOVA_StateInit );
		normalStateData.StateStart	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_NOVA_StateStart );
		normalStateData.OnFrameMove	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_NOVA_FrameMove );

		ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_A_ADE_FORCE_NOVA" );
		hyperStateData.m_bHyperState	= true;
		hyperStateData.StateStart	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_NOVA_StateStart );
		hyperStateData.OnFrameMove	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_NOVA_FrameMove );
	} break;
case CX2SkillTree::SI_A_ADE_FORCE_FINGER:
	{

		ANSI_ThirdPath_CreateState( normalStateData, normalStateData.stateID, "ANSI_A_ADE_FORCE_FINGER" );
		normalStateData.StateInit	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_FINGER_StateInit );
		normalStateData.OnFrameMove	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_FINGER_FrameMove );
		normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_ThirdPath_OneShotGroundPinned_EventProcess );

		ANSI_ThirdPath_CreateState( hyperStateData, hyperStateData.stateID, "ANSI_A_ADE_FORCE_FINGER" );
		hyperStateData.m_bHyperState	= true;
		hyperStateData.OnFrameMove	= SET_CB_FUNC( CX2GUAdd, ANSI_A_ADE_FORCE_FINGER_FrameMove );
		hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
	} break;
#pragma endregion ADD_3RD_PATH_SKILLS
#endif
	}
}

/** @function	: SetEquippedSkillLevel
	@brief		: ��ų �� �߰��Ǵ� ������Ʈ�� ���� ��ų ���� ����
	@param		: ��ų ���̵�, ��� ������Ʈ ���� ����
*/
/*virtual*/ void CX2GUAdd::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	/// �����̽ý� �ʵ�
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_APT_STASIS_FIELD == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_APT_STASIS_FIELD );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_APT_STASIS_FIELD_ACTIVE );
		}
	}

	/// ���� ��ȭ
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AN_PHASE_SHIFT == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AN_PHASE_SHIFT );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_AN_PHASE_SHIFT_BLINK );
		}
	}

	/// ����ź
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AN_PULSE_BULLET == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AN_PULSE_BULLET );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE );
		}
	}

	/// �޽� ĳ��
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_APT_PULSE_CANNON == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_APT_PULSE_CANNON );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_APT_PULSE_CANNON_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_APT_PULSE_CANNON_FINISH );
		}
	}

	/// ���ǹ� �׼�
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK );
		}
	}

	/// ����ű ����
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALP_PSYCHIC_STORM == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALP_PSYCHIC_STORM );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ALP_PSYCHIC_STORM_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ALP_PSYCHIC_STORM_FINISH );
		}
	}
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


#ifdef X2_ADD_2ND_PATH
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AAT_PANZER_BUSTER == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AAT_PANZER_BUSTER );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_AAT_PANZER_BUSTER_AIMING );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_AAT_PANZER_BUSTER_FIRE );
		}
	}
#endif

#ifdef X2_ADD_3RD_PATH
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ATT_SPACE_CRACK == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ATT_SPACE_CRACK );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_ATT_SPACE_CRACK_MOVE );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_ATT_SPACE_CRACK_WAIT );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_A_ATT_SPACE_CRACK_END );
		}
	}

	/// VOID FIELD
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ATT_VOID_FIELD == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ATT_VOID_FIELD );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ATT_VOID_FIELD_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ATT_VOID_FIELD_END );
		}
	}

	/// STARDUST SHOWER
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ATT_STARDUST_SHOWER == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ATT_STARDUST_SHOWER );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ATT_STARDUST_SHOWER_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ATT_STARDUST_SHOWER_END );
		}
	}
#endif

#ifdef X2_ADD_3RD_PATH
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA );

		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ANSI_SA_ADE_REVERSE_STIGMA_RETURN );
		}
	}
#endif
	//}} 2026-09-04 X2GUAdd reverify REG-5 //
#endif
}

/*virtual*/ void CX2GUAdd::CommonStateStartFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStartFuture();

	/// ���ҵ� �踮� ������ �� �ִ��� ����
	LUA_GET_VALUE( m_LuaManager, "POSSIBLE_NASOD_BARRIER", m_bPossibleNasodArmorModeState, false );

	/// ���ҵ� �踮� ������ �� �ִ� Ŭ���� ���� ����
	if ( true == m_LuaManager.BeginTable( "POSSIBLE_NASOD_BARRIER_BY_UNIT_CLASS" ) )
	{
		int	iIndex		= 1;
		int	iUnitClass	= 0;

		while( true == m_LuaManager.GetValue( iIndex, iUnitClass ) )
		{
			CX2Unit::UNIT_CLASS eUnitClass = static_cast<CX2Unit::UNIT_CLASS>( iUnitClass );

			if ( GetUnitClass() == eUnitClass )
			{
				m_bPossibleNasodArmorModeState = true;
				break;
			}

			++iIndex;
		}

		m_LuaManager.EndTable();
	}
}
/*virtual*/ void CX2GUAdd::CommonStateStart()
{
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


#ifdef X2_ADD_2ND_PATH

	if ( 0 != m_iFactoryMarkedSkillID
		&& true == ThirdPath_IsInstallFamilySkill( (int)m_NowStateData.m_eSkillID )
		&& (int)m_NowStateData.m_eSkillID == m_iFactoryMarkedSkillID
		&& CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY != m_NowStateData.m_eSkillID
		&& NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() && NULL != g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hFactoryDroneDE ) )
	{
		m_iFactoryPhase		= 3;	// drone attacking
		m_bFactoryFlagWait	= true;
		ThirdPath_FactoryDronePlayMotion( L"Attack" );	// official: the drone plays its Attack motion
	}
	//}} 2026-09-04 X2GUAdd reverify FACT-9 //
	if ( NULL != g_pX2Game && INVALID_EFFECTSET_HANDLE != m_hFactoryMiscES )
		g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hFactoryMiscES );
#endif
	//}} 2026-09-04 X2GUAdd reverify LIFE-8 //
#endif
	CX2GUUser::CommonStateStart();
}

/*virtual*/ void CX2GUAdd::CommonFrameMoveFuture()
{
	KTDXPROFILE();

	/// ���� ��Ҵٸ�, ���� ��� �ʱ�ȭ
	if( true == IsOnSomethingFuture() && false == m_bEnableAirDash )
		m_bEnableAirDash = true;

	/// ���� ��Ҵٸ�, ���� �ξ� �ʱ�ȭ
	if( true == IsOnSomethingFuture() && false == m_bEnableLevitationWait )
		m_bEnableLevitationWait = true;

	/// DP �� ���� ��ġ ����ȭ
	m_FrameDataFuture.syncData.m_CannonBallCount = GetAbilitySyncData();

	/// ȯ�� ������ Ÿ�� ���� ����
	ProcessFantasyTracerTargetInfo();

#ifdef X2_ADD_3RD_PATH

	if ( true == m_bSpaceCrackActive )
	{
		const int iNowState = GetNowStateID();
		if ( iNowState != ANSI_A_ATT_SPACE_CRACK_MOVE && iNowState != ANSI_A_ATT_SPACE_CRACK_WAIT
			&& CX2SkillTree::SI_A_ATT_SPACE_CRACK != m_NowStateData.m_eSkillID )
		{
			m_bSpaceCrackActive = false;
		}
	}
	//}} 2026-09-05 //
#endif

	CX2GUUser::CommonFrameMoveFuture();
}

/*virtual*/ void CX2GUAdd::CommonFrameMove()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMove();

	/// Ÿ Ŭ���̾�Ʈ�� DP �� ���� ��ġ ����ȭ
	if ( false == IsMyUnit() )
	{
		SetAbilitySyncData( static_cast<USHORT>( m_FrameDataNow.syncData.m_CannonBallCount ) );
	}

	/// ���̳ʸ� ���� - ���Ϸ� �����ð� ����
	ProcessPylonDamageEffect();
	/// ���̳ʸ� ���� - ���Ϸ� ���� ��ȯ ���� ��Ÿ�� ����
	ProcessNonstopSummonPylon();

	/// ���̳ʸ� ���� - ���׳�Ʈ�� ��ġ ���� �Լ�
	ProcessMagnetronDamageEffect();

	/// �����̽ý� �ʵ� ��Ÿ�� ���� �Լ�
	ProcessStasisField();

	/// �̸��� ��ǳ ��Ÿ�� ����
	ProcessDustStormCoolTime();

	/// �����Ӵ� �ѹ��� DP ȸ�� �� �Ҹ� ó�� Flag
	SetUpDPByAttackThisFrame( false );
	SetUpDPByDamageThisFrame( false );

	/// ��� ��ȯ�� DP�� �˻� �� ���� ���� �Լ�
	ProcessPowerExchanger();

	/// ������ ������ ��� ���� �Լ�
	ProcessReverseReactorTargetInfo();

	/// ���ǹ� ������ �߻� ���� ���� �Լ�
	ProcessQuicksilverFrenzy();
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


#ifdef X2_ADD_2ND_PATH

	if ( 0 != m_iFactoryMarkedSkillID || 0 != m_iFactoryPhase )
		ThirdPath_OnFactoryFrameMove( m_fElapsedTime );
	//}} 2026-08-30 MARCH2015 batch6 //

	if ( false == m_vecUltFuryDrones.empty() || 0 != m_iUltFurySound )
		ThirdPath_UltimateFuryAttackDriver();
	//}} 2026-09-06 MARCH2015 Ultimate Fury attack driver (IDA L4) callsite //
#endif

#ifdef X2_ADD_2ND_PATH

	ThirdPath_DynamoLockTimeout( m_fElapsedTime );
	//}} 2026-09-04 X2GUAdd reverify LOCK-1 //
#endif

#ifdef X2_ADD_2ND_PATH

	if ( m_bApocalypseSummoned && false == m_bApocalypseFinisherDone && false == ThirdPath_IsApocalypseSummonActive() )
	{
		ThirdPath_ApocalypseFinishSummon();
		ThirdPath_ApocalypseClearMarker();
	}
	//}} 2026-08-30 MARCH2015 batch5 //
#endif


	if ( 0.f < m_fStigmaMarkTimer )
		ThirdPath_OnStigmaMarkFrameMove( m_fElapsedTime );
	//}} //


	if ( 0.f < m_fMaxStrikeWindowRemain )
		ThirdPath_OnMaxStrikeFrameMove( m_fElapsedTime );
	//}} 2026-09-05 //
#ifdef X2_ADD_3RD_PATH

	if ( true == m_bMoonlightActive )
		ThirdPath_OnMoonlightWaveFrameMove( m_fElapsedTime );
	//}} 2026-09-01 MARCH2015 Moonlight Rhapsody (DEC_B) //
#endif

	if ( true == m_bOverlimitEnabled )
		ThirdPath_OverlimitCoolListTick( m_fElapsedTime );
	//}} 2026-09-07 //
	//}} 2026-09-04 X2GUAdd reverify LIFE-2 //

#ifdef X2_ADD_3RD_PATH

	UpdateDEFormationFace();
	//}} 2026-09-07 //
#endif
#endif
}

/*virtual*/ void CX2GUAdd::CommonStateEndFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEndFuture();
}

/*virtual*/ void CX2GUAdd::CommonStateEnd()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEnd();
}

/*virtual*/ void CX2GUAdd::CommonEventProcess()
{
	KTDXPROFILE();

	/// ���� ����� �� DP �������� 0�� �Ǹ�,  ���� ��� Ǯ�� �ּ���.
	if ( true == GetIsFormationMode() && 0.f >= GetDPValue() )
	{
		SetIsFormationMode( false );
		m_FrameDataFuture.syncData.nowAction = UAI_RELEASE_FORMATION_MODE;
	}

	CX2GUUser::CommonEventProcess();
}

/*virtual*/ void CX2GUAdd::DetonationStateChange( CX2DamageManager::DamageData* pDamageData )
{
	CX2GUUser::DetonationStateChange( pDamageData );
}

/*virtual*/ RENDER_HINT	CX2GUAdd::CommonRender_Prepare()
{
	KTDXPROFILE();

	return CX2GUUser::CommonRender_Prepare();
}

/*virtual*/ bool CX2GUAdd::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition_ /*= CX2SkillTree::ASUT_GROUND */)
{
	KTDXPROFILE();

	if ( false == CanUseSpecialAttack( eActiveSkillUseCondition_ ) )
		return false;

	if( NULL == m_pSkillTemplet )
	{
		return false;
	}
	else
	{
		StateChangeSpecialAttack( m_pSkillTemplet );
	}

	return true;

}
/** @function	: NoStateChangeActionFrameMove
	@brief		: ������Ʈ�� ��ȭ�� ���� ��ų ��� �� ����Ǵ� FrameMove
*/
/*virtual*/ void CX2GUAdd::NoStateChangeActionFrameMove()
{
	switch( m_FrameDataNow.syncData.nowAction )
	{
	/// �ֵ� ���� ��� ����
	case UAI_RELEASE_FORMATION_MODE:
		{
			EraseBuffTempletFromGameUnit( BTI_FORMATION_MODE );
			EraseBuffTempletFromGameUnit( BTI_BUFF_LIBRARY_OF_LIMITLESS_MANA_RECOVERY );
			SetRemainHyperModeTime( 0.f );
			/// UpMutationCount( 1 );	������ ���� ����Ʈ ȹ�� ����

			/// �ʿ����� ��ü - ���� ����� �̵� �ӵ� ���� ����
			m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() / m_fBodyOfTranscendenceMoveSpeed );
			m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() / m_fBodyOfTranscendenceMoveSpeed );
		} break;
	case UAI_ACTIVE_STASIS_FIELD:
		{
			if ( true == IsMyUnit() && true == m_StasisfieldData.GetIsActiveStasisfield() )
				StateChange( ANSI_SA_APT_STASIS_FIELD_ACTIVE );
		} break;
	default:
		break;
	}
}

/*virtual*/ float CX2GUAdd::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = 0.f;

#ifdef X2_ADD_3RD_PATH
	/// 占쏟각쇽옙占쏙옙 MP占쏙옙 占쏙옙占쏙옙, 占쏙옙占쏙옙占쏙옙 DP占쏙옙 占쏙옙占쏙옙 占쏙옙킬占쏙옙 占싣닐띰옙占쏙옙 MP 占쌀몌옙 占쏙옙占쏙옙
	float fUseDPSkillAtFormationMode = GetSpecialAbilityValue( CX2SkillTree::SA_DP_USE_REL, eSkillID_ );


	if ( 0.f < fUseDPSkillAtFormationMode )
	{
		const int iUnitClassGate = ( NULL != GetUnit() ) ? (int)GetUnit()->GetClass() : (int)CX2Unit::UC_ADD_TIME_TRACER;
		if ( iUnitClassGate != (int)CX2Unit::UC_ADD_TIME_TRACER && iUnitClassGate != (int)CX2Unit::UC_ADD_DIABOLIC_ESPER
			&& eSkillID_ != CX2SkillTree::SI_SA_APT_PULSE_CANNON )
			fUseDPSkillAtFormationMode = 0.f;
	}
	//}} 2026-09-05 //
#else
	/// �񰢼��� MP�� ����, ������ DP�� ���� ��ų�� �ƴҶ��� MP �Ҹ� ����
	const float fUseDPSkillAtFormationMode = GetSpecialAbilityValue( CX2SkillTree::SA_DP_USE_REL, eSkillID_ );
#endif

	if ( 0.f >= fUseDPSkillAtFormationMode || false == GetIsFormationMode() )
	{
		fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );

		switch ( eSkillID_ )
		{
		case CX2SkillTree::SI_SA_APT_STASIS_FIELD:			/// �����̽ý� �ʵ�
			{
				/// �����̽ý� �ʵ� �������̸�, MP �Ҹ� ���� ���� ������Ʈ�� ��ȯ ����
				if ( true == m_StasisfieldData.GetIsActiveStasisfield() )
					fMpConsumption = 0;
			} break;
		case CX2SkillTree::SI_A_AN_PHASE_SHIFT:				/// ���� ��ȭ
			{
				if ( true == m_PhaseShiftData.IsValidReturnPosValue() )			/// ���� ��ȭ ���ͽ�, ���� MP�� �Ҹ� ����
					fMpConsumption = GetPhaseShiftMPConsume();
				/// ȯ�� ������ �ߵ���, MP �Ҹ� ������.
				else if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PHASE_SHIFT ) )
					fMpConsumption = 0.f;
			} break;

		case CX2SkillTree::SI_A_AN_PULSE_BULLET:			/// ����ź
			{
				/// ȯ�� ������ �ߵ���, MP �Ҹ� ������.
				if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PULSE_BULLET ) )
					fMpConsumption = 0.f;
			} break;

		case CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL:		/// ���ǹ� �׼�
			{
				/// ���ǹ� �׼� �����ÿ��� MP 10�� �Һ�
				if ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == GetFutureStateSkillID())
					fMpConsumption = 10.f;
			} break;
		default:
			break;

#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
		case CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA:
			{
#endif

#ifdef X2_ADD_3RD_PATH
				if ( 0 != m_uidStigmaTarget )
					fMpConsumption = 20.f;
			} break;
#endif

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH
		case CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE:
			{

				if ( false == m_vecMaxStrikeHandles.empty() )
					fMpConsumption = 0.f;
			} break;
		case CX2SkillTree::SI_HA_AMM_APOCALYPSE:
			{

				if ( ThirdPath_IsApocalypseSummonActive() )
					fMpConsumption = 0.f;
			} break;
#endif

#ifdef X2_ADD_2ND_PATH
		case CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_SHOOTING_CHASER:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL:
			{
				if ( m_iFactoryMarkedSkillID == (int)eSkillID_ )
					fMpConsumption = m_fFactoryMP * GetSpecialAbilityValue( CX2SkillTree::SA_MP_CONSUMPTION, eSkillID_ );
			} break;
#endif
#endif
		}
	}

	return fMpConsumption;
}
float CX2GUAdd::GetActualCoolTime( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN int iSkillLevel ) const
{
	if( NULL == pSkillTemplet_ )
		return 0.f;

	float fSkillCoolTime = pSkillTemplet_->GetSkillCoolTimeValue( iSkillLevel ) ;

	switch ( pSkillTemplet_->m_eID )
	{
	case CX2SkillTree::SI_SA_APT_STASIS_FIELD:			/// �����̽ý� �ʵ�
		{
			/// �����̽ý� �ʵ� ������
			if ( true == m_StasisfieldData.GetIsActiveStasisfield() )
				return 0.f;
		} break;
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:				/// ���� ��ȭ
		{
			/// ���� ��ȭ ���ͽ�
			if ( true == m_PhaseShiftData.IsValidReturnPosValue() )
				return 0.f;
			/// ȯ�� ������ �ߵ�
			else if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PHASE_SHIFT ) )
				return 0.f;
		} break;
	case CX2SkillTree::SI_A_APT_PYLON_DYNAMO:			/// ���̳ʸ� ���� - ���Ϸ�
		{
			/// ���� ��ȯ ������ �����̶��, ��Ÿ�� ���� ����
			if ( true == GetPossibleNonstopSummonPylon() )
				return 0.f;
		} break;
	case CX2SkillTree::SI_A_AN_PULSE_BULLET:			/// ����ź
		{
			/// ȯ�� ������ �ߵ�
			if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PULSE_BULLET ) )
				return 0.f;
		} break;

	case CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL:		/// ���ǹ� �׼�
		{
			/// ���ǹ� �׼� �����ÿ��� ��Ÿ�� ���� ���� �ʴ´�.
			if ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == GetFutureStateSkillID() )
				return 0.f;
		} break;

#ifdef X2_ADD_2ND_PATH
	case CX2SkillTree::SI_HA_AMM_APOCALYPSE:
		{

			if ( ThirdPath_IsApocalypseSummonActive() )
				return 0.f;
		} break;

	case CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_SHOOTING_CHASER:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL:
		{
			/// official slot_263 install case: marked re-cast -> the BASE templet cooltime (fall through); non-marked (factory start/switch) -> m_fFactoryDP AS COOLTIME SECONDS
			if ( m_iFactoryMarkedSkillID != (int)pSkillTemplet_->m_eID )
				return m_fFactoryDP;
		} break;
	case CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE:
		{


			if ( 1 == m_vecMaxStrikeHandles.size() )
			{
				float fMaxStrikeWindowRemain = m_fMaxStrikeWindowRemain;
				if ( true == m_bOverlimitEnabled && true == ThirdPath_IsOverlimitCoolListed( (int)pSkillTemplet_->m_eID ) )
					fMaxStrikeWindowRemain *= m_fOverlimitCooltime;
				return max( 0.f, fMaxStrikeWindowRemain );
			}
			return 0.5f;
			//}} 2026-09-04 X2GUAdd leftovers LOCK-2b //
		} break;
	//}} 2026-09-04 X2GUAdd reverify GAUGE-1 //
#endif
	default:
		break;
	}
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


	if( pSkillTemplet_->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL
		&& NULL != g_pMain && g_pMain->GetNowStateID() != CX2Main::XS_PVP_GAME
		&& NULL != g_pData && NULL != g_pData->GetMyUser() && NULL != g_pData->GetMyUser()->GetSelectUnit()
		&& NULL != g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( CONCENTRATED_EL_ESSENCE_ITEM ) )
	{
		fSkillCoolTime = max( 0.f, fSkillCoolTime - 60.f );
	}
	//}} 2026-09-01 item 78931 //


	if ( true == m_bOverlimitEnabled &&
		( ( true == GetIsFormationMode() && true == ThirdPath_IsSAFamilyWithCooldown( pSkillTemplet_, iSkillLevel ) ) ||
		  true == ThirdPath_IsOverlimitCoolListed( (int)pSkillTemplet_->m_eID ) ) )
	{
		fSkillCoolTime *= m_fOverlimitCooltime;
	}
	//}} 2026-09-07 //
#endif

	return max( 0.f, fSkillCoolTime );
}
/*virtual*/ CX2GageData* CX2GUAdd::CreateGageData()
{
	if ( true == IsMyUnit() )
	{
		const CX2AddGageData* pAddrGageData =
			static_cast<const CX2AddGageData*>( CX2GageManager::GetInstance()->GetMyGageData() );

		if ( NULL != pAddrGageData )
			return new CX2AddGageData( *pAddrGageData );
		else
			return new CX2AddGageData();
	}
	else
		return new CX2AddGageData();
}

/*virtual*/	bool CX2GUAdd::ShouldResetNotBusyTimer()
{
	if ( false == CX2GUUser::ShouldResetNotBusyTimer() )
		return false;
	else if ( ANSI_HYPER_WAIT == m_FrameDataFuture.syncData.nowState )
		return false;
	else if ( ANSI_LEVITATION_WAIT == m_FrameDataFuture.syncData.nowState )
		return false;

	return true; // �ٻ� ���� ��� ���� �ؾ� ��
}

void CX2GUAdd::InitStateID()
{
	m_CommonState.m_Wait	= USI_WAIT;
}

/** @function	: InitStateCommon
	@brief		: ��� ĳ���Ͱ� ���������� ���� ���� �� ������ ������� �������� ���� ���� �ʱ�ȭ
*/
void CX2GUAdd::InitStateCommon()
{
	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "ANSI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DieBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DieBackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DieBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DieBackEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "ANSI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "ANSI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, WaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, WaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, WaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_HYPER_WAIT;
	m_LuaManager.MakeTableReference( "ANSI_HYPER_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, HyperWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, HyperWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, HyperWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "ANSI_WALK", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, WalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "ANSI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, JumpReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "ANSI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, JumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ANSI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, JumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_JUMP_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, JumpLandingStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, JumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "ANSI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "ANSI_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_AIR_DASH;
	m_LuaManager.MakeTableReference( "ANSI_AIR_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AirDashEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, AirDashStateEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_LEVITATION_WAIT;
	m_LuaManager.MakeTableReference( "ANSI_LEVITATION_WAIT", stateData.stateID );
	stateData.OnFrameMove	= SET_CB_FUNC( CX2GUAdd, LevitationWaitFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, LevitationWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ANSI_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, FormationModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DamageDownFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageDownFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DamageDownBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageDownBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DamageAirDownLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "ANSI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "ANSI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "ANSI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "ANSI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "ANSI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "ANSI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ANSI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "ANSI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOnEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_SPECIAL_MOVE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveEndFutrue );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingSpecialMoveEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ANSI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, FormationModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, RidingHyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_STAND_UP_ROLLING_FRONT_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_STANDUP_ROLLING_FRONT_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, StandUpRollingFrontFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, StandUpRollingFrontFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_STAND_UP_ROLLING_FRONT_BACK;
	m_LuaManager.MakeTableReference( "ANSI_STANDUP_ROLLING_FRONT_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, StandUpRollingFrontBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, StandUpRollingFrontBackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_STAND_UP_ROLLING_BACK_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_STANDUP_ROLLING_BACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, StandUpRollingBackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, StandUpRollingBackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_STAND_UP_ROLLING_BACK_BACK;
	m_LuaManager.MakeTableReference( "ANSI_STANDUP_ROLLING_BACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, StandUpRollingBackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, StandUpRollingBackBackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_STAND_UP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "ANSI_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, StandUpAttackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, StandUpAttackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_STAND_UP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "ANSI_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, StandUpAttackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, StandUpAttackBackEnd );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, StandUpAttackBackStateEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_POWER_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_POWER_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpPowerLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DashJumpPowerLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpPowerLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "ANSI_WALL_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
	m_StateList[stateData.stateID] = stateData;

//=====================================================================================
// ���� ������Ʈ

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_Z;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_ZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_ZZZZ;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_ZZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_ZZZX;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_ZZZX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZZZXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_ZZX;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_ZZX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZZXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_ZZXX;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_ZZXX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboZZXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_X;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_XX;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_XX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_XXX;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_XXX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_XXZ;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_XXZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXXZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_XXZZ_START;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_XXZZ_START", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXXZZStartEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, ComboXXZZStartStateEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_XXZZ_LOOP;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_XXZZ_LOOP", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, ComboXXZZLoopStateStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXXZZLoopEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, ComboXXZZLoopStateEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_COMBO_XXZZ_FINISH;
	m_LuaManager.MakeTableReference( "ANSI_COMBO_XXZZ_FINISH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ComboXXZZFinishStateStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, ComboXXZZFinishStateStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ComboXXZZFinishEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ANSI_JUMP_ATTACK_Z", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAdd, JumpComboZEventMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, JumpComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_JUMP_ATTACK_X;
	m_LuaManager.MakeTableReference( "ANSI_JUMP_ATTACK_X", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAdd, JumpComboXEventMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, JumpComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_COMBO_Z;
	m_LuaManager.MakeTableReference( "ANSI_DASH_COMBO_Z", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAdd, DashComboZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ANSI_DASH_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ANSI_DASH_COMBO_ZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_COMBO_X;
	m_LuaManager.MakeTableReference( "ANSI_DASH_COMBO_X", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DashComboXStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DashComboXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashComboXEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, DashComboXStateEnd );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, DashComboXStateEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_COMBO_XX;
	m_LuaManager.MakeTableReference( "ANSI_DASH_COMBO_XX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashComboXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_COMBO_XXX;
	m_LuaManager.MakeTableReference( "ANSI_DASH_COMBO_XXX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashComboXXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_Z;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_ZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_X;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_XX_START;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_XX_START", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXStartEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXStartStateEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_XX_CHARGE;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_XX_CHARGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXLoopStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXLoopFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXLoopEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXLoopStateEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ANSI_DASH_JUMP_COMBO_XX_FINISH;
	m_LuaManager.MakeTableReference( "ANSI_DASH_JUMP_COMBO_XX_FINISH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXFinishStateStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXFinishFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, DashJumpComboXXFinishEventProcess );
	m_StateList[stateData.stateID] = stateData;
}

void CX2GUAdd::InitStateByUnitClass()
{
	UserUnitStateData stateData;
	switch ( GetUnitClass() )
	{
	case CX2Unit::UC_ADD_NASOD_RULER:
		{

		} break;


	case CX2Unit::UC_ADD_LUNATIC_PSYKER:
		{
			stateData.Init();
			stateData.stateID			= ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_START;
			m_LuaManager.MakeTableReference( "ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_START", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZFrontZStartEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP;
			m_LuaManager.MakeTableReference( "ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZFrontZLoopStateStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZFrontZLoopStateStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZFrontZLoopEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX;
			m_LuaManager.MakeTableReference( "ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZFrontZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP;
			m_LuaManager.MakeTableReference( "ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXDirectionXLoopEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXDirectionXLoopStateEndFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ;
			m_LuaManager.MakeTableReference( "ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXDirectionXZEventProcess );
			m_StateList[stateData.stateID] = stateData;
		} /// 1�� ���� ������ Ÿ�� ����, Break ����
		[[fallthrough]];


	case CX2Unit::UC_ADD_PSYCHIC_TRACER:
		{
			stateData.Init();
			stateData.stateID			= ANSI_DP_BARRIER_ACTIVE;
			m_LuaManager.MakeTableReference( "ANSI_DP_BARRIER_ACTIVE", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, NasodBarrierActiveStateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodBarrierActiveEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_READY;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_READY", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorReadyEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_ATTACK;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_ATTACK", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorAttackStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorAttackEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_LANDING;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_LANDING", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorLandingEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorLandingEndFuture );

			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_Z;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_ZZ;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_ZZZ;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_ZZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_ZZX;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_ZZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_ZZXX;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_ZZXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboZZXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_X;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_X", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXStateEndFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_XX;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_XX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_XXX;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_XXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorComboXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

#ifdef X2_ADD_2ND_PATH

	case CX2Unit::UC_ADD_MASTERMIND:
		{
			stateData.Init();
			stateData.stateID			= ANSI_AMM_COMBO_XXDownX_START;
			m_LuaManager.MakeTableReference( "ANSI_AMM_COMBO_XXDownX_START", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXStartEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXStartStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AMM_COMBO_XXDownX_LOOP;
			m_LuaManager.MakeTableReference( "ANSI_AMM_COMBO_XXDownX_LOOP", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXLoopStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXLoopFrameMove );	//{{ official sub_324ECF70 (loop beam DE create + track) //
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXLoopStateEnd );		//{{ destroy loop DE on exit //
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXLoopEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AMM_COMBO_XXDownX_END;
			m_LuaManager.MakeTableReference( "ANSI_AMM_COMBO_XXDownX_END", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXEndStateStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AMMComboXXDownXEndEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AMM_DASH_JUMP_COMBO_XZ;
			m_LuaManager.MakeTableReference( "ANSI_AMM_DASH_JUMP_COMBO_XZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AMMDashJumpComboXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AMM_DASH_JUMP_COMBO_XZZ;
			m_LuaManager.MakeTableReference( "ANSI_AMM_DASH_JUMP_COMBO_XZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AMMDashJumpComboXZZEventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ANSI_AAT_CREATE_DYNAMO_FACTORY;
			m_LuaManager.MakeTableReference( "ANSI_AAT_CREATE_DYNAMO_FACTORY", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_AAT_CREATE_DYNAMO_FACTORY_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_AAT_CREATE_DYNAMO_FACTORY_OnFrameMove );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, ANSI_AAT_CREATE_DYNAMO_FACTORY_StateEnd );
			m_StateList[stateData.stateID] = stateData;
			//}} 2026-08-30 MARCH2015 batch6 //
		}
	//}} 2026-09-04 X2GUAdd reverify REG-1 -- NO break: MM falls through into the AT case below //
#endif

#ifdef X2_ADD_2ND_PATH
	case CX2Unit::UC_ADD_ARC_TRACER:
		{
			// AAT Nasod Armor combos - same mechanics as base, different lua tables
			stateData.Init();
			stateData.stateID			= ANSI_AAT_COMBO_NASOD_ARMOR_X;
			m_LuaManager.MakeTableReference( "ANSI_AAT_COMBO_NASOD_ARMOR_X", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATComboNasodArmorXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AAT_COMBO_NASOD_ARMOR_XX;
			m_LuaManager.MakeTableReference( "ANSI_AAT_COMBO_NASOD_ARMOR_XX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATComboNasodArmorXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AAT_COMBO_NASOD_ARMOR_XXX;
			m_LuaManager.MakeTableReference( "ANSI_AAT_COMBO_NASOD_ARMOR_XXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATComboNasodArmorXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AAT_COMBO_NASOD_ARMOR_Z;
			m_LuaManager.MakeTableReference( "ANSI_AAT_COMBO_NASOD_ARMOR_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATComboNasodArmorZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AAT_COMBO_NASOD_ARMOR_ZZ;
			m_LuaManager.MakeTableReference( "ANSI_AAT_COMBO_NASOD_ARMOR_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATComboNasodArmorZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AAT_COMBO_NASOD_ARMOR_ZZZ;
			m_LuaManager.MakeTableReference( "ANSI_AAT_COMBO_NASOD_ARMOR_ZZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATComboNasodArmorZZZEventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ANSI_AAT_DASH_COMBO_ZBackX;
			m_LuaManager.MakeTableReference( "ANSI_AAT_DASH_COMBO_ZBackX", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, AATDashComboZBackXStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, AATDashComboZBackXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATDashComboZBackXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, AATDashComboZBackXStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ANSI_AAT_DASH_COMBO_ZX;
			m_LuaManager.MakeTableReference( "ANSI_AAT_DASH_COMBO_ZX", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAdd, AATDashComboZXStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAdd, AATDashComboZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, AATDashComboZXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, AATDashComboZXStateEndFuture );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, AATDashComboZXStateEnd );
			m_StateList[stateData.stateID] = stateData;
			//}} 2026-09-04 X2GUAdd reverify C09 D15 //


			stateData.Init();
			stateData.stateID			= ANSI_AAT_CREATE_DYNAMO_FACTORY;
			m_LuaManager.MakeTableReference( "ANSI_AAT_CREATE_DYNAMO_FACTORY", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAdd, ANSI_AAT_CREATE_DYNAMO_FACTORY_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_AAT_CREATE_DYNAMO_FACTORY_OnFrameMove );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAdd, ANSI_AAT_CREATE_DYNAMO_FACTORY_StateEnd );
			m_StateList[stateData.stateID] = stateData;
			//}} 2026-08-30 MARCH2015 batch6 //


			stateData.Init();
			stateData.stateID			= ANSI_COMBO_NASOD_ARMOR_LANDING;
			m_LuaManager.MakeTableReference( "ANSI_COMBO_NASOD_ARMOR_LANDING", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, NasodArmorLandingEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorLandingEndFuture );
			m_StateList[stateData.stateID] = stateData;
			//}} 2026-09-04 X2GUAdd reverify C05 DIFF-C //
		} break;
#endif


#ifdef X2_ADD_3RD_PATH

	case CX2Unit::UC_ADD_DIABOLIC_ESPER:
		{
			// Diabolic Esper: ADE formation combos (XXXX / XXXZ, formation-mode vs no-formation-mode) //
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ADE_COMBO_XXXX_FORMATION_MODE], ANSI_ADE_COMBO_XXXX_FORMATION_MODE, "ANSI_ADE_COMBO_XXXX_FORMATION_MODE" );
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ADE_COMBO_XXXX_NO_FORMATION_MODE], ANSI_ADE_COMBO_XXXX_NO_FORMATION_MODE, "ANSI_ADE_COMBO_XXXX_NO_FORMATION_MODE" );
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ADE_COMBO_XXXZ_FORMATION_MODE], ANSI_ADE_COMBO_XXXZ_FORMATION_MODE, "ANSI_ADE_COMBO_XXXZ_FORMATION_MODE" );
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ADE_COMBO_XXXZ_NO_FORMATION_MODE], ANSI_ADE_COMBO_XXXZ_NO_FORMATION_MODE, "ANSI_ADE_COMBO_XXXZ_NO_FORMATION_MODE" );
		}
	//}} 2026-09-04 X2GUAdd reverify REG-2 -- NO break: DE falls through into the TT case below //
#endif

#ifdef X2_ADD_3RD_PATH
	case CX2Unit::UC_ADD_TIME_TRACER:
		{
			// Time Tracer: Nasod Armor X chain (X->XX->XXX_START->LOOP->END) + Z chain (Z->ZZ->ZZZ->ZZZZ_START->LOOP->END) + ZZXX + LANDING //

			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_ZZXX], ANSI_ATT_COMBO_ZZXX, "ANSI_ATT_COMBO_ZZXX" );

			m_StateList[ANSI_ATT_COMBO_ZZXX].OnFrameMove = SET_CB_FUNC( CX2GUAdd, ATTComboZZXXFrameMove );
			//}} 2026-09-04 X2GUAdd reverify C09 D25 //

			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_X], ANSI_ATT_COMBO_NASOD_ARMOR_X, "ANSI_ATT_COMBO_NASOD_ARMOR_X" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_X].OnEventProcess = SET_CB_FUNC( CX2GUAdd, ANSI_AddComboChain_EventProcess );
			m_mapThirdPathChain[ANSI_ATT_COMBO_NASOD_ARMOR_X] = ANSI_ATT_COMBO_NASOD_ARMOR_XX;
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XX], ANSI_ATT_COMBO_NASOD_ARMOR_XX, "ANSI_ATT_COMBO_NASOD_ARMOR_XX" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XX].OnEventProcess = SET_CB_FUNC( CX2GUAdd, ANSI_AddComboChain_EventProcess );
			m_mapThirdPathChain[ANSI_ATT_COMBO_NASOD_ARMOR_XX] = ANSI_ATT_COMBO_NASOD_ARMOR_XXX_START;
			ANSI_ThirdPath_CreateChainedState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XXX_START], ANSI_ATT_COMBO_NASOD_ARMOR_XXX_START, "ANSI_ATT_COMBO_NASOD_ARMOR_XXX_START", ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP );

			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP], ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP, "ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP].StateStart		= SET_CB_FUNC( CX2GUAdd, ATTComboNasodArmorXXXLoopStateStart );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP].OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ATTComboNasodArmorXXXLoopEventProcess );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP].StateEnd		= SET_CB_FUNC( CX2GUAdd, ATTComboNasodArmorXXXLoopStateEnd );
			//}} 2026-09-04 X2GUAdd reverify C09 D23 //
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_XXX_END], ANSI_ATT_COMBO_NASOD_ARMOR_XXX_END, "ANSI_ATT_COMBO_NASOD_ARMOR_XXX_END" );

			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_Z], ANSI_ATT_COMBO_NASOD_ARMOR_Z, "ANSI_ATT_COMBO_NASOD_ARMOR_Z" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_Z].OnEventProcess = SET_CB_FUNC( CX2GUAdd, ANSI_AddComboChain_EventProcess );
			m_mapThirdPathChain[ANSI_ATT_COMBO_NASOD_ARMOR_Z] = ANSI_ATT_COMBO_NASOD_ARMOR_ZZ;
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZ], ANSI_ATT_COMBO_NASOD_ARMOR_ZZ, "ANSI_ATT_COMBO_NASOD_ARMOR_ZZ" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZ].OnEventProcess = SET_CB_FUNC( CX2GUAdd, ANSI_AddComboChain_EventProcess );
			m_mapThirdPathChain[ANSI_ATT_COMBO_NASOD_ARMOR_ZZ] = ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ;
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ], ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ, "ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ].OnEventProcess = SET_CB_FUNC( CX2GUAdd, ANSI_AddComboChain_EventProcess );
			m_mapThirdPathChain[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ] = ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START;

			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START], ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START, "ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START].OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ATTComboNasodArmorZZZZStartEventProcess );
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP], ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP, "ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP" );
			m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP].OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ATTComboNasodArmorZZZZLoopEventProcess );
			//}} 2026-09-04 X2GUAdd reverify C09 D23 //
			ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END], ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END, "ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END" );

		ANSI_ThirdPath_CreateState( m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_LANDING], ANSI_ATT_COMBO_NASOD_ARMOR_LANDING, "ANSI_ATT_COMBO_NASOD_ARMOR_LANDING" );
		m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_LANDING].OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ATTComboNasodArmorLandingEventProcess );
		m_StateList[ANSI_ATT_COMBO_NASOD_ARMOR_LANDING].StateEndFuture	= SET_CB_FUNC( CX2GUAdd, NasodArmorLandingEndFuture );
		//}} 2026-09-04 X2GUAdd reverify C09 D24 //
	} break;
#endif

//}} 2026-08-28 //
#endif
	default:
		ASSERT(!"GetUnitClass Error");
		break;
	}
}


void CX2GUAdd::SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, ADD_NASODRULER_STATE_ID eStateID_ )
{
	if ( NULL != pSkillTemplet_ )
		SetSkillLevelStateData( pSkillTemplet_, m_StateList[eStateID_] );
}

void CX2GUAdd::SetSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, UserUnitStateData& stateData_ )
{
	if( NULL == pSkillTemplet_ )
		return;

	if ( NULL == GetUnit() )
		return;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
	const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

	stateData_.m_SPLevel	= iSkillTempletLevel;
	stateData_.m_fPowerRate	= pSkillTemplet_->GetSkillPowerRateValue( iSkillTempletLevel );
	stateData_.m_eSkillID	= pSkillTemplet_->m_eID;
}
void CX2GUAdd::ShowActiveSkillCutInAndLight( float fTimeToShow_, bool bOnlyLight_ /*= false*/ )
{
	if( m_pXSkinAnim->EventTimerOneshot( fTimeToShow_ ) )
	{
		if ( GetShowCutInAndChangeWorldColor() && GetShowActiveSkillShow() )
		{
			ActiveSkillShow( L"Dummy2_Lhand", s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].fileName.c_str(),
				s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].vSize,
				s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].vPosition, IsHyperState(), bOnlyLight_ );
		}
	}
}
/*virtual*/ void CX2GUAdd::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ )
{
	ShowActiveSkillCutInAndLight( fTimeToShow_, bOnlyLight_ );
}
bool CX2GUAdd::CanUseSpecialAttack( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition/* = CX2SkillTree::ASUT_GROUND*/ )
{
	KTDXPROFILE();

	if( NULL != GetUnit() )
	{
		const CX2UserSkillTree& cUserSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;	// ������ ��� ��ų Ʈ��

		m_iPressedSkillSlotIndex = -1;
		const CX2UserSkillTree::SkillSlotData* pSkillSlotData = NULL;

		// ����� ��ų�� ��ų���� �����Ϳ� �� �ε����� ����
		if ( false == CommonSpecialAttackEventProcess( cUserSkillTree, pSkillSlotData, m_iPressedSkillSlotIndex ) )
			return false;

		if( NULL == pSkillSlotData )
			return false;

		// ����� ��ų
		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetEquippedActiveSkillTemplet( pSkillSlotData->m_eID );

		if( NULL == pSkillTemplet )
			return false;

		const int iSkillTempletLevel = max( 1, cUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		// ��ų�� ����� �� �ִ� �������� �Ǵ�( ex: ���߿����� ��� �� �� ���� �ʻ�� ��)
		const CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseConditionBySkillTemplet = GetSkillUseCondition( pSkillTemplet );

		if( false == CheckSkillUseCondition( eActiveSkillUseCondition, eActiveSkillUseConditionBySkillTemplet ) )
			return false;

		//��ȯ �ʻ�� ������ ������ ���� ���¿��� ����� �� ����.
		if( false == CheckSummonSpecialAttack( pSkillTemplet->m_eID ) )
			return false;

		// ���Ĵ������� ��� ������ ��ų���� �˻�
		if( false == CheckSkillUsePVPOfficial( pSkillTemplet->m_eID ) )
			return false;

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

		if ( true == ThirdPath_IsInstallFamilySkill( pSkillTemplet->m_eID )
			&& CX2SkillTree::ASUT_GROUND != eActiveSkillUseCondition
			&& ( 0 == m_iFactoryMarkedSkillID || pSkillTemplet->m_eID != m_iFactoryMarkedSkillID ) )
			return false;
		//}} 2026-09-04 X2GUAdd reverify CAST-4 //

		// 탈 占쏙옙 탑占쏙옙 占쏙옙占승울옙占쏙옙 占쏙옙占?占쏙옙占쏙옙占쏙옙 占쏙옙킬占쏙옙占쏙옙 占싯삼옙
#else
		// Ż �� ž�� ���¿��� ��� ������ ��ų���� �˻�
#endif
		if( false == IsValideSkillRidingOn( pSkillTemplet->m_eID)  )
			return false;

		// Ư���� ��Ȳ���� �ߵ��� �� �ִ� ��ų���� �˻�
		if( false == CheckSkillSpecialUse( pSkillTemplet->m_eID ) )
			return false;

		// ���� �Ҹ��� ����
		m_fMPConsume			= GetActualMPConsume( pSkillTemplet->m_eID, iSkillTempletLevel );

		// DP �Ҹ��� ����			( ������ ��� �÷� ��� )
		m_fDPConsume			= GetActualDPConsume( pSkillSlotData );

		// ���� ����Ʈ �Ҹ��� ����	( ������ ĳ���� �÷� ��� )
		m_iMutationCountConsume = GetMutationPointConsume( pSkillSlotData );

		const float fCoolTimeMax = GetActualCoolTime( pSkillTemplet, iSkillTempletLevel );

		float fCoolTimeLeft = min( fCoolTimeMax, static_cast<float>( pSkillSlotData->m_fCoolTimeLeft ) );


#ifdef X2_DEBUG
		if( false == g_pMain->IsMyAuthLevelHigherThan( CX2User::XUAL_DEV ) )
#endif // _SERVICE_
			//#endif _SERVICE_
		{
			if( 0.f < fCoolTimeLeft )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_226 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}

			/// MP �Ҹ� �˻�
			if ( GetNowMp() < m_fMPConsume )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}

			/// DP �Ҹ� �˻� ( ���� ��� ���̶��, DP�� ������� �ʴ´�. �񰢼����� MP, �����ÿ��� DP�� ���� ��ų ���� )
			if ( GetDPValue() < m_fDPConsume )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_29420 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}

			/// ���� ����Ʈ �Ҹ� �˻�
			if ( GetMutationCount() < m_iMutationCountConsume )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_29421 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

			bool bApocSummonAliveGate = false;
#ifdef X2_ADD_2ND_PATH
			bApocSummonAliveGate = ThirdPath_IsApocalypseSummonActive();
#endif
			//}} 2026-09-04 X2GUAdd reverify CAST-1 //
			if ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME && false == bApocSummonAliveGate )
#else
			if ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
#endif
			{
				const int iItemNum = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetNumItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );

				if( iItemNum <= 0 )
				{
					g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_26119 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
					return false;
				}
			}

			/// ���̳ʸ� ���� - ���Ϸ� ��ȯ ���� ���� �˻�
			D3DXVECTOR3 vSummonPylonPosUnused( 0.f, 0.f, 0.f );
			if ( pSkillTemplet->m_eID == CX2SkillTree::SI_A_APT_PYLON_DYNAMO &&
				false == GetSummonPylonPos( vSummonPylonPosUnused ) )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_29419 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}
		}

		/// �������� �� ��� ���� ��� Ȥ�� �������� ����� ���� ��ȭ ���� �̵� ��� �Ұ�
		if ( pSkillTemplet->m_eID == CX2SkillTree::SI_A_AN_PHASE_SHIFT &&
			 true == m_PhaseShiftData.IsValidReturnPosValue() &&
			 NULL != g_pX2Game &&
			 ( true == g_pX2Game->GetGameEnd() || true == g_pX2Game->GetLastKillCheck() ) )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_226 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}

		if( NULL == pSkillTemplet )		/// pSkillTemplet�� ������ ���� �����ϱ� ������, NULLó�� �߰�
			return false;

		m_pSkillTemplet = pSkillTemplet;

		return true;
	}
	else
	{
		return false;
	}

	return false;
}

void CX2GUAdd::StateChangeSpecialAttack( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )
{
	if( NULL != GetUnit() && NULL != pSkillTemplet_ )
	{
		CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;// ������ ��� ��ų Ʈ��

		CountUsedSkill( m_iPressedSkillSlotIndex );

		/// MP �Һ�
			UpNowMp( -m_fMPConsume );

		/// DP �Һ�
			UpNowDPValue( -m_fDPConsume );

		/// ���� ����Ʈ �Һ�
		UpMutationCount( -m_iMutationCountConsume );

#ifdef X2_ADD_3RD_PATH
		ThirdPath_MindControlCostRecoveryProc();
#endif
		/// ����� ��Ƽ�� �̸鼭 DP �Һ��� ���� �� DP��뷮�� ���� DP ���� ó��
		if ( CX2SkillTree::ST_SPECIAL_ACTIVE == pSkillTemplet_->m_eType &&
			 0.f >= pSkillTemplet_->m_usFPConsumtion  )
			UpNowDPValueByUseMP( m_fMPConsume );


#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

		bool bApocSummonAliveConsume = false;
#ifdef X2_ADD_2ND_PATH
		bApocSummonAliveConsume = ThirdPath_IsApocalypseSummonActive();
#endif
		//}} 2026-09-04 X2GUAdd reverify CAST-1 //
		if ( pSkillTemplet_->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME && false == bApocSummonAliveConsume )
#else
		if ( pSkillTemplet_->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
#endif
		{
			CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );

			if ( NULL != pItem )
				g_pX2Game->Handler_EGS_USE_FINALITY_SKILL_REQ( pItem->GetItemData().m_ItemUID );
		}

		const int	iSkillTempletLevel	= max( 1, accessUserSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

		/// ������ ���� ��Ÿ�� ��ȯ �Լ�, Ư�� ���ǿ� ���Ͽ� ��Ÿ�� ������ �����ϱ� ���� ���� ����
		const float fResultCoolTime		= GetCustomSkillCoolTime( GetActualCoolTime( pSkillTemplet_, iSkillTempletLevel), pSkillTemplet_->m_eID );

		if( false == IsSamef( -1.f, fResultCoolTime ))
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet_->m_eID, fResultCoolTime );

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

		ThirdPath_TimeControlCooldownProc( pSkillTemplet_->m_eID, fResultCoolTime );
		//}} 2026-09-07 //

#ifdef X2_ADD_2ND_PATH

		if ( true == ThirdPath_IsInstallFamilySkill( (int)pSkillTemplet_->m_eID ) && (int)pSkillTemplet_->m_eID != m_iFactoryMarkedSkillID )
		{
			for ( int iPage = 0; iPage < 2; ++iPage )	/// A page first, then B page (official slot order)
			{
				for ( int iSlot = 0; iSlot < 4; ++iSlot )
				{
					const CX2UserSkillTree::SkillSlotData* pSlotData = accessUserSkillTree.GetSkillSlot( iSlot, ( 1 == iPage ) );

					if ( NULL == pSlotData )
						continue;

					const int iSlotSkillID = (int)pSlotData->m_eID;

					if ( iSlotSkillID == (int)pSkillTemplet_->m_eID )
						continue;

					if ( false == ThirdPath_IsInstallFamilySkill( iSlotSkillID ) )
						continue;

					const float fSlotCoolLeft = pSlotData->m_fCoolTimeLeft;

					if ( 0.f <= fSlotCoolLeft && fSlotCoolLeft < 3.0f )
						accessUserSkillTree.SetSkillCoolTimeLeft( (CX2SkillTree::SKILL_ID)iSlotSkillID, 3.0f );
				}
			}
		}
		//}} 2026-09-04 X2GUAdd reverify CAST-3 //
#endif


		ThirdPath_OverlimitCoolListRegister( pSkillTemplet_, iSkillTempletLevel );
		//}} 2026-09-07 //


#endif
		if( false == SpecialAttackNoStageChange( pSkillTemplet_ ) )
		{
			/// Ư�� ��ų�� ���� ������Ʈ ���� �˻�
			if ( false == SpecialAttackCustomStateChange( pSkillTemplet_->m_eID ) )
			{
				/// ���� ������ �� ������Ʈ�� ���ٸ�, �Ϲ����� ���� ����
				if( false == GetIsFormationMode() )
				{
					if( m_iPressedSkillSlotIndex > 3 ) //���� B�϶�
					{
						StateChange( _CONST_GUUSER_::aNormalSkillStateIDSlotB[m_iPressedSkillSlotIndex-4] );
					}
					else
					{
						StateChange( _CONST_GUUSER_::aNormalSkillStateID[m_iPressedSkillSlotIndex] );
					}
				}
				else
				{
					if( m_iPressedSkillSlotIndex > 3 ) //���� B�϶�
					{
						StateChange( _CONST_GUUSER_::aHyperSkillStateIDSlotB[m_iPressedSkillSlotIndex-4] );
					}
					else
					{
						StateChange( _CONST_GUUSER_::aHyperSkillStateID[m_iPressedSkillSlotIndex] );
					}
				}
			}
		}

		m_iNowSpecialAttack = m_iPressedSkillSlotIndex + 1;
		m_bSpecialAttackEventProcessedAtThisFrame = true;
	}

}
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

/*virtual*/float CX2GUAdd::GetReduceDebuffDurationRate( BUFF_TEMPLET_ID eBuffTempletID_ )
{
	if ( 0.f < m_fMindControlDebuffReduceRate )
	{
		switch ( eBuffTempletID_ )
		{
		case BTI_DEBUFF_STUN:						// official case 52
		case BTI_DEBUFF_PANIC:						// 53
		case BTI_DEBUFF_CONFUSION:					// 61
		case BTI_DEBUFF_SUPPRESSION:				// 161
		case BTI_BUFF_TROCKTA_MAGICATTACKB_STUN:	// 172 (enum added this wave - X2BuffFactor.h; march2015 Enum.lua:9230)
		case BTI_STONE_GOD_ANGER:					// 184 (added this wave; Enum.lua:9221)
		case BTI_DEBUFF_PANDEMONIUM_FEAR:			// 223
		case BTI_DEBUFF_JUDGEMENT_FIRE:				// 291 (added this wave; Enum.lua:9079)
			return m_fMindControlDebuffReduceRate;
		default:
			break;
		}
	}
	return 1.f;
}
//}} 2026-09-07 //


/*virtual*/float CX2GUAdd::GetESPProcDamageTakenScale()
{
	if ( 0.f < m_fESPProcRate && GetRandomFloat() <= m_fESPProcRate )
		return m_fESPDamagedRel;
	return 1.f;
}
//}} 2026-09-07 //



/*virtual*/ float CX2GUAdd::GetMPGainOnHitFactorByFormation()
{
	// official newslot_298/299 @0x324E09A0/0x324E09D0 (byte-identical twins): formation -> +26356 (m_fOverlimitMpGainOnHit); else 1.0
	if ( true == GetIsFormationMode() )
		return m_fOverlimitMpGainOnHit;
	return 1.f;
}

bool CX2GUAdd::ThirdPath_IsSAFamilyWithCooldown( IN const CX2SkillTree::SkillTemplet* pTemplet_, IN int iSkillLevel_ ) const
{
	// official f_31F6B270 (Ghidra FREE_31F_1.txt): type == 7 || ( 0xE <= type && type <= 0x10 ) = { ST_SPECIAL_ACTIVE, ST_SPECIAL_ACTIVE_EX1..EX3 }; + the has-cooldown gate templet+0x164 (checked at all four x1.5 sites)
	if ( NULL == pTemplet_ )
		return false;
	if ( CX2SkillTree::ST_SPECIAL_ACTIVE != pTemplet_->m_eType &&
		 CX2SkillTree::ST_SPECIAL_ACTIVE_EX1 != pTemplet_->m_eType &&
		 CX2SkillTree::ST_SPECIAL_ACTIVE_EX2 != pTemplet_->m_eType &&
		 CX2SkillTree::ST_SPECIAL_ACTIVE_EX3 != pTemplet_->m_eType )
		return false;
	return ( 0.f != pTemplet_->GetSkillCoolTimeValue( iSkillLevel_ ) );
}

bool CX2GUAdd::ThirdPath_IsOverlimitCoolListed( IN int iSkillID_ ) const
{
	// official slot_211 list lookup (f_324F0700): listed -> the +26364 factor applies
	for ( std::vector<std::pair<int,float>>::const_iterator it = m_vecOverlimitCoolList.begin(); it != m_vecOverlimitCoolList.end(); ++it )
	{
		if ( it->first == iSkillID_ )
			return true;
	}
	return false;
}

void CX2GUAdd::ThirdPath_OverlimitCoolListRegister( IN const CX2SkillTree::SkillTemplet* pTemplet_, IN int iSkillLevel_ )
{
	// official f_324FB830 (cast-dispatcher tail) conditions, in order: (1) Overlimit learned; (2) cast-info non-null + skillID != 0; (3) 9025 guard - the pending-barrage vector must be EMPTY; (4) 9028 guard - NO live stigma mark; (5) remove any existing entry first (unconditional refresh-or-clear); (6) push ( skillID, 180.0f ) only if in formation && SA-family && has-cooldown. Consequence of (5)+(6): a non-formation recast of a listed SA skill REMOVES it (the penalty ends); a formation recast refreshes the 180s
	if ( false == m_bOverlimitEnabled )
		return;
	if ( NULL == pTemplet_ || 0 == (int)pTemplet_->m_eID )
		return;
	if ( CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE == pTemplet_->m_eID && false == m_vecMaxStrikeHandles.empty() )
		return;
	if ( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA == pTemplet_->m_eID && 0 != m_uidStigmaTarget )
		return;

	const int iSkillID = (int)pTemplet_->m_eID;

	for ( std::vector<std::pair<int,float>>::iterator it = m_vecOverlimitCoolList.begin(); it != m_vecOverlimitCoolList.end(); ++it )
	{
		if ( it->first == iSkillID )
		{
			m_vecOverlimitCoolList.erase( it );
			break;
		}
	}

	if ( false == GetIsFormationMode() )
		return;
	if ( false == ThirdPath_IsSAFamilyWithCooldown( pTemplet_, iSkillLevel_ ) )
		return;

	m_vecOverlimitCoolList.push_back( std::make_pair( iSkillID, 180.0f ) );	// flt_32D2F1F8 = 180.0f (byte-verified)
}

void CX2GUAdd::ThirdPath_OverlimitCoolListTick( IN float fElapsed_ )
{
	// official prune f_324F9DB0 + tick helper f_31F90AD0 (disasm line 1656590): entry kept while remain > 0; -= the per-frame elapsed seconds; zeroed + erased at <= 0
	for ( std::vector<std::pair<int,float>>::iterator it = m_vecOverlimitCoolList.begin(); it != m_vecOverlimitCoolList.end(); )
	{
		if ( 0.f < it->second )
		{
			it->second -= fElapsed_;
			if ( 0.f >= it->second )
			{
				it->second = 0.f;
				it = m_vecOverlimitCoolList.erase( it );
				continue;
			}
		}
		++it;
	}
}
//}} 2026-09-07 Phase 3 //


void CX2GUAdd::ThirdPath_MindControlCostRecoveryProc()
{
	if ( 0.f >= m_fMindControlProcRate )
		return;
	if ( GetRandomFloat() > m_fMindControlProcRate )	// official: proc iff roll <= rate (vtbl+368 = GetRandomFloat)
		return;

	if ( true == GetIsFormationMode() )
	{
		const float fRecovered = m_fMindControlRecoveryValue * m_fDPConsume;	// official v11 = +26408 x a3 (DP spent)
		if ( 0.f < fRecovered )
		{
			UpNowDPValue( fRecovered, 1.f );	// official vtbl+528(this, v11, 1.0) = UpNowDPValue (raw units - matches the SetDPValue raw consume above)

			D3DXVECTOR3 vPos = GetPos();
			vPos.y += 50.f;
			std::wstringstream strStream;
			strStream << L"DP+ " << (int)( fRecovered / 20.f );	// official displays v11 / 20.0 (the UI-DP amount)
			if ( NULL != g_pData->GetPicCharBlue() )
				g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), vPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}
	}
	else
	{
		const float fRecovered = m_fMindControlRecoveryValue * m_fMPConsume;	// official v13 = +26408 x a2 (MP spent)
		if ( 0.f < fRecovered )
		{
			UpNowMp( fRecovered );

			D3DXVECTOR3 vPos = GetPos();
			vPos.y += 50.f;
			std::wstringstream strStream;
			strStream << L"MP+ " << (int)fRecovered;
			if ( NULL != g_pData->GetPicCharBlue() )
				g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), vPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
		}
	}
}
//}} 2026-09-07 //


void CX2GUAdd::ThirdPath_TimeControlCooldownProc( const CX2SkillTree::SKILL_ID eCastSkillID_, const float fJustSetCoolTime_ )
{
	if ( 0.f >= m_fTimeControlProcRate )
		return;
	if ( IsSamef( -1.f, fJustSetCoolTime_ ) )	// no cooldown was set for this cast (the -1 sentinel) -> nothing to reduce
		return;
	if ( false == ( GetRandomFloat() < m_fTimeControlProcRate ) )	// official: proc iff roll < rate (fcompp + test ah,41h; je exit)
		return;

	bool bProcAllowed = true;
	switch ( eCastSkillID_ )
	{
	case CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE:	// official case 9025: fire only when the 6-way barrage is NOT pending
		bProcAllowed = m_vecMaxStrikeHandles.empty();
		break;
	case CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA:	// official case 9028: fire only when NO stigma mark is live
		bProcAllowed = ( 0 == m_uidStigmaTarget );
		break;
	case CX2SkillTree::SI_A_APT_PYLON_DYNAMO:		// official case 9205 (the official name maps to our SI_A_APT_PYLON_DYNAMO): fire only when the quicksilver-accel state is NOT the future state
		bProcAllowed = ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL != GetFutureStateSkillID() );
		break;
	default:
		break;
	}

	if ( false == bProcAllowed )
		return;

	if ( NULL != GetUnit() )
		GetUnit()->AccessUnitData().m_UserSkillTree.SetSkillCoolTimeLeft( eCastSkillID_, max( 0.f, fJustSetCoolTime_ - m_fTimeControlCooldownValue ) );
}
//}} 2026-09-07 //

#endif
/*virtual*/float CX2GUAdd::GetChangeDamageRateByAttackerSkill( const CX2DamageManager::DamageData* pDamageData_ ) const
{
	float fAtkDamageRate = 1.f;

	switch( pDamageData_->m_DPRateType )
	{
	case CX2DamageManager::DPRT_SPECIAL_ACTIVE:
	case CX2DamageManager::DPRT_ACTIVE:
		{
			// ��ų�� DP��� ���ο� ���� ������ ���� ����
			if( true == IsDPConsumptionSkill( pDamageData_->m_eUsedSkillID ) )
				fAtkDamageRate *= m_fUsingDPSkillDamageRate;
			else
				fAtkDamageRate *= m_fNotUsingDPSkillDamageRate;
		} break;
	case CX2DamageManager::DPRT_NASOD_ARMOR:
		{
			// ���ҵ� �Ƹ� Ŀ�ǵ� ������ ���� ����
			fAtkDamageRate *= m_fNaCommandDamageRate;
		} break;
	default:
		break;
	}

	return fAtkDamageRate;
}
/*virtual*/ void CX2GUAdd::ReInit( bool bTeamPos /*= false*/, int iStartPosIndex /*= -1*/ )
{
	CX2GUUser::ReInit( bTeamPos, iStartPosIndex );

	if ( IsMyUnit() )
	{
//		������//�������� ��Ȱ ���� �� �ʱ�ȭ�Ǹ� �ȵǱ� ������ ����
// 		switch ( g_pX2Game->GetGameType() )
// 		{
// 		case CX2Game::GT_PVP:
// 			{
// 				SetDPValue( 0.f );
// 				SetFormationModeCount( 0 );
// 				SetMutationCount( 0 );
// 			} break;
//
// 		default:
// 			{
// 				if ( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
// 				{
// 					SetDPValue( 0.f );
// 					SetFormationModeCount( 0 );
// 					SetMutationCount( 0 );
// 				}
// 			} break;
// 		}

		/// DP �� ���� ��ġ ����ȭ
		m_FrameDataFuture.syncData.m_CannonBallCount = GetAbilitySyncData();

		/// ���̳ʸ� ���Ϸ� �� ���� ������ ����Ʈ ���� �ʱ�ȭ
		ResetPylonData();

		/// ���׳�Ʈ�� ���� ��ü �ʱ�ȭ
		m_MagnetronData.ResetMagnetronData();

		/// �����̽ý� �ʵ� ���� �ʱ�ȭ
		m_StasisfieldData.ResetStasisfieldData();

		/// ���� ��ȭ �ʱ�ȭ
		m_PhaseShiftData.InitPhaseEffectData();
		m_PhaseShiftData.InitResturnPosValue();
		m_PhaseShiftData.UpdateSkillSlotUI();

		/// ȯ�� ������ ��ų ���̵� �ʱ�ȭ
		m_eNasodArmorAttackActiveSkillID = CX2SkillTree::SI_NONE;

		/// ������ ������ ���� ��ü �ʱ�ȭ
		m_ReverseReactorTargetInfo.ResetReverseReactorTargetInfo();

		/// ���ǹ� ������ ���� ��ü �ʱ�ȭ
		m_QuicksilverFrenzyData.ResetQuicksilverFrenzyData();
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


		m_bApocalypseSummoned		= false;	// ctor values
		m_bApocalypseFinisherDone	= false;

		m_bXXDownXUsed				= false;
		//}} 2026-09-04 X2GUAdd leftovers LIFE-7 //

		ThirdPath_StigmaClearMark();	// batch3: stops the debuff ES + resets uid/type/pos/duration/timer (official f_324EFB80 family)

		m_bMoonlightActive		= false;
		m_fMoonlightTimer		= 0.f;
		m_fMoonlightPower		= 0.f;
		m_bMoonlightDPConsumed	= false;
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() && INVALID_EFFECTSET_HANDLE != m_hMoonlightWaveES )
			g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hMoonlightWaveES );
		m_hMoonlightWaveES		= INVALID_EFFECTSET_HANDLE;

#ifdef X2_ADD_2ND_PATH
		m_iFactoryMarkedSkillID		= 0;
		m_iFactoryPhase				= 0;
		m_bFactoryFlagWait			= true;
		m_bFactoryModeEntered		= false;
		if ( NULL != g_pX2Game )
		{
			if ( NULL != g_pX2Game->GetEffectSet() )
			{
				if ( INVALID_EFFECTSET_HANDLE != m_hFactoryMiscES )
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hFactoryMiscES );
#endif
	}
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
			if ( NULL != g_pX2Game->GetDamageEffect() )
			{

				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hFactoryDroneDE, false );
				//}} 2026-09-04 X2GUAdd reverify FACT-1 //
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hInstall9015DE, false );
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hInstall9019DE, false );
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hInstall9020DE, false );
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hInstall9022DE, false );
			}
		}
		m_hFactoryDroneDE			= INVALID_DAMAGE_EFFECT_HANDLE;
		m_hFactoryMiscES			= INVALID_EFFECTSET_HANDLE;
		m_hInstall9015DE			= INVALID_DAMAGE_EFFECT_HANDLE;
		m_hInstall9019DE			= INVALID_DAMAGE_EFFECT_HANDLE;
		m_hInstall9020DE			= INVALID_DAMAGE_EFFECT_HANDLE;
		m_hInstall9022DE			= INVALID_DAMAGE_EFFECT_HANDLE;
#ifdef X2_ADD_2ND_PATH

		ThirdPath_UltimateFuryClearSet();
		//}} 2026-09-06 MARCH2015 Ultimate Fury attack driver (IDA L4) reinit //
#endif
#endif

		if ( NULL != g_pX2Game )
		{
			if ( NULL != g_pX2Game->GetEffectSet() )
			{
				if ( INVALID_EFFECTSET_HANDLE != m_hVoidFieldLoopES )
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hVoidFieldLoopES );
				if ( INVALID_EFFECTSET_HANDLE != m_hStardustLoopES )
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hStardustLoopES );
				if ( INVALID_EFFECTSET_HANDLE != m_hSpaceCrackES )
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hSpaceCrackES );
			}
			if ( NULL != g_pX2Game->GetDamageEffect() )
			{
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hVoidFieldDE, false );
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hStardustFieldDE, false );
			}
			if ( NULL != g_pX2Game->GetMajorXMeshPlayer() && INVALID_MESH_INSTANCE_HANDLE != m_hStardustMesh )
				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hStardustMesh );
		}
		m_hVoidFieldDE			= INVALID_DAMAGE_EFFECT_HANDLE;
		m_hVoidFieldLoopES		= INVALID_EFFECTSET_HANDLE;
		m_bVoidFieldEndBurst	= false;
		m_fVoidFieldLoopTime	= 0.f;
		m_hStardustFieldDE		= INVALID_DAMAGE_EFFECT_HANDLE;
		m_hStardustLoopES		= INVALID_EFFECTSET_HANDLE;
		m_hStardustMesh			= INVALID_MESH_INSTANCE_HANDLE;
		m_iStardustDummyCounter	= 1;
		m_fStardustSpawnAccum	= 0.f;
		m_fStardustDuration		= 0.f;
		m_fStardustLoopTime		= 0.f;
		m_hSpaceCrackES			= INVALID_EFFECTSET_HANDLE;
		m_bSpaceCrackActive		= false;
		m_bSpaceCrackFieldOn	= false;
		m_fSpaceCrackPower		= 0.f;

		ThirdPath_MaxStrikeTeardown( false );
		//}} 2026-09-04 X2GUAdd leftovers LIFE-7 (maxstrike) //
		//}} 2026-09-04 X2GUAdd reverify LIFE-7 //
	}


	if ( ThirdPath_IsApocalypseSummonActive() )
		ThirdPath_ApocalypseFinishSummon();
#ifdef X2_ADD_2ND_PATH
	/// AT/MM -> erase the factory-mode buff (official: f_31FB08A0(0x156); the UnSet-342 hook cascades to ThirdPath_FactoryModeClear)
	if ( CX2Unit::UC_ADD_ARC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_MASTERMIND == GetUnitClass() )
		EraseBuffTempletFromGameUnit( BTI_BUFF_DYNAMO_FACTORY );
#endif

#ifdef X2_ADD_3RD_PATH

	if ( NULL != GetUnit() && CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnit()->GetClass() )
		CreateDEFormationFaceObj();
	else
		ReleaseDEFormationFaceObj();
	//}} 2026-09-07 //
#endif
#endif
}
/*virtual*/ void CX2GUAdd::DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ )
{
	KTDXPROFILE();

	if ( NULL == pDamageData_ )
		return;

	/// ���� DP ����� ���� �Ǿ��ٸ� �ǰ� ����� ������ ������, �������� ���� �ʴ´�.
	if ( true == IsPossibleChangeNasodBarrier( pDamageData_ ) )
	{
		SetDamageReducePercent( 100 );		/// ������ ������ 100% ����
		CX2GUUser::DamageReact( pDamageData_ );
		SetDamageReducePercent( -100 );		/// ������ ������ ����
	}
	else
	{
		CX2GUUser::DamageReact( pDamageData_ );
		CX2GUUser::DamageReactStateChange( pDamageData_ );
	}

	/// ���ǹ� ������ ���� ó��
	AttackQuicksilverFrenzy( *pDamageData_ );

	StopLoopSound();
}
/*virtual*/ void CX2GUAdd::Win()
{
	CX2GUUser::Win();
	StateChange( GetWinStateID() );
}
/*virtual*/ void CX2GUAdd::Lose()
{
	CX2GUUser::Lose();

	if ( true == IsMyUnit() && 0.f < GetNowHp() )
	{
		StateChange( GetLoseStateID() );
	}
}
/*virtual*/ void CX2GUAdd::Draw()
{
	CX2GUUser::Draw();
	StateChange( GetWinStateID() );
}
/*virtual*/ void CX2GUAdd::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();
}
/*virtual*/ void CX2GUAdd::InitEnchantWeaponEffectUnitSpecific()
{
	for( Weapon* pWeapon : m_vecpWeapon )
	{
		ASSERT( NULL != pWeapon );
		pWeapon->InitEnchantParticleForBow();
	}
}

/** @function	: AttackResultByType
	@brief		: ���ݽ� �߰� ��� ó�� �Լ�
	@param		: ������ ������
*/
void CX2GUAdd::AttackResultByType( CX2DamageManager::DamageData &pDamageData )
{
	CX2GUUser::AttackResultByType( pDamageData );

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH

	ThirdPath_DynamoLockAcquire( &pDamageData );
	//}} 2026-09-04 X2GUAdd reverify LOCK-2 //
#endif
#else
	/// ���ҵ� �尩 �޺��� Ÿ�� ���� ����
#endif
	if ( true == IsMyUnit() )
		SetNasodArmorAttackTarget( pDamageData );

	/// ���̳ʸ� ���� - ���׳�Ʈ�� ���� ó��
	AttackMagnetronDamageEffect( pDamageData );

	/// ����Ʈ �Ƴ���� Ÿ�� ���� ����
	SetDustAnatomyTarget( pDamageData );

	/// ���ǹ� ������ ���� ó��
	AttackQuicksilverFrenzy( pDamageData, true );
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


	ThirdPath_MaxStrikeRecordTarget( &pDamageData );
	ThirdPath_MaxStrikeFireOnHit( &pDamageData );
	//}} 2026-09-04 X2GUAdd leftovers LOCK-2b //
#endif
}

/*virtual*/ void CX2GUAdd::CreateMajorParticleArray()
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
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RED,		L"DamageImpactRed",			2 );
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
/*virtual*/ void CX2GUAdd::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();
}
/*static*/ void CX2GUAdd::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
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
/*static*/ void CX2GUAdd::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{

}
/*virtual*/ bool CX2GUAdd::SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )
{
	if( NULL == pSkillTemplet_ )
		return false;

	switch( pSkillTemplet_->m_eID )
	{
	case CX2SkillTree::SI_SA_APT_STASIS_FIELD:
		{
			if ( 0.f < m_StasisfieldData.m_fRemainTime )
				m_FrameDataFuture.syncData.nowAction = UAI_ACTIVE_STASIS_FIELD;
			else
				return false;
		}
		break;
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH

	case CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE:
		{
			/// official case 9025: barrage handles pending -> suppress the state re-entry AND pop one staged ball early (official newslot_328 case 9025 arms nowAction 38 -> newslot_327 case 38 -> f_32503B90 = pop-front + fire + destroy placeholder); inline because our engine has no KPT command word (batch6 convention)
			if ( false == m_vecMaxStrikeHandles.empty() )
			{

				ThirdPath_MaxStrikeFireOnHit( NULL );
				//}} 2026-09-05 //
				return true;
			}

			return false;
		}
#endif
#ifdef X2_ADD_2ND_PATH
	case CX2SkillTree::SI_HA_AMM_APOCALYPSE:
		{
			/// official case 9601: summon live -> suppress the re-entry + finish the summon inline (no hyper cast-anim replay; official arms nowAction 35 -> newslot_327 case 35 = the finisher; inline because our engine has no KPT command word ��� batch6 convention)
			if ( true == ThirdPath_IsApocalypseSummonActive() )
			{

				if ( false == GetRidingOn() )
				{
					ThirdPath_ApocalypseFinishSummon();	// C-FACT-3: the marker clear moved into the finisher's found-branch (official f_324EF3D0) - NOT unconditional here
				}
				//}} 2026-09-05 //
				return true;
			}

			return false;
		}
	//}} 2026-09-04 X2GUAdd reverify CAST-6 //
#endif
#endif
	default:
		return false;
		break;
	}
	return true;
}
/*virtual*/ void CX2GUAdd::COMMON_BUFF_FrameMove()
{
	if( true == m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		PlayCommonBuffMinorParticle();
		UpDownCrashCamera( 30.0f, 0.4f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

	}
	else if( true == m_pXSkinAnim->EventTimerOneshot( 0.25f ) )
	{
		int	iSkillSlotIndex = 0;
		bool bSlotB = false;

		if ( true == GetSkillSlotIndexUsed( iSkillSlotIndex, bSlotB ) )
		{
			const CX2Unit::UnitData& pUnitData = GetUnit()->GetUnitData();

			const CX2UserSkillTree::SkillSlotData* pSkillSlotData = pUnitData.m_UserSkillTree.GetSkillSlot( iSkillSlotIndex, bSlotB );
			CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_NONE;

			if( NULL != pSkillSlotData )
				eSkillID = pSkillSlotData->m_eID;

			ProcessAcceleraterBuff( eSkillID, IsHyperState() );
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::FormationModeStart()
{
	CommonStateStart();

	if ( NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChange( m_ptrRidingPet->m_WaitState );

	/// ���� ��� ��ȯ�� �ʿ��� DP��
	const float fUseDPByFormationMode = GetUseDPByFormationMode();

	UpNowDPValue( -fUseDPByFormationMode );

	/// ȯ�� ������ �������̶��, �Ҹ��� DP�� �������� MP�� ġȯ
	if ( true == m_bEnableFantasyTracer )
	{
		/// ���� DP�� UI�� ǥ��� ��ġ���� 20�� ũ�Ƿ�, 20�� ���� �ش�.
		const float fUpMPValue = fUseDPByFormationMode * m_fFantasyTracerUpMPRateByFormationMode / 20.f;

		UpNowMp( fUpMPValue );	/// MP ȸ��

		/// MP ȸ���� ǥ��
		if ( NULL != g_pX2Game )
			g_pX2Game->DisplayAddMPInformation( fUpMPValue, GetPos(), GetDirVector() );
	}

	/// �ʿ����� ��ü - ���� ����� �̵� �ӵ� ���� ����
	m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * m_fBodyOfTranscendenceMoveSpeed );
	m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * m_fBodyOfTranscendenceMoveSpeed );

	/// ���� ������ 1 �߰�
	UpMutationCount( 1 );

	SetRemainHyperModeTime( 99999.f );

	m_bHyperModeUsed							= true;
	m_iLastStateHyperModeCount					= m_FrameDataNow.syncData.m_HyperModeCount;
	m_iHyperModeCountUsed						= m_FrameDataNow.syncData.m_HyperModeCount;
	m_FrameDataNow.syncData.m_HyperModeCount	= 0;
}

/*virtual*/ void CX2GUAdd::HyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.05f, 0.46f );
}
/*virtual*/ void CX2GUAdd::HyperModeBuffEffectStart()
{
	CX2GUUser::HyperModeBuffEffectStart();
}
/** @function	: SetSpecificValueByBuffTempletID
	@brief		: �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param		: �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUAdd::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

 	switch ( eBuffTempletId_ )
 	{
	case BTI_FORMATION_MODE:
		{
			/// ���� ��� ����
			SetIsFormationMode( true );

			if ( GetUnitClass() != CX2Unit::UC_ADD_NASOD_RULER )
				SetBuffFactorToGameUnitByBuffFactorID( BFI_NASOD_ARMOR );
		} break;

		/// ���ǹ� ������
	case BTI_BUFF_QUICKSILVER_FRENZY:
		{
			m_QuicksilverFrenzyData.ResetQuicksilverFrenzyData();
			m_QuicksilverFrenzyData.m_bEnableQuicksilverFrenzy = true;
		} break;

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	case BTI_BUFF_DYNAMO_FACTORY:
		{

			if ( 0 != m_iFactoryMarkedSkillID || 2 != m_iFactoryPhase )
				m_bFactoryModeEntered = true;
			else
				ThirdPath_FactoryModeClear();
		} break;
	//}} 2026-08-30 MARCH2015 batch6 //

#endif
 	default:
 		break;
 	}
}
/** @function	: UnSetSpecificValueByBuffTempletID
	@brief		: �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param		: �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUAdd::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_FORMATION_MODE:
		{
			/// ���� ��� ����
			SetIsFormationMode( false );

			if ( GetUnitClass() != CX2Unit::UC_ADD_NASOD_RULER )
				ReserveToFinishBuffTempletFromGameUnit( BTI_NASOD_ARMOR );
		} break;

		/// ���ǹ� ������
	case BTI_BUFF_QUICKSILVER_FRENZY:
		{
			m_QuicksilverFrenzyData.ResetQuicksilverFrenzyData();
		} break;
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)




	case BTI_BUFF_MOONLIGHT_RHAPSODY:
		{
			m_bMoonlightActive		= false;
			m_fMoonlightTimer		= 0.f;
			m_fMoonlightPower		= 0.f;
			m_bMoonlightDPConsumed	= false;
		} break;
	//}} 2026-09-04 X2GUAdd reverify GAUGE-4 //
#endif

	default:
		break;
	}
}

/** @function	: ApplyRenderParam
	@brief		: ������ ���� renderparam�� �Բ� �߰������� �غ��ٽ��� ���� renderparam�� �����ϴ� �Լ�
	@param		: SkinAnim�� RenderParam ������(pRenderParam_)
*/
/*virtual*/ void CX2GUAdd::ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if ( m_bPassDash )
		return;

	switch( m_eCustomRenderState )
	{
	case CRS_COMBO_DASH_X:
		{
			m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON;
			m_RenderParam.outLineColor.a	= m_fRenderParamColorA;
			m_RenderParam.color.a			= 0.f;
			m_RenderParam.bAlphaBlend		= true;
		} break;
	case CRS_QUICKSILVER_FRENZY_BUFF:
		{
			m_RenderParam.renderType	= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
			m_RenderParam.outLineColor	= D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );
			m_RenderParam.color			= SetQuicksilverFrenzyFlickerColor( D3DXCOLOR( 0.5f, 0.3f, 1.f, 1.f ) );
		} break;
	default:
		{
			CX2GameUnit::ApplyRenderParam( pRenderParam_ );
		} break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������Ʈ �Լ�
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ANSI_DIE_FRONT
void CX2GUAdd::DieFrontStartFuture()
{
	CommonStateStartFuture();
}
void CX2GUAdd::DieFrontStart()
{
	CommonStateStart();
	DyingStart();
	m_pXSkinAnim->SetPlaySpeed( 0.5f );
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	if ( ThirdPath_IsApocalypseSummonActive() )
	{
		ThirdPath_ApocalypseFinishSummon();

	}
	//}} 2026-09-04 X2GUAdd reverify BASE-1 //
#endif
}
void CX2GUAdd::DieFrontFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}
void CX2GUAdd::DieFrontEventProcess()
{
	if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
	{
		StateChange( GUSI_DIE );
	}
}

//ANSI_DIE_BACK
void CX2GUAdd::DieBackStartFuture()
{
	CommonStateStartFuture();
}
void CX2GUAdd::DieBackStart()
{
	CommonStateStart();
	DyingStart();
	m_pXSkinAnim->SetPlaySpeed( 0.5f );
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	if ( ThirdPath_IsApocalypseSummonActive() )
	{
		ThirdPath_ApocalypseFinishSummon();

	}
	//}} 2026-09-04 X2GUAdd reverify BASE-1 //
#endif
}
void CX2GUAdd::DieBackFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}
void CX2GUAdd::DieBackEventProcess()
{
	if ( GetShowObject() == false  && m_pXSkinAnim->IsAnimationEnd() == true )
	{
		StateChange( GUSI_DIE );
	}
}

//ANSI_WAIT
void CX2GUAdd::WaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStartByAdd( m_pXSkinAnimFuture, m_FrameDataFuture );
}
void CX2GUAdd::WaitStart()
{
	CommonStateStart();
	PlayAnimationWaitStartByAdd( m_pXSkinAnim.get(), m_FrameDataNow );

	m_CommonState.m_Wait = USI_WAIT;

	/// ���ҵ� �Ƹ� �޺� Ÿ�� ���� �ʱ�ȭ
	m_NasodArmorAttackTargetData.ResetTargetData();
}
void CX2GUAdd::WaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_COMBO_X );
	}
	else if( true == SpecialAttackEventProcess() )
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
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
				LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
				&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( true == GetIsFormationMode() )
	{
		StateChange( ANSI_HYPER_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}
	CommonEventProcess();
}

//ANSI_HYPER_WAIT
void CX2GUAdd::HyperWaitStartFuture()
{
	CommonStateStartFuture();
}
void CX2GUAdd::HyperWaitStart()
{
	CommonStateStart();

	/// ���ҵ� �Ƹ� �޺� Ÿ�� ���� �ʱ�ȭ
	m_NasodArmorAttackTargetData.ResetTargetData();

	m_CommonState.m_Wait = ANSI_HYPER_WAIT;
}
void CX2GUAdd::HyperWaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_COMBO_X );
	}
	else if( true == SpecialAttackEventProcess() )
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
	ELSE_IF_CAN_RELEASE_FORMATION_MODE
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( false == GetIsFormationMode() )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}
	CommonEventProcess();
}

//ANSI_WALK
void CX2GUAdd::WalkEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
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
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

//ANSI_JUMP_READY
void CX2GUAdd::JumpReadyEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() || true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}

	CommonEventProcess();
}

//ANSI_JUMP_UP
void CX2GUAdd::JumpUpEventProcess()
{
	if( ( m_InputData.oneDoubleRight == true || m_InputData.oneDoubleLeft == true ) &&
		true == m_bEnableAirDash )
	{
		StateChange( ANSI_AIR_DASH );
		m_bEnableAirDash = false;
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( ( m_InputData.oneUp || true == m_InputData.oneDoubleUp ) && true == m_bEnableLevitationWait )
	{
		StateChange( ANSI_LEVITATION_WAIT );
		m_bEnableLevitationWait = false;
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ANSI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ANSI_JUMP_ATTACK_X );
	}
	else if( 0.0f >= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

//ANSI_JUMP_DOWN
void CX2GUAdd::JumpDownEventProcess()
{
	if( ( m_InputData.oneDoubleRight == true || m_InputData.oneDoubleLeft == true ) &&
		true == m_bEnableAirDash )
	{
		StateChange( ANSI_AIR_DASH );
		m_bEnableAirDash = false;
	}
	else if ( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( ( m_InputData.oneUp || true == m_InputData.oneDoubleUp ) && true == m_bEnableLevitationWait )
	{
		StateChange( ANSI_LEVITATION_WAIT );
		m_bEnableLevitationWait = false;
	}
	else if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ANSI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ANSI_JUMP_ATTACK_X );
	}
	else if ( 0.0f <= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

//ANSI_JUMP_LANDING
void CX2GUAdd::JumpLandingStartFuture()
{
	CommonStateStartFuture();
}
void CX2GUAdd::JumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChange( USI_WALK );
	}
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChange( USI_WALK );
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ANSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ANSI_COMBO_X );
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_WALK );
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		StateChange( USI_WAIT, false );
	}

	CommonEventProcess();
}

//ANSI_DASH
void CX2GUAdd::DashEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if( true == m_bEnableAirDash && ( true == m_InputData.pureRight || true == m_InputData.pureLeft ) )
		{
			StateChange( ANSI_AIR_DASH );

			m_bEnableAirDash = false;
		}
		else
		{
			StateChange( USI_DASH_JUMP );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_DASH_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_DASH_COMBO_X );
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.oneUp )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE
	else if( false == m_InputData.pureRight && false == m_InputData.pureLeft )
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
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_DASH_JUMP );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

//ANSI_AIR_DASH
void CX2GUAdd::AirDashEventProcess()
{
	if( 0.45 < m_FrameDataFuture.unitCondition.fStateTime )
	{
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_DASH_JUMP_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_DASH_JUMP_COMBO_X );
	}
	else if ( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( true == m_InputData.oneUp )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	else if( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_DASH_JUMP );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

void CX2GUAdd::AirDashStateEndFuture()
{
	if ( 800.f < m_PhysicParam.nowSpeed.x )
		m_PhysicParam.nowSpeed.x = 800.f;

	CommonStateEndFuture();
}

//ANSI_DASH_END
void CX2GUAdd::DashEndStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUAdd::DashEndFrameMove()
{
	if( m_pXSkinAnim->EventTimer( 0.4f ) == true )
		CreateStepDust();

	CommonFrameMove();
}
void CX2GUAdd::DashEndEventProcess()
{
	// DashEnd���� ���� Ű�� ������ �� ��� ��������, �Ϲ� ���������� ���ϱ� ���� const ��.

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true)
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
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE
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

//ANSI_DASH_JUMP
void CX2GUAdd::DashJumpStartFuture()
{
	CommonStateStartFuture();

	const float MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL = 1.2f;

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL;
	m_bDownForce					= false;

}
void CX2GUAdd::DashJumpStart()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
		CreateStepDust();
}
void CX2GUAdd::DashJumpFrameMoveFuture()
{
	const float MAGIC_NUMBER_MULTIPLY_DASH_SPEED = 1.6f;

	if( m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight	== true )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}
	else if( m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight	== false )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}

	CommonFrameMoveFuture();
}
void CX2GUAdd::DashJumpEventProcess()
{
	bool &bCanDashJump = m_FrameDataFuture.stateParam.bEventFlagList[0];

	if( true == IsOnSomethingFuture() )
	{

		if ( true == m_bDownForce )
		{
			StateChange( ANSI_DASH_JUMP_POWER_LANDING );
			m_bDownForce = false;
		} // if
		else
		{
			if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
			{
				StateChangeDashIfPossible();
			}
			else
				StateChange( USI_DASH_JUMP_LANDING );

			m_bDownForce = false;
		}
	}
	else if( m_InputData.oneUp && true == m_bEnableAirDash )
	{
		StateChange( ANSI_AIR_DASH );
		m_bEnableAirDash = false;
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		StateChange( ANSI_WALL_LANDING );
		m_bDisableGravity = true;
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ANSI_DASH_JUMP_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ANSI_DASH_JUMP_COMBO_X );
	}
	else if( true == m_InputData.pureRight )
	{
		if( false == m_FrameDataFuture.syncData.bIsRight )
		{
			if( GetJumpSpeed() < m_PhysicParam.nowSpeed.y )
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( 0.0f < m_PhysicParam.nowSpeed.y )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}
	else if( true == m_InputData.pureLeft )
	{
		if( true == m_FrameDataFuture.syncData.bIsRight )
		{
			if( GetJumpSpeed() < m_PhysicParam.nowSpeed.y )
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			if( 0.0f < m_PhysicParam.nowSpeed.y )
				StateChange( USI_JUMP_UP );
			else
				StateChange( USI_JUMP_DOWN );
		}
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}
	else if ( ( m_InputData.oneDoubleRight == true || m_InputData.oneDoubleLeft == true ) &&
			  true == m_bEnableAirDash )
	{
		StateChange( ANSI_AIR_DASH );
		m_bEnableAirDash = false;
	}
	else if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
		bCanDashJump = false;
	}

	CommonEventProcess();
}
void CX2GUAdd::DashJumpEndFuture()
{
	CommonStateEndFuture();
	m_PhysicParam.fDownAccel	= 0.0f;
}

//ANSI_DASH_JUMP_LANDING
void CX2GUAdd::DashJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ANSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ANSI_COMBO_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_DASH);
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( USI_JUMP_READY );
	}
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUAdd::DashJumpLandingStartFuture()
{
	CX2GUUser::DashJumpLandingStartFuture();
}

/// ANSI_LEVITATION_WAIT
void CX2GUAdd::LevitationWaitFrameMove()
{
	/// �Һ� = �ʴ� MP 2 * ������Ʈ �����ð�
	const float fConsumeMP = _CONST_ADD_::LEVITATION_CONSUME_MP_PER_SECOND * m_fElapsedTime * m_FrameDataNow.unitCondition.fStateTime;

	FlushMp( fConsumeMP );

	CommonFrameMove();
}

void CX2GUAdd::LevitationWaitEventProcess()
{
	if( true == m_InputData.oneDown || 1.f > GetNowMp() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( 0.f >= GetNowHp() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ANSI_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ANSI_JUMP_ATTACK_X );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( ( m_InputData.oneDoubleRight == true || m_InputData.oneDoubleLeft == true ) &&
			 true == m_bEnableAirDash )
	{
		StateChange( ANSI_AIR_DASH );
		m_bEnableAirDash = false;
	}
	else if( m_InputData.oneRight == true || m_InputData.oneLeft == true )
	{
		StateChange( USI_WALK );
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( m_InputData.oneDown == true )
	{
		if( NULL != g_pX2Game &&
			NULL != g_pX2Game->GetWorld() &&
			NULL != g_pX2Game->GetWorld()->GetLineMap() &&
			true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
			LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
			&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}

	CommonEventProcess();
}

//ANSI_DAMAGE_SMALL_FRONT
//ANSI_DAMAGE_SMALL_BACK
void CX2GUAdd::DamageSmallEventProcess()
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

//ANSI_DAMAGE_BIG_FRONT
void CX2GUAdd::DamageBigStart()
{
	CommonStateStart();
	CreateStepDust();
}
void CX2GUAdd::DamageBigEventProcess()
{
	float fGapFromUnitToLineMap = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
	const float ACCEPTABLE_GAP = 200.0f;	// �̰ͺ��� �Ÿ��� ũ�� ���߿� �ִ� ������

	if( ACCEPTABLE_GAP < fGapFromUnitToLineMap )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( 0.93f < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			StateChange( USI_WAIT );

		}
	}

	CommonEventProcess();
}

//ANSI_DAMAGE_DOWN_FRONT
void CX2GUAdd::DamageDownFrontFrameMove()
{
	if( true == m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
		CreateStepDust();

	CommonFrameMove();
}
void CX2GUAdd::DamageDownFrontEventProcess()
{
	bool& bChangeStandUpAttackFront			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingFrontFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingFrontBack	= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpBigAttackFront		= m_FrameDataFuture.stateParam.bEventFlagList[4];

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	// �Ѿ��� ��
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == bChangeStandUpAttackFront )
		{
			StateChange( ANSI_STAND_UP_ATTACK_FRONT );
		}
		else if ( true == bChangeStandUpBigAttackFront )
		{
			StateChange( ANSI_STAND_UP_ATTACK_FRONT );
		}
		else if( true == bChangeStandUpRollingFrontFront )
		{
			StateChange( ANSI_STAND_UP_ROLLING_FRONT_FRONT );
		}
		else if( true == bChangeStandUpRollingFrontBack )
		{
			StateChange( ANSI_STAND_UP_ROLLING_FRONT_BACK );
		}
		else
		{
			StateChange( USI_DAMAGE_STANDUP_FRONT );
		}
	}
	else if( 0.33f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if( true == m_InputData.oneZ )
		{
			bChangeStandUpAttackFront		= true;
			bChangeStandUpRollingFrontFront	= false;
			bChangeStandUpRollingFrontBack	= false;
			bChangeStandUpBigAttackFront	= false;

		}
		else if ( true == m_InputData.oneX )
		{
			bChangeStandUpAttackFront		= false;
			bChangeStandUpRollingFrontFront	= false;
			bChangeStandUpRollingFrontBack	= false;
			bChangeStandUpBigAttackFront	= true;
		}
		// ���� ������ ���ϰ� �ִ� ��쿡
		else if( true == m_FrameDataFuture.syncData.bIsRight )
		{
			// ���� ȭ��ǥ Ű���带 ���� ���
			if( true == m_InputData.pureRight )
			{
				// ������ �Ѹ�
				bChangeStandUpAttackFront		= false;
				bChangeStandUpRollingFrontBack	= false;
				bChangeStandUpRollingFrontFront	= true;
				bChangeStandUpBigAttackFront	= false;

			}
			// ���� ȭ��ǥ Ű���带 ���� ���
			else if( true == m_InputData.pureLeft )
			{
				// �ڷ� �Ѹ�
				bChangeStandUpAttackFront		= false;
				bChangeStandUpRollingFrontFront	= false;
				bChangeStandUpRollingFrontBack	= true;
				bChangeStandUpBigAttackFront	= false;

			}
		}
		// ���� ������ ���ϰ� �ִ� ��쿡
		else
		{
			// ���� ȭ��ǥ Ű���带 ���� ���
			if( true == m_InputData.pureRight )
			{
				// �ڷ� �Ѹ�
				bChangeStandUpAttackFront		= false;
				bChangeStandUpRollingFrontFront	= false;
				bChangeStandUpRollingFrontBack	= true;
				bChangeStandUpBigAttackFront	= false;

			}
			else if( true == m_InputData.pureLeft )
			{
				// ���� �Ѹ�
				bChangeStandUpAttackFront		= false;
				bChangeStandUpRollingFrontBack	= false;
				bChangeStandUpRollingFrontFront	= true;
				bChangeStandUpBigAttackFront	= false;
			}
		}
	}

	CommonEventProcess();
}

//ANSI_DAMAGE_DOWN_BACK
void CX2GUAdd::DamageDownBackFrameMove()
{
	if( true == m_pXSkinAnim->EventTimer( 0.32f ) )
		CreateStepDust();

	CommonFrameMove();
}
void CX2GUAdd::DamageDownBackEventProcess()
{
	bool& bChangeStandUpAttackBack			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingBackFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingBackBack		= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpBigAttackBack		= m_FrameDataFuture.stateParam.bEventFlagList[4];

	// �Ѿ����µ� �ؿ� ���θ� ���� ���� ���
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	// �Ѿ��� ��
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// Z Ű�� ������ ��
		if( true == bChangeStandUpAttackBack )
		{
			StateChange( ANSI_STAND_UP_ATTACK_BACK );
		}
		// X Ű�� ������ ��
		else if ( true == bChangeStandUpBigAttackBack )
		{
			StateChange( ANSI_STAND_UP_ATTACK_BACK );
		}
		else if( true == bChangeStandUpRollingBackFront )
		{
			StateChange( ANSI_STAND_UP_ROLLING_BACK_FRONT );
		}
		else if( true == bChangeStandUpRollingBackBack )
		{
			StateChange( ANSI_STAND_UP_ROLLING_BACK_BACK );
		}
		else
		{
			StateChange( USI_DAMAGE_STANDUP_BACK );
		}
	}

	else if( 0.33f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if( true == m_InputData.oneZ )
		{
			bChangeStandUpAttackBack		= true;
			bChangeStandUpRollingBackFront	= false;
			bChangeStandUpRollingBackBack	= false;
			bChangeStandUpBigAttackBack		= false;
		}
		else if ( true == m_InputData.oneX )
		{
			bChangeStandUpAttackBack		= false;
			bChangeStandUpRollingBackFront	= false;
			bChangeStandUpRollingBackBack	= false;
			bChangeStandUpBigAttackBack		= true;
		}
		// ���� ������ ���ϰ� �ִ� ��쿡
		else if( true == m_FrameDataFuture.syncData.bIsRight )
		{
			// ���� ȭ��ǥ Ű���带 ���� ���
			if( true == m_InputData.pureRight )
			{
				// ������ �Ѹ�
				bChangeStandUpAttackBack		= false;
				bChangeStandUpRollingBackBack	= false;
				bChangeStandUpRollingBackFront	= true;
				bChangeStandUpBigAttackBack		= false;

			}
			// ���� ȭ��ǥ Ű���带 ���� ���
			else if( true == m_InputData.pureLeft )
			{
				// �ڷ� �Ѹ�
				bChangeStandUpAttackBack		= false;
				bChangeStandUpRollingBackFront	= false;
				bChangeStandUpRollingBackBack	= true;
				bChangeStandUpBigAttackBack		= false;

			}
		}
		// ���� ������ ���ϰ� �ִ� ��쿡
		else
		{
			// ���� ȭ��ǥ Ű���带 ���� ���
			if( true == m_InputData.pureRight )
			{
				// �ڷ� �Ѹ�
				bChangeStandUpAttackBack		= false;
				bChangeStandUpRollingBackFront	= false;
				bChangeStandUpRollingBackBack	= true;
				bChangeStandUpBigAttackBack		= false;

			}
			else if( true == m_InputData.pureLeft )
			{
				// ���� �Ѹ�
				bChangeStandUpAttackBack		= false;
				bChangeStandUpRollingBackBack	= false;
				bChangeStandUpRollingBackFront	= true;
				bChangeStandUpBigAttackBack		= false;
			}
		}
	}

	CommonEventProcess();
}

//ANSI_DAMAGE_AIR_DOWN_LANDING
void CX2GUAdd::DamageAirDownLandingEventProcess()
{
	DamageDownFrontEventProcess();
}

//ANSI_STANDUP_ROLLING_FRONT_FRONT
void CX2GUAdd::StandUpRollingFrontFrontEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
		{
			StateChange( USI_WALK );
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUAdd::StandUpRollingFrontFrontEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ANSI_STANDUP_ROLLING_FRONT_BACK
void CX2GUAdd::StandUpRollingFrontBackEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
		{
			StateChange( USI_WALK );
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUAdd::StandUpRollingFrontBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ANSI_STANDUP_ROLLING_BACK_FRONT
void CX2GUAdd::StandUpRollingBackFrontEventProcess()
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
			StateChange( USI_WALK );
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUAdd::StandUpRollingBackFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ANSI_STANDUP_ROLLING_BACK_BACK
void CX2GUAdd::StandUpRollingBackBackEventProcess()
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
			StateChange( USI_WALK );
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUAdd::StandUpRollingBackBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ANSI_STANDUP_ATTACK_FRONT
void CX2GUAdd::StandUpAttackFrontEventProcess()
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
			StateChange( USI_WALK );
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUAdd::StandUpAttackFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ANSI_STANDUP_ATTACK_BACK
void CX2GUAdd::StandUpAttackBackEventProcess()
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
			StateChange( USI_WALK );
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUAdd::StandUpAttackBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}
void CX2GUAdd::StandUpAttackBackStateEndFuture()
{
	m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
	CommonStateEndFuture();
}

//ANSI_DASH_JUMP_POWER_LANDING
void CX2GUAdd::DashJumpPowerLandingStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x /= 2.0f;
}
void CX2GUAdd::DashJumpPowerLandingStart()
{
	CommonStateStart();
	CreateStepDust();
}
void CX2GUAdd::DashJumpPowerLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneRight == true || m_InputData.oneLeft == true )
	{
		StateChange( USI_WALK );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 6.0f )
	{
		if( m_InputData.oneZ == true )
		{
			StateChange( ANSI_COMBO_Z );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( ANSI_COMBO_X );
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
		//ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 10.f, ESSI_REVENGE_WAIT )
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


//#pragma region Ŀ�ǵ� �Լ�

/// ANSI_COMBO_Z
void CX2GUAdd::ComboZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_ZZ )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_ZZ
void CX2GUAdd::ComboZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_ZZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_ZZX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_ZZZ
void CX2GUAdd::ComboZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_ZZZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_ZZZX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_ZZZZ
void CX2GUAdd::ComboZZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_ZZZX
void CX2GUAdd::ComboZZZXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/// ANSI_COMBO_ZZX
void CX2GUAdd::ComboZZXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
#ifdef X2_ADD_3RD_PATH

	else if( true == m_InputData.oneX && m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		if ( CX2Unit::UC_ADD_TIME_TRACER == GetUnitClass() || CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnitClass() )
			StateChange( ANSI_ATT_COMBO_ZZXX );
		else
			StateChange( ANSI_COMBO_ZZXX );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
#else
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_ZZXX )
#endif
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_ZZXX
void CX2GUAdd::ComboZZXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_X
void CX2GUAdd::ComboXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_XX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_XX
void CX2GUAdd::ComboXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
#ifdef X2_ADD_2ND_PATH

	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		StateChange( ANSI_COMBO_XXX );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[2].stateChange )
	{
		StateChange( ANSI_AMM_COMBO_XXDownX_START );
		m_FrameDataFuture.stateParam.bEventFlagList[2] = false;
	}
	else if( m_InputData.pureDown == true && m_InputData.oneX == true && false == m_bXXDownXUsed && CX2Unit::UC_ADD_MASTERMIND == GetUnitClass() &&
								 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[2].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[2].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[2] = true;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[1].stateChange )
	{
		StateChange( ANSI_COMBO_XXZ );
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( true == m_InputData.oneZ && false == m_bXXDownXUsed &&
								 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[1].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[1].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	else if( true == m_InputData.oneX &&
								 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
#else
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_XXX )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_XXZ )
#endif
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_XXX
void CX2GUAdd::ComboXXXEventProcess()
{
#ifdef X2_ADD_3RD_PATH
	/// Play DamageEffect EffectSet at anim time 0.366 (missing from lua EFFECT_SET_LIST_EX)
	if ( m_pXSkinAnimFuture->GetNowAnimationTime() < 0.05f )
		m_bComboXXXDamageEffectPlayed = false;
	if ( false == m_bComboXXXDamageEffectPlayed && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.41f )
	{

		m_bComboXXXDamageEffectPlayed = true;
		if ( GetDPValue() >= 2.0f )
		{
			SetDPValue( GetDPValue() - 2.0f );

			const WCHAR* pszComboXXXES = ( NULL != GetUnit() && CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnit()->GetClass() )
				? L"EffectSet_ADE_Combo_XXX_Projectile" : L"EffectSet_AN_Combo_XXX_Projectile";
			g_pX2Game->GetEffectSet()->PlayEffectSet( pszComboXXXES, this, NULL, false, GetPowerRate() );
			//}} 2026-09-05 //
		}
		//}} 2026-09-04 X2GUAdd reverify C09 D1 //
	}

#endif
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
#ifdef X2_ADD_3RD_PATH

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									( true == GetIsFormationMode() && CX2Unit::UC_ADD_DIABOLIC_ESPER != GetUnitClass() ), m_NasodArmorAttackTargetData.m_uidTargetUID )
	else if( true == m_InputData.oneZ && CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnitClass() &&
			 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[1].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[1].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[1].stateChange )
	{
		StateChange( true == GetIsFormationMode() ? ANSI_ADE_COMBO_XXXZ_FORMATION_MODE : ANSI_ADE_COMBO_XXXZ_NO_FORMATION_MODE );
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( true == m_InputData.oneX && CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnitClass() &&
			 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[2].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[2].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[2] = true;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[2].stateChange )
	{
		StateChange( true == GetIsFormationMode() ? ANSI_ADE_COMBO_XXXX_FORMATION_MODE : ANSI_ADE_COMBO_XXXX_NO_FORMATION_MODE );
		m_FrameDataFuture.stateParam.bEventFlagList[2] = false;
	}
#else
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
#endif
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_XXZ
void CX2GUAdd::ComboXXZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_XXZZ_START )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_XXZZ_START
void CX2GUAdd::ComboXXZZStartEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if(  0.456f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		StateChange( ANSI_COMBO_XXZZ_LOOP );
	}

	CommonEventProcess();
}

void CX2GUAdd::ComboXXZZStartStateEndFuture()
{
	CommonStateEndFuture();
	/// ������Ʈ ���� ��� ���带 ������� �ֱ� ���� �ڵ忡�� ó��
	PlayLoopSound(L"Add_Effect05.ogg", true, false );
}

/// ANSI_COMBO_XXZZ_LOOP
void CX2GUAdd::ComboXXZZLoopStateStart()
{
	CommonStateStart();

	SetNoDetonation( true );
}

void CX2GUAdd::ComboXXZZLoopEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		++ m_iComboLoopCount;

		/// �ּ� �ݺ� Ƚ���� �������� ���ϰų�, �ִ� �ݺ� Ƚ�� �ȿ��� Ű�� ������ ���� �� �ݺ� ������Ʈ�� ��ȯ
		if ( _CONST_ADD_::COMBO_XXZZ_MIN_COUNT - 1 > m_iComboLoopCount ||
			 ( _CONST_ADD_::COMBO_XXZZ_MAX_COUNT - 2 >= m_iComboLoopCount && true == m_FrameDataFuture.stateParam.bEventFlagList[0]) )
			StateChange( ANSI_COMBO_XXZZ_LOOP );
		else
			StateChange( ANSI_COMBO_XXZZ_FINISH );
	}
	ELSE_IF_SET_EVENT_FLAG( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, true == m_InputData.oneZ )

	CommonEventProcess();
}

void CX2GUAdd::ComboXXZZLoopStateEnd()
{
	CommonStateEnd();

	SetNoDetonation( false );
}

/// ANSI_COMBO_XXZZ_FINISH
void CX2GUAdd::ComboXXZZFinishStateStartFuture()
{
	CommonStateStartFuture();

	m_iComboLoopCount = 0;	/// �޺� �ݺ��� �ʱ�ȭ

	/// ��¡ ���� ����
	StopLoopSound();

	CommonStateStartFuture();
}

void CX2GUAdd::ComboXXZZFinishStateStart()
{
	CommonStateStart();
}

void CX2GUAdd::ComboXXZZFinishEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_JUMP_ATTACK_Z
void CX2GUAdd::JumpComboZEventMoveFuture()
{
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( m_InputData.pureDown == true && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonFrameMoveFuture();
}

void CX2GUAdd::JumpComboZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_JUMP_ATTACK_X
void CX2GUAdd::JumpComboXEventMoveFuture()
{
	if( m_InputData.pureDown == true && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonFrameMoveFuture();
}

void CX2GUAdd::JumpComboXEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_COMBO_Z
void CX2GUAdd::DashComboZFrameMoveFuture()
{
	if( m_InputData.pureDown == true && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonFrameMoveFuture();
}

void CX2GUAdd::DashComboZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_DASH_COMBO_ZZ )
#ifdef X2_ADD_2ND_PATH

	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
								 m_InputData.oneX == true && ( CX2Unit::UC_ADD_ARC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_MASTERMIND == GetUnitClass() ) &&
								 ( ( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true ) || ( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false ) ), ANSI_AAT_DASH_COMBO_ZBackX )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true && ( CX2Unit::UC_ADD_ARC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_MASTERMIND == GetUnitClass() ), ANSI_AAT_DASH_COMBO_ZX )
	//}} 2026-09-04 X2GUAdd reverify C09 D7 //
#endif
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_COMBO_ZZ
void CX2GUAdd::DashComboZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_DASH_COMBO_ZZZ )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_COMBO_ZZZ
void CX2GUAdd::DashComboZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_COMBO_X
void CX2GUAdd::DashComboXStateStart()
{
	m_eCustomRenderState	= CRS_NONE;

	CommonStateStart();
}

void CX2GUAdd::DashComboXFrameMove()
{
	CommonFrameMove();

	/// Ư�� �ִϸ��̼� �ð� ������ ������!

	if( 0.085f <= m_pXSkinAnim->GetNowAnimationTime() && 0.43f > m_pXSkinAnim->GetNowAnimationTime() )
	{
		m_eCustomRenderState	= CRS_COMBO_DASH_X;
	}
}

void CX2GUAdd::DashComboXEventProcess()
{
	if ( false == IsOnSomethingFuture() && 0.45f <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_DASH_COMBO_XX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUAdd::DashComboXStateEnd()
{
	m_fRenderParamColorA	= 1.f;		/// ���İ� �ʱ�ȭ

	CommonStateEnd();
}

void CX2GUAdd::DashComboXStateEndFuture()
{
	/// �ڷ� ���� �ּ���!
	CommonStateStartFuture();

	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}

/// ANSI_DASH_COMBO_XX
void CX2GUAdd::DashComboXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_DASH_COMBO_XXX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_COMBO_XXX
void CX2GUAdd::DashComboXXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_JUMP_COMBO_Z
void CX2GUAdd::DashJumpComboZEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_DASH_JUMP_COMBO_ZZ )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_JUMP_COMBO_ZZ
void CX2GUAdd::DashJumpComboZZEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_DASH_JUMP_COMBO_ZZZ )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_JUMP_COMBO_ZZZ
void CX2GUAdd::DashJumpComboZZZEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_JUMP_COMBO_X
void CX2GUAdd::DashJumpComboXEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_DASH_JUMP_COMBO_XX_START )
#ifdef X2_ADD_2ND_PATH

	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneZ == true && CX2Unit::UC_ADD_MASTERMIND == GetUnitClass(), ANSI_AMM_DASH_JUMP_COMBO_XZ )
	//}} 2026-09-04 X2GUAdd reverify C09 D8 //
#endif
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_DASH_JUMP_COMBO_XX_START
void CX2GUAdd::DashJumpComboXXStartEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_DASH_JUMP_COMBO_XX_CHARGE );
	}

	CommonEventProcess();
}

void CX2GUAdd::DashJumpComboXXStartStateEndFuture()
{
	CommonStateEndFuture();

	/// ��¡ ���� ���
	PlayLoopSound( L"Add_Effect10.ogg", true, false );
}

/// ANSI_DASH_JUMP_COMBO_XX_CHARGE
void CX2GUAdd::DashJumpComboXXLoopStateStart()
{
	CommonStateStart();

	m_fChargeTime = 0.f;	/// ���� �ð� �ʱ�ȭ
	SetNoDetonation( true );
}

void CX2GUAdd::DashJumpComboXXLoopFrameMove()
{
	m_fChargeTime += m_fElapsedTime;

	CommonFrameMove();
}

void CX2GUAdd::DashJumpComboXXLoopEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	/// X�� ������ ���� �ʰų�, �ִ� ���� �ð��� ������ �߻� ������Ʈ�� �̵�
	else if ( false == m_InputData.pureX || _CONST_ADD_::DASH_JUMP_COMBO_X2_MAX_REMAIN_TIME <= m_FrameDataNow.unitCondition.fStateTime )
	{
		StateChange( ANSI_DASH_JUMP_COMBO_XX_FINISH );
	}

	CommonEventProcess();
}

void CX2GUAdd::DashJumpComboXXLoopStateEnd()
{
	CommonStateEnd();

	SetNoDetonation( false );
}

/// ANSI_DASH_JUMP_COMBO_XX_FINISH
void CX2GUAdd::DashJumpComboXXFinishStateStartFuture()
{
	CommonStateStartFuture();

	/// ��¡ ���� ����
	StopLoopSound();
}

void CX2GUAdd::DashJumpComboXXFinishFrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			float fChargeRate = 1.f;		/// ���� ����

			/// �����̶��, 100% ������ ����
			if ( _CONST_ADD_::DASH_JUMP_COMBO_X2_MAX_REMAIN_TIME <= m_fChargeTime )
				fChargeRate = 2.f;
			else	/// ������ �ƴ϶��, �ִ� 75% ������ ����
				fChargeRate = 1.f + ( 0.25f * m_fChargeTime );

			/// �߰� ������ ����
			const float fPowerRate = GetPowerRate() * fChargeRate;

			CX2DamageEffect::CEffect* pDamageEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AN_Dash_Jump_Combo_XX_Bullet", fPowerRate, GetBonePos( L"EF01" ),
															  GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			/// �߻�ü ũ�� ����
			if ( NULL != pDamageEffect && NULL != pDamageEffect->GetMainEffect() )
				pDamageEffect->SetScale( pDamageEffect->GetMainEffect()->GetScale() * fChargeRate );
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::DashJumpComboXXFinishEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_NP_BARRIER_ACTIVE
void CX2GUAdd::NasodBarrierActiveStateStartFuture()
{
	/// DP �Ҹ�
	UpNowDPValue( - m_fUseDPByNasodBarrier );

	CommonStateStartFuture();
}

void CX2GUAdd::NasodBarrierActiveEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_READY,
void CX2GUAdd::NasodArmorReadyEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_COMBO_NASOD_ARMOR_ATTACK );
	}

	/// ĵ���Ǹ� ȯ�� ������ �̻� ���� �Ѵ�... ��ȹ�����ε� ĵ�� ������ ���ٰ� ������, ���� ������

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_ATTACK,
void CX2GUAdd::NasodArmorAttackStartFuture()
{
	CommonStateStartFuture();

	bool		bIsNPC			= m_NasodArmorAttackTargetData.m_bIsNPC;		/// NPC �� ����
	UidType		uidTargetUID	= m_NasodArmorAttackTargetData.m_uidTargetUID;	/// Ÿ�� UID

	/// ��ų�� ���� ���ҵ� �Ƹ� �޺��� ������� ��, �ش� ���� ��ü�� �������
	if ( CX2SkillTree::SI_NONE != m_eNasodArmorAttackActiveSkillID )
	{
		bool bIsSetTargetData = false;	/// Ÿ�� ���� ����

		/// Ÿ�� ���� �˻� �� ����
		for( FantasyTracerTargetInfo& TargetInfo : m_vecFantasyTracerTargetInfo )
		{
			if ( m_eNasodArmorAttackActiveSkillID == TargetInfo.m_eSkillID )
			{
				bIsNPC			= TargetInfo.m_NATargetData.m_bIsNPC;
				uidTargetUID	= TargetInfo.m_NATargetData.m_uidTargetUID;

				bIsSetTargetData = true;
			}
		}

		/// Ÿ�� ���� ���н�, �н�
		if ( false == bIsSetTargetData )
			return;
	}

	const CX2GameUnit* pUnit = NULL;		/// Ÿ�� ���

	if ( true == bIsNPC )	/// NPC�� ��
	{
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( static_cast<int>( uidTargetUID ) );

		if ( NULL != pNPC )
			pUnit = static_cast<CX2GameUnit*>( pNPC );		/// Ÿ�� ����
	}
	else					/// ������ ��
	{
		CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( uidTargetUID );

		if ( NULL != pUser )
			pUnit = static_cast<CX2GameUnit*>( pUser );		/// Ÿ�� ����
	}

	/// �װų� ���� ���̰ų� �����̸�, ����
	if ( NULL == pUnit ||
		 0.f >= pUnit->GetNowHp() ||
		 pUnit->GetTeam() == GetTeam() ||
		 true == pUnit->GetInvincible() )
		return;

	/// ��ȿ�� ���θ����� �˻�
	if ( NULL == g_pX2Game ||
		 NULL == g_pX2Game->GetWorld() ||
		 NULL == g_pX2Game->GetWorld()->GetLineMap() )
		 return;

	const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pUnit->GetLastTouchLineIndex() );

	if( NULL == pLineData )
		return;

	if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false ) //���θ��� ��ȿ���� üũ
		return;

	if( pUnit->GetPos().y < pLineData->startPos.y )
		return;

	/// ��� ������ �����ϹǷ�, ��ġ ����
	D3DXVECTOR3 vOffSetMovePos;		/// OffSet�� ������ ���� �̵� ��ġ

	/// �ڷ� OffSet ��ŭ ��ġ�� ���θ� ���� ���� �˻�, �����ϸ� �׳� OffSet ���� ���ڸ��� ����
	if ( true == g_pX2Game->GetWorld()->GetLineMap()->GetFixedDistancePositionForWeaponTaker( &pUnit->GetPos(), 100.f, !GetIsRight(), vOffSetMovePos ) )
	{
		vOffSetMovePos.y = pUnit->GetPos().y;

		m_FrameDataFuture.syncData.position = vOffSetMovePos;
	}
	else
	{
		m_FrameDataFuture.syncData.position	= pUnit->GetPos();
	}
}

void CX2GUAdd::NasodArmorAttackEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
#ifdef X2_ADD_3RD_PATH

		if ( CX2Unit::UC_ADD_TIME_TRACER == GetUnitClass() || CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnitClass() )
			StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_LANDING );
		else
#endif
		StateChange( ANSI_COMBO_NASOD_ARMOR_LANDING );
	}

	/// ĵ���Ǹ� ȯ�� ������ �̻� ���� �Ѵ�... ��ȹ�����ε� ĵ�� ������ ���ٰ� ������, ���� ������

	CommonEventProcess();
}

///	ANSI_COMBO_NASOD_ARMOR_LANDING,
void CX2GUAdd::NasodArmorLandingEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
								 m_InputData.oneZ == true && ( CX2Unit::UC_ADD_NASOD_RULER == GetUnitClass() || CX2Unit::UC_ADD_PSYCHIC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_LUNATIC_PSYKER == GetUnitClass() ), ANSI_COMBO_NASOD_ARMOR_Z )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true && ( CX2Unit::UC_ADD_NASOD_RULER == GetUnitClass() || CX2Unit::UC_ADD_PSYCHIC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_LUNATIC_PSYKER == GetUnitClass() ), ANSI_COMBO_NASOD_ARMOR_X )
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
								 m_InputData.oneZ == true && ( CX2Unit::UC_ADD_NASOD_RULER == GetUnitClass() || CX2Unit::UC_ADD_ARC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_MASTERMIND == GetUnitClass() ), ANSI_AAT_COMBO_NASOD_ARMOR_Z )
	ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange,
								 m_InputData.oneX == true && ( CX2Unit::UC_ADD_NASOD_RULER == GetUnitClass() || CX2Unit::UC_ADD_ARC_TRACER == GetUnitClass() || CX2Unit::UC_ADD_MASTERMIND == GetUnitClass() ), ANSI_AAT_COMBO_NASOD_ARMOR_X )
#else
								 m_InputData.oneZ == true, ANSI_COMBO_NASOD_ARMOR_Z )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_NASOD_ARMOR_X )
#endif

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUAdd::NasodArmorLandingEndFuture()
{
	/// Ÿ�� ����
	const CX2GameUnit* pTargetUnit = m_NasodArmorAttackTargetData.GetTargetUnit();

	/// ��ų�� ���� ���ҵ� �Ƹ� �޺��� ������� ��, �ش� ���� ��ü�� �������
	if ( CX2SkillTree::SI_NONE != m_eNasodArmorAttackActiveSkillID )
	{
		bool bIsSetTargetData = false;	/// Ÿ�� ���� ����

		/// Ÿ�� ���� �˻� �� ����
		for( FantasyTracerTargetInfo& TargetInfo : m_vecFantasyTracerTargetInfo )
		{
			if ( m_eNasodArmorAttackActiveSkillID == TargetInfo.m_eSkillID )
			{
				pTargetUnit		= TargetInfo.m_NATargetData.GetTargetUnit();

				bIsSetTargetData = true;
			}
		}

		/// ���ݽ� ����� Ÿ�� ��ų ���̵� �ʱ�ȭ
		m_eNasodArmorAttackActiveSkillID = CX2SkillTree::SI_NONE;

		/// Ÿ�� ���� ���н�, �н�
		if ( false == bIsSetTargetData )
			return;
	}

	/// Ÿ���� ������ �ٶ󺸵��� ����
	if ( NULL != pTargetUnit )
	{
		const bool	bRight	= IsRightTarget( pTargetUnit->GetPos() );	/// Ÿ���� �ִ� ����
		bool		bFlip	= false;									/// ���� ��ȯ ����

		/// Ÿ���� ������ �ٶ󺸰� �ִ��� �˻�
		if( ( false == bRight	&& true		== GetIsRight() ) ||
			( true	== bRight	&& false	== GetIsRight() ) )
		{
			bFlip = true;
		}

		/// Ÿ���� �ٶ󺸰� ���� �ʴٸ�, ���� ��ȯ ����
		if ( true == bFlip )
			m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	}

	CommonStateEndFuture();
}

/// ANSI_COMBO_NASOD_ARMOR_Z
void CX2GUAdd::NasodArmorComboZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_NASOD_ARMOR_ZZ )

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_ZZ
void CX2GUAdd::NasodArmorComboZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
								( CX2Unit::UC_ADD_LUNATIC_PSYKER == GetUnitClass() || CX2Unit::UC_ADD_LUNATIC_PSYKER == GetUnitClass() ) &&
								m_InputData.oneZ == true &&
								( ( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ||
								( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) )
								, ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_START )

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_NASOD_ARMOR_ZZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_NASOD_ARMOR_ZZX )

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_ZZZ
void CX2GUAdd::NasodArmorComboZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_ZZX
void CX2GUAdd::NasodArmorComboZZXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_NASOD_ARMOR_ZZXX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_ZZXX
void CX2GUAdd::NasodArmorComboZZXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_X
void CX2GUAdd::NasodArmorComboXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 ( CX2Unit::UC_ADD_LUNATIC_PSYKER == GetUnitClass() || CX2Unit::UC_ADD_LUNATIC_PSYKER == GetUnitClass() ) &&
								 m_InputData.oneX == true &&
								 ( ( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ||
								   ( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) )
								, ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP )

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_NASOD_ARMOR_XX )

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUAdd::NasodArmorComboXStateEndFuture()
{
	m_iComboLoopCount		= 0;			/// �޺� ī��Ʈ �ʱ�ȭ

	CommonStateEndFuture();
}

/// ANSI_COMBO_NASOD_ARMOR_XX
void CX2GUAdd::NasodArmorComboXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_NASOD_ARMOR_XXX )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_NASOD_ARMOR_XXX
void CX2GUAdd::NasodArmorComboXXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

//#pragma endregion //Ŀ�ǵ� �Լ�


//#pragma region ���м� ��Ƽ��

/// ���̵� �극��Ŀ
void CX2GUAdd::ANSI_SA_AN_VOID_BREAKER_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA02.tga");
	TextureReadyInBackground(L"VOID_BREAKER_A.tga");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.x");
	XMeshReadyInBackground(L"ADD_ShockWave003.Y");
	XMeshReadyInBackground(L"ADD_MAGNETRON_DYNAMO_Spark2.Y");
	XMeshReadyInBackground(L"Condense_Light01.dds");
}

void CX2GUAdd::ANSI_SA_AN_VOID_BREAKER_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 vPos	= GetBonePos( L"EF01" );
		vPos.x				+= 30.f;

		float fPowerRate = GetPowerRate();
		float fScaleRate = 1.f;
		/// ���� ����Ʈ�� �ִٸ�, ���ݷ°� ũ�⸦ ���� ��Ű��
		if ( 0 < GetMutationCount() && true == GetIsFormationMode() )
		{
			/// ���� ����Ʈ �Ҹ� ���
			const int iUseMutationCount = min( GetMutationCount(), _CONST_ADD_::CONQUEROR_MAX_USE_MUTATION_COUNT );

			/// ���ݷ� ����
			const float fSADamageValue	= GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_REL, CX2SkillTree::SI_SA_AN_VOID_BREAKER ) - 1.f;
			const float fDamageValue	= static_cast<float>( ( fSADamageValue * iUseMutationCount ) + 1.f );
			fPowerRate *= fDamageValue;

			// ũ�� ����
			const float fSAScaleValue	= GetSpecialAbilityValue( CX2SkillTree::SA_SIZE_ABS, CX2SkillTree::SI_SA_AN_VOID_BREAKER ) - 1.f ;
			const float fSizeValue		= static_cast<float>( ( fSAScaleValue * iUseMutationCount ) + 1.f );
			fScaleRate *= fSizeValue;

			UpMutationCount( -iUseMutationCount );	/// ���� ����Ʈ ����
		}

		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AN_Void_Breaker_Projectile", fPowerRate,
																				vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		/// ũ�� ����
		if( false == IsSamef(fScaleRate,1.f) &&
			NULL != pDamageEffect )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = pDamageEffect->GetMainEffect();
			if ( NULL != pMesh )
			{
				pMesh->SetScale( pMesh->GetScale() * fScaleRate );
			}
		}

	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_AN_VOID_BREAKER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ��ƼŬ ������
void CX2GUAdd::ANSI_SA_AN_PARTICLE_PRISM_Init()
{
	XSkinMeshReadyInBackground(L"AN_PARTICLE_PRISM_Dummy01.X");
	XMeshReadyInBackground(L"AN_PARTICLE_PRISM_04.Y");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Add_Gravity_Ball.tga");
	TextureReadyInBackground(L"ADD_CYCLE_COA01.Tga");
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
}

void CX2GUAdd::ANSI_SA_AN_PARTICLE_PRISM_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		DestroyMinePrismNPC();
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hParticlePrsim );

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			float fRotateRadianY = D3DXToRadian( GetRotateDegree().y ) + 1.57f;	/// ȸ����

			/// �� NPC ��ȯ
			D3DXVECTOR3 vStartNPCPos = GetPos();
			vStartNPCPos.z += 50.f * cos( fRotateRadianY );
			vStartNPCPos.x += 50.f * sin( fRotateRadianY );

			int iHyperMode = true == IsHyperState() ? 1 : 0;
			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_PARTICLE_PRISM_START, iHyperMode, true, vStartNPCPos,
				GetIsRight(), 0.f, true, -1, static_cast<CX2Room::TEAM_NUM>(GetTeam()), CX2NPCAI::NAT_ALLY, GetUnitUID() );

			/// �� NPC ��ȯ
			D3DXVECTOR3 vEndNPCPos		= GetPos();

			/// ���� ũ�⿡ ���� ���� ����
			const float	fOffSetLength	= 650.f * m_vTransScale.x;

			vEndNPCPos.z += fOffSetLength * cos( fRotateRadianY );
			vEndNPCPos.x += fOffSetLength * sin( fRotateRadianY );

			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_PARTICLE_PRISM_END, iHyperMode, true, vEndNPCPos,
				GetIsRight(), 0.f, true, -1, static_cast<CX2Room::TEAM_NUM>(GetTeam()), CX2NPCAI::NAT_ALLY, GetUnitUID() );


			// ����Ʈ ����
			{
				/// ���� ����Ʈ�� �ִٸ�, ����� ȿ�� ��ȭ
				bool bUasMutationPoint = false;
				if ( 0 < GetMutationCount() && true == GetIsFormationMode() )
				{
					bUasMutationPoint = true;
					SetMutationCount( GetMutationCount() - 1);
				}

				m_hParticlePrsim =
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_AN_PARTICLE_PRISM", this,
					NULL, bUasMutationPoint, GetPowerRate(), -1.f, D3DXVECTOR3(1,1,1),
					true, vStartNPCPos, GetRotateDegree(), GetDirVector() );
			}

		}
	}
	CommonFrameMove();
}
void CX2GUAdd::ANSI_SA_AN_PARTICLE_PRISM_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
/** @function : IsExistingMinePrismNPC
	@brief : �� ������ ������ NPC �Ҹ��Ű��
*/
void CX2GUAdd::DestroyMinePrismNPC() const
{
	for( int i=0; i < g_pX2Game->GetNPCUnitListSize(); ++i)
	{
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnit(i);
		if( NULL != pNPC )
		{
			UidType uiOwnerUnitUID = pNPC->GetOwnerGameUnitUID();
			if( -1 != uiOwnerUnitUID &&
				GetUnitUID() == uiOwnerUnitUID )
			{
				switch(pNPC->GetNPCTemplet().m_nNPCUnitID)
				{
				case CX2UnitManager::NUI_PARTICLE_PRISM_START:
				case CX2UnitManager::NUI_PARTICLE_PRISM_END:
					{
						pNPC->SetNowHp(0.f);
					} break;
				default:
					break;
				}
			}
		}
	}
}


/// ������ ��
void CX2GUAdd::ANSI_SA_AN_ENERGY_BOOM_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA01.tga" );
	TextureReadyInBackground(L"ADD_CYCLE_COA02.tga" );
	TextureReadyInBackground(L"ADD_CYCLE_COA03.tga" );
	TextureReadyInBackground(L"Add_ENERGY_BOOM_A.tga" );
	XMeshReadyInBackground( L"Up_Impact01.Y" );
	XMeshReadyInBackground( L"ADD_Effect_Line02.Y" );
	XSkinMeshReadyInBackground( L"ADD_Effect_Shock_01.x" );
	XSkinMeshReadyInBackground( L"ADD_ENERGY_BOOM_Stone.X" );
	XSkinMeshReadyInBackground( L"ADD_ENERGY_BOOM_Stone02.X" );
	XSkinMeshReadyInBackground( L"ADD_ENERGY_BOOM_A.X" );
	XMeshReadyInBackground( L"ADD_ShockWave003.Y" );
	TextureReadyInBackground(L" Black_Circle01.dds" );
	TextureReadyInBackground(L" Black_Light.dds" );
	TextureReadyInBackground(L" EffectSet_Riding_Dark_HAMELLING_Skill_C03.tga" );
	TextureReadyInBackground(L" add_EFFECT_Cycle_Blur01.dds" );
}

void CX2GUAdd::ANSI_SA_AN_ENERGY_BOOM_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.527f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01" );

		/// ���� ������ ����Ʈ
		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SI_SA_AN_Energy_Boom_Projectile", GetPowerRate(),
												  vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		/// ���� ����Ʈ�� �ִٸ�, ��Ÿ��� ���� ��Ű��
		if ( 0 < GetMutationCount() && NULL != pDamageEffect && true == GetIsFormationMode() )
		{
			/// ���� �Ÿ� ����
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = pDamageEffect->GetMainEffect();

			if ( NULL != pMesh )
			{
				const float fSARangeValue	= GetSpecialAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, CX2SkillTree::SI_SA_AN_ENERGY_BOOM );
				const float fIncreaseRange	= pMesh->GetMaxLifeTime() * fSARangeValue;

				pMesh->SetMaxLifeTime( fIncreaseRange );
			}

			UpMutationCount( -1 );	/// ���� ����Ʈ �Ҹ�
		}
	}

	CommonFrameMove();
}
void CX2GUAdd::ANSI_SA_AN_ENERGY_BOOM_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ��ƼŬ �׼���������
void CX2GUAdd::ANSI_SA_AN_PARTICLE_ACCELERATOR_Init()
{
	XSkinMeshReadyInBackground(L"ADD_PARTICLE_ACCELERATOR_DUMMY01.X");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK01.Y" );
	XSkinMeshReadyInBackground( L"SA_AEM_CHAIN_LIGHTNING_P01.X" );
}

void CX2GUAdd::ANSI_SA_AN_PARTICLE_ACCELERATOR_FrameMove()
{
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	bool bMemoHyperAccel = false;
	bMemoHyperAccel = ( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ADD_MEMO05 ) );
	if ( true == bMemoHyperAccel &&
		 0.01f < m_FrameDataNow.unitCondition.fStateTime &&
		 true == IsAddSkillKeyHeld() )	// official: the per-slot key-held check (f_31E934E0 on the key manager) = our SAKP-poll (C13 DI-4 role-equal)
	{
		++m_iPAMemoCharge;
	}

	if( m_pXSkinAnim->EventTimerOneshot( ( true == bMemoHyperAccel ) ? 1.15f : 1.35f ) )
	//}} 2026-09-04 X2GUAdd reverify SA1-4 //
#else
	if( m_pXSkinAnim->EventTimerOneshot( 1.35f ) )
#endif
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			float fPowerRate = GetPowerRate();

			/// ���� �������� �ִٸ�, ������ ���� ó�� ( 3�� ���� )
			if ( 0 < GetMutationCount() && true == GetIsFormationMode() )
			{
				/// ���� ����Ʈ �Ҹ� ���
				const int iUseMutationCount = min( GetMutationCount(), _CONST_ADD_::PARTICLE_ACCELERATOR_MAX_USE_MUTATION_COUNT );

				const float fIncreaseDamageRate = GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_REL, CX2SkillTree::SI_SA_AN_PARTICLE_ACCELERATOR ) - 1.f;
				fPowerRate *= ( ( fIncreaseDamageRate * iUseMutationCount ) + 1.f ) ;
				UpMutationCount( -iUseMutationCount );
			}

			D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Spine1" );

			/// ���� ������ ����Ʈ
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SA_AN_Particle_Accelerator_Attack", fPowerRate,
																					vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			/// ȿ���� ����Ʈ ���
			SetEffectSoundParticle( GetBonePos( L"Bip01" ), L"ADD_STR_Kwagwang" );
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_AN_PARTICLE_ACCELERATOR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ��Ʈ�� ��
void CX2GUAdd::ANSI_SA_AN_NEUTRON_BOMB_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA01.tga");
	TextureReadyInBackground(L"ADD_Effect_CIRCLE01.tga");
	TextureReadyInBackground(L"Add_Gravity_Ball.tga");
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"NEUTRON_BOMB_Impact.tga");
	TextureReadyInBackground(L"PlusHPMP.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

void CX2GUAdd::ANSI_SA_AN_NEUTRON_BOMB_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// �޽� ĳ��
void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_READY_Init()
{
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect01.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect02.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect03.X");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Add_Gravity_Ball.tga");
}

void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_READY_StateStartFuture()
{
	/// Ű �Է� ���� ��ü �ʱ�ȭ
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );

	CommonStateStartFuture();
}

void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_READY_StateStart()
{
	/// �޽� ĳ�� �߻� �ε��� ����
	vector<int> vecTempIndex = m_vecPulseBulletFireIndex;
	m_vecPulseBulletFireIndex.resize( 0 );

	/// 1���� 6������ ���� ���Ƿ� �����Ѵ�. ( Sync�� ����, ���� ����� ����� ����... )
	for( int i = static_cast<int>( vecTempIndex.size() ); 0 < i; --i )
	{
		const int iRandomNumb = RandomInt( 1, i );

		if( static_cast<UINT>( iRandomNumb ) > vecTempIndex.size() )
			continue;

		m_vecPulseBulletFireIndex.push_back( vecTempIndex[iRandomNumb -1] );

		vecTempIndex.erase( vecTempIndex.begin() + iRandomNumb - 1 );
	}

	m_iComboLoopCount = 0;	/// �ݺ� ī��Ʈ �ʱ�ȭ

	CommonStateStart();
}

void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_READY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_SA_APT_PULSE_CANNON_LOOP );
	}

	CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_LOOP_FrmaeMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		/// �޽� ĳ�� �߻� �ε��� ���� ( 7��° �̻��� 0�� �ε������� )
		int iWeaponIndex = ( 6 <= m_iComboLoopCount ) ? m_iComboLoopCount - 6 : m_iComboLoopCount;

		/// ���� �ε����� �Ѿ�ٸ�, 0���� ����
		if ( m_vecPulseBulletFireIndex.size() < static_cast<UINT>( iWeaponIndex + 1 ) )
			iWeaponIndex = 0;

		/// ���� ���� ���� ��ġ ����
		WCHAR buf[256] = {0,};

		StringCchPrintf( buf, 255, L"Weapon0%d", m_vecPulseBulletFireIndex[ iWeaponIndex ] );	/// Weapon01 ~ Weapon06

		D3DXVECTOR3 vPos = GetBonePos( buf );


		/// ������ ����Ʈ ����
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SA_APT_Pulse_Cannon_Projectile", GetPowerRate(), vPos,


			GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_LOOP_EventProcess()
{
	/// Ű �Է� ���� ��ü���� ������ ���� �Ǿ� �ִ��� �˻�
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		++m_iComboLoopCount;

		const bool bIsUnPushKey		= m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData );

		if ( _CONST_ADD_::PULSE_CANNON_MIN_FIRE_COUNT > m_iComboLoopCount ||
			 ( _CONST_ADD_::PULSE_CANNON_MAX_FIRE_COUNT > m_iComboLoopCount && false == bIsUnPushKey &&
			   true == ConsumeMPorDP( _CONST_ADD_::PULSE_CANNON_CONSUME_MP_VALUE, _CONST_ADD_::PULSE_CANNON_CONSUME_DP_VALUE ) ) )
		{
			StateChange( ANSI_SA_APT_PULSE_CANNON_LOOP );
		}
		else
		{
			StateChange( ANSI_SA_APT_PULSE_CANNON_FINISH );
		}
	}

	CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_APT_PULSE_CANNON_FINISH_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// EMP ��ũ
void CX2GUAdd::ANSI_SA_APT_EMP_SHOCK_Init()
{
	TextureReadyInBackground(L"ADD_Effect_Triangle01.Tga");
	TextureReadyInBackground(L"Arme_Aging_Gate01.tga");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"EFFECT_Cycle_LensFlare01.dds");
}

void CX2GUAdd::ANSI_SA_APT_EMP_SHOCK_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.961f ) )
	{
		if ( NULL != g_pX2Game && NULL != GetUnit() )
		{
			/// ��ų ���� ��ȯ
			const CX2UserSkillTree& userSkillTree	= GetUnit()->GetUnitData().m_UserSkillTree;
			const int				iSkillLevel		= userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_APT_EMP_SHOCK );

			for( CX2GameUnit* pGameUnit : g_pX2Game->GetUnitList() )
			{
				if ( NULL == pGameUnit )
					continue;

				/// ���� ���̸� �н�
				if ( GetTeam() == pGameUnit->GetTeam() )
					continue;


				/// NPC�� ��, �Ϲ� �� NPC�� �ƴ϶�� �н�
				if( GUT_NPC == pGameUnit->GetGameUnitType() )
				{
					CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pGameUnit );

					if ( NULL == pNPC || CX2UnitManager::NCT_BASIC != pNPC->GetNPCTemplet().m_ClassType )
						continue;
				}

				/// ������ ���� ����� ����
				const float fDistance = GetDistance( GetPos(), pGameUnit->GetPos() );

				if ( _CONST_ADD_::EMP_SHOCK_NEAR_RANGE >= fDistance )		/// ���� Ÿ�� ����
					pGameUnit->SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_EMP_SHOCK_NEAR, iSkillLevel );
				else if ( _CONST_ADD_::EMP_SHOCK_FAR_RANGE >= fDistance )	/// ���� Ÿ�� ����
					pGameUnit->SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_EMP_SHOCK_FAR, iSkillLevel );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_APT_EMP_SHOCK_EventProcess()
{
	bool& bUseEmpShokOnLine = m_FrameDataFuture.stateParam.bEventFlagList[1];

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == bUseEmpShokOnLine && true == IsOnSomethingFuture() )
			StateChange( USI_JUMP_LANDING );
		else
			StateChange( USI_WAIT );
	}
	else if( 0.3 > m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		bUseEmpShokOnLine = IsOnSomethingFuture();
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ����ũ ������
void CX2GUAdd::ANSI_SA_APT_QUAKE_BUSTER_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA01.Tga");
	TextureReadyInBackground(L"ADD_Effect_Triangle01.Tga");
	TextureReadyInBackground(L"ADD_ShockWave.dds");
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

void CX2GUAdd::ANSI_SA_APT_QUAKE_BUSTER_EventProcess()
{
	if ( true == IsOnSomethingFuture() && 1.2f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
void CX2GUAdd::ANSI_SA_APT_QUAKE_BUSTER_FrameMove()
{
	CommonFrameMove();

	const float fEventTimer = ( true == IsHyperState() ) ? 1.f: 0.6f;
	if( m_pXSkinAnim->EventTimerOneshot( fEventTimer ) )
	{
		float fPowerRate = GetPowerRate();
		if ( 0 < GetMutationCount() && true == GetIsFormationMode() )
		{
			const float fChangePowerRate =
				GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_REL, CX2SkillTree::SI_SA_APT_QUAKE_BUSTER );

			if( 0 < fChangePowerRate )
				fPowerRate *= fChangePowerRate;

			// ����ũ ����Ʈ�� ��������Ʈ 1�� ���
			SetMutationCount( GetMutationCount() - 1 );
		}

		if( true == IsHyperState() )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Quake_Buster_Hyper", this, NULL, IsHyperState(), fPowerRate );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Quake_Buster", this, NULL, IsHyperState(), fPowerRate );
	}
}
void CX2GUAdd::ANSI_SA_APT_QUAKE_BUSTER_HYPER_EventProcess()
{
	if ( true == IsOnSomethingFuture() && 1.4f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// �����̽ý� �ʵ�
void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_Init()
{
	TextureReadyInBackground(L"ADD_ShockWave.dds");
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"ADD_CYCLE_COA03.tga");
	TextureReadyInBackground(L"ADD_Dark_Cycle_Attack_2.tga");
	TextureReadyInBackground(L"Arme_Aging_Gate01.tga");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"EFFECT_Cycle_LensFlare01.dds");
}

void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_StateStart()
{
	/// �����̽ý� �ʵ� Ȱ�� �� ���� �ð� ����
	m_StasisfieldData.m_fRemainTime	= _CONST_ADD_::STASIS_FIELD_REMAIN_TIME;

	/// ��ų ������ ����
	if ( IsMyUnit() )
		g_pX2Game->UpdateSkillSlotUI();

	CommonStateStart();
}

void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		/// ī��Ʈ �ٿ�� ����Ʈ ����
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			CX2EffectSet::Handle hLoopEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_APT_STASIS_FIELD_Loop", this,
				NULL, false, -1.f, _CONST_ADD_::STASIS_FIELD_REMAIN_TIME );

			if ( INVALID_EFFECTSET_HANDLE != hLoopEffectSet )
				m_StasisfieldData.m_hLoopEffectSet = hLoopEffectSet;
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_ACTIVE_StateStart()
{
	/// �����̽ý� ���� �ð� �ʱ�ȭ
	/// ( ���� �ð��� �ʱ�ȭ �ϴ� ������, ������ ����Ʈ ���� �� ���� �ǰ� �� Ÿ�� �������� �����ؾ� �ϱ� ������ )
	m_StasisfieldData.m_fRemainTime = 0.f;

	/// �ǰ�/Ÿ�ݿ� ���� ��ġ ���� ���� -> ���� : 100~150%
	/// ���� : ( 1 + min( ���� ������/�ڽ��� �ִ�HP , 0.5) )
	const float fTotalValue = m_StasisfieldData.m_fHitValue + m_StasisfieldData.m_fDamageValue;
	m_StasisfieldData.m_fChangeRate = ( 1.f + min( fTotalValue / GetMaxHp(), 0.5f ) );

	/// ��ų ������ ����
	if ( IsMyUnit() )
		g_pX2Game->UpdateSkillSlotUI();

	CommonStateStart();
}

void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_ACTIVE_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ����Ʈ�� ����
		CX2EffectSet::Handle hEffectSet =
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_APT_Stasis_Field_Activate", this);
		CX2EffectSet::EffectSetInstance* pEffectSet =
			g_pX2Game->GetEffectSet()->GetEffectSetInstance(hEffectSet);
		if( NULL != pEffectSet )
		{

			D3DXVECTOR3 vScale = pEffectSet->m_vScale * m_StasisfieldData.m_fChangeRate;
			pEffectSet->SetEffectScale( vScale, true );
		}
	}
	else if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		// ������ ����Ʈ ����
		const float fPowerRate = GetPowerRate() * m_StasisfieldData.m_fChangeRate;
		CX2DamageEffect::CEffect* pDamageEffect =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_APT_Stasis_Field_Activate", fPowerRate,
			GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if( NULL != pDamageEffect )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = pDamageEffect->GetMainEffect();
			if( NULL != pMesh )
			{
				D3DXVECTOR3 vScale = pMesh->GetScale() * m_StasisfieldData.m_fChangeRate ;
				pMesh->SetScale( vScale );
			}
		}
		m_StasisfieldData.ResetStasisfieldData();
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_APT_STASIS_FIELD_ACTIVE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ��Ŀ��
void CX2GUAdd::ANSI_SA_APT_CONQUEROR_Init()
{
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_Spark2.Y" );
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATO_CYCLE1.Y" );
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y" );
	XSkinMeshReadyInBackground(L"ADD_CONQUERER_DUMMY01.X");
}

void CX2GUAdd::ANSI_SA_APT_CONQUEROR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
/** @function : ANSI_SA_APT_CONQUEROR_FrameMove
	@brief : ���� �� �ָ� ����, �񰢼� �� ���� �̻��� ����
*/
void CX2GUAdd::ANSI_SA_APT_CONQUEROR_FrameMove()
{
	CommonFrameMove();

	if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		if( true == GetIsFormationMode() )
		{
			float fPowerRate = GetPowerRate();
			/// ���� ����Ʈ�� �ִٸ�, ���ݷ� ���� ��Ű��
			if ( 0 < GetMutationCount() )
			{
				/// ���� ����Ʈ �Ҹ� ���
				const int iUseMutationCount = min( GetMutationCount(), _CONST_ADD_::CONQUEROR_MAX_USE_MUTATION_COUNT );

				const float fSADamageValue	= GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_REL, CX2SkillTree::SI_SA_APT_CONQUEROR ) - 1.f;
				const float fDamageValue	= static_cast<float>( ( fSADamageValue * iUseMutationCount ) + 1.f );
				fPowerRate *= fDamageValue;

				UpMutationCount( -iUseMutationCount );	/// ���� ����Ʈ ����
			}

			if ( NULL != g_pX2Game->GetDamageEffect() )
			{
				D3DXVECTOR3 vPos;
				GetBonePos( L"Bip01", vPos );
				vPos.x += ( true == GetIsRight() ) ? 300.f : -300.f;
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SA_APT_Conqueror_Hyper", fPowerRate,
					vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			}

		}
		// �밢�� : �����̻���
		else
		{
			int iProjectileCount = 12;

			/// ���� ����Ʈ�� �ִٸ�, �߻�ü ���� ���� ��Ű�� -> �����ÿ��� ���
			/*if ( 0 < GetMutationCount() )
			{
				/// ���� ����Ʈ �Ҹ� ���
				const int iUseMutationCount = min( GetMutationCount(), _CONST_ADD_::CONQUEROR_MAX_USE_MUTATION_COUNT );

				const float fProjectileBaseCount = GetSpecialAbilityValue( CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, CX2SkillTree::SI_SA_APT_CONQUEROR );
				iProjectileCount += static_cast<int>(fProjectileBaseCount) * iUseMutationCount;

				SetMutationCount( -iUseMutationCount );	/// ���� ����Ʈ ����
			}*/

			if ( NULL != g_pX2Game->GetDamageEffect() )
			{
				D3DXVECTOR3 vPos;
				GetBonePos( L"Bip01", vPos );
				vPos.x += ( true == GetIsRight() ) ? 200.f : -200.f;

				for( int i = 0; i < iProjectileCount; ++i )
				{
					D3DXVECTOR3 vRotateDegree = GetRotateDegree();
					vRotateDegree.y += -135.f + GetRandomInt(i) * 2;
					vRotateDegree.z += -135.f + GetRandomInt( iProjectileCount+i ) * 2;

					CX2DamageEffect::CEffect* pDamageEffect =
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SA_APT_Conqueror_Projectile", GetPowerRate(),
						vPos, vRotateDegree, GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

					SetLockOn( pDamageEffect, i, CX2DamageEffect::LOT_UID_VECTOR );
				}
			}
		}
	}
}


/// ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_START
void CX2GUAdd::NasodArmorComboZZFrontZStartEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_iComboLoopCount = 0;	/// �޺� �ݺ� ī��Ʈ �ʱ�ȭ
		StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/// ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP
void CX2GUAdd::NasodArmorComboZZFrontZLoopStateStartFuture()
{
	CommonStateStartFuture();

	/// �ݺ� Ƚ���� ���� �ִϸ��̼� �ӵ� ����
	if ( 0 < m_iComboLoopCount )
	{
		const float fChangeSpeedValue = m_fAnimSpeedFuture + ( 0.1f * m_iComboLoopCount );
		ResetAnimSpeed( m_fAnimSpeedFuture, true, true, fChangeSpeedValue );
	}
}

void CX2GUAdd::NasodArmorComboZZFrontZLoopStateStart()
{
	CommonStateStart();

	/// �ݺ� Ƚ���� ���� �ִϸ��̼� �ӵ� ����
	if ( 0 < m_iComboLoopCount )
	{
		const float fChangeSpeedValue = 1.f + ( 0.1f * m_iComboLoopCount );
		ResetAnimSpeed( fChangeSpeedValue, true, false, fChangeSpeedValue );
	}
}

void CX2GUAdd::NasodArmorComboZZFrontZLoopEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		++ m_iComboLoopCount;

		/// DP ������, ������ ����
		if ( 0.f >= GetDPValue() )
			StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX );
		/// �ּ� �ݺ� Ƚ���� �������� ���ϸ�, Loop ����
		else if ( _CONST_ADD_::NASOD_ARMOR_ZZFrontZ_LOOP_MIN_COUNT > m_iComboLoopCount )
			StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP );
		/// �ִ� �ݺ� Ƚ�� �̸� �̶��
		else if ( _CONST_ADD_::NASOD_ARMOR_ZZFrontZ_LOOP_MAX_COUNT > m_iComboLoopCount )
		{
			/// Z Ű �Է��� �ִٸ�, Loop ����
			if ( true == m_FrameDataFuture.stateParam.bEventFlagList[0] )
				StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP );
			/// �Է� ������ ������ ����
			else
				StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX );
		}
		else
			StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX );
	}
	/// �ݺ� Ƚ�� �̸� �̸鼭 X Ű ������ �ٷ� ������ ����
	else if ( _CONST_ADD_::NASOD_ARMOR_ZZFrontZ_LOOP_MIN_COUNT <= m_iComboLoopCount &&
		_CONST_ADD_::NASOD_ARMOR_ZZFrontZ_LOOP_MAX_COUNT >  m_iComboLoopCount &&
		true == m_InputData.oneX )
		StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX );
	ELSE_IF_SET_EVENT_FLAG( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, true == m_InputData.oneZ )

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/// ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX
void CX2GUAdd::NasodArmorComboZZFrontZXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/// ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP
void CX2GUAdd::NasodArmorComboXDirectionXLoopEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		++ m_iComboLoopCount;

		/// DP ������, ������ ����
		if ( 0.f >= GetDPValue() )
			StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ );
		/// �ִ� �ݺ� Ƚ�� �̸� �̶��
		else if ( _CONST_ADD_::NASOD_ARMOR_XDirectionX_LOOP_MAX_COUNT > m_iComboLoopCount )
		{
			/// X Ű �Է��� �ִٸ�, Loop ����
			if ( true == m_FrameDataFuture.stateParam.bEventFlagList[0] )
				StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP );
			/// �Է� ������ ������ ����
			else
				StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ );
		}
		else
			StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ );
	}
	/// �ݺ� Ƚ�� �̸� �̸鼭 Z Ű ������ �ٷ� ������ ����
	else if ( _CONST_ADD_::NASOD_ARMOR_XDirectionX_LOOP_MAX_COUNT >  m_iComboLoopCount &&
		true == m_InputData.oneZ )
		StateChange( ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ );
	ELSE_IF_SET_EVENT_FLAG( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, true == m_InputData.oneX )

		SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUAdd::NasodArmorComboXDirectionXLoopStateEndFuture()
{
	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;

	CommonStateEndFuture();
}

/// ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ
void CX2GUAdd::NasodArmorComboXDirectionXZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}


/// ������ ������
void CX2GUAdd::ANSI_SA_ALP_REVERSE_REACTOR_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA01.tga");
	TextureReadyInBackground(L"ADD_Effect_Triangle01.Tga");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"ADD_ShockWave.dds");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	TextureReadyInBackground(L"Black_Light.dds");
	XMeshReadyInBackground( L"ADD_STASIS_FIELD_Spark.Y" );
	XMeshReadyInBackground( L"ADD_PHASE_SHIFT_ATTACK01.Y" );
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"ADD_Reverse_Reactor_Link01.X");
}

void CX2GUAdd::ANSI_SA_ALP_REVERSE_REACTOR_StateStart()
{
	m_ReverseReactorTargetInfo.ResetReverseReactorTargetInfo();

	CommonStateStart();
}

void CX2GUAdd::ANSI_SA_ALP_REVERSE_REACTOR_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.31f ) )
	{
		/// Ÿ���� ��� ã�� ( ���̻� ������ �巹�� ���� )
		float fDistance = ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? _CONST_ADD_::REVERSE_REACTOR_ENABLE_ATTACK_RANGE_PVP
																		  : _CONST_ADD_::REVERSE_REACTOR_ENABLE_ATTACK_RANGE;
		CX2GameUnit* pFrontNearestUnit = NULL;

		for( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
		{
			CX2GameUnit *pUnit = g_pX2Game->GetUnit( i );

			if ( NULL == pUnit )
				continue;
			if( g_pX2Game->IsValidUnit( pUnit ) == false )
				continue;
			if( GetTeam() == pUnit->GetTeam() )
				continue;
			if( pUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE ||
				pUnit->GetNowHp() <= 0.f ||
				pUnit->GetInvincible() == true )
				continue;

			if( g_pX2Game->IsFront( (CX2GameUnit*)this, pUnit ) == false )
				continue;

			float fOffset = 0.f;
			if(pUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC)
			{
				CX2GUNPC *pNpc = static_cast<CX2GUNPC*>( pUnit );
				if( NULL == pNpc || CX2UnitManager::NCT_BASIC != pNpc->GetNPCTemplet().m_ClassType )
					continue;

				fOffset = ( pNpc->GetOrgUnitSize().x + pNpc->GetOrgUnitSize().y ) / 4.f;
			}

			D3DXVECTOR3 vDistance = pUnit->GetPos() - GetPos();

			float fDistanceUnit = D3DXVec3Length( &vDistance ) - fOffset;

			if( fDistance >= fDistanceUnit )
			{
				fDistance = fDistanceUnit;
				pFrontNearestUnit = pUnit;
			}
		}

		if ( NULL != pFrontNearestUnit && NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			/// ���� ������ ����Ʈ ����
			D3DXVECTOR3 vMyPos	=  GetBonePos( L"Bip01" );
			vMyPos.y			+= 25.f;

			D3DXVECTOR3	vEnemyPos;

			if ( false == pFrontNearestUnit->GetBonePos( L"Bip01", vEnemyPos ) )
			{
				vEnemyPos	=  pFrontNearestUnit->GetPos();
				vEnemyPos.y	+= 100.f;
			}
			else
			{
				vEnemyPos.y	+= 25.f;
			}

			/// Ÿ�� ��ġ�� ���� ����Ʈ ����
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ALP_Reverse_Reactor", GetPowerRate(),
														  vEnemyPos, GetRotateDegree(), GetRotateDegree() );

			/// ��ũ ����Ʈ ����
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_Reverse_Reactor_Link", GetPowerRate(),
																							  vMyPos, GetRotateDegree(), GetRotateDegree() );

			if ( NULL != pEffect )
			{
				/// ����Ʈ ���� ����
				pEffect->SetLinkMainEffectByPos( vMyPos, vEnemyPos );

				/// ���� ���� ���
				if ( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDeviceManager() )
					g_pKTDXApp->GetDeviceManager()->PlaySound( L"Add_Reverse_Reactor_Connect.ogg" );

				/// ���� ���� ( �����ÿ���, ���� ������ �� �ִ� ���̷��� ���� �����ش�. )
				const float fPVPRate = ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? 0.1f : 1.f;

				/// �ִ� ���� ��������
				const float fMaxTransitionDamage	= GetMaxHp() * fPVPRate;
				/// ���� ������ ����
				const float fTransitionDamageRate	= GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_REL, CX2SkillTree::SI_SA_ALP_REVERSE_REACTOR ) * fPVPRate;
				/// ���� �ð�
				const float fRemainTime				= GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, CX2SkillTree::SI_SA_ALP_REVERSE_REACTOR );

				const bool	bIsNPC					= ( CX2GameUnit::GUT_NPC == pFrontNearestUnit->GetGameUnitType() ) ? true : false;

				/// ���� ����
				m_ReverseReactorTargetInfo = ReverseReactorTargetInfo( fMaxTransitionDamage, 0.f, fRemainTime, fTransitionDamageRate,
																	   pFrontNearestUnit->GetUnitUID(), bIsNPC, pEffect->GetHandle() );

				/// ����� ����
				pFrontNearestUnit->SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_PLASMA_LINK );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_REVERSE_REACTOR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
	GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ���ǹ� �׼�
void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	XSkinMeshReadyInBackground(L"ADD_PSYCHIC_STORM_01.X");
	XMeshReadyInBackground( L"ADD_PHASE_SHIFT_ATTACK01.Y" );

}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_FrameMove()
{
	/// ��Ÿ ����Ʈ ����
	if ( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		/// ��Ÿ ��Ÿ ����
		wstring wstrImpactDamageEffectName = L"DamageEffect_ALP_Quicksilver_Accel_Impact";

		/// ���� ����Ʈ�� ������, ���� ������ ����Ʈ �߻�
		if ( true == GetIsFormationMode() && 0 < GetMutationCount() )
		{
			wstrImpactDamageEffectName.clear();
			wstrImpactDamageEffectName = L"DamageEffect_ALP_Quicksilver_Accel_Impact_Stun";

			UpMutationCount( -1 );
		}

		D3DXVECTOR3 vPos = GetIsRight() ? ( GetBonePos( L"Bip01" ) + ( GetDirVector() * 250.f ) ):
										  ( GetBonePos( L"Bip01" ) - ( GetDirVector() * 250.f ) );
		vPos.y += 30.f;

		g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrImpactDamageEffectName.c_str(), GetPowerRate(),
														  vPos, GetRotateDegree(), GetRotateDegree() );
	}
	/// �����ֱ�� ��ũ ����Ʈ ����
	else if( m_pXSkinAnim->EventTimerOneshot( 0.25f ) )
	{
		CX2DamageEffect::CEffect* pEffect =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_APT_Pylon_Additional_Shock", GetPowerRate(),
			GetBonePos( L"Bip01_L_Finger2" ), GetRotateDegree(), GetRotateDegree() );

		if ( NULL != pEffect )
			m_eQuicksilverAccelLinkEffect = pEffect->GetHandle();
	}
	/// �����ֱ�� ��ũ ����Ʈ ����
	else if( m_pXSkinAnim->EventTimerOneshot( 0.85f ) )
	{
		if ( INVALID_DAMAGE_EFFECT_HANDLE != m_eQuicksilverAccelLinkEffect )
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_eQuicksilverAccelLinkEffect );
	}

	/// �����ֱ�� ��ũ ����Ʈ ����
	if ( INVALID_DAMAGE_EFFECT_HANDLE != m_eQuicksilverAccelLinkEffect &&
		 0.25f <= m_pXSkinAnim->GetNowAnimationTime() &&
		 0.85f > m_pXSkinAnim->GetNowAnimationTime() )
	{
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_eQuicksilverAccelLinkEffect );

		if ( NULL != pEffect )
		{
			D3DXVECTOR3 vEffectPos = GetBonePos( L"Bip01" );	/// ����Ʈ�� ���� ��ġ

			float fRotateRadianY = D3DXToRadian( GetRotateDegree().y ) + 1.57f;	/// ȸ����
			vEffectPos.z += 150.f * cos( fRotateRadianY );
			vEffectPos.x += 150.f * sin( fRotateRadianY );
			vEffectPos.y += 45.f;

			pEffect->SetLinkMainEffectByPos( GetBonePos( L"Bip01_L_Finger2" ), vEffectPos );
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	/// ���� ���� ���� �˻�
	else if( false == m_FrameDataFuture.stateParam.bEventFlagList[0] &&
			 m_fEventTime[1].keyInputStart < m_pXSkinAnimFuture->GetNowAnimationTime() &&
			  m_fEventTime[1].keyInputEnd >= m_pXSkinAnimFuture->GetNowAnimationTime() &&
			 true == SpecialAttackEventProcess() )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
	GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_StateEnd()
{
	if ( INVALID_DAMAGE_EFFECT_HANDLE != m_eQuicksilverAccelLinkEffect &&
		NULL != g_pX2Game &&
		NULL != g_pX2Game->GetDamageEffect() )
	{
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_eQuicksilverAccelLinkEffect );
	}

	CommonStateEnd();
}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_FrameMove ()
{
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		/// ��Ÿ ����Ʈ ����
		if ( m_pXSkinAnim->EventTimerOneshot( 0.596f ) )
		{
			/// ��Ÿ ��Ÿ ����
			wstring wstrImpactDamageEffectName = L"DamageEffect_ALP_Quicksilver_Accel_Impact";

			/// ���� ����Ʈ�� ������, ���� ������ ����Ʈ �߻�
			if ( true == GetIsFormationMode() && 0 < GetMutationCount() )
			{
				wstrImpactDamageEffectName.clear();
				wstrImpactDamageEffectName = L"DamageEffect_ALP_Quicksilver_Accel_Impact_Stun";

				UpMutationCount( -1 );
			}

			D3DXVECTOR3 vPos = GetIsRight() ? ( GetBonePos( L"Bip01" ) + ( GetDirVector() * 100.f ) ):
				( GetBonePos( L"Bip01" ) - ( GetDirVector() * 100.f ) );
			vPos.y += 30.f;

				g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrImpactDamageEffectName.c_str(), GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree() );
		}
		/// �����ֱ�� ��ũ ����Ʈ ����
		else if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_APT_Pylon_Additional_Shock", GetPowerRate(),
				GetBonePos( L"Bip01_L_Finger2" ), GetRotateDegree(), GetRotateDegree() );

			if ( NULL != pEffect )
				m_eQuicksilverAccelLinkEffect = pEffect->GetHandle();
		}
		/// �����ֱ�� ��ũ ����Ʈ ����
		else if( m_pXSkinAnim->EventTimerOneshot( 0.57f ) )
		{
			if ( INVALID_DAMAGE_EFFECT_HANDLE != m_eQuicksilverAccelLinkEffect )
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_eQuicksilverAccelLinkEffect );
		}

		/// �����ֱ�� ��ũ ����Ʈ ����
		if ( INVALID_DAMAGE_EFFECT_HANDLE != m_eQuicksilverAccelLinkEffect &&
			 0.0f <= m_pXSkinAnim->GetNowAnimationTime() &&
			 0.57f > m_pXSkinAnim->GetNowAnimationTime() )
		{
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_eQuicksilverAccelLinkEffect );

			if ( NULL != pEffect )
			{
				D3DXVECTOR3 vEffectPos = GetBonePos( L"Bip01" );	/// ����Ʈ�� ���� ��ġ

				float fRotateRadianY = D3DXToRadian( GetRotateDegree().y ) + 1.57f;	/// ȸ����
				vEffectPos.z += 150.f * cos( fRotateRadianY );
				vEffectPos.x += 150.f * sin( fRotateRadianY );
				vEffectPos.y += 45.f;

				pEffect->SetLinkMainEffectByPos( GetBonePos( L"Bip01_L_Finger2" ), vEffectPos );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_EventProcess ()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
	GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_StateEnd()
{
	if ( INVALID_DAMAGE_EFFECT_HANDLE != m_eQuicksilverAccelLinkEffect &&
		 NULL != g_pX2Game &&
		 NULL != g_pX2Game->GetDamageEffect() )
	{
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_eQuicksilverAccelLinkEffect );
	}

	CommonStateEnd();
}

/// ���ǹ� ������
void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_FRENZY_Init()
{
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y" );
	XMeshReadyInBackground( L"ADD_QUICKSILVER_FRENZY_SPARK01.Y" );

}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_FRENZY_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		UINT uiSkillLevel = static_cast<UINT>( GetSkillLevelValue( CX2SkillTree::SI_SA_ALP_QUICKSILVER_FRENZY ) );
		SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_QUICKSILVER_FRENZY, uiSkillLevel );

		/// ��ų ���� ����� ������ ���� ����
		m_QuicksilverFrenzyData.m_fPowerRate = GetPowerRate();
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_QUICKSILVER_FRENZY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
	GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ����Ʈ �Ƴ����
void CX2GUAdd::ANSI_SA_ALP_DUST_ANATOMY_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA01.tga");
	XSkinMeshReadyInBackground(L"ADD_Dust_Anatomy_Start_DUMMY01.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"EFFECT_Cycle_LensFlare01.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blr01.dds");
	XSkinMeshReadyInBackground(L"ADD_Dust_Anatomy_Spark01.X");
	TextureReadyInBackground(L"Black_Light.dds");
	XMeshReadyInBackground( L"ADD_Effect_Explosion_01.Y" );
	XMeshReadyInBackground( L"ADD_GroundShockWave001.Y" );
	XMeshReadyInBackground( L"ADD_STASIS_FIELD_Spark.Y" );
}

void CX2GUAdd::ANSI_SA_ALP_DUST_ANATOMY_FrameMove()
{
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		/// ���� ����Ʈ ����
		if ( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
		{
			if ( NULL !=  g_pX2Game->GetEffectSet() )
			{
				vector<DustAnatomyTargetInfo>::iterator vIt = m_vecDustAnatomyTargetInfo.begin();

				while( vIt != m_vecDustAnatomyTargetInfo.end() )
				{
					CX2GameUnit* pUnit = vIt->GetTargetUnit();

					/// �׾� ������, �ش� ���� ���� ���� ����
					if ( NULL == pUnit || 0.f >= pUnit->GetNowHp() )
					{
						vIt = m_vecDustAnatomyTargetInfo.erase( vIt );

						continue;
					}

					const D3DXVECTOR3& vMyPos = GetBonePos( vIt->m_wstrCreateBoneName.c_str() );

					CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Dust_Anatomy_Link_Effect",
						GetPowerRate(), vMyPos, GetRotateDegree(), GetRotateDegree() );

					if ( NULL == pEffect )
						continue;

					const D3DXVECTOR3&	vEnemyPos = pUnit->GetBonePos( L"Bip01" );

					pEffect->SetLinkMainEffectByPos( vMyPos, vEnemyPos );

					/// ����Ʈ �ڵ� ����
					vIt->m_eLinkEffect = pEffect->GetHandle();

					/// �����ֱ�� ����Ʈ ����
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_ALP_Dust_Anatomy_Explosion_1nd", pUnit );

					++vIt;
				}
			}
		}
		/// ���� ����Ʈ ����
		else if ( m_pXSkinAnim->EventTimerOneshot( 1.f ) )
		{
			for( DustAnatomyTargetInfo TargetInfo : m_vecDustAnatomyTargetInfo )
			{
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( TargetInfo.m_eLinkEffect );
			}

			m_vecDustAnatomyTargetInfo.resize( 0 );
		}

		/// ���� ����Ʈ ����
		if ( 0.11f < m_pXSkinAnim->GetNowAnimationTime() && 0.9f >= m_pXSkinAnim->GetNowAnimationTime() )
		{
			vector<DustAnatomyTargetInfo>::iterator vIt = m_vecDustAnatomyTargetInfo.begin();

			while( vIt != m_vecDustAnatomyTargetInfo.end() )
			{
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( vIt->m_eLinkEffect );

				if ( NULL != pEffect )
				{
					CX2GameUnit* pUnit = vIt->GetTargetUnit();

					/// �׾� ������, �ش� ���� ���� ���� ����
					if ( NULL == pUnit || 0.f >= pUnit->GetNowHp() )
					{
						vIt = m_vecDustAnatomyTargetInfo.erase( vIt );

						continue;
					}

					const D3DXVECTOR3&	vMyPos		= GetBonePos( vIt->m_wstrCreateBoneName.c_str() );
					const D3DXVECTOR3&	vEnemyPos	= pUnit->GetBonePos( L"Bip01" );

					pEffect->SetLinkMainEffectByPos( vMyPos, vEnemyPos );
				}

				++vIt;
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_DUST_ANATOMY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_ALP_DUST_ANATOMY_StateEnd()
{
	m_vecDustAnatomyTargetInfo.resize( 0 );

	CommonStateEnd();
}

/// ����ű ����
void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_Init()
{
	TextureReadyInBackground(L"ADD_CENTER_LIGHT01.dds");
	XSkinMeshReadyInBackground(L"ADD_PSYCHIC_STORM_DUMMY01.X");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"ADD_PSYCHIC_STORM_DUMMY02.X");
	XSkinMeshReadyInBackground( L"ADD_PSYCHIC_STORM_01.X" );
	XMeshReadyInBackground(L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y");
	XMeshReadyInBackground(L"ADD_ShockWave003.Y");
	XMeshReadyInBackground(L"ADD_PARTICLE_ACCELERATO_CYCLE1.Y");
	XMeshReadyInBackground(L"ADD_PSYCHIC_STORM_03.Y");
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_StateStartFuture()
{
	m_PsychicStormData.ResetPsychicStormData();

	m_PsychicStormData.m_fLoopTime = _CONST_ADD_::PSYCHIC_STORM_DEFAULT_LOOP_TIME;	/// �ݺ� �ð� �ʱ�ȭ

	CommonStateStartFuture();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_StateStart()
{
	/// �ʱ�ȭ �Լ��� Future �� Now �ι� �ҷ��� ����.
	/// ��¥�� Future���� ������ LoopTime�� Now �� ���� ���� Loop�� ������ ���� ���� ���� ó�� �̱� �����̴�.
	m_PsychicStormData.ResetPsychicStormData();

	float& fLoopTime		= m_PsychicStormData.m_fLoopTime;					/// �ݺ� �ð�
	fLoopTime				= _CONST_ADD_::PSYCHIC_STORM_DEFAULT_LOOP_TIME;


	if ( true == GetIsFormationMode() )
	{
		/// ������ �ݺ� �ð� �߰�
		fLoopTime += static_cast<float>( GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME_HYPER,
			CX2SkillTree::SI_SA_ALP_PSYCHIC_STORM ) );

		/// ���� ����Ʈ�� �ִٸ�, �ݺ� �ð�, ����Ʈ ũ�� �߰�
		if ( 0 < GetMutationCount() )
		{
			/// ���� ����Ʈ �Ҹ� ���
			const int iUseMutationCount = min( GetMutationCount(), _CONST_ADD_::PSYCHIC_STORM_MAX_USE_MUTATION_COUNT );

			/// �߰� �ݺ� �ð� ����
			fLoopTime += static_cast<float>( iUseMutationCount *
				GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, CX2SkillTree::SI_SA_ALP_PSYCHIC_STORM ) );

			/// �߰� ũ�� ����
			m_PsychicStormData.m_fIncreaseSize *= static_cast<float>( iUseMutationCount *
				GetSpecialAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, CX2SkillTree::SI_SA_ALP_PSYCHIC_STORM ) );

			UpMutationCount( -iUseMutationCount );	/// ���� ����Ʈ �Ҹ�
		}
	}

	CommonStateStart();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_SA_ALP_PSYCHIC_STORM_LOOP );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_LOOP_StateStart()
{
	/// �����ֱ�� ����Ʈ ����
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
	{
		/// ����Ʈ ���� �� �ڵ� ����
		CX2EffectSet::Handle hPsychicStorm		= g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_ALP_Psychic_Storm_Loop", this );
		m_PsychicStormData.eLoopEffectHandle	= hPsychicStorm;

		/// ����Ʈ ũ�� ����
		const float fIncreaseSize = m_PsychicStormData.m_fIncreaseSize;

		if ( 1.f < fIncreaseSize )
		{
			CX2EffectSet::EffectSetInstance * phPsychicStorm = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hPsychicStorm );

			if ( NULL !=  phPsychicStorm )
			{
				const float fIncreaseSize = m_PsychicStormData.m_fIncreaseSize;
				phPsychicStorm->SetEffectScale( D3DXVECTOR3( fIncreaseSize, fIncreaseSize, fIncreaseSize ), true );
			}
		}
	}

	CommonStateStart();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_LOOP_FrameMove()
{
	float& fFireGap = m_PsychicStormData.m_fFireGap;	/// ����Ʈ ���� ����


	if( fFireGap <= m_pXSkinAnim->GetNowAnimationTime() )
	{
		fFireGap += _CONST_ADD_::PSYCHIC_STORM_ATTACK_INTERVAL_TIME;	/// ���� ���� ���� ����

		/// ũ�Ⱚ�� �����Ͽ� ���� ����Ʈ ����
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			D3DXVECTOR3 vPos = GetBonePos( L"Bip01" );
			vPos.y += 150.f;

			CX2DamageEffect::CEffect* pCEffect
				= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ALP_Psychic_Storm_Loop", GetPowerRate(), vPos,
																GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			/// ����Ʈ ũ�� ����
			const float fIncreaseSize = m_PsychicStormData.m_fIncreaseSize;

			if ( 1.f < fIncreaseSize && NULL != pCEffect && NULL != pCEffect->GetMainEffect() )
			{
				const float fIncreaseSize = m_PsychicStormData.m_fIncreaseSize;
				pCEffect->SetScale( pCEffect->GetMainEffect()->GetScale() * fIncreaseSize ) ;
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_LOOP_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true ||
		m_PsychicStormData.m_fLoopTime <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		StateChange( ANSI_SA_ALP_PSYCHIC_STORM_FINISH );
	}

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_FINISH_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		m_PsychicStormData.ResetPsychicStormData();
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_SA_ALP_PSYCHIC_STORM_FINISH_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}


//#pragma endregion //���м� ��Ƽ��


//#pragma region ��Ƽ��

/// ����ź
void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_Init()
{
	TextureReadyInBackground(L"ADD_Effect_CIRCLE01.tga");
	TextureReadyInBackground(L"Add_Gravity_Ball.tga");
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"ADD_Effect_Triangle01.Tga");
}
void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ����ź - ��������
void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_StateStartFuture()
{
	CommonStateStartFuture();
	/// Ű �Է� ���� ��ü �ʱ�ȭ
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );
}

void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE );
	}

	CommonEventProcess();
}

void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_StateEndFuture()
{
	CommonStateEndFuture();

	/// ������Ʈ ���� ��� ���带 ������� �ֱ� ���� �ڵ忡�� ó��
	PlayLoopSound(L"ADD_PULSE_BULLET_CHARGE_01.ogg", false, false );
}


void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_StateStart()
{
	CommonStateStart();
	m_fChargeTime = 0.f;
}
void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_FrameMove()
{
	CommonFrameMove();
	m_fChargeTime += m_fElapsedTime;
}
void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_EventProcess()
{
	/// Ű �Է� ���� ��ü���� ������ ���� �Ǿ� �ִ��� �˻�
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	if ( _CONST_ADD_::PULSE_BULLET_MAX_CHARGE_TIME <= m_fChargeTime ||
		true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) )
	{
		StateChange( ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE );
	}

	CommonEventProcess();
}


void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_StateStartFuture()
{
	CommonStateStartFuture();
	StopLoopSound();
}

/** @function : ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_FrameMove
	@brief : �������� �нú��� ������ �޾�, ����ź�� ����Ʈ�� �Ӽ��� ������ ��ŭ ���� ��Ŵ
			 ����� ���ӽð�, ũ��, ������,  ���� ���� �� Ŀ�� ���ǵ� �� ���� ó�� ��
*/
void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vPos;
		GetBonePos( L"EF01", vPos );
		vPos.x += ( true == GetIsRight() ) ? 70.f : -70.f;

		// ���� ���� 0~1��
		const float fIncreaseChargeRate = (min( _CONST_ADD_::PULSE_BULLET_MAX_APPLY_CHARGE_TIME, m_fChargeTime )) /_CONST_ADD_::PULSE_BULLET_MAX_APPLY_CHARGE_TIME ;

		// ���� ������ �ִ� ���� : 2, ������ ���� ���� �� : 0. 5 �϶�
		// -> (2-1)*0.5 +1 = 1.5
		float fPowerRate = GetPowerRate();
		{
			const float fDamageRate = (m_ChargedImpulsarData.m_fIncreaseDamageRate-1) * fIncreaseChargeRate + 1;
			if( 0 < fDamageRate )
				fPowerRate *= fDamageRate;
		}

		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AN_Pulse_Bullet_Ball", fPowerRate,
																				vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( NULL != pDamageEffect )
		{
			SetLockOn( pDamageEffect, 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
			pDamageEffect->SetLockOnType( CX2DamageEffect::LOT_NEARST_UID_VECTOR );

			m_fChargeTime = 0.f;

			// �Ͽ� ���� ���� ����
			const float fIncreaseCurveSpeedRate = (m_ChargedImpulsarData.m_fIncreaseCurveSpeedMaxRate-1) * fIncreaseChargeRate + 1;
			const float fCurveSpeed = _CONST_ADD_::PULSE_BULLET_BASE_CURVE_SPEED * fIncreaseCurveSpeedRate;
			pDamageEffect->GetLockOnData().m_fCurveSpeed			= fCurveSpeed;
			pDamageEffect->GetLockOnData().m_fTimeUnlockFromStart	= 0.2f;
			pDamageEffect->GetLockOnData().m_bIsOnlyFront			= true;

			/// ���� �ð��� ���� �� ��ġ ����
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = pDamageEffect->GetMainEffect();
			if( NULL != pMesh )
			{
				// ���� ������ �ִ� ���� : 2, ������ ���� ���� �� : 0. 5 �϶�
				// -> (2-1)*0.5 +1 = 1.5

				// ������Ÿ��
				const float fIncreaseLifeRate = (m_ChargedImpulsarData.m_fIncreaseLifeTimeMaxRate-1) * fIncreaseChargeRate + 1;
				const float fLifeTime = pMesh->GetMaxLifeTime() * fIncreaseLifeRate;
				pMesh->SetMaxLifeTime( fLifeTime );

				// ������
				const float fScaleRate = (m_ChargedImpulsarData.m_fIncreaseScaleMaxRate-1) * fIncreaseChargeRate + 1;
				pMesh->SetScale( fScaleRate, fScaleRate, fScaleRate );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ���� �ر�
void CX2GUAdd::ANSI_A_AN_MIND_BREAK_Init()
{
	XSkinMeshReadyInBackground(L"ADD_MIND_BREAK_Dynamo2.X");
	XSkinMeshReadyInBackground( L"ADD_MIND_BREAK_Dynamo.X" );
	XMeshReadyInBackground( L"ADD_ShockWave003.Y" );
	TextureReadyInBackground( L"ADD_Effect_Triangle01.Tga" );
	XSkinMeshReadyInBackground( L"ADD_NEUTRON_BOMB_B.X" );
}

void CX2GUAdd::ANSI_A_AN_MIND_BREAK_FrmaeMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.271f ) )
	{
		/// Ÿ���� ��� ã�� ( ���̻� ������ �巹�� ���� )
		float			fDistance				= 700.f;
		CX2GameUnit*	pFrontNearestUnit		= NULL;

//		// ���߿� �����Ÿ� SA ������ �޾ƿ;� �� �ʿ䰡 ������ �ּ� �������ּ���!
// 		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_ENERGY_DRAIN );
// 		if( NULL != pSkillTemplet )
// 		{
// 			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
// 			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
// 			fDistance = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );
// 		}
		for( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
		{
			CX2GameUnit *pUnit = g_pX2Game->GetUnit( i );

			if ( NULL == pUnit )
				continue;
			if( g_pX2Game->IsValidUnit( pUnit ) == false )
				continue;
			if( GetTeam() == pUnit->GetTeam() )
				continue;
			if( pUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE || pUnit->GetNowHp() <= 0.f )
				continue;

			if( g_pX2Game->IsFront( (CX2GameUnit*)this, pUnit ) == false )
				continue;

			float fOffset = 0.f;
			if(pUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC)
			{
				CX2GUNPC *pNpc = static_cast<CX2GUNPC*>( pUnit );
				if( NULL == pNpc || CX2UnitManager::NCT_BASIC != pNpc->GetNPCTemplet().m_ClassType )
					continue;

				fOffset = ( pNpc->GetOrgUnitSize().x + pNpc->GetOrgUnitSize().y ) / 4.f;
			}

			D3DXVECTOR3 vDistance = pUnit->GetPos() - GetPos();

			float fDistanceUnit = D3DXVec3Length( &vDistance ) - fOffset;

			if( fDistance >= fDistanceUnit )
			{
				fDistance = fDistanceUnit;
				pFrontNearestUnit = pUnit;
			}
		}

		if ( NULL != pFrontNearestUnit )
		{
			/// ��ų ���� ��ȯ
			const CX2UserSkillTree& userSkillTree	= GetUnit()->GetUnitData().m_UserSkillTree;
			const int				iSkillLevel		= userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AN_MIND_BREAK );

			/// ����� �Ӹ��� ������ ����� �ھ� ����
			pFrontNearestUnit->SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_MIND_BREAK, iSkillLevel );

			/// ����� �Ӹ��� ������ ������ ����Ʈ �ھ� ����
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() && g_pX2Game->GetEffectSet() )
			{
				D3DXVECTOR3 vHeadPos = pFrontNearestUnit->GetHeadBonePos();

				/// Ÿ�ݿ� ������ ����Ʈ
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SI_A_AN_MIND_BREAK", GetPowerRate(),
					vHeadPos, GetRotateDegree(),GetRotateDegree() );

				/// ���÷��̿� ����Ʈ��
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_A_AN_MIND_BREAK", pFrontNearestUnit );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_A_AN_MIND_BREAK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ���� ��ȭ
void CX2GUAdd::ANSI_A_AN_PHASE_SHIFT_Init()
{
	XMeshReadyInBackground(L"ADD_Effect_CIRCLE_03_1.Y");
	XMeshReadyInBackground(L"ADD_PHASE_SHIFT_BLINK06.Y");
	XMeshReadyInBackground(L"ADD_MAGNETRON_DYNAMO_Spark.Y");
	XMeshReadyInBackground(L"ADD_MAGNETRON_DYNAMO_Spark2.Y");
	XSkinMeshReadyInBackground(L"ADD_PHASE_SHIFT_SPARK01.X");
	XSkinMeshReadyInBackground(L"ADD_PHASE_SHIFT_LOOP01.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"add_EFFECT_Cycle_Blur01.dds");
	TextureReadyInBackground(L"ADD_ShockWave.dds");
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
}
void CX2GUAdd::ANSI_A_AN_PHASE_SHIFT_FrameMove()
{
	CommonFrameMove();

	if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		/// �����Ǵ� ������ ����Ʈ ����
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			// ���� ����Ʈ�� �ִٸ� ������ �Ͷ߸���
			if( INVALID_DAMAGE_EFFECT_HANDLE != m_PhaseShiftData.m_hPhasShiftDamageEffect )
			{
				CX2DamageEffect::CEffect* pEffect =
					g_pX2Game->GetDamageEffect()->GetInstance( m_PhaseShiftData.m_hPhasShiftDamageEffect );
				if( NULL != pEffect )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pMesh = pEffect->GetMainEffect();
					if( NULL != pMesh )
						pMesh->SetMaxLifeTime(0.1f);

					pEffect->GetDamageData().m_eUsedSkillID = CX2SkillTree::SI_NONE;
					m_PhaseShiftData.InitPhaseEffectData();
				}
			}

			// ���� ����
			{
				CX2DamageEffect::CEffect* pEffect =
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Add_Phase_Shift",
					GetPowerRate(), GetPos(), GetRotateDegree(),GetRotateDegree() );

				if( NULL != pEffect )
				{
					// ���ư� ��ġ ����
					m_PhaseShiftData.m_vUserReturnPos = GetPos();

					// ������ ����Ʈ �ڵ� ����
					m_PhaseShiftData.m_hPhasShiftDamageEffect = pEffect->GetHandle();

					// ���� UI ����
					if ( true == IsMyUnit() )
						m_PhaseShiftData.UpdateSkillSlotUI();

					// ����Ʈ ���ӽð� ����
					CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = pEffect->GetMainEffect();
					pMeshInstance->SetMaxLifeTime( m_PhaseShiftData.m_fEffectLifeTime );
				}
			}
		}
	}
}

void CX2GUAdd::ANSI_A_AN_PHASE_SHIFT_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
/** @function : ANSI_A_AN_PHASE_SHIFT_BLINK
	@brief : ��ġ�ص� ����ȭ ����Ʈ�� �������� ��ġ �� ����
*/
void CX2GUAdd::ANSI_A_AN_PHASE_SHIFT_BLINK_StateStart()
{
	CommonStateStart();

	if( NULL != g_pX2Game &&
		NULL != g_pX2Game->GetDamageEffect() )
	{
		CX2DamageEffect::CEffect* pEffect =
			g_pX2Game->GetDamageEffect()->GetInstance( m_PhaseShiftData.m_hPhasShiftDamageEffect );

		if( NULL != pEffect )
		{
			pEffect->SetPos( GetPos() );
		}
	}

	// # ���� ������ ���� Futrue���� ���� �ʰ� IsMyUnit���� �˻� ��
	//   ( ���� ���� : ����Ʈ�� ���� �Űܰ� ����, ���� ��ġ�� �Űܾ� ��)
	if( true == IsMyUnit() &&
	    true == m_PhaseShiftData.IsValidReturnPosValue() )
	{
// 		// ���� ����
//		(��ȹ������ ���� �������� �ʵ��� ���������� �ڵ� ���� �� ��..)
// 		const D3DXVECTOR3 vDistancePos = m_PhaseShiftData.m_vUserReturnPos -m_FrameDataFuture.syncData.position;
// 		if( (GetDirVector().x * vDistancePos.x) < 0.f ||
// 			(GetDirVector().z * vDistancePos.z) < 0.f )
// 		{
// 			m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
// 		}

		// ��ġ ����
		m_FrameDataFuture.syncData.position = m_PhaseShiftData.m_vUserReturnPos;
		m_FrameDataNow.syncData.position = m_PhaseShiftData.m_vUserReturnPos;
		m_PhaseShiftData.InitResturnPosValue();

		if ( IsMyUnit() )
			m_PhaseShiftData.UpdateSkillSlotUI();
	}
}
void CX2GUAdd::ANSI_A_AN_PHASE_SHIFT_BLINK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ���̳ʸ� ���� - ���׳�Ʈ��
void CX2GUAdd::ANSI_A_APT_MAGNETRON_DYNAMO_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_100x50x50_center.X" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_Spark.Y" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_Spark2.Y" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_CYCLE1.Y" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_CYCLE2.Y" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_MESH1.Y" );
}

void CX2GUAdd::ANSI_A_APT_MAGNETRON_DYNAMO_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		/// ���׳�Ʈ�� ���� �ʱ�ȭ
		m_MagnetronData.ResetMagnetronData();

		for( int i = 0; i < _CONST_ADD_::MAGNETRON_MAX_NUM; ++i )
		{
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
			{
				D3DXVECTOR3 vPos = GetBonePos( L"Bip01" );
				D3DXVECTOR3 vRot = GetRotateDegree();

				vRot.y	+= static_cast<float>( ( 360.f / _CONST_ADD_::MAGNETRON_MAX_NUM ) * i );

				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"DamageEffect_SA_APT_Magnetron_Dynamo_Start",
																				  GetPowerRate(), vPos, vRot, vRot, m_FrameDataNow.unitCondition.landPosition.y );

				if ( NULL != pEffect )
					m_MagnetronData.m_vecMagnetronDamageEffectData.push_back( pEffect->GetHandle() );
			}
		}

#ifdef X2_ADD_2ND_PATH

		if ( ThirdPath_DynamoLockValidOrClear() )
			ThirdPath_DynamoLockReAimOrbits();
		//}} 2026-09-04 X2GUAdd reverify SA1-13 //
#endif
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_A_APT_MAGNETRON_DYNAMO_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ���̳ʸ� ���� - ������
void CX2GUAdd::ANSI_A_APT_DESOLVER_DYNAMO_Init()
{
	XSkinMeshReadyInBackground(L"ADD_DESOLVER_DYNAMO_Start_DUMMY01.X");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XMeshReadyInBackground( L"ADD_GroundShockWave001.Y" );
	XMeshReadyInBackground( L"ADD_Effect_Explosion_02.Y" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_CYCLE2.Y" );
	XMeshReadyInBackground( L"ADD_ShockWave003.Y" );
	TextureReadyInBackground( L"Black_Light.dds" );
}

void CX2GUAdd::ANSI_A_APT_DESOLVER_DYNAMO_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}

/// ���̳ʸ� ���� - ���Ϸ�
void CX2GUAdd::ANSI_A_APT_PYLON_DYNAMO_Init()
{
	XSkinMeshReadyInBackground(L"DummyAttackBox_100x50x50_center.X");
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_Spark.Y" );
	XMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_Spark2.Y" );
	TextureReadyInBackground( L"ADD_CYCLE_COA01.Tga" );
	TextureReadyInBackground( L"Black_Light.dds" );
	TextureReadyInBackground( L"add_EFFECT_Cycle_Blur01.dds" );
}

void CX2GUAdd::ANSI_A_APT_PYLON_DYNAMO_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		D3DXVECTOR3 vSummonPos = GetPos();

		vSummonPos.y += 110.f;

		/// ���Ϸ� ������ ����Ʈ
		CX2DamageEffect::CEffect* pEffect		= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_APT_Pylon_Dynamo", GetPowerRate(),
			vSummonPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		/// ������ ������ ����Ʈ
		CX2DamageEffect::CEffect* pBuffEffect	= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Pylon_Buff", GetPowerRate(),
			vSummonPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		if ( NULL != pEffect && NULL != pBuffEffect )
		{
			AddPylonLinkDamageEffect( pEffect );

			/// ��ȯ�� ���Ϸ��� �ϳ��� ������, ���� ��ȯ ��Ÿ�� ����
			if ( 0 == m_uiNonstopSummonPylonCount )
				m_fNonstopSummonPylonCoolTime = _CONST_ADD_::PYLON_NONESTOP_SUMMON_COOL_TIME;

			/// ���� ��ȯ �˻縦 ���� ���Ϸ� ��ü�� ����
			++m_uiNonstopSummonPylonCount;

			/// ��ȯ�� ���Ϸ� UID ����
			m_vecSummonPylonDamageEffectData.push_back( PylonDamageEffectData( pEffect->GetHandle(), pBuffEffect->GetHandle(),
				_CONST_ADD_::PYLON_REMAIN_TIME ) );

			/// ��ȯ�� ���Ϸ��� ���� ��ȯ ���Ѽ��� �Ѿ��ٸ�, ���� ��ȯ ��Ÿ�� �� ī��Ʈ ����
			if ( _CONST_ADD_::PYLON_NONESTOP_SUMMON_LIMIT_NUM <=  m_uiNonstopSummonPylonCount )
			{
				m_fNonstopSummonPylonCoolTime	= 0.f;
				m_uiNonstopSummonPylonCount		=	0;

			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_A_APT_PYLON_DYNAMO_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() && true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}


/// ���̿��� ��Ʈ
void CX2GUAdd::ANSI_A_ALP_PSIONIC_BEAT_Init()
{
	TextureReadyInBackground(L"ADD_CYCLE_COA01.tga");
	TextureReadyInBackground(L"ADD_Effect_Triangle01.Tga");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
}

void CX2GUAdd::ANSI_A_ALP_PSIONIC_BEAT_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}

/// Ʈ�� ����
void CX2GUAdd::ANSI_A_ALP_TWIRL_RUSH_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GlowBall_G01.tga");
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y" );
	XMeshReadyInBackground( L"ADD_ShockWave003.Y" );
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATO_CYCLE1.Y" );
	XMeshReadyInBackground( L"ADD_PSYCHIC_STORM_03.Y" );
	XSkinMeshReadyInBackground(L"ADD_PSYCHIC_STORM_01.X");
}

void CX2GUAdd::ANSI_A_ALP_TWIRL_RUSH_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}


//#pragma endregion //��Ƽ��

//#pragma region ������ ����� ��Ƽ��


/// �ҽ� ����
void CX2GUAdd::ANSI_HA_ALP_DOOMS_DAY_Init()
{

}

void CX2GUAdd::ANSI_HA_ALP_DOOMS_DAY_StateStart()
{
	m_vecDoomsDayLinkEffect.resize( 0 );	/// ��ũ ����Ʈ �ʱ�ȭ

	CommonStateStart();
}

void CX2GUAdd::ANSI_HA_ALP_DOOMS_DAY_FrameMove()
{
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		/// ����Ʈ ����
		if( m_pXSkinAnim->EventTimerOneshot( 0.33f ) )
		{
			/// ���� �� ����Ʈ
			CX2DamageEffect::CEffect* pCEffectDummy	= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ALP_Dooms_Day_Dummy_effect", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			/// ��ũ ����Ʈ 1
			CX2DamageEffect::CEffect* pCEffectLink1 = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ALP_Dooms_Day_Link_effect", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			/// ��ũ ����Ʈ 2
			CX2DamageEffect::CEffect* pCEffectLink2 = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ALP_Dooms_Day_Link_effect", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			if ( NULL != pCEffectDummy && NULL != pCEffectLink1 && NULL != pCEffectLink2 )
			{
				m_vecDoomsDayLinkEffect.push_back( pCEffectDummy->GetHandle() );
				m_vecDoomsDayLinkEffect.push_back( pCEffectLink1->GetHandle() );
				m_vecDoomsDayLinkEffect.push_back( pCEffectLink2->GetHandle() );
			}
		}
		/// ����Ʈ ����
		else if( m_pXSkinAnim->EventTimerOneshot( 4.294f ) )
		{
			for( CX2DamageEffect::CEffectHandle hHandle : m_vecDoomsDayLinkEffect )
			{
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( hHandle );
			}

			m_vecDoomsDayLinkEffect.resize( 0 );
		}

		/// ����Ʈ ��ũ ��ġ ����
		if ( false == m_vecDoomsDayLinkEffect.empty() )
		{
			vector<CX2DamageEffect::CEffectHandle>::iterator vIt = m_vecDoomsDayLinkEffect.begin();

			CX2DamageEffect::CEffect* pCEffectDummy = g_pX2Game->GetDamageEffect()->GetInstance( *vIt );
			CX2DamageEffect::CEffect* pCEffectLink1 = g_pX2Game->GetDamageEffect()->GetInstance( *( vIt + 1 ) );
			CX2DamageEffect::CEffect* pCEffectLink2 = g_pX2Game->GetDamageEffect()->GetInstance( *( vIt + 2 ) );

			if ( NULL != pCEffectDummy && NULL != pCEffectDummy->GetMainEffect() &&
				 NULL != pCEffectLink1 && NULL != pCEffectLink2 )
			{
				CKTDGXMeshPlayer::CXMeshInstance* pDummyMesh = pCEffectDummy->GetMainEffect();

				if ( NULL != pDummyMesh )
				{
					const D3DXVECTOR3 vLinkPos1_1 = pDummyMesh->GetBonePos( L"Dummy02" );
					const D3DXVECTOR3 vLinkPos1_2 = pDummyMesh->GetBonePos( L"Dummy04" );
					pCEffectLink1->SetLinkMainEffectByPos( vLinkPos1_1, vLinkPos1_2 );

					const D3DXVECTOR3 vLinkPos2_1 = pDummyMesh->GetBonePos( L"Dummy03" );
					const D3DXVECTOR3 vLinkPos2_2 = pDummyMesh->GetBonePos( L"Dummy05" );
					pCEffectLink2->SetLinkMainEffectByPos( vLinkPos2_1, vLinkPos2_2 );
				}
			}
		}
	}

	CommonFrameMove();
}

void CX2GUAdd::ANSI_HA_ALP_DOOMS_DAY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
}

void CX2GUAdd::ANSI_HA_ALP_DOOMS_DAY_StateEnd()
{
	/// ���� ����Ʈ �����ִٸ�, ��������
	if ( false == m_vecDoomsDayLinkEffect.empty() )
	{
		for( CX2DamageEffect::CEffectHandle hHandle : m_vecDoomsDayLinkEffect )
		{
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( hHandle );
		}
	}

	CommonStateEnd();
}


//#pragma endregion //������ ����� ��Ƽ��

/*virtual*/void CX2GUAdd::SetDamageExceptionProcess( OUT CX2DamageManager::DamageData* pDamageData_ ,OUT float& fPowerRate)
{
	CX2GUUser::SetDamageExceptionProcess(pDamageData_, fPowerRate);

	/// �ʿ����� ��ü ���� ����
	ProcessBodyOfTranscendence( pDamageData_ );
}

void CX2GUAdd::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.f, 0.1f, true );
}
void CX2GUAdd::CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ /*= false*/ )
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );

	if( m_pXSkinAnim->EventTimerOneshot( fTime1_ ) )
	{
		ShowMinorParticleHyperModeChange( L"Bip01_Spine1" );
	}

	if( m_pXSkinAnim->EventTimerOneshot( fTime2_ ) )
	{

		if ( NULL !=g_pX2Game->GetMinorParticle()  )
		{
			D3DXVECTOR3 pos = GetBonePos( L"Bip01_Spine1" );

			CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave =
				g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
			PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  3 );

			CKTDGParticleSystem::CParticleEventSequence* pSeqWhiteShockWave =
				g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_WHITE_SHOCK_WAVE ) );
			PlaySequenceByTriggerCount( pSeqWhiteShockWave, pos, 10, 10,  1 );

			CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTick =
				g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_IMPACT_TICK ) );
			PlaySequenceByTriggerCount( pSeqImpactTick, pos,  200, 200,  30 );
		}

		/// ȿ���� ����Ʈ ���
		SetEffectSoundParticle( GetBonePos( L"Bip01" ), L"ADD_STR_ToKang" );

		UpDownCrashCamera( 30.0f, 0.4f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

		ShowMinorParticleHyperModeTrace();
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


/** @function	: GetPossibleChangeFormationMode
	@brief		: ���� ��� ��� ���� ���� ��ȯ
	@param		: �ȳ� �޽��� ǥ�� ����
	@return		: ��� ���� ����
*/
bool CX2GUAdd::GetPossibleChangeFormationMode( IN const bool bOutputInformation /*= true*/ )
{
	if ( NULL == g_pX2Game  )
		return false;

	/// ���� ��� ���̶��, �н�
	if ( true == GetIsFormationMode() )
		return false;

	/// DP �˻�
	if ( m_fUseDPByEnableFormationMode > GetDPValue() )
	{
		/// DP �����ϸ�, �ȳ� �ؽ�Ʈ ǥ��
		if ( true == bOutputInformation  )
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_29420 ), D3DXCOLOR(1,1,1,1),
			D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );

		return false;
	}

	/// ��Ÿ�� �˻�
	if ( GetChangeFormationCoolTime() > 0.f )
	{
		/// ��Ÿ�� �����ϸ�, �ȳ� �ؽ�Ʈ ǥ��
		if ( true == bOutputInformation  )
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_29939 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );

		return false;
	}

	return true;
}

/** @function	: GetChangeFormationCoolTime
	@brief		: ���� ��� ��ȯ ��Ÿ�� ��ȯ
*/
float CX2GUAdd::GetChangeFormationCoolTime()
{
	return ( NULL != GetAddGageData() ) ? GetAddGageData()->GetChangeFormationCoolTime() : 0.f;
}

/** @function	: SetChangeFormationCoolTime
	@brief		: ���� ��� ��ȯ ��Ÿ�� ����
	@param		: ������ ��Ÿ��
*/
void CX2GUAdd::SetChangeFormationCoolTime( IN const float fCoolTime_ )
{
	if ( NULL != GetAddGageData() )
	{
		GetAddGageData()->SetChangeFormationCoolTime( fCoolTime_ );
	}
}

/** @function	: GetAddGageData
	@brief		: �ֵ� ���� ������ ������ ��ȯ
	@return		: ������ ������
*/
CX2AddGageData* CX2GUAdd::GetAddGageData() const
{
	return static_cast<CX2AddGageData*>( m_pGageData );
}

/** @function	: SpecialAttackCustomStateChange
	@brief		: Ư�� ��ų�� ���� ������Ʈ ���� ó��
	@param		: ��ų ���̵�
	@return		: ���� ���� ����
*/
bool CX2GUAdd::SpecialAttackCustomStateChange( IN const CX2SkillTree::SKILL_ID eSkillID_ )
{
	switch ( eSkillID_ )
	{
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	case CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_SHOOTING_CHASER:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY:
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL:
		{
			if ( 0 == m_iFactoryMarkedSkillID )
			{
				ThirdPath_FactorySkillValueLoader();	// official loads the factory params from passives 9411/9412/9413 + 9015 at the mode-enter moment (official hook = the engine recompute virtual slot_260; ours = this point ��� the only moment they are consumed)
			m_iFactoryMarkedSkillID = (int)eSkillID_;	// official sets the marker inside the 187 spawn path; net effect identical ��� batch6 note
			StateChange( ANSI_AAT_CREATE_DYNAMO_FACTORY );
				return true;
			}
			if ( m_iFactoryMarkedSkillID == (int)eSkillID_ )
				break;	// proceed to the normal cast (re-create) ��� official result=0

			{
				ThirdPath_FactoryModeClear();			// clear-old (marker + phase=2 + the old drone + UI refresh)
				ThirdPath_FactorySkillValueLoader();	// reload the factory params
				m_iFactoryMarkedSkillID = (int)eSkillID_;

				int iSkillLevel = 1;
				if ( NULL != GetUnit() )
					iSkillLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID_ ) );
				SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_DYNAMO_FACTORY, iSkillLevel );	// re-arm the mode marker
				if ( 0 < m_iNewUpdateLevel )
					SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_NEW_UPDATE, (UINT)m_iNewUpdateLevel );	// the 10s MP-charge buff refresh per generation

				ThirdPath_FactorySpawnDrone();	// the new per-skill drone DE
				m_iFactoryPhase			= 1;	// generated/active (official f_325197C0 sets phase=1)
				m_bFactoryModeEntered	= true;

				if ( NULL != GetAddGageData() )
					GetAddGageData()->AddDPPoint( 1 );
				//}} 2026-09-05 //

				if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
				{
					const WCHAR* pszChangeES = L"EffectSet_SI_SA_AAT_Installation_Drone_Change_2";	// 9019/9020 family
					if ( CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR == eSkillID_ )
						pszChangeES = L"EffectSet_SI_SA_AAT_Installation_Drone_Change_1";			// 9015 family
					else if ( CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY == eSkillID_ || CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL == eSkillID_ )
						pszChangeES = L"EffectSet_SI_SA_AAT_Installation_Drone_Change_3";			// 9021/9022 family
					g_pX2Game->GetEffectSet()->PlayEffectSet( pszChangeES, this, NULL, false, GetPowerRate() );
				}

				if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
					g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillSlotOverlay();
				//}} 2026-09-06 //
				return true;	// swallowed - no state change (the official router returns 1 here)
			}
			//}} 2026-09-04 X2GUAdd reverify FACT-4 //
		}
	//}} 2026-08-30 MARCH2015 batch6 //
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:
#else
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:		/// ���� ��ȭ ��ġ ���� ������Ʈ
#endif
		{
			if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PHASE_SHIFT ) && true == GetIsFormationMode() )
			{
				SetUseFantasyTracerTarget( CX2SkillTree::SI_A_AN_PHASE_SHIFT );	/// ��� ó��

				StateChange( ANSI_COMBO_NASOD_ARMOR_READY );

				return true;
			}
			else if ( true == m_PhaseShiftData.IsValidReturnPosValue() )
			{
				StateChange( ANSI_A_AN_PHASE_SHIFT_BLINK );
				return true;
			}
		} break;
	case CX2SkillTree::SI_A_AN_PULSE_BULLET:	/// ����ź - ��������� ������Ʈ
		{
			if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PULSE_BULLET ) && true == GetIsFormationMode() )
			{
				SetUseFantasyTracerTarget( CX2SkillTree::SI_A_AN_PULSE_BULLET );	/// ��� ó��

				StateChange( ANSI_COMBO_NASOD_ARMOR_READY );

				return true;
			}
			// ����ź - �������� �нú� ���� �� ������ ������Ʈ�� ����
			else if ( true == m_ChargedImpulsarData.m_bEnableChargedImpulsar )
			{
				StateChange( ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY );
				return true;
			}

		} break;
	case CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL:
		{
			/// ���ǹ� �׼� ���� ��ȯ
			if ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == GetFutureStateSkillID() )
			{
				StateChange( ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK );
				return true;
			}
		} break;
	default:
		break;

#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
	case CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA:
		{
#endif

#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
			if ( 0 != m_uidStigmaTarget )
			{
				StateChange( ANSI_SA_ADE_REVERSE_STIGMA_RETURN );
				return true;
			}
		} break;
#endif
	}

	return false;
}

/** @function	: GetCustomSkillCoolTime
	@brief		: Ư�� ��ų�� ���� ������Ʈ ���� ó��
	@param		: ���� ��Ÿ��, ��ų ���̵�
	@return		: ���� ��ų ��Ÿ��
*/
float	CX2GUAdd::GetCustomSkillCoolTime( IN const float fCoolTime_, IN const CX2SkillTree::SKILL_ID eSkillID_ )
{
	float fCoolTime = fCoolTime_;

	switch ( eSkillID_ )
	{
	case CX2SkillTree::SI_SA_APT_STASIS_FIELD:		/// �����̽ý� �ʵ�
		{
			if ( true == m_StasisfieldData.GetIsActiveStasisfield() )
			{
				fCoolTime = -1.f;
			}
		} break;
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:			/// ���� ��ȭ
		{
			if ( INVALID_DAMAGE_EFFECT_HANDLE != m_PhaseShiftData.m_hPhasShiftDamageEffect )
			{
				fCoolTime = -1.f;
			}
			/// ���� ��ȭ ���� Ȥ�� ȯ�� ������ �ߵ��ÿ� ��Ÿ�� ���� ����
			else if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PHASE_SHIFT ) )
			{
				fCoolTime = -1.f;
			}

		} break;
	case CX2SkillTree::SI_A_APT_PYLON_DYNAMO:		/// ���̳ʸ� ���� - ���Ϸ�
		{
			/// ���� ��ȯ ������ �����̶��, ��Ÿ�� ���� ����
			if ( true == GetPossibleNonstopSummonPylon() )
			{
				fCoolTime = -1.f;
			}
		} break;
	case CX2SkillTree::SI_A_AN_PULSE_BULLET:		/// ����ź
		{
			/// ȯ�� ������ �ߵ��ÿ� ��Ÿ�� ���� ����
			if ( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PULSE_BULLET ) )
				fCoolTime = -1.f;
		} break;

	case CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL:	/// ���ǹ� �׼�
		{
			/// ���ǹ� �׼� �����ÿ��� ��Ÿ�� ���� ���� �ʴ´�.
			if ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == GetFutureStateSkillID() )
				fCoolTime = -1.f;
		} break;
	default:
		break;

#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif

#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
	case CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA:
		{
			/// official case 9028: mark NOT live (the marking cast) -> cooltime 0 (the write proceeds: 0 != -1) so the Recall is always available right after marking; mark live (the recall cast) -> the templet 9s
			if ( 0 == m_uidStigmaTarget )
				fCoolTime = 0.f;
		} break;
	case CX2SkillTree::SI_HA_AMM_APOCALYPSE:
		{
			/// official case 9601: marker buff live -> skip the cooltime WRITE (-1 sentinel) so the original 180s keeps ticking through the finisher (no free instant re-summon)
			if ( ThirdPath_IsApocalypseSummonActive() )
				fCoolTime = -1.f;
		} break;
#endif
#endif
	}

	return fCoolTime;
}

/** @function	: SetCustomAnimName
	@brief		: Ư�� ������ ��, �ִϸ��̼��� �ٲٴ� �Լ�
	@param		: �ִϸ��̼� �̸�
*/
/*virtual*/ void CX2GUAdd::SetCustomAnimName( IN const UserUnitStateData& UserStateData, IN OUT wstring& wstrAnimName )

{
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	if ( false == GetIsFormationMode() )
		return;

	const int iClass = ( NULL != GetUnit() ) ? (int)GetUnit()->GetClass() : 0;
	const wchar_t* pszPath = NULL;
	if ( (int)CX2Unit::UC_ADD_PSYCHIC_TRACER == iClass ) pszPath = L"APT";
	else if ( (int)CX2Unit::UC_ADD_ARC_TRACER == iClass ) pszPath = L"AAT";
	else if ( (int)CX2Unit::UC_ADD_TIME_TRACER == iClass ) pszPath = L"ATT";

#endif
	switch( UserStateData.stateID )
	{
	case USI_DASH:
	case ANSI_AIR_DASH:
		{
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
			/// 占쏙옙占쏙옙占쏙옙 占쏙옙占쏙옙占쏙옙占?占쏙옙
			if ( NULL == GetUnit() || (int)CX2Unit::UC_ADD_NASOD_RULER != iClass )
#else
			/// ������ ������� ��
			if ( true == GetIsFormationMode() )
#endif
				wstrAnimName = L"Formation_Dash";
		} break;
	case USI_DASH_END:
		{
			/// ������ ��� ������ ��
			if ( true == GetIsFormationMode() )
				wstrAnimName = L"Formation_DashEnd";
		} break;
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
	case USI_HYPER_MODE:
		if ( NULL != pszPath ) { wstrAnimName = pszPath; wstrAnimName += L"_Formation_Start"; }
		break;
	case ANSI_HYPER_WAIT:
		if ( NULL != pszPath ) { wstrAnimName = pszPath; wstrAnimName += L"_Formation_Wait"; }
		break;
	case ANSI_COMBO_NASOD_ARMOR_LANDING:
		if ( (int)CX2Unit::UC_ADD_ARC_TRACER == iClass ) wstrAnimName = L"AAT_NA_preAttack_Landing";
		else if ( (int)CX2Unit::UC_ADD_TIME_TRACER == iClass ) wstrAnimName = L"ATT_NA_preAttack_Landing";
		break;
#endif
	}
}

CX2SkillTree::ACTIVE_SKILL_USE_CONDITION CX2GUAdd::GetSkillUseCondition( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )
{
	CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = pSkillTemplet_->m_eActiveSkillUseCondtion;

	switch (pSkillTemplet_->m_eID)
	{
	case CX2SkillTree::SI_SA_AHM_CHAIN_LIGHTNING:
		{
			if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO17) == true )
				eActiveSkillUseCondition = CX2SkillTree::ASUT_AIR;
		} break;
	case CX2SkillTree::SI_SA_APT_STASIS_FIELD:
		{
			// �����̽ý� �ʵ� ����ó��
			if( true == m_StasisfieldData.GetIsActiveStasisfield() &&
				CX2SkillTree::SI_SA_APT_STASIS_FIELD != m_FutureStateData.m_eSkillID )
				eActiveSkillUseCondition = CX2SkillTree::ASUT_ANYTIME_EXCEPT_AIR;
		} break;
	default:
		break;
	}

	return eActiveSkillUseCondition;
}

/** @function	: ConsumeMPorDP
	@brief		: MP Ȥ�� DP �Һ� �˻� �Լ�
	@param		: MP �Һ�, DP �Һ�
	@return		: ������ ���� ���� �Һ� �Ǿ����� ����
*/
bool CX2GUAdd::ConsumeMPorDP( IN const float fConsumeMP, IN const float fConsumeDP )
{
	if ( true == GetIsFormationMode() )
	{
		if ( GetDPValue() <= fConsumeDP )
			return false;
		else
			UpNowDPValue( -fConsumeDP );
	}
	else
	{
		return FlushMp( fConsumeMP );
	}

	return true;
}

/** @function	: GetDPRateByType
	@brief		: DP ���� Ÿ�Կ� ���� ���� ��
	@param		: DP ���� Ÿ��
	@return		: ���� ��
*/
float CX2GUAdd::GetDPRateByType( IN const CX2DamageManager::DYNAMO_PARTICLE_RATE_TYPE eDPRateType )
{
	switch ( eDPRateType )
	{
	case CX2DamageManager::DPRT_SPECIAL_ACTIVE:	/// ����� ��Ƽ��
		{
			return m_fDPRateBySpecialActive;
		} break;
	case CX2DamageManager::DPRT_ACTIVE:			/// ��Ƽ��
		{
			return m_fDPRateByActive;
		} break;
	case CX2DamageManager::DPRT_COMMAND:		/// Ŀ�ǵ�
		{
			return m_fDPRateByCommand;
		} break;
	case CX2DamageManager::DPRT_NASOD_ARMOR:	/// ���ҵ� �Ƹ�
		{
			/// �������� �ƴϸ�, ������ 0���� ����
			return ( true == GetIsFormationMode() ) ? m_fDPRateByNasodArmor : 0.f;
		} break;
	default:
		{
			/// Ÿ���� ������� �ʰų�, �̻��� ���� ��� �־��!
			//ASSERT( ! "Unknown DYNAMO_PARTICLE_RATE_TYPE!!!" );
			return 0.f;
		} break;
	}
}

/** @function	: GetMutationCount
	@brief		: ���� ��ġ ��ȯ �Լ�
	@return		: ���� ��ġ
*/
int CX2GUAdd::GetMutationCount()
{
	return ( NULL != GetAddGageData() ) ? GetAddGageData()->GetMutationCount() : 0;
}

/** @function	: SetChangedMutationCount
	@brief		: ���� ��ġ ���� ���� ����
	@param		: ���� ��ġ ���� ����
*/
void CX2GUAdd::SetChangedMutationCount( IN const bool bChangedMutationCount_ )
{
	if ( NULL != GetAddGageData() )
	{
		GetAddGageData()->SetChangedMutationCount( bChangedMutationCount_ );
	}
}

/** @function	: SetMutationCount
	@brief		: ���� ��ġ ���� �Լ�
	@param		: ���� ��ġ
*/
/*virtual*/ void CX2GUAdd::SetMutationCount( IN const int iMutationCount_ )
{
	if ( NULL != GetAddGageData() )
	{
		GetAddGageData()->SetChangedMutationCount( true );
		GetAddGageData()->SetMutationCount( iMutationCount_ );
	}
}

/** @function	: UpMutationCount
	@brief		: ���� ��ġ ���� �Լ�
	@param		: ���� ��ġ
*/
void CX2GUAdd::UpMutationCount( IN const int iMutationCount_  )
{
	if ( NULL != GetAddGageData() )
	{
		int iNowMutationCount = GetAddGageData()->GetMutationCount() + iMutationCount_ ;

		if ( MAX_MUTATION_COUNT_VALUE < iNowMutationCount )
			iNowMutationCount = MAX_MUTATION_COUNT_VALUE;
		else if ( 0 >= iNowMutationCount )
			iNowMutationCount = 0;

		GetAddGageData()->SetChangedMutationCount( true );
		GetAddGageData()->SetMutationCount( iNowMutationCount );
	}
}

/** @function	: GetMaxDPValue
	@brief		: �ִ� DP ��ġ ��ȯ �Լ�
	@return		: �ִ� DP ��ġ
*/
float	CX2GUAdd::GetMaxDPValue()
{
	return ( NULL != GetAddGageData() ? GetAddGageData()->GetMaxDPValue() : 0.f );
}

/** @function	: GetDPValue
	@brief		: DP ��ġ ��ȯ �Լ�
	@return		: DP ��ġ
*/
float CX2GUAdd::GetDPValue()
{
	return ( NULL != GetAddGageData() ? GetAddGageData()->GetDPValue() : 0.f );
}

/** @function	: SetDPValue
	@brief		: DP ��ġ ���� �Լ�
	@param		: DP ��ġ
*/
/*virtual*/ void CX2GUAdd::SetDPValue( IN const float fDPValue_ )
{
	if ( NULL != GetAddGageData() )
		GetAddGageData()->SetDPValue( fDPValue_ );
}

/** @function	: UpNowDPValue
	@brief		: DP ��ġ ���� �Լ�
				  ����, ��ų ȿ���� ���� ���� ����

				  ���� ȿ���� ���� ���� ��Ƣ�� �Ǵ� ������ �����ϱ� ����, ���ڷ� rate�� ����

				  ���� : fNowDPValue * ����� * ��彺ų * �нú꽺ų
				 		  1000      *  1.5   *    1.5   * 1.5 -> 3.37�� ����
				  ���� : fNowDPValue * ( 1 + (����� - 1) + (��彺ų-1) + (�нú꽺ų-1) )
				 		  1000      * ( 1 + 0.5  + 0.5 + 0.5 ) -> 2.5�� ����

	@param		: fDPValue_(DP ��ġ), fChangeValueRate_ ( �� ������ ���� �� )
*/
/*virtual*/ void CX2GUAdd::UpNowDPValue( IN const float fDPValue_, IN const float fChangeValueRate_ /*= 1.f */ )
{
	if( 0 == fDPValue_ )
		return;

	float		fNowDPValue = fDPValue_;			/// ���� ��ų DP ��ġ
	const float	fMaxNPValue = GetMaxDPValue();		/// �ִ� DP ��ġ

	// DPȹ��
	// �нú� ��ų(���̳ʸ� ����), ���� ȿ��(���������ӵ�), ��� ��ų ���� ����
	if ( 0.f < fNowDPValue )
	{
		float fChangeRate = 0.f;
		{
			if ( NULL != g_pData &&
				NULL != g_pData->GetMyUser() &&
				NULL != g_pData->GetMyUser()->GetSelectUnit()  )
			{
				// ���� �� �нú� ��ų�� ���� DP ȹ�淮 ������
				const float fChangeRate_BySocketAndPassive = GetEffectiveIncreaseDPValue();

				// ���ڷ� ���� ��Ÿ ȿ�� ( ex 20% ���� : 1.2 -> 0.2f )
				const float fChangeRate_ByArg = fChangeValueRate_ - 1.f;

				// ex ( 1 + 0.5f + 0.15f + 0.2f = 1.85f )
				fChangeRate = 1.f + ( fChangeRate_BySocketAndPassive + fChangeRate_ByArg );
			}
		}
		fNowDPValue *= fChangeRate;
	}
	// DP �Ҹ�
	// �нú� ��ų(���̳ʸ� ����), ���� ȿ��(�������ӽð�) ���� ����
	else if( 0.f > fNowDPValue )
	{
		float fChangeRate = 0.f;
		{
			// ���� �� �нú� ��ų�� ���� DP �Ҹ� ���Ұ�
			const float fChangeRate_BySocketAndPassive = GetEffectiveDecreaseDPConsume();

			// ���ڷ� ���� ��Ÿ ȿ�� ( ex -10%���� : 0.9 -> 0.1)
			const float fChangeRate_ByArg = 1.f - fChangeValueRate_;

			// ( ex  1.f - 0.06f - 0.02f - 0.1f = 0.82f )
			fChangeRate = 1.f - fChangeRate_BySocketAndPassive - fChangeRate_ByArg;
		}
		fNowDPValue *= fChangeRate;
	}

	/// ���� DP ��ġ�� ����
	fNowDPValue += GetDPValue();

	/// DP �ִ� �� �ּ�ġ ����
	if ( fMaxNPValue < fNowDPValue )
		fNowDPValue = fMaxNPValue;
	else if ( 0.f >= fNowDPValue )
		fNowDPValue = 0.f;

	SetDPValue( fNowDPValue );
}

/** @function	: UpNowDPValueByDamage
	@brief		: �ǰݽ� DP ���� ó��
	@param		: ������
*/
/*virtual*/ void CX2GUAdd::UpNowDPValueByDamage( IN const float fDamage_ )
{
	/// ���� ��� ���϶�, ����
	if ( true == GetIsFormationMode() )
		return;

	/// ���� ü�� ����
	float fNowDPValue = fDamage_ / GetMaxHp();

	/// �ڽ��� �ִ� ü�º��� �� ū �������� ������ ��, 1������ ����
	if ( 1.f < fNowDPValue )
		fNowDPValue = 1.f;

	/// �ǰ� �� DPȹ�淮 = ( ���� ���ط� / ��HP�� ) * 2400
	fNowDPValue = fNowDPValue * m_fDPRateByFormationModeHit;


	// ��ų�� ���� ���� �� ����
	float fIncreaseValueRateBySkill = 1.f;
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2UserSkillTree& cUserSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;		// ������ ��� ��ų Ʈ��

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
		fIncreaseValueRateBySkill = cUserSkillTree.GetIncreaseGainSoulRateByDamage();

		fIncreaseValueRateBySkill *= GetSpecialAbilityValue( CX2SkillTree::SA_DP_GAIN_REL, CX2SkillTree::SI_P_ATT_NASOD_ARMOR_MODE_OVERLIMIT );
		//}} 2026-09-05 //

		if ( false == GetIsFormationMode() )
			fIncreaseValueRateBySkill += ( m_fOverlimitDpUse - 1.0f );
		//}} 2026-09-07 //			// 占실격쏙옙 占쏙옙占쏙옙占실댐옙 占쏙옙占쏙옙占쏙옙占쏙옙占쌈듸옙
#else
		fIncreaseValueRateBySkill = cUserSkillTree.GetIncreaseGainSoulRateByDamage();			// �ǰݽ� �����Ǵ� ���������ӵ�
#endif
	}

	UpNowDPValue( fNowDPValue, fIncreaseValueRateBySkill );
}

/** @function	: UpNowDPValueByHit
	@brief		: Ÿ�ݽ� DP ���� ó��
	@param		: ������ ������
*/
/*virtual*/ void CX2GUAdd::UpNowDPValueByHit( IN const CX2DamageManager::DamageData& DamageData_ )
{
	const float fPowerRate		= GetPowerRate();	/// �⺻ ������ ����
	const float fSkillTypeRate	= GetDPRateByType( DamageData_.m_DPRateType );	/// ��ų Ÿ�Կ� ���� ����
	float		fDamageRate		= 0.f;				/// ������ Ÿ�Կ� ���� ����

	/// ������ Ÿ�Կ� ���� ���� ����
	switch ( DamageData_.damageType )
	{
	case CX2DamageManager::DT_PHYSIC:	fDamageRate = DamageData_.damage.fPhysic; break;
	case CX2DamageManager::DT_MAGIC:	fDamageRate = DamageData_.damage.fMagic; break;
	case CX2DamageManager::DT_MIX:		fDamageRate = ( DamageData_.damage.fPhysic + DamageData_.damage.fPhysic ) / 2.f; break;
	default:
		break;
	}

	float fNowDPValue = 0.f;		/// �����ų DP ��ġ

	/// ���� ��� ���ο� ����, ��ġ ����
	if ( false == GetIsFormationMode() )
		/// Ÿ�� �� DPȹ�淮 = ������ ���� * �Ŀ�����Ʈ * 20 * ��ųTYPE��� (Ŀ�ǵ�2, ��Ƽ��1, ����Ⱦ�Ƽ��0)
		fNowDPValue += fPowerRate * fDamageRate * m_fDPRateByNoFormationModeAttack * fSkillTypeRate;
	else
		/// Ÿ�� �� DP�Ҹ� = ������ ���� * �Ŀ�����Ʈ * 20 * ��ųTYPE��� (Ŀ�ǵ�2, ��Ƽ��1, ����Ⱦ�Ƽ��0)
		fNowDPValue -= fPowerRate * fDamageRate * m_fDPRateByFormationModeAttack * fSkillTypeRate;

	// ��ų�� ���� ���� �� ����
	float fIncreaseValueRateBySkill = 1.f;
	if( NULL != g_pData &&
		NULL != g_pData->GetMyUser() &&
		NULL != g_pData->GetMyUser()->GetSelectUnit() )
	{
		const CX2UserSkillTree& cUserSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;	// ������ ��� ��ų Ʈ��

#ifdef X2_ADD_3RD_PATH
		fIncreaseValueRateBySkill = cUserSkillTree.GetIncreaseGainSoulRateByHit();			// 타占쌥쏙옙 占쏙옙占쏙옙占실댐옙 占쏙옙占쏙옙占쏙옙占쏙옙占쌈듸옙

		if ( false == GetIsFormationMode() )
			fIncreaseValueRateBySkill += ( m_fOverlimitDpGain - 1.0f );
		//}} 2026-09-07 //
#else
		fIncreaseValueRateBySkill = cUserSkillTree.GetIncreaseGainSoulRateByHit();			// Ÿ�ݽ� �����Ǵ� ���������ӵ�
#endif
	}

	UpNowDPValue( fNowDPValue, fIncreaseValueRateBySkill );
}

/** @function	: UpNowDPValueByUseMP
	@brief		: MP ���� DP ���� ó��
	@param		: MP ��뷮
*/
void CX2GUAdd::UpNowDPValueByUseMP( IN const float fUseMP )
{
	/// ���� ��� ���϶�, ����
	if ( true == GetIsFormationMode() )
		return;

	/// MP�Ҹ�� DPȹ�淮 = �����MP * 4.8
	const float fNowMPValue = fUseMP * m_fDPRateByFormationModeUseMP;

	UpNowDPValue( fNowMPValue );
}

/** @function	: �ش� ��ų�� ���� DP �Ҹ� ��ȯ
	@brief		: GetActualDPConsume
	@param		: ��ų ����
	@return		: DP �Ҹ�
*/
float CX2GUAdd::GetActualDPConsume( IN const CX2UserSkillTree::SkillSlotData* pSkillSlotData_ )
{
	float fDPConsume = 0.f;		/// DP �Ҹ�

	/// �񰢼��� MP�� ����, ������ DP�� ���� ��ų�� ���� DP �Ҹ� ����
	if ( true == GetIsFormationMode() )
	{
		/// �񰢼��� MP�� ����, ������ DP�� ���� ��ų�� ���� DP �Ҹ� ����
		const float fUseDPSkillAtFormationMode = GetSpecialAbilityValue( CX2SkillTree::SA_DP_USE_REL, pSkillSlotData_->m_eID );

		if ( 0.f < fUseDPSkillAtFormationMode )
		{
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
			const int iUnitClassGate = (int)GetUnitClass();
			if ( iUnitClassGate != (int)CX2Unit::UC_ADD_TIME_TRACER && iUnitClassGate != (int)CX2Unit::UC_ADD_DIABOLIC_ESPER
				&& pSkillSlotData_->m_eID != CX2SkillTree::SI_SA_APT_PULSE_CANNON )
				fUseDPSkillAtFormationMode = 0.f;
		}
		//}} 2026-09-05 //

		if ( 0.f < fUseDPSkillAtFormationMode )
		{
			/// 占쌀몌옙 MP占쏙옙 占쏙옙占쏙옙占쏙옙 DP 占쌀몌옙 ( UI占쏙옙 표占시듸옙 DP占쏙옙 占쏙옙占쏙옙 占쏙옙 DP占쏙옙 20占쏙옙 크占쏙옙 )
			float fMPConsume = CX2GUUser::GetActualMPConsume( pSkillSlotData_->m_eID, GetSkillLevelValue( pSkillSlotData_->m_eID ) );

#ifdef X2_ADD_2ND_PATH

			switch ( pSkillSlotData_->m_eID )
			{
			case CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE:
				if ( false == m_vecMaxStrikeHandles.empty() )
					fMPConsume = 0.f;
				break;
			case CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA:
				if ( 0 != m_uidStigmaTarget )
					fMPConsume = 20.f;
				break;
			default:
				break;
			}
			//}} 2026-09-05 //
#endif
#else
			/// �Ҹ� MP�� ������ DP �Ҹ� ( UI�� ǥ�õ� DP�� ���� �� DP�� 20�� ũ�� )
			const float fMPConsume = CX2GUUser::GetActualMPConsume( pSkillSlotData_->m_eID, GetSkillLevelValue( pSkillSlotData_->m_eID ) );
#endif

			fDPConsume = fUseDPSkillAtFormationMode * fMPConsume * 20.f;
		}
	}
	else
	{
		/// DP�� ��� ���ڸ� ����Ѵ�.
		if ( NULL != pSkillSlotData_ )
			fDPConsume = static_cast<float>( pSkillSlotData_->m_usFPConsumption );

		switch ( pSkillSlotData_->m_eID )
		{
		case CX2SkillTree::SI_A_AN_PHASE_SHIFT:
			{
				/// ���� ��ȭ ���ͽ�, DP�Ҹ����� �ʱ�
				if ( true == m_PhaseShiftData.IsValidReturnPosValue() )
					fDPConsume = 0.f;
				/// ȯ�� ������ �ߵ���, DP�Ҹ����� �ʱ�
				else if( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PHASE_SHIFT ) )
					fDPConsume = 0.f;
			} break;
		case CX2SkillTree::SI_A_AN_PULSE_BULLET:
			{
				/// ȯ�� ������ �ߵ���, DP�Ҹ����� �ʱ�
				if( true == GetIsFantasyTracerTargetSkillID( CX2SkillTree::SI_A_AN_PULSE_BULLET ) )
					fDPConsume = 0.f;
			} break;

		case CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL:
			{
				/// ���ǹ� �׼� �����ÿ��� DP�Ҹ����� �ʱ�
				if ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == GetFutureStateSkillID())
					fDPConsume = 0.f;
			} break;
#ifdef X2_ADD_2ND_PATH

		case CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_SHOOTING_CHASER:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY:
		case CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL:
			{
				if ( m_iFactoryMarkedSkillID == (int)pSkillSlotData_->m_eID )
					fDPConsume = 0.f;
			} break;
		//}} 2026-09-04 X2GUAdd reverify GAUGE-2 //
#endif
		default:
			break;
		}
	}

	return fDPConsume;
}

/** @function	: �ش� ��ų�� ���� ���� ����Ʈ �Ҹ� ��ȯ
	@brief		: GetMutationPointConsume
	@param		: ��ų ����
	@return		: ���� ����Ʈ �Ҹ�
*/
int CX2GUAdd::GetMutationPointConsume( IN const CX2UserSkillTree::SkillSlotData* pSkillSlotData_ )
{
	int iMutationPointConsume = 0;		/// ���� ����Ʈ �Ҹ�

	/// ���� ����Ʈ�� ĳ���� ���ڸ� ����Ѵ�.
	if ( NULL != pSkillSlotData_ )
		iMutationPointConsume = static_cast<int>( pSkillSlotData_->m_usCBConsumption );

	return iMutationPointConsume;
}

/** @function	: GetIsFormationMode
	@brief		: ���� ��� ���� ���� ��ȯ �Լ�
	@return		: ���� ��� ���� ����
*/
/*virtual*/ bool CX2GUAdd::GetIsFormationMode() const
{
	return ( NULL != GetAddGageData() ? GetAddGageData()->GetIsFormationMode() : false );
}

/** @function	: SetIsFormationMode
	@brief		: ���� ��� ���� ���� ���� �Լ�
	@param		: ���� ��� ���� ����
*/
/*virtual*/ void CX2GUAdd::SetIsFormationMode( IN const bool bIsFormationMode_ )
{
	if ( NULL != GetAddGageData() )
		GetAddGageData()->SetIsFormationMode( bIsFormationMode_ );

	/// ���� ��� ��Ÿ�� ����
	if ( true == bIsFormationMode_ )
		SetChangeFormationCoolTime( m_fChangeFormationCoolTime );

#ifdef X2_ADD_3RD_PATH

	UpdateDEFormationFace();
	//}} 2026-09-07 //
#endif
}

#ifdef X2_ADD_3RD_PATH


/** @function	: CreateDEFormationFaceObj
	@brief		: build the separate berserk-face render object (item 41841) for the DE unit, attach it to this
				  GU's scene graph, hidden by default. DE-only. Called at ReInit ONLY -- formation mode toggles
				  visibility, it never creates/destroys (official slot_14).
*/
void CX2GUAdd::CreateDEFormationFaceObj()
{
	/// DE-only gate (official: unit->m_UnitClass == 126)
	if ( NULL == GetUnit() || CX2Unit::UC_ADD_DIABOLIC_ESPER != GetUnit()->GetClass() )
	{
		ReleaseDEFormationFaceObj();
		return;
	}
	if ( NULL == m_pXSkinAnim )
		return;

	/// create once -- a ReInit re-run (DE -> DE) keeps the live object; the official swaps the handle each
	/// ReInit but the object is stateless besides visibility, and the per-frame driver + setter re-drive it
	if ( NULL == m_pDEFormationFaceObj )
	{
		m_pDEFormationFaceObj = CX2ItemManager::CreateDummyEquip( 41841, GetUnit(), m_pXSkinAnim );
		if ( NULL != m_pDEFormationFaceObj )
		{
			m_pDEFormationFaceObj->SetShowObject( false );		/// HIDDEN by default (official)
			m_pDEFormationFaceObj->SetManagerObject( this );	/// attach to the unit scene graph (official SetParent)
			/// if this GU is already in the render chain (respawn / map-change), chain the face now; otherwise
			/// the NotifyDGManagerChanged cascade chains it when the GU enters a manager (scene attach)
			if ( IsRecursivelyManagedByDGManager() && false == m_pDEFormationFaceObj->IsManagedByDGManager()
				&& NULL != g_pKTDXApp )
				g_pKTDXApp->GetDGManager()->AddObjectChain( m_pDEFormationFaceObj.get() );
		}
	}
}

/** @function	: ReleaseDEFormationFaceObj
	@brief		: refcounted release of the berserk-face object (the non-DE ReInit path / teardown). The
				  refcount hitting zero runs the CKTDGObject deleter (which also chain-removes); the explicit
				  RemoveObjectChain first mirrors the Arme transform-weapon teardown.
*/
void CX2GUAdd::ReleaseDEFormationFaceObj()
{
	if ( NULL == m_pDEFormationFaceObj )
		return;

	if ( m_pDEFormationFaceObj->IsManagedByDGManager() && NULL != g_pKTDXApp )
		g_pKTDXApp->GetDGManager()->RemoveObjectChain( m_pDEFormationFaceObj.get() );
	m_pDEFormationFaceObj.reset();
}

/** @function	: UpdateDEFormationFace
	@brief		: official f_324EF7B0 -- drive the berserk-face visibility: show iff (formation mode on) AND
				  (unit shown) AND (not in the suppress state). Called from CommonFrameMove (per-frame) +
				  SetIsFormationMode (the toggle choke point). Cheap: SetShowObject no-ops when unchanged.
*/
void CX2GUAdd::UpdateDEFormationFace()
{
	/// DE-only gate (official: unit->m_UnitClass == 126)
	if ( NULL == GetUnit() || CX2Unit::UC_ADD_DIABOLIC_ESPER != GetUnit()->GetClass() )
		return;
	if ( NULL == m_pDEFormationFaceObj )
		return;

	/// DESIGN INFERENCE (de_render_official.md section 9 open item): the official suppress gate = the unit's
	/// state set +0x848 containing value 2 (f_31FDE190 -- a "hide the formation face while an alternate
	/// appearance is active" unit state). Our tree has NO per-unit state-set equivalent -- the closest existing
	/// "is transformed / hide-default-face" check = CX2GUUser::IsTransformed() (the transform-skill flag).
	const bool bShow = ( true == GetIsFormationMode() )
		&& ( true == GetShowObject() )
		&& ( false == IsTransformed() );
	m_pDEFormationFaceObj->SetShowObject( bShow );
}

/** @function	: NotifyDGManagerChanged
	@brief		: official slot_10 (0x324EB5F0) -- keep the berserk-face object in the active DGManager chain
				  when this GU enters/leaves a scene (device reset/lost, world swap). Mirrors the GUUser
				  m_ViewEqipList cascade (X2GUUser.cpp:13269) + the Chung promotion-equip override
				  (X2GUChung.cpp:6780). AddObjectChain is NOT duplicate-safe -- guard with IsManagedByDGManager.
*/
/*virtual*/ void CX2GUAdd::NotifyDGManagerChanged( CKTDGManager& refInManager_ )
{
	CX2GUUser::NotifyDGManagerChanged( refInManager_ );

	if ( NULL == m_pDEFormationFaceObj )
		return;

	if ( IsRecursivelyManagedByDGManager() )
	{
		if ( false == m_pDEFormationFaceObj->IsManagedByDGManager() )
			refInManager_.AddObjectChain( m_pDEFormationFaceObj.get() );
	}
	else
	{
		if ( true == m_pDEFormationFaceObj->IsManagedByDGManager() )
			refInManager_.RemoveObjectChain( m_pDEFormationFaceObj.get() );
	}
}
//}} 2026-09-07 //
#endif

/** @function	: IsPossibleChangeNasodBarrier
	@brief		: ���ҵ� �踮�� ���� ó�� �Լ�
	@return		: ���ҵ� �踮�� ���� ����
*/
bool CX2GUAdd::IsPossibleChangeNasodBarrier(  IN const CX2DamageManager::DamageData* pDamageData_ )
{
	if ( NULL != pDamageData_ )
	{
		/// ���ҵ� �踮�� �ߵ� ������ ������Ʈ �̸� DP�� ���� �ְ�, ���� �������� �ƴ϶�� �踮�� �ߵ�
		if ( true == m_bPossibleNasodArmorModeState &&
			 true == GetIsFormationMode() &&
			 0.f < GetDPValue() &&
			 CX2DamageManager::RT_DUMMY_DAMAGE != pDamageData_->reActType )
		{
			/// DP ������ ������Ʈ�� ��ȯ
			StateChange( ANSI_DP_BARRIER_ACTIVE );

			return true;
		}
	}

	return false;
}

/** @function	: PlayAnimationWaitStartByAdd
	@brief		: ��Ȳ�� ���� Wait ��� ó��
	@param		: �ִϸ��̼� ��ü, ������ ������
*/
void CX2GUAdd::PlayAnimationWaitStartByAdd( IN CKTDGXSkinAnim* pXSkinAnim_, IN const FrameData& refFrameData_ )
{
	CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE playType;
	LUA_GET_VALUE_ENUM ( m_LuaManager, "PLAY_TYPE", playType, CKTDGXSkinAnim::XSKIN_ANIM_PLAYTYPE, CKTDGXSkinAnim::XAP_ONE_WAIT );

	// FATAL_HP_PERCENT ���� HP�� ������ fatal �ִϸ��̼��� ����
	const float FATAL_HP_PERCENT = 0.25f;

	if( GetMaxHp() * FATAL_HP_PERCENT >= refFrameData_.syncData.fNowHP )
	{
		bool bTransition = true;
		LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );

		if( false == refFrameData_.unitCondition.bTransitionChange || false == bTransition )
			pXSkinAnim_->ChangeAnim( L"Fatal", false );
		else
			pXSkinAnim_->ChangeAnim( L"Fatal", true );
	}
	else
	{
		bool bTransition = true;
		LUA_GET_VALUE( m_LuaManager, "TRANSITION", bTransition, true );

		if( false == refFrameData_.unitCondition.bTransitionChange || false == bTransition )
		{
			pXSkinAnim_->ChangeAnim( L"Wait", false );
		}
		else
		{
			pXSkinAnim_->ChangeAnim( L"Wait", true );
		}
	}

	pXSkinAnim_->Play( playType );
}

/** @function	: IsDPConsumptionSkill
	@brief		: �ش� ��ų�� DP�� ����ϴ� ��ų���� �˻�
				/// ***�����ÿ��� DP�� �Ҹ��ϴ���, DP�Ҹ��ϴ� ��ų�� �Ǵ� ��
	@param		: ��ų ���̵�
	@return		: DP ��ų ����
*/
bool CX2GUAdd::IsDPConsumptionSkill( IN const CX2SkillTree::SKILL_ID eSkillID_ ) const
{
	// ID�� �������� ������, DP �Ҹ����� �ʴ� ��ų�� �Ǵ� ��
	if( CX2SkillTree::SI_NONE == eSkillID_)
		return false;

	if ( NULL != g_pData && NULL != g_pData->GetSkillTree() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( eSkillID_ );

		if ( NULL == pSkillTemplet )
			return false;

		/// DP �Ҹ��� �޾ƿͼ�, �Ҹ��� �����Ѵٸ� DP�� ����ϴ� ��ų�� �Ǵ�
		const float fDPConsumptionValue = pSkillTemplet->GetSkillDPConsumptionValue();

		if ( 0 < fDPConsumptionValue )
			return true;
#ifdef X2_ADD_3RD_PATH


		float fDPRelValue = GetSpecialAbilityValue( CX2SkillTree::SA_DP_USE_REL, eSkillID_ );

		if ( 0.f < fDPRelValue )
		{
		const int iUnitClassGate = ( NULL != GetUnit() ) ? (int)GetUnit()->GetClass() : (int)CX2Unit::UC_ADD_TIME_TRACER;
		if ( iUnitClassGate != (int)CX2Unit::UC_ADD_TIME_TRACER && iUnitClassGate != (int)CX2Unit::UC_ADD_DIABOLIC_ESPER
			&& eSkillID_ != CX2SkillTree::SI_SA_APT_PULSE_CANNON )
			fDPRelValue = 0.f;
		}
		//}} 2026-09-05 //
		if ( 0.f < fDPRelValue )
			return true;
		//}} 2026-09-04 X2GUAdd reverify GAUGE-7 //
#endif
	}

	return false;	/// DP�� �Ҹ��ϴ� ��ų�� �ƴϳ׿�
}
/** @function : GetUseDPByFormationMode
	@brief : ���� ���� ��ȯ�� �Ҹ�Ǵ� DP ��ġ
*/
float CX2GUAdd::GetUseDPByFormationMode()
{
	ASSERT( 0.f < m_fDPValueRateByActiveFormationMode );

	// ���� �� �нú� ��ų�� ���� DP �Ҹ� ���Ұ�
	const float fConsumeDP = m_fUseDPByFormationMode * ( m_fDPValueRateByActiveFormationMode + GetEffectiveDecreaseDPConsume() );

	return fConsumeDP;
}

/** @function	: GetEffectiveIncreaseDPValue
	@brief		: DP ȹ�淮 ������ ��ȯ
	@return		: DP ȹ�淮 ������
*/
float CX2GUAdd::GetEffectiveIncreaseDPValue()
{
	// ���� ȿ��(���������ӵ�) ( ex 50%���� : 0.5f )		/// �ֵ�� ���ݸ� ���� �ȴ�.  ( ���� ����� ���� �����Ƿ�, ���⿡�� ���� ó�� ���� )
	/// ���� ���� ������ ����

	const float fChangeRate_BySocket = GetHyperChargeSpeed();


	// �нú� ��ų(���̳ʸ� ����) ( ex 15%���� : 1.15 -> 0.15f )
	const float fChangeRate_ByPassiveSkill = m_fGainDPValueChangeRate - 1.f;

	return fChangeRate_BySocket + fChangeRate_ByPassiveSkill;
}

/** @function	: GetEffectiveDecreaseDPConsume
	@brief		: DP �Ҹ� ���Ұ� ��ȯ
	@return		: DP �Ҹ� ���Ұ�
*/
float	CX2GUAdd::GetEffectiveDecreaseDPConsume()
{
	// ���� ȿ��(�������ӽð�)  (ex 6%���� : 6 -> 0.06)		/// �ֵ�� ���ݸ� ���� �ȴ�. ( ���� ����� ���� �����Ƿ�, ���⿡�� ���� ó�� ���� )
	const float fChangeRate_BySocket = GetSocketData().m_fHyperModeTime * 0.01f;

	// �нú� ��ų(���̳ʸ� ����) (ex -2%���� : 0.98 -> 0.02)
	const float fChangeRate_Passive = 1.f - m_fUsingDPValueChangeRate;
	return fChangeRate_BySocket + fChangeRate_Passive;
}

/** @function	: StateChange
	@brief		: ������Ʈ ���� �Լ�
	@param		: ������Ʈ ���̵�, ��� ������Ʈ ���� ó�� ����
	@return		: ���� ���� ����
*/
/*virtual*/ bool CX2GUAdd::StateChange( int state, bool bDirectChange /*= false*/ )
{
	/// ��� ������ ��, ���� ��� ���̶�� ���� ��� ��� ���·� ��������.
	if ( USI_WAIT == state )
	{
		if ( true ==  GetIsFormationMode() )
			state = ANSI_HYPER_WAIT;
	}

	return CX2GUUser::StateChange( state, bDirectChange );
}

/** @function : ApplyHyperModeBuff
	@brief : �������� ����
*/
/*virtual*/ void CX2GUAdd::ApplyHyperModeBuff()
{
	SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_FORMATION_MODE ), 1 );

	// ������ ������ ���� ����
	if( 0 < m_iLibraryOfLimitlessLevel )
		SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_LIBRARY_OF_LIMITLESS_MANA_RECOVERY ), m_iLibraryOfLimitlessLevel );

	/// ������ ������ ������ �ִ��� �˻�
	if ( false == m_SocketData.m_vecInfoByUseHyperMode.empty() &&
		GUT_USER == GetGameUnitType() )
	{
		CX2GUUser* pGUUser = static_cast<CX2GUUser*>( this );

		/// ���� ����
		if ( NULL != pGUUser )
		{
				pGUUser->ApplyBuffByUseHyperMode();
		}
	}
}

/** @function	: GetAbilitySyncData
	@brief		: ����ȭ ��ų ������ ��ȯ �Լ�
	@return		: ����ȭ ��ų ������ ( DP, ���� ��ġ )
*/
SHORT CX2GUAdd::GetAbilitySyncData()
{
	SHORT sAbilitySyncData = 0;

	/// ���� ��ġ ���� ( ���� ��ġ�� ���� 3 ��Ʈ�� �Ѿ�� �ʵſ�! )
	sAbilitySyncData |= static_cast<SHORT>( GetMutationCount() );

	/// DP ��ġ ������ ����, �̵�
	sAbilitySyncData = sAbilitySyncData << 13;

	/// DP ��ġ ���� ( DP�� ���� 13 ��Ʈ�� �Ѿ�� �ʵſ�! )
	sAbilitySyncData |= static_cast<SHORT>( GetDPValue() );

	return sAbilitySyncData;
}

/** @function	: SetAbilitySyncData
	@brief		: ������ ����ȭ ���� ���� ���� �����ϴ� �Լ�
	@param		: ������ ����ȭ ��
*/
void CX2GUAdd::SetAbilitySyncData( IN const SHORT sAbilitySyncData_ )
{
	/// DP ��ġ ���� ( 3��Ʈ )
	const float fNPValue		= static_cast<float>( sAbilitySyncData_ & 0x1FFF ) ;
	SetDPValue( fNPValue );

	/// ���� ��ġ ���� ( 13��Ʈ )
	const int iMutationCount	= static_cast<int>( ( sAbilitySyncData_ >> 13 ) & 0x0007 );
	SetMutationCount( iMutationCount );
}

/** @function	: AddPylonLinkDamageEffect
	@brief		: ���Ϸ��� ���� ������ ����Ʈ ���� �Լ�
	@param		: �߰��� ���Ϸ� ������ ����Ʈ �ڵ�
*/
void CX2GUAdd::AddPylonLinkDamageEffect( IN CX2DamageEffect::CEffect* pAddPylon_ )
{
	if ( NULL == pAddPylon_ )
		return;

	const CX2DamageEffect::CEffectHandle hAddPylonHandle = pAddPylon_->GetHandle();

	/// ��ġ�Ǿ� �ִ� ���Ϸ� �˻�
	for( const PylonDamageEffectData& AnotherPylonData : m_vecSummonPylonDamageEffectData )
	{
		bool bIsLink = false;		/// �ٸ� ���Ϸ��� ������ ����Ʈ ���� ����

		/// ���� ������ ���� ������ ����Ʈ �˻�
		for( const PylonLinkDamageEffectData& LinkData : m_vecPylonLinkDamageEffectData )
		{
			const CX2DamageEffect::CEffectHandle iStartLinkPylonHandle	= LinkData.m_hPylonHandle.first;	/// ����� ���Ϸ� �ڵ�
			const CX2DamageEffect::CEffectHandle iEndLinkPylonHandle	= LinkData.m_hPylonHandle.second;	/// ����� �ݴ��� ���Ϸ� �ڵ�

			/// �˻��Ϸ��� �� ���Ϸ� �� ����� ������ ����Ʈ���� �˻�
			if ( ( hAddPylonHandle					== iStartLinkPylonHandle	|| hAddPylonHandle					== iEndLinkPylonHandle ) &&
				( AnotherPylonData.m_EffectHandle	== iStartLinkPylonHandle	|| AnotherPylonData.m_EffectHandle	== iEndLinkPylonHandle ) )
			{
				bIsLink = true;		/// ���� �Ǿ����.
				break;
			}
		}

		/// ���� �Ǿ� ���� �ʴٸ�, ����� ������ ����Ʈ ����
		if ( false == bIsLink )
		{
			/// ���Ϸ��� �������� �ʰų�, �׾������� ���� ���Ϸ��� �˻�
			if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )
				continue;

			CX2DamageEffect::CEffect* pAnotherEffect = g_pX2Game->GetDamageEffect()->GetInstance( AnotherPylonData.m_EffectHandle );

			if ( NULL == pAnotherEffect )
				continue;

			/// �� ����Ʈ ���� �Ÿ� �˻�
			float fDistance = GetDistance( pAddPylon_->GetPos(), pAnotherEffect->GetPos() );

			if ( 0 >= fDistance )
				fDistance = 1.f;

			/// Ư�� �Ÿ����� �ָ� ���� ���Ϸ��� �˻�
			if ( 1000.f	< fDistance )
				continue;

			/// Main Mesh ��ȯ
			CKTDGXMeshPlayer::CXMeshInstance* pAddMeshInst		= pAddPylon_->GetMainEffect();
			CKTDGXMeshPlayer::CXMeshInstance* pAnotherMeshInst	= pAnotherEffect->GetMainEffect();

			if ( NULL == pAddMeshInst || NULL == pAnotherMeshInst )
				continue;

			const D3DXVECTOR3	vAddPylonPos	= pAddMeshInst->GetBonePos(L"Dummy01");	/// ��ġ��
			const D3DXVECTOR3	vAddPylonRot	= pAddMeshInst->GetRotateDegree();					/// ȸ����

			/// ���� ������ ����Ʈ ����
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_APT_Pylon_Additional_Shock",
				GetPowerRate(), vAddPylonPos, vAddPylonRot, vAddPylonRot );

			if ( NULL == pEffect )
				continue;

			const D3DXVECTOR3 vAnotherPylonPos = pAnotherMeshInst->GetBonePos(L"Dummy01");

			/// �� ���Ϸ� �� ������ ����Ʈ �Ž� ���� ���� ( �ε��� ��Ŀ ���� )
			pEffect->SetLinkMainEffectByPos( vAddPylonPos, vAnotherPylonPos, fDistance );

			/// ������ ������ ����Ʈ ����
			m_vecPylonLinkDamageEffectData.push_back( PylonLinkDamageEffectData( pEffect->GetHandle(), std::make_pair( hAddPylonHandle, AnotherPylonData.m_EffectHandle ) ) );
		}
	}
}

/** @function	: DeletePylonLinkDamageEffect
	@brief		: ���Ϸ��� ���� ������ ����Ʈ ���� �Լ�
	@param		: ������ ���Ϸ� ������ ����Ʈ �ڵ�
*/
void CX2GUAdd::DeletePylonLinkDamageEffect( IN CX2DamageEffect::CEffectHandle hDeletePylonHandle_ )
{
	/// ���� ������ ���� ������ ����Ʈ �˻�
	vector<PylonLinkDamageEffectData>::const_iterator vItLinkData = m_vecPylonLinkDamageEffectData.begin();

	while( vItLinkData != m_vecPylonLinkDamageEffectData.end() )
	{
		PylonLinkDamageEffectData LinkData = *vItLinkData;

		/// ������ ���Ϸ��� ����� ������ ����Ʈ�� �ִٸ�, ����
		if ( hDeletePylonHandle_ == LinkData.m_hPylonHandle.first || hDeletePylonHandle_ == LinkData.m_hPylonHandle.second )
		{
			if ( NULL != g_pX2Game &&
				 NULL != g_pX2Game->GetDamageEffect() )
			{
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( LinkData.m_hLinkEffectHandle );
			}

			/// ����� ���� ���� ����
			vItLinkData = m_vecPylonLinkDamageEffectData.erase( vItLinkData );
		}
		else
			++vItLinkData;
	}
}

/** @function	: ProcessPylonDamageEffect
	@brief		: ���Ϸ� ���� �ð� ���� �Լ�
*/
void CX2GUAdd::ProcessPylonDamageEffect()
{
	vector<PylonDamageEffectData>::iterator vItSummonPylon = m_vecSummonPylonDamageEffectData.begin();

	while( vItSummonPylon != m_vecSummonPylonDamageEffectData.end() )
	{
		PylonDamageEffectData& PylonData = *vItSummonPylon;

		float& fRemainTime = PylonData.m_fRemainTime;

		fRemainTime -= m_fElapsedTime;

		if ( 0.f >= fRemainTime && NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			/// ��ũ�� ������ ����Ʈ ����
			DeletePylonLinkDamageEffect( PylonData.m_EffectHandle );

			/// ���Ϸ� �� ������ ������ ����Ʈ ����
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( PylonData.m_EffectHandle );
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( PylonData.m_BuffEffectLHandle );

			vItSummonPylon = m_vecSummonPylonDamageEffectData.erase( vItSummonPylon );
		}
		else
			++vItSummonPylon;
	}
}

/** @function	: GetSummonPylonPos
	@brief		: ���Ϸ� ��ȯ ������ ��ġ ��ȯ �Լ�
	@param		: ��ġ��
	@return		: ���� ����
*/
bool CX2GUAdd::GetSummonPylonPos( OUT D3DXVECTOR3& fSummonPos )
{
	if ( NULL != g_pX2Game &&
		 NULL != g_pX2Game->GetWorld() &&
		 NULL != g_pX2Game->GetWorld()->GetLineMap() &&
		 NULL != g_pX2Game->GetDamageEffect() )

	{
		/// �˻��� ��ȯ ��ġ Offset
		float fCheckOffsetPosition[3] = { 100.f, 0.f, -100.f };

		for ( int i = 0 ; i < std::ssize( fCheckOffsetPosition ) ; ++i )
		{
			bool bPossibleSummon = true;		/// ��ȯ ���� ����

			/// ��ȯ ��ġ Offset ����
			D3DXVECTOR3 fMovePosition = GetIsRight() ? GetPos() + ( GetDirVector() * fCheckOffsetPosition[i] ) :
													   GetPos() - ( GetDirVector() * fCheckOffsetPosition[i] );

			/// ���� ��ġ�� ���Ϸ����� ������ ��ġ�� �ʴ��� �˻�
			for( PylonDamageEffectData PylonData : m_vecSummonPylonDamageEffectData )
			{
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( PylonData.m_EffectHandle );

				if ( NULL != pEffect && 250.f >= GetDistance( fMovePosition, pEffect->GetPos() ) )
				{
					bPossibleSummon = false;	/// ��ġ�� ������ �־��!
					break;
				}
			}

			/// �ٸ� ���Ϸ���� ��ġ�� ������ ���ٸ�, �ش� ��ġ ��ȯ
			if ( true == bPossibleSummon )
			{
				fSummonPos = fMovePosition;
				return true;
			}
		}
	}

	return false;
}

/** @function	: ResetPylonData
	@brief		: ���Ϸ� ���� ��� ���� �� ����Ʈ �ʱ�ȭ
*/
void CX2GUAdd::ResetPylonData()
{
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		for( PylonDamageEffectData PylonData : m_vecSummonPylonDamageEffectData )
		{
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( PylonData.m_EffectHandle );
		}

		for( PylonLinkDamageEffectData LinkData : m_vecPylonLinkDamageEffectData )
		{
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( LinkData.m_hLinkEffectHandle);
		}

		m_vecSummonPylonDamageEffectData.resize( 0 );
		m_vecPylonLinkDamageEffectData.resize( 0 );
	}
}

/** @function	: ProcessNonstopSummonPylon
	@brief		: ���Ϸ� ���� ��ȯ ���� ����
*/
void CX2GUAdd::ProcessNonstopSummonPylon()
{
	if ( 0.f < m_fNonstopSummonPylonCoolTime )
	{
		m_fNonstopSummonPylonCoolTime -= m_fElapsedTime;

		/// ��Ÿ�� �����, ��Ÿ�� �� ���� ��ȯ ī��Ʈ �ʱ�ȭ
		if ( 0.f >= m_fNonstopSummonPylonCoolTime )
		{
			m_fNonstopSummonPylonCoolTime	= 0.f;
			m_uiNonstopSummonPylonCount		= 0;
		}
	}
}

/** @function	: GetPossibleNonstopSummonPylon
	@brief		: ���Ϸ� ���� ��ȯ ���� ����
*/
bool CX2GUAdd::GetPossibleNonstopSummonPylon() const
{
	/// ���� ��ȯ ���Ѽ��� ���� �ʰ�, ���� ��ȯ ��Ÿ���� �����ִٸ� ��ȯ ����
	if ( 1 <= m_uiNonstopSummonPylonCount &&
		_CONST_ADD_::PYLON_NONESTOP_SUMMON_LIMIT_NUM > m_uiNonstopSummonPylonCount &&
		0.f < m_fNonstopSummonPylonCoolTime )
		return true;

	return false;
}

/** @function	: SetNasodArmorAttackTarget
	@brief		: ���ҵ� �Ƹ� ���� Ÿ���� ���� �Լ�
	@param		: ������ ������
*/
void CX2GUAdd::SetNasodArmorAttackTarget( IN const CX2DamageManager::DamageData& pDamageData_ )
{
	if( NULL != g_pX2Game &&
		0.f  >= pDamageData_.m_fHpPercentUp &&
		null != pDamageData_.optrDefenderGameUnit &&
		CX2DamageManager::AT_UNIT == pDamageData_.defenderType )
	{
		/// ȯ�� ������ �нú�� ���Ͽ�, ���ҵ� �Ƹ� ���� Ÿ���� ���� ���� ����

		NasodArmorAttackTargetData	NAAttackTargetData		= NasodArmorAttackTargetData();	/// �� Ÿ�� ��ü
		bool						bIsSuccessSetTargetData	= false;	/// Ÿ�� ���� ��� ����

		/// Ÿ�� ���� ����
		if( CX2GameUnit::GUT_USER == pDamageData_.optrDefenderGameUnit->GetGameUnitType() )
		{
			NAAttackTargetData.SetTargetData( pDamageData_.optrDefenderGameUnit->GetUnitUID(), false );

			bIsSuccessSetTargetData = true;		/// ��� ����
		}
		else if( CX2GameUnit::GUT_NPC == pDamageData_.optrDefenderGameUnit->GetGameUnitType() )
		{
			CX2GUNPC *pNpc = static_cast<CX2GUNPC *>( pDamageData_.optrDefenderGameUnit.GetObservable() );

			if( pNpc != NULL && CX2UnitManager::NCT_BASIC == pNpc->GetNPCTemplet().m_ClassType )
			{
				NAAttackTargetData.SetTargetData( pDamageData_.optrDefenderGameUnit->GetUnitUID(), true );
				bIsSuccessSetTargetData = true;	/// ��� ����
			}
		}

		/// Ÿ�� ���� ��� ���� �ߴٸ�, �н�
		if ( false == bIsSuccessSetTargetData )
			return;

		const CX2SkillTree::SKILL_ID eSkillID = pDamageData_.m_eUsedSkillID;	/// ��ų ���̵�

		switch ( eSkillID )
		{
		//////////////////////////////////////////////////////////////////////////
		/// Ư�� ��ų�� ���� Ÿ�� ���� ����
		case CX2SkillTree::SI_A_AN_PULSE_BULLET:	/// ����ź
		case CX2SkillTree::SI_A_AN_PHASE_SHIFT:		/// ���� ��ȭ
			{
				/// ���� ��� ���϶��� ����
				if ( true == GetIsFormationMode() )
				{
					vector<FantasyTracerTargetInfo>::iterator vIt = m_vecFantasyTracerTargetInfo.begin();

					/// ���� ���� ID�� Ÿ�� ������ ����ִٸ�, ����������
					for ( ; vIt != m_vecFantasyTracerTargetInfo.end(); ++vIt )
					{
						if ( vIt->m_eSkillID == eSkillID )
						{
							m_vecFantasyTracerTargetInfo.erase( vIt );
							break;	/// �������� Ż��
						}
					}

					/// Ÿ�� ���� �ð� ���� ( ���� ���� �ð� + ���� ��� �ð� )
					const float fRemainTime = _CONST_ADD_::FANTASY_TRACER_TARGET_REMAIN_TIME + _CONST_ADD_::FANTASY_TRACER_TARGET_DELETE_READY_TIME;

					/// ȯ�� ������ Ÿ�� ���� ����
					FantasyTracerTargetInfo FTTargetInfo = FantasyTracerTargetInfo( fRemainTime, NAAttackTargetData, eSkillID );

					/// ���� ����
					m_vecFantasyTracerTargetInfo.push_back( FantasyTracerTargetInfo( FTTargetInfo ) );

					/// ������ ����
					if ( NULL != g_pX2Game )
						g_pX2Game->UpdateSkillSlotUI();
				}
			} break;
		//////////////////////////////////////////////////////////////////////////
		/// �� �� ( �Ϲ� Ÿ�� ���� ���� )
		default:
			{
				const D3DXVECTOR3 vMyPos		= GetPos();
				const D3DXVECTOR3 vTargetPos	= pDamageData_.optrDefenderGameUnit->GetPos();

				/// Ÿ���� ���� �˻�
				if ( _CONST_ADD_::NASOD_ARMOR_TARGET_RANGE < GetDistance( vMyPos, vTargetPos ) )
					return;

				/// ������ ���, �߰��� ���� ���� �˻�
				if ( CX2Game::GT_PVP == g_pX2Game->GetGameType() )
				{
					const D3DXVECTOR3 vMyBonePos		= GetBonePos( L"Bip01" );
					const D3DXVECTOR3 vTargetBonePos	= pDamageData_.optrDefenderGameUnit->GetBonePos( L"Bip01" );

					if ( _CONST_ADD_::NASOD_ARMOR_TARGET_HEIGHT_RANGE < abs( vMyBonePos.y - vTargetBonePos.y ) )
						return;
				}

				/// ���� Ÿ�� ������ ����
				m_NasodArmorAttackTargetData = NAAttackTargetData;
			}
		}


	}
}

/** @function	: ProcessMagnetronDamageEffect
	@brief		: ���׳�Ʈ�� ��ġ ���� �Լ�
*/
void CX2GUAdd::ProcessMagnetronDamageEffect()
{
	vector<CX2DamageEffect::CEffectHandle>& vecMagnetronDamageEffectData = m_MagnetronData.m_vecMagnetronDamageEffectData;

	if ( true == vecMagnetronDamageEffectData.empty() ||
		 NULL == g_pX2Game ||
		 NULL == g_pX2Game->GetDamageEffect() )
		return;

	vector<CX2DamageEffect::CEffectHandle>::iterator vIt = vecMagnetronDamageEffectData.begin();

	while( vIt != vecMagnetronDamageEffectData.end() )
	{
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( *vIt );

		/// ���� �� Ư�� ��ġ�� �̵�
		if ( NULL != pEffect )
		{
			pEffect->SetPos( GetBonePos( L"Bip01" ) );
			++vIt;
		}
		else
			vIt = vecMagnetronDamageEffectData.erase( vIt );
	}

	/// ���׳�Ʈ�� �߻� ���� ����
	float& fFireReadyTime = m_MagnetronData.m_fFireReadyTime;

	if ( 0.f < fFireReadyTime )
	{
		fFireReadyTime -= m_fElapsedTime;

		if ( 0.f >= fFireReadyTime )
			fFireReadyTime = 0.f;
	}
}

/** @function	: SetMagnetronDamageEffectTarget
	@brief		: ���̳ʸ� ���� - ���׳�Ʈ�� Ÿ�� ����
	@param		: ������ ������
*/
void CX2GUAdd::AttackMagnetronDamageEffect( IN const CX2DamageManager::DamageData& pDamageData_ )
{
	if( 0.f < m_MagnetronData.m_fFireReadyTime ||
		true == m_MagnetronData.m_vecMagnetronDamageEffectData.empty() ||
		NULL == g_pX2Game ||
		NULL == g_pX2Game->GetDamageEffect() ||
		null == pDamageData_.optrDefenderGameUnit ||
		CX2DamageManager::AT_UNIT != pDamageData_.defenderType ||
		CX2DamageManager::AT_SPECIAL == pDamageData_.attackType||
		CX2DamageManager::RT_DUMMY_DAMAGE == pDamageData_.reActResult )
		return;

	/// ���׳�Ʈ�� ���� ����Ʈ���, ���� ���� ��Ű��
	if ( INVALID_DAMAGE_EFFECT_HANDLE != pDamageData_.hAttackerEffect )
	{
		CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( pDamageData_.hAttackerEffect );

		if ( NULL != pEffect )
		{
			const wstring wstrDamageEffectName = pEffect->GetName();

			if ( wstrDamageEffectName == L"DamageEffect_APT_Magnetron_Dynamo_Explosion" )
				return;
		}
	}

	/// ����Ǿ� �ִ� ���׳�Ʈ�� ����Ʈ �� ���� ù��° ����
	vector<CX2DamageEffect::CEffectHandle>::iterator vIt = m_MagnetronData.m_vecMagnetronDamageEffectData.begin();

	CX2DamageEffect::CEffectHandle hEffectHandle = *vIt;

	CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( hEffectHandle );

	if( pEffect == NULL )
		return;

	/// �ش� ������ ����Ʈ�� Main Mesh ��ȯ
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pEffect->GetMainEffect();

	if( pMeshInst == NULL )
		return;

	/// �߻� ��ġ ��ȯ
	const D3DXVECTOR3 vFirePos = pMeshInst->GetBonePos( L"Dummy02" );

	/// �߻� ����Ʈ ����
	CX2DamageEffect::CEffect* pFireEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"DamageEffect_SA_APT_Magnetron_Dynamo_Projectile", GetPowerRate(), vFirePos,
		GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

	/// Ÿ���� ���� ����
	if ( NULL != pFireEffect )
	{
		CX2DamageEffect::LockOnData& FireLockOnData = pFireEffect->GetLockOnData();

		if( CX2GameUnit::GUT_USER == pDamageData_.optrDefenderGameUnit->GetGameUnitType() )
			FireLockOnData.m_LockOnUnitUID = pDamageData_.optrDefenderGameUnit->GetUnitUID();
		else if( CX2GameUnit::GUT_NPC == pDamageData_.optrDefenderGameUnit->GetGameUnitType() )
			FireLockOnData.m_LockOnNPCUID = static_cast<int>( pDamageData_.optrDefenderGameUnit->GetUnitUID() );
	}

	/// ����Ʈ ���� ( �ܻ� ���� �ٷ�! )
	g_pX2Game->GetDamageEffect()->DestroyInstanceHandleSilently( hEffectHandle );

	m_MagnetronData.m_vecMagnetronDamageEffectData.erase( vIt );

	/// �߻� ���� ����
	m_MagnetronData.m_fFireReadyTime = _CONST_ADD_::MAGNETRON_FIRE_READY_TIME;
}

/** @function	: SetStasisfieldHitValue
	@brief		: �����̽ý� �ʵ� Ÿ��ġ ���� �Լ�
	@param		: Ÿ��ġ
*/
/*virtual*/ void CX2GUAdd::SetStasisfieldHitValue( IN const float fHitValue_ )
{
	if ( true == m_StasisfieldData.GetIsActiveStasisfield() )
		m_StasisfieldData.m_fHitValue += fHitValue_;
}

/** @function	: SetStasisfieldDamageValue
	@brief		: �����̽ý� �ʵ� �ǰ�ġ ���� �Լ�
	@param		: �ǰ�ġ
*/
/*virtual*/ void CX2GUAdd::SetStasisfieldDamageValue( IN const float fDamageValue_ )
{
	if ( true == m_StasisfieldData.GetIsActiveStasisfield() )
		m_StasisfieldData.m_fDamageValue += fDamageValue_;
}

/** @function	: ProcessStasisField
	@brief		: �����̽ý� �ʵ� �� ������ ó�� �Լ�
*/
void CX2GUAdd::ProcessStasisField()
{
	if ( 0.f < m_StasisfieldData.m_fRemainTime )
	{
		m_StasisfieldData.m_fRemainTime -= m_fElapsedTime;

		/// 5�� ���Ϸ� ���Ҵµ� ī��Ʈ �ٿ� ����Ʈ�� ������ �� ������, ����Ʈ ����
		if ( 5.f >= m_StasisfieldData.m_fRemainTime &&
			INVALID_EFFECTSET_HANDLE == m_StasisfieldData.m_hCountEffectSet )
		{
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
			{
				CX2EffectSet::Handle hCountEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_APT_STASIS_FIELD_Count_Down", this,
					NULL, false, -1.f, _CONST_ADD_::STASIS_FIELD_REMAIN_TIME );

				m_StasisfieldData.m_hCountEffectSet = hCountEffectSet;
			}
		}

		/// �����̽ý� �ʵ� ����
		if ( 0.f >=  m_StasisfieldData.m_fRemainTime )
			EndStasisFieldByTime();
	}
}
/** @function : CreateDustStormToAttackResult
	@brief : #1. Ÿ�ݿ� ���� �̸��� ����Ʈ ����(�̸��� ��ǳ)
			 #2. AttackResult �Լ����� ���� ��
			 #3. �������� : AT_SPECIAL �ƴϰ�, ������ damagedata�� ���ݷ��� 0�� �ƴ� �� Ȯ���� ���� ����
*/
void CX2GUAdd::CreateDustStormToAttackResult( const CX2DamageManager::DamageData& damageData_ )
{
	if( CX2DamageManager::AT_SPECIAL != damageData_.attackType && 0.f >= m_fDustStormCoolTime )
	{
		float fRandVal = GetRandomFloat();;
		if( ( m_fDustStormRateByHit  * damageData_.m_fRateModifier) > fRandVal )
		{
			// # �̸��ڸ� ������ �������� ���ݷ� ���� ���
			float fDamageRate = 0.f;
			switch ( damageData_.damageType )
			{
			case CX2DamageManager::DT_PHYSIC:
				fDamageRate = damageData_.damage.fPhysic;
				break;
			case CX2DamageManager::DT_MAGIC:
				fDamageRate = damageData_.damage.fMagic;
				break;
			case CX2DamageManager::DT_MIX:
				fDamageRate = ( damageData_.damage.fPhysic + damageData_.damage.fPhysic ) / 2.f;
				break;
			default:
				fDamageRate = 1.f;
				break;
			}

			CreateDustStormDamageEffect(fDamageRate, damageData_.impactPoint );
		}
	}
}
/** @function : CreateDustStormToDamageReact
	@brief : �ǰݿ� ���� �̸��� ����Ʈ ����(�̸��� ��ǳ)
	@brief : #1. �ǰݿ� ���� �̸��� ����Ʈ ����(�̸��� ��ǳ)
			 #2. DamageReact �Լ����� ���� ��
			 #3. �������� : AT_SPECIAL �ƴϰ�, ������ damagedata�� ���ݷ��� 0�� �ƴ� �� Ȯ���� ���� ����
*/
void CX2GUAdd::CreateDustStormToDamageReact( const CX2DamageManager::DamageData& damageData_ )
{
	if( CX2DamageManager::AT_SPECIAL != damageData_.attackType && 0.f >= m_fDustStormCoolTime )
	{
		float fRandVal = GetRandomFloat();;
		if( ( m_fDustStormRateByDamage * damageData_.m_fRateModifier) > fRandVal )
		{
			// # �̸��ڸ� ������ �������� ���ݷ� ���� ���
			float fDamageRate = 0.f;
			switch ( damageData_.damageType )
			{
			case CX2DamageManager::DT_PHYSIC:
				fDamageRate = damageData_.damage.fPhysic;
				break;
			case CX2DamageManager::DT_MAGIC:
				fDamageRate = damageData_.damage.fMagic;
				break;
			case CX2DamageManager::DT_MIX:
				fDamageRate = ( damageData_.damage.fPhysic + damageData_.damage.fPhysic ) / 2.f;
				break;
			default:
				fDamageRate = 1.f;
				break;
			}

			CreateDustStormDamageEffect(fDamageRate, damageData_.impactPoint );
		}
	}
}

/** @function	: CreateDustStormDamageEffect
	@brief		: �̸��� ��ǳ ������ ����Ʈ ���� �Լ�
	@param		: ������ ����, ���� ��ġ
*/
void CX2GUAdd::CreateDustStormDamageEffect( const float fDamageRate_, const D3DXVECTOR3& vPos_ )
{
	if( false == IsSamef( fDamageRate_, 0.f ))
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ADD_DUST_STORM", fDamageRate_,
			vPos_, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		m_fDustStormCoolTime = _CONST_ADD_::DUST_STORM_FIRE_COOL_TIME;	/// ��Ÿ�� ����
	}
}

/** @function	: ProcessDustStormCoolTime
	@brief		: �̸��� ��ǳ ��Ÿ�� ���� �Լ�
*/
void CX2GUAdd::ProcessDustStormCoolTime()
{
	if ( 0.f < m_fDustStormCoolTime )
	{
		m_fDustStormCoolTime -= m_fElapsedTime;

		if ( 0.f >= m_fDustStormCoolTime )
			m_fDustStormCoolTime = 0.f;
	}
}

/** @function	: EndStasisFieldByTime
	@brief		: ���� �ð��� ������, �����̽ý� �ʵ� ���� �Ǿ��� �� ���� ó�� �Լ�
*/
void CX2GUAdd::EndStasisFieldByTime()
{
	if ( NULL != g_pData && g_pData->GetSkillTree() && NULL != GetUnit() )
	{
		CX2UserSkillTree& userSkillTree = GetUnit()->AccessUnitData().m_UserSkillTree;
		CX2SkillTree* const pSkillTree = g_pData->GetSkillTree();

		const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_SA_APT_STASIS_FIELD;

		const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
		if ( 0 < iSkillLevel )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
			if( NULL != pSkillTemplet )
			{
				/// �ǰ� �� Ÿ��ġ �ջ�
				const float fTotalValue = m_StasisfieldData.m_fHitValue + m_StasisfieldData.m_fDamageValue;
				/// ���� MP
				const float fMPConsume	= GetActualMPConsume( pSkillTemplet->m_eID, iSkillLevel );
				/// MP ȸ���� = ���� MP * ( 0.3333 + min( ���� ������/�ڽ��� �ִ�HP , 0.3333) )
				const float fIncreaseMPValue = fMPConsume * ( 0.3333f + min( fTotalValue / GetMaxHp(), 0.3333f ) );

				/// MP ȸ��
				UpNowMp( fIncreaseMPValue );

				/// MP ȸ�� ��ġ ǥ��
				if ( NULL != g_pX2Game )
					g_pX2Game->DisplayAddMPInformation( fIncreaseMPValue, GetPos(), GetDirVector() );
			}
		}
	}

	/// ���� ���
	PlaySound( L"AddVoice_Stasis_Field03.ogg" );
	PlaySound( L"Add_StasisField2.ogg" );

	/// �����̽ý� �ʵ� �ʱ�ȭ
	m_StasisfieldData.ResetStasisfieldData();

	/// ��ų ������ ����
	if ( IsMyUnit() )
		g_pX2Game->UpdateSkillSlotUI();
}

/** @function	: ResetMagnetronData
	@brief		: ���׳�Ʈ�� ���� �ʱ�ȭ �Լ�
*/
void CX2GUAdd::MagnetronData::ResetMagnetronData()
{
	m_fFireReadyTime = 0.f;

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		for( CX2DamageEffect::CEffectHandle hMagnetronHandle : m_vecMagnetronDamageEffectData )
		{
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( hMagnetronHandle );
		}
	}
}

/** @function	: ResetStasisfieldData
	@brief		: �����̽ý� �ʵ� ���� �ʱ�ȭ �Լ�
*/
void CX2GUAdd::StasisfieldData::ResetStasisfieldData()
{
	m_fRemainTime			= 0.f;
	m_fHitValue				= 0.f;
	m_fDamageValue			= 0.f;

	if ( NULL != g_pX2Game &&
		 NULL != g_pX2Game->GetEffectSet() )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hLoopEffectSet );
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hCountEffectSet );
	}
}

//////////////////////////////////////////////////////////////////////////
// ����ȭ ���� ������ �Լ�
CX2GUAdd::PhaseShiftData::PhaseShiftData()
{
	m_fEffectLifeTime		= 0.f;
	m_fReturnMPConsume		= 0.f;
	m_vUserReturnPos		= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_hPhasShiftDamageEffect= INVALID_DAMAGE_EFFECT_HANDLE;
}
void CX2GUAdd::PhaseShiftData::InitPhaseEffectData()
{
	m_hPhasShiftDamageEffect = INVALID_DAMAGE_EFFECT_HANDLE;
}
void CX2GUAdd::PhaseShiftData::InitResturnPosValue()
{
	m_vUserReturnPos = D3DXVECTOR3(0.f,0.f,0.f);
}
/** @function : IsValidReturnPosValue
	@brief : ������ ������ ��ġ���� ��ȿ���� �˻�
*/
bool CX2GUAdd::PhaseShiftData::IsValidReturnPosValue() const
{
	if( INVALID_DAMAGE_EFFECT_HANDLE != m_hPhasShiftDamageEffect )
	{
		const float fPosValue = m_vUserReturnPos.x + m_vUserReturnPos.y + m_vUserReturnPos.z;
		return !IsSamef(fPosValue,0.f);
	}
	else
		return false;
}
/** @function : UpdateSkillSlotUI
	@brief : ����ȭ ��ų ���� ������ ���� ���� ����.
			 (ȣ�� ��ġ�� �ľ��ϱ� ������ �Լ� �߰� ��)
*/
void CX2GUAdd::PhaseShiftData::UpdateSkillSlotUI()
{
// ��ų ������ ���� ����.
//#case1 ��ġ ����Ʈ ��ġ ���� ��
//#case2 ��ġ ����Ʈ�� ���ӽð��� ���� ���ŵǾ��� ��
//#case3 ������ ��ġ ����Ʈ ��ġ�� �̵� ���� ��

	g_pX2Game->UpdateSkillSlotUI();
}
/** @function : DieDamageEffectProc
	@brief : ������ ����Ʈ�κ��� �ҷ����� Callback �Լ�
			 ������ ����Ʈ�� ������ ���� ��ü���Ը� �ҷ� ��
*/
void CX2GUAdd::DieDamageEffectProc( const CX2DamageManager::DamageData& pDamageData_ )
{
	switch( pDamageData_.m_eUsedSkillID )
	{
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:
		{ // ����ȭ ��� ����Ʈ ��� �� ��Ÿ�� ó��
			if( true == pDamageData_.m_bNoDamage )
			{
				// �����߿� �ִ� ����Ʈ�� ���� ���� ó��
				if( INVALID_DAMAGE_EFFECT_HANDLE != m_PhaseShiftData.m_hPhasShiftDamageEffect )
				{
					m_PhaseShiftData.InitResturnPosValue();
					m_PhaseShiftData.InitPhaseEffectData();
					if ( IsMyUnit() )
						m_PhaseShiftData.UpdateSkillSlotUI();
				}
			}
		} break;
	default:
		break;
	}
}
void CX2GUAdd::PlayLoopSound( const WCHAR* pSoundFileName_, bool /*bLoop_*/, bool /*b3D_*/ )
{
	if( NULL != g_pKTDXApp &&
		NULL != g_pKTDXApp->GetDeviceManager() )
	{
		// TODO: ���� ���� ó�� �� �ٸ� ����� ã�� ���� �� �� ��.
		//		 ����� ���� ���尡 ��ø �� �� ���ٰ� �����ϰ� �۾� ��.
		StopLoopSound();

		m_pLoopSound =
			g_pKTDXApp->GetDeviceManager()->PlaySound( pSoundFileName_, false, false );
	}
}

/** @function : StopLoopSound
	@brief : �Ϻ� Loop ������Ʈ�� ���带 ���� ��Ű�� �Լ�.
			 �ش� ������Ʈ�� ���������, �ǰ�, ���� �Ҹ� �� ó���Ѵ�.
*/
void CX2GUAdd::StopLoopSound()
{
	if( NULL != m_pLoopSound )
	{
		m_pLoopSound->Stop();
		m_pLoopSound = NULL;
	}
}

/** @function : IsValideSkillRidingOn
	@brief : Ż �� ž���߿� ��� �� �� �ִ� ��ų���� üũ.
	@return : ��� ���� : true ��� �Ұ� falle
*/
/*virtual*/ bool CX2GUAdd::IsValideSkillRidingOn( CX2SkillTree::SKILL_ID eSkill_ID_ )
{
	if( true == GetRidingOn() )
	{
		switch( eSkill_ID_ )
		{
		case CX2SkillTree::SI_SA_APT_STASIS_FIELD: // �����̽ý� �ʵ�
			{
				return false;
			} break;
		default:
			return true;
			break;
		}
	}
	else
	{
		return true;
	}
}


/*virtual*/ void CX2GUAdd::ChargeMpAndEtcInTrainingGame()
{
	CX2GUUser::ChargeMpAndEtcInTrainingGame();

	CX2AddGageData* pAddGageData = static_cast<CX2AddGageData*>( GetGageData() );
	if( NULL != pAddGageData )
	{
		SetDPValue( pAddGageData->GetMaxDPValue() );
	}
}

void CX2GUAdd::RidingWaitEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( CheckElapsedTimeRidingWaitHabit() &&
		NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT_HABIT );
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_RIDING_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE

	CommonEventProcess();
}

void CX2GUAdd::RidingWalkEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
		TRACE( L"Walk RidingPetSpecialAttackEventProcess" );
	}
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		TRACE( L"Walk m_InputData.pureDoubleRight m_InputData.pureDoubleLeft" );
		StateChangeDashIfPossible();
	}
	else if ( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		TRACE( L"Walk m_InputData.pureRight m_InputData.pureLeft" );
		StateChange( USI_RIDING_WAIT );

		TRACE( L"RidingWalkStart Wait\n" );
	}
	else if ( true == m_InputData.pureUp )
	{
		TRACE( L"Walk m_InputData.pureUp" );
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_RIDING_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE

	CommonEventProcess();
}

void CX2GUAdd::RidingJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( USI_RIDING_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_RIDING_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE

	CommonEventProcess();
}

void CX2GUAdd::RidingDashEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if ( NULL != m_ptrRidingPet && GetAirDashCount() < 1 )
		{
			if ( CX2RidingPet::RSM_AIR_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove ||
				CX2RidingPet::RSM_PASS_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove )
				StateChange( USI_RIDING_SPECIAL_MOVE );
		}
		else
		{
			StateChange( USI_RIDING_DASH_JUMP );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.oneUp )
	{
		StateChange( USI_RIDING_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	else if( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_RIDING_DASH_END );
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	if ( NULL != m_ptrRidingPet && CX2RidingPet::RSM_BOOST_DASH == m_ptrRidingPet->GetInfo().m_eSpecialMove &&
		1.0f <= m_FrameDataFuture.unitCondition.fStateTime )
	{
		StateChange( USI_RIDING_SPECIAL_MOVE );
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_RIDING_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE

	CommonEventProcess();
}

void CX2GUAdd::RidingDashEndEventProcess()
{

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( false == GetRidingOn() )
	{
		StateChange( USI_RIDING_OFF );
	}
	else if ( true == RidingPetSpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_RIDING_WALK );
	}
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_RIDING_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_RIDING_JUMP_UP );
	}
	else if( NULL != m_ptrRidingPet && NULL != m_ptrRidingPet->GetXSkinAnimFuturePtr() && m_ptrRidingPet->GetXSkinAnimFuturePtr()->IsAnimationEnd() )
	{
		StateChange( USI_RIDING_WAIT );
	}
	ELSE_IF_CAN_CHANGE_FORMATION_MODE( USI_RIDING_HYPER_MODE )
	ELSE_IF_CAN_RELEASE_FORMATION_MODE

	CommonEventProcess();
}

void CX2GUAdd::InitPhaseShift()
{
	if( INVALID_DAMAGE_EFFECT_HANDLE != m_PhaseShiftData.m_hPhasShiftDamageEffect )
	{
		CX2DamageEffect::CEffect* pEffect =
			g_pX2Game->GetDamageEffect()->GetInstance( m_PhaseShiftData.m_hPhasShiftDamageEffect );
		if( NULL != pEffect )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMesh = pEffect->GetMainEffect();
			if( NULL != pMesh )
				pMesh->SetMaxLifeTime(0.1f);

			pEffect->GetDamageData().m_eUsedSkillID = CX2SkillTree::SI_NONE;
			m_PhaseShiftData.InitPhaseEffectData();
		}
	}

	m_PhaseShiftData.InitPhaseEffectData();
	m_PhaseShiftData.InitResturnPosValue();
	m_PhaseShiftData.UpdateSkillSlotUI();
}

/** @function	: GetTargetUnit
	@brief		: ���ҵ� �Ƹӿ� Ÿ���� �� ���� ��ȯ
	@return		: Ÿ���� ���� ��ü
*/
const CX2GameUnit* CX2GUAdd::NasodArmorAttackTargetData::GetTargetUnit()
{
	const CX2GameUnit* pGameUnit = NULL;

	if ( 0 != m_uidTargetUID )
	{
		if ( true == m_bIsNPC )	/// NPC�� ��
		{
			CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( static_cast<int>( m_uidTargetUID ) );

			if ( NULL != pNPC )
				pGameUnit = static_cast<CX2GameUnit*>( pNPC );		/// GameUnit ��ü ��ȯ
		}
		else					/// ������ ��
		{
			CX2GUUser* pUser = g_pX2Game->GetUserUnitByUID( m_uidTargetUID );

			if ( NULL != pUser )
				pGameUnit = static_cast<CX2GameUnit*>( pUser );		/// GameUnit ��ü ��ȯ
		}
	}

	return pGameUnit;
}


/** @function	: ProcessBodyOfTranscendence
	@brief		: �ʿ����� ��ü ���� �˻� �� ���� �Լ�
	@param		: ������ ������
*/
void CX2GUAdd::ProcessBodyOfTranscendence( IN OUT CX2DamageManager::DamageData* pDamageData_ )
{
	/// NULL �����͸� �н�
	if ( NULL == pDamageData_ )
		return;

	/// ���ҵ� �Ƹ� �޺��� �ƴϸ�, �н�
	if ( CX2DamageManager::DPRT_NASOD_ARMOR != pDamageData_->m_DPRateType )
		return;

	/// �ߵ� Ȯ�� �˻� ( �нú긦 ���� �ʾҴٸ�, Ȯ���� 0% )
	if ( m_fBodyOfTranscendenceActiveRate >= RandomFloat() )
	{
		pDamageData_->m_bIgnoreDefence		= true;	/// ���� ����
		pDamageData_->m_bIgnoreStateDefence	= true;	/// ���� ����
	}
}

/** @function	: ProcessPowerExchanger
	@brief		: ��� ��ȯ�� ���� ���� �Լ�
*/
void CX2GUAdd::ProcessPowerExchanger()
{
	/// ��� ��ȯ�� ��������� �н�
	if ( false == m_bEnablePowerExchanger )
		return;

	/// ���� DP���� ���� ��� ��ȯ�� ������Ʈ ( ���� DP�� 50% �̻��ΰ�? �̸��ΰ�? )
	const POWER_EXCHANGER_STATE eNowPowerExchangerState = ( GetMaxDPValue() * 0.5f <=  GetDPValue() ) ? PES_HALF_UP : PES_HALF_DOWN;

	/// ���� �����Ǿ� �ִ� ��� ��ȯ�� ������Ʈ�� �ٸ� ��
	if ( m_ePowerExchangerState != eNowPowerExchangerState )
	{
		/// ��� ��ȯ�� ������Ʈ ����
		m_ePowerExchangerState = eNowPowerExchangerState;


		if ( NULL != GetUnit() )
		{
			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALP_POWER_EXCHANGER );

			if ( PES_HALF_UP == eNowPowerExchangerState )		/// 50% �̻��� ��
			{
				/// ��ü������ �������� BFI_BUFF_POWER_EXCHANGER_HALF_DOWN�� ����� �����
				SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_POWER_EXCHANGER_HALF_UP, iSkillLevel );
			}
			else		/// 50% �̸��� ��
			{
				/// ��ü������ �������� BFI_BUFF_POWER_EXCHANGER_HALF_UP�� ����� �����
				SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_POWER_EXCHANGER_HALF_DOWN, iSkillLevel );
			}
		}
	}
}

/** @function	: GetIsFantasyTracerTargetSkillID
	@brief		: �ش� ��ų�� Ÿ�� ������ �ִ��� �˻�
	@param		: ��ų ���̵�
	@return		: Ÿ�� ���� ���� ����
*/
bool CX2GUAdd::GetIsFantasyTracerTargetSkillID( IN const CX2SkillTree::SKILL_ID eSkillID_ ) const
{
	/// ȯ�� ������ ����ÿ��� �˻�
	if ( true == GetIsFormationMode() && true == m_bEnableFantasyTracer )
	{
		for( FantasyTracerTargetInfo TargetInfo : m_vecFantasyTracerTargetInfo )
		{
			if ( eSkillID_ == TargetInfo.m_eSkillID && false == TargetInfo.m_bIsUseData )
				return true;
		}
	}

	return false;
}

/** @function	: SetUseFantasyTracerTarget
	@brief		: �ش� ��ų ���̵��� Ÿ�� ���� ��� ó�� ����
	@param		: ��ų ���̵�
*/
void CX2GUAdd::SetUseFantasyTracerTarget( IN const CX2SkillTree::SKILL_ID eSkillID_ )
{
	for( FantasyTracerTargetInfo& TargetInfo : m_vecFantasyTracerTargetInfo )
	{
		if ( eSkillID_ == TargetInfo.m_eSkillID )
		{
			TargetInfo.m_bIsUseData				= true;						/// ��� ó��
			m_eNasodArmorAttackActiveSkillID	= eSkillID_;				/// ���ݽ� ����� ��ų ���̵� ����
			m_eNasodArmorLandingActiveSkillID	= CX2SkillTree::SI_NONE;	/// ������ ����� ��ų ���̵� �ʱ�ȭ

			/// ��ų ������ ����
			if ( NULL != g_pX2Game )
				g_pX2Game->UpdateSkillSlotUI();
		}
	}
}

/** @function	: ProcessFantasyTracerTargetInfo
	@brief		: ȯ�� ������ Ÿ�� ���� ���� �Լ�
*/
void CX2GUAdd::ProcessFantasyTracerTargetInfo()
{
	vector<FantasyTracerTargetInfo>::iterator vIt = m_vecFantasyTracerTargetInfo.begin();

	while ( vIt != m_vecFantasyTracerTargetInfo.end() )
	{
		float& fRemainTime = vIt->m_fRemainTime;

		fRemainTime -= m_fElapsedTime;

		/// ���� �����ð� ���޽�, �ش� Ÿ�� ���� ��� ó��
		if ( _CONST_ADD_::FANTASY_TRACER_TARGET_DELETE_READY_TIME >= fRemainTime && false == vIt->m_bIsUseData )
		{
			vIt->m_bIsUseData = true;

			/// ��ų ������ ����
			if ( NULL != g_pX2Game )
				g_pX2Game->UpdateSkillSlotUI();
		}

		/// �ð� ����� ���� ��������
		if ( 0.f >= fRemainTime )
		{
			vIt = m_vecFantasyTracerTargetInfo.erase( vIt );

			/// ��ų ������ ����
			if ( NULL != g_pX2Game )
				g_pX2Game->UpdateSkillSlotUI();
		}
		else
			++vIt;
	}
}

/** @function	: SetDustAnatomyTarget
	@brief		: ����Ʈ �Ƴ���� Ÿ�� ����
	@param		: ������ ������
*/
void CX2GUAdd::SetDustAnatomyTarget( IN const CX2DamageManager::DamageData& pDamageData_ )
{
	if ( null == pDamageData_.optrDefenderGameUnit ||
		 CX2SkillTree::SI_SA_ALP_DUST_ANATOMY != pDamageData_.m_eUsedSkillID )
		return;

	/// ���� �� ���ֵ��� Ÿ�� ������ ����
	if ( NULL != g_pX2Game && false == g_pX2Game->GetUnitList().empty() )
	{
		CX2GameUnit* pUnit = pDamageData_.optrDefenderGameUnit.GetObservable();

		if ( NULL != pUnit )
		{
			/// Ÿ�� ���⿡ ���� ����Ʈ ���� �� ��ġ ����
			wstring wstrCreateBoneName = L"Bip01";

			if ( true == GetIsRight() )
				wstrCreateBoneName = IsRightTarget( pUnit->GetPos() ) ? L"Bip01_L_Finger2" : L"Bip01_R_Finger2";
			else
				wstrCreateBoneName = IsRightTarget( pUnit->GetPos() ) ? L"Bip01_R_Finger2" : L"Bip01_L_Finger2";

			bool bIsNPC = ( CX2GameUnit::GUT_NPC == pUnit->GetGameUnitType() ) ? true : false;

			DustAnatomyTargetInfo TargetInfo = DustAnatomyTargetInfo( INVALID_DAMAGE_EFFECT_HANDLE, pUnit->GetUnitUID(), bIsNPC, wstrCreateBoneName );

			m_vecDustAnatomyTargetInfo.push_back( TargetInfo );
		}
	}
}

/** @function	: GetTargetUnit
	@brief		: ����Ʈ �Ƴ���� Ÿ�� ��ȯ �Լ�
	@return		: Ÿ�� ��ü
*/
CX2GameUnit* CX2GUAdd::DustAnatomyTargetInfo::GetTargetUnit()
{
	if ( 0 < m_uidTargetUID && NULL != g_pX2Game )
	{
		for( CX2GameUnit* pUnit : g_pX2Game->GetUnitList() )
		{
			if ( NULL == pUnit )
				continue;

			bool bIsNPC = ( CX2GameUnit::GUT_NPC == pUnit->GetGameUnitType() ) ? true : false;

			/// NPC ��� �˻�
			if ( true == m_bIsNPC )
			{
				if ( true == bIsNPC && pUnit->GetUnitUID() == m_uidTargetUID )
					return pUnit;
			}
			/// User ��� �˻�
			else
			{
				if ( false == bIsNPC && pUnit->GetUnitUID() == m_uidTargetUID )
					return pUnit;
			}
		}
	}

	return NULL;
}

/** @function	: GetTargetUnit
	@brief		: ������ ������ Ÿ�� ��ȯ �Լ�
	@return		: Ÿ�� ��ü
*/
CX2GameUnit* CX2GUAdd::ReverseReactorTargetInfo::GetTargetUnit()
{
	if ( 0 < m_uidTargetUID && NULL != g_pX2Game )
	{
		for( CX2GameUnit* pUnit : g_pX2Game->GetUnitList() )
		{
			if ( NULL == pUnit )
				continue;

			bool bIsNPC = ( CX2GameUnit::GUT_NPC == pUnit->GetGameUnitType() ) ? true : false;

			/// NPC ��� �˻�
			if ( true == m_bIsNPC )
			{
				if ( true == bIsNPC && pUnit->GetUnitUID() == m_uidTargetUID )
					return pUnit;
			}
			/// User ��� �˻�
			else
			{
				if ( false == bIsNPC && pUnit->GetUnitUID() == m_uidTargetUID )
					return pUnit;
			}
		}
	}

	return NULL;
}

/** @function	: ResetReverseReactorTargetInfo
	@brief		: ������ ������ ���� ��ü �ʱ�ȭ �Լ�
*/
void CX2GUAdd::ReverseReactorTargetInfo::ResetReverseReactorTargetInfo()
{
	m_fMaxTransitionDamage	= 0.f;
	m_fCumulativeDamage		= 0.f;
	m_fRemainTime			= 0.f;
	m_fTransitionDamageRate = 0.f;

	CX2GameUnit* pTargetUnit = GetTargetUnit();

	if ( NULL != pTargetUnit )
		pTargetUnit->EraseBuffTempletFromGameUnit( BTI_DEBUFF_PLASMA_LINK );

	m_uidTargetUID = 0;

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_eLinkEffect );

	/// ����Ʈ ���� ���� ���
	if ( NULL != g_pKTDXApp && NULL != g_pKTDXApp->GetDeviceManager() )
		g_pKTDXApp->GetDeviceManager()->PlaySound( L"Add_Reverse_Reactor_ConnectCancel.ogg" );
}

/** @function	: ProcessReverseReactorTargetInfo
	@brief		: ������ ������ ���� ���� �Լ�
*/
void CX2GUAdd::ProcessReverseReactorTargetInfo()
{
	float& fRemainTime = m_ReverseReactorTargetInfo.m_fRemainTime;

	if ( 0.f < fRemainTime )
	{
		fRemainTime -= m_fElapsedTime;

		/// ���� �ð� �����, ��� ����
		if ( 0.f >= fRemainTime )
		{
			fRemainTime = 0.f;

			m_ReverseReactorTargetInfo.ResetReverseReactorTargetInfo();
		}
		/// ��ũ ����Ʈ ����
		else
		{
			CX2GameUnit* pUnit = m_ReverseReactorTargetInfo.GetTargetUnit();

			if ( NULL == pUnit || 0.f >= pUnit->GetNowHp() )
			{
				m_ReverseReactorTargetInfo.ResetReverseReactorTargetInfo();
				return;
			}

			D3DXVECTOR3	vMyPos	= GetBonePos( L"Bip01" );
			vMyPos.y			+= 25.f;

			D3DXVECTOR3	vEnemyPos;

			if ( false == pUnit->GetBonePos( L"Bip01", vEnemyPos ) )
			{
				vEnemyPos	=  pUnit->GetPos();
				vEnemyPos.y	+= 100.f;
			}
			else
			{
				vEnemyPos.y	+= 25.f;
			}

			/// ���� ���� �̻����� �־�����, ��ų ����
			if ( _CONST_ADD_::REVERSE_REACTOR_LINK_RANGE < GetDistance( vMyPos, vEnemyPos ) )
				m_ReverseReactorTargetInfo.ResetReverseReactorTargetInfo();
			/// ����Ʈ ���� ����
			else
			{
				CX2DamageEffect::CEffect*	pEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_ReverseReactorTargetInfo.m_eLinkEffect );

				if ( NULL != pEffect )
					pEffect->SetLinkMainEffectByPos( vMyPos, vEnemyPos );
			}
		}
	}
}

/** @function	: SetReverseReactorTransitionDamage
	@brief		: ������ ������ ���� ������ ó�� �Լ�
	@param		: �ǰ� ������
*/
/*virtual*/ void CX2GUAdd::SetReverseReactorTransitionDamage( IN const float fFinalDamage_ )
{
	if ( 0.f < m_ReverseReactorTargetInfo.m_fRemainTime )
	{
		CX2GameUnit* pUnit = m_ReverseReactorTargetInfo.GetTargetUnit();

		if ( NULL != g_pX2Game && NULL != pUnit && 0.f < pUnit->GetNowHp() )
		{
			float		fTransitionDamage		= m_ReverseReactorTargetInfo.m_fTransitionDamageRate * fFinalDamage_;	/// ���� ��ų ������
			float&		fCumulativeDamage		= m_ReverseReactorTargetInfo.m_fCumulativeDamage;		/// ���� ������
			const float fMaxTransitionDamage	= m_ReverseReactorTargetInfo.m_fMaxTransitionDamage;	/// �ִ� ��� ���� ������

			fCumulativeDamage += fTransitionDamage;		/// ������ ����

			/// �ִ� ��� ���� ������ ��ġ�� �Ѿ �� ������ ����
			if ( fMaxTransitionDamage < fCumulativeDamage )
			{
				float fOverDamage = fCumulativeDamage - fMaxTransitionDamage;

				fTransitionDamage -= fOverDamage;

				/// ��� ����
				m_ReverseReactorTargetInfo.ResetReverseReactorTargetInfo();
			}

			/// ü�� ���� �� ����ġ ǥ��
			pUnit->UpNowHp( - fTransitionDamage );
			g_pX2Game->DisplayDamageInformation( fTransitionDamage, pUnit->GetHeadBonePos(), pUnit->GetDirVector() );
		}

	}
}

/** @function	: ProcessQuicksilverFrenzy
	@brief		: ���ǹ� ������ �߻� ���� ����
	@param		:
	@return		:
*/
void CX2GUAdd::ProcessQuicksilverFrenzy()
{
	if ( false == m_QuicksilverFrenzyData.m_bEnableQuicksilverFrenzy )
	{
		m_eCustomRenderState = CRS_NONE;
		return;
	}
	else
	{
		m_eCustomRenderState = CRS_QUICKSILVER_FRENZY_BUFF;

		float& fFireIntervalTime = m_QuicksilverFrenzyData.m_fFireIntervalTime;

		if ( 0.f < fFireIntervalTime )
		{
			fFireIntervalTime -= m_fElapsedTime;

			if ( 0.f >= fFireIntervalTime )
				fFireIntervalTime = 0.f;
		}
	}
}

/** @function	: AttackQuicksilverFrenzy
	@brief		: �ǰ� Ȥ�� Ÿ�ݽ� ���ǹ� ������ �߻�ü ���� �� �Ͽ�
	@param		: ������ ����
*/
void CX2GUAdd::AttackQuicksilverFrenzy( IN const CX2DamageManager::DamageData& pDamageData_, IN const bool bIsAttack_ /*= false*/ )
{
	/// ���� ������ �̰ų� Ư�� ���� Ȥ�� �߻� ������ ����������, �߻�ü ���� �н�
	if ( false == m_QuicksilverFrenzyData.m_bEnableQuicksilverFrenzy ||
		 CX2DamageManager::AT_SPECIAL == pDamageData_.attackType ||
		 0.f < m_QuicksilverFrenzyData.m_fFireIntervalTime )
		return;

	/// �� ��Ȳ�� ���� Ÿ�� ��� ����
	CX2GameUnit* pTargetUnit = ( true == bIsAttack_ ) ? pDamageData_.optrDefenderGameUnit.GetObservable() : pDamageData_.optrAttackerGameUnit.GetObservable();

	if ( NULL != pTargetUnit )
	{

		/// �߻�ü ����
		CX2DamageEffect::CEffect* pEffect =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ALP_Quicksilver_Frenzy_Projectile", m_QuicksilverFrenzyData.m_fPowerRate,
														  GetBonePos( L"Bip01" ), GetRotateDegree(), GetRotateDegree(),
														  m_FrameDataNow.unitCondition.landPosition.y );
		/// �Ͽ� Ÿ�� ����
		if ( NULL != pEffect )
		{
			CX2DamageEffect::LockOnData* pLockOnData = &pEffect->GetLockOnData();

			if( CX2GameUnit::GUT_USER == pTargetUnit->GetGameUnitType() )
				pLockOnData->m_LockOnUnitUID = pTargetUnit->GetUnitUID();
			else
				pLockOnData->m_LockOnNPCUID = static_cast<int>( pTargetUnit->GetUnitUID() );
		}

		///�߻� ���� ����
		m_QuicksilverFrenzyData.m_fFireIntervalTime = _CONST_ADD_::QUICKSILVER_FRENZY_FIRE_INTERVAL_TIME;
	}
}

/** @function	: SetQuicksilverFrenzyFlickerColor
	@brief		: ���ǹ� ������ ���� ���� ȿ�� �÷��� ����
	@param		: ���� ��ų �÷���
*/
const D3DXCOLOR CX2GUAdd::SetQuicksilverFrenzyFlickerColor( IN const D3DXCOLOR& coRenderColor_ )
{
	const float fFlickerGap = _CONST_ADD_::QUICKSILVER_FRENZY_FLICKER_INTERVAL_TIME;	/// ��¦�̴� ����

	D3DXCOLOR coResultColor( 1.f, 1.f, 1.f, 1.f );

	/// ��¦�̴� �ð� ����
	m_fQuicksilverFrenzyFlickerTime += m_fElapsedTime;

	if ( m_fQuicksilverFrenzyFlickerTime >= fFlickerGap )
		m_fQuicksilverFrenzyFlickerTime = 0.f;

	/// ��¦�̴� ���ݿ� ���� �����
	float fFlickerRate = m_fQuicksilverFrenzyFlickerTime / fFlickerGap;

	/// ������� ���� �̻��̸�, ���� ����
	if ( ( fFlickerGap / 2 ) < m_fQuicksilverFrenzyFlickerTime )
	{
		fFlickerRate = ( fFlickerRate - 0.5f ) * 2.f;	/// 0 ~ 1.f �� ����� ��ȯ

		coResultColor.r = max( 1.f - ( ( 1.f - coRenderColor_.r ) * fFlickerRate ), coRenderColor_.r );
		coResultColor.g = max( 1.f - ( ( 1.f - coRenderColor_.g ) * fFlickerRate ), coRenderColor_.g );
		coResultColor.b = max( 1.f - ( ( 1.f - coRenderColor_.b ) * fFlickerRate ), coRenderColor_.b );
	}
	///  ������� ���� ���ϸ�, ���� ����
	else
	{
		fFlickerRate *= 2.f;	/// 0 ~ 1.f �� ����� ��ȯ

		coResultColor.r = min( coRenderColor_.r + ( ( 1.f - coRenderColor_.r ) * fFlickerRate ), 1.f );
		coResultColor.g = min( coRenderColor_.g + ( ( 1.f - coRenderColor_.g ) * fFlickerRate ), 1.f );
		coResultColor.b = min( coRenderColor_.b + ( ( 1.f - coRenderColor_.b ) * fFlickerRate ), 1.f );
	}

	return coResultColor;
}

/** @function	: CheckSkillSpecialUse
	@brief		: Ư���� ��ų�� ���Ͽ�, ��� ���� ���θ� �˻�
	@param		: ��ų ���̵�
	@return		: ��ų ��� ���� ����
*/
bool CX2GUAdd::CheckSkillSpecialUse( IN const CX2SkillTree::SKILL_ID eSkillID_  )
{
	/// ���ǹ� �׼� ���� ���� ������ �ð��߿� �ٸ� ��ų�� ����Ϸ� �ϸ�, ������ ����.
	if ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == GetFutureStateSkillID() )
	{
		if ( ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK != GetFutureStateID() )
		{
			if ( m_fEventTime[1].keyInputStart < m_pXSkinAnimFuture->GetNowAnimationTime() &&
				m_fEventTime[1].keyInputEnd >= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				return  ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == eSkillID_ ) ? true : false;
			}
			else
			{
				return  ( CX2SkillTree::SI_SA_ALP_QUICKSILVER_ACCEL == eSkillID_ ) ? false :  true;
			}
		}
			return false;
	}

#ifdef X2_ADD_2ND_PATH

	if ( true == ThirdPath_IsInstallFamilySkill( (int)eSkillID_ ) && 0 != m_iFactoryMarkedSkillID && (int)eSkillID_ != m_iFactoryMarkedSkillID && 3 == m_iFactoryPhase )
		return false;
	//}} 2026-09-04 X2GUAdd reverify CAST-7 //
#endif

	return true;
}

/** @function	: ResetPsychicStormData
	@brief		: ����ű ���� ��ü �ʱ�ȭ �Լ�
*/
void CX2GUAdd::PsychicStormData::ResetPsychicStormData()
{
	m_fIncreaseSize		= 0.f;
	m_fLoopTime			= 0.f;
	m_fFireGap			= 0.f;

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		g_pX2Game->GetEffectSet()->DestroyInstanceHandle( eLoopEffectHandle );

#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATComboNasodArmorXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_AAT_COMBO_NASOD_ARMOR_XX )


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATComboNasodArmorXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_AAT_COMBO_NASOD_ARMOR_XXX )


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATComboNasodArmorXXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATComboNasodArmorZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_AAT_COMBO_NASOD_ARMOR_ZZ )


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATComboNasodArmorZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ANSI_AAT_COMBO_NASOD_ARMOR_ZZZ )


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATComboNasodArmorZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;

	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZBackXStateStart()
{
	m_eCustomRenderState	= CRS_NONE;
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZBackXFrameMove()
{
	CommonFrameMove();

	// official FM 324E6EE0: render window [0.0, 0.233) (dbl_32D20458 / dbl_32D875F8 byte-verified) //
	if( 0.f <= m_pXSkinAnim->GetNowAnimationTime() && 0.233f > m_pXSkinAnim->GetNowAnimationTime() )
	{
		m_eCustomRenderState	= CRS_COMBO_DASH_X;
	}
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZBackXEventProcess()
{
	// official EP 324E6F30: air->17+y-30; animEnd->13; buffered oneX window[0] -> COMBO_XX with the MM once-flag set BEFORE the transition (official byte 25748 := 1 = m_bXXDownXUsed - after a ZBackX the following XX can't go XXDownX/XXZ, COMBO-4 member); cancels //
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		m_bXXDownXUsed = true;
		StateChange( ANSI_COMBO_XX );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( true == m_InputData.oneX && m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZBackXStateEnd()
{
	m_fRenderParamColorA	= 1.f;
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZXStateStart()
{
	m_eCustomRenderState	= CRS_NONE;
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZXFrameMove()
{
	CommonFrameMove();

	// official FM 324E6C80: render window [0.085, 0.43) (dbl_32D87590 / dbl_32D7D518 - same consts as the base dash-X) //
	if( 0.085f <= m_pXSkinAnim->GetNowAnimationTime() && 0.43f > m_pXSkinAnim->GetNowAnimationTime() )
	{
		m_eCustomRenderState	= CRS_COMBO_DASH_X;
	}
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZXEventProcess()
{
	// official EP 324E6CD0: air->17+y-30; animEnd->13; buffered oneX window[0] -> DASH_COMBO_XX (official 133; the fire's "|| animEnd" alternative is dead - the anim-end branch pre-empts, C09 section 0); cancels //
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_DASH_COMBO_XX )

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZXStateEndFuture()
{
	// official SEF 324DE080: CommonStateStartFuture + facing flip + dirDegree.y += 180 (identical body to the base dash-X SEF 324DDFD0) //
	CommonStateStartFuture();

	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AATDashComboZXStateEnd()
{
	m_fRenderParamColorA	= 1.f;
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMComboXXDownXStartEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_AMM_COMBO_XXDownX_LOOP );
	}

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMComboXXDownXStartStateEnd()
{

	m_fXXDownXLoopSpawnAccum = 0.f;
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMComboXXDownXLoopEventProcess()
{

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_fSkillCancelAfter >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fSkillCancelAfter &&
		SpecialAttackEventProcess() == true )
	{
	}
	else if( false == IsThereEffectiveBlackHoleDamageEffect() &&
		true == m_InputData.oneLCtr && true == GetPossibleChangeFormationMode() &&
		m_fSkillCancelAfter >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fSkillCancelAfter )
	{
		StateChange( USI_HYPER_MODE );
	}
	else if( m_FrameDataNow.unitCondition.fStateTime < 0.3f )	// official this[21388] = the state elapsed time (DJXX-loop-proven member)
	{
		if( m_fWalkCancelAfter >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fWalkCancelAfter &&
			( true == m_InputData.pureRight || true == m_InputData.pureLeft ) )
		{
			StateChange( USI_WAIT );
		}
		else if( m_fDashCancelAfter >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fDashCancelAfter &&
			( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft ) )
		{
			StateChangeDashIfPossible();
		}
	}
	else if( false == m_InputData.pureX || m_FrameDataNow.unitCondition.fStateTime >= 3.0f )
	{
		StateChange( ANSI_AMM_COMBO_XXDownX_END );
	}

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::AMMComboXXDownXLoopStateStart()
{

	CommonStateStart();
	SetNoDetonation( true );
	m_fXXDownXLoopSpawnAccum = 0.f;
	m_fXXDownXChildSpawnAccum = 0.f;
	m_iXXDownXChildSpawnCount = 0;	//}} 2026-09-06 //
	if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hXXDownXLoopDE && NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hXXDownXLoopDE, false );
	m_hXXDownXLoopDE = INVALID_DAMAGE_EFFECT_HANDLE;

				GetBonePos( L"Bip01_R_Hand", m_vXXDownXAnchor );
				m_vXXDownXAnchor += GetDirVector() * ( true == GetIsRight() ? 750.f : -750.f );
}
void CX2GUAdd::AMMComboXXDownXLoopFrameMove()
{

	const float fStateTime = m_FrameDataNow.unitCondition.fStateTime;
	if ( fStateTime - m_fXXDownXLoopSpawnAccum >= 0.05f )	// official window dbl_32D20460 = 0.05
	{
		m_fXXDownXLoopSpawnAccum = fStateTime;
		const float fDrainDP = m_fElapsedTime * 13.333f;	// official: this[86] (frame elapsed) x dbl_32D81E50

		bool bCanConsume = true;
		if ( IsMyUnit() )
		{
			bCanConsume = ( GetDPValue() >= fDrainDP );
			if ( bCanConsume )
				SetDPValue( GetDPValue() - fDrainDP );
		}
		if ( bCanConsume )
		{
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
			{
				D3DXVECTOR3 vHand = GetPos();
				GetBonePos( L"Bip01_R_Hand", vHand );	// official bone aBip01RHand
				if ( INVALID_DAMAGE_EFFECT_HANDLE == m_hXXDownXLoopDE )
				{
					CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AMM_XXDownX_Loop", GetPowerRate(),
						vHand, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				if ( NULL != pDE )
					m_hXXDownXLoopDE = pDE->GetHandle();
			}
				CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hXXDownXLoopDE );
				if ( NULL != pDE )
				{

					const float fReach = ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? 600.f : 800.f;	// official search radius (PvP/PvE, byte-verified consts flt_32D3B718/flt_32D2FA48)
					static const float s_afAnchorParams[3] = { 1.0472f, -0.17453f, FLT_32D87830 };	// official v45[] block (60deg rad, 10deg rad x -1, cone base)
					D3DXVECTOR3 vEnemyPos( 0.f, 0.f, 0.f );	// official out-param (the found unit's pos), discarded there too
					if ( false == ThirdPath_AnchorChaseTarget( fReach, s_afAnchorParams, &m_vXXDownXAnchor, &vEnemyPos, 0 ) )
						m_vXXDownXAnchor = vHand + GetDirVector() * ( true == GetIsRight() ? 750.f : -750.f );
					pDE->SetLinkMainEffectByPos( vHand, m_vXXDownXAnchor );	// stretch the beam hand->anchor (official f_322BB350; scale = distance/800 engine-side)

				}
			}
		}
		else
		{
			// DP exhausted -> the beam dies (official fail path: destroy + fizzle) //
			if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hXXDownXLoopDE && NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hXXDownXLoopDE, false );
			m_hXXDownXLoopDE = INVALID_DAMAGE_EFFECT_HANDLE;
		}
	}
	CommonFrameMove();
}
void CX2GUAdd::AMMComboXXDownXLoopStateEnd()
{

	SetNoDetonation( false );
	if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hXXDownXLoopDE && NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hXXDownXLoopDE, false );
	m_hXXDownXLoopDE = INVALID_DAMAGE_EFFECT_HANDLE;
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMComboXXDownXEndEventProcess()
{

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMComboXXDownXEndStateStart()
{

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		D3DXVECTOR3 vHand = GetPos();
		GetBonePos( L"Bip01_R_Hand", vHand );

		CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AMM_XXDownX_Finish", GetPowerRate(),
			vHand, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		if ( NULL != pDE )
			pDE->SetLinkMainEffectByPos( vHand, m_vXXDownXAnchor );
	}
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMDashJumpComboXZEventProcess()
{

	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true &&
			 ( m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange || m_pXSkinAnimFuture->IsAnimationEnd() == true ) )
	{
		StateChange( ANSI_AMM_DASH_JUMP_COMBO_XZZ );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( true == m_InputData.oneZ &&
			 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::AMMDashJumpComboXZZEventProcess()
{

	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
									GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_ThirdPath_CreateState( UserUnitStateData& stateData_, const int iStateId_, const char* pszTableName )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( pszTableName, stateData_.stateID );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_ThirdPath_OneShot_EventProcess );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_ThirdPath_CreateChainedState( UserUnitStateData& stateData_, const int iStateId_, const char* pszTableName, const int iNextStateId_ )
{
	ANSI_ThirdPath_CreateState( stateData_, iStateId_, pszTableName );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAdd, ANSI_ThirdPath_Chain_EventProcess );
	m_mapThirdPathChain[iStateId_] = iNextStateId_;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_ThirdPath_OneShot_EventProcess()
{

	const int iNowStateID = GetNowStateID();
	const bool bComboTerminal = ( ANSI_ATT_COMBO_ZZXX == iNowStateID ||
		ANSI_ATT_COMBO_NASOD_ARMOR_XXX_END == iNowStateID || ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END == iNowStateID ||
		ANSI_ADE_COMBO_XXXX_FORMATION_MODE == iNowStateID || ANSI_ADE_COMBO_XXXX_NO_FORMATION_MODE == iNowStateID ||
		ANSI_ADE_COMBO_XXXZ_FORMATION_MODE == iNowStateID || ANSI_ADE_COMBO_XXXZ_NO_FORMATION_MODE == iNowStateID );
	const bool bCarriesNA = ( ANSI_ADE_COMBO_XXXX_FORMATION_MODE == iNowStateID || ANSI_ADE_COMBO_XXXZ_FORMATION_MODE == iNowStateID || ANSI_ATT_COMBO_ZZXX == iNowStateID );

	if ( true == bComboTerminal )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}
		else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
		// the NA-entry macro is inert for the non-carrier terminals (the buffer needs the composite gate) and live for the carriers //
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
										( true == GetIsFormationMode() && true == bCarriesNA ), m_NasodArmorAttackTargetData.m_uidTargetUID )
		SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
		return;
	}

	// GRAVITY_BURSTER family: anim-end -> USI_JUMP_LANDING if grounded (official 3250EB10; no air-exit officially) //
	if ( (int)CX2SkillTree::SI_SA_ADE_GRAVITY_BURSTER == (int)m_NowStateData.m_eSkillID )
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true && true == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_LANDING );
		}

		CommonEventProcess();
		return;
	}

	// REVERSE_CIRCLE family: anim-end -> USI_WAIT if grounded (official 3250DDC0) //
	if ( (int)CX2SkillTree::SI_SA_ADE_REVERSE_CIRCLE == (int)m_NowStateData.m_eSkillID )
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true && true == IsOnSomethingFuture() )
		{
			StateChange( USI_WAIT );
		}


		SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

		CommonEventProcess();
		return;
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}


	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_ThirdPath_Chain_EventProcess()
{

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		std::map<int,int>::const_iterator it = m_mapThirdPathChain.find( GetNowStateID() );
		if( it != m_mapThirdPathChain.end() )
		{
			StateChange( it->second );
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_COMMON_CREST_OF_TIME_StateInit()
{

	TextureReadyInBackground( L"Arme_Critical2.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Buff_Critical.dds" );
	TextureReadyInBackground( L"CREST_OF_TIME_Ring.dds" );
	XSkinMeshReadyInBackground( L"Crest_of_Time_Time_Start.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	XMeshReadyInBackground( L"Arme_ChargeMp1.y" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	XSkinMeshReadyInBackground( L"Crest_of_Time_Time_Back.X" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_COMMON_CREST_OF_TIME_FrameMove()
{
	/// official sub_324EDB20: one-shot @0.342s (flt_32D879B4 ��� matches the lua EFFECT_SET_LIST_EX timing) -> buff SELF + ALL same-team units within range^2 <= 700.0 (dbl_32D2ECF8); the 2015 unit+288..296 protected triple == our CX2GameUnit::m_TeamNum (GetTeam()); NO HP%/party-list/class filter //
	if ( m_pXSkinAnim->EventTimerOneshot( 0.342f ) )
	{
		const int iSkillLevel = ( NULL != GetUnit() ) ? GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_COMMON_CREST_OF_TIME, true ) : 1;	/// official f_31E9B2A0: tree level, fallback 1
		const D3DXVECTOR3 vMyPos = GetPos();

		if ( NULL != g_pX2Game )
		{
			for ( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
			{
				CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
				if ( NULL == pGameUnit )
					continue;
				if ( pGameUnit->GetTeam() != GetTeam() )	/// same-team only
					continue;

				const D3DXVECTOR3 vDelta = pGameUnit->GetPos() - vMyPos;

				const float fDistCrest = sqrtf( vDelta.x * vDelta.x + vDelta.y * vDelta.y + vDelta.z * vDelta.z );
				if ( fDistCrest <= 700.f )
				{
					pGameUnit->SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_CREST_OF_TIME, (UINT)iSkillLevel );					/// BFI 474 -> BTI 436 (snapshot marker + buff-loop effect)
					pGameUnit->SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_CREST_OF_TIME_DECREASE_COOLTIME, (UINT)iSkillLevel );	/// BFI 501 -> BTI 442 (BFT_IMMEDIATE: subtract 2*level s from equipped-skill cooldowns)
				}
			}
		}
	}

	CommonFrameMove();
}
#endif
#endif
#ifdef X2_ADD_3RD_PATH
/*virtual*/ void CX2GUAdd::RecordCrestSnapshot()
{
	CX2GUUser::RecordCrestSnapshot();	/// shared HP/MP (official shared body 0x31EA20A0)
	if ( false == IsMyUnit() )
		return;

	m_fCrestRecordDP		= GetDPValue();			/// official: f_31F4B8C0(gage) ��� max-capped DP read
	m_iCrestRecordMutation	= GetMutationCount();	/// official: *(gage+248) clamped 0..6 (our accessor clamps the same way)
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
/*virtual*/ bool CX2GUAdd::IsBlockPacketBuffTemplet( int iBuffTempletID_ )
{
	return ( BTI_BUFF_DYNAMO_FACTORY == iBuffTempletID_ || BTI_BUFF_MOONLIGHT_RHAPSODY == iBuffTempletID_ );
}
#endif
#ifdef X2_ADD_3RD_PATH
/*virtual*/ void CX2GUAdd::RestoreCrestSnapshot()
{
	if ( true == IsMyUnit() )
	{
		SetDPValue( m_fCrestRecordDP );				/// official vtable+1076 (SetDPValue family)
		SetMutationCount( m_iCrestRecordMutation );	/// official vtable+1124
	}
	CX2GUUser::RestoreCrestSnapshot();	/// official f_31ECC550 ��� HP (floor 1.0) + MP + clear the block
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_ATT_VOID_IMPACT_StateInit()
{
	/// 9023 ��� official sub_324DF0B0, 12 preloads //
	XMeshReadyInBackground( L"ADD_Effect_CIRCLE_03_1.Y" );
	XMeshReadyInBackground( L"ADD_Effect_CIRCLE_03_2.Y" );
	TextureReadyInBackground( L"ADD_GlowBall_G01.tga" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	TextureReadyInBackground( L"add_EFFECT_Cycle_Blur01.dds" );
	TextureReadyInBackground( L"ADD_Dark_Cycle_Attack_2.tga" );
	TextureReadyInBackground( L"ADD_Effect_Triangle01.Tga" );
	XSkinMeshReadyInBackground( L"ATT_Void_Impact_Rupture_01.X" );
	XSkinMeshReadyInBackground( L"ATT_Void_Impact_Rupture_02.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	TextureReadyInBackground( L"GlowBall_G01.tga" );
}
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_StateInit()
{
	/// 9024 ��� official sub_324DF160, 23 preloads //
	TextureReadyInBackground( L"ADD_CYCLE_COA02.tga" );
	XMeshReadyInBackground( L"ADD_DOOMS_DAY_Grow01.Y" );
	TextureReadyInBackground( L"ADD_Dark_Cycle_Attack_1.tga" );
	TextureReadyInBackground( L"ADD_MM_Spark01.tga" );
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y" );
	XSkinMeshReadyInBackground( L"ADD_PHASE_SHIFT_SPARK01.X" );
	XSkinMeshReadyInBackground( L"ATT_Void_Field_Attack_Line.X" );
	XSkinMeshReadyInBackground( L"ATT_Void_Field_Jin.X" );
	XSkinMeshReadyInBackground( L"ATT_Void_Field_Jin2.X" );
	XMeshReadyInBackground( L"ATT_Void_Field_Semicircle.Y" );
	TextureReadyInBackground( L"Arme01_Frost_Ring_LV2_effect_01.dds" );
	XMeshReadyInBackground( L"Arme_ChargeMp1.y" );
	TextureReadyInBackground( L"Black_Circle01.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	XSkinMeshReadyInBackground( L"Spin_Liner02.X" );
	TextureReadyInBackground( L"add_EFFECT_Cycle_Blur01.dds" );
	TextureReadyInBackground( L"aisha_active_guillotinePress_lightBlur.dds" );
	XSkinMeshReadyInBackground( L"elsword_SA_rageCutter_attackSphere.X" );
	SoundReadyInBackground( L"ADD_VOID_FIELD01.ogg" );
	SoundReadyInBackground( L"AddVoice_Void_Field01.ogg" );
	SoundReadyInBackground( L"AddVoice_Void_Field02.ogg" );
	SoundReadyInBackground( L"AddVoice_Void_Field01_Hyper.ogg" );
	SoundReadyInBackground( L"AddVoice_Void_Field02_Hyper.ogg" );
}
void CX2GUAdd::ANSI_SA_ATT_MAXIMUM_STRIKE_StateInit()
{
	/// 9025 ��� official sub_324DF380, 17 preloads //
	XMeshReadyInBackground( L"AA_SUPPRESSION_SPECTER_P01.Y" );
	TextureReadyInBackground( L"ADD_Dark_Cycle_Attack_2.tga" );
	XMeshReadyInBackground( L"ADD_MM_tech001.Y" );
	XSkinMeshReadyInBackground( L"ADD_PSYCHIC_STORM_01.X" );
	XMeshReadyInBackground( L"ADE_PE03.Y" );
	XMeshReadyInBackground( L"ATT_MAXIMUM_STRIKE_ARMS01.Y" );
	TextureReadyInBackground( L"CREST_OF_TIME_Ring.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Common_Moon_AC_Map01.dds" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	TextureReadyInBackground( L"RingAlpha.dds" );
	TextureReadyInBackground( L"ADD_Effect_Triangle01.Tga" );
	XMeshReadyInBackground( L"ADE_Win_P01.Y" );
	XMeshReadyInBackground( L"ADE_Win_P03.Y" );
	XSkinMeshReadyInBackground( L"ATT_Maximum_Strike_Ball_AttackBox.X" );
	XSkinMeshReadyInBackground( L"ATT_Maximum_Strike_Dummy01.X" );
	XMeshReadyInBackground( L"ATT_Maximum_Strike_W.Y" );
}
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_StateInit()
{
	/// 9026 ��� official sub_324DF480, 18 preloads (the #16 spwanPoint mesh also rides our batch1 StartFuture preload ��� the loader cache makes the dupe a no-op) //
	TextureReadyInBackground( L"ADD_CYCLE_COA02.tga" );
	TextureReadyInBackground( L"ADD_Dark_Cycle_Attack_2.tga" );
	XMeshReadyInBackground( L"ADD_LS_Win01.Y" );
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y" );
	XSkinMeshReadyInBackground( L"ATT_Maximum_Strike_Ball_AttackBox.X" );
	XSkinMeshReadyInBackground( L"ATT_Stardust_Shower_Rupture.X" );
	XSkinMeshReadyInBackground( L"ATT_Void_Field_Jin.X" );
	XSkinMeshReadyInBackground( L"ATT_Void_Field_Jin2.X" );
	XSkinMeshReadyInBackground( L"Bat_StandUpAttack.X" );
	TextureReadyInBackground( L"Black_Circle01.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	XMeshReadyInBackground( L"Dark_Drabaki_Dead_01.Y" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"HELLPUTT_GuideArrow02.dds" );
	TextureReadyInBackground( L"PROTECT_GUARD_MAGIC_Map08.dds" );
	XSkinMeshReadyInBackground( L"elsword_SA_rageCutter_spwanPoint.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_100x50x50_center.X" );
	XSkinMeshReadyInBackground( L"ATT_ArmorComboZZZZ_Gate.X" );
}
void CX2GUAdd::ANSI_SA_ADE_REVERSE_CIRCLE_StateInit()
{
	/// 9027 ��� official sub_324DF640, 7 preloads //
	XMeshReadyInBackground( L"ADD_MM_tech001.Y" );
	XSkinMeshReadyInBackground( L"ADD_PSYCHIC_STORM_01.X" );
	TextureReadyInBackground( L"Arme_Critical.dds" );
	TextureReadyInBackground( L"Arme_Damage_Center.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"star1.dds" );
}
void CX2GUAdd::ANSI_SA_ADE_REVERSE_STIGMA_StateInit()
{
	/// 9028 ��� official sub_324DF6B0, 3 preloads //
	TextureReadyInBackground( L"ADD_RingAlpha.tga" );
	XMeshReadyInBackground( L"ADE_PE04.Y" );
	XMeshReadyInBackground( L"ADD_MM_Effect_Color01.Y" );
}
void CX2GUAdd::ANSI_SA_ADE_MOONLIGHT_RHAPSODY_StateInit()
{
	/// 9029 ��� official sub_324DF6E0, 7 preloads //
	XMeshReadyInBackground( L"ADD_MM_Effect_Color01.Y" );
	TextureReadyInBackground( L"ADD_RingAlpha.tga" );
	XMeshReadyInBackground( L"ADE_FORCE_NOVA_P01.Y" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	XMeshReadyInBackground( L"AA_SUPPRESSION_SPECTER_P01.Y" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
}
void CX2GUAdd::ANSI_SA_ADE_GRAVITY_BURSTER_StateInit()
{
	/// 9030 ��� official sub_324DF750, 9 preloads //
	XMeshReadyInBackground( L"AAS_NINE_TAILED_FOX_P01.Y" );
	XMeshReadyInBackground( L"ADD_MM_Effect_Color01.Y" );
	XMeshReadyInBackground( L"ADD_MM_tech001.Y" );
	XSkinMeshReadyInBackground( L"ADD_PSYCHIC_STORM_01.X" );
	XMeshReadyInBackground( L"ADE_PE04.Y" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"ELK_WIND_SLICER_01.dds" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_6Point.X" );
}
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_StateInit()
{
	/// 9212 ��� official sub_324DFE90, 7 preloads //
	TextureReadyInBackground( L"ADD_CYCLE_COA02.tga" );
	XMeshReadyInBackground( L"ADD_PARTICLE_ACCELERATOR_SPARK02.Y" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"ADD_Light001.tga" );
	XSkinMeshReadyInBackground( L"ATT_Void_Field_Jin2.X" );
	TextureReadyInBackground( L"Nasod_Rupture01.dds" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
}
void CX2GUAdd::ANSI_A_ADE_FORCE_NOVA_StateInit()
{
	/// 9213 ��� official sub_324DFFE0, 10 preloads //
	XMeshReadyInBackground( L"ADD_Effect_Line04.Y" );
	XMeshReadyInBackground( L"ADD_MM_Effect_Color01.Y" );
	TextureReadyInBackground( L"ADD_RingAlpha.tga" );
	XMeshReadyInBackground( L"ADE_FORCE_NOVA_P01.Y" );
	XSkinMeshReadyInBackground( L"AN_explosion_01.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"Flame_REVOLVER.dds" );
	TextureReadyInBackground( L"HALLOWEEN_PUMPKIN_Attack01.tga" );
	TextureReadyInBackground( L"smoke02.dds" );
}
void CX2GUAdd::ANSI_A_ADE_FORCE_FINGER_StateInit()
{
	/// 9214 ��� official sub_324E00A0, 4 preloads //
	XMeshReadyInBackground( L"ADE_PS01.Y" );
	TextureReadyInBackground( L"ADE_L_light.dds" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	XMeshReadyInBackground( L"ADE_PE04.Y" );
}
void CX2GUAdd::ANSI_HA_ADE_PARANOIA_StateInit()
{
	/// 9602 ��� official sub_324E01A0, 14 preloads //
	XMeshReadyInBackground( L"AA_SUPPRESSION_SPECTER_P01.Y" );
	XMeshReadyInBackground( L"ADE_FORCE_NOVA_P01.Y" );
	XMeshReadyInBackground( L"ADE_PS01.Y" );
	XMeshReadyInBackground( L"ADE_Win_P02.Y" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"eve_A_blink_light.dds" );
	TextureReadyInBackground( L"ADD_Effect_Triangle01.tga" );
	XSkinMeshReadyInBackground( L"ADE_PARANOIA_CP01.X" );
	XSkinMeshReadyInBackground( L"ADE_PARANOIA_LP01.X" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	XMeshReadyInBackground( L"ADE_REVERSE_CIRCLE_GATE01.Y" );
	XMeshReadyInBackground( L"ADE_REVERSE_CIRCLE_GATE02.Y" );
	XMeshReadyInBackground( L"ADE_REVERSE_CIRCLE_GATE03.Y" );
	XMeshReadyInBackground( L"ADE_REVERSE_CIRCLE_GATE04.Y" );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
float CX2GUAdd::ThirdPath_GetScanScope( const CX2SkillTree::SKILL_ID eSkillID_ )
{
	// official value-type 29 (SA_SCOPE_ABS); PvP game types use 157 (SA_SCOPE_ABS_PVP ��� official game-type check {1,4} = {PVP, SQUARE}; squares can't cast so the live PvP case = GT_PVP only; stigma desc "Range: 750 (PvP 400)")
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	if ( NULL == pSkillTemplet || NULL == GetUnit() )
		return 1.f;
	const int iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( eSkillID_ ) );
	if ( NULL != g_pX2Game )
	{
		const CX2Game::GAME_TYPE eGameType = g_pX2Game->GetGameType();

		if ( CX2Game::GT_PVP == eGameType )
		{
			const float fPvPScope = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS_PVP, iLevel );
			if ( 0.f < fPvPScope )
				return fPvPScope;
		}
	}
	const float fScope = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iLevel );
	return ( 0.f < fScope ) ? fScope : 1.f;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
int CX2GUAdd::ThirdPath_FindScanTargets( const float fRadius_, const float fYBand_, CX2GameUnit** ppOutTargets_, const int iMaxTargets_, const float fDownAngleDeg_ /*= -9999.f*/, const float fUpAngleDeg_ /*= 9999.f*/ )
{
	// official f_31EBB040: iterate hostile units ��� skip dead/self/friendly-team; facing-cone filter; |dy| band; closest-first by dist2
	if ( NULL == g_pX2Game || NULL == ppOutTargets_ || iMaxTargets_ <= 0 )
		return 0;

	const D3DXVECTOR3 vMyPos = GetPos();

	D3DXVECTOR3 vMyDir = GetDirVector();
	if ( false == GetIsRight() )
	{
		vMyDir.x = -vMyDir.x;
		vMyDir.z = -vMyDir.z;
	}
	const float fRadiusSq = fRadius_ * fRadius_;

	int iCount = 0;
	for ( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
	{
		CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
		if ( NULL == pGameUnit || pGameUnit == this ||
			pGameUnit->GetNowHp() <= 0.f ||
			pGameUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE ||
			pGameUnit->GetTeam() == GetTeam() )
		{
			continue;
		}


		if ( CX2GameUnit::GUT_NPC == pGameUnit->GetGameUnitType() &&
			CX2UnitManager::NCT_BASIC != static_cast<CX2GUNPC*>( pGameUnit )->GetClassType() )
		{
			continue;
		}


		const D3DXVECTOR3 vTargetPos = pGameUnit->GetPos();

		const D3DXVECTOR3 vDelta = vTargetPos - vMyPos;
		if ( vDelta.x * vMyDir.x + vDelta.z * vMyDir.z <= 0.f )
			continue;


		if ( -9999.f < fDownAngleDeg_ || fUpAngleDeg_ < 9999.f )
		{
			const float fHorizontal = sqrtf( vDelta.x * vDelta.x + vDelta.z * vDelta.z );
			const float fElevationDeg = D3DXToDegree( atan2f( vDelta.y, fHorizontal ) );
			if ( fElevationDeg < fDownAngleDeg_ || fUpAngleDeg_ < fElevationDeg )
				continue;
		}

		const float fDistSq = vDelta.x * vDelta.x + vDelta.y * vDelta.y + vDelta.z * vDelta.z;
		if ( fDistSq > fRadiusSq )
			continue;

		// insert sorted by distance (closest-first), capped at iMaxTargets_
		int iSlot = iCount;
		if ( iSlot >= iMaxTargets_ )
			iSlot = iMaxTargets_ - 1;
		for ( ; iSlot > 0; --iSlot )
		{
			const D3DXVECTOR3 vPrevDelta = ppOutTargets_[iSlot-1]->GetPos() - vMyPos;
			const float fPrevDistSq = vPrevDelta.x * vPrevDelta.x + vPrevDelta.y * vPrevDelta.y + vPrevDelta.z * vPrevDelta.z;
			if ( fPrevDistSq <= fDistSq )
				break;
			if ( iSlot < iMaxTargets_ )
				ppOutTargets_[iSlot] = ppOutTargets_[iSlot-1];
		}
		if ( iSlot < iMaxTargets_ )
		{
			ppOutTargets_[iSlot] = pGameUnit;
			if ( iCount < iMaxTargets_ )
				++iCount;
		}
	}
	return iCount;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
CX2GameUnit* CX2GUAdd::ThirdPath_FindScanTarget( const CX2SkillTree::SKILL_ID eSkillID_ )
{
	CX2GameUnit* pTarget = NULL;
	if ( ThirdPath_FindScanTargets( ThirdPath_GetScanScope( eSkillID_ ), 180.f, &pTarget, 1 ) > 0 )
		return pTarget;
	return NULL;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::ThirdPath_AnchorChaseTarget( const float fRange_, const float pfParams_[3], OUT D3DXVECTOR3* pvAnchorOut_, OUT D3DXVECTOR3* pvEnemyPosOut_, const char cMode_ /*= 0*/ )
{
	if ( NULL == g_pX2Game || NULL == pvAnchorOut_ )
		return false;
	const D3DXVECTOR3 vMyPos = GetPos();									// official vtable+256
	float fBestDistSq = fRange_ * fRange_;									// official v97 = a3*a3 (the range^2 gate; the caller passes 600 PvP / 800 PvE)
	CX2GameUnit* pBest = NULL;												// official v94
	D3DXVECTOR3 vBestPoint( 0.f, 0.f, 0.f );
	for ( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )						// official re-reads the count per iteration (round3a:408-411)
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );						// official game vtable+160
		if ( NULL == pUnit )
			continue;
		// official filter 2 (f_31F19790 = the in-unit-list validity check) = NO-OP here - our loop source IS the live unit list
		if ( pUnit->GetTeam() == GetTeam() )
			continue;														// official filter 3: the team chain; also excludes SELF (a unit is always on its own team - official has no explicit self-check)
		if ( pUnit->GetNowHp() <= 0.f )
			continue;														// official filter 4: the gage chain (our null-gage case returns 1 = the official gage-null-PASS quirk; byte-exact = NO GUSI_DIE check in this helper)
		// official filter 5: NPCs pass only if NCT_BASIC-class AND not invincible; non-NPC units (players) pass REGARDLESS of invincibility (A || (B && C) precedence)
		if ( CX2GameUnit::GUT_NPC == pUnit->GetGameUnitType() )
		{
			if ( CX2UnitManager::NCT_BASIC != static_cast<CX2GUNPC*>( pUnit )->GetClassType() )
				continue;
			if ( pUnit->GetInvincible() )
				continue;
		}
		// aim-point refinement: the nearest body-collision point to us across the candidate's whole collision set (the official nested walk == CollisionData::GetPointStart() inlined)
		D3DXVECTOR3 vPoint( 0.f, 0.f, 0.f );
		bool bHavePoint = false;
		BOOST_TEST_FOREACH( const CKTDXCollision::CollisionDataList*, pList, pUnit->GetCollisionListSet() )
		{
			if ( NULL == pList )
				continue;
			BOOST_TEST_FOREACH( CKTDXCollision::CollisionData*, pCol, *pList )
			{
				if ( NULL == pCol )
					continue;												// official checks m_bEnable NOT AT ALL (KOG quirk - disabled spheres still contribute; kept)
				const D3DXVECTOR3 vP = pCol->GetPointStart();				// == the official matrix block verbatim (the matrices-null edge: official reads a stale stack matrix vs ours (0,0,0) - degenerate-never-happens, documented)
				if ( false == bHavePoint || GetDistance( vMyPos, vP ) <= GetDistance( vMyPos, vPoint ) )
				{
					vPoint = vP;
					bHavePoint = true;
				}
			}
		}
		if ( false == bHavePoint )
			vPoint = pUnit->GetPos();										// official !v27 fallback
		const float fDistSq = GetDistance3Sq( vPoint, vMyPos );				// official v90 (3D dist^2 to the refined point)
		if ( fDistSq > fBestDistSq )
			continue;														// official filter 8: the running-nearest range gate (shrinks per qualified pick only)
		// forward half-space probe along the current line (official: the WHOLE angle section nests under the line lookup - no line data -> the candidate CANNOT qualify)
		CKTDGLineMap* pLineMap = g_pX2Game->GetLineMap();
		const CKTDGLineMap::LineData* pLine = ( NULL != pLineMap ) ? pLineMap->GetLineData( GetLastTouchLineIndex() ) : NULL;	// official game vtable+128+52 line map + unit vtable+244 current line index
		if ( NULL == pLine )
			continue;
		// spec section 5 R-2: the facing-signed probe step is implemented (the semantic intent). The 2015 decompile dead-stores the second sign write (v75 overwritten before any read, round3a:284-286) - unresolvable whether that is a preserved KOG bug or a Hex-Rays misallocation; the orchestrator's call = the facing-signed form.
		const float fSign = ( GetIsRight() ? FLT_32D2FF9C : FLT_32D3C998 );	// official vtable+312 facing + the sign constants (+0.1 / -0.1)
		const D3DXVECTOR3 vProbe = vMyPos + pLine->dirVector * fSign;		// official: the sign multiplies all 3 components (no magnitude scale)
		if ( fDistSq <= GetDistance3Sq( vPoint, vProbe ) )
			continue;														// enter iff the aim point is closer to one line-step ahead (facing) than to us = forward half-space along the line
		// slope-band bounds (official: upper = params[0] x hDistSq, lower = hDistSq x params[1])
		const D3DXVECTOR3 vCandPos = pUnit->GetPos();						// official uses the vtable+252 getter pair; INERT for us (the land pos differs from GetPos only in Y; hDistSq uses X/Z only)
		const float fHDistSq = ( vCandPos.x - vMyPos.x ) * ( vCandPos.x - vMyPos.x ) + ( vCandPos.z - vMyPos.z ) * ( vCandPos.z - vMyPos.z );
		float fUpper = pfParams_[0] * fHDistSq;								// 1.0472 x hDistSq (60deg rad)
		float fLower = fHDistSq * pfParams_[1];								// -0.17453 x hDistSq (10deg rad, negated at the official call site)
		if ( 1 == cMode_ )
		{
			fUpper += (float)DBL_32D2B940;									// official a8==1 band shift (DEAD for our only caller - a8=0)
			fLower -= (float)DBL_32D2B960;
		}
		// (official dead pre-check: upper >= 0 && lower <= 0 - always true with the shipped constants; comment-only for fidelity)
		// cone gate: |normalize(facingXZ) . normalize(toCandXZ)| <= params[2] (params[2] = 0.17453 = a REAL narrow band, NOT a no-op)
		D3DXVECTOR3 vFace( pLine->dirVector.x * fSign, 0.f, pLine->dirVector.z * fSign );
		D3DXVECTOR3 vTo( vPoint.x - vMyPos.x, 0.f, vPoint.z - vMyPos.z );
		D3DXVec3Normalize( &vFace, &vFace );
		D3DXVec3Normalize( &vTo, &vTo );
		const float fDot = D3DXVec3Dot( &vFace, &vTo );
		if ( !( -pfParams_[2] <= fDot && fDot <= pfParams_[2] ) )
			continue;
		// elevation slope band: pass iff lower <= signed(dy^2) <= upper
		const float fDy = vPoint.y - vMyPos.y;
		const float fSignedDySq = ( vMyPos.y > vPoint.y ) ? -( fDy * fDy ) : ( fDy * fDy );
		if ( !( fLower <= fSignedDySq && fSignedDySq <= fUpper ) )
			continue;
		// full pass -> the running-nearest select; the anchor = the nearest body-collision point (NOT the unit origin, NOT Bip01)
		fBestDistSq = fDistSq;
		pBest = pUnit;
		vBestPoint = vPoint;
	}
	if ( NULL == pBest )
		return false;
	*pvAnchorOut_ = vBestPoint;
	if ( NULL != pvEnemyPosOut_ )
		*pvEnemyPosOut_ = pBest->GetPos();									// officially the vtable+252 position; discarded by our caller too
	// official FPU return (v98/v95) DISCARDED by the caller - omitted (its vtable+208 getter chain feeds ONLY that return value, so it needs no port for this caller - spec section 5 R-4 note)
	return true;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_RecoverMPorDP( const float fValue_ )
{
	// official FORCE_NOVA/FORCE_FINGER recovery branch: formation (official vtable+1080 = gage byte 244) �넂 SetDPValue(DP + value*20) + "DP+ %d"; else MP += value + "MP+ %d" (f_324EB490 / f_31F1EF90; *20 = UI-DP scale ��� same constant as GetActualDPConsume)
	if ( 0.f >= fValue_ )
		return;

	if ( true == GetIsFormationMode() )
		SetDPValue( GetDPValue() + fValue_ * 20.f );	// official dbl_32D20D68 = 20.0
	else
		UpNowMp( fValue_ );

	D3DXVECTOR3 vPos = GetPos();
	vPos.y += 50.f;
	std::wstringstream strStream;
	strStream << ( ( true == GetIsFormationMode() ) ? L"DP+ " : L"MP+ " ) << (int)fValue_;
	if ( NULL != g_pData->GetPicCharBlue() )
		g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), vPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9023 VOID_IMPACT (official FM 0x324F5F20) ===
void CX2GUAdd::ANSI_SA_ATT_VOID_IMPACT_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.846f ) )	// official flt_32D883B4 = 0.846
	{
		D3DXVECTOR3 vSpawnPos( 0.f, 0.f, 0.f );

		CX2GameUnit* pTarget = NULL;
		ThirdPath_FindScanTargets( ThirdPath_GetScanScope( CX2SkillTree::SI_SA_ATT_VOID_IMPACT ), 180.f, &pTarget, 1, m_fVoidImpactDownAngle, m_fVoidImpactUpAngle );
		if ( NULL != pTarget )
			vSpawnPos = pTarget->GetBonePos( L"Bip01" );	// official: target bone pos (slot_187, GetPos fallback)
		else
		{

			vSpawnPos = GetPos() + GetDirVector() * ThirdPath_GetScanScope( CX2SkillTree::SI_SA_ATT_VOID_IMPACT ) * ( true == GetIsRight() ? 1.f : -1.f );
		}

		vSpawnPos.y += 80.f;

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Void_Impact_Attack", this, NULL, IsHyperState(), GetPowerRate(), -1.f, D3DXVECTOR3( 1, 1, 1 ), true, vSpawnPos );
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_ThirdPath_OneShotGroundPinned_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9028 REVERSE_STIGMA (official main FM 0x324F6740 + sub-212 0x324F92D0�넂f_324F8900) ===
bool CX2GUAdd::ThirdPath_StigmaMarkTarget( CX2GameUnit* pTarget_ )
{
	// official f_324F09E0: transferable-check (alive; desc "Invincible or named targets cannot be transferred" ��� boss-class NPCs rejected) �넂 clear old mark + store uid/type/pos/duration
	if ( NULL == pTarget_ )
		return false;
	if ( pTarget_->GetNowHp() <= 0.f || pTarget_->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
		return false;

	if ( true == pTarget_->GetInvincible() )
		return false;
	if ( CX2GameUnit::GUT_NPC == pTarget_->GetGameUnitType() )
	{
		CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pTarget_ );
		if ( true == pNPC->GetIsBosRaidNPC() || true == pNPC->GetIsMiddleBosRaidNPC() )
			return false;
		if ( CX2UnitManager::NCT_BASIC != pNPC->GetClassType() )
			return false;
	}

	ThirdPath_StigmaClearMark();

	float fDuration = 3.f;	// official value-type 1 = SA_EFFECTIVE_TIME (templet 9028: 3.0-17.5 by level; desc "Use Duration: 3 secs")
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA );
	if ( NULL != pSkillTemplet && NULL != GetUnit() )
	{
		const int iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA ) );
		const float fTempletTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iLevel );
		if ( 0.f < fTempletTime )
			fDuration = fTempletTime;
	}

	m_uidStigmaTarget		= pTarget_->GetUnitUID();
	m_iStigmaTargetType		= pTarget_->GetGameUnitType();
	m_vStigmaMarkPos		= pTarget_->GetPos();
	m_fStigmaMarkDuration	= fDuration;
	m_fStigmaMarkTimer		= fDuration;
	return true;
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_StigmaClearMark()
{
	// official f_324EFB80: destroy the debuff ES + reset uid/type/pos/duration + (my unit) refresh the slot UI

	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() && INVALID_EFFECTSET_HANDLE != m_hStigmaDebuffES )
		g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hStigmaDebuffES );
	m_hStigmaDebuffES		= INVALID_EFFECTSET_HANDLE;


	if ( 0 != m_uidStigmaTarget && NULL != g_pX2Game )
	{
		CX2GameUnit* pMarked = g_pX2Game->GetUnitByUIDAndType( m_uidStigmaTarget, m_iStigmaTargetType );
		if ( NULL != pMarked )
		{
			bool bSharedMark = false;
#ifdef X2_ADD_3RD_PATH
			const int iUnitNum = g_pX2Game->GetUnitNum();
			for ( int i = 0; i < iUnitNum; ++i )
			{
				CX2GameUnit* pOther = g_pX2Game->GetUnit( i );
				if ( NULL == pOther || pOther == static_cast<CX2GameUnit*>( this ) )
					continue;
				if ( CX2GameUnit::GUT_USER != pOther->GetGameUnitType() )
					continue;
				CX2GUUser* pOtherGU = static_cast<CX2GUUser*>( pOther );
				if ( NULL == pOtherGU->GetUnit() || CX2Unit::UC_ADD_DIABOLIC_ESPER != pOtherGU->GetUnit()->GetClass() )
					continue;
				CX2GUAdd* pOtherAdd = static_cast<CX2GUAdd*>( pOtherGU );
				if ( pOtherAdd->GetStigmaMarkTargetUID() == m_uidStigmaTarget &&
					pOtherAdd->GetStigmaMarkTargetType() == m_iStigmaTargetType )
				{
					bSharedMark = true;
					break;
				}
			}
#endif
			if ( false == bSharedMark )
				pMarked->EraseBuffTempletFromGameUnit( BTI_DEBUFF_REVERSE_STIGMA );
		}
	}

	m_uidStigmaTarget		= 0;
	m_iStigmaTargetType		= 0;
	m_vStigmaMarkPos		= D3DXVECTOR3( 0.f, 0.f, 0.f );
	m_fStigmaMarkDuration	= 0.f;
	m_fStigmaMarkTimer		= 0.f;


	if ( true == IsMyUnit() && NULL != g_pX2Game )
		g_pX2Game->UpdateSkillSlotUI();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_OnStigmaMarkFrameMove( const float fElapsedTime_ )
{
	// official 26576/26596 duration tick (official ticking site = the newslot_317 CommonFrameMove tick dispatcher; hosted in CX2GUAdd::CommonFrameMove per LIFE-2)
	if ( 0.f < m_fStigmaMarkTimer )
	{
		m_fStigmaMarkTimer -= fElapsedTime_;
		if ( 0.f >= m_fStigmaMarkTimer )
			ThirdPath_StigmaClearMark();
	}
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_ADE_REVERSE_STIGMA_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )	// official dbl_32D281A8 = 0.1 / flt_32D84C4C = 0.1
	{
		CX2GameUnit* pTarget = ThirdPath_FindScanTarget( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA );
		if ( NULL != pTarget )
		{

			int iLevel = 1;
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA );
			if ( NULL != pSkillTemplet && NULL != GetUnit() )
				iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ADE_REVERSE_STIGMA ) );

			D3DXVECTOR3 vTargetPos = pTarget->GetPos();
			pTarget->GetBonePos( L"Bip01", vTargetPos );	// official: the target's Bip01 bone (GetPos feet = the fallback)

			if ( true == ThirdPath_StigmaMarkTarget( pTarget ) )
			{
				// mark: debuff BFI 478 on the target @skill level (official target->vtable+428(478, level)) + Debuff ES spawned ON the target
				pTarget->SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_REVERSE_STIGMA, (UINT)iLevel );
				if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
				{
					CX2EffectSet::Handle hDebuff = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSetI_SA_ADE_REVERSE_STIGMA_Debuff", pTarget, NULL, IsHyperState(), GetPowerRate() );
					if ( INVALID_EFFECTSET_HANDLE != hDebuff )
						m_hStigmaDebuffES = hDebuff;
				}
			}
			else
			{
				// non-transferable target �넂 instant Forced Transfer damage (official: _Return at the target) + the mark CLEAR (official sub_324F0FB0 �넂 f_324EFB80)
				if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ADE_REVERSE_STIGMA_Return", GetPowerRate(), vTargetPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				ThirdPath_StigmaClearMark();
			}

			// both paths: the main hit (official: DamageEffect_ADE_REVERSE_STIGMA_Attack at the target pos)
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ADE_REVERSE_STIGMA_Attack", GetPowerRate(), vTargetPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			if ( NULL != g_pX2Game )
				g_pX2Game->UpdateSkillSlotUI();	// official: unconditional f_31F25BA0(game, 0) at the FM tail
		}
		else
			ThirdPath_StigmaClearMark();	// official: no target �넂 mark-clear (f_324F0FB0 chain)
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_ADE_REVERSE_STIGMA_RETURN_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )	// official sub-212 FM 0x324F92D0 �넂 f_324F8900 @0.1
	{
		CX2GameUnit* pTarget = NULL;
		if ( NULL != g_pX2Game && 0 != m_uidStigmaTarget )
			pTarget = g_pX2Game->GetUnitByUIDAndType( m_uidStigmaTarget, m_iStigmaTargetType );



		if ( NULL != pTarget && pTarget->GetNowHp() > 0.f && pTarget->GetGameUnitState() != CX2GameUnit::GUSI_DIE )
		{
			// FORCED TRANSFER: move the marked target to the recorded position (official f_320069F0 NPC force-move / f_31E93F20 player work-vector ��� simplified to the direct position set)
			if ( false == pTarget->GetInvincible() )	// C-TTDE-D6: the transfer = invincible-gated only; the damage below always runs
				pTarget->SetPositionOnLine( m_vStigmaMarkPos, pTarget->GetLastTouchLineIndex() );

			// Forced Transfer damage at the target bone
			D3DXVECTOR3 vTargetPos = pTarget->GetPos();
			pTarget->GetBonePos( L"Bip01", vTargetPos );
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ADE_REVERSE_STIGMA_Return", GetPowerRate(), vTargetPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
		ThirdPath_StigmaClearMark();
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_ADE_REVERSE_STIGMA_RETURN_EventProcess()
{
	// shared generic-cancel EP (official main-state EP 0x3250E010 + sub-212 EP 0x3250E250)
	if ( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( USI_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9030 GRAVITY_BURSTER (official FM 0x3250E840) ===
void CX2GUAdd::ANSI_SA_ADE_GRAVITY_BURSTER_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 1.1f ) )	// official flt_32D84CBC = 1.1
	{
		// official: formation + mutation stacks 1..6 �넂 consume 1 stack (f_32503FC0(this,-1)) + _Damage_Mutauion set; else the normal _Damage set
		const bool bMutation = ( true == GetIsFormationMode() && 0 < ThirdPath_ConsumeDPPoints( 1 ) );

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet(
				( true == bMutation ) ? L"EffectSet_SA_ADE_GRAVITY_BURSTER_Damage_Mutauion" : L"EffectSet_SA_ADE_GRAVITY_BURSTER_Damage",
				this, NULL, IsHyperState(), GetPowerRate() );
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_HA_ADE_PARANOIA_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 4.5f ) )	// official flt_32D87D44 = 4.5 (== the Final_Hit timing, Add_NasodRuler.lua:15782)
	{
		const int iLevel = ( NULL != GetUnit() ) ? max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_HA_ADE_PARANOIA, true ) ) : 1;	// official f_31E9B2A0 = the tree level, fallback 1

		// the hostile scan: self-contained here (the shared ThirdPath_FindScanTargets applies the facing cone UNCONDITIONALLY - the official helper takes the cone flag, 0 for PARANOIA = sweep ALL directions). radius 900 (flt_32D84C38), Y-band +/-180 (flt_32D87D40/32D2F1F8), cap 99999; the helper's filters: not-self + team-skip + alive + not-DIE + NPC NCT-basic-only (named/boss NPCs excluded - a hyper that can't paranoia a boss = official behavior)
		if ( NULL != g_pX2Game )
		{
			const D3DXVECTOR3 vMyPos = GetPos();
			const float fRadiusSq = 900.f * 900.f;
			int iRollIndex = 0;
			const int iUnitNum = g_pX2Game->GetUnitNum();
			for ( int i = 0; i < iUnitNum; ++i )
			{
				CX2GameUnit* pTarget = g_pX2Game->GetUnit( i );
				if ( NULL == pTarget || pTarget == this )
					continue;
				if ( pTarget->GetNowHp() <= 0.f || pTarget->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
					continue;
				if ( pTarget->GetTeam() == GetTeam() )
					continue;
				if ( CX2GameUnit::GUT_NPC == pTarget->GetGameUnitType() &&
					CX2UnitManager::NCT_BASIC != static_cast<CX2GUNPC*>( pTarget )->GetClassType() )
					continue;


				const D3DXVECTOR3 vTargetPos = pTarget->GetPos();
				const D3DXVECTOR3 vDelta = vTargetPos - vMyPos;
				if ( vDelta.x * vDelta.x + vDelta.y * vDelta.y + vDelta.z * vDelta.z > fRadiusSq )
					continue;

				const float fRoll = GetRandomFloat( iRollIndex++ );	// official slot_92 (0x31F0E4B0) == CX2GUUser::GetRandomFloat(iOffset) - the index increments ONLY per accepted target
				const UINT uiBuffID = ( fRoll >= 0.66f ) ? BFI_DEBUFF_PARANOIA_FIRE : ( fRoll >= 0.33f ) ? BFI_DEBUFF_PARANOIA_WATER : BFI_DEBUFF_PARANOIA_POISON;	// official dbl_32D87D38=0.66 / dbl_32D3C500=0.33 - a clean 1/3-1/3-1/3 split
				pTarget->SetBuffFactorToGameUnitByBuffFactorID( uiBuffID, (UINT)iLevel );	// official: the target's vtable+428; the level scales the buff, the roll gates the tier
			}
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
// === 9213 FORCE_NOVA (official StateStart 0x324E0080 + FM 0x325023C0) ===
void CX2GUAdd::ANSI_A_ADE_FORCE_NOVA_StateStart()
{
	CommonStateStart();
	m_bForceNovaHit = false;	// official this[26620] = 0
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_ADE_FORCE_NOVA_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.5f ) && true == m_bForceNovaHit )	// official flt_32D2C9E8 = 0.5 + hit-flag gate
	{
		float fValue = 0.f;	// official value-type 66 = SA_MP_RETAKE_ABS (templet 9213: 35-122 by level; desc "DP 35 Recovery / MP 35 Recovery")
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADE_FORCE_NOVA );
		if ( NULL != pSkillTemplet && NULL != GetUnit() )
		{
			const int iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ADE_FORCE_NOVA ) );
			fValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_RETAKE_ABS, iLevel );
		}
		ThirdPath_RecoverMPorDP( fValue );
		m_bForceNovaHit = false;
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9214 FORCE_FINGER (official FM 0x32502580) ===
void CX2GUAdd::ANSI_A_ADE_FORCE_FINGER_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.271f ) )	// official flt_32D88628 = 0.271
	{
		float fMPIntake = 0.f;	// official value-type 44 = SA_MP_INTAKE_ABS (templet 9214: 9-38 by level)
		float fMPRetake = 0.f;	// official value-type 66 = SA_MP_RETAKE_ABS (templet 9214: 35-122 by level)
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADE_FORCE_FINGER );
		if ( NULL != pSkillTemplet && NULL != GetUnit() )
		{
			const int iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ADE_FORCE_FINGER ) );
			fMPIntake = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_INTAKE_ABS, iLevel );
			fMPRetake = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_RETAKE_ABS, iLevel );
		}

		CX2GameUnit* aTargets[2] = { NULL, NULL };
		const int iTargetCount = ThirdPath_FindScanTargets( 700.f, 180.f, aTargets, 2 );	// official scan: radius flt_32D84CFC = 700, Y-band 짹180 (flt_32D2F1F8/-87D40), max 2 targets (official loop cap)
		for ( int i = 0; i < iTargetCount; ++i )
		{
			CX2GameUnit* pTarget = aTargets[i];
			if ( NULL == pTarget )
				continue;

			const D3DXVECTOR3 vTargetPos = pTarget->GetPos();

			// official: EffectSetI on self + _By_Target on the target (scale 1)
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
			{
				CX2EffectSet::Handle hFFSelfES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSetI_A_ADE_FORCE_FINGER", this, NULL, IsHyperState(), GetPowerRate() );	m_vecEffectSetToDeleteOnStateEnd.push_back( hFFSelfES );
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSetI_A_ADE_FORCE_FINGER_By_Target", pTarget, NULL, IsHyperState(), GetPowerRate() );
			}

			// Explosion damage at the target (official DamageEffect_A_ADE_FORCE_FINGER_Explosion, USED_SKILL_ID 9214)
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_A_ADE_FORCE_FINGER_Explosion", GetPowerRate(), vTargetPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			// MP drain on the target (players only ��� official f_31F05EB0(target, -intake); NPCs have no MP gage)
			if ( CX2GameUnit::GUT_USER == pTarget->GetGameUnitType() )
				static_cast<CX2GUUser*>( pTarget )->UpNowMp( -fMPIntake );

			// caster resource recovery (official: per target)
			ThirdPath_RecoverMPorDP( fMPRetake );


			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetMajorParticle() )
			{
				D3DXVECTOR3 vBeamPos = vTargetPos;	vBeamPos.y += 100.0f;	// official: the beam aim point is lifted +100 (dbl_32D21180)
				CKTDGParticleSystem::CParticleEventSequence* pBeamSeq = g_pX2Game->GetMajorParticle()->CreateSequence( this, L"ADE_FORCE_FINGER_PH01M", vBeamPos );	// all other args = defaults incl bNextFrame=false (LIVE) == official
				if ( NULL != pBeamSeq )
				{
					pBeamSeq->SetBlackHolePosition( GetBonePos( L"Bip01_L_Hand" ) );	// official stores the hand-bone attractor (PH01M's BlackHoleTime event steers the particles toward it)
					if ( false == GetIsRight() )	// official facing-left gate (vtable+312 == 0)
					{
						const D3DXVECTOR3 vPos = GetPos();
						pBeamSeq->SetAxisAngle( D3DXVECTOR3( vPos.x, -180.0f, vPos.z ) );	// official seq+708 store
						pBeamSeq->SetAddRotate( D3DXVECTOR3( vPos.x, -180.0f, vPos.z ) );	// official f_31B7F7F0 -> seq+672 (the -180 Y facing flip)
					}
				}
			}
		}
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::IsAddSkillKeyHeld()
{
	switch( m_eSpecialAttackKeyPressed )
	{
	case SAKP_A:	return m_InputData.pureA;
	case SAKP_S:	return m_InputData.pureS;
	case SAKP_D:	return m_InputData.pureD;
	case SAKP_C:	return m_InputData.pureC;
	case SAKP_Q:	return m_InputData.pureQ;
	case SAKP_W:	return m_InputData.pureW;
	case SAKP_E:	return m_InputData.pureE;
	case SAKP_R:	return m_InputData.pureR;
	default:		break;
	}
	return false;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::ThirdPath_DPCheckOrConsume( const float fOtherUnitRate_, const float fMyUnitRate_ )
{

	if ( true == GetIsFormationMode() )
	{
		if ( GetDPValue() <= fMyUnitRate_ )
			return false;
		UpNowDPValue( -fMyUnitRate_ );
		return true;
	}
	if ( GetDPValue() >= fOtherUnitRate_ )
	{
		UpNowDPValue( -fOtherUnitRate_ );
		return true;
	}
	return false;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
int CX2GUAdd::ThirdPath_ConsumeDPPoints( const int iMaxPoints_ )
{

	if( NULL == GetAddGageData() || false == GetIsFormationMode() )
		return 0;

	const int iConsumed = GetAddGageData()->ConsumeDPPoint( iMaxPoints_ );


	if ( 0 < iConsumed && 0.f < m_fMindCircleMpValue && 0.f < m_fMindCircleDpValue )
	{
		const bool	bFormation	= GetIsFormationMode();
		const float fRecovered	= (float)iConsumed * ( true == bFormation ? m_fMindCircleMpValue : m_fMindCircleDpValue );

		if ( true == bFormation )
			UpNowMp( fRecovered );
		else
			UpNowDPValue( fRecovered * 20.f );	// official dbl_32D20D68 = 20.0 (the UI-DP scale)

		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 50.f;
		std::wstringstream strStream;
		strStream << ( true == bFormation ? L"MP+ " : L"DP+ " ) << (int)fRecovered;
		if ( NULL != g_pData->GetPicCharBlue() )
			g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), vPos, GetDirVector(), CKTDGPicChar::AT_CENTER );
	}

	return iConsumed;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::CreateSpaceCrackField( const bool bSubProjectile_ )
{
	if( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() || NULL == g_pX2Game->GetEffectSet() )
		return;

	if( true == bSubProjectile_ )
		ThirdPath_DPCheckOrConsume( 10.f, 200.f );	// official flt_32D2C968=10 (other) / flt_32D334A0=200 (my)

	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Space_Crack_Performing", this, NULL, IsHyperState(), GetPowerRate() );

	const D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01" );
	g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this,
		( true == bSubProjectile_ ) ? L"DamageEffect_ATT_Space_Crack_Rupture_Sub_Projectile" : L"DamageEffect_ATT_Space_Crack_Rupture_Main_Projectile",
		GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
}
#endif
#ifdef X2_ADD_3RD_PATH
// === 9024 VOID_FIELD (official: slot FM@0.01 spawns field+loop ES; LOOP drains DP 500/s (my-unit) / 25/s (other) and ends at >=1s on key-release / DP-out / 5s cap; END stops ES + terminates field) ===
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_StartFuture()
{
	CommonStateStartFuture();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )	// official flt_32D2AA30 = 0.01
	{
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			m_hVoidFieldDE = g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this, L"DamageEffect_ATT_Void_Field", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			CX2EffectSet::Handle hLoopES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Void_Field", this, NULL, IsHyperState(), GetPowerRate() );
			if( INVALID_EFFECTSET_HANDLE != hLoopES )
				m_hVoidFieldLoopES = hLoopES;
		}
		m_bVoidFieldEndBurst = false;	// official byte 26512 = 0
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fVoidFieldLoopTime = 0.f;
		StateChange( ANSI_SA_ATT_VOID_FIELD_LOOP );
	}
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_LOOP_FrameMove()
{
	// official 0x32500F10: DP>0 refresh (anti-tamper protected-value touch ��� no-op on our plain-float DP) + [this+0x5270]-=45.0
	// (that member = Y of a CX2GUUser D3DXVECTOR3 written by CommonFrameMove from dash/knockback sources ��� NOT a DP/MP drain; semantics unresolved, DOCUMENTED SKIP)
	m_fVoidFieldLoopTime += m_fElapsedTime;
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_LOOP_EventProcess()
{
	if( 1.f <= m_fVoidFieldLoopTime )	// official dbl_32D20D50 = 1.0 (min time before release-check)
	{
		const bool bMaxTime		= ( 5.f <= m_fVoidFieldLoopTime );		// official dbl_32D2A8B0 = 5.0 (cap)
		const bool bKeyReleased	= ( false == IsAddSkillKeyHeld() );		// official f_31E93560 key-page released
		const bool bDPOk		= ThirdPath_DPCheckOrConsume( m_fElapsedTime * 25.f, m_fElapsedTime * 500.f );	// official dbl_32D3D478=25 (other) / dbl_32D34F08=500 (my), per frame
		if( bMaxTime || bKeyReleased || false == bDPOk )
			StateChange( ANSI_SA_ATT_VOID_FIELD_END );
	}
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_END_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_bVoidFieldEndBurst = true;	// official byte 26512 = 1

		if( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() && INVALID_EFFECTSET_HANDLE != m_hVoidFieldLoopES )
			g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hVoidFieldLoopES );
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			// official: resolve field handle �넂 set 2015-only CEffect member 586 = -0.03 (collapse/time-reverse param; no counterpart in our 2014 CEffect).
			// DOCUMENTED DEVIATION: DamageEffect_ATT_Void_Field has DAMAGE_TIME=9999 (never dies naturally) �넂 destroy to guarantee cleanup
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hVoidFieldDE, false );
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_VOID_FIELD_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( USI_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9025 MAXIMUM_STRIKE (official FM@1.151: 6-way barrage at Y+150 with 60째 angle steps + DP-point-scaled power) ===
void CX2GUAdd::ANSI_SA_ATT_MAXIMUM_STRIKE_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 1.151f ) )	// official flt_32D89DE4 = 1.151
	{

		ThirdPath_MaxStrikeTeardown( false );

		if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			const D3DXVECTOR3 vPos = GetPos();
			for( int iAngle = 0; iAngle < 360; iAngle += 60 )	// official 6-iteration loop (v2 = 0..360 step 60)
			{
				CX2DamageEffect::CEffectHandle hBullet = g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this,
					L"DamageEffect_ATT_Maximum_Strike_Bullet_Projectile", GetPowerRate(),
					D3DXVECTOR3( vPos.x, vPos.y + 150.f, vPos.z ),	// official dbl_32D3C260 = +150 Y (per-iter z-offset math IDA-mangled �넂 documented-simplified)
					D3DXVECTOR3( 0.f, (float)iAngle, 0.f ),
					GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				if( INVALID_DAMAGE_EFFECT_HANDLE != hBullet )
					m_vecMaxStrikeHandles.push_back( hBullet );
			}
		}

		m_fMaxStrikePower = GetPowerRate();
		const int iDPPoints = ThirdPath_ConsumeDPPoints( 3 );	// official f_325042B0(this, 3)
		if( iDPPoints > 0 )
			m_fMaxStrikePower *= ( iDPPoints * 0.1f + 1.f );	// official dbl_32D281A8 = 0.1 per point

		float fCapValue = 60.f;	// official floor 60 (dbl_32D34028 / flt_32D35068)
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE );
		if( NULL != pSkillTemplet && NULL != GetUnit() )
		{
			const int iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ATT_MAXIMUM_STRIKE ) );
			const float fTempletValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iLevel );	// official value-type 1
			if( fTempletValue > 60.f )
				fCapValue = fTempletValue;
		}

		m_fMaxStrikeWindowDuration = m_fMaxStrikeWindowRemain = fCapValue;
		m_uidMaxStrikeTarget = 0;
		m_iMaxStrikeTargetType = 0;

		if( IsMyUnit() )
			g_pX2Game->UpdateSkillSlotUI();
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_MAXIMUM_STRIKE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( USI_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_MaxStrikeRecordTarget( CX2DamageManager::DamageData* pDamageData_ )
{
	// official f_324EFC30 (the AttackResultByType tail call, gated on the window this[6618] > 0): record the defender's UID64 + game-unit type into the homing record
	if ( NULL == pDamageData_ || 0.f >= m_fMaxStrikeWindowRemain )
		return;
	CX2GameUnit* pTarget = pDamageData_->optrDefenderGameUnit.GetObservable();
	if ( NULL == pTarget )
		return;
	m_uidMaxStrikeTarget	= pTarget->GetUnitUID();
	m_iMaxStrikeTargetType	= (int)pTarget->GetGameUnitType();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_MaxStrikeFireOnHit( CX2DamageManager::DamageData* pDamageData_ )
{
	// DESIGN-INFERENCE (marked): the OFFICIAL fire driver = the KPT command-word machine (newslot_327 case 38, gated on queue-non-empty) - KPT-bound and asm-unresolvable (zero direct/vcall callers recoverable; section 49a/49d settled). Our adaptation = spec option A "fire-on-hit": each attack hit while the barrage window is live pops one placeholder ball -> spawns the REAL damaging homing bullet at it -> attaches the recorded target's lock-on fields -> destroys the placeholder; queue emptied -> the full teardown.

	if ( 0.f >= m_fMaxStrikeWindowRemain || m_vecMaxStrikeHandles.empty() )
		return;
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )
		return;

	CX2DamageEffect::CEffectHandle hBall = m_vecMaxStrikeHandles.front();
	m_vecMaxStrikeHandles.erase( m_vecMaxStrikeHandles.begin() );	// official f_32503B90: pop the queue FRONT + shift

	D3DXVECTOR3 vFirePos = GetPos();
	CX2DamageEffect::CEffect* pBall = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( hBall );
	if ( NULL != pBall )
		vFirePos = pBall->GetPos();									// fire from the popped placeholder ball's live position (the follow tick keeps them glued to the player + Y+150)
	else
		vFirePos.y += 150.f;										// dead handle fallback (official dbl_32D3C260 = +150 Y)

	CX2DamageEffect::CEffect* pBullet = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_ATT_Maximum_Strike_Bullet_Attack", m_fMaxStrikePower,
		vFirePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );	// official: power = the DP-pip-scaled capture (this[6620]); name image-verified @0x32D89930
	if ( NULL != pBullet && 0 != m_uidMaxStrikeTarget )
	{
		// the homing attach (official f_324F8EC0: user -> the uid64 fields; NPC -> the npc-uid field).
		if ( CX2GameUnit::GUT_NPC == m_iMaxStrikeTargetType || CX2GameUnit::GUT_USER == m_iMaxStrikeTargetType )
		{
			CX2DamageEffect::LockOnData& kLock = pBullet->GetLockOnData();
			// DESIGN-INFERENCE: force LOT_UID_VECTOR - the templet parses LOCK_ON_TYPE.LOT_UID_VECTOR_AFTER_ATTACK_IN_RANGE (= 8, a 2015 lua enum) but our 2014 engine's lock-on switch (X2DamageEffect.cpp:2736) has NO case for 8 (our enum ends at LOT_UID_BONE_POS = 7) so the parsed type never homes; LOT_UID_VECTOR (= 4) is the engine-supported UID homing (the dynamo-retarget precedent ThirdPath_DynamoLockRetarget). The templet's CURVE_SPEED 50 / range / ONLY_FRONT fields stay as parsed.
			kLock.m_LockOnType = CX2DamageEffect::LOT_UID_VECTOR;
			if ( CX2GameUnit::GUT_NPC == m_iMaxStrikeTargetType )
			{
				kLock.m_LockOnNPCUID	= static_cast<int>( m_uidMaxStrikeTarget );
				kLock.m_bLockOnUserUnit	= false;
			}
			else
			{
				kLock.m_LockOnUnitUID	= m_uidMaxStrikeTarget;
				kLock.m_bLockOnUserUnit	= true;
			}
		}
	}
	g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( hBall, true );	// consume the placeholder

	if ( m_vecMaxStrikeHandles.empty() )
		ThirdPath_MaxStrikeTeardown( false );	// official: queue emptied -> f_32503980(this,0) full teardown
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_OnMaxStrikeFrameMove( const float fElapsedTime_ )
{
	// official newslot_317 window branch: if (this[6618] > 0) { tick down; if expired -> f_32503980(this,1) teardown+refund; else f_324F8DC0 = reposition every queued ball to the player's Bip01 + Y+150 (the placeholders follow the player) }
	if ( 0.f >= m_fMaxStrikeWindowRemain )
		return;
	m_fMaxStrikeWindowRemain -= fElapsedTime_;
	if ( 0.f < m_fMaxStrikeWindowRemain )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{

			for ( size_t i = 0; i < m_vecMaxStrikeHandles.size(); ++i )
			{
				CX2DamageEffect::CEffect* pBall = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_vecMaxStrikeHandles[i] );
				if ( NULL != pBall )
				{
					D3DXVECTOR3 vBallPos = pBall->GetPos();
					vBallPos.y = GetPos().y + 150.f;
					pBall->SetPos( vBallPos );
				}
			}
		}
		return;
	}
	ThirdPath_MaxStrikeTeardown( true );	// window expired -> the official f_32503980(this,1) = teardown + the unfired-ball refund
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_MaxStrikeTeardown( const bool bRefundUnfired_ )
{
	// official f_32503980: destroy the remaining queue handles + clear the queue + zero the window pair + clear the homing record + zero the power; arg==1 (expiry-with-unfired-balls) ALSO refunds unfiredCount x 25.0 (official dRam32d3d478) via the shared recovery branch (formation -> DP x20 UI-scale + "DP+ %d"; else MP + "MP+ %d")
	const int iUnfired = (int)m_vecMaxStrikeHandles.size();
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		for ( size_t i = 0; i < m_vecMaxStrikeHandles.size(); ++i )
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_vecMaxStrikeHandles[i], true );
	}
	m_vecMaxStrikeHandles.clear();
	m_fMaxStrikeWindowDuration	= 0.f;
	m_fMaxStrikeWindowRemain	= 0.f;
	m_uidMaxStrikeTarget		= 0;
	m_iMaxStrikeTargetType		= 0;
	m_fMaxStrikePower			= 0.f;
	if ( true == bRefundUnfired_ && iUnfired > 0 )
		ThirdPath_RecoverMPorDP( (float)iUnfired * 25.f );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9026 STARDUST_SHOWER (official: duration = SA_EFFECTIVE_TIME + DP-points횞SA_DAMAGE_TIME; rupture field + meteor mesh spawn at Y+700; meteors every 0.1s at the mesh's "spwan_Dummy0N" frame positions, N cycling 1..10) ===
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_StartFuture()
{
	CommonStateStartFuture();
	XSkinMeshReadyInBackground( L"elsword_SA_rageCutter_spwanPoint.X" );	// Mesh_ATT_Stardust_Shower_Drop underlying mesh (official StateInit preload, truncated-name list skipped per 짠42b precedent)
	m_fStardustLoopTime = 0.f;

	float fDuration = 2.5f;	// official value-type 1 = SA_EFFECTIVE_TIME (lua templet 9026: 2.5 all levels)
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ATT_STARDUST_SHOWER );
	int iLevel = 1;
	if( NULL != pSkillTemplet && NULL != GetUnit() )
	{
		iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ATT_STARDUST_SHOWER ) );
		const float fTempletTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iLevel );
		if( 0.f < fTempletTime )
			fDuration = fTempletTime;
	}
	const int iDPPoints = ThirdPath_ConsumeDPPoints( 3 );	// official f_325042B0(this, 3) ��� consumes up to 3 DP points to extend the shower
	if( iDPPoints > 0 )
	{
		float fPerPointTime = 0.33f;	// official value-type 78 = SA_DAMAGE_TIME (lua templet 9026: 0.33 all levels)
		if( NULL != pSkillTemplet )
		{
			const float fTempletPerPoint = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_TIME, iLevel );
			if( 0.f < fTempletPerPoint )
				fPerPointTime = fTempletPerPoint;
		}
		fDuration += ( (float)iDPPoints * fPerPointTime );
	}
	m_fStardustDuration = fDuration;

	m_vStardustSavedNowSpeed = m_PhysicParam.nowSpeed;
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_StateStart()
{
	m_fStardustSpawnAccum = 1.f;	// official this[6630] = 1.0 (first LOOP frame fires immediately)
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 700.f;	// official dbl_32D2ECF8 = 700
		m_hStardustFieldDE = g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this, L"DamageEffect_ATT_Stardust_Shower_Rupture", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
	CommonStateStart();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fStardustLoopTime = 0.f;
		StateChange( ANSI_SA_ATT_STARDUST_SHOWER_LOOP );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StartFuture()
{
	CommonStateStartFuture();
	m_fStardustLoopTime = 0.f;

	m_PhysicParam.nowSpeed = m_vStardustSavedNowSpeed;
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StateStart()
{
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
	{
		CX2EffectSet::Handle hAttackES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Stardust_Shower_Attack", this, NULL, IsHyperState(), GetPowerRate() );
		if( INVALID_EFFECTSET_HANDLE != hAttackES )
			m_hStardustLoopES = hAttackES;
	}
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 700.f;	// official dbl_32D2ECF8 = 700
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,
			L"Mesh_ATT_Stardust_Shower_Drop", vPos, GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
			m_hStardustMesh = pMeshInst->GetHandle();
	}
	m_iStardustDummyCounter = 1;	// official this[6633] = 1
	CommonStateStart();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_LOOP_FrameMove()
{
	CommonFrameMove();

	m_fStardustLoopTime += m_fElapsedTime;
	m_fStardustSpawnAccum += m_fElapsedTime;
	if( 0.1f <= m_fStardustSpawnAccum )	// official dbl_32D281A8 = 0.1 (meteor interval)
	{
		m_fStardustSpawnAccum = 0.f;
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() && NULL != g_pX2Game->GetMajorXMeshPlayer() )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hStardustMesh, true );	// official f_31BD8E60(handle, 1) live-check
			if( NULL != pMeshInst )
			{
				WCHAR wszDummyName[32] = {0,};
				_snwprintf( wszDummyName, 31, L"spwan_Dummy0%d", m_iStardustDummyCounter );	// official "spwan_Dummy0%d" (KOG typo 'spwan' preserved)
				wszDummyName[31] = 0;
				m_iStardustDummyCounter = ( m_iStardustDummyCounter < 10 ) ? ( m_iStardustDummyCounter + 1 ) : 1;	// official cycles 1..10

				D3DXVECTOR3 vDummyPos( 0.f, 0.f, 0.f );
				if( pMeshInst->GetBonePos( wszDummyName, vDummyPos ) )	// official f_31F8F140 dummy-pos lookup
				{
					g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this, L"DamageEffect_ATT_Stardust_Shower_Ball", GetPowerRate(),
						vDummyPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				}
			}
		}
	}
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_LOOP_EventProcess()
{
	if( m_fStardustLoopTime >= m_fStardustDuration )	// official 0x324DF5C0: stateTime >= captured duration �넂 END
		StateChange( ANSI_SA_ATT_STARDUST_SHOWER_END );
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StateEnd()
{
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetMajorXMeshPlayer() )
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hStardustMesh );	// official f_31BD8D50
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_END_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 1.f ) )
	{

		if( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() && INVALID_EFFECTSET_HANDLE != m_hStardustLoopES )
			g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hStardustLoopES );
		if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			// DOCUMENTED DEVIATION (same as VOID_FIELD END): official sets the 2015-only CEffect collapse member �넂 destroy to guarantee cleanup
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hStardustFieldDE, false );
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( USI_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_SA_ATT_STARDUST_SHOWER_END_StateEnd()
{
	if( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		m_hStardustFieldDE = INVALID_DAMAGE_EFFECT_HANDLE;	// official this[6631] = -1
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_3RD_PATH
// === 9212 SPACE_CRACK (official: slot FM@0.31 spawns crack field Main projectile + ES; MOVE�넄WAIT toggle while X/Z held; END on skill-key release / DP<200(my)|<10(other) / flag off) ===
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_StartFuture()
{
	CommonStateStartFuture();
	m_bSpaceCrackActive = true;	// official this[26442] = 1

	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.31f ) )	// official flt_32D88E4C = 0.31
	{
		m_bSpaceCrackFieldOn = true;	// official this[26441] = 1

		m_fSpaceCrackPower = 0.f;	// official this[26444] = value-type 38 (skill power-rate curve at level)
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ATT_SPACE_CRACK );
		if( NULL != pSkillTemplet && NULL != GetUnit() )
		{
			const int iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ATT_SPACE_CRACK ) );
			m_fSpaceCrackPower = pSkillTemplet->GetSkillPowerRateValue( iLevel );
		}

		if( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			CX2EffectSet::Handle hCrackES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Space_Crack", this, NULL, IsHyperState(), GetPowerRate() );
			if( INVALID_EFFECTSET_HANDLE != hCrackES )
				m_hSpaceCrackES = hCrackES;
		}
		CreateSpaceCrackField( false );	// official f_32501B70(this, 0) ��� Main projectile
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( ANSI_A_ATT_SPACE_CRACK_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_MOVE_EventProcess()
{

	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();	// official: the key-manager swap check first (armed by the slot SSF)
	const bool bKeyReleased = ( false == IsAddSkillKeyHeld() );
	const bool bDPDepleted = ( true == GetIsFormationMode() ) ? ( GetDPValue() < 200.f ) : ( GetNowMp() < 10.f );

	if( false == m_bSpaceCrackActive )
	{
		StateChange( ANSI_A_ATT_SPACE_CRACK_END );
	}
	else if( false == bKeyReleased && false == bDPDepleted )
	{
		// still holding + resources OK -> the MOVE->WAIT toggle rides the FORWARD movement key (official dir-gate: vtable+312 facing ? pureRight : pureLeft)
		const bool bForwardKey = ( true == GetIsRight() ) ? ( true == m_InputData.pureRight ) : ( true == m_InputData.pureLeft );
		if ( false == bForwardKey )
			StateChange( ANSI_A_ATT_SPACE_CRACK_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	else
	{
		StateChange( ANSI_A_ATT_SPACE_CRACK_END );
	}

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_WAIT_EventProcess()
{

	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();
	const bool bKeyReleased = ( false == IsAddSkillKeyHeld() );
	const bool bDPDepleted = ( true == GetIsFormationMode() ) ? ( GetDPValue() < 200.f ) : ( GetNowMp() < 10.f );

	if( false == m_bSpaceCrackActive )
	{
		StateChange( ANSI_A_ATT_SPACE_CRACK_END );
	}
	else if( false == bKeyReleased && false == bDPDepleted )
	{
		// still holding + resources OK -> the WAIT->MOVE toggle rides the FORWARD key going NOT-held (official mirror)
		const bool bForwardKey = ( true == GetIsRight() ) ? ( true == m_InputData.pureRight ) : ( true == m_InputData.pureLeft );
		if ( true == bForwardKey )
			StateChange( ANSI_A_ATT_SPACE_CRACK_MOVE );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	else
	{
		StateChange( ANSI_A_ATT_SPACE_CRACK_END );
	}

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_END_StartFuture()
{
	CommonStateStartFuture();
	m_bSpaceCrackActive = false;	// official this[26442] = 0
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_END_StateStart()
{
	CommonStateStart();
	m_bSpaceCrackFieldOn = false;	// official this[26441] = 0
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ANSI_A_ATT_SPACE_CRACK_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( USI_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
// === 9029 MOONLIGHT_RHAPSODY (official FM@1.3: formation-mode DP-pip consume �넂 buff BFI_BUFF_MOONLIGHT_RHAPSODY 476 at skill level + power = skill power-rate value, 횞0.66 when DP consumed) ===
void CX2GUAdd::ANSI_SA_ADE_MOONLIGHT_RHAPSODY_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )	// official flt_32D84CB8 = 1.3
	{
		m_bMoonlightActive = false;		// official 26600
		m_fMoonlightTimer = 0.f;		// official 26604
		m_fMoonlightPower = 0.f;		// official 26608
		m_bMoonlightDPConsumed = false;	// official 26612

		bool bDPConsumed = false;


		if( 0 < ThirdPath_ConsumeDPPoints( 1 ) )
		{
			bDPConsumed = true;
		}

		int iLevel = 1;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ADE_MOONLIGHT_RHAPSODY );
		if( NULL != pSkillTemplet && NULL != GetUnit() )
			iLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ADE_MOONLIGHT_RHAPSODY ) );

		SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_MOONLIGHT_RHAPSODY, iLevel );	// official vtable+428(this, 476, level)

		float fPower = 0.f;	// official: value-table row 38 (skill power-rate curve) [level-1]
		if( NULL != pSkillTemplet )
			fPower = pSkillTemplet->GetSkillPowerRateValue( iLevel );
		if( true == bDPConsumed )
			fPower *= 0.66f;	// official dbl_32D87D38 = 0.66

		m_bMoonlightActive = true;
		m_fMoonlightPower = fPower;
		m_bMoonlightDPConsumed = bDPConsumed;
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_ADE_MOONLIGHT_RHAPSODY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		StateChange( USI_WAIT );
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )

	CommonEventProcess();
}
#endif
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_OnMoonlightWaveFrameMove( const float fElapsedTime_ )
{
	if( false == m_bMoonlightActive )	// official byte 26600 (armed by the cast FM at the 1.3s point)
		return;

	m_fMoonlightTimer += fElapsedTime_;	// official +26604 (+344 elapsed seconds added per frame)

	if( 0.f < m_fMoonlightPower && m_fMoonlightPower < m_fMoonlightTimer )	// official +26608 = the pulse PERIOD in seconds (the value captured at cast; x0.66 when a DP pip was consumed)
	{
		if( HaveThisBuff( BTI_BUFF_MOONLIGHT_RHAPSODY ) )	// official f_31FA7710(this, 438) - self-buff still up
		{
			if( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
			{
				if( INVALID_EFFECTSET_HANDLE != m_hMoonlightWaveES )	// official +26616: destroy the old zone before spawning the new
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hMoonlightWaveES );	// official f_31F87910

				m_hMoonlightWaveES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_MOONLIGHT_RHAPSODY_DEBUFF",
					this, this, false, 1.f, -1.f, D3DXVECTOR3( 1.f, 1.f, 1.f ), false, GetPos() );	// official f_31F890E0 15-arg form: owner=this, trace=this, hyper=0, power=1.0, lifetime=-1.0 sentinel, scale 1, no custom pos, pos arg = GetPos()
			}
		}
		else
		{
			// self-buff expired -> disarm (official does NOT destroy the last wave handle here - the zone lives out its own lifetime)
			m_bMoonlightActive = false;
			m_fMoonlightPower = 0.f;
			m_bMoonlightDPConsumed = false;
		}
		m_fMoonlightTimer = 0.f;	// official: timer reset on every pulse check
	}
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_AddComboChain_EventProcess()
{

	const int iNowStateID = GetNowStateID();
	const bool bChainKeyPressed = ( ANSI_ATT_COMBO_NASOD_ARMOR_Z == iNowStateID || ANSI_ATT_COMBO_NASOD_ARMOR_ZZ == iNowStateID || ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ == iNowStateID )
		? ( true == m_InputData.oneZ ) : ( true == m_InputData.oneX );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		std::map<int,int>::const_iterator it = m_mapThirdPathChain.find( GetNowStateID() );
		if( it != m_mapThirdPathChain.end() )
		{
			StateChange( it->second );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
	}
	else if( true == bChainKeyPressed && m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ATTComboNasodArmorLandingEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_Z );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( true == m_InputData.oneZ && NULL != GetUnit() &&
			 ( CX2Unit::UC_ADD_NASOD_RULER == GetUnitClass() || CX2Unit::UC_ADD_TIME_TRACER == GetUnitClass() || CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnitClass() ) &&
			 true == GetIsFormationMode() &&
			 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[1].stateChange )
	{
		StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_X );
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( true == m_InputData.oneX && NULL != GetUnit() &&
			 ( CX2Unit::UC_ADD_NASOD_RULER == GetUnitClass() || CX2Unit::UC_ADD_TIME_TRACER == GetUnitClass() || CX2Unit::UC_ADD_DIABOLIC_ESPER == GetUnitClass() ) &&
			 true == GetIsFormationMode() &&
			 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[1].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[1].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ATTComboNasodArmorXXXLoopStateStart()
{
	// official SS 324DE140: play the loop sound + CommonStateStart (official stores the handle at byte 26340; ours = the shared m_pLoopSound slot via PlayLoopSound - the XXZZ/DJXX loop sounds share it and these combo states are mutually exclusive) //
	PlayLoopSound( L"ADD_Panzer_Ready01.ogg", true, false );
	CommonStateStart();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ATTComboNasodArmorXXXLoopEventProcess()
{
	// official EP 324E8A30: hold-release channel - (stateTime >= 1.0 && X released) || stateTime >= 2.0 -> XXX_END (official 198; dbl_32D20D50=1.0 / dbl_32D20D80=2.0 byte-verified); air->17+y-30; the lua cancel windows are 99999 = data-dead both sides //
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( ( m_FrameDataNow.unitCondition.fStateTime >= 1.0f && false == m_InputData.pureX ) || m_FrameDataNow.unitCondition.fStateTime >= 2.0f )
	{
		StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_XXX_END );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ATTComboNasodArmorXXXLoopStateEnd()
{
	// official SE 324DE170: free the loop sound + CommonStateEnd //
	StopLoopSound();
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ATTComboNasodArmorZZZZStartEventProcess()
{
	// official EP 324FFCD0: animEnd -> reset the loop counter (the official KPT re-register block stores 0) -> ZZZZ_LOOP; NO input buffer (the loop's Z-hold is buffered in the LOOP's own EP); cancels; air->17+y-30 //
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_iComboLoopCount = 0;
		StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP );
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ATTComboNasodArmorZZZZLoopEventProcess()
{
	// official EP 324FFEE0: animEnd -> ++count -> DP > 0 ? (count < 4 && buffered flag0 -> loop again : -> ZZZZ_END) : -> ZZZZ_END (the self-loop StateChange re-runs CommonStateStartFuture -> flag0 cleared -> a FRESH Z press per loop, matching official); buffer: oneZ window[0] (the ZZZZ_LOOP table has NO EVENT_PROCESS_TIME -> the {0,9999,9999} default = always-open); cancels (99999 = data-dead both sides); air->17+y-30 //
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		++m_iComboLoopCount;
		if ( GetDPValue() > 0.f )	// official: gage DP > 0.0 (dbl_32D20458) - gate only, no consume here
		{
			if ( m_iComboLoopCount < 4 && true == m_FrameDataFuture.stateParam.bEventFlagList[0] )
				StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP );
			else
				StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END );
		}
		else
		{
			StateChange( ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END );
		}
	}
	else if( true == m_InputData.oneZ &&
			 m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
#endif
#ifdef X2_ADD_3RD_PATH
void CX2GUAdd::ATTComboZZXXFrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.124f ) )
	{
		if ( GetDPValue() >= 2.0f )
		{
			SetDPValue( GetDPValue() - 2.0f );
			if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
			{
				if ( INVALID_EFFECTSET_HANDLE != m_hATTComboZZXXAttackES )
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hATTComboZZXXAttackES );
				m_hATTComboZZXXAttackES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Combo_ZZXX_Attack", this, NULL, false, GetPowerRate() );
				if ( INVALID_EFFECTSET_HANDLE != m_hATTComboZZXXES )
					g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hATTComboZZXXES );
				m_hATTComboZZXXES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ATT_Combo_ZZXX", this, NULL, false, GetPowerRate() );
			}
		}
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_Init()
{

	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Cycle_SPARK02.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_Drone_Activator_Dummy01.X" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"ADD_psionic_generate_MESH_01.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XMeshReadyInBackground( L"ADD_psionic_generate_Cristal_01.Y" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"ColorBallRed.dds" );
	TextureReadyInBackground( L"GuideArrow01.dds" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_02.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_03.Y" );

	XSkinMeshReadyInBackground( L"ADD_Drone_Activator_Generate.X" );
	XSkinMeshReadyInBackground( L"ADD_Installation_Drone_Generate_Dummy01.X" );
	XMeshReadyInBackground( L"ADD_Cube_Map01_03.Y" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	TextureReadyInBackground( L"AN_Formation_TRACE.tga" );
	SoundReadyInBackground( L"ADD_ATT_DroneStart01.ogg" );
	SoundReadyInBackground( L"AddVoice_Pylon_Dynamo01.ogg" );
}
void CX2GUAdd::ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_START_Init()
{

	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	XMeshReadyInBackground( L"ADD_Panzer_Buster_Aiming001.Y" );
	XMeshReadyInBackground( L"ADD_Panzer_Buster_Aiming002.Y" );
	XMeshReadyInBackground( L"ADD_Panzer_Buster_Aiming003.Y" );
	XSkinMeshReadyInBackground( L"ADD_Panzer_Buster_Start_Dummy_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Panzer_Buster_Aiming_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Panzer_Buster_Aiming_RED.X" );
	TextureReadyInBackground( L"AN_Formation_TRACE.tga" );
	XMeshReadyInBackground( L"ADD_Cube_Map01_03.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_04.Y" );
	TextureReadyInBackground( L"ColorBallRed.dds" );
	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Panzer_Buster_Fire_001.X" );
	XSkinMeshReadyInBackground( L"ADD_Panzer_Buster_Fire_002.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_PanzerResetAim()
{
	// official f_324EAF50: destroy the prior aim REDLINE DE + the aiming-BG EffectSet + zero the charge/angle
	if ( NULL != g_pX2Game )
	{
		if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hPanzerAimDE && NULL != g_pX2Game->GetDamageEffect() )
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hPanzerAimDE, false );
		if ( INVALID_EFFECTSET_HANDLE != m_hPanzerAimBG && NULL != g_pX2Game->GetEffectSet() )
			g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hPanzerAimBG );
	}
	m_fPanzerChargeTime		= 0.f;
	m_fPanzerAimAngle		= 0.f;
	m_bPanzerAimSpawned		= false;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_PanzerApplyAimAngle()
{
	if ( INVALID_DAMAGE_EFFECT_HANDLE == m_hPanzerAimDE || NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )
		return;
	CX2DamageEffect::CEffect* pAim = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hPanzerAimDE );
	if ( NULL == pAim || NULL == pAim->GetMainEffect() )
		return;
	pAim->GetMainEffect()->SetRotateDegree( D3DXVECTOR3( 0.f, GetRotateDegree().y, m_fPanzerAimAngle ) );
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_START_StateStartFuture()
{
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );

	if ( NULL != m_hPanzerReadySound )	// never stack a stale loop
	{
		m_hPanzerReadySound->Stop();
		m_hPanzerReadySound = NULL;
	}
	m_hPanzerReadySound = PlaySound( L"ADD_Panzer_Ready01.ogg", false, true );	// bLoop = true (verified in data069.kom)

	CommonStateStartFuture();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_START_StateStart()
{
	// official sub_324ED7A0: reset prior aim handles (f_324EAF50) + base parse (the ready sound moved OUT of here - it plays LOOPED from the START SSF per ATMM-9)
	ThirdPath_PanzerResetAim();
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ANSI_SA_AAT_PANZER_BUSTER_AIMING );
	}
	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_AIMING_StateStart()
{
	if ( false == m_bPanzerAimSpawned )
	{
		m_bPanzerAimSpawned = true;
		if ( NULL != g_pX2Game )
		{
			if ( NULL != g_pX2Game->GetDamageEffect() )
			{
				D3DXVECTOR3 vPos = GetPos();
				GetBonePos( L"EF01", vPos );
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Panzer_Buster_Aiming_REDLINE", GetPowerRate(),
					vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				if ( NULL != pEffect )
				{
					m_hPanzerAimDE = pEffect->GetHandle();
					ThirdPath_PanzerApplyAimAngle();	// official: mesh [275]=0.0, [276]=facing-rotY, [277]=the aim angle at spawn
				}
			}
			if ( NULL != g_pX2Game->GetEffectSet() )
				m_hPanzerAimBG = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_AAT_Panzer_Buster_Aiming_BG", this, NULL, IsHyperState(), GetPowerRate() );
		}
	}

	if ( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable			= true;
		m_GameCameraOffset.fCameraDistance	= 220.f;
		m_GameCameraOffset.fLookatDistance	= 550.f;
	}
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_AIMING_FrameMove()
{
	m_fPanzerChargeTime += m_fElapsedTime;
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_AIMING_EventProcess()
{

	if ( ( false == IsAddSkillKeyHeld() && 0.5f < m_fPanzerChargeTime ) || 3.f < m_fPanzerChargeTime )
	{
		StateChange( ANSI_SA_AAT_PANZER_BUSTER_FIRE );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneUp == true, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneDown == true, ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_StateStartFuture()
{
	PlayLoopSound( L"ADD_Panzer_AIM01.ogg", false, false );	// our PlayLoopSound = StopLoopSound-first + play + store (the official free-then-play shape)
	CommonStateStartFuture();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_FrameMove()
{
	m_fPanzerChargeTime += m_fElapsedTime;
	if ( 30.f <= m_fPanzerAimAngle )
	{
		m_fPanzerAimAngle = 30.f;
	}
	else
	{
		m_fPanzerAimAngle += m_fElapsedTime * 50.f;	// official rate dbl_32D2B940 = 50
		ThirdPath_PanzerApplyAimAngle();	// official f_324EAFB0: re-apply the aim angle onto the REDLINE DE's mesh
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();	// official EP sub_324E9880: the key-manager swap check first (the key manager is armed by the START SSF)

	if ( false == IsAddSkillKeyHeld() || 3.f < m_fPanzerChargeTime )
	{
		StateChange( ANSI_SA_AAT_PANZER_BUSTER_FIRE );
	}
	else if ( true == m_FrameDataFuture.stateParam.bEventFlagList[0] &&
			  ( m_fEventTime[0].stateChange < m_pXSkinAnimFuture->GetNowAnimationTime() || true == m_pXSkinAnimFuture->IsAnimationEnd() ) )
	{
		StateChange( ANSI_SA_AAT_PANZER_BUSTER_AIMING );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if ( false == m_InputData.oneUp &&
			  m_fEventTime[0].keyInputStart <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
			  m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_StateStartFuture()
{
	PlayLoopSound( L"ADD_Panzer_AIM01.ogg", false, false );
	CommonStateStartFuture();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_FrameMove()
{
	m_fPanzerChargeTime += m_fElapsedTime;
	if ( -30.f >= m_fPanzerAimAngle )
	{
		m_fPanzerAimAngle = -30.f;
	}
	else
	{
		m_fPanzerAimAngle -= m_fElapsedTime * 50.f;
		ThirdPath_PanzerApplyAimAngle();
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();	// official EP sub_324E9970 (mirror of DEG_UP's)

	if ( false == IsAddSkillKeyHeld() || 3.f < m_fPanzerChargeTime )
	{
		StateChange( ANSI_SA_AAT_PANZER_BUSTER_FIRE );
	}
	else if ( true == m_FrameDataFuture.stateParam.bEventFlagList[0] &&
			  ( m_fEventTime[0].stateChange < m_pXSkinAnimFuture->GetNowAnimationTime() || true == m_pXSkinAnimFuture->IsAnimationEnd() ) )
	{
		StateChange( ANSI_SA_AAT_PANZER_BUSTER_AIMING );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if ( false == m_InputData.oneDown &&
			  m_fEventTime[0].keyInputStart <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
			  m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateStartFuture()
{
	StopLoopSound();
	if ( NULL != m_hPanzerReadySound )
	{
		m_hPanzerReadySound->Stop();
		m_hPanzerReadySound = NULL;
	}
	CommonStateStartFuture();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateStart()
{
	if ( NULL != g_pX2Game )
	{
		if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hPanzerAimDE && NULL != g_pX2Game->GetDamageEffect() )
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hPanzerAimDE, false );
		if ( INVALID_EFFECTSET_HANDLE != m_hPanzerAimBG && NULL != g_pX2Game->GetEffectSet() )
			g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hPanzerAimBG );
		m_hPanzerAimDE = INVALID_DAMAGE_EFFECT_HANDLE;
		m_hPanzerAimBG = INVALID_EFFECTSET_HANDLE;
	}
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateEnd()
{
	if ( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable			= false;
		m_GameCameraOffset.fLookatDistance	= 0.f;
	}
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_FIRE_FrameMove()
{

	if ( m_pXSkinAnim->EventTimerOneshot( 0.444f ) )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			CX2EffectSet::Handle hFireES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_AAT_Panzer_Buster_Fire", this, NULL, IsHyperState(), GetPowerRate() );
			CX2EffectSet::EffectSetInstance* pInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hFireES );
			if ( NULL != pInst )
				pInst->m_vRotationDegree = D3DXVECTOR3( 0.f, 0.f, m_fPanzerAimAngle );
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PANZER_BUSTER_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PSIONIC_GENERATOR_Init()
{

	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_02.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	XSkinMeshReadyInBackground( L"ADD_psionic_generate_DronMesh_01.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"ADD_Cycle_SPARK02.X" );
	XMeshReadyInBackground( L"ADD_MM_Effect_Color01.Y" );
	XMeshReadyInBackground( L"ADD_psionic_generate_Cristal_01.Y" );
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XMeshReadyInBackground( L"ADD_MM_Effect_Color02.Y" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	TextureReadyInBackground( L"aisha_active_guillotinePress_lightBlur.dds" );
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PSIONIC_GENERATOR_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		// official: all-around unit count (no facing cone), radius 1000 (squared-compare), capped at 10
		int iNearby = 0;
		if ( NULL != g_pX2Game )
		{
			const D3DXVECTOR3 vMyPos = GetPos();
			const float fRadiusSq = 1000.f * 1000.f;
			const int iUnitNum = g_pX2Game->GetUnitNum();
			for ( int i = 0; i < iUnitNum && iNearby < 10; ++i )
			{
				CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );

				if ( NULL == pGameUnit ||
					 pGameUnit->GetNowHp() <= 0.f ||
					 pGameUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE ||
					 true == pGameUnit->GetInvincible() )
					continue;
				const D3DXVECTOR3 vDelta = pGameUnit->GetPos() - vMyPos;
				const float fDistSq = vDelta.x * vDelta.x + vDelta.y * vDelta.y + vDelta.z * vDelta.z;
				if ( fDistSq <= fRadiusSq )
					++iNearby;
			}
		}


		float fSizeScale = 1.f + 0.1f * (float)iNearby;
		if ( fSizeScale > 2.0f )	fSizeScale = 2.0f;
		float fDmgScale = 1.f + 0.05f * (float)iNearby;
		if ( fDmgScale > 1.5f )	fDmgScale = 1.5f;

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{
			// damage scale carried via the power rate (m_fPowerRate); the set's explosion DamageEffect picks it up
			CX2EffectSet::Handle hES = g_pX2Game->GetEffectSet()->PlayEffectSet(
				L"EffectSet_SI_SA_AAT_Psionic_Generator_Start", this, NULL, IsHyperState(), GetPowerRate() * fDmgScale );
			// official applies the size scale to the spawned instance's visual elements
			CX2EffectSet::EffectSetInstance* pInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hES );
			if ( NULL != pInst )
				pInst->m_vScale = D3DXVECTOR3( fSizeScale, fSizeScale, fSizeScale );
		}
	}
	CommonFrameMove();
}
void CX2GUAdd::ANSI_SA_AAT_PSIONIC_GENERATOR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_PHANTOM_SEEKER_Init()
{

	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_02.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_Phantom_Seeker_Dummy_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Phantom_Seeker_Trace_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Phantom_Seeker_MESH_01.X" );
	XMeshReadyInBackground( L"ADD_MM_tech001.Y" );
	TextureReadyInBackground( L"ADD_Effect_Triangle01.tga" );
	TextureReadyInBackground( L"WhiteCircle.dds" );
	TextureReadyInBackground( L"Black_Light.dds" );
	TextureReadyInBackground( L"GlowBall_G01.tga" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
}
void CX2GUAdd::ANSI_SA_AAT_PHANTOM_SEEKER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_A_AAT_RUSHING_DRONE_DYNAMO_Init()
{

	XSkinMeshReadyInBackground( L"ADD_MM_Drone_Activator_Dummy01.X" );
	XSkinMeshReadyInBackground( L"ADD_Rushing_Drone_MESH_01.X" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.X" );
	TextureReadyInBackground( L"AN_Formation_TRACE.tga" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	XMeshReadyInBackground( L"ADD_Cube_Map01_03.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_AAT_RUSHING_DRONE_DYNAMO_StateStart()
{

	m_abRushingDroneFlag[1] = ( GetIsFormationMode() && 0 < ThirdPath_ConsumeDPPoints( 1 ) );
	m_abRushingDroneFlag[0] = ThirdPath_DynamoLockValidOrClear();
	CommonStateStart();
}
void CX2GUAdd::ANSI_A_AAT_RUSHING_DRONE_DYNAMO_FrameMove()
{

	if ( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			const int iCount = ( !m_abRushingDroneFlag[0] && m_abRushingDroneFlag[1] ) ? 2 : 1;
			for ( int i = 0; i < iCount; ++i )
			{
				D3DXVECTOR3 vPos = GetPos();
				GetBonePos( L"Bip01", vPos );
				CX2DamageEffect::CEffect* pDrone = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AAT_Rushing_Drone", GetPowerRate(),
					vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				if ( NULL == pDrone )
					continue;
				if ( NULL != pDrone->GetMainEffect() )
				{
					D3DXVECTOR3 vMeshRot = pDrone->GetMainEffect()->GetRotateDegree();
					vMeshRot.y += 30.f * (float)i;	// official: the per-index +30-degree yaw spread
					pDrone->GetMainEffect()->SetRotateDegree( vMeshRot );
				}
				if ( m_abRushingDroneFlag[0] )
					ThirdPath_DynamoLockRetarget( pDrone );
			}
		}
	}
	else if ( m_pXSkinAnim->EventTimerOneshot( 0.6f ) && m_abRushingDroneFlag[0] && m_abRushingDroneFlag[1] )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			D3DXVECTOR3 vPos = GetPos();
			GetBonePos( L"Bip01", vPos );
			CX2DamageEffect::CEffect* pDrone = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AAT_Rushing_Drone", GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( NULL != pDrone )
				ThirdPath_DynamoLockRetarget( pDrone );
		}
	}
	CommonFrameMove();
}
void CX2GUAdd::ANSI_A_AAT_RUSHING_DRONE_DYNAMO_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_Init()
{

	XSkinMeshReadyInBackground( L"ADD_PULSE_GATLING_Dron_Mesh_01.X" );
	XSkinMeshReadyInBackground( L"ADD_psionic_generate_MESH_01.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.X" );
	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Cycle_SPARK02.X" );
	XMeshReadyInBackground( L"ADD_Cube_Map01_03.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XMeshReadyInBackground( L"ADD_psionic_generate_Cristal_01.Y" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	TextureReadyInBackground( L"ADD_ShockWave.dds" );
	TextureReadyInBackground( L"GuideArrow01.dds" );

	XSkinMeshReadyInBackground( L"ADD_PULSE_GATLING_FollowDrone_Mesh.X" );
	XSkinMeshReadyInBackground( L"ADD_Installation_Drone_Generate_Dummy01.X" );
	TextureReadyInBackground( L"eve_thousandsOfStars_gearLight.DDS" );
	SoundReadyInBackground( L"ADD_ATT_DroneStart01.ogg" );
	SoundReadyInBackground( L"AddVoice_Pylon_Dynamo01.ogg" );
}
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_Init()
{

	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.X" );
	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Shooting_Chaser_Projectile_01.X" );
	XSkinMeshReadyInBackground( L"ADD_Shooting_Chaser_Attack_BIM.X" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_04.Y" );
	XMeshReadyInBackground( L"ADD_Cube_Map01_03.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	TextureReadyInBackground( L"AN_Formation_TRACE.tga" );
	TextureReadyInBackground( L"eve_thousandsOfStars_gearLight.DDS" );

	XSkinMeshReadyInBackground( L"ADD_PULSE_GATLING_FollowDrone_Mesh.X" );
	XSkinMeshReadyInBackground( L"ADD_Installation_Drone_Generate_Dummy01.X" );
	SoundReadyInBackground( L"ADD_ATT_DroneStart01.ogg" );
	SoundReadyInBackground( L"AddVoice_Pylon_Dynamo01.ogg" );
}
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_Init()
{

	XSkinMeshReadyInBackground( L"ADD_Ultimate_Fury_AttackLine01.X" );
	XSkinMeshReadyInBackground( L"ADD_Ultimate_Fury_Attack_Mesh.X" );
	TextureReadyInBackground( L"AN_Formation_TRACE.tga" );
	TextureReadyInBackground( L"eve_thousandsOfStars_gearLight.DDS" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );

	XSkinMeshReadyInBackground( L"ADD_Ultimate_Fury_Follow_Mesh.X" );
	XSkinMeshReadyInBackground( L"ADD_Installation_Drone_Generate_Dummy01.X" );
	SoundReadyInBackground( L"ADD_ATT_DroneStart01.ogg" );
	SoundReadyInBackground( L"AddVoice_Pylon_Dynamo01.ogg" );
}
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_STAR_FALL_Init()
{

	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_02.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	XSkinMeshReadyInBackground( L"ADD_Star_Fall_Attack_Mesh.X" );
	XSkinMeshReadyInBackground( L"ADD_Star_Fall_Start_Display01.X" );
	TextureReadyInBackground( L"ColorBallRed.dds" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XMeshReadyInBackground( L"ADD_Star_Fall_Attack_BIM_01.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_04.Y" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	XSkinMeshReadyInBackground( L"aisha_active_energySpurt_circle.X" );
	XMeshReadyInBackground( L"ADD_Star_Fall_Attack_BIM_02.Y" );
	XMeshReadyInBackground( L"ADD_DOOMS_DAY_Grow01.Y" );
	TextureReadyInBackground( L"aisha_active_guillotinePress_lightBlur.dds" );
	XMeshReadyInBackground( L"ADD_Star_Fall_Explode_006.Y" );
	TextureReadyInBackground( L"ADD_ShockWave.dds" );

	XSkinMeshReadyInBackground( L"ADD_Star_Fall_FollowDrone_Mesh.X" );
	XSkinMeshReadyInBackground( L"ADD_Installation_Drone_Generate_Dummy01.X" );
	SoundReadyInBackground( L"ADD_ATT_DroneStart01.ogg" );
	SoundReadyInBackground( L"AddVoice_Pylon_Dynamo01.ogg" );
}
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_STAR_FALL_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_Init()
{

	XSkinMeshReadyInBackground( L"ADD_Cube_Map01_01.X" );
	XSkinMeshReadyInBackground( L"ADD_MM_Drone_Activator_Dummy01.X" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_04.Y" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	XMeshReadyInBackground( L"ADD_particle_02.Y" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"ADD_Rushing_Drone_MESH_01.X" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	XSkinMeshReadyInBackground( L"ADD_Cycle_SPARK02.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XSkinMeshReadyInBackground( L"ADD_Cycle_SPARK02.X" );
	TextureReadyInBackground( L"ColorBallRed.dds" );
	TextureReadyInBackground( L"GuideArrow01.dds" );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )	// official flt_32D2AF74
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{

			const bool bLockValid = ThirdPath_DynamoLockValidOrClear();	// one side-effecting check, reused for the suffix + the retarget (officially f_324F0690 runs at both points -- identical outcome)
			std::wstring wstrName = L"DamageEffect_ADD_Dividing_Missile_Dynamo";
			if ( GetIsFormationMode() && 0 < ThirdPath_ConsumeDPPoints( 1 ) )	// official: variant <=> pip available AND consumed (review-fix: consume-result gates; was mutation-gate + non-gating consume)
			{
				wstrName += L"_Mutation";
			}
			if ( true == bLockValid )
				wstrName += L"_DA";		// official: the Drone-Activator LOCK variant
			D3DXVECTOR3 vPos = GetPos();
			GetBonePos( L"Bip01", vPos );
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrName.c_str(), GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( true == bLockValid )
				ThirdPath_DynamoLockRetarget( pEffect );	// the lock-retarget carries the x1.2 + the homing fields
		}
	}
	CommonFrameMove();
}
void CX2GUAdd::ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_A_AMM_FLICK_DISC_DYNAMO_StateStart()
{
	CommonStateStart();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_A_AMM_FLICK_DISC_DYNAMO_Init()
{

	XMeshReadyInBackground( L"ADD_Cube_Map01_03.Y" );
	XSkinMeshReadyInBackground( L"ADD_MM_Drone_Activator_Dummy01.X" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_01.Y" );
	XMeshReadyInBackground( L"ADD_MM_ROCKON_04.Y" );
	XSkinMeshReadyInBackground( L"ADD_MM_tech002.x" );
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	TextureReadyInBackground( L"ADD_Effect_Triangle01.tga" );
	TextureReadyInBackground( L"Black_Light.dds" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	XSkinMeshReadyInBackground( L"ADD_MAGNETRON_DYNAMO_MESH.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_AMM_FLICK_DISC_DYNAMO_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		{

			const bool bLockValid = ThirdPath_DynamoLockValidOrClear();
			const bool bMutation = ( GetIsFormationMode() && 0 < ThirdPath_ConsumeDPPoints( 1 ) );	// official: 2 discs <=> a pip available AND consumed (review-fix: consume-result gates; was formation+mutation gate + non-gating pip consume = free 2-disc at 0 pips)
			const int iCount = bMutation ? 2 : 1;

			std::vector<CX2EffectSet::Handle> vecDiscES;	// official member vector 26268/26272 (the lifetime = this spawn; ours is local)
			for ( int i = 0; i < iCount; ++i )
			{
				WCHAR wszName[128];
				if ( bMutation )
					swprintf( wszName, L"EffectSet_SI_A_AMM_Flick_Disc_Fire_Lock_Mutation%d", i );	// official aEffectsetSiAAm_0
				else
					swprintf( wszName, L"EffectSet_SI_A_AMM_Flick_Disc_Fire_Lock" );				// official aEffectsetSiAAm
				float fPower = GetPowerRate();
				if ( bLockValid )
					fPower *= 1.2f;		// official: the spawn power x1.2 when lock-valid (dbl_32D3BCE0)
				CX2EffectSet::Handle hDisc = g_pX2Game->GetEffectSet()->PlayEffectSet( wszName, this, NULL, false, fPower );
				if ( bLockValid && INVALID_EFFECTSET_HANDLE != hDisc )
					vecDiscES.push_back( hDisc );
			}

			if ( bLockValid && NULL != g_pX2Game->GetDamageEffect() )
			{
				BOOST_TEST_FOREACH( CX2EffectSet::Handle, hDisc, vecDiscES )
				{
					CX2EffectSet::EffectSetInstance* pInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hDisc );
					if ( NULL == pInst )
						continue;
					BOOST_TEST_FOREACH( CX2EffectSet::EffectInstance*, pElem, pInst->m_vecpEffectInstance )
					{
						if ( NULL == pElem || NULL == pElem->m_pEffectData )
							continue;
						if ( pElem->m_pEffectData->m_EffectName != L"DamageEffect_AMM_Flick_Disc_Dynamo_Damage_Sub" )
							continue;
						CX2DamageEffect::CEffect* pSubDE = g_pX2Game->GetDamageEffect()->GetInstance( pElem->m_hDamageEffect );
						if ( NULL != pSubDE )
							ThirdPath_DynamoLockRetarget( pSubDE );
					}
				}
			}
		}
	}
	CommonFrameMove();
}
void CX2GUAdd::ANSI_A_AMM_FLICK_DISC_DYNAMO_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_Init()
{

	XSkinMeshReadyInBackground( L"ADD_PHASE_SHIFT_SPARK01.X" );
	XSkinMeshReadyInBackground( L"ADD_Countdown_Bomb_Mesh_01.X" );
	TextureReadyInBackground( L"AN_Formation_TRACE.tga" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XSkinMeshReadyInBackground( L"ADD_Cycle_SPARK02.X" );
	TextureReadyInBackground( L"ColorBallRed.dds" );
	TextureReadyInBackground( L"ADD_Light001.tga" );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{

			std::wstring wstrName = L"DamageEffect_AMM_Countdown_Bomb_Projectile";
			if ( GetIsFormationMode() && 0 < ThirdPath_ConsumeDPPoints( 1 ) )	// official: variant <=> pip available AND consumed (review-fix: consume-result gates)
			{
				wstrName += L"_Mutation";
			}
			D3DXVECTOR3 vPos = GetPos();
			GetBonePos( L"Bip01", vPos );
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrName.c_str(), GetPowerRate(),
				vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( ThirdPath_DynamoLockValidOrClear() )
				ThirdPath_DynamoLockRetarget( pEffect );
		}
	}
	CommonFrameMove();
}
void CX2GUAdd::ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_HA_AMM_APOCALYPSE_Init()
{

	XSkinMeshReadyInBackground( L"Motion_AMM_APOCALYPSE.x" );
	XMeshReadyInBackground( L"ADD_APOCALYPSE_Cube001.Y" );
	XMeshReadyInBackground( L"ADD_APOCALYPSE_Cube002.Y" );
	XMeshReadyInBackground( L"ADD_APOCALYPSE_Cube003.Y" );
	TextureReadyInBackground( L"ADD_Cube_Map01.tga" );
}
void CX2GUAdd::ANSI_HA_AMM_APOCALYPSE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_HA_AMM_APOCALYPSE_FrameMove()
{

	if ( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{

		if ( ThirdPath_IsApocalypseSummonActive() )
		{
			// RECAST (the marker buff is still live = a summon is already out): unleash the final attack instead of a second summon (official f_324EF3D0 path); the cast anim plays out and exits via the EP (no forced StateChange - official does not cut the state)
			ThirdPath_ApocalypseFinishSummon();	// C-FACT-3: clear moved into the finisher's found-branch
		}
		else
		{
			// FRESH CAST
			m_bApocalypseSummoned		= true;
			m_bApocalypseFinisherDone	= false;

			// official: buff factor applied unconditionally (all clients, not IsMyUnit-gated), level = the caster's Apocalypse skill level (min 1)
			int iSkillLevel = 1;
			if ( NULL != GetUnit() )
			{
				iSkillLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_HA_AMM_APOCALYPSE ) );
			}
			SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_APOCALYPSE, iSkillLevel );

			if( true == g_pX2Game->IsHost() )
			{
				D3DXVECTOR3 vPos = GetPos();
				if( true == GetIsRight() )
					vPos += GetDirVector() * 300.f;		// official dbl_32D33A90 = 300.0
				else
					vPos -= GetDirVector() * 300.f;

				int iHyperMode = GetHyperModeUsed()? 1 : 0;
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_SI_HA_APOCALYPSE, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID(), false, CX2Room::TN_NONE, CX2GUNPC::NCT_CHANGESTAGE_REMAIN  );
			}


			if ( IsMyUnit() )
				g_pX2Game->UpdateSkillSlotUI();		// official f_31F25BA0 post-spawn skill-UI refresh
		}
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_ApocalypseFinishSummon()
{
	// official f_324EF3D0: find my alive Apocalypse summon (NPC NUI 1604, owner==me) �넂 force APOCALYPSE_FINISH_ATTACK (its StateStart repositions it in front of the owner; DYING_END finishes it)
	if ( true == m_bApocalypseFinisherDone || NULL == g_pX2Game )
		return;

	const int iUnitNum = g_pX2Game->GetUnitNum();
	for ( int i = 0; i < iUnitNum; ++i )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if ( NULL == pUnit || CX2GameUnit::GUT_NPC != pUnit->GetGameUnitType() )
			continue;
		CX2GUNPC* pNPC = static_cast<CX2GUNPC*>( pUnit );
		if ( NULL == pNPC->GetNPCUnitInfoPtr() || CX2UnitManager::NUI_SI_HA_APOCALYPSE != pNPC->GetNPCUnitInfoPtr()->m_templet.m_nNPCUnitID )
			continue;
		if ( pNPC->GetNowHp() <= 0.f || pNPC->GetGameUnitState() == CX2GameUnit::GUSI_DIE )
			continue;
		if ( pNPC->GetOwnerGameUnitUID() != GetUnitUID() )
			continue;

		pNPC->StateChange_LUA( "APOCALYPSE_DIE_ATTACK", true );
		m_bApocalypseFinisherDone = true;

		ThirdPath_ApocalypseClearMarker();
		break;
	}
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_ApocalypseClearMarker()
{
	// official f_324E08E0: reset the finisher flag + remove the marker buff + refresh the slot UI
	m_bApocalypseSummoned		= false;
	m_bApocalypseFinisherDone	= false;
	if ( IsMyUnit() )
		EraseBuffTempletFromGameUnit( BTI_BUFF_APOCALYPSE );
	if ( IsMyUnit() && NULL != g_pX2Game )
		g_pX2Game->UpdateSkillSlotUI();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::ThirdPath_IsApocalypseSummonActive() const
{
	// official f_31FA7710(this, 345): scan the unit buff-factor list for the Apocalypse marker templet (BTI_BUFF_APOCALYPSE=345; the factor BFI_BUFF_APOCALYPSE=369 links to it) ��� const-safe (HaveThisBuff is non-const); iteration mirror X2GUEL.cpp:2005
	const std::vector<CX2BuffFactorPtr>& vecBuff = GetBuffFactorList();
	for( size_t i = 0; i < vecBuff.size(); ++i )
	{
		if( vecBuff[i] && vecBuff[i]->GetBuffTempletID() == BTI_BUFF_APOCALYPSE )
			return true;
	}
	return false;
}
#endif
#ifdef X2_ADD_2ND_PATH
#ifdef X2_ADD_2ND_PATH
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::ThirdPath_IsInstallFamilySkill( const int iSkillID_ ) const
{
	// official f_324E0860: 9015 || (9019..9022)
	return ( CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR == iSkillID_
		|| ( CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING <= iSkillID_ && CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL >= iSkillID_ ) );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::ThirdPath_IsMarkedInstallSkill( const int iSkillID_ ) const
{
	// official slot_290 (0x324E0890)
	return ( 0 != iSkillID_ && m_iFactoryMarkedSkillID == iSkillID_ );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_FactoryModeClear()
{
	// official f_324EAC50(&marker, 1): marker=0 + phase=2 + wait-flag=1 + modeEntered=0 + skill-slot UI refresh + destroy the drone handle
	m_iFactoryMarkedSkillID	= 0;
	m_iFactoryPhase			= 2;
	m_bFactoryFlagWait		= true;
	m_bFactoryModeEntered	= false;
	if ( NULL != g_pX2Game )
	{
		g_pX2Game->UpdateSkillSlotUI();		// official f_31F25BA0

		if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
			g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillSlotOverlay();

		if ( NULL != g_pX2Game->GetDamageEffect() && INVALID_DAMAGE_EFFECT_HANDLE != m_hFactoryDroneDE )
			g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hFactoryDroneDE, false );
		m_hFactoryDroneDE = INVALID_DAMAGE_EFFECT_HANDLE;
	}

	ThirdPath_UltimateFuryClearSet();

}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
CX2GameUnit* CX2GUAdd::ThirdPath_DynamoLockTargetValid()
{
	// official f_324EEAE0: lock-not-set -> NULL; resolve by the stored UID (NPC -> GetNPCUnitByUID(uid-lo); user -> GetUserUnitByUID(uid64)); return it iff HP > 0 AND not invincible (target vtable+320)
	if ( 0 == m_uidDynamoLockTarget )
		return NULL;
	if ( NULL == g_pX2Game )
		return NULL;

	CX2GameUnit* pTarget = ( true == m_bDynamoLockIsNPC )
		? static_cast<CX2GameUnit*>( g_pX2Game->GetNPCUnitByUID( static_cast<int>( m_uidDynamoLockTarget ) ) )
		: static_cast<CX2GameUnit*>( g_pX2Game->GetUserUnitByUID( m_uidDynamoLockTarget ) );

	if ( NULL == pTarget )
		return NULL;
	if ( pTarget->GetNowHp() <= 0.f || true == pTarget->GetInvincible() )
		return NULL;
	return pTarget;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_DynamoLockClear()
{
	// official f_324EA900: destroy the lock ES + reset 26112/invalid, 26116(marker)/-1, 26120/-1, 26124/-1, 26128/0, 26132/-1.0 (+26136/1250 -- skipped, see the member comment: m_fFactoryDroneSpeed is the SHARED passive-loaded member that also drives the factory drone follow)
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() && INVALID_EFFECTSET_HANDLE != m_hDynamoLockES )
		g_pX2Game->GetEffectSet()->DestroyInstanceHandle( m_hDynamoLockES );
	m_hDynamoLockES			= INVALID_EFFECTSET_HANDLE;
	m_hInstall9015DE		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_uidDynamoLockTarget	= 0;
	m_bDynamoLockIsNPC		= false;
	m_fDynamoLockRemainTime	= -1.f;
	m_bDynamoLockArmed		= false;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
bool CX2GUAdd::ThirdPath_DynamoLockValidOrClear()
{
	// official f_324F0690: lock-not-set -> false; target valid -> true; else the teardown + false
	if ( 0 == m_uidDynamoLockTarget )
		return false;
	if ( NULL != ThirdPath_DynamoLockTargetValid() )
		return true;
	ThirdPath_DynamoLockClear();
	return false;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_DynamoLockTimeout( const float fElapsedTime_ )
{
	// official f_324EA940 (called from newslot_317, right after the factory tick f_324EED30): count the lock down; on expiry -> the teardown
	if ( 0.f < m_fDynamoLockRemainTime )
	{
		m_fDynamoLockRemainTime -= fElapsedTime_;
		if ( m_fDynamoLockRemainTime <= 0.f )
			ThirdPath_DynamoLockClear();
	}
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_DynamoLockRetarget( CX2DamageEffect::CEffect* pEffect )
{
	// official f_324EA9C0: write the DE's lock-on fields at the lock target (user -> the uid64 fields; NPC -> the npc-uid field) + curve speed 15 + the mesh dir-speed = the shared 1250 member + power x1.2
	if ( NULL == pEffect )
		return;

	CX2DamageEffect::LockOnData& kLock = pEffect->GetLockOnData();
	kLock.m_LockOnType = CX2DamageEffect::LOT_UID_VECTOR;	// official DE+1880 = 4
	if ( true == m_bDynamoLockIsNPC )
	{
		kLock.m_LockOnNPCUID	= static_cast<int>( m_uidDynamoLockTarget );	// official DE+1912 = the uid-lo dword
		kLock.m_bLockOnUserUnit	= false;
	}
	else
	{
		kLock.m_LockOnUnitUID	= m_uidDynamoLockTarget;						// official DE+1904/1908 = the uid64
		kLock.m_bLockOnUserUnit	= true;
	}
	kLock.m_fCurveSpeed = 15.f;	// official DE+1916 = flt_32D84C48 = 15.0 (image-verified)
	// official secondary writes SKIPPED (the member identities are unresolved from the dumps): DE+1928 = 0.1 (m_fTimeUnlockFromStart?), DE+1940 = 0, DE+1976 = -1, DE+1396 = 0.4
	pEffect->SetPowerRate( pEffect->GetPowerRate() * 1.2f );	// official DE+412 x= 1.2 (dbl_32D3BCE0) -- "Drones fired to the marked target does 1.2 times the damage"
	if ( NULL != pEffect->GetMainEffect() )
		pEffect->GetMainEffect()->SetDirSpeed( m_fFactoryDroneSpeed );	// official mesh+1124 = +26136
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_DynamoLockReAimOrbits()
{
	// official f_324F7C20: per live magnetron orbit: resolve -> the main mesh's Dummy02 bone pos -> create a NEW DamageEffect_SA_APT_Magnetron_Dynamo_Projectile there (power = the stored cast power; rotation = the unit's) -> retarget at the lock -> consume the old orbit; then fire-ready = 0 + destroy every remaining orbit + clear the vector
	if ( 0 == m_uidDynamoLockTarget )	// defensive -- the callers gate on the valid lock (f_324F0690)
		return;
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )
		return;

	vector<CX2DamageEffect::CEffectHandle>::iterator vIt = m_MagnetronData.m_vecMagnetronDamageEffectData.begin();
	for ( ; vIt != m_MagnetronData.m_vecMagnetronDamageEffectData.end(); ++vIt )
	{
		CX2DamageEffect::CEffectHandle hOldOrbit = *vIt;
		CX2DamageEffect::CEffect* pOldOrbit = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( hOldOrbit );	// official f_322B79D0 (resolve + invalidate-if-dead)
		if ( NULL == pOldOrbit || NULL == pOldOrbit->GetMainEffect() )
			continue;

		const D3DXVECTOR3 vFirePos = pOldOrbit->GetMainEffect()->GetBonePos( L"Dummy02" );	// official: the orbit mesh's Dummy02 bone (string image-verified @0x32d7e390)
		CX2DamageEffect::CEffect* pNewEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"DamageEffect_SA_APT_Magnetron_Dynamo_Projectile",
			m_fMagnetronStoredPower, vFirePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );	// official: power = this+25892 (the stored cast power), name image-verified @0x32d888c0
		ThirdPath_DynamoLockRetarget( pNewEffect );
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandleSilently( hOldOrbit );	// consume the orbit (mirrors the normal fire path)
	}

	m_MagnetronData.m_fFireReadyTime = 0.f;	// official: 25888 = 0

	// official tail: destroy every remaining orbit handle + empty the vector
	for ( vIt = m_MagnetronData.m_vecMagnetronDamageEffectData.begin(); vIt != m_MagnetronData.m_vecMagnetronDamageEffectData.end(); ++vIt )
	{
		CX2DamageEffect::CEffectHandle hLeft = *vIt;
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( hLeft, false );
	}
	m_MagnetronData.m_vecMagnetronDamageEffectData.clear();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_DynamoLockAcquire( CX2DamageManager::DamageData* pDamageData_ )
{
	// official f_32519450 (the AttackResultByType hook): the hit's attacker DE == the 9015 marker projectile (+26116) && the one-shot latch && the defender observable valid -> [a previous live lock -> the swap teardown] -> consume the latch -> store uid64/isNPC/duration -> spawn the lock-indicator ES OWNED BY THE TARGET -> re-aim the orbits
	if ( NULL == pDamageData_ )
		return;
	if ( pDamageData_->hAttackerEffect != m_hInstall9015DE ||
		 false == m_bDynamoLockArmed ||
		 null == pDamageData_->optrDefenderGameUnit )
		return;

	CX2GameUnit* pTarget = pDamageData_->optrDefenderGameUnit.GetObservable();
	if ( NULL == pTarget )
		return;

	if ( 0 != m_uidDynamoLockTarget )	// a previous lock is set -> the swap teardown (official: f_324EEAE0 -> f_324EA900)
		ThirdPath_DynamoLockClear();

	m_bDynamoLockArmed		= false;	// consume the latch (official: DamageData+0x4C4 = -1)
	m_uidDynamoLockTarget	= pTarget->GetUnitUID();
	m_bDynamoLockIsNPC		= ( CX2GameUnit::GUT_NPC == pTarget->GetGameUnitType() );

	{
		const int iDroneSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR );
		const float fLevel01 = (float)( iDroneSkillLevel > 0 ? iDroneSkillLevel : 1 ) - 1.f;
		m_fDynamoLockRemainTime	= ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? ( 2.0f + 0.15f * fLevel01 ) : ( 4.0f + 0.3f * fLevel01 );
	}

	// the lock-indicator ES owned by the TARGET (official: PlayEffectSet internal f_31F890E0 with the defender as the owner; the name image-verified @0x32d8b888; the set is particle-only so the power arg is cosmetic -- the official's -1.0 sentinel = the owner's GetPowerRate() per DI-9)
	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		m_hDynamoLockES = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_AAT_Installation_Drone_Activator_Lackon", pTarget, NULL, false, GetPowerRate() );
	// official extras SKIPPED: (a) the PvP/PvE notice-string push -- the string IDs 370/368 resolve to UNRELATED strings in the March-2015 US general.ess ("Ariel" / "Request for friend registration succeeded") = a KR-locale leftover, no lock-notice text exists in the US data -> document-skip; (b) the ES-instance tweak (f_31F86F20 with DamageData+0x3C4 -- the +964 field has no our-side mapping)

	// convert the live magnetron orbits into homing projectiles at the new lock (official: f_324F7C20 at the acquire tail)
	ThirdPath_DynamoLockReAimOrbits();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_OnFactoryFrameMove( const float fElapsedTime_ )
{
	// official sub_324EED30 (per-frame): (a) mode entered via install cast && drone-out phase && the current cast skill is NOT install-family �넂 end the mode; (b) DP empty �넂 end the mode; (c) while marked �넂 keep the drone glued to the player (facing횞offset, 0.9/0.1 per-frame lerp) + revert its anim to Wait when the attack anim ends
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetEffectSet() )
		return;

	const int iNowSkillID = (int)m_NowStateData.m_eSkillID;

	if ( true == m_bFactoryModeEntered && 2 == m_iFactoryPhase && false == ThirdPath_IsInstallFamilySkill( iNowSkillID ) )
	{
		ThirdPath_FactoryModeClear();
		return;
	}
	if ( GetDPValue() <= 0.f )
	{
		ThirdPath_FactoryModeClear();
		return;
	}

	if ( 0 == m_iFactoryMarkedSkillID )
		return;

	if ( NULL == g_pX2Game->GetDamageEffect() )
		return;
	CX2DamageEffect::CEffect* pDrone = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hFactoryDroneDE );
	if ( NULL == pDrone )
		return;

	const D3DXVECTOR3 vDir = GetDirVector();
	D3DXVECTOR3 vTarget = GetPos();
	if ( true == GetIsRight() )
		vTarget += vDir * m_vFactoryDroneOffset[0];
	else
		vTarget -= vDir * m_vFactoryDroneOffset[0];
	vTarget.y += m_vFactoryDroneOffset[1];
	vTarget.z += m_vFactoryDroneOffset[2];

	D3DXVECTOR3 vNow( m_vFactoryDroneLastPos[0], m_vFactoryDroneLastPos[1], m_vFactoryDroneLastPos[2] );
	if ( 0.f == m_vFactoryDroneLastPos[0] && 0.f == m_vFactoryDroneLastPos[1] && 0.f == m_vFactoryDroneLastPos[2] )
		vNow = vTarget;	// first frame after the spawn ��� snap to the anchor
	vNow = vNow * 0.9f + vTarget * 0.1f;	// official dbl_32D28190=0.9 / dbl_32D281A8=0.1
	m_vFactoryDroneLastPos[0] = vNow.x;
	m_vFactoryDroneLastPos[1] = vNow.y;
	m_vFactoryDroneLastPos[2] = vNow.z;
	pDrone->SetPos( vNow );


	if ( true == m_bFactoryFlagWait && 3 == m_iFactoryPhase &&
		NULL != pDrone->GetMainEffect() && NULL != pDrone->GetMainEffect()->GetXSkinAnim() &&
		true == pDrone->GetMainEffect()->GetXSkinAnim()->IsAnimationEnd() )
	{
		ThirdPath_FactoryDronePlayMotion( L"Wait" );
		m_iFactoryPhase		= 2;
		m_bFactoryFlagWait	= false;
	}
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_FactorySkillValueLoader()
{
	// official slot_260 (0x324FE0A0) MM case: the factory params come from passives 9411/9412/9413 + 9015's SA_REMOTE_SPEED_ABS; called at the mode-enter moment from the router (official hook = the engine's skill-value recompute virtual; ours consumes them only at mode-enter ��� observable-equal)
	if ( NULL == GetUnit() )
		return;
	if ( 0 < GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AMM_ENHANCED_DYNAMO_FACTORY_TRANSFORM ) )
	{
		m_fFactoryMP = GetSpecialAbilityValue( CX2SkillTree::SA_MP_CONSUMPTION, CX2SkillTree::SI_P_AMM_ENHANCED_DYNAMO_FACTORY_TRANSFORM );
		if ( 0.f >= m_fFactoryMP )
			m_fFactoryMP = 1.f;		// official clamp quirk (protected value <= 0 �넂 1.0)
		m_fFactoryDP = 3.f - GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, CX2SkillTree::SI_P_AMM_ENHANCED_DYNAMO_FACTORY_TRANSFORM );
		if ( 0.f > m_fFactoryDP )
			m_fFactoryDP = 0.f;
	}
	if ( 0 < GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AMM_TRANSFORM_OFFENSE_MODE ) )
		m_fInstallDmgScale = GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_REL, CX2SkillTree::SI_P_AMM_TRANSFORM_OFFENSE_MODE );	// official value-15
	if ( 0 < GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR ) )
	{
		const float fSpeed = GetSpecialAbilityValue( CX2SkillTree::SA_REMOTE_SPEED_ABS, CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR );
		m_fFactoryDroneSpeed = ( 0.f == fSpeed ) ? 1250.f : fSpeed;	// official: 0 �넂 default flt_32D87770=1250
	}
	m_iNewUpdateLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AMM_DYNAMO_FACTORY_MP_RECOVERY );	// official slot_260: this+25800 = 9412 level (stored only when > 0; ours re-reads at every mode-enter ��� level cannot decrease)
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_FactorySpawnDrone()
{
	// official f_325197C0 (the generate helper, disasm-recovered): spawn the per-skill persistent drone DE at pos +/- facing x droneOffset; the power = the owner's GetPowerRate (the OQ-4 tail window 32519BBB = vtable+412); the handle -> this+26088; the old drone destroyed first (the switch path)
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() )
		return;

	const WCHAR* pszDroneDE = NULL;
	switch ( m_iFactoryMarkedSkillID )
	{
	case CX2SkillTree::SI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR:	pszDroneDE = L"DAMAGE_EFFECT_Drone_Activator_Generate";	break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING:	pszDroneDE = L"DAMAGE_EFFECT_Drone_Pulse_Gatling_Generate";	break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_SHOOTING_CHASER:	pszDroneDE = L"DAMAGE_EFFECT_Shooting_Chaser_Generate";		break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY:	pszDroneDE = L"DAMAGE_EFFECT_Ultimate_Fury_Generate";		break;
	case CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL:		pszDroneDE = L"DAMAGE_EFFECT_Star_Fall_Generate";			break;
	default: break;
	}
	if ( NULL == pszDroneDE )
		return;

	if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hFactoryDroneDE )
		g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hFactoryDroneDE, false );

	const D3DXVECTOR3 vDir = GetDirVector();
	D3DXVECTOR3 vAnchor = GetPos();
	if ( true == GetIsRight() )
		vAnchor += vDir * m_vFactoryDroneOffset[0];
	else
		vAnchor -= vDir * m_vFactoryDroneOffset[0];
	vAnchor.y += m_vFactoryDroneOffset[1];
	vAnchor.z += m_vFactoryDroneOffset[2];

	m_hFactoryDroneDE = g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this, pszDroneDE, GetPowerRate(),
		vAnchor, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

	// the follow smoothing starts snapped at the anchor (the tick lerps from here)
	m_vFactoryDroneLastPos[0] = vAnchor.x;
	m_vFactoryDroneLastPos[1] = vAnchor.y;
	m_vFactoryDroneLastPos[2] = vAnchor.z;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_FactoryDronePlayMotion( const WCHAR* wcMotion_ )
{
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() || NULL == wcMotion_ )
		return;
	CX2DamageEffect::CEffect* pDrone = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hFactoryDroneDE );
	if ( NULL == pDrone || NULL == pDrone->GetMainEffect() )
		return;

	const bool bStartMotion = ( 0 == wcscmp( wcMotion_, L"Start" ) );
	pDrone->GetMainEffect()->ChangeAnim( wcMotion_, bStartMotion ? CKTDGXSkinAnim::XAP_ONE_WAIT : CKTDGXSkinAnim::XAP_LOOP, 1.f );
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
int CX2GUAdd::ThirdPath_GetInstallSlotOverlayIndex( const int iSkillID_ ) const
{

	if ( 0 != iSkillID_ && m_iFactoryMarkedSkillID == iSkillID_ )
		return 30;
	return -1;
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_AAT_CREATE_DYNAMO_FACTORY_StateStartFuture()
{
	// official 0x324F6F30: apply the factory marker buff (BFI 366 �넂 BTI 342) at the marked skill's level + spawn the drone-generate EffectSet (fixed string per official), then the base parse
	int iSkillLevel = 1;
	if ( NULL != GetUnit() && 0 != m_iFactoryMarkedSkillID )
		iSkillLevel = max( 1, GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( (CX2SkillTree::SKILL_ID)m_iFactoryMarkedSkillID ) );
	SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_DYNAMO_FACTORY, iSkillLevel );

	if ( 0 < m_iNewUpdateLevel )
		SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_NEW_UPDATE, (UINT)m_iNewUpdateLevel );


	if ( NULL != g_pX2Game && NULL != g_pX2Game->GetEffectSet() )
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_AAT_Installation_Drone_Activator_Generate", this, NULL, false, GetPowerRate(), -1.f );

	// the persistent drone = the per-skill DAMAGE_EFFECT_<skill>_Generate (our adaptation point for the official nowAction-machine dispatch of f_325197C0); the phase = 1 (generated/active) at the generate - NOT 2 (FACT-2: the tick's mode-kill check (a) must stay DEAD until the drone's first attack)
	ThirdPath_FactorySpawnDrone();
	m_iFactoryPhase = 1;

	if ( NULL != GetAddGageData() )
		GetAddGageData()->AddDPPoint( 1 );


	if ( NULL != g_pX2Game )
		g_pX2Game->UpdateSkillSlotUI( true );

	if ( NULL != g_pData && NULL != g_pData->GetUIManager() && NULL != g_pData->GetUIManager()->GetUISkillTree() )
		g_pData->GetUIManager()->GetUISkillTree()->UpdateSkillSlotOverlay();



	CommonStateStartFuture();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_AAT_CREATE_DYNAMO_FACTORY_OnFrameMove()
{
	// official 0x325103F0 = the generic Add special-attack EP shape (anim-end �넂 USI_WAIT + cancel windows)
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ELSE_IF_CAN_NASOD_ARMOR_ATTACK( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, GetIsFormationMode(), m_NasodArmorAttackTargetData.m_uidTargetUID )
	CommonEventProcess();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_AAT_CREATE_DYNAMO_FACTORY_StateEnd()
{
	// official 0x324F2A40 = the generic state-end fixup (landing/facing) ��� our CommonStateEnd covers it
	CommonStateEnd();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_FrameMove()
{
	// official 0x324ED650: @0.5s spawn the factory projectile from Bip01_L_Hand (the marker-blast carrier; its CREATE/DIE chain spawns Drone_Activator_Loop_Explosion, USED_SKILL_ID=9015)
	if ( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )	// official flt_32D2C9E8 = 0.5
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			D3DXVECTOR3 vBonePos = GetPos();
			GetBonePos( L"Bip01_L_Hand", vBonePos );
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AAT_Dynamo_Factory_Projectile", GetPowerRate() * m_fInstallDmgScale,
				vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( NULL != pEffect )
			{
				m_hInstall9015DE = pEffect->GetHandle();
				m_bDynamoLockArmed = true;
			}
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_FrameMove()
{
	// official 0x324ED9B0: @0.01s spawn the gatling drone from Bip01 + track the handle + the (SA_EFFECTIVE_TIME) duration
	if ( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )	// official flt_32D2AA30 = 0.01
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			D3DXVECTOR3 vBonePos = GetPos();
			GetBonePos( L"Bip01", vBonePos );
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AMM_Pulse_Gatling_Drone", GetPowerRate() * m_fInstallDmgScale,
				vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( NULL != pEffect )
			{
				m_hInstall9019DE		= pEffect->GetHandle();
				m_fInstall9019Duration	= GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, CX2SkillTree::SI_SA_AMM_INSTALLATION_PULSE_GATLING );	// official this[6543]
			}
		}
	}
	CommonFrameMove();
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_FrameMove()
{
	// official 0x32500CF0: @0.4s spawn the chaser attacker from Bip01 + the nearest-hostile target-UID seed into the DE (without it the DE's LOT_NEARST_UID_VECTOR_IN_RANGE never acquires + the templet's NOT_FIND_TARGET_MOVE_STOP=TRUE stalls the projectile) + the destroy-old before re-create
	if ( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )	// official flt_32D2AF74 = 0.4
	{
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{

			if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hInstall9020DE )
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hInstall9020DE, false );

			D3DXVECTOR3 vBonePos = GetPos();
			GetBonePos( L"Bip01", vBonePos );
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AMM_Shooting_Chaser_Attacker", GetPowerRate() * m_fInstallDmgScale,
				vBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( NULL != pEffect )
			{
				m_hInstall9020DE	= pEffect->GetHandle();
				m_fInstall9020Flag	= 1.f;	// official this[6549]


				bool bUserUnit = true;
				D3DXVECTOR2 vRange( 0.f, ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? 700.f : 1200.f );
				const UidType uidTarget = g_pX2Game->GetLockOnNearstTargetInRange( (CX2Room::TEAM_NUM)GetTeam(), vBonePos, vRange, bUserUnit );
				if ( 0 != uidTarget && -1 != (int)uidTarget )
				{
					if ( true == bUserUnit )
					{
						pEffect->SetLockOnUnitUID( uidTarget );
						pEffect->GetLockOnData().m_bLockOnUserUnit = true;
					}
					else
					{
						pEffect->SetLockOnNPCUID( (int)uidTarget );
						pEffect->GetLockOnData().m_bLockOnUserUnit = false;
					}
				}
			}
		}
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_FrameMove()
{
	// official 0x32503F10: @1.064s ��� the factory drone goes to attack phase (3) + 3 attack-drone meshes spawn (official sub_32502E50)
	if ( m_pXSkinAnim->EventTimerOneshot( 1.064f ) )	// official dword_32D8998C = 1.064
	{

		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() && NULL != g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hFactoryDroneDE ) )
		{
			m_iFactoryPhase		= 3;
			m_bFactoryFlagWait	= false;
			ThirdPath_FactoryDronePlayMotion( L"Attack" );
		}

		ThirdPath_UltimateFurySpawnAttack();
				// (the 3 attack-drone meshes + the set params spawn in ThirdPath_UltimateFurySpawnAttack - handles captured, official sub_32502E50)	// official: 3횞 Mesh_ADD_Ultimate_Fury_Attack_Dron
	}
	CommonFrameMove();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_UltimateFurySpawnAttack()
{
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() || NULL == g_pX2Game->GetMajorXMeshPlayer() )
		return;

	ThirdPath_UltimateFuryClearSet();

	m_fUltFuryInterval	= GetSpecialAbilityValue( CX2SkillTree::SA_DAMAGE_TIME, CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY );
	m_fUltFuryDuration	= GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, CX2SkillTree::SI_SA_AMM_INSTALLATION_ULTIMATE_FURY );
	m_fUltFuryPower		= GetPowerRate() * m_fInstallDmgScale;

	const D3DXVECTOR3 vPos = GetPos();
	for ( int i = 0; i < 3; ++i )
	{
		UltimateFuryDroneRec rec;
		CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this, L"Mesh_ADD_Ultimate_Fury_Attack_Dron", vPos, GetRotateDegree(), GetRotateDegree() );
		if ( NULL != pMesh )
			rec.m_hMesh = pMesh->GetHandle();
		m_vecUltFuryDrones.push_back( rec );
	}
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_UltimateFuryAttackDriver()
{
	if ( GetDPValue() <= 0.f )
	{
		ThirdPath_UltimateFuryClearSet();
		return;
	}
	if ( true == m_vecUltFuryDrones.empty() )
		return;
	if ( NULL == g_pX2Game || NULL == g_pX2Game->GetDamageEffect() || NULL == g_pX2Game->GetMajorXMeshPlayer() )
		return;

	CX2DamageEffect::CEffect* pFollowerDE = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hFactoryDroneDE );
	CKTDGXMeshPlayer::CXMeshInstance* pFollowerMesh = ( NULL != pFollowerDE ) ? pFollowerDE->GetMainEffect() : NULL;
	if ( NULL == pFollowerMesh )
		return;

	bool bEnd = false;
	if ( 0.f < m_fUltFuryDuration )
	{
		m_fUltFuryDuration -= m_fElapsedTime;
		if ( m_fUltFuryDuration <= 0.f )
			bEnd = true;
	}
	if ( m_fUltFuryElapsed >= m_fUltFuryInterval )
		bEnd = true;
	if ( true == bEnd )
	{
		ThirdPath_UltimateFuryClearSet();
		m_iFactoryPhase		= 2;
		m_bFactoryFlagWait	= true;
		ThirdPath_FactoryDronePlayMotion( L"Wait" );
		m_iUltFurySound		= 0;
		return;
	}

	bool bAnyValid = false;
	const float fRange = ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? 700.f : 1000.f;
	const float fRangeSq = fRange * fRange;
	const D3DXVECTOR3 vMyPos = GetPos();
	static const WCHAR* s_apTargetBone[3] = { L"Dummy01", L"Dummy02", L"Dummy03" };

	int iDrone = 0;
	for ( std::vector<UltimateFuryDroneRec>::iterator it = m_vecUltFuryDrones.begin(); it != m_vecUltFuryDrones.end(); ++it, ++iDrone )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMesh = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( it->m_hMesh, true );
		if ( NULL == pMesh )
			continue;

		{
			WCHAR wcBone[16];
			swprintf( wcBone, _countof( wcBone ), L"Dummy0%d", iDrone + 1 );
			D3DXVECTOR3 vNode( 0.f, 0.f, 0.f );
			pFollowerMesh->GetBonePos( wcBone, vNode );	// official returns (0,0,0) on a missing bone; zero-init reproduces that exactly
			pMesh->SetPos( vNode );
		}
		it->m_fTimer -= m_fElapsedTime;
		if ( it->m_fTimer < 0.f )
			it->m_fTimer = 0.f;

		CX2GameUnit* pTarget = NULL;
		if ( 0 != it->m_uidTarget )
		{
			pTarget = ( true == it->m_bIsNPC )
				? static_cast<CX2GameUnit*>( g_pX2Game->GetNPCUnitByUID( static_cast<int>( it->m_uidTarget ) ) )
				: static_cast<CX2GameUnit*>( g_pX2Game->GetUserUnitByUID( it->m_uidTarget ) );
		}
		bool bValid = false;
		D3DXVECTOR3 vTargetPos( 0.f, 0.f, 0.f );
		if ( NULL != pTarget && pTarget->GetNowHp() > 0.f && false == pTarget->GetInvincible() )
		{
			if ( false == pTarget->GetBonePos( s_apTargetBone[ ( iDrone < 3 ) ? iDrone : 0 ], vTargetPos ) )
				vTargetPos = pTarget->GetPos();
			if ( GetDistance3Sq( vMyPos, vTargetPos ) <= fRangeSq )
				bValid = true;
		}

		if ( true == bValid )
		{
			bAnyValid = true;
			D3DXVECTOR3 vMuzzle( 0.f, 0.f, 0.f );
			if ( false == pMesh->GetBonePos( L"Dummy01", vMuzzle ) )
				vMuzzle = pMesh->GetPos();
			if ( INVALID_DAMAGE_EFFECT_HANDLE == it->m_hShotDE )
			{
				CX2DamageEffect::CEffect* pShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AMM_Ultimate_Fury_Dynamo", m_fUltFuryPower,
					vMuzzle, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				if ( NULL != pShot )
					it->m_hShotDE = pShot->GetHandle();
				if ( 0 == m_iUltFurySound )
				{
					PlaySound( L"ADD_ChaserAttack02.ogg" );	// DESIGN INFERENCE (spec RISK-2): official latches a looped fire-sound; ours = a one-shot play per acquisition window (cosmetic only)
					m_iUltFurySound = 1;
				}
			}
			CX2DamageEffect::CEffect* pShotLive = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( it->m_hShotDE );
			if ( NULL != pShotLive )
				pShotLive->SetLinkMainEffectByPos( vMuzzle, vTargetPos );
			m_fUltFuryElapsed += m_fElapsedTime;
		}
		else
		{
			if ( INVALID_DAMAGE_EFFECT_HANDLE != it->m_hShotDE )
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( it->m_hShotDE, false );
			it->m_hShotDE		= INVALID_DAMAGE_EFFECT_HANDLE;
			it->m_uidTarget		= 0;
			it->m_iTargetType	= 0;
			it->m_bIsNPC		= false;
			if ( it->m_fTimer <= 0.f )
				it->m_fTimer = 0.5f;
		}


		{
			D3DXVECTOR3 vAim = vMyPos;	// no-target default (official quirk)
			if ( true == bValid )
				vAim = GetDirVecToDegree( vTargetPos - pMesh->GetPos() );
			pMesh->SetRotateDegree( vAim );
			pMesh->SetMoveAxisAngleDegree( vAim );
		}
	}

	if ( false == bAnyValid )
		m_iUltFurySound = 0;
	ThirdPath_UltimateFuryAssignTargets();
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_UltimateFuryAssignTargets()
{
	if ( true == m_vecUltFuryDrones.empty() || NULL == g_pX2Game )
		return;

	const float fRange = ( CX2Game::GT_PVP == g_pX2Game->GetGameType() ) ? 700.f : 1000.f;
	const float fRangeSq = fRange * fRange;
	const D3DXVECTOR3 vMyPos = GetPos();

	UidType aCandUID[3]		= { 0, 0, 0 };
	int		aiCandType[3]	= { 0, 0, 0 };
	bool	abCandNPC[3]	= { false, false, false };
	float	afCandDist[3]	= { 0.f, 0.f, 0.f };
	int		iCandCount		= 0;

	for ( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if ( NULL == pUnit )
			continue;
		if ( pUnit->GetTeam() == GetTeam() )
			continue;
		if ( pUnit->GetNowHp() <= 0.f )
			continue;
		if ( CX2GameUnit::GUT_NPC == pUnit->GetGameUnitType() )
		{
			if ( CX2UnitManager::NCT_BASIC != static_cast<CX2GUNPC*>( pUnit )->GetClassType() )
				continue;
			if ( pUnit->GetInvincible() )
				continue;
		}
		const float fDistSq = GetDistance3Sq( pUnit->GetPos(), vMyPos );
		if ( fDistSq > fRangeSq )
			continue;
		if ( 3 == iCandCount && fDistSq >= afCandDist[2] )
			continue;
		int iSlot = ( iCandCount < 3 ) ? iCandCount : 2;
		if ( iCandCount < 3 )
			++iCandCount;
		while ( 0 < iSlot && afCandDist[iSlot-1] > fDistSq )
		{
			afCandDist[iSlot]	= afCandDist[iSlot-1];
			aCandUID[iSlot]		= aCandUID[iSlot-1];
			aiCandType[iSlot]	= aiCandType[iSlot-1];
			abCandNPC[iSlot]	= abCandNPC[iSlot-1];
			--iSlot;
		}
		afCandDist[iSlot]	= fDistSq;
		aCandUID[iSlot]		= pUnit->GetUnitUID();
		aiCandType[iSlot]	= (int)pUnit->GetGameUnitType();
		abCandNPC[iSlot]	= ( CX2GameUnit::GUT_NPC == pUnit->GetGameUnitType() );
	}
	if ( 0 == iCandCount )
		return;

	int iDrone = 0;
	for ( std::vector<UltimateFuryDroneRec>::iterator it = m_vecUltFuryDrones.begin(); it != m_vecUltFuryDrones.end(); ++it, ++iDrone )
	{
		int iPick = 0;
		if ( iCandCount >= 3 )
			iPick = iDrone;
		else if ( 2 == iCandCount )
			iPick = ( 1 == iDrone ) ? 1 : 0;	// DESIGN INFERENCE (spec section 5.5): the official STL-inlined dedup walk is ambiguous on the 2-candidate tiebreak
		if ( iPick >= iCandCount )
			iPick = iCandCount - 1;
		it->m_uidTarget		= aCandUID[iPick];
		it->m_iTargetType	= aiCandType[iPick];
		it->m_bIsNPC		= abCandNPC[iPick];
	}
}
#endif
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
void CX2GUAdd::ThirdPath_UltimateFuryClearSet()
{
	if ( NULL != g_pX2Game )
	{
		for ( std::vector<UltimateFuryDroneRec>::iterator it = m_vecUltFuryDrones.begin(); it != m_vecUltFuryDrones.end(); ++it )
		{
			if ( NULL != g_pX2Game->GetMajorXMeshPlayer() && INVALID_MESH_INSTANCE_HANDLE != it->m_hMesh )
				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( it->m_hMesh );
			if ( NULL != g_pX2Game->GetDamageEffect() && INVALID_DAMAGE_EFFECT_HANDLE != it->m_hShotDE )
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( it->m_hShotDE, false );
		}
	}
	m_vecUltFuryDrones.clear();
	m_fUltFuryElapsed	= 0.f;
	m_fUltFuryInterval	= 0.f;
	m_fUltFuryDuration	= 0.f;
	m_fUltFuryPower		= 0.f;
	m_iUltFurySound		= 0;
}
#endif
#ifdef X2_ADD_2ND_PATH
void CX2GUAdd::ANSI_SA_AMM_INSTALLATION_STAR_FALL_FrameMove()
{
	// official 0x324F2BD0: @1.2s ��� power capture + destroy the old starfall DE + spawn DAMAGE_EFFECT_Star_Fall_Start_MESH + store the handle
	if ( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )	// official flt_32D84CB4 = 1.2
	{
		m_fInstall9022Power		= GetPowerRate() * m_fInstallDmgScale;	// official this[6559]
		m_fInstall9022Duration	= GetSpecialAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, CX2SkillTree::SI_SA_AMM_INSTALLATION_STAR_FALL );	// official this[6558] (the templet's last per-level value = the 8s duration)
		if ( NULL != g_pX2Game && NULL != g_pX2Game->GetDamageEffect() )
		{
			if ( INVALID_DAMAGE_EFFECT_HANDLE != m_hInstall9022DE )
				g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hInstall9022DE, false );
			const D3DXVECTOR3 vPos = GetPos();
			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_Star_Fall_Start_MESH", m_fInstall9022Power,
				vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( NULL != pEffect )
				m_hInstall9022DE = pEffect->GetHandle();
		}
		m_fInstall9022Timer = 0.5f;	// official this[6557]
	}
	CommonFrameMove();
}
#endif
