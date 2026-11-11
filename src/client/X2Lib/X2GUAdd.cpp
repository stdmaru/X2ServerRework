#include "stdafx.h"
#include "X2GUAdd.h"


namespace _CONST_ADD_
{
	/// �޺� XXZZ �ݺ���
	const float COMBO_XXZZ_MIN_COUNT						= 3.f;		/// �ּ� �ݺ���
	const float COMBO_XXZZ_MAX_COUNT						= 10.f;		/// �ִ� �ݺ���

	/// ��� ���� �޺� XX ���� �ð�
	const float DASH_JUMP_COMBO_X2_MAX_CHARGE_TIME			= 3.f;		/// �ִ� ���� �ð�
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
	const int	VOID_BREAKER_MAX_USE_MUTATION_COUNT			= 3;		/// �ִ� ���� ������ �Ҹ��

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
	}
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
, m_fUsingDPSkillDamageRate( 1.f )
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

}

CX2GUAdd::~CX2GUAdd(void)
{
	StopLoopSound();
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
		{
			m_iSkillCutInSetIndex = 0;
		} break;
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

	/// �ʿ����� ��ü
	m_fBodyOfTranscendenceActiveRate		= 0.f;		/// �ߵ� Ȯ�� �ʱ�ȭ
	m_fBodyOfTranscendenceMoveSpeed			= 1.f;		/// �̵� �ӵ� ������ �ʱ�ȭ

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
	default:
		break;
	}
}
/*virtual*/ void CX2GUAdd::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
{
	if ( iSlotIndex_ < 0 || iSlotIndex_ >= ARRAY_SIZE( _CONST_GUUSER_::aNormalSkillStateID ) )
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
		{	// ����
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

	/// �񰢼��� MP�� ����, ������ DP�� ���� ��ų�� �ƴҶ��� MP �Ҹ� ����
	const float fUseDPSkillAtFormationMode = GetSpecialAbilityValue( CX2SkillTree::SA_DP_USE_REL, eSkillID_ );

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
	default:
		break;
	}

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

		// Ż �� ž�� ���¿��� ��� ������ ��ų���� �˻�
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

		bool bSlotB		= (m_iPressedSkillSlotIndex > 3) ? true : false;
		int iSlotIndex	= (m_iPressedSkillSlotIndex > 3) ? m_iPressedSkillSlotIndex-4 : m_iPressedSkillSlotIndex;

#if X2_DEV_TOOLS
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

			if ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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

		/// ����� ��Ƽ�� �̸鼭 DP �Һ��� ���� �� DP��뷮�� ���� DP ���� ó��
		if ( CX2SkillTree::ST_SPECIAL_ACTIVE == pSkillTemplet_->m_eType &&
			 0.f >= pSkillTemplet_->m_usFPConsumtion  )
			UpNowDPValueByUseMP( m_fMPConsume );


		if ( pSkillTemplet_->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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
	}
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

	/// ���ҵ� �尩 �޺��� Ÿ�� ���� ����
	if ( true == IsMyUnit() )
		SetNasodArmorAttackTarget( pDamageData );

	/// ���̳ʸ� ���� - ���׳�Ʈ�� ���� ó��
	AttackMagnetronDamageEffect( pDamageData );

	/// ����Ʈ �Ƴ���� Ÿ�� ���� ����
	SetDustAnatomyTarget( pDamageData );

	/// ���ǹ� ������ ���� ó��
	AttackQuicksilverFrenzy( pDamageData, true );
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
	const float TRIGGER_TIME_FACTOR_DASH_ATTACK = 0.3f;

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
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_ZZXX )
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
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_XXX )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneZ == true, ANSI_COMBO_XXZ )
	SKILL_CANCEL_AFTER_ONLY_ADD( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/// ANSI_COMBO_XXX
void CX2GUAdd::ComboXXXEventProcess()
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

	/// ���� ���갪 ���� ( �ִ� ���� ���갪 ���� )
	const float fChargeValue = ( _CONST_ADD_::DASH_JUMP_COMBO_X2_MAX_CHARGE_TIME > m_fChargeTime )
							   ? _CONST_ADD_::DASH_JUMP_COMBO_X2_MAX_CHARGE_TIME : m_fChargeTime;

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
								 m_InputData.oneZ == true, ANSI_COMBO_NASOD_ARMOR_Z )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								 m_InputData.oneX == true, ANSI_COMBO_NASOD_ARMOR_X )

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
	if( m_pXSkinAnim->EventTimerOneshot( 1.35f ) )
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
			CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SA_AN_Particle_Accelerator_Attack", fPowerRate,
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

			CX2DamageEffect::CEffect* pEffect =
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

	float& fIncreaseSize	= m_PsychicStormData.m_fIncreaseSize;				/// ����Ʈ ũ��

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

	float dfhdjkfhjksdhkjf = m_pXSkinAnim->GetNowAnimationTime();

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
	case CX2SkillTree::SI_A_AN_PHASE_SHIFT:		/// ���� ��ȭ ��ġ ���� ������Ʈ
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
	}

	return fCoolTime;
}

/** @function	: SetCustomAnimName
	@brief		: Ư�� ������ ��, �ִϸ��̼��� �ٲٴ� �Լ�
	@param		: �ִϸ��̼� �̸�
*/
/*virtual*/ void CX2GUAdd::SetCustomAnimName( IN const UserUnitStateData& UserStateData, IN OUT wstring& wstrAnimName )

{
	switch( UserStateData.stateID )
	{
	case USI_DASH:
	case ANSI_AIR_DASH:
		{
			/// ������ ������� ��
			if ( true == GetIsFormationMode() )
				wstrAnimName = L"Formation_Dash";
		} break;
	case USI_DASH_END:
		{
			/// ������ ��� ������ ��
			if ( true == GetIsFormationMode() )
				wstrAnimName = L"Formation_DashEnd";
		} break;
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

		fIncreaseValueRateBySkill = cUserSkillTree.GetIncreaseGainSoulRateByDamage();			// �ǰݽ� �����Ǵ� ���������ӵ�
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

		fIncreaseValueRateBySkill = cUserSkillTree.GetIncreaseGainSoulRateByHit();			// Ÿ�ݽ� �����Ǵ� ���������ӵ�
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
			/// �Ҹ� MP�� ������ DP �Ҹ� ( UI�� ǥ�õ� DP�� ���� �� DP�� 20�� ũ�� )
			const float fMPConsume = CX2GUUser::GetActualMPConsume( pSkillSlotData_->m_eID, GetSkillLevelValue( pSkillSlotData_->m_eID ) );

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
}

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
					for ( vIt ; vIt != m_vecFantasyTracerTargetInfo.end(); ++vIt )
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
	ELSE_IF_CAN_RELEASE_FORMATION_MODE( USI_RIDING_HYPER_MODE );

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
	ELSE_IF_CAN_RELEASE_FORMATION_MODE( USI_RIDING_HYPER_MODE );

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
	ELSE_IF_CAN_RELEASE_FORMATION_MODE( USI_RIDING_HYPER_MODE );

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
	ELSE_IF_CAN_RELEASE_FORMATION_MODE( USI_RIDING_HYPER_MODE );

	CommonEventProcess();
}

void CX2GUAdd::RidingDashEndEventProcess()
{
	const float TRIGGER_TIME_FACTOR_DASH_ATTACK = 0.3f;

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
	ELSE_IF_CAN_RELEASE_FORMATION_MODE( USI_RIDING_HYPER_MODE );

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
		D3DXVECTOR3 vRotateDegree = GetRotateDegree();

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
}
