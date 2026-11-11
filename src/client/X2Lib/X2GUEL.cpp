#include "stdafx.h"
#include "X2GUEl.h"


namespace _CONST_EL_
{
	// ����� ����, HP ���� 30% -> 50% �� ����
	const float CHIVALRY_BUFF_CONDITION_HP_RATE = 0.5f;	 // ����� ���� ���� ���� ���� ü�� ����

	const float MAX_EXTINCTION_CHARGE_TIME		= 1.f;	 // �ͽ��ü� �ִ� ���� �ð�
	const float LIMIT_EXTINCTION_POWER_RATE		= 0.2f;  // �ͽ��ü� ���ݷ� ���� ���� ��

	const float MIN_EXTINCTION_DEGREE_Z			= -45.f; // �ͽ��ü� �ּ� ����
	const float MAX_EXTINCTION_DEGREE_Z			= 0.f;   // �ͽ��ü� �ְ� ����
	const float BASE_EXTINCTION_DEGREE_Z		= -30.f; // �ͽ��ü� �⺻ ����
	const float EXTINCTION_DEGREE_Z_CONTROL		= 100.f; // �ͽ��ü� ���� ���� ��

	// ���� ��
	const float GALE_COOL_TIME_DECREASE_RATE		= 0.2f;	// ��ǳ ��ų ��Ÿ�� Rate
	const float GALE_COOL_TIME_DECREASE_RATE_PVP	= 0.6f;	// ��ǳ ��ų ��Ÿ�� Rate(���� 1/2)

	const float GALE_MP_CONSUME_RATE				= 0.6f; // ��ǳ ��ų MP �Ҹ� Rate
	const float GALE_MP_CONSUME_RATE_PVP			= 0.8f; // ��ǳ ��ų MP �Ҹ� Rate(���� 1/2)

	const float GALE_FORCE_DOWN_DECREASE_RATE		= 0.6f;	// ��ǳ Ŀ�ǵ� �ٿ��ġ ����Rate
	const float GALE_FORCE_DOWN_DECREASE_RATE_PVP	= 0.8f;	// ��ǳ Ŀ�ǵ� �ٿ��ġ ����Rate(���� 1/2)

	// Ŀ�ǵ�
	const UINT MAX_DASH_JUMP_COMBO_SPK_XZ_REATTACK	= 2;	// ESI_DASH_JUMP_COMBO_SPK_XZ �߰� ���� ���� Ƚ��

	// MP �Ҹ�
	const float FIRE_BALL_MP_COST					= 6.f;
	const float SPK_DASH_JUMP_XZ_MP_COST			= 4.f;


	const int MAX_XX_DOWN_X_LOOP_COUNT = 4;				// �׷��� ������, XX��X Ŀ�ǵ� �ִ� ���� Ƚ��
	const float MAX_DRASTIC_CHARGE_LOOP_TIME = 2.0f;	// �׷��� ������ ��Ƽ��, �巡��ƽ ���� �ִ� ���� �ð�
	const float MIN_DRASTIC_CHARGE_LOOP_TIME = 0.5f;	// �׷��� ������ ��Ƽ��, �巡��ƽ ���� �ּ� ���� �ð�

	const float MAX_JUGGERNAUT_BUSTER_LOOP_TIME = 1.0f;

	const float SBM_DASH_COMBO_DELAY_TIME = 0.5;	// ������¡ ��Ʈ �뽬 �޺� ���� ������  Ÿ��. ��� �޺��� ������( XXX�� XXXZ ) �Ŀ� �ٽ� ��� �޺��� �����ϱ� ������ �ð�



}

static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] =
{
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_Pyro_Knight.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_Saber_Knight.dds"),
	},

		{ /// �׷��� ������
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_SGM.dds"),
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_SGM.dds"),
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_SGM.dds"),
		},

		{ /// ������¡ ��Ʈ
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_SBH.dds"),
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_SBH.dds"),
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Elesis_SBH.dds"),
		}
};

/*static */CX2GUEL* CX2GUEL::CreateGUEL( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit )
{
	return new CX2GUEL( unitIndex, teamNum, pScriptLuaState, frameBufferNum, pUnit );
};

CX2GUEL::CX2GUEL( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit )
: CX2GUUser( unitIndex, teamNum, pScriptLuaState, frameBufferNum, pUnit )
, m_pSkillTemplet(NULL)
, m_iPressedSkillSlotIndex( -1 )
, m_fMPConsume ( 0.f )
, m_iPreComboCount( -1 )
, m_uiDodgeAndSlashMaxReuseCount(0)
, m_uiDodgeAndSlashReuseCount(0)
, m_fFireBallPowerRate(1.f)
, m_fChivalryBuffCheckTime ( 0.f )
, m_uiChivalryAttackBuffFactorID ( 0 )
, m_uiChivalryDefenceBuffFactorID ( 0 )
, m_uiChivalryLevel( 0 )
, m_bIsActiveEndurancePower( false )
, m_fStigmaOfFireCoolTime( 0.f )
, m_bIsStigmaOfFire( false )
, m_uiSkillLevelStigmaOfFire( 0 )
, m_eFireBlossomsSaveDebuffID( BTI_NONE )
, m_fFireBlossomsCoolTime( 0.f )
, m_fFireBlossomsCheckTime( -1.f )
, m_bIsFireBlossoms( false )
, m_uiFireBlossomsSkillLevel( 0 )
, m_bIsLearnUnextinguishableFire( false )
// �׷��� ������ ��� ����
, m_fExtinctionRemainTime( 0.f )
, m_fExtinctionDegreeZ ( 30.f )
, m_hExctionBG(INVALID_PARTICLE_SEQUENCE_HANDLE)
, m_hExtinctionArrow(INVALID_PARTICLE_SEQUENCE_HANDLE)
, m_hExtinctionArrowParticle(INVALID_PARTICLE_HANDLE)
, m_bIsApplyVictoriousSword ( false )
, m_fDodgeAndSlashCoolTimeDecreaseRate ( 1.f )
, m_bIsDashAttackPossible(true)
, m_uiSPKXZReattakCount(0)
, m_iComboXXdownXNowLoopCount (0)
, m_iComboXXdownXMaxLoopCount (0)
// ������¡ ��Ʈ ��� ����
, m_bAbleQuickStand ( false )
, m_bAbleQuickStandAttack ( false )
, m_iRushingSwordAddLoopValue ( 0 )
, m_iRushingSwordNowLoopValue ( 0 )
, m_iDecreaseMPRushingSword ( 0 )
, m_cFirstStrikeData ( 0, 0, 0 )
, m_fDrasticChargeNowLoopTime ( 0 )
, m_fDrasticChargeLoopDecreaseMPValue ( 0 )
, m_bIsProvokeStateChange ( false )
, m_hSnapCresherLoop ( INVALID_EFFECTSET_HANDLE )
, m_hSnapCreaherLoopAttack ( INVALID_EFFECTSET_HANDLE )
, m_hXXDownXLoop ( INVALID_EFFECTSET_HANDLE )
, m_hDrasticChargeLoop ( INVALID_EFFECTSET_HANDLE )
, m_hDrasticChargeGroundFire ( INVALID_EFFECTSET_HANDLE )
, m_fJuggernautBusterNowLoopTime ( 0.f )
, m_bJuggernautBusterIsStartDirectionRight ( false )
, m_hSBHScaletRoseDamageEffect( INVALID_DAMAGE_EFFECT_HANDLE )
, m_bIsThereBlazingDanceTarget( false )
, m_bIsBlazingDanceLastAttack( false )
, m_bIsFrameSword( false )
, m_uiSkillLevelFlameSword( 0 )

, m_fWallOfProminenceLifeTime( 0.0f )
, m_fStigmaOfFireDamageValue( 1.0f )
, m_fDashComboStartDelay( -1.0f )
, m_fAncientFireRenderTime( -1.0f )
{
	m_fThrowStartTime = 0.66f;
	m_vThrowStartPosOffset = D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity = D3DXVECTOR3(2000.f, 100.f, 0.f);

	m_vecBlazingDanceTargetUid.clear();

}

CX2GUEL::~CX2GUEL(void)
{
}

/*static*/ void CX2GUEL::AppendDeviceToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

/*static*/ void CX2GUEL::AppendComponentToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

/*static*/ void CX2GUEL::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUEL::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUEL::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForSwordToDeviceList( listInOut_, eDamageType_ );
}

/*virtual */bool CX2GUEL::CanRidingState()
{
	if( true == CX2GUUser::CanRidingState() )
		return true;

 	switch( GetNowStateID() )
 	{
 	case ESI_SWORD_WAIT:
 		return true;
 	}

	return false;
}

/*virtual*/ void CX2GUEL::Init( bool bUseTeamPos_, int iStartPosIndex_ )
{
	CX2GUUser::Init( bUseTeamPos_, iStartPosIndex_ );

	SetNeedToUpdateWeaponEffect( true );
	UpdateEquipCollisionData();
}
/*virtual*/ void CX2GUEL::ParseCommonRandomState()
{
	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{
		const char* tableName = "";
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_ELESIS_KNIGHT:
			{
				tableName = "EL_KNIGHT";
			} break;
		case CX2Unit::UC_ELESIS_SABER_KNIGHT:
			{
				tableName = "EL_SABER_KNIGHT";
			} break;
		case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
			{
				tableName = "EL_PYRO_KNIGHT";
			} break;
		case CX2Unit::UC_ELESIS_GRAND_MASTER:
			{
				tableName = "EL_GRAND_MASTER";
			} break;
		case CX2Unit::UC_ELESIS_BLAZING_HEART:
			{
				tableName = "EL_BLAZING_HEART";
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
/*virtual*/ void CX2GUEL::InitState()
{
	switch ( GetUnitClass() )
	{
	case CX2Unit::UC_ELESIS_KNIGHT:
	case CX2Unit::UC_ELESIS_SABER_KNIGHT:
	case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
		{
			m_iSkillCutInSetIndex = 0;
		} break;
	case CX2Unit::UC_ELESIS_GRAND_MASTER:
		{
			m_iSkillCutInSetIndex = 1;
		} break;
	case CX2Unit::UC_ELESIS_BLAZING_HEART:
		{
			m_iSkillCutInSetIndex = 2;
		} break;
	default:
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
/*virtual*/ void CX2GUEL::InitComponent()
{
	CX2GUUser::InitComponent();

	if( true == IsMyUnit() )
	{
		ASSERT( NULL != g_pX2Game );
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_PVP:
			{
				static_cast<CX2ElesisGageData*>(m_pGageData)->SetWayOfSwordPoint( 0.f );
				static_cast<CX2ElesisGageData*>(m_pGageData)->SetWayOfSwordState( 0 );
				m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
			} break;

		default:
			{
				m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
			} break;
		}

		switch ( GetWayOfSwordState() )
		{
		case WSS_VIGOR:
			{
				Start_SwordmanVigor();
			} break;
		case WSS_DESTRUCTION:
			{
				Start_SwordmanDestruction();
			} break;
		default:
			break;
		}
	}
}
/*virtual*/ void CX2GUEL::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();
 	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
 	CX2SkillTree* const pSkillTree = g_pData->GetSkillTree();

	// ���߷� ��� �ʱ�ȭ
	SetAddMagicAttackCriticalDamageRate(0.f);
	SetAddMagicAttackCriticalRate(0.f);
	// ���߷� ��� �ʱ�ȭ
	SetAddAnimSpeedFactor(0.f);

	// ���� ģȭ �ʱ�ȭ
	SetFireBallPowerRate(1.f);

	// ����� ���� ���� ID �ʱ�ȭ
	m_uiChivalryAttackBuffFactorID = 0;
	m_uiChivalryDefenceBuffFactorID = 0;
	m_uiChivalryLevel = 0;

	// ����&������ ���� Ƚ��
	m_uiDodgeAndSlashMaxReuseCount = 0;

	// �Ҳ��� ���� �ʱ�ȭ
	m_fStigmaOfFireCoolTime = 0.f;
	m_bIsStigmaOfFire = false;
	m_uiSkillLevelStigmaOfFire = 0;
	m_fStigmaOfFireDamageValue = 1.f;
	// �Ҳ� ��ȭ �ʱ�ȭ
	m_eFireBlossomsSaveDebuffID = BTI_NONE;
	m_fFireBlossomsCoolTime = 0.f;
	m_fFireBlossomsCheckTime = -1.f;
	m_bIsFireBlossoms = false;
	m_uiFireBlossomsSkillLevel = 0;
	// ������ �ʴ� �Ҳ� �ʱ�ȭ
	m_bIsLearnUnextinguishableFire = false;
	//���̾� �� �����ð�
	m_fWallOfProminenceLifeTime = 0.f;
	// ȫ���� �� �ʱ�ȭ
	m_uiSkillLevelFlameSword = 0;

	// ������ ���� ��ų
	{
		// ����&������
		{
			const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_A_EL_DODGE_AND_SLASH;
			const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
			if ( 0 < iSkillLevel )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
				if( NULL != pSkillTemplet )
				{
					// ��ų ���� Ƚ��
					m_uiDodgeAndSlashMaxReuseCount = static_cast<UINT>(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillLevel  ));
				}
			}
		}
	}

	switch( GetUnitClass() )
	{
	case CX2Unit::UC_ELESIS_GRAND_MASTER :
		{
			// ��� ��� ����
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SGM_ADVANCED_KNIGHT_MASTERY;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );

					if( NULL != pSkillTemplet )
					{
						// ����� ��� ���� ����
						m_bAbleQuickStand = true;

						// ��ȸ�� ������ �ִٸ�, ����� ��� �� ������ �����ϴ�.
						int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_GET_CHANCE, true );
						if( iSkillLevel > 0 )
						{
							m_bAbleQuickStandAttack = true;
						}
					}
				}
			}
			// ���� ����
			{

				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SGM_WALTZ_OF_SWORD;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );

					if( NULL != pSkillTemplet )
					{
						// ���� �ҵ� ���� Ƚ�� ����
						m_iRushingSwordAddLoopValue = static_cast<int> ( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillLevel  ) );

						// ���� �ҵ� MP �Ҹ� ����
						m_iDecreaseMPRushingSword = static_cast<int> ( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevel ) );
					}
				}
			}

			// ���� �ʽ�
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SGM_FIRST_STRIKE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );

					if( NULL != pSkillTemplet )
					{
						float fStopTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_TIME, iSkillLevel  );
						float fAddDamage = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel  );
						float fIncreaseMPRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_REGENERATION_ABS, iSkillLevel  );

						m_cFirstStrikeData.SetFirstStrikeData( fStopTime, fAddDamage, fIncreaseMPRate );
					}
				}
			}
		} // break ������ �ȵ�.
	// 1-1 ���̹� ����Ʈ
	case CX2Unit::UC_ELESIS_SABER_KNIGHT:
		{
			// ���߷� ���
			{
 				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SSK_AGILITY_MASTERY;
 				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
 				if ( 0 < iSkillLevel )
 				{
 					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );

 					if( NULL != pSkillTemplet )
 					{
 						// �̵� �ӵ� ����
 						const float fMoveSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel  );


						m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * ( 1.f + fMoveSpeed ) );
						m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * ( 1.f + fMoveSpeed ) );


						// ���� �ӵ� ����
						float fAddAnimSpeedFactor = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ANI_SPEED, iSkillLevel  ) );
 						SetAddAnimSpeedFactor( max(0.f, fAddAnimSpeedFactor) );
 					}
 				}
			}
			// ����� ����
			{
 				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SSK_CHIVALRY;
 				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
 				if ( 0 < iSkillLevel )
 				{
					m_uiChivalryLevel = static_cast<UINT>(iSkillLevel);
 					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
 					if( NULL != pSkillTemplet &&
 						2 <= pSkillTemplet->m_vecBuffFactorID.size() )
 					{
  						m_uiChivalryAttackBuffFactorID = pSkillTemplet->m_vecBuffFactorID[0];
  						m_uiChivalryDefenceBuffFactorID = pSkillTemplet->m_vecBuffFactorID[1];
 					}
 				}
			}
		} break;

	// 2-2 ������¡ ��Ʈ
	case CX2Unit::UC_ELESIS_BLAZING_HEART:
		{
			// �Ҳ� ��ȭ
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SBH_FIRE_BLOSSOMS;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet  )
					{
						//����ð� ����
						m_fFireBlossomsCheckTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel  );
						//��ų ���� ����
						m_uiFireBlossomsSkillLevel = iSkillLevel;
						//ó���� ������ ���·� ����
						m_bIsFireBlossoms = true;
					}
				}
			}

			// �Ҳ��� ����
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SBH_STIGMA_OF_FIRE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					m_uiSkillLevelStigmaOfFire = iSkillLevel;
					InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUEL, ApplyDebuffStigmaOfFire ) );

					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet  )
					{
						//��ų ������ ���� ������ ������ ����
						m_fStigmaOfFireDamageValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel  );
					}
				}
			}

			// ȫ���� ��
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_A_SBH_FLAME_SWORD;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					m_uiSkillLevelFlameSword = iSkillLevel;
					InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUEL, ApplyDebuffFlameSword ) );
				}
			}

			// ������ �ʴ� �Ҳ�
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SBH_UNEXTINGUISHABLE_FIRE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if ( 0 < iSkillLevel )
				{
					m_bIsLearnUnextinguishableFire = true;
				}
			}

			// ���̾� ��
			{
				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_SA_SBH_WALL_OF_PROMINENCE;
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
				if( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet  )
					{
						// �����ð� ����
						m_fWallOfProminenceLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel  );
					}
				}
			}


		} // break ������ �ȵ�.
	// 1-2 ���̷� ����Ʈ
	case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
		{
			// ���߷� ���
			{
 				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SPK_CONCENTRATION_MASTERY;

				// �нú� ��ų�ε� ��ų ���� ȿ�� �޴� �κ� ����
				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID, true );
 				if ( 0 < iSkillLevel )
 				{
 					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
 					if( NULL != pSkillTemplet )
 					{
 						// ���� ���ݷ� ũ��Ƽ�� ������ ���� ����
						SetAddMagicAttackCriticalDamageRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_DAM_ABS, iSkillLevel  ) );
 						// ���� ���ݷ� ũ��Ƽ�� �ߵ� Ȯ�� ����
 						SetAddMagicAttackCriticalRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ABS, iSkillLevel  ) );
 					}
 				}
			}
			// ���� ��ü
			{
 				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_P_SPK_BODY_OF_FIRE;
 				const int iSkillLevel = userSkillTree.GetSkillLevel( eSkillID );
 				if ( 0 < iSkillLevel )
 				{
 					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( eSkillID );
 					if( NULL != pSkillTemplet )
 					{
 						// ���̾ ���ݷ� ����
						SetFireBallPowerRate( GetFireBallPowerRate() * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel  ) );

 						// �� ���� ��������� SA_FIRE_DEFENCE_ABS �˻� ���ؼ� Ȯ�� �ϸ� ��.
 					}
 				}
			}
		} break;
	default:
		break;
	}
}
/*virtual*/ void CX2GUEL::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
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

	switch( pSkillTempletEquipped->m_eType )
	{
	case CX2SkillTree::ST_BUFF:
		{	// ����
			m_LuaManager.MakeTableReference( "ESI_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_COMMON_BUFF", hyperStateData.stateID );
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
					m_LuaManager.MakeTableReference( "ESI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "ESI_THROW_ITEM", hyperStateData.stateID );
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

//#pragma region ACTIVE, SPECIAL ACTIVE ��ų
	switch ( eSkillID_ )
	{
	case CX2SkillTree::SI_SA_SSK_SPIRAL_BLAST:
		{	/// �����̷� ������Ʈ
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_SPIRAL_BLAST", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_SPIRAL_BLAST_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_SPIRAL_BLAST_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_SPIRAL_BLAST", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SSK_EXTINCTION:
		{ // ���̹� - �ͽ��ü�
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_EXTINCTION_START", normalStateData.stateID );

			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_START_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_START_Start );
			normalStateData.StateStartFuture= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_START_StartFuture );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_START_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_EXTINCTION_START", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SSK_EXTINCTION_CHARGE;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_EXTINCTION_CHARGE", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_CHARGE_FrameMove );
				stateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_CHARGE_FrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_CHARGE_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SSK_EXTINCTION_ATTACK;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_EXTINCTION_ATTACK", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_ATTACK_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_ATTACK_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_ATTACK_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_EXTINCTION_DEGREE", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP_FrameMove );
				stateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP_FrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_EXTINCTION_DEGREE", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN_FrameMove );
				stateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN_FrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_SA_SSK_VICTORIOUS_SWORD:
		{	/// �¸��� ��
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_VICTORIOUS_SWORD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_VICTORIOUS_SWORD_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_VICTORIOUS_SWORD_EventProcess );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_VICTORIOUS_SWORD_FrameMove );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_VICTORIOUS_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SSK_JUDGEMENT_DRIVE:
		{ // ������Ʈ ����̺�
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_JUDGEMENT_DRIVE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_JUDGEMENT_DRIVE_Init );
			normalStateData.StateStartFuture = SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_JUDGEMENT_DRIVE_StateStartFuture );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_JUDGEMENT_DRIVE_StateStart );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SSK_JUDGEMENT_DRIVE_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SSK_JUDGEMENT_DRIVE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SPK_BURST_RISING:
		{	/// ����Ʈ ����¡
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_BURST_RISING", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_BURST_RISING_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_BURST_RISING_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_BURST_RISING", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SPK_BURST_WAVE:
 		{ // ����Ʈ ���̺�
 			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_BURST_WAVE", normalStateData.stateID );
 			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_BURST_WAVE_Init );
 			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_BURST_WAVE_EventProcess );

 			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_BURST_WAVE", hyperStateData.stateID );
 			hyperStateData.m_bHyperState		= true;
 			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
 			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
 		} break;
	case CX2SkillTree::SI_SA_SPK_SWORDFIRE:
		{	/// �ҵ� ���̾�
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_SWORDFIRE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_SWORDFIRE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_SWORDFIRE_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_SWORDFIRE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SPK_INFERNAL_BLADE:
		{ // ���丣�� �����̵�
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_INFERNAL_BLADE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_INFERNAL_BLADE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_INFERNAL_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_INFERNAL_BLADE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SPK_ETERNAL_FIRE:
		{ // ���ͳ� ���̾�
		 	m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_ETERNAL_FIRE", normalStateData.stateID );
		 	normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_ETERNAL_FIRE_Init );
		 	normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SPK_ETERNAL_FIRE_EventProcess );

		 	m_LuaManager.MakeTableReference( "ESI_SI_SA_SPK_ETERNAL_FIRE", hyperStateData.stateID );
		 	hyperStateData.m_bHyperState		= true;
		 	hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		 	hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SBH_SCALDIC_SWORD:
		{	//��Į�� �ҵ�
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_SCALDIC_SWORD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_SCALDIC_SWORD_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_SCALDIC_SWORD_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_SCALDIC_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SBH_WALL_OF_PROMINENCE:
		{	//���̾� ��
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_WALL_OF_PROMINENCE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_WALL_OF_PROMINENCE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_WALL_OF_PROMINENCE_EventProcess );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_WALL_OF_PROMINENCE_FrameMove );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_WALL_OF_PROMINENCE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SBH_BLAZE_WING:
		{	//�������� ��
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZE_WING", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZE_WING_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZE_WING_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZE_WING", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_SBH_BLAZING_DANCE:
		{	//������¡ ����
			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_START", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_START_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_START_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_START_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_START", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SBH_BLAZING_DANCE_1ST;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_1ST", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_1ST_StartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_1ST_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SBH_BLAZING_DANCE_2ND;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_2ND", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_2ND_StartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_2ND_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SBH_BLAZING_DANCE_3RD;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_3RD", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_3RD_StartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_3RD_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SBH_BLAZING_DANCE_4TH;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_4TH", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_4TH_StartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_4TH_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_SA_SBH_BLAZING_DANCE_FINISH;
				m_LuaManager.MakeTableReference( "ESI_SI_SA_SBH_BLAZING_DANCE_FINISH", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_StartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_StateEnd );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_FrameMoveFuture );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_HA_SBH_ANCIENT_FIRE:
		{	//���̼�Ʈ ���̾�
			m_LuaManager.MakeTableReference( "ESI_SI_HA_SBH_ANCIENT_FIRE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_HA_SBH_ANCIENT_FIRE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_HA_SBH_ANCIENT_FIRE_EventProcess );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_HA_SBH_ANCIENT_FIRE_FrameMove );

			m_LuaManager.MakeTableReference( "ESI_SI_HA_SBH_ANCIENT_FIRE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

		} break;
	case CX2SkillTree::SI_A_SBH_FLAME_SWORD:
		{	//ȫ���� ��
			m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_FLAME_SWORD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_FLAME_SWORD_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_FLAME_SWORD_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_FLAME_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_A_SBH_FLAME_SWORD_FINISH;
				m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_FLAME_SWORD_FINISH", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_FLAME_SWORD_FINISH_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_A_SBH_FIREWORKS:
		{	//�ҳ���
			m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_FIREWORKS", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_FIREWORKS_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_FIREWORKS_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_FIREWORKS", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_SBH_SCALET_ROSE:
		{	//�Ҳ� ���
			m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_SCALET_ROSE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_SCALET_ROSE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_SCALET_ROSE_EventProcess );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_SCALET_ROSE_FrameMove );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_SCALET_ROSE_StateEnd );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_SCALET_ROSE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_A_SBH_SCALET_ROSE_SUCCESS;
				m_LuaManager.MakeTableReference( "ESI_SI_A_SBH_SCALET_ROSE_SUCCESS", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SBH_SCALET_ROSE_SUCCESS_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_A_SPK_SWORD_ERUPTION:
		{ // �ҵ� �̷���
			m_LuaManager.MakeTableReference( "ESI_SI_A_SPK_SWORD_ERUPTION", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SPK_SWORD_ERUPTION_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SPK_SWORD_ERUPTION_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SPK_SWORD_ERUPTION", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_SPK_BIG_BURST:
 	{	/// ������
 		m_LuaManager.MakeTableReference( "ESI_SI_A_SPK_BIG_BURST", normalStateData.stateID );
 		normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SPK_BIG_BURST_Init );
 		normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SPK_BIG_BURST_EventProcess );

 		m_LuaManager.MakeTableReference( "ESI_SI_A_SPK_BIG_BURST", hyperStateData.stateID );
 		hyperStateData.m_bHyperState		= true;
 		hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
 		hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
 	} break;
	case CX2SkillTree::SI_A_SSK_CROSS_SLASH:
		{	/// ũ�ν� ������
			m_LuaManager.MakeTableReference( "ESI_SI_A_SSK_CROSS_SLASH", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SSK_CROSS_SLASH_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_SSK_CROSS_SLASH_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SSK_CROSS_SLASH", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_SSK_POWER_BREAK:
		{	/// �Ŀ� �극��ũ
			m_LuaManager.MakeTableReference( "ESI_SI_A_SSK_POWER_BREAK", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL,ESI_SI_A_SSK_POWER_BREAK_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL,ESI_SI_A_SSK_POWER_BREAK_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SSK_POWER_BREAK", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_SSK_HEAVY_STUNNER:
		{	/// ��� ���ͳ�
			m_LuaManager.MakeTableReference( "ESI_SI_A_SSK_HEAVY_STUNNER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL,ESI_SI_A_SSK_HEAVY_STUNNER_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL,ESI_SI_A_SSK_HEAVY_STUNNER_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_SSK_HEAVY_STUNNER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
//////////////////////////////////////////////////////////////////////////////////////////////////////
//�Ʒ� ������ ��ų
//////////////////////////////////////////////////////////////////////////////////////////////////////
	case CX2SkillTree::SI_SA_EL_UNLIMITED_BLADE:
		{ // �𸮹�Ƽ�� �����̵�
			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_UNLIMITED_BLADE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_UNLIMITED_BLADE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_UNLIMITED_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_UNLIMITED_BLADE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_EL_WILD_SHOCK:
		{	/// ���ϵ� ��ũ
			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_WILD_SHOCK", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_WILD_SHOCK_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_WILD_SHOCK_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_WILD_SHOCK", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_EL_MEGA_BURSTER:
		{ // �ް� ������
			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_MEGA_BURSTER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_MEGA_BURSTER_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_MEGA_BURSTER_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_MEGA_BURSTER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_EL_POWER_BURSTER:
		{ // �Ŀ� ������
			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_POWER_BURSTER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_POWER_BURSTER_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_SA_EL_POWER_BURSTER_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_SA_EL_POWER_BURSTER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_EL_SONIC_ASSAULT_STING:
		{	/// �Ҵ� ���Ʈ - ����
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_SONIC_ASSAULT_STING", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_SONIC_ASSAULT_STING_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_SONIC_ASSAULT_STING_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_SONIC_ASSAULT_STING", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_EL_SONIC_ASSAULT_STAB:
		{	/// �Ҵ� ���Ʈ - ����
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_SONIC_ASSAULT_STAB", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_SONIC_ASSAULT_STAB_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_SONIC_ASSAULT_STAB_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_SONIC_ASSAULT_STAB", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_EL_MEGA_SLASH:
		{ // �ް� ������
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_MEGA_SLASH", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_MEGA_SLASH_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_MEGA_SLASH_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_MEGA_SLASH", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_EL_KICK:
	{ // ������
		m_LuaManager.MakeTableReference( "ESI_SI_A_EL_KICK", normalStateData.stateID );
		normalStateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_KICK_Start );
		normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_KICK_Init );
		normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_KICK_EventProcess );

		m_LuaManager.MakeTableReference( "ESI_SI_A_EL_KICK", hyperStateData.stateID );
		hyperStateData.m_bHyperState		= true;
		hyperStateData.StateStart			= normalStateData.StateStart;
		hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
	} break;
	case CX2SkillTree::SI_A_EL_LEAP_CRASH:
	{ // ����
		m_LuaManager.MakeTableReference( "ESI_SI_A_EL_LEAP_CRASH", normalStateData.stateID );
		normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_LEAP_CRASH_Init );
		normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_LEAP_CRASH_EventProcess );

		m_LuaManager.MakeTableReference( "ESI_SI_A_EL_LEAP_CRASH", hyperStateData.stateID );
		hyperStateData.m_bHyperState		= true;
		hyperStateData.StateStart			= normalStateData.StateStart;
		hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

		{
			UserUnitStateData stateData;
			stateData.Init();
			stateData.stateID			= ESI_SI_A_EL_LEAP_CRASH_JUMP;
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_LEAP_CRASH_JUMP", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_LEAP_CRASH_JUMP_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_SI_A_EL_LEAP_CRASH_JUMP_LANDING;
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_LEAP_CRASH_JUMP_LANDING", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_LEAP_CRASH_JUMP_LANDING_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
	} break;
	case CX2SkillTree::SI_A_EL_RUSHING_SWORD:
		{ // ���� �ҵ�
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_RUSHING_SWORD", normalStateData.stateID );
			normalStateData.StateStartFuture = SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_StartFuture );
			// �׷��� ������ �нú�, ���� ���� �߰��� ���� StateStart �Լ� �߰�
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_StateStart );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_RUSHING_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			// �׷��� ������ �нú�, ���� ���� �߰��� ���� StateStart �Լ� �߰�
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= ESI_SI_A_EL_RUSHING_SWORD_2nd;
				m_LuaManager.MakeTableReference( "ESI_SI_A_EL_RUSHING_SWORD_2nd", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_2nd_EventProcess );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_2nd_StartFuture );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_A_EL_RUSHING_SWORD_3rd;
				m_LuaManager.MakeTableReference( "ESI_SI_A_EL_RUSHING_SWORD_3rd", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_RUSHING_SWORD_3rd_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_A_EL_DODGE_AND_SLASH:
		{	/// ����&������
			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_DODGE_AND_SLASH", normalStateData.stateID );
			normalStateData.StateStartFuture = SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_StartFuture );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SI_A_EL_DODGE_AND_SLASH", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= ESI_SI_A_EL_DODGE_AND_SLASH_SECOND;
				m_LuaManager.MakeTableReference( "ESI_SI_A_EL_DODGE_AND_SLASH", stateData.stateID );
				stateData.StateStartFuture  = SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_SECOND_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_FrameMove );	//����
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_EventProcess ); //����
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z;
				m_LuaManager.MakeTableReference( "ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	// �׷��� ������ ��Ƽ��
	// ����
	case CX2SkillTree::SI_A_SGM_PROVOKE :
		{
			m_LuaManager.MakeTableReference( "ESI_A_SGM_PROVOKE", normalStateData.stateID );
			// ���� ����
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_PROVOKE_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_PROVOKE_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_PROVOKE_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_PROVOKE_StateEnd );


			m_LuaManager.MakeTableReference( "ESI_A_SGM_PROVOKE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;


			// ���� ����
			{
				UserUnitStateData subStateData;
				subStateData.Init();
				subStateData.stateID			= ESI_A_SGM_PROVOKE_REVENGE;
				m_LuaManager.MakeTableReference( "ESI_A_SGM_PROVOKE_REVENGE", subStateData.stateID );
				subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_PROVOKE_REVENGE_EventProcess );

				m_StateList[subStateData.stateID] = subStateData;
			}
		}
		break;
	// �巡��ƽ ����
	case CX2SkillTree::SI_A_SGM_DRASTIC_CHARGE:
		{
			m_LuaManager.MakeTableReference( "ESI_A_SGM_DRASTIC_CHARGE_START", normalStateData.stateID );
			// �巡��ƽ ���� ��ŸƮ
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_START_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_START_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_START_EventProcess );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_START_StartFurture );


			m_LuaManager.MakeTableReference( "ESI_A_SGM_DRASTIC_CHARGE_START", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			// �巡��ƽ ���� ����
			{
				UserUnitStateData subStateData;
				subStateData.Init();
				subStateData.stateID			= ESI_A_SGM_DRASTIC_CHARGE_LOOP;
				m_LuaManager.MakeTableReference( "ESI_A_SGM_DRASTIC_CHARGE_LOOP", subStateData.stateID );
 				subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_LOOP_EventProcess );
				subStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_LOOP_FrameMove );

				m_StateList[subStateData.stateID] = subStateData;
			}
			// �巡��ƽ ���� ����
			{
				UserUnitStateData subStateData;
				subStateData.Init();
				subStateData.stateID			= ESI_A_SGM_DRASTIC_CHARGE_END;
				m_LuaManager.MakeTableReference( "ESI_A_SGM_DRASTIC_CHARGE_END", subStateData.stateID );
				subStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_END_StateStart );
				subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_DRASTIC_CHARGE_END_EventProcess );

				m_StateList[subStateData.stateID] = subStateData;
			}

			if ( NULL != GetUnit() )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_SGM_DRASTIC_CHARGE );
				if ( NULL != pSkillTemplet )
				{
					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
					m_fDrasticChargeLoopDecreaseMPValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillTempletLevel ); // ���� �� ���Ǵ� MP ��
				}
			}
		}
		break;
	// ���� ũ����
	case CX2SkillTree::SI_A_SGM_SNAP_CRASHER:
		{
			m_LuaManager.MakeTableReference( "SI_A_SGM_SNAP_CRESHER_READY", normalStateData.stateID );
			// ���� ũ����, ���߿��� ���
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_START_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_START_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_START_StateEnd );

			m_LuaManager.MakeTableReference( "SI_A_SGM_SNAP_CRESHER_READY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;

			// ���� ũ����, ���߿��� ��� �� ������
			{
				UserUnitStateData subStateData;
				subStateData.Init();
				subStateData.stateID			= ESI_A_SGM_SNAP_CRASHER_LOOP;
				m_LuaManager.MakeTableReference( "SI_A_SGM_SNAP_CRESHER_LOOP", subStateData.stateID );
				subStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_LOOP_StateStart );
				subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_LOOP_EventProcess );

				m_StateList[subStateData.stateID] = subStateData;
			}
			// ���� ũ����, ���߿��� ��� �� �����ͼ� �� ���
			{
				UserUnitStateData subStateData;
				subStateData.Init();
				subStateData.stateID			= ESI_A_SGM_SNAP_CRASHER_END;
				m_LuaManager.MakeTableReference( "SI_A_SGM_SNAP_CRESHER_END", subStateData.stateID );
				subStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_END_StateStart );
				subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_A_SGM_SNAP_CRASHER_END_EventProcess );

				m_StateList[subStateData.stateID] = subStateData;
			}
		}
		break;

		// ��Ƽ�� ��

	// ����� ��Ƽ�� ����
	// �Ҵ� �����̵�
	case CX2SkillTree::SI_SA_SGM_SONIC_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESI_SA_SGM_SONIC_BLADE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_SONIC_BLADE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_SONIC_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESI_SA_SGM_SONIC_BLADE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;

	// ũ��Ƽ�� ���̺�
	case CX2SkillTree::SI_SA_SGM_CRITICAL_DIVE:
		{
			m_LuaManager.MakeTableReference( "ESI_SA_SGM_CRITICAL_DIVE", normalStateData.stateID );
			// ũ��Ƽ�� ���̺�, ���� ~ ���� �ȸ¾Ҵٸ� ��
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_CRITICAL_DIVE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_CRITICAL_DIVE_EventProcess );


			m_LuaManager.MakeTableReference( "ESI_SA_SGM_CRITICAL_DIVE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			// ���� ũ����, ���߿��� ��� �� �����ͼ� �� ���
			{
				UserUnitStateData subStateData;
				subStateData.Init();
				subStateData.stateID			= ESI_SA_SGM_CRITICAL_DIVE_HIT;
				m_LuaManager.MakeTableReference( "ESI_SA_SGM_CRITICAL_DIVE_HIT", subStateData.stateID );
				subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_CRITICAL_DIVE_HIT_EventProcess );

				m_StateList[subStateData.stateID] = subStateData;
			}
		}
		break;

	// ���ų� ������
	case CX2SkillTree::SI_SA_SGM_JUGGERNAUT_BUSTER:
	{
		m_LuaManager.MakeTableReference( "ESI_SA_SGM_JUGGERNAUT_BUSTER_START", normalStateData.stateID );
		// ���ų� ������, ���� ~ ���� ��
		normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_START_Init );
		normalStateData.StateStart			= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_START_StateStart );
		normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_START_EventProcess );


		m_LuaManager.MakeTableReference( "ESI_SA_SGM_JUGGERNAUT_BUSTER_START", hyperStateData.stateID );
		hyperStateData.m_bHyperState		= true;
		hyperStateData.StateInit			= normalStateData.StateInit;
		hyperStateData.StateStart			= normalStateData.StateStart;
		hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		// ���ų� ������, ���� ~ ������ ��
		{
			UserUnitStateData subStateData;
			subStateData.Init();
			subStateData.stateID			= ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP;
			m_LuaManager.MakeTableReference( "ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP", subStateData.stateID );
			subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_EventProcess );
			subStateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_FrameMove );

			m_StateList[subStateData.stateID] = subStateData;
		}
		// ���ų� ������, ������ �� ~ ��
		{
			UserUnitStateData subStateData;
			subStateData.Init();
			subStateData.stateID			= ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST;
			m_LuaManager.MakeTableReference( "ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST", subStateData.stateID );
			subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST_EventProcess );
			subStateData.StateEnd			= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST_StartEnd );

			m_StateList[subStateData.stateID] = subStateData;
		}
		// ���ų� ������, ~ ��
		{
			UserUnitStateData subStateData;
			subStateData.Init();
			subStateData.stateID			= ESI_SA_SGM_JUGGERNAUT_BUSTER_END;
			m_LuaManager.MakeTableReference( "ESI_SA_SGM_JUGGERNAUT_BUSTER_END", subStateData.stateID );
			subStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_JUGGERNAUT_BUSTER_END_EventProcess );

			m_StateList[subStateData.stateID] = subStateData;
		}
	}
	break;

	// ������ ����
	case CX2SkillTree::SI_SA_SGM_WAR_PRELUDE:
		{
			m_LuaManager.MakeTableReference( "ESI_SA_SGM_WAR_PRELUDE", normalStateData.stateID );
			// ������ ����
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_WAR_PRELUDE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_SA_SGM_WAR_PRELUDE_EventProcess );


			m_LuaManager.MakeTableReference( "ESI_SA_SGM_WAR_PRELUDE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;
	// ����� ��Ƽ�� ��

	// ������ ��Ƽ�� ����
	// �ҵ� ���� ������
	case CX2SkillTree::SI_HA_SGM_SWORD_OF_RELICS:
		{
			m_LuaManager.MakeTableReference( "ESI_HA_SGM_SWORD_OF_RELICS", normalStateData.stateID );
			// �ҵ� ���� ������
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_HA_SGM_SWORD_OF_RELICS_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUEL, ESI_HA_SGM_SWORD_OF_RELICS_EventProcess );


			m_LuaManager.MakeTableReference( "ESI_HA_SGM_SWORD_OF_RELICS", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;

	// ������ ��Ƽ�� ��








	default:
		break;
	}
//#pragma endregion ACTIVE, SPECIAL ACTIVE ��ų
}


/*virtual*/ void CX2GUEL::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_EL_DODGE_AND_SLASH == eSkillID_ )
	{/// ����&������
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_DODGE_AND_SLASH );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_EL_DODGE_AND_SLASH_SECOND );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_EL_RUSHING_SWORD == eSkillID_ )
	{/// ���� �ҵ�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_RUSHING_SWORD );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_EL_RUSHING_SWORD_2nd );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_EL_RUSHING_SWORD_3rd );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_SSK_EXTINCTION == eSkillID_ )
	{/// ���̹� - �ͽ��ü�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_SSK_EXTINCTION );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SSK_EXTINCTION_CHARGE );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SSK_EXTINCTION_ATTACK );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_EL_ENDURANCE_POWER == eSkillID_ )
	{/// �ر� - ��(˭)
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_ENDURANCE_POWER );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE_MEMO );

		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_EL_LEAP_CRASH == eSkillID_ )
	{/// ����
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_LEAP_CRASH );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_EL_LEAP_CRASH_JUMP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_EL_LEAP_CRASH_JUMP_LANDING );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_SGM_JUGGERNAUT_BUSTER == eSkillID_ )
	{/// ���ų� ������
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_SGM_JUGGERNAUT_BUSTER );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_SGM_JUGGERNAUT_BUSTER_START );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_SGM_JUGGERNAUT_BUSTER_END );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_SGM_CRITICAL_DIVE == eSkillID_ )
	{/// ũ��Ƽ�� ���̺�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_SGM_CRITICAL_DIVE );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SA_SGM_CRITICAL_DIVE_HIT );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_SGM_SNAP_CRASHER == eSkillID_ )
	{/// ���� ũ����
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_SGM_SNAP_CRASHER );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_A_SGM_SNAP_CRASHER_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_A_SGM_SNAP_CRASHER_END );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_A_SGM_SNAP_CRASHER_GROUND );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_SGM_PROVOKE == eSkillID_ )
	{/// ����
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_SGM_PROVOKE );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_A_SGM_PROVOKE_REVENGE );
		}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_SGM_DRASTIC_CHARGE == eSkillID_ )
	{/// �巡��ƽ ����
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_SGM_DRASTIC_CHARGE );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_A_SGM_DRASTIC_CHARGE_LOOP );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_A_SGM_DRASTIC_CHARGE_END );

		}
	}


	if ( true == bChangeAll_ || CX2SkillTree::SI_A_SBH_SCALET_ROSE == eSkillID_ )
	{/// �Ҳ� ���
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_SBH_SCALET_ROSE );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_SBH_SCALET_ROSE_SUCCESS );
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_SBH_BLAZING_DANCE == eSkillID_ )
	{/// ������¡ ����
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_SBH_BLAZING_DANCE );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SBH_BLAZING_DANCE_1ST );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SBH_BLAZING_DANCE_2ND );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SBH_BLAZING_DANCE_3RD );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SBH_BLAZING_DANCE_4TH );
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_SA_SBH_BLAZING_DANCE_FINISH );
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_SBH_FLAME_SWORD == eSkillID_ )
	{/// ȫ���� ��
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_SBH_FLAME_SWORD );
		if( NULL != pSkillTemplet )
		{
			SetEquippedSkillLevelStateData( pSkillTemplet, ESI_SI_A_SBH_FLAME_SWORD_FINISH );
		}
	}
}

/*virtual*/ void CX2GUEL::CommonStateStartFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStartFuture();
}
/*virtual*/ void CX2GUEL::CommonStateStart()
{
	CX2GUUser::CommonStateStart();
}

/*virtual*/ void CX2GUEL::CommonFrameMoveFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMoveFuture();
	m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
}

/*virtual*/ void CX2GUEL::CommonFrameMove()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMove();

	WayOfSwordStateChange();
	GaleBuffFrameMove();

	// ����� ���� �нú� ó��
	ChivalryProc_FrameMove();

	DrasticChargeProc_FrameMove ();			// �巡��ƽ ���� ����Ʈ
	XX_DOWN_XProc_FrameMove ();				// XX �� X ����Ʈ
	SnapCrasherProc_FrameMove ();			// �巡��ƽ ���� ����Ʈ


	// �Ҳ��� ���� �нú� ó��
	StigmaOfFireProc_FrameMove();
	// �Ҳ� ��ȭ �нú� ó��
	FireBlossomsProc_FrameMove();

	// ��� �޺��� ������( XXX�� XXXZ ) �Ŀ� �ٽ� ��� �޺��� �����ϱ� ������ �ð�
	if( 0.0f <= m_fDashComboStartDelay )
		m_fDashComboStartDelay -= m_fElapsedTime;

	if( 0.0f < m_fAncientFireRenderTime )
	{
		m_fAncientFireRenderTime -= m_fElapsedTime;

		if( m_fAncientFireRenderTime <= 0.0f )
		{
			SetLayer(XL_UNIT_0);
			m_bSetCustomRenderParam = false;
			SetAlphaObject(false);
			m_fAncientFireRenderTime = -1.f;
		}
	}

}

/*virtual*/ void CX2GUEL::CommonStateEndFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEndFuture();
}

/*virtual*/ void CX2GUEL::CommonStateEnd()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEnd();
}

/*virtual*/ void CX2GUEL::DetonationStateChange( CX2DamageManager::DamageData* pDamageData )
{
	if( WSS_DESTRUCTION == GetWayOfSwordState() )
	{ // ���� �������� ���� �� ������
		DoStateEndurancePowerRage( true );
		SetNowChargeMpForDetonation( 0.f ) ;
	}
	else
	{
		CX2GUUser::DetonationStateChange( pDamageData );
	}
}

/*virtual*/ RENDER_HINT	CX2GUEL::CommonRender_Prepare()
{
	KTDXPROFILE();

	if( GetWayOfSwordState() == WSS_VIGOR )
	{
		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
		m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_SKYBLUE;
		m_RenderParam.outLineColor		= 0xffffffff;
	}
	else if( GetWayOfSwordState() == WSS_DESTRUCTION )
	{
		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
		m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_SCARLET;
		m_RenderParam.outLineColor		= 0xffffffff;
	}

	return CX2GUUser::CommonRender_Prepare();
}

/*virtual*/ bool CX2GUEL::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition_ /*= CX2SkillTree::ASUT_GROUND */)
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

	Send_SKILL_USE_REQ( m_pSkillTemplet->m_eID );

	return true;

}
/** @function	: NoStateChangeActionFrameMove
	@brief		: ������Ʈ�� ��ȭ�� ���� ��ų ��� �� ����Ǵ� FrameMove
*/
/*virtual*/ void CX2GUEL::NoStateChangeActionFrameMove()
{
	switch( m_FrameDataNow.syncData.nowAction )
	{
	case UAI_ENDURANCE_POWER:
		{
			ProcessEndurancePowerSkill();
		} break;
	default:
		break;
	}
}

/*virtual*/ float CX2GUEL::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );

	switch ( eSkillID_ )
	{
		// ����&������
	case CX2SkillTree::SI_A_EL_DODGE_AND_SLASH:
		{
			// �¸��� �� ������ ���� �� ���¶��
			if( true == m_bIsApplyVictoriousSword )
			{
				fMpConsumption = 0.f;
			}

		} break;



	case CX2SkillTree::SI_A_EL_RUSHING_SWORD:
		{
			// ���� ������ ���� m_iSGMDecreaseMPRushingSword ���� ���� ���, �ش� �� ��ŭ�� MP �Ҹ��� ���δ�.
			fMpConsumption = fMpConsumption - m_iDecreaseMPRushingSword;
		} break;

	case CX2SkillTree::SI_A_SSK_POWER_BREAK:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO5 ) == true )
				fMpConsumption *= 0.9f;
		} break;

	case CX2SkillTree::SI_SA_SPK_ETERNAL_FIRE:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO16 ) == true )
				fMpConsumption -= 30.f;
		} break;

	case CX2SkillTree::SI_SA_SSK_VICTORIOUS_SWORD:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO8 ) == true )
				fMpConsumption *= 0.9f;
		}

	default:
		break;
	}

//////////////////////////////////////////////////////////////////////////
// ��ǳ ��ų MP ����ó��.
	// ������
	// GetEquippedActiveSkillTemplet()�� �ι� ȣ�� �ϴ� ������ ������
	// UI������ �������� ��� �� �� �ֵ��� ���� ��ġ���� ó��
	if( CX2GUUser::WSS_VIGOR == GetWayOfSwordState() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetEquippedActiveSkillTemplet( eSkillID_ );

		if( NULL != pSkillTemplet &&
			WSS_VIGOR == pSkillTemplet->m_iWOSType )
		{
			// ���������� ���ҷ� 1/2
			if( NULL != g_pX2Game && CX2Game::GT_PVP == g_pX2Game->GetGameType())
				fMpConsumption *= _CONST_EL_::GALE_MP_CONSUME_RATE_PVP;
			else
				fMpConsumption *= _CONST_EL_::GALE_MP_CONSUME_RATE;
		}
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	float fMpDecreaseRate = 1.0f;
	if( NULL != pSkillTemplet )
		fMpDecreaseRate  =  g_pData->GetMyUser()->GetSelectUnit()->GetSkillMpDecreaseRate(eSkillID_, pSkillTemplet->m_eType);

	return fMpConsumption * fMpDecreaseRate;
}
/*virtual*/ float CX2GUEL::GetActualCoolTime( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN int iSkillLevel ) const
{
	if( NULL == pSkillTemplet_ )
		return 0.f;

	float fSkillCoolTime = CX2GUUser::GetActualCoolTime( pSkillTemplet_, iSkillLevel );
	if( WSS_VIGOR == GetWayOfSwordState() )
	{ // ��ǳ ��ų ��Ÿ�� ����
		if( WSS_VIGOR == pSkillTemplet_->m_iWOSType )
		{
			float fGaleCoolTimeDescraseRate = _CONST_EL_::GALE_COOL_TIME_DECREASE_RATE;

			// ���������� ���ҷ� 1/2
			if( NULL != g_pX2Game && CX2Game::GT_PVP == g_pX2Game->GetGameType())
				fGaleCoolTimeDescraseRate = _CONST_EL_::GALE_COOL_TIME_DECREASE_RATE_PVP;

			fSkillCoolTime *= fGaleCoolTimeDescraseRate;
		}
	}

	switch ( pSkillTemplet_->m_eID )
	{
	// ����&������
	case CX2SkillTree::SI_A_EL_DODGE_AND_SLASH:
		{
			// �¸��� �� ������ ���� �� ���¶��
			if( true == m_bIsApplyVictoriousSword )
			{
				ASSERT( m_fDodgeAndSlashCoolTimeDecreaseRate < 1.f );
				fSkillCoolTime *= m_fDodgeAndSlashCoolTimeDecreaseRate;
			}

		} break;
	default:
		break;
	}

	return max( 0.f, fSkillCoolTime );
}
/*virtual*/ CX2GageData* CX2GUEL::CreateGageData()
{
	if ( true == IsMyUnit() )
	{
		const CX2ElesisGageData* pElswordGageData =
			static_cast<const CX2ElesisGageData*>( CX2GageManager::GetInstance()->GetMyGageData() );

		if ( NULL != pElswordGageData )
			return new CX2ElesisGageData( *pElswordGageData );
		else
			return new CX2ElesisGageData();
	}
	else
		return new CX2ElesisGageData();
}

/*virtual*/	bool CX2GUEL::ShouldResetNotBusyTimer()
{
	if ( false == CX2GUUser::ShouldResetNotBusyTimer() )
		return false;
	else if ( ESI_SWORD_WAIT == m_FrameDataFuture.syncData.nowState )
		return false;

	return true; // �ٻ� ���� ��� ���� �ؾ� ��
}

void CX2GUEL::InitStateID()
{
	m_CommonState.m_Wait	= USI_WAIT;
}

/** @function	: InitStateCommon
	@brief		: ��� ĳ���Ͱ� ���������� ���� ���� �� ������ ������� �������� ���� ���� �ʱ�ȭ
*/
void CX2GUEL::InitStateCommon()
{
	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "ESI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "ESI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DieBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DieBackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, DieBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DieBackEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "ESI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "ESI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, WaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, WaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, WaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "ESI_WALK", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, WalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "ESI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, JumpReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, JumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, JumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESI_JUMP_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, JumpLandingStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, JumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "ESI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "ESI_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, DashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, DashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEL, DashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ESI_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, DamageDownFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageDownFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, DamageDownBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageDownBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DamageAirDownLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "ESI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "ESI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "ESI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "ESI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "ESI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "ESI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "ESI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOnEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "ESI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "ESI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "ESI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "ESI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "ESI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ESI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ESI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "ESI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "ESI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "ESI_RIDING_SPECIAL_MOVE", stateData.stateID );
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
	m_LuaManager.MakeTableReference( "ESI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingHyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, RidingHyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ROLLING_FRONT_FRONT;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ROLLING_FRONT_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpRollingFrontFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpRollingFrontFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ROLLING_FRONT_BACK;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ROLLING_FRONT_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpRollingFrontBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpRollingFrontBackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ROLLING_BACK_FRONT;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ROLLING_BACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpRollingBackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpRollingBackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ROLLING_BACK_BACK;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ROLLING_BACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpRollingBackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpRollingBackBackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpAttackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpAttackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpAttackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpAttackBackEnd );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEL, StandUpAttackBackStateEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ATTACK_FRONT_NO_DOWN;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ATTACK_FRONT_NO_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpAttackFrontNoDownEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpAttackFrontNoDownEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_STANDUP_ATTACK_BACK_NO_DOWN;
	m_LuaManager.MakeTableReference( "ESI_STANDUP_ATTACK_BACK_NO_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpAttackBackNoDownEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpAttackBackNoDownEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_POWER_LANDING;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_POWER_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, DashJumpPowerLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, DashJumpPowerLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpPowerLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "ESI_WALL_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
	m_StateList[stateData.stateID] = stateData;

//=====================================================================================
// ���� ������Ʈ

	stateData.Init();
	stateData.stateID			= ESI_COMBO_Z;
	m_LuaManager.MakeTableReference( "ESI_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ESI_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ESI_COMBO_ZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_ZZZZ;
	m_LuaManager.MakeTableReference( "ESI_COMBO_ZZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboZZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_ZZZX;
	m_LuaManager.MakeTableReference( "ESI_COMBO_ZZZX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboZZZXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_X;
	m_LuaManager.MakeTableReference( "ESI_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_XX;
	m_LuaManager.MakeTableReference( "ESI_COMBO_XX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_XXX;
	m_LuaManager.MakeTableReference( "ESI_COMBO_XXX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboXXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_COMBO_XXZ;
	m_LuaManager.MakeTableReference( "ESI_COMBO_XXZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ComboXXZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ESI_JUMP_ATTACK_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, JumpAttackZEventProcess );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, JumpAttackZFrameMoveFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_JUMP_ATTACK_X;
	m_LuaManager.MakeTableReference( "ESI_JUMP_ATTACK_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, JumpAttackXEventProcess );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, JumpAttackXFrameMoveFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_COMBO_Z;
	m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_COMBO_X;
	m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_COMBO_Z;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_ZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_COMBO_X;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_COMBO_XX;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_XX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpComboXXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_DASH_JUMP_COMBO_XX_LANDING;
	m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_XX_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, DashJumpComboXXLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_QUICK_STANDUP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "ESI_QUICK_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpAttackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpAttackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_QUICK_STANDUP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "ESI_QUICK_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, StandUpAttackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUEL, StandUpAttackBackEnd );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUEL, StandUpAttackBackStateEndFuture );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESI_SWORD_WAIT;
	m_LuaManager.MakeTableReference( "ESI_SWORD_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, SwordWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, SwordWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, SwordWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	// �ر� - ��(˭)
	// ���� ���������� ����ϱ� ���� ��ų ���� ���� ������ ������Ʈ �ε��ϵ��� ����
	stateData.Init();
	stateData.stateID			= ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE;
	m_LuaManager.MakeTableReference( "ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_StateStart );
	stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_Init );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE_MEMO;
	m_LuaManager.MakeTableReference( "ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE_MEMO", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_StateStart );
	stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_Init );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESI_CAN_NOT_ATTACK_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESI_CAN_NOT_ATTACK_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_CAN_NOT_ATTACK_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;
}

void CX2GUEL::InitStateByUnitClass()
{
	UserUnitStateData stateData;
	switch ( GetUnitClass() )
	{
	case CX2Unit::UC_ELESIS_KNIGHT:
		{

		} break;
	case CX2Unit::UC_ELESIS_GRAND_MASTER:
		{
			// Ŀ�ǵ�, �޺� ����
			// ��� ���� �޺� XZ
			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SGM_XZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SGM_XZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SGM_XZ_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SGM_XZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			// Ŀ�ǵ�, �޺� ����
			// ��� ���� �޺� XZ
			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SGM_XZ_LOOP;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SGM_XZ_LOOP", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SGM_XZ_LOOP_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			// ��� ���� �޺� XZ �ٿ�
			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SGM_XZ_DOWN;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SGM_XZ_DOWN", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SGM_XZ_DOWN_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			// �޺� XX �ٿ� X ����
			stateData.Init();
			stateData.stateID			= ESI_COMBO_SGM_XX_DOWN_X_START;
			m_LuaManager.MakeTableReference( "ESI_COMBO_SGM_XX_DOWN_X_START", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SGM_XX_DOWN_X_START_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SGM_XX_DOWN_X_START_FrameMove );
			stateData.StateStart		= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SGM_XX_DOWN_X_START_StateStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SGM_XX_DOWN_X_START_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			// �޺� XX �ٿ� X ����
			stateData.Init();
			stateData.stateID			= ESI_COMBO_SGM_XX_DOWN_X_LOOP;
			m_LuaManager.MakeTableReference( "ESI_COMBO_SGM_XX_DOWN_X_LOOP", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SGM_XX_DOWN_X_LOOP_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			// �޺� XX �ٿ� X ��
			stateData.Init();
			stateData.stateID			= ESI_COMBO_SGM_XX_DOWN_X_END;
			m_LuaManager.MakeTableReference( "ESI_COMBO_SGM_XX_DOWN_X_END", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SGM_XX_DOWN_X_END_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			// ��� ��� ����, ������� �Ծ� �ڷ� �ڸ� �� ���� �Է� ��, �ش� State �� ��ȯ
			stateData.Init();
			stateData.stateID			= ESI_QUICK_STANDUP_ATTACK_FRONT_ADVANCED;
			m_LuaManager.MakeTableReference( "ESI_QUICK_STANDUP_ATTACK_FRONT_ADVANCED", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_QUICK_STANDUP_ATTACK_FRONT_ADVANCED_EventProcess );

			m_StateList[stateData.stateID] = stateData;

			// ��� ��� ����, ������� �Ծ� ������ �ڸ� �� ���� �Է� ��, �ش� State �� ��ȯ
			stateData.Init();
			stateData.stateID			= ESI_QUICK_STANDUP_ATTACK_BACK_ADVANCED;
			m_LuaManager.MakeTableReference( "ESI_QUICK_STANDUP_ATTACK_BACK_ADVANCED", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_QUICK_STANDUP_ATTACK_BACK_ADVANCED_EventProcess );

			m_StateList[stateData.stateID] = stateData;
			// Ŀ�ǵ�, �޺� ��
		} //break;
	case CX2Unit::UC_ELESIS_SABER_KNIGHT:
		{
			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SSK_ZZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SSK_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SSK_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SSK_ZZZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SSK_ZZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SSK_ZZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SSK_ZZX;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SSK_ZZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SSK_ZZX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SSK_XX;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SSK_XX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SSK_XX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SSK_XXX;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SSK_XXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SSK_XXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;
	case CX2Unit::UC_ELESIS_BLAZING_HEART:
		{
			stateData.Init();
			stateData.stateID			= ESI_COMBO_SBH_XZ;
			m_LuaManager.MakeTableReference( "ESI_COMBO_SBH_XZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZ_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZ_FrameMove );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_COMBO_SBH_XZZ;
			m_LuaManager.MakeTableReference( "ESI_COMBO_SBH_XZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZZ_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_COMBO_SBH_XZZZ;
			m_LuaManager.MakeTableReference( "ESI_COMBO_SBH_XZZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZZZ_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZZZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_COMBO_SBH_XZZZ_FrameMove );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SBH_X;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SBH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_X_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_X_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_X_FrameMove );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_X_FrameMoveFuture );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SBH_XX;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SBH_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XX_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XX_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XX_FrameMove );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XX_FrameMoveFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SBH_XXX;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SBH_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXX_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXX_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXX_FrameMove );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXX_FrameMoveFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SBH_XXXZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SBH_XXXZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXXZ_Init );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXXZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXXZ_FrameMove );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SBH_XXXZ_FrameMoveFuture );
			m_StateList[stateData.stateID] = stateData;
		}
	case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
		{
			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SPK_ZX;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SPK_ZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SPK_ZX_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SPK_ZX_FrameMove );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SPK_ZZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SPK_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SPK_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_COMBO_SPK_ZZZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_COMBO_SPK_ZZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SPK_ZZZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_COMBO_SPK_ZZZ_FrameMove );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SPK_XZ;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SPK_XZ", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SPK_XZ_StartFurture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SPK_XZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SPK_XZ_FrameMove );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESI_DASH_JUMP_COMBO_SPK_XZ_REATTACK;
			m_LuaManager.MakeTableReference( "ESI_DASH_JUMP_COMBO_SPK_XZ", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SPK_XZ_REATTACK_StartFurture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SPK_XZ_EventProcess );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUEL, ESI_DASH_JUMP_COMBO_SPK_XZ_FrameMove );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		ASSERT(!"GetUnitClass Error");
		break;
	}
}


void CX2GUEL::SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, EL_STATE_ID eStateID_ )
{
	if ( NULL != pSkillTemplet_ )
		SetSkillLevelStateData( pSkillTemplet_, m_StateList[eStateID_] );
}

void CX2GUEL::SetSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, UserUnitStateData& stateData_ )
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
void CX2GUEL::ShowActiveSkillCutInAndLight( float fTimeToShow_, bool bOnlyLight_ /*= false*/ )
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
/*virtual*/ void CX2GUEL::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ )
{
	ShowActiveSkillCutInAndLight( fTimeToShow_, bOnlyLight_ );
}
bool CX2GUEL::CanUseSpecialAttack( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition/* = CX2SkillTree::ASUT_GROUND*/ )
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
		if( false == CheckSkillUseCondition( eActiveSkillUseCondition, pSkillTemplet->m_eActiveSkillUseCondtion ) )
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

		// ���� �Ҹ��� ����
		m_fMPConsume = GetActualMPConsume( pSkillTemplet->m_eID, iSkillTempletLevel );

		float fCoolTimeLeft = pSkillSlotData->m_fCoolTimeLeft;

		bool bSlotB = (m_iPressedSkillSlotIndex > 3) ? true : false;
		int iSlotIndex = (m_iPressedSkillSlotIndex > 3) ? m_iPressedSkillSlotIndex-4 : m_iPressedSkillSlotIndex;


#if X2_DEV_TOOLS
		if( false == g_pMain->IsMyAuthLevelHigherThan( CX2User::XUAL_DEV ) )
#endif // _SERVICE_
		{
			if( 0.f < fCoolTimeLeft )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_226 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}
			if ( GetNowMp() < m_fMPConsume )
			{
				g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1),
					D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
				return false;
			}

			if ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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

void CX2GUEL::StateChangeSpecialAttack( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )
{
	if( NULL != GetUnit() )
	{
		CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;

		CountUsedSkill( m_iPressedSkillSlotIndex );
		UpNowMp( -m_fMPConsume );

		if ( pSkillTemplet_->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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

		const int iSkillTempletLevel = max( 1, accessUserSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����
		accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet_->m_eID, GetActualCoolTime( pSkillTemplet_, iSkillTempletLevel) );

		if( false == SpecialAttackNoStageChange( pSkillTemplet_ ) )
		{
			if( GetRemainHyperModeTime() <= 0.f )
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

		m_iNowSpecialAttack = m_iPressedSkillSlotIndex + 1;
		m_bSpecialAttackEventProcessedAtThisFrame = true;
	}

}
/** @function : IsValideSkillRidingOn
	@brief : Ż �� ž���߿� ��� �� �� �ִ� ��ų���� üũ.
	@return : ��� ���� : true ��� �Ұ� falle
*/
/*virtual*/ bool CX2GUEL::IsValideSkillRidingOn( CX2SkillTree::SKILL_ID eSkill_ID_ )
{
	if( true == GetRidingOn() )
	{
		switch( eSkill_ID_ )
		{
		case CX2SkillTree::SI_A_EL_ENDURANCE_POWER: // �ر� - ��(˭)
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
/*virtual*/ void CX2GUEL::ReInit( bool bTeamPos /*= false*/, int iStartPosIndex /*= -1*/ )
{
	CX2GUUser::ReInit( bTeamPos, iStartPosIndex );
	// ���� �� ���� �ִٸ� ���Ǳ� ȿ�� �ʱ�ȭ
	if( true == IsMyUnit() )
	{
		if( CX2Game::GT_PVP == g_pX2Game->GetGameType() )
		{
			const WAY_OF_SWORD_STATE eWayOfSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

			if( GetTeam() == CX2Room::TN_RED && g_pX2Game->GetRedTeamKill() > g_pX2Game->GetBlueTeamKill() ||
				GetTeam() == CX2Room::TN_BLUE && g_pX2Game->GetRedTeamKill() < g_pX2Game->GetBlueTeamKill() )
			{
				if( eWayOfSwordState == WSS_VIGOR )
				{
					End_SwordmanVigor();
				}
				else if( eWayOfSwordState == WSS_DESTRUCTION )
				{
					End_SwordmanDestruction();
				}
				SetWayOfSwordPoint( 0 );
				SetWayOfSwordState( static_cast<int>( WSS_CENTER ) );
			}
			else
			{
				if( eWayOfSwordState == WSS_VIGOR )
				{
					End_SwordmanVigor();
					SetWayOfSwordState( static_cast<int>( WSS_TOWARD_VIGOR ) );
				}
				else if( eWayOfSwordState == WSS_DESTRUCTION )
				{
					End_SwordmanDestruction();
					SetWayOfSwordState( static_cast<int>( WSS_TOWARD_DESTRUCTION ) );
				}
			}
			m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
		}
		else
		{
			switch ( GetWayOfSwordState() )
			{
			case WSS_VIGOR:
				{
					Start_SwordmanVigor();
				} break;
			case WSS_DESTRUCTION:
				{
					Start_SwordmanDestruction();
				} break;
			default:
				break;
			}
		}
	}

}
/*virtual*/ void CX2GUEL::DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ )
{
	KTDXPROFILE();

	if( WSS_DESTRUCTION == GetWayOfSwordState() && // ���� ���� �� �� ���� ����ó��
		true == IsDamagedReaction( pDamageData_ ) &&
		true == DamageFlushMp() )
	{
		// ���� ���� �� Damage�� ���� �ʵ��� �ϱ� ���� DamageReact Ÿ�� �ʵ��� ����ó��
		DetonationStateChange( pDamageData_ );
	}
	// �ر� - ��(˭)
	else if( true == m_bIsActiveEndurancePower &&
			 true == CanSpecialReactProcess( pDamageData_->reActType )
			&& false == IsSuperArmor()
			)
	{
		DoStateEndurancePowerRage( false );
	}
	else
	{
		CX2GUUser::DamageReact( pDamageData_ );
		CX2GUUser::DamageReactStateChange( pDamageData_ );
	}


	if(	IsMyUnit() == true &&
		pDamageData_->reActResult != CX2DamageManager::RT_REVENGE &&
		true == CanSpecialReactProcess(pDamageData_->reActType) )
	{
		const int iWayOfSwordState = GetWayOfSwordState();
		if( WSS_VIGOR != iWayOfSwordState )
		{ // ���� ���Ǳ��� ��ǳ ���¿��� �Ҹ��Ű�� ����.
			const float fWSPGain =
				g_pData->GetDamageManager()->CalcWayOfSwordPointOnDamageReact( pDamageData_->damage.GetAllDamage(), iWayOfSwordState );

			SetWayOfSwordPoint( GetWayOfSwordPoint() + fWSPGain );
		}
	}
	if ( true == GetIsProvokeStateChange() )
	{
		StateChange( ESI_A_SGM_PROVOKE_REVENGE, true );
		SetIsProvokeStateChange( false );
	}
}
/*virtual*/ void CX2GUEL::Win()
{
	CX2GUUser::Win();
	StateChange( GetWinStateID() );
}
/*virtual*/ void CX2GUEL::Lose()
{
	CX2GUUser::Lose();

	if ( true == IsMyUnit() && 0.f < GetNowHp() )
	{
		StateChange( GetLoseStateID() );
	}
}
/*virtual*/ void CX2GUEL::Draw()
{
	CX2GUUser::Draw();
	StateChange( GetWinStateID() );
}
/*virtual*/ void CX2GUEL::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();
}
/*virtual*/ void CX2GUEL::InitEnchantWeaponEffectUnitSpecific()
{
	for( Weapon* pWeapon : m_vecpWeapon )
	{
		ASSERT( NULL != pWeapon );
		pWeapon->InitEnchantParticleForSword();
	}
}
/*virtual*/ void CX2GUEL::CreateMajorParticleArray()
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
/*virtual*/ void CX2GUEL::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();
}
/*static*/ void CX2GUEL::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
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
/*static*/ void CX2GUEL::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{

}
/*virtual*/ bool CX2GUEL::SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )
{
	if( NULL == pSkillTemplet_ )
		return false;

	switch( pSkillTemplet_->m_eID )
	{
	case CX2SkillTree::SI_A_EL_ENDURANCE_POWER:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ENDURANCE_POWER;
		} break;
	default:
		return false;
		break;
	}
	return true;
}
/*virtual*/ void CX2GUEL::COMMON_BUFF_FrameMove()
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
/*virtual*/ void CX2GUEL::HyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.05f, 0.83f );
}
/*virtual*/ void CX2GUEL::HyperModeBuffEffectStart()
{
	CX2GUUser::HyperModeBuffEffectStart();

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRFoot )
		m_hHyperBoostRFoot	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRight",	0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLFoot )
		m_hHyperBoostLFoot	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeft",	0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRArm )
		m_hHyperBoostRArm	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRight",	0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLArm )
		m_hHyperBoostLArm	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeft",	0, 0, 0, 0, 0);
}
/** @function	: SetSpecificValueByBuffTempletID
	@brief		: �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param		: �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUEL::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

 	switch ( eBuffTempletId_ )
 	{
	// �ر� - ��(˭)
	case BTI_BUFF_INDURANCE_POWER:
		{
			m_bIsActiveEndurancePower = true;
		} break;
	// �¸��� ��
	case BTI_BUFF_VICTORIOUS_SWORD:
	case BTI_BUFF_SGM_VICTORIOUS_SWORD:
		{
			// ����&������ ��Ÿ�� ���� ���� ����
			{
				m_fDodgeAndSlashCoolTimeDecreaseRate = 1.f;

				const CX2SkillTree::SKILL_ID eSkillID = CX2SkillTree::SI_SA_SSK_VICTORIOUS_SWORD;
				const int iSkillLevel = GetMyUnitSkillLevel(eSkillID);
				if ( 0 < iSkillLevel )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID );
					if( NULL != pSkillTemplet )
					{
						// ����&������ ��Ÿ�� ���� ����
						m_fDodgeAndSlashCoolTimeDecreaseRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DECREASE_REL, iSkillLevel );
					}
				}
			}

			// ����&������ ��Ÿ��, MP ���� ��� Ȱ��ȭ�� ���� ���� ����
			m_bIsApplyVictoriousSword = true;
		} break;
	// ȫ���� �� Ȱ��ȭ
	case BTI_BUFF_FLAME_SWORD:
		{
			m_bIsFrameSword = true;
		} break;
 	default:
 		break;
 	}

	//��ų�� �������, üũ Ÿ���� ���� Ȱ��ȭ �Ǿ����� üũ
	if( 0 < m_uiFireBlossomsSkillLevel && true == m_bIsFireBlossoms )
	{
		// ������� �ɷ��������� üũ
		CX2BuffTempletPtr ptrBuffTemplet = CX2BuffTempletManager::GetInstance()->GetBuffTempletPtr( eBuffTempletId_ );
		if( NULL != ptrBuffTemplet && true == ptrBuffTemplet->IsDeBuff() && false == ptrBuffTemplet->IsWorldBuff() )
		{
			// �ൿ �Ҵ� ������� ���� ���� �߰�
			if ( false == ptrBuffTemplet->IsNonControlDebuff() )
			{
				// ���� SetSpecificValueByBuffTempletID �Լ� ���Ŀ� ���ֿ� ������ �����Ű�� vector�� eBuffTempletId_�� �ִ´�.
				// ���� ���⼭ ������ �����ִ� �� �ǹ̰� ���� ���߿� �����ֱ� ���� eBuffTempletId_�� ������ ���´�.
				m_eFireBlossomsSaveDebuffID = eBuffTempletId_;
			}
		}
	}
}
/** @function	: UnSetSpecificValueByBuffTempletID
	@brief		: �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param		: �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUEL::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	// �ر� - ��(˭)
	case BTI_BUFF_INDURANCE_POWER:
		{
			m_bIsActiveEndurancePower = false;
		} break;
	// �¸��� ��
	case BTI_BUFF_VICTORIOUS_SWORD:
	case BTI_BUFF_SGM_VICTORIOUS_SWORD:
		{
			// ����&������ ��Ÿ��, MP ���� ��� ��Ȱ��ȭ�� ���� ���� ����
			m_fDodgeAndSlashCoolTimeDecreaseRate = 1.f;
			m_bIsApplyVictoriousSword = false;
		} break;
	// �Ҳ� ��ȭ - ������ �����Ǹ� ���� ��ġ�� �Ҳ� ��ȭ npc�� �����Ѵ�.
	case BTI_BUFF_FIRE_BLOSSOMS:
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_P_SBH_FIRE_BLOSSOMS_Fire", this );

			D3DXVECTOR3 vPos = GetLandPosition();
			const int iHyperMode = 0;
			g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_ELESIS_FIRE_BLOSSOM, iHyperMode, true, vPos,
				GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		} break;
	// ȫ���� �� - ��Ȱ��ȭ
	case BTI_BUFF_FLAME_SWORD:
		{
			m_bIsFrameSword = false;
		} break;
	default:
		break;
	}
}


// ���� ��
/*virtual*/ int	CX2GUEL::GetWayOfSwordState() const
{
	if( NULL != GetELGageData() )
		return GetELGageData()->GetWayOfSwordState();

	return 0;
}
void CX2GUEL::SetWayOfSwordState( const int iWayOfSwordState_ )
{
	if( NULL != GetELGageData() )
		GetELGageData()->SetWayOfSwordState( iWayOfSwordState_ );
}
/*virtual*/ float CX2GUEL::GetWayOfSwordPoint() const
{
	if( NULL != GetELGageData() )
		return GetELGageData()->GetWayOfSwordPoint();

	return 0.f;
}

/*virtual*/ void CX2GUEL::SetWayOfSwordPoint( float fWayOfSwordPoint_ )
{
	if( fWayOfSwordPoint_ > g_pData->GetDamageManager()->GetVigorStateThreshold() )
		fWayOfSwordPoint_ = g_pData->GetDamageManager()->GetVigorStateThreshold();

	if( fWayOfSwordPoint_ < g_pData->GetDamageManager()->GetDestStateThreshold() )
		fWayOfSwordPoint_ = g_pData->GetDamageManager()->GetDestStateThreshold();

	if( NULL != GetELGageData() )
		GetELGageData()->SetWayOfSwordPoint( fWayOfSwordPoint_ );
}
// ��ǳ ����
void CX2GUEL::Start_SwordmanVigor()
{
	if ( NULL != g_pX2Game->GetWorld() && GetShowCutInAndChangeWorldColor() )
	{
		g_pX2Game->GetWorld()->SetWorldColor( 0xff222255 );
		g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 4.0f );
	}

	SetBuffFactorToGameUnitByBuffFactorID(BFI_BUFF_GALE_WAY_OF_SWORD);
	PlaySound( L"elsword_vitality_use_VOX.ogg" );

	m_CommonState.m_Wait = ESI_SWORD_WAIT;
}
// ��ǳ ����
void CX2GUEL::End_SwordmanVigor()
{
	EraseBuffTempletFromGameUnit(BTI_BUFF_GALE_WAY_OF_SWORD);

	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_VigorEnd", this );
	PlaySound( L"elsword_vitality_end.ogg" );

	m_CommonState.m_Wait = USI_WAIT;
}
// ���� ����
void CX2GUEL::Start_SwordmanDestruction()
{
	if ( NULL != g_pX2Game->GetWorld() && GetShowCutInAndChangeWorldColor() )
	{
		g_pX2Game->GetWorld()->SetWorldColor( 0xff552222 );
		g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 4.0f );
	}

	if( NULL != g_pX2Game && CX2Game::GT_PVP == g_pX2Game->GetGameType() )
		SetBuffFactorToGameUnitByBuffFactorID(BFI_BUFF_ANNIHILATION_WAY_OF_SWORD_PVP);
	else
		SetBuffFactorToGameUnitByBuffFactorID(BFI_BUFF_ANNIHILATION_WAY_OF_SWORD);

	PlaySound( L"elsword_demolition_use_VOX.ogg" );

	m_CommonState.m_Wait = ESI_SWORD_WAIT;
}
// ���� ����
void CX2GUEL::End_SwordmanDestruction()
{
	SetNowChargeMpForDetonation( 0.f ) ;
	EraseBuffTempletFromGameUnit(BTI_BUFF_ANNIHILATION_WAY_OF_SWORD);

	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_DestructionEnd", this );
	PlaySound( L"elsword_demolition_end.ogg" );

	m_CommonState.m_Wait = USI_WAIT;
}

void CX2GUEL::WayOfSwordStateChange()
{
	int iNextWayOfSwordState = 0;
	if( IsMyUnit() == true )
	{
		const float fWayOfSwordPoint = GetWayOfSwordPoint();
		if( fWayOfSwordPoint >= g_pData->GetDamageManager()->GetVigorStateThreshold() )
			iNextWayOfSwordState = WSS_VIGOR;
		else if( fWayOfSwordPoint <= g_pData->GetDamageManager()->GetDestStateThreshold() )
			iNextWayOfSwordState = WSS_DESTRUCTION;
		else if( fWayOfSwordPoint > 0 )
			iNextWayOfSwordState = WSS_TOWARD_VIGOR;
		else if( fWayOfSwordPoint < 0 )
			iNextWayOfSwordState = WSS_TOWARD_DESTRUCTION;
		else if( fWayOfSwordPoint == 0 )
			iNextWayOfSwordState = WSS_CENTER;
	}
	else
	{
		iNextWayOfSwordState = m_FrameDataNow.syncData.m_CannonBallCount;
	}

	const WAY_OF_SWORD_STATE eWayofSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

	if( iNextWayOfSwordState == WSS_VIGOR &&
		eWayofSwordState != WSS_VIGOR )
	{
		if( eWayofSwordState == WSS_DESTRUCTION )
			End_SwordmanDestruction();

		Start_SwordmanVigor();
		SetWayOfSwordState( static_cast<int>( WSS_VIGOR ) );
	}
	else if( iNextWayOfSwordState == WSS_DESTRUCTION &&
		eWayofSwordState != WSS_DESTRUCTION )
	{
		if( eWayofSwordState == WSS_VIGOR )
			End_SwordmanVigor();

		Start_SwordmanDestruction();
		SetWayOfSwordState( static_cast<int>( WSS_DESTRUCTION ) );
	}
	else if( iNextWayOfSwordState == WSS_TOWARD_VIGOR &&
		eWayofSwordState != WSS_TOWARD_VIGOR && eWayofSwordState != WSS_VIGOR )
	{
		if( eWayofSwordState == WSS_DESTRUCTION )
			End_SwordmanDestruction();

		SetWayOfSwordState( static_cast<int>( WSS_TOWARD_VIGOR ) );
	}
	else if( iNextWayOfSwordState == WSS_TOWARD_DESTRUCTION &&
		eWayofSwordState != WSS_TOWARD_DESTRUCTION && eWayofSwordState != WSS_DESTRUCTION )
	{
		if( eWayofSwordState == WSS_VIGOR )
			End_SwordmanVigor();

		SetWayOfSwordState( static_cast<int>( WSS_TOWARD_DESTRUCTION ) );
	}
	else if( iNextWayOfSwordState == WSS_CENTER &&
		eWayofSwordState != WSS_CENTER )
	{
		if( eWayofSwordState == WSS_VIGOR )
			End_SwordmanVigor();
		else if( eWayofSwordState == WSS_DESTRUCTION )
			End_SwordmanDestruction();

		SetWayOfSwordState( static_cast<int>( WSS_CENTER ) );
	}
	else // Do Nothing
	{
		;
	}
}
/** @function : GaleBuffFrameMove()
	@brief : �� 5ȸ ��Ʈ�� MPȸ���� ���� ���� ���� ��Ű�� ���� ������ ����
*/
void CX2GUEL::GaleBuffFrameMove()
{
	if( WSS_VIGOR != GetWayOfSwordState() )
		return;

	const int iComboCount = ( NULL != GetComboManager() ? GetComboManager()->GetCombo() : 0 );

	// ��Ʈ ī��Ʈ ��ȭ�� ���ٸ� �������� ����
	if( iComboCount == m_iPreComboCount )
		return;

	m_iPreComboCount = iComboCount;

	// �� 5ȸ ��Ʈ �� MP ȸ���� ���� ���� ����
	if( 0 < iComboCount && 0 == ( iComboCount % 5) )
	{
		SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_GALE_MP_GAIN_INCREASE );
	}
}
void CX2GUEL::SwordWaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationSwordWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );
}

void CX2GUEL::SwordWaitStart()
{
	CommonStateStart();
	PlayAnimationSwordWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}

void CX2GUEL::SwordWaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESI_COMBO_X );
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
	else if( GetWayOfSwordState() != WSS_VIGOR && GetWayOfSwordState() != WSS_DESTRUCTION )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

void CX2GUEL::PlayAnimationSwordWaitStart( CKTDGXSkinAnim* pXSkinAnim_, const FrameData& refFrameData_ )
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
			if( GetWayOfSwordState() == WSS_VIGOR )
				pXSkinAnim_->ChangeAnim( L"Wait_W0", false );
			else if( GetWayOfSwordState() == WSS_DESTRUCTION )
				pXSkinAnim_->ChangeAnim( L"Wait_W1", false );
			else
				pXSkinAnim_->ChangeAnim( L"Wait", false );
		}
		else
		{
			if( GetWayOfSwordState() == WSS_VIGOR )
				pXSkinAnim_->ChangeAnim( L"Wait_W0", true );
			else if( GetWayOfSwordState() == WSS_DESTRUCTION )
				pXSkinAnim_->ChangeAnim( L"Wait_W1", true );
			else
				pXSkinAnim_->ChangeAnim( L"Wait", true );
		}
	}

	pXSkinAnim_->Play( playType );
}
bool CX2GUEL::CanSpecialReactProcess( CX2DamageManager::REACT_TYPE eReActType_ )
{
	if( 0.f >= GetNowHp() )
		return false;

	if( true == GetInvincible() )
		return false;

	switch( eReActType_ )
	{
	case CX2DamageManager::RT_NO_DAMAGE:
	case CX2DamageManager::RT_DUMMY_DAMAGE:
	case CX2DamageManager::RT_SENSOR:
		return false;
	default:
		break;
	}

	return true;
}
/** @function : GetFinalDamage_WayOfSword
	@brief : -GetFinalDamage ��� �� �� ���Ǳ� Ÿ�Կ� ���� ������ ���� ���� �ֱ� ���� �Լ�
			 -���ҵ�� �����ý��� �ٸ� ������ �� �� �ֵ��� virtual �Լ��� ����
*/
/*virtual*/float CX2GUEL::GetFinalDamage_WayOfSword( CX2DamageManager::WAY_OF_SWORD_TYPE eWayOfSwordType_ )
{
	float fDamageMagnifications = 1.f;
	if( WSS_DESTRUCTION == GetWayOfSwordState() )
	{
		switch( eWayOfSwordType_ )
		{
		case CX2DamageManager::WST_DESTRUCTION_NORMAL:
			{
				if( CX2Game::GT_PVP == g_pX2Game->GetGameType() )
					fDamageMagnifications = 1.25f;
				else
					fDamageMagnifications = 1.5f;
			} break;
		default:
			break;
		}
	}

	return fDamageMagnifications;
}
/** @function : GetCriticalPercent_WayOfSword
	@brief : ���Ǳ� Ÿ�Կ� ���� ũ��Ƽ�� ���� ����
*/
/*virtual*/ void CX2GUEL::SetCriticalPercent_WayOfSword( IN CX2DamageManager::WAY_OF_SWORD_TYPE eWayOfSwordType_ ,
												OUT float& fCriticalPercent_)
{
	if( WSS_DESTRUCTION == GetWayOfSwordState() )
	{
		switch( eWayOfSwordType_ )
		{
		case CX2DamageManager::WST_DESTRUCTION_NORMAL:
		case CX2DamageManager::WST_DESTRUCTION_ACTIVE:
		case CX2DamageManager::WST_DESTRUCTION_SPECIAL:
			{
				fCriticalPercent_ = 1.f;
			} break;
		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������Ʈ �Լ�
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ESI_DIE_FRONT
void CX2GUEL::DieFrontStartFuture()
{
	CommonStateStartFuture();

	const WAY_OF_SWORD_STATE eWayofSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

	if( eWayofSwordState == WSS_VIGOR )
		End_SwordmanVigor();
	else if( eWayofSwordState == WSS_DESTRUCTION )
		End_SwordmanDestruction();

}
void CX2GUEL::DieFrontStart()
{
	CommonStateStart();
	DyingStart();
	m_pXSkinAnim->SetPlaySpeed( 0.5f );
}
void CX2GUEL::DieFrontFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}
void CX2GUEL::DieFrontEventProcess()
{
	if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
	{
		StateChange( GUSI_DIE );
	}
}

//ESI_DIE_BACK
void CX2GUEL::DieBackStartFuture()
{
	CommonStateStartFuture();

	const WAY_OF_SWORD_STATE eWayofSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

	if( eWayofSwordState == WSS_VIGOR )
		End_SwordmanVigor();
	else if( eWayofSwordState == WSS_DESTRUCTION )
		End_SwordmanDestruction();

}
void CX2GUEL::DieBackStart()
{
	CommonStateStart();
	DyingStart();
	m_pXSkinAnim->SetPlaySpeed( 0.5f );
}
void CX2GUEL::DieBackFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}
void CX2GUEL::DieBackEventProcess()
{
	if ( GetShowObject() == false  && m_pXSkinAnim->IsAnimationEnd() == true )
	{
		StateChange( GUSI_DIE );
	}
}

//ESI_WAIT
void CX2GUEL::WaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );
}
void CX2GUEL::WaitStart()
{
	CommonStateStart();
	PlayAnimationWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}
void CX2GUEL::WaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESI_COMBO_X );
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
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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
	else if( ( GetWayOfSwordState() == WSS_VIGOR || GetWayOfSwordState() == WSS_DESTRUCTION ) &&
			m_FrameDataNow.unitCondition.fStateTime >= 0.1f )
	{
		StateChange( ESI_SWORD_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}
	CommonEventProcess();
}

//ESI_WALK
void CX2GUEL::WalkEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESI_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESI_COMBO_X );
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
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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

//ESI_JUMP_READY
void CX2GUEL::JumpReadyEventProcess()
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

//ESI_JUMP_UP
void CX2GUEL::JumpUpEventProcess()
{
	if( (m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true ) &&
		GetEnableDash() == true )
	{
		StateChange( USI_DASH_JUMP );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ESI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ESI_JUMP_ATTACK_X );
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

//ESI_JUMP_DOWN
void CX2GUEL::JumpDownEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if( (m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true ) &&
		GetEnableDash() == true )
	{
		StateChange( USI_DASH_JUMP );
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ESI_WALL_LANDING );
	}
	else if ( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ESI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ESI_JUMP_ATTACK_X );
	}
	else if ( 0.0f <= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_UP );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

//ESI_JUMP_LANDING
void CX2GUEL::JumpLandingStartFuture()
{
	m_bIsDashAttackPossible = true;
	CommonStateStartFuture();
}
void CX2GUEL::JumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
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
		StateChange( ESI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ESI_COMBO_X );
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
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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

//ESI_DASH
void CX2GUEL::DashEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESI_DASH_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		if( GetUnitClass() == CX2Unit::UC_ELESIS_BLAZING_HEART )
		{
			if( m_fDashComboStartDelay < 0.f )
				StateChange( ESI_DASH_COMBO_SBH_X );
		}
		else
		StateChange( ESI_DASH_COMBO_X );
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.oneUp )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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

//ESI_DASH_END
void CX2GUEL::DashEndStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = GetDashSpeed();
}
void CX2GUEL::DashEndFrameMove()
{
	if( m_pXSkinAnim->EventTimer( 0.4f ) == true )
		CreateStepDust();

	CommonFrameMove();
}
void CX2GUEL::DashEndEventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() * TRIGGER_TIME_FACTOR_DASH_ATTACK )
		{
			StateChange( ESI_COMBO_Z );
		}
		else
		{
			StateChange( ESI_DASH_COMBO_Z );
		}
	}
	else if( m_InputData.oneX == true )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() * TRIGGER_TIME_FACTOR_DASH_ATTACK )
		{
			StateChange( ESI_COMBO_X );
		}
		else
		{
			if( GetUnitClass() == CX2Unit::UC_ELESIS_BLAZING_HEART )
				StateChange( ESI_DASH_COMBO_SBH_X );
			else
			StateChange( ESI_DASH_COMBO_X );
		}
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
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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

//ESI_DASH_JUMP
void CX2GUEL::DashJumpStartFuture()
{
	CommonStateStartFuture();

	const float MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL = 1.2f;

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL;
	m_bDownForce					= false;

}
void CX2GUEL::DashJumpStart()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
		CreateStepDust();
}
void CX2GUEL::DashJumpFrameMoveFuture()
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
void CX2GUEL::DashJumpEventProcess()
{
	bool &bCanDashJump = m_FrameDataFuture.stateParam.bEventFlagList[0];

		if( true == IsOnSomethingFuture() )
	{
		// ���� ȭ��ǥ�� �ѹ� �� ��������
		if( true == bCanDashJump )
		{
			StateChange( USI_DASH_JUMP );	// ����ؼ� ��� ����
			m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
			m_bDownForce = false;
		}
		else if ( true == m_bDownForce )
		{
			StateChange( ESI_DASH_JUMP_POWER_LANDING );
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
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ESI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ && true == m_bIsDashAttackPossible)
	{
		StateChange( ESI_DASH_JUMP_COMBO_Z );
	}
	else if ( true == m_InputData.oneX && true == m_bIsDashAttackPossible)
	{
		StateChange( ESI_DASH_JUMP_COMBO_X );
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

	if( true == m_InputData.oneUp  && 0.0f >= m_PhysicParam.nowSpeed.y  )
	{
		bCanDashJump = true;
	}
	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
		bCanDashJump = false;
	}

	CommonEventProcess();
}
void CX2GUEL::DashJumpEndFuture()
{
	CommonStateEndFuture();
	m_PhysicParam.fDownAccel	= 0.0f;
}

//ESI_DASH_JUMP_LANDING
void CX2GUEL::DashJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ESI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ESI_COMBO_X );
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
		StateChange( USI_WALK );
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
void CX2GUEL::DashJumpLandingStartFuture()
{
	CX2GUUser::DashJumpLandingStartFuture();
	m_bIsDashAttackPossible = true;
}
//ESI_DAMAGE_SMALL_FRONT
//ESI_DAMAGE_SMALL_BACK
void CX2GUEL::DamageSmallEventProcess()
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

//ESI_DAMAGE_BIG_FRONT
void CX2GUEL::DamageBigStart()
{
	CommonStateStart();
	CreateStepDust();
}
void CX2GUEL::DamageBigEventProcess()
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

//ESI_DAMAGE_DOWN_FRONT
void CX2GUEL::DamageDownFrontFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
		CreateStepDust();

	CommonFrameMove();
}
void CX2GUEL::DamageDownFrontEventProcess()
{
	bool& bChangeStandUpAttackFront			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingFrontFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingFrontBack	= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpBigAttackFront		= m_FrameDataFuture.stateParam.bEventFlagList[4];

	float fInputStartTime = ( true == m_bAbleQuickStand ) ? m_fEventTime[0].keyInputStart : 0.33f;

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
			StateChange( ESI_STANDUP_ATTACK_FRONT );
		}
		else if ( true == bChangeStandUpBigAttackFront )
		{
			if( GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent )
			{
				StateChange( ESI_QUICK_STANDUP_ATTACK_FRONT );
			}
			else
			{
				StateChange( ESI_STANDUP_ATTACK_FRONT );
			}
		}
		else if( true == bChangeStandUpRollingFrontFront )
		{
			StateChange( ESI_STANDUP_ROLLING_FRONT_FRONT );
		}
		else if( true == bChangeStandUpRollingFrontBack )
		{
			StateChange( ESI_STANDUP_ROLLING_FRONT_BACK );
		}
		else
		{
			StateChange( USI_DAMAGE_STANDUP_FRONT );
		}
	}

	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 5, 0, true == m_bAbleQuickStand && true == bChangeStandUpRollingFrontFront, ESI_STANDUP_ROLLING_FRONT_FRONT )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 6, 0, true == m_bAbleQuickStand && true == bChangeStandUpRollingFrontFront, ESI_STANDUP_ROLLING_FRONT_BACK )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 7, 0, true == m_bAbleQuickStand && true == bChangeStandUpRollingFrontFront, ESI_STANDUP_ATTACK_FRONT )

	else if( fInputStartTime < m_pXSkinAnimFuture->GetNowAnimationTime() )
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

//ESI_DAMAGE_DOWN_BACK
void CX2GUEL::DamageDownBackFrameMove()
{
	if( true == m_pXSkinAnim->EventTimer( 0.32f ) )
		CreateStepDust();

	CommonFrameMove();
}
void CX2GUEL::DamageDownBackEventProcess()
{
	bool& bChangeStandUpAttackBack			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingBackFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingBackBack		= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpBigAttackBack		= m_FrameDataFuture.stateParam.bEventFlagList[4];
	// �Ѿ����µ� �ؿ� ���θ� ���� ���� ���

	float fInputStartTime = ( true == m_bAbleQuickStand ) ? m_fEventTime[0].keyInputStart : 0.33f;
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
			StateChange( ESI_STANDUP_ATTACK_BACK );
		}
		// X Ű�� ������ ��
		else if ( true == bChangeStandUpBigAttackBack )
		{
			if( GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent )
			{
				StateChange( ESI_QUICK_STANDUP_ATTACK_BACK );
			}
			else
			{
				StateChange( ESI_STANDUP_ATTACK_BACK );
			}
		}
		else if( true == bChangeStandUpRollingBackFront )
		{
			StateChange( ESI_STANDUP_ROLLING_BACK_FRONT );
		}
		else if( true == bChangeStandUpRollingBackBack )
		{
			StateChange( ESI_STANDUP_ROLLING_BACK_BACK );
		}
		else
		{
			StateChange( USI_DAMAGE_STANDUP_BACK );
		}
	}

	ELSE_IF_STATE_CHANGE_ON_EX_( 5, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpRollingBackFront, ESI_STANDUP_ROLLING_BACK_FRONT )
	ELSE_IF_STATE_CHANGE_ON_EX_( 6, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpRollingBackBack, ESI_STANDUP_ROLLING_BACK_BACK )
	ELSE_IF_STATE_CHANGE_ON_EX_( 7, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpAttackBack, ESI_STANDUP_ATTACK_BACK )
	else if( fInputStartTime < m_pXSkinAnimFuture->GetNowAnimationTime() )
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

//ESI_DAMAGE_AIR_DOWN_LANDING
void CX2GUEL::DamageAirDownLandingEventProcess()
{
	DamageDownFrontEventProcess();
}

//ESI_STANDUP_ROLLING_FRONT_FRONT
void CX2GUEL::StandUpRollingFrontFrontEventProcess()
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
			StateChangeDashIfPossible();
		}
		else
			StateChange( USI_WAIT );
	}
	// ��� ��� �������� ���� ����� ��� ����
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_InputData.oneX && true == m_bAbleQuickStandAttack, ESI_QUICK_STANDUP_ATTACK_BACK_ADVANCED )

	CommonEventProcess();
}
void CX2GUEL::StandUpRollingFrontFrontEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_STANDUP_ROLLING_FRONT_BACK
void CX2GUEL::StandUpRollingFrontBackEventProcess()
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
			StateChangeDashIfPossible();
		}
		else
			StateChange( USI_WAIT );
	}
	// ��� ��� �������� ���� ����� ��� ����
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_InputData.oneX && true == m_bAbleQuickStandAttack, ESI_QUICK_STANDUP_ATTACK_FRONT_ADVANCED )

	CommonEventProcess();
}
void CX2GUEL::StandUpRollingFrontBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_STANDUP_ROLLING_BACK_FRONT
void CX2GUEL::StandUpRollingBackFrontEventProcess()
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
	// ��� ��� �������� ���� ����� ��� ����
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_InputData.oneX && true == m_bAbleQuickStandAttack, ESI_QUICK_STANDUP_ATTACK_BACK_ADVANCED )

	CommonEventProcess();
}
void CX2GUEL::StandUpRollingBackFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_STANDUP_ROLLING_BACK_BACK
void CX2GUEL::StandUpRollingBackBackEventProcess()
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
	// ��� ��� �������� ���� ����� ��� ����
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_InputData.oneX && true == m_bAbleQuickStandAttack, ESI_QUICK_STANDUP_ATTACK_FRONT_ADVANCED )

	CommonEventProcess();
}
void CX2GUEL::StandUpRollingBackBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_STANDUP_ATTACK_FRONT
void CX2GUEL::StandUpAttackFrontEventProcess()
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
void CX2GUEL::StandUpAttackFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_STANDUP_ATTACK_BACK
void CX2GUEL::StandUpAttackBackEventProcess()
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
void CX2GUEL::StandUpAttackBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}
void CX2GUEL::StandUpAttackBackStateEndFuture()
{
	m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
	CommonStateEndFuture();
}
//ESI_STANDUP_ATTACK_FRONT_NO_DOWN
void CX2GUEL::StandUpAttackFrontNoDownEventProcess()
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
void CX2GUEL::StandUpAttackFrontNoDownEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_STANDUP_ATTACK_BACK_NO_DOWN
void CX2GUEL::StandUpAttackBackNoDownEventProcess()
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
void CX2GUEL::StandUpAttackBackNoDownEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

//ESI_DASH_JUMP_POWER_LANDING
void CX2GUEL::DashJumpPowerLandingStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x /= 2.0f;
}
void CX2GUEL::DashJumpPowerLandingStart()
{
	CommonStateStart();
	CreateStepDust();
}
void CX2GUEL::DashJumpPowerLandingEventProcess()
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
			StateChange( ESI_COMBO_Z );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( ESI_COMBO_X );
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
void CX2GUEL::ComboZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_COMBO_ZZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_COMBO_ZZZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_COMBO_ZZZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneX == true, ESI_COMBO_ZZZX )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboZZZZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		// Speed_Time �� ���, End ���� ó�����ִ� ����� ���
		// �ش� ��� �� ���� �ʿ��� �������� Speed �� ��ü�� 550 ���� ����
		m_PhysicParam.nowSpeed.x = 550.f;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboZZZXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneX == true, ESI_COMBO_XX )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneZ == true && CX2Unit::UC_ELESIS_BLAZING_HEART == GetUnitClass() ,
										ESI_COMBO_SBH_XZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 2, 2, m_InputData.pureDown == true && m_InputData.oneX == true &&
		CX2Unit::UC_ELESIS_GRAND_MASTER == GetUnitClass(), ESI_COMBO_SGM_XX_DOWN_X_START )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneX == true, ESI_COMBO_XXX )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneZ == true, ESI_COMBO_XXZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboXXXEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ComboXXZEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::JumpAttackZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			m_fCanNotJumpTime = 0.2f;
		}
		StateChange( USI_JUMP_LANDING );
	}
	// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
	else if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
		( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	CommonEventProcess();
}
void CX2GUEL::JumpAttackZFrameMoveFuture()
{
	if( m_InputData.pureDown == true )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	CommonFrameMoveFuture();
}
void CX2GUEL::JumpAttackXEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			m_fCanNotJumpTime = 0.2f;
		}
		StateChange( USI_JUMP_LANDING );
	}
	// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
	else if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
		( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	CommonEventProcess();
}
void CX2GUEL::JumpAttackXFrameMoveFuture()
{
	if( m_InputData.pureDown == true )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	CommonFrameMoveFuture();
}
void CX2GUEL::DashComboZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneZ == true && GetUnitClassLine( UCL_FIRST ) ,
		ESI_DASH_COMBO_SSK_ZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 2, 2, m_InputData.oneZ == true && GetUnitClassLine( UCL_SECOND ) ,
										ESI_DASH_COMBO_SPK_ZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_DASH_COMBO_ZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 3, 3, true == m_InputData.oneX && GetUnitClassLine( UCL_SECOND ) ,
										ESI_DASH_COMBO_SPK_ZX )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::DashComboZZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::DashComboXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::DashJumpComboZEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_DASH_JUMP_COMBO_ZZ );

	CommonEventProcess();
}
void CX2GUEL::DashJumpComboZZEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_DASH_JUMP_COMBO_ZZZ )

	CommonEventProcess();
}
void CX2GUEL::DashJumpComboZZZEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == IsOnSomethingFuture() )
		{
			StateChange( USI_DASH_JUMP_LANDING );
		};
	}

	CommonEventProcess();
}
void CX2GUEL::DashJumpComboXEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, true == m_InputData.oneX && GetUnitClassLine( UCL_FIRST ),
		ESI_DASH_JUMP_COMBO_SSK_XX )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 2, 2, true == m_InputData.oneZ && GetUnitClassLine( UCL_SECOND ),
										ESI_DASH_JUMP_COMBO_SPK_XZ )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, true == m_InputData.oneX , ESI_DASH_JUMP_COMBO_XX )

	ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange,
	m_InputData.oneZ == true && CX2Unit::UC_ELESIS_GRAND_MASTER == GetUnitClass(), ESI_DASH_JUMP_COMBO_SGM_XZ )

	CommonEventProcess();
}
void CX2GUEL::DashJumpComboXXEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( ESI_DASH_JUMP_COMBO_XX_LANDING );
	}

	CommonEventProcess();
}
void CX2GUEL::DashJumpComboXXLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_CAN_NOT_ATTACK_JUMP_DOWN_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if( (m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true ) &&
		GetEnableDash() == true )
	{
		StateChange( USI_DASH_JUMP );
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ESI_WALL_LANDING );
	}
	else if ( 0.0f <= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_UP );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ų ������Ʈ �Լ� - ������
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ����&������
void CX2GUEL::ESI_SI_A_EL_DODGE_AND_SLASH_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"RingAlpha.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"waldo_trock_specialAttackB_particle.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}
void CX2GUEL::ESI_SI_A_EL_DODGE_AND_SLASH_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_DODGE_AND_SLASH );
		if( NULL != pSkillTemplet )
		{	// ȸ�� ���� ����
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_SI_A_EL_DODGE_AND_SLASH_StartFuture()
{
	CommonStateStartFuture();
	// ��ųŰ ���Է� üũ�� ���� �Է��� ��ųŰ�� ���� ���� ����
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );

	// ���� Ƚ�� �ʱ�ȭ
	m_uiDodgeAndSlashReuseCount = m_uiDodgeAndSlashMaxReuseCount;
}
void CX2GUEL::ESI_SI_A_EL_DODGE_AND_SLASH_EventProcess()
{
	// ��ų ���� ü���� ��� ���� üũ
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if ( m_uiDodgeAndSlashReuseCount > 0 &&
			  true == m_sCurrentSkillKeyManager.IsPressedCurrentSkillKey(m_InputData) )
	{
		StateChange( ESI_SI_A_EL_DODGE_AND_SLASH_SECOND );
	}
	else if ( true == m_InputData.oneZ || true == m_InputData.oneX )
	{
		StateChange( ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_A_EL_DODGE_AND_SLASH_SECOND_StartFuture()
{
	CommonStateStartFuture();

	// ���� Ƚ�� ���̱�
	--m_uiDodgeAndSlashReuseCount;
}
/** @function : ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z_EventProcess
	@brief : ����&������ �� Z&X ����
*/
void CX2GUEL::ESI_SI_A_EL_DODGE_AND_SLASH_ATTACK_Z_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ����
void CX2GUEL::ESI_SI_A_EL_LEAP_CRASH_Init()
{
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}
void CX2GUEL::ESI_SI_A_EL_LEAP_CRASH_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SI_A_EL_LEAP_CRASH_JUMP );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )
	CommonEventProcess();
}
void CX2GUEL::ESI_SI_A_EL_LEAP_CRASH_JUMP_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( ESI_SI_A_EL_LEAP_CRASH_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )
	CommonEventProcess();
}
void CX2GUEL::ESI_SI_A_EL_LEAP_CRASH_JUMP_LANDING_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
/** @function : ESI_SI_A_EL_RUSHING_SWORD_StartFuture
	@brief : ���� �ҵ�
			 2ȸ �߰� ���ݿ� ���ؼ� ��� ������Ʈ �� �и� ��.
			 �� ������Ʈ���� ���۾ƸӸ� �ٸ��� �����ϱ� ���� �и�.

*/
void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_StartFuture()
{
	CommonStateStartFuture();
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );
}
void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"RingAlpha.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	TextureReadyInBackground(L"waldo_trock_specialAttackB_particle.dds");
}
void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// ��ũ��Ʈ���� SKILL_CANCEL_AFTER�� ������ �ð� ����, ��ų Ű�� �Է� �Ǹ� ó��
	else if( m_fSkillCancelAfter >= 0.f && m_fSkillCancelAfter < m_pXSkinAnimFuture->GetNowAnimationTime() &&
		true == m_sCurrentSkillKeyManager.IsPressedCurrentSkillKey(m_InputData) )
	{
		// MPüũ
		if ( GetNowMp() < m_fMPConsume )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
		}
		else
		{
			if( true == m_InputData.pureRight )
				m_FrameDataFuture.syncData.bIsRight = true;
			else if( true == m_InputData.pureLeft )
				m_FrameDataFuture.syncData.bIsRight = false;

			UpNowMp( -m_fMPConsume );
			StateChange( ESI_SI_A_EL_RUSHING_SWORD_2nd );
		}
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_StateStart()
{
	m_iRushingSwordNowLoopValue = 0;
	CommonStateStart();
}

void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_2nd_StartFuture()
{
	++m_iRushingSwordNowLoopValue;
	CommonStateStartFuture();
}



void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_2nd_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// ��ũ��Ʈ���� SKILL_CANCEL_AFTER�� ������ �ð� ����, ��ų Ű�� �Է� �Ǹ� ó��
	else if( m_fSkillCancelAfter >= 0.f && m_fSkillCancelAfter < m_pXSkinAnimFuture->GetNowAnimationTime() &&
		true == m_sCurrentSkillKeyManager.IsPressedCurrentSkillKey(m_InputData) )
	{
		// MPüũ
		if ( GetNowMp() < m_fMPConsume )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_2549 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
		}
		else
		{
			if( true == m_InputData.pureRight )
				m_FrameDataFuture.syncData.bIsRight = true;
			else if( true == m_InputData.pureLeft )
				m_FrameDataFuture.syncData.bIsRight = false;
			UpNowMp( -m_fMPConsume );

			if ( m_iRushingSwordAddLoopValue >= m_iRushingSwordNowLoopValue )
			{
				StateChange( ESI_SI_A_EL_RUSHING_SWORD_2nd );
			}
			else
			{
				StateChange( ESI_SI_A_EL_RUSHING_SWORD_3rd );
			}
		}
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )
}
void CX2GUEL::ESI_SI_A_EL_RUSHING_SWORD_3rd_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

		CommonEventProcess();
}
// ������
void CX2GUEL::ESI_SI_A_EL_KICK_Start()
{
	CommonStateStart();
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_KICK );
	if( NULL != pSkillTemplet )
	{
		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillLevel = userSkillTree.GetSkillLevel( pSkillTemplet->m_eID );

		m_DamageData.fForceDownValue = -pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_ABS, iSkillLevel );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO3 ) == true )
			m_DamageData.fForceDownValue *= 1.1f;
	}
}
void CX2GUEL::ESI_SI_A_EL_KICK_Init()
{
	TextureReadyInBackground( L"ColorBallGray.dds" );
	TextureReadyInBackground( L"Condense_Pulse01.dds" );
}
void CX2GUEL::ESI_SI_A_EL_KICK_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �ް� ������
void CX2GUEL::ESI_SI_A_EL_MEGA_SLASH_Init()
{
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Fire_Dummy.x");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}
void CX2GUEL::ESI_SI_A_EL_MEGA_SLASH_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �Ҵ� ���Ʈ - ����
void CX2GUEL::ESI_SI_A_EL_SONIC_ASSAULT_STAB_Init()
{
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}
void CX2GUEL::ESI_SI_A_EL_SONIC_ASSAULT_STAB_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �Ҵ� ���Ʈ - ����
void CX2GUEL::ESI_SI_A_EL_SONIC_ASSAULT_STING_Init()
{
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}
void CX2GUEL::ESI_SI_A_EL_SONIC_ASSAULT_STING_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ���ϵ� ��ũ
void CX2GUEL::ESI_SI_SA_EL_WILD_SHOCK_Init()
{
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"SA_EL_Wild_Shock_WAVE.tga");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"SI_SA_EL_WILD_SHOCK_ATTACKBOX.X");
}
void CX2GUEL::ESI_SI_SA_EL_WILD_SHOCK_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �Ŀ� ������
void CX2GUEL::ESI_SI_SA_EL_POWER_BURSTER_Init()
{
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"EL_FIRE_10.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"Particle_Blur.DDS");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"SA_EL_POWER_BURSTER_FIRE01.X");
	XSkinMeshReadyInBackground(L"Effect_ElSword_FlameGagerLand.X");
	XSkinMeshReadyInBackground(L"Elsword_SI_SA_Phoenix_Talon_Mesh02.X");
}
void CX2GUEL::ESI_SI_SA_EL_POWER_BURSTER_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �ް� ������
void CX2GUEL::ESI_SI_SA_EL_MEGA_BURSTER_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"EL_FIRE_10.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"Particle_Blur.DDS");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"Light_Sigong_Box_Open01.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"NUI_CUTTYSARK_Dying_Landing_Effect_Hole.X");
	XSkinMeshReadyInBackground(L"NUI_CUTTYSARK_Dying_Landing_Effect_Rock.X");
	XSkinMeshReadyInBackground(L"Elsword_SI_SA_Phoenix_Talon_Mesh02.X");
	XSkinMeshReadyInBackground(L"SA_EL_MEGA_BURSTER_FIRE_CIRCLE.X");
	XSkinMeshReadyInBackground(L"SA_EL_MEGA_BURSTER_FIRE_Earth.X");
}
void CX2GUEL::ESI_SI_SA_EL_MEGA_BURSTER_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �𸮹�Ƽ�� �����̵�
void CX2GUEL::ESI_SI_SA_EL_UNLIMITED_BLADE_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"COMET_CRASHER_Explosion_02.tga");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	XSkinMeshReadyInBackground(L"Effect_ElSword_FlameGagerLand.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"elsword_SA_SwordBlasting_circle.X");
}
void CX2GUEL::ESI_SI_SA_EL_UNLIMITED_BLADE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �ر� - ��(˭)
void CX2GUEL::ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallGray.dds");
	XSkinMeshReadyInBackground(L"HummingWind_AttackBox.X");
}
void CX2GUEL::ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_EventProcess()
{
	// �ر� ���¿��� �ǰݽ� Ȱ��ȭ �Ǵ� ������Ʈ
 	if( false == IsOnSomethingFuture() )
 	{
		StateChange( ESI_CAN_NOT_ATTACK_JUMP_DOWN );
 		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
 	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( GetWaitStateID() );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_A_EL_ENDURANCE_POWER_ACTIVE_StateStart()
{
	m_fGroggyTime = 0.f;
	m_PhysicParam.nowSpeed.x = 0.0f;
	m_PhysicParam.nowSpeed.y = 0.0f;

	CureExtraDamage( CX2DamageManager::EDT_ENCHANT_SHOCK );
	CureExtraDamage( CX2DamageManager::EDT_FROZEN );
	CureExtraDamage( CX2DamageManager::EDT_ENTANGLE );
	CureExtraDamage( CX2DamageManager::EDT_DRYAD );
	CureExtraDamage( CX2DamageManager::EDT_STUN );
	CureExtraDamage( CX2DamageManager::EDT_Y_PRESSED );
	CureExtraDamage( CX2DamageManager::EDT_PANIC );

	EraseBuffTempletFromGameUnit(BTI_DEBUFF_STUN);
	EraseBuffTempletFromGameUnit(BTI_DEBUFF_PANIC);
	EraseBuffTempletFromGameUnit(BTI_DEBUFF_COLD);
	EraseBuffTempletFromGameUnit(BTI_DEBUFF_CONFUSION);
	EraseBuffTempletFromGameUnit(BTI_BUFF_INDURANCE_POWER);

	CommonStateStart();
}
/** @function : DoStateEndurancePowerRage
	@brief : �ر�, ���꿡 ���� ���� �� ������Ʈ�� ��ȯ
			�ر⿡ ���� ������Ʈ ��ȯ �ô� �޸� ���� ���θ� �˻��ؾ� ��
*/
void CX2GUEL::DoStateEndurancePowerRage( bool bIsWayOfSword_ )
{
	if( true == bIsWayOfSword_ )
		StateChange( ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE, false );
	else
	{
		// �޸� ���� ��
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO9 ) == true )
		{
			StateChange( ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE_MEMO, false );
		}
		else
			StateChange( ESI_SA_A_EL_ENDURANCE_POWER_ACTIVE, false );
	}
}
void CX2GUEL::ProcessEndurancePowerSkill()
{	// �ر� - ��(˭)
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_EL_ENDURANCE_POWER );
	if( NULL != pSkillTemplet )
	{
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );

		// ��ų ���� �ð� �� �� ��� ���� �� ó���� ����,
		// ����Ʈ���� ������ ���� �ʰ� ���� ���� ��.
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_ENDURANCE", this );
		PlaySound( L"Elsword_Endurance.ogg" );
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ų ������Ʈ �Լ� - 1-1�� ���̹� ����Ʈ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CX2GUEL::ESI_DASH_COMBO_SSK_ZZ_EventProcess()			/// DASH_COMBO_ZZ
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, true == m_InputData.oneZ, ESI_DASH_COMBO_SSK_ZZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, true == m_InputData.oneX, ESI_DASH_COMBO_SSK_ZZX )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SSK_ZZZ_EventProcess()			/// DASH_COMBO_ZZZ
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SSK_ZZX_EventProcess()			/// DASH_COMBO_ZZX
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
/** @function : ESI_DASH_JUMP_COMBO_SSK_XX_EventProcess
	@brief : ���� ���¿��� �������鼭 �Է��� �ް�,
			 �������� �ٷ� ������Ʈ �����Ű�� ���� �۾�
*/
void CX2GUEL::ESI_DASH_JUMP_COMBO_SSK_XX_EventProcess()		/// DASH_JUMP_COMBO_XX
{
	bool& bESI_DASH_JUMP_COMBO_SSK_XXX = m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool& bESI_COMBO_Z = m_FrameDataFuture.stateParam.bEventFlagList[1];

	if( true == m_InputData.oneX )
		bESI_DASH_JUMP_COMBO_SSK_XXX = true;
	else if ( true == m_InputData.oneZ )
		bESI_COMBO_Z = true;

	if ( true == IsOnSomethingFuture() )
	{
		if( true == bESI_COMBO_Z )
			StateChange( ESI_COMBO_Z );
		else if( true == bESI_DASH_JUMP_COMBO_SSK_XXX )
			StateChange( ESI_DASH_JUMP_COMBO_SSK_XXX );
		else
			StateChange( ESI_DASH_JUMP_COMBO_XX_LANDING );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )
	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_JUMP_COMBO_SSK_XXX_EventProcess()		/// DASH_JUMP_COMBO_XXX
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

		CommonEventProcess();
}
// ũ�ν� ������
void CX2GUEL::ESI_SI_A_SSK_CROSS_SLASH_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.DDS");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"Whitecircle02.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	XSkinMeshReadyInBackground(L"SA_SSK_EXTINCTION_END_FX_Blade.X");
	XSkinMeshReadyInBackground(L"Lire_SI_A_Low_Kick_Mesh02.X");
	XSkinMeshReadyInBackground(L"AirSlash02.X");
}
void CX2GUEL::ESI_SI_A_SSK_CROSS_SLASH_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// �Ŀ� �극��ũ
void CX2GUEL::ESI_SI_A_SSK_POWER_BREAK_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
}
void CX2GUEL::ESI_SI_A_SSK_POWER_BREAK_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ��� ���ͳ�
void CX2GUEL::ESI_SI_A_SSK_HEAVY_STUNNER_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallBlue.dds");
}
void CX2GUEL::ESI_SI_A_SSK_HEAVY_STUNNER_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �����̷� ������Ʈ
void CX2GUEL::ESI_SI_SA_SSK_SPIRAL_BLAST_Init()
{
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	XSkinMeshReadyInBackground(L"SA_EL_SA_SSK_SPIRAL_BLAST_01.X");
	XSkinMeshReadyInBackground(L"SA_EL_SA_SSK_SPIRAL_BLAST_03.X");
	XSkinMeshReadyInBackground(L"SpiralBlast01.X");
}
void CX2GUEL::ESI_SI_SA_SSK_SPIRAL_BLAST_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ���̹� - �ͽ��ü�
//#pragma region ���̹� - �ͽ��ü�
void CX2GUEL::InitExtinction()
{
	// ���� ���� �ð� �ʱ�ȭ
	m_fExtinctionRemainTime = 0.f;

	// �߻� ���� �ʱ�ȭ
	m_fExtinctionDegreeZ = _CONST_EL_::BASE_EXTINCTION_DEGREE_Z;
}
void CX2GUEL::CreateExctionUIEffect()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ���� ���� ���¸� �˷��ֱ� ���� UI ����Ʈ
		if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hExctionBG )
		{
			// ������ ���UI ����
			wstring wstrParticleName = L"SA_SSK_EXTINCTION_CHARGE_P14_";
			wstrParticleName += ( m_FrameDataFuture.syncData.bIsRight == true ) ? L"R" : L"L";
			if( GetEqippedSkillMemo(CX2SkillTree::SMI_ELESIS_MEMO6) == true )
				wstrParticleName += L"_MEMO";
			m_hExctionBG = g_pData->GetGameMajorParticle()->CreateSequenceHandle( this,  wstrParticleName.c_str(), GetBonePos(L"Bip01") );

			CKTDGParticleSystem::CParticleEventSequence* pSeq =
				g_pData->GetGameMajorParticle()->GetInstanceSequence( m_hExctionBG );
			if( NULL != pSeq )
			{
				pSeq->CreateNewParticle(GetBonePos(L"Bip01"));
			}
		}
		// ���� ���� ���¸� �˷��ֱ� ���� UI ����Ʈ
		if( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hExtinctionArrow )
		{
			// ������ UI ����
            CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetGameMajorParticle()->CreateSequence( this,  L"SA_SSK_EXTINCTION_CHARGE_P15", GetBonePos(L"Bip01") );
            if ( pSeq != NULL )
            {
                m_hExtinctionArrow = pSeq->GetHandle();
                m_hExtinctionArrowParticle = pSeq->CreateNewParticleHandle(GetBonePos(L"Bip01"));
            }
		}
	}
}
void CX2GUEL::DestoryExctionUIEffect()
{
	g_pData->GetGameMajorParticle()->DestroyInstanceHandle( m_hExtinctionArrow );
	g_pData->GetGameMajorParticle()->DestroyInstanceHandle( m_hExctionBG );
	m_hExtinctionArrow = INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hExctionBG = INVALID_PARTICLE_SEQUENCE_HANDLE;
    m_hExtinctionArrowParticle = INVALID_PARTICLE_HANDLE;
}
void CX2GUEL::Extinction_FrameMoveFuture()
{
	CreateExctionUIEffect();
	// ���� ���� ���¸� �˷��ֱ� ���� UI ����Ʈ�� ���� ����
    CKTDGParticleSystem::CParticle* pParticle = NULL;
    pParticle = g_pX2Game->GetMajorParticle()->ValidateParticleHandle( m_hExtinctionArrowParticle );
	if( NULL != pParticle )
	{
		D3DXVECTOR3 vRot(0, (m_FrameDataFuture.syncData.bIsRight == true ) ? 0.f : 180.f ,m_fExtinctionDegreeZ );
		pParticle->SetAxisRotateDegree( vRot );
		pParticle->SetRotate( vRot );
        CKTDGParticleSystem::CParticleEventSequence* pSeq = pParticle->GetMasterSequence();
		if( NULL != pSeq )
		{
			pSeq->SetAddRotate( vRot );
			pSeq->SetAxisAngle( vRot );
		}

	}
}
// ���̹� - �ͽ��ü�
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_START_Init()
{
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"Lire_Kick_Impact01.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"EXTINCTION_ATTACK_DUMMY.X");
	XSkinMeshReadyInBackground(L"SA_SSK_EXTINCTION_END_FX_Blade.X");
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_START_Start()
{
	InitExtinction();
	CommonStateStart();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_START_StartFuture()
{
	// �ͽ��ü� ���� �ϷḦ üũ�ϱ� ���� ��ų Ű ����
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );

	DestoryExctionUIEffect();

	CommonStateStartFuture();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_START_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_CHARGE );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )
	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_CHARGE_FrameMove()
{
	m_fExtinctionRemainTime += m_fElapsedTime;

	CommonFrameMove();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_CHARGE_FrameMoveFuture()
{
	// ����Ʈ ���� ����
	Extinction_FrameMoveFuture();

	CommonFrameMoveFuture();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_CHARGE_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	// �ִ� ���� �ð� �ʰ� �� ���� ������� ����
	if( m_fExtinctionRemainTime >= _CONST_EL_::MAX_EXTINCTION_CHARGE_TIME )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_ATTACK );
	}
	else if( true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_ATTACK );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, true == m_InputData.pureUp, ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 0, true == m_InputData.pureDown, ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
/// ���̹� - �ͽ��ü� - Attack
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_ATTACK_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// �ͽ��ü� ���� ������ ����
		CX2EffectSet::Handle hEffect =
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_SSK_EXTINCTION_DAMAGE", this ,
			NULL, IsHyperState(), GetPowerRate() );



		if( INVALID_EFFECTSET_HANDLE != hEffect )
		{
			if ( CX2EffectSet::EffectSetInstance* pEffectSet = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
			{	// Ű �Է��� ���� ������ ������ ����Ʈ ����
				pEffectSet->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, m_fExtinctionDegreeZ ) ) ;
			}
		}
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_ATTACK_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if( false == IsOnSomethingFuture() )
	{
		// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
		if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
			( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
		{
			m_PhysicParam.nowSpeed.x = GetWalkSpeed();
		}
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_ATTACK_StartFuture()
{
	DestoryExctionUIEffect();
	CommonStateStartFuture();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP_FrameMove()	// ���� ����
{
	m_fExtinctionRemainTime += m_fElapsedTime;

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO6) == true )
	{
		if( m_fExtinctionDegreeZ < _CONST_EL_::MAX_EXTINCTION_DEGREE_Z + 30.f )
			m_fExtinctionDegreeZ += m_fElapsedTime * _CONST_EL_::EXTINCTION_DEGREE_Z_CONTROL;
		else
			m_fExtinctionDegreeZ = _CONST_EL_::MAX_EXTINCTION_DEGREE_Z + 30.f;
	}
	else
	{
		if( m_fExtinctionDegreeZ < _CONST_EL_::MAX_EXTINCTION_DEGREE_Z )
			m_fExtinctionDegreeZ += m_fElapsedTime * _CONST_EL_::EXTINCTION_DEGREE_Z_CONTROL;
		else
			m_fExtinctionDegreeZ = _CONST_EL_::MAX_EXTINCTION_DEGREE_Z;
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	// �ִ� ���� �ð� �ʰ� �� ���� ������� ����
	if( m_fExtinctionRemainTime >= _CONST_EL_::MAX_EXTINCTION_CHARGE_TIME )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_ATTACK );
	}
	else if( true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_ATTACK );
	}
	// ����Ű�� ���� �߻� ���� ����
	else if( true == m_InputData.pureDown )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP_FrameMoveFuture()
{
	// ����Ʈ ���� ����
	Extinction_FrameMoveFuture();

	CommonFrameMoveFuture();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN_FrameMove()	// ���� ����
{
	m_fExtinctionRemainTime += m_fElapsedTime;

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO6) == true )
	{
		if( m_fExtinctionDegreeZ > _CONST_EL_::MIN_EXTINCTION_DEGREE_Z - 30.f)
			m_fExtinctionDegreeZ -= m_fElapsedTime * _CONST_EL_::EXTINCTION_DEGREE_Z_CONTROL;
		else
			m_fExtinctionDegreeZ = _CONST_EL_::MIN_EXTINCTION_DEGREE_Z- 30.f;
	}
	else
	{
		if( m_fExtinctionDegreeZ > _CONST_EL_::MIN_EXTINCTION_DEGREE_Z )
			m_fExtinctionDegreeZ -= m_fElapsedTime * _CONST_EL_::EXTINCTION_DEGREE_Z_CONTROL;
		else
			m_fExtinctionDegreeZ = _CONST_EL_::MIN_EXTINCTION_DEGREE_Z;
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN_FrameMoveFuture()
{
	// ����Ʈ ���� ����
	Extinction_FrameMoveFuture();

	CommonFrameMoveFuture();
}
void CX2GUEL::ESI_SI_SA_SSK_EXTINCTION_DEGREE_DOWN_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	// �ִ� ���� �ð� �ʰ� �� ���� ������� ����
	if( m_fExtinctionRemainTime >= _CONST_EL_::MAX_EXTINCTION_CHARGE_TIME )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_ATTACK );
	}
	else if( true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_ATTACK );
	}
	// ����Ű�� ���� �߻� ���� ����
	else if( true == m_InputData.pureUp )
	{
		StateChange( ESI_SI_SA_SSK_EXTINCTION_DEGREE_UP );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
//#pragma endregion ���̹� - �ͽ��ü�
// �¸��� ��
void CX2GUEL::ESI_SI_SA_SSK_VICTORIOUS_SWORD_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"HummingWind_AttackBox.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"SA_SPK_INFERNAL_BLADE_Weapon_Dummy.x");
	TextureReadyInBackground(L"Whitecircle02.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
}
void CX2GUEL::ESI_SI_SA_SSK_VICTORIOUS_SWORD_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SSK_VICTORIOUS_SWORD_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CX2EffectSet::Handle hHandle = INVALID_EFFECTSET_HANDLE;
		if( GetUnitClass() == CX2Unit::UC_ELESIS_SABER_KNIGHT )
			hHandle = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_SSK_VICTORIOUS_SWORD_FX_NEW", this );
		else
			hHandle = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SA_SGM_VICTORIOUS_SWORD_NEW", this );

		m_vecEffectSetToDeleteOnDamageReact.push_back(hHandle);
	}

	if( m_pXSkinAnim->EventTimerOneshot( 0.533f ) )
	{
		/// ��ų ���� ��ȯ
		const int iSkillLevel = max( GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_SSK_VICTORIOUS_SWORD ), 1 );

		if( GetUnitClass() == CX2Unit::UC_ELESIS_SABER_KNIGHT )
			SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_VICTORIOUS_SWORD, iSkillLevel );
		else
			SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_SGM_VICTORIOUS_SWORD, iSkillLevel );
	}

	CommonFrameMove();
}
// ������Ʈ ����̺�
void CX2GUEL::ESI_SI_SA_SSK_JUDGEMENT_DRIVE_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	XSkinMeshReadyInBackground(L"Elsword_Double_Slash_Mesh01.X");
	XSkinMeshReadyInBackground(L"Elsword_Double_Slash_Mesh02.X");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
}
void CX2GUEL::ESI_SI_SA_SSK_JUDGEMENT_DRIVE_StateStartFuture()
{
	CommonStateStartFuture();

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO7 ) == true )
	{
		float fAniSpeed = m_pXSkinAnimFuture->GetPlaySpeed();
		m_pXSkinAnimFuture->SetPlaySpeed( fAniSpeed * 1.3f );
	}
}
void CX2GUEL::ESI_SI_SA_SSK_JUDGEMENT_DRIVE_StateStart()
{
	CommonStateStart();

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELESIS_MEMO7 ) == true )
	{
		float fAniSpeed = m_pXSkinAnim->GetPlaySpeed();
		m_pXSkinAnim->SetPlaySpeed( fAniSpeed * 1.3f );
	}

}
void CX2GUEL::ESI_SI_SA_SSK_JUDGEMENT_DRIVE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ų ������Ʈ �Լ� - 2-1�� ���̷� ����Ʈ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CX2GUEL::ESI_DASH_COMBO_SPK_ZX_EventProcess()			// ESI_DASH_COMBO_SPK_ZX
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SPK_ZX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.233f ) )
	{
		if(	true == FlushMp( _CONST_EL_::FIRE_BALL_MP_COST ) )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_EL_Dash_Combo_ZX", this, NULL, IsHyperState(), GetPowerRate()  * GetFireBallPowerRate() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_EL_Dash_Combo_ZX_FAIL", this );
	}
	CommonFrameMove();
}
void CX2GUEL::ESI_DASH_COMBO_SPK_ZZ_EventProcess()			// DASH_COMBO_ZZ
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, true == m_InputData.oneZ, ESI_DASH_COMBO_SPK_ZZZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SPK_ZZZ_EventProcess()			// DASH_COMBO_ZZZ
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SPK_ZZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.266f ) )
	{
		if(	true == FlushMp( _CONST_EL_::FIRE_BALL_MP_COST ) )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_EL_Dash_Combo_ZZZ", this, NULL, IsHyperState(), GetPowerRate()  * GetFireBallPowerRate() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_EL_Dash_Combo_ZZZ_FAIL", this );
	}
	CommonFrameMove();
}
void CX2GUEL::ESI_DASH_JUMP_COMBO_SPK_XZ_EventProcess()		// DASH_JUMP_COMBO_XZ
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_JUMP_DOWN );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT(0,0, true == m_InputData.oneZ && 0 < m_uiSPKXZReattakCount, ESI_DASH_JUMP_COMBO_SPK_XZ_REATTACK)
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

		CommonEventProcess();
}
void CX2GUEL::ESI_DASH_JUMP_COMBO_SPK_XZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		if(	true == FlushMp( _CONST_EL_::SPK_DASH_JUMP_XZ_MP_COST ) )
		{
			if( true == m_bIsLearnUnextinguishableFire )
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_SBH_DashJump_Combo_XZ", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
			else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_EL_DashJump_Combo_XZ", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
		}
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_EL_DashJump_Combo_XZ_FAIL", this );
	}

	CommonFrameMove();
}
/** @function : ESI_DASH_JUMP_COMBO_SPK_ZX_StartFurture
	@brief : ESI_DASH_JUMP_COMBO_SPK_ZX ���� �߰� X����(���̾) ������ ���� �Լ�
*/
void CX2GUEL::ESI_DASH_JUMP_COMBO_SPK_XZ_StartFurture()
{
	m_bIsDashAttackPossible = false;
	m_uiSPKXZReattakCount = _CONST_EL_::MAX_DASH_JUMP_COMBO_SPK_XZ_REATTACK;
	CommonStateStartFuture();
}
/** @function : ESI_DASH_JUMP_COMBO_SPK_ZX_REATTACK_StartFurture
	@brief : ESI_DASH_JUMP_COMBO_SPK_ZX ���� �߰� X����(���̾) ������ ���� �Լ�
*/
void CX2GUEL::ESI_DASH_JUMP_COMBO_SPK_XZ_REATTACK_StartFurture()
{
	--m_uiSPKXZReattakCount;
	CommonStateStartFuture();
}
// �ҵ� �̷���
void CX2GUEL::ESI_SI_A_SPK_SWORD_ERUPTION_Init()
{
	XSkinMeshReadyInBackground(L"A_SPK_SWORD_ERUPTION_BLADE.x");
	XSkinMeshReadyInBackground(L"A_SPK_SWORD_ERUPTION_Square.x");
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"COMET_CRASHER_Explosion_02.tga");
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"Particle_Blur.DDS");
}
void CX2GUEL::ESI_SI_A_SPK_SWORD_ERUPTION_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ������
void CX2GUEL::ESI_SI_A_SPK_BIG_BURST_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"EL_FIRE_09.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}
void CX2GUEL::ESI_SI_A_SPK_BIG_BURST_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ����Ʈ ����¡
void CX2GUEL::ESI_SI_SA_SPK_BURST_RISING_Init()
{
	XSkinMeshReadyInBackground(L"A_SPK_SWORD_ERUPTION_BLADE.x");
	XSkinMeshReadyInBackground(L"A_SPK_SWORD_ERUPTION_Square.x");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"EL_FIRE_09.dds");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"SA_EL_MEGA_BURSTER_FIRE_CIRCLE.X");
	XSkinMeshReadyInBackground(L"SA_EL_MEGA_BURSTER_FIRE_Earth.X");
}
void CX2GUEL::ESI_SI_SA_SPK_BURST_RISING_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ����Ʈ ���̺�
void CX2GUEL::ESI_SI_SA_SPK_BURST_WAVE_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	TextureReadyInBackground(L"Particle_Blur.DDS");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"elsword_SA_rageCutter_smogPoint.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"DummyAttackBox_200x200x200.X");
}
void CX2GUEL::ESI_SI_SA_SPK_BURST_WAVE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// �ҵ� ���̾�
void CX2GUEL::ESI_SI_SA_SPK_SWORDFIRE_Init()
{

}
void CX2GUEL::ESI_SI_SA_SPK_SWORDFIRE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_LANDING );
		}
		else
		{
			// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
			if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
				( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
			{
				m_PhysicParam.nowSpeed.x = GetWalkSpeed();
			}
		}
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
// ���丣�� �����̵�
void CX2GUEL::ESI_SI_SA_SPK_INFERNAL_BLADE_Init()
{
	TextureReadyInBackground(L"EL_FIRE_01.dds");
	TextureReadyInBackground(L"EL_FIRE_02.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	XSkinMeshReadyInBackground(L"SA_SPK_INFERNAL_BLADE_Weapon_Dummy.x");
	TextureReadyInBackground(L"Whitecircle03.dds");
}
void CX2GUEL::ESI_SI_SA_SPK_INFERNAL_BLADE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )
	CommonEventProcess();
}
// ���ͳ� ���̾�
void CX2GUEL::ESI_SI_SA_SPK_ETERNAL_FIRE_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_10.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Particle_Blur.DDS");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE02.dds");
	TextureReadyInBackground(L"state_potion_flare.DDS");
	TextureReadyInBackground(L"stone_AirShip.dds");
	XSkinMeshReadyInBackground(L"Elsword_SI_SA_Phoenix_Talon_Mesh02.X");
	XSkinMeshReadyInBackground(L"SA_SPK_ETERNAL_FIRE_Fire_Circle01.x");
	XSkinMeshReadyInBackground(L"SA_SPK_ETERNAL_FIRE_Fire_Circle02.x");
	XSkinMeshReadyInBackground(L"SA_SPK_ETERNAL_FIRE_Fire_Circle03.x");
}
void CX2GUEL::ESI_SI_SA_SPK_ETERNAL_FIRE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// ��� ���� �޺� XZ
void CX2GUEL::ESI_DASH_JUMP_COMBO_SGM_XZ_Init()
{
	XSkinMeshReadyInBackground(L"SGM_DASH_JUMP_COMBO_XZ_M01.X");
	XSkinMeshReadyInBackground(L"Ara_Dash_Combo_X_M01.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"GroundShockWave02.dds");
}

void CX2GUEL::ESI_DASH_JUMP_COMBO_SGM_XZ_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_DASH_JUMP_COMBO_SGM_XZ_LOOP );
	}
	CommonEventProcess();
}

// ��� ���� �޺� XZ
void CX2GUEL::ESI_DASH_JUMP_COMBO_SGM_XZ_LOOP_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( ESI_DASH_JUMP_COMBO_SGM_XZ_DOWN );
	}
	CommonEventProcess();


}

// ��� ���� �޺� XZ �ٿ�
void CX2GUEL::ESI_DASH_JUMP_COMBO_SGM_XZ_DOWN_EventProcess()
{

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// �޺� XX �ٿ� X ����

void CX2GUEL::ESI_COMBO_SGM_XX_DOWN_X_START_Init()
{
	XSkinMeshReadyInBackground(L"Elsword_SI_A_ES_WINDMILL_Mesh01.X");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"Elsword_SI_A_ES_WINDMILL_Mesh02.X");
	XSkinMeshReadyInBackground(L"SA_SSK_EXTINCTION_END_FX_Blade.X");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
}

void CX2GUEL::ESI_COMBO_SGM_XX_DOWN_X_START_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) == true )
	{

		if ( NULL != g_pX2Game->GetEffectSet() )
		{
			m_hXXDownXLoop = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SGM_XXX_Start",
				this, this, IsHyperState(), GetPowerRate() );
		}

		m_vecEffectSetToDeleteOnDie.push_back( m_hXXDownXLoop );
		m_vecEffectSetToDeleteOnCustomState.push_back( m_hXXDownXLoop );
	}
	CommonFrameMove();
}



void CX2GUEL::ESI_COMBO_SGM_XX_DOWN_X_START_StateStart()
{
	CommonStateStart();
	m_iComboXXdownXNowLoopCount = 0; // ���� Ƚ�� �ʱ�ȭ ( �⺻ �ִϸ��̼ǿ��� 4ȸ ȸ�� )
	m_iComboXXdownXMaxLoopCount = 0;

}

void CX2GUEL::ESI_COMBO_SGM_XX_DOWN_X_START_EventProcess()
{
	if ( m_InputData.oneX == true )
	{
		++m_iComboXXdownXMaxLoopCount;
		if ( m_iComboXXdownXMaxLoopCount >= _CONST_EL_::MAX_XX_DOWN_X_LOOP_COUNT )
			m_iComboXXdownXMaxLoopCount = _CONST_EL_::MAX_XX_DOWN_X_LOOP_COUNT;
	}

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( m_iComboXXdownXMaxLoopCount > 0 )
			StateChange( ESI_COMBO_SGM_XX_DOWN_X_LOOP );
		else
			StateChange( ESI_COMBO_SGM_XX_DOWN_X_END );
	}

	CommonEventProcess();
}

// �޺� XX �ٿ� X ����
void CX2GUEL::ESI_COMBO_SGM_XX_DOWN_X_LOOP_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		++m_iComboXXdownXNowLoopCount;

		if ( m_iComboXXdownXNowLoopCount < m_iComboXXdownXMaxLoopCount )
			StateChange( ESI_COMBO_SGM_XX_DOWN_X_LOOP );
		else
			StateChange( ESI_COMBO_SGM_XX_DOWN_X_END );
	}
	CommonEventProcess();
}


// �޺� XX �ٿ� X ��
void CX2GUEL::ESI_COMBO_SGM_XX_DOWN_X_END_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// ����� ���, �� ������ ����
void CX2GUEL::ESI_QUICK_STANDUP_ATTACK_FRONT_ADVANCED_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// ����� ���, �� ������ ����
void CX2GUEL::ESI_QUICK_STANDUP_ATTACK_BACK_ADVANCED_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


// ���� ����
void CX2GUEL::ESI_A_SGM_PROVOKE_Init()
{
	TextureReadyInBackground(L"A_SGM_DRASTIC_CHARGE_Power_P03.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	TextureReadyInBackground(L"Iron_Will_P01.DDS");
}

void CX2GUEL::ESI_A_SGM_PROVOKE_StateStart()
{
	SetIsProvokeStateChange( true );
	CommonStateStart();
}

void CX2GUEL::ESI_A_SGM_PROVOKE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


void CX2GUEL::ESI_A_SGM_PROVOKE_StateEnd()
{
	SetIsProvokeStateChange( false );
	CommonStateEnd();
}


// ���� ���� �� �ݰ�
void CX2GUEL::ESI_A_SGM_PROVOKE_REVENGE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// �巡��ƽ ���� ����
void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_START_Init()
{
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_04.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"A_SGM_DRASTIC_CHARGE_Power.dds");
	TextureReadyInBackground(L"A_SGM_DRASTIC_CHARGE_Power_P03.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SA_EL_SA_SSK_SPIRAL_BLAST_01.X");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");

}

void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_START_StateStart ()
{
	CommonStateStart();
	m_fDrasticChargeNowLoopTime = 0;
}

void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_START_FrameMove()
{
if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) == true )
	{
		if ( NULL != g_pX2Game->GetEffectSet() )
		{
			m_hDrasticChargeGroundFire = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_SGM_DRASTIC_CHARGE_GroundFire",
				this, this, IsHyperState(), GetPowerRate() );
			m_hDrasticChargeLoop = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_SGM_DRASTIC_CHARGE_LOOP",
				this, this, IsHyperState(), GetPowerRate() );
		}

		m_vecEffectSetToDeleteOnDie.push_back( m_hDrasticChargeLoop );
		m_vecEffectSetToDeleteOnCustomState.push_back( m_hDrasticChargeLoop );

		m_vecEffectSetToDeleteOnDie.push_back( m_hDrasticChargeGroundFire );
		m_vecEffectSetToDeleteOnCustomState.push_back( m_hDrasticChargeGroundFire );
	}
	CommonFrameMove();
}



void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_START_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_A_SGM_DRASTIC_CHARGE_LOOP );
	}

	CommonEventProcess();
}

void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_START_StartFurture()
{
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );
	CommonStateStartFuture();
}


// �巡��ƽ ���� ����
void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_LOOP_EventProcess()
{
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();		// ��ų ������ ����Ǿ��°�?

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )									// Animation End �� ��
	{
		StateChange( ESI_A_SGM_DRASTIC_CHARGE_LOOP );									// Loop �� ����
	}

	else if ( m_fDrasticChargeNowLoopTime >= _CONST_EL_::MAX_DRASTIC_CHARGE_LOOP_TIME )	/// �ִ� �ð��� ������ ���
	{
		StateChange( ESI_A_SGM_DRASTIC_CHARGE_END );										// End �� ����
	}

	else if ( m_fDrasticChargeNowLoopTime >= _CONST_EL_::MIN_DRASTIC_CHARGE_LOOP_TIME )	// �ּ� �ð��� �������ٸ�
	{
		if ( true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) || 	// Ű �Է��� �Էµ��� �ʾҰų�
			 m_fDrasticChargeLoopDecreaseMPValue > GetNowMp() )								// ������ ���� ���
		{
			StateChange( ESI_A_SGM_DRASTIC_CHARGE_END );									// End �� ����
		}
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_LOOP_FrameMove()
{
	m_fDrasticChargeNowLoopTime = m_fDrasticChargeNowLoopTime + m_fElapsedTime;

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) == true )
	{
		FlushMp( m_fDrasticChargeLoopDecreaseMPValue );
	}

	CommonFrameMove();
}

// �巡��ƽ ���� ��
void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_END_StateStart()
{
	CommonStateStart();
}

void CX2GUEL::ESI_A_SGM_DRASTIC_CHARGE_END_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}



// ���� ũ����, ���߿��� ����ߴٸ� Loop, �ƴ϶�� End �� �Ѿ
void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_START_Init()
{
	XSkinMeshReadyInBackground(L"Ara_Dash_Combo_X_M01.X");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_100x50x50_front.X");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	XSkinMeshReadyInBackground(L"SGM_DASH_JUMP_COMBO_XZ_M01.X");
	XSkinMeshReadyInBackground(L"JUGGERNAUT_BUSTER_ATTACK_BOX.X");
	TextureReadyInBackground(L"A_SGM_DRASTIC_CHARGE_Power_P01.dds");
	XSkinMeshReadyInBackground(L"BREAKING_FIST_Attack_Box.X");
}

void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_START_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )			// ������ ���ִٸ� Loop
			StateChange( ESI_A_SGM_SNAP_CRASHER_LOOP );
		else											// �پ��ִٸ� End
			StateChange( ESI_A_SGM_SNAP_CRASHER_END );
	}

	CommonEventProcess();
}

void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_START_StateEnd()
{
	CommonStateEnd();

	if ( NULL != g_pX2Game->GetEffectSet() )
	{
		m_hSnapCreaherLoopAttack = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_SGM_SNAP_CRESHER_LOOP_ATTACK",
			this, this, IsHyperState(), GetPowerRate() );
	}
	m_vecEffectSetToDeleteOnCustomState.push_back ( m_hSnapCreaherLoopAttack );
	m_vecEffectSetToDeleteOnDie.push_back ( m_hSnapCreaherLoopAttack );
}


// ���� ũ����, ���߿��� ��� �� ������
void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_LOOP_StateStart()
{
	CommonStateStart();
	if ( NULL != g_pX2Game->GetEffectSet() )
	{
		m_hSnapCresherLoop = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_SGM_SNAP_CRESHER_LOOP",
			this, this, IsHyperState(), GetPowerRate() );
	}
	m_vecEffectSetToDeleteOnCustomState.push_back ( m_hSnapCresherLoop );
	m_vecEffectSetToDeleteOnDie.push_back ( m_hSnapCresherLoop );
}

void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_LOOP_EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( ESI_A_SGM_SNAP_CRASHER_END );
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_A_SGM_SNAP_CRASHER_LOOP );
	}
	CommonEventProcess();
}


// ���� ũ����, ���߿��� ��� �� �����ͼ� �� ���
void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_END_StateStart()
{
	if ( NULL != g_pX2Game->GetEffectSet() )
	{
		if ( INVALID_EFFECTSET_HANDLE != m_hSnapCresherLoop )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hSnapCresherLoop );
			m_hSnapCresherLoop = INVALID_EFFECTSET_HANDLE;
		}

		if ( INVALID_EFFECTSET_HANDLE != m_hSnapCreaherLoopAttack )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hSnapCreaherLoopAttack );
			m_hSnapCreaherLoopAttack = INVALID_EFFECTSET_HANDLE;
		}

	}
	CommonStateStart();
}

void CX2GUEL::ESI_A_SGM_SNAP_CRASHER_END_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// �Ҵ� �����̵�

void CX2GUEL::ESI_SA_SGM_SONIC_BLADE_Init()
{
	TextureReadyInBackground(L"A_SGM_DRASTIC_CHARGE_Power_P01.dds");
	TextureReadyInBackground(L"A_SGM_DRASTIC_CHARGE_Power_P02.dds");
	TextureReadyInBackground(L"A_SGM_SONIC_BLADE_Map02.dds");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"elsword_active_crecentCut_crecent.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"elsword_active_crecentCut_crecent.dds");
}

void CX2GUEL::ESI_SA_SGM_SONIC_BLADE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_LANDING );
		}

		else
		{
			// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
			if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
				( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
			{
				m_PhysicParam.nowSpeed.x = GetWalkSpeed();
			}
		}
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// ũ��Ƽ�� ���̺�

void CX2GUEL::ESI_SA_SGM_CRITICAL_DIVE_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallGray.dds");
	TextureReadyInBackground(L"RingAlpha.dds");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	XSkinMeshReadyInBackground(L"A_SGM_CRITICAL_DIVE_Attack_Line.X");
	TextureReadyInBackground(L"waldo_trock_specialAttackB_particle.dds");
}

void CX2GUEL::ESI_SA_SGM_CRITICAL_DIVE_EventProcess()
{

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.425f && GetAttackSuccess(true) == true )
	{
		StateChange( ESI_SA_SGM_CRITICAL_DIVE_HIT );
	}

	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		const CX2UserSkillTree::SkillSlotData* pSkillSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( CX2SkillTree::SI_SA_SGM_CRITICAL_DIVE );
		if( NULL != pSkillSlotData )
		{
			float fMPConsume = pSkillSlotData->m_fMPConsumption / 2.f;
			UpNowMp(fMPConsume);
			if( NULL != g_pData->GetPicCharBlue() )
			{
				WCHAR wszText[64] = L"";
				StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MP+ %d", (int)fMPConsume );

				D3DXVECTOR3 pos = GetPos();
				pos.y += 50.f;

				g_pData->GetPicCharBlue()->DrawText( wszText, pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}
		}

		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// ũ��Ƽ�� ���̺� ��� �� ���� �¾��� ��
void CX2GUEL::ESI_SA_SGM_CRITICAL_DIVE_HIT_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// ���ų� ������, ����

void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_START_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"JUGGERNAUT_BUSTER_END_TRACE.X");
	TextureReadyInBackground(L"RingAlpha.dds");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	XSkinMeshReadyInBackground(L"JUGGERNAUT_BUSTER_ATTACK_BOX.X");
	XSkinMeshReadyInBackground(L"JUGGERNAUT_BUSTER_LOOP_TRACE2.X");
	XSkinMeshReadyInBackground(L"JUGGERNAUT_BUSTER_READY_TRACE2.X");
}
void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_START_StateStart()
{
	m_fJuggernautBusterNowLoopTime = 0;
	m_bJuggernautBusterIsStartDirectionRight = GetIsRight();
	CommonStateStart();
}
void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_START_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP );
	}
	else
	{
		if( true == IsOnSomethingFuture() )
		{
			m_PhysicParam.nowSpeed.y = 0.f;
		}
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 0.5f;
		if( m_InputData.pureRight == true )
		{
			if ( GetIsRight() == false )
				m_PhysicParam.nowSpeed.x = m_PhysicParam.nowSpeed.x * -1;
		}
		else if( m_InputData.pureLeft == true )
		{
			if ( GetIsRight() == true )
				m_PhysicParam.nowSpeed.x = m_PhysicParam.nowSpeed.x * -1;
		}
		else
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
		}
	}

	CommonEventProcess();
}


// ���ų� ������, ����
void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_EventProcess()
{
	if ( _CONST_EL_::MAX_JUGGERNAUT_BUSTER_LOOP_TIME < m_fJuggernautBusterNowLoopTime)
	{
		StateChange( ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST );
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP );
	}
	else
	{
		if( true == IsOnSomethingFuture() )
		{
			m_PhysicParam.nowSpeed.y = 0.f;
		}
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 0.7f;
		if( m_InputData.pureRight == true )
		{
			if ( GetIsRight() == false )
				m_PhysicParam.nowSpeed.x = m_PhysicParam.nowSpeed.x * -1;
		}
		else if( m_InputData.pureLeft == true )
		{
			if ( GetIsRight() == true )
				m_PhysicParam.nowSpeed.x = m_PhysicParam.nowSpeed.x * -1;
		}
		else
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
		}
	}

	CommonEventProcess();
}

void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_FrameMove()
{
	if ( false == m_FrameDataNow.syncData.bFrameStop )
		m_fJuggernautBusterNowLoopTime = m_fJuggernautBusterNowLoopTime + m_fElapsedTime;

	CommonFrameMove();
}


// ���ų� ������, ���� ��
void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SA_SGM_JUGGERNAUT_BUSTER_END );
	}

	CommonEventProcess();
}

void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_LOOP_LAST_StartEnd()
{
	m_FrameDataNow.syncData.bIsRight = m_bJuggernautBusterIsStartDirectionRight;
	CommonStateEnd();
}


// ���ų� ������, ��
void CX2GUEL::ESI_SA_SGM_JUGGERNAUT_BUSTER_END_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// ������ ����

void CX2GUEL::ESI_SA_SGM_WAR_PRELUDE_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallGray.dds");
	TextureReadyInBackground(L"Condense_Pulse01.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Pulsewave01.dds");
	XSkinMeshReadyInBackground(L"SA_SGM_WAR_PRELUDE_AURA.X");
	TextureReadyInBackground(L"SA_SGM_WAR_PRELUDE_Map01.dds");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
}

void CX2GUEL::ESI_SA_SGM_WAR_PRELUDE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


// �ҵ� ���� ������

void CX2GUEL::ESI_HA_SGM_SWORD_OF_RELICS_Init()
{
	TextureReadyInBackground(L"ARA_SA_FallingDragon_down.DDS");
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"BlackImage.dds");
	TextureReadyInBackground(L"ColorBallGray.dds");
	TextureReadyInBackground(L"Condense_Pulse01.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"GroundShockWave.dds");
	TextureReadyInBackground(L"HELLPUTT_COMET_CRASHER_Explosion_02.tga");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"PROTECT_GUARD_PHYSIC_Attack_B_Slash.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Pulsewave01.dds");
	XSkinMeshReadyInBackground(L"SA_SGM_SWORD_OF_RELICS_Hand_M01.X");
	TextureReadyInBackground(L"SA_SGM_SWORD_OF_RELICS_MAGIC01.tga");
	TextureReadyInBackground(L"SA_SGM_SWORD_OF_RELICS_MAGIC02.tga");
	XSkinMeshReadyInBackground(L"SA_SGM_SWORD_OF_RELICS_SWORD_EX3.X");
	XSkinMeshReadyInBackground(L"SA_SGM_SWORD_OF_RELICS_SWORD_EX4.X");
	TextureReadyInBackground(L"SA_SGM_WAR_PRELUDE_Map01.dds");
	TextureReadyInBackground(L"SGM_D_J_C_XZ_Landing2.dds");
	TextureReadyInBackground(L"WhiteImage.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"stone_AirShip.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_100x50x50_tall2.x");
}

void CX2GUEL::ESI_HA_SGM_SWORD_OF_RELICS_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��ų ������Ʈ �Լ� - 2-2�� ������¡ ��Ʈ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �޺� XZ
void CX2GUEL::ESI_COMBO_SBH_XZ_Init()
{
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
}
void CX2GUEL::ESI_COMBO_SBH_XZ_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true && CX2Unit::UC_ELESIS_BLAZING_HEART == GetUnitClass() ,
		ESI_COMBO_SBH_XZZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ESI_COMBO_SBH_XZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) == true )
	{
		m_DamageData.damage.fMagic *= GetFireBallPowerRate();
	}

	CommonFrameMove();
}
// �޺� XZZ
void CX2GUEL::ESI_COMBO_SBH_XZZ_Init()
{
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SI_SA_SBH_WALL_OF_PROMINENCE_02.X");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
}
void CX2GUEL::ESI_COMBO_SBH_XZZ_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true && CX2Unit::UC_ELESIS_BLAZING_HEART == GetUnitClass() ,
		ESI_COMBO_SBH_XZZZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ESI_COMBO_SBH_XZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) == true )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Combo_XZZ", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
	}

	CommonFrameMove();
}
// �޺� XZZZ
void CX2GUEL::ESI_COMBO_SBH_XZZZ_Init()
{
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
}
void CX2GUEL::ESI_COMBO_SBH_XZZZ_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ESI_COMBO_SBH_XZZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) == true )
	{
		m_DamageData.damage.fMagic *= GetFireBallPowerRate();
	}

	CommonFrameMove();
}
// ��� �޺� X
void CX2GUEL::ESI_DASH_COMBO_SBH_X_Init()
{
	XSkinMeshReadyInBackground(L"C_SPK_Fire_Ball_FX.X");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall01.X");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall02.X");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall03.X");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Arme_smoke.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.x");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	XMeshReadyInBackground(L"Blur_Light_Mesh01.y");
}
void CX2GUEL::ESI_DASH_COMBO_SBH_X_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
			( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
		{
			StateChangeDashIfPossible();
		}
		else if ( true == IsOnSomethingFuture() )
		{
			StateChange( USI_DASH_JUMP_LANDING );
		}
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneX == true, ESI_DASH_COMBO_SBH_XX )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneZ == true, ESI_DASH_COMBO_SBH_XXXZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_X_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.135f ) == true  )
	{
		if(	true == FlushMp( _CONST_EL_::SPK_DASH_JUMP_XZ_MP_COST ) )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Dash_Combo_X", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Combo_XZZ_FAIL", this );
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_X_FrameMoveFuture()
{
	CommonFrameMoveFuture();

	m_PhysicParam.passiveSpeed.x = GetDashSpeed();
}

// ��� �޺� XX
void CX2GUEL::ESI_DASH_COMBO_SBH_XX_Init()
{
	XSkinMeshReadyInBackground(L"C_SPK_Fire_Ball_FX.X");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall01.X");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall02.X");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall03.X");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Arme_smoke.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.x");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	XMeshReadyInBackground(L"Blur_Light_Mesh01.y");
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XX_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
			( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
		{
			StateChangeDashIfPossible();
		}
		else if ( true == IsOnSomethingFuture() )
		{
			StateChange( USI_DASH_JUMP_LANDING );
		}
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneX == true, ESI_DASH_COMBO_SBH_XXX )
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 1, 1, m_InputData.oneZ == true, ESI_DASH_COMBO_SBH_XXXZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.363f ) == true )
	{
		if(	true == FlushMp( _CONST_EL_::SPK_DASH_JUMP_XZ_MP_COST ) )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Dash_Combo_XX", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Combo_XZZ_FAIL", this );
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XX_FrameMoveFuture()
{
	CommonFrameMoveFuture();

	m_PhysicParam.passiveSpeed.x = GetDashSpeed();
}

// ��� �޺� XXX
void CX2GUEL::ESI_DASH_COMBO_SBH_XXX_Init()
{
	XSkinMeshReadyInBackground(L"C_SPK_Fire_Ball_FX.X");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall01.X");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall02.X");
	XSkinMeshReadyInBackground(L"EffectSet_SBH_FireBall03.X");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Arme_smoke.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.x");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	XMeshReadyInBackground(L"Blur_Light_Mesh01.y");
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XXX_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
			( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
		{
			StateChangeDashIfPossible();
		}
		else if ( true == IsOnSomethingFuture() )
		{
			StateChange( USI_DASH_JUMP_LANDING );
		}
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_STRUCT( 0, 0, m_InputData.oneZ == true, ESI_DASH_COMBO_SBH_XXXZ )
	/*SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )*/

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XXX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.492f ) == true )
	{
		if(	true == FlushMp( _CONST_EL_::SPK_DASH_JUMP_XZ_MP_COST ) )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Dash_Combo_XXX", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Combo_XZZ_FAIL", this );
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XXX_FrameMoveFuture()
{
	CommonFrameMoveFuture();

	m_PhysicParam.passiveSpeed.x = GetDashSpeed();
	m_fDashComboStartDelay = _CONST_EL_::SBM_DASH_COMBO_DELAY_TIME;
}

// ��� �޺� X �� Z
void CX2GUEL::ESI_DASH_COMBO_SBH_XXXZ_Init()
{
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"EL_FIRE_09.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XXXZ_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
			( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
		{
			StateChangeDashIfPossible();
		}
		else if ( true == IsOnSomethingFuture() )
		{
			StateChange( USI_DASH_JUMP_LANDING );
		}
		else if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	/*SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )*/

	CommonEventProcess();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XXXZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.27f ) == true )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elesis_SBH_Combo_XXXZ", this, NULL, IsHyperState(), GetPowerRate() * GetFireBallPowerRate() );
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_DASH_COMBO_SBH_XXXZ_FrameMoveFuture()
{
	CommonFrameMoveFuture();

	m_PhysicParam.passiveSpeed.x = GetDashSpeed();
}

// ȫ���� ��
void CX2GUEL::ESI_SI_A_SBH_FLAME_SWORD_Init()
{
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
}
void CX2GUEL::ESI_SI_A_SBH_FLAME_SWORD_EventProcess()
{
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.235f && GetAttackSuccess(true) == true )
	{
		StateChange( ESI_SI_A_SBH_FLAME_SWORD_FINISH );
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_A_SBH_FLAME_SWORD_FINISH_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// �ҳ���
void CX2GUEL::ESI_SI_A_SBH_FIREWORKS_Init()
{
	TextureReadyInBackground(L"EL_FIRE_07.dds");
	TextureReadyInBackground(L"Flame_REVOLVER.dds");
	XSkinMeshReadyInBackground(L"RVC_SA_ignitionCrow_circle.X");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.x");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	TextureReadyInBackground(L"DarkSmoke01.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"raven_beed_hit_circle.DDS");
	TextureReadyInBackground(L"Rose_explosion.Tga");
	XSkinMeshReadyInBackground(L"EffectSet_SA_SBH_BLAZING_DANCE_02.X");
}
void CX2GUEL::ESI_SI_A_SBH_FIREWORKS_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// �Ҳ� ���
void CX2GUEL::ESI_SI_A_SBH_SCALET_ROSE_Init()
{
	XSkinMeshReadyInBackground(L"RVC_SA_ignitionCrow_circle.X");
	TextureReadyInBackground(L"Rose_explosion_02.Tga");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	XSkinMeshReadyInBackground(L"WindBlade_MagicSquare03.X");
	XSkinMeshReadyInBackground(L"SI_A_SBH_SCALET_ROSE_attackSphere.X");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	TextureReadyInBackground(L"Rose_explosion.Tga");
	TextureReadyInBackground(L"DarkSmoke01.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	TextureReadyInBackground(L"raven_beed_hit_circle.DDS");
}
void CX2GUEL::ESI_SI_A_SBH_SCALET_ROSE_EventProcess()
{
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.33f && m_pXSkinAnim->GetNowAnimationTime() < 0.465f )
	{
        if ( CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_hSBHScaletRoseDamageEffect ) )
		{
			if( pEffect->GetDamageTimeNow() > 0 )
			{
				StateChange( ESI_SI_A_SBH_SCALET_ROSE_SUCCESS );
			}
		}
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_A_SBH_SCALET_ROSE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.25f ) == true  )
	{
		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 90.f;
		if(GetIsRight() == true)
		{
			vDamageEffectPos += (30.f * GetDirVector());
		}
		else
		{
			vDamageEffectPos -= (30.f * GetDirVector());
		}
        m_hSBHScaletRoseDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this, L"DamageEffect_Elesis_A_SBH_SCALET_ROSE_Start", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.465f ) == true )
	{
        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hSBHScaletRoseDamageEffect );
	}

	CommonFrameMove();
}
void CX2GUEL::ESI_SI_A_SBH_SCALET_ROSE_StateEnd()
{
	CommonStateEnd();

    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hSBHScaletRoseDamageEffect );
}
void CX2GUEL::ESI_SI_A_SBH_SCALET_ROSE_SUCCESS_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// ��Į�� �ҵ�
void CX2GUEL::ESI_SI_SA_SBH_SCALDIC_SWORD_Init()
{
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.x");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	XSkinMeshReadyInBackground(L"P_EBS_BoostDash_circle.X");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"RVC_SA_DeadlyRaid_circleLine.X");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	XMeshReadyInBackground(L"SCALDIC_SWORD_01.Y");
	XMeshReadyInBackground(L"SCALDIC_SWORD_03.Y");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
}
void CX2GUEL::ESI_SI_SA_SBH_SCALDIC_SWORD_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// �Ҳ��� ���
void CX2GUEL::ESI_SI_SA_SBH_WALL_OF_PROMINENCE_Init()
{
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"smoke02.dds");
	XSkinMeshReadyInBackground(L"BREAKING_FIST_Attack_Box.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_100x50x50_tall2.x");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	XSkinMeshReadyInBackground(L"SI_SA_SBH_WALL_OF_PROMINENCE_01.X");
}
void CX2GUEL::ESI_SI_SA_SBH_WALL_OF_PROMINENCE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_WALL_OF_PROMINENCE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.454f ) == true )
	{
		D3DXVECTOR3 vHandPos = GetBonePos(L"Bip01");
		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this,
			L"DamageEffect_Wall_of_Prominence_Projectile", GetPowerRate(), vHandPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( NULL != pDamageEffect )
		{
			CX2DamageEffect::DamageEffectDataInLua& DamageEffectData = pDamageEffect->GetDamageEffectDataInLua();

			if( 1 < DamageEffectData.m_vecDieDamageEffect.size() )
			{
				DamageEffectData.m_vecDieDamageEffect[0]->fLifeTime = m_fWallOfProminenceLifeTime;
				DamageEffectData.m_vecDieDamageEffect[1]->fLifeTime = m_fWallOfProminenceLifeTime;
			}

			if( 1 < DamageEffectData.m_vecHitDamageEffect.size() )
			{
				DamageEffectData.m_vecHitDamageEffect[0]->fLifeTime = m_fWallOfProminenceLifeTime;
				DamageEffectData.m_vecHitDamageEffect[1]->fLifeTime = m_fWallOfProminenceLifeTime;
			}
		}
	}

	CommonFrameMove();
}

// �������� ��
void CX2GUEL::ESI_SI_SA_SBH_BLAZE_WING_Init()
{
	TextureReadyInBackground(L"Black_Light.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	TextureReadyInBackground(L"smoke02.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SI_SA_SBH_BLAZE_WING_02.X");
	TextureReadyInBackground(L"secretVelder_perpit_fire_flame.dds");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZE_WING_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}

// ������¡ ����
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_START_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"PET_SILVERFOX_MIHOEffect02.tga");
	TextureReadyInBackground(L"raven_beed_hit_circle.DDS");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Rose_explosion_01.Tga");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"DarkSmoke01.dds");
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_START_StateStart()
{
	CommonStateStart();

	m_vecBlazingDanceTargetUid.clear();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_START_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_1ST );
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_1ST_StartFuture()
{
	CommonStateStartFuture();

	BlazingDanceFindTargetAndMove();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_1ST_EventProcess()
{
	if( true == m_bIsThereBlazingDanceTarget && true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_2ND );
	}
	else if( false == m_bIsThereBlazingDanceTarget )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_FINISH );
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_2ND_StartFuture()
{
	CommonStateStartFuture();

	BlazingDanceFindTargetAndMove();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_2ND_EventProcess()
{
	if( true == m_bIsThereBlazingDanceTarget && true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_3RD );
	}
	else if( false == m_bIsThereBlazingDanceTarget )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_FINISH );
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_3RD_StartFuture()
{
	CommonStateStartFuture();

	BlazingDanceFindTargetAndMove();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_3RD_EventProcess()
{
	if( true == m_bIsThereBlazingDanceTarget && true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_4TH );
	}
	else if( false == m_bIsThereBlazingDanceTarget )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_FINISH );
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_4TH_StartFuture()
{
	CommonStateStartFuture();

	BlazingDanceFindTargetAndMove();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_4TH_EventProcess()
{
	if( false == m_bIsThereBlazingDanceTarget || ( true == m_bIsThereBlazingDanceTarget && true == m_pXSkinAnimFuture->IsAnimationEnd() ) )
	{
		StateChange( ESI_SI_SA_SBH_BLAZING_DANCE_FINISH );
	}

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_StartFuture()
{
	CommonStateStartFuture();

	m_bIsBlazingDanceLastAttack = true;
	BlazingDanceFindTargetAndMove();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_StateEnd()
{
	m_bIsBlazingDanceLastAttack = false;
	CommonStateEnd();
}
void CX2GUEL::ESI_SI_SA_SBH_BLAZING_DANCE_FINISH_FrameMoveFuture()
{
	CommonFrameMoveFuture();

	if( NULL != g_pX2Game->GetWorld() &&
		NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( GetLastTouchLineIndex() );
		if( NULL != pLineData )
		{
			D3DXVECTOR3 vFinalDestDir = m_vec3BlazingDanceTargetPos - GetPos();
			D3DXVec3Normalize( &vFinalDestDir, &vFinalDestDir );
			D3DXVECTOR3 vRightVec = pLineData->dirVector;
			D3DXVec3Normalize( &vRightVec, &vRightVec );

			float fIsRight = D3DXVec3Dot( &vRightVec, &vFinalDestDir );
			bool bRight = ( fIsRight > 0.f ) ? true : false;

			m_FrameDataFuture.syncData.bIsRight = bRight;
		}
	}
}

// ���μ�Ʈ ���̾�
void CX2GUEL::ESI_SI_HA_SBH_ANCIENT_FIRE_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"Condense_Pulse01.dds");
	XSkinMeshReadyInBackground(L"EffectSet_SA_SBH_ANCIENT_FIRE01.X");
	XSkinMeshReadyInBackground(L"EffectSet_SA_SBH_ANCIENT_FIRE02.X");
	TextureReadyInBackground(L"EffectSet_SA_SBH_ANCIENT_FIRE_03.tga");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
	TextureReadyInBackground(L"PET_SILVERFOX_MIHOEffect02.tga");
	XSkinMeshReadyInBackground(L"RVC_SA_DeadlyRaid_circleLine.X");
	TextureReadyInBackground(L"SA_SGM_WAR_PRELUDE_Map01.dds");
	TextureReadyInBackground(L"smoke02.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_100x50x50_tall2.x");
	TextureReadyInBackground(L"star1.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
}
void CX2GUEL::ESI_SI_HA_SBH_ANCIENT_FIRE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	ZX_CANCEL_AFTER_EX( m_fAttackCancelAfterEx )

	CommonEventProcess();
}
void CX2GUEL::ESI_SI_HA_SBH_ANCIENT_FIRE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.887f ) == true )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01" );

		if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_SA_SBH_ANCIENT_FIRE_Attack01_Pvp", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_SA_SBH_ANCIENT_FIRE_Attack01_Dungeon", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	}
    if( m_pXSkinAnim->EventTimerOneshot( 2.177f ) == true )
	{
		SetLayer(XL_EFFECT_2);
		m_bSetCustomRenderParam = true;
		m_RenderParam.color = 0xff000000;
		SetAlphaObject(true);
		m_fAncientFireRenderTime = 2.4f;
	}

	CommonFrameMove();
}










///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �нú� �Լ�
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** @function : ChivalryProc_FrameMove
	@brief : �нú� ��ų - ����� ���� ó�� ������ ����
			 - �ش� ������ �� �ɷ��� ���� üũ�Ͽ� ���� �ɱ�
*/

void CX2GUEL::ChivalryProc_FrameMove()
{
	// ����� ������ ����� �ʾҴٸ� ó������ �ʱ�
	if( 0 == m_uiChivalryLevel )
		return;

	// 1�� �������� ó��
	m_fChivalryBuffCheckTime += m_fElapsedTime;
	if( m_fChivalryBuffCheckTime < 1.f )
	{
		return;
	}

	m_fChivalryBuffCheckTime = 0.f;
	// ���ݷ� ���� ���� ���� üũ
	{
		/// ����ö // 2013-07-18
		// A Ŭ���̾�Ʈ�� �� �Լ��� ȣ����, B Ŭ���̾�Ʈ�� ��Ƽ���� ���
		// B �� HP�� Ư�� % ���Ϸ� �������� true�� ���� �ؾ� �ϳ�
		// A Ŭ���̾�Ʈ������ B�� ��Ƽ��������
		// B Ŭ���̾�Ʈ������ B�� ��Ƽ���� �ƴ� �ڱ� �ڽ��̱� ������ ���� üũ �ؾ� ��
		// ���� ��Ƽ���� �ƴ� ������ Ŭ���̾�Ʈ������ üũ ��ü�� �ȵ�
		if( true == g_pX2Game->GetIsExsitancePartyMemberHPLessThanPer( _CONST_EL_::CHIVALRY_BUFF_CONDITION_HP_RATE, this ) )
		{
			// ���ݷ� ���� ����
			if( 0 < m_uiChivalryAttackBuffFactorID )
			SetBuffFactorToGameUnitByBuffFactorID( m_uiChivalryAttackBuffFactorID, m_uiChivalryLevel);
		}
	}

	// ���� ���� ���� ���� üũ
	{
		if(  _CONST_EL_::CHIVALRY_BUFF_CONDITION_HP_RATE >= (GetNowHp() / GetMaxHp()) )
		{
			// ���� ���� ����
			if( 0 < m_uiChivalryDefenceBuffFactorID )
				SetBuffFactorToGameUnitByBuffFactorID( m_uiChivalryDefenceBuffFactorID, m_uiChivalryLevel );
		}
	}
}

void CX2GUEL::StigmaOfFireProc_FrameMove()
{
	// �Ҳ��� ������ �����Ǿ��µ� ������ �ʾҴٸ� return
	// ��ų�� ����� �ʾҴٸ� ������ �������̹Ƿ� ��ų�� ��������� üũ�� ����.
	if( true == m_bIsStigmaOfFire )
		return;

	m_fStigmaOfFireCoolTime += m_fElapsedTime;
	if( m_fStigmaOfFireCoolTime < 0.5f )
	{
		m_bIsStigmaOfFire = false;
		return;
	}

	m_bIsStigmaOfFire = true;
}

void CX2GUEL::ApplyDebuffStigmaOfFire( const CX2DamageManager::DamageData& damageData_ )
{
	//�Ҳ��� ���� ��ų�� �����, ������ ��Ƽ�꿡�� �������� �ʴ´�. 0.5���� ��Ÿ���� ������ �ٽ� ��� �����ϴ�.
	if( CX2DamageManager::AT_NORMAL == damageData_.attackType && true == m_bIsStigmaOfFire && 0 < m_uiSkillLevelStigmaOfFire )
	{
		if( null != damageData_.optrDefenderGameUnit )
		{
			CX2GameUnit* pDefenderUnit = damageData_.optrDefenderGameUnit.GetObservable();
			if( NULL != pDefenderUnit )
			{
				pDefenderUnit->SetBuffFactorToGameUnitByBuffFactorID(static_cast<UINT>(BFI_DEBUFF_STIGMA_OF_FIRE), m_uiSkillLevelStigmaOfFire );
				//�Ҳ��� ���� ������ ��ø Ƚ���� �˻�.
				//��ø�� 5���Ǹ� �Ҳ��� ���� ������ �ɾ��ش�.
				vector<KBuffFactor> VecDebuffFactor;
				pDefenderUnit->GetBuffFactorOnlyDebuff( VecDebuffFactor );


				for( const KBuffFactor& kFactor_ : VecDebuffFactor )
				{
					if( kFactor_.m_BuffIdentity.m_eBuffTempletID == BTI_DEBUFF_STIGMA_OF_FIRE && kFactor_.m_ucAccumulationCountNow > 4 )
					{
						pDefenderUnit->SetBuffFactorToGameUnitByBuffFactorID(static_cast<UINT>(BFI_DEBUFF_JUDGEMENT_FIRE), m_uiSkillLevelStigmaOfFire );
						pDefenderUnit->SetBuffFactorToGameUnitByBuffFactorID(static_cast<UINT>(BFI_DEBUFF_JUDGEMENT_FIRE2), m_uiSkillLevelStigmaOfFire );
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_SBH_Judgement_Fire", GetPowerRate() * m_fStigmaOfFireDamageValue, pDefenderUnit->GetPos(), GetRotateDegree(), GetRotateDegree() );
					}
				}
			}
		}

		m_bIsStigmaOfFire = false;
		m_fStigmaOfFireCoolTime = 0.0f;
	}
}

void CX2GUEL::ApplyDebuffFlameSword( const CX2DamageManager::DamageData& damageData_ )
{
	//ȫ���� �� ��ų�� �����, ������ ��Ƽ�꿡�� �������� �ʴ´�.
	if( CX2DamageManager::AT_NORMAL == damageData_.attackType && true == m_bIsFrameSword )
	{
		if( null != damageData_.optrDefenderGameUnit )
		{
			CX2GameUnit* pDefenderUnit = damageData_.optrDefenderGameUnit.GetObservable();
			if( NULL != pDefenderUnit )
			{
				pDefenderUnit->SetBuffFactorToGameUnitByBuffFactorID(static_cast<UINT>( BFI_DEBUFF_HIGH_FEVER ), m_uiSkillLevelFlameSword );
				pDefenderUnit->SetBuffFactorToGameUnitByBuffFactorID(static_cast<UINT>( BFI_DEBUFF_FLAME_SWORD ), m_uiSkillLevelFlameSword );
			}
		}
	}
}

void CX2GUEL::FireBlossomsProc_FrameMove()
{
	// ��ų�� ����� �ʾҴٸ� return
	if( m_uiFireBlossomsSkillLevel < 1 )
		return;


	if( true == m_bIsFireBlossoms )
	{
		bool bUse = false;

		// ���Ӱ� ������� �� BuffTempletID�� �����Ǿ����� �ش� ���� ����.
		if( m_eFireBlossomsSaveDebuffID != BTI_NONE )
		{
			EraseBuffTempletFromGameUnit( m_eFireBlossomsSaveDebuffID );
			bUse = true;
		}
		else if( CX2DamageManager::EDT_NONE != GetApplyWeaponExtraDamageType() )
		{
			// BuffTempletID�� �����ȵ� ���¶�� ���� �� ���ֿ� ����� ���� �Ӽ� üũ
			AccessExtraDamagePack().Init( GetApplyWeaponExtraDamageType() );
			bUse = true;
		}
		else
		{
			// ���Ӱ� BuffTempletID�� �������� �ʾҰ�, ����Ӽ��� �ɸ��� �ʾҴٸ�
			// ������ �ɷ��ִ� ����� �� �ϳ��� ����
			vector<KBuffFactor> VecDebuffFactor;
			GetBuffFactorOnlyDebuff( VecDebuffFactor );

			if( false == m_vecBuffTempletPtr.empty() )
			{
				BUFF_TEMPLET_ID eEraseDebuff = BTI_NONE;

				for( CX2BuffTempletPtr ptr : m_vecBuffTempletPtr )
				{
					if ( NULL != ptr && false == ptr->GetDidFinish() && ptr->IsDeBuff() && false == ptr->IsWorldBuff() )
					{
						// �ൿ �Ҵ� ������� ���� ���� �߰�
						if ( false == ptr->IsNonControlDebuff() )
							eEraseDebuff = ptr->GetBuffTempletID();
					}
				}

				if( eEraseDebuff != BTI_NONE )
				{
					EraseBuffTempletFromGameUnit( eEraseDebuff );
					bUse = true;
				}
			}
		}


		if( true == bUse )
		{
			if( NULL != g_pData->GetPicCharBlue() )
				g_pData->GetPicCharBlue()->DrawText( L"RESIST", GetHeadBonePos(), GetDirVector(), CKTDGPicChar::AT_CENTER );

			m_eFireBlossomsSaveDebuffID = BTI_NONE;
			m_fFireBlossomsCoolTime = 0.f;
			m_bIsFireBlossoms = false;
			SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_FIRE_BLOSSOMS, m_uiFireBlossomsSkillLevel );
		}
	}
	else
	{
		// ��Ÿ���� üũ
		m_fFireBlossomsCoolTime += m_fElapsedTime;
		m_bIsFireBlossoms =  m_fFireBlossomsCoolTime >= m_fFireBlossomsCheckTime ;
	}
}

void CX2GUEL::BlazingDanceFindTargetAndMove()
{
	// �� ������ �̵������� ������ �ִ� Ÿ���� ��ġ�� ã�� �� ������ �̵���Ŵ
	// �ѹ� Ÿ���� �Ǿ��� ������ �켱������ �ڷ� �и���.

	m_bIsThereBlazingDanceTarget = false;
	CX2GameUnit* pDidTargetingUnit = NULL;
	const CKTDGLineMap::LineData* pDidLineData = NULL;
	if ( NULL != g_pX2Game->GetWorld() &&
				NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
			bool IsDidTargeting = false;
			if( NULL != pUnit )
			{
				if( true == pUnit->GetIsBosRaidNPC() )
					continue;

				const CKTDGLineMap::LineData* pLineData =
					g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pUnit->GetLastTouchLineIndex() );

				if( NULL == pLineData )
					continue;

				if( false == BlazindDanceCheckTargetPosition( pUnit, pLineData ) )
					continue;

				//�̹� �ѹ� Ÿ���� �Ǿ��� �����̶�� �ϴ� ����
				for( UidType uTargetUid_ : m_vecBlazingDanceTargetUid )
				{
					if( uTargetUid_ == pUnit->GetUnitUID() )
					{
						IsDidTargeting = true;
						break;
					}
				}
				if( true == IsDidTargeting )
				{
					pDidTargetingUnit = pUnit;
					pDidLineData = pLineData;
					continue;
				}

				D3DXVECTOR3 MyPos = GetPos();
				D3DXVECTOR3 TargetPos = pUnit->GetPos();
				if( GetDistance3Sq( MyPos, TargetPos ) < 490000.0f )
				{
					m_FrameDataFuture.syncData.position		= TargetPos;
					m_bIsThereBlazingDanceTarget = true;

					m_vecBlazingDanceTargetUid.push_back( pUnit->GetUnitUID() ); //Ÿ�ٵ� ������ UID�� ����

					if( true == m_bIsBlazingDanceLastAttack ) //������ Ÿ���̶�� ������ȯ�� ���� Ÿ���� ��ġ�� ����
					{
						m_vec3BlazingDanceTargetPos = TargetPos;
					}
					break;
				}
			}
		}
	}

	// ������ �̵���ų Ÿ���� ���ٸ� �̹� �ѹ� Ÿ���� �Ǿ��� ������ �ִ��� üũ�ϰ� �ش� ��ġ�� �̵�.
	if( false == m_bIsThereBlazingDanceTarget && NULL != pDidTargetingUnit && NULL != pDidLineData )
	{
		D3DXVECTOR3 MyPos = GetPos();
		D3DXVECTOR3 TargetPos = pDidTargetingUnit->GetPos();
		if( GetDistance3Sq( MyPos, TargetPos ) < 490000.0f )
		{
			bool bMove = true;

			if( false == BlazindDanceCheckTargetPosition( pDidTargetingUnit, pDidLineData ) )
				bMove = false;

			if( true == bMove )
			{
				m_FrameDataFuture.syncData.position		= TargetPos;
				m_bIsThereBlazingDanceTarget = true;

				if( true == m_bIsBlazingDanceLastAttack ) //������ Ÿ���̶�� ������ȯ�� ���� Ÿ���� ��ġ�� ����
				{
					m_vec3BlazingDanceTargetPos = TargetPos;
				}
			}
		}
	}
}
bool CX2GUEL::BlazindDanceCheckTargetPosition( CX2GameUnit* pUnit_, const CKTDGLineMap::LineData* pLineData_  )
{
	// Ÿ���� ����, ��ġ�� �� ������ �̵���Ű�⿡ �������� üũ

	bool bValiedTargetPosition = true;


	if( pUnit_->GetTeam() == GetTeam() )	//���� ������ üũ
	{
		bValiedTargetPosition = false;
	}
	else if( pUnit_->GetInvincible() == true )	//������������ üũ
	{
		bValiedTargetPosition = false;
	}
	else if( pUnit_->GetNowHp() <= 0.f )	//��� �ִ��� üũ
	{
		bValiedTargetPosition = false;
	}
	else
	{
		D3DXVECTOR3 MyPos = GetPos();
		D3DXVECTOR3 TargetPos = pUnit_->GetPos();
		int			lineIndex = 0;


		if( NULL == pLineData_ )
		{
			bValiedTargetPosition = false;
		}
		else if( pLineData_->lineType != CKTDGLineMap::LT_NORMAL || pLineData_->bEnable == false ) //���θ��� ��ȿ���� üũ
		{
			bValiedTargetPosition = false;
		}
		else if( CX2GameUnit::GUT_NPC == pUnit_->GetGameUnitType() )
		{

			CX2GUNPC* pMonster = static_cast<CX2GUNPC*>(pUnit_);


			if( NULL != pMonster &&							//���� AI�� �� �� �ִ� ��ġ�� �̵��ϴ� ������ ���� üũ,
				NULL != pMonster-> GetNPCAI()&&
				true == pMonster->GetNPCAI()->GetAIFly() &&
				TargetPos.y < pLineData_->startPos.y )
			{
					bValiedTargetPosition = false;
			}
			else	// Ʈ�� ���� ���� üũ, üũ �������� üũ
			{
				if( CX2UnitManager::NCT_THING_TRAP == pMonster->GetNPCTemplet().m_ClassType ||
					CX2UnitManager::NCT_THING_CHECKER == pMonster->GetNPCTemplet().m_ClassType )
				bValiedTargetPosition = false;
			}
		}
		else if( g_pX2Game->GetWorld()->GetLineMap()->CanDown( MyPos, LINE_RADIUS, &TargetPos, &lineIndex, false ) == false ) //��ġ �Ʒ��� ������ �� �ִ� ���θ��� �ִ��� üũ
		{
			bValiedTargetPosition = false;
		}
	}

	return bValiedTargetPosition;
}

bool CX2GUEL::GetUnitClassLine( UNIT_CLASS_LINE UnitClassLine_ )
{
	switch( UnitClassLine_ )
	{
	case CX2GUUser::UCL_FIRST:
		{
			switch( GetUnitClass() )
			{
			case CX2Unit::UC_ELESIS_SABER_KNIGHT:
			case CX2Unit::UC_ELESIS_GRAND_MASTER:

				return true;
				break;
			default:
				break;
			}
		} break;

	case UCL_SECOND:
		{
			switch( GetUnitClass() )
			{
			case CX2Unit::UC_ELESIS_PYRO_KNIGHT:
			case CX2Unit::UC_ELESIS_BLAZING_HEART:

				return true;
				break;
			default:
				break;
			}
		} break;
	default:
		break;
	}

	return false;
}


/* virtual */ void CX2GUEL::ChangeDamageData()
{
	if ( true == IsFirstStrikeRelateState() && GetUnitClass() == CX2Unit::UC_ELESIS_GRAND_MASTER )
	{
		CX2GUEL::CFirstStrikeData cFirstStrikeData = GetFirstStrikeData();
		float fAddfStopTimeDef = cFirstStrikeData.GetFirstStrikeAddStopTimeDef();
		if ( GetNowStateID() == ESI_JUMP_ATTACK_X )	// ���� X Ŀ�ǵ忡 ���ؼ��� ���� ó��, ���� �ð� 0.5��
		{
			fAddfStopTimeDef = fAddfStopTimeDef * 0.5f;
		}
		m_DamageData.fStopTimeDef = m_DamageData.fStopTimeDef + fAddfStopTimeDef;	// ���� �ð� ���� ( ���밪 n�� �߰� )
		m_DamageData.fHitAddMP = m_DamageData.fHitAddMP * cFirstStrikeData.GetFirstStrikeAddIncreaseMPRate();		// ���� ȸ���� ���� ( ���� ���� )
		m_DamageData.damage.fPhysic = m_DamageData.damage.fPhysic * cFirstStrikeData.GetFirstStrikeAddDamage();		// ������ ���� ( ���� ���� )
	}
}

void CX2GUEL::DrasticChargeProc_FrameMove()
{
	switch ( GetNowStateSkillID() )
	{
		case CX2SkillTree::SI_A_SGM_DRASTIC_CHARGE :
			break;

		default:
			{
				if ( NULL != g_pX2Game->GetEffectSet() )
				{
					if ( INVALID_EFFECTSET_HANDLE != m_hDrasticChargeLoop )
					{
						g_pX2Game->GetEffectSet()->StopEffectSet( m_hDrasticChargeLoop );
					}
					if ( INVALID_EFFECTSET_HANDLE != m_hDrasticChargeGroundFire )
					{
						g_pX2Game->GetEffectSet()->StopEffectSet( m_hDrasticChargeGroundFire );
					}
				}
			}
			break;
	}
}

void CX2GUEL::XX_DOWN_XProc_FrameMove()
{
	switch ( GetNowStateID() )
	{
	case ESI_COMBO_SGM_XX_DOWN_X_START :
	case ESI_COMBO_SGM_XX_DOWN_X_LOOP :
	case ESI_COMBO_SGM_XX_DOWN_X_END :
		break;

	default:
		{
			if ( NULL != g_pX2Game->GetEffectSet() )
			{
				if ( INVALID_EFFECTSET_HANDLE != m_hXXDownXLoop )
				{
					g_pX2Game->GetEffectSet()->StopEffectSet( m_hXXDownXLoop );
				}
			}
		}
		break;
	}
}

void CX2GUEL::SnapCrasherProc_FrameMove()
{
	switch ( GetNowStateSkillID() )
	{
	case CX2SkillTree::SI_A_SGM_SNAP_CRASHER :
		break;

	default:
		{
			if ( NULL != g_pX2Game->GetEffectSet() )
			{
				if ( INVALID_EFFECTSET_HANDLE != m_hSnapCresherLoop )
				{
					g_pX2Game->GetEffectSet()->StopEffectSet( m_hSnapCresherLoop );
				}

				if ( INVALID_EFFECTSET_HANDLE != m_hSnapCreaherLoopAttack )
				{
					g_pX2Game->GetEffectSet()->StopEffectSet( m_hSnapCreaherLoopAttack );
				}

			}
		}
		break;
	}
}


/*virtual*/void CX2GUEL::SetDamageExceptionProcess( OUT CX2DamageManager::DamageData* pDamageData_ ,OUT float& fPowerRate)
{
	CX2GUUser::SetDamageExceptionProcess(pDamageData_, fPowerRate);

	if( WSS_VIGOR == GetWayOfSwordState() )
	{
		switch( pDamageData_->m_WayofSwordType )
		{
		// ��ǳ Ŀ�ǵ� �ٿ��ġ ����
		case CX2DamageManager::WST_VIGOR_NORMAL:
			{
				float fGaleForceDownDecreaseRate = _CONST_EL_::GALE_FORCE_DOWN_DECREASE_RATE;

				// ���������� ���ҷ� 1/2
				if( NULL != g_pX2Game && CX2Game::GT_PVP == g_pX2Game->GetGameType())
					fGaleForceDownDecreaseRate = _CONST_EL_::GALE_FORCE_DOWN_DECREASE_RATE_PVP;

				pDamageData_->fForceDownValue *= fGaleForceDownDecreaseRate;
			} break;
		default:
			break;
		}
	}
	else if( WSS_DESTRUCTION == GetWayOfSwordState() )
	{
		switch( pDamageData_->m_WayofSwordType )
		{
		// ���� ��ų, Ŀ�ǵ� ��� ���� ����
		case CX2DamageManager::WST_DESTRUCTION_NORMAL:
		case CX2DamageManager::WST_DESTRUCTION_ACTIVE:
		case CX2DamageManager::WST_DESTRUCTION_SPECIAL:
			{
				pDamageData_->m_bIgnoreStateDefence = true;
			} break;
		default:
			break;
		}
	}
}

void CX2GUEL::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove(0.f, 0.1f, true);
}
void CX2GUEL::CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ /*= false*/ )
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );

    if( m_pXSkinAnim->EventTimerOneshot( fTime1_ ) )
	{
		ShowMinorParticleHyperModeChange();
	}

    if( m_pXSkinAnim->EventTimerOneshot( fTime2_ ) )
	{
		ShowMinorParticleHyperModeShock();

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
