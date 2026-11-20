#pragma once


/// �ֵ��� ���� ��带 ������ �� �ִ���
#define ELSE_IF_CAN_CHANGE_FORMATION_MODE( hyperModeState ) \
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && \
	true == m_InputData.oneLCtr && true == GetPossibleChangeFormationMode() ) \
	{ StateChange( hyperModeState ); }

#define IF_CAN_CHANGE_FORMATION_MODE( hyperModeState ) \
	if( false == IsThereEffectiveBlackHoleDamageEffect() && \
	true == m_InputData.oneLCtr && true == GetPossibleChangeFormationMode() ) \
	{ StateChange( hyperModeState ); }

/// �ֵ��� ���� ��带 ������ �� �ִ���
#define ELSE_IF_CAN_RELEASE_FORMATION_MODE \
	else if( true == m_InputData.oneLCtr && true == GetIsFormationMode() )  \
	{ m_FrameDataFuture.syncData.nowAction = UAI_RELEASE_FORMATION_MODE; }

// Outside formation mode the new paths pay one DP point for a Nasod armor attack (V5).
#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#define X2_NASOD_ARMOR_DP_COST( bIsFormationMode ) if ( false == (bool)(bIsFormationMode) ) { ThirdPath_ConsumeDPPoints( 1 ); }
#else
#define X2_NASOD_ARMOR_DP_COST( bIsFormationMode )
#endif

/// �ֵ��� ���ҵ� �尩 ��带 ����� �� �ִ���
#define ELSE_IF_CAN_NASOD_ARMOR_ATTACK( iEventFlagIndex, fInputTimingStart, fInputTiming, fChangeTiming, bIsFormationMode, uidTargetUID ) \
	else if( m_FrameDataFuture.stateParam.bEventFlagList[(int)(iEventFlagIndex)] == true && \
			 m_pXSkinAnimFuture->GetNowAnimationTime() > (float)(fChangeTiming) && (UidType)uidTargetUID > 0 ) \
			{ X2_NASOD_ARMOR_DP_COST( bIsFormationMode ) \
			  StateChange( CX2GUAdd::ANSI_COMBO_NASOD_ARMOR_READY ); m_FrameDataFuture.stateParam.bEventFlagList[(int)(iEventFlagIndex)] = false; } \
	else if( ( true == m_InputData.oneZ || true == m_InputData.oneX ) && \
			 m_pXSkinAnimFuture->GetNowAnimationTime() < (float)(fInputTiming) && \
			 m_pXSkinAnimFuture->GetNowAnimationTime() >= (float)(fInputTimingStart) && \
			 NULL != GetUnit() && CX2Unit::UC_ADD_NASOD_RULER != GetUnit()->GetClass() && \
			 true == (bool)bIsFormationMode ) \
			{ m_FrameDataFuture.stateParam.bEventFlagList[(int)(iEventFlagIndex)] = true; }

/// �ֵ� ���� ��ų ĵ�� ���
#define SKILL_CANCEL_AFTER_ONLY_ADD( time ) \
	else if( (time) >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() > (time) && \
	SpecialAttackEventProcess() == true ) \
	{ } \
	else if( false == IsThereEffectiveBlackHoleDamageEffect() && \
		true == m_InputData.oneLCtr && true == GetPossibleChangeFormationMode() && \
		(time) >= 0.f && m_pXSkinAnimFuture->GetNowAnimationTime() > (time) ) \
	{ StateChange( USI_HYPER_MODE ); }

class CX2GUAdd : public CX2GUUser
{
public:
	/// ���ҵ� �Ƹ� ���� Ÿ�� ����
	struct NasodArmorAttackTargetData
	{
		UidType m_uidTargetUID;
		bool	m_bIsNPC;

		NasodArmorAttackTargetData()
		: m_uidTargetUID( 0 )
		, m_bIsNPC( false )
		{}

		void SetTargetData( IN const UidType uidTargetUID, IN const bool bIsNPC )
		{
			m_uidTargetUID	= uidTargetUID;
			m_bIsNPC		= bIsNPC;
		}

		const CX2GameUnit* GetTargetUnit();

		void ResetTargetData()
		{
			m_uidTargetUID	= 0;
			m_bIsNPC		= false;
		}
	};

	/// ���̳ʸ� ���׳�Ʈ�� ��ȯ ���� ( ���̳ʸ� ���� - ���׳�Ʈ�� )
	struct MagnetronDamageEffectData
	{
		CX2DamageEffect::CEffectHandle									m_hLinkEffectHandle;	/// ������ ����Ʈ �ڵ�
		std::pair<CX2DamageEffect::CEffectHandle, CX2DamageEffect::CEffectHandle>	m_hPylonHandle;			/// ����� �� ���Ϸ��� ������ ����Ʈ �ڵ�

		MagnetronDamageEffectData()
		{
			m_hLinkEffectHandle = INVALID_DAMAGE_EFFECT_HANDLE;
			m_hPylonHandle		= std::make_pair( INVALID_DAMAGE_EFFECT_HANDLE, INVALID_DAMAGE_EFFECT_HANDLE );
		}

		MagnetronDamageEffectData( CX2DamageEffect::CEffectHandle hLinkEffectHandle_, std::pair<CX2DamageEffect::CEffectHandle, CX2DamageEffect::CEffectHandle> hPylonHandle_ )
		{
			m_hLinkEffectHandle = hLinkEffectHandle_;
			m_hPylonHandle		= hPylonHandle_;
		}
	};

	/// ���̳ʸ� ���Ϸ� ��ȯ ���� ( ���̳ʸ� ���� - ���Ϸ� )
	struct PylonDamageEffectData
	{
		CX2DamageEffect::CEffectHandle	m_EffectHandle;			/// ������ ����Ʈ �ڵ�
		CX2DamageEffect::CEffectHandle	m_BuffEffectLHandle;		/// ������ ����Ʈ �ڵ�
		float					m_fRemainTime;		/// ���� �ð�

		PylonDamageEffectData()
		: m_EffectHandle( INVALID_DAMAGE_EFFECT_HANDLE )
		, m_BuffEffectLHandle( INVALID_DAMAGE_EFFECT_HANDLE )
		, m_fRemainTime( 0.f )
		{}

		PylonDamageEffectData( CX2DamageEffect::CEffectHandle hEffectHandle_, CX2DamageEffect::CEffectHandle hBuffEffectHandle_, float  fRemainTime_ )
		{
			m_EffectHandle		= hEffectHandle_;
			m_BuffEffectLHandle	= hBuffEffectHandle_;
			m_fRemainTime		= fRemainTime_;
		}
	};

	/// ���̳ʸ� ���Ϸ� ���� ����Ʈ ��ü ���� ( ���̳ʸ� ���� - ���Ϸ� )
	struct PylonLinkDamageEffectData
	{
		CX2DamageEffect::CEffectHandle									m_hLinkEffectHandle;	/// ���� ������ ����Ʈ �ڵ�
		std::pair<CX2DamageEffect::CEffectHandle, CX2DamageEffect::CEffectHandle>	m_hPylonHandle;			/// ����� �� ���Ϸ��� ������ ����Ʈ �ڵ�

		PylonLinkDamageEffectData()
		{
			m_hLinkEffectHandle = INVALID_DAMAGE_EFFECT_HANDLE;
			m_hPylonHandle		= std::make_pair( INVALID_DAMAGE_EFFECT_HANDLE, INVALID_DAMAGE_EFFECT_HANDLE );
		}

		PylonLinkDamageEffectData( CX2DamageEffect::CEffectHandle hLinkEffectHandle_, std::pair<CX2DamageEffect::CEffectHandle, CX2DamageEffect::CEffectHandle> PylonHandle_ )
		{
			m_hLinkEffectHandle = hLinkEffectHandle_;
			m_hPylonHandle		= PylonHandle_;
		}
	};

	/// ���̳ʸ� ���׳�Ʈ�� ���� ����Ʈ ��ü ���� ( ���̳ʸ� ���� - ���׳�Ʈ�� )
	struct MagnetronData
	{
		vector<CX2DamageEffect::CEffectHandle>	m_vecMagnetronDamageEffectData;	/// ���׳�Ʈ�� ������ ����Ʈ ���� ��ü
		float									m_fFireReadyTime;				/// ���׳�Ʈ�� �߻� ����

		MagnetronData()
			: m_fFireReadyTime( 0.f )
		{
			m_vecMagnetronDamageEffectData.reserve( 0 );
		}

		/// �����̽ý� �ʵ� ���� �ʱ�ȭ �Լ�
		void ResetMagnetronData();
	};

	/// �����̽ý� �ʵ� �ǰ� �� Ÿ�� ��ġ ����� ����ü
	struct StasisfieldData
	{
		float					m_fRemainTime;		/// �����̽ý� �ʵ� ���� �ð�
		float					m_fHitValue;		/// ���� Ÿ��ġ
		float					m_fDamageValue;		/// ���� �ǰ�ġ

		float					m_fChangeRate;		/// ���� �ǰ�/Ÿ�� ��ġ�� ���� ��� ��
		CX2EffectSet::Handle	m_hLoopEffectSet;	/// �ʵ� ���� ����Ʈ ��
		CX2EffectSet::Handle	m_hCountEffectSet;	/// ī��Ʈ �ٿ� ����Ʈ ��

		StasisfieldData()
		: m_fRemainTime( 0.f )
		, m_fHitValue( 0.f )
		, m_fDamageValue( 0.f )
		, m_fChangeRate(1.f)
		, m_hLoopEffectSet( INVALID_EFFECTSET_HANDLE )
		, m_hCountEffectSet( INVALID_EFFECTSET_HANDLE )
		{}

		/// �����̽ý� �ʵ� ���� �ʱ�ȭ �Լ�
		void ResetStasisfieldData();

		/// �����̽ý� �ʵ� ���� ����
		bool GetIsActiveStasisfield() const  { return ( 0.f < m_fRemainTime ); }
	};

	/// ���� ��ȭ ���� ����
	struct PhaseShiftData
	{
		float					m_fEffectLifeTime;
		float					m_fReturnMPConsume;		/// ���� ��ȭ ���� ���� MP �Һ�
		D3DXVECTOR3				m_vUserReturnPos;		/// ���� ���� ��ġ
		CX2DamageEffect::CEffectHandle m_hPhasShiftDamageEffect;

		// �ʱ�ȭ ����
		PhaseShiftData();
		void InitPhaseEffectData();
		void InitResturnPosValue();
		// ��ȿȭ �˻�
		bool IsValidReturnPosValue() const;
		bool GetIsActivePhaseShift() const{	return IsValidReturnPosValue();}
		// ���� ������ ����
		void UpdateSkillSlotUI();
	};

	/// ����ź - �������� ���� ����
	struct ChargedImpulsarData
	{
		bool	m_bEnableChargedImpulsar;	/// ����ź - �������� �нú� ���� ����

		// ���� ������ �ִ� ����. ��ũ��Ʈ���� ���� ��.
		float	m_fIncreaseLifeTimeMaxRate;
		float	m_fIncreaseScaleMaxRate;
		float	m_fIncreaseCurveSpeedMaxRate;
		float	m_fIncreaseDamageRate;

		ChargedImpulsarData()
		{
			m_bEnableChargedImpulsar	= false;
			m_fIncreaseLifeTimeMaxRate		= 1.f;
			m_fIncreaseScaleMaxRate			= 1.f;
			m_fIncreaseCurveSpeedMaxRate	= 1.f;
			m_fIncreaseDamageRate			= 1.f;
		}

		void ResetChargedImpulsarData()
		{
			m_bEnableChargedImpulsar	= false;
			m_fIncreaseLifeTimeMaxRate		= 1.f;
			m_fIncreaseScaleMaxRate			= 1.f;
			m_fIncreaseCurveSpeedMaxRate	= 1.f;
			m_fIncreaseDamageRate			= 1.f;
		}
	};


	/// ȯ�� ������ Ÿ�� ���� ����
	struct FantasyTracerTargetInfo
	{
		bool	m_bEnableChargedImpulsar;	/// ����ź - �������� �нú� ���� ����

		float						m_fRemainTime;		/// ���� �ð�
		NasodArmorAttackTargetData	m_NATargetData;		/// Ÿ�� ����
		CX2SkillTree::SKILL_ID		m_eSkillID;			/// ���ҵ� �޺��� �ߵ� ��ų ��ų ���̵�
		bool						m_bIsUseData;		/// �ش� Ÿ�� ���� ��� ����

		FantasyTracerTargetInfo()
		{
			m_fRemainTime	= 0.f;
			m_NATargetData	= NasodArmorAttackTargetData();
			m_eSkillID		= CX2SkillTree::SI_NONE;
			m_bIsUseData	= false;
		}

		FantasyTracerTargetInfo( float fRemainTime, NasodArmorAttackTargetData NATargetData, CX2SkillTree::SKILL_ID eSkillID )
		{
			m_fRemainTime	= fRemainTime;
			m_NATargetData	= NATargetData;
			m_eSkillID		= eSkillID;
			m_bIsUseData	= false;
		}

		void ResetFantasyTracerTargetInfo()
		{
			m_fRemainTime	= 0.f;
			m_NATargetData	= NasodArmorAttackTargetData();
			m_eSkillID		= CX2SkillTree::SI_NONE;
			m_bIsUseData	= false;
		}
	};

	/// ����Ʈ �Ƴ���� Ÿ�� ����
	struct DustAnatomyTargetInfo
	{
		CX2DamageEffect::CEffectHandle	m_eLinkEffect;			/// ���� ����Ʈ �ڵ�
		UidType							m_uidTargetUID;			/// Ÿ�� ���
		bool							m_bIsNPC;				/// Ÿ�� NPC ����
		wstring							m_wstrCreateBoneName;	/// ���� �� ��ġ

		DustAnatomyTargetInfo()
		{
			m_eLinkEffect			= INVALID_DAMAGE_EFFECT_HANDLE;
			m_uidTargetUID			= 0;
			m_bIsNPC				= false;
			m_wstrCreateBoneName	= L"";
		}

		DustAnatomyTargetInfo( CX2DamageEffect::CEffectHandle eLinkEffect, UidType uidTargetUID, bool bIsNPC, wstring wstrCreateBoneName )
		{
			m_eLinkEffect			= eLinkEffect;
			m_uidTargetUID			= uidTargetUID;
			m_bIsNPC				= bIsNPC;
			m_wstrCreateBoneName	= wstrCreateBoneName;
		}

		CX2GameUnit*	GetTargetUnit();
	};

	/// ������ ������ Ÿ�� ����
	struct ReverseReactorTargetInfo
	{
		float							m_fMaxTransitionDamage;		/// ������ �� �ִ� �ִ� ������
		float							m_fCumulativeDamage;		/// ���� ������
		float							m_fRemainTime;				/// ���� �ð�
		float							m_fTransitionDamageRate;	/// ���� ������ ����
		UidType							m_uidTargetUID;				/// Ÿ�� UID
		bool							m_bIsNPC;					/// Ÿ�� NPC ����
		CX2DamageEffect::CEffectHandle	m_eLinkEffect;				/// ���� ����Ʈ �ڵ�

		ReverseReactorTargetInfo()
		{
			m_fMaxTransitionDamage	= 0.f;
			m_fCumulativeDamage		= 0.f;
			m_fRemainTime			= 0.f;
			m_fTransitionDamageRate	= 0.f;
			m_uidTargetUID			= 0;
			m_bIsNPC				= false;
			m_eLinkEffect			= INVALID_DAMAGE_EFFECT_HANDLE;
		}

		ReverseReactorTargetInfo( float fMaxTransitionDamage, float fCumulativeDamage, float fRemainTime, float fTransitionDamageRate,
								  UidType uidTargetUID, bool bIsNPC, CX2DamageEffect::CEffectHandle eLinkEffect )
		{
			m_fMaxTransitionDamage	= fMaxTransitionDamage;
			m_fCumulativeDamage		= fCumulativeDamage;
			m_fRemainTime			= fRemainTime;
			m_fTransitionDamageRate	= fTransitionDamageRate;
			m_uidTargetUID			= uidTargetUID;
			m_bIsNPC				= bIsNPC;
			m_eLinkEffect			= eLinkEffect;
		}

		CX2GameUnit*	GetTargetUnit();
		void			ResetReverseReactorTargetInfo();
	};

	/// ���ǹ� ������ ���� ����
	struct QuicksilverFrenzyData
	{
		bool	m_bEnableQuicksilverFrenzy;		/// ���� ����
		float	m_fFireIntervalTime;			/// �߻� ����
		float	m_fPowerRate;					/// ������ ����

		QuicksilverFrenzyData()
		{
			m_bEnableQuicksilverFrenzy	= false;
			m_fFireIntervalTime			= 0.f;
			m_fPowerRate				= 1.f;
		}

		void ResetQuicksilverFrenzyData()
		{
			m_bEnableQuicksilverFrenzy	= false;
			m_fFireIntervalTime			= 0.f;
			m_fPowerRate				= 1.f;
		}
	};

	/// ����ű ���� ���� ����
	struct PsychicStormData
	{
		float					m_fIncreaseSize;		/// �����Ǵ� ũ��
		float					m_fLoopTime;			/// �ݺ� �ð�
		float					m_fFireGap;				/// �߻� ����
		CX2EffectSet::Handle	eLoopEffectHandle;		/// ���� ����Ʈ �ڵ�

		PsychicStormData()
		{
			m_fIncreaseSize		= 0.f;
			m_fLoopTime			= 0.f;
			m_fFireGap			= 0.f;
			eLoopEffectHandle	= INVALID_EFFECTSET_HANDLE;
		}

		void ResetPsychicStormData();
	};

	enum ADD_NASODRULER_STATE_ID
	{
		/// �� ĳ���� ���� ������Ʈ
		ANSI_BASE = USI_END + 1,

		ANSI_AIR_DASH,						/// ���� ���

		ANSI_HYPER_WAIT,					/// ���� ��� ����

		ANSI_STAND_UP_ROLLING_FRONT_FRONT,	/// FRONT DOWN ���¿��� ������ �̵��ϸ� ���
		ANSI_STAND_UP_ROLLING_FRONT_BACK,	/// FRONT DOWN ���¿��� �ڷ� ���� ���
		ANSI_STAND_UP_ROLLING_BACK_FRONT,	/// BACK DOWN ���¿��� ������ ���� ���
		ANSI_STAND_UP_ROLLING_BACK_BACK,	/// BACK DOWN ���¿��� �ڷ� �̵��ϸ� ���

		ANSI_STAND_UP_ATTACK_FRONT,			/// FRONT DOWN ���¿��� ��� ����
		ANSI_STAND_UP_ATTACK_BACK,			/// BACK DOWN ���¿��� ��� ����

		ANSI_DASH_JUMP_POWER_LANDING,		/// �ִ� �ӵ� ���� ����

		ANSI_LEVITATION_WAIT,				/// ���ڸ� ���� �ξ�

		ANSI_WALL_LANDING,					/// ???


		/// �⺻ ����
		//////////////////////////////////////////////////////////////////////////

		/// �⺻ �޺�
		ANSI_COMBO_Z,						/// �޺� Z
		ANSI_COMBO_ZZ,						/// �޺� ZZ
		ANSI_COMBO_ZZZ,						/// �޺� ZZZ
		ANSI_COMBO_ZZZZ,					/// �޺� ZZZZ
		ANSI_COMBO_ZZZX,					/// �޺� ZZZX
		ANSI_COMBO_ZZX,						/// �޺� ZZX
		ANSI_COMBO_ZZXX,					/// �޺� ZZXX

		ANSI_COMBO_X,						/// �޺� X
		ANSI_COMBO_XX,						/// �޺� XX
		ANSI_COMBO_XXX,						/// �޺� XXX
		ANSI_COMBO_XXZ,						/// �޺� XXZ
		ANSI_COMBO_XXZZ_START,				/// �޺� XXZZ ����
		ANSI_COMBO_XXZZ_LOOP,				/// �޺� XXZZ �ݺ�
		ANSI_COMBO_XXZZ_FINISH,				/// �޺� XXZZ ������

		ANSI_JUMP_ATTACK_Z,					/// ���� ���� Z
		ANSI_JUMP_ATTACK_X,					/// ���� ���� X

		ANSI_DASH_COMBO_Z,					/// �뽬 �޺� Z
		ANSI_DASH_COMBO_ZZ,					/// �뽬 �޺� ZZ
		ANSI_DASH_COMBO_ZZZ,				/// �뽬 �޺� ZZz

		ANSI_DASH_COMBO_X,					/// �뽬 �޺� X
		ANSI_DASH_COMBO_XX,					/// �뽬 �޺� XX
		ANSI_DASH_COMBO_XXX,				/// �뽬 �޺� XXx

		ANSI_DASH_JUMP_COMBO_Z,				/// �뽬 ���� �޺� Z
		ANSI_DASH_JUMP_COMBO_ZZ,			/// �뽬 ���� �޺� ZZ
		ANSI_DASH_JUMP_COMBO_ZZZ,			/// �뽬 ���� �޺� ZZZ
		ANSI_DASH_JUMP_COMBO_X,				/// �뽬 ���� �޺� X
		ANSI_DASH_JUMP_COMBO_XX_START,		/// �뽬 ���� �޺� XXZZ ����
		ANSI_DASH_JUMP_COMBO_XX_CHARGE,		/// �뽬 ���� �޺� XXZZ �ݺ�
		ANSI_DASH_JUMP_COMBO_XX_FINISH,		/// �뽬 ���� �޺� XXZZ ������


		/// 1-1�� ����ű Ʈ���̼�
		//////////////////////////////////////////////////////////////////////////

		/// ���ҵ� �踮��
		ANSI_DP_BARRIER_ACTIVE,				/// DP ������ ����

		/// ���ҵ� �Ƹ� �޺�
		ANSI_COMBO_NASOD_ARMOR_READY,		/// ���ҵ� �Ƹ� ���� �غ�
		ANSI_COMBO_NASOD_ARMOR_ATTACK,		/// ���ҵ� �Ƹ� ����
		ANSI_COMBO_NASOD_ARMOR_LANDING,		/// ���ҵ� �Ƹ� ����
		ANSI_COMBO_NASOD_ARMOR_Z,			/// ���ҵ� �Ƹ� �޺� Z
		ANSI_COMBO_NASOD_ARMOR_ZZ,			/// ���ҵ� �Ƹ� �޺� ZZ
		ANSI_COMBO_NASOD_ARMOR_ZZZ,			/// ���ҵ� �Ƹ� �޺� ZZZZ
		ANSI_COMBO_NASOD_ARMOR_ZZX,			/// ���ҵ� �Ƹ� �޺� ZZX
		ANSI_COMBO_NASOD_ARMOR_ZZXX,		/// ���ҵ� �Ƹ� �޺� ZZXX
		ANSI_COMBO_NASOD_ARMOR_X,			/// ���ҵ� �Ƹ� �޺� X
		ANSI_COMBO_NASOD_ARMOR_XX,			/// ���ҵ� �Ƹ� �޺� XX
		ANSI_COMBO_NASOD_ARMOR_XXX,			/// ���ҵ� �Ƹ� �޺� XXX

		/// ����� ��Ƽ��
		ANSI_SA_APT_STASIS_FIELD_ACTIVE,	/// �����̽ý� �ʵ� �ߵ� ������Ʈ

		ANSI_SA_APT_PULSE_CANNON_LOOP,		/// �޽� ĳ�� ����
		ANSI_SA_APT_PULSE_CANNON_FINISH,	/// �޽� ĳ�� �߻�

		/// ��Ƽ��
		ANSI_A_AN_PHASE_SHIFT_BLINK,		/// ���� ��ȭ ���� ������Ʈ

		ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY,		/// ����ź �������� �غ�
		ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE,		/// ����ź �������� ����
		ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE,		/// ����ź �������� �߻�


		/// 1-2�� �糪ƽ ����Ŀ
		//////////////////////////////////////////////////////////////////////////

		/// ���ҵ� �Ƹ� �޺�
		ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_START,		/// ���ҵ� �Ƹ� �޺� ZZFrontZ �غ�
		ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP,		/// ���ҵ� �Ƹ� �޺� ZZFrontZ �ݺ�
		ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX,			/// ���ҵ� �Ƹ� �޺� ZZFrontZX
		ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP,	/// ���ҵ� �Ƹ� �޺� XDirectionX �ݺ�
		ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ,		/// ���ҵ� �Ƹ� �޺� XDirectionXZ

		ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK,				/// ���ǹ� �׼� ����

		/// ����� ��Ƽ��
		ANSI_SA_ALP_PSYCHIC_STORM_LOOP,		/// ����ű ���� �ݺ�
		ANSI_SA_ALP_PSYCHIC_STORM_FINISH,	/// ����ű ���� ������ Ÿ��
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)


#ifdef X2_ADD_2ND_PATH
		ANSI_AAT_COMBO_NASOD_ARMOR_X,
		ANSI_AAT_COMBO_NASOD_ARMOR_XX,
		ANSI_AAT_COMBO_NASOD_ARMOR_XXX,
		ANSI_AAT_COMBO_NASOD_ARMOR_Z,
		ANSI_AAT_COMBO_NASOD_ARMOR_ZZ,
		ANSI_AAT_COMBO_NASOD_ARMOR_ZZZ,
		ANSI_AAT_DASH_COMBO_ZBackX,
		ANSI_AAT_DASH_COMBO_ZX,
		ANSI_AAT_CREATE_DYNAMO_FACTORY,
		ANSI_SA_AAT_PANZER_BUSTER_START,
		ANSI_SA_AAT_PANZER_BUSTER_AIMING,
		ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP,
		ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN,
		ANSI_SA_AAT_PANZER_BUSTER_FIRE,
		ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR,
		ANSI_SA_AAT_PHANTOM_SEEKER,
		ANSI_SA_AAT_PSIONIC_GENERATOR,
#endif

#ifdef X2_ADD_2ND_PATH
		ANSI_AMM_COMBO_XXDownX_START,
		ANSI_AMM_COMBO_XXDownX_LOOP,
		ANSI_AMM_COMBO_XXDownX_END,
		ANSI_AMM_DASH_JUMP_COMBO_XZ,
		ANSI_AMM_DASH_JUMP_COMBO_XZZ,
		ANSI_SA_AMM_INSTALLATION_PULSE_GATLING,
		ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER,
		ANSI_SA_AMM_INSTALLATION_STAR_FALL,
		ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY,
		ANSI_HA_AMM_APOCALYPSE,
#endif

		ANSI_CAN_NOT_ATTACK_JUMP_DOWN,


		ANSI_SA_ATT_VOID_IMPACT,
		ANSI_SA_ATT_VOID_FIELD,
		ANSI_SA_ATT_VOID_FIELD_LOOP,
		ANSI_SA_ATT_VOID_FIELD_END,
		ANSI_SA_ATT_MAXIMUM_STRIKE,
		ANSI_SA_ATT_STARDUST_SHOWER,
		ANSI_SA_ATT_STARDUST_SHOWER_LOOP,
		ANSI_SA_ATT_STARDUST_SHOWER_END,
		ANSI_SA_ADE_REVERSE_CIRCLE,
		ANSI_SA_ADE_REVERSE_STIGMA,
		ANSI_SA_ADE_REVERSE_STIGMA_RETURN,
		ANSI_SA_ADE_MOONLIGHT_RHAPSODY,
		ANSI_SA_ADE_GRAVITY_BURSTER,
		ANSI_SA_COMMON_CREST_OF_TIME,
		ANSI_HA_ADE_PARANOIA,
		ANSI_A_ATT_SPACE_CRACK,
		ANSI_A_ATT_SPACE_CRACK_START,
		ANSI_A_ATT_SPACE_CRACK_MOVE,
		ANSI_A_ATT_SPACE_CRACK_WAIT,
		ANSI_A_ATT_SPACE_CRACK_END,
		ANSI_A_ADE_FORCE_NOVA,
		ANSI_A_ADE_FORCE_FINGER,
		ANSI_ATT_COMBO_ZZXX,
		ANSI_ATT_COMBO_NASOD_ARMOR_X,
		ANSI_ATT_COMBO_NASOD_ARMOR_XX,
		ANSI_ATT_COMBO_NASOD_ARMOR_XXX_START,
		ANSI_ATT_COMBO_NASOD_ARMOR_XXX_LOOP,
		ANSI_ATT_COMBO_NASOD_ARMOR_XXX_END,
		ANSI_ATT_COMBO_NASOD_ARMOR_Z,
		ANSI_ATT_COMBO_NASOD_ARMOR_ZZ,
		ANSI_ATT_COMBO_NASOD_ARMOR_ZZZ,
		ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_START,
		ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_LOOP,
		ANSI_ATT_COMBO_NASOD_ARMOR_ZZZZ_END,
		ANSI_ATT_COMBO_NASOD_ARMOR_LANDING,
		ANSI_ADE_COMBO_XXXX_FORMATION_MODE,
		ANSI_ADE_COMBO_XXXX_NO_FORMATION_MODE,
		ANSI_ADE_COMBO_XXXZ_FORMATION_MODE,
		ANSI_ADE_COMBO_XXXZ_NO_FORMATION_MODE,
#endif
	};


	/// Ư���� ��Ȳ������ ���� ���� ����
	enum CUSTOM_RENDER_STATE
	{
		CRS_NONE						= 0,
		CRS_COMBO_DASH_X				= 1,	/// ��� X ����
		CRS_QUICKSILVER_FRENZY_BUFF		= 2,	/// ���ǹ� ������ ���� ���� ȿ��
		CRS_END							= 3,
	};

	/// ��� ��ȯ�� ���� ��Ȳ ( DP�� 50% �̻��ΰ�, �̸��ΰ� )
	enum POWER_EXCHANGER_STATE
	{
		PES_NONE			= 0,
		PES_HALF_UP			= 1,	/// DP 50% �̻�
		PES_HALF_DOWN		= 2,	/// DP 50% �̸�
		PES_END				= 3,
	};

/// private Function

//#pragma region private Function

private:

	CX2GUAdd( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit );
	virtual ~CX2GUAdd(void);

/// private Function End


/// protected Function Start

protected:

	virtual void Init( bool bUseTeamPos_, int iStartPosIndex_ );
	virtual void ParseCommonRandomState();
	virtual void InitState();
	virtual void InitComponent();
	virtual void InitPassiveSkillState();
	virtual void SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ );
	virtual void SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ );

	virtual void CommonStateStartFuture();
	virtual void CommonStateStart();

	virtual void CommonFrameMoveFuture();
	virtual void CommonFrameMove();

	virtual void CommonStateEndFuture();
	virtual void CommonStateEnd();

	virtual void CommonEventProcess();

	virtual void DetonationStateChange( CX2DamageManager::DamageData* pDamageData );

	virtual RENDER_HINT	CommonRender_Prepare();

	virtual bool SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition_ = CX2SkillTree::ASUT_GROUND );
	virtual void NoStateChangeActionFrameMove();
	virtual float GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;

	float GetActualCoolTime( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN int iSkillLevel ) const;

	virtual CX2GageData* CreateGageData();

	virtual	bool ShouldResetNotBusyTimer();

	void InitStateID();
	void InitStateCommon();
	void InitStateByUnitClass();

	void SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, ADD_NASODRULER_STATE_ID eStateID_ );
	void SetSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, UserUnitStateData& stateData_ );


	void ShowActiveSkillCutInAndLight( float fTimeToShow_, bool bOnlyLight_ = false );
	virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

	bool CanUseSpecialAttack( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = CX2SkillTree::ASUT_GROUND );
	void StateChangeSpecialAttack( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ );

	void PlayLoopSound( const WCHAR* pSoundFileName_, bool bLoop_, bool b3D_ );
	void StopLoopSound();

	virtual float GetChangeDamageRateByAttackerSkill( const CX2DamageManager::DamageData* pDamageData_  ) const;

	virtual bool IsValideSkillRidingOn( CX2SkillTree::SKILL_ID eSkill_ID_ );

	virtual void DieDamageEffectProc( const CX2DamageManager::DamageData& pDamageData_ );

	virtual void ChargeMpAndEtcInTrainingGame();
	/// ���� ������Ʈ
	//////////////////////////////////////////////////////////////////////////

	/// ANSI_DIE_FRONT
	void DieFrontStartFuture();
	void DieFrontStart();
	void DieFrontFrameMove();
	void DieFrontEventProcess();

	/// ANSI_DIE_BACK
	void DieBackStartFuture();
	void DieBackStart();
	void DieBackFrameMove();
	void DieBackEventProcess();

	/// ANSI_WAIT
	void WaitStartFuture();
	void WaitStart();
	void WaitEventProcess();

	/// ANSI_HYPER_WAIT
	void HyperWaitStartFuture();
	void HyperWaitStart();
	void HyperWaitEventProcess();

	/// ANSI_WALK
	void WalkEventProcess();

	/// ANSI_JUMP_READY
	void JumpReadyEventProcess();

	/// ANSI_JUMP_UP
	void JumpUpEventProcess();

	/// ANSI_JUMP_DOWN
	void JumpDownEventProcess();

	/// ANSI_JUMP_LANDING
	void JumpLandingStartFuture();
	void JumpLandingEventProcess();

	/// ANSI_DASH
	void DashEventProcess();

	/// ANSI_DASH_END
	void DashEndStartFuture();
	void DashEndFrameMove();
	void DashEndEventProcess();

	/// ANSI_AIR_DASH
	void AirDashEventProcess();
	void AirDashStateEndFuture();

	/// ANSI_DASH_JUMP
	void DashJumpStartFuture();
	void DashJumpStart();
	void DashJumpFrameMoveFuture();
	void DashJumpEventProcess();
	void DashJumpEndFuture();

	/// ANSI_DASH_JUMP_LANDING
	void DashJumpLandingEventProcess();
	void DashJumpLandingStartFuture();

	/// ANSI_LEVITATION_WAIT
	void LevitationWaitFrameMove();
	void LevitationWaitEventProcess();

	/// ANSI_DAMAGE_SMALL_FRONT
	/// USI_DAMAGE_SMALL_BACK
	void DamageSmallEventProcess();

	/// ANSI_DAMAGE_BIG_FRONT
	/// ANSI_DAMAGE_BIG_BACK
	void DamageBigStart();
	void DamageBigEventProcess();

	/// ANSI_DAMAGE_DOWN_FRONT
	void DamageDownFrontFrameMove();
	void DamageDownFrontEventProcess();

	/// ANSI_DAMAGE_DOWN_BACK
	void DamageDownBackFrameMove();
	void DamageDownBackEventProcess();

	/// ANSI_DAMAGE_AIR_DOWN_LANDING
	void DamageAirDownLandingEventProcess();

	/// ANSI_STAND_UP_ROLLING_FRONT_FRONT
	void StandUpRollingFrontFrontEventProcess();
	void StandUpRollingFrontFrontEnd();

	/// ANSI_STAND_UP_ROLLING_FRONT_BACK
	void StandUpRollingFrontBackEventProcess();
	void StandUpRollingFrontBackEnd();

	/// ANSI_STAND_UP_ROLLING_BACK_FRONT
	void StandUpRollingBackFrontEventProcess();
	void StandUpRollingBackFrontEnd();

	/// ANSI_STAND_UP_ROLLING_BACK_BACK
	void StandUpRollingBackBackEventProcess();
	void StandUpRollingBackBackEnd();

	/// ANSI_STAND_UP_ATTACK_FRONT
	void StandUpAttackFrontEventProcess();
	void StandUpAttackFrontEnd();

	/// ANSI_STAND_UP_ATTACK_BACK
	void StandUpAttackBackEventProcess();
	void StandUpAttackBackEnd();
	void StandUpAttackBackStateEndFuture();

	/// ANSI_DASH_JUMP_POWER_LANDING
	void DashJumpPowerLandingStartFuture();
	void DashJumpPowerLandingStart();
	void DashJumpPowerLandingEventProcess();

	/// ���� ���
	void FormationModeStart();

	/// ANSI_NP_BARRIER_ACTIVE
	void NasodBarrierActiveStateStartFuture();
	void NasodBarrierActiveEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_READY,
	void NasodArmorReadyEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_ATTACK,
	void NasodArmorAttackStartFuture();
	void NasodArmorAttackEventProcess();

	///	ANSI_COMBO_NASOD_ARMOR_LANDING,
	void NasodArmorLandingEventProcess();
	void NasodArmorLandingEndFuture();

	/// ANSI_COMBO_NASOD_ARMOR_Z
	void NasodArmorComboZEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_ZZ
	void NasodArmorComboZZEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_ZZZ
	void NasodArmorComboZZZEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_ZZX
	void NasodArmorComboZZXEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_ZZXX
	void NasodArmorComboZZXXEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_X
	void NasodArmorComboXEventProcess();
	void NasodArmorComboXStateEndFuture();

	/// ANSI_COMBO_NASOD_ARMOR_XX
	void NasodArmorComboXXEventProcess();

	/// ANSI_COMBO_NASOD_ARMOR_XXX
	void NasodArmorComboXXXEventProcess();


	///�⺻ Ŀ�ǵ�
	//////////////////////////////////////////////////////////////////////////

	/// ANSI_COMBO_Z
	void ComboZEventProcess();

	/// ANSI_COMBO_ZZ
	void ComboZZEventProcess();

	/// ANSI_COMBO_ZZZ
	void ComboZZZEventProcess();

	/// ANSI_COMBO_ZZZZ
	void ComboZZZZEventProcess();

	/// ANSI_COMBO_ZZZX
	void ComboZZZXEventProcess();

	/// ANSI_COMBO_ZZX
	void ComboZZXEventProcess();

	/// ANSI_COMBO_ZZXX
	void ComboZZXXEventProcess();

	/// ANSI_COMBO_X
	void ComboXEventProcess();

	/// ANSI_COMBO_XX
	void ComboXXEventProcess();

	/// ANSI_COMBO_XXX
	void ComboXXXEventProcess();

	/// ANSI_COMBO_XXZ
	void ComboXXZEventProcess();

	/// ANSI_COMBO_XXZZ_START
	void ComboXXZZStartEventProcess();
	void ComboXXZZStartStateEndFuture();

	/// ANSI_COMBO_XXZZ_LOOP
	void ComboXXZZLoopStateStart();
	void ComboXXZZLoopEventProcess();
	void ComboXXZZLoopStateEnd();

	/// ANSI_COMBO_XXZZ_FINISH
	void ComboXXZZFinishStateStartFuture();
	void ComboXXZZFinishStateStart();
	void ComboXXZZFinishEventProcess();

	/// ANSI_JUMP_ATTACK_Z
	void JumpComboZEventMoveFuture();
	void JumpComboZEventProcess();

	/// ANSI_JUMP_ATTACK_X
	void JumpComboXEventMoveFuture();
	void JumpComboXEventProcess();

	/// ANSI_DASH_COMBO_Z
	void DashComboZFrameMoveFuture();
	void DashComboZEventProcess();

	/// ANSI_DASH_COMBO_ZZ
	void DashComboZZEventProcess();

	/// ANSI_DASH_COMBO_ZZZ
	void DashComboZZZEventProcess();

	/// ANSI_DASH_COMBO_X
	void DashComboXStateStart();
	void DashComboXFrameMove();
	void DashComboXEventProcess();
	void DashComboXStateEnd();
	void DashComboXStateEndFuture();

	/// ANSI_DASH_COMBO_XX
	void DashComboXXEventProcess();

	/// ANSI_DASH_COMBO_XXX
	void DashComboXXXEventProcess();

	/// ANSI_DASH_JUMP_COMBO_Z
	void DashJumpComboZEventProcess();

	/// ANSI_DASH_JUMP_COMBO_ZZ
	void DashJumpComboZZEventProcess();

	/// ANSI_DASH_JUMP_COMBO_ZZZ
	void DashJumpComboZZZEventProcess();

	/// ANSI_DASH_JUMP_COMBO_X
	void DashJumpComboXEventProcess();

	/// ANSI_DASH_JUMP_COMBO_XX_START
	void DashJumpComboXXStartEventProcess();
	void DashJumpComboXXStartStateEndFuture();

	/// ANSI_DASH_JUMP_COMBO_XX_CHARGE
	void DashJumpComboXXLoopStateStart();
	void DashJumpComboXXLoopFrameMove();
	void DashJumpComboXXLoopEventProcess();
	void DashJumpComboXXLoopStateEnd();

	/// ANSI_DASH_JUMP_COMBO_XX_FINISH
	void DashJumpComboXXFinishStateStartFuture();
	void DashJumpComboXXFinishFrameMove();
	void DashJumpComboXXFinishEventProcess();


	/// ���м� ��Ƽ��
	//////////////////////////////////////////////////////////////////////////

	/// ANSI_SA_AN_VOID_BREAKER
	void ANSI_SA_AN_VOID_BREAKER_Init();
	void ANSI_SA_AN_VOID_BREAKER_FrameMove();
	void ANSI_SA_AN_VOID_BREAKER_EventProcess();

	/// ANSI_SA_AN_PARTICLE_PRISM
	void ANSI_SA_AN_PARTICLE_PRISM_Init();
	void ANSI_SA_AN_PARTICLE_PRISM_FrameMove();
	void ANSI_SA_AN_PARTICLE_PRISM_EventProcess();

	/// ANSI_SA_AN_ENERGY_BOOM
	void ANSI_SA_AN_ENERGY_BOOM_Init();
	void ANSI_SA_AN_ENERGY_BOOM_FrameMove();
	void ANSI_SA_AN_ENERGY_BOOM_EventProcess();

	/// ANSI_SA_AN_PARTICLE_ACCELERATOR
	void ANSI_SA_AN_PARTICLE_ACCELERATOR_Init();
	void ANSI_SA_AN_PARTICLE_ACCELERATOR_FrameMove();
	void ANSI_SA_AN_PARTICLE_ACCELERATOR_EventProcess();

	/// ANSI_SA_AN_NEUTRON_BOMB
	void ANSI_SA_AN_NEUTRON_BOMB_Init();
	void ANSI_SA_AN_NEUTRON_BOMB_EventProcess();

	/// ANSI_SA_APT_PULSE_CANNON
	void ANSI_SA_APT_PULSE_CANNON_READY_Init();
	void ANSI_SA_APT_PULSE_CANNON_READY_StateStartFuture();
	void ANSI_SA_APT_PULSE_CANNON_READY_StateStart();
	void ANSI_SA_APT_PULSE_CANNON_READY_EventProcess();

	void ANSI_SA_APT_PULSE_CANNON_LOOP_FrmaeMove();
	void ANSI_SA_APT_PULSE_CANNON_LOOP_EventProcess();

	void ANSI_SA_APT_PULSE_CANNON_FINISH_EventProcess();

	/// ANSI_SA_APT_PULSE_CANNON
	void ANSI_SA_APT_EMP_SHOCK_Init();
	void ANSI_SA_APT_EMP_SHOCK_FrameMove();
	void ANSI_SA_APT_EMP_SHOCK_EventProcess();

	/// ANSI_SA_APT_QUAKE_BUSTER
	void ANSI_SA_APT_QUAKE_BUSTER_Init();
	void ANSI_SA_APT_QUAKE_BUSTER_EventProcess();
	void ANSI_SA_APT_QUAKE_BUSTER_FrameMove();
	void ANSI_SA_APT_QUAKE_BUSTER_HYPER_EventProcess();

	/// ANSI_SA_APT_STASIS_FIELD
	void ANSI_SA_APT_STASIS_FIELD_Init();
	void ANSI_SA_APT_STASIS_FIELD_StateStart();
	void ANSI_SA_APT_STASIS_FIELD_FrameMove();
	void ANSI_SA_APT_STASIS_FIELD_EventProcess();

	void ANSI_SA_APT_STASIS_FIELD_ACTIVE_StateStart();
	void ANSI_SA_APT_STASIS_FIELD_ACTIVE_FrameMove();
	void ANSI_SA_APT_STASIS_FIELD_ACTIVE_EventProcess();

	/// ANSI_SA_APT_CONQUEROR ��Ŀ��
	void ANSI_SA_APT_CONQUEROR_Init();
	void ANSI_SA_APT_CONQUEROR_EventProcess();
	void ANSI_SA_APT_CONQUEROR_FrameMove();


	/// ��Ƽ��
	//////////////////////////////////////////////////////////////////////////

	/// SI_A_AN_PULSE_BULLET
	void ANSI_A_AN_PULSE_BULLET_Init();
	void ANSI_A_AN_PULSE_BULLET_EventProcess();

	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_StateStartFuture();
	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_EventProcess();
	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_READY_StateEndFuture();

	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_StateStart();
	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_FrameMove();
	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_CHARGE_EventProcess();

	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_StateStartFuture();
	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_FrameMove();
	void ANSI_A_AN_PULSE_BULLET_CHARGED_IMPULSAR_FIRE_EventProcess();

	/// SI_A_AN_MIND_BREAK
	void ANSI_A_AN_MIND_BREAK_Init();
	void ANSI_A_AN_MIND_BREAK_FrmaeMove();
	void ANSI_A_AN_MIND_BREAK_EventProcess();

	/// SI_A_AN_PHASE_SHIFT
	void ANSI_A_AN_PHASE_SHIFT_Init();
	void ANSI_A_AN_PHASE_SHIFT_FrameMove();
	void ANSI_A_AN_PHASE_SHIFT_EventProcess();

	void ANSI_A_AN_PHASE_SHIFT_BLINK_StateStart();
	void ANSI_A_AN_PHASE_SHIFT_BLINK_EventProcess();

	/// SI_A_APT_MAGNETRON_DYNAMO
	void ANSI_A_APT_MAGNETRON_DYNAMO_Init();
	void ANSI_A_APT_MAGNETRON_DYNAMO_FrameMove();
	void ANSI_A_APT_MAGNETRON_DYNAMO_EventProcess();

	/// SI_A_APT_DESOLVER_DYNAMO
	void ANSI_A_APT_DESOLVER_DYNAMO_Init();
	void ANSI_A_APT_DESOLVER_DYNAMO_EventProcess();

	/// SI_A_APT_PYLON_DYNAMO
	void ANSI_A_APT_PYLON_DYNAMO_Init();
	void ANSI_A_APT_PYLON_DYNAMO_FrameMove();
	void ANSI_A_APT_PYLON_DYNAMO_EventProcess();

	/// ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_START
	void NasodArmorComboZZFrontZStartEventProcess();

	/// ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZ_LOOP
	void NasodArmorComboZZFrontZLoopStateStartFuture();
	void NasodArmorComboZZFrontZLoopStateStart();
	void NasodArmorComboZZFrontZLoopEventProcess();

	/// ANSI_ALP_COMBO_NASOD_ARMOR_ZZFrontZX
	void NasodArmorComboZZFrontZXEventProcess();

	/// ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionX_LOOP
	void NasodArmorComboXDirectionXLoopEventProcess();
	void NasodArmorComboXDirectionXLoopStateEndFuture();

	/// ANSI_ALP_COMBO_NASOD_ARMOR_XDirectionXZ
	void NasodArmorComboXDirectionXZEventProcess();


	/// SA_ALP_REVERSE_REACTOR
	void ANSI_SA_ALP_REVERSE_REACTOR_Init();
	void ANSI_SA_ALP_REVERSE_REACTOR_StateStart();
	void ANSI_SA_ALP_REVERSE_REACTOR_FrameMove();
	void ANSI_SA_ALP_REVERSE_REACTOR_EventProcess();

	/// SA_ALP_QUICKSILVER_ACCEL
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_Init();
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_FrameMove();
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_EventProcess();
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_StateEnd();

	/// SA_ALP_QUICKSILVER_ACCEL_BACK
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_FrameMove();
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_EventProcess();
	void ANSI_SA_ALP_QUICKSILVER_ACCEL_BACK_StateEnd();

	/// SA_ALP_QUICKSILVER_FRENZY
	void ANSI_SA_ALP_QUICKSILVER_FRENZY_Init();
	void ANSI_SA_ALP_QUICKSILVER_FRENZY_FrameMove();
	void ANSI_SA_ALP_QUICKSILVER_FRENZY_EventProcess();

	/// SA_ALP_DUST_ANATOMY
	void ANSI_SA_ALP_DUST_ANATOMY_Init();
	void ANSI_SA_ALP_DUST_ANATOMY_FrameMove();
	void ANSI_SA_ALP_DUST_ANATOMY_EventProcess();
	void ANSI_SA_ALP_DUST_ANATOMY_StateEnd();

	/// SA_ALP_PSYCHIC_STORM
	void ANSI_SA_ALP_PSYCHIC_STORM_Init();
	void ANSI_SA_ALP_PSYCHIC_STORM_StateStartFuture();
	void ANSI_SA_ALP_PSYCHIC_STORM_StateStart();
	void ANSI_SA_ALP_PSYCHIC_STORM_EventProcess();

	void ANSI_SA_ALP_PSYCHIC_STORM_LOOP_StateStart();
	void ANSI_SA_ALP_PSYCHIC_STORM_LOOP_FrameMove();
	void ANSI_SA_ALP_PSYCHIC_STORM_LOOP_EventProcess();

	void ANSI_SA_ALP_PSYCHIC_STORM_FINISH_FrameMove();
	void ANSI_SA_ALP_PSYCHIC_STORM_FINISH_EventProcess();

	/// A_ALP_PSIONIC_BEAT
	void ANSI_A_ALP_PSIONIC_BEAT_Init();
	void ANSI_A_ALP_PSIONIC_BEAT_EventProcess();

	/// A_ALP_TWIRL_RUSH
	void ANSI_A_ALP_TWIRL_RUSH_Init();
	void ANSI_A_ALP_TWIRL_RUSH_EventProcess();

	/// HA_ALP_DOOMS_DAY
	void ANSI_HA_ALP_DOOMS_DAY_Init();
	void ANSI_HA_ALP_DOOMS_DAY_StateStart();
	void ANSI_HA_ALP_DOOMS_DAY_FrameMove();
	void ANSI_HA_ALP_DOOMS_DAY_EventProcess();
	void ANSI_HA_ALP_DOOMS_DAY_StateEnd();

#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
#ifdef X2_ADD_2ND_PATH
	void AATComboNasodArmorXEventProcess();
	void AATComboNasodArmorXXEventProcess();
	void AATComboNasodArmorXXXEventProcess();
	void AATComboNasodArmorZEventProcess();
	void AATComboNasodArmorZZEventProcess();
	void AATComboNasodArmorZZZEventProcess();
void AATDashComboZBackXStateStart();
void AATDashComboZBackXFrameMove();
void AATDashComboZBackXEventProcess();
void AATDashComboZBackXStateEnd();
void AATDashComboZXStateStart();
void AATDashComboZXFrameMove();
void AATDashComboZXEventProcess();
void AATDashComboZXStateEndFuture();
void AATDashComboZXStateEnd();
#endif

#ifdef X2_ADD_2ND_PATH
	void AMMComboXXDownXStartEventProcess();
void AMMComboXXDownXStartStateEnd();
	void AMMComboXXDownXLoopEventProcess();
	void AMMComboXXDownXLoopStateStart();
	void AMMComboXXDownXLoopStateEnd();		///< destroy the loop beam DE on exit
	void AMMComboXXDownXEndEventProcess();
	void AMMComboXXDownXEndStateStart();	///< V5 END @0.01 : spawn DamageEffect_AMM_XXDownX_Finish
	void AMMDashJumpComboXZEventProcess();
	void AMMDashJumpComboXZZEventProcess();
#endif

#ifdef X2_ADD_2ND_PATH
	void ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_Init();


	void ANSI_ThirdPath_CreateState( UserUnitStateData& stateData_, const int iStateId_, const char* pszTableName );
	void ANSI_ThirdPath_CreateChainedState( UserUnitStateData& stateData_, const int iStateId_, const char* pszTableName, const int iNextStateId_ );
void ANSI_ThirdPath_OneShot_EventProcess();
void ANSI_ThirdPath_Chain_EventProcess();
std::map<int,int> m_mapThirdPathChain;


	void ANSI_AddComboChain_EventProcess();

	void ATTComboNasodArmorLandingEventProcess();
	void ATTComboNasodArmorXXXLoopStateStart();
	void ATTComboNasodArmorXXXLoopEventProcess();
	void ATTComboNasodArmorXXXLoopStateEnd();
	void ATTComboNasodArmorZZZZStartEventProcess();
	void ATTComboNasodArmorZZZZLoopEventProcess();
	void ATTComboZZXXFrameMove();



	bool	IsAddSkillKeyHeld();																		/// V5 key-page released check — mirror IsELSkillKeyHeld (X2GUEL.cpp:6956)
	bool	ThirdPath_DPCheckOrConsume( const float fOtherUnitRate_, const float fMyUnitRate_ );		/// V5 DPCheck: consume DP at rate when affordable (my-unit rate / other-unit rate)
	int		ThirdPath_ConsumeDPPoints( const int iMaxPoints_ );										/// V5: consume up to N DP points (2015 pip system 0-6 @gageData+248; our float DP 0-8000 → 1 point = MAX_DP_GAGE_VALUE/6)
	void	CreateSpaceCrackField( const bool bSubProjectile_ );										/// V5: spawn the crack rupture field (Main projectile; Sub variant costs 200 DP)
	/// 9024 VOID_FIELD — slot[StartFuture + FM@0.01(field DE + loop ES spawn) + EP anim-end→LOOP], LOOP[FM + EP DP-drain end-check ≥1s min / 5s cap / key-release / DP-out], END[FM@0.01 stop ES + destroy field + EP cancel]
	void ANSI_SA_ATT_VOID_FIELD_StartFuture();
	void ANSI_SA_ATT_VOID_FIELD_FrameMove();
	void ANSI_SA_ATT_VOID_FIELD_EventProcess();
	void ANSI_SA_ATT_VOID_FIELD_LOOP_FrameMove();
	void ANSI_SA_ATT_VOID_FIELD_LOOP_EventProcess();
	void ANSI_SA_ATT_VOID_FIELD_END_FrameMove();
	void ANSI_SA_ATT_VOID_FIELD_END_EventProcess();
	/// 9025 MAXIMUM_STRIKE — FM@1.151 6-way barrage (angle 0..300 step 60, Y+150) + DP-point-scaled power + EP cancel
	void ANSI_SA_ATT_MAXIMUM_STRIKE_FrameMove();
	void ANSI_SA_ATT_MAXIMUM_STRIKE_EventProcess();
	/// 9026 STARDUST_SHOWER — slot[StartFuture duration capture + StateStart rupture field @Y+700 + EP→LOOP], LOOP[StartFuture/StateStart ES + meteor mesh @Y+700 + FM 0.1s spawner @mesh "spwan_Dummy0N" frames 1..10 + EP duration-end + StateEnd mesh destroy], END[FM@1.0 stop+destroy + StateEnd handle reset]
	void ANSI_SA_ATT_STARDUST_SHOWER_StartFuture();
	void ANSI_SA_ATT_STARDUST_SHOWER_StateStart();
	void ANSI_SA_ATT_STARDUST_SHOWER_EventProcess();
	void ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StartFuture();
	void ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StateStart();
	void ANSI_SA_ATT_STARDUST_SHOWER_LOOP_FrameMove();
	void ANSI_SA_ATT_STARDUST_SHOWER_LOOP_EventProcess();
	void ANSI_SA_ATT_STARDUST_SHOWER_LOOP_StateEnd();
	void ANSI_SA_ATT_STARDUST_SHOWER_END_FrameMove();
	void ANSI_SA_ATT_STARDUST_SHOWER_END_EventProcess();
	void ANSI_SA_ATT_STARDUST_SHOWER_END_StateEnd();
	/// 9212 SPACE_CRACK — slot[StartFuture flag + FM@0.31 ES + crack field + EP→MOVE], MOVE↔WAIT toggle on X/Z held, END on skill-key release / DP<200(my)|<10(other) / flag off
	void ANSI_A_ATT_SPACE_CRACK_StartFuture();
	void ANSI_A_ATT_SPACE_CRACK_FrameMove();
	void ANSI_A_ATT_SPACE_CRACK_EventProcess();
	void ANSI_A_ATT_SPACE_CRACK_MOVE_EventProcess();
	void ANSI_A_ATT_SPACE_CRACK_WAIT_EventProcess();
	void ANSI_A_ATT_SPACE_CRACK_END_StartFuture();
	void ANSI_A_ATT_SPACE_CRACK_END_StateStart();
	void ANSI_A_ATT_SPACE_CRACK_END_EventProcess();
	/// 9029 MOONLIGHT_RHAPSODY — FM@1.3 DP-point consume → buff BFI 476 @level + power ×0.66 when DP consumed + EP cancel
	void ANSI_SA_ADE_MOONLIGHT_RHAPSODY_FrameMove();
	void ANSI_SA_ADE_MOONLIGHT_RHAPSODY_EventProcess();

	void ThirdPath_OnMoonlightWaveFrameMove( const float fElapsedTime_ );
	//}}
	/// 9023 VOID_IMPACT — V5 FM @0.846: scan nearest hostile in front (radius SA_SCOPE_ABS, band ±180) → spawn EffectSet_ATT_Void_Impact_Attack at the target's Bip01 (fallback: my pos + facing×scope, Y+80)
	void ANSI_SA_ATT_VOID_IMPACT_FrameMove();
	void ANSI_ThirdPath_OneShotGroundPinned_EventProcess();
	/// 9028 REVERSE_STIGMA — V5 main FM @0.1: scan → mark target (record uid+pos+duration, debuff ES on target) + Attack damage at target; non-transferable target → instant Return damage; RETURN-state FM → @0.1: transfer marked target to the recorded position + Return damage + cleanup
	void ANSI_SA_ADE_REVERSE_STIGMA_FrameMove();
	void ANSI_SA_ADE_REVERSE_STIGMA_RETURN_FrameMove();
	void ANSI_SA_ADE_REVERSE_STIGMA_RETURN_EventProcess();	///< shared generic-cancel EP (V5 main EP + sub-212 EP)
	bool ThirdPath_StigmaMarkTarget( CX2GameUnit* pTarget_ );			///< V5 (transferable-check + clear old mark + store new)
	void ThirdPath_StigmaClearMark();									///< V5 (stop debuff ES + reset uid/pos/duration)
	/// 9030 GRAVITY_BURSTER — V5 FM @1.1: formation+mutation-stacks → consume 1 stack + EffectSet_SA_ADE_GRAVITY_BURSTER_Damage_Mutauion; else normal _Damage
	void ANSI_SA_ADE_GRAVITY_BURSTER_FrameMove();
	/// 9213 FORCE_NOVA — V5 FM @0.5 (hit-flag armed by DamageEffect_ADE_FORCE_NOVA_Attack landing, V5 owner-callbacks /): formation → DP += SA_MP_RETAKE_ABS×20 + "DP+ %d"; else MP += value + "MP+ %d"
	void ANSI_A_ADE_FORCE_NOVA_StateStart();
	void ANSI_A_ADE_FORCE_NOVA_FrameMove();
	/// 9214 FORCE_FINGER — V5 FM @0.271: per scanned target (up to 2): EffectSetI self + By_Target + Explosion DE at target + target MP-drain (SA_MP_INTAKE_ABS) + caster MP/DP recovery (SA_MP_RETAKE_ABS) + left-hand beam particle
	void ANSI_A_ADE_FORCE_FINGER_FrameMove();
	/// shared DE helpers (V5 2015 semantics)
	int ThirdPath_FindScanTargets( const float fRadius_, const float fYBand_, CX2GameUnit** ppOutTargets_, const int iMaxTargets_, const float fDownAngleDeg_ = -9999.f, const float fUpAngleDeg_ = 9999.f );	///< V5 (front cone scan, closest-first; args 5/6 = the elevation-angle window, default-off)
	CX2GameUnit* ThirdPath_FindScanTarget( const CX2SkillTree::SKILL_ID eSkillID_ );	///< scope from templet (SA_SCOPE_ABS, PvP room → SA_SCOPE_ABS_PVP)
	float ThirdPath_GetScanScope( const CX2SkillTree::SKILL_ID eSkillID_ );			///< SA_SCOPE_ABS (PvP room → SA_SCOPE_ABS_PVP; V5 value-type 29/157)
	void ThirdPath_RecoverMPorDP( const float fValue_ );					///< V5 FORCE_NOVA/FORCE_FINGER recovery branch (formation → DP ×20 + "DP+ %d"; else → MP + "MP+ %d")
	void ThirdPath_OnStigmaMarkFrameMove( const float fElapsedTime_ );		///< mark duration tick (V5 /26596)

	bool	ThirdPath_AnchorChaseTarget( const float fRange_, const float pfParams_[3], OUT D3DXVECTOR3* pvAnchorOut_, OUT D3DXVECTOR3* pvEnemyPosOut_);	///< V5 - the AMM XXDownX loop-beam anchor-chase target search (collision-point walk + running-nearest range + line-direction probe + cone gate + elevation slope band; non-const like the)
	void	ThirdPath_MaxStrikeRecordTarget( CX2DamageManager::DamageData* pDamageData_ );				///< V5 - record the attack-hit defender into the MaxStrike homing record (gated on the barrage window)
	void	ThirdPath_MaxStrikeFireOnHit( CX2DamageManager::DamageData* pDamageData_ );					///< DESIGN-INFERENCE fire-on-hit (the V5 driver = the KPT command-word machine newslot_327 case 38, asm-unresolvable per section 49a/49d)
	void	ThirdPath_OnMaxStrikeFrameMove( const float fElapsedTime_ );								///< V5 newslot_317 MaxStrike window branch (tick + the ball follow + the expiry teardown)
	void	ThirdPath_MaxStrikeTeardown( const bool bRefundUnfired_ );									///< V5 (queue destroy + window/record/power clear; arg==1 = the expiry refund count x 25.0)

	UidType							GetStigmaMarkTargetUID() const { return m_uidStigmaTarget; }
	int								GetStigmaMarkTargetType() const { return m_iStigmaTargetType; }

	CX2DamageEffect::CEffectHandle	m_hVoidFieldDE;
	CX2EffectSet::Handle			m_hVoidFieldLoopES;
	float							m_fVoidFieldLoopTime;	/// V5 stateTime this+21016 (LOOP accumulator)
	std::vector<CX2DamageEffect::CEffectHandle>	m_vecMaxStrikeHandles;	/// V5 vector (the barrage placeholder-ball queue)
	float							m_fMaxStrikePower;		/// V5 (the captured barrage power, post DP-pip x(1+0.1N))

	float							m_fMaxStrikeWindowDuration;	/// V5 (the armed window duration = max(SA_EFFECTIVE_TIME, 60))
	float							m_fMaxStrikeWindowRemain;	/// V5 (the ticking window remaining; cool-time + fire gates read this)
	UidType							m_uidMaxStrikeTarget;		/// V5 (the homing-target record UID64; 0 = none)
	int								m_iMaxStrikeTargetType;		/// V5 (the homing-target record GAME_UNIT_TYPE: 0 = user, 1 = NPC)
	CX2DamageEffect::CEffectHandle	m_hStardustFieldDE;
	CX2EffectSet::Handle			m_hStardustLoopES;
	CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hStardustMesh;	/// V5 (Mesh_ATT_Stardust_Shower_Drop = elsword_SA_rageCutter_spwanPoint.X)

	bool							m_bApocalypseSummoned;		/// summon out this cast
	bool							m_bApocalypseFinisherDone;	/// V5 byte 26324 (recast-finisher already triggered)
	bool							m_bXXDownXUsed;				/// Mastermind's XX down X already used in this combo
	bool							m_bComboXXXDamageEffectPlayed;	/// COMBO_XXX projectile already fired
	int								m_iStardustDummyCounter;
	float							m_fStardustSpawnAccum;
	float							m_fStardustDuration;
	float							m_fStardustLoopTime;

	D3DXVECTOR2						m_vStardustSavedNowSpeed;
	CX2EffectSet::Handle			m_hSpaceCrackES;
	bool							m_bSpaceCrackActive;
	bool							m_bMoonlightActive;
	float							m_fMoonlightTimer;
	float							m_fMoonlightPower;
	CX2EffectSet::Handle			m_hMoonlightWaveES;

	bool							m_bForceNovaHit;		/// V5 byte 26620 (armed when DamageEffect_ADE_FORCE_NOVA_Attack lands — V5 DamageEffect owner-callbacks /; reset at 9213 StateStart)
	UidType							m_uidStigmaTarget;		/// V5 = 26552-26556 (marked target uid; 0 = none)
	int								m_iStigmaTargetType;	/// V5 = 26560 (marked target GUT type)
	D3DXVECTOR3						m_vStigmaMarkPos;		/// V5 -26588 (recorded mark position)
	float							m_fStigmaMarkTimer;		/// V5 this+26472 (the mark countdown); (the 26596 cite was a batch-era error - 26596 = a 1.0/0.0 flag per the manifest section7)
	CX2EffectSet::Handle			m_hStigmaDebuffES;		/// V5 (EffectSetI_SA_ADE_REVERSE_STIGMA_Debuff spawned ON the target)

	int								m_iFactoryMarkedSkillID;	/// V5 this+26084 (0 = none, else the marked install skill ID)
	float							m_vFactoryDroneOffset[3];	/// V5 this+26092/26096/26100 (init -200/150/0)

	int								m_iFactoryPhase;			/// V5 this+26104 (0 = none, 1 = generated/active, 2 = post-attack idle, 3 = attacking)
	bool							m_bFactoryFlagWait;			/// V5 byte 26108 (drone wait-anim flag; default true)
	bool							m_bFactoryModeEntered;		/// V5 byte 26109 (mode entered via install cast)
	float							m_fFactoryDP;				/// V5 this+25768 (install-switch DP cost; ctor init 0.0 per the V5 ctor disasm, = 3.0 - passive 9411 SA_EFFECTIVE_TIME at the mode-enter)
	float							m_fFactoryMP;				/// V5 this+25772 (install re-create MP rate; init 1.0, = passive 9411 SA_MP_CONSUMPTION multiplier 0.97..0.622)
	float							m_fInstallDmgScale;			/// V5 this+25804 (install-skill damage scale; = passive 9413 value-15)
	float							m_fFactoryDroneSpeed;		/// V5 this+26136 (drone missile speed; = 9015 SA_REMOTE_SPEED_ABS, default 1250)
	int								m_iNewUpdateLevel;			/// V5 this+25800 (passive 9412 "New Update!" level; consumed at factory generate)
	CX2DamageEffect::CEffectHandle	m_hInstall9015DE;			/// V5 (Dynamo_Factory_Projectile DE)
	CX2DamageEffect::CEffectHandle	m_hInstall9019DE;			/// V5 (Pulse_Gatling_Drone DE)
	CX2DamageEffect::CEffectHandle	m_hInstall9020DE;			/// V5 (Shooting_Chaser_Attacker DE)
	CX2DamageEffect::CEffectHandle	m_hInstall9022DE;			/// V5 (Star_Fall MESH DE)
	float							m_fInstall9022Power;		/// V5 (power capture; init 1.0)

	struct UltimateFuryDroneRec {						// V5 56-byte record (the fields the driver uses):
		float								m_fTimer;		// +0  the per-drone timer (decrement; the 0.5s re-acquire cadence)
		UidType								m_uidTarget;	// +8  the target UID (0 = none)
		int									m_iTargetType;	// +12 the target unit type (GAME_UNIT_TYPE)
		bool								m_bIsNPC;		// +16 the resolution flavor flag
		CKTDGXMeshPlayer::CXMeshInstanceHandle m_hMesh;		// +20 the attack-drone mesh handle (MajorXMeshPlayer)
		CX2DamageEffect::CEffectHandle		m_hShotDE;		// +24 the laser-beam DE handle (invalid when none)
		UltimateFuryDroneRec()
			: m_fTimer( 0.f ), m_uidTarget( 0 ), m_iTargetType( 0 ), m_bIsNPC( false )
			, m_hMesh( INVALID_MESH_INSTANCE_HANDLE ), m_hShotDE( INVALID_DAMAGE_EFFECT_HANDLE ) {}
	};
	std::vector<UltimateFuryDroneRec>	m_vecUltFuryDrones;	// the V5 vector at this+26284
	float							m_fUltFuryElapsed;		// this+26300 - accumulates frameTime per shot-spawner call
	float							m_fUltFuryInterval;		// this+26304 - SA_DAMAGE_TIME (the per-target hit window, 3s)
	float							m_fUltFuryDuration;		// this+26308 - SA_EFFECTIVE_TIME (the attack duration, 6s)
	float							m_fUltFuryPower;		// this+26312 - GetPowerRate() x m_fInstallDmgScale
	int								m_iUltFurySound;		// this+26316 - the latched fire-sound (0 = none; ours = a one-shot latch, DESIGN INFERENCE per spec RISK-2)
	CX2EffectSet::Handle			m_hFactoryMiscES;			/// V5 (helper EffectSet handle destroyed on state start — newslot_315)
	float							m_vFactoryDroneLastPos[3];	/// drone follow smoothing (V5 tick lerps 0.9×current + 0.1×target per frame)

	CX2EqipPtr						m_pDEFormationFaceObj;

	CX2DamageEffect::CEffectHandle	m_hFactoryDroneDE;		/// V5 this+26088 (the persistent per-skill drone DamageEffect: DAMAGE_EFFECT_<skill>_Generate)
	void	ThirdPath_FactorySpawnDrone();					///< the per-skill drone DE spawn by the marked install skill (the CDF mode-enter + the FACT-4 install switch)
	void	ThirdPath_FactoryDronePlayMotion( const WCHAR* wcMotion_ );	///< play a motion on the drone DE's main mesh (V5: 'A' attack / 'W' wait)
	void	ANSI_HA_ADE_PARANOIA_FrameMove();				///< V5 - the @4.5s Hallucination debuff sweep (C14 D1)

	CX2EffectSet::Handle			m_hDynamoLockES;		/// V5 this+26112 (the lock-indicator EffectSet, spawned OWNED BY THE TARGET at the acquire)
	UidType							m_uidDynamoLockTarget;	/// V5 this+26120/26124 (the lock-target UID; 0 = the lock-not-set sentinel)
	bool							m_bDynamoLockIsNPC;		/// V5 byte 26128
	float							m_fDynamoLockRemainTime;/// V5 this+26132 (the per-frame countdown)
	bool							m_bDynamoLockArmed;		/// the one-shot latch -- ADAPTATION: the V5 latches on a 2015 DamageData field (==1 -> set to -1) that OUR DamageData does NOT have (verified); ours arms at the marker-DE spawn (the 9015 FM), consumes at the acquire, clears at the teardown -- identical one-shot semantics, zero struct/wire change
	float							m_fMagnetronStoredPower;/// V5 this+25892 (the magnetron cast power, captured at the orbit spawn; consumed by the lock re-aim)

	/// V5 helpers (manifest section 3 / chunk16 section 3.1): / / / / / /
	CX2GameUnit* ThirdPath_DynamoLockTargetValid();										///< V5 -- resolve the lock target by the stored UID; NULL unless it is alive AND not invincible
	void	ThirdPath_DynamoLockClear();												///< V5 -- destroy the lock ES + reset the lock members (swap/timeout/invalid teardown)
	bool	ThirdPath_DynamoLockValidOrClear();											///< V5 -- true iff the lock is set AND the target valid; else the teardown + false
	void	ThirdPath_DynamoLockTimeout( const float fElapsedTime_ );					///< V5 -- the per-frame lock-timeout tick (called from CommonFrameMove after the factory tick)
	void	ThirdPath_DynamoLockReAimOrbits();											///< V5 -- convert the live magnetron orbits into homing projectiles at the lock target (Dummy02 bone -> new Projectile DE -> retarget -> consume the old orbit)
	void	ThirdPath_DynamoLockRetarget( CX2DamageEffect::CEffect* pEffect );			///< V5 -- write the DE's lock-on fields at the lock target + curve speed 15 + power x1.2 + mesh dir-speed = m_fFactoryDroneSpeed
	void	ThirdPath_DynamoLockAcquire( CX2DamageManager::DamageData* pDamageData_ );	///< V5 -- the AttackResultByType hook: marker-DE hit + the armed latch -> store the lock + spawn the indicator ES on the target + re-aim the orbits

	CX2EffectSet::Handle			m_hPanzerAimBG;			/// V5 this+26212 (Panzer aiming BG EffectSet)
	CX2EffectSet::Handle			m_hATTComboZZXXAttackES;
	CX2EffectSet::Handle			m_hATTComboZZXXES;
	CX2DamageEffect::CEffectHandle	m_hPanzerAimDE;			/// V5 this+26216 (Panzer_Buster_Aiming_REDLINE DE)
	float							m_fPanzerChargeTime;	/// V5 this+26204 (aim hold time)
	float							m_fPanzerAimAngle;		/// V5 this+26208 (aim angle, ±50°/s, clamp ±30)
	bool							m_bPanzerAimSpawned;	/// once-gate for the AIMING spawns
	bool							m_abRushingDroneFlag[2];/// V5 bytes 26320/26321 (9208 formation/DP-pip flags)
	float							m_fXXDownXLoopSpawnAccum;/// AMM XXDownX LOOP spawn-interval accumulator (V5 window: current-last >= 0.05)
	CX2DamageEffect::CEffectHandle	m_hXXDownXLoopDE;		/// AMM XXDownX LOOP beam DamageEffect handle (V5; created once, repositioned per interval, destroyed on END)
	D3DXVECTOR3						m_vXXDownXAnchor;		/// AMM XXDownX LOOP beam tip anchor (V5 = hand+facing*750 default, homed to nearest hostile within reach)


	void ANSI_SA_AAT_PANZER_BUSTER_START_StateStart();		///< V5 (reset prior aim handles + ready sound)
	void ANSI_SA_AAT_PANZER_BUSTER_AIMING_FrameMove();		///< V5 (spawn REDLINE aim DE + Aiming_BG ES once + charge track)
	void ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_FrameMove();	///< V5 (angle += elapsed×50, clamp ≤30)
	void ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_FrameMove();	///< V5 (angle -= elapsed×50, clamp ≥-30)
	void ANSI_SA_AAT_PANZER_BUSTER_FIRE_FrameMove();		///< V5 (@0.444 spawn EffectSet_SI_SA_AAT_Panzer_Buster_Fire = the damage set)
	void ThirdPath_PanzerResetAim();						///< V5 (destroy aim DE + aim BG ES + zero charge/angle)
	void ANSI_SA_AAT_PSIONIC_GENERATOR_FrameMove();			///< V5 (@0.2s unit-count scale + spawn Start ES)
	void ANSI_A_AAT_RUSHING_DRONE_DYNAMO_StateStart();		///< V5 (DP-pip consume + formation flags)
	void ANSI_A_AAT_RUSHING_DRONE_DYNAMO_FrameMove();		///< V5 (drone spawn @0.4s + conditional @0.6s)
	void ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_FrameMove();	///< V5 gap (@0.4s spawn + suffix select + formation ×1.2)
	void ANSI_A_AMM_FLICK_DISC_DYNAMO_FrameMove();			///< V5 (@0.3s disc spawn + retarget + DP-pip)
	void ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_FrameMove();		///< V5 gap (@0.3s spawn projectile + retarget)
	void AMMComboXXDownXLoopFrameMove();					///< V5 (per-interval spawn DamageEffect_AMM_XXDownX_Loop)
	void AMMComboXXDownXEndFrameMove();						///< V5 (@0.01s spawn DamageEffect_AMM_XXDownX_Finish)

	int								m_iPAMemoCharge;		/// V5 this+26332 (SA1-4: the Particle Accelerator memo-609005 hold-to-charge counter)
	CKTDXDeviceSound*				m_hPanzerReadySound;	/// V5 this+26220 (ATMM-9: the LOOPED Panzer ready sound handle — freed at the FIRE SSF)
	void ANSI_SA_AAT_PANZER_BUSTER_START_StateStartFuture();	///< V5 (arm the key manager + play the ready sound looped)
	void ANSI_SA_AAT_PANZER_BUSTER_AIMING_StateStart();			///< V5 (spawn the REDLINE aim DE + the aiming-BG ES + the angle mesh-init)
	void ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_StateStartFuture();	///< V5 (the aim sound loop-restart at DEG entry)
	void ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_StateStartFuture();	///< V5 (same)
	void ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateStartFuture();		///< V5 (free the loop sound + the ready-sound handle)
	void ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateStart();			///< V5 (destroy the aim BG ES + the aim DE; the angle survives into the FIRE FM)
	void ThirdPath_PanzerApplyAimAngle();						///< V5 (write the aim angle onto the REDLINE DE's main mesh: {0, facing-rotY, angle})

	float							m_fMindControlDebuffReduceRate;	/// 9415 Mind Control SA_REDUCE_EDT_TIME_REL (V5; the debuff-duration scale - Phase-2 -override consumer)
	float							m_fMindControlProcRate;			/// 9415 SA_EFFECTIVE_RATE (V5; the cast-cost recovery proc rate - Phase 2)
	float							m_fMindControlRecoveryValue;	/// 9415 SA_RECOVERY_VALUE_REL=154 (V5; the recovered cost fraction - Phase 2)
	float							m_fTimeControlProcRate;			/// 9416 Time Control SA_EFFECTIVE_RATE (V5; the cooldown-reduction proc rate - Phase 2; PvP x0.5 at load)
	float							m_fTimeControlCooldownValue;	/// 9416 SA_COOLTIME_1 (V5; the seconds subtracted from the just-cast cooldown - Phase 2; PvP x0.5 at load)
	float							m_fMindCircleMpValue;			/// 9417 Mind Circle SA_ADD_MP_VALUE_ABS=155 (V5; MP back per consumed pip in formation - the ThirdPath_ConsumeDPPoints conversion)
	float							m_fMindCircleDpValue;			/// 9417 SA_ADD_DP_VALUE_ABS=156 (V5; DP back per consumed pip out of formation)
	float							m_fESPProcRate;					/// 9418 SA_EFFECTIVE_RATE (V5; the damage-taken-reduction proc rate - Phase 2)
	float							m_fESPDamagedRel;				/// 9418 SA_DAMAGED_REL (V5; the damage-taken scale - Phase 2)
	bool							m_bOverlimitEnabled;			/// 9414 Overlimit learned flag (V5 byte; Phase-2 gates)
	float							m_fOverlimitMpGainOnHit;		/// 9414 SA_MP_GAIN_ON_HIT_REL (V5; the formation MP-gain-on-hit factor - Phase 2)
	float							m_fOverlimitDpGain;				/// 9414 SA_DP_GAIN_REL (V5; the DP-gain economy factor - the direct-read consumer already lives at X2GUAdd.cpp:11155)
	float							m_fOverlimitDpUse;				/// 9414 SA_DP_USE_REL (V5; the DP-use economy factor - Phase 2)
	float							m_fOverlimitCooltime;			/// 9414 SA_COOLTIME_1 (V5; the per-skill Overlimit cooldown factor - Phase 2)

	std::vector<std::pair<int,float>>	m_vecOverlimitCoolList;		/// V5 vector // - the per-skill Overlimit cooldown list {skillID, remainSeconds}; pushed at 180.0f on a formation SA cast, ticked down per frame, pruned at <= 0
	virtual float						GetMPGainOnHitFactorByFormation();	/// V5 newslot_298/299 @/ (SS2): formation -> m_fOverlimitMpGainOnHit; else 1.0
	bool								ThirdPath_IsSAFamilyWithCooldown( IN const CX2SkillTree::SkillTemplet* pTemplet_, IN int iSkillLevel_ ) const;	/// V5 type gate {7,14,15,16} + templet has-cooldown gate
	bool								ThirdPath_IsOverlimitCoolListed( IN int iSkillID_ ) const;	/// V5 list lookup
	void								ThirdPath_OverlimitCoolListRegister( IN const CX2SkillTree::SkillTemplet* pTemplet_, IN int iSkillLevel_ );	/// V5 (cast-tail register/refresh/clear)
	void								ThirdPath_OverlimitCoolListTick( IN float fElapsed_ );	/// V5 prune + tick helper (per-frame, flag-gated only - ticks down out of formation too)
	void ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_EventProcess();
	void ANSI_SA_AAT_PANZER_BUSTER_START_Init();
	void ANSI_SA_AAT_PANZER_BUSTER_START_EventProcess();
	void ANSI_SA_AAT_PANZER_BUSTER_AIMING_EventProcess();
	void ANSI_SA_AAT_PANZER_BUSTER_DEGREE_UP_EventProcess();
	void ANSI_SA_AAT_PANZER_BUSTER_DEGREE_DOWN_EventProcess();
	void ANSI_SA_AAT_PANZER_BUSTER_FIRE_EventProcess();
	void ANSI_SA_AAT_PANZER_BUSTER_FIRE_StateEnd();				///< V5 FIRE SE (C-FACT-1 / RT-3: clear the aim camera offset, my-unit gated; fCameraDistance deliberately NOT cleared = the KOG asymmetry)
void ANSI_SA_AAT_PSIONIC_GENERATOR_Init();
void ANSI_SA_AAT_PSIONIC_GENERATOR_EventProcess();
void ANSI_SA_AAT_PHANTOM_SEEKER_Init();
	void ANSI_SA_AAT_PHANTOM_SEEKER_EventProcess();
	void ANSI_A_AAT_RUSHING_DRONE_DYNAMO_Init();
	void ANSI_A_AAT_RUSHING_DRONE_DYNAMO_EventProcess();
#endif

#ifdef X2_ADD_2ND_PATH
	void ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_Init();
	void ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_EventProcess();
	void ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_Init();
	void ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_EventProcess();
	void ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_Init();
	void ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_EventProcess();
	void ANSI_SA_AMM_INSTALLATION_STAR_FALL_Init();
	void ANSI_SA_AMM_INSTALLATION_STAR_FALL_EventProcess();
	void ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_Init();
	void ANSI_A_AMM_DIVIDING_MISSILE_DYNAMO_EventProcess();
	void ANSI_A_AMM_FLICK_DISC_DYNAMO_Init();
	void ANSI_A_AMM_FLICK_DISC_DYNAMO_StateStart();				///< V5 newslot_311 case 9210 SS (ATMM-10: zero the per-cast lock-valid flag + CommonStateStart)
	void ANSI_A_AMM_FLICK_DISC_DYNAMO_EventProcess();
	void ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_Init();
	void ANSI_A_AMM_COUNTDOWN_BOMB_DYNAMO_EventProcess();
	void ANSI_HA_AMM_APOCALYPSE_Init();
	void ANSI_HA_AMM_APOCALYPSE_EventProcess();

	void ANSI_HA_AMM_APOCALYPSE_FrameMove();	///< fix: V5 binds OnFrameMove (spawn/dismiss one-shot @0.01s), NOT StateStart
	void ThirdPath_ApocalypseFinishSummon();	///< V5 — find my alive Apocalypse summon (NPC 1604, owner=me) → force APOCALYPSE_FINISH_ATTACK
	void ThirdPath_ApocalypseClearMarker();		///< V5 — clear finisher flag + remove buff BTI_BUFF_APOCALYPSE + UpdateSkillSlotUI
	bool ThirdPath_IsApocalypseSummonActive() const;	///< const-safe buff-templet-345 scan (V5; HaveThisBuff is non-const so the const MP/cooltime overrides use this)

	void ANSI_SA_COMMON_CREST_OF_TIME_StateInit();
	void ANSI_SA_COMMON_CREST_OF_TIME_FrameMove();

	void ANSI_AAT_CREATE_DYNAMO_FACTORY_StateStartFuture();		///< V5 — apply BFI 366 (factory buff BTI 342) at cast-skill level + spawn the drone-generate EffectSet
	void ANSI_AAT_CREATE_DYNAMO_FACTORY_OnFrameMove();			///< V5 — generic cancel FM (anim-end→WAIT + cancel windows)
	void ANSI_AAT_CREATE_DYNAMO_FACTORY_StateEnd();				///< officially NO StateEnd bound for state 187 (the registrar binds {SS, EP } only; the cite was state-145 NA_LANDING's SE, not CDF's - corrected per C15 D9) - ours = the CommonStateEnd passthrough, net-identical
	void ANSI_SA_AAT_INSTALLATION_DRONE_ACTIVATOR_FrameMove();	///< V5 — @0.5s spawn DamageEffect_AAT_Dynamo_Factory_Projectile (Bip01_L_Hand, power×m_fInstallDmgScale)
	void ANSI_SA_AMM_INSTALLATION_PULSE_GATLING_FrameMove();	///< V5 — @0.01s spawn DamageEffect_AMM_Pulse_Gatling_Drone (Bip01) + duration track
	void ANSI_SA_AMM_INSTALLATION_SHOOTING_CHASER_FrameMove();	///< V5 — @0.4s spawn DamageEffect_AMM_Shooting_Chaser_Attacker + nearest-target capture
	void ANSI_SA_AMM_INSTALLATION_ULTIMATE_FURY_FrameMove();	///< V5 — @1.064s drone handle valid → phase=3 + drone Attack anim + 3× attack-drone mesh spawn
	void ANSI_SA_AMM_INSTALLATION_STAR_FALL_FrameMove();		///< V5 — @1.2s spawn DAMAGE_EFFECT_Star_Fall_Start_MESH + handle mgmt

	void ThirdPath_UltimateFurySpawnAttack();
	void ThirdPath_UltimateFuryAttackDriver();
	void ThirdPath_UltimateFuryAssignTargets();
	void ThirdPath_UltimateFuryClearSet();
	bool ThirdPath_IsInstallFamilySkill( const int iSkillID_ ) const;
	bool ThirdPath_IsMarkedInstallSkill( const int iSkillID_ ) const;	///< V5 — marker==skill
	void ThirdPath_FactoryModeClear();							///< V5 — marker=0 + phase=2 + flagWait + destroy drone handle + skill-slot UI refresh
	void ThirdPath_OnFactoryFrameMove( const float fElapsedTime_ );		///< V5 — per-frame drone follow + DP-expiry + mode-kill checks
	void ThirdPath_FactorySkillValueLoader();					///< V5 — load passive 9411/9412/9413 + 9015 values into the factory members
#endif

	void ANSI_CAN_NOT_ATTACK_JUMP_DOWN_EventProcess();

#endif
/// protected Function End


/// public Function Start

public:

	static CX2GUAdd*	CreateGUAdd( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit );

	static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)
	void SetForceNovaHitFlag() { m_bForceNovaHit = true; }	///< 9213 Force Nova hit-arm, called from CX2GUNPC::DamageReact (V5 DamageEffect owner-callbacks /)
	int  ThirdPath_GetInstallSlotOverlayIndex( const int iSkillID_ ) const;	///< V5 — install-skill slot overlay shows on the slot whose skill IS the marked factory skill (marker==skill, NO phase gate; else -1); called from X2UISkillTreeNew::UpdateSkillSlotOverlay

#endif
	// Ż ��
	virtual bool		CanRidingState();

	virtual void		RidingWaitEventProcess();
	virtual void		RidingWalkEventProcess();
	virtual void		RidingJumpLandingEventProcess();
	virtual void		RidingDashEventProcess();
	virtual void		RidingDashEndEventProcess();

	virtual void		ReInit( bool bTeamPos = false, int iStartPosIndex = -1 );
#ifdef X2_ADD_3RD_PATH


#ifdef X2_ADD_3RD_PATH
	void				CreateDEFormationFaceObj();											/// V5 -- build the berserk-face CX2Eqip (item 41841) at ReInit, hidden by default, parented to this GU
	void				ReleaseDEFormationFaceObj();										/// refcounted release (chain-remove first) -- the non-DE ReInit path / teardown
	void				UpdateDEFormationFace();											/// V5 -- show iff formation-mode ON && unit shown && not transformed
	virtual void		NotifyDGManagerChanged( CKTDGManager& refInManager_ );				/// V5 -- keep the face in the DGManager chain on scene attach/detach (device reset/lost, world swap)
#endif
#endif
	virtual void		DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ );

	virtual void		Win();
	virtual void		Lose();
	virtual void		Draw();

	virtual void		InitEffect();

	virtual void		CreateMajorParticleArray();
	virtual void		CreateMinorParticleArray();

	static void			AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );

	virtual bool		SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ );
	virtual void		COMMON_BUFF_FrameMove();

	virtual void		HyperModeFrameMove();

	void				RidingHyperModeFrameMove();
	void				CommonHyperModeFrameMove( float fTime1_, float fTime2_, bool bSound_ = false );

	virtual void		HyperModeBuffEffectStart();

	virtual void		SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
	virtual void		UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
#ifdef X2_ADD_3RD_PATH


	virtual void		RecordCrestSnapshot();
	virtual void		RestoreCrestSnapshot();


	virtual bool		IsBlockPacketBuffTemplet( int iBuffTempletID_ );

#endif
	virtual void		ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ );

	virtual void		InitEnchantWeaponEffectUnitSpecific();

	void				AttackResultByType( CX2DamageManager::DamageData &pDamageData );

	virtual	void		SetDamageExceptionProcess( OUT CX2DamageManager::DamageData* pDamageData_ ,OUT float& fPowerRate);

	virtual void		ApplyHyperModeBuff();

	/// ����ȭ ���� ó��
	SHORT			GetAbilitySyncData();
	void				SetAbilitySyncData( IN const SHORT sAbilitySyncData_ );

	/// Ư�� ������Ʈ ���� ���� ó��
	void				PlayAnimationWaitStartByAdd( IN CKTDGXSkinAnim* pXSkinAnim_, IN const FrameData& refFrameData_ );
	virtual bool		StateChange( int state, bool bDirectChange = false );
	virtual void		SetCustomAnimName( IN const UserUnitStateData& UserStateData, IN OUT wstring& wstrAnimName );

	/// ������ ����
	CX2AddGageData*		GetAddGageData() const;

	/// Ư�� ��ų�� ���� ó��
	bool			SpecialAttackCustomStateChange( IN const CX2SkillTree::SKILL_ID eSkillID_ );
	float			GetCustomSkillCoolTime( IN const float fCoolTime_, IN const CX2SkillTree::SKILL_ID eSkillID_ );


	bool			CheckSkillSpecialUse( IN const CX2SkillTree::SKILL_ID eSkillID_  );

	CX2SkillTree::ACTIVE_SKILL_USE_CONDITION GetSkillUseCondition( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ );

	/// ���� ��ġ
	int			GetMutationCount();																		/// ���� ��ġ ��ȯ �Լ�
	void				SetChangedMutationCount( IN const bool bChangedMutationCount_ );						/// ���� ��ġ ���� ���� ����
	virtual void		SetMutationCount( IN const int iMutationCount_ );										/// ���� ��ġ ���� �Լ�
	void				UpMutationCount( IN const int iMutationCount_  );										/// ���� ��ġ ���� �Լ�

	/// DP ��ġ
	float			GetMaxDPValue();																		/// �ִ� DP ��ġ ��ȯ �Լ�
	using CX2GUUser::GetDPValue;
	float			GetDPValue();																			/// DP ��ġ ��ȯ �Լ�
	virtual void		SetDPValue( IN const float fDPValue_ );													/// DP ��ġ ���� �Լ�
	virtual void		UpNowDPValue( IN const float fDPValue_, IN const float fChangeValueRate_ = 1.f );		/// DP ��ġ ���� �Լ�
	virtual void		UpNowDPValueByDamage( IN const float fDamage_ );										/// �ǰݽ� DP ���� ó��
	virtual void		UpNowDPValueByHit( IN const CX2DamageManager::DamageData& DamageData_ );				/// Ÿ�ݽ� DP ���� ó��
	void				UpNowDPValueByUseMP( IN const float fUseMP );											/// MP ���� DP ���� ó��
	float			GetActualDPConsume( IN const CX2UserSkillTree::SkillSlotData* pSkillSlotData_ );				/// DP �Ҹ�
	int			GetMutationPointConsume( IN const CX2UserSkillTree::SkillSlotData* pSkillSlotData_ );	/// ���� ����Ʈ �Ҹ�
	bool				IsDPConsumptionSkill( IN const CX2SkillTree::SKILL_ID eSkillID_ ) const;				/// �ش� ��ų�� DP�� ����ϴ� ��ų���� �˻�
	bool			ConsumeMPorDP( IN const float fConsumeMP, IN const float fConsumeDP );					/// MP Ȥ�� DP �Һ� �˻� �Լ�
	float			GetDPRateByType( IN const CX2DamageManager::DYNAMO_PARTICLE_RATE_TYPE eDPRateType );	/// DP ���� Ÿ�Կ� ���� ���� ��
	float 		GetUseDPByFormationMode();		 														/// ���� ��忡 ���Ǵ� DP ��
	float			GetEffectiveIncreaseDPValue();															/// DP ȹ�淮 ������ ��ȯ
	float			GetEffectiveDecreaseDPConsume();														/// DP �Ҹ� ���Ұ� ��ȯ
	virtual bool	GetUpDPByAttackThisFrame() { return m_bSetUpDPByAttackThisFrame; }						/// Ÿ�ݽ� �� �����ӿ� DP ȹ��� ���� Flag ��ȯ
	virtual void		SetUpDPByAttackThisFrame( IN const bool bval_ ) { m_bSetUpDPByAttackThisFrame = bval_; }/// Ÿ�ݽ� �� �����ӿ� DP ȹ��� ���� Flag ����
	virtual bool	GetUpDPByDamageThisFrame() { return m_bSetUpDPByAttackThisFrame; }						/// �ǰݽ� �� �����ӿ� DP ȹ��� ���� Flag ��ȯ
	virtual void		SetUpDPByDamageThisFrame( IN const bool bval_ ) { m_bSetUpDPByDamageThisFrame = bval_; }/// �ǰݽ� �� �����ӿ� DP ȹ��� ���� Flag ����


	/// ���� ���
	virtual bool	GetIsFormationMode() const;																/// ���� ��� ���� ���� ��ȯ �Լ�
	virtual void		SetIsFormationMode( IN const bool bIsFormationMode_ );									/// ���� ��� ���� ���� ���� �Լ�
	virtual float		GetHyperModeTime() const { return 9999999.f; }											/// ���� �ð� ��ȯ ( �ֵ�� ���� ���� �ð��� �����̴�. )
	bool			GetPossibleChangeFormationMode( IN const bool bOutputInformation = true );				/// ���� ��� ��� ���� ���� ��ȯ
	float			GetChangeFormationCoolTime();															/// ���� ��� ��ȯ ��Ÿ�� ��ȯ
	void				SetChangeFormationCoolTime( IN const float fCoolTime_ );								/// ���� ��� ��ȯ ��Ÿ�� ����

	bool				IsPossibleChangeNasodBarrier( IN const CX2DamageManager::DamageData* pDamageData_ );	/// ���ҵ� �踮�� ���� ó�� �Լ�
	void				SetNasodArmorAttackTarget( IN const CX2DamageManager::DamageData& pDamageData_ );		/// ���ҵ� �Ƹ� ���� Ÿ���� ���� �Լ�

	/// ���̳ʸ� ���� - ���Ϸ�
	void				AddPylonLinkDamageEffect( IN CX2DamageEffect::CEffect* pAddPylon_ );					/// ���Ϸ��� ���� ������ ����Ʈ �߰� �Լ�
	void				DeletePylonLinkDamageEffect( IN CX2DamageEffect::CEffectHandle hDeletePylonHandle_ );	/// ���Ϸ��� ���� ������ ����Ʈ ���� �Լ�
	void				ProcessPylonDamageEffect();																/// ���Ϸ� ���� �ð� ���� �Լ�
	bool				GetSummonPylonPos( OUT D3DXVECTOR3& fSummonPos );										/// ���Ϸ� ��ȯ ������ ��ġ ��ȯ �Լ�
	void				ResetPylonData();																		/// ���Ϸ� ���� ��� ���� �� ����Ʈ �ʱ�ȭ
	void				ProcessNonstopSummonPylon();															/// ���Ϸ� ���� ��ȯ ���� ����
	bool			GetPossibleNonstopSummonPylon() const;													/// ���Ϸ� ���� ��ȯ ���� ����

	/// ���̳ʸ� ���� - ���׳�Ʈ��
	void				ProcessMagnetronDamageEffect();															/// ���׳�Ʈ�� ��ġ ���� �Լ�
	void				AttackMagnetronDamageEffect( IN const CX2DamageManager::DamageData& pDamageData_ );		/// ���׳�Ʈ�� Ÿ���� ���� �Լ�

	/// �����̽ý� �ʵ�
	const StasisfieldData&	GetStasisfieldData() { return m_StasisfieldData; }									/// �����̽ý� �ʵ� ������ ��ȯ �Լ�
	bool			GetIsActiveStasisfield() { return m_StasisfieldData.GetIsActiveStasisfield(); }			/// �����̽ý� �ʵ� ���� ���� ��ȯ �Լ�
	virtual void		SetStasisfieldHitValue( IN const float fHitValue_ );									/// �����̽ý� �ʵ� Ÿ��ġ ���� �Լ�
	virtual void		SetStasisfieldDamageValue( IN const float fDamageValue_ );								/// �����̽ý� �ʵ� �ǰ�ġ ���� �Լ�
	void				ProcessStasisField();																	/// �����̽ý� �ʵ� �� ������ ó�� �Լ�
	void				EndStasisFieldByTime();																	/// ���� �ð��� ������, �����̽ý� �ʵ� ���� �Ǿ��� �� ���� ó�� �Լ�

	/// ���� ��ȭ
	float			GetPhaseShiftMPConsume() const { return m_PhaseShiftData.m_fReturnMPConsume; }
	bool			GetIsActivePhaseShift() const { return m_PhaseShiftData.GetIsActivePhaseShift(); }
	void				InitPhaseShift();

	/// �̸��� ��ǳ
	void				CreateDustStormToAttackResult( const CX2DamageManager::DamageData& damageData_ );
	void				CreateDustStormToDamageReact( const CX2DamageManager::DamageData& damageData_ );
	void				CreateDustStormDamageEffect( const float fDamageRate_, const D3DXVECTOR3& vPos_ );
	void				ProcessDustStormCoolTime();

	// ��ƼŬ ������
	void				DestroyMinePrismNPC() const;
	CX2EffectSet::Handle GetParticlePrismHandle() const { return m_hParticlePrsim; }

	// �ʿ����� ��ü
	void				ProcessBodyOfTranscendence( IN OUT CX2DamageManager::DamageData* pDamageData_ );

	// ��� ��ȯ��
	void				ProcessPowerExchanger();

	// ȯ�� ������
	bool			GetIsFantasyTracerTargetSkillID( IN const CX2SkillTree::SKILL_ID eSkillID_ )  const;
	void				SetUseFantasyTracerTarget( IN const CX2SkillTree::SKILL_ID eSkillID_ );
	void				ProcessFantasyTracerTargetInfo();

	void				SetDustAnatomyTarget( IN const CX2DamageManager::DamageData& pDamageData_ );

	// ������ ������
	void				ProcessReverseReactorTargetInfo();
	virtual void		SetReverseReactorTransitionDamage( IN const float fFinalDamage_ );

	/// ���ǹ� ������
	void				ProcessQuicksilverFrenzy();
	void				AttackQuicksilverFrenzy( IN const CX2DamageManager::DamageData& pDamageData_, IN const bool bIsAttack_ = false );
	const D3DXCOLOR 	SetQuicksilverFrenzyFlickerColor( IN const D3DXCOLOR& coRenderColor_ );

#if defined(X2_ADD_2ND_PATH) || defined(X2_ADD_3RD_PATH)

	virtual float		GetReduceDebuffDurationRate( BUFF_TEMPLET_ID eBuffTempletID_ );	///< V5 CX2GUAdd:: @ (9415 Mind Control: the 8-BTI debuff-duration scale; CX2GameUnit base @ X2GameUnit.h = return 1.f)
	virtual float		GetESPProcDamageTakenScale();										///< V5 CX2GUAdd:: @ (9418 ESP: per-hit damage-taken proc; CX2GameUnit base = return 1.f)
	void				ThirdPath_MindControlCostRecoveryProc();							///< V5 @ (9415: roll vs m_fMindControlProcRate -> recover m_fMindControlRecoveryValue x the just-consumed MP/DP cost; formation = DP + "DP+ %d", else MP + "MP+ %d")
	void				ThirdPath_TimeControlCooldownProc( const CX2SkillTree::SKILL_ID eCastSkillID_, const float fJustSetCoolTime_ );	///< V5 @ (9416: roll vs m_fTimeControlProcRate -> subtract m_fTimeControlCooldownValue seconds from the just-cast skill's slot cooldown; guards 9025/9028/9205 = disasm-verified)

#endif
/// public Function End


/// protected Valuable Start

protected:

	const CX2SkillTree::SkillTemplet*	m_pSkillTemplet;
	int		m_iPressedSkillSlotIndex;
	float	m_fMPConsume;				/// MP �Ҹ�
	float	m_fDPConsume;				/// DP �Ҹ�
	int		m_iMutationCountConsume;	/// ���� ������ �Ҹ�

	int		m_iPreComboCount;

	bool	m_bEnableAirDash;					/// ���� ��� ���� ����
	bool	m_bEnableLevitationWait;			/// ���� �ξ� ���� ����

	int		m_iComboLoopCount;					/// �ݺ� �޺��� ���� �ݺ���

	float	m_fRenderParamColorA;				/// ĳ���� ���İ� ����

	float	m_fChargeTime;						/// ���� �ð�
	int		m_iChargeCount;						/// ���� Ƚ��

	float	m_fChangeFormationCoolTime;			/// ���� ��� ��ȯ ��Ÿ��

	float	m_fDPRateBySpecialActive;			/// ����� ��Ƽ�� ������ ���� DP ȹ�淮 ����
	float	m_fDPRateByActive;					/// ��Ƽ�� ������ ���� DP ȹ�淮 ����
	float	m_fDPRateByCommand;					/// Ŀ�ǵ� ������ ���� DP ȹ�淮 ����
	float	m_fDPRateByNasodArmor;				/// ���ҵ� �Ƹ� ������ ���� DP ȹ�淮 ����

	float	m_fDPRateByFormationModeAttack;		/// ��������� Ÿ�ݽ� DP �Ҹ� ���� ����
	float	m_fDPRateByFormationModeHit;		/// ��������� �ǰݽ� DP ȹ�淮 ���� ����
	float	m_fDPRateByFormationModeUseMP;		/// ��������� MP ���� DP ȹ�淮 ���� ����

	float	m_fDPRateByNoFormationModeAttack;	/// ������尡 �ƴ� �� Ÿ�ݽ� DP ȹ�淮 ���� ����
	float	m_fUseDPByNasodBarrier;				/// ���ҵ� �踮�� �ߵ��� DP �Ҹ� ��ġ

	float	m_fUseDPByEnableFormationMode;		/// ���� ���� ��ȯ�� ���� �����ؾ� �ϴ� �ּ� DP ��ġ
	float	m_fUseDPByFormationMode;			/// ���� ���� ��ȯ�� DP �Ҹ� ��ġ

	bool	m_bPossibleNasodArmorModeState;		/// ���ҵ� �踮�� �ߵ� ������ ������Ʈ �ΰ�

	bool	m_bSetUpDPByAttackThisFrame;		/// Ÿ�ݽ� �� �����ӿ� �ѹ��� DP ȹ�� �ǵ��� ����
	bool	m_bSetUpDPByDamageThisFrame;		/// �ǰݽ� �� �����ӿ� �ѹ��� DP ȹ�� �ǵ��� ����


	// ��ƼŬ ������
	CX2EffectSet::Handle m_hParticlePrsim;		/// �帷 ���� ���� �� ���� ���̱� ���� �ڵ� ����

	// Loop ������Ʈ�� ���� ���Ḧ ���� ����
	CKTDXDeviceSound*	m_pLoopSound;

	/// Ư���� ��Ȳ������ ���� ���� ����
	CUSTOM_RENDER_STATE	m_eCustomRenderState;

//////////////////////////////////////////////////////////////////////////
// �нú� ��ų ����
	// ���̳ʸ� ����-��ü
	float	m_fGainDPValueChangeRate;			/// ȹ�淮 ���� ����
	float	m_fNotUsingDPSkillDamageRate;		/// �� DP ��ų ������ ����

	// ���̳ʸ� ����-����
	float	m_fUsingDPValueChangeRate;			/// �Ҹ� ���� ����
	float	m_fUsingDPSkillDamageRate;			/// DP ��ų ������ ����

	// ���ҵ� �Ƹ� ���
	float	m_fNaCommandDamageRate;				/// ���ҵ� �Ƹ� Ŀ�ǵ� ������ ����
	float	m_fDPValueRateByActiveFormationMode;/// ���� ���� ��ȯ �� �Ҹ�Ǵ� DP���� ���� ��Ű�� ����

	// ������ ������
	int		m_iLibraryOfLimitlessLevel;			/// ������ ������ ���� ������ ���� ���� ��

	// �̸��� ��ǳ
	float	m_fDustStormRateByHit;				///  Ÿ�ݽ� �ߵ� Ȯ��
	float	m_fDustStormRateByDamage;			///  �İݽ� �ߵ� Ȯ��
#if defined(X2_ARA_3RD_PATH) || defined(X2_ADD_3RD_PATH)

	float	m_fVoidImpactDownAngle;		/// V5 this+25616 (lua VOID_IMPACT_DOWN_ANGLE, default -60)
	float	m_fVoidImpactUpAngle;		/// V5 this+25620 (lua VOID_IMPACT_UP_ANGLE, default +60)
	// MISC-1 (merged C10 D10 + C11 D6 + C12 D8 + C13 D4 + C14 D2 + C13 section2.0a): the V5 per-skill StateInit asset-preload bodies — the 3rd-path skills had NO StateInit bound (the ANSI_ThirdPath_CreateState helper never sets StateInit); the lists = reports\misc1_preload_lists.md (349 calls / 167 unique assets / 166 verified present in the deployed koms); normal-state bind only — the V5 hyper copy gets NO Init //
	void ANSI_SA_ATT_VOID_IMPACT_StateInit();
	void ANSI_SA_ATT_VOID_FIELD_StateInit();
	void ANSI_SA_ATT_MAXIMUM_STRIKE_StateInit();
	void ANSI_SA_ATT_STARDUST_SHOWER_StateInit();
	void ANSI_SA_ADE_REVERSE_CIRCLE_StateInit();
	void ANSI_SA_ADE_REVERSE_STIGMA_StateInit();
	void ANSI_SA_ADE_MOONLIGHT_RHAPSODY_StateInit();
	void ANSI_SA_ADE_GRAVITY_BURSTER_StateInit();
	void ANSI_A_ATT_SPACE_CRACK_StateInit();
	void ANSI_A_ADE_FORCE_NOVA_StateInit();
	void ANSI_A_ADE_FORCE_FINGER_StateInit();
	void ANSI_HA_ADE_PARANOIA_StateInit();

#endif
	float	m_fDustStormCoolTime;				///  �ߵ� ��Ÿ��



	// �ʿ����� ��ü
	float	m_fBodyOfTranscendenceActiveRate;	/// �ߵ� Ȯ��
	float	m_fBodyOfTranscendenceMoveSpeed;	/// �̵� �ӵ� ����ġ

	// ��� ��ȯ��
	bool								m_bEnablePowerExchanger;				/// ���� ����
	POWER_EXCHANGER_STATE				m_ePowerExchangerState;					/// ���� ��Ȳ

	/// ȯ�� ������
	bool								m_bEnableFantasyTracer;					/// ���� ����
	float								m_fFantasyTracerUpMPRateByFormationMode;/// ���� ��� ��ȯ�� ȸ�� MP ����
	vector<FantasyTracerTargetInfo>		m_vecFantasyTracerTargetInfo;			/// Ÿ�� ���� �����̳�
	CX2SkillTree::SKILL_ID				m_eNasodArmorAttackActiveSkillID;		/// ���ҵ� �Ƹ� ������ ������ ��ų ���̵�
	CX2SkillTree::SKILL_ID				m_eNasodArmorLandingActiveSkillID;		/// ���ҵ� �Ƹ� ������ ������ ��ų ���̵�


//////////////////////////////////////////////////////////////////////////

	NasodArmorAttackTargetData			m_NasodArmorAttackTargetData;		/// ���ҵ� �Ƹ� ���� Ÿ�� ����

	/// ���̳ʸ� ���� - ���Ϸ�
	float								m_fNonstopSummonPylonCoolTime;		/// ���Ϸ� ���� ��ȯ ��Ÿ��
	UINT								m_uiNonstopSummonPylonCount;		/// ���Ϸ� ���� ��ȯ ����
	vector<PylonLinkDamageEffectData>	m_vecPylonLinkDamageEffectData;		/// ���̳ʸ� ���Ϸ� ���� ������ ����Ʈ ���� �����̳�
	vector<PylonDamageEffectData>		m_vecSummonPylonDamageEffectData;	/// ��ȯ�� ���̳ʸ� ���Ϸ� ������ ����Ʈ ���� �����̳�

	/// ���̳ʸ� ���� - ���׳�Ʈ��
	MagnetronData						m_MagnetronData;					/// ���׳�Ʈ�� ���� ��ü

	/// �����̽ý� �ʵ�
	StasisfieldData						m_StasisfieldData;					/// �����̽ý� �ʵ� ��ġ ���� ��ü

	/// ���� ��ȭ
	PhaseShiftData						m_PhaseShiftData;					/// ���� ��ȭ ���� ��ü

	/// ����ź
	ChargedImpulsarData					m_ChargedImpulsarData;				/// ����ź - �������� ��ü

	/// �޽� ĳ��
	vector<int>							m_vecPulseBulletFireIndex;			/// �޽� ĳ�� �߻� �ε��� ���� ��ü

	/// ����Ʈ �Ƴ����
	vector<DustAnatomyTargetInfo>		m_vecDustAnatomyTargetInfo;			/// ����Ʈ �Ƴ���� ���� ����Ʈ�� Ÿ�� ����

	/// ������ ������
	ReverseReactorTargetInfo			m_ReverseReactorTargetInfo;			/// ������ ������ ���� ��ü

	/// ���ǹ� ������
	QuicksilverFrenzyData				m_QuicksilverFrenzyData;			/// ���ǹ� ������ ���� ��ü
	float								m_fQuicksilverFrenzyFlickerTime;	/// ��¦�̴� �ð�

	/// ���ǹ� �׼�
	CX2DamageEffect::CEffectHandle		m_eQuicksilverAccelLinkEffect;		/// ���ǹ� �׼� ���� ����Ʈ

	/// ����ű ����
	PsychicStormData					m_PsychicStormData;					/// ����ű ���� ���� ��ü

	/// �ҽ� ����
	vector<CX2DamageEffect::CEffectHandle> m_vecDoomsDayLinkEffect;			/// ���� ����Ʈ ���� ��ü

/// protected Valuable End

};
