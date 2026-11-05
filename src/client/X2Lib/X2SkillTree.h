#pragma once


/**
	@class CX2SkillTree
	@brief
		- ĳ���� Skill�� ���� ������ ó��
		- enum : SKILL_TYPE, ACTIVE_SKILL_USE_CONDITION, SKILL_ID, SKILL_ABILITY_TYPE, SKILL_MEMO_ID
		- struct : SkillTemplet, SkillTemplet, SealSkillItemInfo
	@date 2010/11/05
	@section MODIFYINFO ���� ����
		- 2010/10/31: JHKang
			- Raven(Blade Master SKILL_ID 2519) : ��� Ż��(Emergency Escape)
			- Elsword(Rune Slayer SKILL_ID 1019) : �糪 �����̵�(Luna Blade)
		- 2010/11/
			- Eve(Code Empress SKILL_ID 3019) : �Ʈ ���Ǿ�(Assualt Spear)
			- Eve(Code Nemesis SKILL_ID 3020) : ����� ����(Atomic Shield)
		- 2013/05/30: JHKang
			- ��ų ���� ����, Aisha ��ų ���̵� �߰�
*/

class CX2SkillTree
{
public:

	enum SKILL_TYPE
	{
		ST_NONE = 0,
		ST_PASSIVE_PHYSIC_ATTACK,		// STAT ��ġ�� ��������ִ� passive
		ST_PASSIVE_MAGIC_ATTACK,
		ST_PASSIVE_MAGIC_DEFENCE,
		ST_PASSIVE_PHYSIC_DEFENCE,
		ST_BUFF,
		ST_ACTIVE,						// timestop�� ���� �ʻ��
		ST_SPECIAL_ACTIVE,				// timestop�� �ִ� �ʻ��
		ST_PASSIVE,						// STAT�ܿ� ��Ÿ ����� �ִ� passive

//#ifdef GUILD_SKILL // oasis907 : ����� [2009.11.25] // ���� ���� �ּ� ����
		// ��� ��ų
		ST_GUILD_PASSIVE_PHYSIC_ATTACK_MAGIC_DEFENCE,
		ST_GUILD_PASSIVE_MAGIC_ATTCK_PHYSIC_DEFENCE,
		ST_GUILD_PASSIVE,
//#endif GUILD_SKILL

//#ifdef ADDED_RELATIONSHIP_SYSTEM
		ST_RELATIONSHIP_SKILL,
//#endif // ADDED_RELATIONSHIP_SYSTEM

		ST_HYPER_ACTIVE_SKILL,
	};



	enum ACTIVE_SKILL_USE_CONDITION
	{
		ASUT_NONE,
		ASUT_GROUND,		// ���������� �� �� �ִ� �ʻ��, skip_point������
		ASUT_AIR,			// ���߿����� �� �� �ִ� �ʻ��, skip_point������
		ASUT_ANYTIME,		// �ƹ����� �� �� �ִ� �ʻ��
		ASUT_AIR_ONLY,		// ���߿����� �� �� �ִ� �ʻ��, skip_point������
		ASUT_ANYTIME_EXCEPT_AIR, // ������ ������ ��𼭵�(�ǰ� �� ����) ��� �� �� �ִ� ��ų
	};




	enum SKILL_SLOT_ENCHANT
	{
		SSE_NONE,
		SSE_OVERFLOWING_MANA,
		SSE_STRONG_POWER,

		SSE_END,
	};


	// note: �����Ǹ� xslskilltree.h���� update
	enum SKILL_ID
	{
		SI_INVALID = -1,
		SI_NONE,

		//#pragma region ĳ���� ����
		SI_P_COMMON_PHYSIC_ATTACK_BEGINNER = 100,		/// ���� �ٷ´ܷ�
		SI_P_COMMON_MAGIC_ATTACK_BEGINNER,				/// ���� ��������
		SI_P_COMMON_PHYSIC_ATTACK_INTERMEDIATE,			/// �߱� �ٷ´ܷ�
		SI_P_COMMON_MAGIC_ATTACK_INTERMEDIATE,			/// �߱� ��������
		SI_P_COMMON_GET_CHANCE,							/// ��ȸ�� ����
		SI_P_COMMON_SUPPORT_AURA = 105,					/// �ݷ��� ���
		//#pragma endregion �н���

		//#pragma region ĳ���� ����
		SI_SA_COMMON_AURA_POWER_ACCEL = 200,			/// ���� ����
		SI_SA_COMMON_AURA_MAGIC_ACCEL,					/// ������ ����
		SI_SA_COMMON_AURA_SHIELD_ACCEL,					/// �γ��� ����
		SI_SA_COMMON_AURA_SPEED_ACCEL,					/// ������ ����
		SI_SA_COMMON_AURA_EL_DEFENCE_ACCEL,				///	��ȣ�� ����
		SI_SA_COMMON_AURA_CRITICAL_ACCEL = 205,			/// �¸��� ����
		SI_SA_COMMON_AURA_ADDATK_ACCEL,					/// ������ ����
		//#pragma endregion ����

		//#pragma region ���, ��ȥ
		SI_GP_COMMON_BALANCED_BODY					= 500,		/// �������� ��ü
		SI_GP_COMMON_HARMONIC_SPIRIT				= 501,		/// ��ȭ�ο� ��ȥ
		SI_GP_COMMON_CHEERING_OF_TEAM				= 502,		/// ������ ����
		SI_GP_COMMON_MASTER_OF_ALCHEMIST			= 503,		/// ���ݼ��� ����
		SI_GP_COMMON_CONCENTRATION_OF_SPIRIT		= 504,		/// ���� ����
		SI_GP_COMMON_CONCENTRATION_MAGICAL_POWER	= 505,		/// ���ߵ� ����
		SI_GP_COMMON_VIOLENT						= 506,		/// �ݳ�
		SI_GP_COMMON_LIBERATION_OF_ANGER			= 507,		/// �г� �ع�
		// ������� 2Ʈ��
		SI_GP_COMMON_INFINITE_STRENGTH				= 508,		/// ������ ü��
		SI_GP_COMMON_CHANCE_TO_REVERSE				= 509,		/// ������ ��ȸ
		SI_GP_COMMON_SACRIFICE_OF_HERO				= 510,		/// ������ ���
		SI_GP_COMMON_MIDAS							= 511,		/// ���̴����� ��

		SI_ETC_WS_COMMON_LOVE						= 900,		/// Ŀ�� ��ų õ������
		//#pragma endregion Ư��

		//#pragma region Elsword
		SI_SA_ES_FATAL_FURY = 1000,						/// ����Ż ǻ��
		SI_SA_ES_FLAME_GEYSER,							/// �÷��� ������
		SI_SA_ES_UNLIMITED_BLADE,						/// �𸮹�Ƽ�� �����̵�
		SI_SA_ES_TRIPLE_GEYSER,							/// Ʈ���� ������
		SI_SA_ESK_SPIRAL_BLAST,							/// �����̷� ������Ʈ
		SI_SA_ESK_ARMAGEDON_BLADE = 1005,				/// �Ƹ��ٵ� �����̵�
		SI_SA_ESK_DOUBLE_SLASH,							/// ���� ������
		SI_SA_ELK_WINDMILL,								/// �����
		SI_SA_ELK_SAND_STORM,							/// ���� ����
		SI_SA_EMK_RISING_SLASH,							/// ����¡ ������
		SI_SA_EMK_RISING_WAVE = 1010,					/// ����¡ ���̺�
		SI_SA_EMK_SWORD_FIRE,							/// �ҵ� ���̾�
		SI_SA_EMK_PHOENIX_TALON,						/// �Ǵн� �ŷ�
		SI_SA_ERS_STORM_BLADE,							/// ���� �����̵�
		SI_SA_ERS_LUNA_BLADE,							/// �糪 �����̵�
		SI_SA_ETK_FINAL_STRIKE = 1015,					/// ���̳� ��Ʈ����ũ
		SI_SA_ETK_PHANTOM_SWORD,						/// ���� �ҵ�
		SI_SA_EIS_MAELSTORM_RAGE,						/// ���Ͻ��� ������
		SI_SA_EIS_SWORD_FALL,							/// �ҵ���
		SI_SA_EIS_RAGE_CUTTER,							/// ������ Ŀ��
		//#pragma endregion Special Active

		//#pragma region Elsword
		SI_A_ES_MEGASLASH = 1200,						/// �ް� ������
		SI_A_ES_ASSAULT_SLASH,							/// �Ʈ ������
		SI_A_ES_COUNTER,								/// �ݰ�
		SI_A_ES_ENDURANCE,								/// �ر�
		SI_A_ES_KICK,									/// ������
		SI_A_ES_ROLLING = 1205,							/// ������
		SI_A_ES_SWORD_WAVE,								/// �ҵ� ���̺�
		SI_A_ES_DEFENCE,								/// ����
		SI_A_ESK_AIR_SLASH,								/// ���� ������
		SI_A_ESK_ARMOR_BREAK,							/// �Ƹ� �극��ũ
		SI_A_ESK_WEAPON_BREAK = 1210,					/// ���� �극��ũ
		SI_A_ELK_SONIC_BLADE,							/// �Ҵ� �����̵�
		SI_A_ELK_ROLLING_SMASH,							/// �Ѹ� ���Ž�
		SI_A_ELK_IMPACT_SMASH,							/// ����Ʈ ���Ž�
		SI_A_EMK_BIG_BURST,								/// ������
		SI_A_EMK_WIND_BLADE = 1215,						/// ���� �����̵�
		SI_A_ERS_SPLASH_EXPLOSION,						/// ���÷��� �ͽ��÷���
		SI_A_ERS_SWORD_ENCHANT,							/// �ҵ� ��æƮ
		SI_A_ERS_CRITICAL_SWORD,						///	ũ��Ƽ�� �ҵ�
		SI_A_ERS_RUNE_OF_FIRE,							/// �ʿ��� ��
		SI_A_ERS_RUNE_OF_ICE = 1220,					/// �Ѻ��� ��
		SI_A_ETK_CRESCENT_CUT,							/// ũ����Ʈ ��
		SI_A_ETK_SWORD_SHIELD,							/// �ҵ� �ǵ�
		SI_A_ETK_HARSH_CHASER,							/// �Ͻ� ü�̼�
		SI_A_ETK_PIERCING_SWORD,						/// �Ǿ�� �ҵ�
		SI_A_EIS_SWORD_BLASTING = 1225,					/// �ҵ� ��������
		SI_A_EIS_MIRAGE_STING,							/// �̶��� ����
		SI_A_EIS_FATAL_SLAP,							/// ����Ż ����
		//#pragma endregion Active

		//#pragma region Elsword
		SI_P_ES_POWERFUL_VITAL = 1400,					/// ������ ��ü
		SI_P_ESK_GET_MANA_UP,							/// Ȱ��
		SI_P_ESK_AUTO_GUARD,							/// ���� ����
		SI_P_ELK_STRONG_BONE,							/// �ٰ� ü��
		SI_P_ELK_REDUCE_PAIN,							/// �������
		SI_P_ELK_INTENSIVE_ATTACK = 1405,				/// ������ �ϰ�
		SI_P_ELK_INDURANCE_OF_REVENGE,					/// ������ �γ�
		SI_P_ES_POWERFUL_FIREBALL,						/// ���̾ ��ȭ
		SI_P_ERS_SPELL_CHAIN,							/// ������ �罽
		SI_P_ERS_MAGIC_RESISTANCE,						/// �������� ����
		SI_P_ERS_RUNE_MASTERY = 1410,					/// �� �����͸�
		SI_P_ETK_BRUTAL_SLAYER,							/// ��Ȥ�� �л���
		SI_P_ETK_HARSH_SLAYER,							/// ��Ȥ�� �л���
		SI_P_EIS_COMPACT_COUNTER,						/// ��ƴ ���� �ݰ�
		SI_P_EIS_MIND_OF_FIGHTER,						/// �ºλ��� ����
		SI_P_EIS_LIGHTNING_STEP = 1415,					/// ���� ��ȭ
		SI_P_EIS_CUTTING_SWORD,							/// ��ī�ο� ��
		//#pragma endregion Passive

		//#pragma region Elsword
		SI_HA_ELK_GIGANTIC_SLASH = 1600,				/// �Ⱓƽ ������
		SI_HA_ERS_SHINING_RUNE_BUSTER,					/// ���̴� �������
		SI_HA_EIS_BLADE_RAIN,							/// �����̵� ����
		//#pragma endregion Finality Skill

		//#pragma region Aisha
		SI_SA_AV_GUST_SCREW = 2000,						/// �Ž�Ʈ ��ũ��
		SI_SA_AV_LIGHTNING_BOLT,						/// ����Ʈ�� ��Ʈ
		SI_SA_AV_GUST_STORM,							/// �Ž�Ʈ ����
		SI_SA_AV_ICE_STORM,								/// ���̽� ����
		SI_SA_AHM_MAGIC_MISSILE,						/// ���� �̻���
		SI_SA_AHM_BLIZZARDSHOWER = 2005,				/// �����ڵ� ����
		SI_SA_AHM_CHAIN_LIGHTNING,						/// ü�� ����Ʈ��
		SI_SA_AHM_BLAZE_STEP,							/// �������� ����
		SI_SA_AEM_METEOR_SHOWER,						/// ���׿� ����
		SI_SA_AEM_LIGHTNING_SHOWER,						/// ����Ʈ�� ����
		SI_SA_ADM_DARK_CLOUD = 2010,					/// ��ũ Ŭ����
		SI_SA_ADM_PLASMA_CUTTER,						/// �ö�� Ŀ��
		SI_SA_ADM_DEATHFIELD,							/// �����ʵ�
		SI_SA_ADM_AGING,								/// ����¡
		SI_SA_AVP_PHANTOM_BREATHING_DARK_FALL,			/// ���� �긮�� - ��ũ��
		SI_SA_AVP_PHANTOM_BREATHING_DARK_HOLE = 2015,	/// ���� �긮�� - ��ũ Ȧ
		SI_SA_ABM_ENERGY_SPURT,							/// ������ ����Ʈ
		SI_SA_ABM_SUPER_NOVA,							/// ���� ���
		SI_SA_ABM_GUILLOTINE_PRESS,						/// ���ƾ ������
		SI_SA_ABM_MAGICAL_MAKEUP,						/// ������ ����ũ��
		SI_SA_ADW_SCREWDRIVER_TORNADO = 2020,			/// ��ũ�� ����̹� - ����̵�
		SI_SA_ADW_SCREWDRIVER_DRILLER,					/// ��ũ�� ����̹� - �帱��
		//#pragma endregion Special Active

		//#pragma region Aisha
		SI_A_AV_CHAIN_FIRE_BALL = 2200,					/// ü�� ���̾
		SI_A_AV_BINDING_CIRCLE,							/// ���ε� ��Ŭ
		SI_A_AV_TELEPORT,								/// �ڷ���Ʈ
		SI_A_AV_MANA_SHIELD,							/// ���� �ǵ�
		SI_A_AV_CIRCLE_FLAME,							/// ��Ŭ �÷���
		SI_A_AV_FIRE_ROAD = 2205,						/// ���̾� �ε�
		SI_A_AV_ICICLE_WAVE,							/// ���̽�Ŭ ���̺�
		SI_A_AHM_CYCLONE,								/// ����Ŭ��
		SI_A_AHM_MEDITATION,							/// ����
		SI_A_AEM_CHAIN_BURST,							/// ü�� ����Ʈ
		SI_A_AEM_STONE_WALL = 2210,						/// ����
		SI_A_AEM_AQUA_SHOWER,							/// ������ ����
		SI_A_AEM_SHINING_BODY,							/// ���̴� �ٵ�
		SI_A_ADM_HELL_STONE,							/// �ｺ��
		SI_A_ADM_HELL_DROP,								/// �� ���
		SI_A_ADM_MANA_INTAKE = 2215,					/// ���� ����
		SI_A_AVP_SUMMON_BAT_TIME_BOMB,					/// ���� ��ȯ - Ÿ�� ��
		SI_A_AVP_SUMMON_BAT_HEAVY_DOLL,					/// �ڶ� �Ӱ��� - ��� ��
		SI_A_AVP_SHADOW_BODY,							/// ������ �ٵ�
		SI_A_ABM_VITAL_DRAIN,							/// ����Ż �巹��
		SI_A_ABM_ENERGY_DRAIN = 2220,					/// ������ �巹��
		SI_A_ADW_DISTORTION,							/// ���� �ְ�
		SI_A_ADW_ENERGETIC_BODY,						/// ������ƽ �ٵ�
		SI_A_ADW_HEAVY_PRESS,							/// ��� ������
		SI_A_ADW_IMPACT_HAMMER,							/// ����Ʈ �ظ�
		SI_A_ADW_WORM_HOLE = 2225,						/// ��Ȧ
		//#pragma endregion Active

		//#pragma region Aisha
		SI_P_AV_FIREBALL_UP = 2400,						/// ���̾ ����
		SI_P_AHM_FREEZING_POINT_RESEARCH,				/// ���� ����
		SI_P_AHM_UNLIMIT_MANA,							/// ������ ����
		SI_P_AEM_ELEMENTAL_FRIENDSHIP,					/// ���ҿ��� ģȭ
		SI_P_AEM_ENERGY_OF_THE_PLENTY,					/// ��ȭ�� ���
		SI_P_AEM_ELEMENTAL_RESEARCH = 2405,				/// ������ ����
		SI_P_ADM_SHADOW_DATH,							/// ������ �׸���
		SI_P_ADM_SHADOW_OF_DISASTER,					/// ����� �׸���
		SI_P_AVP_LIMITED_MANA_MANAGEMENT,				/// ������ ���� ���
		SI_P_AVP_STRONG_MIND,							/// ����� ���
		SI_P_AVP_SPRINTER = 2410,						/// ��������
		SI_P_AVP_MAGIC_AMPLIFICATION,					/// ���� ����
		SI_P_AVP_GRAVITY_BALL_RESEARCH,					/// �׶��Ƽ �� ����
		SI_P_ABM_FITNESS,								/// ��ü �ܷ�
		SI_P_ABM_MANAFLOW_OVERDRIVE,					/// ���� ����
		SI_P_ADW_SPIRIT_ACCELERATION  = 2415,			/// ��� ����
		SI_P_ADW_ADVANCED_TELEPORTATION,				/// ���� ����
		SI_P_ADW_MAGICAL_STAFF,							/// ������ ������
		//#pragma endregion Passive

		//#pragma region Aisha
		SI_HA_AEM_ELEMENTAL_STORM = 2600,				/// ������Ż ����
		SI_HA_AVP_ABYSS_ANGOR,							/// ��� �Ӱ���
		SI_HA_ADW_FATE_SPACE,							/// ����Ʈ �����̽�
		//#pragma endregion Finality Skill

		//#pragma region Lena
		SI_SA_LE_PERFECT_STORM = 3000,					/// ����Ʈ ����
		SI_SA_LE_MULTIPLE_STINGER,						/// ��Ƽ�� ���ð�
		SI_SA_LE_AERO_TORNADO,							/// ����� ����̵�
		SI_SA_LE_PHOENIX_STRIKE,						/// �Ǵн� ��Ʈ����ũ
		SI_SA_LCR_CRESCENT_KICK,						/// ũ����Ʈ ű
		SI_SA_LCR_DIVE_KICK_BOMBING = 3005,				/// ���̺� ű ����
		SI_SA_LCR_VIOLENT_ATTACK,						/// ���̿÷�Ʈ ����
		SI_SA_LWS_SHARPFALL,							/// ���� ��
		SI_SA_LWS_AIRELINNA_NYMPH,						/// ���̷�����- ����
		SI_SA_LWS_AIRELINNA_SYLPH,						/// ���̷����� - ����
		SI_SA_LSR_GUIDED_ARROW = 3010,					/// ���̵� �ַο�
		SI_SA_LSR_CRAZY_SHOT,							/// ũ������ ��
		SI_SA_LSR_WINDWARD,								/// ����͵�
		SI_SA_LSR_GUNGNIR,								/// �ñ״�
		SI_SA_LGA_AERO_STRAFE,							/// ����� ��Ʈ������
		SI_SA_LTR_FATALITY = 3015,						///	����Ż��Ƽ
		SI_SA_LTR_TRAPPING_ARROW_BURST,					///	Ʈ���� �ַο� - ����Ʈ
		SI_SA_LTR_TRAPPING_ARROW_FUNGUS,				/// Ʈ���� �ַο� - ��Ž�
		SI_SA_LTR_CALL_OF_RUIN,							///	�� ���� ����
		SI_SA_LTR_KARMA,								///	ī����
		SI_SA_LNW_GLIDING_STRIKE = 3020,				///	�۶��̵� ��Ʈ����ũ
		//#pragma endregion Special Active

		//#pragma region Lena
		SI_A_LE_RAIL_STINGER = 3200,					/// ���� ���ð�
		SI_A_LE_ASSAULT_KICK,							/// �Ʈ ű
		SI_A_LE_BACKJUMP_SHOT,							/// ������� ��
		SI_A_LE_REFLEX_MAGIC,							/// �����ݻ�
		SI_A_LE_RISING_FALCON,							///	����¡ ����
		SI_A_LE_SIEGEMODE = 3205,						/// ���� ���
		SI_A_LCR_SPINNING_KICK,							/// ���Ǵ� ű
		SI_A_LCR_LOW_KICK,								/// �ο�ű
		SI_A_LCR_MIDDLE_KICK,							/// �̵�ű
		SI_A_LWS_NATURE_FORCE,							/// �ڿ��� ��
		SI_A_LWS_SLIDE_KICK = 3210,						/// �����̵� ���� ű
		SI_A_LWS_ASSAULT_IMPACT,						/// �Ʈ ����Ʈ
		SI_A_LWS_BACK_KICK,								/// ��ű
		SI_A_LWS_HIGH_KICK,								/// ����ű
		SI_A_LSR_ENTANGLE,								/// ���ʱ� - ��Ÿ���� ������
		SI_A_LSR_SHOOTING_MAGNUM = 3215,				/// ���� �ű׳�
		SI_A_LSR_HUMMING_WIND,							/// ��� ����
		SI_A_LGA_FREEZING_ARROW,						/// ����¡ �ַο�
		SI_A_LGA_FREEZING_BOLT,							/// ����¡ ��Ʈ
		SI_A_LGA_ARC_SHOT,								/// ��ũ ��
		SI_A_LGA_RAPID_SHOT = 3220,						/// ���ǵ� ��
		SI_A_LGA_WIND_BLAST,							/// ���� ������Ʈ
		SI_A_LGA_STIGMA_ARROW,							/// ������ ���
		SI_A_LTR_EVOKE,									///	�̺�ũ
		SI_A_LTR_ENTANGLE,								/// ������ ����
		SI_A_LTR_EXPLOSION_TRAP = 3225,					///	������ ��
		SI_A_LNW_FURIOUS_ENGAGE,						///	ǻ��� �ΰ�����
		SI_A_LNW_DELAYED_FIRING,						///	������ ��ȣź
		SI_A_RNW_THORNS_TRAP,							/// ���õ��� ��
		SI_A_RNW_ROSEBUSH_TRAP,							/// �� ��
		SI_A_RNW_SEED_OF_RUIN = 3230,					/// ������ ����
		SI_A_RNW_ANGER_OF_ELF,							/// ��������� �г�
		//#pragma endregion Active

		//#pragma region Lena
		SI_P_LCR_SHARP_KICK = 3400,						/// ������ ������
		SI_P_LCR_AGILE_MOVEMENT,						/// ����� ���
		SI_P_LCR_SOFTBODY,								/// �����Ѹ��
		SI_P_LWS_POWERFUL_SHOT,							/// �� �ִ� ���
		SI_P_LWS_FRIENDSHIP_OF_NATURE,					/// �ڿ����� ģȭ
		SI_P_LSR_POWERFUL_BOWSTRING = 3405,				/// ��ȭ�� ���콺Ʈ��
		SI_P_LSR_HAWK_EYE,								/// ȣũ����
		SI_P_LSR_SHARP_ARROW,							/// �ٵ���� ȭ��
		SI_P_LGA_COMMUNE_OF_NATURE,						/// �ڿ��� ����
		SI_P_LGA_VIBRATION_SHOOTING,					/// ���� ���
		SI_P_LTR_HUNTERS_ABILITY = 3410,				///	��ɲ��� ���
		SI_P_LTR_SHARPEN_ARROW,							///	��ī�ο� ȭ��
		SI_P_LNW_VITALPOINT_PIERCING,					///	�޼� ���
		SI_P_LNW_PRIOR_PLANNED_BLOW,					///	���� �ϰ�
		//#pragma endregion Passive

		//#pragma region Lena
		SI_HA_LWS_SPIRAL_STRIKE = 3600,					/// �����̷� ��Ʈ����ũ
		SI_HA_LGA_CRYOTRON_BOLT,						/// ũ���ƿ�Ʈ�� ��Ʈ
		SI_HA_LNW_INNOCENT,								/// �̳뼾Ʈ
		//#pragma endregion Finality Skill

		//#pragma region Raven
		SI_SA_RF_POWER_ASSAULT = 4000,					/// �Ŀ� �Ʈ
		SI_SA_RF_CANNON_BLADE,							/// ĳ�� �����̵�
		SI_SA_RF_SEVEN_BURST,							/// ���� ����Ʈ
		SI_SA_RST_HYPER_SONIC_STAB,						/// ������ �Ҵн���
		SI_SA_RST_SHOCKWAVE,							/// ��ũ ���̺� - ��
		SI_SA_RST_SHOCKWAVE_FRONT = 4005,				/// ��ũ ���̺� - ��
		SI_SA_RST_BLOODY_ACCEL,							/// ������ �׼�
		SI_SA_RBM_FLYING_IMPACT,						/// �ö��� ����Ʈ
		SI_SA_RBM_GIGA_DRIVE_LIMITER,					/// �Ⱑ ����̺�- ������
		SI_SA_RBM_GIGA_DRIVE_SEISMIC_TREMOR,			/// �Ⱑ ����̺� - ������� Ʈ����
		SI_SA_ROT_ARC_ENEMY = 4010,						/// ��ũ �ֳʹ�
		SI_SA_ROT_GUARDIAN_STRIKE,						/// ����� ��Ʈ����ũ
		SI_SA_ROT_VALKYRIESJAVELIN,						/// ��Ű���� �ں���
		SI_SA_ROT_NUCLEAR,								/// ��Ŭ����
		SI_SA_RRF_WILD_CHARGE,							/// ���ϵ� ����
		SI_SA_RWT_BURSTING_BLADE = 4015,				/// ������ �����̵�
		SI_SA_RWT_REVOLVER_CANNON_ORDNANCE_PENETRATOR,	/// ������ ĳ�� - OPź
		SI_SA_RWT_REVOLVER_CANNON_HIGH_EXPLOSIVE_SHELL,	/// ������ ĳ�� - HEź
		SI_SA_RWT_HARPOON_SPEAR,						/// ��Ǭ ���Ǿ�
		SI_SA_RWT_HELLFIRE_GATLING,						/// �����̾� ��Ʋ��
		SI_SA_RWT_GIGA_PROMINENCE = 4020,				/// �Ⱑ ���ι̳ͽ�
		SI_SA_RVC_DEADLY_RAID,							/// ���鸮 ���̵�
		SI_SA_RVC_IGNITION_CROW_NAPALM,					/// �̱״ϼ� ũ�ο� - ������
		SI_SA_RVC_IGNITION_CROW_INCINERATION,			/// �̱״ϼ� ũ�ο� - �νóʷ��̼�
		//#pragma endregion Special Active

		//#pragma region Raven
		SI_A_RF_MAXIMUM_CANNON = 4200,					/// �ƽø� ĳ��
		SI_A_RF_EARTH_BREAKER,							/// � �극��ũ
		SI_A_RF_MEGADRILL_BREAK,						/// �ް� �帱 �극��ũ
		SI_A_RF_BURNING_RUSH,							/// ���׷���
		SI_A_RF_GROUND_IMPACT,							/// �׶��� ����Ʈ
		SI_A_RF_SHADOW_STEP = 4205,						/// �����콺��
		SI_A_RF_BREAKING_FIST,							/// �극��ŷ �ǽ�Ʈ
		SI_A_RST_BERSERKER_BLADE,						/// ����Ŀ �����̵�
		SI_A_RST_CUT_TENDON,							/// ���ٴ�
		SI_A_RBM_ONE_FLASH,								/// �ϼ�
		SI_A_RBM_SONIC_SLASH = 4210,					/// �Ҵ� ������
		SI_A_RBM_WOFL_FANG,								/// ������
		SI_A_ROT_CHARGED_BOLT_HEDGHOG,					/// ������ ��Ʈ - ����ȣ��
		SI_A_ROT_CHARGED_BOLT_BLOOD,					/// ������ ��Ʈ - ���� ������
		SI_A_RRF_HELL_DIVE,								/// �� ���̺�
		SI_A_ROT_WEAPON_BREAK = 4215,					/// ���� ����
		SI_A_ROT_ARMOR_BREAK,							/// �� ����
		SI_A_RRF_X_CRASH,								/// ���� ũ����
		SI_A_RRF_LIMIT_CRUSHER,							/// ����Ʈ ũ����
		SI_A_RWT_BARRAGE_ATTACK,						/// ���� ����
		SI_A_RWT_FLAME_SWORD = 4220,					/// ȭ����
		SI_A_RVC_BLEEDING_SLICER,						/// ġ����
		SI_A_RVC_NAPALM_GRENADE,						/// ������ �׷����̵�
		//#pragma endregion Active

		//#pragma region Raven
		SI_P_RF_SHADOW_PIERCING = 4400,					/// �������Ǿ��
		SI_P_RF_HARDEN_BODY,							/// �ܷõȽ�ü
		SI_P_RF_INDUCE_DESIRE_WINNING,					/// ȣ�½�����
		SI_P_RST_BLADE_MASTERY,							/// ���õ� �˼�
		SI_P_RST_EXQUISITE_SWORDMANSHIP,				/// ������ �˼�
		SI_P_RBM_REVERSAL_SOLDIER = 4405,				/// �����ǿ��
		SI_P_RBM_SHADOW_THRUST,							/// ������ ������Ʈ
		SI_P_RBM_EMERGENCY_ESCAPE,						/// ��� Ż��
		SI_P_RRF_BURNING_NASOD_HAND,					/// Ÿ������ ���ҵ� �ڵ�
		SI_P_RRF_SPIRITUALIZED_FURY,					/// ��ȭ�� �г�
		SI_P_RRF_EXPLOSIVE_NASOD_HAND = 4410,			/// �����ϴ� ���ҵ� �ڵ�
		SI_P_RRF_SHADOW_PUNISHER,						/// ������ �۴ϼ�
		SI_P_RRF_CONFIDENCE_VICTORY,					/// �¸����ڽŰ�
		SI_P_RWT_BEHAVIOR_OF_MERCENARY,					/// �뺴�� ó�ż�
		SI_P_RVC_SHADOW_BACK_SLIDE,						/// ������ �� �����̵�
		SI_P_RVC_SURVIVAL_TECHNIQUE_OF_MERCENARY = 4415,/// �뺴�� ���� ����
		SI_P_RVC_WINNABLE_TECHNIQUE_OF_MERCENARY,		/// �뺴�� �¸�����
		SI_P_RVC_AIR_SLICER,							/// ���� �����̼�
		SI_P_RVC_OVERHEAT_MODE,							/// ������� �۵�!
		//#pragma endregion Passive

		//#pragma region Raven
		SI_HA_RBM_EXTREM_BLADE = 4600,					/// �ͽ�Ʈ�� �����̵�
		SI_HA_RRF_INFERNAL_ARM,							/// ���丣�� ����
		SI_HA_RVC_BURNING_BUSTER,						/// ���� ������
		//#pragma endregion Finality Skill

		//#pragma region Eve
		SI_SA_EN_ILLUSION_STINGER = 5000,				/// �Ϸ��� ���þ�
		SI_SA_EN_DIMENSION_LINK_BLADER,					/// ���� ��ũ - �����̴�
		SI_SA_EN_GENERATE_BLACKHOLE,					/// ���׷����� ����Ȧ
		SI_SA_EN_DIMENSION_LINK_GUARDIAN,				/// ���� ��ũ - �����
		SI_SA_EEG_EXPLOSION_IMPACT,						/// �ͽ��÷��� ����Ʈ
		SI_SA_EEG_QUEENS_THRONE = 5005,					/// ���� ����
		SI_SA_EEG_JUNK_BREAK,							/// ��ũ �극��ũ
		SI_SA_ENS_IRONSCRAPS,							/// ���̾� ��ũ����
		SI_SA_ENS_ATOMIC_SHIELD,						/// ����� ����
		SI_SA_ENS_ASSUALT_SPEAR_ANNIHILATOR,			/// �Ʈ ���Ǿ� - ��̾�����
		SI_SA_EAT_SONIC_WAVE = 5010,					/// �Ҵ� ���̺�
		SI_SA_EAT_GENOCIDE_RIPPER,						/// ������̵� ����
		SI_SA_EAT_HEAVENS_FIST_PRESSURE,				/// �غ��� �ǽ�Ʈ - ������
		SI_SA_EAT_HEAVENS_FIST_SWEEPER,					/// ����� �ǽ�Ʈ - ������
		SI_SA_EEP_SPACE_WRENCH,							/// ������ ����ó
		SI_SA_EEP_ASSUALT_SPEAR_BURST = 5015,			/// �Ʈ ���Ǿ� - ������
		SI_SA_EEL_PARTICLE_RAY,							/// ��ƼŬ ����
		SI_SA_EEL_SWEEP_ROLLING_OVER_CHARGE,			/// ������ �Ѹ� - ��������
		SI_SA_EEL_SWEEP_ROLLING_TRI_VULCAN,				/// ������ �Ѹ� - Ʈ���� ��ĭ
		SI_SA_EEL_PHOTON_FLARE,							/// ���� �÷���
		SI_SA_EEL_GIGA_STREAM = 5020,					/// �Ⱑ ��Ʈ��
		SI_SA_EEL_THOUSANDS_OF_STARS,					/// ������� ��Ÿ
		SI_SA_EBS_ENERGY_NEEDLES,						/// ������ �ϵ齺
		SI_SA_EBS_ENERGETIC_HEART,						/// ������ƽ ��Ʈ
		SI_SA_EEL_LINEAR_DIVIDER,						/// ���Ͼ� ����̴�
		//#pragma endregion Special Active

		//#pragma region Eve
		SI_A_EN_ILLUSION_STRIKE = 5200,					/// �Ϸ��� ��Ʈ����ũ
		SI_A_EN_DIMENSION_LINK,							/// ���� ��ũ - ��Ʈ��
		SI_A_EN_CLOAKING,								/// Ŭ��ŷ
		SI_A_EN_CONVERSION,								/// ���� ��ȯ
		SI_A_EN_MEGA_ELECTRONBALL,						/// �ް� �Ϸ�Ʈ�к�
		SI_A_EN_ILLUSION_RAY = 5205,					/// �Ϸ��� ����
		SI_A_EN_TESLA_SHOCK,							/// �׽��� ��ũ
		SI_A_EN_PHOTON_BLINK,							/// ���� ����
		SI_A_EEG_HORNET_STING_EXPLOSION,				/// ȣ�� ���� - �ͽ�������
		SI_A_EEG_HORNET_STING_SHAPED_CHARGE,			/// ȣ�� ���� - ������Ʈ ����
		SI_A_EEG_METAL_DUST_AURA = 5210,				/// ��Ż ����Ʈ �ƿ��
		SI_A_ENS_ATOMIC_BLASTER_PLASMAGUN,				/// ����� �������� - �ö�� ��
		SI_A_ENS_ATOMIC_BLASTER_SONICGUN,				/// ����� �������� - �Ҵ� ��
		SI_A_ENS_SPEAR_BURST,							/// ���Ǿ� ����Ʈ
		SI_A_EAT_SURFACE_CUTING,						/// ���ǽ� Ŀ��
		SI_A_EAT_OBERON_GUARD = 5215,					/// ������ ����
		SI_A_EEP_SPIT_FIRE_FLUSH,						/// ���� ���̾� - �÷���
		SI_A_EEP_SPIT_FIRE_GRENADE,						/// ���� ���̾� - �׸����̵�
		SI_A_EEP_ELECTRONIC_FIELD,						/// �Ϸ�Ʈ�δ� �ʵ�
		SI_A_EEL_SPECTRO_EL_CRYSTAL,					/// �� �б� ����
		SI_A_EBS_TASER_PILUM = 5220,					/// ������ �ʶ� (������ ����)
		SI_A_EBS_KUGELBLITZ,							/// ���ۺ�����
		//#pragma endregion Active

		//#pragma region Eve
		SI_P_EN_ELECTRONBALL_UP = 5400,					/// �Ϸ�Ʈ�к� ����
		SI_P_EN_QUEENS_CONTROL,							/// ������ �����
		SI_P_EN_QUEENS_POTENTIAL,						/// ������ �����
		SI_P_EN_REFINED_STEP,							/// ����ѹ߰���
		SI_P_EN_PHOTON_BOOSTER,							/// ���� ������
		SI_P_ENS_NASOD_WEAPON = 5405,					/// ���ҵ� ���� ��ȭ
		SI_P_ENS_ENERGY_OF_CONCENTRATION,				/// ������ ���
		SI_P_EEP_ELECTRONBALL_MASTER,					/// �Ϸ�Ʈ�� ��������
		SI_P_EEP_CHARGING_BOOSTER,						/// ����������
		SI_P_EEL_SPECTRUM_DYNAMICS,						/// �б� ����
		SI_P_EEL_ADVANCED_EL_ENERGY_REACTOR = 5410,		/// ��ȭ �� ������ ������
		SI_P_EBS_HYPER_OPTICAL_RESEARCH,				/// �ʱ��� ����
		SI_P_EBS_HIGHPOWERED_ELECTRICS,					/// ����� ����ȸ��
		SI_P_EBS_AMPLIFIED_EL_ENERGY_REACTOR,			/// ���� �� ������ ������
		//#pragma endregion Passive

		//#pragma region Eve
		SI_HA_ENS_LUNATIC_SCUD = 5600,					/// �糪ƽ ��Ŀ��
		SI_HA_EEP_LINK_OVERCHARGE_ILLUSION,				/// ��ũ �������� �Ϸ���
		SI_HA_EBS_PSYCHIC_ARTILLERRY,					/// ����ű ��ƿ����( �̷����� ĳ�� )
		//#pragma endregion Finality Skill

		//#pragma region Chung
		SI_SA_CC_LUNATIC_BLOW = 6000,					/// �糪ƽ ���ο�
		SI_SA_CC_SCARE_CHASE,							/// ���ɾ� ü�̽�
		SI_SA_CC_GIGANTIC_IMPACT,						/// �Ⱓƽ ����Ʈ
		SI_SA_CC_ACELDAMA,								/// �ưִٸ�
		SI_SA_CFG_LUNATIC_FURY,							/// �糪ƽ ǻ��
		SI_SA_CFG_PANDEMONIUM_CHAOS = 6005,				/// �ǵ���Ͽ� - ī����
		SI_SA_CFG_PANDEMONIUM_FEAR,						/// �ǵ���Ͽ� - �Ǿ�
		SI_SA_CFG_PAIN_OF_CALADBOLG,					/// Į�󺼱� ����
		SI_SA_CIP_IRON_HOWLING,							/// ���̾� �Ͽ︵
		SI_SA_CIP_LAND_DEMOLISHIER_HEAVY_ARMS,			/// ���� ������� - ��� ����
		SI_SA_CIP_LAND_DEMOLISHER_EARTHQUAKE = 6010,	/// ���� ������� - � ����ũ
		SI_SA_CSG_ARTILLERY_STRIKE_QUANTUM_BALLISTA,	/// ��ƿ���� ��Ʈ����ũ - ���� �߸���Ÿ
		SI_SA_CSG_HEAVY_RAILGUN,						/// ����ϰ�
		SI_SA_CSG_SHOOTING_STAR,						/// ���� ��Ÿ
		SI_SA_CDC_COMET_CRASHER,						/// ī�� ũ����
		SI_SA_CDC_SHARPSHOOTER_SYNDROME = 6015,			/// �������� ��Ʈ��
		SI_SA_CHG_BIGBANG_STREAM,						/// ��� ��Ʈ��
		SI_SA_CHG_CHAOS_CANNON,							/// ī���� ĳ��
		SI_SA_CHG_CARPET_BOMBING,						/// ī�� �ٹ�
		SI_SA_CTT_ARTILLERY_STRIKE,						/// ��ƿ���� ��Ʈ����ũ - �̽� ����
		SI_SA_CTT_DREAD_CHASE = 6020,					/// �巹�� ü�̽�
		SI_SA_CTT_TACTICAL_FIELD_INDURANCE,				/// ��Ƽ�� �ʵ� - �ε෱��
		SI_SA_CTT_TACTICAL_FIELD_RAID,					/// ��Ƽ�� �ʵ� - ���̵�
		//#pragma endregion Special Active

		//#pragma region Chung
		SI_A_CC_BRUTAL_SWING = 6200,					/// ���Ż ����
		SI_A_CC_DETONATION,								/// ������̼�
		SI_A_CC_IMPACT_DETONATION,						/// ����Ʈ ������̼�
		SI_A_CC_RELOAD_CANNON,							/// ź��
		SI_A_CC_AIMMING_SHOT,							/// ���̹� ��
		SI_A_CC_SIEGE_SHELLING = 6205,					/// ���� ����
		SI_A_CFG_STEEL_EDGE,							/// ��ƿ ����
		SI_A_CFG_GUARD,									/// ����
		SI_A_CFG_LEAP_ATTACK,							/// ��������
		SI_A_CIP_SUDDEN_BURSTER,						/// ���� ������
		SI_A_CIP_ARTILLERY_NOVA = 6210,					/// ��ƿ���� ���
		SI_A_CIP_BURST_WOLF,							/// ����Ʈ ����
		SI_A_CSG_DOUBLE_FIRE,							/// ���� ���̾�
		SI_A_CSG_DUAL_BUSTER,							/// ��� ������
		SI_A_CSG_HEAD_SHOT,								/// ��弦
		SI_A_CSG_MAGNUM_SHOT = 6215,					/// �ű׳� ��
		SI_A_CDC_RUMBLE_SHOT,							/// ������
		SI_A_CDC_BULLET_BLITZ,							/// �ҷ� ������
		SI_A_CDC_GATLING_SHOT,							/// ��Ʋ�� �� ( ���ǵ� ĳ�� )
		SI_A_CDC_LEG_SHOT,								/// ���׼�
		SI_A_CHG_CANNON_STRIKE = 6220,					/// ĳ�� ��Ʈ����ũ
		SI_A_CHG_ELASTIC_BOMB_GRENADE,					/// �Ϸ���ƽ ��
		SI_A_CHG_ELASTIC_BOMB_LAUNCHER,					/// �Ϸ���ƽ �� - �׸����̵� ��ó
		SI_A_CHG_MARK_OF_COMMANDER,						/// ���ְ��� ǥ��
		SI_A_CTT_WONDER_WALL,							/// ���� ��
		SI_A_CTT_AUTOMATIC_MORTAR = 6225,				/// �ڰ���
		SI_A_CTT_BOMBARD_SERVICE,						/// ���� ����
		//#pragma endregion Active

		//#pragma region Chung
		SI_P_CFG_METABOLISM_BOOST = 6400,				/// ������� ����
		SI_P_CFG_GUARD_MASTERY,							/// ������
		SI_P_CFG_ELEMENTAL_DEFENCE_A,					/// �Ұ� ���� �ڿ�
		SI_P_CFG_ELEMENTAL_DEFENCE_B,					/// �ٶ��� �ؿ� ��
		SI_P_CIP_ADVANCED_METABOLISM_BOOST,				/// ������� ���� ��ȭ
		SI_P_CIP_IRON_WILL = 6405,						/// ������ ����
		SI_P_CSG_REMODEL_MAGAZINE,						/// źâ/��� ����
		SI_P_CSG_REMODEL_RIFLE,							/// ���� ����
		SI_P_CDC_TUSSLE_TECHNIQUE,						/// ������ ���
		SI_P_CDC_ACCURATE_MARKMANSHIP,					/// ������ ��ݼ�
		SI_P_CDC_INTENSE_SHOWTIME = 6410,				/// ������ ��Ÿ��
		SI_P_CDC_GAS_PISTON_SYSTEM,						/// ���� �ǽ��� �ý���
		SI_P_CHG_RELOAD_MASTERY,						/// ���ε� �����͸�
		SI_P_CHG_CONVERTED_WARHEAD,						/// ź�� ����
		SI_P_CHG_CANNONEER_WITHSTANDING,				/// ������ ��Ƽ��
		SI_P_CTT_MOBILE_ARMORED_MODE = 6415,			/// �̵� ����
		SI_P_CTT_REACTIVE_ARMOR,						/// ���� �尩
		//#pragma endregion Passive

		//#pragma region Chung
		SI_HA_CIP_DOOM_STRIKER = 6600,					/// �� ��Ʈ����ũ
		SI_HA_CDC_OUTRAGE_STRIKE,						/// �ƿ������� ��Ʈ����ũ
		SI_HA_CTT_SATELITE_RAIN,						/// ��Ʋ����Ʈ ����
		//#pragma endregion Finality Skill

		//#pragma region Ara
		SI_SA_AM_PULLING_PIERCE = 7000,					/// õ��_���� �α�
		SI_SA_AM_TEMPEST_DANCE,							/// ��ǳ_�ָ�
		SI_SA_AM_FALLING_DRAGON,						/// ������_���� ������
		SI_SA_AM_SHADOW_KNOT,							/// �῵_�׸��� �ŵ�_�ͻ� 3��
		SI_SA_ALH_MOONLIGHT_SLASH,						/// ��� 4�� - �޺� ����
		SI_SA_ALH_PEERLESSNESS_SLASH = 7005,			/// ������
		SI_SA_ALH_WHITE_TIGER,							/// ��ȣ��
		SI_SA_ALH_SWALLOW_ASSAULT,						/// ��
		SI_SA_ASD_SUPPRESSION,							/// ����
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // ����ȯ
		SI_SA_ALD_FINGER_BULLET,						/// ���� 1�� ��ȥ
		SI_SA_ALD_WOLF_SPEAR = 7010,					/// ���� 4�� ���� �̻�
		SI_SA_ALD_ENERGY_WAVE,							/// �����
		SI_SA_AYR_SUPPRESSION_ENERGY,					/// ���� : ��
		SI_SA_ALD_ENERGY_BEAD,							/// ���� 2�� ��ȯ��
		SI_SA_ALD_HELL_OF_TORNADO,						/// ��ȯ����
		SI_SA_ALD_PRISON_SPEAR = 7015,					/// ���� 3�� â�차��
		SI_SA_AYR_ENERGY_VOID,							/// ��������
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		//#pragma endregion Special Active

		//#pragma region Ara
		SI_A_AM_WIND_WEDGE = 7200,						/// ��ǳ��_�ٶ� ����_���� 2��
		SI_A_AM_SHADOW_RISING,							/// ���·�_õ�� �ɸ���
		SI_A_AM_TIGER_CLAW,								/// ȣ����_����_��ȣ 2��
		SI_A_AM_FORCE_SPEAR,							/// ���â
		SI_A_AM_GAIN_FORCE,								/// ����
		SI_A_AM_QUICK_PIERCING = 7205,					/// �ż�
		SI_A_AM_SHOULDER_BLOW,							/// ö���
		SI_A_AM_QUICK_STAB,								/// õ��
		SI_A_ALH_ROCK_CRASH,							/// ��ȣ 1�� - ��������
		SI_A_ALH_BREAKING,								/// ��ȭ����
		SI_A_ALH_DOUBLE_COLLISION = 7210,				/// ��� 3�� - �ȱ���
		SI_A_ASD_DRAGON_FALLING,						/// ��ȭ
		SI_A_ASD_TURBULENT_WAVE,						/// ��ȣ 4�� - �뵵
		SI_A_ASD_LOW_BRANDISH,							/// ��ȣ 3�� - ����
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // ����ȯ
		SI_A_ALD_WOLF_CLAW,								/// ���� 3�� ���� ����
		SI_A_ALD_CONVERSION_HEALTH = 7215,				/// ������
		SI_A_AYR_HIGH_SPEED,							/// ���
		SI_A_ALD_CONVERSION_MANA,						/// �̱��
		SI_A_AYR_STEAL_SOUL,							/// ���� 4�� Żȥ
		SI_A_ALD_REFLECTION,							/// ��ź��
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		//#pragma endregion Active

		//#pragma region Ara
		SI_P_ALH_QUICK_STAND = 7400,					/// ����� ���
		SI_P_ALH_VIGOR_ENERGIZE,						/// ��� Ȱ��ȭ
		SI_P_ALH_FILLED_POWER,							/// �游�� ��
		SI_P_ASD_QUICKEN_GUARD,							/// �����̴� ����
		SI_P_ASD_BRUTAL_PIERCING,						/// â���� ����
		SI_P_ASD_FULLMOON_SLASH = 7405,					/// ���� ����
		SI_P_ASD_POWER_OF_TIGER,						/// ��ȣ�� ��
		SI_P_ASD_SELF_PROTECTION_FORTITUDE,				/// ȣ�Ű���
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // ����ȯ
		SI_P_AYR_ENERGY_BULLET_REINFORCE,				/// ��ź ��ȭ
		SI_P_AYR_HOWLING,								/// ������ ���¢��
		SI_P_AYR_RESURRECTION = 7410,					/// ȸ������
		SI_P_ALD_ENERGY_REINFORCE,						/// ��� ��ȭ
		SI_P_AYR_HELL_GATE_OPEN,						/// ���� �ʷ�
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		//#pragma endregion Passive

		//#pragma region Ara
		SI_HA_ASD_THOUSANDS_BLOSSOMS = 7600,			/// �б��õ - õ��ȭ
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // ����ȯ
		SI_HA_AYR_ENERGY_CANNON,						/// ���ȯ����
//#endif // SERV_ARA_CHANGE_CLASS_SECOND
		//#pragma endregion Finality Skill


//#pragma region �����ý�
		SI_SA_EL_WILD_SHOCK							= 8000,		/// ���ϵ� ��ũ
		SI_SA_EL_POWER_BURSTER						= 8001,		/// �Ŀ� ������
		SI_SA_EL_MEGA_BURSTER						= 8002,		/// �ް� ������
		SI_SA_EL_UNLIMITED_BLADE					= 8003,		/// �𸮹�Ƽ�� �����̵�

		SI_SA_SSK_SPIRAL_BLAST						= 8004,		/// �����̷� ������Ʈ
		SI_SA_SSK_EXTINCTION						= 8005,		/// ���̹� - �ͽ��ü�
		SI_SA_SSK_VICTORIOUS_SWORD					= 8006,		/// �¸��� ��
		SI_SA_SSK_JUDGEMENT_DRIVE					= 8007,		/// ������Ʈ ����̺�

		SI_SA_SPK_BURST_RISING						= 8008,		/// ����Ʈ ����¡
		SI_SA_SPK_BURST_WAVE						= 8009,		/// ����Ʈ ���̺�
		SI_SA_SPK_SWORDFIRE							= 8010,		/// �ҵ� ���̾�
		SI_SA_SPK_INFERNAL_BLADE					= 8011,		/// ���丣�� �����̵�
		SI_SA_SPK_ETERNAL_FIRE						= 8012,		/// ���ͳ� ���̾�
		SI_SA_SGM_SONIC_BLADE						= 8013,		/// �Ҵ� �����̵�
		SI_SA_SGM_CRITICAL_DIVE						= 8014,		/// ũ��Ƽ�� ���̺�
		SI_SA_SBH_SCALDIC_SWORD						= 8015,		/// ��Į�� �ҵ�
		SI_SA_SBH_WALL_OF_PROMINENCE				= 8016,		/// ���̾� ��
		SI_SA_SGM_JUGGERNAUT_BUSTER					= 8017,		/// ���ų� ������
		SI_SA_SGM_WAR_PRELUDE						= 8018,		/// �� ��������
		SI_SA_SBH_BLAZE_WING						= 8019,		/// �������� ��
		SI_SA_SBH_BLAZING_DANCE						= 8020,		/// ������¡ ����

		SI_A_EL_DODGE_AND_SLASH						= 8200,		/// ����&������
		SI_A_EL_LEAP_CRASH							= 8201,		/// ����
		SI_A_EL_RUSHING_SWORD						= 8202,		/// ���� �ҵ�
		SI_A_EL_KICK								= 8203,		/// ������
		SI_A_EL_MEGA_SLASH							= 8204,		/// �ް� ������
		SI_A_EL_SONIC_ASSAULT_STAB					= 8205,		/// �Ҵ� �Ʈ - ����
		SI_A_EL_SONIC_ASSAULT_STING					= 8206,		/// �Ҵ� �Ʈ - ����

		SI_A_EL_ENDURANCE_POWER						= 8207,		/// �ر� - ��

		SI_A_SSK_CROSS_SLASH						= 8208,		/// ũ�ν� ������
		SI_A_SSK_POWER_BREAK						= 8209,		/// �Ŀ� �극��ũ
		SI_A_SSK_HEAVY_STUNNER						= 8210,		/// ��� ���ͳ�

		SI_A_SPK_SWORD_ERUPTION						= 8211,		/// �ҵ� �̷���
		SI_A_SPK_SCALDIC_SWORD						= 8212,		/// �Ҳ��� ��
		SI_A_SPK_BIG_BURST							= 8213,		/// ������
		SI_A_SGM_PROVOKE							= 8214,		/// ����
		SI_A_SGM_DRASTIC_CHARGE						= 8215,		/// �巡��ƽ ����
		SI_A_SGM_SNAP_CRASHER						= 8216,		/// ���� ũ����
		SI_A_SBH_FLAME_SWORD						= 8217,		///	ȫ���� ��
		SI_A_SBH_FIREWORKS							= 8218,		/// �ҳ���
		SI_A_SBH_SCALET_ROSE						= 8219,		/// �Ҳ� ���

		SI_P_SSK_AGILITY_MASTERY					= 8400,		/// ���߷� ���
		SI_P_SSK_CHIVALRY							= 8401,		/// ����� ����
		SI_P_SPK_CONCENTRATION_MASTERY				= 8402,		/// ���߷� ���
		SI_P_SPK_BODY_OF_FIRE						= 8403,		/// ���� ��ü
		SI_P_SGM_ADVANCED_KNIGHT_MASTERY			= 8404,		/// ��� ��� ����
		SI_P_SGM_WALTZ_OF_SWORD						= 8405,		/// ���� ����
		SI_P_SGM_FIRST_STRIKE						= 8406,		/// �����ʽ�
		SI_P_SBH_FIRE_BLOSSOMS						= 8407,		/// �Ҳ� ��ȭ
		SI_P_SBH_UNEXTINGUISHABLE_FIRE				= 8408,		/// ������ �ʴ� �Ҳ�
		SI_P_SBH_STIGMA_OF_FIRE						= 8409,		/// �Ҳ��� ����

		SI_HA_SGM_SWORD_OF_RELICS					= 8600,		/// �ҵ� ���� ������
		SI_HA_SBH_ANCIENT_FIRE						= 8601,		/// ���μ�Ʈ ���̾�

//#pragma endregion ��ų


//#pragma region �ֵ�

		/// ������ ��Ƽ��
		SI_SA_AN_VOID_BREAKER						= 9000,		/// ���̵� �극��Ŀ
		SI_SA_AN_PARTICLE_PRISM						= 9001,		/// ��ƼŬ ������
		SI_SA_AN_ENERGY_BOOM						= 9002,		/// ������ ��
		SI_SA_AN_PARTICLE_ACCELERATOR				= 9003,		/// ��ƼŬ �׼���������
		SI_SA_AN_NEUTRON_BOMB						= 9004,		/// ��Ʈ�� ��

		SI_SA_APT_PULSE_CANNON						= 9005,		/// �޽� ĳ��
		SI_SA_APT_EMP_SHOCK							= 9006,		/// EMP ��ũ
		SI_SA_APT_QUAKE_BUSTER						= 9007,		/// ����ũ ������
		SI_SA_APT_STASIS_FIELD						= 9008,		/// �����̽ý� �ʵ�
		SI_SA_APT_CONQUEROR							= 9009,		/// ��Ŀ��

		SI_SA_ALP_REVERSE_REACTOR					= 9010,		/// ������ ������
		SI_SA_ALP_QUICKSILVER_ACCEL					= 9011,		/// ���ǹ� �׼�
		SI_SA_ALP_QUICKSILVER_FRENZY				= 9012,		/// ���ǹ� ������
		SI_SA_ALP_DUST_ANATOMY						= 9013,		/// ����Ʈ �Ƴ����
		SI_SA_ALP_PSYCHIC_STORM						= 9014,		/// ����ű ����


		/// ��Ƽ��
		SI_A_AN_PULSE_BULLET						= 9200,		/// ����ź
		SI_A_AN_MIND_BREAK							= 9201,		/// ���� �ر�
		SI_A_AN_PHASE_SHIFT							= 9202,		/// ���� ��ȭ

		SI_A_APT_MAGNETRON_DYNAMO					= 9203,		/// ���̳ʸ� ���� - ���׳�Ʈ��
		SI_A_APT_DESOLVER_DYNAMO					= 9204,		/// ���̳ʸ� ���� - ������
		SI_A_APT_PYLON_DYNAMO						= 9205,		/// ���̳ʸ� ���� - ���Ϸ�

		SI_A_ALP_PSIONIC_BEAT						= 9206,		/// ���̿��� ��Ʈ
		SI_A_ALP_TWIRL_RUSH							= 9207,		/// Ʈ�� ����


		/// �нú�
		SI_P_AN_CHARGED_IMPULSAR					= 9400,		/// ���̳ʸ� ���� - ��������
		SI_P_AN_RESEARCH_DYNAMO_DISSOLUTION			= 9401,		/// ���̳ʸ� ���� - ��ü
		SI_P_AN_RESEARCH_DYNAMO_COMPOSITION			= 9402,		/// ���̳ʸ� ���� - ����
		SI_P_APT_NASOD_ARMOR_MODE					= 9403,		/// ���ҵ� �Ƹ� ���

		SI_P_APT_LIBRARY_OF_LIMITLESS				= 9404,		/// ������ ������
		SI_P_APT_DUST_STORM							= 9405,		/// �̸��� ��ǳ

		SI_P_ALP_BODY_OF_TRANSCENDENCE				= 9406,		/// �ʿ����� ��ü
		SI_P_ALP_POWER_EXCHANGER					= 9407,		/// ��� ��ȯ��
		SI_P_ALP_FANTASY_TRACER						= 9408,		/// ȯ�� ������


		/// ������ ��Ƽ�� ��ų
		SI_HA_ALP_DOOMS_DAY							= 9600,		/// �ҽ� ����

//#pragma endregion ��ų
	};



	enum SKILL_ABILITY_TYPE
	{
		SA_INVALID,
		SA_EFFECTIVE_TIME,					/// ���� �ð�
		SA_EFFECTIVE_COUNT,					/// ���� Ƚ��
		SA_SKILL_ID,						/// Ư�� ��ų ����
		SA_COMBO_ID,						/// Ư�� �޺� ����
		SA_EFFECTIVE_RATE,					/// �ߵ� Ȯ��
		SA_ATK_REL,							/// ���ݷ�(����,����) ���� ����
		SA_ATK_PHYSIC_REL,					/// ���� ���ݷ� ���� ����
		SA_ATK_MAGIC_REL,					/// ���� ���ݷ� ���� ����
		SA_DEF_PHYSIC_REL,					/// ���� ���� ���� ����
		SA_DEF_MAGIC_REL,					/// ���� ���� ���� ����
		SA_TEAM_ATK_PHYSIC_REL,				/// ���� ���ݷ� ���� ���� (��)
		SA_TEAM_ATK_MAGIC_REL,				/// ���� ���ݷ� ���� ���� (��)
		SA_TEAM_DEF_PHYSIC_REL,				/// ���� ���� ���� ���� (��)
		SA_TEAM_DEF_MAGIC_REL,				/// ���� ���� ���� ���� (��)
		SA_DAMAGE_REL,						/// ������ ���� ����
		SA_DAMAGED_REL,						/// �޴� ������ ����
		SA_UNFIXED_DEFENSE_REL,				/// �� ����
		SA_SUMMON_ATK_REL,					/// ��ȯ�� ���ݷ� ����
		SA_MAX_HP_ABS,						/// �ִ� HP ���� (��ġ)
		SA_MAX_HP_REL,						/// �ִ� HP ���� (%)
		SA_MAX_MP_ABS,						/// �ִ� MP ���� (��ġ)
		SA_MAX_MP_REL,						/// �ִ� MP ���� (%)
		SA_TELEPORT_RANGE_ABS,				/// ĳ���� �̵���ų �Ÿ� ����
		SA_CHARGING_TIME,					/// ������ �ð�
		SA_HP_REL_TO_MP_ABS_1,				/// ������ȯ ù��° param (HP�� %)
		SA_HP_REL_TO_MP_ABS_2,				/// ���� ��ȯ �ι�° param (MP�� ��ġ)
		SA_MP_CONSUME_REL,					/// ���� �Ҹ� ����
		SA_MP_GAIN_ON_HIT_REL,				/// (Ÿ�ݽ�)���� ȹ�淮 ����
		SA_SCOPE_ABS,						/// ��ų���� ���� (�ݰ�,��ġ)
		SA_PROJECTILE_FIRE_COUNT,			/// �߻�ü ���� ����
		SA_PROJECTILE_RANGE_REL,			/// �߻�ü �Ÿ� ����
		SA_PROJECTILE_SIZE,					/// �߻�ü ������ ����
		SA_PROJECTILE_PIERCING,				/// �߻�ü ��Ʈ �� (�����) ����
		SA_MOVE_SPEED,						/// �̵� �ӵ� ����
		SA_JUMP_SPEED,						/// ���� �ӵ� ����
		SA_ATK_PHYSIC_TO_ATK_MAGIC,			/// ���� ���ݷ��� ������ �������ݷ�
		SA_ATK_MAGIC_TO_ATK_PHYSIC,			/// ���� ���ݷ��� ������ �������ݷ�
		SA_READY_TIME_REL,					/// ������ �غ� �ð� ����
		SA_FORCE_DOWN_REL,					/// �����ٿ� ��ġ ���� (%)
		SA_FORCE_DOWN_MELEE_REL,			/// �������� �����ٿ� ��ġ ���� (%)
		SA_FORCE_DOWN_RANGE_REL,			/// �߻�ü���� �����ٿ� ��ġ ���� (%)
		SA_FORCE_DOWN_ABS,					/// ��ųŸ�ݽ� �����ٿ� ��ġ ���� (��ġ)
		SA_MP_REGENERATION_ABS,				/// MP ȸ�� ������
		SA_MP_INTAKE_ABS,					/// MP ��Ż��
		SA_TARGET_ANI_SPEED,				/// ��� �ִ� �ӵ� ����
		SA_TARGET_MOVE_SPEED,				/// ��� �̵� �ӵ� ����
		SA_TARGET_JUMP_SPEED,				/// ��� ���� �ӵ� ����
		SA_TARGET_ATK_REL,					/// ��� ���ݷ� ����
		SA_HP_INTAKE_REL_DAMAGE,			/// �� ������ ��� HP ������(%)
		SA_ATK_CRITICAL_DAM_ABS,			/// �����ڰ� �ִ� ũ��Ƽ�� ������ ����
		SA_ATK_CRITICAL_RATE_ABS,			/// �����ڰ� �ִ� ũ��Ƽ�� ����
		SA_DEF_CRITICAL_DAM_ABS,			/// �ǰ��ڰ� �޴� ũ��Ƽ�� ������ ����
		SA_DEF_CRITICAL_RATE_ABS,			/// �ǰ��ڰ� �޴� ũ��Ƽ�� ����		// kimhc // 2011-04 �� ��ġ�� û ĳ�� ��ų
		SA_BACK_SPEED_X,					/// ���ݿ� ���� x�������� �з����� ��
		SA_ITEM_SPECIAL_ABILITY_REL,		/// HP/MP ������ ����� ȸ���� ����(%)
		SA_MP_GAIN_GET_HIT_REL,				/// (�ǰݽ�)���� ȹ�淮 ���� (%)
		SA_SOUL_GAIN_ON_HIT_REL,			/// (Ÿ�ݽ�)�ҿ� ȹ�淮 ���� (%)
		SA_SOUL_GAIN_GET_HIT_REL,			/// (�ǰݽ�)�ҿ� ȹ�淮 ���� (%)
		SA_HP_GAIN_REL_MAX_HP,				/// ĳ������ MAX HP�� %�� �ش��ϴ� HP ȸ��
		SA_ED_GAIN_REL,						/// ED ���淮 ����
		SA_ATK_PHYSIC_PLUS_ATK_MAGIC_REL,	/// (���� ���ݷ� + ���� ���ݷ�)�� %������
		SA_EXP_GAIN_REL,					/// EXP ���淮 ����
		SA_DAMAGE_ABSORB_REL_MAX_HP,		/// ĳ������ MAX HP�� %�� �ش��ϴ� ������ ����


		SA_STRONG_MIND_ACTIVE_REL,			/// ??
		SA_STRONG_MIND_SPECIAL_ACTIVE_REL,	/// ??
		SA_MP_RETAKE_ABS,					/// ���� ȸ��

		SA_RESIST_FIRE_ABS,					/// �� ����
		SA_RESIST_WATER_ABS,				/// �� ����
		SA_RESIST_GREEN_ABS,				/// �ڿ� ����
		SA_RESIST_WIND_ABS,					/// �ٶ� ����
		SA_RESIST_LIGHT_ABS,				/// �� ����
		SA_RESIST_DARK_ABS,					/// ��� ����

		SA_ATK_MAGIC_FIRST_REL,				/// ù��° ����
		SA_ATK_MAGIC_SECOND_REL,			/// �ι�° ����
		SA_ATK_MAGIC_THIRD_REL,				/// ����° ����

		SA_REMOTE_SPEED_ABS,				/// �ӵ� ����

		SA_DAMAGE_TYPE_CHANGE,				/// ������ ���� ����
		SA_DAMAGE_TIME,						/// ������ �ð�

//{{ ����� : 2010.11.12
//#ifdef NEW_SKILL_2010_11
		SA_RECOVER_MP_ABS,
		SA_TARGET_MANA,
//#endif NEW_SKILL_2010_11
//}} ����� : 2010.11.12
//#ifdef NEW_SKILL_2010_11
		SA_TARGET_MANA_REL,					/// Ÿ�� ���� ���� ����, JHKang
		SA_SIZE_ABS,						/// ������ NPC ũ��, (����� ���� ũ�� ��), JHKang
//#endif NEW_SKILL_2010_11
		SA_REFLECT_REL,						/// ���� �ݻ� Ȯ��
		SA_EL_DEFENCE_ABS,					/// ��� �Ӽ� ����
		SA_TARGET_HP_REL,					/// ��� HP �ۼ�Ʈ ȸ��
		SA_TARGET_HITRATE_REL,				/// EDT BLIND�� ����
		//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
		SA_CHARGE_CANNON_BALL,				/// ĳ���� ������
		//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

		//{{ kimhc // 2011.1.14 // û 1�� ����
		SA_FIRE_DEFENCE_ABS,				/// ���� â�� ��
		SA_WATER_DEFENCE_ABS,				/// ���� â�� ��
		SA_NATURE_DEFENCE_ABS,				/// ���� â�� �ڿ�(�׸�)
		SA_WIND_DEFENCE_ABS,				/// ���� â�� �ٶ�
		SA_LIGHT_DEFENCE_ABS,				/// ���� â�� ��
		SA_DARK_DEFENCE_ABS,				/// ���� â�� ���

		SA_EDT_FIRE_RESIST_REL,				/// ȭ�� ���� ����
		SA_EDT_FROZEN_RESIST_REL,			/// ���� ���� ����
		SA_EDT_ICE_RESIST_REL,				/// ���� ���� ����
		SA_EDT_POISON_RESIST_REL,			/// �� ���� ����
		SA_EDT_Y_PRESSED_RESIST_REL,		/// ������ ���� ����
		SA_EDT_LEG_WOUND_RESIST_REL,		/// ��ó ���� ����
		SA_EDT_STUN_RESIST_REL,				/// ���� ���� ����
		SA_EDT_CURSE_RESIST_REL,			/// ���� ���� ����

		SA_EXPAND_CB,						/// ĳ���� Ȯ�� ��
		SA_ANI_SPEED,						/// �ִϸ��̼� �ӵ� ����
		//}} kimhc // 2011.1.14 // û 1�� ����

		//{{ kimhc // 2011-04-04 // 2011-04 �� ��ġ�� û ĳ�� ��ų
		SA_ACCURACY_PERCENT_ABS,			/// ���߷�
		//}} kimhc // 2011-04-04 // 2011-04 �� ��ġ�� û ĳ�� ��ų
		SA_CRITICAL_EVASION_REL,
//#ifdef CHUNG_SECOND_CLASS_CHANGE
		SA_DECREASE_REL,					/// ���� ����
//#endif
//#ifdef ELSWORD_SHEATH_KNIGHT
		SA_ATK_ADD_DAMAGE_FIRST_ABS,
		SA_ATK_ADD_DAMAGE_SECOND_ABS,
		SA_ATK_ADD_DAMAGE_THIRD_ABS,
		SA_ATK_ADD_DAMAGE_FOURTH_ABS,
		SA_ATK_ADD_DAMAGE_FIFTH_ABS,
		SA_TELEPORT_SPEED_REL,
		HP_CONSUME_REL,
		SA_MP_BURN_ABS,
//#endif ELSWORD_SHEATH_KNIGHT
//#ifdef	SERV_TRAPPING_RANGER_TEST	//	����Ż��Ƽ �������� Ȯ��
		SA_SKILLDAMAGE_MULTIPLE_01,
		SA_SKILLDAMAGE_MULTIPLE_02,
		SA_SKILLDAMAGE_MULTIPLE_03,
		SA_SKILLDAMAGE_MULTIPLE_04,

		SA_SKILLDAMAGE_MULTIPLE_01_RATE,
		SA_SKILLDAMAGE_MULTIPLE_02_RATE,
		SA_SKILLDAMAGE_MULTIPLE_03_RATE,
		SA_SKILLDAMAGE_MULTIPLE_04_RATE,
//#endif	SERV_TRAPPING_RANGER_TEST
//#ifdef RAVEN_WEAPON_TAKER
		SA_EVASION_REL,
		SA_REDUCE_EDT_TIME_REL,
		SA_OVERHEAT_HP,
//#endif RAVEN_WEAPON_TAKER
//#ifdef EVE_ELECTRA
		SA_SCOPE_ABS_HYPER,
		SA_EFFECTIVE_TIME_HYPER,
		SA_AIR_MOVE_COUNT,
//#endif EVE_ELECTRA
//#ifdef SERV_ADD_CHUNG_SHELLING_GUARDIAN
		SA_NOT_CANNONBALL_REL,
		SA_INSERT_CANNONBALL_REL,
		SA_BE_SHOT_CANNONBALL_REL,
		SA_CANNONBALL_DAMAGE_UP_REL,
		SA_FORCE_DOWN_REL_DAMAGE_RATE_BASE,
		SA_SIEGE_SHELLING_REL,
		SA_EFFECTIVE_DAMAGED_ATTACK_BASE,
//#endif //SERV_ADD_CHUNG_SHELLING_GUARDIAN
//#ifdef SERV_CHUNG_TACTICAL_TROOPER
		SA_FIRST_HYPER_EFFECTIVE,
		SA_SECOND_HYPER_EFFECTIVE,
		SA_THIRD_HYPER_EFFECTIVE,
//#endif SERV_CHUNG_TACTICAL_TROOPER
//#ifdef BALANCE_CODE_NEMESIS_20121213
		SA_ATK_CRITICAL_RATE_ATK_BASE,			/// ���ݷ��� % ��ŭ �߰��Ǵ� ũ��Ƽ�� ����
//#endif //BALANCE_CODE_NEMESIS_20121213
		SA_PROJECTILE_CURVE_SPEED_REL,			/// Ŀ�� ���ǵ带 ���� ���ִ� ����
		SA_DP_GAIN_REL,							/// DP ȹ�淮 ����
		SA_DP_USE_REL,							/// DP �Ҹ� ����
		// XSLSkillTree.h���� �߰����ּ���~
	};


	// ���ҵ�	601001 ~ 601999
	// ���̻�	602001 ~ 602999
	// ����		603001 ~ 603999
	// ���̺�	604001 ~ 604999
	// �̺�		605001 ~ 605999
	// û		606001 ~ 606999
	enum SKILL_MEMO_ID
	{
		SMI_NONE			= 0,

		SMI_ELSWORD_MEMO1	= 601001,	/// ���ڴ� ������ ���� �˾ƾ��Ѵ�.
		SMI_ELSWORD_MEMO2,				/// ���� ������ ���� ����
		SMI_ELSWORD_MEMO3,				/// ������ ���� �� �̷а� ����
		SMI_ELSWORD_MEMO4,				/// ���� ��� ���� ����
		SMI_ELSWORD_MEMO5,				/// ���� �δ� ���� ��
		SMI_ELSWORD_MEMO6,				/// ����� ȣ���� ��ȭ ������
		SMI_ELSWORD_MEMO7,				/// ������
		SMI_ELSWORD_MEMO8,				/// ��Ϻ��� (�÷��Ӱ�����)
		SMI_ELSWORD_MEMO9,				/// ��Ȯ�ϰ� ���� Ÿ�� (�����̷� ������Ʈ)
		SMI_ELSWORD_MEMO10,				/// ��ȥ�ı� (���� �����̵�)
		SMI_ELSWORD_MEMO11,				/// ���ʺر� (�Ƹ� �극��ũ)
		SMI_ELSWORD_MEMO12,				/// ������ �ʴ� �ұ� (������)
		SMI_ELSWORD_MEMO13	= 601013,	/// �˶մ� �� (�Ͻ� ü�̼�)
		SMI_ELSWORD_MEMO14,				/// ���� �� ���� (ũ����Ʈ ��)
		SMI_ELSWORD_MEMO15,				/// Į�� �� (�ҵ� ��)

		SMI_AISHA_MEMO1		= 602001,	/// ��� ���� ���(ü�� ���̾ ��)
		SMI_AISHA_MEMO2,				/// ������ �ܻ�
		SMI_AISHA_MEMO3,				/// �ּ��� �̻���
		SMI_AISHA_MEMO4,				/// ���� �� ������ �ں�� ����!
		SMI_AISHA_MEMO5,				/// ��ȥ �̵��� ���� �޸�
		SMI_AISHA_MEMO6,				/// �߰��� ���� �ǵ忡 ���� �޸�
		SMI_AISHA_MEMO7,				/// ��Ŭ�÷���
		SMI_AISHA_MEMO8,				/// 200% ������ ��ũ (����Ʈ�� ��Ʈ)
		SMI_AISHA_MEMO9,				/// �������� (ü�� ����Ʈ)
		SMI_AISHA_MEMO10,				/// ��������� ȸ�� ( �ｺ�� )
		SMI_AISHA_MEMO11,				/// �����ο� ��ȥ (�޵����̼�)
		SMI_AISHA_MEMO12,				/// ������ ���� (���� ������ũ)
		SMI_AISHA_MEMO13	= 602013,	/// �Ŀ� �巹�� (������ �巹��)
		SMI_AISHA_MEMO14,				/// ������ ���� ��ġ�� (��� ������)
		SMI_AISHA_MEMO15,				/// �������� �� (������ ����Ʈ)

		SMI_RENA_MEMO1		= 603001,	/// ������ ���̴� ����¿� ���� ����
		SMI_RENA_MEMO2,					/// ������ �һ������� ����
		SMI_RENA_MEMO3,					/// ���ӱ��� ������ ����
		SMI_RENA_MEMO4,					/// �������� ���󰡴ϱ⸸ �� ���ΰ�
		SMI_RENA_MEMO5,					/// �� ������ ��ȸ (������� �� ��)
		SMI_RENA_MEMO6,					/// �� ������ ��ȸ (���÷�Ʈ ű ��)
		SMI_RENA_MEMO7,					/// ������
		SMI_RENA_MEMO8,					/// ó���� ���3 ( ��Ƽ�� ���ð�)
		SMI_RENA_MEMO9,					/// �޽� ( ���Ǵ� ű )
		SMI_RENA_MEMO10,				/// ���� ����� (��� ����)
		SMI_RENA_MEMO11,				/// ��ǰ ������ ( �̵�ű )	/// 2013�� ��ų Ʈ�� ���� ����, �ο�ű���� �̵�ű���� ����
		SMI_RENA_MEMO12,				/// �� ���� ( ���ʱ� )
		SMI_RENA_MEMO13		= 603013,	/// ���߹� ��� ����(������ ��)
		SMI_RENA_MEMO14,				/// �߰��ϴ� ���� (�̺�ũ)
		SMI_RENA_MEMO15,				/// ��ȣ�� ��������� ���� (Ʈ���� �ַο�)

		SMI_RAVEN_MEMO1		= 604001,	/// ���뽺���� ���� �ӵ�
		SMI_RAVEN_MEMO2,				/// ������ �Ұ� ����� Ÿ��
		SMI_RAVEN_MEMO3,				/// ȸ������ ���� �������� �鿪
		SMI_RAVEN_MEMO4,				/// ����
		SMI_RAVEN_MEMO5,				/// �ų��� �޸���
		SMI_RAVEN_MEMO6,				/// ���� ���� �߰���,
		SMI_RAVEN_MEMO7,				/// ������ �Ǿ��
		SMI_RAVEN_MEMO8,				/// ���ο� ���� (�Ŀ� �Ʈ)
		SMI_RAVEN_MEMO9,				/// ���� ���� (����Ŀ �����̵�)
		SMI_RAVEN_MEMO10,				/// ��Ʈ ��� ��ȭ (������ ��Ʈ)
		SMI_RAVEN_MEMO11,				/// �ƹ��� �ʴ� ��ó (���ٴ�)
		SMI_RAVEN_MEMO12,				/// ȿ������ ���� �ı� (���� �극��ũ)
		SMI_RAVEN_MEMO13	= 604013,	/// ����� ȭ�� (��������)
		SMI_RAVEN_MEMO14,				/// �Һ��� Į�� �� (������ �����̵�)
		SMI_RAVEN_MEMO15,				/// �н�Ʈ ������ (������ ĳ��)

		SMI_EVE_MEMO1		= 605001,	/// ���� ���Է�
		SMI_EVE_MEMO2,					/// ���Ӱ����� ���� ��ȯ
		SMI_EVE_MEMO3,					/// ó���� ��� (�ͽ��÷��� ����Ʈ ��)
		SMI_EVE_MEMO4,					/// ó���� ��� (�Ҵ� ���̺� ��)
		SMI_EVE_MEMO5,					/// ������ ��ٸ�
		SMI_EVE_MEMO6,					/// ���Ӽ� ���
		SMI_EVE_MEMO7,					/// �ް� �Ϸ�Ʈ�к�
		SMI_EVE_MEMO8,					/// ���� ī���� ���� (�Ϸ��� ���þ�)
		SMI_EVE_MEMO9,					/// Ư�� ���ҵ� ����ź (ȣ�� ����)
		SMI_EVE_MEMO10,					/// ������ �Ǳ� ���� �Ʒ� (���ǽ� Ŀ��)
		SMI_EVE_MEMO11,					/// ª�� ����! (��Ż ����Ʈ ����)
		SMI_EVE_MEMO12,					/// ���潺 ��� ��ȯ! (������ ����)
		SMI_EVE_MEMO13		= 605013,	/// ���� �ڰ� ������ (�� �б�����)
		SMI_EVE_MEMO14,					/// ����� ���� �� (��ƼŬ ����)
		SMI_EVE_MEMO15,					/// ���� ���� (������ �Ѹ�)

		//{{ 2011-04 �� ��ġ�� û �޸� ��ų
		SMI_CHUNG_MEMO1		= 606001,	/// ź�ళ�� (������̼�)
		SMI_CHUNG_MEMO2,				/// ������ (���ɾ�ü�̽�)
		SMI_CHUNG_MEMO3,				/// ������ (��ƿ����)
		SMI_CHUNG_MEMO4,				/// ������ ���͸� �׼� (��������)
		SMI_CHUNG_MEMO5,				/// ��ֱܳ� (���ε�ĳ��)
		SMI_CHUNG_MEMO6,				/// �Ӽ�źȯ (���ּ̹�)
		SMI_CHUNG_MEMO7,				/// �����ũ (��������)
		SMI_CHUNG_MEMO8,				/// REAL ��弦 (��弦)
		//}} 2011-04 �� ��ġ�� û �޸� ��ų
		SMI_CHUNG_MEMO9		= 606009,	/// Ǫ��&Ǫ�� (���Ż ����)
		SMI_CHUNG_MEMO10,				/// ��ǳ�� ���� �Լ� (���̾� �Ͽ︵)
		SMI_CHUNG_MEMO11,				/// �ڵ� ����ź (��� ���ϰ�)
		SMI_CHUNG_MEMO12,				/// �γ�,�γ�,�γ� (����)
		SMI_CHUNG_MEMO13,				/// �������ź (������)
		SMI_CHUNG_MEMO14,				/// ��ȣ�ϴ� �������� (���� ��)
		SMI_CHUNG_MEMO15,				/// 3�� �п� ����ź (�巹�� ü�̽�)
		SMI_CHUNG_MEMO16,				/// ���� ��ô (�Ϸ���ƽ ��)
		SMI_CHUNG_MEMO17,				/// �������� �����Ӵ� (���ְ��� ǥ��)


		SMI_ARA_MEMO1		= 607001,	/// �� ���� ���� II (����)
		SMI_ARA_MEMO2		= 607006,	/// �帧�� Ÿ�� â�� (��ȣ 2�� : �ٶ�����)
		SMI_ARA_MEMO3,					/// ��ȥ ��� (���â)
		SMI_ARA_MEMO4,					/// ��õ���� (��� 2�� : �����α�)
		SMI_ARA_MEMO5,					/// ���� �׸��� â�� (�׸��ڸŵ�)
		SMI_ARA_MEMO6,					/// ������ ���� ġ�� (��ȣ 1�� : ��������)
		SMI_ARA_MEMO7,					/// �� �帣���� (��ȭ����)
		SMI_ARA_MEMO8,					/// ������ �Ұ� ����� Ÿ�� II (��� 3�� : �ȱ���)
		SMI_ARA_MEMO9,					/// �� �޺����� (��� 4�� : �޺�����)


		SMI_ELSWORD_MEMO16	= 601016,	/// �տ� �ı� ( ���� �극��ũ )
		SMI_ELSWORD_MEMO17,				/// ������ �İ���� �ϰ� ( ���� ������ )
		SMI_ELSWORD_MEMO18,				/// ��ȭ�� �˱��� ����� ( �Ƹ��ٵ� �����̵� )
		SMI_ELSWORD_MEMO19,				/// ����Ʈ ��æƮ ( �ҵ� ��æƮ )
		SMI_ELSWORD_MEMO20,				/// ���� ��� ���� ���� 1 (����¡ ���̺� )
		SMI_ELSWORD_MEMO21,				/// �ۿ��ϴ� ȭ���� �� (�ҵ� ���̾� )
		SMI_ELSWORD_MEMO22,				/// Ÿ������ ���� Ȱ�� (�Ǵн� �ŷ� )
		SMI_ELSWORD_MEMO23,				/// �鳯 Į�� ( �ҵ� �ǵ� )
		SMI_ELSWORD_MEMO24,				/// ���� ��ô ( �Ǿ�� �ҵ� )
		SMI_ELSWORD_MEMO25,				/// ����˹� ( ���̳� ��Ʈ����ũ )
		SMI_ELSWORD_MEMO26,				/// ���� ��� ���� ���� 2 ( ���� �ҵ� )

		SMI_AISHA_MEMO16	= 602016,	/// ��ǳ�� �θ� ( ����Ŭ�� )
		SMI_AISHA_MEMO17,				/// ������ �ٷ�� ��� ( ü�� ����Ʈ�� )
		SMI_AISHA_MEMO18,				/// �г�? �г�! (�������� ���� )
		SMI_AISHA_MEMO19,				/// ������ �������� ( �����ڵ� ���� )
		SMI_AISHA_MEMO20,				/// ����� �̼� ( ���� �ʵ� )
		SMI_AISHA_MEMO21,				/// �ֹ��Ͻ� ��Դϴ� ( �� ��� )
		SMI_AISHA_MEMO22,				/// ����, ����, ��� ( ����¡ )
		SMI_AISHA_MEMO23,				/// �̿� �ö�� ( �ö�� Ŀ�� )
		SMI_AISHA_MEMO24,				/// �������� ���� ( ���ƾ ������ )
		SMI_AISHA_MEMO25,				/// �Ƹ��ٿ� ȭ��� ( ������ ����ũ�� )
		SMI_AISHA_MEMO26,				/// �� �͵� ����-! ( ����Ż �巹�� )
		SMI_AISHA_MEMO27,				/// �߰� �ֹ� ( ���� ��� )

		SMI_RENA_MEMO16		= 603016,	/// ���뿡 ���� ���� 1 ( �ο� ű )
		SMI_RENA_MEMO17,				/// ����ǳ ( ���̺�ű ���� )
		SMI_RENA_MEMO18,				/// ������ ���� ( ���̿÷�Ʈ ���� )
		SMI_RENA_MEMO19,				/// �Ӽ� ȭ�� ( ���� �ű׳� )
		SMI_RENA_MEMO20,				/// ��ź�� ��ü ( ũ������ �� )
		SMI_RENA_MEMO21,				/// ���߿� ���� ���� 1 ( �ñ״� )
		SMI_RENA_MEMO22,				/// ��ƴ�� �븮�� �ϰ� ( ����Ż��Ƽ )
		SMI_RENA_MEMO23,				/// �ñ⵶�� ���� ���� ( ������ ���� )
		SMI_RENA_MEMO24,				/// ��ī�ο� ���� 1 ( ���õ��� �� )
		SMI_RENA_MEMO25,				/// ��ī�ο� ���� 2 ( �� �� )
		SMI_RENA_MEMO26,				/// �ֻ�� ¡�� ( �� ���� ���� )
		SMI_RENA_MEMO27,				/// �ż��� ����� ���� ( ī���� )

		SMI_RAVEN_MEMO16	= 604016,	///	�˱��� ��� 1 ( ��ũ ���̺� - ����̴� )
		SMI_RAVEN_MEMO17,				/// �˱��� ��� 2 ( ��ũ ���̺� - Ŀ�� )
		SMI_RAVEN_MEMO18,				/// �߰Խ�� ( ������ �Ҵ� ���� )
		SMI_RAVEN_MEMO19,				/// ���� �;� ( ������ �׼� )
		SMI_RAVEN_MEMO20,				/// ���� ���� ( �� ���̺� )
		SMI_RAVEN_MEMO21,				/// ���� �������� ���� ( ��Ű���� �ں��� )
		SMI_RAVEN_MEMO22,				/// �Ϻ��� ����� �ִ� ��� ( �Ƹ� ũ���� )
		SMI_RAVEN_MEMO23,				/// ��ö�� ���� ( ������ ��Ʈ - ������ ���� )
		SMI_RAVEN_MEMO24,				/// �ż��� ���� ( ����� ��Ʈ����ũ )
		SMI_RAVEN_MEMO25,				/// ���߿� ���� ���� ( ��Ŭ���� )
		SMI_RAVEN_MEMO26,				/// ������� ( ��Ǭ ���Ǿ� )
		SMI_RAVEN_MEMO27,				/// ȭ���� �ܻ� ( ȭ���� )
		SMI_RAVEN_MEMO28,				/// ����Ʈ ������ ( ������ ĳ�� - HEź )
		SMI_RAVEN_MEMO29,				/// ���� ���� ( �����̾� ��Ʋ�� )
		SMI_RAVEN_MEMO30,				/// ���߿� ���� ���� 2 ( �Ⱑ ���ι̳ͽ� )

		SMI_EVE_MEMO16		= 605016,	/// Ư�� ���ҵ� ����ź ( ȣ�� ���� - ������Ʈ ���� )
		SMI_EVE_MEMO17,					/// ������ ��ȭ ( ���� ���� )
		SMI_EVE_MEMO18,					/// �ں� �ͼ��� ���� ( ��ũ �극��ũ )
		SMI_EVE_MEMO19,					/// ���� ���� ( ������̵� ���� )
		SMI_EVE_MEMO20,					/// ���� ����� ���ϴ� ��� ( ����� �ǽ�Ʈ - ������ )
		SMI_EVE_MEMO21,					/// �������� ( ����� �ǽ�Ʈ - ������ )
		SMI_EVE_MEMO22,					/// ��� ������ ( ������ �Ѹ� - Ʈ���� ��ĭ )
		SMI_EVE_MEMO23,					/// �ں� Ư�� ���� ( ���� �÷��� )
		SMI_EVE_MEMO24,					/// ����� ���� ( ���Ͼ� ����̴� )
		SMI_EVE_MEMO25,					/// Ǯ �Ϸ�Ʈ�δ� ( �Ⱑ ��Ʈ�� )

		SMI_CHUNG_MEMO18	= 606018,	/// ���� �׸��� ��ħ ( �ǵ���Ͽ� - �Ǿ� )
		SMI_CHUNG_MEMO19,				/// ȥ�� �׸��� ��ħ ( �ǵ���Ͽ� - ī���� )
		SMI_CHUNG_MEMO20,				/// ������ �ϰ� ( �糪ƽ ǻ�� )
		SMI_CHUNG_MEMO21,				/// ���� ��ġ�� ( Į��庼�� ���� )
		SMI_CHUNG_MEMO22,				/// ���� ���ع� ( ���� ���̾� )
		SMI_CHUNG_MEMO23,				/// Ȯ��ź ������ ( �ű׳� �� )
		SMI_CHUNG_MEMO24,				/// �ں� Ư�� ����ź ( ���� ��Ÿ )
		SMI_CHUNG_MEMO25,				/// ���õ� ������ ���� ( ��ƿ���� ��Ʈ����ũ - ���� �߸���Ÿ )
		SMI_CHUNG_MEMO26,				/// �ΰ����� ���� ��ź ( ��� ��Ʈ�� )
		SMI_CHUNG_MEMO27,				/// Ư�� ����ź ������ ( �Ϸ���ƽ �� - �׸����̵� ��ó )
		SMI_CHUNG_MEMO28,				/// ��鸲 ���� �ڼ� ( ĳ�� ��Ʈ����ũ )
		SMI_CHUNG_MEMO29,				/// ���� ��ȭ ( ī���� ĳ�� )
		SMI_CHUNG_MEMO30,				/// �߰� ���� ( ī�� �ٹ� )

		SMI_ARA_MEMO10		= 607018,	/// ���� ( ��ȭ)
		SMI_ARA_MEMO11,					/// ������ �ͼ��� ���� ( ��ȣ 2�� : ȣ���� ���� )
		SMI_ARA_MEMO12,					/// ��ȣ�� ��� ( ��ȣ�� )
		SMI_ARA_MEMO13,					/// ȿ������ ��� ��� ( ���� 1�� : ��ȥ )
		SMI_ARA_MEMO14,					/// ���� ������ ( ���� 3�� : ���� ���� )
		SMI_ARA_MEMO15,					/// �� ���� ���� 3 ( �̱�� )
		SMI_ARA_MEMO16,					/// �� ���� ���� 5 ( ������ )
		SMI_ARA_MEMO17,					/// ������ ���� ��� ( ���� 4�� : ���� �̻� )
		SMI_ARA_MEMO18,					/// ������ ����ġ�� ( ��ȯ���� )
		SMI_ARA_MEMO19,					/// Ż�� �Ұ���! ( ���� 3�� : â�차�� )
		SMI_ARA_MEMO20,					/// ��Ȳ�� ��� ( ����� )
		SMI_ARA_MEMO21,					/// ź������ ���� ��� ( ��ź�� )

		SMI_ELESIS_MEMO1	= 608007,	/// ������ ���� �غ� ( ũ�ν� ������ )
		SMI_ELESIS_MEMO2,				/// ������ ���� ����1 ( �����̷� ������Ʈ )
		SMI_ELESIS_MEMO3,				/// ȿ������ Ÿ�� ��� ( ������ )
		SMI_ELESIS_MEMO4,				/// �� ������ ������ ��� ( ��� ���ͳ� )
		SMI_ELESIS_MEMO5,				/// �ı� ������ ��� ( �Ŀ� �극��ũ )
		SMI_ELESIS_MEMO6,				/// �����ϰ� �����̴� ��� ( ���̹� - �ͽ��ü� )
		SMI_ELESIS_MEMO7,				/// ������ ���� ����2 ( ������ �� )
		SMI_ELESIS_MEMO8,				/// ȯ�� ( �¸��� �� )
		SMI_ELESIS_MEMO9,				/// �������� ( �ر� - �� )
		SMI_ELESIS_MEMO10,				/// ���� ��� ( �ҵ� �̷��� )
		SMI_ELESIS_MEMO11,				/// ȿ������ �� �¿�� ( ������ )
		SMI_ELESIS_MEMO12,				/// �ż��� �˰� ( ����Ʈ ����¡ )
		SMI_ELESIS_MEMO13,				///	���� ���� ����! ( ����Ʈ ���̺� )
		SMI_ELESIS_MEMO14,				/// ���� ���� ( �ҵ� ���̾� )
		SMI_ELESIS_MEMO15,				/// ������ ���� Į�� ( ���丣�� �����̵� )
		SMI_ELESIS_MEMO16,				/// ���� ��ȭ ( ���ͳ� ���̾� )


		SMI_END,
	};


	enum SKILL_POWER_RATE_TYPE		/// �� ���ǿ� ���� ������ ���� �ε���
	{
		SPRT_NONE			= -1,
		SPRT_BASIC_CLASS,			/// �⺻ ����
		SPRT_FIRST_CLASS,			/// 1������
		SPRT_SECOND_CLASS,			/// 2������
		SPRT_SECOND_CLASS2,			/// 2������2 (�Ĺ� ��ų)
		SPRT_HYPER_SKILL,			/// ������
		SPRT_END,
	};


	struct SkillTemplet
	{
		bool						m_bGuildSkill;					/// ��� ��ų Ȯ��
		SKILL_TYPE					m_eType;						/// ��ų Ÿ��
		SKILL_ID					m_eID;							/// ��ų ���̵�

		//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
		USHORT						m_usCBConsumption;				/// ĳ���� �Ҹ�
		//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG

		USHORT						m_usFPGainCount;				/// ��� ȹ�淮
		USHORT						m_usFPConsumtion;				/// ��� ��뷮

		wstring						m_wstrName;						/// ��ų �̸�
		wstring						m_wstrMainDesc;					/// ��ų ���� ( ������ ������� �������� ǥ�õǴ� ���� ���� )

		bool						m_bBornSealed;					/// ������ �ִ� ��ų����

		wstring						m_wstrIconName;					/// ��ų ������ �ؽ�ó �̸�
		wstring						m_wstrIconPieceName;			/// ��ų ������ �ؽ�ó Rect �̸�
		ACTIVE_SKILL_USE_CONDITION	m_eActiveSkillUseCondtion;		/// ��ų ����� ���� ����
		bool						m_bShowSkillLevel;				/// ��ų ���� ǥ�� ����

		vector<CX2BuffFactorPtr>	m_vecBuffFactorPtr;				/// ��ų ����
		vector<UINT>				m_vecBuffFactorID;				/// ���� ���� ID, m_vecBuffFactorPtr�� ���� index�� ���� ����

		vector<wstring>				m_vecEffectiveDesc;				/// ��ų ȿ�� ���� ( �������� �ٸ��� ǥ�õǴ� ���� ����  )
		wstring						m_wstrImageName;				/// ��ų �̹��� �ؽ�ó �̸�
		wstring						m_wstrImagePieceName;			/// ��ų �̹��� �ؽ�ó Rect �̸�
		int							m_iRequireLearnSkillPoint;		/// ��ų ���濡 �ʿ��� SP
		int							m_iRequireUpgradeSkillPoint;	/// ��ų ��ȭ�� �ʿ��� SP
		vector<CX2Stat::Stat>		m_vecStat;						/// �����ʻ���� ��� stat ����ġ
		vector<float>				m_vecSkillCoolTime;				/// ��Ÿ��
		vector<float>				m_vecMPConsumption;				/// MP �Ҹ�
		vector<float>				m_vecPowerRate;					/// ���ݷ� ����
		vector<int>					m_vecRequireCharacterLevel;		/// ��ų ���� �䱸 ����
		int							m_iWOSType;						/// ���Ǳ� Ÿ��
		std::map< CX2SkillTree::SKILL_ABILITY_TYPE, vector<float> > m_mapSkillAbility;	/// ��ų �ɷ�ġ



		/** @function	: GetSkillStatValue
		@brief		: ��ų ������ �ش��ϴ� ���� �䱸 ������ ������
		*/
		const int GetSkillRequireLevelValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecRequireCharacterLevel.size() ) > iIndex )
				return m_vecRequireCharacterLevel[iIndex];

			return 0;
		}

		/** @function	: GetSkillStatValue
		@brief		: ��ų ������ �ش��ϴ� ���� ���� ������
		*/
		const CX2Stat::Stat GetSkillStatValue( int iLevel ) const
		{
			CX2Stat::Stat stat;

			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecStat.size() ) > iIndex )
				stat.AddStat( m_vecStat[iIndex] );

			return stat;
		}

		/** @function	: GetSkillPowerRateValue
		@brief		: ��ų ������ �ش��ϴ� ������ ���� ���� ������
		*/
		const float GetSkillPowerRateValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecPowerRate.size() ) > iIndex )
				return m_vecPowerRate[iIndex];

			return 0.f;
		}

		/** @function	: GetSkillEffectiveDescValue
			@brief		: ��ų Ÿ�Կ� �ش��ϴ� ��ų ȿ�� ���� ���� ������
		*/
		const wstring GetSkillEffectiveDescValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecEffectiveDesc.size() ) > iIndex )
			{
				// ��ų ���� Ȯ���� �ʿ��� ����� ���� ó��, ������ ����Ʈ�� @1 ( ���� ���� ǥ�� ) ������ �߰�
				wstring wstrEffectiveDesc = m_vecEffectiveDesc[iIndex];

				if ( true == GetSkillEffectiveDescExpand ( wstrEffectiveDesc, iLevel ) )
					return wstrEffectiveDesc;
				else
					return m_vecEffectiveDesc[iIndex];
			}
			return L"";
		}

		/** @function	: GetSkillMPConsumption
			@brief		: ��ų ������ �ش��ϴ� MP�Һ��� ������
		*/
		const float GetSkillMPConsumptionValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecMPConsumption.size() ) > iIndex )
				return m_vecMPConsumption[iIndex];

			return 0.f;
		}

		/** @function	: GetSkillCoolTimeValue
			@brief		: ��ų ������ �ش��ϴ� ��Ÿ�� ���� ������
		*/
		const float GetSkillCoolTimeValue( int iLevel ) const
		{
			const int iIndex = iLevel - 1;
			if ( 0 <= iIndex && static_cast<int>( m_vecSkillCoolTime.size() ) > iIndex )
				return m_vecSkillCoolTime[iIndex];

			return 0.f;
		}

		/** @function	: GetSkillAbilityValue
			@brief		: ��ų �����Ƽ�� �ش��ϴ� ���� ������
		*/
		float GetSkillAbilityValue( CX2SkillTree::SKILL_ABILITY_TYPE eType, int iLevel ) const
		{
			//ASSERT( false == m_mapSkillAbility.empty() );

			std::map< CX2SkillTree::SKILL_ABILITY_TYPE, vector<float> >::const_iterator it = m_mapSkillAbility.find( eType );
			//ASSERT( m_mapSkillAbility.end() != it );
			if( m_mapSkillAbility.end() != it )
			{
				vector<float> vecValue = it->second;

				const int iIndex = iLevel - 1;

				if ( 0 <= iIndex && static_cast<int>( vecValue.size() ) > iIndex )
					return vecValue[iIndex];
			}

			return 0.f;
		}


		/** @function : GetCloneBuffFactorPtr
			@brief : ��ų�� ������ �ִ� ���������� Ŭ���� ������ �Լ�
			@param : �������� �ϴ� ���������� �ε���
			@return : ��ų�� ������ �ִ� ���������� Ŭ�� �Ǵ� �� ����Ʈ ������
		*/
		CX2BuffFactorPtr GetCloneBuffFactorPtr( const UINT uiIndex_ ) const
		{
			if ( uiIndex_ < m_vecBuffFactorPtr.size() && NULL != m_vecBuffFactorPtr[uiIndex_] )
				return m_vecBuffFactorPtr[uiIndex_]->GetClonePtr();
			else
				return CX2BuffFactorPtr();
		}

		/** @function	: SkillTemplet
			@brief		: ������, �Ӽ� �ʱ�ȭ
		*/

		bool GetSkillEffectiveDescExpand ( OUT wstring & wstrSkillEffectiveDesc, int iLevel ) const;

		/** @function	: GetSkillDPConsumptionValue
			@brief		: DP�Һ��� ������
		*/
		const float GetSkillDPConsumptionValue() const { return static_cast<float>( m_usFPConsumtion ); }




		SkillTemplet() :
		m_bGuildSkill( false ),
		m_eType(ST_NONE ),
		m_eID(SI_NONE ),
	//{{ kimhc // 2010.12.14 // 2010-12-23 New Character CHUNG
		m_usCBConsumption( 0 ),
	//}} kimhc // 2010.12.14 //  2010-12-23 New Character CHUNG
		m_usFPGainCount( 0 ),
		m_usFPConsumtion( 0 ),
		m_wstrName( L"" ),
		m_wstrMainDesc( L"" ),
		m_bBornSealed( false ),
		m_wstrIconName( L"" ),
		m_wstrIconPieceName( L"" ),
		m_wstrImageName( L"" ),
		m_wstrImagePieceName( L"" ),
		m_iRequireLearnSkillPoint( 0 ),
		m_iRequireUpgradeSkillPoint( 0 ),
		m_eActiveSkillUseCondtion( ASUT_NONE ),
		m_bShowSkillLevel( false )
		{
			m_vecBuffFactorPtr.clear();
			m_mapSkillAbility.clear();
			m_vecStat.clear();
			m_vecSkillCoolTime.clear();
			m_vecMPConsumption.clear();
			m_vecPowerRate.clear();
			m_vecRequireCharacterLevel.clear();
			m_vecEffectiveDesc.clear();
		}
	}; // struct SkillTemplet


	typedef std::map<SKILL_ID, SkillTemplet*> SkillTempletMap;

	/// ��ų Ʈ�� ���ø�
	struct SkillTreeTemplet
	{
		int				m_iMaxSkillLevel;			/// �ִ� ��ų ����
		int				m_iTier;					/// ��ųƮ������ ������ ��ġ, ���η� �� ��° row���� 0���� ����
		int				m_iIndexInTier;				/// ��ųƮ������ ������ ��ġ, ���η� �� ��° column���� 0���� ����

		int				m_iPrecedingSkill;			/// ���� ��ų ID
		int				m_iFollowingSkill;			/// ��ų �ǵ����� �� �� �ʿ�

		/** @function	: SkillTreeTemplet
			@brief		: ������, �Ӽ� �ʱ�ȭ
		*/
		SkillTreeTemplet()
		: m_iMaxSkillLevel( 0 )
		, m_iTier( 0 )
		, m_iIndexInTier( 0 )
		, m_iPrecedingSkill( 0 )
		, m_iFollowingSkill( 0 )
		{}
	};
	typedef std::map< SKILL_ID, SkillTreeTemplet > SkillTreeTempletMap;		/// ��ų ���̵�, ��ų Ʈ�����ø��� ������ ��
	typedef std::map< int, SkillTreeTempletMap > UnitSkillTreeTempletMap;	/// < CX2Unit::UNIT_CLASS, SkillTreeTempletMap >

// oasis907 : ����� [2009.12.2] // ���� �Ľ� ���� �ּ�
//#ifdef GUILD_SKILL
	//{{ oasis907 : ����� //// 2009-11-12 ////
	typedef std::map< int, SkillTreeTempletMap > GuildSkillTreeTempletMap;	/// < CX2Unit::GUILD_CLASS, SkillTreeTempletMap >
	//}} oasis907 : ����� //// 2009-11-12 ////
//#endif GUILD_SKILL


	//{{ 2009. 8. 5  ������		���ν�ų����
	struct SealSkillItemInfo
	{
		int 								m_iUnitClassType;	/// ��� ������ ����	CX2Unit::UNIT_CLASS_TYPE
		std::vector<CX2SkillTree::SKILL_ID>	m_vecSkillID;		/// ��� ������ ���� ��ų ���̵� ���� �����̳�

		SealSkillItemInfo()
		{
			m_iUnitClassType	= -1;
			m_vecSkillID.clear();
		}
	};
	//}}



public:
	static const WCHAR* GetAccelCoreParticleName( const SKILL_ID eAccelSkillID_ );

	CX2SkillTree();
	~CX2SkillTree();

	bool OpenScriptFile( const WCHAR* pFileName, const WCHAR* pSkillDataFileName );
	void OpenScriptFilePostProcess();
	bool AddSkillTemplet_LUA();
	bool AddSkillTreeTemplet_LUA();

	//{{ 2009. 8. 5  ������		���� ��ų ������ ����
	bool AddSealSkillInfo_LUA();
	//}}

	const SkillTemplet* GetSkillTemplet( SKILL_ID eSkillID ) const;

	const SkillTempletMap& GetSkillTempletMap() const { return m_mapSkillTemplet; }
	const SkillTreeTempletMap& GetSkillTreeTempletMap( int eUnitClass );

	bool IsUnitTypeDefaultSkill( SKILL_ID eSkillID );

	const SkillTreeTemplet* GetSkillTreeTemplet( int iUnitClass, SKILL_ID eSkillID );

	// oasis907 : ����� [2009.11.25] // ���� �Ľ� ���� �̵�
	bool AddGuildSkillTreeTemplet_LUA(); //{{ oasis907 : ����� //// 2009-11-12 ////

	int GetMaxSkillLevel( int iUnitClass, SKILL_ID eSkillID );

	wstring GetSkillLevelTextureName( int iLevel );
	wstring GetSkillLevelTextureKey( int iLevel );

	//{{ 2009. 8. 5  ������		���� ��ų ������ ����
	bool IsUnsealSkillItemID( int iItemID ) { return ( m_mapSealSkillItemTemplet.find( iItemID ) != m_mapSealSkillItemTemplet.end() ); }

	const int GetUnsealSkillItemRequireClass( const int iItemID ) const;

	const CX2SkillTree::SKILL_ID GetUnsealSkillItemInfo( const int iItemID ) const;
	//}}


	const SkillTreeTemplet* GetGuildSkillTreeTemplet( int iGuildClass, SKILL_ID eSkillID ); //{{ oasis907 : ����� //// 2009-11-16 ////

	int GetMaxGuildSkillLevel( int iGuildClass, SKILL_ID eSkillID );  //{{ oasis907 : ����� //// 2009-11-16 ////



	const SkillTreeTempletMap& GetGuildSkillTreeTempletMap( int eGuildClass ); //{{ oasis907 : ����� //// 2009-11-12 ////

	//{{ �ֹ�ö [2013/1/4]  ���ӳ� ���� ��Ʈ���� �������Ϸ� ���
	//}} �ֹ�ö [2013/1/4]  ���ӳ� ���� ��Ʈ���� �������Ϸ� ���

	//////////////////////////////////////////////////////////////////////////
	bool UIServerEventProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_CHANGE_SKILL_SLOT_REQ( int iSlotID, CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_CHANGE_SKILL_SLOT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );




	bool Handler_EGS_CHANGE_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_GET_SKILL_REQ( USHORT usPageNumber_ = 0 );

	bool Handler_EGS_GET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_RESET_SKILL_REQ( CX2SkillTree::SKILL_ID eDelSkillID = CX2SkillTree::SI_NONE );
	bool Handler_EGS_RESET_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_INIT_SKILL_TREE_REQ( UidType itemUID );
	bool Handler_EGS_INIT_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_EXPIRE_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_UPDATE_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_EXPAND_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_EXPIRE_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_SKILL_SLOT_CHANGE_ITEM_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_UNSEAL_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	void On_EGS_EXPIRE_CASH_SKILL_POINT_NOT( KEGS_EXPIRE_CASH_SKILL_POINT_NOT kEvent );

	//{{ oasis907 : ����� //// 2009-11-12 ////

	// oasis907 : ����� [2009.11.27] // ��� ��ų�� ��Ƽ�� ��ų�� ���� �̻� ���x
	/*
	bool Handler_EGS_CHANGE_GUILD_SKILL_SLOT_REQ( int iSlotID, CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_CHANGE_GUILD_SKILL_SLOT_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool Handler_EGS_CHANGE_GUILD_SKILL_SLOT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	*/


	bool Handler_EGS_GET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_GET_GUILD_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_GET_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : ����� [2009.11.19] //

	bool Handler_EGS_RESET_GUILD_SKILL_REQ( CX2SkillTree::SKILL_ID eSkillID );
	bool Handler_EGS_RESET_GUILD_SKILL_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_RESET_GUILD_SKILL_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : ����� [2009.11.19] //

	bool Handler_EGS_INIT_GUILD_SKILL_TREE_REQ( UidType itemUID );
	bool Handler_EGS_INIT_GUILD_SKILL_TREE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_INIT_GUILD_SKILL_TREE_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : ����� [2009.11.19] //

	bool Handler_EGS_EXPIRE_GUILD_CASH_SKILL_POINT_NOT( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ); // oasis907 : ����� [2009.11.27] //

	void Hander_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_REQ( const int iYear_, const int iMonth_, const int iDay_, const int iHour_, const int iMinute_ );
	void Handler_EGS_ADMIN_CASH_SKILL_POINT_DATE_CHANGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


	//}} oasis907 : ����� //// 2009-11-12 ////

	bool Handler_EGS_GET_NEXT_SKILL_PAGE_ED_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_EXPAND_SKILL_PAGE_REQ();
	bool Handler_EGS_EXPAND_SKILL_PAGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	bool Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_REQ( USHORT usPageNumber_ );
	bool Handler_EGS_DECIDE_TO_USE_THIS_SKILL_PAGE_ACK( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	//{{ kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
	// ��ų����ü���� ���ΰ�? (SkillSlotChangeReq�� ������ �� Ack�� ���� ���� ���� �����ΰ�?)
	bool IsSkillSlotChanging() const { return m_bSkillSlotChanging; }
	void SetSkillSlotChanging(bool val) { m_bSkillSlotChanging = val; }
	//}} kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����

	void UnequipRelationSkill();

	/// ��ų ���ø� �� �� ���� �Լ�
	void SetSkillAbilityMap( KLuaManager* pLuaManager, SKILL_ABILITY_TYPE eSkillAbility, CX2SkillTree::SkillTemplet* pSkillTemplet );
	template< typename T >
	void SetSkillVlaue( KLuaManager& pLuaManager, vector<T>& vecValue, bool bAutoFill = true );
	void SetSkillStat( KLuaManager& pLuaManager, vector<CX2Stat::Stat>& vecStat );

	/// ��ų ���� Ȥ�� ��ȭ�� ������ �ѱ�� ���� ������ ó��
	const std::map<int, KGetSkillInfo>& GetMapSkillInfo() const { return m_mapGetSkillInfo; }
	void SetMapSkillInfo(std::map<int, KGetSkillInfo> val) { m_mapGetSkillInfo = val; }
	void ClearMapSkillInfo() { m_mapGetSkillInfo.clear();
	m_vecNowLearnSkill.clear();
	}

	/// ��ų ���� Ȥ�� ��ȭ�� ������ �ѱ�� ���� ������ ���� �����̳��� �� ���� Ȥ�� �߰� �Լ�
	void FixOrInsertMapSkillInfoValue( CX2SkillTree::SKILL_ID eSkillID );

	/// ������� ����� ��ų ����Ʈ
	const int GetUsedSPoint() const { return m_iUsedSPoint; }
	void SetUsedSPoint( int val ) { m_iUsedSPoint = val; }

	/// ������� ����� ĳ�� ��ų ����Ʈ
	const int GetUsedCSPoint() const { return m_iUsedCSPoint; }
	void SetUsedCSPoint( int val ) { m_iUsedCSPoint = val; }

	/// ������ ������ ���� ���̺� �Ľ� �Լ�
	void AddSkillPowerRate_LUA();
	void SetPowerRateValue( SKILL_POWER_RATE_TYPE eSkillPowerRateType );
	void SetSkillPowerRate( IN SKILL_POWER_RATE_TYPE eSkillPowerRateType, OUT vector<float>& vecPowerRate );
	const std::map<SKILL_POWER_RATE_TYPE, std::vector<float>>& GetMapSkillPowerRateType() const { return m_mapSkillPowerRateType; }

	/// ������ �⺻ ��ų ���̺� �Ľ� �Լ�
	void AddDefaultSkill_LUA();
	const std::map<int, std::vector<int>>& GetMapDefaultSkill() const { return m_mapDefaultSkill; }

	/// �ش� ��ų ���̵� �⺻ ��ų���� ����
	bool isDefaultSkill( SKILL_ID eSkillID );

	/// ���� �ɸ����� ��ų���� �˻�
	bool IsMyClassSkillID( SKILL_ID eSkillID );

	/// �ִ� ���� ��ų ������ ��ȯ
	const int GetMaxLimitedSkillLevel() { return MAX_LIMITED_SKILL_LEVEL; }

	/// ���� ��ų ���ø� ��ȯ
	const map<int, SealSkillItemInfo>& GetSealSkillItemTemplet() const { return m_mapSealSkillItemTemplet; }

	/// ������ ���� �� ��ų ����Ʈ ��ȯ
	const int GetCalcInitSkillPoint( const int iUnitLevel ) const;

	void SetSkillTemplet( SKILL_ID  eSkillID, SkillTemplet* pSkillTemplet )
	{
		if ( NULL != pSkillTemplet )
			m_mapSkillTemplet.insert( std::make_pair( eSkillID, pSkillTemplet ) );
	}

private:
	SkillTempletMap			m_mapSkillTemplet;

	std::map<SKILL_POWER_RATE_TYPE, std::vector<float>>	m_mapSkillPowerRateType;	/// �� ������ ���� ������ ���� ���� �����̳�
	std::map<int, std::vector<int>>						m_mapDefaultSkill;			/// �� ������ ���� �⺻ ��ų ���� �����̳�
	std::map<int, KGetSkillInfo>						m_mapGetSkillInfo;			/// ��ų ���� Ȥ�� ��ȭ�� ������ �ѱ�� ���� ������ ���� �����̳�

	bool					m_bNowLearnSkill;				/// ���� ��ųâ���� ��ų ������ �ø��� �ִ°�
	int						m_iUsedSPoint;					/// ��ų ����Ʈ �Һ� �˻縦 ���� �ӽ� SP ���尪
	int						m_iUsedCSPoint;					/// ĳ�� ��ų ����Ʈ �Һ� �˻縦 ���� �ӽ� ĳ�� SP ���尪
	UnitSkillTreeTempletMap	m_mapUnitSkillTreeTemplet;
//#ifdef GUILD_SKILL // oasis907 : ����� [2009.11.25] // ���� �Ľ� ���� �ּ� ����
	//{{ oasis907 : ����� //// 2009-11-12 ////
	GuildSkillTreeTempletMap m_mapGuildSkillTreeTemplet;
	//}} oasis907 : ����� //// 2009-11-12 ////
//#endif GUILD_SKILL

	//{{ 2009. 8. 5  ������		���ν�ų����
	map< int, SealSkillItemInfo > m_mapSealSkillItemTemplet;

	//}}


	//{{ oasis907 : ����� //// 2009-11-17 ////	�ӽ�
	SkillTreeTempletMap nulltemplet;
	//}} oasis907 : ����� //// 2009-11-17 ////

	//{{ kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����
	bool m_bSkillSlotChanging;			// ��ų����ü���� ���ΰ�? (SkillSlotChangeReq�� ������ �� Ack�� ���� ���� ���� �����ΰ�?)
	//}} kimhc // 2010.7.29 // ���Խ� ���� ��ų ���� ����

	vector<int>	m_vecNowLearnSkill;
}; // class CX2SkillTree
