#pragma once

#include "XSLStat.h"


class CXSLInventory;
class CXSLUnit
{
public:
	enum UNIT_TYPE
	{
		UT_NONE	= 0,
		UT_ELSWORD,
		UT_ARME,
		UT_LIRE,
		UT_RAVEN,
		UT_EVE,
		UT_CHUNG,		// kimhc // 2010-12-23 에 추가될 신캐릭터 청
		UT_ARA,			/// JHKang / 신 캐릭터 아라
		UT_ELESIS,
		UT_ADD,
		UT_END,			//  엘리시스가 추가되면 END를 엘리시스 아래로..
	};

	enum UNIT_CLASS
	{
		//초기직업
		UC_NONE = 0,
		UC_ELSWORD_SWORDMAN,
		UC_ARME_VIOLET_MAGE,
		UC_LIRE_ELVEN_RANGER,
		UC_RAVEN_FIGHTER,
		UC_EVE_NASOD,
		UC_CHUNG_IRON_CANNON,
		UC_ARA_MARTIAL_ARTIST,		/// 7 아라 노전직, 무술가
		UC_ELESIS_KNIGHT = 8,
		UC_ADD_NASOD_RULER = 9,


		//일차전직
		UC_ELSWORD_KNIGHT = 10,
		UC_ELSWORD_MAGIC_KNIGHT,
		UC_LIRE_COMBAT_RANGER,
		UC_LIRE_SNIPING_RANGER,
		UC_ARME_HIGH_MAGICIAN,
		UC_ARME_DARK_MAGICIAN,
		UC_RAVEN_SOUL_TAKER,
		UC_RAVEN_OVER_TAKER,
		UC_EVE_EXOTIC_GEAR,
		UC_EVE_ARCHITECTURE,
		UC_CHUNG_FURY_GUARDIAN,		// 20	// kimhc // 청 1차 전직
		UC_CHUNG_SHOOTING_GUARDIAN,	// 21	// kimhc // 청 1차 전직
		UC_ELSWORD_SHEATH_KNIGHT,	// 22
		UC_ARME_BATTLE_MAGICIAN,	// 23
		UC_LIRE_TRAPPING_RANGER			= 24,	// 24	레나 트래핑 레인저
		UC_RAVEN_WEAPON_TAKER			= 25,
		UC_EVE_ELECTRA					= 26,
		UC_CHUNG_SHELLING_GUARDIAN		= 27,
		UC_ARA_LITTLE_HSIEN				= 28,	// 소선 - 아라 1차 전직

		UC_ELESIS_SABER_KNIGHT			= 29,	// 엘리시스 1-1차 세이버 나이트
		UC_ELESIS_PYRO_KNIGHT			= 30,	// 엘리시스 1-2차 파이로 나이트

		UC_ARA_LITTLE_DEVIL				= 31,	// 소마 - 아라 1차 전직

		UC_ADD_PSYCHIC_TRACER			= 32,	// 사이킥 트레이서 - 애드 1차 전직



		// 2차 전직
		UC_ELSWORD_LORD_KNIGHT		= 100,
		UC_ELSWORD_RUNE_SLAYER,		// 101
		UC_LIRE_WIND_SNEAKER,		// 102
		UC_LIRE_GRAND_ARCHER,		// 103
		UC_ARME_ELEMENTAL_MASTER,	// 104
		UC_ARME_VOID_PRINCESS,		// 105
		UC_RAVEN_BLADE_MASTER,		// 106
		UC_RAVEN_RECKLESS_FIST,		// 107
		UC_EVE_CODE_NEMESIS,		// 108
		UC_EVE_CODE_EMPRESS,		// 109
		UC_CHUNG_IRON_PALADIN,		// 110
		UC_CHUNG_DEADLY_CHASER,		// 111

		UC_ELSWORD_INFINITY_SWORD,	// 112
		UC_ARME_DIMENSION_WITCH,	// 113
		UC_LIRE_NIGHT_WATCHER,		// 114
		UC_RAVEN_VETERAN_COMMANDER,	// 115

		UC_EVE_BATTLE_SERAPH,		// 116

		UC_CHUNG_TACTICAL_TROOPER,	// 117

		UC_ARA_SAKRA_DEVANAM		= 118,	// 제천 - 아라 2차 전직

		UC_ARA_YAMA_RAJA			= 119,	// 명왕 - 아라 2차 전직


		UC_ELESIS_GRAND_MASTER		= 120,	// 김종훈 // 2013-09-04 // 엘리시스 1-2 그랜드 마스터
		UC_ELESIS_BLAZING_HEART		= 121,	// 김종훈 // 2013-09-04 // 엘리시스 2-2 블레이징 하트

		UC_ADD_LUNATIC_PSYKER		= 122,	// 루나틱 사이커 - 애드 2차 전직


		//{{ 2011. 11. 21  김민성	전직 변경 아이템
		UC_MAX,		// 마지막 값
		//}}
	};

	//{{ 이벤트 몬스터 출현 ( 아군 몬스터	) - 김민성
	enum NPC_AI_TYPE
	{
		NAT_NORMAL,			//
		NAT_ALLY,			// 소환된 친구 NPC
		NAT_NO_BRAIN,
	};
	//}}

	enum EQIP_POSITION
	{
		EP_NONE		= 0,
		EP_QUICK_SLOT,		//퀵슬롯 아이템

		//무기
		EP_WEAPON_HAND,			//무기
		EP_WEAPON_TEMP1,		//임시1
		EP_WEAPON_TEMP2,		//임시2
		EP_WEAPON_TEMP3,		//임시3

		//방어구
		EP_DEFENCE_HAIR,		//헤어스타일
		EP_DEFENCE_FACE,		//얼굴
		EP_DEFENCE_BODY,		//상의
		EP_DEFENCE_LEG,			//하의
		EP_DEFENCE_HAND,		//장갑
		EP_DEFENCE_FOOT,		//신발
		EP_DEFENCE_TEMP1,		//임시1
		EP_DEFENCE_TEMP2,		//임시2
		EP_DEFENCE_TEMP3,		//임시3

		//액세서리
		EP_AC_TITLE,			//칭호
		EP_AC_HAIR,				//헤어
		EP_AC_FACE1,			//얼굴(상)
		EP_AC_FACE2,			//얼굴(중)
		EP_AC_FACE3,			//얼굴(하)
		EP_AC_BODY,				//상의
		EP_AC_LEG,				//다리
		EP_AC_ARM,				//팔
		EP_AC_RING,				//반지
		EP_AC_NECKLESS,			//목걸이
		EP_AC_WEAPON,			//무기 악세사리
		EP_AC_TEMP2,			//임시2
		EP_AC_TEMP3,			//임시3
		EP_AC_TEMP4,			//임시4
		EP_AC_TEMP5,			//임시5

		//필살기
		EP_SKILL_1,				//1단계 필살기
		EP_SKILL_2,				//2단계 필살기
		EP_SKILL_3,				//3단계 필살기
		EP_SKILL_TEMP1,			//임시1
		EP_SKILL_TEMP2,			//임시2
		EP_SKILL_TEMP3,			//임시3

		EP_RAVEN_LEFT_ARM,		// dmlee 2008.07.31 - 레이븐 왼팔, 오른팔 나누면서
		EP_WEAPON_SECOND,

		EP_ONEPIECE_FASHION,	// 원피스 아바타

		EP_END,
	};

	enum NOW_EQIP_SLOT_ID
	{
		NESI_DEFENCE_HAIR_FASHION = 0,

		NESI_DEFENCE_BODY,
		NESI_DEFENCE_BODY_FASHION,

		NESI_DEFENCE_LEG,
		NESI_DEFENCE_LEG_FASHION,

		NESI_DEFENCE_HAND,
		NESI_DEFENCE_HAND_FASHION,

		NESI_DEFENCE_FOOT,
		NESI_DEFENCE_FOOT_FASHION,

		NESI_WEAPON_HAND,
		NESI_WEAPON_HAND_FASHION,

		NESI_AC_FACE1,				//얼굴(상)
		NESI_AC_FACE2,				//얼굴(중)
		NESI_AC_FACE3,				//얼굴(하)
		NESI_AC_BODY,				//상의
		NESI_AC_LEG,				//다리
		NESI_AC_ARM,				//팔
		NESI_AC_RING,				//반지
		NESI_AC_NECKLESS,			//목걸이
		NESI_AC_WEAPON,				//무기 악세사리

		NESI_ONEPIECE_FASHION,		//원피스 아바타

		NESI_END,
	};

	enum NOW_SKILL_SLOT_ID
	{
		NSSI_SKILL_1 = 0,			//1단계 필살기
		NSSI_SKILL_2,				//2단계 필살기
		NSSI_SKILL_3,				//3단계 필살기

		NSSI_END,
	};

	enum NOW_QUICK_SLOT_ID
	{
		NQSI_QUICK_SLOT_1 = 0,
		NQSI_QUICK_SLOT_2,
		NQSI_QUICK_SLOT_3,
		NQSI_QUICK_SLOT_4,
		NQSI_QUICK_SLOT_5,
		NQSI_QUICK_SLOT_6,

		NQSI_END,
	};

	enum CONNECT_UNIT_STATE
	{
		CUS_NONE		= 0,
		CUS_FIELD_MAP,
		CUS_MARKET,
		CUS_PVP_LOBBY,
		CUS_PVP_WAIT,
		CUS_PVP_PLAY,
		CUS_TC_PLAY,
		CUS_DUNGEON_PLAY,
	};

	//{{ 2011. 06. 20	최육사	대전 개편
		//{{ 2012. 06. 19	박세훈	2012 대전 시즌2
	enum PVP_EMBLEM
	{
		PE_NONE			= -1,
		PE_RANK_E		= 0,
		PE_RANK_D		= 251,
		PE_RANK_C		= 551,
		PE_RANK_B		= 951,
		PE_RANK_A		= 1351,
		PE_RANK_S		= 1851,
		PE_RANK_SS		= 2351,
		PE_RANK_SSS		= 2951,
		PE_END			= 3300,
	};

	enum PVP_RANK
	{
		PVPRANK_NONE = 0,
		PVPRANK_RANK_ARRANGE,
		PVPRANK_RANK_E,
		PVPRANK_RANK_D,
		PVPRANK_RANK_C,
		PVPRANK_RANK_B,
		PVPRANK_RANK_A,
		PVPRANK_RANK_S,
		PVPRANK_RANK_SS,
		PVPRANK_RANK_SSS,

		PVPRANK_END,
	};
		//}}
	//}}

	//{{ 2008. 12. 14  최육사	캐릭터 슬롯
	enum CHAR_SLOT
	{
		CHAR_SLOT_MAX = 10,

	};
	//}}

	//{{ 2011. 07. 08    김민성    옵션 수치화
//#ifdef SERV_USE_PERCENT_IN_OPTION_DATA
	enum UNIT_PERCENT_HP
	{
		UPHP_INIT				= 100,    // 레벨 1의 1 단위 값
		UPHP_INCREASE_ONE_POINT = 15,     // 레벨당 증가되는 단위 증분값
	};
//#endif SERV_USE_PERCENT_IN_OPTION_DATA
	//}}

	//{{ 2011. 05. 16  김민성	칭호 획득 조건 추가
//#ifdef SERV_ADD_TITLE_CONDITION
	enum USER_UNIT_DIE_REASON			// 유저가 죽은 이유
	{
		UUDR_EMPTY_OXYGEN = 1,			// 산소 부족으로 인한 사망
	};
//#endif SERV_ADD_TITLE_CONDITION
	//}}

//#ifdef	SERV_UPGRADE_SKILL_SYSTEM_2013 // 적용날짜: 2013-06-27
	enum UNIT_CLASS_TYPE
	{
		UCT_NONE			= -1,
		UCT_BASIC_CLASS		= 0,	// 기본 전직
		UCT_FIRST_CLASS		= 1,	// 1차 전직
		UCT_SECOND_CLASS	= 2,	// 2차 전직
		UCT_END				= 3,
	};
//#endif	// SERV_UPGRADE_SKILL_SYSTEM_2013

	struct UnitTemplet
	{
		UNIT_TYPE			m_UnitType;
		UNIT_CLASS			m_UnitClass;

		std::wstring		m_Name;
		std::wstring		m_Description;

		std::wstring		m_MotionFile;

		CXSLStat::Stat		m_Stat;

		int					m_BasicWeaponItemID;
		int					m_BasicHairItemID;
		int					m_BasicFaceItemID;
		int					m_BasicBodyItemID;
		int					m_BasicLegItemID;
		int					m_BasicHandItemID;
		int					m_BasicFootItemID;

		int					m_RavenLeftArmItemID;	// dmlee 2008.07.31 - 레이븐 왼팔, 오른팔 나누면서
		int					m_SecondWeaponItemID;
		UnitTemplet()
		{
			m_UnitType				= UT_NONE;
			m_UnitClass				= UC_NONE;

			m_BasicWeaponItemID		= 0;
			m_BasicHairItemID		= 0;
			m_BasicFaceItemID		= 0;
			m_BasicBodyItemID		= 0;
			m_BasicLegItemID		= 0;
			m_BasicHandItemID		= 0;
			m_BasicFootItemID		= 0;

			m_RavenLeftArmItemID	= 0;
			m_SecondWeaponItemID	= 0;
		}
	};

	struct UnitData
	{
		UidType					m_UnitUID;
		UidType					m_UserUID;
		UNIT_CLASS				m_UnitClass;

		std::wstring			m_IP;
		int						m_Port;

		std::wstring			m_NickName;

		int						m_ED;
		//{{ 2011. 07. 11	최육사	대전 개편
		int						m_Rating;
		int						m_RPoint;
		int						m_APoint;
		//}}
		int						m_Level;
		int						m_EXP;

		int						m_Win;
		int						m_Lose;
		int						m_Seceder;

		int						m_NowBaseLevelEXP;
		int						m_NextBaseLevelEXP;

		int						m_EquipInventorySize;
		int						m_SkillInventorySize;
		int						m_MaterialInventorySize;
		int						m_SpecialInventorySize;
		int						m_CardInventorySize;
		int						m_QuickSlotInventorySize;
		int						m_AvartaInventorySize;

		int						m_QuickSlotSize;

		CXSLStat::Stat 			m_Stat;
		int						m_RemainStatPoint;

		std::vector<UidType>	m_NowEqipItemUIDList;
		CXSLInventory*			m_pInventory;

		int						m_nStraightVictories;
		int						m_nMapID;

		bool					m_bIsGameBang;

		UnitData& operator=( const KUnitInfo& data );

		UnitData();
		~UnitData();
	};

public:
	CXSLUnit(void);
	~CXSLUnit(void);

	//{{ 2012. 09. 11	박세훈	2012 대전 시즌2
	static PVP_RANK			ComputeNewPVPRank( IN PVP_RANK eCurrentPvpRank, IN const int iNowRating );
	//}}

	static bool				IsValidUnitClass( UNIT_CLASS eUnitClass ) { return ( GetUnitClassToUnitType( eUnitClass ) != UT_NONE ); }
	static UNIT_CLASS		GetUnitClassDownGrade( UNIT_CLASS eUnitClass );
	static UNIT_CLASS		GetUnitClassBaseGrade( UNIT_CLASS eUnitClass );
	static UNIT_CLASS		GetUnitClassUpGrade( UNIT_CLASS eUnitClass );
	static UNIT_TYPE		GetUnitClassToUnitType( UNIT_CLASS eUnitClass );
	//}}

	//{{ 2009. 11. 10  최육사	2차전직
	static bool				IsFirstChangeJob( CXSLUnit::UNIT_CLASS unitClass );
	static bool				IsSecondChangeJob( CXSLUnit::UNIT_CLASS unitClass );
	//}}

	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	static bool				IsInitNormalJob( CXSLUnit::UNIT_CLASS unitClass );					// 초기 직업인가?
	static bool				IsUnBalanceUnitClass( IN char cUnitType, IN char cUnitClass, IN char cItemUnitClass, OUT int& iNewUnitClass );
	//}}

	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	static bool				IsMan_UnitType( UNIT_TYPE eUnitType );
	//}
};
