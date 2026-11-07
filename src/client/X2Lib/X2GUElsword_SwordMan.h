#pragma once

class CX2GUElsword_SwordMan : public CX2GUUser
{
	public:
		enum ELSWORD_SWORDMAN_STATE_ID
		{
			ESSI_BASE = USI_END + 1,

			ESSI_DASH_JUMP_POWER_LANDING,

			ESSI_STANDUP_ROLLING_FRONT_FRONT,
			ESSI_STANDUP_ROLLING_FRONT_BACK,
			ESSI_STANDUP_ROLLING_BACK_FRONT,
			ESSI_STANDUP_ROLLING_BACK_BACK,

			ESSI_STANDUP_ATTACK_FRONT,
			ESSI_STANDUP_ATTACK_BACK,

			ESSI_REVENGE_WAIT,
			ESSI_REVENGE_ATTACK,

			ESSI_COMBO_Z1,
			ESSI_COMBO_Z2,
			ESSI_COMBO_Z3,
			ESSI_COMBO_Z4,
			ESSI_COMBO_Z4a,

			ESSI_COMBO_X1,
			ESSI_COMBO_X2,
			ESSI_COMBO_X3,
			ESSI_COMBO_X3a,

			ESSI_DASH_COMBO_Z1,
			ESSI_DASH_COMBO_Z2,
			ESSI_DASH_COMBO_Z3,
			ESSI_DASH_COMBO_Z3a,

			ESSI_DASH_ATTACK_X,

			ESSI_JUMP_ATTACK_Z,
			ESSI_JUMP_ATTACK_X,

			ESSI_DASH_JUMP_COMBO_Z1,
			ESSI_DASH_JUMP_COMBO_Z2,
			ESSI_DASH_JUMP_COMBO_Z3,
			ESSI_DASH_JUMP_COMBO_Z3_LANDING,

			ESSI_DASH_JUMP_COMBO_X1,
			ESSI_DASH_JUMP_COMBO_X2,
			ESSI_DASH_JUMP_COMBO_X2a,
			ESSI_DASH_JUMP_COMBO_X2_LANDING,
			ESSI_DASH_JUMP_COMBO_X3,

			ESSI_STANDUP_ATTACK_FRONT_NO_DOWN,
			ESSI_STANDUP_ATTACK_BACK_NO_DOWN,

			ESSI_SA_ESK_WINDMILL,
			ESSI_SA_ESK_WINDMILL_HYPER,
			ESSI_SA_ESK_WINDMILL_END,
			ESSI_SA_ESK_WINDMILL_END_HYPER,

			ESSI_SA_EMK_SWORD_FIRE_LANDING,
			ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER,

			ESSI_SA_AIR_SLASH_SECOND,
			ESSI_SA_AIR_SLASH_SECOND_HYPER,


			// ���ҵ� 2�� ���� �޺�
			ESSI_ELK_ZZZfrontZ,
			ESSI_ELK_XXX,
			ESSI_ELK_XXXX,

			ESSI_ERS_XZ,
			ESSI_ERS_XZZ,
			ESSI_ERS_DASH_JUMP_XZ,
			ESSI_ERS_DASH_JUMP_XZZ,



			ESSI_A_ELK_ROLLING_SMASH,
			ESSI_A_ELK_ROLLING_SMASH_HYPER,
			ESSI_A_ELK_ROLLING_SMASH_LANDING,
			ESSI_A_ELK_ROLLING_SMASH_LANDING_HYPER,

			ESSI_SWORD_WAIT,
			ESSI_DESTRUCTION_RAGE,

			ESSI_ETK_COMBO_ZZZ,
			ESSI_ETK_COMBO_ZZZZ,
			ESSI_ETK_COMBO_ZZZZZ,
			ESSI_ETK_COMBO_XX,
			ESSI_ETK_COMBO_XXX_LOOP,
			ESSI_ETK_COMBO_XXXZ,
			ESSI_ETK_COMBO_XXX_END,
			ESSI_ETK_DASH_COMBO_ZZ,

			ESSI_SI_A_ES_ROLLING,
			ESSI_SI_A_ES_ROLLING_END,
			ESSI_SI_SA_ETK_HARSH_CHASER,
			ESSI_SI_SA_ETK_HARSH_CHASER_PULL,
			ESSI_SI_SA_ETK_MAELSTORM_RAGE,
			ESSI_SI_SA_ETK_FINAL_STRIKE,
			ESSI_SI_SA_ETK_SWORD_FALL,
			ESSI_SI_SA_ETK_CRESCENT_CUT,


			ESSI_EIS_DASH_COMBO_XX,
			ESSI_EIS_DASH_COMBO_XXX,
			ESSI_EIS_DASHJUMP_COMBO_XXX,

			ESSI_SI_A_ES_DEFENCE_READY,
			ESSI_SI_A_ES_DEFENCE_WAIT,
			ESSI_SI_A_ES_DEFENCE_END,
			ESSI_SI_A_ES_DEFENCE_DAMAGE,
			ESSI_SI_P_ESK_AUTO_GUARD,

			//////////////////////////////////////////////////////////////////////////
			// ���ʿ� �߰����ּ���~ �� �Ʒ��� �׽�Ʈ�� �߰��� enum �Դϴ�



			ESSI_TRANSFORMED,

			ESSI_LINK_JUMP,

			ESSI_CLIMB_WAIT,
			ESSI_CLIMB_FRONT,
			ESSI_CLIMB_UP,
			ESSI_CLIMB_UP_FRONT,
			ESSI_CLIMB_DOWN,
			ESSI_CLIMB_DOWN_FRONT,


			ESSI_GRAB_FRONT,
			ESSI_PICK_UP,
			ESSI_THROW_FORWARD,
			ESSI_THROW_UPSIDE,


			ESSI_THROW_WOODEN_PIECE,

			ESSI_MACHINE_GUN_RIDE,
			ESSI_MACHINE_GUN_WAIT,
			ESSI_MACHINE_GUN_WALK,
			ESSI_MACHINE_GUN_ATTACK,
			ESSI_MACHINE_GUN_LEAVE,
			ESSI_MACHINE_GUN_JUMP_UP,
			ESSI_MACHINE_GUN_JUMP_DOWN,

			ESSI_SUMMON_MAGIC_SPEAR,

			ESSI_LEAP,


//#ifdef WALL_JUMP_TEST
			ESSI_WALL_LANDING,
//#endif WALL_JUMP_TEST

//#ifdef PVP_BOSS_COMBAT_TEST
//			ESSI_FROZEN,
//#endif PVP_BOSS_COMBAT_TEST
		};

		enum ELSWORD_MAJOR_PARTICLE_INSTANCE_ID
		{
			ELSWORD_MAJOR_PII_BASE,			// kimhc // ����� ���̴� ���� ��� �ӽ÷� �־����
			ELSWORD_MAJOR_PII_END,
		};

		enum ELSWORD_MINOR_PARTICLE_INSTANCE_ID
		{
			ELSWORD_MINOR_PII_REVENGE_WAIT,
			ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_BLUE,
			ELSWORD_MINOR_PII_IMPACT_BLUE,
			ELSWORD_MINOR_PII_STR_KUAANG,
			ELSWORD_MINOR_PII_GROUND_SHOCK_WAVE_RED,
			ELSWORD_MINOR_PII_IMPACT_TICK_RED,
			ELSWORD_MINOR_PII_MAGNUM_BLADE_FIRE_WAVE,

			ELSWORD_MINOR_PII_END,
		};

		struct ThirdBladeMatrix
		{
			D3DXMATRIX		PastCombineMatrix;
			float			fPastTime;
			ThirdBladeMatrix()
			{
				fPastTime	= 0.f;
			}
		};


		struct ThirdBladeDamageData
		{
			CX2DamageManager::DamageData	m_DamageData;
			float							m_fAttackTimeStart;
			float							m_fAttackTimeEnd;
			ThirdBladeDamageData()
			{
				m_fAttackTimeStart		= 0.f;
				m_fAttackTimeEnd		= 0.f;
			}
		};





        class   CElswordGuardSkill;
        typedef x2::intrusive_ptr<CElswordGuardSkill> CElswordGuardSkillPtr;

		class CElswordGuardSkill : x2::NonCopyable
		{
		private:
            unsigned                                        m_uRefCount;
			ELSWORD_SWORDMAN_STATE_ID	m_eSkillSlotID_Guard;			/// ���ҵ��� Guard�� ��� ��ų���Կ� �����Ǿ� �ִ����� ��� ����
			const bool*					m_pGuardKey;					/// ���ҵ��� Guard�� �����Ǿ� �ִ� ��ų������ Key(A, S, D, C ��)
			bool						m_bGuardEquippedAtSlotB;		/// ���ҵ��� Guard�� B���Կ� �����Ǿ� �ִ����� ��� ����
			int							m_iDamageReducePercent;			/// ���ҵ��� Guard ���¿��� ������ ������
			CElswordGuardSkill() : m_uRefCount(0),
				m_eSkillSlotID_Guard( ESSI_BASE ), m_pGuardKey( NULL ), m_bGuardEquippedAtSlotB( false )
                , m_iDamageReducePercent( 0 )
			{}

		public:
			static CElswordGuardSkillPtr    CreateElswordGuardSkill() { return CElswordGuardSkillPtr( new CElswordGuardSkill ); }

			CX2GUElsword_SwordMan::ELSWORD_SWORDMAN_STATE_ID GetSkillSlotID_Guard() const { return m_eSkillSlotID_Guard; }	// ���ҵ��� Guard�� ��� ��ų���Կ� �����Ǿ� �ִ����� ��� �������� ����
			void SetSkillSlotID_Guard(CX2GUElsword_SwordMan::ELSWORD_SWORDMAN_STATE_ID eSkillSlotID_Guard_) { m_eSkillSlotID_Guard = eSkillSlotID_Guard_; } // ���ҵ��� Guard�� ��� ��ų���Կ� �����Ǿ� �ִ����� ��� �������� Set��

			bool GetGuardKeyValue() const { return *m_pGuardKey; }	// ���ҵ��� Guard�� �����Ǿ� �ִ� ��ų������ Key(A, S, D, C ��)	�� ���� ���� (���ȴ��� �ȴ��ȴ���)
			void SetGuardKeyPointer( const bool* pGuardKey_ ) { m_pGuardKey = pGuardKey_; }	// ���ҵ��� Guard�� �����Ǿ� �ִ� ��ų������ Key(A, S, D, C ��)	�����͸� Set��

			bool GetGuardEquippedAtSlotB() const { return m_bGuardEquippedAtSlotB; }	// ���ҵ��� Guard�� B���Կ� �����Ǿ� �ִ����� ����
			void SetGuardEquippedAtSlotB(bool bGuardEquippedAtSlotB_) { m_bGuardEquippedAtSlotB = bGuardEquippedAtSlotB_; }

			int GetDamageReducePercent() const { return m_iDamageReducePercent; }
			void SetDamageReducePercent(int iDamageReducePercent_) { m_iDamageReducePercent = iDamageReducePercent_; }

            void    AddRef()    {   ++m_uRefCount; }
            void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }
		};




	private:
		CX2GUElsword_SwordMan( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit );
		virtual ~CX2GUElsword_SwordMan(void);

		//{{ kimhc // 2010.11.2 // �ε峪��Ʈ - ���� ����
		void				ReducePain( CX2SkillTree::SKILL_TYPE eSkillType_, const IN CX2UserSkillTree& cUserSkillTree_ );
		//}} kimhc // 2010.11.2 // �ε峪��Ʈ - ���� ����


	public:
		static CX2GUElsword_SwordMan* CreateGUElsword( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit )
		{
			return new CX2GUElsword_SwordMan( unitIndex, teamNum,
				pScriptLuaState,
				frameBufferNum, pUnit );
		};

		static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
		static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
		static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );


		virtual void		ReInit( bool bRandom = false, int startPosIndex = -1 );


		virtual void		DamageReact( CX2DamageManager::DamageData* pDamageData );
		virtual void		DamageReactStateChange( CX2DamageManager::DamageData* pDamageData, int iRevengeAttack = -1, int iAvoidance = -1 );

		virtual float		GetFinalDamageToBeChangedByActiveSkill( float fFinalDamage_ );
		void				ApplyBuffToHarshSlayer( const CX2DamageManager::DamageData& damageData_ );

		//�� ���ָ��� Enchant�� ����� �� �����ؾ� �ϴ� �Լ� ����.
		virtual void				SetSpecificValueByEnchant();

		virtual void		Win();
		virtual void		Lose();
		virtual void		Draw();


		virtual void		InitEffect();
		virtual void		InitEnchantWeaponEffectUnitSpecific();


		void				InitializeElswordMajorParticleArray();	// ������ ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		virtual void		CreateMajorParticleArray();		// ������ ��ƼŬ �迭�� ���� // kimhc // 2010.11.5
		static void			AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
		CKTDGParticleSystem::CParticleEventSequence* SetElswordMajorParticleByEnum( ELSWORD_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		ParticleEventSequenceHandle	GetHandleElswordMajorParticleByEnum( ELSWORD_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const // ���ҵ常 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		{
			ASSERT( ELSWORD_MAJOR_PII_END > eVal_ && ELSWORD_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahElswordMajorParticleInstance[eVal_];
		}

		ParticleEventSequenceHandle& GetHandleReferenceElswordMajorParticleByEnum( ELSWORD_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) // ���ҵ常 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		{
			ASSERT( ELSWORD_MAJOR_PII_END > eVal_ && ELSWORD_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahElswordMajorParticleInstance[eVal_];
		}

		void				SetHandleElswordMajorParticleByEnum( ELSWORD_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ ) // ���ҵ常 ���� ������ ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������ // kimhc // 2010.11.5
		{
			ASSERT( ELSWORD_MAJOR_PII_END > eVal_ && ELSWORD_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			m_ahElswordMajorParticleInstance[eVal_] = hHandle_;
		}
		void				DeleteElswordMajorParticle();


		void				InitializeElswordMinorParticleArray();	// ���������� ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		virtual void		CreateMinorParticleArray();		// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		static void			AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
		CKTDGParticleSystem::CParticleEventSequence* SetElswordMinorParticleByEnum( ELSWORD_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		ParticleEventSequenceHandle	GetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const	// ���ҵ常 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
		{
			ASSERT( ELSWORD_MINOR_PII_END > eVal_ && ELSWORD_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahElswordMinorParticleInstance[eVal_];
		}

		ParticleEventSequenceHandle& GetHandleReferenceElswordMinorParticleByEnum( ELSWORD_MINOR_PARTICLE_INSTANCE_ID eVal_ ) // ���ҵ常 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
		{
			ASSERT( ELSWORD_MINOR_PII_END > eVal_ && ELSWORD_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahElswordMinorParticleInstance[eVal_];
		}

		void				SetHandleElswordMinorParticleByEnum( ELSWORD_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )	// ���ҵ常 ���� ���̳� ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������	// kimhc // 2010.11.5
		{
			ASSERT( ELSWORD_MINOR_PII_END > eVal_ && ELSWORD_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			m_ahElswordMinorParticleInstance[eVal_] = hHandle_;
		}
		void				DeleteElswordMinorParticle();

		virtual void		DeleteMinorParticle();

		virtual bool		SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet );

		virtual float	GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;

		//{{ oasis907 : ����� [2010.11.1] // �� �����̾� - ���� ���� ����
		float				GetMagicReflectRate();
		//}}

		CX2ElswordGageData*	GetElswordGageData()
		{
			ASSERT( NULL != GetGageData() );
			return static_cast<CX2ElswordGageData*>( GetGageData() );
		}
		const CX2ElswordGageData*	GetElswordGageData() const
		{
			ASSERT( NULL != GetGageData() );
			return static_cast<const CX2ElswordGageData*>( GetGageData() );
		}

		virtual int			GetWayOfSwordState() const;
		void SetWayOfSwordState( const int iWayOfSwordState_ );

		virtual float		GetWayOfSwordPoint() const;
		virtual void		SetWayOfSwordPoint( float fWayOfSwordPoint_ );
		float				GetAnimSpeedUpByVigor(){ return m_fAnimSpeedUpByVigor; }

		void				Start_SwordmanVigor();
		void				End_SwordmanVigor();
		void				Start_SwordmanDestruction();
		void				End_SwordmanDestruction();

		void				AddComboToBrutalSlayer( const CX2DamageManager::DamageData& damageData_ );
		virtual bool CanRidingState();

		virtual float		GetFinalDamage_WayOfSword( CX2DamageManager::WAY_OF_SWORD_TYPE eWayOfSwordType_ );

		virtual int GetComboZStateID() const { return ESSI_COMBO_Z1; }
		virtual int GetComboXStateID() const { return ESSI_COMBO_X1; }

	protected:
		virtual void ParseCommonRandomState();

		virtual void		InitState();


		void InitStateID();
		void InitStateCommon();


		virtual void		InitComponent();

		/// �� ���ָ��� Ư�� ������ ���� �Ǵ� ���� �� �� �����ؾ� �ϴ� �Լ� ����(ex: �����ø���ũ���� �������̶�� �÷��� ��...)
		virtual void		SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
		virtual void		UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );

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

		void				ProcessEnduranceSkill();		/// �ر� ��ų ó��

		virtual	bool		ShouldResetNotBusyTimer();
		virtual void		HyperModeBuffEffectStart();

	protected:
		bool				m_bReAttackZ1Right;
		float				m_fReAttackZ1Time;
		float				m_fReDashJumpXTime;

		float				m_fAttackPowerRateFireBall;


		float					m_fWindMillDuration;
		CX2EffectSet::Handle	m_hWindMill;
		CX2EffectSet::Handle	m_hWindMillLand;
		CKTDXDeviceSound*		m_pSoundWindMill;

		CX2EffectSet::Handle	m_hSwordFire;

        CX2DamageEffect::CEffectHandle   m_hRuneSlayerDashJumpXZ;
		CX2EffectSet::Handle m_hRuneSlayerComboXZ;
		CX2EffectSet::Handle m_hSwordEnchant;



        CX2DamageEffect::CEffectHandle			    m_hDoubleSlash;

		//{{ oasis907 : ����� [2010.11.1] // �� �����̾� - ���� ���� ����
		float								m_fMagicReflectRate;	// ���� �ݻ� Ȯ��
		//}}

        /** �Ʒ��� CKTDGXSkinAnim* Ÿ���� ������� ���������� �����ϴ� ��ü�̹Ƿ�,
            smart pointer�� ������� �ʴ´�.
            - jintaeks on 2009-01-12, 16:26 */




		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hMeshInstAirSlash1;
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hMeshInstFatalFury;

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqAssualtSlashRing;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqAssualtSlashDust;

		// UI Major
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqHandFire;

		D3DXVECTOR3										m_StartPosToLeap;
		D3DXVECTOR3										m_DirVecToLeap;
		float											m_fVerticalSpeedToLeap;
		float											m_fParallelSpeedToLeap;
		float											m_fHeightToLeap;
		int												m_DegreeToLeap;
		float											m_fTimeToLeap;
		float											m_fElapsedTimeToLeap;



		float m_fRollingSmashDuration;
		bool m_bIsMagicKnightWinMotion2;
		bool m_bERS_DASH_JUMP_XZ_Rune_Created;
		bool m_bERS_COMBO_XZ_Rune_Created;

		bool m_bFirstAttackSuccess;

		CX2EffectSet::Handle							m_hEffectVigor;
		CX2EffectSet::Handle							m_hEffectDestruction;
		float											m_fDestructionRageTime;
		float											m_fAnimSpeedUpByVigor;

		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hThirdBladeSheathMesh;

		float											m_fDamageDataChangeTime;
		float											m_fDamageDataChangeTime2;

		int												m_iETKComboXXXLoopCount;

		D3DXVECTOR3										m_vFinalStrikeBlackHolePos;
        CX2DamageEffect::CEffectHandle   						m_hFinalStrikeBlackHole;

		D3DXVECTOR3										m_vSwordFallSheathPos;

		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeFirst;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeSecond;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeThird;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeFourth;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeFirst_Memo;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeSecond_Memo;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeThird_Memo;
		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSwordFallBladeFourth_Memo;

		CKTDGXMeshPlayer::CXMeshInstanceHandle			m_hSecondBladeSheath;

        CX2DamageEffect::CEffectHandle                           m_hDamageEffectHarshChaserBlade;

		float											m_fRollingHpConsumeRel;
		float											m_fRollingSpeedRel;
		float											m_fHarshChaserProjectiveRange;
		float											m_fMaelStormEffectiveTime;


		bool											m_bCompactCounterEnable;
		float											m_fCompactCounterCoolTime;


		float											m_fDashStateTime;
		float											m_fMindOfFighterConsumeMpRate;
		int												m_iBrutalSlayerActivatedLevel;
        CX2DamageEffect::CEffectHandle                           m_hDamageSwordBlasting[3];

		//���� ���ø����� �ٸ� ���� ����Ʈ�� �����ߴٸ�,  ��ȭ ����Ʈ ���� ���� �ʵ��� �ϱ� ���� �߰�
		bool											m_bShowWeaponEnchantEffect; // ���� ��ȭ ����Ʈ ��� ����.

		CElswordGuardSkillPtr							m_GuardForElswordPtr;				//����
		float											m_fAutoGuardRate;					//���� ���� Ȯ��
        CX2DamageEffect::CEffectHandle						    m_hDamageEffectPiercingSword;		//�Ǿ�� �ҵ� ����Ʈ �ڵ�
		float											m_fPiercingSwordProjectileRange;	//�Ǿ�� �ҵ� ��Ÿ�
		float											m_fRuneProjectileRangeIncPercent;	//�� ��Ÿ� ���� ����(�� �����͸�)
		float											m_fRuneProjectileSizeIncPercent;	//�� ũ�� ���� ����(�� �����͸�)
		bool											m_bIsGuardMotion;					//���� ���� ����� ���ϰ� �ִ��� üũ
		UCHAR											m_ucSaveDebuffCount;				//������ �γ� ����� �ɸ���츦 �˾Ƴ��� ���� ����
		bool											m_bAutoGuardBlocking;				//���� ���� üũ ��
		float											m_fIncreaseCoolTimeRel;		//������ �ϰ� ��ų�� ���� �߰��Ǵ� ��Ÿ��
		float											m_fActiveSkillAddPowerRate;			//������ �ϰ� ��ų�� ���� �����Ǵ� ��Ƽ�� ��ų�� ���ݷ�
		CX2EffectSet::Handle							m_hEffectSwordShield;				//�ҵ� �ǵ� ����Ʈ �ڵ�
		float											m_fSwordShieldEffectiveTime;		//�ҵ� �ǵ� ����Ʈ ���� �ð�

		int												m_iSkillLevelInduranceOfRevenge; //������ �γ� ��ų ���� ��.

	private:

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahElswordMajorParticleInstance[ELSWORD_MAJOR_PII_END];
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_ahElswordMinorParticleInstance[ELSWORD_MINOR_PII_END];



		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////

		//							������Ʈ �Լ� ����

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////

	protected:

		void LightingStepFrameMove();	/// ������ȭ ���� ����

		//ESSI_DIE_FRONT
		void DieFrontStartFuture();
		void DieFrontStart();
		void DieFrontFrameMove();
		void DieFrontEventProcess();

		//ESSI_DIE_BACK
		void DieBackStartFuture();
		void DieBackStart();
		void DieBackFrameMove();
		void DieBackEventProcess();

		void WinFrameMove();
		//void WinCameraMove();

		//ESSI_WAIT
		void WaitStartFuture();
		void WaitStart();
		//void WaitFrameMoveFuture();
		void WaitEventProcess();

		//ESSI_WALK
		void WalkEventProcess();

		//ESSI_JUMP_READY
		void JumpReadyEventProcess();

		//ESSI_JUMP_UP
		void JumpUpEventProcess();

		//ESSI_JUMP_DOWN
		void JumpDownEventProcess();

		//ESSI_JUMP_LANDING
		void JumpLandingEventProcess();

		//ESSI_DASH
		void DashEventProcess();

		//ESSI_DASH_END
		void DashEndStartFuture();
		void DashEndStart();
		void DashEndFrameMove();
		void DashEndEventProcess();

		//ESSI_DASH_JUMP
		void DashJumpStartFuture();
		void DashJumpStart();
		void DashJumpFrameMoveFuture();
		void DashJumpEventProcess();
		void DashJumpEndFuture();

		//ESSI_DASH_JUMP_LANDING
		void DashJumpLandingEventProcess();

		//ESSI_DASH_JUMP_POWER_LANDING
		void DashJumpPowerLandingStartFuture();
		void DashJumpPowerLandingStart();
		void DashJumpPowerLandingEventProcess();

		//ESSI_HYPER_MODE
		void HyperModeFrameMove();

		void RidingHyperModeFrameMove();
		void CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ = false );

		////ESSI_DAMAGE_GROGGY
		//void ESSI_DAMAGE_GROGGY_FrameMoveFuture();
		//void ESSI_DAMAGE_GROGGY_EventProcess();

		//ESSI_DAMAGE_SMALL_FRONT
		void DamageSmallFrontEventProcess();

		//ESSI_DAMAGE_SMALL_BACK
		void DamageSmallBackEventProcess();

		//ESSI_DAMAGE_BIG_FRONT
		void DamageBigFrontStart();
		void DamageBigFrontEventProcess();

		//ESSI_DAMAGE_BIG_BACK
		void DamageBigBackStart();
		void DamageBigBackEventProcess();

		//ESSI_DAMAGE_DOWN_FRONT
		void DamageDownFrontFrameMove();
		void DamageDownFrontEventProcess();

		//ESSI_DAMAGE_DOWN_BACK
		void DamageDownBackFrameMove();
		void DamageDownBackEventProcess();

		//ESSI_DAMAGE_AIR_DOWN_LANDING
		void DamageAirDownLandingEventProcess();

		//ESSI_STANDUP_ROLLING_FRONT_FRONT
		void StandUpRollingFrontFrontEventProcess();
		void StandUpRollingFrontFrontEnd();

		//ESSI_STANDUP_ROLLING_FRONT_BACK
		void StandUpRollingFrontBackEventProcess();
		void StandUpRollingFrontBackEnd();

		//ESSI_STANDUP_ROLLING_BACK_FRONT
		void StandUpRollingBackFrontEventProcess();
		void StandUpRollingBackFrontEnd();

		//ESSI_STANDUP_ROLLING_BACK_BACK
		void StandUpRollingBackBackEventProcess();
		void StandUpRollingBackBackEnd();

		void StandUpAttackFrontEventProcess();
		void StandUpAttackFrontEnd();

		void StandUpAttackBackEventProcess();
		void StandUpAttackBackEnd();

		//ESSI_REVENGE_WAIT
		void RevengeWaitStart();
		void RevengeWaitFrameMoveFuture();
		void RevengeWaitFrameMove();
		void RevengeWaitEventProcess();

		//ESSI_REVENGE_ATTACK
		void RevengeAttackStart();
		void RevengeAttackFrameMoveFuture();
		void RevengeAttackFrameMove();
		void RevengeAttackCameraMove();
		void RevengeAttackEventProcess();

		//ESSI_COMBO_Z1
		void ComboZ1FrameMoveFuture();
		void ComboZ1FrameMove();
		void ComboZ1EventProcess();

		//ESSI_COMBO_Z2
		void ComboZ2FrameMove();
		void ComboZ2EventProcess();

		//ESSI_COMBO_Z3
		void ComboZ3FrameMove();
		void ComboZ3EventProcess();

		//ESSI_COMBO_Z4
		void ComboZ4FrameMoveFuture();
		void ComboZ4FrameMove();
		void ComboZ4EventProcess();

		//ESSI_COMBO_Z4a
		void ComboZ4aFrameMove();
		void ComboZ4aEventProcess();

		//ESSI_COMBO_X1
		void ComboX1FrameMoveFuture();
		void ComboX1FrameMove();
		void ComboX1EventProcess();

		//ESSI_COMBO_X2
		void ComboX2FrameMoveFuture();
		void ComboX2FrameMove();
		void ComboX2EventProcess();

		//ESSI_COMBO_X3
		void ComboX3FrameMoveFuture();
		void ComboX3FrameMove();
		void ComboX3EventProcess();

		//ESSI_COMBO_X3a
		void ComboX3aFrameMove();
		void ComboX3aEventProcess();

		//ESSI_DASH_COMBO_Z1
		void DashComboZ1FrameMove();
		void DashComboZ1EventProcess();

		//ESSI_DASH_COMBO_Z2
		void DashComboZ2StartFuture();
		void DashComboZ2FrameMove();
		void DashComboZ2EventProcess();

		//ESSI_KNIGHT_DASH_COMBO_Z2
		void ESSI_KNIGHT_DASH_COMBO_Z2_StateStartFuture();
		void ESSI_KNIGHT_DASH_COMBO_Z2_EventProcess();

		//ESSI_KNIGHT_DASH_COMBO_Z3
		void ESSI_KNIGHT_DASH_COMBO_Z3_StateStartFuture();
		void ESSI_KNIGHT_DASH_COMBO_Z3_EventProcess();

		//ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3
		void ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3_StateStartFuture();
		void ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3_EventProcess();

		//ESSI_KNIGHT_DASH_COMBO_Z3a
		void ESSI_KNIGHT_DASH_COMBO_Z3a_StateStartFuture();
		void ESSI_KNIGHT_DASH_COMBO_Z3a_EventProcess();

		//ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a
		void ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_FrameMove();
		void ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_EventProcess();
		void ESSI_MAGIC_KNIGHT_DASH_COMBO_Z3a_StateEndFuture();




		//ESSI_DASH_ATTACK_X
		void DashAttackXStartFuture();
		void DashAttackXFrameMove();
		void DashAttackXEventProcess();

		//ESSI_JUMP_ATTACK_Z
		void JumpAttackZFrameMoveFuture();
		void JumpAttackZFrameMove();
		void JumpAttackZEventProcess();

		//ESSI_JUMP_ATTACK_X
		void JumpAttackXFrameMoveFuture();
		void JumpAttackXFrameMove();
		void JumpAttackXEventProcess();

		//ESSI_DASH_JUMP_COMBO_Z1
		void DashJumpComboZ1FrameMove();
		void DashJumpComboZ1EventProcess();

		//ESSI_DASH_JUMP_COMBO_Z2
		void DashJumpComboZ2FrameMove();
		void DashJumpComboZ2EventProcess();

		//ESSI_DASH_JUMP_COMBO_Z3
		void DashJumpComboZ3FrameMove();
		void DashJumpComboZ3EventProcess();

		//ESSI_DASH_JUMP_COMBO_Z3_LANDING
		void DashJumpComboZ3LandingStartFuture();
		void DashJumpComboZ3LandingStart();
		void DashJumpComboZ3LandingEventProcess();

		//ESSI_DASH_JUMP_COMBO_X1
		void DashJumpComboX1FrameMove();
		void DashJumpComboX1EventProcess();

		//ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X1
		//void ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X1_EventProcess();

		//ESSI_DASH_JUMP_COMBO_X2
		void DashJumpComboX2FrameMoveFuture();
		void DashJumpComboX2FrameMove();
		void DashJumpComboX2EventProcess();
		void DashJumpComboX2End();

		//ESSI_DASH_JUMP_COMBO_X2_LANDING
		void DashJumpComboX2LandingStartFuture();
		void DashJumpComboX2LandingStart();
		void DashJumpComboX2LandingEventProcess();

		//ESSI_KNIGHT_DASH_JUMP_COMBO_X2
		void ESSI_KNIGHT_DASH_JUMP_COMBO_X2_FrameMoveFuture();
		void ESSI_KNIGHT_DASH_JUMP_COMBO_X2_FrameMove();
		void ESSI_KNIGHT_DASH_JUMP_COMBO_X2_EventProcess();
		void ESSI_KNIGHT_DASH_JUMP_COMBO_X2_End();

		//ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a
		void ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_FrameMoveFuture();
		void ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_FrameMove();
		void ESSI_MAGIC_KNIGHT_DASH_JUMP_COMBO_X2a_EventProcess();

		//ESSI_KNIGHT_DASH_JUMP_COMBO_X3
		void ESSI_KNIGHT_DASH_JUMP_COMBO_X3_EventProcess();


		////////////////////////////////////////////////////////////////////////
		//ESSI_SI_ES_POWER_ATTACK
		void ESSI_SI_ES_POWER_ATTACK_FrameMoveFuture();
		void ESSI_SI_ES_POWER_ATTACK_FrameMove();
		void ESSI_SI_ES_POWER_ATTACK_EventProcess();

		//ESSI_SI_ES_POWER_ATTACK_HYPER
		void ESSI_SI_ES_POWER_ATTACK_HYPER_FrameMoveFuture();
		void ESSI_SI_ES_POWER_ATTACK_HYPER_FrameMove();
		void ESSI_SI_ES_POWER_ATTACK_HYPER_EventProcess();

		//ESSI_SI1_ES_MEGA_SLASH
		void ESSI_SI1_ES_MEGA_SLASH_Init();
		void ESSI_SI1_ES_MEGA_SLASH_FrameMoveFuture();
		void ESSI_SI1_ES_MEGA_SLASH_FrameMove();
		void ESSI_SI1_ES_MEGA_SLASH_CameraMove();
		void ESSI_SI1_ES_MEGA_SLASH_EventProcess();

		//ESSI_SI1_ES_MEGA_SLASH_HYPER
		void ESSI_SI1_ES_MEGA_SLASH_HYPER_FrameMoveFuture();
		void ESSI_SI1_ES_MEGA_SLASH_HYPER_FrameMove();
		void ESSI_SI1_ES_MEGA_SLASH_HYPER_CameraMove();
		void ESSI_SI1_ES_MEGA_SLASH_HYPER_EventProcess();


		//ESSI_SI2_ES_FLAME_GEYSER
		void ESSI_SI2_ES_FLAME_GEYSER_Init();
		void ESSI_SI2_ES_FLAME_GEYSER_Start();
		//void ESSI_SI2_ES_FLAME_GEYSER_FrameMoveFuture();
		void ESSI_SI2_ES_FLAME_GEYSER_FrameMove();
		void ESSI_SI2_ES_FLAME_GEYSER_CameraMove();
		void ESSI_SI2_ES_FLAME_GEYSER_EventProcess();

		//ESSI_SI2_ES_FLAME_GEYSER_HYPER
		void ESSI_SI2_ES_FLAME_GEYSER_HYPER_Start();
		//void ESSI_SI2_ES_FLAME_GEYSER_HYPER_FrameMoveFuture();
		void ESSI_SI2_ES_FLAME_GEYSER_HYPER_FrameMove();
		void ESSI_SI2_ES_FLAME_GEYSER_HYPER_CameraMove();
		void ESSI_SI2_ES_FLAME_GEYSER_HYPER_EventProcess();


		//ESSI_SI3_ES_UNLIMITED_BLADE_LV1
		void ESSI_SI3_ES_UNLIMITED_BLADE_Init();
		void ESSI_SI3_ES_UNLIMITED_BLADE_FrameMoveFuture();
		void ESSI_SI3_ES_UNLIMITED_BLADE_FrameMove();
		void ESSI_SI3_ES_UNLIMITED_BLADE_CameraMove();
		void ESSI_SI3_ES_UNLIMITED_BLADE_EventProcess();

		//ESSI_SI3_ES_UNLIMITED_BLADE_HYPER
		void ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_Init();
		void ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_FrameMoveFuture();
		void ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_FrameMove();
		void ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_CameraMove();
		void ESSI_SI3_ES_UNLIMITED_BLADE_HYPER_EventProcess();



		//ESSI_SI1_ES_AIR_SLASH
		void ESSI_SI1_ES_AIR_SLASH_Init();
		void ESSI_SI1_ES_AIR_SLASH_FrameMoveFuture();
		void ESSI_SI1_ES_AIR_SLASH_FrameMove();
		void ESSI_SI1_ES_AIR_SLASH_CameraMove();
		void ESSI_SI1_ES_AIR_SLASH_EventProcess();

		//ESSI_SI1_ES_AIR_SLASH_HYPER
		void ESSI_SI1_ES_AIR_SLASH_HYPER_FrameMoveFuture();
		void ESSI_SI1_ES_AIR_SLASH_HYPER_FrameMove();
		void ESSI_SI1_ES_AIR_SLASH_HYPER_CameraMove();
		void ESSI_SI1_ES_AIR_SLASH_HYPER_EventProcess();

		//ESSI_SI1_ES_AIR_SLASH_NEXT
		void ESSI_SI1_ES_AIR_SLASH_NEXT_FrameMove();
		void ESSI_SI1_ES_AIR_SLASH_NEXT_EventProcess();

		//ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT
		void ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT_FrameMove();
		void ESSI_SI1_ES_AIR_SLASH_HYPER_NEXT_EventProcess();

		//ESSI_SI1_ES_ASSAULT_SLASH
		void ESSI_SI1_ES_ASSAULT_SLASH_Init();
		void ESSI_SI1_ES_ASSAULT_SLASH_Start();
		void ESSI_SI1_ES_ASSAULT_SLASH_FrameMoveFuture();
		void ESSI_SI1_ES_ASSAULT_SLASH_FrameMove();
		void ESSI_SI1_ES_ASSAULT_SLASH_CameraMove();
		void ESSI_SI1_ES_ASSAULT_SLASH_EventProcess();

		//ESSI_SI1_ES_ASSAULT_SLASH_HYPER
		void ESSI_SI1_ES_ASSAULT_SLASH_HYPER_FrameMoveFuture();
		void ESSI_SI1_ES_ASSAULT_SLASH_HYPER_FrameMove();
		void ESSI_SI1_ES_ASSAULT_SLASH_HYPER_CameraMove();
		void ESSI_SI1_ES_ASSAULT_SLASH_HYPER_EventProcess();


		//{{ dmlee 2009.6.2 obsolete!! ������� �ʴ� �ʻ��
		////ESSI_SI2_ES_LUNA_BLADE
		//void ESSI_SI2_ES_LUNA_BLADE_Init();
		//void ESSI_SI2_ES_LUNA_BLADE_FrameMoveFuture();
		//void ESSI_SI2_ES_LUNA_BLADE_FrameMove();
		//void ESSI_SI2_ES_LUNA_BLADE_CameraMove();
		//void ESSI_SI2_ES_LUNA_BLADE_EventProcess();

		////ESSI_SI2_ES_LUNA_BLADE_HYPER
		//void ESSI_SI2_ES_LUNA_BLADE_HYPER_FrameMoveFuture();
		//void ESSI_SI2_ES_LUNA_BLADE_HYPER_FrameMove();
		//void ESSI_SI2_ES_LUNA_BLADE_HYPER_CameraMove();
		//void ESSI_SI2_ES_LUNA_BLADE_HYPER_EventProcess();
		//}} dmlee 2009.6.2 obsolete!! ������� �ʴ� �ʻ��

		//ESSI_SI2_ES_FATAL_FURY
		void ESSI_SI2_ES_FATAL_FURY_Init();
		void ESSI_SI2_ES_FATAL_FURY_FrameMoveFuture();
		void ESSI_SI2_ES_FATAL_FURY_FrameMove();
		void ESSI_SI2_ES_FATAL_FURY_CameraMove();
		void ESSI_SI2_ES_FATAL_FURY_EventProcess();

		//ESSI_SI2_ES_FATAL_FURY_HYPER
		void ESSI_SI2_ES_FATAL_FURY_HYPER_FrameMoveFuture();
		void ESSI_SI2_ES_FATAL_FURY_HYPER_FrameMove();
		void ESSI_SI2_ES_FATAL_FURY_HYPER_CameraMove();
		void ESSI_SI2_ES_FATAL_FURY_HYPER_EventProcess();


		//ESSI_SI2_ES_SPIRAL_BLAST
		void ESSI_SI2_ES_SPIRAL_BLAST_Init();
		void ESSI_SI2_ES_SPIRAL_BLAST_FrameMoveFuture();
		void ESSI_SI2_ES_SPIRAL_BLAST_FrameMove();
		void ESSI_SI2_ES_SPIRAL_BLAST_CameraMove();
		void ESSI_SI2_ES_SPIRAL_BLAST_EventProcess();

		//ESSI_SI2_ES_SPIRAL_BLAST_HYPER
		void ESSI_SI2_ES_SPIRAL_BLAST_HYPER_FrameMoveFuture();
		void ESSI_SI2_ES_SPIRAL_BLAST_HYPER_FrameMove();
		void ESSI_SI2_ES_SPIRAL_BLAST_HYPER_CameraMove();
		void ESSI_SI2_ES_SPIRAL_BLAST_HYPER_EventProcess();


		//ESSI_SI3_ES_TRIPLE_GEYSER
		void ESSI_SI3_ES_TRIPLE_GEYSER_Init();
		void ESSI_SI3_ES_TRIPLE_GEYSER_FrameMoveFuture();
		void ESSI_SI3_ES_TRIPLE_GEYSER_FrameMove();
		void ESSI_SI3_ES_TRIPLE_GEYSER_CameraMove();
		void ESSI_SI3_ES_TRIPLE_GEYSER_EventProcess();

		//ESSI_SI3_ES_TRIPLE_GEYSER_HYPER
		void ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_FrameMoveFuture();
		void ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_FrameMove();
		void ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_CameraMove();
		void ESSI_SI3_ES_TRIPLE_GEYSER_HYPER_EventProcess();


		//ESSI_SI3_ES_ARMAGEDDON_BLADE
		void ESSI_SI3_ES_ARMAGEDDON_BLADE_Init();
		void ESSI_SI3_ES_ARMAGEDDON_BLADE_FrameMove();
		void ESSI_SI3_ES_ARMAGEDDON_BLADE_EventProcess();

		//ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER
		void ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER_FrameMove();
		void ESSI_SI3_ES_ARMAGEDDON_BLADE_HYPER_EventProcess();

		//ESSI_SI1_ES_WIND_BLADE
		void ESSI_SI1_ES_WIND_BLADE_Init();
		void ESSI_SI1_ES_WIND_BLADE_FrameMove();
		void ESSI_SI1_ES_WIND_BLADE_EventProcess();

		//ESSI_SI1_ES_WIND_BLADE_HYPER
		void ESSI_SI1_ES_WIND_BLADE_HYPER_FrameMove();
		void ESSI_SI1_ES_WIND_BLADE_HYPER_EventProcess();

		//ESSI_SI2_ES_RISING_SLASH
		void ESSI_SI2_ES_RISING_SLASH_Init();
		void ESSI_SI2_ES_RISING_SLASH_FrameMove();
		void ESSI_SI2_ES_RISING_SLASH_EventProcess();

		//ESSI_SI2_ES_RISING_SLASH_HYPER
		void ESSI_SI2_ES_RISING_SLASH_HYPER_FrameMove();
		void ESSI_SI2_ES_RISING_SLASH_HYPER_EventProcess();

		//ESSI_SI3_ES_STORM_BLADE
		void ESSI_SI3_ES_STORM_BLADE_Init();
		void ESSI_SI3_ES_STORM_BLADE_FrameMove();
		void ESSI_SI3_ES_STORM_BLADE_EventProcess();

		//ESSI_SI3_ES_STORM_BLADE_HYPER
		void ESSI_SI3_ES_STORM_BLADE_HYPER_FrameMove();
		void ESSI_SI3_ES_STORM_BLADE_HYPER_EventProcess();


		//ESSI_A_ELK_IMPACT_SMASH
		void ESSI_A_ELK_IMPACT_SMASH_Init();
		void ESSI_A_ELK_IMPACT_SMASH_EventProcess();

		//ESSI_A_ES_SWORD_WAVE
		void ESSI_A_ES_SWORD_WAVE_Init();
		void ESSI_A_ES_SWORD_WAVE_EventProcess();

		//ESSI_SI_A_ES_DEFENCE
		void ESSI_SI_A_ES_DEFENCE_READY_StartFuture();
		void ESSI_SI_A_ES_DEFENCE_READY_EventProcess();

		void ESSI_SI_A_ES_DEFENCE_WAIT_Start();
		void ESSI_SI_A_ES_DEFENCE_WAIT_FrameMove();
		void ESSI_SI_A_ES_DEFENCE_WAIT_EventProcess();

		void ESSI_SI_A_ES_DEFENCE_END_Start();
		void ESSI_SI_A_ES_DEFENCE_END_EventProcess();

		void ESSI_SI_A_ES_DEFENCE_DAMAGE_FrameMove();
		void ESSI_SI_A_ES_DEFENCE_DAMAGE_EventProcess();

		//ESSI_SI_P_ESK_AUTO_GUARD
		void ESSI_SI_P_ESK_AUTO_GUARD_Start();
		void ESSI_SI_P_ESK_AUTO_GUARD_EventProcess();
		void ESSI_SI_P_ESK_AUTO_GUARD_End();

		//ESSI_SI_SA_EMK_RISING_SLASH_WAVE
		void ESSI_SI_SA_EMK_RISING_SLASH_WAVE_Init();
		void ESSI_SI_SA_EMK_RISING_SLASH_WAVE_EventProcess();
		void ESSI_SI_SA_EMK_RISING_SLASH_WAVE_FrameMove();
		void ESSI_SI_SA_EMK_RISING_SLASH_WAVE_HYPER_FrameMove();

		//ESSI_SI_A_ERS_CRITICAL_SWORD
		void ESSI_SI_A_ERS_CRITICAL_SWORD_Init();
		void ESSI_SI_A_ERS_CRITICAL_SWORD_FrameMove();
		void ESSI_SI_A_ERS_CRITICAL_SWORD_EventProcess();

		//ESSI_SI_A_ERS_RUNE_OF_FIRE
		void ESSI_SI_A_ERS_RUNE_OF_FIRE_Init();
		void ESSI_SI_A_ERS_RUNE_OF_FIRE_EventProcess();
		void ESSI_SI_A_ERS_RUNE_OF_FIRE_FrameMove();

		//ESSI_SI_A_ERS_RUNE_OF_ICE
		void ESSI_SI_A_ERS_RUNE_OF_ICE_Init();
		void ESSI_SI_A_ERS_RUNE_OF_ICE_EventProcess();
		void ESSI_SI_A_ERS_RUNE_OF_ICE_FrameMove();

		//ESSI_SI_A_ETK_PIERCING_SWORD
		void ESSI_SI_A_ETK_PIERCING_SWORD_Init();
		void ESSI_SI_A_ETK_PIERCING_SWORD_EventProcess();
		void ESSI_SI_A_ETK_PIERCING_SWORD_StateStart();
		void ESSI_SI_A_ETK_PIERCING_SWORD_FrameMove();
		void ESSI_SI_A_ETK_PIERCING_SWORD_End();

		//ESSI_SI_A_EIS_FATAL_SLAP
		void ESSI_SI_A_EIS_FATAL_SLAP_Init();
		void ESSI_SI_A_EIS_FATAL_SLAP_EventProcess();

		//ESSI_A_ETK_SWORD_SHIELD
		void ESSI_A_ETK_SWORD_SHIELD_Init();
		void ESSI_A_ETK_SWORD_SHIELD_StateStart();
		void ESSI_A_ETK_SWORD_SHIELD_FrameMove();
		void ESSI_A_ETK_SWORD_SHIELD_EventProcess();

		//ESSI_A_ESK_WEAPON_BREAK
		void ESSI_A_ESK_WEAPON_BREAK_Init();
		void ESSI_A_ESK_WEAPON_BREAK_EventProcess();


		//COMMON_BUFF
		virtual void COMMON_BUFF_FrameMove();
		//ESSI_LEAP
		void ESSI_LEAP_StartFuture();
		void ESSI_LEAP_FrameMoveFuture();
		void ESSI_LEAP_EventProcess();
		void ESSI_LEAP_End();




		void ESSI_SI_A_ES_KICK_Start();
		void ESSI_SI_A_ES_KICK_EventProcess();


		void StandUpAttackFrontNoDownEventProcess();
		void StandUpAttackFrontNoDownEnd();

		void StandUpAttackBackNoDownEventProcess();
		void StandUpAttackBackNoDownEnd();




	void ESSI_SA_ESK_WINDMILL_START_Init();
	void ESSI_SA_ESK_WINDMILL_START_StateStart();
	void ESSI_SA_ESK_WINDMILL_START_FrameMove();
	void ESSI_SA_ESK_WINDMILL_START_EventProcess();
	void ESSI_SA_ESK_WINDMILL_START_StateEnd();

	void ESSI_SA_ESK_WINDMILL_START_HYPER_StateStart();
	void ESSI_SA_ESK_WINDMILL_START_HYPER_FrameMove();
	void ESSI_SA_ESK_WINDMILL_START_HYPER_EventProcess();
	void ESSI_SA_ESK_WINDMILL_START_HYPER_StateEnd();


	void ESSI_SA_ESK_WINDMILL_FrameMove();
	void ESSI_SA_ESK_WINDMILL_EventProcess();
	void ESSI_SA_ESK_WINDMILL_StateEnd();
	void ESSI_SA_ESK_WINDMILL_HYPER_FrameMove();
	void ESSI_SA_ESK_WINDMILL_HYPER_EventProcess();
	void ESSI_SA_ESK_WINDMILL_HYPER_StateEnd();


	void ESSI_SA_ESK_WINDMILL_END_FrameMove();
	void ESSI_SA_ESK_WINDMILL_END_EventProcess();
	void ESSI_SA_ESK_WINDMILL_END_HYPER_FrameMove();
	void ESSI_SA_ESK_WINDMILL_END_HYPER_EventProcess();

	void ESSI_SA_EMK_SWORD_FIRE_Init();
	void ESSI_SA_EMK_SWORD_FIRE_StartFuture();
	void ESSI_SA_EMK_SWORD_FIRE_FrameMoveFuture();
	void ESSI_SA_EMK_SWORD_FIRE_FrameMove();
	void ESSI_SA_EMK_SWORD_FIRE_EventProcess();
	void ESSI_SA_EMK_SWORD_FIRE_EndFuture();

	void ESSI_SA_EMK_SWORD_FIRE_HYPER_Init();
	void ESSI_SA_EMK_SWORD_FIRE_HYPER_StartFuture();
	void ESSI_SA_EMK_SWORD_FIRE_HYPER_FrameMoveFuture();
	void ESSI_SA_EMK_SWORD_FIRE_HYPER_FrameMove();
	void ESSI_SA_EMK_SWORD_FIRE_HYPER_EventProcess();
	void ESSI_SA_EMK_SWORD_FIRE_HYPER_EndFuture();

	void ESSI_SA_EMK_SWORD_FIRE_LANDING_FrameMove();
	void ESSI_SA_EMK_SWORD_FIRE_LANDING_EventProcess();
	void ESSI_SA_EMK_SWORD_FIRE_LANDING_End();

	void ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_FrameMove();
	void ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_EventProcess();
	void ESSI_SA_EMK_SWORD_FIRE_LANDING_HYPER_End();

	//ESSI_SA_EMK_BIG_BURST
	void ESSI_SA_EMK_BIG_BURST_Init();
	void ESSI_SA_EMK_BIG_BURST_FrameMoveFuture();
	void ESSI_SA_EMK_BIG_BURST_FrameMove();
	void ESSI_SA_EMK_BIG_BURST_EventProcess();
	void ESSI_SA_EMK_BIG_BURST_End();



	void ESSI_A_ESK_ARMOR_BREAK_Init();
	void ESSI_A_ESK_ARMOR_BREAK_StateStart();
	void ESSI_A_ESK_ARMOR_BREAK_FrameMove();
	void ESSI_A_ESK_ARMOR_BREAK_EventProcess();
	void ESSI_A_ESK_ARMOR_BREAK_StateEnd();

	void ESSI_A_ESK_ARMOR_BREAK_HYPER_FrameMove();
	void ESSI_A_ESK_ARMOR_BREAK_HYPER_EventProcess();
	void ESSI_A_ESK_ARMOR_BREAK_HYPER_StateEnd();










	void ESSI_A_ESK_PHOENIX_TALON_Init();
	void ESSI_A_ESK_PHOENIX_TALON_StateStart();
	void ESSI_A_ESK_PHOENIX_TALON_FrameMove();
	void ESSI_A_ESK_PHOENIX_TALON_Hyper_FrameMove();
	void ESSI_A_ESK_PHOENIX_TALON_EventProcess();
	void ESSI_A_ESK_PHOENIX_TALON_StateEnd();


	void ESSI_SA_ESK_DOUBLE_SLASH_FrameMove();
	void ESSI_SA_ESK_DOUBLE_SLASH_EventProcess();
	void ESSI_SA_ESK_DOUBLE_SLASH_StateEnd();

	void ESSI_SA_ESK_DOUBLE_SLASH_HYPER_FrameMove();







	// 2�� ����
	void ESSI_ELK_ZZZfrontZ_FrameMove();
	void ESSI_ELK_ZZZfrontZ_EventProcess();
	void ESSI_ELK_XXX_FrameMove();
	void ESSI_ELK_XXX_EventProcess();
	void ESSI_ELK_XXXX_FrameMove();
	void ESSI_ELK_XXXX_EventProcess();


	void ESSI_ERS_XZ_StateStartFuture();
	void ESSI_ERS_XZ_FrameMoveFuture();
	void ESSI_ERS_XZ_FrameMove();
	void ESSI_ERS_XZ_EventProcess();
	void ESSI_ERS_XZZ_FrameMove();
	void ESSI_ERS_XZZ_EventProcess();


	void ESSI_ERS_DASH_JUMP_XZ_StateStartFuture();
	void ESSI_ERS_DASH_JUMP_XZ_StateEndFuture();
	void ESSI_ERS_DASH_JUMP_XZ_FrameMoveFuture();
	void ESSI_ERS_DASH_JUMP_XZ_FrameMove();
	void ESSI_ERS_DASH_JUMP_XZ_EventProcess();


	void ESSI_ERS_DASH_JUMP_XZZ_FrameMove();
	void ESSI_ERS_DASH_JUMP_XZZ_EventProcess();




	void ESSI_SA_ELK_SONIC_BLADE_Init();
	void ESSI_SA_ELK_SONIC_BLADE_StateStart();
	void ESSI_SA_ELK_SONIC_BLADE_FrameMove();
	void ESSI_SA_ELK_SONIC_BLADE_EventProcess();
	void ESSI_SA_ELK_SONIC_BLADE_StateEnd();

	void ESSI_SA_ELK_SONIC_BLADE_HYPER_StateStart();
	void ESSI_SA_ELK_SONIC_BLADE_HYPER_FrameMove();
	void ESSI_SA_ELK_SONIC_BLADE_HYPER_EventProcess();
	void ESSI_SA_ELK_SONIC_BLADE_HYPER_StateEnd();


	void ESSI_SA_ERS_SPLASH_EXPLOSION_Init();
	void ESSI_SA_ERS_SPLASH_EXPLOSION_StateStart();
	void ESSI_SA_ERS_SPLASH_EXPLOSION_FrameMove();
	//void ESSI_SA_ERS_SPLASH_EXPLOSION_EventProcess();
	void ESSI_SA_ERS_SPLASH_EXPLOSION_StateEnd();

	void ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_StateStart();
	void ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_FrameMove();
	//void ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_EventProcess();
	void ESSI_SA_ERS_SPLASH_EXPLOSION_HYPER_StateEnd();


	void ESSI_A_ELK_ROLLING_SMASH_READY_Init();
	void ESSI_A_ELK_ROLLING_SMASH_READY_StateStart();
	void ESSI_A_ELK_ROLLING_SMASH_READY_FrameMove();
	void ESSI_A_ELK_ROLLING_SMASH_READY_EventProcess();
	void ESSI_A_ELK_ROLLING_SMASH_READY_StateEnd();

	void ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_StateStart();
	void ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_FrameMove();
	void ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_EventProcess();
	void ESSI_A_ELK_ROLLING_SMASH_READY_HYPER_StateEnd();



	void ESSI_A_ELK_ROLLING_SMASH_StateStartFuture();
	void ESSI_A_ELK_ROLLING_SMASH_FrameMove();
	void ESSI_A_ELK_ROLLING_SMASH_EventProcess();
	void ESSI_A_ELK_ROLLING_SMASH_End();

	void ESSI_A_ELK_ROLLING_SMASH_HYPER_StateStartFuture();
	void ESSI_A_ELK_ROLLING_SMASH_HYPER_EventProcess();





	void ESSI_A_ERS_SWORD_ENCHANT_Init();
	void ESSI_A_ERS_SWORD_ENCHANT_FrameMove();
	void ESSI_A_ERS_SWORD_ENCHANT_EventProcess();

//{{ JHKang / ������ / 2010/10/31 / �� �����̾� �糪 �����̵� �߰�
	/**
		@function
			- ESSI_SA_ERS_LUNA_BLADE_Init
			- ESSI_SA_ERS_LUNA_BLADE_Start
			- ESSI_SA_ERS_LUNA_BLADE_FrameMove
			- ESSI_SA_ERS_LUNA_BLADE_EventProcess
			- ESSI_SA_ERS_LUNA_BLADE_HYPER_Start
			- ESSI_SA_ERS_LUNA_BLADE_HYPER_FrameMove
			- ESSI_SA_ERS_LUNA_BLADE_HYPER_EventProcess
		@brief
			- ���õ� ���ҽ� �ε�
			- ���� ���� ��������
			- �ִϸ��̼� ����
			- �̺�Ʈ ����
	*/
	void ESSI_SA_ERS_LUNA_BLADE_Init();
	void ESSI_SA_ERS_LUNA_BLADE_FrameMove();
	void ESSI_SA_ERS_LUNA_BLADE_EventProcess();
//}} JHKang / ������ / 2010/10/31 / �� �����̾� �糪 �����̵� �߰�
	//{{ oasis907 : ����� [2010.11.5] // �ε� ����Ʈ - ���� ����
	void ESSI_SA_ELK_SAND_STORM_FrameMoveFuture();
	void ESSI_SA_ELK_SAND_STORM_FrameMove();
	void ESSI_SA_ELK_SAND_STORM_EventProcess();

	//}} oasis907 : ����� [2010.11.5] // �ε� ����Ʈ - ���� ����

	void ESSI_ETK_COMBO_ZZZ_StateStart();
	void ESSI_ETK_COMBO_ZZZ_FrameMove();
	void ESSI_ETK_COMBO_ZZZ_EventProcess();
	void ESSI_ETK_COMBO_ZZZ_StateEnd();
	void ESSI_ETK_COMBO_ZZZZ_FrameMove();
	void ESSI_ETK_COMBO_ZZZZ_EventProcess();
	void ESSI_ETK_COMBO_ZZZZ_StateEnd();

	void ESSI_ETK_COMBO_ZZZZZ_StateStart();
	void ESSI_ETK_COMBO_ZZZZZ_FrameMove();
	void ESSI_ETK_COMBO_ZZZZZ_EventProcess();
	void ESSI_ETK_COMBO_ZZZZZ_StateEnd();

	void ESSI_ETK_COMBO_XX_StateStart();
	void ESSI_ETK_COMBO_XX_FrameMove();
	void ESSI_ETK_COMBO_XX_EventProcess();
	void ESSI_ETK_COMBO_XX_StateEnd();

	void ESSI_ETK_COMBO_XXX_LOOP_StateStart();
	void ESSI_ETK_COMBO_XXX_LOOP_FrameMove();
	void ESSI_ETK_COMBO_XXX_LOOP_EventProcess();
	void ESSI_ETK_COMBO_XXX_LOOP_StateEnd();

	void ESSI_ETK_COMBO_XXXZ_FrameMove();
	void ESSI_ETK_COMBO_XXXZ_EventProcess();
	void ESSI_ETK_COMBO_XXXZ_StateEnd();

	void ESSI_ETK_COMBO_XXX_END_StateStart();
	void ESSI_ETK_COMBO_XXX_END_FrameMove();
	void ESSI_ETK_COMBO_XXX_END_EventProcess();
	void ESSI_ETK_COMBO_XXX_END_StateEnd();

	void ESSI_ETK_DASH_COMBO_ZZ_FrameMove();
	void ESSI_ETK_DASH_COMBO_ZZ_EventProcess();
	void ESSI_ETK_DASH_COMBO_ZZ_StateEnd();

	void ESSI_SI_A_ES_ROLLING_StateStart();
	void ESSI_SI_A_ES_ROLLING_StateStartFuture();
	void ESSI_SI_A_ES_ROLLING_FrameMove();
	void ESSI_SI_A_ES_ROLLING_EventProcess();

	void ESSI_SI_A_ES_ROLLING_END_StateStart();
	void ESSI_SI_A_ES_ROLLING_END_FrameMove();
	void ESSI_SI_A_ES_ROLLING_END_EventProcess();

	void ESSI_SI_SA_ETK_HARSH_CHASER_StateStart();
	void ESSI_SI_SA_ETK_HARSH_CHASER_FrameMove();
	void ESSI_SI_SA_ETK_HARSH_CHASER_EventProcess();
	void ESSI_SI_SA_ETK_HARSH_CHASER_End();

	void ESSI_SI_SA_ETK_HARSH_CHASER_PULL_FrameMove();
	void ESSI_SI_SA_ETK_HARSH_CHASER_PULL_EventProcess();

	void CompactCounterAttackStart();
	void CompactCounterAttackStartFuture();
	void CompactCounterAttackFrameMoveFuture();
	void CompactCounterAttackFrameMove();
	void CompactCounterAttackEventProcess();
	void CompactCounterAttackStateEnd();

	void CompactCounterGuardAttackStart();
	void CompactCounterGuardAttackStartFuture();
	void CompactCounterGuardAttackFrameMoveFuture();
	void CompactCounterGuardAttackFrameMove();
	void CompactCounterGuardAttackCameraMove();
	void CompactCounterGuardAttackEventProcess();
	void CompactCounterGuardAttackStateEnd();

	void ESSI_SI_SA_ETK_MAELSTORM_RAGE_Init();
	void ESSI_SI_SA_ETK_MAELSTORM_RAGE_StartFuture();
	void ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMoveFuture();
	void ESSI_SI_SA_ETK_MAELSTORM_RAGE_FrameMove();
	void ESSI_SI_SA_ETK_MAELSTORM_RAGE_EventProcess();
	void ESSI_SI_SA_ETK_MAELSTORM_RAGE_EndFuture();

	void ESSI_SI_SA_ETK_FINAL_STRIKE_Init();
	void ESSI_SI_SA_ETK_FINAL_STRIKE_FrameMove();
	void ESSI_SI_SA_ETK_FINAL_STRIKE_EventProcess();
	void ESSI_SI_SA_ETK_FINAL_STRIKE_End();

	void ESSI_SI_SA_ETK_SWORD_FALL_Init();
	void ESSI_SI_SA_ETK_SWORD_FALL_FrameMove();
	void ESSI_SI_SA_ETK_SWORD_FALL_EventProcess();
	void ESSI_SI_SA_ETK_SWORD_FALL_End();

	void ESSI_SI_SA_ETK_CRESCENT_CUT_Init();
	void ESSI_SI_SA_ETK_CRESCENT_CUT_FrameMove();
	void ESSI_SI_SA_ETK_CRESCENT_CUT_EventProcess();
	void ESSI_SI_SA_ETK_CRESCENT_CUT_End();

	void ESSI_SI_SA_ETK_PHANTOM_SWORD_Init();
	void ESSI_SI_SA_ETK_PHANTOM_SWORD_FrameMove();
	void ESSI_SI_SA_ETK_PHANTOM_SWORD_EventProcess();

	void ESSI_EIS_DASH_COMBO_XX_EventProcess();
	void ESSI_EIS_DASH_COMBO_XXX_EventProcess();
	void ESSI_EIS_DASHJUMP_COMBO_XX_EventProcess();
	void ESSI_EIS_DASHJUMP_COMBO_XXX_FrameMove();
	void ESSI_EIS_DASHJUMP_COMBO_XXX_EventProcess();
	void ESSI_EIS_DASHJUMP_COMBO_XXX_End();
	void ESSI_SI_A_EIS_SPIRAL_STING_FrameMove();
	void ESSI_SI_A_EIS_SPIRAL_STING_EventProcess();
	void ESSI_SI_SA_EIS_SWORD_BLASTING_FrameMove();
	void ESSI_SI_SA_EIS_SWORD_BLASTING_EventProcess();
	void ESSI_SI_SA_EIS_RAGE_CUTTER_FrameMove();
	void ESSI_SI_SA_EIS_RAGE_CUTTER_EventProcess();


	void SwordWaitStartFuture();
	void SwordWaitStart();
	void SwordWaitEventProcess();
	void PlayAnimationSwordWaitStart( CKTDGXSkinAnim* pXSkinAnim_, const FrameData& refFrameData_ );

	void WayOfSwordStateChange();
	void DestructionRageFrameMove();

	void DoStateDestructionRage();
	void DestructionRageStateStart();
	void DestructionRageStateFrameMove();
	void DestructionRageStateEventProcess();


	//#pragma region SI_FS_ELK_Gigantic_Slash
	void ESSI_HA_ELK_Gigantic_Slash_Init();
	//#pragma endregion �Ⱓƽ ������ : �ñر�

	//#pragma region SI_FS_ERS_Shining_Rune_Buster
	void ESSI_HA_ERS_Shining_Rune_Buster_Init();
	void ESSI_HA_ERS_Shining_Rune_Buster_EventProcess();

	//#pragma endregion ���̴� �� ����Ʈ : �ñر�

	//#pragma region SI_FS_EIS_Blade_Rain
	void ESSI_HA_EIS_Blade_Rain_Init();
	void ESSI_HA_EIS_Blade_Rain_EventProcess();
	//#pragma endregion �����̵� ���� : �ñر�


	void ShowActiveSkillCutInAndLight( const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ = false );
	virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

	virtual CX2GageData*	CreateGageData();
};



IMPLEMENT_INTRUSIVE_PTR( CX2GUElsword_SwordMan::CElswordGuardSkill );
