#pragma once

//{{ kimhc // 2010.11.18 // 2010-12-23 New Character CHUNG

/** @class : CX2GUChung
	@brief : 2010-12-23 �� ��ġ�� ĳ���� û Ŭ����
	@date  : 2010/11/18
*/
class CX2GUChung : public CX2GUUser
{
public:

	enum CHUNG_STATE_ID
	{
		CSI_BASE = USI_END	+ 1,			/// û���� ������Ʈ �� ���� ID

		//CSI_POWER_DOWN,			/// ���� ���¿��� DOWN ȭ��ǥ�� ������ ��
		CSI_POWER_DOWN_LANDING,		/// ���� ���¿��� DOWN ȭ��ǥ�� ���ؼ� ���� ���� ���� ������Ʈ

		CSI_STANDUP_ROLLING_FRONT_FRONT,
		CSI_STANDUP_ROLLING_FRONT_BACK,
		CSI_STANDUP_ROLLING_BACK_FRONT,
		CSI_STANDUP_ROLLING_BACK_BACK,

		CSI_STANDUP_ATTACK_FRONT,
		CSI_STANDUP_ATTACK_BACK,

		CSI_WALL_LANDING,

		CSI_CHARGE_CANNON_BALL,				/// ĳ������ ������ ���

		CSI_GUARD,
		CSI_GUARD_ATTACK_Z,
		CSI_GUARD_ATTACK_X,

		CSI_COMBO_Z,
		CSI_COMBO_ZZ,
		CSI_COMBO_ZZZ,

		CSI_COMBO_ZZZZ,

		CSI_COMBO_ZZZX_READY,
		CSI_COMBO_ZZZX_CANNON_BALL_SHOT,	/// ĳ������ 2�� �̻����� �� �߻��ϴ� ��� (1���������� ������� �ʰ� �ٷ� finish�� �����)
		CSI_COMBO_ZZZX_FINISH,				/// ĳ���� 1�� �̻� ���� �� �߻� �ϴ� ������ ���

		CSI_COMBO_ZZdownZ,
		CSI_COMBO_ZZdownZZ,

		CSI_COMBO_X,
		CSI_COMBO_XX,
		CSI_COMBO_XXX,

		CSI_COMBO_XXZ,

		CSI_COMBO_XXZX_CANNON_BALL_SHOT,	/// ĳ������ 2�� �̻����� �� �߻��ϴ� ��� (1���������� ������� �ʰ� �ٷ� finish�� �����)
		CSI_COMBO_XXZX_FINISH,				/// ĳ���� 1�� �̻� ���� �� �߻� �ϴ� ������ ���

		CSI_COMBO_DASH_Z,

		CSI_ATTACK_DASH_X,

		CSI_ATTACK_JUMP_Z,
		CSI_ATTACK_JUMP_X,

		CSI_COMBO_DASH_JUMP_Z,
		CSI_COMBO_DASH_JUMP_ZZ,
		CSI_COMBO_DASH_JUMP_ZZLanding,
		CSI_COMBO_DASH_JUMP_ZZLanding_CANNON_BALL_SHOT,

		CSI_ATTACK_DASH_JUMP_X,

		CSI_STANDUP_CANNON_ATTACK_FRONT,
		CSI_STANDUP_CANNON_ATTACK_BACK,

		//{{ kimhc // 2011.1.4 // û 1�� ����
		// ǻ�������(FG)
		CSI_CFG_COMBO_ZZX,
		CSI_CFG_COMBO_DASH_ZZ,
		CSI_CFG_COMBO_DASH_ZZZ,
		CSI_CFG_COMBO_DASH_JUMP_ZX,
		CSI_CFG_COMBO_DASH_JUMP_ZXX,
		CSI_CFG_COMBO_DASH_JUMP_ZXXLanding,
		//}} kimhc // 2011.1.4 // û 1�� ����

		///////////////////////////////////////////////////////
		//{{ ���� : 2011-01-12 : û 1�� ���� ���ð����
		CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON,
		CSI_CSG_COMBO_XFRONTX_FIRE,
		CSI_CSG_COMBO_XFRONTXX,

		CSI_CSG_COMBO_DASH_XX,
		CSI_CSG_COMBO_DASH_XXX,

		CSI_CSG_COMBO_DASH_JUMP_X,
		CSI_CSG_COMBO_DASH_JUMP_XX,
		CSI_CSG_COMBO_DASH_JUMP_XX_Landing,

		//}} ���� : 2011-01-12 : û 1�� ���� ���ð����
		///////////////////////////////////////////////////////

		//{{ JHKang / ������ / 2011.6.14 / û 2�� ���� ���鸮 ü�̼�
		CSI_CDC_COMBO_X,
		CSI_CDC_COMBO_XX,

		CSI_CDC_COMBO_XZ,
		CSI_CDC_COMBO_XZZ,

		CSI_CDC_COMBO_ZZX,
		CSI_CDC_COMBO_ZZXX,
		CSI_CDC_COMBO_ZZXXX,
		//}}

		///////////////////////// ��ų ////////////////////////////////////////////////////

		// ������̼�
		CSI_SI_SA_CHUNG_DETONATION_LOOP,
		CSI_SI_SA_CHUNG_DETONATION_END,
		// �糪ƽ ���ο�
		CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION,
		CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH,

		//{{���� : 2010-12-15 : Gigantic impact(specialactive)
		CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY,
		CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_HYPER,
		CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING,
		CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_HYPER,
		//}} ���� : 2010-12-15 : Gigantic impact(specialactive)

		//{{ kimhc // 2011.1.4 // û 1�� ����
		// �糪ƽǻ��(ǻ�������)
		CSI_SI_SA_CFG_LUNATIC_FURY_RUN,
		CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK,
		CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING,
		CSI_SI_SA_CFG_LUNATIC_FURY_END,
		CSI_SI_SA_CFG_LUNATIC_FURY_END_HYPER,

		// ��������(ǻ�������)
		CSI_SI_A_CFG_LEAP_ATTACK_JUMP,
		CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN,
		CSI_SI_A_CFG_LEAP_ATTACK_LANDING,

		// ����(ǻ�������)
		CSI_SI_A_CFG_GUARD_WAIT,
		CSI_SI_A_CFG_GUARD_END,
		CSI_SI_A_CFG_GUARD_DAMAGE,		// ���� ��ų�� �� ���¿��� �������� �޴� ���

		// ��������(���ð����)
		CSI_SI_SA_CSG_DUAL_BUSTER_LANDING,

		// ���ý�Ÿ(���ð����)
		CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK,
		CSI_SI_SA_CSG_SHOOTING_STAR_END,
		//}} kimhc // 2011.1.4 // û 1�� ����

		//{{ ���� : 2011-01-18 : ��Ƽ�� ��ų - ������/��弦
		CSI_SI_A_CSG_RUMBLE_SHOT,
		CSI_SI_A_CSG_HEAD_SHOT,
		//}} ���� : 2011-01-18 : ��Ƽ�� ��ų - ������/��弦

		//{{ kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų �� ���� ����� 2��, ī��ũ����
		// ī��ũ����(���ð����)
		CSI_SI_SA_CSG_COMET_CRASHER_LOOP,
		CSI_SI_SA_CSG_COMET_CRASHER_FINISH,
		//}} kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų �� ���� ����� 2��, ī��ũ����

		//{{ JHKang / ������ / 2011-6-17 / û 2�� ���� ���鸮 ü�̼� ���� ��, �ҷ� ������, ��ƿ���� ��Ʈ����ũ
		CSI_SI_A_CDC_LEG_SHOT,

		CSI_SI_SA_CDC_BULLET_BLITZ,

		CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_END,

		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN,

		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT,
		CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT,
		//}}

		CSI_CIP_COMBO_POWER_DOWN_LANDING_Z,
		CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ,
		CSI_CIP_COMBO_DASH_XX,
		CSI_CIP_COMBO_DASH_XXX,
		CSI_CIP_COMBO_DASH_XXXX,
		CSI_CIP_COMBO_DASH_XXXZ,
		CSI_SI_A_CIP_SUDDEN_BURSTER,
		CSI_SI_SA_CIP_BURST_WOLF,
		CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN,
		CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING,

		CSI_CHG_COMBO_X,
		CSI_CHG_COMBO_XX,

		CSI_CHG_COMBO_XZ,

		CSI_CHG_DASH_COMBO_ZZ,
		CSI_CHG_DASH_COMBO_ZZX,

		CSI_CHG_DASH_JUMP_COMBO_ZX,

		CSI_SI_A_SIEGE_SHELLING_READY,
		CSI_SI_A_SIEGE_SHELLING_READYLOOP,
		CSI_SI_A_SIEGE_SHELLING_X,
		CSI_SI_A_SIEGE_SHELLING_Z_FINISH,
		CSI_SI_A_SIEGE_SHELLING_X_FINISH,

		CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE,
		CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT,

		CSI_A_CHG_ELASTIC_BOMB,
		CSI_SA_CHG_WONDER_WALL,
		CSI_SA_CHG_DREAD_CHASE_START,
		CSI_SA_CHG_DREAD_CHASE_LOOP,
		CSI_SA_CHG_DREAD_CHASE_END,

		CSI_SA_CHG_BIGBANG_STREAM,
		CSI_SA_CHG_CARPET_BOMBING,

		CSI_SI_A_CHG_CHAOS_CANNON_START,
		CSI_SI_A_CHG_CHAOS_CANNON_WAIT,
		CSI_SI_A_CHG_CHAOS_CANNON_WALK,
		CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY,
		CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP,
		CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN,
		CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG,
		CSI_SI_A_CHG_CHAOS_CANNON_01,
		CSI_SI_A_CHG_CHAOS_CANNON_02,
		CSI_SI_A_CHG_CHAOS_CANNON_03,
		CSI_SI_A_CHG_CHAOS_CANNON_GUARD,
		CSI_SI_A_CHG_CHAOS_CANNON_END,

		CSI_A_CHG_MARK_OF_COMMANDER,

		CSI_SI_A_CFG_GUARD_READY_MEMO,

		CSI_CTT_COMBO_ZZX,
		CSI_CTT_COMBO_ZZXX,
		CSI_CTT_DASH_COMBO_XX,
		CSI_CTT_MOBILE_ARMORED_MODE_READY,
		CSI_CTT_MOBILE_ARMORED_MODE_LOOP,
		CSI_CTT_MOBILE_ARMORED_MODE_LOOP_FOWARD,
		CSI_CTT_MOBILE_ARMORED_MODE_LOOP_BACKWARD,
		CSI_CTT_MOBILE_ARMORED_MODE_Z,
		CSI_CTT_MOBILE_ARMORED_MODE_X,
		CSI_CTT_MOBILE_ARMORED_MODE_UNREADY,
		CSI_A_CTT_REMOTE_MORTAR,		/// û 2�� ���� ��Ƽ�� Ʈ���� - �ڰ���
		CSI_SI_SA_CTT_CROSS_FIRE,
		CSI_SA_CTT_TACTICAL_FIELD,		/// û 2�� ���� ��Ƽ�� Ʈ���� - ��Ƽ�� �ʵ�
		CSI_CTT_COMBO_DASH_X,

//#ifdef UPGRADE_SKILL_SYSTEM_2013 // ������
		CSI_A_CHUNG_IMPACT_DETONATION_LOOP,	// ����Ʈ ������̼�
		CSI_A_CHUNG_IMPACT_DETONATION_FINAL,
		CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN,	// ���� ������� - �����ũ
		CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING,
//#endif // UPGRADE_SKILL_SYSTEM_2013

		CSI_A_CDC_RAPID_CANNON_LOOP,		// ���ǵ� ĳ�� ����
		CSI_A_CDC_RAPID_CANNON_FINAL,		// ���ǵ� ĳ�� ������ ��

		CSI_A_CIP_ARTILLERY_NOVA_FIRE,		// ��ƿ���� ��� �߻�

	};

	enum CHUNG_MAJOR_PARTICLE_INSTANCE_ID
	{
		CHUNG_MAJOR_PII_BASE,		/// û�� ���� ������ ��ƼŬ( �ӽ÷� �־� ���� )
		CHUNG_MAJOR_PII_END,
	};

	enum CHUNG_MINOR_PARTICLE_INSTANCE_ID
	{
		CHUNG_MINOR_PII_BASE,		/// û�� ���� ���̳� ��ƼŬ( �ӽ÷� �־� ���� )
		CHUNG_MINOR_PII_END,
	};

	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	enum CHUNG_MAJOR_MESH_INSTANCE_ID
	{
		CHUNG_MAJOR_MII_BASE,
		CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_PULL_SUBWEAPON_MINI_GUN,
		CHUNG_MAJOR_MII_CSG_COMBO_XFRONTX_FIRE_MINI_GUN,
		CHUNG_MAJOR_MII_CSG_COMBO_XFRONTXX_MINI_GUN,

		CHUNG_MAJOR_MII_CSG_COMBO_DASH_XX_MINI_GUN,
		CHUNG_MAJOR_MII_CSG_COMBO_DASH_XXX_MINI_GUN,

		CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_X_MINI_GUN,
		CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_MINI_GUN,
		CHUNG_MAJOR_MII_CSG_COMBO_DASH_JUMP_XX_LANDING_MINI_GUN,

		CHUNG_MAJOR_MII_SI_A_CSG_HEAD_SHOT_MINI_GUN,

		CHUNG_MAJOR_MII_CDC_COMBO_X_MINI_GUN_L,

		CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_L,
		CHUNG_MAJOR_MII_CDC_COMBO_XZ_MINI_GUN_R,

		CHUNG_MAJOR_MII_CDC_COMBO_XZZ_MINI_GUN_L,

		CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_L,
		CHUNG_MAJOR_MII_CDC_COMBO_ZZXX_MINI_GUN_R,

		CHUNG_MAJOR_MII_SI_A_CDC_LEG_SHOT_MINI_GUN_R,

		CHUNG_MAJOR_MII_SI_SA_CDC_BULLET_BLITZ_MINI_GUN_L,
		CHUNG_MAJOR_MII_END,
	};
	//}} ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�

	// ��ƿ���� ��Ʈ����ũ Ÿ��
	enum CHUNG_ARTILLERY_STIKE_TYPE
	{
		CAST_NONE = 0,
		CAST_MISSILE_SHOWER,	// �̽� ����
		CAST_QUANTUM_BALLISTA,	// ���� �߸���Ÿ
	};


	struct GUARD_STATE_INFO
	{
		CHUNG_STATE_ID	m_eStateId;
		D3DXVECTOR2		m_vecGuardTimer;
	};

	///////////////////////////////////////////////////////

	/** @class : CGuardSystem
		@brief : û�� ����ϴ� ���� �ý��� Ŭ����
	*/
	class CGuardSystem
	{
	private:
		int		m_iGuardPointRemain;				/// �����ִ� ���� �� �� �ִ� Ƚ��
		int		m_iMaxGuardPoint;					/// ������ �� �ִ� �ִ� Ƚ��
		int		m_iDamageReducePercentGuardState;	/// ���� ������Ʈ���� ���ҵ� �������� Percent

	public:
		static const int INITIAL_MAX_GUARD_POINT = 6;

	public:
		CGuardSystem() : m_iGuardPointRemain( INITIAL_MAX_GUARD_POINT ),
			m_iMaxGuardPoint( INITIAL_MAX_GUARD_POINT ), m_iDamageReducePercentGuardState( 0 )
		{}

		int GetGuardPointRemain() const { return m_iGuardPointRemain; }
		__forceinline void SetGuardPointRemain( int iGuardPointRemain_ )
		{
			m_iGuardPointRemain = iGuardPointRemain_;

			if ( 0 > m_iGuardPointRemain )
				m_iGuardPointRemain = 0;
		}

		int GetMaxGuardPoint() const { return m_iMaxGuardPoint; }
		void SetMaxGuardPoint( int iGuardPointAtEachState_ ) { m_iMaxGuardPoint = iGuardPointAtEachState_; }

		// �����ִ� ���� ����Ʈ�� �� ������Ʈ���� ������ �� �ִ� Ƚ���� �ٽ� ä��
		void ChargeGuardPointRemain() { SetGuardPointRemain( GetMaxGuardPoint() ); }

		int GetDamageReducePercentGuardState() const { return m_iDamageReducePercentGuardState; }
		void SetDamageReducePercentGuardState( int iDamageReducePercentGuardState_ ) { m_iDamageReducePercentGuardState = iDamageReducePercentGuardState_; }
	};

	/** @class : CCannonBallSystem
		@brief : û�� ����ϴ� ĳ���� �ý��� Ŭ����
		@date  : 2010/12/05
	*/
	class CCannonBallSystem
	{
	private:
		float	m_fTimeNeededToChangeCannonBallChargeState;	/// ĳ���� ���� ���·� �Ѿ�� ���� �ʿ���, Wait���¿��� �����ؾ� �ϴ� �ð�
		float	m_fTimeElapsedAtWaitState;					/// Wait ���¿��� �ٸ� ���·� �ٲ��� �ʰ� ���ӵ� �ð�
		int		m_iUsedCannonballCountAtThisState;			/// ���� ������Ʈ���� �Ҹ��� CannonBall�� ����

	public:
		CCannonBallSystem() : m_fTimeNeededToChangeCannonBallChargeState( 0.5f ),
			m_fTimeElapsedAtWaitState( 0.0f ), m_iUsedCannonballCountAtThisState( 0 )
		{}

		float GetTimeNeededToChangeCannonBallChargeState() const { return m_fTimeNeededToChangeCannonBallChargeState; }	// CannonBallChaarge ���·� �Ѿ�� ���� �����ؾ��� �ð�
		void SetTimeNeededToChangeCannonBallChargeState(float fTimeNeededToChangeCannonBallChargeState_) { m_fTimeNeededToChangeCannonBallChargeState = fTimeNeededToChangeCannonBallChargeState_; } // CannonBallChaarge ���·� �Ѿ�� ���� �����ؾ��� �ð�

		float GetTimeElapsedAtWaitState() const { return m_fTimeElapsedAtWaitState; }	// Wait ���¿��� ������ �ð�
		void SetTimeElapsedAtWaitState(float fTimeElapsedAtWaitState_) { m_fTimeElapsedAtWaitState = fTimeElapsedAtWaitState_; }	// Wait ���¿��� ������ �ð�

		int GetUsedCannonballCountAtThisState() const { return m_iUsedCannonballCountAtThisState; }	// ���� ������Ʈ���� �Ҹ��� CannonBall�� ����
		void SetUsedCannonballCountAtThisState( int iUsedCannonballCountAtThisState_) { m_iUsedCannonballCountAtThisState = iUsedCannonballCountAtThisState_; }

	};

	//{{ kimhc // 2011.1.11 // û 1�� ����
	/** @class : CFuryGuardianGuardSkill
	@brief : û 1�� ���� ǻ��������� ����ϴ� Guard ��Ƽ�꽺ų�� Ŭ����
	@date  : 2011/01/11
	*/

    class   CFuryGuardianGuardSkill;
    typedef x2::intrusive_ptr<CFuryGuardianGuardSkill> CFuryGuardianGuardSkillPtr;	/// ǻ��������� ����ϴ� Guard ��Ƽ�꽺ų Ŭ������ ����Ʈ������Type

	class CFuryGuardianGuardSkill : x2::NonCopyable
	{
	private:
        unsigned                                        m_uRefCount;

		CHUNG_STATE_ID	m_eSkillSlotID_Guard;			/// ǻ��������� Guard�� ��� ��ų���Կ� �����Ǿ� �ִ����� ��� ����
		const bool*		m_pGuardKey;					/// ǻ��������� Guard�� �����Ǿ� �ִ� ��ų������ Key(A, S, D, C ��)
		bool			m_bGuardEquippedAtSlotB;		/// ǻ��������� Guard�� B���Կ� �����Ǿ� �ִ����� ��� ����
		int				m_iDamageReducePercent;			/// ǻ��������� Guard ���¿��� ������ ������
		CFuryGuardianGuardSkill() : m_uRefCount(0),
			m_eSkillSlotID_Guard( CSI_BASE ), m_pGuardKey( NULL ), m_bGuardEquippedAtSlotB( false )
            , m_iDamageReducePercent( 0 )
		{}

	public:
		static CFuryGuardianGuardSkillPtr   CreateFuryGuardianGuardSkill() { return CFuryGuardianGuardSkillPtr( new CFuryGuardianGuardSkill ); }

		CX2GUChung::CHUNG_STATE_ID GetSkillSlotID_Guard() const { return m_eSkillSlotID_Guard; }	// ǻ��������� Guard�� ��� ��ų���Կ� �����Ǿ� �ִ����� ��� �������� ����
		void SetSkillSlotID_Guard(CX2GUChung::CHUNG_STATE_ID eSkillSlotID_Guard_) { m_eSkillSlotID_Guard = eSkillSlotID_Guard_; } // ǻ��������� Guard�� ��� ��ų���Կ� �����Ǿ� �ִ����� ��� �������� Set��

		bool GetGuardKeyValue() const { return *m_pGuardKey; }	// ǻ��������� Guard�� �����Ǿ� �ִ� ��ų������ Key(A, S, D, C ��)	�� ���� ���� (���ȴ��� �ȴ��ȴ���)
		void SetGuardKeyPointer( const bool* pGuardKey_ ) { m_pGuardKey = pGuardKey_; }	// ǻ��������� Guard�� �����Ǿ� �ִ� ��ų������ Key(A, S, D, C ��)	�����͸� Set��

		bool GetGuardEquippedAtSlotB() const { return m_bGuardEquippedAtSlotB; }	// ǻ��������� Guard�� B���Կ� �����Ǿ� �ִ����� ����
		void SetGuardEquippedAtSlotB(bool bGuardEquippedAtSlotB_) { m_bGuardEquippedAtSlotB = bGuardEquippedAtSlotB_; }

		int GetDamageReducePercent() const { return m_iDamageReducePercent; }
		void SetDamageReducePercent(int iDamageReducePercent_) { m_iDamageReducePercent = iDamageReducePercent_; }

        void    AddRef()    {   ++m_uRefCount; }
        void    Release()   { if ( (--m_uRefCount) == 0 )   delete this; }

	};


	//}} kimhc // 2011.1.11 // û 1�� ����

	int GetShot() const { return m_iShot; }
	void SetShot( int iNPCId_ ) { m_iShot = iNPCId_; }
	virtual bool CanRidingState();

private:
	CX2GUChung( int iUnitIndex_, int iTeamNum_,
			lua_State*   pScriptLuaState,
		int iFrameBufferNum_, IN CX2Unit* pUnit_ );
	virtual ~CX2GUChung();

protected:
	virtual void		Init( bool bUseTeamPos_, int iStartPosIndex_ );	// û�� ������ �ִ� ����� �ʱ�ȭ
	virtual void		ParseCommonRandomState();	// ������ Win, Lose, Start ���� ���� �����ϰ� Load �ϱ� ���� �Լ�
	virtual void		InitState();				// ��� ���¸� �ʱ�ȭ

	void				InitStateID();				// �⺻���� State�� ID�� �ʱ�ȭ (walk, dash, jump ��)
	void				InitStateCommon();			// �������� ������ �ִ� State(������ ������Ʈ ���� �ٸ�)�� �ʱ�ȭ
	void				InitStateByUnitClass();		// ���� ���� �ٸ� ������Ʈ�� �ʱ�ȭ
	virtual void		InitComponent();			// ĳ���� ��ũ��Ʈ�� INIT_COMPONENT ���̺��� �ε���

	virtual void		InitPassiveSkillState();	// �нú� ��ų �ʱ�ȭ
	virtual void		SetEquippedSkillFuncBySkillSlotIndex( const CX2SkillTree::SKILL_ID eSkillID_, const int iSlotIndex_, const bool bSlotB_ );	// ������ ��ų ���� ����
	virtual void		SetEquippedSkillLevel( const CX2SkillTree::SKILL_ID eSkillID_, const bool bChangeAll_ );	// ������ ��ų�� ������ ���� �� ��ų�� �ش��ϴ� ������ PowerRate���� ����
	void				SetEquippedSkillLevelStateData( const CX2SkillTree::SkillTemplet* pSkillTemplet, CHUNG_STATE_ID eStateID );	// ������ ��ų�� �������� ����� �� ���� ����Ǿ�� �� ������Ʈ ����Ÿ

	virtual void		CommonStateStartFuture();	// ������Ʈ�� ����� �� ���� �ѹ��� ����Ǵ� �Լ��� �ش� ������Ʈ�� lua ���̺��� �о��. ��, Future�� LocalUnit�� ����
	virtual void		CommonStateStart();	// ������Ʈ�� ����� �� ���� �ѹ��� ����Ǵ� �Լ��� �ش� ������Ʈ�� lua ���̺��� �о��

	virtual void		CommonFrameMoveFuture();
	virtual void		CommonFrameMove();

	virtual void		CommonStateEndFuture();
	virtual void		CommonStateEnd();

	virtual RENDER_HINT	CommonRender_Prepare();

	virtual bool		SpecialAttackEventProcess( CX2SkillTree::ACTIVE_SKILL_USE_CONDITION eActiveSkillUseCondition = CX2SkillTree::ASUT_GROUND );
	virtual void		NoStateChangeActionFrameMove();

	virtual float		GetActualMPConsume( const CX2SkillTree::SKILL_ID eSkillID_, const int iSkillLevel_ ) const;

	void				LoadPersonalInfo( IN KLuaManager& luaManager_ );	// û���� ������ �ִ� �Ľ��� ���� ���� �ε���
	void				LoadStateIDCanBeChangedGuardState( IN KLuaManager& luaManager_ );	// ���� ������Ʈ�� �Ѿ �� �ִ� ������Ʈ ID ����Ʈ�� ��ũ��Ʈ�� ���� �о����
	void				LoadPromotionItemID( IN KLuaManager& luaManager_ );


	bool				IsThisStateCanBeChangedToGuardStateOnHit();	// Ÿ���� �޾��� �� Guard ���·� ���� �� �� �ִ� ������Ʈ �ΰ�?

	void				ChangeAnimationChargeCannonBallNow();
	void				ChangeAnimationChargeCannonBallFuture();


	__forceinline void UpCannonBallCount( int iUpCount_, bool bForce = false, bool bNoCheckReloadMastery = false );

	void UpCannonBallCountAtThisAnimationTime( float fAnimationTimeNeededToChargeCannonBall_, int iUpCount_ );	// ������ �ִϸ��̼� Ÿ�ӿ� ĳ���� ī��Ʈ�� �Ѱ� �ø���. (m_pXSkinAnim�� animation �ð��� üũ��)
	void UseCannonAtThisAnimationTime( float fAnimationTimeNeededToUseCannonBall_, int iUpCount_ );	// ������ �ִϸ��̼� Ÿ�ӿ� ĳ���� ī��Ʈ�� �Ѱ� �Ҹ��Ѵ�. (m_pXSkinAnim�� animation �ð��� üũ��)

	void				UseCannon( int iCount_ );	// ĳ���� ��� �� �� ȣ�� �ؾ��Ѵ�.
	void				ConsumeCannonBallCount( int iCount_  );	// ĳ������ ������ ���� ��ŭ �Ҹ��Ѵ�.

	__forceinline bool	CanUseCannon();
	__forceinline bool	IsEmptyCannonBallCount();
	__forceinline bool	IsFullCannonBallCount();

	__forceinline int	GetTotalCannonBallCount();
	void				SetTotalCannonBallCount( int iTotalCannonBallCount_ );

	__forceinline int	GetExtraCannonBallCount();
	void				SetExtraCannonBallCount( int iExtraCannonBallCount_ )
	{
		GetChungGageData()->SetCannonBallChanged( true );
		GetChungGageData()->SetNowCannonBallCountEx( iExtraCannonBallCount_ );
	}

	void				SetMaxExtraCannonBallCount( int iMaxExtraCannonCount_ ) { GetChungGageData()->SetMaxCannonBallCountEx( iMaxExtraCannonCount_ ); }

	void				SetCannonBallCount( int iCannonBallCount_ )
	{
		GetChungGageData()->SetCannonBallChanged( true );
		GetChungGageData()->SetNowCannonBallCount( iCannonBallCount_ );
	}
	void				SetMaxCannonBallCount( int iMaxCannonCount_ ) { GetChungGageData()->SetMaxCannonBallCount( iMaxCannonCount_ ); }

	CX2ChungGageData*	GetChungGageData() { return static_cast<CX2ChungGageData*>( GetGageData() ); }
	const CX2ChungGageData*	GetChungGageData() const { return static_cast<const CX2ChungGageData*>( GetGageData() ); }
	void				InitPromotionEquipList();

// 	CX2OldGageManager::CCannonBallUIPtr GetCannonBallUIPtr()
// 	{
// 		ASSERT( 0 < m_GageManager.GetCannonBallUIPtr().use_count() );
// 		return m_GageManager.GetCannonBallUIPtr();
// 	}

	void ShowActiveSkillCutInAndLightByBerserkMode( float fTimeToShow_, bool bOnlyLight_ = false );	// �������� �ð��� ����ȭ��忡 ���� ��ų ���ΰ� light�� �����

	virtual void ShowActiveSkillCutInAndLightByScript( float fTimeToShow_, bool bOnlyLight_ );

	// ���ε� ĳ���� ���ؼ� ������ ĳ������ ���� (���ε�ĳ���� �⺻ ��ų�̹Ƿ� ��� ������ ����)
	UCHAR GetCannonBallCountToBeUpUsingReLoadCannon() const { return m_ucCannonBallCountToBeUpUsingReLoadCannon; }
	void SetCannonBallCountToBeUpUsingReLoadCannon(UCHAR ucCannonBallCountToBeUpUsingReLoadCannon_ ) { m_ucCannonBallCountToBeUpUsingReLoadCannon = ucCannonBallCountToBeUpUsingReLoadCannon_; }
	UCHAR GetCannonBallCountToBeUpUsingReLoadCannon() { return m_ucCannonBallCountToBeUpUsingReLoadCannon; }

	UCHAR GetBasicCannonBallCount() const { return m_ucBasicCannonBallCount; }
	void SetBasicCannonBallCount( UCHAR ucBasicCannonBallCount_ ) { m_ucBasicCannonBallCount = ucBasicCannonBallCount_; }

	void ResetCannonBallCountWhenRebirthInPVPGame( int iNumOfMyTeamKill_, int iNumOfEnmyTeamKill_ );

	void ShowOnomatopoeiaParticle( const WCHAR* wszWeaponBoneName_, const WCHAR* wszParticleName_ );	// '��' �ϴ� �Ǽ��� ��ƼŬ�� ���

	float GetDamageRelByCannonBallMastery() const { return m_fDamageRelByCannonBallMastery; }
	void SetDamageRelByCannonBallMastery( float fDamageRelByCannonBallMastery_ ) { m_fDamageRelByCannonBallMastery = fDamageRelByCannonBallMastery_; }

//{{ kimhc // 2011.1.14 // û 1�� ����
	float GetForceDownRelAtMeleeAttack() const { return m_fForceDownRelAtMeleeAttack; }	// ���� ���� �����ٿ� ��ġ ����
	void SetForceDownRelAtMeleeAttack(float fForceDownRelAtMeleeAttack_) { m_fForceDownRelAtMeleeAttack = fForceDownRelAtMeleeAttack_; }

	float GetProbabilityBigDamageAtGuardMastery() const { return m_fProbabilityBigDamageAtGuardMastery; }	// ���帶���͸��� ���� GuardX���� ������ big�������� �� Ȯ��
	void SetProbabilityBigDamageAtGuardMastery(float Val_) { m_fProbabilityBigDamageAtGuardMastery = Val_; }

	void InitRemodelMagazinePassiveSkill(); // źâ/��� ���� �нú� ��ų Init�Լ�

	float GetIncreaseRangeSubWeaponShot() const { return m_fIncreaseRangeSubWeaponShot; }	// û 1�� ����, ���� ������� ������� ��Ÿ� ������(�������� �нú�)
	void SetIncreaseRangeSubWeaponShot(float fIncreaseRateSubWeaponShot_) { m_fIncreaseRangeSubWeaponShot = fIncreaseRateSubWeaponShot_; }

	virtual float GetCriticalValue( float fDefenderCritical_ = 0.f );

	float GetCriticalDamageRate() const { return m_fCriticalDamageRate; }	// û 1�� ����, ���� ������� ���������� ���� Critical �� ������ ����
	void SetCriticalDamageRate(float fCriticalDamageRate_) { m_fCriticalDamageRate = fCriticalDamageRate_; }
	float CalculateHpPercentUpRateByDamageDataHpUp( float fDamageDataHpPercentUp_ );

	void SetReduceGuardDamageAtHeavyStanceFromGuardSkill();	// ǻ��������� ���� ��ų�� ���ؼ� ������ ���� ����ٽ��� ������ �������� ����
//}} kimhc // 2011.1.14 // û 1�� ����

	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-19
	//}} ���� : 2011-01-19
	///////////////////////////////////////////////////////
	float GetProbabilityIronWill() const { return m_fProbabilityIronWill; }
	void SetProbabilityIronWill(float Val_) { m_fProbabilityIronWill = Val_; }

	void SetSelfDestructSummonedNPC( CX2UnitManager::NPC_UNIT_ID eNPCID = CX2UnitManager::NUI_NONE, float fDeleteTime = 3.f );


	virtual CX2GageData*	CreateGageData();

	void SetExtraDamageMiniGun( CX2DamageManager::DamageData& DamageData_, float fRate_ ); //

public:
	static CX2GUChung*	CreateGUChung( int iUnitIndex_, int iTeamNum_,
			lua_State*   pScriptLuaState,
		int iFrameBufferNum_, IN CX2Unit* pUnit_ )	// ���丮 �Լ�
	{
		return new CX2GUChung( iUnitIndex_, iTeamNum_,
			pScriptLuaState,
			iFrameBufferNum_, pUnit_ );
	}


	static void			AppendDeviceToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendComponentToDeviceList( CKTDXDeviceDataList& listInOut_ );
	static void			AppendEffectToDeviceList( CKTDXDeviceDataList& listInOut_, CX2DamageManager::EXTRA_DAMAGE_TYPE eDamageType_ );

	virtual void		ReInit( bool bTeamPos_ = false, int iStartPosIndex_ = -1 );	// ���ӳ����� ��Ȱ�� � ���� �ٽ� ������ ������ �� ����

	virtual void		DamageReact( OUT CX2DamageManager::DamageData* pDamageData_ );	// �ǰݵ��� �� ����Ǵ� �Լ�

	virtual void		Win();	// �¸����
	virtual void		Lose();	// �й���
	virtual void		Draw();	// ����� ���� ���

	virtual void		InitEffect();	// ĳ���Ͱ� ���� ����Ʈ �� �������� ���� ����Ʈ(�뽬 �Ҷ� ������ ���� ��) �� �� ĳ���� ���� ���� ����Ʈ �ʱ�ȭ
	virtual void		InitEnchantWeaponEffectUnitSpecific();	// �� ĳ���Ͱ� ���� ���� �� ��þƮ ��ƼŬ �ʱ�ȭ (��������, ������, �������� ���⼭ �ϰ� �׿��� ���� �ٸ������� ��)

	void				InitializeChungMajorParticleArray();	// ������ ��ƼŬ �迭�� �ʱ�ȭ
	virtual void		CreateMajorParticleArray();		// ������ ��ƼŬ �迭�� ����
	static void			AppendMajorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	CKTDGParticleSystem::CParticleEventSequence* SetChungMajorParticleByEnum( CHUNG_MAJOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����
	ParticleEventSequenceHandle	GetHandleChungMajorParticleByEnum( CHUNG_MAJOR_PARTICLE_INSTANCE_ID eVal_ ) const // û�� ���� ������ ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����
	{
		ASSERT( CHUNG_MAJOR_PII_END > eVal_ && CHUNG_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		return m_ahChungMajorParticleInstance[eVal_];
	}

	void				SetHandleChungMajorParticleByEnum( CHUNG_MAJOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ ) // û�� ���� ������ ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������
	{
		ASSERT( CHUNG_MAJOR_PII_END > eVal_ && CHUNG_MAJOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		m_ahChungMajorParticleInstance[eVal_] = hHandle_;
	}
	void				DeleteChungMajorParticle();

	void				InitializeChungMinorParticleArray();	// ���������� ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ
	virtual void		CreateMinorParticleArray();		// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �迭�� �ʱ�ȭ
	static void			AppendMinorParticleToDeviceList( CKTDXDeviceDataList& listInOut_ );
	CKTDGParticleSystem::CParticleEventSequence* SetChungMinorParticleByEnum( CHUNG_MINOR_PARTICLE_INSTANCE_ID eVal_, wstring wstrParticleName_, int iDrawCount_ = -1 );	// �� ĳ���͵鸸 ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����
	ParticleEventSequenceHandle	GetHandleChungMinorParticleByEnum( CHUNG_MINOR_PARTICLE_INSTANCE_ID eVal_ ) const	// û�� ���� ���̳� ��ƼŬ �� ENUM ���� �ش��ϴ� ��ƼŬ �ڵ� �ϳ��� ����
	{
		ASSERT( CHUNG_MINOR_PII_END > eVal_ && CHUNG_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		return m_ahChungMinorParticleInstance[eVal_];
	}

	void				SetHandleChungMinorParticleByEnum( CHUNG_MINOR_PARTICLE_INSTANCE_ID eVal_, ParticleEventSequenceHandle hHandle_ )	// û�� ���� ���̳� ��ƼŬ �ڵ� �� ENUM ���� �ش��ϴ� �ڵ��� ������
	{
		ASSERT( CHUNG_MINOR_PII_END > eVal_ && CHUNG_MINOR_PARTICLE_INSTANCE_ID(0) <= eVal_ );
		m_ahChungMinorParticleInstance[eVal_] = hHandle_;
	}
	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	void InitializeMajorMeshArray();	// ������ ��ƼŬ �迭�� �ʱ�ȭ
	void DeleteMajorMesh();
	CKTDGXMeshPlayer::CXMeshInstanceHandle GetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MESH_INSTANCE_ID eVal_ ) const
	{
        ASSERT( CHUNG_MAJOR_MII_END > eVal_ && CHUNG_MAJOR_MESH_INSTANCE_ID(0) <= eVal_ );
		return m_ahChungMajorMeshInstance[eVal_];
	}
	void SetHandleChungMajorMeshByEnum( CHUNG_MAJOR_MESH_INSTANCE_ID eVal_, CKTDGXMeshPlayer::CXMeshInstanceHandle hHandle_ );
	//}} ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	///////////////////////////////////////////////////////
	void				DeleteChungMinorParticle();

	virtual	void		DeleteMinorParticle();

	virtual bool		SpecialAttackNoStageChange( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_ );	// �ʻ�� �� ������Ʈ ������ ���� �� ����
	virtual void		COMMON_BUFF_FrameMove();

	virtual void		NotifyShowObjectChanged();
	virtual void		NotifyDGManagerChanged( CKTDGManager& refInManager_ );		// DGManager�� ���
	void				SetShowPromotionEquips( bool bShow_ );
	virtual void		SetShowViewList( bool bShow_ );
	void				SetShowOriginalViewList( bool bShow_ );
	void				ReplaceWeapon( CX2EqipPtr pWeaponPtr_ );	// ����ȭ ���� �����Ͽ� ���� ����
	virtual bool		UpdateEquipViewListInfo( CX2Item* pItemToAddInViewList, CX2Item* const pItemToRemoveInViewList );
	virtual void		UpdateEquipCollisionData();

	virtual bool		IsFullHyperMode() const { return GetChungGageData()->GetBerserkMode(); }
	virtual void		SetFullHyperMode( const bool bBerserkMode_ )
	{
		GetChungGageData()->SetBerserkModeChanged( true );
		GetChungGageData()->SetBerserkMode( bBerserkMode_ );
	}

	__forceinline int	GetCannonBallCount();

	virtual		HRESULT OnFrameMove( double fTime, float fElapsedTime );

	virtual void ChargeMpAndEtcInTrainingGame();

	virtual void		ApplyHyperModeBuff();
	virtual void		HyperModeBuffEffectStart();
	virtual void		HyperModeBuffEffectEnd();

	virtual void SetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
	virtual void UnSetSpecificValueByBuffTempletID( const BUFF_TEMPLET_ID eBuffTempletId_ );
	virtual void ApplyRenderParamOnEmptyRenderParamByBuff( CKTDGXRenderer::RenderParam* pRenderParam_ );

	void				CreateRemoteMortar();							/// �ڰ��� ��ȯ �Լ�
	void				InsertRemoteMortarNPCUID( UidType iNPCUID ) { m_iRemoteMortarNPCUID = iNPCUID; }	/// �ڰ��� ���̵� �߰� �Լ�
	void				DeleteRemoteMortarNPCUID() { m_iRemoteMortarNPCUID = -1; }							/// �ڰ��� ���̵� ���� �Լ�
	UidType				GetRemoteMortarNPCUID() { return m_iRemoteMortarNPCUID; }							/// �ڰ��� ���̵� ��ȯ �Լ�

	void				SupplyCannonBallCount( int SupplyCount );		/// ��Ƽ�� �ʵ忡�� ���޵� �ɳ���

	virtual float		GetFinalDamageToBeChangedBySkill( float fFinalDamage_ );

	virtual int GetComboZStateID() const { return CSI_COMBO_Z; }
	virtual int GetComboXStateID() const { return CSI_COMBO_X; }
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//							������Ʈ �Լ� ����
	///////////////////////////////////////////////////////////////////////////////////////////////////////
protected:

	// CSI_DIE_FRONT
	void DieFrontStartFuture();
	void DieFrontStart();
	void DieFrontFrameMove();
	void DieFrontEventProcess();

	// CSI_DIE_BACK
	void DieBackStartFuture();
	void DieBackStart();
	void DieBackFrameMove();
	void DieBackEventProcess();

	// CSI_WAIT
	void WaitStartFuture();
	void WaitStart();
	void WaitFrameMoveFuture();
	void WaitEventProcess();

	// CSI_WALK
	void WalkEventProcess();

	// CSI_JUMP_READY
	void JumpReadyEventProcess();

	// CSI_JUMP_UP
	void JumpUpEventProcess();

	// CSI_JUMP_DOWN
	void JumpDownEventProcess();

	// CSI_JUMP_LANDING
	void JumpLandingEventProcess();

	// CSI_DASH
	void DashEventProcess();

	//CSI_DASH_END
	void DashEndStartFuture();
	void DashEndFrameMove();
	void DashEndEventProcess();

	// CSI_DASH_JUMP
	void DashJumpStartFuture();
	void DashJumpStart();
	void DashJumpFrameMoveFuture();
	void DashJumpEventProcess();
	void DashJumpEndFuture();

	// CSI_DASH_JUMP_LANDING
	void DashJumpLandingEventProcess();

	// CSI_POWER_DOWN_LANDING
	void PowerLandingStartFuture();
	void PowerDownLandingStart();
	void PowerDownLandingEventProcess();

	// CSI_HYPER_MODE
	void HyperModeFrameMove();
	void RidingHyperModeFrameMove();
	void CommonHyperModeFrameMove( float fTime1_, float fTime2_, float fTime3_, bool bSound_ = false );
	// CSI_DAMAGE_SMALL_FRONT and CSI_DAMAGE_SMALL_BACK
	void DamageSmallEventProcess();

	// CSI_DAMAGE_BIG_FRONT and CSI_DAMAGE_BIG_BACK
	void DamageBigStart();
	void DamageBigEventProcess();

	// CSI_DAMAGE_DOWN_FRONT
	void DamageDownFrontFrameMove();
	void DamageDownFrontEventProcess();

	// CSI_DAMAGE_DOWN_BACK
	void DamageDownBackFrameMove();
	void DamageDownBackEventProcess();

	// CSI_DAMAGE_AIR_DOWN_LANDING
	void DamageAirDownLandingEventProcess();

	//CSI_STANDUP_ROLLING
	void StandUpRollingEventProcess();
	void StandUpRollingEnd();

	//CSI_STANDUP_ATTACK
	void StandUpAttackEventProcess();
	void StandUpAttackEnd();

	// CSI_CHARGE_CANNON_BALL
	void ChargeCannonBallStart();
	void ChargeCannonBallStartFuture();
	void ChargeCannonBallFrameMove();
	void ChargeCannonBallEventProcess();

	// CSI_GUARD
	void GuardStart();
	void GuardEventProcess();

	// CSI_GUARD_ATTACK_Z
	void GuardAttackZEventProcess();

    // CSI_GUARD_ATTACK_X
	void GuardAttackXInit();
	void GuardAttackXFrameMove();
	void GuardAttackXEventProcess();

	// CSI_COMBO_Z
	void ComboZEventProcess();
	// CSI_COMBO_ZZ
	void ComboZZEventProcess();
	// CSI_COMBO_ZZZ
	void ComboZZZEventProcess();
	// CSI_COMBO_ZZZZ
	void ComboZZZZEventProcess();

	// CSI_COMBO_ZZZX_READY
	void ComboZZZXReadyEventProcess();

	// CSI_COMBO_ZZZX_CANNON_BALL_SHOT
	void ComboZZZXCannonBallShotInit();
	void ComboZZZXCannonBallShotStartFuture();
	void ComboZZZXCannonBallShotFrameMove();
	void ComboZZZXCannonBallShotEventProcess();

	// CSI_COMBO_ZZZX_FINISH
	void ComboZZZXFinishInit();
	void ComboZZZXFinishStartFuture();
	void ComboZZZXFinishFrameMove();
	void ComboZZZXFinishEventProcess();

	// CSI_COMBO_ZZdownZ
	void ComboZZdownZEventProcess();
	// CSI_COMBO_ZZdownZZ
	void ComboZZdownZZEventProcess();

	// CSI_COMBO_X
	void ComboXEventProcess();
	// CSI_COMBO_XX
	void ComboXXInit();

	void ComboXXEventProcess();
	// CSI_COMBO_XXX
	void ComboXXXInit();
	void ComboXXXFrameMove();
	void ComboXXXEventProcess();
	// CSI_COMBO_XXZ
	void ComboXXZEventProcess();

	// CSI_COMBO_XXZX_CANNON_BALL_SHOT
	void ComboXXZXCannonBallShotInit();
	void ComboXXZXCannonBallShotFrameMove();
	void ComboXXZXCannonBallShotEventProcess();

	// CSI_COMBO_XXZX_FINISH
	void ComboXXZXFinishInit();
	void ComboXXZXFinishFrameMove();
	void ComboXXZXFinishEventProcess();

	// CSI_COMBO_DASH_Z
	void ComboDashZEventProcess();

	// CSI_ATTACK_DASH_X
	void AttackDashXInit();
	void AttackDashXFrameMove();
	void AttackDashXEventProcess();
	void AttackDashXEndFuture();

	// CSI_ATTACK_JUMP_Z
	void AttackJumpZEventProcess();
	void AttackJumpZFrameMoveFuture();
    // CSI_ATTACK_JUMP_X
	void AttackJumpXEventProcess();
	void AttackJumpXFrameMoveFuture();

	// CSI_COMBO_DASH_JUMP_Z
	void ComboDashJumpZEventProcess();
	// CSI_COMBO_DASH_JUMP_ZZ
	void ComboDashJumpZZEventProcess();

	// CSI_COMBO_DASH_JUMP_ZZLanding
	void ComboDashJumpZZLandingEventProcess();

	// CSI_COMBO_DASH_JUMP_ZZLanding_CANNON_BALL_SHOT
	void ComboDashJumpZZLandingCannonBallShotInit();

	void ComboDashJumpZZLandingCannonBallShotEventProcess();

	// CSI_ATTACK_DASH_JUMP_X
	void AttackDashJumpXInit();
	void AttackDashJumpXStartFuture();
	void AttackDashJumpXFrameMove();
	void AttackDashJumpXFrameMoveFuture();
	void AttackDashJumpXEventProcess();

	// CSI_STANDUP_CANNON_ATTACK_FRONT && BACK
	void StandUpCannonAttackInit();
	void StandUpCannonAttackFrameMove();
	void StandUpCannonAttackEventProcess();
	void StandUpCannonAttackEnd();

	// CSI_STANDUP_CANNON_ATTACK_BACK
	void StandUpCannonAttackEndFuture();

//{{ kimhc // 2011.1.4 // û 1�� ����
	// CSI_CFG_COMBO_ZZX (ǻ�������)

	void CFGComboZZXEventProcess();

	// CSI_COMBO_DASH_Z (ǻ�� ����� ���)
	void CFGComboDashZEventProcess();

	// CSI_CFG_COMBO_DASH_ZZ
	void CFGComboDashZZEventProcess();

	// CSI_CFG_COMBO_DASH_ZZZ
	void CFGComboDashZZZEventProcess();

	// CSI_CFG_COMBO_DASH_JUMP_ZX
	void CFGComboDashJumpZXEventProcess();

	// CSI_CFG_COMBO_DASH_JUMP_ZXX
	void CFGComboDashJumpZXXStart();
	void CFGComboDashJumpZXXEventProcess();

	// CSI_CFG_COMBO_DASH_JUMP_ZXXLanding
	void CFGComboDashJumpZXXLandingEventProcess();

	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-12 : û 1�� ���� ���ð����

	// CSI_CSG_COMBO_XFRONTX_PULL_SUBWEAPON,
	void CSGComboXFrontXPullSubweaponInit();
	void CSGComboXFrontXPullSubweaponFrameMove();
	void CSGComboXFrontXPullSubweaponEventProcess();
	void CSGComboXFrontXPullSubweaponEnd();
	// CSI_CSG_COMBO_XFRONTX_FIRE,
	void CSGComboXFrontXFireInit();
	void CSGComboXFrontXFireFrameMove();
	void CSGComboXFrontXFireEventProcess();
	void CSGComboXFrontXFireEnd();
	// CSI_CSG_COMBO_XFRONTXX,
	void CSGComboXFrontXXInit();
	void CSGComboXFrontXXFrameMove();
	void CSGComboXFrontXXEventProcess();
	void CSGComboXFrontXXEnd();

	// CSI_CSG_COMBO_DASH_XX,
	void CSGComboDashXXInit();
	void CSGComboDashXXFrameMove();
	void CSGComboDashXXEventProcess();
	void CSGComboDashXXEnd();
	// CSI_CSG_COMBO_DASH_XXX,
	void CSGComboDashXXXInit();
	void CSGComboDashXXXFrameMove();
	void CSGComboDashXXXEventProcess();
	void CSGComboDashXXXEnd();

	// CSI_CSG_COMBO_DASH_JUMP_X,
	void CSGComboDashJumpXInit();
	void CSGComboDashJumpXFrameMove();
	void CSGComboDashJumpXEventProcess();
	void CSGComboDashJumpXEnd();
	// CSI_CSG_COMBO_DASH_JUMP_XX,
	void CSGComboDashJumpXXInit();
	void CSGComboDashJumpXXFrameMove();
	void CSGComboDashJumpXXEventProcess();
	void CSGComboDashJumpXXEnd();
	// CSI_CSG_COMBO_DASH_JUMP_XX_Landing,
	void CSGComboDashJumpXXLandingInit();
	void CSGComboDashJumpXXLandingFrameMove();
	void CSGComboDashJumpXXLandingEventProcess();
	void CSGComboDashJumpXXLandingEnd();
	//}} ���� : 2011-01-12 : û 1�� ���� ���Ͱ����
	///////////////////////////////////////////////////////

//}} kimhc // 2011.1.4 // û 1�� ����

	//{{ JHKang / ������ / 2011.6.14 / û 2�� ����
	// CSI_CDC_COMBO_X
	void CDCComboXInit();
	void CDCComboXFrameMove();
	void CDCComboXEventProcess();
	void CDCComboXEnd();
	// CSI_CDC_COMBO_XX
	void CDCComboXXInit();
	void CDCComboXXFrameMove();
	void CDCComboXXEventProcess();
	// CSI_CDC_COMBO_XZ
	void CDCComboXZInit();
	void CDCComboXZFrameMove();
	void CDCComboXZEventProcess();
	void CDCComboXZEnd();
	// CSI_CDC_COMBO_XZZ
	void CDCComboXZZInit();
	void CDCComboXZZFrameMove();
	void CDCComboXZZEventProcess();
	// CSI_CDC_COMBO_ZZX
	void CDCComboZZXEventProcess();
	// CSI_CDC_COMBO_ZZXX
	void CDCComboZZXXInit();
	void CDCComboZZXXFrameMove();
	void CDCComboZZXXEventProcess();
	void CDCComboZZXXEnd();
	// CSI_CDC_COMBO_ZZXXX
	void CDCComboZZXXXInit();
	void CDCComboZZXXXFrameMove();
	void CDCComboZZXXXEventProcess();
	//}}

	// oasis907 : ����� [2011.6.15] û 2�� ���� - ���̾� �ȶ��
	// CSI_CIP_COMBO_POWER_DOWN_LANDING_Z
	void CIPComboPowerDownLandingZInit();

	void CIPComboPowerDownLandingZEventProcess();
	void CIPComboPowerDownLandingZEnd();

	// CSI_CIP_COMBO_POWER_DOWN_LANDING_ZZ
	void CIPComboPowerDownLandingZZInit();

	void CIPComboPowerDownLandingZZEventProcess();
	void CIPComboPowerDownLandingZZEnd();

	// CSI_CIP_COMBO_DASH_XX
	void CIPComboDashXXInit();
	void CIPComboDashXXFrameMove();
	void CIPComboDashXXEventProcess();
	void CIPComboDashXXEnd();
	// CSI_CIP_COMBO_DASH_XXX,
	void CIPComboDashXXXInit();
	void CIPComboDashXXXFrameMove();
	void CIPComboDashXXXEventProcess();
	void CIPComboDashXXXEnd();
	// CSI_CIP_COMBO_DASH_XXXX
	void CIPComboDashXXXXInit();

	void CIPComboDashXXXXEventProcess();
	void CIPComboDashXXXXEnd();
	// CSI_CIP_COMBO_DASH_XXXZ
	void CIPComboDashXXXZInit();
	void CIPComboDashXXXZFrameMove();
	void CIPComboDashXXXZEventProcess();
	void CIPComboDashXXXZEnd();

	// ���������� �޺��� �ٴ� ����Ʈ ���� �Ѱ��� ���Ƴ���
	// ���� �� �Ǹ� ����ġ�� �� ����

	void ComboZFrameMove();
	void ComboZZFrameMove();
	void ComboZZZFrameMove();
	void ComboZZZZFrameMove();


	void ComboZZdownZZFrameMove();

	void ComboXFrameMove();


	void CFGComboDashZFrameMove();
	void CIPComboDashXXXXFrameMove();

	void PowerDownLandingFrameMove();
	void CIPComboPowerDownLandingZFrameMove();
	void CIPComboPowerDownLandingZZFrameMove();

	void ComboDashJumpZZLandingFrameMove();


	void StandUpAttackFrontFrameMove();
	void StandUpAttackBackFrameMove();
	void GuardAttackZFrameMove();



	void CHG_ComboXFrameMove();
	void CHG_ComboXEventProcess();
	void CHG_ComboXXFrameMove();
	void CHG_ComboXXEventProcess();

	void CHG_ComboDashZEventProcess();
	void CHG_ComboDashZZFrameMove();
	void CHG_ComboDashZZEventProcess();
	void CHG_ComboDashZZXFrameMove();
	void CHG_ComboDashZZXEventProcess();

	void CHG_ComboDashJumpZXFrameMove();
	void CHG_ComboDashJumpZXEventProcess();

	void CSI_SI_A_SIEGE_SHELLING_Ready_StateStart();
	void CSI_SI_A_SIEGE_SHELLING_Ready_FrameMove();
	void CSI_SI_A_SIEGE_SHELLING_Ready_EventProcess();
	void CSI_SI_A_SIEGE_SHELLING_ReadyLoop_FrameMove();
	void CSI_SI_A_SIEGE_SHELLING_ReadyLoop_EventProcess();
	void CSI_SI_A_SIEGE_SHELLING_X_FrameMove();
	void CSI_SI_A_SIEGE_SHELLING_X_EventProcess();
	void CSI_SI_A_SIEGE_SHELLING_X_Finish_FrameMove();
	void CSI_SI_A_SIEGE_SHELLING_X_Finish_EventProcess();
	void CSI_SI_A_SIEGE_SHELLING_Z_Finish_EventProcess();
	void CSI_P_CHG_CANNONEER_WITHSTANDING_FrameMove();
	void CSI_P_CHG_CANNONEER_WITHSTANDING_SIEGE_EventProcess();
	void CSI_P_CHG_CANNONEER_WITHSTANDING_WAIT_EventProcess();

	void CSI_A_CHG_ELASTIC_BOMB_FrameMove();
	void CSI_A_CHG_ELASTIC_BOMB_EventProcess();
	void CSI_A_CHG_MARK_OF_COMMANDER_Init();
	void CSI_A_CHG_MARK_OF_COMMANDER_FrameMove();
	void CSI_A_CHG_MARK_OF_COMMANDER_EventProcess();
	void CSI_SA_CHG_WONDER_WALL_FrameMove();
	void CSI_SA_CHG_WONDER_WALL_EventProcess();
	void CSI_SA_CHG_DREAD_CHASE_START_StateStart();
	void CSI_SA_CHG_DREAD_CHASE_START_FrameMove();
	void CSI_SA_CHG_DREAD_CHASE_START_EventProcess();
	void CSI_SA_CHG_DREAD_CHASE_LOOP_FrameMove();
	void CSI_SA_CHG_DREAD_CHASE_LOOP_EventProcess();
	void CSI_SA_CHG_DREAD_CHASE_END_FrameMove();
	void CSI_SA_CHG_DREAD_CHASE_END_EventProcess();
	void CSI_SA_CHG_BIGBANG_STREAM_FrameMove();
	void CSI_SA_CHG_BIGBANG_STREAM_EventProcess();
	void CSI_SA_CHG_CARPET_BOMBING_FrameMove();
	void CSI_SA_CHG_CARPET_BOMBING_EventProcess();
	void DoCarpetBombing();

	void CSI_SI_A_CHG_CHAOS_CANNON_START_FrameMove();
	void CSI_SI_A_CHG_CHAOS_CANNON_START_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_WAIT_FrameMove();
	void CSI_SI_A_CHG_CHAOS_CANNON_WAIT_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_WALK_FrameMove();
	void CSI_SI_A_CHG_CHAOS_CANNON_WALK_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_JUMP_READY_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_JUMP_UP_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_JUMP_DOWN_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG_FrameMove();
	void CSI_SI_A_CHG_CHAOS_CANNON_JUMP_LANDIG_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_END_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_GUARD_EventProcess();
	void CSI_SI_A_CHG_CHAOS_CANNON_ATTACK_EventProcess();

	// ���� ������ ������ ������ ���� ���� �ٵǸ� ����ġ�� ������  <-- �̰� ���� ����ġ�� ������?
	void ComboZZdownZFrameMove();
	void ComboXXFrameMove();
	void ComboDashJumpZZLandingCannonBallShotFrameMove();
	void CFGComboZZXFrameMove();

	/// CSI_CTT_COMBO_ZZX (��Ƽ�� Ʈ����)
	void CTT_ComboZZXFrameMove();
	void CTT_ComboZZXEventProcess();

	/// CSI_CTT_COMBO_ZZXX (��Ƽ�� Ʈ����)
	void CTT_ComboZZXXFrameMove();
	void CTT_ComboZZXXEventProcess();

	void CTT_DashComboXXFrameMove();
	void CTT_DashComboXXEventProcess();

	void CTT_MobileArmoredModeReadyStateStart();
	void CTT_MobileArmoredModeReadyFrameMove();
	void CTT_MobileArmoredModeReadyEventProcess();
	void CTT_MobileArmoredModeReadyStateEnd();

	void CTT_MobileArmoredModeLoopFowardStateStart();
	void CTT_MobileArmoredModeLoopFowardFrameMove();
	void CTT_MobileArmoredModeLoopFowardEventProcess();
	void CTT_MobileArmoredModeLoopFowardStateEnd();

	void CTT_MobileArmoredModeLoopStateStart();
	void CTT_MobileArmoredModeLoopFrameMove();
	void CTT_MobileArmoredModeLoopEventProcess();
	void CTT_MobileArmoredModeLoopStateEnd();

	void CTT_MobileArmoredModeLoopBackwardStateStart();
	void CTT_MobileArmoredModeLoopBackwardFrameMove();
	void CTT_MobileArmoredModeLoopBackwardEventProcess();
	void CTT_MobileArmoredModeLoopBackwardStateEnd();


	void CTT_MobileArmoredModeUnreadyStateStart();
	void CTT_MobileArmoredModeUnreadyFrameMove();
	void CTT_MobileArmoredModeUnreadyEventProcess();
	void CTT_MobileArmoredModeUnreadyStateEnd();

	void CTT_MobileArmoredModeZStateStartFuture();
	void CTT_MobileArmoredModeZStateStart();
	void CTT_MobileArmoredModeZFrameMove();
	void CTT_MobileArmoredModeZEventProcess();
	void CTT_MobileArmoredModeZStateEnd();

	void CTT_MobileArmoredModeXStateStartFuture();
	void CTT_MobileArmoredModeXStateStart();
	void CTT_MobileArmoredModeXFrameMove();
	void CTT_MobileArmoredModeXEventProcess();
	void CTT_MobileArmoredModeXStateEnd();


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// ���⼭ ���� ACTIVE, SPECIAL ACTIVE ��ų//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// IRON_CANNON ����� ��Ƽ��, ������̼�
	void CSI_SI_SA_CHUNG_DETONATION_START_StartFuture();
	void CSI_SI_SA_CHUNG_DETONATION_START_FrameMove();
	void CSI_SI_SA_CHUNG_DETONATION_START_EventProcess();

	void CSI_SI_SA_CHUNG_DETONATION_LOOP_Init();
	void CSI_SI_SA_CHUNG_DETONATION_LOOP_StartFuture();
	void CSI_SI_SA_CHUNG_DETONATION_LOOP_FrameMove();
	void CSI_SI_SA_CHUNG_DETONATION_LOOP_EventProcess();

	void CSI_SI_SA_CHUNG_DETONATION_END_Init();
	void CSI_SI_SA_CHUNG_DETONATION_END_StartFuture();
	void CSI_SI_SA_CHUNG_DETONATION_END_FrameMove();
	void CSI_SI_SA_CHUNG_DETONATION_END_EventProcess();

	// IRON_CANNON ����� ��Ƽ��, �糪ƽ ���ο�
	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_FrameMove();
	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_START_EventProcess();

	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_Init();
	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FrameMove();
	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_EventProcess();

	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_Init();
	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_FrameMove();
	void CSI_SI_SA_CHUNG_LUNATIC_BLOW_EXPLOSION_FINISH_EventProcess();

	// IRON_CANNON ��Ƽ��, ���ε�ĳ��
	void CSI_SI_A_CHUNG_RELOAD_CANNON_Init();
	void CSI_SI_A_CHUNG_RELOAD_CANNON_Start();
	void CSI_SI_A_CHUNG_RELOAD_CANNON_FrameMove();
	void CSI_SI_A_CHUNG_RELOAD_CANNON_EventProcess();


	//////////////////////////////////////////////
	//{{ ���� : 2010-12-15 : Gigantic impact(specialactive)
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_Init();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateStartFuture();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_FrameMove();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_FrameMoveFuture();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_EventProcess();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateEnd();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_START_StateEndFuture();

	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_Init();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_FrameMove();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_FrameMoveFuture();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_EventProcess();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_AIRFLY_StateEnd();

	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_Init();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_FrameMove();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_EventProcess();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_Hyper_EventProcess();
	void CSI_SI_SA_CHUNG_GIGANTIC_IMPACT_LANDING_StateEnd();
	//}} ���� : 2010-12-15 : Gigantic impact(specialactive)
	///////////////////////////////////////////

	// IRON_CANNON ����� ��Ƽ��, ���Ż ����
	void CSI_SI_SA_CHUNG_BRUTAL_SWING_Init();
	void CSI_SI_SA_CHUNG_BRUTAL_SWING_StartFuture();
	void CSI_SI_SA_CHUNG_BRUTAL_SWING_FrameMove();
	void CSI_SI_SA_CHUNG_BRUTAL_SWING_EventProcess();

	// IRON_CANNON ����� ��Ƽ��, ���ɾ� ü�̽�
	void CSI_SI_SA_CHUNG_SCARE_CHASE_Init();
	void CSI_SI_SA_CHUNG_SCARE_CHASE_FrameMove();
	void CSI_SI_SA_CHUNG_SCARE_CHASE_EventProcess();

	// ���̹� ��
	void CSI_SI_A_CHUNG_AIMMING_SHOT_Init();
	void CSI_SI_A_CHUNG_AIMMING_SHOT_StateStart();
	void CSI_SI_A_CHUNG_AIMMING_SHOT_FrameMove();
	void CSI_SI_A_CHUNG_AIMMING_SHOT_EventProcess();
	void CSI_SI_A_CHUNG_AIMMING_SHOT_StateEnd();

	//{{ oasis907 : ����� [2010.12.18] // �ưִٸ�
	void CSI_SI_SA_CHUNG_ACELDAMA_Init();
	void CSI_SI_SA_CHUNG_ACELDAMA_StateStart();
	void CSI_SI_SA_CHUNG_ACELDAMA_FrameMove();
	void CSI_SI_SA_CHUNG_ACELDAMA_EventProcess();
	void CSI_SI_SA_CHUNG_ACELDAMA_StateEnd();
	//}} oasis907 : ����� [2010.12.18] // �ưִٸ�

//{{ kimhc // 2011.1.8 // û 1�� ����
	// ��ƿ���� (ǻ�������)
	void CSI_SI_SA_CFG_STEEL_EDGE_Init();
	void CSI_SI_SA_CFG_STEEL_EDGE_FrameMove();
	void CSI_SI_SA_CFG_STEEL_EDGE_EventProcess();

	// ���̾� �Ͽ︵ (ǻ�������)
	void CSI_SI_SA_CFG_IRON_HOWLING_Init();
	void CSI_SI_SA_CFG_IRON_HOWLING_FrameMove();
	void CSI_SI_SA_CFG_IRON_HOWLING_EventProcess();

	// �糪ƽ ǻ�� (ǻ�������)
	void CSI_SI_SA_CFG_LUNATIC_FURY_READY_Init();
	void CSI_SI_SA_CFG_LUNATIC_FURY_READY_FrameMove();
	void CSI_SI_SA_CFG_LUNATIC_FURY_READY_EventProcess();

	void CSI_SI_SA_CFG_LUNATIC_FURY_RUN_FrameMove();
	void CSI_SI_SA_CFG_LUNATIC_FURY_RUN_EventProcess();

	void CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_FrameMove();
	void CSI_SI_SA_CFG_LUNATIC_FURY_ATTACK_EventProcess();

	void CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_FrameMove();
	void CSI_SI_SA_CFG_LUNATIC_FURY_BOMBING_EventProcess();

	void CSI_SI_SA_CFG_LUNATIC_FURY_END_FrameMove();
	void CSI_SI_SA_CFG_LUNATIC_FURY_END_EventProcess();

	// �������� (ǻ�������)
	void CSI_SI_A_CFG_LEAP_ATTACK_READY_Init();
	void CSI_SI_A_CFG_LEAP_ATTACK_READY_FrameMove();
	void CSI_SI_A_CFG_LEAP_ATTACK_READY_EventProcess();

	void CSI_SI_A_CFG_LEAP_ATTACK_JUMP_Start();
	void CSI_SI_A_CFG_LEAP_ATTACK_JUMP_FrameMove();
	void CSI_SI_A_CFG_LEAP_ATTACK_JUMP_EventProcess();

	void CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_Start();
	void CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_FrameMove();
	void CSI_SI_A_CFG_LEAP_ATTACK_JUMP_DOWN_EventProcess();

	void CSI_SI_A_CFG_LEAP_ATTACK_LANDING_Start();
	void CSI_SI_A_CFG_LEAP_ATTACK_LANDING_FrameMove();
	void CSI_SI_A_CFG_LEAP_ATTACK_LANDING_EventProcess();

	// ���� (ǻ�������)
	void CSI_SI_A_CFG_GUARD_READY_Init();
	void CSI_SI_A_CFG_GUARD_READY_StartFuture();
	void CSI_SI_A_CFG_GUARD_READY_FrameMove();
	void CSI_SI_A_CFG_GUARD_READY_EventProcess();

	void CSI_SI_A_CFG_GUARD_WAIT_Start();
	void CSI_SI_A_CFG_GUARD_WAIT_FrameMove();
	void CSI_SI_A_CFG_GUARD_WAIT_EventProcess();
	void CSI_SI_A_CFG_GUARD_WAIT_End();

	void CSI_SI_A_CFG_GUARD_END_FrameMove();
	void CSI_SI_A_CFG_GUARD_END_EventProcess();
	void CSI_SI_A_CFG_GUARD_END_End();

	void CSI_SI_A_CFG_GUARD_DAMAGE_Start();
	void CSI_SI_A_CFG_GUARD_DAMAGE_FrameMove();
	void CSI_SI_A_CFG_GUARD_DAMAGE_EventProcess();
	void CSI_SI_A_CFG_GUARD_DAMAGE_End();

	// �������� (���ð����)
	void CSI_SI_SA_CSG_DUAL_BUSTER_Init();
	void CSI_SI_SA_CSG_DUAL_BUSTER_FrameMove();
	void CSI_SI_SA_CSG_DUAL_BUSTER_EventProcess();

	void CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_FrameMove();
	void CSI_SI_SA_CSG_DUAL_BUSTER_LANDING_EventProcess();

	// ����ϰ� (���ð����)
	void CSI_SI_SA_CSG_HEAVY_RAILGUN_Init();
	void CSI_SI_SA_CSG_HEAVY_RAILGUN_FrameMove();
	void CSI_SI_SA_CSG_HEAVY_RAILGUN_EventProcess();

	// ���ý�Ÿ (���ð����)
	void CSI_SI_SA_CSG_SHOOTING_STAR_READY_Init();
	void CSI_SI_SA_CSG_SHOOTING_STAR_READY_FrameMove();
	void CSI_SI_SA_CSG_SHOOTING_STAR_READY_EventProcess();

	void CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_Start();
	void CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_FrameMove();
	void CSI_SI_SA_CSG_SHOOTING_STAR_ATTACK_EventProcess();

	void CSI_SI_SA_CSG_SHOOTING_STAR_END_EventProcess();

	// ������(���ð����)
	//{{ ���� : 2011-01-18 : ��Ƽ�� ��ų - ������/��弦
	void CSI_SI_A_CSG_RUMBLE_SHOT_Init();
	void CSI_SI_A_CSG_RUMBLE_SHOT_FrameMove();
	void CSI_SI_A_CSG_RUMBLE_SHOT_EventProcess();

	void CSI_SI_A_CSG_HEAD_SHOT_Init();
	void CSI_SI_A_CSG_HEAD_SHOT_FrameMove();
	void CSI_SI_A_CSG_HEAD_SHOT_EventProcess();
	//}} ���� : 2011-01-18 : ��Ƽ�� ��ų - ������/��弦
//}} kimhc // 2011.1.8 // û 1�� ����

//{{ kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų
	// ī��ũ����(���ð����)
	void CSI_SI_SA_CSG_COMET_CRASHER_READY_Init();
	void CSI_SI_SA_CSG_COMET_CRASHER_READY_FrameMove();
	void CSI_SI_SA_CSG_COMET_CRASHER_READY_EventProcess();

	void CSI_SI_SA_CSG_COMET_CRASHER_LOOP_FrameMove();
	void CSI_SI_SA_CSG_COMET_CRASHER_LOOP_EventProcess();

	void CSI_SI_SA_CSG_COMET_CRASHER_FINISH_FrameMove();
	void CSI_SI_SA_CSG_COMET_CRASHER_FINISH_EventProcess();

	// �������ͽŵ��(���ð����)
	void CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_Init();
	void CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_FrameMove();
	void CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_FrameMoveFuture();
	void CSI_SI_SA_CSG_SHARPSHOOTER_SYNDROME_EventProcess();

	// �ǵ���Ͽ�(ǻ�������)
	void CSI_SI_SA_CFG_PANDEMONIUM_Init();
	void CSI_SI_SA_CFG_PANDEMONIUM_FrameMove();
	void CSI_SI_SA_CFG_PANDEMONIUM_EventProcess();

	// Į�󺼱�����(ǻ�������)
	void CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_Init();
	void CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_FrameMove();
	void CSI_SI_SA_CFG_PAIN_OF_CALADBOLG_EventProcess();
//}} kimhc // 2011-3-16 // 2011-04 �� ��ġ�� û ĳ�� ��ų

	// oasis907 : ����� [2011.6.16] û 2�� ���� - ���̾� �ȶ��
	void CSI_SI_A_CIP_SUDDEN_BURSTER_Init();
	void CSI_SI_A_CIP_SUDDEN_BURSTER_FrameMoveFuture();
	void CSI_SI_A_CIP_SUDDEN_BURSTER_FrameMove();
	void CSI_SI_A_CIP_SUDDEN_BURSTER_EventProcess();

	void CSI_SI_SA_CIP_BURST_WOLF_Init();
	void CSI_SI_SA_CIP_BURST_WOLF_StateStart();
	void CSI_SI_SA_CIP_BURST_WOLF_FrameMove();
	void CSI_SI_SA_CIP_BURST_WOLF_EventProcess();
	void CSI_SI_SA_CIP_BURST_WOLF_StateEnd();


	void CSI_SI_SA_CIP_LAND_DEMOLISHER_START_StartFuture();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_START_FrameMoveFuture();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_START_FrameMove();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_START_EventProcess();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_START_EndFuture();

	void CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_StartFuture();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_FrameMoveFuture();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_FrameMove();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_EventProcess();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_DOWN_EndFuture();

	void CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_Init();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_StateStart();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_FrameMove();
	void CSI_SI_SA_CIP_LAND_DEMOLISHER_LANDING_EventProcess();






	//{{ JHKang / ������ / 2011.6.19 / ���� ��
	void CSI_SI_A_CDC_LEG_SHOT_Init();
	void CSI_SI_A_CDC_LEG_SHOT_Start();
	void CSI_SI_A_CDC_LEG_SHOT_FrameMove();
	void CSI_SI_A_CDC_LEG_SHOT_EventProcess();
	//}}
	//{{ JHKang / ������ / 2011.6.20 / ��ƿ���� ��Ʈ����ũ
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Init();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_Start();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_FrameMove();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_READY_EventProcess();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_StateEnd();

	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_EventProcess();

	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_IDLE_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_STRIKE_StateEnd();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_LEFT_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_RIGHT_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_LEFT_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_UP_RIGHT_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_LEFT_StateStart();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CONTROL_DOWN_RIGHT_StateStart();

	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_Start();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_FrameMove();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_FIRE_EventProcess();

	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_Start();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_CANCEL_EventProcess();

	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_END_EventProcess();

	void ArtilleryStrikeControl_EventProcess();
	//}}

	//{{ JHKang / ������ / 2011.7.1 / �ҷ� ����ġ
	void CSI_SI_SA_CDC_BULLET_BLITZ_Init();
	void CSI_SI_SA_CDC_BULLET_BLITZ_StateStart();
	void CSI_SI_SA_CDC_BULLET_BLITZ_FrameMove();
	void CSI_SI_SA_CDC_BULLET_BLITZ_EventProcess();
	void CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_StateStart();
	void CSI_SI_SA_CDC_BULLET_BLITZ_HYPER_FrameMove();
	//}}

	void CSI_SI_SA_CTT_CROSS_FIRE_Init();
	void CSI_SI_SA_CTT_CROSS_FIRE_StartFuture();
	void CSI_SI_SA_CTT_CROSS_FIRE_FrameMove();
	void CSI_SI_SA_CTT_CROSS_FIRE_EventProcess();

	// ����Ʈ ������̼�
	void CSI_SA_CHUNG_IMPACT_DETONATION_READY_StartFuture();
	void CSI_SA_CHUNG_IMPACT_DETONATION_READY_EventProcess();

	void CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_Init();
	void CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_StartFuture();
	void CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_FrameMove();
	void CSI_SA_CHUNG_IMPACT_DETONATION_LOOP_EventProcess();

	void CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_StartFuture();
	void CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_FrameMove();
	void CSI_SA_CHUNG_IMPACT_DETONATION_FINAL_EventProcess();

	// ��ƿ���� ���
	void CSI_A_CIP_ARTILLERY_NOVA_CHARGE_Init();
	void CSI_A_CIP_ARTILLERY_NOVA_CHARGE_StartFuture();
	void CSI_A_CIP_ARTILLERY_NOVA_CHARGE_FrameMove();
	void CSI_A_CIP_ARTILLERY_NOVA_CHARGE_EventProcess();

	void CSI_A_CIP_ARTILLERY_NOVA_FIRE_FrameMove();
	void CSI_A_CIP_ARTILLERY_NOVA_FIRE_EventProcess();

	// �ǵ���Ͽ� �Ǿ�
	void CSI_SI_SA_CFG_PANDEMONIUM_FEAR_Init();
	void CSI_SI_SA_CFG_PANDEMONIUM_FEAR_FrameMove();
	void CSI_SI_SA_CFG_PANDEMONIUM_FEAR_EventProcess();


	// ���� ������� - � ����ũ
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_StartFuture();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_FrameMoveFuture();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_FrameMove();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_EventProcess();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_START_EndFuture();

	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN_StartFuture();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_DOWN_EventProcess();

	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_Init();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_FrameMove();
	void CSI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE_LANDING_EventProcess();

	// ���� ���̾�
	void CSI_A_CSG_DOUBLE_FIRE_Init();
	void CSI_A_CSG_DOUBLE_FIRE_FrameMove();
	void CSI_A_CSG_DOUBLE_FIRE_EventProcess();

	// �Ϸ���ƽ �� - �׸����̵� ��ó
	void CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_Init ();
	void CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_FrameMove();
	void CSI_A_CHG_ELASTIC_BOMB_LAUNCHER_EventProcess();



	// �ű׳� ��
	void CSI_A_CSG_MAGNUM_SHOT_Init ();
	void CSI_A_CSG_MAGNUM_SHOT_FrameMove ();
	void CSI_A_CSG_MAGNUM_SHOT_EventProcess ();

	// ���ǵ� ĳ��
	void CSI_A_CDC_RAPID_CANNON_READY_StartFuture();
	void CSI_A_CDC_RAPID_CANNON_READY_Start();
	void CSI_A_CDC_RAPID_CANNON_READY_EventProcess();

	void CSI_A_CDC_RAPID_CANNON_LOOP_Init();
	void CSI_A_CDC_RAPID_CANNON_LOOP_StartFuture();
	void CSI_A_CDC_RAPID_CANNON_LOOP_FrameMove();
	void CSI_A_CDC_RAPID_CANNON_LOOP_EventProcess();

	void CSI_A_CDC_RAPID_CANNON_FINAL_StartFuture();
	void CSI_A_CDC_RAPID_CANNON_FINAL_FrameMove();
	void CSI_A_CDC_RAPID_CANNON_FINAL_EventProcess();


	/// ��ƿ���� ��Ʈ����ũ ���� �߸���Ÿ, Start �� ����
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Init();
	void CSI_SI_SA_CDC_ARTILLERY_STRIKE_QUANTUM_BALLISTA_READY_Start();

	/// ��Ƽ�� �ʵ� ���̵�
	void CSI_SA_CTT_TACTICAL_FIELD_RAID_Init();
	void CSI_SA_CTT_TACTICAL_FIELD_RAID_FrameMove();
	void CSI_SA_CTT_TACTICAL_FIELD_RAID_EventProcess();

	/// ���� ����
	void CSI_A_CTT_BOMBARD_SERVICE_Init ();
	void CSI_A_CTT_BOMBARD_SERVICE_FrameMove ();
	void CSI_A_CTT_BOMBARD_SERVICE_EventProcess ();


	// ��ƿ���� ��Ʈ����ũ ����, ���� Anim ���� ī�� �ٹְ� ���� Style �� ����
	void DoArtilleryStrikeFire();		// ��ƿ���� ��Ʈ����ũ �̻��� �߻�!
	void ResetArtilleryStrikeData ();	// ArtilleryStike ���� �����͸� �ʱ�ȭ ��Ŵ
										// ��Ÿ��, ���� �ð� ���� ��ų ���ø����� �����Ƿ�
										// ���� ���Ǵ� �ڵ常 �ʱ�ȭ
	// ��ƿ���� ��Ʈ����ũ ������ ����
	void SetArtilleryStrikeData ( CHUNG_ARTILLERY_STIKE_TYPE eType_ );
	//{{ kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ
	//virtual void RenderName();
	UINT m_uiCountToCallSetDamageReducePercentFuncInCommonStateStart;
	UINT m_uiCountToCallSetDamageReducePercentFuncInCommonStateEnd;
	wstring m_wstrFunctionName;
	bool m_bVerifyGuardSystem;
	//}} kimhc // 2011-04-13 // û ���� ���� �׽�Ʈ

	/// �ڰ���(��Ƽ�� Ŀ�Ǵ�)
	void CSI_A_CTT_REMOTE_MORTAR_Init();
	void CSI_A_CTT_REMOTE_MORTAR_StateStart();
	void CSI_A_CTT_REMOTE_MORTAR_FrameMove();
	void CSI_A_CTT_REMOTE_MORTAR_EventProcess();

	/// ũ�ν� ���̾�(��Ƽ�� Ŀ�Ǵ�)

	/// ��Ƽ�� �ʵ�(��Ƽ�� Ŀ�Ǵ�)
	void CSI_SA_CTT_TACTICAL_FIELD_Init();
	void CSI_SA_CTT_TACTICAL_FIELD_StateStart();
	void CSI_SA_CTT_TACTICAL_FIELD_FrameMove();
	void CSI_SA_CTT_TACTICAL_FIELD_EventProcess();


//#pragma region SI_FS_CIP_DOOM_STRIKER
	void CSI_HA_CIP_DOOM_STRIKER_Init();
//#pragma endregion �� ��Ʈ����Ŀ - �ñر�

//#pragma region SI_FS_CDC_OUTRAGE_STRIKE
	void CSI_HA_CDC_OUTRAGE_STRIKE_Init();
//#pragma endregion �ƿ� ������ ��Ʈ��ũ - �ñر�

	// ��Ʋ����Ʈ ����
	void CSI_HA_CTT_SATELITE_RAIN_Init ();
	void CSI_HA_CTT_SATELITE_RAIN_FrameMove();
	void CSI_HA_CTT_SATELITE_RAIN_EventProcess();

	void DoSateliteRain ();		// ��Ʋ����Ʈ ���� ���� �Լ�
	void ResetSateliteRain ();	// ��Ʋ����Ʈ ���� ���� ������ �ʱ�ȭ
	void SateliteRainFrameMove ();	// ��Ʋ����Ʈ ����, CommonFrameMove



	virtual void ApplyRenderParam( CKTDGXRenderer::RenderParam* pRenderParam_ );

	void ChaosCannonAttackStateChange ( int iOffset_ );

	virtual float GetActualCoolTime( IN const CX2SkillTree::SkillTemplet* pSkillTemplet_, IN int iSkillLevel ) const;

protected:

	CKTDGParticleSystem::CParticleEventSequenceHandle		m_ahChungMajorParticleInstance[CHUNG_MAJOR_PII_END];
	CKTDGParticleSystem::CParticleEventSequenceHandle		m_ahChungMinorParticleInstance[CHUNG_MINOR_PII_END];
	///////////////////////////////////////////////////////
	//{{ ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	CKTDGXMeshPlayer::CXMeshInstanceHandle					m_ahChungMajorMeshInstance[CHUNG_MAJOR_MII_END];
	//}} ���� : 2011-01-15 : û�� ���� ������ �޽� �ν��Ͻ�
	///////////////////////////////////////////////////////

	CGuardSystem			m_GuardSystem;			/// ���� �ý��ۿ� ���� ����
	CCannonBallSystem		m_CannonBallSystem;		/// ĳ���� �ý��ۿ� ���� ����

	vector<GUARD_STATE_INFO> m_vecStateIDCanBeChangedToGuardState;	/// ���� ������Ʈ�� �Ѿ �� �ִ� ������Ʈ ID ����Ʈ

	vector<CX2EqipPtr> m_vecPromotionEquipList;		/// ���� ��� ����Ʈ�� ���� ���� 3���� �Ҹ��� ������ ������ ��������.
	vector<int> m_vecPromotionItemIDList;			/// ���θ�� ����� ID ����Ʈ
	bool		m_bBerserkMode;						/// �ִ� ������ ���� ������ ������ ������ �����ΰ�?(����ȭ)
	const int	MAX_ANIMATION_LOOP_COUNT_AT_THIS_STATE;	/// ���� ���¿��� �ִ�� Loop �� ���ִ� �ִϸ��̼� Ƚ��
	UCHAR		m_ucCannonBallCountToBeUpUsingReLoadCannon;	/// ���ε� ĳ���� ���ؼ� ������ ĳ������ ���� (���ε�ĳ���� �⺻ ��ų�̹Ƿ� ��� ������ ����)
	float		m_fDamageRelByCannonBallMastery;	/// ���ε� �����͸��� ���ؼ� ������ ���ε�ĳ���� ������ ����
	UCHAR		m_ucBasicCannonBallCount;			/// �⺻ ĳ���� ����

	bool		m_bAimingTarget;
	D3DXVECTOR3 m_vAimingTargetPos;
	D3DXVECTOR3 m_vAimingTargetLandPos;

	//{{ kimhc // 2011.1.11 // û 1�� ����
	CFuryGuardianGuardSkillPtr	m_GuardForFuryGuardianPtr;		/// û 1�� ����, ǻ�� ������� Guard ��ų
	float						m_fForceDownRelAtMeleeAttack;	/// ���� ������ ��� �нú� ��ų� ���� ForceDown ��ġ ����
	float						m_fProbabilityBigDamageAtGuardMastery;		/// ���帶���͸����� BigDamage�� �� Ȯ��
	float						m_fIncreaseRangeSubWeaponShot;	/// û 1�� ����, ���� ������� ������� ��Ÿ� ������(�������� �нú�)
	float						m_fCriticalDamageRate;			/// û 1�� ����, ���� ������� ���������� ���� Critical �� ������ ����
	//}} kimhc // 2011.1.11 // û 1�� ����

	int							m_iCountbyCsgComboDashJumpXXMiniGun;

	float						m_fProbabilityIronWill;
	bool						m_bApplyLandDemolisher;
	CX2EffectSet::Handle		m_hLandDemolisherAfterEffect;
	float						m_fLandDemolisherPowerRate;
	float						m_fRemainIronWillRenderEffectTime;
	int							m_iDebuffTransitNum;


	CKTDXTimer	m_TimerCheckInvalidShot;		/// ��ƿ���� ��Ʈ����ũ Ÿ�̸�
	//CX2GUNPC*	m_pShot;						/// ���� Ŀ�� NPC
	int			m_iShot;						/// ���� Ŀ�� NPC UID
	bool		m_bIsSummoningShot;				/// ��ȯ Ȯ��
	CKTDXTimer	m_TimerShotControl;				/// Shot Ÿ�̸�
	short		m_iArtilleryStrikePlayCount;	/// ���� ����
	D3DXVECTOR3	m_vLastShotPos;					/// ������ ��ġ
	D3DXVECTOR3 m_vLastShotDir;					/// ������ ���� ����

	int			m_sBulletCount;					/// �ҷ� ������ źȯ ����

	float		m_fDecreaseRate;				/// ������ ��Ÿ�� ������(MP, ��Ÿ��)
	CKTDXTimer	m_timerSummoned;				/// ��ȯ�� ���� �ð�
	float		m_fSummonedLifeTime;			/// ��ȯ�� ���͵� ����ִ� �ð�

	bool		m_bIncreaseCritical;			/// ���� ������ ���� ũ��Ƽ�� ���� ����

	int			m_iSiegeShellingCount;
	float		m_fSiegeShellingDistance;
	float		m_fSiegeShellingDamageRel;
	float		m_fSiegeShellingUserBallRate;
	float		m_fInsertCannonBallRel;
	float		m_fBeShotCannonBallRel;
	float		m_fBeShotCannonBallRelTime;
	float		m_fWithStandingRate;
	float		m_fWithStandingPowerRate;
	int			m_iChargeCannonBall;
	int			m_iDreadChaseCount;
	int			m_iCarpetBombingCount;
	int			m_iCarpetBombingSmallCount;
	float		m_fCarpetBombingTime;
	bool		m_bCarpetBombing;
	D3DXVECTOR3	m_vCarpetBombingPos;
	D3DXVECTOR3 m_vCarpetBombingDir;
	D3DXVECTOR3 m_vCarpetBombingRot;
	float		m_fChaosCannonTime;
	CX2EffectSet::Handle		m_hChaosCannonEffect;
	CKTDXDeviceSound	*m_pChaosCannonSound;

	bool		m_bMobileArmoredModeEnable;
	bool		m_bMobileArmoredModeXFired;
	bool		m_bInStateMobileArmoredMode;
	int			m_iExtraCannonBallReloadCount;

	float		m_fMobileArmoredModeDistance;
	float		m_fMobileArmoredModeDamageRel;

	float		m_fMobileArmoredModeStepDustElapsedTime;
	float		m_fMobileArmoredModeFireXCoolTime;
	float		m_fMobileArmoredModeFireZCoolTime;
	CX2EffectSet::Handle	m_hMobileArmoredModeEffect;

	UidType		m_iRemoteMortarNPCUID;			/// ��ġ�� �ڰ��� ���̵�

	float		m_fCannonBallExtraChargeRate;	/// ĳ���� �߰� ���� Ȯ��
	float		m_fMiniGunHeadShotRate;			/// ���� ���ݿ� ��弦 �ߵ� Ȯ��


	CHUNG_ARTILLERY_STIKE_TYPE		m_eArtilleryStrikeType;		// ��ƿ���� ��Ʈ����ũ ����, ����ϰڴ°�? Ÿ����?
	float		m_fRateGasPistonSystemAutoRecharge;				// ���� �ǽ��� �ý���, ĳ���� ��� �� �ڵ� ���� Ȯ��
	int			m_iArtilleryStrikeNowPlayCount;					// ��ƿ���� ��Ʈ����ũ, ���� �°?
	CKTDXTimer	m_TimerCheckArtilleryStrikeFire;				// ��ƿ���� ��Ʈ����ũ �߻� Ÿ�̸�
	bool		m_bArtilleryStrikeFire;							// ��ƿ���� ��Ʈ����ũ �߻�?
	float		m_fArtilleryStrikePowerRate;					// ��ƿ���� ��Ʈ����ũ, ��� ���� 1�� �� ������ ����Ʈ�� ������ PowerRate �� �޾ƿ��� ��������
																// PowerRate �� ���� �����صд�.
	int			m_iBombardServiceTarget;						// ���� ���� Ÿ�� UID
	CX2EffectSet::Handle	 m_hBombardServiceEffect;			// ���� ���� ����� ����Ʈ
	int			m_iFireCount;									// �߻� Ƚ��
	float		m_fChargeTime;									// ���� �ð�
	float		m_fPassiveCriticalRateChung;					// ũ��Ƽ�� ������

	CX2EffectSet::Handle			m_hSateliteRainBigMissileSummonEffect;		// ��Ʋ����Ʈ ���� ����Ʈ
	CKTDXTimer						m_TimerCheckSateliteRain;	// ��Ʋ����Ʈ ���� ����Ʈ Ÿ�̸�
	bool							m_bSateliteRainFire;		// ��Ʋ����Ʈ ���� �߻�?
	float							m_fSateliteRainPowerRate;	// ��Ʋ����Ʈ ���� ��� ���� 1�� �� ������ ����Ʈ�� ������ PowerRate �� �޾ƿ��� ��������
																// PowerRate �� ���� �����صд�.
    CX2DamageEffect::CEffectHandle   		m_hCEffectSateliteRain;		// ��Ʋ����Ʈ ���� ����Ʈ
	double							m_fSateliteRainPhase;		// ��Ʋ����Ʈ ���� ���� ���� ����, 1�� ���� �ѹ���
	bool							m_bIsFireSateliteRainBigMissile; // ��Ʋ����Ʈ ����, ū �̻��� �߻� �ߴ°�?

	D3DXVECTOR3						m_vSateliteRainSummonMagicZonePosition;		// ��Ʋ����Ʈ ����, ū �̻��� �߻��ϱ� �� ������ ��ǥ
	D3DXVECTOR3						m_vSateliteRainSummonMagicZoneRotate;		// ��Ʋ����Ʈ ����, ������ ȸ����

	float		m_fArtilleryStrikeCreateDamageEffectGapTime;				// ��ƿ���� ��Ʈ����ũ �̽� ������ ������ ����Ʈ ���� �ð� Gap, �̽� ���� ���� �������� ���� �߰���
	float		m_fSiegeShellingIncraseMPRel;									// ź�� ���� �нú�, MP ������ �߰�


};

IMPLEMENT_INTRUSIVE_PTR(  CX2GUChung::CFuryGuardianGuardSkill );

//}} kimhc // 2010.11.18 //  2010-12-23 New Character CHUNG
