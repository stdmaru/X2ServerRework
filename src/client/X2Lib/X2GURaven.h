#pragma once

class CX2GURaven : public CX2GUUser
{
	public:
		enum RAVEN_STATE_ID
		{
			RSI_BASE = USI_END + 1,

			RSI_DASH_JUMP_POWER_LANDING,

			RSI_STANDUP_ROLLING_FRONT_FRONT,
			RSI_STANDUP_ROLLING_FRONT_BACK,
			RSI_STANDUP_ROLLING_BACK_FRONT,
			RSI_STANDUP_ROLLING_BACK_BACK,

			RSI_STANDUP_ATTACK_FRONT,
			RSI_STANDUP_ATTACK_BACK,

			RSI_SUPER_DASH_READY,
			RSI_SUPER_DASH,
			RSI_SUPER_DASH_END,

			RSI_COMBO_Z,
			RSI_COMBO_ZZ,
			RSI_COMBO_ZZZ,
			RSI_COMBO_ZZZZ,

			RSI_COMBO_ZZZX,
			RSI_COMBO_ZZX,
			RSI_COMBO_ZZXX,

			RSI_COMBO_X,
			RSI_COMBO_XX,
			RSI_COMBO_XXX,

			RSI_DASH_COMBO_Z,
			RSI_DASH_COMBO_ZZ,
			RSI_DASH_COMBO_X,

			RSI_DASH_JUMP_COMBO_Z,
			RSI_DASH_JUMP_COMBO_ZZ,
			RSI_DASH_JUMP_COMBO_ZZ_Landing,
			RSI_DASH_JUMP_COMBO_X,

			RSI_JUMP_COMBO_Z,
			RSI_JUMP_COMBO_X,

			RSI_SOUL_TAKER_COMBO_ZZZupZ,
			RSI_SOUL_TAKER_COMBO_XXZ,		// Z 8��Ÿ �غ�
			RSI_SOUL_TAKER_COMBO_XXZZ8,		// Z 8��Ÿ
			RSI_SOUL_TAKER_COMBO_XXZZ8Z,	// Z 8��Ÿ�Ŀ�
			RSI_SOUL_TAKER_DASH_COMBO_ZZZ,

			RSI_OVER_TAKER_COMBO_XXdownX,
			RSI_OVER_TAKER_COMBO_XXdownXdownX,
			RSI_OVER_TAKER_COMBO_XXdownXdownXdownX,
			RSI_OVER_TAKER_COMBO_XXfrontX,

			RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX,
			RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_LANDING,
			RSI_OVER_TAKER_DASH_JUMP_COMBO_ZXX,

//#ifdef WALL_JUMP_TEST
			RSI_WALL_LANDING,
//#endif WALL_JUMP_TEST

			RSI_SHADOW_STEP_THRUST,

			RSI_PARRYING_SMALL,
			RSI_PARRYING,

			RSI_STANDUP_ATTACK_FRONT_NO_DOWN,
			RSI_STANDUP_ATTACK_BACK_NO_DOWN,




			RSI_SA_ROT_VALKYRIESJAVELIN_LANDING,
			RSI_SA_ROT_VALKYRIESJAVELIN_HYPER_LANDING,

			RRF_DASH_JUMP_COMBO_XX,
			RRF_DASH_JUMP_COMBO_XX_LANDING,
			RRF_COMBO_ZZXFRONTX,
			RRF_COMBO_ZZXFRONTXX,

			RBM_COMBO_XZ,
			RBM_COMBO_XZZ,
			RBM_COMBO_XZZZ,
			RBM_COMBO_ZZZX_EXPLOSION,
			RBM_COMBO_ZZZXZ,

			SI_SA_RBM_WOLF_FANG_STARTLANDING,
			SI_SA_RBM_WOLF_FANG_STARTLANDING_HYPER,
			SI_SA_RBM_WOLF_FANG_HIT,
			SI_SA_RBM_WOLF_FANG_HIT_HYPER,
			SI_SA_RBM_WOLF_FANG_HITLANDING,
			//{{ JHKang / ������ / 2010/10/31 / ���Ż�� �нú� �߰�
			RSI_SHADOW_STEP,		/// ��� Ż���� ���� ������ ���� ���� �߰�
			//}} JHKang / ������ / 2010/10/31 / ���Ż�� �нú� �߰�

//{{ kimhc // 2010.11.11 // ����-���ϵ�����

			RSI_SA_RRF_WILD_CHARGING,			/// ����-���ϵ��������� �⸦ ������ �ִ� ����
			RSI_SA_RRF_WILD_CHARGE_DASH,		/// ����_���̵��������� ������ ������� ������ ����
//}} kimhc // 2010.11.11 // ����-���ϵ�����

			RSI_SHADOW_STEP_THRUST_ENHANCE,		/// ����-������ ���� �� x Ű ������ ������ ������Ʈ
			RSI_P_RRF_SHADOW_PUNISHER,			/// ����-������ ���� �� x Ű ������ ������ �۴ϼ�

			//RSI_COMBO_X,
			RSI_WEAPON_TAKER_COMBO_XX,
			RSI_WEAPON_TAKER_COMBO_XXX,
			RSI_WEAPON_TAKER_COMBO_XXX_LOOP,
			RSI_WEAPON_TAKER_COMBO_XXX_FINISH,

			//RSI_COMBO_ZZZX,
			RSI_WEAPON_TAKER_COMBO_ZZZXX,
			RSI_WEAPON_TAKER_COMBO_ZZZdownX,
			RSI_WEAPON_TAKER_COMBO_ZZZdownXX,

			RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZX,
			RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZXX,

			RSI_A_RWT_BARRAGE_ATTACK_STAND,
			RSI_A_RWT_BARRAGE_ATTACK_AIR,

			RSI_SA_RWT_BURSTING_BLADE_STAND,
			RSI_SA_RWT_BURSTING_BLADE_AIR,

			RSI_SA_RWT_REVOLVER_CANNON_LOOP,
			RSI_SA_RWT_REVOLVER_CANNON_FINISH,
			RSI_SA_RWT_REVOLVER_CANNON_END,
			RSI_SA_RWT_REVOLVER_CANNON_OVER_END,

			RSI_SA_RWT_HELLFIRE_GATLING_LOOP,
			RSI_SA_RWT_HELLFIRE_GATLING_END,
			RSI_SA_RWT_HELLFIRE_GATLING_OVER_END,

			RSI_SA_RWT_GIGA_PROMINENCE_LOOP,
			RSI_SA_RWT_GIGA_PROMINENCE_FINISH,
			RSI_SA_RWT_GIGA_PROMINENCE_END,
			RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH,
			RSI_SA_RWT_GIGA_PROMINENCE_OVER_END,

			//Dash Jump Combo XX
			RVC_DASH_JUMP_COMBO_X,
			RVC_DASH_JUMP_COMBO_XX_START,
			RVC_DASH_JUMP_COMBO_XX_LOOP,
			RVC_DASH_JUMP_COMBO_XX_FINISH,

			//Dash Combo ZZZZ
			RVC_DASH_COMBO_ZZZ,
			RVC_DASH_COMBO_ZZZPushZ_READY,
			RVC_DASH_COMBO_ZZZPushZ,
			RVC_DASH_COMBO_ZZZZ_FINISH,

			//Deadly Raid
			RVC_SI_SA_DEADLY_RAID_LOOP,
			RVC_SI_SA_DEADLY_RAID_LOOP_OVERHEAT,
			RVC_SI_SA_DEADLY_RAID_LOOP_END,
			RVC_SI_SA_DEADLY_RAID_LOOP_END_OVERHEAT,
			RVC_SI_SA_DEADLY_RAID_FINISH,
			RVC_SI_SA_DEADLY_RAID_FINISH_OVERHEAT,
			RVC_SI_SA_DEADLY_RAID_END,

			//Ignition Crow
			RVC_SI_SA_IGNITION_CROW_FIRE,
			RVC_SI_SA_IGNITION_CROW_FIRE_OVERHEAT,
			RVC_SI_SA_IGNITION_CROW_OVERHEAT_END,

			//Shadow Back Slide
			RVC_SI_P_SHADOW_BACK_SLIDE,
			RVC_SI_P_SHADOW_BACK_SLIDE_END,
			RVC_SI_P_SHADOW_STEP_FINISH,

			//Shadow Buster
			RVC_SI_P_SHADOW_BUSTER,

			//Bleeding Slicer
			RVC_SI_A_BLEEDING_SLICER_GROUND,
			RVC_SI_A_BLEEDING_SLICER_FLY,

			//Harpoon Spear Fire
			RSI_SA_RWT_HARPOON_SPEAR_FIRE,

			//Harpoon Spear Fire Overheat
			RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT,



			// ȭ����
			RSI_A_RWT_FLAME_SWORD_GROUND,
			RSI_A_RWT_FLAME_SWORD_FLY,

			// ������ ĳ�� HE
			RSI_SA_RWT_REVOLVER_CANNON_HE,
			RSI_SA_RWT_REVOLVER_CANNON_HE_LOOP,
			RSI_SA_RWT_REVOLVER_CANNON_HE_FINISH,
			RSI_SA_RWT_REVOLVER_CANNON_HE_OVER_END,
			RSI_SA_RWT_REVOLVER_CANNON_HE_END,

			// �̱״ϼ� ũ�ο� �νóʷ��̼�
			RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE,
			RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_OVERHEAT,
			RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END,

			// ����Ʈ ũ����
			RSI_A_RRF_LIMIT_CRUSHER_REVENGE_ATTACK,
			RSI_A_RRF_LIMIT_CRUSHER_AVOID_ATTACK,


			RSI_HA_RBM_EXTREM_BLADE_READY,			//�ͽ�Ʈ�� �����̵� FX ���� �غ�
			RSI_HA_RBM_EXTREM_BLADE_ATTACK,			//�ͽ�Ʈ�� �����̵� FX ����
			RSI_HA_RBM_EXTREM_BLADE_ATTACK_FINISH,	//�ͽ�Ʈ�� �����̵� FX �ǴϽ� ����

			//////////////////////////////////////////////////////////////////////////
			// ���ʿ� �߰����ּ���~ �� �Ʒ��� �׽�Ʈ�� �߰��� enum �Դϴ�


			RSI_TRANSFORMED,

//#ifdef PVP_BOSS_COMBAT_TEST
//			RSI_FROZEN,
//#endif PVP_BOSS_COMBAT_TEST

			RSI_A_RBM_SONIC_SLASH_FINISH,

		};

		enum RAVEN_MAJOR_PARTICLE_INSTANCE_ID
		{
			RAVEN_MAJOR_PII_GIGA_DRIVE_WIND_FIRST,
			RAVEN_MAJOR_PII_GIGA_DRIVE_WIND_SECOND,
			RAVEN_MAJOR_PII_GIGA_DRIVE_WIND_THIRD,
			RAVEN_MAJOR_PII_END,
		};

		enum RAVEN_MINOR_PARTICLE_INSTANCE_ID
		{
			RAVEN_MINOR_PII_BASE,		// kimhc // ����� ���̴� ���� ��� �ӽ÷� �־����
			RAVEN_MINOR_PII_END,
		};

		struct ArcEnemyData
		{
			D3DXVECTOR3					m_vOffsetPos;
			D3DXVECTOR3					m_vOffsetRotate;
            CX2DamageEffect::CEffectHandle       m_hEffect;

			ArcEnemyData();
			~ArcEnemyData();
		};

		/** @struct : WildChargeData
			@brief : ����-���ϵ����� ��ų���� �ʿ�� �ϴ� ����Ÿ�� ��� ���� ����ü
			@date  : 2010/11/16
		*/
        struct  WildChargeData;
        typedef x2::intrusive_ptr<WildChargeData> WildChargeDataPtr;	/// WildChargeData ����ü�� ����Ʈ ������ Ÿ��

		struct WildChargeData : x2::NonCopyable
		{
		private:
            unsigned                                        m_uRefCount;

			float m_fHpEachWildChargeFrame;				/// ���ϵ� �������� ���� ���� �� ���� �������� HP�� ������ �뵵(���� �������� �Ҹ�� HP�� �˾Ƴ��� ���� ���)
			float m_fTimeAfterFullCharging;				/// ���ϵ������� Ǯ���� �� ���� ��� �ð�
			float m_fPowerRateWildCharging;				/// ���ϵ��������� ������ �Ŀ�����Ʈ(����������)
			RAVEN_STATE_ID m_eSlotID_WildCharge;		/// ���ϵ������� ������ ��ų������ �˾Ƴ��� ���� �뵵( ��ų ������ ������Ʈ�� �ϳ��� �� ���� �Ǿ� ���� )
			bool m_bSlotB;								/// ���ϵ������� ������ ������ A����, B���� �����ϱ� ���ѿ뵵
			bool* m_pChargeKey;							/// ���ϵ� ������ Ű�� �����ִ��� �˾Ƴ��� ���� �뵵 (�޸𸮸� �����ϴ� ���� �ƴϹǷ� �������� �ʴ´�.)
			CX2EffectSet::Handle m_hHandleWildCharingEffectSet;	/// �������� ǥ���ϴ� ����Ʈ �ڵ�

			WildChargeData() : m_uRefCount(0), m_fHpEachWildChargeFrame( 0.f ),
				m_fTimeAfterFullCharging( 0.f ), m_fPowerRateWildCharging( 0.f ), m_eSlotID_WildCharge( RSI_BASE ),
				m_bSlotB( false ), m_pChargeKey( NULL )
                , m_hHandleWildCharingEffectSet( INVALID_EFFECTSET_HANDLE )
			{}

		public:

			static WildChargeDataPtr  CreateWildChargeData() { return WildChargeDataPtr( new WildChargeData ); }

			float GetHpEachWildChargeFrame() const { return m_fHpEachWildChargeFrame; }
			void SetHpEachWildChargeFrame(float fHpEachWildChargeFrame_) { m_fHpEachWildChargeFrame = fHpEachWildChargeFrame_; }

			float GetTimeAfterFullCharging() const { return m_fTimeAfterFullCharging; }
			void SetTimeAfterFullCharging(float fTimeAfterFullCharging_) { m_fTimeAfterFullCharging = fTimeAfterFullCharging_; }

			float GetPowerRateWildCharging() const { return m_fPowerRateWildCharging; }
			void SetPowerRateWildCharging(float fPowerRateWildCharging_) { m_fPowerRateWildCharging = fPowerRateWildCharging_; }

			RAVEN_STATE_ID GetSlotID_WildCharge() const { return m_eSlotID_WildCharge; }

			/** @function : SetSlotID_WildCharge
				@brief : ���� ��ų�� � ���Կ� �ִ����� �����ϱ� ���� �Լ�(��ų�� �� ������ �ϳ��� ���·� �Ǿ� �־� ���� ID�� ���ڰ����� ������ �ش�.)
				@param : ���� ID
			*/
			void SetSlotID_WildCharge(RAVEN_STATE_ID eStateID_) { m_eSlotID_WildCharge = eStateID_; }

			bool GetSlotB() const { return m_bSlotB; }
			void SetSlotB(bool bSlotB_) { m_bSlotB = bSlotB_; }

			bool GetChargeKey() const { return *m_pChargeKey; }
			void SetChargeKeyPointer(bool* pChargeKey_) { m_pChargeKey = pChargeKey_; }

			CX2EffectSet::Handle GetHandleWildCharingEffectSet() const { return m_hHandleWildCharingEffectSet; }
			CX2EffectSet::Handle& GetHandleReferenceWildCharingEffectSet() { return m_hHandleWildCharingEffectSet; }
			void SetHandleWildCharingEffectSet(CX2EffectSet::Handle hHandle_) { m_hHandleWildCharingEffectSet = hHandle_; }

            void    AddRef()    {   ++m_uRefCount; }
            void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }

		};


	private:
		CX2GURaven( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit );
		virtual ~CX2GURaven(void);


	public:
		static CX2GURaven* CreateGURaven( int unitIndex, int teamNum,
			lua_State*   pScriptLuaState,
			int frameBufferNum, CX2Unit* pUnit )
		{
			return new CX2GURaven( unitIndex, teamNum,
				pScriptLuaState,
				frameBufferNum, pUnit );
		};

		static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
		static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
		static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );

		virtual void		ReInit( bool bRandom = false, int startPosIndex = -1 );

		virtual void		DamageReact( CX2DamageManager::DamageData* pDamageData );
		virtual void		AttackResultPlayEffect();

		virtual void		Win();
		virtual void		Lose();
		virtual void		Draw();


		virtual void		InitEffect();
		virtual void		InitEnchantWeaponEffectUnitSpecific();

		void				InitializeRavenMajorParticleArray();	// ������ ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		virtual void		CreateMajorParticleArray();		// ������ ��ƼŬ �迭�� ���� // kimhc // 2010.11.5
		static void			AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
		CKTDGParticleSystem::CParticleEventSequence* SetRavenMajorParticleByEnum( RAVEN_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		ParticleEventSequenceHandle	GetHandleRavenMajorParticleByEnum( RAVEN_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const // ĳ���͸� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		{
			ASSERT( RAVEN_MAJOR_PII_END > eVal_ && RAVEN_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahRavenMajorParticleInstance[eVal_];
		}
		ParticleEventSequenceHandle& GetHandleReferenceRavenMajorParticleByEnum( RAVEN_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) // ĳ���͸� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ��� ���۷��� �ϳ��� ���� // kimhc // 2010.11.5
		{
			ASSERT( RAVEN_MAJOR_PII_END > eVal_ && RAVEN_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahRavenMajorParticleInstance[eVal_];
		}

		void				SetHandleRavenMajorParticleByEnum( RAVEN_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ ) // ĳ���͸� ���� ������ ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������ // kimhc // 2010.11.5
		{
			ASSERT( RAVEN_MAJOR_PII_END > eVal_ && RAVEN_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			m_ahRavenMajorParticleInstance[eVal_] = hHandle_;
		}
		void				DeleteRavenMajorParticle();


		void				InitializeRavenMinorParticleArray();	// ���������� ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		virtual void		CreateMinorParticleArray();		// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ // kimhc // 2010.11.5
		static void			AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
		CKTDGParticleSystem::CParticleEventSequence* SetRavenMinorParticleByEnum( RAVEN_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ���� // kimhc // 2010.11.5
		ParticleEventSequenceHandle	GetHandleRavenMinorParticleByEnum( RAVEN_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const	// ĳ���͸� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����	// kimhc // 2010.11.5
		{
			ASSERT( RAVEN_MINOR_PII_END > eVal_ && RAVEN_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahRavenMinorParticleInstance[eVal_];
		}
		ParticleEventSequenceHandle& GetHandleReferenceRavenMinorParticleByEnum( RAVEN_MINOR_PARTICLE_INSTANCE_ID eVal_ ) // ĳ���͸� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ��� ���۷��� �ϳ��� ����	// kimhc // 2010.11.5
		{
			ASSERT( RAVEN_MINOR_PII_END > eVal_ && RAVEN_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			return m_ahRavenMinorParticleInstance[eVal_];
		}
		void				SetHandleRavenMinorParticleByEnum( RAVEN_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )	// ĳ���͸� ���� ���̳� ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������	// kimhc // 2010.11.5
		{
			ASSERT( RAVEN_MINOR_PII_END > eVal_ && RAVEN_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
			m_ahRavenMinorParticleInstance[eVal_] = hHandle_;
		}
		void				DeleteRavenMinorParticle();

		virtual	void		DeleteMinorParticle();

		virtual bool		SpecialAttackNoStageChange( const CX2SkillTree::SkillTemplet* pSkillTemplet );
		virtual float		GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;

		virtual void		Init( bool bUseTeamPos_, int iStartPosIndex_ );
		void				DoHyper(bool bFullHyper);
		int					GetNasodBall() { return m_iNaosdBallLv; }
		int					GetNasodLvByGuage() { return (int)(m_fRageGauge / 20.f); }
		bool				GetParrying() { return m_bParrying; }
		bool				CanParrying() { return m_bCanParrying; }
		float				GetParryingTime() { return m_fParryingTime; }
		bool				CanParryingState();
		void				AddRageGuage(float fVal);
		float				GetRageGuage() { return m_fRageGauge; }
		bool				IsAttackNasodBall()
		{
			if( m_bDelayAttackNasodBall == false && m_bAttackNasodBall == false )
			{
				return false;
			}

			return true;
		}

		bool GetIsShadowStepIcon() { return m_bIsShadowStepIcon; }		/// ������ ���� ������ ���� ����
		bool GetActiveShadowStep() { return m_bActiveShadowStepCombo; }	/// ������ ���� �迭 ������ ����
		float GetEDTBleedingTime() { return m_fEDTBleedingTime; }		/// ġ���� ���� �ð� ��ȯ

		virtual bool		IsFullHyperMode() const { return m_bHyperAura; }
		virtual void		SetFullHyperMode( const bool bFullHyperMode_ ) { m_bHyperAura = bFullHyperMode_; }

		virtual void ApplyHyperModeBuff();
		virtual void HyperModeBuffEffectStart();
		virtual void HyperModeBuffEffectEnd();
		virtual void AddUnitSlashData( const CX2UnitSlashTraceManager::SLASH_TRACE_CONDITION eSlashTraceCondition_ );

	protected:
		virtual void ParseCommonRandomState();
		virtual void		InitState();
		void InitStateID();
		void InitStateCommon();
		void InitStateSwordTaker();
		void InitStateOverTaker();
		void InitStateWeaponTaker();

		virtual void		InitComponent();

		virtual void		InitPassiveSkillState();
		virtual void		SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ );
		virtual void		SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_);




		virtual void		CommonStateStartFuture();
		virtual void		CommonStateStart();
		virtual void		CommonFrameMoveFuture();
		virtual void		CommonFrameMove();
		virtual void		CommonEventProcess();
		virtual void		CommonStateEndFuture();
		virtual void		CommonStateEnd();
//{{ robobeg : 2008-10-28
		//virtual void		CommonRender();
        virtual RENDER_HINT CommonRender_Prepare();
//}} robobeg : 2008-10-28

		virtual bool		SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = CX2SkillTree::ASUT_GROUND );
		virtual void		NoStateChangeActionFrameMove();

		virtual void		ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ );
		virtual void		ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ );

		void				ClearArcEnemyData();


//{{ kimhc // 2010.11.3 // ���� - ��ȭ�� �г�
		virtual float		GetHyperModeTime() const;
//}} kimhc // 2010.11.3 // ���� - ��ȭ�� �г�

		void				ClearNasodBall();
		void				CreateNasodBall();
		void				DoParrying();
		void				RestoreNasodBall();
		virtual void		AttackResultByType( CX2DamageManager::DamageData &pDamageData );
		void				ChangePowerRateNasodBall();
		void				AttackNasodBall( );
		void				DoFrameMoveNasodBall();

		void SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
		void UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );

		//���̺� �뺴�� ó�ż� �нú�(SI_P_RWT_BEHAVIOR_OF_MERCENARY) ȿ���� �����Ű�� ���� �߰��� �Լ�
		virtual float GetReduceDebuffDurationRate(BUFF_TEMPLET_ID eBuffTempletID_);

		virtual int GetComboZStateID() const { return RSI_COMBO_Z; }
		virtual int GetComboXStateID() const { return RSI_COMBO_X; }
	protected:
		bool								m_bReAttackZ1Right;
		float								m_fReAttackZ1Time;
		float								m_fReDashJumpXTime;
		bool								m_bZZZX;


		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqCannonBlade;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqMagnumBlaster1;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMesh_MagnumBlaster2;


		//Major
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqPowerAssault[3];
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqSevenBurst[3];

		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqBloodyAccel;

		//Minor
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqSonicStab;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqSonicStab1;


		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshFlyingImpact;

		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshBerserkerBlade;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshBerserkerBlade1;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshBerserkerBlade2;
		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshBerserkerBlade3;


		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hMeshGuardianStrike1;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqGuardianStrike1;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqGuardianStrike2;
		CKTDGParticleSystem::CParticleEventSequenceHandle 	m_hSeqGuardianStrike3;

		int												m_iSuccessiveHit_OneZ;
		std::vector< ArcEnemyData* >					m_vecArcEnemyData;
		CKTDXTimer										m_TimerFlyingImpact;
		float											m_fArcEnemyEachTime;





		float			m_fShadowStepTimeLeft;
		bool			m_bEnableShadowStepThrust;
		float			m_fAttackPowerRateShadowStepThrust;
		float			m_fMPConsumeShadowStepThrust;

		bool			m_bEnableShadowStepThrustEnhance;			// ������ ������ ������Ʈ ��� ���� ����
		float			m_fAttackPowerRateShadowStepThrustEnhance;	// ������ ������ ������Ʈ ���ݷ�

		float			m_fChanceOfExplosiveNasodHand;				// ���ǿ� �����ϴ� ���ҵ� �ڵ� Ȯ��
		bool			m_bEnableShadowStepPunisher;				// ���ǿ� ������ �۴ϼ� ��� ���� ����
		float			m_fAttackPowerRateShadowStepPunisher;		// ���ǿ� ������ �۴ϼ� ���ݷ�

		float			m_fOverheatDamageLimitedRate;				// ��Ŀ�� ������Ʈ ������ ���� �ۼ������� ( �뺴�� �¸����� )
		float			m_fAddOverHeatBuffDurationTime;				// ��Ŀ�� ������Ʈ ���ӽð� ���� ( ������� �۵�! )
		int				m_iAddOverHeatLevel;						// ��Ŀ�� ������Ʈ ���� ���� ( ������� �۵�!)


		int				m_iBlackHoleHitCount;
		float			m_fBlackHoleTime;
		CX2EffectSet::Handle m_hBlackHoleEffectSet;

		CX2DamageManager::DAMAGE_TYPE	m_eDamageTypeRaven;
		float							m_fChangeDamageRelRaven;
		float							m_fPassiveCriticalRateRaven;

		D3DXVECTOR3	m_vXCrashPos;
		int				m_iRWTMaxComboLoopCount;	/// Ű�� ���� ������ �����ϰ� �� Ƚ���� ������Ʈ ���� Ƚ������ Ŀ���� ������Ʈ �ߵ� // kimhc // 2013-07-04
		int				m_iRWTComboLoopCount;		/// loop ���� ������Ʈ�� ����̳� loop ������� ��Ÿ�� (������Ʈ �ߵ� �� �� Ƚ���� m_iRWTMaxComboLoopCount�� ���Ѵ�.) // kimhc // 2013-07-04
		int				m_iOverheatKeyPressTime;
		bool			m_bOverheated;
		bool			m_bShowedOverheat;

		float			m_fAirSlicerDamageRel;
		float			m_fOverheatDamageRate;
		float			m_fReduceEDTDurationRate;
		float			m_fOverheatBuffTime;

		// �����ú����̵� Ÿ�ݸ޽�
		CKTDGXMeshPlayer::CXMeshInstanceHandle
						m_hBurstingBladeAttackBox;

		// �Ⱑ ���ι̳ͽ� ��ġ ������ ���� �����̳�
		std::vector< char >
						m_vecLeftSpot;
		std::vector< char >
						m_vecRightSpot;

		CX2EffectSet::Handle	m_hEffectDashJumpComboX;

		int													m_iRVCComboLoopCount;					/// ���� �޺� �ݺ� Ƚ��
		int													m_iRVCComboLoopInputCount;				/// ���� �޺� �ݺ� �Է� Ƚ��

        CX2DamageEffect::CEffectHandle   							m_hEffectFrameThrow;					/// ��� zzzz�� ȭ�� ��� ����Ʈ
		bool												m_bEndFrameThrow;						/// ȭ�� ��� ���� ���� ���� ����

		bool												m_bEnableShadowBackSlide;				/// ������ �� �����̵� �нú� ���� ����
		float												m_fShadowStepCooTime;					/// ������ ���� ��Ÿ��
		bool												m_bActiveShadowStepCombo;				/// ������ ���� �迭 ������ ����
		bool												m_bIsShadowStepIcon;					/// ������ ���� ������ ������ ����
		int													m_iShadowBackSlideLevel;				/// ������ �� �����̵� ��ų ����
		float												m_fShadowBusterPowerRate;				/// ������ ������ �Ŀ�����Ʈ
		float												m_fShadowBackSlideTimeLeft;				/// ������ �� �����̵� ���� �ð�
		float												m_fShadowStepFinishTimeLeft;			/// ������ ������ ���� ���� �ð�

		bool												m_bEnableSurvivalTechniqueOfMercenary;	/// �뺴�� ������ �нú� ���� ����
		bool												m_bActiveSurvivalTechniqueOfMercenary;	/// �뺴�� ������ �߻� ����

		float												m_fLimitOverHeatHP;						/// ���� ��Ʈ ������ HP ����ġ

		bool												m_bShowedOverheat1;						/// �ܰ��� ������Ʈ�� �̸�Ƽ�� ���� Ȯ�� ��ü
		bool												m_bShowedOverheat2;						/// �ܰ��� ������Ʈ�� �̸�Ƽ�� ���� Ȯ�� ��ü
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqOverHeatIcon;						/// �ܰ��� ������Ʈ�� �̸�Ƽ�� �ڵ�
		CKTDGParticleSystem::CParticleEventSequenceHandle	m_hSeqOverHeatOverlapIcon;				/// �ܰ��� ������Ʈ ��ø �̸�Ƽ�� �ڵ�

		float												m_fEDTBleedingTime;						/// ġ���� ���� �ð�


		bool												m_bIsHyperDeadlyRaid;					/// ���� ���鸮 ���̵� ���� ����

		float												m_fDamageDataChangeTime;
		float												m_fDamageDataChangeTime1;
		float												m_fDamageDataChangeTime2;

		CKTDGXMeshPlayer::CXMeshInstanceHandle				m_hOverHeatObject;						/// ���� ��Ʈ �ߵ��� �����Ǵ� ������

		CX2DamageEffect::CEffectHandle							    m_hEffectOverHeatFire1;					/// ���� ��Ʈ �ߵ��� �����Ǵ� ȭ��
		CX2DamageEffect::CEffectHandle							    m_hEffectOverHeatFire2;					/// ���� ��Ʈ �ߵ��� �����Ǵ� ȭ��


		float												m_fExtremBladeDelay;					/// �ͽ�Ʈ�� �����̵� ��Ʈ ����

	private:

		CKTDGParticleSystem::CParticleEventSequenceHandle		m_ahRavenMajorParticleInstance[RAVEN_MAJOR_PII_END];
		CKTDGParticleSystem::CParticleEventSequenceHandle		m_ahRavenMinorParticleInstance[RAVEN_MINOR_PII_END];

	protected:
		//RSI_DIE_FRONT
		void RSI_DIE_FRONT_StartFuture();
		void RSI_DIE_FRONT_Start();
		void RSI_DIE_FRONT_FrameMove();
		void RSI_DIE_FRONT_EventProcess();

		//RSI_DIE_BACK
		void RSI_DIE_BACK_StartFuture();
		void RSI_DIE_BACK_Start();
		void RSI_DIE_BACK_FrameMove();
		void RSI_DIE_BACK_EventProcess();

		//RSI_WAIT
		void RSI_WAIT_StartFuture();
		void RSI_WAIT_Start();
		void RSI_WAIT_EventProcess();

		//RSI_WALK
		void RSI_WALK_Start();
		void RSI_WALK_EventProcess();

		//RSI_JUMP_READY
		void RSI_JUMP_READY_EventProcess();

		//RSI_JUMP_UP
		void RSI_JUMP_UP_EventProcess();

		//RSI_JUMP_DOWN
		void RSI_JUMP_DOWN_EventProcess();

		//RSI_JUMP_LANDING
		void RSI_JUMP_LANDING_EventProcess();

		//RSI_DASH
		void RSI_DASH_EventProcess();

		//RSI_DASH_END
		void RSI_DASH_END_StartFuture();
		void RSI_DASH_END_Start();
		void RSI_DASH_END_FrameMove();
		void RSI_DASH_END_EventProcess();

		//RSI_DASH_JUMP
		void RSI_DASH_JUMP_StartFuture();
		void RSI_DASH_JUMP_Start();
		void RSI_DASH_JUMP_FrameMoveFuture();
		void RSI_DASH_JUMP_EventProcess();
		void RSI_DASH_JUMP_EndFuture();

		//RSI_DASH_JUMP_LANDING
		void RSI_DASH_JUMP_LANDING_EventProcess();

		//RSI_DASH_JUMP_POWER_LANDING
		void RSI_DASH_JUMP_POWER_LANDING_StartFuture();
		void RSI_DASH_JUMP_POWER_LANDING_Start();
		void RSI_DASH_JUMP_POWER_LANDING_EventProcess();

		//RSI_HYPER_MODE
		void HyperModeFrameMove();
		void RidingHyperModeFrameMove();
		void CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ = false );

		//RSI_DAMAGE_GROGGY
		//void RSI_DAMAGE_GROGGY_FrameMoveFuture();
		//void RSI_DAMAGE_GROGGY_EventProcess();

		//RSI_DAMAGE_SMALL_FRONT
		void RSI_DAMAGE_SMALL_FRONT_EventProcess();

		//RSI_DAMAGE_SMALL_BACK
		//{{ JHKang / ������ / 2010/11/02 / ���Ż�� �нú�
		void RSI_DAMAGE_SMALL_BACK_Start();
		//}} JHKang / ������ / 2010/11/02 / ���Ż�� �нú�

		//RSI_DAMAGE_SMALL_BACK
		void RSI_DAMAGE_SMALL_BACK_EventProcess();

		//RSI_DAMAGE_BIG_FRONT
		void RSI_DAMAGE_BIG_FRONT_Start();
		void RSI_DAMAGE_BIG_FRONT_EventProcess();

		//RSI_DAMAGE_BIG_BACK
		void RSI_DAMAGE_BIG_BACK_Start();
		void RSI_DAMAGE_BIG_BACK_EventProcess();

		//RSI_DAMAGE_DOWN_FRONT
		void RSI_DAMAGE_DOWN_FRONT_FrameMove();
		void RSI_DAMAGE_DOWN_FRONT_EventProcess();

		//RSI_DAMAGE_DOWN_BACK
		void RSI_DAMAGE_DOWN_BACK_FrameMove();
		void RSI_DAMAGE_DOWN_BACK_EventProcess();

		//RSI_DAMAGE_AIR_DOWN_LANDING
		void RSI_DAMAGE_AIR_DOWN_LANDING_EventProcess();

		//RSI_STANDUP_ROLLING_FRONT_FRONT
		void RSI_STANDUP_ROLLING_FRONT_FRONT_EventProcess();
		void RSI_STANDUP_ROLLING_FRONT_FRONT_End();

		//RSI_STANDUP_ROLLING_FRONT_BACK
		void RSI_STANDUP_ROLLING_FRONT_BACK_EventProcess();
		void RSI_STANDUP_ROLLING_FRONT_BACK_End();

		//RSI_STANDUP_ROLLING_BACK_FRONT
		void RSI_STANDUP_ROLLING_BACK_FRONT_EventProcess();
		void RSI_STANDUP_ROLLING_BACK_FRONT_End();

		//RSI_STANDUP_ROLLING_BACK_BACK
		void RSI_STANDUP_ROLLING_BACK_BACK_EventProcess();
		void RSI_STANDUP_ROLLING_BACK_BACK_End();

		//RSI_STANDUP_ATTACK_FRONT
		void RSI_STANDUP_ATTACK_FRONT_FrameMove();
		void RSI_STANDUP_ATTACK_FRONT_EventProcess();
		void RSI_STANDUP_ATTACK_FRONT_End();

		//RSI_STANDUP_ATTACK_BACK
		void RSI_STANDUP_ATTACK_BACK_FrameMove();
		void RSI_STANDUP_ATTACK_BACK_EventProcess();
		void RSI_STANDUP_ATTACK_BACK_End();

		//RSI_SUPER_DASH_READY
		void RSI_SUPER_DASH_READY_StateStart();
		void RSI_SUPER_DASH_READY_EventProcess();

		//RSI_SUPER_DASH
		// ���� ����
		void RSI_SUPER_DASH_Init();
		void RSI_SUPER_DASH_StateStart();
		void RSI_SUPER_DASH_FrameMoveFuture();
		void RSI_SUPER_DASH_EventProcess();

		//RSI_SUPER_DASH_END
		void RSI_SUPER_DASH_END_FrameMove();
		void RSI_SUPER_DASH_END_EventProcess();

		//RSI_COMBO_Z
		void RSI_COMBO_Z_EventProcess();

		//RSI_COMBO_ZZ
		void RSI_COMBO_ZZ_EventProcess();

		//RSI_COMBO_ZZZ
		void RSI_COMBO_ZZZ_EventProcess();

		//RSI_COMBO_ZZZZ
		void RSI_COMBO_ZZZZ_FrameMoveFuture();
		void RSI_COMBO_ZZZZ_EventProcess();

		//RSI_COMBO_ZZZX
		void RSI_COMBO_ZZZX_Start();
		void RSI_COMBO_ZZZX_FrameMove();
		void RSI_COMBO_ZZZX_EventProcess();

		//RSI_COMBO_ZZX
		void RSI_COMBO_ZZX_EventProcess();

		//RSI_COMBO_ZZXX
		void RSI_COMBO_ZZXX_FrameMoveFuture();
		void RSI_COMBO_ZZXX_EventProcess();

		//RSI_COMBO_X
		void RSI_COMBO_X_EventProcess();

		//RSI_COMBO_XX
		void RSI_COMBO_XX_EventProcess();

		//RSI_COMBO_XXX
		void RSI_COMBO_XXX_FrameMove();
		void RSI_COMBO_XXX_EventProcess();

		//RSI_DASH_COMBO_Z
		void RSI_DASH_COMBO_Z_EventProcess();

		//RSI_DASH_COMBO_ZZ
		void RSI_DASH_COMBO_ZZ_EventProcess();

		//RSI_DASH_COMBO_X
		void RSI_DASH_COMBO_X_FrameMove();
		void RSI_DASH_COMBO_X_EventProcess();

		//RSI_DASH_JUMP_COMBO_Z
		void RSI_DASH_JUMP_COMBO_Z_EventProcess();

		//RSI_DASH_JUMP_COMBO_ZZ
		void RSI_DASH_JUMP_COMBO_ZZ_StateStart();
		void RSI_DASH_JUMP_COMBO_ZZ_FrameMoveFuture();
		void RSI_DASH_JUMP_COMBO_ZZ_EventProcess();

		//RSI_DASH_JUMP_COMBO_ZZ_Landing
		void RSI_DASH_JUMP_COMBO_ZZ_Landing_StartFuture();
		void RSI_DASH_JUMP_COMBO_ZZ_Landing_EventProcess();

		//RSI_DASH_JUMP_COMBO_X
		void RSI_DASH_JUMP_COMBO_X_Init();
		void RSI_DASH_JUMP_COMBO_X_Start();
		void RSI_DASH_JUMP_COMBO_X_FrameMoveFuture();
		void RSI_DASH_JUMP_COMBO_X_FrameMove();
		void RSI_DASH_JUMP_COMBO_X_EventProcess();

		//RSI_JUMP_COMBO_Z
		void RSI_JUMP_COMBO_Z_FrameMoveFuture();
		void RSI_JUMP_COMBO_Z_EventProcess();

		//RSI_JUMP_COMBO_X
		void RSI_JUMP_COMBO_X_StateStart();
		void RSI_JUMP_COMBO_X_FrameMoveFuture();
		void RSI_JUMP_COMBO_X_EventProcess();






		//RSI_SI1_MAXIMUM_CANNON
		void RSI_SI1_MAXIMUM_CANNON_Init();
		void RSI_SI1_MAXIMUM_CANNON_FrameMove();
		void RSI_SI1_MAXIMUM_CANNON_EventProcess();
		//RSI_SI1_MAXIMUM_CANNON_HYPER
		void RSI_SI1_MAXIMUM_CANNON_HYPER_FrameMove();
		void RSI_SI1_MAXIMUM_CANNON_HYPER_EventProcess();


		//RSI_SI2_POWER_ASSAULT
		void RSI_SI2_POWER_ASSAULT_Init();
		void RSI_SI2_POWER_ASSAULT_FrameMoveFuture();
		void RSI_SI2_POWER_ASSAULT_FrameMove();
		void RSI_SI2_POWER_ASSAULT_EventProcess();
		//RSI_SI2_POWER_ASSAULT_HYPER
		void RSI_SI2_POWER_ASSAULT_HYPER_FrameMoveFuture();
		void RSI_SI2_POWER_ASSAULT_HYPER_FrameMove();
		void RSI_SI2_POWER_ASSAULT_HYPER_EventProcess();


		//RSI_SI1_EARTH_BREAKER
		void RSI_SI1_EARTH_BREAKER_Init();
		void RSI_SI1_EARTH_BREAKER_FrameMove();
		void RSI_SI1_EARTH_BREAKER_EventProcess();
		//RSI_SI1_EARTH_BREAKER_HYPER
		void RSI_SI1_EARTH_BREAKER_HYPER_FrameMove();
		void RSI_SI1_EARTH_BREAKER_HYPER_EventProcess();


		//RSI_SI2_CANNON_BLADE
		void RSI_SI2_CANNON_BLADE_Init();
		void RSI_SI2_CANNON_BLADE_FrameMove();
		void RSI_SI2_CANNON_BLADE_EventProcess();
		//RSI_SI2_CANNON_BLADE_HYPER
		void RSI_SI2_CANNON_BLADE_HYPER_FrameMove();
		void RSI_SI2_CANNON_BLADE_HYPER_EventProcess();



		//RSI_SI3_SEVEN_BURST
		void RSI_SI3_SEVEN_BURST_Init();
		void RSI_SI3_SEVEN_BURST_FrameMoveFuture();
		void RSI_SI3_SEVEN_BURST_FrameMove();
		void RSI_SI3_SEVEN_BURST_EventProcess();
		//RSI_SI3_SEVEN_BURST_HYPER
		void RSI_SI3_SEVEN_BURST_HYPER_FrameMoveFuture();
		void RSI_SI3_SEVEN_BURST_HYPER_FrameMove();
		void RSI_SI3_SEVEN_BURST_HYPER_EventProcess();

		//COMMON_BUFF
		virtual void COMMON_BUFF_FrameMove();

		void RSI_SOUL_TAKER_DASH_COMBO_ZZ_EventProcess();

		void RSI_SOUL_TAKER_COMBO_ZZZupZ_FrameMoveFuture();
		void RSI_SOUL_TAKER_COMBO_ZZZupZ_EventProcess();

		void RSI_SOUL_TAKER_COMBO_XXZ_EventProcess();

		void RSI_SOUL_TAKER_COMBO_XXZZ8_StateStart();
		void RSI_SOUL_TAKER_COMBO_XXZZ8_FrameMove();
		void RSI_SOUL_TAKER_COMBO_XXZZ8_EventProcess();
		void RSI_SOUL_TAKER_COMBO_XXZZ8Z_EventProcess();

		void RSI_SOUL_TAKER_DASH_COMBO_ZZZ_EventProcess();



		void RSI_OVER_TAKER_COMBO_XXdownX_FrameMoveFuture();
		void RSI_OVER_TAKER_COMBO_XXdownX_EventProcess();

		void RSI_OVER_TAKER_COMBO_XXdownXdownX_FrameMoveFuture();
		void RSI_OVER_TAKER_COMBO_XXdownXdownX_EventProcess();

		void RSI_OVER_TAKER_COMBO_XXdownXdownXdownX_FrameMoveFuture();
		void RSI_OVER_TAKER_COMBO_XXdownXdownXdownX_EventProcess();


		void RSI_OVER_TAKER_COMBO_XXfrontX_Init();
		void RSI_OVER_TAKER_COMBO_XXfrontX_FrameMove();
		void RSI_OVER_TAKER_COMBO_XXfrontX_EventProcess();


		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_FrameMoveFuture();
		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_FrameMove();
		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_EventProcess();

		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_LANDING_StartFuture();
		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_LANDING_Start();
		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZX_LANDING_EventProcess();

		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZXX_FrameMove();
		void RSI_OVER_TAKER_DASH_JUMP_COMBO_ZXX_EventProcess();

		void RRF_DASH_JUMP_COMBO_X_Init();
		void RRF_DASH_JUMP_COMBO_X_Start();
		void RRF_DASH_JUMP_COMBO_X_FrameMoveFuture();
		void RRF_DASH_JUMP_COMBO_X_FrameMove();
		void RRF_DASH_JUMP_COMBO_X_EventProcess();

		void RRF_DASH_JUMP_COMBO_XX_Init();
		void RRF_DASH_JUMP_COMBO_XX_FrameMoveFuture();
		void RRF_DASH_JUMP_COMBO_XX_FrameMove();
		void RRF_DASH_JUMP_COMBO_XX_EventProcess();

		void RRF_DASH_JUMP_COMBO_XX_LANDING_Start();
		void RRF_DASH_JUMP_COMBO_XX_LANDING_FrameMove();
		void RRF_DASH_JUMP_COMBO_XX_LANDING_EventProcess();

		void RRF_COMBO_ZZXFRONTX_Start();
		void RRF_COMBO_ZZXFRONTX_FrameMoveFuture();
		void RRF_COMBO_ZZXFRONTX_FrameMove();
		void RRF_COMBO_ZZXFRONTX_EventProcess();
		void RRF_COMBO_ZZXFRONTX_End();

		void RRF_COMBO_ZZXFRONTXX_FrameMove();
		void RRF_COMBO_ZZXFRONTXX_EventProcess();

		void RBM_Combo_XZ_EventProcess();
		void RBM_Combo_XZZ_EventProcess();
		void RBM_Combo_XZZZ_FrameMoveFuture();
		void RBM_Combo_XZZZ_EventProcess();

		void RBM_COMBO_ZZZX_Init();
		void RBM_COMBO_ZZZX_StateStart();
		void RBM_COMBO_ZZZX_FrameMove();
		void RBM_COMBO_ZZZX_EventProcess();
		void RBM_COMBO_ZZZXZ_Start();
		void RBM_COMBO_ZZZXZ_EventProcess();
		void RBM_COMBO_ZZZX_EXPLOSION_StateStart();

		void RBM_COMBO_ZZZX_EXPLOSION_FrameMove();
		void RBM_COMBO_ZZZX_EXPLOSION_EventProcess();

		void SI_SA_RRF_X_CRASH_Init();
		void SI_SA_RRF_X_CRASH_FrameMove();
		void SI_SA_RRF_X_CRASH_EventProcess();

		void SI_A_RBM_ONE_FLASH_Init();
		void SI_A_RBM_ONE_FLASH_StateStart();
		void SI_A_RBM_ONE_FLASH_EventProcess();

		void SI_SA_RBM_WOLF_FANG_START_Init();
		void SI_SA_RBM_WOLF_FANG_START_StateStart();
		void SI_SA_RBM_WOLF_FANG_START_FrameMove();
		void SI_SA_RBM_WOLF_FANG_START_EventProcess();
		void SI_SA_RBM_WOLF_FANG_STARTLANDING_EventProcess();
		void SI_SA_RBM_WOLF_FANG_HIT_EventProcess();
		void SI_SA_RBM_WOLF_FANG_HITLANDING_EventProcess();

		// �ʻ��
		void RSI_SI1_SOUL_TAKER_BERSERKER_BLADE_Init();
		void RSI_SI1_SOUL_TAKER_BERSERKER_BLADE_FrameMove();
		void RSI_SI1_SOUL_TAKER_BERSERKER_BLADE_EventProcess();
		void RSI_SI1_SOUL_TAKER_BERSERKER_BLADE_HYPER_FrameMove();
		void RSI_SI1_SOUL_TAKER_BERSERKER_BLADE_HYPER_EventProcess();


		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_Init();
		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_FrameMove();
		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_EventProcess();
		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_End();
		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_HYPER_FrameMove();
		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_HYPER_EventProcess();
		void RSI_SI2_SOUL_TAKER_FLYING_IMPACT_HYPER_End();



		void RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_FrameMoveFuture();
		void RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_FrameMove();
		void RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_EventProcess();

		void RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_HYPER_FrameMoveFuture();
		void RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_HYPER_FrameMove();
		void RSI_SI3_SOUL_TAKER_HYPERSONIC_STAB_HYPER_EventProcess();



		// �ʻ��
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_Init();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_StateStartFuture();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_StateStart();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_FrameMove();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_EventProcess();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_StateEnd();

		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_HYPER_StateStartFuture();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_HYPER_StateStart();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_HYPER_FrameMove();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_HYPER_EventProcess();
		void RSI_SI1_OVER_TAKER_CHARGED_BOLT_HYPER_StateEnd();



		void RSI_SI2_OVER_TAKER_ARC_ENEMY_Init();
		void RSI_SI2_OVER_TAKER_ARC_ENEMY_FrameMove();
		void RSI_SI2_OVER_TAKER_ARC_ENEMY_EventProcess();
		void RSI_SI2_OVER_TAKER_ARC_ENEMY_HYPER_FrameMove();
		void RSI_SI2_OVER_TAKER_ARC_ENEMY_HYPER_EventProcess();


		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_Init();
		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_StateStartFuture();
		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_FrameMove();
		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_EventProcess();

		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_HYPER_StateStartFuture();
		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_HYPER_FrameMove();
		void RSI_SI3_OVER_TAKER_GUARDIAN_STRIKE_HYPER_EventProcess();

		void RSI_SI_A_RF_SHADOW_STEP_Init();
		void RSI_SI_A_RF_SHADOW_STEP_StartFuture();
		void RSI_SI_A_RF_SHADOW_STEP_Start();
		void RSI_SI_A_RF_SHADOW_STEP_FrameMove();
		void RSI_SI_A_RF_SHADOW_STEP_EventProcess();
		void RSI_SI_A_RF_SHADOW_STEP_End();



		void RSI_SHADOW_STEP_THRUST_Init();
		void RSI_SHADOW_STEP_THRUST_Start();
		void RSI_SHADOW_STEP_THRUST_EventProcess();



		void RSI_STANDUP_ATTACK_FRONT_NO_DOWN_FrameMove();
		void RSI_STANDUP_ATTACK_FRONT_NO_DOWN_EventProcess();
		void RSI_STANDUP_ATTACK_FRONT_NO_DOWN_End();


		void RSI_STANDUP_ATTACK_BACK_NO_DOWN_FrameMove();
		void RSI_STANDUP_ATTACK_BACK_NO_DOWN_EventProcess();
		void RSI_STANDUP_ATTACK_BACK_NO_DOWN_End();






		void RSI_SA_RST_SHOCKWAVE_Init();
		void RSI_SA_RST_SHOCKWAVE_StateStart();
		void RSI_SA_RST_SHOCKWAVE_FrameMove();
		void RSI_SA_RST_SHOCKWAVE_EventProcess();
		void RSI_SA_RST_SHOCKWAVE_StateEnd();

		void RSI_SA_RST_SHOCKWAVE_HYPER_Init();
		void RSI_SA_RST_SHOCKWAVE_HYPER_StateStart();
		void RSI_SA_RST_SHOCKWAVE_HYPER_FrameMove();
		void RSI_SA_RST_SHOCKWAVE_HYPER_EventProcess();
		void RSI_SA_RST_SHOCKWAVE_HYPER_StateEnd();

		void RSI_SA_ROT_VALKYRIESJAVELIN_Init();
		void RSI_SA_ROT_VALKYRIESJAVELIN_StateStart();
		void RSI_SA_ROT_VALKYRIESJAVELIN_FrameMove();
		void RSI_SA_ROT_VALKYRIESJAVELIN_EventProcess();
		void RSI_SA_ROT_VALKYRIESJAVELIN_StateEnd();

		void RSI_SA_ROT_VALKYRIESJAVELIN_HYPER_Init();
		void RSI_SA_ROT_VALKYRIESJAVELIN_HYPER_StateStart();
		void RSI_SA_ROT_VALKYRIESJAVELIN_HYPER_FrameMove();
		void RSI_SA_ROT_VALKYRIESJAVELIN_HYPER_EventProcess();
		void RSI_SA_ROT_VALKYRIESJAVELIN_HYPER_StateEnd();



		void RSI_SA_ROT_VALKYRIESJAVELIN_LANDING_FrameMove();
		void RSI_SA_ROT_VALKYRIESJAVELIN_LANDING_EventProcess();

		void RSI_SA_ROT_VALKYRIESJAVELIN_LANDING_HYPER_FrameMove();
		void RSI_SA_ROT_VALKYRIESJAVELIN_LANDING_HYPER_EventProcess();





		void RSI_A_RST_CUT_TENDON_Init();
		void RSI_A_RST_CUT_TENDON_StateStart();
		void RSI_A_RST_CUT_TENDON_EventProcess();

		void RSI_A_ROT_WEAPON_BREAK_Init();
		void RSI_A_ROT_WEAPON_BREAK_StateStart();
		void RSI_A_ROT_WEAPON_BREAK_EventProcess();

		void RSI_A_ROT_WEAPON_BREAK_HYPER_EventProcess();


		void RSI_A_RST_BLOODY_ACCEL_Init();
		void RSI_A_RST_BLOODY_ACCEL_FrameMove();
		void RSI_A_RST_BLOODY_ACCEL_EventProcess();

		void RSI_A_ROT_NUCLEAR_Init();
		void RSI_A_ROT_NUCLEAR_FrameMove();
		void RSI_A_ROT_NUCLEAR_EventProcess();
		void RSI_A_ROT_NUCLEAR_Hyper_FrameMove();
		void RSI_A_ROT_NUCLEAR_Hyper_EventProcess();

		void SI_A_RRF_GROUND_IMPACT_Init();
		void SI_A_RRF_GROUND_IMPACT_FrameMove();
		void SI_A_RRF_GROUND_IMPACT_EventProcess();

		//{{ JHKang / ������ / 2010/10/31 / ���Ż�� �нú� �߰�
		/** @function RBM_EMERGENCY_ESCAPE
			@brief �нú� ����
		*/
		void RBM_EMERGENCY_ESCAPE();

		/** @function RBM_EMERGENCY_ESCAPE_StartFuture
			@brief ������ ���ܰ��� ������ ����
		*/
		void RBM_EMERGENCY_ESCAPE_StartFuture();
		//}} JHKang / ������ / 2010/10/31 / ���Ż�� �нú� �߰�

//{{ kimhc // 2010.11.10 // ����-�Ⱑ����̺�, ����-���ϵ�����

		// ����-�Ⱑ����̺�
		void RSI_SA_RBM_GIGA_DRIVE_Init();
		void RSI_SA_RBM_GIGA_DRIVE_FrameMove();
		void RSI_SA_RBM_GIGA_DRIVE_EventProcess();

		// ����-���ϵ����� ���� ������Ʈ
		void RSI_SA_RRF_WILD_CHARGE_READY_Init();
		void RSI_SA_RRF_WILD_CHARGE_READY_StateStart();
		void RSI_SA_RRF_WILD_CHARGE_READY_StateStartFuture();
		void RSI_SA_RRF_WILD_CHARGE_READY_FrameMove();
		void RSI_SA_RRF_WILD_CHARGE_READY_EventProcess();

		// ����-���ϵ����� ������� ������Ʈ
		void RSI_SA_RRF_WILD_CHARGING_StateStart();
		void RSI_SA_RRF_WILD_CHARGING_FrameMove();
		void RSI_SA_RRF_WILD_CHARGING_EventProcess();
		void RSI_SA_RRF_WILD_CHARGING_StateEnd();

		// ����-���ϵ����� ������ �����ϴ� ������Ʈ
		void RSI_SA_RRF_WILD_CHARGE_DASH_FrameMove();
		void RSI_SA_RRF_WILD_CHARGE_DASH_EventProcess();

//}} kimhc // 2010.11.10 // ����-�Ⱑ����̺�, ����-���ϵ�����

		void RSI_PARRYING_StartFuture();
		void RSI_PARRYING_Start();
		void RSI_PARRYING_EventProcess();
		void RSI_PARRYING_StateEnd();

		void RSI_RWT_LOSE2_FrameMove();

		void RWT_COMBO_X_Init();
		void RWT_COMBO_X_FrameMove();
		void RWT_COMBO_X_EventProcess();
		void RSI_WEAPON_TAKER_COMBO_XX_FrameMoveFuture();
		void RSI_WEAPON_TAKER_COMBO_XX_EventProcess();

		void RSI_WEAPON_TAKER_COMBO_XXX_StateStart();
		void RSI_WEAPON_TAKER_COMBO_XXX_FrameMove();
		void RSI_WEAPON_TAKER_COMBO_XXX_EventProcess();

		void RSI_WEAPON_TAKER_COMBO_XXX_LOOP_StateStart();
		void RSI_WEAPON_TAKER_COMBO_XXX_LOOP_FrameMove();
		void RSI_WEAPON_TAKER_COMBO_XXX_LOOP_EventProcess();
		void RSI_WEAPON_TAKER_COMBO_XXX_LOOP_StateEnd();

		void RSI_WEAPON_TAKER_COMBO_XXX_FINISH_FrameMove();
		void RSI_WEAPON_TAKER_COMBO_XXX_FINISH_EventProcess();

		void RWT_COMBO_ZZZX_Start();
		void RWT_COMBO_ZZZX_FrameMove();
		void RWT_COMBO_ZZZX_EventProcess();
		void RWT_COMBO_ZZZX_EndFuture();
		void RSI_WEAPON_TAKER_COMBO_ZZZXX_FrameMoveFuture();
		void RSI_WEAPON_TAKER_COMBO_ZZZXX_EventProcess();
		void RSI_WEAPON_TAKER_COMBO_ZZZdownX_FrameMove();
		void RSI_WEAPON_TAKER_COMBO_ZZZdownX_EventProcess();
		void RSI_WEAPON_TAKER_COMBO_ZZZdownXX_FrameMove();
		void RSI_WEAPON_TAKER_COMBO_ZZZdownXX_EventProcess();

		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZX_StateStart();
		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZX_FrameMove();
		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZX_EventProcess();
		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZX_StateEnd();

		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZXX_StateStart();
		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZXX_FrameMove();
		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZXX_EventProcess();
		void RSI_WEAPON_TAKER_DASH_JUMP_COMBO_ZXX_StateEnd();


		void RSI_A_RF_BREAKING_FIST_Init();
		void RSI_A_RF_BREAKING_FIST_StateStart();
		void RSI_A_RF_BREAKING_FIST_FrameMove();
		void RSI_A_RF_BREAKING_FIST_EventProcess();
		void RSI_A_RF_BREAKING_FIST_StateEnd();
		void RSI_A_RF_BREAKING_FIST_StateEndFuture();

		void RSI_A_RWT_BARRAGE_ATTACK_Init();
		void RSI_A_RWT_BARRAGE_ATTACK_StateStartFuture();
		void RSI_A_RWT_BARRAGE_ATTACK_STAND_EventProcess();
		void RSI_A_RWT_BARRAGE_ATTACK_STAND_FrameMove();
		void RSI_A_RWT_BARRAGE_ATTACK_AIR_EventProcess();
		void RSI_A_RWT_BARRAGE_ATTACK_AIR_FrameMove();

		void RSI_SA_RWT_BURSTING_BLADE_Init();
		void RSI_SA_RWT_BURSTING_BLADE_StateStartFuture();
		void RSI_SA_RWT_BURSTING_BLADE_ATTACK_StateStart();
		void RSI_SA_RWT_BURSTING_BLADE_ATTACK_FrameMove();
		void RSI_SA_RWT_BURSTING_BLADE_ATTACK_EventProcess();
		void RSI_SA_RWT_BURSTING_BLADE_ATTACK_StateEnd();

		void RSI_SA_RWT_REVOLVER_CANNON_Init();
		void RSI_SA_RWT_REVOLVER_CANNON_StateStart();
		void RSI_SA_RWT_REVOLVER_CANNON_FrameMove();
		void RSI_SA_RWT_REVOLVER_CANNON_EventProcess();
		void RSI_SA_RWT_REVOLVER_CANNON_StateEnd();
		void RSI_SA_RWT_REVOLVER_CANNON_LOOP_StateStart();
		void RSI_SA_RWT_REVOLVER_CANNON_LOOP_FrameMove();
		void RSI_SA_RWT_REVOLVER_CANNON_LOOP_EventProcess();
		void RSI_SA_RWT_REVOLVER_CANNON_LOOP_StateEnd();
		void RSI_SA_RWT_REVOLVER_CANNON_FINISH_EventProcess();
		void RSI_SA_RWT_REVOLVER_CANNON_END_EventProcess();

		void RSI_SA_RWT_HARPOON_SPEAR_Init();
		void RSI_SA_RWT_HARPOON_SPEAR_StateStart();
		void RSI_SA_RWT_HARPOON_SPEAR_FrameMove();
		void RSI_SA_RWT_HARPOON_SPEAR_EventProcess();
		void RSI_SA_RWT_HARPOON_SPEAR_StateEnd();

		/// �Ϲ�
		void RSI_SA_RWT_HARPOON_SPEAR_FIRE_EventProcess();

		/// ���� ��Ʈ
		void RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT_StateStart();
		void RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT_FrameMove();
		void RSI_SA_RWT_HARPOON_SPEAR_FIRE_OVERHEAT_EventProcess();

		void RSI_SA_RWT_HELLFIRE_GATLING_Init();
		void RSI_SA_RWT_HELLFIRE_GATLING_StateStart();
		void RSI_SA_RWT_HELLFIRE_GATLING_FrameMove();
		void RSI_SA_RWT_HELLFIRE_GATLING_EventProcess();
		void RSI_SA_RWT_HELLFIRE_GATLING_StateEnd();
		void RSI_SA_RWT_HELLFIRE_GATLING_LOOP_StateStart();
		void RSI_SA_RWT_HELLFIRE_GATLING_LOOP_FrameMove();
		void RSI_SA_RWT_HELLFIRE_GATLING_LOOP_EventProcess();
		void RSI_SA_RWT_HELLFIRE_GATLING_LOOP_StateEnd();
		void RSI_SA_RWT_HELLFIRE_GATLING_END_EventProcess();

		void RSI_SA_RWT_GIGA_PROMINENCE_Init();
		void RSI_SA_RWT_GIGA_PROMINENCE_StateStart();
		void RSI_SA_RWT_GIGA_PROMINENCE_FrameMove();
		void RSI_SA_RWT_GIGA_PROMINENCE_EventProcess();
		void RSI_SA_RWT_GIGA_PROMINENCE_StateEnd();
		void RSI_SA_RWT_GIGA_PROMINENCE_LOOP_StateStart();
		void RSI_SA_RWT_GIGA_PROMINENCE_LOOP_EventProcess();
		void RSI_SA_RWT_GIGA_PROMINENCE_LOOP_FrameMove();
		void RSI_SA_RWT_GIGA_PROMINENCE_LOOP_StateEnd();
		void RSI_SA_RWT_GIGA_PROMINENCE_FINISH_FrameMove();
		void RSI_SA_RWT_GIGA_PROMINENCE_FINISH_EventProcess();
		void RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH_StateStart();
		void RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH_FrameMove();
		void RSI_SA_RWT_GIGA_PROMINENCE_OVER_FINISH_EventProcess();
		void RSI_SA_RWT_GIGA_PROMINENCE_END_EventProcess();

		void RSI_SA_RWT_GIGA_PROMINENCE_OVER_END_StateStart();
		void RSI_SA_RWT_GIGA_PROMINENCE_OVER_END_EventProcess();

		void _GetGigaProminencePosition( OUT bool& bRight, OUT float& fDistance );

		void _ShowOverheatIcon( int iSize = 0 );

		bool _CanOverheat( bool bShowMassage = true );

		void _BurnOverheatHP( const CX2DamageManager::DamageData& AttDamageData, float fDecreaseHPRate = -1.f, float fDecreaseHPRate2 = 1.f );

		void _SetOverheat( bool bOn = true, float fDuration = -1.f );


		/// RVC_DASH_JUMP_COMBO_X				- �Ϲ����� ����
		void RVC_DASH_JUMP_COMBO_X_Init();
		void RVC_DASH_JUMP_COMBO_X_Start();
		void RVC_DASH_JUMP_COMBO_X_FrameMoveFuture();
		void RVC_DASH_JUMP_COMBO_X_FrameMove();
		void RVC_DASH_JUMP_COMBO_X_EventProcess();

		/// RVC_DASH_JUMP_COMBO_XX_START		- ���� ��Ʈ ����
		void RVC_DASH_JUMP_COMBO_XX_START_StateStart();
		void RVC_DASH_JUMP_COMBO_XX_START_FrameMoveFuture();
		void RVC_DASH_JUMP_COMBO_XX_START_FrameMove();
		void RVC_DASH_JUMP_COMBO_XX_START_EventProcess();
		void RVC_DASH_JUMP_COMBO_XX_START_StateEnd();

		/// RVC_DASH_JUMP_COMBO_XX_LOOP		- ���� ��Ʈ ������
		void RVC_DASH_JUMP_COMBO_XX_LOOP_StateStart();
		void RVC_DASH_JUMP_COMBO_XX_LOOP_FrameMoveFuture();
		void RVC_DASH_JUMP_COMBO_XX_LOOP_FrameMove();
		void RVC_DASH_JUMP_COMBO_XX_LOOP_EventProcess();
		void RVC_DASH_JUMP_COMBO_XX_LOOP_StateEnd();

		/// RVC_DASH_JUMP_COMBO_XX_FINISH		- ���� ��Ʈ ��Ÿ
		void RVC_DASH_JUMP_COMBO_XX_FINISH_StateStart();
		void RVC_DASH_JUMP_COMBO_XX_FINISH_FrameMoveFuture();
		void RVC_DASH_JUMP_COMBO_XX_FINISH_FrameMove();
		void RVC_DASH_JUMP_COMBO_XX_FINISH_EventProcess();
		void RVC_DASH_JUMP_COMBO_XX_FINISH_StateEnd();

		/// RVC_DASH_COMBO_ZZZ				- ���� ��Ʈ �� ���Ƽ��� ����
		void RVC_DASH_COMBO_ZZZ_StateStart();
		void RVC_DASH_COMBO_ZZZ_EventProcess();
		void RVC_DASH_COMBO_ZZZ_StateEnd();

		/// RVC_DASH_COMBO_ZZZPushZ_READY		- ȭ�� ��� �� �غ� ����
		void RVC_DASH_COMBO_ZZZPushZ_READY_StateStart();
		void RVC_DASH_COMBO_ZZZPushZ_READY_EventProcess();
		void RVC_DASH_COMBO_ZZZPushZ_READY_StateEnd();

		/// RVC_DASH_COMBO_ZZZPushZ			- ���� ��Ʈ ( ȭ�� ��� )
		void RVC_DASH_COMBO_ZZZPushZ_StateStart();
		void RVC_DASH_COMBO_ZZZPushZ_FrameMove();
		void RVC_DASH_COMBO_ZZZPushZ_EventProcess();
		void RVC_DASH_COMBO_ZZZPushZ_StateEnd();

		/// RVC_DASH_COMBO_ZZZZ_FINISH		- ���� ��Ʈ ������ ����
		void RVC_DASH_COMBO_ZZZZ_FINISH_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID
		void RVC_SI_SA_DEADLY_RAID_Init();
		void RVC_SI_SA_DEADLY_RAID_StateStart();
		void RVC_SI_SA_DEADLY_RAID_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_LOOP
		void RVC_SI_SA_DEADLY_RAID_LOOP_StateStart();
		void RVC_SI_SA_DEADLY_RAID_LOOP_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_LOOP_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_LOOP_OVERHEAT
		void RVC_SI_SA_DEADLY_RAID_LOOP_OVERHEAT_StateStart();
		void RVC_SI_SA_DEADLY_RAID_LOOP_OVERHEAT_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_LOOP_OVERHEAT_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_LOOP_END
		void RVC_SI_SA_DEADLY_RAID_LOOP_END_StateStart();
		void RVC_SI_SA_DEADLY_RAID_LOOP_END_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_LOOP_END_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_LOOP_END_OVERHEAT
		void RVC_SI_SA_DEADLY_RAID_LOOP_END_OVERHEAT_StateStart();
		void RVC_SI_SA_DEADLY_RAID_LOOP_END_OVERHEAT_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_LOOP_END_OVERHEAT_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_FINISH
		void RVC_SI_SA_DEADLY_RAID_FINISH_StateStart();
		void RVC_SI_SA_DEADLY_RAID_FINISH_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_FINISH_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_FINISH_OVERHEAT
		void RVC_SI_SA_DEADLY_RAID_FINISH_OVERHEAT_StateStart();
		void RVC_SI_SA_DEADLY_RAID_FINISH_OVERHEAT_FrameMove();
		void RVC_SI_SA_DEADLY_RAID_FINISH_OVERHEAT_EventProcess();

		/// RVC_SI_SA_DEADLY_RAID_END
		void RVC_SI_SA_DEADLY_RAID_END_EventProcess();

		/// RVC_SI_SA_IGNITION_CROW
		void RVC_SI_SA_IGNITION_CROW_Init();
		void RVC_SI_SA_IGNITION_CROW_StateStart();
		void RVC_SI_SA_IGNITION_CROW_FrameMove();
		void RVC_SI_SA_IGNITION_CROW_EventProcess();
		void RVC_SI_SA_IGNITION_CROW_StateEnd();

		/// RVC_SI_SA_IGNITION_CROW
		void RVC_SI_SA_IGNITION_CROW_FIRE_FrameMove();
		void RVC_SI_SA_IGNITION_CROW_FIRE_EventProcess();

		void RVC_SI_SA_IGNITION_CROW_FIRE_OVERHEAT_StateStart();
		void RVC_SI_SA_IGNITION_CROW_FIRE_OVERHEAT_FrameMove();
		void RVC_SI_SA_IGNITION_CROW_FIRE_OVERHEAT_EventProcess();

		void RVC_SI_SA_IGNITION_CROW_OVERHEAT_END_EventProcess();

		/// RVC_SI_P_SHADOW_BACK_SLIDE
		void RVC_SI_P_SHADOW_BACK_SLIDE_Init();
		void RVC_SI_P_SHADOW_BACK_SLIDE_StartFuture();
		void RVC_SI_P_SHADOW_BACK_SLIDE_Start();
		void RVC_SI_P_SHADOW_BACK_SLIDE_FrameMove();
		void RVC_SI_P_SHADOW_BACK_SLIDE_EventProcess();
		void RVC_SI_P_SHADOW_BACK_SLIDE_End();

		/// RVC_SI_P_SHADOW_STEP_FINISH
		void RVC_SI_P_SHADOW_STEP_FINISH_Init();
		void RVC_SI_P_SHADOW_STEP_FINISH_StartFuture();
		void RVC_SI_P_SHADOW_STEP_FINISH_Start();
		void RVC_SI_P_SHADOW_STEP_FINISH_FrameMove();
		void RVC_SI_P_SHADOW_STEP_FINISH_EventProcess();
		void RVC_SI_P_SHADOW_STEP_FINISH_End();

		/// RVC_SI_P_SHADOW_BACK_SLIDE_END
		void RVC_SI_P_SHADOW_BACK_SLIDE_END_FrameMoveFuture();
		void RVC_SI_P_SHADOW_BACK_SLIDE_END_EventProcess();

		/// RVC_SI_P_SHADOW_BUSTER
		void RVC_SI_P_SHADOW_BUSTER_Init();
		void RVC_SI_P_SHADOW_BUSTER_Start();
		void RVC_SI_P_SHADOW_BUSTER_FrameMove();
		void RVC_SI_P_SHADOW_BUSTER_EventProcess();

		/// RVC_SI_A_BLEEDING_SLICE
		void RVC_SI_A_BLEEDING_SLICER_Init();
		void RVC_SI_A_BLEEDING_SLICER_StateStart();
		void RVC_SI_A_BLEEDING_SLICER_EventProcess();

		/// RVC_SI_A_BLEEDING_SLICER_GROUND
		void RVC_SI_A_BLEEDING_SLICER_GROUND_StateStart();
		void RVC_SI_A_BLEEDING_SLICER_GROUND_FrameMove();
		void RVC_SI_A_BLEEDING_SLICER_GROUND_EventProcess();
		void RVC_SI_A_BLEEDING_SLICER_GROUND_StateEnd();

		/// RVC_SI_A_BLEEDING_SLICER_AIR
		void RVC_SI_A_BLEEDING_SLICER_AIR_StateStart();
		void RVC_SI_A_BLEEDING_SLICER_AIR_FrameMove();
		void RVC_SI_A_BLEEDING_SLICER_AIR_FrameMoveFuture();
		void RVC_SI_A_BLEEDING_SLICER_AIR_EventProcess();
		void RVC_SI_A_BLEEDING_SLICER_AIR_StateEnd();

		/// ���鸮 ���̵�
		bool	PossibleDeadlyRaidLoop();											/// ���鸮 ���̵� ���� ��Ʈ ���� ���� �� �̸�Ƽ�� ����
		bool	ChangeDeadlyRaidLoop();												/// ���鸮 ���̵� �ݺ� State ���� ���� ����
		wstring DeadlyRaidLoopDamageEffectName();									/// ���鸮 ���̵� �ݺ� State ���� ����� DamageEffect �̸� ��ȯ

		/// ������ �� �����̵�
		void SetShadowStepAttackSkill( RAVEN_STATE_ID eStateID = RSI_SHADOW_STEP_THRUST );			/// ������ ����,�� �����̵� ���� ���� ���� ����
		void InitShadowStep();														/// ������ ���� �迭 �ʱ�ȭ �Լ�

		/// ���� ��Ʈ �߻��� ���� ������
		void CreateOverHeatObject();
		void DeleteOverHeatObject();


		void ShowActiveSkillCutInAndLight( const WCHAR* szBoneName_, const float fTimeToShow_, const UINT uiCutInIndex_, const bool bOnlyLight_ = false );
		virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

		void CheckAndDoReversalSoldier();
		void CheckAndDoServivalTechniqueOfMercenary();

//{{ kimhc // 2010.11.12 // ����-���ϵ�����
		WildChargeDataPtr m_WildChargeDataPtr; /// ���ϵ����� ��ų ������
//}} kimhc // 2010.11.12 // ����-���ϵ�����

		bool							m_bLockOnNpc;
		UidType							m_iLockOnUid;

		bool							m_bParrying;
		bool							m_bCanParrying;
		float							m_fParryingTime;
		bool							m_bAttackNasodBall;
		bool							m_bDelayAttackNasodBall;
		float							m_fDelayAttackTime;
		int								m_iNaosdBallLv;
		float							m_fNasodBallAttackTime;
		KProtectedType<float>			m_fRageGauge;
		ArcEnemyData*					m_pNasodBall;
		bool							m_bHyperAura;
		CX2EffectSet::Handle			m_hHyperAuraEffectset;
		CKTDGParticleSystem::CParticleEventSequenceHandle m_hSeqHyperBall;
		float							m_fOldHp;

		RAVEN_STATE_ID					m_eRavenCurrentStage;		/// �����̵� ������ ZZZXZZZ �޺� ĵ���� Ŭ�� ����ȭ�� ���߱� ����, ĵ���� ������Ʈ�� ����

		/// RSI_A_RF_MEGADRILL_BREAK �ް��帱 �극��ũ
		void RSI_A_RF_MEGADRILL_BREAK_Init();
		void RSI_A_RF_MEGADRILL_BREAK_FrameMoveFuture();
		void RSI_A_RF_MEGADRILL_BREAK_FrameMove();
		void RSI_A_RF_MEGADRILL_BREAK_EventProcess();
		void RSI_A_RF_MEGADRILL_BREAK_End();

		/// SI_A_RBM_SONIC_SLASH �Ҵ� ������
		void RSI_A_RBM_SONIC_SLASH_LOOP_Init();
		void RSI_A_RBM_SONIC_SLASH_LOOP_StateStart();
		void RSI_A_RBM_SONIC_SLASH_LOOP_FrameMove();
		void RSI_A_RBM_SONIC_SLASH_LOOP_EventProcess();
		void RSI_A_RBM_SONIC_SLASH_FINISH_EventProcess();

		/// RSI_SA_RST_SHOCKWAVE_FRONT ��ũ ���̺� - ��
		void RSI_SA_RST_SHOCKWAVE_FRONT_Init();
		void RSI_SA_RST_SHOCKWAVE_FRONT_StateStart();
		void RSI_SA_RST_SHOCKWAVE_FRONT_FrameMove();
		void RSI_SA_RST_SHOCKWAVE_FRONT_EventProcess();
		void RSI_SA_RST_SHOCKWAVE_FRONT_StateEnd();

		/// RSI_SA_RST_SHOCKWAVE_FRONT �Ⱑ ����̺� - ��
		void RSI_SA_RBM_GIGA_DRIVE_BURST_Init();
		void RSI_SA_RBM_GIGA_DRIVE_BURST_StateStart();
		void RSI_SA_RBM_GIGA_DRIVE_BURST_FrameMove();
		void RSI_SA_RBM_GIGA_DRIVE_BURST_EventProcess();
		void RSI_SA_RBM_GIGA_DRIVE_BURST_StateEnd();

		/// RSI_SA_ROT_CHARGED_BOLT_BLOOD ������ ��Ʈ - ���� ������
		void RSI_SA_ROT_CHARGED_BOLT_BLOOD_Init();
		void RSI_SA_ROT_CHARGED_BOLT_BLOOD_FrameMove();
		void RSI_SA_ROT_CHARGED_BOLT_BLOOD_EventProcess();


		// RSI_A_RWT_FLAME_SWORD ȭ����, ������
		void RSI_A_RWT_FLAME_SWORD_Init ();
		void RSI_A_RWT_FLAME_SWORD_StateStart ();
		void RSI_A_RWT_FLAME_SWORD_EventProcess ();


		// RSI_A_RWT_FLAME_SWORD ȭ���� ���󿡼� ���, ������
		void RSI_A_RWT_FLAME_SWORD_GROUND_StateStart ();
		void RSI_A_RWT_FLAME_SWORD_GROUND_EventProcess ();

		// RSI_A_RWT_FLAME_SWORD ȭ���� ���߿��� ���, ������
		void RSI_A_RWT_FLAME_SWORD_AIR_StateStart ();
		void RSI_A_RWT_FLAME_SWORD_AIR_FrameMoveFuture ();
		void RSI_A_RWT_FLAME_SWORD_AIR_EventProcess ();

		// RSI_A_RRF_LIMIT_CRUSHER ����Ʈ ũ����, ������
		void RSI_A_RRF_LIMIT_CRUSHER_CHARGE_Init();
		void RSI_A_RRF_LIMIT_CRUSHER_CHARGE_StateStartFuture();
		void RSI_A_RRF_LIMIT_CRUSHER_CHARGE_StateStart();
		void RSI_A_RRF_LIMIT_CRUSHER_CHARGE_EventProcess();
		void RSI_A_RRF_LIMIT_CRUSHER_CHARGE_StateEnd();

		void RSI_A_RRF_LIMIT_CRUSHER_REVENGE_ATTACK_EventProcess ();

		void RSI_A_RRF_LIMIT_CRUSHER_AVOID_ATTACK_EventProcess ();

		// RSI_A_RRF_HELL_DIVE �� ���̺�, ������
		void RSI_A_RRF_HELL_DIVE_Init ();
		void RSI_A_RRF_HELL_DIVE_FrameMove ();
		void RSI_A_RRF_HELL_DIVE_EventProcess ();

		// RSI_A_RVC_NAPALM_GRENADE ������ �׷����̵�, ������
		void RSI_A_RVC_NAPALM_GRENADE_Init ();
		void RSI_A_RVC_NAPALM_GRENADE_EventProcess ();
		void RSI_A_RVC_NAPALM_GRENADE_FrameMove ();

		// RSI_SA_RWT_REVOLVER_CANNON_HE ������ ĳ�� HE, ������
		void RSI_SA_RWT_REVOLVER_CANNON_HE_Init ();
		void RSI_SA_RWT_REVOLVER_CANNON_HE_StateStart ();
		void RSI_SA_RWT_REVOLVER_CANNON_HE_EventProcess ();
		void RSI_SA_RWT_REVOLVER_CANNON_HE_FrameMove ();

		// RSI_SA_RWT_REVOLVER_CANNON_HE ������ ĳ�� HE ����, ������
		void RSI_SA_RWT_REVOLVER_CANNON_HE_LOOP_StateStart();
		void RSI_SA_RWT_REVOLVER_CANNON_HE_LOOP_FrameMove ();
		void RSI_SA_RWT_REVOLVER_CANNON_HE_LOOP_EventProcess ();
		void RSI_SA_RWT_REVOLVER_CANNON_HE_FINISH_EventProcess ();

		// RSI_SA_RWT_REVOLVER_CANNON_HE ������ ĳ�� HE ����, ������

		void RSI_SA_RWT_REVOLVER_CANNON_HE_END_EventProcess ();

		/// RVC_SI_SA_IGNITION_CROW_INCINERATION �̱״ϼ� ũ�ο� �νóʷ��̼�, ������
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_Init();
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_StateStart();
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_FrameMove();
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_EventProcess();

		/// RVC_SI_SA_IGNITION_CROW_INCINERATION �̱״ϼ� ũ�ο� �νóʷ��̼� �߻�, ������
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_EventProcess();

		/// RVC_SI_SA_IGNITION_CROW_INCINERATION �̱״ϼ� ũ�ο� �νóʷ��̼� ������Ʈ �߻�, ������
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_OVERHEAT_StateStart();
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_OVERHEAT_FrameMove();
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_FIRE_OVERHEAT_EventProcess();

		/// RVC_SI_SA_IGNITION_CROW_INCINERATION �̱״ϼ� ũ�ο� �νóʷ��̼� ������Ʈ ����, ������
		void RVC_SI_SA_IGNITION_CROW_INCINERATION_OVERHEAT_END_EventProcess();

		//  RSI_P_RBM_SHADOW_STEP_THRUST ������ ������Ʈ, ������
		// ���� ������ ������Ʈ�� State �߰�
		// ���� Shadow Step ���� �ٸ� �Ϳ� ����!
		// �ش� ��ų ��� ���¿��� X ��ư�� ������ ������.
		void RSI_P_RBM_SHADOW_STEP_THRUST_Init();
		void RSI_P_RBM_SHADOW_STEP_THRUST_Start();
		void RSI_P_RBM_SHADOW_STEP_THRUST_EventProcess();

		void RSI_P_RRF_SHADOW_PUNISHER_Init ();
		void RSI_P_RRF_SHADOW_PUNISHER_Start ();
		void RSI_P_RRF_SHADOW_PUNISHER_EventProcess ();

		void RSI_A_ROT_ARMOR_BREAK_Init();
		void RSI_A_ROT_ARMOR_BREAK_EventProcess();


		void SetSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, UserUnitStateData& stateData_ );
		void SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet, RAVEN_STATE_ID eStateID );


		// ���丣�� �Ͻ�
		void RSI_HA_RRF_INFERNAL_ARM_Init();
		void RSI_HA_RRF_INFERNAL_ARM_EventProcess();

		// �ͽ�Ʈ�� �����̵� FX
		void RSI_HA_RBM_EXTREM_BLADE_READY_Init();
		void RSI_HA_RBM_EXTREM_BLADE_READY_EventProcess();

		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_Init();
		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_StateStart();
		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_FrameMove();
		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_EventProcess();
		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_StateEnd();

		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_FINISH_Init();
		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_FINISH_StartFuture();
		void RSI_HA_RBM_EXTREM_BLADE_ATTACK_FINISH_EventProcess();

		//#pragma region SI_HA_RVC_BURNING_BUSTER
		void RSI_HA_RVC_BURNING_BUSTER_Init();
		//#pragma endregion ���� ������ - �ñر�


		void SetInvisibility(bool bVal_);
};

IMPLEMENT_INTRUSIVE_PTR( CX2GURaven::WildChargeData );
