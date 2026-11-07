#pragma once

class CX2GUArme_VioletMage : public CX2GUUser
{
	public:
		enum ARME_VIOLEMAGE_STATE_ID
		{
			AVSI_BASE = USI_END + 1,

			AVSI_DASH_JUMP_POWER_LANDING,

			AVSI_JUMP_LEVITATION,
			AVSI_DASH_JUMP_LEVITATION,

			AVSI_CHARGE_MP,

			AVSI_EVASION_START,
			AVSI_TELEPORT_END,
			AVSI_TELEPORT_STAND_UP_START,
			AVSI_TELEPORT_STAND_UP_END,
			AVSI_TELEPORT_STANDUP_ATTACK,

			AVSI_COMBO_Z1,
			AVSI_COMBO_Z2,
			AVSI_COMBO_Z3,
			AVSI_COMBO_Z3up,			// ZZupZ
			AVSI_COMBO_Z4up,			// ZZupZZ

			AVSI_COMBO_X1,
			AVSI_COMBO_X2,
			AVSI_COMBO_X3,

			AVSI_COMBO_X3front,			// XXfrontX
			AVSI_COMBO_X3down,
			AVSI_COMBO_X4down,
			AVSI_COMBO_X3back,

			AVSI_DASH_COMBO_Z1,
			AVSI_DASH_COMBO_Z2,
			AVSI_DASH_COMBO_Z2a,
			AVSI_DASH_COMBO_Z3up,
			AVSI_DASH_ATTACK_X,
			AVSI_DASH_ATTACK_X2,

			AVSI_JUMP_ATTACK_Z,
			AVSI_JUMP_ATTACK_X,

			AVSI_DASH_JUMP_ATTACK_Z,
			AVSI_DASH_JUMP_ATTACK_X,
			AVSI_DASH_JUMP_ATTACK_X2,

//#ifdef WALL_JUMP_TEST
			AVSI_WALL_LANDING,
//#endif WALL_JUMP_TEST

			AVSI_SI_A_AV_CIRCLE_FLAME,

			// 2�� ����
			AVSI_HELL_DROP_CONTROL_IDLE,
			AVSI_HELL_DROP_CONTROL_LEFT,
			AVSI_HELL_DROP_CONTROL_RIGHT,
			AVSI_HELL_DROP_CONTROL_UP,
			AVSI_HELL_DROP_CONTROL_DOWN,

			AVSI_HELL_DROP_CONTROL_UP_LEFT,
			AVSI_HELL_DROP_CONTROL_UP_RIGHT,
			AVSI_HELL_DROP_CONTROL_DOWN_LEFT,
			AVSI_HELL_DROP_CONTROL_DOWN_RIGHT,

			AVSI_HELL_DROP_CONTROL_FIRE,

			// ���� Fire ���� ��� -> ���� �ð� Ȥ�� ���ǿ� ���� ��� ���濡 ���� State �߰�
			AVSI_HELL_DROP_CONTROL_DYING,


			AVSI_AEM_XZ,
			AVSI_AEM_XZZ,

			AVSI_AVP_ZZfrontZ,
			AVSI_AVP_ZZfrontZZ,
			AVSI_AVP_ZZfrontZZ_FALL,


			AVSI_COMBO_Z4,

			AVSI_ABM_DASH_COMBO_Z2a,
			ABM_DASH_COMBO_ZXX,
			ABM_DASH_COMBO_ZXXX,

			AVSI_A_ABM_ENERGY_DRAIN_SUCCESS,
			AVSI_A_ABM_ENERGY_DRAIN_FAIL,

			AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP,
			AVSI_SI_SA_ABM_GUILLOTINE_PRESS_END,

			AVSI_COMBO_X4,

			AVSI_ADW_COMBO_ADD_X,
			AVSI_ADW_COMBO_ADD_X2,

			AVSI_COMBO_X4_CHARGE,					/// 4��° ���̾ ���� ������Ʈ �߰�
			AVSI_COMBO_X4_CHARGE_FIRE,				/// 4��° ���̾ ���� �߻� ������Ʈ �߰�
			AVSI_AEM_XZZ_END,						/// XZZ �޺� ���� ������Ʈ
			AVSI_SA_AHM_METEOSHOWER_LOOP,			/// ���׿� ���� ���� ������Ʈ�߰�
			AVSI_SA_AHM_METEOSHOWER_HYPER_LOOP,		/// ���� ���׿� ���� ���� ������Ʈ�߰�
			AVSI_SA_AHM_METEOSHOWER_END,			/// ���׿� ���� ���� ������Ʈ
			AVSI_EM_DASH_COMBO_Z2a,					/// ���� ������ ��� �޺� ZX

			AVSI_SA_AHM_LIGHTNING_SHOWER_LOOP,			/// ����Ʈ�� ���� ���� ������Ʈ�߰�
			AVSI_SA_AHM_LIGHTNING_SHOWER_HYPER_LOOP,	/// ���� ����Ʈ�� ���� ���� ������Ʈ�߰�
			AVSI_SA_AHM_LIGHTNING_SHOWER_END,			/// ����Ʈ�� ���� ���� ������Ʈ

			AVSI_A_ABM_VITAL_DRAIN_SUCCESS,	/// ����Ż �巹�� ����
			AVSI_A_ABM_VITAL_DRAIN_FAIL,	/// ����Ż �巹�� ����

			//////////////////////////////////////////////////////////////////////////
			// ���ʿ� �߰����ּ���~ �� �Ʒ��� �׽�Ʈ�� �߰��� enum �Դϴ�


			AVSI_TRANSFORMED,

//#ifdef PVP_BOSS_COMBAT_TEST
//			AVSI_FROZEN,
//#endif PVP_BOSS_COMBAT_TEST
		};

		enum AISHA_MAJOR_PARTICLE_INSTANCE_ID
		{
			AISHA_PII_MAJOR_AIR_SHOCK_RING,
			AISHA_PII_MAJOR_AIR_SHOCK_CENTER,
			AISHA_PII_MAJOR_AIR_BLAST_CENTER,

			//{{ ����Ʈ�׺�Ʈ, ���۳ο��̺꿡�� ���̴� ��ƼŬ
			// �� ��ų�� EFFECT_SET���� �����ϸ� �̺κ��� ���� �ص� �ɵ�

			AISHA_PII_MAJOR_LIGHTNING_BOLT_01,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_02,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_03,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_04,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_05,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_06,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_07,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_08,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_09,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_10,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_11,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_12,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_CORE,

			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_01,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_02,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_03,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_04,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_05,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_06,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_07,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_08,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_09,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_10,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_11,
			AISHA_PII_MAJOR_LIGHTNING_BOLT_RED_12,
			//}} ����Ʈ�׺�Ʈ, ���۳ο��̺꿡�� ���̴� ��ƼŬ

			AISHA_PII_MAJOR_END,
		};

		enum AISHA_MINOR_PARTICLE_INSTANCE_ID
		{
			AISHA_PII_MINOR_AIR_BLAST_RING,
			AISHA_PII_MINOR_TIME_STOP_01,
			AISHA_PII_MINOR_TIME_STOP_02,
			AISHA_PII_MINOR_TIME_STOP_03,
			AISHA_PII_MINOR_TIME_STOP_RED,

			AISHA_PII_MINOR_END,
		};

		enum EffectSetInstanceID
		{
			EffSetID_Elemental_Friendship,
			EffSetID_Wing,
			EffSetID_END,
		};



		struct StrongMindData : public TimeLimited
		{
			float m_fSpecialActiveSkillRate;
			float m_fActiveSkillRate;
			float m_fMPRestoreRate;

			StrongMindData()
			: m_fSpecialActiveSkillRate( 0.f )
			, m_fActiveSkillRate( 0.f )
			, m_fMPRestoreRate( 0.f )
			{
			}
		};


		struct StoneWallData : public TimeLimited
		{
			UidType m_NPCUID;

			StoneWallData()
				: m_NPCUID( -1 )
			{
			}

		};

		//struct PigBatData : public TimeLimited
		//{
		//	UidType m_NPCUID;
		//	bool m_bCatched;	// �������� ������

		//	PigBatData()
		//		: m_NPCUID( -1 )
		//		, m_bCatched( false )
		//	{
		//	}

		//};

	private:
		CX2GUArme_VioletMage( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit );
		virtual ~CX2GUArme_VioletMage(void);


	public:
		static CX2GUArme_VioletMage* CreateGUArme( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit )
		{
			return new CX2GUArme_VioletMage( unitIndex, teamNum,
				pScriptLuaState,
				frameBufferNum, pUnit );
		};

		static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
		static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
		static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );


		virtual void		ReInit( bool bRandom = false, int startPosIndex = -1 );

		virtual void		DamageReact( CX2DamageManager::DamageData* pDamageData );

		virtual void		Win();
		virtual void		Lose();
		virtual void		Draw();

		virtual void		InitEffect();
		virtual void		InitEnchantWeaponEffectUnitSpecific();

		void				InitializeArmeMajorParticleArray();	// ������ ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		virtual void		CreateMajorParticleArray();		// ������ ��ƼŬ �迭�� ���� // kimhc // 2010.11.5
		static void			AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
		CKTDGParticleSystem::CParticleEventSequence* SetAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_ );	// �� ĳ���͵鸸 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		CKTDGParticleSystem::CParticleEventSequence* SetAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ );	// �� ĳ���͵鸸 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		ParticleEventSequenceHandle	GetHandleAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const // ĳ���͸� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		{
			ASSERT( AISHA_PII_MAJOR_END > eVal_ && AISHA_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahAishaMajorParticleInstance[eVal_];
		}

		ParticleEventSequenceHandle& GetHandleReferenceAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) // ĳ���͸� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		{
			ASSERT( AISHA_PII_MAJOR_END > eVal_ && AISHA_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahAishaMajorParticleInstance[eVal_];
		}

		void				SetHandleAishaMajorParticleByEnum( AISHA_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ ) // ���ҵ常 ���� ������ ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������ // kimhc // 2010.11.5
		{
			ASSERT( AISHA_PII_MAJOR_END > eVal_ && AISHA_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			m_ahAishaMajorParticleInstance[eVal_] = hHandle_;
		}
		void				DeleteArmeMajorParticle();


		void				InitializeArmeMinorParticleArray();	// ���������� ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		virtual void		CreateMinorParticleArray();		// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		static void			AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
		CKTDGParticleSystem::CParticleEventSequence* SetAishaMinorParticleByEnum( AISHA_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ );	// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5

		ParticleEventSequenceHandle	GetHandleAishaMinorParticleByEnum( AISHA_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const	// ĳ���͸� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
		{
			ASSERT( AISHA_PII_MINOR_END > eVal_ && AISHA_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahAishaMinorParticleInstance[eVal_];
		}

		ParticleEventSequenceHandle& GetHandleReferenceAishaMinorParticleByEnum( AISHA_MINOR_PARTICLE_INSTANCE_ID eVal_ ) // ĳ���͸� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
		{
			ASSERT( AISHA_PII_MINOR_END > eVal_ && AISHA_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahAishaMinorParticleInstance[eVal_];
		}

		void				SetHandleAishaMinorParticleByEnum( AISHA_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )	// ĳ���͸� ���� ���̳� ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������	// kimhc // 2010.11.5
		{
			ASSERT( AISHA_PII_MINOR_END > eVal_ && AISHA_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			m_ahAishaMinorParticleInstance[eVal_] = hHandle_;
		}
		void				DeleteArmeMinorParticle();

		virtual void		DeleteMinorParticle();

		void StartMeditation( bool bStartMeditation );

		virtual bool		SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet );
		virtual	float		GetAdditionalAccelBuffTime( const CX2SkillTree::SKILL_ID eSkillID_ ) const;
		virtual void		ApplyAuraAcceleratorBuffToGameUnit( const CX2SkillTree::SkillTemplet* pSkillTemplet_, const float fAddtionalAccelBuffTime_, const D3DXVECTOR3& vMyPos_ );
		CX2BuffFactorPtr	GetEnergyOfThePlentyBuffFactorClonePtr( const CX2SkillTree::SKILL_ID eSkillIdAccelerator_ ) const;




		void AddStoneWall( UidType uid, float fTimeLeft );
		//void AddPigBat( UidType uid, float fTimeLeft );

		CX2GUNPC* GetUFO() const { return ( null != m_optrUFOGameUnit ? static_cast<CX2GUNPC*>( m_optrUFOGameUnit.GetObservable() ) : NULL ); }
		void SetUFO( CX2GUNPC* val)
		{
			if ( NULL != val )
				m_optrUFOGameUnit = val;
		}
		void ResetUFO() { m_optrUFOGameUnit.Reset(); }

		bool IsInMeditation() const { return m_bInMeditation; }

		float GetMeditationMPChangeRate() const
		{
			return m_fMeditationMPChangeRate;
		}

		float GetAddMPChargeRate() const
		{
			return m_fAddMPChangeRate;
		}

		virtual void		HyperModeBuffEffectStart();

		/// �� ���ָ��� Ư�� ������ ���� �Ǵ� ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ��...)
		virtual void SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
		virtual void UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
		virtual bool CanRidingState();

	protected:

		void				InitComponent();

		virtual void ParseCommonRandomState();

		virtual void		InitState();
		void InitStateID();
		void InitStateCommon();


		void				InitDevice();
		virtual	void		InitEtc();

		virtual void		InitEquippedActiveSkillState( bool bOnlySkillLevel = false );

		virtual void		InitPassiveSkillState();
		virtual void		SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ );
		virtual void		SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ );




		virtual void		CommonStateStartFuture();
		virtual void		CommonStateStart();
		virtual void		CommonFrameMoveFuture();
		virtual void		CommonFrameMove();
		virtual void		CommonStateEndFuture();
		virtual void		CommonStateEnd();

//{{ robobeg : 2008-10-28
		//virtual void		CommonRender();
        virtual RENDER_HINT CommonRender_Prepare();
//}} robobeg : 2008-10-28

		virtual bool		SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = CX2SkillTree::ASUT_GROUND );
		virtual void		NoStateChangeActionFrameMove();

		virtual void		ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ );

		virtual float		GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;


		CX2SkillTree::ACTIVE_SKILL_USE_CONDITION GetSkillUseCondition(const CX2SkillTree::SkillTemplet* pSkillTemplet_);

		void				SetSelfDestructSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID = CX2UnitManager::NUI_NONE );
		void				AdaptAdvancedTeleportationBuff();	/// �������� ���� ����

		virtual	void				SetDamageExceptionProcess( CX2DamageManager::DamageData* pDamageData_ ,OUT float& fPowerRate);

		bool													m_bZ4up;
		bool													m_bDashJumpZ;
		D3DXVECTOR3												m_bDashJumpZPos;
		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqArmeAirStepRight;
		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqArmeAirStepLeft;

		float m_fMPConsumeRateTeleport;		/// �ڷ���Ʈ�ÿ� ���� �Ҹ�
		float m_fMPConsumeRateFireBall;
		float m_fFireBallRangeRate;			/// ���̾�� ���ư��� �Ÿ� ����

		bool m_bInMeditation;				/// SI_A_AHM_MEDITATION ��ų�� ��������� �ƴ���
		float m_fMeditationMPChangeRate;
		float m_fAddMPChangeRate;			/// �������� �߰��Ǵ� ������.

		CX2EffectSet::Handle m_ahEffectSet[ EffSetID_END ];


//{{ ����� 2010.10.29

		//bool m_bLimitManaManagement; //Ȯ�������� �нú� ��ų�� �ߵ��ߴ���.
		float m_iHyperAddMP; // �нú� ��ų ������ ���� ���߰���
		float m_iHyperChargeMP; // �нú� ��ų ������ ���� �� ȸ����
		float m_fRateLimitManaManagement; // ������ ���߰�/ȸ�� �ߵ� Ȯ�� ( 0.33% * ���������� �� )

		bool m_bIsOnSomethingStart; // �����߿� ����ϴ� ��ų�� �����߿� �����, ���θʿ� ���� �����.
//}} ����� 2010.10.29

		CKTDXTimer m_TimerCheckInvalidUFO;
		CX2GameUnitoPtr m_optrUFOGameUnit;
		bool m_bIsSummoningUFO;
		CKTDXTimer m_TimerUFOControl;

		bool	m_bIceBall;					/// ���� ����
		float	m_fIceBallPower;			/// ���� ���� ���ݷ� ���� ��
		float	m_fElementalResearchRate;	/// ������ ������ ���ҵǴ� ��Ÿ�� ��ġ
		bool	m_bResearchGravityBall;		/// �׶��Ƽ �� ����
		float	m_fGravityBallPower;		/// �׶��Ƽ �� ������ ���� ��


        /** �Ʒ��� CKTDGXSkinAnim*Ÿ���� ������� ���������� �����ϴ� ��ü�̹Ƿ�
            smart pointer�� ������� �ʴ´�.
            - jintaeks on 2009-01-12, 16:25 */
		CKTDGXSkinAnim*											m_pMPChargeA;
		CKTDGXSkinAnim*											m_pMPChargeB;
		CKTDGXSkinAnim*											m_pMPChargeC;

		CKTDGXSkinAnim*											m_pEvasionAttackAnim;

		CKTDGXSkinAnim*											m_pComboX3Up1;
		CKTDGXSkinAnim*											m_pComboX3Up2;
		CKTDGXSkinAnim*											m_pComboX3Up3;

		//CKTDGXSkinAnim*											m_pMeteoCall;		// ������� �ʴ� ����

		float													m_fMPChargeWaitTime;
		float													m_fMPChargeSoundTime;

		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqMPEnergy;
		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqMPEnergyCenter;
		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqMPChargeWave;

		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqMPCharge2MagicCircle;
		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqMPCharge2Up;
		CKTDGParticleSystem::CParticleEventSequenceHandle 		m_hSeqMPCharge2Up2;

		CX2DamageEffect::CEffectHandle								    m_hHellStoneEffect;
		CX2DamageEffect::CEffectHandle								    m_hCatastropheLaser;
		CX2DamageEffect::CEffectHandle								    m_hCatastropheLaser1;
		CX2DamageEffect::CEffectHandle								    m_hCatastropheLaser2;
		CX2DamageEffect::CEffectHandle								    m_hCatastropheLaser3;
		CX2DamageEffect::CEffectHandle								    m_hCatastropheLaser4;




		//Major
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqLight_Arme_Aging1;
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqLight_Arme_Aging2;
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqLight_Arme_Aging3;

		//minor
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqLight_Arme_Circle_Flame;

		bool		m_bElementalFriendShip;
		StrongMindData* m_pStrongMindData;
		std::vector< StoneWallData > m_vecStoneWallData;
		//std::vector< PigBatData > m_vecPigBatData;

		int m_iDrainEnergyTarget;

		CX2EqipPtr	m_ptrTransformWeapon;
		float m_fTransformTime;


		CX2EffectSet::Handle		m_hEffectMorningStar;
		float						m_fMagicStaffExtraRate;
		float						m_fMagicStaffSize;
		CKTDGXMeshPlayer::CXMeshInstanceHandle		m_hMagicalStaff;

		float						m_fLevitationElapsedTime;

		bool						m_bCanChangeState;			/// Ư�� ������Ʈ�� �Ѿ �� �ִ��� �����ϴ� ����
		float						m_fPushXChargeTime;			/// 4��° ���̾ ���� �ð�
		int							m_iMaxFireCount;			/// ���׿� ���� ������Ʈ���� �ִ� �߻��� �� �ִ� ���� ����
		int							m_iMaxFireCountByLoopState;	/// ���׿� ���� ���� ������Ʈ���� �ִ� �߻��� �� �ִ� ���� ����
		int							m_iElapsedFireCount;		/// ���׿� ���� ���� ������Ʈ���� ������ �߻緮 ����
		float						m_fUseMPConsume;			/// ���׿� ���� ���� ������Ʈ���� �ѹߴ� �Ҹ�Ǵ� MP��
		float						m_fFireGap;					/// ���׿� ���� �ѹߴ� �߻�Ǵ� ����


		float							m_fHellDropCreateEffectCooltime;	/// �� ���, ������ Effect ��Ÿ��
		bool							m_bIsHellDropTimerStart;			/// �� ���, Ÿ�̸Ӱ� ���۵Ǿ��°�?
	private:

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahAishaMajorParticleInstance[AISHA_PII_MAJOR_END];
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahAishaMinorParticleInstance[AISHA_PII_MINOR_END];

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		//							������Ʈ �Լ� ����

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////
	protected:
		//AVSI_DIE_FRONT
		void DieFrontStartFuture();
		void DieFrontStart();
		void DieFrontFrameMove();
		void DieFrontEventProcess();

		//AVSI_DIE_BACK
		void DieBackStartFuture();
		void DieBackStart();
		void DieBackFrameMove();
		void DieBackEventProcess();

		//AVSI_WAIT
		void WaitStartFuture();
		void WaitStart();
		void WaitFrameMoveFuture();
		void WaitEventProcess();

		//AVSI_WALK
		void WalkEventProcess();

		//AVSI_JUMP_READY
		void JumpReadyEventProcess();

		//AVSI_JUMP_UP
		void JumpUpEventProcess();

		//AVSI_JUMP_DOWN
		void JumpDownEventProcess();

		//AVSI_JUMP_LEVITATION
		void JumpLevitationStartFuture();
		void JumpLevitationFrameMoveFuture();
		void JumpLevitationFrameMove();
		void JumpLevitationEventProcess();
		void JumpLevitationEnd();

		//AVSI_JUMP_LANDING
		void JumpLandingEventProcess();

		//AVSI_DASH
		void DashEventProcess();

		//AVSI_DASH_END
		void DashEndStart();
		void DashEndFrameMoveFuture();
		void DashEndFrameMove();
		void DashEndEventProcess();

		//AVSI_DASH_JUMP
		void DashJumpStartFuture();
		void DashJumpStart();
		void DashJumpFrameMoveFuture();
		void DashJumpEventProcess();
		void DashJumpEndFuture();

		//AVSI_DASH_JUMP_LEVITATION
		void DashJumpLevitationStartFuture();
		void DashJumpLevitationFrameMoveFuture();
		void DashJumpLevitationFrameMove();
		void DashJumpLevitationEventProcess();
		void DashJumpLevitationEnd();

		//AVSI_DASH_JUMP_LANDING
		void DashJumpLandingEventProcess();

		//AVSI_DASH_JUMP_POWER_LANDING
		void DashJumpPowerLandingStart();
		void DashJumpPowerLandingEventProcess();

		//AVSI_HYPER_MODE
		void HyperModeInit();
		void HyperModeFrameMove();
		void HyperModeCameraMove();

		//AVSI_HYPER_MODE
		void RidingHyperModeInit();
		void RidingHyperModeFrameMove();
		void RidingHyperModeCameraMove();
		void CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ = false );

		//AVSI_CHARGE_MP
		void ChargeMPInit();
		void ChargeMPStartFuture();
		void ChargeMPStart();
		void ChargeMPFrameMove();
		void ChargeMPEventProcess();
		void ChargeMPEndFuture();
		void ChargeMPEnd();

		////AVSI_DAMAGE_GROGGY
		//void AVSI_DAMAGE_GROGGY_FrameMoveFuture();
		//void AVSI_DAMAGE_GROGGY_EventProcess();

		//AVSI_DAMAGE_SMALL_FRONT
		void DamageSmallFrontEventProcess();

		//AVSI_DAMAGE_SMALL_BACK
		void DamageSmallBackEventProcess();

		//AVSI_DAMAGE_BIG_FRONT
		void DamageBigFrontStart();
		void DamageBigFrontEventProcess();

		//AVSI_DAMAGE_BIG_BACK
		void DamageBigBackStart();
		void DamageBigBackEventProcess();

		//AVSI_DAMAGE_DOWN_FRONT
		void DamageDownFrontFrameMove();
		void DamageDownFrontEventProcess();

		//AVSI_DAMAGE_DOWN_BACK
		void DamageDownBackFrameMove();
		void DamageDownBackEventProcess();

		//AVSI_DAMAGE_AIR_DOWN_LANDING
		void DamageAirDownLandingInit();
		void DamageAirDownLandingEventProcess();

		//AVSI_EVASION_START
		void AVSI_A_AV_Teleport_StateStart();
		void AVSI_A_AV_Teleport_FrameMoveFuture();
		void AVSI_A_AV_Teleport_EventProcess();

		//AVSI_EVASION_END
		void EvasionEndFrameMove();
		void EvasionEndEventProcess();

		//AVSI_EVASION_STAND_UP_START
		void AVSI_EVASION_STAND_UP_START_Start();
		void AVSI_EVASION_STAND_UP_START_FrameMoveFuture();
		void AVSI_EVASION_STAND_UP_START_EventProcess();

		//AVSI_EVASION_STAND_UP_END
		void AVSI_EVASION_STAND_UP_END_FrameMove();
		void AVSI_EVASION_STAND_UP_END_EventProcess();
		void AVSI_EVASION_STAND_UP_END_End();


		//AVSI_EVASION_STANDUP_ATTACK
		void EvasionStandupAttackInit();
		void EvasionStandupAttackStart();
		void EvasionStandupAttackFrameMove();
		void EvasionStandupAttackEventProcess();
		void EvasionStandupAttackEnd();

		//AVSI_COMBO_Z1
		void ComboZ1FrameMove();
		void ComboZ1EventProcess();

		//AVSI_COMBO_Z2
		//void ComboZ2FrameMove();
		void ComboZ2EventProcess();

		//AVSI_COMBO_Z3
		void ComboZ3EventProcess();

		//AVSI_COMBO_Z3up
		void ComboZ3upEventProcess();

		//AVSI_COMBO_Z4up
		void ComboZ4upStart();
		void ComboZ4upFrameMoveFuture();
		void ComboZ4upFrameMove();
		void ComboZ4upEventProcess();

		//AVSI_COMBO_X1
		void ComboX1FrameMoveFuture();
		void ComboX1FrameMove();
		void ComboX1EventProcess();

		//AVSI_COMBO_X2
		void ComboX2FrameMove();
		void ComboX2EventProcess();

		//AVSI_COMBO_X3
		void ComboX3FrameMove();
		void ComboX3EventProcess();
		void ComboX3StateEnd();

		//AVSI_COMBO_X4_CHARGE
		void ComboX4ChargeStateStart();
		void ComboX4ChargeFrameMove();
		void ComboX4ChargeEventProcess();
		void ComboX4ChargeStateEnd();

		//AVSI_COMBO_X4_CHARGE_FIRE
		void ComboX4ChargeFireFrameMove();
		void ComboX4ChargeFireEventProcess();

		//AVSI_COMBO_X4
		void ComboX4FrameMove();
		void ComboX4EventProcess();

		//AVSI_COMBO_X3front
		void ComboX3frontStart();
		void ComboX3frontFrameMoveFuture();
		void ComboX3frontFrameMove();
		void ComboX3frontEventProcess();

		//AVSI_COMBO_X3down
		void ComboX3downFrameMove();
		void ComboX3downEventProcess();

		//AVSI_HIGH_MAGICIAN_COMBO_X3down
		void AVSI_HIGH_MAGICIAN_COMBO_X3down_FrameMove();
		void AVSI_HIGH_MAGICIAN_COMBO_X3down_EventProcess();

		//AVSI_HIGH_MAGICIAN_COMBO_X4down
		void AVSI_HIGH_MAGICIAN_COMBO_X4down_FrameMove();
		void AVSI_HIGH_MAGICIAN_COMBO_X4down_EventProcess();

		//AVSI_DARK_MAGICIAN_COMBO_X3down
		void AVSI_DARK_MAGICIAN_COMBO_X3down_FrameMove();
		void AVSI_DARK_MAGICIAN_COMBO_X3down_EventProcess();

		//AVSI_DARK_MAGICIAN_COMBO_X4down(Z �޺���)
		void AVSI_DARK_MAGICIAN_COMBO_X4down_Start();
		void AVSI_DARK_MAGICIAN_COMBO_X4down_FrameMove();
		void AVSI_DARK_MAGICIAN_COMBO_X4down_EventProcess();

		//AVSI_HIGH_MAGICIAN_COMBO_X2
		void AVSI_HIGH_MAGICIAN_COMBO_X2_FrameMove();
		void AVSI_HIGH_MAGICIAN_COMBO_X2_EventProcess();

		//AVSI_HIGH_MAGICIAN_COMBO_X3back
		void AVSI_HIGH_MAGICIAN_COMBO_X3back_FrameMove();
		void AVSI_HIGH_MAGICIAN_COMBO_X3back_EventProcess();
		void AVSI_HIGH_MAGICIAN_COMBO_X3back_EndFuture();

		//AVSI_DARK_MAGICIAN_COMBO_X2
		void AVSI_DARK_MAGICIAN_COMBO_X2_FrameMove();
		void AVSI_DARK_MAGICIAN_COMBO_X2_EventProcess();

		//AVSI_DARK_MAGICIAN_COMBO_X3back
		void AVSI_DARK_MAGICIAN_COMBO_X3back_FrameMove();
		void AVSI_DARK_MAGICIAN_COMBO_X3back_EventProcess();

		//AVSI_DASH_COMBO_Z1
		void DashComboZ1StartFuture();
		void DashComboZ1FrameMove();
		void DashComboZ1EventProcess();

		//AVSI_DASH_COMBO_Z2
		void DashComboZ2StartFuture();
		//void DashComboZ2FrameMove();
		void DashComboZ2EventProcess();

		//AVSI_DASH_COMBO_Z2a
		void DashComboZ2aStartFuture();
		void DashComboZ2aFrameMove();
		void DashComboZ2aEventProcess();

		//AVSI_EM_DASH_COMBO_Z2a
		void AVSI_EM_DASH_COMBO_Z2a_StartFuture();
		void AVSI_EM_DASH_COMBO_Z2a_FrameMove();
		void AVSI_EM_DASH_COMBO_Z2a_EventProcess();

		//AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a
		void AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_Start();
		void AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_StartFuture();
		void AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_FrameMove();
		void AVSI_DARK_MAGICIAN_DASH_COMBO_Z2a_EventProcess();

		//AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up
		void AVSI_DARK_MAGICIAN_DASH_COMBO_Z3up_EventProcess();

		//AVSI_DASH_ATTACK_X
		void DashAttackXStartFuture();
		void DashAttackXFrameMove();
		void DashAttackXEventProcess();
		void DashAttackXEndFuture();

		//AVSI_HIGH_MAGICION_DASH_ATTACK_X
		void AVSI_HIGH_MAGICION_DASH_ATTACK_X_StartFuture();
		void AVSI_HIGH_MAGICION_DASH_ATTACK_X_FrameMove();
		void AVSI_HIGH_MAGICION_DASH_ATTACK_X_EventProcess();
		void AVSI_HIGH_MAGICION_DASH_ATTACK_X_EndFuture();

		//AVSI_HIGH_MAGICION_DASH_ATTACK_X2
		void AVSI_HIGH_MAGICION_DASH_ATTACK_X2_FrameMove();
		void AVSI_HIGH_MAGICION_DASH_ATTACK_X2_EventProcess();

		//AVSI_JUMP_ATTACK_Z
		void JumpAttackZFrameMoveFuture();
		void JumpAttackZFrameMove();
		void JumpAttackZEventProcess();

		//AVSI_JUMP_ATTACK_X
		void JumpAttackXFrameMoveFuture();
		void JumpAttackXFrameMove();
		void JumpAttackXEventProcess();

		//AVSI_DASH_JUMP_ATTACK_Z
		void DashJumpAttackZFrameMoveFuture();
		void DashJumpAttackZFrameMove();
		void DashJumpAttackZEventProcess();

		//AVSI_DASH_JUMP_ATTACK_X
		void DashJumpAttackXFrameMoveFuture();
		void DashJumpAttackXFrameMove();
		void DashJumpAttackXEventProcess();

		//AVSI_DASH_JUMP_ATTACK_X2
		void DashJumpAttackX2FrameMoveFuture();
		void DashJumpAttackX2FrameMove();
		void DashJumpAttackX2EventProcess();



		////////////////////////////////////////////////////////////////////////
		//AVSI_SI_AV_POWER_ATTACK
		void AVSI_SI_AV_POWER_ATTACK_FrameMove();
		void AVSI_SI_AV_POWER_ATTACK_EventProcess();

		//AVSI_SI_AV_POWER_ATTACK_HYPER
		void AVSI_SI_AV_POWER_ATTACK_HYPER_FrameMove();
		void AVSI_SI_AV_POWER_ATTACK_HYPER_EventProcess();

		//AVSI_SI1_AV_BINDING_CIRCLE
		void AVSI_SI1_AV_BINDING_CIRCLE_FrameMove();
		void AVSI_SI1_AV_BINDING_CIRCLE_CameraMove();
		void AVSI_SI1_AV_BINDING_CIRCLE_EventProcess();
		void AVSI_SI1_AV_BINDING_CIRCLE_End();

		//AVSI_SI1_AV_BINDING_CIRCLE_LV1_HYPER
		void AVSI_SI1_AV_BINDING_CIRCLE_HYPER_FrameMove();
		void AVSI_SI1_AV_BINDING_CIRCLE_HYPER_CameraMove();
		void AVSI_SI1_AV_BINDING_CIRCLE_HYPER_EventProcess();
		void AVSI_SI1_AV_BINDING_CIRCLE_HYPER_End();

		//AVSI_SI2_AV_LIGHTNING_BOLT
		void AVSI_SI2_AV_LIGHTNING_BOLT_FrameMove();
		void AVSI_SI2_AV_LIGHTNING_BOLT_CameraMove();
		void AVSI_SI2_AV_LIGHTNING_BOLT_EventProcess();
		void AVSI_SI2_AV_LIGHTNING_BOLT_End();

		//AVSI_SI2_AV_LIGHTNING_BOLT_HYPER
		void AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_FrameMove();
		void AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_CameraMove();
		void AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_EventProcess();
		void AVSI_SI2_AV_LIGHTNING_BOLT_HYPER_End();


		//AVSI_SI1_AV_CHAIN_FIRE_BALL
		void AVSI_SI1_AV_CHAIN_FIRE_BALL_FrameMove();
		void AVSI_SI1_AV_CHAIN_FIRE_BALL_CameraMove();
		void AVSI_SI1_AV_CHAIN_FIRE_BALL_EventProcess();

		//AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER
		void AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_FrameMove();
		void AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_CameraMove();
		void AVSI_SI1_AV_CHAIN_FIRE_BALL_HYPER_EventProcess();


		//AVSI_SI1_AV_HELL_STONE
		void AVSI_SI1_AV_HELL_STONE_Init();
		void AVSI_SI1_AV_HELL_STONE_FrameMoveFuture();
		void AVSI_SI1_AV_HELL_STONE_FrameMove();
		void AVSI_SI1_AV_HELL_STONE_CameraMove();
		void AVSI_SI1_AV_HELL_STONE_EventProcess();

		//AVSI_SI1_AV_HELL_STONE_HYPER
		void AVSI_SI1_AV_HELL_STONE_HYPER_FrameMoveFuture();
		void AVSI_SI1_AV_HELL_STONE_HYPER_FrameMove();
		void AVSI_SI1_AV_HELL_STONE_HYPER_CameraMove();
		void AVSI_SI1_AV_HELL_STONE_HYPER_EventProcess();


		//AVSI_SI2_AV_GUIDE_FIRE_BALL
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_Init();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_FrameMoveFuture();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_FrameMove();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_CameraMove();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_EventProcess();

		//AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_FrameMoveFuture();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_FrameMove();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_CameraMove();
		void AVSI_SI2_AV_GUIDE_FIRE_BALL_HYPER_EventProcess();


		//AVSI_SI2_AV_GUST_SCREW
		void AVSI_SI2_AV_GUST_SCREW_Init();
		void AVSI_SI2_AV_GUST_SCREW_FrameMoveFuture();
		void AVSI_SI2_AV_GUST_SCREW_FrameMove();
		void AVSI_SI2_AV_GUST_SCREW_CameraMove();
		void AVSI_SI2_AV_GUST_SCREW_EventProcess();

		//AVSI_SI2_AV_GUST_SCREW_HYPER
		void AVSI_SI2_AV_GUST_SCREW_HYPER_FrameMoveFuture();
		void AVSI_SI2_AV_GUST_SCREW_HYPER_FrameMove();
		void AVSI_SI2_AV_GUST_SCREW_HYPER_CameraMove();
		void AVSI_SI2_AV_GUST_SCREW_HYPER_EventProcess();


		//AVSI_SI3_AV_GUST_STORM
		void AVSI_SI3_AV_GUST_STORM_Init();
		void AVSI_SI3_AV_GUST_STORM_FrameMoveFuture();
		void AVSI_SI3_AV_GUST_STORM_FrameMove();
		void AVSI_SI3_AV_GUST_STORM_CameraMove();
		void AVSI_SI3_AV_GUST_STORM_EventProcess();

		//AVSI_SI3_AV_GUST_STORM_HYPER
		void AVSI_SI3_AV_GUST_STORM_HYPER_FrameMoveFuture();
		void AVSI_SI3_AV_GUST_STORM_HYPER_FrameMove();
		void AVSI_SI3_AV_GUST_STORM_HYPER_CameraMove();
		void AVSI_SI3_AV_GUST_STORM_HYPER_EventProcess();


		//AVSI_SI3_AV_CATASTROPHE
		void AVSI_SI3_AV_CATASTROPHE_Init();
		void AVSI_SI3_AV_CATASTROPHE_FrameMoveFuture();
		void AVSI_SI3_AV_CATASTROPHE_FrameMove();
		void AVSI_SI3_AV_CATASTROPHE_EventProcess();
		void AVSI_SI3_AV_CATASTROPHE_End();

		//AVSI_SI3_AV_CATASTROPHE_HYPER
		void AVSI_SI3_AV_CATASTROPHE_HYPER_FrameMoveFuture();
		void AVSI_SI3_AV_CATASTROPHE_HYPER_FrameMove();
		void AVSI_SI3_AV_CATASTROPHE_HYPER_EventProcess();
		void AVSI_SI3_AV_CATASTROPHE_HYPER_End();


		//AVSI_SI1_AV_CHAIN_BURST
		//void AVSI_SI1_AV_CHAIN_BURST_Init();
		void AVSI_SI1_AV_CHAIN_BURST_FrameMove();
		void AVSI_SI1_AV_CHAIN_BURST_EventProcess();

		//AVSI_SI1_AV_CHAIN_BURST_HYPER
		void AVSI_SI1_AV_CHAIN_BURST_HYPER_FrameMove();
		void AVSI_SI1_AV_CHAIN_BURST_HYPER_EventProcess();

		//AVSI_SI2_AV_DARK_CLOUD
		void AVSI_SI2_AV_DARK_CLOUD_Init();
		void AVSI_SI2_AV_DARK_CLOUD_FrameMove();
		void AVSI_SI2_AV_DARK_CLOUD_EventProcess();

		//AVSI_SI2_AV_DARK_CLOUD_HYPER
		void AVSI_SI2_AV_DARK_CLOUD_HYPER_FrameMove();
		void AVSI_SI2_AV_DARK_CLOUD_HYPER_EventProcess();


		virtual void COMMON_BUFF_FrameMove();

		//AVSI_SI_AV_COMMON_BUFF
		void AVSI_SI_AV_COMMON_BUFF_FrameMove();
		void AVSI_SI_AV_COMMON_BUFF_EventProcess();

		//AVSI_SI_AV_COMMON_BUFF_HYPER
		void AVSI_SI_AV_COMMON_BUFF_HYPER_FrameMove();
		void AVSI_SI_AV_COMMON_BUFF_HYPER_EventProcess();



		////AVSI_SI2_AV_LAND_OF_FLAME
		//void AVSI_SI2_AV_LAND_OF_FLAME_Init();
		//void AVSI_SI2_AV_LAND_OF_FLAME_FrameMoveFuture();
		//void AVSI_SI2_AV_LAND_OF_FLAME_FrameMove();
		//void AVSI_SI2_AV_LAND_OF_FLAME_CameraMove();
		//void AVSI_SI2_AV_LAND_OF_FLAME_EventProcess();

		////AVSI_SI2_AV_LAND_OF_FLAME_HYPER
		//void AVSI_SI2_AV_LAND_OF_FLAME_HYPER_FrameMoveFuture();
		//void AVSI_SI2_AV_LAND_OF_FLAME_HYPER_FrameMove();
		//void AVSI_SI2_AV_LAND_OF_FLAME_HYPER_CameraMove();
		//void AVSI_SI2_AV_LAND_OF_FLAME_HYPER_EventProcess();

		void AVSI_SI_A_AV_CIRCLE_FLAME_READY_StartFuture();
		void AVSI_SI_A_AV_CIRCLE_FLAME_READY_FrameMove();
		void AVSI_SI_A_AV_CIRCLE_FLAME_READY_EventProcess();
		void AVSI_SI_A_AV_CIRCLE_FLAME_READY_EndFuture();

		void AVSI_SI_A_AV_CIRCLE_FLAME_Init();
		// ���� ���� ���뿡 ���� StateStart ���� �߰�
		void AVSI_SI_A_AV_CIRCLE_FLAME_StateStart();
		void AVSI_SI_A_AV_CIRCLE_FLAME_StartFuture();
		void AVSI_SI_A_AV_CIRCLE_FLAME_FrameMove();
		void AVSI_SI_A_AV_CIRCLE_FLAME_EventProcess();
		void AVSI_SI_A_AV_CIRCLE_FLAME_FrameMoveFuture();
		void AVSI_SI_A_AV_CIRCLE_FLAME_EndFuture();



		void AVSI_SA_AHM_BLIZZARDSHOWER_Init();
		void AVSI_SA_AHM_BLIZZARDSHOWER_StateStart();
		void AVSI_SA_AHM_BLIZZARDSHOWER_FrameMove();
		void AVSI_SA_AHM_BLIZZARDSHOWER_EventProcess();
		void AVSI_SA_AHM_BLIZZARDSHOWER_StateEnd();

		void AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_StateStart();
		void AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_FrameMove();
		void AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_EventProcess();
		void AVSI_SA_AHM_BLIZZARDSHOWER_HYPER_StateEnd();

		void AVSI_SA_ADM_DEATHFIELD_Init();
		void AVSI_SA_ADM_DEATHFIELD_StateStart();
		void AVSI_SA_ADM_DEATHFIELD_FrameMove();
		void AVSI_SA_ADM_DEATHFIELD_EventProcess();
		void AVSI_SA_ADM_DEATHFIELD_StateEnd();

		void AVSI_SA_ADM_DEATHFIELD_HYPER_StateStart();
		void AVSI_SA_ADM_DEATHFIELD_HYPER_FrameMove();
		void AVSI_SA_ADM_DEATHFIELD_HYPER_EventProcess();
		void AVSI_SA_ADM_DEATHFIELD_HYPER_StateEnd();

		void AVSI_A_ADM_MANA_INTAKE_Init();
		void AVSI_A_ADM_MANA_INTAKE_StateStart();
		void AVSI_A_ADM_MANA_INTAKE_FrameMove();
		void AVSI_A_ADM_MANA_INTAKE_EventProcess();

		void AVSI_A_ADM_MANA_INTAKE_HYPER_Init();
		void AVSI_A_ADM_MANA_INTAKE_HYPER_StateStart();
		void AVSI_A_ADM_MANA_INTAKE_HYPER_FrameMove();
		void AVSI_A_ADM_MANA_INTAKE_HYPER_EventProcess();


//{{ ����� 2010.11.1
		void AVSI_SA_AHM_METEOSHOWER_Init();
		void AVSI_SA_AHM_METEOSHOWER_StateStart();
		void AVSI_SA_AHM_METEOSHOWER_FrameMove();
		void AVSI_SA_AHM_METEOSHOWER_EventProcess();
		void AVSI_SA_AHM_METEOSHOWER_StateEnd();

		void AVSI_SA_AHM_METEOSHOWER_LOOP_StateStart();
		void AVSI_SA_AHM_METEOSHOWER_LOOP_FrameMove();
		void AVSI_SA_AHM_METEOSHOWER_LOOP_EventProcess();
		void AVSI_SA_AHM_METEOSHOWER_LOOP_StateEnd();

		void AVSI_SA_AHM_METEOSHOWER_END_EventProcess();
		void AVSI_SA_AHM_METEOSHOWER_END_StateEnd();
//}} ����� 2010.11.1

//{{ ����� 2010.11.9
		void AVSI_SA_AVP_PHANTOM_BREATHING_Init();
		void AVSI_SA_AVP_PHANTOM_BREATHING_StateStart();
		void AVSI_SA_AVP_PHANTOM_BREATHING_FrameMove();
		void AVSI_SA_AVP_PHANTOM_BREATHING_EventProcess();
		void AVSI_SA_AVP_PHANTOM_BREATHING_StateEnd();

		void AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_Init();
		void AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_StateStart();
		void AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_FrameMove();
		void AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_EventProcess();
		void AVSI_SA_AVP_PHANTOM_BREATHING_HYPER_StateEnd();
//}} ����� 2010.11.9



		void AVSI_SA_AHM_BLAZE_STEP_Init();
		void AVSI_SA_AHM_BLAZE_STEP_FrameMove();
		void AVSI_SA_AHM_BLAZE_STEP_EventProcess();

		void AVSI_SA_AHM_BLAZE_STEP_HYPER_Init();
		void AVSI_SA_AHM_BLAZE_STEP_HYPER_FrameMove();
		void AVSI_SA_AHM_BLAZE_STEP_HYPER_EventProcess();

		void AVSI_SA_ADM_AGING_Init();
		void AVSI_SA_ADM_AGING_FrameMove();
		void AVSI_SA_ADM_AGING_EventProcess();




		void AVSI_A_AEM_STONE_WALL_Init();
		void AVSI_A_AEM_STONE_WALL_FrameMove();

		void AVSI_A_AVP_SUMMON_BAT_Init();
		void AVSI_A_AVP_SUMMON_BAT_FrameMove();

		void AVSI_SA_AEM_CYCLONE_Init();
		void AVSI_SA_AEM_CYCLONE_FrameMove();
		//void AVSI_SA_AEM_CYCLONE_EventProcess();


		void AVSI_SA_AVP_HELL_DROP_Init();
		void AVSI_SA_AVP_HELL_DROP_Start();
		//void AVSI_SA_AVP_HELL_DROP_FrameMoveFuture();
		void AVSI_SA_AVP_HELL_DROP_FrameMove();
		//void AVSI_SA_AVP_HELL_DROP_CameraMove();
		void AVSI_SA_AVP_HELL_DROP_EventProcess();
		void AVSI_SA_AVP_HELL_DROP_StateEnd();


		void AVSI_HELL_DROP_CONTROL_FrameMove();

		void AVSI_HELL_DROP_CONTROL_EventProcess();

		void AVSI_HELL_DROP_CONTROL_IDLE_StateStart();
		void AVSI_HELL_DROP_CONTROL_LEFT_StateStart();
		void AVSI_HELL_DROP_CONTROL_RIGHT_StateStart();
		void AVSI_HELL_DROP_CONTROL_UP_StateStart();
		void AVSI_HELL_DROP_CONTROL_DOWN_StateStart();

		void AVSI_HELL_DROP_CONTROL_UP_LEFT_StateStart();
		void AVSI_HELL_DROP_CONTROL_UP_RIGHT_StateStart();
		void AVSI_HELL_DROP_CONTROL_DOWN_LEFT_StateStart();
		void AVSI_HELL_DROP_CONTROL_DOWN_RIGHT_StateStart();

		void AVSI_HELL_DROP_CONTROL_FIRE_StateStart();
		void AVSI_HELL_DROP_CONTROL_FIRE_FrameMove();

		void AVSI_HELL_DROP_CONTROL_FIRE_EventProcess();



		// elemental master �޺�
		//void AVSI_AEM_ZZupZ_EventProcess();

		void AVSI_AEM_ZZupZZ_FrameMove();
		void AVSI_AEM_ZZupZZ_EventProcess();


		void AVSI_AEM_XXfrontX_Init();
		void AVSI_AEM_XXfrontX_FrameMove();
		void AVSI_AEM_XXfrontX_EventProcess();


		void AVSI_AEM_XZ_Init();
		void AVSI_AEM_XZ_FrameMove();
		void AVSI_AEM_XZ_EventProcess();

		void AVSI_AEM_XZZ_FrameMove();
		void AVSI_AEM_XZZ_EventProcess();

		void AVSI_AEM_XZZ_END_EventProcess();

		// void princess �޺�
		void AVSI_AVP_ZZfrontZ_EventProcess();
		void AVSI_AVP_ZZfrontZZ_Init();
		void AVSI_AVP_ZZfrontZZ_StateStartFuture();
		void AVSI_AVP_ZZfrontZZ_EventProcess();
		void AVSI_AVP_ZZfrontZZ_FALL_EventProcess();


		void AVSI_AVP_DASH_JUMP_X_Init();
		void AVSI_AVP_DASH_JUMP_X_FrameMove();
		void AVSI_AVP_DASH_JUMP_X_EventProcess();



		HRESULT OnFrameMove( double fTime, float fElapsedTime );
		void NotifyDGManagerChanged( CKTDGManager& refInManager_ );
		void TransformStart( const WCHAR* pWstrMeshName_, const bool bChangeWeapon_ = false );
		void TransformEnd( const bool bChangeWeapon_ = false );
		void ChangeWeapon( CX2EqipPtr ptrWeaon );

		float GetTransformTime() const { return m_fTransformTime; }
		void SetTransformTime(float val) { m_fTransformTime = val; }

		void ABM_ComboZ1EventProcess();
		void ABM_ComboZ2EventProcess();
		void ABM_ComboZ3EventProcess();
		void ABM_ComboZ4EventProcess();

		void AVSI_ABM_DASH_COMBO_Z2a_FrameMove();
		void AVSI_ABM_DASH_COMBO_Z2a_EventProcess();
		void AVSI_ABM_DASH_COMBO_ZXX_EventProcess();
		void AVSI_ABM_DASH_COMBO_ZXXX_FrameMove();
		void AVSI_ABM_DASH_COMBO_ZXXX_EventProcess();

		void AVSI_A_AV_FIRE_ROAD_Start();
		void AVSI_A_AV_FIRE_ROAD_EventProcess();
		void AVSI_A_AV_FIRE_ROAD_FrameMove();

		void AVSI_A_ABM_ENERGY_DRAIN_READY_FrameMove();
		void AVSI_A_ABM_ENERGY_DRAIN_READY_EventProcess();
		void AVSI_A_ABM_ENERGY_DRAIN_SUCCESS_FrameMove();
		void AVSI_A_ABM_ENERGY_DRAIN_EventProcess();

		void AVSI_SI_SA_ABM_HEAVY_PRESS_FrameMove();
		void AVSI_SI_SA_ABM_ENERGY_SPURT_FrameMove();
		void AVSI_SI_SA_ABM_GUILLOTINE_PRESS_FrameMove();
		void AVSI_SI_SA_ABM_GUILLOTINE_PRESS_START_EventProcess();
		void AVSI_SI_SA_ABM_GUILLOTINE_PRESS_LOOP_EventProcess();
		void AVSI_SI_SA_ABM_SUPER_NOVA_FrameMove();

		void AVSI_SI_SA_ABM_MAGICAL_MAKEUP_Init();
		void AVSI_SI_SA_ABM_MAGICAL_MAKEUP_FrameMove();
		void AVSI_SI_SA_ABM_MAGICAL_MAKEUPR_StateEnd();

		void ShowActiveSkillCutInAndLight( const WCHAR* szBoneName_, const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ = false );
		virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

		void AVSI_DIMENSION_WITCH_DASH_ATTACK_X_EventProcess();
		void AVSI_DIMENSION_WITCH_DASH_ATTACK_X2_EventProcess();
		void AVSI_ADW_COMBO_ADD_X_EventProcess();

		void AVSI_SI_A_ADW_DISTORTION_Init();
		void AVSI_SI_A_ADW_DISTORTION_StateStart();
		void AVSI_SI_A_ADW_DISTORTION_FrameMove();

		void AVSI_SI_SA_ADW_MORNING_STAR_FrameMove();
		void AVSI_SI_SA_ADW_SCREWDRIVER_FrameMove();

	//#pragma region SI_A_AV_ICICLE_WAVE
		void AVSI_A_AV_Icicle_Wave_Init();
		void AVSI_A_AV_Icicle_Wave_EventProcess();
		void AVSI_A_AV_Icicle_Wave_FrameMove();
	//#pragma endregion ���̽�Ŭ ���̺�

	//#pragma region SI_SA_AV_ICE_STORM
		void AVSI_SA_AV_Ice_Storm_Init();
		void AVSI_SA_AV_Ice_Storm_EventProcess();
		void AVSI_SA_AV_Ice_Storm_FrameMove();
	//#pragma endregion ���̽� ����

	//#pragma region SI_SA_AHM_AQUA_SHOWER
		void AVSI_A_AHM_Aqua_Shower_Init();
		void AVSI_A_AHM_Aqua_Shower_FrameMove();
		void AVSI_A_AHM_Aqua_Shower_EventProcess();
	//#pragma endregion ������ ����

	//#pragma region SI_SA_AHM_Lightning_Shower
		void AVSI_SA_AHM_Lightning_Shower_Init();
		void AVSI_SA_AHM_Lightning_Shower_StateStart();
		void AVSI_SA_AHM_Lightning_Shower_FrameMove();
		void AVSI_SA_AHM_Lightning_Shower_EventProcess();
		void AVSI_SA_AHM_Lightning_Shower_StateEnd();

		void AVSI_SA_AHM_Lightning_Shower_Loop_StateStart();
		void AVSI_SA_AHM_Lightning_Shower_Loop_FrameMove();
		void AVSI_SA_AHM_Lightning_Shower_Loop_EventProcess();
		void AVSI_SA_AHM_Lightning_Shower_Loop_StateEnd();

		void AVSI_SA_AHM_Lightning_Shower_End_EventProcess();
		void AVSI_SA_AHM_Lightning_Shower_End_StateEnd();
	//#pragma endregion ����Ʈ�� ����

	//#pragma region SI_SA_AEM_CHAIN_LIGHTNING
		void AVSI_SA_AEM_Chain_Lightning_Init();
		void AVSI_SA_AEM_Chain_Lightning_StartFuture();
		void AVSI_SA_AEM_Chain_Lightning_EndFuture();
		void AVSI_SA_AEM_Chain_Lightning_FrameMove();
		void AVSI_SA_AEM_Chain_Lightning_EventProcess();
	//#pragma endregion ü�� ����Ʈ��

	//#pragma region SI_A_AVP_SUMMON_BAT_HEAVY
		void AVSI_A_AVP_Summon_Bat_Heavy_Init();
		void AVSI_A_AVP_Summon_Bat_Heavy_FrameMove();
	//#pragma endregion �ڶ� �Ӱ��� - ��� ��

	//#pragma region SI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE
		void AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_Init();
		void AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_FrameMove();
		void AVSI_SA_AVP_Phantom_Breathing_Dark_Hole_EventProcess();
	//#pragma endregion ���� �긮�� - ��ũ Ȧ

	//#pragma region SHINGIN_BODY, SHADOW_BODY, ENERGETIC
		void ProcessShiningBodySkill();
		void ProcessShadowBodySkill();
		void ProcessEnergeticBodySkill();
	//#pragma endregion �ٵ� �ø���

	//#pragma region SI_A_ABM_VITAL_DRAIN
		void AVSI_A_ABM_Vital_Drain_Ready_Init();
		void AVSI_A_ABM_Vital_Drain_Ready_FrameMove();
		void AVSI_A_ABM_Vital_Drain_Ready_EventProcess();
		void AVSI_A_ABM_Vital_Drain_Success_FrameMove();
		void AVSI_A_ABM_Vital_Drain_EventProcess();
	//#pragma endregion ����Ż �巹��

	//#pragma region SI_A_ADW_WORM_HOLE
		void AVSI_A_ADW_Worm_Hole_Init();
		void AVSI_A_ADW_Worm_Hole_StateStart();
		void AVSI_A_ADW_Worm_Hole_FrameMove();
	//#pragma endregion ��Ȧ

	//#pragma region SI_SA_ADW_SCREWDRIVER_DRILLER
		void AVSI_SI_SA_ADW_Screwdriver_Driller_Init();
		void AVSI_SI_SA_ADW_Screwdriver_Driller_FrameMove();
	//#pragma endregion ��ũ�� ����̹� - �帱��


	//#pragma region SI_SA_AEM_Elemental_Storm
		void AVSI_HA_AEM_Elemental_Storm_Init();
	//#pragma endregion ������Ż ���� - �ñر�

	//#pragma region SI_FS_AVP_Abyss_Angor
		void AVSI_HA_AVP_Abyss_Angor_Init();
	//#pragma endregion ��� �Ӱ��� - �ñر�

	//#pragma region SI_FS_ADW_Fate_space
		void AVSI_HA_ADW_Fate_space_Init();
	//#pragma endregion ����Ʈ �����̽� - �ñر�


		virtual bool IsValideSkillRidingOn( CX2SkillTree::SKILL_ID eSkill_ID_ );
};
