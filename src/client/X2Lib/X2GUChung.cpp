#include "stdafx.h"
#include "./X2GUChung.h"

//{{ kimhc // 2010.11.22 // 2010-12-23 New Character CHUNG

namespace _CONST_CHUNG_
{
	const float MAGIC_NUMBER_MULTIPLY_LINE_RADIUS = 1.5f;
	const int	MAX_NUMBER_OF_CSG_COMBO_DASH_JUMP_XX_MINI_GUN = 4; // XX state �� �б����� X state ���� 1���� �߻��ϱ� ������ ��ȹ�ǵ����� 1���� ���� �����ؾ���.
	const float	MAGIC_NUMBER_IRON_WILL_RENDER_EFFECT_TIME = 0.6f;
	const float MAGIC_MOBILE_MODE_LOOKAT_DISTANCE	= 350.f;
	const float MAGIC_MOBILE_MODE_CAMERA_DISTANCE	= 180.f;
	const float MAGIC_MOBILE_MODE_Z_COOLTIME		= 0.4f;
	const float MAGIC_MOBILE_MODE_X_COOLTIME		= 1.1f;
	const float MAGIC_MOBILE_MODE_X_MAX_RANGE		= 1200.f;
	const float MAGIC_MOBILE_MODE_X_MIN_RANGE		= 650.f;
	const float	MAX_TACTICAL_FIELD_RANGE			= 660.f;			/// ��Ƽ�� �ʵ� �ִ� ���� �ݰ�

	const int MAX_NUMBER_OF_RAPID_CANNON_BULLET   = 12;		// ���ǵ� ĳ���� �Ҹ� ������ �ִ� źȯ ����
	const int MIN_NUMBER_OF_RAPID_CANNON_BULLET   = 3;		// ���ǵ� ĳ���� �Ҹ� ������ �ּ� źȯ ����



	// ��Ʋ����Ʈ ������ ����Ʈ ���ۿ��� ���� ��������� �ð�
	// State ���� �ú��� ī��Ʈ ������
	const float TICK_COUNT_OF_SATELITE_RAIN_SUMMON_DAMAGE_EFFECT = 0.2f;
	const float SUMMON_TIME_OF_SATELITE_RAIN_BIG_MISSILE_BEFORE_SUMMONER_LIFE_END = 0.5f;
	const int	SATELITE_RAIN_BOMB1_SUMMON_MANY = 1;
	const int	SATELITE_RAIN_BOMB2_SUMMON_MANY = 2;
	const int	SATELITE_RAIN_BOMB3_SUMMON_MANY = 4;
	const int	SATELITE_RAIN_BOMB4_SUMMON_MANY = 3;

	// ��ƿ���� ��Ʈ����ũ�� Targeting �ϰ� �� �� �� ��Ÿ��
	const float START_TIME_OF_ARTILLERY_STRIKE_FIRE_WHEH_TARGETING_SUCCESS = 1.5f;
	// ��ƿ���� ��Ʈ����ũ �̽� ���� �� Damage Effect ���� ��
	const float GAP_TIME_OF_ARTILLERY_STRIKE_CREATE_DAMAGE_EFFECT_TIME = 0.15f;
	// ī���� ĳ��, X ��ư ���� �߻� �� ���� �ð� ���ҷ�
	const float DECREASE_TIME_OF_FAST_SHOT_CHAOS_CANNON = 0.35f;


}


static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] = /// �ʻ�� �� �� cut in image
{
    // ������
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_normal.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),//D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),//D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
	},

	//{{ kimhc // 2011.1.3 // û 1�� ����
	// FURY_GUARDIAN
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_normal.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
	},

	// SHOOTER_GUARDIAN
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_normal.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
	},
	//}} kimhc // 2011.1.3 // û 1�� ����

	//{{ JHKang / ������ / 2011.6.23 / û 2�� ����
	// IRON_PALADIN
	{
		CX2GUUser::SkillCutIn( L"DDS_Chung_CIP_Skill_Normal.dds"),// D3DXVECTOR2(625.f,625.f), D3DXVECTOR3(274.0f,486.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"DDS_Chung_CIP_Skill_Rage.dds"),//D3DXVECTOR2(625.f,625.f), D3DXVECTOR3(274.0f,486.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"DDS_Chung_CIP_Skill_Rage.dds"),//D3DXVECTOR2(625.f,625.f), D3DXVECTOR3(274.0f,486.0f,0.0f) ),
	},

	// DEADLY_CHASER
	{
		CX2GUUser::SkillCutIn( L"DDS_Chung_CDC_Skill_Normal.dds"),// D3DXVECTOR2(740.f,740.f), D3DXVECTOR3(283.f,499.f,0.0f) ),
		CX2GUUser::SkillCutIn( L"DDS_Chung_CDC_Skill_Rage.dds"),// D3DXVECTOR2(740.f,740.f), D3DXVECTOR3(283.f,499.f,0.0f) ),
		CX2GUUser::SkillCutIn( L"DDS_Chung_CDC_Skill_Rage.dds"),// D3DXVECTOR2(740.f,740.f), D3DXVECTOR3(283.f,499.f,0.0f) ),
	},

	// �߰� 1�� ���� �����
		{
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_normal.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
			CX2GUUser::SkillCutIn( L"HQ_CutIn_Chung_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(318.0f,559.0f,0.0f) ),
		},
	//}}

	/// û 2�� ���� ��Ƽ�� Ʈ����
		{
			CX2GUUser::SkillCutIn( L"DDS_Chung_CTT_Skill_Normal.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(290.0f,459.0f,0.0f) ),
			CX2GUUser::SkillCutIn( L"DDS_Chung_CTT_Skill_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(290.0f,459.0f,0.0f) ),
			CX2GUUser::SkillCutIn( L"DDS_Chung_CTT_Skill_Rage.dds"),// D3DXVECTOR2(703.0f,703.0f), D3DXVECTOR3(290.0f,459.0f,0.0f) ),
		},
};

/** @function : ������
	@brief : ��� �ʱ�ȭ �� ���� Ŭ������ �����ڿ� �μ� ����
	@param : iUnitIndex_(���� �ε���), iTeamNum_(�� �ѹ�), pWstrScriptFileName_(ĳ������ ������� ��), iFrameBufferNum_(�������� ��Ƴ��� ������ ũ��), pUnit_(��������)
*/
CX2GUChung::CX2GUChung( int iUnitIndex_, int iTeamNum_,
	lua_State*   pScriptLuaState,
	int iFrameBufferNum_, IN CX2Unit* pUnit_ )
: CX2GUUser( iUnitIndex_, iTeamNum_,
	pScriptLuaState,
iFrameBufferNum_, pUnit_ )
, m_uiCountToCallSetDamageReducePercentFuncInCommonStateStart( 0 ), m_uiCountToCallSetDamageReducePercentFuncInCommonStateEnd( 0 )
, m_bVerifyGuardSystem( true ), m_bBerserkMode( false )
, MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE(5), m_ucCannonBallCountToBeUpUsingReLoadCannon(1), m_fDamageRelByCannonBallMastery( 1.0f )
, m_ucBasicCannonBallCount(0)
//{{ kimhc // 2011.1.14 // û 1�� ����
, m_bAimingTarget(false), m_vAimingTargetPos( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_vAimingTargetLandPos( D3DXVECTOR3(0.f, 0.f, 0.f) ), m_fForceDownRelAtMeleeAttack( 1.0f )
//}} kimhc // 2011.1.14 // û 1�� ����
, m_fProbabilityBigDamageAtGuardMastery( 0.0f )

//{{ kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ
, m_fIncreaseRangeSubWeaponShot( 1.0f )
, m_fCriticalDamageRate( 0.0f )
, m_iCountbyCsgComboDashJumpXXMiniGun(0)
//}} kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ
, m_fProbabilityIronWill( 0.f )
, m_bApplyLandDemolisher( false )
, m_hLandDemolisherAfterEffect( INVALID_EFFECTSET_HANDLE )
, m_fLandDemolisherPowerRate( 0.f )
, m_fRemainIronWillRenderEffectTime( 0.f )
, m_iDebuffTransitNum( 0 )
, m_iShot( -1 )
, m_bIsSummoningShot( false )
, m_iArtilleryStrikePlayCount ( 0 )
, m_vLastShotPos ( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_vLastShotDir ( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_sBulletCount( 0 )
, m_fDecreaseRate( 0.f )
, m_fSummonedLifeTime( 0.f )
, m_bIncreaseCritical( false )
, m_iSiegeShellingCount( 0 )
, m_fSiegeShellingDistance( 1.f )
, m_fSiegeShellingDamageRel( 1.f )
, m_fSiegeShellingUserBallRate( -1.f )
, m_fInsertCannonBallRel( -1.f )
, m_fBeShotCannonBallRel( -1.f )
, m_fBeShotCannonBallRelTime( 0.f )
, m_fWithStandingRate( -1.f )
, m_fWithStandingPowerRate( 1.f )
, m_iChargeCannonBall( 0 )
, m_iDreadChaseCount( 0 )
, m_iCarpetBombingCount( 0 )
, m_iCarpetBombingSmallCount( 0 )
, m_fCarpetBombingTime( 0.f )
, m_bCarpetBombing( false )
, m_vCarpetBombingPos( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_vCarpetBombingDir( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_vCarpetBombingRot( D3DXVECTOR3(0.f, 0.f, 0.f) )
, m_fChaosCannonTime( 0.f )
, m_hChaosCannonEffect( INVALID_EFFECTSET_HANDLE )
, m_pChaosCannonSound(NULL)
, m_bMobileArmoredModeEnable( false )
, m_bMobileArmoredModeXFired( false )
, m_bInStateMobileArmoredMode( false )
, m_iExtraCannonBallReloadCount( 0 )
, m_fMobileArmoredModeDistance( 0.f )
, m_fMobileArmoredModeDamageRel( 0.f )
, m_fMobileArmoredModeStepDustElapsedTime( 0.f )
, m_fMobileArmoredModeFireXCoolTime( 0.f )
, m_fMobileArmoredModeFireZCoolTime( 0.f )
, m_hMobileArmoredModeEffect( INVALID_EFFECTSET_HANDLE )
, m_iRemoteMortarNPCUID( -1 )			/// ��ġ�� �ڰ��� ���̵�
, m_fCannonBallExtraChargeRate( 0.f )	/// ĳ���� �߰� ���� Ȯ��
, m_fMiniGunHeadShotRate( 0.f )
, m_eArtilleryStrikeType ( CAST_NONE )
, m_fRateGasPistonSystemAutoRecharge ( 0.f )
, m_iArtilleryStrikeNowPlayCount ( 0 )
, m_bArtilleryStrikeFire ( false )
, m_fArtilleryStrikePowerRate ( 0 )
, m_iBombardServiceTarget ( 0 )
, m_hBombardServiceEffect( INVALID_EFFECTSET_HANDLE )
, m_iFireCount( 0 )
, m_fChargeTime( 0.f )
, m_fPassiveCriticalRateChung( 0.f )

, m_hSateliteRainBigMissileSummonEffect ( INVALID_EFFECTSET_HANDLE )
, m_bSateliteRainFire ( false )
, m_fSateliteRainPowerRate ( 0.f )
, m_hCEffectSateliteRain ( INVALID_DAMAGE_EFFECT_HANDLE )
, m_fSateliteRainPhase ( 0.f )
, m_bIsFireSateliteRainBigMissile ( false )

, m_fArtilleryStrikeCreateDamageEffectGapTime ( 0.f )	// ��ƿ���� ��Ʈ����ũ �̽� ������ ������ ����Ʈ ���� �ð� Gap, �̽� ���� ���� �������� ���� �߰���
, m_fSiegeShellingIncraseMPRel ( 0.f )
{
	InitializeChungMajorParticleArray();
	InitializeChungMinorParticleArray();
	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	InitializeMajorMeshArray();
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	///////////////////////////////////////////////////////
	m_fThrowStartTime		= 0.6666f;
	m_vThrowStartPosOffset	= D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity	= D3DXVECTOR3(1600.f, 200.f, 0.f);
}

/** @function : �Ҹ���
	@brief : �޸� ����
*/
/*virtual*/ CX2GUChung::~CX2GUChung()
{
	DeleteChungMajorParticle();
	DeleteChungMinorParticle();
	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	DeleteMajorMesh();
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	///////////////////////////////////////////////////////

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER );

	if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
	{
		if( m_hChaosCannonEffect != INVALID_EFFECTSET_HANDLE )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hChaosCannonEffect );
	}
	if( m_pChaosCannonSound != NULL && m_pChaosCannonSound->IsPlaying() == true )
	{
		m_pChaosCannonSound->Stop();
	}
	SAFE_CLOSE( m_pChaosCannonSound );

}


/*static*/
void	CX2GUChung::AppendDeviceToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

//{{ ���� : 2011-01-19
//}} ���� : 2011-01-19


/** @function : Init
	@brief : û�� ������ �ִ� ����� �ʱ�ȭ �� ��ũ��Ʈ �Ľ�
	@param : bool bUseTeamPos, int startPosIndex ( ���� Ŭ������ ���� )
*/
/*virtual*/ void CX2GUChung::Init( bool bUseTeamPos_, int iStartPosIndex_ )
{
	CX2GUUser::Init( bUseTeamPos_, iStartPosIndex_ );

	LoadPersonalInfo( m_LuaManager );
	LoadPromotionItemID( m_LuaManager );
	InitPromotionEquipList();
	UpdateEquipCollisionData();

	// û�� �� ĳ���� �̸�
	if ( IsMyUnit() )
	{
		if( m_bMobileArmoredModeEnable == true )
			GetChungGageData()->SetCannonBallChanged( true );

		// ĳ������ �ִ� ���� ������ ���� �󸶸�ŭ�� UI�� ����ؾ��ϴ��� �����
// 		GetCannonBallUIPtr()->UpdateNumOfVerticalCannonBallUI();
// 		GetCannonBallUIPtr()->InitializeCannonBallUI();
		m_FrameDataFuture.syncData.m_CannonBallCount = GetTotalCannonBallCount();
	}
}

/** @function : ReInit
	@brief : ���ӳ����� ��Ȱ�� � ���� �ٽ� ������ ������ �� ó������� �ϴ� �κ�
	@param :  bTeamPos_(������ �� ���� ��ġ�� ������ ������ ����), iStartPosIndex_(������ ���θ� �ε��� ����)
*/
/*virtual*/ void CX2GUChung::ReInit( bool bTeamPos_ /*= false*/, int iStartPosIndex_ /*= -1 */ )
{
	SetFullHyperMode( false );
	CX2GUUser::ReInit( bTeamPos_, iStartPosIndex_ );

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER );

	if( m_eNowPulseWeaponColorID == PWCI_CHUNG_LAND_DEMOLISHER )
		SetEnableWeaponRenderEffect(false);

	if( INVALID_EFFECTSET_HANDLE != m_hLandDemolisherAfterEffect )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hLandDemolisherAfterEffect );
		m_hLandDemolisherAfterEffect = INVALID_EFFECTSET_HANDLE;
	}
	m_bApplyLandDemolisher = false;

	switch ( g_pX2Game->GetGameType() )
	{
	default:
	case CX2Game::GT_DUNGEON:
		{
			// ���� ������ �ִ� ĳ������ ������ �⺻������ ó�� ������ ������ �ִ� ���� ���� ������
			if ( GetCannonBallCount() < GetBasicCannonBallCount() )
				SetCannonBallCount( GetBasicCannonBallCount() );
		} break;

	case CX2Game::GT_PVP:
		{
			switch ( GetTeam() )
			{
			case CX2Room::TN_RED:
				{
					ResetCannonBallCountWhenRebirthInPVPGame( g_pX2Game->GetRedTeamKill(), g_pX2Game->GetBlueTeamKill() );
				} break;

			case CX2Room::TN_BLUE:
				{
					ResetCannonBallCountWhenRebirthInPVPGame( g_pX2Game->GetBlueTeamKill(), g_pX2Game->GetRedTeamKill() );
				} break;

			default:
				{
					SetCannonBallCount( GetBasicCannonBallCount() );
				} break;
			} // switch ( GetTeam() )

		} break;
	} // switch ( g_pX2Game->GetGameType() )

	if ( IsMyUnit() )
	{
		m_FrameDataFuture.syncData.m_CannonBallCount = GetTotalCannonBallCount();
	}

	m_iSiegeShellingCount = 0;
	m_fBeShotCannonBallRelTime = 0.f;
	m_iDreadChaseCount = 0;
	m_iCarpetBombingCount = 0;
	m_fCarpetBombingTime = 0.f;
	m_bCarpetBombing = false;
	m_fChaosCannonTime = 0.f;

	if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL )
	{
		if( m_hChaosCannonEffect != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hChaosCannonEffect );
			m_hChaosCannonEffect = INVALID_EFFECTSET_HANDLE;
		}
	}
	if( m_pChaosCannonSound != NULL && m_pChaosCannonSound->IsPlaying() == true )
	{
		m_pChaosCannonSound->Stop();
	}
}

/** @function : DamageReact
	@brief : �ǰ� ������ ���� ó�� �κ�
	@param : pDamageData_(�ǰ� ������ ���� ������ ����Ÿ)
*/
/*virtual*/ void CX2GUChung::DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ )
{
	//{{ kimhc // 2011.1.20 // û 1�� ����
	// ǻ��������� ����������� ��ų�� ���� ���̷�����, �ϸ��Ǿ��� �ձ� ���� Hp Up���� ���� ��Ŵ
	float fHpPercentUp = CalculateHpPercentUpRateByDamageDataHpUp( pDamageData_->m_fHpPercentUp );
	if ( 0.0f < fHpPercentUp )
		pDamageData_->m_fHpPercentUp += fHpPercentUp;
	//}} kimhc // 2011.1.20 // û 1�� ����

	float fProbabilityIronWill = GetProbabilityIronWill();
	switch ( GetNowStateID() )
	{
	case CSI_SI_A_CFG_GUARD_WAIT:
	case CSI_SI_A_CFG_GUARD_DAMAGE:
		fProbabilityIronWill *= 2.f;
		break;
	default:
		break;
	}
	switch ( g_pX2Game->GetGameType() )
	{
	case CX2Game::GT_DUNGEON:
	case CX2Game::GT_BATTLE_FIELD:
		fProbabilityIronWill *= 2.f;
		break;
	default:
		break;
	}

	// ���� ���·� �� �� �ִ� ������Ʈ �̸� ������ ���� ����
	int iGetDamageReducePercentGuardState = 0;
	if ( IsThisStateCanBeChangedToGuardStateOnHit() == true )
	{
		// ���� ����Ʈ �Ѱ� �Ҹ�
		m_GuardSystem.SetGuardPointRemain( m_GuardSystem.GetGuardPointRemain() - 1 );

		iGetDamageReducePercentGuardState = m_GuardSystem.GetDamageReducePercentGuardState();
	}

	bool bInvokeIronWill = false;
	// oasis907 : ����� [2011.6.21] û 2�� ���� - ���̾� �ȶ��
	if( GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN &&
		pDamageData_->reActType != CX2DamageManager::RT_NO_DAMAGE &&
		pDamageData_->reActType != CX2DamageManager::RT_DUMMY_DAMAGE &&
		pDamageData_->reActType != CX2DamageManager::RT_SENSOR &&
		GetInvincible() == false &&
		GetNowHp() > 0.f &&
		m_FrameDataNow.unitCondition.bFootOnLine == true &&
		fProbabilityIronWill > GetRandomFloat() )
	{
		bInvokeIronWill = true;
		m_fRemainIronWillRenderEffectTime = _CONST_CHUNG_::MAGIC_NUMBER_IRON_WILL_RENDER_EFFECT_TIME;

		if( null != pDamageData_->optrAttackerGameUnit &&
			pDamageData_->attackerType != CX2DamageManager::AT_EFFECT )
		{
			pDamageData_->optrAttackerGameUnit->SetStopTime(0.5f);
		}
		SetStopTime(0.5f);
		SetStop2Time(0.f);

		AddForceDownGage( 5.f );

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_IRON_WILL", this );

		const int DAMAGE_REDUCE_PERCENT_IRON_WILL = 100;
		SetDamageReducePercent( GetDamageReducePercent() + (DAMAGE_REDUCE_PERCENT_IRON_WILL + iGetDamageReducePercentGuardState) );
		CX2GUUser::DamageReact( pDamageData_ );
		SetDamageReducePercent( GetDamageReducePercent() - (DAMAGE_REDUCE_PERCENT_IRON_WILL + iGetDamageReducePercentGuardState) );
	}
	else if( ( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			&& m_fChaosCannonTime > 0.f )
	{
		const int DAMAGE_REDUCE_PERCENT_CHAOSCANNON = 80;
		SetDamageReducePercent( GetDamageReducePercent() + (DAMAGE_REDUCE_PERCENT_CHAOSCANNON + iGetDamageReducePercentGuardState) );
		CX2GUUser::DamageReact( pDamageData_ );
		SetDamageReducePercent( GetDamageReducePercent() - (DAMAGE_REDUCE_PERCENT_CHAOSCANNON + iGetDamageReducePercentGuardState) );
	}
	else
	{
		SetDamageReducePercent( GetDamageReducePercent() + iGetDamageReducePercentGuardState );
		CX2GUUser::DamageReact( pDamageData_ );
		SetDamageReducePercent( GetDamageReducePercent() - iGetDamageReducePercentGuardState );
	}


	//{{ kimhc // 2011.1.20 // û 1�� ����
	// ǻ��������� ������������� ���� ���� ������ �ٽ� ������� �ǵ���
	if ( 0.0f < fHpPercentUp)
		pDamageData_->m_fHpPercentUp -= fHpPercentUp;
//}} kimhc // 2011.1.20 // û 1�� ����

	//{{ kimhc // 2011.1.12 // û 1�� ����
	switch ( pDamageData_->reActType )
	{
		// �������� �ִ� ������ �ƴϸ�
	case CX2DamageManager::RT_NO_DAMAGE:
	case CX2DamageManager::RT_DUMMY_DAMAGE:
	case CX2DamageManager::RT_SENSOR:
		{
			DamageReactStateChange( pDamageData_ );
		} break;

		// �������� �ִ� �����̸�
	default:
		{
			if( m_fBeShotCannonBallRelTime <= 0.f && m_fBeShotCannonBallRel >= GetRandomFloat() )
			{
				m_fBeShotCannonBallRelTime = 2.f;
				UpCannonBallCount( 1, true );
			}

			if( true == bInvokeIronWill)
			{
				if( pDamageData_->reActResult == CX2DamageManager::RT_DOWN ||
					pDamageData_->reActResult == CX2DamageManager::RT_UP ||
					pDamageData_->reActResult == CX2DamageManager::RT_FLY ||
					pDamageData_->reActResult == CX2DamageManager::RT_DRAG_UP)
				{
					pDamageData_->reActResult = CX2DamageManager::RT_BIG_DAMAGE;
				}

			}


			// ����ٽ� �����̸�
			if ( true == IsThisStateCanBeChangedToGuardStateOnHit() )
			{
				if( m_FrameDataNow.stateParam.bInvincible == false )
				{
					StateChange( CSI_GUARD );
				}
			} // if
			// ǻ�������, ���� ��ų�� ��� ���� ���¸�
			else
			{
				switch ( GetNowStateID() )
				{
				case CSI_SI_A_CFG_GUARD_WAIT:
				case CSI_SI_A_CFG_GUARD_DAMAGE:
					{
						if ( true == m_FrameDataNow.unitCondition.bAttackerFront )
						{
							StateChange( CSI_SI_A_CFG_GUARD_DAMAGE );
						} // if
						else
						{
							DamageReactStateChange( pDamageData_ );
						}
					}
					break;

				case CSI_SI_A_CHG_CHAOS_CANNON_GUARD:
					break;

				default:
					{
						if( m_fChaosCannonTime > 0.f && m_FrameDataNow.stateParam.bInvincible == false )
						{
							StateChange( CSI_SI_A_CHG_CHAOS_CANNON_GUARD );
						}
						else
						{
							DamageReactStateChange( pDamageData_ );
						}
					}
					break;
				}
			}
			// ���� ���°� �ƴϸ�


		} break;
	} // switch
//}} kimhc // 2011.1.12 // û 1�� ����
}

/** @function : Win
	@brief : �¸����� ���� ������Ʈ�� ����
*/
/*virtual*/ void CX2GUChung::Win()
{
	CX2GUUser::Win();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}

/** @function : Lose
	@brief : �й����� ���� ������Ʈ�� ����
*/
/*virtual*/ void CX2GUChung::Lose()
{
	CX2GUUser::Lose();

	if ( true == IsMyUnit() &&
		 0.f < GetNowHp() )
	{
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			StateChange( USI_RIDING_WAIT );
		else
			StateChange( USI_LOSE );
	}
}

/** @function : Draw
	@brief : ����� ���� ������Ʈ�� ����
*/
/*virtual*/ void CX2GUChung::Draw()
{
	CX2GUUser::Draw();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}

/** @function : InitStateID
	@brief : Ư�� ������Ʈ���� ID�� ����Ŭ������ ��������� ������
*/
/*virtual*/ void CX2GUChung::InitStateID()
{
	// û�� MP�� ä��� ������Ʈ�� ĳ���� ���� �ϴ� ������Ʈ�� ����
	m_ChargeMpState			= CSI_CHARGE_CANNON_BALL;	// ��� ��ü�� �� �ִ� ������Ʈ�� �Ǵ��� �� ����
	m_CommonState.m_Wait	= USI_WAIT;
}

/** @function : InitState
	@brief : ���� ���� ������Ʈ �ʱ�ȭ
*/
/*virtual*/ void CX2GUChung::InitState()
{
	switch ( GetUnitClass() )
	{
	case CX2Unit::UC_NONE:
		{
			ASSERT( !L"Unit Class Is fault!" );
			m_iSkillCutInSetIndex = 0;
		} break;

	case CX2Unit::UC_CHUNG_IRON_CANNON:
		{
			m_iSkillCutInSetIndex = 0;
		} break;

	//{{ kimhc // 2011.1.3 // û 1�� ����
	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
		{
			m_iSkillCutInSetIndex = 1;
		} break;

	case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
		{
			m_iSkillCutInSetIndex = 2;
		} break;
	//}} kimhc // 2011.1.3 // û 1�� ����

	case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
		{
			m_iSkillCutInSetIndex = 5;
		} break;

		//{{ JHKang / ������ / 2011.6.21 / 2�� ���� �ƽ�
	case CX2Unit::UC_CHUNG_IRON_PALADIN:
		{
			m_iSkillCutInSetIndex = 3;
		} break;

	case CX2Unit::UC_CHUNG_DEADLY_CHASER:
		{
			m_iSkillCutInSetIndex = 4;
		} break;
		//}}

	case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
		{
			m_iSkillCutInSetIndex = 6;
		} break;
	default:
		break;
	} // switch

	InitStateID();

	// �������� ���� ����, ��� Ŭ�������� ���� �޺� �ʱ�ȭ
	InitStateCommon();
	// ���� ���� ���� ����, �޺� �ʱ�ȭ
	InitStateByUnitClass();


	std::string strStartTableNameUTF8;
	std::string strWinTableNameUTF8;
	std::string strLoseTableNameUTF8;

	// �����ϰ� �����Ǵ� start, win, lose ���̺����� ����
	InitStateCommonRandom( strStartTableNameUTF8, strWinTableNameUTF8, strLoseTableNameUTF8 );

	// ������Ʈ ������ ����
	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID		= USI_START;
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

	// �޺� ������Ʈ ������ ����

	// ��ų ������ �� ��ų ������Ʈ �ʱ�ȭ
	InitPassiveSkillState();
	InitEquippedActiveSkillState();

	CX2GUUser::InitState();
}

/** @function : InitStateCommon
	@brief : ��� ĳ���Ͱ� ���������� ���� ������Ʈ �� ������ ������� �������� ���� ������Ʈ �ʱ�ȭ
*/
void CX2GUChung::InitStateCommon()
{
	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "CSI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "CSI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, DieBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, DieBackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, DieBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DieBackEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "CSI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "CSI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, WaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, WaitStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, WaitFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, WaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "CSI_WALK", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
#else
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, WalkFrameMoveFuture );
#endif // CONVERSION_VS
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, WalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "CSI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, JumpReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "CSI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, JumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "CSI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, JumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "CSI_JUMP_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, JumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "CSI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "CSI_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, DashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, DashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "CSI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, DashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, DashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, DashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, DashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "CSI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	// oasis907 : ����� [2011.6.14] û 2�� ���� - ���̾� �ȶ��
	// UnitClass�� ���� State �� �Űܰ�

	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "CSI_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, DamageDownFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageDownFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, DamageDownBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageDownBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DamageAirDownLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "CSI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_ROLLING_FRONT_FRONT;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_ROLLING_FRONT_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_ROLLING_FRONT_BACK;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_ROLLING_FRONT_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_ROLLING_BACK_FRONT;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_ROLLING_BACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_ROLLING_BACK_BACK;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_ROLLING_BACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, StandUpAttackFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpAttackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, StandUpAttackBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpAttackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "CSI_WALL_LANDING", stateData.stateID );
#ifdef CONVERSION_VS
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
#else
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, WallLandingEndFuture );
#endif // CONVERSION_VS
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "CSI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "CSI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "CSI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "CSI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "CSI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "CSI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;

	//#pragma region CSI_RIDING_START
	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "CSI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_ON
	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "CSI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOnEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_OFF
	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "CSI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_WAIT_HABIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "CSI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_WAIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "CSI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_WALK
	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "CSI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "CSI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "CSI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "CSI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DASH
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DASH_END
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DASH_JUMP
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DASH_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DAMAGE_FRONT
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DAMAGE_BACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );;
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_DIE
	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "CSI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "CSI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_JUMP_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "CSI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_ATTACK_X
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "CSI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_SPECIAL_ATTACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "CSI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region CSI_RIDING_SPECIAL_MOVE
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "CSI_RIDING_SPECIAL_MOVE", stateData.stateID );
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
	m_LuaManager.MakeTableReference( "CSI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingHyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, RidingHyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_CHARGE_CANNON_BALL;
	m_LuaManager.MakeTableReference( "CSI_CHARGE_CANNON_BALL", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, ChargeCannonBallStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, ChargeCannonBallStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ChargeCannonBallFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ChargeCannonBallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_GUARD;
	m_LuaManager.MakeTableReference( "CSI_GUARD", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUChung, GuardStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, GuardEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= CSI_GUARD_ATTACK_Z;
	m_LuaManager.MakeTableReference( "CSI_GUARD_ATTACK_Z", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, GuardAttackZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, GuardAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_GUARD_ATTACK_X;
	m_LuaManager.MakeTableReference( "CSI_GUARD_ATTACK_X", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, GuardAttackXInit );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, GuardAttackXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, GuardAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;



	////////////////////////////////////////////////////////////////////////////////////////////////
	//									���⼭ ���� �޺�										  //
	////////////////////////////////////////////////////////////////////////////////////////////////

	stateData.Init();
	stateData.stateID			= CSI_COMBO_Z;
	m_LuaManager.MakeTableReference( "CSI_COMBO_Z", stateData.stateID );
	stateData.OnFrameMove	= SET_CB_FUNC( CX2GUChung, ComboZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZ", stateData.stateID );
	stateData.OnFrameMove	= SET_CB_FUNC( CX2GUChung, ComboZZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZZ", stateData.stateID );
	stateData.OnFrameMove	= SET_CB_FUNC( CX2GUChung, ComboZZZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZZZ;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZZZ", stateData.stateID );
	stateData.OnFrameMove	= SET_CB_FUNC( CX2GUChung, ComboZZZZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;


	////////////////// CSI_COMBO_ZZZX ///////////////////////////////////////////////
	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZZX_READY;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZZX_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZZXReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZZX_CANNON_BALL_SHOT;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZZX_CANNON_BALL_SHOT", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboZZZXCannonBallShotInit );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, ComboZZZXCannonBallShotStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboZZZXCannonBallShotFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZZXCannonBallShotEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZZX_FINISH;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZZX_FINISH", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboZZZXFinishInit );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, ComboZZZXFinishStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboZZZXFinishFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZZXFinishEventProcess );
	m_StateList[stateData.stateID] = stateData;

	//////////////////////////////////////////////////////////////////////////////////////

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZdownZ;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZdownZ", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboZZdownZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZdownZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_ZZdownZZ;
	m_LuaManager.MakeTableReference( "CSI_COMBO_ZZdownZZ", stateData.stateID );
	stateData.OnFrameMove	= SET_CB_FUNC( CX2GUChung, ComboZZdownZZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboZZdownZZEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= CSI_COMBO_XXZX_CANNON_BALL_SHOT;
	m_LuaManager.MakeTableReference( "CSI_COMBO_XXZX_CANNON_BALL_SHOT", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXZXCannonBallShotInit );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXZXCannonBallShotFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZXCannonBallShotEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_XXZX_FINISH;
	m_LuaManager.MakeTableReference( "CSI_COMBO_XXZX_FINISH", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXZXFinishInit );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXZXFinishFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZXFinishEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= CSI_ATTACK_JUMP_Z;
	m_LuaManager.MakeTableReference( "CSI_ATTACK_JUMP_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackJumpZEventProcess );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, AttackJumpZFrameMoveFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_ATTACK_JUMP_X;
	m_LuaManager.MakeTableReference( "CSI_ATTACK_JUMP_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackJumpXEventProcess );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, AttackJumpXFrameMoveFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_DASH_JUMP_Z;
	m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_JUMP_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashJumpZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_DASH_JUMP_ZZ;
	m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_JUMP_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashJumpZZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_DASH_JUMP_ZZLanding;
	m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_JUMP_ZZLanding", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboDashJumpZZLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashJumpZZLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_COMBO_DASH_JUMP_ZZLanding_CANNON_BALL_SHOT;
	m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_JUMP_ZZLanding_CANNON_BALL_SHOT", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboDashJumpZZLandingCannonBallShotInit );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboDashJumpZZLandingCannonBallShotFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashJumpZZLandingCannonBallShotEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_ATTACK_DASH_JUMP_X;
	m_LuaManager.MakeTableReference( "CSI_ATTACK_DASH_JUMP_X", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashJumpXInit );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, AttackDashJumpXStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashJumpXFrameMove );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, AttackDashJumpXFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashJumpXEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_CANNON_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_CANNON_ATTACK_FRONT", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackInit );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_STANDUP_CANNON_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "CSI_STANDUP_CANNON_ATTACK_BACK", stateData.stateID );
	// StandUp�� ���� ���ҽ��� ���� ������ StandBack�� Init�� ���� ����
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackEnd );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, StandUpCannonAttackEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_SI_A_SIEGE_SHELLING_READYLOOP;
	m_LuaManager.MakeTableReference( "CSI_SI_A_SIEGE_SHELLING_READYLOOP", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_ReadyLoop_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_ReadyLoop_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_SI_A_SIEGE_SHELLING_X;
	m_LuaManager.MakeTableReference( "CSI_SI_A_SIEGE_SHELLING_X", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_X_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_X_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_SI_A_SIEGE_SHELLING_X_FINISH;
	m_LuaManager.MakeTableReference( "CSI_SI_A_SIEGE_SHELLING_X_FINISH", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_X_Finish_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_X_Finish_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= CSI_SI_A_SIEGE_SHELLING_Z_FINISH;
	m_LuaManager.MakeTableReference( "CSI_SI_A_SIEGE_SHELLING_Z_FINISH", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Z_Finish_EventProcess );
	m_StateList[stateData.stateID] = stateData;
}


/** @function : InitStateByUnitClass
	@brief : ���� ���� �ٸ� ������Ʋ �ʱ�ȭ
*/
void CX2GUChung::InitStateByUnitClass()
{
	UserUnitStateData stateData;

	switch ( GetUnitClass() )
	{
	default:
	case CX2Unit::UC_CHUNG_IRON_CANNON:
		{
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_COMBO_X", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_ATTACK_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;

		} break;

	//{{ kimhc // 2011.1.4 // û 1�� ����
	case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
		{
			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_COMBO_X", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			// ComboZZX
			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_ZZX;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_ZZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CFGComboZZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			// ComboDash
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_Z", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CFGComboDashZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_ZZ;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_ZZZ;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_ZZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashZZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			// ComboDashJump
			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_JUMP_ZX;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_JUMP_ZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_JUMP_ZXX;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_JUMP_ZXX", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXXStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_JUMP_ZXXLanding;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_JUMP_ZXXLanding", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXXLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_ATTACK_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;
		} break;
	case CX2Unit::UC_CHUNG_IRON_PALADIN:
		{
			stateData.Init();
			stateData.stateID			= CSI_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_COMBO_X", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			// ComboZZX
			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_ZZX;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_ZZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CFGComboZZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			// ComboDash
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_Z", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CFGComboDashZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_ZZ;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_ZZZ;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_ZZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashZZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			// ComboDashJump
			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_JUMP_ZX;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_JUMP_ZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_JUMP_ZXX;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_JUMP_ZXX", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXXStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CFG_COMBO_DASH_JUMP_ZXXLanding;
			m_LuaManager.MakeTableReference( "CSI_CFG_COMBO_DASH_JUMP_ZXXLanding", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CFGComboDashJumpZXXLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_CIP_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, PowerDownLandingFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CIP_COMBO_POWER_DOWN_LANDING_Z;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_POWER_DOWN_LANDING_Z", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZZInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZZEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CIPComboPowerDownLandingZZEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CIP_COMBO_DASH_XX;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_DASH_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CIPComboDashXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CIPComboDashXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CIP_COMBO_DASH_XXX;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_DASH_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CIP_COMBO_DASH_XXXX;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_DASH_XXXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CIP_COMBO_DASH_XXXZ;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_DASH_XXXZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXZInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXZEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CIPComboDashXXXZEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_CIP_COMBO_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;

		} break;
	case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
		{
			//# �����Ŀ��� �״�� ������� �޺� ������Ʈ
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= CSI_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_COMBO_X", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			///////////////////////////////////////////////////////
			//{{ ���� : 2011-01-12 : û 1�� ���� ���Ͱ����
			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_XFRONTX_FIRE;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_XFRONTX_FIRE", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_XFRONTXX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_XFRONTXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_XX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_XXX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXEnd );
			m_StateList[stateData.stateID] = stateData;

			//////////////////////////////////////////////////////////////////////////
			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_JUMP_X;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_JUMP_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_JUMP_XX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_JUMP_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_JUMP_XX_Landing;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_JUMP_XX_Landing", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingEnd );
			m_StateList[stateData.stateID] = stateData;
			//}} ���� : 2011-01-12 : û 1�� ���� ���Ͱ����
			///////////////////////////////////////////////////////

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;
		} break;

	//}} kimhc // 2011.1.4 // û 1�� ����

	case CX2Unit::UC_CHUNG_DEADLY_CHASER:
		{
			//{{ �����Ŀ��� �״�� ������� �޺� ������Ʈ
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_COMBO_DASH_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXPullSubweaponEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_XFRONTX_FIRE;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_XFRONTX_FIRE", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXFireEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_XFRONTXX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_XFRONTXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboXFrontXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_XX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_XXX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashXXXEnd );
			m_StateList[stateData.stateID] = stateData;

			// �뽬 ����
			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_JUMP_X;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_JUMP_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_JUMP_XX;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_JUMP_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CSG_COMBO_DASH_JUMP_XX_Landing;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_JUMP_XX_Landing", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSGComboDashJumpXXLandingEnd );
			m_StateList[stateData.stateID] = stateData;
			//}}

			// û 2�� ���鸮 ü�̼� �߰� �޺�
			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CDCComboXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CDCComboXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CDCComboXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_XX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CDCComboXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CDCComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_XZ;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_XZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CDCComboXZInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CDCComboXZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboXZEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CDCComboXZEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXX;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, ComboXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, ComboXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_COMBO_XXZ;
			m_LuaManager.MakeTableReference( "CSI_COMBO_XXZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_XZZ;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_XZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CDCComboXZZInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CDCComboXZZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboXZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_ZZX;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_ZZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_ZZXX;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_ZZXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CDCComboZZXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CDCComboZZXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboZZXXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CDCComboZZXXEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CDC_COMBO_ZZXXX;
			m_LuaManager.MakeTableReference( "CSI_CDC_COMBO_ZZXXX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CDCComboZZXXXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CDCComboZZXXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CDCComboZZXXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_CSG_COMBO_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
		{
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_COMBO_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_DASH_COMBO_ZZ;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_COMBO_ZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_DASH_COMBO_ZZX;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_COMBO_ZZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_DASH_JUMP_COMBO_ZX;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_JUMP_COMBO_ZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboDashJumpZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashJumpZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_CHG_COMBO_X", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_CHG_COMBO_XX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_COMBO_XZ;
			m_LuaManager.MakeTableReference( "CSI_CHG_COMBO_XZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_ATTACK_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE;
			m_LuaManager.MakeTableReference( "CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT;
			m_LuaManager.MakeTableReference( "CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;

	case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
		{
			/// �����Ŀ��� �״�� ������� �޺� ������Ʈ
			stateData.Init();
			stateData.stateID			= CSI_COMBO_DASH_Z;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_COMBO_Z", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_DASH_COMBO_ZZ;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_COMBO_ZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_DASH_COMBO_ZZX;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_COMBO_ZZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_DASH_JUMP_COMBO_ZX;
			m_LuaManager.MakeTableReference( "CSI_CHG_DASH_JUMP_COMBO_ZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboDashJumpZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboDashJumpZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_POWER_DOWN_LANDING;
			m_LuaManager.MakeTableReference( "CSI_POWER_DOWN_LANDING", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, PowerLandingStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, PowerDownLandingStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, PowerDownLandingEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_COMBO_X;
			m_LuaManager.MakeTableReference( "CSI_CHG_COMBO_X", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_CHG_COMBO_XX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CHG_ComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CHG_ComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CHG_COMBO_XZ;
			m_LuaManager.MakeTableReference( "CSI_CHG_COMBO_XZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, ComboXXZEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_ATTACK_DASH_X;
			m_LuaManager.MakeTableReference( "CSI_CTT_COMBO_DASH_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUChung, AttackDashXInit );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, AttackDashXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, AttackDashXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, AttackDashXEndFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE;
			m_LuaManager.MakeTableReference( "CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT;
			m_LuaManager.MakeTableReference( "CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			/// û 2�� ���� ��Ƽ�� Ʈ���� - �޺� ZZXXX
			stateData.Init();
			stateData.stateID			= CSI_CTT_COMBO_ZZX;
			m_LuaManager.MakeTableReference( "CSI_CTT_COMBO_ZZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_ComboZZXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_ComboZZXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_COMBO_ZZXX;
			m_LuaManager.MakeTableReference( "CSI_CTT_COMBO_ZZXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_ComboZZXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_ComboZZXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_DASH_COMBO_XX;
			m_LuaManager.MakeTableReference( "CSI_CTT_DASH_COMBO_XX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_DashComboXXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_DashComboXXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_READY;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_READY", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeReadyStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeReadyFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeReadyEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeReadyStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_LOOP;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_LOOP", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopFowardStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopFowardFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopFowardEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopFowardStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_LOOP_BACKWARD;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_LOOP_BACKWARD", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopBackwardStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopBackwardFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopBackwardEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeLoopBackwardStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_UNREADY;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_UNREADY", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeUnreadyStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeUnreadyFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeUnreadyEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeUnreadyStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_Z;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_Z", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeZStateStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeZStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeZFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeZEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeZStateEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= CSI_CTT_MOBILE_ARMORED_MODE_X;
			m_LuaManager.MakeTableReference( "CSI_CTT_MOBILE_ARMORED_MODE_X", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeXStateStartFuture );
			stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeXStateStart );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeXEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CTT_MobileArmoredModeXStateEnd );
			m_StateList[stateData.stateID] = stateData;

		} break;
	} // switch

}
/** @function : InitPassiveSkillState
	@brief : ������ �нú� ��ų�� ���� �ʱ�ȭ
*/
/*virtual*/ void CX2GUChung::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();
	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	const CX2SkillTree* const pSkillTree = g_pData->GetSkillTree();

	m_fPassiveCriticalRateChung = 0;		/// ũ��Ƽ�� ����ġ


	// ���ε� ĳ�� �����͸� ���� �� ���� ���ε� ĳ������ ���� ó����
	const int iSkillLevelReloadCannon = userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_CC_RELOAD_CANNON );

	if ( 0 < iSkillLevelReloadCannon )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_A_CC_RELOAD_CANNON );

		if( NULL != pSkillTemplet )
		{
			UCHAR ucCannonBallCountToBeUp = static_cast<UCHAR>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGE_CANNON_BALL, iSkillLevelReloadCannon ) );
			SetCannonBallCountToBeUpUsingReLoadCannon( ucCannonBallCountToBeUp );
			//SetDamageRelByCannonBallMastery( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL ) );
		}
	} // if

	// ����� ���ε� �����͸� : SI_P_CHG_RELOAD_MASTERY
	const int iSkillLevelReloadMastery = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CHG_RELOAD_MASTERY, true );


	if ( 0 < iSkillLevelReloadMastery )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CHG_RELOAD_MASTERY );

		if( NULL != pSkillTemplet )
		{
			m_fInsertCannonBallRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_INSERT_CANNONBALL_REL, iSkillLevelReloadMastery );
			m_fBeShotCannonBallRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_BE_SHOT_CANNONBALL_REL, iSkillLevelReloadMastery );
		}
	} // if



	// ���õ� ������ �ɷ� : SI_P_CHUNG_ABILITY_CANNONEER, -> 2013�� ��ų �������� ���õ� ������ �ɷ� ��
	// �� �̻� ������� �ʰ� ���� ���ݿ� ��� ���� ��

	const int iSkillLvAbillityCannoneer = userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_CC_SIEGE_SHELLING );


	if( iSkillLvAbillityCannoneer > 0 )
	{
		// ���õ� ������ �ɷ� : SI_P_CHUNG_ABILITY_CANNONEER, -> 2013�� ��ų �������� ���õ� ������ �ɷ� ��
		// �� �̻� ������� �ʰ� ���� ���ݿ� ��� ���� ��

		const CX2SkillTree::SkillTemplet *pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_A_CC_SIEGE_SHELLING );

		if( pSkillTemplet != NULL )
		{
			m_fSiegeShellingDamageRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLvAbillityCannoneer );
			m_fSiegeShellingDistance = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLvAbillityCannoneer );
			m_fSiegeShellingUserBallRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_NOT_CANNONBALL_REL, iSkillLvAbillityCannoneer );
		}
	}
	else
	{
		m_fSiegeShellingDamageRel = 1;
		m_fSiegeShellingDistance = 1;
		m_fSiegeShellingUserBallRate =-1;
	}

	// ź�� ���� : SI_P_CHG_CONVERTED_WARHEAD
	const int iSkillLevelConvertedWarhead = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CHG_CONVERTED_WARHEAD, true );

	if ( 0 < iSkillLevelConvertedWarhead )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CHG_CONVERTED_WARHEAD );
		if( NULL != pSkillTemplet )
		{
			m_fForceDownRelDamageRateBase = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE, iSkillLevelConvertedWarhead );
			m_fCannonBallDamageUp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CANNONBALL_DAMAGE_UP_REL, iSkillLevelConvertedWarhead );
			// �̵� ����, ź�� ���� �нú��� MP �߰� ȹ�淮 ������ �޵��� �߰�
			m_fSiegeShellingIncraseMPRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevelConvertedWarhead );

		}
	}

	// ������ ��Ƽ�� : SI_P_CHG_CANNONEER_WITHSTANDING
	const int iSkillLevelWithStanding = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CHG_CANNONEER_WITHSTANDING, true );

	if ( 0 < iSkillLevelWithStanding )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CHG_CANNONEER_WITHSTANDING );

		if( NULL != pSkillTemplet )
		{
			m_fWithStandingPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillLevelWithStanding );
			m_fWithStandingRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIEGE_SHELLING_REL, iSkillLevelWithStanding );
			m_iChargeCannonBall = (int)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGE_CANNON_BALL, iSkillLevelWithStanding );
		}
	}


	//{{ kimhc // 2011.1.14 // û 1�� ����
	// ������� ����(ǻ�������)
	const int iSkillLevelMetabolismBoost = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

	if ( 0 < iSkillLevelMetabolismBoost )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );

		if( NULL != pSkillTemplet )
		{
			SetForceDownRelAtMeleeAttack( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_MELEE_REL, iSkillLevelMetabolismBoost ) );
		}
	} // if

	// ����(ǻ�������)
	const int iSkillLevelGuardMastery	= userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_GUARD_MASTERY, true );

	if ( 0 < iSkillLevelGuardMastery )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CFG_GUARD_MASTERY );

		if ( NULL != pSkillTemplet )
		{
			SetProbabilityBigDamageAtGuardMastery( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelGuardMastery ) );
		} // if
	} // if

	// źâ/��� ����(���ð����)
	// ó�� Init�ÿ��� ������� ������ ��� ��ü ���� �̽��� ���� �� ���� �ǵ��� ���⿡ �־��
	InitRemodelMagazinePassiveSkill();

	// ���� ����(���ð����)
	int iSkillLevelRemodelRifle = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CSG_REMODEL_RIFLE, true );

	if( 0 < iSkillLevelRemodelRifle )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CSG_REMODEL_RIFLE );

		if( NULL != pSkillTemplet )
		{
			float fIncreaseRangeSubWeaponShot = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevelRemodelRifle ) );
			SetIncreaseRangeSubWeaponShot( 1.f + fIncreaseRangeSubWeaponShot );

			float fCriticalDamageRate = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_DAM_ABS, iSkillLevelRemodelRifle ) );
			SetCriticalDamageRate( 0.f + fCriticalDamageRate );
		}
	}

	// ������ ���(���ð����)
	int iSkillLevelTussleTechnique = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CDC_TUSSLE_TECHNIQUE, true );

	if( 0 < iSkillLevelTussleTechnique )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CDC_TUSSLE_TECHNIQUE );

		if( NULL != pSkillTemplet )
		{
			float fAddForceDownRate = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_RANGE_REL, iSkillLevelTussleTechnique ) );
			m_SkillRelatedData.SetForceDownValueRateOfRangeAttack( m_SkillRelatedData.GetForceDownValueRateOfRangeAttack() + fAddForceDownRate );

			float fAddAnimSpeedFactor = CalculateIncreasingRate( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ANI_SPEED, iSkillLevelTussleTechnique ) );
			SetAddAnimSpeedFactor( fAddAnimSpeedFactor );
		}
	}

	// ǻ��������� ���� ��ų�� ���ؼ� ������ ���� ����ٽ��� ������ �������� ����
	SetReduceGuardDamageAtHeavyStanceFromGuardSkill();
//}} kimhc // 2011.1.14 // û 1�� ����

	// oasis907 : ����� [2011.6.21] û 2�� ���� - ���̾� �ȶ��
	const int iSkillLevelAdvancedMetabolismBoost = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CIP_ADVANCED_METABOLISM_BOOST, true );

	if ( 0 < iSkillLevelAdvancedMetabolismBoost )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CIP_ADVANCED_METABOLISM_BOOST );

		if ( NULL != pSkillTemplet )
		{
			m_fAntiCriticalRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CRITICAL_EVASION_REL, iSkillLevelAdvancedMetabolismBoost );
		}
	} // if
	// �׽�Ʈ �ڵ�

	// oasis907 : ����� [2011.6.21] û 2�� ���� - ���̾� �ȶ��
	const int iSkillLevelIronWill	= userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CIP_IRON_WILL, true );

	if ( 0 < iSkillLevelIronWill )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CIP_IRON_WILL );

		if ( NULL != pSkillTemplet )
		{
			SetProbabilityIronWill( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelIronWill ) );
		} // if
	} // if

	//{{ JHKang / ������ / 2011.7.4 / û 2�� ���� ���鸮 ü�̼�_������ ��Ÿ��(���� �ӵ�, MP/��Ÿ�� ����)

	const int iSkillLevelShowTime= userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CDC_INTENSE_SHOWTIME, true );

	if ( 0 < iSkillLevelShowTime )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CDC_INTENSE_SHOWTIME );

		if ( NULL != pSkillTemplet )
		{
			m_fDecreaseRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DECREASE_REL, iSkillLevelShowTime );
		}
	}
	//}}

	// �̵� ���� : SI_P_CTT_MOBILE_ARMORED_MODE
	const int iSkillLvMobileArmor = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CTT_MOBILE_ARMORED_MODE, true );

	if( iSkillLvMobileArmor > 0 )
	{
		const CX2SkillTree::SkillTemplet *pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CTT_MOBILE_ARMORED_MODE );

		if( pSkillTemplet != NULL )
		{
			m_bMobileArmoredModeEnable = true;
			m_fMobileArmoredModeDistance = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLvMobileArmor );
			m_fMobileArmoredModeDamageRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLvMobileArmor );
		}
	}

	// źâ/��� ����
	m_fCannonBallExtraChargeRate = 0.f;
	const int iSkillLevelRemodelMagazine = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CSG_REMODEL_MAGAZINE, true );

	if ( 0 < iSkillLevelRemodelMagazine )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CSG_REMODEL_MAGAZINE );
		if( NULL != pSkillTemplet )
		{
			m_fCannonBallExtraChargeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_INSERT_CANNONBALL_REL, iSkillLevelRemodelMagazine );
		}
	} // if

	m_fRateGasPistonSystemAutoRecharge = 0.f; // ���� �ǽ��� �ý���, ĳ���� ��� �� �ڵ� ���� Ȯ��, 0 ~ 1
	const int iSkillLevelGasPistonSystem = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CDC_GAS_PISTON_SYSTEM, true );
	if ( 0 < iSkillLevelGasPistonSystem )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CDC_GAS_PISTON_SYSTEM );

		if( NULL != pSkillTemplet )
		{
			m_fRateGasPistonSystemAutoRecharge	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelGasPistonSystem );

			m_fPassiveCriticalRateChung			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ABS, iSkillLevelGasPistonSystem );
		}
	}
}

/** @function : SetEquippedSkillFunc
	@brief : ���Կ� ������ ��ų�� ���� �ش� ��ų ������Ʈ �ʱ�ȭ
*/
/*virtual*/ void CX2GUChung::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
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

	const CX2SkillTree::SkillTemplet* pSkillTempletEquipped = GetEquippedActiveSkillTemplet( eSkillID_ );
	if( NULL == pSkillTempletEquipped )
		return;

	switch( pSkillTempletEquipped->m_eType )
	{
	case CX2SkillTree::ST_BUFF:
		{	// ���� �ʻ��
			m_LuaManager.MakeTableReference( "CSI_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_COMMON_BUFF", hyperStateData.stateID );
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
					m_LuaManager.MakeTableReference( "CSI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "CSI_THROW_ITEM", hyperStateData.stateID );
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


	switch ( eSkillID_ )
	{
	default:
		case CX2SkillTree::SI_A_CC_DETONATION:	// ������̼�
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_DETONATION_START", normalStateData.stateID );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_START_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_START_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_DETONATION_START_HYPER", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CHUNG_DETONATION_LOOP;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_DETONATION_LOOP", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_LOOP_Init );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_LOOP_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CHUNG_DETONATION_END;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_DETONATION_END", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_END_Init );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_END_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_DETONATION_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
		case CX2SkillTree::SI_SA_CC_LUNATIC_BLOW:	// �糪ƽ ���ο�
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_LUNATIC_BLOW_START", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

		case CX2SkillTree::SI_A_CC_RELOAD_CANNON:	// ���ε�ĳ��
		{
			// ���ε� ĳ�� ����
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_A_CHUNG_RELOAD_CANNON", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_RELOAD_CANNON_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_RELOAD_CANNON_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_RELOAD_CANNON_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_RELOAD_CANNON_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CHUNG_RELOAD_CANNON", hyperStateData.stateID );
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;

		///////////////////////////////////////////////////////
		//{{ ���� : 2010-12-15 : Gigantic impact(specialactive)
		case CX2SkillTree::SI_SA_CC_GIGANTIC_IMPACT:
		{
			//# START
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateStartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_FrameMove );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_FrameMoveFuture );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateEnd );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateEndFuture );
			//# START HYPER
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_HYPER", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
			hyperStateData.StateEndFuture		= normalStateData.StateEndFuture;

			{
				UserUnitStateData stateData;

				//# AIRFLY
				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_FrameMove );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_FrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_StateEnd );
				m_StateList[stateData.stateID] = stateData;
				//# AIRFLY HYPER
				stateData.stateID			= CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_HYPER;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_HYPER", stateData.stateID );
				stateData.m_bHyperState		= true;
				m_StateList[stateData.stateID] = stateData;

				//# LANDING
				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_StateEnd );
				m_StateList[stateData.stateID] = stateData;
				//# LANDING HYPER
				stateData.stateID			= CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_HYPER;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_HYPER", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_Hyper_EventProcess );
				stateData.m_bHyperState		= true;
				m_StateList[stateData.stateID] = stateData;
			}
		}
		break;
		//}} ���� : 2010-12-15 : Gigantic impact(specialactive)
		///////////////////////////////////////////////////////

		case CX2SkillTree::SI_A_CC_BRUTAL_SWING:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_BRUTAL_SWING", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_BRUTAL_SWING_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_BRUTAL_SWING_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_BRUTAL_SWING_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_BRUTAL_SWING_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_BRUTAL_SWING_HYPER", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		case CX2SkillTree::SI_SA_CC_SCARE_CHASE:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_SCARE_CHASE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_SCARE_CHASE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_SCARE_CHASE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_SCARE_CHASE_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_SCARE_CHASE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		case CX2SkillTree::SI_SA_CC_ACELDAMA:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_ACELDAMA", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_ACELDAMA_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_ACELDAMA_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_ACELDAMA_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_ACELDAMA_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CHUNG_ACELDAMA_StateEnd );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CHUNG_ACELDAMA_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;
		} break;

		case CX2SkillTree::SI_A_CC_AIMMING_SHOT:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_A_CHUNG_AIMMING_SHOT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_AIMMING_SHOT_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_AIMMING_SHOT_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_AIMMING_SHOT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_AIMMING_SHOT_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHUNG_AIMMING_SHOT_StateEnd );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CHUNG_AIMMING_SHOT", hyperStateData.stateID );
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEnd				= normalStateData.StateEnd;
		}
		break;

		//{{ kimhc // 2011.1.8 // û 1�� ����
		case CX2SkillTree::SI_A_CFG_STEEL_EDGE:	// ��ƿ����(ǻ�������)
		{
			//{{ kimhc // 2011-03-28 // 2011-04 �� ��ġ�� û �޸� ��ų, ������(��ƿ����)
			// ������ �޸� ���� ��쿡
			if ( false == GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO3 ) )
			{
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_STEEL_EDGE", normalStateData.stateID );
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_STEEL_EDGE_HYPER", hyperStateData.stateID );
			}
			else
			{
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_STEEL_EDGE_MEMO3", normalStateData.stateID );
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_STEEL_EDGE_MEMO3_HYPER", hyperStateData.stateID );
			}
			//}} kimhc // 2011-03-28 // 2011-04 �� ��ġ�� û �޸� ��ų, ������(��ƿ����)
			// ������ ���´� ���� ������ ����

			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_STEEL_EDGE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_STEEL_EDGE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_STEEL_EDGE_EventProcess );


			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			//{{ kimhc // 2011-03-28 // 2011-04 �� ��ġ�� û �޸� ��ų, ������(��ƿ����)
			// ������ �޸� �ִ� ���
			if ( IsMyUnit() && true == GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO3 ) )
				GetUnit()->AccessUnitData().m_UserSkillTree.SetCBConsumption( iSlotIndex_, bSlotB_, 0 );	// ĳ���� �Ҹ��� 0���� ��
			//}} kimhc // 2011-03-28 // 2011-04 �� ��ġ�� û �޸� ��ų, ������(��ƿ����)
		} break;

		case CX2SkillTree::SI_SA_CIP_IRON_HOWLING:	// ���̾��Ͽ︵(ǻ�������)
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_IRON_HOWLING", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_IRON_HOWLING_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_IRON_HOWLING_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_IRON_HOWLING_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_IRON_HOWLING_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_SA_CFG_LUNATIC_FURY:	// �糪ƽǻ��(ǻ�������)
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_READY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_READY_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CFG_LUNATIC_FURY_RUN;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_RUN", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_RUN_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_RUN_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CFG_LUNATIC_FURY_END;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_END", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CFG_LUNATIC_FURY_END_HYPER;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_LUNATIC_FURY_END_HYPER", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_LUNATIC_FURY_END_EventProcess );
				stateData.m_bHyperState		= true;
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	case CX2SkillTree::SI_A_CFG_LEAP_ATTACK:	// ��������(ǻ�������)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_LEAP_ATTACK_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_READY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_LEAP_ATTACK_READY", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_A_CFG_LEAP_ATTACK_JUMP;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_LEAP_ATTACK_JUMP", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_A_CFG_LEAP_ATTACK_LANDING;

				// oasis907 : ����� [2011.6.14] û 2�� ���� - ���̾� �ȶ��
				if( GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN )
				{
					if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO7 ) )
						m_LuaManager.MakeTableReference( "CSI_SI_A_CIP_LEAP_ATTACK_LANDING_SMI_CHUNG_MEMO7", stateData.stateID );
					else
						m_LuaManager.MakeTableReference( "CSI_SI_A_CIP_LEAP_ATTACK_LANDING", stateData.stateID );
				}
				else
				{
					if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO7 ) )
						m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_LEAP_ATTACK_LANDING_SMI_CHUNG_MEMO7", stateData.stateID );
					else
						m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_LEAP_ATTACK_LANDING", stateData.stateID );
				}

				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_LANDING_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_LEAP_ATTACK_LANDING_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}

		} break;

	case CX2SkillTree::SI_A_CFG_GUARD:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_GUARD_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_GUARD_READY", hyperStateData.stateID );

			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_A_CFG_GUARD_WAIT;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_GUARD_WAIT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_WAIT_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_WAIT_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_WAIT_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_WAIT_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_A_CFG_GUARD_END;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_GUARD_END", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_END_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_END_End );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_A_CFG_GUARD_DAMAGE;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_GUARD_DAMAGE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_DAMAGE_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_DAMAGE_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_DAMAGE_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_DAMAGE_End );
				m_StateList[stateData.stateID] = stateData;

					if( GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO12) == true )
					{
						stateData.Init();
						stateData.stateID			= CSI_SI_A_CFG_GUARD_READY_MEMO;
						m_LuaManager.MakeTableReference( "CSI_SI_A_CFG_GUARD_READY_MEMO", stateData.stateID );
						stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_Init );
						stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_StartFuture );
						stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_FrameMove );
						stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CFG_GUARD_READY_EventProcess );
						m_StateList[stateData.stateID] = stateData;
					}
				}


			m_GuardForFuryGuardianPtr = CFuryGuardianGuardSkill::CreateFuryGuardianGuardSkill();
			m_GuardForFuryGuardianPtr->SetSkillSlotID_Guard( CHUNG_STATE_ID( iNormalStateID ) );
			m_GuardForFuryGuardianPtr->SetGuardEquippedAtSlotB( bSlotB_ );

		} break;

		case CX2SkillTree::SI_A_CSG_DUAL_BUSTER:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_DUAL_BUSTER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_DUAL_BUSTER_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_DUAL_BUSTER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_DUAL_BUSTER_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_DUAL_BUSTER_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CSG_DUAL_BUSTER_LANDING;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_DUAL_BUSTER_LANDING", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	case CX2SkillTree::SI_SA_CSG_HEAVY_RAILGUN:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_HEAVY_RAILGUN", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_HEAVY_RAILGUN_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_HEAVY_RAILGUN_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_HEAVY_RAILGUN_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_HEAVY_RAILGUN_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_SA_CSG_SHOOTING_STAR:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_SHOOTING_STAR_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_READY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_SHOOTING_STAR_READY_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CSG_SHOOTING_STAR_END;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_SHOOTING_STAR_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHOOTING_STAR_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
		case CX2SkillTree::SI_A_CDC_RUMBLE_SHOT:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CSG_RUMBLE_SHOT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CSG_RUMBLE_SHOT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CSG_RUMBLE_SHOT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CSG_RUMBLE_SHOT_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CSG_RUMBLE_SHOT", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	case CX2SkillTree::SI_A_CSG_HEAD_SHOT:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CSG_HEAD_SHOT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CSG_HEAD_SHOT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CSG_HEAD_SHOT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CSG_HEAD_SHOT_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CSG_HEAD_SHOT", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
		//}} kimhc // 2011.1.8 // û 1�� ����

		//{{ kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų
		case CX2SkillTree::SI_SA_CDC_COMET_CRASHER:	// ī��ũ����(���ð����)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_COMET_CRASHER_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_READY_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_COMET_CRASHER_READY_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CSG_COMET_CRASHER_LOOP;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_COMET_CRASHER_LOOP", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CSG_COMET_CRASHER_FINISH;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_COMET_CRASHER_FINISH", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_FINISH_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_COMET_CRASHER_FINISH_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

		case CX2SkillTree::SI_SA_CDC_SHARPSHOOTER_SYNDROME:	// �������ͽŵ��(���ð����)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_FrameMove );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_FrameMoveFuture );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

		case CX2SkillTree::SI_SA_CFG_PANDEMONIUM_CHAOS:	// �ǵ���Ͽ�(ǻ�������)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_PANDEMONIUM", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_PANDEMONIUM_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;

	case CX2SkillTree::SI_SA_CFG_PAIN_OF_CALADBOLG:	// Į�󺼱�����(ǻ�������)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_PAIN_OF_CALADBOLG", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_HYPER", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
		//}} kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų
		// oasis907 : ����� [2011.6.16] û 2�� ���� - ���̾� �ȶ��
	case CX2SkillTree::SI_A_CIP_SUDDEN_BURSTER: // ���� ������ (���̾� �ȶ��)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CIP_SUDDEN_BURSTER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CIP_SUDDEN_BURSTER_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CIP_SUDDEN_BURSTER_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CIP_SUDDEN_BURSTER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CIP_SUDDEN_BURSTER_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CIP_SUDDEN_BURSTER", hyperStateData.stateID );
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			//UCHAR ucCannonBallCountToBeUp = static_cast<UCHAR>( pSkillTempletEquipped->GetSkillAbilityValue( CX2SkillTree::SA_CHARGE_CANNON_BALL ) );
			//SetCannonBallCountToBeUpUsingReLoadCannon( ucCannonBallCountToBeUp );
		} break;

		case CX2SkillTree::SI_A_CIP_BURST_WOLF: // ����Ʈ ���� (���̾� �ȶ��)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CIP_BURST_WOLF", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_BURST_WOLF_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_BURST_WOLF_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_BURST_WOLF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_BURST_WOLF_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_BURST_WOLF_StateEnd );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CIP_BURST_WOLF_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;
		} break;

		case CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS: // ���� ������� (���̾� �ȶ��)
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CIP_LAND_DEMOLISHER_START", normalStateData.stateID );

			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_START_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_START_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_START_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_START_EndFuture );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CIP_LAND_DEMOLISHER_HYPER", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEndFuture		= normalStateData.StateEndFuture;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_StartFuture );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_FrameMoveFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_EventProcess );
				stateData.StateEndFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_EndFuture );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_Init );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;





		//{{ JHKang / ������ / 2011.6.19 / ���鸮 ü�̼� ��Ƽ�� ��ų : ���� ��
	case CX2SkillTree::SI_A_CDC_LEG_SHOT:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CDC_LEG_SHOT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CDC_LEG_SHOT_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CDC_LEG_SHOT_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CDC_LEG_SHOT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CDC_LEG_SHOT_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CDC_LEG_SHOT", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
		//}}

		//{{ JHKang / ������ / 2011.6.20 / ���鸮 ü�̼� ����� ��Ƽ�� ��ų : ��ƿ���� ��Ʈ����ũ
		case CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_HYPER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateEnd  );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CDC_ARTILLERY_STRIKE_END;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_END_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_Start );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
		//}}

		//{{ JHKang / ������ / 2011.7.1 / ���� ������
		case CX2SkillTree::SI_A_CDC_BULLET_BLITZ:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_BULLET_BLITZ", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_BULLET_BLITZ_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_BULLET_BLITZ_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_BULLET_BLITZ_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_BULLET_BLITZ_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_BULLET_BLITZ_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_StateStart );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
		//}}

		case CX2SkillTree::SI_A_CC_SIEGE_SHELLING:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_SIEGE_SHELLING_READY", normalStateData.stateID );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Ready_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Ready_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Ready_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_SIEGE_SHELLING_READY_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Ready_StateStart );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Ready_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_SIEGE_SHELLING_Ready_EventProcess );
		}
		break;

		case CX2SkillTree::SI_A_CHG_ELASTIC_BOMB_GRENADE:
		{
			m_LuaManager.MakeTableReference( "CSI_A_CHG_ELASTIC_BOMB", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CHG_ELASTIC_BOMB_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_EventProcess );
		}
		break;
	case CX2SkillTree::SI_A_CHG_MARK_OF_COMMANDER:
		{
			m_LuaManager.MakeTableReference( "CSI_A_CHG_MARK_OF_COMMANDER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_MARK_OF_COMMANDER_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_MARK_OF_COMMANDER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_MARK_OF_COMMANDER_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CHG_MARK_OF_COMMANDER_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
		case CX2SkillTree::SI_A_CTT_WONDER_WALL:
		{
			m_LuaManager.MakeTableReference( "CSI_SA_CHG_WONDER_WALL", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_WONDER_WALL_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_WONDER_WALL_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CHG_WONDER_WALL_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;
		case CX2SkillTree::SI_SA_CTT_DREAD_CHASE:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SA_CHG_DREAD_CHASE_START", normalStateData.stateID );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_START_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_START_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CHG_DREAD_CHASE_START_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_START_StateStart );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID	= CSI_SA_CHG_DREAD_CHASE_LOOP;
				m_LuaManager.MakeTableReference( "CSI_SA_CHG_DREAD_CHASE_LOOP", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SA_CHG_DREAD_CHASE_END;
				m_LuaManager.MakeTableReference( "CSI_SA_CHG_DREAD_CHASE_END", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_DREAD_CHASE_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_SA_CHG_BIGBANG_STREAM:
		{
			m_LuaManager.MakeTableReference( "CSI_SA_CHG_BIGBANG_STREAM", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_BIGBANG_STREAM_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_BIGBANG_STREAM_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CHG_BIGBANG_STREAM_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_BIGBANG_STREAM_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_BIGBANG_STREAM_EventProcess );
		}
		break;
	case CX2SkillTree::SI_SA_CHG_CARPET_BOMBING:
		{
			m_LuaManager.MakeTableReference( "CSI_SA_CHG_CARPET_BOMBING", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_CARPET_BOMBING_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_CARPET_BOMBING_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CHG_CARPET_BOMBING_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_CARPET_BOMBING_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHG_CARPET_BOMBING_EventProcess );
		}
		break;
	case CX2SkillTree::SI_SA_CHG_CHAOS_CANNON:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_START", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_START_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_START_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_START_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_START_EventProcess );

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_WAIT;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_WAIT", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_WAIT_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_WAIT_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_WALK;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_WALK", stateData.stateID );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_WALK_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_WALK_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP", stateData.stateID );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN", stateData.stateID );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_END;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_GUARD;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_GUARD", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_GUARD_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_01;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_01", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_ATTACK_EventProcess );
				m_StateList[stateData.stateID] = stateData;
				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_02;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_02", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_ATTACK_EventProcess );
				m_StateList[stateData.stateID] = stateData;
				stateData.Init();
				stateData.stateID	= CSI_SI_A_CHG_CHAOS_CANNON_03;
				m_LuaManager.MakeTableReference( "CSI_SI_A_CHG_CHAOS_CANNON_03", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_A_CHG_CHAOS_CANNON_ATTACK_EventProcess );
				m_StateList[stateData.stateID] = stateData;

			}
		}
		break;
	case CX2SkillTree::SI_A_CTT_AUTOMATIC_MORTAR:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_A_CTT_REMOTE_MORTAR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_REMOTE_MORTAR_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_REMOTE_MORTAR_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_REMOTE_MORTAR_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_REMOTE_MORTAR_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CTT_REMOTE_MORTAR", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;
		case CX2SkillTree::SI_A_CHG_CANNON_STRIKE:
		{
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CTT_CANNON_STRIKE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CTT_CROSS_FIRE_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CTT_CROSS_FIRE_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CTT_CROSS_FIRE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CTT_CROSS_FIRE_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CTT_CANNON_STRIKE_HYPER", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;
		case CX2SkillTree::SI_SA_CTT_TACTICAL_FIELD_INDURANCE:
		{
			// ������ ���´� ���� ������ ����
			m_LuaManager.MakeTableReference( "CSI_SA_CTT_TACTICAL_FIELD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_StateStart );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CTT_TACTICAL_FIELD_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart			= normalStateData.StateStart;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		}
		break;
	case CX2SkillTree::SI_SA_CFG_PANDEMONIUM_FEAR:	// �ǵ���Ͽ� - �Ǿ�
		{
			m_LuaManager.MakeTableReference( "CSI_SA_CFG_PANDEMONIUM_FEAR", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_FEAR_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_FEAR_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_FEAR_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CFG_PANDEMONIUM_FEAR", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CFG_PANDEMONIUM_FEAR_Init );
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.m_bHyperState = true;
		} break;
	case CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE: // ���� ������� - �����ũ
		{
			m_LuaManager.MakeTableReference( "CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START", normalStateData.stateID );

			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_StartFuture );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_EventProcess );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_EndFuture );

			m_LuaManager.MakeTableReference( "CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMoveFuture	= normalStateData.OnFrameMoveFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.StateEndFuture		= normalStateData.StateEndFuture;
			hyperStateData.m_bHyperState		= true;
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN;
				m_LuaManager.MakeTableReference( "CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN_StartFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING;
				m_LuaManager.MakeTableReference( "CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_A_CC_IMPACT_DETONATION:	// ����Ʈ ������̼�
		{
			m_LuaManager.MakeTableReference( "CSI_A_CHUNG_IMPACT_DETONATION_READY", normalStateData.stateID );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_READY_StartFuture );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CHUNG_IMPACT_DETONATION_READY", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.m_bHyperState = true;
			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= CSI_A_CHUNG_IMPACT_DETONATION_LOOP;
				m_LuaManager.MakeTableReference( "CSI_A_CHUNG_IMPACT_DETONATION_LOOP", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_Init );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_A_CHUNG_IMPACT_DETONATION_FINAL;
				m_LuaManager.MakeTableReference( "CSI_A_CHUNG_IMPACT_DETONATION_FINAL", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_A_CIP_ARTILLERY_NOVA:		// ��ƿ���� ���
		{
			m_LuaManager.MakeTableReference( "CSI_A_CIP_ARTILLERY_NOVA_CHARGE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CIP_ARTILLERY_NOVA_CHARGE_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_A_CIP_ARTILLERY_NOVA_CHARGE_StartFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CIP_ARTILLERY_NOVA_CHARGE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CIP_ARTILLERY_NOVA_CHARGE_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CIP_ARTILLERY_NOVA_CHARGE", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.m_bHyperState		= true;

			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID				= CSI_A_CIP_ARTILLERY_NOVA_FIRE;
				m_LuaManager.MakeTableReference( "CSI_A_CIP_ARTILLERY_NOVA_FIRE", stateData.stateID );
				stateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CIP_ARTILLERY_NOVA_FIRE_FrameMove );
				stateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CIP_ARTILLERY_NOVA_FIRE_EventProcess );
				m_StateList[stateData.stateID]	= stateData;
			}
		} break;
	case CX2SkillTree::SI_A_CSG_DOUBLE_FIRE:		// ���� ���̾�
		{
			m_LuaManager.MakeTableReference( "CSI_A_CSG_DOUBLE_FIRE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_DOUBLE_FIRE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_DOUBLE_FIRE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_DOUBLE_FIRE_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CSG_DOUBLE_FIRE", hyperStateData.stateID );

			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
			hyperStateData.m_bHyperState		= true;
		} break;

	case CX2SkillTree::SI_A_CSG_MAGNUM_SHOT:		// �ű׳� ��
		{
			m_LuaManager.MakeTableReference( "CSI_A_CSG_MAGNUM_SHOT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_MAGNUM_SHOT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_MAGNUM_SHOT_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_MAGNUM_SHOT_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CSG_MAGNUM_SHOT", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_MAGNUM_SHOT_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_MAGNUM_SHOT_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CSG_MAGNUM_SHOT_EventProcess );

			hyperStateData.m_bHyperState = true;
		} break;
	case CX2SkillTree::SI_A_CDC_GATLING_SHOT :		// ��Ʋ�� ��
		{
			m_LuaManager.MakeTableReference( "CSI_A_CDC_RAPID_CANNON_READY", normalStateData.stateID );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_READY_StartFuture );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_READY_Start );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_READY_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CDC_RAPID_CANNON_READY", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_READY_StartFuture );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_READY_Start );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_READY_EventProcess );

			hyperStateData.m_bHyperState = true;

			{
				UserUnitStateData stateData;
				stateData.Init();
				stateData.stateID			= CSI_A_CDC_RAPID_CANNON_LOOP;
				m_LuaManager.MakeTableReference( "CSI_A_CDC_RAPID_CANNON_LOOP", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_LOOP_Init );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_LOOP_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_A_CDC_RAPID_CANNON_FINAL;
				m_LuaManager.MakeTableReference( "CSI_A_CDC_RAPID_CANNON_FINAL", stateData.stateID );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_FINAL_StartFuture );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_FINAL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_A_CDC_RAPID_CANNON_FINAL_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		}
		break;
	case CX2SkillTree::SI_A_CHG_ELASTIC_BOMB_LAUNCHER:		// �Ϸ���ƽ �� ����
		{
			m_LuaManager.MakeTableReference( "CSI_A_CHG_ELASTIC_BOMB_LAUNCHER", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CHG_ELASTIC_BOMB_LAUNCHER", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_EventProcess );

			hyperStateData.m_bHyperState = true;
		}
		break;

	case CX2SkillTree::SI_SA_CTT_TACTICAL_FIELD_RAID:		// ��Ƽ�� �ʵ� ���̵�
		{
			m_LuaManager.MakeTableReference( "CSI_SA_CTT_TACTICAL_FIELD_RAID", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_RAID_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_RAID_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_RAID_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_SA_CTT_TACTICAL_FIELD_RAID", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_RAID_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_RAID_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SA_CTT_TACTICAL_FIELD_RAID_EventProcess );

			hyperStateData.m_bHyperState = true;
		}
		break;

	case CX2SkillTree::SI_A_CTT_BOMBARD_SERVICE:		// ���� ����
		{
			m_LuaManager.MakeTableReference( "CSI_A_CTT_BOMBARD_SERVICE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_BOMBARD_SERVICE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_BOMBARD_SERVICE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_BOMBARD_SERVICE_EventProcess );

			m_LuaManager.MakeTableReference( "CSI_A_CTT_BOMBARD_SERVICE", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_BOMBARD_SERVICE_Init );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_BOMBARD_SERVICE_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_A_CTT_BOMBARD_SERVICE_EventProcess );


			hyperStateData.m_bHyperState = true;
		}
		break;

	case CX2SkillTree::SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA:	// ��ƿ���� ��Ʈ����ũ ���� �߸���Ÿ
		{
			// ��ƿ���� ��Ʈ����ũ �� CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Start
			// �� Init �κи� �ٸ���.
			m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd );

			m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY", hyperStateData.stateID );
			hyperStateData.StateInit			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Init );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Start );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd );
			hyperStateData.m_bHyperState = true;
			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateEnd  );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID	= CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_Start );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CDC_ARTILLERY_STRIKE_END;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL;
				m_LuaManager.MakeTableReference( "CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_Start );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	//#pragma region SI_HA_CDC_OUTRAGE_STRIKE
	case CX2SkillTree::SI_HA_CDC_OUTRAGE_STRIKE:
		{
			m_LuaManager.MakeTableReference( "CSI_HA_CDC_OUTRAGE_STRIKE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUChung, CSI_HA_CDC_OUTRAGE_STRIKE_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "CSI_HA_CDC_OUTRAGE_STRIKE", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
		} break;
	//#pragma endregion  : �ñر�

	//#pragma region SI_HA_CDC_OUTRAGE_STRIKE
	case CX2SkillTree::SI_HA_CIP_DOOM_STRIKER:
		{
			m_LuaManager.MakeTableReference( "CSI_HA_CIP_DOOM_STRIKER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUChung, CSI_HA_CIP_DOOM_STRIKER_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "CSI_HA_CIP_DOOM_STRIKER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
		} break;
	//#pragma endregion �� ��Ʈ����Ŀ : �ñر�

//#pragma region CSI_HA_CTT_SATELITE_RAIN, ������
	case CX2SkillTree::SI_HA_CTT_SATELITE_RAIN:
		{
			m_LuaManager.MakeTableReference( "CSI_HA_CTT_SATELITE_RAIN", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUChung, CSI_HA_CTT_SATELITE_RAIN_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung, CSI_HA_CTT_SATELITE_RAIN_EventProcess );
			normalStateData.OnFrameMove	    = SET_CB_FUNC( CX2GUChung, CSI_HA_CTT_SATELITE_RAIN_FrameMove );

			m_LuaManager.MakeTableReference( "CSI_HA_CTT_SATELITE_RAIN", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUChung,  CSI_HA_CTT_SATELITE_RAIN_EventProcess );
			hyperStateData.OnFrameMove	    = SET_CB_FUNC( CX2GUChung,  CSI_HA_CTT_SATELITE_RAIN_FrameMove );

		} break;
//#pragma endregion ��Ʋ����Ʈ ���� : �ñر�, ������



	} // switch
}

/** @function : SetEquippedSkillLevel
	@brief : �� ĳ������ ��ų ���� ������Ʈ�� ��ųID, ��ų ����, �Ŀ�����Ʈ ���� ������Ʈ �����Ϳ� �ʱ�ȭ\n
	Ư�� ��ų�� �ΰ� �̻��� ������Ʈ�� ������ ��� �̰��� �߰��ϴ� ���� �����ؾ���
	@param : const CX2SkillTree::SKILL_ID eSkillID_ (������ ��ų�� ID), const bool bChangeAll_ (true�� ��� ��� ��ų ����)
*/
/*virtual*/ void CX2GUChung::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CC_DETONATION == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletDetonation = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CC_DETONATION );

		// normal�� hyper�� ����� �ٸ��� ���⶧���� hyper�� ó������ ����
		// ������̼�
		SetEquippedSkillLevelStateData( pSkillTempletDetonation, CSI_SI_SA_CHUNG_DETONATION_LOOP );
		SetEquippedSkillLevelStateData( pSkillTempletDetonation, CSI_SI_SA_CHUNG_DETONATION_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CC_LUNATIC_BLOW == eSkillID_ )
	{
		// �糪ƽ ���ο�
		const CX2SkillTree::SkillTemplet* pSkillTempletLunaticBlow = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CC_LUNATIC_BLOW );

		SetEquippedSkillLevelStateData( pSkillTempletLunaticBlow, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION );
		SetEquippedSkillLevelStateData( pSkillTempletLunaticBlow, CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CC_GIGANTIC_IMPACT == eSkillID_ )
	{
		// �Ⱓƽ ����Ʈ
		const CX2SkillTree::SkillTemplet* pSkillTempletGiganticImpact = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CC_GIGANTIC_IMPACT );

		SetEquippedSkillLevelStateData( pSkillTempletGiganticImpact, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY );
		SetEquippedSkillLevelStateData( pSkillTempletGiganticImpact, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_HYPER );
		SetEquippedSkillLevelStateData( pSkillTempletGiganticImpact, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING );
		SetEquippedSkillLevelStateData( pSkillTempletGiganticImpact, CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_HYPER );
	}

	//{{ kimhc // 2011.1.9 // û 1�� ����

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CFG_LUNATIC_FURY == eSkillID_ )
	{
		// �糪ƽǻ��(ǻ�������)
		const CX2SkillTree::SkillTemplet* pSkillTempletLunaticFury = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CFG_LUNATIC_FURY );

		SetEquippedSkillLevelStateData( pSkillTempletLunaticFury, CSI_SI_SA_CFG_LUNATIC_FURY_RUN );
		SetEquippedSkillLevelStateData( pSkillTempletLunaticFury, CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK );
		SetEquippedSkillLevelStateData( pSkillTempletLunaticFury, CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING );
		SetEquippedSkillLevelStateData( pSkillTempletLunaticFury, CSI_SI_SA_CFG_LUNATIC_FURY_END );
		SetEquippedSkillLevelStateData( pSkillTempletLunaticFury, CSI_SI_SA_CFG_LUNATIC_FURY_END_HYPER );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CFG_LEAP_ATTACK == eSkillID_ )
	{
		// ��������(ǻ�������)
		const CX2SkillTree::SkillTemplet* pSkillTempletLeapAttack = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CFG_LEAP_ATTACK );

		SetEquippedSkillLevelStateData( pSkillTempletLeapAttack, CSI_SI_A_CFG_LEAP_ATTACK_JUMP );
		SetEquippedSkillLevelStateData( pSkillTempletLeapAttack, CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN );
		SetEquippedSkillLevelStateData( pSkillTempletLeapAttack, CSI_SI_A_CFG_LEAP_ATTACK_LANDING );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CFG_GUARD == eSkillID_ )
	{
		// ����(ǻ�������)
		const CX2SkillTree::SkillTemplet* pSkillTempletGuard = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CFG_GUARD );

		SetEquippedSkillLevelStateData( pSkillTempletGuard, CSI_SI_A_CFG_GUARD_WAIT );
		SetEquippedSkillLevelStateData( pSkillTempletGuard, CSI_SI_A_CFG_GUARD_END );
		SetEquippedSkillLevelStateData( pSkillTempletGuard, CSI_SI_A_CFG_GUARD_DAMAGE );

		// �Ҽ��� �Ǿ��ִ� ���� ������ ����
		if ( NULL != pSkillTempletGuard )
		{
			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTempletGuard->m_eID ) );	/// ��ų ����

			int iReduceDamagePercent = 100 - static_cast<int>( pSkillTempletGuard->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillTempletLevel ) * 100 );
			if ( m_GuardForFuryGuardianPtr != NULL )
			{
				m_GuardForFuryGuardianPtr->SetDamageReducePercent( iReduceDamagePercent );
			} // if
		} // if
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CSG_DUAL_BUSTER == eSkillID_ )
	{
		// ��������(���ð����)
		const CX2SkillTree::SkillTemplet* pSkillTempletDualBuster = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CSG_DUAL_BUSTER );

		SetEquippedSkillLevelStateData( pSkillTempletDualBuster, CSI_SI_SA_CSG_DUAL_BUSTER_LANDING );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CSG_SHOOTING_STAR == eSkillID_ )
	{
		// ���ý�Ÿ(���ð����)
		const CX2SkillTree::SkillTemplet* pSkillTempletShootingStar = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CSG_SHOOTING_STAR );

		SetEquippedSkillLevelStateData( pSkillTempletShootingStar, CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK );
		SetEquippedSkillLevelStateData( pSkillTempletShootingStar, CSI_SI_SA_CSG_SHOOTING_STAR_END );
	}
//}} kimhc // 2011.1.9 // û 1�� ����

	//{{ kimhc // 2011-04-26 // 2011-04 �� ��ġ�� û ĳ�� ��ų
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CDC_COMET_CRASHER == eSkillID_ )
	{
		// ī��ũ����(���ð����)
		const CX2SkillTree::SkillTemplet* pSkillTempletCometCrasher = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CDC_COMET_CRASHER );

		SetEquippedSkillLevelStateData( pSkillTempletCometCrasher, CSI_SI_SA_CSG_COMET_CRASHER_LOOP );
		SetEquippedSkillLevelStateData( pSkillTempletCometCrasher, CSI_SI_SA_CSG_COMET_CRASHER_FINISH );
	}
	//}} kimhc // 2011-04-26 // 2011-04 �� ��ġ�� û ĳ�� ��ų


	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CDC_LEG_SHOT == eSkillID_ )
	{
		//{{ JHKang / ������ / 2011.6.20 / ���� ��
		const CX2SkillTree::SkillTemplet* pSkillTempletLegShot = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CDC_LEG_SHOT );
		SetEquippedSkillLevelStateData( pSkillTempletLegShot, CSI_SI_A_CDC_LEG_SHOT );
		//}}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE == eSkillID_ )
	{
		//{{ JHKang / ������ / 2011.6.20 / ��ƿ���� ��Ʈ����ũ
		const CX2SkillTree::SkillTemplet* pSkillTempletAtilleryStrike = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE );
		SetEquippedSkillLevelStateData( pSkillTempletAtilleryStrike, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY );
		SetEquippedSkillLevelStateData( pSkillTempletAtilleryStrike, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE );
		SetEquippedSkillLevelStateData( pSkillTempletAtilleryStrike, CSI_SI_SA_CDC_ARTILLERY_STRIKE_END );
		//}}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CIP_SUDDEN_BURSTER == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletSuddenBurster = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CIP_SUDDEN_BURSTER );
		SetEquippedSkillLevelStateData( pSkillTempletSuddenBurster, CSI_SI_A_CIP_SUDDEN_BURSTER );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CIP_BURST_WOLF == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletBurstWolf = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CIP_BURST_WOLF );
		SetEquippedSkillLevelStateData( pSkillTempletBurstWolf, CSI_SI_SA_CIP_BURST_WOLF );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletLandDemolishier = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS );
		SetEquippedSkillLevelStateData( pSkillTempletLandDemolishier, CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN );
		SetEquippedSkillLevelStateData( pSkillTempletLandDemolishier, CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CTT_DREAD_CHASE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletDreadChase = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CTT_DREAD_CHASE );
		SetEquippedSkillLevelStateData( pSkillTempletDreadChase, CSI_SA_CHG_DREAD_CHASE_LOOP );
		SetEquippedSkillLevelStateData( pSkillTempletDreadChase, CSI_SA_CHG_DREAD_CHASE_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CHG_CHAOS_CANNON == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletChaosCannon = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CHG_CHAOS_CANNON );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_WAIT );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_WALK );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_END );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_GUARD );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_01 );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_02 );
		SetEquippedSkillLevelStateData( pSkillTempletChaosCannon, CSI_SI_A_CHG_CHAOS_CANNON_03 );
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CC_IMPACT_DETONATION == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletDetonation = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CC_IMPACT_DETONATION );

		SetEquippedSkillLevelStateData( pSkillTempletDetonation, CSI_A_CHUNG_IMPACT_DETONATION_LOOP );
		SetEquippedSkillLevelStateData( pSkillTempletDetonation, CSI_A_CHUNG_IMPACT_DETONATION_FINAL );
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletLandDemolishier = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE );
		SetEquippedSkillLevelStateData( pSkillTempletLandDemolishier, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN );
		SetEquippedSkillLevelStateData( pSkillTempletLandDemolishier, CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletAtilleryStrike = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA );
		SetEquippedSkillLevelStateData( pSkillTempletAtilleryStrike, CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY );
		SetEquippedSkillLevelStateData( pSkillTempletAtilleryStrike, CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE );
		SetEquippedSkillLevelStateData( pSkillTempletAtilleryStrike, CSI_SI_SA_CDC_ARTILLERY_STRIKE_END );
		//}}
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CDC_GATLING_SHOT == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletDetonation = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CDC_GATLING_SHOT );

		SetEquippedSkillLevelStateData( pSkillTempletDetonation, CSI_A_CDC_RAPID_CANNON_LOOP );
		SetEquippedSkillLevelStateData( pSkillTempletDetonation, CSI_A_CDC_RAPID_CANNON_FINAL );
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CC_SIEGE_SHELLING == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletSiegeShelling = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CC_SIEGE_SHELLING );

		SetEquippedSkillLevelStateData( pSkillTempletSiegeShelling, CSI_SI_A_SIEGE_SHELLING_X );
		SetEquippedSkillLevelStateData( pSkillTempletSiegeShelling, CSI_SI_A_SIEGE_SHELLING_X_FINISH );
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_CIP_ARTILLERY_NOVA == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletArtilleryNova= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CIP_ARTILLERY_NOVA );

		SetEquippedSkillLevelStateData( pSkillTempletArtilleryNova, CSI_A_CIP_ARTILLERY_NOVA_FIRE );
	}

}

/** @function : SetEquippedSkillLevelStateData
	@brief : ������ ��ų�� �������� ����� �� ���� ����Ǿ�� �� ������Ʈ ����Ÿ
	@param : const CX2SkillTree::SkillTemplet* pSkillTemplet, CHUNG_STATE_ID eStateID
*/
void CX2GUChung::SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet, CHUNG_STATE_ID eStateID )
{
	if ( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		UserUnitStateData& stateData = m_StateList[eStateID];
		stateData.m_SPLevel		= iSkillTempletLevel;
		stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
		stateData.m_eSkillID	= pSkillTemplet->m_eID;
	} // if
}

/** @function : InitComponent
	@brief : ĳ���� ��ũ��Ʈ�� INIT_COMPONENT ���̺��� �ε���
*/

/*static*/
void	CX2GUChung::AppendComponentToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}

/*virtual*/ void CX2GUChung::InitComponent()
{
	CX2GUUser::InitComponent();

	if ( IsMyUnit() )
	{
		ASSERT( NULL != g_pX2Game );
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_PVP:
			{
				static_cast<CX2ChungGageData*>(m_pGageData)->SetBerserkMode(false);
				static_cast<CX2ChungGageData*>(m_pGageData)->SetBerserkModeChanged(false);
				static_cast<CX2ChungGageData*>(m_pGageData)->SetNowCannonBallCount(3);
				static_cast<CX2ChungGageData*>(m_pGageData)->SetNowCannonBallCountEx(0);
				static_cast<CX2ChungGageData*>(m_pGageData)->SetCannonBallChanged(true);
			} break;

		default:
			{
				if ( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					static_cast<CX2ChungGageData*>(m_pGageData)->SetBerserkMode(false);
					static_cast<CX2ChungGageData*>(m_pGageData)->SetBerserkModeChanged(false);
					static_cast<CX2ChungGageData*>(m_pGageData)->SetNowCannonBallCount(3);
					static_cast<CX2ChungGageData*>(m_pGageData)->SetNowCannonBallCountEx(0);
					static_cast<CX2ChungGageData*>(m_pGageData)->SetCannonBallChanged(true);
				}
			} break;
		}
	}
}

/** @function : ParseCommonRandomState
	@brief : Start, Win, Lose ������Ʈ�� �����ϰ� �ε��Ѵ�
	@param : �� ������ start, win, lose ������Ʈ�� ����ִ� ������ ���̺���
*/
/*virtual*/ void CX2GUChung::ParseCommonRandomState()
{
	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{

		const char* tableName = "";
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_CHUNG_IRON_CANNON:
			{
				tableName = "CHUNG_IRON_CANNON";
			} break;

		//{{ kimhc // 2011.1.3 // û 1�� ����
		case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
			{
                tableName = "CHUNG_FURY_GUARDIAN";
			} break;

		case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
			{
				tableName = "CHUNG_SHOOTING_GUARDIAN";
			} break;
		//}} kimhc // 2011.1.3 // û 1�� ����
		case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
			{
				tableName = "CHUNG_SHELLING_GUARDIAN";
			} break;

		case CX2Unit::UC_CHUNG_IRON_PALADIN:
			{
				tableName = "CHUNG_IRON_PALADIN";
			} break;

		case CX2Unit::UC_CHUNG_DEADLY_CHASER:
			{
				tableName = "CHUNG_DEADLY_CHASER";
			} break;

		case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
			{
				tableName = "CHUNG_TACTICAL_TROOPER";
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
void	CX2GUChung::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUChung::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUChung::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForSwordToDeviceList( listInOut_, eDamageType_ );
}

/** @function : InitEffect()
	@brief : ����� ������ �ִ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUChung::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();
}

/** @function : InitEnchantWeaponEffectUnitSpecific
	@brief : �� ĳ���Ͱ� ���� ����Ʈ ��ƼŬ �ʱ�ȭ
*/
/*virtual*/ void CX2GUChung::InitEnchantWeaponEffectUnitSpecific()
{
	for( Weapon* pWeapon : m_vecpWeapon )
	{
		ASSERT( NULL != pWeapon );
		pWeapon->InitEnchantParticleForSword();	// �ӽ�
	}
}
/** @function : InitializeMajorParticleArray
	@brief : ����� ������ �ִ� ������ ��ƼŬ �ڵ� �迭 �ʱ�ȭ
*/
void CX2GUChung::InitializeChungMajorParticleArray()
{
	CX2GUUser::InitializeGUUserMajorParticleArray();

	for ( int index = 0; index < CHUNG_MAJOR_PII_END; index++ )
	{
		m_ahChungMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUChung::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
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

/** @function : CreateMajorParticleArray
@brief : ����� ������ �ִ� ������ ��ƼŬ ����
*/
/*virtual*/ void CX2GUChung::CreateMajorParticleArray()
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

/** @function : SetChungMajorParticleByEnum
	@brief : ��ƼŬ���� ��ġ�ϴ� ������ ��ƼŬ�� ������ �� �迭�� �ش� ��ƼŬ �ڵ��� ������, �ڵ��� �迭�� ����, �׸��� �� �ν��Ͻ��� ��������
	@param : ��ƼŬ Instance ID, ��ƼŬ ��, DrawCount
	@return : ������ �Ǵ� �̹� �����ϴ� ��ƼŬ�� �ν��Ͻ�(CKTDGParticleSystem::CParticleEventSequence*)
*/
CKTDGParticleSystem::CParticleEventSequence* CX2GUChung::SetChungMajorParticleByEnum( CHUNG_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleChungMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleChungMajorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleChungMajorParticleByEnum( eVal_) );

	return pSeq;
}

/** @function : DeleteMajorParticle
	@brief : ����� ������ ������ ��ƼŬ ����Ʈ ����
*/
void CX2GUChung::DeleteChungMajorParticle()
{
	CX2GUUser::DeleteGUUserMajorParticle();

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	for ( int index = 0; index < CHUNG_MAJOR_PII_END; index++ )
	{
		pMajorParticleSystem->DestroyInstanceHandle( m_ahChungMajorParticleInstance[index] );
	}
}

/** @function : InitializeChungMinorParticleArray
@brief : ����� ������ �ִ� ���̳� ��ƼŬ �ڵ� �迭 �ʱ�ȭ
*/
void CX2GUChung::InitializeChungMinorParticleArray()
{
	CX2GUUser::InitializeGUUserMinorParticleArray();

	for ( int index = 0; index < CHUNG_MINOR_PII_END; index++ )
	{
		m_ahChungMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUChung::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}


/** @function : CreateMinorParticleArray
@brief : ����� ������ �ִ� ������ ��ƼŬ ����
*/
/*virtual*/ void CX2GUChung::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();
}

/** @function : SetChungMinorParticleByEnum
@brief : ��ƼŬ���� ��ġ�ϴ� ���̳� ��ƼŬ�� ������ �� �迭�� �ش� ��ƼŬ �ڵ��� ������, �ڵ��� �迭�� ����, �׸��� �� �ν��Ͻ��� ��������
@param : ��ƼŬ Instance ID, ��ƼŬ ��, DrawCount
@return : ������ �Ǵ� �̹� �����ϴ� ��ƼŬ�� �ν��Ͻ�(CKTDGParticleSystem::CParticleEventSequence*)
*/
CKTDGParticleSystem::CParticleEventSequence* CX2GUChung::SetChungMinorParticleByEnum( CHUNG_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleChungMinorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleChungMinorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleChungMinorParticleByEnum( eVal_) );

	return pSeq;
}

/** @function : DeleteChungMinorParticle
	@brief : ����� ������ ���̳� ��ƼŬ ����Ʈ ����
*/
void CX2GUChung::DeleteChungMinorParticle()
{
	CX2GUUser::DeleteGUUserMinorParticle();

	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	for ( int index = 0; index < CHUNG_MINOR_PII_END; index++ )
	{
		pMinorParticleSystem->DestroyInstanceHandle( m_ahChungMinorParticleInstance[index] );
	}
}

/** @function : InitializeMajorMeshArray
@brief : ����� ������ �ִ� ������ �޽� �ڵ� �迭 �ʱ�ȭ
*/
void CX2GUChung::InitializeMajorMeshArray()
{
	for ( int index = 0; index < CHUNG_MAJOR_MII_END; ++index )
	{
		m_ahChungMajorMeshInstance[index] = INVALID_MESH_INSTANCE_HANDLE;
	}
}

/** @function : SetHandleChungMajorMeshByEnum
@brief : �޽��÷��� �ڵ��� ������ �迭�� ����, �����ҷ��� �ڵ��� INVALID �ϰ�� �޽��÷��� DESTROY �� �迭�ʱ�ȭ
@param : Mesh Instance ID, Mesh Instance Handle
*/
void CX2GUChung::SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MESH_INSTANCE_ID eVal_, CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_ )
{
	if( INVALID_MESH_INSTANCE_HANDLE == hHandle_ && INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum( eVal_ ) )
	{
		CKTDGXMeshPlayer::CXMeshInstanceHandle hHandleChungMesh = GetHandleChungMajorMeshByEnum( eVal_ );

		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(hHandleChungMesh);
	}

	m_ahChungMajorMeshInstance[eVal_] = hHandle_;
}

/** @function : DeleteMajorMesh
	@brief : ����� ������ ������ ����Ʈ ����
*/
void CX2GUChung::DeleteMajorMesh()
{
	CKTDGXMeshPlayer* pMajorMeshSystem = g_pX2Game->GetMajorXMeshPlayer();

	for ( int index = 0; index < CHUNG_MAJOR_MII_END; index++ )
	{
        pMajorMeshSystem->DestroyInstanceHandle( m_ahChungMajorMeshInstance[index] );
	}
}

/** @function : CommonStateStartFuture
	@brief : ��� ������Ʈ�� StartFuture���� ȣ��Ǵ� �Լ�
*/
void CX2GUChung::CommonStateStartFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStartFuture();
}

/** @function : CommonStateStart
	@brief : ��� ������Ʈ�� Start���� ȣ��Ǵ� �Լ�
*/
void CX2GUChung::CommonStateStart()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStart();

	// ���� ���°� �ƴϸ� ���� ����Ʈ�� ����
	if ( CSI_GUARD != GetNowStateID() )
		m_GuardSystem.ChargeGuardPointRemain();

	// ���� ������Ʈ���� ����� ĳ������ ������ �ʱ�ȭ
	m_CannonBallSystem.SetUsedCannonballCountAtThisState( 0 );

	//{{ kimhc // 2011.1.14 // û 1�� ����
	if ( m_DamageData.m_bMeleeAttack )
	{
		m_DamageData.fForceDownValue *= GetForceDownRelAtMeleeAttack();
	} // if
//}} kimhc // 2011.1.14 // û 1�� ����

	m_bIncreaseCritical = false;

	SetPassiveCriticalRate(m_fPassiveCriticalRateChung);

}

/** @function : CommonFrameMoveFuture
	@brief : ��� ������Ʈ�� FrameMoveFuture���� ȣ��Ǵ� �Լ�
*/
void CX2GUChung::CommonFrameMoveFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMoveFuture();
	m_FrameDataFuture.syncData.m_CannonBallCount = GetTotalCannonBallCount();

// 	int iMaxAnimationLoopTimeAtThisSate = 1;
// 	LUA_GET_VALUE( m_LuaManager, "MAX_AIMATION_LOOP_TIME_AT_THIS_STATE", iMaxAnimationLoopTimeAtThisSate, 1 );
// 	SetMaxAnimationLoopTimeAtThisSate( iMaxAnimationLoopTimeAtThisSate, false );
}

/** @function : CommonFrameMove
	@brief : ��� ������Ʈ�� FrameMove���� ȣ��Ǵ� �Լ�
*/
void CX2GUChung::CommonFrameMove()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMove();

	// ��ȯ�� ���� ó��
	if( 0.f != m_fSummonedLifeTime )
	{
		if( m_timerSummoned.elapsed() > (double) m_fSummonedLifeTime )
		{
			m_fSummonedLifeTime = 0.f;
			SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG );
			SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER );
		}
	}

	if ( false == IsMyUnit() )
	{
		SetTotalCannonBallCount( m_FrameDataNow.syncData.m_CannonBallCount );
	} // if

	// ����ȭ ��忡 ���̴� ���� frameMove ������
	for( CX2EqipPtr pPromotionEquipPtr : m_vecPromotionEquipList )
	{
		pPromotionEquipPtr->SetRenderParam( m_fTime, m_fElapsedTime, m_pXSkinAnim->GetRenderParam() );
		pPromotionEquipPtr->OnFrameMove( m_fTime, m_fElapsedTime );
	}

	// oasis907 : ����� [2011.6.22] û 2�� ���� - ���̾� �ȶ��
	if(m_fRemainIronWillRenderEffectTime > 0.f)
	{
		m_fRemainIronWillRenderEffectTime -= m_fElapsedTime;
	}
	else
	{
		m_fRemainIronWillRenderEffectTime = 0.f;
	}


	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
	{
		const double MAGIC_ARTILLERY_STRIKE_CHECK_TIME = 1.0;
		if( m_TimerCheckInvalidShot.elapsed() > MAGIC_ARTILLERY_STRIKE_CHECK_TIME )
		{
			m_TimerCheckInvalidShot.restart();

			if ( true == GetHold() || true == GetGrap() )
			{
				pNPC->StateChange( "SHOT_ARTILLERY_STRIKER_CANCEL", true );
			}

			if( false == m_bIsSummoningShot )
			{
				switch( GetNowStateID() )
				{
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT:
				case CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE:
					{
						// do nothing;
					} break;

				default:
					{
					} break;
				}
			}
		}
	}

	if( m_fBeShotCannonBallRelTime > 0.f )
	{
		m_fBeShotCannonBallRelTime -= m_fElapsedTime;
		if( m_fBeShotCannonBallRelTime < 0.f )
			m_fBeShotCannonBallRelTime = 0.f;
	}

	if( m_bCarpetBombing == true && m_iCarpetBombingCount < 9+9 )
	{
		m_fCarpetBombingTime += m_fElapsedTime;
		DoCarpetBombing();
	}
	else
	{
		m_iCarpetBombingCount = 0;
		m_fCarpetBombingTime = 0.f;
		m_bCarpetBombing = false;
	}

	if( GetNowHp() <= 0.f )
		m_fChaosCannonTime = 0.f;

	if( m_fChaosCannonTime > 0.f )
	{
		m_fChaosCannonTime -= m_fElapsedTime;
	}
	if( m_fChaosCannonTime <= 0.f )
	{
		m_fChaosCannonTime = 0.f;
		if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL &&
			m_hChaosCannonEffect != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hChaosCannonEffect );
			m_hChaosCannonEffect = INVALID_EFFECTSET_HANDLE;
		}
		if( m_pChaosCannonSound != NULL && m_pChaosCannonSound->IsPlaying() == true )
		{
			m_pChaosCannonSound->Stop();
		}
	}
	if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL && m_bInStateMobileArmoredMode == false )
	{
		if( m_hMobileArmoredModeEffect != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hMobileArmoredModeEffect );
			m_hMobileArmoredModeEffect = INVALID_EFFECTSET_HANDLE;
		}
	}

	// ��ƿ���� ��Ʈ����ũ ��� ����, ������ �ִ� �ð�
	if ( true == m_bArtilleryStrikeFire )
	{
		// ��� �̸��� �濡 ���� �ʾ� ����
		if ( m_TimerCheckArtilleryStrikeFire.elapsed() > _CONST_CHUNG_::START_TIME_OF_ARTILLERY_STRIKE_FIRE_WHEH_TARGETING_SUCCESS )
		{
			// ��ƿ���� ��Ʈ����ũ ����
			m_fArtilleryStrikeCreateDamageEffectGapTime = m_fArtilleryStrikeCreateDamageEffectGapTime + m_fElapsedTime;
			DoArtilleryStrikeFire ();
		}
	}

	// ���� ���� ����Ʈ ���� �ش� ������ ������� �����
	if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL && m_iBombardServiceTarget >= 0 )
	{
		CX2GameUnit *pUnit = g_pX2Game->GetUnit ( m_iBombardServiceTarget );
		if ( NULL == pUnit || pUnit->GetNowHp() <= 0 )
		{
			m_iBombardServiceTarget = -1;
			if( m_hBombardServiceEffect != INVALID_EFFECTSET_HANDLE )
			{
				g_pX2Game->GetEffectSet()->StopEffectSet( m_hBombardServiceEffect );
				m_hBombardServiceEffect = INVALID_EFFECTSET_HANDLE;
			}
		}
	}


	SateliteRainFrameMove();	// ��Ʋ����Ʈ ����, CommonFrameMove �ʿ� ���� �Լ�

}

/** @function : CommonStateEndFuture
	@brief : ��� ������Ʈ�� EndFuture���� ȣ��Ǵ� �Լ�
*/
void CX2GUChung::CommonStateEndFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEndFuture();
}

/** @function : CommonStateEnd
	@brief : ��� ������Ʈ�� End���� ȣ��Ǵ� �Լ�
*/
void CX2GUChung::CommonStateEnd()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEnd();
}

/** @function : CommonRender_Prepare
	@brief : ��� ������Ʈ�� End���� ȣ��Ǵ� �Լ�
*/
RENDER_HINT CX2GUChung::CommonRender_Prepare()
{
	KTDXPROFILE();

	return CX2GUUser::CommonRender_Prepare();
}

/** @function : SepcialAttackEventProcess
	@brief : ��ų�� ���������� ���� �� �ִ� �� �Ǵ�, ��ų��뿡 �ʿ��� ó�� �� ������Ʈ ����
	@param : ��ų�� ����� �� �ִ� ȯ�� ����
	@return : bool (��ų�� ���������� ���Ǿ��°��� ����)
*/
/*virtual*/ bool CX2GUChung::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_ONLY_ON_LINE*/ )
{
	KTDXPROFILE();

	const int INVALID_SKILL_SLOT_INDEX = -1;
	int iPressedSkillSlotIndex = INVALID_SKILL_SLOT_INDEX;

	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = NULL;
	CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree; // ������ ��� ��ų Ʈ��

	// ����� ��ų�� ��ų���� �����Ϳ� �� �ε����� ����
	if ( false == CommonSpecialAttackEventProcess( accessUserSkillTree, pSkillSlotData, iPressedSkillSlotIndex ) )
		return false;

	if( NULL == pSkillSlotData )
		return false;

	// ����� ��ų
	const CX2SkillTree::SkillTemplet* pSkillTempletUsing = GetEquippedActiveSkillTemplet( pSkillSlotData->m_eID );
	if( NULL == pSkillTempletUsing )
		return false;

	// ��ų�� ����� �� �ִ� �������� �Ǵ�( ex: ���߿����� ��� �� �� ���� �ʻ�� ��)
	if( false == CheckSkillUseCondition( eActiveSkillUseCondition, pSkillTempletUsing->m_eActiveSkillUseCondtion ) )
		return false;

	//��ȯ �ʻ�� ������ ������ ���� ���¿��� ����� �� ����.
	if( false == CheckSummonSpecialAttack( pSkillTempletUsing->m_eID ) )
		return false;

	// ���Ĵ������� ��� ������ ��ų���� �˻�
	if( false == CheckSkillUsePVPOfficial( pSkillTempletUsing->m_eID ) )
		return false;

	bool bGuardDamageReact = false;
	if( GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO12) == true )
	{
		if( GetGameUnitStateFuture() == CX2GUUser::USI_DAMAGE_SMALL_FRONT || GetGameUnitStateFuture() == CX2GUUser::USI_DAMAGE_BIG_FRONT )
		{
			if( pSkillTempletUsing->m_eID == CX2SkillTree::SI_A_CFG_GUARD )
				bGuardDamageReact = true;
			else
				return false;
		}
	}

	const int iSkillTempletLevel = max( 1, accessUserSkillTree.GetSkillLevel( pSkillTempletUsing->m_eID ) );	/// ��ų ����

	// ���� �Ҹ��� ����
	float fMPConsume = GetActualMPConsume( pSkillTempletUsing->m_eID, iSkillTempletLevel );


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
		bool bSlotB = (iPressedSkillSlotIndex > 3) ? true : false;
		int iSlotIndex = (iPressedSkillSlotIndex > 3) ? iPressedSkillSlotIndex-4 : iPressedSkillSlotIndex;
		// ĳ���� ���� ������ �Һ��� �������� ���ų� ����ȭ ����� ���
		const USHORT usCBConsumptionNeeded = accessUserSkillTree.GetCBConsumption( iSlotIndex, bSlotB );

		if ( GetCannonBallCount() < usCBConsumptionNeeded && false == IsFullHyperMode() )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_10225 ), D3DXCOLOR(1,1,1,1), D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
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

	float fSkillCoolTime = GetActualCoolTime(pSkillTempletUsing, iSkillTempletLevel );

	if( IsFullHyperMode() && 0.f < m_fDecreaseRate )
	{
		accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, fSkillCoolTime * m_fDecreaseRate );
	}
	else
	{
		// ��ų���ø����� ������ ��Ÿ���� ��ų�� ��� �������� ������
		accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTempletUsing->m_eID, fSkillCoolTime );

	}

	// state�� ������� ���ƾ��ϴ� ��ų�� �ƴ϶�� ������Ʈ ����
	if( false == SpecialAttackNoStageChange( pSkillTempletUsing ) )
	{
		if( bGuardDamageReact == true )
		{
			StateChange( CSI_SI_A_CFG_GUARD_READY_MEMO, true );
		}
		else
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

	Send_SKILL_USE_REQ( pSkillTempletUsing->m_eID );

	return true;
}

/** @function : SpecialAttackNoStageChange
	@brief : ������Ʈ�� ������� ���ƾ��ϴ� ��ų������ üũ �ϰ� ó����
	@param : IN const CX2SkillTree::SkillTemplet* (����� ��ų�� ��ų ���ø� ������)
	@return : ������Ʈ ���� ����
*/
/*virtual*/ bool CX2GUChung::SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* /*pSkillTemplet_*/ )
{
	// pSkillTemplet_ NULL ó�� ����, ���� �Լ����� ȣ��� �̹� ó���Ͽ���

// 	switch ( pSkillTemplet_->m_eID )
// 	{
// 	default:
// 		{
// 			return false;
// 		} break;
//
// 	} // switch

	return false;
}

/** @function : COMMON_BUFF_FrameMove
	@brief : ���������� ���� ���� ��ų�� FrameMove
*/
/*virtual*/ void CX2GUChung::COMMON_BUFF_FrameMove()
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

/** @function : ChangeAnimationChargeCannonBall
	@brief : ĳ���� ������ animation�� Ȯ���� ���� ����
	@param : const FrameData& (now or future)
*/

void CX2GUChung::ChangeAnimationChargeCannonBallNow()
{
	if ( 0.5f > GetRandomFloat() )
		m_pXSkinAnim->ChangeAnim( L"ChargeCannonball02", m_FrameDataNow.unitCondition.bTransitionChange );
}

/** @function : ChangeAnimationChargeCannonBallFuture
	@brief : ĳ���� ������ animationFuture�� Ȯ���� ���� ����
*/
void CX2GUChung::ChangeAnimationChargeCannonBallFuture()
{
	if ( 0.5f > GetRandomFloatFuture() )
		m_pXSkinAnimFuture->ChangeAnim( L"ChargeCannonball02", m_FrameDataFuture.unitCondition.bTransitionChange );
}

/** @function : UpCannonBallCount
	@brief : ĳ������ ī��Ʈ�� �Ѱ� �ø���.
*/

__forceinline void CX2GUChung::UpCannonBallCount( int iUpCount_, bool bForce/*= false*/, bool bNoCheckReloadMastery /*= false*/ )

{
	// ����ȭ ���� �׻� Ǯ ���� �����̹Ƿ� �������� �ʴ´�.
	if ( IsFullHyperMode() )
		return;

	if( bForce == true )
	{
		SetCannonBallCount( GetCannonBallCount() + iUpCount_ );
		return;
	}


	if( bNoCheckReloadMastery == false && m_fInsertCannonBallRel >= GetRandomFloat() )
		SetCannonBallCount( GetCannonBallCount() + iUpCount_ + 1 );
	else
		SetCannonBallCount( GetCannonBallCount() + iUpCount_ );

}

/** @function : UpCannonBallCountAtThisAnimationTime
	@brief : ������ �ִϸ��̼� Ÿ�ӿ� ĳ���� ī��Ʈ�� �Ѱ� �ø���. (m_pXSkinAnim�� animation �ð��� üũ��)
*/
void CX2GUChung::UpCannonBallCountAtThisAnimationTime( float fAnimationTimeNeededToChargeCannonBall_, int iUpCount_ )
{
	if( m_fCannonBallExtraChargeRate >= GetRandomFloat( 1 ) )
	{
		++iUpCount_;
	}

    if ( true == m_pXSkinAnim->EventTimerOneshot( fAnimationTimeNeededToChargeCannonBall_ ) )
	{
		UpCannonBallCount( iUpCount_ );
	}
}

/** @function : UseCannonAtThisAnimationTime
	@brief : ������ �ִϸ��̼� Ÿ�ӿ� ĳ���� ī��Ʈ�� �Ѱ� �Ҹ��Ѵ�. (m_pXSkinAnim�� animation �ð��� üũ��)
*/
void CX2GUChung::UseCannonAtThisAnimationTime( float fAnimationTimeNeededToUseCannonBall_, int iUpCount_ )
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( fAnimationTimeNeededToUseCannonBall_ ) )
	{
		UseCannon( iUpCount_ );
	}
}

/** @function : UseCannon
	@brief : ĳ���� ��� �� �� ȣ�� �ؾ��Ѵ�. (����ȭ�� �ƴ� ��� ĳ������ �Ҹ�, ����ȭ ���ο� ������� ���� ������Ʈ���� ĳ�� ��� ������ �ջ���)
	@param : int iCount_( ��� �ÿ� �ʿ��� ĳ������ ���� )
*/
void CX2GUChung::UseCannon( int iCount_ )
{
	// ����ȭ ��忡���� ĳ������ �Һ� ���� �ʴ´�.
	if ( false == IsFullHyperMode() )
	{
		// �Ʒ� �ڵ�� ĳ���� ����� ������ Ȯ�� ��� ( 5���� ���� 1���� ���� 1���� �ٽ� ä���ش�. )
		if ( RandomFloat () <= m_fRateGasPistonSystemAutoRecharge )
		{
			PlaySound ( L"Chung_CannonGun_Reload.ogg");	// ö��!
			iCount_ -= 1;
			if ( iCount_ < 0 )
				iCount_ = 0;
		}

		ConsumeCannonBallCount( iCount_ );
	}

	m_CannonBallSystem.SetUsedCannonballCountAtThisState( m_CannonBallSystem.GetUsedCannonballCountAtThisState() + iCount_ );
}

/** @function : ConsumeCannonBallCount
	@brief : ĳ������ �Ѱ� �Ҹ��Ѵ�.
*/
void CX2GUChung::ConsumeCannonBallCount( int iCount_ )
{
	SetCannonBallCount( GetCannonBallCount() - iCount_ );
}

/** @function : CanUseCannon
	@brief : ĳ���� ����� �� �ִ��� ���� �Ǵ��� �� ������
	@return; bool( ��� ���� ���� )
*/
__forceinline bool CX2GUChung::CanUseCannon()
{
	if ( false == IsFullHyperMode() )
	{
		// ����ȭ ��尡 �ƴ� ���� ĳ������ ������ true
		return ( false == IsEmptyCannonBallCount() );
	}
	else
	{
		// ����ȭ ��忡���� ������ ����� �� �ִ�.
		return true;
	}
}

/** @function : IsEmptyCannonBallCount
	@brief : ������ ĳ������ ���°�?
	@return : ������ true, ������ false
*/
__forceinline bool CX2GUChung::IsEmptyCannonBallCount()
{
	return GetChungGageData()->IsEmptyCannonBallCount();
}

/** @function : IsFullCannonBallCount
	@brief : ĳ������ ��� ä���������� �˻�
	@return : ��� ä�������� true, �׷��� ������ false
*/
__forceinline bool CX2GUChung::IsFullCannonBallCount()
{
	return GetChungGageData()->IsFullCannonBallCount();
}

/** @function : GetCannonBallCount
	@brief : ���� ������ ĳ������ ������ ������
	@return : int ������ ĳ������ ����
*/
int	CX2GUChung::GetCannonBallCount()
{
	return GetChungGageData()->GetNowCannonBallCount();
}
__forceinline int	CX2GUChung::GetTotalCannonBallCount()
{
	/// ��Ƽ�� Ʈ���� �� ���� Ư�� ��ź ����
	if ( CX2Unit::UC_CHUNG_TACTICAL_TROOPER == GetUnitClass() && true == m_bMobileArmoredModeEnable )
		return GetExtraCannonBallCount() * 7 + GetCannonBallCount();
	else
		return GetCannonBallCount();

}
__forceinline int	CX2GUChung::GetExtraCannonBallCount()
{
	return GetChungGageData()->GetNowCannonBallCountEx();
}
void CX2GUChung::SetTotalCannonBallCount( int iTotalCannonBallCount_ )
{
	/// ��Ƽ�� Ʈ���� �� ���� Ư�� ��ź ����
	if ( CX2Unit::UC_CHUNG_TACTICAL_TROOPER == GetUnitClass() && true == m_bMobileArmoredModeEnable )
	{
		int iCannonBallCount = iTotalCannonBallCount_ % 7;
		int iExtraCannonBallCount = (iTotalCannonBallCount_ - iCannonBallCount) / 7;

		SetCannonBallCount( iCannonBallCount );
		SetExtraCannonBallCount( iExtraCannonBallCount );
	}
	else
	{
		SetCannonBallCount( iTotalCannonBallCount_ );
	}

}

/** @function : InitPromotionEquipList
	@brief : ���θ�� ��� ����Ʈ�� �ʱ�ȭ �Ѵ�.
*/
void CX2GUChung::InitPromotionEquipList()
{
	m_vecPromotionEquipList.reserve( m_vecPromotionItemIDList.size() );

	for( int iItemID : m_vecPromotionItemIDList )
	{
		CX2EqipPtr pEquipPtr = CX2ItemManager::CreateDummyEquip( iItemID, GetUnit(), m_pXSkinAnim );
		pEquipPtr->SetShowObject( false );

		if ( pEquipPtr != NULL )
		{
			pEquipPtr->SetManagerObject( this );
			m_vecPromotionEquipList.push_back( pEquipPtr );
		}//if

	}
}

/** @function : ShowActiveSkillCutInAndLight
	@brief : �������� �ð��� ����ȭ��忡 ���� ��ų ���ΰ� light�� �����
	@param : float fTimeToShow_, bool bOnlyLight_ (
*/
void CX2GUChung::ShowActiveSkillCutInAndLightByBerserkMode( float fTimeToShow_, bool bOnlyLight_ /*= false*/ )
{
    if( m_pXSkinAnim->EventTimerOneshot( fTimeToShow_ ) )
	{
		if ( GetShowCutInAndChangeWorldColor() && GetShowActiveSkillShow() )
		{
			// ����ȭ ���¸�
			if ( IsFullHyperMode() )
				ActiveSkillShow( L"Dummy1_Rhand", s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[1].fileName.c_str(), s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[1].vSize, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[1].vPosition, IsHyperState(), bOnlyLight_ );
			else
				ActiveSkillShow( L"Dummy1_Rhand", s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].fileName.c_str(), s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].vSize, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].vPosition, IsHyperState(), bOnlyLight_ );
		}
	}
}

/** @function : ResetCannonBallCountWhenRebirthInPVPGame
	@brief : ��Ȱ���� �� �츮���� ������� ų���� ���� ĳ���� ������ ������
	@param : int iNumOfMyTeamKill_ (�츮���� ų��), int iNumOfEnmyTeamKill_ (������� ų��)
*/
void CX2GUChung::ResetCannonBallCountWhenRebirthInPVPGame( int iNumOfMyTeamKill_, int iNumOfEnmyTeamKill_ )
{
	// �������� �׾��ٰ� ��Ȱ�ϴ� ���
	// ���ų� ���� �ְ�
	if ( iNumOfMyTeamKill_ <= iNumOfEnmyTeamKill_ )
	{
		// ĳ������ ������ ó�� ������ ������ �ִ� ���� ���� ������
		if ( GetCannonBallCount() < GetBasicCannonBallCount() )
			SetCannonBallCount( GetBasicCannonBallCount() ); // �⺻ ������ ����
	}
	// �Ʊ��� �̱�������� ������
	else
	{
		// ������ �⺻ ������ ����
		SetCannonBallCount( GetBasicCannonBallCount() );
	}

	// �̱�� �ִµ� �⺻ �������� ������ �׳� ����
}

/** @function : ShowOnomatopoeiaParticle
	@brief : '��' �ϴ� �Ǽ��� ��ƼŬ�� ���
	@param : const wstring& wstrWeaponBoneName_ (��ƼŬ�� ���� ���� �� �̸�), const wstring& wstrParticleName_ (��ƼŬ ��)
*/
void CX2GUChung::ShowOnomatopoeiaParticle( const WCHAR* wszWeaponBoneName_, const WCHAR* wszParticleName_ )
{
	if ( GetShowSmallGageAndName() )
	{
		D3DXVECTOR3 vWeaponBonePos = GetWeaponBonePos( 0, wszWeaponBoneName_, 0 );
		D3DXVECTOR2 projPos = g_pKTDXApp->GetProj3DPos( vWeaponBonePos );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  wszParticleName_, projPos.x, projPos.y, 0.f );
	}
}
/** @function : NoStateChangeActionFrameMove
	@brief : ������Ʈ�� ��ȭ�� ���� ��ų ��� �� ����Ǵ� FrameMove
*/
/*virtual*/ void CX2GUChung::NoStateChangeActionFrameMove()
{
}

/*virtual*/ float CX2GUChung::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );

	switch( eSkillID_ )
	{
		case CX2SkillTree::SI_A_CDC_RUMBLE_SHOT:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO13 ) == true )
			{
				fMpConsumption = 0.f;
			}
		} break;
		case CX2SkillTree::SI_SA_CFG_PANDEMONIUM_CHAOS:
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO19 ) == true )
					fMpConsumption -= 20.f;
			} break;

		case CX2SkillTree::SI_SA_CSG_SHOOTING_STAR:
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO24 ) == true )
					fMpConsumption -= 15.f;
			} break;

		case CX2SkillTree::SI_A_CHG_CANNON_STRIKE:
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO28 ) == true )
					fMpConsumption += 10.f;
			} break;

		case CX2SkillTree::SI_SA_CHG_CARPET_BOMBING:
			{
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO30) == true )
					fMpConsumption *= 0.9f;
			} break;
	default:
		break;
	}

	if( fMpConsumption < 0.f )
		fMpConsumption = 0.f;

	if ( true == IsFullHyperMode() && m_fDecreaseRate > 0 && eSkillID_ != CX2SkillTree::SI_HA_CDC_OUTRAGE_STRIKE )
	{
		fMpConsumption = fMpConsumption * m_fDecreaseRate;
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	float fMpDecreaseRate = 1.0f;
	if( NULL != pSkillTemplet )
		fMpDecreaseRate  =  g_pData->GetMyUser()->GetSelectUnit()->GetSkillMpDecreaseRate(eSkillID_, pSkillTemplet->m_eType);

	return fMpConsumption * fMpDecreaseRate;
}

/** @function : LoadPersonalInfo
	@brief : û���� ������ �ִ� �Ľ��� ���� ���� �ε���
*/
void CX2GUChung::LoadPersonalInfo( IN KLuaManager& luaManager_ )
{
	if ( true == luaManager_.BeginTable( "INIT_PERSONAL_INFO" ) )
	{
		LoadStateIDCanBeChangedGuardState( luaManager_ );

		// �⺻������ �ִ�� ���� �� �� �ִ� ĳ���� ����
		int iBasicMaxCannonBallCount = 0;
		LUA_GET_VALUE( luaManager_, "BASIC_MAX_CANNON_BALL_COUNT", iBasicMaxCannonBallCount, 6 );
		SetMaxCannonBallCount( iBasicMaxCannonBallCount );
		SetMaxExtraCannonBallCount( 6 );

		InitRemodelMagazinePassiveSkill();

		// �⺻���� �����Ǿ� ���� ĳ���� ����
		int iCannonBallCount = 0;
		LUA_GET_VALUE( luaManager_, "CANNON_BALL_COUNT_AT_DUNGEON_OR_PVP_START", iCannonBallCount, 0 );
		SetBasicCannonBallCount( iCannonBallCount );
		//SetCannonBallCount( GetBasicCannonBallCount() );

		// ������¿����� ����
		int iDamageReducePercentGuardState = 0;
		LUA_GET_VALUE( luaManager_, "DAMAGE_REDUCE_PERCENT_AT_GUARD_STATE", iDamageReducePercentGuardState, 0 );
		m_GuardSystem.SetDamageReducePercentGuardState( iDamageReducePercentGuardState );

		//{{ kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ
		// ��� �̻��� ����� ������ �ְ�, ���������� ���� �ý��� ������ �������� �̻��� �����ٸ�
		if ( CX2User::XUAL_OPERATOR <= g_pData->GetMyUser()->GetAuthLevel() && true == m_bVerifyGuardSystem )
		{
			// ���� �ý��� ������ �������� �̻��� ����ٸ�
			if ( 0 >= m_GuardSystem.GetDamageReducePercentGuardState() )
			{
				m_bVerifyGuardSystem = false;
				m_wstrFunctionName = L"LoadPersonalInfo()";
			}
		}
		//}} kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ

		// ǻ��������� ���� ��ų�� ���ؼ� ������ ���� ����ٽ��� ������ �������� ����
		SetReduceGuardDamageAtHeavyStanceFromGuardSkill();

		luaManager_.EndTable();
	}
}

/** @function : LoadStateIDCanBeChangedGuardState
	@brief : ���� ������Ʈ�� �Ѿ �� �ִ� ������Ʈ ID ����Ʈ�� ��ũ��Ʈ�� ���� �о����
	@param : KLuaManager �� ���۷���
*/
void CX2GUChung::LoadStateIDCanBeChangedGuardState( IN KLuaManager& luaManager_ )
{
	if ( true == luaManager_.BeginTable( "CHUNG_STATE_ID_LIST_ABLE_TO_BE_CHANGED_TO_GUARD_STATE" ) )
	{
		int iIndex = 1;
		CHUNG_STATE_ID eChungStateID = CSI_BASE;

		while( luaManager_.BeginTable(iIndex) == true )
		{
			GUARD_STATE_INFO stGuardStateInfo;

			LUA_GET_VALUE_ENUM( luaManager_, 1, eChungStateID, CHUNG_STATE_ID, CSI_BASE );
			LUA_GET_VALUE( luaManager_, 2, stGuardStateInfo.m_vecGuardTimer.x, 0.f );
			LUA_GET_VALUE( luaManager_, 3, stGuardStateInfo.m_vecGuardTimer.y, 100.f );

			if ( CSI_BASE != eChungStateID )
			{
				stGuardStateInfo.m_eStateId = eChungStateID;
				m_vecStateIDCanBeChangedToGuardState.push_back( stGuardStateInfo );
			}

			++iIndex;
			luaManager_.EndTable();
		}

		luaManager_.EndTable();
	}
}

/** @function : LoadPromotionItemID
	@brief : ��ũ��Ʈ���� �� Ŭ������ �ش��ϴ� ���θ�� ��� ID�� ����
	@param : KLuaManager�� ���۷���
*/
void CX2GUChung::LoadPromotionItemID( IN KLuaManager& luaManager_ )
{
	if ( true == luaManager_.BeginTable( "PROMOTION_ITEM_ID_LIST" ) )
	{
		bool bOpenTable = false;
		switch ( GetUnit()->GetClass() )
		{
			case CX2Unit::UC_CHUNG_IRON_CANNON:
			{
				bOpenTable = luaManager_.BeginTable( "CHUNG_IRON_CANNON" );
			} break;

		//{{ kimhc // 2011.1.19 // û 1�� ����

			case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
			{
				bOpenTable = luaManager_.BeginTable( "CHUNG_FURY_GUARDIAN" );
			} break;

			case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
			{
				bOpenTable = luaManager_.BeginTable( "CHUNG_SHOOTING_GUARDIAN" );
			} break;

		//}} kimhc // 2011.1.19 // û 1�� ����
			case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
			{
				bOpenTable = luaManager_.BeginTable( "CHUNG_SHELLING_GUARDIAN" );
			} break;

			case CX2Unit::UC_CHUNG_IRON_PALADIN:
				{
					bOpenTable = luaManager_.BeginTable( "CHUNG_IRON_PALADIN" );
				} break;

			case CX2Unit::UC_CHUNG_DEADLY_CHASER:
				{
					bOpenTable = luaManager_.BeginTable( "CHUNG_DEADLY_CHASER" );
				} break;

			case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
				{
					bOpenTable = luaManager_.BeginTable( "CHUNG_TACTICAL_TROOPER" );
				} break;

			default:
			{
				ASSERT( !L"Wrong Path" );
				bOpenTable = luaManager_.BeginTable( "CHUNG_IRON_CANNON" );
			} break;
		}

		if ( true == bOpenTable )
		{
			int iIndex = 1;
			int iItemID = 0;

			do
			{
				LUA_GET_VALUE( luaManager_, iIndex++, iItemID, 0 );
				if ( 0 != iItemID )
					m_vecPromotionItemIDList.push_back( iItemID );

			} while ( iItemID != 0 );

			luaManager_.EndTable();
		} // if

		luaManager_.EndTable();
	}
}

/** @function : IsThisStateCanBeChangedToGuardStateOnHit
	@brief : Ÿ���� �޾��� �� Guard ���·� ���� �� �� �ִ� ������Ʈ �ΰ��� ����
	@return : bool (true �� ���� ���·� ���� ����)
*/
bool CX2GUChung::IsThisStateCanBeChangedToGuardStateOnHit()
{
	CHUNG_STATE_ID eChungStateID = CHUNG_STATE_ID( GetNowStateID() );

	for(UINT i=0; i<m_vecStateIDCanBeChangedToGuardState.size(); ++i)
	{
		GUARD_STATE_INFO stGuardStateInfo = m_vecStateIDCanBeChangedToGuardState[i];
		if( stGuardStateInfo.m_eStateId == eChungStateID )
		{
			if( stGuardStateInfo.m_vecGuardTimer.x <= m_pXSkinAnim->GetNowAnimationTime() && //m_FrameDataNow.unitCondition.fStateTime &&
				stGuardStateInfo.m_vecGuardTimer.y >= m_pXSkinAnim->GetNowAnimationTime() )
			{
				// ������°� �ƴϸ� true
				if ( CSI_GUARD != stGuardStateInfo.m_eStateId )
					return true;
				// ������¿� ��������Ʈ�� 0���� ũ�� true
				else if ( 0 < m_GuardSystem.GetGuardPointRemain() )
					return true;
				// ��������ε� ���� �� �� ���� ���¸� false
				else
					return false;
			}
			else
				return false;
		}
	}

	return false;
}

/** @function : NotifyShowObjectChanged
	@brief : û�� ���̰ų� �Ǵ� ������ �ʰ� �ɶ� ���� ���̰ų� ������ �ʾƾ� �� �͵��� ó��
*/
/*virtual*/ void CX2GUChung::NotifyShowObjectChanged()
{
	CX2GUUser::NotifyShowObjectChanged();

	// û�� ������ �ʴ´ٸ�
	SetShowViewList( GetShowObject() );
	UpdateEquipCollisionData();
}

/** @function : NotifyDGManagerChanged
	@brief : û�� DGManager�� ��� �Ǵ� ���� �ɶ� ���� ��� �ǰų� �����Ǿ�� �� �͵��� ó����
	@param : CKTDGManager& refInManager_
*/
/*virtual*/ void CX2GUChung::NotifyDGManagerChanged( CKTDGManager& refInManager_ )		// DGManager�� ���
{
	CX2GUUser::NotifyDGManagerChanged( refInManager_ );

	if ( IsRecursivelyManagedByDGManager() )
	{
		for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
		{
			if ( pEquipPtr != NULL )
				refInManager_.AddObjectChain( pEquipPtr.get() );
		}
	}
	else
	{
		for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
		{
			if ( pEquipPtr != NULL )
				refInManager_.RemoveObjectChain( pEquipPtr.get() );
		}
	}
}

/** @function : SetShowPromotionEquips
	@brief : ���θ�� ��� �������� ���θ� ����
	@param : bool bShow_
*/
void CX2GUChung::SetShowPromotionEquips( bool bShow_ )
{
	// ���θ�� ��� ������ ��
	for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
	{
		if ( pEquipPtr != NULL )
		{
			pEquipPtr->SetShowObject( bShow_ );

			if ( true == bShow_ && CX2Unit::EP_WEAPON_HAND == pEquipPtr->GetEqipPos() )
			{
				ReplaceWeapon( pEquipPtr );
			}
		}
	}
}

/** @function : SetShowViewList
	@brief : ����ȭ ���� �ƴ����� ���� ��� ��� �������� �� ������
	@param : bool bShow_
*/
/*virtual*/ void CX2GUChung::SetShowViewList( bool bShow_ )
{
	// ����ȭ ��� ��
	if ( IsFullHyperMode() )
	{
		SetShowOriginalViewList( false );
		SetShowPromotionEquips( bShow_ );
	}
	// ����ȭ ��尡 �ƴϸ�
	else
	{
		SetShowPromotionEquips( false );
		SetShowOriginalViewList( bShow_ );
	}
}

/** @function : SetShowOriginalViewList
	@brief : ViewList�� ��� ������
	@param : bool bShow_ (�������� ���� ����)
*/
void CX2GUChung::SetShowOriginalViewList( bool bShow_ )
{
	for( CX2EqipPtr pViewEquipPtr : m_ViewEqipList )
	{
		if ( pViewEquipPtr != NULL )
		{
			pViewEquipPtr->SetShowObject( bShow_ );

			if ( true == bShow_ && CX2Unit::EP_WEAPON_HAND == pViewEquipPtr->GetEqipPos() )
			{
				ReplaceWeapon( pViewEquipPtr );
			}
		}
	}
}
/** @function : ReplaceWeapon
	@brief : ����ȭ���� �����ϰų� ������ �� ���� ����
	@param : CX2EqipPtr pWeaponPtr_ ( pWeaponPtr_�� �ٲ�� )
*/
void CX2GUChung::ReplaceWeapon( CX2EqipPtr pWeaponPtr_ )
{
	// kimhc // 2010-12-13 // ���� û�� m_vecpWeapon�� �Ѱ��� �����
	CX2GameUnit::Weapon* pOldWeapon = m_vecpWeapon[0];

	if ( NULL != pOldWeapon &&
		 pOldWeapon->m_pUserWeaponEquip != NULL &&
		 pOldWeapon->m_pUserWeaponEquip != pWeaponPtr_ &&
		 pOldWeapon->m_pUserWeaponEquip->GetItem() != NULL )
	{
		int iEnchantLevel = pOldWeapon->m_pUserWeaponEquip->GetEnchantLevel();
		const CX2Item::ItemTemplet* pOldItemTemplet =
            pOldWeapon->m_pUserWeaponEquip->GetItem()->GetItemTemplet();

		pOldWeapon->m_pUserWeaponEquip.reset();
		SAFE_DELETE_KTDGOBJECT( pOldWeapon );
		m_vecpWeapon.resize(0);

		// ��ȭ ����Ʈ�� ���� Enchant level ����
		pWeaponPtr_->ChangeEnchantLevel( iEnchantLevel );
		CreateWeapon( pOldItemTemplet, pWeaponPtr_ );
	}
}

/** @function : UpdateEquipViewListInfo
	@brief : ����� ����, Ż��, �׸��� ����ȭ ���ο� ���� ViewList�� ������
	@param : CX2Item* pItemToAddInViewList(�߰��� ���), CX2Item* const pItemToRemoveInViewList(Ż���� ���)
	@return : �Լ��� ���� ����
*/
/*virtual*/ bool CX2GUChung::UpdateEquipViewListInfo( CX2Item* pItemToAddInViewList, CX2Item* const pItemToRemoveInViewList )
{
	CX2EqipPtr pEquipPtrToRemove;
	int iIndexInViewListForRemove_ = 0;
	if ( false == GetAddItemAndRemoveItemToViewList( pEquipPtrToRemove, &pItemToAddInViewList, pItemToRemoveInViewList, iIndexInViewListForRemove_ ) )
		return false;

	if ( pEquipPtrToRemove.get() != NULL )
	{
		RemoveEquipFromViewList( pEquipPtrToRemove, iIndexInViewListForRemove_ );
	}

	if ( pItemToAddInViewList != NULL )
	{
		AddEquipToViewList( pItemToAddInViewList );
	}

	// �̰��� ����ϴ� ��ƾ���� UpdateEquipCollisionData()�� ȣ���ϱ� ������
	// ���⼭�� ȣ�� �� �ʿ䰡 ����.
	return true;
}

/** @function : UpdateEquipCollisionData
	@brief : ���� ���̴� ��� ���� �浹 �����͸� �����Ѵ�.
*/
/*virtual*/ void CX2GUChung::UpdateEquipCollisionData()
{
	// ����ȭ ��� �̸�
	if ( IsFullHyperMode() )
	{
		CX2GUUser::UpdateEquipCollisionData( m_vecPromotionEquipList );
	}
	// �Ϲ� ��� �̸�
	else
	{
		CX2GUUser::SetShowViewList( true );
		CX2GUUser::UpdateNotRenderingOtherEquipList();
		CX2GUUser::UpdateEquipCollisionData( m_ViewEqipList );
	}

	SetEnableAttackBox( L"Rfoot", false );
	SetEnableAttackBox( L"Lfoot", false );
	SetEnableAttackBox( L"Lhand2", false );
}

/** @function : OnFrameMove
	@brief : û�� ������ ���� �Լ� ( ������ ����Ʈ�� ������Ʈ �� �ʿ䰡 ������ ������Ʈ�� )
	@param : double fTime, float fElapsedTime
	@return : HRESULT
*/
/*virtual*/ HRESULT CX2GUChung::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2GUUser::OnFrameMove( fTime, fElapsedTime );

	if ( GetNeedToUpdateWeaponEffect() )
	{
		UpdateWeaponEnchantEffectAndExtraDamageType();
		SetNeedToUpdateWeaponEffect( false );
	}

	return S_OK;
}

//{{ kimhc // 2011.1.19 // û 1�� ����
/** @function : InitRemodelMagazinePassiveSkill
	@brief : û 1������ ���� ������� źâ/��� ���� �нú� ��ų Init
*/
void CX2GUChung::InitRemodelMagazinePassiveSkill()
{
	// ĳ���� UI�� ���� �������� �ʾҴٸ� �������� ����
//	if ( m_GageManager.GetCannonBallUIPtr() == NULL )
//		return;

	const CX2UserSkillTree& userSkillTree	= GetUnit()->GetUnitData().m_UserSkillTree;

	const CX2SkillTree* const pSkillTree	= g_pData->GetSkillTree();

	// źâ/��� ����(���ð����)
	const int iSkillLevelRemodelMagazine	= userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CSG_REMODEL_MAGAZINE, true );

	if ( 0 < iSkillLevelRemodelMagazine )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_CSG_REMODEL_MAGAZINE );

		// ó�� Init�ÿ��� ������� ����
		if ( NULL != pSkillTemplet )
		{
			int iMaxCannonBallCount = static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EXPAND_CB, iSkillLevelRemodelMagazine ) );
			SetMaxCannonBallCount( iMaxCannonBallCount );
		} // if
	} // if
}

/** @function : GetCriticalValue
	@brief : ũ��Ƽ�� ���� ������ ũ��Ƽ�� �������� ��� �� �� ���
	@param : float fDefenderCritical_ (�ǰ����� ũ��Ƽ�� �����)
	@return : float (ũ��Ƽ�� ������ ����)
*/
/*virtual*/ float CX2GUChung::GetCriticalValue( float fDefenderCritical_ /*= 0.f*/ )
{
	float fCriticalValue = CX2GameUnit::GetCriticalValue( fDefenderCritical_ );

	if ( true == m_bIncreaseCritical )
		return fCriticalValue + GetCriticalDamageRate();
	else
		return fCriticalValue;
}

/** @function : CalculateHpPercentUpRateByDamageDataHpUp
	@brief : ����������Ÿ�� Hp�� Up�����ִ� ���� ���� (ǻ��������� ������� ���� �нú� ��ų)
	@param : fDamageDataHpPercentUp_ (����������Ÿ ���� Hp ������)
	@return : float (��ų�� ���� ������ Percent)
*/
float CX2GUChung::CalculateHpPercentUpRateByDamageDataHpUp( float fDamageDataHpPercentUp_ )
{
	float fHpUpRate = 0.0f;

	if ( 0.f < fDamageDataHpPercentUp_ )
	{
		// ǻ������� ������� ����
		if ( CX2Unit::UC_CHUNG_FURY_GUARDIAN == GetUnitClass() ||
			 CX2Unit::UC_CHUNG_IRON_PALADIN == GetUnitClass() )
		{
			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			int iSkillLv = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST, true );

			if ( 0 < iSkillLv )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet =
					g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CFG_METABOLISM_BOOST );

				if ( NULL != pSkillTemplet )
				{
					float fIncreaseRateMetabolism	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ITEM_SPECIAL_ABILITY_REL, iSkillLv );
					fIncreaseRateMetabolism			= CalculateIncreasingRate( fIncreaseRateMetabolism );
					// ���� fDamageDataHpPercentUp_�� 10% �̰� fIncreaseRateMetabolism�� 25% �� �������� �� 2.5%
					fHpUpRate =  fDamageDataHpPercentUp_ * fIncreaseRateMetabolism;
				}
			} // if
		}
	} // if

	return fHpUpRate;
}

/** @function : SetReduceGuardDamageAtHeavyStanceFromGuardSkill
	@brief : ǻ��������� ���� ��ų�� ���ؼ� ������ ���� ����ٽ��� ������ �������� ����
*/
void CX2GUChung::SetReduceGuardDamageAtHeavyStanceFromGuardSkill()
{
	// ����(ǻ�������) // ��Ƽ�� ��ų������ �нú� ���� ��ɵ� ���� (����� ������ ������ ���)
	int iSkillLevelGuard = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_CFG_GUARD );

	if( 0 < iSkillLevelGuard )
	{

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_CFG_GUARD );

		if( NULL != pSkillTemplet )
		{
			int iReduceDamagePercent = 100 - static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillLevelGuard ) * 100 );
			m_GuardSystem.SetDamageReducePercentGuardState( iReduceDamagePercent );

			//{{ kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ
			// ��� �̻��� ����� ������ �ְ�, ���������� ���� �ý��� ������ �������� �̻��� �����ٸ�
			if ( CX2User::XUAL_OPERATOR <= g_pData->GetMyUser()->GetAuthLevel()  )
			{
				// ���� �ý��� ������ �������� �̻��� ����ٸ�
				if ( 0 >= m_GuardSystem.GetDamageReducePercentGuardState() && true == m_bVerifyGuardSystem )
				{
					m_bVerifyGuardSystem = false;
					m_wstrFunctionName = L"SetReduceGuardDamageAtHeavyStanceFromGuardSkill()";
				}
			}
			//}} kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ
		}
	}
}

//}} kimhc // 2011.1.19 // û 1�� ����

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
//							������Ʈ �Լ� ����
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

/** @function : DieFrontStartFuture
	@brief : �տ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUChung::DieFrontStartFuture()
{
	CommonStateStartFuture();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnimFuture->SetPlaySpeed( PLAY_SPEED );
}

/** @function : DieFrontStart
	@brief : �տ��� ���ݹ޾� �״� ������ Start �Լ�
*/
void CX2GUChung::DieFrontStart()
{
	CommonStateStart();
	DyingStart();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnim->SetPlaySpeed( PLAY_SPEED );

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER );
}

/** @function : DieFrontFrameMove
	@brief : �տ��� ���ݹ޾� �״� ������ FrameMove �Լ�
*/
void CX2GUChung::DieFrontFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

/** @function : DieFrontEventProcess
	@brief : �տ��� ���ݹ޾� �״� ������ EventProcess �Լ�
*/
void CX2GUChung::DieFrontEventProcess()
{
	if ( false == GetShowObject() && true == m_pXSkinAnim->IsAnimationEnd() )
		StateChange( GUSI_DIE );
}

/** @function : DieBackStartFuture
	@brief : �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUChung::DieBackStartFuture()
{
	CommonStateStartFuture();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnimFuture->SetPlaySpeed( PLAY_SPEED );
}

/** @function : DieBackStart
	@brief : �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUChung::DieBackStart()
{
	CommonStateStart();
	DyingStart();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnim->SetPlaySpeed( PLAY_SPEED );

	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER );
}

/** @function : DieBackFrameMove
	@brief : �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUChung::DieBackFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

/** @function : DieBackEventProcess
	@brief : �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUChung::DieBackEventProcess()
{
	if ( false == GetShowObject() && true == m_pXSkinAnim->IsAnimationEnd() )
		StateChange( GUSI_DIE );
}

/** @function :WaitStartFuture
	@brief : Wait ������ StartFuture() �Լ�
*/
void CX2GUChung::WaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );

	// Wait���¸� ������ �� -m_fElapsedTime���� �ʱ�ȭ �� (-m_fElapsedTime���� �� ������ ���� �����ӿ����� ����ð��� �������� �ʱ� ���ؼ� �̴�.)
	m_CannonBallSystem.SetTimeElapsedAtWaitState( -m_fElapsedTime );
}

/** @function : Wait ������ StartFuture() �Լ�
	@brief : HP�� ���� Wait or Fatal ����� �÷�����
*/
void CX2GUChung::WaitStart()
{
	CommonStateStart();
	PlayAnimationWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}

/** @function : WaitFrameMoveFuture
	@brief : Wait�� FrameMoveFuture (ĳ���� ���� ���·��� ��ȯ�� ����, Wait ���¿����� ����ð��� üũ �� ��� ��)
*/
void CX2GUChung::WaitFrameMoveFuture()
{
	// Wait ���¿����� ��� �ð��� ��� �ð��� ������
	m_CannonBallSystem.SetTimeElapsedAtWaitState( m_CannonBallSystem.GetTimeElapsedAtWaitState() + m_fElapsedTime );

	CommonFrameMoveFuture();
}

/** @function : WaitEventProcess
	@brief : HP�� ���� Wait or Fatal ����� �÷�����
*/
void CX2GUChung::WaitEventProcess()
{
	if( m_fChaosCannonTime > 0.f )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WAIT );
		CommonEventProcess();
		return;
	}

	if ( false == IsOnSomethingFuture() )	// ��� �ִ� ���� ������
	{
		// �ϰ�
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	} // if
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			StateChange( CSI_CDC_COMBO_X );
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			StateChange( CSI_CHG_COMBO_X );
		else
			StateChange( CSI_COMBO_X );
	}
	// wait���¿����� ĳ���� �������·� ���ϱ����� �ʿ��� �ð���ŭ ��� �߰�
	// ĳ������ ���� ���°� Full �� �ƴϰ�
	// �ɱ� �̸�� ���°� �ƴ϶��
	else if ( false == IsFullHyperMode() && ( m_CannonBallSystem.GetTimeElapsedAtWaitState() >= m_CannonBallSystem.GetTimeNeededToChangeCannonBallChargeState() )
			  && false == IsFullCannonBallCount()
			  && CX2Unit::ET_SITWAIT != GetNowEmotionId() && CX2Unit::ET_SITREADY != GetNowEmotionId() )
	{
		StateChange( CSI_CHARGE_CANNON_BALL );
	}
	else if ( true == SpecialAttackEventProcess() )	// Wait ���¿��� ��ų�� ��� �� �� ����
	{
	} // else if
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		// ��� �� �� ������ ���, �׷��� ������ walk
		StateChangeDashIfPossible();
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )	// ����
	else if( true == m_InputData.oneDown	// �Ʒ� ���� ȭ��ǥ�� ������, ���θ� �Ʒ��� ������ �� ������
			 &&	true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function : WalkEventProcess
	@brief : Walk ������ EventProcess
*/
void CX2GUChung::WalkEventProcess()
{
	if ( false == IsOnSomethingFuture() )	// ��� �ִ� ���� ������
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == SpecialAttackEventProcess() ) // walk ���¿��� ��ų�� ��� �� �� ����
	{

	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			StateChange( CSI_CDC_COMBO_X );
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			StateChange( CSI_CHG_COMBO_X );
		else
			StateChange( CSI_COMBO_X );
	}
	else if ( false == m_InputData.pureRight && false == m_InputData.pureLeft ) // �̵� �Է��� ������ ����
	{
		StateChange( USI_WAIT );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )	// ����
	else if( true == m_InputData.oneDown	// �Ʒ� ���� ȭ��ǥ�� ������, ���θ� �Ʒ��� ������ �� ������
			 &&	true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function : JumpReadyEventProcess
	@brief : �����ϱ� �ٷ� ������ ���� EventProcess
*/
void CX2GUChung::JumpReadyEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd()
			|| true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}

	CommonEventProcess();
}

/** @function : JumpUpEventProcess
	@brief : JumpUp ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::JumpUpEventProcess()
{
	// ��� �߿� ������ �ߴ��� �Ǵ�
	if( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
	{
		if( true == GetEnableDash() )
		StateChange( USI_DASH_JUMP );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )	// ���� ���¿��� �����ִ� �ʻ�� ��� ����
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_ATTACK_JUMP_Z );
		m_pComboGuide->ShowComboCommand( L"J", L"JZ" );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( CSI_ATTACK_JUMP_X );
		m_pComboGuide->ShowComboCommand( L"J", L"JX" );
	}
	else if( 0.0f >= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;

		// JumpDown�� m_bDownForce�� ���� �̾ ���� �� �� �ֵ��� false ������ �ʴ´�.
		m_bDownForce = true;
	}

	CommonEventProcess();
}

/** @function : JumpDownEventProcess
	@brief : �ϰ��ϴ� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::JumpDownEventProcess()
{
	// �� �Ʒ��� ���θ��� ������ ����
	if ( true == IsOnSomethingFuture() )
	{
		// �Ʒ��� ���� ȭ��ǥ�� ���� ���
		if( true == m_bDownForce )
		{
			StateChange( CSI_POWER_DOWN_LANDING );
			m_bDownForce = false;

			m_pComboGuide->ShowComboCommand(L"J", L"D");
			m_pComboGuide->ShowComboCommand(L"A", L"AXD");
		}
		else
		{
			StateChange( USI_JUMP_LANDING );
			m_bDownForce = false;
		}
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
			 ( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( CSI_WALL_LANDING );
	}
	else if ( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
	{
		if( true == GetEnableDash() )
			StateChange( USI_DASH_JUMP );
	}
	else if ( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_ATTACK_JUMP_Z );
		m_pComboGuide->ShowComboCommand( L"A", L"AXZ" );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( CSI_ATTACK_JUMP_X );
		m_pComboGuide->ShowComboCommand( L"A", L"AXX" );
	}
	else if ( 0.0f <= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_UP );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}

/** @function : JumpLandingEventProcess
	@brief : ���� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::JumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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
		StateChange( CSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			StateChange( CSI_CDC_COMBO_X );
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			StateChange( CSI_CHG_COMBO_X );
		else
			StateChange( CSI_COMBO_X );
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
	// �Ʒ��� ���� ȭ��ǥ Ű�� ������, ������ ���� �� �� �ִٸ�
	else if( true == m_InputData.oneDown
			 && true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ��� Ű�� ��� ������ ������ ��ŵ ����
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		StateChange( USI_WAIT, false );
	}

	CommonEventProcess();
}

/** @function : DashEventProcess
	@brief : Dash ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::DashEventProcess()
{
	if ( false == IsOnSomethingFuture() )	// �� �ؿ� ���θ��� ���� ��
	{
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( CSI_COMBO_DASH_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( CSI_ATTACK_DASH_X );
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.oneUp )	// ��� �߿� ������
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	// ��� �߿� ����, ������ ����Ű�� �Ѵ� ���������� ���
	else if( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		StateChange( USI_DASH_END );
	}
	// ��� �߿� ����, ������ ����Ű�� �Ѵ� �������� �� ���
	else if( m_InputData.pureRight == true && m_InputData.pureLeft == true )
	{
		StateChange( USI_DASH_END );
	}
	// ��� �߿� ������ �ٲ� ���
	else if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		StateChange( USI_DASH_END );
	}
	// ��� �߿� ������ �ٲ� ���
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		StateChange( USI_DASH_END );
	}
	// �Ʒ��� ���� ȭ��ǥ Ű�� ������, ������ ���� �� �� �ִٸ�
	else if( true == m_InputData.oneDown
			 && true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_DASH_JUMP );	// ��� ���� ���·� ����
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function : DashEndStartFuture
	@brief : Dash ����� ���ǵ� 0�� �ʱ�ȭ
*/
void CX2GUChung::DashEndStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = GetDashSpeed();
	m_bApplyMotionOffset = false;
}

/** @function : DashEndFrameMove
	@brief : Dash ���� �� ���� ���
*/
void CX2GUChung::DashEndFrameMove()
{
	if( true == m_pXSkinAnim->EventTimer( 0.4f ) )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}

/** @function : DashEndEventProcess
	@brief : Dash �� ����Ǵ� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::DashEndEventProcess()
{
	// DashEnd���� ���� Ű�� ������ �� ��� ��������, �Ϲ� ���������� ���ϱ� ���� const ��.
	const float TRIGGER_TIME_FACTOR_DASH_ATTACK = 0.3f;

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_InputData.oneZ )
	{
		if ( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() * TRIGGER_TIME_FACTOR_DASH_ATTACK )
		{
			StateChange( CSI_COMBO_Z );
		}
		else
		{
			StateChange( CSI_COMBO_DASH_Z );
		}
	}
	else if ( true == m_InputData.oneX )
	{
		if ( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() * TRIGGER_TIME_FACTOR_DASH_ATTACK )
		{
			if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
				StateChange( CSI_CDC_COMBO_X );
			else if( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
				StateChange( CSI_CHG_COMBO_X );
			else
				StateChange( CSI_COMBO_X );
		}
		else
		{
			StateChange( CSI_ATTACK_DASH_X );
		}
	}
	else if( SpecialAttackEventProcess() == true)
	{
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	// ��ð� ����Ƿ��� ���� ���� �������� ȭ��ǥ Ű���带 �ѹ� �� ������ �ٽ� ��� ����
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	// ��ð� ����Ƿ��� ���� ���� �������� ȭ��ǥ Ű���带 �ѹ� �� ������ �ٽ� ��� ����
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	// ���� ������ �ƴϰ� �׳� ȭ��ǥ Ű���带 �����ٸ�
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_WALK );
	}
	// �Ʒ��� ���� ȭ��ǥ Ű�� ������, ������ ���� �� �� �ִٸ�
	else if( true == m_InputData.oneDown
			 && true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function : DashJumpStartFuture
	@brief : ĳ������ �ϰ� ���ӵ��� �߷� ���ӵ���, DownForce �÷���(���� ���¿��� �Ʒ����� Ű���带 �����°�) �ʱ�ȭ
*/
void CX2GUChung::DashJumpStartFuture()
{
	CommonStateStartFuture();

	const float MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL = 1.2f;

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL;
	m_bDownForce					= false;
}

/** @function : DashJumpStart
	@brief : ��� ���� ���� �� ���θʿ� ��� �ִ� ���¶�� ���� ��ƼŬ ���
*/
void CX2GUChung::DashJumpStart()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
	{
		CreateStepDust();
	}
}

/** @function : DashJumpFrameMoveFuture
	@brief : ���� �޸��� �ִ� �������� ��� ���� ���� �ÿ� ������ ���� �ָ� ������ ��
*/
void CX2GUChung::DashJumpFrameMoveFuture()
{
	const float MAGIC_NUMBER_MULTIPLY_DASH_SPEED = 1.6f;

	// ���� �޸��� �ִ� ����� ���� �������� ��� ������ �ߴ��� �Ǵ�
	if( true == m_InputData.pureRight && true == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}
	else if( true == m_InputData.pureLeft && false == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}

	CommonFrameMoveFuture();
}

/** @function : DashJumpEventProcess
	@brief : Dash Jump ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::DashJumpEventProcess()
{
	bool &bCanDashJump = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// ��� ������ ���θʿ� ���� ��� ( �����ؼ� ���� ���θʿ� �ö� ��� �Ǵ� �ϰ��ϴ� ��� )
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
			StateChange( CSI_POWER_DOWN_LANDING );
			m_bDownForce = false;
			m_pComboGuide->ShowComboCommand(L"A", L"AD");
		} // if
		else
		{
			// �� or �� ������ ȭ��ǥ�� ������ �ִ� ��� ���
			if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
			{
				StateChangeDashIfPossible();
			}
			else
				StateChange( USI_DASH_JUMP_LANDING );

			m_bDownForce = false;
		}
	}
	// Wall�� �����Ǿ� �ִ� ���θʿ��� ���� ���� Ű���带 ���� ���
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
			 ( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( CSI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )	// ���߿��� ����� �� �ִ� �ʻ�⸦ �� �� �ִ�.
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_COMBO_DASH_JUMP_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( CSI_ATTACK_DASH_JUMP_X );
	}
	// ��� ���� �߿� ������ �ٲٸ� JumpState�� ����
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
	// ��� ���� �߿� ������ �ٲٸ� JumpState�� ����
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

	// ��� ���� �� �ϰ��ϴ� ���¿��� ���� ȭ��ǥ�� �ѹ� �� ������
	if( true == m_InputData.oneUp  && 0.0f >= m_PhysicParam.nowSpeed.y  )
	{
		bCanDashJump = true;
	}

	// ��� ���� �� �ϰ��ϴ� ���¿��� �Ʒ��� ȭ��ǥ�� ������
	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
		bCanDashJump = false;
	}

	CommonEventProcess();
}

/** @function : DashJumpEndFuture
	@brief : �ϰ� ���ӵ� �ʱ�ȭ
*/
void CX2GUChung::DashJumpEndFuture()
{
	CommonStateEndFuture();
	m_PhysicParam.fDownAccel	= 0.0f;
}

/** @function : DashJumpLandingEventProcess
	@brief : Dash Jump �� ���� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::DashJumpLandingEventProcess()
{
	// ���� ���� �ε� �� �ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			StateChange( CSI_CDC_COMBO_X );
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			StateChange( CSI_CHG_COMBO_X );
		else
			StateChange( CSI_COMBO_X );
	}
	// �� ���ķδ� ���θʿ� ���� ��� ��
	else if( SpecialAttackEventProcess() == true )
	{
	}
	// ��ø� ���� �� ���
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChangeDashIfPossible();
	}
	// ������ ��� ���� ����� ���� ������ Ű�� ������
	else if( true == m_InputData.oneRight && true == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	else if( true == m_InputData.oneLeft && false == m_FrameDataFuture.syncData.bIsRight )
	{
		StateChangeDashIfPossible();
	}
	// ������ ��� ���� ����� �ٸ� ������ Ű�� ��������
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_WALK );
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( USI_JUMP_READY );
	}
	else if( true == m_InputData.oneDown
			 && true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : PowerLandingStartFuture
	@brief : Dash Jump Power Landing ���۽� �� ����
*/
void CX2GUChung::PowerLandingStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x *= 0.5f; // kimhc // /= 2.0f �� ������ ������ ���� * ������ �� ����
}

/** @function : DashJumpPowerLandingStart
	@brief : ������ �� ���� ����Ʈ ���
*/
void CX2GUChung::PowerDownLandingStart()
{
	CommonStateStart();
	CreateStepDust();
}

/** @function : DashJumpPowerLandingEventProcess
	@brief : Dash Jump Power Landing ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUChung::PowerDownLandingEventProcess()
{
	// �� �ؿ� ���� ���� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// oasis907 : ����� [2011.6.14] û 2�� ���� - ���̾� �ȶ��
	else if ( true == m_InputData.oneZ && GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN )
	{
		const float TRIGGER_TIME_COMBO_PZ	= 0.060f;

		if ( TRIGGER_TIME_COMBO_PZ < m_pXSkinAnimFuture->GetNowAnimationTime() )
			StateChange( CSI_CIP_COMBO_POWER_DOWN_LANDING_Z );
	}
	// ���θʿ��� ������ �Ǵ� ���� ����Ű�� ������ ��
	else if( true == m_InputData.oneRight || true == m_InputData.oneLeft )
	{
		const float TRIGGER_TIME_WALK	= 0.13333f;

		if ( TRIGGER_TIME_WALK < m_pXSkinAnimFuture->GetNowAnimationTime() )
			StateChange( USI_WALK );
	}
	// kimhc // 2010-11-25 // �̺κе� / 6.0f ���ٴ� ���ʿ� * 6.0�� �ϴ°� �� ���� ��...
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 6.0f )
	{
		if( true == SpecialAttackEventProcess() )
		{
		}
		else if( true == m_InputData.pureUp )
		{
			StateChange( USI_JUMP_READY );
		}
		else if( true == m_InputData.oneDown
				 && true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
							LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
							&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function : HyperModeFrameMove
	@brief : ���� ���¿��� ǥ���Ǿ��ϴ� ��ƼŬ ��� �� ���� ���� ī��Ʈ ����
*/
void CX2GUChung::HyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.05f, 0.2f, 0.3f );
}

/** @function : DamageSmallEventProcess
	@brief : �� �Ǵ� �ڿ��� small Ÿ���� �޾��� ���� EventProcess
*/
void CX2GUChung::DamageSmallEventProcess()
{
	float fGapFromUnitToLineMap = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
	const float ACCEPTABLE_GAP = 200.0f;	// �̰ͺ��� �Ÿ��� ũ�� ���߿� �ִ� ������

	if( ACCEPTABLE_GAP < fGapFromUnitToLineMap )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if(  GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO12) == true &&
		g_pMain->GetNowStateID() != CX2Main::XS_PVP_GAME &&
		GetGameUnitStateFuture() == CX2GUUser::USI_DAMAGE_SMALL_FRONT &&
		SpecialAttackEventProcess() == true )
	{
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : DamageBigStart
	@brief : �� �Ǵ� �ڿ��� Big Ÿ���� �޾��� �� ���� ��ƼŬ ���
*/
void CX2GUChung::DamageBigStart()
{
	CommonStateStart();
	CreateStepDust();
}

/** @function : DamageBigEventProcess
	@brief : �� �Ǵ� �ڿ��� Big Ÿ���� �޾��� ���� EventProcess
*/
void CX2GUChung::DamageBigEventProcess()
{
	float fGapFromUnitToLineMap = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
	const float ACCEPTABLE_GAP = 200.0f;	// �̰ͺ��� �Ÿ��� ũ�� ���߿� �ִ� ������

	if( ACCEPTABLE_GAP < fGapFromUnitToLineMap )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if(  GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO12) == true &&
		g_pMain->GetNowStateID() != CX2Main::XS_PVP_GAME &&
		GetGameUnitStateFuture() == CX2GUUser::USI_DAMAGE_BIG_FRONT &&
		SpecialAttackEventProcess() == true )
	{
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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

/** @function : DamageDownFrontFrameMove
	@brief : �տ��� ���� �޾� �Ѿ� �� ������ FrameMove
*/
void CX2GUChung::DamageDownFrontFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}

/** @function : DamageDownFrontEventProcess
	@brief : �տ��� ���� �޾� �Ѿ� �� ������ EventProcess
*/
void CX2GUChung::DamageDownFrontEventProcess()
{
	bool& bChangeStandUpAttackFront			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingFrontFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingFrontBack	= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpCannonAttackFront	= m_FrameDataFuture.stateParam.bEventFlagList[4];

	// �Ѿ����µ� �ؿ� ���θ� ���� ���� ���
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS ;
	}
	// �Ѿ��� ��
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// Z Ű�� ������ ��
		if( true == bChangeStandUpAttackFront )
		{
			StateChange( CSI_STANDUP_ATTACK_FRONT );
		}
		// X Ű�� ������ ��
		else if ( true == bChangeStandUpCannonAttackFront )
		{
			// ĳ���� ����� �� ������
			if ( CanUseCannon() )
				StateChange( CSI_STANDUP_CANNON_ATTACK_FRONT );
			// ĳ���� ����� �� ������
			else
				StateChange( CSI_STANDUP_ATTACK_FRONT );
		}
		else if( true == bChangeStandUpRollingFrontFront )
			StateChange( CSI_STANDUP_ROLLING_FRONT_FRONT );
		else if( true == bChangeStandUpRollingFrontBack )
			StateChange( CSI_STANDUP_ROLLING_FRONT_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_FRONT );
	}
	else if( 0.66f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if( true == m_InputData.oneZ )
		{
			bChangeStandUpAttackFront		= true;
			bChangeStandUpRollingFrontFront	= false;
			bChangeStandUpRollingFrontBack	= false;
			bChangeStandUpCannonAttackFront	= false;

		}
		else if ( true == m_InputData.oneX )
		{
			bChangeStandUpAttackFront		= false;
			bChangeStandUpRollingFrontFront	= false;
			bChangeStandUpRollingFrontBack	= false;
			bChangeStandUpCannonAttackFront	= true;
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
				bChangeStandUpCannonAttackFront	= false;

			}
			// ���� ȭ��ǥ Ű���带 ���� ���
			else if( true == m_InputData.pureLeft )
			{
				// �ڷ� �Ѹ�
				bChangeStandUpAttackFront		= false;
				bChangeStandUpRollingFrontFront	= false;
				bChangeStandUpRollingFrontBack	= true;
				bChangeStandUpCannonAttackFront	= false;

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
				bChangeStandUpCannonAttackFront	= false;

			}
			else if( true == m_InputData.pureLeft )
			{
				// ���� �Ѹ�
				bChangeStandUpAttackFront		= false;
				bChangeStandUpRollingFrontBack	= false;
				bChangeStandUpRollingFrontFront	= true;
				bChangeStandUpCannonAttackFront	= false;
			}
		}
	}

	CommonEventProcess();
}

/** @function : DamageDownBackFrameMove
	@brief : �ڿ��� ���� �޾� �Ѿ� �� ������ FrameMove
*/
void CX2GUChung::DamageDownBackFrameMove()
{
	if( true == m_pXSkinAnim->EventTimer( 0.32f ) )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}

/** @function : DamageDownBackEventProcess
	@brief : �ڿ��� ���� �޾� �Ѿ� �� ������ EventProcess
*/
void CX2GUChung::DamageDownBackEventProcess()
{
	bool& bChangeStandUpAttackBack			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingBackFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingBackBack		= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpCannonAttackBack	= m_FrameDataFuture.stateParam.bEventFlagList[4];

	// �Ѿ����µ� �ؿ� ���θ� ���� ���� ���
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS ;
	}
	// �Ѿ��� ��
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// Z Ű�� ������ ��
		if( true == bChangeStandUpAttackBack )
		{
			StateChange( CSI_STANDUP_ATTACK_BACK );
		}
		// X Ű�� ������ ��
		else if ( true == bChangeStandUpCannonAttackBack )
		{
			// ĳ���� ����� �� ������
			if ( CanUseCannon() )
				StateChange( CSI_STANDUP_CANNON_ATTACK_BACK );
			else
				StateChange( CSI_STANDUP_ATTACK_BACK );

		}
		else if( true == bChangeStandUpRollingBackFront )
			StateChange( CSI_STANDUP_ROLLING_BACK_FRONT );
		else if( true == bChangeStandUpRollingBackBack )
			StateChange( CSI_STANDUP_ROLLING_BACK_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_BACK );
	}
	else if( 0.66f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if( true == m_InputData.oneZ )
		{
			bChangeStandUpAttackBack		= true;
			bChangeStandUpRollingBackFront	= false;
			bChangeStandUpRollingBackBack	= false;
			bChangeStandUpCannonAttackBack	= false;
		}
		else if ( true == m_InputData.oneX )
		{
			bChangeStandUpAttackBack		= false;
			bChangeStandUpRollingBackFront	= false;
			bChangeStandUpRollingBackBack	= false;
			bChangeStandUpCannonAttackBack	= true;
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
				bChangeStandUpCannonAttackBack	= false;

			}
			// ���� ȭ��ǥ Ű���带 ���� ���
			else if( true == m_InputData.pureLeft )
			{
				// �ڷ� �Ѹ�
				bChangeStandUpAttackBack		= false;
				bChangeStandUpRollingBackFront	= false;
				bChangeStandUpRollingBackBack	= true;
				bChangeStandUpCannonAttackBack	= false;

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
				bChangeStandUpCannonAttackBack	= false;

			}
			else if( true == m_InputData.pureLeft )
			{
				// ���� �Ѹ�
				bChangeStandUpAttackBack		= false;
				bChangeStandUpRollingBackBack	= false;
				bChangeStandUpRollingBackFront	= true;
				bChangeStandUpCannonAttackBack	= false;
			}
		}
	}

	CommonEventProcess();
}

/** @function : DamageAirDownLandingEventProcess
	@brief : ���߿��� �ٿ� ���� �޾� ���� ���� ������ EventProcess
*/
void CX2GUChung::DamageAirDownLandingEventProcess()
{
	// �տ��� ���ݹ޾� �Ѿ����� ���� ���� EventProcess�� ���� ������ DamageDownFrontEventProcess�� ȣ�� �ϵ��� �Ͽ���
	DamageDownFrontEventProcess();
}

/** @function : StandUpRollingEventProcess
	@brief : �����鼭 �Ͼ�� ������ EventProcess
*/
void CX2GUChung::StandUpRollingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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

	CommonEventProcess();
}

/** @function : StandUpRollingEnd
	@brief : �����鼭 �Ͼ�� ������ End
*/
void CX2GUChung::StandUpRollingEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

/** @function : StandUpAttackEventProcess
	@brief : �Ͼ�鼭 ������ ������ EventProcess
*/
void CX2GUChung::StandUpAttackEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );

			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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
	}

	CommonEventProcess();
}

/** @function : StandUpAttackEnd
	@brief : �Ͼ�鼭 ������ ������ End
*/
void CX2GUChung::StandUpAttackEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

/** @function : ChargeCannonBallStart
	@brief : ChargeCannonBall�� Start ( m_pXSkinAnim �� ����� ���� )
*/
void CX2GUChung::ChargeCannonBallStart()
{
	CommonStateStart();
	ChangeAnimationChargeCannonBallNow();
}

/** @function : ChargeCannonBallStartFuture
	@brief : ChargeCannonBall�� StartFuture ( m_pXSkinAnimFuture �� ����� ���� )
*/
void CX2GUChung::ChargeCannonBallStartFuture()
{
	CommonStateStartFuture();
	ChangeAnimationChargeCannonBallFuture();
}

/** @function : ChargeCannonBallFrameMove
	@brief : ���� �ִϸ��̼� �ð��� ������ ĳ���� ����
*/
void CX2GUChung::ChargeCannonBallFrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.96f, 1 );

	CommonFrameMove();
}

/** @function : ChargeCannonBallEventProcess
	@brief : ĳ���� ���� ���¿��� � ���·� �Ѿ�� ���� ����
*/
void CX2GUChung::ChargeCannonBallEventProcess()
{
	if ( false == IsOnSomethingFuture() )	// ��� �ִ� ���� ������
	{
		// �ϰ�
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	} // if
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			StateChange( CSI_CDC_COMBO_X );
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN || GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			StateChange( CSI_CHG_COMBO_X );
		else
			StateChange( CSI_COMBO_X );
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		// ��� �� �� ������ ���, �׷��� ������ walk
		StateChangeDashIfPossible();
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( USI_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )	// ����
	else if( true == m_InputData.oneDown	// �Ʒ� ���� ȭ��ǥ�� ������, ���θ� �Ʒ��� ������ �� ������
			 &&	true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
						LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
						&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function : GuardStart
	@brief : Guard�� Start �Լ��� ������ �޴� ������ ������ ��
*/
void CX2GUChung::GuardStart()
{
	CommonStateStart();
}

/** @function : GuardEventProcess
	@brief : Guard�� EventProcess ( Wait���� �Ѿ�� ���� �Ǵ��� )
*/
void CX2GUChung::GuardEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if ( true == m_InputData.oneZ )
	{
		if( m_fWithStandingRate <= -1.f )
			StateChange( CSI_GUARD_ATTACK_Z );
		else if( m_fWithStandingRate >= GetRandomFloat(1) )
		{
			m_iSiegeShellingCount = 0;
			StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE );
		}
		else
		{
			StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT );
		}
	} // else if
	// X�� ��������
	else if ( true == m_InputData.oneX )
	{
		if( m_fWithStandingRate <= -1.f )
		{
			// ĳ������ ����� �� ������
			if ( CanUseCannon() )
				StateChange( CSI_GUARD_ATTACK_X );
			else
				StateChange( CSI_GUARD_ATTACK_Z );
		}
		else if( m_fWithStandingRate >= GetRandomFloat(1) )
		{
			m_iSiegeShellingCount = 0;
			StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE );
		}
		else
		{
			StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT );
		}
	} // if

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : GuardAttackZEventProcess
	@brief : GuardAttackZ �� EventProcess (Wait, JumpDown ������ ���� ����)
*/
void CX2GUChung::GuardAttackZEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.7f;

		if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
		SKILL_CANCEL_AFTER( 0.01f )
		else if ( CAN_DASH_CANCEL )
		{
			StateChangeDashIfPossible();
		}
		else if ( CAN_WALK_CANCEL )
		{
			StateChange( USI_WALK );
		}
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function : GuardAttackXFrameMove
	@brief : GuardAttackX�� FrameMove ( ĳ���� �Ҹ� )
*/
void CX2GUChung::GuardAttackXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.05f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
//{{ kimhc // 2011.1.14 // û 1�� ����
			// ���� �����͸� ����
			if ( GetRandomFloat() <= GetProbabilityBigDamageAtGuardMastery() )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_GUARD_ATTACK_X_BIG_DAMAGE", this );
			} // if
			else
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_GUARD_ATTACK_X", this );
			}
//}} kimhc // 2011.1.14 // û 1�� ����
			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}

/** @function : GuardAttackXInit
	@brief : GuardAttackX���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::GuardAttackXInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : GuardAttackXEventProcess
	@brief : GuardAttackX �� EventProcess (Wait, JumpDown ������ ���� ����)
*/
void CX2GUChung::GuardAttackXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.7f;

		if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
		SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
				StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function : ComboZEventProcess
	@brief : ComboZ�� EventProcess ( ComboZZ�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboZZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// ������ �ٲ����� �޺� ���
	if( CAN_WALK_CANCEL )
	{
		bCanChangeStateComboZZ = false;
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_Z = 0.42f;	// Combo_Z�� �б���

		IF_SKILL_CANCEL_AFTER( 0.01f )
		// �б� �ð� ����
		else if ( TRIGGER_TIME_COMBO_Z >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneZ )
				bCanChangeStateComboZZ = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else
		{
			const float TIGGER_TIME_COMBO_Z_LIMIT = 0.4f;	// �Է� �ð� �Ѱ� ��

			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboZZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_ZZ );
			}
			// �б� ���Ŀ��� �Ѱ�ð� ���� ZŰ�� ��������
			else if ( true == m_InputData.oneZ &&
					  TIGGER_TIME_COMBO_Z_LIMIT > m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_ZZ );
			}

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : ComboZZEventProcess
	@brief : ComboZZ�� EventProcess ( ComboZZZ, ComboZZdownZ�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboZZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboZZZ		= m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanChangeStateComboZZdownZ	= m_FrameDataFuture.stateParam.bEventFlagList[1];

//{{ kimhc // 2011.1.4 // û 1�� ����
	bool &bCanChangeStateCGFComboZZX	= m_FrameDataFuture.stateParam.bEventFlagList[3];	// ǻ�� ����� ����
//}} kimhc // 2011.1.4 // û 1�� ����

	bool &bCanChangeStateCDCComboZZX	= m_FrameDataFuture.stateParam.bEventFlagList[4];

	bool &bCanChangeStateCTTComboZZX	= m_FrameDataFuture.stateParam.bEventFlagList[5];

	// ������ �ٲ����� �޺� ���
	if( CAN_WALK_CANCEL )
	{
		bCanChangeStateComboZZZ			= false;
		bCanChangeStateComboZZdownZ		= false;
		//{{ kimhc // 2011.1.4 // û 1�� ����
		bCanChangeStateCGFComboZZX		= false;
		//}} kimhc // 2011.1.4 // û 1�� ����

		bCanChangeStateCDCComboZZX		= false;

		bCanChangeStateCTTComboZZX		= false;
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_ZZ = 0.45f;	// Combo_ZZ�� �б���

		const float CTT_TRIGGER_TIME_COMBO_ZZ = 0.83f;	// ��Ƽ�� Ʈ���� Combo_ZZ�� �б���

		if ( CTT_TRIGGER_TIME_COMBO_ZZ >= m_pXSkinAnimFuture->GetNowAnimationTime() &&
				true == m_InputData.oneX && GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
		{
			bCanChangeStateCTTComboZZX		= true;
			bCanChangeStateCDCComboZZX		= false;
			bCanChangeStateCGFComboZZX		= false;
			bCanChangeStateComboZZdownZ		= false;
			bCanChangeStateComboZZZ			= false;
		}

		else if ( TRIGGER_TIME_COMBO_ZZ >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// Z Ű�� �����µ�
			if ( true == m_InputData.oneZ )
			{
				// �Ʒ��� ȭ��ǥ�� ���� �����ٸ�
				if ( true == m_InputData.pureDown )
				{
					bCanChangeStateComboZZdownZ		= true;
					bCanChangeStateComboZZZ			= false;
				}
				else
				{
					bCanChangeStateComboZZdownZ		= false;
					bCanChangeStateComboZZZ			= true;
				}

			//{{ kimhc // 2011.1.4 // û 1�� ����
				bCanChangeStateCGFComboZZX			= false;	// ZŰ�� �������ϱ� XŰ �޺��� false
			//}} kimhc // 2011.1.4 // û 1�� ����

			} // if

			//{{ kimhc // 2011.1.4 // û 1�� ����
			else if ( true == m_InputData.oneX &&
				( GetUnitClass() == CX2Unit::UC_CHUNG_FURY_GUARDIAN ||
				GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN) )
			{
				bCanChangeStateCGFComboZZX			= true;
				bCanChangeStateComboZZdownZ			= false;
				bCanChangeStateComboZZZ				= false;
				bCanChangeStateCDCComboZZX			= false;
				bCanChangeStateCTTComboZZX			= false;
			} // else if
			//}} kimhc // 2011.1.4 // û 1�� ����
			else if ( true == m_InputData.oneX && GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			{
				bCanChangeStateCDCComboZZX		= true;
				bCanChangeStateCGFComboZZX		= false;
				bCanChangeStateComboZZdownZ		= false;
				bCanChangeStateComboZZZ			= false;
				bCanChangeStateCTTComboZZX		= false;
			}
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else
		{
			// ������ �� �� ������ ���� ���� �Ǵ� �ϰ� ��
			// ���� ��� Z��ư�� ������ �ͺ��� downZ�� ������ ���� �� ������

			// CSI_COMBO_ZZdownZ �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboZZdownZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_ZZdownZ );
			}
			// CSI_COMBO_ZZZ �޺� Ű�� ���Ⱦ�����
			else if ( true == bCanChangeStateComboZZZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_ZZZ );
			}
		//{{ kimhc // 2011.1.4 // û 1�� ����
			else if ( true == bCanChangeStateCGFComboZZX )
			{
				StateChange( CSI_CFG_COMBO_ZZX );
			} // else if
		//}} kimhc // 2011.1.4 // û 1�� ����
			else if ( true == bCanChangeStateCDCComboZZX )
			{
				StateChange( CSI_CDC_COMBO_ZZX );
			}

			else if ( true == bCanChangeStateCTTComboZZX )
			{
				StateChange( CSI_CTT_COMBO_ZZX );
			}
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : ComboZZZEventProcess
	@brief : ComboZZZ�� EventProcess ( ComboZZZZ, ComboZZZZ�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboZZZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboZZZZ			= m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanChangeStateComboZZZX			= m_FrameDataFuture.stateParam.bEventFlagList[1];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ְ� ��õ� ���� �ʾ�����
	else
	{
		const float LOWER_BOUND_INPUT_TIME_COMBO = 0.4f;	// �� �ð� ���ĺ��� �޺� �Է� ����
		const float UPPER_BOUNT_INPUT_TIME_COMBO = 0.69f;	// �� �ð� ���� �޺� �Է� ����
		// �޺��Է��� ������ ����
		if ( LOWER_BOUND_INPUT_TIME_COMBO <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
			 UPPER_BOUNT_INPUT_TIME_COMBO >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// Z Ű�� �������� ZZZZ �޺���
			if ( true == m_InputData.oneZ )
			{
				bCanChangeStateComboZZZZ = true;
				bCanChangeStateComboZZZX = false;
			}
			// X Ű�� �������� ZZZX �޺���
			else if ( true == m_InputData.oneX )
			{
				bCanChangeStateComboZZZZ = false;
				bCanChangeStateComboZZZX = true;
			}
		}

		const float TRIGGER_TIME_COMBO_ZZZ	= 0.69f;	// Combo_ZZZ�� �б���
		if( TRIGGER_TIME_COMBO_ZZZ < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// ZZZZ �޺�
			if ( true == bCanChangeStateComboZZZZ )
			{
				StateChange( CSI_COMBO_ZZZZ );	// ���� �޺��� ���� ����
			}
			// ZZZX �޺� ����
			else if ( true == bCanChangeStateComboZZZX )
			{
				StateChange( CSI_COMBO_ZZZX_READY );
			}
		}

		const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.5f;		// �� �ð� ���ĺ��� ĵ�� �Է� ����
		//const float UPPER_BOUND_INPUT_TIME_CANCEL = 1.066f;	// �� �ð� ���� ĵ�� �Է� ����

		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : ComboZZZZEventProcess
	@brief : ComboZZZZ�� EventProcess ( ��� ĵ�� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboZZZZEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		const float TRIGGER_TIME_COMBO_ZZZZ = 0.607f;
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 1.033f;

		if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
		// �б� �� ���Ŀ� ��ų�� ����ϸ�
		SKILL_CANCEL_AFTER( TRIGGER_TIME_COMBO_ZZZZ )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function : ComboZZZXEventProcess
	@brief : ComboZZZX�� EventProcess
*/
void CX2GUChung::ComboZZZXReadyEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// ����ȭ��尡 �ƴϸ� ĳ���� ������ ����
		if ( 1 >= GetCannonBallCount() && false == IsFullHyperMode() )
		{
			StateChange( CSI_COMBO_ZZZX_FINISH );
		}
		else
		{
			StateChange( CSI_COMBO_ZZZX_CANNON_BALL_SHOT );
		}
	}

	CommonEventProcess();
}

/** @function : ComboZZZXCannonBallShotInit
	@brief : ComboZZZXCannonBallShot�� �ʿ��� ���ҽ� �ε�
*/

void CX2GUChung::ComboZZZXCannonBallShotInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : ComboZZZXCannonBallShotStartFuture
	@brief : ComboZZZXCannonBallShot�� StartFuture ( ��� ������ ���� ���� )
*/
void CX2GUChung::ComboZZZXCannonBallShotStartFuture()
{
	CommonStateStartFuture();

	m_bApplyMotionOffset = false;
}
/** @function : ComboZZZXCannonBallShotFrameMove
	@brief : ComboZZZXCannonBallShot�� FrameMove ( ĳ���� �Һ� )
*/
void CX2GUChung::ComboZZZXCannonBallShotFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ 1�� �̻� �ְų� ����ȭ ����
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_COMBO_ZZZX_CANNON_BALL_SHOT", this );
				UseCannon( 1 );
				ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwa" );
			}
		}
	}


	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}

	CommonFrameMove();
}

/** @function : ComboZZZXCannonBallShotEventProcess
	@brief : ComboZZZXCannonBallShot�� EventProcess ( FinishState�� �������, ���� ������Ʈ�� �ѹ� �� ������ ���� ������ )
*/
void CX2GUChung::ComboZZZXCannonBallShotEventProcess()
{
	// ��� ĵ������ 3�� �̻��� ĳ������ �Һ��ؾ����� �����ϴ�.
	const int TRIGGER_CANNON_BALL_COUNT = 3;
	bool &bCanDashCancel = m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanWalkCancel = m_FrameDataFuture.stateParam.bEventFlagList[1];

	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( TRIGGER_CANNON_BALL_COUNT <= m_CannonBallSystem.GetUsedCannonballCountAtThisState() )
	{
		if ( CAN_DASH_CANCEL )
		{
			bCanDashCancel = true;
			bCanWalkCancel = false;
		}
		else if ( CAN_WALK_CANCEL )
		{
			bCanDashCancel = false;
			bCanWalkCancel = true;
		}
	}

	if ( true == bCanDashCancel )
	{
		StateChangeDashIfPossible();
	}
	else if ( true == bCanWalkCancel )
	{
		StateChange( USI_WALK );
	}
	// �ִϸ��̼��� ������
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� ����������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			 ( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
			StateChange( CSI_COMBO_ZZZX_FINISH );
	}

	CommonEventProcess();
}

/** @function : ComboZZZXFinish
	@brief : ComboZZZXFinish�� �ʿ��� ���ҽ� �ε�
*/

void CX2GUChung::ComboZZZXFinishInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : ComboZZZXFinishStartFuture
	@brief : ComboZZZXFinish�� StartFuture ( ��� ������ ���� ���� )
*/
void CX2GUChung::ComboZZZXFinishStartFuture()
{
	CommonStateStartFuture();

	m_bApplyMotionOffset = false;
}

/** @function : ComboZZZXFinishFrameMove
	@brief : ComboZZZXFinish�� FrameMove ( ����Ʈ ��� �� CannonBall Count �Ҹ� )
*/
void CX2GUChung::ComboZZZXFinishFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_COMBO_ZZZX_FINISH", this );
			UseCannon( 1 );
			ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwang" );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}
	}

	CommonFrameMove();
}

/** @function : ComboZZZXFinishEventProcess
	@brief : ComboZZZXFinish�� EventProcess ( Wait���·� ���� )
*/
void CX2GUChung::ComboZZZXFinishEventProcess()
{
	const float TRIGGER_TIME_CANCEL = 0.2f;

	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ��� �Ǵ� ��ũ ĵ�� ��� ����
	SKILL_CANCEL_AFTER(TRIGGER_TIME_CANCEL )
	WALK_CANCEL_AFTER( TRIGGER_TIME_CANCEL )
	DASH_CANCEL_AFTER( TRIGGER_TIME_CANCEL )
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}



/** @function : ComboZZdownZEventProcess
	@brief : ComboZZdownZ�� EventProcess ( ComboZZdownZZ�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboZZdownZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboZZdownZZ		= m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ְ� ��� ĵ�� ��� ��������
	else
	{
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.766f; // �� �ð� ���ĺ��� ĵ�� �Է� ����
		const float LOWER_BOUND_INPUT_TIME_COMBO = 0.566f;

		const float TRIGGER_TIME_COMBO_ZZZdownZ = 1.1f;	 // Combo_ZZZdownZ�� �б���

		if ( LOWER_BOUND_INPUT_TIME_COMBO <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
			 TRIGGER_TIME_COMBO_ZZZdownZ > m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// Z Ű�� ��������
			if ( true == m_InputData.oneZ )
			{
				bCanChangeStateComboZZdownZZ = true;
			}
		}
		// �б��� ���Ŀ�
		else if ( TRIGGER_TIME_COMBO_ZZZdownZ <= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// Z Ű�� ����������
			if ( true == bCanChangeStateComboZZdownZZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_ZZdownZZ );
			}
		}

		if ( LOWER_BOUND_INPUT_TIME_CANCEL <= m_pXSkinAnimFuture->GetNowAnimationTime() )

		{
			if ( CAN_DASH_CANCEL )
			{
				StateChangeDashIfPossible();
			}
			else if ( CAN_WALK_CANCEL )
			{
				StateChange( USI_WALK );
			}
		}
	}

	// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}

	CommonEventProcess();
}

/** @function : ComboZZdownZZEventProcess
	@brief : ComboZZdownZZ�� EventProcess ( ĵ�� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboZZdownZZEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		const float TRIGGER_TIME_COMBO_ZZdownZZ = 0.766f;
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 1.233f;

		if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
		SKILL_CANCEL_AFTER( TRIGGER_TIME_COMBO_ZZdownZZ )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function : ComboXEventProcess
	@brief : ComboX�� EventProcess ( ComboXX�� ���º��� ���θ� �Ǵ��ϰ� ������ )
	@brief : û 1�� ���� ���� ������� �޺� �б��� ����
*/
void CX2GUChung::ComboXEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// ������ �ٲ����� �޺� ���
	if( CAN_WALK_CANCEL )
	{
		bCanChangeStateComboXX = false;
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_X = 0.6f;	// Combo_X�� �б���

		// �б� �ð� ����
		if ( TRIGGER_TIME_COMBO_X >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneX )
				bCanChangeStateComboXX = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXX )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_XX );
			}

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// û 1�� ���� ���� ������� ���� �޺�
		// ���� �϶� �ߵ��ϱ� ������ ��� ������ Unit Class �˻��� �ڵ����
		if( GetUnitClass() == CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN ||
			GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
		{
			const float TRIGGER_TIME_CSG_COMBO_XX = 0.54f; // ���� �޺� �б� �ð�
			bool &bCanChangeStateCSGComboXX = m_FrameDataFuture.stateParam.bEventFlagList[1]; // �бⰡ ������ġ üũ

			// �б�ð� ���̰� ���� Ű�� �������� �ʾҴٸ�.
			if( false == bCanChangeStateCSGComboXX && TRIGGER_TIME_CSG_COMBO_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				//{{ JHKang / ������ / 2011.6.14 / ���� �޺� Ű ����(X>X -> X>Z)
				if ( true == m_InputData.oneZ )
					//}}
				{
					bCanChangeStateCSGComboXX = true;
				}
			}
			// �б�ð� ����.
			else if( TRIGGER_TIME_CSG_COMBO_XX <= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				if( true == bCanChangeStateCSGComboXX )
				{
					// ���� �޺��� ���º���
					StateChange(CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON);
				}
			}
			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : ComboXXInit
	@brief : ���ҽ� �ε�
*/

void CX2GUChung::ComboXXInit()
{
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust02.X" );
	TextureReadyInBackground( L"GroundShockWave.dds" );

}



/** @function : ComboXEventProcess
	@brief : ComboX�� EventProcess ( ComboXX�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboXXEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboXXX	= m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanChangeStateComboXXZ	= m_FrameDataFuture.stateParam.bEventFlagList[1];

	// ������ �ٲ����� �޺� ���
	if( CAN_WALK_CANCEL )
	{
		bCanChangeStateComboXXX		= false;
		bCanChangeStateComboXXZ		= false;
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float LOWER_BOUND_INPUT_TIME	= 0.4f;	// �� �ð� ������ �Է��� �ȵ�
		const float TRIGGER_TIME_COMBO_XX	= 0.8f;	// Combo_XX�� �б���

		// �б� �ð� ����
		if ( ( LOWER_BOUND_INPUT_TIME <= m_pXSkinAnimFuture->GetNowAnimationTime() ) &&
			 ( TRIGGER_TIME_COMBO_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() ) )
		{
			// Z Ű�� �����ٸ�
			if ( true == m_InputData.oneZ )
			{
				bCanChangeStateComboXXZ		= true;
				bCanChangeStateComboXXX		= false;
			}
			// X Ű�� �����ٸ�
			else if ( true == m_InputData.oneX )
			{
				bCanChangeStateComboXXZ		= false;
				bCanChangeStateComboXXX		= true;
			}
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else if ( TRIGGER_TIME_COMBO_XX < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// CSI_COMBO_XXZ �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXXZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_XXZ );
			}
			// CSI_COMBO_XXX �޺� Ű�� ���Ⱦ�����
			else if ( true == bCanChangeStateComboXXX )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_XXX );
			}
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : ComboXXXInit
@brief : ComboXXX���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::ComboXXXInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : ComboXXFrameMove
	@brief : ComboXX�� FrameMove ( CannonBall�� �Ҹ� )
*/
void CX2GUChung::ComboXXXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.56f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			// ����Ʈ ���
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_COMBO_XXX", this );
			UseCannon( 1 );
			ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwang" );
		}
	}

	CommonFrameMove();
}

/** @function : ComboXXXEventProcess
	@brief : ComboXXX�� EventProcess ( ComboXX�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboXXXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		float LOWER_BOUND_INPUT_TIME_SKILL_CANCEL = 0.9f;
		float LOWER_BOUND_INPUT_TIME_CANCEL = 1.533f;

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_SKILL_CANCEL )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function : ComboXXZEventProcess
	@brief : ComboXXZ�� EventProcess ( ComboXXZX�� ���º��� ���θ� �Ǵ��ϰ� ������ )
*/
void CX2GUChung::ComboXXZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboXXZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float LOWER_BOUND_INPUT_TIME	= 0.433f;		// �� �ð� ���� �Է��� ���� ����
		const float TRIGGER_TIME_COMBO_XXZ	= 1.066f;		// Combo_XXZ�� �б���

		// �б� �ð� ����
		if ( ( LOWER_BOUND_INPUT_TIME <= m_pXSkinAnimFuture->GetNowAnimationTime() ) &&
			 ( TRIGGER_TIME_COMBO_XXZ >= m_pXSkinAnimFuture->GetNowAnimationTime() ) )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneX )
				bCanChangeStateComboXXZ = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else if ( TRIGGER_TIME_COMBO_XXZ < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			if ( CAN_DASH_CANCEL )
			{
				StateChangeDashIfPossible();
			}
			else if ( CAN_WALK_CANCEL )
			{
				StateChange( USI_WALK );
			}
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXXZ )
			{
				// ����ȭ��尡 �ƴϸ� ĳ���� ������ ����
				if ( 1 >= GetCannonBallCount() && false == IsFullHyperMode() )
				{
					StateChange( CSI_COMBO_XXZX_FINISH );
				}
				else
				{
					StateChange( CSI_COMBO_XXZX_CANNON_BALL_SHOT );
				}
			}

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : ComboXXZXCannonBallShotFrameMove
	@brief : ComboXXZXCannonBallShot�� FrameMove ( ����Ʈ ��� �� ĳ���� �Ҹ� )
*/
void CX2GUChung::ComboXXZXCannonBallShotFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ �Ѱ� �̻� �ְų� ����ȭ ����
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_COMBO_XXZX_CANNON_BALL_SHOT", this );
				UseCannon( 1 );
				ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwa" );
			}
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}

	CommonFrameMove();
}

/** @function : ComboXXZXCannonBallShotInit
	@brief : ComboXXZXCannonBallShot���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::ComboXXZXCannonBallShotInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : ComboXXZXCannonBallShotEventProcess
	@brief : ComboXXZXCannonBallShot�� EventProcess ( ĵ�� ����Ʈ �� Finish�� �̵� )
*/
void CX2GUChung::ComboXXZXCannonBallShotEventProcess()
{
	// ��� ĵ������ 3�� �̻��� ĳ������ �Һ��ؾ����� �����ϴ�.
	const int TRIGGER_CANNON_BALL_COUNT = 3;
	bool &bCanDashCancel = m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanWalkCancel = m_FrameDataFuture.stateParam.bEventFlagList[1];

	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( TRIGGER_CANNON_BALL_COUNT <= m_CannonBallSystem.GetUsedCannonballCountAtThisState() )
	{
		if ( CAN_DASH_CANCEL )
		{
			bCanDashCancel = true;
			bCanWalkCancel = false;
		}
		else if ( CAN_WALK_CANCEL )
		{
			bCanDashCancel = false;
			bCanWalkCancel = true;
		}
	}

	if ( true == bCanDashCancel )
	{
		StateChangeDashIfPossible();
	}
	else if ( true == bCanWalkCancel )
	{
		StateChange( USI_WALK );
	}
	// �ִϸ��̼��� ������
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� ����������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
			StateChange( CSI_COMBO_XXZX_FINISH );
	}

	CommonEventProcess();
}

/** @function : ComboXXZXFinishInit
	@brief : ComboXXZXFinish���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::ComboXXZXFinishInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : ComboXXZXFinishFrameMove
	@brief : ComboXXZXFinish�� FrameMove ( ����Ʈ ��� �� ĳ���� �Ҹ� )
*/
void CX2GUChung::ComboXXZXFinishFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_COMBO_XXZX_FINISH", this );
			UseCannon( 1 );
			ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwang" );
		}
	}

	CommonFrameMove();
}

/** @function : ComboXXZXFinishEventProcess
	@brief : ComboXXZXFinish�� EventProcess ( ĵ�� ����Ʈ �� Wait���� �̵� )
*/
void CX2GUChung::ComboXXZXFinishEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.3f;

		if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
		// ��� �Ǵ� ��ũ ĵ�� ��� ����
		SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	}

	CommonEventProcess();
}

/** @function : ComboDashZEventProcess
	@brief : ComboDashZ�� EventProcess ( ĵ�� ����Ʈ �߰� )
*/
void CX2GUChung::ComboDashZEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.5f;

	IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}

	// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : AttackDashXInit
	@brief : AttackDashX���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::AttackDashXInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : AttackDashXFrameMove
	@brief : AttackDashX�� FrameMove ( CannonBall �Ҹ� )
*/
void CX2GUChung::AttackDashXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			// ����Ʈ ���
			switch ( GetUnitClass() )
			{
			case CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN:
			case CX2Unit::UC_CHUNG_DEADLY_CHASER:
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CSG_ATTACK_DASH_X", this );
				break;
				// oasis907 : ����� [2011.6.15] û 2�� ���� - ���̾� �ȶ��
			case CX2Unit::UC_CHUNG_IRON_PALADIN:
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_ATTACK_DASH_X", this );
				break;
			case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_ATTACK_DASH_X", this );
				break;
			default:
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_ATTACK_DASH_X", this );
				break;
			} // switch

			UseCannon( 1 );
			ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwang" );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}
	}

	CommonFrameMove();
}


/** @function : AttackDashXEventProcess
	@brief : AttackDashXLanding �� EventProcess
*/
void CX2GUChung::AttackDashXEventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	else
	{
		// û 1�� ���� ���� ������� ���� �޺�
		// ���� �϶� �ߵ��ϱ� ������ ��� ������ Unit Class �˻��� �ڵ����
		if( GetUnitClass() == CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN ||
			GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
		{
			const float TRIGGER_TIME_CSG_COMBO_DASH_XX = 0.48f; // ���� �޺� �б� �ð�
			bool &bCanChangeStateCSGComboDashXX = m_FrameDataFuture.stateParam.bEventFlagList[0]; // �бⰡ ������ġ üũ

			// �б�ð� ���̰� ���� Ű�� �������� �ʾҴٸ�.
			if( false == bCanChangeStateCSGComboDashXX && TRIGGER_TIME_CSG_COMBO_DASH_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				// X Ű�� �����ٸ�
				if( true == m_InputData.oneX )
					bCanChangeStateCSGComboDashXX = true;
			}
			// �б�ð� ����.
			else if( TRIGGER_TIME_CSG_COMBO_DASH_XX <= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				if( true == bCanChangeStateCSGComboDashXX )
				{
					// ���� �޺��� ���º���
					StateChange(CSI_CSG_COMBO_DASH_XX);
				}
			}
			DASH_CANCEL_AFTER( m_fDashCancelAfter )
			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}
		// oasis907 : ����� [2011.6.15] û 2�� ���� - ���̾� �ȶ��
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN )
		{
			const float TRIGGER_TIME_CIP_COMBO_DASH_XX = 0.53333f; // ���� �޺� �б� �ð�
			bool &bCanChangeStateCIPComboDashXX = m_FrameDataFuture.stateParam.bEventFlagList[0]; // �бⰡ ������ġ üũ

			// �б�ð� ���̰� ���� Ű�� �������� �ʾҴٸ�.
			if( false == bCanChangeStateCIPComboDashXX && TRIGGER_TIME_CIP_COMBO_DASH_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				// Z Ű�� �����ٸ�
				if( true == m_InputData.oneZ )
					bCanChangeStateCIPComboDashXX = true;
			}
			// �б�ð� ����.
			else if( TRIGGER_TIME_CIP_COMBO_DASH_XX <= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				if( true == bCanChangeStateCIPComboDashXX )
				{
					// ���� �޺��� ���º���
					StateChange(CSI_CIP_COMBO_DASH_XX);
				}
			}
			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����

		}
		else if( GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
		{
			const float TRIGGER_TIME_CTT_COMBO_DASH_XX = 0.53333f; // ���� �޺� �б� �ð�
			bool &bCanChangeStateCTTComboDashXX = m_FrameDataFuture.stateParam.bEventFlagList[0]; // �бⰡ ������ġ üũ

			// �б�ð� ���̰� ���� Ű�� �������� �ʾҴٸ�.
			if( false == bCanChangeStateCTTComboDashXX && TRIGGER_TIME_CTT_COMBO_DASH_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				// X Ű�� �����ٸ�
				if( true == m_InputData.oneX )
					bCanChangeStateCTTComboDashXX = true;
			}
			// �б�ð� ����.
			else if( TRIGGER_TIME_CTT_COMBO_DASH_XX <= m_pXSkinAnimFuture->GetNowAnimationTime() )
			{
				if( true == bCanChangeStateCTTComboDashXX )
				{
					// ���� �޺��� ���º���
					StateChange(CSI_CTT_DASH_COMBO_XX);
				}
			}
			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����

		}
	}

	CommonEventProcess();
}

/** @function : AttackDashXEndFuture
	@brief : AttackDashX �� EndFuture (������ ȸ�� �����Ϳ� IsRight ���� ����)
*/
void CX2GUChung::AttackDashXEndFuture()
{
	CommonStateEndFuture();

	// ������ �ݴ��� �˷���
	m_FrameDataFuture.syncData.bIsRight				= !m_FrameDataFuture.syncData.bIsRight;

	// ȸ���ߴٰ� �˷���
	m_FrameDataFuture.unitCondition.dirDegree.y		+= 180.0f;
}

/** @function : AttackJumpZEventProcess
	@brief : AttackJumpZ �� EventProcess
*/
void CX2GUChung::AttackJumpZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		bool &bPureDown = m_FrameDataFuture.stateParam.bEventFlagList[0];

		// �ٿ� Ű�� ��������
		if ( true == bPureDown )
		{
			m_fCanNotJumpTime = 0.2f;
		}

		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}

/** @function : AttackJumpZFrameMoveFuture
	@brief : AttackJumpZ �� FrameMoveFuture (����Ű�� ���� ���ǵ� ����)
*/
void CX2GUChung::AttackJumpZFrameMoveFuture()
{
	// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
	if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
		( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( true == m_InputData.pureDown && false == m_InputData.pureUp )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonFrameMoveFuture();
}

/** @function : AttackJumpXEventProcess
	@brief : AttackJumpX �� EventProcess
*/
void CX2GUChung::AttackJumpXEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		bool &bPureDown = m_FrameDataFuture.stateParam.bEventFlagList[0];
		if ( true == bPureDown )
		{
			m_fCanNotJumpTime = 0.2f;
		}

		StateChange( USI_JUMP_LANDING );
	}

	CommonEventProcess();
}

/** @function : AttackJumpXFrameMoveFuture
	@brief : AttackJumpX �� FrameMoveFuture (����Ű�� ���� ���ǵ� ����)
*/
void CX2GUChung::AttackJumpXFrameMoveFuture()
{
	// ���� ���� ���� ������ Ű���带 ������ ������ �� �� ���ư�
	if( ( true == m_InputData.pureRight && true == m_FrameDataFuture.syncData.bIsRight ) ||
		( true == m_InputData.pureLeft && false == m_FrameDataFuture.syncData.bIsRight ) )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( true == m_InputData.pureDown && false == m_InputData.pureUp )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	CommonFrameMoveFuture();
}

/** @function : ComboDashJumpZEventProcess
	@brief : ComboDashJumpZ �� EventProcess (ComboDashJumpZZ�� ���°� ���� �� �� �ִ��� �Ǵ�)
*/
void CX2GUChung::ComboDashJumpZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboDashJumpZZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

//{{ kimhc // 2011.1.4 // û 1�� ����
	bool &bCanChangeStateComboDashJumpZX = m_FrameDataFuture.stateParam.bEventFlagList[1];
//}} kimhc // 2011.1.4 // û 1�� ����

	// �߹ؿ� ���θ��� ������
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	// ���߿� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_DASH_JUMP_ZZ = 0.3f;	// Combo_Jump_ZZ�� �б���

		// �޺� Ű�� ������, �����ٴ� �����͸� ����

		//{{ kimhc // 2011.1.4 // û 1�� ����
		if ( true == m_InputData.oneZ )
		{
			bCanChangeStateComboDashJumpZZ = true;
			bCanChangeStateComboDashJumpZX = false;
		}
		else if ( true == m_InputData.oneX &&
			( GetUnitClass() == CX2Unit::UC_CHUNG_FURY_GUARDIAN ||
			GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN) )
		{
			bCanChangeStateComboDashJumpZZ = false;
			bCanChangeStateComboDashJumpZX = true;
		}
		else if( m_InputData.oneX == true &&
				( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN ||
				GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER ) )
		{
			bCanChangeStateComboDashJumpZZ = false;
			bCanChangeStateComboDashJumpZX = true;
		}

		//}} kimhc // 2011.1.4 // û 1�� ����

		// �б� �ð���, �Ǵ� �� �Ŀ�
		if ( TRIGGER_TIME_COMBO_DASH_JUMP_ZZ < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboDashJumpZZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_DASH_JUMP_ZZ );
			}

			if( bCanChangeStateComboDashJumpZX == true && GetUnitClass() != CX2Unit::UC_CHUNG_SHELLING_GUARDIAN && GetUnitClass() != CX2Unit::UC_CHUNG_TACTICAL_TROOPER )
			{
				StateChange( CSI_CFG_COMBO_DASH_JUMP_ZX );
			}
		}
		else if( ( GetUnitClass() == CX2Unit::UC_CHUNG_SHELLING_GUARDIAN ||
				GetUnitClass() == CX2Unit::UC_CHUNG_TACTICAL_TROOPER ) &&
				bCanChangeStateComboDashJumpZX == true &&
				m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.235f )
		{
			StateChange( CSI_CHG_DASH_JUMP_COMBO_ZX );
		}
	}

	CommonEventProcess();
}

/** @function : ComboDashJumpZZEventProcess
	@brief : ComboDashJumpZZ �� EventProcess (DashJumpZZLanding ������Ʈ�� ����)
*/
void CX2GUChung::ComboDashJumpZZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			StateChange( CSI_COMBO_DASH_JUMP_ZZLanding_CANNON_BALL_SHOT );
		}
		else
		{
			StateChange( CSI_COMBO_DASH_JUMP_ZZLanding );
		}
	}

	CommonEventProcess();
}

/** @function : ComboDashJumpZZLandingEventProcess
	@brief : ComboDashJumpZZLanding�� EventProcess ( Landing �� ������Ʈ ���� )
*/
void CX2GUChung::ComboDashJumpZZLandingEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}

	CommonEventProcess();
}

/** @function : ComboDashJumpZZLandingCannonBallShotInit
	@brief : ComboDashJumpZZLandingCannonBallShot���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::ComboDashJumpZZLandingCannonBallShotInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust02.X" );
	XMeshReadyInBackground( L"VANGUARD_MagicAttack_Explosion.Y");
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}



/** @function : ComboDashJumpZZLandingCannonBallShotEventProcess
	@brief : ComboDashJumpZZLandingCannonBallShot �� EventProcess (Landing �� ������Ʈ ����)
*/
void CX2GUChung::ComboDashJumpZZLandingCannonBallShotEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME = 0.566f;

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER ( LOWER_BOUND_INPUT_TIME )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}

	CommonEventProcess();
}

/** @function : ComboXXZXCannonBallShotInit
	@brief : ComboXXZXCannonBallShot���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::AttackDashJumpXInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function : AttackDashJumpXStartFuture
	@brief : AttackDashJumpX�� StartFuture ( ĳ������ ���� ���ο� ���� speed�� �ٸ��� ���� )
*/
void CX2GUChung::AttackDashJumpXStartFuture()
{
	CommonStateStartFuture();

	// ĳ���� ����� �� ������
	if ( false == CanUseCannon() )
	{
		// Speed_Time�� 1/4���δ�.
		const float REF_ONE_FORTH = 0.25f;
		for( TIME_SPEED& Factor : m_vecSpeedFactor )
		{
			Factor.vSpeed.x *= REF_ONE_FORTH;
			Factor.vSpeed.y *= REF_ONE_FORTH;
		}
	}
}

/** @function : AttackDashJumpXFrameMove
	@brief : AttackDashJumpX�� FrameMove ( CannonBall ��� )
*/
void CX2GUChung::AttackDashJumpXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.13f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			// ����Ʈ ���
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_ATTACK_DASH_JUMP_X", this );
			UseCannon( 1 );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}
	}

	CommonFrameMove();
}

/** @function : AttackDashJumpXFrameMoveFuture
	@brief : AttackDashJumpX�� FrameMoveFuture ( �¿� �̵� �� �����ϵ��� �� )
*/
void CX2GUChung::AttackDashJumpXFrameMoveFuture()
{
	JumpFrameMoveFuture();
}

/** @function : AttackDashJumpXEventProcess
	@brief : AttackDashJumpX �� EventProcess (Landing ������Ʈ�� ����)
*/
void CX2GUChung::AttackDashJumpXEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME	= 0.58f;	// �� �ð� ���ĺ��� �Է� ����

	if( GetUnitClass() == CX2Unit::UC_CHUNG_SHOOTING_GUARDIAN ||
		GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
	{
		if ( 0.36 <= m_pXSkinAnimFuture->GetNowAnimationTime() && true == m_InputData.oneX )
		{
			StateChange( CSI_CSG_COMBO_DASH_JUMP_X );
		}
	}

	if( true == IsOnSomethingFuture() )
	{
		// �Ʒ��� ���� ȭ��ǥ�� ���� ���
		if( true == m_bDownForce )
		{
			StateChange( CSI_POWER_DOWN_LANDING );
			m_bDownForce = false;

			m_pComboGuide->ShowComboCommand(L"A", L"AXD");
		}
		else
		{
			StateChange( USI_DASH_JUMP_LANDING );
			m_bDownForce = false;
		}
	}
	else if ( LOWER_BOUND_INPUT_TIME < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
		{
		}
		else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
			( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
		{
			m_bDisableGravity = true;
			StateChange( CSI_WALL_LANDING );
		}
		else if ( true == m_InputData.oneZ )
		{
			StateChange( CSI_ATTACK_JUMP_Z );
			m_pComboGuide->ShowComboCommand( L"J", L"JZ" );
		}
		else if ( true == m_InputData.oneX )
		{
			switch ( GetUnitClass() )
			{
			case CX2Unit::UC_CHUNG_IRON_CANNON:
			case CX2Unit::UC_CHUNG_FURY_GUARDIAN:
			case CX2Unit::UC_CHUNG_IRON_PALADIN:
			case CX2Unit::UC_CHUNG_SHELLING_GUARDIAN:
			case CX2Unit::UC_CHUNG_TACTICAL_TROOPER:
				StateChange( CSI_ATTACK_JUMP_X );
				break;

			default:
				break;
			} // switch
			m_pComboGuide->ShowComboCommand( L"J", L"JX" );
		}
	}

	if ( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}

/** @function : ComboDashJumpZZLandingCannonBallShotInit
	@brief : ComboDashJumpZZLandingCannonBallShot���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::StandUpCannonAttackInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XMeshReadyInBackground( L"VANGUARD_MagicAttack_Explosion.Y");
	XMeshReadyInBackground( L"Up_Impact01.Y");
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
}

/** @function : StandUpCannonAttackFrameMove
	@brief : StandUpCannonAttack�� FrameMove ( CannonBall �Ҹ� �� ����Ʈ ��� )
*/
void CX2GUChung::StandUpCannonAttackFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			//{{ kimhc // 2011.1.14 // û 1�� ����
			bool bApplyGuardMastery = false;
			if ( GetRandomFloat() <= GetProbabilityBigDamageAtGuardMastery() )
				bApplyGuardMastery = true;

			switch ( CHUNG_STATE_ID( GetNowStateID() ) )
			{
			case CSI_STANDUP_CANNON_ATTACK_FRONT:
				{
					if ( true == bApplyGuardMastery )
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_STANDUP_CANNON_ATTACK_FRONT_BIG_DAMAGE", this );
					else
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_STANDUP_CANNON_ATTACK_FRONT", this );
				}
				break;

			case CSI_STANDUP_CANNON_ATTACK_BACK:
				{
					if ( true == bApplyGuardMastery )
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_STANDUP_CANNON_ATTACK_BACK_BIG_DAMAGE", this );
					else
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_STANDUP_CANNON_ATTACK_BACK", this );
				}
				break;

			default:
				ASSERT( !L"Wrong Path" );
				break;
			}
			//}} kimhc // 2011.1.14 // û 1�� ����

			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}

/** @function : StandUpCannonAttackEventProcess
	@brief : StandUpCannonAttack�� EventProcess ( Wait���� ���·� ���� )
*/
void CX2GUChung::StandUpCannonAttackEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 0.833f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );

			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : StandUpCannonAttackEnd
	@brief : StandUpCannonAttack�� End ( ���� ������Ʈ�� ������ Invincible ���� ���� ���� )
*/
void CX2GUChung::StandUpCannonAttackEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}

/** @function : StandUpCannonAttackEndFuture
	@brief : StandUpCannonAttack�� EndFuture (������ ȸ�� �����Ϳ� IsRight ���� ����)
*/
void CX2GUChung::StandUpCannonAttackEndFuture()
{
	CommonStateEndFuture();

	// ������ �ݴ��� �˷���
	m_FrameDataFuture.syncData.bIsRight				= !m_FrameDataFuture.syncData.bIsRight;

	// ȸ���ߴٰ� �˷���
	m_FrameDataFuture.unitCondition.dirDegree.y		+= 180.0f;
}

//{{ kimhc // 2011.1.4 // û 1�� ���� ����



/** @function : CFGComboZZXEventProcess
	@brief : û 1�� ���� ǻ��������� ZZX ���� EventProcess (ĵ�� ����Ʈ �� ����)
*/
void CX2GUChung::CFGComboZZXEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 1.16f;

	IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )

	// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function : CFGComboDashZEventProcess
	@brief : û 1�� ���� ǻ��������� ��� Z ���� EventProcess ( ���� ������Ʈ�� �Ѿ�� ���� �Ǵ� )
*/
void CX2GUChung::CFGComboDashZEventProcess()
{
// 	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.5f;
//
// 	if (  LOWER_BOUND_INPUT_TIME_CANCEL < m_pXSkinAnimFuture->GetNowAnimationTime() )
// 	{
// 		if ( true == SpecialAttackEventProcess() )
// 		{
// 		}
// 		else if ( CAN_WALK_CANCEL )
// 		{
// 			StateChange( USI_WALK );
// 		}
// 		else if ( CAN_DASH_CANCEL )
// 		{
// 			StateChangeDashIfPossible();
// 		}
// 		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
// 		{
// 			StateChange( USI_WALK );
// 		}
// 	}

	// ��� ZZ �Է�
	bool& bCanChangeCFGComboDashZZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �б���
	const float	TRIGGER_TIME_CFG_COMBO_DASH_Z = 0.38f;
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.5f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		} // if
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		} // else
	} // if
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}
	// �б��� ��
	else if ( TRIGGER_TIME_CFG_COMBO_DASH_Z > m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		// �б��� ���� �޺� �Է� ����
		if ( true == m_InputData.oneZ )
		{
			bCanChangeCFGComboDashZZ = true;
		} // if
	} // if

	if ( TRIGGER_TIME_CFG_COMBO_DASH_Z <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
		 true == bCanChangeCFGComboDashZZ )
	{
			StateChange( CSI_CFG_COMBO_DASH_ZZ );
	} // if


	CommonEventProcess();
}

/** @function : CFGComboDashZZEventProcess
	@brief : û 1�� ���� ǻ��������� ��� ZZ ���� EventProcess
*/
void CX2GUChung::CFGComboDashZZEventProcess()
{
	bool& bCanChangeCFGComboDashZZZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �� �ð� ���� ���� �Է� ����
	const float LOWER_BOUND_INPUT_TIME_COMBO_DASH_ZZZ = 0.6f;
	// �б���
	const float	TRIGGER_TIME_CFG_COMBO_DASH_ZZ = 0.87f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		} // if
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		} // else
	} // if
	// �б��� ��
	else if ( LOWER_BOUND_INPUT_TIME_COMBO_DASH_ZZZ < m_pXSkinAnimFuture->GetNowAnimationTime()
		&& TRIGGER_TIME_CFG_COMBO_DASH_ZZ > m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		// �б��� ���� �޺� �Է� ����
		if ( true == m_InputData.oneZ )
		{
			bCanChangeCFGComboDashZZZ = true;
		} // if
	} // if
	// �б��� ����, �ִϸ��̼� ������ ��
	else
	{
		if ( true == bCanChangeCFGComboDashZZZ )
		{
			StateChange( CSI_CFG_COMBO_DASH_ZZZ );
		} // if
	}

	CommonEventProcess();
}

/** @function : CFGComboDashZZZEventProcess
	@brief : û 1�� ���� ǻ��������� ��� ZZZ ���� EventProcess
*/
void CX2GUChung::CFGComboDashZZZEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.633f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		} // if
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		} // else
	} // if
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}

	CommonEventProcess();
}

/** @function : CFGComboDashJumpZXEventProcess
	@brief : û 1�� ���� ǻ�� ������� DashJump ZX ���� EventProcess
*/
void CX2GUChung::CFGComboDashJumpZXEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool& bCanChangeCFGComboDashJumpZXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �б���
	const float TRIGGER_TIME_CFG_COMBO_DASH_JUMP_ZX = 0.45f;
	const float TRIGGER_TIME_FORCE_DOWN = 0.6f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �� �Ʒ��� ���θ��� ������ ����
		if ( true == IsOnSomethingFuture() )
		{
			// �Ʒ��� ���� ȭ��ǥ�� ���� ���
			if( true == m_bDownForce )
			{
				StateChange( CSI_POWER_DOWN_LANDING );
				m_bDownForce = false;
			}
			else
			{
				StateChange( USI_JUMP_LANDING );
				m_bDownForce = false;
			}
		}
		else
		{
			if ( true == m_InputData.oneZ )
			{
				StateChange( CSI_ATTACK_JUMP_Z );
				m_bDownForce = false;
			}
			else if ( true == m_InputData.oneX )
			{
				StateChange( CSI_ATTACK_JUMP_X );
				m_bDownForce = false;
			}
		}
	} // if
	// �б�ð� ���̸�
	else if ( TRIGGER_TIME_CFG_COMBO_DASH_JUMP_ZX < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( true == m_InputData.oneX )
		{
			StateChange( CSI_CFG_COMBO_DASH_JUMP_ZXX );
			m_bDownForce = false;
		} // if

	} // else if
	// �б�ð� ���� ��

	if( true == m_InputData.oneDown )
	{
		// ��ȹ �ǵ�� ����
		m_bDownForce = true;
		bCanChangeCFGComboDashJumpZXX = false;
	}


	if( true == m_bDownForce &&
		TRIGGER_TIME_FORCE_DOWN < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
	}

	CommonEventProcess();
}

/** @function : CFGComboDashJumpZXXStart
	@brief : û 1�� ���� ǻ�� ������� DashJump ZXX ���� Start
*/
void CX2GUChung::CFGComboDashJumpZXXStart()
{
	// ������ �ݴ��� �˷���
	//m_FrameDataNow.syncData.bIsRight			= !m_FrameDataNow.syncData.bIsRight;

	// ȸ���ߴٰ� �˷���
	//m_FrameDataNow.unitCondition.dirDegree.y	+= 180.0f;

	CommonStateStart();
}

/** @function : CFGComboDashJumpZXXEventProcess
	@brief : û 1�� ���� ǻ�� ������� DashJump ZXX ���� EventProcess
*/
void CX2GUChung::CFGComboDashJumpZXXEventProcess()
{
	// �ִϸ��̼��� ������ ���θ� ���� ���� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		 true == IsOnSomethingFuture() )
	{
		StateChange( CSI_CFG_COMBO_DASH_JUMP_ZXXLanding );
	}

	CommonEventProcess();
}

/** @function : CFGComboDashJumpZXXLandingEventProcess
	@brief : û 1�� ���� ǻ�� ������� DashJump ZXX ������ Landing �� EventProcess
*/
void CX2GUChung::CFGComboDashJumpZXXLandingEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.3f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )

	CommonEventProcess();
}

/** @function : void CSGComboXFrontXPullSubweaponInit()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� �ʱ�ȭ �Լ�
*/
void CX2GUChung::CSGComboXFrontXPullSubweaponInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_XFRONTX_PULL_SUBWEAPON.X");
}

/** @function : void CSGComboXFrontXPullSubweaponFrameMove()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� ��ũó��
*/
void CX2GUChung::CSGComboXFrontXPullSubweaponFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CX2DamageEffect::CEffect* pDamageEffect = NULL;
		pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON_Mini_Gun", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pDamageEffect )
		{
			CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_MiniGun;
			hHandle_MiniGun = pDamageEffect->GetMainEffectHandle();
			if( INVALID_MESH_INSTANCE_HANDLE != hHandle_MiniGun )
			{
				SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_PULL_SUBWEAPON_MINI_GUN, hHandle_MiniGun );
			}
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_PULL_SUBWEAPON_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_PULL_SUBWEAPON_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );

			//D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );
			//D3DXVECTOR3 vBoneRotate = GetRotateDegree();
			//pMeshInst_MiniGun->SetPos(vBonePos);
			//pMeshInst_MiniGun->SetRotateDegree(vBoneRotate);
			//pMeshInst_MiniGun->SetMoveAxisAngleDegree( vBoneRotate );
		}
	}
	CommonFrameMove();
}
/** @function : void CSGComboXFrontXPullSubweaponEventProcess()
	@brief : û 1�� ���� ���� ����� ����
	@brief : �ǹ� ���͸� �̴� ������Ʈ
	@brief : Combo X(������) / Combo ->X
*/
void CX2GUChung::CSGComboXFrontXPullSubweaponEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		// �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( CSI_CSG_COMBO_XFRONTX_FIRE );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}
/** @function : void CSGComboXFrontXPullSubweaponEnd()
	@brief : û 1�� ���� ���� ����� ����
	@brief : ���� ���ҽ� �ʱ�ȭ
*/
void CX2GUChung::CSGComboXFrontXPullSubweaponEnd()
{
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_PULL_SUBWEAPON_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE);
	CommonStateEnd();
}
/** @function : void CSGComboXFrontXFireInit()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� �ʱ�ȭ �Լ�
*/
void CX2GUChung::CSGComboXFrontXFireInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_XFRONTX_FIRE.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}
/** @function : void CSGComboXFrontXFireFrameMove()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� ��ũó��
*/
void CX2GUChung::CSGComboXFrontXFireFrameMove()
{
	//{ �̴ϰ� �𵨸� ��ũ
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
            pMeshInst_MiniGun->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}
	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}
	//} �̴ϰ� �𵨸� ��ũ

	//# �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Dummy01");
		}
	}

	//# �� 3�� �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, fDegreeZ );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
														0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			PlaySound( L"Chung_MiniGun_Fire2.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, fDegreeZ );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.72f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell_Big", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
														0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, fDegreeZ );
		}
	}

	CommonFrameMove();
}
/** @function : void CSGComboXFrontXFireEventProcess()
	@brief : û 1�� ���� ���� ����� ����
	@brief : �ǹ� ���͸� �̾� 3���� ����ϴ� ������Ʈ
	@brief : Combo X(������) / Combo ->X
	@brief : û 2�� ������ �Բ� Combo Key ���� Combo ->Z
*/
void CX2GUChung::CSGComboXFrontXFireEventProcess()
{
	bool &bCanChangeStateComboXXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_XFRONTXX = 1.02f;	// CSG_CSF_COMBO_XFRONTXX�� �б���

		// �б� �ð� ����
		if ( TRIGGER_TIME_COMBO_XFRONTXX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			//{{ JHKang / ������ / 2011.6.14 / ���� �޺� Ű ����(X>XX -> X>ZZ)
			if ( true == m_InputData.oneZ )
			//}}
				bCanChangeStateComboXXX = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXXX )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_CSG_COMBO_XFRONTXX );
			}

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		const float LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 = 1.2f;
		const float LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 = 0.43f;

		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 )	/// 1.2�� ���� ��, 0.43�� �� �˻�??? �׳� 1.2�� ������ �˻� ����
		{
			StateChange( USI_WALK );
		}
		SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 )

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}
/** @function : void CSGComboXFrontXFireEnd()
	@brief : û 1�� ���� ���� ����� ����
	@brief : ���ҽ� �ʱ�ȭ
*/
void CX2GUChung::CSGComboXFrontXFireEnd()
{
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE);
	CommonStateEnd();
}
/** @function : void CSGComboXFrontXXInit()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� �ʱ�ȭ �Լ�
*/
void CX2GUChung::CSGComboXFrontXXInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_XFRONTXX.X");
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}
/** @function : void CSGComboXFrontXXFrameMove()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� ��ũó��
*/
void CX2GUChung::CSGComboXFrontXXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_XFRONTXX_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_XFRONTXX_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTXX_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_XFRONTXX_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.44f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ �Ѱ� �̻� �ְų� ����ȭ ����
			if ( 1 <= GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CSG_COMBO_XFRONTXX_CANNON_BALL_SHOT", this );
				UseCannon( 1 );
			}
			// ĳ�� ��õ�� źȯ�� ������ �׳� ������
			else
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
			}
		}
	}

	CommonFrameMove();
}
/** @function : void CSGComboXFrontXXEventProcess()
	@brief : û 1�� ���� ���� ����� ����
	@brief : 3���� �� ö���� ���ϰ� �ѹ� ��� ������Ʈ
	@brief : Combo X(������) / Combo ->X / Combo XX
	@brief : û 2�� ������ �Բ� Combo Key ���� Combo ZZ
*/
void CX2GUChung::CSGComboXFrontXXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.9f;

		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WALK );
		}

		// �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}
/** @function : void CSGComboXFrontXXEnd()
	@brief : û 1�� ���� ���� ����� ����
	@brief : ���ҽ� �ʱ�ȭ
*/
void CX2GUChung::CSGComboXFrontXXEnd()
{
	SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_XFRONTXX_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE );

	CommonStateEnd();
}

/** @function : void CSGComboDashXXInit()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� �ʱ�ȭ
	@brief : ->-> X(����) / XX
*/
void CX2GUChung::CSGComboDashXXInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_DASH_XX.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}

/** @function : void CSGComboDashXXFrameMove()
	@brief : û 1�� ���� ���� ����� ���� �� ���ҽ� ��ũó��
	@brief : ->-> X(����) / XX
*/
void CX2GUChung::CSGComboDashXXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_DASH_XX_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
            pMeshInst_MiniGun->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Dummy01");
		}
	}

	//# �� 2�� �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.533f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_DASH_XX_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, fDegreeZ );
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.93f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_DASH_XX_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
				SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire2.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, fDegreeZ );
		}
	}
	CommonFrameMove();
}
/** @function : void CSGComboDashXXEventProcess()
	@brief : û 1�� ���� ���� ����� ���� �̺�Ʈ ó��
	@brief : ->-> X(����) / XX
*/
void CX2GUChung::CSGComboDashXXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		//bool &bCanChangeStateCSGComboDashXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

		const float TRIGGER_TIME_CSG_COMBO_DASH_XX = 1.1f;	// CSG_COMBO_DASH_XXX �� �б���

		// �б� �ð� ����
		if ( TRIGGER_TIME_CSG_COMBO_DASH_XX < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneX )
				//bCanChangeStateCSGComboDashXX = true;
				StateChange( CSI_CSG_COMBO_DASH_XXX );
		}
		// �б� �ð� ����..
		//else
		//{
		//	// �޺� Ű�� ���Ⱦ�����
		//	if ( true == bCanChangeStateCSGComboDashXX )
		//	{
		//		// ���� �޺��� ���� ����
		//		StateChange( CSI_CSG_COMBO_DASH_XXX );
		//	}

		//	// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
		//	// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		//}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	}

	CommonEventProcess();
}
/** @function : void CSGComboDashXXEnd()
	@brief : û 1�� ���� ���� ����� ���� ������Ʈ ����ó��(���ҽ� �Ҹ��)
	@brief : ->-> X(����) / XX
*/
void CX2GUChung::CSGComboDashXXEnd()
{
	SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE );
	CommonStateEnd();
}
/** @function : void CSGComboDashXXXInit()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� �ʱ�ȭ
	@brief : ->-> X(����) / XXX
*/
void CX2GUChung::CSGComboDashXXXInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_DASH_XXX.X");
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}
/** @function : void CSGComboDashXXXFrameMove()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� ��ũó��
	@brief : ->-> X(����) / XX / XXX
*/
void CX2GUChung::CSGComboDashXXXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_DASH_XXX_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_XXX_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_XXX_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_XXX_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.48f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ �Ѱ� �̻� �ְų� ����ȭ ����
			if ( 1 <= GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CSG_COMBO_DASH_XXX_CANNON_BALL_SHOT", this );
				UseCannon( 1 );
			}
			// ĳ�� ��õ�� źȯ�� ������ �׳� ������
			else
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
			}
		}
	}

	CommonFrameMove();
}
/** @function : void CSGComboDashXXXEventProcess()
	@brief : û 1�� ���� ���� ����� ���� - �̺�Ʈ ó��
	@brief : ->-> X(����) / XX / XXX
*/
void CX2GUChung::CSGComboDashXXXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{

		// �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	}

	CommonEventProcess();
}
/** @function : void CSGComboDashXXXEnd()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : ->-> X(����) / XX / XXX
*/
void CX2GUChung::CSGComboDashXXXEnd()
{
	SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_XXX_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE );
	CommonStateEnd();
}
/** @function : void CSGComboDashJumpXInit()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� �ʱ�ȭ
	@brief : dash jump X(����) / x(�̴ϰ� ù�� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_DASHJUMP.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");

}
/** @function : void CSGComboDashJumpXFrameMove()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� ��ũó��
	@brief : dash jump X(����) / x(�̴ϰ� ù�� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_DASH_JUMP_X_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
            pMeshInst_MiniGun->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �� 1�� �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.36f ) )
	{
		//# �ѹ߻� ��ġ�� �޾ƿ��� ����
		D3DXVECTOR3 vBonePos_MiniGun;
		D3DXVECTOR3 vDegree_MiniGun_Shell = GetRotateDegree();
		vDegree_MiniGun_Shell.z = -35.f;

		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN));
			if( NULL != pMeshInst_MiniGun )
			{
				vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Dummy01");
			}
		}

		if( FlushMp( 4.0f ) == true )
		{

			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_DASH_JUMP_XX_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, vDegree_MiniGun_Shell, vDegree_MiniGun_Shell,
															0,false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, vDegree_MiniGun_Shell.z );
		}
	}

	CommonFrameMove();
}
/** @function : void CSGComboDashJumpXEventProcess()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : dash jump X(����) / x(�̴ϰ� ù�� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( CSI_CSG_COMBO_DASH_JUMP_XX_Landing );
	}
	else
	{
		bool &bCanChangeStateCSGComboDashJumpX = m_FrameDataFuture.stateParam.bEventFlagList[0];


		if ( true == m_InputData.oneX )
			bCanChangeStateCSGComboDashJumpX = true;

		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		//if( LOWER_BOUND_INPUT_TIME_COMBO < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateCSGComboDashJumpX )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_CSG_COMBO_DASH_JUMP_XX );
			}

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}
		// �б� �ð� ����
		//else if ( LOWER_BOUND_INPUT_TIME_COMBO >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		//{
		//	// �޺� Ű�� ������, �����ٴ� �����͸� ����
		//	if ( true == m_InputData.oneX )
		//		bCanChangeStateCSGComboDashJumpX = true;
		//}
		// �б� �ð� ����..
	}

	if ( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
	}

	CommonEventProcess();
}
/** @function : void CSGComboDashJumpXEnd()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : dash jump X(����) / x(�̴ϰ� ù�� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXEnd()
{
	SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE );

	CommonStateEnd();
}
/** @function : void CSGComboDashJumpXXInit()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� �ʱ�ȭ
	@brief : dash jump X(����) / XX(�̴ϰ� ���ӹ߻� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXXInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_DASHJUMP_XX.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}
/** @function : void CSGComboDashJumpXXFrameMove()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� ��ũó��
	@brief : dash jump X(����) / XX(�̴ϰ� ���ӹ߻� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_DASH_JUMP_XX_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
            pMeshInst_MiniGun->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �� 1�� �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		//# �ѹ߻� ��ġ�� �޾ƿ��� ����
		D3DXVECTOR3 vBonePos_MiniGun;
		D3DXVECTOR3 vDegree_MiniGun_Shell = GetRotateDegree();
		vDegree_MiniGun_Shell.z = -35.f;

		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN));
			if( NULL != pMeshInst_MiniGun )
			{
				vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Dummy01");
			}
		}

		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_DASH_JUMP_XX_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, vDegree_MiniGun_Shell, vDegree_MiniGun_Shell,
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
				SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire2.ogg" );

			++m_iCountbyCsgComboDashJumpXXMiniGun;
		}
		else
		{
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, vDegree_MiniGun_Shell.z );
			m_iCountbyCsgComboDashJumpXXMiniGun = _CONST_CHUNG_::MAX_NUMBER_OF_CSG_COMBO_DASH_JUMP_XX_MINI_GUN;
		}
	}
	CommonFrameMove();
}
/** @function : void CSGComboDashJumpXXEventProcess()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : dash jump X(����) / XX(�̴ϰ� ���ӹ߻� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXXEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( CSI_CSG_COMBO_DASH_JUMP_XX_Landing );
		m_iCountbyCsgComboDashJumpXXMiniGun = 0;
	}
	else
	{
		bool &bCanChangeStateCSGComboDashJumpXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

		// �޺� Ű�� ������, �����ٴ� �����͸� ����
		if ( true == m_InputData.oneX )
			bCanChangeStateCSGComboDashJumpXX = true;

		const float TRIGGER_TIME_CSI_CSG_COMBO_DASH_JUMP_XX = 0.4f;	// CSI_CSG_COMBO_DASH_JUMP_XX �� �б���

		// �б� �ð� ����
		//if ( TRIGGER_TIME_CSI_CSG_COMBO_DASH_JUMP_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		// �б��� ���� �ִϸ��̼� ������ �б�
		if( TRIGGER_TIME_CSI_CSG_COMBO_DASH_JUMP_XX < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateCSGComboDashJumpXX && _CONST_CHUNG_::MAX_NUMBER_OF_CSG_COMBO_DASH_JUMP_XX_MINI_GUN > m_iCountbyCsgComboDashJumpXXMiniGun )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_CSG_COMBO_DASH_JUMP_XX );
			}

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
 		}
// 		// �б� �ð� ����..
// 		// �ִϸ��̼� ������
// 		else
// 		{
//
// 		}
	}

	if ( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
	}

	CommonEventProcess();
}
/** @function : void CSGComboDashJumpXXEnd()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : dash jump X(����) / XX(�̴ϰ� ���ӹ߻� ������Ʈ)
*/
void CX2GUChung::CSGComboDashJumpXXEnd()
{
	SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE );
	CommonStateEnd();
}
/** @function : void CSGComboDashJumpXXLandingInit()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� �ʱ�ȭ
	@brief : dash jump X(����) / X landing(�̴ϰ� ���� �ִ� ����)
*/
void CX2GUChung::CSGComboDashJumpXXLandingInit()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_COMBO_DASHJUMP_X_DownLanding.X");
}
/** @function : void CSGComboDashJumpXXLandingFrameMove()
	@brief : û 1�� ���� ���� ����� ���� - ���ҽ� ��ũó��
	@brief : dash jump X(����) / X landing(�̴ϰ� ���� �ִ� ����)
*/
void CX2GUChung::CSGComboDashJumpXXLandingFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CSG_COMBO_DASH_JUMP_XX_LANDING_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_LANDING_MINI_GUN, pMeshInst_MiniGun->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_LANDING_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_LANDING_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	CommonFrameMove();
}
/** @function : void CSGComboDashJumpXXLandingEventProcess()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : dash jump X(����) / X landing(�̴ϰ� ���� �ִ� ����)
*/
void CX2GUChung::CSGComboDashJumpXXLandingEventProcess()
{
	//if( true == IsOnSomethingFuture() )
	//{
	//	StateChange( USI_WAIT );
	//}
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if ( true == m_InputData.oneZ )
	{
		StateChange( CSI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		if( GetUnitClass() == CX2Unit::UC_CHUNG_DEADLY_CHASER )
			StateChange( CSI_CDC_COMBO_X );
		else
			StateChange( CSI_COMBO_X );
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
		// �Ʒ��� ���� ȭ��ǥ Ű�� ������, ������ ���� �� �� �ִٸ�
	else if( true == m_InputData.oneDown
	&& true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
	&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ��� Ű�� ��� ������ ������ ��ŵ ����
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		StateChange( USI_WAIT, false );
	}

	CommonEventProcess();
}
/** @function : void CSGComboDashJumpXXLandingEnd()
	@brief : û 1�� ���� ���� ����� ���� - �������� ����ó��
	@brief : dash jump X(����) / X landing(�̴ϰ� ���� �ִ� ����)
*/
void CX2GUChung::CSGComboDashJumpXXLandingEnd()
{
	SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_LANDING_MINI_GUN, INVALID_MESH_INSTANCE_HANDLE );

	CommonStateEnd();
}

//}} kimhc // 2011.1.4 // û 1�� ���� ����

/** @function : CSI_SI_SA_CHUNG_DETONATION_START_StartFuture
	@brief : ������̼��� StartFuture (��ǿ����� ���� ����)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_START_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_START_FrameMove
	@brief : CSI_SI_SA_CHUNG_DETONATION_START�� FrameMove (����Ʈ �� ���)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_START_FrameMove()
{

// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.003f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_START_EventProcess
	@brief : CSI_SI_SA_CHUNG_DETONATION_START�� EventProcess (ĳ���� ���� ���¿� ���� ���� �ٸ� ���� ���·� ����)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_START_EventProcess()
{
	if ( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// ����ȭ��尡 �ƴϸ� ĳ���� ������ ����
		if ( 1 >= GetCannonBallCount() && false == IsFullHyperMode() )
		{
			StateChange( CSI_SI_SA_CHUNG_DETONATION_END );
		}
		else
		{
			StateChange( CSI_SI_SA_CHUNG_DETONATION_LOOP );
		}
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_LOOP_Init
	@brief : CSI_SI_SA_CHUNG_DETONATION_LOOP���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_LOOP_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	TextureReadyInBackground( L"Smoke.dds" );
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_LOOP_StartFuture
	@brief : ������̼� Loop �� StartFuture (��� ������ ���� �ȵǵ��� ��)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_LOOP_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_LOOP_FrameMove
	@brief : CSI_SI_SA_CHUNG_DETONATION_LOOP�� FrameMove (����Ʈ ��� �� �ִϸ��̼� �ݺ�)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_LOOP_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		int iMaxCountAnimationLoopCountAtThisState = 0;

		//{{ kimhc // 2011-03-28 // û �޸� (ź�� ����)
		// ź�� ���� �޸� ���� �Ǿ� ������
		const bool bEquippedChungMemo01 = GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO1 );

		if ( true == bEquippedChungMemo01 )
			iMaxCountAnimationLoopCountAtThisState = MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE - 1;	// 5ȸ�� ���
		else
		//}} kimhc // 2011-03-28 // û �޸� (ź�� ����)

		iMaxCountAnimationLoopCountAtThisState = MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE;

		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( m_pXSkinAnim->GetNowPlayCount() < iMaxCountAnimationLoopCountAtThisState )
		{
			// ĳ������ 1�� �̻� �ְų� ����ȭ ����
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				CX2EffectSet::Handle hEffectSetDetonation =
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CHUNG_DETONATION_LOOP", this,
					NULL, false, GetPowerRate() );

			//{{ kimhc // 2011-03-28 // û �޸� (ź�� ����)
				if ( true == bEquippedChungMemo01 )
				{
					if ( CX2EffectSet::EffectSetInstance* pInstance = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectSetDetonation ) )
					{
						pInstance->SetPowerRateScale( 1.25f );
					}
				}
			//}} kimhc // 2011-03-28 // û �޸� (ź�� ����)

				UseCannon( 1 );
			}
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_LOOP_EventProcess
	@brief : CSI_SI_SA_CHUNG_DETONATION_LOOP�� EventProcess ( ���ǿ� ���� ���� ���·� ���� )
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_LOOP_EventProcess()
{
	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� ����������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
			StateChange( CSI_SI_SA_CHUNG_DETONATION_END );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_END_Init
	@brief : CSI_SI_SA_CHUNG_DETONATION_END���� �ʿ��� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_END_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	TextureReadyInBackground( L"Smoke.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_END_StartFuture
	@brief : ������̼� End (��� ������ ���� �ȵǵ��� ��)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_END_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_END_FrameMove
	@brief : CSI_SI_SA_CHUNG_DETONATION_END�� FrameMove ( ����Ʈ ��� �� ĳ���� ��� )
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CHUNG_DETONATION_END", this );
			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_DETONATION_END_EventProcess
	@brief : CSI_SI_SA_CHUNG_DETONATION_END�� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CHUNG_DETONATION_END_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_FrameMove
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_START�� FrameMove (��ų ���� ��� �� ���� Į�� ����)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_EventProcess
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_START�� EventProcess ( ĳ���� ���ο� ���� ���� �ٸ� ���� ���·� ���� )
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_EventProcess()
{
	if ( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// ����ȭ��尡 �ƴϸ� ĳ���� ������ ����
		if ( 1 >= GetCannonBallCount() && false == IsFullHyperMode() )
		{
			StateChange( CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH );
		}
		else
		{
			StateChange( CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION );
		}
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_Init
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION�� Init ( �ʿ��� ���ҽ� �ε� )
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XSkinMeshReadyInBackground( L"NEPHILIM_DashAttackA_01.X" );
	XSkinMeshReadyInBackground( L"EarthBreaker_Mesh01.X" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	XMeshReadyInBackground( L"VANGUARD_MagicAttack_Explosion.Y" );
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_FrameMove
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION�� FrameMove (����Ʈ ���)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ 1�� �̻� �ְų� ����ȭ ����
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION", this );
				UseCannon( 1 );
			}
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
		m_DamageData.hitUnitList.clear();
        ClearAnimEventTimerOneshotNow();
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_EventProcess
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION�� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_EventProcess()
{
	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� ����������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
			StateChange( CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_Init
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH�� Init ( �ʿ��� ���ҽ� �ε� )
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XSkinMeshReadyInBackground( L"NEPHILIM_DashAttackA_01.X" );
	XSkinMeshReadyInBackground( L"EarthBreaker_Mesh01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust02.X" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	XMeshReadyInBackground( L"VANGUARD_MagicAttack_Explosion.Y" );
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_FrameMove
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH�� FrameMove (����Ʈ ���)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH", this );
			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_EventProcess
	@brief : CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH�� EventProcess (����Ʈ ���)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_A_CHUNG_RELOAD_CANNON_Init
	@brief : CSI_SI_A_CHUNG_RELOAD_CANNON�� Init (���ҽ� �ε�)
*/
void CX2GUChung::CSI_SI_A_CHUNG_RELOAD_CANNON_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust02.X" );
	XSkinMeshReadyInBackground( L"Raven_RRF_Ground_Impact_AttackBox01.X" );
	XMeshReadyInBackground( L"VANGUARD_MagicAttack_Explosion.Y" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
}

/** @function : CSI_SI_A_CHUNG_RELOAD_CANNON_Start
	@brief : ���ε� ĳ�� �����͸� ������ ����
*/
void CX2GUChung::CSI_SI_A_CHUNG_RELOAD_CANNON_Start()
{
	CommonStateStart();

	m_NowStateData.m_fPowerRate					*= GetDamageRelByCannonBallMastery();
	m_DamageData.damage.fPhysic					*= GetDamageRelByCannonBallMastery();
	m_DamageData.damage.fMagic					*= GetDamageRelByCannonBallMastery();
	m_DamageData.m_ExtraDamage.m_DamagePerSec	*= GetDamageRelByCannonBallMastery();
}

/** @function : CSI_SI_A_CHUNG_RELOAD_CANNON_FrameMove
	@brief : CSI_SI_A_CHUNG_RELOAD_CANNON�� FrameMove (����Ʈ ���)
*/
void CX2GUChung::CSI_SI_A_CHUNG_RELOAD_CANNON_FrameMove()
{

	int iExtraChargeCannonBall = 0;
	//{{ kimhc // 2011-03-30 // 2011-04 �� ��ġ�� û �޸� ��ų
	// û '��ֱܳ�' �޸� ���� ���̰� 50% Ȯ���� ��� �ߴٸ�
	if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO5 ) && 0.5f > GetRandomFloat( 0 ) )
	{
		// ���ε� ĳ�� 1�� �� ����
		++iExtraChargeCannonBall;
	}
	//}} kimhc // 2011-03-30 // 2011-04 �� ��ġ�� û �޸� ��ų
	// ���ε� ĳ���� ���ؼ� �����Ǿ��� ĳ������ ���� ����
	UpCannonBallCountAtThisAnimationTime( 0.52f, GetCannonBallCountToBeUpUsingReLoadCannon() + iExtraChargeCannonBall );

	CommonFrameMove();
}

/** @function : CSI_SI_A_CHUNG_RELOAD_CANNON_EventProcess
	@brief : CSI_SI_A_CHUNG_RELOAD_CANNON�� EventProcess (� ���·� �Ѿ ������ �Ǵ���)
*/
void CX2GUChung::CSI_SI_A_CHUNG_RELOAD_CANNON_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų�� �ʱ�ȭ �Լ�
	@brief : - Gigantic impact start state ���� ���Ǵ� ���ҽ� �̸� �ε��ϴ� ����
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init()
{
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų ��� ���۽� �ѹ� ȣ��Ǿ� �������� �ϴ� �Լ�
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateStartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų ���� framemove�� ���鼭 ��ųó��
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.566f );

	CommonFrameMove();
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų ���� ����ڸ��� ���� framemove�� ���鼭 ���� �� p2p push
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų ���� ������� �Է� �� �ִϸ��̼� ��Ȳ�� üũ�Ͽ� ��ųó��
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_EventProcess()
{
	//if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.5f )
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY );
	}
	CommonEventProcess();
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų ���Ϸ� ������ ȣ��Ǿ� ��ų�� ���Ǵ� �ڿ� ������
	@brief : Ex. ��ų�� ���Ǵ� ���ҽ�(�ؽ���,����Ʈ,�ִϸ��̼ǵ�..)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateEnd()
{
	CommonStateEnd();
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų ���Ϸ� ������ ȣ��Ǿ� �Է´������ ����ó��
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateEndFuture()
{
	CommonStateEndFuture();
}
/** @function : CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init
	@brief : û�� Gigantic impact ��ų�� �ʱ�ȭ �Լ�
	@brief : �̷������Ǵ� ���ҽ�
	@brief : XSkinMeshReady(L"CHUNG_Cannon_Effect01.X");
	@brief : XSkinMeshReady(L"CHUNG_Cannon_Effect02.X");
	@brief : XSkinMeshReady(L"CHUNG_Cannon_Effect03.X");
	@brief : XSkinMeshReady(L"CHUNG_GIGANTIC_IMPACT04.X");
	@brief : XSkinMeshReady(L"CHUNG_GIGANTIC_IMPACT05.X");
	@brief : XSkinMeshReady(L"VANGUARD_MagicAttack_Explosion_03.X");
	@brief : TextureReady(L"ColorBallFire.dds");
	@brief : TextureReady(L"GroundShockWave02.dds");
	@brief : TextureReady(L"Arme_Ring2.dds");
	@brief : TextureReady(L"Steam_Bp.dds");
*/
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_Init()
{
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect01.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect02.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect03.X");
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT04.X");
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT05.X");
	XSkinMeshReadyInBackground(L"VANGUARD_MagicAttack_Explosion_03.X");

	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Steam_Bp.dds");
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_FrameMove()
{
	CommonFrameMove();
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_EventProcess()
{
	if( true == IsOnSomethingFuture() )
		StateChange( CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING );

    CommonEventProcess();
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_StateEnd()
{
	CommonStateEnd();
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_Init()
{
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT04.X");
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT05.X");
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT06.X");
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT07.X");
	XSkinMeshReadyInBackground(L"SI_SA_ROT_NUCLEAR_Effect_Dust01.X");
	XSkinMeshReadyInBackground(L"SI_SA_ROT_NUCLEAR_Effect_Dust02.X");

	XMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT_Bottom_Broken.Y");

	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"Steam_Bp.dds");
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_FrameMove()
{
	CommonFrameMove();
}
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_EventProcess()
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
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_Hyper_EventProcess()
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
void CX2GUChung::CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_StateEnd()
{
	CommonStateEnd();
}

/** @function : CSI_SI_SA_CHUNG_BRUTAL_SWING_Init
	@brief : ���Ż ������ ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CHUNG_BRUTAL_SWING_Init()
{
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh03.X" );
	XSkinMeshReadyInBackground( L"Raven_SI_A_RF_Thrust_Mesh01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_BRUTAL_SWING_02.X" );
}

/** @function : CSI_SI_SA_CHUNG_BRUTAL_SWING_StartFuture
	@brief : ���Ż ������ StartFuture (��� ������ ���� ����)
*/
void CX2GUChung::CSI_SI_SA_CHUNG_BRUTAL_SWING_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}

/** @function : CSI_SI_SA_CHUNG_BRUTAL_SWING_FrameMove
	@brief : ���� Į�� ����, ��ų����
*/
void CX2GUChung::CSI_SI_SA_CHUNG_BRUTAL_SWING_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������


    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO9 ) == true )
		{
			m_NowStateData.m_fPowerRate *= 1.2f;
			m_DamageData.damage.fPhysic	*= 1.2f;
			m_DamageData.damage.fMagic	*= 1.2f;
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CHUNG_BRUTAL_SWING_MEMO", this, NULL, false, GetPowerRate() );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CHUNG_BRUTAL_SWING", this, NULL, false, GetPowerRate() );
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_BRUTAL_SWING_EventProcess
	@brief : CSI_SI_SA_CHUNG_BRUTAL_SWING�� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CHUNG_BRUTAL_SWING_EventProcess()
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
/** @function : CSI_SI_SA_CHUNG_SCARE_CHASE_Init
	@brief : ���ɾ� ü�̽��� ���ҵ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CHUNG_SCARE_CHASE_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	XSkinMeshReadyInBackground( L"CHUNG_SCARE_CHASE_Shell.X" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	TextureReadyInBackground( L"Smoke.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"Explosion_Fire01.dds" );
	TextureReadyInBackground( L"Shell.tga" );
	TextureReadyInBackground( L"Explosion_Light_Ring01.tga" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
}

/** @function : CSI_SI_SA_CHUNG_SCARE_CHASE_FrameMove
	@brief : ���ɾ� ü�̽� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CHUNG_SCARE_CHASE_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

	UpCannonBallCountAtThisAnimationTime( 0.6f, 1 );

    if( true == m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{

		D3DXVECTOR3 vWeaponBonePos		= GetWeaponBonePos( 0, L"TRACE_START0", 0 );
		D3DXVECTOR3 vRotateDegree	= GetRotateDegree();

		//{{ kimhc // 2011-03-27 // 2011-04 �� ��ġ�� û �޸� ��ų
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO2 ) )
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_SCARE_CHASE_SMI_CHUNG_MEMO2", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		else
		//}} kimhc // 2011-03-27 // 2011-04 �� ��ġ�� û �޸� ��ų
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_SCARE_CHASE", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );

		UseCannon( 1 );

		//g_pX2Game->GetNearestNPCUnitUID()

	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CHUNG_SCARE_CHASE_EventProcess
	@brief : ���ɾ� ü�̽� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CHUNG_SCARE_CHASE_EventProcess()
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
void CX2GUChung::CSI_SI_A_CHUNG_AIMMING_SHOT_Init()
{
}

void CX2GUChung::CSI_SI_A_CHUNG_AIMMING_SHOT_StateStart()
{
	CommonStateStart();
}

void CX2GUChung::CSI_SI_A_CHUNG_AIMMING_SHOT_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 40.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 80.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		float fDegree = ( DoAimingTarget( 1000.f, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos ) * 180.f ) / D3DX_PI;
		if( m_bAimingTarget == true && (fDegree >= 0.001f || fDegree <= -0.001f) )
		{
			m_bRotateBone = true;
			m_fRotateBoneDegree = fDegree;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		float fRotateDegree = 0.f;
		fRotateDegree = m_fRotateBoneDegree;

		int iCbCount = 0;
		const CX2SkillTree::SkillTemplet* pSkillTempletUsing = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CC_AIMMING_SHOT );
		if( pSkillTempletUsing != NULL )
			iCbCount = (int)pSkillTempletUsing->m_usCBConsumption;

		if( m_bAimingTarget == true && ( GetCannonBallCount() >= iCbCount || IsFullHyperMode() == true ))
		{
			UseCannon(iCbCount);
			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT", this );
			if( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
				pEffect->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, fRotateDegree) );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT_MISFIRE", this );
			m_bAimingTarget = false;
			m_bRotateBone = false;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.17f ) )
	{
		if( m_bAimingTarget == true )
		{
			CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AIMING_EXPLOSION", GetPowerRate(), m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );

			//{{ kimhc // 2011-03-30 // 2011-04 �� ��ġ�� û �޸� ��ų
			if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO6 ) && NULL != pDamageEffectAimmingShot ) // ����!: ����������Ʈ
			{
				CX2DamageManager::DamageData& damageDataAimmingShot = pDamageEffectAimmingShot->GetDamageData();

				// ����: ������ ����Ÿ (����Ʈ �ƴ�)
				//if ( NULL != pDamageDataAimmingShot )
					damageDataAimmingShot.m_fRateModifier = 3.0f;
			}
				//}} kimhc // 2011-03-30 // 2011-04 �� ��ġ�� û �޸� ��ų
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		m_bRotateBone = false;
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_A_CHUNG_AIMMING_SHOT_EventProcess()
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

void CX2GUChung::CSI_SI_A_CHUNG_AIMMING_SHOT_StateEnd()
{
	m_bRotateBone = true;
	CommonStateEnd();
}





/** @function : CSI_SI_SA_CHUNG_ACELDAMA_Init
	@brief : �ưִٸ��� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CHUNG_ACELDAMA_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_ACELDAMA_Shell.X" );
	XMeshReadyInBackground( L"ACELDAMA_Shell.Y" );
}

/** @function : CSI_SI_SA_CHUNG_ACELDAMA_StateStart
	@brief : �ưִٸ��� StartStart
*/
void CX2GUChung::CSI_SI_SA_CHUNG_ACELDAMA_StateStart()
{
	CommonStateStart();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

/** @function : CSI_SI_SA_CHUNG_ACELDAMA_FrameMove
	@brief : �ưִٸ� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CHUNG_ACELDAMA_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		m_bDisableGravity = false;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CC_ACELDAMA );
		if( NULL != pSkillTemplet )
		{
			const int	NUM_OF_FIXED_POS_SHELL				= 10;
			const int	MAX_POS_Y							= 4000;
			const int	MIN_POS_Y							= 1000;
			const int	MAGIC_NUMBER_ARG					= 100;
			const float MAGIC_NUMBER_NOT_CHECK_LAND_TIME	= 0.3f;
			const int	START_POS_X[NUM_OF_FIXED_POS_SHELL]	= { -100, 100, -330, 330, -490, 490, -680, 680, -900, 900 };
			const int	RANDOM_GAP							= ( MAX_POS_Y - MIN_POS_Y ) / MAGIC_NUMBER_ARG;
			const int	ADDITIONAL_SHELL_POS_X_VAR			= 1200;


			std::multimap< float, D3DXVECTOR3 > mapGungnirPos;
			D3DXVECTOR3 vStartPos( GetPos() );
			D3DXVECTOR3 vTempPos( 0.0f, 0.0f, 0.0f );

			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			int NUM_OF_SHELL = (int)pSkillTemplet->GetSkillAbilityValue(CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, iSkillTempletLevel);

			int NUM_OF_ADDITIONAL_SHELL = 0;

			if(NUM_OF_SHELL > NUM_OF_FIXED_POS_SHELL)
			{
				NUM_OF_ADDITIONAL_SHELL = NUM_OF_SHELL - NUM_OF_FIXED_POS_SHELL;
				NUM_OF_SHELL = NUM_OF_FIXED_POS_SHELL;
			}

			for ( int i = 0; i < NUM_OF_SHELL; i++ )
			{
				D3DXVECTOR3 vDirVector = GetDirVector();

				vTempPos = vStartPos + vDirVector * static_cast< float >( START_POS_X[i] );
				vTempPos.y += ( GetRandomInt( i * 2 + 3 ) % ( RANDOM_GAP ) ) * MAGIC_NUMBER_ARG + MIN_POS_Y;

				mapGungnirPos.insert( std::make_pair( vTempPos.y, vTempPos ) );
			}

			for( int i = 0; i < NUM_OF_ADDITIONAL_SHELL; i++)
			{
				D3DXVECTOR3 vDirVector = GetDirVector();

				vTempPos = vStartPos + vDirVector * static_cast< float >( (GetRandomInt( i * 2 + 3 ) % ( ADDITIONAL_SHELL_POS_X_VAR )) - (ADDITIONAL_SHELL_POS_X_VAR/2) );
				vTempPos.y += ( GetRandomInt( i * 2 + 3 ) % ( RANDOM_GAP ) ) * MAGIC_NUMBER_ARG + MIN_POS_Y;

				mapGungnirPos.insert( std::make_pair( vTempPos.y, vTempPos ) );
			}


			std::multimap< float, D3DXVECTOR3 >::iterator mItrStartPos = mapGungnirPos.begin();
			CX2DamageEffect::CEffect* pPEffectInDamage = NULL;

			while ( mItrStartPos != mapGungnirPos.end() )
			{
				pPEffectInDamage = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_ACELDAMA_SHELL", GetPowerRate(), mItrStartPos->second,
					GetRotateDegree(), GetRotateDegree(), vStartPos.y, true, GetRandomInt( NUM_OF_ADDITIONAL_SHELL*4 + 3 ) / 100.f * 0.8f );


				if ( CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = ( pPEffectInDamage != NULL ) ? pPEffectInDamage->GetMainEffect() : NULL )
				{
					float addNotCheckLandTime = ( ( mItrStartPos->second.y - vStartPos.y - MIN_POS_Y ) / 1000 ) * MAGIC_NUMBER_NOT_CHECK_LAND_TIME;
					pMeshInstance->SetNotCheckLandTime( pMeshInstance->GetNotCheckLandTime() +  addNotCheckLandTime );
				}
				pPEffectInDamage = NULL;
				mItrStartPos++;
			}
		}
	}

	CommonFrameMove();
}
/** @function : CSI_SI_SA_CHUNG_ACELDAMA_EventProcess
	@brief : �ưִٸ� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CHUNG_ACELDAMA_EventProcess()
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

/** @function : CSI_SI_SA_CHUNG_ACELDAMA_StateEnd
	@brief : �ưִٸ� StateEnd
*/
void CX2GUChung::CSI_SI_SA_CHUNG_ACELDAMA_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}

//{{ kimhc // 2011.1.8 // û 1�� ����

/** @function : CSI_SI_SA_CFG_STEEL_EDGE_Init
	@brief : û 1�� ����, ǻ��������� ��ƿ���� ��ų�� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CFG_STEEL_EDGE_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_BRUTAL_SWING_02.X" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ColorBallGray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
}

/** @function : CSI_SI_SA_CFG_STEEL_EDGE_FrameMove
	@brief : û 1�� ����, ǻ��������� ��ƿ���� ��ų�� FrameMove�� ���� ���� ������
*/
void CX2GUChung::CSI_SI_SA_CFG_STEEL_EDGE_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.033f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}

	//{{ kimhc // 2011-03-28 // 2011-04 �� ��ġ�� û �޸� ��ų, ������(��ƿ����)
	// ������ �޸� ���� ��쿡
	if ( false == GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO3 ) )
		UseCannonAtThisAnimationTime( 0.93f, 1 );	// ĳ���� �Ҹ�
	//}} kimhc // 2011-03-28 // 2011-04 �� ��ġ�� û �޸� ��ų, ������(��ƿ����)

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_STEEL_EDGE_EventProcess
	@brief : û 1�� ����, ǻ��������� ��ƿ���� ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_STEEL_EDGE_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 2.133f;
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	else if ( true == IsOnSomethingFuture() )
	{
		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CFG_IRON_HOWLING_Init
	@brief : û 1�� ����, ǻ��������� ���̾� �Ͽ︵ ��ų�� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CFG_IRON_HOWLING_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XMeshReadyInBackground(	L"CHUNG_GIGANTIC_IMPACT_Bottom_Broken.Y" );

	TextureReadyInBackground( L"Pulsewave01.dds" );
	TextureReadyInBackground( L"Condense_Pulse01.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"ColorBallGray.dds" );
}

/** @function : CSI_SI_SA_CFG_IRON_HOWLING_FrameMove
	@brief : û 1�� ����, ǻ��������� ���̾� �Ͽ︵ ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CFG_IRON_HOWLING_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.08f );

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_IRON_HOWLING_EventProcess
	@brief : û 1�� ����, ǻ��������� ���̾� �Ͽ︵ ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_IRON_HOWLING_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 4.43f;

	IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )

	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_READY_Init
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� Init���� ���ҽ��� �ε�
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_READY_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	XSkinMeshReadyInBackground( L"CHUNG_GIGANTIC_IMPACT07.X" );
	XSkinMeshReadyInBackground( L"CHUNG_GIGANTIC_IMPACT06.X" );
	XSkinMeshReadyInBackground( L"LUNATIC_FURY_Broken.X" );
	XSkinMeshReadyInBackground( L"LUNATIC_FURY_FireWave.X" );
	XSkinMeshReadyInBackground( L"LUNATIC_FURY_FireWave02.X" );
	XSkinMeshReadyInBackground( L"LUNATIC_FURY_FireWave01.X" );
	XSkinMeshReadyInBackground( L"LUNATIC_FURY_FireWave02.X" );

	XMeshReadyInBackground( L"Up_Light01_Mesh.Y" );

	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ColorBallGray.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_READY_FrameMove
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_READY_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_READY_EventProcess
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_READY_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_SA_CFG_LUNATIC_FURY_RUN );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_RUN_FrameMove
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_RUN_FrameMove()
{
	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
		m_DamageData.hitUnitList.resize(0);
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_RUN_EventProcess
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_RUN_EventProcess()
{
	const int MAX_ANIMATION_COUNT_LUNATIC_FURY_RUN = 1;

	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� ����������
		if ( MAX_ANIMATION_COUNT_LUNATIC_FURY_RUN <= m_pXSkinAnimFuture->GetNowPlayCount() )
			StateChange( CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_FrameMove
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_FrameMove()
{
	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_EventProcess
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_FrameMove
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_FrameMove()
{

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ 1�� �̻� ������
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_CFG_LUNATIC_FURY_BOMBING", this );
				UseCannon( 1 );
			}
		}
	}


	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
		m_DamageData.hitUnitList.resize(0);
	}


	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_EventProcess
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_EventProcess()
{
	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ĳ������ 1�� ����������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			 ( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
		{
			// ������ ���
			if ( 0.0f < GetRemainHyperModeTime() )
			{
				StateChange( CSI_SI_SA_CFG_LUNATIC_FURY_END_HYPER );
			} // if
			else
			{
				StateChange( CSI_SI_SA_CFG_LUNATIC_FURY_END );
			}
		}
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_END_FrameMove
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			float fPowerRate = 1.f;
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO20 ) == true && GetRandomFloat() <= 0.2f )
				fPowerRate = 2.f;
			if ( true == IsHyperState() )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_CFG_LUNATIC_FURY_END_HYPER", this, NULL, true, fPowerRate );
			} // if
			else
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_CFG_LUNATIC_FURY_END", this, NULL, false, fPowerRate );
			}

			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CFG_LUNATIC_FURY_END_EventProcess
	@brief : û 1�� ����, ǻ��������� �糪ƽ ǻ�� ��ų�� EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_SA_CFG_LUNATIC_FURY_END_EventProcess()
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

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_READY_Init
	@brief : û 1�� ����, ǻ��������� �������� ��ų Ready������  Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_READY_Init()
{

}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_READY_FrameMove
	@brief : û 1�� ����, ǻ��������� �������� ��ų Ready������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_READY_FrameMove()
{
	if ( 0.57f < m_pXSkinAnim->GetNowAnimationTime() )
	{
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	}

	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_READY_EventProcess
	@brief : û 1�� ����, ǻ��������� �������� ��ų Ready������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_READY_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_CFG_LEAP_ATTACK_JUMP );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_JUMP_Start
	@brief : û 1�� ����, ǻ��������� �������� ��ų Jump���¿��� ĳ���� outline ���������� �������� ��
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_JUMP_Start()
{
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_JUMP_FrameMove
	@brief : û 1�� ����, ǻ��������� �������� ��ų Jump������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_JUMP_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
		{
			//m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_SECOND" );
			m_LuaManager.EndTable();
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			//m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_THIRD" );
			m_LuaManager.EndTable();
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.09f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			//m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_FOURTH" );
			m_LuaManager.EndTable();
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.12f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			//m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_FIFTH" );
			m_LuaManager.EndTable();
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_JUMP_EventProcess
	@brief : û 1�� ����, ǻ��������� �������� ��ų Jump������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_JUMP_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_Start
	@brief : û 1�� ����, ǻ��������� �������� ��ų JumpDown������ Start�� Outline ���������� �������� ��
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_Start()
{
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_FrameMove
	@brief : û 1�� ����, ǻ��������� �������� ��ų JumpDown������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_FrameMove()
{
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_EventProcess
	@brief : û 1�� ����, ǻ��������� �������� ��ų JumpDown������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_EventProcess()
{
	// �ִϸ��̼��� ������ ���θ� ���� ���� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		true == IsOnSomethingFuture() )
	{
		StateChange( CSI_SI_A_CFG_LEAP_ATTACK_LANDING );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_LANDING_Start
	@brief : û 1�� ����, ǻ��������� �������� ��ų Landing������ Start�� Outline�� ���������� �������� ��
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_LANDING_Start()
{
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_LANDING_FrameMove
	@brief : û 1�� ����, ǻ��������� �������� ��ų Landing������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_LANDING_FrameMove()
{
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_LEAP_ATTACK_LANDING_EventProcess
	@brief : û 1�� ����, ǻ��������� �������� ��ų Landing������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_LEAP_ATTACK_LANDING_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 0.4f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if
	// oasis907 : ����� [2011.6.14] û 2�� ���� - ���̾� �ȶ��
	else if ( true == m_InputData.oneZ && GetUnitClass() == CX2Unit::UC_CHUNG_IRON_PALADIN )
	{
		const float TRIGGER_TIME_COMBO_PZ	= 0.133f;

		if ( TRIGGER_TIME_COMBO_PZ < m_pXSkinAnimFuture->GetNowAnimationTime() )
			StateChange( CSI_CIP_COMBO_POWER_DOWN_LANDING_Z );
	}
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_GUARD_READY_Init
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Ready������ Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_READY_Init()
{

}


/** @function : CSI_SI_A_CFG_GUARD_READY_StartFuture
	@brief : û 1�� ����, ǻ��������� ���� ��ų, StartFuture���·� ��ų�� ������ ���Կ� �ش��ϴ� Ű�� �����͸� ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_READY_StartFuture()
{
	CommonStateStartFuture();

	// m_eSlotID_WildCharge�� ��� normal ������Ʈ�� �����̳Ŀ� ����, �����ϴ� Ű�� �������� �˾Ƴ�
	switch ( m_GuardForFuryGuardianPtr->GetSkillSlotID_Guard() )
	{
	case USI_SPECIAL_ATTACK_1:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureA );
		break;

	case USI_SPECIAL_ATTACK_2:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureS );
		break;

	case USI_SPECIAL_ATTACK_3:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureD );
		break;

	case USI_SPECIAL_ATTACK_4:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureC );
		break;
	case USI_SPECIAL_ATTACK_SLOTB_1:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureQ );
		break;

	case USI_SPECIAL_ATTACK_SLOTB_2:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureW );
		break;

	case USI_SPECIAL_ATTACK_SLOTB_3:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureE );
		break;

	case USI_SPECIAL_ATTACK_SLOTB_4:
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureR );
		break;

	default:
		ASSERT( !L"RSI_SA_RRF_WILD_CHARGE_READY_StateStartFuture" );
		m_GuardForFuryGuardianPtr->SetGuardKeyPointer( &m_InputData.pureA );
		break;
	}
}

/** @function : CSI_SI_A_CFG_GUARD_READY_FrameMove
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Ready������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_READY_FrameMove()
{
	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_GUARD_READY_EventProcess
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Ready������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_READY_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_CFG_GUARD_WAIT );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_GUARD_WAIT_Start
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Wait ������ Start�� ������ ������ ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_WAIT_Start()
{
	CommonStateStart();
	SetDamageReducePercent( GetDamageReducePercent() + m_GuardForFuryGuardianPtr->GetDamageReducePercent() );
}

/** @function : CSI_SI_A_CFG_GUARD_WAIT_FrameMove
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Wait ������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_WAIT_FrameMove()
{
	UpNowMp(-4.f * m_fElapsedTime);

	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_GUARD_WAIT_EventProcess
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Wait ������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_WAIT_EventProcess()
{
	int iCountToBeAbleToCheckKey = 1;	// ī��Ʈ�� 0 ���� ���� �ϹǷ� 2�� Loop�� �� �� ���� üũ ����

	// 2�� Loop �� �Ŀ�
	if ( iCountToBeAbleToCheckKey < m_pXSkinAnimFuture->GetNowPlayCount() )
	{
		// ��ų Ű���尡 Up �Ǿ��ų�, ��ų ������ ü���� �� ���
		if ( false == m_GuardForFuryGuardianPtr->GetGuardKeyValue()	)
		{
			StateChange( CSI_SI_A_CFG_GUARD_END );
		} // if
	} // if


	if ( static_cast<int>(GetNowMp()) <= 0 )

	{
		StateChange( CSI_SI_A_CFG_GUARD_END );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_GUARD_WAIT_End
	@brief : û 1�� ����, ǻ��������� ���� ��ų, Wait ������ End�� ������ ������ ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_WAIT_End()
{
	CommonStateEnd();
	SetDamageReducePercent( GetDamageReducePercent() - m_GuardForFuryGuardianPtr->GetDamageReducePercent() );
}

/** @function : CSI_SI_A_CFG_GUARD_END_FrameMove
	@brief : û 1�� ����, ǻ��������� ���� ��ų, End ������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_END_FrameMove()
{
	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_GUARD_END_EventProcess
	@brief : û 1�� ����, ǻ��������� ���� ��ų, End ������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_END_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 0.5f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_GUARD_END_End
	@brief : û 1�� ����, ǻ��������� ���� ��ų, End ������ End ������Ʈ
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_END_End()
{
	CommonStateEnd();
}

/** @function : CSI_SI_A_CFG_GUARD_DAMAGE_Start
	@brief : û 1�� ���� ǻ��������� ���� ��ų ��� �� �������� �޴� ������ Start�� ������ ������ ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_DAMAGE_Start()
{
	CommonStateStart();
	SetDamageReducePercent( GetDamageReducePercent() + m_GuardForFuryGuardianPtr->GetDamageReducePercent() );
}

/** @function : CSI_SI_A_CFG_GUARD_DAMAGE_FrameMove
	@brief : û 1�� ����, ǻ��������� ���� ��ų ����� �������� �޴� ������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_DAMAGE_FrameMove()
{
	UpNowMp(-4.f * m_fElapsedTime);

	CommonFrameMove();
}

/** @function : CSI_SI_A_CFG_GUARD_DAMAGE_EventProcess
	@brief : û 1�� ����, ǻ��������� ���� ��ų ����� �������� �޴� ������ EventProcess�� ���Ŀ� ����� ������Ʈ�� ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_DAMAGE_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_CFG_GUARD_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if
	else
	{
		if ( true == m_InputData.oneZ )
		{
			if( m_fWithStandingRate <= -1.f )
			{
				StateChange( CSI_GUARD_ATTACK_Z );
			}
			else if( m_fWithStandingRate >= GetRandomFloat(1) )
			{
				m_iSiegeShellingCount = 0;
				StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE );
			}
			else
			{
				StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT );
			}
		} // else if
		// X�� ��������
		else if ( true == m_InputData.oneX )
		{
			if( m_fWithStandingRate <= -1.f )
			{
				// ĳ������ ����� �� ������
				if ( CanUseCannon() )
					StateChange( CSI_GUARD_ATTACK_X );
				else
					StateChange( CSI_GUARD_ATTACK_Z );
			}
			else if( m_fWithStandingRate >= GetRandomFloat(1) )
			{
				m_iSiegeShellingCount = 0;
				StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE );
			}
			else
			{
				StateChange( CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT );
			}
		} // if
		SKILL_CANCEL_AFTER( 0.01f )
		// ��ų Ű���尡 Up �Ǿ��ų�, ��ų ������ ü���� �� ���
		else if ( false == m_GuardForFuryGuardianPtr->GetGuardKeyValue() )
		{
			StateChange( CSI_SI_A_CFG_GUARD_END );
		} // if
	}

	CommonEventProcess();
}

/** @function : CSI_SI_A_CFG_GUARD_DAMAGE_End
	@brief : û 1�� ���� ǻ��������� ���� ��ų ��� �� �������� �޴� ������ End�� ������ ������ ����
*/
void CX2GUChung::CSI_SI_A_CFG_GUARD_DAMAGE_End()
{
	CommonStateStart();
	SetDamageReducePercent( GetDamageReducePercent() - m_GuardForFuryGuardianPtr->GetDamageReducePercent() );
}

/** @function : CSI_SI_SA_CSG_DUAL_BUSTER_Init
	@brief : û 1�� ���� ���ð������ �������� ��ų �� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CSG_DUAL_BUSTER_Init()
{
	XSkinMeshReadyInBackground( L"CSG_Mini_Gun_DUAL_BUSTER.X" );
	XSkinMeshReadyInBackground( L"Mini_Gun_Shell_Dummy.X" );

	XMeshReadyInBackground( L"CSG_DUAL_BUSTER_Shell.Y" );

	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"IceHit_Impact01.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
}

/** @function : CSI_SI_SA_CSG_DUAL_BUSTER_FrameMove
	@brief : û 1�� ���� ���ð������ �������� ��ų �� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CSG_DUAL_BUSTER_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.69f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		if ( 0.0f < GetRemainHyperModeTime() )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_DUAL_BUSTER_Mini_Gun_Shell_HYPER", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), 0,
				false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
		} // if
		else
		{
			//{{ kimhc // 2011-03-29 // 2011-04 �� ��ġ�� û �޸� ��ų
			// ������ ���͸� �׼� �޸� �����ϰ� ������
			if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO4 ) )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_DUAL_BUSTER_Mini_Gun_Shell_MEMO", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), 0,
					false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			}
			else
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_DUAL_BUSTER_Mini_Gun_Shell", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), 0,
					false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			}
			//}} kimhc // 2011-03-29 // 2011-04 �� ��ġ�� û �޸� ��ų

		}
	}

    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.96f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		if ( 0.0f < GetRemainHyperModeTime() )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_DUAL_BUSTER_Mini_Gun_Shell_HYPER", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), 0,
				false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
		} // if
		else
		{
			//{{ kimhc // 2011-03-29 // 2011-04 �� ��ġ�� û �޸� ��ų
			// ������ ���͸� �׼� �޸� �����ϰ� ������
			if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO4 ) )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_DUAL_BUSTER_Mini_Gun_Shell_MEMO", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), 0,
					false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			}
			else
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_DUAL_BUSTER_Mini_Gun_Shell", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(), 0,
					false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			}
			//}} kimhc // 2011-03-29 // 2011-04 �� ��ġ�� û �޸� ��ų
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_DUAL_BUSTER_EventProcess
	@brief : û 1�� ���� ���ð������ �������� ��ų �� EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CSG_DUAL_BUSTER_EventProcess()
{

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == IsOnSomethingFuture())
		{
			StateChange( CSI_SI_SA_CSG_DUAL_BUSTER_LANDING );
		} // if
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_FrameMove
	@brief : û 1�� ���� ���ð������ �������� ��ų, Landing ������Ʈ�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_FrameMove()
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		//{{ kimhc // 2011-03-29 // 2011-04 �� ��ġ�� û �޸� ��ų
		// ������ ���͸� �׼� �޸� �����ϰ� ������
		if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO4 ) )
		{
			if( m_fCannonBallExtraChargeRate >= GetRandomFloat( 1 ) )
			{
				UpCannonBallCount( 2 );
			}
			else
			{
				UpCannonBallCount( 1 );
			}
			PlaySound( L"Chung_CannonGun_Reload.ogg" );
		}
		//}} kimhc // 2011-03-29 // 2011-04 �� ��ġ�� û �޸� ��ų
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_EventProcess
	@brief : û 1�� ���� ���ð������ �������� ��ų, Landing ������Ʈ�� EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 0.566f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if
	else if ( true == IsOnSomethingFuture() )
	{
		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_HEAVY_RAILGUN_Init
	@brief : û 1�� ���� ���ð������ ����ϰ� ��ų�� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CSG_HEAVY_RAILGUN_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	XSkinMeshReadyInBackground( L"CHUNG_SCARE_CHASE_Shell.X" );

	XMeshReadyInBackground( L"Maximumcanon_Mesh02.Y" );
	XMeshReadyInBackground( L"Ring_Plane.Y" );

	TextureReadyInBackground( L"Smoke.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Explosion_Fire01.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
}

/** @function : CSI_SI_SA_CSG_HEAVY_RAILGUN_FrameMove
	@brief : û 1�� ���� ���ð������ ����ϰ� ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CSG_HEAVY_RAILGUN_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );
	UseCannonAtThisAnimationTime( 0.04f, 1 );

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_HEAVY_RAILGUN_EventProcess
	@brief : û 1�� ���� ���ð������ ����ϰ� ��ų�� EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CSG_HEAVY_RAILGUN_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 1.399f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if
	else if ( true == IsOnSomethingFuture() )
	{
		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_READY_Init
	@brief : û 1�� ���� ���ð������ ���ý�Ÿ ��ų�� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_READY_Init()
{

}

/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_READY_FrameMove
	@brief : û 1�� ���� ���ð������ ���ý�Ÿ ��ų, READY ������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_READY_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );
	UseCannonAtThisAnimationTime( 0.04f, 3 );

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_READY_FrameMove
	@brief : û 1�� ���� ���ð������ ���ý�Ÿ ��ų, READY ���� �� EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_READY_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_Start
	@brief :
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_Start()
{
	PlaySound( L"ChungVoice_Shout09.ogg" );

	CommonStateStart();
}
/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_FrameMove
	@brief : û 1�� ���� ���ð������ ���ý�Ÿ ��ų, ATTACK ������ FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_FrameMove()
{

	ChangeWorldColorByHyperMode();

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		int iMaxPlayCount = 0;
		// ���� �ÿ���
		if ( 0.0f < GetRemainHyperModeTime() )
			iMaxPlayCount = 15;
		else
			iMaxPlayCount = 12;

		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( m_pXSkinAnim->GetNowPlayCount() < iMaxPlayCount )
		{
			D3DXVECTOR3 vWeaponBonePos	= GetWeaponBonePos( 0, L"TRACE_END0", 0 );
			D3DXVECTOR3 vDirVector		= GetDirVector();
			D3DXVECTOR3 vOffsetPos( 70.0f, 130.0f, 40.0f );

			if( 0.f != vDirVector.y )
			{
				vDirVector.y = 0.f;
				D3DXVec3Normalize( &vDirVector, &vDirVector );
			}

			if( true == GetIsRight() )
			{
				vWeaponBonePos += vDirVector * vOffsetPos.x;
			}
			else
			{
				vWeaponBonePos -= vDirVector * vOffsetPos.x;
			}

			vWeaponBonePos.y += vOffsetPos.y;

			if( true == GetIsRight() )
			{
				vWeaponBonePos += GetZVector() * vOffsetPos.z;
			}
			else
			{
				vWeaponBonePos += GetZVector() * vOffsetPos.z;
			}

			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CSG_SHOOTING_STAR", this );
			CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CSG_SHOOTING_STAR_SHELL", GetPowerRate(), vWeaponBonePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO23 ) == true && pDamageEffect != NULL )
			{
				CX2DamageEffect::DamageEffectDataInLua& DamageEffectData = pDamageEffect->GetDamageEffectDataInLua();
				if( DamageEffectData.m_vecDieDamageEffect.empty() == false )
					DamageEffectData.m_vecDieDamageEffect[0]->damageEffectName += L"_MEMO";
				if( DamageEffectData.m_vecHitDamageEffect.empty() == false )
					DamageEffectData.m_vecHitDamageEffect[0]->damageEffectName += L"_MEMO";
			}

			SetLockOn( pDamageEffect, m_pXSkinAnim->GetNowPlayCount(), CX2DamageEffect::LOT_UID_VECTOR, iMaxPlayCount );
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_EventProcess
	@brief : û 1�� ���� ���ð������ ���ý�Ÿ ��ų, ATTACK ������ EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_EventProcess()
{
	int iMaxPlayCount = 0;
	// ���� �ÿ���
	if ( 0.0f < GetRemainHyperModeTime() )
		iMaxPlayCount = 15;
	else
		iMaxPlayCount = 12;

	if ( m_pXSkinAnimFuture->GetNowPlayCount() >= iMaxPlayCount)
	{
		StateChange( CSI_SI_SA_CSG_SHOOTING_STAR_END );
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_SHOOTING_STAR_END_EventProcess
	@brief : û 1�� ���� ���ð������ ���ý�Ÿ ��ų, END ������ EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CSG_SHOOTING_STAR_END_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_RUMBLE_SHOT_Init
	@brief : û 1�� ���� ���ð������ ������ ��ų�� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_A_CSG_RUMBLE_SHOT_Init()
{
	XMeshReadyInBackground(L"Stone01_AirShip.y");
	TextureReadyInBackground(L"ColorBallBlue.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}

/** @function : CSI_SI_SA_CSG_RUMBLE_SHOT_FrameMove
	@brief : û 1�� ���� ���ð������ ������ ��ų�� FrameMove�� ����Ʈ ���
*/
void CX2GUChung::CSI_SI_A_CSG_RUMBLE_SHOT_FrameMove()
{
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO13 ) == true )
		UseCannonAtThisAnimationTime(0.01f, 2);
	else
		UseCannonAtThisAnimationTime(0.01f, 1);

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.24f ) )
	{
		D3DXVECTOR3 vBonePos = GetPos();//m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );//GetWeaponBonePos( 0, L"TRACE_START0", 0 );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO13 ) == true )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_CSI_SI_A_CSG_RUMBLE_SHOT_Shell_MEMO", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );
		}
		else
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_CSI_SI_A_CSG_RUMBLE_SHOT_Shell", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_RUMBLE_SHOT_EventProcess
	@brief : û 1�� ���� ���ð������ ������ ��ų�� EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_A_CSG_RUMBLE_SHOT_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 0.72f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )


	CommonEventProcess();
}
//}} kimhc // 2011.1.8 // û 1�� ����

/** @function : CSI_SI_A_CSG_HEAD_SHOT_Init
	@brief : û 1�� ���� ���ð������ ��弦 ��ų�� Init���� ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_A_CSG_HEAD_SHOT_Init()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_HEAD_SHOT.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}
/** @function : CSI_SI_A_CSG_HEAD_SHOT_FrameMove
	@brief : û 1�� ���� ���ð������ ��弦 ��ų�� ���ҽ� ��ũ
*/
void CX2GUChung::CSI_SI_A_CSG_HEAD_SHOT_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CX2DamageEffect::CEffect* pDamageEffect =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_HEAD_SHOT_SUBWEAPON_Mini_Gun", 1.f, vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pDamageEffect )
		{
			CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_MiniGun;
			hHandle_MiniGun = pDamageEffect->GetMainEffectHandle();
			if( INVALID_MESH_INSTANCE_HANDLE != hHandle_MiniGun )
			{
				SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_SI_A_CSG_HEAD_SHOT_MINI_GUN, hHandle_MiniGun );
			}
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CSG_HEAD_SHOT_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CSG_HEAD_SHOT_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CSG_HEAD_SHOT_MINI_GUN) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CSG_HEAD_SHOT_MINI_GUN));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Dummy01");
		}
	}

	//# �� 1�� �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.39f ) )
	{
		m_bIncreaseCritical = true;

		//{{ kimhc // 2011-03-30 // 2011-04 �� ��ġ�� û �޸� ��ų
		// û '��弦' �޸� ���� ���̸� EDT_HEAD_SHOT�� ����� ������ ����Ʈ ����
		if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO8 ) )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_A_CSG_HEAD_SHOT_SMI_CHUNG_MEMO8_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
			0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
		else
			//}} kimhc // 2011-03-30 // 2011-04 �� ��ġ�� û �޸� ��ų
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_A_CSG_HEAD_SHOT_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
			0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
	}

	CommonFrameMove();
}
/** @function : CSI_SI_A_CSG_HEAD_SHOT_EventProcess
	@brief : û 1�� ���� ���ð������ ��弦 ��ų�� EventProcess�� ���� ������Ʈ���� ������ ����
*/
void CX2GUChung::CSI_SI_A_CSG_HEAD_SHOT_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if

	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.933f;

	IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}

	CommonEventProcess();
}

//{{ kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_READY_Init
	@brief : ���ð����, 2�� ��ų ī��ũ������ Init �Լ���, �ʿ��� ���ҽ��� Ready �Ѵ�.
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_READY_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	XSkinMeshReadyInBackground( L"SHOOTING_STAR_Shell_Dummy.X" );
	XSkinMeshReadyInBackground( L"COMET_CRASHER_Explosion_01.X" );
	XSkinMeshReadyInBackground( L"COMET_CRASHER_Explosion_02.X" );
	XSkinMeshReadyInBackground( L"COMET_CRASHER_Explosion_03.X" );
	XSkinMeshReadyInBackground( L"Elsword_SI_SA_Phoenix_Talon_Mesh02.X" );

	TextureReadyInBackground( L"Steam_Bp.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
}

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_READY_FrameMove
	@brief : ���ð����, 2�� ī��ũ���� FrameMove �Լ���, ��ų ���� �� ���
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_READY_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_READY_EventProcess
	@brief : ���ð����, 2�� ī��ũ���� EventProcess �Լ��� ����� ������ LOOP ���·� �Ѿ
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_READY_EventProcess()
{
	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_SA_CSG_COMET_CRASHER_LOOP );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_LOOP_FrameMove
	@brief : ���ð����, 2�� ī��ũ����LOOP �� FrameMove �Լ��� EventTime�� ����Ʈ ���� ��� �ϰ�,
	�ִ� ���� ����(5��)�� ���� �ʵ��� ��
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_LOOP_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE > m_pXSkinAnim->GetNowPlayCount() )
		{
			// ĳ������ 1�� �̻� �ְų� ����ȭ ����
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CSG_COMET_CRASHER_LOOP", this );
				UseCannon( 1 );
			}
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}


	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_LOOP_EventProcess
	@brief : ���ð����, 2�� ī��ũ����LOOP �� EventProcess �Լ��� ���� FINISH ���·� �Ѿ
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_LOOP_EventProcess()
{
	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� �ۿ� ���� ���� ������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
			StateChange( CSI_SI_SA_CSG_COMET_CRASHER_FINISH );
		// ���� ���¸� �ٽ� �ѹ� �ݺ� �� �� ������
		else
        {
            ClearAnimEventTimerOneshotFuture();
        }
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_FINISH_FrameMove
	@brief : ���ð����, 2�� ī��ũ����FINISH�� FrameMove �Լ��� ������ EventTime�� ����Ʈ ���� �����
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_FINISH_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_SA_CSG_COMET_CRASHER_FINISH", this );
			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}

/** @function : CSI_SI_SA_CSG_COMET_CRASHER_FINISH_EventProcess
	@brief : ���ð����, 2�� ī��ũ����FINISH�� EventProcess �Լ��� ����� ����Ǹ� Wait�Ǵ� JumpDown ���·� ���� ��
*/
void CX2GUChung::CSI_SI_SA_CSG_COMET_CRASHER_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if

	CommonEventProcess();
}

/** @function :
	@brief :
*/
void CX2GUChung::CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_Init()
{
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	XSkinMeshReadyInBackground( L"CSG_Mini_Gun_SHARPSHOOTER_SYNDROME.X" );
	XMeshReadyInBackground( L"Up_Light01_Mesh.Y" );

	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"Arme_Critical2.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ACELDAMA_EXPLOSION.DDS" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"SHARPSHOOTER_SYNDROME.Tga" );
	TextureReadyInBackground( L"Condense_Pulse01.dds" );
}

void CX2GUChung::CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );

	// �հ����� ���׶� �͵��� ���̴� ����Ʈ ���
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Dummy2_Lhand" );
		vBonePos.y += 50.0f;

		CKTDGParticleSystem::CParticleEventSequence* pParticleEventSeq =
			g_pX2Game->GetMajorParticle()->CreateSequence( static_cast<CKTDGObject*>(this),
			L"CHUNG_SHARPSHOOTER_SYNDROME01", vBonePos );

		// ���� ���� ���
		pParticleEventSeq->SetBlackHolePosition( vBonePos );
	}

	//{{ kimhc // 2011.03.29 // 2011-04 �� ��ġ�� û ĳ�� ��ų �� ���� ��� ����
    if ( true == m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
	{
		const CX2SkillTree::SkillTemplet* pSharpShooterSyndromeTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CDC_SHARPSHOOTER_SYNDROME );
		if ( NULL != pSharpShooterSyndromeTemplet && !pSharpShooterSyndromeTemplet->m_vecBuffFactorPtr.empty() )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSharpShooterSyndromeTemplet->m_eID ) );	/// ��ų ����

			CX2BuffFactorPtr ptrBuffFactorClone
				= pSharpShooterSyndromeTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );
			if ( NULL != ptrBuffFactorClone )
			{
				ptrBuffFactorClone->SetGameUnitBuffFactor( this );
				for ( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
				{
					CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
					if( pGameUnit != NULL &&
						pGameUnit->GetTeam() == GetTeam() &&
						GetDistance3Sq( GetPos(), pGameUnit->GetPos() ) < 640000.0f /// �Ÿ� 800 �̳�
						)
						pGameUnit->SetBuffFactorToGameUnit( *ptrBuffFactorClone );
				}	// for
			}
		}
		//UseBuffSkillAcceptByDistance( pSharpShooterSyndromeTemplet, true, 800.0f );
	}
	//}} kimhc // 2011.03.29 // 2011-04 �� ��ġ�� û ĳ�� ��ų �� ���� ��� ����

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUChung::CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 3.6f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if
	else if ( LOWER_BOUND_INPUT_TIME_TO_CANCEL < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( CAN_WALK_CANCEL )
		{
			StateChange( USI_WALK );
		}
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
		{
			StateChange( USI_WALK );
		}
	} // if

	CommonEventProcess();
}

void CX2GUChung::CSI_SI_SA_CFG_PANDEMONIUM_Init()
{

}

void CX2GUChung::CSI_SI_SA_CFG_PANDEMONIUM_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );

    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletPandemonium =
			GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CFG_PANDEMONIUM_CHAOS );

		if ( NULL != pSkillTempletPandemonium && !pSkillTempletPandemonium->m_vecBuffFactorPtr.empty() )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01" );
			CX2DamageEffect::CEffect* pDamageEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CFG_PANDEMONIUM_DUMMY_ATTACK", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );

			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTempletPandemonium->m_eID ) );	/// ��ų ����

			if ( NULL != pDamageEffect )
				pDamageEffect->GetDamageData().PushBuffFactor( pSkillTempletPandemonium->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CFG_PANDEMONIUM_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if

	CommonEventProcess();
}

void CX2GUChung::CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_Init()
{

}

void CX2GUChung::CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_EventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_TO_CANCEL = 3.55f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if
	else if ( LOWER_BOUND_INPUT_TIME_TO_CANCEL < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( CAN_WALK_CANCEL )
		{
			StateChange( USI_WALK );
		}
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL )
		{
			StateChange( USI_WALK );
		}
	} // if

	CommonEventProcess();
}

//}} kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų

//{{ JHKang / ������ / 2011.6.14 / û 2�� ���� ���鸮 ü�̼�
/** @function 	: CDCComboXInit
	@brief 		: û 2�� ���� ComboX Init
	@brief		: �ǹ� ���͸� �̾Ƽ� 1ȸ �߻�
*/
void CX2GUChung::CDCComboXInit()
{
	XSkinMeshReadyInBackground(L"CDC_MiniGun_L_COMBO_X.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}

/** @function 	: CDCComboXFrameMove
	@brief 		: û 2�� ���� ComboX Frame Move
*/
void CX2GUChung::CDCComboXFrameMove()
{
	//{ �̴ϰ� �𵨸� ��ũ
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = NULL;
		pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_COMBO_X_Mini_Gun", vBonePos, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun )
		{
            pMeshInst_MiniGun->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L, pMeshInst_MiniGun->GetHandle() );
		}
	}
	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}
	//} �̴ϰ� �𵨸� ��ũ

	// �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Gun_L_Dummy03");
		}
	}
	// �̾� ġ��
    if ( m_pXSkinAnim->EventTimerOneshot( 0.466f ) )
	{
		D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01_L_Hand" );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CDC_COMBO_X_Mini_Gun_Hit", GetPowerRate(), vBonePos, GetRotateDegree(), GetRotateDegree() );
	}

	// �� �߻� 1��
    if( m_pXSkinAnim->EventTimerOneshot( 0.599f ) )
	{
		m_bIncreaseCritical = true;

		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell_Big", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );

		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun, 0.f, 0.f, fDegreeZ );
		}
	}

	CommonFrameMove();
}

/** @function 	: CDCComboXEventProcess
	@brief 		: û 2�� ���� ComboX Event Process
*/
void CX2GUChung::CDCComboXEventProcess()
{
	bool &bCanChangeStateComboXX = m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanChangeStateComboXZ = m_FrameDataFuture.stateParam.bEventFlagList[1];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_XX = 0.865f;	// CDC_CSF_COMBO_XFRONTXX�� �б���

		// �б� �ð� ����
		if ( TRIGGER_TIME_COMBO_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneX )
				bCanChangeStateComboXX = true;

			if ( true == m_InputData.oneZ )
				bCanChangeStateComboXZ = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXX )
				StateChange( CSI_CDC_COMBO_XX );	// ���� �޺��� ���� ����

			if ( true == bCanChangeStateComboXZ )
				StateChange( CSI_CDC_COMBO_XZ );	// ���� �޺��� ���� ����

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	}

	CommonEventProcess();
}

/** @function 	: CDCComboXEnd
	@brief 		: û 2�� ���� ComboX End
*/
void CX2GUChung::CDCComboXEnd()
{
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L, INVALID_MESH_INSTANCE_HANDLE);
	CommonStateEnd();
}

/** @function 	: CDCComboXXInit
	@brief 		: û 2�� ���� ComboXX EventProcess
*/
void CX2GUChung::CDCComboXXInit()
{
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust01.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ROT_NUCLEAR_Effect_Dust02.X" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
}

/** @function 	: CDCComboXXEventProcess
	@brief 		: û 2�� ���� ComboXX EventProcess
*/
void CX2GUChung::CDCComboXXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 1.232f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
	UpCannonBallCountAtThisAnimationTime( 1.232f, 1 );

	CommonFrameMove();
}

/** @function 	: CDCComboXXEventProcess
	@brief 		: û 2�� ���� ComboXX EventProcess
*/
void CX2GUChung::CDCComboXXEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboXXX	= m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool &bCanChangeStateComboXXZ	= m_FrameDataFuture.stateParam.bEventFlagList[1];

	// ������ �ٲ����� �޺� ���
	if( CAN_WALK_CANCEL )
	{
		bCanChangeStateComboXXX		= false;
		bCanChangeStateComboXXZ		= false;
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float LOWER_BOUND_INPUT_TIME	= 1.282f;	// �� �ð� ������ �Է��� �ȵ�
		const float TRIGGER_TIME_COMBO_XX	= 1.584f;		// Combo_XX�� �б���

		// �б� �ð� ����
		if ( ( LOWER_BOUND_INPUT_TIME <= m_pXSkinAnimFuture->GetNowAnimationTime() ) &&
			( TRIGGER_TIME_COMBO_XX >= m_pXSkinAnimFuture->GetNowAnimationTime() ) )
		{
			// Z Ű�� �����ٸ�
			if ( true == m_InputData.oneZ )
			{
				bCanChangeStateComboXXZ		= true;
				bCanChangeStateComboXXX		= false;
			}
			// X Ű�� �����ٸ�
			else if ( true == m_InputData.oneX )
			{
				bCanChangeStateComboXXZ		= false;
				bCanChangeStateComboXXX		= true;
			}
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else if ( TRIGGER_TIME_COMBO_XX < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// CSI_COMBO_XXZ �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXXZ )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_XXZ );
			}
			// CSI_COMBO_XXX �޺� Ű�� ���Ⱦ�����
			else if ( true == bCanChangeStateComboXXX )
			{
				// ���� �޺��� ���� ����
				StateChange( CSI_COMBO_XXX );
			}
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function 	: CDCComboXZInit
	@brief 		: û 2�� ���� ComboXZ EventProcess
*/
void CX2GUChung::CDCComboXZInit()
{
	XSkinMeshReadyInBackground(L"CDC_MiniGun_L_COMBO_XZ.X");
	XSkinMeshReadyInBackground(L"CDC_MiniGun_R_COMBO_XZ.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}

/** @function 	: CDCComboXZFrameMove
	@brief 		: û 2�� ���� ComboXZ Frame Move
*/
void CX2GUChung::CDCComboXZFrameMove()
{
	//{ �̴ϰ� �𵨸� ��ũ
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePosL = GetBonePos( L"Bip01_L_Hand" );
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_R_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = NULL;
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = NULL;

		pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_COMBO_XZ_Mini_Gun_L", vBonePosL, GetRotateDegree(), GetRotateDegree() );

		pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_COMBO_XZ_Mini_Gun_R", vBonePosR, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun_L )
		{
            pMeshInst_MiniGun_L->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L, pMeshInst_MiniGun_L->GetHandle() );
		}

		if( NULL != pMeshInst_MiniGun_R )
		{
            pMeshInst_MiniGun_R->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R, pMeshInst_MiniGun_R->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun_L )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun_L->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun_R )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Hand" );
			pMeshInst_MiniGun_R->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}
	//} �̴ϰ� �𵨸� ��ũ

	// �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun_L;
	D3DXVECTOR3 vBonePos_MiniGun_R;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun_L )
		{
			vBonePos_MiniGun_L = pMeshInst_MiniGun_L->GetXSkinAnim()->GetCloneFramePosition(L"Gun_L_Dummy03");
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun_R )
		{
			vBonePos_MiniGun_R = pMeshInst_MiniGun_R->GetXSkinAnim()->GetCloneFramePosition(L"Gun_R_Dummy03");
		}
	}

	// �� �߻� 1��
    if( m_pXSkinAnim->EventTimerOneshot( 0.632f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun_L, GetRotateDegree(), GetRotateDegree(),
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun_L, 0.f, 0.f, fDegreeZ );
		}
	}
	// �� �߻� 2��
    if( m_pXSkinAnim->EventTimerOneshot( 0.965f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun_R, GetRotateDegree(), GetRotateDegree(), 0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun_R, 0.f, 0.f, fDegreeZ );
		}
	}
	// �� �߻� 3��
    if( m_pXSkinAnim->EventTimerOneshot( 1.332f ) )
	{
		if( FlushMp( 4.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_XFRONTX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun_L, GetRotateDegree(), GetRotateDegree(),
															0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			float fDegreeZ = GetRotateDegree().z;
			CreateNotEnoughMPEffect( vBonePos_MiniGun_L, 0.f, 0.f, fDegreeZ );
		}
	}

	CommonFrameMove();
}

/** @function 	: CDCComboXZEventProcess
	@brief 		: û 2�� ���� ComboXZ Event Process
*/
void CX2GUChung::CDCComboXZEventProcess()
{
	bool &bCanChangeStateComboXZZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float INPUT_TIME_COMBO_XZZ = 1.3f;		// CDC_COMBO_ZZXXX �Է� �ð�
		const float TRIGGER_TIME_COMBO_XZZ = 1.458f;	// CDC_CSF_COMBO_XFRONTXX�� �б���

		// �Է� �ð� �Ŀ�
		if ( INPUT_TIME_COMBO_XZZ <= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneZ )
				bCanChangeStateComboXZZ = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		if ( TRIGGER_TIME_COMBO_XZZ <= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboXZZ )
				StateChange( CSI_CDC_COMBO_XZZ );	// ���� �޺��� ���� ����

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		const float LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 = 1.495f;
		const float LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 = 1.074f;

		/// 1.495f �� ���� ���͸� Walk Cancel, Ű �Է� ĵ�� �����ϴ� ��?? ���� �̻��ϴ�... �ϴ� ���� �ǹ̴�� ����
		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_01 )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_TO_CANCEL_02 )
		{
			StateChange( USI_WALK );
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	}

	CommonEventProcess();
}

/** @function 	: CDCComboXZEnd
	@brief 		: û 2�� ���� ComboXZ End
*/
void CX2GUChung::CDCComboXZEnd()
{
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L, INVALID_MESH_INSTANCE_HANDLE);
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R, INVALID_MESH_INSTANCE_HANDLE);
	CommonStateEnd();
}

/** @function 	: CDCComboXZZInit
	@brief 		: û 2�� ���� CDCComboXZZ Init
*/
void CX2GUChung::CDCComboXZZInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}

/** @function 	: CDCComboXZZFrameMove
	@brief 		: û 2�� ���� CDCComboXZZ FrameMove, ĳ���� �Ҹ�
*/
void CX2GUChung::CDCComboXZZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			// ����Ʈ ���
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CDC_COMBO_XZZ", this );
			UseCannon( 1 );
			ShowOnomatopoeiaParticle( L"TRACE_START0", L"STR_Kwang" );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}
	}

	CommonFrameMove();
}

/** @function 	: CDCComboXZZEventProcess
	@brief 		: û 2�� ���� ComboXZZ EventProcess
*/
void CX2GUChung::CDCComboXZZEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		float LOWER_BOUND_INPUT_TIME_SKILL_CANCEL = 0.9f;
		float LOWER_BOUND_INPUT_TIME_CANCEL = 1.440f;

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_SKILL_CANCEL )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WAIT );
		}
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
	}

	CommonEventProcess();
}

/** @function 	: CDCComboZZXEventProcess
	@brief 		: û 2�� ���� ComboZZX EventProcess
	@brief		: ���� �÷� �ļ� ����
*/
void CX2GUChung::CDCComboZZXEventProcess()
{
	bool &bCanChangeStateCDCComboZZXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float TRIGGER_TIME_COMBO_ZZXX = 0.521f;	// �б���

		// �б� �ð� ����
		if ( TRIGGER_TIME_COMBO_ZZXX >= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneX )
				bCanChangeStateCDCComboZZXX = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		else
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateCDCComboZZXX )
				StateChange( CSI_CDC_COMBO_ZZXX );	// ���� �޺��� ���� ����

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}
	}

	// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}

	CommonEventProcess();
}

/** @function 	: CDCComboZZXXInit
	@brief 		: û 2�� ���� ComboZZXX Init
*/
void CX2GUChung::CDCComboZZXXInit()
{
	XSkinMeshReadyInBackground(L"CDC_MiniGun_L_COMBO_ZZXX.X");
	XSkinMeshReadyInBackground(L"CDC_MiniGun_R_COMBO_ZZXX.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}

/** @function 	: CDCComboZZXXFrameMove
	@brief 		: û 2�� ���� ComboZZXX FrameMove
*/
void CX2GUChung::CDCComboZZXXFrameMove()
{
	//{ �̴ϰ� �𵨸� ��ũ
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePosL = GetBonePos( L"Bip01_L_Hand" );
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_R_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = NULL;
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = NULL;

		pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_COMBO_ZZXX_Mini_Gun_L", vBonePosL, GetRotateDegree(), GetRotateDegree() );

		pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_COMBO_ZZXX_Mini_Gun_R", vBonePosR, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun_L )
		{
            pMeshInst_MiniGun_L->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L, pMeshInst_MiniGun_L->GetHandle() );
		}

		if( NULL != pMeshInst_MiniGun_R )
		{
            pMeshInst_MiniGun_R->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R, pMeshInst_MiniGun_R->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun_L )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun_L->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun_R )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Hand" );
			pMeshInst_MiniGun_R->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}
	//}} �̴ϰ� �𵨸� ��ũ

	//{{ �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun_L;
	D3DXVECTOR3 vBonePos_MiniGun_R;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun_L )
		{
			vBonePos_MiniGun_L = pMeshInst_MiniGun_L->GetXSkinAnim()->GetCloneFramePosition(L"Gun_L_Dummy03");
		}
	}

	D3DXVECTOR3 vDegree_MiniGun_Shell = GetRotateDegree();
	vDegree_MiniGun_Shell.z = 23.f;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun_R )
		{
			vBonePos_MiniGun_R = pMeshInst_MiniGun_R->GetXSkinAnim()->GetCloneFramePosition(L"Gun_R_Dummy03");
		}
	}
	//}}

	//{{ ���� 9�� + Special 1��
	for ( float fcurrTime = 0.6f; fcurrTime <= 1.62f; fcurrTime += 0.25f )
	{
    if( m_pXSkinAnim->EventTimerOneshot( fcurrTime ) )
		{
			if( FlushMp( 2.0f ) == true )
			{

				CX2DamageEffect::CEffect* pEffect =
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_ZZXX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun_L, GetRotateDegree(), GetRotateDegree(), 0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
				SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
				PlaySound( L"Chung_MiniGun_Fire1.ogg" );
			}
			else
			{
				CreateNotEnoughMPEffect( vBonePos_MiniGun_L, 0.f, 0.f, vDegree_MiniGun_Shell.z );
			}
		}
	}

	for ( float fcurrTime = 0.67f; fcurrTime <= 1.43; fcurrTime += 0.25f )
	{
    if( m_pXSkinAnim->EventTimerOneshot( fcurrTime ) )
		{
			if( FlushMp( 2.0f ) == true )
			{
				CX2DamageEffect::CEffect* pEffect =
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_ZZXX_FIRE_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun_R, GetRotateDegree(), GetRotateDegree(), 0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
				SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
				PlaySound( L"Chung_MiniGun_Fire1.ogg" );
			}
			else
			{
				CreateNotEnoughMPEffect( vBonePos_MiniGun_R, 0.f, 0.f, vDegree_MiniGun_Shell.z );
			}
		}
	}

    if ( m_pXSkinAnim->EventTimerOneshot( 1.67f ) )
	{
		if( FlushMp( 2.0f ) == true )
		{
			CX2DamageEffect::CEffect* pEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CSG_COMBO_ZZXX_FIRE_Mini_Gun_Last_Shell", GetPowerRate(), vBonePos_MiniGun_R, GetRotateDegree(), GetRotateDegree(), 0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			PlaySound( L"Chung_MiniGun_Fire1.ogg" );
		}
		else
		{
			CreateNotEnoughMPEffect( vBonePos_MiniGun_R, 0.f, 0.f, vDegree_MiniGun_Shell.z );
		}
	}
	//}}

	CommonFrameMove();
}

/** @function 	: CDCComboZZXXEventProcess
	@brief 		: û 2�� ���� ComboZZXX EventProcess
*/
void CX2GUChung::CDCComboZZXXEventProcess()
{
	bool &bCanChangeStateComboZZXXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float INPUT_TIME_COMBO_ZZXXX = 1.5f;		// CDC_COMBO_ZZXXX �Է� �ð�
		const float TRIGGER_TIME_COMBO_ZZXXX = 1.9f;	// CDC_COMBO_ZZXXX ���� ��ȯ �ð�

		// �б� �ð� ����
		if ( INPUT_TIME_COMBO_ZZXXX <= m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneX )
				bCanChangeStateComboZZXXX = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( TRIGGER_TIME_COMBO_ZZXXX <= m_pXSkinAnimFuture->GetNowAnimationTime() && true == bCanChangeStateComboZZXXX )
				StateChange( CSI_CDC_COMBO_ZZXXX );	// ���� �޺��� ���� ����

			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	}

	CommonEventProcess();
}

/** @function 	: CDCComboZZXXEnd
	@brief 		: û 2�� ���� CDCComboZZXX End
*/
void CX2GUChung::CDCComboZZXXEnd()
{
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L, INVALID_MESH_INSTANCE_HANDLE);
	SetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R, INVALID_MESH_INSTANCE_HANDLE);
	CommonStateEnd();
}

/** @function 	: CDCComboZZXXXInit
	@brief 		: û 2�� ���� CDCComboZZXXX Init
	@brief		: ������ ���� ������ �̻��� ���� ����
*/
void CX2GUChung::CDCComboZZXXXInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	XSkinMeshReadyInBackground( L"CHUNG_SCARE_CHASE_Shell.X" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	TextureReadyInBackground( L"Smoke.dds" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"ColorBallFire.dds" );
	TextureReadyInBackground( L"Explosion_Fire01.dds" );
	TextureReadyInBackground( L"Shell.tga" );
	TextureReadyInBackground( L"Explosion_Light_Ring01.tga" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
}

/** @function 	: CDCComboZZXXXFrameMove
	@brief 		: û 2�� ���� CDCComboZZXXX FrameMove
*/
void CX2GUChung::CDCComboZZXXXFrameMove()
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.594f ) )
	{
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CDC_CHUNG_COMBO_ZZXXX", this, NULL, false, GetPowerRate() );

			UseCannon( 1 );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}
	}

	CommonFrameMove();
}

/** @function 	: CDCComboZZXXXEventProcess
	@brief 		: û 2�� ���� CDCComboZZXXX EventProcess
*/
void CX2GUChung::CDCComboZZXXXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		float LOWER_BOUND_INPUT_TIME_SKILL_CANCEL = 0.9f;
		float LOWER_BOUND_INPUT_TIME_CANCEL = 1.440f;

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
		SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_SKILL_CANCEL )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}
//}}

/** @function	: CSI_SI_A_CDC_LEG_SHOT_Init
	@brief		: û 2�� ���� ���鸮 ü�̼� ���� �� Init
*/
void CX2GUChung::CSI_SI_A_CDC_LEG_SHOT_Init()
{
	XSkinMeshReadyInBackground(L"SI_A_CDC_MiniGun_R_LEG_SHOT.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XSkinMeshReadyInBackground(L"CDC_LEG_SHOT_GUNSMOKE.X");
	XSkinMeshReadyInBackground(L"CDC_LEG_SHOT_GUNFIRE.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"ColorBallBlue.dds");
	TextureReadyInBackground(L"stone.dds");
	TextureReadyInBackground(L"Smoke.dds");
}
/** @function 	:
	@brief 		:
*/
void CX2GUChung::CSI_SI_A_CDC_LEG_SHOT_Start()
{
	CommonStateStart();
}
/** @function	: CSI_SI_A_CDC_LEG_SHOT_FrameMove
	@brief		: û 2�� ���� ���鸮 ü�̼� ���� �� ��ų�� ���ҽ� ��ũ
*/
void CX2GUChung::CSI_SI_A_CDC_LEG_SHOT_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		m_bIncreaseCritical = true;

		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_R_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = NULL;

		pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_SI_A_CDC_Mini_Gun_LEG_SHOT_Mini_Gun", vBonePosR, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun_R )
		{
            pMeshInst_MiniGun_R->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R, pMeshInst_MiniGun_R->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Gun_R_Dummy03");
		}
	}

	// �̾� ġ��
    if ( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_R_Hand" );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CDC_COMBO_X_Mini_Gun_Hit", 1.f, vBonePosR, GetRotateDegree(), GetRotateDegree() );
	}

	//# ���� �� �߻� ����Ʈ
    if( m_pXSkinAnim->EventTimerOneshot( 0.27f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_GUNFIRE", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
			0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );
	}
	//# �� 1�� �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.299f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_CDC_LEG_SHOT);
		if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
		{
			CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_A_CDC_LEG_SHOT_Mini_Gun_Shell", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
				0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );

			if ( NULL != pDE )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				pDE->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
			}
		}
	}


    if( m_pXSkinAnim->EventTimerOneshot( 0.94f ) )
	{
		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
			DeleteMajorMesh();
	}




	CommonFrameMove();
}
/** @function	: CSI_SI_A_CDC_LEG_SHOT_EventProcess
	@brief		: û 2�� ���� ���鸮 ü�̼� ���� �� EventProcess
*/
void CX2GUChung::CSI_SI_A_CDC_LEG_SHOT_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
			DeleteMajorMesh();
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if

	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.833f;

	if (  LOWER_BOUND_INPUT_TIME_CANCEL <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		else if ( CAN_WALK_CANCEL )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
				DeleteMajorMesh();
			StateChange( USI_WALK );
		}
		else if ( CAN_DASH_CANCEL )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
				DeleteMajorMesh();
			StateChangeDashIfPossible();
		}
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
				DeleteMajorMesh();
			StateChange( USI_WALK );
		}
	}

	CommonEventProcess();
}

/** @function	: CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Init
	@brief		: û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ Init
	@biref		: ���� ����Ʈ ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Init()
{
	XSkinMeshReadyInBackground(L"CDC_AS_ROCKON_DUMMY.x");
	XSkinMeshReadyInBackground(L"CDC_AS_SHELL.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_GUNFIRE.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_LIGHT.x");
	XSkinMeshReadyInBackground(L"CDC_AS_WIND.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND01.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND02.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND03.x");
	XMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT_Bottom_Broken.Y");
	TextureReadyInBackground(L"CDC_AS_ROCKON_CENTER.tga");
	TextureReadyInBackground(L"CDC_AS_ROCKON_ROUND1.tga");
	TextureReadyInBackground(L"CDC_AS_ROCKON_ROUND2.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"CDC_AS_ROCKON_ROUND3.tga");
	TextureReadyInBackground(L"steam_BP.dds");
	TextureReadyInBackground(L"GroundShockWave.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"smoke02.dds");
	TextureReadyInBackground(L"Dark_Smoke.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Rush_Impact02.tga");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"ColorBallBlue.dds");
	TextureReadyInBackground(L"stone.dds");
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Start
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ �غ� Start
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Start()
{
	CommonStateStart();
	if ( true == m_bArtilleryStrikeFire )
		DoArtilleryStrikeFire ();		// ��ƿ���� ��Ʈ����ũ Ư�� �� ���� ���� ��Ÿ���� �帣��
										// ��Ÿ�� ���� ������ 2���� ���� ���� �ٷ� ������ �������� ��
										// ������ ���� �� �����Ƿ� �̸� �����س����� ������ �ٷ� �߻��ϰ� �ʱ�ȭ
	SetArtilleryStrikeData ( CAST_MISSILE_SHOWER );
	m_bIsSummoningShot = true;
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_FrameMove
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ �غ� FrameMove
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_FrameMove()
{
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );

    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		bool bIsRight = GetIsRight();
		D3DXVECTOR3 vPos = GetPos();

		const float fOffsetPosX	= 120.f;
		if ( bIsRight )
			vPos += GetDirVector() * fOffsetPosX;
		else
			vPos -= GetDirVector() * fOffsetPosX;
		vPos.y += 300.f;
		// ���� Targeting NPC �� �������� �� �����Ƿ� ��� ����
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG, 0.f );
		SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER, 0.f );

		if( true == g_pX2Game->IsHost() )
		{
			int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;

			D3DXVECTOR3 vLandPos = vPos;
			if( false == g_pX2Game->GetWorld()->GetLineMap()->CanDown( vPos, LINE_RADIUS, &vLandPos, &iNewLineIndex ) )
			{
				vPos = GetPos();
				vPos.y += 240.f;
			}
			// ��ƿ���� ��Ʈ����ũ ���� �߸���Ÿ �� ��쿡 ���� ���� ó�� �߰�
			if ( m_eArtilleryStrikeType == CAST_QUANTUM_BALLISTA )
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER, 0, true, vPos, bIsRight, 0.3f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			else if ( m_eArtilleryStrikeType == CAST_MISSILE_SHOWER )
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_TARGETING_CHUNG, 0, true, vPos, bIsRight, 0.3f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
		}

		m_fSummonedLifeTime = 10.f;
		m_timerSummoned.restart();
	}

	CommonFrameMove();
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ �غ� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess()
{
	if ( 0.6f <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		const float MAGIC_TIME = 10.f;

		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

		if( NULL != pNPC )
		{
			//{{ ī�޶� ���� �ٴϱ�
			pNPC->SetFocusCamera( true );
			pNPC->SetFocusCameraForce( true );
			pNPC->SetApplyCinematicUI( false );
			//}}
			StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE );
			m_TimerShotControl.restart();
		}
		else if ( m_FrameDataFuture.unitCondition.fStateTime > MAGIC_TIME )
		{
			StateChange( USI_WAIT );
		}
	}
	m_InputData.Init();

	CommonEventProcess();
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_StateEnd
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ StateEnd
			 ī�޶� �ʱ�ȭ�� ���� ���� �Լ�
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd()
{
	if( true == IsMyUnit())
		g_pX2Game->InitFocusUnit();
	CommonStateEnd();
}
void CX2GUChung::ArtilleryStrikeControl_EventProcess()
{
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	bool bSpecialAttackKeyPressed = true;
	switch( m_eSpecialAttackKeyPressed )
	{
	case SAKP_A:
		{
			if( m_InputData.pureA == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;

	case SAKP_S:
		{
			if( m_InputData.pureS == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;

	case SAKP_D:
		{
			if( m_InputData.pureD == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;

	case SAKP_C:
		{
			if( m_InputData.pureC == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;
	case SAKP_Q:
		{
			if( m_InputData.pureQ == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;
	case SAKP_W:
		{
			if( m_InputData.pureW == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;
	case SAKP_E:
		{
			if( m_InputData.pureE == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;
	case SAKP_R:
		{
			if( m_InputData.pureR == false )
			{
				bSpecialAttackKeyPressed = false;
			}
		} break;
	default:
		{
			bSpecialAttackKeyPressed = false;
		} break;
	}

	const float MAGIC_CONTROL_RADIUS_X = -750.f;
	const float MAGIC_CONTROL_RADIUS_Y = 1500.f;
	const double MAGIC_CONTROL_TIME = 10.0;

	if( true == bSpecialAttackKeyPressed &&
		m_TimerShotControl.elapsed() < MAGIC_CONTROL_TIME )
	{
		D3DXVECTOR3 vDirection(0,0,0);
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

		if( NULL != pNPC )
		{
			D3DXVECTOR3 vPosDir1( GetPos().x * GetDirVector().x,
				GetPos().y * GetDirVector().y,
				GetPos().z * GetDirVector().z );
			D3DXVECTOR3 vPosDir2( pNPC->GetPos().x * GetDirVector().x,
				pNPC->GetPos().y * GetDirVector().y,
				pNPC->GetPos().z * GetDirVector().z );
			if( GetDistance( vPosDir1, vPosDir2 ) <= MAGIC_CONTROL_RADIUS_X )
			{
				vDirection = vPosDir1 - vPosDir2;

				vDirection.x += vDirection.z;
			}

			if ( pNPC->GetPos().y >= MAGIC_CONTROL_RADIUS_Y + GetPos().y )
			{
				vDirection.y -= vDirection.y - MAGIC_CONTROL_RADIUS_Y;
			}
		}

		if( vDirection.x >= 0.f && vDirection.y <= 0.f &&
			true == m_InputData.pureUp && true == m_InputData.pureLeft )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT );
		}
		else if( vDirection.x <= 0.f && vDirection.y <= 0.f &&
			true == m_InputData.pureUp && true == m_InputData.pureRight )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT );
		}
		else if( vDirection.x >= 0.f && vDirection.y >= 0.f &&
			true == m_InputData.pureDown && true == m_InputData.pureLeft )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT );
		}
		else if( vDirection.x <= 0.f && vDirection.y >= 0.f &&
			true == m_InputData.pureDown && true == m_InputData.pureRight )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT );
		}
		else if( vDirection.x >= 0.f && true == m_InputData.pureLeft )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT );
		}
		else if( vDirection.x <= 0.f && true == m_InputData.pureRight )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT );
		}
		else if( vDirection.y <= 0.f && true == m_InputData.pureUp )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP );
		}
		else if( vDirection.y >= 0.f && true == m_InputData.pureDown )
		{
			if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN );
		}
		else if( GetNowStateID() != CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE )
		{
			StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE );
		}
	}
	else
	{
		D3DXVECTOR3 vDirection(0,0,0);
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

		if( NULL != pNPC )
		{
			D3DXVECTOR3 vPosDir1( GetPos().x * GetDirVector().x,
				GetPos().y * GetDirVector().y,
				GetPos().z * GetDirVector().z );
			D3DXVECTOR3 vPosDir2( pNPC->GetPos().x * GetDirVector().x,
				pNPC->GetPos().y * GetDirVector().y,
				pNPC->GetPos().z * GetDirVector().z );
			if( GetDistance( vPosDir1, vPosDir2 ) <= MAGIC_CONTROL_RADIUS_X )
			{
				vDirection = vPosDir1 - vPosDir2;
			}

			if ( GetDistance( vPosDir1, vPosDir2 ) >= MAGIC_CONTROL_RADIUS_X )
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE );
			else
				StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL );
		}
		else
		{
			StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_END );
		}
	}

	m_InputData.Init(); // ĳ���ʹ� ���� �� �� ���� UFO�� ������ �� �ִ�

}
/** @function 	: CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess
	@brief 		: û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess()
{
	ArtilleryStrikeControl_EventProcess();
	CommonEventProcess();
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_IDLE", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateEnd()
{
	CommonStateEnd();

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC && true == m_bIsSummoningShot )
	{
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_FIRE", true );
		m_vLastShotPos = pNPC->GetPos();
		m_vLastShotDir = pNPC->GetDirVector();

		StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE, true );

		m_bIsSummoningShot = false;
	}
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_LEFT", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_RIGHT", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_UP", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_DOWN", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_UP_LEFT", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_UP_RIGHT", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_DOWN_LEFT", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ��Ʈ��
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC )
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKE_DOWN_RIGHT", true );
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_Start
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ���� Start
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_Start()
{
	CommonStateStart();

	// ��ƿ���� ��Ʈ����ũ ���� �߸���Ÿ�� 6���� ���,
	// �̽� ������ 5���� ����Ѵ�.
	if ( m_eArtilleryStrikeType == CAST_QUANTUM_BALLISTA )
	{
		UseCannon( 6 );
	}
	else if ( m_eArtilleryStrikeType == CAST_MISSILE_SHOWER )
	{
		UseCannon( 5 );
	}
	/// skill Templet���� �޾ƾ���

	m_iArtilleryStrikePlayCount = 15;
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_FrameMove
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ���� FrameMove
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_FrameMove()
{
	ChangeWorldColorByHyperMode();
	// �̽� ������ �� ������ ����ź�� �߻�
	// ���� �߸���Ÿ�� �̻��� ��ü�� �������� �߻�
	// �ش� ����Ʈ ���� ������ ����Ʈ�� ����.
    if( m_pXSkinAnim->EventTimerOneshot( 0.04f ) )
	{
		switch ( m_eArtilleryStrikeType )
		{
			case CAST_MISSILE_SHOWER :
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EFFECTSET_ARTILLERY_STRIKE_FLARE", this );
				break;
			case CAST_QUANTUM_BALLISTA :
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_CTT_ARTILLERY_STRIKE_QUANTUM_BALLISTA", this );
				PlaySound( L"Chung_ArtilleryStrike_Shot.ogg" );
				break;
			default :
				break;

		}

		m_bArtilleryStrikeFire = true;					// ��ƿ���� ��Ʈ����ũ �߻�!
		m_fArtilleryStrikePowerRate	= GetPowerRate();	// PowerRate ���� ����
		m_TimerCheckArtilleryStrikeFire.restart();		// Ÿ�̸� ����ŸƮ

	}

	CommonFrameMove();
}
/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_EventProcess
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ���� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_EventProcess()
{
	// ��ƿ���� ��Ʈ����ũ�� �߻� �� ���� ������ �� �ְ� �Ǹ鼭
	// Loop �� ��ġ�� �ʰ� �ٷ� End �� ����.
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_SA_CDC_ARTILLERY_STRIKE_END );
	}

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_END_EventProcess
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ���� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_END_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_bIsSummoningShot = false;
		m_fCanNotAttackTime = 0.1f;
	} // if

	CommonEventProcess();
}

/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_Start
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ���� Start
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_Start()
{
	CommonStateStart();

	CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( GetShot() );

	if( NULL != pNPC && true == m_bIsSummoningShot )
	{
		pNPC->StateChange( "SHOT_ARTILLERY_STRIKER_CANCEL", true );

		const CX2UserSkillTree::SkillSlotData* pSkillSlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillSlot( CX2SkillTree::SI_SA_CTT_ARTILLERY_STRIKE );
		if ( NULL != pSkillSlotData )
		{
			float fMPConsume = pSkillSlotData->m_fMPConsumption;
			if( IsFullHyperMode() && 0.f < m_fDecreaseRate )
				UpNowMp( (fMPConsume * m_fDecreaseRate) * 0.5f );
			else
				UpNowMp( fMPConsume * 0.5f );

			if( NULL != g_pData->GetPicCharBlue() )
			{
				WCHAR wszText[64] = L"";
				StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MP+ %d", (int) (fMPConsume * 0.5f) );

				D3DXVECTOR3 pos = GetPos();
				pos.y += 50.f;

				g_pData->GetPicCharBlue()->DrawText( wszText, pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}

			m_bIsSummoningShot = false;
		}
	}
}

/** @function : CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_EventProcess
	@brief : û 2�� ���� ���鸮 ü�̼� ��ƿ���� ��Ʈ����ũ ���� EventProcess
*/
void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if

	CommonEventProcess();
}


/** @function 	: CSI_SI_SA_CDC_BULLET_BLITZ_Init
	@brief 		: ���� ������ ���ҽ� �ε�
*/
void CX2GUChung::CSI_SI_SA_CDC_BULLET_BLITZ_Init()
{
	XSkinMeshReadyInBackground(L"CDC_BULLET_BLITZ_GUNFIRE1.X");
	XSkinMeshReadyInBackground(L"CDC_BULLET_BLITZ_GUNFIRE2.X");
	XSkinMeshReadyInBackground(L"CDC_BULLET_BLITZ_WIND.X");
	XSkinMeshReadyInBackground(L"CDC_BULLET_BLITZ_Dummy.X");
	XSkinMeshReadyInBackground(L"SI_SA_CDC_MiniGun_R_BULLET_BLITZ.X");
	XSkinMeshReadyInBackground(L"Spin_Liner03.X");
	XMeshReadyInBackground(L"Ring_Plane.Y");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"Light_Impact01.dds");
	TextureReadyInBackground(L"ColorBallBlue2.dds");
	TextureReadyInBackground(L"steam_BP.dds");
	TextureReadyInBackground(L"Explosion_Sphere.dds");
}

/** @function 	: CSI_SI_SA_CDC_BULLET_BLITZ_StateStart
	@brief 		: �⺻ ����
*/
void CX2GUChung::CSI_SI_SA_CDC_BULLET_BLITZ_StateStart()
{
	CommonStateStart();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_CDC_BULLET_BLITZ);
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		// Ƚ��
		m_sBulletCount = static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel ) );

	}
}

/** @function 	: CSI_SI_SA_CDC_BULLET_BLITZ_FrameMove
	@brief 		: ���� ������ ����
*/
void CX2GUChung::CSI_SI_SA_CDC_BULLET_BLITZ_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

	//# ���� ������ �߻� ����Ʈ
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effect_CDC_Chung_Bullet_Blitz_Start", this );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = NULL;

		pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_MiniGun_L_BULLET_BLITZ", vBonePosR, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun_L )
		{
            pMeshInst_MiniGun_L->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L, pMeshInst_MiniGun_L->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Gun_L_Dummy03");
		}
	}

	// �̾� ġ��
    if ( m_pXSkinAnim->EventTimerOneshot( 0.598f ) )
	{
		D3DXVECTOR3 vBonePosL = GetBonePos( L"Bip01_L_Hand" );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CDC_COMBO_X_Mini_Gun_Hit", GetPowerRate(), vBonePosL, GetRotateDegree(), GetRotateDegree() );
	}

	//# �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.598f ) )
	{
 		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this,
 			L"Damage_Chung_CDC_Bullet_Blitz", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( NULL != pDamageEffect )
		{
			bool bUserUnit = false;
			UidType gameUnitUID = -1;

			pDamageEffect->SetDamageTime( m_sBulletCount );

			if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
				gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vBonePos_MiniGun, D3DXVECTOR2(150, 300), bUserUnit );
			else
				gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vBonePos_MiniGun, D3DXVECTOR2(150, 750), bUserUnit );

			if( bUserUnit == true )
			{
				pDamageEffect->SetLockOnUnitUID( gameUnitUID );
				pDamageEffect->SetLockOnRandomSeed( (int)GetRandomInt() );
			}
			else
			{
				pDamageEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
				pDamageEffect->SetLockOnRandomSeed( (int)GetRandomInt() );
			}
		}
	}

	CommonFrameMove();
}


/** @function 	: CSI_SI_SA_CDC_BULLET_BLITZ_StateStart
	@brief 		: �⺻ ���� ����
*/
void CX2GUChung::CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_StateStart()
{
	CommonStateStart();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_CDC_BULLET_BLITZ);
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		// Ƚ��
		m_sBulletCount = static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel ) );

		// ���� �� 5ȸ �߰�
		m_sBulletCount += 5;
	}
}

/** @function 	: CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_FrameMove
	@brief 		: ���� ������ ���� ����
*/
void CX2GUChung::CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

	//# ���� ������ �߻� ����Ʈ
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effect_CDC_Chung_Bullet_Blitz_Start", this );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_L_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_L = NULL;

		pMeshInst_MiniGun_L = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_CDC_MiniGun_L_BULLET_BLITZ", vBonePosR, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun_L )
		{
            pMeshInst_MiniGun_L->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L, pMeshInst_MiniGun_L->GetHandle() );
		}
	}

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_L_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
		}
	}

	//# �ѹ߻� ��ġ�� �޾ƿ��� ����
	D3DXVECTOR3 vBonePos_MiniGun;

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L));
		if( NULL != pMeshInst_MiniGun )
		{
			vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Gun_L_Dummy03");
		}
	}

	// �̾� ġ��
    if ( m_pXSkinAnim->EventTimerOneshot( 0.598f ) )
	{
		D3DXVECTOR3 vBonePosL = GetBonePos( L"Bip01_L_Hand" );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CDC_COMBO_X_Mini_Gun_Hit", GetPowerRate(), vBonePosL, GetRotateDegree(), GetRotateDegree() );
	}

	//# �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.598f ) )
	{
 		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this,
 			L"Damage_Chung_CDC_Bullet_Blitz_Hyper", GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( NULL != pDamageEffect )
		{
			bool bUserUnit = false;
			UidType gameUnitUID = -1;

			pDamageEffect->SetDamageTime( m_sBulletCount );

			if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
				gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vBonePos_MiniGun, D3DXVECTOR2(150, 300), bUserUnit );
			else
				gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vBonePos_MiniGun, D3DXVECTOR2(150, 750), bUserUnit );

			if( bUserUnit == true )
			{
				pDamageEffect->SetLockOnUnitUID( gameUnitUID );
				pDamageEffect->SetLockOnRandomSeed( (int)GetRandomInt() );
			}
			else
			{
				pDamageEffect->SetLockOnNPCUID( static_cast<int>( gameUnitUID ) );
				pDamageEffect->SetLockOnRandomSeed( (int)GetRandomInt() );
			}
		}
	}

	CommonFrameMove();
}

/** @function 	: CSI_SI_SA_CDC_BULLET_BLITZ_EventProcess
	@brief 		: ���� ������ �̺�Ʈ
*/
void CX2GUChung::CSI_SI_SA_CDC_BULLET_BLITZ_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if

	const float LOWER_BOUND_INPUT_TIME_CANCEL = 1.2f;

	IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}

	CommonEventProcess();
}

void CX2GUChung::CIPComboPowerDownLandingZInit()
{

	return;
}

void CX2GUChung::CIPComboPowerDownLandingZEventProcess()
{
	// �̺�Ʈ �÷��װ� ������ ��Ÿ������ �˱� ���� �ϵ���
	bool &bCanChangeStateComboPZZ = m_FrameDataFuture.stateParam.bEventFlagList[0];

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else
	{
		const float LOWER_BOUND_INPUT_TIME	= 0.200f;		// �� �ð� ���� �Է��� ���� ����
		const float TRIGGER_TIME_COMBO_PZZ	= 0.600f;		// Combo_PZZ�� �б���

		// �б� �ð� ����
		if ( ( LOWER_BOUND_INPUT_TIME <= m_pXSkinAnimFuture->GetNowAnimationTime() ) &&
			( TRIGGER_TIME_COMBO_PZZ >= m_pXSkinAnimFuture->GetNowAnimationTime() ) )
		{
			// �޺� Ű�� ������, �����ٴ� �����͸� ����
			if ( true == m_InputData.oneZ )
				bCanChangeStateComboPZZ = true;
		}
		// �б� �ð���, �Ǵ� �� �Ŀ�
		SKILL_CANCEL_AFTER( TRIGGER_TIME_COMBO_PZZ )
		WALK_CANCEL_AFTER( TRIGGER_TIME_COMBO_PZZ )
		DASH_CANCEL_AFTER( TRIGGER_TIME_COMBO_PZZ )
		else if ( TRIGGER_TIME_COMBO_PZZ < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			// �޺� Ű�� ���Ⱦ�����
			if ( true == bCanChangeStateComboPZZ )
			{

					StateChange( CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ );
			}
			// ������Ʈ�� �ٲ�� CX2GUUser�� CommonStateStartFuture�� CommonStateStart����
			// �÷��׸� �ʱ�ȭ ���ֹǷ� �÷��� �ʱ�ȭ ���� �ʿ� ����
		}

		// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
		if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}


	CommonEventProcess();
	return;
}
void CX2GUChung::CIPComboPowerDownLandingZEnd()
{
	CommonStateEnd();
	return;
}

// CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ
void CX2GUChung::CIPComboPowerDownLandingZZInit()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
	return;
}

void CX2GUChung::CIPComboPowerDownLandingZZEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else
	{
		const float LOWER_BOUND_INPUT_TIME	= 0.600f;

		if ( LOWER_BOUND_INPUT_TIME < m_pXSkinAnimFuture->GetNowAnimationTime() )
		{
			IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME )
			else if ( CAN_DASH_CANCEL )
			{
				StateChangeDashIfPossible();
			}
			else if ( CAN_WALK_CANCEL )
			{
				StateChange( USI_WALK );
			}
			ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
			{
				StateChange( USI_WAIT );
			}
		}
	}
	// �ƹ� �Է� ���� �׳� �ִϸ��̼��� ��������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	}
	CommonEventProcess();
	return;
}
void CX2GUChung::CIPComboPowerDownLandingZZEnd()
{
	CommonStateEnd();
	return;
}
///////////////////////////
// CSI_CIP_COMBO_DASH_XX //
void CX2GUChung::CIPComboDashXXInit()
{
	return;
}
void CX2GUChung::CIPComboDashXXFrameMove()
{
	CommonFrameMove();
	return;
}
void CX2GUChung::CIPComboDashXXEventProcess()
{
	bool& bCanChangeCIPComboDashXXX = m_FrameDataFuture.stateParam.bEventFlagList[0];

	const float	TRIGGER_TIME_CIP_COMBO_DASH_XX = 1.f;
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.8f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}

	}
	else if ( LOWER_BOUND_INPUT_TIME_CANCEL <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( CAN_WALK_CANCEL )
		{
			StateChange( USI_WALK );
		}
	}
	else if ( TRIGGER_TIME_CIP_COMBO_DASH_XX > m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( true == m_InputData.oneZ )
		{
			bCanChangeCIPComboDashXXX = true;
		}
	}
	if ( TRIGGER_TIME_CIP_COMBO_DASH_XX <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
		true == bCanChangeCIPComboDashXXX )
	{
		StateChange( CSI_CIP_COMBO_DASH_XXX );
	}

	CommonEventProcess();
	return;
}
void CX2GUChung::CIPComboDashXXEnd()
{
	CommonStateEnd();
	return;
}
////////////////////////////
// CSI_CIP_COMBO_DASH_XXX //
void CX2GUChung::CIPComboDashXXXInit()
{
	return;
}
void CX2GUChung::CIPComboDashXXXFrameMove()
{
	CommonFrameMove();
	return;
}
void CX2GUChung::CIPComboDashXXXEventProcess()
{
	bool& bCanChangeCIPComboDashXXXX = m_FrameDataFuture.stateParam.bEventFlagList[0];
	bool& bCanChangeCIPComboDashXXXZ = m_FrameDataFuture.stateParam.bEventFlagList[1];

	const float LOWER_BOUND_INPUT_TIME_COMBO_DASH_XXXX = 0.6f;
	const float	TRIGGER_TIME_CIP_COMBO_DASH_XXX = 0.87f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}
	else if ( LOWER_BOUND_INPUT_TIME_COMBO_DASH_XXXX < m_pXSkinAnimFuture->GetNowAnimationTime()
		&& TRIGGER_TIME_CIP_COMBO_DASH_XXX > m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		if ( true == m_InputData.oneX )
		{
			bCanChangeCIPComboDashXXXX = true;
		}
		else if( true == m_InputData.oneZ )
		{
			bCanChangeCIPComboDashXXXZ = true;
		}
	}
	else
	{
		if ( true == bCanChangeCIPComboDashXXXX )
		{
			StateChange( CSI_CIP_COMBO_DASH_XXXX );
		}
		else if ( true == bCanChangeCIPComboDashXXXZ )
		{
			StateChange( CSI_CIP_COMBO_DASH_XXXZ );
		}
	}

	CommonEventProcess();
	return;
}
void CX2GUChung::CIPComboDashXXXEnd()
{

	CommonStateEnd();
	return;
}
/////////////////////////////
// CSI_CIP_COMBO_DASH_XXXX //
void CX2GUChung::CIPComboDashXXXXInit()
{

}

void CX2GUChung::CIPComboDashXXXXEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 1.1333f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}

	CommonEventProcess();
	return;
}
void CX2GUChung::CIPComboDashXXXXEnd()
{

	CommonStateEnd();
	return;
}
/////////////////////////////
// CSI_CIP_COMBO_DASH_XXXZ //
void CX2GUChung::CIPComboDashXXXZInit()
{
	return;
}
void CX2GUChung::CIPComboDashXXXZFrameMove()
{

	CommonFrameMove();
	return;
}
void CX2GUChung::CIPComboDashXXXZEventProcess()
{
	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.633f;

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
			m_fCanNotAttackTime = 0.1f;
		}
	}
	SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
	{
		StateChange( USI_WALK );
	}

	CommonEventProcess();
	return;
}
void CX2GUChung::CIPComboDashXXXZEnd()
{

	CommonStateEnd();
	return;
}


/////////////////////////////////
// CSI_SI_A_CIP_SUDDEN_BURSTER //
void CX2GUChung::CSI_SI_A_CIP_SUDDEN_BURSTER_Init()
{
	TextureReadyInBackground( L"Sudden_Buster_P01.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P02.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P03.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P04.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P05.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P06.dds" );
}

void CX2GUChung::CSI_SI_A_CIP_SUDDEN_BURSTER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUChung::CSI_SI_A_CIP_SUDDEN_BURSTER_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.5000f ) )
	{
		if( CanUseCannon() )
		{
			UpDownCrashCamera( 20.0f, 1.0f );

			D3DXVECTOR3 vWeaponBonePos	= GetWeaponBonePos( 0, L"TRACE_START0", 0 );

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CIP_SUDDEN_BURST", GetPowerRate(), vWeaponBonePos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );

			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Sudden_Buster_P04", vWeaponBonePos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Sudden_Buster_P03", vWeaponBonePos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Sudden_Buster_P01", vWeaponBonePos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Sudden_Buster_P02", vWeaponBonePos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Sudden_Buster_P05", vWeaponBonePos );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Sudden_Buster_P06", vWeaponBonePos );

			UseCannon( 1 );
		}
	}
	CommonFrameMove();
}

void CX2GUChung::CSI_SI_A_CIP_SUDDEN_BURSTER_EventProcess()
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

//////////////////////////////
// CSI_SI_SA_CIP_BURST_WOLF //
void CX2GUChung::CSI_SI_SA_CIP_BURST_WOLF_Init()
{
	TextureReadyInBackground( L"Bust_Wolf_P01.dds" );
	TextureReadyInBackground( L"Bust_Wolf_P02.dds" );
	XSkinMeshReadyInBackground( L"Bust_Wolf_M01.X" );
	XSkinMeshReadyInBackground( L"Bust_Wolf_M02.X" );
	XSkinMeshReadyInBackground( L"Bust_Wolf_M03.X" );
}


void CX2GUChung::CSI_SI_SA_CIP_BURST_WOLF_StateStart()
{
	CommonStateStart();

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_CIP_BURST_WOLF);
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
		return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		m_iDebuffTransitNum = static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel ) );
	}


	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUChung::CSI_SI_SA_CIP_BURST_WOLF_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		FadeInWeaponRenderEffect(D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 0.f), 0.7f, 0.7f);
	}


    if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_BURST_WOLF_FIRST", this );
	}

else if( m_pXSkinAnim->EventTimerOneshot( 1.1f ) &&
		GetUnitCondition().bHit == true)
	{
		m_bDisableGravity = false;

		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );

		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 400.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -400.f;

		[[maybe_unused]] CX2DamageEffect::CEffect* pPEffectInDamage = NULL;

		pPEffectInDamage = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CIP_BURST_WOLF", GetPowerRate(), vTargetPos,
			GetRotateDegree(), GetRotateDegree(), vTargetPos.y, true);


		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 180.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -180.f;

		vTargetPos.y = vTargetPos.y + 100.f;

		CX2DamageEffect::CEffect *pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CIP_BURST_WOLF_DEBUFF", GetPowerRate(), vTargetPos,
			GetRotateDegree(), GetRotateDegree(), vTargetPos.y, true);

		if( pEffect != NULL )
		{
			CX2DamageManager::DamageData& damageData = pEffect->GetDamageData();
			//if( pDamageData != NULL )
			{
				damageData.m_ExtraDamage.m_Accumulation = m_iDebuffTransitNum;
			}
		}

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_BURST_WOLF_SECOND", this );
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CIP_BURST_WOLF_EventProcess()
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

void CX2GUChung::CSI_SI_SA_CIP_BURST_WOLF_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}



///////////////////////////////////
// CSI_SI_SA_CIP_LAND_DEMOLISHER //




void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_START_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = false;
	//m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0.f, 1650.f );
	//m_PhysicParam.passiveSpeed	= D3DXVECTOR2( 220.f, -1 );
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_START_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.4f )
	{
		m_bDisableGravity = true;
		//m_PhysicParam.fDownAccel = m_PhysicParam.fGAccel * 0.1f;
	}
	CommonFrameMoveFuture();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.7f );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		FadeInWeaponRenderEffect(D3DXCOLOR(1.f, 1.f, 1.f, 1.f), D3DXCOLOR(1.f, 1.f, 1.f, 0.f), 0.7f, 5.f);
	}


	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN );
	}
	CommonEventProcess();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_START_EndFuture()
{
	m_bDisableGravity = false;
	//m_PhysicParam.fDownAccel = 0.f;

	CommonStateEndFuture();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = false;
	//m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0.f, 0.f );
	//m_PhysicParam.passiveSpeed	= D3DXVECTOR2( 240.f, -1 );
	//m_PhysicParam.fDownAccel = m_PhysicParam.fGAccel * 1.2f;
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.06666f )
	{
		//m_PhysicParam.fDownAccel = m_PhysicParam.fGAccel * 1.4f;
	}
	CommonFrameMoveFuture();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_FrameMove()
{
	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_EventProcess()
{

	if( true == IsOnSomethingFuture() )
		StateChange( CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING );

	CommonEventProcess();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_EndFuture()
{
	//m_bDisableGravity = false;
	//m_PhysicParam.fDownAccel = 0.f;

	CommonStateEndFuture();
}



void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_Init()
{
	TextureReadyInBackground( L"soket_earth_quake_smoke01.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds" );
	TextureReadyInBackground( L"soket_earth_quake_stone01.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P02.dds" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M01.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M02.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M03.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M05.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M06.X" );
}


void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_StateStart()
{
	CommonStateStart();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{

		if( INVALID_EFFECTSET_HANDLE == m_hLandDemolisherAfterEffect )
		{
			m_hLandDemolisherAfterEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_AFTER_EFFECT", this );
			m_vecEffectSetToDeleteOnDie.push_back( m_hLandDemolisherAfterEffect );
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS );
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_EventProcess()
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

void CX2GUChung::ComboZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.1400f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}
	CommonFrameMove();
}

void CX2GUChung::ComboZZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.2300f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}
	CommonFrameMove();
}

void CX2GUChung::ComboZZZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5400f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 150.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -150.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
	CommonFrameMove();
}

void CX2GUChung::ComboZZZZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.6000f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 50.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -50.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
	CommonFrameMove();
}


void CX2GUChung::ComboZZdownZZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.7900f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}
	CommonFrameMove();
}

void CX2GUChung::ComboXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.3000f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 80.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -80.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

	CommonFrameMove();
	return;
}


void CX2GUChung::CFGComboDashZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.2700f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.4900f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * -220.f;
		else
			vTargetPos = GetPos() + GetDirVector() * 220.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
	CommonFrameMove();
}

void CX2GUChung::CIPComboDashXXXXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.62f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 50.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -50.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

	CommonFrameMove();
	return;
}

void CX2GUChung::PowerDownLandingFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.1333f ) &&
		m_bApplyLandDemolisher )
	{

		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * -125.f;
		else
			vTargetPos = GetPos() + GetDirVector() * 125.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL_for_POWER_DOWN_LANDING", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());

		// g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this);
	}
	CommonFrameMove();
}

void CX2GUChung::CIPComboPowerDownLandingZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.3500f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5100f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * -220.f;
		else
			vTargetPos = GetPos() + GetDirVector() * 220.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

	CommonFrameMove();
	return;
}

void CX2GUChung::CIPComboPowerDownLandingZZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.335f ) )
	{
		if( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_POWERLANDING_CANNON", this );
			UseCannon( 1 );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_POWERLANDING_CANNON_MISFIRE", this );
		}
	}
	CommonFrameMove();
	return;
}

void CX2GUChung::ComboDashJumpZZLandingFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 120.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -120.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

	CommonFrameMove();
	return;
}


void CX2GUChung::StandUpAttackFrontFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.58f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * -120.f;
		else
			vTargetPos = GetPos() + GetDirVector() * 120.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.60f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 120.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -120.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL_REVERSE", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
	CommonFrameMove();
}

void CX2GUChung::StandUpAttackBackFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.45f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * -120.f;
		else
			vTargetPos = GetPos() + GetDirVector() * 120.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL_REVERSE", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.47f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 120.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -120.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
	CommonFrameMove();
}


void CX2GUChung::GuardAttackZFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.2700f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 120.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -120.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
	CommonFrameMove();
}


/** @function : ComboZZdownZFrameMove
	@brief : ComboZZdownZ�� FrameMove ( ĳ���� ���� )
*/
void CX2GUChung::ComboZZdownZFrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.466f, 1 );
	// oasis907 : ����� [2011.6.23] û 2�� ���� - ���̾� �ȶ��
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.7200f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}

	CommonFrameMove();
}

/** @function : ComboXXFrameMove
	@brief : ComboXX�� FrameMove ( ������ Ÿ�ӿ� ĳ������ �Ѱ� ���� �ϰ�, DamageDataNext�� ���� )
*/
void CX2GUChung::ComboXXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.466f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_NEXT" );
			m_LuaManager.EndTable();
		}
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.5f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 80.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -80.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());

	}

	UpCannonBallCountAtThisAnimationTime( 0.54f, 1 );

	CommonFrameMove();
}


/** @function : ComboDashJumpZZLandingCannonBallShotFrameMove
	@brief : ComboDashJumpZZLandingCannonBallShot�� FrameMove ( ĳ���� �Ҹ� �� ����Ʈ ��� )
*/
void CX2GUChung::ComboDashJumpZZLandingCannonBallShotFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 100.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -100.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_VERTICAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.15f ) )
	{
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_COMBO_DASH_JUMP_ZZ_Landing_Cannon_Ball_Shot", this );
			UseCannon( 1 );
		}
	}

	CommonFrameMove();
}


/** @function : CFGComboZZXFrameMove
	@brief : û 1�� ���� ǻ��������� ZZX ���� FrameMove ( ������ ������ ���� )
*/
void CX2GUChung::CFGComboZZXFrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5300f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 70.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -70.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.6100f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL_REVERSE", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.7100f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 70.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -70.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());
	}
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.8000f ) &&
		m_bApplyLandDemolisher )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL_REVERSE", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			false, GetPos(), GetRotateDegree());
	}
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.9400f ) &&
		m_bApplyLandDemolisher )
	{
		D3DXVECTOR3 vTargetPos( 0.0f, 0.0f, 0.0f );
		if(GetIsRight() == true)
			vTargetPos = GetPos() + GetDirVector() * 70.f;
		else
			vTargetPos = GetPos() + GetDirVector() * -70.f;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_LAND_DEMOLISHER_COMBO_HORIZONTAL", this, NULL, false, m_fLandDemolisherPowerRate, -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			true, vTargetPos, GetRotateDegree());

	}
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.633f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_SECOND" );
			m_LuaManager.EndTable();
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.84f ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen)
		{
			m_DamageData.SimpleInit();
			m_DamageData.attackerType		= CX2DamageManager::AT_UNIT;
			m_DamageData.optrAttackerGameUnit		= this;
			SetDamageData( "DAMAGE_DATA_FINAL" );
			m_LuaManager.EndTable();
		}
	}

	CommonFrameMove();
}

void CX2GUChung::SetSelfDestructSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID /*= CX2UnitManager::NUI_NONE*/, float fDeleteTime /* = 3.f */ )
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

		if( true == pNPC->GetSelfDestruction() )
			continue;


		if( NULL != pNPC->GetNPCAI() && CX2NPCAI::NAT_ALLY == pNPC->GetNPCAI()->GetAIType() )
		{
			CX2AllyNPCAI* pAllyAI = (CX2AllyNPCAI*) pNPC->GetNPCAI();
			if( GetUnitUID() == pAllyAI->GetAllyUnitUID() )
			{
				switch ( eNPCID )
				{
				case CX2UnitManager::NUI_TARGETING_CHUNG:
				// ��ƿ���� ��Ʈ����ũ ���� �߸���Ÿ �� NPC
				case CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER:

					// ���� �ð� ���� �߰�
					pNPC->StartSelfDestruction( fDeleteTime );
					pNPC->AttachParticle_LUA( true, "CountDown3", "Bip01_Head", D3DXVECTOR3( 0, 80, 0 ) );
					break;
				default:
					break;
				}
			}
		}
	}
}


void CX2GUChung::CHG_ComboXFrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.432f, 1 );

	CommonFrameMove();
}

void CX2GUChung::CHG_ComboXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, CSI_CHG_COMBO_XZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true, CSI_CHG_COMBO_XX )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CHG_ComboXXFrameMove()
{
	// ���̹� ��� ����
    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 10.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 60.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		float fDegree = ( DoAimingTarget( 800.f, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
		if( m_bAimingTarget == true && (fDegree >= 0.001f || fDegree <= -0.001f) )
		{
			m_bRotateBone = true;
			m_fRotateBoneDegree = -fDegree;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		float fRotateDegree = 0.f;
		fRotateDegree = m_fRotateBoneDegree;
		if( m_bAimingTarget == true && ( GetCannonBallCount() >= 1 || IsFullHyperMode() == true ))
		{
			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT", this );
			if( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
				pEffect->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, fRotateDegree) );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT_MISFIRE", this );
			m_bAimingTarget = false;
			m_bRotateBone = false;

			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}

		if( m_fSiegeShellingUserBallRate < GetRandomFloat() )
			UseCannon(1);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.69f ) )
	{
		if( m_bAimingTarget == true )
		{
			[[maybe_unused]] CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHG_COMO_XX_EXPLOSION", GetPowerRate() * m_fSiegeShellingDamageRel, m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );
		}

		m_bRotateBone = false;
	}


	CommonFrameMove();
}

void CX2GUChung::CHG_ComboXXEventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		m_iSiegeShellingCount = 0;
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}

	CommonEventProcess();
}

void CX2GUChung::CHG_ComboDashZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, CSI_CHG_DASH_COMBO_ZZ )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
void CX2GUChung::CHG_ComboDashZZFrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.633f, 1 );

	CommonFrameMove();
}
void CX2GUChung::CHG_ComboDashZZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true, CSI_CHG_DASH_COMBO_ZZX )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUChung::CHG_ComboDashZZXFrameMove()
{
	// ���̹� ��� ����
    if( m_pXSkinAnim->EventTimerOneshot( 0.415f ) )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 10.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 60.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		float fDegree = ( DoAimingTarget( 800.f, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
		if( m_bAimingTarget == true && (fDegree >= 0.001f || fDegree <= -0.001f) )
		{
			m_bRotateBone = true;
			m_fRotateBoneDegree = -fDegree;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.581f ) )
	{
		float fRotateDegree = 0.f;
		fRotateDegree = m_fRotateBoneDegree;
		if( m_bAimingTarget == true && ( GetCannonBallCount() >= 1 || IsFullHyperMode() == true ))
		{
			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT", this );
			if( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect )  )
				pEffect->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, fRotateDegree) );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT_MISFIRE", this );
			m_bAimingTarget = false;
			m_bRotateBone = false;

			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}

		if( m_fSiegeShellingUserBallRate < GetRandomFloat() )
			UseCannon(1);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.585f ) )
	{
		if( m_bAimingTarget == true )
		{
			[[maybe_unused]] CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHG_COMO_XX_EXPLOSION", GetPowerRate() * m_fSiegeShellingDamageRel, m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.834f ) )
	{
		m_bRotateBone = false;
	}

	CommonFrameMove();
}
void CX2GUChung::CHG_ComboDashZZXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		m_iSiegeShellingCount = 0;
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}

	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	//ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true, CSI_SI_A_SIEGE_SHELLING_READYLOOP )


		CommonEventProcess();
}

void CX2GUChung::CHG_ComboDashJumpZXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.202f ) )
 	{
		if( FlushMp( 3.0f ) == true )
		{
 			D3DXVECTOR3 vPos = GetPos();
			D3DXVECTOR3 vHandPos = GetBonePos(L"Dummy2_Lhand");
			vPos.y = vHandPos.y;
			//g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_HAND_GRENADE_0", this, NULL, false, GetPowerRate(), -1.0f, D3DXVECTOR3( 1, 1, 1 ),
			//	true, vPos, GetRotateDegree());
 			CX2DamageEffect::CEffect* pDamageEffectHandGrenade = g_pX2Game->GetDamageEffect()->CreateInstance(
				this, L"CHG_DASHJUM_ZX_BOMB", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
			if( g_pX2Game != NULL && pDamageEffectHandGrenade != NULL )
                g_pX2Game->AddRemoveDamageEffect( pDamageEffectHandGrenade->GetHandle() );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 vAngle = GetRotateDegree();
			vAngle.z -= 50.f;


			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_MAGIC_FAIL ) );
			if( pSeq != NULL )
			{
				PlaySequenceByTriggerCount( pSeq, pos, 100, 100, 1 );
				pSeq->SetAxisAngle( vAngle );
				pSeq->SetAddRotate( vAngle );
			}
		}
 	}

	CommonFrameMove();
}
void CX2GUChung::CHG_ComboDashJumpZXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �߹ؿ� ���θ��� ������
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_Ready_StateStart()
{
	CommonStateStart();

	m_iSiegeShellingCount = 0;
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_Ready_FrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.844f, 1 );

	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_Ready_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_ReadyLoop_FrameMove()
{
	UpNowMp(-3.f * m_fElapsedTime);
	if( GetNowMp() <= 0.f )
	{
		StateChange( USI_WAIT );
	}

	// ���ֿɼ��� �ֻ��ϰ�츸 �����Ѵ�.
	if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_HIGH )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 10.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 60.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		float fDegree = ( DoAimingTarget( 800.f * m_fSiegeShellingDistance, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
		if( m_bAimingTarget && (fDegree >= 0.001f || fDegree <= -0.001f) )
		{
			m_bRotateBone = true;
			m_fRotateBoneDegree = fDegree;
		}
		else
		{
			m_bRotateBone = false;
			m_fRotateBoneDegree = 0.f;
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_ReadyLoop_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	// ���θ� ���� �ִ� ���¶��
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true &&
		m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[1].stateChange )
	{
		if( m_iSiegeShellingCount >= 6 )
		{
			StateChange( CSI_SI_A_SIEGE_SHELLING_X_FINISH );
		}
		else if( IsFullHyperMode() == false )
		{
			if( GetCannonBallCount() <= 0 )
				StateChange( CSI_SI_A_SIEGE_SHELLING_Z_FINISH );
			else if( GetCannonBallCount() <= 1 && m_fSiegeShellingUserBallRate < GetRandomFloatFuture() )
				StateChange( CSI_SI_A_SIEGE_SHELLING_X_FINISH );
			else
				StateChange( CSI_SI_A_SIEGE_SHELLING_X );
		}
		else
		{
			StateChange( CSI_SI_A_SIEGE_SHELLING_X );
		}

		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true && GetExtraCannonBallCount() > 0 &&
		m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[2].stateChange )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_READY );
	}
	else if( m_InputData.oneX == true &&
		m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[1].keyInputEnd &&
		m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[1].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	else if( m_bMobileArmoredModeEnable == true && m_InputData.oneDown == true &&
		m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[2].keyInputEnd &&
		m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[2].keyInputStart )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[2] = true;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true, CSI_SI_A_SIEGE_SHELLING_Z_FINISH )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )


	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_X_FrameMove()
{
	// ���̹� ��� ����
	bool bBoneRotate = true;

// 	if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail == CX2GameOption::OL_HIGH )
// 		bBoneRotate = true;
    // 	else if( m_pXSkinAnim->EventTimerOneshot( 0) )
// 		bBoneRotate = true;

	if( (bBoneRotate = true) )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 10.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 60.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		float fDegree = ( DoAimingTarget( 800.f * m_fSiegeShellingDistance, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
		if( m_bAimingTarget == true && (fDegree >= 0.001f || fDegree <= -0.001f) )
		{
			m_bRotateBone = true;
			m_fRotateBoneDegree = fDegree;
		}
		else
		{
			m_bRotateBone = false;
			m_fRotateBoneDegree = 0.f;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.144f ) )
	{
		float fRotateDegree = 0.f;
		fRotateDegree = m_fRotateBoneDegree;
		if( m_bAimingTarget == true && ( GetCannonBallCount() >= 1 || IsFullHyperMode() == true ))
		{
			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT", this );
			if( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
				pEffect->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, fRotateDegree) );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT_MISFIRE", this );
			m_bAimingTarget = false;
			m_bRotateBone = false;

			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}

		if( m_fSiegeShellingUserBallRate < GetRandomFloat() )
			UseCannon(1);
		++m_iSiegeShellingCount;

		if( m_iExtraCannonBallReloadCount >= 1 )
		{
			SetExtraCannonBallCount( GetExtraCannonBallCount() + 1 );
			m_iExtraCannonBallReloadCount = 0;
		}
		else
		{
			m_iExtraCannonBallReloadCount++;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.147f ) )
	{
		if( m_bAimingTarget == true )
		{
			CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CSI_SI_A_SIEGE_SHELLING_X_EXPLOSION", GetPowerRate(), m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );

			// ź�� ���� �нú�, ���� ����, �̵� ���ݿ� Ÿ�� �� MP ȸ�� ��� �߰�
			if ( NULL != pDamageEffectAimmingShot )
				pDamageEffectAimmingShot->GetDamageData().fHitAddMP += m_fSiegeShellingIncraseMPRel;
		}
	}

	if( g_pMain->GetGameOption().GetOptionList().m_UnitDetail != CX2GameOption::OL_HIGH )
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.245f ) )
		{
			m_bRotateBone = false;
		}
	}
	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_X_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_X_Finish_FrameMove()
{
	// ���̹� ��� ����
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 10.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 60.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		float fDegree = ( DoAimingTarget( 800.f * m_fSiegeShellingDistance, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
		if( m_bAimingTarget == true && (fDegree >= 0.001f || fDegree <= -0.001f) )
		{
			m_bRotateBone = true;
			m_fRotateBoneDegree = fDegree;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.124f ) )
	{
		float fRotateDegree = 0.f;
		fRotateDegree = m_fRotateBoneDegree;
		if( m_bAimingTarget == true && ( GetCannonBallCount() >= 1 || IsFullHyperMode() == true ))
		{
			UseCannon(1);

			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT", this );
			if( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
				pEffect->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, fRotateDegree) );
		}
		else
		{
			// ���� ������ ������ ź (6��) �� �� ����ߴµ�
			// ĳ�� ���� �������� ���, ���� ��´�.
			if ( GetCannonBallCount() > 0 )
				UseCannon(1);
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT_MISFIRE", this );
			m_bAimingTarget = false;
			m_bRotateBone = false;

			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}
		if( m_iExtraCannonBallReloadCount >= 1 )
		{
			SetExtraCannonBallCount( GetExtraCannonBallCount() + 1 );
			m_iExtraCannonBallReloadCount = 0;
		}
		else
		{
			m_iExtraCannonBallReloadCount++;
		}

	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.127f ) )
	{
		if( m_bAimingTarget == true )
		{
			[[maybe_unused]] CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CSI_SI_A_SIEGE_SHELLING_FINISH_EXPLOSION", GetPowerRate() * m_fSiegeShellingDamageRel, m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.466f ) )
	{
		m_bRotateBone = false;
	}
	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_X_Finish_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_SIEGE_SHELLING_Z_Finish_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CSI_P_CHG_CANNONEER_WITHSTANDING_FrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.533f, m_iChargeCannonBall );

	// Ư�� �ð��� ���� ����Ʈ �ߵ� - ����Ʈ�� ���ݷ��� m_fWithStandingPowerRate��ŭ �����Ѵ�.
    if( m_pXSkinAnim->EventTimerOneshot( 0.364f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 100.f;
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHG_WITHSTANDING", GetPowerRate() * m_fWithStandingPowerRate, vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		m_iSiegeShellingCount = 0;
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	CommonEventProcess();
}

void CX2GUChung::CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	CommonEventProcess();
}

void CX2GUChung::CSI_A_CHG_ELASTIC_BOMB_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.195f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vHandPos = GetBonePos(L"Dummy2_Lhand");
		vPos.y = vHandPos.y;

		int iNumBomb = 2;
		float fLifeTime = 0.f;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO16 ) == true && GetRandomFloat() <= 0.2f )
			iNumBomb = 4;

		for(int i=0; i<iNumBomb; ++i)
		{
			CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_ELASTIC_BOMB", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
			if( g_pX2Game != NULL && pDamageEffect != NULL )
			{
                g_pX2Game->AddRemoveDamageEffect( pDamageEffect->GetHandle() );
				CKTDGXMeshPlayer::CXMeshInstance *pMesh = pDamageEffect->GetMainEffect();
				if( pMesh != NULL )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CHG_ELASTIC_BOMB_GRENADE );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == GetUnit() )
						return;

						const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

						float fLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );
						pMesh->SetMaxLifeTime( pMesh->GetMaxLifeTime() * fLifeTime );
					}

					D3DXVECTOR3 vVelocity = pMesh->GetVelocity();

					switch(i)
					{
					case 1:
						vVelocity.x *= 0.6f;
						vVelocity.y *= 1.75f;
						break;
					case 2: // �޸� �߰�
						vVelocity.x *= 0.9f;
						vVelocity.y *= 1.75f;
						break;
					case 3: // �޸� �߰�
						vVelocity.x *= 1.3f;
						break;
					default:
						break;
					}

					pMesh->SetVelocity(vVelocity );
					pMesh->SetMaxLifeTime( pMesh->GetMaxLifeTime() - fLifeTime );

					if( iNumBomb == 2 )
						fLifeTime += 1.f;
					else
						fLifeTime += 0.333f;
				}
			}
		}
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_A_CHG_ELASTIC_BOMB_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CSI_A_CHG_MARK_OF_COMMANDER_Init()
{
	TextureReadyInBackground( L"soket_earth_quake_smoke02.DDS");
	TextureReadyInBackground( L"eve_A_blink_light.dds");
	TextureReadyInBackground( L"chung_markOfCommande_mark.DDS");
	TextureReadyInBackground( L"GuideArrow02.DDS");
	TextureReadyInBackground( L"chung_Elastic_Bome_smoke.dds");
	TextureReadyInBackground( L"Sudden_Buster_P03.dds");
	TextureReadyInBackground( L"chung_Elastic_Bome_spark.DDS");
	TextureReadyInBackground( L"Sudden_Buster_P05.dds");
	XSkinMeshReadyInBackground( L"chung_carpetBombing_bigBomb.X" );
}

void CX2GUChung::CSI_A_CHG_MARK_OF_COMMANDER_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CHUNG_MARK_OF_COMMANDER", this );

		D3DXVECTOR3 vWeaponBonePos	= GetWeaponBonePos( 0, L"ATTACK_SPHERE3_Back01", 0 );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_MARK_OF_COMMANDER", GetPowerRate(), vWeaponBonePos, GetRotateDegree(), GetRotateDegree() );

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CHG_MARK_OF_COMMANDER );
		if ( NULL != pSkillTemplet && pSkillTemplet->m_vecBuffFactorPtr.size() > 1 )
		{
			CX2DamageEffect::CEffect* pEffect
				= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_MARK_OF_COMMANDER_EDT", GetPowerRate(), vWeaponBonePos, GetRotateDegree(), GetRotateDegree() );

			if ( NULL != pEffect )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				/// �޸� ������� 1�� ����, �޸� ����� �ʾ����� 0�� ����
				if ( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO17 ) )
					pEffect->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[1]->GetClonePtr( iSkillTempletLevel ) );
				else
					pEffect->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
			}
		}

	}
	CommonFrameMove();
}

void CX2GUChung::CSI_A_CHG_MARK_OF_COMMANDER_EventProcess()
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

void CX2GUChung::CSI_SA_CHG_WONDER_WALL_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );
// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

    if( m_pXSkinAnim->EventTimerOneshot( 0.195f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CTT_WONDER_WALL );
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );	/// �ڽ����� �Ŵ� ����

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO14) == true )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CTT_WONDER_WALL );
			if ( pSkillTemplet->m_vecBuffFactorPtr.size() > 1 )
			{
				/// �޸� ���� factor�� vector�� index 1���� �������
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
				CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet->m_vecBuffFactorPtr[1]->GetClonePtr( iSkillTempletLevel );
				if ( NULL != ptrBuffFactorClone )
				{
					/// ����� �ڽ��� ������ �����Ͽ� BuffFactor ����
					ptrBuffFactorClone->SetGameUnitBuffFactor( this );

					const D3DXVECTOR3& vMyPos = GetPos();
					for ( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
					{
						CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
						if( pGameUnit != NULL &&
							pGameUnit != this &&
							pGameUnit->GetTeam() == GetTeam() &&
							pGameUnit->CanApplyBuffToGameUnit() &&
							GetDistance3Sq( vMyPos, pGameUnit->GetPos() ) < 160000.0f	/// �Ÿ� 400 �̳�
							 )
							 pGameUnit->SetBuffFactorToGameUnit( *ptrBuffFactorClone );
					}	// for
				}
			}
		}
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHG_WONDER_WALL_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_START_StateStart()
{
	CommonStateStart();

	m_iDreadChaseCount = 0;
}
void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 1.f ) )
	{

		D3DXVECTOR3 vWeaponBonePos		= GetWeaponBonePos( 0, L"TRACE_START0", 0 );
		D3DXVECTOR3 vRotateDegree	= GetRotateDegree();

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO15 ) == true )
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_BIG_MEMO", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		else
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_BIG", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );

		UseCannon( 1 );
		++m_iDreadChaseCount;
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
		{
			if( IsFullHyperMode() == true || GetCannonBallCount() >= 2 )
				StateChange( CSI_SA_CHG_DREAD_CHASE_LOOP );
			else
				StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_LOOP_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		++m_iDreadChaseCount;

		D3DXVECTOR3 vWeaponBonePos	= GetWeaponBonePos( 0, L"TRACE_START0", 0 );
		D3DXVECTOR3 vRotateDegree	= GetRotateDegree();

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO15 ) == true )
		{
			if( m_iDreadChaseCount <= 2 )
				SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_BIG_MEMO", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
			else
				SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_FLY_MEMO", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		}
		else
		{
			if( m_iDreadChaseCount <= 2 )
				SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_BIG", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
			else
				SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_FLY", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		}

		UseCannon( 1 );
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_LOOP_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		else
		{
			if( IsFullHyperMode() == true )
			{
				if( m_iDreadChaseCount >= 4 )
					StateChange( CSI_SA_CHG_DREAD_CHASE_END );
				else
					StateChange( CSI_SA_CHG_DREAD_CHASE_LOOP );
			}
			else
			{
				if( GetCannonBallCount() == 1 || m_iDreadChaseCount >= 4 )
					StateChange( CSI_SA_CHG_DREAD_CHASE_END );
				else if( GetCannonBallCount() > 1 )
					StateChange( CSI_SA_CHG_DREAD_CHASE_LOOP );
				else
					StateChange( USI_WAIT );
			}
		}
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vWeaponBonePos	= GetWeaponBonePos( 0, L"TRACE_START0", 0 );
		D3DXVECTOR3 vRotateDegree	= GetRotateDegree();

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO15 ) == true )
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_FLY_MEMO", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		else
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CHUNG_DREAD_CHASE_FLY", GetPowerRate(), vWeaponBonePos, vRotateDegree, vRotateDegree, m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );

		UseCannon( 1 );
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHG_DREAD_CHASE_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
void CX2GUChung::CSI_SA_CHG_BIGBANG_STREAM_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.68f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CHG_BIGBANG_STREAM );
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );

		CX2DamageEffect::CEffect* pDamageEffect[12] = { NULL, };
		const D3DXVECTOR3& vPos = GetBonePos(L"Bip01");

		for( int i=0; i<12; ++i )
		{
			if( i <= 3)
				pDamageEffect[i] = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_BIGBANGSTREAM", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
			else
				pDamageEffect[i] = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HAND_GRENADE_0", 1.f, vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );

			if( pDamageEffect[i] != NULL )
			{
                g_pX2Game->AddRemoveDamageEffect( pDamageEffect[i]->GetHandle() );
				CKTDGXMeshPlayer::CXMeshInstance *pMesh = pDamageEffect[i]->GetMainEffect();
                if ( pMesh != NULL )
                {
				switch(i)
				{
					// ū��
				case 0:
					{
						float fLifeTime = GetRandomFloat(1) * 0.1f;
						float fRotateZ = (float)((GetRandomInt(1) % 41) + 20);
						pMesh->SetMaxLifeTime(pMesh->GetMaxLifeTime() + fLifeTime);
						pMesh->SetRotateLocalDegree(D3DXVECTOR3(0.f, 0.f, fRotateZ));
					}
					break;
				case 1:
					{
						float fLifeTime = GetRandomFloat(2) * 0.1f;
						float fRotateZ = (float)((GetRandomInt(2) % 41) + 20);
						pMesh->SetMaxLifeTime(pMesh->GetMaxLifeTime() + fLifeTime);
						pMesh->SetRotateLocalDegree(D3DXVECTOR3(0.f, 0.f, fRotateZ+90.f));
					}
					break;
				case 2:
					{
						float fLifeTime = GetRandomFloat(3) * 0.1f;
						float fRotateZ = float((GetRandomInt(3) % 41) + 20);
						pMesh->SetMaxLifeTime(pMesh->GetMaxLifeTime() + fLifeTime);
						pMesh->SetRotateLocalDegree(D3DXVECTOR3(0.f, 0.f, fRotateZ+180.f));
					}
					break;
				case 3:
					{
						float fLifeTime = GetRandomFloat(4) * 0.1f;
						float fRotateZ = float((GetRandomInt(4) % 41) + 20);
						pMesh->SetMaxLifeTime(pMesh->GetMaxLifeTime() + fLifeTime);
						pMesh->SetRotateLocalDegree(D3DXVECTOR3(0.f, 0.f, fRotateZ+270.f));
					}
					break;
					// ������
				case 4:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(5) );
					pMesh->SetVelocity( D3DXVECTOR3(400.f, -200.f, 0.f));
					break;
				case 5:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(6) );
					pMesh->SetVelocity( D3DXVECTOR3(600.f, 700.f, 0.f));
					break;
				case 6:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(7) );
					pMesh->SetVelocity( D3DXVECTOR3(800.f, 400.f, 0.f));
					break;
				case 7:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(8) );
					pMesh->SetVelocity( D3DXVECTOR3(600.f, 100.f, 0.f));
					break;
				case 8:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(9) );
					pMesh->SetVelocity( D3DXVECTOR3(-400.f, -200.f, 0.f));
					break;
				case 9:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(10) );
					pMesh->SetVelocity( D3DXVECTOR3(-600.f, 700.f, 0.f));
					break;
				case 10:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(11) );
					pMesh->SetVelocity( D3DXVECTOR3(-800.f, 400.f, 0.f));
					break;
				case 11:
					pMesh->SetMaxLifeTime( 1.5f + GetRandomFloat(12) );
					pMesh->SetVelocity( D3DXVECTOR3(-600.f, 100.f, 0.f));
					break;
				default:
					break;
				}
                }
			}
		}
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHG_BIGBANG_STREAM_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CSI_SA_CHG_CARPET_BOMBING_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CHG_CARPET_BOMBING );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_iCarpetBombingSmallCount = (int)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, iSkillTempletLevel );
		}

		m_iCarpetBombingCount = 0;
		m_fCarpetBombingTime = 0.f;
		m_bCarpetBombing = true;

		m_vCarpetBombingPos = GetPos();
		D3DXVECTOR3 vDir = GetDirVector();
		m_vCarpetBombingRot = GetRotateDegree();

		D3DXVec3Normalize( &m_vCarpetBombingDir, &vDir );
		if( GetIsRight() == false )
		{
			m_vCarpetBombingDir.x *= -1.f;
			m_vCarpetBombingDir.z *= -1.f;
		}
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHG_CARPET_BOMBING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

		CommonEventProcess();
}

void CX2GUChung::DoCarpetBombing()
{
	if( m_fCarpetBombingTime >= (1.f+(m_iCarpetBombingCount*0.2f)) )
	{
		int iCarpetBombingCnt = m_iCarpetBombingCount % 9;

		float fBaseDist = 2400.f; //1500.f
		float fBombDist = 2000.f; //3000.f

		const float fGap = 3000.f / 8.f;
		D3DXVECTOR3 vPos = m_vCarpetBombingPos;
		float fLandPos = vPos.y;
		vPos.x = vPos.x + (m_vCarpetBombingDir.x * fBaseDist) + (m_vCarpetBombingDir.x * fBombDist);
		vPos.z = vPos.z + (m_vCarpetBombingDir.z * fBaseDist) + (m_vCarpetBombingDir.z * fBombDist);

		vPos.x -= (m_vCarpetBombingDir.x * fGap * iCarpetBombingCnt);
		vPos.z -= (m_vCarpetBombingDir.z * fGap * iCarpetBombingCnt);

		float fPosY = GetRandomFloat(m_iCarpetBombingCount) * 600.f; //1000.f;
		vPos.y += fPosY + 2000.f;

		//
		float fPowerRate = 1.f;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CHG_CARPET_BOMBING );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			fPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
		}

		CX2DamageEffect::CEffect *pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CHUNG_CARPETBOMBING_BIG", fPowerRate, vPos, m_vCarpetBombingRot, m_vCarpetBombingRot,  fLandPos );
        g_pX2Game->AddRemoveDamageEffect( pDamageEffect->GetHandle() );

		CX2DamageEffect::DamageEffectDataInLua &damageEffectDataInLua = pDamageEffect->GetDamageEffectDataInLua();
		float fRotateTemp = 0.f;

		for(int i=0; i<m_iCarpetBombingSmallCount; ++i)
		{
			CX2DamageEffect::DamageEffectData* pData = new CX2DamageEffect::DamageEffectData;
			pData->fEventTime	= 0.85f;
			pData->posType = CX2DamageEffect::DEPT_POS;
			pData->damageEffectName = L"CHUNG_CARPETBOMBING_SMALL";
			pData->bApplyRotate = true;
			pData->bApplyRotatedOffset = true;
			pData->bAutoDie = true;
			float fBombRot = fRotateTemp;
 			if( i % 2 == 1 )
 			{
 				fBombRot *= -1.f;
 			}
 			else
 			{
 				fRotateTemp += 10.f;
 				fBombRot = fRotateTemp;
 			}

			pData->vRotateAngleDegree.z = fBombRot;
			damageEffectDataInLua.m_vecCreateDamageEffect.push_back( pData );

		}

		++m_iCarpetBombingCount;
	}
}

void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CHG_CHAOS_CANNON );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_fChaosCannonTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );

			if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL &&
				m_hChaosCannonEffect == INVALID_EFFECTSET_HANDLE )
			{
				m_hChaosCannonEffect = g_pX2Game->GetEffectSet()->PlayEffectSet(L"EFFECTSET_CHUNG_CHAOS_CANNON", this );
			}
		}
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 1.6f ) )
	{
		if( m_pChaosCannonSound == NULL )
			m_pChaosCannonSound = PlaySound( L"Chung_ChaosCannon_Rumble.ogg", true, true );
		else
			m_pChaosCannonSound->Play(true, true);
	}


	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		}
		else
		{
			//StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WAIT );
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_01 );
		}
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )

		CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_WAIT_FrameMove()
{


	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_WAIT_EventProcess()
{
	if ( false == IsOnSomethingFuture() )	// ��� �ִ� ���� ������
	{
		// �ϰ�
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	} // if
	else if( m_fChaosCannonTime <= 0.f )
	{
		// ī���� ĳ�� ����
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_END );
	}
	// X �� ���� �ִ� ���¶�� ChaosCannon ���� �ð��� 0.35 �� ���̰� �ٷ� �߻� ( �� �߻� Ƚ���� ���� )
	else if ( m_InputData.oneX == true )
	{
		m_fChaosCannonTime = m_fChaosCannonTime - _CONST_CHUNG_::DECREASE_TIME_OF_FAST_SHOT_CHAOS_CANNON;
		ChaosCannonAttackStateChange (1);
	}
	else if( m_FrameDataFuture.unitCondition.fStateTime >= 0.4f )
	{
		ChaosCannonAttackStateChange (1);
	}


	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WALK );
	}
	else if ( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WALK );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown	// �Ʒ� ���� ȭ��ǥ�� ������, ���θ� �Ʒ��� ������ �� ������
	&&	true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
	&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_WALK_FrameMove()
{


	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_WALK_EventProcess()
{
	if ( false == IsOnSomethingFuture() )	// ��� �ִ� ���� ������
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( m_fChaosCannonTime <= 0.f )
	{
		// ī���� ĳ�� ����
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_END );
	}
	// X �� ���� �ִ� ���¶�� ChaosCannon ���� �ð��� 0.35 �� ���̰� �ٷ� �߻� ( �� �߻� Ƚ���� ���� )
	else if ( m_InputData.oneX == true )
	{
		m_fChaosCannonTime = m_fChaosCannonTime - _CONST_CHUNG_::DECREASE_TIME_OF_FAST_SHOT_CHAOS_CANNON;
		ChaosCannonAttackStateChange (2);
	}
	else if( m_FrameDataFuture.unitCondition.fStateTime >= 0.4f )
	{
		ChaosCannonAttackStateChange (2);

	}
	else if ( false == m_InputData.pureRight && false == m_InputData.pureLeft ) // �̵� �Է��� ������ ����
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WAIT );
	}
	else if ( true == m_InputData.pureUp )	// ����
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown	// �Ʒ� ���� ȭ��ǥ�� ������, ���θ� �Ʒ��� ������ �� ������
	&&	true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
	&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd()
		|| true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP_EventProcess()
{
	// ��� �߿� ������ �ߴ��� �Ǵ�
	if( 0.0f >= m_PhysicParam.nowSpeed.y )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;

		// JumpDown�� m_bDownForce�� ���� �̾ ���� �� �� �ֵ��� false ������ �ʴ´�.
		m_bDownForce = true;
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN_EventProcess()
{
	// �� �Ʒ��� ���θ��� ������ ����
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG );
		m_bDownForce = false;
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG );
	}
	else if ( 0.0f <= m_PhysicParam.nowSpeed.y )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG_FrameMove()
{


	CommonFrameMove();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( m_fChaosCannonTime <= 0.f )
	{
		// ī���� ĳ�� ����
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_END );
	}
	//else if( true == m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.2f )
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{

		ChaosCannonAttackStateChange(3);
	}
	else if( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WALK );
	}
	else if( true == m_InputData.pureRight || true == m_InputData.pureLeft )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WALK );
	}
	else if( true == m_InputData.pureUp )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( true == m_InputData.oneDown
	&& true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position,
	LINE_RADIUS, &m_FrameDataFuture.unitCondition.landPosition,
	&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		else
			StateChange( USI_WAIT );
	}
	//DASH_CANCEL_AFTER( m_fDashCancelAfter )
	//WALK_CANCEL_AFTER( m_fWalkCancelAfter )

	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_GUARD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		}
		else if( m_fChaosCannonTime <= 0.f )
		{
			// ī���� ĳ�� ����
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_END );
		}
		else
		{
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WAIT );
		}
	}
 	else if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.2f )
 	{
 		switch( GetRandomIntFuture(4) % 4 )
 		{
 		case 0:
 			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_02 );
 			break;
 		case 1:
 			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_03 );
 			break;
 		default:
 			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_01 );
 			break;
 		}
 	}
	CommonEventProcess();
}
void CX2GUChung::CSI_SI_A_CHG_CHAOS_CANNON_ATTACK_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN );
		}
		else if( m_fChaosCannonTime <= 0.f )
		{
			// ī���� ĳ�� ����
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_END );
		}
		else
		{
			StateChange( CSI_SI_A_CHG_CHAOS_CANNON_WAIT );
		}
	}
	CommonEventProcess();
}

void CX2GUChung::DeleteMinorParticle()
{
	CX2GUUser::DeleteMinorParticle();

	DeleteChungMinorParticle();
}

/*virtual*/ CX2GageData* CX2GUChung::CreateGageData()
{
	if ( IsMyUnit() )
	{
		const CX2ChungGageData* pChungGageData
			= static_cast<const CX2ChungGageData*>( CX2GageManager::GetInstance()->GetMyGageData() );

		if ( NULL != pChungGageData )
			return new CX2ChungGageData( *pChungGageData );
		else
			return new CX2ChungGageData();
	}
	else
		return new CX2ChungGageData();
}

/*virtual*/ void CX2GUChung::ChargeMpAndEtcInTrainingGame()
{
	CX2GUUser::ChargeMpAndEtcInTrainingGame();

	SetCannonBallCount( GetChungGageData()->GetMaxCannonBallCount() );
}

/** @function : ApplyHyperModeBuff
	@brief : �������� ����
*/
/*virtual*/ void CX2GUChung::ApplyHyperModeBuff()
{
	CX2GameUnit::ApplyHyperModeBuff();

	if ( IsFullHyperMode() )	/// Ǯ���� ���۽� ������ ����Ʈ
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CHUNG_BERSERK_MODE_START", this );
}

/** @function : HyperModeBuffEffectStart
	@brief : ���� �� ���;��ϴ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUChung::HyperModeBuffEffectStart()
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRFoot )
		m_hHyperBoostRFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightBlue",	0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLFoot )
		m_hHyperBoostLFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftBlue",		0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRArm )
		m_hHyperBoostRArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightBlue",	0, 0, 0, 0, 0);

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLArm )
		m_hHyperBoostLArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftBlue",		0, 0, 0, 0, 0);

	// ����ȭ
	if ( IsFullHyperMode() )
	{
		SetShowViewList( true );
		// BerserkMode�� Set�� ������ �� �Ŀ� UpdateEquipCollisionData()�� ȣ���ؾ� �Ѵ�.
		UpdateEquipCollisionData();
		// ���⿡ �ٴ� ����Ʈ ���� ����
		SetNeedToUpdateWeaponEffect( true );
		GetChungGageData()->SetCannonBallChanged( true );


		for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
		{
			pEquipPtr->ChangeHyperMode();
		}
	}
	else
	{
		for( CX2EqipPtr pEquipPtr : m_ViewEqipList )
		{
			pEquipPtr->ChangeHyperMode();
		}
	}
}

/** @function : HyperModeBuffEffectEnd
	@brief : ���� ����� ��������Ʈ ������ ���� ����Ǵ� �Լ�
*/
/*virtual*/ void CX2GUChung::HyperModeBuffEffectEnd()
{
	CX2GUUser::HyperModeBuffEffectEnd();

	// ����ȭ ����ε�, �����ð��� �� �Ǿ�����
	if ( IsFullHyperMode() )
	{
		for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
		{
			pEquipPtr->ChangeNormalMode();
		}

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CHUNG_BERSERK_MODE_START", this );

		SetFullHyperMode( false );
		SetShowViewList( true );
		UpdateEquipCollisionData();
		// ���⿡ �ٴ� ����Ʈ ���� ����
		SetNeedToUpdateWeaponEffect( true );
		GetChungGageData()->SetCannonBallChanged( true );

		// ����ȭ�� ������ ĳ������ Ǯ ���� ������
		SetCannonBallCount( GetChungGageData()->GetMaxCannonBallCount() );

		for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
		{
			pEquipPtr->ChangeNormalMode();
		}
	}
	else
	{
		for( CX2EqipPtr pEquipPtr : m_ViewEqipList )
		{
			pEquipPtr->ChangeNormalMode();
		}
	}
}

/** @function : SetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUChung::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_BUFF_LAND_DIMOLISHER:
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS );

			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				m_bApplyLandDemolisher = true;
				m_fLandDemolisherPowerRate = pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				PulseWeaponRenderEffect(D3DXCOLOR(1.f, 1.f, 1.f, 1.0f), D3DXCOLOR(1.f, 1.f, 1.f, 0.1f), 1.0f, 9999.0f, PWCI_CHUNG_LAND_DEMOLISHER);
			}
		} break;
	case BTI_SI_SA_CSG_SHARPSHOOTER_SYNDROME:
		{
			m_fMiniGunHeadShotRate = 0.f;
			const CX2UserSkillTree& cUserSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillLevel = cUserSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_CDC_SHARPSHOOTER_SYNDROME );

			if ( 0 < iSkillLevel )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_SA_CDC_SHARPSHOOTER_SYNDROME);

				if( NULL != pSkillTemplet )
				{
					m_fMiniGunHeadShotRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
				}
			}
		} break;

	default:
		break;
	}
}

/** @function : UnSetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUChung::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_BUFF_LAND_DIMOLISHER:
		{
			SetEnableWeaponRenderEffect( false );
			m_fLandDemolisherPowerRate = 0.0f;
			m_bApplyLandDemolisher = false;
		} break;
	case BTI_SI_SA_CSG_SHARPSHOOTER_SYNDROME:
		{
			m_fMiniGunHeadShotRate = 0.f;
		} break;
	default:
		break;
	}
}
/** @function : ApplyRenderParam
	@brief : ������ ���� renderparam�� �Բ� �߰������� �غ��ٽ��� ���� renderparam�� �����ϴ� �Լ�
	@param : SkinAnim�� RenderParam ������(pRenderParam_)

*/
/*virtual*/ void CX2GUChung::ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if ( m_bPassDash )
		return;

	if ( true == IsThisStateCanBeChangedToGuardStateOnHit()
		//{{ kimhc // 2011.1.13 // û 1�� ����
		|| CSI_SI_A_CFG_GUARD_DAMAGE == GetNowStateID()
		//}} kimhc // 2011.1.13 // û 1�� ����
		|| m_fChaosCannonTime > 0.f
		)
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
		pRenderParam_->outLineColor		= 0xff00f5ff;	// �Ķ���
	} // if
	else
		CX2GameUnit::ApplyRenderParam( pRenderParam_ );
}

/*virtual*/ void CX2GUChung::ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if( GetRemainHyperModeTime() > 0.0f )
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
		pRenderParam_->outLineColor		= 0xffffffff;	// ���
	}
	else
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
		pRenderParam_->outLineColor		= 0xffffffff;
	}

	if(m_fRemainIronWillRenderEffectTime > 0.f)
	{
		SetUseWorldColor(false);
		float fAlpha = 1.f - m_fRemainIronWillRenderEffectTime / _CONST_CHUNG_::MAGIC_NUMBER_IRON_WILL_RENDER_EFFECT_TIME;
		pRenderParam_->renderType			= CKTDGXRenderer::RT_CARTOON_FADE;
		pRenderParam_->color = D3DXCOLOR( 0.4f, 1.f, 0.7f, fAlpha );
	}
	else
	{
		SetUseWorldColor(true);
	}
}

/// CSI_CTT_COMBO_ZZX
void CX2GUChung::CTT_ComboZZXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.4333f ) )
	{
		if( FlushMp( 3.0f ) == true )
		{
			D3DXVECTOR3 vPos = GetPos();
			D3DXVECTOR3 vHandPos = GetBonePos(L"Dummy2_Lhand");
			vPos.y = vHandPos.y + 50.f;

			CX2DamageEffect::CEffect* pDamageEffectHandGrenade = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_ComboZZX_BOMB", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
			if( g_pX2Game != NULL && pDamageEffectHandGrenade != NULL )
				g_pX2Game->AddRemoveDamageEffect( pDamageEffectHandGrenade->GetHandle() );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 vAngle = GetRotateDegree();
			vAngle.z -= 50.f;


			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_MAGIC_FAIL ) );
			if( pSeq != NULL )
			{
				PlaySequenceByTriggerCount( pSeq, pos, 100, 100, 1 );
				pSeq->SetAxisAngle( vAngle );
				pSeq->SetAddRotate( vAngle );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CTT_ComboZZXEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true, CSI_CTT_COMBO_ZZXX )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fCanNotAttackTime = 0.1f;
		StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	BWALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

/// CSI_CTT_COMBO_ZZXX
void CX2GUChung::CTT_ComboZZXXFrameMove()
{
	UpCannonBallCountAtThisAnimationTime( 0.4f, 1 );
	CommonFrameMove();
}


/// CSI_CTT_COMBO_ZZXX
void CX2GUChung::CTT_ComboZZXXEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_iSiegeShellingCount = 0;
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}
	CommonEventProcess();
}


/// CSI_CTT_COMBO_ZZX
void CX2GUChung::CTT_DashComboXXFrameMove()
{
	// ���̹� ��� ����
    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		float fMinCoefficient = -1.f * tan( D3DX_PI * 10.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 30.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 45.f / 180.f);
		[[maybe_unused]] float fDegree = ( DoAimingTarget( 800.f, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.8333f ) )
	{
		if( m_bAimingTarget == true && ( GetCannonBallCount() >= 1 || IsFullHyperMode() ) )
		{
			[[maybe_unused]] CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_ATTACK_DASH_XX", this );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_AIMMING_SHOT_MISFIRE", this );
			m_bAimingTarget = false;

			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}

		if( m_fSiegeShellingUserBallRate < GetRandomFloat() )
			UseCannon(1);
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.866f ) )
	{
		if( m_bAimingTarget == true )
		{
			[[maybe_unused]] CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CTT_ATTACK_DASH_XX", GetPowerRate() * m_fSiegeShellingDamageRel, m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );
		}
	}

	UpCannonBallCountAtThisAnimationTime( 0.3f, 1 );

	CommonFrameMove();
}

void CX2GUChung::CTT_DashComboXXEventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 3 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_iSiegeShellingCount = 0;
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}

	CommonEventProcess();
}

void CX2GUChung::CTT_MobileArmoredModeReadyStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = 0.f;
		m_GameCameraOffset.fCameraDistance = 0.f;
	}
	m_fMobileArmoredModeFireZCoolTime = 0.f;
	m_fMobileArmoredModeFireXCoolTime = 0.f;

	if( g_pX2Game != NULL && g_pX2Game->GetEffectSet() != NULL &&
		m_hMobileArmoredModeEffect == INVALID_EFFECTSET_HANDLE )
	{
		m_hMobileArmoredModeEffect = g_pX2Game->GetEffectSet()->PlayEffectSet(L"EFFECTSET_CHUNG_CHAOS_CANNON", this );
	}
	m_bInStateMobileArmoredMode = true;
}
void CX2GUChung::CTT_MobileArmoredModeReadyFrameMove()
{
	if( IsMyUnit() )
	{
		m_GameCameraOffset.fLookatDistance = ( m_pXSkinAnim->GetNowAnimationTime() / m_pXSkinAnim->GetMaxAnimationTime() ) * _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = ( m_pXSkinAnim->GetNowAnimationTime() / m_pXSkinAnim->GetMaxAnimationTime() ) * _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeReadyEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_LOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneX == true && m_fMobileArmoredModeFireXCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_X )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeReadyStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}

void CX2GUChung::CTT_MobileArmoredModeLoopFowardStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_bInStateMobileArmoredMode = true;
}
void CX2GUChung::CTT_MobileArmoredModeLoopFowardFrameMove()
{
	m_fMobileArmoredModeStepDustElapsedTime += m_fElapsedTime;
	if( m_fMobileArmoredModeStepDustElapsedTime > 0.4666f )
	{
		m_fMobileArmoredModeStepDustElapsedTime = 0.f;
		CreateStepDust();
	}
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeLoopFowardEventProcess()
{
	UpNowMp( -3.f * m_fElapsedTime );
	if( GetNowMp() <= 0.f )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_UNREADY );
	}

	bool bFowardKey = false;
	bool bBackwardKey = false;
	if( GetIsRight() == true )
	{
		if( m_InputData.pureRight == true )
			bFowardKey = true;
		else
			bFowardKey = false;

		if( m_InputData.pureLeft == true )
			bBackwardKey = true;
		else
			bBackwardKey = false;
	}
	else
	{
		if( m_InputData.pureLeft == true )
			bFowardKey = true;
		else
			bFowardKey = false;

		if( m_InputData.pureRight == true )
			bBackwardKey = true;
		else
			bBackwardKey = false;
	}

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange, m_InputData.oneDown == true || GetExtraCannonBallCount() <= 0, CSI_CTT_MOBILE_ARMORED_MODE_UNREADY )
	//ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true && m_fMobileArmoredModeFireZCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_Z )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true && m_fMobileArmoredModeFireXCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_X )
	ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange, bBackwardKey == true, CSI_CTT_MOBILE_ARMORED_MODE_LOOP_BACKWARD )
	ELSE_IF_STATE_CHANGE_ON_EX_( 4, m_fEventTime[4].keyInputStart, m_fEventTime[4].keyInputEnd, m_fEventTime[4].stateChange, bFowardKey == false, CSI_CTT_MOBILE_ARMORED_MODE_LOOP )

	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeLoopFowardStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}

void CX2GUChung::CTT_MobileArmoredModeLoopBackwardStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_bInStateMobileArmoredMode = true;
}
void CX2GUChung::CTT_MobileArmoredModeLoopBackwardFrameMove()
{
	m_fMobileArmoredModeStepDustElapsedTime += m_fElapsedTime;
	if( m_fMobileArmoredModeStepDustElapsedTime > 0.4666f )
	{
		m_fMobileArmoredModeStepDustElapsedTime = 0.f;
		CreateStepDust();
	}
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeLoopBackwardEventProcess()
{
	UpNowMp( -3.f * m_fElapsedTime );
	if( GetNowMp() <= 0.f )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_UNREADY );
	}

	bool bFowardKey = false;
	bool bBackwardKey = false;
	if( GetIsRight() == true )
	{
		if( m_InputData.pureRight == true )
			bFowardKey = true;
		else
			bFowardKey = false;

		if( m_InputData.pureLeft == true )
			bBackwardKey = true;
		else
			bBackwardKey = false;
	}
	else
	{
		if( m_InputData.pureLeft == true )
			bFowardKey = true;
		else
			bFowardKey = false;

		if( m_InputData.pureRight == true )
			bBackwardKey = true;
		else
			bBackwardKey = false;
	}

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange, m_InputData.oneDown == true || GetExtraCannonBallCount() <= 0, CSI_CTT_MOBILE_ARMORED_MODE_UNREADY )
	//ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true && m_fMobileArmoredModeFireZCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_Z )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true && m_fMobileArmoredModeFireXCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_X )
	ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange, bFowardKey == true, CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD )
	ELSE_IF_STATE_CHANGE_ON_EX_( 4, m_fEventTime[4].keyInputStart, m_fEventTime[4].keyInputEnd, m_fEventTime[4].stateChange, bBackwardKey == false, CSI_CTT_MOBILE_ARMORED_MODE_LOOP )

	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeLoopBackwardStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}

void CX2GUChung::CTT_MobileArmoredModeLoopStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_bInStateMobileArmoredMode = true;
}
void CX2GUChung::CTT_MobileArmoredModeLoopFrameMove()
{
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeLoopEventProcess()
{
	UpNowMp( -3.f * m_fElapsedTime );
	if( GetNowMp() <= 0.f )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_UNREADY );
	}

	bool bFowardKey = false;
	bool bBackwardKey = false;
	if( GetIsRight() == true )
	{
		if( m_InputData.pureRight == true )
			bFowardKey = true;
		else
			bFowardKey = false;

		if( m_InputData.pureLeft == true )
			bBackwardKey = true;
		else
			bBackwardKey = false;
	}
	else
	{
		if( m_InputData.pureLeft == true )
			bFowardKey = true;
		else
			bFowardKey = false;

		if( m_InputData.pureRight == true )
			bBackwardKey = true;
		else
			bBackwardKey = false;
	}

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange, m_InputData.oneDown == true || GetExtraCannonBallCount() <= 0, CSI_CTT_MOBILE_ARMORED_MODE_UNREADY )
	//ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange, m_InputData.oneZ == true && m_fMobileArmoredModeFireZCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_UNREADY )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange, m_InputData.oneX == true && m_fMobileArmoredModeFireXCoolTime <= 0.f, CSI_CTT_MOBILE_ARMORED_MODE_X )
	ELSE_IF_STATE_CHANGE_ON_EX_( 3, m_fEventTime[3].keyInputStart, m_fEventTime[3].keyInputEnd, m_fEventTime[3].stateChange, bFowardKey == true, CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD )
	ELSE_IF_STATE_CHANGE_ON_EX_( 4, m_fEventTime[4].keyInputStart, m_fEventTime[4].keyInputEnd, m_fEventTime[4].stateChange, bBackwardKey == true, CSI_CTT_MOBILE_ARMORED_MODE_LOOP_BACKWARD )

	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeLoopStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}

void CX2GUChung::CTT_MobileArmoredModeUnreadyStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_bInStateMobileArmoredMode = false;
}
void CX2GUChung::CTT_MobileArmoredModeUnreadyFrameMove()
{
	if( IsMyUnit() )
	{
		m_GameCameraOffset.fLookatDistance = ( 1.f - m_pXSkinAnim->GetNowAnimationTime() / m_pXSkinAnim->GetMaxAnimationTime() ) * _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = ( 1.f - m_pXSkinAnim->GetNowAnimationTime() / m_pXSkinAnim->GetMaxAnimationTime() ) * _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeUnreadyEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeUnreadyStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}

void CX2GUChung::CTT_MobileArmoredModeZStateStartFuture()
{
	CommonStateStartFuture();
	m_fMobileArmoredModeFireZCoolTime = _CONST_CHUNG_::MAGIC_MOBILE_MODE_Z_COOLTIME;
}

void CX2GUChung::CTT_MobileArmoredModeZStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_bInStateMobileArmoredMode = true;
}
void CX2GUChung::CTT_MobileArmoredModeZFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{

		D3DXVECTOR3 vWeaponBonePos	= GetWeaponBonePos( 0, L"TRACE_END0", 0 );
		D3DXVECTOR3 vDirVector		= GetDirVector();
		D3DXVECTOR3 vOffsetPos( 170.0f, 22.0f, 0.0f );
		if( true == GetIsRight() )
		{
			vWeaponBonePos += vDirVector * vOffsetPos.x;
		}
		else
		{
			vWeaponBonePos -= vDirVector * vOffsetPos.x;
		}
		vWeaponBonePos.y += vOffsetPos.y;

		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vHandPos = GetBonePos(L"Dummy2_Lhand");
		vPos.y = vHandPos.y;

		if( FlushMp( 3.0f ) == true )
		{
			CX2DamageEffect::CEffect* pDamageEffectHandGrenade = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_MOBILE_MODE_Z_BOMB", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
			if( g_pX2Game != NULL && pDamageEffectHandGrenade != NULL )
                g_pX2Game->AddRemoveDamageEffect( pDamageEffectHandGrenade->GetHandle() );
		}
		else
		{
			D3DXVECTOR3 vAngle = GetRotateDegree();
			vAngle.z += 10.f;

			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleCommonMajorParticleByEnum( COMMON_MAJOR_PII_MAGIC_FAIL ) );
			if( pSeq != NULL )
			{
				PlaySequenceByTriggerCount( pSeq, vWeaponBonePos, 100, 100, 1 );
				pSeq->SetAxisAngle( vAngle );
				pSeq->SetAddRotate( vAngle );
			}
		}
	}
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->GetNowAnimationTime() > 0.06666f )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_LOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeZStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}

void CX2GUChung::CTT_MobileArmoredModeXStateStartFuture()
{
	CommonStateStartFuture();
	m_fMobileArmoredModeFireXCoolTime = _CONST_CHUNG_::MAGIC_MOBILE_MODE_X_COOLTIME;
}
void CX2GUChung::CTT_MobileArmoredModeXStateStart()
{
	CommonStateStart();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = true;
		m_GameCameraOffset.fLookatDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_LOOKAT_DISTANCE;
		m_GameCameraOffset.fCameraDistance = _CONST_CHUNG_::MAGIC_MOBILE_MODE_CAMERA_DISTANCE;
	}
	m_bInStateMobileArmoredMode = true;
}
void CX2GUChung::CTT_MobileArmoredModeXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.033333f ) )
	{
		m_bMobileArmoredModeXFired = false;
		float fMinCoefficient = -1.f * tan( D3DX_PI * 15.f / 180.f );
		float fMaxCoefficient = tan( D3DX_PI * 15.f / 180.f);
		D3DXVECTOR3 vCoefficient = D3DXVECTOR3(fMaxCoefficient, fMinCoefficient, D3DX_PI * 25.f / 180.f);
		[[maybe_unused]] float fDegree = ( DoAimingTarget( _CONST_CHUNG_::MAGIC_MOBILE_MODE_X_MAX_RANGE * m_fMobileArmoredModeDistance, vCoefficient, m_bAimingTarget, m_vAimingTargetPos, m_vAimingTargetLandPos, false ) * 180.f ) / D3DX_PI;
		if( GetExtraCannonBallCount() >= 1 )
		{
			CX2EffectSet::Handle hEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_MOBILE_MODE_X", this );
 			if( CX2EffectSet::EffectSetInstance* pEffect = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffect ) )
 				pEffect->SetLocalRotateDegree( D3DXVECTOR3(0.f, 0.f, 10.f) );
			SetExtraCannonBallCount( GetExtraCannonBallCount() - 1 );
			m_bMobileArmoredModeXFired = true;
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_MOBILE_MODE_X_MISFIRE", this );
			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.066666f ) )
	{
		if( m_bAimingTarget == true && m_bMobileArmoredModeXFired == true )
		{
			CX2DamageEffect::CEffect* pDamageEffectAimmingShot = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CSI_CTT_MOBILE_MODE_X_EXPLOSION", GetPowerRate() * m_fMobileArmoredModeDamageRel, m_vAimingTargetPos, GetRotateDegree(), GetRotateDegree(),  m_vAimingTargetLandPos.y );
			// �̵� ����, ź�� ���� �нú��� MP �߰� ȹ�淮 ������ �޵��� �߰�
			if ( NULL != pDamageEffectAimmingShot )
				pDamageEffectAimmingShot->GetDamageData().fHitAddMP += m_fSiegeShellingIncraseMPRel;
		}
		else if( m_bMobileArmoredModeXFired == true )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_MOBILE_MODE_X_MISFIRE", this );
			PlaySound( L"Chung_CannonGun_Trigger.ogg" );
		}
	}
	m_fMobileArmoredModeFireZCoolTime -= m_fElapsedTime;
	m_fMobileArmoredModeFireXCoolTime -= m_fElapsedTime;
	CommonFrameMove();
}
void CX2GUChung::CTT_MobileArmoredModeXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( CSI_CTT_MOBILE_ARMORED_MODE_LOOP );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	CommonEventProcess();
}
void CX2GUChung::CTT_MobileArmoredModeXStateEnd()
{
	CommonStateEnd();
	if( IsMyUnit() )
	{
		m_GameCameraOffset.bEnable = false;
		m_GameCameraOffset.fLookatDistance = 0.f;
	}
	m_bInStateMobileArmoredMode = false;
}





void CX2GUChung::CSI_SI_SA_CTT_CROSS_FIRE_Init()
{

}

void CX2GUChung::CSI_SI_SA_CTT_CROSS_FIRE_StartFuture()
{
	CommonStateStartFuture();
	//m_bApplyMotionOffset = false;
}

void CX2GUChung::CSI_SI_SA_CTT_CROSS_FIRE_FrameMove()
{
// #ifndef UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );
/// #endif // UPGRADE_SKILL_SYSTEM_2013 // û ��ų ����, ������

    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_CANNON_STRIKE_FIRST", this );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CTT_CANNON_STRIKE_SECOND", this );
	}

	if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) && GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO28 ) == true )
		SetCannonBallCount( GetChungGageData()->GetMaxCannonBallCount() );
	else
	UpCannonBallCountAtThisAnimationTime( 0.5, 1 );
	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CTT_CROSS_FIRE_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
		{
			m_iSiegeShellingCount = 0;
			StateChange( CSI_SI_A_SIEGE_SHELLING_READYLOOP );
		}
	}
	CommonEventProcess();
}


/// CSI_A_CTT_REMOTE_MORTAR
void CX2GUChung::CSI_A_CTT_REMOTE_MORTAR_Init()
{

}
void CX2GUChung::CSI_A_CTT_REMOTE_MORTAR_StateStart()
{
	CommonStateStart();
}

void CX2GUChung::CSI_A_CTT_REMOTE_MORTAR_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		CreateRemoteMortar();		/// �ڰ��� ����
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 50.f;
		}
		else
		{
			vPos -= GetDirVector() * 50.f;
		}
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_REMOTE_MORTAR_DAMAGE", GetPowerRate(),
			vPos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_A_CTT_REMOTE_MORTAR_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
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

/// �ڰ��� ��ȯ �Լ�
void CX2GUChung::CreateRemoteMortar()
{
	if( -1 != m_iRemoteMortarNPCUID )
	{
		CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( static_cast<int>( m_iRemoteMortarNPCUID ) );
		if( NULL != pNPC )
		{
			pNPC->SetNowHp( 0.f );

			DeleteRemoteMortarNPCUID();
		}
	}

	if( true == g_pX2Game->IsHost() )
	{
		// ���ο� ���� ��ȯ
		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
		{
			vPos += GetDirVector() * 50.f;
		}
		else
		{
			vPos -= GetDirVector() * 50.f;
		}

		g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_REMOTE_MORTAR, IsHyperState(), true, vPos,
			GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
	}
}

void CX2GUChung::SupplyCannonBallCount( int SupplyCount )
{
	UpCannonBallCount( SupplyCount, false, true );		/// ��Ƽ�� �ʵ�� ���ε� �����͸� ���� ����
	SetExtraCannonBallCount( GetExtraCannonBallCount() + SupplyCount );
}
/// CSI_SA_CTT_TACTICAL_FIELD
void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_Init()
{

}

void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_StateStart()
{
	CommonStateStart();
}

void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( m_pXSkinAnim->EventTimerOneshot( 0.932f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_MESH1_NODAMAGE", GetPowerRate(),
			GetPos(), GetRotateDegree(), GetRotateDegree() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_MESH2_NODAMAGE", GetPowerRate(),
			GetPos(), GetRotateDegree(), GetRotateDegree() );


		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CTT_TACTICAL_FIELD_INDURANCE );

		if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_DAMAGE", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree() );

			/// ��Ƽ�� �ʵ� ����
            CX2DamageEffect::CEffect*
			m_pCEffectTacticalField = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_ZONE", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree() );
            m_hCEffectTacticalField = ( m_pCEffectTacticalField != NULL ) ? m_pCEffectTacticalField->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
			if( NULL != m_pCEffectTacticalField )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				m_pCEffectTacticalField->GetDamageData().PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
			}

			if ( IsMyUnit() && NULL != GetChungGageData() )
			{
				SetCannonBallCount( GetChungGageData()->GetMaxCannonBallCount() );
				SetExtraCannonBallCount( GetChungGageData()->GetMaxCannonBallCountEx() );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.6f )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
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

/** @function : GetFinalDamageToBeChangedBySkill
	@brief : �� ������ ��ų�� ���ؼ� ����� FinalDamage�� ���� �Լ�
	@param : ����Ǳ� ���� FinalDamage
	@return : ����� FinalDamage
*/
/*virtual*/ float CX2GUChung::GetFinalDamageToBeChangedBySkill( float fFinalDamage_ )
{
	if( CX2Unit::UC_CHUNG_TACTICAL_TROOPER == GetUnitClass() )
	{
		/// ���� �尩
		if ( GetRemainHyperModeTime() > 0.f )
		{
			const CX2UserSkillTree& cUserSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillLevel = cUserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_CTT_REACTIVE_ARMOR, true );

			if ( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_CTT_REACTIVE_ARMOR );
				if ( NULL != pSkillTemplet )
				{
					CX2SkillTree::SKILL_ABILITY_TYPE eSkillAbilityType = CX2SkillTree::SA_FIRST_HYPER_EFFECTIVE;
					switch ( GetHyperModeCountUsed()  )
					{
					case 1:
						eSkillAbilityType = CX2SkillTree::SA_FIRST_HYPER_EFFECTIVE;
						break;

					case 2:
						eSkillAbilityType = CX2SkillTree::SA_SECOND_HYPER_EFFECTIVE;
						break;

					default:
						eSkillAbilityType = CX2SkillTree::SA_THIRD_HYPER_EFFECTIVE;
						break;
					}

					float fDamageReduceRate = 1.f - pSkillTemplet->GetSkillAbilityValue( eSkillAbilityType, iSkillLevel );
					fFinalDamage_ *= fDamageReduceRate;
				}
			}
		}
	}

	/// ��Ƽ�� �ʵ� ������ ����
	if ( m_fTacticalFieldDecreaseDamageValue < 1.f )
	{
		fFinalDamage_ *= m_fTacticalFieldDecreaseDamageValue;
	}

	return fFinalDamage_;
}

void CX2GUChung::SetExtraDamageMiniGun( CX2DamageManager::DamageData& damageData_, float fRate_ )
{
	damageData_.m_ExtraDamage.Init();
	if( 0.f >= fRate_ )
		return;

	damageData_.m_ExtraDamage.m_ExtraDamageType = CX2DamageManager::EDT_HEAD_SHOT;
	damageData_.m_ExtraDamage.m_fTime = 5.f;
	damageData_.m_ExtraDamage.m_iLevel = 0;
	damageData_.m_ExtraDamage.m_fRate = fRate_;
	damageData_.m_ExtraDamage.m_fPureRate = fRate_;
	//damageData_.m_ExtraDamage.m_bIgnoreResist = true;
}

/*virtual*/ bool CX2GUChung::CanRidingState()
{
	if ( GetNowStateID() == CX2GUUser::USI_WAIT || GetNowStateID() == CX2GUUser::USI_WALK ||
		GetNowStateID() == CX2GUUser::USI_DASH || GetNowStateID() == CX2GUUser::USI_DASH_END ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT || GetNowStateID() == CX2GUUser::USI_RIDING_WALK ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT_HABIT || GetNowStateID() == CX2GUUser::USI_RIDING_DASH ||
		GetNowStateID() == CX2GUUser::USI_RIDING_DASH_END || GetNowStateID() == CX2GUChung::CSI_CHARGE_CANNON_BALL )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//2013 ��ų����
/////////////////////////////////////////////////////////////////////////////////////////////////////




void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_READY_StartFuture()
{
	CommonStateStartFuture();
	// ������̼��� StartFuture (��ǿ����� ���� ����)
	m_bApplyMotionOffset = false;
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_READY_EventProcess()
{
	if ( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// ����ȭ��尡 �ƴϸ� ĳ���� ������ ����
		if ( 1 >= GetCannonBallCount() && false == IsFullHyperMode() )
		{
			StateChange( CSI_A_CHUNG_IMPACT_DETONATION_FINAL );
		}
		else
		{
			StateChange( CSI_A_CHUNG_IMPACT_DETONATION_LOOP );
		}
	}

	CommonEventProcess();
}

void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	XSkinMeshReadyInBackground(L"CHUNG_SCARE_CHASE_Shell.X");
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground( L"Steam_Bp.dds" );
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�. (PlayCount�� 0���� ����)
		if ( m_pXSkinAnim->GetNowPlayCount() < MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE )
		{
			// ĳ������ 1�� �̻� �ְų� ����ȭ ����
			if ( 1 < GetCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				CX2EffectSet::Handle hEffectSetDetonation =
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SA_CHUNG_IMPACT_DETONATION_LOOP", this,
					NULL, false, GetPowerRate() );
				UseCannon( 1 );
			}
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}
	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_EventProcess()
{
	// �ִϸ��̼��� ������
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// �ִ� LOOP ��ŭ �����߰ų�, ����ȭ ��尡 �ƴҶ� ĳ������ 1�� ����������
		if ( MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE <= m_pXSkinAnimFuture->GetNowPlayCount() ||
			( false == IsFullHyperMode() && 1 == GetCannonBallCount() ) )
			StateChange( CSI_A_CHUNG_IMPACT_DETONATION_FINAL );
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		CX2EffectSet::Handle hEffectSetDetonation = INVALID_EFFECTSET_HANDLE;
		// ĳ���� ����� �� ������
		if ( CanUseCannon() )
		{
			hEffectSetDetonation =
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SA_CHUNG_IMPACT_DETONATION_FINAL", this, NULL, false, GetPowerRate() );
			UseCannon( 1 );
		}
		else
		{
			hEffectSetDetonation =
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}

		if( INVALID_EFFECTSET_HANDLE != hEffectSetDetonation )
			m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSetDetonation );
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	CommonEventProcess();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ��ƿ���� ��� �غ�
void CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_CHARGE_Init()
{
	TextureReadyInBackground( L"star2.dds" );
	TextureReadyInBackground( L"Arme_Critical.dds" );
	TextureReadyInBackground( L"Arme_Damage_Center.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ColorBallGray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
	TextureReadyInBackground( L"steam_BP.dds" );
}

void CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_CHARGE_StartFuture()
{
	CommonStateStartFuture();

	/// Ű �Է� ���� ��ü �ʱ�ȭ
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );
}

void CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_CHARGE_FrameMove()
{
	/// �ִϸ��̼� 0.7�� ���ķ� ���� �ð� ����
	const float fAnimationTime = m_pXSkinAnim->GetNowAnimationTime();
	if ( 0.7f < fAnimationTime )
	{
		m_fChargeTime = fAnimationTime - 0.7f;

		/// 1�� �̻� ���� �Ǿ��ٸ�, 1�ʷ� ����
		if ( 1.f < m_fChargeTime )
			m_fChargeTime = 1.f;
	}
	else
		m_fChargeTime = 0;

	CommonFrameMove();
}

void CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_CHARGE_EventProcess()
{
	/// Ű �Է� ���� ��ü���� ������ ���� �Ǿ� �ִ��� �˻�
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	/// �ִϸ��̼��� �����ų�, Ư�� �ð� ���ķ� ��ų Ű�� ������ �� �߻� ������Ʈ�� ��ȯ
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() ||
		 ( 0.7f < m_pXSkinAnimFuture->GetNowAnimationTime() && true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) ) )
		StateChange( CSI_A_CIP_ARTILLERY_NOVA_FIRE );

	CommonEventProcess();
}

// ��ƿ���� ��� �߻�
void CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_FIRE_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.079f ) )
	{
		if ( NULL == g_pData ||
			NULL == g_pData->GetSkillTree() ||
			NULL == GetUnit() )
			return;

		const CX2SkillTree::SkillTemplet *pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_CIP_ARTILLERY_NOVA );

		if ( NULL != pSkillTemplet )
		{
			const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_CIP_ARTILLERY_NOVA );

			/// ���� �ð��� ���� ������ ���� ����
			float fIncreasePowerRate	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevel ) * m_fChargeTime;

			if ( 1.f > fIncreasePowerRate )
				fIncreasePowerRate = 1.f;

			/// ���� �ð��� ���� ��Ÿ� ����
			float fLifeTime				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel ) * m_fChargeTime;

			if ( 0.f > fLifeTime )
				fLifeTime = 0.f;

			D3DXVECTOR3 vFirePos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );

			/// ������ ����Ʈ ����
			CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_A_CIP_ARTILLERY_NOVA",
				GetPowerRate() * fIncreasePowerRate , vFirePos, GetRotateDegree(), GetRotateDegree() );

			UseCannon( 1 );

			/// ���� �ð��� ���� ��Ÿ� ���� ����
			if ( CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = ( pDamageEffect != NULL ) ? pDamageEffect->GetMainEffect() : NULL )
			{
				const float fMainLifeTime = pMeshInstance->GetMaxLifeTime();	/// �޽ÿ� ������ ������ Ÿ��
				pMeshInstance->SetMaxLifeTime( fMainLifeTime * fLifeTime );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_A_CIP_ARTILLERY_NOVA_FIRE_EventProcess()
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
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
///////////////////////////////////////////////////////////////////////////////////////////
// �ǵ���Ͽ� �Ǿ�


void CX2GUChung::CSI_SI_SA_CFG_PANDEMONIUM_FEAR_Init ()
{
	XSkinMeshReadyInBackground( L"CHUNG_BRUTAL_SWING_02.X" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ColorBallGray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
}

void CX2GUChung::CSI_SI_SA_CFG_PANDEMONIUM_FEAR_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.04f );
	CommonFrameMove();
}

void CX2GUChung::CSI_SI_SA_CFG_PANDEMONIUM_FEAR_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );

		m_fCanNotAttackTime = 0.1f;
	} // if
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
/////////////////////////////////////////////////////////////////////////////////////////////
// ���� ������� - � ����ũ

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_StartFuture()
{
	CommonStateStartFuture();
	m_bDisableGravity = false;
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.4f )
		m_bDisableGravity = true;
	CommonFrameMoveFuture();
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.7f );

	CommonFrameMove();
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN );
	}
	CommonEventProcess();
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = false;
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN_EventProcess()
{
	if( true == IsOnSomethingFuture() )
		StateChange( CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING );

	CommonEventProcess();
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_Init()
{
	TextureReadyInBackground( L"soket_earth_quake_smoke01.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds" );
	TextureReadyInBackground( L"soket_earth_quake_stone01.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P02.dds" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M01.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M02.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M03.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M05.X" );
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M06.X" );

	// ������ ���ҽ�
	XMeshReadyInBackground(L"Stone01_AirShip.y");
	TextureReadyInBackground(L"ColorBallBlue.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_FrameMove()
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( 1.15f ) )
	{
		UseCannon( 3 );
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_EventProcess()
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
/////////////////////////////////////////////////////////////////////////////////////////////
// ���� ���̾�
void CX2GUChung::CSI_A_CSG_DOUBLE_FIRE_Init()
{
	XSkinMeshReadyInBackground(L"CSG_Mini_Gun_HEAD_SHOT.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	XMeshReadyInBackground(L"CSG_DUAL_BUSTER_Shell.Y");
	TextureReadyInBackground(L"IceHit_Impact01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Arme_Critical2.dds");
}
void CX2GUChung::CSI_A_CSG_DOUBLE_FIRE_FrameMove()
{

	if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R));
		if( NULL != pMeshInst_MiniGun )
		{
			CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Bip01_R_Hand" );
			pMeshInst_MiniGun->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
			pMeshInst_MiniGun->GetRotateDegree();
		}
	}

	//# �� ����
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_R_Hand" );

		CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun_R = NULL;

		pMeshInst_MiniGun_R = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( this, L"Mesh_CSI_SI_A_CDC_Mini_Gun_LEG_SHOT_Mini_Gun", vBonePosR, GetRotateDegree(), GetRotateDegree() );

		if( NULL != pMeshInst_MiniGun_R )
		{
            pMeshInst_MiniGun_R->SetPerFrameSimulation( true );
			SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R, pMeshInst_MiniGun_R->GetHandle() );
		}
	}

	// �̾� ġ��
    else if ( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		D3DXVECTOR3 vBonePosR = GetBonePos( L"Bip01_R_Hand" );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_CDC_COMBO_X_Mini_Gun_Hit", 1.f, vBonePosR, GetRotateDegree(), GetRotateDegree() );
	}

	//# �� 1�� �߻�
    else if( m_pXSkinAnim->EventTimerOneshot( 0.318f ) )
	{
		//# �ѹ߻� ��ġ�� �޾ƿ��� ����
		D3DXVECTOR3 vBonePos_MiniGun;
		D3DXVECTOR3 vBoneRot_MiniGun = GetRotateDegree();
		vBoneRot_MiniGun.z -= 6;

		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun =
				g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R));
			if( NULL != pMeshInst_MiniGun )
			{
				vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Gun_R_Dummy03");
			}
		}

		CX2DamageEffect::CEffect* pEffect =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_SI_A_CSG_DOUBLE_FIRE",
			GetPowerRate(), vBonePos_MiniGun, vBoneRot_MiniGun, vBoneRot_MiniGun,
			0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );

		if( NULL != pEffect )
		{
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO22 ) == true )
				pEffect->GetDamageData().m_fApplyCriticalDamage = 0.25f;
		}
	}


//# �� 2�� �߻�
    else if( m_pXSkinAnim->EventTimerOneshot( 0.483f ) )
	{
		//# �ѹ߻� ��ġ�� �޾ƿ��� ����
		D3DXVECTOR3 vBonePos_MiniGun;

		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst_MiniGun =
				g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance(GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R));
			if( NULL != pMeshInst_MiniGun )
			{
				vBonePos_MiniGun = pMeshInst_MiniGun->GetXSkinAnim()->GetCloneFramePosition(L"Gun_R_Dummy03");

			}
		}

		CX2DamageEffect::CEffect* pEffect =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_SI_A_CSG_DOUBLE_FIRE",
			GetPowerRate(), vBonePos_MiniGun, GetRotateDegree(), GetRotateDegree(),
			0, false, -1.f, 1.f, GetIncreaseRangeSubWeaponShot() );


		if( NULL != pEffect )
		{
			SetExtraDamageMiniGun( pEffect->GetDamageData(), m_fMiniGunHeadShotRate );
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO22 ) == true )
				pEffect->GetDamageData().m_fApplyCriticalDamage = 0.25f;
		}
	}

//# �� ����
    else if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
			DeleteMajorMesh();

	}

	CommonFrameMove();
}
void CX2GUChung::CSI_A_CSG_DOUBLE_FIRE_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if

	const float LOWER_BOUND_INPUT_TIME_CANCEL = 0.933f;

	if (  LOWER_BOUND_INPUT_TIME_CANCEL <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		else if ( CAN_WALK_CANCEL )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
				DeleteMajorMesh();
			StateChange( USI_WALK );
		}
		else if ( CAN_DASH_CANCEL )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
				DeleteMajorMesh();
			StateChangeDashIfPossible();
		}
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			if( INVALID_MESH_INSTANCE_HANDLE != GetHandleChungMajorMeshByEnum(CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R) )
				DeleteMajorMesh();
			StateChange( USI_WALK );
		}
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	CommonEventProcess();
}
/////////////////////////////////////////////////////////////////////////////////////////////
//

void CX2GUChung::CSI_A_CSG_MAGNUM_SHOT_Init()
{
	XSkinMeshReadyInBackground( L"chung_Elastic_Bome_model.x" );

	TextureReadyInBackground( L"WhitePointSmall.dds" );
	TextureReadyInBackground( L"chung_Elastic_Bome_smoke.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P03.dds" );
	TextureReadyInBackground( L"chung_Elastic_Bome_spark.DDS" );
	TextureReadyInBackground( L"WhiteCircle02.DDS" );
	TextureReadyInBackground( L"Sudden_Buster_P05.dds" );

}


void CX2GUChung::CSI_A_CSG_MAGNUM_SHOT_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
	{
		UseCannon( 3 );
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_A_CSG_MAGNUM_SHOT_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

void CX2GUChung::CSI_A_CDC_RAPID_CANNON_READY_StartFuture()
{
	CommonStateStartFuture();
	// ������̼��� StartFuture (��ǿ����� ���� ����)
	m_bApplyMotionOffset = false;

	/// Ű �Է� ���� ��ü �ʱ�ȭ
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_READY_Start()
{
	CommonStateStart();

	m_iFireCount = 0;	/// �߻� ī��Ʈ �ʱ�ȭ
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_READY_EventProcess()
{
	if ( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// ����ȭ��尡 �ƴϸ� ĳ���� ������ ����
		if ( 1 >= GetTotalCannonBallCount() && false == IsFullHyperMode() )
		{
			StateChange( CSI_A_CDC_RAPID_CANNON_FINAL );
		}
		else
		{
			StateChange( CSI_A_CDC_RAPID_CANNON_LOOP );
		}
	}

	CommonEventProcess();
}

void CX2GUChung::CSI_A_CDC_RAPID_CANNON_LOOP_Init()
{
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect01.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect02.X" );
	XSkinMeshReadyInBackground( L"CHUNG_Cannon_Effect03.X" );
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"Steam_Bp.dds" );
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_LOOP_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_LOOP_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		int iMaxCountAnimationLoopCountAtThisState = _CONST_CHUNG_::MAX_NUMBER_OF_RAPID_CANNON_BULLET;

		// �ִ� �ݺ� Ƚ������ ������ ����ȴ�.
		if ( m_iFireCount <= iMaxCountAnimationLoopCountAtThisState )
		{
			// ĳ������ 1�� �̻� �ְų� ����ȭ ����
			if ( 1 < GetTotalCannonBallCount() || IsFullHyperMode() )
			{
				// ����Ʈ ���
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_A_Gatling_Shot_Loop", this,
					NULL, false, GetPowerRate() );
				UseCannon( 1 );

				++m_iFireCount;
			}
		}
	}

	if ( true == m_pXSkinAnim->IsAnimationEnd() )
	{
        ClearAnimEventTimerOneshotNow();
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_LOOP_EventProcess()
{
	/// Ű �Է� ���� ��ü���� ������ ���� �Ǿ� �ִ��� �˻�
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();


	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		const int iMaxFireNum = _CONST_CHUNG_::MAX_NUMBER_OF_RAPID_CANNON_BULLET - 1;	/// �ִ� �߻� ���Ѽ� ( ������ �߻� ���� )
		const int iMinFireNum = _CONST_CHUNG_::MIN_NUMBER_OF_RAPID_CANNON_BULLET - 1;	/// �ּ� �߻� ���Ѽ� ( ������ �߻� ���� )

		/// �ִ� �߻� ź���� �Ѿ��ų�,
		/// 3������ �ƴѵ� ĳ������ 1�� ���Ұų�,
		/// �ּ� �߻� ź���� �Ѱ����� ��ų Ű �Է��� ���������� ������ ������Ʈ�� ����
		if ( iMaxFireNum <= m_iFireCount ||
			 ( false == IsFullHyperMode() && 1 == GetTotalCannonBallCount() ) ||
			 ( iMinFireNum <= m_iFireCount && true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) ) )
			StateChange( CSI_A_CDC_RAPID_CANNON_FINAL );
	}

	CommonEventProcess();
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_FINAL_StartFuture()
{
	CommonStateStartFuture();
	m_bApplyMotionOffset = false;
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_FINAL_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if ( CanUseCannon() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_A_Gatling_Shot_Final", this, NULL, false, GetPowerRate() );
			UseCannon( 1 );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_SI_A_CHUNG_MISFIRE", this );
		}
	}

	CommonFrameMove();
}
void CX2GUChung::CSI_A_CDC_RAPID_CANNON_FINAL_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}


void CX2GUChung::CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_Init()
{
	XSkinMeshReadyInBackground( L"chung_Elastic_Bome_model.x" );

	TextureReadyInBackground( L"WhitePointSmall.dds" );
	TextureReadyInBackground( L"chung_Elastic_Bome_smoke.dds" );
	TextureReadyInBackground( L"Sudden_Buster_P03.dds" );
	TextureReadyInBackground( L"chung_Elastic_Bome_spark.DDS" );
	TextureReadyInBackground( L"WhiteCircle02.DDS" );
	TextureReadyInBackground( L"Sudden_Buster_P05.dds" );
}


void CX2GUChung::CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.190f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		D3DXVECTOR3 vHandPos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );
		vPos.y = vHandPos.y;

		CX2DamageEffect::CEffect* pDamageEffect = NULL;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO27 ) == true )
			pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CHG_Grenade_Launcher_Shell_MEMO",
				GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
		else
		pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CHG_Grenade_Launcher_Shell",
			GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );

		if( g_pX2Game != NULL && pDamageEffect != NULL )
		{
            g_pX2Game->AddRemoveDamageEffect( pDamageEffect->GetHandle() );

			CKTDGXMeshPlayer::CXMeshInstance *pMesh = pDamageEffect->GetMainEffect();
			if( pMesh != NULL )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CHG_ELASTIC_BOMB_GRENADE );
				if( NULL != pSkillTemplet )
				{

					if ( NULL == GetUnit() )
					return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

					float fLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );

					pMesh->SetMaxLifeTime( pMesh->GetMaxLifeTime() * fLifeTime );
					D3DXVECTOR3 vVelocity = pMesh->GetVelocity();

					vVelocity.x *= 0.6f;
					vVelocity.y *= 1.75f;

					pMesh->SetVelocity(vVelocity );
					pMesh->SetMaxLifeTime( pMesh->GetMaxLifeTime() - fLifeTime );

					fLifeTime += 1.f;
				}
			}
		}
	}
	CommonFrameMove();
}
void CX2GUChung::CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	CommonEventProcess();
}

/*  @function : ��ƿ���� ��Ʈ����ũ ������ �ʱ�ȭ
	@brief : ��ƿ���� ��Ʈ����ũ �������� ���� �ʿ��� �������� �ʱ�ȭ
	@param : ����		// kimjh
*/
void CX2GUChung::ResetArtilleryStrikeData()
{
	m_eArtilleryStrikeType = CAST_NONE;
	m_iArtilleryStrikeNowPlayCount = 0;
	m_bArtilleryStrikeFire = false;
	m_fArtilleryStrikePowerRate	= 0.f;
	// NPC �� 2���� ���� ��찡 �����ϴ� ���� ����, ���� NPC �� �������� �ʵ��� ����
	m_fArtilleryStrikeCreateDamageEffectGapTime = 0.f;
	m_fSummonedLifeTime = 0.f;
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG, 0.f );
	SetSelfDestructSummonedNPC( CX2UnitManager::NUI_TARGETING_CHUNG_NOT_SHOW_OTHER_PLAYER , 0.f );
}

/*  @function : ��ƿ���� ��Ʈ����ũ ����
	@brief : ��ƿ���� ��Ʈ����ũ �������� ���� Chung �� OnFrameMove �� �ش� Process ���� �߰�
	@param : ����		// kimjh
*/
void CX2GUChung::DoArtilleryStrikeFire()
{
	if ( m_eArtilleryStrikeType > CAST_NONE )
	{
		switch ( m_eArtilleryStrikeType )
		{
		case CAST_MISSILE_SHOWER :
			{
				const int MAGIC_BOMB_WIDTH = 800;
				const int MAGIC_BOMB_HEIGHT = 1800;
				// ��ƿ���� ��Ʈ����ũ �߻� �� Frame �� �� �߻��ϴ� ���� ���� �ð� ���� �߻� �ϵ��� ����
				if ( m_fArtilleryStrikeCreateDamageEffectGapTime > _CONST_CHUNG_::GAP_TIME_OF_ARTILLERY_STRIKE_CREATE_DAMAGE_EFFECT_TIME )
				{
					D3DXVECTOR3 vTempPos = m_vLastShotPos;
					vTempPos = vTempPos + m_vLastShotDir * static_cast<float>(GetRandomInt( m_iArtilleryStrikeNowPlayCount ) % ( MAGIC_BOMB_WIDTH / 100 ) * 100 - MAGIC_BOMB_WIDTH / 2);
					// ���� Y �� ��ġ ���� �� ���� �ð� �ʱ�ȭ �߰�
					vTempPos.y += MAGIC_BOMB_HEIGHT - 360.f;
					m_fArtilleryStrikeCreateDamageEffectGapTime = 0.f;
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CDC_CHUNG_ARTILLERY_STRIKE_SHELL_GROUND", m_fArtilleryStrikePowerRate, vTempPos, GetRotateDegree(), GetRotateDegree() );
					m_iArtilleryStrikeNowPlayCount += 1;
				}

				// ���� ��ƿ���� ��Ʈ����ũ �߻� ���� �ٷ� ���� -> ���� �ð� �� ���� ���� ����Ǿ� �ʱ�ȭ ��ġ ����,
				if ( m_iArtilleryStrikePlayCount <= m_iArtilleryStrikeNowPlayCount )
				{
					ResetArtilleryStrikeData();
				}
			}
			break;

		case CAST_QUANTUM_BALLISTA :
			{
				const int MAGIC_BOMB_HEIGHT = 1800;
				D3DXVECTOR3 vTempPos = m_vLastShotPos;
				vTempPos.y += MAGIC_BOMB_HEIGHT - 200.f;
//				g_pX2Game->GetEffectSet()->PlayEffectSet ( L"DAMAGE_EFFECT_CSI_SI_SA_CDC_CHUNG_ARTILLERY_STRIKE_QUANTUM_BALLISTA_GROUND_SHELL", this, NULL, false, GetPowerRate(), -1.f, D3DXVECTOR3 ( 1.f, 1.f, 1.f ), true, vTempPos, GetRotateDegree() );
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_CHUNG_MEMO25 ) == true )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CDC_CHUNG_ARTILLERY_STRIKE_QUANTUM_BALLISTA_GROUND_SHELL_MEMO", m_fArtilleryStrikePowerRate, vTempPos, GetRotateDegree(), GetRotateDegree() );
				else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_CSI_SI_SA_CDC_CHUNG_ARTILLERY_STRIKE_QUANTUM_BALLISTA_GROUND_SHELL", m_fArtilleryStrikePowerRate, vTempPos, GetRotateDegree(), GetRotateDegree() );
				// ���� ��ƿ���� ��Ʈ����ũ �߻� ���� �ٷ� ���� -> ���� �ð� �� ���� ���� ����Ǿ� �ʱ�ȭ ��ġ ����,
				ResetArtilleryStrikeData();
			}
			break;

		default :
			break;
		}
	}
}


/*  @function : ��ƿ���� ��Ʈ����ũ ������ ����
	@brief : ��ƿ���� ��Ʈ����ũ �������� ���� �ʿ��� �����͸� ����
	@param : ��ƿ���� ��Ʈ����ũ Ÿ��, CHUNG_ARTILLERY_STIKE_TYPE  // kimjh
*/
void CX2GUChung::SetArtilleryStrikeData (CHUNG_ARTILLERY_STIKE_TYPE eType_ )
{
	m_eArtilleryStrikeType = eType_;
}


void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Start()
{
	CommonStateStart();
	if ( true == m_bArtilleryStrikeFire )
		DoArtilleryStrikeFire ();		// ��ƿ���� ��Ʈ����ũ Ư�� �� ���� ���� ��Ÿ���� �帣��
										// ��Ÿ�� ���� ������ 2���� ���� ���� �ٷ� ������ �������� ��
										// ������ ���� �� �����Ƿ� �̸� �����س����� ������ �ٷ� �߻��ϰ� �ʱ�ȭ
	SetArtilleryStrikeData ( CAST_QUANTUM_BALLISTA );
	m_bIsSummoningShot = true;
}

void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_RAID_Init()
{
	XSkinMeshReadyInBackground( L"Land_Dimolisher_M05.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.x" );

	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"eve_particleRay_spark.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.DDS" );

	XMeshReadyInBackground(L"CTT_SA_TACTICAL_FIELD_circleField02.Y");
	XMeshReadyInBackground(L"chung_wonderWall_mark.Y");
}


void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_RAID_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( m_pXSkinAnim->EventTimerOneshot( 0.932f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_MESH1_NODAMAGE", GetPowerRate(),
			GetPos(), GetRotateDegree(), GetRotateDegree() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_MESH2_NODAMAGE", GetPowerRate(),
			GetPos(), GetRotateDegree(), GetRotateDegree() );

		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_CTT_TACTICAL_FIELD_RAID );

		if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_RAID_DAMAGE", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree() );

			/// ��Ƽ�� �ʵ� ����
            CX2DamageEffect::CEffect*
			m_pCEffectTacticalField = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"CTT_TACTICAL_FIELD_ZONE", GetPowerRate(),
				GetPos(), GetRotateDegree(), GetRotateDegree() );
            m_hCEffectTacticalField = ( m_pCEffectTacticalField != NULL ) ? m_pCEffectTacticalField->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
			if ( IsMyUnit() && NULL != GetChungGageData() )
			{
				SetCannonBallCount( GetChungGageData()->GetMaxCannonBallCount() );
				SetExtraCannonBallCount( GetChungGageData()->GetMaxCannonBallCountEx() );
			}
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_SA_CTT_TACTICAL_FIELD_RAID_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.6f )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}




void CX2GUChung::CSI_A_CTT_BOMBARD_SERVICE_Init()
{
	TextureReadyInBackground( L"soket_earth_quake_smoke02.DDS");
	TextureReadyInBackground( L"eve_A_blink_light.dds");
	TextureReadyInBackground( L"chung_markOfCommande_mark.DDS");
	TextureReadyInBackground( L"GuideArrow02.DDS");
	TextureReadyInBackground( L"chung_Elastic_Bome_smoke.dds");
	TextureReadyInBackground( L"Sudden_Buster_P03.dds");
	TextureReadyInBackground( L"chung_Elastic_Bome_spark.DDS");
	TextureReadyInBackground( L"Sudden_Buster_P05.dds");
	XSkinMeshReadyInBackground( L"chung_carpetBombing_bigBomb.X" );
}


void CX2GUChung::CSI_A_CTT_BOMBARD_SERVICE_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 1.15f ) )
	{
		float fDistance = 700.f;

		// �ٸ� ��󿡰� �ɷ��־��� ���, �ش� ����Ʈ ���� ����
		if ( m_iBombardServiceTarget >= 0 && m_hBombardServiceEffect != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hBombardServiceEffect );
			m_hBombardServiceEffect = INVALID_EFFECTSET_HANDLE;
		}
		m_iBombardServiceTarget = -1;

		for( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
		{
			CX2GameUnit *pUnit = g_pX2Game->GetUnit( i );

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
				CX2GUNPC *pNpc = (CX2GUNPC*)pUnit;
				if( pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
					continue;

				fOffset = ( pNpc->GetOrgUnitSize().x + pNpc->GetOrgUnitSize().y ) / 4.f;
			}

			D3DXVECTOR3 vDistance = pUnit->GetPos() - GetPos();

			float fDistanceUnit = D3DXVec3Length( &vDistance ) - fOffset;
			if( fDistance >= fDistanceUnit )
			{
				fDistance = fDistanceUnit;
				m_iBombardServiceTarget = i;
			}
		}

		if ( m_iBombardServiceTarget >= 0 )
		{
			CX2GameUnit *pUnit = g_pX2Game->GetUnit( m_iBombardServiceTarget );

			if( pUnit != NULL && g_pX2Game->IsValidUnit( pUnit ) == true )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_CTT_BOMBARD_SERVICE );
				if( NULL != pSkillTemplet )
				{
					if ( NULL != g_pX2Game->GetEffectSet() )
					{
						m_hBombardServiceEffect = g_pX2Game->GetEffectSet()->PlayEffectSet ( L"EffectSet_Chung_BombardServiceHitEnemy", this, pUnit );
					}
				}
			}
		}
	}

	CommonFrameMove();
}

void CX2GUChung::CSI_A_CTT_BOMBARD_SERVICE_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	CommonEventProcess();
}


void CX2GUChung::CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Init()
{
	XSkinMeshReadyInBackground(L"QUANTUM_BALLISTA_BOMB_FIRE_NEW.X");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
	XSkinMeshReadyInBackground(L"NUI_CUTTYSARK_Dying_Landing_Effect_Hole.X");
	XSkinMeshReadyInBackground(L"NUI_CUTTYSARK_Dying_Landing_Effect_Rock.X");
	XSkinMeshReadyInBackground(L"CDC_AS_ROCKON_DUMMY.x");
	XSkinMeshReadyInBackground(L"CDC_AS_SHELL.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_GUNFIRE.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_LIGHT.x");
	XSkinMeshReadyInBackground(L"CDC_AS_WIND.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND01.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND02.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND03.x");
	XMeshReadyInBackground(L"Up_Light01_Mesh.Y");
	XMeshReadyInBackground(L"Blur_Light_Mesh01.y");
	XMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT_Bottom_Broken.Y");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ARTILLERY_STRIKE_QUANTUM_BALLISTA_FIRE.dds");
	TextureReadyInBackground(L"eve_particleRay_spark.dds");
	TextureReadyInBackground(L"soket_earth_quake_smoke02.DDS");
	TextureReadyInBackground(L"WhiteImage.dds");
	TextureReadyInBackground(L"Explosion_Light_Ring01.tga");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"GroundShockWave.dds");
	TextureReadyInBackground(L"Condense_Pulse01.dds");
	TextureReadyInBackground(L"title_bloodAlliance_line_p03.dds");
	TextureReadyInBackground(L"Explosion_Light01.dds");
	TextureReadyInBackground(L"CDC_AS_ROCKON_CENTER.tga");
	TextureReadyInBackground(L"CDC_AS_ROCKON_ROUND1.tga");
	TextureReadyInBackground(L"CDC_AS_ROCKON_ROUND2.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"CDC_AS_ROCKON_ROUND3.tga");
	TextureReadyInBackground(L"steam_BP.dds");
	TextureReadyInBackground(L"GroundShockWave.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"smoke02.dds");
	TextureReadyInBackground(L"Dark_Smoke.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Rush_Impact02.tga");
	TextureReadyInBackground(L"Explosion_Light02.dds");
	TextureReadyInBackground(L"ColorBallBlue.dds");
	TextureReadyInBackground(L"stone.dds");
}





void CX2GUChung::CSI_HA_CTT_SATELITE_RAIN_Init ()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_GUNFIRE.x");
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_LIGHT.x");
	XSkinMeshReadyInBackground(L"CDC_AS_WIND.x");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"Explosion_Sphere_01.dds");
	TextureReadyInBackground(L"GroundShockWave.dds");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"QUANTUM_BALLISTA_BOMB_START.X");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"Sprriggan_Light_Ball_01.Tga");
	XSkinMeshReadyInBackground(L"chung_markOfCommande_redShell.x");
	TextureReadyInBackground(L"steam_BP.dds");
}

void CX2GUChung::CSI_HA_CTT_SATELITE_RAIN_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		ResetSateliteRain();
		m_fSateliteRainPowerRate = GetPowerRate();
		// ��Ʋ����Ʈ ���� ���� ������ �ʱ�ȭ
		if ( NULL != g_pX2Game->GetEffectSet() )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hSateliteRainBigMissileSummonEffect );

			m_hSateliteRainBigMissileSummonEffect = INVALID_EFFECTSET_HANDLE;
		}

	}
    if( m_pXSkinAnim->EventTimerOneshot( 4.094f ) )
	{
        CX2DamageEffect::CEffect* pCEffectSateliteRain =
            g_pX2Game->GetDamageEffect()->CreateInstance(
            this, L"DAMAGE_EFFECT_Satelite_Rain_Summoner", GetPowerRate(),
			GetPos(), GetRotateDegree(), GetRotateDegree() );
        m_hCEffectSateliteRain  = ( pCEffectSateliteRain != NULL ) ? pCEffectSateliteRain->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;

		m_vSateliteRainSummonMagicZonePosition = GetPos();
		m_vSateliteRainSummonMagicZoneRotate = GetRotateDegree();
		m_hSateliteRainBigMissileSummonEffect = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Satelite_Rain_Call_BigThing", this, NULL, IsHyperState(), m_fSateliteRainPowerRate );


		m_bSateliteRainFire = true;
		m_TimerCheckSateliteRain.restart();		//  ��Ʋ����Ʈ ���� ��ŸƮ!
	}


	CommonFrameMove();
}

void CX2GUChung::CSI_HA_CTT_SATELITE_RAIN_EventProcess()
{
	// �߹ؿ� ���θ��� ������
	if( m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_CHUNG_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	CommonEventProcess();
}

//#pragma region SI_HA_CDC_OUTRAGE_STRIKE
void CX2GUChung::CSI_HA_CDC_OUTRAGE_STRIKE_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	XSkinMeshReadyInBackground(L"CDC_MiniGun_L_Start.X");
	XSkinMeshReadyInBackground(L"CDC_MiniGun_R_Start.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect01.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect02.X");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Effect03.X");
	XSkinMeshReadyInBackground(L"Mini_Gun_Shell_Dummy.X");
	TextureReadyInBackground(L"Particle_Blur.dds");
	XSkinMeshReadyInBackground(L"SHOOTING_STAR_Shell_Dummy.X");
	TextureReadyInBackground(L"chung_wonderWall_mark.dds");
	TextureReadyInBackground(L"eve_particleRay_spark.dds");
}
//#pragma endregion �ƿ������� ��Ʈ����ũ : �ñر�


// ��Ʋ����Ʈ ���� ���� �Լ�
void CX2GUChung::DoSateliteRain ()
{
	if ( m_TimerCheckSateliteRain.elapsed() > m_fSateliteRainPhase )
	{
        CX2DamageEffect::CEffect* pCEffectSateliteRain = g_pX2Game->GetDamageEffect()->GetInstance( m_hCEffectSateliteRain );
		if ( CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = ( NULL != pCEffectSateliteRain ) ? pCEffectSateliteRain->GetMainEffect() : NULL )
		{
			if ( false == m_bIsFireSateliteRainBigMissile )
			// ū �̻����� ��ȯ �ƴ°�?
			{
				// ū �̻��� ��ȯ �ð��� �ƴٸ� ��ȯ���� ��ġ �� ȸ������ �޾ƿ�
				// ��ȯ�Ѵ�.
				// ��Ʋ����Ʈ ���� �Ŀ�����Ʈ�� ������ �����̴� �� Ȯ��!

				if ( m_fSateliteRainPhase >= pMeshInstance->GetMaxLifeTime() - _CONST_CHUNG_::SUMMON_TIME_OF_SATELITE_RAIN_BIG_MISSILE_BEFORE_SUMMONER_LIFE_END )
				{
					D3DXVECTOR3 vEffectEndPosition = m_vSateliteRainSummonMagicZonePosition;
					D3DXVECTOR3 vEffectStartPositionOfBigMissile = vEffectEndPosition + D3DXVECTOR3( 0.f , 850.f, 0.f );
					D3DXVECTOR3 vRotation = m_vSateliteRainSummonMagicZoneRotate;
					CX2DamageEffect::CEffect* pSateliteRainEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CTT_Satelite_Rain_Big_Shell", GetPowerRate(), vEffectStartPositionOfBigMissile,
						vRotation, vRotation, vEffectStartPositionOfBigMissile.y );
					if ( NULL != pSateliteRainEffect )
					{
						pSateliteRainEffect->SetPowerRate( m_fSateliteRainPowerRate );
					}
					m_bIsFireSateliteRainBigMissile = true;
				}
			}

			// ������ ��
			m_fSateliteRainPhase = m_fSateliteRainPhase + _CONST_CHUNG_::TICK_COUNT_OF_SATELITE_RAIN_SUMMON_DAMAGE_EFFECT;



			D3DXVECTOR3 vEffectEndPosition = pMeshInstance->GetBonePos( L"Dummy01");
 			D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0.f , 850.f, 0.f );
 			D3DXVECTOR3 vRotation = pCEffectSateliteRain->GetMainEffectAxisAngleDegree();

			for ( int i = 0; i < _CONST_CHUNG_::SATELITE_RAIN_BOMB1_SUMMON_MANY; ++i )
			{
				float fRandomPosX = static_cast<float>( ( GetRandomInt( i * 3 + 1 ) - 50 ) * 10 );
				float fRandomPosY = static_cast<float>( ( GetRandomInt( i * 5 + 1 ) - 50 ) * 10 );
				// -250 ~ 250 ���� ���� ������ ��ġ�� �Ϸ���ƽ ���� �Ѹ���.
				D3DXVECTOR3 vEffectStartPositionOfMissile = vEffectStartPosition + D3DXVECTOR3 (fRandomPosX, fRandomPosY, 0 );

				CX2DamageEffect::CEffect* pSateliteRainEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CTT_Satellite_Rain_Bomb1", GetPowerRate(), vEffectStartPositionOfMissile,
					vRotation, vRotation, vEffectStartPositionOfMissile.y );
				if ( NULL != pSateliteRainEffect )
				{
					pSateliteRainEffect->SetPowerRate( m_fSateliteRainPowerRate );
				}

			}

// 			vRotation.y += 180.f;
			for ( int i = 0; i < _CONST_CHUNG_::SATELITE_RAIN_BOMB2_SUMMON_MANY; ++i )
			{
				float fRandomPosX = static_cast<float>( ( GetRandomInt( i * 2 + 1 ) - 50 ) * 10 );
				float fRandomPosY = static_cast<float>( ( GetRandomInt( i * 4 + 1 ) - 50 ) * 10 );
				// -250 ~ 250 ���� ���� ������ ��ġ�� �Ϸ���ƽ ���� �Ѹ���.
				D3DXVECTOR3 vEffectStartPositionOfMissile = vEffectStartPosition + D3DXVECTOR3 (fRandomPosX, fRandomPosY, 0 );

				CX2DamageEffect::CEffect* pSateliteRainEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CTT_Satellite_Rain_Bomb2", GetPowerRate(), vEffectStartPositionOfMissile,
					vRotation, vRotation, vEffectStartPositionOfMissile.y );
				if ( NULL != pSateliteRainEffect )
				{
					pSateliteRainEffect->SetPowerRate( m_fSateliteRainPowerRate );
				}

			}

			for ( int i = 0; i < _CONST_CHUNG_::SATELITE_RAIN_BOMB3_SUMMON_MANY; ++i )
			{
				float fRandomPosX = static_cast<float>( ( GetRandomInt( i * 1 + 1 ) - 50 ) * 10 );
				float fRandomPosY = static_cast<float>( ( GetRandomInt( i * 6 + 1 ) - 50 ) * 10 );
				// -250 ~ 250 ���� ���� ������ ��ġ�� �Ϸ���ƽ ���� �Ѹ���.
				D3DXVECTOR3 vEffectStartPositionOfMissile = vEffectStartPosition + D3DXVECTOR3 (fRandomPosX, fRandomPosY, 0 );

				CX2DamageEffect::CEffect* pSateliteRainEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CTT_Satellite_Rain_Bomb3", GetPowerRate(), vEffectStartPositionOfMissile,
					vRotation, vRotation, vEffectStartPositionOfMissile.y );
				if ( NULL != pSateliteRainEffect )
				{
					pSateliteRainEffect->SetPowerRate( m_fSateliteRainPowerRate );
				}

			}

			for ( int i = 0; i < _CONST_CHUNG_::SATELITE_RAIN_BOMB4_SUMMON_MANY; ++i )
			{
				float fRandomPosX = static_cast<float>( ( GetRandomInt( i * 7 + 1 ) - 50 ) * 10 );
				float fRandomPosY = static_cast<float>( ( GetRandomInt( i * 8 + 1 ) - 50 ) * 10 );
				// -250 ~ 250 ���� ���� ������ ��ġ�� �Ϸ���ƽ ���� �Ѹ���.
				D3DXVECTOR3 vEffectStartPositionOfMissile = vEffectStartPosition + D3DXVECTOR3 (fRandomPosX, fRandomPosY, 0 );

				CX2DamageEffect::CEffect* pSateliteRainEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_CTT_Satellite_Rain_Bomb4", GetPowerRate(), vEffectStartPositionOfMissile,
					vRotation, vRotation, vEffectStartPositionOfMissile.y );
				if ( NULL != pSateliteRainEffect )
				{
					pSateliteRainEffect->SetPowerRate( m_fSateliteRainPowerRate );
				}

			}

		}
	}
}
void CX2GUChung::ResetSateliteRain()
{
	m_bSateliteRainFire = false;
	m_fSateliteRainPowerRate = 0.f;
	m_fSateliteRainPhase = 0.f;
	m_bIsFireSateliteRainBigMissile = false;
	m_TimerCheckSateliteRain.restart();

}


//#pragma region SI_HA_CIP_DOOM_STRIKER
void CX2GUChung::CSI_HA_CIP_DOOM_STRIKER_Init()
{
	XSkinMeshReadyInBackground(L"CDC_AS_FIRE_EX_WIND01.x");
	XSkinMeshReadyInBackground(L"CHUNG_Cannon_Landing_Effect01.X");
	XSkinMeshReadyInBackground(L"CHUNG_GIGANTIC_IMPACT04.X");
	TextureReadyInBackground(L"COMET_CRASHER_Explosion_02.tga");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	XSkinMeshReadyInBackground(L"ICOOS_BOSS_Magic_Attack_A.X");
	XSkinMeshReadyInBackground(L"LUNATIC_FURY_Broken.X");
	XSkinMeshReadyInBackground(L"LUNATIC_FURY_FireWave.X");
	XSkinMeshReadyInBackground(L"LUNATIC_FURY_FireWave02.X");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"secretVelder_alchemystBoss_magicAttackE_light.dds");
	TextureReadyInBackground(L"stone_AirShip.dds");
}
//#pragma endregion �� ��Ʈ����: �ñر�

// ��Ʋ����Ʈ ����, CommonFrameMove �ʿ� ���� �Լ�
void CX2GUChung::SateliteRainFrameMove()
{
	// ��Ʋ����Ʈ ������ ����ִ°�?
	if ( true == m_bSateliteRainFire )
	{
        CX2DamageEffect::CEffect* pCEffectSateliteRain = g_pX2Game->GetDamageEffect()->GetInstance( m_hCEffectSateliteRain );
        if ( pCEffectSateliteRain != NULL && pCEffectSateliteRain->GetMainEffect() != NULL )
		{
			// ��Ʋ����Ʈ ���� ����
			DoSateliteRain ();
		}

		else	// �ð��� �� �Ǹ� ����Ʈ �� �������ش�.
		{
			ResetSateliteRain();
		}
	}
}





/*virtual*/ void CX2GUChung::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ )
{
	ShowActiveSkillCutInAndLightByBerserkMode( fTimeToShow_, bOnlyLight_ );
}

/** @function : HyperModeFrameMove
	@brief : ���� ���¿��� ǥ���Ǿ��ϴ� ��ƼŬ ��� �� ���� ���� ī��Ʈ ����
*/
void CX2GUChung::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.f, 0.11f, 0.1f, true);
}
void CX2GUChung::CommonHyperModeFrameMove( float fTime1_, float fTime2_, float fTime3_, bool bSound_ /*= false*/  )
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );

    if( true == m_pXSkinAnim->EventTimerOneshot( fTime1_ ) )
	{
		ShowMinorParticleHyperModeChange();
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( fTime2_ ) )
	{
		ShowMinorParticleHyperModeShock();

		UpDownCrashCamera( 30.0f, 0.4f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

		ShowMinorParticleHyperModeTrace();

		ApplyHyperModeBuff();

		if( true == IsSamef(fTime2_, fTime3_ ))
		{
			if ( true == IsFullHyperMode() )
				PlaySound( L"ChungVoice_BoostPower.ogg" );
		}

		if( true == bSound_ )
		{
			PlaySound( L"Energy.ogg" );
			PlaySound( L"Break.ogg" );
			PlaySound( L"Change.ogg" );
		}
	}
    else if ( true == m_pXSkinAnim->EventTimerOneshot( fTime3_ ) )
	{
		if ( true == IsFullHyperMode() )
			PlaySound( L"ChungVoice_BoostPower.ogg" );
	}

	CommonFrameMove();
}


// ī���� ĳ�� �߻� State �� Offset ���� �°� ���� ����
void CX2GUChung::ChaosCannonAttackStateChange ( int iOffset_ )
{
	switch( GetRandomIntFuture(iOffset_) % 4 )
	{
	case 0:
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_02 );
		break;
	case 1:
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_03 );
		break;
	default:
		StateChange( CSI_SI_A_CHG_CHAOS_CANNON_01 );
		break;
	}
}

/*virtual*/ float CX2GUChung::GetActualCoolTime( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN int iSkillLevel ) const
{
	if( NULL == pSkillTemplet_ )
		return 0.f;

	float fSkillCoolTime = CX2GUUser::GetActualCoolTime( pSkillTemplet_, iSkillLevel );

	switch ( pSkillTemplet_->m_eID )
	{

	case CX2SkillTree::SI_SA_CHG_CHAOS_CANNON:
		{
			if( GetEqippedSkillMemo(CX2SkillTree::SMI_CHUNG_MEMO29) == true )
				fSkillCoolTime *= 0.9f;

		} break;

	default:
		break;
	}

	return max( 0.f, fSkillCoolTime );
}
