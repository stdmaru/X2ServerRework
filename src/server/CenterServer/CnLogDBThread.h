#pragma once
#include "BaseServer.h" // KBaseServer::GetKObj() in the templates below
#include "SubThread.h"
#include "ODBC/Odbc.h"
//{{ 2009. 8. 17  최육사	sp profiler
#include "ODBC/DBUtil.h"
//}}
#include "KncSend.h"

#include "Event.h"
#include "CommonPacket.h"
#include "ClientPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

class KCnLogDBThread : public KSubThread
{
    DeclareDBThread( KCnLogDBThread );
	//{{ 2009. 8. 17  최육사	sp profiler
	DeclareSPProfiler;
	//}}
    DeclPfID;
public:

    KCnLogDBThread();  // 구현부는 없음. 사용하지 않는다. (사용시 linking error 유발)
    virtual ~KCnLogDBThread(void);

protected:
    virtual inline void ProcessEvent( const KEventPtr& spEvent );

protected:
    // packet send function
    template < class T > void SendToServer( unsigned short usEventID, const T& data );
    template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    template < class T > void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data );
    void SendToServer( unsigned short usEventID );
    void SendToUser( UidType nTo, unsigned short usEventID );
    void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID );

    //void WriteServerInfoToDB( IN KServerInfo kPacket_, OUT KServerList& kPacket );

    // event handler
   _DECL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress );
   //{{ 2011. 07. 19	최육사	서버간 접속 코드 리팩토링
   //#ifdef SERV_SERVER_CONNECT_CODE_REFACTORING
   //{{ 2010. 05. 13  최육사	대전 던전 서버군 통합
   _DECL_ON_FUNC( DBE_VERIFY_SERVER_CONNECT_REQ, KECN_VERIFY_SERVER_CONNECT_REQ );
   //}}
   //#endif SERV_SERVER_CONNECT_CODE_REFACTORING
   //}}
   //_DECL_ON_FUNC( DBE_UPDATE_SERVER_INFO_REQ, KServerInfo );
    DECL_ON_FUNC_NOPARAM( DBE_SERVER_OFF_NOT );
    DECL_ON_FUNC( ELOG_CHAT_LOG_NOT );
	DECL_ON_FUNC( ELOG_INSERT_TRADE_LOG_NOT );
	DECL_ON_FUNC( ELOG_INSERT_PERSONAL_SHOP_LOG_NOT );
   _DECL_ON_FUNC( ELOG_STAT_RES_STONE, KStatisticsData );
	DECL_ON_FUNC_NOPARAM( DBE_EVENT_UPDATE_REQ );

	//{{ 2008. 3. 4  최육사  어뷰저 로그
	DECL_ON_FUNC( ELOG_INSERT_ABUSER_LOG_NOT );
	//}}
	//{{ 2008. 10. 31  최육사	어뷰저 리스트
	//{{ 2012. 02. 03	박세훈	이벤트 관련정보 처리방법 변경 ( Script -> DB )
	//}}
	DECL_ON_FUNC( DBE_ABUSER_LIST_REQ );

	//}}
	//{{ 2009. 5. 23  최육사	시간드롭
	DECL_ON_FUNC_NOPARAM( DBE_GET_TIME_DROP_INFO_REQ );
	DECL_ON_FUNC( DBE_CHECK_TIME_DROP_RESULT_REQ );
	//}}
	//{{ 2009. 7. 27  최육사	헤니르 클리어 로그
	DECL_ON_FUNC( ELOG_HENIR_DUNGEON_CLEAR_NOT );
	//}}
	//{{ 2010. 11. 8	최육사	던전 로그 DB
   _DECL_ON_FUNC( DBE_DB_LOG_DUNGEON_NOT, KE_LOCAL_LOG_DUNGEON_NOT );
	//}}
   //{{ 2010. 11. 20	최육사	특정 시각 몬스터 출현
	DECL_ON_FUNC_NOPARAM( DBE_GET_DUNGEON_EVENT_INFO_REQ );
	DECL_ON_FUNC( DBE_UPDATE_EVENT_MONSTER_KILL_INFO_NOT );
   //}}
	//{{ 2010. 12. 8	최육사	적정 레벨 장비 통계
	//}}
	//{{ 2011. 01. 26	최육사	던전 클리어 타임 핵유저 체크
	DECL_ON_FUNC( DBE_REG_DUNGEON_CLEAR_TIME_HACK_USER_NOT );
	//}}
	//{{ 2011. 03. 17	김민성	던전 플레이 로그 요청사항
	//}}
	//{{ 2011. 06. 13	최육사	스테이지 로그
	//}}
	//{{ 2011. 07. 27	최육사	대전 개편
	DECL_ON_FUNC( DBE_PVP_MATCH_RESULT_LOG_NOT );
	//}}
	//{{ 2012. 10. 16	최육사		P2P & Relay 정보 수집
	DECL_ON_FUNC( ELOG_COLLECTION_OF_RELAY_AND_P2P_INFO_NOT );
	//}}
	//{{ 2012. 09. 05	박세훈	랜선렉 로그 추가
	//}}
	//{{ 2012. 10. 18	최육사	배틀필드 시스템
	DECL_ON_FUNC( ELOG_BATTLE_FIELD_LEAVE_LOG_NOT );
	//}}
	//{{ 2012. 12. 12	최육사	결과창 이탈 유저 로그
	DECL_ON_FUNC( DBE_DUNGEON_RESULT_LEAVE_LOG_NOT );
	//}}
	//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
	DECL_ON_FUNC( DBE_TICK_PERFORMANCE_LOG_NOT );
	//}}
	//{{ 2013. 02. 21	최육사	로그 시스템 개선
	DECL_ON_FUNC( DBE_LOG_STATISTICS_INFO_NOT );
	//}}

	DECL_ON_FUNC_NOPARAM( DBE_CHECK_EVENT_UPDATE_REQ );

	//{{ 2013. 2. 26	박세훈	랜선렉 방지 코드2
	DECL_ON_FUNC( DBE_UDP_CHECK_KICK_USER_NOT );
	//}}
	//{{ 2013. 04. 01	 인연 시스템 - 김민성
	DECL_ON_FUNC( DBE_WRITE_RELATIONSHIP_INFO_LOG_REQ );
	//}


    DECL_ON_FUNC( ELOG_HENIR_HACKING_LOG_NOT );

    _DECL_ON_FUNC( ELOG_GEN_FIELD_MIDDLE_BOSS_STAT_NOT, KFieldBossGenKillStat );
    _DECL_ON_FUNC( ELOG_KILL_FIELD_MIDDLE_BOSS_STAT_NOT, KFieldBossGenKillStat );

	DECL_ON_FUNC( DBE_BOSS_FIELD_LOG_NOT );
};

template < class T >
void KCnLogDBThread::SendToServer( unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_CN_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KCnLogDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_CN_USER, nTo, NULL, usEventID, data );
}

template < class T >
void KCnLogDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { nGS, -1 };
    KncSend( GetPfID(), 0, PI_GS_USER, nTo, anTrace, usEventID, data );
}
