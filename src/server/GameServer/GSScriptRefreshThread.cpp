#include "GSScriptRefreshThread.h"
#include "GSScriptRefreshManager.h"

#include "KncSend.h"

#include "X2Data/XSLItemManager.h"
#include "X2Data/XSLRandomItemManager.h"
#include "X2Data/XSLManufactureItemManager.h"
#include "RewardTable.h"
#include "X2Data/XSLQuestManager.h"
#include "X2Data/XSLTitleManager.h"
#include "X2Data/XSLDungeonManager.h"
#include "X2Data/XSLMapData.h"
#include "X2Data/XSLGuildManager.h"
#include "GameSysVal/GameSysVal.h"
#include "X2Data/XSLResolveItemManager.h"
#include "X2Data/XSLEnchantItemManager.h"
#include "X2Data/XSLSocketItem.h"
#include "X2Data/XSLAttribEnchantItem.h"
#include "RecommendUserTable.h"
#include "ExpTable.h"
//{{ 2010. 9. 11	최육사	펫 시스템
	#include "X2Data/XSLPetManager.h"
//}}
//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
#include "AbuserLog/KAbuserLogManager.h"
//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	#include "GameEvent/GameEventScriptManager.h"
//}}
//{{ 2011. 07. 23	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
//}}
//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	#include "GSContentManager.h"
//}}

//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	#include "CompanyIPTable.h"
//}}
//{{ 2011. 11. 21  김민성	전직 변경 아이템
	#include "ClassChangeTable.h"
//}}

//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	#include "X2Data/XSLStringFilter.h"
//}}

//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
//}}

//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2012. 10. 19	최육사	배틀필드 시스템
	#include "X2Data/XSLBattleFieldManager.h"
	#include "X2Data/XSLPartyManager.h"
//}}
//{{ 2013. 1. 28	박세훈	빙고 이벤트
//}}


//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}
//{{ 2013. 04. 15	최육사	어둠의 문 개편
	#include "X2Data/XSLDefenceDungeonManager.h"
	#include "X2Data/XSLBuffManager.h"
//}}

	#include "X2Data/XSLRidingPetManager.h"

	#include "X2Data/XSLSkillTree.h"

	#include "X2Data/XSLFieldBossData.h"

    #include "X2Data/XSLEDInventoryExpand.h"

#define CASE_REFRESH_SCRIPT( flag, classname )	_CASE_REFRESH_SCRIPT( flag, classname, OpenScriptFile )

#define _CASE_REFRESH_SCRIPT( flag, classname, functionname ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	START_LOG( cout, L ## #classname L" Starting script refresh!" ); \
	{ \
		KLocker lock( classname::GetTempCS() ); \
		if( SiTemp##classname()->functionname( SiKGSScriptRefreshManager()->GetRefreshLuaState() ) == false ) \
		{ \
			START_LOG( cerr, L"Failed to refresh the script!" ) \
			<< END_LOG; \
			return; \
		} \
	} \
	break; \


//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신


#define CLASS_TYPE  KGSScriptRefreshThread

KGSScriptRefreshThread::KGSScriptRefreshThread( void )
{
}

KGSScriptRefreshThread::~KGSScriptRefreshThread( void )
{
	End( 15000 );
}

void KGSScriptRefreshThread::ProcessEvent( const KEventPtr& spEvent_ )
{
	DWORD dwElapTime = ::GetTickCount();

	switch( spEvent_->m_usEventID )
	{
   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT );

	default:
		START_LOG( cerr, L"Event has no handler assigned." )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< END_LOG;
	}

	dwElapTime = ::GetTickCount() - dwElapTime;
	if( dwElapTime > 3000 )
	{
		START_LOG( cwarn, L"Event processing duration." )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( dwElapTime );
	}

	return;
}

template < class T >
void KGSScriptRefreshThread::SendToServer( unsigned short usEventID, const T& data )
{
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_GS_SERVER, NULL, usEventID, data );
	KBaseServer::GetKObj()->QueueingEvent( spEvent );
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
	CASE_REFRESH_SCRIPT( OT_GS_ITEM_MANAGER, CXSLItemManager );
	CASE_REFRESH_SCRIPT( OT_GS_RANDOM_ITEM_MANAGER, CXSLRandomItemManager );
	CASE_REFRESH_SCRIPT( OT_GS_MANUFACTURE_ITEM_MANAGER, CXSLManufactureItemManager );
	CASE_REFRESH_SCRIPT( OT_GS_REWARD_TABLE, KRewardTable );
	CASE_REFRESH_SCRIPT( OT_GS_QUEST_MANAGER, CXSLQuestManager );
	CASE_REFRESH_SCRIPT( OT_GS_TITLE_MANAGER, CXSLTitleManager );
   _CASE_REFRESH_SCRIPT( OT_GS_DUNGEON_MANAGER, CXSLDungeonManager, OpenScriptFile_AllDungeonScriptLoad );
	CASE_REFRESH_SCRIPT( OT_GS_MAP_DATA, CXSLMapData );
	CASE_REFRESH_SCRIPT( OT_GS_GUILD_MANAGER, CXSLGuildManager );
	CASE_REFRESH_SCRIPT( OT_GS_GAME_SYS_VAL, KGameSysVal );
	CASE_REFRESH_SCRIPT( OT_GS_RESOLVE_ITEM_MANAGER, CXSLResolveItemManager );
	CASE_REFRESH_SCRIPT( OT_GS_ENCHANT_ITEM_MANAGER, CXSLEnchantItemManager );
	CASE_REFRESH_SCRIPT( OT_GS_SOCKET_ITEM, CXSLSocketItem );
	CASE_REFRESH_SCRIPT( OT_GS_ATTRIB_ENCHANT_ITEM, CXSLAttribEnchantItem );
	CASE_REFRESH_SCRIPT( OT_GS_RECOMMEND_USER_TABLE, KRecommendUserTable );
	CASE_REFRESH_SCRIPT( OT_GS_EXP_TABLE, KExpTable );
	//{{ 2010. 9. 11	최육사	펫 시스템
	CASE_REFRESH_SCRIPT( OT_GS_PET_MANAGER, CXSLPetManager );
	//}}
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	CASE_REFRESH_SCRIPT( OT_GS_ABUSER_LOG_MANAGER, KAbuserLogManager );
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	CASE_REFRESH_SCRIPT( OT_GS_GAME_EVENT_SCRIPT_MANAGER, KGameEventScriptManager );
	//}}
	//{{ 2011. 07. 23	최육사	대전 개편
	CASE_REFRESH_SCRIPT( OT_GS_PVP_MATCH_MANAGER, KPvpMatchManager );
	//}}
	//{{ 2011. 10. 12	최육사	컨텐츠 관리자
	CASE_REFRESH_SCRIPT( OT_GS_CONTENT_MANAGER, KGSContentManager );
	//}}
	//{{ 2011.10.14     김민성    운영자 기능은 특정 IP 에서만 사용 가능
	CASE_REFRESH_SCRIPT( OT_GS_COMPANY_IP_TABLE, KCompanyIPTable );
	//}}
	//{{ 2011. 11. 21  김민성	전직 변경 아이템
	CASE_REFRESH_SCRIPT( OT_GS_CLASS_CHANGE_TABLE, KClassChangeTable );
	//}}

	//{{ 2012. 01. 27	박세훈	String Filter 실시간 스크립트 갱신
	CASE_REFRESH_SCRIPT( OT_GS_STRING_FILTER, CXSLStringFilter );
	//}}

	//{{ 2012. 06. 06	박세훈	매일매일 선물 상자
	//}}

	//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2012. 10. 19	최육사	배틀필드 시스템
   _CASE_REFRESH_SCRIPT( OT_GS_BATTLE_FIELD_MANAGER, CXSLBattleFieldManager, OpenScriptFile_AllBattleFieldScriptLoad );
	CASE_REFRESH_SCRIPT( OT_GS_PARTY_MANAGER, CXSLPartyManager );
	//}}
	//{{ 2013. 1. 28	박세훈	빙고 이벤트
	//}}


	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	CASE_REFRESH_SCRIPT( OT_GS_LOG_MANAGER, KLogManager );
	//}}
	//{{ 2013. 04. 15	최육사	어둠의 문 개편
	CASE_REFRESH_SCRIPT( OT_GS_DEFENCE_DUNGEON_MANAGER, CXSLDefenceDungeonManager );
	CASE_REFRESH_SCRIPT( OT_GS_BUFF_MANAGER, CXSLBuffManager );
	//}}

	CASE_REFRESH_SCRIPT( OT_GS_RIDING_PET_MANAGER, CXSLRidingPetManager );

	CASE_REFRESH_SCRIPT( OT_GS_SKILL_TREE, CXSLSkillTree );

	CASE_REFRESH_SCRIPT( OT_GS_FIELD_BOSS_DATA, CXSLFieldBossData );

    CASE_REFRESH_SCRIPT( OT_GS_ED_INVENTORY_EXPAND, CXSLEDInventoryExpand );
	default:
		{
			START_LOG( cerr, L"This type should not reach this point." )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		return;
	}

	// 로직 스레드에서 교체하도록 처리!
	SendToServer( ESR_ORDER_TO_REFRESH_MANAGER_ACK, kPacket_ );
}

//}}
