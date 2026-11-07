#pragma once

class CX2GameUnit;
typedef KObserverPtr<CX2GameUnit>   CX2GameUnitoPtr;

class CX2GUUser;



struct  CX2DamageEffect_CEffect_TAG {};
typedef KHandleType<int,CX2DamageEffect_CEffect_TAG>     CX2DamageEffect_CEffectHandle;
#define INVALID_DAMAGE_EFFECT_HANDLE    (CX2DamageEffect_CEffectHandle::invalid_handle())




class CX2DamageManager
{
	public:
		/// Ÿ�ݽ� Ʈ���� ID
		enum DAMAGE_TRIGGER_TYPE
		{
			DTT_NONE	= 0,

			DTT_RAVEN_RAGE_ATTACK,
			DTT_EVE_THOUSAND_STAR,
			DTT_MARK_OF_COMMANDER,

			// ���� �Ĺ� ������, ��ī�ο� �������� �Ϻ� ��Ƽ�� ��ų MP ȸ���� ������ �����ִ� Trigger Type
			DTT_SHARP_KICK_RELATE_ACTIVE_SKILL_INCREASE_MP_RATE,

			DTT_END,
		};

		/// DAMAGE ����
		enum DAMAGE_TYPE
		{
			DT_PHYSIC = 0,			/// ���� Ÿ��
			DT_MAGIC,				/// ���� Ÿ��
			DT_MIX,					/// ����,���� ����

			DT_WEAPON_PHYSIC,		/// ������ �������ݷ�(��������)
			DT_WEAPON_MAGIC,		/// ������ �������ݷ�(��������)
			DT_WEAPON_MIX,			/// ������ ����, ���� ���ݷ�(��������)

			DT_BUFF,				/// ������. �� ����Ÿ���� ��*Ÿ�ݽ� ���� + ����ġ, ��*Ÿ�ݽ� ���� + ����ġ, ��� ��ų ���� ������ ���� �ʴ´�
		};

		/// ���� ����
		enum ATTACK_TYPE
		{
			AT_NORMAL = 0,
			AT_SPECIAL,
		};

		/// Ÿ�� ����
		enum HIT_TYPE
		{
			HT_NO_SOUND = -1,	/// ����
			HT_DEFAULT = 0,		/// �⺻
			HT_SWORD_SLASH,		/// Į ����
			HT_SWORD_SLASH2,	/// Į ����2
			HT_SWORD_HIT,		/// Į Ÿ��
			HT_SWORD_CRASH,		/// Į �浹

			HT_ROD_SLASH,		/// ������ ����
			HT_ROD_SLASH2,		/// ������ ����2
			HT_ROD_HIT,			/// ������ Ÿ��
			HT_ROD_CRASH,		/// ������ �浹

			HT_PUNCH_HIT,		/// �ָ� ġ��
			HT_PUNCH_HIT2,		/// �ָ� ġ��2
			HT_PUNCH_CRASH,		/// �ָ� �浹

			HT_KICK_SLASH,		/// �� ����
			HT_KICK_SLASH2,
			HT_KICK_HIT,
			HT_KICK_CRASH,

			HT_ARROW_HIT,
			HT_ARROW_HIT2,
			HT_ARROW_CRASH,

			HT_FIRE,
			HT_FIRE2,
			HT_BOMB,
			HT_WOOD,

			HT_LIGHTNING,

			HT_NASOD_KING,

			HT_METAL_PUNCH_HIT,

            HT_GROUND_HIT,
			HT_SLAP,				// �̺� �� ������
			HT_EVE_SLASH1,
			HT_EVE_SLASH2,

//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
//#ifdef	NEW_CHARACTER_CHUNG
			HT_CHUNG_SLASH1,
			HT_CHUNG_SLASH2,
			HT_CHUNG_SLASH3,
//#endif	NEW_CHARACTER_CHUNG
//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

//{{ kimhc // 2011.1.21 // û 1�� ����
//#ifdef	CHUNG_FIRST_CLASS_CHANGE
			HT_HANDGUN_HIT1,
			HT_HANDGUN_HIT2,
//#endif	CHUNG_FIRST_CLASS_CHANGE
//}} kimhc // 2011.1.21 // û 1�� ����

			HT_AREA_HIT,
			HT_SPEAR_HIT,


//#pragma region �ƶ� â ����� ����
			HT_POLE_BRANDISH_WEAK,		/// â ���� ����
			HT_POLE_BRANDISH_STRONG,	/// â ���� ����
			HT_SHORTSWORD_SLASH,		/// �ܰ� ������
			HT_SHORTSWORD_PIERCE,		/// �ܰ� ���
//#pragma endregion

			HT_ELECTRIC_1,				/// ������Ʈ 1
			HT_ELECTRIC_2,				/// ������Ʈ 2
			HT_PLASMA_1,				/// �ö����Ʈ 1
			HT_PLASMA_2,				/// �ö����Ʈ 2
			HT_PRESSURE_1,				/// ����/�������Ʈ 1
			HT_PRESSURE_2,				/// ����/�������Ʈ 2
			HT_WATER_1,					/// ��ü��Ʈ 1
			HT_WATER_2,					/// ��ü��Ʈ 2
			HT_WATER_3,					/// ��ü��Ʈ 3
			HT_ICE,						/// ������Ʈ
			HT_METAL,					/// ��������Ʈ
			HT_WIND,					/// �ٶ���Ʈ
			HT_STONE,					/// ������ ��Ʈ

			HIT_TYPE_COUNT,
		};

		//�ǰ� ���� - ���� ����
		enum HITTED_TYPE
		{
			HTD_NO_SOUND = -1,
			HTD_DEFAULT	= 0,
			HTD_MEAT,
			HTD_WOOD,
			HTD_STONE,
			HTD_STONE2,
			HTD_METAL,
			HTD_METAL2,
			HTD_BEEHOUSE,
//{{ kimhc // 2011.1.21 // û 1�� ����
//#ifdef	CHUNG_FIRST_CLASS_CHANGE
			HTD_GUARD_1,
//#endif	CHUNG_FIRST_CLASS_CHANGE
//}} kimhc // 2011.1.21 // û 1�� ����
//#ifdef UPGRADE_SKILL_SYSTEM_2013 //��â��
			HTD_GUARD_2,
//#endif //UPGRADE_SKILL_SYSTEM_2013
			HITTED_TYPE_COUNT,
		};

		/// ���� ���� ����
		enum REACT_TYPE
		{
			RT_NO_DAMAGE = 0,			/// �����̰ų� ��� �ƹ��� ���׼ǵ� ���� ����
			RT_SMALL_DAMAGE,			/// ���� ������
			RT_BIG_DAMAGE,				/// ū ������
			RT_DOWN,					/// ��������
			RT_UP,						/// ���� �Ŀø���
			RT_FLY,						/// ���ư��� �Ѿ�����
			RT_REVENGE,					/// �ݰݱ� ����
			RT_DUMMY_DAMAGE,			/// �´� ǥ�ð� �ȳ��� ������
			RT_GROGGY,					/// ��Ʋ�Ÿ��� ����
			RT_GUARD,					/// ���
            RT_SENSOR,          		/// ����Ÿ�� (NO ������)
			RT_CREATE_DAMAGE_EFFECT,	/// ���� ��ġ�� ���ο� ������ ����Ʈ�� �����Ѵ�
			RT_CREATE_EFFECT_SET,		///
			RT_GRAPPLE,					/// ��� ����
			RT_THROW,					/// ������
			RT_BOUNDING,				///
			RT_DRAG_UP,					/// kimhc // 2010.02.08 // ��ġ���� ���� ���� ��� ���� �ø��� ����
//#ifdef NEW_SKILL_2010_11
			RT_REFLECT,					// oasis907 : ����� [2010.11.1] // �� �����̾� - �������� ����
//#endif NEW_SKILL_2010_11

			//{{ kimhc // 2011.1.8 // û 1�� ����
			RT_FLIP_SAME_DIR,			/// kimhc // 2011.01.08 // ������ ���� ������ �ٶ󺸵��� ȸ��(�̹� ���� ������ ���� �ִ� ��쿡�� ȸ������ ����, �׿ܿ��� �⺻������ Small_Damage_Type�� ���� ó��)
			//}} kimhc // 2011.1.8 // û 1�� ����

// #ifdef NEW_GRAPPLE_ATTACK
// 			RT_NEW_GRAPPLE,				/// lucidash :  ������.
// #endif NEW_GRAPPLE_ATTACK
//#ifdef SEASON3_MONSTER_2011_7
			RT_ABSORB_MAGIC,
//#endif SEASON3_MONSTER_2011_7
//#ifdef ARA_CHANGE_CLASS_FIRST
			RT_AVOIDANCE,				/// ȸ��
//#endif //ARA_CHANGE_CLASS_FIRST
// #ifdef SERV_ELESIS_SECOND_CLASS_CHANGE	  // ������, �����ý� 1-2 �׷��� ������, 2-2 ������¡ ��Ʈ
			RT_FLIP_DIFF_DIR,					/// kimjh, ������ �ٸ� ������ �ٶ󺸵��� ( ���� ���ֺ����� ) ȸ�� // ���߿� ���
			RT_FLIP_DIFF_DIR_DUMMY_DAMAGE,		/// kimjh, ������ �ٸ� ������ �ٶ󺸵��� ( ���� ���ֺ����� ) ȸ�� // ���߿� ���
// #endif // SERV_ELESIS_SECOND_CLASS_CHANGE // ������, �����ý� 1-2 �׷��� ������, 2-2 ������¡ ��Ʈ

		};

		enum REACTER_TYPE
		{
			AT_UNIT = 0,
			AT_EFFECT,
			AT_MESH,
			AT_MANUAL_COLLISION_DATA,
		};

		enum EXTRA_DAMAGE_TYPE
		{
			EDT_NONE = 0,
			EDT_POISON,
			EDT_SLOW,
			EDT_FIRE,
			EDT_CURSE,
			EDT_SILENCE,
			EDT_LIGHTNING,
			EDT_REVERSE_LEFT_RIGHT,
			EDT_COLD,
			EDT_Y_PRESSED,



			EDT_FIRE_SMASH,				/// �巡���� ������ ȭ����Ÿ, �ߺ� ������ extra_damage
			EDT_COLD_SMASH,				/// �ñ� ��Ÿ
			EDT_EVENT_SMASH,			/// �̺�Ʈ������ ����ϴ� ��Ÿ �迭 extra_damage
			EDT_LIGHTNING_SMASH,		/// kimhc // 2010-07-30 // ��Ʈȿ���� ���� ��Ÿ �߰�

			EDT_POISON_CLOUD,			/// ������
			EDT_DOUBLE_ATTACK,			/// �ι� ����

			EDT_MANA_DAMAGE,			/// MANA DAMAGE�� �ִ� Extra Damage, �������� ����

			EDT_FROZEN,					/// �����Ǽ� �������̰� ������ �Դ�
			EDT_LEG_WOUND,				/// ��ü ��ó, �������� ������, ��� �Ұ���, SI_A_RST_CUT_TENDON ����
			EDT_WEAPON_BREAK,			/// ���� �ı�
			EDT_ARMOR_BREAK,			/// �� �ı�

			EDT_ENTANGLE,				/// ������ �ӵ� 0, ���ڸ� ����. �ǰݽ� �ٿ� ���. ����, �ʻ�� ����
			EDT_DULL,					/// ��ȯ (SLOW�ʹ� �ٸ��� �Ӽ����� �������� ���� / ���ӵ����� ����)
			EDT_TIME_BOMB,				/// ���� ��ź
			EDT_STUN,					///
			EDT_AGING,					/// ��ȭ (����,�̼�,����,���ݷ� ����)
			EDT_DAMAGE_RATE,			/// �ǰ��� ���� hp�� Ư�������� �������� ������.

			EDT_MAGIC_DEFENCE_DEBUFF,
			EDT_STIGMA_DEBUFF,			/// �ǰ��ڰ� �޴� ũ��Ƽ�� �������� ������Ų�� (����)

			EDT_DRYAD,					/// ���ʱ۰� ȿ���� �����ϳ�, �ڿ������� ����Ǹ�, ����̾�� ����Ʈ�� ���� �����
//#ifdef EDT_EVENT_SMASH_SECOND
			EDT_EVENT_SMASH_2,
//#endif EDT_EVENT_SMASH_SECOND

//#ifdef NEW_SKILL_2010_11 // oasis907 : ����� [2010.11.8] // �ε� ����Ʈ - ���� ����
			EDT_BLIND,
//#endif NEW_SKILL_2010_11

			//{{ kimhc // 2010.12.29 // �ϸ� ���� �߰��� ���� ExtraDamage
			EDT_ICE_STING,
			EDT_FIRE_BALL,
			//}} kimhc // 2010.12.29 // �ϸ� ���� �߰��� ���� ExtraDamage

//#ifdef EDT_WATER_HOLD_TEST   // lucidash :: ����� [2010.1.4]
			EDT_WATER_HOLD,
//#endif EDT_WATER_HOLD_TEST

			EDT_BIGHEAD,				// �������� ��� �Ұ� (user reform scale�� �浹��)
			//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
//#ifdef	CASH_SKILL_FOR_CHUNG_2011_04
			EDT_PANIC,					/// �¿�Ű ������ ���� �������� �ɷȴ� Ǯ�ȴٸ� �ݺ� & ũ��Ƽ�ÿ� ���� Ȯ�� ����
			EDT_PAIN,					/// ���濡�� �ɷ��ִ� ExtraDamage�� ���� �����ϴ� Ƚ���� �޶����� ExtraDamage
			EDT_HEAD_SHOT,				/// ���� �����, ��弦 �޸� ���� �� �ߵ�
//#endif	CASH_SKILL_FOR_CHUNG_2011_04
			//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

			//{{ JHKang / ������ / 2011.6.19 / LEG_SHOT EDT
//#ifdef CHUNG_SECOND_CLASS_CHANGE
			EDT_LEG_SHOT,		/// �̵�, ���� �ӵ� ����
//#endif
			//}}
//#ifdef CHUNG_SECOND_CLASS_CHANGE // oasis907 : ����� [2011.6.16] û 2�� ���� - ���̾� �ȶ��
			EDT_TRANSIT_DEBUFF,
//#endif CHUNG_SECOND_CLASS_CHANGE
//#ifdef HAMEL_NEW_SETOPTION_EXTRADAMAGE
			EDT_WIND_STORM,
			EDT_EARTH_QUAKE,
//#endif HAMEL_NEW_SETOPTION_EXTRADAMAGE

//#ifdef VELDER_SECRET_DUNGEON
			EDT_CURSE_CLOUD,

			EDT_SCALE_CRITICAL_BUFF,
			EDT_BERSERKER_BUFF,
			EDT_MANA_BUFF,
			EDT_ATTACK_ALL_TEAM,
//#endif
//#ifdef UPGRADE_RAVEN
			EDT_RUNJUMP_SPEED,
//#endif
//#ifdef RAVEN_WEAPON_TAKER
			EDT_SMOKE_BLIND,
//#endif RAVEN_WEAPON_TAKER
//#ifdef EVE_ELECTRA
			EDT_FLASH_BANG,
//#endif EVE_ELECTRA
//#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
			EDT_MARK_OF_COMMANDER,
//#endif SERV_ADD_CHUNG_SHELLING_GUARDIAN

//#ifdef ADD_SOCKET_FOR_SECRET_SET
			EDT_FLAME_SMASH,		/// ���ùڽ��� ������ ȭ�� Ÿ��
			EDT_FIRE_WRAITH,		/// ����Ÿ ��Ʈ�� ���̽� �߻�
//#endif

//#ifdef SERV_RENA_NIGHT_WATCHER
			EDT_START_OF_DELAYED_FIRING,	///mauntain : ����ȯ [2012.05.21] ���� 2�� ���� ����Ʈ ���� - ������ ȭ��
//#endif SERV_RENA_NIGHT_WATCHER

//#ifdef SERV_RAVEN_VETERAN_COMMANDER
			EDT_BLEEDING,			///mauntain : ����ȯ [2012.07.16] ���̺� 3�� ���� ���׶� Ŀ�Ǵ� - ġ����
//#endif SERV_RAVEN_VETERAN_COMMANDER

//#ifdef STARLIGHT_SMASH
			EDT_STARLIGHT_SMASH,	/// ���� ��Ÿ ����
//#endif STARLIGHT_SMASH

//#ifdef CAMILLA_SMASH
			EDT_CAMILLA_SMASH,		/// �ؿ��� �߰� ����
//#endif
//#ifdef ADD_SOCKET_OPTION_SANDER_SET
			EDT_FALLING_ROCK,		/// ���� ��Ʈ ȿ�� - ���� ��ȯ
			EDT_SANDSTORM,			/// ���� ��Ʈ ȿ�� - �𷡵�ǳ ��ȯ

			EDT_WIND_VIBRATION,		/// ���� Īȣ
			EDT_BREEZE_ACCEL,
			EDT_EVIL_SWORD,
			EDT_SPIRIT_SWORD,
			EDT_GHOST_SWORD,
//#endif //ADD_SOCKET_OPTION_SANDER_SET

//#ifdef HAMEL_SECRET_DUNGEON // ����ȯ
			EDT_ACTIVE_DEBUFF,		/// Ÿ�ݽ� Ư�� ������� ����
//#endif // HAMEL_SECRET_DUNGEON

//#ifdef EDT_EVENT_SMASH_THIRD
			EDT_EVENT_SMASH_3,
//#endif EDT_EVENT_SMASH_THIRD

			//////////////////////////////////////////////////////////////////////////
			// 1�� �Ӽ� ��æƮ
			EDT_ENCHANT_BLAZE			= 100,
			EDT_ENCHANT_FROZEN,
			EDT_ENCHANT_POISON,
			EDT_ENCHANT_PIERCING,
			EDT_ENCHANT_SHOCK,
			EDT_ENCHANT_SNATCH,



			// 2�� �Ӽ� ��æƮ
			EDT_ENCHANT_BLAZE_MASTER,
			EDT_ENCHANT_FROZEN_MASTER,
			EDT_ENCHANT_POISON_MASTER,
			EDT_ENCHANT_MASTER_PIERCING,
			EDT_ENCHANT_MASTER_SHOCK,
			EDT_ENCHANT_MASTER_SNATCH,

			EDT_ENCHANT_BLAZE_PIERCING,
			EDT_ENCHANT_BLAZE_SHOCK,
			EDT_ENCHANT_BLAZE_SNATCH,

			EDT_ENCHANT_FROZEN_PIERCING,
			EDT_ENCHANT_FROZEN_SHOCK,
			EDT_ENCHANT_FROZEN_SNATCH,

			EDT_ENCHANT_POISON_PIERCING,
			EDT_ENCHANT_POISON_SHOCK,
			EDT_ENCHANT_POISON_SNATCH,


			// 3�� �Ӽ� ENCHANT
			EDT_ENCHANT_BLAZE_DOUBLE_MASTER,
			EDT_ENCHANT_FROZEN_DOUBLE_MASTER,
			EDT_ENCHANT_POISON_DOUBLE_MASTER,
			EDT_ENCHANT_DOUBLE_MASTER_PIERCING,
			EDT_ENCHANT_DOUBLE_MASTER_SHOCK,
			EDT_ENCHANT_DOUBLE_MASTER_SNATCH,
			EDT_ENCHANT_DOUBLE_BLAZE_PIERCING,
			EDT_ENCHANT_DOUBLE_BLAZE_SHOCK,
			EDT_ENCHANT_DOUBLE_BLAZE_SNATCH,
			EDT_ENCHANT_DOUBLE_FROZEN_PIERCING,
			EDT_ENCHANT_DOUBLE_FROZEN_SHOCK,
			EDT_ENCHANT_DOUBLE_FROZEN_SNATCH,
			EDT_ENCHANT_DOUBLE_POISON_PIERCING,
			EDT_ENCHANT_DOUBLE_POISON_SHOCK,
			EDT_ENCHANT_DOUBLE_POISON_SNATCH,
			EDT_ENCHANT_BLAZE_DOUBLE_PIERCING,
			EDT_ENCHANT_BLAZE_DOUBLE_SHOCK,
			EDT_ENCHANT_BLAZE_DOUBLE_SNATCH,
			EDT_ENCHANT_FROZEN_DOUBLE_PIERCING,
			EDT_ENCHANT_FROZEN_DOUBLE_SHOCK,
			EDT_ENCHANT_FROZEN_DOUBLE_SNATCH,
			EDT_ENCHANT_POISON_DOUBLE_PIERCING,
			EDT_ENCHANT_POISON_DOUBLE_SHOCK,
			EDT_ENCHANT_POISON_DOUBLE_SNATCH,

		};

		enum WAY_OF_SWORD_TYPE
		{
			WST_NONE,
			WST_VIGOR_NORMAL,
			WST_VIGOR_ACTIVE,
			WST_VIGOR_SPECIAL,
			WST_DESTRUCTION_NORMAL,
			WST_DESTRUCTION_ACTIVE,
			WST_DESTRUCTION_SPECIAL,
		};

		/// ���� Ÿ�Կ� ���� DP ���� ��ġ
		/// �ش� Ÿ���� Enum.lua�� ���� �Ǿ� �ְ�, Add_NasodRuler.lua�� ���� ��Ī �Ǿ� �ִ�.
		/// �ڵ�� CX2GUAdd::GetDPRateByType �Լ� ������ �������̴�.
		enum DYNAMO_PARTICLE_RATE_TYPE
		{
			DPRT_NONE				= -1,
			DPRT_SPECIAL_ACTIVE		= 0,	/// ����� ��Ƽ��
			DPRT_ACTIVE				= 1,	/// ��Ƽ�� ����
			DPRT_COMMAND			= 2,	/// Ŀ�ǵ� ����
			DPRT_NASOD_ARMOR		= 3,	/// ���ҵ� �Ƹ� ����
		};


		enum DAMAGE_AREA_TYPE
		{
			DAT_ALL_UNIT = 0,
			DAT_NEAR_UNIT,
			DAT_FAR_UNIT,
		};

		struct sAreaHitUnit
		{
			int unitIndex;
			float fDistance;
		};


		struct StartOfDelayedFiringData
		{
			int			m_iType;				/// ȭ�� Ÿ��( ������ ȭ�� ���� ������ ��ȣź ���� )
			int			m_iAttackerIndex;		/// �ش� ȭ�� �߻���
			int			m_iArrowOrderNumber;	/// ȭ�� �߻� ����
			int			m_iArrowIndex;			/// ���� ���� ȭ���� �ε���
			float		m_fCoolTime;			/// EDT ���� �ð�
			float		m_fIncreaseDamage;		/// �ش� ��ü�� �߰� ������
			float		m_fSkillPowerRate;
			D3DXVECTOR3 m_vExplosionOffSet;		/// ���ֿ��� ���� �� ����Ʈ ��ġ

			StartOfDelayedFiringData( int iType, int iAttackerIndex, int iArrowOrderNumber, int iArrowIndex = -1, float fCoolTime = 10.f, float fIncreaseDamage = 1.f )
			{
				m_iType				= iType;
				m_iAttackerIndex	= iAttackerIndex;
				m_iArrowOrderNumber = iArrowOrderNumber;
				m_iArrowIndex		= iArrowIndex;
				m_fCoolTime			= fCoolTime;
				m_fIncreaseDamage	= fIncreaseDamage;
				m_fSkillPowerRate	= 1.f;
				m_vExplosionOffSet  = D3DXVECTOR3( 0.f, 0.f, 0.f );
			}
		};

		struct StartOfDelayedFiringDataSort

		{
			bool operator()( const CX2DamageManager::StartOfDelayedFiringData* _Left, const CX2DamageManager::StartOfDelayedFiringData* _Right ) const
			{
				if( _Left->m_iArrowOrderNumber < _Right->m_iArrowOrderNumber )
				{
					return true;
				}
				else
				{
					return false;
				}
			};
		};


		struct ExtraDamageData
		{
			EXTRA_DAMAGE_TYPE		m_ExtraDamageType;
			KProtectedType<float>	m_fTime;
			KProtectedType<float>	m_DamagePerSec;
			KProtectedType<int>		m_Accumulation;
			KProtectedType<float>	m_fElapsedTimeToEffectDamage;
			KProtectedType<float>	m_fRate;			/// �� extra damage�� �۵��� Ȯ��
			KProtectedType<float>	m_fPureRate;		/// Rate ���� ������ ���� ������ ���� Rate
			KProtectedType<float>	m_Damage;			/// ������

			KProtectedType<int>		m_iLevel;			/// ����
			KProtectedType<int>		m_iAddLevel;		/// �߰� ����
			KProtectedType<bool>	m_bIgnoreResist;	/// ����
			bool					m_bDefenderLevel;

			KProtectedType<float>	m_fDamageRate;		/// ������
			KProtectedType<float>	m_fRunJumpRate;		/// �̵�, ���� �ӵ�
			KProtectedType<float>	m_fAnimSpeedRate;	/// �ִϸ��̼� �ӵ�

			bool					m_bFixedDamage;

			//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
			/// ExtraDamage�� Map���� �ٲٰ� �Ǹ� ����
			bool					m_bCorrectDirection;		/// Panic���� Ű���� �Է¿� ���� ���������� �����̴��� ���θ� �����ϴ� �÷���
			KProtectedType<float>	m_fCriticalRateToDefender;	/// �ǰ��ڰ� ũ��Ƽ�ÿ� ���� Ȯ�� ����
			//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

			//{{ JHKang / ������ / 2011.6.20 / ���� �� ���� �̵�, ���� �ӵ� ���� ������ ���� ���� �߰�
			KProtectedType<float>	m_fJumpRate;		/// ���� �ӵ�(LEG_SHOT���� m_fRunJumpRate�� �̵� �ӵ��� ���)
			//}}

			//{{ kimhc // 2011-07-29 // Į�󺼱� ���� ���� ����
			// ����� EDT_PAIN���� ������ ���� EDT������ �����ϱ� ���� �뵵�� ���
			CX2GameUnitoPtr			m_optrGameUnit;
			//}} kimhc // 2011-07-29 // Į�󺼱� ���� ���� ����
			bool					m_bCured;

			vector<CX2DamageManager::StartOfDelayedFiringData*> m_vecStartOfDelayedFiringData;		/// ������ ��ȣź ���� ����

			vector<float>				m_vecOverlapDamaage;	/// ġ���� ��ø ������ ����

			CX2BuffFactorPtr			m_ptrBuffFactor;		/// ������ ���� ����


            bool                    m_bAttackedByMyUnit;        /// �� ���ֿ� ���� ���ݴ��ߴ���

			ExtraDamageData()
			{
				Init();
			}
			void Init()
			{
				m_fRate							= 0.f;
				m_fPureRate						= 0.f;
				m_Damage						= 0.0f;
				m_ExtraDamageType				= EDT_NONE;
				m_fTime							= 0.f;
				m_DamagePerSec					= 0.f;
				m_Accumulation					= 0;
				m_fElapsedTimeToEffectDamage	= 0.f;

				m_iLevel						= -1;
				m_iAddLevel						= 0;
				m_bIgnoreResist					= false;
				m_bDefenderLevel				= false;

				m_fDamageRate					= 0.f;
				m_fRunJumpRate					= 0.f;
				m_fAnimSpeedRate				= 0.f;
				m_bFixedDamage					= false;
				//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
				/// ExtraDamage�� Map���� �ٲٰ� �Ǹ� ����
				m_fCriticalRateToDefender		= 0.0f;
				m_bCorrectDirection				= true;	/// Panic���� Ű���� �Է¿� ���� ���������� �����̴��� ���θ� �����ϴ� �÷���
				//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

				//{{ JHKang / ������ / 2011.6.20 / ���� �� ���� �̵�, ���� �ӵ� ���� ������ ���� ���� �߰�
				m_fJumpRate						= 0.f;
				//}}

				//{{ kimhc // 2011-07-29 // Į�󺼱� ���� ���� ����
				m_optrGameUnit.Reset();
				//}} kimhc // 2011-07-29 // Į�󺼱� ���� ���� ����
				m_bCured						= false;

				for( UINT i = 0; i < m_vecStartOfDelayedFiringData.size(); ++i )		/// ������ ��ȣź ���� ����
				{
					CX2DamageManager::StartOfDelayedFiringData* pData = m_vecStartOfDelayedFiringData[i];
					SAFE_DELETE( pData );
				}
				m_vecStartOfDelayedFiringData.erase( m_vecStartOfDelayedFiringData.begin(), m_vecStartOfDelayedFiringData.end() );

				m_vecOverlapDamaage.erase( m_vecOverlapDamaage.begin(), m_vecOverlapDamaage.end() );	/// ġ���� ��ø ������ ���� ����

                m_bAttackedByMyUnit = false;

			}
			void Copy( const ExtraDamageData& data )
			{
				m_fRate							= data.m_fRate;
				m_fPureRate						= data.m_fPureRate;
				m_Damage						= data.m_Damage;
				m_ExtraDamageType				= data.m_ExtraDamageType;
				m_fTime							= data.m_fTime;
				m_DamagePerSec					= data.m_DamagePerSec;
				m_Accumulation					= data.m_Accumulation;
				m_fElapsedTimeToEffectDamage	= data.m_fElapsedTimeToEffectDamage;

				m_iLevel						= data.m_iLevel;
				m_iAddLevel						= data.m_iAddLevel;
				m_bIgnoreResist					= data.m_bIgnoreResist;
				m_bDefenderLevel				= data.m_bDefenderLevel;
				m_fDamageRate					= data.m_fDamageRate;
				m_fRunJumpRate					= data.m_fRunJumpRate;
				m_fAnimSpeedRate				= data.m_fAnimSpeedRate;
				m_bFixedDamage					= data.m_bFixedDamage;
				//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
				/// ExtraDamage�� Map���� �ٲٰ� �Ǹ� ����
				m_fCriticalRateToDefender		= data.m_fCriticalRateToDefender;
				m_bCorrectDirection				= data.m_bCorrectDirection;	/// Panic���� Ű���� �Է¿� ���� ���������� �����̴��� ���θ� �����ϴ� �÷���
				//}} 2011-04 �� ��ġ�� û ĳ�� ��ų

				//{{ JHKang / ������ / 2011.6.20 / ���� �� ���� �̵�, ���� �ӵ� ���� ������ ���� ���� �߰�
				m_fJumpRate						= data.m_fJumpRate;
				//}}

				//{{ kimhc // 2011-07-29 // Į�󺼱� ���� ���� ����
				m_optrGameUnit					= data.m_optrGameUnit;		// swallow copy
				//}} kimhc // 2011-07-29 // Į�󺼱� ���� ���� ����
				m_bCured						= data.m_bCured;

				m_ptrBuffFactor					= data.m_ptrBuffFactor;

                m_bAttackedByMyUnit = data.m_bAttackedByMyUnit;

			}
			bool Verify() const
			{
				if( m_fTime.Verify() == false
					|| m_DamagePerSec.Verify() == false
					|| m_Accumulation.Verify() == false
					|| m_fElapsedTimeToEffectDamage.Verify() == false
					|| m_fRate.Verify() == false
					|| m_fPureRate.Verify() == false
					|| m_Damage.Verify() == false
					|| m_iLevel.Verify() == false
					|| m_iAddLevel.Verify() == false
					|| m_bIgnoreResist.Verify() == false
					|| m_fDamageRate.Verify() == false
					|| m_fRunJumpRate.Verify() == false
					|| m_fAnimSpeedRate.Verify() == false
					//{{ 2011-04 �� ��ġ�� û ĳ�� ��ų
					/// ExtraDamage�� Map���� �ٲٰ� �Ǹ� ����
					|| m_fCriticalRateToDefender.Verify() == false
				//}} 2011-04 �� ��ġ�� û ĳ�� ��ų
					|| m_fJumpRate.Verify() == false
					)
				{
					return false;
				}
				return true;
			}

			float GetCalcPerDamage(int iUnitLevel, float fResist) const;

		};

		struct HitUnit
		{
			float			fRemainGap; //0���� �۾����� ���� ���ݰ���
			CX2GameUnitoPtr	optrHitGameUnit;

            HitUnit()
                : fRemainGap( 0.f )
                , optrHitGameUnit()
            {
            }
		};
        typedef std::vector<HitUnit>    HitUnitVector;
        static inline void swap( HitUnit& lhs_, HitUnit& rhs_ )
        {
            std::swap( lhs_.fRemainGap, rhs_.fRemainGap );
            lhs_.optrHitGameUnit.Swap( rhs_.optrHitGameUnit );
        }

		struct Damage
		{
			float	fPhysic;
			float	fMagic;

			Damage()
			{
				Init();
			}
			void Init()
			{
				fPhysic		= 0.0f;
				fMagic		= 0.0f;
			}
			float GetAllDamage()
			{
				return fPhysic + fMagic;
			}
		};

		struct AttackerGet
		{
			float m_fHP;
			float m_fMP;

			AttackerGet()
			{
				Init();
			}

			void Init()
			{
				m_fHP = 0.f;
				m_fMP = 0.f;
			}
		};


		struct DamageData
		{
			DAMAGE_TYPE						damageType;
			REACTER_TYPE					attackerType;
			ATTACK_TYPE						attackType;
			CX2GameUnitoPtr					optrAttackerGameUnit;	/// �̰͵� �̰�����.. UID�� �ʿ��� ����..
            CX2DamageEffect_CEffectHandle   hAttackerEffect;
			//{{ seojt // 2009-1-12, 16:15
			CKTDGXSkinAnim*	                pAttackerAnim;			/// ������...
			//CKTDGXSkinAnimPtr	            pAttackerAnim;
			//}} seojt // 2009-1-12, 16:15
			CKTDXCollision::CollisionData*	pCollisionData;


			HIT_TYPE				hitType;
			REACT_TYPE				reActType;
			REACT_TYPE				reActResult;
			bool					m_bMeleeAttack;		/// ���������ΰ�?(�⺻������ ������ Ÿ�ݹڽ��� true, ����������Ʈ�� false �̳�, ��ũ��Ʈ�� DAMAGE_DATA���� ����Ʈ���� �������� ���氡��)
			bool					m_bDrain;
			bool					m_bDrainMana;
			bool					m_bAttackAllTeam;
			float					m_fDungeonRate;

			int						techPoint;
			Damage					damage;							/// ����, �������� �ٲٴ°�...

			vector<CX2BuffFactorPtr>	m_vecBuffFactorPtr;			/// ���濡�� ����/�����

			ExtraDamageData			m_ExtraDamage;					/// ������ ��ũ��Ʈ�� �ִ� extra_damage
			ExtraDamageData			m_BufExtraDamage;				/// ������ ������
			ExtraDamageData			m_SocketExtraDamage;			/// ������ ���� �ɼǿ� ���� extra_damage, �ߺ� �ȵǴ� ��
			vector<ExtraDamageData>	m_MultipleExtraDamage;			/// ������ ���� �ɼǿ� ���� extra_damage, �ߺ� �Ǵ� ��


			AttackerGet				attackerGet;
			D3DXVECTOR2				backSpeed;
			float					fForceDownValue;
			float					fGroggyTime;

			float					fHitAddMP;
			float					fHitAddHPPer;					/// ���� ������ �����ڿ��� ȸ����ų ü�� ����

			float					fStopTimeAtt;
			float					fStopTimeDef;

			CKTDGCamera::SHAKE_TYPE	m_eCameraShakeType;
			float					fCameraCrashGap;
			float					fCameraCrashTime;
			int						clearScreen;
			D3DXCOLOR				clearScreenColor;
			//{{ oasis907 : ����� [2010.2.11] //
			bool					bArrangedFly;
			//}}
			bool					bForceFly;
			bool					bCanRevenge;
			bool					bCanReflexMagic;
			bool					bReAttack;
			float					fHitGap;
			vector<HitUnit>			hitUnitList;

			D3DXVECTOR3				impactPoint;

			REACTER_TYPE			defenderType;
			CX2GameUnitoPtr			optrDefenderGameUnit;

            bool                    m_bGuard;
            bool                    m_bWBackSpeed;

			std::wstring			m_wstrCreateDamageEffectName;
			std::wstring			m_wstrCreateEffectSetName;
			bool					m_bCreateEffectSetTraceUnit;			// ����Ʈ �����ϴ� damage effect �� �� ������ effect set�� �ǰݴ��� ������ trace ���� ����
			bool					m_bTraceUnitDieDeleteEffectSet;			// trace ������ ������ �ش� ����Ʈ ���� ��������, true�� ����
			bool					m_bPenetrate;

            bool                    DamageLineGroup;
			float					DamageDistance;

			bool					m_bFlipDirOnHit;		// Ÿ���� ���� ����� ������ �ݴ�� ��������

			DAMAGE_AREA_TYPE		m_eDamageAreaType;
			int						m_iDamageUnitNum;
			bool					m_bNoDamage;
			bool					m_bHitedManaDamage;

			float					m_fDrainHpRate;		// Ÿ�ݽ� �������� �󸶸� hp�� ȯ��

			std::vector<std::wstring>	m_vecHitParticleName;

			bool					m_bNoDetonation;	// �������� ���ϵ��� �Ѵ�.
			bool					m_bDrainHpParticle; // �����...������׼���...HP�� ����� ����Ʈ�� ���� ������..
			bool					m_bApplyExtraDamage;

			int						m_ResistType;
			bool					m_bAttackOurTeam;
			float					m_fHpPercentUp;
			float					m_fMpPercentUp;

			bool					m_bApplyMotionOffset;

			float					m_fCriticalRate;
			float					m_fDecreaseForceDown;

			bool					m_bNoBuff;

			float					m_fRateModifier;

			// ���� DamageEffect.lua �ѹ����� �ʰ� �ݿ���Ű�� ���� �ּ� ����
			// oasis907 : ����� [2010.11.4] // �׷��� ��ó - ���� ���(�нú�)
			float					m_fHitDamageEffectInvokeRate;
			float					m_fHitDamageEffectDamageRate;

			bool					m_bIgnoreDefence;

			float	m_fPvpRate;

			DAMAGE_TRIGGER_TYPE		m_eDamageTrigger;	//
			bool					m_bOnlyAttackBasic;
			bool					m_bIgnoreStateDefence;
			bool					m_bNoMiss;

			WAY_OF_SWORD_TYPE		m_WayofSwordType;

			DYNAMO_PARTICLE_RATE_TYPE	m_DPRateType;		///	DP ���� ����


			float					m_fDrainMpByReact;
			float					m_fDrainMpRate;
			bool					m_bLvDamage;

			int						m_iLaserGroupID;

#ifdef ADD_BACKATTACK
			bool					m_bApplyBackAttack;
#endif

			float					m_fCreateDamageEffectRate;

			float					m_fIncreaseMP;				/// ���� ���Ƿ�


			int							m_iHitAddForcePower;		/// �ǰݽ� �������� ��� ȸ��
			CX2SkillTree::SKILL_ID		m_eUsedSkillID;				/// �ش� ������ �����͸� ������ ��ų ���̵� ( ���� ������ ��� �ؿ�... )

            bool                    m_bRelaxNPCReactionStateCheck;

			float					m_fApplyCriticalDamage;			/// ũ��Ƽ�� ������ ������ ������Ų��. ex> ���� ũ��Ƽ�� ������ 1.5�� �ش� ���� ������ �������� �������� ���ȴ�.
			float					fHitAddHPbyAttackPower;			/// ���� ������ �����ڿ��� ȸ����ų ü�� ���� (������ �������� ��հ��� ����Ͽ� ����)


			void SimpleInit()
			{
				damageType			= DT_PHYSIC;
				attackerType		= AT_UNIT;
				attackType			= AT_NORMAL;
				optrAttackerGameUnit.Reset();
                hAttackerEffect     = INVALID_DAMAGE_EFFECT_HANDLE;
				//{{ seojt // 2009-1-12, 16:15
				pAttackerAnim		= NULL;
                //pAttackerAnim.reset();
				//}} seojt // 2009-1-12, 16:15
				pCollisionData		= NULL;
				m_bMeleeAttack		= false;
				m_bDrain			= false;
				m_bDrainMana		= false;
				m_bAttackAllTeam	= false;
				m_fDungeonRate		= 1.0f;

				techPoint			= 0;
				hitType				= HT_DEFAULT;
				reActType			= RT_NO_DAMAGE;
				reActResult			= RT_NO_DAMAGE;

				damage.Init();

				m_vecBuffFactorPtr.resize( 0 );
				m_ExtraDamage.Init();
				m_BufExtraDamage.Init();


				backSpeed			= D3DXVECTOR2( 0.0f, 0.0f );
				fForceDownValue		= 0.0f;
				fGroggyTime			= 0.0f;

				fHitAddMP			= 6.0f;
				fHitAddHPPer		= 0.0f;

				fStopTimeAtt		= 0.0f;
				fStopTimeDef		= 0.0f;
				m_eCameraShakeType	= CKTDGCamera::DECT_UP_DOWN;
				fCameraCrashGap		= 0.0f;
				fCameraCrashTime	= 0.0f;
				clearScreen			= 0;
				clearScreenColor	= D3DXCOLOR(0x00ffffff);
				//{{ oasis907 : ����� [2010.2.11] //
				bArrangedFly		= false;
				//}
				bForceFly			= false;
				bCanRevenge			= true;
				bCanReflexMagic		= false;
				bReAttack			= false;
				fHitGap				= 0.0f;

				hitUnitList.resize(0);

                m_bGuard = false;
                m_bWBackSpeed = false;

				m_wstrCreateDamageEffectName = L"";
				m_wstrCreateEffectSetName = L"";
				m_bCreateEffectSetTraceUnit = false;
				m_bTraceUnitDieDeleteEffectSet = true;
				m_bPenetrate		= false;

                DamageLineGroup     = false;
				DamageDistance		= 99999.f;

				m_bFlipDirOnHit		= false;

				m_eDamageAreaType	= DAT_ALL_UNIT;
				m_iDamageUnitNum	= 0;
				m_bNoDamage			= false;
				m_bHitedManaDamage	= false;
				m_fDrainHpRate		= 0.f;

				m_vecHitParticleName.clear();

				m_bNoDetonation		= false;
				m_bDrainHpParticle	= false;
				m_bApplyExtraDamage = true;
				m_ResistType = 0;
				m_fHpPercentUp = 0.f;
				m_fMpPercentUp = 0.f;
				m_bAttackOurTeam = false;
				m_bApplyMotionOffset	= true;

				m_fCriticalRate		= 0.f;
				m_fDecreaseForceDown = 0.f;

				m_bNoBuff			= false;

				m_fRateModifier		= 1.f;

				// ���� DamageEffect.lua �ѹ����� �ʰ� �ݿ���Ű�� ���� �ּ� ����
				// oasis907 : ����� [2010.11.4] // �׷��� ��ó - ���� ���(�нú�)
				m_fHitDamageEffectInvokeRate = 1.f;
				m_fHitDamageEffectDamageRate = 1.f;

				m_bIgnoreDefence	= false;

				m_fPvpRate = 1.f;

				m_eDamageTrigger = DTT_NONE;
				m_bOnlyAttackBasic = false;
				m_bIgnoreStateDefence = false;
				m_bNoMiss = false;

				m_WayofSwordType	= WST_NONE;

				m_DPRateType		= DPRT_NONE;

				m_fDrainMpByReact = 0.f;
				m_fDrainMpRate = 1.f;
				m_bLvDamage = false;

				m_iLaserGroupID			= 0;

#ifdef ADD_BACKATTACK
				m_bApplyBackAttack		= true;
#endif
				m_fCreateDamageEffectRate	= 0.f;

				m_fIncreaseMP				= 0.f;				/// ���� ���Ƿ�


				m_iHitAddForcePower = 0;
				m_eUsedSkillID		= CX2SkillTree::SI_NONE;

                m_bRelaxNPCReactionStateCheck = false;

				m_fApplyCriticalDamage = 0.f;
				fHitAddHPbyAttackPower = 0.f;

			}

			void Init()
			{
				SimpleInit();

				impactPoint		= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
				defenderType	= AT_UNIT;
				optrDefenderGameUnit.Reset();

				m_SocketExtraDamage.Init();
				m_MultipleExtraDamage.resize(0);
			}

			void PushBuffFactor( CX2BuffFactorPtr ptrBuffFactor_, const bool bClearExistedFactor_ = true )
			{
				if ( bClearExistedFactor_ )
				{
					m_vecBuffFactorPtr.clear();
					PushSocketAndTemporaryBuffFactor( this );	/// ���Ͽɼ�, �������� �Ͻ������� ���� ���͵��� �ٽ� ����
				}

				if ( NULL != ptrBuffFactor_ )
				{
					CX2BuffFactorPtr ptrBuffFactorClone = ptrBuffFactor_->GetClonePtr();
					if ( NULL != ptrBuffFactorClone )
					{
						ptrBuffFactorClone->SetGameUnitBuffFactor( optrAttackerGameUnit.GetObservable() );
						m_vecBuffFactorPtr.push_back( ptrBuffFactorClone );
					}
				}
			}

			~DamageData()
			{
				hitUnitList.clear();
			}

			DamageData()
			{
				Init();
			}

			DamageData( const DamageData& ) = default;
			DamageData& operator=( const DamageData& ) = default;

//{{ robobeg : 2013-09-17
            bool    IsAttackedByMyUnit();
//}} robobeg : 2013-09-17

		};

		struct WayOfSwordConstTable
		{
			float m_fSkillTypeConst_VigorCommand;
			float m_fSkillTypeConst_VigorActive;
			float m_fSkillTypeConst_VigorSpecialActive;
			float m_fSkillTypeConst_DestCommand;
			float m_fSkillTypeConst_DestActive;
			float m_fSkillTypeConst_DestSpecialActive;
			float m_fSkillTypeConst_DamageReact;

			float m_fStateConst_DestSkill_TowardVigorState;
			float m_fStateConst_VigorSkill_TowardVigorState;
			float m_fStateConst_DestSkill_TowrdDestState;
			float m_fStateConst_VigorSkill_TowrdDestState;
			float m_fStateConst_DestSkill_InVigorState;
			float m_fStateConst_VigorSkill_InVigorState;
			float m_fStateConst_DestSkill_InDestState;
			float m_fStateConst_VigorSkill_InDestState;
			float m_fStateConst_DestSkill_CenterState;
			float m_fStateConst_VigorSkill_CenterState;

			float m_fStateConst_DamageReact_TowardVigorState;
			float m_fStateConst_DamageReact_TowrdDestState;
			float m_fStateConst_DamageReact_InVigorState;
			float m_fStateConst_DamageReact_InDestState;
			float m_fStateConst_DamageReact_CenterState;


			float m_fGameModeConst_NPC_Hit;
			float m_fGameModeConst_PVP_Hit;

			float m_fConst_VigorStateThreshold;
			float m_fConst_DestStateThreshold;
			float m_fConst_DestRageWSPGain;

			// ���� ������ ����/���� ���� ���� ����
			float m_fConst_DestRageWSPGain_Elesis;

			WayOfSwordConstTable()
			{
				m_fSkillTypeConst_VigorCommand				= 0.f;
				m_fSkillTypeConst_VigorActive				= 0.f;
				m_fSkillTypeConst_VigorSpecialActive		= 0.f;
				m_fSkillTypeConst_DestCommand				= 0.f;
				m_fSkillTypeConst_DestActive				= 0.f;
				m_fSkillTypeConst_DestSpecialActive			= 0.f;
				m_fSkillTypeConst_DamageReact				= 0.f;

				m_fStateConst_DestSkill_TowardVigorState	= 0.f;
				m_fStateConst_VigorSkill_TowardVigorState	= 0.f;
				m_fStateConst_DestSkill_TowrdDestState		= 0.f;
				m_fStateConst_VigorSkill_TowrdDestState		= 0.f;
				m_fStateConst_DestSkill_InVigorState		= 0.f;
				m_fStateConst_VigorSkill_InVigorState		= 0.f;
				m_fStateConst_DestSkill_InDestState			= 0.f;
				m_fStateConst_VigorSkill_InDestState		= 0.f;
				m_fStateConst_DestSkill_CenterState			= 0.f;
				m_fStateConst_VigorSkill_CenterState		= 0.f;

				m_fStateConst_DamageReact_TowardVigorState	= 0.f;
				m_fStateConst_DamageReact_TowrdDestState	= 0.f;
				m_fStateConst_DamageReact_InVigorState		= 0.f;
				m_fStateConst_DamageReact_InDestState		= 0.f;
				m_fStateConst_DamageReact_CenterState		= 0.f;

				m_fGameModeConst_NPC_Hit					= 0.f;
				m_fGameModeConst_PVP_Hit					= 0.f;

				m_fConst_VigorStateThreshold				= 0.f;
				m_fConst_DestStateThreshold					= 0.f;
				m_fConst_DestRageWSPGain					= 0.f;

				// ���� ������ ����/���� ���� ���� ����
				m_fConst_DestRageWSPGain_Elesis				= 0.f;
			}
		};



	public:
		CX2DamageManager(void);
		~CX2DamageManager(void);

		static void PushSocketAndTemporaryBuffFactor( DamageData* pDamageData_ );	/// ���� �ɼ�, �Ͻ������� �ɸ� ������ ���� BuffFactor ����
		bool OpenDamageSoundScript( const WCHAR* wszFileName );
		bool ParseDamageSoundBlock( KLuaManager& luaManager, HIT_TYPE eHitType, const char* pszTableNameUTF8 );
		const WCHAR* GetDamageSound( HIT_TYPE hitType, HITTED_TYPE hittedType );

		void ParsingBuffFactor( KLuaManager& luaManager_, DamageData* pDamageData_ );

		bool SetDamageDataFromLUA( DamageData* pDamageData, KLuaManager& luaManager, const char* pTableName = "DAMAGE_DATA", float fPowerRate = 1.f, const bool IsEqippedMemo_ = false );

		bool DamageCheck( DamageData* pDamageData, bool bAttackOnlyThisUnit = false, UidType attackUnitUID = -1 );

		wstring GetExtraDamageName( CX2DamageManager::EXTRA_DAMAGE_TYPE extraDamageType, bool bTwoLine = false );

		void SetExtraDamageData( DamageData* pDamageData, bool bApplyEffect,
			CX2DamageManager::EXTRA_DAMAGE_TYPE edtType, float fRate, float fTime, float fDamage,
			float fExtraDamage = 0.f, int iLevel = -1, bool bIgnoreResist = false,
			float fDamageRate = 0.f, float fRunJumpRate = 0.f, float fAnimSpeedRate = 0.f );

		float	CalcWayOfSwordPoint( WAY_OF_SWORD_TYPE wayOfSwordType_, float fDamageRate_, int iWayOfSwordState );
		float	CalcWayOfSwordPointOnDamageReact( float fDamageRate_, int iWayOfSwordState );
		float	CalcVigorMpUpOnAttack( WAY_OF_SWORD_TYPE wayOfSwordType_, int iWayOfSwordState );
		float	CalcVigorMpUpOnDamageReact( int iWayOfSwordState );
		bool	OpenWayOfSwordScript( const WCHAR* wszFileName );
		float	GetVigorStateThreshold(){	return m_WayOfSwordConstTable.m_fConst_VigorStateThreshold; }
		float	GetDestStateThreshold(){	return m_WayOfSwordConstTable.m_fConst_DestStateThreshold;	}
		float	GetDestRageWSPGain();

		// �����ý� ���� ��
		float	GetDestRageWSPGain_Elesis();
		float	CalcVigorMpUpOnAttack_Elesis( WAY_OF_SWORD_TYPE wayOfSwordType_, int iWayOfSwordState );

		void	AddHitUnit( DamageData& kDamageData, HitUnit _HitUnit ){ kDamageData.hitUnitList.push_back(_HitUnit); }


		bool	GetIsPossibleProvokeExtraDamage ( DamageData * pDamageData );
	private:
		bool CollisionDataCheckFromUnitToUnit( CX2GameUnit* pAttackerGameUnit_,
			const CKTDXCollision::CollisionDataListSet& setDefenderCollisionDataList_,
			CKTDXCollision::CollisionType& collisionType_, D3DXVECTOR3* pImpactPos_ );

		//bool CollisionDataCheckFromUnitToEffect( CX2GameUnit* pAttackerGameUnit_,
		//	const CKTDXCollision::CollisionDataList& listDefenderCollisionData_,
		//	CKTDXCollision::CollisionType& collisionType_, D3DXVECTOR3* pImpactPos_ );

		bool UnitToUnit(DamageData* pDamageData, bool bAttackOnlyThisUnit = false, UidType attackUnitUID = -1 );	//������ ���� ����
		bool MeshToUnit(DamageData* pDamageData );  //�޽÷� ���� ����
		bool CollisionDataToUnit(DamageData* pDamageData );
		bool UnitToEffect( DamageData* pDamageData );
		bool EffectToUnit( DamageData* pDamageData );
		bool EffectToEffect( DamageData* pDamageData );

		float CalcHitDodgePercent( CX2GameUnit* pAttacker, CX2GameUnit* pDefender );

        bool GroundHit( DamageData* pDamageData, bool bAttackOnlyThisUnit, UidType attackUnitUID );
		bool AreaHit( DamageData* pDamageData, bool bAttackOnlyThisUnit, UidType attackUnitUID );

		/// �׷� ������ Ȱ���� �˻�
		bool CheckActiveGroupDamage( const wstring& wstrDamageEffectName_, CX2DamageManager::DamageData* pDamageData_ );
		/// �׷� ������ ����
		void SetGroupDamage( const wstring& wstrDamageEffectName_, CX2DamageManager::DamageData* pDamageData_ );



		bool SetBuffFactorToDamageDataByBuffFactorID( OUT DamageData* pDamageData_, IN const UINT uiBuffFactorId_, IN const UINT uiLevel_ = 1 );

		void ParsingBuffFactorID( KLuaManager& luaManager_, DamageData* pDamageData_, const bool IsEqippedMemo_ = false );


		WayOfSwordConstTable	m_WayOfSwordConstTable;

		CKTDXCollision*			m_pCollision;

		std::wstring			m_DamageSoundName[HIT_TYPE_COUNT][HITTED_TYPE_COUNT];
		vector<CKTDXDeviceSound*>	m_pvecReadySound;

};
