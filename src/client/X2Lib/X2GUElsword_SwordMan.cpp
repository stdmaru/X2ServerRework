#include "stdafx.h"
#include "./X2GUElsword_SwordMan.h"

#define LEAP_GRAVITY 9.8f





// �ʻ�� �� �� cut in image
static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] =
{
	// ���ҵ�, �ҵ峪��Ʈ, ��������Ʈ
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_1.dds"),// D3DXVECTOR2(655.0f,655.0f), D3DXVECTOR3(192.0f,496.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_2.dds"),// D3DXVECTOR2(691.0f,691.0f), D3DXVECTOR3(202.0f,423.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_3.dds"),// D3DXVECTOR2(670.0f,670.0f), D3DXVECTOR3(275.0f,471.0f,0.0f) ),
	},

	// �ε� ����Ʈ
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_LK_1.dds"),// D3DXVECTOR2( 601.f, 601.f ), D3DXVECTOR3( 269.f, 488.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_LK_1.dds"),// D3DXVECTOR2( 601.f, 601.f ), D3DXVECTOR3( 269.f, 488.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_LK_1.dds"),// D3DXVECTOR2( 601.f, 601.f ), D3DXVECTOR3( 269.f, 488.f, 0.f ) ),
	},

	// �� �����̾�
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_RS_1.dds"),// D3DXVECTOR2( 615.f, 615.f ), D3DXVECTOR3( 268.f, 475.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_RS_1.dds"),// D3DXVECTOR2( 615.f, 615.f ), D3DXVECTOR3( 268.f, 475.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_RS_1.dds"),// D3DXVECTOR2( 615.f, 615.f ), D3DXVECTOR3( 268.f, 475.f, 0.f ) ),
	},

	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_IS_1.dds"),// D3DXVECTOR2( 615.f, 615.f ), D3DXVECTOR3( 310.f, 433.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_IS_1.dds"),// D3DXVECTOR2( 615.f, 615.f ), D3DXVECTOR3( 310.f, 433.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Elsword_IS_1.dds"),// D3DXVECTOR2( 615.f, 615.f ), D3DXVECTOR3( 310.f, 433.f, 0.f ) ),
	},
};





namespace _CONST_ELSWORD_
{
	const float SKILL_DEFENCE_MP_COST_PER_SEC = 3.f;		// ���� ��ų�� �ʴ� ���� �Ҹ�

	const float MAGIC_FIRE_BALL_MP_COST = 6.f;
	const float MAGIC_ERS_XZ_MP_COST = 4.f;
	const float MAGIC_ERS_XZZ_MP_COST = 0.f;
	const float MAGIC_ERS_DASH_JUMP_XZ_MP_COST = 4.f;
	const float MAGIC_ERS_DASH_JUMP_XZZ_MP_COST = 0.f;

	const float VIGOR_FIRE_BALL_MP_COST = 5.f;			// �ı�
	const float VIGOR_ERS_XZ_MP_COST = 3.f;				// �ı�
	const float VIGOR_ERS_XZZ_MP_COST = 6.f;			// �ı�
	const float VIGOR_ERS_DASH_JUMP_XZ_MP_COST = 3.f;	// �ı�
	const float VIGOR_ERS_DASH_JUMP_XZZ_MP_COST = 3.5f;	// Ȱ�� - MP�Ҹ� 50% ����

	const float MAGIC_PHANTOM_SWORD_DELAY	= 0.2f;

	const int	CONST_BRUTAL_SLAYER_LEVEL = 5;


	const int BLADE_RAIN_SKILL_RANGE = 250;								// �����̵� ���� �Ÿ�
	const int BLADE_RAIN_GET_SUMMON_POS_CHECK_LINEMAP_COUNT = 10;		// �����̵� ���� ���θ� üũ �Ÿ�

}





CX2GUElsword_SwordMan::CX2GUElsword_SwordMan( int unitIndex, int teamNum,
	lua_State*   pScriptLuaState,
	int frameBufferNum, CX2Unit* pUnit )
: CX2GUUser( unitIndex, teamNum,
	pScriptLuaState,
	frameBufferNum, pUnit )
, m_hRuneSlayerDashJumpXZ( INVALID_DAMAGE_EFFECT_HANDLE )
, m_hRuneSlayerComboXZ( INVALID_EFFECTSET_HANDLE )
, m_hSwordEnchant( INVALID_EFFECTSET_HANDLE )
, m_fRollingSmashDuration( 0.f )
, m_bIsMagicKnightWinMotion2( false )
, m_bERS_DASH_JUMP_XZ_Rune_Created( false )
, m_bERS_COMBO_XZ_Rune_Created( false )
, m_hDamageEffectHarshChaserBlade( INVALID_DAMAGE_EFFECT_HANDLE )
, m_bShowWeaponEnchantEffect( true )
, m_fAutoGuardRate( 0.f )
, m_hDamageEffectPiercingSword( INVALID_DAMAGE_EFFECT_HANDLE )
, m_fPiercingSwordProjectileRange( 0.f )
, m_fRuneProjectileRangeIncPercent( 0.f )
, m_fRuneProjectileSizeIncPercent( 0.f )
, m_bIsGuardMotion( false )
, m_ucSaveDebuffCount( 0 )
, m_bAutoGuardBlocking( false )
, m_fIncreaseCoolTimeRel( 1.f )
, m_fActiveSkillAddPowerRate( 1.f )
, m_hEffectSwordShield( INVALID_EFFECTSET_HANDLE )
, m_fSwordShieldEffectiveTime( 0.f )
, m_iSkillLevelInduranceOfRevenge( 0 )
{
	m_bReAttackZ1Right					= false;
	m_fReAttackZ1Time					= 0.0f;
	m_fReDashJumpXTime					= 0.0f;



	m_hMeshInstAirSlash1				= INVALID_MESH_INSTANCE_HANDLE;

	m_hMeshInstFatalFury				= INVALID_MESH_INSTANCE_HANDLE;

	InitializeElswordMajorParticleArray();
	InitializeElswordMinorParticleArray();

	m_hSeqAssualtSlashRing				= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqAssualtSlashDust				= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hSeqHandFire						= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_StartPosToLeap					= D3DXVECTOR3(0,0,0);
	m_DirVecToLeap						= D3DXVECTOR3(0,0,0);
	m_fVerticalSpeedToLeap				= 0.0f;
	m_fParallelSpeedToLeap				= 0.0f;
	m_fHeightToLeap						= 0.0f;
	m_DegreeToLeap						= 0;
	m_fTimeToLeap						= 0.0f;
	m_fElapsedTimeToLeap				= 0.0f;






	m_fAttackPowerRateFireBall = 1.f;

	m_fWindMillDuration = 0.f;
	m_hWindMill = INVALID_EFFECTSET_HANDLE;
	m_hWindMillLand = INVALID_EFFECTSET_HANDLE;
	m_hSwordFire = INVALID_EFFECTSET_HANDLE;


	m_pSoundWindMill = NULL;
	m_pSoundWindMill = g_pKTDXApp->GetDeviceManager()->OpenSound( L"Elsword_WindmillSpin.ogg" );



    m_hDoubleSlash = INVALID_DAMAGE_EFFECT_HANDLE;

	//SetWayOfSwordState( static_cast<int>( WSS_CENTER ) );
	m_hEffectVigor				= INVALID_EFFECTSET_HANDLE;
	m_hEffectDestruction		= INVALID_EFFECTSET_HANDLE;
	m_fDestructionRageTime		= 0.f;
	m_fAnimSpeedUpByVigor		= 0.f;

	m_hSecondBladeSheath		= INVALID_MESH_INSTANCE_HANDLE;
	m_hThirdBladeSheathMesh		= INVALID_MESH_INSTANCE_HANDLE;

	m_fDamageDataChangeTime		= 0.f;
	m_fDamageDataChangeTime2	= 0.f;

	m_iETKComboXXXLoopCount		= 0;

	m_hSwordFallBladeFirst		= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeSecond		= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeThird		= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeFourth		= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeFirst_Memo		= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeSecond_Memo	= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeThird_Memo		= INVALID_MESH_INSTANCE_HANDLE;
	m_hSwordFallBladeFourth_Memo	= INVALID_MESH_INSTANCE_HANDLE;


	m_fRollingHpConsumeRel			= 0.f;
	m_fRollingSpeedRel				= 0.f;
	m_fHarshChaserProjectiveRange	= 0.f;
	m_fMaelStormEffectiveTime		= 0.f;

	m_bCompactCounterEnable			= false;
	m_fCompactCounterCoolTime		= 0.f;

    m_hFinalStrikeBlackHole			= INVALID_DAMAGE_EFFECT_HANDLE;

	m_fThrowStartTime		= 0.6666f;
	m_vThrowStartPosOffset	= D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity	= D3DXVECTOR3(2000.f, 100.f, 0.f);

	m_bFirstAttackSuccess = false;
	m_fMagicReflectRate = 0.f;

	m_fDashStateTime = 0.f;
	m_fMindOfFighterConsumeMpRate = 0.f;
	m_iBrutalSlayerActivatedLevel = 0;
	for(int i=0; i<3; ++i)
    {
        m_hDamageSwordBlasting[i] = INVALID_DAMAGE_EFFECT_HANDLE;
    }
}




CX2GUElsword_SwordMan::~CX2GUElsword_SwordMan(void)
{
	SAFE_CLOSE( m_pSoundWindMill );



	DeleteElswordMajorParticle();
	DeleteElswordMinorParticle();

	g_pData->GetUIMajorParticle()->DestroyInstanceHandle( m_hSeqHandFire );

	g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMill );
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMillLand );
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hSwordFire );
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hSwordEnchant );

	g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hThirdBladeSheathMesh );
    if ( m_hFinalStrikeBlackHole != INVALID_DAMAGE_EFFECT_HANDLE )
	    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hFinalStrikeBlackHole );

	for(int i=0; i<3; ++i)
	{
        if ( m_hDamageSwordBlasting[i] != INVALID_DAMAGE_EFFECT_HANDLE )
            g_pX2Game->GetDamageEffect()->DestroyInstanceHandle(m_hDamageSwordBlasting[i]);
	}

}

/*static*/
void	CX2GUElsword_SwordMan::AppendDeviceToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}


void CX2GUElsword_SwordMan::ReInit( bool bRandom, int startPosIndex )
{
	CX2GUUser::ReInit( bRandom, startPosIndex );
	m_fReDashJumpXTime		= 0.0f;


	if( IsMyUnit() == true && CX2Game::GT_PVP == g_pX2Game->GetGameType() )
	{
		const WAY_OF_SWORD_STATE eWayOfSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

		if( (GetTeam() == CX2Room::TN_RED && g_pX2Game->GetRedTeamKill() > g_pX2Game->GetBlueTeamKill()) ||
			(GetTeam() == CX2Room::TN_BLUE && g_pX2Game->GetRedTeamKill() < g_pX2Game->GetBlueTeamKill()) )
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
			//m_GageManager.UpdateUIElswordWSP( WSS_CENTER );
		}
		else
		{
			if( eWayOfSwordState == WSS_VIGOR )
			{
				End_SwordmanVigor();
				SetWayOfSwordState( static_cast<int>( WSS_TOWARD_VIGOR ) );
				//m_GageManager.UpdateUIElswordWSP( WSS_TOWARD_VIGOR );
			}
			else if( eWayOfSwordState == WSS_DESTRUCTION )
			{
				End_SwordmanDestruction();
				SetWayOfSwordState( static_cast<int>( WSS_TOWARD_DESTRUCTION ) );
				//m_GageManager.UpdateUIElswordWSP( WSS_TOWARD_DESTRUCTION );
			}
		}
		m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
	}
	m_fDestructionRageTime = 0.f;

}

/*virtual*/ int	CX2GUElsword_SwordMan::GetWayOfSwordState() const
{
	return GetElswordGageData()->GetWayOfSwordState();
}

void CX2GUElsword_SwordMan::SetWayOfSwordState( const int iWayOfSwordState_ )
{
	GetElswordGageData()->SetWayOfSwordState( iWayOfSwordState_ );
}

/*virtual*/ float CX2GUElsword_SwordMan::GetWayOfSwordPoint() const
{
	return GetElswordGageData()->GetWayOfSwordPoint();
}

/*virtual*/ void CX2GUElsword_SwordMan::SetWayOfSwordPoint( float fWayOfSwordPoint_ )
{
	if( fWayOfSwordPoint_ > g_pData->GetDamageManager()->GetVigorStateThreshold() )
		fWayOfSwordPoint_ = g_pData->GetDamageManager()->GetVigorStateThreshold();

	if( fWayOfSwordPoint_ < g_pData->GetDamageManager()->GetDestStateThreshold() )
		fWayOfSwordPoint_ = g_pData->GetDamageManager()->GetDestStateThreshold();

	GetElswordGageData()->SetWayOfSwordPoint( fWayOfSwordPoint_ );
}

void CX2GUElsword_SwordMan::Start_SwordmanVigor()
{
	// ���� ���� ������ ������ ���� �޵��� ///////////////////////////////////////
	const float MAGIC_ANIM_UP_SPEED_FACTOR = 0.2f;
	float fAnimSpeedUpOriginal = 100.0f * CX2SocketItem::GetFinalAnimationSpeedPercent( m_SocketData.m_fAnimationSpeedRate, GetUnitLevel() );
	fAnimSpeedUpOriginal = fAnimSpeedUpOriginal / 100.f;

	float fAnimSpeedUpCorrection = fAnimSpeedUpOriginal;
	fAnimSpeedUpCorrection += MAGIC_ANIM_UP_SPEED_FACTOR;

	fAnimSpeedUpCorrection = CX2SocketItem::GetOptionRateCorrection( fAnimSpeedUpCorrection );

	fAnimSpeedUpOriginal = fAnimSpeedUpOriginal + 1.f;
	fAnimSpeedUpCorrection = fAnimSpeedUpCorrection + 1.f;

	m_fAnimSpeedUpByVigor = fAnimSpeedUpCorrection/fAnimSpeedUpOriginal;

	//////////////////////////////////////////////////////////////////////////

	// �̼� ���� ������ ������ ���� �޵��� ///////////////////////////////////////
	const float MAGIC_MOVE_UP_SPEED_FACTOR = 0.2f;
	float fMoveSpeedUpOriginal = 1.0f + CX2SocketItem::GetFinalMoveSpeedPercent( m_SocketData.m_fMoveSpeed, GetUnitLevel() );
	float fMoveSpeedUpCorrection = fMoveSpeedUpOriginal + MAGIC_MOVE_UP_SPEED_FACTOR;
	fMoveSpeedUpCorrection = CX2SocketItem::GetOptionRateCorrection( fMoveSpeedUpCorrection );

	fMoveSpeedUpOriginal = fMoveSpeedUpOriginal + 1.f;
	fMoveSpeedUpCorrection = fMoveSpeedUpCorrection + 1.f;

	float fMoveSpeedUpRate = 1.f;
	fMoveSpeedUpRate = fMoveSpeedUpCorrection/fMoveSpeedUpOriginal;

	SetVecMoveSpeedFactor( fMoveSpeedUpRate,	99999.f, CX2GameUnit::SFI_SPEED_8, CX2GameUnit::SFST_UPDATE );
	//////////////////////////////////////////////////////////////////////////

	m_hEffectVigor = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_VigorStart", this );

	if ( NULL != g_pX2Game->GetWorld() && GetShowCutInAndChangeWorldColor() )
	{
		g_pX2Game->GetWorld()->SetWorldColor( 0xff222255 );
		g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 4.0f );
	}

	PlaySound( L"elsword_vitality_use_VOX.ogg" );
	//PlaySound( L"elsword_vitality_loop.ogg", true, true );

	m_CommonState.m_Wait = ESSI_SWORD_WAIT;
	return;
}
void CX2GUElsword_SwordMan::End_SwordmanVigor()
{
	SetAnimSpeedFactor( 1.f,	0.f, CX2GameUnit::SFI_SPEED_8, CX2GameUnit::SFST_UPDATE );
	SetVecMoveSpeedFactor( 1.f, 0.f, CX2GameUnit::SFI_SPEED_8, CX2GameUnit::SFST_UPDATE );

	if ( NULL != g_pX2Game->GetWorld() )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectVigor );
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_VigorEnd", this );
	}

	//g_pKTDXApp->GetDeviceManager()->StopSound( L"elsword_vitality_loop.ogg" );
	PlaySound( L"elsword_vitality_end.ogg" );

	m_CommonState.m_Wait = USI_WAIT;
	return;
}
void CX2GUElsword_SwordMan::Start_SwordmanDestruction()
{
	m_hEffectDestruction = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_DestructionStart", this );

	if ( NULL != g_pX2Game->GetWorld() && GetShowCutInAndChangeWorldColor() )
	{
		g_pX2Game->GetWorld()->SetWorldColor( 0xff552222 );
		g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 4.0f );
	}

	PlaySound( L"elsword_demolition_use_VOX.ogg" );
	//PlaySound( L"elsword_demolition_loop.ogg", true, true );

	m_CommonState.m_Wait = ESSI_SWORD_WAIT;
	return;
}
void CX2GUElsword_SwordMan::End_SwordmanDestruction()
{
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectDestruction );
	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_DestructionEnd", this );

	//g_pKTDXApp->GetDeviceManager()->StopSound( L"elsword_demolition_loop.ogg" );
	PlaySound( L"elsword_demolition_end.ogg" );

	m_CommonState.m_Wait = USI_WAIT;
	return;
}
void CX2GUElsword_SwordMan::AddComboToBrutalSlayer( const CX2DamageManager::DamageData& /*damageData_*/ )
{
	const int iComboNumber
		= ( NULL != GetComboManager() ? GetComboManager()->GetCombo() : 0 );

	if ( iComboNumber > 0
		 && (iComboNumber % 5) == 0 )
	{
		const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ETK_BRUTAL_SLAYER, true );

		if( iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ETK_BRUTAL_SLAYER );
			if( NULL != pSkillTemplet )
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}

		if ( m_iBrutalSlayerActivatedLevel > 0 )
		{
			++m_iBrutalSlayerActivatedLevel;
			if ( _CONST_ELSWORD_::CONST_BRUTAL_SLAYER_LEVEL < m_iBrutalSlayerActivatedLevel )
				m_iBrutalSlayerActivatedLevel = _CONST_ELSWORD_::CONST_BRUTAL_SLAYER_LEVEL;
		}
	}


}

void CX2GUElsword_SwordMan::DamageReact( CX2DamageManager::DamageData* pDamageData )
{
	KTDXPROFILE();

	bool bCheckExtraDamage = false;

	switch( pDamageData->m_ExtraDamage.m_ExtraDamageType )
	{
	case CX2DamageManager::EDT_MANA_DAMAGE:
	case CX2DamageManager::EDT_DAMAGE_RATE:
	case CX2DamageManager::EDT_WATER_HOLD:
	case CX2DamageManager::EDT_ATTACK_ALL_TEAM:
		bCheckExtraDamage = true;
		break;
	default:
		break;
	}

	m_bAutoGuardBlocking = false;
	if( bCheckExtraDamage == false && CX2DamageManager::AT_NORMAL == pDamageData->attackType && pDamageData->m_bNoDamage == false
		&& pDamageData->m_fHpPercentUp <= 0.f && pDamageData->m_fIncreaseMP <= 0.f
		&& pDamageData->reActResult != CX2DamageManager::RT_DUMMY_DAMAGE
		&& pDamageData->reActResult != CX2DamageManager::RT_SENSOR && pDamageData->hitType != CX2DamageManager::HT_GROUND_HIT
		&& GetHold() == false && GetGrap() == false
		&& false == GetRidingOn()
		)
	{
		if( true != IsSuperArmor() && true == GetFrameData().unitCondition.bFootOnLine )
		{

			if( true == m_FrameDataFuture.unitCondition.bAttackerFront )
			{
				if(  GetRandomFloat() < m_fAutoGuardRate )
					m_bAutoGuardBlocking = true;
			}
			else
			{
				m_bIsGuardMotion = false;
			}


			if( true == m_bIsGuardMotion )
			{
				int GuardDamageReducePercent = 0;
				if( NULL != m_GuardForElswordPtr )
					GuardDamageReducePercent = m_GuardForElswordPtr->GetDamageReducePercent();

				SetDamageReducePercent( GetDamageReducePercent() + GuardDamageReducePercent );
				CX2GUUser::DamageReact( pDamageData );
				SetDamageReducePercent( GetDamageReducePercent() - GuardDamageReducePercent );

				StateChange( CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_DAMAGE, true );

				return;
			}

		}
	}

	if( GetWayOfSwordState() == WSS_DESTRUCTION &&
		IsSuperArmor() == false &&
		m_fDestructionRageTime <= 0.f &&
		pDamageData->reActType != CX2DamageManager::RT_NO_DAMAGE &&
		pDamageData->reActType != CX2DamageManager::RT_DUMMY_DAMAGE &&
		pDamageData->reActType != CX2DamageManager::RT_SENSOR &&
		GetInvincible() == false &&
		GetNowHp() > 0.f &&
		m_FrameDataNow.unitCondition.bFootOnLine == true )
	{
		const int DAMAGE_REDUCE_PERCENT_DESTRUCTION_RAGE = 100;
		SetDamageReducePercent( GetDamageReducePercent() + DAMAGE_REDUCE_PERCENT_DESTRUCTION_RAGE );
		CX2GUUser::DamageReact( pDamageData );
		SetDamageReducePercent( GetDamageReducePercent() - DAMAGE_REDUCE_PERCENT_DESTRUCTION_RAGE );

		if(	pDamageData->reActResult == CX2DamageManager::RT_REVENGE ||
			pDamageData->reActResult == CX2DamageManager::RT_REFLECT )
		{
			DamageReactStateChange( pDamageData, ESSI_REVENGE_ATTACK );
		}
		else
		{
			SetHoldStateID( USI_DAMAGE_AIR_FALL );
			DoStateDestructionRage();
		}
	}
	else if( IsSuperArmor() )
	{
		SetDamageReducePercent( GetDamageReducePercent() );
		CX2GUUser::DamageReact( pDamageData );
		SetDamageReducePercent( GetDamageReducePercent() );

		DamageReactStateChange( pDamageData, ESSI_REVENGE_ATTACK );
	}
	else
	{
		CX2GUUser::DamageReact( pDamageData );
		DamageReactStateChange( pDamageData, ESSI_REVENGE_ATTACK );
	}

	if(	IsMyUnit() == true &&
		pDamageData->reActType != CX2DamageManager::RT_NO_DAMAGE &&
		pDamageData->reActType != CX2DamageManager::RT_DUMMY_DAMAGE &&
		pDamageData->reActType != CX2DamageManager::RT_SENSOR &&
		pDamageData->reActResult != CX2DamageManager::RT_REVENGE &&
		GetInvincible() == false &&
		GetNowHp() > 0.f )
	{
		int iWayOfSwordState = GetWayOfSwordState();
		float fWSPGain = 0.f;

		fWSPGain = g_pData->GetDamageManager()->CalcWayOfSwordPointOnDamageReact( pDamageData->damage.GetAllDamage(), iWayOfSwordState );

		SetWayOfSwordPoint( GetWayOfSwordPoint() + fWSPGain );
	}


}

/*virtual*/ void CX2GUElsword_SwordMan::DamageReactStateChange( CX2DamageManager::DamageData* pDamageData, int iRevengeAttack/* = -1*/, int iAvoidance/* = -1 */)
{
	if( true == m_bAutoGuardBlocking )
	{
		m_bAutoGuardBlocking = false;

		StateChange( CX2GUElsword_SwordMan::ESSI_SI_P_ESK_AUTO_GUARD , true );

		return;
	}


	CX2GUUser::DamageReactStateChange( pDamageData,  iRevengeAttack, iAvoidance );
}

/*virtual*/ float CX2GUElsword_SwordMan::GetFinalDamageToBeChangedByActiveSkill( float fFinalDamage_ )
{

	return fFinalDamage_ * m_fActiveSkillAddPowerRate;
}

void CX2GUElsword_SwordMan::ApplyBuffToHarshSlayer( const CX2DamageManager::DamageData& damageData_ )
{
	if( CX2DamageManager::AT_NORMAL == damageData_.attackType )
	{
		const int iSkillLevelHarshSlayer = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ETK_HARSH_SLAYER, true );

		if( iSkillLevelHarshSlayer > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ETK_HARSH_SLAYER );
			if( NULL != pSkillTemplet )
			{
				const float SkillRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelHarshSlayer );
				const float RandomRate = GetRandomFloat();
				if( SkillRate > RandomRate )
				{
					if( pSkillTemplet->m_vecBuffFactorPtr.size() > 2 )
					{
						SetBuffFactorToGameUnit( pSkillTemplet, 1 );
						SetBuffFactorToGameUnit( pSkillTemplet, 2 );
						CX2GameUnit*	pDefenderUnit = NULL;
						if( null != damageData_.optrDefenderGameUnit )
						{
							pDefenderUnit = damageData_.optrDefenderGameUnit.GetObservable();
							if( NULL != pDefenderUnit )
							{
								pDefenderUnit->SetBuffFactorToGameUnit( *(pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillLevelHarshSlayer )) );

								//�ºλ��� ������ ����� ��츦 üũ ����� ��.
								const int iSkillLevelMindOfFighter = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_MIND_OF_FIGHTER, true );

								if( iSkillLevelMindOfFighter > 0 )
								{
									pDefenderUnit->SetBuffFactorToGameUnitByBuffFactorID(static_cast<UINT>( BFI_DEBUFF_SI_P_EIS_MIND_OF_FIGHTER ), iSkillLevelMindOfFighter );
								}
							}
						}
					}
				}
			}
		}
	}
}

void CX2GUElsword_SwordMan::Win()
{
	CX2GUUser::Win();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}

void CX2GUElsword_SwordMan::Lose()
{
	CX2GUUser::Lose();

	if( IsMyUnit() == true &&
		GetNowHp() > 0.f )
	{
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			StateChange( USI_RIDING_WAIT );
		else
			StateChange( USI_LOSE );
	}
}

void CX2GUElsword_SwordMan::Draw()
{
	CX2GUUser::Draw();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}


void CX2GUElsword_SwordMan::InitStateID()
{
	m_MachineGunWaitState	= ESSI_MACHINE_GUN_WAIT;
	m_ChargeMpState			= USI_WAIT;

	m_CommonState.m_Wait			= USI_WAIT;
	m_CommonState.m_Thrown			= USI_DAMAGE_AIR_FLY_FRONT;



//#ifdef TRANSFORMER_TEST
//	m_CommonState.m_Transformed			= ESSI_TRANSFORMED;
//#endif TRANSFORMER_TEST




}


void CX2GUElsword_SwordMan::InitState()
{
	ASSERT( NULL != GetUnit() );

	const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();


	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
	case CX2Unit::UC_ELSWORD_KNIGHT:
	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
		{
			m_iSkillCutInSetIndex = 0;
		} break;
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		{
			m_iSkillCutInSetIndex = 1;
		} break;
	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		{
			m_iSkillCutInSetIndex = 2;
		} break;
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			m_iSkillCutInSetIndex = 3;
		} break;
	default:
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
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WIN;
	m_LuaManager.MakeTableReference( tableNameWinUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, WinStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, WinFrameMove );
	//stateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, WinCameraMove );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_LOSE;
	m_LuaManager.MakeTableReference( tableNameLoseUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, LoseStateStart );
	m_StateList[stateData.stateID] = stateData;



	// ���� ����Ʈ win��� ���� ó��
	if( 0 == tableNameWinUTF8.compare( "ESSI_EMK_WIN2" ) )
	{
		m_bIsMagicKnightWinMotion2 = true;
	}

	// ���� ���� �޺� ���� variation�� �ִ� �Ϳ� ���ؼ� switch ���� ���� ������
	// COMBO_ZZ �迭
	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_ZZZ;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_ZZZ", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZ_StateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZ_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZ_StateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_ZZZZ;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_ZZZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZ_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZ_StateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_ZZZZZ;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_ZZZZZ", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZZ_StateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZZ_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_ZZZZZ_StateEnd );
			m_StateList[stateData.stateID] = stateData;

		} break;
	default:
		break;
	}

	// COMBO_XX �迭
	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_XX;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_XX", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XX_StateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XX_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XX_StateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_XXX_LOOP;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_XXX_LOOP", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_LOOP_StateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_LOOP_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_LOOP_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_LOOP_StateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_XXXZ;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_XXXZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXXZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXXZ_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXXZ_StateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_ETK_COMBO_XXX_END;
			m_LuaManager.MakeTableReference( "ESSI_ETK_COMBO_XXX_END", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_END_StateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_END_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_END_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_COMBO_XXX_END_StateEnd );
			m_StateList[stateData.stateID] = stateData;

		} break;
	default:
		break;
	}




	// dash_combo_zz �迭
	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z2;
			m_LuaManager.MakeTableReference( "ESSI_DASH_COMBO_Z2", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashComboZ2StartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashComboZ2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashComboZ2EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ELSWORD_KNIGHT:
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z2;
			m_LuaManager.MakeTableReference( "ESSI_KNIGHT_DASH_COMBO_Z2", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z2_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z2_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z3;
			m_LuaManager.MakeTableReference( "ESSI_KNIGHT_DASH_COMBO_Z3", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z3_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z3_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z3a;
			m_LuaManager.MakeTableReference( "ESSI_KNIGHT_DASH_COMBO_Z3a", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z3a_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z3a_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;


	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z2;
			m_LuaManager.MakeTableReference( "ESSI_KNIGHT_DASH_COMBO_Z2", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z2_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_COMBO_Z2_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z3;
			m_LuaManager.MakeTableReference( "ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_DASH_COMBO_Z3a;
			m_LuaManager.MakeTableReference( "ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_EventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_StateEndFuture );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			stateData.Init();
			stateData.stateID			= ESSI_ETK_DASH_COMBO_ZZ;
			m_LuaManager.MakeTableReference( "ESSI_ETK_DASH_COMBO_ZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_DASH_COMBO_ZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_DASH_COMBO_ZZ_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ETK_DASH_COMBO_ZZ_StateEnd );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}




	// dash_jump_combo_XX �迭
	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ELSWORD_SWORDMAN:
	case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_JUMP_COMBO_X2;
			m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_X2", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2End );
			m_StateList[stateData.stateID] = stateData;


		} break;


	case CX2Unit::UC_ELSWORD_KNIGHT:
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_JUMP_COMBO_X2;
			m_LuaManager.MakeTableReference( "ESSI_KNIGHT_DASH_JUMP_COMBO_X2", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_JUMP_COMBO_X2_FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_JUMP_COMBO_X2_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_JUMP_COMBO_X2_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_JUMP_COMBO_X2_End );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESSI_DASH_JUMP_COMBO_X3;
			m_LuaManager.MakeTableReference( "ESSI_KNIGHT_DASH_JUMP_COMBO_X3", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_KNIGHT_DASH_JUMP_COMBO_X3_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;

	case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_JUMP_COMBO_X2;
			m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_X2", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2End );
			m_StateList[stateData.stateID] = stateData;




			stateData.Init();
			stateData.stateID			= ESSI_DASH_JUMP_COMBO_X2a;
			m_LuaManager.MakeTableReference( "ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			stateData.Init();
			stateData.stateID			= ESSI_DASH_JUMP_COMBO_X2;
			m_LuaManager.MakeTableReference( "ESSI_EIS_DASHJUMP_COMBO_XX", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_EIS_DASHJUMP_COMBO_XX_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2End );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_EIS_DASHJUMP_COMBO_XXX;
			m_LuaManager.MakeTableReference( "ESSI_EIS_DASHJUMP_COMBO_XXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_EIS_DASHJUMP_COMBO_XXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_EIS_DASHJUMP_COMBO_XXX_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_EIS_DASHJUMP_COMBO_XXX_End );
			m_StateList[stateData.stateID] = stateData;
		}
		break;
	default:
		break;
	}












	// �ϴ� 2���������� �ִ� �޺��� �䷸�� �۾�
	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
		{
			stateData.Init();
			stateData.stateID			= ESSI_ELK_ZZZfrontZ;
			m_LuaManager.MakeTableReference( "ESSI_ELK_ZZZfrontZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ELK_ZZZfrontZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ELK_ZZZfrontZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESSI_ELK_XXX;
			m_LuaManager.MakeTableReference( "ESSI_ELK_XXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ELK_XXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ELK_XXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESSI_ELK_XXXX;
			m_LuaManager.MakeTableReference( "ESSI_ELK_XXXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ELK_XXXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ELK_XXXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;


	case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
		{

			stateData.Init();
			stateData.stateID			= ESSI_ERS_XZ;
			m_LuaManager.MakeTableReference( "ESSI_ERS_XZ", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_XZ_StateStartFuture );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_XZ_FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_XZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_XZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESSI_ERS_XZZ;
			m_LuaManager.MakeTableReference( "ESSI_ERS_XZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_XZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_XZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;



			stateData.Init();
			stateData.stateID			= ESSI_ERS_DASH_JUMP_XZ;
			m_LuaManager.MakeTableReference( "ESSI_ERS_DASH_JUMP_XZ", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZ_StateStartFuture );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZ_StateEndFuture );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZ_FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESSI_ERS_DASH_JUMP_XZZ;
			m_LuaManager.MakeTableReference( "ESSI_ERS_DASH_JUMP_XZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_ERS_DASH_JUMP_XZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
		{
			stateData.Init();
			stateData.stateID			= ESSI_EIS_DASH_COMBO_XX;
			m_LuaManager.MakeTableReference( "ESSI_EIS_DASH_COMBO_XX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_EIS_DASH_COMBO_XX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_EIS_DASH_COMBO_XXX;
			m_LuaManager.MakeTableReference( "ESSI_EIS_DASH_COMBO_XXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_EIS_DASH_COMBO_XXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
		break;

	default:
		break;
	}




	InitPassiveSkillState();
	InitEquippedActiveSkillState();


	CX2GUUser::InitState();
}

void CX2GUElsword_SwordMan::InitStateCommon()
{

	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "ESSI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DieBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DieBackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DieBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DieBackEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "ESSI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "ESSI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, WaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, WaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, WaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "ESSI_WALK", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, WalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "ESSI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESSI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser,			JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser,			JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_JUMP_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "ESSI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "ESSI_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashEndStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashEndStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_POWER_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_POWER_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpPowerLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpPowerLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpPowerLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ESSI_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageSmallFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageSmallBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageBigFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageBigFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageBigBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageBigBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageDownFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageDownFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageDownBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageDownBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DamageAirDownLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "ESSI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ROLLING_FRONT_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ROLLING_FRONT_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingFrontFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingFrontFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ROLLING_FRONT_BACK;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ROLLING_FRONT_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingFrontBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingFrontBackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ROLLING_BACK_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ROLLING_BACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingBackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingBackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ROLLING_BACK_BACK;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ROLLING_BACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingBackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpRollingBackBackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackFrontEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackFrontEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackBackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackBackEnd );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ATTACK_FRONT_NO_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ATTACK_FRONT_NO_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackFrontNoDownEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackFrontNoDownEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_STANDUP_ATTACK_BACK_NO_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_STANDUP_ATTACK_BACK_NO_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackBackNoDownEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, StandUpAttackBackNoDownEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_SI_P_ESK_AUTO_GUARD;
	m_LuaManager.MakeTableReference( "ESSI_SI_P_ESK_AUTO_GUARD", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_P_ESK_AUTO_GUARD_Start );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_P_ESK_AUTO_GUARD_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_P_ESK_AUTO_GUARD_End );
	m_StateList[stateData.stateID] = stateData;


	int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER, true );
	if( iSkillLevel > 0 )
	{
		stateData.Init();
		stateData.stateID			= ESSI_REVENGE_ATTACK;
		m_LuaManager.MakeTableReference( "ESSI_SI_A_ETK_COMPACT_COUNTER_GUARD_ATTACK", stateData.stateID );
		stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackStart );
		stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackStartFuture );
		stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackFrameMoveFuture );
		stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackFrameMove );
		stateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackCameraMove );
		stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackEventProcess );
		stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, CompactCounterGuardAttackStateEnd );
		m_StateList[stateData.stateID] = stateData;
	}
	else
	{
		stateData.Init();
		stateData.stateID			= ESSI_REVENGE_ATTACK;
		m_LuaManager.MakeTableReference( "ESSI_REVENGE_ATTACK", stateData.stateID );
		stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, RevengeAttackStart );
		stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, RevengeAttackFrameMoveFuture );
		stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, RevengeAttackFrameMove );
		stateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, RevengeAttackCameraMove );
		stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, RevengeAttackEventProcess );
		m_StateList[stateData.stateID] = stateData;
	}

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_Z1;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_Z1", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ1FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ1EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_Z2;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_Z2", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ2FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ2EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_Z3;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_Z3", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ3FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ3EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_Z4;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_Z4", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ4FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ4FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ4EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_Z4a;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_Z4a", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ4aFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboZ4aEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_X1;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_X1", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX1FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX1EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_X2;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_X2", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX2FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX2FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX2EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_X3;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_X3", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX3FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX3FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX3EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_COMBO_X3a;
	m_LuaManager.MakeTableReference( "ESSI_COMBO_X3a", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX3aFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ComboX3aEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_DASH_COMBO_Z1;
	m_LuaManager.MakeTableReference( "ESSI_DASH_COMBO_Z1", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashComboZ1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashComboZ1EventProcess );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ESSI_DASH_ATTACK_X;
	m_LuaManager.MakeTableReference( "ESSI_DASH_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashAttackXStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashAttackXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ESSI_JUMP_ATTACK_Z", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpAttackZFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpAttackZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_JUMP_ATTACK_X;
	m_LuaManager.MakeTableReference( "ESSI_JUMP_ATTACK_X", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpAttackXFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpAttackXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, JumpAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_COMBO_Z1;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_Z1", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ1EventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_COMBO_Z2;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_Z2", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ2FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ2EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_COMBO_Z3;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_Z3", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ3FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ3EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_COMBO_Z3_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_Z3_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ3LandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ3LandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboZ3LandingEventProcess );
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_COMBO_X1;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_X1", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX1EventProcess );
	m_StateList[stateData.stateID] = stateData;






	stateData.Init();
	stateData.stateID			= ESSI_DASH_JUMP_COMBO_X2_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_DASH_JUMP_COMBO_X2_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2LandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2LandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DashJumpComboX2LandingEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESSI_SWORD_WAIT;
	m_LuaManager.MakeTableReference( "ESSI_SWORD_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, SwordWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, SwordWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, SwordWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESSI_DESTRUCTION_RAGE;
	m_LuaManager.MakeTableReference( "ESSI_DESTRUCTION_RAGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DestructionRageStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DestructionRageStateFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DestructionRageStateEventProcess );
	m_StateList[stateData.stateID] = stateData;




	stateData.Init();
	stateData.stateID			= ESSI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_WALL_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
	m_StateList[stateData.stateID] = stateData;




	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "ESSI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "ESSI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "ESSI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESSI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "ESSI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;

	//#pragma region ESSI_RIDING_START
	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "ESSI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_ON
	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_OFF
	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_WAIT_HABIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_WAIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_WALK
	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DASH
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DASH_END
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DASH_JUMP
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DASH_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DAMAGE_FRONT
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DAMAGE_BACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_DIE
	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_JUMP_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_ATTACK_X
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_SPECIAL_ATTACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ESSI_RIDING_SPECIAL_MOVE
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "ESSI_RIDING_SPECIAL_MOVE", stateData.stateID );
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
	m_LuaManager.MakeTableReference( "ESSI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingHyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, RidingHyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_RIDE;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_RIDE", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.StateStartFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_RIDE_StartFuture );
	stateData.StateStart			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_RIDE_Start );
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_RIDE_FrameMoveFuture );
	stateData.OnFrameMove			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_RIDE_FrameMove );
	stateData.OnCameraMove			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_RIDE_CameraMove );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_RIDE_EventProcess );
#else
	stateData.StateStartFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_RIDE_StartFuture );
	stateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_RIDE_Start );
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_RIDE_FrameMoveFuture );
	stateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_RIDE_FrameMove );
	stateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_RIDE_CameraMove );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_RIDE_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_WAIT;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_WAIT", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.StateStartFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_WAIT_StartFuture );
	stateData.StateStart			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_WAIT_Start );
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_WAIT_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_WAIT_EventProcess );
#else
	stateData.StateStartFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_WAIT_StartFuture );
	stateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_WAIT_Start );
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_WAIT_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_WAIT_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_WALK;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_WALK", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_WALK_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_WALK_EventProcess );
#else
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_WALK_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_WALK_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_ATTACK;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_ATTACK", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_ATTACK_FrameMoveFuture );
	stateData.OnFrameMove			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_ATTACK_FrameMove );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_ATTACK_EventProcess );
#else
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_ATTACK_FrameMoveFuture );
	stateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_ATTACK_FrameMove );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_ATTACK_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;



	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_LEAVE;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_LEAVE", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.StateStartFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_LEAVE_StartFuture );
	stateData.StateStart			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_LEAVE_Start );
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_LEAVE_FrameMoveFuture );
	stateData.OnFrameMove			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_LEAVE_FrameMove );
	stateData.OnCameraMove			= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_LEAVE_CameraMove );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_LEAVE_EventProcess );
#else
	stateData.StateStartFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_LEAVE_StartFuture );
	stateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_LEAVE_Start );
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_LEAVE_FrameMoveFuture );
	stateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_LEAVE_FrameMove );
	stateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_LEAVE_CameraMove );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_LEAVE_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_JUMP_UP", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_JUMP_UP_EventProcess );
#else
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_JUMP_UP_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_MACHINE_GUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ESSI_MACHINE_GUN_JUMP_DOWN", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, MACHINE_GUN_JUMP_DOWN_EventProcess );
#else
	stateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, MACHINE_GUN_JUMP_DOWN_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= ESSI_SUMMON_MAGIC_SPEAR;
	m_LuaManager.MakeTableReference( "ESSI_SUMMON_MAGIC_SPEAR", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, SUMMON_MAGIC_SPEAR_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, SUMMON_MAGIC_SPEAR_EventProcess );
#else
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, SUMMON_MAGIC_SPEAR_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, SUMMON_MAGIC_SPEAR_EventProcess );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= ESSI_LEAP;
	m_LuaManager.MakeTableReference( "ESSI_LEAP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_LEAP_StartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_LEAP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_LEAP_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_LEAP_End );
	m_StateList[stateData.stateID] = stateData;









//#ifdef PVP_BOSS_COMBAT_TEST
//
//	stateData.Init();
//	stateData.stateID			= ESSI_FROZEN;
//	m_LuaManager.MakeTableReference( "ESSI_FROZEN", stateData.stateID );
//	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, Frozen_StateStart );
//	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, Frozen_EventProcess );
//	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, Frozen_StateEnd );
//	m_StateList[stateData.stateID] = stateData;
//
//
//	m_FrozenState = ESSI_FROZEN;
//
//#endif PVP_BOSS_COMBAT_TEST
















//#ifdef TRANSFORMER_TEST
//
//	stateData.Init();
//	stateData.stateID			= ESSI_TRANSFORMED;
//	m_LuaManager.MakeTableReference( "ESSI_TRANSFORMED", stateData.stateID );
//	m_StateList[stateData.stateID] = stateData;
//
//
//#endif TRANSFORMER_TEST


}





/*virtual*/ void CX2GUElsword_SwordMan::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();

	m_iSkillLevelInduranceOfRevenge = 0;

	int iSkillLevel = 0;

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_LIGHTNING_STEP, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_LIGHTNING_STEP );

		if( NULL != pSkillTemplet )
		{
			const float fMoveSpeedUp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel );
			m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * fMoveSpeedUp );
			m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * fMoveSpeedUp );
			InsertDelegateProcessToCommonFrameMove( SET_DELEGATE_PROCESS( CX2GUElsword_SwordMan, LightingStepFrameMove ) );
		}
	}


	m_fAttackPowerRateFireBall = 1.f;
	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ES_POWERFUL_FIREBALL, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ES_POWERFUL_FIREBALL );

		if( NULL != pSkillTemplet )
		{
			m_fAttackPowerRateFireBall *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel );
		}
	}

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ELK_STRONG_BONE, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ELK_STRONG_BONE );

		if( NULL != pSkillTemplet )
		{
			const float fDamageRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillLevel );
			m_AdditionalFinalDefenceMultiplier.SetPassiveMultiple( m_AdditionalFinalDefenceMultiplier.GetPassiveMultiple() * fDamageRate );
		}
	}

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ERS_SPELL_CHAIN, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ERS_SPELL_CHAIN );
		if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
			CreateAndInsertSkillAndSocketBuffFactorToList( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillLevel ) );
	}

	//{{ oasis907 : ����� [2010.11.1] // �� �����̾� - �������� ����
	m_fMagicReflectRate = 0.f;

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ERS_MAGIC_RESISTANCE, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ERS_MAGIC_RESISTANCE );
		if( NULL != pSkillTemplet )
		{
			m_fMagicReflectRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_REFLECT_REL, iSkillLevel );
		}
	}
	//}}


	/// ��Ȥ�� �л��ڴ� ����°�?
	const int iSkillLevelBrutalSlayer = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ETK_BRUTAL_SLAYER, true );

	if( iSkillLevelBrutalSlayer > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletBrutalSlayer = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ETK_BRUTAL_SLAYER );
		if ( NULL != pSkillTempletBrutalSlayer && pSkillTempletBrutalSlayer->m_vecBuffFactorPtr.size() > 0 )
		{
			InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUElsword_SwordMan, AddComboToBrutalSlayer ) );
		}
	}

	iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ES_ROLLING );
	if( iSkillLevel > 0 )
	{

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_ES_ROLLING );

		if( NULL != pSkillTemplet )
		{
			m_fRollingHpConsumeRel	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::HP_CONSUME_REL, min( iSkillLevel, 10 ) );
			m_fRollingSpeedRel		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TELEPORT_SPEED_REL, min( iSkillLevel, 10 ) );
		}
	}

	/// �ºλ��� ������ ����°�?
	const int iSkillLevelFighter = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_MIND_OF_FIGHTER, true );

	if ( iSkillLevelFighter > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletMindOfFighter = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_MIND_OF_FIGHTER );
		if ( NULL != pSkillTempletMindOfFighter && !pSkillTempletMindOfFighter->m_vecBuffFactorPtr.empty() )
		{
			m_fMindOfFighterConsumeMpRate = pSkillTempletMindOfFighter->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevelFighter );
		}
	}
	else
		m_fMindOfFighterConsumeMpRate = 0.0f;

	//���� - ��Ƽ�� ��ų������ �� ��ų�� ������� ���� ���忡�� �����
	const int iSkillLevelGuard = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ES_DEFENCE );
	if( iSkillLevelGuard > 0 )
	{
		if( NULL == m_GuardForElswordPtr )
		{
			m_GuardForElswordPtr = CElswordGuardSkill::CreateElswordGuardSkill();
		}

		if( NULL != m_GuardForElswordPtr )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_ES_DEFENCE );

			if( NULL != pSkillTemplet )
			{
				float ReducePercent = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillLevelGuard );
				m_GuardForElswordPtr->SetDamageReducePercent( static_cast<int>( ReducePercent ) );
			}
		}
	}

	// ���� ����
	const int iSkillLevelAutoGuard = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ESK_AUTO_GUARD, true );

	if( iSkillLevelAutoGuard > 0 && ( CX2Unit::UC_ELSWORD_LORD_KNIGHT == GetUnit()->GetClass() || CX2Unit::UC_ELSWORD_KNIGHT == GetUnit()->GetClass() ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ESK_AUTO_GUARD );

		if( NULL != pSkillTemplet )
		{
			m_fAutoGuardRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelAutoGuard );
		}
	}
	else
		m_fAutoGuardRate = 0.f;

	// �� �����͸�
	const int iSkillLevelRuneMastery = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ERS_RUNE_MASTERY, true );

	if( iSkillLevelRuneMastery > 0 && CX2Unit::UC_ELSWORD_RUNE_SLAYER == GetUnit()->GetClass() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ERS_RUNE_MASTERY );

		if( NULL != pSkillTemplet )
		{
			m_fRuneProjectileRangeIncPercent	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevelRuneMastery );
			m_fRuneProjectileSizeIncPercent		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillLevelRuneMastery );
		}
	}
	else
	{
		m_fRuneProjectileRangeIncPercent	= 1.f;
		m_fRuneProjectileSizeIncPercent		= 1.f;
	}

	//��ī�ο� ��
	const int iSkillLevelCuttingSword = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_CUTTING_SWORD, true );

	if( iSkillLevelCuttingSword > 0 && CX2Unit::UC_ELSWORD_INFINITY_SWORD == GetUnit()->GetClass() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_CUTTING_SWORD );

		if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() &&
			pSkillTemplet->m_eType != CX2SkillTree::ST_SPECIAL_ACTIVE && pSkillTemplet->m_eType != CX2SkillTree::ST_HYPER_ACTIVE_SKILL )
			CreateAndInsertSkillAndSocketBuffFactorToList( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillLevelCuttingSword ) );
	}

	//��Ȥ�� �л���
	const int iSkillLevelHarshSlayer = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ETK_HARSH_SLAYER, true );

	if( iSkillLevelHarshSlayer > 0 && ( CX2Unit::UC_ELSWORD_SHEATH_KNIGHT == GetUnit()->GetClass() || CX2Unit::UC_ELSWORD_INFINITY_SWORD == GetUnit()->GetClass() ) )
	{
		InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUElsword_SwordMan, ApplyBuffToHarshSlayer ) );
	}

	//������ �ϰ�
	const int iSkillLevelIntensiveAttack = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ELK_INTENSIVE_ATTACK, true );

	if( iSkillLevelIntensiveAttack > 0 && CX2Unit::UC_ELSWORD_LORD_KNIGHT == GetUnit()->GetClass() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ELK_INTENSIVE_ATTACK );
		if( NULL != pSkillTemplet )
		{
			m_fIncreaseCoolTimeRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevelIntensiveAttack );
			m_fActiveSkillAddPowerRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevelIntensiveAttack );
		}
	}
	else
	{
		m_fIncreaseCoolTimeRel = 1.f;
		m_fActiveSkillAddPowerRate = 1.f;
	}

	m_iSkillLevelInduranceOfRevenge = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ELK_INDURANCE_OF_REVENGE, true );
}

/*virtual*/ void CX2GUElsword_SwordMan::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
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
			m_LuaManager.MakeTableReference( "ESSI_SI_ES_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI_ES_COMMON_BUFF_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, COMMON_BUFF_FrameMove );
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
					m_LuaManager.MakeTableReference( "ESSI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "ESSI_THROW_ITEM", hyperStateData.stateID );
					hyperStateData.m_bHyperState	= true;
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
		case CX2SkillTree::SI_A_ES_MEGASLASH:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI1_ES_MEGA_SLASH", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_EventProcess );


			m_LuaManager.MakeTableReference( "ESSI_SI1_ES_MEGA_SLASH_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_MEGA_SLASH_HYPER_EventProcess );
		} break;
	case CX2SkillTree::SI_A_ELK_IMPACT_SMASH:
		{
			m_LuaManager.MakeTableReference( "ESSI_A_ELK_IMPACT_SMASH", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_IMPACT_SMASH_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_IMPACT_SMASH_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_A_ELK_IMPACT_SMASH", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_A_ES_SWORD_WAVE:
		{
			m_LuaManager.MakeTableReference( "ESSI_A_ES_SWORD_WAVE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ES_SWORD_WAVE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ES_SWORD_WAVE_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_A_ES_SWORD_WAVE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_A_ES_DEFENCE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_DEFENCE_READY", normalStateData.stateID );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_READY_StartFuture );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_READY_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_DEFENCE_READY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID				= ESSI_SI_A_ES_DEFENCE_WAIT;
				m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_DEFENCE_WAIT", stateData.stateID );
				stateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_WAIT_Start );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_WAIT_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_WAIT_EventProcess );
				m_StateList[stateData.stateID]	= stateData;

				stateData.Init();
				stateData.stateID				= ESSI_SI_A_ES_DEFENCE_END;
				m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_DEFENCE_END", stateData.stateID );
				stateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_END_Start );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_END_EventProcess );
				m_StateList[stateData.stateID]	= stateData;

				stateData.Init();
				stateData.stateID				= ESSI_SI_A_ES_DEFENCE_DAMAGE;
				m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_DEFENCE_DAMAGE", stateData.stateID );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_DAMAGE_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_DEFENCE_DAMAGE_EventProcess );
				m_StateList[stateData.stateID]	= stateData;
			}

			if( NULL == m_GuardForElswordPtr )
				m_GuardForElswordPtr = CElswordGuardSkill::CreateElswordGuardSkill();

			if( NULL != m_GuardForElswordPtr )
			{
				m_GuardForElswordPtr->SetSkillSlotID_Guard( static_cast<ELSWORD_SWORDMAN_STATE_ID>( iNormalStateID ) );
				m_GuardForElswordPtr->SetGuardEquippedAtSlotB( bSlotB_ );
			}
		} break;

	case CX2SkillTree::SI_SA_EMK_RISING_WAVE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_EMK_RISING_SLASH_WAVE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EMK_RISING_SLASH_WAVE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EMK_RISING_SLASH_WAVE_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EMK_RISING_SLASH_WAVE_FrameMove );
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_EMK_RISING_SLASH_WAVE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EMK_RISING_SLASH_WAVE_HYPER_FrameMove );
		} break;

	case CX2SkillTree::SI_A_ERS_CRITICAL_SWORD:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ERS_CRITICAL_SWORD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_CRITICAL_SWORD_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_CRITICAL_SWORD_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_CRITICAL_SWORD_FrameMove );
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ERS_CRITICAL_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;

	case CX2SkillTree::SI_A_ERS_RUNE_OF_FIRE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ERS_RUNE_OF_FIRE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_RUNE_OF_FIRE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_RUNE_OF_FIRE_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_RUNE_OF_FIRE_FrameMove );
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ERS_RUNE_OF_FIRE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;

	case CX2SkillTree::SI_A_ERS_RUNE_OF_ICE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ERS_RUNE_OF_ICE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_RUNE_OF_ICE_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_RUNE_OF_ICE_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ERS_RUNE_OF_ICE_FrameMove );
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ERS_RUNE_OF_ICE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
		} break;

	case CX2SkillTree::SI_A_ETK_PIERCING_SWORD:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ETK_PIERCING_SWORD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ETK_PIERCING_SWORD_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ETK_PIERCING_SWORD_EventProcess );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ETK_PIERCING_SWORD_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ETK_PIERCING_SWORD_FrameMove );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ETK_PIERCING_SWORD_End );
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ETK_PIERCING_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		} break;

	case CX2SkillTree::SI_A_EIS_FATAL_SLAP:
		{
		m_LuaManager.MakeTableReference( "ESSI_SI_A_EIS_FATAL_SLAP", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_EIS_FATAL_SLAP_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_EIS_FATAL_SLAP_EventProcess );
			m_LuaManager.MakeTableReference( "ESSI_SI_A_EIS_FATAL_SLAP", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_A_ETK_SWORD_SHIELD:
		{
			m_LuaManager.MakeTableReference( "ESSI_A_ETK_SWORD_SHIELD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ETK_SWORD_SHIELD_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ETK_SWORD_SHIELD_StateStart );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ETK_SWORD_SHIELD_EventProcess );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ETK_SWORD_SHIELD_FrameMove );

			m_LuaManager.MakeTableReference( "ESSI_A_ETK_SWORD_SHIELD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;

		} break;

	case CX2SkillTree::SI_A_ESK_WEAPON_BREAK:
		{
			m_LuaManager.MakeTableReference( "ESSI_A_ESK_WEAPON_BREAK", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_WEAPON_BREAK_Init );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_WEAPON_BREAK_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_A_ESK_WEAPON_BREAK", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= normalStateData.StateInit;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_A_ESK_AIR_SLASH:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO3 ) == true )
			{
				m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_MEMO", normalStateData.stateID );
			}
			else
			{
				m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH", normalStateData.stateID );
			}
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_EventProcess );

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO3 ) == true )
			{
				m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_MEMO_HYPER", hyperStateData.stateID );
			}
			else
			{
				m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_HYPER", hyperStateData.stateID );
			}
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_HYPER_EventProcess );


			// note!! �Ʒ� �ڵ�� ������� �ʻ���� ����� 2���� �������� �־, ���������� ���¸� �߰����ִ� �ڵ��̴�
			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= ESSI_SA_AIR_SLASH_SECOND;
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO3 ) == true )
				{
					m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_MEMO_NEXT", stateData.stateID );
				}
				else
				{
					m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_NEXT", stateData.stateID );
				}
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_NEXT_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_NEXT_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESSI_SA_AIR_SLASH_SECOND_HYPER;
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO3 ) == true )
				{
					m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_MEMO_HYPER_NEXT", stateData.stateID );
				}
				else
				{
					m_LuaManager.MakeTableReference( "ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT", stateData.stateID );
				}
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

		case CX2SkillTree::SI_A_ES_ASSAULT_SLASH:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI1_ES_ASSAULT_SLASH", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_Start );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI1_ES_ASSAULT_SLASH_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_Start );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_ASSAULT_SLASH_HYPER_EventProcess );
		} break;

	case CX2SkillTree::SI_A_EMK_WIND_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI1_ES_WIND_BLADE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_WIND_BLADE_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_WIND_BLADE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_WIND_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI1_ES_WIND_BLADE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_WIND_BLADE_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI1_ES_WIND_BLADE_HYPER_EventProcess );
		}
		break;

	case CX2SkillTree::SI_SA_ES_FLAME_GEYSER:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI2_ES_FLAME_GEYSER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_Start );
			//normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI2_ES_FLAME_GEYSER_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_HYPER_Start );
			//hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FLAME_GEYSER_HYPER_EventProcess );
		}
		break;

	case CX2SkillTree::SI_SA_ES_FATAL_FURY:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI2_ES_FATAL_FURY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI2_ES_FATAL_FURY_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_FATAL_FURY_HYPER_EventProcess );
		}
		break;

	case CX2SkillTree::SI_SA_ESK_SPIRAL_BLAST:
		{
			const char* pszStateName = "ESSI_SI2_ES_SPIRAL_BLAST";
			const char* pszHyperStateName = "ESSI_SI2_ES_SPIRAL_BLAST_HYPER";

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO9 ) == true )
			{
				pszStateName = "ESSI_SI2_ES_SPIRAL_BLAST_MEMO";
				pszHyperStateName = "ESSI_SI2_ES_SPIRAL_BLAST_HYPER_MEMO";
			}

			m_LuaManager.MakeTableReference( pszStateName, normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_EventProcess );

			m_LuaManager.MakeTableReference( pszHyperStateName, hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_SPIRAL_BLAST_HYPER_EventProcess );
		}
		break;

	case CX2SkillTree::SI_SA_EMK_RISING_SLASH:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI2_ES_RISING_SLASH", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_RISING_SLASH_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_RISING_SLASH_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_RISING_SLASH_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI2_ES_RISING_SLASH_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_RISING_SLASH_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI2_ES_RISING_SLASH_HYPER_EventProcess );
		}
		break;
	case CX2SkillTree::SI_SA_ES_UNLIMITED_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_UNLIMITED_BLADE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_EventProcess );


			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_UNLIMITED_BLADE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_Init );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_EventProcess );

		}
		break;

	case CX2SkillTree::SI_SA_ES_TRIPLE_GEYSER:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_TRIPLE_GEYSER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_EventProcess );


			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_TRIPLE_GEYSER_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_ESK_ARMAGEDON_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_ARMAGEDDON_BLADE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_ARMAGEDDON_BLADE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_ARMAGEDDON_BLADE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_ARMAGEDDON_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER_EventProcess );
		} break;

	case CX2SkillTree::SI_SA_ERS_STORM_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_STORM_BLADE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_STORM_BLADE_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_STORM_BLADE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_STORM_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI3_ES_STORM_BLADE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_STORM_BLADE_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI3_ES_STORM_BLADE_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_A_ES_COUNTER:
		{

			int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER, true );

			if( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER );

				if( NULL != pSkillTemplet )
				{
					m_bCompactCounterEnable = true;

					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

					m_fCompactCounterCoolTime = pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel );
				}

				m_LuaManager.MakeTableReference( "ESSI_SI_A_ETK_COMPACT_COUNTER_ATTACK", normalStateData.stateID );
				normalStateData.StateStart			= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackStart);
				normalStateData.StateStartFuture	= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackStartFuture);
				normalStateData.OnFrameMoveFuture	= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackFrameMoveFuture);
				normalStateData.OnFrameMove			= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackFrameMove);
				normalStateData.OnEventProcess		= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackEventProcess);
				normalStateData.StateEnd			= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackStateEnd);

				m_LuaManager.MakeTableReference( "ESSI_SI_A_ETK_COMPACT_COUNTER_ATTACK", hyperStateData.stateID );
				hyperStateData.m_bHyperState		= true;
				hyperStateData.StateStart			= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackStart);
				hyperStateData.StateStartFuture		= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackStartFuture);
				hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackFrameMoveFuture);
				hyperStateData.OnFrameMove			= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackFrameMove);
				hyperStateData.OnEventProcess		= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackEventProcess);
				hyperStateData.StateEnd				= SET_CB_FUNC(CX2GUElsword_SwordMan, CompactCounterAttackStateEnd);
			}
			else
			{
				m_LuaManager.MakeTableReference( "ESSI_REVENGE_WAIT", normalStateData.stateID );
				normalStateData.StateStart			= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitStart);
				normalStateData.OnFrameMoveFuture	= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitFrameMoveFuture);
				normalStateData.OnFrameMove			= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitFrameMove);
				normalStateData.OnEventProcess		= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitEventProcess);

				m_LuaManager.MakeTableReference( "ESSI_REVENGE_WAIT", hyperStateData.stateID );
				hyperStateData.m_bHyperState		= true;
				hyperStateData.StateStart			= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitStart);
				hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitFrameMoveFuture);
				hyperStateData.OnFrameMove			= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitFrameMove);
				hyperStateData.OnEventProcess		= SET_CB_FUNC(CX2GUElsword_SwordMan, RevengeWaitEventProcess);
			}

		} break;

	case CX2SkillTree::SI_A_ES_KICK:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_KICK", normalStateData.stateID );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_KICK_Start );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_KICK_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_KICK", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_KICK_Start );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_KICK_EventProcess );
		} break;


	case CX2SkillTree::SI_SA_ELK_WINDMILL:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_WINDMILL_START", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_StateEnd );



			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_WINDMILL_START_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_START_HYPER_StateEnd );




			UserUnitStateData stateData;
			stateData.Init();
			stateData.stateID			= ESSI_SA_ESK_WINDMILL;
			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_WINDMILL", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_StateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_SA_ESK_WINDMILL_HYPER;
			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_WINDMILL_HYPER", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_HYPER_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_HYPER_EventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_HYPER_StateEnd );

			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ESSI_SA_ESK_WINDMILL_END;
			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_WINDMILL_END", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_END_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_END_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= ESSI_SA_ESK_WINDMILL_END_HYPER;
			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_WINDMILL_END_HYPER", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_END_HYPER_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_WINDMILL_END_HYPER_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;


	case CX2SkillTree::SI_SA_EMK_SWORD_FIRE:
		{

			m_LuaManager.MakeTableReference( "ESSI_SA_EMK_SWORD_FIRE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_EndFuture );

			m_LuaManager.MakeTableReference( "ESSI_SA_EMK_SWORD_FIRE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_HYPER_Init );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_HYPER_StartFuture );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_HYPER_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_HYPER_EndFuture );

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESSI_SA_EMK_SWORD_FIRE_LANDING;
				m_LuaManager.MakeTableReference( "ESSI_SA_EMK_SWORD_FIRE_LANDING", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_LANDING_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_LANDING_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER;
				m_LuaManager.MakeTableReference( "ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_End );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;




	case CX2SkillTree::SI_A_ESK_ARMOR_BREAK:
		{
			m_LuaManager.MakeTableReference( "ESSI_A_ESK_ARMOR_BREAK", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_StateEnd );


			m_LuaManager.MakeTableReference( "ESSI_A_ESK_ARMOR_BREAK_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_ARMOR_BREAK_HYPER_StateEnd );

		} break;

	case CX2SkillTree::SI_A_EMK_BIG_BURST:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_EMK_BIG_BURST", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_End );

			m_LuaManager.MakeTableReference( "ESSI_SA_EMK_BIG_BURST", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_Init );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_EMK_BIG_BURST_End );

		} break;





	case CX2SkillTree::SI_SA_EMK_PHOENIX_TALON:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_EMK_PHOENIX_TALON", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_StateEnd );

			m_LuaManager.MakeTableReference( "ESSI_SA_EMK_PHOENIX_TALON_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_Init );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_StateStart );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_Hyper_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ESK_PHOENIX_TALON_StateEnd );

		} break;
		//{{ ����� : [2009/9/22] //	���� ������
	case CX2SkillTree::SI_SA_ESK_DOUBLE_SLASH:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_DOUBLE_SLASH", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_DOUBLE_SLASH_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_DOUBLE_SLASH_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_DOUBLE_SLASH_StateEnd );

			m_LuaManager.MakeTableReference( "ESSI_SA_ESK_DOUBLE_SLASH_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ESK_DOUBLE_SLASH_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		}
		break;
		//}} ����� : [2009/9/22] //	���� ������






	case CX2SkillTree::SI_A_ELK_SONIC_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_ELK_SONIC_BLADE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_StateEnd );

			m_LuaManager.MakeTableReference( "ESSI_SA_ELK_SONIC_BLADE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_HYPER_StateStart );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_HYPER_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SONIC_BLADE_HYPER_StateEnd );

		} break;



	case CX2SkillTree::SI_A_ERS_SPLASH_EXPLOSION:
		{

			m_LuaManager.MakeTableReference( "ESSI_SA_ERS_SPLASH_EXPLOSION", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_FrameMove );
#ifdef CONVERSION_VS
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
#else
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, GenericActiveSkillEventProcess );
#endif // CONVERSION_VS
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_StateEnd );

			m_LuaManager.MakeTableReference( "ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_StateStart );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_FrameMove );
#ifdef CONVERSION_VS
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
#else
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, GenericActiveSkillEventProcess );
#endif // CONVERSION_VS
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_StateEnd );

		} break;







	case CX2SkillTree::SI_A_ELK_ROLLING_SMASH:
		{

			m_LuaManager.MakeTableReference( "ESSI_A_ELK_ROLLING_SMASH_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_StateEnd );

			m_LuaManager.MakeTableReference( "ESSI_A_ELK_ROLLING_SMASH_READY_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_StateStart );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_StateEnd );




			{
				UserUnitStateData stateData;


				stateData.Init();
				stateData.stateID			= ESSI_A_ELK_ROLLING_SMASH;
				m_LuaManager.MakeTableReference( "ESSI_A_ELK_ROLLING_SMASH", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_StateStartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESSI_A_ELK_ROLLING_SMASH_HYPER;
				m_LuaManager.MakeTableReference( "ESSI_A_ELK_ROLLING_SMASH_HYPER", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_HYPER_StateStartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ELK_ROLLING_SMASH_HYPER_EventProcess );
				m_StateList[stateData.stateID] = stateData;





				stateData.Init();
				stateData.stateID			= ESSI_A_ELK_ROLLING_SMASH_LANDING;
				m_LuaManager.MakeTableReference( "ESSI_A_ELK_ROLLING_SMASH_LANDING", stateData.stateID );
#ifdef CONVERSION_VS
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
#else
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, GenericSpecialActiveSkillEventProcess );
#endif // CONVERSION_VS
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ESSI_A_ELK_ROLLING_SMASH_LANDING_HYPER;
				m_LuaManager.MakeTableReference( "ESSI_A_ELK_ROLLING_SMASH_LANDING_HYPER", stateData.stateID );
#ifdef CONVERSION_VS
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
#else
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, GenericSpecialActiveSkillEventProcess );
#endif // CONVERSION_VS
				m_StateList[stateData.stateID] = stateData;
			}




		} break;



	case CX2SkillTree::SI_A_ERS_SWORD_ENCHANT:
		{

			m_LuaManager.MakeTableReference( "ESSI_A_ERS_SWORD_ENCHANT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ERS_SWORD_ENCHANT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ERS_SWORD_ENCHANT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_A_ERS_SWORD_ENCHANT_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_A_ERS_SWORD_ENCHANT_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;





		//{{ oasis907 : ����� [2010.11.5] // �ε� ����Ʈ - ���� ����
	case CX2SkillTree::SI_SA_ELK_SAND_STORM:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_ELK_SAND_STORM", normalStateData.stateID );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SAND_STORM_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SAND_STORM_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ELK_SAND_STORM_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SA_ELK_SAND_STORM_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

		} break;
		//}} oasis907 : ����� [2010.11.5] // �ε� ����Ʈ - ���� ����

		//{{ JHKang / ������ / 2010/11/02 / �糪 �����̵� �߰�
	case CX2SkillTree::SI_SA_ERS_LUNA_BLADE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SA_ERS_LUNA_BLADE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_LUNA_BLADE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_LUNA_BLADE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SA_ERS_LUNA_BLADE_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SA_ERS_LUNA_BLADE_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
		//}} JHKang / ������ / 2010/11/02 / �糪 �����̵� �߰�

	case CX2SkillTree::SI_A_ES_ROLLING:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_ROLLING", normalStateData.stateID );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_StateStart );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_StateStartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_ROLLING", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_StateStartFuture );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_EventProcess );
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESSI_SI_A_ES_ROLLING_END;
				m_LuaManager.MakeTableReference( "ESSI_SI_A_ES_ROLLING_END", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_END_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_ES_ROLLING_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	case CX2SkillTree::SI_A_ETK_HARSH_CHASER:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_HARSH_CHASER", normalStateData.stateID );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_End );

			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_HARSH_CHASER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_End );
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ESSI_SI_SA_ETK_HARSH_CHASER_PULL;
				m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_HARSH_CHASER_PULL", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_PULL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_HARSH_CHASER_PULL_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	case CX2SkillTree::SI_SA_EIS_MAELSTORM_RAGE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_MAELSTORM_RAGE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_EndFuture );

			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_MAELSTORM_RAGE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_Init );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_StartFuture );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_EventProcess );
			hyperStateData.StateEndFuture		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_MAELSTORM_RAGE_EndFuture );
		} break;
	case CX2SkillTree::SI_SA_ETK_FINAL_STRIKE:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_FINAL_STRIKE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_End );

			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_FINAL_STRIKE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_FINAL_STRIKE_End );
		} break;

	case CX2SkillTree::SI_SA_EIS_SWORD_FALL:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_SWORD_FALL", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_End );

			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_SWORD_FALL", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_SWORD_FALL_End );

		} break;

	case CX2SkillTree::SI_A_ETK_CRESCENT_CUT:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_CRESCENT_CUT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_End );

			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_CRESCENT_CUT", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_CRESCENT_CUT_End );
		} break;

	case CX2SkillTree::SI_SA_ETK_PHANTOM_SWORD:
		{
			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_PHANTOM_SWORD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_PHANTOM_SWORD_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_PHANTOM_SWORD_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_PHANTOM_SWORD_EventProcess );

			m_LuaManager.MakeTableReference( "ESSI_SI_SA_ETK_PHANTOM_SWORD", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_PHANTOM_SWORD_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_PHANTOM_SWORD_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_ETK_PHANTOM_SWORD_EventProcess );
		} break;


		case CX2SkillTree::SI_A_EIS_MIRAGE_STING:
			{
				m_LuaManager.MakeTableReference( "ESSI_SI_A_EIS_SPIRAL_STING", normalStateData.stateID );
				normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_EIS_SPIRAL_STING_FrameMove );
				normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_EIS_SPIRAL_STING_EventProcess );

				m_LuaManager.MakeTableReference( "ESSI_SI_A_EIS_SPIRAL_STING", hyperStateData.stateID );
				hyperStateData.m_bHyperState		= true;
				hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_EIS_SPIRAL_STING_FrameMove );
				hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_A_EIS_SPIRAL_STING_EventProcess );
			} break;
		case CX2SkillTree::SI_A_EIS_SWORD_BLASTING:
			{
				m_LuaManager.MakeTableReference( "ESSI_SI_SA_EIS_SWORD_BLASTING", normalStateData.stateID );
				normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_SWORD_BLASTING_FrameMove );
				normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_SWORD_BLASTING_EventProcess );

				m_LuaManager.MakeTableReference( "ESSI_SI_SA_EIS_SWORD_BLASTING", hyperStateData.stateID );
				hyperStateData.m_bHyperState		= true;
				hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_SWORD_BLASTING_FrameMove );
				hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_SWORD_BLASTING_EventProcess );
			} break;
		case CX2SkillTree::SI_SA_EIS_RAGE_CUTTER:
			{
				m_LuaManager.MakeTableReference( "ESSI_SI_SA_EIS_RAGE_CUTTER", normalStateData.stateID );
				normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_RAGE_CUTTER_FrameMove );
				normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_RAGE_CUTTER_EventProcess );

				m_LuaManager.MakeTableReference( "ESSI_SI_SA_EIS_RAGE_CUTTER", hyperStateData.stateID );
				hyperStateData.m_bHyperState		= true;
				hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_RAGE_CUTTER_FrameMove );
				hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_SI_SA_EIS_RAGE_CUTTER_EventProcess );
			} break;


		#pragma	region
		case CX2SkillTree::SI_HA_ELK_GIGANTIC_SLASH:
			{
				m_LuaManager.MakeTableReference( "ESSI_HA_ELK_GIGANTIC_SLASH", normalStateData.stateID );
				normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_ELK_Gigantic_Slash_Init );
				normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );

				m_LuaManager.MakeTableReference( "ESSI_HA_ELK_GIGANTIC_SLASH", hyperStateData.stateID );
				hyperStateData.m_bHyperState	= true;
				hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
			}
			break;
		//#pragma endregion �Ⱓƽ ������ : �ñر�

		//#pragma region SI_FS_ERS_Shining_Rune_Buster
		case CX2SkillTree::SI_HA_ERS_SHINING_RUNE_BUSTER:
			{
				m_LuaManager.MakeTableReference( "ESSI_HA_ERS_SHINING_RUNE_BUSTER", normalStateData.stateID );
				normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_ERS_Shining_Rune_Buster_Init );
				normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_ERS_Shining_Rune_Buster_EventProcess );



				m_LuaManager.MakeTableReference( "ESSI_HA_ERS_SHINING_RUNE_BUSTER", hyperStateData.stateID );
				hyperStateData.m_bHyperState	= true;
				hyperStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_ERS_Shining_Rune_Buster_Init );
				hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_ERS_Shining_Rune_Buster_EventProcess );
			}
			break;
		//#pragma endregion ���̴� �� ����Ʈ - �ñر�

		//#pragma region SI_FS_EIS_Blade_Rain
		case CX2SkillTree::SI_HA_EIS_BLADE_RAIN:
			{
				m_LuaManager.MakeTableReference( "ESSI_HA_EIS_BLADE_RAIN", normalStateData.stateID );
				normalStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_EIS_Blade_Rain_Init );
				normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_EIS_Blade_Rain_EventProcess );

				m_LuaManager.MakeTableReference( "ESSI_HA_EIS_BLADE_RAIN", hyperStateData.stateID );
				hyperStateData.m_bHyperState	= true;
				hyperStateData.StateInit		= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_EIS_Blade_Rain_Init );
				hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUElsword_SwordMan, ESSI_HA_EIS_Blade_Rain_EventProcess );

		}
			break;
		//#pragma endregion �����̵� ���� - �ñر�
	}
}

/*virtual*/ void CX2GUElsword_SwordMan::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ESK_AIR_SLASH == eSkillID_ )
	{
		// note!! �Ʒ��� �ڵ�� ������ø� ���� ����ó�� �ڵ� �Դϴ�
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ESK_AIR_SLASH );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			UserUnitStateData& normalStateData = m_StateList[ ESSI_SA_AIR_SLASH_SECOND ];
			normalStateData.m_SPLevel = iSkillTempletLevel;
			normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			normalStateData.m_eSkillID = pSkillTemplet->m_eID;

			UserUnitStateData& hyperStateData = m_StateList[ ESSI_SA_AIR_SLASH_SECOND_HYPER ];
			hyperStateData.m_SPLevel = iSkillTempletLevel;
			hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ELK_WINDMILL == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ELK_WINDMILL );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SA_ESK_WINDMILL ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ESSI_SA_ESK_WINDMILL_HYPER ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SA_ESK_WINDMILL_END ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ESSI_SA_ESK_WINDMILL_END_HYPER ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}

			m_fWindMillDuration = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_EMK_SWORD_FIRE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EMK_SWORD_FIRE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SA_EMK_SWORD_FIRE_LANDING ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

				UserUnitStateData& hyperStateData = m_StateList[ ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;

			}
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ELK_ROLLING_SMASH == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ELK_ROLLING_SMASH );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_A_ELK_ROLLING_SMASH ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;


				UserUnitStateData& hyperStateData = m_StateList[ ESSI_A_ELK_ROLLING_SMASH_HYPER ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_A_ELK_ROLLING_SMASH_LANDING ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;


				UserUnitStateData& hyperStateData = m_StateList[ ESSI_A_ELK_ROLLING_SMASH_LANDING_HYPER ];
				hyperStateData.m_SPLevel = iSkillTempletLevel;
				hyperStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				hyperStateData.m_eSkillID = pSkillTemplet->m_eID;
			}
		}
	}

	if( true == bChangeAll_ || CX2SkillTree::SI_A_ES_DEFENCE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ES_DEFENCE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SI_A_ES_DEFENCE_READY ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SI_A_ES_DEFENCE_WAIT ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SI_A_ES_DEFENCE_END ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;

			}

			{
				UserUnitStateData& normalStateData = m_StateList[ ESSI_SI_A_ES_DEFENCE_DAMAGE ];
				normalStateData.m_SPLevel = iSkillTempletLevel;
				normalStateData.m_fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				normalStateData.m_eSkillID = pSkillTemplet->m_eID;
			}
		}
	}

}



/*static*/
void	CX2GUElsword_SwordMan::AppendComponentToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}



void CX2GUElsword_SwordMan::InitComponent()
{
	CX2GUUser::InitComponent();


	if( IsMyUnit() )
	{
		ASSERT( NULL != g_pX2Game );
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_PVP:
			{
				static_cast<CX2ElswordGageData*>(m_pGageData)->SetWayOfSwordPoint( 0.f );
				static_cast<CX2ElswordGageData*>(m_pGageData)->SetWayOfSwordState( 0 );
				m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
			} break;

		default:
			{
				//m_GageManager.UpdateUIElswordWSP( WSS_CENTER );
				m_FrameDataFuture.syncData.m_CannonBallCount = GetWayOfSwordState();
			} break;
		}
	}
}


void CX2GUElsword_SwordMan::ParseCommonRandomState()
{
	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{

		const char* tableName = "";
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_ELSWORD_SWORDMAN:
			{
				tableName = "ELSWORD_SWORDMAN";
			} break;

		case CX2Unit::UC_ELSWORD_KNIGHT:
			{
				tableName = "ELSWORD_KNIGHT";
			} break;

		case CX2Unit::UC_ELSWORD_SHEATH_KNIGHT:
			{
				tableName = "ELSWORD_SHEATH_KNIGHT";
			} break;

		case CX2Unit::UC_ELSWORD_MAGIC_KNIGHT:
			{
				tableName = "ELSWORD_MAGIC_KNIGHT";
			} break;

		case CX2Unit::UC_ELSWORD_LORD_KNIGHT:
			{
				tableName = "ELSWORD_LORD_KNIGHT";
			} break;


		case CX2Unit::UC_ELSWORD_RUNE_SLAYER:
			{
				tableName = "ELSWORD_RUNE_SLAYER";
			} break;

		case CX2Unit::UC_ELSWORD_INFINITY_SWORD:
			{
				tableName = "ELSWORD_INFINITY_SWORD";
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


/*static*/
void	CX2GUElsword_SwordMan::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUElsword_SwordMan::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUElsword_SwordMan::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForSwordToDeviceList( listInOut_, eDamageType_ );
}


void CX2GUElsword_SwordMan::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();

	if ( IsMyUnit() )
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



/*virtual*/ void CX2GUElsword_SwordMan::InitEnchantWeaponEffectUnitSpecific()
{
	if( true == m_bShowWeaponEnchantEffect )
	{
		for( UINT i=0; i<m_vecpWeapon.size(); ++i )
		{
			Weapon* pWeapon = m_vecpWeapon[i];
			pWeapon->InitEnchantParticleForSword();
		}
	}
}



void CX2GUElsword_SwordMan::DeleteElswordMajorParticle()
{
	CX2GUUser::DeleteGUUserMajorParticle();

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	for ( int index = 0; index < ELSWORD_MAJOR_PII_END; index++ )
	{
		pMajorParticleSystem->DestroyInstanceHandle( m_ahElswordMajorParticleInstance[index] );
	}
}

void CX2GUElsword_SwordMan::DeleteElswordMinorParticle()
{
	CX2GUUser::DeleteGUUserMinorParticle();

	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	for ( int index = 0; index < ELSWORD_MINOR_PII_END; index++ )
	{
		pMinorParticleSystem->DestroyInstanceHandle( m_ahElswordMinorParticleInstance[index] );
	}
}

void CX2GUElsword_SwordMan::InitializeElswordMajorParticleArray()
{
	CX2GUUser::InitializeGUUserMajorParticleArray();

	for ( int index = 0; index < ELSWORD_MAJOR_PII_END; index++ )
	{
		m_ahElswordMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUElsword_SwordMan::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
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


/*virtual*/ void CX2GUElsword_SwordMan::CreateMajorParticleArray()
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

CKTDGParticleSystem::CParticleEventSequence* CX2GUElsword_SwordMan::SetElswordMajorParticleByEnum( ELSWORD_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleElswordMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleElswordMajorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleElswordMajorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUElsword_SwordMan::InitializeElswordMinorParticleArray()
{
	CX2GUUser::InitializeGUUserMinorParticleArray();

	for ( int index = 0; index < ELSWORD_MINOR_PII_END; index++ )
	{
		m_ahElswordMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUElsword_SwordMan::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMinorParticle() == NULL )
		return;

	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"RevengeWait" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"GroundShockWaveBlue" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"ImpactTickBlue" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"STR_KuAAang" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"GroundShockWaveRed" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"ImpactTickRed" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"MagnumBladeFireWave" );
}




/*virtual*/ void CX2GUElsword_SwordMan::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_REVENGE_WAIT, L"RevengeWait" );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE, L"GroundShockWaveBlue", 2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE, L"ImpactTickBlue", 2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_STR_KUAANG, L"STR_KuAAang", 1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_RED, L"GroundShockWaveRed", 2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_TICK_RED, L"ImpactTickRed", 2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetElswordMinorParticleByEnum( ELSWORD_MINOR_PII_MAGNUM_BLADE_FIRE_WAVE, L"MagnumBladeFireWave" );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUElsword_SwordMan::SetElswordMinorParticleByEnum( ELSWORD_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleElswordMinorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleElswordMinorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUElsword_SwordMan::CommonStateStartFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStartFuture();
}

void CX2GUElsword_SwordMan::CommonStateStart()
{
	KTDXPROFILE();

	LUA_GET_VALUE(	m_LuaManager, "DAMAGEDATA_CHANGE_TIME",				m_fDamageDataChangeTime,			0.f );
	LUA_GET_VALUE(	m_LuaManager, "DAMAGEDATA_CHANGE_TIME2",			m_fDamageDataChangeTime2,			0.f );

	CX2GUUser::CommonStateStart();

	m_bFirstAttackSuccess = false;
}

void CX2GUElsword_SwordMan::CommonFrameMoveFuture()
{
	KTDXPROFILE();
	m_fReAttackZ1Time -= m_fElapsedTime;
	if( m_fReAttackZ1Time < 0.0f )
		m_fReAttackZ1Time = 0.0f;

	m_fReDashJumpXTime -= m_fElapsedTime;
	if( m_fReDashJumpXTime < 0.0f )
		m_fReDashJumpXTime = 0.0f;

	CX2GUUser::CommonFrameMoveFuture();
	m_FrameDataFuture.syncData.m_CannonBallCount = (int)GetWayOfSwordState();
}



void CX2GUElsword_SwordMan::CommonFrameMove()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMove();


	if( m_hSecondBladeSheath != INVALID_MESH_INSTANCE_HANDLE )
	{

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSecondBladeSheath );
		if( NULL != pMeshInst )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy2_Lhand" );
			pMeshInst->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}
	// ����� ������ ������ ���̻� �����ڵ� ������ ������ ����� ����Ʈ�� ������� �ʴ� ������ �־, �ʻ�� ���� Ȥ�� ����� ���°� �ƴϸ� ������ ����� ����Ʈ�� �����ֵ��� �Ѵ�.
	if( INVALID_EFFECTSET_HANDLE != m_hWindMill ||
		INVALID_EFFECTSET_HANDLE != m_hWindMillLand )
	{
		switch( GetNowStateID() )
		{
		case USI_SPECIAL_ATTACK_1:
		case USI_SPECIAL_ATTACK_HYPER_1:
		case USI_SPECIAL_ATTACK_2:
		case USI_SPECIAL_ATTACK_HYPER_2:
		case USI_SPECIAL_ATTACK_3:
		case USI_SPECIAL_ATTACK_HYPER_3:
		case USI_SPECIAL_ATTACK_4:
		case USI_SPECIAL_ATTACK_HYPER_4:
		case USI_SPECIAL_ATTACK_SLOTB_1:
		case USI_SPECIAL_ATTACK_SLOTB_HYPER_1:
		case USI_SPECIAL_ATTACK_SLOTB_2:
		case USI_SPECIAL_ATTACK_SLOTB_HYPER_2:
		case USI_SPECIAL_ATTACK_SLOTB_3:
		case USI_SPECIAL_ATTACK_SLOTB_HYPER_3:
		case USI_SPECIAL_ATTACK_SLOTB_4:
		case USI_SPECIAL_ATTACK_SLOTB_HYPER_4:
		case ESSI_SA_ESK_WINDMILL:
		case ESSI_SA_ESK_WINDMILL_HYPER:
		case ESSI_SA_ESK_WINDMILL_END:
		case ESSI_SA_ESK_WINDMILL_END_HYPER:
			{
				if( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.f ||
					m_ExtraDamagePack.m_Frozen.m_fTime > 0.f )
				{
					g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMill );
					g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMillLand );

					if( NULL != m_pSoundWindMill )
					{
						m_pSoundWindMill->Stop();
					}
				}
			} break;
		default:
			{
				g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMill );
				g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMillLand );

				if( NULL != m_pSoundWindMill )
				{
					m_pSoundWindMill->Stop();
				}
			} break;
		}
	}

	WayOfSwordStateChange();
	DestructionRageFrameMove();

	if ( GetNeedToUpdateWeaponEffect() )
	{
		UpdateWeaponEnchantEffectAndExtraDamageType();
		SetNeedToUpdateWeaponEffect( false );
	}
}

void CX2GUElsword_SwordMan::CommonStateEndFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEndFuture();
}

void CX2GUElsword_SwordMan::CommonStateEnd()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEnd();
}

//{{ robobeg : 2008-10-28
//void CX2GUElsword_SwordMan::CommonRender()
/*virtual*/
RENDER_HINT CX2GUElsword_SwordMan::CommonRender_Prepare()
//}} robobeg : 2008-10-28
{
	KTDXPROFILE();
// 	if( GetRemainHyperModeTime() > 0.0f )
// 	{
// 		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
// 		m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_RED;
// 		m_RenderParam.outLineColor		= 0xffff5511;
// 	}
	/*else */if( GetWayOfSwordState() == WSS_VIGOR )
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

/*virtual*/ bool CX2GUElsword_SwordMan::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_ONLY_ON_LINE*/ )
{
	KTDXPROFILE();

	const int INVALID_SKILL_SLOT_INDEX = -1;

	int iPressedSkillSlotIndex = INVALID_SKILL_SLOT_INDEX;
	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = NULL;

	CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree; 	// ������ ��� ��ų Ʈ��

	if ( false == CommonSpecialAttackEventProcess( accessUserSkillTree, pSkillSlotData, iPressedSkillSlotIndex ) )
		return false;

	if( NULL == pSkillSlotData )
		return false;

	// ����� ��ų
	const CX2SkillTree::SkillTemplet* pSkillTempletUsing = GetEquippedActiveSkillTemplet( pSkillSlotData->m_eID );
	if( NULL == pSkillTempletUsing )
		return false;

	if( false == CheckSkillUseCondition( eActiveSkillUseCondition, pSkillTempletUsing->m_eActiveSkillUseCondtion ) )
		return false;

	// ���Ĵ������� ��� ������ ��ų���� �˻�
	if( false == CheckSkillUsePVPOfficial( pSkillTempletUsing->m_eID ) )
		return false;

	const int iSkillTempletLevel = max( 1, accessUserSkillTree.GetSkillLevel( pSkillTempletUsing->m_eID ) );	/// ��ų ����

	float fMPConsume = GetActualMPConsume( pSkillTempletUsing->m_eID, iSkillTempletLevel );

	if( pSkillTempletUsing->m_eType == CX2SkillTree::ST_ACTIVE )
		fMPConsume -= ( fMPConsume * ( m_fMindOfFighterConsumeMpRate * m_iBrutalSlayerActivatedLevel ) );

	if( GetWayOfSwordState() == WSS_VIGOR )
	{
		const int iWayOfSwordType	= pSkillTempletUsing->m_iWOSType;

		if( iWayOfSwordType == 2 )
		{
			fMPConsume *= 0.7f;
		}
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

		if ( pSkillTempletUsing->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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

	if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_ES_ROLLING )
	{//
		// HP �� �Ҹ�� �纸�� ������ üũ.
		if( (GetMaxHp() * pSkillTempletUsing->GetSkillAbilityValue( CX2SkillTree::HP_CONSUME_REL, iSkillTempletLevel ) ) >= GetNowHp() )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD,
				GET_STRING( STR_ID_2627 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );

			return false;
		}
	}



	//{{ JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����
	CountUsedSkill( iPressedSkillSlotIndex );
	//}} JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����

	UpNowMp( -fMPConsume );

	if ( pSkillTempletUsing->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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

	float fAdjustCoolTimeMemo = 0.f;
	if( (pSkillTempletUsing->m_eID == CX2SkillTree::SI_SA_EMK_PHOENIX_TALON) && (GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO22 ) == true ) )
		fAdjustCoolTimeMemo = -5.f;

			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) + fAdjustCoolTimeMemo);

	if( CX2SkillTree::ST_ACTIVE == pSkillTempletUsing->m_eType )
	{
		accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) * m_fIncreaseCoolTimeRel );
	}

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO6 ) == true && pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_ES_ENDURANCE )
	{
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, pSkillTempletUsing->GetSkillCoolTimeValue( iSkillTempletLevel ) - 5.f );
	}


	if( m_bCompactCounterEnable == true && pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_ES_COUNTER )
	{
		accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, m_fCompactCounterCoolTime );
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


	m_iNowSpecialAttack = iPressedSkillSlotIndex + 1;
	m_bSpecialAttackEventProcessedAtThisFrame = true;

//{{ kimhc // 2010.11.1 // �ε峪��Ʈ - ���� ����
	ReducePain( pSkillTempletUsing->m_eType, accessUserSkillTree );
//}} kimhc // 2010.11.1 // �ε峪��Ʈ - ���� ����

	Send_SKILL_USE_REQ( pSkillTempletUsing->m_eID );

	return true;

}




// ���º�ȭ�� ���� �ʻ��� �ϵ��ڵ��ؼ� ���� ó�� �Ѵ�
/*virtual*/ bool CX2GUElsword_SwordMan::SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet )
{
	if( NULL == pSkillTemplet )
		return false;

	switch( pSkillTemplet->m_eID )
	{
	case CX2SkillTree::SI_A_ES_ENDURANCE:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ENDURANCE;

		} break;

		//{{ kimhc // 2010.11.1 // �ε峪��Ʈ - ���� ����
	case CX2SkillTree::SI_P_ELK_REDUCE_PAIN:
		{
			// �ر� ���¸�
			if ( UAI_ENDURANCE == m_FrameDataFuture.syncData.nowAction )
				m_FrameDataFuture.syncData.nowAction = UAI_ENDURANCE_AND_REDUCE_PAIN;
			else
				m_FrameDataFuture.syncData.nowAction = UAI_REDUCE_PAIN;
		} break;
		//}} kimhc // 2010.11.1 // �ε峪��Ʈ - ���� ����

	default:
		{
			return false;
		} break;
	}

	return true;
}



/*virtual*/ void CX2GUElsword_SwordMan::NoStateChangeActionFrameMove()
{
	switch( m_FrameDataNow.syncData.nowAction )
	{
	case UAI_ENDURANCE:
		{
			ProcessEnduranceSkill();
		} break;

		//{{ kimhc // 2010.11.1 // �ε峪��Ʈ - ���� ����
	case UAI_REDUCE_PAIN:
		{
			CureExtraDamage();
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELK_MAGIC_RESISTANCE", this );
		} break;

	case UAI_ENDURANCE_AND_REDUCE_PAIN:
		{
			ProcessEnduranceSkill();
			CureExtraDamage();
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELK_MAGIC_RESISTANCE", this );
		} break;
		//}} kimhc // 2010.11.1 // �ε峪��Ʈ - ���� ����

	}
}

void CX2GUElsword_SwordMan::ProcessEnduranceSkill()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ES_ENDURANCE );
	if( NULL != pSkillTemplet )
	{

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO6 ) == true )
		{
			SetBuffFactorToGameUnit( pSkillTemplet, 1 );
		}
		else
		{
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_ENDURANCE", this );
		PlaySound( L"Elsword_Endurance.ogg" );
	}
}


//{{ oasis907 : ����� [2010.11.1] // �� �����̾� - ���� ���� ����
float CX2GUElsword_SwordMan::GetMagicReflectRate()
{
	float fMagicReflectRate = m_fMagicReflectRate;
	if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
	{
		fMagicReflectRate *= 3.f;
	}
	return fMagicReflectRate;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//							������Ʈ �Լ� ����

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//ESSI_DIE_FRONT
void CX2GUElsword_SwordMan::DieFrontStartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( 0.5f );
}

void CX2GUElsword_SwordMan::DieFrontStart()
{
	CommonStateStart();
	DyingStart();

	const WAY_OF_SWORD_STATE eWayofSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

	if( eWayofSwordState == WSS_VIGOR )
		End_SwordmanVigor();
	else if( eWayofSwordState == WSS_DESTRUCTION )
		End_SwordmanDestruction();
	m_pXSkinAnim->SetPlaySpeed( 0.5f );
}

void CX2GUElsword_SwordMan::DieFrontFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

void CX2GUElsword_SwordMan::DieFrontEventProcess()
{
//{{ robobeg : 2008-10-21
	//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
    if ( GetShowObject() == false && m_pXSkinAnim->IsAnimationEnd() == true )
//}} robobeg : 2008-10-21
	{
		StateChange( GUSI_DIE );
	}
}


//ESSI_DIE_BACK
void CX2GUElsword_SwordMan::DieBackStartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( 0.5f );
}

void CX2GUElsword_SwordMan::DieBackStart()
{
	CommonStateStart();
	DyingStart();

	const WAY_OF_SWORD_STATE eWayofSwordState = static_cast<const WAY_OF_SWORD_STATE>( GetWayOfSwordState() );

	if( eWayofSwordState == WSS_VIGOR )
		End_SwordmanVigor();
	else if( eWayofSwordState == WSS_DESTRUCTION )
		End_SwordmanDestruction();
	m_pXSkinAnim->SetPlaySpeed( 0.5f );
}


void CX2GUElsword_SwordMan::DieBackFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

void CX2GUElsword_SwordMan::DieBackEventProcess()
{
//{{ robobeg : 2008-10-21
	//if( m_bShow == false && m_pXSkinAnim->IsAnimationEnd() == true )
    if ( GetShowObject() == false  && m_pXSkinAnim->IsAnimationEnd() == true )
//}} robobeg : 2008-10-21
	{
		StateChange( GUSI_DIE );
	}
}

#pragma DMLEE_NOTE( "�� �Լ� �� ĳ���� ���� �����ϰ� �����Ǹ� GUUSER�� �Űܼ� �ϳ��� ��������" )

void CX2GUElsword_SwordMan::WinFrameMove()
{
	if( true == m_bIsMagicKnightWinMotion2 )
	{
		if( m_hSeqHandFire != INVALID_PARTICLE_SEQUENCE_HANDLE )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqHandFire );

			if( m_pXSkinAnim->EventTimer( 1.08f ) == true )
			{
				if( NULL != pSeq )
				{
					pSeq->SetShowObject( true );
					pSeq->SetEmitRate( 30.0f, 50.0f );
				}
				else
				{
					m_hSeqHandFire = INVALID_PARTICLE_SEQUENCE_HANDLE;
				}
			}

			if( NULL != pSeq )
			{
				D3DXVECTOR3 vBonePos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
				pSeq->SetPosition( vBonePos );
			}
		}
		else
		{
			m_hSeqHandFire = g_pData->GetUIMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MagicKnightHandFire", 0,0,0 );

			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pData->GetUIMajorParticle()->GetInstanceSequence( m_hSeqHandFire );
			if( NULL != pSeq )
			{
				pSeq->SetShowObject(false);
			}
		}
	}

	CommonFrameMove();
}

//USI_WAIT
void CX2GUElsword_SwordMan::WaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );
}

void CX2GUElsword_SwordMan::WaitStart()
{
	CommonStateStart();
	PlayAnimationWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}

void CX2GUElsword_SwordMan::WaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ESSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_COMBO_X1 );
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
	//ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 10.f, ESSI_REVENGE_WAIT )
	else if( m_InputData.oneDown == true
		&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( ( GetWayOfSwordState() == WSS_VIGOR || GetWayOfSwordState() == WSS_DESTRUCTION ) &&
			m_FrameDataNow.unitCondition.fStateTime >= 0.1f )
	{
		StateChange( ESSI_SWORD_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}
	CommonEventProcess();
}




//USI_WALK

void CX2GUElsword_SwordMan::WalkEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ESSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_COMBO_X1 );
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
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}


//USI_JUMP_READY
void CX2GUElsword_SwordMan::JumpReadyEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESSI_JUMP_ATTACK_Z );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_JUMP_ATTACK_X );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
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

void CX2GUElsword_SwordMan::JumpUpEventProcess()
{
	if( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true )
	{
		if( GetEnableDash() == true )
		{
			StateChange( USI_DASH_JUMP );
		}
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESSI_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_JUMP_ATTACK_X );
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

void CX2GUElsword_SwordMan::JumpDownEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ESSI_WALL_LANDING );
	}
	else if( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true )
	{
		if( GetEnableDash() == true )
		{
			StateChange( USI_DASH_JUMP );
		}
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESSI_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_JUMP_ATTACK_X );
	}
	else if( m_PhysicParam.nowSpeed.y >= 0.0f )
	{
		StateChange( USI_JUMP_UP );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::JumpLandingEventProcess()
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
	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ESSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_COMBO_X1 );
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		StateChange( USI_WAIT, false );
	}


	CommonEventProcess();
}

void CX2GUElsword_SwordMan::DashEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESSI_DASH_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_DASH_ATTACK_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	//ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 10.f, ESSI_REVENGE_WAIT )
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
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
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}


//ESSI_DASH_END
void CX2GUElsword_SwordMan::DashEndStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUElsword_SwordMan::DashEndStart()
{
	CommonStateStart();

}

void CX2GUElsword_SwordMan::DashEndFrameMove()
{
	if( m_pXSkinAnim->EventTimer( 0.4f ) == true )
	{
		CreateStepDust();

	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashEndEventProcess()
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
			StateChange( ESSI_COMBO_Z1 );
		}
		else
		{
			StateChange( ESSI_DASH_COMBO_Z1 );
		}
	}
	else if( m_InputData.oneX == true )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 8.0f )
		{
			StateChange( ESSI_COMBO_X1 );
		}
		else
		{
			StateChange( ESSI_DASH_ATTACK_X );
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


//ESSI_DASH_JUMP
void CX2GUElsword_SwordMan::DashJumpStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * 1.2f;
	m_bDownForce					= false;
}

void CX2GUElsword_SwordMan::DashJumpStart()
{
	CommonStateStart();


	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
	{
		CreateStepDust();
	}
}

void CX2GUElsword_SwordMan::DashJumpFrameMoveFuture()
{
	if( m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight	== true )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * 1.6f;
	}
	else if( m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight	== false )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * 1.6f;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::DashJumpEventProcess()
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
			StateChange( ESSI_DASH_JUMP_POWER_LANDING );
			m_bDownForce = false;
		}
		else
		{
			if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else
				StateChange( USI_DASH_JUMP_LANDING );
			m_bDownForce = false;
		}
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ESSI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ESSI_DASH_JUMP_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true && m_fReDashJumpXTime <= 0.0f )
	{
		StateChange( ESSI_DASH_JUMP_COMBO_X1 );
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
	if( m_InputData.oneDown == true )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
		m_bDownForce = true;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}


	CommonEventProcess();
}

void CX2GUElsword_SwordMan::DashJumpEndFuture()
{
	CommonStateEnd();
	m_PhysicParam.fDownAccel	= 0.0f;
}

void CX2GUElsword_SwordMan::DashJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_InputData.oneZ == true )
	{
		StateChange( ESSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_COMBO_X1 );
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

	CommonEventProcess();
}


//ESSI_DASH_JUMP_POWER_LANDING
void CX2GUElsword_SwordMan::DashJumpPowerLandingStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x /= 2.0f;
}

void CX2GUElsword_SwordMan::DashJumpPowerLandingStart()
{
	CommonStateStart();


	CreateStepDust();

}

void CX2GUElsword_SwordMan::DashJumpPowerLandingEventProcess()
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
			StateChange( ESSI_COMBO_Z1 );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( ESSI_COMBO_X1 );
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



//ESSI_HYPER_MODE
void CX2GUElsword_SwordMan::HyperModeFrameMove()
{
	CommonHyperModeFrameMove(0.05f, 0.83f);
}

////ESSI_DAMAGE_GROGGY
//void CX2GUElsword_SwordMan::ESSI_DAMAGE_GROGGY_FrameMoveFuture()
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
//void CX2GUElsword_SwordMan::ESSI_DAMAGE_GROGGY_EventProcess()
//{
//	if( m_fGroggyTime <= 0.0f )
//	{
//		StateChange( USI_WAIT );
//	}
//	CommonEventProcess();
//}

//ESSI_DAMAGE_SMALL_FRONT
void CX2GUElsword_SwordMan::DamageSmallFrontEventProcess()
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


//ESSI_DAMAGE_SMALL_BACK
void CX2GUElsword_SwordMan::DamageSmallBackEventProcess()
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


//ESSI_DAMAGE_BIG_FRONT
void CX2GUElsword_SwordMan::DamageBigFrontStart()
{
	CommonStateStart();

	CreateStepDust();

}

void CX2GUElsword_SwordMan::DamageBigFrontEventProcess()
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.93f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}


//ESSI_DAMAGE_BIG_BACK
void CX2GUElsword_SwordMan::DamageBigBackStart()
{
	CommonStateStart();

	CreateStepDust();

}

void CX2GUElsword_SwordMan::DamageBigBackEventProcess()
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


//ESSI_DAMAGE_DOWN_FRONT
void CX2GUElsword_SwordMan::DamageDownFrontFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
	{

		CreateStepDust();

	}

	CommonFrameMove();
}







void CX2GUElsword_SwordMan::DamageDownFrontEventProcess()
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
				StateChange( ESSI_STANDUP_ATTACK_FRONT_NO_DOWN );
			}
			else
			{
				StateChange( ESSI_STANDUP_ATTACK_FRONT );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ESSI_STANDUP_ATTACK_FRONT );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
			StateChange( ESSI_STANDUP_ROLLING_FRONT_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
			StateChange( ESSI_STANDUP_ROLLING_FRONT_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_FRONT );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
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





//ESSI_DAMAGE_DOWN_BACK
void CX2GUElsword_SwordMan::DamageDownBackFrameMove()
{
	if( m_pXSkinAnim->EventTimer( 0.32f ) == true )
	{
		CreateStepDust();

	}

	CommonFrameMove();
}





void CX2GUElsword_SwordMan::DamageDownBackEventProcess()
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
				StateChange( ESSI_STANDUP_ATTACK_BACK_NO_DOWN );
			}
			else
			{
				StateChange( ESSI_STANDUP_ATTACK_BACK );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ESSI_STANDUP_ATTACK_BACK );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
			StateChange( ESSI_STANDUP_ROLLING_BACK_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
			StateChange( ESSI_STANDUP_ROLLING_BACK_BACK );
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

void CX2GUElsword_SwordMan::DamageAirDownLandingEventProcess()
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
				StateChange( ESSI_STANDUP_ATTACK_FRONT_NO_DOWN );
			}
			else
			{
				StateChange( ESSI_STANDUP_ATTACK_FRONT );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ESSI_STANDUP_ATTACK_FRONT );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
			StateChange( ESSI_STANDUP_ROLLING_FRONT_FRONT );
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
			StateChange( ESSI_STANDUP_ROLLING_FRONT_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_FRONT );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66f )
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

//ESSI_STANDUP_ROLLING_FRONT_FRONT
void CX2GUElsword_SwordMan::StandUpRollingFrontFrontEventProcess()
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

void CX2GUElsword_SwordMan::StandUpRollingFrontFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}


//ESSI_STANDUP_ROLLING_FRONT_BACK
void CX2GUElsword_SwordMan::StandUpRollingFrontBackEventProcess()
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


void CX2GUElsword_SwordMan::StandUpRollingFrontBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}



//ESSI_STANDUP_ROLLING_BACK_FRONT
void CX2GUElsword_SwordMan::StandUpRollingBackFrontEventProcess()
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

void CX2GUElsword_SwordMan::StandUpRollingBackFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}



//ESSI_STANDUP_ROLLING_BACK_BACK
void CX2GUElsword_SwordMan::StandUpRollingBackBackEventProcess()
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

void CX2GUElsword_SwordMan::StandUpRollingBackBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}




void CX2GUElsword_SwordMan::StandUpAttackFrontEventProcess()
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

void CX2GUElsword_SwordMan::StandUpAttackFrontEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

void CX2GUElsword_SwordMan::StandUpAttackBackEventProcess()
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

void CX2GUElsword_SwordMan::StandUpAttackBackEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}



void CX2GUElsword_SwordMan::RevengeWaitStart()
{
	CommonStateStart();
	CreateStepDust();

}

void CX2GUElsword_SwordMan::RevengeWaitFrameMoveFuture()
{
	m_PhysicParam.nowSpeed.x = 0.0f;
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::RevengeWaitFrameMove()
{
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO5 ) == true &&
    m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		SetDefenceMemoTime(4.f);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.09f ) )
	{

		D3DXVECTOR3 pos = m_FrameDataNow.syncData.position;
		pos.y += 70.0f;
		if( m_FrameDataNow.syncData.bIsRight == true )
			pos = pos + m_FrameDataNow.unitCondition.dirVector * 100.0f;
		else
			pos = pos + m_FrameDataNow.unitCondition.dirVector * -100.0f;

		CKTDGParticleSystem::CParticleEventSequence* pSeqRevengeWait = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_REVENGE_WAIT ) );

		if( pSeqRevengeWait != NULL )
		{
			PlaySequenceByTriggerCount( pSeqRevengeWait, pos, 10,10, 5 );

			pSeqRevengeWait->SetAddRotate( D3DXVECTOR3( 0.0f, m_FrameDataNow.unitCondition.dirDegree.y + 70.0f, 0.0f ) );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeqQuestion = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hQuestionMark );
		if( NULL != pSeqQuestion )
		{
			pSeqQuestion->CreateNewParticle( pSeqQuestion->GetPosition() );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::RevengeWaitEventProcess()
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
	ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 0.6f )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}



//ESSI_REVENGE_ATTACK
void CX2GUElsword_SwordMan::RevengeAttackStart()
{
	CommonStateStart();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_ES_COUNTER );

	if( NULL != pSkillTemplet )
	{
		int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ES_COUNTER );

		m_DamageData.damage.fPhysic += m_fRevengeAddDamage * pSkillTemplet->GetSkillPowerRateValue( iSkillLevel );

		if( GetRandomInt() < pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel ) * 100.f )
		{
			m_DamageData.reActType = CX2DamageManager::RT_UP;
			m_DamageData.backSpeed = D3DXVECTOR2( 1100.f, 1900.f );
		}
	}

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO5 ) == true )
	{
		SetDefenceMemoTime(4.f);
	}

}

void CX2GUElsword_SwordMan::RevengeAttackFrameMoveFuture()
{
	if( m_FrameDataFuture.unitCondition.fStateTime >= 0.1f && m_FrameDataFuture.unitCondition.fStateTime < 0.4 )
	{
		//m_FrameDataFuture.syncData.bFrameStop = true;
	}
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::RevengeAttackFrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::RevengeAttackCameraMove()
{
	CommonCameraMove();

	if( m_FrameDataNow.stateParam.bEventFlagList[0] == false )
		{
			switch( m_FrameDataNow.stateParam.normalCamera )
			{
				case 1:
					{
						if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
						{
							g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
							g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
							m_LuaManager.EndTable();
						}
					}
					break;

				case 2:
					{
						if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
						{
							g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
							g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
							m_LuaManager.EndTable();
						}
					}
					break;
			}
		}

	m_FrameDataNow.stateParam.bEventFlagList[0] = true;
}

void CX2GUElsword_SwordMan::RevengeAttackEventProcess()
{
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

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::WayOfSwordStateChange()
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
		//m_GageManager.UpdateUIElswordWSP( WSS_VIGOR );
	}
	else if( iNextWayOfSwordState == WSS_DESTRUCTION &&
		eWayofSwordState != WSS_DESTRUCTION )
	{
		if( eWayofSwordState == WSS_VIGOR )
			End_SwordmanVigor();

		Start_SwordmanDestruction();
		SetWayOfSwordState( static_cast<int>( WSS_DESTRUCTION ) );
		//m_GageManager.UpdateUIElswordWSP( WSS_DESTRUCTION );
	}
	else if( iNextWayOfSwordState == WSS_TOWARD_VIGOR &&
		eWayofSwordState != WSS_TOWARD_VIGOR && eWayofSwordState != WSS_VIGOR )
	{
		if( eWayofSwordState == WSS_DESTRUCTION )
			End_SwordmanDestruction();

		SetWayOfSwordState( static_cast<int>( WSS_TOWARD_VIGOR ) );
		//m_GageManager.UpdateUIElswordWSP( WSS_TOWARD_VIGOR );
	}
	else if( iNextWayOfSwordState == WSS_TOWARD_DESTRUCTION &&
		eWayofSwordState != WSS_TOWARD_DESTRUCTION && eWayofSwordState != WSS_DESTRUCTION )
	{
		if( eWayofSwordState == WSS_VIGOR )
			End_SwordmanVigor();

		SetWayOfSwordState( static_cast<int>( WSS_TOWARD_DESTRUCTION ) );
		//m_GageManager.UpdateUIElswordWSP( WSS_TOWARD_DESTRUCTION );
	}
	else if( iNextWayOfSwordState == WSS_CENTER &&
		eWayofSwordState != WSS_CENTER )
	{
		if( eWayofSwordState == WSS_VIGOR )
			End_SwordmanVigor();
		else if( eWayofSwordState == WSS_DESTRUCTION )
			End_SwordmanDestruction();

		SetWayOfSwordState( static_cast<int>( WSS_CENTER ) );
		//m_GageManager.UpdateUIElswordWSP( WSS_CENTER );
	}
	else // Do Nothing
	{
		;
	}

	return;
}

void CX2GUElsword_SwordMan::SwordWaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationSwordWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );
}

void CX2GUElsword_SwordMan::SwordWaitStart()
{
	CommonStateStart();
	PlayAnimationSwordWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}

void CX2GUElsword_SwordMan::SwordWaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	else if( m_InputData.oneZ == true && (m_bReAttackZ1Right != m_FrameDataFuture.syncData.bIsRight || m_fReAttackZ1Time <= 0.0f) )
	{
		StateChange( ESSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ESSI_COMBO_X1 );
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
		//ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 10.f, ESSI_REVENGE_WAIT )
	else if( m_InputData.oneDown == true
	&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition, &m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
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

void CX2GUElsword_SwordMan::PlayAnimationSwordWaitStart( CKTDGXSkinAnim* pXSkinAnim_, const FrameData& refFrameData_ )
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




void CX2GUElsword_SwordMan::DestructionRageFrameMove()
{
	if( IsSuperArmor() )
		return;

	m_fDestructionRageTime -= m_fElapsedTime;
	if( m_fDestructionRageTime < 0.0f )
	{
		m_fDestructionRageTime = 0.0f;
	}
	else
	{
		m_FrameDataNow.stateParam.bSuperArmor		= true;
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	}
	return;
}


void CX2GUElsword_SwordMan::DoStateDestructionRage()
{
	if( IsMyUnit() == true )
		SetWayOfSwordPoint( GetWayOfSwordPoint() + g_pData->GetDamageManager()->GetDestRageWSPGain() );

	m_FrameDataNow.stateParam.bSuperArmor		= true;
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	m_fDestructionRageTime						= 6.0f;
	m_PhysicParam.nowSpeed.x					= 0.0f;
	m_PhysicParam.nowSpeed.y					= 0.0f;

	CureExtraDamage( CX2DamageManager::EDT_ENCHANT_SHOCK );
	CureExtraDamage( CX2DamageManager::EDT_FROZEN );
	CureExtraDamage( CX2DamageManager::EDT_ENTANGLE );
	CureExtraDamage( CX2DamageManager::EDT_DRYAD );
	CureExtraDamage( CX2DamageManager::EDT_STUN );
	CureExtraDamage( CX2DamageManager::EDT_Y_PRESSED );
	CureExtraDamage( CX2DamageManager::EDT_PANIC );

	m_fGroggyTime = 0.f;

	StateChange( ESSI_DESTRUCTION_RAGE, false );

	D3DXVECTOR3 pos = m_FrameDataNow.syncData.position;
	pos.y += 80.f;
	g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MonsterAngryWind", pos);
	g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Counter_Particle01", pos );



	g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_ANGRY_WIND", GetPowerRate(), pos,
		GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );



	UpDownCrashCamera( 30.0f, 0.4f );
}

void CX2GUElsword_SwordMan::DestructionRageStateStart()
{
	CommonStateStart();
}

void CX2GUElsword_SwordMan::DestructionRageStateFrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DestructionRageStateEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		if( true == GetRidingOn() )
			StateChange( USI_RIDING_JUMP_DOWN );
		else
			StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
/*	// ��ȹ ��û���� ��ų ĵ�� ����
	else if ( true == SpecialAttackEventProcess() )
	{
	}
*/
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == GetRidingOn() )
			StateChange( USI_RIDING_WAIT );
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//ESSI_COMBO_Z1
void CX2GUElsword_SwordMan::ComboZ1FrameMoveFuture()
{
	m_PhysicParam.nowSpeed.x = 0.0f;
	m_PhysicParam.nowSpeed.y = 0.0f;

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ComboZ1FrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboZ1EventProcess()
{
	if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.27f )
	{
		StateChange( ESSI_COMBO_Z2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.27f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.27f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.4f )
	{
		StateChange( ESSI_COMBO_Z2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}
	CommonEventProcess();
}


//ESSI_COMBO_Z2
void CX2GUElsword_SwordMan::ComboZ2FrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboZ2EventProcess()
{
	if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.366f &&
		CX2Unit::UC_ELSWORD_SHEATH_KNIGHT != GetUnit()->GetClass() && CX2Unit::UC_ELSWORD_INFINITY_SWORD != GetUnit()->GetClass() )
	{
		StateChange( ESSI_COMBO_Z3 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.366f &&
		(CX2Unit::UC_ELSWORD_SHEATH_KNIGHT == GetUnit()->GetClass() || CX2Unit::UC_ELSWORD_INFINITY_SWORD == GetUnit()->GetClass()) )
	{
		StateChange( ESSI_ETK_COMBO_ZZZ );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.366f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fCanNotAttackTime = 0.1f;
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//ESSI_COMBO_Z3
void CX2GUElsword_SwordMan::ComboZ3FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboZ3EventProcess()
{
	m_bReAttackZ1Right = m_FrameDataFuture.syncData.bIsRight;
	m_fReAttackZ1Time = 0.2f;

	CANCEL_COMBO_ON_DIRECTION_CHANGE( 3 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	SKILL_CANCEL_AFTER( 0.01f )
	ELSE_IF_STATE_CHANGE_ON_( 2, 0.333f, 0.333f, m_InputData.oneZ == true && GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_LORD_KNIGHT &&
		( ( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) ||
		( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ), ESSI_ELK_ZZZfrontZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								true == m_InputData.oneZ, ESSI_COMBO_Z4 )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
								true == m_InputData.oneX, ESSI_COMBO_Z4a )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}



//ESSI_COMBO_Z4
void CX2GUElsword_SwordMan::ComboZ4FrameMoveFuture()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.17f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed() * 1.3f;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ComboZ4FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboZ4EventProcess()
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.7f )
		{
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}


//ESSI_COMBO_Z4a
void CX2GUElsword_SwordMan::ComboZ4aFrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboZ4aEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureUp == true || m_InputData.pureDown == true
		|| m_InputData.pureRight == true || m_InputData.pureLeft == true )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
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


//ESSI_COMBO_X1
void CX2GUElsword_SwordMan::ComboX1FrameMoveFuture()
{
	m_PhysicParam.nowSpeed.y = 0.0f;

    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.2f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ComboX1FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboX1EventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 3 );


	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_( 1, 0.43f, 0.43f, m_InputData.oneZ == true && GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_RUNE_SLAYER, ESSI_ERS_XZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true && ( GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_SHEATH_KNIGHT || GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_INFINITY_SWORD ), ESSI_ETK_COMBO_XX )
	ELSE_IF_STATE_CHANGE_ON_( 0, 0.43f, 0.43f, m_InputData.oneX == true && GetUnit()->GetClass() != CX2Unit::UC_ELSWORD_SHEATH_KNIGHT && GetUnit()->GetClass() != CX2Unit::UC_ELSWORD_INFINITY_SWORD, ESSI_COMBO_X2 )
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
		{
			m_fCanNotAttackTime = 0.2f;
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}


//ESSI_COMBO_X2
void CX2GUElsword_SwordMan::ComboX2FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.31f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed() * 1.5f;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ComboX2FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboX2EventProcess()
{

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_InputData.oneX && CX2Unit::UC_ELSWORD_LORD_KNIGHT != GetUnit()->GetClass(), ESSI_COMBO_X3 )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
		true == m_InputData.oneZ, ESSI_COMBO_X3a )
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
		true == m_InputData.oneX && CX2Unit::UC_ELSWORD_LORD_KNIGHT == GetUnit()->GetClass() , ESSI_ELK_XXX )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}


	CommonEventProcess();
}


//ESSI_COMBO_X3
void CX2GUElsword_SwordMan::ComboX3FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.3f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ComboX3FrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboX3EventProcess()
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.7f )
		{
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}


//ESSI_COMBO_X3a
void CX2GUElsword_SwordMan::ComboX3aFrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ComboX3aEventProcess()
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
		{
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}


//ESI_DASH_COMBO_Z1
void CX2GUElsword_SwordMan::DashComboZ1FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashComboZ1EventProcess()
{
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.3666f
		&& ( CX2Unit::UC_ELSWORD_SHEATH_KNIGHT == GetUnit()->GetClass() || CX2Unit::UC_ELSWORD_INFINITY_SWORD == GetUnit()->GetClass() )
		&& true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_ETK_DASH_COMBO_ZZ );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.36f
		&& ( CX2Unit::UC_ELSWORD_SHEATH_KNIGHT != GetUnit()->GetClass() && CX2Unit::UC_ELSWORD_INFINITY_SWORD != GetUnit()->GetClass() )
		&& true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_DASH_COMBO_Z2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.36f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}


//ESSI_DASH_COMBO_Z2
void CX2GUElsword_SwordMan::DashComboZ2StartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUElsword_SwordMan::DashComboZ2FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashComboZ2EventProcess()
{
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

	CommonEventProcess();
}

//ESSI_KNIGHT_DASH_COMBO_Z2
void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_COMBO_Z2_StateStartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_COMBO_Z2_EventProcess()
{
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.45f
		&& true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_DASH_COMBO_Z3 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.45f
		&& true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_DASH_COMBO_Z3a );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.45f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.45f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	else if( m_InputData.pureUp == true || m_InputData.pureDown == true ||
		m_InputData.pureA == true || m_InputData.pureS == true || m_InputData.pureD == true || m_InputData.pureC == true
		|| m_InputData.pureQ == true || m_InputData.pureW == true || m_InputData.pureE == true || m_InputData.pureR == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
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

	CommonEventProcess();
}


//ESSI_KNIGHT_DASH_COMBO_Z3
void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_COMBO_Z3_StateStartFuture()
{
	CommonStateStartFuture();

	/*
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
		*/
}

void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_COMBO_Z3_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.0f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();

}

//ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3
void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3_StateStartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3_EventProcess()
{
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}


//ESSI_KNIGHT_DASH_COMBO_Z3a
void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_COMBO_Z3a_StateStartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_COMBO_Z3a_EventProcess()
{
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}



//ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a
void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.515f ) )
	{
		if( FlushMp( _CONST_ELSWORD_::MAGIC_FIRE_BALL_MP_COST ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 rot = GetRotateDegree();
			rot.y += 180.0f;

			if( CX2Unit::UC_ELSWORD_RUNE_SLAYER == GetUnit()->GetClass() )
			{
				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_RUNE_SLAYER_DASH_ZZX", GetPowerRate() * m_fAttackPowerRateFireBall, posR, D3DXVECTOR3(0,0,0), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false,-1.f, 1.f, m_fRuneProjectileRangeIncPercent );
				if( NULL != pEffect )
				{
					pEffect->SetScale( m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent );
				}
			}
			else
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_BIG_ELSWORD", GetPowerRate() * m_fAttackPowerRateFireBall, posR, rot, rot, m_FrameDataNow.unitCondition.landPosition.y );
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, 1, 1 );
			if( pSeq != NULL )
			{
				D3DXVECTOR3 rot = GetRotateDegree();
				rot.y += 180.0f;
				pSeq->SetAxisAngle( rot );
				pSeq->SetAddRotate( rot );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.2f )
		{
			if( false == IsOnSomethingFuture() )
			{
				StateChange( USI_JUMP_DOWN );
				m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
			}
			else
				StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_StateEndFuture()
{
	CommonStateEndFuture();
	m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}


//ESSI_DASH_ATTACK_X
void CX2GUElsword_SwordMan::DashAttackXStartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
}

void CX2GUElsword_SwordMan::DashAttackXFrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashAttackXEventProcess()
{
	if( GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_INFINITY_SWORD )
	{
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
		ELSE_IF_STATE_CHANGE_ON_EX_( 0, 0.233f, 0.63f, 0.63f, m_InputData.oneX == true, ESSI_EIS_DASH_COMBO_XX )
		ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
		{
			if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.66666f )
			{
				StateChange( USI_WAIT );
			}
		}
	}
	else
	{
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
		ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
		{
			if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
			{
				StateChange( USI_WAIT );
			}
		}
	}

	CommonEventProcess();
}


//ESSI_JUMP_ATTACK_Z
void CX2GUElsword_SwordMan::JumpAttackZFrameMoveFuture()
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

void CX2GUElsword_SwordMan::JumpAttackZFrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::JumpAttackZEventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST


	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			m_fCanNotJumpTime = 0.2f;
		}
		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}



//ESSI_JUMP_ATTACK_X
void CX2GUElsword_SwordMan::JumpAttackXFrameMoveFuture()
{
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( m_InputData.pureDown == true )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::JumpAttackXFrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::JumpAttackXEventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST


	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			m_fCanNotJumpTime = 0.2f;
		}
		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}


//ESSI_DASH_JUMP_COMBO_Z1
void CX2GUElsword_SwordMan::DashJumpComboZ1FrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashJumpComboZ1EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST



	if( m_InputData.oneZ == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( ESSI_DASH_JUMP_COMBO_Z2 );
		}
	}

	CommonEventProcess();
}


//ESI_DASH_JUMP_COMBO_Z2
void CX2GUElsword_SwordMan::DashJumpComboZ2FrameMove()
{

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashJumpComboZ2EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST


	if( m_InputData.oneZ == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( ESSI_DASH_JUMP_COMBO_Z3 );
		}
	}

	CommonEventProcess();
}


//ESSI_DASH_JUMP_COMBO_Z3
void CX2GUElsword_SwordMan::DashJumpComboZ3FrameMove()
{

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.26f && m_pXSkinAnim->GetNowAnimationTime() < 0.37f )
	{
		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataNow.stateParam.normalCamera	= m_FrameDataNow.stateParam.hitCamera;
			m_FrameDataNow.stateParam.bResetCamera	= false;
			m_FrameDataNow.unitCondition.bHit		= true;
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashJumpComboZ3EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST


	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataNow.unitCondition.bHit == true )
		{
			StateChange( ESSI_DASH_JUMP_COMBO_Z3_LANDING );
		}
		else
		{
			StateChange( USI_DASH_JUMP_LANDING );
		}
	}

	CommonEventProcess();
}




//ESSI_DASH_JUMP_COMBO_Z3_LANDING
void CX2GUElsword_SwordMan::DashJumpComboZ3LandingStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.x /= 2.0f;
}

void CX2GUElsword_SwordMan::DashJumpComboZ3LandingStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.normalCamera = m_FrameDataNow.stateParam.hitCamera;
}

void CX2GUElsword_SwordMan::DashJumpComboZ3LandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 6.0f )
	{
		if( m_InputData.oneZ == true )
		{
			StateChange( ESSI_COMBO_Z1 );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( ESSI_COMBO_X1 );
		}
		else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
		{
			StateChangeDashIfPossible();
		}
		else if( m_InputData.pureRight == true || m_InputData.pureLeft == true )
		{
			StateChange( USI_WALK );
		}
		//ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 10.f, ESSI_REVENGE_WAIT )
		else if( m_InputData.pureUp == true )
		{
			StateChange( USI_JUMP_READY );
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
		else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}



//ESSI_DASH_JUMP_COMBO_X1
void CX2GUElsword_SwordMan::DashJumpComboX1FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashJumpComboX1EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN );
//#endif LINEMAP_FAST_WIND_TEST








	//CANCEL_COMBO_ON_DIRECTION_CHANGE( 3 );
	if( m_InputData.oneX == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[2] = false;
	}
	else if( m_InputData.oneZ == true && GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_MAGIC_KNIGHT )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
		m_FrameDataFuture.stateParam.bEventFlagList[2] = false;
	}
	else if( m_InputData.oneZ == true && GetUnit()->GetClass() == CX2Unit::UC_ELSWORD_RUNE_SLAYER )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[2] = true;
	}




	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( ESSI_DASH_JUMP_COMBO_X2 );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ESSI_DASH_JUMP_COMBO_X2a );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
		{
			StateChange( ESSI_ERS_DASH_JUMP_XZ );
		}
	}

	//m_fReDashJumpXTime = 0.3f;

	CommonEventProcess();
}





//ESSI_DASH_JUMP_COMBO_X2
void CX2GUElsword_SwordMan::DashJumpComboX2FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.23f ) )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::DashJumpComboX2FrameMove()
{
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.15f )
	{

		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataNow.stateParam.bResetCamera	= false;
			m_FrameDataNow.unitCondition.bHit		= true;
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::DashJumpComboX2EventProcess()
{
//
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST



	if( true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_DASH_JUMP_COMBO_X2_LANDING );
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::DashJumpComboX2End()
{
	CommonStateEnd();
	UpCrashCamera( 200.0f, -3500.0f );
}


//ESSI_DASH_JUMP_COMBO_X2_LANDING
void CX2GUElsword_SwordMan::DashJumpComboX2LandingStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.x /= 2.0f;
}

void CX2GUElsword_SwordMan::DashJumpComboX2LandingStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.normalCamera = m_FrameDataNow.stateParam.hitCamera;
}



void CX2GUElsword_SwordMan::DashJumpComboX2LandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	//ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE( 10.f, ESSI_REVENGE_WAIT )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}


	CommonEventProcess();
}

//ESSI_KNIGHT_DASH_JUMP_COMBO_X2
void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_JUMP_COMBO_X2_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.23f ) )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_JUMP_COMBO_X2_FrameMove()
{
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.15f )
	{
		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataNow.stateParam.bResetCamera	= false;
			m_FrameDataNow.unitCondition.bHit		= true;
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_JUMP_COMBO_X2_EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST



	if( m_InputData.oneX == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	if( m_InputData.oneZ == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( ESSI_DASH_JUMP_COMBO_X3 );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ESSI_COMBO_Z1 );
		}
		else
		{
			StateChange( ESSI_DASH_JUMP_COMBO_X2_LANDING );
		}
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_JUMP_COMBO_X2_End()
{
	CommonStateEnd();
	UpCrashCamera( 200.0f, -3500.0f );
}

//ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a
void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_FrameMoveFuture()
{
	if( m_pXSkinAnim->GetNowAnimationTime() < 0.39f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	CommonFrameMoveFuture();
}




void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		if( FlushMp( _CONST_ELSWORD_::MAGIC_FIRE_BALL_MP_COST ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();

			angle.z = -30.0f;
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL_DOWN_ELSWORD", GetPowerRate() * m_fAttackPowerRateFireBall, posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.z = -30.0f;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, 1, 1 );
			if( pSeq != NULL )
			{
				pSeq->SetAxisAngle( GetRotateDegree() );
				pSeq->SetAddRotate( GetRotateDegree() );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.59f )
	{
		StateChange( USI_JUMP_DOWN );
	}

	CommonEventProcess();
}

//ESSI_KNIGHT_DASH_JUMP_COMBO_X3
void CX2GUElsword_SwordMan::ESSI_KNIGHT_DASH_JUMP_COMBO_X3_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.68f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}


////////////////////////////////////////////////////////////////////////
//ESSI_SI_ES_POWER_COMBO
void CX2GUElsword_SwordMan::ESSI_SI_ES_POWER_ATTACK_FrameMoveFuture()
{
	m_PhysicParam.nowSpeed.y = 0.0f;

    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.2f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI_ES_POWER_ATTACK_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI_ES_POWER_ATTACK_EventProcess()
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


//ESSI_SI_ES_POWER_ATTACK_HYPER
void CX2GUElsword_SwordMan::ESSI_SI_ES_POWER_ATTACK_HYPER_FrameMoveFuture()
{
	m_PhysicParam.nowSpeed.y = 0.0f;

    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.2f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI_ES_POWER_ATTACK_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI_ES_POWER_ATTACK_HYPER_EventProcess()
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


//ESSI_SI1_ES_MEGA_SLASH
void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_Init()
{


	XSkinMeshReadyInBackground( L"Effect_ElSword_MegaSlashBackDust.x" );
	XSkinMeshReadyInBackground( L"Effect_ElSword_MegaSlash.x" );


}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.5f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_MEGA_SLASH", this );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;

	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.2f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}

}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_EventProcess()
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



//ESSI_SI1_ES_MEGA_SLASH_HYPER
void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_HYPER_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.5f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_MEGA_SLASH", this, NULL, true );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_HYPER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					if( GetIsRight() == true )
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					else
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					if( GetIsRight() == true )
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					else
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA3" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;

	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.2f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_MEGA_SLASH_HYPER_EventProcess()
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





//ESSI_SI2_ES_FLAME_GEYSER
void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_Init()
{



	XSkinMeshReadyInBackground( L"Effect_ElSword_FlameGager.x" );
	XSkinMeshReadyInBackground( L"Effect_ElSword_FlameGagerLand.x" );




}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_Start()
{
	CommonStateStart();



}

//void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_FrameMoveFuture()
//{
//	CommonFrameMoveFuture();
//}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.7666f ) )
	{

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );


		D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );


		if(GetIsRight() == true)
		{
			pos -= (50.f * GetDirVector());
		}
		else
		{
			pos += (50.f * GetDirVector());
		}

		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );
		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 20 );

			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}


		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO8 ) == true )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_FLAME_GEYSER_MEMO", this, NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ),
				true, pos, GetRotateDegree(), GetDirVector() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_FLAME_GEYSER", this, NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ),
				true, pos, GetRotateDegree(), GetDirVector() );


		if ( GetShowSmallGageAndName() )
		{
			D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( pos );
			CKTDGParticleSystem::CParticleEventSequence* pSeqKuAAang = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_STR_KUAANG ) );
			PlaySequenceByTriggerCount( pSeqKuAAang, D3DXVECTOR3( projPos.x, projPos.y, 0.0f ), 100,100, 1 );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					if( GetIsRight() == true )
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					else
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;

	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_EventProcess()
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



//ESSI_SI2_ES_FLAME_GEYSER_HYPER
void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_HYPER_Start()
{
	CommonStateStart();

}

//void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_HYPER_FrameMoveFuture()
//{
//	//if( m_FrameDataFuture.unitCondition.fStateTime >= 0.85f && m_FrameDataFuture.unitCondition.fStateTime < 1.1f )
//	//{
//	//	//m_FrameDataFuture.syncData.bFrameStop = true;
//	//}
//
//	CommonFrameMoveFuture();
//}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.7666f ) )
	{

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveRed = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_RED ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickRed = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_TICK_RED ) );


		D3DXVECTOR3 pos = GetWeaponBonePos( 0,  L"TRACE_START0", 0 );

		if(GetIsRight() == true)
		{
			pos -= (50.f * GetDirVector());
		}
		else
		{
			pos += (50.f * GetDirVector());
		}
		PlaySequenceByTriggerCount( pSeqGroundShockWaveRed, pos, 10,10, 2 );
		if( pSeqImpactTickRed != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickRed, pos, 200,200, 20 );

			pSeqImpactTickRed->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}


		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO8 ) == true )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_FLAME_GEYSER_MEMO", this, NULL, true, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ),
				true, pos, GetRotateDegree(), GetDirVector() );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_FLAME_GEYSER", this, NULL, true, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ),
				true, pos, GetRotateDegree(), GetDirVector() );


		if ( GetShowSmallGageAndName() )
		{
			D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( pos );
			CKTDGParticleSystem::CParticleEventSequence* pSeqKuAAang = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_STR_KUAANG ) );
			PlaySequenceByTriggerCount( pSeqKuAAang, D3DXVECTOR3( projPos.x, projPos.y, 0.0f ), 100,100, 1 );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_HYPER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					if( GetIsRight() == true )
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					else
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;

	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}

}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FLAME_GEYSER_HYPER_EventProcess()
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





//ESSI_SI3_ES_UNLIMITED_BLADE
void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_Init()
{



	XSkinMeshReadyInBackground( L"Effect_ElSword_MagnumBlade.X" );


}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.08f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.5f;
	}

	//1Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.166f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//2Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.433f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//3Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.666f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//4Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.966f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//5Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 1.3f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//6Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 1.566f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

    if( m_pXSkinAnimFuture->EventTimerOneshot( 2.0f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.25f;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_FrameMove()
{

	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 2 );

	ShowActiveSkillCutInAndLight( 1.9f, 2, true );

	//1Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.166f ) )
	{
		CreateStepDust();
		m_DamageData.hitUnitList.resize(0);
	}
	//2Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.433f ) )
	{
		CreateStepDust();
		m_DamageData.hitUnitList.resize(0);
	}
	//3Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		CreateStepDust();
		m_DamageData.hitUnitList.resize(0);
	}
	//4Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.966f ) )
	{
		CreateStepDust();

		m_DamageData.reActType = CX2DamageManager::RT_BIG_DAMAGE;
		m_DamageData.hitUnitList.resize(0);

	}
	//5Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		CreateStepDust();

		m_DamageData.hitUnitList.resize(0);
	}
	//6Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 1.566f ) )
	{
		CreateStepDust();
		m_DamageData.hitUnitList.resize(0);

	}


	//����Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 1.95f ) )
	{
		CreateStepDust();


		m_DamageData.hitType			= CX2DamageManager::HT_SWORD_SLASH;
		m_DamageData.reActType			= CX2DamageManager::RT_DOWN;
		// ������ ���� ���� 6.41 -> 10.68
		m_DamageData.damage.fPhysic		= 10.68f * GetPowerRate();

		m_DamageData.backSpeed.x		= GetDashSpeed() * 2.0f;
		m_DamageData.fCameraCrashGap	= 40.0f;
		m_DamageData.fCameraCrashTime	= 0.6f;
		m_DamageData.clearScreen		= 2;

		m_DamageData.hitUnitList.resize(0);
	}




    if( m_pXSkinAnim->EventTimerOneshot( 2.1f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_MAGNUM_BLADE", this );
	}




	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.2f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else if( m_pXSkinAnim->GetNowAnimationTime() < 2.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					if( GetIsRight() == true )
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					else
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA3" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;

	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.2f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else if( m_pXSkinAnim->GetNowAnimationTime() < 2.1f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}

}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_EventProcess()
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



//ESSI_SI3_ES_UNLIMITED_BLADE_HYPER
void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_Init()
{



	XSkinMeshReadyInBackground( L"Effect_ElSword_MagnumBladeFire.X" );



}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.08f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.5f;
	}

	//1Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.166f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//2Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.433f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//3Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.666f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//4Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.966f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//5Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 1.3f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	//6Ÿ
    if( m_pXSkinAnimFuture->EventTimerOneshot( 1.5f ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 2 );

	ShowActiveSkillCutInAndLight( 1.56f, 2, true );

	//1Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.166f ) )
	{
		CreateStepDust();

		m_DamageData.hitUnitList.resize(0);
	}
	//2Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.433f ) )
	{
		CreateStepDust();

		m_DamageData.hitUnitList.resize(0);
	}
	//3Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		CreateStepDust();

		m_DamageData.hitUnitList.resize(0);
	}
	//4Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 0.966f ) )
	{
		CreateStepDust();


		m_DamageData.reActType = CX2DamageManager::RT_BIG_DAMAGE;
		m_DamageData.hitUnitList.resize(0);

	}
	//5Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		CreateStepDust();

		m_DamageData.hitUnitList.resize(0);

	}
	//6Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		CreateStepDust();

		m_DamageData.hitUnitList.resize(0);

	}




	//����Ÿ
    if( m_pXSkinAnim->EventTimerOneshot( 1.73f ) )
	{

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_MAGNUM_BLADE_HYPER", this );


		UpDownCrashCamera( 20.0f, 1.0f );


		D3DXVECTOR3 pos = m_FrameDataNow.syncData.position;
		pos.y += 10.0f;


		CKTDGParticleSystem::CParticleEventSequence* pSeqMagnumBladeFireWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_MAGNUM_BLADE_FIRE_WAVE ) );
		PlaySequenceByTriggerCount( pSeqMagnumBladeFireWave, pos, 10,10, 4 );

		if ( GetShowSmallGageAndName() )
		{
			D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( GetPos() );
			CKTDGParticleSystem::CParticleEventSequence* pSeqKuAAang = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_STR_KUAANG ) );
			PlaySequenceByTriggerCount( pSeqKuAAang, D3DXVECTOR3( projPos.x, projPos.y, 0.0f ), 100,100, 1 );
		}
	}



	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.2f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else if( m_pXSkinAnim->GetNowAnimationTime() < 1.8f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					if( GetIsRight() == true )
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					else
						g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					m_LuaManager.EndTable();
				}
			}
			else if( m_pXSkinAnim->GetNowAnimationTime() < 2.5f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA3" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA4" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;

	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.2f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else if( m_pXSkinAnim->GetNowAnimationTime() < 1.8f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			else if( m_pXSkinAnim->GetNowAnimationTime() < 2.5f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA2" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA3" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}

}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_EventProcess()
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



//ESSI_SI1_ES_AIR_SLASH
void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_Init()
{
	XSkinMeshReadyInBackground( L"AirSlash01.X" );
	XSkinMeshReadyInBackground( L"AirSlash02.X" );
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.2f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"AirSlash01",
			GetPos(), GetRotateDegree(), GetRotateDegree(), XL_EFFECT_0 );
		if( NULL != pMeshInst )
		{
			m_hMeshInstAirSlash1 = pMeshInst->GetHandle();
		}
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hMeshInstAirSlash1 );
	if( NULL != pMeshInst )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 50.0f;
		pMeshInst->SetPos( pos );
		pMeshInst->SetRotateDegree( GetRotateDegree() );
		pMeshInst->SetMoveAxisAngleDegree( GetRotateDegree() );
	}
	else if( m_hMeshInstAirSlash1 != INVALID_MESH_INSTANCE_HANDLE )
	{
		m_hMeshInstAirSlash1 = INVALID_MESH_INSTANCE_HANDLE;
	}


	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_CameraMove()
{
	CommonCameraMove();


	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.7f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.37f )
	{
		StateChange( ESSI_SA_AIR_SLASH_SECOND );
	}
	CommonEventProcess();
}

//ESSI_SI1_ES_AIR_SLASH_NEXT
void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_NEXT_FrameMove()
{
	ChangeWorldColorByHyperMode();

    if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 125.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"AirSlash02", pos, GetRotateDegree(), GetRotateDegree(), XL_EFFECT_0 );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_NEXT_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.8f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}


//ESSI_SI1_ES_AIR_SLASH_HYPER
void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_HYPER_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.2f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"AirSlash01_Hyper",
			GetPos(), GetRotateDegree(), GetRotateDegree(), XL_EFFECT_0 );
		if( NULL != pMeshInst )
		{
			m_hMeshInstAirSlash1 = pMeshInst->GetHandle();
		}
	}


	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hMeshInstAirSlash1 );
	if( NULL != pMeshInst )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 50.0f;
		pMeshInst->SetPos( pos );
		pMeshInst->SetRotateDegree( GetRotateDegree() );
		pMeshInst->SetMoveAxisAngleDegree( GetRotateDegree() );
	}
	else if( m_hMeshInstAirSlash1 != INVALID_MESH_INSTANCE_HANDLE )
	{
		m_hMeshInstAirSlash1 = INVALID_MESH_INSTANCE_HANDLE;
	}


	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_HYPER_CameraMove()
{
	CommonCameraMove();


	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.7f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.37f )
	{
		StateChange( ESSI_SA_AIR_SLASH_SECOND_HYPER );
	}
	CommonEventProcess();
}




//ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT
void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT_FrameMove()
{
	ChangeWorldColorByHyperMode();

    if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 125.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"AirSlash02_Hyper", pos, GetRotateDegree(), GetRotateDegree(), XL_EFFECT_0 );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.8f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}





//ESSI_SI1_ES_ASSAULT_SLASH
void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_Init()
{
	XMeshReadyInBackground( L"Ring_Plane.y" );
	XMeshReadyInBackground( L"AssaultSlash02.Y" );
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_Start()
{
	CommonStateStart();

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO2 ) == true )
	{
		m_PhysicParam.nowSpeed.x = 1200.f;
		m_DamageData.fHitGap = 0.05f;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.37f ) )
	{
		D3DXVECTOR3 tip;
		GetFramePos( &tip, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		m_hSeqAssualtSlashRing = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"AssualtSlashRingBlue", tip );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashRing );
		if( NULL != pSeq )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}

		m_hSeqAssualtSlashDust = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"AssualtSlashDust", GetPos() );
		pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashDust );
		if( NULL != pSeq )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}
	if( m_hSeqAssualtSlashRing != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashRing );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 tip;
			GetFramePos( &tip, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
			pSeq->SetPosition( tip );
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}
	if( m_hSeqAssualtSlashDust != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashDust );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( GetPos() );
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.4f )
	{
		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.4f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.3f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.1f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}

//ESSI_SI1_ES_ASSAULT_SLASH_HYPER
void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.37f ) )
	{
		D3DXVECTOR3 tip;
		GetFramePos( &tip, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		m_hSeqAssualtSlashRing = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"AssualtSlashRingRed", tip );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashRing );
		if( NULL != pSeq )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}

		m_hSeqAssualtSlashDust = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"AssualtSlashDust", GetPos() );
		pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashDust );
		if( NULL != pSeq )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}
	if( m_hSeqAssualtSlashRing != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashRing );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 tip;
			GetFramePos( &tip, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
			pSeq->SetPosition( tip );
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}
	if( m_hSeqAssualtSlashDust != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashDust );
		if( NULL != pSeq )
		{
			pSeq->SetPosition( GetPos() );
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}


	if( m_pXSkinAnim->GetNowAnimationTime() > 0.45f )
	{
		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_HYPER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.4f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	case 2:
		{
			if( m_pXSkinAnim->GetNowAnimationTime() < 0.3f )
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					m_LuaManager.EndTable();
				}
			}
			else
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
		}
		break;
	}
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_ASSAULT_SLASH_HYPER_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.1f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}


//{{ dmlee 2009.6.2 obsolete!! ������� �ʴ� �ʻ��
////ESSI_SI2_ES_LUNA_BLADE
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_Init()
//{
//	XSkinMeshReadyInBackground( L"LunaBlade01.X" );
//	XSkinMeshReadyInBackground( L"LunaBlade02.X" );
//	XSkinMeshReadyInBackground( L"LunaBlade03.X" );
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_FrameMoveFuture()
//{
//	if( m_pXSkinAnimFuture->EventTimer( 0.48f ) == true && EventCheck( 0.48f, true ) == true )
//	{
//		//m_PhysicParam.nowSpeed.x = m_OrgPhysicParam.fRunSpeed * 1.5f;
//	}
//	CommonFrameMoveFuture();
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_FrameMove()
//{
//	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );
//
//	if( m_pXSkinAnim->EventTimer( 0.11f ) == true && EventCheck( 0.11f, false ) == true )
//	{
//		ActiveSkillShow( L"Dummy1_Rhand", s_SkillCutInSet[s_iSkillCutInSetIndex].cutIn[1].fileName.c_str(), s_SkillCutInSet[s_iSkillCutInSetIndex].cutIn[1].vSize, s_SkillCutInSet[s_iSkillCutInSetIndex].cutIn[1].vPosition, false );
//	}
//	if( m_pXSkinAnim->EventTimer( 0.433f ) == true && EventCheck( 0.433f, false ) == true )
//	{
//		g_pX2Game->GetDamageEffect()->CreateInstance( (CKTDGObject*) this,  this, L"LUNA_BLADE", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
//	}
//	if( m_pXSkinAnim->EventTimer( 0.9f ) == true && EventCheck( 0.9f, false ) == true )
//	{
//		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"LunaBlade02", GetPos(), GetRotateDegree(), GetRotateDegree() );
//	}
//	if( m_pXSkinAnim->EventTimer( 0.533f ) == true && EventCheck( 0.533f, false ) == true )
//	{
//		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"LunaBlade03", GetPos(), GetRotateDegree(), GetRotateDegree() );
//	}
//	CommonFrameMove();
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_CameraMove()
//{
//	CommonCameraMove();
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_EventProcess()
//{
//	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
//	{
//		if( false == IsOnSomethingFuture() )
//			StateChange( ESSI_JUMP_DOWN );
//		else
//			StateChange( USI_WAIT );
//	}
//	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
//	{
//		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.5f )
//		{
//			StateChange( USI_WAIT );
//		}
//	}
//	CommonEventProcess();
//}
//
//
////ESSI_SI2_ES_LUNA_BLADE_HYPER
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_HYPER_FrameMoveFuture()
//{
//	if( m_pXSkinAnimFuture->EventTimer( 0.48f ) == true && EventCheck( 0.48f, true ) == true )
//	{
//		//m_PhysicParam.nowSpeed.x = m_OrgPhysicParam.fRunSpeed * 1.5f;
//	}
//	CommonFrameMoveFuture();
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_HYPER_FrameMove()
//{
//	g_pX2Game->GetWorld()->SetWorldColor( 0xffff0000 );
//
//	if( m_pXSkinAnim->EventTimer( 0.06f ) == true && EventCheck( 0.06f, false ) == true )
//	{
//		ActiveSkillShow( L"Dummy1_Rhand", s_SkillCutInSet[s_iSkillCutInSetIndex].cutIn[1].fileName.c_str(), s_SkillCutInSet[s_iSkillCutInSetIndex].cutIn[1].vSize, s_SkillCutInSet[s_iSkillCutInSetIndex].cutIn[1].vPosition, true );
//	}
//	if( m_pXSkinAnim->EventTimer( 0.433f ) == true && EventCheck( 0.433f, false ) == true )
//	{
//		g_pX2Game->GetDamageEffect()->CreateInstance( (CKTDGObject*) this,  this, L"LUNA_BLADE_HYPER", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
//	}
//	if( m_pXSkinAnim->EventTimer( 0.9f ) == true && EventCheck( 0.9f, false ) == true )
//	{
//		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"LunaBlade02Hyper", GetPos(), GetRotateDegree(), GetRotateDegree() );
//	}
//	if( m_pXSkinAnim->EventTimer( 0.533f ) == true && EventCheck( 0.533f, false ) == true )
//	{
//		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"LunaBlade03Hyper", GetPos(), GetRotateDegree(), GetRotateDegree() );
//	}
//	CommonFrameMove();
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_HYPER_CameraMove()
//{
//	CommonCameraMove();
//}
//
//void CX2GUElsword_SwordMan::ESSI_SI2_ES_LUNA_BLADE_HYPER_EventProcess()
//{
//	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
//	{
//		if( false == IsOnSomethingFuture() )
//			StateChange( ESSI_JUMP_DOWN );
//		else
//			StateChange( USI_WAIT );
//	}
//	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
//	{
//		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.5f )
//		{
//			StateChange( USI_WAIT );
//		}
//	}
//	CommonEventProcess();
//}
//

//ESSI_SI2_ES_FATAL_FURY
//}} dmlee 2009.6.2 obsolete!! ������� �ʴ� �ʻ��
void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_Init()
{
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_FrameMoveFuture()
{
	//if( m_pXSkinAnimFuture->EventTimer( 0.48f ) == true && EventCheck( 0.48f, true ) == true )
	//{
	//	//m_PhysicParam.nowSpeed.x = m_OrgPhysicParam.fRunSpeed * 1.5f;
	//}
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.1f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		m_DamageData.hitUnitList.resize(0);

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO1 ) == true )
		{
			m_DamageData.damage.fPhysic *= 1.2f;
			m_DamageData.m_bNoDetonation = true;
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"FatalFury02", GetPos() );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		UpDownCrashCameraNoReset( 20.0f, 0.3f );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed", GetPos(), -1, -1, -1, 1 );

		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
		PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  1 );


	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.95f ) )
	{
		m_DamageData.hitUnitList.resize(0);
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		D3DXVECTOR3 down;
		GetFramePos( &down, m_vecpWeapon[0]->m_pFrame_TRACE_END[0] );
		D3DXVECTOR3 dirTip	= down - up;
		D3DXVec3Normalize( &dirTip, &dirTip );
		D3DXVECTOR3 pos = up + dirTip * 70.0f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"FatalFury02", pos );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		UpDownCrashCameraNoReset( 20.0f, 0.3f );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed", pos, -1, -1, -1, 1 );

		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
		PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  2 );
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.54f ) )
	{
		m_DamageData.hitUnitList.resize(0);
	}
    if( m_pXSkinAnim->EventTimerOneshot( 2.2f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		D3DXVECTOR3 down;
		GetFramePos( &down, m_vecpWeapon[0]->m_pFrame_TRACE_END[0] );
		D3DXVECTOR3 dirTip	= down - up;
		D3DXVec3Normalize( &dirTip, &dirTip );
		D3DXVECTOR3 pos = up + dirTip * 20.0f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"FatalFury02", pos );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		UpDownCrashCameraNoReset( 20.0f, 0.3f );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed", pos, -1, -1, -1, 1 );

		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
		PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  3 );

	}
    if( m_pXSkinAnim->EventTimerOneshot( 2.3f ) )
	{
		m_DamageData.hitUnitList.resize(0);
		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed.x = m_OrgPhysicParam.GetDashSpeed() * 1.5f;
	}
    if( m_pXSkinAnim->EventTimerOneshot( 3.0f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"FatalFury01", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_hMeshInstFatalFury = pMeshInst->GetHandle();
		}
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hMeshInstFatalFury );
	if( NULL != pMeshInst )
	{
		pMeshInst->SetPos( GetPos() );
		pMeshInst->SetRotateDegree( GetRotateDegree() );
		pMeshInst->SetMoveAxisAngleDegree( GetRotateDegree() );
	}
	else if( m_hMeshInstAirSlash1 != INVALID_MESH_INSTANCE_HANDLE )
	{
		m_hMeshInstAirSlash1 = INVALID_MESH_INSTANCE_HANDLE;
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_CameraMove()
{
	CommonCameraMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_EventProcess()
{
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO1 ) == true )
	{
		if( GetAttackSuccess(false) == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.6f )
		{
			m_bFirstAttackSuccess = true;
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f && m_bFirstAttackSuccess == false )
		{
			const CX2UserSkillTree::SkillSlotData* pSkillSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( CX2SkillTree::SI_SA_ES_FATAL_FURY );
			if ( NULL != pSkillSlotData )
			{
				float fMPConsume = pSkillSlotData->m_fMPConsumption / 2.f;
				UpNowMp( fMPConsume );

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
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 4.0f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}


//ESSI_SI2_ES_FATAL_FURY_HYPER
void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_HYPER_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.48f ) )
	{
		//m_PhysicParam.nowSpeed.x = m_OrgPhysicParam.fRunSpeed * 1.5f;
	}
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.1f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		m_DamageData.hitUnitList.resize(0);

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO1 ) == true )
		{
			m_DamageData.damage.fPhysic *= 1.2f;
			m_DamageData.m_bNoDetonation = true;
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"FatalFury02", GetPos() );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		UpDownCrashCameraNoReset( 20.0f, 0.3f );

		//g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"DropDownRed", GetPos(), GetRotateDegree(), GetRotateDegree(), XL_EFFECT_0 );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed", GetPos(), -1, -1, -1, 1 );

		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
		PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  1 );
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.95f ) )
	{
		m_DamageData.hitUnitList.resize(0);
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		D3DXVECTOR3 down;
		GetFramePos( &down, m_vecpWeapon[0]->m_pFrame_TRACE_END[0] );
		D3DXVECTOR3 dirTip	= down - up;
		D3DXVec3Normalize( &dirTip, &dirTip );
		D3DXVECTOR3 pos = up + dirTip * 70.0f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"FatalFury02", pos );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		UpDownCrashCameraNoReset( 20.0f, 0.3f );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed", pos, -1, -1, -1, 1 );

		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
		PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  2 );

	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.54f ) )
	{
		m_DamageData.hitUnitList.resize(0);
	}
    if( m_pXSkinAnim->EventTimerOneshot( 2.2f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		D3DXVECTOR3 down;
		GetFramePos( &down, m_vecpWeapon[0]->m_pFrame_TRACE_END[0] );
		D3DXVECTOR3 dirTip	= down - up;
		D3DXVec3Normalize( &dirTip, &dirTip );
		D3DXVECTOR3 pos = up + dirTip * 20.0f;

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"FatalFury02", pos );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		UpDownCrashCameraNoReset( 20.0f, 0.3f );

		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"FireDamageImpactSlashRed", pos, -1, -1, -1, 1 );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWave = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleCommonMinorParticleByEnum( COMMON_MINOR_PII_GROUND_SHOCK_WAVE ) );
		PlaySequenceByTriggerCount( pSeqGroundShockWave, m_FrameDataNow.unitCondition.landPosition.x, m_FrameDataNow.unitCondition.landPosition.y + 5.0f, m_FrameDataNow.unitCondition.landPosition.z,  5, 10,  3 );

	}
    if( m_pXSkinAnim->EventTimerOneshot( 2.3f ) )
	{
		m_DamageData.hitUnitList.resize(0);
		m_DamageData.reActType = CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed.x = m_OrgPhysicParam.GetDashSpeed() * 1.5f;
	}
    if( m_pXSkinAnim->EventTimerOneshot( 3.0f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"FatalFury01", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( NULL != pMeshInst )
		{
			m_hMeshInstFatalFury = pMeshInst->GetHandle();
		}
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMinorXMeshPlayer()->GetMeshInstance( m_hMeshInstFatalFury );
	if( NULL != pMeshInst )
	{
		pMeshInst->SetPos( GetPos() );
		pMeshInst->SetRotateDegree( GetRotateDegree() );
		pMeshInst->SetMoveAxisAngleDegree( GetRotateDegree() );
	}
	else if( m_hMeshInstAirSlash1 != INVALID_MESH_INSTANCE_HANDLE )
	{
		m_hMeshInstAirSlash1 = INVALID_MESH_INSTANCE_HANDLE;
	}


	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_FATAL_FURY_HYPER_EventProcess()
{
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO1 ) == true )
	{
		if( GetAttackSuccess(false) == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.6f )
		{
			m_bFirstAttackSuccess = true;
		}

		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f && m_bFirstAttackSuccess == false )
		{
			const CX2UserSkillTree::SkillSlotData* pSkillSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( CX2SkillTree::SI_SA_ES_FATAL_FURY );
			if ( NULL != pSkillSlotData )
			{
				float fMPConsume = pSkillSlotData->m_fMPConsumption / 2.f;
				UpNowMp( fMPConsume );

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
	}

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 4.0f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}


//ESSI_SI2_ES_SPIRAL_BLAST
void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_Init()
{
	XSkinMeshReadyInBackground( L"SpiralBlast01.X" );
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos ( L"Bip01");
		if( true == GetIsRight() )
		{
			vPos += ( 300.f * GetDirVector() );
		}
		else
		{
			vPos -= ( 300.f * GetDirVector() );
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPIRAL_BLAST", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.93f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.07f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.33f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpiralBlast01", up );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.03f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpiralBlast02", up );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}

	/*
#ifdef  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
    else if( m_pXSkinAnim->EventTimerOneshot( 1) )
#else   X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	else if( m_pXSkinAnim->EventTimer( 1.1f ) == true && EventCheck( 1.1f, false ) == true )
#endif  X2OPTIMIZE_NPC_ADAPTIVE_FRAME_MOVE
	{
		g_pX2Game->GetMinorParticle()->DestroyInstance( m_pSpiralBlastSwordFriction );
		m_pSpiralBlastSwordFriction = NULL;

		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		if( GetIsRight() == true )
			m_pSpiralBlastSwordFriction = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"SwordFrictionRight", up );
		else
			m_pSpiralBlastSwordFriction = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"SwordFrictionLeft", up );
		m_pSpiralBlastSwordFriction->SetAxisAngle( GetRotateDegree() );


	}
	if( m_pSpiralBlastSwordFriction != NULL && g_pX2Game->GetMinorParticle()->IsLiveInstance(m_pSpiralBlastSwordFriction) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		m_pSpiralBlastSwordFriction->SetPosition( up );
		m_pSpiralBlastSwordFriction->SetAxisAngle( GetRotateDegree() );
	}
	if( m_pXSkinAnim->GetNowAnimationTime() > 1.5f && m_pSpiralBlastSwordFriction != NULL )
	{
		//m_pSpiralBlastSwordFriction->SetAutoDie();
		//m_pSpiralBlastSwordFriction = NULL;
	}
	*/

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_CameraMove()
{
	CommonCameraMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 2.4f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}


//ESSI_SI2_ES_SPIRAL_BLAST_HYPER
void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos ( L"Bip01");
		if( true == GetIsRight() )
		{
			vPos += ( 300.f * GetDirVector() );
		}
		else
		{
			vPos -= ( 300.f * GetDirVector() );
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SPIRAL_BLAST", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.93f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.07f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.33f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"SpiralBlast01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		CKTDGParticleSystem::CParticleEventSequence* m_pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpiralBlast01", up );
		if( m_pSeq != NULL )
		{
			m_pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.03f ) )
	{
		D3DXVECTOR3 up;
		GetFramePos( &up, m_vecpWeapon[0]->m_pFrame_TRACE_START[0] );
		CKTDGParticleSystem::CParticleEventSequence* m_pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"SpiralBlast02", up );
		if( m_pSeq != NULL )
		{
			m_pSeq->SetAxisAngle( GetRotateDegree() );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_SPIRAL_BLAST_HYPER_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 2.4f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}


//ESSI_SI3_ES_TRIPLE_GEYSER
void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_Init()
{
	XSkinMeshReadyInBackground( L"Effect_ElSword_FlameGager.X" );
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.7666f ) )
	{
		D3DXVECTOR3 pos = GetWeaponBonePos( 0,  L"TRACE_START0", 0 );

		if(GetIsRight() == true)
		{
			pos -= (50.f * GetDirVector());
		}
		else
		{
			pos += (50.f * GetDirVector());
		}

		pos.y = GetPos().y;

		UpDownCrashCamera( 50.0f, 0.3f );

		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );


		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );
		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 10 );

			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"TRIPLE_GEYSER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( GetShowSmallGageAndName() )
		{
			D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( pos );
			CKTDGParticleSystem::CParticleEventSequence* pSeqKuAAang = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_STR_KUAANG ) );
			PlaySequenceByTriggerCount( pSeqKuAAang, D3DXVECTOR3( projPos.x, projPos.y, 0.0f ), 100,100, 1 );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.9666f ) )
	{
		D3DXVECTOR3 pos = GetWeaponBonePos( 0,  L"TRACE_START0", 0 );
		if( GetIsRight() == true )
			pos += GetDirVector() * 300.0f;
		else
			pos -= GetDirVector() * 300.0f;

		if(GetIsRight() == true)
		{
			pos -= (50.f * GetDirVector());
		}
		else
		{
			pos += (50.f * GetDirVector());
		}

		pos.y = GetPos().y;

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );



		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );
		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 10 );

			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"TRIPLE_GEYSER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.1666f ) )
	{
		D3DXVECTOR3 pos = GetWeaponBonePos( 0,  L"TRACE_START0", 0 );
		if( GetIsRight() == true )
			pos += GetDirVector() * 600.0f;
		else
			pos -= GetDirVector() * 600.0f;

		if(GetIsRight() == true)
		{
			pos -= (50.f * GetDirVector());
		}
		else
		{
			pos += (50.f * GetDirVector());
		}

		pos.y = GetPos().y;

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );


		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );
		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 10 );

			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"TRIPLE_GEYSER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_EventProcess()
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


//ESSI_SI3_ES_TRIPLE_GEYSER_HYPER
void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.7666f ) )
	{
		D3DXVECTOR3 pos = GetPos();

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );


		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );
		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 10 );

			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"TRIPLE_GEYSER_HYPER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( GetShowSmallGageAndName() )
		{
			D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( pos );
			CKTDGParticleSystem::CParticleEventSequence* pSeqKuAAang = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_STR_KUAANG ) );
			PlaySequenceByTriggerCount( pSeqKuAAang, D3DXVECTOR3( projPos.x, projPos.y, 0.0f ), 100,100, 1 );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.9666f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos += GetDirVector() * 300.0f;
		else
			pos -= GetDirVector() * 300.0f;

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );


		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );

		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 10 );
			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"TRIPLE_GEYSER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.1666f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos += GetDirVector() * 600.0f;
		else
			pos -= GetDirVector() * 600.0f;

		UpDownCrashCamera( 50.0f, 0.3f );


		CKTDGParticleSystem::CParticleEventSequence* pSeqGroundShockWaveBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE ) );
		CKTDGParticleSystem::CParticleEventSequence* pSeqImpactTickBlue = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_IMPACT_BLUE ) );




		PlaySequenceByTriggerCount( pSeqGroundShockWaveBlue, pos, 10,20, 2 );
		if( pSeqImpactTickBlue != NULL )
		{
			PlaySequenceByTriggerCount( pSeqImpactTickBlue, pos, 200,200, 10 );

			pSeqImpactTickBlue->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"TRIPLE_GEYSER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_EventProcess()
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

//ESSI_SI3_ES_ARMAGEDDON_BLADE
void CX2GUElsword_SwordMan::ESSI_SI3_ES_ARMAGEDDON_BLADE_Init()
{
	XSkinMeshReadyInBackground( L"ArmageddonBlade.x" );
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_ARMAGEDDON_BLADE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Knight_ArmageddonBlade01", pos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Knight_ArmageddonBlade02", pos );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.68f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ESK_ARMAGEDON_BLADE );
		if ( NULL != pSkillTemplet )
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO18 ) == true )
				SetBuffFactorToGameUnit( pSkillTemplet, 1 );
			else
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.7f ) )
	{
		D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );
		D3DXVECTOR3 vDir = GetDirVector();
		if( true == GetIsRight() )
			pos += 305.f * vDir + D3DXVECTOR3( 0.f, 70.f, 0.f );
		else
			pos += -305.f * vDir + D3DXVECTOR3( 0.f, 70.f, 0.f );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_WindBlade01", pos );
		if( NULL != pSeq )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_ARMAGEDDON_BLADE_EventProcess()
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

//ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER
void CX2GUElsword_SwordMan::ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.06f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Knight_ArmageddonBlade01", pos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_Knight_ArmageddonBlade02", pos );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.68f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ESK_ARMAGEDON_BLADE );
		if ( NULL != pSkillTemplet )
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO18 ) == true )
				SetBuffFactorToGameUnit( pSkillTemplet, 3 );
			else
				SetBuffFactorToGameUnit( pSkillTemplet, 2 );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.8f ) )
	{
		D3DXVECTOR3 pos = GetBonePos( L"Dummy1_Rhand" );
		D3DXVECTOR3 vDir = GetDirVector();
		if( true == GetIsRight() )
			pos += 305.f * vDir + D3DXVECTOR3( 0.f, 70.f, 0.f );
		else
			pos += -305.f * vDir + D3DXVECTOR3( 0.f, 70.f, 0.f );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_WindBlade01", pos );
		if( NULL != pSeq )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}


	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER_EventProcess()
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


//ESSI_SI1_ES_WIND_BLADE
void CX2GUElsword_SwordMan::ESSI_SI1_ES_WIND_BLADE_Init()
{
	XSkinMeshReadyInBackground( L"WindBlade01.x" );
	XSkinMeshReadyInBackground( L"WindBlade_MagicSquare01.x" );
	XSkinMeshReadyInBackground( L"WindBlade_MagicSquare02.x" );
	XSkinMeshReadyInBackground( L"WindBlade_MagicSquare03.x" );
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_WIND_BLADE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.067f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.08f ) )
	{
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_WindBlade01_Mesh", GetPos(), GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_WindBlade02_Mesh", GetPos(), GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_WindBlade03_Mesh", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO10 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"WIND_BLADE_MEMO", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"WIND_BLADE", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 vDir = GetDirVector();
		if( true == GetIsRight() )
			pos += 200.f * vDir + D3DXVECTOR3( 0.f, 140.f, 0.f );
		else
			pos += -200.f * vDir + D3DXVECTOR3( 0.f, 140.f, 0.f );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_WindBlade01", pos );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_WIND_BLADE_EventProcess()
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


//ESSI_SI3_ES_WIND_BLADE_HYPER
void CX2GUElsword_SwordMan::ESSI_SI1_ES_WIND_BLADE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.067f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.08f ) )
	{
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_WindBlade01_Mesh", GetPos(), GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_WindBlade02_Mesh", GetPos(), GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_WindBlade03_Mesh", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO10 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"WIND_BLADE_MEMO_HYPER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"WIND_BLADE_HYPER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 vDir = GetDirVector();
		if( true == GetIsRight() )
			pos += 200.f * vDir + D3DXVECTOR3( 0.f, 140.f, 0.f );
		else
			pos += -200.f * vDir + D3DXVECTOR3( 0.f, 140.f, 0.f );

		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_WindBlade01", pos );
		if( pSeq != NULL )
		{
			pSeq->SetAxisAngle( GetRotateDegree() );
			pSeq->SetAddRotate( GetRotateDegree() );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI1_ES_WIND_BLADE_HYPER_EventProcess()
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


//ESSI_SI2_ES_RISING_SLASH
void CX2GUElsword_SwordMan::ESSI_SI2_ES_RISING_SLASH_Init()
{
	XSkinMeshReadyInBackground( L"RisingSlash_Blade01.x" );
	XSkinMeshReadyInBackground( L"RisingSlash_MagicSquare01.x" );
	XMeshReadyInBackground( L"Arme_ChargeMp2.Y" );

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
	{
		XSkinMeshReadyInBackground( L"RisingSlash_Blade01_Memo.x" );
		XSkinMeshReadyInBackground( L"RisingSlash_MagicSquare01_Memo.x" );
	}
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_RISING_SLASH_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.1f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LizardMan_Older_PowerBooster01", GetPos() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 vec = GetDirVector();
		pos = GetPos() + vec * 170.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		pos = GetPos() - vec * 170.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		pos = GetPos() + vec * 420.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		pos = GetPos() - vec * 420.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
		{
			pos = GetPos() + vec * 670.f;
			pos.y = m_FrameDataNow.unitCondition.landPosition.y;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
			pos = GetPos() - vec * 670.f;
			pos.y = m_FrameDataNow.unitCondition.landPosition.y;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.12f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 2.0f;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_RisingSlash01_Mesh", pos, GetRotateDegree(), GetRotateDegree() );
		CX2DamageEffect::CEffect *pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"RISING_BLADE", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		if( pEffect != NULL && GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
		{
			pEffect->GetDamageData().fHitGap = 0.03f;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) &&
        GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
	{
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 vec = GetDirVector();
		float oy = pos.y;
		float fScalePos = 670.f * GetScaleByUnit().x;

		pos = GetPos() + vec * fScalePos;
		pos.y = oy + 2.0f;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_RisingSlash01_Mesh_Memo", pos, GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"RISING_BLADE_MEMO", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		pos = GetPos() - vec * fScalePos;
		pos.y = oy + 2.0f;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_RisingSlash01_Mesh_Memo", pos, GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"RISING_BLADE_MEMO", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_RISING_SLASH_EventProcess()
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

//ESSI_SI2_ES_RISING_SLASH_HYPER
void CX2GUElsword_SwordMan::ESSI_SI2_ES_RISING_SLASH_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.1f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LizardMan_Older_PowerBooster01", GetPos() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 vec = GetDirVector();
		pos = GetPos() + vec * 170.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		pos = GetPos() - vec * 170.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		pos = GetPos() + vec * 420.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		pos = GetPos() - vec * 420.0f;
		pos.y = m_FrameDataNow.unitCondition.landPosition.y;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
		{
			pos = GetPos() + vec * 670.f;
			pos.y = m_FrameDataNow.unitCondition.landPosition.y;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
			pos = GetPos() - vec * 670.f;
			pos.y = m_FrameDataNow.unitCondition.landPosition.y;
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_MagicKnight_RisingSlash01", pos );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.12f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 2.0f;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_RisingSlash01_Mesh", pos, GetRotateDegree(), GetRotateDegree() );
		CX2DamageEffect::CEffect *pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"RISING_BLADE_HYPER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if( pEffect != NULL && GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
		{
			pEffect->GetDamageData().fHitGap = 0.03f;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) &&
        GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO4 ) == true )
	{
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 vec = GetDirVector();
		float oy = pos.y;

		pos = GetPos() + vec * 670.0f;
		pos.y = oy + 2.0f;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_RisingSlash01_Mesh_Memo", pos, GetRotateDegree(), GetRotateDegree() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"RISING_BLADE_MEMO_HYPER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		pos = GetPos() - vec * 670.0f;
		pos.y = oy + 2.0f;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Elsword_MagicKnight_RisingSlash01_Mesh_Memo", pos, GetRotateDegree(), GetRotateDegree() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"RISING_BLADE_MEMO_HYPER", GetPowerRate(), pos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI2_ES_RISING_SLASH_HYPER_EventProcess()
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


//ESSI_SI3_ES_STORM_BLADE
void CX2GUElsword_SwordMan::ESSI_SI3_ES_STORM_BLADE_Init()
{
	XSkinMeshReadyInBackground( L"StormBlade_Blade01.x" );
	XSkinMeshReadyInBackground( L"StormBlade_MagicSquare00.x" );
	XSkinMeshReadyInBackground( L"StormBlade_SmallBlade01.x" );
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_STORM_BLADE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.2f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI3_ES_STROM_BLADE", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_STORM_BLADE_EventProcess()
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

//ESSI_SI3_ES_STORM_BLADE_HYPER
void CX2GUElsword_SwordMan::ESSI_SI3_ES_STORM_BLADE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.2f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI3_ES_STROM_BLADE_HYPER", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI3_ES_STORM_BLADE_HYPER_EventProcess()
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
/*virtual*/ void CX2GUElsword_SwordMan::COMMON_BUFF_FrameMove()
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

void CX2GUElsword_SwordMan::ESSI_LEAP_StartFuture()
{
	m_bLeaping = true;
	m_fElapsedTimeToLeap = 0.0f;

	int npcUID = g_pX2Game->GetNearestNPCUnitUID( GetPos(), -1 );
	if ( npcUID == 0 )
		StateChange( m_CommonState.m_Wait );

	CX2GUNPC* pGUNPC = g_pX2Game->GetNPCUnitByUID( npcUID );
	if ( pGUNPC == NULL )
		StateChange( m_CommonState.m_Wait );

	m_fTimeToLeap = 1.0f;

	m_StartPosToLeap = GetPos();


	D3DXVECTOR3 tempPos = pGUNPC->GetPos() - GetPos();
	//ĳ���� ��ġ�� ������ġ��� ���� ���Ǿ��� �ٶ󺼱�?

	D3DXVec3Normalize( &m_DirVecToLeap, &tempPos );

	float fHypotenuseLength = D3DXVec3Length( &tempPos );

	tempPos.y = 0;
	float fBaseLineLength = D3DXVec3Length( &tempPos );

	float radian = acosf( fBaseLineLength / fHypotenuseLength );
	float degree = D3DXToDegree( radian );
	degree = abs( degree );
	//�������� �ٶ� ���Ǿ��� �����ΰ�?

	//���� ���ư� ����?
	int degreeToLeap = 0;
	if ( degree >= 45 )
	{
		degreeToLeap = 45 - (int)degree;
	}
	else
	{
		degreeToLeap = 45;
	}

	if ( degreeToLeap < 10 )
		degreeToLeap = 10;


	//float fSpeedToLeap = sqrt( m_fHypotenuseLength * LEAP_GRAVITY / sin( D3DXToRadian(degreeToLeap*2) ) );

	//�ӷ��� �󸶷�?
	//float fSpeedToLeap = (m_fTimeToLeap / 2.0f) * LEAP_GRAVITY / sin( D3DXToRadian(degreeToLeap) );

	/*
	float fSpeedToLeap = ( (2.0f * LEAP_GRAVITY * m_fTimeToLeap/2.0f) +
		sqrt( pow( (2.0f*LEAP_GRAVITY*m_fTimeToLeap/2.0f), 2 ) - ( 4.0f * pow( sin( D3DXToRadian(degreeToLeap) ), 2 ) * pow( LEAP_GRAVITY, 2 ) * pow( m_fTimeToLeap/2.0f, 2 ) ) ) ) /
		( 2 * pow( sin( D3DXToRadian(degreeToLeap) ), 2 ) );
*/
	//�ӽ÷� ���� 100 ����..



	//m_fVerticalSpeedToLeap = sqrt( 2.0f * LEAP_GRAVITY * 100 );
	//m_fVerticalSpeedToLeap = LEAP_GRAVITY * pow( m_fTimeToLeap / 2.0f, 2 );


	m_fParallelSpeedToLeap =  fBaseLineLength / m_fTimeToLeap;

	//�ִ밡 45���̰� �ּҰ� 10���ε�.. ����.. ��ϸ� �����?
	m_fHeightToLeap = fHypotenuseLength/3.0f;
	m_fHeightToLeap *= degreeToLeap/45.0f;
	//m_fVerticalSpeedToLeap = sin( D3DXToRadian(degreeToLeap) ) * fSpeedToLeap;
	m_fVerticalSpeedToLeap = 2.0f * m_fHeightToLeap / pow( m_fTimeToLeap/2.0f, 2 ) * (m_fTimeToLeap/2.0f); // g = ( 2.0f * 100.0f / pow( m_fTimeToLeap/2.0f, 2 ) );



	m_DegreeToLeap = (int)degree;

}

void CX2GUElsword_SwordMan::ESSI_LEAP_FrameMoveFuture()
{
	CommonFrameMoveFuture();

	m_fElapsedTimeToLeap += m_fElapsedTime;

	//m_FrameDataFuture.syncData.position.y = m_StartPosToLeap.y + ( ( m_fVerticalSpeedToLeap * m_fElapsedTimeToLeap ) - ( 0.5f * /*LEAP_GRAVITY*/ ( 2.0f * m_fHeightToLeap / pow( m_fTimeToLeap/2.0f, 2 ) ) * pow( m_fElapsedTimeToLeap, 2 ) )  );

	//m_FrameDataFuture.syncData.position.x = ( ( m_fVerticalSpeedToLeap * m_UpVectorToLeap.x * m_fElapsedTimeToLeap ) - ( 0.5f * /*LEAP_GRAVITY*/ ( 2.0f * m_fHeightToLeap / pow( m_fTimeToLeap/2.0f, 2 ) ) * pow( m_fElapsedTimeToLeap, 2 ) )  );
	//m_FrameDataFuture.syncData.position.z = ( ( m_fVerticalSpeedToLeap * m_UpVectorToLeap.z * m_fElapsedTimeToLeap ) - ( 0.5f * /*LEAP_GRAVITY*/ ( 2.0f * m_fHeightToLeap / pow( m_fTimeToLeap/2.0f, 2 ) ) * pow( m_fElapsedTimeToLeap, 2 ) )  );

	m_FrameDataFuture.syncData.position = m_StartPosToLeap;


	//�������� �޸��� �κ� ���غ���?
	D3DXVECTOR3 tempPos;
	tempPos = m_DirVecToLeap;
	tempPos.y = 0;
	D3DXVec3Normalize( &tempPos, &tempPos );
	tempPos = ( /*tempPos*/m_DirVecToLeap * m_fElapsedTimeToLeap * m_fParallelSpeedToLeap);
	float fHeight = tempPos.y;

	m_FrameDataFuture.syncData.position.x += tempPos.x;
	m_FrameDataFuture.syncData.position.z += tempPos.z;
	m_FrameDataFuture.syncData.position.y += fHeight;

	D3DXVECTOR3 vecHeight = D3DXVECTOR3(0,0,0);
	vecHeight.y = ( ( m_fVerticalSpeedToLeap * m_fElapsedTimeToLeap ) - ( 0.5f * /*LEAP_GRAVITY*/ ( 2.0f * m_fHeightToLeap / pow( m_fTimeToLeap/2.0f, 2 ) ) * pow( m_fElapsedTimeToLeap, 2 ) )  );


	//x���̳� z������ �󸶳� ��Ʋ������ ���ư��°���?
	D3DXMATRIX matTrans;
	D3DXMatrixIdentity( &matTrans );
	D3DXMatrixRotationY( &matTrans, D3DXToRadian( 90 ) );
	D3DXVECTOR3 tempDirVec;	//�ٶ󺸴� �Žñ�� ������ �Ǵ� ���⺤�� ���� �̰� 90���� ������ ������?
	D3DXVec3TransformNormal( &tempDirVec, &m_DirVecToLeap, &matTrans );
	D3DXVec3Normalize( &tempDirVec, &tempDirVec );

	D3DXMatrixIdentity( &matTrans );
	D3DXMatrixRotationAxis( &matTrans, &tempDirVec, D3DXToRadian( m_DegreeToLeap ) );
	D3DXVec3TransformCoord( &vecHeight, &vecHeight, &matTrans );

	m_FrameDataFuture.syncData.position += vecHeight;

	m_FrameDataFuture.unitCondition.dirDegree = m_DirVecToLeap;
	m_FrameDataFuture.unitCondition.dirVector = m_DirVecToLeap;





}

void CX2GUElsword_SwordMan::ESSI_LEAP_EventProcess()
{
	/*
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( m_CommonState.m_Wait );
	}
	*/

	if ( m_fElapsedTimeToLeap >= m_fTimeToLeap )
		StateChange( m_CommonState.m_Wait );

	//Ư�� �ð� �Ŀ� ��ǥ ��ġ�� ���� �� ����Ʈ ������� ���߰���?? ��~~?

	CommonEventProcess();

}
void CX2GUElsword_SwordMan::ESSI_LEAP_End()
{
	m_bLeaping = false;
	m_fElapsedTimeToLeap = 0.0f;
}

void CX2GUElsword_SwordMan::ESSI_SI_A_ES_KICK_Start()
{
	CommonStateStart();


	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO7 ) == true )
	{
		m_NowStateData.m_fPowerRate *= 1.1f;
		m_DamageData.damage.fPhysic	*= 1.1f;
		m_DamageData.damage.fMagic	*= 1.1f;
		m_DamageData.m_bNoDetonation = true;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ES_KICK );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		    return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_DamageData.fForceDownValue = -pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_ABS, iSkillTempletLevel );
	}
}


void CX2GUElsword_SwordMan::ESSI_SI_A_ES_KICK_EventProcess()
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.5f )
		{
			//m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}












void CX2GUElsword_SwordMan::StandUpAttackFrontNoDownEventProcess()
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

void CX2GUElsword_SwordMan::StandUpAttackFrontNoDownEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}




void CX2GUElsword_SwordMan::StandUpAttackBackNoDownEventProcess()
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

void CX2GUElsword_SwordMan::StandUpAttackBackNoDownEnd()
{
	CommonStateEnd();
	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}





void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_Init()
{
	XSkinMeshReadyInBackground( L"Spin_Liner03.X" );
	XSkinMeshReadyInBackground( L"Spin_Liner_Fire02.X" );
}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}


void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		m_hWindMill		= g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_WINDMILL", this, NULL, false, GetPowerRate() );
		m_hWindMillLand = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_WINDMILL_LAND", this );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ESSI_SA_ESK_WINDMILL );
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_StateEnd()
{
	if( NULL != m_pSoundWindMill )
	{
		m_pSoundWindMill->Stop();
		m_pSoundWindMill->Set3DPosition( GetPos() );
		m_pSoundWindMill->Play( true );
	}

	m_bDisableGravity = false;
	CommonStateEnd();
}







void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_HYPER_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}


void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		m_hWindMill		= g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_WINDMILL", this, NULL, true, GetPowerRate() );
		m_hWindMillLand = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_WINDMILL_LAND", this );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ESSI_SA_ESK_WINDMILL );
	}

	CommonEventProcess();
}




void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_START_HYPER_StateEnd()
{

	if( NULL != m_pSoundWindMill )
	{
		m_pSoundWindMill->Stop();
		m_pSoundWindMill->Set3DPosition( GetPos() );
		m_pSoundWindMill->Play( true );
	}

	m_bDisableGravity = false;
	CommonStateEnd();
}




//void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_FrameMoveFuture()
//{
//	if( m_InputData.pureRight == true )
//	{
//		m_FrameDataFuture.syncData.bIsRight = true;
//		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
//	}
//	else if( m_InputData.pureLeft == true )
//	{
//		m_FrameDataFuture.syncData.bIsRight = false;
//		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
//	}
//	else
//	{
//		m_PhysicParam.nowSpeed.x = 0.0f;
//	}

//	CommonFrameMoveFuture();
//}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_FrameMove()
{
	if( NULL != m_pSoundWindMill )
	{
		m_pSoundWindMill->Set3DPosition( GetPos() );
	}
	if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		// �� ������ ���۸���, ������ ���� ������ �ɷ��ִ��� Ȯ���ϰ� �ɷ����� �ʴٸ� �ɾ��ش�.
		if ( false == HaveThisBuff ( BTI_BUFF_SA_ESK_WINDMILL ) )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ELK_WINDMILL );
			if ( NULL != pSkillTemplet )
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_EventProcess()
{
	if( m_FrameDataFuture.unitCondition.fStateTime > m_fWindMillDuration )
	{
		StateChange( ESSI_SA_ESK_WINDMILL_END );
	}
	else
	{
		if( true == IsOnSomethingFuture() )
		{
			if( m_InputData.pureUp == true )
			{
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();
				m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
			}
			else if( m_InputData.oneDown == true
				&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
				&m_FrameDataFuture.unitCondition.landPosition,
				&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
			{
				m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
			}
			else
			{
				m_PhysicParam.nowSpeed.y = 0.f;
			}
		}


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
		else
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
		}

	}


	CommonEventProcess();

}


void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_StateEnd()
{
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMill );
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMillLand );

	if( NULL != m_pSoundWindMill )
	{
		m_pSoundWindMill->Stop();
	}

	CommonStateEnd();
}




void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_HYPER_FrameMove()
{
	if( NULL != m_pSoundWindMill )
	{
		m_pSoundWindMill->Set3DPosition( GetPos() );
	}


	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_HYPER_EventProcess()
{


	if( m_FrameDataFuture.unitCondition.fStateTime > m_fWindMillDuration )
	{
		StateChange( ESSI_SA_ESK_WINDMILL_END_HYPER );
	}
	else
	{
		if( true == IsOnSomethingFuture() )
		{
			if( m_InputData.pureUp == true )
			{
				m_PhysicParam.nowSpeed.y = GetJumpSpeed();
				m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
			}
			else if( m_InputData.oneDown == true
				&& g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
				&m_FrameDataFuture.unitCondition.landPosition,
				&m_FrameDataFuture.syncData.lastTouchLineIndex ) == true )
			{
				m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
			}
			else
			{
				m_PhysicParam.nowSpeed.y = 0.f;
			}
		}


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
		else
		{
			m_PhysicParam.nowSpeed.x = 0.0f;
		}
	}


	CommonEventProcess();
}


void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_HYPER_StateEnd()
{
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMill );
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hWindMillLand );

	if( NULL != m_pSoundWindMill )
	{
		m_pSoundWindMill->Stop();
	}

	CommonStateEnd();
}





void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_END_FrameMove()
{
	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_END_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.267f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_END_HYPER_FrameMove()
{
	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_SA_ESK_WINDMILL_END_HYPER_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.267f )
		{
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_Init()
{
	XSkinMeshReadyInBackground( L"Elsword_SI_SA_Sword_Fire_Mesh01.X" );
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.3f )
	{
		m_bDisableGravity = false;
		m_PhysicParam.fDownAccel = m_PhysicParam.fGAccel * 2.f;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_hSwordFire = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_SWORDFIRE_NODAMAGE", this );
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( true == IsOnSomethingFuture() )
			StateChange( ESSI_SA_EMK_SWORD_FIRE_LANDING );
	}
	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_EndFuture()
{
	m_bDisableGravity = false;
	m_PhysicParam.fDownAccel = 0.f;

	CommonStateEndFuture();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_HYPER_Init()
{
	XSkinMeshReadyInBackground( L"Elsword_SI_SA_Sword_Fire_Mesh01.X" );
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_HYPER_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_HYPER_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.3f )
	{
		m_bDisableGravity = false;
		m_PhysicParam.fDownAccel = m_PhysicParam.fGAccel * 2.f;
	}

	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		m_hSwordFire = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_SWORDFIRE_NODAMAGE", this );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( true == IsOnSomethingFuture() )
			StateChange( ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER );
	}
	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_HYPER_EndFuture()
{
	m_bDisableGravity = false;
	m_PhysicParam.fDownAccel = 0.f;

	CommonStateEndFuture();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_LANDING_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hSwordFire );
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_LANDING_EventProcess()
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

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_LANDING_End()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EMK_SWORD_FIRE );
	SetBuffFactorToGameUnit( pSkillTemplet, 0 );

	CommonStateEnd();
}


void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.02f ) )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hSwordFire );
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_EventProcess()
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

void CX2GUElsword_SwordMan::ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_End()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EMK_SWORD_FIRE );
	SetBuffFactorToGameUnit( pSkillTemplet, 0 );

	CommonStateEnd();
}







void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_Init()
{
	XMeshReadyInBackground( L"Armor_Break_Impact01.Y" );
}

void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_StateStart()
{
	CommonStateStart();

	const CX2SkillTree::SkillTemplet* pSkillTemplet
		= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ESK_ARMOR_BREAK );

	if ( NULL != pSkillTemplet )
	{
		/// �޸� �������
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		if ( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO11 ) && pSkillTemplet->m_vecBuffFactorPtr.size() > 1 )
			m_DamageData.PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[1]->GetClonePtr( iSkillTempletLevel ) );
		else
			m_DamageData.PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
	}
}


void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.22f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elsword_Armor_Break", this );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_EventProcess()
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


void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_StateEnd()
{
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_HYPER_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.22f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Elsword_Armor_Break", this );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_HYPER_EventProcess()
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

void CX2GUElsword_SwordMan::ESSI_A_ESK_ARMOR_BREAK_HYPER_StateEnd()
{
	CommonStateEnd();
}



//ESSI_SA_EMK_BIG_BURST
void CX2GUElsword_SwordMan::ESSI_SA_EMK_BIG_BURST_Init()
{
	TextureReadyInBackground( L"smoke02.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"DarkSmoke01.dds" );
}
void CX2GUElsword_SwordMan::ESSI_SA_EMK_BIG_BURST_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.2f ) )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.65f ) )
	{
		m_PhysicParam.nowSpeed.x -= 1600 * 0.8f;
	}
	CommonFrameMoveFuture();
}
void CX2GUElsword_SwordMan::ESSI_SA_EMK_BIG_BURST_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) )
	{
		D3DXVECTOR3 vPos;
		vPos = GetBonePos( L"Dummy2_Lhand" );
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO12) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMEGA_EFFECT_ELSWORD_MNK_BIG_BURST_MEMO", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMEGA_EFFECT_ELSWORD_MNK_BIG_BURST", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );
		vPos = GetBonePos( L"Dummy2_Lhand" );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_Elsword_SI_A_Big_Burst04", vPos );
		vPos = GetBonePos( L"Dummy2_Lhand" );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Flare_Elsword_SI_A_Big_Burst05", vPos );
		vPos = GetBonePos( L"Dummy2_Lhand" );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Smoke_Elsword_SI_A_Big_Burst06", vPos );
	}

	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SA_EMK_BIG_BURST_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 1.06f )
		{
			StateChange( USI_WAIT );
		}
	}
	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SA_EMK_BIG_BURST_End()
{
	CommonStateEnd();
}







// 2�� ����
void CX2GUElsword_SwordMan::ESSI_ELK_ZZZfrontZ_FrameMove()
{
	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_ELK_ZZZfrontZ_EventProcess()
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
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.867f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ELK_XXX_FrameMove()
{
	CommonFrameMove();

}




void CX2GUElsword_SwordMan::ESSI_ELK_XXX_EventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_( 0, 0.5f, 0.5f, m_InputData.oneX == true, ESSI_ELK_XXXX )
	SKILL_CANCEL_AFTER( 0.67f )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();

}

void CX2GUElsword_SwordMan::ESSI_ELK_XXXX_FrameMove()
{
	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_ELK_XXXX_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.3f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}



void CX2GUElsword_SwordMan::ESSI_ERS_XZ_StateStartFuture()
{
	CommonStateStartFuture();

	m_bERS_COMBO_XZ_Rune_Created = false;
}

void CX2GUElsword_SwordMan::ESSI_ERS_XZ_FrameMoveFuture()
{

    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.3f ) )
	{
		if( GetNowMp() > _CONST_ELSWORD_::MAGIC_ERS_XZ_MP_COST )
		{
			m_bERS_COMBO_XZ_Rune_Created = true;
		}
		else
		{
			m_bERS_COMBO_XZ_Rune_Created = false;
		}
	}


	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::ESSI_ERS_XZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		if( FlushMp( _CONST_ELSWORD_::MAGIC_ERS_XZ_MP_COST ) == true )
		{
			D3DXVECTOR3 vScale( m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent );
			m_hRuneSlayerComboXZ = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ERS_COMBO_XZ", this, NULL, false, -1.f, -1.f, vScale );
			if ( CX2EffectSet::EffectSetInstance* pInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( m_hRuneSlayerComboXZ ) )
			{
				pInstance->SetPowerRateScale( m_fAttackPowerRateFireBall );
				pInstance->m_fLifeTime *= m_fRuneProjectileRangeIncPercent;
				pInstance->m_bDamageEffectScale = true;
			}


			if( false == IsMyUnit() )
			{
				m_bERS_COMBO_XZ_Rune_Created = true;
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, 1, 1 );
			if( pSeq != NULL )
			{
				pSeq->SetAxisAngle( angle );
				pSeq->SetAddRotate( angle );
			}


			if( false == IsMyUnit() )
			{
				m_bERS_COMBO_XZ_Rune_Created = false;
			}
		}
	}

	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_ERS_XZ_EventProcess()
{


	CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_( 0, 0.5f, 0.5f, m_InputData.oneZ == true, ESSI_ERS_XZZ )
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.8f )
		{
			StateChange( USI_WAIT );
		}
	}


	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ERS_XZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if( FlushMp( _CONST_ELSWORD_::MAGIC_ERS_XZZ_MP_COST ) == true &&
			true == m_bERS_COMBO_XZ_Rune_Created )
		{
			D3DXVECTOR3 vScale( m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent );
			CX2EffectSet::Handle hComboXZZ = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ERS_COMBO_XZZ", this, NULL, false, -1.f, -1.f, vScale );

			if ( CX2EffectSet::EffectSetInstance* pInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hComboXZZ ) )
			{
				pInstance->SetPowerRateScale( m_fAttackPowerRateFireBall );
				pInstance->m_fLifeTime *= m_fRuneProjectileRangeIncPercent;
				pInstance->m_bDamageEffectScale = true;
			}

			m_vecEffectSetToDeleteOnDamageReact.push_back( hComboXZZ );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, 1, 1 );
			if( pSeq != NULL )
			{
				pSeq->SetAxisAngle( angle );
				pSeq->SetAddRotate( angle );
			}

		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.38f ) )
	{
		if( INVALID_EFFECTSET_HANDLE != m_hRuneSlayerComboXZ )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hRuneSlayerComboXZ );
		}
	}

	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_ERS_XZZ_EventProcess()
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

	CommonEventProcess();
}




void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZ_StateStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.nowSpeed.y = 0.f;
	m_bDisableGravity = true;

	m_bERS_DASH_JUMP_XZ_Rune_Created = false;
}

void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZ_StateEndFuture()
{
	m_bDisableGravity = false;

	CommonStateEndFuture();
}

void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZ_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.26f ) )
	{
		if( GetNowMp() > _CONST_ELSWORD_::MAGIC_ERS_DASH_JUMP_XZ_MP_COST )
		{
			m_bERS_DASH_JUMP_XZ_Rune_Created = true;
		}
		else
		{
			m_bERS_DASH_JUMP_XZ_Rune_Created = false;
		}
	}



	CommonFrameMoveFuture();
}


void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZ_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.26f ) )
	{
		if(	FlushMp( _CONST_ELSWORD_::MAGIC_ERS_DASH_JUMP_XZ_MP_COST ) == true )
		{
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
			posR.y += -90.f;
			if( true == GetIsRight() )
			{
				posR += GetDirVector() * 160.f;
			}
			else
			{
				posR -= GetDirVector() * 160.f;
			}
			D3DXVECTOR3 angle = GetRotateDegree();

			D3DXVECTOR3 vSize( m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent );
            CX2DamageEffect::CEffect*
			m_pRuneSlayerDashJumpXZ = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_RUNE_SLAYER_DASH_JUMP_XZ",
				GetPowerRate() * m_fAttackPowerRateFireBall, posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fRuneProjectileRangeIncPercent );
            m_hRuneSlayerDashJumpXZ = ( m_pRuneSlayerDashJumpXZ != NULL ) ? m_pRuneSlayerDashJumpXZ->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
			if( NULL != m_pRuneSlayerDashJumpXZ )
			{
				m_pRuneSlayerDashJumpXZ->SetScale( vSize );
			}
			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_DASH_JUMP_XZ", this, NULL, false, -1.f, -1.f, vSize );
			if ( CX2EffectSet::EffectSetInstance* pEffectInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
			{
				pEffectInstance->m_fLifeTime *= m_fRuneProjectileRangeIncPercent;
				pEffectInstance->m_bDamageEffectScale = true;
			}




			if( false == IsMyUnit() )
			{
				m_bERS_DASH_JUMP_XZ_Rune_Created = true;
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.z = -30.0f;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, 1, 1 );
			if( pSeq != NULL )
			{
				pSeq->SetAxisAngle( angle );
				pSeq->SetAddRotate( angle );
			}


			if( false == IsMyUnit() )
			{
				m_bERS_DASH_JUMP_XZ_Rune_Created = false;
			}
		}
	}
	//else if( m_pXSkinAnim->IsAnimationEnd() == true )
	//{
	//	m_bDisableGravity = false;
	//}

	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZ_EventProcess()
{
	//CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );

	if( m_InputData.oneZ == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( ESSI_ERS_DASH_JUMP_XZZ );
		}
		else /*if( m_FrameDataFuture.unitCondition.fStateTime > 0.56f )*/
		{
			StateChange( USI_JUMP_DOWN );
		}
	}


	CommonEventProcess();
}





void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		float fMagicERSDashJumpXZZMpCost = 0.f;
		if( GetWayOfSwordState() == WSS_VIGOR )
		{
			fMagicERSDashJumpXZZMpCost = _CONST_ELSWORD_::VIGOR_ERS_DASH_JUMP_XZZ_MP_COST;
		}
		else
		{
			fMagicERSDashJumpXZZMpCost = _CONST_ELSWORD_::MAGIC_ERS_DASH_JUMP_XZZ_MP_COST;
		}

		if( FlushMp( fMagicERSDashJumpXZZMpCost ) == true &&
			true == m_bERS_DASH_JUMP_XZ_Rune_Created )
		{
            if ( m_hRuneSlayerDashJumpXZ != INVALID_DAMAGE_EFFECT_HANDLE )
                g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hRuneSlayerDashJumpXZ );

			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
			posR.y += -90.f;
			if( true == GetIsRight() )
			{
				posR += GetDirVector() * 160.f;
			}
			else
			{
				posR -= GetDirVector() * 160.f;
			}

			D3DXVECTOR3 angle = GetRotateDegree();
			angle.z = -30.0f;

			CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_RUNE_SLAYER_DASH_JUMP_XZZ", GetPowerRate() * m_fAttackPowerRateFireBall, posR, D3DXVECTOR3(0,0,0), angle, m_FrameDataNow.unitCondition.landPosition.y, false,-1.f, 1.f, m_fRuneProjectileRangeIncPercent );
			if( NULL != pEffect )
			{
				pEffect->SetScale( m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent, m_fRuneProjectileSizeIncPercent );
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.z = -30.0f;
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, 1, 1 );
			if( pSeq != NULL )
			{
				pSeq->SetAxisAngle( angle );
				pSeq->SetAddRotate( angle );
			}
		}
	}

	CommonFrameMove();

}

void CX2GUElsword_SwordMan::ESSI_ERS_DASH_JUMP_XZZ_EventProcess()
{

//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST



	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_( 0, m_fAttackCancelAfter, m_fAttackCancelAfter, m_InputData.oneZ == true, ESSI_JUMP_ATTACK_Z )
	ELSE_IF_STATE_CHANGE_ON_( 1, m_fAttackCancelAfter, m_fAttackCancelAfter, m_InputData.oneX == true, ESSI_JUMP_ATTACK_X )

	CommonEventProcess();

}


void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_Init()
{
}

void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_StateStart()
{
	CommonStateStart();
}


void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_StateEnd()
{
	CommonStateEnd();
}







void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_HYPER_StateStart()
{
	CommonStateStart();
}


void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

	CommonFrameMove();
}


void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_HYPER_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SA_ELK_SONIC_BLADE_HYPER_StateEnd()
{
	CommonStateEnd();
}
















void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_Init()
{
}

void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_StateStart()
{
	D3DXVECTOR3 vSize( m_fRuneProjectileSizeIncPercent,m_fRuneProjectileSizeIncPercent,m_fRuneProjectileSizeIncPercent);
	CX2EffectSet::Handle hEffectExplosion = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_SPLASH_EXPLOSION", this, NULL,false,-1.f, -1.f, vSize );
	if ( CX2EffectSet::EffectSetInstance* pEffectInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectExplosion ) )
	{
		pEffectInstance->m_fLifeTime *= m_fRuneProjectileRangeIncPercent;
		pEffectInstance->m_bDamageEffectScale = true;
	}

	CommonStateStart();
}


void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_StateEnd()
{
	CommonStateEnd();
}







void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_StateStart()
{
	CommonStateStart();
}


void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

	CommonFrameMove();
}




void CX2GUElsword_SwordMan::ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_StateEnd()
{
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_Init()
{
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_StateStart()
{
	CommonStateStart();
}


void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ESSI_A_ELK_ROLLING_SMASH );
	}

	CommonEventProcess();
}




void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_StateEnd()
{
	CommonStateEnd();
}




void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_StateStart()
{
	CommonStateStart();
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_FrameMove()
{
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ESSI_A_ELK_ROLLING_SMASH_HYPER );
	}

	CommonEventProcess();
}


void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_StateEnd()
{
	CommonStateEnd();
}










void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_StateStartFuture()
{
	CommonStateStartFuture();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ELK_ROLLING_SMASH );
	ASSERT( NULL != pSkillTemplet );
	if( NULL != pSkillTemplet )
	{
		m_fRollingSmashDuration = 2.f * m_PhysicParam.nowSpeed.y / m_PhysicParam.fGAccel;

		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_PhysicParam.passiveSpeed.x = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TELEPORT_RANGE_ABS, iSkillTempletLevel ) *
										100.f / m_fRollingSmashDuration;
	}
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_FrameMove()
{
	CommonFrameMove();

}


void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_EventProcess()
{
	CommonEventProcess();

	if( m_FrameDataFuture.unitCondition.fStateTime > m_fRollingSmashDuration &&
		true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_A_ELK_ROLLING_SMASH_LANDING );
	}
}

void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_End()
{
	CommonStateEnd();

}





void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_HYPER_StateStartFuture()
{
	CommonStateStartFuture();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ELK_ROLLING_SMASH );
	ASSERT( NULL != pSkillTemplet );
	m_fRollingSmashDuration = 2.f * m_PhysicParam.nowSpeed.y / m_PhysicParam.fGAccel;

	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_PhysicParam.passiveSpeed.x = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TELEPORT_RANGE_ABS, iSkillTempletLevel ) *
										100.f / m_fRollingSmashDuration;
	}

}





void CX2GUElsword_SwordMan::ESSI_A_ELK_ROLLING_SMASH_HYPER_EventProcess()
{
	CommonEventProcess();

	if( m_FrameDataFuture.unitCondition.fStateTime > m_fRollingSmashDuration &&
		true == IsOnSomethingFuture() )
	{
		StateChange( ESSI_A_ELK_ROLLING_SMASH_LANDING_HYPER );
	}
}

















void CX2GUElsword_SwordMan::ESSI_A_ERS_SWORD_ENCHANT_Init()
{
	XSkinMeshReadyInBackground( L"Elsword_SI_A_RS_Sword_Enchant_Mesh01.X" );
	TextureReadyInBackground( L"Condense_Pulse02.dds" );
	TextureReadyInBackground( L"Lire_RingLight01.dds" );
	TextureReadyInBackground( L"Critical_State01.dds" );
}

void CX2GUElsword_SwordMan::ESSI_A_ERS_SWORD_ENCHANT_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.333f ) )
	{
		int iRandBuffIndex = (GetRandomInt() % 3);
		if( NULL != GetUnit() && GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO19) == true )
		{
			iRandBuffIndex += 3;
			const int iSkillLevel = max( GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ERS_SWORD_ENCHANT ), 1 );
			SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_SWORD_ENCHANT_CHARGE_MP, iSkillLevel );
		}

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ERS_SWORD_ENCHANT );
		SetBuffFactorToGameUnit( pSkillTemplet, iRandBuffIndex );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_A_ERS_SWORD_ENCHANT_EventProcess()
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

	void CX2GUElsword_SwordMan::ESSI_A_ESK_PHOENIX_TALON_Init()
	{
		XSkinMeshReadyInBackground( L"Elsword_SI_SA_Phoenix_Talon_Mesh01.X" );
		XSkinMeshReadyInBackground( L"Elsword_SI_SA_Phoenix_Talon_Mesh02.X" );
		TextureReadyInBackground( L"Gate_Light02_Gray.dds" );
	}

	void CX2GUElsword_SwordMan::ESSI_A_ESK_PHOENIX_TALON_StateStart()
	{
		CommonStateStart();

		// to do.

	}

	void CX2GUElsword_SwordMan::ESSI_A_ESK_PHOENIX_TALON_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( 0.002f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 1.75f ) )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EMK_PHOENIX_TALON );
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}

		CommonFrameMove();
	}

	void CX2GUElsword_SwordMan::ESSI_A_ESK_PHOENIX_TALON_Hyper_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( 0.002f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 1.75f ) )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EMK_PHOENIX_TALON );
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}

		CommonFrameMove();
	}

	void CX2GUElsword_SwordMan::ESSI_A_ESK_PHOENIX_TALON_EventProcess()
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

	void CX2GUElsword_SwordMan::ESSI_A_ESK_PHOENIX_TALON_StateEnd()
	{
		CommonStateEnd();
	}

	void CX2GUElsword_SwordMan::ESSI_SA_ESK_DOUBLE_SLASH_FrameMove ()
	{
		ShowActiveSkillCutInAndLight( 0.002f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.004f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_DOUBLE_SLASH", this, NULL, false, GetPowerRate() );
		}

		if( m_pXSkinAnim->GetNowAnimationTime() <= 3.667f )
		{
			ChangeWorldColorByHyperMode();

			g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );

		}

    if( m_pXSkinAnim->EventTimerOneshot( 2.3666f ) )
		{
			//	���
            CX2DamageEffect::CEffect* m_pDoubleSlash = NULL;
			if( GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO17) == true )
				m_pDoubleSlash = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DOUBLE_SLASH_MEMO", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree() );
			else
			m_pDoubleSlash = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DOUBLE_SLASH", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree() );
            m_hDoubleSlash = ( m_pDoubleSlash != NULL ) ? m_pDoubleSlash->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;

			if( m_pDoubleSlash != NULL )
			{
				if( GetIsRight() == false )
				{
					m_pDoubleSlash->SetScale( D3DXVECTOR3( 1.0f, 1.0f, -1.0f ) );
				}
			}
		}

        if ( CX2DamageEffect::CEffect* pDoubleShash = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hDoubleSlash ) )
		{
			//	���� �����̸� ���� Ʋ��
			D3DXVECTOR3 vBone = GetBonePos(L"Bip01");


			//{{ ����� : [2009/10/21] //	���� ���� ���� �߰�
			D3DXVECTOR3 vDir = GetDirVector();
			if( GetIsRight() == true )
			{
				//vBone += D3DXVECTOR3( -250.0f, 20.0f, 0.0f );

				vBone.x = vBone.x + (-250.0f * vDir.x);
				vBone.y = vBone.y + 20.0f *	vDir.y;
				vBone.z = vBone.z + 0.0f * vDir.z;
			}
			else
			{
				//vBone += D3DXVECTOR3( 250.0f, 20.0f, 0.0f );

				vBone.x = vBone.x + 250.0f * vDir.x;
				vBone.y = vBone.y + 20.0f *	vDir.y;
				vBone.z = vBone.z + 0.0f * vDir.z;
			}

			pDoubleShash->SetPos(vBone);
		}

		//	���� �Ӽ� ����
    if( m_pXSkinAnim->EventTimerOneshot( 1.9f ) )
		{
			m_DamageData.m_ExtraDamage.m_ExtraDamageType = CX2DamageManager::EDT_NONE;
		}

		CommonFrameMove();
	}

	void CX2GUElsword_SwordMan::ESSI_SA_ESK_DOUBLE_SLASH_EventProcess ()
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
			if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 3.667f )
			{
				StateChange( USI_WAIT );
			}
		}


		CommonEventProcess();
	}

	void CX2GUElsword_SwordMan::ESSI_SA_ESK_DOUBLE_SLASH_StateEnd()
	{
        m_hDoubleSlash = INVALID_DAMAGE_EFFECT_HANDLE;

		CommonStateEnd();
	}

	void CX2GUElsword_SwordMan::ESSI_SA_ESK_DOUBLE_SLASH_HYPER_FrameMove ()
	{
		ShowActiveSkillCutInAndLight( 0.002f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.004f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ELSWORD_DOUBLE_SLASH_HYPER", this, NULL, false, GetPowerRate() );
		}

		if( m_pXSkinAnim->GetNowAnimationTime() <= 3.667f )
		{
			ChangeWorldColorByHyperMode();
			g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );
		}

    if( m_pXSkinAnim->EventTimerOneshot( 2.3666f ) )
		{
			//	���
            CX2DamageEffect::CEffect* m_pDoubleSlash = NULL;
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO17) == true )
					m_pDoubleSlash = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DOUBLE_SLASH_MEMO", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree() );
				else
			m_pDoubleSlash = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DOUBLE_SLASH", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree() );
            m_hDoubleSlash = ( m_pDoubleSlash != NULL ) ? m_pDoubleSlash->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
			if( m_pDoubleSlash != NULL )
			{
				if( GetIsRight() == false )
				{
					m_pDoubleSlash->SetScale( D3DXVECTOR3( 1.0f, 1.0f, -1.0f ) );
				}
			}
		}

        if ( CX2DamageEffect::CEffect* pDoubleShash = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hDoubleSlash ) )
		{
			//	���� �����̸� ���� Ʋ��
			D3DXVECTOR3 vBone = GetBonePos(L"Bip01");


			//{{ ����� : [2009/10/21] //	���� ���� ���� �߰�
			D3DXVECTOR3 vDir = GetDirVector();
			if( GetIsRight() == true )
			{
				//vBone += D3DXVECTOR3( -250.0f, 20.0f, 0.0f );

				vBone.x = vBone.x + (-250.0f * vDir.x);
				vBone.y = vBone.y + 20.0f *	vDir.y;
				vBone.z = vBone.z + 0.0f * vDir.z;
			}
			else
			{
				//vBone += D3DXVECTOR3( 250.0f, 20.0f, 0.0f );

				vBone.x = vBone.x + 250.0f * vDir.x;
				vBone.y = vBone.y + 20.0f *	vDir.y;
				vBone.z = vBone.z + 0.0f * vDir.z;
			}

			pDoubleShash->SetPos(vBone);
		}

		//	���� �Ӽ� ����
    if( m_pXSkinAnim->EventTimerOneshot( 1.9f ) )
		{
			m_DamageData.m_ExtraDamage.m_ExtraDamageType = CX2DamageManager::EDT_NONE;
		}

		CommonFrameMove();
	}


//{{ kimhc // 2010.11.2 // �ε峪��Ʈ - ���� ����

void CX2GUElsword_SwordMan::ReducePain( CX2SkillTree::SKILL_TYPE eSkillType_, const IN CX2UserSkillTree& cUserSkillTree_ )
{
	// ������� ��ų�� �ִ��� �Ǵ�
	const CX2SkillTree::SkillTemplet* pSkillTempletReducePain = cUserSkillTree_.GetUserSkillTemplet( CX2SkillTree::SI_P_ELK_REDUCE_PAIN );

	if ( NULL == pSkillTempletReducePain )
		return;

	if ( NULL == GetUnit() )
		return;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTempletReducePain->m_eID, true ) );	/// ��ų ����

	// ����� ��ų Ÿ�Կ� ���� ������� �ߵ� Ȯ���� �ٸ��� ����
	float fRateReducePain = pSkillTempletReducePain->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillTempletLevel );

	switch ( eSkillType_ )
	{
	case CX2SkillTree::ST_SPECIAL_ACTIVE:	// ����� ��Ƽ���� ��� Ȯ�� 2���!
		fRateReducePain *= 2.0f;
		break;

	default:
		break;
	}

	// ���� ���� �ߵ�
	if ( GetRandomFloat() < fRateReducePain )
	{
		SpecialAttackNoStageChange( pSkillTempletReducePain );
	}
}

//}} kimhc // 2010.11.2 // �ε峪��Ʈ - ���� ����




//{{ oasis907 : ����� [2010.11.5] // �ε� ����Ʈ - ���� ����
void CX2GUElsword_SwordMan::ESSI_SA_ELK_SAND_STORM_FrameMoveFuture()
{
    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.3f ) )
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.2f;


    if( m_pXSkinAnimFuture->EventTimerOneshot( 0.9333f ) )
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 0.8f;

	CommonFrameMoveFuture();
}


void CX2GUElsword_SwordMan::ESSI_SA_ELK_SAND_STORM_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1333f ) )
	{
		D3DXVECTOR3 DamageEffectPos = GetPos();

		DamageEffectPos.y += 300.f;

		if(GetIsRight() == true)
		{
			DamageEffectPos += (245.f * GetDirVector() * GetScaleByUnit().x );
		}
		else
		{
			DamageEffectPos -= (245.f * GetDirVector() * GetScaleByUnit().x );
		}

		CX2DamageEffect::CEffect* pDE = NULL;
		pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"LORD_KNIGHT_SAND_STORM", GetPowerRate(), DamageEffectPos, GetRotateDegree(), GetRotateDegree() );
		//pDE->SetScale(GetScale());
		if( NULL != pDE )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ELK_SAND_STORM );
			if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				pDE->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SA_ELK_SAND_STORM_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.f )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

//}} oasis907 : ����� [2010.11.5] // �ε� ����Ʈ - ���� ����



void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZ_StateStart()
{
	CommonStateStart();
	m_hSecondBladeSheath = INVALID_MESH_INSTANCE_HANDLE;
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZ_FrameMove()
{


    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ElswordSheath02", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( pMeshInst != NULL )
		{
			m_hSecondBladeSheath = pMeshInst->GetHandle();
			pMeshInst->SetBoundingRadius( 0.0f );
		}


	}

    if( m_pXSkinAnim->EventTimerOneshot( m_fDashCancelAfter ) )
	{
		m_hSecondBladeSheath = INVALID_MESH_INSTANCE_HANDLE;
	}




    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZ_EventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 0 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	SKILL_CANCEL_AFTER( 0.01f )
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, ESSI_ETK_COMBO_ZZZZ )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZ_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}

	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZ_EventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if( m_InputData.pureDoubleRight == true || m_InputData.pureDoubleLeft == true )
	{
		StateChangeDashIfPossible();
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, ESSI_ETK_COMBO_ZZZZZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZ_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZZ_StateStart()
{
	CommonStateStart();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZZ_EventProcess()
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
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_ZZZZZ_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XX_StateStart()
{
	m_iETKComboXXXLoopCount = 0;
	CommonStateStart();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ElswordSheath02", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( pMeshInst != NULL )
		{
			m_hSecondBladeSheath = pMeshInst->GetHandle();
			pMeshInst->SetBoundingRadius( 0.0f );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( m_fDashCancelAfter ) )
	{
		m_hSecondBladeSheath = INVALID_MESH_INSTANCE_HANDLE;
	}

    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XX_EventProcess()
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
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneDoubleX == true, ESSI_ETK_COMBO_XXX_LOOP )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true, ESSI_ETK_COMBO_XXX_END )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XX_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_LOOP_StateStart()
{
	m_iETKComboXXXLoopCount++;
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_LOOP_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_LOOP_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ESSI_ETK_COMBO_XXX_END );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true && m_iETKComboXXXLoopCount < 3, ESSI_ETK_COMBO_XXX_LOOP )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneZ == true, ESSI_ETK_COMBO_XXXZ )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_LOOP_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXXZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXXZ_EventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXXZ_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_END_StateStart()
{
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}
void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_END_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_END_EventProcess()
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
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ETK_COMBO_XXX_END_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_ETK_DASH_COMBO_ZZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ElswordSheath02", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( pMeshInst != NULL )
		{
			m_hSecondBladeSheath = pMeshInst->GetHandle();
			pMeshInst->SetBoundingRadius( 0.0f );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( m_fDashCancelAfter ) )
	{
		m_hSecondBladeSheath = INVALID_MESH_INSTANCE_HANDLE;
	}


    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime2 ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_LAST" );
			m_LuaManager.EndTable();
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_ETK_DASH_COMBO_ZZ_EventProcess()
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
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_ETK_DASH_COMBO_ZZ_StateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_StateStart()
{
	CommonStateStart();
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ES_ROLLING );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_fRollingHpConsumeRel	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::HP_CONSUME_REL, iSkillTempletLevel );
		m_fRollingSpeedRel		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TELEPORT_SPEED_REL, iSkillTempletLevel );
	}
}

void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_StateStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = m_fRollingSpeedRel * 1200.f;
}


void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_FrameMove()
{

	CommonFrameMove();
}


void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( true == IsOnSomethingFuture() )
			StateChange( ESSI_SI_A_ES_ROLLING_END );
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_END_StateStart()
{
	CommonStateStart();
	float fUpRate = GetMaxHp() * m_fRollingHpConsumeRel;
	UpHPIfAlive( -fUpRate );
}

void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_END_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.03333f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ES_ROLLING", this );
	}

	CommonFrameMove();
}


void CX2GUElsword_SwordMan::ESSI_SI_A_ES_ROLLING_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_HARSH_CHASER_StateStart()
{
	CommonStateStart();
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ETK_HARSH_CHASER );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_fHarshChaserProjectiveRange = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO13 ) == true )
			m_fHarshChaserProjectiveRange *= 1.1f;
	}

}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_HARSH_CHASER_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		if(GetIsRight() == true)
		{
			vDamageEffectPos += (30.f * GetDirVector());
		}
		else
		{
			vDamageEffectPos -= (30.f * GetDirVector());
		}

        CX2DamageEffect::CEffect* m_pDamageEffectHarshChaserBlade = NULL;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO13 ) == true )
			m_pDamageEffectHarshChaserBlade = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_HASHCHASER_BLADE_MEMO", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
		else
			m_pDamageEffectHarshChaserBlade = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_HASHCHASER_BLADE", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree() );
        m_hDamageEffectHarshChaserBlade = ( m_pDamageEffectHarshChaserBlade != NULL )
            ? m_pDamageEffectHarshChaserBlade->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
	}


    if ( m_hDamageEffectHarshChaserBlade != INVALID_DAMAGE_EFFECT_HANDLE )
	{
    if( m_pXSkinAnim->EventTimerOneshot( ( (m_fSkillCancelAfter - 0.3f) * m_fHarshChaserProjectiveRange ) + 0.3f ) == true  )
		{
	        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffectHarshChaserBlade );
		}
	}

	CommonFrameMove();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_HARSH_CHASER_EventProcess()
{
	if( m_pXSkinAnim->GetNowAnimationTime() > 0.4f && m_pXSkinAnim->GetNowAnimationTime() < ( (m_fSkillCancelAfter - 0.3f) * m_fHarshChaserProjectiveRange ) + 0.3f )
	{
        if ( m_hDamageEffectHarshChaserBlade != INVALID_DAMAGE_EFFECT_HANDLE )
        {
            if ( false == g_pX2Game->GetDamageEffect()->IsLiveInstanceHandle( m_hDamageEffectHarshChaserBlade ) )
            {
                m_hDamageEffectHarshChaserBlade = INVALID_DAMAGE_EFFECT_HANDLE;
				StateChange( ESSI_SI_SA_ETK_HARSH_CHASER_PULL );
            }
        }
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_HARSH_CHASER_End()
{
    if ( m_hDamageEffectHarshChaserBlade != INVALID_DAMAGE_EFFECT_HANDLE )
	    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffectHarshChaserBlade );
	CommonStateEnd();
}



void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_HARSH_CHASER_PULL_FrameMove()
{
	CommonFrameMove();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_HARSH_CHASER_PULL_EventProcess()
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

	CommonEventProcess();
}



void CX2GUElsword_SwordMan::CompactCounterAttackStart()
{
	CommonStateStart();
	CreateStepDust();

	int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER, true );

	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER );

		if( NULL != pSkillTemplet )
		{
			m_DamageData.damage.fPhysic *= pSkillTemplet->GetSkillPowerRateValue( iSkillLevel );
		}
	}
}

void CX2GUElsword_SwordMan::CompactCounterAttackStartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}



void CX2GUElsword_SwordMan::CompactCounterAttackFrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::CompactCounterAttackFrameMove()
{
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO5 ) == true &&
    m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		SetDefenceMemoTime(4.f);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.09f ) )
	{

		D3DXVECTOR3 pos = m_FrameDataNow.syncData.position;
		pos.y += 70.0f;
		if( m_FrameDataNow.syncData.bIsRight == true )
			pos = pos + m_FrameDataNow.unitCondition.dirVector * 100.0f;
		else
			pos = pos + m_FrameDataNow.unitCondition.dirVector * -100.0f;

		CKTDGParticleSystem::CParticleEventSequence* pSeqRevengeWait = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PII_REVENGE_WAIT ) );

		if( pSeqRevengeWait != NULL )
		{
			PlaySequenceByTriggerCount( pSeqRevengeWait, pos, 10,10, 5 );

			pSeqRevengeWait->SetAddRotate( D3DXVECTOR3( 0.0f, m_FrameDataNow.unitCondition.dirDegree.y + 70.0f, 0.0f ) );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ElswordSheath02", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( pMeshInst != NULL )
		{
			m_hSecondBladeSheath = pMeshInst->GetHandle();
			pMeshInst->SetBoundingRadius( 0.0f );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( m_fDashCancelAfter ) )
	{
		m_hSecondBladeSheath = INVALID_MESH_INSTANCE_HANDLE;
	}

    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );

			int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER, true );

			if( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_COMPACT_COUNTER );

				if( NULL != pSkillTemplet )
				{
					m_DamageData.damage.fPhysic *= pSkillTemplet->GetSkillPowerRateValue( iSkillLevel );
				}
			}

			m_LuaManager.EndTable();
		}
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::CompactCounterAttackEventProcess()
{
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
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::CompactCounterAttackStateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

//ESSI_REVENGE_ATTACK
void CX2GUElsword_SwordMan::CompactCounterGuardAttackStart()
{
	CommonStateStart();

	m_DamageData.damage.fPhysic += m_fRevengeAddDamage;

	int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ES_COUNTER );
	const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_ES_COUNTER );

	if( NULL != pSkillTemplet )
	{
		if( GetRandomInt() < pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel ) * 100.f )
		{
			m_DamageData.reActType = CX2DamageManager::RT_SMALL_DAMAGE;
			m_DamageData.backSpeed = D3DXVECTOR2( 0.f, 0.f );
		}
	}

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO5 ) == true )
	{
		SetDefenceMemoTime(4.f);
	}

}

void CX2GUElsword_SwordMan::CompactCounterGuardAttackStartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

void CX2GUElsword_SwordMan::CompactCounterGuardAttackFrameMoveFuture()
{
	if( m_FrameDataFuture.unitCondition.fStateTime >= 0.1f && m_FrameDataFuture.unitCondition.fStateTime < 0.4 )
	{
		//m_FrameDataFuture.syncData.bFrameStop = true;
	}
	CommonFrameMoveFuture();
}

void CX2GUElsword_SwordMan::CompactCounterGuardAttackFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType				= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::CompactCounterGuardAttackCameraMove()
{
	CommonCameraMove();

	if( m_FrameDataNow.stateParam.bEventFlagList[0] == false )
	{
		switch( m_FrameDataNow.stateParam.normalCamera )
		{
		case 1:
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			break;

		case 2:
			{
				if( m_LuaManager.BeginTable( "CAMERA", m_FrameDataNow.stateParam.normalCamera ) == true )
				{
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA0" );
					g_pX2Game->GetX2Camera()->PlayLuaCamera( this, m_LuaManager, "SUB_CAMERA1" );
					m_LuaManager.EndTable();
				}
			}
			break;
		}
	}

	m_FrameDataNow.stateParam.bEventFlagList[0] = true;
}

void CX2GUElsword_SwordMan::CompactCounterGuardAttackEventProcess()
{
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
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::CompactCounterGuardAttackStateEnd()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_MAELSTORM_RAGE_Init()
{
	XSkinMeshReadyInBackground( L"elsword_active_maelstromRage_sword.X" );
	TextureReadyInBackground( L"elsword_active_maelstromRage_wave.tga" );
	TextureReadyInBackground( L"elsword_active_maelstromRage_spark.DDS" );
	TextureReadyInBackground( L"elsword_active_maelstromRage_flare.DDS" );
	TextureReadyInBackground( L"elsword_active_maelstromRage_circle.dds" );
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_MAELSTORM_RAGE_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1333f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_EIS_MAELSTORM_RAGE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_fMaelStormEffectiveTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
		}

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ETK_MAELSTORM_RAGE", (CX2GameUnit*) this, NULL, false, GetPowerRate(), m_fMaelStormEffectiveTime );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		UpDownCrashCamera( 8.0f, m_fMaelStormEffectiveTime + 3.f );
	}

	CommonFrameMove();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_MAELSTORM_RAGE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_MAELSTORM_RAGE_EndFuture()
{
	m_bDisableGravity = false;
	m_PhysicParam.fDownAccel = 0.f;

	CommonStateEndFuture();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_FINAL_STRIKE_Init()
{
	XSkinMeshReadyInBackground( L"elsword_active_finalStrike_sheath.X" );
	TextureReadyInBackground( L"elsword_active_finalStrike_space.DDS" );
	TextureReadyInBackground( L"elsword_active_finalStrike_smoke.DDS" );
	TextureReadyInBackground( L"elsword_active_finalStrike_particle.DDS" );
	TextureReadyInBackground( L"elsword_active_finalStrike_core.DDS" );
	TextureReadyInBackground( L"elsword_active_finalStrike_circle.dds" );
	TextureReadyInBackground( L"elsword_active_finalStrike_boomSpark.dds" );
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_FINAL_STRIKE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.633f ) )
	{
		D3DXVECTOR3 DamageEffectPos = GetPos();

		DamageEffectPos.y += 130.f;

		if(GetIsRight() == true)
		{
			DamageEffectPos += (330.f * GetDirVector() * GetScaleByUnit().x );
		}
		else
		{
			DamageEffectPos -= (330.f * GetDirVector() * GetScaleByUnit().x );
		}

		m_vFinalStrikeBlackHolePos = DamageEffectPos;

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_FINALSTRIKE_BLACKHOLE_EFFECT", GetPowerRate(), DamageEffectPos, GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.83333f ) )
	{
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO25 ) == true )
			m_hFinalStrikeBlackHole = g_pX2Game->GetDamageEffect()->CreateInstanceHandle( this, L"ELSWORD_FINALSTRIKE_BLACKHOLE_MEMO", GetPowerRate(), m_vFinalStrikeBlackHolePos, GetRotateDegree(), GetRotateDegree() );
		else
        m_hFinalStrikeBlackHole = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
            this, L"ELSWORD_FINALSTRIKE_BLACKHOLE", GetPowerRate(), m_vFinalStrikeBlackHolePos, GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 2.53333f ) )
	{
        if ( m_hFinalStrikeBlackHole != INVALID_DAMAGE_EFFECT_HANDLE )
		    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hFinalStrikeBlackHole );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 2.4f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_FINALSTRIKE", GetPowerRate(), m_vFinalStrikeBlackHolePos, GetRotateDegree(), GetRotateDegree() );
		UpDownCrashCamera( 45.0f, 0.4f );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 2.8f ) )
	{
		UpDownCrashCamera( 8.0f, 1.0f );
	}

	CommonFrameMove();
}



void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_FINAL_STRIKE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_FINAL_STRIKE_End()
{
    if ( m_hFinalStrikeBlackHole != INVALID_DAMAGE_EFFECT_HANDLE )
	    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hFinalStrikeBlackHole );
	CommonStateEnd();
}



void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_SWORD_FALL_Init()
{
	XSkinMeshReadyInBackground( L"elsword_active_swordFall_sword01.X" );
	XSkinMeshReadyInBackground( L"elsword_active_swordFall_sword02.X" );
	XSkinMeshReadyInBackground( L"elsword_active_swordFall_spark.X" );
	XSkinMeshReadyInBackground( L"elsword_active_swordFall_downSword.X" );
	XMeshReadyInBackground( L"elsword_active_swordFall_light.Y" );
	TextureReadyInBackground( L"elsword_active_swordFall_sword.dds" );
	TextureReadyInBackground( L"elsword_active_swordFall_spark.dds" );
	TextureReadyInBackground( L"elsword_active_swordFall_light.dds" );
	TextureReadyInBackground( L"elsword_active_swordFall_downSpark.DDS" );
	TextureReadyInBackground( L"elsword_active_swordFall_crack.dds" );
	TextureReadyInBackground( L"elsword_active_crecentCut_spark.dds" );
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_SWORD_FALL_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.004f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SECOND_BLADE", this );

		D3DXVECTOR3 vEffectPos = GetBonePos( L"Bip01" );

		vEffectPos.y += ( 60.f );

		if(GetIsRight() == true)
			vEffectPos += (0.f * GetDirVector() );
		else
			vEffectPos -= (0.f * GetDirVector() );

		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M01", vEffectPos, GetRotateDegree(), GetRotateDegree() );
		m_vSwordFallSheathPos = vEffectPos;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.03333f ) )
	{
		D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M02", vEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInst != NULL )
        {
            pMeshInst->SetPerFrameSimulation( true );
		    m_hSwordFallBladeFirst = pMeshInst->GetHandle();
        }

	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.06666f ) )
	{
		D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M03", vEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInst != NULL )
        {
            pMeshInst->SetPerFrameSimulation( true );
		    m_hSwordFallBladeSecond = pMeshInst->GetHandle();
        }
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M04", vEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInst != NULL )
        {
            pMeshInst->SetPerFrameSimulation( true );
		    m_hSwordFallBladeThird = pMeshInst->GetHandle();
        }
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M05", vEffectPos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInst != NULL )
        {
            pMeshInst->SetPerFrameSimulation( true );
		    m_hSwordFallBladeFourth = pMeshInst->GetHandle();
        }
	}

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO15 ) == true )
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.05f ) )
		{
			D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M02_Memo", vEffectPos, GetRotateDegree(), GetRotateDegree() );
            if ( pMeshInst != NULL )
            {
                pMeshInst->SetPerFrameSimulation( true );
			    m_hSwordFallBladeFirst_Memo = pMeshInst->GetHandle();
            }
		}
    if( m_pXSkinAnim->EventTimerOneshot( 0.08f ) )
		{
			D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M03_Memo", vEffectPos, GetRotateDegree(), GetRotateDegree() );
            if ( pMeshInst != NULL )
            {
                pMeshInst->SetPerFrameSimulation( true );
			    m_hSwordFallBladeSecond_Memo = pMeshInst->GetHandle();
            }
		}
    if( m_pXSkinAnim->EventTimerOneshot( 0.125f ) )
		{
			D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M04_Memo", vEffectPos, GetRotateDegree(), GetRotateDegree() );
            if ( pMeshInst != NULL )
            {
                pMeshInst->SetPerFrameSimulation( true );
			    m_hSwordFallBladeThird_Memo = pMeshInst->GetHandle();
            }
		}
    if( m_pXSkinAnim->EventTimerOneshot( 0.17f ) )
		{
			D3DXVECTOR3 vEffectPos = m_vSwordFallSheathPos;
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"elsword_A_swordFall_M05_Memo", vEffectPos, GetRotateDegree(), GetRotateDegree() );
            if ( pMeshInst != NULL )
            {
                pMeshInst->SetPerFrameSimulation( true );
			    m_hSwordFallBladeFourth_Memo = pMeshInst->GetHandle();
            }
		}
	}

	float fPowerRate = GetPowerRate();


    if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFirst );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );

			vEffectPos.y += ( 42.f );

			if(GetIsRight() == true)
				vEffectPos += (-59.f * GetDirVector() );
			else
				vEffectPos -= (-59.f * GetDirVector() );

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO15 ) == true )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE1_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE1", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFirst );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.16666f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeSecond );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );

			vEffectPos.y += ( 42.f );

			if(GetIsRight() == true)
				vEffectPos += (-59.f * GetDirVector() );
			else
				vEffectPos -= (-59.f * GetDirVector() );

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO15 ) == true )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE2_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE2", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeSecond );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.33333f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeThird );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );
			vEffectPos.y += ( 42.f );

			if(GetIsRight() == true)
				vEffectPos += (-59.f * GetDirVector() );
			else
				vEffectPos -= (-59.f * GetDirVector() );

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO15 ) == true )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE3_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE3", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeThird );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFourth );
		if( NULL != pMeshInst )
		{
			D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );
			vEffectPos.y += ( 42.f );

			if(GetIsRight() == true)
				vEffectPos += (-59.f * GetDirVector() );
			else
				vEffectPos -= (-59.f * GetDirVector() );

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO15 ) == true )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE4_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE4", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFourth );
		}
	}

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO15 ) == true )
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.917f ) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFirst_Memo );
			if( NULL != pMeshInst )
			{
				D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );

				vEffectPos.y += ( 42.f );

				if(GetIsRight() == true)
					vEffectPos += (-59.f * GetDirVector() );
				else
					vEffectPos -= (-59.f * GetDirVector() );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE1_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );

				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFirst_Memo );
			}
		}

    if( m_pXSkinAnim->EventTimerOneshot( 1.083f ) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeSecond_Memo );
			if( NULL != pMeshInst )
			{
				D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );

				vEffectPos.y += ( 42.f );

				if(GetIsRight() == true)
					vEffectPos += (-59.f * GetDirVector() );
				else
					vEffectPos -= (-59.f * GetDirVector() );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE2_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );

				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeSecond_Memo );
			}
		}

    if( m_pXSkinAnim->EventTimerOneshot( 1.25f ) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeThird_Memo );
			if( NULL != pMeshInst )
			{
				D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );
				vEffectPos.y += ( 42.f );

				if(GetIsRight() == true)
					vEffectPos += (-59.f * GetDirVector() );
				else
					vEffectPos -= (-59.f * GetDirVector() );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE3_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );

				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeThird_Memo );
			}
		}

    if( m_pXSkinAnim->EventTimerOneshot( 1.416f ) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFourth_Memo );
			if( NULL != pMeshInst )
			{
				D3DXVECTOR3 vEffectPos = pMeshInst->GetBonePos( L"Dummy01" );
				vEffectPos.y += ( 42.f );

				if(GetIsRight() == true)
					vEffectPos += (-59.f * GetDirVector() );
				else
					vEffectPos -= (-59.f * GetDirVector() );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_SWORDFALL_BLADE4_MEMO", fPowerRate, vEffectPos, GetRotateDegree(), GetRotateDegree() );

				g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFourth_Memo );
			}
		}
	}

	CommonFrameMove();
}



void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_SWORD_FALL_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_SWORD_FALL_End()
{
	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstFirst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFirst );
	if( NULL != pMeshInstFirst )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFirst );
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstSecond = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeSecond );
	if( NULL != pMeshInstSecond )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeSecond );
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstThird = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeThird );
	if( NULL != pMeshInstThird )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeThird );
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstFourth = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFourth );
	if( NULL != pMeshInstFourth )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFourth );
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFirst_Memo );
	if( NULL != pMeshInstFirst )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFirst_Memo );
	}
	pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeSecond_Memo );
	if( NULL != pMeshInst )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeSecond_Memo );
	}
	pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeThird_Memo );
	if( NULL != pMeshInst )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeThird_Memo );
	}
	pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hSwordFallBladeFourth_Memo );
	if( NULL != pMeshInst )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hSwordFallBladeFourth_Memo );
	}

	CommonStateEnd();
}



void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_CRESCENT_CUT_Init()
{
	XSkinMeshReadyInBackground( L"elsword_active_crecentCut_sheath.X" );
	TextureReadyInBackground( L"elsword_active_crecentCut_trace.DDS" );
	TextureReadyInBackground( L"elsword_active_crecentCut_strike.DDS" );
	TextureReadyInBackground( L"elsword_active_crecentCut_mark.DDS" );
	TextureReadyInBackground( L"elsword_active_crecentCut_crecent.dds" );
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_CRESCENT_CUT_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		D3DXVECTOR3 DamageEffectPos = GetPos();

		DamageEffectPos.y += 80.f;

		if(GetIsRight() == true)
		{
			DamageEffectPos += (130.f * GetDirVector() );
		}
		else
		{
			DamageEffectPos -= (130.f * GetDirVector() );
		}
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CRESCENT_ONE", this );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_CRESCENT_ONE", GetPowerRate(), DamageEffectPos, GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		D3DXVECTOR3 DamageEffectPos = GetPos();

		DamageEffectPos.y += 80.f;

		if(GetIsRight() == true)
		{
			DamageEffectPos += (130.f * GetDirVector() );
		}
		else
		{
			DamageEffectPos -= (130.f * GetDirVector() );
		}
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CRESCENT_TWO", this );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_CRESCENT_TWO", GetPowerRate(), DamageEffectPos, GetRotateDegree(), GetRotateDegree() );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 1.48f ) )
	{
		D3DXVECTOR3 DamageEffectPos = GetPos();

		DamageEffectPos.y += 80.f;

		if(GetIsRight() == true)
		{
			DamageEffectPos += (130.f * GetDirVector() );
		}
		else
		{
			DamageEffectPos -= (130.f * GetDirVector() );
		}
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CRESCENT_THREE", this );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO14 ) == true && GetRandomFloat() <= 0.5f )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_CRESCENT_THREE_MEMO", GetPowerRate(), DamageEffectPos, GetRotateDegree(), GetRotateDegree() );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ELSWORD_CRESCENT_THREE", GetPowerRate(), DamageEffectPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}



void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_CRESCENT_CUT_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_CRESCENT_CUT_End()
{
	CommonStateEnd();
}


void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_PHANTOM_SWORD_Init()
{
	XSkinMeshReady( L"Elsword_SisKnight_sword.x" );
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_PHANTOM_SWORD_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.05f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstSheath = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ElswordPhantomSwordSheath01", GetPos(), GetRotateDegree(), GetRotateDegree() );
		if( pMeshInstSheath != NULL )
		{
			m_hThirdBladeSheathMesh = pMeshInstSheath->GetHandle();
		}
	}

	CKTDGXMeshPlayer::CXMeshInstance* pMeshInstSheath = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hThirdBladeSheathMesh );
	if( NULL != pMeshInstSheath)
	{
		CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );

		D3DXMATRIX matCombineMatrix;
		D3DXMatrixScaling( &matCombineMatrix, 1.3f, 1.3f, 1.3f );
		matCombineMatrix = matCombineMatrix * pMultiAnimFrame->combineMatrix;
		pMeshInstSheath->SetUseDXMatrix( matCombineMatrix );
	}


    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstSheath = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hThirdBladeSheathMesh );
		if( NULL != pMeshInstSheath )
		{
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hThirdBladeSheathMesh );
		}
	}




    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SECOND_BLADE", this );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.0333f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ETK_PHANTOM_SWORD );
		if(GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO26) == true)
			SetBuffFactorToGameUnit( pSkillTemplet, 1 );
		else
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}

	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_ETK_PHANTOM_SWORD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}





void CX2GUElsword_SwordMan::ESSI_EIS_DASH_COMBO_XX_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true, ESSI_EIS_DASH_COMBO_XXX )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_EIS_DASH_COMBO_XXX_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_EIS_DASHJUMP_COMBO_XX_EventProcess()
{
//#ifdef LINEMAP_FAST_WIND_TEST
//	IF_TIME_ELAPSED_IN_THE_AIR_THEN_STATE_CHANGE( 1.f, USI_JUMP_DOWN )
//#endif LINEMAP_FAST_WIND_TEST

	if( m_InputData.oneX == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_InputData.oneZ == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	else if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( ESSI_EIS_DASHJUMP_COMBO_XXX );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( ESSI_COMBO_Z1 );
		}
		else
		{
			StateChange( ESSI_DASH_JUMP_COMBO_X2_LANDING );
		}
	}

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_EIS_DASHJUMP_COMBO_XXX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit	= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_EIS_DASHJUMP_COMBO_XXX_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( true == IsOnSomethingFuture() )
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

void CX2GUElsword_SwordMan::ESSI_EIS_DASHJUMP_COMBO_XXX_End()
{
	SetEnableAttackBox(L"Sword", true );
	CommonStateEnd();
}

void CX2GUElsword_SwordMan::ESSI_SI_A_EIS_SPIRAL_STING_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType			= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit	= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}


	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI_A_EIS_SPIRAL_STING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_SA_EIS_SWORD_BLASTING_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( 0.01f, 0 );
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 vRotDegree = GetRotateDegree();
		D3DXVECTOR3 vDirVec = GetDirVector();
		vDirVec.y = 0.f;

		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vTargetPos = vPos;
		if( GetIsRight() == true )
		{
			vPos -= (150.f * GetScaleByUnit().x * GetDirVector() );
			vTargetPos = vTargetPos + (vDirVec * 1000.f);
		}
		else
		{
			vPos += (150.f * GetScaleByUnit().x * GetDirVector() );
			vTargetPos = vTargetPos - (vDirVec * 1000.f);
		}

		for(int i=0; i<3; ++i)
		{
			vPos.y += (100.f * GetScaleByUnit().y);

			float fReserveTime = 0.f;
			if( i == 1 )
				fReserveTime = 0.1f;
			else if( i == 2 )
				fReserveTime = 0.05f;
			CX2DamageEffect::CEffect* pDamageSwordBlasting = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this, L"ELSWORD_EIS_SWORDBLASTING_PHANTOMSWORD", GetPowerRate(), vPos,
				vRotDegree, vRotDegree, m_FrameDataNow.unitCondition.landPosition.y, true, fReserveTime  );
            m_hDamageSwordBlasting[i] = ( pDamageSwordBlasting != NULL ) ? pDamageSwordBlasting->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
			if( pDamageSwordBlasting != NULL )
				pDamageSwordBlasting->SetLockOnPos( vTargetPos );

		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.95f ) )
	{
		D3DXVECTOR3 vDirVec = GetDirVector();
		D3DXVECTOR3 vPos = GetPos();
		if( GetIsRight() == true )
		{
			vPos = vPos + (vDirVec * 100.f);
		}
		else
		{
			vPos = vPos - (vDirVec * 100.f);
		}
		vPos.y += 100.f;

		for(int i=0; i<3; ++i)
		{
            CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->GetInstance( m_hDamageSwordBlasting[i] );
            if ( pEffect != NULL && pEffect->GetMainEffect() != NULL )
            {
                pEffect->SetLockOnPos( vPos );
                m_hDamageSwordBlasting[i] = INVALID_DAMAGE_EFFECT_HANDLE;
            }
		}
	}
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SI_SA_EIS_SWORD_BLASTING_EventProcess()
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

void CX2GUElsword_SwordMan::ESSI_SI_SA_EIS_RAGE_CUTTER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_EIS_RAGE_CUTTER);
		if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
		{
			D3DXVECTOR3 vRotDegree = GetRotateDegree();
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += 10.f;

			CX2DamageEffect::CEffect *pDamageEffect
				= g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this,
				L"ELSWORD_SA_RAGECUTTER_HIT_M01", GetPowerRate(), vPos, vRotDegree, vRotDegree,
				m_FrameDataNow.unitCondition.landPosition.y );

			if ( NULL != pDamageEffect )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				pDamageEffect->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
			}
		}
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::ESSI_SI_SA_EIS_RAGE_CUTTER_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}






	//{{ JHKang / ������ / 2010/11/01 / �糪 �����̵� �߰�
	/**
		@function ESSI_SA_ERS_LUNA_BLADE_Init
		@brief ���õ� ���ҽ� �ε�
			- �޽� : �糪 �����̵�, ���� ��
	*/
	void CX2GUElsword_SwordMan::ESSI_SA_ERS_LUNA_BLADE_Init()
	{
		XSkinMeshReadyInBackground( L"Elsword_RS_DashJumpComboXa_Mesh01.X" );
		XSkinMeshReadyInBackground( L"SI_SA_ERS_LUNA_BLADE_BLADE.X" );
		XSkinMeshReadyInBackground( L"SI_SA_ERS_LUNA_BLADE_Light.X" );
		TextureReadyInBackground( L"RB_Slash01.tga" );
		TextureReadyInBackground( L"Mesh_LUNA_BLADE.tga" );
		TextureReadyInBackground( L"Lightning02_LB.tga" );
		TextureReadyInBackground( L"ColorBallBlue_LB.tga" );
	}

	/**
		@function ESSI_SA_ERS_LUNA_BLADE_FrameMove
		@brief �ִϸ��̼� �� ������ ����Ʈ ���(�߿�)
		-
	*/
	void CX2GUElsword_SwordMan::ESSI_SA_ERS_LUNA_BLADE_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( 0.06f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.075f ) )
		{
			D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Head" );
			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * -380.f * GetScaleByUnit().x;
			}
			else
			{
				vPos -= GetDirVector() * -380.f * GetScaleByUnit().x;
			}
			vPos.y += 470.f * GetScaleByUnit().y;

			CX2DamageEffect::CEffect* pEffect
				= ( IsHyperState() ? g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_ERS_LUNA_BLADE_HYPER", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y )
				: g_pX2Game->GetDamageEffect()->CreateInstance( this, L"SA_ERS_LUNA_BLADE", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y )
				);

			if ( NULL != pEffect )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_ERS_LUNA_BLADE);
				if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
				{
					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

					pEffect->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
				}
			}
		}

		CommonFrameMove();
	}

	void CX2GUElsword_SwordMan::ESSI_SA_ERS_LUNA_BLADE_EventProcess()
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
			if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 1.98f )
			{
				StateChange( USI_WAIT );
			}
		}
		CommonEventProcess();
	}

	//}} JHKang / ������ / 2010/11/01 / �糪 �����̵� �߰�

void CX2GUElsword_SwordMan::ShowActiveSkillCutInAndLight( const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ /*= false */ )
{
    if( m_pXSkinAnim->EventTimerOneshot( fTimeToShow_ ) )
	{
 		if ( GetShowCutInAndChangeWorldColor() && GetShowActiveSkillShow() )
 			ActiveSkillShow( L"Dummy1_Rhand", s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].fileName.c_str(), s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].vSize, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].vPosition, IsHyperState(), bOnlyLight_ );
 	}
}

/*virtual*/ CX2GageData* CX2GUElsword_SwordMan::CreateGageData()
{
	if ( IsMyUnit() )
	{
		const CX2ElswordGageData* pElswordGageData
			= static_cast<const CX2ElswordGageData*>( CX2GageManager::GetInstance()->GetMyGageData() );

		if ( NULL != pElswordGageData )
			return new CX2ElswordGageData( *pElswordGageData );
		else
			return new CX2ElswordGageData();
	}
	else
		return new CX2ElswordGageData();

}

bool CX2GUElsword_SwordMan::ShouldResetNotBusyTimer()
{
	if ( false == CX2GUUser::ShouldResetNotBusyTimer() )
		return false;
	else if ( ESSI_SWORD_WAIT == m_FrameDataFuture.syncData.nowState )
		return false;
	else
		return true;	// ���� �ؾ� ��
}

/*virtual*/ void CX2GUElsword_SwordMan::DeleteMinorParticle()
{
	CX2GUUser::DeleteMinorParticle();

	DeleteElswordMinorParticle();
}

/** @function : HyperModeBuffEffectStart
	@brief : ���� �� ���;��ϴ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUElsword_SwordMan::HyperModeBuffEffectStart()
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

void CX2GUElsword_SwordMan::LightingStepFrameMove()
{
	if( IsDashState() == true )		/// DASH, DASH_END, DASH_JUMP, DASH_JUMP_LANDING �� ���
	{
		m_fDashStateTime += m_fElapsedTime;
		if( m_fDashStateTime > 1.f )
		{
			const int iSkillLevelLightningStep = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_EIS_LIGHTNING_STEP, true );

			if ( iSkillLevelLightningStep > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet
					= g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_EIS_LIGHTNING_STEP );
				if ( NULL != pSkillTemplet  )
					SetBuffFactorToGameUnit( pSkillTemplet, 0 );
			}
			m_fDashStateTime = 0.f;
		}
	}
	else	/// ������ ������ ���°� �ƴ� ���
	{
		m_fDashStateTime = 0.f;	///
	}
}

/** @function : SetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUElsword_SwordMan::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_SI_P_ETK_BRUTAL_SLAYER:
		{
			m_iBrutalSlayerActivatedLevel = 1;
		} break;

	case BTI_SI_SA_EMK_SWORD_FIRE:
	case BTI_BUFF_SWORD_ENCHANT_ICE:
	case BTI_BUFF_SWORD_ENCHANT_FIRE:
	case BTI_BUFF_SWORD_ENCHANT_POISON:
		{
			m_bShowWeaponEnchantEffect = false;
			SetNeedToUpdateWeaponEffect(true);
		} break;
	default:
		break;
	}

	if( 0 < m_iSkillLevelInduranceOfRevenge )
	{
		// ������� �ɷ��������� üũ
		CX2BuffTempletPtr ptrBuffTemplet = CX2BuffTempletManager::GetInstance()->GetBuffTempletPtr( eBuffTempletId_ );
		if( NULL != ptrBuffTemplet && true == ptrBuffTemplet->IsDeBuff() && false == ptrBuffTemplet->IsWorldBuff() )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ELK_INDURANCE_OF_REVENGE );
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}

}

/** @function : UnSetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUElsword_SwordMan::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_SI_P_ETK_BRUTAL_SLAYER:
		{
			m_iBrutalSlayerActivatedLevel = 0;
		} break;
	case BTI_SI_SA_EMK_SWORD_FIRE:
	case BTI_BUFF_SWORD_ENCHANT_ICE:
	case BTI_BUFF_SWORD_ENCHANT_FIRE:
	case BTI_BUFF_SWORD_ENCHANT_POISON:
		{
			m_bShowWeaponEnchantEffect = true;
			SetNeedToUpdateWeaponEffect(true);
		} break;
	default:
		break;
	}

	m_ucSaveDebuffCount = m_ucNumOfDeBuff;

}

/*virtual*/ bool CX2GUElsword_SwordMan::CanRidingState()
{
	if ( GetNowStateID() == CX2GUUser::USI_WAIT || GetNowStateID() == CX2GUUser::USI_WALK ||
		GetNowStateID() == CX2GUUser::USI_DASH || GetNowStateID() == CX2GUUser::USI_DASH_END ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT || GetNowStateID() == CX2GUUser::USI_RIDING_WALK ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT_HABIT || GetNowStateID() == CX2GUUser::USI_RIDING_DASH ||
		GetNowStateID() == CX2GUUser::USI_RIDING_DASH_END || GetNowStateID() == CX2GUElsword_SwordMan::ESSI_SWORD_WAIT )
		return true;

	return false;
}


//-----------------------ESSI_A_ELK_IMPACT_SMASH-----------------------//
void CX2GUElsword_SwordMan::ESSI_A_ELK_IMPACT_SMASH_Init()
{
	TextureReadyInBackground( L"Explosion_Light01" );
	TextureReadyInBackground( L"Whitecircle02.dds" );
	TextureReadyInBackground( L"Whitecircle03.dds" );
	TextureReadyInBackground( L"Smoke.dds" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"Mesh_Raven_Event_AC_Upbody21_Effect.tga" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	XSkinMeshReadyInBackground( L"Effect_ElSword_FlameGagerLand.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XMeshReadyInBackground( L"Impact_Wave_Half01.Y" );
}
void CX2GUElsword_SwordMan::ESSI_A_ELK_IMPACT_SMASH_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}


//-----------------------ESSI_A_ES_SWORD_WAVE-----------------------//
void CX2GUElsword_SwordMan::ESSI_A_ES_SWORD_WAVE_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"Elsword_Sword_Wave.tga" );
	TextureReadyInBackground( L"Mesh_Raven_Event_AC_Upbody21_Effect.tga" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
}
void CX2GUElsword_SwordMan::ESSI_A_ES_SWORD_WAVE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}


//-----------------------ESSI_SI_A_ES_DEFENCE-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_READY_StartFuture()
{
	CommonStateStartFuture();

	if( NULL != m_GuardForElswordPtr )
	{
		switch( m_GuardForElswordPtr->GetSkillSlotID_Guard() )
		{
		case USI_SPECIAL_ATTACK_1:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureA );
			break;

		case USI_SPECIAL_ATTACK_2:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureS );
			break;

		case USI_SPECIAL_ATTACK_3:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureD );
			break;

		case USI_SPECIAL_ATTACK_4:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureC );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_1:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureQ );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_2:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureW );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_3:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureE );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_4:
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureR );
			break;

		default:
			ASSERT( !L"ESSI_SI_A_ES_DEFENCE_READY_StartFuture");
			m_GuardForElswordPtr->SetGuardKeyPointer( &m_InputData.pureA );
			break;
		}
	}
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_READY_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESSI_SI_A_ES_DEFENCE_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_WAIT_Start()
{
	m_bIsGuardMotion = true;
	CommonStateStart();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_WAIT_FrameMove()
{
	// ���� �Ҹ� ���� 4 -> 3
	UpNowMp( -_CONST_ELSWORD_::SKILL_DEFENCE_MP_COST_PER_SEC * m_fElapsedTime );
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_WAIT_EventProcess()
{
	int iCountToBeAbleToCheckKey = 1;

	if( iCountToBeAbleToCheckKey < m_pXSkinAnimFuture->GetNowPlayCount() )
	{
		// ���� ��� ��, MP �� ��� �����Ǹ� ���� ���� ��ȯ ����
		if( 0 >= static_cast<int> ( GetNowMp() )
			 || NULL != m_GuardForElswordPtr && false == m_GuardForElswordPtr->GetGuardKeyValue() )
		{
			StateChange( ESSI_SI_A_ES_DEFENCE_END );
		}
	}

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_END_Start()
{
	m_bIsGuardMotion = false;
	CommonStateStart();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_END_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 0.5f;

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}
	else if( LOWER_BOUND_INPUT_TIME_TO_CANCEL < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if( true == IsOnSomethingFuture() )
		{
			IF_SKILL_CANCEL_AFTER( 0.01f )
			else if( CAN_DASH_CANCEL )
			{
				StateChangeDashIfPossible();
			}
			else if( CAN_WALK_CANCEL )
			{
				StateChange( USI_WALK );
			}
		}
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_DAMAGE_FrameMove()
{
	UpNowMp(-4.f * m_fElapsedTime);
	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ES_DEFENCE_DAMAGE_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ESSI_SI_A_ES_DEFENCE_WAIT );
	}
	else if( 0.f >= GetNowMp() || 0.f >= GetNowHp() || NULL != m_GuardForElswordPtr && false == m_GuardForElswordPtr->GetGuardKeyValue() )
	{
		StateChange( ESSI_SI_A_ES_DEFENCE_END );
	}

	CommonEventProcess();
}

//-----------------------ESSI_SI_P_ESK_AUTO_GUARD-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_P_ESK_AUTO_GUARD_Start()
{
	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_IRON_WILL", this );
	int GuardDamageReducePercent = 0;
	if( NULL != m_GuardForElswordPtr )
		GuardDamageReducePercent = m_GuardForElswordPtr->GetDamageReducePercent();

	SetDamageReducePercent( GetDamageReducePercent() + GuardDamageReducePercent );
	CommonStateStart();
}
void CX2GUElsword_SwordMan::ESSI_SI_P_ESK_AUTO_GUARD_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_P_ESK_AUTO_GUARD_End()
{
	CommonStateEnd();
	int GuardDamageReducePercent = 0;
	if( NULL != m_GuardForElswordPtr )
		GuardDamageReducePercent = m_GuardForElswordPtr->GetDamageReducePercent();

	SetDamageReducePercent( GetDamageReducePercent() - GuardDamageReducePercent );
}

//-----------------------ESSI_SI_SA_EMK_RISING_SLASH_WAVE-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_SA_EMK_RISING_SLASH_WAVE_Init()
{
	XSkinMeshReadyInBackground( L"RisingSlash_MagicSquare01_memo.X" );
	XSkinMeshReadyInBackground( L"RisingSlash_Blade01_memo.x" );
	XMeshReadyInBackground( L"Arme_ChargeMp2.Y" );
	TextureReadyInBackground( L"star1.dds" );
	TextureReadyInBackground( L"Arme_Damage_Center.dds" );
	TextureReadyInBackground( L"Arme_Critical.dds" );
}
void CX2GUElsword_SwordMan::ESSI_SI_SA_EMK_RISING_SLASH_WAVE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_SA_EMK_RISING_SLASH_WAVE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.067f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO20 ) == true )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effect_EMK_RISING_SLASH_WAVE_MEMO", this, NULL, false, GetPowerRate() );
		else
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effect_EMK_RISING_SLASH_WAVE", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SI_SA_EMK_RISING_SLASH_WAVE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( 0.067f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO20 ) == true )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effect_EMK_RISING_SLASH_WAVE_MEMO", this, NULL, true, GetPowerRate() );
		else
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effect_EMK_RISING_SLASH_WAVE", this, NULL, true, GetPowerRate() );
	}

	CommonFrameMove();
}

//-----------------------ESSI_SI_A_ERS_CRITICAL_SWORD-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_CRITICAL_SWORD_Init()
{
		TextureReadyInBackground( L"Smoke.dds" );
		TextureReadyInBackground( L"Lire_RingLight01.dds" );
		TextureReadyInBackground( L"star2.dds" );
		TextureReadyInBackground( L"Arme_Damage_Ring.dds" );
		TextureReadyInBackground( L"Arme_Damage_Center.dds" );
		TextureReadyInBackground( L"Arme_Critical.dds" );
		XSkinMeshReadyInBackground( L"Elsword_SI_A_RS_Sword_Enchant_Mesh01.X" );
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_CRITICAL_SWORD_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.333f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ERS_CRITICAL_SWORD );
		if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty())
		{
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}

	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_CRITICAL_SWORD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}


//-----------------------ESSI_SI_A_ERS_RUNE_OF_FIRE-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_RUNE_OF_FIRE_Init()
{
	TextureReadyInBackground( L"Entrance_Fire01.tga" );
	TextureReadyInBackground( L"star1.dds" );
	TextureReadyInBackground( L"Arme_Damage_Center.dds" );
	TextureReadyInBackground( L"Arme_Critical.dds" );
	TextureReadyInBackground( L"Rune_MagicSquare03_R.tga" );
	TextureReadyInBackground( L"Rune_MagicSquare04_LR.dds" );
	XSkinMeshReadyInBackground( L"FRIENDSHIPOFNATURE.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	XSkinMeshReadyInBackground( L"Elsword_RS_DashJumpComboXa_Mesh01" );
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_RUNE_OF_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_RUNE_OF_FIRE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		D3DXVECTOR3 vSize( m_fRuneProjectileSizeIncPercent,m_fRuneProjectileSizeIncPercent,m_fRuneProjectileSizeIncPercent);
		CX2EffectSet::Handle hEffectRune = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_RunSlayer_RUNE_GUARD_FIRE", this, NULL,false,-1.f, -1.f, vSize );
		if ( CX2EffectSet::EffectSetInstance* pEffectInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectRune ) )
		{
			pEffectInstance->m_fLifeTime *= m_fRuneProjectileRangeIncPercent;
			pEffectInstance->m_bDamageEffectScale = true;
		}
	}

	CommonFrameMove();
}


//-----------------------ESSI_SI_A_ERS_RUNE_OF_ICE-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_RUNE_OF_ICE_Init()
{
	XSkinMeshReadyInBackground( L"Elsword_RS_DashJumpComboXa_Mesh01" );
	XSkinMeshReadyInBackground( L"FRIENDSHIPOFNATURE.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	TextureReadyInBackground( L"Rune_MagicSquare03_LB.tga" );
	TextureReadyInBackground( L"Rune_MagicSquare04_LB.tga" );
	TextureReadyInBackground( L"Ice_Crystal01.dds" );
	TextureReadyInBackground( L"star1.dds" );
	TextureReadyInBackground( L"Arme_Damage_Center.dds" );
	TextureReadyInBackground( L"Arme_Critical.dds" );
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_RUNE_OF_ICE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ERS_RUNE_OF_ICE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		D3DXVECTOR3 vSize( m_fRuneProjectileSizeIncPercent,m_fRuneProjectileSizeIncPercent,m_fRuneProjectileSizeIncPercent);
		CX2EffectSet::Handle hEffectRune = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_RunSlayer_RUNE_GUARD_ICE", this, NULL,false,-1.f, -1.f, vSize );
		if ( CX2EffectSet::EffectSetInstance* pEffectInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectRune ) )
		{
			pEffectInstance->m_fLifeTime *= m_fRuneProjectileRangeIncPercent;
			pEffectInstance->m_bDamageEffectScale = true;
		}

	}

	CommonFrameMove();
}

//-----------------------ESSI_SI_A_ETK_PIERCING_SWORD-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_A_ETK_PIERCING_SWORD_Init()
{
	XSkinMeshReadyInBackground( L"Elsword_SisKnight_sword.X" );
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ETK_PIERCING_SWORD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ETK_PIERCING_SWORD_StateStart()
{
	CommonStateStart();
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ETK_PIERCING_SWORD );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_fPiercingSwordProjectileRange = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );
	}
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ETK_PIERCING_SWORD_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		D3DXVECTOR3 vDamageEffectPos = GetPos();
		vDamageEffectPos.y += 130.f;
		if(GetIsRight() == true)
		{
			vDamageEffectPos += (30.f * GetDirVector());
		}
		else
		{
			vDamageEffectPos -= (30.f * GetDirVector());
		}
        CX2DamageEffect::CEffect* pEffect
            = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_PIERCING_SWORD", GetPowerRate(), vDamageEffectPos, GetRotateDegree(), GetRotateDegree(), 0.f, false, -1.f, 1.f, m_fPiercingSwordProjectileRange );
		if( NULL != pEffect && GetEqippedSkillMemo( CX2SkillTree::SMI_ELSWORD_MEMO24 ) )
		{
			pEffect->GetDamageData().m_bIgnoreStateDefence = true;
		}

        m_hDamageEffectPiercingSword = ( pEffect != NULL ) ? pEffect->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
	}

    if( m_hDamageEffectPiercingSword != INVALID_DAMAGE_EFFECT_HANDLE )
	{
		if( m_pXSkinAnim->EventTimerOneshot( ( (m_fSkillCancelAfter - 0.3f) * m_fPiercingSwordProjectileRange ) + 0.3f ) == true )
		{
	        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffectPiercingSword );
		}
	}

	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_SI_A_ETK_PIERCING_SWORD_End()
{
    if ( m_hDamageEffectPiercingSword != INVALID_DAMAGE_EFFECT_HANDLE )
	    g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hDamageEffectPiercingSword );
	CommonStateEnd();
}


//-----------------------ESSI_SI_A_EIS_FATAL_SLAP-----------------------//
void CX2GUElsword_SwordMan::ESSI_SI_A_EIS_FATAL_SLAP_Init()
{
	XSkinMeshReadyInBackground( L"Elsword_Sword_Wave_Slash.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"Condense_Pulse02.dds" );
	TextureReadyInBackground( L"Mesh_Raven_Event_AC_Upbody21_Effect.tga" );
}
void CX2GUElsword_SwordMan::ESSI_SI_A_EIS_FATAL_SLAP_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}


//-----------------------ESSI_A_ETK_SWORD_SHIELD-----------------------//
void CX2GUElsword_SwordMan::ESSI_A_ETK_SWORD_SHIELD_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50_Round.X" );
	TextureReadyInBackground( L"A_ETK_SWORD_SHIELD.dds" );
}
void CX2GUElsword_SwordMan::ESSI_A_ETK_SWORD_SHIELD_StateStart()
{
	g_pX2Game->GetEffectSet()->StopEffectSet( m_hEffectSwordShield );
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ETK_SWORD_SHIELD );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_fSwordShieldEffectiveTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
	}

	CommonStateStart();
}
void CX2GUElsword_SwordMan::ESSI_A_ETK_SWORD_SHIELD_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.68f ) )
	{
		m_hEffectSwordShield = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_A_ETK_SWORD_SHIELD", this, NULL, false, -1.f, m_fSwordShieldEffectiveTime );
	}

	CommonFrameMove();
}
void CX2GUElsword_SwordMan::ESSI_A_ETK_SWORD_SHIELD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}


//-----------------------ESSI_A_ESK_WEAPON_BREAK-----------------------//
void CX2GUElsword_SwordMan::ESSI_A_ESK_WEAPON_BREAK_Init()
{
	TextureReadyInBackground( L"Break_Impact01.dds" );
}
void CX2GUElsword_SwordMan::ESSI_A_ESK_WEAPON_BREAK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

/** @function : GetFinalDamage_WayOfSword
	@brief : -GetFinalDamage ��� �� �� ���Ǳ� Ÿ�Կ� ���� ������ ���� ���� �ֱ� ���� �Լ�
			 -���ҵ�� �����ý��� �ٸ� ������ �� �� �ֵ��� virtual �Լ��� ����
*/
/*virtual*/float CX2GUElsword_SwordMan::GetFinalDamage_WayOfSword( CX2DamageManager::WAY_OF_SWORD_TYPE eWayOfSwordType_ )
{
	float fDamageMagnifications = 1.f;
	if( -2 == GetWayOfSwordState() )
	{
		switch( eWayOfSwordType_ )
		{
		case CX2DamageManager::WST_DESTRUCTION_NORMAL:
			{
				fDamageMagnifications = 2.f;
			} break;
		case CX2DamageManager::WST_DESTRUCTION_ACTIVE:
		case CX2DamageManager::WST_DESTRUCTION_SPECIAL:
			{
				fDamageMagnifications = 1.5f;
			} break;
		default:
			break;
		}
	}

	return fDamageMagnifications;
}

//#pragma region SI_FS_ELK_Gigantic_Slash
void CX2GUElsword_SwordMan::ESSI_HA_ELK_Gigantic_Slash_Init()
{
	TextureReadyInBackground(L"Arme_Critical2.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"COMET_CRASHER_Explosion_02.tga");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	TextureReadyInBackground(L"Mesh_Raven_Event_AC_Upbody21_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"UnoHound_AttackB_Mesh01.X");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
}
//#pragma endregion �Ⱓƽ ������ : �ñر�

//#pragma region SI_FS_ERS_Shining_Rune_Buster
void CX2GUElsword_SwordMan::ESSI_HA_ERS_Shining_Rune_Buster_Init()
{
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"ERS_Luna_Buster_Dummy01.X");
	XSkinMeshReadyInBackground(L"Elsword_RS_DashJumpComboXa_Mesh01_2.X");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"LUNA_BLADE_Rune_02.X");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"RTW_Flame_Buster.dds");
	TextureReadyInBackground(L"Riding_Ancient_Pporu_Fireball02A.dds");
	XSkinMeshReadyInBackground(L"SI_SA_ERS_LUNA_BLADE_BLADE_02.X");
	TextureReadyInBackground(L"WhiteImage.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"elsword_Rune_Buster_Deco01.dds");
	TextureReadyInBackground(L"secretVelder_alchemystBoss_specialAttackA_outCircle_top.dds");
	TextureReadyInBackground(L"state_potion_flare.DDS");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
}

void CX2GUElsword_SwordMan::ESSI_HA_ERS_Shining_Rune_Buster_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���̴� �� ����Ʈ : �ñر�



//#pragma region SI_FS_EIS_Blade_Rain
void CX2GUElsword_SwordMan::ESSI_HA_EIS_Blade_Rain_Init()
{
	TextureReadyInBackground(L"Arme_Critical2.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"SI_SA_EIS_Blade_Rain_Sheath.X");
	TextureReadyInBackground(L"cube_light_P02.dds");
	TextureReadyInBackground(L"elsword_active_crecentCut_mark.DDS");
	TextureReadyInBackground(L"elswrod_demolition_edge.dds");
	TextureReadyInBackground(L"title_StarLight_Smash_spark.dds");
	TextureReadyInBackground(L"ColorBallGray.dds");
	XSkinMeshReadyInBackground(L"Land_Dimolisher_M01.X");
	TextureReadyInBackground(L"aisha_active_heavyPress_downCircle.dds");
}


void CX2GUElsword_SwordMan::ESSI_HA_EIS_Blade_Rain_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

//#pragma endregion �����̵� ���� : �ñر�

/*virtual*/ void CX2GUElsword_SwordMan::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ /*= false*/ )
{
	ShowActiveSkillCutInAndLight( fTimeToShow_, m_iSkillCutInSetSubIndex, bOnlyLight_ );
}
void CX2GUElsword_SwordMan::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.f, 0.1f, true );
}
void CX2GUElsword_SwordMan::CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ /*= false*/  )
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );
	//g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 1.0f );

    if( m_pXSkinAnim->EventTimerOneshot( fTime1_ ) )
	{
		ShowMinorParticleHyperModeChange();
		if( true == bSound_ )
		{
			PlaySound( L"Stickslash1.ogg" );
		}
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
			PlaySound( L"Slash.ogg" );
			PlaySound( L"Break.ogg" );
			PlaySound( L"Change.ogg" );
		}
	}
	CommonFrameMove();
}

void CX2GUElsword_SwordMan::SetSpecificValueByEnchant()
{
	//������ �γ��� ����ٸ� �ڽſ��� �ɸ� ���� �Ӽ��� üũ�ؼ� �ڽſ��� ������ �Ǵ�.
	if( m_iSkillLevelInduranceOfRevenge > 0 )
	{
		if( CX2DamageManager::EDT_NONE != GetApplyWeaponExtraDamageType() && NULL != g_pData->GetSkillTree() )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ELK_INDURANCE_OF_REVENGE );
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}
}

/*virtual*/ float CX2GUElsword_SwordMan::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );


	switch( eSkillID_ )
	{
	case CX2SkillTree::SI_A_ERS_SWORD_ENCHANT:
		{
			if( GetEqippedSkillMemo(CX2SkillTree::SMI_ELSWORD_MEMO19) == true )
				fMpConsumption -= 5.f;
		} break;

	default:
		break;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	float fMpDecreaseRate = 1.0f;
	if( NULL != pSkillTemplet )
		fMpDecreaseRate  =  g_pData->GetMyUser()->GetSelectUnit()->GetSkillMpDecreaseRate(eSkillID_, pSkillTemplet->m_eType);

	return fMpConsumption * fMpDecreaseRate;
}
