#include "stdafx.h"
#include "./X2GUArme_VioletMage.h"



// �ʻ�� �� �� cut in image
static const CX2GUUser::SkillCutInSet s_SkillCutInSet[] =
{
	// ���̻�, ���̸�����, ��ũ������
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_1.dds"),// D3DXVECTOR2(619.0f,619.0f), D3DXVECTOR3(176.0f,497.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_2.dds"),// D3DXVECTOR2(674.0f,674.0f), D3DXVECTOR3(155.0f,523.0f,0.0f) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_3.dds"),// D3DXVECTOR2(670.0f,670.0f), D3DXVECTOR3(275.0f,471.0f,0.0f) ),
	},


	// elemental master
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_EM_1.dds"),// D3DXVECTOR2( 663.f, 663.f ), D3DXVECTOR3( 319.f, 459.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_EM_1.dds"),// D3DXVECTOR2( 663.f, 663.f ), D3DXVECTOR3( 319.f, 459.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_EM_1.dds"),// D3DXVECTOR2( 663.f, 663.f ), D3DXVECTOR3( 319.f, 459.f, 0.f ) ),
	},

	// void princess
	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_VP_1.dds"),// D3DXVECTOR2( 722.f, 722.f ), D3DXVECTOR3( 353.f, 420.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_VP_1.dds"),// D3DXVECTOR2( 722.f, 722.f ), D3DXVECTOR3( 353.f, 420.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_VP_1.dds"),// D3DXVECTOR2( 722.f, 722.f ), D3DXVECTOR3( 353.f, 420.f, 0.f ) ),
	},

	{
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_DW_1.dds"),//D3DXVECTOR2( 722.f, 722.f ), D3DXVECTOR3( 353.f, 420.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_DW_1.dds"),//D3DXVECTOR2( 722.f, 722.f ), D3DXVECTOR3( 353.f, 420.f, 0.f ) ),
		CX2GUUser::SkillCutIn( L"HQ_CutIn_Aisha_DW_1.dds"),//D3DXVECTOR2( 722.f, 722.f ), D3DXVECTOR3( 353.f, 420.f, 0.f ) ),
	},
};







namespace _CONST_AISHA_
{

	const std::string UFO_IDLE			= "UFO_AISHA_IDLE";
	const std::string UFO_GO_LEFT		= "UFO_AISHA_GO_LEFT";
	const std::string UFO_GO_RIGHT 	= "UFO_AISHA_GO_RIGHT";
	const std::string UFO_GO_UP		= "UFO_AISHA_GO_UP";
	const std::string UFO_GO_DOWN		= "UFO_AISHA_GO_DOWN";

	const std::string UFO_GO_UP_LEFT		= "UFO_AISHA_GO_UP_LEFT";
	const std::string UFO_GO_UP_RIGHT 		= "UFO_AISHA_GO_UP_RIGHT";
	const std::string UFO_GO_DOWN_LEFT		= "UFO_AISHA_GO_DOWN_LEFT";
	const std::string UFO_GO_DOWN_RIGHT 	= "UFO_AISHA_GO_DOWN_RIGHT";

	const std::string UFO_FIRE			= "UFO_AISHA_FIRE";
	// �� ��ӿ� ��� State Parsing �� ���� �߰�
	const std::string UFO_DYING		= "UFO_AISHA_DYING";


	const D3DXCOLOR coSpecialActive( 0xff222222 );
	const D3DXCOLOR coSpecialActiveHyper( 0xffff0000 );

	const float AISHA_INITIAL_MP( 50.f );

	const float AISHA_LEVITATION_CONSUME_MP_TIME( 0.2f );
	const float MAGICAL_MAKEUP_TRANSFORMING_TIME = 1.3f;	/// �����ø���ũ�� ���ŵ����� �����ϴ� �ð�

	const float AEM_COMBOX4_MAX_CHARGING_TIME( 2.f );					/// 4��° ���̾ �ִ� ���� �ð�
	const float AEM_METEO_SHOWER_USE_MP( 5.f );							/// ���׿� ���� 1�ߴ� �Ҹ� MP��
	const float AEM_MEDITATION_REDUCE_COOLTIME_VALUE( 15.f );			/// �޵����̼� �޸� ��Ÿ�� ����ġ
	const float AHM_CHAIN_BURST_MEMO_INCREASE_POWER_RATE_VALUE( 1.1f );	/// ü�� ����Ʈ �޸� ������ ����ġ

	const float CREATE_HELL_DROP_DAMAGE_EFFECT_TIME_GAP ( 0.25f );		/// �� ���, �������� �ð� ��
	const float CREATE_HELL_DROP_DAMAGE_EFFECT_START_TIME ( 1.0f );		/// �� ���, �������� �����ϴ� �ð�
	const float BIDING_CIRCLE_BASIC_DISTANCE ( 500.f );					/// ���ε� ��Ŭ, �⺻ ���� ( ���� 1 ���� )
}





CX2GUArme_VioletMage::CX2GUArme_VioletMage( int unitIndex, int teamNum,
	lua_State*   pScriptLuaState,
	int frameBufferNum, CX2Unit* pUnit )
: CX2GUUser( unitIndex, teamNum,
	pScriptLuaState,
	frameBufferNum, pUnit )
, m_optrUFOGameUnit()
, m_bIsSummoningUFO( false )
, m_bElementalFriendShip( false )
, m_pStrongMindData( NULL )
, m_ptrTransformWeapon( CX2EqipPtr() )
, m_fTransformTime( 0.0f )
, m_fLevitationElapsedTime( 0.f )
, m_bCanChangeState( false )		/// Ư�� ������Ʈ�� �Ѿ �� �ִ��� �����ϴ� ����
, m_fPushXChargeTime( 0.f )			/// 4��° ���̾ ���� �ð�
, m_iMaxFireCount( 10 )				/// ���׿� ���� ������Ʈ���� �ִ� �߻��� �� �ִ� ���� ����
, m_iMaxFireCountByLoopState( 10 )	/// ���׿� ���� ���� ������Ʈ���� �ִ� �߻��� �� �ִ� ���� ����
, m_iElapsedFireCount( 0 )			/// ���׿� ���� ���� ������Ʈ���� ������ �߻緮 ����
, m_fUseMPConsume( 5.f )			/// ���׿� ���� ���� ������Ʈ���� �ѹߴ� �Ҹ�Ǵ� MP��
, m_fFireGap( 0.2f )				/// ���׿� ���� �ѹߴ� �߻�Ǵ� ����
, m_fHellDropCreateEffectCooltime ( 0.f )		/// �� ���, ������ Effect ����
, m_bIsHellDropTimerStart ( false )				// �� ��� Ÿ�̸Ӱ� �����°�?

{
	for( int i=0; i<EffSetID_END; i++ )
	{
		m_ahEffectSet[i] = INVALID_EFFECTSET_HANDLE;
	}


	m_hSeqArmeAirStepRight	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqArmeAirStepLeft	= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_pMPChargeA			= NULL;
	m_pMPChargeB			= NULL;
	m_pMPChargeC			= NULL;

	m_pEvasionAttackAnim	= NULL;

	m_hSeqMPEnergy				= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqMPEnergyCenter		= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqMPChargeWave			= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_hSeqMPCharge2MagicCircle	= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqMPCharge2Up			= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqMPCharge2Up2			= INVALID_PARTICLE_SEQUENCE_HANDLE;

	m_pComboX3Up1			= NULL;
	m_pComboX3Up2			= NULL;
	m_pComboX3Up3			= NULL;




	//m_pMeteoCall			= NULL;
	m_hHellStoneEffect		    = INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser1		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser2		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser3		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser4		= INVALID_DAMAGE_EFFECT_HANDLE;

	m_bZ4up					= false;



	InitializeArmeMajorParticleArray();
	InitializeArmeMinorParticleArray();

	//������

	m_hSeqLight_Arme_Aging1				= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqLight_Arme_Aging2				= INVALID_PARTICLE_SEQUENCE_HANDLE;
	m_hSeqLight_Arme_Aging3				= INVALID_PARTICLE_SEQUENCE_HANDLE;


	//���̳�
	m_fMPConsumeRateTeleport 	= 1.f;
	m_fMPConsumeRateFireBall 	= 1.f;
	m_fFireBallRangeRate		= 1.f;
	m_bInMeditation				= false;
	m_fMeditationMPChangeRate	= 0.f;
	m_fAddMPChangeRate			= 0.f;

	m_iHyperChargeMP = 0.f;
	m_iHyperAddMP = 0.f;
	m_fRateLimitManaManagement = 0.f;
	m_iHyperChargeMP = 100.f;

	m_iDrainEnergyTarget = -1;
	m_hEffectMorningStar = INVALID_EFFECTSET_HANDLE;
	m_fMagicStaffExtraRate = 0.f;
	m_fMagicStaffSize = 0.f;
	m_hMagicalStaff	= INVALID_MESH_INSTANCE_HANDLE;

	m_fThrowStartTime		= 0.6666f;
	m_vThrowStartPosOffset	= D3DXVECTOR3(40.f, 130.f, 0.f);
	m_vThrowStartVelocity	= D3DXVECTOR3(1600.f, 200.f, 0.f);

	m_bDashJumpZ = false;
	m_bIsOnSomethingStart = false;
	m_fMPChargeSoundTime = 0.f;
	m_fMPChargeWaitTime = 0.f;

	m_bIceBall = false;
	m_fIceBallPower = 1.f;
	m_fElementalResearchRate = 0.f;
	m_bResearchGravityBall = false;
	m_fGravityBallPower = 1.f;
}

CX2GUArme_VioletMage::~CX2GUArme_VioletMage(void)
{
	SAFE_DELETE( m_pStrongMindData );

	if( NULL != g_pX2Game )
	{
		for( StoneWallData& stoneWallData : m_vecStoneWallData )
		{
			CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( (int) stoneWallData.m_NPCUID );
			if( NULL != pNPC )
			{
				pNPC->SetNowHp( 0.f );
			}
		}
	}


	for( int i=0; i<EffSetID_END; i++ )
	{
		g_pX2Game->GetEffectSet()->StopEffectSet( m_ahEffectSet[i] );
	}


	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqArmeAirStepRight );
	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqArmeAirStepLeft );



	SAFE_DELETE_KTDGOBJECT( m_pMPChargeA );
	SAFE_DELETE_KTDGOBJECT( m_pMPChargeB );
	SAFE_DELETE_KTDGOBJECT( m_pMPChargeC );




	//SAFE_DELETE_KTDGOBJECT( m_pMeteoCall );
	SAFE_DELETE_KTDGOBJECT( m_pEvasionAttackAnim );



	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqMPEnergy );
	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqMPEnergyCenter );
	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqMPChargeWave );

	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqMPCharge2MagicCircle );
	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqMPCharge2Up );
	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqMPCharge2Up2 );


	SAFE_DELETE_KTDGOBJECT( m_pComboX3Up1 );
	SAFE_DELETE_KTDGOBJECT( m_pComboX3Up2 );
	SAFE_DELETE_KTDGOBJECT( m_pComboX3Up3 );


	DeleteArmeMajorParticle();
	DeleteArmeMinorParticle();



	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqLight_Arme_Aging1 );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqLight_Arme_Aging2 );
	g_pX2Game->GetMajorParticle()->DestroyInstanceHandle( m_hSeqLight_Arme_Aging3 );

	if( m_hMagicalStaff != INVALID_MESH_INSTANCE_HANDLE )
	{
		g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hMagicalStaff );
		m_SubAttackListSet.clear();
	}

	g_pX2Game->GetMinorParticle()->DestroyInstanceHandle( m_hSeqLight_Arme_Circle_Flame	);
}


void CX2GUArme_VioletMage::ReInit( bool bRandom, int startPosIndex )
{
	CX2GUUser::ReInit( bRandom, startPosIndex );


	if( GetNowMp() < _CONST_AISHA_::AISHA_INITIAL_MP )
		SetNowMp( _CONST_AISHA_::AISHA_INITIAL_MP );

	m_bZ4up					= false;
}

void CX2GUArme_VioletMage::DamageReact( CX2DamageManager::DamageData* pDamageData )
{
	KTDXPROFILE();
	CX2GUUser::DamageReact( pDamageData );
	CX2GUUser::DamageReactStateChange( pDamageData );


	if( true == m_bInMeditation && false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO11 ) )
		EraseBuffTempletFromGameUnit( BTI_BUFF_SI_A_AHM_MEDITATION );


}

void CX2GUArme_VioletMage::Win()
{
	CX2GUUser::Win();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}

void CX2GUArme_VioletMage::Lose()
{
	CX2GUUser::Lose();

	if( IsMyUnit() == true && GetNowHp() > 0.0f )
	{
		if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
			StateChange( USI_RIDING_WAIT );
		else
			StateChange( USI_LOSE );
	}
}


void CX2GUArme_VioletMage::Draw()
{
	CX2GUUser::Draw();

	if ( true == GetRidingOn() && NULL != m_ptrRidingPet )
		StateChange( USI_RIDING_WAIT_HABIT );
	else
		StateChange( USI_WIN );
}

/*static*/
void	CX2GUArme_VioletMage::AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMinorParticle() != NULL )
	{
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"ArmeAirStep" );
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"MPEnergyArme" );
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"MPEnergyCenter" );
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"MPChargeWave" );
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"MPCharge2MagicCircle" );
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"MPCharge2Up" );
		g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_,  L"MPCharge2Up2" );
	}
}


void CX2GUArme_VioletMage::InitComponent()
{
	CX2GUUser::InitComponent();

	if ( IsMyUnit() )
	{
		ASSERT( NULL != g_pX2Game );
		switch ( g_pX2Game->GetGameType() )
		{
		case CX2Game::GT_PVP:
			{
				m_pGageData->SetNowMp( _CONST_AISHA_::AISHA_INITIAL_MP );
			} break;
		default:
			break;
		}
	}


	m_hSeqArmeAirStepRight	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"ArmeAirStep", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );
	m_hSeqArmeAirStepLeft	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"ArmeAirStep", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

	m_hSeqMPEnergy			= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MPEnergyArme", 0,0,0, 0,0 );
	m_hSeqMPEnergyCenter		= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MPEnergyCenter", 0,0,0, 0,0 );


	CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergy );
	if( NULL != pSeq )
	{
		pSeq->UseLookPoint( true );
	}

	m_hSeqMPChargeWave			= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MPChargeWave", 0,0,0, 0,0 );

	m_hSeqMPCharge2MagicCircle	= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MPCharge2MagicCircle", 0,0,0, 0,0 );
	m_hSeqMPCharge2Up			= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MPCharge2Up", 0,0,0, 0,0 );
	m_hSeqMPCharge2Up2			= g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"MPCharge2Up2", 0,0,0, 0,0 );
}


/*static*/
void	CX2GUArme_VioletMage::AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ )
{
	CX2GUArme_VioletMage::AppendMajorParticleToDeviceList( listInOut_ );
	CX2GUArme_VioletMage::AppendMinorParticleToDeviceList( listInOut_ );
	CX2GameUnit::Weapon::AppendEnchantParticleForSwordToDeviceList( listInOut_, eDamageType_);
}

void CX2GUArme_VioletMage::InitEffect()
{
	CX2GUUser::InitEffect();

	CreateMajorParticleArray();
	CreateMinorParticleArray();
}




/*virtual*/ void CX2GUArme_VioletMage::InitEnchantWeaponEffectUnitSpecific()
{
	for( UINT i=0; i<m_vecpWeapon.size(); ++i )
	{
		Weapon* pWeapon = m_vecpWeapon[i];
		pWeapon->InitEnchantParticleForSword();
	}
}


void CX2GUArme_VioletMage::DeleteArmeMajorParticle()
{
	CX2GUUser::DeleteGUUserMajorParticle();

	CKTDGParticleSystem* pMajorParticleSystem = g_pX2Game->GetMajorParticle();

	for ( int index = 0; index < AISHA_PII_MAJOR_END; index++ )
	{
		pMajorParticleSystem->DestroyInstanceHandle( m_ahAishaMajorParticleInstance[index] );
	}
}

void CX2GUArme_VioletMage::DeleteArmeMinorParticle()
{
	CX2GUUser::DeleteGUUserMinorParticle();

	CKTDGParticleSystem* pMinorParticleSystem = g_pX2Game->GetMinorParticle();

	for ( int index = 0; index < AISHA_PII_MINOR_END; index++ )
	{
		pMinorParticleSystem->DestroyInstanceHandle( m_ahAishaMinorParticleInstance[index] );
	}
}

void CX2GUArme_VioletMage::InitializeArmeMajorParticleArray()
{
	CX2GUUser::InitializeGUUserMajorParticleArray();

	for ( int index = 0; index < AISHA_PII_MAJOR_END; index++ )
	{
		m_ahAishaMajorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}
}

/*static*/
void	CX2GUArme_VioletMage::AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMajorParticle() == NULL )
		return;

	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpact" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpactCore" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpactSlash" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpactRed" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpactRingRed" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpactCoreRed" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"FireDamageImpactSlashRed" );

	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"ArmeAirShockRing" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"ArmeAirShockCenter" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"ArmeAirBlastCenter" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt1" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt2" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt3" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt4" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt5" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt6" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt7" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt8" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt9" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt10" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt11" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt12" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBoltCore" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt1Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt2Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt3Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt4Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt5Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt6Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt7Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt8Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt9Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt11Red" );
	g_pData->GetGameMajorParticle()->AppendToDeviceList( listInOut_, L"LightningBolt12Red" );

}


/*virtual*/ void CX2GUArme_VioletMage::CreateMajorParticleArray()
{
	CX2GUUser::CreateMajorParticleArray();

	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	// small damage Ÿ���� ���� �� ������ ��ƼŬ
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT,			L"FireDamageImpact",			2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_CORE,		L"FireDamageImpactCore",		2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_SLASH,		L"FireDamageImpactSlash",		2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	// AttackResult�� switch case���� default:�� ó���Ǵ� Ÿ�ݿ� ���� ��ƼŬ (����� small �̿��� ��ƼŬ)
	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RED,		L"FireDamageImpactRed",			2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_RING_RED,	L"FireDamageImpactRingRed",		1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_CORE_RED,	L"FireDamageImpactCoreRed",		1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetCommonMajorParticleByEnum( COMMON_MAJOR_PII_DAMAGE_IMPACT_SLASH_RED,	L"FireDamageImpactSlashRed",	1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	// �޺���
	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_RING, L"ArmeAirShockRing", 2 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_CENTER, L"ArmeAirShockCenter", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_BLAST_CENTER, L"ArmeAirBlastCenter", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	//{{ ����Ʈ�׺�Ʈ, ���۳ο��̺꿡�� ���̴� ��ƼŬ
	// kimhc // need refactoring �� ��ų�� EFFECT_pSeq = Set���� �����ϸ� �̺κ��� ���� �ص� �ɵ�
	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_01, L"LightningBolt1", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_02, L"LightningBolt2", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_03, L"LightningBolt3", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_04, L"LightningBolt4", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_05, L"LightningBolt5", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_06, L"LightningBolt6", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_07, L"LightningBolt7", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_08, L"LightningBolt8", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_09, L"LightningBolt9", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_10, L"LightningBolt10", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_11, L"LightningBolt11", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_12, L"LightningBolt12", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_CORE, L"LightningBoltCore", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_01, L"LightningBolt1Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_02, L"LightningBolt2Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_03, L"LightningBolt3Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_04, L"LightningBolt4Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_05, L"LightningBolt5Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_06, L"LightningBolt6Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_07, L"LightningBolt7Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_08, L"LightningBolt8Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_09, L"LightningBolt9Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_10, L"LightningBolt10Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_11, L"LightningBolt11Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_12, L"LightningBolt12Red", -1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );
	//}} ����Ʈ�׺�Ʈ, ���۳ο��̺꿡�� ���̴� ��ƼŬ
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUArme_VioletMage::SetAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleAishaMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );

		SetHandleAishaMajorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( eVal_) );

	return pSeq;
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUArme_VioletMage::SetAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleAishaMajorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_ );

		SetHandleAishaMajorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUArme_VioletMage::InitializeArmeMinorParticleArray()
{
	CX2GUUser::InitializeGUUserMinorParticleArray();

	for ( int index = 0; index < AISHA_PII_MINOR_END; index++ )
	{
		m_ahAishaMinorParticleInstance[index] = INVALID_PARTICLE_SEQUENCE_HANDLE;
	}

	m_hSeqLight_Arme_Circle_Flame = INVALID_PARTICLE_SEQUENCE_HANDLE;
}

/*static*/
void	CX2GUArme_VioletMage::AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	if ( g_pData->GetGameMinorParticle() == NULL )
		return;

	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"ArmeAirBlastRing" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"TimeStop1" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"TimeStop2" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"TimeStop3" );
	g_pData->GetGameMinorParticle()->AppendToDeviceList( listInOut_, L"TimeStopRed" );

}

/*virtual*/ void CX2GUArme_VioletMage::CreateMinorParticleArray()
{
	CX2GUUser::CreateMinorParticleArray();


	CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;

	pSeq = SetAishaMinorParticleByEnum( AISHA_PII_MINOR_AIR_BLAST_RING,	L"ArmeAirBlastRing", 1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMinorParticleByEnum( AISHA_PII_MINOR_TIME_STOP_01,		L"TimeStop1", 1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMinorParticleByEnum( AISHA_PII_MINOR_TIME_STOP_02,		L"TimeStop2", 1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMinorParticleByEnum( AISHA_PII_MINOR_TIME_STOP_03,		L"TimeStop3", 1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );

	pSeq = SetAishaMinorParticleByEnum( AISHA_PII_MINOR_TIME_STOP_RED,		L"TimeStopRed", 1 );
	if( pSeq != NULL )
		pSeq->SetTriggerWait( true );
}

CKTDGParticleSystem::CParticleEventSequence* CX2GUArme_VioletMage::SetAishaMinorParticleByEnum( AISHA_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ )
{
	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == GetHandleAishaMinorParticleByEnum( eVal_ ) )
	{
		ParticleEventSequenceHandle hHandle =
			g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  wstrParticleName_.c_str(), D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 0, 0, iDrawCount_ );

		SetHandleAishaMinorParticleByEnum( eVal_, hHandle );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeq =
		g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleAishaMinorParticleByEnum( eVal_) );

	return pSeq;
}

void CX2GUArme_VioletMage::ParseCommonRandomState()
{


	if( true == m_LuaManager.BeginTable( "INIT_COMMON_RANDOM_STATE" ) )
	{

		const char* tableName = "";
		switch( GetUnit()->GetClass() )
		{
		case CX2Unit::UC_ARME_VIOLET_MAGE:
			{
				tableName = "ARME_VIOLET_MAGE";
			} break;
		case CX2Unit::UC_ARME_HIGH_MAGICIAN:
			{
				tableName = "ARME_HIGH_MAGICIAN";
			} break;
		case CX2Unit::UC_ARME_DARK_MAGICIAN:
			{
				tableName = "ARME_DARK_MAGICIAN";
			} break;
		case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
			{
				tableName = "ARME_BATTLE_MAGICIAN";
			} break;

		case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
			{
				tableName = "ARME_ELEMENTAL_MASTER";
			} break;
		case CX2Unit::UC_ARME_VOID_PRINCESS:
			{
				tableName = "ARME_VOID_PRINCESS";
			} break;
		case CX2Unit::UC_ARME_DIMENSION_WITCH:
			{
				tableName = "ARME_DIMENSION_WITCH";
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


void CX2GUArme_VioletMage::InitState()
{
	ASSERT( NULL != GetUnit() );

	const CX2Unit::UnitData* pUnitData = &GetUnit()->GetUnitData();


	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
		{
			m_iSkillCutInSetIndex = 0;
		} break;
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			m_iSkillCutInSetIndex = 1;
		} break;
	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{
			m_iSkillCutInSetIndex = 2;
		} break;
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			m_iSkillCutInSetIndex = 3;
		} break;
	default:
		break;
	}

	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
		{
			m_iSkillCutInSetSubIndex = 1;
		} break;
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
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
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WIN;
	m_LuaManager.MakeTableReference( tableNameWinUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, WinStateStart );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_LOSE;
	m_LuaManager.MakeTableReference( tableNameLoseUTF8.c_str(), stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, LoseStateStart );
	m_StateList[stateData.stateID] = stateData;






	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X2;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_X2", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX2EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X2;
			m_LuaManager.MakeTableReference( "AVSI_HIGH_MAGICIAN_COMBO_X2", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X2_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X2_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3back;
			m_LuaManager.MakeTableReference( "AVSI_HIGH_MAGICIAN_COMBO_X3back", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X3back_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X3back_EventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X3back_EndFuture );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X2;
			m_LuaManager.MakeTableReference( "AVSI_DARK_MAGICIAN_COMBO_X2", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X2_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X2_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3back;
			m_LuaManager.MakeTableReference( "AVSI_DARK_MAGICIAN_COMBO_X3back", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X3back_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X3back_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}



	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3down;
			m_LuaManager.MakeTableReference( "AVSI_HIGH_MAGICIAN_COMBO_X3down", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X3down_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X3down_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X4down;
			m_LuaManager.MakeTableReference( "AVSI_HIGH_MAGICIAN_COMBO_X4down", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X4down_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICIAN_COMBO_X4down_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3down;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_X3down", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3downFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3downEventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3down;
			m_LuaManager.MakeTableReference( "AVSI_DARK_MAGICIAN_COMBO_X3down", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X3down_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X3down_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X4down;
			m_LuaManager.MakeTableReference( "AVSI_DARK_MAGICIAN_COMBO_X4down", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X4down_Start );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X4down_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_COMBO_X4down_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}









	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_COMBO_Z2a;
			m_LuaManager.MakeTableReference( "AVSI_DASH_COMBO_Z2a", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ2aStartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ2aFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ2aEventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;

		case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			stateData.Init();
			stateData.stateID			= AVSI_EM_DASH_COMBO_Z2a;
			m_LuaManager.MakeTableReference( "AVSI_EM_DASH_COMBO_Z2a", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EM_DASH_COMBO_Z2a_StartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EM_DASH_COMBO_Z2a_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EM_DASH_COMBO_Z2a_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;


	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_COMBO_Z2a;
			m_LuaManager.MakeTableReference( "AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_Start );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_StartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_DASH_COMBO_Z3up;
			m_LuaManager.MakeTableReference( "AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_ABM_DASH_COMBO_Z2a;
			m_LuaManager.MakeTableReference( "AVSI_ABM_DASH_COMBO_Z2a", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_Start );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_StartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ABM_DASH_COMBO_Z2a_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ABM_DASH_COMBO_Z2a_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ABM_DASH_COMBO_ZXX;
			m_LuaManager.MakeTableReference( "ABM_DASH_COMBO_ZXX", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ABM_DASH_COMBO_ZXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= ABM_DASH_COMBO_ZXXX;
			m_LuaManager.MakeTableReference( "ABM_DASH_COMBO_ZXXX", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ABM_DASH_COMBO_ZXXX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ABM_DASH_COMBO_ZXXX_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
		break;
	default:
		break;
	}





	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_VOID_PRINCESS:
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_DASH_ATTACK_X", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXStartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXEventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXEndFuture );
			m_StateList[stateData.stateID] = stateData;
		} break;


	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_HIGH_MAGICION_DASH_ATTACK_X", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICION_DASH_ATTACK_X_StartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICION_DASH_ATTACK_X_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICION_DASH_ATTACK_X_EventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICION_DASH_ATTACK_X_EndFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_DASH_ATTACK_X2;
			m_LuaManager.MakeTableReference( "AVSI_HIGH_MAGICION_DASH_ATTACK_X2", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICION_DASH_ATTACK_X2_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HIGH_MAGICION_DASH_ATTACK_X2_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;

	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_DIMENSION_WITCH_DASH_ATTACK_X", stateData.stateID );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXStartFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DIMENSION_WITCH_DASH_ATTACK_X_EventProcess );
			stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashAttackXEndFuture );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_DASH_ATTACK_X2;
			m_LuaManager.MakeTableReference( "AVSI_DIMENSION_WITCH_DASH_ATTACK_X2", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_DIMENSION_WITCH_DASH_ATTACK_X2_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}

	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_JUMP_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_ATTACK_X", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_DASH_JUMP_ATTACK_X2;
			m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_ATTACK_X2", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackX2FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackX2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackX2EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;


	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_JUMP_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_ATTACK_X", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXEventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_DASH_JUMP_ATTACK_X2;
			m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_ATTACK_X2", stateData.stateID );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackX2FrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackX2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackX2EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;


	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_JUMP_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_AVP_DASH_JUMP_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_DASH_JUMP_X_Init );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_DASH_JUMP_X_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_DASH_JUMP_X_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;

	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_DASH_JUMP_ATTACK_X;
			m_LuaManager.MakeTableReference( "AVSI_ABM_DASH_JUMP_ATTACK_X", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_DASH_JUMP_X_Init );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackXFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_DASH_JUMP_X_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_DASH_JUMP_X_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;

	default:
		break;
	}




	switch( pUnitData->m_UnitClass )
	{
	default:
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_VOID_PRINCESS:
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z4up;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_Z4up", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ4upStart );
			stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ4upFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ4upFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ4upEventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;

	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{
			//stateData.Init();
			//stateData.stateID			= AVSI_COMBO_Z3up;
			//m_LuaManager.MakeTableReference( "AVSI_AEM_ZZupZ", stateData.stateID );
			//stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_ZZupZ_EventProcess );
			//m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z4up;
			m_LuaManager.MakeTableReference( "AVSI_AEM_ZZupZZ", stateData.stateID );
			//stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ4upStart );
			//stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ4upFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_ZZupZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_ZZupZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	}


	switch( pUnitData->m_UnitClass )
	{
	default:
	case CX2Unit::UC_ARME_VIOLET_MAGE:
	case CX2Unit::UC_ARME_HIGH_MAGICIAN:
	case CX2Unit::UC_ARME_DARK_MAGICIAN:
	case CX2Unit::UC_ARME_VOID_PRINCESS:
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3front;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_X3front", stateData.stateID );
			stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3frontStart );
			stateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3frontFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3frontFrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3frontEventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;

	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_X3front;
			m_LuaManager.MakeTableReference( "AVSI_AEM_XXfrontX", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XXfrontX_Init );
			//stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3frontStart );
			//stateData.OnFrameMoveFuture = SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3frontFrameMoveFuture );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XXfrontX_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XXfrontX_EventProcess );
			m_StateList[stateData.stateID] = stateData;


		} break;
	}








	switch( pUnitData->m_UnitClass )
	{
	case CX2Unit::UC_ARME_ELEMENTAL_MASTER:
		{

			stateData.Init();
			stateData.stateID			= AVSI_AEM_XZ;
			m_LuaManager.MakeTableReference( "AVSI_AEM_XZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XZ_Init );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= AVSI_AEM_XZZ;
			m_LuaManager.MakeTableReference( "AVSI_AEM_XZZ", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XZZ_FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_AEM_XZZ_END;
			m_LuaManager.MakeTableReference( "AVSI_AEM_XZZ_END", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AEM_XZZ_END_EventProcess );
			m_StateList[stateData.stateID] = stateData;

		} break;


	case CX2Unit::UC_ARME_VOID_PRINCESS:
		{

			stateData.Init();
			stateData.stateID			= AVSI_AVP_ZZfrontZ;
			m_LuaManager.MakeTableReference( "AVSI_AVP_ZZfrontZ", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_ZZfrontZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;



			stateData.Init();
			stateData.stateID			= AVSI_AVP_ZZfrontZZ;
			m_LuaManager.MakeTableReference( "AVSI_AVP_ZZfrontZZ", stateData.stateID );
			stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_ZZfrontZZ_Init );
			stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_ZZfrontZZ_StateStartFuture );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_ZZfrontZZ_EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= AVSI_AVP_ZZfrontZZ_FALL;
			m_LuaManager.MakeTableReference( "AVSI_AVP_ZZfrontZZ_FALL", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_AVP_ZZfrontZZ_FALL_EventProcess );
			m_StateList[stateData.stateID] = stateData;


		} break;
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_ADW_COMBO_ADD_X;
			m_LuaManager.MakeTableReference( "AVSI_ADW_COMBO_ADD_X", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ADW_COMBO_ADD_X_EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_ADW_COMBO_ADD_X2;
			m_LuaManager.MakeTableReference( "AVSI_ADW_COMBO_ADD_X2", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_ADW_COMBO_ADD_X_EventProcess );
			m_StateList[stateData.stateID] = stateData;
		} break;
	default:
		break;
	}



	InitPassiveSkillState();
	InitEquippedActiveSkillState();


	CX2GUUser::InitState();
}



void CX2GUArme_VioletMage::InitStateID()
{
	m_CommonState.m_Wait	= USI_WAIT;
	m_ChargeMpState			= AVSI_CHARGE_MP;

//#ifdef TRANSFORMER_TEST
//	m_CommonState.m_Transformed			= AVSI_TRANSFORMED;
//#endif TRANSFORMER_TEST




}

void CX2GUArme_VioletMage::InitStateCommon()
{
	UserUnitStateData stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_DIE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DIE_BACK;
	m_LuaManager.MakeTableReference( "AVSI_DIE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DieBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DieBackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DieBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DieBackEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_START_INTRUDE;
	m_LuaManager.MakeTableReference( "AVSI_START_INTRUDE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, StartIntrudeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, StartIntrudeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, StartIntrudeEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= USI_WAIT;
	m_LuaManager.MakeTableReference( "AVSI_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, WaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, WaitStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, WaitFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, WaitEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_WALK;
	m_LuaManager.MakeTableReference( "AVSI_WALK", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser,			WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, WalkEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_READY;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpReadyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_UP;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_UP", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_DOWN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_JUMP_LEVITATION;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_LEVITATION", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpLevitationStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpLevitationFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, JumpLevitationFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpLevitationEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, JumpLevitationEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH;
	m_LuaManager.MakeTableReference( "AVSI_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_END;
	m_LuaManager.MakeTableReference( "AVSI_DASH_END", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DashEndStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashEndFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP;
	m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_DASH_JUMP_LEVITATION;
	m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_LEVITATION", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpLevitationStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpLevitationFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpLevitationFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpLevitationEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpLevitationEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, JumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_DASH_JUMP_POWER_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_POWER_LANDING", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpPowerLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpPowerLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_HYPER_MODE;
	m_LuaManager.MakeTableReference( "AVSI_HYPER_MODE", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, HyperModeInit );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser,			 HyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, HyperModeFrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, HyperModeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser,			 HyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser,			 HyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser,			 HyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_CHARGE_MP;
	m_LuaManager.MakeTableReference( "AVSI_CHARGE_MP", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPInit );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPEndFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, ChargeMPEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_GROGGY;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_GROGGY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DAMAGE_GROGGY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_SMALL_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageSmallFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_SMALL_BACK;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_SMALL_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageSmallBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_BIG_FRONT", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DamageBigFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageBigFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_BIG_BACK;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_BIG_BACK", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DamageBigBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageBigBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_DOWN_FRONT", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DamageDownFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageDownFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_DOWN_BACK;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_DOWN_BACK", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DamageDownBackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageDownBackEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_STANDUP_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_STANDUP_BACK;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_STANDUP_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageStandUpEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, DamageStandUpEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_SMALL;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_SMALL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DamageAirSmallStartFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirSmallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_DOWN", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_INVINCIBLE;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_DOWN_INVINCIBLE", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirDownInvincibleEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_DOWN_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_DOWN_LANDING", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, DamageAirDownLandingInit );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, DamageAirDownLandingFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DamageAirDownLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FALL;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_FALL", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFallEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_UP;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_UP", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirUpEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_FLY_FRONT", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_AIR_FLY_BACK;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_AIR_FLY_BACK", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageAirFlyEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_DAMAGE_REVENGE;
	m_LuaManager.MakeTableReference( "AVSI_DAMAGE_REVENGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, DamageRevengeStart );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, DamageRevengeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, DamageRevengeEventProcess );
	m_StateList[stateData.stateID] = stateData;





	stateData.Init();
	stateData.stateID			= AVSI_TELEPORT_END;
	m_LuaManager.MakeTableReference( "AVSI_A_AV_TELEPORT_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionEndEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_TELEPORT_STAND_UP_START;
	m_LuaManager.MakeTableReference( "AVSI_TELEPORT_STAND_UP_START", stateData.stateID );
	stateData.StateStart 		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EVASION_STAND_UP_START_Start );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EVASION_STAND_UP_START_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EVASION_STAND_UP_START_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_TELEPORT_STAND_UP_END;
	m_LuaManager.MakeTableReference( "AVSI_TELEPORT_STAND_UP_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EVASION_STAND_UP_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EVASION_STAND_UP_END_EventProcess );
	//stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_EVASION_STAND_UP_END_End );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_TELEPORT_STANDUP_ATTACK;
	m_LuaManager.MakeTableReference( "AVSI_TELEPORT_STANDUP_ATTACK", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionStandupAttackInit );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionStandupAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionStandupAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionStandupAttackEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, EvasionStandupAttackEnd );
	m_StateList[stateData.stateID] = stateData;

	switch( GetUnit()->GetClass() )
	{
	case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
	case CX2Unit::UC_ARME_DIMENSION_WITCH:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z1;
			m_LuaManager.MakeTableReference( "AVSI_ABM_COMBO_Z1", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ABM_ComboZ1EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z2;
			m_LuaManager.MakeTableReference( "AVSI_ABM_COMBO_Z2", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ABM_ComboZ2EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z3;
			m_LuaManager.MakeTableReference( "AVSI_ABM_COMBO_Z3", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ABM_ComboZ3EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z4;
			m_LuaManager.MakeTableReference( "AVSI_ABM_COMBO_Z4", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ABM_ComboZ4EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
		break;
	default:
		{
			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z1;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_Z1", stateData.stateID );
			stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ1FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ1EventProcess );
			m_StateList[stateData.stateID] = stateData;

			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z2;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_Z2", stateData.stateID );
			//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ2FrameMove );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ2EventProcess );
			m_StateList[stateData.stateID] = stateData;


			stateData.Init();
			stateData.stateID			= AVSI_COMBO_Z3;
			m_LuaManager.MakeTableReference( "AVSI_COMBO_Z3", stateData.stateID );
			stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ3EventProcess );
			m_StateList[stateData.stateID] = stateData;
		}
		break;
	}


	stateData.Init();
	stateData.stateID			= AVSI_COMBO_X1;
	m_LuaManager.MakeTableReference( "AVSI_COMBO_X1", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX1EventProcess );
	m_StateList[stateData.stateID] = stateData;






	stateData.Init();
	stateData.stateID			= AVSI_COMBO_Z3up;
	m_LuaManager.MakeTableReference( "AVSI_COMBO_Z3up", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboZ3upEventProcess );
	m_StateList[stateData.stateID] = stateData;






	stateData.Init();
	stateData.stateID			= AVSI_COMBO_X3;
	m_LuaManager.MakeTableReference( "AVSI_COMBO_X3", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX3StateEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();		/// ���̾ �߻� ���� ���� ������Ʈ
	stateData.stateID			= AVSI_COMBO_X4_CHARGE;
	m_LuaManager.MakeTableReference( "AVSI_COMBO_X4_CHARGE", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4ChargeStateStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4ChargeFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4ChargeEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4ChargeStateEnd );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();		/// ���� ���̾ �߻� ������Ʈ
	stateData.stateID			= AVSI_COMBO_X4_CHARGE_FIRE;
	m_LuaManager.MakeTableReference( "AVSI_COMBO_X4_CHARGE_FIRE", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4ChargeFireFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4ChargeFireEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_COMBO_X4;
	m_LuaManager.MakeTableReference( "AVSI_COMBO_X4", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, ComboX4EventProcess );
	m_StateList[stateData.stateID] = stateData;




	stateData.Init();
	stateData.stateID			= AVSI_DASH_COMBO_Z1;
	m_LuaManager.MakeTableReference( "AVSI_DASH_COMBO_Z1", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ1StartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ1FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ1EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_DASH_COMBO_Z2;
	m_LuaManager.MakeTableReference( "AVSI_DASH_COMBO_Z2", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ2StartFuture );
	//stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ2FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashComboZ2EventProcess );
	m_StateList[stateData.stateID] = stateData;




	stateData.Init();
	stateData.stateID			= AVSI_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_ATTACK_Z", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpAttackZFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, JumpAttackZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= AVSI_JUMP_ATTACK_X;
	m_LuaManager.MakeTableReference( "AVSI_JUMP_ATTACK_X", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpAttackXFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, JumpAttackXFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, JumpAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;


	stateData.Init();
	stateData.stateID			= AVSI_DASH_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "AVSI_DASH_JUMP_ATTACK_Z", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackZFrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackZFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DashJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;




	stateData.Init();
	stateData.stateID			= AVSI_WALL_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_WALL_LANDING", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, WallLandingEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, WallLandingEndFuture );
	m_StateList[stateData.stateID] = stateData;





	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_READY;
	m_LuaManager.MakeTableReference( "AVSI_PEPPER_RUN_READY", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_READY_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN;
	m_LuaManager.MakeTableReference( "AVSI_PEPPER_RUN", stateData.stateID );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_END;
	m_LuaManager.MakeTableReference( "AVSI_PEPPER_RUN_END", stateData.stateID );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_FrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_END_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_UP;
	m_LuaManager.MakeTableReference( "AVSI_PEPPER_RUN_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_UP_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_PEPPER_RUN_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "AVSI_PEPPER_RUN_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_StateStartFuture );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_FrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, PEPPER_RUN_JUMP_DOWN_EventProcess );
	m_StateList[stateData.stateID] = stateData;

	stateData.Init();
	stateData.stateID			= USI_THROW_ITEM;
	m_LuaManager.MakeTableReference( "AVSI_THROW_ITEM", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMoveFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_FrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUUser, THROW_ITEM_CameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, THROW_ITEM_EventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, THROW_ITEM_StateEnd );
	m_StateList[stateData.stateID] = stateData;

	//#pragma region AVSI_RIDING_START
	stateData.Init();
	stateData.stateID			= GetRidingStartStateID();
	m_LuaManager.MakeTableReference( "AVSI_RIDING_START", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingStartStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingStartStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingStartEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, StartEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_ON
	stateData.Init();
	stateData.stateID			= USI_RIDING_ON;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_ON", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingOnStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingOnStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOnEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOnEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_OFF
	stateData.Init();
	stateData.stateID			= USI_RIDING_OFF;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_OFF", stateData.stateID );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingOffEventProcess );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, RidingOffEnd );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_WAIT_HABIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT_HABIT;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_WAIT_HABIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitHabitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitHabitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_WAIT
	stateData.Init();
	stateData.stateID			= USI_RIDING_WAIT;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_WAIT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWaitStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWaitStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWaitEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_WALK
	stateData.Init();
	stateData.stateID			= USI_RIDING_WALK;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_WALK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingWalkStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingWalkStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, WalkFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingWalkEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_JUMP_UP
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_UP;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_JUMP_UP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpUpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpUpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpUpEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_JUMP_DOWN
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_DOWN;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_JUMP_DOWN", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpDownStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpDownStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, JumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpDownEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DASH
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DASH", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, DashFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DASH_END
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_END;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DASH_END", stateData.stateID );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashEndStart );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashEndStartFuture );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingDashEndFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashEndEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DASH_JUMP
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DASH_JUMP", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEventProcess );
	stateData.StateEndFuture	= SET_CB_FUNC( CX2GUUser, RidingDashJumpEndFuture );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DASH_JUMP_LANDING
	stateData.Init();
	stateData.stateID			= USI_RIDING_DASH_JUMP_LANDING;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DASH_JUMP_LANDING", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, DashJumpLandingStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDashJumpLandingEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DAMAGE_FRONT
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_FRONT;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DAMAGE_FRONT", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageFrontStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DAMAGE_BACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_DAMAGE_BACK;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DAMAGE_BACK", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingDamageBackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingDamageBackStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingDamageBackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_DIE
	stateData.Init();
	stateData.stateID			= USI_RIDING_DIE;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_DIE", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, DieFrontEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_Z;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackZStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_JUMP_ATTACK_Z
	stateData.Init();
	stateData.stateID			= USI_RIDING_JUMP_ATTACK_Z;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_JUMP_ATTACK_Z", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZStart );
	stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZFrameMoveFuture );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingJumpAttackZEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_ATTACK_X
	stateData.Init();
	stateData.stateID			= USI_RIDING_ATTACK_X;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_ATTACK_X", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingAttackXStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingAttackXStart );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingAttackXEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_SPECIAL_ATTACK
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_ATTACK;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_ATTACK_SPECIAL", stateData.stateID );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStartFuture );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackFrameMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, RidingSpecialAttackEventProcess );
	m_StateList[stateData.stateID] = stateData;
	//#pragma endregion

	//#pragma region AVSI_RIDING_SPECIAL_MOVE
	stateData.Init();
	stateData.stateID			= USI_RIDING_SPECIAL_MOVE;
	m_LuaManager.MakeTableReference( "AVSI_RIDING_SPECIAL_MOVE", stateData.stateID );
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
	m_LuaManager.MakeTableReference( "AVSI_RIDING_HYPER_MODE", stateData.stateID );
	stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, RidingHyperModeInit );
	stateData.StateStart		= SET_CB_FUNC( CX2GUUser,			 RidingHyperModeStart );
	stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, RidingHyperModeFrameMove );
	stateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, RidingHyperModeCameraMove );
	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser,			 RidingHyperModeEventProcess );
	stateData.StateStartFuture	= SET_CB_FUNC( CX2GUUser,			 RidingHyperModeStartFuture );
	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser,			 RidingHyperModeEnd );
	m_StateList[stateData.stateID] = stateData;

//#ifdef PVP_BOSS_COMBAT_TEST
//
//	stateData.Init();
//	stateData.stateID			= AVSI_FROZEN;
//	m_LuaManager.MakeTableReference( "AVSI_FROZEN", stateData.stateID );
//	stateData.StateStart		= SET_CB_FUNC( CX2GUUser, Frozen_StateStart );
//	stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, Frozen_EventProcess );
//	stateData.StateEnd			= SET_CB_FUNC( CX2GUUser, Frozen_StateEnd );
//	m_StateList[stateData.stateID] = stateData;
//
//	m_FrozenState = AVSI_FROZEN;
//
//#endif PVP_BOSS_COMBAT_TEST



//#ifdef TRANSFORMER_TEST
//
//	stateData.Init();
//	stateData.stateID			= AVSI_TRANSFORMED;
//	m_LuaManager.MakeTableReference( "AVSI_TRANSFORMED", stateData.stateID );
//	m_StateList[stateData.stateID] = stateData;
//
//
//#endif TRANSFORMER_TEST


}






void CX2GUArme_VioletMage::InitEquippedActiveSkillState(bool bOnlySkillLevel /* = false  */)
{
	CX2GUUser::InitEquippedActiveSkillState(bOnlySkillLevel);

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AHM_BLAZE_STEP );
	if( NULL != pSkillTemplet )
	{
		if ( NULL == GetUnit() )
			return;

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		[[maybe_unused]] const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_AHM_BLAZE_STEP ) );	/// ��ų ����
	}
}

/*virtual*/ void CX2GUArme_VioletMage::InitEtc()
{
	CX2GUUser::InitEtc();

// #ifdef SERV_ADD_ARME_BATTLE_MAGICIAN
// 	m_TransformWeapon = CX2ItemManager::CreateDummyEquip( 40930, GetUnit(), m_pXSkinAnim );
// 	m_TransformWeapon->SetManagerObject( this );
// 	m_TransformWeapon->SetShowObject(false);
//
// 	m_pOrgMotion = m_pMotion;
// #endif
}





/*virtual*/ void CX2GUArme_VioletMage::InitPassiveSkillState()
{
	CX2GUUser::InitPassiveSkillState();
	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	// ���� ȸ���� ���� �� �ʱ�ȭ.
	SetAdditionalMPChangeRateByPassive(0.f);

	m_fMagicStaffSize = 0.f;

	CX2SkillTree* pSkillTree = g_pData->GetSkillTree();

	int iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_SPRINTER, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AVP_SPRINTER );

		if( NULL != pSkillTemplet )
		{
			const float fMoveSpeed = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MOVE_SPEED, iSkillLevel );


			m_AdditionalWalkSpeed.SetPassiveMultiple( m_AdditionalWalkSpeed.GetPassiveMultiple() * fMoveSpeed );
			m_AdditionalDashSpeed.SetPassiveMultiple( m_AdditionalDashSpeed.GetPassiveMultiple() * fMoveSpeed );


		}
	}


	m_fMPConsumeRateFireBall = 1.f;
	m_fFireBallRangeRate = 1.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AV_FIREBALL_UP, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AV_FIREBALL_UP );

		if( NULL != pSkillTemplet )
		{
			m_fMPConsumeRateFireBall *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillLevel );
			m_fFireBallRangeRate *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillLevel );
		}
	}

	const CX2SkillTree::SkillTemplet* pSkillTemplet = NULL;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AEM_ELEMENTAL_FRIENDSHIP, true );
	if( iSkillLevel > 0 )
	{
		pSkillTemplet = userSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_AEM_ELEMENTAL_FRIENDSHIP );
		if( NULL != pSkillTemplet )
		{
			const float fAddMpChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_REGENERATION_ABS, iSkillLevel );
			SetAdditionalMPChangeRateByPassive( fAddMpChangeRate );
			if ( GetNowStateID() != AVSI_CHARGE_MP )
				ResetMPChangeRate( GetOriginalMPChangeRate());

			m_bElementalFriendShip = true;
		}
	}

	pSkillTemplet = userSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_AVP_STRONG_MIND );
	if( NULL != pSkillTemplet )
	{
		if( NULL == m_pStrongMindData )
		{
			m_pStrongMindData = new StrongMindData;


			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_STRONG_MIND, true ) );	/// ��ų ����

			m_pStrongMindData->m_fActiveSkillRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_STRONG_MIND_ACTIVE_REL, iSkillTempletLevel );
			m_pStrongMindData->m_fSpecialActiveSkillRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_STRONG_MIND_SPECIAL_ACTIVE_REL, iSkillTempletLevel );
			m_pStrongMindData->m_fMPRestoreRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_RETAKE_ABS, iSkillTempletLevel );

		}
	}

	m_iAdvancedTeleportationLv = 0;

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADW_ADVANCED_TELEPORTATION, true );
	if( iSkillLevel > 0 )
	{
		pSkillTemplet = userSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_ADW_ADVANCED_TELEPORTATION );
		if( NULL != pSkillTemplet )
		{
			m_iAdvancedTeleportationLv = iSkillLevel;
		}
	}


	m_fMagicStaffExtraRate = 0.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADW_MAGICAL_STAFF, true );

	if( iSkillLevel > 0 )
	{
		pSkillTemplet = userSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_ADW_MAGICAL_STAFF );
		if( NULL != pSkillTemplet )
		{
			m_fMagicStaffExtraRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_RATE, iSkillLevel );
			SetHitCreateDamageEffectRate( m_fMagicStaffExtraRate );
			SetHitCreateDamageEffectPowerRate( pSkillTemplet->GetSkillPowerRateValue( iSkillLevel ) );
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ABM_MAGICAL_MAKEUP );
	if( iSkillLevel > 0 )
	{
		m_fMagicStaffSize = 1.f; //pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS );
	}



	// ���� ����
	m_bIceBall = false;
	m_fIceBallPower = 1.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AHM_FREEZING_POINT_RESEARCH, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AHM_FREEZING_POINT_RESEARCH );
		if( NULL != pSkillTemplet )
		{
			m_bIceBall = true;
			m_fIceBallPower *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_REL, iSkillLevel );
		}
	}

	// ����� �׸���
	m_fShadowOfDisasterRate = 0.f;
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADM_SHADOW_OF_DISASTER, true );
	if( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_ADM_SHADOW_OF_DISASTER );
		if( NULL != pSkillTemplet )
		{
			m_fShadowOfDisasterRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_CRITICAL_RATE_ABS, iSkillLevel );
		}
	}

	// ������ ����
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AEM_ELEMENTAL_RESEARCH, true );
	m_fElementalResearchRate = 0.f;
	if ( 0 < iSkillLevel )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AEM_ELEMENTAL_RESEARCH );

		if ( NULL != pSkillTemplet )
		{
			m_fElementalResearchRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_DECREASE_REL, iSkillLevel );
			const float fAddMpChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_REGENERATION_ABS, iSkillLevel );
			SetAdditionalMPChangeRateByPassive( fAddMpChangeRate );
			if ( GetNowStateID() != AVSI_CHARGE_MP )
				ResetMPChangeRate( GetOriginalMPChangeRate());
		}
	}

	// �׶��Ƽ �� ����
	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_GRAVITY_BALL_RESEARCH, true );
	m_bResearchGravityBall = false;
	m_fGravityBallPower = 1.f;
	if ( 0 < iSkillLevel )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = pSkillTree->GetSkillTemplet( CX2SkillTree::SI_P_AVP_GRAVITY_BALL_RESEARCH );

		if ( NULL != pSkillTemplet )
		{
			m_bResearchGravityBall = true;
			m_fGravityBallPower *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_ATK_REL, iSkillLevel );
		}
	}

	iSkillLevel = userSkillTree.GetSkillLevel( CX2SkillTree::SI_SA_ABM_SUPER_NOVA );
	if( iSkillLevel > 0 )
	{
		m_fMagicStaffSize = 1.f; //pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SIZE_ABS );
	}
}



/*virtual*/ void CX2GUArme_VioletMage::SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ )
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
			m_LuaManager.MakeTableReference( "AVSI_SI_AV_COMMON_BUFF", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, COMMON_BUFF_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_BUFF_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_AV_COMMON_BUFF_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, COMMON_BUFF_FrameMove);
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
					m_LuaManager.MakeTableReference( "AVSI_THROW_ITEM", normalStateData.stateID );
					normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_FrameMove );
					normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, COMMON_RELATIONSHIP_SKILL_EventProcess );

					m_LuaManager.MakeTableReference( "AVSI_THROW_ITEM", hyperStateData.stateID );
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
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_AV_POWER_ATTACK1", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_AV_POWER_ATTACK_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_AV_POWER_ATTACK_EventProcess );
			m_LuaManager.MakeTableReference( "AVSI_SI_AV_POWER_ATTACK1", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_AV_POWER_ATTACK_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_AV_POWER_ATTACK_HYPER_EventProcess );
		} break;

		case CX2SkillTree::SI_A_AV_CHAIN_FIRE_BALL:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_CHAIN_FIRE_BALL", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_FIRE_BALL_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_FIRE_BALL_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_FIRE_BALL_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_FrameMove );
			hyperStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_CameraMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_EventProcess );
		} break;

		case CX2SkillTree::SI_A_AV_BINDING_CIRCLE:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_BINDING_CIRCLE", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_End );

			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_BINDING_CIRCLE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_HYPER_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_BINDING_CIRCLE_HYPER_End );

		} break;



	case CX2SkillTree::SI_SA_AV_GUST_SCREW:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_GUST_SCREW", normalStateData.stateID );
			normalStateData.StateInit				= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_Init );
			normalStateData.OnFrameMoveFuture		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_FrameMoveFuture );
			normalStateData.OnFrameMove				= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_FrameMove );
			normalStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_GUST_SCREW_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUST_SCREW_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_AV_LIGHTNING_BOLT:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_LIGHTNING_BOLT", normalStateData.stateID );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_End );

			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_LIGHTNING_BOLT_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_End );

		} break;

	case CX2SkillTree::SI_SA_AV_GUST_STORM:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI3_AV_GUST_STORM", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI3_AV_GUST_STORM_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_GUST_STORM_HYPER_EventProcess );
		} break;


	case CX2SkillTree::SI_A_AEM_CHAIN_BURST:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_CHAIN_BURST", normalStateData.stateID );
			//normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_BURST_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_BURST_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_BURST_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_CHAIN_BURST_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_BURST_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_CHAIN_BURST_HYPER_EventProcess );
		} break;

	case CX2SkillTree::SI_SA_AHM_MAGIC_MISSILE:		//** Code ���� �̸��� ���Ǵ� �̸��� �ٸ� : �Լ��� ���� �ʿ�
		{
			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_GUIDE_FIRE_BALL", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_EventProcess );
		} break;


		case CX2SkillTree::SI_A_ADM_HELL_STONE:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_HELL_STONE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_FrameMove );
			normalStateData.OnCameraMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_CameraMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI1_AV_HELL_STONE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_HYPER_FrameMove );
			hyperStateData.OnCameraMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_HYPER_CameraMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI1_AV_HELL_STONE_HYPER_EventProcess );
		} break;

	case CX2SkillTree::SI_SA_ADM_DARK_CLOUD:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_DARK_CLOUD", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_DARK_CLOUD_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_DARK_CLOUD_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_DARK_CLOUD_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI2_AV_DARK_CLOUD_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_DARK_CLOUD_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI2_AV_DARK_CLOUD_HYPER_EventProcess );

		} break;

	case CX2SkillTree::SI_SA_ADM_PLASMA_CUTTER:		//** Code ���� �̸��� ���Ǵ� �̸��� �ٸ� : �Լ��� ���� �ʿ�
		{
			m_LuaManager.MakeTableReference( "AVSI_SI3_AV_CATASTROPHE", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_Init );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_FrameMoveFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_End );

			m_LuaManager.MakeTableReference( "AVSI_SI3_AV_CATASTROPHE_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_HYPER_FrameMoveFuture );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_HYPER_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_HYPER_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI3_AV_CATASTROPHE_HYPER_End );

		} break;



	case CX2SkillTree::SI_A_AV_TELEPORT:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_AV_TELEPORT_START", normalStateData.stateID );

			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Teleport_StateStart );
			normalStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Teleport_FrameMoveFuture );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Teleport_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_A_AV_TELEPORT_START", hyperStateData.stateID );

			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Teleport_StateStart );
			hyperStateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Teleport_FrameMoveFuture );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Teleport_EventProcess );

			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_fMPConsumeRateTeleport *= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillTempletLevel );


		} break;


	case CX2SkillTree::SI_A_AV_CIRCLE_FLAME:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_A_AV_CIRCLE_FLAME_READY", normalStateData.stateID );
			normalStateData.StateStartFuture= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_StartFuture );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_EventProcess );
			normalStateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_EndFuture );

			m_LuaManager.MakeTableReference( "AVSI_SI_A_AV_CIRCLE_FLAME_READY", hyperStateData.stateID );
			hyperStateData.StateStartFuture = SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_StartFuture );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_EventProcess );
			hyperStateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_READY_EndFuture );


			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= AVSI_SI_A_AV_CIRCLE_FLAME;
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO7 ) == true )
				{
					m_LuaManager.MakeTableReference( "AVSI_SI_A_AV_CIRCLE_FLAME_MEMO", stateData.stateID );
				}
				else
				{
					m_LuaManager.MakeTableReference( "AVSI_SI_A_AV_CIRCLE_FLAME", stateData.stateID );
				}

				stateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_Init );
				stateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_StartFuture );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_FrameMove );
				stateData.OnFrameMoveFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_FrameMoveFuture );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_EventProcess );
				stateData.StateEndFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_AV_CIRCLE_FLAME_EndFuture );
				m_StateList[stateData.stateID] = stateData;
			}

		} break;





	case CX2SkillTree::SI_SA_AHM_BLIZZARDSHOWER:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_BLIZZARDSHOWER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_StateEnd );


			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_BLIZZARDSHOWER_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_StateEnd );

		} break;

	case CX2SkillTree::SI_SA_ADM_DEATHFIELD:
		{

			m_LuaManager.MakeTableReference( "AVSI_SA_ADM_DEATHFIELD", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_StateEnd );


			m_LuaManager.MakeTableReference( "AVSI_SA_ADM_DEATHFIELD_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_DEATHFIELD_HYPER_StateEnd );


		} break;

	case CX2SkillTree::SI_A_ADM_MANA_INTAKE:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_ADM_MANA_INTAKE", normalStateData.stateID );

			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_EventProcess );


			m_LuaManager.MakeTableReference( "AVSI_A_ADM_MANA_INTAKE_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_HYPER_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADM_MANA_INTAKE_HYPER_EventProcess );
		} break;


	case CX2SkillTree::SI_SA_AHM_BLAZE_STEP:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_BLAZE_STEP", normalStateData.stateID );

			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLAZE_STEP_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLAZE_STEP_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLAZE_STEP_EventProcess );


			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_BLAZE_STEP_HYPER", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLAZE_STEP_HYPER_Init );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLAZE_STEP_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_BLAZE_STEP_HYPER_EventProcess );
		} break;

	case CX2SkillTree::SI_SA_ADM_AGING:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_ADM_AGING", normalStateData.stateID );

			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_AGING_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_AGING_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_ADM_AGING_EventProcess );


			m_LuaManager.MakeTableReference( "AVSI_SA_ADM_AGING_HYPER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;




	case CX2SkillTree::SI_A_AEM_STONE_WALL:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_AEM_STONE_WALL", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AEM_STONE_WALL_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AEM_STONE_WALL_FrameMove );
#ifdef CONVERSION_VS
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
#else
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, GenericActiveSkillEventProcess );
#endif // CONVERSION_VS

			m_LuaManager.MakeTableReference( "AVSI_A_AEM_STONE_WALL", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AEM_STONE_WALL_FrameMove );
#ifdef CONVERSION_VS
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
#else
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, GenericActiveSkillEventProcess );
#endif // CONVERSION_VS

		} break;

		case CX2SkillTree::SI_A_AVP_SUMMON_BAT_TIME_BOMB:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_AVP_SUMMON_BAT", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AVP_SUMMON_BAT_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AVP_SUMMON_BAT_FrameMove );
#ifdef CONVERSION_VS
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
#else
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, GenericActiveSkillEventProcess );
#endif // CONVERSION_VS

			m_LuaManager.MakeTableReference( "AVSI_A_AVP_SUMMON_BAT", hyperStateData.stateID );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AVP_SUMMON_BAT_FrameMove );
#ifdef CONVERSION_VS
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
#else
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, GenericActiveSkillEventProcess );
#endif // CONVERSION_VS

		} break;

		case CX2SkillTree::SI_A_AHM_CYCLONE:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AEM_CYCLONE", normalStateData.stateID );
			normalStateData.m_bHyperState = false;
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_CYCLONE_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_CYCLONE_FrameMove );
#ifdef CONVERSION_VS
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
#else
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, GenericSpecialActiveSkillEventProcess );
#endif // CONVERSION_VS

			m_LuaManager.MakeTableReference( "AVSI_SA_AEM_CYCLONE", hyperStateData.stateID );
			hyperStateData.m_bHyperState = true;
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_CYCLONE_FrameMove );
#ifdef CONVERSION_VS
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
#else
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, GenericSpecialActiveSkillEventProcess );
#endif // CONVERSION_VS

		} break;


	case CX2SkillTree::SI_A_ADM_HELL_DROP:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AVP_HELL_DROP", normalStateData.stateID );
			normalStateData.m_bHyperState = false;
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_Init );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_EventProcess );
			normalStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_StateEnd );

			m_LuaManager.MakeTableReference( "AVSI_SA_AVP_HELL_DROP", hyperStateData.stateID );
			hyperStateData.m_bHyperState = true;
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_Start );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_EventProcess );
			hyperStateData.StateEnd				= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_HELL_DROP_StateEnd );

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_IDLE;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_IDLE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_IDLE_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_LEFT;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_LEFT_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_RIGHT;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_RIGHT_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_UP;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_UP", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_UP_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_DOWN;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_DOWN", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_DOWN_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_UP_LEFT;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_UP_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_UP_LEFT_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_UP_RIGHT;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_UP_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_UP_RIGHT_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_DOWN_LEFT;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_DOWN_LEFT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_DOWN_LEFT_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_DOWN_RIGHT;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_DOWN_RIGHT", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_DOWN_RIGHT_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_FIRE;

				// �� ���, �߻� ���� �ٷ� ��� ���� �߻� �ð� ������ ������� ���濡 ����
				// State �Լ��� ����
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_FIRE", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FIRE_StateStart );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HELL_DROP_CONTROL_FIRE_EventProcess );

				m_StateList[stateData.stateID] = stateData;


				// �� ��� Dying State �߰�
				stateData.Init();
				stateData.stateID			= AVSI_HELL_DROP_CONTROL_DYING;
				m_LuaManager.MakeTableReference( "AVSI_HELL_DROP_CONTROL_DYING", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
				m_StateList[stateData.stateID] = stateData;
			}

		} break;


		// ����� 2010.11.1   : ���׿� ����
	case CX2SkillTree::SI_SA_AEM_METEOR_SHOWER:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_METEOSHOWER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_StateEnd );


			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_METEOSHOWER_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;

			{
				UserUnitStateData stateData;

				/// �Ϲ�
				stateData.Init();
				stateData.stateID			= AVSI_SA_AHM_METEOSHOWER_LOOP;
				m_LuaManager.MakeTableReference( "AVSI_SA_AHM_METEOSHOWER_LOOP", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				/// ����
				stateData.Init();
				stateData.stateID			= AVSI_SA_AHM_METEOSHOWER_HYPER_LOOP;
				m_LuaManager.MakeTableReference( "AVSI_SA_AHM_METEOSHOWER_LOOP", stateData.stateID );
				stateData.m_bHyperState		= true;
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_LOOP_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				/// ����
				stateData.Init();
				stateData.stateID			= AVSI_SA_AHM_METEOSHOWER_END;
				m_LuaManager.MakeTableReference( "AVSI_SA_AHM_METEOSHOWER_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_END_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_METEOSHOWER_END_StateEnd );
				m_StateList[stateData.stateID] = stateData;
			}

			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_fFireGap					= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGING_TIME, iSkillTempletLevel );			/// �ѹߴ� �߻� ��Ÿ��
			m_iMaxFireCount				= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, iSkillTempletLevel ) );	/// �ִ� �߻緮
			m_iMaxFireCountByLoopState	= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel ) );			/// �ִ� �߻緮( ���� ������Ʈ )
			m_fUseMPConsume				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillTempletLevel );			/// �ѹߴ� �Һ� ����

		} break;

		// ����� 2010.11.14 : ���� �긮��
	case CX2SkillTree::SI_SA_AVP_PHANTOM_BREATHING_DARK_FALL:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AVP_PHANTOM_BREATHING", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_StateEnd );


			m_LuaManager.MakeTableReference( "AVSI_SA_AVP_PHANTOM_BREATHING_HYPER", hyperStateData.stateID );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_EventProcess );
			hyperStateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_StateEnd );
		} break;
		// ����� 2010.11.1   : ���׿� ����



	case CX2SkillTree::SI_A_AV_FIRE_ROAD:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_AV_FIRE_ROAD", normalStateData.stateID );
			normalStateData.StateStart			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_FIRE_ROAD_Start );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_FIRE_ROAD_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_FIRE_ROAD_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_A_AV_FIRE_ROAD", hyperStateData.stateID );
			hyperStateData.StateStart			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_FIRE_ROAD_Start );
			hyperStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_FIRE_ROAD_FrameMove );
			hyperStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_FIRE_ROAD_EventProcess );
		} break;
	case CX2SkillTree::SI_A_ABM_ENERGY_DRAIN:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_ABM_ENERGY_DRAIN_READY", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_READY_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_READY_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_A_ABM_ENERGY_DRAIN_READY", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_READY_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_READY_EventProcess );

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= AVSI_A_ABM_ENERGY_DRAIN_SUCCESS;
				m_LuaManager.MakeTableReference( "AVSI_A_ABM_ENERGY_DRAIN_SUCCESS", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_SUCCESS_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_A_ABM_ENERGY_DRAIN_FAIL;
				m_LuaManager.MakeTableReference( "AVSI_A_ABM_ENERGY_DRAIN_FAIL", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_ENERGY_DRAIN_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
		case CX2SkillTree::SI_A_ADW_HEAVY_PRESS:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_HEAVY_PRESS", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_HEAVY_PRESS_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_HEAVY_PRESS", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_HEAVY_PRESS_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
		}
		break;
	case CX2SkillTree::SI_SA_ABM_ENERGY_SPURT:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_ENERGY_SPURT", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_ENERGY_SPURT_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_ENERGY_SPURT", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_ENERGY_SPURT_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
		}
		break;
	case CX2SkillTree::SI_SA_ABM_GUILLOTINE_PRESS:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_GUILLOTINE_PRESS_START", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_GUILLOTINE_PRESS_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_GUILLOTINE_PRESS_START_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_GUILLOTINE_PRESS_START", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_GUILLOTINE_PRESS_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_GUILLOTINE_PRESS_START_EventProcess );

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP;
				m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_SI_SA_ABM_GUILLOTINE_PRESS_END;
				m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_GUILLOTINE_PRESS_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	case CX2SkillTree::SI_SA_ABM_SUPER_NOVA:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_SUPER_NOVA", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_SUPER_NOVA_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_SUPER_NOVA", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_SUPER_NOVA_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
		}
		break;
	case CX2SkillTree::SI_SA_ABM_MAGICAL_MAKEUP:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_MAGICAL_MAKEUP", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_MAGICAL_MAKEUP_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_MAGICAL_MAKEUP_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_MAGICAL_MAKEUPR_StateEnd );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ABM_MAGICAL_MAKEUP", hyperStateData.stateID );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_MAGICAL_MAKEUP_Init );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_MAGICAL_MAKEUP_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
			hyperStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ABM_MAGICAL_MAKEUPR_StateEnd );
		}
		break;

	case CX2SkillTree::SI_A_ADW_DISTORTION: // ���� �ְ�
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_A_ADW_DISTORTION", normalStateData.stateID );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_ADW_DISTORTION_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_ADW_DISTORTION_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_ADW_DISTORTION_FrameMove );

			m_LuaManager.MakeTableReference( "AVSI_SI_A_ADW_DISTORTION", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_ADW_DISTORTION_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_ADW_DISTORTION_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_A_ADW_DISTORTION_FrameMove );
		}
		break;
		case CX2SkillTree::SI_A_ADW_IMPACT_HAMMER:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ADW_MORNING_STAR", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ADW_MORNING_STAR_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ADW_MORNING_STAR", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ADW_MORNING_STAR_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
		}
		break;
		case CX2SkillTree::SI_SA_ADW_SCREWDRIVER_TORNADO:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ADW_SCREWDRIVER", normalStateData.stateID );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ADW_SCREWDRIVER_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ADW_SCREWDRIVER", hyperStateData.stateID );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ADW_SCREWDRIVER_FrameMove );
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
		}
		break;
	//#pragma region SI_A_AV_ICICLE_WAVE
	case CX2SkillTree::SI_A_AV_ICICLE_WAVE:
		{
			m_LuaManager.MakeTableReference( "SI_A_AV_ICICLE_WAVE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Icicle_Wave_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Icicle_Wave_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AV_Icicle_Wave_EventProcess );

			m_LuaManager.MakeTableReference( "SI_A_AV_ICICLE_WAVE", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ���̽�Ŭ ���̺�

	//#pragma region SI_SA_AV_ICE_STORM
	case CX2SkillTree::SI_SA_AV_ICE_STORM:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AV_ICE_STORM", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AV_Ice_Storm_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AV_Ice_Storm_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AV_Ice_Storm_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SA_AV_ICE_STORM", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ���̽� ����

	//#pragma region SI_A_AHM_AQUA_SHOWER
	case CX2SkillTree::SI_A_AEM_AQUA_SHOWER:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_AHM_AQUA_SHOWER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AHM_Aqua_Shower_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AHM_Aqua_Shower_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AHM_Aqua_Shower_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_A_AHM_AQUA_SHOWER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ������ ����

	//#pragma region SI_SA_AHM_LIGHTNING_SHOWER
	case CX2SkillTree::SI_SA_AEM_LIGHTNING_SHOWER:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_AHM_LIGHTNING_SHOWER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_EventProcess );
			normalStateData.StateEnd		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_StateEnd );


			m_LuaManager.MakeTableReference( "AVSI_SA_AHM_AHM_LIGHTNING_SHOWER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
			hyperStateData.StateEnd			= normalStateData.StateEnd;

			{
				UserUnitStateData stateData;

				/// �Ϲ�
				stateData.Init();
				stateData.stateID			= AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP;
				m_LuaManager.MakeTableReference( "AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP", stateData.stateID );
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				/// ����
				stateData.Init();
				stateData.stateID			= AVSI_SA_AHM_LIGHTNING_SHOWER_HYPER_LOOP;
				m_LuaManager.MakeTableReference( "AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP", stateData.stateID );
				stateData.m_bHyperState		= true;
				stateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_StateStart );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_Loop_StateEnd );
				m_StateList[stateData.stateID] = stateData;

				/// ����
				stateData.Init();
				stateData.stateID			= AVSI_SA_AHM_LIGHTNING_SHOWER_END;
				m_LuaManager.MakeTableReference( "AVSI_SA_AHM_LIGHTNING_SHOWER_END", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_End_EventProcess );
				stateData.StateEnd			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AHM_Lightning_Shower_End_StateEnd );
				m_StateList[stateData.stateID] = stateData;
			}

			if ( NULL == GetUnit() )
			return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			m_fFireGap					= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_CHARGING_TIME, iSkillTempletLevel );								/// �ѹߴ� �߻� ��Ÿ��
			m_iMaxFireCount				= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, iSkillTempletLevel ) );	/// �ִ� �߻緮
			m_iMaxFireCountByLoopState	= static_cast<int>( pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel ) );			/// �ִ� �߻緮( ���� ������Ʈ )
			m_fUseMPConsume				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_CONSUME_REL, iSkillTempletLevel );								/// �ѹߴ� �Һ� ����
		} break;
		//#pragma endregion ����Ʈ�� ����

	//#pragma region SI_SA_AEM_CHAIN_LIGHTNING
	case CX2SkillTree::SI_SA_AHM_CHAIN_LIGHTNING:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AEM_CHAIN_LIGHTNING", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_Chain_Lightning_Init );
			normalStateData.StateStartFuture	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_Chain_Lightning_StartFuture );
			normalStateData.StateEndFuture		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_Chain_Lightning_EndFuture );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_Chain_Lightning_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AEM_Chain_Lightning_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SA_AEM_CHAIN_LIGHTNING", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.StateStartFuture		= normalStateData.StateStartFuture;
			hyperStateData.StateEndFuture		= normalStateData.StateEndFuture;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ü�� ����Ʈ��

	//#pragma region SI_A_AVP_SUMMON_BAT_HEAVY
	case CX2SkillTree::SI_A_AVP_SUMMON_BAT_HEAVY_DOLL:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_AVP_SUMMON_BAT_HEAVY", normalStateData.stateID );
			normalStateData.StateInit			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AVP_Summon_Bat_Heavy_Init );
			normalStateData.OnFrameMove			= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_AVP_Summon_Bat_Heavy_FrameMove );
			normalStateData.OnEventProcess		= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_A_AVP_SUMMON_BAT_HEAVY", hyperStateData.stateID );
			hyperStateData.m_bHyperState		= true;
			hyperStateData.OnFrameMove			= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess		= normalStateData.OnEventProcess;

		} break;
	//#pragma endregion �ڶ� �Ӱ��� - ��� ��

	//#pragma region SI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE
	case CX2SkillTree::SI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE:
		{
			m_LuaManager.MakeTableReference( "AVSI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_EventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.StateStart		= normalStateData.StateStart;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= normalStateData.OnEventProcess;
		} break;
	//#pragma endregion ���� �긮�� - ��ũ Ȧ

	//#pragma region SI_A_ABM_VITAL_DRAIN
	case CX2SkillTree::SI_A_ABM_VITAL_DRAIN:
		{
			m_LuaManager.MakeTableReference( "AVSI_A_ABM_VITAL_DRAIN_READY", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_Vital_Drain_Ready_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_Vital_Drain_Ready_FrameMove );

			m_LuaManager.MakeTableReference( "AVSI_A_ABM_VITAL_DRAIN_READY", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;

			{
				UserUnitStateData stateData;

				stateData.Init();
				stateData.stateID			= AVSI_A_ABM_VITAL_DRAIN_SUCCESS;
				m_LuaManager.MakeTableReference( "AVSI_A_ABM_VITAL_DRAIN_SUCCESS", stateData.stateID );
				stateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_Vital_Drain_Success_FrameMove );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_Vital_Drain_EventProcess );
				m_StateList[stateData.stateID] = stateData;

				stateData.Init();
				stateData.stateID			= AVSI_A_ABM_VITAL_DRAIN_FAIL;
				m_LuaManager.MakeTableReference( "AVSI_A_ABM_VITAL_DRAIN_FAIL", stateData.stateID );
				stateData.OnEventProcess	= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ABM_Vital_Drain_EventProcess );
				m_StateList[stateData.stateID] = stateData;
			}
		} break;
	//#pragma endregion ����Ż �巹��

	//#pragma region SI_A_ADW_WORM_HOLE
	case CX2SkillTree::SI_A_ADW_WORM_HOLE:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_A_ADW_WORM_HOLE", normalStateData.stateID );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADW_Worm_Hole_Init );
			normalStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADW_Worm_Hole_StateStart );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADW_Worm_Hole_FrameMove );

			m_LuaManager.MakeTableReference( "AVSI_SI_A_ADW_WORM_HOLE", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
			hyperStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADW_Worm_Hole_Init );
			hyperStateData.StateStart		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADW_Worm_Hole_StateStart );
			hyperStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_A_ADW_Worm_Hole_FrameMove );
		} break;
	//#pragma endregion ��Ȧ

	//#pragma region SI_SA_ADW_SCREWDRIVER_DRILLER
	case CX2SkillTree::SI_SA_ADW_SCREWDRIVER_DRILLER:
		{
			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ADW_SCREWDRIVER_DRILLER", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ADW_Screwdriver_Driller_Init );
			normalStateData.OnFrameMove		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_SI_SA_ADW_Screwdriver_Driller_FrameMove );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_SI_SA_ADW_SCREWDRIVER_DRILLER", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnFrameMove		= normalStateData.OnFrameMove;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericActiveSkillEventProcess );
		}
		break;

	//#pragma endregion ��ũ�� ����̹� - �帱��

	//#pragma region SI_SA_AEM_Elemental_Storm
	case CX2SkillTree::SI_HA_AEM_ELEMENTAL_STORM:
		{
			m_LuaManager.MakeTableReference( "AVSI_HA_AEM_ELEMENTAL_STORM", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HA_AEM_Elemental_Storm_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_HA_AEM_ELEMENTAL_STORM", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
		} break;
	//#pragma endregion ������Ż ���� : �ñر�

	//#pragma region SI_FS_AVP_Abyss_Angor
	case CX2SkillTree::SI_HA_AVP_ABYSS_ANGOR:
		{
			m_LuaManager.MakeTableReference( "AVSI_HA_AVP_ABYSS_ANGOR", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HA_AVP_Abyss_Angor_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_HA_AVP_ABYSS_ANGOR", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
		} break;
	//#pragma endregion ��� �Ӱ��� : �ñر�

	//#pragma region SI_FS_ADW_Fate_space
		case CX2SkillTree::SI_HA_ADW_FATE_SPACE:
		{
			m_LuaManager.MakeTableReference( "AVSI_HA_ADW_FATE_SPACE", normalStateData.stateID );
			normalStateData.StateInit		= SET_CB_FUNC( CX2GUArme_VioletMage, AVSI_HA_ADW_Fate_space_Init );
			normalStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );

			m_LuaManager.MakeTableReference( "AVSI_HA_ADW_FATE_SPACE", hyperStateData.stateID );
			hyperStateData.m_bHyperState	= true;
			hyperStateData.OnEventProcess	= SET_CB_FUNC( CX2GUUser, GenericSpecialActiveSkillEventProcess );
		} break;
	//#pragma endregion ����Ʈ �����̽� : �ñر�

	}
}

/*virtual*/ void CX2GUArme_VioletMage::SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ )
{
	if ( NULL == GetUnit() )
		return;

	const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AV_CIRCLE_FLAME == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AV_CIRCLE_FLAME );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AV_CIRCLE_FLAME ) );	/// ��ų ����

			UserUnitStateData& stateData = m_StateList[ AVSI_SI_A_AV_CIRCLE_FLAME ];
			stateData.m_SPLevel		= iSkillTempletLevel;
			stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			stateData.m_eSkillID	= pSkillTemplet->m_eID;
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_AHM_MEDITATION == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AHM_MEDITATION );
		if( NULL != pSkillTemplet )
		{
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AHM_MEDITATION ) );	/// ��ų ����

			m_fMeditationMPChangeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_REGENERATION_ABS, iSkillTempletLevel );
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ABM_ENERGY_DRAIN == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_ENERGY_DRAIN );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( CX2SkillTree::SI_A_ABM_ENERGY_DRAIN ) );	/// ��ų ����

			UserUnitStateData& stateData = m_StateList[ AVSI_A_ABM_ENERGY_DRAIN_SUCCESS ];
			stateData.m_SPLevel		= iSkillTempletLevel;
			stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			stateData.m_eSkillID	= pSkillTemplet->m_eID;
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_ABM_GUILLOTINE_PRESS == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ABM_GUILLOTINE_PRESS );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			UserUnitStateData& loopStateData = m_StateList[ AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP ];
			loopStateData.m_SPLevel		= iSkillTempletLevel;
			loopStateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			loopStateData.m_eSkillID	= pSkillTemplet->m_eID;

			UserUnitStateData& endStateData = m_StateList[ AVSI_SI_SA_ABM_GUILLOTINE_PRESS_END ];
			endStateData.m_SPLevel		= iSkillTempletLevel;
			endStateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			endStateData.m_eSkillID	= pSkillTemplet->m_eID;
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ABM_VITAL_DRAIN == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_VITAL_DRAIN );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			UserUnitStateData& stateData = m_StateList[ AVSI_A_ABM_VITAL_DRAIN_SUCCESS ];
			stateData.m_SPLevel		= iSkillTempletLevel;
			stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
			stateData.m_eSkillID	= pSkillTemplet->m_eID;
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AEM_METEOR_SHOWER == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AEM_METEOR_SHOWER );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_SA_AHM_METEOSHOWER_LOOP ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_SA_AHM_METEOSHOWER_HYPER_LOOP ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}
		}
	}

	if ( true == bChangeAll_ || CX2SkillTree::SI_SA_AEM_LIGHTNING_SHOWER == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AEM_LIGHTNING_SHOWER );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_SA_AHM_LIGHTNING_SHOWER_HYPER_LOOP ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}
		}
	}

	// �� ��� ���� State �߰�
	if ( true == bChangeAll_ || CX2SkillTree::SI_A_ADM_HELL_DROP == eSkillID_ )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADM_HELL_DROP );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_IDLE ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_LEFT ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_RIGHT ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_UP ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_DOWN ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_UP_LEFT ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_UP_RIGHT ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_DOWN_LEFT ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_DOWN_RIGHT ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}

			{
				UserUnitStateData& stateData = m_StateList[ AVSI_HELL_DROP_CONTROL_FIRE ];
				stateData.m_SPLevel		= iSkillTempletLevel;
				stateData.m_fPowerRate	= pSkillTemplet->GetSkillPowerRateValue( iSkillTempletLevel );
				stateData.m_eSkillID	= pSkillTemplet->m_eID;
			}
		}
	}
}

/*static*/
void	CX2GUArme_VioletMage::AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ )
{
	g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Arme_ComboX3Up-1.X" );
	g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Arme_ComboX3Up-2.X" );
	g_pKTDXApp->GetDeviceManager()->AppendXSkinMesh( listInOut_, L"Arme_ComboX3Up-3.X" );
}


void CX2GUArme_VioletMage::InitDevice()
{
	CX2GUUser::InitDevice();

	CKTDXDeviceXSkinMesh* pMesh1 = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Arme_ComboX3Up-1.X" );
	CKTDXDeviceXSkinMesh* pMesh2 = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Arme_ComboX3Up-2.X" );
	CKTDXDeviceXSkinMesh* pMesh3 = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Arme_ComboX3Up-3.X" );
	DeviceReady( pMesh1 );
	DeviceReady( pMesh2 );
	DeviceReady( pMesh3 );

	m_pComboX3Up1			= CKTDGXSkinAnim::CreateSkinAnim();
	m_pComboX3Up2			= CKTDGXSkinAnim::CreateSkinAnim();
	m_pComboX3Up3			= CKTDGXSkinAnim::CreateSkinAnim();

	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pComboX3Up1 );
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pComboX3Up2 );
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pComboX3Up3 );

	m_pComboX3Up1->SetAnimXSkinMesh( pMesh1 );
	m_pComboX3Up1->AddModelXSkinMesh( pMesh1 );
	m_pComboX3Up2->SetAnimXSkinMesh( pMesh2 );
	m_pComboX3Up2->AddModelXSkinMesh( pMesh2 );
	m_pComboX3Up3->SetAnimXSkinMesh( pMesh3 );
	m_pComboX3Up3->AddModelXSkinMesh( pMesh3 );

	m_pComboX3Up1->SetAlphaObject( true );
	m_pComboX3Up1->SetLayer( XL_EFFECT_0 );
	m_pComboX3Up2->SetAlphaObject( true );
	m_pComboX3Up2->SetLayer( XL_EFFECT_0 );
	m_pComboX3Up3->SetAlphaObject( true );
	m_pComboX3Up3->SetLayer( XL_EFFECT_0 );

	m_pComboX3Up1->GetRenderParam()->bAlphaBlend	= true;
	//m_pComboX3Up1->GetRenderParam()->bZEnable		= false;
	m_pComboX3Up1->GetRenderParam()->bZWriteEnable	= false;
	m_pComboX3Up1->GetRenderParam()->cullMode		= D3DCULL_NONE;
	m_pComboX3Up1->GetRenderParam()->srcBlend		= D3DBLEND_SRCALPHA;
	m_pComboX3Up1->GetRenderParam()->destBlend		= D3DBLEND_DESTALPHA;

	m_pComboX3Up2->GetRenderParam()->bAlphaBlend	= true;
	//m_pComboX3Up2->GetRenderParam()->bZEnable		= false;
	m_pComboX3Up2->GetRenderParam()->bZWriteEnable	= false;
	m_pComboX3Up2->GetRenderParam()->cullMode		= D3DCULL_NONE;
	m_pComboX3Up2->GetRenderParam()->srcBlend		= D3DBLEND_SRCALPHA;
	m_pComboX3Up2->GetRenderParam()->destBlend		= D3DBLEND_DESTALPHA;

	m_pComboX3Up3->GetRenderParam()->bAlphaBlend	= true;
	//m_pComboX3Up3->GetRenderParam()->bZEnable		= false;
	m_pComboX3Up3->GetRenderParam()->bZWriteEnable	= false;
	m_pComboX3Up3->GetRenderParam()->cullMode		= D3DCULL_NONE;
	m_pComboX3Up3->GetRenderParam()->srcBlend		= D3DBLEND_SRCALPHA;
	m_pComboX3Up3->GetRenderParam()->destBlend		= D3DBLEND_DESTALPHA;


}

void CX2GUArme_VioletMage::CommonStateStartFuture()
{
	CX2GUUser::CommonStateStartFuture();
}

void CX2GUArme_VioletMage::CommonStateStart()
{
	switch ( GetNowStateID() )
	{
		case AVSI_HELL_DROP_CONTROL_IDLE :
		case AVSI_HELL_DROP_CONTROL_LEFT :
		case AVSI_HELL_DROP_CONTROL_RIGHT :
		case AVSI_HELL_DROP_CONTROL_UP :
		case AVSI_HELL_DROP_CONTROL_DOWN :
		case AVSI_HELL_DROP_CONTROL_UP_LEFT :
		case AVSI_HELL_DROP_CONTROL_UP_RIGHT :
		case AVSI_HELL_DROP_CONTROL_DOWN_LEFT :
		case AVSI_HELL_DROP_CONTROL_DOWN_RIGHT :
			if ( false == m_bIsHellDropTimerStart )
			{
				// �� ��� ���� �� ������ ������ ���, Ÿ�̸Ӱ� ������ �ȵǾ� �ִ�.
				// ��, �ش� State �ε� Ÿ�̸� ������ �ȵ��ִ� ��쿡�� Ÿ�̸Ӹ� ���ְ�
				// ����Ʈ�� �ٷ� �����ϰ� �Ѵ�.
				m_TimerUFOControl.restart();
				m_bIsHellDropTimerStart = true;
				m_fHellDropCreateEffectCooltime = _CONST_AISHA_::CREATE_HELL_DROP_DAMAGE_EFFECT_START_TIME;
			}
			break;
		default:
			break;
	}

	CX2GUUser::CommonStateStart();
}

void CX2GUArme_VioletMage::CommonFrameMoveFuture()
{
	CX2GUUser::CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::CommonFrameMove()
{
	CX2GUUser::CommonFrameMove();




	if( NULL != m_pStrongMindData )
	{
		m_pStrongMindData->OnFrameMove( m_fTime, m_fElapsedTime );
	}



	for( int i=0; i< (int)m_vecStoneWallData.size(); i++ )
	{
		StoneWallData& stoneWallData = m_vecStoneWallData[i];
		stoneWallData.OnFrameMove( m_fTime, m_fElapsedTime );

		if( stoneWallData.m_fTimeLeft <= 0.f )
		{
			if( NULL != g_pX2Game )
			{
				CX2GUNPC* pNPC = g_pX2Game->GetNPCUnitByUID( (int) stoneWallData.m_NPCUID );

				if( NULL != pNPC )
				{
					pNPC->SetNowHp( 0.f );
				}


			}

			m_vecStoneWallData.erase( m_vecStoneWallData.begin() + i );
			--i;
		}
	}

	//*
	if(
		GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO21) == false &&
		NULL != GetUFO() )
	{
		//const double MAGIC_HELL_DROP_CHECK_TIME = 1.0;
		//if( m_TimerCheckInvalidUFO.elapsed() > MAGIC_HELL_DROP_CHECK_TIME )
		{
			//m_TimerCheckInvalidUFO.restart();

			//if( false == m_bIsSummoningUFO )
			{
				switch( GetNowStateID() )
				{
				case AVSI_HELL_DROP_CONTROL_IDLE:
				case AVSI_HELL_DROP_CONTROL_LEFT:
				case AVSI_HELL_DROP_CONTROL_RIGHT:
				case AVSI_HELL_DROP_CONTROL_UP:
				case AVSI_HELL_DROP_CONTROL_DOWN:
				case AVSI_HELL_DROP_CONTROL_UP_LEFT:
				case AVSI_HELL_DROP_CONTROL_UP_RIGHT:
				case AVSI_HELL_DROP_CONTROL_DOWN_LEFT:
				case AVSI_HELL_DROP_CONTROL_DOWN_RIGHT:
				// �� ���, �߻� ���� �ٷ� ��� ���� �߻� �ð� ������ ������� ���濡 ���� ����
				case AVSI_HELL_DROP_CONTROL_FIRE:
				case USI_SPECIAL_ATTACK_1:				/// A������ ù��° ��ų����(�����AŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_HYPER_1:			/// A������ ù��° ��ų����(�����AŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_2:				/// A������ �ι�° ��ų����(�����SŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_HYPER_2:			/// A������ �ι�° ��ų����(�����SŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_3:				/// A������ ����° ��ų����(�����DŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_HYPER_3:			/// A������ ����° ��ų����(�����DŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_4:				/// A������ �׹�° ��ų����(�����CŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_HYPER_4:			/// A������ �׹�° ��ų����(�����CŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_1:			/// B������ ù��° ��ų����(�����AŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_HYPER_1:	/// B������ ù��° ��ų����(�����AŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_2:			/// B������ �ι�° ��ų����(�����SŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_HYPER_2:	/// B������ �ι�° ��ų����(�����SŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_3:			/// B������ ����° ��ų����(�����DŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_HYPER_3:	/// B������ ����° ��ų����(�����DŰ)�� �ش��ϴ� ���� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_4:			/// B������ �׹�° ��ų����(�����CŰ)�� �ش��ϴ� ������Ʈ
				case USI_SPECIAL_ATTACK_SLOTB_HYPER_4:	/// B������ �׹�° ��ų����(�����CŰ)�� �ش��ϴ� ���� ������Ʈ
					{
						// do nothing;
					} break;

				// �� ���, �߻� ���� �ٷ� ��� ���� �߻� �ð� ������ ������� ���濡 ���� ����
				case AVSI_HELL_DROP_CONTROL_DYING:

				default:
					{
						if( NULL != GetUFO()->GetNPCAI() && CX2NPCAI::NAT_ALLY == GetUFO()->GetNPCAI()->GetAIType()
							&& static_cast<CX2AllyNPCAI*>( GetUFO()->GetNPCAI() )->GetAllyUnitUID() != GetUnitUID() )
						{
							ResetUFO();
						}
						else if( GetUFO()->GetNowHp() <= 0.f )
						{
							ResetUFO();
						}
						else
						{
							// �� ���, �߻� ���� �ٷ� ��� ���� �߻� �ð� ������ ������� ���濡 ���� ����
							GetUFO()->StateChange( _CONST_AISHA_::UFO_DYING.c_str(), true );
							ResetUFO();
						}
					} break;
				}
			}
		}
	}
	//*/



	if( m_pMPChargeA != NULL && m_pMPChargeB != NULL && m_pMPChargeC != NULL )
	{
		if( m_pMPChargeA->GetRenderParam()->color.a > 0.0f )
		{
			if( m_FrameDataNow.syncData.nowState == AVSI_CHARGE_MP )
			{
				m_pMPChargeA->GetMatrix().Move( GetMatrix().GetPos() );
				m_pMPChargeA->GetMatrix().Rotate( GetMatrix().GetRotate() );
			}
			else
			{
				m_pMPChargeA->GetRenderParam()->color.a = m_pMPChargeA->GetRenderParam()->color.a - (m_fElapsedTime * 2.0f);
			}
			m_pMPChargeA->OnFrameMove( m_fTime, m_fElapsedTime );
		}
		else
		{
			m_pMPChargeA->Stop();
		}
		if( m_pMPChargeB->GetRenderParam()->color.a > 0.0f )
		{
			if( m_FrameDataNow.syncData.nowState == AVSI_CHARGE_MP )
			{
				m_pMPChargeB->GetMatrix().Move( GetMatrix().GetPos() );
				m_pMPChargeB->GetMatrix().Rotate( GetMatrix().GetRotate() );
			}
			else
			{
				m_pMPChargeB->GetRenderParam()->color.a = m_pMPChargeB->GetRenderParam()->color.a - (m_fElapsedTime * 2.0f);
			}
			m_pMPChargeB->OnFrameMove( m_fTime, m_fElapsedTime );
		}
		else
		{
			m_pMPChargeB->Stop();
		}
		if( m_pMPChargeC->GetRenderParam()->color.a )
		{
			if( m_FrameDataNow.syncData.nowState == AVSI_CHARGE_MP )
			{
				m_pMPChargeC->GetMatrix().Move( GetMatrix().GetPos() );
				m_pMPChargeC->GetMatrix().Rotate( GetMatrix().GetRotate() );
			}
			else
			{
				m_pMPChargeC->GetRenderParam()->color.a = m_pMPChargeC->GetRenderParam()->color.a - (m_fElapsedTime * 2.0f);
			}
			m_pMPChargeC->OnFrameMove( m_fTime, m_fElapsedTime );
		}
		else
		{
			m_pMPChargeC->Stop();
		}
	}

	if( m_pEvasionAttackAnim != NULL )
		m_pEvasionAttackAnim->OnFrameMove( m_fTime, m_fElapsedTime );

	if( m_pComboX3Up1 != NULL )
	{
		float fAlpha = m_pComboX3Up1->GetMaxAnimationTime() - m_pComboX3Up1->GetNowAnimationTime();
		fAlpha = fAlpha / m_pComboX3Up1->GetMaxAnimationTime();
		m_pComboX3Up1->GetRenderParam()->color.a = fAlpha;
		m_pComboX3Up1->OnFrameMove( m_fTime, m_fElapsedTime );
	}
	if( m_pComboX3Up2 != NULL )
	{
		float fAlpha = 1.0f;
		if( m_pComboX3Up2->GetNowAnimationTime() > 0.2f )
		{
			fAlpha = 0.35f - 0.2f;
			fAlpha = ( 0.35f - m_pComboX3Up2->GetNowAnimationTime() ) / fAlpha;
		}
		m_pComboX3Up2->GetRenderParam()->color.a = fAlpha;
		m_pComboX3Up2->OnFrameMove( m_fTime, m_fElapsedTime );
	}
	if( m_pComboX3Up3 != NULL )
	{
		float fAlpha = 1.0f;
		if( m_pComboX3Up3->GetNowAnimationTime() > 0.3f )
		{
			fAlpha = 0.5f - 0.3f;
			fAlpha = ( 0.5f - m_pComboX3Up3->GetNowAnimationTime() ) / fAlpha;
		}
		m_pComboX3Up3->GetRenderParam()->color.a = fAlpha;
		m_pComboX3Up3->OnFrameMove( m_fTime, m_fElapsedTime );
	}


	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		D3DXVECTOR3 rFoot;
		GetFramePos( &rFoot, m_pFrame_Bip01_R_Foot );
		pSeqArmeAirStepRight->SetPosition( rFoot );
	}
	if( pSeqArmeAirStepLeft != NULL )
	{
		D3DXVECTOR3 lFoot;
		GetFramePos( &lFoot, m_pFrame_Bip01_L_Foot );
		pSeqArmeAirStepLeft->SetPosition( lFoot );
	}




	if( IsTransformed() )
	{
		if( m_ptrTransformWeapon != NULL )
		{
			m_ptrTransformWeapon->SetRenderParam( m_fTime, m_fElapsedTime, m_pXSkinAnim->GetRenderParam() );
			m_ptrTransformWeapon->OnFrameMove( m_fTime, m_fElapsedTime );
		}
	}

	if ( GetTransformTime() > 0.f )
	{
		SetTransformTime( GetTransformTime() - m_fElapsedTime );
		if( GetTransformTime() < 0.f )
			SetTransformTime( 0.f );
	}



	if( m_fMagicStaffSize > 0.f && GetNowHp() > 0.f && !IsTransformed() )
	{
		if( m_hMagicalStaff == INVALID_MESH_INSTANCE_HANDLE )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"aisha_P_magical_staff_M01", GetPos(), GetRotateDegree(), GetRotateDegree() );
			if( pMeshInst != NULL )
			{
                pMeshInst->SetPerFrameSimulation( true );
				m_hMagicalStaff = pMeshInst->GetHandle();
				pMeshInst->SetBoundingRadius( 0.0f );

				CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				pMeshInst->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
				pMeshInst->OnFrameMove( g_pKTDXApp->GetTime(), 0.01f );

				SetSubAttackList( pMeshInst->GetXSkinAnim() );
			}
		}
		else
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = g_pX2Game->GetMajorXMeshPlayer()->GetMeshInstance( m_hMagicalStaff );
			if( NULL != pMeshInst )
			{
				CKTDXDeviceXSkinMesh::MultiAnimFrame* pMultiAnimFrame = m_pXSkinAnim->GetCloneFrame( L"Dummy1_Rhand" );
				pMeshInst->SetUseDXMatrix( pMultiAnimFrame->combineMatrix );
			}
			else
			{
				m_hMagicalStaff = INVALID_MESH_INSTANCE_HANDLE;
				m_SubAttackListSet.clear();
			}
		}
	}
	else
	{
		if( m_hMagicalStaff != INVALID_MESH_INSTANCE_HANDLE )
		{
			g_pX2Game->GetMajorXMeshPlayer()->DestroyInstanceHandle( m_hMagicalStaff );
			m_hMagicalStaff = INVALID_MESH_INSTANCE_HANDLE;
			m_SubAttackListSet.clear();
		}
	}
}

void CX2GUArme_VioletMage::CommonStateEndFuture()
{
	CX2GUUser::CommonStateEndFuture();
}

void CX2GUArme_VioletMage::CommonStateEnd()
{
	CX2GUUser::CommonStateEnd();
}

//{{ robobeg : 2008-10-28
//void CX2GUArme_VioletMage::CommonRender()
/*virtual*/
RENDER_HINT CX2GUArme_VioletMage::CommonRender_Prepare()
//}} robobeg : 2008-10-28
{
// 	if( GetRemainHyperModeTime() > 0.0f )
// 	{
// 		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_COLOR_EDGE;
// 		m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_RED;
// 		m_RenderParam.outLineColor		= 0xff993388;
// 	}
// 	else
// 	{
// 		m_RenderParam.renderType		= CKTDGXRenderer::RT_CARTOON_BLACK_EDGE;
// 		if( m_NowStateData.stateID == AVSI_CHARGE_MP )
// 			m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_NORMAL_COUNTER;
// 		else
// 			m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
// 		m_RenderParam.outLineColor		= 0xffffffff;
// 	}

//{{ robobeg : 2008-10-28
	//CX2GUUser::CommonRender();
    return CX2GUUser::CommonRender_Prepare();
//}} robobeg : 2008-10-28
}

/*virtual*/ bool CX2GUArme_VioletMage::SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition /*= CX2SkillTree::ASUT_ONLY_ON_LINE*/ )
{
	KTDXPROFILE();

	const int INVALID_SKILL_SLOT_INDEX = -1;

	int iPressedSkillSlotIndex = INVALID_SKILL_SLOT_INDEX;
	const CX2UserSkillTree::SkillSlotData* pSkillSlotData = NULL;

	CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;// ������ ��� ��ų Ʈ��

	if ( false == CommonSpecialAttackEventProcess( accessUserSkillTree, pSkillSlotData, iPressedSkillSlotIndex ) )
		return false;

	if( NULL == pSkillSlotData )
		return false;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( pSkillSlotData->m_eID );
	if( NULL == pSkillTemplet )
		return false;

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

	const int iSkillTempletLevel = max( 1, accessUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

	float fMPConsume = GetActualMPConsume( pSkillTemplet->m_eID, iSkillTempletLevel );

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

	//{{ JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����
	CountUsedSkill( iPressedSkillSlotIndex );
	//}} JHKang / ������ / 2011/02/14 / ���� ��ũ ���� ����

	UpNowMp( -fMPConsume );


	if ( pSkillTemplet->m_eType == CX2SkillTree::ST_HYPER_ACTIVE_SKILL && g_pMain->GetNowStateID() != CX2Main::XS_TRAINING_GAME )
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

	if ( pSkillTemplet->m_eID == CX2SkillTree::SI_A_AHM_MEDITATION )
	{
		float fReduceCoolTime = 0.f;	/// ��Ÿ�� ����ġ

		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO11 ) )
			fReduceCoolTime = _CONST_AISHA_::AEM_MEDITATION_REDUCE_COOLTIME_VALUE;		/// -15�� ����

		if ( 0.f < m_fElementalResearchRate )
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, ( pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - fReduceCoolTime ) * m_fElementalResearchRate );
		else
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) - fReduceCoolTime );
	}
	else
	{
		if ( 0.f < m_fElementalResearchRate )
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) * m_fElementalResearchRate );
		else
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) );

	}


	if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO6 ) == true && pSkillTemplet->m_eID == CX2SkillTree::SI_A_AV_MANA_SHIELD )
	{
		if ( 0.f < m_fElementalResearchRate )
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) + 5.f - m_fElementalResearchRate );
		else
			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) + 5.f );

	}



	if ( m_bElementalFriendShip )
	{
		switch( pSkillTemplet->m_eType )
		{
		case CX2SkillTree::ST_SPECIAL_ACTIVE:
			{
				m_FrameDataFuture.syncData.nowAction = UAI_ELEMENTAL_FRIENDSHIP_UP;

			} break;
		default:
			break;
		}
	}


	if( NULL != m_pStrongMindData &&
		m_pStrongMindData->m_fTimeLeft <= 0.f )
	{
		float fRestoreMPRate = 0.f;
		switch( pSkillTemplet->m_eType )
		{
		case CX2SkillTree::ST_ACTIVE:
			{
				fRestoreMPRate = m_pStrongMindData->m_fActiveSkillRate;
			} break;

		case CX2SkillTree::ST_BUFF:
		case CX2SkillTree::ST_SPECIAL_ACTIVE:
			{
				fRestoreMPRate = m_pStrongMindData->m_fSpecialActiveSkillRate;
			} break;
		default:
			break;
		}


		if( GetRandomFloat() < fRestoreMPRate )
		{
			m_pStrongMindData->m_fTimeLeft = 3.f; // cool time 3��

			float fRestoredMP = fMPConsume * m_pStrongMindData->m_fMPRestoreRate;
			if( fRestoredMP > 0.f )
			{
				UpNowMp( fRestoredMP );

				if( NULL != g_pData->GetPicCharBlue() )
				{
					WCHAR wszText[64] = L"";
					StringCchPrintfW( wszText, ARRAY_SIZE(wszText), L"MP+ %d", (int) fRestoredMP );

					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.f;

					g_pData->GetPicCharBlue()->DrawText( wszText, pos, GetDirVector(), CKTDGPicChar::AT_CENTER );

					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Strong_Mind_MP_Up", this );


					PlaySound( L"AISHA_VP_STRONG_MIND.ogg" );

				}
			}
		}
	}



	if( false == SpecialAttackNoStageChange( pSkillTemplet ) )
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
	Send_SKILL_USE_REQ( pSkillTemplet->m_eID );

	return true;
}


// ���º�ȭ�� ���� �ʻ��� �ϵ��ڵ��ؼ� ���� ó�� �Ѵ�
/*virtual*/ bool CX2GUArme_VioletMage::SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet )
{
	if( NULL == pSkillTemplet )
		return false;

	switch( pSkillTemplet->m_eID )
	{
	case CX2SkillTree::SI_A_AV_MANA_SHIELD:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_MANA_SHIELD;
		} break;

	case CX2SkillTree::SI_A_AHM_MEDITATION:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_AHM_MEDITATION_START;
		} break;

	case CX2SkillTree::SI_A_AEM_SHINING_BODY:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_AHM_SHINING_BODY;
		} break;

	case CX2SkillTree::SI_A_AVP_SHADOW_BODY:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_AVP_SHADOW_BODY;
		} break;

	case CX2SkillTree::SI_A_ADW_ENERGETIC_BODY:
		{
			m_FrameDataFuture.syncData.nowAction = UAI_ADW_ENERGETIC_BODY;
		} break;

	default:
		{
			return false;
		} break;
	}

	return true;
}



/*virtual*/ void CX2GUArme_VioletMage::NoStateChangeActionFrameMove()
{
	switch( m_FrameDataNow.syncData.nowAction )
	{
	case UAI_MANA_SHIELD:
		{
			if ( false == GetRidingOn() )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AV_MANA_SHIELD );
				if( NULL != pSkillTemplet )
				{
					// ���� �ǵ� �޸� ���� ���� �߰�
					if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO6 ) == true )
					{
						SetBuffFactorToGameUnit( pSkillTemplet, 1 );
					}
					else
					{
						SetBuffFactorToGameUnit( pSkillTemplet, 0 );
					}
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ManaShield", this );
					PlaySound( L"Aisha_ManaShield.ogg" );
				}
			}

		} break;

	case UAI_AHM_MEDITATION_START:
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AHM_MEDITATION );

			if ( g_pX2Game->GetGameType() == CX2Game::GT_PVP )
				SetBuffFactorToGameUnit( pSkillTemplet, 2 );
			else
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );

			m_bInMeditation = true;		/// �޵����̼� ������ ���� ����
		} break;





	case UAI_ELEMENTAL_FRIENDSHIP_UP:
		{
			if ( NULL != GetUnit()  )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_AEM_ELEMENTAL_FRIENDSHIP );
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );
				SetBuffFactorToGameUnit( pSkillTemplet, 1 );
			}
		} break;

	case UAI_AHM_SHINING_BODY:
		{
			ProcessShiningBodySkill();
		} break;

	case UAI_AVP_SHADOW_BODY:
		{
			ProcessShadowBodySkill();
		} break;

	case UAI_ADW_ENERGETIC_BODY:
		{
			ProcessEnergeticBodySkill();
		} break;

	}
}

/** @function : CX2GetActualMPConsume
	@brief : ����� ��ų�� MP �Ҹ��� ����
	@param : ����� ��ų�� ID, ����� ��ų�� ����
	@return : �Ҹ�� MP ��ġ
*/
/*virtual*/ float CX2GUArme_VioletMage::GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const
{
	float fMpConsumption = CX2GUUser::GetActualMPConsume( eSkillID_, iSkillLevel_ );

	switch( eSkillID_ )
	{
	case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
	case CX2SkillTree::SI_SA_COMMON_AURA_SPEED_ACCEL:
		{
			const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA );
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

	case CX2SkillTree::SI_A_AV_TELEPORT:
		{
			fMpConsumption *= m_fMPConsumeRateTeleport;


			vector<KBuffFactor> VecDebuffFactor;
			GetBuffFactorOnlyDebuff( VecDebuffFactor );

			for( const KBuffFactor& kFactor_ : VecDebuffFactor )
			{
				for( const KBuffBehaviorFactor& kBehavior_ : kFactor_.m_vecBuffBehaviorFactors )
				{
					if ( kBehavior_.m_uiType == BBT_CHANGE_CONSUME_MP_RATE
						&& static_cast<UINT>( kBehavior_.m_vecValues[1] ) == CX2SkillTree::SI_A_AV_TELEPORT )
						fMpConsumption = kBehavior_.m_vecValues[2];
				}
			}

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO5 ) == true )
			{
				fMpConsumption *= 1.4f;
			}
		} break;
	case CX2SkillTree::SI_A_ADM_MANA_INTAKE:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO12 ) == true )
			{
				fMpConsumption -= 10.f;
			}
		}
		break;

	case CX2SkillTree::SI_SA_ABM_ENERGY_SPURT:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO15 ) == true )
			{
				fMpConsumption += 20.f;
			}
		}
		break;
	// ü�� ���̾�� ���̾� �� ��ȭ �нú� ������ �ް� ��
	case CX2SkillTree::SI_A_AV_CHAIN_FIRE_BALL :
		{
			// ���̾ ��ȭ �нú� ������ �ް� ��, �ĺ� MP �������� ������ ����
			fMpConsumption -= fMpConsumption * (1 - m_fMPConsumeRateFireBall) / 2;
		}
		break;
	case CX2SkillTree::SI_SA_ADM_AGING:
		{
			if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO22 ) == true )
				fMpConsumption -= 20.f;
		}
		break;
	default:
		break;
	}

	if( fMpConsumption < 0.f )
		fMpConsumption = 0.f;

	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( eSkillID_ );
	float fMpDecreaseRate = 1.0f;
	if( NULL != pSkillTemplet )
		fMpDecreaseRate  =  g_pData->GetMyUser()->GetSelectUnit()->GetSkillMpDecreaseRate(eSkillID_, pSkillTemplet->m_eType);

	return fMpConsumption * fMpDecreaseRate;
}

void CX2GUArme_VioletMage::StartMeditation( bool bStartMeditation )
{
	if ( bStartMeditation )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AHM_MEDITATION );
		if ( NULL != pSkillTemplet )
		{
			m_bInMeditation = bStartMeditation;


			if( false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO11 ) )
				SetBuffFactorToGameUnit( pSkillTemplet, 1 );
		}
		PlaySound( L"Aisha_Meditation.ogg" );
	}
	else
	{
		/// ��Ÿ�� �ʱ�ȭ
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AHM_MEDITATION );

		if( NULL != pSkillTemplet && NULL != GetUnit() )
		{
			CX2UserSkillTree& accessUserSkillTree =  GetUnit()->AccessUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, accessUserSkillTree.GetSkillLevel( pSkillTemplet->m_eID, true ) );	/// ��ų ����

			accessUserSkillTree.SetSkillCoolTimeLeft( pSkillTemplet->m_eID, pSkillTemplet->GetSkillCoolTimeValue( iSkillTempletLevel ) );
		}

		m_bInMeditation = bStartMeditation;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//							������Ʈ �Լ� ����

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//AVSI_DIE_FRONT
void CX2GUArme_VioletMage::DieFrontStartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( 0.5f );
}

void CX2GUArme_VioletMage::DieFrontStart()
{
	CommonStateStart();
	DyingStart();

	m_pXSkinAnim->SetPlaySpeed( 0.5f );
}

void CX2GUArme_VioletMage::DieFrontFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

void CX2GUArme_VioletMage::DieFrontEventProcess()
{
	if( EventTimer( 4.0f, true ) == true )
	{
		StateChange( GUSI_DIE );
	}
}


//AVSI_DIE_BACK
void CX2GUArme_VioletMage::DieBackStartFuture()
{
	CommonStateStartFuture();

	m_pXSkinAnimFuture->SetPlaySpeed( 0.5f );
}

void CX2GUArme_VioletMage::DieBackStart()
{
	CommonStateStart();
	DyingStart();

	m_pXSkinAnim->SetPlaySpeed( 0.5f );
}

void CX2GUArme_VioletMage::DieBackFrameMove()
{
	const float TIME_CREATE_STEP_DUST = 0.19f;		// ���Ⱑ �߻��Ǿ��ϴ� �ִϸ��̼� �ð�
	const float TIME_SACRIFICE_OF_HERO = 1.12f;		// ������ ����� ����Ǿ�� �ϴ� �ִϸ��̼� �ð�

	DieFrameMove( TIME_CREATE_STEP_DUST, TIME_SACRIFICE_OF_HERO );
}

void CX2GUArme_VioletMage::DieBackEventProcess()
{
	if( EventTimer( 4.0f, true ) == true )
	{
		StateChange( GUSI_DIE );
	}
}

void CX2GUArme_VioletMage::WaitStartFuture()
{
	CommonStateStartFuture();
	PlayAnimationWaitStart( m_pXSkinAnimFuture, m_FrameDataFuture );

	m_fMPChargeWaitTime = 0.0f;
}

void CX2GUArme_VioletMage::WaitStart()
{
	CommonStateStart();
	PlayAnimationWaitStart( m_pXSkinAnim.get(), m_FrameDataNow );
}

void CX2GUArme_VioletMage::WaitFrameMoveFuture()
{
	CommonFrameMoveFuture();

	m_fMPChargeWaitTime += m_fElapsedTime;
}

void CX2GUArme_VioletMage::WaitEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_COMBO_X1 );
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
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
	else if( m_fMPChargeWaitTime >= 0.5f && GetNowMp() != GetMaxMp() && g_pX2Game->GetSlideShot()->IsPresentNow() == false )
	{
		// �̸�� ���۽ÿ��� charge_mp ��ҽ�Ű�� �̸�� ���۽�Ų��.
		if(m_ePlayedEmotion == CX2Unit::ET_NONE)
			StateChange( AVSI_CHARGE_MP );
	}
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

void CX2GUArme_VioletMage::WalkEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_COMBO_X1, false );
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
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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


//AVSI_JUMP_READY
void CX2GUArme_VioletMage::JumpReadyEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_JUMP_ATTACK_Z );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_JUMP_ATTACK_X );
		m_PhysicParam.nowSpeed.y = GetJumpSpeed();
		m_FrameDataFuture.syncData.position.y += LINE_RADIUS;
	}
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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

void CX2GUArme_VioletMage::JumpUpEventProcess()
{
	if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_JUMP_ATTACK_X );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneUp == true && m_FrameDataFuture.unitCondition.bFootOnWall == false )
	{
		StateChange( AVSI_JUMP_LEVITATION );
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

void CX2GUArme_VioletMage::JumpDownEventProcess()
{
	if( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			if( m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else if( m_InputData.pureRight == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_JUMP_LANDING );
			}
		}
		else
			StateChange( USI_JUMP_LANDING );
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_JUMP_ATTACK_X );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( AVSI_WALL_LANDING );
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( AVSI_JUMP_LEVITATION );
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

//AVSI_JUMP_LEVITATION
void CX2GUArme_VioletMage::JumpLevitationStartFuture()
{
	CommonStateStartFuture();
	m_fLevitationElapsedTime		= _CONST_AISHA_::AISHA_LEVITATION_CONSUME_MP_TIME;
}


void CX2GUArme_VioletMage::JumpLevitationFrameMoveFuture()
{
	m_fLevitationElapsedTime	+= m_fElapsedTime;

	if( m_InputData.pureRight == true )
	{
		m_FrameDataFuture.syncData.bIsRight = true;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true )
	{
		m_FrameDataFuture.syncData.bIsRight = false;
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}

	if( IsTransformed() )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed / 40.0f;
	}
	else
	{
		if( m_fLevitationElapsedTime > _CONST_AISHA_::AISHA_LEVITATION_CONSUME_MP_TIME )
		{
			if( FlushMp( 5.0f * _CONST_AISHA_::AISHA_LEVITATION_CONSUME_MP_TIME ) == true )
			{
				m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed / 40.0f;
				m_fLevitationElapsedTime = 0.f;
			}
		}
		else
		{
			m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed / 40.0f;
		}
	}
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::JumpLevitationFrameMove()
{
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		pSeqArmeAirStepRight->SetEmitRate(10,20);
	}

	if( pSeqArmeAirStepLeft != NULL )
	{
		pSeqArmeAirStepLeft->SetEmitRate(10,20);
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::JumpLevitationEventProcess()
{
	if( m_InputData.pureDoubleLeft == true || m_InputData.pureDoubleRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( true == IsOnSomethingFuture() )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			if( m_InputData.pureLeft == true )
			{
				StateChangeDashIfPossible();
			}
			else if( m_InputData.pureRight == true )
			{
				StateChangeDashIfPossible();
			}
			else
			{
				StateChange( USI_JUMP_LANDING );
			}
		}
		else
			StateChange( USI_JUMP_LANDING );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_JUMP_ATTACK_X );
	}
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
	else if( m_InputData.pureUp == false )
	{
		StateChange( USI_JUMP_DOWN );
	}
	else if( m_PhysicParam.nowSpeed.y >= 0.0f )
	{
		StateChange( USI_JUMP_UP );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::JumpLevitationEnd()
{
	CommonStateEnd();

	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		pSeqArmeAirStepRight->SetEmitRate(0,0);
	}

	if( pSeqArmeAirStepLeft != NULL )
	{
		pSeqArmeAirStepLeft->SetEmitRate(0,0);
	}
}

void CX2GUArme_VioletMage::JumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_COMBO_X1 );
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
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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
		StateChange( USI_WAIT );
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::DashEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_DASH_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_DASH_ATTACK_X );
	}
	else if( SpecialAttackEventProcess() == true )
	{
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
	else if( m_InputData.oneUp == true )
	{
		StateChange( USI_DASH_JUMP );
		m_PhysicParam.nowSpeed.y = GetDashJumpSpeed();
	}
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


//AVSI_DASH_END
void CX2GUArme_VioletMage::DashEndStart()
{
	CommonStateStart();

	//m_pSoundStep->Set3DPosition( m_FrameDataNow.syncData.position );

}

void CX2GUArme_VioletMage::DashEndFrameMoveFuture()
{
	if( m_FrameDataFuture.unitCondition.fStateTime < 0.36f )
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 0.7f;

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::DashEndFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.23f ) )
	{
		CreateStepDust();
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		CreateStepDust();
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashEndEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 3.0f )
		{
			StateChange( AVSI_COMBO_Z1 );
		}
		else
		{
			StateChange( AVSI_DASH_COMBO_Z1 );
		}
	}
	else if( m_InputData.oneX == true )
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 3.0f )
		{
			StateChange( AVSI_COMBO_X1 );
		}
		else
		{
			StateChange( AVSI_DASH_ATTACK_X );
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
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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





//AVSI_DASH_JUMP
void CX2GUArme_VioletMage::DashJumpStartFuture()
{
	CommonStateStartFuture();

	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * 1.1f;
	m_bDownForce					= false;
}

void CX2GUArme_VioletMage::DashJumpStart()
{
	CommonStateStart();

	//m_pSoundStep->Set3DPosition( m_FrameDataNow.syncData.position );


	if( GetMatrix().GetYPos() == m_FrameDataNow.unitCondition.landPosition.y )
	{
		CreateStepDust();
	}
}

void CX2GUArme_VioletMage::DashJumpFrameMoveFuture()
{
	if( m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight	== true )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * 1.2f;
	}
	else if( m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight	== false )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * 1.2f;
	}

	if( m_bDownForce == true )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed;
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::DashJumpEventProcess()
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
			StateChange( AVSI_DASH_JUMP_POWER_LANDING );
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
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_DASH_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_DASH_JUMP_ATTACK_X );
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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
	if( m_InputData.oneUp == true && m_FrameDataFuture.syncData.position.y - m_FrameDataFuture.unitCondition.landPosition.y < LINE_RADIUS * 2.0f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	else if( m_FrameDataFuture.unitCondition.bFootOnWall == true &&
		( true == m_InputData.oneUp || true == m_InputData.oneDoubleUp ) )
	{
		m_bDisableGravity = true;
		StateChange( AVSI_WALL_LANDING );
	}
	else if( m_InputData.oneUp == true )
	{
		StateChange( AVSI_DASH_JUMP_LEVITATION );
	}
	if( m_InputData.oneDown == true )
	{
		m_PhysicParam.fMaxGSpeed = m_OrgPhysicParam.fMaxGSpeed * 2.0f;
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

void CX2GUArme_VioletMage::DashJumpEndFuture()
{
	CommonStateEnd();
	m_PhysicParam.fDownAccel	= 0.0f;
}




//AVSI_DASH_JUMP_LEVITATION
void CX2GUArme_VioletMage::DashJumpLevitationStartFuture()
{
	CommonStateStartFuture();
	m_PhysicParam.fDownAccel		= m_PhysicParam.fGAccel * 1.1f;
	m_fLevitationElapsedTime		= _CONST_AISHA_::AISHA_LEVITATION_CONSUME_MP_TIME;
}

void CX2GUArme_VioletMage::DashJumpLevitationFrameMoveFuture()
{
	m_fLevitationElapsedTime	+= m_fElapsedTime;

	if( m_InputData.pureRight == true && m_FrameDataNow.syncData.bIsRight	== true )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * 1.2f;
	}
	else if( m_InputData.pureLeft == true && m_FrameDataNow.syncData.bIsRight	== false )
	{
		m_PhysicParam.nowSpeed.x			= GetDashSpeed() * 1.2f;
	}

	if( IsTransformed() )
	{
		m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed / 40.0f;
	}
	else
	{
		if( m_fLevitationElapsedTime > _CONST_AISHA_::AISHA_LEVITATION_CONSUME_MP_TIME )
		{
			if( FlushMp( 5.0f * _CONST_AISHA_::AISHA_LEVITATION_CONSUME_MP_TIME ) == true )
			{
				m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed / 40.0f;
				m_fLevitationElapsedTime = 0.f;
			}
		}
		else
		{
			m_PhysicParam.nowSpeed.y = m_PhysicParam.fMaxGSpeed / 40.0f;
		}
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::DashJumpLevitationFrameMove()
{
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		pSeqArmeAirStepRight->SetEmitRate(10,20);
	}

	if( pSeqArmeAirStepLeft != NULL )
	{
		pSeqArmeAirStepLeft->SetEmitRate(10,20);
	}


	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashJumpLevitationEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		if( m_bDownForce == true )
		{
			StateChange( AVSI_DASH_JUMP_POWER_LANDING );
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
		}
	}
	else if( true == SpecialAttackEventProcess( CX2SkillTree::ASUT_AIR ) )
	{
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_DASH_JUMP_ATTACK_Z );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_DASH_JUMP_ATTACK_X );
	}
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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
	if( m_InputData.pureUp == false )
	{
		StateChange( USI_DASH_JUMP );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::DashJumpLevitationEnd()
{

	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		pSeqArmeAirStepRight->SetEmitRate(0, 0);
	}

	if( pSeqArmeAirStepLeft != NULL )
	{
		pSeqArmeAirStepLeft->SetEmitRate(0, 0);
	}

	CommonStateEnd();
}

void CX2GUArme_VioletMage::DashJumpLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_COMBO_X1 );
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
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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


//AVSI_DASH_JUMP_POWER_LANDING
void CX2GUArme_VioletMage::DashJumpPowerLandingStart()
{
	CommonStateStart();

	CreateStepDust();
}

void CX2GUArme_VioletMage::DashJumpPowerLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->GetNowAnimationTime() > m_pXSkinAnimFuture->GetMaxAnimationTime() / 2.0f )
	{
		if( m_InputData.oneZ == true )
		{
			StateChange( AVSI_COMBO_Z1 );
		}
		else if( m_InputData.oneX == true )
		{
			StateChange( AVSI_COMBO_X1 );
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
		////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
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

//AVSI_HYPER_MODE
void CX2GUArme_VioletMage::HyperModeInit()
{
	TextureReadyInBackground( L"ColorBallBlue.dds" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"WhiteCircle.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"ToKang.dds" );
	TextureReadyInBackground( L"Energy_Violet.dds" );
}

void CX2GUArme_VioletMage::HyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.47f, 0.63f );
}

void CX2GUArme_VioletMage::HyperModeCameraMove()
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
	}
}


//AVSI_CHARGE_MP
void CX2GUArme_VioletMage::ChargeMPInit()
{
	CKTDXDeviceXSkinMesh* pChargeMPA = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"ChargeMPA.X" );
	CKTDXDeviceXSkinMesh* pChargeMPB = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"ChargeMPB.X" );
	DeviceReady( pChargeMPA );
	DeviceReady( pChargeMPB );

	m_pMPChargeA			= CKTDGXSkinAnim::CreateSkinAnim();
	m_pMPChargeB			= CKTDGXSkinAnim::CreateSkinAnim();
	m_pMPChargeC			= CKTDGXSkinAnim::CreateSkinAnim();
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pMPChargeA );
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pMPChargeB );
	g_pKTDXApp->GetDGManager()->AddObjectChain( m_pMPChargeC );

	m_pMPChargeA->SetAnimXSkinMesh( pChargeMPA );
	m_pMPChargeA->AddModelXSkinMesh( pChargeMPA );
	m_pMPChargeB->SetAnimXSkinMesh( pChargeMPB );
	m_pMPChargeB->AddModelXSkinMesh( pChargeMPB );
	m_pMPChargeC->SetAnimXSkinMesh( pChargeMPA );
	m_pMPChargeC->AddModelXSkinMesh( pChargeMPA );

	m_pMPChargeA->SetAlphaObject( true );
	m_pMPChargeA->SetLayer( XL_EFFECT_0 );

	m_pMPChargeB->SetAlphaObject( true );
	m_pMPChargeB->SetLayer( XL_EFFECT_0 );

	m_pMPChargeC->SetAlphaObject( true );
	m_pMPChargeC->SetLayer( XL_EFFECT_0 );


	m_pMPChargeA->GetRenderParam()->bAlphaBlend		= true;
	//m_pMPChargeA->GetRenderParam()->bZEnable		= false;
	m_pMPChargeA->GetRenderParam()->bZWriteEnable	= false;
	m_pMPChargeA->GetRenderParam()->cullMode		= D3DCULL_NONE;
	m_pMPChargeA->GetRenderParam()->srcBlend		= D3DBLEND_SRCALPHA;
	m_pMPChargeA->GetRenderParam()->destBlend		= D3DBLEND_INVSRCALPHA;

	m_pMPChargeB->GetRenderParam()->bAlphaBlend		= true;
	//m_pMPChargeB->GetRenderParam()->bZEnable		= false;
	m_pMPChargeB->GetRenderParam()->bZWriteEnable	= false;
	m_pMPChargeB->GetRenderParam()->cullMode		= D3DCULL_NONE;
	m_pMPChargeB->GetRenderParam()->srcBlend		= D3DBLEND_SRCALPHA;
	m_pMPChargeB->GetRenderParam()->destBlend		= D3DBLEND_INVSRCALPHA;

	m_pMPChargeC->GetRenderParam()->bAlphaBlend		= true;
	//m_pMPChargeC->GetRenderParam()->bZEnable		= false;
	m_pMPChargeC->GetRenderParam()->bZWriteEnable	= false;
	m_pMPChargeC->GetRenderParam()->cullMode		= D3DCULL_NONE;
	m_pMPChargeC->GetRenderParam()->srcBlend		= D3DBLEND_SRCALPHA;
	m_pMPChargeC->GetRenderParam()->destBlend		= D3DBLEND_INVSRCALPHA;
}

void CX2GUArme_VioletMage::ChargeMPStartFuture()
{
	CommonStateStartFuture();

	const float fMPChangeRate = GetOriginalMPChangeRate();
	LUA_GET_VALUE( m_LuaManager, "ADD_MP_CHANGE_RATE", m_fAddMPChangeRate, 0.f );

	ResetMPChangeRate( (fMPChangeRate + m_fAddMPChangeRate) * (1.f + m_SocketData.m_fSpeedUpManaGather) );

// 	if( g_pX2Game->GetGameType() == CX2Game::GT_DUNGEON )
// 	{
// 		SetChangeRateMp( GetChangeRateMp() * 0.8f );
// 	}

}

void CX2GUArme_VioletMage::ChargeMPStart()
{
	CommonStateStart();

	m_pMPChargeA->ChangeAnim( L"ChargeSoulA" );
	m_pMPChargeA->SetPlaySpeed( 0.7f );
	//m_pMPChargeA->Play( CKTDGXSkinAnim::XAP_LOOP );

	m_pMPChargeB->ChangeAnim( L"ChargeSoulB" );
	//m_pMPChargeB->Play( CKTDGXSkinAnim::XAP_LOOP );

	m_pMPChargeC->ChangeAnim( L"ChargeSoulA" );
	m_pMPChargeC->SetPlaySpeed( 0.4f );
	//m_pMPChargeC->Play( CKTDGXSkinAnim::XAP_LOOP );

	m_fMPChargeSoundTime = 1.5f;

	PlaySound( L"wind.ogg" );

	CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2MagicCircle	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2MagicCircle );
	CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2Up	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2Up );
	CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2Up2	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2Up2 );

	if( pSeqMPCharge2MagicCircle != NULL )
	{
		pSeqMPCharge2MagicCircle->SetPosition( m_FrameDataNow.unitCondition.landPosition );
		pSeqMPCharge2MagicCircle->SetEmitRate( CMinMax<float>(1.0f,1.0f) );
	}

	if( pSeqMPCharge2Up != NULL )
	{
		pSeqMPCharge2Up->SetPosition( m_FrameDataNow.unitCondition.landPosition );
		pSeqMPCharge2Up->SetEmitRate( CMinMax<float>(10.0f,10.0f) );
	}

	if( pSeqMPCharge2Up2 != NULL )
	{
		pSeqMPCharge2Up2->SetPosition( m_FrameDataNow.unitCondition.landPosition );
		pSeqMPCharge2Up2->SetEmitRate( CMinMax<float>(10.0f,10.0f) );
	}

	m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_NORMAL_COUNTER;
}

void CX2GUArme_VioletMage::ChargeMPFrameMove()
{
	D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );

	if( m_hSeqMPEnergy != INVALID_PARTICLE_SEQUENCE_HANDLE &&
		m_hSeqMPEnergyCenter != INVALID_PARTICLE_SEQUENCE_HANDLE &&
		m_hSeqMPChargeWave != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeqMPEnergy		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergy );
		CKTDGParticleSystem::CParticleEventSequence* pSeqMPEnergyCenter = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergyCenter );
		CKTDGParticleSystem::CParticleEventSequence* pSeqMPChargeWave	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPChargeWave );

		CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2MagicCircle	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2MagicCircle );
		CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2Up	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2Up );
		CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2Up2	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2Up2 );


		if( NULL != pSeqMPEnergy )
		{
			pSeqMPEnergy->SetPosition( pos );
			pSeqMPEnergy->SetBlackHolePosition( pos );
			pSeqMPEnergy->SetLookPoint( pos );
		}

		if( NULL != pSeqMPEnergyCenter )
		{
			pSeqMPEnergyCenter->SetPosition( pos );
		}


		D3DXVECTOR3 pos2 = GetMatrix().GetPos();
		pos2.y += 10.0f;
		if( NULL != pSeqMPChargeWave )
		{
			pSeqMPChargeWave->SetPosition( pos2 );
		}

		if( pSeqMPCharge2MagicCircle != NULL )
			pSeqMPCharge2MagicCircle->SetPosition( m_FrameDataNow.unitCondition.landPosition );
		if( pSeqMPCharge2Up != NULL )
			pSeqMPCharge2Up->SetPosition( m_FrameDataNow.unitCondition.landPosition );
		if( pSeqMPCharge2Up2 != NULL )
			pSeqMPCharge2Up2->SetPosition( m_FrameDataNow.unitCondition.landPosition );
	}

	float alpha = m_FrameDataNow.unitCondition.fStateTime * 2.0f;
	if( alpha > 0.7f )
		alpha = 0.7f;
	m_pMPChargeA->GetRenderParam()->color.a = alpha;

	if( m_FrameDataNow.unitCondition.fStateTime < 5.0f )
	{
		alpha = m_FrameDataNow.unitCondition.fStateTime;
		if( alpha > 1.0f )
			alpha = 1.0f;
		m_pMPChargeA->GetMatrix().Scale( 1.0f, alpha, 1.0f );

		alpha = m_FrameDataNow.unitCondition.fStateTime / 2.0f;
		if( alpha > 1.0f )
			alpha = 1.0f;
		m_pMPChargeB->GetMatrix().Scale( 1.0f, alpha, 1.0f );
	}
	else
	{
		alpha = m_FrameDataNow.unitCondition.fStateTime - 4.0f;
		if( alpha > 2.0f )
			alpha = 2.0f;
		m_pMPChargeA->GetMatrix().Scale( 1.0f, alpha, 1.0f );

		alpha = m_FrameDataNow.unitCondition.fStateTime - 4.0f;
		if( alpha > 2.0f )
			alpha = 2.0f;
		m_pMPChargeB->GetMatrix().Scale( 1.0f, alpha, 1.0f );
	}

	alpha = m_FrameDataNow.unitCondition.fStateTime;
	if( alpha > 1.0f )
		alpha = 1.0f;
	m_pMPChargeB->GetRenderParam()->color.a = alpha;

	alpha = m_pMPChargeC->GetNowAnimationTime() / m_pMPChargeC->GetMaxAnimationTime();
	m_pMPChargeC->GetRenderParam()->color.a = 1.0f - alpha;

	if( m_FrameDataNow.unitCondition.fStateTime < 5.0f )
	{
		m_pMPChargeC->GetMatrix().Scale( alpha * 2.5f, alpha * 2.0f, alpha * 2.5f );
	}
	else
	{
		m_pMPChargeC->GetMatrix().Scale( alpha * 3.0f, alpha * 3.0f, alpha * 3.0f );
		//if( GetDistance(GetPos(), g_pX2Game->GetMyUnit()->GetPos() ) < 500.0f )
		//	g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCameraNoReset( 2.0f, 0.1f );
	}

	if( m_FrameDataNow.unitCondition.fStateTime < 1.0f )
	{
		m_pMPChargeC->GetMatrix().Scale( 0.0f, 0.0f, 0.0f );
	}

	if( EventTimer(5.0f, false) == true )
	{
		m_pMPChargeB->SetPlaySpeed( 1.5f );
		m_pMPChargeC->SetPlaySpeed( 0.3f );
		//g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 5.0f, 0.2f );
		//g_pX2Game->GetWorld()->SetWorldColor( 0xff333333 );
		//g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 2.0f );

		//m_pMPChargeWave->SetEmitRate( CMinMax<float>(3.0f,5.0f) );




		CKTDGParticleSystem::CParticleEventSequence* pSeqMPEnergy		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergy );
		CKTDGParticleSystem::CParticleEventSequence* pSeqMPEnergyCenter = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergyCenter );



		if( pSeqMPEnergy != NULL )
			pSeqMPEnergy->SetEmitRate( CMinMax<float>(5.0f,15.0f) );
		if( pSeqMPEnergyCenter != NULL )
			pSeqMPEnergyCenter->SetEmitRate( CMinMax<float>(5.0f,15.0f) );



		//CKTDGParticleSystem::CParticleEventSequence* pSeq;
		//pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"ImpactTickBlue", pos, 200, 200, 2, 20 );
		//if( pSeq != NULL )
		//	pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );

		//g_pKTDXApp->GetDGManager()->ClearScreen();
		//m_pSoundBreak->Set3DPosition( GetMatrix().GetPos() );
		//m_pSoundBreak->Play();

		PlaySound( L"Charge.ogg" );
	}

	m_fMPChargeSoundTime -= m_fElapsedTime;
	if( m_fMPChargeSoundTime <= 0.0f )
	{
		if( m_FrameDataNow.unitCondition.fStateTime > 5.0f )
		{
			PlaySound( L"Charge.ogg" );
		}
		PlaySound( L"wind.ogg" );

		m_fMPChargeSoundTime = 1.5f;
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ChargeMPEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_InputData.oneZ == true )
	{
		StateChange( AVSI_COMBO_Z1 );
	}
	else if( m_InputData.oneX == true )
	{
		StateChange( AVSI_COMBO_X1 );
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
		StateChange( USI_JUMP_READY );
	}
	ELSE_IF_CAN_HYPER_MODE_THEN_STATE_CHANGE( USI_HYPER_MODE )
	////ELSE_IF_CAN_USE_SPECIAL_ABILITY_THEN_STATE_CHANGE_OR_GUIDE( 10.f * m_fMPConsumeRateTeleport, AVSI_EVASION_START, ShowMagicFailGuide() )
	else if( GetNowMp() == GetMaxMp() )
	{
		StateChange( USI_WAIT );
	}
	else if( g_pX2Game->GetSlideShot()->IsPresentNow() == true )
	{
		StateChange( USI_WAIT );
	}
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

void CX2GUArme_VioletMage::ChargeMPEndFuture()
{
	CommonStateEndFuture();

	m_fAddMPChangeRate = 0.f;
	const float fMPChangeRate = GetOriginalMPChangeRate();
	ResetMPChangeRate( fMPChangeRate );
}

void CX2GUArme_VioletMage::ChargeMPEnd()
{
	CommonStateEnd();

	CKTDGParticleSystem::CParticleEventSequence* pSeqMPEnergy		= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergy );
	CKTDGParticleSystem::CParticleEventSequence* pSeqMPEnergyCenter = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPEnergyCenter );
	CKTDGParticleSystem::CParticleEventSequence* pSeqMPChargeWave	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPChargeWave );

	CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2MagicCircle	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2MagicCircle );
	CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2Up	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2Up );
	CKTDGParticleSystem::CParticleEventSequence* pSeqMPCharge2Up2	= g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqMPCharge2Up2 );


	if( pSeqMPEnergy != NULL )
		pSeqMPEnergy->SetEmitRate( CMinMax<float>(0.0f,0.0f) );
	if( pSeqMPEnergyCenter != NULL )
		pSeqMPEnergyCenter->SetEmitRate( CMinMax<float>(0.0f,0.0f) );
	if( pSeqMPChargeWave != NULL )
		pSeqMPChargeWave->SetEmitRate( CMinMax<float>(0.0f,0.0f) );

	if( pSeqMPCharge2MagicCircle != NULL )
		pSeqMPCharge2MagicCircle->SetEmitRate( CMinMax<float>(0.0f,0.0f) );
	if( pSeqMPCharge2Up != NULL )
		pSeqMPCharge2Up->SetEmitRate( CMinMax<float>(0.0f,0.0f) );
	if( pSeqMPCharge2Up2 != NULL )
		pSeqMPCharge2Up2->SetEmitRate( CMinMax<float>(0.0f,0.0f) );


	//m_pSoundChargeWind->Stop();
	g_pKTDXApp->GetDeviceManager()->StopSound( L"wind.ogg" );
	//m_pSoundCharge->Stop();
	g_pKTDXApp->GetDeviceManager()->StopSound( L"Charge.ogg" );

	/*
	if( GetGageManager()->GetMPGage()->fNow == GetGageManager()->GetMPGage()->fMax )
	{
		D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );

		CKTDGParticleSystem::CParticleEventSequence* pSeq;
		pSeq = g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"ImpactTickBlue",
			pos, 200, 200, 2, 20 );
		if( pSeq != NULL )
			pSeq->SetLandPosition( m_FrameDataNow.unitCondition.landPosition.y );

		g_pKTDXApp->GetDGManager()->ClearScreen();
		m_pSoundBreak->Play();
		g_pX2Game->GetX2Camera()->GetCamera().UpDownCrashCamera( 5.0f, 0.2f );
		g_pX2Game->GetWorld()->SetWorldColor( 0xff333333 );
		g_pX2Game->GetWorld()->FadeWorldColor( g_pX2Game->GetWorld()->GetOriginColor(), 2.0f );
	}
	*/

	m_RenderParam.cartoonTexType	= CKTDGXRenderer::CTT_NORMAL;
}


////AVSI_DAMAGE_GROGGY
//void CX2GUArme_VioletMage::AVSI_DAMAGE_GROGGY_FrameMoveFuture()
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
//void CX2GUArme_VioletMage::AVSI_DAMAGE_GROGGY_EventProcess()
//{
//	if( m_fGroggyTime <= 0.0f )
//	{
//		StateChange( AVSI_WAIT );
//	}
//	CommonEventProcess();
//}


//AVSI_DAMAGE_SMALL_FRONT
void CX2GUArme_VioletMage::DamageSmallFrontEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//AVSI_DAMAGE_SMALL_BACK
void CX2GUArme_VioletMage::DamageSmallBackEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//AVSI_DAMAGE_BIG_FRONT
void CX2GUArme_VioletMage::DamageBigFrontStart()
{
	CommonStateStart();

	CreateStepDust();

}

void CX2GUArme_VioletMage::DamageBigFrontEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//AVSI_DAMAGE_BIG_BACK
void CX2GUArme_VioletMage::DamageBigBackStart()
{
	CommonStateStart();

	CreateStepDust();
}

void CX2GUArme_VioletMage::DamageBigBackEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_SMALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	CommonEventProcess();
}


//AVSI_DAMAGE_DOWN_FRONT
void CX2GUArme_VioletMage::DamageDownFrontFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.19f ) )
	{
		//m_pSoundDown->Set3DPosition( m_FrameDataNow.syncData.position );

		CreateStepDust();
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::DamageDownFrontEventProcess()
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
				StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
			}
			else
			{
				StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
		{
			m_FrameDataFuture.syncData.bIsRight = true;

			StateChange( AVSI_TELEPORT_STAND_UP_START );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
		{
			m_FrameDataFuture.syncData.bIsRight = false;

			StateChange( AVSI_TELEPORT_STAND_UP_START );
		}
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
		else if( m_InputData.pureRight == true )
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

	CommonEventProcess();
}


//AVSI_DAMAGE_DOWN_BACK
void CX2GUArme_VioletMage::DamageDownBackFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.32f ) )
	{
		//m_pSoundDown->Set3DPosition( m_FrameDataFuture.syncData.position );

		CreateStepDust();
	}

	CommonFrameMove();
}


void CX2GUArme_VioletMage::DamageDownBackEventProcess()
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
				StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
			}
			else
			{
				StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
			}
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
		{
			m_FrameDataFuture.syncData.bIsRight = true;

			StateChange( AVSI_TELEPORT_STAND_UP_START );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
		{
			m_FrameDataFuture.syncData.bIsRight = false;

			StateChange( AVSI_TELEPORT_STAND_UP_START );
		}
		else
			StateChange( USI_DAMAGE_STANDUP_BACK );
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
		else if( m_InputData.pureRight == true )
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

	CommonEventProcess();
}

//AVSI_DAMAGE_AIR_DOWN_LANDING
void CX2GUArme_VioletMage::DamageAirDownLandingInit()
{
	TextureReadyInBackground( L"stone.dds" );
	XSkinMeshReadyInBackground( L"Effect_ElSword_dropdown.X" );
}

void CX2GUArme_VioletMage::DamageAirDownLandingEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_DAMAGE_AIR_FALL );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
		{
			StateChange( AVSI_TELEPORT_STANDUP_ATTACK );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[2] == true )
		{
			m_FrameDataFuture.syncData.bIsRight = true;

			StateChange( AVSI_TELEPORT_STAND_UP_START );
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[3] == true )
		{
			m_FrameDataFuture.syncData.bIsRight = false;

			StateChange( AVSI_TELEPORT_STAND_UP_START );
		}
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
		else if( m_InputData.pureRight == true )
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

	CommonEventProcess();
}

//AVSI_EVASION_START
void CX2GUArme_VioletMage::AVSI_A_AV_Teleport_StateStart()
{
	CommonStateStart();

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO5 ) == true )
	{
		D3DXVECTOR2 superArmorTime( 0.0f, 0.5f);
		AccessVecNowSuperArmorTime().push_back(superArmorTime);
		m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
	}

	AdaptAdvancedTeleportationBuff();

	const int iSkillLevel
		= GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_A_AV_TELEPORT, true );

	if ( iSkillLevel > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet
			= GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_A_AV_TELEPORT );

		if ( NULL != pSkillTemplet )
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}
}

void CX2GUArme_VioletMage::AVSI_A_AV_Teleport_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		//{{ dmlee 2009/08/10 ��ȭ, �����ڵ� ���� �¾��� �� �ϴó��� �ö󰡹����� ������ ��������
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f &&
			m_ExtraDamagePack.m_Frozen.m_fTime <= 0.f )
		//}} dmlee 2009/08/10 ��ȭ, �����ڵ� ���� �¾��� �� �ϴó��� �ö󰡹����� ������ ��������
		{
			m_FrameDataFuture.stateParam.bCanPushUnit = false;
			m_FrameDataFuture.stateParam.bCanPassUnit = true;

			float fMoveTime;
			LUA_GET_VALUE( m_LuaManager, "MOVE_TIME", fMoveTime, 0.0f );

			if( m_iAdvancedTeleportationLv > 0 && m_FrameDataFuture.stateParam.bEventFlagList[1] == true )
			{
				m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
				m_PhysicParam.nowSpeed.y = -1.f *  GetJumpSpeed();
			}
			else
			{
				m_FrameDataFuture.syncData.position.y += LINE_RADIUS * 1.5f;
				m_PhysicParam.nowSpeed.y =  GetJumpSpeed();
			}
			float y = m_FrameDataFuture.syncData.position.y;

			for( int i = 0; i < (int)(fMoveTime * 85.0f); i++ )
			{
				m_PhysicParam.nowSpeed.x = 700.f;
				CX2GUUser::PhysicProcess();

				if( true == IsOnSomethingFuture() )
					m_FrameDataFuture.syncData.position.y = m_FrameDataFuture.unitCondition.landPosition.y + LINE_RADIUS * 1.5f;
			}

			if( m_iAdvancedTeleportationLv < 1 || m_FrameDataFuture.stateParam.bEventFlagList[1] == false )
			{
				if( m_FrameDataFuture.syncData.position.y <= y )
					m_FrameDataFuture.syncData.position.y = y + 100.0f;
			}
			m_PhysicParam.nowSpeed.x = 0.0f;

			m_FrameDataFuture.stateParam.bCanPushUnit = true;
			m_FrameDataFuture.stateParam.bCanPassUnit = false;

			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_A_AV_Teleport_EventProcess()
{
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
	{
		StateChange( AVSI_TELEPORT_END );
	}
	else if( m_InputData.pureDown == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	CommonEventProcess();
}



//AVSI_EVASION_END
void CX2GUArme_VioletMage::EvasionEndFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
		pos.y -= 200.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
			pos,
			m_FrameDataNow.unitCondition.dirDegree,
			m_FrameDataNow.unitCondition.dirDegree,
			XL_EFFECT_0 );

	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::EvasionEndEventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.4f )
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
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
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
	CommonEventProcess();
}


//AVSI_EVASION_STAND_UP_START
void CX2GUArme_VioletMage::AVSI_EVASION_STAND_UP_START_Start()
{
	CommonStateStart();

	D3DXVECTOR2 superArmorTime( 0.0f, 0.6f + 0.5f);
	AccessVecNowSuperArmorTime().push_back(superArmorTime);
	m_FrameDataNow.stateParam.bSuperArmorNotRed = false;


	AdaptAdvancedTeleportationBuff();
}

void CX2GUArme_VioletMage::AVSI_EVASION_STAND_UP_START_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{

		//{{ dmlee 2009/08/10 ��ȭ, �����ڵ� ���� �¾��� �� �ϴó��� �ö󰡹����� ������ ��������
		if( m_ExtraDamagePack.m_EnchantShock.m_fTime <= 0.f &&
			m_ExtraDamagePack.m_Frozen.m_fTime <= 0.f )
		//}} dmlee 2009/08/10 ��ȭ, �����ڵ� ���� �¾��� �� �ϴó��� �ö󰡹����� ������ ��������
		{
			m_FrameDataFuture.stateParam.bCanPushUnit = false;
			m_FrameDataFuture.stateParam.bCanPassUnit = true;

			float fMoveTime;
			LUA_GET_VALUE( m_LuaManager, "MOVE_TIME", fMoveTime, 0.0f );


			m_FrameDataFuture.syncData.position.y += LINE_RADIUS * 1.5f;
			float y = m_FrameDataFuture.syncData.position.y;
			m_PhysicParam.nowSpeed.y = GetJumpSpeed();

			for( int i = 0; i < (int)(fMoveTime * 85.0f); i++ )
			{
				m_PhysicParam.nowSpeed.x = 700.f;
				CX2GUUser::PhysicProcess();

				if( true == IsOnSomethingFuture() )
					m_FrameDataFuture.syncData.position.y = m_FrameDataFuture.unitCondition.landPosition.y + LINE_RADIUS * 1.5f;
			}

			if( m_FrameDataFuture.syncData.position.y <= y )
				m_FrameDataFuture.syncData.position.y = y + 100.0f;
			m_PhysicParam.nowSpeed.x = 0.0f;

			m_FrameDataFuture.stateParam.bCanPushUnit = true;
			m_FrameDataFuture.stateParam.bCanPassUnit = false;

			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;

		}
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_EVASION_STAND_UP_START_EventProcess()
{
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
	{
		StateChange( AVSI_TELEPORT_END );
	}
	CommonEventProcess();
}



//AVSI_EVASION_STAND_UP_END
void CX2GUArme_VioletMage::AVSI_EVASION_STAND_UP_END_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
		pos.y -= 200.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
			pos,
			m_FrameDataNow.unitCondition.dirDegree,
			m_FrameDataNow.unitCondition.dirDegree,
			XL_EFFECT_0 );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_EVASION_STAND_UP_END_EventProcess()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.4f )
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
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
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
	CommonEventProcess();
}


//void CX2GUArme_VioletMage::AVSI_EVASION_STAND_UP_END_End()
//{
//	CommonStateEnd();
//}

//AVSI_EVASION_STANDUP_ATTACK
void CX2GUArme_VioletMage::EvasionStandupAttackInit()
{
	CKTDXDeviceXSkinMesh* pEvasionAttackMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Arme_EvasionAttack.X" );
	CKTDXDeviceXSkinMesh* pEvasionMesh = g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( L"Arme_Evasion.X" );
	DeviceReady( pEvasionAttackMesh );
	DeviceReady( pEvasionMesh );

	m_pEvasionAttackAnim = CKTDGXSkinAnim::CreateSkinAnim();
	m_pEvasionAttackAnim->SetAnimXSkinMesh( pEvasionAttackMesh );
	m_pEvasionAttackAnim->AddModelXSkinMesh( pEvasionAttackMesh );
}

void CX2GUArme_VioletMage::EvasionStandupAttackStart()
{
	CommonStateStart();

	m_DamageData.attackerType	= CX2DamageManager::AT_MESH;
	m_DamageData.pAttackerAnim	= m_pEvasionAttackAnim;
}

void CX2GUArme_VioletMage::EvasionStandupAttackFrameMove()
{
	if( m_pEvasionAttackAnim->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
	{
		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataFuture.unitCondition.bHit = true;
			m_FrameDataNow.unitCondition.bHit = true;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
		pos.y -= 200.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
														pos,
														m_FrameDataNow.unitCondition.dirDegree,
														m_FrameDataNow.unitCondition.dirDegree,
														XL_EFFECT_0 );


		m_pEvasionAttackAnim->GetMatrix().Move( pos );
		m_pEvasionAttackAnim->GetMatrix().RotateDegree( m_FrameDataNow.unitCondition.dirDegree );
		m_pEvasionAttackAnim->ChangeAnim( L"EvasionAttack", false );
		m_pEvasionAttackAnim->SetPlaySpeed( 2.0f );
		m_pEvasionAttackAnim->Play( CKTDGXSkinAnim::XAP_ONE );
	}


	m_pEvasionAttackAnim->OnFrameMove( m_fTime, m_fElapsedTime );
	CommonFrameMove();
}

void CX2GUArme_VioletMage::EvasionStandupAttackEventProcess()
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
	CommonEventProcess();
}

void CX2GUArme_VioletMage::EvasionStandupAttackEnd()
{
	CommonStateEnd();

	SetForceInvincible( 0.3f );
	SetShowInvincible( 0.3f );
}


//AVSI_COMBO_Z1
void CX2GUArme_VioletMage::ComboZ1FrameMove()
{

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboZ1EventProcess()
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
		StateChange( AVSI_COMBO_Z2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.27f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.5f )
		{
			m_fCanNotAttackTime = 0.2f;
			StateChange( USI_WAIT );
		}
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fCanNotAttackTime = 0.1f;
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_COMBO_Z2
//void CX2GUArme_VioletMage::ComboZ2FrameMove()
//{
//
//	CommonFrameMove();
//}

void CX2GUArme_VioletMage::ComboZ2EventProcess()
{
	CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )		/// ��ų ĵ�� ���� �߰�
	ELSE_IF_STATE_CHANGE_ON_( 1, 0.43f, 0.43f, m_InputData.oneX == true &&
	( GetUnit()->GetClass() == CX2Unit::UC_ARME_HIGH_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_ELEMENTAL_MASTER ), AVSI_COMBO_X3 )	/// ���̾ ����� �߰�

	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.47f )
	{

		if( CX2Unit::UC_ARME_VOID_PRINCESS == GetUnitClass() &&
			( ( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false ) ||
			( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true ) ) )
		{
			StateChange( AVSI_AVP_ZZfrontZ );
		}
		else if( m_InputData.pureUp == true )
			StateChange( AVSI_COMBO_Z3up );
		else
			StateChange( AVSI_COMBO_Z3 );

		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.47f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.6f )
		{
			m_fCanNotAttackTime = 0.2f;
			StateChange( USI_WAIT );
		}
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		m_fCanNotAttackTime = 0.2f;
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}




void CX2GUArme_VioletMage::ComboZ3EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}
	else if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 0.65f )
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_DOWN );
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}



void CX2GUArme_VioletMage::ComboZ3upEventProcess()
{
	if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}


	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.4f )
	{
		StateChange( AVSI_COMBO_Z4up );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.4f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.7f )
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

	CommonEventProcess();
}



//AVSI_COMBO_Z4up
void CX2GUArme_VioletMage::ComboZ4upStart()
{
	CommonStateStart();

	m_DamageData.attackerType	= CX2DamageManager::AT_MANUAL_COLLISION_DATA;
	m_DamageData.bCanRevenge	= false;

	if( FlushMp( 10.0f ) == true )
		m_bZ4up = true;
	else
		m_bZ4up = false;
}

void CX2GUArme_VioletMage::ComboZ4upFrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.2f && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.8f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::ComboZ4upFrameMove()
{
	if( m_bZ4up == true )
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
		{

			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );

			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_RING ) );
			if( NULL != pSeq )
			{
				PlaySequenceByTriggerTime( pSeq, pos, 5, 10, 0.4f );
				pSeq->SetAddRotate( D3DXVECTOR3( 0.0f, m_FrameDataNow.unitCondition.dirDegree.y + 70.0f, 0.0f ) );
			}

			if( GetIsRight() == true )
				pos += 50.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 50.0f * m_FrameDataNow.unitCondition.dirVector;

			pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_CENTER ) );
			PlaySequenceByTriggerTime( pSeq, pos, 5, 10, 0.4f );
		}

		if( m_pXSkinAnim->GetNowAnimationTime() > 0.2f && m_pXSkinAnim->GetNowAnimationTime() < 0.7f )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDXCollision::CollisionData data;
			data.m_fRadius = 100.0f;

			if( GetIsRight() == true )
				pos += 50.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 50.0f * m_FrameDataNow.unitCondition.dirVector;
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._41 = pos.x;
			mat._42 = pos.y;
			mat._43 = pos.z;
			data.m_pCombineMatrix = &mat;
			m_DamageData.pCollisionData = &data;

			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
		}

		if( EventTimer( 0.71f, false ) == true )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDXCollision::CollisionData data;
			data.m_fRadius = 100.0f;

			if( GetIsRight() == true )
				pos += 50.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 50.0f * m_FrameDataNow.unitCondition.dirVector;
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._41 = pos.x;
			mat._42 = pos.y;
			mat._43 = pos.z;
			data.m_pCombineMatrix = &mat;

			m_DamageData.pCollisionData = &data;
			m_DamageData.backSpeed.x	= GetDashSpeed() * 2.0f;
			m_DamageData.backSpeed.y	= GetJumpSpeed() * 0.3f;
			m_DamageData.bReAttack		= false;
			m_DamageData.hitUnitList.resize(0);


			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
			m_bZ4up = false;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );

		CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboZ4upEventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.4f )
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




//AVSI_COMBO_X1
void CX2GUArme_VioletMage::ComboX1FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
				IsTransformed() )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MAGICBALL_SHORT", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			}
			else
			{
				if ( m_bIceBall )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Short", GetPowerRate() * m_fIceBallPower, posR,
					GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
				else
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SHORT", GetPowerRate(), posR, GetRotateDegree(),
						GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			}
		}
		else
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
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX1EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	ELSE_IF_STATE_CHANGE_ON_( 1, 0.43f, 0.43f, GetUnitClass() == CX2Unit::UC_ARME_ELEMENTAL_MASTER && m_InputData.oneZ == true, AVSI_AEM_XZ )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.43f )
	{
		StateChange( AVSI_COMBO_X2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.43f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_COMBO_X2
void CX2GUArme_VioletMage::ComboX2FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.53f ) )
	{
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
				IsTransformed() )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MAGICBALL_SHORT", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			}
			else
			{
				if ( m_bIceBall )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Short", GetPowerRate() * m_fIceBallPower, posR,
					GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
				else
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SHORT", GetPowerRate(), posR, GetRotateDegree(),
						GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX2EventProcess()
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
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
	{
		if( m_InputData.pureDown == true )
			StateChange( AVSI_COMBO_X3down );
		else if( m_FrameDataFuture.syncData.bIsRight == true && m_InputData.pureRight == true )
			StateChange( AVSI_COMBO_X3front );
		else if( m_FrameDataFuture.syncData.bIsRight == false && m_InputData.pureLeft == true )
			StateChange( AVSI_COMBO_X3front );
		else
		{
			m_bCanChangeState = true;		/// ���̾ ���� ������Ʈ ��� ����

			StateChange( AVSI_COMBO_X3 );
		}
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.9f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}



//AVSI_COMBO_X3
void CX2GUArme_VioletMage::ComboX3FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		if( FlushMp( 6.0f  * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			bool bBattleMagician = false;
			if( GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				bBattleMagician = true;
			if( bBattleMagician && IsTransformed() )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MAGICBALL_BIG2", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			}
			else
			{
				if ( m_bIceBall )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Big2", GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(),
						GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
				else
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_BIG2", GetPowerRate(), posR, GetRotateDegree(),
						GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );

			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX3EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.735f )
	{
		if( true == m_bCanChangeState && false == IsTransformed() )		/// ���� ������Ʈ�� ������ �����ϰ�, �������� �ƴ� ��
			StateChange( AVSI_COMBO_X4_CHARGE );
		else
			StateChange( AVSI_COMBO_X4 );

		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.4f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::ComboX3StateEnd()
{
	m_bCanChangeState = false;

	CommonStateEnd();
}

//AVSI_COMBO_X4_CHARGE
void CX2GUArme_VioletMage::ComboX4ChargeStateStart()
{
	CommonStateStart();

	m_fPushXChargeTime = 0.f;
	SetNoDetonation( true );		/// ���� ���� ����
}

void CX2GUArme_VioletMage::ComboX4ChargeFrameMove()
{
	CommonFrameMove();

	if ( _CONST_AISHA_::AEM_COMBOX4_MAX_CHARGING_TIME >= m_fPushXChargeTime )
		m_fPushXChargeTime += m_fElapsedTime;

	if ( _CONST_AISHA_::AEM_COMBOX4_MAX_CHARGING_TIME <= m_fPushXChargeTime )
		m_fPushXChargeTime = _CONST_AISHA_::AEM_COMBOX4_MAX_CHARGING_TIME;
}

void CX2GUArme_VioletMage::ComboX4ChargeEventProcess()
{
	if ( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	/// X�� ������ ���� �ʰų�, �ִ� ���� �ð��� ������ �߻� ������Ʈ�� �̵�
	else if ( false == m_InputData.pureX || _CONST_AISHA_::AEM_COMBOX4_MAX_CHARGING_TIME <= m_FrameDataNow.unitCondition.fStateTime )
	{
		StateChange( AVSI_COMBO_X4_CHARGE_FIRE );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::ComboX4ChargeStateEnd()
{
	CommonStateEnd();

	SetNoDetonation( false );		/// ���� ���� ����
}

//AVSI_COMBO_X4_CHARGE_FIRE
void CX2GUArme_VioletMage::ComboX4ChargeFireFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.166f ) )
	{
		if( FlushMp( 6.0f  * m_fMPConsumeRateFireBall ) == true )
		{
			/// ������ �ð��� �Ŀ� ����Ʈ�� ����
			const float fComboX4PowerRate = GetPowerRate() *  ( min( m_fPushXChargeTime / 2.f, _CONST_AISHA_::AEM_COMBOX4_MAX_CHARGING_TIME ) + 1.f );
			/// ������ �ð��� ���� ������ ����
			const float fChargeFireBallScale = ( m_fPushXChargeTime / 2.f ) + 1.f;

			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );

			/// ������ ����Ʈ ����
			CX2DamageEffect::CEffect* pChargeFireBall = NULL;

			if ( m_bIceBall )
				pChargeFireBall = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Big1",
					fComboX4PowerRate * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			else if ( m_bResearchGravityBall )
				pChargeFireBall = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_GRAVITY_BALL_BIG",
					fComboX4PowerRate + m_fGravityBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			else
				pChargeFireBall = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_BIG", fComboX4PowerRate, posR, GetRotateDegree(), GetRotateDegree(),
				m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			/// ������ ����
			if( NULL != pChargeFireBall )
			{
				pChargeFireBall->SetScale( fChargeFireBallScale, fChargeFireBallScale, fChargeFireBallScale );
			}

			m_fPushXChargeTime = 0.f;	/// ���� �ð� �ʱ�ȭ
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX4ChargeFireEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_COMBO_X4
void CX2GUArme_VioletMage::ComboX4FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.166f ) )
	{
		if( FlushMp( 6.0f  * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
				IsTransformed() )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MAGICBALL_BIG", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			}
			else if ( m_bIceBall )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Big1", GetPowerRate() * m_fIceBallPower, posR,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			else
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_BIG", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX4EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	WALK_CANCEL_AFTER( m_fWalkCancelAfter )
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//AVSI_COMBO_X3front
void CX2GUArme_VioletMage::ComboX3frontStart()
{
	CommonStateStart();
	m_DamageData.attackerType	= CX2DamageManager::AT_MANUAL_COLLISION_DATA;
	m_DamageData.bCanRevenge	= false;

	if( FlushMp( 10.0f ) == true )
		m_bZ4up = true;
	else
		m_bZ4up = false;
}

void CX2GUArme_VioletMage::ComboX3frontFrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.46f && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.95f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::ComboX3frontFrameMove()
{
	if( m_bZ4up == true )
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.47f ) )
		{

			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );

			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_RING ) );
			if( NULL != pSeq )
			{
				PlaySequenceByTriggerTime( pSeq, pos, 5, 10, 0.4f );
				pSeq->SetAddRotate( D3DXVECTOR3( 0.0f, m_FrameDataNow.unitCondition.dirDegree.y + 70.0f, 0.0f ) );
			}

			if( GetIsRight() == true )
				pos += 20.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 20.0f * m_FrameDataNow.unitCondition.dirVector;

			pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_CENTER ) );
			PlaySequenceByTriggerTime( pSeq, pos, 5, 10, 0.4f );
		}

		if( m_pXSkinAnim->GetNowAnimationTime() > 0.47f && m_pXSkinAnim->GetNowAnimationTime() < 0.82f )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDXCollision::CollisionData data;
			data.m_fRadius = 100.0f;

			if( GetIsRight() == true )
				pos += 50.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 50.0f * m_FrameDataNow.unitCondition.dirVector;
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._41 = pos.x;
			mat._42 = pos.y;
			mat._43 = pos.z;
			data.m_pCombineMatrix = &mat;
			m_DamageData.pCollisionData = &data;

			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
		}

		if( EventTimer( 0.83f, false ) == true )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDXCollision::CollisionData data;
			data.m_fRadius = 100.0f;

			if( GetIsRight() == true )
				pos += 20.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 20.0f * m_FrameDataNow.unitCondition.dirVector;
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._41 = pos.x;
			mat._42 = pos.y;
			mat._43 = pos.z;
			data.m_pCombineMatrix = &mat;

			m_DamageData.pCollisionData = &data;
			m_DamageData.backSpeed.x	= GetDashSpeed() * 2.0f;
			m_DamageData.backSpeed.y	= GetJumpSpeed() * 0.3f;
			m_DamageData.bReAttack		= false;
			m_DamageData.hitUnitList.resize(0);

			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
			m_bZ4up = false;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
		CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX3frontEventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.4f )
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
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

//AVSI_COMBO_X3down
void CX2GUArme_VioletMage::ComboX3downFrameMove()
{
	if( m_pComboX3Up1->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
	{
		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataNow.unitCondition.bHit = true;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		if( FlushMp( 10.0f ) == true )
		{
			if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
				IsTransformed() )
			{
				m_pComboX3Up1->SetScale( D3DXVECTOR3(2.f, 1.f, 2.f) );
				m_pComboX3Up2->SetScale( D3DXVECTOR3(2.f, 1.f, 2.f) );
				m_pComboX3Up3->SetScale( D3DXVECTOR3(2.f, 1.f, 2.f) );
			}
			else
			{
				m_pComboX3Up1->SetScale( D3DXVECTOR3(1.f, 1.f, 1.f) );
				m_pComboX3Up2->SetScale( D3DXVECTOR3(1.f, 1.f, 1.f) );
				m_pComboX3Up3->SetScale( D3DXVECTOR3(1.f, 1.f, 1.f) );
			}
			m_pComboX3Up1->GetRenderParam()->color = 0xffffffff;
			m_pComboX3Up2->GetRenderParam()->color = 0xffffffff;
			m_pComboX3Up3->GetRenderParam()->color = 0xffffffff;

			D3DXVECTOR3 pos = GetPos();
			pos.y += 5.0f;
			m_pComboX3Up1->GetMatrix().Move( pos );
			m_pComboX3Up1->GetMatrix().RotateDegree( GetRotateDegree() );
			m_pComboX3Up2->GetMatrix().Move( pos );
			m_pComboX3Up2->GetMatrix().RotateDegree( GetRotateDegree() );
			m_pComboX3Up3->GetMatrix().Move( pos );
			m_pComboX3Up3->GetMatrix().RotateDegree( GetRotateDegree() );

			m_pComboX3Up1->ChangeAnim( L"ComboX3Up-1", false );
			m_pComboX3Up2->ChangeAnim( L"ComboX3Up-2", false );
			m_pComboX3Up3->ChangeAnim( L"ComboX3Up-3", false );

			m_pComboX3Up1->Play( CKTDGXSkinAnim::XAP_ONE );
			m_pComboX3Up2->Play( CKTDGXSkinAnim::XAP_ONE );
			m_pComboX3Up3->Play( CKTDGXSkinAnim::XAP_ONE );
			m_pComboX3Up3->SetPlaySpeed( 0.8f );

			m_DamageData.attackerType	= CX2DamageManager::AT_MESH;
			m_DamageData.bCanRevenge	= false;
			m_DamageData.pAttackerAnim	= m_pComboX3Up1;
		}
		else
		{
			CreateStepDust();
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::ComboX3downEventProcess()
{
	if( GetUnit()->GetUnitData().m_UnitClass == CX2Unit::UC_ARME_DIMENSION_WITCH )
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.801f )
		{
			StateChange( AVSI_ADW_COMBO_ADD_X );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.3f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.801f )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
	}

	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		//m_fCanNotAttackTime = 0.5f;
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_DARK_MAGICIAN_COMBO_X3down
void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X3down_FrameMove()
{
	if( m_pComboX3Up1->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
	{
		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataNow.unitCondition.bHit = true;
		}
	}

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		if( FlushMp( 10.0f ) == true )
		{
			m_pComboX3Up1->GetRenderParam()->color = 0xffffffff;
			m_pComboX3Up2->GetRenderParam()->color = 0xffffffff;
			m_pComboX3Up3->GetRenderParam()->color = 0xffffffff;

			D3DXVECTOR3 pos = GetPos();
			pos.y += 5.0f;
			m_pComboX3Up1->GetMatrix().Move( pos );
			m_pComboX3Up1->GetMatrix().RotateDegree( GetRotateDegree() );
			m_pComboX3Up2->GetMatrix().Move( pos );
			m_pComboX3Up2->GetMatrix().RotateDegree( GetRotateDegree() );
			m_pComboX3Up3->GetMatrix().Move( pos );
			m_pComboX3Up3->GetMatrix().RotateDegree( GetRotateDegree() );

			m_pComboX3Up1->ChangeAnim( L"ComboX3Up-1", false );
			m_pComboX3Up2->ChangeAnim( L"ComboX3Up-2", false );
			m_pComboX3Up3->ChangeAnim( L"ComboX3Up-3", false );

			m_pComboX3Up1->Play( CKTDGXSkinAnim::XAP_ONE );
			m_pComboX3Up2->Play( CKTDGXSkinAnim::XAP_ONE );
			m_pComboX3Up3->Play( CKTDGXSkinAnim::XAP_ONE );
			m_pComboX3Up3->SetPlaySpeed( 0.8f );

			m_DamageData.attackerType	= CX2DamageManager::AT_MESH;
			m_DamageData.bCanRevenge	= false;
			m_DamageData.pAttackerAnim	= m_pComboX3Up1;
		}
		else
		{
			CreateStepDust();
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X3down_EventProcess()
{
	if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() < 0.85f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.85f && m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
	{
		StateChange( AVSI_COMBO_X4down );
	}
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		//m_fCanNotAttackTime = 0.5f;
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_DARK_MAGICIAN_COMBO_X4down
void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X4down_Start()
{
	CommonStateStart();
	m_DamageData.attackerType	= CX2DamageManager::AT_MANUAL_COLLISION_DATA;
	m_DamageData.bCanRevenge	= false;

	if( FlushMp( 10.0f ) == true )
		m_bZ4up = true;
	else
		m_bZ4up = false;
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X4down_FrameMove()
{
	if( m_bZ4up == true )
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.44f ) )
		{

			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );


			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_RING ) );
			if( NULL != pSeq )
			{
				PlaySequenceByTriggerTime( pSeq, pos, 5, 10, 0.4f );
				pSeq->SetAddRotate( D3DXVECTOR3( 0.0f, m_FrameDataNow.unitCondition.dirDegree.y + 70.0f, 0.0f ) );
			}

			if( GetIsRight() == true )
				pos += 20.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 20.0f * m_FrameDataNow.unitCondition.dirVector;

			pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_SHOCK_CENTER ) );
			PlaySequenceByTriggerTime( pSeq, pos, 5, 10, 0.4f );
		}

		if( m_pXSkinAnim->GetNowAnimationTime() > 0.44f && m_pXSkinAnim->GetNowAnimationTime() < 0.9f )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDXCollision::CollisionData data;
			data.m_fRadius = 100.0f;

			if( GetIsRight() == true )
				pos += 50.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 50.0f * m_FrameDataNow.unitCondition.dirVector;
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._41 = pos.x;
			mat._42 = pos.y;
			mat._43 = pos.z;
			data.m_pCombineMatrix = &mat;
			m_DamageData.pCollisionData = &data;

			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
		}

		if( EventTimer( 0.91f, false ) == true )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CKTDXCollision::CollisionData data;
			data.m_fRadius = 100.0f;

			if( GetIsRight() == true )
				pos += 20.0f * m_FrameDataNow.unitCondition.dirVector;
			else
				pos -= 20.0f * m_FrameDataNow.unitCondition.dirVector;
			D3DXMATRIX mat;
			D3DXMatrixIdentity( &mat );
			mat._41 = pos.x;
			mat._42 = pos.y;
			mat._43 = pos.z;
			data.m_pCombineMatrix = &mat;

			m_DamageData.pCollisionData = &data;
			m_DamageData.backSpeed.x	= GetDashSpeed() * 2.0f;
			m_DamageData.backSpeed.y	= GetJumpSpeed() * 0.3f;
			m_DamageData.bReAttack		= false;
			m_DamageData.hitUnitList.resize(0);

			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
			m_bZ4up = false;
		}
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
		CreateNotEnoughMPEffect( pos, 0.f, 0.f, 30.f );

	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X4down_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.4f )
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
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_HIGH_MAGICIAN_COMBO_X2
void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X2_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.53f ) )
	{
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			if ( m_bIceBall )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Short", GetPowerRate() * m_fIceBallPower, posR,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SHORT", GetPowerRate(), posR, GetRotateDegree(),
					GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X2_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
	{
		if( m_InputData.pureDown == true )
			StateChange( AVSI_COMBO_X3down );
		else if( m_FrameDataFuture.syncData.bIsRight == true && m_InputData.pureRight == true )
			StateChange( AVSI_COMBO_X3front );
		else if( m_FrameDataFuture.syncData.bIsRight == false && m_InputData.pureLeft == true )
			StateChange( AVSI_COMBO_X3front );
		else if( m_FrameDataFuture.syncData.bIsRight == false && m_InputData.pureRight == true )
			StateChange( AVSI_COMBO_X3back );
		else if( m_FrameDataFuture.syncData.bIsRight == true && m_InputData.pureLeft == true )
			StateChange( AVSI_COMBO_X3back );
		else
		{
			StateChange( AVSI_COMBO_X3 );
			m_bCanChangeState = true;		/// ���̾ ���� ������Ʈ ��� ����
		}
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.9f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	ELSE_IF_STATE_CHANGE_ON_( 1, 0.86f, 0.86f, true == m_InputData.oneZ && GetUnit()->GetClass() == CX2Unit::UC_ARME_ELEMENTAL_MASTER , AVSI_AEM_XZ )

	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.966f )
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
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_HIGH_MAGICIAN_COMBO_X3back
void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X3back_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.34f ) )
	{
		if( FlushMp( 6.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 rot = GetRotateDegree();
			rot.y += 180.0f;
			if ( m_bIceBall )
				// ������ ����Ʈ �̸� ���� ( �ߺ��� ������ ����Ʈ ���� )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ICEBALL_BIG_AISHA_HIGH_MAGICIAN", GetPowerRate() * m_fIceBallPower, posR,
				rot, rot, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_BIG_AISHA_HIGH_MAGICIAN", GetPowerRate(), posR,
					rot, rot, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );

			CreateNotEnoughMPEffect( pos, 0.f, 180.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X3back_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.4f )
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
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X3back_EndFuture()
{
	CommonStateEndFuture();

	m_FrameDataFuture.syncData.bIsRight = !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}


//AVSI_DARK_MAGICIAN_COMBO_X2
void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X2_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.53f ) )
	{
		if( FlushMp( 7.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SHORT", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X2_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	SKILL_CANCEL_AFTER( 0.01f )
	else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
	{
		if( m_InputData.pureDown == true )
			StateChange( AVSI_COMBO_X3down );
		else if( m_FrameDataFuture.syncData.bIsRight == true && m_InputData.pureRight == true )
			StateChange( AVSI_COMBO_X3front );
		else if( m_FrameDataFuture.syncData.bIsRight == false && m_InputData.pureLeft == true )
			StateChange( AVSI_COMBO_X3front );
		else if( m_FrameDataFuture.syncData.bIsRight == false && m_InputData.pureRight == true )
			StateChange( AVSI_COMBO_X3back );
		else if( m_FrameDataFuture.syncData.bIsRight == true && m_InputData.pureLeft == true )
			StateChange( AVSI_COMBO_X3back );
		else
		{
			m_bCanChangeState = true;		/// ���̾ ���� ������Ʈ ��� ����
			StateChange( AVSI_COMBO_X3 );
		}

		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.9f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.966f )
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
	}
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_DARK_MAGICIAN_COMBO_X3back
void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X3back_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.71f ) )
	{
		if( FlushMp( 10.0f ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DARK_SHOCK_BALL", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_COMBO_X3back_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 1.4f )
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
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_DASH_COMBO_Z1
void CX2GUArme_VioletMage::DashComboZ1StartFuture()
{
	CommonStateStartFuture();

	if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
		IsTransformed() )
		m_PhysicParam.nowSpeed.x = 1000.f;
}

void CX2GUArme_VioletMage::DashComboZ1FrameMove()
{
	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashComboZ1EventProcess()
{
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.46f
		&& true == IsOnSomethingFuture() )
	{
		StateChange( AVSI_DASH_COMBO_Z2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_FrameDataFuture.stateParam.bEventFlagList[1] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.46f
		&& true == IsOnSomethingFuture() )
	{
		if( GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
			StateChange( AVSI_ABM_DASH_COMBO_Z2a );

		else if( GetUnit()->GetClass() == CX2Unit::UC_ARME_ELEMENTAL_MASTER )
			StateChange( AVSI_EM_DASH_COMBO_Z2a );

		else
			StateChange( AVSI_DASH_COMBO_Z2a );

		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.46f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.46f )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		m_FrameDataFuture.stateParam.bEventFlagList[1] = true;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_C
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.65f )
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

//AVSI_DASH_COMBO_Z2
void CX2GUArme_VioletMage::DashComboZ2StartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.2f;
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed() * 1.2f;
	}
}

//void CX2GUArme_VioletMage::DashComboZ2FrameMove()
//{
//
//	CommonFrameMove();
//}

void CX2GUArme_VioletMage::DashComboZ2EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.55f )
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

//AVSI_DASH_COMBO_Z2a
void CX2GUArme_VioletMage::DashComboZ2aStartFuture()
{
	CommonStateStartFuture();

	bool bMove = false;
	D3DXVECTOR3 pos = GetPos();
	for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( pUnit->GetTeam() == GetTeam() )
			continue;

		if( pUnit->GetInvincible() == true )
			continue;

		if( pUnit->GetNowHp() <= 0.f )
			continue;

		//{{ oasis907 : ����� [2011.3.21] ������ ���� �� �ִ� ���� üũ
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pUnit->GetLastTouchLineIndex() );
		if( NULL == pLineData )
			continue;

		if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
			continue;
		//}} oasis907 : ����� [2011.3.21]

		if( GetDistance( pos, pUnit->GetPos() ) < 700.0f )
		{
			if( pUnit->GetPos().y < pLineData->startPos.y )
				continue;

			m_FrameDataFuture.syncData.position		= pUnit->GetPos();
			m_FrameDataFuture.syncData.position.y	+= 350.0f;
			bMove = true;
			break;
		}
	}

	if( bMove == false )
		m_FrameDataFuture.syncData.position.y	+= 350.0f;

	m_FrameDataFuture.stateParam.bCanPushUnit = false;
	m_FrameDataFuture.stateParam.bCanPassUnit = true;

	for( int i = 0; i < (int)(0.05f * 85.0f); i++ )
	{
		m_PhysicParam.nowSpeed.x = -700.f;
		CX2GUUser::PhysicProcess();
	}

	m_FrameDataFuture.stateParam.bCanPushUnit = true;
	m_FrameDataFuture.stateParam.bCanPassUnit = false;
}

void CX2GUArme_VioletMage::DashComboZ2aFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
		pos.y -= 200.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
			pos,
			m_FrameDataNow.unitCondition.dirDegree,
			m_FrameDataNow.unitCondition.dirDegree,
			XL_EFFECT_0 );

	}
	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashComboZ2aEventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange(USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_EM_DASH_COMBO_Z2a_StartFuture()
{
	CommonStateStartFuture();

	bool bMove = false;
	D3DXVECTOR3 pos = GetPos();
	for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( pUnit->GetTeam() == GetTeam() )
			continue;

		if( pUnit->GetInvincible() == true )
			continue;

		if( pUnit->GetNowHp() <= 0.f )
			continue;

		//{{ oasis907 : ����� [2011.3.21] ������ ���� �� �ִ� ���� üũ
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pUnit->GetLastTouchLineIndex() );
		if( NULL == pLineData )
			continue;

		if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
			continue;
		//}} oasis907 : ����� [2011.3.21]

		if( GetDistance( pos, pUnit->GetPos() ) < 700.0f )
		{
			if( pUnit->GetPos().y < pLineData->startPos.y )
				continue;

			m_FrameDataFuture.syncData.position		= pUnit->GetPos();
			m_FrameDataFuture.syncData.position.y	+= 350.0f;
			bMove = true;
			break;
		}
	}

	if( bMove == false )
		m_FrameDataFuture.syncData.position.y	+= 350.0f;

	m_FrameDataFuture.stateParam.bCanPushUnit = false;
	m_FrameDataFuture.stateParam.bCanPassUnit = true;

	for( int i = 0; i < (int)(0.05f * 85.0f); i++ )
	{
		m_PhysicParam.nowSpeed.x = -700.f;
		CX2GUUser::PhysicProcess();
	}

	m_FrameDataFuture.stateParam.bCanPushUnit = true;
	m_FrameDataFuture.stateParam.bCanPassUnit = false;
}

void CX2GUArme_VioletMage::AVSI_EM_DASH_COMBO_Z2a_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
		pos.y -= 200.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
			pos,
			m_FrameDataNow.unitCondition.dirDegree,
			m_FrameDataNow.unitCondition.dirDegree,
			XL_EFFECT_0 );

	}
	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_EM_DASH_COMBO_Z2a_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange(USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( USI_WAIT );
	}
	else if( true == m_FrameDataFuture.stateParam.bEventFlagList[0] && 0.608f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		StateChange( AVSI_COMBO_X3down );
	}
	else if( true == m_InputData.oneX && 0.468f < m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

		CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_Start()
{
	CommonStateStart();

	AdaptAdvancedTeleportationBuff();
}

//AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a
void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_StartFuture()
{
	CommonStateStartFuture();

	bool bMove = false;
	D3DXVECTOR3 pos = GetPos();
	for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
	{
		CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
		if( pUnit->GetTeam() == GetTeam() )
			continue;

		if( pUnit->GetInvincible() == true )
			continue;

		if( pUnit->GetNowHp() <= 0.f )
			continue;

		//{{ oasis907 : ����� [2011.3.21] ������ ���� �� �ִ� ���� üũ
		const CKTDGLineMap::LineData* pLineData = g_pX2Game->GetWorld()->GetLineMap()->GetLineData( pUnit->GetLastTouchLineIndex() );
		if( NULL == pLineData )
			continue;

		if( pLineData->lineType != CKTDGLineMap::LT_NORMAL || pLineData->bEnable == false )
			continue;
		//}} oasis907 : ����� [2011.3.21]

		if( GetDistance( pos, pUnit->GetPos() ) < 700.0f )
		{
			if( pUnit->GetPos().y < pLineData->startPos.y )
				continue;

			m_FrameDataFuture.syncData.position		= pUnit->GetPos();
			m_FrameDataFuture.syncData.position.y	+= 350.0f;
			bMove = true;

			break;
		}
	}

	if( bMove == false )
		m_FrameDataFuture.syncData.position.y	+= 350.0f;

	m_FrameDataFuture.stateParam.bCanPushUnit = false;
	m_FrameDataFuture.stateParam.bCanPassUnit = true;

	for( int i = 0; i < (int)(0.05f * 85.0f); i++ )
	{
		m_PhysicParam.nowSpeed.x = -700.f;
		CX2GUUser::PhysicProcess();
	}

	m_FrameDataFuture.stateParam.bCanPushUnit = true;
	m_FrameDataFuture.stateParam.bCanPassUnit = false;
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
	{
		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
		pos.y -= 200.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
			pos,
			m_FrameDataNow.unitCondition.dirDegree,
			m_FrameDataNow.unitCondition.dirDegree,
			XL_EFFECT_0 );

	}
	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_EventProcess()
{
	if( m_InputData.oneX == true )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.47f )
	{
		StateChange( AVSI_DASH_COMBO_Z3up );
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


//AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up
void CX2GUArme_VioletMage::AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up_EventProcess()
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
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.75f )
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


//AVSI_DASH_ATTACK_X
void CX2GUArme_VioletMage::DashAttackXStartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
}

void CX2GUArme_VioletMage::DashAttackXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		//�ĺ�
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.y += 180.0f;

			switch( GetUnit()->GetClass() )
			{
			case CX2Unit::UC_ARME_BATTLE_MAGICIAN:
				{
					if( IsTransformed() )
					{
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MAGICBALL_SMALL", GetPowerRate(), posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					}
					else
					{
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL", GetPowerRate(), posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					}
				}
				break;
			case CX2Unit::UC_ARME_DIMENSION_WITCH:
				{
					if( IsTransformed() )
					{
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ADW_MAGICBALL_BIG_SLOW", GetPowerRate(), posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					}
					else
					{
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ADW_MAGICBALL_SMALL_SLOW", GetPowerRate(), posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					}
				}
				break;
			default:
				{
					if ( m_bIceBall )
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Small", GetPowerRate() * m_fIceBallPower, posR, angle, angle,
						m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					else
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL", GetPowerRate(), posR, angle, angle,
							m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
				}
				break;
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 180.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashAttackXEventProcess()
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

void CX2GUArme_VioletMage::DashAttackXEndFuture()
{
	CommonStateEndFuture();

	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}


//AVSI_HIGH_MAGICION_DASH_ATTACK_X
void CX2GUArme_VioletMage::AVSI_HIGH_MAGICION_DASH_ATTACK_X_StartFuture()
{
	CommonStateStartFuture();

	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x = GetDashSpeed();
	}
}

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICION_DASH_ATTACK_X_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		//�ĺ�
		if( FlushMp( 5.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.y += 180.0f;
			if ( m_bIceBall )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Small", GetPowerRate() * m_fIceBallPower, posR, angle, angle,
				m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL", GetPowerRate(), posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 180.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICION_DASH_ATTACK_X_EventProcess()
{
	static float fChangeStateTime = 0.7f;
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > fChangeStateTime )
	{
		StateChange( AVSI_DASH_ATTACK_X2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() <= fChangeStateTime )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
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

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICION_DASH_ATTACK_X_EndFuture()
{
	CommonStateEndFuture();

	m_FrameDataFuture.syncData.bIsRight	= !m_FrameDataFuture.syncData.bIsRight;
	m_FrameDataFuture.unitCondition.dirDegree.y += 180.0f;
}


//AVSI_HIGH_MAGICION_DASH_ATTACK_X2
void CX2GUArme_VioletMage::AVSI_HIGH_MAGICION_DASH_ATTACK_X2_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.27f ) )
	{
		if( FlushMp( 10.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );

			if ( m_bIceBall )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HIGH_MAGICIAN_DASH_COMBO_XX_ICEBALL", GetPowerRate() * m_fIceBallPower, posR,
				GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );

			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HIGH_MAGICIAN_DASH_COMBO_XX_FIREBALL", GetPowerRate(), posR,
					GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_HIGH_MAGICION_DASH_ATTACK_X2_EventProcess()
{
	if( false == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_DOWN );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}
	ELSE_IF_KEY_PRESSED_AT_SKIP_POINT_ZXC
	{
		if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.9f )
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
	else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}


//AVSI_JUMP_ATTACK_Z
void CX2GUArme_VioletMage::JumpAttackZFrameMoveFuture()
{
	if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
	{
		m_PhysicParam.nowSpeed.x = GetWalkSpeed();
	}
	if( m_InputData.pureDown == true  && m_InputData.pureUp == false )
	{
		m_PhysicParam.nowSpeed.y -= m_PhysicParam.fGAccel * m_fElapsedTime;
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}


	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::JumpAttackZFrameMove()
{
	CommonFrameMove();
}

void CX2GUArme_VioletMage::JumpAttackZEventProcess()
{
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

//AVSI_JUMP_ATTACK_X
void CX2GUArme_VioletMage::JumpAttackXFrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.13f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.26f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}
	else
	{
		if( m_InputData.pureRight == true && m_FrameDataFuture.syncData.bIsRight == true )
		{
			m_PhysicParam.nowSpeed.x = GetWalkSpeed();
		}
		else if( m_InputData.pureLeft == true && m_FrameDataFuture.syncData.bIsRight == false )
		{
			m_PhysicParam.nowSpeed.x = GetWalkSpeed();
		}
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::JumpAttackXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.16f ) )
	{
		//�ĺ�
		if(  FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//m_pSoundSlash->Play();
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
				IsTransformed() )
			{
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"MAGICBALL_SMALL_DOWN", GetPowerRate(), posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			}
			else
			{
				if ( m_bIceBall )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Small_Down", GetPowerRate() * m_fIceBallPower, posR,
						angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
				else if ( m_bResearchGravityBall )
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_GRAVITY_BALL", m_fGravityBallPower, posR, angle,
						angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
				else
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL_DOWN", GetPowerRate(), posR, angle,
					angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			}
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"MagicFail", pos, 100, 100, -1, 1 );
			pSeq->SetAxisAngle( angle );
			pSeq->SetAddRotate( angle );
		}

	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::JumpAttackXEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_JUMP_LANDING );
	}


	CommonEventProcess();
}

//AVSI_DASH_JUMP_ATTACK_Z
void CX2GUArme_VioletMage::DashJumpAttackZFrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.26f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.4f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}
    else if( m_pXSkinAnimFuture->EventTimerOneshot( 0.41f ) )
	{
		m_PhysicParam.nowSpeed.x -= GetDashSpeed();
		m_PhysicParam.nowSpeed.y = GetJumpSpeed() / 2.0f;
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::DashJumpAttackZFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.27f ) )
	{
		if( FlushMp( 5.0f ) == true )
		{
			PlaySound( L"flame.ogg" );
			PlaySound( L"Sound_Arme_shout2.ogg" );

			D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );


			CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleAishaMinorParticleByEnum( AISHA_PII_MINOR_AIR_BLAST_RING ) );
			if( NULL != pSeq )
			{
				PlaySequenceByTriggerCount( pSeq, pos, 1000, 1000,  1 );

				if( GetIsRight() == true )
					pSeq->SetAddRotate( D3DXVECTOR3( 30.0f, m_FrameDataNow.unitCondition.dirDegree.y + 70.0f, 0.0f ) );
				else
					pSeq->SetAddRotate( D3DXVECTOR3( -30.0f, m_FrameDataNow.unitCondition.dirDegree.y - 70.0f, 0.0f ) );
			}


			pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_AIR_BLAST_CENTER ) );
			if( NULL != pSeq )
			{
				PlaySequenceByTriggerTime( pSeq, pos, 10, 20, 0.4f );
			}

			m_bDashJumpZ = true;
			m_bDashJumpZPos = pos;
		}
		else
		{
			D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, -30.f );


			m_bDashJumpZ = false;
		}
	}

	if( m_pXSkinAnim->GetNowAnimationTime() > 0.28f && m_pXSkinAnim->GetNowAnimationTime() < 0.7f && m_bDashJumpZ == true )
	{
		m_DamageData.attackerType	= CX2DamageManager::AT_MANUAL_COLLISION_DATA;
		m_DamageData.bCanRevenge	= false;
		CKTDXCollision::CollisionData data;
		data.m_fRadius = 100.0f;

		D3DXMATRIX mat;
		D3DXMatrixIdentity( &mat );
		mat._41 = m_bDashJumpZPos.x;
		mat._42 = m_bDashJumpZPos.y;
		mat._43 = m_bDashJumpZPos.z;
		data.m_pCombineMatrix = &mat;
		m_DamageData.pCollisionData = &data;

		if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
		{
			m_FrameDataNow.unitCondition.bHit = true;
			m_FrameDataFuture.unitCondition.bHit = m_FrameDataNow.unitCondition.bHit;
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashJumpAttackZEventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	CommonEventProcess();
}

//AVSI_DASH_JUMP_ATTACK_X
void CX2GUArme_VioletMage::DashJumpAttackXFrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.13f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.27f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::DashJumpAttackXFrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.16f ) )
	{
		//�ĺ����
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//m_pSoundSlash->Play();
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.z = -30.0f;
			if ( m_bIceBall )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Small", GetPowerRate() * m_fIceBallPower, posR, angle,
					angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL", GetPowerRate(), posR, angle, angle,
					m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );

		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, -30.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashJumpAttackXEventProcess()
{
	if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.4f )
	{
		StateChange( AVSI_DASH_JUMP_ATTACK_X2 );
		m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
	}
	else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() )
	{
		m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
	}

	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}
	//else if( m_pXSkinAnimFuture->IsAnimationEnd() == true && false == IsOnSomethingFuture() )
	//{
	//	m_pXSkinAnimFuture->Stop();
	//	m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	//}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::DashJumpAttackX2FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.13f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.27f )
	{
		m_PhysicParam.nowSpeed.x = 0.0f;
		m_PhysicParam.nowSpeed.y = 0.0f;
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::DashJumpAttackX2FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.16f ) )
	{
		//�ĺ����
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
		{
			//���̾���
			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			D3DXVECTOR3 angle = GetRotateDegree();
			angle.z = -30.0f;

			if ( m_bIceBall )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_IceBall_Small_Down", GetPowerRate() * m_fIceBallPower, posR, angle,
					angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
			else
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"FIREBALL_SMALL_DOWN", GetPowerRate(), posR, angle, angle,
					m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
		}
		else
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
			CreateNotEnoughMPEffect( pos, 0.f, 0.f, -30.f );

		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::DashJumpAttackX2EventProcess()
{
	if( true == IsOnSomethingFuture() )
	{
		StateChange( USI_DASH_JUMP_LANDING );
	}

	CommonEventProcess();
}


////////////////////////////////////////////////////////////////////////
//AVSI_SI_AV_POWER_ATTACK
void CX2GUArme_VioletMage::AVSI_SI_AV_POWER_ATTACK_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.06f, 0 );

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI_AV_POWER_ATTACK_EventProcess()
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


//AVSI_SI_AV_POWER_ATTACK_HYPER
void CX2GUArme_VioletMage::AVSI_SI_AV_POWER_ATTACK_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.06f, 0 );

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI_AV_POWER_ATTACK_HYPER_EventProcess()
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

//AVSI_SI1_AV_BINDING_CIRCLE
void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.001f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
// 		D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01_Spine1" );
//
// 		CKTDGParticleSystem::CParticleEventSequence* pSeq = NULL;
//
		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_AV_BINDING_CIRCLE );
		if ( NULL != pSkillTemplet && NULL != g_pX2Game->GetEffectSet() )
		{
			const int iSkillTempletLevel = userSkillTree.GetSkillLevel( pSkillTemplet->m_eID );	/// ��ų ����
			float m_fBindingCircleDistance				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iSkillTempletLevel );			/// �ѹߴ� �Һ� ����
			CX2EffectSet::Handle hBindingCircleEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_BINDING_CIRCLE_LEVEL_01", this );
			CX2EffectSet::EffectSetInstance * pBindingCircleEffectSetInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hBindingCircleEffectSet );
			if ( NULL !=  pBindingCircleEffectSetInst )
			{
				float fScale = m_fBindingCircleDistance / _CONST_AISHA_::BIDING_CIRCLE_BASIC_DISTANCE;
				D3DXVECTOR3 vScale ( fScale, fScale, fScale );
				pBindingCircleEffectSetInst->SetEffectScale( vScale );
			}
		}



	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 pos = GetPos();

		int nCount = 0;
		for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
			if( pUnit->GetTeam() == GetTeam() )
				continue;

			if( pUnit->GetInvincible() == true )
				continue;

			float fLength = 0.0f;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_AV_BINDING_CIRCLE );
			if ( NULL != pSkillTemplet )
			{
				const int iSkillTempletLevel = userSkillTree.GetSkillLevel( pSkillTemplet->m_eID );	/// ��ų ����
				fLength				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iSkillTempletLevel );			/// �ѹߴ� �Һ� ����
				fLength *= fLength;
			}

			if( GetDistance3Sq( pos, pUnit->GetPos() ) < fLength )
			{
				nCount++;
// 				pUnit->SetStopTime( 2.0f );
// 				D3DXVECTOR3 pos2 = pUnit->GetBonePos( L"Bip01" );
// 				g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"TimeStopRingX", pos2, 100, 100, -1, 1 );
// 				g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"TimeStopRingY", pos2, 100, 100, -1, 1 );
// 				g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"TimeStopRingZ", pos2, 100, 100, -1, 1 );

				pUnit->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_BIND ) );
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO2 ) == true )
				{
					pUnit->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_BIND_WITH_MEMO ) );
					//pUnit->SetAnimSpeedFactor( 0.8f, 10.f, CX2GameUnit::SFI_SPEED_1 );
				}
			}
		}

		if( nCount > 0 )
		{
			PlaySound( L"Aisha_special_attack1.ogg" );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
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

void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_EventProcess()
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

void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_End()
{
	CommonStateEnd();
}

//AVSI_SI1_AV_BINDING_CIRCLE_HYPER
void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.001f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{

		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_AV_BINDING_CIRCLE );
		if ( NULL != pSkillTemplet && NULL != g_pX2Game->GetEffectSet() )
		{
			const int iSkillTempletLevel = userSkillTree.GetSkillLevel( pSkillTemplet->m_eID );
			float m_fBindingCircleDistance				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iSkillTempletLevel );
			CX2EffectSet::Handle hBindingCircleEffectSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_BINDING_CIRCLE_HYPER_LEVEL_01", this );
			CX2EffectSet::EffectSetInstance * pBindingCircleEffectSetInst = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hBindingCircleEffectSet );
			if ( NULL !=  pBindingCircleEffectSetInst )
			{
				float fScale = m_fBindingCircleDistance / _CONST_AISHA_::BIDING_CIRCLE_BASIC_DISTANCE;
				D3DXVECTOR3 vScale ( fScale, fScale, fScale );
				pBindingCircleEffectSetInst->SetEffectScale( vScale );
			}
		}

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.5f ) )
	{
		D3DXVECTOR3 pos = GetPos();

		float fLength = 0.0f;
		const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_A_AV_BINDING_CIRCLE );
		if ( NULL != pSkillTemplet )
		{
			const int iSkillTempletLevel = userSkillTree.GetSkillLevel( pSkillTemplet->m_eID );	/// ��ų ����
			fLength				= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_SCOPE_ABS, iSkillTempletLevel );			/// �ѹߴ� �Һ� ����
			fLength *= fLength;
		}

		int nCount = 0;
		for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
		{
			CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
			if( pUnit->GetTeam() == GetTeam() )
				continue;

			if( pUnit->GetInvincible() == true )
				continue;

			if( GetDistance3Sq( pos, pUnit->GetPos() ) < fLength )
			{
				nCount++;
				pUnit->SetStopTime( 3.0f );
				D3DXVECTOR3 pos2 = pUnit->GetBonePos( L"Bip01" );
				g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"TimeStopRingXRed", pos2, 100, 100, -1, 1 );
				g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"TimeStopRingYRed", pos2, 100, 100, -1, 1 );
				g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"TimeStopRingZRed", pos2, 100, 100, -1, 1 );

				if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO2 ) == true )
				{
					pUnit->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_BIND_WITH_MEMO ) );
					//pUnit->SetAnimSpeedFactor( 0.8f, 10.f, CX2GameUnit::SFI_SPEED_1 );
				}
			}
		}
		if( nCount > 0 )
		{
			PlaySound( L"Aisha_special_attack1.ogg" );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_HYPER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
	case 1:
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

void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_HYPER_EventProcess()
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

void CX2GUArme_VioletMage::AVSI_SI1_AV_BINDING_CIRCLE_HYPER_End()
{
	CommonStateEnd();
}



//AVSI_SI2_AV_LIGHTNING_BOLT
void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.2f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{


		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO8 ) == true )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Lightning_Bolt_Memo", this );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Lightning_Bolt", this );
		}



		UpDownCrashCamera( 20.0f, 0.7f );
		g_pKTDXApp->GetDGManager()->ClearScreen();


		D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning1 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_01 ) );
		if( NULL != pSeqLightning1 )
		{
			PlaySequenceByTriggerTime( pSeqLightning1, pos,  5,9, 0.7f );
			pSeqLightning1->SetAddRotate( GetRotateDegree() );

			pSeqLightning1->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning2 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_02 ) );
		if( NULL != pSeqLightning2 )
		{
			PlaySequenceByTriggerTime( pSeqLightning2, pos,  5,9, 0.6f );
			pSeqLightning2->SetAddRotate( GetRotateDegree() );

			pSeqLightning2->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning3 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_03 ) );
		if( NULL != pSeqLightning3 )
		{
			PlaySequenceByTriggerTime( pSeqLightning3, pos, 5,7, 0.8f );
			pSeqLightning3->SetAddRotate( GetRotateDegree() );

			pSeqLightning3->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning4 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_04 ) );
		if( NULL != pSeqLightning4 )
		{
			PlaySequenceByTriggerTime( pSeqLightning4, pos, 5,7, 0.7f  );
			pSeqLightning4->SetAddRotate( GetRotateDegree() );


			pSeqLightning4->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning5 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_05 ) );
		if( NULL != pSeqLightning5 )
		{
			PlaySequenceByTriggerTime( pSeqLightning5, pos,5,9, 0.65f  );
			pSeqLightning5->SetAddRotate( GetRotateDegree() );


			pSeqLightning5->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning6 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_06 ) );
		if( NULL != pSeqLightning6 )
		{
			PlaySequenceByTriggerTime( pSeqLightning6, pos, 5,7, 0.75f );
			pSeqLightning6->SetAddRotate( GetRotateDegree() );


			pSeqLightning6->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning7 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_07 ) );
		if( NULL != pSeqLightning7 )
		{
			PlaySequenceByTriggerTime( pSeqLightning7, pos, 1,2, 0.7f );
			pSeqLightning7->SetAddRotate( GetRotateDegree() );


			pSeqLightning7->SetScaleByUnit( GetScaleByUnit() );

		}



		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning8 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_08 ) );
		if( NULL != pSeqLightning8 )
		{
			PlaySequenceByTriggerTime( pSeqLightning8, pos, 1,2, 0.6f );
			pSeqLightning8->SetAddRotate( GetRotateDegree() );

			pSeqLightning8->SetScaleByUnit( GetScaleByUnit() );


		}


		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning9 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_09 ) );
		if( NULL != pSeqLightning9 )
		{
			PlaySequenceByTriggerTime( pSeqLightning9, pos, 1,2, 0.8f );
			pSeqLightning9->SetAddRotate( GetRotateDegree() );

			pSeqLightning9->SetScaleByUnit( GetScaleByUnit() );


		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning10 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_10 ) );
		if( NULL != pSeqLightning10 )
		{
			PlaySequenceByTriggerTime( pSeqLightning10, pos, 1,2, 0.7f  );
			pSeqLightning10->SetAddRotate( GetRotateDegree() );

			pSeqLightning10->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning11 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_11 ) );
		if( NULL != pSeqLightning11 )
		{
			PlaySequenceByTriggerTime( pSeqLightning11, pos,  1,2, 0.65f  );
			pSeqLightning11->SetAddRotate( GetRotateDegree() );

			pSeqLightning11->SetScaleByUnit( GetScaleByUnit() );


		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning12 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_12 ) );
		if( NULL != pSeqLightning12 )
		{
			PlaySequenceByTriggerTime( pSeqLightning12, pos, 1,2, 0.75f );
			pSeqLightning12->SetAddRotate( GetRotateDegree() );

			pSeqLightning12->SetScaleByUnit( GetScaleByUnit() );

		}


		CKTDGParticleSystem::CParticleEventSequence* pSeqLightningCore = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_CORE ) );

		if( NULL != pSeqLightningCore )
		{

			PlaySequenceByTriggerTime( pSeqLightningCore, pos, 50,50, 0.75f );


			pSeqLightningCore->SetScaleByUnit( GetScaleByUnit() );

		}


	}



	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
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

void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_EventProcess()
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

void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_End()
{
	CommonStateEnd();
}

//AVSI_SI2_AV_LIGHTNING_BOLT_HYPER
void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.2f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO8 ) == true )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Lightning_Bolt_Memo", this, NULL, true );
		}
		else
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Lightning_Bolt", this, NULL, true );
		}

		UpDownCrashCamera( 20.0f, 0.7f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

		D3DXVECTOR3 pos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );


		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning1 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_01 ) );
		if( NULL != pSeqLightning1 )
		{
			PlaySequenceByTriggerTime( pSeqLightning1, pos,   8,12, 0.6f );
			pSeqLightning1->SetAddRotate( GetRotateDegree() );

			pSeqLightning1->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning2 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_02 ) );
		if( NULL != pSeqLightning2 )
		{
			PlaySequenceByTriggerTime( pSeqLightning2, pos,  8,10, 0.8f );
			pSeqLightning2->SetAddRotate( GetRotateDegree() );

			pSeqLightning2->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning3 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_03 ) );
		if( NULL != pSeqLightning3 )
		{
			PlaySequenceByTriggerTime( pSeqLightning3, pos,  8,10, 0.7f );
			pSeqLightning3->SetAddRotate( GetRotateDegree() );


			pSeqLightning3->SetScaleByUnit( GetScaleByUnit() );

		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning4 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_04 ) );
		if( NULL != pSeqLightning4 )
		{
			PlaySequenceByTriggerTime( pSeqLightning4, pos, 8,10, 0.7f );
			pSeqLightning4->SetAddRotate( GetRotateDegree() );

			pSeqLightning4->SetScaleByUnit( GetScaleByUnit() );


		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning5 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_05 ) );
		if( NULL != pSeqLightning5 )
		{
			PlaySequenceByTriggerTime( pSeqLightning5, pos,  8,12, 0.7f  );
			pSeqLightning5->SetAddRotate( GetRotateDegree() );

			pSeqLightning5->SetScaleByUnit( GetScaleByUnit() );

		}
		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning6 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_06 ) );
		if( NULL != pSeqLightning6 )
		{
			PlaySequenceByTriggerTime( pSeqLightning6, pos,8,10, 0.65f );
			pSeqLightning6->SetAddRotate( GetRotateDegree() );


			pSeqLightning6->SetScaleByUnit( GetScaleByUnit() );

		}
		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning7 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_07 ) );
		if( NULL != pSeqLightning7 )
		{
			PlaySequenceByTriggerTime( pSeqLightning7, pos, 4,6, 0.75f  );
			pSeqLightning7->SetAddRotate( GetRotateDegree() );

			pSeqLightning7->SetScaleByUnit( GetScaleByUnit() );

		}






		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning8 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_08 ) );
		if( NULL != pSeqLightning8 )
		{
			PlaySequenceByTriggerTime( pSeqLightning8, pos,  4,5, 0.75f );
			pSeqLightning8->SetAddRotate( GetRotateDegree() );

			pSeqLightning8->SetScaleByUnit( GetScaleByUnit() );



		}
		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning9 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_09 ) );
		if( NULL != pSeqLightning9 )
		{
			PlaySequenceByTriggerTime( pSeqLightning9, pos,  4,5, 0.75f );
			pSeqLightning9->SetAddRotate( GetRotateDegree() );

			pSeqLightning9->SetScaleByUnit( GetScaleByUnit() );



		}
		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning10 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_10 ) );
		if( NULL != pSeqLightning10 )
		{
			PlaySequenceByTriggerTime( pSeqLightning10, pos,  4,5, 0.75f  );
			pSeqLightning10->SetAddRotate( GetRotateDegree() );

			pSeqLightning10->SetScaleByUnit( GetScaleByUnit() );


		}
		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning11 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_11 ) );
		if( NULL != pSeqLightning11 )
		{
			PlaySequenceByTriggerTime( pSeqLightning11, pos,   4,5, 0.75f  );
			pSeqLightning11->SetAddRotate( GetRotateDegree() );

			pSeqLightning11->SetScaleByUnit( GetScaleByUnit() );


		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqLightning12 = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_12 ) );
		if( NULL != pSeqLightning12 )
		{
			PlaySequenceByTriggerTime( pSeqLightning12, pos,  4,5, 0.75f );
			pSeqLightning12->SetAddRotate( GetRotateDegree() );


			pSeqLightning12->SetScaleByUnit( GetScaleByUnit() );

		}





		CKTDGParticleSystem::CParticleEventSequence* pSeqLightningCore = g_pX2Game->GetMajorParticle()->GetInstanceSequence( GetHandleAishaMajorParticleByEnum( AISHA_PII_MAJOR_LIGHTNING_BOLT_CORE ) );

		if( NULL != pSeqLightningCore )
		{
			PlaySequenceByTriggerTime( pSeqLightningCore, pos, 50,50, 0.75f );
			pSeqLightningCore->SetScaleByUnit( GetScaleByUnit() );

		}


	}



	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_CameraMove()
{
	CommonCameraMove();

	switch( m_FrameDataNow.stateParam.normalCamera )
	{
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

void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_EventProcess()
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

void CX2GUArme_VioletMage::AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_End()
{
	CommonStateEnd();
}




//AVSI_SI1_AV_CHAIN_FIRE_BALL
void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_FIRE_BALL_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		//���̾���
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );

		// ���� ���� �нú긦 �����ߴٸ� ����Ʈ ���� �߰�
		// ���̾� �� ��ȭ �нú긦 �����ߴٸ� ������ ���� ���� �߰�
		wstring wstrDamageEffectName = L"FIREBALL_CHAIN";
		if ( true == m_bIceBall )
			wstrDamageEffectName = L"FIREBALL_CHAIN_Ice";

		if ( NULL != g_pX2Game->GetDamageEffect() )
		{
			CX2DamageEffect::CEffect * pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( CKTDGXMeshPlayer::CXMeshInstance* pManiEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
			{
				float fMaxLifeTime = pManiEffect->GetMaxLifeTime();
				pManiEffect->SetMaxLifeTime( fMaxLifeTime * m_fFireBallRangeRate);
			}
		}


	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		//���̾���
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );

		// ���� ���� �нú긦 �����ߴٸ� ����Ʈ ���� �߰�
		// ���̾� �� ��ȭ �нú긦 �����ߴٸ� ������ ���� ���� �߰�

		wstring wstrDamageEffectName = L"FIREBALL_CHAIN";
		if ( true == m_bIceBall )
			wstrDamageEffectName = L"FIREBALL_CHAIN_Ice";

		if ( NULL != g_pX2Game->GetDamageEffect() )
		{
			CX2DamageEffect::CEffect * pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( CKTDGXMeshPlayer::CXMeshInstance* pManiEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
			{
				float fMaxLifeTime = pManiEffect->GetMaxLifeTime();
				pManiEffect->SetMaxLifeTime( fMaxLifeTime * m_fFireBallRangeRate);
			}
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		//���̾���
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );

		// ���� ���� �нú긦 �����ߴٸ� ����Ʈ ���� �߰�
		// ���̾� �� ��ȭ �нú긦 �����ߴٸ� ������ ���� ���� �߰�

		wstring wstrDamageEffectName = L"FIREBALL_CHAIN_DOWN_Ice";
		if ( true == m_bIceBall )												// ���� ������ �����
		{
			if ( true == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO1 ) )	// ü�� ���̾� �� �޸� �ִٸ�
			{
				wstrDamageEffectName =  L"FIREBALL_CHAIN_DOWN_Ice_MEMO";
			}
		}
		else																	// ���� ������ ����� �ʾҰ�
		{
			wstrDamageEffectName = L"FIREBALL_CHAIN_DOWN";
			if ( true == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO1 ) )	// ü�� ���̾� �� �޸� �ִٸ�
			{
				wstrDamageEffectName = L"FIREBALL_CHAIN_DOWN_MEMO";
			}
		}

		if ( NULL != g_pX2Game->GetDamageEffect() )
		{
			CX2DamageEffect::CEffect * pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
			{
				float fMaxLifeTime = pMainEffect->GetMaxLifeTime();
				pMainEffect->SetMaxLifeTime( fMaxLifeTime * m_fFireBallRangeRate);
			}
		}

	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_FIRE_BALL_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_FIRE_BALL_EventProcess()
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


//AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER
void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		//���̾���
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );
		// ���� ���� �нú긦 �����ߴٸ� ����Ʈ ���� �߰�
		// ���̾� �� ��ȭ �нú긦 �����ߴٸ� ������ ���� ���� �߰�

		wstring wstrDamageEffectName = L"FIREBALL_CHAIN";
		if ( true == m_bIceBall )
			wstrDamageEffectName = L"FIREBALL_CHAIN_Ice";

		if ( NULL != g_pX2Game->GetDamageEffect() )
		{
			CX2DamageEffect::CEffect * pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
			{
				float fMaxLifeTime = pMainEffect->GetMaxLifeTime();
				pMainEffect->SetMaxLifeTime( fMaxLifeTime * m_fFireBallRangeRate);
			}
		}

	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		//���̾���
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );
		// ���� ���� �нú긦 �����ߴٸ� ����Ʈ ���� �߰�
		// ���̾� �� ��ȭ �нú긦 �����ߴٸ� ������ ���� ���� �߰�

		wstring wstrDamageEffectName = L"FIREBALL_CHAIN";
		if ( true == m_bIceBall )
			wstrDamageEffectName = L"FIREBALL_CHAIN_Ice";

		if ( NULL != g_pX2Game->GetDamageEffect() )
		{
			CX2DamageEffect::CEffect * pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
			{
				float fMaxLifeTime = pMainEffect->GetMaxLifeTime();
				pMainEffect->SetMaxLifeTime( fMaxLifeTime * m_fFireBallRangeRate);
			}
		}
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		//���̾���
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy1_Rhand" );
		// ���� ���� �нú긦 �����ߴٸ� ����Ʈ ���� �߰�
		// ���̾� �� ��ȭ �нú긦 �����ߴٸ� ������ ���� ���� �߰�

		wstring wstrDamageEffectName = L"FIREBALL_CHAIN_DOWN_Ice";
		if ( true == m_bIceBall )												// ���� ������ �����
		{
			if ( true == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO1 ) )	// ü�� ���̾� �� �޸� �ִٸ�
			{
				wstrDamageEffectName =  L"FIREBALL_CHAIN_DOWN_Ice_MEMO";
			}
		}
		else																	// ���� ������ ����� �ʾҰ�
		{
			wstrDamageEffectName = L"FIREBALL_CHAIN_DOWN";
			if ( true == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO1 ) )	// ü�� ���̾� �� �޸� �ִٸ�
			{
				wstrDamageEffectName = L"FIREBALL_CHAIN_DOWN_MEMO";
			}
		}

		if ( NULL != g_pX2Game->GetDamageEffect() )
		{
			CX2DamageEffect::CEffect * pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageEffectName.c_str(), GetPowerRate() * m_fIceBallPower, posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
			if ( CKTDGXMeshPlayer::CXMeshInstance* pMainEffect = ( pEffect != NULL ) ? pEffect->GetMainEffect() : NULL )
			{
				float fMaxLifeTime = pMainEffect->GetMaxLifeTime();
				pMainEffect->SetMaxLifeTime( fMaxLifeTime * m_fFireBallRangeRate);
			}
		}

	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_EventProcess()
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



//AVSI_SI1_AV_HELL_STONE
void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_Init()
{
	XSkinMeshReadyInBackground( L"HellStone_Normal.X" );
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.1f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
        CX2DamageEffect::CEffect*								m_pHellStoneEffect = NULL;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO10 ) == true && GetRandomFloat() <= 0.3f )
		{
			m_pHellStoneEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HELL_STONE_MEMO", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
		else
		{
			m_pHellStoneEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HELL_STONE", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
        m_hHellStoneEffect = ( m_pHellStoneEffect != NULL ) ? m_pHellStoneEffect->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_EventProcess()
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


//AVSI_SI1_AV_HELL_STONE_HYPER
void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.001f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
	{
		D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
        CX2DamageEffect::CEffect*								m_pHellStoneEffect = NULL;
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO10 ) == true && GetRandomFloat() <= 0.3f )
		{
			m_pHellStoneEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HELL_STONE_HYPER_MEMO", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
		else
		{
			m_pHellStoneEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"HELL_STONE_HYPER", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		}
        m_hHellStoneEffect = ( m_pHellStoneEffect != NULL ) ? m_pHellStoneEffect->GetHandle() : INVALID_DAMAGE_EFFECT_HANDLE;
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_HELL_STONE_HYPER_EventProcess()
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


//AVSI_SI2_AV_GUIDE_FIRE_BALL
void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_Init()
{

	TextureReadyInBackground( L"Black_Light.tga" );
	TextureReadyInBackground( L"Black_pulse.dds" );
	TextureReadyInBackground( L"Particle_Blur.dds" );
	XSkinMeshReadyInBackground( L"GuideArrow_AttackBox.X" );
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.3f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) )
	{
		D3DXVECTOR3 posR = GetWeaponBonePos( 0, L"TRACE_START0", 0 );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO3 ) == true )
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUIDE_FIRE_BALL_MEMO", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		else
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUIDE_FIRE_BALL", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_EventProcess()
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


//AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER
void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.3f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) )
	{
		D3DXVECTOR3 posR = GetWeaponBonePos( 0, L"TRACE_START0", 0 );

		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO3 ) == true )
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUIDE_FIRE_BALL_MEMO_HYPER", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
		else
			SetLockOn( g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUIDE_FIRE_BALL_HYPER", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y ), 0, CX2DamageEffect::LOT_NEARST_UID_VECTOR );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_EventProcess()
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


//AVSI_SI2_AV_GUST_SCREW
void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_Init()
{
	XSkinMeshReadyInBackground( L"GustScrew01.X" );
	XSkinMeshReadyInBackground( L"GustScrew02.X" );
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.27f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.466f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUST_SCREW", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.51f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustScrew01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.46f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustScrew02", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.52f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 15.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustScrew02", pos, GetRotateDegree(), GetRotateDegree() );
	}
	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_EventProcess()
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


//AVSI_SI2_AV_GUST_SCREW_HYPER
void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.27f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.466f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUST_SCREW_HYPER", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.51f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustScrew01", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.46f ) )
	{
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustScrew02", GetPos(), GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.52f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		pos.y += 15.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustScrew02", pos, GetRotateDegree(), GetRotateDegree() );
	}
	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_GUST_SCREW_HYPER_EventProcess()
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


//AVSI_SI3_AV_GUST_STORM
void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_Init()
{
	XSkinMeshReadyInBackground( L"GustStorm01.x" );
	XSkinMeshReadyInBackground( L"GustStorm01_1.x" );
	XSkinMeshReadyInBackground( L"GustStorm02.x" );
	XSkinMeshReadyInBackground( L"GustStorm03.x" );
	XSkinMeshReadyInBackground( L"GustStorm04.x" );
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.12f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 pos = GetPos();
        CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
		pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm03", pos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInstance != NULL )    pMeshInstance->SetPerFrameSimulation( false);
		pos.y += 2;
		pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm02", pos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInstance != NULL )    pMeshInstance->SetPerFrameSimulation( false);
		pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm04", pos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInstance != NULL )    pMeshInstance->SetPerFrameSimulation( false);
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUST_STORM_LOW", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUST_STORM_TOWER", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.733f ) )
	{
		D3DXVECTOR3 pos = GetPos();
        CKTDGXMeshPlayer::CXMeshInstance* pMeshInstance = NULL;
		pMeshInstance = g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm01", pos, GetRotateDegree(), GetRotateDegree() );
        if ( pMeshInstance != NULL )    pMeshInstance->SetPerFrameSimulation( false);
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_EventProcess()
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


//AVSI_SI3_AV_GUST_STORM_HYPER
void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_HYPER_FrameMoveFuture()
{
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.12f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm03", pos, GetRotateDegree(), GetRotateDegree() );
		pos.y += 2;
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm02", pos, GetRotateDegree(), GetRotateDegree() );
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm04", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.666f ) )
	{
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUST_STORM_LOW", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"GUST_STORM_TOWER_HYPER", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.733f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		g_pX2Game->GetMajorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"GustStorm01", pos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_HYPER_CameraMove()
{
	CommonCameraMove();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_GUST_STORM_HYPER_EventProcess()
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


// �ö�� Ŀ��
void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_Init()
{
	XSkinMeshReadyInBackground( L"Catastrophe_Laser.X" );
	XSkinMeshReadyInBackground( L"Catastrophe01.X" );
	XSkinMeshReadyInBackground( L"Catastrophe02.X" );
	XSkinMeshReadyInBackground( L"Catastrophe03.X" );
	XSkinMeshReadyInBackground( L"Catastrophe04.X" );
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 2.6f )
	{
		if( m_InputData.pureRight == true )
		{
			//m_FrameDataFuture.syncData.bIsRight = true;
		}
		else if( m_InputData.pureLeft == true )
		{
			//m_FrameDataFuture.syncData.bIsRight = false;
		}
	}

	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.3f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.587f ) )
	{
		wstring wstrEffectName = L"CATASTROPHE_LASER";

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO23) == true )
			wstrEffectName += L"_MEMO";

        m_hCatastropheLaser = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
			this, wstrEffectName.c_str(), GetPowerRate(), GetBonePos(L"Bip01_L_Finger0"), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

    if ( CX2DamageEffect::CEffect* pCatastropheLaser = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hCatastropheLaser ) )
	{
		if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pCatastropheLaser->GetMainEffect() )
		{
			pMeshInst->SetPos( GetBonePos(L"Bip01_L_Finger0") );
			pMeshInst->SetRotateDegree( GetRotateDegree() );
		}
	}


    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 60.0f;
		else
			pos = pos - GetDirVector() * 60.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe01", pos, GetRotateDegree(), GetRotateDegree() );

		pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 55.0f;
		else
			pos = pos - GetDirVector() * 55.0f;

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Catastrophe_P01", pos );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Catastrophe_P02", pos );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Catastrophe_P03", pos );

		pos.y += 2.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe03", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * -10.0f;
		else
			pos = pos - GetDirVector() * -10.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe01", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.566f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * -100.0f;
		else
			pos = pos - GetDirVector() * -100.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe02", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 80.0f;
		else
			pos = pos - GetDirVector() * 80.0f;
		pos.y -= 26.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe02-1", pos, GetRotateDegree(), GetRotateDegree() );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_EventProcess()
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

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_End()
{
	CommonStateEnd();
    m_hCatastropheLaser = INVALID_DAMAGE_EFFECT_HANDLE;
}


//AVSI_SI3_AV_CATASTROPHE_HYPER
void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_HYPER_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 2.6f )
	{
		if( m_InputData.pureRight == true )
		{
			//m_FrameDataFuture.syncData.bIsRight = true;
		}
		else if( m_InputData.pureLeft == true )
		{
			//m_FrameDataFuture.syncData.bIsRight = false;
		}
	}
	CommonFrameMoveFuture();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.3f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.587f ) )
	{
		wstring wstrEffectName = L"CATASTROPHE_LASER_HYPER";

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO23) == true )
			wstrEffectName += L"_MEMO";

        m_hCatastropheLaser = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
			this, wstrEffectName.c_str(), GetPowerRate(), GetBonePos(L"Bip01_L_Finger0"), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");

		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 50.0f;
		else
			pos = pos - GetDirVector() * 50.0f;
		pos.y += 50.0f;

		D3DXVECTOR3 angle = GetRotateDegree();
		angle.x += 70.0f;

		wstring wstrEffectName = L"CATASTROPHE_LASER_HYPER";

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO23) == true )
			wstrEffectName += L"_MEMO";

        m_hCatastropheLaser1 = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
			this, wstrEffectName.c_str(), GetPowerRate(), pos, angle, angle, m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 100.0f;
		else
			pos = pos - GetDirVector() * 100.0f;
		pos.y += 150.0f;

		D3DXVECTOR3 angle = GetRotateDegree();
		angle.x += 180.0f;
		angle.z -= 30.0f;

		wstring wstrEffectName = L"CATASTROPHE_LASER_HYPER";

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO23) == true )
			wstrEffectName += L"_MEMO";

        m_hCatastropheLaser2 = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
			this, wstrEffectName.c_str(), GetPowerRate(), pos, angle, angle, m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
	{
		D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 50.0f;
		else
			pos = pos - GetDirVector() * 50.0f;
		pos.y -= 30.0f;

		D3DXVECTOR3 angle = GetRotateDegree();
		angle.x += -70.0f;

		wstring wstrEffectName = L"CATASTROPHE_LASER_HYPER";

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO23) == true )
			wstrEffectName += L"_MEMO";

        m_hCatastropheLaser3 = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
			this, wstrEffectName.c_str(), GetPowerRate(), pos, angle, angle, m_FrameDataNow.unitCondition.landPosition.y );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.7f ) )
	{
		D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 100.0f;
		else
			pos = pos - GetDirVector() * 100.0f;
		pos.y -= 80.0f;

		D3DXVECTOR3 angle = GetRotateDegree();
		angle.x += 180.0f;
		angle.z += 20.0f;

		wstring wstrEffectName = L"CATASTROPHE_LASER_HYPER";

		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO23) == true )
			wstrEffectName += L"_MEMO";

        m_hCatastropheLaser4 = g_pX2Game->GetDamageEffect()->CreateInstanceHandle(
			this, wstrEffectName.c_str(), GetPowerRate(), pos, angle, angle, m_FrameDataNow.unitCondition.landPosition.y );
	}

	if( m_pXSkinAnimFuture->GetNowAnimationTime() < 2.6f )
	{

        if ( CX2DamageEffect::CEffect* pCatastropheLaser = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hCatastropheLaser ) )
		{
			if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pCatastropheLaser->GetMainEffect() )
			{
				pMeshInst->SetPos( GetBonePos(L"Bip01_L_Finger0") );
				pMeshInst->SetRotateDegree( GetRotateDegree() );
			}
		}

        if ( CX2DamageEffect::CEffect* pCatastropheLaser1 = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hCatastropheLaser1 ) )
		{
			if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pCatastropheLaser1->GetMainEffect() )
			{
				D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
				if( GetIsRight() == true )
					pos = pos + GetDirVector() * 50.0f;
				else
					pos = pos - GetDirVector() * 50.0f;
				pos.y += 50.0f;

				D3DXVECTOR3 angle = GetRotateDegree();
				angle.x += 70.0f;

				pMeshInst->SetPos( pos );
				pMeshInst->SetRotateDegree( angle );
			}
		}


        if ( CX2DamageEffect::CEffect* pCatastropheLaser2 = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hCatastropheLaser2 ) )
		{
			CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pCatastropheLaser2->GetMainEffect();
			if( NULL != pMeshInst )
			{
				D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
				if( GetIsRight() == true )
					pos = pos + GetDirVector() * 100.0f;
				else
					pos = pos - GetDirVector() * 100.0f;
				pos.y += 150.0f;

				D3DXVECTOR3 angle = GetRotateDegree();
				angle.x += 180.0f;
				angle.z -= 30.0f;

				pMeshInst->SetPos( pos );
				pMeshInst->SetRotateDegree( angle );
			}
		}

        if ( CX2DamageEffect::CEffect* pCatastropheLaser3 = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hCatastropheLaser3 ) )
		{
			if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pCatastropheLaser3->GetMainEffect() )
			{
				D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
				if( GetIsRight() == true )
					pos = pos + GetDirVector() * 50.0f;
				else
					pos = pos - GetDirVector() * 50.0f;
				pos.y -= 30.0f;

				D3DXVECTOR3 angle = GetRotateDegree();
				angle.x += -70.0f;

				pMeshInst->SetPos( pos );
				pMeshInst->SetRotateDegree( angle );
			}
		}

        if ( CX2DamageEffect::CEffect* pCatastropheLaser4 = g_pX2Game->GetDamageEffect()->ValidateInstanceHandle( m_hCatastropheLaser4 ) )
		{
			if( CKTDGXMeshPlayer::CXMeshInstance* pMeshInst = pCatastropheLaser4->GetMainEffect() )
			{
				D3DXVECTOR3 pos = GetBonePos(L"Bip01_L_Finger0");
				if( GetIsRight() == true )
					pos = pos + GetDirVector() * 100.0f;
				else
					pos = pos - GetDirVector() * 100.0f;
				pos.y -= 80.0f;

				D3DXVECTOR3 angle = GetRotateDegree();
				angle.x += 180.0f;
				angle.z += 20.0f;

				pMeshInst->SetPos( pos );
				pMeshInst->SetRotateDegree( angle );
			}
		}

	}
	else
	{
	    m_hCatastropheLaser			= INVALID_DAMAGE_EFFECT_HANDLE;
	    m_hCatastropheLaser1		= INVALID_DAMAGE_EFFECT_HANDLE;
	    m_hCatastropheLaser2		= INVALID_DAMAGE_EFFECT_HANDLE;
	    m_hCatastropheLaser3		= INVALID_DAMAGE_EFFECT_HANDLE;
	    m_hCatastropheLaser4		= INVALID_DAMAGE_EFFECT_HANDLE;
	}
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 60.0f;
		else
			pos = pos - GetDirVector() * 60.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe01", pos, GetRotateDegree(), GetRotateDegree() );

		pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 55.0f;
		else
			pos = pos - GetDirVector() * 55.0f;

		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Catastrophe_P01", pos );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Catastrophe_P02", pos );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Catastrophe_P03", pos );

		pos.y += 2.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe03", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * -10.0f;
		else
			pos = pos - GetDirVector() * -10.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe01", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.566f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * -100.0f;
		else
			pos = pos - GetDirVector() * -100.0f;
		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe02", pos, GetRotateDegree(), GetRotateDegree() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		D3DXVECTOR3 pos = GetPos();
		if( GetIsRight() == true )
			pos = pos + GetDirVector() * 80.0f;
		else
			pos = pos - GetDirVector() * 80.0f;
		pos.y -= 26.0f;

		g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"Catastrophe02-1", pos, GetRotateDegree(), GetRotateDegree() );
	}
	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_HYPER_EventProcess()
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

void CX2GUArme_VioletMage::AVSI_SI3_AV_CATASTROPHE_HYPER_End()
{
	CommonStateEnd();
	m_hCatastropheLaser			= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser1		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser2		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser3		= INVALID_DAMAGE_EFFECT_HANDLE;
	m_hCatastropheLaser4		= INVALID_DAMAGE_EFFECT_HANDLE;
}


//AVSI_SI1_AV_CHAIN_BURST
//void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_BURST_Init()
//{
//
//}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_BURST_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.16f, 0 );

	D3DXVECTOR3 pos = GetPos();
	if( GetIsRight() == true )
		pos = pos + GetDirVector() * 50.0f;
	else
		pos = pos - GetDirVector() * 50.0f;
	pos.y += 50.0f;
	D3DXVECTOR3 rotate;

    if( m_pXSkinAnim->EventTimerOneshot( 0.13f ) )
	{
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst01", GetBonePos( L"Dummy1_Rhand" ) );
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.8f ) )
	{
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst01", GetBonePos( L"Dummy1_Rhand" ) );
	}

	float fIncreasePowerRate = 1.f;	/// ���̾� �� ������ ����ġ

	wstring wstrDamageName = L"AISHA_CHAIN_BURST_FIREBALL_SMALL";
	wstring wstrDamageBigName = L"AISHA_CHAIN_BURST_FIREBALL_BIG";

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO9 ) == true )
	{
		wstrDamageName = L"AISHA_CHAIN_BURST_FIREBALL_SMALL_MEMO";
		wstrDamageBigName = L"AISHA_CHAIN_BURST_FIREBALL_BIG_MEMO";

		fIncreasePowerRate = _CONST_AISHA_::AHM_CHAIN_BURST_MEMO_INCREASE_POWER_RATE_VALUE;
	}

	bool bCreatFireBall = false;	/// ���̾ ���� ����

    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst02", GetBonePos( L"Dummy1_Rhand" ) );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst03", GetBonePos( L"Dummy1_Rhand" ) );
		rotate = GetRotateDegree();
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 10.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		rotate = GetRotateDegree();
		rotate.z += 20.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		rotate = GetRotateDegree();
		rotate.z += 10.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		rotate = GetRotateDegree();
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.1f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 20.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		rotate = GetRotateDegree();
		rotate.z += 40.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 40.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 30.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		rotate = GetRotateDegree();
		bCreatFireBall = true;
		wstrDamageName = wstrDamageBigName;		/// ū ���̾ ����
	}

	if( true == bCreatFireBall )
	{
		CX2DamageEffect::CEffect* pDe = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageName.c_str(), GetPowerRate(), pos, rotate,
			rotate, m_FrameDataNow.unitCondition.landPosition.y );

		if( 1.f < fIncreasePowerRate && NULL != pDe )		/// �޸�� ���� ������ �������� ����Ǿ� �ִٸ�, ������ ����
		{
			pDe->SetScale( fIncreasePowerRate, fIncreasePowerRate, fIncreasePowerRate );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_BURST_EventProcess()
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


//AVSI_SI1_AV_CHAIN_BURST_HYPER
void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_BURST_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.16f, 0 );

	D3DXVECTOR3 pos = GetPos();
	if( GetIsRight() == true )
		pos = pos + GetDirVector() * 50.0f;
	else
		pos = pos - GetDirVector() * 50.0f;
	pos.y += 50.0f;

	D3DXVECTOR3 rotate;

    if( m_pXSkinAnim->EventTimerOneshot( 0.13f ) )
	{
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst01", GetBonePos( L"Dummy1_Rhand" ) );
	}
    if( m_pXSkinAnim->EventTimerOneshot( 1.8f ) )
	{
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst01", GetBonePos( L"Dummy1_Rhand" ) );
	}

	float fIncreaseScale = 1.f;		/// ũ�� ����ġ

	wstring wstrDamageName = L"AISHA_CHAIN_BURST_FIREBALL_SMALL";
	wstring wstrDamageBigName = L"AISHA_CHAIN_BURST_FIREBALL_BIG";

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO9 ) == true )
	{
		wstrDamageName = L"AISHA_CHAIN_BURST_FIREBALL_SMALL_MEMO";
		wstrDamageBigName = L"AISHA_CHAIN_BURST_FIREBALL_BIG_MEMO";

		fIncreaseScale = _CONST_AISHA_::AHM_CHAIN_BURST_MEMO_INCREASE_POWER_RATE_VALUE;
	}

	bool bCreatFireBall = false;	/// ���̾ ���� ����

    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst02", GetBonePos( L"Dummy1_Rhand" ) );
		g_pX2Game->GetMinorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_ChainBurst03", GetBonePos( L"Dummy1_Rhand" ) );
		rotate = GetRotateDegree();
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 10.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
	{
		rotate = GetRotateDegree();
		rotate.z += 20.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.9f ) )
	{
		rotate = GetRotateDegree();
		rotate.z += 10.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		rotate = GetRotateDegree();
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.1f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 20.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.2f ) )
	{
		rotate = GetRotateDegree();
		rotate.z += 40.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.3f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 40.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
	{
		rotate = GetRotateDegree();
		rotate.z -= 30.0f;
		bCreatFireBall = true;
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		rotate = GetRotateDegree();
		bCreatFireBall = true;
		wstrDamageName = wstrDamageBigName;		/// ū ���̾ ����
	}

	if( true == bCreatFireBall )
	{
		CX2DamageEffect::CEffect* pDe = g_pX2Game->GetDamageEffect()->CreateInstance( this, wstrDamageName.c_str(), GetPowerRate(), pos, rotate,
										rotate, m_FrameDataNow.unitCondition.landPosition.y );

		if( 1.f < fIncreaseScale && NULL != pDe )		/// �޸�� ���� ������ �������� ����Ǿ� �ִٸ�, ������ ����
		{
			pDe->SetScale( fIncreaseScale, fIncreaseScale, fIncreaseScale );
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI1_AV_CHAIN_BURST_HYPER_EventProcess()
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

void CX2GUArme_VioletMage::SetSelfDestructSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID /*= CX2UnitManager::NUI_NONE*/ )
{
	if( NULL == g_pX2Game )
		return;

	for( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
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
				switch ( eNPCID )
				{
				case CX2UnitManager::NUI_ADW_DISTORTION:
					pNPC->StartSelfDestruction( 0.f );
					break;
				default:
					break;
				}
			}
		}
	}
}

/** @function : COMMON_BUFF_FrameMove
@brief : ���������� ���� ���� ��ų�� FrameMove
*/
/*virtual*/ void CX2GUArme_VioletMage::COMMON_BUFF_FrameMove()
{

    if( m_pXSkinAnim->EventTimerOneshot( 0.63f ) )
	{
		PlayCommonBuffMinorParticle();
		UpDownCrashCamera( 20.0f, 0.3f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) )
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

//AVSI_SI2_AV_DARK_CLOUD
void CX2GUArme_VioletMage::AVSI_SI2_AV_DARK_CLOUD_Init()
{
	TextureReadyInBackground( L"Poison01.dds" );
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_DARK_CLOUD_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.18f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		D3DXVECTOR3 pos = GetBonePos( L"Dummy2_Lhand" );
		pos.y += 30.0f;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DarkCloud_HandLight01", pos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_DarkCloud03", pos );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DarkCloud_PowerBooster01", GetPos() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO4 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DARK_CLOUD_MEMO", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DARK_CLOUD", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_DARK_CLOUD_EventProcess()
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


//AVSI_SI2_AV_DARK_CLOUD_HYPER
void CX2GUArme_VioletMage::AVSI_SI2_AV_DARK_CLOUD_HYPER_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.18f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.6f ) )
	{
		D3DXVECTOR3 pos = GetBonePos( L"Dummy2_Lhand" );
		pos.y += 30.0f;
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DarkCloud_HandLight01", pos );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Light_DarkCloud03", pos );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.65f ) )
	{
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"DarkCloud_PowerBooster01", GetPos() );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.0f ) )
	{
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO4 ) == true )
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DARK_CLOUD_MEMO_HYPER", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
		else
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DARK_CLOUD_HYPER", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
	}


	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SI2_AV_DARK_CLOUD_HYPER_EventProcess()
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

//
////AVSI_SI2_AV_LAND_OF_FLAME
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_Init()
//{
//
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_FrameMoveFuture()
//{
//	CommonFrameMoveFuture();
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_FrameMove()
//{
//	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );
//
//	if( m_pXSkinAnim->EventTimer( 0.27f ) == true && EventCheck( 0.27f, false ) == true )
//	{
//	}
//	else if( m_pXSkinAnim->EventTimer( 0.5f ) == true && EventCheck( 0.5f, false ) == true )
//	{
//		D3DXVECTOR3 pos = GetBonePos( L"Dummy2_Lhand" );
//		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"LandOfFlameShot", m_FrameDataNow.unitCondition.landPosition );
//		if( pSeq != NULL )
//		{
//			pSeq->SetBlackHolePosition( m_FrameDataNow.unitCondition.landPosition );
//		}
//	}
//
//
//	CommonFrameMove();
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_CameraMove()
//{
//	CommonCameraMove();
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_EventProcess()
//{
//	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
//	{
//		if( false == IsOnSomethingFuture() )
//			StateChange( AVSI_JUMP_DOWN );
//		else
//			StateChange( AVSI_WAIT );
//	}
//
//	CommonEventProcess();
//}
//
//
////AVSI_SI2_AV_LAND_OF_FLAME_HYPER
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_HYPER_FrameMoveFuture()
//{
//
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_HYPER_FrameMove()
//{
//
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_HYPER_CameraMove()
//{
//
//}
//
//void CX2GUArme_VioletMage::AVSI_SI2_AV_LAND_OF_FLAME_HYPER_EventProcess()
//{
//
//}



void CX2GUArme_VioletMage::AddStoneWall( UidType uid, float fTimeLeft )
{
	StoneWallData stoneWallData;
	stoneWallData.m_NPCUID = uid;
	stoneWallData.m_fTimeLeft = fTimeLeft;

	m_vecStoneWallData.push_back( stoneWallData );
}


void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_READY_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_READY_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
	{
		D3DXVECTOR3 vBonePos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );
		m_hSeqLight_Arme_Circle_Flame = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Light_Arme_Circle_Flame01", vBonePos );
		//CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqAssualtSlashRing );
	}
	if( m_hSeqLight_Arme_Circle_Flame != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqLight_Arme_Circle_Flame );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			m_hSeqLight_Arme_Circle_Flame = INVALID_PARTICLE_SEQUENCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_READY_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( AVSI_SI_A_AV_CIRCLE_FLAME );
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_READY_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}

void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_Init()
{
	TextureReadyInBackground( L"CenterLight_Gray01.dds" );
	TextureReadyInBackground( L"SpreadLight02_Gray.dds" );
	TextureReadyInBackground( L"SpreadLight02_Gray.dds" );
	XSkinMeshReadyInBackground( L"Arme_SI_A_AV_Circle_Flame_CMesh01.X" );
	XSkinMeshReadyInBackground( L"Arme_SI_A_AV_Circle_Flame_CMesh02.X" );
	XSkinMeshReadyInBackground( L"Arme_SI_A_AV_Circle_Flame_CMesh03.X" );
}
void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_StateStart()
{
	CommonStateStart();
	wstring wstrEffectSetName = L"EffectSet_Aisha_Circle_Flame_Ice";
	if ( true == m_bIceBall )
	{
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO7 ) == true )
		{
			wstrEffectSetName = L"EffectSet_Aisha_Circle_Flame_Ice_Memo";
		}
	}

	else
	{
		wstrEffectSetName = L"EffectSet_Aisha_Circle_Flame";
		if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO7 ) == true )
		{
			wstrEffectSetName = L"EffectSet_Aisha_Circle_Flame_Memo";
		}
	}

	if ( NULL != g_pX2Game->GetEffectSet() )
		g_pX2Game->GetEffectSet()->PlayEffectSet( wstrEffectSetName.c_str(), this );
}

void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_StartFuture()
{
	CommonStateStartFuture();

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_FrameMove()
{
	if( m_hSeqLight_Arme_Circle_Flame != INVALID_PARTICLE_SEQUENCE_HANDLE )
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeq = g_pX2Game->GetMajorParticle()->GetInstanceSequence( m_hSeqLight_Arme_Circle_Flame );
		if( NULL != pSeq )
		{
			D3DXVECTOR3 vBonePos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );
			pSeq->SetPosition( vBonePos );
		}
		else
		{
			m_hSeqLight_Arme_Circle_Flame = INVALID_PARTICLE_SEQUENCE_HANDLE;
		}
	}

	CommonFrameMove();
}
void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_EventProcess()
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
void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_FrameMoveFuture()
{
	if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.66f )
	{
		m_bDisableGravity = false;
	}

	CommonFrameMoveFuture();
}
void CX2GUArme_VioletMage::AVSI_SI_A_AV_CIRCLE_FLAME_EndFuture()
{
	m_bDisableGravity = false;
	CommonStateEndFuture();
}







// ����
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_Init()
	{
		TextureReadyInBackground( L"Ice_Particle01.dds" );
		XSkinMeshReadyInBackground( L"FrozenMissile01.X" );
		XSkinMeshReadyInBackground( L"Arme_SI_SA_BlizzardShower_Spin_Mesh01.X" );
		XMeshReadyInBackground( L"Lightning02.Y" );
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_StateStart()
	{
		CommonStateStart();

		m_bDisableGravity = true;
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_BlizzardShowerNoDamage", this );

			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_BLIZZARD_SHOWER_FROZEN_ATTACK", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );

		}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.93f ) )
		{

			m_bDisableGravity = false;



			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AHM_BLIZZARDSHOWER );
			if( NULL != pSkillTemplet )
			{
				const float MAGIC_MAX_RADIUS = 1000.f; // 10 meter
				const float MAGIC_MIN_RADIUS = 100.f;

				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
				int iPlusFireCount = 0;
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO19) == true )
					iPlusFireCount = 5;

				for( int i=0; i< (pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, iSkillTempletLevel ) + iPlusFireCount); i++ )
				{
					float fRadius = (MAGIC_MAX_RADIUS-MAGIC_MIN_RADIUS) * (float) GetRandomInt( i*4 + 0 ) / 100.f + MAGIC_MIN_RADIUS;
					D3DXVECTOR3 vDirVector = GetDirVector() * (float) ( GetRandomInt( i*4 + 1 ) - 50 ) + D3DXVECTOR3(0, 1, 0 ) * (float) ( GetRandomInt( i*4 + 2 ) - 50 );
					D3DXVec3Normalize( &vDirVector, &vDirVector );

					D3DXVECTOR3 vEffectEndPosition = GetPos() + vDirVector * fRadius;
					D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0, 850, 0 );


					D3DXVECTOR3 vRotation = GetRotateDegree();
					vRotation.z -= 45.f;
					if( true == GetIsRight() )
					{
						vEffectStartPosition -= GetDirVector() * 750;
					}
					else
					{
						vEffectStartPosition += GetDirVector() * 750;
					}

					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_BLIZZARD_SHOWER", GetPowerRate(), vEffectStartPosition, vRotation, vRotation,
						vEffectEndPosition.y, true, GetRandomInt( i*4 + 3 ) / 100.f * 0.8f );
				}
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_EventProcess()
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

	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_StateEnd()
	{
		m_bDisableGravity = false;
		CommonStateEnd();
	}





	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_StateStart()
	{
		CommonStateStart();

		m_bDisableGravity = true;
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_BlizzardShowerNoDamage", this );

			D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_BLIZZARD_SHOWER_FROZEN_ATTACK", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
		}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.93f ) )
		{
			m_bDisableGravity = false;


			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AHM_BLIZZARDSHOWER );
			if( NULL != pSkillTemplet )
			{
				const float MAGIC_MAX_RADIUS = 1000.f; // 10 meter
				const float MAGIC_MIN_RADIUS = 100.f;

				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				int iPlusFireCount = 0;
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO19) == true )
					iPlusFireCount = 5;

				for( int i=0; i< (pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_FIRE_COUNT, iSkillTempletLevel ) + iPlusFireCount); i++ )
				{
					float fRadius = (MAGIC_MAX_RADIUS-MAGIC_MIN_RADIUS) * (float) GetRandomInt( i*4 + 0 ) / 100.f + MAGIC_MIN_RADIUS;
					D3DXVECTOR3 vDirVector = GetDirVector() * (float) ( GetRandomInt( i*4 + 1 ) - 50 ) + D3DXVECTOR3(0, 1, 0 ) * (float) ( GetRandomInt( i*4 + 2 ) - 50 );
					D3DXVec3Normalize( &vDirVector, &vDirVector );

					D3DXVECTOR3 vEffectEndPosition = GetPos() + vDirVector * fRadius;
					D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0, 850, 0 );


					D3DXVECTOR3 vRotation = GetRotateDegree();
					vRotation.z -= 45.f;
					if( true == GetIsRight() )
					{
						vEffectStartPosition -= GetDirVector() * 750;
					}
					else
					{
						vEffectStartPosition += GetDirVector() * 750;
					}

					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_BLIZZARD_SHOWER", GetPowerRate(), vEffectStartPosition, vRotation, vRotation,
						vEffectEndPosition.y, true, GetRandomInt( i*4 + 3 ) / 100.f * 0.8f );
				}
			}
		}


		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_EventProcess()
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



	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_StateEnd()
	{
		m_bDisableGravity = false;
		CommonStateEnd();
	}





	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_Init()
	{
		TextureReadyInBackground( L"Skull_Death_Field01.dds" );

		XSkinMeshReadyInBackground( L"Arme_SI_SA_DeathNote_Mesh01.X" );
		XSkinMeshReadyInBackground( L"Arme_SI_SA_DeathNote_Mesh02.X" );
	}
	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_StateStart()
	{
		CommonStateStart();

		m_bDisableGravity = true;
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

	}
	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
		{
			D3DXVECTOR3 pos = GetPos();

			for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
			{
				CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
				if( pUnit->GetTeam() == GetTeam() )
					continue;

				if( pUnit->GetInvincible() == true )
					continue;

				float MAGIC_RANGE = 160000.f;
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO20) == true )
					MAGIC_RANGE *= 1.3f;

				if( GetDistance3Sq( pos, pUnit->GetPos() ) < MAGIC_RANGE )
				{
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_DeathField_FirstImpact", this, pUnit, false, GetPowerRate() );

					pUnit->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_DEATH_SENTENCE ) );

					//{BFI_DEBUFF_DEATH_SENTENCE�� ���ӽð� ������
					float fDeathSentenceDurationTime = 0.f;

					vector<CX2BuffFinalizerTempletPtr>	vecFinalizerPtr;
					pUnit->GetBuffFinalizerTempletPtrList(BTI_DEBUFF_DEATH_SENTENCE, vecFinalizerPtr);

					if( !vecFinalizerPtr.empty())
					{
						vector<KBuffFinalizerFactor> vecFactors;
						vecFinalizerPtr[0]->GetFactor(vecFactors, this);

						if( !vecFactors.empty() && !vecFactors[0].m_vecValues.empty())
							fDeathSentenceDurationTime = vecFactors[0].m_vecValues[0];
					}
					//}BFI_DEBUFF_DEATH_SENTENCE�� ���ӽð� ������

					CX2EffectSet::Handle hEffectBomb = 	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_DeathFieldBomb", this, pUnit, true, GetPowerRate() );

					if( INVALID_EFFECTSET_HANDLE != hEffectBomb )
					{
						if ( CX2EffectSet::EffectSetInstance* pBomb = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectBomb ) )
							pBomb->m_fDelayTime = fDeathSentenceDurationTime;
					}
				}
			}
		}


    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
		{
			m_bDisableGravity = false;
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_EventProcess()
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

	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_StateEnd()
	{
		m_bDisableGravity = false;
		CommonStateEnd();
	}




	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_HYPER_StateStart()
	{
		CommonStateStart();
		m_bDisableGravity = true;
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

	}


	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_HYPER_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.06f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.8f ) )
		{
			D3DXVECTOR3 pos = GetPos();

			for( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
			{
				CX2GameUnit* pUnit = g_pX2Game->GetUnit( i );
				if( pUnit->GetTeam() == GetTeam() )
					continue;

				if( pUnit->GetInvincible() == true )
					continue;

				float MAGIC_RANGE = 160000.f;
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO20) == true )
					MAGIC_RANGE *= 1.3f;
				if( GetDistance3Sq( pos, pUnit->GetPos() ) < MAGIC_RANGE )
				{
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_DeathField_FirstImpact", this, pUnit, true, GetPowerRate() );

					pUnit->SetBuffFactorToGameUnitByBuffFactorID( static_cast<UINT>( BFI_DEBUFF_DEATH_SENTENCE ) );

					//{BFI_DEBUFF_DEATH_SENTENCE�� ���ӽð� ������
					float fDeathSentenceDurationTime = 0.f;

					vector<CX2BuffFinalizerTempletPtr>	vecFinalizerPtr;
					pUnit->GetBuffFinalizerTempletPtrList(BTI_DEBUFF_DEATH_SENTENCE, vecFinalizerPtr);

					if( !vecFinalizerPtr.empty())
					{
						vector<KBuffFinalizerFactor> vecFactors;
						vecFinalizerPtr[0]->GetFactor(vecFactors, this);

						if( !vecFactors.empty() && !vecFactors[0].m_vecValues.empty())
							fDeathSentenceDurationTime = vecFactors[0].m_vecValues[0];
					}
					//}BFI_DEBUFF_DEATH_SENTENCE�� ���ӽð� ������

					CX2EffectSet::Handle hEffectBomb = 	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_DeathFieldBomb", this, pUnit, true, GetPowerRate() );

					if( INVALID_EFFECTSET_HANDLE != hEffectBomb )
					{
						if ( CX2EffectSet::EffectSetInstance* pBomb = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hEffectBomb ) )
							pBomb->m_fDelayTime = fDeathSentenceDurationTime;
					}
				}
			}
		}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.4f ) )
		{
			m_bDisableGravity = false;
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_HYPER_EventProcess()
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

	void CX2GUArme_VioletMage::AVSI_SA_ADM_DEATHFIELD_HYPER_StateEnd()
	{
		m_bDisableGravity = false;
		CommonStateEnd();
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_Init()
	{
		TextureReadyInBackground( L"AeroTornado04.dds" );
		TextureReadyInBackground( L"Explosion_Sphere.dds" );
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_StateStart()
	{
		CommonStateStart();
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
		{
			float fPowerRate = 1.f;
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_ADM_MANA_INTAKE);
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				fPowerRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_INTAKE_ABS, iSkillTempletLevel );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ARME_MANA_INTAKE", fPowerRate, GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				PlaySound( L"Aisha_Mana_Intake.ogg" );
			}
		}




		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_EventProcess()
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
			if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.68f )
			{
				StateChange( USI_WAIT );
			}
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_HYPER_Init()
	{
		TextureReadyInBackground( L"AeroTornado04.dds" );
		TextureReadyInBackground( L"Explosion_Sphere.dds" );
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_HYPER_StateStart()
	{
		CommonStateStart();
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_HYPER_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
		{
			float fPowerRate = 1.f;

			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet(CX2SkillTree::SI_A_ADM_MANA_INTAKE);
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				fPowerRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_INTAKE_ABS, iSkillTempletLevel );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ARME_MANA_INTAKE", fPowerRate, GetBonePos(L"Bip01"), GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
				PlaySound( L"Aisha_Mana_Intake.ogg" );
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_A_ADM_MANA_INTAKE_HYPER_EventProcess()
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
			if( m_pXSkinAnimFuture->GetNowAnimationTime() > 0.68f )
			{
				StateChange( USI_WAIT );
			}
		}

		CommonEventProcess();
	}



	// SI_SA_AHM_BLAZE_STEP
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLAZE_STEP_Init()
	{
		TextureReadyInBackground( L"Fire_Flower01.dds" );
		TextureReadyInBackground( L"smoke02.dds" );
		TextureReadyInBackground( L"Condense_Pulse02.dds" );
		TextureReadyInBackground( L"SpreadLight02_Gray.dds" );
		TextureReadyInBackground( L"GroundShockwave02.dds" );
	}
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLAZE_STEP_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 1.53f ) )
		{


			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AHM_BLAZE_STEP );
			if ( NULL != pSkillTemplet )
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO18 ) == true )
					SetBuffFactorToGameUnit( pSkillTemplet, 1 );
				else
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );
			}

			if ( NULL != GetUnit() )
			{
				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
				[[maybe_unused]] const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
			}
		}

		CommonFrameMove();
	}
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLAZE_STEP_EventProcess()
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
	// SI_SA_AHM_BLAZE_STEP HYPER
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLAZE_STEP_HYPER_Init()
	{
		TextureReadyInBackground( L"Fire_Flower01.dds" );
		TextureReadyInBackground( L"smoke02.dds" );
		TextureReadyInBackground( L"Condense_Pulse02.dds" );
		TextureReadyInBackground( L"SpreadLight02_Gray.dds" );
		TextureReadyInBackground( L"GroundShockwave02.dds" );
	}
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLAZE_STEP_HYPER_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 1.53f ) )
		{

			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AHM_BLAZE_STEP );
			if ( NULL != pSkillTemplet )
			{
				if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO18 ) == true )
					SetBuffFactorToGameUnit( pSkillTemplet, 1 );
				else
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );
			}

		}

		CommonFrameMove();
	}
	void CX2GUArme_VioletMage::AVSI_SA_AHM_BLAZE_STEP_HYPER_EventProcess()
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

	void CX2GUArme_VioletMage::AVSI_SA_ADM_AGING_Init()
	{
		XSkinMeshReadyInBackground( L"Arme_Aging_UpLight_Mesh02.X" );
		XSkinMeshReadyInBackground( L"Effect_SI_SA_ADM_AGING_BALL01.X" );
		XSkinMeshReadyInBackground( L"Effect_SI_SA_ADM_AGING_BALL.X" );
		XMeshReadyInBackground( L"Arme_Aging_UnderPlane_Mesh01.Y" );
		XMeshReadyInBackground( L"Arme_Aging_UpLight_Mesh01.Y" );
		TextureReadyInBackground( L"Gate_Light02_Gray.dds" );
	}

	void CX2GUArme_VioletMage::AVSI_SA_ADM_AGING_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.06f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 1.366f ) )
		{
			D3DXVECTOR3 vBonePos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );
			m_hSeqLight_Arme_Aging1 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ring_Arme_SI_SA_Aging01", vBonePos );
			m_hSeqLight_Arme_Aging2 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Ring_Arme_SI_SA_Aging02", vBonePos );
		}

    if( m_pXSkinAnim->EventTimerOneshot( 1.37f ) )
		{
			D3DXVECTOR3 vBonePos = GetWeaponBonePos( 0, L"Center_Jewel", 0 );
			m_hSeqLight_Arme_Aging3 = g_pX2Game->GetMajorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"Impact_Arme_SI_SA_Aging01", vBonePos );
		}

    if ( m_pXSkinAnim->EventTimerOneshot( 0.266f ) )
		{
			D3DXVECTOR3 vBonePos = GetBonePos( L"Bip01" );

			if ( GetIsRight() )
				vBonePos.x += 20;
			else
				vBonePos.x -= 20;

			vBonePos.y += 200;

			CX2DamageEffect::CEffect* pDamageEffect
				= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ADM_AGING_THROW", GetPowerRate(),
				vBonePos, GetRotateDegree() , GetRotateDegree() ,m_FrameDataNow.unitCondition.landPosition.y  );

			if ( NULL != pDamageEffect )
			{
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO22) == true )
				{
					CX2DamageEffect::DamageEffectDataInLua& DamageEffectData = pDamageEffect->GetDamageEffectDataInLua();
					if( 0 < DamageEffectData.m_vecDieDamageEffect.size() )
					{
						DamageEffectData.m_vecDieDamageEffect[0]->damageEffectName = L"ADM_AGING_THROW_BALL_BROKEN_MEMO";
					}
				}

				const CX2SkillTree::SkillTemplet* pSkillTemplet
					= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ADM_AGING );

				if ( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
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

	void CX2GUArme_VioletMage::AVSI_SA_ADM_AGING_EventProcess()
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











	void CX2GUArme_VioletMage::AVSI_A_AEM_STONE_WALL_Init()
	{
		XSkinMeshReadyInBackground( L"EM_Stone_Wall_Effect.x" );
		XSkinMeshReadyInBackground( L"EM_Stone_Wall_Piece.X" );
		XSkinMeshReadyInBackground( L"EM_Stone_Wall_Bottom.X" );


		TextureReadyInBackground( L"AEM_Effect01.tga" );
		TextureReadyInBackground( L"AEM_Effect02.tga" );
		TextureReadyInBackground( L"AEM_Stone_Wall_Bottom.tga" );
	}


	void CX2GUArme_VioletMage::AVSI_A_AEM_STONE_WALL_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
		{
			// ���ο� ���� ��ȯ
			if( true == g_pX2Game->IsHost() )
			{
				D3DXVECTOR3 vPos = GetPos();
				if( true == GetIsRight() )
				{
					vPos += GetDirVector() * 200.f;
				}
				else
				{
					vPos -= GetDirVector() * 200.f;
				}

				int iHyperMode = 0; // ���� �ƴ�, 1�̸� ������
				if ( 0.f < GetRemainHyperModeTime() )
					iHyperMode = 1;
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_STONE_WALL_AISHA, iHyperMode, true, vPos,
					GetIsRight(), 0.f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );

				vPos.y += 20.f;
				vPos += GetZVector() * -50.f;
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Stone_Wall_Summon_Smoke", this, NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ), true, vPos, GetRotateDegree(), GetDirVector() );
			}
		}

		CommonFrameMove();
	}



	void CX2GUArme_VioletMage::AVSI_A_AVP_SUMMON_BAT_Init()
	{
		XSkinMeshReadyInBackground( L"AVP_SUMMON_BAT_MESH_EFFECT.X" );
	}


	void CX2GUArme_VioletMage::AVSI_A_AVP_SUMMON_BAT_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
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

			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AVP_SUMMON_BAT_TIME_BOMB );
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				float fLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );

				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_SUMMON_BAT_FLY", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), GetLandPos().y, false, -1.f,
					1.f, fLifeTime );
			}



			vPos.y += 150.f;
			vPos += GetZVector() * -50.f;
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Summon_Smoke", this, NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ), true, vPos, GetRotateDegree(), GetDirVector() );

		}

		CommonFrameMove();
	}




	void CX2GUArme_VioletMage::AVSI_SA_AEM_CYCLONE_Init()
	{
		XSkinMeshReadyInBackground( L"EM_Cyclone_Effect.x" );
		TextureReadyInBackground( L"EM_Cyclone.tga" );
	}

	void CX2GUArme_VioletMage::AVSI_SA_AEM_CYCLONE_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.733f ) )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AHM_CYCLONE );
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				float fLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
				int iHitCount = (int)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_COUNT, iSkillTempletLevel );


				D3DXVECTOR3 vPos = GetPos();
				if( true == GetIsRight() )
				{
					vPos += GetDirVector() * 150.f;
				}
				else
				{
					vPos -= GetDirVector() * 150.f;
				}


				CX2DamageEffect::CEffect* pEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_ELEMENTAL_MASTER_CYCLONE_OUTSIDE", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), GetLandPos().y, false, -1.f,
					1.f, fLifeTime, iHitCount );

				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO16) == true && pEffect != NULL )
				{
					pEffect->GetDamageData().fHitAddMP *= 1.25f;
				}

			}
		}


		CommonFrameMove();
	}


	//void CX2GUArme_VioletMage::AVSI_SA_AEM_CYCLONE_EventProcess()
	//{
	//

	//}



	void CX2GUArme_VioletMage::AVSI_SA_AVP_HELL_DROP_Init()
	{

		XSkinMeshReadyInBackground( L"AVP_Hell_Drop_Body.X" );
		XSkinMeshReadyInBackground( L"AVP_Hell_Drop_Light.x" );
		XSkinMeshReadyInBackground( L"AVP_Hell_Drop_Ring.x" );
		XSkinMeshReadyInBackground( L"AVP_Hell_Drop_Stone.x" );
		XSkinMeshReadyInBackground( L"Arme_SI_SA_Hell_Drop_Broken_Mesh01.x" );

		XMeshReadyInBackground( L"Split_Light01.Y" );

		TextureReadyInBackground( L"VP_Hell_Drop_Light.tga" );
		TextureReadyInBackground( L"VP_Hell_Drop_Ring.tga" );
		TextureReadyInBackground( L"VP_Hell_Drop_Stone.tga" );


	}



	void CX2GUArme_VioletMage::AVSI_SA_AVP_HELL_DROP_Start()
	{
		CommonStateStart();
		m_bIsSummoningUFO = true;
		m_fHellDropCreateEffectCooltime = _CONST_AISHA_::CREATE_HELL_DROP_DAMAGE_EFFECT_START_TIME;		// �� ��� ���� ��Ÿ��, 1�ʺ��� ����
		m_bIsHellDropTimerStart = false;		// �� ��� Ÿ�̸Ӱ� �����°�?
	}





	void CX2GUArme_VioletMage::AVSI_SA_AVP_HELL_DROP_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.45f ) )
		{
			D3DXVECTOR3 vPos = GetPos();
			if( true == GetIsRight() )
			{
				vPos += GetDirVector() * 200.f;
			}
			else
			{
				vPos -= GetDirVector() * 200.f;
			}
			vPos.y += 240.f;

			ResetUFO();

			if( true == g_pX2Game->IsHost() )
			{
				int iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;

				D3DXVECTOR3 vLandPos = vPos;
				if( false == g_pX2Game->GetWorld()->GetLineMap()->CanDown( vPos, LINE_RADIUS, &vLandPos, &iNewLineIndex ) )
				{
					vPos = GetPos();
					vPos.y += 240.f;
					iNewLineIndex = m_FrameDataNow.syncData.lastTouchLineIndex;
				}

				int iHyperMode = 0; // ���� �ƴ�, 1�̸� ������
				if ( 0.f < GetRemainHyperModeTime() )
					iHyperMode = 1;
				if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO21) == true )
					g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_UFO_AISHA_DROPPER_MEMO, iHyperMode, true, vPos,
					GetIsRight(), 0.3f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
				else
				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_UFO_AISHA, iHyperMode, true, vPos,
					GetIsRight(), 0.3f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}


			vPos += GetZVector() * -50.f;
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Summon_Smoke", this, NULL, false, -1.f, -1.f, D3DXVECTOR3( 1, 1, 1 ), true, vPos, GetRotateDegree(), GetDirVector() );

		}


		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_HELL_DROP_EventProcess()
	{
		if( true == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) )
		{
			if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
			{
				m_TimerUFOControl.restart();
				if( false == IsOnSomethingFuture() )
					StateChange( USI_JUMP_DOWN );
				else
					StateChange( USI_WAIT );
			}
		}
		else
		{
			const float MAGIC_TIME = 3.f;		// ���� �ð� ���� 5�� -> 3��

			if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
			{
				if( NULL != GetUFO() )
				{
					StateChange( AVSI_HELL_DROP_CONTROL_IDLE );
				}
				else if( m_FrameDataFuture.unitCondition.fStateTime > MAGIC_TIME ) // time out �Ǹ� wait ���·� �ϴ� ���ư���
				{
					StateChange( USI_WAIT );
				}
			}


			m_InputData.Init(); // ĳ���ʹ� ���� �� �� ���� UFO�� ������ �� �ִ�
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_HELL_DROP_StateEnd()
	{
		m_bIsSummoningUFO = false;
		CommonStateEnd();
		m_TimerUFOControl.restart();			// �� ��� Ÿ�̸Ӹ� �����Ѵ�.
		m_bIsHellDropTimerStart = true;			// �� ��� Ÿ�̸Ӱ� �����°�?
	}


	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_FrameMove()
	{
		if( false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) )
		{
			// ��ų Ű�� ���� ���¿���, 1�ʰ� ������ ( ���� �� ��Ÿ�� ), ���� �� �� 0.25�ʰ� �����ٸ�
			// �� ����� �߻��Ѵ�.
			if ( m_TimerUFOControl.elapsed() > m_fHellDropCreateEffectCooltime )
			{
				m_fHellDropCreateEffectCooltime = m_fHellDropCreateEffectCooltime + _CONST_AISHA_::CREATE_HELL_DROP_DAMAGE_EFFECT_TIME_GAP;
				// StateChange( AVSI_HELL_DROP_CONTROL_FIRE );
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Hell_Drop_Stone", GetUFO(), NULL, IsHyperState(), GetPowerRate() );
			}
		}
		CommonFrameMove();
	}


	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_EventProcess()
	{
		if( false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) )
		{
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
						TRACE( L"HIT C\n" );
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


			const float MAGIC_CONTROL_RADIUS = 1500.f;
			const double MAGIC_CONTROL_TIME = 2.9;	// ���� �ð� ���� 5�� -> 3.1��

			{
				if( ( true == bSpecialAttackKeyPressed &&
					m_TimerUFOControl.elapsed() < MAGIC_CONTROL_TIME )
					|| m_TimerUFOControl.elapsed() < _CONST_AISHA_::CREATE_HELL_DROP_DAMAGE_EFFECT_START_TIME + 0.1f
					)
				{
					D3DXVECTOR3 vDirection(0,0,0);
					if( NULL != GetUFO() )
					{
						if( GetDistance( GetUFO()->GetPos(), GetPos() ) > MAGIC_CONTROL_RADIUS )
						{
							vDirection = GetUFO()->GetPos() - GetPos();
						}
					}
					if( vDirection.x >= 0.f && vDirection.y <= 0.f &&
						true == m_InputData.pureUp && true == m_InputData.pureLeft )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_UP_LEFT )
							StateChange( AVSI_HELL_DROP_CONTROL_UP_LEFT );
					}
					else if( vDirection.x <= 0.f && vDirection.y <= 0.f &&
						true == m_InputData.pureUp && true == m_InputData.pureRight )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_UP_RIGHT )
							StateChange( AVSI_HELL_DROP_CONTROL_UP_RIGHT );
					}
					else if( vDirection.x >= 0.f && vDirection.y >= 0.f &&
						true == m_InputData.pureDown && true == m_InputData.pureLeft )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_DOWN_LEFT )
							StateChange( AVSI_HELL_DROP_CONTROL_DOWN_LEFT );
					}
					else if( vDirection.x <= 0.f && vDirection.y >= 0.f &&
						true == m_InputData.pureDown && true == m_InputData.pureRight )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_DOWN_RIGHT )
							StateChange( AVSI_HELL_DROP_CONTROL_DOWN_RIGHT );
					}
					else if( vDirection.x >= 0.f &&
						true == m_InputData.pureLeft )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_LEFT )
							StateChange( AVSI_HELL_DROP_CONTROL_LEFT );
					}
					else if( vDirection.x <= 0.f &&
						true == m_InputData.pureRight )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_RIGHT )
							StateChange( AVSI_HELL_DROP_CONTROL_RIGHT );
					}
					else if( vDirection.y <= 0.f &&
						true == m_InputData.pureUp )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_UP )
							StateChange( AVSI_HELL_DROP_CONTROL_UP );
					}
					else if( vDirection.y >= 0.f &&
						true == m_InputData.pureDown )
					{
						if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_DOWN )
							StateChange( AVSI_HELL_DROP_CONTROL_DOWN );
					}
					else if( GetNowStateID() != AVSI_HELL_DROP_CONTROL_IDLE )
					{
						StateChange( AVSI_HELL_DROP_CONTROL_IDLE );
					}
				}
				else
				{
					// ���� Fire ���� ��� -> ���� �ð� Ȥ�� ���ǿ� ���� ������� ����
					// ����, �� ��� Ÿ�̸Ӱ� �����ִ°�?
					if ( true == m_bIsHellDropTimerStart )
					{
						TRACE( L"AVSI_HELL_DROP_CONTROL_DYING\n" );
						StateChange( AVSI_HELL_DROP_CONTROL_DYING, true );
					}
				}
			}
			m_InputData.Init(); // ĳ���ʹ� ���� �� �� ���� UFO�� ������ �� �ִ�
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_IDLE_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_IDLE.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_LEFT_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_LEFT.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_RIGHT_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_RIGHT.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_UP_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_UP.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_DOWN_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_DOWN.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_UP_LEFT_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_UP_LEFT.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_UP_RIGHT_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_UP_RIGHT.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_DOWN_LEFT_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_DOWN_LEFT.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_DOWN_RIGHT_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			GetUFO()->StateChange( _CONST_AISHA_::UFO_GO_DOWN_RIGHT.c_str(), true );
		}
	}

	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_FIRE_StateStart()
	{
		CommonStateStart();
		if(
			false == GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO21 ) &&
			NULL != GetUFO() )
		{
			TRACE( L"Fire StateStart\n" );
			GetUFO()->StateChange( _CONST_AISHA_::UFO_FIRE.c_str(), true );
		}
	}


	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_FIRE_FrameMove()
	{
// 		if( m_pXSkinAnim->EventTimer( 0.01f ) == true && EventCheck( 0.01f, false ) == true )
// 		{
// 			if( NULL != GetUFO() )
// 			{
// 				TRACE( L"Fire StateStart\n" );
// 				GetUFO()->StateChange( _CONST_AISHA_::UFO_FIRE.c_str(), true );
// 				ResetUFO();
// 			}
// 		}

		CommonFrameMove();
	}
	void CX2GUArme_VioletMage::AVSI_HELL_DROP_CONTROL_FIRE_EventProcess()
	{
		StateChange ( AVSI_HELL_DROP_CONTROL_IDLE );
		CommonEventProcess();
	}
	//void CX2GUArme_VioletMage::AVSI_AEM_ZZupZ_EventProcess()
	//{
	//	CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );


	//	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	//	{
	//		if( false == IsOnSomethingFuture() )
	//		{
	//			StateChange( AVSI_JUMP_DOWN );
	//			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	//		}
	//		else
	//			StateChange( AVSI_WAIT );
	//	}
	//	ELSE_IF_STATE_CHANGE_ON_( 0, 0.4f, 0.4f, m_InputData.oneZ == true, AVSI_COMBO_Z4up )
	//	ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 0.7f )
	//	{
	//		if( false == IsOnSomethingFuture() )
	//			StateChange( AVSI_JUMP_DOWN );
	//		else
	//			StateChange( AVSI_WAIT );
	//	}

	//	CommonEventProcess();
	//}


	void CX2GUArme_VioletMage::AVSI_AEM_ZZupZZ_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
		{
			if( FlushMp( 10.f ) == true )
			{
				CX2EffectSet::Handle hEffSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_EM_ZZupZZ", this );

				m_vecEffectSetToDeleteOnDamageReact.push_back( hEffSet );
			}
			else
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

				CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
			}
		}


		CommonFrameMove();
	}




	void CX2GUArme_VioletMage::AVSI_AEM_ZZupZZ_EventProcess()
	{

		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			if( false == IsOnSomethingFuture() )
				StateChange( USI_JUMP_DOWN );
			else
				StateChange( USI_WAIT );
		}
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 1.4f )
		{
			if( false == IsOnSomethingFuture() )
				StateChange( USI_JUMP_DOWN );
			else
				StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}





	void CX2GUArme_VioletMage::AVSI_AEM_XXfrontX_Init()
	{
		XSkinMeshReadyInBackground( L"Arme_EM_Combo_X2F_Mesh01.X" );
		TextureReadyInBackground( L"Arme_Ice_Spiner01.tga" );


		XSkinMeshReadyInBackground( L"Arme_EM_Combo_Z4UP_Mesh01.X" );

	}





	void CX2GUArme_VioletMage::AVSI_AEM_XXfrontX_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
		{
			if( FlushMp( 10.f ) == true )
			{
				CX2EffectSet::Handle hEffSet = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_EM_XXfrontX", this );

				m_vecEffectSetToDeleteOnDamageReact.push_back( hEffSet );

			}
			else
			{
				D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
				if( true == GetIsRight() )
				{
					pos += GetDirVector() * 150.f;
				}
				else
				{
					pos -= GetDirVector() * 150.f;
				}

				CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
			}
		}


		CommonFrameMove();
	}


	void CX2GUArme_VioletMage::AVSI_AEM_XXfrontX_EventProcess()
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
		else if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 1.33f )
		{
			if( false == IsOnSomethingFuture() )
				StateChange( USI_JUMP_DOWN );
			else
				StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}



	void CX2GUArme_VioletMage::AVSI_AEM_XZ_Init()
	{
		XSkinMeshReadyInBackground( L"Arme_EM_Combo_XZZ_Mesh01.X" );
		XSkinMeshReadyInBackground( L"Wind_Liner03.X" );

		TextureReadyInBackground( L"Arme_Lightning01.tga" );
	}


	void CX2GUArme_VioletMage::AVSI_AEM_XZ_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
		{
			if( FlushMp( 3.f ) == true )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Elemental_Master_XZ", this );
			}
			else
			{
				D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
				//D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( "Bip01" );
				if( true == GetIsRight() )
				{
					pos += GetDirVector() * 120.f;
				}
				else
				{
					pos -= GetDirVector() * 120.f;
				}
				pos.y += 40.f;
				CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
			}
		}

		CommonFrameMove();
	}




	void CX2GUArme_VioletMage::AVSI_AEM_XZ_EventProcess()
	{
		CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );


		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
		else if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		ELSE_IF_STATE_CHANGE_ON_( 0, 0.89f, 0.89f, m_InputData.oneZ == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.54f, AVSI_AEM_XZZ )

		CommonEventProcess();
	}


	void CX2GUArme_VioletMage::AVSI_AEM_XZZ_FrameMove()
	{
		/// 3Ÿ ����
		if( m_pXSkinAnim->EventTimerOneshot( 0.333f ) == true ||
			m_pXSkinAnim->EventTimerOneshot( 1.133f ) == true ||
			m_pXSkinAnim->EventTimerOneshot( 1.933f ) == true )

		{
			if( FlushMp( 3.f ) == true )	/// ���� �Һ� ����
			{
				/// ������ ������ ����Ʈ�� �߻��ؼ�, ������ ������ ���� �����Ǵ� ������ ���� ( ���� ������ Die Damage Effect�� �߻� )

				/// ������ EffectSet���� �����Ǿ� �ִ����� Damage Effect�� ���� ( Die Damage Effect�� ����ϱ� ���ؼ� )
				g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ELEMENTAL_MASTER_XXZ_INVISIBLE_ATTACKBOX", GetPowerRate(), GetPos(), GetRotateDegree(), GetRotateDegree(),  GetLandPos().y );
			}
			else
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

				CreateNotEnoughMPEffect( pos, 0.f, 0.f, 0.f );
			}
		}

		CommonFrameMove();
	}




	void CX2GUArme_VioletMage::AVSI_AEM_XZZ_EventProcess()
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( AVSI_AEM_XZZ_END );
		}
		else if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
	}

	/// XZZ �޺� ���� ������Ʈ�߰�
	void CX2GUArme_VioletMage::AVSI_AEM_XZZ_END_EventProcess()
	{
		if( false == IsOnSomethingFuture() )
			StateChange( USI_JUMP_LANDING );
		else
			StateChange( USI_WAIT );

		CommonEventProcess();
	}






	void CX2GUArme_VioletMage::AVSI_AVP_ZZfrontZ_EventProcess()
	{
		CANCEL_COMBO_ON_DIRECTION_CHANGE( 1 );


		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}
		else if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
		}
		ELSE_IF_STATE_CHANGE_ON_( 0, 0.8f, 0.8f, m_InputData.oneZ == true, AVSI_AVP_ZZfrontZZ )
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 0.94f )
		{
			if( false == IsOnSomethingFuture() )
				StateChange( USI_JUMP_DOWN );
			else
				StateChange( USI_WAIT );
		}

		CommonEventProcess();

	}


	void CX2GUArme_VioletMage::AVSI_AVP_ZZfrontZZ_Init()
	{

		XSkinMeshReadyInBackground( L"VP_ComboZ3_Effect.X" );
		XSkinMeshReadyInBackground( L"VP_ComboZ4_Effect.X" );


		TextureReadyInBackground( L"AVP_Effect128.tga" );
		TextureReadyInBackground( L"AVP_Phantom_Blade_Arc.tga" );
		TextureReadyInBackground( L"AVP_Phantom_Blade_Weapon.tga" );

	}



	void CX2GUArme_VioletMage::AVSI_AVP_ZZfrontZZ_StateStartFuture()
	{
		CommonStateStartFuture();

		m_FrameDataFuture.syncData.position.y += LINE_RADIUS * 1.5f;
	}



	void CX2GUArme_VioletMage::AVSI_AVP_ZZfrontZZ_EventProcess()
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			if( false == IsOnSomethingFuture() )
				StateChange( AVSI_AVP_ZZfrontZZ_FALL );
			else
				StateChange( USI_WAIT );
		}
		ELSE_IF_ARROW_ASDC_PRESSED_AFTER( 1.65f )
		{
			if( false == IsOnSomethingFuture() )
				StateChange( AVSI_AVP_ZZfrontZZ_FALL );
			else
				StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}




	void CX2GUArme_VioletMage::AVSI_AVP_ZZfrontZZ_FALL_EventProcess()
	{
		if( true == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_LANDING );
		}

		CommonEventProcess();
	}


	void CX2GUArme_VioletMage::AVSI_AVP_DASH_JUMP_X_Init()
	{
		TextureReadyInBackground( L"Black_FireBall01.dds" );
	}


	void CX2GUArme_VioletMage::AVSI_AVP_DASH_JUMP_X_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.16f ) )
		{
			//�ĺ����
		if( FlushMp( 4.0f * m_fMPConsumeRateFireBall ) == true )
			{
				if( GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				{
					D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
					if( IsTransformed() )
					{
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ARME_ACTIVE_MAGIC_BALL2", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					}
					else
					{
						g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ARME_ACTIVE_MAGIC_BALL1", GetPowerRate(), posR, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
					}
				}
				else
				{
					//���̾���
					D3DXVECTOR3 posR = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
					D3DXVECTOR3 angle = GetRotateDegree();
					angle.z = -30.0f;
					g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_GRAVITY_BALL", m_fGravityBallPower,
						posR, angle, angle, m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, 1.f, m_fFireBallRangeRate  );
				}

			}
			else
			{
				D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Dummy2_Lhand" );
				CreateNotEnoughMPEffect( pos, 0.f, 0.f, -30.f );

			}
		}

		CommonFrameMove();
	}



	void CX2GUArme_VioletMage::AVSI_AVP_DASH_JUMP_X_EventProcess()
	{
		if( true == IsOnSomethingFuture() )
		{
			StateChange( USI_DASH_JUMP_LANDING );
		}

		CommonEventProcess();
	}


	//{{ ����� 2010.11.1 : ���׿� ����

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_Init()
	{
		XSkinMeshReadyInBackground( L"MeteoStone01.X" );
		XSkinMeshReadyInBackground( L"MeteoStone02.X" );
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_StateStart()
	{
		CommonStateStart();

		m_bIsOnSomethingStart =false;

//#ifdef STEP_ON_MONSTER_TEST
//		if (m_FrameDataNow.unitCondition.bFootOnLine == true ||
//			m_FrameDataNow.unitCondition.bFootOnUnit == true )
//#else STEP_ON_MONSTER_TEST
		if ( m_FrameDataNow.unitCondition.bFootOnLine == true )
//#endif STEP_ON_MONSTER_TEST
			m_bIsOnSomethingStart = true;

		m_bDisableGravity = true;
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 500 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

		m_iElapsedFireCount			= 0;		/// ���׿� ���� ���� ��ȯ ���� �ʱ�ȭ
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_METEOR_SHOWER_MOTION_EFFECT", this );
		}

		if( m_iElapsedFireCount >= m_iMaxFireCount )	/// �ִ� �߻� Ƚ���� �Ѱ�ٸ�, �н�
			return;

		const int fCheckTime = static_cast<int>( m_FrameDataNow.unitCondition.fStateTime / m_fFireGap );

		if( fCheckTime >= m_iElapsedFireCount )
		{
			++m_iElapsedFireCount;	/// �߻� ī��Ʈ ����

			/// ��ü ���� : -850 ~ 850, 1�׷� : -850 ~ 340, 2�׷� : -340  ~ 850		-> 1, 2 �׷��� �ݺ��ϸ� �߾��� ��ġ�� ���� ���� ����
			float fRadius = static_cast<float>( ( GetRandomInt( m_iElapsedFireCount * 4 + 1 ) - 50 ) * 12 );

			if( 0 == m_iElapsedFireCount % 2 )
				fRadius += 250.f;
			else
				fRadius -= 250.f;

			if( true == GetIsRight() )		/// ���⿡ ���� ��ġ ����
				fRadius += 500.f;
			else
				fRadius -= 500.f;

			D3DXVECTOR3 vDirVector = GetDirVector();

			D3DXVECTOR3 vEffectEndPosition = GetPos() + vDirVector * fRadius;
			D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0.f , 850.f, 0.f );

			D3DXVECTOR3 vRotation = GetRotateDegree();

			vRotation.z -= 45.f;
			if( true == GetIsRight() )
			{
				vEffectStartPosition -= GetDirVector() * 900;
			}
			else
			{
				vEffectStartPosition += GetDirVector() * 900;
			}

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_METEO_SHOWER01", GetPowerRate(), vEffectStartPosition, vRotation+D3DXVECTOR3(0,0,45), vRotation,
				vEffectEndPosition.y );
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_METEO_SHOWERO2", GetPowerRate(), vEffectStartPosition, vRotation+D3DXVECTOR3(0,0,45), vRotation,
				vEffectEndPosition.y );
		}

		CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
		CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

		if( pSeqArmeAirStepRight != NULL )
		{
			pSeqArmeAirStepRight->SetEmitRate(10,20);
		}

		if( pSeqArmeAirStepLeft != NULL )
		{
			pSeqArmeAirStepLeft->SetEmitRate(10,20);
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_EventProcess()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
		{
			m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		}
		else if ( m_iElapsedFireCount >= m_iMaxFireCount )
		{
			StateChange( AVSI_SA_AHM_METEOSHOWER_LOOP );
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_StateEnd()
	{
		m_bDisableGravity = false;
		CommonStateEnd();
	}


	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_LOOP_StateStart()
	{
		CommonStateStart();

		SetNoDetonation( true );			/// ���� ���� ����

		m_iElapsedFireCount			= 0;	///	�߻� ī��Ʈ �ʱ�ȭ
		m_bDisableGravity			= true;	/// �߷� ����
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_LOOP_FrameMove ()
	{
		ChangeWorldColorByHyperMode();

		if ( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.0f )
		{
			m_ExtraDamagePack.m_EnchantShock.m_fTime = 0;
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}


		if( m_iElapsedFireCount >= m_iMaxFireCountByLoopState )	/// �ִ� �߻� Ƚ���� �Ѱ�ٸ�, �н�
			return;

		const int fCheckTime = static_cast<int>( m_FrameDataNow.unitCondition.fStateTime / m_fFireGap );

		if( fCheckTime >= m_iElapsedFireCount )
		{
			++m_iElapsedFireCount;				/// ���� ī��Ʈ ����

			if( false == FlushMp( m_fUseMPConsume ) )
				return;

			/// ��ü ���� : -850 ~ 850, 1�׷� : -850 ~ 340, 2�׷� : -340  ~ 850		-> 1, 2 �׷��� �ݺ��ϸ� �߾��� ��ġ�� ���� ���� ����
			float fRadius = static_cast<float>( ( GetRandomInt( m_iElapsedFireCount * 4 + 1 ) - 50 ) * 12 );

			if( 0 == m_iElapsedFireCount % 2 )
				fRadius += 250.f;
			else
				fRadius -= 250.f;

			if( true == GetIsRight() )		/// ���⿡ ���� ��ġ ����
				fRadius += 500.f;
			else
				fRadius -= 500.f;

			D3DXVECTOR3 vDirVector = GetDirVector();

			D3DXVECTOR3 vEffectEndPosition = GetPos() + vDirVector * fRadius;
			D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0.f , 850.f, 0.f );


			D3DXVECTOR3 vRotation = GetRotateDegree();

			vRotation.z -= 45.f;
			if( true == GetIsRight() )
			{
				vEffectStartPosition -= GetDirVector() * 900;
			}
			else
			{
				vEffectStartPosition += GetDirVector() * 900;
			}

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_METEO_SHOWER01", GetPowerRate(), vEffectStartPosition, vRotation+D3DXVECTOR3(0,0,45), vRotation,
				vEffectEndPosition.y );
			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"AISHA_METEO_SHOWERO2", GetPowerRate(), vEffectStartPosition, vRotation+D3DXVECTOR3(0,0,45), vRotation,
				vEffectEndPosition.y );
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_LOOP_EventProcess()
	{
		if ( m_fUseMPConsume > GetNowMp() ||						/// ���� MP�� ���׿� ��ȯ ��뺸�� ���ų�
			 m_iElapsedFireCount >= m_iMaxFireCountByLoopState ||	/// ���׿� ������ ��ȯ�ϴµ� ����� �� ���� MP �Ҹ���� �Ѱ�ġ�� �Ѱų�
			 0.f >= GetNowHp() ||									/// �ǰ� ���ų�
			 ( m_InputData.pureA != true && m_InputData.pureS != true && m_InputData.pureD != true && m_InputData.pureC != true &&	/// ��ų Ű�� ������ ���� �ʴٸ� ����
			   m_InputData.pureQ != true && m_InputData.pureW != true && m_InputData.pureE != true && m_InputData.pureR != true ) )
		{
			StateChange( AVSI_SA_AHM_METEOSHOWER_END );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_LOOP_StateEnd()
	{
		m_bDisableGravity = false;		/// �߷� ����
		m_iElapsedFireCount = 0;		/// �߻� ī��Ʈ �ʱ�ȭ

		CommonStateEnd();
		SetNoDetonation( false );		/// ���� ���� ����
	}

	/// ���׿� ���� ���� ������Ʈ�߰�
	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_END_EventProcess()
	{
		if( true == IsOnSomethingFuture() )
			StateChange( USI_JUMP_LANDING );

		CommonEventProcess();
	}
	void CX2GUArme_VioletMage::AVSI_SA_AHM_METEOSHOWER_END_StateEnd()
	{
		CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
		CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

		if( pSeqArmeAirStepRight != NULL )
		{
			pSeqArmeAirStepRight->SetEmitRate(0,0);
		}

		if( pSeqArmeAirStepLeft != NULL )
		{
			pSeqArmeAirStepLeft->SetEmitRate(0,0);
		}
	}

	//}} ����� 2010.11.1 : ���׿� ����


//{{ ����� 2010.11.1 : ���Һ긮��

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_Init()
	{
		XSkinMeshReadyInBackground( L"AVP_PHANTOM_BREATHING_Body.X" );
		XSkinMeshReadyInBackground( L"AVP_PHANTOM_BREATHING_Attack.X" );
	}


	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_StateStart()
	{
		CommonStateStart();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_FrameMove()
	{
		ChangeWorldColorByHyperMode();
		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 0 );

    if (m_pXSkinAnim->EventTimerOneshot( 0.25f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_PHANTOM_BREATHING", this );
		}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.815f ) )
		{
			D3DXVECTOR3 degree = GetRotateDegree();
			D3DXVECTOR3 pos = GetPos();
			D3DXVECTOR3 Dirvec = GetDirVector();

			Dirvec.y = 0;
			D3DXVec3Normalize( &Dirvec , &Dirvec );

			if ( GetIsRight() == true )
				pos += Dirvec*310.0f;
			else
			{
				degree.y += 180.0f;
				pos -= Dirvec*310.0f;
			}

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"PHANTOM_BREATHING_NO_DAMAGE", GetPowerRate(), pos, degree , degree ,m_FrameDataNow.unitCondition.landPosition.y );

			CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"PHANTOM_BREATHING", GetPowerRate(), pos, degree , degree ,m_FrameDataNow.unitCondition.landPosition.y );

			if( NULL != pDE )		/// ��ų ������ ���� ���� ���� ����
			{
				CX2DamageManager::DamageData& damageData = pDE->GetDamageData();

				//if( NULL != pDamageData )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AVP_PHANTOM_BREATHING_DARK_FALL );
					//pDE->GetDamageData().m_ExtraDamage.m_DamagePerSec = -1 * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TARGET_MANA );

					if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
					{
						if ( NULL == GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

						damageData.PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
					}
				}
			}

			pos = GetBonePos( L"Bip01_L_Finger11" );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Pa_AVP_PHANTOM_BREATHING_Finger", pos );
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_EventProcess()
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_StateEnd()
	{
		CommonStateEnd();
	}



// ���Һ긮�� hyper mode
	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_Init()
	{
		XSkinMeshReadyInBackground( L"AVP_PHANTOM_BREATHING_Body.X" );
		XSkinMeshReadyInBackground( L"AVP_PHANTOM_BREATHING_Attack.X" );
	}


	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_StateStart()
	{
		CommonStateStart();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 0 );

    if (m_pXSkinAnim->EventTimerOneshot( 0.25f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_PHANTOM_BREATHING", this );
		}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.815f ) )
		{
			D3DXVECTOR3 degree = GetRotateDegree();
			D3DXVECTOR3 pos = GetPos();
			D3DXVECTOR3 Dirvec = GetDirVector();

			Dirvec.y = 0;
			D3DXVec3Normalize( &Dirvec , &Dirvec );

			if ( GetIsRight() == true )
				pos += Dirvec*310.0f;
			else
			{
				degree.y += 180.0f;
				pos -= Dirvec*310.0f;
			}

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"PHANTOM_BREATHING_NO_DAMAGE", GetPowerRate(), pos, degree , degree ,m_FrameDataNow.unitCondition.landPosition.y );

			CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"PHANTOM_BREATHING", GetPowerRate(), pos, degree , degree ,m_FrameDataNow.unitCondition.landPosition.y );

			if( NULL != pDE )		/// ��ų ������ ���� ���� ���� ����
			{
				CX2DamageManager::DamageData& damageData = pDE->GetDamageData();

				//if( NULL != pDamageData )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_AVP_PHANTOM_BREATHING_DARK_FALL );
					//float ff = pDE->GetDamageData().m_ExtraDamage.m_DamagePerSec = -1 * pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_TARGET_MANA );

					if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
					{
						if ( NULL == GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

						damageData.PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
					}
				}
			}

			pos = GetBonePos( L"Bip01_L_Finger11" );
			g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Pa_AVP_PHANTOM_BREATHING_Finger", pos );
		}


		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_EventProcess()
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_StateEnd()
	{
		CommonStateEnd();
	}

//}} ����� 2010.11.1 : ���Һ긮��



	HRESULT CX2GUArme_VioletMage::OnFrameMove( double fTime, float fElapsedTime )
	{
		CX2GUUser::OnFrameMove( fTime, fElapsedTime );

		if( m_bNeedToUpdateWeaponEffect == true )
		{
			UpdateWeaponEnchantEffectAndExtraDamageType();
			m_bNeedToUpdateWeaponEffect = false;
		}

		return S_OK;
	}

	void CX2GUArme_VioletMage::NotifyDGManagerChanged( CKTDGManager& refInManager_ )		// DGManager�� ���
	{
		CX2GUUser::NotifyDGManagerChanged( refInManager_ );

// 		if ( IsRecursivelyManagedByDGManager() )
// 		{
// 			if ( m_TransformWeapon != NULL )
// 				refInManager_.AddObjectChain( m_TransformWeapon.get() );
// 		}
// 		else
// 		{
// 			if( m_TransformWeapon != NULL )
// 				refInManager_.RemoveObjectChain( m_TransformWeapon.get() );
// 		}+
	}

	void CX2GUArme_VioletMage::TransformStart( const WCHAR* pWstrMeshName_, const bool bChangeWeapon_ /*= false */ )
	{
		if ( NULL == m_pOrgMotion && NULL != m_pMotion )
			m_pOrgMotion = m_pMotion;


		if ( m_pMotion != NULL )
			m_pXSkinAnim->RemoveModelXSkinMesh( m_pMotion );

		m_pMotion			= g_pKTDXApp->GetDeviceManager()->OpenXSkinMesh( pWstrMeshName_ );

		m_CollisionListSet.clear();
		m_AttackListSet.clear();

 		CKTDXDeviceXET *pAnimXet = g_pKTDXApp->GetDeviceManager()->OpenXET( L"Mesh_Arme_ABM_Transform_Face.XET" );
		if(GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO25 ) == true )
		{
			CKTDXDeviceXET *pChangeTextureXet = NULL;
			if( GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				pChangeTextureXet = g_pKTDXApp->GetDeviceManager()->OpenXET( L"Mesh_Arme_ADW_Cash_M_A.xet");
			else
				pChangeTextureXet = g_pKTDXApp->GetDeviceManager()->OpenXET( L"Mesh_Arme_BM_Cash_M_A.xet");

			m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, pAnimXet, NULL, pChangeTextureXet );
		}
		else
		m_pXSkinAnim->AddModelXSkinMesh( m_pMotion, pAnimXet, NULL, NULL );

 		m_pXSkinAnim->SetModelDetailPercent( g_pData->GetModelDetailPercent() );

		m_CollisionListSet.insert( &m_pXSkinAnim->GetCollisionDataList() );
		m_AttackListSet.insert( &m_pXSkinAnim->GetAttackDataList() );

		CX2GUUser::SetShowViewList(false);

		if( bChangeWeapon_ == true )
		{
			ChangeWeapon(m_ptrTransformWeapon);

			CKTDGXSkinAnim* pAnim = NULL;
			for( int j=0; j<CX2Item::MAX_MODEL_COUNT_A_ITEM; j++ )
			{
				pAnim = m_ptrTransformWeapon->GetNormalAnim(j);
				if( pAnim != NULL)
				{
					if ( false == pAnim->GetCollisionDataList().empty() )
						m_CollisionListSet.insert( &pAnim->GetCollisionDataList() );

					if ( false == pAnim->GetAttackDataList().empty() )
						m_AttackListSet.insert( &pAnim->GetAttackDataList() );
				}
			}
		}
		else
		{
			for(UINT i=0; i<m_ViewEqipList.size(); ++i)
			{
				CX2EqipPtr pViewEquipPtr = m_ViewEqipList[i];
				if ( NULL != pViewEquipPtr )
				{
					if ( pViewEquipPtr->GetEqipPos() == CX2Unit::EP_WEAPON_HAND )
					{
						pViewEquipPtr->SetShowObject(true);
						break;
					}
				}
			}
		}

		SetEnableAttackBox( L"Rfoot", false );
		SetEnableAttackBox( L"Lfoot", false );
	}

	void CX2GUArme_VioletMage::TransformEnd( const bool bChangeWeapon_ /*= false */ )
	{

		if( m_pMotion != NULL && m_pOrgMotion != NULL && m_pMotion != m_pOrgMotion )
		{
			m_pXSkinAnim->RemoveModelXSkinMesh( m_pMotion );
			SAFE_CLOSE( m_pMotion );
		}

		m_pMotion = m_pOrgMotion;
		m_pOrgMotion = NULL;
		m_RenderParam.color = D3DXCOLOR( 1.f, 1.f, 1.f, 1.f );

		if ( bChangeWeapon_ )
		{
			for(UINT i=0; i<m_ViewEqipList.size(); ++i)
			{
				CX2EqipPtr pViewEquipPtr = m_ViewEqipList[i];
				if ( NULL != pViewEquipPtr )
				{
					if ( pViewEquipPtr->GetEqipPos() == CX2Unit::EP_WEAPON_HAND )
					{
						ChangeWeapon( pViewEquipPtr );
						m_ptrTransformWeapon->SetShowObject(false);
						break;
					}
				}
			}
		}

		CX2GUUser::SetShowViewList( true );
		CX2GUUser::UpdateNotRenderingOtherEquipList();
		CX2GUUser::UpdateEquipCollisionData( m_ViewEqipList );

		SetEnableAttackBox( L"Rfoot", false );
		SetEnableAttackBox( L"Lfoot", false );

		if( m_ahEffectSet[EffSetID_Wing] != INVALID_EFFECTSET_HANDLE )
		{
			g_pX2Game->GetEffectSet()->StopEffectSet( m_ahEffectSet[EffSetID_Wing] );
		}

		// �׳� ������ ���� �ϵ���...
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_MagicalMakeup_End", this );

	}

 	void CX2GUArme_VioletMage::ChangeWeapon( CX2EqipPtr ptrWeaon )
 	{
 		CX2GameUnit::Weapon* pOldWeapon = m_vecpWeapon[0];

 		if ( NULL != pOldWeapon &&
 			pOldWeapon->m_pUserWeaponEquip != NULL &&
 			pOldWeapon->m_pUserWeaponEquip->GetItem() != NULL )
 		{
 			int iEnchantLevel = pOldWeapon->m_pUserWeaponEquip->GetEnchantLevel();
 			const CX2Item::ItemTemplet* pOldItemTemplet =
                pOldWeapon->m_pUserWeaponEquip->GetItem()->GetItemTemplet();

 			pOldWeapon->m_pUserWeaponEquip.reset();
 			SAFE_DELETE_KTDGOBJECT( pOldWeapon );
 			m_vecpWeapon.resize(0);

 			// ��ȭ ����Ʈ�� ���� Enchant level ����
 			ptrWeaon->ChangeEnchantLevel( iEnchantLevel );
 			CreateWeapon( pOldItemTemplet, ptrWeaon );
			ptrWeaon->SetShowObject(true);

			m_bNeedToUpdateWeaponEffect = true;
		}
 	}

	void CX2GUArme_VioletMage::ABM_ComboZ1EventProcess()
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else if( m_InputData.oneZ == true &&
			m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.43f )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.43f )
		{
			StateChange( AVSI_COMBO_Z2 );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		SKILL_CANCEL_AFTER( 0.01f )
		else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}
	void CX2GUArme_VioletMage::ABM_ComboZ2EventProcess()
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else if( m_InputData.oneZ == true &&
			m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.433f )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.433f )
		{
			if( m_InputData.pureUp == true )
				StateChange( AVSI_COMBO_Z3up );
			else
				StateChange( AVSI_COMBO_Z3 );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}
	void CX2GUArme_VioletMage::ABM_ComboZ3EventProcess()
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else if( m_InputData.oneZ == true &&
			m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.251f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.466f )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
		else if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.466f )
		{
			StateChange( AVSI_COMBO_Z4 );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		else if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.647f && CAN_WALK_CANCEL )
		{
			StateChange( USI_WALK );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		else if( CAN_DASH_CANCEL )
		{
			StateChangeDashIfPossible();
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		SKILL_CANCEL_AFTER( 0.01f )
		else if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			m_fCanNotAttackTime = 0.1f;
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}
	void CX2GUArme_VioletMage::ABM_ComboZ4EventProcess()
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
		else if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 1.133f && CAN_WALK_CANCEL )
		{
			StateChange( USI_WALK );
		}
		SKILL_CANCEL_AFTER( 1.345f )

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_ABM_DASH_COMBO_Z2a_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
		{
			D3DXVECTOR3 pos = m_pXSkinAnim->GetCloneFramePosition( L"Bip01" );
			pos.y -= 200.0f;

			g_pX2Game->GetMinorXMeshPlayer()->CreateInstance( (CKTDGObject*) this,  L"ArmeEvasion",
				pos,
				m_FrameDataNow.unitCondition.dirDegree,
				m_FrameDataNow.unitCondition.dirDegree,
				XL_EFFECT_0 );

			if( m_iAdvancedTeleportationLv > 0 && GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO5 ) == true )
			{
				D3DXVECTOR2 superArmorTime( 0.0f, 0.5f);
				AccessVecNowSuperArmorTime().push_back(superArmorTime);
				m_FrameDataNow.stateParam.bSuperArmorNotRed = false;
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_ABM_DASH_COMBO_Z2a_EventProcess()
	{
		if( m_InputData.oneX == true )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.633f )
		{
			StateChange( ABM_DASH_COMBO_ZXX );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
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

	void CX2GUArme_VioletMage::AVSI_ABM_DASH_COMBO_ZXX_EventProcess()
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
		else
		{
			if( m_InputData.oneX == true &&
				m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.26f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.566f )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
			}

			if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.566f )
			{
				StateChange( ABM_DASH_COMBO_ZXXX );
				m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
			}
			else if ( CAN_WALK_CANCEL && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.466f )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
				StateChange( USI_WALK );
			}
			else if ( CAN_DASH_CANCEL && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.39f )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
				StateChangeDashIfPossible();
			}
		}


		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_ABM_DASH_COMBO_ZXXX_FrameMove()
	{
		if( m_pComboX3Up1->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
		{
			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
		}

    if( m_pXSkinAnim->EventTimerOneshot( 0.833f ) )
		{
			if( FlushMp( 10.0f ) == true )
			{
				if( (GetUnit()->GetClass() == CX2Unit::UC_ARME_BATTLE_MAGICIAN || GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH) &&
					IsTransformed() )
				{
					m_pComboX3Up1->SetScale( D3DXVECTOR3(2.f, 1.f, 2.f) );
					m_pComboX3Up2->SetScale( D3DXVECTOR3(2.f, 1.f, 2.f) );
					m_pComboX3Up3->SetScale( D3DXVECTOR3(2.f, 1.f, 2.f) );
				}
				else
				{
					m_pComboX3Up1->SetScale( D3DXVECTOR3(1.f, 1.f, 1.f) );
					m_pComboX3Up2->SetScale( D3DXVECTOR3(1.f, 1.f, 1.f) );
					m_pComboX3Up3->SetScale( D3DXVECTOR3(1.f, 1.f, 1.f) );
				}
				m_pComboX3Up1->GetRenderParam()->color = 0xffffffff;
				m_pComboX3Up2->GetRenderParam()->color = 0xffffffff;
				m_pComboX3Up3->GetRenderParam()->color = 0xffffffff;

				D3DXVECTOR3 pos = GetPos();
				pos.y += 5.0f;
				m_pComboX3Up1->GetMatrix().Move( pos );
				m_pComboX3Up1->GetMatrix().RotateDegree( GetRotateDegree() );
				m_pComboX3Up2->GetMatrix().Move( pos );
				m_pComboX3Up2->GetMatrix().RotateDegree( GetRotateDegree() );
				m_pComboX3Up3->GetMatrix().Move( pos );
				m_pComboX3Up3->GetMatrix().RotateDegree( GetRotateDegree() );

				m_pComboX3Up1->ChangeAnim( L"ComboX3Up-1", false );
				m_pComboX3Up2->ChangeAnim( L"ComboX3Up-2", false );
				m_pComboX3Up3->ChangeAnim( L"ComboX3Up-3", false );

				m_pComboX3Up1->Play( CKTDGXSkinAnim::XAP_ONE );
				m_pComboX3Up2->Play( CKTDGXSkinAnim::XAP_ONE );
				m_pComboX3Up3->Play( CKTDGXSkinAnim::XAP_ONE );
				m_pComboX3Up3->SetPlaySpeed( 0.8f );

				m_DamageData.attackerType	= CX2DamageManager::AT_MESH;
				m_DamageData.bCanRevenge	= false;
				m_DamageData.pAttackerAnim	= m_pComboX3Up1;
			}
			else
			{
				CreateStepDust();
			}
		}

		CommonFrameMove();
	}
	void CX2GUArme_VioletMage::AVSI_ABM_DASH_COMBO_ZXXX_EventProcess()
	{
		if( GetUnit()->GetUnitData().m_UnitClass == CX2Unit::UC_ARME_DIMENSION_WITCH )
		{
			if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 1.014f )
			{
				StateChange( AVSI_ADW_COMBO_ADD_X2 );
				m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
			}
			else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.6f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 1.014f )
			{
				m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
			}
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

	void CX2GUArme_VioletMage::AVSI_A_AV_FIRE_ROAD_Start()
	{
		CommonStateStart();

		// To Do...
	}

	void CX2GUArme_VioletMage::AVSI_A_AV_FIRE_ROAD_EventProcess()
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
		else if ( CAN_DASH_CANCEL && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.25f )
		{
			StateChangeDashIfPossible();
		}
		else if ( CAN_WALK_CANCEL && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.638f )
		{
			StateChange( USI_WALK );
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_A_AV_FIRE_ROAD_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.24f ) )
		{
			D3DXVECTOR3 vPos = GetPos();
			vPos.y += 70.f;

			float fLifeTimeRate = 1.f;
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AV_FIRE_ROAD );
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				fLifeTimeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );

			}

			g_pX2Game->GetDamageEffect()->CreateInstance( this, L"ARME_FIRE_ROAD", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, -1.f, fLifeTimeRate);


		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_A_ABM_ENERGY_DRAIN_READY_FrameMove()
	{
		// To Do...
    if( m_pXSkinAnim->EventTimerOneshot( 0.47f ) )
		{
			float fDistance = 2000.f;
			m_iDrainEnergyTarget = -1;
			const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_ENERGY_DRAIN );
			if( NULL != pSkillTemplet )
			{
				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

				float fDistanceTemp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );

				fDistance = fDistanceTemp;

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
						m_iDrainEnergyTarget = i;
					}
				}
			}

			if( m_iDrainEnergyTarget >= 0 )
			{
				StateChange( AVSI_A_ABM_ENERGY_DRAIN_SUCCESS, true );
			}
			else
			{
				StateChange( AVSI_A_ABM_ENERGY_DRAIN_FAIL, true );
			}
		}
		else if( m_iDrainEnergyTarget < 0 && m_pXSkinAnim->IsAnimationEnd() == true )
		{
			StateChange( AVSI_A_ABM_ENERGY_DRAIN_FAIL, true );
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_A_ABM_ENERGY_DRAIN_READY_EventProcess()
	{
// 		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
// 		{
// 			if( false == IsOnSomethingFuture() )
// 			{
// 				StateChange( USI_JUMP_DOWN );
// 				m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
// 			}
// 			else
// 				StateChange( AVSI_A_ABM_ENERGY_DRAIN_FAIL );
// 		}
//
// 		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_A_ABM_ENERGY_DRAIN_SUCCESS_FrameMove()
	{
		// To Do...
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
		{
			if( m_iDrainEnergyTarget >= 0 )
			{
				CX2GameUnit *pUnit = g_pX2Game->GetUnit( m_iDrainEnergyTarget );

				if( pUnit != NULL && g_pX2Game->IsValidUnit( pUnit ) == true )
				{
					const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_ENERGY_DRAIN );
					if( NULL != pSkillTemplet )
					{
						if ( NULL == GetUnit() )
							return;

						const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

						const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

						float fDrainRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_INTAKE_ABS, iSkillTempletLevel );

						if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO13 ) == true && GetRandomFloat() <= 0.3f )
							fDrainRate *= 2.f;
						m_DamageData.optrAttackerGameUnit	= this;
						m_DamageData.impactPoint			= pUnit->GetPos();
						m_DamageData.optrDefenderGameUnit	= pUnit;
						m_DamageData.reActResult			= m_DamageData.reActType;
						m_DamageData.m_fDrainHpRate		= fDrainRate;
						m_DamageData.m_fDrainMpByReact		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillTempletLevel );
						m_DamageData.m_fDrainMpRate		= fDrainRate;

						pUnit->DamageReact(&m_DamageData);

						// ������ ����Ʈ
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_EnegryDrain01", this );


						// �ǰ��� ����Ʈ
						g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_EnegryDrain02", pUnit, pUnit );
					}
				}

				m_iDrainEnergyTarget = -1;
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_A_ABM_ENERGY_DRAIN_EventProcess()
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
		else if ( CAN_WALK_CANCEL && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.451f )
		{
			StateChange( USI_WALK );
		}
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_HEAVY_PRESS_FrameMove()
	{
		// ���� ���� �ƴ����� ���� World�� ����
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.1f, 0 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
		{
			ClearHitUnitList_LUA();
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_ENERGY_SPURT_FrameMove()
	{
		// ���� ���� �ƴ����� ���� World�� ����
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.03f, 1 );

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_GUILLOTINE_PRESS_FrameMove()
	{
		// ���� ���� �ƴ����� ���� World�� ����
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.03f, 1 );

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_GUILLOTINE_PRESS_START_EventProcess()
	{
		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			if( false == IsOnSomethingFuture() )
			{
				StateChange( AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP );
				m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
			}
			else
				StateChange( AVSI_SI_SA_ABM_GUILLOTINE_PRESS_END );
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP_EventProcess()
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		else
			StateChange( AVSI_SI_SA_ABM_GUILLOTINE_PRESS_END );

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_SUPER_NOVA_FrameMove()
	{
		// ���� ���� �ƴ����� ���� World�� ����
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Bip01_Spine1", 0.02f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
		{
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_SuperNova01", this );
		}

    if( m_pXSkinAnim->EventTimerOneshot( 1.8f ) )
		{
			int nCount = 0;
			for( int i = 0; i < g_pX2Game->GetUnitNum(); ++i )
			{
				CX2GameUnit *pUnit = g_pX2Game->GetUnit( i );

				if( g_pX2Game->IsValidUnit( pUnit ) == false )
					continue;
				if( pUnit->GetInvincible() == true )
					continue;
				if( pUnit->GetGameUnitState() == CX2GameUnit::GUSI_DIE || pUnit->GetNowHp() <= 0.f )
					continue;

				float fOffset = 0.f;
				if(pUnit->GetGameUnitType() == CX2GameUnit::GUT_NPC)
				{
					CX2GUNPC *pNpc = (CX2GUNPC*)pUnit;
					if( pNpc->GetNPCTemplet().m_ClassType != CX2UnitManager::NCT_BASIC )
						continue;

					fOffset = ( pNpc->GetUnitWidth(false) + pNpc->GetUnitHeight(false) ) / 2.f;
				}
				else
				{
					CX2GUUser *pUser = (CX2GUUser*)pUnit;
					if( pUser->IsMyUnit() == true )
						continue;
				}

				D3DXVECTOR3 vDistance = GetPos() - pUnit->GetPos();
				float fDistanceUnit = D3DXVec3Length( &vDistance ) - fOffset;
				if( fDistanceUnit <= 700.f )
				{
					++nCount;
				}
			}

			float fScale = 1.f + (nCount / 10.f);
			if( fScale < 1.f )
				fScale = 1.f;
			else if( fScale > 2.f )
				fScale = 2.f;
			float fDamageScale = 1.f + ( nCount * 0.05f );
			if( fDamageScale < 1.f )
				fDamageScale = 1.f;
			else if( fDamageScale > 1.5f )
				fDamageScale = 1.5f;

			CX2EffectSet::Handle hSuperNova = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_SuperNova02", this );
 			if( hSuperNova != INVALID_EFFECTSET_HANDLE )
 			{
 				if ( CX2EffectSet::EffectSetInstance* pSuperNova = g_pX2Game->GetEffectSet()->GetEffectSetInstance( hSuperNova ) )
				{
					pSuperNova->SetPowerRateScale( fDamageScale );
 					pSuperNova->SetEffectScale( D3DXVECTOR3( fScale * GetScaleByUnit().x, fScale * GetScaleByUnit().y, fScale * GetScaleByUnit().z ), true );
				}
 			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_MAGICAL_MAKEUP_Init()
	{

		XSkinMeshReadyInBackground(  L"Mesh_Arme_BM_Cash_Upbody_Transforming_Sorted.X" );
		XSkinMeshReadyInBackground(  L"Mesh_Arme_BM_Cash_Upbody_Transform_Sorted.X" );

		if( GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
		{
			XSkinMeshReadyInBackground(  L"Mesh_Arme_ADW_Cash_Upbody_Transforming_Sorted.X" );
			XSkinMeshReadyInBackground(  L"Mesh_Arme_ADW_Cash_Upbody_Transform_Sorted.X" );
		}

	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_MAGICAL_MAKEUP_FrameMove()
	{
		// ���� ���� �ƴ����� ���� World�� ����
		ChangeWorldColorByHyperMode();
		ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.03f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.7f ) )
		{
			SetTransformTime( _CONST_AISHA_::MAGICAL_MAKEUP_TRANSFORMING_TIME );	/// ���� ������ �����ϴ� �ð�
			SetUseWorldColor( true );

			if( GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				TransformStart(  L"Mesh_Arme_ADW_Cash_Upbody_Transforming_Sorted.X" );
			else
				TransformStart(  L"Mesh_Arme_BM_Cash_Upbody_Transforming_Sorted.X" );
		}
    if( m_pXSkinAnim->EventTimerOneshot( 2.0f ) )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet
				= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ABM_MAGICAL_MAKEUP );
			if(GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO25 ) == true )
				SetBuffFactorToGameUnit( pSkillTemplet, 1 );
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ABM_MAGICAL_MAKEUPR_StateEnd()
	{
		// To Do...

		if ( !IsTransformed() )
		{
			SetTransformTime( 0.0f );	/// ���� ���� ȿ�� ������ ������.
			const CX2SkillTree::SkillTemplet* pSkillTemplet
				= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_SA_ABM_MAGICAL_MAKEUP );
			SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}

		SetForceInvincible( 1.f );
		CommonStateEnd();
	}

/*virtual*/ void CX2GUArme_VioletMage::DeleteMinorParticle()
{
	CX2GUUser::DeleteMinorParticle();

	DeleteArmeMinorParticle();
}

void CX2GUArme_VioletMage::ShowActiveSkillCutInAndLight( const WCHAR* szBoneName_, const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ /*= false */ )
{
    if( m_pXSkinAnim->EventTimerOneshot( fTimeToShow_ ) )
	{
		if ( GetShowCutInAndChangeWorldColor() && GetShowActiveSkillShow() )
 			ActiveSkillShow( szBoneName_, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].fileName.c_str(), s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].vSize, s_SkillCutInSet[m_iSkillCutInSetIndex].cutIn[uiCutInIndex_].vPosition, IsHyperState(), bOnlyLight_ );
	}
}

	//AVSI_HIGH_MAGICIAN_COMBO_X3down
	void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X3down_FrameMove()
	{
		if( m_pComboX3Up1->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
		{
			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
		}

    if( m_pXSkinAnim->EventTimerOneshot( 0.06f ) )
		{
			if( FlushMp( 10.0f ) == true )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ComboXXDownX", this );
			}
			else
			{
				CreateStepDust();
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X3down_EventProcess()
	{
		if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.6f )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
		}
		if( m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.6f && m_FrameDataFuture.stateParam.bEventFlagList[0] == true )
		{
			StateChange( AVSI_COMBO_X4down );
		}
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			//m_fCanNotAttackTime = 0.5f;
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}

	//AVSI_HIGH_MAGICIAN_COMBO_X4down
	void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X4down_FrameMove()
	{
		if( m_pComboX3Up1->GetState() == CKTDGXSkinAnim::XAS_PLAYING )
		{
			if( g_pData->GetDamageManager()->DamageCheck( &m_DamageData ) == true )
			{
				m_FrameDataNow.unitCondition.bHit = true;
			}
		}

    if( m_pXSkinAnim->EventTimerOneshot( 0.35f ) )
		{
			if( FlushMp( 10.0f ) == true )
			{
				g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ComboXXDownXX", this );
			}
			else
			{
				CreateStepDust();
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_HIGH_MAGICIAN_COMBO_X4down_EventProcess()
	{
		if( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
		}
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( 0.8333 )
		{
			StateChange( USI_WAIT );
		}
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
		{
			StateChange( USI_WAIT );
		}

		CommonEventProcess();
	}

	void CX2GUArme_VioletMage::AVSI_DIMENSION_WITCH_DASH_ATTACK_X_EventProcess()
	{
		if( m_FrameDataFuture.stateParam.bEventFlagList[0] == true && m_pXSkinAnimFuture->GetNowAnimationTime() > 0.604f )
		{
			StateChange( AVSI_DASH_ATTACK_X2 );
			m_FrameDataFuture.stateParam.bEventFlagList[0] = false;
		}
		else if( m_InputData.oneX == true && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.3f && m_pXSkinAnimFuture->GetNowAnimationTime() <= 0.604f )
		{
			m_FrameDataFuture.stateParam.bEventFlagList[0] = true;
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
	void CX2GUArme_VioletMage::AVSI_DIMENSION_WITCH_DASH_ATTACK_X2_EventProcess()
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
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
	}
	void CX2GUArme_VioletMage::AVSI_ADW_COMBO_ADD_X_EventProcess()
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
		WALK_CANCEL_AFTER( m_fWalkCancelAfter )
		SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
		DASH_CANCEL_AFTER( m_fDashCancelAfter )

		CommonEventProcess();
	}


	void CX2GUArme_VioletMage::AVSI_SI_A_ADW_DISTORTION_Init()
	{
		XSkinMeshReady( L"aisha_A_DISTORTION_dummy.X" );
	}

	void CX2GUArme_VioletMage::AVSI_SI_A_ADW_DISTORTION_StateStart()
	{
		CommonStateStart();

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ADW_DISTORTION_WEAPON", this );
	}


	void CX2GUArme_VioletMage::AVSI_SI_A_ADW_DISTORTION_FrameMove()
	{
    if( m_pXSkinAnim->EventTimerOneshot( 0.01f ) )
		{
			// ��ȯ �����ð���CX2Game::SetUserSummonedNPCInfo ���� ó���ϵ��� ����
			if( true == g_pX2Game->IsHost() )
			{
				SetSelfDestructSummonedNPC( CX2UnitManager::NUI_ADW_DISTORTION );

				// ��ġ ����
				D3DXVECTOR3 vPosOffset = GetDirVector() * 150.f * GetScaleByUnit().x;
				D3DXVECTOR3 vPos = (true == GetIsRight()) ? GetPos() + vPosOffset : GetPos()  - vPosOffset ;
				vPos.y += 100.f * GetScaleByUnit().x;

				// ���� : 1 , �̰��� : 0
				int iHyperMode = ( 0.f < GetRemainHyperModeTime() ) ? 1 : 0;

				g_pX2Game->CreateNPCReq( CX2UnitManager::NUI_ADW_DISTORTION, iHyperMode, true, vPos,
					GetIsRight(), 0.0f, true, -1, (CX2Room::TEAM_NUM) GetTeam(), CX2NPCAI::NAT_ALLY, GetUnitUID() );
			}
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ADW_MORNING_STAR_FrameMove()
	{
		ChangeWorldColorByHyperMode();

		ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.03f ) )
		{
			m_hEffectMorningStar = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ADW_MORNING_STAR_HAMMER", this );
			m_vecEffectSetToDeleteOnDamageReact.push_back( m_hEffectMorningStar );
		}
    else if( m_pXSkinAnim->EventTimerOneshot( 1.49f ) )
		{
			if( m_hEffectMorningStar != INVALID_EFFECTSET_HANDLE )
			{
				if ( CX2EffectSet::EffectSetInstance* pEffectSetInstance
					= g_pX2Game->GetEffectSet()->GetEffectSetInstance( m_hEffectMorningStar ) )
				{
					vector<CX2EffectSet::EffectInstance*>& vecEffectInstance = pEffectSetInstance->m_vecpEffectInstance;
					for ( UINT i=0; i < vecEffectInstance.size(); ++i )
					{
                        CX2DamageEffect::CEffect *pDmageEffect = g_pX2Game->GetDamageEffect()->GetInstance( vecEffectInstance[i]->m_hDamageEffect );
						if ( CKTDGXMeshPlayer::CXMeshInstance *pMeshInstance = ( pDmageEffect != NULL ) ? pDmageEffect->GetMainEffect() : NULL )
						{
							D3DXVECTOR3 vPos
								= pMeshInstance->GetBonePos( L"effect_Dummy03" );

							[[maybe_unused]] CX2EffectSet::Handle hMorningStarHit = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ADW_MORNING_STAR_HIT", this, NULL, false, -1.f, -1.f, D3DXVECTOR3(1.f, 1.f, 1.f), true, vPos );

							CX2DamageEffect::CEffect *pDmageHitEffect
								= g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DAMAGE_EFFECT_ADW_MORNING_STAR_HIT", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );
							if ( NULL != pDmageHitEffect )
							{
								CX2DamageManager::DamageData& damageData = pDmageHitEffect->GetDamageData();
								//if ( NULL != pDamageData )
								{
									const CX2SkillTree::SkillTemplet* pSkillTemplet
										= GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADW_IMPACT_HAMMER );

									if( NULL != pSkillTemplet && !pSkillTemplet->m_vecBuffFactorPtr.empty() )
									{
										if ( NULL == GetUnit() )
											return;

										const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
										const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

										damageData.PushBuffFactor( pSkillTemplet->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel ) );
									}

									break;
								}
							}
						}
					}	/// for
				}	/// if
			}	/// if
		}

		CommonFrameMove();
	}

	void CX2GUArme_VioletMage::AVSI_SI_SA_ADW_SCREWDRIVER_FrameMove()
	{
		ChangeWorldColorByHyperMode();
		ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 0 );

		CommonFrameMove();
	}


/*virtual*/ void CX2GUArme_VioletMage::HyperModeBuffEffectStart()
{
	CX2GUUser::HyperModeBuffEffectStart();

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRFoot )
		m_hHyperBoostRFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightViolet",	0,0,0, 0, 0 );

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLFoot )
		m_hHyperBoostLFoot = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftViolet",	0,0,0, 0, 0 );

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostRArm )
		m_hHyperBoostRArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostRightViolet",	0,0,0, 0, 0 );

	if ( INVALID_PARTICLE_SEQUENCE_HANDLE == m_hHyperBoostLArm )
		m_hHyperBoostLArm = g_pX2Game->GetMinorParticle()->CreateSequenceHandle( (CKTDGObject*) this,  L"HyperBoostLeftViolet",	0,0,0, 0, 0 );
}

/** @function : GetAdditionalAccelBuffTime
	@brief : ���������ͷ� ������ �߰��ð��� ������ �Լ�
	@param : � ���������Ͱ� �ߵ��ƴ����� ���� ��ų���̵�(eSkillID_)
	@return : �߰��ð�(float)
*/
/*virtual*/ float CX2GUArme_VioletMage::GetAdditionalAccelBuffTime( const CX2SkillTree::SKILL_ID /*eSkillID_*/ ) const
{
	float fAdditionalAccelBuffTime = 0.0f;

	/// ���, ��°���
	{


		// ��� ������ �нú� �̹Ƿ� �⺻ ������ ���� �޵��� ����
		const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADW_SPIRIT_ACCELERATION, true );


		if ( iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ADW_SPIRIT_ACCELERATION );

			if( NULL != pSkillTemplet )
			{
				fAdditionalAccelBuffTime += pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel );
			}
		}
	}

	/// ���̸�����, �ݷ��� ���
	{
		const int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA );
		if ( iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_COMMON_SUPPORT_AURA );

			if( NULL != pSkillTemplet )
			{
				fAdditionalAccelBuffTime += pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillLevel );
			}
		}
	}

	return fAdditionalAccelBuffTime;
}

/** @function : ApplyAuraAcceleratorBuffToGameUnit
	@brief : �׼��������͹����� Ư�� �������� ���ֿ��� ����, ���̻��� �ΰ����� ȿ�� ó���� ���� overriding
	@param : �ټ��������ͽ�ų���ø�(pSkillTemplet_), �ܺο���(fAddtionalAccelBuffTime_)
*/
/*virtual*/ void CX2GUArme_VioletMage::ApplyAuraAcceleratorBuffToGameUnit( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const float fAddtionalAccelBuffTime_, const D3DXVECTOR3& vMyPos_ )
{
	CX2BuffFactorPtr ptrEnergyOfThePlentyFactor
		= GetEnergyOfThePlentyBuffFactorClonePtr( pSkillTemplet_->m_eID );

	/// ǳ���� ����� �� �������
	if ( NULL == ptrEnergyOfThePlentyFactor )
		CX2GUUser::ApplyAuraAcceleratorBuffToGameUnit( pSkillTemplet_, fAddtionalAccelBuffTime_, vMyPos_ );
	else	/// ǳ���� ����� �������
	{
		if ( false == pSkillTemplet_->m_vecBuffFactorPtr.empty() )	/// �׼��������Ͱ˻�
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

			CX2BuffFactorPtr ptrBuffFactorClone = pSkillTemplet_->m_vecBuffFactorPtr[0]->GetClonePtr( iSkillTempletLevel );
			if ( NULL != ptrBuffFactorClone )
			{
				ptrBuffFactorClone->SetGameUnitBuffFactor( this );	/// ������������ �������� ����
				ptrBuffFactorClone->ApplyExternalFactorToFinalizer( BFT_TIME, fAddtionalAccelBuffTime_ );

				ptrEnergyOfThePlentyFactor->SetGameUnitBuffFactor( this );	/// ǳ���� ��� �������� ����

				if ( NULL == GetUnit() )
					return;

				const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

				const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet_->m_eID ) );	/// ��ų ����

				ptrEnergyOfThePlentyFactor->ApplyExternalFactorToFinalizer( BFT_TIME, 	/// ǳ���� ����� �ܺο��ο� ���� �߰� �ð� 1/3 ����
					( pSkillTemplet_->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel ) + fAddtionalAccelBuffTime_ ) / 3 );

				const WCHAR* pwsCoreParticleName = CX2SkillTree::GetAccelCoreParticleName( pSkillTemplet_->m_eID );
				ASSERT( NULL != pwsCoreParticleName );

				for ( int i = 0; i < g_pX2Game->GetUnitNum(); i++ )
				{
					CX2GameUnit* pGameUnit = g_pX2Game->GetUnit( i );
					if( pGameUnit != NULL &&
						GetDistance3Sq( vMyPos_, pGameUnit->GetPos() ) < 490000.f &&	/// �Ÿ� ����
						pGameUnit->CanApplyBuffToGameUnit() )
					{
						if ( pGameUnit->GetTeam() == GetTeam() )	/// ���� ��
						{
							if ( g_pX2Game->GetGameType() == CX2Game::GT_PVP || pGameUnit->GetUnitUID() == GetUnitUID() ||
								( NULL != g_pData && NULL != g_pData->GetPartyManager() && true == g_pData->GetPartyManager()->IsMyPartyMember( pGameUnit->GetUnitUID() ) ) )
							{
								if ( pGameUnit->SetBuffFactorToGameUnit( *ptrBuffFactorClone ) )
								{
									D3DXVECTOR3 vGameUnitPos = pGameUnit->GetPos();
									vGameUnitPos.y += 100.0f;
									g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  pwsCoreParticleName, vGameUnitPos );
								}
							}
						}
						else	/// �ٸ� �����Դ� ǳ���Ǳ�� �����
						{
							pGameUnit->SetBuffFactorToGameUnit( *ptrEnergyOfThePlentyFactor );
						}
					}
				}	// for
			} // if ( NULL != ptrBuffFactorClone )
		}
	}
}

/** @function : GetEnergyOfThePlentyBuffFactorClonePtr
	@brief : ǳ���Ǳ��(����) �нú� ��ų�� ������ �ִ� �������͸� ������ �Լ�
	@param : ����� �׼��������ͽ�ų ID(eSkillIdAccelerator_)
	@return : ǳ���Ǳ���� ����� �ʾ����� �� ����Ʈ������ ����
*/
CX2BuffFactorPtr CX2GUArme_VioletMage::GetEnergyOfThePlentyBuffFactorClonePtr( const CX2SkillTree::SKILL_ID eSkillIdAccelerator_ ) const
{
	const int iSkillLevelEnergyOfThePlenty = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AEM_ENERGY_OF_THE_PLENTY, true );

	if( iSkillLevelEnergyOfThePlenty > 0 )
	{
		const CX2SkillTree::SkillTemplet* pSkillTempletEnergyOfThePlenty
			= g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AEM_ENERGY_OF_THE_PLENTY );
		if ( NULL != pSkillTempletEnergyOfThePlenty )
		{
			if ( 0 < pSkillTempletEnergyOfThePlenty->m_vecBuffFactorID.size() )
			{
				int iValue = 0;

				switch ( eSkillIdAccelerator_ )	/// ����� ������ �׼��������� ��ų�� �����ΰ��� ����
				{
				case CX2SkillTree::SI_SA_COMMON_AURA_POWER_ACCEL:
					iValue = 0;
					break;
				case CX2SkillTree::SI_SA_COMMON_AURA_MAGIC_ACCEL:
					iValue = 1;
					break;
				case CX2SkillTree::SI_SA_COMMON_AURA_SHIELD_ACCEL:
					iValue = 2;
					break;
				default:
					return CX2BuffFactorPtr();
					break;
				}

				return pSkillTempletEnergyOfThePlenty->m_vecBuffFactorPtr[iValue]->GetClonePtr( iSkillLevelEnergyOfThePlenty );
			}

		}
	}

	return CX2BuffFactorPtr();
}

/** @function : SetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUArme_VioletMage::SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::SetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_SI_SA_ABM_MAGICAL_MAKEUP:
		{
			SetUseWorldColor( true );

			if( m_ahEffectSet[EffSetID_Wing] == INVALID_EFFECTSET_HANDLE )
			{
				if( GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				{
					if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO25) == true )
						m_ahEffectSet[EffSetID_Wing] = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ADW_MagicalMakeup_Wing_memo", this );
					else
					m_ahEffectSet[EffSetID_Wing] = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ADW_MagicalMakeup_Wing", this );
				}
				else
				{
					if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO25) == true )
						m_ahEffectSet[EffSetID_Wing] = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_MagicalMakeup_Wing_memo", this );
					else
					m_ahEffectSet[EffSetID_Wing] = g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_MagicalMakeup_Wing", this );
				}
			}

			//TransformEnd();

			if ( NULL == m_ptrTransformWeapon )
			{
				if( GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				{
					if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO25) == true )
						m_ptrTransformWeapon = CX2ItemManager::CreateDummyEquip( 41321, GetUnit(), m_pXSkinAnim );
					else
					m_ptrTransformWeapon = CX2ItemManager::CreateDummyEquip( 41320, GetUnit(), m_pXSkinAnim );
				}
				else
				{
					if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO25) == true )
						m_ptrTransformWeapon = CX2ItemManager::CreateDummyEquip( 40931, GetUnit(), m_pXSkinAnim );
					else
					m_ptrTransformWeapon = CX2ItemManager::CreateDummyEquip( 40930, GetUnit(), m_pXSkinAnim );
				}
				m_ptrTransformWeapon->SetManagerObject( this );
				g_pKTDXApp->GetDGManager()->AddObjectChain( m_ptrTransformWeapon.get() );
			}

			m_bIsTransformed = true;

			if( GetUnit()->GetClass() == CX2Unit::UC_ARME_DIMENSION_WITCH )
				TransformStart(  L"Mesh_Arme_ADW_Cash_Upbody_Transform_Sorted.X" , true );
			else
				TransformStart(  L"Mesh_Arme_BM_Cash_Upbody_Transform_Sorted.X" , true );

			SetUnitReForm();
		} break;


	default:
		break;
	}
}

/** @function : UnSetSpecificValueByBuffTempletID
	@brief : �� ���ָ��� Ư�� ������ ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� ���� ��ٴ� �÷��� ���� ��...)
	@param : �������ø�ID(eBuffTempletId_)
*/
/*virtual*/ void CX2GUArme_VioletMage::UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ )
{
	CX2GUUser::UnSetSpecificValueByBuffTempletID( eBuffTempletId_ );

	switch ( eBuffTempletId_ )
	{
	case BTI_SI_SA_ABM_MAGICAL_MAKEUP:
		{
			m_bIsTransformed = false;
			TransformEnd( true );
			SetUnitReForm();
			if ( NULL != m_ptrTransformWeapon )
			{
				g_pKTDXApp->GetDGManager()->RemoveObjectChain( m_ptrTransformWeapon.get() );
				m_ptrTransformWeapon.reset();
			}
		} break;

	case BTI_BUFF_SI_A_AHM_MEDITATION:
		{
			m_bInMeditation = false;			/// �޵����̼� ������ ���� ����
		} break;

	default:
		break;
	}
}

/** @function : ApplyRenderParamOnEmptyRenderParamByBuff
	@brief : ������ ���� RenderParam ���� ���� ��� ����Ǵ� RenderParam ó��
	@param : ��������� ����� �����Ķ� ����ü ������(pRenderParam_)
*/
/*virtual*/ void CX2GUArme_VioletMage::ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ )
{
	if ( m_bPassDash )
		return;

	if( GetTransformTime() > 0 )
	{
		float fAlpha = GetTransformTime() / _CONST_AISHA_::MAGICAL_MAKEUP_TRANSFORMING_TIME;
		m_RenderParam.renderType			= CKTDGXRenderer::RT_CARTOON_FADE;
		m_RenderParam.color = D3DXCOLOR( 1.f, 1.f, 1.f, fAlpha );
	}
	else
	{
		CX2GameUnit::ApplyRenderParamOnEmptyRenderParamByBuff( pRenderParam_ );
	}
}

/** @function : AdaptAdvancedTeleportationBuff
	@brief : �������� ���� ����
*/
void CX2GUArme_VioletMage::AdaptAdvancedTeleportationBuff()
{
	if ( 0 < m_iAdvancedTeleportationLv )
	{
		const int iSkillLevel
			= GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ADW_ADVANCED_TELEPORTATION, true );

		if ( iSkillLevel > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTemplet
				= GetUnit()->GetUnitData().m_UserSkillTree.GetUserSkillTemplet( CX2SkillTree::SI_P_ADW_ADVANCED_TELEPORTATION );

			if ( NULL != pSkillTemplet )
				SetBuffFactorToGameUnit( pSkillTemplet, 0 );
		}
	}
}


void CX2GUArme_VioletMage::SetDamageExceptionProcess( CX2DamageManager::DamageData* pDamageData_,OUT float& fPowerRate)
{
	CX2GUUser::SetDamageExceptionProcess(pDamageData_, fPowerRate);

	if( null != pDamageData_->optrAttackerGameUnit && pDamageData_->optrAttackerGameUnit->GetGameUnitType() == CX2GameUnit::GUT_USER )
	{
		CX2GUUser *pUser = static_cast<CX2GUUser*>( pDamageData_->optrAttackerGameUnit.GetObservable() );

		int iSkillLevelFitness = pUser->GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_ABM_FITNESS, true );

		if( iSkillLevelFitness > 0 )
		{
			const CX2SkillTree::SkillTemplet* pSkillTempletFitness = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_ABM_FITNESS );

			if( NULL != pSkillTempletFitness && pDamageData_->damageType == CX2DamageManager::DT_PHYSIC )
			{
				pDamageData_->fForceDownValue = pDamageData_->fForceDownValue *
					pSkillTempletFitness->GetSkillAbilityValue( CX2SkillTree::SA_FORCE_DOWN_RANGE_REL, iSkillLevelFitness );
				pDamageData_->fHitAddMP = pDamageData_->fHitAddMP *
					pSkillTempletFitness->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillLevelFitness );
			}
		}
	}
}

/*virtual*/ bool CX2GUArme_VioletMage::CanRidingState()
{
	if ( GetNowStateID() == CX2GUUser::USI_WAIT || GetNowStateID() == CX2GUUser::USI_WALK ||
		GetNowStateID() == CX2GUUser::USI_DASH || GetNowStateID() == CX2GUUser::USI_DASH_END ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT || GetNowStateID() == CX2GUUser::USI_RIDING_WALK ||
		GetNowStateID() == CX2GUUser::USI_RIDING_WAIT_HABIT || GetNowStateID() == CX2GUUser::USI_RIDING_DASH ||
		GetNowStateID() == CX2GUUser::USI_RIDING_DASH_END || GetNowStateID() == CX2GUArme_VioletMage::AVSI_CHARGE_MP )
		return true;

	return false;
}

//#pragma region SI_A_AV_ICICLE_WAVE
void CX2GUArme_VioletMage::AVSI_A_AV_Icicle_Wave_Init()
{
	XSkinMeshReadyInBackground( L"WindWard_XMesh01.X" );
	XSkinMeshReadyInBackground( L"IceNova_Effect_Mesh.X" );
	TextureReadyInBackground( L"Wind_WindWard01.tga" );
	TextureReadyInBackground( L"crystal_Nova.tga" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
	TextureReadyInBackground( L"Ice_Particle01.dds" );
	TextureReadyInBackground( L"Ice_Crystal_Ring01.dds" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
}

void CX2GUArme_VioletMage::AVSI_A_AV_Icicle_Wave_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.24f ) )
	{
		D3DXVECTOR3 vPos = GetPos();
		vPos.y += 70.f;

		float fLifeTimeRate = 1.f;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AV_ICICLE_WAVE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			fLifeTimeRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Aisha_Icicle_Wave", GetPowerRate(), vPos,
			GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y, false, -1.f, -1.f, fLifeTimeRate);
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_A_AV_Icicle_Wave_EventProcess()
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
	SKILL_CANCEL_AFTER( m_fSkillCancelAfter )
	DASH_CANCEL_AFTER( m_fDashCancelAfter )

	CommonEventProcess();
}
//#pragma endregion ���̽�Ŭ ���̺�

//#pragma region SI_SA_AV_ICE_STORM
void CX2GUArme_VioletMage::AVSI_SA_AV_Ice_Storm_Init()
{
	XSkinMeshReadyInBackground( L"ICE_Storm01.x" );
	XSkinMeshReadyInBackground( L"ICE_Storm01_1.x" );
	XSkinMeshReadyInBackground( L"ICE_Storm02.x" );
	XSkinMeshReadyInBackground( L"ICE_Storm03.x" );
	XSkinMeshReadyInBackground( L"ICE_Storm04.x" );
	TextureReadyInBackground( L"ICE_Storm03.tga" );
	TextureReadyInBackground( L"ICE_Storm02.tga" );
	TextureReadyInBackground( L"GroundShockWave.dds" );
	TextureReadyInBackground( L"Ice_Particle01.dds" );
	TextureReadyInBackground( L"steam_BP.dds" );
	TextureReadyInBackground( L"GroundShockWave02.dds" );
	TextureReadyInBackground( L"Ice_Crystal_Ring01.dds" );
	TextureReadyInBackground( L"Particle_blur.dds" );
}

void CX2GUArme_VioletMage::AVSI_SA_AV_Ice_Storm_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.12f, 2 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.2f ) )
	{
		bool bHyper = false;
		if ( GetRemainHyperModeTime() > 0.f )
			bHyper = true;

		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_AV_ICE_STORM", this, NULL, bHyper );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SA_AV_Ice_Storm_EventProcess()
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
//#pragma endregion ���̽� ����

//#pragma region SI_SA_AHM_AQUA_SHOWER
void CX2GUArme_VioletMage::AVSI_A_AHM_Aqua_Shower_Init()
{
	XSkinMeshReadyInBackground( L"Arme_Fireballs.x" );
	XMeshReadyInBackground( L"Ring_Plane.y" );
	XMeshReadyInBackground( L"SA_AHM_AQUA_SHOWER.y" );
	TextureReadyInBackground( L"AHM_FREEZING_POINT_01.tga" );
	TextureReadyInBackground( L"ColorBallGray_Gear.tga" );
	TextureReadyInBackground( L"Arme_ICEballS2.tga" );
	TextureReadyInBackground( L"AQUA_SHOWER_WAVE.tga" );
	TextureReadyInBackground( L"Ring.dds" );
	TextureReadyInBackground( L"PlusHPMP.dds" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"aisha_active_heavyPress_energy.dds" );
	TextureReadyInBackground( L"PlusHPMP.dds" );
	TextureReadyInBackground( L"RingAlpha.dds" );
	TextureReadyInBackground( L"Ice_Particle01.dds" );
	TextureReadyInBackground( L"RS_New_Sword_ICE.dds" );
	TextureReadyInBackground( L"Steam_BP.dds" );
	TextureReadyInBackground( L"acce_userAvatar_201205_bubble.dds" );
}

void CX2GUArme_VioletMage::AVSI_A_AHM_Aqua_Shower_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.13f, 0 );

    if ( m_pXSkinAnim->EventTimerOneshot( 0.13f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_AHM_AQUA_SHOWER", this, NULL );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_A_AHM_Aqua_Shower_EventProcess()
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
//#pragma endregion ������ ����

//#pragma region SI_SA_AHM_LIGHTNING_SHOWER
void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_Init()
{
	XSkinMeshReadyInBackground( L"SA_AEM_CHAIN_LIGHTNING_P01.x" );
	XMeshReadyInBackground( L"Up_Light01_Mesh.y" );
	XMeshReadyInBackground( L"raven_powrUp_M_lightning02.y" );
	TextureReadyInBackground( L"LIGHTNING_SHOWER_Emblem.tga" );
	TextureReadyInBackground( L"LIGHTNING_SHOWER_Square.tga" );
	TextureReadyInBackground( L"Arme_LightningBolts2a2.tga" );
	TextureReadyInBackground( L"LIGHTNING_SHOWER_Square.tga" );
	TextureReadyInBackground( L"Colorballgray.dds" );
	TextureReadyInBackground( L"aisha_SA_MorningStar_circle.dds" );
	TextureReadyInBackground( L"aisha_C_DashComboXX_circle.dds" );
	TextureReadyInBackground( L"eve_thousandsOfStars_gearLight02.dds" );
	TextureReadyInBackground( L"Bust_Wolf_P02.dds" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
	TextureReadyInBackground( L"WhitePointSmall.dds" );
	TextureReadyInBackground( L"Lightning_Piercing01.dds" );
	TextureReadyInBackground( L"Arme_ICEballS2.dds" );
	TextureReadyInBackground( L"ColorBallRed.dds" );
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_StateStart()
{
	CommonStateStart();

	m_bIsOnSomethingStart =false;
	if ( m_FrameDataNow.unitCondition.bFootOnLine == true )
		m_bIsOnSomethingStart = true;

	m_bDisableGravity = true;
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 500 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );

	m_iElapsedFireCount	= 0;		/// ����Ʈ�� ���� ���� ��ȯ ���� �ʱ�ȭ
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_FrameMove()
{
	ChangeWorldColorByHyperMode();

	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.01f, 1 );

    if( m_pXSkinAnim->EventTimerOneshot( 0.1f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_AHM_LIGHTNING_SHOWER", this );
	}

	if( m_iElapsedFireCount >= m_iMaxFireCount )	/// �ִ� �߻� Ƚ���� �Ѱ�ٸ�, �н�
		return;

	const int fCheckTime = static_cast<int>( m_FrameDataNow.unitCondition.fStateTime / m_fFireGap );

	if( fCheckTime >= m_iElapsedFireCount )
	{
		++m_iElapsedFireCount;	/// �߻� ī��Ʈ ����

		/// ��ü ���� : -850 ~ 850, 1�׷� : -850 ~ 340, 2�׷� : -340  ~ 850		-> 1, 2 �׷��� �ݺ��ϸ� �߾��� ��ġ�� ���� ���� ����
		float fRadius = static_cast<float>( ( GetRandomInt( m_iElapsedFireCount * 4 + 1 ) - 50 ) * 5 );

		if( 0 == m_iElapsedFireCount % 2 )
			fRadius += 250.f;
		else
			fRadius -= 250.f;

		if( true == GetIsRight() )		/// ���⿡ ���� ��ġ ����
			fRadius += 250.f;
		else
			fRadius -= 250.f;

		D3DXVECTOR3 vDirVector = GetDirVector();

		D3DXVECTOR3 vEffectEndPosition = GetPos() + vDirVector * fRadius;
		D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0.f , 850.f, 0.f );

		D3DXVECTOR3 vRotation = GetRotateDegree();

		if( true == GetIsRight() )
		{
			vEffectStartPosition -= GetDirVector() * 500;
		}
		else
		{
			vEffectStartPosition += GetDirVector() * 500;
		}


		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Lightning_Shower_LineCrash", GetPowerRate(), vEffectStartPosition,
			vRotation, vRotation, vEffectEndPosition.y );
	}

	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		pSeqArmeAirStepRight->SetEmitRate(10,20);
	}

	if( pSeqArmeAirStepLeft != NULL )
	{
		pSeqArmeAirStepLeft->SetEmitRate(10,20);
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_EventProcess()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	}
	else if ( m_iElapsedFireCount >= m_iMaxFireCount )
	{
		StateChange( AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP );
	}
    else if ( m_pXSkinAnim->EventTimerOneshot( 1.5f ) )
	{
		m_bDisableGravity = false;
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_StateEnd()
{
	m_bDisableGravity = false;
	CommonStateEnd();
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_Loop_StateStart()
{
	CommonStateStart();

	SetNoDetonation( true );			/// ���� ���� ����

	m_iElapsedFireCount			= 0;	///	�߻� ī��Ʈ �ʱ�ȭ
	m_bDisableGravity			= true;	/// �߷� ����
	m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
	m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_Loop_FrameMove()
{
	ChangeWorldColorByHyperMode();


	if ( m_ExtraDamagePack.m_EnchantShock.m_fTime > 0.0f )
	{
		m_ExtraDamagePack.m_EnchantShock.m_fTime = 0;
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}



	if( m_iElapsedFireCount >= m_iMaxFireCountByLoopState )	/// �ִ� �߻� Ƚ���� �Ѱ�ٸ�, �н�
		return;

	const int fCheckTime = static_cast<int>( m_FrameDataNow.unitCondition.fStateTime / m_fFireGap );

	if( fCheckTime >= m_iElapsedFireCount )
	{
		++m_iElapsedFireCount;				/// ���� ī��Ʈ ����

		if( false == FlushMp( m_fUseMPConsume ) )
			return;

		/// ��ü ���� : -850 ~ 850, 1�׷� : -850 ~ 340, 2�׷� : -340  ~ 850		-> 1, 2 �׷��� �ݺ��ϸ� �߾��� ��ġ�� ���� ���� ����
		float fRadius = static_cast<float>( ( GetRandomInt( m_iElapsedFireCount * 4 + 1 ) - 50 ) * 5 );

		if( 0 == m_iElapsedFireCount % 2 )
			fRadius += 250.f;
		else
			fRadius -= 250.f;

		if( true == GetIsRight() )		/// ���⿡ ���� ��ġ ����
			fRadius += 250.f;
		else
			fRadius -= 250.f;

		D3DXVECTOR3 vDirVector = GetDirVector();

		D3DXVECTOR3 vEffectEndPosition = GetPos() + vDirVector * fRadius;
		D3DXVECTOR3 vEffectStartPosition = vEffectEndPosition + D3DXVECTOR3( 0.f , 850.f, 0.f );

		D3DXVECTOR3 vRotation = GetRotateDegree();

		if( true == GetIsRight() )
		{
			vEffectStartPosition -= GetDirVector() * 500;
		}
		else
		{
			vEffectStartPosition += GetDirVector() * 500;
		}

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Lightning_Shower_LineCrash", GetPowerRate(), vEffectStartPosition, vRotation, vRotation,
			vEffectEndPosition.y );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_Loop_EventProcess()
{
	if ( m_fUseMPConsume > GetNowMp() ||						/// ���� MP�� ���׿� ��ȯ ��뺸�� ���ų�
		m_iElapsedFireCount >= m_iMaxFireCountByLoopState ||	/// ���׿� ������ ��ȯ�ϴµ� ����� �� ���� MP �Ҹ���� �Ѱ�ġ�� �Ѱų�
		0.f >= GetNowHp() ||									/// �ǰ� ���ų�
		( m_InputData.pureA != true && m_InputData.pureS != true && m_InputData.pureD != true && m_InputData.pureC != true &&	/// ��ų Ű�� ������ ���� �ʴٸ� ����
		m_InputData.pureQ != true && m_InputData.pureW != true && m_InputData.pureE != true && m_InputData.pureR != true ) )
	{
		StateChange( AVSI_SA_AHM_LIGHTNING_SHOWER_END );
		m_FrameDataFuture.syncData.position.y -= LINE_RADIUS * 1.5f;
	}

	CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_Loop_StateEnd()
{
	m_bDisableGravity = false;		/// �߷� ����
	m_iElapsedFireCount = 0;		/// �߻� ī��Ʈ �ʱ�ȭ

	CommonStateEnd();
	SetNoDetonation( false );		/// ���� ���� ����
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_End_EventProcess()
{
	if( true == IsOnSomethingFuture() )
		StateChange( USI_JUMP_LANDING );

	CommonEventProcess();
}

void CX2GUArme_VioletMage::AVSI_SA_AHM_Lightning_Shower_End_StateEnd()
{
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepRight = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepRight );
	CKTDGParticleSystem::CParticleEventSequence* pSeqArmeAirStepLeft  = g_pX2Game->GetMinorParticle()->GetInstanceSequence( m_hSeqArmeAirStepLeft );

	if( pSeqArmeAirStepRight != NULL )
	{
		pSeqArmeAirStepRight->SetEmitRate(0,0);
	}

	if( pSeqArmeAirStepLeft != NULL )
	{
		pSeqArmeAirStepLeft->SetEmitRate(0,0);
	}

	CommonStateEnd();
}
//#pragma endregion ����Ʈ�� ����

//#pragma region SI_SA_AEM_CHAIN_LIGHTNING
void CX2GUArme_VioletMage::AVSI_SA_AEM_Chain_Lightning_Init()
{
	XSkinMeshReadyInBackground( L"SA_AEM_CHAIN_LIGHTNING_P01.X" );
	TextureReadyInBackground( L"aisha_SA_MorningStar_circle.dds" );
	TextureReadyInBackground( L"aisha_C_DashComboXX_circle.dds" );
	TextureReadyInBackground( L"eve_thousandsOfStars_gearLight02.dds" );
	TextureReadyInBackground( L"Arme_LightningBolts2a2.tga" );
}
void CX2GUArme_VioletMage::AVSI_SA_AEM_Chain_Lightning_StartFuture()
{
	CommonStateStartFuture();

	if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO17 ) == true && false == IsOnSomethingFuture())
	{
		m_bDisableGravity = true;
		m_PhysicParam.nowSpeed		= D3DXVECTOR2( 0, 0 );
		m_PhysicParam.passiveSpeed	= D3DXVECTOR2( -1, -1 );
	}
}
void CX2GUArme_VioletMage::AVSI_SA_AEM_Chain_Lightning_EndFuture()
{
	CommonStateEndFuture();

	m_bDisableGravity = false;
}
void CX2GUArme_VioletMage::AVSI_SA_AEM_Chain_Lightning_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Dummy2_Lhand", 0.13f, 0 );

	// ȿ��
    if( m_pXSkinAnim->EventTimerOneshot( 0.001f ) )
	{
		if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO17) == true )
			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_AEM_CHAIN_LIGHTNING_MEMO", this );
		else
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_SA_AEM_CHAIN_LIGHTNING", this );
	}

	// �߻�
    if( m_pXSkinAnim->EventTimerOneshot( 0.55f ) )
	{
		D3DXVECTOR3 vPos = GetWeaponBonePos( 0, L"TRACE_START0", 0 );
		[[maybe_unused]] bool bHyper = false;
		D3DXVECTOR3 vScale( GetScale(), GetScale(), GetScale() );

		if ( GetRemainHyperModeTime() > 0.f )
			bHyper = true;

		CX2DamageEffect::CEffect* pDamageEffect = g_pX2Game->GetDamageEffect()->CreateInstance( this,
			L"DamageEffect_AHM_Chain_Lightning", GetPowerRate(), vPos, GetRotateDegree(), GetRotateDegree(), m_FrameDataNow.unitCondition.landPosition.y );

		if ( NULL != pDamageEffect )
		{
			bool bUserUnit = false;
			UidType gameUnitUID = -1;

			if ( g_pMain->GetNowStateID() == CX2Main::XS_PVP_GAME )
				gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vPos, D3DXVECTOR2(150, 300), bUserUnit );
			else
				gameUnitUID = g_pX2Game->GetLockOnNearstTargetInRange( static_cast<CX2Room::TEAM_NUM>( GetTeam() ), vPos, D3DXVECTOR2(150, 750), bUserUnit );

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

void CX2GUArme_VioletMage::AVSI_SA_AEM_Chain_Lightning_EventProcess()
{
	if ( true == m_pXSkinAnimFuture->IsAnimationEnd() )
	{
		if ( false == IsOnSomethingFuture() )
		{
			StateChange( USI_JUMP_DOWN );
			m_FrameDataFuture.syncData.position.y -= LINE_RADIUS;
		}
		else
		StateChange( USI_WAIT );
		m_fCanNotAttackTime = 0.1f;
	} // if

	if( false == m_bDisableGravity )
	{
		const float LOWER_BOUND_INPUT_TIME_CANCEL = 1.2f;

		IF_SKILL_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		WALK_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		DASH_CANCEL_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		ELSE_IF_ARROW_ZXASDC_PRESSED_AFTER( LOWER_BOUND_INPUT_TIME_CANCEL )
		{
			StateChange( USI_WALK );
		}
	}

	CommonEventProcess();
}
//#pragma endregion ü�� ����Ʈ��

//#pragma region SI_A_AVP_SUMMON_BAT_HEAVY
void CX2GUArme_VioletMage::AVSI_A_AVP_Summon_Bat_Heavy_Init()
{
	XSkinMeshReadyInBackground( L"AVP_SUMMON_BAT_MESH_EFFECT.X" );
	TextureReadyInBackground( L"AVP_Effect128.tga" );
}

void CX2GUArme_VioletMage::AVSI_A_AVP_Summon_Bat_Heavy_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{

		D3DXVECTOR3 vPos = GetPos();
		if( true == GetIsRight() )
			vPos += GetDirVector() * 100.f;
		else
			vPos -= GetDirVector() * 100.f;

		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AVP_SUMMON_BAT_HEAVY_DOLL );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
			float fLifeTime = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );

			bool bHyper = false;
			if ( GetRemainHyperModeTime() > 0.f )
				bHyper = true;

			g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Summon_Bat_Heavy", this, NULL, bHyper, GetPowerRate(),
				fLifeTime );
		}

		vPos.y += 150.f;
		vPos += GetZVector() * -50.f;
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_Summon_Smoke", this, NULL, false, -1.f, -1.f,
			D3DXVECTOR3( 1, 1, 1 ), true, vPos, GetRotateDegree(), GetDirVector() );
	}

	CommonFrameMove();
}
//#pragma endregion �ڶ� �Ӱ��� - ���

//#pragma region SI_SA_AVP_Phantom_Breathing_Dark_Hole
void CX2GUArme_VioletMage::AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_Init()
{
	XSkinMeshReadyInBackground( L"Spin_Liner02.X" );
	XSkinMeshReadyInBackground( L"SA_AEM_CHAIN_LIGHTNING_P01.X" );
	TextureReadyInBackground( L"Spin_Liner02_Gray.tga" );
	TextureReadyInBackground( L"Arme_LightningBolts2a2.tga" );
	TextureReadyInBackground( L"Wind_WindWard01.tga" );
	TextureReadyInBackground( L"Condense_pulse01.dds" );
	TextureReadyInBackground( L"Arme_Ring2.dds" );
	TextureReadyInBackground( L"secretVelder_alchemystBoss_magicAttackC_circleLine.dds" );
	TextureReadyInBackground( L"Black_Circle01.dds" );
	TextureReadyInBackground( L"Arme01_Frost_Ring_LV2_effect_01.dds" );
}

void CX2GUArme_VioletMage::AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Bip01_L_Finger1Nub", 0.04f, 0 );

    if (m_pXSkinAnim->EventTimerOneshot( 0.25f ) )
	{
		g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_PHANTOM_BREATHING", this );
	}
    else if( m_pXSkinAnim->EventTimerOneshot( 0.815f ) )
	{
		D3DXVECTOR3 degree = GetRotateDegree();
		D3DXVECTOR3 pos = GetPos();
		D3DXVECTOR3 Dirvec = GetDirVector();

		Dirvec.y = 0;
		D3DXVec3Normalize( &Dirvec , &Dirvec );

		if ( GetIsRight() == true )
			pos += Dirvec*310.0f;
		else
		{
			degree.y += 180.0f;
			pos -= Dirvec*310.0f;
		}

		[[maybe_unused]] bool bHyper = false;
		if ( GetRemainHyperModeTime() > 0.f )
			bHyper = true;
		D3DXVECTOR3 vScale( GetScale(), GetScale(), GetScale() );

		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Phantom_Breathing_DarkHole_NoDamage", GetPowerRate(),
			pos, degree , degree, m_FrameDataNow.unitCondition.landPosition.y );

		CX2DamageEffect::CEffect* pDE = g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Phantom_Breathing_DarkHole_Damage", GetPowerRate(),
			pos, degree , degree ,m_FrameDataNow.unitCondition.landPosition.y );

		pos.y += 255;
		g_pX2Game->GetDamageEffect()->CreateInstance( this, L"DamageEffect_Phantom_Breathing_DarkHole_BlackHole", GetPowerRate(),
			pos, degree , degree ,m_FrameDataNow.unitCondition.landPosition.y );

		pos = GetBonePos( L"Bip01_L_Finger11" );
		g_pX2Game->GetMajorParticle()->CreateSequence( (CKTDGObject*) this,  L"Pa_AVP_PHANTOM_BREATHING_Finger", pos );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_EventProcess()
{
	if( m_pXSkinAnimFuture->IsAnimationEnd() == true )
	{
		StateChange( USI_WAIT );
	}

	CommonEventProcess();
}
//#pragma endregion ���� �긮�� - ��ũ Ȧ

//#pragma region Active Body Skill
void CX2GUArme_VioletMage::ProcessShiningBodySkill()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AEM_SHINING_BODY );
	if( NULL != pSkillTemplet )
	{
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}
}

void CX2GUArme_VioletMage::ProcessShadowBodySkill()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_AVP_SHADOW_BODY );
	if( NULL != pSkillTemplet )
	{
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}
}

void CX2GUArme_VioletMage::ProcessEnergeticBodySkill()
{
	const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADW_ENERGETIC_BODY );
	if( NULL != pSkillTemplet )
	{
		SetBuffFactorToGameUnit( pSkillTemplet, 0 );
	}
}
//#pragma endregion �ٵ� �ø���

//#pragma region SI_A_ABM_VITAL_DRAIN
void CX2GUArme_VioletMage::AVSI_A_ABM_Vital_Drain_Ready_Init()
{
	TextureReadyInBackground( L"aisha_active_guillotinePress_lightBlur.dds" );
}

void CX2GUArme_VioletMage::AVSI_A_ABM_Vital_Drain_Ready_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.47f ) )
	{
		float fDistance = 2000.f;
		m_iDrainEnergyTarget = -1;
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_VITAL_DRAIN );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			float fDistanceTemp = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_PROJECTILE_RANGE_REL, iSkillTempletLevel );

			fDistance = fDistanceTemp;

			if( GetEqippedSkillMemo( CX2SkillTree::SMI_AISHA_MEMO26 ) == true )
				fDistance *= 1.5f;

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
					m_iDrainEnergyTarget = i;
				}
			}
		}

		if( m_iDrainEnergyTarget >= 0 )
		{
			StateChange( AVSI_A_ABM_VITAL_DRAIN_SUCCESS, true );
		}
		else
		{
			StateChange( AVSI_A_ABM_VITAL_DRAIN_FAIL, true );
		}
	}
	else if( m_iDrainEnergyTarget < 0 && m_pXSkinAnim->IsAnimationEnd() == true )
	{
		StateChange( AVSI_A_ABM_VITAL_DRAIN_FAIL, true );
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_A_ABM_Vital_Drain_Success_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.3f ) )
	{
		if( m_iDrainEnergyTarget >= 0 )
		{
			CX2GameUnit *pUnit = g_pX2Game->GetUnit( m_iDrainEnergyTarget );

			if( pUnit != NULL && g_pX2Game->IsValidUnit( pUnit ) == true )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ABM_VITAL_DRAIN );
				if( NULL != pSkillTemplet )
				{
					if ( NULL == GetUnit() )
						return;

					const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;
					const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����
					float fDrainRate = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_INTAKE_ABS, iSkillTempletLevel );

					m_DamageData.optrAttackerGameUnit	= this;
					m_DamageData.impactPoint			= pUnit->GetPos();
					m_DamageData.optrDefenderGameUnit	= pUnit;
					m_DamageData.reActResult			= m_DamageData.reActType;
					m_DamageData.m_fDrainHpRate			= fDrainRate;
					m_DamageData.m_fDrainMpByReact		= pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MP_GAIN_ON_HIT_REL, iSkillTempletLevel );
					m_DamageData.m_fDrainMpRate			= fDrainRate;

					pUnit->DamageReact( &m_DamageData );
					// ������ ����Ʈ
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_EnegryDrain01", this );
					// �ǰ��� ����Ʈ
					g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_Aisha_ABM_EnegryDrain02", pUnit, pUnit );
				}
			}

			m_iDrainEnergyTarget = -1;
		}
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::AVSI_A_ABM_Vital_Drain_EventProcess()
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
	else if ( CAN_WALK_CANCEL && m_pXSkinAnimFuture->GetNowAnimationTime() >= 0.451f )
	{
		StateChange( USI_WALK );
	}

	CommonEventProcess();
}
//#pragma endregion ����Ż �巹��

//#pragma region SI_A_ADW_Worm_Hole
void CX2GUArme_VioletMage::AVSI_A_ADW_Worm_Hole_Init()
{
	XSkinMeshReadyInBackground( L"DummyAttackBox_50x50x50.X" );
	TextureReadyInBackground( L"Arme_Aging_Gate01.tga" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
}

void CX2GUArme_VioletMage::AVSI_A_ADW_Worm_Hole_StateStart()
{
	CommonStateStart();

	g_pX2Game->GetEffectSet()->PlayEffectSet( L"EffectSet_ADW_DISTORTION_WEAPON", this );
}


void CX2GUArme_VioletMage::AVSI_A_ADW_Worm_Hole_FrameMove()
{
    if( m_pXSkinAnim->EventTimerOneshot( 0.4f ) )
	{
		const CX2SkillTree::SkillTemplet* pSkillTemplet = GetEquippedActiveSkillTemplet( CX2SkillTree::SI_A_ADW_WORM_HOLE );
		if( NULL != pSkillTemplet )
		{
			if ( NULL == GetUnit() )
				return;

			const CX2UserSkillTree& userSkillTree = GetUnit()->GetUnitData().m_UserSkillTree;

			const int iSkillTempletLevel = max( 1, userSkillTree.GetSkillLevel( pSkillTemplet->m_eID ) );	/// ��ų ����

			float fEffectTime = (float)pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_EFFECTIVE_TIME, iSkillTempletLevel );

			D3DXVECTOR3 vPos = GetBonePos( L"Bip01_Footsteps" );

			if( true == GetIsRight() )
				vPos += GetDirVector() * 150.f * GetScaleByUnit().x;
			else
				vPos -= GetDirVector() * 150.f * GetScaleByUnit().x;

			bool bHyper = false;
			if ( GetRemainHyperModeTime() > 0.f )
				bHyper = true;
			D3DXVECTOR3 vScale( GetScale(), GetScale(), GetScale() );

			g_pX2Game->GetEffectSet()->PlayEffectSet( L"Effectset_A_ADW_WORM_HOLE", this, NULL, bHyper, GetPowerRate(),
				fEffectTime, vScale, true, vPos );
		}
	}

	CommonFrameMove();
}
//#pragma endregion ��Ȧ

//#pragma region SI_SA_ADW_SCREWDRIVER_DRILLER
void CX2GUArme_VioletMage::AVSI_SI_SA_ADW_Screwdriver_Driller_Init()
{
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_AttackSphere.X" );
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_circle.X" );
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_screw.X" );
	XSkinMeshReadyInBackground( L"elsword_SA_SwordBlastiong_slash.X" );
	XSkinMeshReadyInBackground( L"aisha_SA_SCREWDRIVER_weaponCircle.X" );
	TextureReadyInBackground( L"aisha_SA_SCREWDRIVER_screw.dds" );
	TextureReadyInBackground( L"aisha_SA_SCREWDRIVER_circle.dds" );
	TextureReadyInBackground( L"aisha_SA_SCREWDRIVER_screw.dds" );
	TextureReadyInBackground( L"elsword_SA_SwordBlastiong_slash.dds" );
	TextureReadyInBackground( L"Mesh_Common_AC_Face1_81160_Sorted_P02.dds" );
	TextureReadyInBackground( L"Explosion_Sphere.dds" );
	TextureReadyInBackground( L"WhitePoint.dds" );
	TextureReadyInBackground( L"aisha_SA_SCREWDRIVER_weaponCircle.dds" );
	TextureReadyInBackground( L"soket_earth_quake_smoke02.dds" );
	TextureReadyInBackground( L"stone.dds" );
	TextureReadyInBackground( L"rena_A_fatality_moveSpark.dds" );
}

void CX2GUArme_VioletMage::AVSI_SI_SA_ADW_Screwdriver_Driller_FrameMove()
{
	ChangeWorldColorByHyperMode();
	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", 0.01f, 0 );

	CommonFrameMove();
}
//#pragma endregion ��ũ�� ����̹� - �帱��

//#pragma region SI_SA_AEM_Elemental_Storm
void CX2GUArme_VioletMage::AVSI_HA_AEM_Elemental_Storm_Init()
{
	TextureReadyInBackground(L"AHM_FREEZING_POINT_01.tga");
	TextureReadyInBackground(L"Arme_ring2.dds");
	TextureReadyInBackground(L"Bust_Wolf_P02.dds");
	TextureReadyInBackground(L"CDC_Bullet_01.dds");
	TextureReadyInBackground(L"ColorBallFire.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"EL_FIRE_03.dds");
	TextureReadyInBackground(L"EL_FIRE_05.dds");
	TextureReadyInBackground(L"EL_FIRE_06.dds");
	TextureReadyInBackground(L"EL_FIRE_09.dds");
	XSkinMeshReadyInBackground(L"Elsword_SI_SA_Sonic_Blade_Mesh01.X");
	TextureReadyInBackground(L"Explosion_Sphere.dds");
	TextureReadyInBackground(L"Fire_Flower01.dds");
	TextureReadyInBackground(L"GroundShockWave.dds");
	TextureReadyInBackground(L"GroundShockWave02.dds");
	XSkinMeshReadyInBackground(L"GustStorm01.X");
	XSkinMeshReadyInBackground(L"GustStorm01_1.X");
	XSkinMeshReadyInBackground(L"GustStorm02.X");
	XSkinMeshReadyInBackground(L"GustStorm03.X");
	XSkinMeshReadyInBackground(L"GustStorm04.X");
	XSkinMeshReadyInBackground(L"ICE_Storm02.X");
	XSkinMeshReadyInBackground(L"ICE_Storm03.X");
	XSkinMeshReadyInBackground(L"ICE_Storm04.X");
	TextureReadyInBackground(L"Ice_Crystal_Ring01.dds");
	TextureReadyInBackground(L"Ice_Particle01.dds");
	TextureReadyInBackground(L"LIGHTNING_SHOWER_Emblem.tga");
	TextureReadyInBackground(L"LIGHTNING_SHOWER_Square.tga");
	XSkinMeshReadyInBackground(L"Lightning01.X");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Particle_blur.dds");
	XSkinMeshReadyInBackground(L"SA_AEM_ELEMENTAL_STORM_FIRE01.X");
	XSkinMeshReadyInBackground(L"SA_AEM_ELEMENTAL_STORM_FIRE02.X");
	XSkinMeshReadyInBackground(L"SA_AEM_ELEMENTAL_STORM_ICE.X");
	XSkinMeshReadyInBackground(L"SA_AEM_ELEMENTAL_STORM_ICE_DUMMY.X");
	TextureReadyInBackground(L"SA_EL_MEGA_SLASH_FIRE04.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	XSkinMeshReadyInBackground(L"aisha_active_energySpurt_circle.X");
	TextureReadyInBackground(L"soket_wind_stom_wind01.dds");
	TextureReadyInBackground(L"steam_BP.dds");
}
//#pragma endregion ������Ż ���� : �ñر�

//#pragma region SI_FS_AVP_Abyss_Angor
void CX2GUArme_VioletMage::AVSI_HA_AVP_Abyss_Angor_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Aging_Gate01.tga");
	TextureReadyInBackground(L"Arme_Critical2.dds");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Condense_Light01.dds");
	TextureReadyInBackground(L"Condense_Pulse01.dds");
	TextureReadyInBackground(L"Condense_Pulse02.dds");
	TextureReadyInBackground(L"Explosion_Sphere.dds");
	TextureReadyInBackground(L"Gear_Water.tga");
	TextureReadyInBackground(L"Inspector_State_Shield.tga");
	TextureReadyInBackground(L"Lire_Kick_Impact01.dds");
	TextureReadyInBackground(L"Particle_Blur.dds");
	TextureReadyInBackground(L"Sprriggan_Light_Ball_01.Tga");
	TextureReadyInBackground(L"Steam_Bp.dds");
	TextureReadyInBackground(L"WhiteImage.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"blackImage.dds");
	TextureReadyInBackground(L"secretVelder_Unohound_fire.dds");
	TextureReadyInBackground(L"secretVelder_alchemystBoss_magicAttackE_light.dds");
	TextureReadyInBackground(L"smoke.dds");
	XSkinMeshReadyInBackground(L"Dullahan_Special_Laser_Mesh01.X");
	XSkinMeshReadyInBackground(L"Lire_SI_SA_Gungnir_Mesh05.X");
	XSkinMeshReadyInBackground(L"Mirror_Left_light.X");
	XSkinMeshReadyInBackground(L"NEPHILIM_AttackB_02.X");
	XSkinMeshReadyInBackground(L"SA_AVP_ABYSS_ANGOR_ANGOR.X");
	XSkinMeshReadyInBackground(L"Taranvash_SpecialAttackA_Mesh01.X");
}
//#pragma endregion ��� �Ӱ��� : �ñر�

//#pragma region SI_FS_ADW_Fate_space
void CX2GUArme_VioletMage::AVSI_HA_ADW_Fate_space_Init()
{
	TextureReadyInBackground(L"AeroTornado04.dds");
	TextureReadyInBackground(L"Arme_Ring2.DDS");
	TextureReadyInBackground(L"Arme_Ring2.dds");
	TextureReadyInBackground(L"CenterLight_Gray01.dds");
	TextureReadyInBackground(L"Colorballgray.dds");
	TextureReadyInBackground(L"Explosion_Sphere.dds");
	TextureReadyInBackground(L"Sudden_Buster_P05.dds");
	TextureReadyInBackground(L"WhitePoint.dds");
	TextureReadyInBackground(L"WhitePointSmall.dds");
	TextureReadyInBackground(L"aisha_active_guillotinePress_lightBlur.dds");
	TextureReadyInBackground(L"eve_A_blink_light.DDS");
	TextureReadyInBackground(L"secretVelder_Unohound_fire.dds");
}
//#pragma endregion ����Ʈ �����̽� : �ñر�

/*virtual*/ void CX2GUArme_VioletMage::ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ )
{
	ShowActiveSkillCutInAndLight( L"Dummy1_Rhand", fTimeToShow_, m_iSkillCutInSetSubIndex, bOnlyLight_ );
}


//AVSI_HYPER_MODE
void CX2GUArme_VioletMage::RidingHyperModeInit()
{
	HyperModeInit();
}

void CX2GUArme_VioletMage::RidingHyperModeFrameMove()
{
	CommonHyperModeFrameMove( 0.f, 0.1f, true );
}

void CX2GUArme_VioletMage::CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ /*= false*/  )
{
	g_pX2Game->GetWorld()->SetWorldColor( 0xff222222 );

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

		if( true == bSound_ )
		{
			PlaySound( L"Energy.ogg" );
			PlaySound( L"Break.ogg" );
			PlaySound( L"Change.ogg" );
		}

		UpDownCrashCamera( 20.0f, 0.3f );
		g_pKTDXApp->GetDGManager()->ClearScreen();

		ShowMinorParticleHyperModeTrace();
		ApplyHyperModeBuff();

		//{{ ����� 2010.10.29 : ������ ���� ���
		m_fRateLimitManaManagement = GetLastStateHyperModeCount() * 0.33f;
		float rdf = GetRandomFloat();
		if ( rdf <= m_fRateLimitManaManagement )
		{
			int iSkillLevel = GetUnit()->GetUnitData().m_UserSkillTree.GetSkillLevel( CX2SkillTree::SI_P_AVP_LIMITED_MANA_MANAGEMENT, true  );

			if( iSkillLevel > 0 )
			{
				const CX2SkillTree::SkillTemplet* pSkillTemplet = g_pData->GetSkillTree()->GetSkillTemplet( CX2SkillTree::SI_P_AVP_LIMITED_MANA_MANAGEMENT );

				if( NULL != pSkillTemplet )
				{
					m_iHyperChargeMP = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_RECOVER_MP_ABS, iSkillLevel );
					m_iHyperAddMP = pSkillTemplet->GetSkillAbilityValue( CX2SkillTree::SA_MAX_MP_ABS, iSkillLevel );

					SetBuffFactorToGameUnit( pSkillTemplet, 0 );
					//GetGageManager()->UpdateMyManaGuageUI();
					UpNowMp( m_iHyperChargeMP );



					std::wstringstream strStream;
					std::wstringstream strStream2;
					strStream << L"MAXMP+ "<< static_cast<int>(m_iHyperAddMP);
					D3DXVECTOR3 pos = GetPos();
					pos.y += 50.0f;
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( strStream.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );


					pos.y += 30.0f;
					strStream2 <<  "MP+ " << static_cast< int >( m_iHyperChargeMP );
					if( NULL != g_pData->GetPicCharBlue() )
						g_pData->GetPicCharBlue()->DrawText( strStream2.str().c_str(), pos, GetDirVector(), CKTDGPicChar::AT_CENTER );
				}
			}
		}
		//}} ����� 2010.10.29 : ������ ���� ���
	}

	CommonFrameMove();
}

void CX2GUArme_VioletMage::RidingHyperModeCameraMove()
{
	HyperModeCameraMove();
}

/** @function : IsValideSkillRidingOn
	@brief : Ż �� ž���߿� ��� �� �� �ִ� ��ų���� üũ.
	@return : ��� ���� : true ��� �Ұ� falle
*/
/*virtual*/ bool CX2GUArme_VioletMage::IsValideSkillRidingOn( CX2SkillTree::SKILL_ID eSkill_ID_ )
{
	if( true == GetRidingOn() )
	{
		switch( eSkill_ID_ )
		{
		case CX2SkillTree::SI_A_AV_MANA_SHIELD: // ���� �ǵ�
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

CX2SkillTree::ACTIVE_SKILL_USE_CONDITION CX2GUArme_VioletMage::GetSkillUseCondition(const CX2SkillTree::SkillTemplet* pSkillTemplet_)
{
	CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = pSkillTemplet_->m_eActiveSkillUseCondtion;

	switch (pSkillTemplet_->m_eID)
	{
	case CX2SkillTree::SI_SA_AHM_CHAIN_LIGHTNING:
		{
			if( GetEqippedSkillMemo(CX2SkillTree::SMI_AISHA_MEMO17) == true )
				eActiveSkillUseCondition = CX2SkillTree::ASUT_AIR;
		} break;
	default:
		break;
	}


	return eActiveSkillUseCondition;
}
