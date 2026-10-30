#pragma once

#include <KNCSingleton.h>
#include "ThreadManager.h"
#include <KncLua.h>
#include <lua_tinker.h>
using namespace lua_tinker;


#define CASE_SCRIPT_REFRESH_SWAP_INSTANCE( flag, className ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	className::SwapInstance( g_pLua ); \
	START_LOG( cout, L#className L" 스크립트 갱신을 완료하였습니다!" ); \
	Si##className()->DumpToLogFile(); \
	break; \


//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신
//#ifdef SERV_REALTIME_SCRIPT


class KGSScriptRefreshManager : public KThreadManager
{
	DeclareSingleton( KGSScriptRefreshManager );
	DeclPfID;

public:
	KGSScriptRefreshManager();
	virtual ~KGSScriptRefreshManager();

	void RegToLua( lua_State* pLuaState );

	// 스크립트 갱신 요청 lua함수
    void ItemManager_LUA();
	void RandomItemManager_LUA();
	void ManufactureItemManager_LUA();
	void RewardTable_LUA();
	void QuestManager_LUA();
	void TitleManager_LUA();
	void DungeonManager_LUA();
	void DropTable_LUA();
	void GameEventScriptManager_LUA();
	void MapData_LUA();
	void GuildManager_LUA();
	void GameSysVal_LUA();
	void ResolveItemManager_LUA();
	void EnchantItemManager_LUA();
	void SocketItem_LUA();
	void AttribEnchantItem_LUA();
	void RecommendUserTable_LUA();
	void ExpTable_LUA();

	//{{ 2010. 9. 11	최육사	펫 시스템
	void PetManager_LUA();
	//}}
	void AttribNpcTable_LUA();
	//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	void CnHackingCheckManager_LUA();
	//}}
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	void AbuserLogManager_LUA();
	//{{ 2011. 02. 23	최육사	던전 랭크 개편
	void ResultProcess_LUA();
	//}}

	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	void WorldMissionManager_LUA();
	//}} 2011. 04. 13  김민성  글로벌 서버 추가
	//{{ 2011. 07. 23	최육사	대전 개편
	void PvpMatchManager_LUA();
	void PvpMatchResultTable_LUA();
	//}}
	//{{ 2011. 08. 24	최육사	시공 개편
	void HenirResultTable_LUA();
	//}}
	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	void ContentManager_LUA();
	//}}

	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	void CompanyIPTable_LUA();
	//}}
	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	void ClassChangeTable_LUA();
	//}}

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	void StringFilter_LUA();
	//}}

	//{{ 2012. 04. 25	박세훈	어둠의 문 개편
	void WorldMissionManager_IncreaseCrystalCount_LUA( int iIncreaseCrystalNum );
	void WorldMissionManager_ResetIncreaseCrystalCount_LUA( void );
	//}}

	//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
	//}}
	//{{ 2011. 12. 26	최육사	배틀필드 시스템
	void BattleFieldManager_LUA();
	void AutoPartyScriptManager_LUA();
	void PartyManager_LUA();
	//}}
	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2013. 1. 28	박세훈	빙고 이벤트
	//}}
	//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	void BadAttitudeTable_LUA();
	//}


	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	void LogManager_LUA();
	//}}
	//{{ 2013. 04. 15	최육사	어둠의 문 개편
	void DefenceDungeonManager_LUA();
	void BuffManager_LUA();
	//}}

	void RidingPetManager_LUA( void );

	void SkillTree_LUA( void );

	void	FieldBossData_LUA( void );

    void EDInventoryExpand_LUA();
	// derived from KThreadManager
	virtual void Init();
	virtual KThread* CreateThread();

	lua_State* GetRefreshLuaState() { return m_pRefreshLuaState; }

protected:
	void RefreshOrderToCnServer( IN int iOrderType );
	void RefreshOrderToCnServerBroadCast( IN int iOrderType );
	void RefreshOrderToCnServerEachServerGroup( IN int iOrderType );
	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	void RefreshOrderToGBServer( IN int iOrderType );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가


protected:
	lua_State* m_pRefreshLuaState;
};

DefSingletonInline( KGSScriptRefreshManager );


//#endif SERV_REALTIME_SCRIPT
//}}
