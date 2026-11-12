#include "stdafx.h"
#include "X2GUAra.h"

/**
	@file	X2GUAra.cpp
	@author	JHKang
	@brief	�ƶ� Ŭ���� ������ ����
	@since  2012-11-06
*/

//#pragma region �ƶ� ���� ��� ����
namespace _CONST_ARA_
{
	const float MAGIC_NUMBER_MULTIPLY_LINE_RADIUS = 1.5f;
	const float MAGIC_MAX_AIRIAL_DASH_COUNT = 1;

	const float			FORCE_SPEAR_MEMO_ADDITIONAL_RANGE_RATE	= 1.33f;				/// ���â �޸�� ���� ��Ÿ� ������ ( ���� ���� �����Ͽ�, 33% ���� )
	const D3DXVECTOR3	GAIN_FORCE_MEMO_ADDITIONAL_RANGE_RATE( 1.35f, 1.35f, 1.35f );	/// ���� �޸�� ���� ������ ����Ʈ ũ�� ������ ( 20% �����ε� Ŀ���ٴ� ������ �� �ʵ�, 1.35��� ���� )

	const float			ENERGY_BULLET_MP_CONSUMPTION			=  4.f;					/// �ƶ� 1�� ���� �Ҹ�, 2�� ���� ���� ��ź MP �Ҹ�
	const int			ENERGY_VOID_MAX_CHARGE_VALUE			=  10;					/// �ƶ� 1�� ���� ���� �������� �ִ� ���� Ƚ��

}
//#pragma endregion namespace

//#pragma region �ʻ�� ��� �� Cut in image
static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] =
{
	{ /// ������, // 1������ ( �Ҽ� )
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_normal.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_Rage.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_Rage.dds"),
	},

	{ /// ��õ
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_SD_normal.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_SD_Rage.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_SD_Rage.dds"),
	},

	{ /// �߸� ����
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_YR_normal.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_YR_Rage.dds"),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Ara_YR_Rage.dds"),
	}

};

//#pragma endregion

//#pragma region ������
/** @function 	: CX2GUAra
	@brief 		: Ara ���� ������ �����Ѵ�.

	@param iUnitIndex_			: ������ �ε��� ��ȣ
	@param iTeamNum_			: �� ��ȣ
	@param pWstrScriptFileName_	: ��ũ��Ʈ ���ϸ�
	@param iFrameBufferNum_		: ������ ���� ��ȣ
	@param pUnit				: Unit ����Ʈ
*/
CX2GUAra::CX2GUAra( int iUnitIndex_, int iTeamNum_, IN lua_State* pScriptLuaState_, int iFrameBufferNum_, IN CX2Unit* pUnit_ )
	: CX2GUUser( iUnitIndex_, iTeamNum_, pScriptLuaState_, iFrameBufferNum_, pUnit_ )
	, m_bBerserkMode( false ), MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE(5)
	, m_fDamageDataChangeTime( 0.f ), m_fDamageDataChangeTime2( 0.f ), m_uiComboXXXLoopCount( 0 ), m_fZpushXChargeTime( 0.f )
	, m_fAirialDashTime( 0.f ), m_usArialCount( 0 ), m_usDoubleJumpCount( 0 ), m_bFPCancel( false )
	, m_usForceGainCount( 0 ), m_usBeforeForceGainCount( 0 ), m_pSkillTemplet( NULL ), m_iPressedSkillSlotIndex( -1 )
	, m_fMPConsume( 0.f )

	, m_fShadowKnotTime( 0.f )
    , m_hShadowKnotWeaponMesh( INVALID_DAMAGE_EFFECT_HANDLE )
	, m_fZZZXChargeTime( 0.f ), m_hPullinPierce( 0 )
	, m_fPullingDragonPowerRate( 1.f )
	, m_bAbleQuickStand( false ) ,m_iGainMpByFpConversion( 0 ), m_fPreRenderParamColorA (1.f), m_fRenderParamColorA( 1.f )
	, m_fBlockingRate( 0.f ), m_fTurningRate( 0.f ), m_bBrutalPiercing( false )
	, m_fReduceEDTDurationRate ( 1.f ), m_hTempEffectSet( INVALID_EFFECTSET_HANDLE )
	, m_vPreTransScale(1,1,1)
	, m_eSkillID( CX2SkillTree::SI_NONE )
	, m_fCurrentSpeed( 1.f ), m_bMoonSlash( false )
	, m_fSuppresionPowerRate( 1.f )
	, m_fUseForcePowerTime( -1.f )
	, m_iUseForcePowerCount( 0 )
	, m_fAddForcePowerTime( -1.f )
	, m_iAddForcePowerCount( 0 )
	, m_fIncreaseEnergeBulletSize( 1.f )
	, m_fIncreaseEnergeBulletDamage( 1.f )
	, m_fIncreaseEnergeBulletRange( 1.f )
	, m_fIncreaseEnergeBulletForceDownValue( 0.f )
	, m_fResurrectionCoolTime( 0.f )
	, m_fHighSpeedValue( 0.f )
	, m_iEnergeVoidChargeValue ( 0 )
    , m_hEnergeVoidDamageEffect( INVALID_DAMAGE_EFFECT_HANDLE )
	, m_bEnergeVoidChargeEnd( false )
	, m_fHowlingDamageRel( 1.0f )
	, m_fHowlingActiveRate( 0.f )
	, m_fOpenTheHellIncreaseDamage( 1.f )
	, m_iWolfBreakeCount( 0 )
//#ifdef FIX_GAIN_FORCE_BUG
	, m_hGainForceIntakeEffect(INVALID_DAMAGE_EFFECT_HANDLE)
	, m_iGainForceDamageTimeNow(0)
//#endif // FIX_GAIN_FORCE_BUG
{
	InitializeAraMajorParticleArray();
	InitializeAraMinorParticleArray();
	InitializeMajorMeshArray();

	m_fThrowStartTime		= 0.666f;
	m_vThrowStartPosOffset	= D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity	= D3DXVECTOR3(1600.f, 200.f, 0.f);

	INIT_VECTOR2( m_vFPCancel, -1.f, -1.f )
}
//#pragma endregion

//#pragma region ������ ���丮
/** @function 	: CreateGUAra
	@brief 		: ������ ���丮
*/
/*static*/ CX2GUAra* CX2GUAra::CreateGUAra( int iUnitIndex_, int iTeamNum_, IN lua_State* pScriptLuaState_,
											int iFrameBufferNum_, IN CX2Unit* pUnit_ )
{
	return new CX2GUAra( iUnitIndex_, iTeamNum_, pScriptLuaState_, iFrameBufferNum_, pUnit_ );
}
//#pragma endregion

//#pragma region �Ҹ���
/** @function 	: �Ҹ���
	@brief 		: �޸� ����
*/
/*virtual*/ CX2GUAra::~CX2GUAra( void )
{
	DeleteAraMajorParticle();
	DeleteAraMinorParticle();
	DeleteMajorMesh();
}
//#pragma endregion

//#pragma region Init
/** @function 	: Init
	@brief 		: �ƶ� ���� ����� �ʱ�ȭ �� ��ũ��Ʈ �Ľ�

	@param buseTeamPos	 : �� ��ġ ��� ����
	@param startPosIndex : ���� ��ġ �ε���
*/
/*virtual*/ void CX2GUAra::Init( bool bUseTeamPos_, int iStartPosIndex_ )
{
	CX2GUUser::Init( bUseTeamPos_, iStartPosIndex_ );

	SetBasicForcePower( 2 );
	LoadPromotionItemID( m_LuaManager );
	InitPromotionEquipList();
	SetFullHyperMode( false );
	SetShowViewList( true );
	SetNeedToUpdateWeaponEffect( true );
	UpdateEquipCollisionData();

	// ��� ���� �ڵ尡 ���� ��
	SetMaxForcePower( min( GetUnitLevel() / 10, 6 ) + 4 );

	if ( true == IsMyUnit() )
		m_FrameDataFuture.syncData.m_CannonBallCount = GetForcePower();
}
//#pragma endregion

//#pragma region ReInit
/** @function 	: ReInit
	@brief 		: ���� ������ ��Ȱ�� � ���� �ٽ� ������ ������ �� ó������� �ϴ� �κ�(�ƶ�)

	@param buseTeamPos	 : �� ��ġ ��� ����
	@param startPosIndex : ���� ��ġ �ε���
*/
/*virtual*/ void CX2GUAra::ReInit( bool bTeamPos_ /* = false */, int iStartPosIndex_ /* = -1 */ )
{
	SetFullHyperMode( false );
	CX2GUUser::ReInit( bTeamPos_, iStartPosIndex_ );

	//#pragma region ��� �ʱ�ȭ
	switch ( g_pX2Game->GetGameType() )
	{
	default:
	case CX2Game::GT_DUNGEON:
		{
			// ���� ���ġ�� �⺻ ���� ���ġ �� �� ����
			if ( GetForcePower() < GetBasicForcePower() )
				SetForcePower( GetBasicForcePower() );
		} break;

	case CX2Game::GT_PVP:
		{
			switch ( GetTeam() )
			{
			case CX2Room::TN_RED:
				{
					ResetForcePowerWhenRebirthInPVPGame( g_pX2Game->GetRedTeamKill(), g_pX2Game->GetBlueTeamKill() );
				} break;

			case CX2Room::TN_BLUE:
				{
					ResetForcePowerWhenRebirthInPVPGame( g_pX2Game->GetBlueTeamKill(), g_pX2Game->GetRedTeamKill() );
				} break;

			default:
				{
					SetForcePower( GetBasicForcePower() );
				} break;
			} // switch ( GetTeam() )
		} // switch ( g_pX2Game->GetGameType() )
	}
	//#pragma endregion switch

	if ( IsMyUnit() )
	{
		// ��� ��ũ ���߱�
		SetMaxForcePower( min( GetUnitLevel() / 10, 6 ) + 4 );
		m_FrameDataFuture.syncData.m_CannonBallCount= GetForcePower();
	}

	/// ���� ȣ�Ű��� ��� ��Ÿ���� ������ �ʾҴٸ�, �ȳ� ����� ����
	if ( 0.f < m_fResurrectionCoolTime )
		SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_SELF_PROTECTION_FORTITUDE );

}
//#pragma endregion

//#pragma region InitStateID
/** @function : InitStateID
	@brief : Ư�� ���µ��� ID�� ����Ŭ������ ��������� ������
*/
/*virtual*/ void CX2GUAra::InitStateID()
{
	m_CommonState.m_Wait	= USI_WAIT;
}
//#pragma endregion

//#pragma region InitState
/** @function	: InitState
	@brief		: �������� ���� �ʱ�ȭ
*/
/*virtual*/ void CX2GUAra::InitState()
{
	switch ( GetUnitClass() )
	{
	case CX2Unit::UC_NONE:
		{
			ASSERT( !L"Unit Class Is fault!" );
			m_iSkillCutInSetIndex = 0;
		} break;

	case CX2Unit::UC_ARA_MARTIAL_ARTIST:
	case CX2Unit::UC_ARA_LITTLE_HSIEN: // �Ҽ� - �ƶ� 1�� ����
	case CX2Unit::UC_ARA_LITTLE_DEVIL: // �Ҹ� - �ƶ� 2_1�� ����
		{
			m_iSkillCutInSetIndex = 0;
		} break;
	case CX2Unit::UC_ARA_SAKRA_DEVANAM: // ��õ - �ƶ� 2�� ����
		{
			m_iSkillCutInSetIndex = 1;
		} break;
	case CX2Unit::UC_ARA_YAMA_RAJA: // ���� - �ƶ� 2_2�� ����
		{
			m_iSkillCutInSetIndex = 2;
		} break;
	default:
		break;
	}

	InitStateID();
	InitStateCommon();
	InitStateByUnitClass();

	std::string strStartTableNameUTF8;
	std::string strWinTableNameUTF8;
	std::string strLoseTableNameUTF8;

	InitStateCommonRandom( strStartTableNameUTF8, strWinTableNameUTF8, strLoseTableNameUTF8 );

	//#pragma region ����
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
	//#pragma endregion ������ ����

	InitPassiveSkillState();
	InitEquippedActiveSkillState();

	CX2GUUser::InitState();
}
//#pragma endregion

//#pragma region InitStateCommon
/** @function	: InitStateCommon
	@brief		: ��� ĳ���Ͱ� ���������� ���� ���� �� ������ ������� �������� ���� ���� �ʱ�ȭ
*/
void CX2GUAra::InitStateCommon()
{
	UserUnitStateData stateData;

	//#pragma region Base State
	//#pragma region ASI_DIE_FRONT
	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DIE_BACK
	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "ASI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DieBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DieBackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DieBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DieBackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_START_INTRUDE
	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "ASI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

//#pragma region ASI_WAIT
	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "ASI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, WaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, WaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, WaitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_HYPER_WAIT
	stateData.Init();
	stateData.stateID			= ASI_HYPER_WAIT;
	m_LuaManager.MakeTableReference( "ASI_HYPER_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, HyperWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, HyperWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, HyperWaitEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, HyperWaitStateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_WALK
	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "ASI_WALK", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, WalkEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_JUMP_READY
	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "ASI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, JumpReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "ASI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, JumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ASI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, JumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ASI_JUMP_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, JumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH
	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "ASI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_END
	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "ASI_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP
	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAra, DashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAra, DashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_HYPER_MODE
	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "ASI_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_BERSERK_MODE
	stateData.Init();
	stateData.stateID			= ASI_BERSERK_MODE;
	m_LuaManager.MakeTableReference( "ASI_BERSERK_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, HyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_GROGGY
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_SMALL_FRONT
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_SMALL_BACK
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_BIG_FRONT
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_BIG_BACK
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DamageBigStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageBigEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_DOWN_FRONT
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DamageDownFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageDownFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_DOWN_BACK
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DamageDownBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageDownBackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_STANDUP_FRONT
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_STANDUP_BACK
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_SMALL
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_DOWN
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_DOWN_INVINCIBLE
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_DOWN_LANDING
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DamageAirDownLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_FALL
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_UP
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_FLY_FRONT
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_AIR_FLY_BACK
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DAMAGE_REVENGE
	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "ASI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_STANDUP_ROLLING_FRONT_FRONT
	stateData.Init();
	stateData.stateID			= ASI_STANDUP_ROLLING_FRONT_FRONT;
	m_LuaManager.MakeTableReference( "ASI_STANDUP_ROLLING_FRONT_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_STANDUP_ROLLING_FRONT_BACK
	stateData.Init();
	stateData.stateID			= ASI_STANDUP_ROLLING_FRONT_BACK;
	m_LuaManager.MakeTableReference( "ASI_STANDUP_ROLLING_FRONT_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_STANDUP_ROLLING_BACK_FRONT
	stateData.Init();
	stateData.stateID			= ASI_STANDUP_ROLLING_BACK_FRONT;
	m_LuaManager.MakeTableReference( "ASI_STANDUP_ROLLING_BACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_STANDUP_ROLLING_BACK_BACK
	stateData.Init();
	stateData.stateID			= ASI_STANDUP_ROLLING_BACK_BACK;
	m_LuaManager.MakeTableReference( "ASI_STANDUP_ROLLING_BACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_STANDUP_ATTACK_FRONT
	stateData.Init();
	stateData.stateID			= ASI_STANDUP_ATTACK_FRONT;
	m_LuaManager.MakeTableReference( "ASI_STANDUP_ATTACK_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpAttackEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_STANDUP_ATTACK_BACK
	stateData.Init();
	stateData.stateID			= ASI_STANDUP_ATTACK_BACK;
	m_LuaManager.MakeTableReference( "ASI_STANDUP_ATTACK_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpAttackEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP_POWER_LANDING
	stateData.Init();
	stateData.stateID			= ASI_DASH_JUMP_POWER_LANDING;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_POWER_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DashJumpPowerLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DashJumpPowerLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpPowerLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_WALL_LANDING
	stateData.Init();
	stateData.stateID			= ASI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "ASI_WALL_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_PEPPER_RUN_READY
	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "ASI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_PEPPER_RUN
	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "ASI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_PEPPER_RUN_END
	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "ASI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_PEPPER_RUN_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "ASI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_PEPPER_RUN_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ASI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_THROW_ITEM
	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "ASI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma endregion �⺻ ���µ�

	//#pragma region ASI_RIDING_START
	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "ASI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_ON
	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "ASI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOnEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_OFF
	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "ASI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_WAIT_HABIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "ASI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_WAIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "ASI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_WALK
	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "ASI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "ASI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "ASI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ASI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DASH
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DASH_END
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DASH_JUMP
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DASH_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DAMAGE_FRONT
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DAMAGE_BACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_DIE
	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "ASI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ASI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_JUMP_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ASI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_ATTACK_X
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "ASI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_SPECIAL_ATTACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "ASI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_RIDING_SPECIAL_MOVE
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "ASI_RIDING_SPECIAL_MOVE", stateData.stateID );
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
	m_LuaManager.MakeTableReference( "ASI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingHyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, RidingHyperModeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;


	//#pragma region Combo State

	//#pragma region ASI_AIRIAL_DASH
	stateData.Init();
	stateData.stateID			= ASI_AIRIAL_DASH;
	m_LuaManager.MakeTableReference( "ASI_AIRIAL_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, AirialDashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, AirialDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAra, AirialDashFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, AirialDashFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, AirialDashEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAra, AirialDashEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DOUBLE_JUMP_FRONT
	stateData.Init();
	stateData.stateID			= ASI_DOUBLE_JUMP_FRONT;
	m_LuaManager.MakeTableReference( "ASI_DOUBLE_JUMP_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DoubleJumpStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DoubleJumpFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DOUBLE_JUMP_UP
	stateData.Init();
	stateData.stateID			= ASI_DOUBLE_JUMP_UP;
	m_LuaManager.MakeTableReference( "ASI_DOUBLE_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, DoubleJumpStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DoubleJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_Z
	stateData.Init();
	stateData.stateID			= ASI_COMBO_Z;
	m_LuaManager.MakeTableReference( "ASI_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZ
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZZ
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZZ", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboZZZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZZEventProcess );
	stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrByStateID( ASI_COMBO_ZZZ );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZZZ
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZZZ;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZZXStart
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZZXStart;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZZXStart", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZZXStartEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZZXCharge
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZZXCharge;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZZXCharge", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ComboZZZXChargeStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboZZZXChargeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZZXChargeEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ComboZZZXChargeStateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZZXAttack
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZZXAttack;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZZXAttack", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboZZZXAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZZXAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ComboZZZXAttackStateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_ZZX
	stateData.Init();
	stateData.stateID			= ASI_COMBO_ZZX;
	m_LuaManager.MakeTableReference( "ASI_COMBO_ZZX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboZZXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_X
	stateData.Init();
	stateData.stateID			= ASI_COMBO_X;
	m_LuaManager.MakeTableReference( "ASI_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_XX
	stateData.Init();
	stateData.stateID			= ASI_COMBO_XX;
	m_LuaManager.MakeTableReference( "ASI_COMBO_XX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_XXX
	stateData.Init();
	stateData.stateID			= ASI_COMBO_XXX;
	m_LuaManager.MakeTableReference( "ASI_COMBO_XXX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ComboXXXStateStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXXXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_XXXloop
	stateData.Init();
	stateData.stateID			= ASI_COMBO_XXXloop;
	m_LuaManager.MakeTableReference( "ASI_COMBO_XXXloop", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ComboXXXloopStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboXXXloopFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXXXloopEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_XXXloopX
	stateData.Init();
	stateData.stateID			= ASI_COMBO_XXXloopX;
	m_LuaManager.MakeTableReference( "ASI_COMBO_XXXloopX", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ComboXXXloopXStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboXXXloopXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXXXloopXEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ComboXXXloopXStateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_XXdownX
	stateData.Init();
	stateData.stateID			= ASI_COMBO_XXdownX;
	m_LuaManager.MakeTableReference( "ASI_COMBO_XXdownX", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboXXdownXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXXdownXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_COMBO_XXdownXX
	stateData.Init();
	stateData.stateID			= ASI_COMBO_XXdownXX;
	m_LuaManager.MakeTableReference( "ASI_COMBO_XXdownXX", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboXXdownXXEventProcess );
	stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrByStateID( ASI_COMBO_XXdownXX );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_COMBO_Z
	stateData.Init();
	stateData.stateID			= ASI_DASH_COMBO_Z;
	m_LuaManager.MakeTableReference( "ASI_DASH_COMBO_Z", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboDashZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboDashZEventProcess );
	stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrByStateID( ASI_DASH_COMBO_Z );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_COMBO_ZpushX_CHARGE
	stateData.Init();
	stateData.stateID			= ASI_DASH_COMBO_ZpushX_CHARGE;
	m_LuaManager.MakeTableReference( "ASI_DASH_COMBO_ZpushX_CHARGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_ChargeStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_ChargeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_ChargeEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_ChargeStateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_COMBO_ZpushX_ATTACK
	stateData.Init();
	stateData.stateID			= ASI_DASH_COMBO_ZpushX_ATTACK;
	m_LuaManager.MakeTableReference( "ASI_DASH_COMBO_ZpushX_ATTACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_AttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_AttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ComboDashZpushX_AttackStateEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_COMBO_X
	stateData.Init();
	stateData.stateID			= ASI_DASH_COMBO_X;
	m_LuaManager.MakeTableReference( "ASI_DASH_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ComboDashXEventProcess );
	stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrByStateID( ASI_DASH_COMBO_X );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_JUMP_ATTACK_Z
	stateData.Init();
	stateData.stateID			= ASI_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "ASI_JUMP_ATTACK_Z", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAra, JumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, JumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_JUMP_ATTACK_X
	stateData.Init();
	stateData.stateID			= ASI_JUMP_ATTACK_X;
	m_LuaManager.MakeTableReference( "ASI_JUMP_ATTACK_X", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUAra, JumpAttackXFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, JumpAttackXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, JumpAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP_COMBO_Z
	stateData.Init();
	stateData.stateID			= ASI_DASH_JUMP_COMBO_Z;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_Z", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpComboZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP_COMBO_ZZ
	stateData.Init();
	stateData.stateID			= ASI_DASH_JUMP_COMBO_ZZ;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_ZZ", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpComboZZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP_COMBO_ZZZ
	stateData.Init();
	stateData.stateID			= ASI_DASH_JUMP_COMBO_ZZZ;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_ZZZ", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, DashJumpComboZZZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpComboZZZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region ASI_DASH_JUMP_COMBO_X
	stateData.Init();
	stateData.stateID			= ASI_DASH_JUMP_COMBO_X;
	m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_X", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, DashJumpComboXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma endregion X, Z �޺� ���µ�
}
//#pragma endregion

//#pragma region InitStateByUnitClass
/** @function	: InitStateByUnitClass
	@brief		: ���� ���� �ٸ� ������Ʋ �ʱ�ȭ
*/
void CX2GUAra::InitStateByUnitClass()
{
	UserUnitStateData stateData;

	switch ( GetUnitClass() )
	{
	default:
	case CX2Unit::UC_ARA_MARTIAL_ARTIST:
		break;
	case CX2Unit::UC_ARA_SAKRA_DEVANAM: // ��õ ���� State case�� (break �߰��ϸ� �ȵ�)
		{
			stateData.Init();
			stateData.stateID			= ASI_DASH_JUMP_COMBO_ASD_ZX;
			m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_ASD_ZX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_ASD_ZX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK;
			m_LuaManager.MakeTableReference( "ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
	case CX2Unit::UC_ARA_LITTLE_HSIEN: // �Ҽ�, ��õ State (case UC_ARA_SAKRA_DEVANAM: break �߰��ϸ� �ȵ�)
		{
			stateData.Init();
			stateData.stateID			= ASI_COMBO_ALH_ZZXX;
			m_LuaManager.MakeTableReference( "ASI_COMBO_ALH_ZZXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_COMBO_ALH_ZZXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_COMBO_ALH_ZZXX_LANDING;
			m_LuaManager.MakeTableReference( "ASI_COMBO_ALH_ZZXX_LANDING", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_COMBO_ALH_ZZXX_LANDING_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_DASH_COMBO_ALH_ZZ;
			m_LuaManager.MakeTableReference( "ASI_DASH_COMBO_ALH_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DASH_COMBO_ALH_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_JUMP_ATTACK_ALH_ZZ;
			m_LuaManager.MakeTableReference( "ASI_JUMP_ATTACK_ALH_ZZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_JUMP_ATTACK_ALH_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			//����� ���
			stateData.Init();
			stateData.stateID			= ASI_P_ALH_QUICK_STAND_FRONT_FRONT;
			m_LuaManager.MakeTableReference( "ASI_P_ALH_QUICK_STAND_FRONT_FRONT", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_P_ALH_QUICK_STAND_FRONT_BACK;
			m_LuaManager.MakeTableReference( "ASI_P_ALH_QUICK_STAND_FRONT_BACK", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_P_ALH_QUICK_STAND_BACK_FRONT;
			m_LuaManager.MakeTableReference( "ASI_P_ALH_QUICK_STAND_BACK_FRONT", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_P_ALH_QUICK_STAND_BACK_BACK;
			m_LuaManager.MakeTableReference( "ASI_P_ALH_QUICK_STAND_BACK_BACK", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
			m_StateList[stateData.stateID] = stateData;

		//#pragma region �����̴� ����
			stateData.Init();
			stateData.stateID			= ASI_P_ASD_QUICKEN_GUARD;
			m_LuaManager.MakeTableReference( "ASI_P_ASD_QUICKEN_GUARD", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_P_ASD_QUICKEN_GUARD_StateStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_P_ASD_QUICKEN_GUARD_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_P_ASD_QUICKEN_GUARD_SMALL;
			m_LuaManager.MakeTableReference( "ASI_P_ASD_QUICKEN_GUARD_SMALL", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_P_ASD_QUICKEN_GUARD_StateStart );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_P_ASD_QUICKEN_GUARD_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

			stateData.Init();
			stateData.stateID			= ASI_STANDUP_ATTACK_FRONT_NO_DOWN;
			m_LuaManager.MakeTableReference( "ASI_STANDUP_ATTACK_FRONT_NO_DOWN", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpAttackEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpAttackEnd );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ASI_STANDUP_ATTACK_BACK_NO_DOWN;
			m_LuaManager.MakeTableReference( "ASI_STANDUP_ATTACK_BACK_NO_DOWN", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpAttackEventProcess );
			stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpAttackEnd );
			m_StateList[stateData.stateID] = stateData;

		} break;

	case CX2Unit::UC_ARA_YAMA_RAJA:
		{

		//#pragma region ASI_DASH_JUMP_COMBO_AYR_XZ
			stateData.Init();
			stateData.stateID			= ASI_DASH_JUMP_COMBO_AYR_XZ;
			m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_AYR_XZ", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_AYR_XZ_StateStartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_AYR_XZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_AYR_XZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

		//#pragma region ASI_DASH_COMBO_AYR_ZZ
			stateData.Init();
			stateData.stateID			= ASI_DASH_COMBO_AYR_ZZ;
			m_LuaManager.MakeTableReference( "ASI_DASH_COMBO_AYR_ZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_DASH_COMBO_AYR_ZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DASH_COMBO_AYR_ZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

		}
	case CX2Unit::UC_ARA_LITTLE_DEVIL:
		{

		//#pragma region ASI_COMBO_ALD_ZZXX
			stateData.Init();
			stateData.stateID			= ASI_COMBO_ALD_ZZXX;
			m_LuaManager.MakeTableReference( "ASI_COMBO_ALD_ZZXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_COMBO_ALD_ZZXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_COMBO_ALD_ZZXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

		//#pragma region ASI_COMBO_ALD_ZZZFrontZ
			stateData.Init();
			stateData.stateID			= ASI_COMBO_ALD_ZZZFrontZ;
			m_LuaManager.MakeTableReference( "ASI_COMBO_ALD_ZZZFrontZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_COMBO_ALD_ZZZFrontZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_COMBO_ALD_ZZZFrontZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

		//#pragma region ASI_DASH_JUMP_COMBO_ALD_ZX
			stateData.Init();
			stateData.stateID			= ASI_DASH_JUMP_COMBO_ALD_ZX;
			m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_ALD_ZX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_ALD_ZX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_ALD_ZX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

		//#pragma region ASI_DASH_JUMP_COMBO_ALD_ZXX
			stateData.Init();
			stateData.stateID			= ASI_DASH_JUMP_COMBO_ALD_ZXX;
			m_LuaManager.MakeTableReference( "ASI_DASH_JUMP_COMBO_ALD_ZXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_ALD_ZXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_DASH_JUMP_COMBO_ALD_ZXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		//#pragma endregion

		} break;
	}
}
//#pragma endregion

//#pragma region InitPassiveSkillState
/** @function	: InitPassiveSkillState
	@brief		: ������ �нú� ��ų�� ���� �ʱ�ȭ
*/
/*virtual*/ void CX2GUAra::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();

	/// AttackResult�� �߰��� �Լ��� ����
	SetAttackResultDelegate();

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	const CX2SkillTree* const pSkillTree = g_pData->GetSkillTree();

	//#pragma region ���â ����
	//#pragma endregion

	switch( GetUnitClass() )
	{
	case CX2Unit::UC_ARA_MARTIAL_ARTIST:
		break;
	case CX2Unit::UC_ARA_SAKRA_DEVANAM: // ��õ ���� State case�� (break �߰��ϸ� �ȵ�)
		{
			//#pragma region �����̴� ����
			const int iSkillLevelQuickenGuard = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ASD_QUICKEN_GUARD, true );

			m_fTurningRate = m_fBlockingRate = 0.f;
			if ( 0 < iSkillLevelQuickenGuard )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ASD_QUICKEN_GUARD );

				if( NULL != pSkillTemplet )
				{
					m_fTurningRate = m_fBlockingRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelQuickenGuard );
				}
			}
			//#pragma endregion

			//#pragma region â���� ����
			const int iSkillLevelBrutalPiercing = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ASD_BRUTAL_PIERCING, true );

			m_bBrutalPiercing = false;
			if ( 0 < iSkillLevelBrutalPiercing )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ASD_BRUTAL_PIERCING );

				if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
				{
					m_pBrutalPiercingBuffFactor = pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillLevelBrutalPiercing );

					if ( NULL != m_pBrutalPiercingBuffFactor )
					{
						if ( g_pX2Game != NULL && ( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON || g_pX2Game->GetGameType() == CX2Game::GT_BATTLE_FIELD ) )
							m_pBrutalPiercingBuffFactor->SetRate( pSkillTemplet->m_vecBuffFactorPtr[0]->GetRate( iSkillLevelBrutalPiercing) * 2.f, iSkillLevelBrutalPiercing );

						m_bBrutalPiercing = true;
					}
				}
			}
			//#pragma endregion

			//#pragma region SI_P_ASD_FULLMOON_SLASH
			const int iSkillLevelFullMoonSlash = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ASD_FULLMOON_SLASH, true );
			m_fCurrentSpeed = 1.f;
			m_bMoonSlash = false;
			if ( 0 < iSkillLevelFullMoonSlash )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ASD_FULLMOON_SLASH );

				if( NULL != pSkillTemplet )
				{
					m_fCurrentSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ANI_SPEED, iSkillLevelFullMoonSlash );
					m_bMoonSlash = true;
				}
			}
			//#pragma endregion ���� ����

			//#pragma region SI_P_ALH_QUICK_STAND
			int iSkillLevelQuickStand = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_QUICK_STAND, true );
			if( iSkillLevelQuickStand > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ALH_QUICK_STAND );
				if( NULL != pSkillTemplet )
				{
					const float fMoveSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevelQuickStand );
					m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * fMoveSpeed );
					m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * fMoveSpeed );

				}
			}
			//#pragma endregion ����� ���
		}

	case CX2Unit::UC_ARA_LITTLE_HSIEN:  // �Ҽ�, ��õ State (case UC_ARA_SAKRA_DEVANAM: break �߰��ϸ� �ȵ�)
		{
			int iSkillLevel = 0;

			//�游�� ��
			m_iGainMpByFpConversion = 0;
			iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_FILLED_POWER, true );

			if( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ALH_FILLED_POWER );

				if( NULL != pSkillTemplet )
				{
					m_iGainMpByFpConversion = static_cast<int>(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevel ) );
				}
			}

			//��ȭ����

			SetCanAvoidance( (0 < userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ALH_BREAKING )) ? true : false );

			//#pragma region ��� Ȱ��ȭ
			const int iSkillLevelVigorEnergize = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_VIGOR_ENERGIZE, true );

			if ( 0 < iSkillLevelVigorEnergize )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ALH_VIGOR_ENERGIZE  );

				if( NULL != pSkillTemplet )
				{
					// ����� ���ӽð� ����
					m_fReduceEDTDurationRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_REDUCE_EDT_TIME_REL, iSkillLevelVigorEnergize );
				}
			}
			//#pragma endregion
		}
		break;

		case CX2Unit::UC_ARA_YAMA_RAJA:
			{
				//#pragma region ������ ���¢��
				const int iSkillLevelHowling = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AYR_HOWLING, true );

				m_fHowlingDamageRel 	= 1.0f;
				m_fHowlingActiveRate	= 0.f;

				if ( 0 < iSkillLevelHowling )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AYR_HOWLING );

					if( NULL != pSkillTemplet )
					{

						/// ������ ���¢�� ���� ������ ����
						/// �Ʒ� ������ �����ؼ� �� ���� ���� ���� (���� SkillTemplet�� �ش� Ű���尡 �߰����� �ʾƼ� �ӽ÷� �̷��� ����)
						const float fDamageRel = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevelHowling );
						m_fHowlingDamageRel		= ( fDamageRel > 1.0f ? fDamageRel : 1.0f );
						/// ������ ���¢�� �ߵ� Ȯ��
						m_fHowlingActiveRate	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevelHowling );
					}
				}
				//#pragma endregion
			}

		case CX2Unit::UC_ARA_LITTLE_DEVIL:	// �Ҹ�, ���� State (case UC_ARA_YAMA_RAJA: break �߰��ϸ� �ȵ�)
			{
				//#pragma region ��ź ��ȭ
				const int iSkillLevelEnergyBulletReinforce = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AYR_ENERGY_BULLET_REINFORCE, true );

				m_fIncreaseEnergeBulletSize		= 1.f;		/// ��ź ũ��
				m_fIncreaseEnergeBulletDamage	= 1.f;		/// ��ź ������

				if ( 0 < iSkillLevelEnergyBulletReinforce )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AYR_ENERGY_BULLET_REINFORCE );

					if( NULL != pSkillTemplet )
					{
						/// ��ź ũ�� ����
						m_fIncreaseEnergeBulletSize		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_SIZE, iSkillLevelEnergyBulletReinforce );
						/// ��ź ������ ����
						m_fIncreaseEnergeBulletDamage	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevelEnergyBulletReinforce );
					}
				}
				//#pragma endregion

				//#pragma region ��� ��ȭ
				const int iSkillLevelEnergeReinforce = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALD_ENERGY_REINFORCE, true );

				m_fIncreaseEnergeBulletRange			= 1.f;	/// ��ź ��Ÿ�
				m_fIncreaseEnergeBulletForceDownValue	= 0.f;	/// ��ź �ٿ� ��ġ


				if ( 0 < iSkillLevelEnergeReinforce )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ALD_ENERGY_REINFORCE );

					if( NULL != pSkillTemplet )
					{
						/// ��ź ��Ÿ� ����
						m_fIncreaseEnergeBulletRange			= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iSkillLevelEnergeReinforce );
						/// ��ź �ٿ� ��ġ ����
						m_fIncreaseEnergeBulletForceDownValue	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_ABS, iSkillLevelEnergeReinforce );
					}
				}
				//#pragma endregion

				//#pragma region ���� �ʷ�
				const int iSkillLevelHellGateOpen = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AYR_HELL_GATE_OPEN, true );

				m_fOpenTheHellIncreaseDamage = 1.f;		/// ���ݷ� ����ġ

				if ( 0 < iSkillLevelHellGateOpen )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AYR_HELL_GATE_OPEN );

					if( NULL != pSkillTemplet )
					{
						/// ���ݷ� ���� ����
						m_fOpenTheHellIncreaseDamage	= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGE_REL, iSkillLevelHellGateOpen );
					}
				}
				//#pragma endregion
			} break;
	default:
		break;
	}



	/// ���� ���� ��� ��ų
//#pragma region ����� ���
	m_bAbleQuickStand = false;
	int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALH_QUICK_STAND, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ALH_QUICK_STAND );

		if( NULL != pSkillTemplet )
		{
			m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent +=
				static_cast<int>(pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel ) * 100.f );
		}
		m_bAbleQuickStand = true;
	}
//#pragma endregion




}
//#pragma endregion

//#pragma region SetEquippedSkillFunc
/** @function	: SetEquippedSkillFunc
	@brief		: ���Կ� ������ ��ų�� ���� �ش� ��ų ������Ʈ �ʱ�ȭ
*/
/*virtual*/ void CX2GUAra::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_,
																 const bool bSlotB_ )
{
	if ( iSlotIndex_ < 0 || static_cast<size_t>( iSlotIndex_ ) >= ARRAY_SIZE( _CONST_GUUSER_::aNormalSkillStateID ) )
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
	//#pragma region ���� ��ų
	case CX2SkillTree::ST_BUFF:
		{	// ���� �ʻ��
			m_LuaManager.MakeTableReference( "ASI_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_COMMON_BUFF", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

			return;
		} break;
	//#pragma endregion
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
					m_LuaManager.MakeTableReference( "ASI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "ASI_THROW_ITEM", hyperStateData.stateID );
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

	//#pragma region ACTIvE, SPECIAL ACTIvE ��ų
	switch ( eSkillID_ )
	{
	default:
	//#pragma region SI_SA_ARA_WIND_WEDGE
		case CX2SkillTree::SI_A_AM_WIND_WEDGE:
		{
			ASI_SA_ARA_WIND_WEDGE_START_CreateState( normalStateData, normalStateData.stateID );
			ASI_SA_ARA_WIND_WEDGE_START_CreateState( hyperStateData, hyperStateData.stateID );

			UserUnitStateData	subStateData;
			ASI_SA_ARA_WIND_WEDGE_END_CreateState( subStateData, ASI_SA_ARA_WIND_WEDGE_END );
			m_StateList[subStateData.stateID] = subStateData;
		} break;
	//#pragma endregion

	//#pragma region SI_SA_ARA_SHADOW_RISING
		case CX2SkillTree::SI_A_AM_SHADOW_RISING:
		{
			ASI_SA_ARA_SHADOW_RISING_JUMP_CreateState( normalStateData, normalStateData.stateID );
			ASI_SA_ARA_SHADOW_RISING_JUMP_CreateState( hyperStateData, hyperStateData.stateID );

			{
				UserUnitStateData	subStateData;
				ASI_SA_ARA_SHADOW_RISING_LANDING_CreateState( subStateData, ASI_SA_ARA_SHADOW_RISING_LANDING );
				m_StateList[subStateData.stateID] = subStateData;
			}

			{
				UserUnitStateData	subStateData;
				ASI_SA_ARA_SHADOW_RISING_END_CreateState( subStateData, ASI_SA_ARA_SHADOW_RISING_END );
				m_StateList[subStateData.stateID] = subStateData;
			}

		} break;
	//#pragma endregion

	//#pragma region SI_SA_ARA_PULLING_PIERCE
		case CX2SkillTree::SI_SA_AM_PULLING_PIERCE:
		{
			ASI_SA_ARA_PULLING_PIERCE_START_CreateState( normalStateData, normalStateData.stateID );
			ASI_SA_ARA_PULLING_PIERCE_START_CreateState( hyperStateData, hyperStateData.stateID );

			{
				UserUnitStateData	subStateData;
				ASI_SA_ARA_PULLING_PIERCE_END_CreateState( subStateData, ASI_SA_ARA_PULLING_PIERCE_END );
				m_StateList[subStateData.stateID] = subStateData;
			}
		} break;
	//#pragma endregion

	//#pragma region SI_SA_ARA_TEMPEST_DANCE
		case CX2SkillTree::SI_SA_AM_TEMPEST_DANCE:
		{
			m_LuaManager.MakeTableReference( "ASI_SA_ARA_TEMPEST_DANCE_START", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_TEMPEST_DANCE_START_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_TEMPEST_DANCE_START_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_TEMPEST_DANCE_START_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_SA_ARA_TEMPEST_DANCE_START", hyperStateData.stateID );
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ASI_SA_ARA_TEMPEST_DANCE_END;
				m_LuaManager.MakeTableReference( "ASI_SA_ARA_TEMPEST_DANCE_END", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_TEMPEST_DANCE_END_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_TEMPEST_DANCE_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_TEMPEST_DANCE_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;

			}
		} break;
	//#pragma endregion

	//#pragma region SI_SA_ARA_FALLING_DRAGON
		case CX2SkillTree::SI_SA_AM_FALLING_DRAGON:
		{
			m_LuaManager.MakeTableReference( "ASI_SA_ARA_FALLING_DRAGON", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_FALLING_DRAGON_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_FALLING_DRAGON_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_FALLING_DRAGON_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_SA_ARA_FALLING_DRAGON", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateInit;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion

	//#pragma region SI_SA_ARA_SHADOW_KNOT
		case CX2SkillTree::SI_SA_AM_SHADOW_KNOT:
		{
			m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_KNOT_START", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_START_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_START_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_START_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_START_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_KNOT_START", hyperStateData.stateID );
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ASI_SA_ARA_SHADOW_KNOT_WAIT;
				m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_KNOT_WAIT", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_WAIT_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_WAIT_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_WAIT_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_WAIT_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ASI_SA_ARA_SHADOW_KNOT_CATCH;
				m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_KNOT_CATCH", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_CATCH_Init );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_CATCH_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ASI_SA_ARA_SHADOW_KNOT_END;
				m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_KNOT_END", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_END_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_END_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_KNOT_END_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	//#pragma endregion
		case CX2SkillTree::SI_A_ALH_DOUBLE_COLLISION: //��� 3�� - �ȱ���
		{
			ASI_SA_ALH_DOUBLE_COLLISION_START_CreateState( normalStateData, normalStateData.stateID );
			ASI_SA_ALH_DOUBLE_COLLISION_START_CreateState( hyperStateData, hyperStateData.stateID );

			{
				UserUnitStateData	subStateData;

				ASI_SA_ALH_DOUBLE_COLLISION_END_CreateState( subStateData, ASI_SA_ALH_DOUBLE_COLLISION_END );
				m_StateList[subStateData.stateID] = subStateData;
			}
		} break;

	//#pragma region SI_SA_ASD_TURBULENT_WAVE
		case CX2SkillTree::SI_A_ASD_TURBULENT_WAVE: // ��ȣ 4�� - �뵵
		{
			ASI_SA_ASD_TURBULENT_WAVE_START_CreateState( normalStateData, normalStateData.stateID );
			ASI_SA_ASD_TURBULENT_WAVE_START_CreateState( hyperStateData, hyperStateData.stateID );

			{
				UserUnitStateData subStateData;
				ASI_SA_ASD_TURBULENT_WAVE_LOOP_CreateState( subStateData, ASI_SA_ASD_TURBULENT_WAVE_LOOP );
				m_StateList[subStateData.stateID] = subStateData;
			}

			{
				UserUnitStateData subStateData;
				ASI_SA_ASD_TURBULENT_WAVE_END_CreateState( subStateData, ASI_SA_ASD_TURBULENT_WAVE_END );
				m_StateList[subStateData.stateID] = subStateData;
			}
		} break;
	//#pragma endregion

	//#pragma region SI_SA_ASD_SUPPRESSION
	case CX2SkillTree::SI_SA_ASD_SUPPRESSION: // �ƶ� 2�� ���� ��õ - ����
		{
			m_LuaManager.MakeTableReference( "ASI_SA_ASD_SUPPRESSION", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_SUPPRESSION_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_SUPPRESSION_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_SUPPRESSION_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_SUPPRESSION_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_SA_ASD_SUPPRESSION", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateInit;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion

	//#pragma region SI_SA_ALH_MOONLIGHT_SLASH
	case CX2SkillTree::SI_SA_ALH_MOONLIGHT_SLASH:	// �޺� ����
		{
			ASI_SA_ALH_MOONLIGHT_SLASH_READY_CreateState( normalStateData, normalStateData.stateID );
			ASI_SA_ALH_MOONLIGHT_SLASH_READY_CreateState( hyperStateData, hyperStateData.stateID );

			{
				UserUnitStateData subStateData;
				ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_CreateState( subStateData, ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE );
				m_StateList[subStateData.stateID] = subStateData;
			}

			{
				UserUnitStateData subStateData;
				ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_CreateState( subStateData, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK );
				m_StateList[subStateData.stateID] = subStateData;
			}

			{
				UserUnitStateData subStateData;
				ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_CreateState( subStateData, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK );
				m_StateList[subStateData.stateID] = subStateData;
			}

		} break;
	//#pragma endregion

		case CX2SkillTree::SI_SA_ALH_WHITE_TIGER: // ��ȣ��
		{
			m_LuaManager.MakeTableReference( "ASI_SA_ALH_POWER_OF_WHITER_TIGER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_POWER_OF_WHITER_TIGER_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_POWER_OF_WHITER_TIGER_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_POWER_OF_WHITER_TIGER_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_POWER_OF_WHITER_TIGER_End );

			m_LuaManager.MakeTableReference( "ASI_SA_ALH_POWER_OF_WHITER_TIGER", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateInit;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING;
				m_LuaManager.MakeTableReference( "ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
		case CX2SkillTree::SI_SA_ALH_PEERLESSNESS_SLASH: // ������
			{
				m_LuaManager.MakeTableReference( "ASI_SA_ALH_PEERLESSNESS_SLASH_START", normalStateData.stateID );
				normalStateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_START_Start );
				normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_START_Init );
				normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_START_FrameMove );
				normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_START_EventProcess );
				normalStateData.StateEnd		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_START_End );

				m_LuaManager.MakeTableReference( "ASI_SA_ALH_PEERLESSNESS_SLASH_START", hyperStateData.stateID );
				hyperStateData.stateID			= normalStateData.stateID;
				hyperStateData.StateStart		= normalStateData.StateStart;
				hyperStateData.StateInit		= normalStateData.StateInit;
				hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
				hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
				hyperStateData.StateEnd			= normalStateData.StateEnd;

				{
					UserUnitStateData stateData;

					stateData.Init();
					stateData.stateID			= ASI_SA_ALH_PEERLESSNESS_SLASH_END;
					m_LuaManager.MakeTableReference( "ASI_SA_ALH_PEERLESSNESS_SLASH_END", stateData.stateID );
					stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_END_FrameMove );
					stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_PEERLESSNESS_SLASH_END_EventProcess );
					m_StateList[stateData.stateID] = stateData;
				}
			} break;

	//#pragma region SI_SA_ALH_SWALLOW_ASSAULT
		case CX2SkillTree::SI_SA_ALH_SWALLOW_ASSAULT: // ��
			{
				m_LuaManager.MakeTableReference( "ASI_SA_ALH_SWALLOW_ASSAULT_READY", normalStateData.stateID );
				normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Ready_Init );
				normalStateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateStart );
				normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Ready_FrameMove );
				normalStateData.StateEnd		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateEnd );
				normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Ready_EventProcess );

				m_LuaManager.MakeTableReference( "ASI_SA_ALH_SWALLOW_ASSAULT_READY", hyperStateData.stateID );
				hyperStateData.StateInit		= normalStateData.StateInit;
				hyperStateData.StateStart		= normalStateData.StateStart;
				hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
				hyperStateData.StateEnd			= normalStateData.StateEnd;
				hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

				{
					UserUnitStateData stateData;

					stateData.Init();
					stateData.stateID			= ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK;
					m_LuaManager.MakeTableReference( "ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK", stateData.stateID );
					stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Attack_Init );
					stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateStart );
					stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Attack_FrameMove );
					stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Attack_EventProcess );
					stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateEnd );
					m_StateList[stateData.stateID] = stateData;

					stateData.Init();
					stateData.stateID			= ASI_SA_ALH_SWALLOW_ASSAULT_FINISH;
					m_LuaManager.MakeTableReference( "ASI_SA_ALH_SWALLOW_ASSAULT_FINISH", stateData.stateID );
					stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Finish_Init );
					stateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Finish_StateStart );
					stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Finish_FrameMove );
					stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Finish_EventProcess );
					m_StateList[stateData.stateID] = stateData;

					stateData.Init();
					stateData.stateID			= ASI_SA_ALH_SWALLOW_ASSAULT_ESCAPE;
					m_LuaManager.MakeTableReference( "ASI_SA_ALH_SWALLOW_ASSAULT_ESCAPE", stateData.stateID );
					stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_SWALLOW_ASSAULT_Escape_EventProcess );
					m_StateList[stateData.stateID] = stateData;
				}


				m_sLockOnSystemPtr = SLockOnSystem::CreateLockOnSystem();
			} break;
	//#pragma endregion

		//#pragma region SI_HA_ASD_THOUSANDS_BLOSSOMS
		case CX2SkillTree::SI_HA_ASD_THOUSANDS_BLOSSOMS: //�ƶ� ��õ �ñر� - �б� ��õ
		{
			m_LuaManager.MakeTableReference( "ASI_HA_ASD_THOUSANDS_BLOSSOMS", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_HA_ASD_THOUSANDS_BLOSSOMS_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_HA_ASD_THOUSANDS_BLOSSOMS_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_HA_ASD_THOUSANDS_BLOSSOMS", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateInit;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
		//#pragma endregion

	//#pragma region SI_A_ARA_TIGER_CLAW
			case CX2SkillTree::SI_A_AM_TIGER_CLAW:
		{
			ASI_A_ARA_TIGER_CLAW_START_CreateState( normalStateData, normalStateData.stateID );
			ASI_A_ARA_TIGER_CLAW_START_CreateState( hyperStateData, hyperStateData.stateID );

			UserUnitStateData subStateData;
			ASI_A_ARA_TIGER_CLAW_END_CreateState( subStateData, ASI_A_ARA_TIGER_CLAW_END );
			m_StateList[subStateData.stateID] = subStateData;
		} break;
	//#pragma endregion

	//#pragma region SI_A_ARA_GAIN_FORCE
		case CX2SkillTree::SI_A_AM_GAIN_FORCE:
		{
			m_LuaManager.MakeTableReference( "ASI_A_ARA_GAIN_FORCE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_GAIN_FORCE_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_GAIN_FORCE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_GAIN_FORCE_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_A_ARA_GAIN_FORCE", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateInit;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion

	//#pragma region SI_A_ARA_FORCE_SPEAR
		case CX2SkillTree::SI_A_AM_FORCE_SPEAR:
		{
			m_LuaManager.MakeTableReference( "ASI_A_ARA_FORCE_SPEAR", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_FORCE_SPEAR_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_FORCE_SPEAR_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_FORCE_SPEAR_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_A_ARA_FORCE_SPEAR", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateStartFuture;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion

	case CX2SkillTree::SI_A_ALH_BREAKING: //��ȭ����
		{
			m_LuaManager.MakeTableReference( "ASI_A_ALH_BREAKING", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_A_ALH_BREAKING", hyperStateData.stateID );
			hyperStateData.StateInit		= normalStateData.StateInit;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ASI_A_ALH_BREAKING_ATTACK;
				m_LuaManager.MakeTableReference( "ASI_A_ALH_BREAKING_ATTACK", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_ATTACK_Init );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_ATTACK_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_ATTACK_EventProcess );
				stateData.StateEndFuture	= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_BREAKING_ATTACK_EndFuture );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= ASI_A_ALH_BREAKING_AVOIDANCE;
				m_LuaManager.MakeTableReference( "ASI_A_ALH_BREAKING_AVOIDANCE", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, StandUpRollingEventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUAra, StandUpRollingEnd );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;

	//#pragma region SI_A_ALH_ROCK_CRASH
	case CX2SkillTree::SI_A_ALH_ROCK_CRASH:
		{
			ASI_A_ALH_ROCK_CRASH_FALLING_START_CreateState( normalStateData, normalStateData.stateID );
			ASI_A_ALH_ROCK_CRASH_FALLING_START_CreateState( hyperStateData, hyperStateData.stateID );

			UserUnitStateData subStateData;
			ASI_A_ALH_ROCK_CRASH_END_CreateState( subStateData, ASI_A_ALH_ROCK_CRASH_END );
			m_StateList[subStateData.stateID] = subStateData;

		} break;
	//#pragma endregion ���� ����

	//#pragma region SI_A_ASD_LOW_BRANDISH
	case CX2SkillTree::SI_A_ASD_LOW_BRANDISH:
		{
			ASI_A_ASD_LOW_BRANDISH_SLIDING_CreateState( normalStateData, normalStateData.stateID );
			ASI_A_ASD_LOW_BRANDISH_SLIDING_CreateState( hyperStateData, hyperStateData.stateID );

			UserUnitStateData subStateData;
			ASI_A_ASD_LOW_BRANDISH_KICK_CreateState( subStateData, ASI_A_ASD_LOW_BRANDISH_KICK );
			m_StateList[subStateData.stateID] = subStateData;
		} break;
	//#pragma endregion ����

	//#pragma region SI_A_ARA_QUICK_PIERCING
	case CX2SkillTree::SI_A_AM_QUICK_PIERCING:
		{
			m_LuaManager.MakeTableReference( "ASI_A_ARA_QUICK_PIERCING", normalStateData.stateID );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_Quick_Piercing_EventProcess );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_Quick_Piercing_FrameMove );

			m_LuaManager.MakeTableReference( "ASI_A_ARA_QUICK_PIERCING", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
		} break;
	//#pragma endregion �ż�

	//#pragma region SI_A_ARA_SHOULDER_BLOW
	case CX2SkillTree::SI_A_AM_SHOULDER_BLOW:
		{
			m_LuaManager.MakeTableReference( "ASI_A_ARA_SHOULDER_BLOW", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_Shoulder_Blow_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_Shoulder_Blow_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_Shoulder_Blow_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_A_ARA_SHOULDER_BLOW", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ö���

	//#pragma region SI_A_ASD_DRAGON_FALLING
	case CX2SkillTree::SI_A_ASD_DRAGON_FALLING:
		{
			m_LuaManager.MakeTableReference( "ASI_A_ASD_FALLING_BLOSSOM_Start", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_Falling_Blossom_Start_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_Falling_Blossom_Start_StateStart );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_Falling_Blossom_Start_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_A_ASD_FALLING_BLOSSOM_Start", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= ASI_A_ASD_FALLING_BLOSSOM_FINISH;
				m_LuaManager.MakeTableReference( "ASI_A_ASD_FALLING_BLOSSOM_Finish", stateData.stateID );
				stateData.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_Falling_Blossom_Finish_Init );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_Falling_Blossom_Finish_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	//#pragma endregion ��ȭ

	//#pragma region SI_A_ARA_QUICK_STAB
	case CX2SkillTree::SI_A_AM_QUICK_STAB:
		{
			m_LuaManager.MakeTableReference( "ASI_A_ARA_QUICK_STAB", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_Quick_Stab_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_Quick_Stab_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_Quick_Stab_EventProcess );

			m_LuaManager.MakeTableReference( "ASI_A_ARA_QUICK_STAB", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion õ��

	//#pragma region SI_A_ALD_WOLF_CLAW
	case CX2SkillTree::SI_A_ALD_WOLF_CLAW:
		{
			ASI_A_ALD_WOLF_CLAW_CreateState( normalStateData, normalStateData.stateID );
			ASI_A_ALD_WOLF_CLAW_CreateState( hyperStateData, hyperStateData.stateID );

			UserUnitStateData subStateData;
			ASI_A_ALD_WOLF_CLAW_FINISH_CreateState( subStateData, ASI_A_ALD_WOLF_CLAW_FINISH );
			m_StateList[subStateData.stateID] = subStateData;
		} break;
	//#pragma endregion �������

	//#pragma region SI_A_ALD_CONVERSION_HEALTH
		case CX2SkillTree::SI_A_ALD_CONVERSION_HEALTH:
		{
			ASI_A_ALD_CONVERSION_HEALTH_CreateState( normalStateData, normalStateData.stateID );
			ASI_A_ALD_CONVERSION_HEALTH_CreateState( hyperStateData, hyperStateData.stateID );
		} break;
	//#pragma endregion ������

	//#pragma region SI_A_ALD_CONVERSION_MANA
		case CX2SkillTree::SI_A_ALD_CONVERSION_MANA:
			{
				ASI_A_ALD_CONVERSION_MANA_CreateState( normalStateData, normalStateData.stateID );
				ASI_A_ALD_CONVERSION_MANA_CreateState( hyperStateData, hyperStateData.stateID );
			} break;
	//#pragma endregion �̱��

	//#pragma region SI_A_AYR_HIGH_SPEED
		case CX2SkillTree::SI_A_AYR_HIGH_SPEED:
			{
				ASI_A_AYR_HIGH_SPEED_READY_CreateState( normalStateData, normalStateData.stateID );
				ASI_A_AYR_HIGH_SPEED_READY_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_A_AYR_HIGH_SPEED_CreateState( subStateData, ASI_A_AYR_HIGH_SPEED );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion ���

	//#pragma region SI_SA_ALD_ENERGY_WAVE
		case CX2SkillTree::SI_SA_ALD_ENERGY_WAVE:
			{
				ASI_SA_ALD_ENERGY_WAVE_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_ALD_ENERGY_WAVE_CreateState( hyperStateData, hyperStateData.stateID );
			} break;
	//#pragma endregion �����

	//#pragma region SI_SA_ALD_HELL_OF_TORNADO
		case CX2SkillTree::SI_SA_ALD_HELL_OF_TORNADO:
			{
				ASI_SA_ALD_HELL_OF_TORNADO_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_ALD_HELL_OF_TORNADO_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_SA_ALD_HELL_OF_TORNADO_FINISH_CreateState( subStateData, ASI_SA_ALD_HELL_OF_TORNADO_FINISH );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion ��ȯ����

	//#pragma region SI_SA_ALD_WOLF_SPEAR
		case CX2SkillTree::SI_SA_ALD_WOLF_SPEAR:
			{
				ASI_SA_ALD_WOLF_SPEAR_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_ALD_WOLF_SPEAR_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_SA_ALD_WOLF_SPEAR_FINISH_CreateState( subStateData, ASI_SA_ALD_WOLF_SPEAR_FINISH );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion �����̻�

	//#pragma region SI_SA_ALD_FINGER_BULLET
		case CX2SkillTree::SI_SA_ALD_FINGER_BULLET:
			{
				ASI_SA_ALD_FINGER_BULLET_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_ALD_FINGER_BULLET_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_SA_ALD_FINGER_BULLET_FINISH_CreateState( subStateData, ASI_SA_ALD_FINGER_BULLET_FINISH );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion ���� 1�� - ��ȥ

	//#pragma region SI_SA_ALD_ENERGY_BEAD
		case CX2SkillTree::SI_SA_ALD_ENERGY_BEAD:
			{
				ASI_SA_ALD_ENERGY_BEAD_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_ALD_ENERGY_BEAD_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_SA_ALD_ENERGY_BEAD_FINISH_CreateState( subStateData, ASI_SA_ALD_ENERGY_BEAD_FINISH );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion ���� 2�� - ��ȯ��

	//#pragma region SI_SA_ALD_PRISON_SPEAR
		case CX2SkillTree::SI_SA_ALD_PRISON_SPEAR:
			{
				ASI_SA_ALD_PRISON_SPEAR_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_ALD_PRISON_SPEAR_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_SA_ALD_PRISON_SPEAR_FINISH_CreateState( subStateData, ASI_SA_ALD_PRISON_SPEAR_FINISH );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion ���� 3�� - â�차��

	//#pragma region SI_A_AYR_STEAL_SOUL
		case CX2SkillTree::SI_A_AYR_STEAL_SOUL:
			{
				ASI_A_AYR_STEAL_SOUL_CreateState( normalStateData, normalStateData.stateID );
				ASI_A_AYR_STEAL_SOUL_CreateState( hyperStateData, hyperStateData.stateID );

				UserUnitStateData subStateData;
				ASI_A_AYR_STEAL_SOUL_FINISH_CreateState( subStateData, ASI_A_AYR_STEAL_SOUL_FINISH );
				m_StateList[subStateData.stateID] = subStateData;
			} break;
	//#pragma endregion ���� 4�� - Żȥ

	//#pragma region SI_SA_AYR_ENERGY_VOID
		case CX2SkillTree::SI_SA_AYR_ENERGY_VOID:
			{
				ASI_SA_AYR_ENERGY_VOID_START_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_AYR_ENERGY_VOID_START_CreateState( hyperStateData, hyperStateData.stateID );
				{
					UserUnitStateData subStateData;
					subStateData.Init();
					ASI_SA_AYR_ENERGY_VOID_LOOP_CreateState( subStateData, ASI_SA_AYR_ENERGY_VOID_LOOP );
					m_StateList[subStateData.stateID] = subStateData;

					subStateData.Init();
					ASI_SA_AYR_ENERGY_VOID_END_CreateState( subStateData, ASI_SA_AYR_ENERGY_VOID_END );
					m_StateList[subStateData.stateID] = subStateData;
				}
			}
			break;
	//#pragma endregion ��������

	//#pragma region SI_SA_AYR_SUPPRESSION_ENERGY
		case CX2SkillTree::SI_SA_AYR_SUPPRESSION_ENERGY:
			{
				ASI_SA_AYR_SUPPRESSION_ENERGY_CreateState( normalStateData, normalStateData.stateID );
				ASI_SA_AYR_SUPPRESSION_ENERGY_CreateState( hyperStateData, hyperStateData.stateID );
			} break;
	//#pragma endregion ���� - ��

	//#pragma region SI_SA_AYR_SUPPRESSION_ENERGY
		case CX2SkillTree::SI_HA_AYR_ENERGY_CANNON:
			{
				ASI_HA_AYR_ENERGY_CANNON_CreateState( normalStateData, normalStateData.stateID );
				ASI_HA_AYR_ENERGY_CANNON_CreateState( hyperStateData, hyperStateData.stateID );
			} break;
	//#pragma endregion ���ȯ����

//#pragma region SI_A_ALD_REFLECTION
		case CX2SkillTree::SI_A_ALD_REFLECTION:
			{
				// 20130812 // ����ö // ��ź���� ������Ʈ�� ��� ���� ���� �̰����� ��Ŵ
				SoundReadyInBackground( L"Elsword_Endurance.ogg" );
			} break;
//#pragma endregion ��ź��

	}

	//#pragma endregion
}
//#pragma endregion

//#pragma region SetEquippedSkillLevel
/** @function	: SetEquippedSkillLevel
	@brief		: �� ĳ������ ��ų ���� ������Ʈ�� ��ųID, ��ų ����, �Ŀ�����Ʈ ���� ������Ʈ �����Ϳ� �ʱ�ȭ\n
				  Ư�� ��ų�� �ΰ� �̻��� ������Ʈ�� ������ ��� �̰��� �߰��ϴ� ���� �����ؾ���
	@param		: const CX2SkillTree::SKILL_ID eSkillID_ (������ ��ų�� ID), const bool bChangeAll_ (true�� ��� ��� ��ų ����)
*/
/*virtual*/ void CX2GUAra::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AM_WIND_WEDGE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AM_WIND_WEDGE );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ARA_WIND_WEDGE_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AM_SHADOW_RISING == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AM_SHADOW_RISING );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ARA_SHADOW_RISING_LANDING );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ARA_SHADOW_RISING_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AM_PULLING_PIERCE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AM_PULLING_PIERCE );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ARA_PULLING_PIERCE_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AM_TIGER_CLAW == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AM_TIGER_CLAW );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ARA_TIGER_CLAW_START );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ARA_TIGER_CLAW_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AM_TEMPEST_DANCE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AM_TEMPEST_DANCE );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ARA_TEMPEST_DANCE_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AM_SHADOW_KNOT == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AM_SHADOW_KNOT );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ARA_SHADOW_KNOT_END );
	}
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ALH_DOUBLE_COLLISION == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ALH_DOUBLE_COLLISION );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_DOUBLE_COLLISION_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ALH_BREAKING == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ALH_BREAKING );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ALH_BREAKING_ATTACK );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ALH_ROCK_CRASH == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ALH_ROCK_CRASH );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ALH_ROCK_CRASH_START );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ALH_ROCK_CRASH_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ASD_TURBULENT_WAVE == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ASD_TURBULENT_WAVE );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ASD_TURBULENT_WAVE_START );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ASD_TURBULENT_WAVE_LOOP );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ASD_TURBULENT_WAVE_END );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ASD_VOID_BREAK );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALH_WHITE_TIGER == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALH_WHITE_TIGER );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALH_PEERLESSNESS_SLASH == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALH_PEERLESSNESS_SLASH );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_PEERLESSNESS_SLASH_END );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALH_MOONLIGHT_SLASH == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALH_MOONLIGHT_SLASH );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_DESCENT_DRAGON );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALH_SWALLOW_ASSAULT == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALH_SWALLOW_ASSAULT );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALH_SWALLOW_ASSAULT_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ASD_LOW_BRANDISH == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ASD_LOW_BRANDISH );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ASD_LOW_BRANDISH_SLICE );
		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ASD_LOW_BRANDISH_KICK );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ASD_SUPPRESSION == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ASD_SUPPRESSION );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ASD_SUPPRESSION );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ASD_DRAGON_FALLING == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ASD_DRAGON_FALLING );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ASD_FALLING_BLOSSOM_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AYR_HIGH_SPEED == eSkillID_ )			/// �ƶ� 1�� ���� �Ҹ� - ���
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AYR_HIGH_SPEED );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_AYR_HIGH_SPEED );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ALD_WOLF_CLAW == eSkillID_ )				/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� �������
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ALD_WOLF_CLAW );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_ALD_WOLF_CLAW_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALD_HELL_OF_TORNADO == eSkillID_ )		/// �ƶ� 1�� ���� �Ҹ� - ��ȯ����
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALD_HELL_OF_TORNADO );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_HELL_OF_TORNADO_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALD_WOLF_SPEAR == eSkillID_ )			/// �ƶ� 1�� ���� �Ҹ� - ���� 4�� �����̻�
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALD_WOLF_SPEAR );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_WOLF_SPEAR );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_WOLF_SPEAR_FINISH );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_WOLF_BREAK );		///  ���� �������� - �ؼ�
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALD_FINGER_BULLET == eSkillID_ )		/// �ƶ� 1�� ���� �Ҹ� - ���� 1�� ��ȥ
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALD_FINGER_BULLET );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_FINGER_BULLET_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALD_ENERGY_BEAD == eSkillID_ )			/// �ƶ� 1�� ���� �Ҹ� - ���� 2�� ��ȯ��
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALD_ENERGY_BEAD );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_ENERGY_BEAD_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ALD_PRISON_SPEAR == eSkillID_ )			/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� â�차��
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALD_PRISON_SPEAR );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_ALD_PRISON_SPEAR_FINISH );
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AYR_STEAL_SOUL == eSkillID_ )			/// �ƶ� 2�� ���� ���� - ���� 4�� Żȥ
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AYR_STEAL_SOUL );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_A_AYR_STEAL_SOUL_FINISH );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_AYR_OPEN_THE_HELL );	/// ���� �������� - ����
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AYR_ENERGY_VOID == eSkillID_ )			/// �ƶ� 2�� ���� ���� - ��������
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AYR_ENERGY_VOID );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_AYR_ENERGY_VOID_LOOP );

		SetEquippedSkillLevelStateData( pSkillTemplet, ASI_SA_AYR_ENERGY_VOID_END );
	}


}
//#pragma endregion

//#pragma region SetEquippedSkillLevelStateData
/** @function	: SetEquippedSkillLevelStateData
	@brief		: ������ ��ų�� �������� ����� �� ���� ����Ǿ�� �� ������Ʈ ����Ÿ
	@param		: const CX2SkillTree::SkillTemplet* pSkillTemplet, ARA_STATE_ID eStateID
*/
void CX2GUAra::SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet, ARA_STATE_ID eStateID )
{
	if ( NULL != pSkillTemplet )
		SetSkillLevelStateData( pSkillTemplet, m_StateList[eStateID] );
}
//#pragma endregion

void CX2GUAra::SetSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, UserUnitStateData& stateData_ )
{

	if ( NULL == GetUnit() )
		return;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

	stateData_.m_SPLevel	= iSkillTempletLevel;
	stateData_.m_fPowerRate	= pSkillTemplet_->GetSkillPowerRateValue( iSkillTempletLevel );


	stateData_.m_eSkillID	= pSkillTemplet_->m_eID;
}

//#pragma region InitComponent
/** @function	: InitComponent
	@brief		: ĳ���� ��ũ��Ʈ�� INIT_COMPONENT ���̺��� �ε���
*/
/*virtual*/ void CX2GUAra::InitComponent()
{
	CX2GUUser::InitComponent();

	if ( IsMyUnit() )
	{
		ASSERT( NULL != g_pX2Game );
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_PVP:
			{
				static_cast<CX2AraGageData*>(m_pGageData)->SetBerserkMode( false );
				static_cast<CX2AraGageData*>(m_pGageData)->SetBerserkModeChanged( false );
				static_cast<CX2AraGageData*>(m_pGageData)->SetNowForcePower( 2 );
				static_cast<CX2AraGageData*>(m_pGageData)->SetForcePowerChanged( true );
			} break;

		default:
			{
				if ( g_pMain->GetNowStateID() == CX2Main::XS_TRAINING_GAME )
				{
					static_cast<CX2AraGageData*>(m_pGageData)->SetBerserkMode( false );
					static_cast<CX2AraGageData*>(m_pGageData)->SetBerserkModeChanged( false );
					static_cast<CX2AraGageData*>(m_pGageData)->SetNowForcePower( 2 );
					static_cast<CX2AraGageData*>(m_pGageData)->SetForcePowerChanged( true );
				}
			} break;
		}
	}
}
//#pragma endregion

//#pragma region ParseCommonRandomState
/** @function	: ParseCommonRandomState
	@brief		: Start, Win, Lose ������Ʈ�� �����ϰ� �ε��Ѵ�
	@param		: �� ������ start, win, lose ������Ʈ�� ����ִ� ������ ���̺���
*/
/*virtual*/ void CX2GUAra::ParseCommonRandomState()
{
	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{

		const char* tableName = "";
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_ARA_MARTIAL_ARTIST:
			{
				tableName = "ARA_MARTIAL_ARTIST";
			} break;
		case CX2Unit::UC_ARA_LITTLE_HSIEN:
			{
				tableName = "ARA_LITTLE_HSIEN";
			} break;
		case CX2Unit::UC_ARA_SAKRA_DEVANAM:
			{
				tableName = "ARA_SAKRA_DEVANAM";
			} break;
		case CX2Unit::UC_ARA_LITTLE_DEVIL:
			{
				tableName = "ARA_LITTLE_DEVIL";
			} break;
		case CX2Unit::UC_ARA_YAMA_RAJA:
			{
				tableName = "ARA_YAMA_RAJA";
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
//#pragma endregion

//#pragma region InitEffect
/** @function	: InitEffect
	@brief		: ����� ������ �ִ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUAra::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();
}
//#pragma endregion

//#pragma region InitEnchantWeaponEffectUnitSpecific
/** @function	: InitEnchantWeaponEffectUnitSpecific
	@brief		: �� ĳ���Ͱ� ���� ����Ʈ ��ƼŬ �ʱ�ȭ
*/
/*virtual*/ void CX2GUAra::InitEnchantWeaponEffectUnitSpecific()
{
	for( Weapon* pWeapon : m_vecpWeapon )
	{
		ASSERT( NULL != pWeapon );
		pWeapon->InitEnchantParticleForBow();	// �ӽ�
	}
}
//#pragma endregion

//#pragma region InitializeAraMajorParticleArray
/** @function	: InitializeAraMajorParticleArray
	@brief		: ����� ������ �ִ� ������ ��ƼŬ �ڵ� �迭 �ʱ�ȭ
*/
void CX2GUAra::InitializeAraMajorParticleArray()
{
	CX2GUUser::InitializeGUUserMajorParticleArray();

	for ( int index = 0; index < ARA_MAJOR_PII_END; index++ )
	{
		m_ahAraMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}
//#pragma endregion

//#pragma region CreateMajorParticleArray
/** @function	: CreateMajorParticleArray
	@brief		: ����� ������ �ִ� ������ ��ƼŬ ����
*/
/*virtual*/ void CX2GUAra::CreateMajorParticleArray()
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
//#pragma endregion

//#pragma region AppendMajorParticleToDeviceList
/*static*/ void CX2GUAra::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
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
//#pragma endregion

//#pragma region SetAraMajorParticleByEnum
/** @function	: SetAraMajorParticleByEnum
	@brief		: ��ƼŬ���� ��ġ�ϴ� ������ ��ƼŬ�� ������ �� �迭�� �ش� ��ƼŬ �ڵ��� ������, �ڵ��� �迭�� ����, �׸��� �� �ν��Ͻ��� ��������
	@param		: ��ƼŬ Instance ID, ��ƼŬ ��, DrawCount
	@return		: ������ �Ǵ� �̹� �����ϴ� ��ƼŬ�� �ν��Ͻ�(CKTDGParticleSystem::CParticleEventSequence*)
*/
CKTDGParticleSystem::CParticleEventSequence* CX2GUAra::SetAraMajorParticleByEnum( ARA_MAJOR_PARTICLE_INSTANCE_ID eVal_,
																			wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleAraMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,
											wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleAraMajorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAraMajorParticleByEnum( eVal_) );

	return pSeq;
}
//#pragma endregion

//#pragma region DeleteAraMajorParticle
/** @function	: DeleteMajorParticle
	@brief		: ����� ������ ������ ��ƼŬ ����Ʈ ����
*/
void CX2GUAra::DeleteAraMajorParticle()
{
	CX2GUUser::DeleteGUUserMajorParticle();

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	for ( int index = 0; index < ARA_MAJOR_PII_END; index++ )
	{
		pMajorParticleSystem->DestroyInstanceHandle( m_ahAraMajorParticleInstance[index] );
	}
}
//#pragma endregion

//#pragma region InitializeAraMinorParticleArray
/** @function : InitializeAraMinorParticleArray
	@brief : ����� ������ �ִ� ���̳� ��ƼŬ �ڵ� �迭 �ʱ�ȭ
*/
void CX2GUAra::InitializeAraMinorParticleArray()
{
	CX2GUUser::InitializeGUUserMinorParticleArray();

	for ( int index = 0; index < ARA_MINOR_PII_END; index++ )
	{
		m_ahAraMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}
//#pragma endregion

//#pragma region CreateMinorParticleArray
/** @function : CreateMinorParticleArray
	@brief : ����� ������ �ִ� ������ ��ƼŬ ����
*/
/*virtual*/ void CX2GUAra::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();
}
//#pragma endregion

//#pragma region AppendMinorParticleToDeviceList
/** @function 	: AppendMinorParticleToDeviceList
	@brief 		:
*/
/*static*/ void CX2GUAra::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}
//#pragma endregion

//#pragma region SetAraMinorParticleByEnum
/** @function	: SetAraMinorParticleByEnum
	@brief		: ��ƼŬ���� ��ġ�ϴ� ���̳� ��ƼŬ�� ������ �� �迭�� �ش� ��ƼŬ �ڵ��� ������, �ڵ��� �迭�� ����, �׸��� �� �ν��Ͻ��� ��������
	@param		: ��ƼŬ Instance ID, ��ƼŬ ��, DrawCount
	@return		: ������ �Ǵ� �̹� �����ϴ� ��ƼŬ�� �ν��Ͻ�(CKTDGParticleSystem::CParticleEventSequence*)
*/
CKTDGParticleSystem::CParticleEventSequence* CX2GUAra::SetAraMinorParticleByEnum( ARA_MINOR_PARTICLE_INSTANCE_ID eVal_,
																			wstring wstrParticleName_, int iDrawCount_ /*= -1*/ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleAraMinorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,
											wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_, 0 );

		SetHandleAraMinorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleAraMinorParticleByEnum( eVal_) );

	return pSeq;
}
//#pragma endregion

//#pragma region DeleteAraMinorParticle
/** @function	: DeleteAraMinorParticle
	@brief		: ����� ������ ���̳� ��ƼŬ ����Ʈ ����
*/
void CX2GUAra::DeleteAraMinorParticle()
{
	CX2GUUser::DeleteGUUserMinorParticle();

	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	for ( int index = 0; index < ARA_MINOR_PII_END; index++ )
	{
		pMinorParticleSystem->DestroyInstanceHandle( m_ahAraMinorParticleInstance[index] );
	}
}
//#pragma endregion

//#pragma region InitializeMajorMeshArray
/** @function	: InitializeMajorMeshArray
	@brief		: ����� ������ �ִ� ������ �޽� �ڵ� �迭 �ʱ�ȭ
*/
void CX2GUAra::InitializeMajorMeshArray()
{
	for ( int index = 0; index < ARA_MAJOR_MII_END; ++index )
	{
		m_ahAraMajorMeshInstance[index] = INVALID_MESH_INSTANCE_HANDLE;
	}
}
//#pragma endregion

//#pragma region SetHandleAraMajorMeshByEnum
/** @function	: SetHandleAraMajorMeshByEnum
	@brief		: �޽��÷��� �ڵ��� ������ �迭�� ����, �����ҷ��� �ڵ��� INVALID �ϰ�� �޽��÷��� DESTROY �� �迭�ʱ�ȭ
	@param		: Mesh Instance ID, Mesh Instance Handle
*/
void CX2GUAra::SetHandleAraMajorMeshByEnum( ARA_MAJOR_MESH_INSTANCE_ID eVal_, CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_ )
{
	if( INVALID_MESH_INSTANCE_HANDLE == hHandle_ && INVALID_MESH_INSTANCE_HANDLE != GetHandleAraMajorMeshByEnum( eVal_ ) )
	{
		CKTDGXMeshPlayer::CXMeshInstanceHandle hHandleAraMesh = GetHandleAraMajorMeshByEnum( eVal_ );

		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle(hHandleAraMesh);
	}

	m_ahAraMajorMeshInstance[eVal_] = hHandle_;
}
//#pragma endregion

//#pragma region DeleteMajorMesh
/** @function	: DeleteMajorMesh
	@brief		: ����� ������ ������ ����Ʈ ����
*/
void CX2GUAra::DeleteMajorMesh()
{
	CKTDGXMeshPlayer* pMajorMeshSystem = g_pX2Game->GetMajorXMeshPlayer();

	for ( int index = 0; index < ARA_MAJOR_MII_END; index++ )
	{
        pMajorMeshSystem->DestroyInstanceHandle( m_ahAraMajorMeshInstance[index] );
	}
}
//#pragma endregion

//#pragma region CommonStateStartFuture
/** @function	: CommonStateStartFuture
	@brief		: ��� ������Ʈ�� StartFuture���� ȣ��Ǵ� �Լ�
*/
void CX2GUAra::CommonStateStartFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateStartFuture();
}
//#pragma endregion

//#pragma region CommonStateStart
/** @function	: CommonStateStart
	@brief		: ��� ������Ʈ�� Start���� ȣ��Ǵ� �Լ�
*/
void CX2GUAra::CommonStateStart()
{
	KTDXPROFILE();

	LUA_GET_VALUE( m_LuaManager, "DAMAGEDATA_CHANGE_TIME", m_fDamageDataChangeTime, 0.f );
	LUA_GET_VALUE( m_LuaManager, "DAMAGEDATA_CHANGE_TIME2", m_fDamageDataChangeTime2, 0.f );

	/// �ش� �ð��� �����Ǿ� ������, �� �ð��� ��� ��� ó���� �Ѵ�.
	m_fUseForcePowerTime	= -1.f;
	m_iUseForcePowerCount	= 0;

	if( m_LuaManager.BeginTable( "USE_FORCE_POWER" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, 1, m_fUseForcePowerTime, -1.f  );	/// ��� ��� ����
		LUA_GET_VALUE( m_LuaManager, 2, m_iUseForcePowerCount, 0  );	/// ��� ��뷮

		m_LuaManager.EndTable();
	}

	/// �ش� �ð��� �����Ǿ� ������, �� �ð��� ��� ���� ó���� �Ѵ�.
	m_fAddForcePowerTime	= -1.f;
	m_iAddForcePowerCount	= 0;

	if( m_LuaManager.BeginTable( "ADD_FORCE_POWER" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, 1, m_fAddForcePowerTime, -1.f  );	/// ��� ���� ����
		LUA_GET_VALUE( m_LuaManager, 2, m_iAddForcePowerCount, 0  );	/// ��� ������

		m_LuaManager.EndTable();
	}

	if( m_LuaManager.BeginTable( "FP_CANCEL" ) == true )
	{
		LUA_GET_VALUE( m_LuaManager, 1, m_vFPCancel.x, - 1.f  );
		LUA_GET_VALUE( m_LuaManager, 2, m_vFPCancel.y, - 1.f  );

		m_LuaManager.EndTable();
	}

	ChangeAnimTail();

	//m_bFPCancel = false;

	CX2GUUser::CommonStateStart();

	if ( true == m_DamageData.m_bMeleeAttack && true == m_bBrutalPiercing && NULL != m_pBrutalPiercingBuffFactor )
		m_DamageData.PushBuffFactor( m_pBrutalPiercingBuffFactor );

	m_ForcePowerSystem.SetUsedForcePowerAtThisState( 0 );
}
//#pragma endregion

//#pragma region CommonFrameMoveFuture
/** @function	: CommonFrameMoveFuture
	@brief		: ��� ������Ʈ�� FrameMoveFuture���� ȣ��Ǵ� �Լ�
*/
void CX2GUAra::CommonFrameMoveFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMoveFuture();

	// ��� ����ȭ �ڵ�
	m_FrameDataFuture.syncData.m_CannonBallCount= GetForcePower();

	if ( NULL == m_FutureStateData.m_ptrComboAndSkillChainInfo )
		m_ComboAndSkillChainSystem.ResetChaining();
	else
	{
		m_FutureStateData.m_ptrComboAndSkillChainInfo->SetChaining( true );
		m_FutureStateData.m_ptrComboAndSkillChainInfo->ResetChainingFromNext();
		m_ComboAndSkillChainSystem.ResetOtherListChaining( m_FutureStateData.m_ptrComboAndSkillChainInfo );
	}

}
//#pragma endregion

//#pragma region CommonFrameMove
/** @function	: CommonFrameMove
	@brief		: ��� ������Ʈ�� FrameMove���� ȣ��Ǵ� �Լ�
*/
void CX2GUAra::CommonFrameMove()
{
	KTDXPROFILE();
	CX2GUUser::CommonFrameMove();

	if ( false == IsMyUnit() )
	{
		// ��� ����
		SetForcePower( m_FrameDataNow.syncData.m_CannonBallCount );
	} // if

	// ����ȭ ��忡 ���̴� ���� frameMove ������
	for( CX2EqipPtr pPromotionEquipPtr : m_vecPromotionEquipList )
	{
		pPromotionEquipPtr->SetRenderParam( m_fTime, m_fElapsedTime, m_pXSkinAnim->GetRenderParam() );
		pPromotionEquipPtr->OnFrameMove( m_fTime, m_fElapsedTime );
	}

	/// �ƶ� 2�� ���� ���� - ȸ������ ��Ÿ�� ����
	if ( 0.f < m_fResurrectionCoolTime )
	{
		m_fResurrectionCoolTime -= m_fElapsedTime;

		/// ��Ÿ�� �Ϸ� �Ǿ�����, ȸ������ ��� �Ұ� �ȳ� ����� ����
		if ( 0.f >= m_fResurrectionCoolTime )
			EraseBuffTempletFromGameUnit( BTI_DEBUFF_SELF_PROTECTION_FORTITUDE );
	}

	/// ��� ��� �ð��� �����Ǿ� �ִٸ�, �ش� �ð��� ��� ���
	if ( false == IsSamef( m_fUseForcePowerTime, -1.f ) )
	{
		if ( true == UseForcePowerAtThisAnimationTime( m_fUseForcePowerTime, m_iUseForcePowerCount ) )
			m_fUseForcePowerTime = -1.f;
	}

	/// ��� ���� �ð��� �����Ǿ� �ִٸ�, �ش� �ð��� ��� ����
	if ( false == IsSamef( m_fAddForcePowerTime, -1.f ) )
	{
		if ( true == UpForcePowerAtThisAnimationTime( m_fAddForcePowerTime, m_iAddForcePowerCount ) )
			m_fAddForcePowerTime = -1.f;
	}
}
//#pragma endregion

//#pragma region CommonStateEndFuture
/** @function	: CommonStateEndFuture
	@brief		: ��� ������Ʈ�� EndFuture���� ȣ��Ǵ� �Լ�
*/
void CX2GUAra::CommonStateEndFuture()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEndFuture();
}
//#pragma endregion

//#pragma region CommonStateEnd
/** @function	: CommonStateEnd
	@brief		: ��� ������Ʈ�� End���� ȣ��Ǵ� �Լ�
*/
void CX2GUAra::CommonStateEnd()
{
	KTDXPROFILE();
	CX2GUUser::CommonStateEnd();
}
//#pragma endregion

//#pragma region CommonRender_Prepare
/** @function	: CommonRender_Prepare
	@brief		: ��� ������Ʈ�� End���� ȣ��Ǵ� �Լ�
*/
RENDER_HINT CX2GUAra::CommonRender_Prepare()
{
	KTDXPROFILE();

	return CX2GUUser::CommonRender_Prepare();
}
//#pragma endregion

//#pragma region SepcialAttackEventProcess
/** @function	: SepcialAttackEventProcess
	@brief		: ��ų�� ���������� ���� �� �ִ� �� �Ǵ�, ��ų��뿡 �ʿ��� ó�� �� ������Ʈ ����
	@param		: ��ų�� ����� �� �ִ� ȯ�� ����
	@return		: bool (��ų�� ���������� ���Ǿ��°��� ����)
*/
/*virtual*/ bool CX2GUAra::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_ONLY_ON_LINE*/ )
{
	if ( false == CanUseSpecialAttack( eActiveSkillUseCondition ) )
		return false;
	if ( CX2SkillTree::SI_NONE != m_eSkillID )
		StateChangeSpecialAttack( m_eSkillID );

	Send_SKILL_USE_REQ( m_eSkillID );

	return true;
}
//#pragma endregion

//#pragma region CanUseSpecialAttack
/** @function 	: CanUseSpecialAttack
	@brief 		:
*/
bool CX2GUAra::CanUseSpecialAttack( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_ONLY_ON_LINE*/ )
{
	KTDXPROFILE();

	int iPressedSkillSlotIndex = -1;
	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = NULL;
	const CX2UserSkillTree& cUserSkillTree =  GetUnit()->GetUnitData().m_UserSkillTree;	// ������ ��� ��ų Ʈ��

	// ����� ��ų�� ��ų���� �����Ϳ� �� �ε����� ����
	if ( false == CommonSpecialAttackEventProcess( cUserSkillTree, pSkillSlotData, iPressedSkillSlotIndex ) )
		return false;

	if( NULL == pSkillSlotData )
		return false;

	// ����� ��ų
	//m_pSkillTemplet = NULL;
	const CX2SkillTree::SkillTemplet* pSkillTemplet
		= GetEquippedActiveSkillTemplet( pSkillSlotData->m_eID );

	if( NULL == pSkillTemplet )
		return false;

	// ��ų�� ����� �� �ִ� �������� �Ǵ�( ex: ���߿����� ��� �� �� ���� �ʻ�� ��)
	if( false == CheckSkillUseCondition( eActiveSkillUseCondition, pSkillTemplet->m_eActiveSkillUseCondtion ) )
		return false;

	//��ȯ �ʻ�� ������ ������ ���� ���¿��� ����� �� ����.
	if( false == CheckSummonSpecialAttack( pSkillTemplet->m_eID ) )
		return false;

	// ���Ĵ������� ��� ������ ��ų���� �˻�
	if( false == CheckSkillUsePVPOfficial( pSkillTemplet->m_eID ) )
		return false;

	if ( NULL == GetUnit() )
		return false;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

	// ������ ��ų�� ����� �� �ִ� �������� �Ǵ�
	if ( CX2SkillTree::SI_A_ALD_CONVERSION_HEALTH == pSkillTemplet->m_eID )
	{
		if ( false == CanUseConversionHealth( pSkillTemplet, iSkillTempletLevel ) )
		{
			/// ü���� �����Ͽ� ����� �� ���ٸ�, �ȳ� ��Ʈ�� ǥ��
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_26868 ), D3DXCOLOR(1,1,1,1),
				D3DXCOLOR(0,0,0,1), DT_CENTER, 1.f, 1.f );
			return false;
		}
	}

	// ���� �Ҹ��� ����
	m_fMPConsume = GetActualMPConsume( pSkillTemplet->m_eID, iSkillTempletLevel );

	float fCoolTimeLeft = pSkillSlotData->m_fCoolTimeLeft;

	bool bSlotB = (iPressedSkillSlotIndex > 3) ? true : false;
	int iSlotIndex = (iPressedSkillSlotIndex > 3) ? iPressedSkillSlotIndex-4 : iPressedSkillSlotIndex;

	/// �ش� ��ų�� ��� �Һ� ( �޸� ȿ�� ���� )
	USHORT usFPConsumptionNeeded = GetActualFPConsume( pSkillTemplet->m_eID, cUserSkillTree.GetFPConsumption( iSlotIndex, bSlotB ) );

	/// �ش� ��ų�� ��� ��ȯ�� ( �޸� ȿ�� ���� ), ( No State Change ��ų�̶��, ��� ��ȯ�� ���� ���� 0���� ���� )
	m_usForceGainCount =	false == IsNoStageChangeSkill( pSkillTemplet ) ?
							GetActualFPGainCount( pSkillTemplet->m_eID, cUserSkillTree.GetFPGainCount( iSlotIndex, bSlotB ) ) : 0;


	/// �ڵ� ������ ���� �ӽ� ���� ó��
	{
		m_ComboAndSkillChainSystem.ResetPtrNowUseChainInfo();

		const UserUnitStateData& stateDataPressed = GetPressedSkillState( iPressedSkillSlotIndex );
		if ( NULL != stateDataPressed.m_ptrComboAndSkillChainInfo &&
			stateDataPressed.m_ptrComboAndSkillChainInfo->GetChaining() )
		{
			CX2ComboAndSkillChainInfoPtr ptrInfo
				= stateDataPressed.m_ptrComboAndSkillChainInfo->GetNotYetChainedInfo();

			if ( NULL != ptrInfo )
			{
				switch ( ptrInfo->GetComboAndSkillChainType() )
				{
				case CASCT_SKILL:
					{
						const CX2SkillTree::SKILL_ID eSkillIdChained
							= static_cast<CX2SkillTree::SKILL_ID>( ptrInfo->GetSkillID() );
						pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillIdChained );
						pSkillSlotData = cUserSkillTree.GetSkillSlot( eSkillIdChained );

						/// ���� ��ų�� ���� �ϰ� �ִ� ���
						if ( NULL != pSkillTemplet && NULL != pSkillSlotData )
						{
							m_fMPConsume = GetActualMPConsume( pSkillTemplet->m_eID, iSkillTempletLevel );

							fCoolTimeLeft = pSkillSlotData->m_fCoolTimeLeft;
							usFPConsumptionNeeded = pSkillTemplet->m_usFPConsumtion;
							m_usForceGainCount = pSkillTemplet->m_usFPGainCount;
							m_ComboAndSkillChainSystem.SetPtrNowUseChainInfo( ptrInfo );
						}
						else	/// ���� ��ų�� ���� �ϰ� ���� ���� ���
						{
							pSkillTemplet = cUserSkillTree.GetUserSkillTemplet( eSkillIdChained );
							if ( NULL != pSkillTemplet )	/// ������ ���߾ ��ų�� ��� ���
							{
								if ( NULL == GetUnit() )
									return false;

								const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

								const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

								m_fMPConsume = pSkillTemplet->GetSkillMPConsumptionValue( iSkillTempletLevel );

								fCoolTimeLeft = 0.0f;
								usFPConsumptionNeeded = pSkillTemplet->m_usFPConsumtion;
								m_usForceGainCount = pSkillTemplet->m_usFPGainCount;
								m_ComboAndSkillChainSystem.SetPtrNowUseChainInfo( ptrInfo );
							}
							/// �� ��� ��쿡�� ���� ����
						}
					} break;

				default:
					{
						m_ComboAndSkillChainSystem.ResetChaining();
						ASSERT( !L"wrong path" );
					}
					break;
				}
			}

		}
	}

	//m_pSkillTemplet = pSkillTemplet;

	if( NULL == pSkillTemplet )		/// pSkillTemplet�� ������ ���� �����ϱ� ������, NULLó�� �߰�
		return false;

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

		if ( GetForcePower() < usFPConsumptionNeeded )
		{
			g_pX2Game->GetInfoTextManager().PushText( XUF_DODUM_20_BOLD, GET_STRING( STR_ID_21303 ), D3DXCOLOR(1,1,1,1),
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

	if ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
	{
		if( true ==  g_pData->GetMyUser()->GetSelectUnit()->IsInfinityElEssence() )
		{
			g_pX2Game->Handler_EGS_USE_FINALITY_SKILL_REQ();
		}
		else
		{
			CX2Item* pItem = g_pData->GetMyUser()->GetSelectUnit()->GetInventory().GetItemByTID( CX2EnchantItem::ATI_HYPER_SKILL_STONE );

			if ( NULL != pItem )
				g_pX2Game->Handler_EGS_USE_FINALITY_SKILL_REQ( pItem->GetItemData().m_ItemUID );
		}
	}

	m_eSkillID = pSkillTemplet->m_eID;
	m_iPressedSkillSlotIndex = iPressedSkillSlotIndex;

	if ( pSkillTemplet->m_eID == CX2SkillTree::SI_SA_ALH_MOONLIGHT_SLASH )
	{
		if ( NULL == m_sMoonRightSlashDataPtr )
			m_sMoonRightSlashDataPtr = sMoonLightSlashData::CreateMoonLightSlashData();

		if ( NULL != m_sMoonRightSlashDataPtr )
		{
			const int iNormalStateID = ( false == bSlotB ? _CONST_GUUSER_::aNormalSkillStateID[iSlotIndex] : _CONST_GUUSER_::aNormalSkillStateIDSlotB[iSlotIndex] );

			m_sMoonRightSlashDataPtr->SetSlotID( iNormalStateID );
			m_sMoonRightSlashDataPtr->SetSlotB( bSlotB );
		}
	}

	return true;
}
//#pragma endregion

//#pragma region StateChangeSpecialAttack
/** @function 	: StateChangeSpecialAttack
	@brief 		:
*/
void CX2GUAra::StateChangeSpecialAttack( IN const CX2SkillTree::SKILL_ID eSkillId_ )
{
	DXUTTRACE( L"stateChange %d\n", eSkillId_ );


	CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;

	// ���� ��ũ ���� ����
	CountUsedSkill( m_iPressedSkillSlotIndex );

	// MP �Ҹ� ��Ŵ
	UpNowMp( -m_fMPConsume );

	const CX2SkillTree::SkillTemplet* pSkillTemplet
		= GetEquippedActiveSkillTemplet( eSkillId_ );

	if( NULL != pSkillTemplet )
	{
		// ��ų���ø����� ������ ��Ÿ���� ��ų�� ��� �������� ������

		const CX2SkillTree::SKILL_ID	eSkillID				= pSkillTemplet->m_eID;

		const int	iSkillTempletLevel	= max( 1, accessUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

		const float	fSkillCoolTimeValue	= GetSkillCoolTimeValue( eSkillID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) );

		accessUserSkillTree.SetSkillCoolTimeLeft( eSkillID, fSkillCoolTimeValue );


		if ( NULL != m_ComboAndSkillChainSystem.GetPtrNowUseChainInfo() )
		{
			CX2ComboAndSkillChainInfoPtr ptrInfo
				= m_ComboAndSkillChainSystem.GetPtrNowUseChainInfo();

			DXUTTRACE( L"chain %d, chained %d\n", ptrInfo->GetStateIdMatchedToChain(), ptrInfo->GetStateIdChained() );

			if( GetRemainHyperModeTime() <= 0.f )
				StateChange( ( 0 != ptrInfo->GetStateIdMatchedToChain() ? ptrInfo->GetStateIdMatchedToChain() : ptrInfo->GetStateIdChained() ) );
			else
				StateChange( ( 0 != ptrInfo->GetHyperStateIdMatchedToChain() ? ptrInfo->GetHyperStateIdMatchedToChain() : ptrInfo->GetStateIdChained() ) );
		}
		else
		{
			// state�� ������� ���ƾ��ϴ� ��ų�� �ƴ϶�� ������Ʈ ����
			if( false == SpecialAttackNoStageChange( pSkillTemplet ) )
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
		}

		m_iNowSpecialAttack = m_iPressedSkillSlotIndex + 1;
		m_bSpecialAttackEventProcessedAtThisFrame = true;
		//������//2013-01-21//�Ϻ� ��� ��ȯ ��ų ��� ��, ��� ��ø ȸ�� �Ǵ� ��찡 �־ �ʱ�ȭ �߰�
		m_bFPCancel = false;
	}
}
//#pragma endregion

//#pragma region SpecialAttackNoStageChange
/** @function	: SpecialAttackNoStageChange
	@brief		: ������Ʈ�� ������� ���ƾ��ϴ� ��ų������ üũ �ϰ� ó����
	@param		: IN const CX2SkillTree::SkillTemplet* (����� ��ų�� ��ų ���ø� ������)
	@return		: ������Ʈ ���� ����
*/
/*virtual*/ bool CX2GUAra::SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )
{
	switch( pSkillTemplet_->m_eID )
	{

	case CX2SkillTree::SI_A_ALD_REFLECTION:		/// �ƶ� 1�� ���� �Ҹ� - ��ź��
		{
			m_FrameDataFuture.syncData.nowAction = UAI_REFLECTION;
		} break;

	case CX2SkillTree::SI_NONE:
	default:
		{
			return false;
		} break;

	}
	return true;
}
//#pragma endregion

//#pragma region COMMON_BUFF_FrameMove
/** @function	: COMMON_BUFF_FrameMove
	@brief		: ���������� ���� ���� ��ų�� FrameMove
*/
/*virtual*/ void CX2GUAra::COMMON_BUFF_FrameMove()
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
//#pragma endregion

//#pragma region InitPromotionEquipList
/** @function	: InitPromotionEquipList
	@brief		: ���θ�� ��� ����Ʈ�� �ʱ�ȭ �Ѵ�.
*/
void CX2GUAra::InitPromotionEquipList()
{
	// ���� ������ ID�� ���´�.
	for( CX2EqipPtr pEquipPtr : m_ViewEqipList )
	{
		if ( pEquipPtr != NULL )
		{
			switch ( pEquipPtr->GetEqipPos() )
			{
			case CX2Unit::EP_WEAPON_HAND:
			case CX2Unit::EP_DEFENCE_BODY:
			case CX2Unit::EP_DEFENCE_LEG:
			case CX2Unit::EP_DEFENCE_HAND:
			case CX2Unit::EP_DEFENCE_FOOT:
				m_vecPromotionItemIDList2.push_back( pEquipPtr->GetItemID() );
				break;
			default:
				break;
			}
		}
	}

	m_vecPromotionEquipList.reserve( m_vecPromotionItemIDList.size() + m_vecPromotionItemIDList2.size() );

	for( int iItemID : m_vecPromotionItemIDList )
	{
		CX2EqipPtr pEquipPtr = CX2ItemManager::CreateDummyEquip( iItemID, GetUnit(), m_pXSkinAnim );
		pEquipPtr->SetShowObject( false );

		if ( pEquipPtr != NULL )
		{
			pEquipPtr->SetManagerObject( this );
			m_vecPromotionEquipList.push_back( pEquipPtr );

			if ( CX2Unit::EP_AC_LEG == pEquipPtr->GetEqipPos() )
				m_ptrPromotionEquipTail = pEquipPtr;
		} //if
	}

	for( int iItemID : m_vecPromotionItemIDList2 )
	{
		CX2EqipPtr pEquipPtr = CX2ItemManager::CreateDummyEquip( iItemID, GetUnit(), m_pXSkinAnim );
		pEquipPtr->SetShowObject( false );

		if ( pEquipPtr != NULL )
		{
			pEquipPtr->SetManagerObject( this );
			m_vecPromotionEquipList.push_back( pEquipPtr );
		} //if
	}

	SetShowOriginalViewList( false );
	SetShowPromotionEquips( true );
}
//#pragma endregion

//#pragma region ShowActiveSkillCutInAndLight
/** @function	: ShowActiveSkillCutInAndLight
	@brief		: �������� �ð��� ����ȭ��忡 ���� ��ų ���ΰ� light�� �����
	@param		: float fTimeToShow_, bool bOnlyLight_ (
*/
/*virtual*/ void CX2GUAra::ShowActiveSkillCutInAndLightByBerserkMode( float fTimeToShow_, bool bOnlyLight_ /*= false*/ )
{
if( m_pXSkinAnim->EventTimerOneshot( fTimeToShow_ ) == true )
	{
		if ( GetShowCutInAndChangeWorldColor() && GetShowActiveSkillShow() )
		{
			// ����ȭ ���¸�
			if ( IsFullHyperMode() )
				ActiveSkillShow( L"Dummy2_Lhand", s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[1].fileName.c_str(),
								 s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[1].vSize,
								 s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[1].vPosition, IsHyperState(), bOnlyLight_ );
			else
				ActiveSkillShow( L"Dummy2_Lhand", s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].fileName.c_str(),
								 s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].vSize,
								 s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[0].vPosition, IsHyperState(), bOnlyLight_ );
		}
	}
}
//#pragma endregion

//#pragma region NoStateChangeActionFrameMove
/** @function	: NoStateChangeActionFrameMove
	@brief		: ������Ʈ�� ��ȭ�� ���� ��ų ��� �� ����Ǵ� FrameMove
*/
/*virtual*/ void CX2GUAra::NoStateChangeActionFrameMove()
{
	switch( m_FrameDataNow.syncData.nowAction )
	{
	case UAI_REFLECTION:	/// �ƶ� 1�� ���� �Ҹ� - ��ź��
		{
			if ( NULL != GetUnit() && g_pX2Game->GetEffectSet() && false == GetRidingOn() )
			{
				/// ��ų ���� ��ȯ
				const int iSkillLevel = max( GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ALD_REFLECTION ), 1 );

				/// ��ź�� ���� ����
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_ARA_MEMO21) == true )
					SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_SI_A_ALD_REFLECTION_MEMO, iSkillLevel );
				else
				SetBuffFactorToGameUnitByBuffFactorID( BFI_BUFF_SI_A_ALD_REFLECTION, iSkillLevel );

				/// ��ź�� ����Ʈ ����
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_A_ALD_REFLECTION", this );

				/// ��ź�� ����
				PlaySound( L"Elsword_Endurance.ogg" );
				/// ��� 1 �Ҹ�
				UseForcePower( 2 );
			}
		} break;
	}
}
//#pragma endregion

//#pragma region GetActualMPConsume
/** @function 	: GetActualMPConsume
	@brief 		: ���� ��뷮 ��ȯ
*/
/*virtual*/ float CX2GUAra::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );

	switch( eSkillID_ )
	{
		case CX2SkillTree::SI_SA_AM_SHADOW_KNOT:
		{
			/// �޸� : ���� �׸��� â�� (�׸��ڸŵ�) - MP �Ҹ� 100 ����
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO5 ) )
				fMpConsumption -= 100.f;
		} break;

		case CX2SkillTree::SI_A_ASD_DRAGON_FALLING:
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO10 ) == true )
					fMpConsumption *= 0.9f;
			} break;

		case CX2SkillTree::SI_A_ALD_CONVERSION_MANA:
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO15 ) == true )
					fMpConsumption *= 0.9f;
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
//#pragma endregion

//#pragma region LoadPromotionItemID
/** @function	: LoadPromotionItemID
	@brief		: ��ũ��Ʈ���� �� Ŭ������ �ش��ϴ� ���θ�� ��� ID�� ����
	@param		: KLuaManager�� ���۷���
*/
void CX2GUAra::LoadPromotionItemID( IN KLuaManager& luaManager_ )
{
	if ( true == luaManager_.BeginTable( "PROMOTION_ITEM_ID_LIST" ) )
	{
		bool bOpenTable = false;
		switch ( GetUnit()->GetClass() )
		{
			case CX2Unit::UC_ARA_MARTIAL_ARTIST:
			{
				bOpenTable = luaManager_.BeginTable( "ARA_MARTIAL_ARTIST" );
			} break;
			case CX2Unit::UC_ARA_LITTLE_HSIEN:
				{
					bOpenTable = luaManager_.BeginTable( "ARA_LITTLE_HSIEN" );
				} break;
			case CX2Unit::UC_ARA_SAKRA_DEVANAM:
				{
					bOpenTable = luaManager_.BeginTable( "ARA_SAKRA_DEVANAM" );
				} break;
			case CX2Unit::UC_ARA_LITTLE_DEVIL:
				{
					bOpenTable = luaManager_.BeginTable( "ARA_LITTLE_DEVIL" );
				} break;
			case CX2Unit::UC_ARA_YAMA_RAJA:
				{
					bOpenTable = luaManager_.BeginTable( "ARA_YAMA_RAJA" );
				} break;
			default:
			{
				ASSERT( !L"Wrong Path" );
				bOpenTable = luaManager_.BeginTable( "ARA_MARTIAL_ARTIST" );
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
//#pragma endregion

//#pragma region NotifyShowObjectChanged
/** @function	: NotifyShowObjectChanged
	@brief		: "�ƶ�"�� ���̰ų� �Ǵ� ������ �ʰ� �ɶ� ���� ���̰ų� ������ �ʾƾ� �� �͵��� ó��
*/
/*virtual*/ void CX2GUAra::NotifyShowObjectChanged()
{
	CX2GUUser::NotifyShowObjectChanged();

	// û�� ������ �ʴ´ٸ�
	SetShowViewList( GetShowObject() );
	UpdateEquipCollisionData();
}
//#pragma endregion

//#pragma region NotifyDGManagerChanged
/** @function	: NotifyDGManagerChanged
	@brief		: "�ƶ�"�� DGManager�� ��� �Ǵ� ���� �ɶ� ���� ��� �ǰų� �����Ǿ�� �� �͵��� ó����
	@param		: CKTDGManager& refInManager_
*/
/*virtual*/ void CX2GUAra::NotifyDGManagerChanged( CKTDGManager& refInManager_ )		// DGManager�� ���
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
//#pragma endregion

//#pragma region SetShowPromotionEquips
/** @function	: SetShowPromotionEquips
	@brief		: ���θ�� ��� �������� ���θ� ����
	@param		: bool bShow_
*/
void CX2GUAra::SetShowPromotionEquips( bool bShow_ )
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
//#pragma endregion

//#pragma region SetShowViewList
/** @function	: SetShowViewList
	@brief		: ����ȭ ���� �ƴ����� ���� ��� ��� �������� �� ������
	@param		: bool bShow_
*/
/*virtual*/ void CX2GUAra::SetShowViewList( bool bShow_ )
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
//#pragma endregion

//#pragma region SetShowOriginalViewList
/** @function	: SetShowOriginalViewList
	@brief		: ViewList�� ��� ������
	@param		: bool bShow_ (�������� ���� ����)
*/
void CX2GUAra::SetShowOriginalViewList( bool bShow_ )
{
	for( CX2EqipPtr pViewEquipPtr : m_ViewEqipList )
	{
		if ( pViewEquipPtr != NULL )
		{
			if ( false != bShow_ || CX2Unit::EP_ONEPIECE_FASHION != pViewEquipPtr->GetEqipPos() )
			pViewEquipPtr->SetShowObject( bShow_ );

			if ( true == bShow_ && CX2Unit::EP_WEAPON_HAND == pViewEquipPtr->GetEqipPos() )
			{
				ReplaceWeapon( pViewEquipPtr );
			}
		}
	}
}
//#pragma endregion

//#pragma region ReplaceWeapon
/** @function	: ReplaceWeapon
	@brief		: ����ȭ���� �����ϰų� ������ �� ���� ����
	@param		: CX2EqipPtr pWeaponPtr_ ( pWeaponPtr_�� �ٲ�� )
*/
void CX2GUAra::ReplaceWeapon( CX2EqipPtr pWeaponPtr_ )
{
	CX2GameUnit::Weapon* pOldWeapon = m_vecpWeapon[0];

	if ( NULL != pOldWeapon &&
		 pOldWeapon->m_pUserWeaponEquip != NULL &&
		 pOldWeapon->m_pUserWeaponEquip != pWeaponPtr_ &&
		 pOldWeapon->m_pUserWeaponEquip->GetItem() != NULL )
	{
		int iEnchantLevel = pOldWeapon->m_pUserWeaponEquip->GetEnchantLevel();
        const CX2Item::ItemTemplet* pOldItemTemplet = pOldWeapon->m_pUserWeaponEquip->GetItem()->GetItemTemplet();

		pOldWeapon->m_pUserWeaponEquip.reset();
		SAFE_DELETE_KTDGOBJECT( pOldWeapon );
		m_vecpWeapon.resize(0);

		// ��ȭ ����Ʈ�� ���� Enchant level ����
		pWeaponPtr_->ChangeEnchantLevel( iEnchantLevel );
		CreateWeapon( pOldItemTemplet, pWeaponPtr_ );
	}
}
//#pragma endregion

//#pragma region UpdateEquipViewListInfo
/** @function	: UpdateEquipViewListInfo
	@brief		: ����� ����, Ż��, �׸��� ����ȭ ���ο� ���� ViewList�� ������
	@param		: CX2Item* pItemToAddInViewList(�߰��� ���), CX2Item* const pItemToRemoveInViewList(Ż���� ���)
	@return		: �Լ��� ���� ����
*/
/*virtual*/ bool CX2GUAra::UpdateEquipViewListInfo( CX2Item* pItemToAddInViewList, CX2Item* const pItemToRemoveInViewList )
{
	CX2EqipPtr pEquipPtrToRemove;
	int iIndexInViewListForRemove_ = 0;
	bool isNullAddList = true;

	if ( NULL != pItemToAddInViewList )
		isNullAddList = false;

	if ( false == GetAddItemAndRemoveItemToViewList( pEquipPtrToRemove, &pItemToAddInViewList, pItemToRemoveInViewList,
		 iIndexInViewListForRemove_ ) )
		return false;

	if ( pEquipPtrToRemove.get() != NULL )
	{
		RemoveEquipFromViewList( pEquipPtrToRemove, iIndexInViewListForRemove_ );
	}

	if ( pItemToAddInViewList != NULL )
	{
		AddEquipToViewList( pItemToAddInViewList );
	}

	CX2EqipPtr pBerserkEquipToRemove;

	if ( true == isNullAddList )
		pItemToAddInViewList = NULL;

	if ( false == GetAddItemAndRemoveItemToViewListBerserk( pBerserkEquipToRemove, &pItemToAddInViewList, pItemToRemoveInViewList,
		iIndexInViewListForRemove_ ) )
		return false;

	if ( NULL != pBerserkEquipToRemove.get() )
	{
		RemoveBerserkEquipFromViewList( pBerserkEquipToRemove, iIndexInViewListForRemove_ );
	}

	if ( pItemToAddInViewList != NULL )
	{
		AddBerserkEquipToViewList( pItemToAddInViewList );
	}

	// �̰��� ����ϴ� ��ƾ���� UpdateEquipCollisionData()�� ȣ���ϱ� ������
	// ���⼭�� ȣ�� �� �ʿ䰡 ����.
	return true;
}
//#pragma endregion

//#pragma region UpdateEquipCollisionData
/** @function	: UpdateEquipCollisionData
	@brief		: ���� ���̴� ��� ���� �浹 �����͸� �����Ѵ�.
*/
/*virtual*/ void CX2GUAra::UpdateEquipCollisionData()
{
	// ����ȭ ��� �̸�
	if ( IsFullHyperMode() )
	{
		UpdateNotRenderingOtherEquipList();
		CX2GUUser::UpdateEquipCollisionData( m_vecPromotionEquipList );
	}
	// �Ϲ� ��� �̸�
	else
	{
		CX2GUUser::SetShowViewList( true );
		CX2GUUser::UpdateNotRenderingOtherEquipList();
		CX2GUUser::UpdateEquipCollisionData( m_ViewEqipList );
	}

	////// ���� �ڽ� �� ����?
	SetEnableAttackBox( L"Rfoot", false );
	SetEnableAttackBox( L"Lfoot", false );
	SetEnableAttackBox( L"Lhand2", false );
}
//#pragma endregion

//#pragma region UpdateNotRenderingOtherEquipList
/*virtual*/ void CX2GUAra::UpdateNotRenderingOtherEquipList()
{
	if ( IsFullHyperMode() )
	{
		SetHideNoRenderable(false);

		// oasis907 : ����� [2011.8.22] �ѹ� ���� �ƹ�Ÿ
		bool bOnePieceAvatarCheck = false;

		CX2EqipPtr pNoRenderFactorEquipBody = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_ONEPIECE_FASHION );

		if( pNoRenderFactorEquipBody != NULL )
		{
			if ( IsOnePieceAvatarRenderHair( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					if( IsOnePieceAvatarWithNotRenderHairItem( pViewEquip->GetItemID() ) == true )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_AC_FACE2:
						case CX2Unit::EP_DEFENCE_LEG:
						case CX2Unit::EP_DEFENCE_HAND:
						case CX2Unit::EP_DEFENCE_FOOT:
						case CX2Unit::EP_DEFENCE_BODY:
							{
								pViewEquip->SetShowObject( false );
								bOnePieceAvatarCheck = true;
							}
							break;

						default:
							break;
						}
					}
					else
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_DEFENCE_LEG:
						case CX2Unit::EP_DEFENCE_HAND:
						case CX2Unit::EP_DEFENCE_FOOT:
						case CX2Unit::EP_DEFENCE_BODY:
							{
								pViewEquip->SetShowObject( false );
								bOnePieceAvatarCheck = true;
							}
							break;

						default:
							break;
						}
					}
				}
			}
			if ( IsOnePieceAvatar( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
			if ( IsOnePieceAvatarNotRenderFace1( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
			if ( IsOnePieceAvatarNotRenderFace1Face2( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_AC_FACE2:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
			if ( IsOnePieceAvatarNotRenderFace( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				SetHideNoRenderable(true);
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_AC_FACE2:
					case CX2Unit::EP_AC_FACE3:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
			/// �ѹ� �ƹ�Ÿ �ܿ� ���� ��� �Ǽ������� ���� �ϴ� ������
			if ( IsOnePieceAvatarNotRenderHairAndAllAccessory( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_AC_FACE2:
					//case CX2Unit::EP_AC_FACE3:	/// ��û���� ���� �� (��) ����
					case CX2Unit::EP_AC_BODY:
					case CX2Unit::EP_AC_ARM:
					case CX2Unit::EP_AC_NECKLESS:
					case CX2Unit::EP_DEFENCE_BODY:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
			if ( IsOnePieceAvatarRenderingHairAndFace1( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
						{
							pViewEquip->SetShowObject( false );
						}
						break;

					default:
						break;
					}
				}
			}
			// �� ��, ��, �� ���� ��� �Ǽ�, ����� ������ ��
			if ( IsOnePieceAvatarOnlyRenderFace1Face2Face3Equip( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_LEG:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_DEFENCE_FOOT:
					case CX2Unit::EP_AC_BODY:
					case CX2Unit::EP_AC_ARM:
					case CX2Unit::EP_AC_NECKLESS:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
			if ( IsOnePieceAvatarWithRenderBodyItem( pNoRenderFactorEquipBody->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAIR:
					case CX2Unit::EP_DEFENCE_HAND:
					case CX2Unit::EP_AC_FACE1:
					case CX2Unit::EP_AC_FACE2:
					//case CX2Unit::EP_AC_FACE3:	/// ��û���� ���� �� (��) ����
					case CX2Unit::EP_AC_NECKLESS:
						{
							pViewEquip->SetShowObject( false );
							bOnePieceAvatarCheck = true;
						}
						break;

					default:
						break;
					}
				}
			}
		}

		CX2EqipPtr pNoRenderFactorEquipACArm = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_AC_ARM );
		if( pNoRenderFactorEquipACArm != NULL )
		{
			if ( IsNoRenderHandEquip( pNoRenderFactorEquipACArm->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_DEFENCE_HAND:
						{
							if ( false == IsFullHyperMode() )
							pViewEquip->SetShowObject( false );
						}
						break;

					default:
						break;
					}
				}
			}
		}

		//CX2Item* pItem = GetUnit()->GetInventory().GetEquippingItemByEquipPos( CX2Unit::EP_AC_FACE1, false );
		// EP_AC_FACE1 ������ġ�� �ִ� �������� id�� ����
		CX2EqipPtr pNoRenderFactorEquip = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_AC_FACE1 );

		if ( pNoRenderFactorEquip == NULL )
			return;
		else
		{
			// kimhc // �� �̻� �Ⱥ����� �� �з��� �޶� ���ٸ�
			// �Ⱥ������� ���� �� �׷� ����� ������ �ִ� �������� ��ũ��Ʈ�� �����ҵ�
			// ����� �� �Ⱥ��̰� �ϴ� �����۰� ���, �� �Ǽ�(��,��)�� �Ⱥ��̰� �ϴ� �������� ���� ��ġ �̹Ƿ�
			// �Ѱ��� ���� �� �� ����
			if ( IsNoRenderHairFace2Face3Equip( pNoRenderFactorEquip->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE2:
					case CX2Unit::EP_AC_FACE3:
						pViewEquip->SetShowObject( false );
						break;

					default:
						break;
					}
				}
			}

			if ( IsNoRenderHairFace2Equip( pNoRenderFactorEquip->GetItemID() ) == true )
			{
				for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
				{
					switch ( pViewEquip->GetEqipPos() )
					{
					case CX2Unit::EP_AC_FACE2:
						pViewEquip->SetShowObject( false );
						break;

					default:
						break;
					}
				}
			}


			else if ( IsNoRenderHair( pNoRenderFactorEquip->GetItemID() ) == true )
			{
				if( bOnePieceAvatarCheck == true )
				{
					for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_AC_FACE1:
						case CX2Unit::EP_AC_FACE2:
							pViewEquip->SetShowObject( false );
							break;

						default:
							break;
						}
					}
				}
				else
				{
					for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_DEFENCE_HAIR:
							if ( false == IsFullHyperMode() )
								pViewEquip->SetShowObject( false );
							break;

						default:
							break;
						}
					}
				}
			}
			else if ( IsNoRenderHairFaceFace2Face3Equip( pNoRenderFactorEquip->GetItemID() ) == true )
			{
				if(bOnePieceAvatarCheck == true)
				{
					for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_AC_FACE1:
							pViewEquip->SetShowObject( false );
							break;

						default:
							break;
						}
					}
				}
				else
				{
					SetHideNoRenderable(true);
					for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_AC_FACE2:
						case CX2Unit::EP_AC_FACE3:
							pViewEquip->SetShowObject( false );
							break;
						default:
							break;
						}
					}
				}
			}
		}

		pNoRenderFactorEquip = GetEquipPtrFromEquipViewListByEquipPosition( CX2Unit::EP_AC_FACE2 );

		if ( pNoRenderFactorEquip != NULL )
		{
			if ( IsNoRenderHairFaceFace1Face3Equip( pNoRenderFactorEquip->GetItemID() ) == true )
			{
				if(bOnePieceAvatarCheck == true)
				{
					for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_AC_FACE2:
							pViewEquip->SetShowObject( false );
							break;

						default:
							break;
						}
					}
				}
				else
				{
					SetHideNoRenderable(true);
					for( CX2EqipPtr pViewEquip : m_vecPromotionEquipList )
					{
						switch ( pViewEquip->GetEqipPos() )
						{
						case CX2Unit::EP_AC_FACE1:
						case CX2Unit::EP_AC_FACE3:
							pViewEquip->SetShowObject( false );
							break;
						default:
							break;
						}
					}
				}
			}
		}
	}
}
//#pragma endregion ������ ���� �ʾƵ� �Ǵ� ��� ����Ʈ ����

//#pragma region GetAddItemAndRemoveItemToViewListBerserk
bool CX2GUAra::GetAddItemAndRemoveItemToViewListBerserk( OUT CX2EqipPtr& pEquipPtrToRemove_, IN OUT CX2Item** ppItemToAddInViewList_,
	IN CX2Item* const pItemToRemoveInViewList_, OUT int& iIndexInViewListForRemove_ )
{
	if (*ppItemToAddInViewList_ != NULL )		// ������et ��� �ִ� ���
	{
		const CX2Item::ItemTemplet* pItemTempletForItemToAdd = NULL;
		pItemTempletForItemToAdd = (*ppItemToAddInViewList_)->GetItemTemplet();

		if ( pItemTempletForItemToAdd == NULL )
		{
			ASSERT( false );
			return false;
		}

		for ( size_t numViewEquipInUser = 0; numViewEquipInUser < m_vecPromotionEquipList.size(); numViewEquipInUser++ )
		{
			if ( pItemTempletForItemToAdd->GetEqipPosition() == m_vecPromotionEquipList[numViewEquipInUser]->GetEqipPos() )
			{
				// ������ ��� Normal�̰� �ش� ��ġ�� �̹� Fashion ���� �ִ� ��쿡�� ������ �������� View���� ���� ���� �߰��� ���� ����
				if (  pItemTempletForItemToAdd->GetFashion() == false && GetUnit()->IsFashionEquipInThePosition( pItemTempletForItemToAdd->GetEqipPosition() ) == true )
				{
					*ppItemToAddInViewList_ = NULL;
				}
				else
				{
					pEquipPtrToRemove_ = m_vecPromotionEquipList[numViewEquipInUser];
					// �̺κп��� remove �ؾ��ҵ�
				}
				iIndexInViewListForRemove_ = numViewEquipInUser;
				break;
			}
		}
	}
	else if ( pItemToRemoveInViewList_ != NULL )		// ��� ������ ���� ���
	{
		const CX2Item::ItemTemplet* pItemTempletForItemToRemove = NULL;
		pItemTempletForItemToRemove = pItemToRemoveInViewList_->GetItemTemplet();

		if ( pItemTempletForItemToRemove == NULL)
		{
			ASSERT( false );
			return false;
		}

		for ( size_t numViewEquipInUser = 0; numViewEquipInUser < m_vecPromotionEquipList.size(); numViewEquipInUser++ )
		{
			if ( pItemTempletForItemToRemove->GetEqipPosition() == m_vecPromotionEquipList[numViewEquipInUser]->GetEqipPos() )
			{
				switch ( static_cast<int>( pItemTempletForItemToRemove->GetFashion() ) )
				{
				case true:	// ���� �� ��� �м� ���� ���
					{
						pEquipPtrToRemove_ = m_vecPromotionEquipList[numViewEquipInUser];	// ������ ���

						// ������ ���� �븻 ��� �ִ� ���
						if ( GetUnit()->IsNormalEquipInThePosition( pItemTempletForItemToRemove->GetEqipPosition() ) == true )
						{
							CX2Item* pNormalEquip	= NULL;

							pNormalEquip	= GetUnit()->GetInventory().GetEquippingItemByEquipPos( pItemTempletForItemToRemove->GetEqipPosition(), false );

							if ( pNormalEquip == NULL )
								return false;

							if ( pNormalEquip->GetItemData().m_PeriodType != CX2Item::PT_ENDURANCE		// �������� ���� ���ų�
								|| pNormalEquip->GetItemData().m_Endurance > 0)		// �������� 0���ϰ� �ƴ϶�� �븻 ��� �־���
								*ppItemToAddInViewList_ = pNormalEquip;
							else
								*ppItemToAddInViewList_ = GetUnit()->CreateBasicEquip( pItemTempletForItemToRemove->GetEqipPosition() );

						}
						else	// ������ ���� �븻 ��� ���� ���
						{
							*ppItemToAddInViewList_ = GetUnit()->CreateBasicEquip( pItemTempletForItemToRemove->GetEqipPosition() );
						}
					}
					break;

				case false: // ���� �� �������� �븻 ���� ���
					{
						// ������ ���� �м� �۵� ���� ���
						if (  GetUnit()->IsFashionEquipInThePosition( pItemTempletForItemToRemove->GetEqipPosition() ) == false )
						{
							pEquipPtrToRemove_		= m_vecPromotionEquipList[numViewEquipInUser];	// ������ ���
							*ppItemToAddInViewList_	= GetUnit()->CreateBasicEquip( pItemTempletForItemToRemove->GetEqipPosition() );
						}
					}
					break;
				}

				iIndexInViewListForRemove_ = numViewEquipInUser;
				break;	// for���� ����
			}
		} // for ( size_t numViewEquipInUser = 0; numViewEquipInUser < m_ViewEqipList.size(); numViewEquipInUser++ )
	}

	return true;
}
//#pragma endregion

//#pragma region AddBerserkEquipToViewList
void CX2GUAra::AddBerserkEquipToViewList( CX2Item* const pItem )
{
	if ( pItem == NULL || pItem->GetItemTemplet() == NULL )
		return;

	const CX2Item::ItemTemplet* pItemTemplet = pItem->GetItemTemplet();
	short ePos = pItemTemplet->GetEqipPosition();

	if ( CX2Unit::EP_WEAPON_HAND != ePos && CX2Unit::EP_DEFENCE_BODY != ePos && CX2Unit::EP_DEFENCE_LEG != ePos &&
		CX2Unit::EP_DEFENCE_HAND != ePos && CX2Unit::EP_DEFENCE_FOOT != ePos )
		return;

	CX2EqipPtr	pCX2EquipPtr;
	int			enchantLevel = 0;

	enchantLevel = pItem->GetItemData().m_EnchantLevel;
    bool bBasicEquip = GetUnit()->IsBasicEquip( pItemTemplet->GetItemID() );

	if( pItemTemplet->GetUseCondition() == CX2Item::UC_ANYONE )
	{
		pCX2EquipPtr = CX2Eqip::CreateEquip( pItem, m_pXSkinAnim, enchantLevel, bBasicEquip, true, GetUnit()->GetType() );
	}
	else
	{
		pCX2EquipPtr = CX2Eqip::CreateEquip( pItem, m_pXSkinAnim, enchantLevel, bBasicEquip, true );
	}

	pCX2EquipPtr->SetManagerObject( this );
	// ���ο� ��� Ư���ɷ��� �پ� �־��ٸ� Ư���ɷ� �ο��� �ֱ�

	if ( IsRecursivelyManagedByDGManager() )
		g_pKTDXApp->GetDGManager()->AddObjectChain( pCX2EquipPtr.get() );

	m_vecPromotionEquipList.push_back( pCX2EquipPtr );

	if ( false == IsFullHyperMode() )
		return;

	int						weaponIndex		= 0;

	switch ( pItemTemplet->GetEqipPosition() )
	{
	case CX2Unit::EP_WEAPON_HAND:
		weaponIndex = 0;				// InitEqip �κп��� 0�� 1�� ���еǾ� ����
		break;

	case CX2Unit::EP_WEAPON_SECOND:
		weaponIndex = 1;
		break;
	default:
		break;
	}

	// ���ο� ��� Weapon �� ���
	switch ( pItemTemplet->GetEqipPosition() )
	{
	case CX2Unit::EP_WEAPON_HAND:
	case CX2Unit::EP_WEAPON_SECOND:
		{
			Weapon* pWeapon = Weapon::CreateWeapon( *this );
			ASSERT( pWeapon != NULL );

			pWeapon->m_iWeaponIndex			= weaponIndex;
			pWeapon->m_pUserWeaponEquip		= pCX2EquipPtr;
			pWeapon->m_bUseSlashTrace		= true;


            pItemTemplet->GetSlashTraceColors( pWeapon->m_coSlashTrace, pWeapon->m_coSlashTraceTip
                , pWeapon->m_coSlashTraceHyper, pWeapon->m_coSlashTraceTipHyper, CX2Unit::UT_ARA );


			if( NULL != g_pX2Game &&
				NULL != g_pX2Game->GetMajorParticle() )
			{
                unsigned uNumParticle = pItemTemplet->GetNumAttachedParticleData();
				for( size_t numParticle = 0; numParticle < uNumParticle; numParticle++ )
				{
                    const wchar_t*  pwszParticleName = L"";
                    const wchar_t*  pwszBoneName = L"";
                    pItemTemplet->GetAttachedParticleData( numParticle, &pwszParticleName, &pwszBoneName );
					CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwszParticleName, D3DXVECTOR3( 0, 0, 0 ) );
					if( NULL != pSeq )
					{
						AttachWeaponParticle( pWeapon->m_iWeaponIndex, true, pwszBoneName, pSeq->GetHandle() );
					}
				}
			}

			m_vecpWeapon.push_back( pWeapon );
		}
		break;	// switch ( pItemTemplet->GetEqipPosition() )
	default:
		break;
	}
}
//#pragma endregion

//#pragma region RemoveBerserkEquipFromViewList
void CX2GUAra::RemoveBerserkEquipFromViewList( CX2EqipPtr pEquipPtrToRemove, int index )
{
	CX2Unit::EQIP_POSITION ePos = pEquipPtrToRemove->GetEqipPos();

	CX2Unit::EQIP_POSITION	equipPosition	= pEquipPtrToRemove->GetEqipPos();
	int						weaponIndex		= 0;

	if ( CX2Unit::EP_WEAPON_HAND != ePos && CX2Unit::EP_DEFENCE_BODY != ePos && CX2Unit::EP_DEFENCE_LEG != ePos &&
		CX2Unit::EP_DEFENCE_HAND != ePos && CX2Unit::EP_DEFENCE_FOOT != ePos )
		return;

	m_vecPromotionEquipList.erase( m_vecPromotionEquipList.begin() + index );

	if ( false == IsFullHyperMode() )
		return;

	switch ( equipPosition )
	{
	case CX2Unit::EP_WEAPON_HAND:
		weaponIndex = 0;				// InitEqip �κп��� 0�� 1�� ���еǾ� ����
		break;

	case CX2Unit::EP_WEAPON_SECOND:
		weaponIndex = 1;
		break;
	default:
		break;
	}

	if ( false == IsFullHyperMode() )
		return;

	switch ( equipPosition )
	{
	case CX2Unit::EP_WEAPON_HAND:		// pWeapon->m_iWeaponIndex�� 0�ΰ�
	case CX2Unit::EP_WEAPON_SECOND:

		for ( size_t numWeapon = 0; numWeapon < m_vecpWeapon.size(); numWeapon++ )
		{
			Weapon* pWeapon	= m_vecpWeapon[numWeapon];

			if ( pWeapon->m_iWeaponIndex == weaponIndex )
			{
				m_vecpWeapon.erase( m_vecpWeapon.begin() + numWeapon );
				Weapon::DeleteKTDGObject( pWeapon );

				--numWeapon;	// added by wonpok, 20100318
			}
		}

		break;	// switch
	default:
		break;
	}
}
//#pragma endregion

//#pragma region SLockOnSystem::ProcessLockOn
void CX2GUAra::SLockOnSystem::ProcessLockOn( CX2GUAra* myUnit )
{
	int iSize = 0;
	float fDistance = m_fPveRange * m_fPveRange;
	vector<CX2GameUnit*> vecAreaUnit;
	D3DXVECTOR3 pos = myUnit->GetPos();

	for( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( pUnit->GetTeam() == myUnit->GetTeam() )
			continue;

		if( true == pUnit->GetIsBosRaidNPC() )
			continue;

		if( pUnit->GetInvincible() == true )
			continue;

		if( pUnit->GetNowHp() <= 0.f )
			continue;

		if( pUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC )
		{
			CX2GUNPC* pNPC = (CX2GUNPC*) pUnit;
			switch( pNPC->GetNPCTemplet().m_ClassType )
			{
			case CX2UnitManager::NCT_THING_TRAP:
			case CX2UnitManager::NCT_THING_BOX:
			case CX2UnitManager::NCT_THING_GATE:
			case CX2UnitManager::NCT_THING_CHECKER:
				{
					continue;
				} break;
			default:
				break;
			}
		}

		if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
			fDistance = m_fPvpRange * m_fPvpRange;

		if( GetDistance3Sq( pos, pUnit->GetPos() ) < fDistance )
		{
			vecAreaUnit.push_back( pUnit );
		}
	}

	iSize = vecAreaUnit.size();

	if ( 1 < iSize )
	{
		int iCount = 0;

		for( CX2GameUnit* pUnit : vecAreaUnit )
		{
			if ( m_BeforeTargetUID == pUnit->GetUnitUID() )
				vecAreaUnit.erase( vecAreaUnit.begin() + iCount );

			++iCount;
		}
	}

	iSize = vecAreaUnit.size();

	if ( 0 < iSize )
	{
		int iRandValue = myUnit->GetRandomInt() % iSize;

		CX2GameUnit* pUnit = vecAreaUnit.at( iRandValue );

		if ( NULL != pUnit )
		{
			m_vDestPos = pUnit->GetPos();
			m_BeforeTargetUID = pUnit->GetUnitUID();


			if( NULL != g_pX2Game->GetWorld() && NULL != g_pX2Game->GetWorld()->GetLineMap() )
			{
				D3DXVECTOR3 outPos;
				int			lineIndex = 0;

				if( g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_vDestPos, LINE_RADIUS, &outPos, &lineIndex, false ) == true )
				{
					const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( lineIndex );
					if( NULL != pLineData )
					{
						if( pLineData->lineType == CKTDGLineMap::LT_NORMAL && pLineData->bEnable == true )
							SetSafePos( m_vDestPos );
					}
				}
			}

			m_bLockOnUnit = true;
		}

	}
	else
	{
		m_bLockOnUnit = false;
	}
}
//#pragma endregion

//#pragma region OnFrameMove
/** @function	: OnFrameMove
	@brief		: �ƶ��� ������ ���� �Լ� ( ������ ����Ʈ�� ������Ʈ �� �ʿ䰡 ������ ������Ʈ�� )
	@param		: double fTime, float fElapsedTime
	@return		: HRESULT
*/
/*virtual*/ HRESULT CX2GUAra::OnFrameMove( double fTime, float fElapsedTime )
{
	CX2GUUser::OnFrameMove( fTime, fElapsedTime );

	if ( GetNeedToUpdateWeaponEffect() )
	{
		UpdateWeaponEnchantEffectAndExtraDamageType();
		SetNeedToUpdateWeaponEffect( false );
	}

	return S_OK;
}
//#pragma endregion

//#pragma region DamageReact
/** @function 	: DamageReact
	@brief 		: �ǰ� ������ ���� ó�� �κ�

	@param pDamageDate	: DamageDate ������
*/
/*virtual*/ void CX2GUAra::DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ )
{
	KTDXPROFILE();

	/// �ƶ� 1�� ���� �Ҽ� - ��� Ȱ��ȭ
	CX2DamageManager::EXTRA_DAMAGE_TYPE& eEDT = pDamageData_->m_ExtraDamage.m_ExtraDamageType;

	switch( eEDT )		/// �ش� EDT( ��þƮ �Ӽ� )�� ���� ���ӽð� ����
	{
	case CX2DamageManager::EDT_ENCHANT_BLAZE:
	case CX2DamageManager::EDT_ENCHANT_POISON:
	case CX2DamageManager::EDT_ENCHANT_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN:

	case CX2DamageManager::EDT_ENCHANT_BLAZE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_MASTER:
	case CX2DamageManager::EDT_ENCHANT_MASTER_PIERCING:

	case CX2DamageManager::EDT_ENCHANT_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_MASTER:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_MASTER_PIERCING:

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_BLAZE_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_FROZEN_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_DOUBLE_POISON_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_BLAZE_DOUBLE_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_FROZEN_DOUBLE_SNATCH:

	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_PIERCING:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SHOCK:
	case CX2DamageManager::EDT_ENCHANT_POISON_DOUBLE_SNATCH:
		{
			pDamageData_->m_ExtraDamage.m_fTime *= m_fReduceEDTDurationRate;
		} break;
	default:
		break;
	}

	/// �޸� ȿ�� �������� ���Ͽ� ������ ������, ���� ��ų ������ ��� ����
	/*switch( GetNowStateSkillID() )
	{
	case CX2SkillTree::SI_A_ALH_ROCK_CRASH:	/// ��������
		{
			/// �޸� : ������ ���� ġ��(��ȣ 1�� : ��������) - ���� ������ 50% ����
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO6 ) )
			{
				int iReduceDamagePercent  = GetDamageReducePercent() + _CONST_ARA_::ROCK_CRASH_MEMO_RESUCE_DAMAGE;

				if( 100 < iReduceDamagePercent )
					iReduceDamagePercent = 100;

				SetDamageReducePercent( iReduceDamagePercent );
			}
		} break;
	}*/

	CX2GUUser::DamageReact( pDamageData_ );

	CX2GUUser::DamageReactStateChange( pDamageData_, ASI_A_ALH_BREAKING_ATTACK, ASI_A_ALH_BREAKING_AVOIDANCE );
}
//#pragma endregion

//#pragma region Win
/** @function 	: Win
	@brief 		: �¸����� ���� ���·� ����
*/
/*virtual*/ void CX2GUAra::Win()
{
	CX2GUUser::Win();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}
//#pragma endregion

//#pragma region Lose
/** @function 	: Lose
	@brief 		: �й����� ���� ���·� ����
*/
/*virtual*/ void CX2GUAra::Lose()
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
//#pragma endregion

//#pragma region Draw
/** @function	: Draw
	@brief		: ����� ���� ���·� ����
*/
/*virtual*/ void CX2GUAra::Draw()
{
	CX2GUUser::Draw();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}
//#pragma endregion

//#pragma region DeleteMinorParticle
/** @function 	: DeleteMinorParticle
	@brief 		: Minor Particle �޸� ����
*/
void CX2GUAra::DeleteMinorParticle()
{
	CX2GUUser::DeleteMinorParticle();

	DeleteAraMinorParticle();
}
//#pragma endregion

//#pragma region IsFullHyperMode
/** @function 	: IsFullHyperMode
	@brief 		: 3���� ����
*/
/*virtual*/bool CX2GUAra::IsFullHyperMode() const
{
	return GetAraGageData()->GetBerserkMode();
}
//#pragma endregion

//#pragma region SetFullHyperMode
/** @function 	: SetFullHyperMode
	@brief 		: 3���� ����
*/
/*virtual*/ void CX2GUAra::SetFullHyperMode( const bool bBerserkMode_ )
{
	GetAraGageData()->SetBerserkModeChanged( true );
	GetAraGageData()->SetBerserkMode( bBerserkMode_ );
}
//#pragma endregion

//#pragma region ApplyHyperModeBuff
/** @function	: ApplyHyperModeBuff
	@brief		: �������� ����
*/
/*virtual*/ void CX2GUAra::ApplyHyperModeBuff()
{
	CX2GameUnit::ApplyHyperModeBuff();

	//if ( IsFullHyperMode() )	/// Ǯ���� ���۽� ������ ����Ʈ
		//g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ARA_BERSERK_MODE_START", this );
}
//#pragma endregion

//#pragma region HyperModeBuffEffectStart
/** @function	: HyperModeBuffEffectStart
	@brief		: ���� �� ���;��ϴ� ����Ʈ�� ����
*/
/*virtual*/ void CX2GUAra::HyperModeBuffEffectStart()
{
	// ����ȭ
	if ( IsFullHyperMode() )
	{
		SetShowViewList( true );
		// BerserkMode�� Set�� ������ �� �Ŀ� UpdateEquipCollisionData()�� ȣ���ؾ� �Ѵ�.
		UpdateEquipCollisionData();
		// ���⿡ �ٴ� ����Ʈ ���� ����
		SetNeedToUpdateWeaponEffect( true );

 		for( CX2EqipPtr pEquipPtr : m_vecPromotionEquipList )
 		{
 			pEquipPtr->ChangeHyperMode();
 		}
	}
	else
	{
		if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRFoot )
			m_hHyperBoostRFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightOrange",	0, 0, 0, 0, 0);

		if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLFoot )
			m_hHyperBoostLFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftOrange",	0, 0, 0, 0, 0);

		if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRArm )
			m_hHyperBoostRArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightOrange",	0, 0, 0, 0, 0);

		if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLArm )
			m_hHyperBoostLArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftOrange",	0, 0, 0, 0, 0);

		for( CX2EqipPtr pEquipPtr : m_ViewEqipList )
		{
			pEquipPtr->ChangeHyperMode();
		}
	}
}
//#pragma endregion

//#pragma region HyperModeBuffEffectEnd
/** @function	: HyperModeBuffEffectEnd
	@brief		: ���� ����� ��������Ʈ ������ ���� ����Ǵ� �Լ�
*/
/*virtual*/ void CX2GUAra::HyperModeBuffEffectEnd()
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
		SetNeedToUpdateWeaponEffect( true );

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
//#pragma endregion

//#pragma region SetSpecificValueByBuffTempletID
/** @function	: SetSpecificValueByBuffTempletID
	@brief		: �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param		: �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUAra::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_HYPER_MODE:
		{
			if ( IsFullHyperMode() )		/// �ڱ� �ڽ� �Ӹ��� �ƴ϶� ������ Ŭ���̾�Ʈ������ �Ǵ� �ؾ� �ϹǷ� Now�� ����
				SetBuffFactorToGameUnitByBuffFactorID( BFI_ARA_FULL_HYPER_MODE );
		} break;

	default:
		break;
	}
}
//#pragma endregion

//#pragma region UnSetSpecificValueByBuffTempletID
/** @function	: UnSetSpecificValueByBuffTempletID
	@brief		: �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param		: �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUAra::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_HYPER_MODE:
		{
			ReserveToFinishBuffTempletFromGameUnit( BTI_ARA_FULL_HYPER_MODE );
		} break;

	default:
		break;
	}
}
//#pragma endregion
/** @function : ApplyRenderParam
	@brief : ������ ���� renderparam�� �Բ� �߰������� �غ��ٽ��� ���� renderparam�� �����ϴ� �Լ�
	@param : SkinAnim�� RenderParam ������(pRenderParam_)

*/
/*virtual*/ void CX2GUAra::ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if ( m_bPassDash )
		return;

	if( true == m_bInvisibility )
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON;
		pRenderParam_->outLineColor.a	= m_fRenderParamColorA;
		m_RenderParam.color.a			= m_fRenderParamColorA;
		m_RenderParam.bAlphaBlend = true;
	}
	else
		CX2GameUnit::ApplyRenderParam( pRenderParam_ );
}

//#pragma region ApplyRenderParamOnEmptyRenderParamByBuff
/** @function 	: ApplyRenderParamOnEmptyRenderParamByBuff
	@brief 		:
*/
/*virtual*/ void CX2GUAra::ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if( GetRemainHyperModeTime() > 0.0f )
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
		pRenderParam_->outLineColor		= 0xffcc9933;	// ��Ȳ��
	}
	else
	{
		pRenderParam_->renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
		pRenderParam_->cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
		pRenderParam_->outLineColor		= 0xffcc9933;
	}

	SetUseWorldColor( true );
}
//#pragma endregion

//#pragma region ChargeMpAndEtcInTrainingGame
/** @function 	: ChargeMpAndEtcInTrainingGame
	@brief 		: ??
*/
/*virtual*/ void CX2GUAra::ChargeMpAndEtcInTrainingGame()
{
	CX2GUUser::ChargeMpAndEtcInTrainingGame();
}
//#pragma endregion

//#pragma region GetHandleAraMajorParticleByEnum
/** @function 	: GetHandleAraMajorParticleByEnum
	@brief 		: "�ƶ�"�� ����ϴ� ��ƼŬ �ڵ��� ��ȯ
*/
__forceinline CKTDGParticleSystem::CParticleEventSequenceHandle CX2GUAra::GetHandleAraMajorParticleByEnum( ARA_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const
{
    ASSERT( ARA_MAJOR_PII_END > eVal_ && ARA_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
	return m_ahAraMajorParticleInstance[eVal_];
}
//#pragma endregion

//#pragma region SetHandleAraMajorParticleByEnum
/** @function 	: SetHandleAraMajorParticleByEnum
	@brief 		: "�ƶ�"�� ����ϴ� ������ ��ƼŬ �ڵ� �� ����
*/
__forceinline void CX2GUAra::SetHandleAraMajorParticleByEnum( ARA_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )
{
    ASSERT( ARA_MAJOR_PII_END > eVal_ && ARA_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
	m_ahAraMajorParticleInstance[eVal_] = hHandle_;
}
//#pragma endregion

//#pragma region GetHandleAraMinorParticleByEnum
/** @function 	: GetHandleAraMinorParticleByEnum
	@brief 		: �ƶ� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����
*/
__forceinline CKTDGParticleSystem::CParticleEventSequenceHandle	CX2GUAra::GetHandleAraMinorParticleByEnum( ARA_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const
{
    ASSERT( ARA_MINOR_PII_END > eVal_ && ARA_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
	return m_ahAraMinorParticleInstance[eVal_];
}
//#pragma endregion

//#pragma region SetHandleAraMinorParticleByEnum
/** @function 	: SetHandleAraMinorParticleByEnum
	@brief 		: û�� ���� ���̳� ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������
*/
__forceinline void CX2GUAra::SetHandleAraMinorParticleByEnum( ARA_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )
{
    ASSERT( ARA_MINOR_PII_END > eVal_ && ARA_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
	m_ahAraMinorParticleInstance[eVal_] = hHandle_;
}
//#pragma endregion

//#pragma region GetHandleAraMajorMeshByEnum
/** @function 	: GetHandleAraMajorMeshByEnum
	@brief 		: ������ �޽� �ڵ� ���
*/
CKTDGXMeshPlayer::CXMeshInstanceHandle CX2GUAra::GetHandleAraMajorMeshByEnum( ARA_MAJOR_MESH_INSTANCE_ID eVal_ ) const
{
    ASSERT( ARA_MAJOR_MII_END > eVal_ && ARA_MAJOR_MESH_INSTANCE_ID(0) <= eVal_ );
	return m_ahAraMajorMeshInstance[eVal_];
}
//#pragma endregion

//#pragma region CX2AraGageData
/*virtual*/ CX2GageData* CX2GUAra::CreateGageData()
{
	if ( IsMyUnit() )
	{
		const CX2AraGageData* pAraGageData
			= static_cast<const CX2AraGageData*>( CX2GageManager::GetInstance()->GetMyGageData() );

		if ( NULL != pAraGageData )
			return new CX2AraGageData( *pAraGageData );
		else
			return new CX2AraGageData();
	}
	else
		return new CX2AraGageData();
}
//#pragma endregion

//#pragma region AppendDeviceToDeviceList
/** @function 	: AppendDeviceToDeviceList
	@brief 		: � ����� �ϴ� �Լ��ϱ�?
*/
/*static*/ void CX2GUAra::AppendDeviceToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}
//#pragma endregion

//#pragma region AppendComponentToDeviceList
/** @function 	: AppendComponentToDeviceList
	@brief 		: � ����� �ϴ� �Լ��ϱ�?
*/
/*static*/ void CX2GUAra::AppendComponentToDeviceList( CKTDXDeviceDataList& /*listInOut_*/ )
{
}
//#pragma endregion

//#pragma region AppendEffectToDeviceList
/** @function 	: AppendEffectToDeviceList
	@brief 		:
*/
void CX2GUAra::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUAra::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUAra::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForSwordToDeviceList( listInOut_, eDamageType_ );
}
//#pragma endregion

//#pragma region May the force be with you
/** @function 	: UpForcePower
	@brief 		: ��� ĭ�� ä��
*/
__forceinline void CX2GUAra::UpForcePower( int iUpCount_ )
{
	if( 0 < m_iGainMpByFpConversion )
	{
		// ���� �� FP
		int iChangeForcePower = GetForcePower() + iUpCount_ ;
		// �ʰ��Ǵ� FP
		const int iExceedFpCount = iChangeForcePower - GetMaxForcePower() ;
		if( 0 < iExceedFpCount )
		{
			//�ʰ��Ǵ� FP�� MP�� ��ȯ
			UpNowMp( static_cast<float>(iExceedFpCount * m_iGainMpByFpConversion) );
		}
		SetForcePower( iChangeForcePower );
	}
	else
	{
		SetForcePower( GetForcePower() + iUpCount_ );
	}
}

/** @function 	: CanUseForcePower
	@brief 		: ����� ����� �� �ִ��� Ȯ��
*/
__forceinline bool CX2GUAra::CanUseForcePower()
{
	return ( false == IsEmptyForcePower() );
}

/** @function 	: IsEmptyForcePower
	@brief 		: ����� ������� Ȯ��
*/
__forceinline bool CX2GUAra::IsEmptyForcePower()
{
	return GetAraGageData()->IsEmptyForcePower();
}

/** @function 	: IsFullForcePower
	@brief 		: ����� ���� á���� Ȯ��
*/
__forceinline bool CX2GUAra::IsFullForcePower()
{
	return GetAraGageData()->IsFullForcePower();
}

/** @function 	: UpForcePowerAtThisAnimationTime
	@brief 		: �ش� �ִϸ��̼� �ð��� ��� ĭ�� ä��
*/
bool CX2GUAra::UpForcePowerAtThisAnimationTime( float fAnimationTimeNeededToChargeForcePower_, int iUpCount_ )
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( fAnimationTimeNeededToChargeForcePower_ ) )
	{
		UpForcePower( iUpCount_ );
		return true;
	}

	return false;
}

/** @function 	: UseForcePowerAtThisAnimationTime
	@brief 		: �ش� �ִϸ��̼� �ð��� ����� ���
*/
bool CX2GUAra::UseForcePowerAtThisAnimationTime( float fAnimationTimeNeededToUseForcePower_, int iUpCount_ )
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( fAnimationTimeNeededToUseForcePower_ ) )
	{
		UseForcePower( iUpCount_ );
		return true;
	}

	return false;
}

/** @function 	: UseForcePower
	@brief 		: ����� �����
*/
void CX2GUAra::UseForcePower( int iCount_ )
{
	ConsumeForcePower( iCount_ );
	m_ForcePowerSystem.SetUsedForcePowerAtThisState( m_ForcePowerSystem.GetUsedForcePowerAtThisState() + iCount_ );
}

/** @function 	: ConsumeForcePower
	@brief 		: ����� �Ҹ���
*/
void CX2GUAra::ConsumeForcePower( int iCount_ )
{
	SetForcePower( GetForcePower() - iCount_ );
}

/** @function 	: SetForcePower
	@brief 		: ����� ����
*/
void CX2GUAra::SetForcePower( int iForcePower_ )
{
	GetAraGageData()->SetForcePowerChanged( true );
	GetAraGageData()->SetNowForcePower( iForcePower_ );
}

/** @function	: GetForcePower
	@brief		: ��� Ȯ��
*/
int CX2GUAra::GetForcePower()
{
	return GetAraGageData()->GetNowForcePower();
}

__forceinline int CX2GUAra::GetMaxForcePower()
{
	return GetAraGageData()->GetMaxForcePower();
}

/** @function : DoProcessWhenKillEnemy
	@brief : ���� �׿��� �� ó���ؾ��ϴ� �κ� �����ϴ� �Լ�
	@param : ���� ������ ������
*/
void CX2GUAra::DoProcessWhenKillEnemy( const CX2GameUnit* /*pDefender_*/ )
{
	if ( NULL != g_pX2Game && IsFullHyperMode() )	/// 3���� �϶� ���� �׿�����
	{
		SetNowHp( GetNowHp() + GetMaxHp() * 0.03f );	/// HP ä���z
		SetForcePower( GetForcePower() + 1 );			/// ��� ä��

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_KILL_ENEMY_WHEN_FULL_HYPER_MODE", this );

	}
}

/** @function 	: ResetForcePowerWhenRebirthInPVPGame
	@brief 		:
*/
void CX2GUAra::ResetForcePowerWhenRebirthInPVPGame( int iNumOfMyTeamKill_, int iNumOfEnmyTeamKill_ )
{
	// �������� �׾��ٰ� ��Ȱ�ϴ� ���
	// ���ų� ���� �ְ�
	if ( iNumOfMyTeamKill_ <= iNumOfEnmyTeamKill_ )
	{
		if ( GetForcePower() < GetBasicForcePower() )
			SetForcePower( GetBasicForcePower() );
	}
	// �Ʊ��� �̱�������� ������
	else
	{
		SetForcePower( GetBasicForcePower() );
	}

	// �̱�� �ִµ� �⺻ �������� ������ �׳� ����
}
//#pragma endregion ��� ���� �Լ�

//#pragma region ShouldResetNotBusyTimer
/** @function 	: ShouldResetNotBusyTimer
	@brief 		: ����ȭ ����� �� �ٻ� ���¾ƴϵ��� ����
*/
bool CX2GUAra::ShouldResetNotBusyTimer()
{
	if ( false == CX2GUUser::ShouldResetNotBusyTimer() )
		return false;
	else if ( ASI_HYPER_WAIT == m_FrameDataFuture.syncData.nowState )
		return false;
	else
		return true;	// ���� �ؾ� ��
}
//#pragma endregion �ٻ� ���� Ȯ��

/** @function : ChangeAnimTail
	@brief : ����ȣ ���� ����� �ִϸ��̼� ����
*/
void CX2GUAra::ChangeAnimTail()
{
	wstring wstrAnimNameTail;
	LUA_GET_VALUE( m_LuaManager, "ANIM_NAME_TAIL", wstrAnimNameTail, L"Wait" );

	bool bTransitionTail;
	LUA_GET_VALUE( m_LuaManager, "TRANSITION_TAIL", bTransitionTail, false );

	if ( NULL != m_ptrPromotionEquipTail )
		m_ptrPromotionEquipTail->ChangeAnim( wstrAnimNameTail.c_str(), bTransitionTail );
}

//#pragma region state

//#pragma region DieFront
/** @function	: DieFrontStartFuture
	@brief		: �տ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUAra::DieFrontStartFuture()
{
	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		m_ptrRidingPet->StateChangeFuture( m_ptrRidingPet->m_DyingState );

	CommonStateStartFuture();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnimFuture->SetPlaySpeed( PLAY_SPEED );
}

/** @function	: DieFrontStart
	@brief		: �տ��� ���ݹ޾� �״� ������ Start �Լ�
*/
void CX2GUAra::DieFrontStart()
{
	CommonStateStart();
	DyingStart();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnim->SetPlaySpeed( PLAY_SPEED );
}

/** @function	: DieFrontFrameMove
	@brief		: �տ��� ���ݹ޾� �״� ������ FrameMove �Լ�
*/
void CX2GUAra::DieFrontFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

/** @function	: DieFrontEventProcess
	@brief		: �տ��� ���ݹ޾� �״� ������ EventProcess �Լ�
*/
void CX2GUAra::DieFrontEventProcess()
{
	if ( false == GetShowObject() && true == m_pXSkinAnim->IsAnimationEnd() )
		StateChange( GUSI_DIE );
}
//#pragma endregion

//#pragma region DieBack
/** @function	: DieBackStartFuture
	@brief		: �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUAra::DieBackStartFuture()
{
	CommonStateStartFuture();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnimFuture->SetPlaySpeed( PLAY_SPEED );
}

/** @function	: DieBackStart
	@brief		: �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUAra::DieBackStart()
{
	CommonStateStart();
	DyingStart();

	const float PLAY_SPEED = 0.5f;
	m_pXSkinAnim->SetPlaySpeed( PLAY_SPEED );
}

/** @function	: DieBackFrameMove
	@brief		: �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUAra::DieBackFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

/** @function : DieBackEventProcess
	@brief : �ڿ��� ���ݹ޾� �״� ������ StartFuture �Լ�
*/
void CX2GUAra::DieBackEventProcess()
{
	if ( false == GetShowObject() && true == m_pXSkinAnim->IsAnimationEnd() )
		StateChange( GUSI_DIE );
}
//#pragma endregion

//#pragma region Wait
/** @function	: WaitStartFuture
	@brief		: Wait ������ StartFuture() �Լ�
*/
void CX2GUAra::WaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );
}

/** @function	: Wait ������ StartFuture() �Լ�
	@brief		: HP�� ���� Wait or Fatal ����� �÷�����
*/
void CX2GUAra::WaitStart()
{
	CommonStateStart();
	PlayAnimationWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );

	m_usArialCount = 0;
	m_usDoubleJumpCount = 0;
}

/** @function	: WaitEventProcess
	@brief		: HP�� ���� Wait or Fatal ����� �÷�����
*/
void CX2GUAra::WaitEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	} // if
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_COMBO_X );
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	} // else if
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
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
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && m_InputData.oneLCtr == true
			 && m_FrameDataFuture.syncData.m_HyperModeCount > 0
			 && ( GetRemainHyperModeTime() <= 0.0f || true == GetCanChangeHyperModeInHyperModeState() ) )
	{
		if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
			StateChange( ASI_BERSERK_MODE );
		else
			StateChange( USI_HYPER_MODE );
	}
	//ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )	// ����
	else if( true == m_InputData.oneDown &&
			 true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
																   &m_FrameDataFuture.unitCondition.landPosition,
																   &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == IsFullHyperMode() && m_FrameDataNow.unitCondition.fStateTime >= 0.1f )
	{
		StateChange( ASI_HYPER_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}
	CommonEventProcess();
}
//#pragma endregion

//#pragma region Hyper Wait
/** @function 	: HyperWaitStartFuture
	@brief 		:
*/
void CX2GUAra::HyperWaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationHyperWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );
}

/** @function 	: HyperWaitStart
	@brief 		:
*/
void CX2GUAra::HyperWaitStart()
{
	CommonStateStart();
	PlayAnimationHyperWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );

	m_usArialCount = 0;
	m_usDoubleJumpCount = 0;

	m_CommonState.m_Wait = ASI_HYPER_WAIT;
}

/** @function 	: HyperWaitEventProcess
	@brief 		:
*/
void CX2GUAra::HyperWaitEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	} // if
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_COMBO_X );
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	} // else if
	else if ( true == m_InputData.pureDoubleRight || true == m_InputData.pureDoubleLeft )
	{
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
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && m_InputData.oneLCtr == true
		&& m_FrameDataFuture.syncData.m_HyperModeCount > 0
		&& ( GetRemainHyperModeTime() <= 0.0f || true == GetCanChangeHyperModeInHyperModeState() ) )
	{
		if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
			StateChange( ASI_BERSERK_MODE );
		else
			StateChange( USI_HYPER_MODE );
	}
	//ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )	// ����
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( false == IsFullHyperMode() )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function 	: HyperWaitStateEnd
	@brief 		:
*/
void CX2GUAra::HyperWaitStateEnd()
{
	m_CommonState.m_Wait = USI_WAIT;

	CommonStateEnd();
}

/** @function 	: PlayAnimationHyperWaitStart
	@brief 		:
*/
void CX2GUAra::PlayAnimationHyperWaitStart( CKTDGXSkinAnim* pXSkinAnim_, const FrameData& refFrameData_ )
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
			if ( true == IsFullHyperMode() )
				pXSkinAnim_->ChangeAnim( L"BoostPowerWait", false );
			else
				pXSkinAnim_->ChangeAnim( L"Wait", false );
		}
		else
		{
			if ( true == IsFullHyperMode() )
				pXSkinAnim_->ChangeAnim( L"BoostPowerWait", true );
			else
				pXSkinAnim_->ChangeAnim( L"Wait", true );
		}
	}

	pXSkinAnim_->Play( playType );
}
//#pragma endregion

//#pragma region Walk
/** @function	: WalkEventProcess
	@brief		: Walk ������ EventProcess
*/
void CX2GUAra::WalkEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_COMBO_X );
	}
	else if ( false == m_InputData.pureRight && false == m_InputData.pureLeft )
	{
		StateChange( USI_WAIT );
	}
	else if ( true == m_InputData.pureUp )
	{
		StateChange( USI_JUMP_UP );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && m_InputData.oneLCtr == true
		&& m_FrameDataFuture.syncData.m_HyperModeCount > 0
		&& ( GetRemainHyperModeTime() <= 0.0f || true == GetCanChangeHyperModeInHyperModeState() ) )
	{
		if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
			StateChange( ASI_BERSERK_MODE );
		else
			StateChange( USI_HYPER_MODE );
	}
	//ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	else if( true == m_InputData.oneDown &&
			 true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
																   &m_FrameDataFuture.unitCondition.landPosition,
																   &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region Jump
/** @function	: JumpReadyEventProcess
	@brief		: �����ϱ� �ٷ� ������ ���� EventProcess
*/
void CX2GUAra::JumpReadyEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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

/** @function	: JumpUpEventProcess
	@brief		: JumpUp ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::JumpUpEventProcess()
{
	if( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
	{
		StateChangeAirialDashIfPossible();
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_JUMP_ATTACK_X );
	}
	else if( 0.0f >= m_PhysicParam.nowSpeed.y )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	if ( true == m_InputData.oneUp )
	{
		StateChange( ASI_DOUBLE_JUMP_UP );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}

/** @function	: JumpDownEventProcess
	@brief		: �ϰ��ϴ� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::JumpDownEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		if( true == m_bDownForce )
		{
			StateChange( ASI_DASH_JUMP_POWER_LANDING );
			m_bDownForce = false;
			m_pComboGuide->ShowComboCommand(L"J", L"D");
		}
		else
		{
			StateChange( USI_JUMP_LANDING );
			m_bDownForce = false;
		}
	}
	else if( true == m_FrameDataFuture.unitCondition.bFootOnWall && ( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( ASI_WALL_LANDING );
	}
	else if ( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
	{
		StateChangeAirialDashIfPossible();
	}
	else if ( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_JUMP_ATTACK_Z );
		m_pComboGuide->ShowComboCommand( L"A", L"AXZ" );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_JUMP_ATTACK_X );
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

	if ( true == m_InputData.oneUp )
	{
		StateChange( ASI_DOUBLE_JUMP_UP );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}

/** @function 	: JumpLandingStart
	@brief 		: AirialDash Count �ʱ�ȭ
*/
void CX2GUAra::JumpLandingStart()
{
	CommonStateStart();
	CreateStepDust();

	m_usArialCount = 0;
	m_usDoubleJumpCount = 0;
}

/** @function	: JumpLandingEventProcess
	@brief		: ���� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::JumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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
		StateChange( ASI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_COMBO_X );
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
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && m_InputData.oneLCtr == true
		&& m_FrameDataFuture.syncData.m_HyperModeCount > 0
		&& ( GetRemainHyperModeTime() <= 0.0f || true == GetCanChangeHyperModeInHyperModeState() ) )
	{
		if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
			StateChange( ASI_BERSERK_MODE );
		else
			StateChange( USI_HYPER_MODE );
	}
	//ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	else if( true == m_InputData.oneDown &&
			 true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
																   &m_FrameDataFuture.unitCondition.landPosition,
																   &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXCAS
	{
		StateChange( USI_WAIT, false );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region Dash
/** @function 	: DashStart
	@brief 		: �ɰ��㵵 �ʱ�ȭ
*/
void CX2GUAra::DashStart()
{
	CommonStateStart();
	CreateStepDust();
	m_usArialCount = 0;
	m_usDoubleJumpCount = 0;
}

/** @function	: DashEventProcess
	@brief		: Dash ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::DashEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChangeAirialDashIfPossible();
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ASI_DASH_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ASI_DASH_COMBO_X );
	}
	else if ( true == SpecialAttackEventProcess() )
	{
	}
	else if( true == m_InputData.oneUp )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && m_InputData.oneLCtr == true
		&& m_FrameDataFuture.syncData.m_HyperModeCount > 0
		&& ( GetRemainHyperModeTime() <= 0.0f || true == GetCanChangeHyperModeInHyperModeState() ) )
	{
		if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
			StateChange( ASI_BERSERK_MODE );
		else
			StateChange( USI_HYPER_MODE );
	}
	//ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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
	else if( true == m_InputData.oneDown &&
			 true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
																   &m_FrameDataFuture.unitCondition.landPosition,
																   &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == GetRidingOn() )
	{
		StateChange( USI_RIDING_ON );
	}

	CommonEventProcess();
}

/** @function	: DashEndStartFuture
	@brief		: Dash ����� ���ǵ� 0�� �ʱ�ȭ
*/
void CX2GUAra::DashEndStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x = GetDashSpeed();
	m_bApplyMotionOffset = false;
}

/** @function	: DashEndFrameMove
	@brief		: Dash ���� �� ���� ���
*/
void CX2GUAra::DashEndFrameMove()
{
	if( true == m_pXSkinAnim->EventTimer( 0.4f ) )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}

/** @function	: DashEndEventProcess
	@brief		: Dash �� ����Ǵ� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::DashEndEventProcess()
{
	// DashEnd���� ���� Ű�� ������ �� ��� ��������, �Ϲ� ���������� ���ϱ� ���� const ��.
	const float TRIGGER_TIME_FACTOR_DASH_ATTACK = 0.3f;

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_InputData.oneZ )
	{
		if ( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() * TRIGGER_TIME_FACTOR_DASH_ATTACK )
		{
			StateChange( ASI_COMBO_Z );
		}
		else
		{
			StateChange( ASI_DASH_COMBO_Z );
		}
	}
	else if ( true == m_InputData.oneX )
	{
		if ( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() * TRIGGER_TIME_FACTOR_DASH_ATTACK )
		{
			StateChange( ASI_COMBO_X );
		}
		else
		{
			StateChange( ASI_DASH_COMBO_X );
		}
	}
	else if( SpecialAttackEventProcess() == true)
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
	else if( true == m_InputData.oneDown &&
			 true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
																   &m_FrameDataFuture.unitCondition.landPosition,
																   &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_JUMP_READY );
	}
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && m_InputData.oneLCtr == true
		&& m_FrameDataFuture.syncData.m_HyperModeCount > 0
		&& ( GetRemainHyperModeTime() <= 0.0f || true == GetCanChangeHyperModeInHyperModeState() ) )
	{
		if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
			StateChange( ASI_BERSERK_MODE );
		else
			StateChange( USI_HYPER_MODE );
	}
	//ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
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
//#pragma endregion

//#pragma region DashJump
/** @function	: DashJumpStartFuture
	@brief		: ĳ������ �ϰ� ���ӵ��� �߷� ���ӵ���, DownForce �÷���(���� ���¿��� �Ʒ����� Ű���带 �����°�) �ʱ�ȭ
*/
void CX2GUAra::DashJumpStartFuture()
{
	CommonStateStartFuture();

	const float MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL = 1.2f;

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * MAGIC_NUMBER_MULTIPLY_GRAVITY_ACCEL;
	m_bDownForce					= false;
}

/** @function	: DashJumpStart
	@brief		: ��� ���� ���� �� ���θʿ� ��� �ִ� ���¶�� ���� ��ƼŬ ���
*/
void CX2GUAra::DashJumpStart()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
	{
		CreateStepDust();
	}
}

/** @function	: DashJumpFrameMoveFuture
	@brief		: ���� �޸��� �ִ� �������� ��� ���� ���� �ÿ� ������ ���� �ָ� ������ ��
*/
void CX2GUAra::DashJumpFrameMoveFuture()
{
	const float MAGIC_NUMBER_MULTIPLY_DASH_SPEED = 1.6f;

	if( true == m_InputData.pureRight && true == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}
	else if( true == m_InputData.pureLeft && false == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * MAGIC_NUMBER_MULTIPLY_DASH_SPEED;
	}

	CommonFrameMoveFuture();
}

/** @function	: DashJumpEventProcess
	@brief		: Dash Jump ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::DashJumpEventProcess()
{
	bool &bCanDashJump = m_FrameDataFuture.stateParam.bEventFlagList[0];

	if ( false == IsOnSomethingFuture() )
	{
		if( true == m_InputData.pureDoubleLeft || true == m_InputData.pureDoubleRight )
		{
			StateChangeAirialDashIfPossible();
		}
	}
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
			StateChange( ASI_DASH_JUMP_POWER_LANDING );
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
		StateChange( ASI_WALL_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_DASH_JUMP_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_DASH_JUMP_COMBO_X );
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

	if ( true == m_InputData.pureLeft )
	{
		if ( true == m_InputData.oneUp )
			if ( false == m_FrameDataFuture.syncData.bIsRight )
			{
				StateChange( ASI_DOUBLE_JUMP_FRONT );
			}
	}
	else if ( true == m_InputData.pureRight )
	{
		if ( true == m_InputData.oneUp )
			if ( true == m_FrameDataFuture.syncData.bIsRight )
			{
				StateChange( ASI_DOUBLE_JUMP_FRONT );
			}
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

/** @function	: DashJumpEndFuture
	@brief		: �ϰ� ���ӵ� �ʱ�ȭ
*/
void CX2GUAra::DashJumpEndFuture()
{
	CommonStateEndFuture();
	m_PhysicParam.fDownAccel	= 0.0f;
}

/** @function	: DashJumpLandingEventProcess
	@brief		: Dash Jump �� ���� ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::DashJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_COMBO_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_COMBO_X );
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
	else if( true == m_InputData.oneDown &&
		true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
		&m_FrameDataFuture.unitCondition.landPosition,
		&m_FrameDataFuture.syncData.lastTouchLineIndex ) )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region PowerLanding
/** @function	: PowerLandingStartFuture
	@brief		: Dash Jump Power Landing ���۽� �� ����
*/
void CX2GUAra::DashJumpPowerLandingStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.nowSpeed.x *= 0.5f;
}

/** @function	: DashJumpPowerLandingStart
	@brief		: ������ �� ���� ����Ʈ ���
*/
void CX2GUAra::DashJumpPowerLandingStart()
{
	CommonStateStart();
	CreateStepDust();
	m_usArialCount = 0;
	m_usDoubleJumpCount = 0;
}

/** @function	: DashJumpPowerLandingEventProcess
	@brief		: Dash Jump Power Landing ���¿����� EventProcess( StateChange�� �Ǵ� )
*/
void CX2GUAra::DashJumpPowerLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( true == m_InputData.oneRight || true == m_InputData.oneLeft )
	{
		StateChange( USI_WALK );
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 6.0f )
	{
		if( m_InputData.oneZ == true )
		{
			StateChange( ASI_COMBO_Z );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( ASI_COMBO_X );
		}
		if( true == SpecialAttackEventProcess() )
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
		else if( true == m_InputData.pureUp )
		{
			StateChange( USI_JUMP_READY );
		}
		else if( true == m_InputData.oneDown &&
				 true == g_pX2Game->GetWorld()->GetLineMap()->CanDown( m_FrameDataFuture.syncData.position, LINE_RADIUS,
																	   &m_FrameDataFuture.unitCondition.landPosition,
																	   &m_FrameDataFuture.syncData.lastTouchLineIndex ) )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region HyperMode
/** @function	: HyperModeFrameMove
	@brief		: ���� ���¿��� ǥ���Ǿ��ϴ� ��ƼŬ ��� �� ���� ���� ī��Ʈ ����
*/
void CX2GUAra::HyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.693f, 0.759f, 1.287f );
}
//#pragma endregion

//#pragma region DamageSmall
/** @function : DamageSmallEventProcess
	@brief : �� �Ǵ� �ڿ��� small Ÿ���� �޾��� ���� EventProcess
*/
void CX2GUAra::DamageSmallEventProcess()
{
	float fGapFromUnitToLineMap = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
	const float ACCEPTABLE_GAP = 200.0f;

	if( ACCEPTABLE_GAP < fGapFromUnitToLineMap )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
//#pragma endregion

//#pragma region DamageBig
/** @function	: DamageBigStart
	@brief		: �� �Ǵ� �ڿ��� Big Ÿ���� �޾��� �� ���� ��ƼŬ ���
*/
void CX2GUAra::DamageBigStart()
{
	CommonStateStart();
	CreateStepDust();
}

/** @function	: DamageBigEventProcess
	@brief		: �� �Ǵ� �ڿ��� Big Ÿ���� �޾��� ���� EventProcess
*/
void CX2GUAra::DamageBigEventProcess()
{
	float fGapFromUnitToLineMap = m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y;
	const float ACCEPTABLE_GAP = 200.0f;	// �̰ͺ��� �Ÿ��� ũ�� ���߿� �ִ� ������

	if( ACCEPTABLE_GAP < fGapFromUnitToLineMap )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_DAMAGE_AIR_SMALL );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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
//#pragma endregion

//#pragma region DamageDown
/** @function : DamageDownFrontFrameMove
	@brief : �տ��� ���� �޾� �Ѿ� �� ������ FrameMove
*/
void CX2GUAra::DamageDownFrontFrameMove()
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
void CX2GUAra::DamageDownFrontEventProcess()
{
	bool& bChangeStandUpAttackFront			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingFrontFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingFrontBack	= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpBigAttackFront		= m_FrameDataFuture.stateParam.bEventFlagList[4];
	//����� ��� ����ٸ� ������Ʈ���� ���� �� �ð����� ����
	float fInputStartTime = ( true == m_bAbleQuickStand ) ? m_fEventTime[0].keyInputStart : 0.66f;
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS ;
	}
	// �Ѿ��� ��
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( true == bChangeStandUpAttackFront )
		{
			StateChange( ASI_STANDUP_ATTACK_FRONT );
		}
		else if ( true == bChangeStandUpBigAttackFront )
		{
			//����� ��� Ȯ���� ���� �����߸��� �ʴ� �������� ��ȯ
			if( GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent )
			{
				StateChange( ASI_STANDUP_ATTACK_FRONT_NO_DOWN );
			}
			else
			{
				StateChange( ASI_STANDUP_ATTACK_FRONT );
			}
		}
		else if( true == bChangeStandUpRollingFrontFront )
			StateChange( ASI_STANDUP_ROLLING_FRONT_FRONT );
		else if( true == bChangeStandUpRollingFrontBack )
			StateChange( ASI_STANDUP_ROLLING_FRONT_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_FRONT );
	}
	// ������ // 2013-01-10 // �Ҽ�,��õ�� ��쿡�� true == m_bAbleQuickStand�� �����ϱ� ������ ���� üũ�� ���� ����.
	ELSE_IF_STATE_CHANGE_ON_EX_( 5, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpRollingFrontFront, ASI_P_ALH_QUICK_STAND_FRONT_FRONT )
	ELSE_IF_STATE_CHANGE_ON_EX_( 6, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpRollingFrontBack, ASI_STANDUP_ROLLING_FRONT_BACK )
	ELSE_IF_STATE_CHANGE_ON_EX_( 7, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpAttackFront, ASI_STANDUP_ATTACK_FRONT )
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

/** @function : DamageDownBackFrameMove
	@brief : �ڿ��� ���� �޾� �Ѿ� �� ������ FrameMove
*/
void CX2GUAra::DamageDownBackFrameMove()
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
void CX2GUAra::DamageDownBackEventProcess()
{
	bool& bChangeStandUpAttackBack			= m_FrameDataFuture.stateParam.bEventFlagList[1];
	bool& bChangeStandUpRollingBackFront	= m_FrameDataFuture.stateParam.bEventFlagList[2];
	bool& bChangeStandUpRollingBackBack		= m_FrameDataFuture.stateParam.bEventFlagList[3];
	bool& bChangeStandUpBigAttackBack		= m_FrameDataFuture.stateParam.bEventFlagList[4];
	//����� ��� ����ٸ� ������Ʈ���� ���� �� �ð����� ����
	float fInputStartTime = ( true == m_bAbleQuickStand ) ? m_fEventTime[0].keyInputStart : 0.66f;
	// �Ѿ����µ� �ؿ� ���θ� ���� ���� ���
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS ;
	}
	// �Ѿ��� ��
	else if( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		// Z Ű�� ������ ��
		if( true == bChangeStandUpAttackBack )
		{
			StateChange( ASI_STANDUP_ATTACK_BACK );
		}
		// X Ű�� ������ ��
		else if ( true == bChangeStandUpBigAttackBack )
		{
			//����� ��� Ȯ���� ���� �����߸��� �ʴ� �������� ��ȯ
			if( GetRandomInt() < m_SkillRelatedData.m_iNoDownDamageAtStandUpAttackPercent )
			{
				StateChange( ASI_STANDUP_ATTACK_BACK_NO_DOWN );
			}
			else
			{
				StateChange( ASI_STANDUP_ATTACK_BACK );
			}
		}
		else if( true == bChangeStandUpRollingBackFront )
			StateChange( ASI_STANDUP_ROLLING_BACK_FRONT );
		else if( true == bChangeStandUpRollingBackBack )
			StateChange( ASI_STANDUP_ROLLING_BACK_BACK );
		else
			StateChange( USI_DAMAGE_STANDUP_BACK );
	}
	// ������ // 2013-01-10 // �Ҽ�,��õ�� ��쿡�� true == m_bAbleQuickStand�� �����ϱ� ������ ���� üũ�� ���� ����.
	ELSE_IF_STATE_CHANGE_ON_EX_( 5, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpRollingBackFront, ASI_P_ALH_QUICK_STAND_BACK_FRONT )
	ELSE_IF_STATE_CHANGE_ON_EX_( 6, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpRollingBackBack, ASI_P_ALH_QUICK_STAND_BACK_BACK )
	ELSE_IF_STATE_CHANGE_ON_EX_( 7, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_bAbleQuickStand && true == bChangeStandUpAttackBack, ASI_STANDUP_ATTACK_BACK )
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

/** @function	: DamageAirDownLandingEventProcess
	@brief		: ���߿��� �ٿ� ���� �޾� ���� ���� ������ EventProcess
*/
void CX2GUAra::DamageAirDownLandingEventProcess()
{
	DamageDownFrontEventProcess();
}
//#pragma endregion

//#pragma region StandUp
/** @function	: StandUpRollingEventProcess
	@brief		: �����鼭 �Ͼ�� ������ EventProcess
*/
void CX2GUAra::StandUpRollingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );

		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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

/** @function	: StandUpRollingEnd
	@brief		: �����鼭 �Ͼ�� ������ End
*/
void CX2GUAra::StandUpRollingEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );

	/// �޸� : �� �帣���� (��ȭ����) - ȸ���� 50% ����
	if( ASI_A_ALH_BREAKING_AVOIDANCE == GetNowStateID() &&
		true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO7 ) )	/// ��ȭ ���� ȸ�����̰�, �޸� �������� ��
	{
		SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_BREAKING_MEMO ) );
	}
}

/** @function	: StandUpAttackEventProcess
	@brief		: �Ͼ�鼭 ������ ������ EventProcess
*/
void CX2GUAra::StandUpAttackEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );

			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
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

/** @function	: StandUpAttackEnd
	@brief		: �Ͼ�鼭 ������ ������ End
*/
void CX2GUAra::StandUpAttackEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}
//#pragma endregion

//#pragma region AirialDash
/** @function 	: AirialDashStartFuture
	@brief 		: �ɰ��㵵 �߷� ����
*/
void CX2GUAra::AirialDashStartFuture()
{
	CommonStateStartFuture();

	m_bDownForce = false;
	m_bDisableGravity = true;
	// �нú꿡 ���ؼ� �ð��� �ٲ� �� ����
	m_fAirialDashTime = 0.f;
	++m_usArialCount;
}

/** @function 	: AirialDashStart
	@brief 		:
*/
void CX2GUAra::AirialDashStart()
{
	CommonStateStart();

	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
		CreateStepDust();
}

/** @function 	: AirialDashFrameMoveFuture
	@brief 		: �ð� ó��
*/
void CX2GUAra::AirialDashFrameMoveFuture()
{
	m_fAirialDashTime += m_fElapsedTime;

	if( false == m_InputData.pureDown && true == m_InputData.pureRight && true == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x;
		m_PhysicParam.nowSpeed.y = max( 200.f - ( m_fAirialDashTime * 500), -1200.f );
	}
	else if( false == m_InputData.pureDown && true == m_InputData.pureLeft && false == m_FrameDataNow.syncData.bIsRight )
	{
		m_PhysicParam.nowSpeed.x = m_PhysicParam.passiveSpeed.x;
		m_PhysicParam.nowSpeed.y = max( 200.f - ( m_fAirialDashTime * 500), -1200.f );
	}
	else
	{
		StateChange( USI_DASH_JUMP );
	}

	CommonFrameMoveFuture();
}

/** @function 	: AirialDashFrameMove
	@brief 		:
*/
void CX2GUAra::AirialDashFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Airial_Dash", this );
	}

	CommonFrameMove();
}

/** @function 	: AirialDashEventProcess
	@brief 		: �ɰ��㵵���� �̾����� �̺�Ʈ ó��
*/
void CX2GUAra::AirialDashEventProcess()
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
			StateChange( USI_DASH );
			m_bDownForce = false;
		}

	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( ASI_DASH_JUMP_COMBO_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( ASI_DASH_JUMP_COMBO_X );
	}

	if ( true == m_InputData.pureLeft )
	{
		if ( true == m_InputData.oneUp )
			if ( false == m_FrameDataFuture.syncData.bIsRight )
			{
				StateChange( ASI_DOUBLE_JUMP_FRONT );
			}
	}
	else if ( true == m_InputData.pureRight )
	{
		if ( true == m_InputData.oneUp )
			if ( true == m_FrameDataFuture.syncData.bIsRight )
			{
				StateChange( ASI_DOUBLE_JUMP_FRONT );
			}
	}

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Airial_Dash", this );

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

/** @function 	: AirialDashEndFuture
	@brief 		:
*/
void CX2GUAra::AirialDashEndFuture()
{
	CommonStateEndFuture();

	//if( m_PhysicParam.nowSpeed.x > GetDashSpeed() )
		//m_PhysicParam.nowSpeed.x = GetDashSpeed();

	m_bDisableGravity = false;
}
//#pragma endregion

//#pragma region Double Jump
/** @function 	: DoubleJumpStartFuture
	@brief 		:
*/
void CX2GUAra::DoubleJumpStartFuture()
{
	CommonStateStartFuture();

	m_bDownForce = false;
	++m_usDoubleJumpCount;
}

/** @function 	: DoubleJumpFrontEventProcess
	@brief 		:
*/
void CX2GUAra::DoubleJumpFrontEventProcess()
{
	bool &bCanDashJump = m_FrameDataFuture.stateParam.bEventFlagList[0];

	if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		if( CX2Unit::UC_ARA_SAKRA_DEVANAM == GetUnitClass() )
		{
			if( (true == GetIsRight() && true == m_InputData.pureLeft) || // �ڷ� ���� ����Ű�� �Է� �ߴٸ�
				( false == GetIsRight() && true == m_InputData.pureRight ) )
			{
				StateChange( ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK );
			}
			else
			{
				StateChange( ASI_DASH_JUMP_COMBO_Z );
			}
		}
		else
		{
			StateChange( ASI_DASH_JUMP_COMBO_Z );
		}
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_DASH_JUMP_COMBO_X );
	}
	else if ( true == IsOnSomethingFuture() )
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
			StateChange( ASI_DASH_JUMP_POWER_LANDING );
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
}

/** @function 	: DoubleJumpUpEventProcess
	@brief 		:
*/
void CX2GUAra::DoubleJumpUpEventProcess()
{
	if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_JUMP_ATTACK_X );
	}
	else if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}

	CommonEventProcess();
}

/** @function 	: DoubleJumpBackEventProcess
	@brief 		:
*/
void CX2GUAra::DoubleJumpBackEventProcess()
{
	if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if ( true == m_InputData.oneZ )
	{
		StateChange( ASI_JUMP_ATTACK_Z );
	}
	else if ( true == m_InputData.oneX )
	{
		StateChange( ASI_JUMP_ATTACK_X );
	}
	else if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}

	if( true == m_InputData.oneDown )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed * 2;
		m_bDownForce = true;
	}
}
//#pragma endregion

//#pragma region ComboZ
/** @function	: ComboZEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboZZ )
*/
void CX2GUAra::ComboZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
								 m_InputData.oneZ == true, ASI_COMBO_ZZ )

	CommonEventProcess();
}
//#pragma endregion 81

//#pragma region ComboZZ
/** @function	: ComboZZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboZZZ, ComboZZX )
*/
void CX2GUAra::ComboZZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneZ == true, ASI_COMBO_ZZZ )
		ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
		m_InputData.oneX == true, ASI_COMBO_ZZX )

		CommonEventProcess();
}
//#pragma endregion 82

//#pragma region ComboZZZ
/** @function 	: ComboZZZFrameMove
	@brief 		: ������ ������ ����
*/
void CX2GUAra::ComboZZZFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
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

/** @function	: ComboZZZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboZZZZ )
*/
void CX2GUAra::ComboZZZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
		m_InputData.oneZ == true && ( CX2Unit::UC_ARA_LITTLE_DEVIL == GetUnitClass() || CX2Unit::UC_ARA_YAMA_RAJA == GetUnitClass() ) &&
		( ( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ||
		( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) )
		, ASI_COMBO_ALD_ZZZFrontZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneZ == true, ASI_COMBO_ZZZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
		m_InputData.oneX == true, ASI_COMBO_ZZZXStart )

	BWALK_CANCEL_AFTER( m_fBWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 83

//#pragma region ComboZZZZ
/** @function	: ComboZZZZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboZZZZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 84

//#pragma region ComboZZZX
/** @function 	: ComboZZZXStartEventProcess
	@brief 		:
*/
void CX2GUAra::ComboZZZXStartEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ASI_COMBO_ZZZXCharge );
	}
}

/** @function 	: ComboZZZXChargeStateStart
	@brief 		:
*/
void CX2GUAra::ComboZZZXChargeStateStart()
{
	CommonStateStart();

	m_fZZZXChargeTime = 0.f;
	SetNoDetonation( true );
}

/** @function 	: ComboZZZXFrameMove
	@brief 		: �ش� �����ӿ� ����� ���� ó��
*/
void CX2GUAra::ComboZZZXChargeFrameMove()
{
	CommonFrameMove();

	if ( 2.f >= m_fZZZXChargeTime )
		m_fZZZXChargeTime += m_fElapsedTime;

	if ( 2.f <= m_fZZZXChargeTime )
		m_fZZZXChargeTime = 2.f;

if( true == m_pXSkinAnim->EventTimerOneshot( 0.495f ) )
	{
		CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ZpushX_Charge", this, NULL,
			false, GetPowerRate() );
		if ( INVALID_EFFECTSET_HANDLE != hEffectSet )
			m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSet );
	}
}

/** @function	: ComboZZZXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboZZZXChargeEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( false == m_InputData.pureX || true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ASI_COMBO_ZZZXAttack );
	}

	CommonEventProcess();
}

/** @function 	: ComboZZZXChargeStateEnd
	@brief 		:
*/
void CX2GUAra::ComboZZZXChargeStateEnd()
{
	CommonStateEnd();

	SetNoDetonation( false );
}

/** @function 	: ComboZZZXAttackFrameMove
	@brief 		:
*/
void CX2GUAra::ComboZZZXAttackFrameMove()
{
if( m_pXSkinAnim->EventTimerOneshot( 0.331f ) == true )
	{
		m_DamageData.damage.fPhysic = m_DamageData.damage.fPhysic * ( 1.f + min( m_fZZZXChargeTime, 2.f ) );

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ZpushX_Attack", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();

	if ( 2.f <= m_fZZZXChargeTime )
	{
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
		m_FrameDataNow.stateParam.bSuperArmor = true;
	}
}

/** @function	: ComboZZZXAttackEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboZZZXAttackEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

/** @function 	: ComboZZZXAttackStateEnd
	@brief 		:
*/
void CX2GUAra::ComboZZZXAttackStateEnd()
{
	CommonStateEnd();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
	m_FrameDataNow.stateParam.bSuperArmor = false;
}
//#pragma endregion

//#pragma region ComboZZX
/** @function	: ComboZZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboZZXEventProcess()
{
	if ( 0.462f < m_pXSkinAnim->GetNowAnimationTime() && true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		true == m_InputData.oneX &&
		( CX2Unit::UC_ARA_LITTLE_HSIEN == GetUnitClass() || CX2Unit::UC_ARA_SAKRA_DEVANAM == GetUnitClass() ),
		ASI_COMBO_ALH_ZZXX )

	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
		true == m_InputData.oneX &&
		( CX2Unit::UC_ARA_LITTLE_DEVIL == GetUnitClass() || CX2Unit::UC_ARA_YAMA_RAJA == GetUnitClass() ),
		ASI_COMBO_ALD_ZZXX )

	CommonEventProcess();
}
//#pragma endregion 86

//#pragma region ComboX
/** @function	: ComboXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboXX )
*/
void CX2GUAra::ComboXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneX == true, ASI_COMBO_XX )

	CommonEventProcess();
}
//#pragma endregion 87

//#pragma region ComboXX
/** @function	: ComboXXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboXXX, ComboXXdownX )
*/
void CX2GUAra::ComboXXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_FrameDataFuture.stateParam.bEventFlagList[0]
			  && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		StateChange( ASI_COMBO_XXX );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if ( true == m_FrameDataFuture.stateParam.bEventFlagList[1]
			  && m_pXSkinAnimFuture->GetNowAnimationTime() > m_fEventTime[0].stateChange )
	{
		StateChange( ASI_COMBO_XXdownX );
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if ( m_pXSkinAnimFuture->GetNowAnimationTime() < m_fEventTime[0].keyInputEnd
			  && m_pXSkinAnimFuture->GetNowAnimationTime() >= m_fEventTime[0].keyInputStart )
	{
		if ( true == m_InputData.oneX )
		{
			if ( true == m_InputData.pureDown )
				m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
			else
				m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
	}

	CommonEventProcess();
}
//#pragma endregion 88

//#pragma region ComboXXX
/** @function 	: ComboXXXStateStart
	@brief 		: XXXLoopCount �ʱ�ȭ
*/

void CX2GUAra::ComboXXXStateStart()
{
	m_uiComboXXXLoopCount = 0;
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}
/** @function	: ComboXXXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboXXXloop )
*/
void CX2GUAra::ComboXXXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneDoubleX == true, ASI_COMBO_XXXloop )

	CommonEventProcess();
}
//#pragma endregion 89

//#pragma region ComboXXXloop
/** @function 	: ComboXXXloopStateStart
	@brief 		:
*/
void CX2GUAra::ComboXXXloopStateStart()
{
	++m_uiComboXXXLoopCount;
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

/** @function 	: ComboXXXloopFrameMove
	@brief 		:
*/
void CX2GUAra::ComboXXXloopFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
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

/** @function	: ComboXXXloopEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboXXXloopX )
*/
void CX2GUAra::ComboXXXloopEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( ASI_COMBO_XXXloopX );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneX == true && m_uiComboXXXLoopCount < 2, ASI_COMBO_XXXloop )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 90

//#pragma region ComboXXXloopX
/** @function 	: ComboXXXloopXStateStart
	@brief 		: ���� �Ƹ� �ܰ��� �ѱ�
*/
void CX2GUAra::ComboXXXloopXStateStart()
{
	CommonStateStart();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
}

/** @function 	: ComboXXXloopXFrameMove
	@brief 		: DamageData ����
*/
void CX2GUAra::ComboXXXloopXFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
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

/** @function	: ComboXXXloopXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboXXXloopXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

/** @function 	: ComboXXXloopXStateEnd
	@brief 		:
*/
void CX2GUAra::ComboXXXloopXStateEnd()
{
	CommonStateEnd();
	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
}
//#pragma endregion 91

//#pragma region ComboXXdownX
/** @function 	: ComboXXdownXFrameMove
	@brief 		: DamageData ����
*/
void CX2GUAra::ComboXXdownXFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
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

/** @function	: ComboEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboXXdownXX )
*/
void CX2GUAra::ComboXXdownXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneX == true, ASI_COMBO_XXdownXX )

	CommonEventProcess();
}
//#pragma endregion 92

//#pragma region ComboXXdownXX
/** @function	: ComboEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboXXdownXXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	else if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
	{
		StateChangeSpecialAttack( m_eSkillID );
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}
//#pragma endregion 93

//#pragma region ComboDashZ
/** @function 	: ComboDashZFrameMove
	@brief 		: ������ ����
*/
void CX2GUAra::ComboDashZFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
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

/** @function	: ComboDashZEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����( ComboZpushX_CHARGE )
*/
void CX2GUAra::ComboDashZEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	/*if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}*/
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.pureX == true, ASI_DASH_COMBO_ZpushX_CHARGE )

		ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
		m_InputData.oneZ == true &&
		( CX2Unit::UC_ARA_LITTLE_HSIEN == GetUnitClass() || CX2Unit::UC_ARA_SAKRA_DEVANAM == GetUnitClass() ),
		ASI_DASH_COMBO_ALH_ZZ )

		ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
		m_InputData.oneZ == true && CX2Unit::UC_ARA_YAMA_RAJA == GetUnitClass(), ASI_DASH_COMBO_AYR_ZZ )

	CommonEventProcess();
}
//#pragma endregion 94

//#pragma region ComboDashZpushX_Charge
/** @function 	: ComboDashZpushX_ChargeStateStart
	@brief 		: ���� �ð� �ʱ�ȭ, ���� �극��ũ ����
*/
void CX2GUAra::ComboDashZpushX_ChargeStateStart()
{
	CommonStateStart();

	m_fZpushXChargeTime = 0.f;
	SetNoDetonation( true );
}

/** @function 	: ComboDashZpushX_ChargeFrameMove
	@brief 		: ���� �ð� ���� �� 2�ʿ� ���� ����
*/
void CX2GUAra::ComboDashZpushX_ChargeFrameMove()
{
	CommonFrameMove();

	if ( 2.f >= m_fZpushXChargeTime )
		m_fZpushXChargeTime += m_fElapsedTime;

	if ( 2.f <= m_fZpushXChargeTime )
		m_fZpushXChargeTime = 2.f;

if( true == m_pXSkinAnim->EventTimerOneshot( 0.495f ) )
	{
		CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ZpushX_Charge", this, NULL,
																					false, GetPowerRate() );
		if ( INVALID_EFFECTSET_HANDLE != hEffectSet )
			m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSet );
	}
}

/** @function	: ComboDashZpushX_ChargeEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����( DashComboZpushX_ATTACK )
*/
void CX2GUAra::ComboDashZpushX_ChargeEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	else if ( false == m_InputData.pureX || true == m_pXSkinAnim->IsAnimationEnd() )
	{
		StateChange( ASI_DASH_COMBO_ZpushX_ATTACK );
	}

	CommonEventProcess();
}

/** @function 	: ComboDashZpushX_ChargeStateEnd
	@brief 		: ���� �극��ũ �ѱ�
*/
void CX2GUAra::ComboDashZpushX_ChargeStateEnd()
{
	CommonStateEnd();

	SetNoDetonation( false );
}
//#pragma endregion 95

//#pragma region ComboDashZpushX_Attack
/** @function 	: ComboDashZpushX_AttackFrameMove
	@brief 		:
*/
void CX2GUAra::ComboDashZpushX_AttackFrameMove()
{
if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) == true )
	{
		m_DamageData.damage.fPhysic = m_DamageData.damage.fPhysic * ( 1.f + min( m_fZpushXChargeTime, 2.f ) );

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ZpushX_Attack", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();

	if ( 2.f <= m_fZpushXChargeTime )
	{
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
		m_FrameDataNow.stateParam.bSuperArmor = true;
	}
}

/** @function	: ComboDashZpushX_AttackEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboDashZpushX_AttackEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	// �߹ؿ� ���θ��� ������
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}

/** @function 	: ComboDashZpushX_AttackStateEnd
	@brief 		:
*/
void CX2GUAra::ComboDashZpushX_AttackStateEnd()
{
	CommonStateEnd();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
	m_FrameDataNow.stateParam.bSuperArmor = false;
}
//#pragma endregion 96

//#pragma region ComboDashX
/** @function	: ComboDashXEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ComboDashXEventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}

	else if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
	{
		StateChangeSpecialAttack( m_eSkillID );
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}
//#pragma endregion 97

//#pragma region JumpAttackZ
/** @function 	: JumpAttackZFrameMoveFuture
	@brief 		: ���� Ű�� ���ǵ� ����
*/
void CX2GUAra::JumpAttackZFrameMoveFuture()
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

/** @function	: JumpAttackZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::JumpAttackZEventProcess()
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
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneZ == true && ( CX2Unit::UC_ARA_LITTLE_HSIEN == GetUnitClass() || CX2Unit::UC_ARA_SAKRA_DEVANAM == GetUnitClass() ),
		ASI_JUMP_ATTACK_ALH_ZZ )

	CommonEventProcess();
}
//#pragma endregion 98

//#pragma region JumpAttackX
/** @function 	: JumpAttackXFrameMoveFuture
	@brief 		: ���� Ű�� ���ǵ� ����
*/
void CX2GUAra::JumpAttackXFrameMoveFuture()
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

/** @function 	: JumpAttackXFrameMove
	@brief 		: Damage Data ����
*/
void CX2GUAra::JumpAttackXFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.396f ) )
	{
		m_DamageData.reActType		= CX2DamageManager::RT_DOWN;
		m_DamageData.backSpeed.y	= -2000.f;
	}

	CommonFrameMove();
}

/** @function	: JumpAttackXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::JumpAttackXEventProcess()
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
//#pragma endregion 99

//#pragma region DashJumpComboZ
/** @function	: DashJumpComboZEventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����( DashJumpComboZZ )
*/
void CX2GUAra::DashJumpComboZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneZ == true, ASI_DASH_JUMP_COMBO_ZZ )
	ELSE_IF_STATE_CHANGE_ON_EX_( 1, m_fEventTime[1].keyInputStart, m_fEventTime[1].keyInputEnd, m_fEventTime[1].stateChange,
		m_InputData.oneX == true && CX2Unit::UC_ARA_SAKRA_DEVANAM == GetUnitClass(), ASI_DASH_JUMP_COMBO_ASD_ZX )

	ELSE_IF_STATE_CHANGE_ON_EX_( 2, m_fEventTime[2].keyInputStart, m_fEventTime[2].keyInputEnd, m_fEventTime[2].stateChange,
	m_InputData.oneX == true && ( CX2Unit::UC_ARA_LITTLE_DEVIL == GetUnitClass() || CX2Unit::UC_ARA_YAMA_RAJA == GetUnitClass() ),
	ASI_DASH_JUMP_COMBO_ALD_ZX )
	CommonEventProcess();
}
//#pragma endregion 100

//#pragma region DashJumpComboZZ
/** @function	: DashJumpComboZZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����( DashJumpComboZZZ )
*/
void CX2GUAra::DashJumpComboZZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneZ == true, ASI_DASH_JUMP_COMBO_ZZZ )

	CommonEventProcess();
}
//#pragma endregion 101

//#pragma region DashJumpComboZZZ
/** @function 	: DashJumpComboZZZFrameMove
	@brief 		: Damage Data ����
*/
void CX2GUAra::DashJumpComboZZZFrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.462f ) )
	{
		m_DamageData.reActType		= CX2DamageManager::RT_FLY;
		m_DamageData.backSpeed.y	= 1250.f;
	}

	CommonFrameMove();
}

/** @function	: DashJumpComboZZZEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::DashJumpComboZZZEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	CommonEventProcess();
}
//#pragma endregion 102

//#pragma region DashJumpComboX
/** @function	: DashJumpComboXEventProecss
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::DashJumpComboXEventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneZ == true && CX2Unit::UC_ARA_YAMA_RAJA == GetUnitClass(), ASI_DASH_JUMP_COMBO_AYR_XZ )

	CommonEventProcess();
}
//#pragma endregion 103

//#pragma region StateChangeAirialDashIfPossible
void CX2GUAra::StateChangeAirialDashIfPossible()
{
	if( true == GetEnableDash() && _CONST_ARA_::MAGIC_MAX_AIRIAL_DASH_COUNT > m_usArialCount )
		StateChange( ASI_AIRIAL_DASH );
	else
		StateChange( USI_JUMP_DOWN );
}
//#pragma endregion

//#pragma endregion functions

//#pragma region special active skill
//#pragma region WIND_WEDGE


void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_WIND_WEDGE_START", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_WIND_WEDGE_START_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_WIND_WEDGE_START_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_WIND_WEDGE_START_EventProcess );
}

/** @function 	: ASI_SA_ARA_WIND_WEDGE_START_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_START_Init()
{
	XMeshReadyInBackground( L"ARA_SA_WindWedge_spark.Y" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_spark.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_windLine.dds" );
}

/** @function 	: ASI_SA_ARA_WIND_WEDGE_START_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) && CanUseForcePower() )
	{
		/// �޸� : �帧�� Ÿ�� â�� (��ȣ 2�� : �ٶ�����) - ��� �Ҹ� 1 ����
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO2 ) )
			UseForcePower( 1 );
		else
			UseForcePower( 2 );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.495f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Wind_Wedge", this );
		m_usBeforeForceGainCount = m_usForceGainCount;
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.495f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Wind_Wedge_1st", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_WIND_WEDGE_START_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{

		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ARA_WIND_WEDGE_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_WIND_WEDGE_END", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_WIND_WEDGE_END_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_WIND_WEDGE_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_WIND_WEDGE_END_EventProcess );
}

/** @function 	: ASI_SA_ARA_WIND_WEDGE_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_END_Init()
{
	XMeshReadyInBackground( L"ARA_SA_WindWedge_spark.Y" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_spark.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_windLine.dds" );
}

/** @function 	: ASI_SA_ARA_WIND_WEDGE_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_END_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Wind_Wedge_Final", this );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.015f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Wind_Wedge_2nd", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_WIND_WEDGE_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_WIND_WEDGE_END_EventProcess()
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
//#pragma endregion �ٶ� ����

//#pragma region PULLING_PIERCE

void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_PULLING_PIERCE_START", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_PULLING_PIERCE_START_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_PULLING_PIERCE_START_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_PULLING_PIERCE_START_EventProcess );
}

/** @function 	: ASI_SA_ARA_PULLING_PIERCE_START_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_START_Init()
{
	XMeshReadyInBackground( L"ARA_SA_WindWedge_spark.Y" );
	XSkinMeshReadyInBackground( L"ARA_SA_PullingPierce_circle.X" );
	XSkinMeshReadyInBackground( L"elsword_SA_SwordBlastiong_slash.X" );
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_circle.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"ARA_SA_PullingPierce_circle.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
}

/** @function 	: ASI_SA_ARA_PULLING_PIERCE_START_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

if( true == m_pXSkinAnim->EventTimerOneshot( 0.396f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Pulling_Pierce", this );
		UpForcePower( 1 );
		m_usBeforeForceGainCount = m_usForceGainCount;
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.462f ) )
	{
		bool bIsMemoAttack = false;		/// �޸� ���� ����

		/// �޸� : ��õ���� (��� 2�� : �����α�) - ũ��Ƽ�� Ȯ�� 25% ����, ���� ���� ���� ����
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO4 ) )
			bIsMemoAttack = true;

		m_hPullinPierce = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Pulling_Pierce_1st",
							this, NULL, bIsMemoAttack, GetPowerRate() );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.759f ) )
	{
		if ( INVALID_EFFECTSET_HANDLE != m_hPullinPierce )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hPullinPierce );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_PULLING_PIERCE_START_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ARA_PULLING_PIERCE_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}
}

void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_ ;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_PULLING_PIERCE_END", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_PULLING_PIERCE_END_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_PULLING_PIERCE_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_PULLING_PIERCE_END_EventProcess );
}

/** @function 	: ASI_SA_ARA_PULLING_PIERCE_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_END_Init()
{
	XMeshReadyInBackground( L"ARA_SA_WindWedge_spark.Y" );
	XSkinMeshReadyInBackground( L"ARA_SA_PullingPierce_circle.X" );
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_circle.X" );
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"ARA_SA_PullingPierce_circle.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
}

/** @function 	: ASI_SA_ARA_PULLING_PIERCE_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_END_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Pulling_Pierce_Final", this );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.066f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Pulling_Pierce_2nd", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_PULLING_PIERCE_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_PULLING_PIERCE_END_EventProcess()
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
//#pragma endregion ���� �α�

//#pragma region FALLING_DRAGON
/** @function 	: ASI_SA_ARA_FALLING_DRAGON_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_FALLING_DRAGON_Init()
{
	XMeshReadyInBackground( L"ARA_SA_FallingDragon_sparkHigh.Y" );
	XMeshReadyInBackground( L"ARA_SA_FallingDragon_spark.Y" );
	TextureReadyInBackground( L"ARA_SA_FallingDragon_circleSpark.dds" );
	TextureReadyInBackground( L"ARA_SA_FallingDragon_donw.dds" );
	TextureReadyInBackground( L"ARA_SA_FallingDragon_spark.dds" );
	TextureReadyInBackground( L"ARA_SA_FallingDragon_sparkHigh.dds" );
}

/** @function 	: ASI_SA_ARA_FALLING_DRAGON_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_FALLING_DRAGON_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

if ( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) && CanUseForcePower() )
	{
		m_fPullingDragonPowerRate = GetPowerRate() * ( 1 + ( GetForcePower() - 5 ) * 0.2f );

		UseForcePower( GetForcePower() );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 1.320f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Falling_Dragon", this, NULL, IsHyperState(),
			m_fPullingDragonPowerRate );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_FALLING_DRAGON_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_FALLING_DRAGON_EventProcess()
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
//#pragma endregion ������_���� ������

//#pragma region SHADOW_RISING

void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_JUMP_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_RISING_JUMP", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_JUMP_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_JUMP_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_JUMP_EventProcess );
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_JUMP_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_JUMP_Init()
{
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Badak.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Badak_BALL.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack_Aura.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack02.X" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Cloud.dds" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_DarkFlame.DDS" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_blackSmoke.dds" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_blackAura.DDS" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Hall.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact01.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact02.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact04.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact05.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Smoke.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"smoke02.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_JUMP_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_JUMP_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Shadow_Rising_Jump", this );
		UpForcePower( 1 );
		m_usBeforeForceGainCount = m_usForceGainCount;
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_JUMP_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_JUMP_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true && IsOnSomethingFuture() == true )
	{
		StateChange( ASI_SA_ARA_SHADOW_RISING_LANDING );
	}

	CommonEventProcess();
}


void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_LANDING_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_RISING_LANDING", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_LANDING_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_LANDING_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_LANDING_EventProcess );
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_LANDING_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_LANDING_Init()
{
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Badak.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Badak_BALL.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack_Aura.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack02.X" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Cloud.dds" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_DarkFlame.DDS" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_blackSmoke.dds" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_blackAura.DDS" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Hall.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact01.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact02.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact04.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact05.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Smoke.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"smoke02.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_LANDING_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_LANDING_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Shadow_Rising_Landing", this );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_LANDING_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_LANDING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ARA_SHADOW_RISING_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}


void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ARA_SHADOW_RISING_END", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_END_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ARA_SHADOW_RISING_END_EventProcess );
}


/** @function 	: ASI_SA_ARA_SHADOW_RISING_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_END_Init()
{
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Badak.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Badak_BALL.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack_Aura.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_RISING_Attack02.X" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Cloud.dds" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_DarkFlame.DDS" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_blackSmoke.dds" );
	TextureReadyInBackground( L"Pa_Ran_SpaD_blackAura.DDS" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Hall.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact01.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact02.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact04.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Impact05.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Smoke.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"smoke02.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_END_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Shadow_Rising_End", this );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_SHADOW_RISING_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_RISING_END_EventProcess()
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
//#pragma endregion ���·�_õ�� �ɸ���

//#pragma region TEMPEST
/** @function 	: ASI_SA_ARA_TEMPEST_DANCE_START_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_TEMPEST_DANCE_START_Init()
{
	XMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.Y" );
	XMeshReadyInBackground( L"Lire_Leaf01.Y" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_IMPACT.X" );
	TextureReadyInBackground( L"stone.dds" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
}

/** @function 	: ASI_SA_ARA_TEMPEST_DANCE_START_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_TEMPEST_DANCE_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Tempest_Dance_Start", this );
		UseForcePower( 3 );
		m_usBeforeForceGainCount = m_usForceGainCount;
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.231f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_1st", this, NULL, false, GetPowerRate() );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.429f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_2nd", this, NULL, false, GetPowerRate() );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.561f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_2nd", this, NULL, false, GetPowerRate() );
	}

if( true == m_pXSkinAnim->EventTimerOneshot( 0.957f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_2nd", this, NULL, false, GetPowerRate() );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 1.056f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_2nd", this, NULL, false, GetPowerRate() );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 1.155f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_2nd", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_TEMPEST_DANCE_START_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_TEMPEST_DANCE_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ARA_TEMPEST_DANCE_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack()  )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function 	: ASI_SA_ARA_TEMPEST_DANCE_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_TEMPEST_DANCE_END_Init()
{
	XMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.Y" );
	XMeshReadyInBackground( L"Lire_Leaf01.Y" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_IMPACT.X" );
	TextureReadyInBackground( L"stone.dds" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"Spin_Liner02_Gray.dds" );
	TextureReadyInBackground( L"CTT_A_REMOTE_MORTAR_shellingPointLight.dds" );
	TextureReadyInBackground( L"SpinWind01.dds" );
}

/** @function 	: ASI_SA_ARA_TEMPEST_DANCE_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_TEMPEST_DANCE_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Tempest_Dance_End", this );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.099f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_Final_1st", this, NULL, false, GetPowerRate() );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.297f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tempest_Dance_Final_2nd", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_TEMPEST_DANCE_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_TEMPEST_DANCE_END_EventProcess()
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
//#pragma endregion ��ǳ_�ָ�

//#pragma region SHADOW_KNOT
/** @function 	: ASI_SA_ARA_SHADOW_KNOT_START_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_START_Init()
{
	XMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.Y" );
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_HIGH.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_KNOT.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE03.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SD_pa_source.X" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	TextureReadyInBackground( L"stone_AirShip.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_KNOT_KNOT_FINAL.dds" );
	TextureReadyInBackground( L"SD_pa_source.tga" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_TATOO.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_HIGHLIGHT.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_DIFFUSE.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_Attack.dds" );
	TextureReadyInBackground( L"RTW_Flame_Buster.dds" );
	TextureReadyInBackground( L"raven_beed_hit_spark.DDS" );
	TextureReadyInBackground( L"Ara_Black_LEAF.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_START_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_START_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;

	/// �޸� : ���� �׸��� â�� (�׸��ڸŵ�) - MP 150 + ��� 3 ���
	if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO5 ) )
		UseForcePower( 3 );
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_START_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_START_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ARA_Shadow_Knot_Start", this );

		/// �޸� : ���� �׸��� â�� (�׸��ڸŵ�) - ù ������ ���� ��� ȹ�� ����
		if( false == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO5 ) )
			UpForcePower( 1 );
	}
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.667f ) )
	{
		/* Frame ũ���� ������ �׽�Ʈ �ʿ�
		CKTDXDeviceXSkinMesh* pSkinMesh = NULL;
		CKTDXDeviceXET* pXET = NULL;

		for( int i = 0; i < (int)m_ViewEqipList.size(); i++ )
		{
			CX2EqipPtr pCX2Eqip = m_ViewEqipList[i];
			if ( pCX2Eqip == NULL )
				continue;

			if( CX2Unit::EP_WEAPON_HAND != pCX2Eqip->GetEqipPos() )
				continue;

			if( NULL != pCX2Eqip->GetItem() &&
				NULL != pCX2Eqip->GetItem()->GetItemTemplet() )
			{
				if( true == pCX2Eqip->GetItem()->GetItemTemplet()->GetFashion() )
				{
					pSkinMesh = pCX2Eqip->GetNormalModel(0);
					pXET = pCX2Eqip->GetNormalTexChangeXET();
					break;
				}
				else
				{
					pSkinMesh = pCX2Eqip->GetNormalModel(0);
					pXET = pCX2Eqip->GetNormalTexChangeXET();
				}
			}
		}
		*/

		D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Footsteps" );

		//vPos.y += 150.f;

		m_hShadowKnotWeaponMesh = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
            (CX2GameUnit*) this,
			L"DAMAGE_EFFECT_SHADOW_KNOT_WEAPON", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
			m_FrameDataNow.unitCondition.landPosition.y );


		/*if( NULL != m_pShadowKnotWeaponMesh && NULL != m_pShadowKnotWeaponMesh->GetMainEffect()
			&& NULL != m_pShadowKnotWeaponMesh->GetMainEffect()->GetXSkinAnim() )
		{
			CKTDGXSkinAnim* pSkinAnim = m_pShadowKnotWeaponMesh->GetMainEffect()->GetXSkinAnim();

			if( NULL != pSkinMesh )
			{
				pSkinAnim->RemoveModelXSkinMesh( pSkinAnim->GetAnimXSkinMesh() );
				pSkinAnim->AddModelXSkinMesh( pSkinMesh, NULL, NULL, pXET, false );
			}
		}*/
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_START_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fShadowKnotTime = m_fDamageDataChangeTime;
		StateChange( ASI_SA_ARA_SHADOW_KNOT_WAIT );
	}

	CommonEventProcess();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_WAIT_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_WAIT_Init()
{
	XMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.Y" );
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_HIGH.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_KNOT.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE03.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SD_pa_source.X" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	TextureReadyInBackground( L"stone_AirShip.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_KNOT_KNOT_FINAL.dds" );
	TextureReadyInBackground( L"SD_pa_source.tga" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_TATOO.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_HIGHLIGHT.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_DIFFUSE.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_Attack.dds" );
	TextureReadyInBackground( L"RTW_Flame_Buster.dds" );
	TextureReadyInBackground( L"raven_beed_hit_spark.DDS" );
	TextureReadyInBackground( L"Ara_Black_LEAF.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_WAIT_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_WAIT_FrameMove()
{
	if ( 0.f < m_fShadowKnotTime )
		m_fShadowKnotTime -= m_fElapsedTime;

	if ( 0.f > m_fShadowKnotTime )
		m_fShadowKnotTime = 0.f;

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ARA_Shadow_Knot_Wait", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_WAIT_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_WAIT_EventProcess()
{
	if ( 0.f >= m_fShadowKnotTime )
		StateChange( ASI_SA_ARA_SHADOW_KNOT_CATCH );

	if ( 0.5f >= m_fShadowKnotTime && 0.033f < m_fShadowKnotTime )
	{
        if ( CX2DamageEffect::CEffect* pShadowKnotWeaponMesh
            = g_pX2Game->GetDamageEffect()->GetInstance( m_hShadowKnotWeaponMesh ) )
		{
			const CKTDGXMeshPlayer::CXMeshInstance* pMeshInstanceShadowKnot
				= pShadowKnotWeaponMesh->GetMainEffect();

			if ( NULL != pMeshInstanceShadowKnot )
			{
				float fDistance
					= GetDistance( GetPos(), pMeshInstanceShadowKnot->GetPos() );

				if ( 250.f >= fDistance )
					StateChange( ASI_SA_ARA_SHADOW_KNOT_CATCH );
			}
		}
	}

	CommonEventProcess();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_WAIT_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_WAIT_StateEnd()
{
    if ( CX2DamageEffect::CEffect* pShadowKnotWeaponMesh
        = g_pX2Game->GetDamageEffect()->GetInstance( m_hShadowKnotWeaponMesh ) )
	{
		CKTDGXMeshPlayer::CXMeshInstance* pMeshInstanceShadowKnotWeapon
			= pShadowKnotWeaponMesh->GetMainEffect();

		if ( NULL != pMeshInstanceShadowKnotWeapon )
			pMeshInstanceShadowKnotWeapon->SetNowLifeTime( pMeshInstanceShadowKnotWeapon->GetMaxLifeTime() );
	}

	CommonStateEnd();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_CATCH_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_CATCH_Init()
{
	XMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.Y" );
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_HIGH.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_KNOT.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE03.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SD_pa_source.X" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	TextureReadyInBackground( L"stone_AirShip.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_KNOT_KNOT_FINAL.dds" );
	TextureReadyInBackground( L"SD_pa_source.tga" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_TATOO.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_HIGHLIGHT.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_DIFFUSE.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_Attack.dds" );
	TextureReadyInBackground( L"RTW_Flame_Buster.dds" );
	TextureReadyInBackground( L"raven_beed_hit_spark.DDS" );
	TextureReadyInBackground( L"Ara_Black_LEAF.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_CATCH_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_CATCH_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ARA_SHADOW_KNOT_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;

		TRACE ( L"m_usBeforeForceGainCount = %d\n", m_usBeforeForceGainCount );
	}

	CommonEventProcess();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_END_Init()
{
	XMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_Ground.Y" );
	XMeshReadyInBackground( L"Split_Light01.Y" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_HIGH.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_TEMPEST_DANCE_BLADE_KNOT.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE_TATOO02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE01.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE02.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE03.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SA_ARA_SHADOW_KNOT_BLADE04.X" );
	XSkinMeshReadyInBackground( L"SD_pa_source.X" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"state_potion_flare.dds" );
	TextureReadyInBackground( L"stone_AirShip.dds" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_KNOT_KNOT_FINAL.dds" );
	TextureReadyInBackground( L"SD_pa_source.tga" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_AURA.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_TATOO.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_HIGHLIGHT.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_DIFFUSE.dds" );
	TextureReadyInBackground( L"SA_ARA_SHDOW_KNOT_BLADE_Attack.dds" );
	TextureReadyInBackground( L"RTW_Flame_Buster.dds" );
	TextureReadyInBackground( L"raven_beed_hit_spark.DDS" );
	TextureReadyInBackground( L"Ara_Black_LEAF.dds" );
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ARA_Shadow_Knot_End", this, NULL, false, GetPowerRate() );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.726f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Shadow_Knot_Final", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ARA_SHADOW_KNOT_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ARA_SHADOW_KNOT_END_EventProcess()
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
//#pragma endregion �׸��� �ŵ�

//#pragma region MOONLIGHT_SLASH

void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_READY_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_MOONLIGHT_SLASH_READY", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_READY_Init );
	stateData_.StateStart			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_READY_StateStart );
	stateData_.StateStartFuture	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_READY_StateStartFuture );
	stateData_.OnFrameMove			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_READY_FrameMove );
	stateData_.OnEventProcess		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_READY_EventProcess );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_Init
	@brief 		: ���ҽ� ��׶�� �ε�
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_READY_Init()
{
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"Ara_Orange_Light.dds" );
	TextureReadyInBackground( L"Explosion_Light02.dds" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE01.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE02.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.tga" );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_READY_StateStart()
{
	CommonStateStart();

	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		m_pXSkinAnim->SetPlaySpeed( m_fCurrentSpeed );

		m_sMoonRightSlashDataPtr->SetChargingTime( 0.f );
		m_sMoonRightSlashDataPtr->SetPowerRate( 0.f );
		UpForcePower( m_usForceGainCount );
	}
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_Ready_StateStartFuture
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_READY_StateStartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( m_fCurrentSpeed );

	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		switch ( m_sMoonRightSlashDataPtr->GetSlotID() )
		{
		case USI_SPECIAL_ATTACK_1:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureA );
			break;

		case USI_SPECIAL_ATTACK_2:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureS );
			break;

		case USI_SPECIAL_ATTACK_3:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureD );
			break;

		case USI_SPECIAL_ATTACK_4:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureC );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_1:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureQ );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_2:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureW );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_3:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureE );
			break;

		case USI_SPECIAL_ATTACK_SLOTB_4:
			m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureR );
			break;

		default:
			//ASSERT( !L"ASI_SA_ALH_MOONLIGHT_SLASh__READY_StateStartFuture" );
			//m_sMoonRightSlashDataPtr->SetChargeKey( &m_InputData.pureA );
			m_sMoonRightSlashDataPtr->SetChargeKey( NULL );
			break;
		}
	}
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_READY_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		if ( NULL != m_sMoonRightSlashDataPtr )
			m_sMoonRightSlashDataPtr->SetHandleEffectSet( g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_Charge", this ) );
	}

    if ( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_2Second", this );
	}

	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		const float fElapsedAnimationTime = m_pXSkinAnim->GetNowAnimationTime() - m_pXSkinAnim->GetBeforeAnimationTime();
		m_sMoonRightSlashDataPtr->SetChargingTime( m_sMoonRightSlashDataPtr->GetChargingTime() + fElapsedAnimationTime );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_READY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE );
	}
	else if ( NULL != m_sMoonRightSlashDataPtr && 0.66 <= m_sMoonRightSlashDataPtr->GetChargingTime()  )
	{
		if ( false == m_sMoonRightSlashDataPtr->GetChargeKey() )
		{
			if ( 1.5f <= m_sMoonRightSlashDataPtr->GetChargingTime() )
				StateChange( ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK );
			else
				StateChange( ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK );
		}
	}

	CommonEventProcess();
}

void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_EventProcess );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_Charge_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_Init()
{
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"Ara_Orange_Light.dds" );
	TextureReadyInBackground( L"Explosion_Light02.dds" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE01.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE02.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.tga" );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_Charge_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_FrameMove()
{
	CommonFrameMove();

	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			m_sMoonRightSlashDataPtr->SetChargingTime( m_sMoonRightSlashDataPtr->GetChargingTime() + m_fElapsedTime );
		}
		else
		{
			const float fElapsedAnimationTime = m_pXSkinAnim->GetNowAnimationTime() - m_pXSkinAnim->GetBeforeAnimationTime();
			m_sMoonRightSlashDataPtr->SetChargingTime( m_sMoonRightSlashDataPtr->GetChargingTime() + fElapsedAnimationTime );
		}
	}

}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_Charge_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_EventProcess()
{
	CommonEventProcess();

	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		if ( 5.0f <= m_sMoonRightSlashDataPtr->GetChargingTime() || false == m_sMoonRightSlashDataPtr->GetChargeKey() )
		{
			StateChange( ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK );
		}
	}
}


void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_FrameMove );
	stateData_.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateEnd );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_EventProcess );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_Init
	@brief 		: ���ҽ� ��׶�� �ε�
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_Init()
{
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE01.X" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE02.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"Ara_Orange_Light.dds" );
	TextureReadyInBackground( L"Explosion_Light02.dds" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE01.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE02.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.tga" );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateStart()
{
	CommonStateStart();

	if( NULL != m_sMoonRightSlashDataPtr )
		m_sMoonRightSlashDataPtr->SetPowerRate( floor( m_sMoonRightSlashDataPtr->GetChargingTime() ) * 0.1f );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_New", this );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.12f ) )
	{
		if ( NULL != m_sMoonRightSlashDataPtr )
		{
			float fPowerRate = GetPowerRate() + m_sMoonRightSlashDataPtr->GetPowerRate();

			bool bIsMemoAttack = false;		/// �޸� ���� ����

			/// �޸� : /// �� �޺����� (��� 4�� : �޺�����) - ���ݷ� 33% ����
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO9 ) )
				bIsMemoAttack = true;

			/// ���� �ڵ忡 ������ ���� ����Ʈ ����� �����Ǿ� ������ ������ �����ϰ� ������ �ʾƼ�, �޸������ ���
			if ( m_bMoonSlash )
				m_sMoonRightSlashDataPtr->SetHandleEffectSet(
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_Crescent_Attack_FullMoon", this, NULL, bIsMemoAttack, fPowerRate ) );
			else
				m_sMoonRightSlashDataPtr->SetHandleEffectSet(
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_Crescent_Attack", this, NULL, bIsMemoAttack, fPowerRate ) );

		}
	}

    if ( m_pXSkinAnim->EventTimerOneshot( 0.275f ) )
	{
		if ( NULL != m_sMoonRightSlashDataPtr )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_sMoonRightSlashDataPtr->GetHandleReferenceEffectSet() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateEnd()
{
	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		m_sMoonRightSlashDataPtr->SetChargingTime( 0.f );
		m_sMoonRightSlashDataPtr->SetPowerRate( 0.f );
	}

	CommonStateEnd();
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_EventProcess()
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

void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_FrameMove );
	stateData_.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateEnd );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_EventProcess );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_Init()
{
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE01.X" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE02.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"Ara_Orange_Light.dds" );
	TextureReadyInBackground( L"Explosion_Light02.dds" );
	TextureReadyInBackground( L"WhiteCircle02.dds" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE01.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE02.tga" );
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.tga" );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateStart()
{
	CommonStateStart();

	if( NULL != m_sMoonRightSlashDataPtr )
		m_sMoonRightSlashDataPtr->SetPowerRate( floor( m_sMoonRightSlashDataPtr->GetChargingTime() - 2.f ) * 0.1f );
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_Full", this );
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.033f ) )
	{
		if ( NULL != m_sMoonRightSlashDataPtr )
		{
			float fPowerRate = GetPowerRate() * ( 1.f + m_sMoonRightSlashDataPtr->GetPowerRate() );


			bool bIsMemoAttack = false;		/// �޸� ���� ����

			/// �޸� : /// �� �޺����� (��� 4�� : �޺�����) - RT_FLY���� RT_BIG_DAMAGE�� ����
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO9 ) )
				bIsMemoAttack = true;

			/// ���� �ڵ忡 ������ ���� ����Ʈ ����� �����Ǿ� ������ ������ �����ϰ� ������ �ʾƼ�, �޸������ ���
			if ( m_bMoonSlash )
				m_sMoonRightSlashDataPtr->SetHandleEffectSet(
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_Fullmoon_Attack_FullMoon", this, NULL, bIsMemoAttack, fPowerRate ) );
			else
				m_sMoonRightSlashDataPtr->SetHandleEffectSet(
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_MoonLight_Slash_Fullmoon_Attack", this, NULL, bIsMemoAttack, fPowerRate ) );
		}
	}

    if ( m_pXSkinAnim->EventTimerOneshot( 0.300f ) )
	{
		if ( NULL != m_sMoonRightSlashDataPtr )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_sMoonRightSlashDataPtr->GetHandleReferenceEffectSet() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateEnd()
{
	if ( NULL != m_sMoonRightSlashDataPtr )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_sMoonRightSlashDataPtr->GetHandleReferenceEffectSet() );

		m_sMoonRightSlashDataPtr->SetChargingTime( 0.f );
		m_sMoonRightSlashDataPtr->SetPowerRate( 0.f );
	}

	CommonStateEnd();
}

/** @function 	: ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_EventProcess()
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
//#pragma endregion �޺� ����

//#pragma region SWALLOW_ASSAULT
/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Ready_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Ready_Init()
{
	XMeshReadyInBackground( L"ALH_SWALLOW_ASSAULT_BASE.Y" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	XMeshReadyInBackground( L"Ara_Orange_Leaf.Y" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE02.X" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"A_ALH_ROCK_Strong.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateStart()
{
	CommonStateStart();

	m_sLockOnSystemPtr->SetPveRange( 1200.f );
	m_sLockOnSystemPtr->SetPvpRange( 700.f );
	m_sLockOnSystemPtr->SetSafePos( GetPos() );
	m_sLockOnSystemPtr->ProcessLockOn( this );
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Ready_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Ready_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		UseForcePower( 1 );
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Swallow_Assault_Ready", this );
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.818f && m_pXSkinAnim->GetNowAnimationTime() < 1.020f )
	{
		SetInvisibility( true );

		m_fRenderParamColorA -= m_fElapsedTime * 10.f;
		if( m_fRenderParamColorA < 0.0f )
			m_fRenderParamColorA = 0.0f;
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateEnd()
{
	CommonStateEnd();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Ready_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Ready_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == m_sLockOnSystemPtr->CheckLockOnUnit() )
		{
			m_sLockOnSystemPtr->ResetLockOnUnit();
			StateChange( ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK );
		}
		else
		{
			SetInvisibility( false );
			StateChange( ASI_SA_ALH_SWALLOW_ASSAULT_FINISH );
		}
	}

	CommonEventProcess();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Attack_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Attack_Init()
{
	XMeshReadyInBackground( L"Ring_Plane.Y" );
	XMeshReadyInBackground( L"AssaultSlash02.Y" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XMeshReadyInBackground( L"ALH_PEERLESSNESS_SHADOW01.Y" );
	XMeshReadyInBackground( L"ALH_PEERLESSNESS_SHADOW02.Y" );
	XMeshReadyInBackground( L"ALH_PEERLESSNESS_SHADOW03.Y" );
	XMeshReadyInBackground( L"ALH_PEERLESSNESS_SHADOW04.Y" );
	XMeshReadyInBackground( L"ALH_PEERLESSNESS_SHADOW05.Y" );
	XMeshReadyInBackground( L"ALH_PEERLESSNESS_SHADOW06.Y" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE02.X" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
	TextureReadyInBackground( L"Sander_Aelo_Light.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"RingAlpha.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_windLine.dds" );
	TextureReadyInBackground( L"Ara_Orange_Light.dds" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
	TextureReadyInBackground( L"Condense_Pulse02.dds" );
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateStart()
{
	CommonStateStart();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Attack_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Attack_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		m_FrameDataFuture.stateParam.bCanPushUnit = false;
		m_FrameDataFuture.stateParam.bCanPassUnit = true;

		D3DXVECTOR3 vMyDir = m_FrameDataNowBefore.unitCondition.dirVector;
		D3DXVECTOR3 vMyPos = GetPos();
		D3DXVECTOR3 vDestPos = m_sLockOnSystemPtr->GetDestPos();
		D3DXVECTOR3 vTargetDir =  vDestPos - vMyPos;
		float		fDot = 0.f;

        vTargetDir.y = 0.f;
        fDot = D3DXVec3Dot( &vMyDir, &vTargetDir );

		if ( fDot < 0.0f )
		{
			m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
			m_FrameDataFuture.unitCondition.dirDegree.y	+= 180.0f;
			m_FrameDataFuture.unitCondition.bAttackerFront = true;
		}
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.198f ) )
	{
		D3DXVECTOR3 vDestPos = m_sLockOnSystemPtr->GetDestPos();
		vDestPos.y += 20.f;

		m_FrameDataFuture.syncData.position = vDestPos;
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Swallow_Assault_Attack", this );

		int iRand = GetRandomInt() % 6;
		WCHAR wszText[64] = L"";
		swprintf( wszText, sizeof(wszText)/sizeof(WCHAR)-1, L"Effectset_Swallow_Assault_Shadow0%d", iRand );
		g_pX2Game->GetEffectSet()->PlayEffectSet( wszText, this, NULL, IsHyperState(), -1.f, -1.f, D3DXVECTOR3( 1.f, 1.f, 1.f ),
			true, vDestPos );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Attack_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Attack_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			if( NULL != g_pX2Game->GetWorld() && NULL != g_pX2Game->GetWorld()->GetLineMap() )
			{
				D3DXVECTOR3 outPos;
				int			lineIndex = 0;

				if( NULL != m_sLockOnSystemPtr )
				{
					if( g_pX2Game->GetWorld()->GetLineMap()->CanDown( GetPos(), LINE_RADIUS, &outPos, &lineIndex, false ) == true )
					{
						const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( lineIndex );

						if( NULL != pLineData )
						{	// ���θ��� ������ LT_NORMAL�� �ƴ� ��쳪 Ȱ��ȭ�Ǿ� ���� ���� ���
							if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
								MoveAraSafePosition();
						}
						else
						{	// ���θ��� ���� ���
							MoveAraSafePosition();
						}
					}
					else	/// ���� ��ġ �Ʒ��� ���θ��� ���� ��
					{
						MoveAraSafePosition();
					}
				}
			}

			StateChangeSpecialAttack( m_eSkillID );
		}
		else if ( 0 < GetForcePower() )
		{
			m_sLockOnSystemPtr->ProcessLockOn( this );

			if ( true == m_sLockOnSystemPtr->CheckLockOnUnit() )
			{
				UseForcePower( 1 );
				StateChange( ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK );
			}
			else
			{
				SetInvisibility( false );
				StateChange( ASI_SA_ALH_SWALLOW_ASSAULT_FINISH );
			}
		}
		else
		{
			StateChange( ASI_SA_ALH_SWALLOW_ASSAULT_FINISH );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateEnd()
{
	SetInvisibility( false );

	CommonStateEnd();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Finish_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Finish_Init()
{
	XMeshReadyInBackground( L"Ring_Plane.Y" );
	XMeshReadyInBackground( L"AssaultSlash02.Y" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SA_ALH_MOONLIGHT_SLASH_BLADE02.X" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
	TextureReadyInBackground( L"Sander_Aelo_Light.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"RingAlpha.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_windLine.dds" );
	TextureReadyInBackground( L"Ara_Orange_Light.dds" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
	TextureReadyInBackground( L"Condense_Pulse02.dds" );
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Finish_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Finish_StateStart()
{
	CommonStateStart();

	if( NULL != g_pX2Game->GetWorld() && NULL != g_pX2Game->GetWorld()->GetLineMap() )
	{
		D3DXVECTOR3 outPos;
		int			lineIndex = 0;

		if( NULL != m_sLockOnSystemPtr )
		{
			if( g_pX2Game->GetWorld()->GetLineMap()->CanDown( GetPos(), LINE_RADIUS, &outPos, &lineIndex, false ) == true )
			{
				const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( lineIndex );

				if( NULL != pLineData )
				{	// ���θ��� ������ LT_NORMAL�� �ƴ� ��쳪 Ȱ��ȭ�Ǿ� ���� ���� ���
					if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
						MoveAraSafePosition();
				}
				else
				{	// ���θ��� ���� ���
					MoveAraSafePosition();
				}
			}
			else	/// ���� ��ġ �Ʒ��� ���θ��� ���� ��
			{
				MoveAraSafePosition();
			}
		}
	}
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Finish_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Finish_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		SetInvisibility( false );
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Swallow_Assault_Finish", this );
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Swallow_Assault_Attack", this );
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Finish_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Finish_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/** @function 	: ASI_SA_ALH_SWALLOW_ASSAULT_Escape_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ALH_SWALLOW_ASSAULT_Escape_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	else if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//#pragma endregion ��

//#pragma endregion functions

//#pragma region active skill
//#pragma region TIGER_CLAW

void CX2GUAra::ASI_A_ARA_TIGER_CLAW_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ARA_TIGER_CLAW_START", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_START_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_START_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_START_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_START_EventProcess );
	stateData_.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_START_StateEnd );
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_START_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_START_Init()
{
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Blade2.X" );
	XSkinMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Blade3.X" );
	TextureReadyInBackground( L"SI_A_Ston_Cutter_Blade.dds" );
	TextureReadyInBackground( L"SI_A_Ston_Cutter_Ground_Wave.dds" );
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_START_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_START_StateStart()
{
	CommonStateStart();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;

	m_usBeforeForceGainCount = m_usForceGainCount;
	if( GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO11 ) == true )
		++m_usBeforeForceGainCount;
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_START_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_START_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.396f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Tiger_Claw", this, NULL, false, GetPowerRate() );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.561f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tiger_Claw_1st", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_START_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_A_ARA_TIGER_CLAW_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_START_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_START_StateEnd()
{
	CommonStateEnd();

	m_FrameDataNow.stateParam.bSuperArmorNotRed = true;
}


void CX2GUAra::ASI_A_ARA_TIGER_CLAW_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ARA_TIGER_CLAW_END", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_END_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ARA_TIGER_CLAW_END_EventProcess );
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_END_Init()
{
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	XSkinMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Blade3.X" );
	TextureReadyInBackground( L"SI_A_Ston_Cutter_Blade.dds" );
	TextureReadyInBackground( L"SI_A_Ston_Cutter_Ground_Wave.dds" );
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Tiger_Claw_Final", this, NULL, false, GetPowerRate() );
	}

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.165f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_ATTACK_BOX_Tiger_Claw_2nd", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ARA_TIGER_CLAW_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_TIGER_CLAW_END_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ȣ���� ����

//#pragma region FORCE_SPEAR
/** @function 	: ASI_A_ARA_FORCE_SPEAR_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_FORCE_SPEAR_Init()
{
	XSkinMeshReadyInBackground( L"elsword_SA_SwordBlastiong_slash.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"elsword_active_crecentCut_crecent.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Force_Spear_round.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
}

/** @function 	: ASI_A_ARA_FORCE_SPEAR_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_FORCE_SPEAR_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.198f ) && CanUseForcePower() )
	{
		UseForcePower( 1 );

		/// �޸� : ��ȥ ��� (���â) - ���â�� ��Ÿ� ����
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO3 ) )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Force_Spear_Memo", this );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Force_Spear", this );

		D3DXVECTOR3 vPos = GetWeaponBonePos( 0, L"Dummy3_Weapon", 0 );

		if ( true == GetIsRight() )
			vPos = vPos - GetDirVector() * 25.f;
		else
			vPos = vPos + GetDirVector() * 25.f;

		vPos.y -= 25.f;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AM_FORCE_SPEAR );
		if( NULL != pSkillTemplet )
		{
			USHORT usForceSpearPiercing = 0;
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AM_FORCE_SPEAR ) );	/// ��ų ����

			usForceSpearPiercing = static_cast<USHORT>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_PIERCING, iSkillTempletLevel ) );

			CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this,
				L"DAMAGE_ARA_FORCE_SPEAR", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );

			// ���â ���� ����, ���� Ƚ���� �̿��� ���� Ȯ��
			if ( NULL != pDamageEffect && 0 < usForceSpearPiercing )
			{
				pDamageEffect->SetDamageTime( pDamageEffect->GetDamageTime() + usForceSpearPiercing );

				/// �޸� : ��ȥ ��� (���â) - ���â�� ��Ÿ� ����
				if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO3 ) )
				{
					CKTDGXMeshPlayer::CXMeshInstance* pMainEffect =  pDamageEffect->GetMainEffect();

					if( NULL != pMainEffect )
						pMainEffect->SetMaxLifeTime( pMainEffect->GetMaxLifeTime() * _CONST_ARA_::FORCE_SPEAR_MEMO_ADDITIONAL_RANGE_RATE );
				}
			}
		}
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ARA_FORCE_SPEAR_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_FORCE_SPEAR_EventProcess()
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
//#pragma endregion ���â

//#pragma region GAIN_FORCE
/** @function 	: ASI_A_ARA_GAIN_FORCE_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_GAIN_FORCE_Init()
{
	XSkinMeshReadyInBackground( L"SI_A_ARA_GAIN_FORCE_KNOT.X" );
	TextureReadyInBackground( L"Heal01.dds" );
	TextureReadyInBackground( L"SI_A_ARA_GAIN_FORCE_NASUN_GATE_DEST.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
	TextureReadyInBackground( L"SM_SP_ShockWave.tga" );
	TextureReadyInBackground( L"SI_SA_ARA_SHADOW_RISING_Cloud.dds" );
	TextureReadyInBackground( L"Heal01.dds" );
}

/** @function 	: ASI_A_ARA_GAIN_FORCE_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_GAIN_FORCE_FrameMove()
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		m_usBeforeForceGainCount = m_usForceGainCount;

		D3DXVECTOR3 vGainForceSize( 1.f, 1.f, 1.f );	/// ����Ʈ ũ��

		/// �޸� : �� ���� ���� II (����) - ���� ũ�� ���� ( ����Ʈ ũ�� )
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO1 ) )
			vGainForceSize = _CONST_ARA_::GAIN_FORCE_MEMO_ADDITIONAL_RANGE_RATE;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ARA_Gain_Force", this, NULL, false, GetPowerRate(), -1.f, vGainForceSize );
	}

    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.066f ) )
	{
		D3DXVECTOR3 vPos = GetBonePos( L"Bip01" );

		if ( true == GetIsRight() )
			vPos = vPos + GetDirVector() * 180.f;
		else
			vPos = vPos - GetDirVector() * 180.f;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AM_GAIN_FORCE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AM_GAIN_FORCE ) );	/// ��ų ����

			float fDrainRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_INTAKE_ABS, iSkillTempletLevel );
			USHORT usPiercing = static_cast<USHORT>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_PIERCING, iSkillTempletLevel ) );

			// ���� ������
//#ifdef FIX_GAIN_FORCE_BUG
			m_iGainForceDamageTimeNow = 0;
			m_hGainForceIntakeEffect = INVALID_DAMAGE_EFFECT_HANDLE;
			//�ڵ尡 �ʹ� ������������ ���� ���� ���� �������������� ���ξ� m_ ���� ��
			CX2DamageEffect::CEffect* m_pGainForceDamaggeEffect =
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_SI_A_ARA_GAIN_FORCE_INTAKE",
				fDrainRate, vPos, GetRotateDegree(), GetRotateDegree(),	m_FrameDataNow.unitCondition.landPosition.y );

			if( NULL != m_pGainForceDamaggeEffect )
				m_hGainForceIntakeEffect = m_pGainForceDamaggeEffect->GetHandle();
//#else
//			m_pGainForceDamaggeEffect = NULL;
//			m_pGainForceDamaggeEffect = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this,
//				L"DAMAGE_EFFECT_SI_A_ARA_GAIN_FORCE_INTAKE", fDrainRate, vPos, GetRotateDegree(), GetRotateDegree(),
//				m_FrameDataNow.unitCondition.landPosition.y );
//#endif // FIX_GAIN_FORCE_BUG

			// �⺻ ������
			CX2DamageEffect::CEffect* pGainForceNormalDamage = g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*) this,
				L"DAMAGE_EFFECT_SI_A_ARA_GAIN_FORCE", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

			if ( NULL != m_pGainForceDamaggeEffect )
			{
				if( 0 < usPiercing )
					m_pGainForceDamaggeEffect->SetDamageTime( usPiercing );

				/// �޸� : �� ���� ���� II (����) - ���� ũ�� ���� ( Ÿ�� ���� )
				if( NULL != pGainForceNormalDamage && true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO1 ) )
				{
					m_pGainForceDamaggeEffect->SetScale( _CONST_ARA_::GAIN_FORCE_MEMO_ADDITIONAL_RANGE_RATE );
					pGainForceNormalDamage->SetScale( _CONST_ARA_::GAIN_FORCE_MEMO_ADDITIONAL_RANGE_RATE );
				}
			}
		}
	}

    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.297f ) )
	{
//#ifdef FIX_GAIN_FORCE_BUG
		if( 0 < m_iGainForceDamageTimeNow )
		{
			UpForcePower( m_usForceGainCount * m_iGainForceDamageTimeNow );
			m_iGainForceDamageTimeNow = 0;
			m_hGainForceIntakeEffect = INVALID_DAMAGE_EFFECT_HANDLE;
		}
//#else
//		if ( NULL != m_pGainForceDamaggeEffect && 0 < m_pGainForceDamaggeEffect->GetDamageTimeNow() )
//		{
//			UpForcePower( m_usForceGainCount * m_pGainForceDamaggeEffect->GetDamageTimeNow() );
//		}
//#endif // FIX_GAIN_FORCE_BUG

	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ARA_GAIN_FORCE_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_GAIN_FORCE_EventProcess()
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

	CommonEventProcess();
}
//#pragma endregion ����

//�ƶ� 1�� ���� - �Ҽ�(Little Hsien)
//�Ҽ� Ŀ�ǵ�
void CX2GUAra::ASI_COMBO_ALH_ZZXX_EventProcess() //ZZXX
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( ASI_COMBO_ALH_ZZXX_LANDING );
	}

	CommonEventProcess();
}
void CX2GUAra::ASI_COMBO_ALH_ZZXX_LANDING_EventProcess() //ZZXX
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )

	CommonEventProcess();
}
void CX2GUAra::ASI_DASH_COMBO_ALH_ZZ_EventProcess() //ZZ
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
void CX2GUAra::ASI_JUMP_ATTACK_ALH_ZZ_EventProcess() //ZZ
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

//�Ҽ� ��Ƽ��
void CX2GUAra::ASI_A_ALH_BREAKING_Init() //��ȭ���� ���
{
	TextureReadyInBackground( L"SI_A_ALH_BREAKING_Jin.dds" );
}
void CX2GUAra::ASI_A_ALH_BREAKING_FrameMove() //��ȭ���� ���
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Breaking", this );
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_A_ALH_BREAKING_EventProcess() //��ȭ���� ���
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{

		StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
void CX2GUAra::ASI_A_ALH_BREAKING_ATTACK_Init() //��ȭ���� ����
{
	XSkinMeshReadyInBackground( L"SI_A_ALH_BREAKING_Ground_Wave.x" );
	XSkinMeshReadyInBackground( L"SI_A_ALH_BREAKING_Traiectory.x" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
}
void CX2GUAra::ASI_A_ALH_BREAKING_ATTACK_FrameMove() //��ȭ���� ����
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Breaking_Attack", this );

		/// �޸� : �� �帣���� (��ȭ����) - ȸ���� 50% ����
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO7 ) )
		{
			SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_BUFF_BREAKING_MEMO ) );
		}
	}
	CommonFrameMove();
}
void CX2GUAra::ASI_A_ALH_BREAKING_ATTACK_EventProcess() //��ȭ���� ����
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

	CommonEventProcess();
}
void CX2GUAra::ASI_A_ALH_BREAKING_ATTACK_EndFuture() //��ȭ���� ����
{
	CommonStateEndFuture();
	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
}

void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_DOUBLE_COLLISION_START", stateData_.stateID );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DOUBLE_COLLISION_START_Start );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DOUBLE_COLLISION_START_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DOUBLE_COLLISION_START_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DOUBLE_COLLISION_START_EventProcess );
}

//�Ҽ� ����� ��Ƽ��
void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_START_Init() //�ȱ���_START
{
	XMeshReadyInBackground( L"AssaultSlash02.y" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Lire_Kick_Impact01.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"RingAlpha.dds" );
	TextureReadyInBackground( L"SA_ALH_DOUBLE_COLLISION_Rune_Base.tga" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"Sander_Aelo_Light.dds" );

}
void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_START_Start() //�ȱ���_START
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}
void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_START_FrameMove() //�ȱ���_START
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Double_Collision_01", this );
			UpForcePower( 1 );	// ��ų ��� �⺻ ��� ȸ��
			m_usBeforeForceGainCount = m_usForceGainCount;
		}
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		bool bIsMemoAttack = false;		/// �޸� ���� ����

		/// �޸� : ������ �Ұ� ����� Ÿ�� II (��� 3�� : �ȱ���) - ���ݷ� 20% ���� �� ���� �극��ũ ����
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO8 ) )
			bIsMemoAttack = true;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Double_Collision_02", this, NULL, bIsMemoAttack, GetPowerRate() );
	}


	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_START_EventProcess() //�ȱ���_START
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ALH_DOUBLE_COLLISION_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_DOUBLE_COLLISION_END", stateData_.stateID );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DOUBLE_COLLISION_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DOUBLE_COLLISION_END_EventProcess );
}
void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_END_FrameMove() //�ȱ���_END
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Double_Collision_03", this );
	}
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Ara_Double_Collision_04", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ALH_DOUBLE_COLLISION_END_EventProcess() //�ȱ���_END
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

	CommonEventProcess();
}

//�ƶ� 2�� ���� - ��õ(Sakra Devanam)

void CX2GUAra::ASI_DASH_JUMP_COMBO_ASD_ZX_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( ASI_COMBO_ALH_ZZXX_LANDING );
	}
	else if ( m_fAttackCancelAfter <= m_pXSkinAnim->GetNowAnimationTime() )
	{
		if ( true == m_InputData.oneZ )
		{
			StateChange( ASI_JUMP_ATTACK_Z );
		}
		else if ( true == m_InputData.oneX )
		{
			StateChange( ASI_JUMP_ATTACK_X );
		}
	}
	CommonEventProcess();
}
void CX2GUAra::ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}
	else if ( m_fAttackCancelAfter <= m_pXSkinAnim->GetNowAnimationTime() )
	{
		if ( true == m_InputData.oneZ )
		{
			StateChange( ASI_JUMP_ATTACK_Z );
		}
		else if ( true == m_InputData.oneX )
		{
			StateChange( ASI_JUMP_ATTACK_X );
		}
	}
	CommonEventProcess();
}

void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ASD_TURBULENT_WAVE_START", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_START_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_START_Start );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_START_FrameMove );
	stateData_.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_START_End );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_START_EventProcess );
}

void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_START_Init() // ��ȣ 4�� - �뵵
{
	XSkinMeshReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Wave.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Stone.X" );
	XMeshReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Stone_D.y" );
	TextureReadyInBackground( L"WhitePoint.dds");
	TextureReadyInBackground( L"Lire_Kick_Impact01.dds");
	TextureReadyInBackground( L"ARA_SA_FallingDragon_sparkHigh.dds");
	TextureReadyInBackground( L"Arme_Ring2.dds");
	TextureReadyInBackground( L"Sander_Aelo_Light.dds");
	TextureReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Tiger.dds");
	TextureReadyInBackground( L"CDC_LEG_SHOT_GUNFIRE.dds");
	TextureReadyInBackground( L"Colorballgray.dds");
	TextureReadyInBackground( L"stone.dds");
	TextureReadyInBackground( L"A_ALH_ROCK_CRASH_Tiger.tga" );
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_START_Start() // ��ȣ 4�� - �뵵
{
	CommonStateStart();
	m_usBeforeForceGainCount = m_usForceGainCount;
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_START_FrameMove()// ��ȣ 4�� - �뵵
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		UpForcePower(1);
		m_hTempEffectSet =
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Start", this );
		m_vecEffectSetToDeleteOnDamageReact.push_back(m_hTempEffectSet);
		m_vecEffectSetToDeleteOnDie.push_back(m_hTempEffectSet);
	}

	//�ٴ� �������� ����
	if( true == IsOnSomethingFuture() )
	{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.402f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.602f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.802f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 1.002f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 1.202f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_START_EventProcess()// ��ȣ 4�� - �뵵
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( ASI_SA_ASD_TURBULENT_WAVE_LOOP );
	}
	CommonEventProcess();
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_START_End()
{
	CommonStateEnd();

	if( false == m_pXSkinAnim->IsAnimationEnd() )
	{
		if ( INVALID_EFFECTSET_HANDLE != m_hTempEffectSet )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hTempEffectSet );

	}
}


void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_LOOP_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ASD_TURBULENT_WAVE_LOOP", stateData_.stateID );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_LOOP_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_LOOP_EventProcess );
	stateData_.StateEnd			= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_LOOP_End );
}

void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_LOOP_FrameMove()// ��ȣ 4�� - �뵵
{
	//�ٴ� �������� ����
	if( true == IsOnSomethingFuture() )
	{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.051f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.251f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.451f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_Stream", this, NULL, false, GetPowerRate() );
		}
	}
	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_LOOP_EventProcess()// ��ȣ 4�� - �뵵
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_SA_ASD_TURBULENT_WAVE_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_LOOP_End()// ��ȣ 4�� - �뵵
{
	CommonStateEnd();
	if ( INVALID_EFFECTSET_HANDLE != m_hTempEffectSet )
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hTempEffectSet );
}

void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ASD_TURBULENT_WAVE_END", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_END_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_TURBULENT_WAVE_END_EventProcess );
}

void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_END_Init()// ��ȣ 4�� - �뵵
{
	XSkinMeshReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Stone_A.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Stone_B.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_TURBULENT_WAVE_Stone_C.X" );
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_END_FrameMove()// ��ȣ 4�� - �뵵
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		if( true == IsOnSomethingFuture() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Turbulent_Wave_End", this, NULL, false, GetPowerRate() );
		}
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ASD_TURBULENT_WAVE_END_EventProcess()// ��ȣ 4�� - �뵵
{
	if ( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.974f && false == IsOnSomethingFuture())
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//#pragma region ASI_SA_ASD_SUPPRESSION
/** @function 	: ASI_SA_ASD_SUPPRESSION_Init
	@brief 		: �ƶ� 2�� ���� ��õ - ����
*/
void CX2GUAra::ASI_SA_ASD_SUPPRESSION_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Divide.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave_A.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave_B.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Sk_Dummy.X" );
	XSkinMeshReadyInBackground( L"Triple_W_PropertyA01.X" );
	XSkinMeshReadyInBackground( L"rena_A_evoke_Dummy.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds");
	TextureReadyInBackground( L"Arme_Critical2.dds");
	TextureReadyInBackground( L"Arme_SpecialAttack1.dds");
	TextureReadyInBackground( L"ColorBallFire.dds");
	TextureReadyInBackground( L"ColorBallGray.dds");
	TextureReadyInBackground( L"Condense_Light01.dds");
	TextureReadyInBackground( L"Explosion_Light_Ring01.dds");
	TextureReadyInBackground( L"Gate_Light_Arua.dds");
	TextureReadyInBackground( L"Gate_Light2.dds");
	TextureReadyInBackground( L"GuideArrow01.dds");
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds");
}

/** @function 	: ASI_SA_ASD_SUPPRESSION_StateStart
	@brief 		: �ƶ� 2�� ���� ��õ - ����
*/
void CX2GUAra::ASI_SA_ASD_SUPPRESSION_StateStart()
{

	CommonStateStart();
}

/** @function 	: ASI_SA_ASD_SUPPRESSION_FrameMove
	@brief 		: �ƶ� 2�� ���� ��õ - ����
*/
void CX2GUAra::ASI_SA_ASD_SUPPRESSION_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    /// ���� ����Ʈ ������ ���� ���� ������ ����Ʈ ����
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		// ������ // �系���� ��� ���� ��� �� �� ASSERT�߻� ���� ����
		m_fSuppresionPowerRate = max(1.f, GetPowerRate() * GetForcePower());

		UseForcePower( GetForcePower() );
		D3DXVECTOR3 vFirePos = GetPos();

		/// y���� ���� ���� ���� ����
		if( true == GetIsRight() )
		{
			vFirePos.x += GetDirVector().x * 500.f;
			vFirePos.z += GetDirVector().z * 500.f;
		}
		else
		{
			vFirePos.x -= GetDirVector().x * 500.f;
			vFirePos.z -= GetDirVector().z * 500.f;
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ASD_SUPPRESSION_READY_GROUND", m_fSuppresionPowerRate, vFirePos,
			GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    /// ���� Ÿ�� ������ ����Ʈ ����
    else if( true == m_pXSkinAnim->EventTimerOneshot( 1.15f ) )
	{
		D3DXVECTOR3 vFirePos = GetPos();

		if( true == GetIsRight() )
			vFirePos += GetDirVector() * 500.f;
		else
			vFirePos -= GetDirVector() * 500.f;

		vFirePos.y += 700.f;

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ASD_SUPPRESSION_DOWN_FORCE", m_fSuppresionPowerRate,
				vFirePos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ASD_SUPPRESSION_EventProcess
	@brief 		: �ƶ� 2�� ���� ��õ - ����
*/
void CX2GUAra::ASI_SA_ASD_SUPPRESSION_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	CommonEventProcess();
}
//#pragma endregion ����

void CX2GUAra::ASI_SA_ALH_POWER_OF_WHITER_TIGER_Init()
{
	XSkinMeshReadyInBackground(L"Ara_Dash_Combo_X_M01.X");
	XMeshReadyInBackground( L"SI_A_ARA_STONE_CUTTER_Ground_Wave.Y" );
	TextureReadyInBackground( L"Arme_Ring2.dds");
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds");
	TextureReadyInBackground( L"Ara_Orange_Light.dds");
	TextureReadyInBackground( L"Explosion_Light02.dds");
	TextureReadyInBackground( L"WhiteCircle02.DDS");
	TextureReadyInBackground( L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.tga");
	TextureReadyInBackground( L"ALH_POWER_OF_WHITE_TIGER.TGA");
	TextureReadyInBackground( L"A_ALH_ROCK_CRASH_Tiger.tga");
	TextureReadyInBackground( L"Particle_Blur.DDS");
	TextureReadyInBackground( L"Explosion_Sphere.dds");
	TextureReadyInBackground( L"WhiteCircle02.DDS");
}
void CX2GUAra::ASI_SA_ALH_POWER_OF_WHITER_TIGER_FrameMove() // ��ȣ��
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		UpForcePower( 1 );
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_Start", this );
	}
    else if ( true == m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		m_hTempEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_BlackHole", this );
	}
    else if ( true == m_pXSkinAnim->EventTimerOneshot( 1.f ) )
	{
		if( true == GetIsRight() )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_Face", this );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_Face_Left", this );
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ALH_POWER_OF_WHITER_TIGER_EventProcess() // ��ȣ��
{
	if( true == m_pXSkinAnimFuture->IsAnimationEnd() &&
		true == IsOnSomethingFuture() )
	{
		StateChange( ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING );
	}
	CommonEventProcess();
}
void CX2GUAra::ASI_SA_ALH_POWER_OF_WHITER_TIGER_End()
{
	CommonStateEnd();

	if ( INVALID_EFFECTSET_HANDLE != m_hTempEffectSet )
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hTempEffectSet );
}
void CX2GUAra::ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING_FrameMove() // ��ȣ��
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_Landing", this, NULL, false, GetPowerRate() );

		if( true == GetIsRight() )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_Face", this );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Power_of_WhiteTiger_Face_Left", this );

		CX2DamageEffect::CEffect* pDE =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ALH_POWER_OF_WHITE_TIGER_BUFF",
				GetPowerRate(), GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO12 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ALH_POWER_OF_WHITE_TIGER_MEMO_DEBUFF",
				GetPowerRate(), GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );

		if( NULL != pDE )		/// ��ų ������ ���� ���� ���� ����
		{
			CX2DamageManager::DamageData& damageData = pDE->GetDamageData();
			//if( NULL != pDamageData )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ALH_WHITE_TIGER );

				if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
				{
					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ALH_WHITE_TIGER ) );	/// ��ų ����

					damageData.PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
				}

			}
		}
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING_EventProcess() // ��ȣ��
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_START_Init() // ������
{
	TextureReadyInBackground( L"SA_ALH_PEERLESSNESS_SLASH.tga" );
	TextureReadyInBackground( L"elsword_active_crecentCut_crecent.dds" );
	TextureReadyInBackground( L"title_StarLight_Smash_spark.dds" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_particle.dds" );
	TextureReadyInBackground( L"Sander_Aelo_Light.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Condense_Light01.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.DDS" );
	TextureReadyInBackground( L"SM_MagicAttakcB_Smoke.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"Ara_Orange_LEAF.dds" );
	TextureReadyInBackground( L"ARA_SA_WindWedge_windLine.dds" );
	XMeshReadyInBackground( L"Wind_Liner02.Y" );
	XMeshReadyInBackground( L"Ring_Plane.y" );
	XMeshReadyInBackground( L"SA_ALH_PEERLESSNESS_SHADOW.Y" );
	XMeshReadyInBackground( L"SA_ALH_PEERLESSNESS_SHADOW02.Y" );
	XMeshReadyInBackground( L"SA_ALH_PEERLESSNESS_SHADOW03.Y" );
}
void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_START_Start()
{
	CommonStateStart();
	m_usBeforeForceGainCount = m_usForceGainCount;
}
void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_START_FrameMove() // ������
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

	const float fInvisibleStartTime = 0.55f;

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		UpForcePower( 1 );
		CX2EffectSet::Handle hEffectSset =
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Peerlessness_Slash_Start", this );
		m_vecEffectSetToDeleteOnDie.push_back(hEffectSset);
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( fInvisibleStartTime ) )
	{
		SetInvisibility(true);
		m_fRenderParamColorA = 0.f;
		m_vPreTransScale = (true == IsChangedGameUnitScaleByBuff()) ? m_vTransScale : D3DXVECTOR3(1,1,1) ;

		TransformScale( PROTECT_VECTOR3( 0.01f, 0.01f, 0.01f ) );

		CX2EffectSet::Handle hEffectSset =
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Peerlessness_Slash_Attack_Effect", this, NULL, false, GetPowerRate() );
		m_vecEffectSetToDeleteOnDie.push_back(hEffectSset);
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.710f ) )
	{
		CX2EffectSet::Handle hEffectSset =
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Peerlessness_Slash_Attack", this, NULL, false, GetPowerRate() );
		m_vecEffectSetToDeleteOnDie.push_back(hEffectSset);
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 1.532f ) )
	{
		SetInvisibility(false);
		m_fRenderParamColorA = 1.f;
		TransformScale( m_vPreTransScale );
	}
	CommonFrameMove();

	m_pUnitShadow->SetShowObject(!m_bInvisibility);
}
void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_START_EventProcess() // ������
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_SA_ALH_PEERLESSNESS_SLASH_END );
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() && true == IsOnSomethingFuture() ) //�ٴڿ����� ��� ĵ�� �� �� �ֵ��� ����
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}
void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_START_End() // ������
{
	CommonStateEnd();
	SetInvisibility(false);
	m_fRenderParamColorA = 1.f;
	TransformScale( m_vPreTransScale );
}
void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_END_FrameMove() // // ������
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Peerlessness_Slash_End", this );
	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.138f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Peerlessness_Slash_Last_Attack", this, NULL, false, GetPowerRate() );
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ALH_PEERLESSNESS_SLASH_END_EventProcess() // // ������
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_JUMP_DOWN );
	}
	CommonEventProcess();
}

void CX2GUAra::SetInvisibility(bool bVal_)
{
	m_bInvisibility					= bVal_;
	m_bAbsoluteInvisibility			= bVal_;

	if( false == m_bInvisibility )
		m_RenderParam.color.a = m_fPreRenderParamColorA;

	m_fPreRenderParamColorA = m_RenderParam.color.a;
	m_fRenderParamColorA	= 1.f;

	m_RenderParam.bAlphaBlend = m_bInvisibility;
	SetAlphaObject( m_bInvisibility );

	m_pXSkinAnim->SetLayer(1);
	// ��� ������ ��ƼŬ
	for( CX2EqipPtr pEquip : m_ViewEqipList )
	{
		if( pEquip != NULL )
			pEquip->SetShowAttachedParticle( !m_bInvisibility );
	}

	// ���� �Ӽ� ��æƮ ��ƼŬ
	SetShowEnchantWeaponParticle( !m_bInvisibility );
	// ������ ��
	if ( false == IsMyUnit() && false == IsNullGageUI() )
	{
		if( true == m_bInvisibility )
			m_pGageUI->SetAlpha( 0.f );
		else
			m_pGageUI->SetAlpha( 255.f );
	}

	// ���� ���¶�� ���� ��ƼŬ �Ѱ�/����
	if( GetRemainHyperModeTime() > 0.f )
	{
		if ( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hHyperBoostRFoot )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq_RFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRFoot );
			if( NULL != pSeq_RFoot )
				pSeq_RFoot->SetShowObject( !m_bInvisibility );
		}
		if ( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hHyperBoostLFoot )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq_LFoot = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLFoot );
			if( NULL != pSeq_LFoot )
				pSeq_LFoot->SetShowObject( !m_bInvisibility );
		}
		if ( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hHyperBoostRArm )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq_RArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostRArm );
			if( NULL != pSeq_RArm )
				pSeq_RArm->SetShowObject( !m_bInvisibility );
		}
		if ( INVALID_PARTICLE_SEQUENCE_HANDLE != m_hHyperBoostLArm )
		{
			CKTDGParticleSystem::CParticleEventSequence* pSeq_LArm = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hHyperBoostLArm );
			if( NULL != pSeq_LArm )
				pSeq_LArm->SetShowObject( !m_bInvisibility );
		}
	}
}


void CX2GUAra::ASI_SA_ALH_DESCENT_DRAGON_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	//����
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALH_DESCENT_DRAGON", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DESCENT_DRAGON_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DESCENT_DRAGON_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALH_DESCENT_DRAGON_EventProcess );
}

void CX2GUAra::ASI_SA_ALH_DESCENT_DRAGON_Init() /// ��� �������� - ����
{
	XSkinMeshReadyInBackground(L"SI_SA_ALH_DESCENT_DRAGON_Stone_A.X");
	XSkinMeshReadyInBackground(L"SI_SA_ALH_DESCENT_DRAGON_Stone_B.X");
	XMeshReadyInBackground(L"SI_SA_ALH_DESCENT_DRAGON_MiniStone_A.y");
	XMeshReadyInBackground(L"SI_SA_ALH_DESCENT_DRAGON_Prickle.y");
	XMeshReadyInBackground(L"Up_Light01_Mesh.Y");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
}
void CX2GUAra::ASI_SA_ALH_DESCENT_DRAGON_FrameMove() /// ��� �������� - ����
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		if ( true == GetIsRight() )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ALH_Descent_Dragon", this );
		else
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ALH_Descent_Dragon", this, NULL, IsHyperState(), -1.f, -1.f,
			GetVec3ScaleByUnit(), true, GetPos(), GetRotateDegree() + D3DXVECTOR3( 0, 180, 0 ), GetDirVector() );

	}
    else if( true == m_pXSkinAnim->EventTimerOneshot( 0.725f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( (CX2GameUnit*)this, L"DAMAGE_EFFECT_ALH_DESCENT_DRAGON",
			GetPowerRate(), GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree(),
			m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}
void CX2GUAra::ASI_SA_ALH_DESCENT_DRAGON_EventProcess() /// ��� �������� - ����
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}

//#pragma region ROCK_CRASH


void CX2GUAra::ASI_A_ALH_ROCK_CRASH_FALLING_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ALH_ROCK_CRASH_FALLING_START", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_START_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_START_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_START_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_START_EventProcess );
	stateData_.StateEnd		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_START_StateEnd );
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_START_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_START_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"A_ALH_ROCK_CRASH_Tiger.tga" );
	TextureReadyInBackground( L"A_ALH_ROCK_Strong.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_START_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_START_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_START_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_START_FrameMove()
{
    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		CX2EffectSet::Handle hEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Rock_Crash_Start", this, NULL,
			false, GetPowerRate() );

		if ( INVALID_EFFECTSET_HANDLE != hEffectSet )
			m_vecEffectSetToDeleteOnStateEnd.push_back( hEffectSet );
	}

    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.275f ) )
	{


		/// �޸� : ������ ���� ġ��(��ȣ 1�� : ��������) - ũ��Ƽ�� 40% ����
		bool bIsMemoAttack = false;

		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO6 ) )
			bIsMemoAttack = true;

		m_hTempEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ALH_DOUBLE_COLLISION_ATTACKBOX", this, NULL,
			bIsMemoAttack, GetPowerRate() );


	}

    if ( true == m_pXSkinAnim->EventTimerOneshot( 0.655f ) )
	{
		if ( INVALID_EFFECTSET_HANDLE != m_hTempEffectSet )
			g_pX2Game->GetEffectSet()->StopEffectSet( m_hTempEffectSet );
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_START_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_START_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_A_ALH_ROCK_CRASH_END );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_START_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_START_StateEnd()
{
	if ( INVALID_EFFECTSET_HANDLE != m_hTempEffectSet )
		g_pX2Game->GetEffectSet()->StopEffectSet( m_hTempEffectSet );

	CommonStateEnd();

	m_PhysicParam.nowSpeed.x = 700.f;
}

void CX2GUAra::ASI_A_ALH_ROCK_CRASH_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ALH_ROCK_CRASH_END", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_END_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALH_ROCK_CRASH_END_EventProcess );
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_END_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_END_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"A_ALH_ROCK_CRASH_Tiger.tga" );
	TextureReadyInBackground( L"A_ALH_ROCK_Strong.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_END_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_END_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{


		/// �޸� : ������ ���� ġ��(��ȣ 1�� : ��������) - ũ��Ƽ�� 40% ����
		bool bIsMemoAttack = false;

		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO6 ) )
			bIsMemoAttack = true;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Rock_Crash_End", this, NULL, bIsMemoAttack, GetPowerRate() );


	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ALH_ROCK_CRASH_END_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_ROCK_CRASH_END_EventProcess()
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

	CommonEventProcess();
}
//#pragma endregion ���� ����

//#pragma region LOW_BRANDISH

void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_SLIDING_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID	= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ASD_LOW_BRANDISH_SLICE", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_SLIDING_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_SLIDING_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_SLIDING_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_SLIDING_EventProcess );
	stateData_.StateEnd		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_SLIDING_StateEnd );
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_SLIDING_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_SLIDING_Init()
{
	XSkinMeshReadyInBackground( L"SI_A_ASD_LOW_BRANDISH_Cut_A.X" );
	XSkinMeshReadyInBackground( L"SI_A_ASD_LOW_BRANDISH_Cut_B.X" );
	XSkinMeshReadyInBackground( L"WindWard_XMesh01.X" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds" );
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_SLIDING_StateStart
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_SLIDING_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_SLIDING_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_SLIDING_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Low_Brandish_Sliding_Kick", this, NULL, false, GetPowerRate() );

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ASD_LOW_BRANDISH );
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );	/// �ڽ����� �Ŵ� ����
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_SLIDING_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_SLIDING_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_A_ASD_LOW_BRANDISH_KICK );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_SLIDING_StateEnd
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_SLIDING_StateEnd()
{
	CommonStateEnd();
}

void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_KICK_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ASD_LOW_BRANDISH_KICK", stateData_.stateID );
	stateData_.StateInit			= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_KICK_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_KICK_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ASD_LOW_BRANDISH_KICK_EventProcess );
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_KICK_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_KICK_Init()
{
	XMeshReadyInBackground( L"SI_A_ASD_LOW_BRANDISH_Kick.y" );
	XSkinMeshReadyInBackground( L"WindWard_XMesh01.X" );
	XSkinMeshReadyInBackground( L"SI_A_ASD_LOW_BRANDISH_Cut_B.X" );
	TextureReadyInBackground( L"Colorballgray.dds" );
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_KICK_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_KICK_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_Low_Brandish_Kick", this );
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ASD_LOW_BRANDISH_KICK_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ASD_LOW_BRANDISH_KICK_EventProcess()
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

	CommonEventProcess();
}
//#pragma endregion ����

//#pragma region VOID_BREAK


void CX2GUAra::ASI_SA_ASD_VOID_BREAK_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ASD_VOID_BREAK", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_VOID_BREAK_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_VOID_BREAK_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ASD_VOID_BREAK_EventProcess );
}

/** @function 	: ASI_SA_ASD_VOID_BREAK_Init
	@brief 		:
*/
void CX2GUAra::ASI_SA_ASD_VOID_BREAK_Init()
{
	XSkinMeshReadyInBackground( L"Lire_SI_SA_Gungnir_Mesh05.X" );
	TextureReadyInBackground( L"ALH_POWER_OF_WHITE_TIGER.tga" );
	TextureReadyInBackground( L"A_ALH_ROCK_CRASH_Tiger.tga" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	TextureReadyInBackground( L"ColorBallRed2.dds" );
	TextureReadyInBackground( L"Condense_Pulse01.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"waldo_trock_specialAttackB_cry_waveLine.dds" );
}

/** @function 	: ASI_SA_ASD_VOID_BREAK_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_SA_ASD_VOID_BREAK_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLightByBerserkMode( 0.001f );

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		bool bHyper = false;
		if ( GetRemainHyperModeTime() > 0.f )
			bHyper = true;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ASD_Void_Break", this, NULL, bHyper, GetPowerRate() );

		CX2DamageEffect::CEffect* pDE =
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ALH_POWER_OF_WHITE_TIGER_BUFF",
			GetPowerRate(), GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree(),
			m_FrameDataNow.unitCondition.landPosition.y );

		if( NULL != pDE )		/// ��ų ������ ���� ���� ���� ����
		{
			CX2DamageManager::DamageData& damageData = pDE->GetDamageData();
			//if( NULL != pDamageData )
			{
				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				CX2SkillTree* pSkillTree = g_pData->GetSkillTree();
				const int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ASD_POWER_OF_TIGER, true );

				if ( iSkillLevel > 0 && NULL != pSkillTree )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ASD_POWER_OF_TIGER  );

					if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
					{
						CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillLevel );
						SetBuffFactorToGameUnit( *ptrBuffFactorClone );
						damageData.PushBuffFactor( ptrBuffFactorClone );
					}
				}
			}
		}
	}

	CommonFrameMove();
}

/** @function 	: ASI_SA_ASD_VOID_BREAK_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_SA_ASD_VOID_BREAK_EventProcess()
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
//#pragma endregion ��ȣ �������� ��ȣ ��õ


//#pragma region SI_A_ARA_QUICK_PIERCING
/** @function 	: ASI_A_ARA_Quick_Piercing_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_Quick_Piercing_EventProcess()
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

	CommonEventProcess();
}

void CX2GUAra::ASI_A_ARA_Quick_Piercing_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( m_fDamageDataChangeTime ) )
	{
		bool bTableOpen = m_LuaManager.BeginTableByReference( m_NowStateData.stateID );

		if ( true == bTableOpen )
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
//#pragma endregion õ��

//#pragma region SI_A_ARA_SHOULDER_BLOW
/** @function 	: ASI_A_ARA_Shoulder_Blow_Init
	@brief 		:
*/
void CX2GUAra::ASI_A_ARA_Shoulder_Blow_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"A_ALH_ROCK_CRASH_Tiger.tga" );
	TextureReadyInBackground( L"A_ALH_ROCK_Strong.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
}

/** @function 	: ASI_A_ALH_Shoulder_Blow_FrameMove
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_Shoulder_Blow_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		UseForcePower( 2 );

		bool bHyper = false;
		if ( GetRemainHyperModeTime() > 0.f )
			bHyper = true;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_A_ARA_SHOULDER_BLOW", this, NULL, bHyper, GetPowerRate() );
	}

	CommonFrameMove();
}

/** @function 	: ASI_A_ALH_Shoulder_Blow_EventProcess
	@brief 		:
*/
void CX2GUAra::ASI_A_ALH_Shoulder_Blow_EventProcess()
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
//#pragma endregion ö���

//#pragma region ASI_A_ASD_Falling_Blossom
/** @function 	: ASI_A_ASD_Falling_Blossom_Start_Init
	@brief 		: ��ų ���� - ��ȭ
*/
void CX2GUAra::ASI_A_ASD_Falling_Blossom_Start_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Divide.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave_A.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave_B.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Sk_Dummy.X" );
	XSkinMeshReadyInBackground( L"Triple_W_PropertyA01.X" );
	XSkinMeshReadyInBackground( L"rena_A_evoke_Dummy.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds");
	TextureReadyInBackground( L"Arme_Critical2.dds");
	TextureReadyInBackground( L"Arme_SpecialAttack1.dds");
	TextureReadyInBackground( L"ColorBallFire.dds");
	TextureReadyInBackground( L"ColorBallGray.dds");
	TextureReadyInBackground( L"Condense_Light01.dds");
	TextureReadyInBackground( L"Explosion_Light_Ring01.dds");
	TextureReadyInBackground( L"Gate_Light_Arua.dds");
	TextureReadyInBackground( L"Gate_Light2.dds");
	TextureReadyInBackground( L"GuideArrow01.dds");
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds");
}

void CX2GUAra::ASI_A_ASD_Falling_Blossom_Start_StateStart()
{
	CommonStateStart();

	UpForcePower( 1 );
	m_usBeforeForceGainCount = m_usForceGainCount;
}

void CX2GUAra::ASI_A_ASD_Falling_Blossom_Start_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
		{
			StateChange( ASI_A_ASD_FALLING_BLOSSOM_FINISH );
		}
	}

	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

void CX2GUAra::ASI_A_ASD_Falling_Blossom_Finish_Init()
{
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Divide.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave_A.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Wave_B.X" );
	XSkinMeshReadyInBackground( L"SI_SA_ASD_SUPPRESSION_Sk_Dummy.X" );
	XSkinMeshReadyInBackground( L"Triple_W_PropertyA01.X" );
	XSkinMeshReadyInBackground( L"rena_A_evoke_Dummy.X" );
	TextureReadyInBackground( L"Arme_Ring2.dds");
	TextureReadyInBackground( L"Arme_Critical2.dds");
	TextureReadyInBackground( L"Arme_SpecialAttack1.dds");
	TextureReadyInBackground( L"ColorBallFire.dds");
	TextureReadyInBackground( L"ColorBallGray.dds");
	TextureReadyInBackground( L"Condense_Light01.dds");
	TextureReadyInBackground( L"Explosion_Light_Ring01.dds");
	TextureReadyInBackground( L"Gate_Light_Arua.dds");
	TextureReadyInBackground( L"Gate_Light2.dds");
	TextureReadyInBackground( L"GuideArrow01.dds");
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds");
}

void CX2GUAra::ASI_A_ASD_Falling_Blossom_Finish_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ��ȭ

//#pragma region SI_A_ARA_QUICK_STAB
/** @function 	: ASI_A_ARA_Quick_Stab_Init
	@brief 		: ��ų ���� - �ż�
*/
void CX2GUAra::ASI_A_ARA_Quick_Stab_Init()
{
	XSkinMeshReadyInBackground( L"elsword_SA_SwordBlastiong_slash.X" );
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_circle.X" );
	TextureReadyInBackground( L"elsword_SA_SwordBlastiong_slash.dds");
	TextureReadyInBackground( L"elsword_active_crecentCut_crecent.dds");
}

void CX2GUAra::ASI_A_ARA_Quick_Stab_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		UseForcePower( 2 );
	}

	CommonFrameMove();
}

void CX2GUAra::ASI_A_ARA_Quick_Stab_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion �ż�


//#pragma endregion functions

//#pragma region passive skill
//#pragma region QUICKEN_GUARD
/** @function 	:
	@brief 		:
*/
void CX2GUAra::ASI_P_ASD_QUICKEN_GUARD_StateStart()
{
	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_CSI_CIP_IRON_WILL", this );

	CommonStateStart();
}
/** @function 	: ASI_P_ASD_QUICKEN_GUARD_EventProcess
	@brief 		: �����̴� ���� EventProcess
*/
void CX2GUAra::ASI_P_ASD_QUICKEN_GUARD_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
		{
			StateChange( USI_WAIT );
		}
	}

	CommonEventProcess();
}

/** @function 	: GetBlockingRate, GetTurnigRate
	@brief 		: �����̴� ���� �нú� ���� ���� �� ���Ƹ±� ���� �� Ȯ��
*/
float CX2GUAra::GetBlockingRate()
{
	return m_fBlockingRate;
}

float CX2GUAra::GetTurnigRate()
{
	return m_fTurningRate;
}
//#pragma endregion �����̴� ����

//#pragma region VIGOR_ENERGIZE
/** @function 	: GetReduceDebuffDurationRate
	@brief 		: ��� Ȱ��ȭ ����� �ð� ����
*/
float CX2GUAra::GetReduceDebuffDurationRate(BUFF_TEMPLET_ID eBuffTempletID_)
{
	float fReduceDebuffDurationRate = 1.f;

	if ( NULL != g_pX2Game )
	{
		/// �طο� ȿ����� ���ǵ� �������� ����� ����Ʈ�� �ش� ������ ���ԵǾ� �ִٸ�, ���� �ð� ����
		for( BUFF_TEMPLET_ID eBuffTempletID : g_pX2Game->GetOfficiallyDebuffList() )
		{
			if ( eBuffTempletID_ == eBuffTempletID )
				fReduceDebuffDurationRate = m_fReduceEDTDurationRate;
		}
	}

	return fReduceDebuffDurationRate;
}
//#pragma endregion ��� Ȱ��ȭ

//#pragma endregion functions

void CX2GUAra::ASI_HA_ASD_THOUSANDS_BLOSSOMS_Init()
{
	XSkinMeshReadyInBackground(L"ALH_MOONLIGHT_SLASH_BLADE_FULL_BASE.X");
	XSkinMeshReadyInBackground(L"ALH_MOONLIGHT_SLASH_BLADE_FULL_LIGHT.X");
	TextureReadyInBackground(L"ALH_MOONLIGHT_SLASH_MOON.tga");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	TextureReadyInBackground(L"SI_SA_ALH_MOONLIGHT_SLASH_MOON.tga");
	TextureReadyInBackground(L"star2.dds");
	TextureReadyInBackground(L"Arme_Damage_Ring.dds");
	TextureReadyInBackground(L"Arme_Damage_Center.dds");
	TextureReadyInBackground(L"Arme_Critical.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"ASD_1000_slash_01.tga");
	XSkinMeshReadyInBackground(L"elsword_SA_SwordBlastiong_slash.X");
}
void CX2GUAra::ASI_HA_ASD_THOUSANDS_BLOSSOMS_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//#pragma region 2-1, 2-2 Combo State

//#pragma region ComboZZXX
/** @function	: ASI_COMBO_ALD_ZZXX_FrameMove
	@brief		: ��ź ������ ����Ʈ�� ���� ����
*/
void CX2GUAra::ASI_COMBO_ALD_ZZXX_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.24f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.39f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.67f ) == true )
	{
		CreateEnergeBullet( -37.f );
	}

	CommonFrameMove();
}

/** @function	: ASI_COMBO_ALD_ZZXX_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_COMBO_ALD_ZZXX_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 101

//#pragma region ComboZZZFrontZ
/** @function	: ASI_COMBO_ALD_ZZZFrontZ_FrameMove
	@brief		: ��ź ������ ����Ʈ�� ���� ����
*/
void CX2GUAra::ASI_COMBO_ALD_ZZZFrontZ_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.34f ) )
		CreateEnergeBullet();

	CommonFrameMove();
}

/** @function	: ASI_COMBO_ALD_ZZZFrontZ_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_COMBO_ALD_ZZZFrontZ_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 102

//#pragma region ComboDashZX
/** @function	: ASI_DASH_JUMP_COMBO_ALD_ZX_FrameMove
	@brief		: ��ź ������ ����Ʈ�� ���� ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_ALD_ZX_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.27f ) )
		CreateEnergeBullet( -45.f );

	CommonFrameMove();
}

/** @function	: ASI_DASH_JUMP_COMBO_ALD_ZX_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_ALD_ZX_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	ELSE_IF_STATE_CHANGE_ON_EX_( 0, m_fEventTime[0].keyInputStart, m_fEventTime[0].keyInputEnd, m_fEventTime[0].stateChange,
		m_InputData.oneX == true && ( CX2Unit::UC_ARA_LITTLE_DEVIL == GetUnitClass() || CX2Unit::UC_ARA_YAMA_RAJA == GetUnitClass() ),
		ASI_DASH_JUMP_COMBO_ALD_ZXX )

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 103

//#pragma region ComboDashZXX
/** @function	: ASI_DASH_JUMP_COMBO_ALD_ZXX_FrameMove
	@brief		: ��ź ������ ����Ʈ�� ���� ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_ALD_ZXX_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.24f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.39f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.67f ) == true )
	{
		CreateEnergeBullet( -45.f );
	}

	CommonFrameMove();
}

/** @function	: ASI_DASH_JUMP_COMBO_ALD_ZXX_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_ALD_ZXX_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 104

//#pragma region ComboDashJumpXZ
/** @function	: ASI_DASH_JUMP_COMBO_AYR_XZ_StateStartFuture
	@brief		: ���� ��ȯ ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_AYR_XZ_StateStartFuture()
{
	CommonStateStartFuture();

	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}

/** @function	: ASI_DASH_JUMP_COMBO_AYR_XZ_FrameMove
	@brief		: ��ź ������ ����Ʈ�� ���� ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_AYR_XZ_FrameMove()
{
	if ( m_pXSkinAnim->EventTimerOneshot( 0.24f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.39f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.67f ) == true )
	{
		CreateEnergeBullet( -45.f );
	}

	CommonFrameMove();
}

/** @function	: ASI_DASH_JUMP_COMBO_AYR_XZ_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_DASH_JUMP_COMBO_AYR_XZ_EventProcess()
{
	if ( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 105

//#pragma region ComboZZX
/** @function	: ASI_DASH_COMBO_AYR_ZZ_FrameMove
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_DASH_COMBO_AYR_ZZ_FrameMove()
{
	if( m_pXSkinAnim->EventTimerOneshot( 0.186f ) == true ||
		m_pXSkinAnim->EventTimerOneshot( 0.326f ) == true )
	{
		CreateEnergeBullet( 0.f, true );	/// �����տ��� �߻�
	}
    else if ( m_pXSkinAnim->EventTimerOneshot( 0.263f) )
	{
		CreateEnergeBullet( 0.f, false );	/// �޼տ��� �߻�
	}

	CommonFrameMove();
}

/** @function	: ASI_DASH_COMBO_AYR_ZZ_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_DASH_COMBO_AYR_ZZ_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		StateChange( USI_WAIT );
	}
	else if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion 106

//#pragma endregion 2-1, 2-2 Ŀ�ǵ� ��ų


//#pragma region 2-1, 2-2 Active

//#pragma region SI_A_ALD_WOLF_CLAW
/** @function	: ASI_A_ALD_WOLF_CLAW_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ALD_WOLF_CLAW", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_WOLF_CLAW_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_WOLF_CLAW_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_WOLF_CLAW_EventProcess );
}

/** @function	: ASI_A_ALD_WOLF_CLAW_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_Init()
{
	TextureReadyInBackground(L"Ara_Orange_LEAF.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"Steam_BP.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"state_potion_flare.DDS");
	TextureReadyInBackground(L"stone.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Ran_flame02.tga");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Impact05.dds");

	XSkinMeshReadyInBackground(L"A_ALD_WOLF_CLAW_START_TRACE.X");
	XSkinMeshReadyInBackground(L"A_ALD_WOLF_CLAW_START_Projecticle.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

/** @function	: ASI_A_ALD_WOLF_CLAW_StateStart
	@brief		: ��� ��ȯ ����
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_A_ALD_WOLF_CLAW_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_A_ALD_WOLF_CLAW_FINISH );
	}

	/// ��� ĵ�� �˻�
	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function	: ASI_A_ALD_WOLF_CLAW_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ALD_WOLF_CLAW_FINISH", stateData_.stateID );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_WOLF_CLAW_FINISH_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_WOLF_CLAW_FINISH_EventProcess );
}

/** @function	: ASI_A_ALD_WOLF_CLAW_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_FINISH_Init()
{
	TextureReadyInBackground(L"Ara_Black_LEAF.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.DDS");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"soket_earth_quake_stone01.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Ran_flame02.tga");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Impact05.dds");

	XSkinMeshReadyInBackground(L"A_ALD_WOLF_CLAW_END_TRACE.X");
	XSkinMeshReadyInBackground(L"A_ALD_WOLF_CLAW_END_Projecticle.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

/** @function	: ASI_A_ALD_WOLF_CLAW_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_ALD_WOLF_CLAW_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� 3�� - �������

//#pragma region SI_A_ALD_CONVERSION_HEALTH
/** @function	: ASI_A_ALD_WOLF_CLAW_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_HEALTH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ALD_CONVERSION_HEALTH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_CONVERSION_HEALTH_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_CONVERSION_HEALTH_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_CONVERSION_HEALTH_EventProcess );
}

/** @function	: ASI_A_ALD_CONVERSION_HEALTH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_HEALTH_Init()
{
	TextureReadyInBackground(L"A_ALD_CONVERSION_HEALTH_MAP02.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"aisha_active_guillotinePress_circleParticle.dds");

	XSkinMeshReadyInBackground(L"Elsword_SI_A_RS_Sword_Enchant_Mesh01.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}

/** @function	: ASI_A_ALD_CONVERSION_HEALTH_FrameMove
	@brief		: HP�� ���� ��� ȹ�� ó���� �ϴ� �Լ�
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_HEALTH_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		if ( NULL != g_pData->GetSkillTree() &&
			 NULL != GetUnit() )
		{
			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;					/// ���� ��ų Ʈ��

			const int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ALD_CONVERSION_HEALTH );	/// ������ ��ų ����

			/// ������ ��ų ���ø�
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_ALD_CONVERSION_HEALTH );

			if ( NULL != pSkillTemplet )
			{
				/// �߰��� ��·�
				const int	iAddForcePower	= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillLevel ) );
				/// �Ҹ�� HP ���
				const float fHPRate			= GetMaxHp() * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_HP_REL_TO_MP_ABS_1, iSkillLevel );

				SetNowHp( GetNowHp() - fHPRate );		/// HP ���� ó��

				SetForcePower( GetForcePower() + iAddForcePower );	/// ��� ����
			}
		}
	}

	CommonFrameMove();
}

/** @function	: ASI_A_ALD_CONVERSION_HEALTH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_HEALTH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ������

//#pragma region SI_A_ALD_CONVERSION_MANA
/** @function	: ASI_A_ALD_CONVERSION_MANA_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_MANA_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_ALD_CONVERSION_MANA", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_CONVERSION_MANA_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_CONVERSION_MANA_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_ALD_CONVERSION_MANA_EventProcess );
}

/** @function	: ASI_A_ALD_CONVERSION_MANA_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_MANA_Init()
{
	TextureReadyInBackground(L"CDC_AS_FIRE_LIGHT.tga");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Smoke.dds");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"aisha_active_guillotinePress_circleParticle.dds");

	XSkinMeshReadyInBackground(L"Elsword_SI_A_RS_Sword_Enchant_Mesh01.X");
	XSkinMeshReadyInBackground(L"Mega_Slash_Trace_Dummy.x");
}

/** @function	: ASI_A_ALD_CONVERSION_MANA_FrameMove
	@brief		: MP�� ���� ��� ȹ�� ó���� �ϴ� �Լ�
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_MANA_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		if ( NULL != g_pData->GetSkillTree() &&
			 NULL != GetUnit() )
		{
			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;					/// ���� ��ų Ʈ��

			const int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ALD_CONVERSION_MANA );	/// ������ ��ų ����

			/// �̱�� ��ų ���ø�
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_ALD_CONVERSION_MANA );

			if ( NULL != pSkillTemplet )
			{
				/// �߰��� ��·�
				const int	iAddForcePower	= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillLevel ) );
				/// �Ҹ�� MP ���
				const float fMPRate = GetMaxMp() * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevel );

				SetNowMp( GetNowMp() - fMPRate );		/// MP ���� ó��

				SetForcePower( GetForcePower() + iAddForcePower );	/// ��� ����
			}
		}
	}

	CommonFrameMove();
}

/** @function	: ASI_A_ALD_CONVERSION_MANA_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_ALD_CONVERSION_MANA_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion �̱��

//#pragma region SI_A_AYR_HIGH_SPEED
/** @function	: ASI_A_AYR_HIGH_SPEED_READY_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_READY_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID = iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_AYR_HIGH_SPEED_READY", stateData_.stateID );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_HIGH_SPEED_READY_EventProcess );
}

/** @function	: ASI_A_AYR_HIGH_SPEED_READY_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_READY_EventProcess()
{
	if ( 0.1f <= m_FrameDataFuture.unitCondition.fStateTime )		/// ��� ������ ���� ������ ���ٸ�, �����ִ� �������� ����
	{
			StateChange( ASI_A_AYR_HIGH_SPEED );
	}
	else if ( m_InputData.oneRight)		/// �������� ������ ��
	{
		m_FrameDataFuture.syncData.bIsRight	= true;		/// ���� ����

		if ( false ==GetIsRight() )		/// ���� �ݴ� ������ ���� �־��ٸ�, 180�� ȸ��
			m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;

		StateChange( ASI_A_AYR_HIGH_SPEED );
	}
	else if ( m_InputData.oneLeft)		/// ������ ������ ��
	{
		m_FrameDataFuture.syncData.bIsRight	= false;	/// ���� ����

		if ( true ==GetIsRight() )		/// ���� �ݴ� ������ ���� �־��ٸ�, 180�� ȸ��
			m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;

		StateChange( ASI_A_AYR_HIGH_SPEED );
	}

	CommonEventProcess();
}

/** @function	: ASI_A_AYR_HIGH_SPEED_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_AYR_HIGH_SPEED", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_HIGH_SPEED_Init );
	stateData_.StateStartFuture	= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_HIGH_SPEED_StateStartFuture );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_HIGH_SPEED_EventProcess );
	stateData_.StateEndFuture	= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_HIGH_SPEED_StateEndFuture );
}

/** @function	: ASI_A_AYR_HIGH_SPEED_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
}

/** @function	: SI_A_AYR_HIGH_SPEED_READY_StateStartFuture
	@brief		: ���� �ӵ� ����
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_StateStartFuture()
{
	CommonStateStartFuture();

	if ( NULL != g_pData &&
		 NULL != g_pData->GetSkillTree() &&
		 NULL != GetUnit() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_AYR_HIGH_SPEED );

		if ( NULL != pSkillTemplet )
		{
			const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AYR_HIGH_SPEED );

			/// �ӵ� ������ ����
			m_fHighSpeedValue = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_REMOTE_SPEED_ABS, iSkillLevel );

			if ( 0 >= m_fHighSpeedValue )
				m_fHighSpeedValue = 1.f;

			m_PhysicParam.passiveSpeed.x *= m_fHighSpeedValue;
		}
	}
}

/** @function	: ASI_A_AYR_HIGH_SPEED_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_EventProcess()
{
	if ( m_fEventTime[0].keyInputStart  <= m_FrameDataFuture.unitCondition.fStateTime )
	{
		if ( false == IsOnSomethingFuture() )
			StateChange( ASI_AIRIAL_DASH );
		else
			StateChange( USI_DASH );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/** @function	: ASI_A_AYR_HIGH_SPEED_StateEndFuture
	@brief		: ���� ���� ����
*/
void CX2GUAra::ASI_A_AYR_HIGH_SPEED_StateEndFuture()
{
	CommonStateEndFuture();

	m_PhysicParam.passiveSpeed.x	= 0.f;
	m_PhysicParam.nowSpeed.x		= 1200.f;
}
//#pragma endregion ���

//#pragma endregion 2-1, 2-2 ��Ƽ�� ��ų


//#pragma region 2-1, 2-2 Special Active

//#pragma region SI_SA_ALD_ENERGY_WAVE
/** @function	: ASI_SA_ALD_ENERGY_WAVE_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_WAVE_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_ENERGY_WAVE", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_WAVE_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_WAVE_EventProcess );
}

/** @function	: ASI_SA_ALD_ENERGY_WAVE_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_WAVE_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP03.dds");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP04.dds");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP06.dds");
	TextureReadyInBackground(L"SA_SPK_SWORDFIRE_WAVE.dds");
	TextureReadyInBackground(L"SA_SPK_SWORDFIRE_WAVE2.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");
	TextureReadyInBackground(L"Steam_Bp.dds");
	TextureReadyInBackground(L"Lire_Kick_Impact01.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.DDS");
	TextureReadyInBackground(L"SpreadLight01.dds");

	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
}

/** @function	: ASI_SA_ALD_ENERGY_WAVE_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_WAVE_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;

		StateChange( USI_JUMP_DOWN );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}
//#pragma endregion �����

//#pragma region SI_SA_ALD_HELL_OF_TORNADO
/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_HELL_OF_TORNADO", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_HELL_OF_TORNADO_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_HELL_OF_TORNADO_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_HELL_OF_TORNADO_EventProcess );
}

/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_StateStart
	@brief		: ��� ��ȯ ����
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Lire_Kick_Impact01.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.DDS");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP03.dds");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP04.dds");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP06.dds");
	TextureReadyInBackground(L"SA_SPK_SWORDFIRE_WAVE.dds");
	TextureReadyInBackground(L"SA_SPK_SWORDFIRE_WAVE2.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");
	TextureReadyInBackground(L"Steam_Bp.dds");

	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
}

/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_SA_ALD_HELL_OF_TORNADO_FINISH );
	}

	/// ��� ĵ�� �˻�
	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_FINISH_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_HELL_OF_TORNADO_FINISH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_HELL_OF_TORNADO_FINISH_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_HELL_OF_TORNADO_FINISH_EventProcess );
}

/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_FINISH_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Lire_Kick_Impact01.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.DDS");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP03.dds");
	TextureReadyInBackground(L"SA_AHD_ENERGY_WAVE_PROJECTILE_MAP04.dds");
	TextureReadyInBackground(L"SA_SPK_SWORDFIRE_WAVE2.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");

	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
}

/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_FINISH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_HELL_OF_TORNADO_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
			StateChange( USI_JUMP_DOWN );
		}
		else
			StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ��ȯ����

//#pragma region SI_SA_ALD_WOLF_SPEAR
/** @function	: ASI_SA_ALD_WOLF_SPEAR_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_WOLF_SPEAR", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_SPEAR_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_SPEAR_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_SPEAR_EventProcess );
}

/** @function	: ASI_SA_ALD_WOLF_SPEAR_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_Init()
{
	XSkinMeshReadyInBackground(L"ALD_SPEAR_Weapon.X");
}

/** @function	: ASI_SA_ALD_WOLF_SPEAR_StateStart
	@brief		: ��� ��ȯ ����
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_SA_ALD_WOLF_SPEAR_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_SA_ALD_WOLF_SPEAR_FINISH );
	}

	/// ��� ĵ�� �˻�
	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	CommonEventProcess();
}

/** @function	: ASI_SA_ALD_WOLF_SPEAR_FINISH_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_WOLF_SPEAR_FINISH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_SPEAR_FINISH_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_SPEAR_FINISH_EventProcess );
}

/** @function	: ASI_SA_ALD_WOLF_SPEAR_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_FINISH_Init()
{
	XSkinMeshReadyInBackground(L"ALD_SPEAR_Weapon.X");

}
/** @function	: ASI_SA_ALD_HELL_OF_TORNADO_FINISH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_WOLF_SPEAR_FINISH_EventProcess()
{
	CommonEventProcess();

	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
			StateChange( USI_JUMP_DOWN );
		}
		else
			StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
}
//#pragma endregion ���� 4�� - �����̻�

//#pragma region SI_SA_ALD_WOLF_SPEAR
/** @function	: ASI_SA_ALD_FINGER_BULLET_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_FINGER_BULLET", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_FINGER_BULLET_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_FINGER_BULLET_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_FINGER_BULLET_EventProcess );
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_Init()
{
	TextureReadyInBackground(L"Force_Spear_round.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SI_A_ARA_GAIN_FORCE_NASUN_GATE_DEST.dds");
	TextureReadyInBackground(L"Whitecircle02.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Impact05.dds");

	XSkinMeshReadyInBackground(L"Bust_Wolf_M03.X");
	XSkinMeshReadyInBackground(L"SA_ALD_FINGER_BULLET_Attack_Box_START.X");
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_Init
	@brief		: ��ȥ�� ���� Ÿ�ݵ� ���� ���� �ʱ�ȭ
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_StateStart()
{
	CommonStateStart();

	/// ������ ���� Ÿ�ݵ� ��� ���� ��ü �ʱ�ȭ
	m_vecHitUnitPointer.clear();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_SA_ALD_FINGER_BULLET_FINISH );
	}

	/// ��� ĵ�� �˻�
	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_FINISH_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )

{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_FINGER_BULLET_FINISH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_FINGER_BULLET_FINISH_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_FINGER_BULLET_FINISH_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_FINGER_BULLET_FINISH_EventProcess );
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_FINISH_Init()
{
	XSkinMeshReadyInBackground(L"SA_ALD_FINGER_BULLET_Attack_Box_END.X");
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_FINISH_FrameMove
	@brief		: ��ȥ�� �ǰݵ� ���� �Ѿ�߸��� ���� ����
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_FINISH_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( m_fEventTime[0].keyInputStart ) )
	{
		for( CX2GameUnitoPtr pGameUnit : m_vecHitUnitPointer )
		{
			/// ���� ���� ������ ��
			if ( null != pGameUnit && 0 < pGameUnit->GetNowHp() )
			{
				/// �浹 ó���� �����ϱ� ������, �ٿ� �������� ���� ����
				m_DamageData.reActResult = CX2DamageManager::RT_DOWN;

				/// ����� ���� �߰�
				m_DamageData.optrDefenderGameUnit = pGameUnit;

				/// ������ ������ ����
				pGameUnit->DamageReact( &m_DamageData );

				/// �̵� �Ұ� ���� ����
				EraseBuffTempletFromGameUnit( BTI_DEBUFF_NEVER_MOVE );
			}
		}

		m_vecHitUnitPointer.clear();
	}

	CommonFrameMove();
}

/** @function	: ASI_SA_ALD_FINGER_BULLET_FINISH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_FINGER_BULLET_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� 1�� - ��ȥ

//#pragma region SI_SA_ALD_WOLF_SPEAR
/** @function	: ASI_SA_ALD_ENERGY_BEAD_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_ENERGY_BEAD", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_BEAD_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_BEAD_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_BEAD_EventProcess );
}

/** @function	: ASI_SA_ALD_ENERGY_BEAD_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_Init()
{
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	TextureReadyInBackground(L"secretVelder_Unohound_fire.dds");

	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
}


/** @function	: ASI_SA_ALD_ENERGY_BEAD_StateStart
	@brief		: ��� ��ȯ ����
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_SA_ALD_ENERGY_BEAD_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_SA_ALD_ENERGY_BEAD_FINISH );
	}

	/// ��� ĵ�� �˻�
	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/** @function	: ASI_SA_ALD_ENERGY_BEAD_FINISH_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_ENERGY_BEAD_FINISH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_BEAD_FINISH_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_BEAD_FINISH_EventProcess );
}

/** @function	: ASI_SA_ALD_ENERGY_BEAD_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_FINISH_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.DDS");
	TextureReadyInBackground(L"secretVelder_Unohound_fire.dds");

	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
}

/** @function	: ASI_SA_ALD_ENERGY_BEAD_FINISH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_ENERGY_BEAD_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� 2�� - ��ȯ��

//#pragma region SI_SA_ALD_PRISON_SPEAR
/** @function	: ASI_SA_ALD_PRISON_SPEAR_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_PRISON_SPEAR", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_PRISON_SPEAR_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_PRISON_SPEAR_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_PRISON_SPEAR_EventProcess );
}

/** @function	: ASI_SA_ALD_PRISON_SPEAR_StateStart
	@brief		: ��� ��ȯ ����
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_StateStart()
{
	CommonStateStart();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_SA_ALD_PRISON_SPEAR_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_Init()
{
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_KNOT_Trace.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");
	TextureReadyInBackground(L"secretVelder_alchemystBoss_magicAttackB_thunder.DDS");

	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
	XSkinMeshReadyInBackground(L"ALD_SPEAR_Weapon.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

/** @function	: ASI_SA_ALD_PRISON_SPEAR_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_SA_ALD_PRISON_SPEAR_FINISH );
	}

	/// ��� ĵ�� �˻�
	if( false == m_bFPCancel && m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime()
		&& m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() && true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
}

/** @function	: ASI_SA_ALD_PRISON_SPEAR_FINISH_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_PRISON_SPEAR_FINISH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_PRISON_SPEAR_FINISH_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_ENERGY_BEAD_FINISH_EventProcess );
}

/** @function	: ASI_SA_ALD_PRISON_SPEAR_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_FINISH_Init()
{
	XSkinMeshReadyInBackground(L"ALD_SPEAR_Weapon.X");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");

	TextureReadyInBackground(L"secretVelder_alchemystBoss_magicAttackB_thunder.DDS");
}

/** @function	: ASI_SA_ALD_PRISON_SPEAR_FINISH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_PRISON_SPEAR_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� 3�� - â�차��

//#pragma region SI_A_AYR_STEAL_SOUL
/** @function	: ASI_A_AYR_STEAL_SOUL_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_CreateState( OUT UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_AYR_STEAL_SOUL", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_STEAL_SOUL_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_STEAL_SOUL_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_STEAL_SOUL_EventProcess );
}

/** @function	: ASI_A_AYR_STEAL_SOUL_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_Init()
{
	TextureReadyInBackground(L"Ara_Black_LEAF.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	XSkinMeshReadyInBackground(L"Effect_ElSword_MegaSlashBackDust.X");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	TextureReadyInBackground(L"Whitecircle03.dds");
}

/** @function	: ASI_A_AYR_STEAL_SOUL_StateStart
	@brief		: Żȥ�� ���� Ÿ�ݵ� ���� ���� �ʱ�ȭ
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_StateStart()
{
	CommonStateStart();

	/// ������ ���� Ÿ�ݵ� ��� ���� ��ü �ʱ�ȭ
	m_vecHitUnitPointer.clear();

	m_usBeforeForceGainCount = m_usForceGainCount;
}

/** @function	: ASI_A_AYR_STEAL_SOUL_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( true == GetAvailableFPCancel ( m_eSkillID ) )	/// ��� ĵ�� ����
		{
			UpForcePower( m_usBeforeForceGainCount );
			StateChangeSpecialAttack( m_eSkillID );
		}
		else
			StateChange( ASI_A_AYR_STEAL_SOUL_FINISH );
	}
	/// ��� ĵ�� �˻�
	else  if( false == m_bFPCancel &&
			m_vFPCancel.x <= m_pXSkinAnimFuture->GetNowAnimationTime() &&
			m_vFPCancel.y > m_pXSkinAnimFuture->GetNowAnimationTime() &&
			true == CanUseSpecialAttack() )
	{
		m_bFPCancel = true;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

/** @function	: ASI_A_AYR_STEAL_SOUL_FINISH_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_FINISH_CreateState( OUT UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_A_AYR_STEAL_SOUL_FINISH", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_STEAL_SOUL_FINISH_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_STEAL_SOUL_FINISH_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_A_AYR_STEAL_SOUL_FINISH_EventProcess );
}

/** @function	: ASI_A_AYR_STEAL_SOUL_FINISH_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_FINISH_Init()
{
	XSkinMeshReadyInBackground(L"A_AYR_STEAL_SOUL_SHADOW_E.X");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Ran_flame02.tga");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"elswrod_demolition_aura.dds");
}

/** @function	: ASI_A_AYR_STEAL_SOUL_FINISH_FrameMove
	@brief		: Żȥ�� �ǰݵ� ���� �Ѿ�߸��� ���� ����
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_FINISH_FrameMove()
{
    if( true == m_pXSkinAnim->EventTimerOneshot( 0.577f ) )
	{
		for( CX2GameUnitoPtr pGameUnit : m_vecHitUnitPointer )
		{
			/// ���� ���� ������ ��
			if ( null != pGameUnit && 0 < pGameUnit->GetNowHp() )
			{
				/// �浹 ó���� �����ϱ� ������, �ٿ� �������� ���� ����
				m_DamageData.reActResult = CX2DamageManager::RT_DOWN;

				/// ����� ���� �߰�
				m_DamageData.optrDefenderGameUnit = pGameUnit;

				/// ������ ������ ����
				pGameUnit->DamageReact( &m_DamageData );
			}
		}

		m_vecHitUnitPointer.clear();
	}

	CommonFrameMove();
}

/** @function	: SI_A_AYR_STEAL_SOU_FINISHL_FINISH_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_A_AYR_STEAL_SOUL_FINISH_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� 4�� - Żȥ

//#pragma region SI_SA_AYR_ENERGY_VOID
/** @function	: ASI_SA_AYR_ENERGY_VOID_START_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_AYR_ENERGY_VOID_START", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_START_Init );
	stateData_.StateStartFuture	= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_START_StateStartFuture );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_START_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_START_EventProcess );
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_START_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_START_Init()
{
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"aisha_active_guillotinePress_circleParticle.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_START_StateStartFuture
	@brief		: Ű �Է� ���� ��ü �ʱ�ȭ
*/

void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_START_StateStartFuture()
{
	/// Ű �Է� ���� ��ü �ʱ�ȭ
	m_sCurrentSkillKeyManager.InitCurrentSkillKeyManager( m_iNowSpecialAttack - 1 );

	CommonStateStartFuture();
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_START_StateStart
	@brief		: �������� ��� �غ� ������ ������ ����Ʈ ���� �� ��� �Ҹ�, ���� �ð� ���� �� �ʱ�ȭ �۾�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_START_StateStart()
{
	CommonStateStart();
	m_iEnergeVoidChargeValue	= 0;			/// ���� �ܰ� �ʱ�ȭ
	m_bEnergeVoidChargeEnd		= false;		/// ���� ���� ���� �ʱ�ȭ
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_START_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_START_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
		StateChange( ASI_SA_AYR_ENERGY_VOID_LOOP );

	CommonEventProcess();
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_LOOP_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_LOOP_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_AYR_ENERGY_VOID_LOOP", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_LOOP_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_LOOP_StateStart );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_LOOP_EventProcess );
}

/** @function	: SI_SA_AYR_ENERGY_VOID_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_LOOP_Init()
{
	TextureReadyInBackground(L"Break_Impact01.dds");
	TextureReadyInBackground(L"SI_A_ARA_GAIN_FORCE_NASUN_GATE.dds");
	TextureReadyInBackground(L"SI_A_ARA_GAIN_FORCE_NASUN_GATE_DEST.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Cloud.dds");
	TextureReadyInBackground(L"SpreadLight02.dds");
	TextureReadyInBackground(L"ThunderBall02.dds");
	TextureReadyInBackground(L"ThunderBall03.dds");
	TextureReadyInBackground(L"aisha_SA_MorningStar_circle.dds");


	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

/** @function	: SI_SA_AYR_ENERGY_VOID_FrameMove
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_LOOP_StateStart()
{
	/// ���� 4�ܰ踦 �ʰ� �ϸ鼭 ����� �� �ִ� ��·��� ������ ��
	if ( 3 < m_iEnergeVoidChargeValue &&
		0 >= GetForcePower() )
	{
		m_bEnergeVoidChargeEnd = true;		/// ���� ����

		return;
	}

	/// ����Ʈ ���� ó��
	if ( NULL != g_pX2Game->GetDamageEffect() )
	{
		wstring wstrEffectName = L"";		/// �����ֱ�� ������ ����Ʈ �̸�

		/// �� �ܰ迡 ���� �����ֱ�� ������ ����Ʈ �̸� ����
		switch ( m_iEnergeVoidChargeValue )
		{
		case 0: wstrEffectName = L"DamageEffect_AYR_Energy_Void_Effect_1st"; break;		/// 1�ܰ� ����Ʈ ����
		case 4: wstrEffectName = L"DamageEffect_AYR_Energy_Void_Effect_2nd"; break;		/// 2�ܰ� ����Ʈ ����
		case 7: wstrEffectName = L"DamageEffect_AYR_Energy_Void_Effect_3rd"; break;		/// 3�ܰ� ����Ʈ ����
		}

		/// ���� ��ġ ����
		D3DXVECTOR3 vPos = GetIsRight() ? GetPos() + ( GetDirVector() * 500.f ) : GetPos() + ( GetDirVector() * -500.f );

		vPos.y += 100.f;

		/// �����ֱ�� ������ ����Ʈ ����
		if ( false == wstrEffectName.empty() )
		{
			/// ���� ����ִ� ����Ʈ�� �ִٸ�, ����
            if ( m_hEnergeVoidDamageEffect != INVALID_DAMAGE_EFFECT_HANDLE )
                g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEnergeVoidDamageEffect );

			/// ������ ����Ʈ ���� ( �����ֱ�� )
            m_hEnergeVoidDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
                this, wstrEffectName.c_str(),
				GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );
		}

		/// ���� ������ ( ���ӵǴ� Ÿ���� ����, ���� �ѹ� �� ��ŭ ����ִ� ������ ����Ʈ ���� )
		if ( 3 < m_iEnergeVoidChargeValue )		/// ����� �Һ��� �߰� Ÿ��
		{
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AYR_Energy_Void_Attack",
				GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );

			UseForcePower( 1 );	/// ��� �Һ�
		}
		else									/// MP�� �Һ��� �⺻ Ÿ��
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_AYR_Energy_Void_Default_Attack",
			GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(),
			m_FrameDataNow.unitCondition.landPosition.y );
	}

	++ m_iEnergeVoidChargeValue;		/// ������ ����

	CommonStateStart();
}

/** @function	: SI_SA_AYR_ENERGY_VOID_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_LOOP_EventProcess()
{
	/// Ű �Է� ���� ��ü���� ������ ���� �Ǿ� �ִ��� �˻�
	m_sCurrentSkillKeyManager.CheckSkillSlotSwap();

	/// ����� �ݺ� ������ �Ѿ��� ��
	if ( 0.4f <= m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		/// ���� 4�ܰ� �̻��̰� Ű �Է��� ���� �Ͽ��ų�, �ִ� �ݺ� Ƚ���� ���޵Ǿ��ٸ� ������Ʈ�� ��ȯ
		if ( ( 3 < m_iEnergeVoidChargeValue &&
			   true == m_sCurrentSkillKeyManager.IsUnpressedCurrentSkillKey( m_InputData ) ) ||
			 _CONST_ARA_::ENERGY_VOID_MAX_CHARGE_VALUE <= m_iEnergeVoidChargeValue )
		{
			StateChange( ASI_SA_AYR_ENERGY_VOID_END );
		}
		else
		{
			StateChange( ASI_SA_AYR_ENERGY_VOID_LOOP );
		}
	}
	/// ���� ���� �����Ǹ�, ������ ������Ʈ�� ��ȯ
	else if ( true == m_bEnergeVoidChargeEnd )
	{
		StateChange( ASI_SA_AYR_ENERGY_VOID_END );
	}

	CommonEventProcess();
}



/** @function	: SI_SA_AYR_ENERGY_VOID_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_AYR_ENERGY_VOID_END", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_END_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_END_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_END_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_ENERGY_VOID_END_EventProcess );
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_END_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_END_Init()
{
	TextureReadyInBackground(L"ASD_1000_slash_01.tga");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SA_AYR_ENERGY_CANNON_Map01.dds");
	TextureReadyInBackground(L"SpreadLight01.dds");

	XSkinMeshReadyInBackground(L"Bust_Wolf_M03.X");
	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}
/** @function	: ASI_SA_AYR_ENERGY_VOID_END_StateStart
	@brief		: ����ִ� ���� ����Ʈ�� �ִٸ�, ����
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_END_StateStart()
{
	CommonStateStart();

	/// ���� ����ִ� ����Ʈ�� �ִٸ�, ����
    if ( m_hEnergeVoidDamageEffect != INVALID_DAMAGE_EFFECT_HANDLE )
        g_pX2Game->GetDamageEffect()->DestroyInstanceHandle( m_hEnergeVoidDamageEffect );
}
/** @function	: ASI_SA_AYR_ENERGY_VOID_END_FrameMove
	@brief		: ���� ����Ʈ ����
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_END_FrameMove()
{
if( true == m_pXSkinAnim->EventTimerOneshot( 0.08f ) &&
		NULL != g_pData->GetSkillTree() )
	{
		/// �������� ��ų ���ø� ��ȯ
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_SA_AYR_ENERGY_VOID );

		if ( NULL != pSkillTemplet &&
			 NULL != g_pX2Game->GetEffectSet() &&
			 NULL != GetUnit() )
		{
			const CX2UserSkillTree&		userSkillTree	= GetUnit()->GetUnitData().m_UserSkillTree;		/// ���� ��ų Ʈ��
			const int					iSkillLevel		= userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_AYR_ENERGY_VOID );	/// ��ų ����

			/// �⺻ ������ ������ ���� �ܰ踸ŭ �����ش�. ( ���� �ܰ� * ���� �ܰ躰 ������ ���� )
			const float fChargePowerRate = max( m_iEnergeVoidChargeValue * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DAMAGED_REL, iSkillLevel ), 1.f );

			/// ���� ����Ʈ ����
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_SI_SA_AHD_ENERGY_VOID_FX_Hit", this, NULL, IsHyperState(),
				GetPowerRate() * fChargePowerRate );
		}
	}

	CommonFrameMove();
}

/** @function	: ASI_SA_AYR_ENERGY_VOID_END_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_AYR_ENERGY_VOID_END_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}

//#pragma endregion ��������

//#pragma region SI_SA_AYR_SUPPRESSION_ENERGY
/** @function	: ASI_SA_AYR_SUPPRESSION_ENERGY_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_AYR_SUPPRESSION_ENERGY_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_AYR_SUPPRESSION_ENERGY", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_SUPPRESSION_ENERGY_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_SUPPRESSION_ENERGY_EventProcess );
}

/** @function	: ASI_SA_AYR_SUPPRESSION_ENERGY_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_AYR_SUPPRESSION_ENERGY_Init()
{
	TextureReadyInBackground(L"ASD_1000_slash_01.tga");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Force_Spear_round.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Light_Impact01.dds");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"PET_GOLDFOX_MIHOEffect02.tga");
	TextureReadyInBackground(L"RVC_SA_DeadlyRaid_line.dds");
	TextureReadyInBackground(L"SA_AHD_SUPPRESSION_ENERGY_P01.dds");
	TextureReadyInBackground(L"SA_AHD_SUPPRESSION_ENERGY_P08.tga");
	TextureReadyInBackground(L"SA_AYR_ENERGY_CANNON_Map03.dds");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");
	TextureReadyInBackground(L"SpreadLight01.dds");
	TextureReadyInBackground(L"secretVelder_alchemystBoss_magicAttackE_wave.dds");

	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
}

/** @function	: ASI_SA_AYR_SUPPRESSION_ENERGY_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_AYR_SUPPRESSION_ENERGY_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���� - ��

//#pragma region SI_SA_ALD_WOLF_BREAK
/** @function	: ASI_SA_ALD_WOLF_BREAK_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_ALD_WOLF_BREAK_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_ALD_WOLF_BREAK", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_BREAK_Init );
	stateData_.StateStart		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_BREAK_StateStart );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_BREAK_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_ALD_WOLF_BREAK_EventProcess );
}

/** @function	: ASI_SA_ALD_WOLF_BREAK_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_ALD_WOLF_BREAK_Init()
{
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Mesh_Elsword_Event_AC_Upbody21A_Effect.tga");
	TextureReadyInBackground(L"SI_SA_ARA_SHADOW_RISING_Hall.dds");
	TextureReadyInBackground(L"WhitePoint.dds");

	XSkinMeshReadyInBackground(L"Ald_Wolf_Break_Dummy.X");
}

/** @function	: ASI_SA_ALD_WOLF_BREAK_StateStart
	@brief		: �߻�ü ���� ī��Ʈ �ʱ�ȭ
*/
void CX2GUAra::ASI_SA_ALD_WOLF_BREAK_StateStart()
{
	CommonStateStart();

	m_iWolfBreakeCount = 0;
}

/** @function	: ASI_SA_ALD_WOLF_BREAK_FrameMove
	@brief		: �߻�ü ����
*/
void CX2GUAra::ASI_SA_ALD_WOLF_BREAK_FrameMove()
{
	CommonFrameMove();

	if ( m_pXSkinAnim->EventTimerOneshot( 1.1f ) ||
		 m_pXSkinAnim->EventTimerOneshot( 1.35f ) ||
		 m_pXSkinAnim->EventTimerOneshot( 1.63f ) ||
		 m_pXSkinAnim->EventTimerOneshot( 2.f ) )
	{
		wstring wstrEffectName = L"DamageEffect_SA_ALD_WOLF_Break";		/// �߻�ü �̸� ����
		float fPowerRate = 1.0f;

		/// �߻� Ƚ���� ���� �߻�ü �̸� ����
		switch ( m_iWolfBreakeCount )
		{
		case 1:
			{
				wstrEffectName = L"DamageEffect_SA_ALD_WOLF_Break2";
			} break;
		case 3:
			{
				wstrEffectName = L"DamageEffect_SA_ALD_WOLF_Break3";
			} break;
		}

		/// ���࿡ �нú� ������ ���¢���� �������̶��, Ư�� Ȯ���� Ŀ�ٶ� ���븦 �߻��Ѵ�.
		if ( m_fHowlingActiveRate > GetRandomFloat() )
		{
			wstrEffectName += L"_Special";
			fPowerRate = m_fHowlingDamageRel;	/// Ŀ�ٶ� ���뿡�� �нú꿡 ���� ������ ����
		}

		/// ����Ʈ ����
		if ( false == wstrEffectName.empty() )
		{
			/// ���� ��ġ ����
			D3DXVECTOR3 vPos = GetIsRight() ? GetBonePos( L"Bip01" ) + ( GetDirVector() * 100.f ) :
											  GetBonePos( L"Bip01" ) + ( GetDirVector() * -100.f );

			/// ������ ����Ʈ ����
			g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrEffectName.c_str(),
				GetPowerRate() * fPowerRate, vPos, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y );
		}

		++ m_iWolfBreakeCount;		/// ī��Ʈ ����
	}
}

/** @function	: ASI_SA_ALD_WOLF_BREAK_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_ALD_WOLF_BREAK_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
//#pragma endregion ���� �������� - �ؼ�

//#pragma region SI_SA_AYR_OPEN_THE_HELL
/** @function	: ASI_SA_AYR_OPEN_THE_HELL_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_SA_AYR_OPEN_THE_HELL_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_SA_AYR_OPEN_THE_HELL", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_OPEN_THE_HELL_Init );
	stateData_.OnFrameMove		= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_OPEN_THE_HELL_FrameMove );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_SA_AYR_OPEN_THE_HELL_EventProcess );
}

/** @function	: ASI_SA_AYR_OPEN_THE_HELL_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_SA_AYR_OPEN_THE_HELL_Init()
{
	TextureReadyInBackground(L"Ara_Black_LEAF.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"SA_AYR_ENERGY_CANNON_Map02.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"aisha_active_guillotinePress_circleParticle.dds");

	XSkinMeshReadyInBackground(L"SA_AYR_OPEN_THE_HELL_BADAK.X");
	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
}

/** @function	: ASI_SA_AYR_OPEN_THE_HELL_FrameMove
	@brief		: �нú� ���� �ʷ� �����, ����� �����ϴ� ������ ����Ʈ ����
*/
void CX2GUAra::ASI_SA_AYR_OPEN_THE_HELL_FrameMove()
{
	CommonFrameMove();

    if( true == m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		if ( NULL != g_pX2Game->GetEffectSet() )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"SA_AYR_OPEN_THE_HELL_ALL", this, NULL, IsHyperState(), GetPowerRate() * m_fOpenTheHellIncreaseDamage );
		}
	}
}

/** @function	: ASI_SA_AYR_OPEN_THE_HELL_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_SA_AYR_OPEN_THE_HELL_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );

	}

	CommonEventProcess();
}
//#pragma endregion ���� �������� - ����

//#pragma region SI_HA_AYR_ENERGY_CANNON
/** @function	: ASI_HA_AYR_ENERGY_CANNON_CreateState
	@brief		: �ش� ��ų ���̵� ���� ������Ʈ ���� ����
	@param		: State ������, State ���̵�
*/
void CX2GUAra::ASI_HA_AYR_ENERGY_CANNON_CreateState( UserUnitStateData& stateData_, const int iStateId_ )
{
	stateData_.Init();
	stateData_.stateID			= iStateId_;
	m_LuaManager.MakeTableReference( "ASI_HA_AYR_ENERGY_CANNON", stateData_.stateID );
	stateData_.StateInit		= SET_CB_FUNC( CX2GUAra, ASI_HA_AYR_ENERGY_CANNON_Init );
	stateData_.OnEventProcess	= SET_CB_FUNC( CX2GUAra, ASI_HA_AYR_ENERGY_CANNON_EventProcess );
}

/** @function	: ASI_HA_AYR_ENERGY_CANNON_Init
	@brief		: �ʿ��� ���ҽ� ���ε�
*/
void CX2GUAra::ASI_HA_AYR_ENERGY_CANNON_Init()
{
	TextureReadyInBackground(L"Ara_Orange_LEAF.dds");
	TextureReadyInBackground(L"Explosion_Fire01.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"GroundShockWave.dds");
	TextureReadyInBackground(L"RTW_Flame_Buster.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"stone_AirShip.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"SA_AYR_ENERGY_CANNON_Map04.dds");
	TextureReadyInBackground(L"SI_SA_ALH_MOONLIGHT_SLASH_BLADE01.tga");

	XSkinMeshReadyInBackground(L"DummyAttackBox_50x50x50.X");
	XSkinMeshReadyInBackground(L"SA_AYR_ORB_KI_BALL.X");
	XSkinMeshReadyInBackground(L"SA_ENERGY_CANNON_DUMMY.X");
}

/** @function	: ASI_HA_AYR_ENERGY_CANNON_EventProcess
	@brief		: ���º��� ���θ� �Ǵ�, ����
*/
void CX2GUAra::ASI_HA_AYR_ENERGY_CANNON_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * _CONST_ARA_::MAGIC_NUMBER_MULTIPLY_LINE_RADIUS;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
//#pragma endregion ���ȯ����

//#pragma endregion 2-1, 2-2 ����� ��Ƽ�� ��ų




void CX2GUAra::InitAquiredActiveSkillState( const int iSkillID_ )
{
	if ( NULL != GetUnit() )
	{
		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
		const CX2SkillTree::SkillTemplet* pSkillTemplet =
			userSkillTree.GetUserSkillTemplet( static_cast<CX2SkillTree::SKILL_ID>( iSkillID_ ) );
		if ( NULL != pSkillTemplet )
		{
			switch ( iSkillID_ )
			{
			case CX2SkillTree::SI_A_ALH_ROCK_CRASH:
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ALH_ROCK_CRASH_START];
						ASI_A_ALH_ROCK_CRASH_FALLING_START_CreateState( stateData, ASI_A_ALH_ROCK_CRASH_START );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ALH_ROCK_CRASH_END];
						ASI_A_ALH_ROCK_CRASH_END_CreateState( stateData, ASI_A_ALH_ROCK_CRASH_END );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

				case CX2SkillTree::SI_A_AM_TIGER_CLAW:
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ARA_TIGER_CLAW_START];
						ASI_A_ARA_TIGER_CLAW_START_CreateState( stateData, ASI_A_ARA_TIGER_CLAW_START );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ARA_TIGER_CLAW_END];
						ASI_A_ARA_TIGER_CLAW_END_CreateState( stateData, ASI_A_ARA_TIGER_CLAW_END );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_A_ASD_LOW_BRANDISH:
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ASD_LOW_BRANDISH_SLICE];
						ASI_A_ASD_LOW_BRANDISH_SLIDING_CreateState( stateData, ASI_A_ASD_LOW_BRANDISH_SLICE );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ASD_LOW_BRANDISH_KICK];
						ASI_A_ASD_LOW_BRANDISH_KICK_CreateState( stateData, ASI_A_ASD_LOW_BRANDISH_KICK );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_A_ASD_TURBULENT_WAVE: // ��ȣ 4�� - �뵵
				{
					//// ���Ǹ� �ߵ� ��Ű�� ������Ʈ�� ��쿡�� ��� ������Ʈ�� stateData.m_ptrComboAndSkillChainInfo ������ �ؾ� ��
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ASD_TURBULENT_WAVE_START];
						ASI_SA_ASD_TURBULENT_WAVE_START_CreateState( stateData, ASI_SA_ASD_TURBULENT_WAVE_START );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ASD_TURBULENT_WAVE_LOOP];
						ASI_SA_ASD_TURBULENT_WAVE_LOOP_CreateState( stateData, ASI_SA_ASD_TURBULENT_WAVE_LOOP );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ASD_TURBULENT_WAVE_END];
						ASI_SA_ASD_TURBULENT_WAVE_END_CreateState( stateData, ASI_SA_ASD_TURBULENT_WAVE_END );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ASD_VOID_BREAK];
						ASI_SA_ASD_VOID_BREAK_CreateState( stateData, ASI_SA_ASD_VOID_BREAK );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

				case CX2SkillTree::SI_SA_AM_PULLING_PIERCE:
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ARA_PULLING_PIERCE_START];
						ASI_SA_ARA_PULLING_PIERCE_START_CreateState( stateData, ASI_SA_ARA_PULLING_PIERCE_START );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ARA_PULLING_PIERCE_END];
						ASI_SA_ARA_PULLING_PIERCE_END_CreateState( stateData, ASI_SA_ARA_PULLING_PIERCE_END );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}

				} break;

				case CX2SkillTree::SI_A_ALH_DOUBLE_COLLISION:
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_DOUBLE_COLLISION_START];
						ASI_SA_ALH_DOUBLE_COLLISION_START_CreateState( stateData, ASI_SA_ALH_DOUBLE_COLLISION_START );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_DOUBLE_COLLISION_END];
						ASI_SA_ALH_DOUBLE_COLLISION_END_CreateState( stateData, ASI_SA_ALH_DOUBLE_COLLISION_END );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_SA_ALH_MOONLIGHT_SLASH:
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_MOONLIGHT_SLASH_READY];
						ASI_SA_ALH_MOONLIGHT_SLASH_READY_CreateState( stateData, ASI_SA_ALH_MOONLIGHT_SLASH_READY);
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE];
						ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_CreateState( stateData, ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK];
						ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_CreateState( stateData, ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK];
						ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_CreateState( stateData, ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALH_DESCENT_DRAGON];
						ASI_SA_ALH_DESCENT_DRAGON_CreateState( stateData, ASI_SA_ALH_DESCENT_DRAGON );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}

					if ( NULL != m_sMoonRightSlashDataPtr )
						m_sMoonRightSlashDataPtr.reset();

					m_sMoonRightSlashDataPtr = sMoonLightSlashData::CreateMoonLightSlashData();
					if ( NULL != m_sMoonRightSlashDataPtr )
					{
						m_sMoonRightSlashDataPtr->SetSlotID( ASI_SA_ALH_MOONLIGHT_SLASH_READY );
						m_sMoonRightSlashDataPtr->SetSlotB( false );
					}
				} break;


			case CX2SkillTree::SI_A_AM_WIND_WEDGE:		/// �ƶ� 1�� ���� - ���� 2�� �ٶ� ����
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ARA_WIND_WEDGE_START];
						ASI_SA_ARA_WIND_WEDGE_START_CreateState( stateData, ASI_SA_ARA_WIND_WEDGE_START );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ARA_WIND_WEDGE_END];
						ASI_SA_ARA_WIND_WEDGE_END_CreateState( stateData, ASI_SA_ARA_WIND_WEDGE_END );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}

				} break;

			case CX2SkillTree::SI_A_ALD_WOLF_CLAW:			/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� �������
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ALD_WOLF_CLAW];
						ASI_A_ALD_WOLF_CLAW_CreateState( stateData, ASI_A_ALD_WOLF_CLAW );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_ALD_WOLF_CLAW_FINISH];
						ASI_A_ALD_WOLF_CLAW_FINISH_CreateState( stateData, ASI_A_ALD_WOLF_CLAW_FINISH );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_SA_ALD_WOLF_SPEAR:		/// �ƶ� 1�� ���� �Ҹ� - ���� 4�� �����̻�
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_WOLF_SPEAR];
						ASI_SA_ALD_WOLF_SPEAR_CreateState( stateData, ASI_SA_ALD_WOLF_SPEAR );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_WOLF_SPEAR_FINISH];
						ASI_SA_ALD_WOLF_SPEAR_FINISH_CreateState( stateData, ASI_SA_ALD_WOLF_SPEAR_FINISH );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_WOLF_BREAK];			/// ���� �������� - �ؼ�
						ASI_SA_ALD_WOLF_BREAK_CreateState( stateData, ASI_SA_ALD_WOLF_BREAK );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_SA_ALD_FINGER_BULLET:		/// �ƶ� 1�� ���� �Ҹ� - ���� 1�� ��ȥ
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_FINGER_BULLET];
						ASI_SA_ALD_FINGER_BULLET_CreateState( stateData, ASI_SA_ALD_FINGER_BULLET );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_FINGER_BULLET_FINISH];
						ASI_SA_ALD_FINGER_BULLET_FINISH_CreateState( stateData, ASI_SA_ALD_FINGER_BULLET_FINISH );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_SA_ALD_ENERGY_BEAD:		/// �ƶ� 1�� ���� �Ҹ� - ���� 2�� ��ȯ��
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_ENERGY_BEAD];
						ASI_SA_ALD_ENERGY_BEAD_CreateState( stateData, ASI_SA_ALD_ENERGY_BEAD );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_ENERGY_BEAD_FINISH];
						ASI_SA_ALD_ENERGY_BEAD_FINISH_CreateState( stateData, ASI_SA_ALD_ENERGY_BEAD_FINISH );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_SA_ALD_PRISON_SPEAR:		/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� â�차��
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_PRISON_SPEAR];
						ASI_SA_ALD_PRISON_SPEAR_CreateState( stateData, ASI_SA_ALD_PRISON_SPEAR );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_ALD_PRISON_SPEAR_FINISH];
						ASI_SA_ALD_PRISON_SPEAR_FINISH_CreateState( stateData, ASI_SA_ALD_PRISON_SPEAR_FINISH );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;

			case CX2SkillTree::SI_A_AYR_STEAL_SOUL:			/// �ƶ� 2�� ���� ���� - ���� 4�� Żȥ
				{
					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_AYR_STEAL_SOUL];
						ASI_A_AYR_STEAL_SOUL_CreateState( stateData, ASI_A_AYR_STEAL_SOUL );
						SetSkillLevelStateData( pSkillTemplet, stateData );
						stateData.m_ptrComboAndSkillChainInfo = m_ComboAndSkillChainSystem.GetSkillChainInfoPtrBySkillID( iSkillID_ );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_A_AYR_STEAL_SOUL_FINISH];
						ASI_A_AYR_STEAL_SOUL_FINISH_CreateState( stateData, ASI_A_AYR_STEAL_SOUL_FINISH );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}

					{
						UserUnitStateData&	stateData = m_StateList[ASI_SA_AYR_OPEN_THE_HELL];		/// ���� �������� - ����
						ASI_SA_AYR_OPEN_THE_HELL_CreateState( stateData, ASI_SA_AYR_OPEN_THE_HELL );
						SetSkillLevelStateData( pSkillTemplet, stateData );
					}
				} break;
			}
		}
	}

}


/** @function	: GetSkillCoolTimeValue
	@brief		: �޸� ������ ���� ��Ÿ�� ���氪 ���� �Լ�
	@param		: ������ ��ų ���̵�, ������ ��ų�� ����Ʈ ��Ÿ��
	@return		: ���� ��ų ���� ��Ÿ��
*/
float CX2GUAra::GetSkillCoolTimeValue( const CX2SkillTree::SKILL_ID eSkillID_, const float fCoolTimeLeft )
{
	float fCoolTimeValue = fCoolTimeLeft;

	switch( eSkillID_ )
	{
		case CX2SkillTree::SI_A_AM_GAIN_FORCE:
		{
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO1 ) )
				fCoolTimeValue -= 1.f;
		} break;
	default:
		break;
	}

	return fCoolTimeValue;
}

/** @function	: GetActualFPConsume
	@brief		: �޸� ������ ���Ͽ� ������ ��� �Һ� ���� �Լ�
	@param		: ������ ��ų ���̵�, ������ ��ų�� ����Ʈ ��·�
	@return		: ���� ��ų ���� �Һ� ��·�
*/
USHORT CX2GUAra::GetActualFPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const USHORT usForceGainValue_ )
{
	USHORT usForceGainValue = usForceGainValue_;

	switch( eSkillID_ )
	{
		case CX2SkillTree::SI_SA_AM_SHADOW_KNOT:
		{
			/// �޸� : ���� �׸��� â�� (�׸��ڸŵ�) - �Һ� ��� 3
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO5 ) )
				usForceGainValue = 3;
		} break;
		case CX2SkillTree::SI_A_AM_WIND_WEDGE:
		{
			/// �޸� : �帧�� Ÿ�� â�� (��ȣ 2�� : �ٶ�����) - �Һ� ��� 1
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO2 ) )
				usForceGainValue = 1;
		} break;
	default:
		break;
	}

	return usForceGainValue;
}

/** @function	: GetActualFPGainCount
	@brief		: �޸� ������ ���Ͽ� ������ ��� ��ȯ�� ���� �Լ�
	@param		: ������ ��ų ���̵�, ������ ��ų�� ����Ʈ ��� ��ȯ��
	@return		: ���� ��ų ���� ��ȯ ��·�
*/
USHORT CX2GUAra::GetActualFPGainCount( const CX2SkillTree::SKILL_ID eSkillID_, const USHORT usForceGainValue_ )
{
	USHORT usForceGainValue = usForceGainValue_;

	switch( eSkillID_ )
	{
		case CX2SkillTree::SI_A_AM_WIND_WEDGE:
		{
			/// �޸� : �帧�� Ÿ�� â�� (��ȣ 2�� : �ٶ�����) - ��ȯ ��� 1
			if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_ARA_MEMO2 ) )
				usForceGainValue = 1;
		} break;
	default:
		break;
	}

	return usForceGainValue;
}


/*virtual*/ bool CX2GUAra::CanRidingState()
{
	if ( GetNowStateID() == CX2GUUser::USI_WAIT || GetNowStateID() == CX2GUUser::USI_WALK ||
		GetNowStateID() == CX2GUUser::USI_DASH || GetNowStateID() == CX2GUUser::USI_DASH_END ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT || GetNowStateID() == CX2GUUser::USI_RIDING_WALK ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT_HABIT || GetNowStateID() == CX2GUUser::USI_RIDING_DASH ||
		GetNowStateID() == CX2GUUser::USI_RIDING_DASH_END || GetNowStateID() == CX2GUAra::ASI_HYPER_WAIT )
		return true;

	return false;
}

/*virtual*/ void CX2GUAra::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_  )
{
	ShowActiveSkillCutInAndLightByBerserkMode(  fTimeToShow_, bOnlyLight_ );
}


//#pragma region CreateEnergeBullet
/** @function	: CreateEnergeBullet
	@brief		: ��ź ������ ����Ʈ ���� �Լ�
	@param		: ȸ����, �߻� �� ��ġ
*/
void CX2GUAra::CreateEnergeBullet( IN const float vRotZOffSet_ /*= 0.f*/, IN const bool bFireRightHand_ /*= true*/ )
{
	if( FlushMp( _CONST_ARA_::ENERGY_BULLET_MP_CONSUMPTION ) == true )		/// MP �Ҹ��� �˻�
	{
		/// ��ź ���� ��ġ
		D3DXVECTOR3 vPos = bFireRightHand_ ? GetBonePos( L"Bip01_R_Finger2" ) : GetBonePos( L"Bip01_L_Finger2" );

		D3DXVECTOR3 vRot = GetRotateDegree();

		vRot.z = vRotZOffSet_;		/// ��ź ȸ���� ����


		float fEnergyBulletPowerRate	= GetPowerRate();	/// ��ź ������ ����
		float fEnergyBulletForcePower	= 1.f;				/// ��ź �ٿ� ��ġ ������

		/// �нú� ��ų ��ź ��ȭ�� ���� ��ź ������ ����
		if ( 1.f < m_fIncreaseEnergeBulletDamage )
			fEnergyBulletPowerRate *= m_fIncreaseEnergeBulletDamage;

		/// �нú� ��ų ��� ��ȭ�� ���� ��ź �ٿ� ��ġ ����
		if ( 0.f < m_fIncreaseEnergeBulletForceDownValue )
			fEnergyBulletForcePower *= m_fIncreaseEnergeBulletForceDownValue;

		/// ��ź ����
		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this,
			L"EffectSet_SA_AYR_ORB_KI_FX_DAMAGE", fEnergyBulletPowerRate, vPos, vRot, vRot,
			m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, fEnergyBulletForcePower );

		if ( NULL != pDamageEffect )
		{
            CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = pDamageEffect->GetMainEffect();
            if ( pMeshInstance != NULL )
            {
			    /// �нú� ��ų ��� ��ȭ�� ���� ��ź ��Ÿ� ����
			    if ( 1.f < m_fIncreaseEnergeBulletRange )
			    {
				    const float fMaxLifeTime = pMeshInstance->GetMaxLifeTime();
				    pMeshInstance->SetMaxLifeTime( fMaxLifeTime * m_fIncreaseEnergeBulletRange );
			    }

			    /// ��ź �ٿ� ��ġ ���
				    pDamageEffect->SetInheritForceDownRate(true);

			    /// �нú� ��ų ��ź ��ȭ�� ���� ��ź ũ�� ����
			    if ( 1.f < m_fIncreaseEnergeBulletSize )
				    pDamageEffect->SetScale( D3DXVECTOR3( m_fIncreaseEnergeBulletSize, m_fIncreaseEnergeBulletSize, m_fIncreaseEnergeBulletSize ) );
            }
		}
	}
	else	/// MP ������ ��,
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
		if( true == GetIsRight() )
		{
			pos += GetDirVector() * 100.f;
		}
		else
		{
			pos -= GetDirVector() * 100.f;
		}

		CreateNotEnoughMPEffect( pos, 0.f, 0.f, vRotZOffSet_ );
	}
}
//#pragma endregion

//#pragma region SetResurrection
/** @function	: ProcessResurrection
	@brief		: �ƶ� 2�� ���� ���� - ȸ������ ���� ó�� �Լ�
	@Praram		: ��ų ����
	@Return		: ���� ����
*/
bool CX2GUAra::ProcessResurrection( IN const int iSkillLevel_ )
{
	/// ȸ������ ��� ���� �����̸�, ����� 3�� �̻��� ��
	if ( 0.f >= m_fResurrectionCoolTime && 3 <= GetForcePower() && NULL != g_pData->GetSkillTree() )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AYR_RESURRECTION );

		if ( NULL != pSkillTemplet )
		{
			/// ��Ÿ�� ����
			m_fResurrectionCoolTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_READY_TIME_REL, iSkillLevel_ );

			/// �⺻ ü�� ȸ����
			const float fDefaultAddHPValue		= ( GetMaxHp() * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TARGET_HP_REL, iSkillLevel_ ) );

			/// ��� 1���� ü�� ȸ����
			const float fForcePowerAddHPValue	= ( GetMaxHp() * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_HP_GAIN_REL_MAX_HP, iSkillLevel_ ) );

			/// ��¿� ���� ü�� ȸ��ġ ���� ( �⺻ ü�� ȸ���� + ���� ��¿� ���� ü�� ȸ���� )
			const float fAddHP					= fDefaultAddHPValue + ( GetForcePower() * fForcePowerAddHPValue );

			SetNowHp( GetNowHp() + fAddHP );	/// ü�� ȸ��

			UseForcePower( GetForcePower() );	/// ��� ��� �Һ�

			/// �ߵ� ����Ʈ ����
			if ( NULL != g_pX2Game->GetEffectSet() )
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_AYR_HEAL_FX", this );

			/// ü�� ȸ�� ���� ǥ��
			if( NULL != g_pData->GetPicCharGreen() )
			{
				D3DXVECTOR3 pos	= GetPos();		/// ���� ��ġ ����
				pos.y += 50.0f;

				WCHAR wszBuf[256] = {0,};		/// ���� ����
				StringCchPrintf( wszBuf, 256, L"HP +%d", static_cast<int>( fAddHP ) );

				g_pData->GetPicCharGreen()->DrawText( wszBuf, pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
			}

			/// ȣ�Ű��� ��� �Ұ� �ȳ� ����� ����
			SetBuffFactorToGameUnitByBuffFactorID( BFI_DEBUFF_SELF_PROTECTION_FORTITUDE );

			return true;
		}
	}

	return false;
}
//#pragma endregion

//#pragma region CanUseConversionHealth
/** @function	: CanUseConversionHealth
	@brief		: �������� ����� �� �ִ� �������� �˻�
	@param		: ��ų ���ø�, ��ų ����
	@return		: ��� ���� ����
*/
bool CX2GUAra::CanUseConversionHealth( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN const int iSkillLevel_ )
{
	if ( NULL == pSkillTemplet_ )
		return false;

	const float fLimitHPValue = GetMaxHp() * pSkillTemplet_->GetSkillAbilityValue( CX2SkillTree::SA_HP_REL_TO_MP_ABS_1, iSkillLevel_ );

	if ( fLimitHPValue < GetNowHp()  )
		return true;

	return false;
}
//#pragma endregion

//#pragma region SetAttackResultDelegate
/** @function	: SetAttackResultDelegate
	@brief		: Ÿ�ݽ� �߰��Ǵ� ��·� ����
	@param		: ������ ����
*/
void CX2GUAra::SetAttackResultDelegate()
{
	if ( NULL == GetUnit() )
	{
		 return;
	}

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	/// �ƶ� 1�� ���� �Ҹ� - ��ȥ Ȥ�� �ƶ� 2�� ���� �Ҹ� - Żȥ �����, Ÿ�� ���� �����ϴ� ��� ����
	if ( 0 < userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ALD_FINGER_BULLET ) ||
		 0 < userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AYR_STEAL_SOUL ) )
		InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUAra, AddHitUnitInfoByDamageData ) );

	/// Ÿ�ݽ� �߰��Ǵ� ��·� ����
	InsertDelegateProcessToAttackResult( SET_DELEGATE_PROCESS_WITH_DAMAGE_DATA(CX2GUAra, UpNowForcePowerHitOthers ) );


}
//#pragma endregion

//#pragma region UpNowForcePowerHitOthers
/** @function	: UpNowForcePowerHitOthers
	@brief		: Ÿ�ݽ� �߰��Ǵ� ��·� ����
	@param		: ������ ����
*/
void CX2GUAra::UpNowForcePowerHitOthers( const CX2DamageManager::DamageData& damageData_ )
{
	/// ��� ���淮�� �����Ǿ� ���� �ʴٸ�, �н�
	if ( 0 >=  damageData_.m_iHitAddForcePower )
		return;

	const int	iMaxForcePower = GetMaxForcePower();				/// �ִ� ��·�
	const int	iNowForcePower = GetForcePower();					/// ���� ��·�
	int			iAddForcePower = damageData_.m_iHitAddForcePower;	/// �߰��� ��·�

	/// �ִ� ��·��� �Ѵ´ٸ�, �ִ�ġ�� �߰� �ǵ��� ����
	if ( iMaxForcePower < iNowForcePower + iAddForcePower )
		iAddForcePower = iMaxForcePower - iNowForcePower;

	SetForcePower( iNowForcePower + iAddForcePower );
}
//#pragma endregion

//#pragma region AddHitUnitByFingerBullet
/** @function	: InsertHitUnitByFingerBullet
	@brief		: Ÿ�ݵ� ����� ��ü ������ ����( �ƶ� 1�� ���� �Ҹ� - ���� )
	@param		: ������ ����
*/
void CX2GUAra::AddHitUnitInfoByDamageData( const CX2DamageManager::DamageData& damageData_ )
{
	if ( null == damageData_.optrDefenderGameUnit )
		return;

	switch ( damageData_.m_eUsedSkillID )
	{
	case CX2SkillTree::SI_SA_ALD_FINGER_BULLET:
	case CX2SkillTree::SI_A_AYR_STEAL_SOUL:
		{
			/// �ǰ����� UID ( �񱳸� ����, NPC�� �ϴ� 64��Ʈ�� ��ȯ )
			UidType uidUnitUID = static_cast<UidType>( damageData_.optrDefenderGameUnit->GetUnitUID() );

			/// ���� ���͸� ��ȸ�Ͽ�, �̹� �ش� UID�� �߰��Ǿ����� �˻��Ѵ�.
			std::vector<CX2GameUnitoPtr>::const_iterator vit = m_vecHitUnitPointer.begin();

			for ( ; vit != m_vecHitUnitPointer.end(); ++vit )
			{
				CX2GameUnitoPtr pGameUnit = *vit;

				/// �̹� �ش� UID�� �߰��Ǿ� �ִٸ�, �н�
				if ( null != pGameUnit && pGameUnit->GetUnitUID() == uidUnitUID )
					return;
			}

			/// �ش� UID�� �߰��Ǿ� ���� �ʴٸ�, �߰�
			m_vecHitUnitPointer.push_back( damageData_.optrDefenderGameUnit );
		} break;
	default:
		break;
	}
}
//#pragma endregion

//#pragma region ModifyFinalDamageByGameUnitType
/** @function	: ModifyFinalDamageByGameUnitType
	@brief		: ������ ó���� �߰��� ���� ��ġ �����ϴ� �Լ�( GetFinalDamage )
	@param		: ������ Ÿ��, �߰� ���� ���ݷ�, �߰� ���� ���ݷ�, �ٿ� ��ġ
*/
/*virtual*/ void CX2GUAra::ModifyFinalDamageByGameUnitType( OUT CX2DamageManager::DAMAGE_TYPE& damageType_, OUT float& /*fAttackerPhysicDamage_*/,
															OUT float& /*fAttackerMagicDamage_*/, OUT float& fForceDownValue_ )
{
	if ( NULL == GetUnit() )
		return;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	/// �ƶ� 1�� ���� �Ҹ� - ��� ��ȭ
	if ( 0 < userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ALD_ENERGY_REINFORCE ) )
	{
		///  �Ϲ� ������ ��, ��� ��ȭ�� ���� �ٿ� ��ġ ���Ұ� ����
		// The original compared with AT_NORMAL (0, same value; see docs/original/bugs.md).
		if ( CX2DamageManager::DT_PHYSIC == damageType_ )
			fForceDownValue_ -= m_fIncreaseEnergeBulletForceDownValue;
	}
}
//#pragma endregion


//#pragma region GetStateIDBySecretSkillStartID
/** @function	: GetAraStateIDBySecretSkillStartID
	@brief		: ��ų ���� ������Ʈ ���̵� �ش��ϴ� ������Ʈ ���̵� ��ȯ
	@param		: ��ų ���̵�
	@return		: ������Ʈ ���̵�
*/
/*virtual*/void CX2GUAra::GetStateIDBySecretSkillTriggerStateID( IN OUT int& iStateID_ ) const
{
	if ( NULL != GetUnit() )
	{
		/// ��ų ���� StateID�� �����Ǿ� �ִ� ��ų ID�� ��ȯ ( ��ų ���� State ID�� �ƴ϶��, SI_NONE ��ȯ )
		const CX2SkillTree::SKILL_ID	eSkillIDBySlotData = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillIDByEquipSlotID( iStateID_ );

		/// ��ų ID���� �ش� ��ų�� State ID�� ��ȯ
		/// ( �⺻ ��ų State ID�� ��ų ������ �ε��� �̹Ƿ�, �ش� ID�� ���� ���� State ID�� ��ȯ )
		if (  CX2SkillTree::SI_NONE != eSkillIDBySlotData )
		{
			switch ( eSkillIDBySlotData )
			{
				/// ���� �������� �ؼ� ���� ������Ʈ - ���� 4�� �����̻�
			case CX2SkillTree::SI_SA_ALD_WOLF_SPEAR:
				{
					iStateID_ = static_cast<int>( ASI_SA_ALD_WOLF_SPEAR );
				} break;

				/// ���� �������� ���� ���� ������Ʈ - ���� 4�� Żȥ
			case CX2SkillTree::SI_A_AYR_STEAL_SOUL:
				{
					iStateID_ = static_cast<int>( ASI_A_AYR_STEAL_SOUL );
				} break;
			default:
				break;
			}
		}
	}
}
//#pragma endregion

void CX2GUAra::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.f, 0.1f, 0.1f, true );
}
void CX2GUAra::CommonHyperModeFrameMove( float fTime1_, float fTime2_, float fTime3_, bool bSound_ /*= false*/  )
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

		// ���� �ڵ� ������ ���� �ڵ忡�� ���� ��� ���ִ� �� ����
		if( true == IsSamef(fTime2_ ,fTime3_) )
		{
			if ( true == IsFullHyperMode() )
				PlaySound( L"AraVoice_BoostPower.ogg" );
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
			PlaySound( L"AraVoice_BoostPower.ogg" );
	}

	CommonFrameMove();
}

//#pragma region IsNoStageChangeSkill
/** @function	: IsNoStageChangeSkill
	@brief		: �ش� ��ų�� ������Ʈ�� ���� ��ų���� �˻�
	@param		: ����� ��ų�� ��ų ���ø� ������
	@return		: ������Ʈ�� ���� ��ų���� ����
*/
bool CX2GUAra::IsNoStageChangeSkill( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ ) const
{
	if ( NULL != pSkillTemplet_ )
	{
		switch( pSkillTemplet_->m_eID )
		{
		case CX2SkillTree::SI_A_ALD_REFLECTION:		/// �ƶ� 1�� ���� �Ҹ� - ��ź��
			return true;
		default:
			break;
		}
	}

	return false;
}
//#pragma endregion

//#ifdef FIX_GAIN_FORCE_BUG
/*virtual*/void CX2GUAra::AttackResultByType( CX2DamageManager::DamageData &pDamageData )
{
	if( CX2SkillTree::SI_A_AM_GAIN_FORCE == m_NowStateData.m_eSkillID )
	{
		if( m_hGainForceIntakeEffect == pDamageData.hAttackerEffect )
			++m_iGainForceDamageTimeNow;
	}
}
//#endif // FIX_GAIN_FORCE_BUG

/** @function	: GetHypetMpdeStateID
	@brief		: �ƶ� 3���� ���ο� ���� ������Ʈ ���̵� ��ȯ
	@return		: ������Ʈ ���̵�
*/
/*virtual*/ int CX2GUAra::GetHypetMpdeStateID()
{
	if ( 3 <= m_FrameDataFuture.syncData.m_HyperModeCount )
		return static_cast<int>( ASI_BERSERK_MODE );
	else
		return static_cast<int>( USI_HYPER_MODE );
}
