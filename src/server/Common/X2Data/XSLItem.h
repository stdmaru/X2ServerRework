#pragma once

#include "XSLUnit.h"

#define MAX_SPECIAL_ABILITY_NUM		 3
#define MAX_ENCHANT_CARD_NUM		10

class CXSLItem
{
	//{{ dmlee 2008.11.29
	public:
		static const int		MAX_MODEL_COUNT_A_ITEM = 6;
	//}} dmlee 2008.11.29

	public:
		enum ITEM_TYPE
		{
			IT_NONE = 0,		//에러
			IT_WEAPON,			//무기
			IT_DEFENCE,			//방어구
			IT_ACCESSORY,		//액세서리
			IT_SKILL,			//스킬
			IT_QICK_SLOT,		//퀵슬롯 아이템(소비성)
			IT_MATERIAL,		//재료
			IT_SPECIAL,			//특수
			IT_QUEST,			//퀘스트
			IT_OUTLAY,			//순간소비성
			IT_ETC,				//기타
			//{{ 2010. 03. 23  최육사	기술의 노트
			IT_SKILL_MEMO,		//기술의노트
			//}}
			//#ifdef SERV_UPGRADE_TRADE_SYSTEM // 김태환
			IT_PET,				//펫
			IT_RIDING,			//탈것
			//#endif //SERV_UPGRADE_TRADE_SYSTEM
		};

		enum ITEM_GRADE
		{
			IG_NONE = 0,
			IG_UNIQUE,
			IG_ELITE,
			IG_RARE,
			IG_NORMAL,
			IG_LOW,
		};

		enum PERIOD_TYPE
		{
			PT_INFINITY = 0,	//무제한
			PT_ENDURANCE,		//내구도
			PT_QUANTITY,		//수량성
		};

		//{{ 2009. 8. 26  최육사	밀봉 아이템
		enum ITEM_STATE
		{
			IS_NORMAL = 0,
			IS_SEALED,
		};
		//}}

		enum SHOP_PRICE_TYPE
		{
			SPT_NONE = 0,		//팔기 불능
			SPT_CASH,			//캐쉬
			SPT_GP,				//GP
		};

		enum USE_CONDITION
		{
			UC_NONE = 0,		// 아무도 사용할 수 없음
			UC_ANYONE,			// 아무나 사용할 수 있음
			UC_ONE_UNIT,		// 지정된 unit_type 만
			UC_ONE_CLASS,		// 지정된 unit_class 만
		};

		enum USE_TYPE
		{
			UT_NONE = 0,		// 장착하는 장비가 아님.
			UT_SKIN,			// 장착하는 장비, 스킨 애니메이션
			UT_ATTACH_ANIM,		// 장착하는 장비, 어태치 애니메이션
			UT_ATTACH_NORMAL,	// 장착하는 장비, 어태치 메시
		};

		enum SPECIAL_SKILL_TYPE
		{
			SST_THROW_WOODEN_PIECE = 1,
			SST_MAGIC_SPEAR,
			SST_ICE_CRYSTAL,
			SST_FIRE_CRYSTAL,
			SST_MAGIC_BULLET,
			SST_POISON_POCKET,
		};

		enum SPECIAL_SKILL_MOTION_TYPE
		{
			SSMT_THROW = 1,		//던지는 형식
			SSMT_INSTALL,		//설치하는 형식
		};

		enum SUPPLEMENT_ITEM_TYPE
		{
			SIT_PLUS_SUCCESS_RATE = 0,
			SIT_NOT_BROKEN,
		};

		//{{ 2010. 07. 01  최육사	내구도 개편
		enum ENDURANCE
		{
			EN_MAX_ENDURANCE = 10000,
		};
		//}}

        enum SPECIAL_ABILITY_TYPE
        {
            SAT_NONE = 0,                    /// 없음
            SAT_HP_UP,                        /// HP증가
            SAT_MP_UP,                        /// MP증가
            SAT_REMOVE_CURSE,                /// 저주제거
            SAT_REMOVE_SLOW,                /// 슬로우제거
            SAT_REMOVE_FIRE,                /// 화염제거
            SAT_REMOVE_POISON,                /// 독제거
            SAT_RECIPE,                        /// 레시피 아이템
            SAT_SUPPLEMENT_ITEM,            /// 보충제 아이템
            SAT_HYPER_MODE,                    /// 각성
            SAT_SOUL_GAGE_UP,                /// 각성게이지 상승
            SAT_SHOW_OPPONENT_MP,            /// 상대팀 MP 보기
            SAT_UP_MP_AT_ATTACK_OR_DAMAGE,    /// 때리거나 맞을때 MP 상승
            SAT_HP_PERCENT_UP,                /// HP 퍼센트 증가
            SAT_TEAM_HP_UP,                    /// 팀원들 전부 HP % 증가
            SAT_TEAM_MP_UP,                    /// 팀원들 전부 MP % 증가
            SAT_PEPPER_RUN,                    /// 고추먹고 맴맴
            SAT_TRANSFORM_UNIT_SCALE,        /// 유닛 스케일 조정
            SAT_SPECIAL_SKILL,                /// 소비성 아이템을 사용함에 의한 특수 기술
            SAT_MP_PERCENT_UP,                /// MP 퍼센트 증가

            //#ifdef DUNGEON_ITEM
            SAT_POWER_RATE_UP,                /// 공격력 증가 (%)
            SAT_MOVE_SPEED_UP,                /// 이동 속도 증가 (%)
            SAT_JUMP_SPEED_UP,                /// 점프 속도 증가 (%)
            SAT_MP_REGENERATION,            /// 초당 mp 회복
            SAT_USE_HYPER,                    /// 각성구슬 없이 강제 각성
            SAT_PHYSIC_ATTACK_UP,            /// 물리 공격력 증가 (%)
            SAT_MAGIC_ATTACK_UP,            /// 마법 공격력 증가 (%)
            SAT_PHYSIC_DEFENCE_UP,            /// 물리 방어력 증가 (%)
            SAT_MAGIC_DEFENCE_UP,            /// 마법 방어력 증가 (%)
            SAT_SUPERARMOR,                    /// 슈퍼아머
            SAT_SUMMON_SPIRIT,                /// 정령소환
            //#endif //DUNGEON_ITEM
            SAT_REMOVE_FROZEN,                // 냉기제거
            SAT_TRANSFORM_MONSTER,            // 몬스터 변신
            SAT_WAY_OF_SWORD_GAUGE_UP,        // 검의길 게이지 증가
            //#ifdef SUMMON_MONSTER_CARD_SYSTEM
            SAT_SUMMON_MONSTER,                // 몬스터 소환 기능
            //#endif //SUMMON_MONSTER_CARD_SYSTEM
            SAT_FORCE_CRITICAL_MAX,            // 다음 타격 크리 100%
            SAT_ENABLE_ATTACK_MONSTER,         // 해외 할로윈 이벤트
            //#ifdef ARA_FORCE_POWER_ITEM
            SAT_ARA_FORCE_POWER_PERCENT_UP,    // 해외 아라 기력 증가 (%)
            //#endif ARA_FORCE_POWER_ITEM
			//#ifdef RETURN_TO_BASE_AT_PVE
			SAT_RETURN_TO_BASE_AT_PVE,			// PVE 귀환석
			//#endif // RETURN_TO_BASE_AT_PVE
			//#ifdef RIDINGPET_STAMINA_ITEM
			SAT_RIDINGPET_STAMINA_PERCENT_UP,    // 해외 라이딩펫 스테미너 증가 (%)
			//#endif RIDINGPET_STAMINA_ITEM
            SAT_END,
        };

		//{{ 2008. 4. 22  최육사  캐쉬 아이템
		enum CASH_ITEM
		{
			CI_RESURRECTION_STONE 			= 127030, // 부활석
			CI_EXPAND_INVENTORY	  			= 200750, // 소지품 확장
			CI_EXPAND_SKILL_SLOT  			= 200880, // 스킬슬롯B확장 기간제
			CI_EXPAND_SKILL_SLOT_PERMANENT  = 200885, // 스킬슬롯B확장 영구아이템
			CI_EXPAND_SKILL_SLOT_ARA		= 261630, // 스킬슬롯B확장 아라 전용 판매 캐쉬템
			CI_EXPAND_SKILL_SLOT_ELESIS		= 261661, // 스킬슬롯B확장 엘리시스 전용 판매 캐쉬템

			CI_CASH_SKILL_POINT_30_15		= 271076,	// 30SP 15일권
			CI_CASH_SKILL_POINT_60_15		= 271077,	// 60SP 15일권
			CI_CASH_SKILL_POINT_30_30		= 271078,	// 30SP 30일권
			CI_CASH_SKILL_POINT_60_30		= 271079,	// 60SP 30일권

			CI_SKILL_RESET_ITEM				= 200730, // 스킬 한개 초기화
			CI_SKILL_RESET_EVENT_ITEM		= 130808, // 스킬 한개 초기화
			CI_SKILL_RESET_EVENT_ITEM2		= 160947, // 스킬 한개 초기화
			EI_SKILL_RESET_EVENT_ITEM2		= 67003592, // 망각의 알약(스킬 한개 초기화)
			EI_SKILL_RESET_EVENT_ITEM3		= 67006745, // 망각의 알약(스킬 한개 초기화)

			CI_SKILL_INIT_ITEM				= 203800, // 스킬 전체 초기화
			CI_SKILL_INIT_EVENT_ITEM		= 130633, // 스킬 전체 초기화 이벤트 아이템

			CI_SKILL_INIT_EVENT_ITEM2		= 135416, // 스킬 전체 초기화 이벤트 아이템2
			CI_SKILL_INIT_EVENT_ITEM3		= 135417, // 스킬 전체 초기화 이벤트 아이템3
			CI_SKILL_INIT_EVENT_ITEM4		= 160946, // 스킬 전체 초기화 이벤트 아이템4

			CI_SKILL_INIT_EVENT_ITEM_JP		= 65000430, // 스킬 전체 초기화 이벤트 아이템 국내 130633


			CI_EXPAND_CHAR_SLOT_1 			= 206820, // 캐릭터 슬롯 확장 1
			CI_EXPAND_CHAR_SLOT_2 			= 206830, // 캐릭터 슬롯 확장 2
			CI_CHANGE_NICKNAME	  			= 209660, // 캐릭터 닉네임 변경 카드
			CI_PREMIUM_SHOP		  			= 200740, // 프리미엄 개인상점

			//{{ 2009. 11. 24  최육사	길드스킬
			CI_GUILD_SKILL_RESET_ITEM		= 110857, // 망각의 피리
			CI_GUILD_SKILL_INIT_ITEM		= 110858, // 망각의 뿔피리
			//}}

			//{{ 2009. 8. 7  최육사		은행
			CI_BANK_MEMBERSHIP_SILVER		= 213460, // 실버 회원권
			CI_BANK_MEMBERSHIP_GOLD			= 213470, // 골드 회원권
			CI_BANK_MEMBERSHIP_EMERALD		= 213480, // 에메랄드 회원권
			CI_BANK_MEMBERSHIP_DIAMOND		= 213490, // 다이아몬드 회원권
			CI_BANK_MEMBERSHIP_PLATINUM		= 213500, // 플래티넘 회원권
			CI_BANK_MEMBERSHIP_ONE_PACKAGE	= 213610, // 코보 신용 보증서
			//}}

			//{{ 2011. 12. 13  김민성	퀵 슬롯 확장 아이템
			CI_EXPAND_QUICK_SLOT			= 244560, // 퀵슬롯 확장권
			CI_EXPAND_QUICK_SLOT_ARA		= 261650,	// 퀵슬롯 확장권(아라 전용)
			//}}
			CI_EXPAND_QUICK_SLOT_ELESIS		= 261663,	// 퀵슬롯 확장권(엘리시스 전용)

			CI_NASOD_MEGAPHONE				= 212350, // 메가폰
			CI_HIGH_NASOD_MEGAPHONE			= 212360, // 고성능 메가폰

			//{{ 2009. 9. 1  최육사		밀봉 아이템
			CI_SEAL_ITEM_NORMAL				= 214370, // 뽀루의 발 도장(하얀색)
			CI_SEAL_ITEM_RARE				= 214380, // 뽀루의 발 도장(노란색)
			CI_SEAL_ITEM_ELITE				= 214390, // 뽀루의 발 도장(보라색)
			CI_SEAL_ITEM_UNIQUE				= 214400, // 뽀루의 발 도장(황금색)
			//}}

			//{{  2011.12.21     김민성    코보 발도장 이벤트
			CI_EVENT_SEAL_ITEM_NORMAL				= 160372, // 코보 뽀루의 발 도장(하얀색)
			CI_EVENT_SEAL_ITEM_ELITE				= 160373, // 코보 뽀루의 발 도장(보라색)

			CI_EVENT_SEAL_ITEM_UNIQUE		= 160794, // [코보]뽀루의 발 도장 (황금색)

			//}}

			//{{ 2009. 9. 22  최육사	전직캐쉬
			CI_CHANGE_JOB_ELSWORD_KNIGHT	= 214410, // 소드 나이트의 증명
			CI_CHANGE_JOB_MAGIC_KNIGHT		= 214420, // 매직 나이트의 증명
			CI_CHANGE_JOB_COMBAT_RANGER		= 214450, // 컴뱃 레인저의 영혼
			CI_CHANGE_JOB_SNIPING_RANGER	= 214460, // 스나이핑 레인저의 영혼
			CI_CHANGE_JOB_HIGH_MAGICIAN		= 214430, // 하이 매지션의 메모라이즈
			CI_CHANGE_JOB_DARK_MAGICIAN		= 214440, // 다크 매지션의 메모라이즈
			CI_CHANGE_JOB_SOUL_TAKER		= 214470, // 소드 테이커의 계시
			CI_CHANGE_JOB_OVER_TAKER		= 214480, // 오버 테이커의 계시
			CI_CHANGE_JOB_EXOTIC_GEAR		= 214490, // 코드 엑조틱의 메모리
			CI_CHANGE_JOB_ARCHITECTURE		= 214500, // 코드 아키텍처의 메모리
			//}}
			//{{ 2009. 10. 24  최육사	엘소드2차전직
			CI_CHANGE_JOB_LORD_KNIGHT		= 216420, // 로드 나이트 임명장
			CI_CHANGE_JOB_RUNE_SLAYER		= 216430, // 룬 슬레이어 징표
			//}}
			//{{ 2009. 11. 23  최육사	레나2차전직
			CI_CHANGE_JOB_WIND_SNEAKER		= 217850, // 윈드 스니커 전직 캐시 아이템
			CI_CHANGE_JOB_GRAND_ARCHER		= 217860, // 그랜드 아처 전직 캐시 아이템
			//}}
			//{{ 2009. 12. 11  최육사	아이샤 2차 전직
			CI_CHANGE_JOB_ELMENTAL_MASTER	= 217920, // 엘리멘탈 마스터 전직 아이템
			CI_CHANGE_JOB_VOID_PRINCESS		= 217930, // 보이드 프린세스 전직 아이템
			//}}
			//{{ 2010. 1. 20  손영준	레이븐/이브 2차 전직
			CI_CHANGE_JOB_BLADE_MASTER		= 217950, // 블레이드 마스터 전직 아이템
			CI_CHANGE_JOB_RECKLESS_FIST		= 217960, // 레크리스 피스트 전직 아이템
			CI_CHANGE_JOB_NEMESIS			= 217970, // 코드 네미시스 전직 아이템
			CI_CHANGE_JOB_EMPRESS			= 217980, // 코드 엠프레스 전직 아이템
			//}}
			//{{ 2010. 04. 26  최육사	기술의 노트
			CI_CASH_SKILL_NOTE_ITEM			= 221600,  // 캐쉬 기술의 노트 아이템
			//}}

			//{{ kimhc // 2011.1.3 // 청 1차 전직
			CI_CHANGE_JOB_FURY_GUARDIAN		= 229460,	// 퓨리 가디언의 결의
			CI_CHANGE_JOB_SHOOTER_GUARDIAN	= 229470,	// 슈터 가디언의 결의
			//}} kimhc // 2011.1.3 // 청 1차 전직

			//{{ JHKang / 강정훈 / 2011.6.13 / 청 2차 전직
			CI_CHANGE_JOB_IRON_PALADIN		= 235120,	/// 아이언 팔라딘의
			CI_CHANGE_JOB_DEADLY_CHASER		= 235130,	/// 데들리 체이서의
			//}}
			CI_UNLIMITED_CHANGE_SECOND_JOB			= 153000168,	//무제한 2차 전직 아이템

			//{{ 2011. 04. 14	최육사	대리 상인
			CI_PERSONAL_SHOP_AGENCY			= 233210,	// 대리 판매 보증서
			//}}

			CI_CHANGE_JOB_SHEATH_KNIGHT		= 242310,	// 시스 나이트 전직 캐시 아이템
			CI_CHANGE_JOB_BATTLE_MAGICIAN	= 242320,	// 배틀 매지션 전직 캐시 아이템
			CI_CHANGE_JOB_TRAPPING_RANGER	= 242330,	// 트래핑 레인져 전직 캐시 아이템
			CI_CHANGE_JOB_WEAPON_TAKER		= 242340,	// 웨폰 테이커 전직 캐시 아이템
			CI_CHANGE_JOB_EVE_ELECTRA		= 242350,	// 일렉트라 전직 캐시 아이템
			CI_CHANGE_JOB_SHELLING_GUARDIAN	= 242360,	// 쉘링 가디언

			CI_CHANGE_JOB_INFINITY_SWORD	= 252640,
			CI_CHANGE_JOB_DIMENSION_WITCH	= 252650,
			CI_CHANGE_JOB_NIGHT_WATCHER		= 252660,
			CI_CHANGE_JOB_VETERAN_COMMANDER	= 252670,
			CI_CHANGE_JOB_BATTLE_SERAPH		= 252680,
			CI_CHANGE_JOB_TACTICAL_TROOPER	= 252690,
			CI_CHANGE_JOB_LITTLE_HSIEN		= 264380,			/// 아라 소선 전직 아이템
			CI_CHANGE_JOB_SAKRA_DEVANAM		= 264390,			/// 아라 제천 전직 아이템

			CI_CHANGE_JOB_SABER_KNIGHT		= 264391,			/// 엘리시스 세이버나이트 전직 아이템
			CI_CHANGE_JOB_PYRO_KNIGHT		= 264392,			/// 엘리시스 파이로나이트 전직 아이템

			CI_CHANGE_JOB_LITTLE_DEVIL		= 264393,			/// 아라 소마 전직 아이템
			CI_CHANGE_JOB_YAMA_RAJA			= 264394,			/// 아라 명왕 전직 아이템

			CI_CHANGE_JOB_GRAND_MASTER		= 272169,			/// 엘리시스 그랜드 마스터 전직 아이템
			CI_CHANGE_JOB_BLAZING_HEART		= 272170,			/// 엘리시스 블레이징 하트 전직 아이템

			CI_CHANGE_JOB_PSYCHIC_TRACER	= 272810,			/// 애드 사이킥 트레이서 전직 아이템

			CI_CHANGE_JOB_LUNATIC_PSYKER	= 273232,			/// 애드 루나틱 사이커 전직 아이템


			//{{ 2011. 11. 30	최육사	패키지 상품 추가
			CI_SKILL_POINT_30_USE_INVEN		= 241780,	// 인벤토리에서 클릭해서 사용하는 그노시스 축복
			CI_SKILL_POINT_60_USE_INVEN		= 241790,	// 인벤토리에서 클릭해서 사용하는 그노시스 축복
			CI_SKILL_POINT_60_USE_INVEN_ARA	= 261660,	// 그노시스 축복(아라전용)
			CI_EXPAND_SKILL_SLOT_IN_PACKAGE	= 241800,	// 스킬 슬롯 체인지 메달
			//}}

			//{{ 2011. 11. 21  김민성	전직 변경 아이템
			CI_CLASS_CHANGE_ELSWORD_KNIGHT			= 241950,
			CI_CLASS_CHANGE_ELSWORD_MAGIC_KNIGHT	= 241960,
			CI_CLASS_CHANGE_ELSWORD_SHEATH_KNIGHT	= 241970,

			CI_CLASS_CHANGE_AISHA_HIGH_MAGICIAN		= 241980,
			CI_CLASS_CHANGE_AISHA_DARK_MAGICIAN		= 241990,
			CI_CLASS_CHANGE_AISHA_BATTLE_MAGICIAN	= 242000,

			CI_CLASS_CHANGE_RENA_COMBAT_RANGER		= 242010,
			CI_CLASS_CHANGE_RENA_SNIPING_RANGER		= 242020,
			CI_CLASS_CHANGE_RENA_TRAPPING_RANGER	= 242030,

			CI_CLASS_CHANGE_REVEN_SOUL_TAKER		= 242040,
			CI_CLASS_CHANGE_REVEN_OVER_TAKER		= 242050,
			CI_CLASS_CHANGE_RAVEN_WEAPON_TAKER		= 242060,

			CI_CLASS_CHANGE_EVE_EXOTIC_GEAR			= 242070,
			CI_CLASS_CHANGE_EVE_ARCHITECTURE		= 242080,
			CI_CLASS_CHANGE_EVE_ELECTRA				= 242090,

			CI_CLASS_CHANGE_CHUNG_FURY_GUARDIAN		= 242100,
			CI_CLASS_CHANGE_CHUNG_SHOOTER_GUARDIAN	= 242110,
			CI_CLASS_CHANGE_CHUNG_SHELLING_GUARDIAN	= 242120,

			CI_CLASS_CHANGE_ELSWORD_LORD_KNIGHT		= 242130,
			CI_CLASS_CHANGE_ELSWORD_RUNE_SLAYER		= 242140,
			CI_CLASS_CHANGE_ELSWORD_INFINITY_SWORD	= 242150,
			CI_CLASS_CHANGE_AISHA_VOID_PRINCESS		= 242160,
			CI_CLASS_CHANGE_AISHA_ELMENTAL_MASTER	= 242170,
			CI_CLASS_CHANGE_AISHA_DIMENSION_WITCH	= 242180,

			CI_CLASS_CHANGE_RENA_WIND_SNEAKER		= 242190,
			CI_CLASS_CHANGE_RENA_GRAND_ARCHER		= 242200,
			CI_CLASS_CHANGE_RENA_NIGHT_WATCHER		= 242210,

			CI_CLASS_CHANGE_REVEN_BLADE_MASTER		= 242220,
			CI_CLASS_CHANGE_REVEN_RECKLESS_FIST		= 242230,
			CI_CLASS_CHANGE_RAVEN_VETERAN_COMMANDER	= 242240,

			CI_CLASS_CHANGE_EVE_NEMESIS				= 242250,
			CI_CLASS_CHANGE_EVE_EMPRESS				= 242260,
			CI_CLASS_CHANGE_EVE_BATTLE_SERAPH		= 242270,

			CI_CLASS_CHANGE_CHUNG_IRON_PALADIN		= 242280,
			CI_CLASS_CHANGE_CHUNG_DEADLY_CHASER		= 242290,
			CI_CLASS_CHANGE_CHUNG_TACTICAL_TROOPER	= 242300,

			CI_CLASS_CHANGE_ELESIS_SABER_KNIGHT		= 252754,
			CI_CLASS_CHANGE_ELESIS_PYRO_KNIGHT		= 252755,


			CI_CLASS_CHANGE_UC_ARA_LITTLE_HSIEN		= 252757,	/// 아라 1차 전직 소선 전직 변경권
			CI_CLASS_CHANGE_ARA_LITTLE_DEVIL		= 252756,	/// 아라 1차 전직 소마 전직 변경권

			CI_CLASS_CHANGE_ARA_SAKRA_DEVANAM		= 252751,	/// 아라 2차 전직 제천 전직 변경권
			CI_CLASS_CHANGE_ARA_YAMA_RAJA			= 252752,	/// 아라 2차 전직 명왕 전직 변경권
			//}}
			CI_CLASS_CHANGE_ELESIS_GRAND_MASTER		= 272171,	/// 엘리시스 2차 전직 그랜드 마스터 전직 변경권
			CI_CLASS_CHANGE_ELESIS_BLAZING_HEART	= 272172,	/// 엘리시스 2차 전직 블레이징 하트 전직 변경권

			CI_CLASS_CHANGE_PSYCHIC_TRACER			= 999992,	/// 애드 1차 전직 사이킥 트레이서 전직 변경권
			CI_CLASS_CHANGE_LUNATIC_PSYKER			= 999993,	/// 애드 2차 전직 루나틱 사이커 전직 변경권

			//{{ 2012. 02. 22	박세훈	펫 이름 변경권
			CI_PET_NAME_CHANGE						= 248390,
			//}}

			//{{ 2012. 02. 22	박세훈	길드 이름 변경권
			CI_GUILD_NAME_CHANGE					= 248400,
			//}}

			//{{ 2012. 04. 26	박세훈	펫 장난감
			CI_PET_TOY								= 500580,
			//}}
			//{{ 2013. 2. 28	박세훈	 빙고 이벤트
			//}}


			CI_EXPAND_SKILL_PAGE					= 127273,	// 스킬 페이지 확장권

			CI_EXPAND_SKILL_SLOT_ADD				= 272738,	// 스킬 슬롯 B확장 엘리시스 전용 판매 캐시템

//----------------------------------------------------------------------------------//
			CI_EVENT_GUILD_SKILL_RESET_ITEM			= 80001182,	//(중국) 이벤트용 아리엘의 망각의 피리 // 2012.06.05 lygan_조성욱
			CI_SKILL_5_POINT_30						= 60005091,	// 예제임 미션던전때 쓰던 아이템
			CI_SKILL_10_POINT_60					= 60005092, // 예제임 미션던전때 쓰던 아이템

			// 인벤토리 카테고리별 확장
			CI_EXPAND_INVENTORY_EQUIP				= 200751, // 장비
			CI_EXPAND_INVENTORY_ACCESSORY			= 200752, // 악세사리
			CI_EXPAND_INVENTORY_QUICK_SLOT			= 200753, // 소비
			CI_EXPAND_INVENTORY_MATERIAL			= 200754, // 재료
			CI_EXPAND_INVENTORY_QUEST				= 200755, // 퀘스트
			CI_EXPAND_INVENTORY_SPECIAL				= 200756, // 특수

			//{{ 지헌 - 은행 개편 퀘스트, 캐쉬 작업
			CI_CASH_SHARING_BACK_OPEN				= 65000701,



			CI_EVENT_SKILL_POINT_5_USE_INVEN_15_DAY	= 90000747, // 미국 이벤트 그노시스

			CI_ICE_BURNER							= 207500,
			CI_OFFICER_EVE_ICE_BURNER				= 120003200,
			CI_OFFICER_CHUNG_ICE_BURNER				= 90001070,
			CI_MAGIC_NECKLACE						= 75000600,
			CI_ICE_BURNER_LURIAL					= 210000226,

//----------------------------------------------------------------------------------//
// 해외팀 전용 이벤트 아이템

			CI_WARP_VIP_ITEM						= 215660,	// 새로운 아이템으로 변경해야 할까?



			EI_RURIEL_RESET_A_SKILL_ITEM			= 67006453, // 루리엘의 선택 스킬 초기화 메달
			EI_RURIEL_RESET_SKILL_ITEM				= 67006454, // 루리엘의 스킬 초기화 메달
			EI_RURIEL_RESET_A_SKILL_EVENT_ITEM		= 67006451, // 루리엘의 망각의 알약

			CI_RURIEL_EVENT_SEAL_ITEM_NORMAL		= 83000290,	 //루리엘의 뽀루의 발 도장 (하얀색)
			CI_RURIEL_EVENT_SEAL_ITEM_RARE			= 83000291,	 //루리엘의 뽀루의 발 도장 (노란색)
			CI_RURIEL_EVENT_SEAL_ITEM_ELITE			= 83000292,	 //루리엘의 뽀루의 발 도장 (보라색)
			CI_RURIEL_EVENT_SEAL_ITEM_UNIQUE		= 83000293,	 //루리엘의 뽀루의 발 도장 (황금색)

			CI_ARIEL_EVENT_SEAL_ITEM_NORMAL			= 82214370,	 //아리엘의 뽀루의 발 도장 (하얀색)
			CI_ARIEL_EVENT_SEAL_ITEM_RARE			= 82214380,	 //아리엘의 뽀루의 발 도장 (노란색)
			CI_ARIEL_EVENT_SEAL_ITEM_ELITE			= 82214390,	 //아리엘의 뽀루의 발 도장 (보라색)
			CI_ARIEL_EVENT_SEAL_ITEM_UNIQUE			= 82214400,	 //아리엘의 뽀루의 발 도장 (황금색)

//----------------------------------------------------------------------------------//
		};
		//}}

		//{{ 2009. 9. 7  최육사		밀봉 카운트
		enum SEAL_ITEM_INFO
		{
            SEAL_ITEM_DECREASE_COUNT = 1,
		};
		//}}

		//{{ 2009. 2. 23  최육사	이벤트 아이템
		enum EVENT_ITEM
		{
			EI_GOLD_MEDAL				= 130002, // 대회채널용 황금메달
			EI_XMAS_EVENT_ITEM			= 131354, // 크리스마스 트리 장식
			EI_CAMILLAS_COIN			= 130715, // 카밀라의 코인
			EI_CAMILLAS_COIN_2010		= 132701, // 카밀라의 코인 ( 2010년 영웅대전 )
			EI_PRELIMINARY_TICKET		= 130843, // 예선 대회 참여권
			EI_OFFLINE_TICKET			= 135304, // 온라인 토너먼트 참가 신청권 130844, // 오프라인 대회 참여권
			EI_TREE_SUMMON_ITEM			= 131857, // 자연의 정수 ( 나무수호신 소환 )

			//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
			//{{ 2010. 12. 8	최육사	이벤트용 그노시스 축복
			EI_SKILL_POINT_30_USE_INVEN	= 133131, // 이벤트용 그노시스 축복
			EI_SKILL_POINT_130_1DAY_USE_INVEN = 67006450,	// 이벤트용 그노시스 축복
			EI_SKILL_POINT_30_1DAY_USE_INVEN		= 67006500, // 이벤트용 그노시스 축복 30sp 1일
			EI_SKILL_POINT_60_1DAY_USE_INVEN		= 67006501, // 이벤트용 그노시스 축복 60sp 1일
			EI_SKILL_POINT_30_USE_INVEN_JP			= 67000382,	// 이벤트용 그노시스 축복(2일)	133131
			EI_SKILL_POINT_10_30DAY_USE_INVEN_JP	= 67005710, // 이벤트용 그노시스 축복 10sp 30일
			EI_SKILL_POINT_10_15DAY_USE_INVEN_JP	= 70007020, // 이벤트용 그노시스 축복 10sp 15일
			EI_SKILL_POINT_30_7DAY_USE_INVEN	= 60003758, // 이벤트용 그노시스 축복 30SP 7일
			EI_SKILL_POINT_60_7DAY_USE_INVEN	= 60003759, // 이벤트용 그노시스 축복 60SP 7일
			EI_SKILL_POINT_60_15DAY_USE_INVEN	= 60004280, // 이벤트용 그노시스 축복 60SP 15일
			EI_SKILL_POINT_60_30DAY_USE_INVEN	= 60005011, // 이벤트용 그노시스 축복 60SP 30일
			EI_SKILL_POINT_60_7DAY_USE_INVEN_2	= 60008007, // 이벤트용 그노시스 축복 60SP 7일
			EI_SKILL_POINT_30_7DAY_USE_INVEN_2	= 70007030, // 이벤트용 그노시스 축복 30SP 7일
			//}}

			//{{ 2010. 12. 20	최육사	특정 시각 큐브 보상
			EI_XMAS_RANDOM_CUBE			= 133223, // 크리스마스 이벤트 큐브 ( 산타 양말 )
			//}}

			//{{ 2011. 07. 04    김민성    네이버 프로모션
			EI_SUMMER_RANDOM_CUBE		= 160227,	 // 한 여름 대박 큐브
			EI_WINTER_RANDOM_CUBE		= 160326,	 // 한 겨울 대박 큐브
			EI_WINTER_BIG_RANDOM_CUBE	= 160374,	 // 한 겨울 왕대박 큐브
			EI_SUMMER_TEN_ENCHANT_CHARM = 130152,	 // 10 강 부적
			EI_SUMMER_RANDOM_NDS		= 160230, // 닌텐도 지급 번호
			EI_SUMMER_NEXON_CASH		= 160229, // 넥슨 캐쉬 10000
			EI_SUMMER_NEXON_CASH_50000	= 135105, // 넥슨 캐쉬 50000
			//{{ 2011. 12.13    김민성	접속 이벤트 기능 추가 (반복 지급)
			EI_RANDOM_CUBE_GOLD			= 135115, // 봉인된 얼음 조각상(Gold)
			//}}
			//}}

			//{{ 2011. 01. 06  김민성  스킬슬롯체인지 체크(인벤토리-기간제) 기능 구현
			EI_EXPAND_SKILL_SLOT_EVENT	= 200880, // 이벤트 스킬 슬롯B (기간제)
			//}}
			//{{ 2011. 08. 26	최육사	캐릭터 레벨업 아이템
			EI_CHAR_LEVEL_UP_ITEM		= 160267, // 현자의 주문서
			//}}
			//{{ 2011. 12. 05	최육사	축복의 메달 큐브 체크
			EI_BLESSING_MEDAL_CUBE		= 132698, // 축복의 메달(1일권) 큐브
			//}}
			//{{ 2011. 12. 06	최육사	특정 서버군 이벤트 시스템
			EI_COBO_PC_BANG_COUPON		= 135093, // 코보 PC방 쿠폰
			//}}


			//{{ 2012. 07. 09	김민성       현자의 마법석
//#ifdef SERV_DUNGEON_CLEAR_PAYMENT_STONE_EVENT
			EI_WISE_MAN_STONE_ITEM		= 135184, // 현자의 마법석
//#endif SERV_DUNGEON_CLEAR_PAYMENT_STONE_EVENT
			//}}
			//{{ 2012. 07. 09	김민성       이벤트 큐브 오픈 가능 시간
			EI_EVENT_LOTTO_CUBE				= 160533, // 대박 큐브
			EI_EVENT_GOLD_CUBE				= 160534, // 황금 큐브
			EI_EVENT_CASH_CUBE				= 160535, // 넥슨 캐시 큐브

			EI_NEXON_CASH_100				= 160536, // 넥슨 캐시 100
			EI_NEXON_CASH_1000				= 160537, // 넥슨 캐시 1000
			EI_NEXON_CASH_5000				= 160538, // 넥슨 캐시 5000
			EI_NEXON_CASH_10000				= 160539, // 넥슨 캐시 10000
			//}}
			//{{ 2012. 12. 19  교환(한정 수량) - 김민성
			EI_ARA_EVENT_NEXON_CASH_1000		= 135318, // 천년 여우 넥슨 캐시 1000원 지급 쿠폰
			//}}

			//{{ 2013. 03. 27	 그노시스의 축복 5point 7일 추가 - 김민성
			EI_SKILL_POINT_30_DAY_7_USE_INVEN		= 268562,	// 인벤토리에서 클릭해서 사용하는 그노시스 축복 30포인트 7일제
			//}

			CI_COBO_SKILL_RESET_ITEM		= 135433, // 스킬 한개 초기화

//----------------------------------------------------------------------------------//
// 해외팀 전용 이벤트 아이템
		// 추후 추가되는 경우 이곳에 넣어주시기 바랍니다.

			EL_CHAR_LEVEL_UP_ITEM2		= 60004276,	// 시공간의 주문서

		// 복귀 유저를 위한 특별 패키지 판매(7.4)
		EI_SERVICE_COUPON_TYPE_A				= 60007075,	// 맞춤 서비수 쿠폰 Type A
		EI_SERVICE_COUPON_TYPE_A_PACKAGE		= 60007078,	// 맞춤 서비수 쿠폰 Type A 패키지

		EI_SERVICE_COUPON_TYPE_B				= 60007076,	// 맞춤 서비수 쿠폰 Type B
		EI_SERVICE_COUPON_TYPE_B_PACKAGE		= 60007079,	// 맞춤 서비수 쿠폰 Type B 패키지

		EI_SERVICE_COUPON_TYPE_C				= 60007077,	// 맞춤 서비수 쿠폰 Type C
		EI_SERVICE_COUPON_TYPE_C_PACKAGE		= 60007080,	// 맞춤 서비수 쿠폰 Type C 패키지

		EI_ARA_SPECIAL_COUPON					= 60007091,	// 아라 스페셜 쿠폰
		EI_ARA_SPECIAL_ATTRACTION_PACKAGE		= 60007113,	// 아라 스페셜 가열기 패키지
		EI_ARA_SPECIAL_AVATAR1_PACKAGE			= 60007114,	// 아라 스페셜 아바타 패키지1
		EI_ARA_SPECIAL_AVATAR2_PACKAGE			= 60007115,	// 아라 스페셜 아바타 패키지2
		EI_ARA_SPECIAL_PET_PACKAGE				= 60007116,	// 아라 스페셜 펫 패키지

		EI_ARIEL_SPECIAL_PRESENT_CUBE_LV12		= 60007062, // 아리엘 특별 선물 큐브 Lv.12
		EI_ARIEL_SPECIAL_PRESENT_CUBE_LV18		= 60007064, // 아리엘 특별 선물 큐브 Lv.18
		EI_ARIEL_SPECIAL_PRESENT_CUBE_LV20		= 60007066, // 아리엘 특별 선물 큐브 Lv.20
		EI_SPECIAL_PRESENT_COPPER_KEY			= 60007069, // 스페셜 선물 열쇠(동)
		EI_SPECIAL_PRESENT_SILVER_KEY			= 60007070, // 스페셜 선물 열쇠(은)
		EI_SPECIAL_PRESENT_GOLD_KEY				= 60007071, // 스페셜 선물 열쇠(금)

		EI_SPECIAL_HEAVENLY_BOSS_RARE			= 70007203, // 천상의 지배자(레어)

		EI_READY_TO_SOSUN_EVENT_ITEM			= 60007092,	// 아라 1차 전직 이름맞추기 실행권
		EI_READY_TO_SOSUN_EVENT_WIN_ITEM		= 60007112,	// 아라 1차 전직 이름맞추기 당첨 보상
		EI_EVENT_PROPOSE_ITEM					= 60007791,	// 가상 결혼 신청서
		EI_EVENT_DIVORCE_ITEM					= 60007792, // 가상 결혼 계약서


		EI_INFINITY_EL_ESSENCE_ITEM_EVENT		= 60008064,		// 엘의 정수 공급기
		EI_SKILL_POINT_30_14DAY_USE_INVEN	= 141000896, //춘절 그노시스
		EI_LURIEL_GNOSIS_30_15DAY				= 210000270, // 루리엘의 그노시스의 축복 (30SP/15일)
		EI_LURIEL_GNOSIS_30_30DAY				= 210000271, // 루리엘의 그노시스의 축복 (30SP/30일)
		EI_LURIEL_GNOSIS_30_60DAY				= 210000272, // 루리엘의 그노시스의 축복 (30SP/60일)
		EI_LURIEL_GNOSIS_30_7DAY				= 210000273, // 루리엘의 그노시스의 축복 (30SP/7일)
		EI_LURIEL_GNOSIS_60_15DAY				= 210000280, // 루리엘의 그노시스의 축복 (60SP/15일)
		EI_LURIEL_GNOSIS_60_30DAY				= 210000281, // 루리엘의 그노시스의 축복 (60SP/30일)
		EI_LURIEL_GNOSIS_60_60DAY				= 210000282, // 루리엘의 그노시스의 축복 (60SP/60일)
		EI_LURIEL_GNOSIS_60_7DAY				= 210000283, // 루리엘의 그노시스의 축복 (60SP/7일)

//----------------------------------------------------------------------------------//
		};
		//}}

		//{{ 2012. 03. 15	최육사	배틀필드 시스템
		enum REWARD_ITEM
		{
            RI_AUTO_PARTY_BONUS			= 135289, // 영웅의 주화
		};
		//}}

		enum DEFENCE_DUNGEON_REWARD
		{
			DDR_DRAGON_PIECE			= 180034, // 비룡의 비늘 조각
		};

		//{{ 2009. 12. 15  최육사	ED아이템
		enum ED_ITEM
		{
            EDI_BRONZE_ED				= 127000, // 브론즈ED
			EDI_SILVER_ED				= 127010, // 실버ED
			EDI_GOLD_ED					= 127020, // 골드ED
			EDI_GOLD_BAR				= 127041, // 금괴
		};
		//}}

		//{{ 2009. 3. 9  최육사		마을이동아이템
		enum USE_IN_INVENTORY_ITEM
		{
			SI_WARP_RUBEN				= 109995, // 마을별 특급 이동 티켓 (루벤 마을)
			SI_WARP_ELDER				= 109996, // 마을별 특급 이동 티켓 (엘더 마을)
			SI_WARP_BESMA				= 109997, // 마을별 특급 이동 티켓 (베스마 마을)
			SI_WARP_ALTERA				= 109998, // 마을별 특급 이동 티켓 (알테라 마을)
			SI_WARP_PEITA				= 110832, // 페이타 특급 이동 티켓 (페이타 던전게이트)
			SI_WARP_VELDER				= 110847, // 벨더 근교 특급 이동 티켓 (벨더근교 마을)

			//{{ 2009. 8. 7  최육사		은행
			SI_BANK_MEMBERSHIP_UPGRADE	= 99380, // 루리엘의 신용 보증서
			//}}

			//{{ 2009. 9. 22  최육사	길드
			SI_GUILD_FOUNDATION			= 110845, // 길드 창단 허가서
			//}}

			//{{ 2009. 9. 23  최육사	1차전직캐쉬템
			SI_FIRST_CHANGEJOB_CUBE		= 109999, // 1차 전직 프로모션 큐브
			//}}
			//{{ 2009. 11. 10  최육사	2차전직
			SI_SECOND_CHANGE_JOB_CUBE	= 110850, // 2차 전직 프로모션 큐브
			//}}

			//{{ 2009. 10. 9  최육사	길드
			SI_GUILD_MAX_MEMBER_UPGRADE	= 110846, // 길드 인원 확장권
			//}}

			SI_RING_OF_PVP_REBIRTH		= 202410, // 리스폰 반지
			SI_BLESSING_MEDAL			= 200720, // 축복의 메달
			SI_EXP_15_PERCENT_MEDAL		= 129940, // 경험치 15% 메달
			SI_EXP_100_PERCENT_MEDAL	= 130529, // 경험치 100% 메달

			//{{ 2011. 10. 19    김민성    200% 경험치 추가 메달(이벤트 아이템)
			SI_EXP_200_PERCENT_MEDAL	= 160299, // 성장의 메달  경험치 200% 메달
			//}}

			SI_USE_FREE_BY_FIELD		= 215660, // 자동결제 마을이동 아이템.

			//{{ 2012. 10. 31	박세훈	코보 익스프레스 티켓 추가
			SI_USE_COBO_EXPRESS_TICKET	= 112323, // ED 소모 마을이동 아이템.
			//}}

			//{{ 2009. 11. 27  최육사	길드스킬
			SI_GUILD_CASH_SKILL_POINT	= 110859, // 그노시스의 대축복
			//}}
			//{{ 2010. 04. 09  최육사	기술의 노트
			SI_SKILL_NOTE_ITEM			= 99600,  // 기술의 노트 아이템
			//}}
			//{{ 2011. 05. 11	최육사	대리상인
			SI_PSHOP_AGENCY_1_DAY		= 160060, // 대리 판매 보증서(1일권)
			SI_PSHOP_AGENCY_2_DAYS		= 160061, // 대리 판매 보증서(2일권)
			SI_PSHOP_AGENCY_3_DAYS		= 160062, // 대리 판매 보증서(3일권)
			SI_PSHOP_AGENCY_5_DAYS		= 160063, // 대리 판매 보증서(5일권)
			SI_PSHOP_AGENCY_7_DAYS		= 160064, // 대리 판매 보증서(7일권)
			SI_PSHOP_AGENCY_10_DAYS		= 160065, // 대리 판매 보증서(10일권)
			SI_PSHOP_AGENCY_15_DAYS		= 160066, // 대리 판매 보증서(15일권)
			SI_PSHOP_AGENCY_30_DAYS		= 160067, // 대리 판매 보증서(30일권)

            SI_PSHOP_AGENCY_7_DAYS_ED   = 160904, // 대리 판매 보증서 (7일권, ED)
            SI_PSHOP_AGENCY_15_DAYS_ED  = 160905,//             대리 판매 보증서
            SI_PSHOP_AGENCY_30_DAYS_ED  = 160906,//             대리 판매 보증서
			//}}

			//{{ 2011. 05. 27    김민성    휴면 복귀 유저 보상
			SI_COME_BACK_REWARD_SKILL_30_POINT_15_DAY		= 127180, // 그노시스의 축복(15일권)
			SI_COME_BACK_REWARD_SKILL_30_POINT_30_DAY		= 127190, // 그노시스의 축복(30일권)
			SI_COME_BACK_REWARD_SKILL_30_POINT_60_DAY		= 127200, // 그노시스의 축복(60일권)
//}}


			//{{ 2012. 03. 25	박세훈	어둠의 문 개편
			SI_THE_GATE_OF_DARKNESS_ARCANUM_NUT		= 78520,	// 누트의 가호
			SI_THE_GATE_OF_DARKNESS_ARCANUM_PTAH	= 78530,	// 프타의 가호
			SI_THE_GATE_OF_DARKNESS_ARCANUM_RACHEL	= 78540,	// 라헬의 가호
			SI_THE_GATE_OF_DARKNESS_ARCANUM_ZADKIEL	= 78550,	// 자키엘의 가호

			// 영약
			SI_THE_GATE_OF_DARKNESS_ELIXIR_SERAPHIM	= 78560,	// 세라핌의의 축복
			SI_THE_GATE_OF_DARKNESS_ELIXIR_GEB		= 78570,	// 게브의 축복
			SI_THE_GATE_OF_DARKNESS_ELIXIR_AMON		= 78580,	// 아몬의 축복
			SI_THE_GATE_OF_DARKNESS_ELIXIR_CRONOS	= 78590,	// 크로노스의 축복

			//{{ 2013. 04. 12	박세훈	어둠의 문 개편
			// 영약
			SI_THE_GATE_OF_DARKNESS_ELIXIR_GIANT_POTION				= 78894,	// 어둠의 문 영약 : 거인화 물약
			SI_THE_GATE_OF_DARKNESS_ELIXIR_BLAZING_BOMB				= 78895,	// 어둠의 문 영약 : 이글이글 폭약
			SI_THE_GATE_OF_DARKNESS_ELIXIR_SPIRIT_OF_CHASER			= 78896,	// 어둠의 문 영약 : 추적자의 혼
			SI_THE_GATE_OF_DARKNESS_ELIXIR_CRADLE_OF_LITTLE_FAIRY	= 78897,	// 어둠의 문 영약 : 아기 요정 요람
			SI_THE_GATE_OF_DARKNESS_ELIXIR_ICE_BALL_OF_DENIPH		= 78898,	// 어둠의 문 영약 : 데니프의 얼음 구슬
			SI_THE_GATE_OF_DARKNESS_ELIXIR_FEATHER_OF_VENTUS		= 78899,	// 어둠의 문 영약 : 벤투스의 날개깃
			SI_THE_GATE_OF_DARKNESS_ELIXIR_FLAME_RING_OF_ROSSO		= 78900,	// 어둠의 문 영약 : 로쏘의 화염고리
			SI_THE_GATE_OF_DARKNESS_ELIXIR_BIG_HAND_POTION			= 78901,	// 어둠의 문 영약 : 거대손 물약
			//}}
			//}}

			//{{ 2012. 12. 24	박세훈	펫 오토 루팅 기능 추가
			SI_PET_AUTO_LOOTING_ITEM				= 500720,	// 펫 오토 루팅
			//}}

			//{{ 2013. 04. 01	 인연 시스템 - 김민성
			SI_COUPLE_RING_ITEM						= 269710,	// 커플링
			SI_WEDDING_RING_ITEM					= 269711,	// 결혼반지
			SI_WEDDING_HALL_A_ITEM					= 269712,	// 결혼식장 캐시 - A type
			SI_WEDDING_HALL_B_ITEM					= 269713,	// 결혼식장 캐시 - B type
			SI_WEDDING_INVITE_TICKET_ITEM			= 269714,	// 결혼 초대권
			SI_WEDDING_RESERVATION_ITEM				= 269715,	// 결혼식 예약권
			SI_WEDDING_INVITATION_ITEM				= 269716,	// 청첩장
			SI_WEDDING_DRESS_ITEM					= 160765,	// 의상 큐브
			SI_LOVE_WORD_CHANGE_ITEM_ITEM			= 269718,	// 애칭 변경권
			SI_COUPLE_ANNIVERSARY_DAY_REWARD_ITEM	= 160762,	// 커플 기념일 큐브
			SI_WEDDING_ANNIVERSARY_DAY_REWARD_ITEM	= 160763,	// 결혼 기념일 큐브
			//}

			SI_FINALITY_SKILL_ITEM					= 271310,	// 궁극의 비전서

			SI_FINALITY_SKILL_BOOK					= 271311,	// 궁극의 비전서

//----------------------------------------------------------------------------------//
// 해외팀 전용 이벤트 아이템

			SI_HALLOWEEN_TRANSFORM_POSION_ITEM		= 90002300, // 펫 변신 물약


			SI_EXP_10_PERCENT_MEDAL					= 60005596,	// 아리엘의 경험치 10% 메달
			SI_EXP_30_PERCENT_MEDAL			= 65000720, // 경험치 30% 메달
			SI_EXP_10_PERCENT_MEDAL365		= 67004384, // 경험치 10% 메달 ( 365 일 )
			SI_EXP_10_PERCENT_MEDAL1		= 67004385, // 경험치 10% 메달 ( 1일 )
			SI_EXP_50_PERCENT_MEDAL_JP		= 60007081, // 경험치 50% 메달
			SI_EXP_30_PERCENT_MEDAL_JP		= 67006203, // 경험치 30% 메달(이벤트용)
			SI_EVENT_GUILD_MAX_MEMBER_UPGRADE		= 80001183, // 중국 전용 이벤트용 길드 인원 확장권 // 2012.06.04 lygan_조성욱
			SI_EVENT_GUILD_CASH_SKILL_POINT			= 80001181, // 중국 전용 이벤트용 아리엘의 그노시스 대축복 15일 5GSP // 2012.06.05 lygan_조성욱
			SI_RING_OF_PVP_REBIRTH_EVENT = 83000001, // 중국 전용 리스폰 반지

			SI_STARTER_EXP_15_PERCENT_MEDAL	= 120000100,// 엘스타터 패키지 경험치 15% 메달
			SI_EXP_50_PERCENT_MEDAL			= 120002740,// 경험치 50% 메달
			SI_EXP_30_PERCENT_MEDAL_1DAY	= 85001942, // 경험치 30% 메달(1일권)
			SI_EXP_10_PERCENT_MEDAL_US		= 85001972, // 경험치 10% 메달

			SI_INVENTORY_SLOT_ADD_ITEM_EQUIP_EVENT			= 141000900,// 인벤 확장 이벤트용
			SI_INVENTORY_SLOT_ADD_ITEM_ACCESSORY_EVENT		= 141000901,
			SI_INVENTORY_SLOT_ADD_ITEM_QUICK_SLOT_EVENT		= 141000902,
			SI_INVENTORY_SLOT_ADD_ITEM_MATERIAL_EVENT		= 141000903,
			SI_INVENTORY_SLOT_ADD_ITEM_QUEST_EVENT			= 141000904,
			SI_INVENTORY_SLOT_ADD_ITEM_SPECIAL_EVENT		= 141000905,

			EI_BUFF_RURIEL_MANA_ENERGIZE_POTION				= 60008058,	// 이벤트 영약 : 루리엘의 마나 활성화 포션
//----------------------------------------------------------------------------------//
		};
		//}}

		//{{ 2011. 08. 12   김민성      헤니르 개편
		enum HENIR_DUNGEON_REWARD
		{
			HDR_TIMESPACE_PIECE			= 61205, // 시공간의 조각
		};
		//}}

		//{{ 2012. 04. 04	최육사	배틀필드 시스템
		enum COOLTIME_ITEM_GROUP
		{
			CIG_NONE				= 0,
			CIG_HP_POTION,
			CIG_FOOD,
			CIG_AP_POTION,
			CIG_MP_POTION,
			CIG_DRINK,
			CIG_THROW_ITEM,
			CIG_MONSTER_CARD,
			CIG_DEFENCE_MONSTER,
			CIG_DEFENCE_MONSTER_SPECIAL,

			CIG_EVENT_APPLE,
			CIG_EVENT_RECOVER_ITEM,
			CIG_SPECIA_FOOD,
			CIG_SPECIAL_RECOVER_ITEM,
			CIG_ANTI_PITION,

			CIG_FULL_RECOVER_ITEM,
			CIG_SHERBET_ITEM,
			CIG_COBO_FULL_RECOVER_ITEM,
			CIG_PILL_ITEM,
			CIG_MP_ELIXIR_ITEM,
			CIG_WEDDING_CAKE_ITEM,
			CIG_WEDDING_DRINK_ITEM,

			CIG_MAX,
		};
		//}}

		enum RIDING_PET_CREATE_ITEM
		{
			RPCI_NONE	= 0,
			RPCI_1		= 1,
			RPCI_2		= 2,
			RPCI_3		= 3,
			RPCI_MAX,
		};

		enum CASH_SKILL_POINT
		{
			CSP_NONE = 0,
			CSP_30_POINT = 30,
			CSP_60_POINT = 60,
			CSP_130_POINT = 130,
		};

		struct SpecialAbility
		{
			DWORD		m_Type;
			int							m_CoolTime;
			int							m_Value1;
			int							m_Value2;
			int							m_Value3;

			SpecialAbility()
			{
				m_Type			= SAT_NONE;
				m_CoolTime		= 0;
				m_Value1		= 0;
				m_Value2		= 0;
				m_Value3		= 0;
			}
		};

		//{{ 2013. 05. 10	최육사	아이템 개편
		struct SStatRelationLevel
		{
			bool		m_bIsCommonItem;
			BYTE		m_byBaseHPRelLV;
			BYTE		m_byAtkPhysicRelLV;
			BYTE		m_byAtkMagicRelLV;
			BYTE		m_byDefPhysicRelLV;
			BYTE		m_byDefMagicRelLV;

			SStatRelationLevel()
			{
				m_bIsCommonItem		= false;
				m_byBaseHPRelLV		= 0;
				m_byAtkPhysicRelLV	= 0;
				m_byAtkMagicRelLV	= 0;
				m_byDefPhysicRelLV	= 0;
				m_byDefMagicRelLV	= 0;
			}
		};
		//}}

		struct ItemTemplet
		{
			USE_TYPE					m_UseType;					// 사용 방식 장착장비인지 아닌지, 장착장비라면 어떤식으로 장착하는지
			USE_CONDITION				m_UseCondition;				// 사용 조건
			int							m_UseLevel;					// 사용 제한 레벨
			CXSLUnit::UNIT_TYPE			m_UnitType;					// 사용 unittype
			CXSLUnit::UNIT_CLASS		m_UnitClass;				// 사용 unit 클래스
			//{{ 2011. 07. 27	최육사	대전 개편
			CXSLUnit::PVP_RANK			m_BuyPvpRankCondition;		// 사용 랭크
			//}}

			int							m_ItemID;					//아이템 ID
			std::wstring				m_Name;						//아이템 이름
			std::wstring				m_Description;				//아이템 설명
			std::wstring				m_DescriptionInShop;		//아이템 구매전 상점에서만 보이는 설명


			//{{ dmlee 2008.11.29
			//std::wstring				m_ModelName;				//하이퍼 모드는 _Hyper 를 붙인다
			std::wstring				m_ModelName[ MAX_MODEL_COUNT_A_ITEM ];	//하이퍼 모드는 _Hyper 를 붙인다
			//}} dmlee 2008.11.29

			std::wstring				m_TextureChangeXETName;		//하이퍼 모드는 _Hyper 를 붙인다
			std::wstring				m_AniXETName;				//하이퍼 모드는 _Hyper 를 붙인다
			std::wstring				m_AniName;					//하이퍼 모드는 _Hyper 를 붙인다
			std::wstring				m_ShopImage;
			std::wstring				m_DropViewer;				//드롭시 사용할 파티클 시스템 이름

			ITEM_TYPE					m_ItemType;
			//{{ 2007. 10. 31  최육사
			ITEM_GRADE					m_ItemGrade;				//아이템 등급
			//}}
			bool						m_bFashion;					//패션 아이템
			bool						m_bVested;					//귀속 아이템(양도불능/판매불능)
			bool						m_bCanEnchant;				//강화가능 아이템인가
			bool						m_bCanUseInventory;			//인벤토리에서 사용가능한 아이템인가
			//{{ 2009. 7. 30  최육사	장착불가
			bool						m_bNoEquip;					//착용할수 없는 장비인가(교환용 장비)
			//}}
			//{{ 2010. 01. 05  최육사	PC방 프리미엄
			bool						m_bIsPcBang;				//PC방 전용 아이템
			//}}
			//{{ 2011. 10. 14	최육사	배틀필드 아이템 레벨 시스템
			int							m_iItemLevel;				// 아이템 레벨 정보
			//}}

			//{{ dmlee 2008.11.29
			//std::wstring				m_AttachFrameName;			//어느 본에 붙는가
			std::wstring				m_AttachFrameName[ MAX_MODEL_COUNT_A_ITEM ];	// 어느 본에 붙는가
			//}} dmlee 2008.11.29
			bool						m_bCanHyperMode;			//하이퍼 모드 가능 아이템인가

			PERIOD_TYPE					m_PeriodType;				//아이템 존재기간
			short						m_Endurance;				//초기내구도
			short						m_EnduranceDamageMin;		//1스테이지플레이당내구도 감소량 최소
			short						m_EnduranceDamageMax;		//1스테이지플레이당내구도 감소량 최대
			//{{ 2007. 11. 16  최육사
			float						m_RepairED;					//내구도 1 수리당 ED
			int							m_RepairVP;					//내구도 1 수리당 VP
			//}}
			int							m_Quantity;					//1묶음수량

			SHOP_PRICE_TYPE				m_PriceType;				//가격타입
			int							m_Price;					//가격
			int							m_PricePvPPoint;			//구매대전포인트


			CXSLUnit::EQIP_POSITION		m_EqipPosition;				//장착 위치
			CXSLStat::Stat				m_Stat;						//아이템 스탯

            std::vector<SpecialAbility>    m_SpecialAbilityList;
			//{{ dmlee 2008.11.14 아이템 고유속성에 소켓옵션 추가
			//{{ 2011. 07. 25    김민성    아이템 옵션ID 데이터 사이즈 증가
			std::vector< int >				m_vecSocketOption;
			//}}
			//}} dmlee 2008.11.14 아이템 고유속성에 소켓옵션 추가

			//{{ 2013. 05. 10	최육사	아이템 개편
			std::vector< int >			m_vecRandomSocketGroupID;
			SStatRelationLevel			m_kStatRelationLevel;
			//}}

			int							m_CoolTime;
			int							m_SetID;

			//{{ 2009. 9. 2  최육사		봉인
			BYTE						m_ucMaxSealCount;			// 최대로 봉인할 수 있는 횟수
			//}}
			//{{ 2009. 11. 17  최육사	속성강화가능갯수
			int							m_iMaxAttribEnchantCount;
			//}}

			//{{ 2012. 06. 28	박세훈	2012 대전 시즌2
			short						m_iAttributeLevel;			// 요구 속성 레벨
			//}}

			//{{ 버프 관리 시스템 - 김민성
			int							m_iBuffFactorID;			// 버프 팩터 id
			//}

			bool						m_bInventoryOnly;			// 은행, 펫인벤 이동제한

			ItemTemplet()
			{
				m_UseType			= UT_NONE;		// 사용 방식 장착장비인지 아닌지, 장착장비라면 어떤식으로 장착하는지
				m_UseCondition		= UC_NONE;	// 사용 조건
				m_UseLevel			= 0;						// 사용 제한 레벨
				m_UnitType			= CXSLUnit::UT_NONE;		// 사용 unittype
				m_UnitClass			= CXSLUnit::UC_NONE;		// 사용 unit 클래스
				//{{ 2011. 07. 27	최육사	대전 개편
				m_BuyPvpRankCondition = CXSLUnit::PVPRANK_NONE; // 사용 랭크
				//}}

				m_ItemID			= -1;						//아이템 ID

				m_ItemType			= IT_NONE;
				//{{ 2007. 10. 31  최육사
				m_ItemGrade			= IG_NONE;					//아이템 등급
				//}}
				m_bFashion			= false;					//패션 아이템
				m_bVested			= false;					//귀속 아이템(양도불능/판매불능)
				m_bCanEnchant		= false;					//강화가능 아이템인가
				m_bCanUseInventory	= false;					//인벤토리에서 사용가능한 아이템인가
				m_bNoEquip			= false;					//장착불가
				//{{ 2010. 01. 05  최육사	PC방 프리미엄
				m_bIsPcBang			= false;					//PC방 전용 아이템
				//}}
				//{{ 2011. 10. 14	최육사	배틀필드 아이템 레벨 시스템
				m_iItemLevel		= 0;						// 아이템 레벨 정보
				//}}

				m_bCanHyperMode		= false;					//하이퍼 모드 가능 아이템인가

				m_PeriodType		= PT_INFINITY;				//아이템 존재기간
				m_Endurance			= -1;						//초기내구도
				m_EnduranceDamageMin	= 0;					//1스테이지플레이당내구도 감소량
				m_EnduranceDamageMax	= 0;					//1스테이지플레이당내구도 감소량
				//{{ 2007. 11. 16  최육사
				m_RepairED			= 0.0f;						//내구도 수리 ED
				m_RepairVP			= 0;						//내구도 수리 VP
				//}}
				m_Quantity			= 0;						//1묶음수량

				m_PriceType			= SPT_NONE;					//가격타입
				m_Price				= 0;						//가격
				m_PricePvPPoint		= 0;						//구매대전포인트

				m_UnitType			= CXSLUnit::UT_NONE;		//장착 유닛
				m_UnitClass			= CXSLUnit::UC_NONE;		//장착 클래스

				m_EqipPosition		= CXSLUnit::EP_NONE;		//장착 위치

				m_CoolTime			= 0;
				m_SetID				= 0;

				//{{ 2009. 9. 2  최육사		봉인
				m_ucMaxSealCount	= 0;						// 최대로 봉인할 수 있는 횟수
				//}}
				//{{ 2009. 11. 17  최육사	속성강화가능갯수
				m_iMaxAttribEnchantCount = 0;
				//}}

				//{{ 2012. 06. 28	박세훈	2012 대전 시즌2
				m_iAttributeLevel	= 0;						// 요구 속성 레벨
				//}}

				//{{ 버프 관리 시스템 - 김민성
				m_iBuffFactorID		= 0;
				//}
				m_bInventoryOnly	= false;
			}

			//{{ 2013. 05. 20	최육사	아이템 개편
			bool IsCommonItem() const		{ return m_kStatRelationLevel.m_bIsCommonItem; }
			//}}
		};

		struct ItemData
		{
			UidType				m_ItemUID;				//UID
			int					m_ItemID;

			PERIOD_TYPE         m_PeriodType;
			short				m_Endurance;			//내구도
			int					m_Quantity;				//수량

			int					m_EnchantAttribute;
			int					m_EnchantLevel;

			ItemData()
			{
				m_ItemUID				= 0;				//UID
				m_ItemID				= 0;

				m_PeriodType            = PT_INFINITY;
				m_Endurance				= -1;				//내구도
				m_Quantity				= 0;				//수량

				m_EnchantAttribute		= 0;
				m_EnchantLevel			= 0;
			}
			ItemData& operator=( const KItemInfo& data )
			{
//				m_ItemUID				= data.m_iItemUID;					//UID
				m_ItemID				= data.m_iItemID;

				m_PeriodType            = (CXSLItem::PERIOD_TYPE)data.m_cUsageType;
				//m_Endurance				= data.m_iEndurance;				//내구도
				m_Quantity				= data.m_iQuantity;					//수량

				//m_EnchantAttribute		= data.m_iEType;
				//m_EnchantLevel			= data.m_iELevel;

				return *this;
			}

			void MakePacket( KItemInfo& itemInfo )
			{
//				itemInfo.m_iItemUID				= m_ItemUID;				//UID
				itemInfo.m_iItemID				= m_ItemID;

				itemInfo.m_cUsageType           = m_PeriodType;
				//itemInfo.m_iEndurance			= m_Endurance;			//내구도
				itemInfo.m_iQuantity			= m_Quantity;			//수량

				//itemInfo.m_iEType				= m_EnchantAttribute;
				//itemInfo.m_iELevel				= m_EnchantLevel;
			}
		};

	public:
		CXSLItem( ItemData* pItemData );
		~CXSLItem(void);

		ItemData*		GetItemData()			{ return m_pItemData; }
		ItemTemplet*	GetItemTemplet()		{ return m_pItemTemplet; }

		UidType			GetUID()				{ return m_pItemData->m_ItemUID; }
		void			SetEqip( bool bEqip )	{ m_bEqip = bEqip; }
		bool			GetEqip()				{ return m_bEqip; }

	public:
		//{{ 2009. 7. 28  최육사	GM무기
		static	bool	IsGMweapon( const int iItemID );
		//}}
		//{{ 2009. 9. 22  최육사	전직캐쉬
		static	char	GetCashItemChangeUnitClass( const int iItemID );
		static  int		GetCashItemByUnitClass( const char cUnitClass );
		//}}
		//{{ 2009. 12. 14  최육사	서버코드정리
		static	bool	IsJobChangeCashItem( const int iItemID ) { return ( GetCashItemChangeUnitClass( iItemID ) != CXSLUnit::UC_NONE ); }
		//}}
		//{{ 2010. 04. 26  최육사	PC방 전용 캐쉬템
		static	bool	IsPcBangOnlyCashItem( const int iItemID );
		//}}

		static	bool	IsKeepItemShowItem( const int iItemID );
		static	bool	IsKeepItemShowItem2( const int iItemID );
		static	bool	IsKeepItemShowItem3( const int iItemID );
		static	bool	IsKeepItemShowItem4( const int iItemID );
		static	bool	IsKeepItemShowItem5( const int iItemID );
		static	bool	IsKeepItemShowItem6( const int iItemID );

#ifndef DELETE_EXCEPTION_WISHLIST_CHANGEJOBITEM //캐시샾 찜하기에서 전직 아이템에 대해 예외처리 해제.
		//{{ 2010. 03. 30  김정협 전직아이템인지 확인하는 함수
		static	bool	IsChangeJobItem( const int iItemID );
#endif //DELETE_EXCEPTION_WISHLIST_CHANGEJOBITEM

		static	bool	IsBankExpandItem( const int iItemID );
		//}}

		static	bool	IsItemToMakeSkillPageExpanded( const int iItemID_ );


		//{{ 2011. 05. 11	최육사	대리상인
		static	int		GetPShopAgencyDays( IN const int iItemID );
        static  int     GetPShopAgencyType( IN const int iItemID_ );

		//}}
		//{{ 2011. 06. 01	최육사	강화된 아이템 보상 큐브
		//{{ 2013. 05. 15	최육사	아이템 개편
		static	bool	UpdateEnchantedItemCubeReward( IN const int iCubeItemID, IN OUT KItemInfo& kInsertItemInfo );
			//}}
		//}}


		//{{ 2011. 11. 21  김민성	전직 변경 아이템
		static	int		GetClassChangeCashItem( const int iItemID );
		static	bool	IsClassChangeCashItem( const int iItemID ) { return ( GetClassChangeCashItem( iItemID ) != CXSLUnit::UC_NONE ); }
		static int		GetClassChangeCashItemIDByUnitClass( const int iUnitClass_ );
		//}}

		//{{ 2012. 02. 22	박세훈	펫 이름 변경권
		static	bool	IsPetNameChangeCashItem( const int iItemID ) { return iItemID == CXSLItem::CI_PET_NAME_CHANGE; }
		//}}
		static bool	IsPossibleCart( const int iItemID_ );


	private:
		ItemData*		m_pItemData;
		ItemTemplet*	m_pItemTemplet;

		bool			m_bEqip;
};
