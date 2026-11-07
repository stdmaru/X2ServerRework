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

/// �ֵ��� ���ҵ� �尩 ��带 ����� �� �ִ���
#define ELSE_IF_CAN_NASOD_ARMOR_ATTACK( iEventFlagIndex, fInputTimingStart, fInputTiming, fChangeTiming, bIsFormationMode, uidTargetUID ) \
	else if( m_FrameDataFuture.stateParam.bEventFlagList[(int)(iEventFlagIndex)] == true && \
			 m_pXSkinAnimFuture->GetNowAnimationTime() > (float)(fChangeTiming) && (UidType)uidTargetUID > 0 ) \
			{ StateChange( CX2GUAdd::ANSI_COMBO_NASOD_ARMOR_READY ); m_FrameDataFuture.stateParam.bEventFlagList[(int)(iEventFlagIndex)] = false; } \
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

/// protected Function End


/// public Function Start

public:

	static CX2GUAdd*	CreateGUAdd( int unitIndex, int teamNum, lua_State*   pScriptLuaState, int frameBufferNum, CX2Unit* pUnit );

	static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );

	// Ż ��
	virtual bool		CanRidingState();

	virtual void		RidingWaitEventProcess();
	virtual void		RidingWalkEventProcess();
	virtual void		RidingJumpLandingEventProcess();
	virtual void		RidingDashEventProcess();
	virtual void		RidingDashEndEventProcess();

	virtual void		ReInit( bool bTeamPos = false, int iStartPosIndex = -1 );
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
