#pragma once
/**
	@file	X2GUAra.h
	@author	JHKang
	@brief	�ƶ� Ŭ������ ���ǵ� ����
	@since  2012-11-06

	@date	JHKang on 2012-11-14,
			- h �� cpp ���� �۾� �簳
	@date	JHKang on 2012-11-18,
			- �߰� �ּ��� �߰�( file ���� )
	@data	JHKang on 2012-11-28,
			- �⺻ �޺� 23�� �Ϸ�
	@data	JHKang on 2012-11-28,
			- Ư�� �̵��� ����( �ɰ��㵵, 2�� ���� )
	@data	JHKang on 2012-11-29,
			- ��ų 3�� ���� �ӽ�( �ٶ� ����, ���� �α�, ȣ���� ���� )
	@data	JHKang on 2012-12-06,
			- ��� �ý��� �߰� �� ����
	@data	JHKang on 2012-12-07
			- ��ų ĵ���� ���� ��� ����, ���� ����
	@data	JHKang on 2013-01-04
			- �ƶ� 1-1, 1-2�� ���� ����( �нú� : �����̴� ����, â���� ���� )
	@data	JHKang on 2013-01-08
			- �ƶ� 1-1, 1-2�� ���� ( ��Ƽ�� : ��ȣ 1�� - ���� ���� )
	@data	JHKang on 2013-01-10
			- �ƶ� 1-1, 1-2�� ���� ( ����� ��Ƽ�� : ��� 4�� - �޺� ���� )
	@data	JHKang on 2013-01-11
			- �ƶ� 3���� ������ ��ü ���� ����
	@data	JHKang on 2013-01-15
			- �ƶ� 1-1, 1-2�� ���� ( ����� ��Ƽ�� ĳ�� 3�� : �� )
	@data	JHKang on 2013-01-17
			- �ƶ� 1-1, 1-2�� ���� ( ��Ƽ�� : ��ȣ 3�� ���� )
	@data	JHKang on 2013-03-28
			- �ƶ� ����ȭ ����� �� �� �� �ƹ�Ÿ ���� ���� ( FIX_ARA_ONE_PIECE_FOR_BERSERK )
	@data	JHKang on 2013-06-04
			- �ƶ� ��ų ����( �ż�, ö���, ����, õ��, ���� ����, ��ȣ�� ��, ȣ�Ű���, ����� ��� )
	@data	JHKang on 2013-06-20
			- �ƶ� ��ų ����( ���� ��� �Ҹ� �� ������ ���� ���� )
	@data	JHKang on 2013-07-02
			- ��ȭ ���� ����
*/


//#pragma region CX2GUAra
/** @class : CX2GUAra
	@brief : 2012 �� ĳ���� "�ƶ�" Ŭ����
	@date  : 2012-11-06
	@desc  : �� â�� ����ϴ� ĳ����, ����ȣ�� ���� ���
*/
class CX2GUAra : public CX2GUUser
{
	//#pragma region public
public:
//#pragma region enum
	enum ARA_STATE_ID
	{
	//#pragma region STATE_ID
		ASI_BASE = USI_END + 1,	/// �ƶ� State ID

		ASI_HYPER_WAIT,			/// ������ �� ��� ����
		ASI_BERSERK_MODE,		/// 3���� ����

		ASI_DASH_JUMP_POWER_LANDING,		/// ������ �����ϱ�

		ASI_STANDUP_ROLLING_FRONT_FRONT,	/// �տ��� ������ ������ �Ͼ��
		//////////////////////////////////////////////////////////////////////5
		ASI_STANDUP_ROLLING_FRONT_BACK,		/// �տ��� �ڷ� ������ �Ͼ��
		ASI_STANDUP_ROLLING_BACK_FRONT,		/// �ڿ��� ������ ������ �Ͼ��
		ASI_STANDUP_ROLLING_BACK_BACK,		/// �ڿ��� �ڷ� ������ �Ͼ��

		ASI_STANDUP_ATTACK_FRONT,			/// ���� �����ϸ鼭 �Ͼ��
		ASI_STANDUP_ATTACK_BACK,			/// �Ĺ� �����ϸ鼭 �Ͼ��
		//////////////////////////////////////////////////////////////////////10
		ASI_WALL_LANDING,		/// ����
		ASI_AIRIAL_DASH,		/// ���� �޸���_�ɰ��㵵
		ASI_DOUBLE_JUMP_FRONT,	/// �̴� ����_��
		ASI_DOUBLE_JUMP_UP,		/// �̴� ����_��

		ASI_COMBO_Z,			/// �޺� Z
		//////////////////////////////////////////////////////////////////////15
		ASI_COMBO_ZZ,			/// �޺� ZZ
		ASI_COMBO_ZZZ,			/// �޺� ZZZ
		ASI_COMBO_ZZZZ,			/// �޺� ZZZZ
		ASI_COMBO_ZZZXStart,	/// �޺� ZZZXStart
		ASI_COMBO_ZZZXCharge,	/// �޺� ZZZXCharge
		//////////////////////////////////////////////////////////////////////20
		ASI_COMBO_ZZZXAttack,	/// �޺� ZZZXAttack
		ASI_COMBO_ZZX,			/// �޺� ZZX

		ASI_COMBO_X,			/// �޺� X
		ASI_COMBO_XX,			/// �޺� XX
		ASI_COMBO_XXX,			/// �޺� XXX
		//////////////////////////////////////////////////////////////////////25
		ASI_COMBO_XXXloop,		/// �޺� XXXloop
		ASI_COMBO_XXXloopX,		/// �޺� XXXloopX
		ASI_COMBO_XXdownX,		/// �޺� XXdownX
		ASI_COMBO_XXdownXX,		/// �޺� XXdownXX_��ȣ 1��

		ASI_DASH_COMBO_Z,				/// �뽬 �޺� Z
		//////////////////////////////////////////////////////////////////////30
		ASI_DASH_COMBO_ZpushX_CHARGE,	/// �뽬 �޺� ZpushX ����
		ASI_DASH_COMBO_ZpushX_ATTACK,	/// �뽬 �޺� ZpushX ����
		ASI_DASH_COMBO_X,				/// �뽬 �޺� X

		ASI_JUMP_ATTACK_Z,			/// ���� ���� Z
		ASI_JUMP_ATTACK_X,			/// ���� ���� X
		//////////////////////////////////////////////////////////////////////35
		ASI_DASH_JUMP_COMBO_Z,		/// �뽬 ���� �޺� Z
		ASI_DASH_JUMP_COMBO_ZZ,		/// �뽬 ���� �޺� ZZ
		ASI_DASH_JUMP_COMBO_ZZZ,	/// �뽬 ���� �޺� ZZZ
		ASI_DASH_JUMP_COMBO_X,		/// �뽬 ���� �޺� X

		ASI_SA_ARA_WIND_WEDGE_START,		/// ��ǳ��_�ٶ� ����_���� 2�� ù��° ����
		//////////////////////////////////////////////////////////////////////40
		ASI_SA_ARA_WIND_WEDGE_END,			/// ��ǳ��_�ٶ� ����_���� 2�� �ι�° ����

		ASI_SA_ARA_SHADOW_RISING_JUMP,		/// ���·�_õ�� �ɸ���-����
		ASI_SA_ARA_SHADOW_RISING_LANDING,	/// ���·�_õ�� �ɸ���-���� ����
		ASI_SA_ARA_SHADOW_RISING_END,		/// ���·�_õ�� �ɸ���-������ ����

		ASI_SA_ARA_PULLING_PIERCE_START,	/// õ��_���� �α� ù��° ����
		//////////////////////////////////////////////////////////////////////45
		ASI_SA_ARA_PULLING_PIERCE_END,		/// õ��_���� �α� �ι�° ����

		ASI_SA_ARA_TEMPEST_DANCE_START,		/// ��ǳ_�ָ� ù��° ����
		ASI_SA_ARA_TEMPEST_DANCE_END,		/// ��ǳ_�ָ� �ι�° ����

		ASI_SA_ARA_SHADOW_KNOT_START,		/// �῵_�׸��� �ŵ�_�ͻ� 3��_����
		ASI_SA_ARA_SHADOW_KNOT_WAIT,		/// �῵_�׸��� �ŵ�_�ͻ� 3��_���
		//////////////////////////////////////////////////////////////////////50
		ASI_SA_ARA_SHADOW_KNOT_CATCH,		/// �῵_�׸��� �ŵ�_�ͻ� 3��_���� �ޱ�
		ASI_SA_ARA_SHADOW_KNOT_END,			/// �῵_�׸��� �ŵ�_�ͻ� 3��_����

		ASI_SA_ARA_FALLING_DRAGON,		/// ������_���� ������

		ASI_A_ARA_TIGER_CLAW_START,		/// ȣ���� ����_��ȣ 2��_ù��° ����
		ASI_A_ARA_TIGER_CLAW_END,		/// ȣ���� ����_��ȣ 2��_�ι�° ����
		//////////////////////////////////////////////////////////////////////55

		ASI_A_ARA_FORCE_SPEAR,			/// ���â

		ASI_A_ARA_GAIN_FORCE,			/// ����
	//#pragma endregion ������

	//#pragma region ù ��° ����
		//�ƶ� 1�� ���� - �Ҽ�(Little Hsien)
		ASI_COMBO_ALH_ZZXX,					/// �޺�ZZXX
		ASI_COMBO_ALH_ZZXX_LANDING,			/// �޺�ZZXX ����
		ASI_DASH_COMBO_ALH_ZZ,				/// �뽬 �޺� ZZ
		//////////////////////////////////////////////////////////////////////60
		ASI_JUMP_ATTACK_ALH_ZZ,				/// ���� ���� ZZ

		ASI_STANDUP_ATTACK_FRONT_NO_DOWN,	/// �����߸��� �ʴ� ���� �����ϸ鼭 �Ͼ��
		ASI_STANDUP_ATTACK_BACK_NO_DOWN,	/// �����߸��� �ʴ� �Ĺ� �����ϸ鼭 �Ͼ��

		ASI_P_ALH_QUICK_STAND_FRONT_FRONT,	/// ����� ���
		ASI_P_ALH_QUICK_STAND_FRONT_BACK,
		//////////////////////////////////////////////////////////////////////65
		ASI_P_ALH_QUICK_STAND_BACK_FRONT,
		ASI_P_ALH_QUICK_STAND_BACK_BACK,

		ASI_A_ALH_BREAKING,					/// ��ȭ���� ���
		ASI_A_ALH_BREAKING_ATTACK,			/// ��ȭ���� ����(�ݰ�)
		ASI_A_ALH_BREAKING_AVOIDANCE,		/// ��ȭ���� ȸ��
		//////////////////////////////////////////////////////////////////////70
		ASI_A_ALH_ROCK_CRASH_START,		/// ���� ����
		ASI_A_ALH_ROCK_CRASH_END,

		ASI_SA_ALH_DOUBLE_COLLISION_START,	/// �ȱ��� ù��° ����
		ASI_SA_ALH_DOUBLE_COLLISION_END,	/// �ȱ��� �ι�° ����

		ASI_SA_ALH_MOONLIGHT_SLASH_READY,		/// �޺� ���� ����
		//////////////////////////////////////////////////////////////////////75
		ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE,		/// �޺� ���� ����
		ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK,	/// �޺� ����(�ʽ´�)
		ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK,	/// �޺� ����(������)

		ASI_SA_ALH_POWER_OF_WHITER_TIGER,	/// ��ȣ��
		ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING,
		//////////////////////////////////////////////////////////////////////80
		ASI_SA_ALH_PEERLESSNESS_SLASH_START,	/// ������
		ASI_SA_ALH_PEERLESSNESS_SLASH_END,

		ASI_SA_ALH_DESCENT_DRAGON,				/// ��� �������� - ����

		//�ƶ� 2�� ���� - ��õ(Sakra Devanam)
		ASI_DASH_JUMP_COMBO_ASD_ZX,		/// �뽬 ���� ZX
		ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK,	/// �̴� ���� �޺� Z Back
		//////////////////////////////////////////////////////////////////////85
		ASI_P_ASD_QUICKEN_GUARD,		/// �����̴� ���� ���
		ASI_P_ASD_QUICKEN_GUARD_SMALL,	/// �����̴� ���� SMALL_DAMAGE ���

		ASI_SA_ASD_TURBULENT_WAVE_START,	/// ��ȣ 4�� - �뵵
		ASI_SA_ASD_TURBULENT_WAVE_LOOP,
		ASI_SA_ASD_TURBULENT_WAVE_END,
		//////////////////////////////////////////////////////////////////////90
		ASI_SA_ALH_SWALLOW_ASSAULT_READY,	/// �� ����
		ASI_SA_ALH_SWALLOW_ASSAULT_ATTACK,	/// �� ����
		ASI_SA_ALH_SWALLOW_ASSAULT_FINISH,	/// �� ����
		ASI_SA_ALH_SWALLOW_ASSAULT_ESCAPE,	/// �� Ż��
		ASI_A_ASD_LOW_BRANDISH_SLICE,		/// ���� �����̵�
		//////////////////////////////////////////////////////////////////////95
		ASI_A_ASD_LOW_BRANDISH_KICK,		/// ���� ű
		ASI_SA_ASD_VOID_BREAK,				/// �������� ��ȣ ��õ
		ASI_SA_ASD_SUPPRESSION,				/// ����
		ASI_A_ASD_FALLING_BLOSSOM_FINISH,	/// ��ȭ
		//////////////////////////////////////////////////////////////////////100
	//#pragma endregion 1-1, 1-2�� ����

	//#pragma region �� ��° ����
		ASI_COMBO_ALD_ZZXX,
		ASI_COMBO_ALD_ZZZFrontZ,
		ASI_DASH_JUMP_COMBO_ALD_ZX,
		ASI_DASH_JUMP_COMBO_ALD_ZXX,
		ASI_DASH_JUMP_COMBO_AYR_XZ,
		//////////////////////////////////////////////////////////////////////105
		ASI_DASH_COMBO_AYR_ZZ,
		ASI_A_AYR_HIGH_SPEED,				/// �ƶ� 1�� ���� �Ҹ� - ���
		ASI_A_ALD_WOLF_CLAW,				/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� �������
		ASI_A_ALD_WOLF_CLAW_FINISH,			/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� ������� ������
		ASI_SA_ALD_HELL_OF_TORNADO_FINISH,	/// �ƶ� 1�� ���� �Ҹ� - ��ȯ����
		//////////////////////////////////////////////////////////////////////110
		ASI_SA_ALD_WOLF_SPEAR,				/// �ƶ� 1�� ���� �Ҹ� - ���� 4�� �����̻�
		ASI_SA_ALD_WOLF_SPEAR_FINISH,		/// �ƶ� 1�� ���� �Ҹ� - ���� 4�� �����̻� ������
		ASI_SA_ALD_WOLF_BREAK,				/// �ƶ� 1�� ���� �Ҹ� - ���� �������� - �ؼ�
		ASI_SA_ALD_FINGER_BULLET,			/// �ƶ� 1�� ���� �Ҹ� - ���� 1�� ��ȥ
		ASI_SA_ALD_FINGER_BULLET_FINISH,	/// �ƶ� 1�� ���� �Ҹ� - ���� 1�� ��ȥ ������
		//////////////////////////////////////////////////////////////////////115
		ASI_SA_ALD_ENERGY_BEAD,				/// �ƶ� 1�� ���� �Ҹ� - ���� 2�� ��ȯ��
		ASI_SA_ALD_ENERGY_BEAD_FINISH,		/// �ƶ� 1�� ���� �Ҹ� - ���� 2�� ��ȯ�� ������
		ASI_SA_ALD_PRISON_SPEAR,			/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� â�차��
		ASI_SA_ALD_PRISON_SPEAR_FINISH,		/// �ƶ� 1�� ���� �Ҹ� - ���� 3�� â�차�� ������
		ASI_A_AYR_STEAL_SOUL,				/// �ƶ� 2�� ���� ���� - ���� 4�� Żȥ
		//////////////////////////////////////////////////////////////////////120
		ASI_A_AYR_STEAL_SOUL_FINISH,		/// �ƶ� 2�� ���� ���� - ���� 4�� Żȥ ������
		ASI_SA_AYR_OPEN_THE_HELL,			/// �ƶ� 2�� ���� ���� - ���� �������� - ����
		ASI_SA_AYR_ENERGY_VOID_START,		/// �ƶ� 2�� ���� ���� - �������� �غ�
		ASI_SA_AYR_ENERGY_VOID_LOOP,		/// �ƶ� 2�� ���� ���� - �������� ����
		ASI_SA_AYR_ENERGY_VOID_END,			/// �ƶ� 2�� ���� ���� - �������� ������
		//////////////////////////////////////////////////////////////////////120
		ASI_SA_AYR_SUPPRESSION_ENERGY,		/// �ƶ� 2�� ���� ���� - ���� - ��
	//#pragma endregion 2-1, 2-2�� ����

	};
	//#pragma endregion ARA_STATE_ID

	//#pragma region enum
	enum ARA_MAJOR_PARTICLE_INSTANCE_ID
	{
		ARA_MAJOR_PII_BASE,
		ARA_MAJOR_PII_END,
	};
	//#pragma endregion ARA_MAJOR_PARTICLE_INSTANCE_ID

	//#pragma region enum
	enum ARA_MINOR_PARTICLE_INSTANCE_ID
	{
		ARA_MINOR_PII_BASE,
		ARA_MINOR_PII_END,
	};
	//#pragma endregion ARA_MINOR_PARTICLE_INSTANCE_ID

	//#pragma region enum
	enum ARA_MAJOR_MESH_INSTANCE_ID
	{
		ARA_MAJOR_MII_BASE,
		ARA_MAJOR_MII_END,
	};
	//#pragma endregion ARA_MAJOR_MESH_INSTANCE_ID

	//#pragma region class
	/** @class : CForcePowerSystem
		@brief : ��� �ý���
		@since  2012-11-06
		@date	JHKang on 2012-11-30,
				- ���� �� ������
	*/
	class CForcePowerSystem
	{
	private:
		int			m_iUsedForcePowerAtThisState;	/// ���� ������Ʈ���� �Ҹ��� ����� ����

	public:
		CForcePowerSystem( void ) : m_iUsedForcePowerAtThisState( 0 )
		{}

		int GetUsedForcePowerAtThisState() const
		{
			return m_iUsedForcePowerAtThisState;
		}

		void SetUsedForcePowerAtThisState( int iUsedForcePowerAtThisState_)
		{
			m_iUsedForcePowerAtThisState = iUsedForcePowerAtThisState_;
		}

	};
	//#pragma endregion CForcePowerSystem

	//#pragma region struct
	/** @struct SMoonLightSlashData
		@brief  �޺� ���� ������
		@since  2012-11-06
		@date	JHKang on 2013-1-14,
				- ����-���ϵ� ���� ����
	*/
    struct sMoonLightSlashData;
    typedef x2::intrusive_ptr<sMoonLightSlashData> sMoonRightSlashDataPtr;

	struct sMoonLightSlashData : x2::NonCopyable
	{
	private:
        unsigned                                        m_uRefCount;
		float	m_fChargingTime;		/// ���� �ð�
		float	m_fPowerRate;			/// ���� �ð��� ���� ������ ����
		bool	m_bSlotB;				/// A/B Slot Ȯ��
		bool*	m_pChargeKey;			/// ���� Ű Ȯ��

		int						m_iSlotID;			/// ������ ��ų������ �˾Ƴ��� ���� �뵵
		CX2EffectSet::Handle	m_hChargeEffect;	/// ���� �� �߻��ϴ� ����Ʈ
		CKTDGXMeshPlayer::CXMeshInstanceHandle	m_hNewAttackBox;		/// ���� Attack���� ����

		sMoonLightSlashData() : m_uRefCount(0), m_fChargingTime( 0.f ), m_fPowerRate( 0.f ),
			m_bSlotB( false ), m_pChargeKey( NULL ),
			m_iSlotID( 0 ), m_hChargeEffect( INVALID_EFFECTSET_HANDLE )
            , m_hNewAttackBox( INVALID_MESH_INSTANCE_HANDLE )
		{}

	public:

		static sMoonRightSlashDataPtr CreateMoonLightSlashData() { return sMoonRightSlashDataPtr( new sMoonLightSlashData ); }

		float	GetChargingTime() const { return m_fChargingTime; }
		void	SetChargingTime( float fChargingTime ) { m_fChargingTime = fChargingTime; }

		float	GetPowerRate() const { return m_fPowerRate; }
		void	SetPowerRate( float fPowerRate ) { m_fPowerRate = fPowerRate; }

		int GetSlotID() const { return m_iSlotID; }
		void SetSlotID( int iStateID ) { m_iSlotID = iStateID; }

		bool	GetSlotB() const { return m_bSlotB; }
		void	SetSlotB( bool bSlotB_ ) { m_bSlotB = bSlotB_; }

		bool	GetChargeKey() const { return (NULL != m_pChargeKey ? *m_pChargeKey : false); }
		void	SetChargeKey( bool* pChargeKey_ ) { m_pChargeKey = pChargeKey_; }

		CX2EffectSet::Handle GetHandleEffectSet() const { return m_hChargeEffect; }
		CX2EffectSet::Handle& GetHandleReferenceEffectSet() { return m_hChargeEffect; }
		void SetHandleEffectSet( CX2EffectSet::Handle hHandle_ ) { m_hChargeEffect = hHandle_; }

		CKTDGXMeshPlayer::CXMeshInstanceHandle GetHandleAttackMesh() const { return m_hNewAttackBox; }
		CKTDGXMeshPlayer::CXMeshInstanceHandle& GetHandleRefAttackMesh() { return m_hNewAttackBox; }
		void SetHandleAttackMesh( CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_ ) { m_hNewAttackBox = hHandle_; }

        void    AddRef()    {   ++m_uRefCount; }
        void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }

	};
	//#pragma endregion SMoonLightSlashData

	//#pragma region SLockOnSystem

    struct SLockOnSystem;
    typedef x2::intrusive_ptr<SLockOnSystem> sLockOnSystemPtr;
	struct SLockOnSystem : x2::NonCopyable
	{
	private:
        unsigned                                        m_uRefCount;
		bool		m_bLockOnUnit;		/// LockOn ���� ����
		float		m_fPvpRange;		/// ���� ����
		float		m_fPveRange;		/// ���� ����
		D3DXVECTOR3	m_vDestPos;			/// ��� ��ġ
		D3DXVECTOR3	m_vSafePos;			/// �ֱ� �̵��� ���� ������ ��ġ
		UidType		m_BeforeTargetUID;	/// ������ Ÿ�ٵ� ������ UID

		SLockOnSystem() : m_uRefCount(0), m_bLockOnUnit( false ), m_fPvpRange( 0.f ), m_fPveRange( 0.f )
        , m_BeforeTargetUID( -1 )
		{
			INIT_VECTOR3( m_vDestPos,	0.f, 0.f, 0.f );
			INIT_VECTOR3( m_vSafePos,	0.f, 0.f, 0.f );
		}

	public:

		static sLockOnSystemPtr CreateLockOnSystem() { return sLockOnSystemPtr( new SLockOnSystem ); }

		bool	CheckLockOnUnit() { return m_bLockOnUnit; }
		void	ResetLockOnUnit() { m_bLockOnUnit = false; }

		float	GetPvpRange() { return m_fPvpRange; }
		void	SetPvpRange( float fRange_ ) { m_fPvpRange = fRange_; }

		float	GetPveRange() { return m_fPveRange; }
		void	SetPveRange( float fRange_ ) { m_fPveRange = fRange_; }

		D3DXVECTOR3	GetDestPos() { return m_vDestPos; }

		D3DXVECTOR3 GetSafePos() { return m_vSafePos; }
		void		SetSafePos( D3DXVECTOR3 vPos_ ) { m_vSafePos = vPos_; }

		void ProcessLockOn( CX2GUAra* myUnit );

        void    AddRef()    {   ++m_uRefCount; }
        void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }
	};
	//#pragma endregion

	//#pragma endregion Enum, Struct, Class

	//#pragma region private
private:
	CX2GUAra( int iUnitIndex_, int iTeamNum_, IN lua_State* pScriptLuaState_, int iFrameBufferNum_, IN CX2Unit* pUnit_ );
	virtual ~CX2GUAra();

	virtual bool CanRidingState();

	//#pragma endregion function

	//#pragma region protected
protected:
	virtual void	Init( bool bUseTeamPos_, int iStartPosIndex_ );
	virtual void	ParseCommonRandomState();
	virtual void	InitState();

	void			InitStateID();
	void			InitStateCommon();
	void			InitStateByUnitClass();
	virtual void	InitComponent();
	void			InitPromotionEquipList();

	virtual void	InitAquiredActiveSkillState( const int iSkillID_ );
	virtual void	InitPassiveSkillState();
	virtual void	SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_,
		const int iSlotIndex_, const bool bSlotB_ );
	virtual void	SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ );
	void			SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet, ARA_STATE_ID eStateID );
	void			SetSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet_, UserUnitStateData& stateData_ );

	virtual void	CommonStateStartFuture();
	virtual void	CommonStateStart();

	virtual void	CommonFrameMoveFuture();
	virtual void	CommonFrameMove();

	virtual void	CommonStateEndFuture();
	virtual void	CommonStateEnd();

	virtual RENDER_HINT	CommonRender_Prepare();

	virtual bool	SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition
		= CX2SkillTree::ASUT_GROUND );
	virtual void	NoStateChangeActionFrameMove();
	virtual float	GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;

	void			LoadPromotionItemID( IN KLuaManager& luaManager_ );

	void			ShowActiveSkillCutInAndLightByBerserkMode( float fTimeToShow_, bool bOnlyLight_ = false );

	virtual CX2GageData*	CreateGageData();
	CX2AraGageData*			GetAraGageData() { return static_cast<CX2AraGageData*>( GetGageData() ); }
	const CX2AraGageData*	GetAraGageData() const { return static_cast<const CX2AraGageData*>( GetGageData() ); }

	void			StateChangeAirialDashIfPossible();

	__forceinline void	UpForcePower( int iUpCount_ );
	__forceinline bool	CanUseForcePower();
	__forceinline bool	IsEmptyForcePower();
	__forceinline bool	IsFullForcePower();

	bool	UpForcePowerAtThisAnimationTime( float fAnimationTimeNeededToChargeForcePower_, int iUpCount_ );
	bool	UseForcePowerAtThisAnimationTime( float fAnimationTimeNeededToUseForcePower_, int iUpCount_ );

	void	UseForcePower( int iCount_ );
	void	ConsumeForcePower( int iCount_ );

	void	SetForcePower( int iForcePower_ );
	void	SetMaxForcePower( int iMaxForcePower_ )  { GetAraGageData()->SetMaxForcePower( iMaxForcePower_ ); };

	UCHAR	GetBasicForcePower() const { return m_ucBasicForcePower; }
	void	SetBasicForcePower( UCHAR ucBasicForcePower_ ) { m_ucBasicForcePower = ucBasicForcePower_; }

	void	ResetForcePowerWhenRebirthInPVPGame( int iNumOfMyTeamKill_, int iNumOfEnmyTeamKill_ );

	inline void	ChangeAnimTail();

	bool	CanUseSpecialAttack( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = CX2SkillTree::ASUT_GROUND );
	void	StateChangeSpecialAttack( IN const CX2SkillTree::SKILL_ID eSkillId_ );

	virtual	bool	ShouldResetNotBusyTimer();

	bool GetAddItemAndRemoveItemToViewListBerserk( OUT CX2EqipPtr& pEquipPtrToRemove_, IN OUT CX2Item** ppItemToAddInViewList_,
		IN CX2Item* const pItemToRemoveInViewList_, OUT int& iIndexInViewListForRemove_ );
	void		AddBerserkEquipToViewList( CX2Item* const pItem );
	void		RemoveBerserkEquipFromViewList( CX2EqipPtr pEquipPtrToRemove, int index	);

	void		MoveAraSafePosition()
	{
		D3DXVECTOR3 vPos = m_sLockOnSystemPtr->GetSafePos();
		vPos.y += LINE_RADIUS * 1.5f;
		m_FrameDataFuture.syncData.position = vPos;

		StateChange( ASI_SA_ALH_SWALLOW_ASSAULT_ESCAPE );
	}

	virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

//#pragma endregion function

	//#pragma region public
public:
	static CX2GUAra*	CreateGUAra( int iUnitIndex_, int iTeamNum_, IN lua_State* pScriptLuaState_, int iFrameBufferNum_,
		IN CX2Unit* pUnit_ );

	static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );

	virtual void	ReInit( bool bTeamPos_ = false, int iStartPosIndex_ = -1 );
	virtual void	DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ );

	virtual void	Win();
	virtual void	Lose();
	virtual void	Draw();

	virtual void	InitEffect();
	virtual void	InitEnchantWeaponEffectUnitSpecific();

	void			InitializeAraMajorParticleArray();
	virtual void	CreateMajorParticleArray();
	static void		AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	CKTDGParticleSystem::CParticleEventSequence* SetAraMajorParticleByEnum( ARA_MAJOR_PARTICLE_INSTANCE_ID eVal_,
																			wstring wstrParticleName_, int iDrawCount_ = -1 );
	__forceinline CKTDGParticleSystem::CParticleEventSequenceHandle GetHandleAraMajorParticleByEnum( ARA_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const;
	__forceinline void SetHandleAraMajorParticleByEnum( ARA_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ );
	void			DeleteAraMajorParticle();

	void			InitializeAraMinorParticleArray();
	virtual void	CreateMinorParticleArray();
	static void		AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	CKTDGParticleSystem::CParticleEventSequence* SetAraMinorParticleByEnum( ARA_MINOR_PARTICLE_INSTANCE_ID eVal_,
		wstring wstrParticleName_, int iDrawCount_ = -1 );
	__forceinline CKTDGParticleSystem::CParticleEventSequenceHandle GetHandleAraMinorParticleByEnum( ARA_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const;
	__forceinline void SetHandleAraMinorParticleByEnum( ARA_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ );
	void			DeleteAraMinorParticle();
	virtual	void	DeleteMinorParticle();

	void			InitializeMajorMeshArray();
	void			DeleteMajorMesh();
	CKTDGXMeshPlayer::CXMeshInstanceHandle GetHandleAraMajorMeshByEnum( ARA_MAJOR_MESH_INSTANCE_ID eVal_ ) const;
	void SetHandleAraMajorMeshByEnum( ARA_MAJOR_MESH_INSTANCE_ID eVal_, CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_ );

	virtual bool	SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ );
	virtual void	COMMON_BUFF_FrameMove();

	virtual void	NotifyShowObjectChanged();
	virtual void	NotifyDGManagerChanged( CKTDGManager& refInManager_ );
	void			SetShowPromotionEquips( bool bShow_ );
	virtual void	SetShowViewList( bool bShow_ );
	void			SetShowOriginalViewList( bool bShow_ );
	void			ReplaceWeapon( CX2EqipPtr pWeaponPtr_ );
	virtual bool	UpdateEquipViewListInfo( CX2Item* pItemToAddInViewList, CX2Item* const pItemToRemoveInViewList );
	virtual void	UpdateEquipCollisionData();
	virtual void UpdateNotRenderingOtherEquipList();

	virtual bool	IsFullHyperMode() const;
	virtual void	SetFullHyperMode( const bool bBerserkMode_ );

	virtual	HRESULT	OnFrameMove( double fTime, float fElapsedTime );

	virtual void	ApplyHyperModeBuff();
	virtual void	HyperModeBuffEffectStart();
	virtual void	HyperModeBuffEffectEnd();

	virtual void	SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
	virtual void	UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
	virtual void	ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ );
	virtual void	ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ );

	virtual void	ChargeMpAndEtcInTrainingGame();

	__forceinline int	GetForcePower();

	virtual void	DoProcessWhenKillEnemy( const CX2GameUnit* pDefender_ );

	__forceinline int	GetMaxForcePower();

	float			GetBlockingRate();
	float			GetTurnigRate();

	virtual float GetReduceDebuffDurationRate(BUFF_TEMPLET_ID eBuffTempletID_);		/// �ƶ� 1�� ���� �Ҽ� - �нú� ��� Ȱ��ȭ : ����� ���� �ð� ����

	float			GetSkillCoolTimeValue( const CX2SkillTree::SKILL_ID eSkillID_, const float fCoolTimeLeft_ );	/// �޸� ������ ���� ��Ÿ�� ���氪 ���� �Լ�
	USHORT			GetActualFPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const USHORT usForceGainValue_ );	/// �޸� ������ ���� ��� �Һ� ���� �Լ�
	USHORT			GetActualFPGainCount( const CX2SkillTree::SKILL_ID eSkillID_, const USHORT usForceGainValue_ );	/// �޸� ������ ���� ��� ��ȯ�� ���� �Լ�

	virtual int GetComboZStateID() const { return ASI_COMBO_Z; }
	virtual int GetComboXStateID() const { return ASI_COMBO_X; }

	/// ��ź ������ ����Ʈ ���� �Լ�
	void			CreateEnergeBullet( IN const float vRotZOffSet_ = 0.f, IN const bool bFireRightHand_ = true );
	/// �ƶ� 2�� ���� ���� - ȸ������ ���� ó��
	bool			ProcessResurrection( IN const int iSkillLevel_ );
	/// �������� ����� �� �ִ� �������� �˻�
	bool			CanUseConversionHealth( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN const int iSkillLevel_ );
	/// AttackResult�� �߰� ���۽�ų �Լ� ����
	void			SetAttackResultDelegate();
	/// Ÿ�ݽ� �߰��Ǵ� ��·� ����
	void			UpNowForcePowerHitOthers( const CX2DamageManager::DamageData& damageData_ );
	/// Ÿ�ݵ� ����� ��ü ������ ����
	void			AddHitUnitInfoByDamageData( const CX2DamageManager::DamageData& damageData_ );
	/// ������ ó���� �߰��� ���� ��ġ �����ϴ� �Լ�( GetFinalDamage )
	virtual void	ModifyFinalDamageByGameUnitType( OUT CX2DamageManager::DAMAGE_TYPE& damageType_, OUT float& fAttackerPhysicDamage_,
													 OUT float& fAttackerMagicDamage_, OUT float& fForceDownValue_ );
	///  MP ������, ��Ÿ ����

	/// ��ų ���� ������Ʈ ���̵� �ش��ϴ� ������Ʈ ���̵� ��ȯ
	virtual void GetStateIDBySecretSkillTriggerStateID( IN OUT int& iStateID_ ) const;

	/// �ش� ��ų�� ������Ʈ�� ���� ��ų���� �˻�
	bool IsNoStageChangeSkill( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ )  const;

//#ifdef FIX_GAIN_FORCE_BUG
	virtual void		AttackResultByType( CX2DamageManager::DamageData &pDamageData );
//#endif // FIX_GAIN_FORCE_BUG

	virtual int GetHypetMpdeStateID();

	//#pragma endregion function

	//#pragma region protected

	// FP Cancel �� �������� ����, ���� boolean �� ���� �κп� Skill ID üũ�ϴ� �κ��� �߰���
	bool GetAvailableFPCancel ( CX2SkillTree::SKILL_ID m_eSkillID ) const
	{
		return ( true == m_bFPCancel && CX2SkillTree::SI_NONE != m_eSkillID && CX2SkillTree::SI_A_ALD_REFLECTION != m_eSkillID );
	}

protected:
	//#pragma region state set
	void DieFrontStartFuture();
	void DieFrontStart();
	void DieFrontFrameMove();
	void DieFrontEventProcess();

	void DieBackStartFuture();
	void DieBackStart();
	void DieBackFrameMove();
	void DieBackEventProcess();
	//#pragma endregion ASI_DIE_FRONT, ASI_DIE_BACK

	//#pragma region state Set
	void WaitStartFuture();
	void WaitStart();
	void WaitEventProcess();

	void HyperWaitStartFuture();
	void HyperWaitStart();
	void HyperWaitEventProcess();
	void HyperWaitStateEnd();
	void PlayAnimationHyperWaitStart( CKTDGXSkinAnim* pXSkinAnim_, const FrameData& refFrameData_ );

	void WalkEventProcess();

	void JumpReadyEventProcess();
	void JumpUpEventProcess();
	void JumpDownEventProcess();
	void JumpLandingStart();
	void JumpLandingEventProcess();
	//#pragma endregion ASI_WAIT, WALK, JUMP_READY, JUMP_UP, JUMP_DOWN, JUMP_LANDING

	//#pragma region state set
	void DashStart();
	void DashEventProcess();

	void DashEndStartFuture();
	void DashEndFrameMove();
	void DashEndEventProcess();

	void DashJumpStartFuture();
	void DashJumpStart();
	void DashJumpFrameMoveFuture();
	void DashJumpEventProcess();
	void DashJumpEndFuture();

	void DashJumpLandingEventProcess();

	void DashJumpPowerLandingStartFuture();
	void DashJumpPowerLandingStart();
	void DashJumpPowerLandingEventProcess();
	//#pragma endregion ASI_DASH, DASH_END, DASH_JUMP, DASH_JUMP_LANDING, POWER_LANDING

	//#pragma region state set
	void DamageSmallEventProcess();

	void DamageBigStart();
	void DamageBigEventProcess();

	void DamageDownFrontFrameMove();
	void DamageDownFrontEventProcess();

	void DamageDownBackFrameMove();
	void DamageDownBackEventProcess();

	void DamageAirDownLandingEventProcess();
	//#pragma endregion DAMAGE

	//#pragma region state set
	void HyperModeFrameMove();
	void RidingHyperModeFrameMove();
	void CommonHyperModeFrameMove( float fTime1_, float fTime2_, float fTime3_, bool bSound_ = false );

	void StandUpRollingEventProcess();
	void StandUpRollingEnd();

	void StandUpAttackEventProcess();
	void StandUpAttackEnd();
	//#pragma endregion ASI_HYPER_MODE, ASI_STANDUP_ROLLING, ASI_STANDUP_ATTACK

	//#pragma region state set
	void ComboZEventProcess();
	void ComboZZEventProcess();
	void ComboZZZFrameMove();
	void ComboZZZEventProcess();
	void ComboZZZZEventProcess();
	void ComboZZZXStartEventProcess();
	void ComboZZZXChargeStateStart();
	void ComboZZZXChargeFrameMove();
	void ComboZZZXChargeEventProcess();
	void ComboZZZXChargeStateEnd();
	void ComboZZZXAttackFrameMove();
	void ComboZZZXAttackEventProcess();
	void ComboZZZXAttackStateEnd();
	void ComboZZXEventProcess();
	//#pragma endregion COMBO_Z

	//#pragma region state set
	void ComboXEventProcess();
	void ComboXXEventProcess();
	void ComboXXXStateStart();
	void ComboXXXEventProcess();
	void ComboXXXloopStateStart();
	void ComboXXXloopFrameMove();
	void ComboXXXloopEventProcess();
	void ComboXXXloopXStateStart();
	void ComboXXXloopXFrameMove();
	void ComboXXXloopXEventProcess();
	void ComboXXXloopXStateEnd();
	void ComboXXdownXFrameMove();
	void ComboXXdownXEventProcess();
	void ComboXXdownXXEventProcess();
	//#pragma endregion COMBO_X

	//#pragma region state set
	void ComboDashZFrameMove();
	void ComboDashZEventProcess();
	void ComboDashZpushX_ChargeStateStart();
	void ComboDashZpushX_ChargeFrameMove();
	void ComboDashZpushX_ChargeEventProcess();
	void ComboDashZpushX_ChargeStateEnd();
	void ComboDashZpushX_AttackFrameMove();
	void ComboDashZpushX_AttackEventProcess();
	void ComboDashZpushX_AttackStateEnd();
	void ComboDashXEventProcess();
	//#pragma endregion DASH_COMBO

	//#pragma region state set
	void JumpAttackZFrameMoveFuture();
	void JumpAttackZEventProcess();
	void JumpAttackXFrameMoveFuture();
	void JumpAttackXFrameMove();
	void JumpAttackXEventProcess();
	void DashJumpComboZEventProcess();
	void DashJumpComboZZEventProcess();
	void DashJumpComboZZZFrameMove();
	void DashJumpComboZZZEventProcess();
	void DashJumpComboXEventProcess();
	//#pragma endregion JUMP_ATTACK

	//#pragma region state set
	void AirialDashStartFuture();
	void AirialDashStart();
	void AirialDashFrameMoveFuture();
	void AirialDashFrameMove();
	void AirialDashEventProcess();
	void AirialDashEndFuture();
	void DoubleJumpStartFuture();
	void DoubleJumpFrontEventProcess();
	void DoubleJumpUpEventProcess();
	void DoubleJumpBackEventProcess();
	//#pragma endregion special command

	//#pragma region state skill set

	void ASI_SA_ARA_WIND_WEDGE_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_WIND_WEDGE_START_Init();
	void ASI_SA_ARA_WIND_WEDGE_START_FrameMove();
	void ASI_SA_ARA_WIND_WEDGE_START_EventProcess();

	void ASI_SA_ARA_WIND_WEDGE_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_WIND_WEDGE_END_Init();
	void ASI_SA_ARA_WIND_WEDGE_END_FrameMove();
	void ASI_SA_ARA_WIND_WEDGE_END_EventProcess();

	void ASI_SA_ARA_SHADOW_RISING_JUMP_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_SHADOW_RISING_JUMP_Init();
	void ASI_SA_ARA_SHADOW_RISING_JUMP_FrameMove();
	void ASI_SA_ARA_SHADOW_RISING_JUMP_EventProcess();

	void ASI_SA_ARA_SHADOW_RISING_LANDING_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_SHADOW_RISING_LANDING_Init();
	void ASI_SA_ARA_SHADOW_RISING_LANDING_FrameMove();
	void ASI_SA_ARA_SHADOW_RISING_LANDING_EventProcess();

	void ASI_SA_ARA_SHADOW_RISING_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_SHADOW_RISING_END_Init();
	void ASI_SA_ARA_SHADOW_RISING_END_FrameMove();
	void ASI_SA_ARA_SHADOW_RISING_END_EventProcess();

	void ASI_SA_ARA_PULLING_PIERCE_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_PULLING_PIERCE_START_Init();
	void ASI_SA_ARA_PULLING_PIERCE_START_FrameMove();
	void ASI_SA_ARA_PULLING_PIERCE_START_EventProcess();

	void ASI_SA_ARA_PULLING_PIERCE_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ARA_PULLING_PIERCE_END_Init();
	void ASI_SA_ARA_PULLING_PIERCE_END_FrameMove();
	void ASI_SA_ARA_PULLING_PIERCE_END_EventProcess();

	void ASI_SA_ARA_TEMPEST_DANCE_START_Init();
	void ASI_SA_ARA_TEMPEST_DANCE_START_FrameMove();
	void ASI_SA_ARA_TEMPEST_DANCE_START_EventProcess();
	void ASI_SA_ARA_TEMPEST_DANCE_END_Init();
	void ASI_SA_ARA_TEMPEST_DANCE_END_FrameMove();
	void ASI_SA_ARA_TEMPEST_DANCE_END_EventProcess();

	void ASI_SA_ARA_SHADOW_KNOT_START_Init();
	void ASI_SA_ARA_SHADOW_KNOT_START_StateStart();
	void ASI_SA_ARA_SHADOW_KNOT_START_FrameMove();
	void ASI_SA_ARA_SHADOW_KNOT_START_EventProcess();
	void ASI_SA_ARA_SHADOW_KNOT_WAIT_Init();
	void ASI_SA_ARA_SHADOW_KNOT_WAIT_FrameMove();
	void ASI_SA_ARA_SHADOW_KNOT_WAIT_EventProcess();
	void ASI_SA_ARA_SHADOW_KNOT_WAIT_StateEnd();
	void ASI_SA_ARA_SHADOW_KNOT_CATCH_Init();
	void ASI_SA_ARA_SHADOW_KNOT_CATCH_EventProcess();
	void ASI_SA_ARA_SHADOW_KNOT_END_Init();
	void ASI_SA_ARA_SHADOW_KNOT_END_FrameMove();
	void ASI_SA_ARA_SHADOW_KNOT_END_EventProcess();

	void ASI_SA_ARA_FALLING_DRAGON_Init();
	void ASI_SA_ARA_FALLING_DRAGON_FrameMove();
	void ASI_SA_ARA_FALLING_DRAGON_EventProcess();

	void ASI_A_ARA_TIGER_CLAW_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ARA_TIGER_CLAW_START_Init();
	void ASI_A_ARA_TIGER_CLAW_START_StateStart();
	void ASI_A_ARA_TIGER_CLAW_START_FrameMove();
	void ASI_A_ARA_TIGER_CLAW_START_EventProcess();
	void ASI_A_ARA_TIGER_CLAW_START_StateEnd();

	void ASI_A_ARA_TIGER_CLAW_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ARA_TIGER_CLAW_END_Init();
	void ASI_A_ARA_TIGER_CLAW_END_FrameMove();
	void ASI_A_ARA_TIGER_CLAW_END_EventProcess();

	void ASI_A_ARA_FORCE_SPEAR_Init();
	void ASI_A_ARA_FORCE_SPEAR_FrameMove();
	void ASI_A_ARA_FORCE_SPEAR_EventProcess();

	void ASI_A_ARA_GAIN_FORCE_Init();
	void ASI_A_ARA_GAIN_FORCE_FrameMove();
	void ASI_A_ARA_GAIN_FORCE_EventProcess();

	//#pragma region Little Hsien
	void ASI_SA_ALH_MOONLIGHT_SLASH_READY_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALH_MOONLIGHT_SLASH_READY_Init();
	void ASI_SA_ALH_MOONLIGHT_SLASH_READY_StateStart();
	void ASI_SA_ALH_MOONLIGHT_SLASH_READY_StateStartFuture();
	void ASI_SA_ALH_MOONLIGHT_SLASH_READY_FrameMove();
	void ASI_SA_ALH_MOONLIGHT_SLASH_READY_EventProcess();

	void ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_Init();
	void ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_FrameMove();
	void ASI_SA_ALH_MOONLIGHT_SLASH_CHARGE_EventProcess();

	void ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_Init();
	void ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateStart();
	void ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_FrameMove();
	void ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_StateEnd();
	void ASI_SA_ALH_MOONLIGHT_SLASH_CRESCENT_ATTACK_EventProcess();

	void ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_Init();
	void ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateStart();
	void ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_FrameMove();
	void ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_StateEnd();
	void ASI_SA_ALH_MOONLIGHT_SLASH_FULLMOON_ATTACK_EventProcess();

	void ASI_SA_ALH_SWALLOW_ASSAULT_Ready_Init();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateStart();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Ready_FrameMove();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Ready_StateEnd();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Ready_EventProcess();

	void ASI_SA_ALH_SWALLOW_ASSAULT_Attack_Init();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateStart();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Attack_FrameMove();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Attack_EventProcess();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Attack_StateEnd();

	void ASI_SA_ALH_SWALLOW_ASSAULT_Finish_Init();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Finish_StateStart();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Finish_FrameMove();
	void ASI_SA_ALH_SWALLOW_ASSAULT_Finish_EventProcess();

	void ASI_SA_ALH_SWALLOW_ASSAULT_Escape_EventProcess();

	void ASI_A_ALH_ROCK_CRASH_FALLING_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ALH_ROCK_CRASH_START_Init();
	void ASI_A_ALH_ROCK_CRASH_START_StateStart();
	void ASI_A_ALH_ROCK_CRASH_START_FrameMove();
	void ASI_A_ALH_ROCK_CRASH_START_EventProcess();
	void ASI_A_ALH_ROCK_CRASH_START_StateEnd();

	void ASI_A_ALH_ROCK_CRASH_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ALH_ROCK_CRASH_END_Init();
	void ASI_A_ALH_ROCK_CRASH_END_FrameMove();
	void ASI_A_ALH_ROCK_CRASH_END_EventProcess();
	//#pragma endregion 1-1

	//#pragma region SAKRA_DEVANAM
	void ASI_A_ASD_LOW_BRANDISH_SLIDING_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ASD_LOW_BRANDISH_SLIDING_Init();
	void ASI_A_ASD_LOW_BRANDISH_SLIDING_StateStart();
	void ASI_A_ASD_LOW_BRANDISH_SLIDING_FrameMove();
	void ASI_A_ASD_LOW_BRANDISH_SLIDING_EventProcess();
	void ASI_A_ASD_LOW_BRANDISH_SLIDING_StateEnd();

	void ASI_A_ASD_LOW_BRANDISH_KICK_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ASD_LOW_BRANDISH_KICK_Init();
	void ASI_A_ASD_LOW_BRANDISH_KICK_FrameMove();
	void ASI_A_ASD_LOW_BRANDISH_KICK_EventProcess();

	void ASI_P_ASD_QUICKEN_GUARD_StateStart();
	void ASI_P_ASD_QUICKEN_GUARD_EventProcess();

	void ASI_SA_ASD_VOID_BREAK_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ASD_VOID_BREAK_Init();
	void ASI_SA_ASD_VOID_BREAK_FrameMove();
	void ASI_SA_ASD_VOID_BREAK_EventProcess();
	//#pragma endregion 1-2

	//#pragma region ��ų ����
	void ASI_A_ARA_Quick_Piercing_EventProcess();	// õ��
	void ASI_A_ARA_Quick_Piercing_FrameMove();

	void ASI_A_ARA_Shoulder_Blow_Init();			// ö���
	void ASI_A_ALH_Shoulder_Blow_FrameMove();
	void ASI_A_ALH_Shoulder_Blow_EventProcess();

	void ASI_A_ASD_Falling_Blossom_Start_Init();			// ��ȭ
	void ASI_A_ASD_Falling_Blossom_Start_StateStart();
	void ASI_A_ASD_Falling_Blossom_Start_EventProcess();

	void ASI_A_ASD_Falling_Blossom_Finish_Init();			// ��ȭ
	void ASI_A_ASD_Falling_Blossom_Finish_EventProcess();

	void ASI_A_ARA_Quick_Stab_Init();				// �ż�
	void ASI_A_ARA_Quick_Stab_FrameMove();
	void ASI_A_ARA_Quick_Stab_EventProcess();
	//#pragma endregion �ż�, ö���, ����, õ��, ��������, ��ȣ�� ��, ȣ�Ű���

	//#pragma endregion passive, active, special active skill

	void ASI_COMBO_ALH_ZZXX_EventProcess();					//COMBO_ZZXX
	void ASI_COMBO_ALH_ZZXX_LANDING_EventProcess();			//COMBO_ZZXX
	void ASI_DASH_COMBO_ALH_ZZ_EventProcess();				//DASH_COMBO_ZZ
	void ASI_JUMP_ATTACK_ALH_ZZ_EventProcess();				//JUMP_ATTACK_ZZ

	void ASI_DASH_JUMP_COMBO_ASD_ZX_EventProcess();			//DASH_JUMP_COMBO_ZX
	void ASI_DOUBLE_JUMP_FRONT_COMBO_ASD_Z_BACK_EventProcess();//DOUBLE_JUMP_FRONT_COMBO_Z_BACK

	void ASI_A_ALH_BREAKING_Init();							//��ȭ���� ���
	void ASI_A_ALH_BREAKING_FrameMove();
	void ASI_A_ALH_BREAKING_EventProcess();
	void ASI_A_ALH_BREAKING_ATTACK_Init();					//��ȭ���� ����
	void ASI_A_ALH_BREAKING_ATTACK_FrameMove();
	void ASI_A_ALH_BREAKING_ATTACK_EventProcess();
	void ASI_A_ALH_BREAKING_ATTACK_EndFuture();

	void ASI_SA_ALH_DOUBLE_COLLISION_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALH_DOUBLE_COLLISION_START_Init();
	void ASI_SA_ALH_DOUBLE_COLLISION_START_Start();
	void ASI_SA_ALH_DOUBLE_COLLISION_START_FrameMove();
	void ASI_SA_ALH_DOUBLE_COLLISION_START_EventProcess();

	void ASI_SA_ALH_DOUBLE_COLLISION_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALH_DOUBLE_COLLISION_END_FrameMove();
	void ASI_SA_ALH_DOUBLE_COLLISION_END_EventProcess();

	void ASI_SA_ASD_TURBULENT_WAVE_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );			// ��ȣ 4�� - �뵵
	void ASI_SA_ASD_TURBULENT_WAVE_START_Init();			// ��ȣ 4�� - �뵵
	void ASI_SA_ASD_TURBULENT_WAVE_START_Start();
	void ASI_SA_ASD_TURBULENT_WAVE_START_FrameMove();
	void ASI_SA_ASD_TURBULENT_WAVE_START_End();
	void ASI_SA_ASD_TURBULENT_WAVE_START_EventProcess();

	void ASI_SA_ASD_TURBULENT_WAVE_LOOP_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ASD_TURBULENT_WAVE_LOOP_FrameMove();
	void ASI_SA_ASD_TURBULENT_WAVE_LOOP_EventProcess();
	void ASI_SA_ASD_TURBULENT_WAVE_LOOP_End();

	void ASI_SA_ASD_TURBULENT_WAVE_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ASD_TURBULENT_WAVE_END_Init();
	void ASI_SA_ASD_TURBULENT_WAVE_END_FrameMove();
	void ASI_SA_ASD_TURBULENT_WAVE_END_EventProcess();

	void ASI_SA_ASD_SUPPRESSION_Init();						// ����
	void ASI_SA_ASD_SUPPRESSION_StateStart();
	void ASI_SA_ASD_SUPPRESSION_FrameMove();
	void ASI_SA_ASD_SUPPRESSION_EventProcess();

	void ASI_SA_ALH_POWER_OF_WHITER_TIGER_Init();			/// ��ȣ��
	void ASI_SA_ALH_POWER_OF_WHITER_TIGER_FrameMove();
	void ASI_SA_ALH_POWER_OF_WHITER_TIGER_EventProcess();
	void ASI_SA_ALH_POWER_OF_WHITER_TIGER_End();
	void ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING_FrameMove();
	void ASI_SA_ALH_POWER_OF_WHITER_TIGER_LANDING_EventProcess();

	void ASI_SA_ALH_PEERLESSNESS_SLASH_START_Init();			/// ������
	void ASI_SA_ALH_PEERLESSNESS_SLASH_START_Start();
	void ASI_SA_ALH_PEERLESSNESS_SLASH_START_FrameMove();
	void ASI_SA_ALH_PEERLESSNESS_SLASH_START_EventProcess();
	void ASI_SA_ALH_PEERLESSNESS_SLASH_START_End();
	void ASI_SA_ALH_PEERLESSNESS_SLASH_END_FrameMove();
	void ASI_SA_ALH_PEERLESSNESS_SLASH_END_EventProcess();

	void ASI_SA_ALH_DESCENT_DRAGON_CreateState( UserUnitStateData& stateData_, const int iStateId_ );			/// ��� �������� - ����
	void ASI_SA_ALH_DESCENT_DRAGON_Init();			/// ��� �������� - ����
	void ASI_SA_ALH_DESCENT_DRAGON_FrameMove();
	void ASI_SA_ALH_DESCENT_DRAGON_EventProcess();

	void SetInvisibility(bool bVal_);


	//#pragma region 2-1, 2-2 Combo State
	/// DASH_JUMP_COMBO_ZX
	void ASI_DASH_JUMP_COMBO_ALD_ZX_FrameMove();
	void ASI_DASH_JUMP_COMBO_ALD_ZX_EventProcess();

	/// DASH_JUMP_COMBO_ALD_ZXX
	void ASI_DASH_JUMP_COMBO_ALD_ZXX_FrameMove();
	void ASI_DASH_JUMP_COMBO_ALD_ZXX_EventProcess();

	/// COMBO_ZZXX
	void ASI_COMBO_ALD_ZZXX_FrameMove();
	void ASI_COMBO_ALD_ZZXX_EventProcess();

	/// COMBO_ZZZFrontZ
	void ASI_COMBO_ALD_ZZZFrontZ_FrameMove();
	void ASI_COMBO_ALD_ZZZFrontZ_EventProcess();

	/// DASH_JUMP_COMBO_XZ
	void ASI_DASH_JUMP_COMBO_AYR_XZ_StateStartFuture();
	void ASI_DASH_JUMP_COMBO_AYR_XZ_FrameMove();
	void ASI_DASH_JUMP_COMBO_AYR_XZ_EventProcess();

	/// DASH_COMBO_ZZX
	void ASI_DASH_COMBO_AYR_ZZ_FrameMove();
	void ASI_DASH_COMBO_AYR_ZZ_EventProcess();
	//#pragma endregion 2-1, 2-2 Ŀ�ǵ� ��ų

	//#pragma region 2-1, 2-2 Active
	/// ���� 3�� - �������
	void ASI_A_ALD_WOLF_CLAW_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ALD_WOLF_CLAW_Init();
	void ASI_A_ALD_WOLF_CLAW_StateStart();
	void ASI_A_ALD_WOLF_CLAW_EventProcess();
	void ASI_A_ALD_WOLF_CLAW_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ALD_WOLF_CLAW_FINISH_Init();
	void ASI_A_ALD_WOLF_CLAW_FINISH_EventProcess();

	/// ������
	void ASI_A_ALD_CONVERSION_HEALTH_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ALD_CONVERSION_HEALTH_Init();
	void ASI_A_ALD_CONVERSION_HEALTH_FrameMove();
	void ASI_A_ALD_CONVERSION_HEALTH_EventProcess();

	/// �̱��
	void ASI_A_ALD_CONVERSION_MANA_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_ALD_CONVERSION_MANA_Init();
	void ASI_A_ALD_CONVERSION_MANA_FrameMove();
	void ASI_A_ALD_CONVERSION_MANA_EventProcess();

	/// ���
	void ASI_A_AYR_HIGH_SPEED_READY_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_AYR_HIGH_SPEED_READY_EventProcess();
	void ASI_A_AYR_HIGH_SPEED_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_AYR_HIGH_SPEED_Init();
	void ASI_A_AYR_HIGH_SPEED_StateStartFuture();
	void ASI_A_AYR_HIGH_SPEED_EventProcess();
	void ASI_A_AYR_HIGH_SPEED_StateEndFuture();
	//#pragma endregion 2-1, 2-2 ��Ƽ�� ��ų

	//#pragma region 2-1, 2-2 Special Active
	/// �����
	void ASI_SA_ALD_ENERGY_WAVE_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_ENERGY_WAVE_Init();
	void ASI_SA_ALD_ENERGY_WAVE_EventProcess();

	/// ��ȯ����
	void ASI_SA_ALD_HELL_OF_TORNADO_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_HELL_OF_TORNADO_Init();
	void ASI_SA_ALD_HELL_OF_TORNADO_StateStart();
	void ASI_SA_ALD_HELL_OF_TORNADO_EventProcess();
	void ASI_SA_ALD_HELL_OF_TORNADO_FINISH_CreateState(  UserUnitStateData& stateData_, const int iStateId_  );
	void ASI_SA_ALD_HELL_OF_TORNADO_FINISH_Init();
	void ASI_SA_ALD_HELL_OF_TORNADO_FINISH_EventProcess();

	/// ���� 4�� - �����̻�
	void ASI_SA_ALD_WOLF_SPEAR_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_WOLF_SPEAR_Init();
	void ASI_SA_ALD_WOLF_SPEAR_StateStart();
	void ASI_SA_ALD_WOLF_SPEAR_EventProcess();
	void ASI_SA_ALD_WOLF_SPEAR_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_WOLF_SPEAR_FINISH_Init();
	void ASI_SA_ALD_WOLF_SPEAR_FINISH_EventProcess();

	/// ���� 1�� - ��ȥ
	void ASI_SA_ALD_FINGER_BULLET_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_FINGER_BULLET_Init();
	void ASI_SA_ALD_FINGER_BULLET_StateStart();
	void ASI_SA_ALD_FINGER_BULLET_EventProcess();
	void ASI_SA_ALD_FINGER_BULLET_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_FINGER_BULLET_FINISH_Init();
	void ASI_SA_ALD_FINGER_BULLET_FINISH_FrameMove();
	void ASI_SA_ALD_FINGER_BULLET_FINISH_EventProcess();

	/// ���� 2�� - ��ȯ��
	void ASI_SA_ALD_ENERGY_BEAD_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_ENERGY_BEAD_Init();
	void ASI_SA_ALD_ENERGY_BEAD_StateStart();
	void ASI_SA_ALD_ENERGY_BEAD_EventProcess();
	void ASI_SA_ALD_ENERGY_BEAD_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_ENERGY_BEAD_FINISH_Init();
	void ASI_SA_ALD_ENERGY_BEAD_FINISH_EventProcess();

	/// ���� 3�� - â�차��
	void ASI_SA_ALD_PRISON_SPEAR_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_PRISON_SPEAR_Init();
	void ASI_SA_ALD_PRISON_SPEAR_StateStart();
	void ASI_SA_ALD_PRISON_SPEAR_EventProcess();
	void ASI_SA_ALD_PRISON_SPEAR_FINISH_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_PRISON_SPEAR_FINISH_Init();
	void ASI_SA_ALD_PRISON_SPEAR_FINISH_EventProcess();

	/// ���� 4�� - Żȥ
	void ASI_A_AYR_STEAL_SOUL_CreateState( OUT UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_AYR_STEAL_SOUL_Init();
	void ASI_A_AYR_STEAL_SOUL_StateStart();
	void ASI_A_AYR_STEAL_SOUL_EventProcess();
	void ASI_A_AYR_STEAL_SOUL_FINISH_CreateState( OUT UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_A_AYR_STEAL_SOUL_FINISH_Init();
	void ASI_A_AYR_STEAL_SOUL_FINISH_FrameMove();
	void ASI_A_AYR_STEAL_SOUL_FINISH_EventProcess();

	/// ��������
	void ASI_SA_AYR_ENERGY_VOID_START_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_AYR_ENERGY_VOID_START_Init();
	void ASI_SA_AYR_ENERGY_VOID_START_StateStartFuture();
	void ASI_SA_AYR_ENERGY_VOID_START_StateStart();
	void ASI_SA_AYR_ENERGY_VOID_START_EventProcess();

	void ASI_SA_AYR_ENERGY_VOID_LOOP_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_AYR_ENERGY_VOID_LOOP_Init();
	void ASI_SA_AYR_ENERGY_VOID_LOOP_StateStart();
	void ASI_SA_AYR_ENERGY_VOID_LOOP_EventProcess();

	void ASI_SA_AYR_ENERGY_VOID_END_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_AYR_ENERGY_VOID_END_Init();
	void ASI_SA_AYR_ENERGY_VOID_END_StateStart();
	void ASI_SA_AYR_ENERGY_VOID_END_FrameMove();
	void ASI_SA_AYR_ENERGY_VOID_END_EventProcess();

	/// ���� - ��
	void ASI_SA_AYR_SUPPRESSION_ENERGY_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_AYR_SUPPRESSION_ENERGY_Init();
	void ASI_SA_AYR_SUPPRESSION_ENERGY_EventProcess();

	/// ���� �������� - �ؼ�
	void ASI_SA_ALD_WOLF_BREAK_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_ALD_WOLF_BREAK_Init();
	void ASI_SA_ALD_WOLF_BREAK_StateStart();
	void ASI_SA_ALD_WOLF_BREAK_FrameMove();
	void ASI_SA_ALD_WOLF_BREAK_EventProcess();

	/// ���� �������� - ����
	void ASI_SA_AYR_OPEN_THE_HELL_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_SA_AYR_OPEN_THE_HELL_Init();
	void ASI_SA_AYR_OPEN_THE_HELL_FrameMove();
	void ASI_SA_AYR_OPEN_THE_HELL_EventProcess();

	/// ���ȯ����
	void ASI_HA_AYR_ENERGY_CANNON_CreateState( UserUnitStateData& stateData_, const int iStateId_ );
	void ASI_HA_AYR_ENERGY_CANNON_Init();
	void ASI_HA_AYR_ENERGY_CANNON_EventProcess();
	//#pragma endregion 2-1, 2-2 ����� ��Ƽ�� ��ų


	//�ƶ� ��õ �ñر� - �б� ��õ
	void ASI_HA_ASD_THOUSANDS_BLOSSOMS_Init();
	void ASI_HA_ASD_THOUSANDS_BLOSSOMS_EventProcess();

	//#pragma region protected
protected:
	CKTDGParticleSystem::CParticleEventSequenceHandle	m_ahAraMajorParticleInstance[ARA_MAJOR_PII_END];	/// ������ ��ƼŬ �ڵ�
	CKTDGParticleSystem::CParticleEventSequenceHandle	m_ahAraMinorParticleInstance[ARA_MINOR_PII_END];	/// ���̳� ��ƼŬ �ڵ�
	CKTDGXMeshPlayer::CXMeshInstanceHandle				m_ahAraMajorMeshInstance[ARA_MAJOR_MII_END];		/// ������ �޽� �ڵ�

	vector<CX2EqipPtr>	m_vecPromotionEquipList;		/// ���� ��� ����Ʈ�� ���� ���� 3���� �Ҹ��� ������ ������ ��������.
	CX2EqipPtr			m_ptrPromotionEquipTail;		/// �ƶ� ������ ������ ����ȣ ����

	vector<int>			m_vecPromotionItemIDList;		/// ���θ�� ����� ID ����Ʈ
	vector<int>			m_vecPromotionItemIDList2;		///
	bool				m_bBerserkMode;					/// ����ȭ ����

	const int	MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE;	/// ���� ���¿��� �ִ�� Loop �� ���ִ� �ִϸ��̼� Ƚ��

	float		m_fDamageDataChangeTime;	/// DamageDate ���� �ð� 1
	float		m_fDamageDataChangeTime2;	/// DamageData ���� �ð� 2

	USHORT		m_uiComboXXXLoopCount;		/// XXXLoop �޺� ī��Ʈ ( �ִ� 3ȸ )
	float		m_fZpushXChargeTime;		/// Dash_Combo_ZpushX_Charge �ð�( �ִ� 2.0�� )
	float		m_fAirialDashTime;			/// �ɰ��㵵 �ð�( ���� �нú�� ������ �� ���� )
	USHORT		m_usArialCount;				/// �ɰ��㵵 Ƚ��
	USHORT		m_usDoubleJumpCount;		/// �̴����� Ƚ��

	UCHAR		m_ucBasicForcePower;		/// �⺻ ���ġ
	CForcePowerSystem	m_ForcePowerSystem;	/// ��� �ý���
	D3DXVECTOR2	m_vFPCancel;				/// ��� ��� �ð�
	bool		m_bFPCancel;				/// ������� ���
	USHORT		m_usForceGainCount;			/// ��ų ĵ���� ���� ��� ȹ�淮
	USHORT		m_usBeforeForceGainCount;	/// m_usForceGainCount�� ĵ���� ��ų�� ���ŵǴ� ������ ���� �� ����

	const CX2SkillTree::SkillTemplet*	m_pSkillTemplet;	/// ��ų ���ø� ������
	int		m_iPressedSkillSlotIndex;
	float	m_fMPConsume;

	float	m_fShadowKnotTime;								/// �׸��� �ŵ� Wait Ÿ��
	CX2DamageEffect::CEffectHandle	    m_hShadowKnotWeaponMesh;	/// �׸��� �ŵ� ����


	float		m_fZZZXChargeTime;				/// ZZZXChargeTime �ð�( �ִ� 2.0�� )

	CX2EffectSet::Handle	m_hPullinPierce;	/// ���� �α� �ڵ� ������

	float		m_fPullingDragonPowerRate;		/// ������ ���ݷ�

	bool		m_bAbleQuickStand;				/// ����� ��� ���� ����
	int			m_iGainMpByFpConversion;		/// ��� ��� ���� �� �ִ� MP��(�нú� : �游�� ��)
	float		m_fPreRenderParamColorA;		/// ���������� ���� ���� �� �ӽ� �����ϴ� ���� ���İ�
	float		m_fRenderParamColorA;			/// ���������� ���� �� �� ���̵� �ƿ� ��Ű�� ���� ���İ�

	float	m_fBlockingRate;					/// 1-2�� �����̴� ���� ���� ����
	float	m_fTurningRate;						/// 1-2�� �����̴� ���� �� ���Ƹ±� ����

	bool				m_bBrutalPiercing;				/// â���� ���� ��ų ����
	CX2BuffFactorPtr	m_pBrutalPiercingBuffFactor;	/// â���� ���� ����� Buff Factor

	float	m_fReduceEDTDurationRate;			/// �ƶ� 1�� ���� �Ҽ� - �нú� ��� Ȱ��ȭ : ����� �ð� ������

	sMoonRightSlashDataPtr	m_sMoonRightSlashDataPtr;		/// �޺����� ������

	CX2EffectSet::Handle	m_hTempEffectSet;	/// ����Ʈ�� ���� ������ ��Ʈ���ϱ� ���� �ӽ� �ڵ�

    sLockOnSystemPtr                m_sLockOnSystemPtr;		/// ���� �ý��� Smart Ptr

	PROTECT_VECTOR3				m_vPreTransScale;	/// ���������� ���� ������ ���� ��ų ��, ������ ���� ������ ���� �� ����


	CX2SkillTree::SKILL_ID	m_eSkillID;		/// ��ų ���̵�

	float	m_fCurrentSpeed;	/// ������ ���� ���� �ð� ����ġ
	bool	m_bMoonSlash;		/// ���� ���� �нú�
	float	m_fSuppresionPowerRate;		/// ���� ���ݷ�

	float		m_fUseForcePowerTime;						/// ��� �Ҹ� �Ǵ� �ð�
	int			m_iUseForcePowerCount;						/// ��� �Ҹ�

	float		m_fAddForcePowerTime;						/// ��� ���� �Ǵ� �ð�
	int			m_iAddForcePowerCount;						/// ��� ������

	float		m_fIncreaseEnergeBulletSize;				/// �ƶ� 1�� ���� �Ҹ� - ��ź ��ȭ�� ���� �����Ǵ� ��ź ũ��
	float		m_fIncreaseEnergeBulletDamage;				/// �ƶ� 1�� ���� �Ҹ� - ��ź ��ȭ�� ���� �����Ǵ� ��ź ���ݷ�

	float		m_fIncreaseEnergeBulletRange;				/// �ƶ� 1�� ���� �Ҹ� - ��� ��ȭ�� ���� �����Ǵ� ��ź ��Ÿ�
	float		m_fIncreaseEnergeBulletForceDownValue;		/// �ƶ� 1�� ���� �Ҹ� - ��� ��ȭ�� ���� �����Ǵ� �ٿ� ��ġ ���ҷ�

	std::vector<CX2GameUnitoPtr> m_vecHitUnitPointer;		/// �ƶ� 1�� ���� �Ҹ� - ������ �ǰݵ� ���� ������ ���� ��ü

	float		m_fResurrectionLimitHPValue;				/// �ƶ� 2�� ���� ���� - ȸ������ ���� ü�� ��ġ
	float		m_fResurrectionCoolTime;					/// �ƶ� 2�� ���� ���� - ȸ������ ��ߵ� �ð�

	float		m_fHighSpeedValue;							/// �ƶ� 2�� ���� ���� - ��� ���� �ð�

	int			m_iEnergeVoidChargeValue;					/// �ƶ� 2�� ���� ���� - �������� ���� �ܰ�
    CX2DamageEffect::CEffectHandle   m_hEnergeVoidDamageEffect;	/// �ƶ� 2�� ���� ���� - �������� ���� ����Ʈ
	bool		m_bEnergeVoidChargeEnd;						/// �ƶ� 2�� ���� ���� - �������� ���� ���� ����

	float		m_fHowlingDamageRel;						/// ������ ���¢�� ������ ����
	float		m_fHowlingActiveRate;						/// ������ ���¢�� �ߵ� Ȯ��

	float		m_fOpenTheHellIncreaseDamage;				/// ���� �ʷ��� ���� ���� ������ ������

	int		m_iWolfBreakeCount;								/// ���� �������� - �ؼ� �߻� ī��Ʈ

	//#pragma endregion variable

//#ifdef FIX_GAIN_FORCE_BUG
	CX2DamageEffect::CEffectHandle m_hGainForceIntakeEffect;	// ���� ��� ȸ���� ������ ����Ʈ�� �ڵ�
	int		m_iGainForceDamageTimeNow;	// ���� ��� ȸ���� ���� ����,(Ÿ�� Ƚ�� ����)
//#endif // FIX_GAIN_FORCE_BUG
};
//#pragma endregion Ŭ����

IMPLEMENT_INTRUSIVE_PTR( CX2GUAra::SLockOnSystem );
IMPLEMENT_INTRUSIVE_PTR( CX2GUAra::sMoonLightSlashData );
