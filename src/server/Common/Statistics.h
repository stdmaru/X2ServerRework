#pragma once

#include "x2/platform/timer.h"
#include <ToString.h>
#include "KncSmartPtr.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"
#include "DisconnectReason.h"


#define KStatisticsPtr KStatistics*
#undef KStatisticsPtr
SmartPointer( KStatistics );

class KStatistics : public KDisconnectReason
{
    DeclToStringW;
    DeclDump;

public:
#   undef _ENUM
#   define _ENUM( id ) id,
    enum
    {
#   include "StatisticsID_def.h"
    };

	// 아이템 통계
	enum StatInventory
	{
		SI_ITEM_TRADE = 0,		// 개인거래
		SI_ITEM_PERSONAL_SHOP,	// 개인상점
		SI_ITEM_TRADE_BOARD,	// 거래게시판
		SI_ITEM_BUY,			// 아이템 구매
		SI_ITEM_SELL,			// 아이템 판매
		SI_ITEM_USE,			// 아이템 사용
		SI_ITEM_MAKE,			// 제조
		SI_ITEM_FAILED,			// 제조 실패
		SI_ITEM_DROP,			// 아이템 드롭
		SI_ITEM_QUEST,			// 퀘스트 습득템
		SI_ITEM_EQUIP,			// 아이템 장착
		SI_ITEM_RANDOM,			// 랜덤 큐브 사용
		SI_ITEM_RESOLVE,		// 아이템 분해 시도 횟수
		SI_ITEM_R_RESULT,		// 아이템 분해 결과물
		SI_ITEM_MALL,			// 캐쉬 아이템
		SI_ITEM_POST,			// 우체국 편지 첨부 아이템
		SI_ITEM_IDENTIFY,		// 속성석 감정
		SI_ITEM_EXCHANGE_S,		// 아이템 교환 [원래것]
		SI_ITEM_EXCHANGE_R,		// 아이템 교환 [바뀐것]
		//{{ 2010. 05. 20  최육사	아이템 버리기 통계
		SI_ITEM_DISCARD,		// 아이템 버리기
		//}}
		//{{ 2010. 9. 14	최육사	펫 시스템
//#ifdef SERV_PET_SYSTEM
		SI_ITEM_PET_FEED,		// 펫 먹이
//#endif SERV_PET_SYSTEM
		//}}
		//{{ 2011. 04. 26	최육사	대리 상인
//#ifdef SERV_PSHOP_AGENCY
		SI_ITEM_REG_PSHOP,		// 대리상인 Reg Item
		SI_ITEM_BUY_PSHOP_AGENCY, // 대리상인 아이템 구매
		SI_ITEM_PICK_UP_PSHOP,	// 대리상인 PickUp
//#endif SERV_PSHOP_AGENCY
		//}}
		//{{ 2011. 06. 22    김민성    이탈 방지 모델 - NEXON 세션 시스템 수정
//#ifdef SERV_NEXON_SESSION_PROTOCOL
		SI_ITEM_RETRANINING_REWARD,		// 이탈 방지 보상 아이템 지급
//#endif SERV_NEXON_SESSION_PROTOCOL
		//}}
//#ifdef SERV_FINALITY_SKILL_SYSTEM	// 적용날짜: 2013-08-01
		SI_ITEM_EXTRACT_S,		// 아이템 추출 [원래것]
		SI_ITEM_EXTRACT_R,		// 아이템 추출 [바뀐것]
		SI_ITEM_USE_FINALITY_SKILL,		// 궁극기 사용
//#endif // SERV_FINALITY_SKILL_SYSTEM

		SI_ITEM_MAX,
	};

	// 대전 일일 통계
	enum StatLocalPvp
	{
		SLP_CREATE_COUNT = 0,
		SLP_UNITCLASS_ELSWORD,
		SLP_UNITCLASS_AISHA,
		SLP_UNITCLASS_RENA,
		SLP_UNITCLASS_RAVEN_FIGHTER,
		SLP_UNITCLASS_EVE_NASOD,
		SLP_UNITCLASS_SWORD_KNIGHT,
		SLP_UNITCLASS_MAGIC_KNIGHT,
		SLP_UNITCLASS_COMBAT_RANGER,
		SLP_UNITCLASS_SNIPING_RANGER,
		SLP_UNITCLASS_HIGH_MAGICIAN,
		SLP_UNITCLASS_DARK_MAGICIAN,
		SLP_UNITCLASS_SOUL_TAKER,
		SLP_UNITCLASS_OVER_TAKER,
		SLP_UNITCLASS_EXOTIC_GEAR,
		SLP_UNITCLASS_ARCHITECTURE,
		SLP_INTRUDER,
		SLP_GIVE_UP,
		SLP_GET_VP,
		SLP_GET_EXP,
		SLP_TEAM_MATCH_MC,
		SLP_TEAM_MATCH_PT,
		SLP_TEAM_DEATH_MC,
		SLP_TEAM_DEATH_PT,
		SLP_SURVIVAL_MC,
		SLP_SURVIVAL_PT,
//#ifdef SERV_ARA_CHANGE_CLASS_SECOND // 김태환
		SLP_UNITCLASS_ARA,
//#endif // SERV_ARA_CHANGE_CLASS_SECOND

		SLP_MAX
	};

	// 대전 맵 통계
	enum ESIColumn_PvpMap
	{
		eSIColPvpMap_PlayCount = 0,

		eSIColPvpMap_Max,
	};

	// FPS 통계
	enum ESIColumn_Fps
	{
		eSIColFps_0_20 = 0,
		eSIColFps_21_30,
		eSIColFps_31_40,
		eSIColFps_41_50,
		eSIColFps_51_60,
		eSIColFps_61_70,
		eSIColFps_71_80,
		eSIColFps_81_90,
		eSIColFps_91_unlimited,

		eSIColFps_Max,
	};

	// 근성도 통계
	enum ESIColumn_Spirit
	{
		eSIColSpirit_Play = 0,
		eSIColSpirit_UseSpirit,
		eSIColSpirit_SpiritZero,

		eSIColSpirit_Max,
	};

	// 일일 강화 통계
	enum ESIColumn_Enchant
	{
		eSIColEnchant_Lv_1_Succ = 0,
		eSIColEnchant_Lv_1_Fail,
		eSIColEnchant_Lv_2_Succ,
		eSIColEnchant_Lv_2_Fail,
		eSIColEnchant_Lv_3_Succ,
		eSIColEnchant_Lv_3_Fail,
		eSIColEnchant_Lv_4_Succ,
		eSIColEnchant_Lv_4_Fail,
		eSIColEnchant_Lv_5_Succ,
		eSIColEnchant_Lv_5_Fail,
		eSIColEnchant_Lv_6_Succ,
		eSIColEnchant_Lv_6_Fail,
		eSIColEnchant_Lv_7_Succ,
		eSIColEnchant_Lv_7_Fail,
		eSIColEnchant_Lv_8_Succ,
		eSIColEnchant_Lv_8_Fail,
		eSIColEnchant_Lv_9_Succ,
		eSIColEnchant_Lv_9_Fail,

		eSIColEnchant_Max,
	};

	// 부활석 통계 [DB]
	enum ESIColumn_DB_ResStone
	{
		eSIColResStone_PlayerCount = 0,
		eSIColResStone_UseCount,

		eSIColResStone_Max,
	};

	// 일일 ED 통계 [DB]
	enum ESIColumn_DB_ED
	{
		eSIColDB_ED_PDungeon				= 0,
		eSIColDB_ED_PTraining				= 1,
		eSIColDB_ED_PQuest					= 2,
		eSIColDB_ED_PItemSell				= 3,
		eSIColDB_ED_PItemTrade				= 4,
		eSIColDB_ED_PPersonalShop			= 5,
		eSIColDB_ED_PPostLetter				= 6,
		eSIColDB_ED_MItemBuy				= 7,
		eSIColDB_ED_MItemTrade				= 8,
		eSIColDB_ED_MItemRepair				= 9,
		eSIColDB_ED_MManufacture			= 10,
		eSIColDB_ED_MDunPenalty				= 11,
		eSIColDB_ED_MPersonalShop			= 12,
		eSIColDB_ED_MOpenPShop				= 13,
		eSIColDB_ED_MEnchant				= 14,
		eSIColDB_ED_MSocket					= 15,
		eSIColDB_ED_MPostLetter				= 16,
		eSIColDB_ED_MSendLetterBasic		= 17,
		eSIColDB_ED_MSendLetterItem			= 18,
		eSIColDB_ED_MAttribEnchant			= 19,
		eSIColDB_ED_MSellTaxPShop			= 20,
		eSIColDB_ED_MBuyTaxPShop			= 21,
		//{{ 2011. 05. 11	최육사	대리상인
//#ifdef SERV_PSHOP_AGENCY
		eSIColDB_ED_MPShopAgency			= 22,
		eSIColDB_ED_MBuyTaxPShopAgency		= 23,
		eSIColDB_ED_PPShopPickUp			= 24,
//#endif SERV_PSHOP_AGENCY
		//}}
		//{{ 2012. 11. 26 큐브 ED 오픈 조건 기능 추가 - 김민성
//#ifdef SERV_CUBE_OPEN_ED_CONDITION
		eSIColDB_ED_POpenCube				= 25,
//#endif SERV_CUBE_OPEN_ED_CONDITION
		//}

//#ifdef SERV_SEPARATE_DUNGEON_FIELD_ED_LOG// 작업날짜: 2013-05-08	// 박세훈
		eSIColDB_ED_PBattleField			= 26,		// 배틀 필드에서 획득한 ED
		eSIColDB_ED_MCoboExpressTicketED	= 27,		// 마을 이동서를 통해 소모한 ED
//#endif // SERV_SEPARATE_DUNGEON_FIELD_ED_LOG
//#ifdef SERV_MULTI_RESOLVE // 해외팀 추가 분해 수수료
		eSIColDB_ED_MItemResolve              = 28,
//#endif SERV_MULTI_RESOLVE

//#ifdef SERV_NEW_ITEM_SYSTEM_2013_05// 작업날짜: 2013-06-17	// 박세훈
		eSIColDB_ED_MItemEvaluate			= 29,		// 아이템 감정으로 소모한 ED
//#endif // SERV_NEW_ITEM_SYSTEM_2013_05

//#ifdef SERV_GOOD_ELSWORD // 작업날짜 2013-11-08 , ED로 인벤토리 확장, ED 로 은행 확장
        eSIColDB_ED_ExpandInventory         = 30, // 인벤토리
        eSIColDB_ED_ExpandBank              = 31, // 은행
//#endif SERV_GOOD_ELSWORD

//#ifdef SERV_SKILL_PAGE_SYSTEM
		eSIColDB_ED_ExpandSkillPage         = 32, // 스킬 페이지
//#endif // SERV_SKILL_PAGE_SYSTEM

		eSIColDB_ED_Max,
	};

	// 퀘스트 통계 [DB]
	enum ESIColumn_DB_Quest
	{
		eSIColQuest_Accept = 0,
		eSIColQuest_GiveUp,
		eSIColQuest_Complete,
		eSIColQuest_AcceptLevel,
		eSIColQuest_GiveUpLevel,
		eSIColQuest_CompleteLevel,

		eSIColQuest_Max,
	};

	// disconnection reasons (ESIColumn_Disconnect_Reason): KDisconnectReason, DisconnectReason.h

	// 종료 상태 통계
	enum ESIColumn_Disconnect_UserFSM
	{
		eSIColDU_S_INIT = 0,
		eSIColDU_S_UID_UPDATED,
		eSIColDU_S_LOGINED,
		eSIColDU_S_SERVER_SELECT,
		eSIColDU_S_UNIT_SELECT,
		eSIColDU_S_VILLAGE_MAP,
		eSIColDU_S_LOCAL_MAP,
		eSIColDU_S_WORLD_MAP,
		eSIColDU_S_CHANNEL,
		eSIColDU_S_ROOM,
		eSIColDU_S_CHECK_SECOND_PW,

		eSIColDU_Num,
	};

	//{{ 2011. 02. 28	최육사	사내 통합 통계
	//}}

	// - - - - - - - - - - - - - - - - - - - - - - - - -

    KStatistics();
    virtual ~KStatistics();

    float GetFlushGap()             { return m_fFlushGap; }
    bool  IsWriteToDB()             { return m_bWriteToDB; }
    static const wchar_t* GetIDStr( int iStatisticsID );

	void Init( int iStatisticsID, float fFlushGap, bool bWriteToDB );
	//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
	void SetFlushTime( IN const int iHour );
	//}}

    void Tick();

    void IncreaseCount( const KStatisticsKey& kKey, int iIndex, int iCount );
    void IncreaseCount( const KStatisticsKey& kKey, const std::vector< __int64 >& vecCount );
    void FlushData();
	void WriteLocalLogData( std::wfstream& fout );
	void ClearLocalLogData();

	void InitLocalLogKeys();

	static int GetDisconnectUserFSM( int iStateID );
	static bool GetUnitClassPvpStatColumn( char cUnitClass, int& iColumn );
	static const wchar_t* GetDissconnectReasonString( int iReason );

protected:
	unsigned short GetEventID();

protected:
    int m_iStatisticsID;
    KStatisticsData m_kStatisticsData;
    x2::Timer m_kUpdateTimer;
    float m_fFlushGap;
    bool m_bWriteToDB;
	//{{ 2011. 04. 13	최육사	아이템 통계 DB기록
	bool m_bTimeFlush;
	CTime m_tFlushTime;
	x2::Timer m_kTimeFlushTimer;
	//}}

	static const wchar_t* ms_szStatistics[];
};
