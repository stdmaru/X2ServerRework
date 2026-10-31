#include "CnRMEventProcess.h"
#include "CnRoomManager.h"

#include "NetError.h"
#include "X2Data/XSLItemManager.h"
#include "X2Data/XSLQuestManager.h"
#include "KDropTable.h"
//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
#include "GameEvent/GameEventScriptManager.h"
//}}
#include "GameSysVal/GameSysVal.h"
#include "KAttribNpcTable.h"
//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	#include "CnHackingCheckManager.h"
//}}
#include "AbuserLog/KAbuserLogManager.h"
#include "Socket/NetCommon.h"
//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	#include "GameEvent/GameEventManager.h"
//}}
//{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현
	#include "DungeonEventManager.h"
//}}
//{{ 2011. 02. 23	최육사	던전 랭크 개편
	#include "ResultProcess.h"
//}}
//{{ 2011. 07. 23	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
	#include "PvpMatchResultTable.h"
//}}
//{{ 2011. 08. 24	최육사	시공 개편
	#include "HenirResultTable.h"
//}}
//{{ 2011. 12. 26	최육사	배틀필드 시스템
	#include "X2Data/XSLBattleFieldManager.h"
	#include "X2Data/XSLPartyManager.h"
//}}
//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	#include "../Common/AutoParty/AutoPartyScriptManager.h"
//}}
//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	#include "BadAttitudeTable.h"
//}

// Port: KSMS (ProcessCommunicationManager, Windows named pipes to an external tool) removed, as in the other servers

//{{ 2013. 04. 15	최육사	어둠의 문 개편
	#include "X2Data/XSLDefenceDungeonManager.h"
	#include "X2Data/XSLBuffManager.h"
//}}

	#include "X2Data/XSLFieldBossData.h"

#define CASE_SCRIPT_REFRESH_SWAP_INSTANCE( flag, className ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	className::SwapInstance( g_pLua ); \
	START_LOG( cout, L ## #className L" 스크립트 갱신을 완료하였습니다!" ); \
	Si##className()->DumpToLogFile(); \
	break; \


ImplPfID( KCnRMEventProcess, PI_CN_ROOM );

#define CLASS_TYPE  KCnRMEventProcess

KCnRMEventProcess::KCnRMEventProcess()
{
}

KCnRMEventProcess::~KCnRMEventProcess()
{
}

//{{ 2010. 8. 29	최육사	이벤트 큐 사이즈 덤프

ImplToStringW( KCnRMEventProcess )
{
	return START_TOSTRING_PARENTW( KPerformer );
}

//}}

void KCnRMEventProcess::ProcessEvent( const KEventPtr& spEvent_ )
{
	DWORD dwElapTime = ::GetTickCount();

	switch( spEvent_->m_usEventID )
	{
   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT );

	CASE( DBE_GET_TIME_DROP_INFO_ACK );
	CASE( DBE_CHECK_TIME_DROP_RESULT_ACK );
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	CASE( DBE_ABUSER_LIST_ACK );
	CASE( DBE_EVENT_UPDATE_ACK );
	//}}
	//{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현
	CASE( DBE_GET_DUNGEON_EVENT_INFO_ACK );
	//}}


	default:
		START_LOG( cerr, L"핸들러가 지정되지 않은 이벤트." )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< END_LOG;
	}

	dwElapTime = ::GetTickCount() - dwElapTime;
	if( dwElapTime > 3000 )
	{
		START_LOG( cwarn, L"이벤트 처리 소요 시간." )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( dwElapTime );
	}
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_ACK, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_ITEM_MANAGER, CXSLItemManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_QUEST_MANAGER, CXSLQuestManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_DUNGEON_MANAGER, CXSLDungeonManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_DROP_TABLE, KDropTable );
	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_GAME_EVENT_SCRIPT_MANAGER, KGameEventScriptManager );
	//}}
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_GAME_SYS_VAL, KGameSysVal );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_ATTRIB_NPC_TABLE, KAttribNpcTable );
	//{{ 2010. 10. 05	최육사	센터서버 해킹체크
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_HACKING_CHECK_MANAGER, KCnHackingCheckManager );
	//}}
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_ABUSER_LOG_MANAGER, KAbuserLogManager );
	//{{ 2011. 02. 23	최육사	던전 랭크 개편
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_RESULT_PROCESS, KResultProcess );
	//}}
	//{{ 2011. 07. 23	최육사	대전 개편
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_PVP_MATCH_MANAGER, KPvpMatchManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_PVP_MATCH_RESULT_TABLE, KPvpMatchResultTable );
	//}}
	//{{ 2011. 08. 24	최육사	시공 개편
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_HENIR_RESULT_TABLE, KHenirResultTable );
	//}}
	//{{ 2011. 12. 26	최육사	배틀필드 시스템
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_BATTLE_FIELD_MANAGER, CXSLBattleFieldManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_PARTY_MANAGER, CXSLPartyManager );
	//}}
//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2012. 11. 30	박세훈	던전별 자동 파티 매칭 성공 인원수 설정
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_AUTO_PARTY_SCRIPT_MANAGER, KAutoPartyScriptManager );
	//}}
	//{{ 2013. 01. 09 던전 강퇴 시스템 - 김민성
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_BAD_ATTITUDE_TABLE, KBadAttitudeTable );
	//}
	//{{ 2013. 04. 15	최육사	어둠의 문 개편
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_DEFENCE_DUNGEON_MANAGER, CXSLDefenceDungeonManager );
	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_BUFF_MANAGER, CXSLBuffManager );
	//}}

	CASE_SCRIPT_REFRESH_SWAP_INSTANCE( OT_CN_FIELD_BOSS_DATA, CXSLFieldBossData );

	default:
		{
			START_LOG( cerr, L"이쪽으로 오면 안되는 타입인데?" )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		break;
	}

	// 드롭 테이블이 갱신 되는 경우
	if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_CN_DROP_TABLE )
	{
		// DB로부터 특정 시간 드롭 정보 받아오기!
		KncSend( PI_NULL, 0, PI_LOG_DB, 0, NULL, DBE_GET_TIME_DROP_INFO_REQ, char() );
	}
	//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
	else if( kPacket_.m_iOrderType == KESR_SCRIPT_REFRESH_ORDER_NOT::OT_CN_GAME_EVENT_SCRIPT_MANAGER )
	{


		SiKGameEventManager()->RefreshEventScript();
		//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
		SiKGameEventScriptManager()->Init();
		//}}
	}
	//}}

}

IMPL_ON_FUNC( DBE_GET_TIME_DROP_INFO_ACK )
{
	// 시간 드롭 정보 업데이트
	SiKDropTable()->SetTimeDropInfo( kPacket_.m_vecTimeDropInfo, NetCommon::GetLocalIPW() );
}

IMPL_ON_FUNC( DBE_CHECK_TIME_DROP_RESULT_ACK )
{
	if( kPacket_.m_iOK == NetError::NET_OK )
	{
		START_LOG( cerr, L"특정 시각 드롭 이벤트 성공했는데 여기로 왔네? 일어나면 안되는 에러!" )
			<< BUILD_LOG( NetError::GetErrStr( kPacket_.m_iOK ) )
			<< BUILD_LOG( kPacket_.m_iGSUID )
			<< BUILD_LOG( kPacket_.m_iUserUID )
			<< BUILD_LOG( kPacket_.m_iUnitUID )
			<< BUILD_LOG( kPacket_.m_wstrNickName )
			<< BUILD_LOG( kPacket_.m_iTimeDropID )
			<< BUILD_LOG( kPacket_.m_iRewardItemID )
			<< BUILD_LOG( kPacket_.m_wstrRewardItemName )
			<< BUILD_LOG( kPacket_.m_iDropItemUID )
			<< END_LOG;
		return;
	}

	// 롤백 하자!
	SiKDropTable()->RollbackTimeDrop( kPacket_.m_iTimeDropID );
}

//{{ 2008. 10. 31  최육사	어뷰저 리스트
//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
IMPL_ON_FUNC( DBE_ABUSER_LIST_ACK )
{
	if( kPacket_.m_mapAbuserList.empty() )
		return;

	START_LOG( cout, L"어뷰저 리스트 갱신!" );

	// 어뷰저 리스트 업데이트
	SiKAbuserLogManager()->UpdateAbuserList( kPacket_.m_mapAbuserList );
}
//}}
//}}

//{{ 2010. 11. 15	최육사	이벤트 스크립트 실시간 패치
IMPL_ON_FUNC( DBE_EVENT_UPDATE_ACK )
{
	if( kPacket_.m_vecEventList.size() > 0 )
	{
		SiKGameEventManager()->SetEvent( kPacket_.m_vecEventList );
	}
}

//}}

//{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현

IMPL_ON_FUNC( DBE_GET_DUNGEON_EVENT_INFO_ACK )
{
    SiKDungeonEventManager()->UpdateTimeDropEventMonsterInfo( kPacket_.m_vecEventInfo );

	START_LOG( cout, L"특정 시각 던전 출현 이벤트 몬스터 세팅!" )
		<< BUILD_LOG( kPacket_.m_vecEventInfo.size() );
}

//}}
