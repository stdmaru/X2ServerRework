#pragma once
#include "SubThread.h"
#include "ODBC/Odbc.h"
//{{ 2009. 8. 13  최육사	sp profiler
#include "ODBC/DBUtil.h"
//}}
#include "KncSend.h"
#include "BaseServer.h"  // KBaseServer::GetKObj() in the template bodies below

#include "Event.h"
#include "CommonPacket.h"
#include "ServerPacket.h"

class KChannelLogDBThread : public KSubThread
{
    DeclareDBThread( KChannelLogDBThread );
	DeclareSPProfiler;
    DeclPfID;
public:

    KChannelLogDBThread();  // 구현부는 없음. 사용하지 않는다. (사용시 linking error 유발)
    virtual ~KChannelLogDBThread(void);

protected:
    virtual inline void ProcessEvent( const KEventPtr& spEvent );

protected:
    // packet send function
    template < class T > void SendToServer( unsigned short usEventID, const T& data );
    template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    void SendToServer( unsigned short usEventID );
    void SendToUser( UidType nTo, unsigned short usEventID );

    //void WriteServerInfoToDB( IN KServerInfo kPacket_, OUT KServerList& kPacket );

    // event handler
   _DECL_ON_FUNC( DBE_SERVER_ON_REQ, KNetAddress );
   //_DECL_ON_FUNC( ECN_VERIFY_SERVER_REQ, KNetAddress );
    DECL_ON_FUNC_NOPARAM( DBE_SERVER_OFF_NOT );
	DECL_ON_FUNC( DBE_CHANNEL_LIST_REQ );

	//{{ 2009. 12. 16  최육사	동접툴
	DECL_ON_FUNC( DBE_GET_CONCURRENT_USER_INFO_REQ );
	//}}
	//{{ 2010. 02. 16  최육사	해킹툴 리스트
	DECL_ON_FUNC( DBE_CHECK_HACKING_TOOL_LIST_REQ );
	//}}

	//{{ 2011. 01. 13 김민성	동접 모니터링 툴
	DECL_ON_FUNC_NOPARAM( DBE_GET_GAME_SERVER_LIST_REQ );
	//}}
	//{{ 2012. 04. 24	최육사	UDP릴레이 퍼포먼스 체크
	//}}

	//{{ 2012. 07. 04	최육사		대기열 시스템
	DECL_ON_FUNC_NOPARAM( DBE_GET_TOTAL_CCU_FOR_WAIT_QUEUE_REQ );
	//}}
	//{{ 2013. 02. 13	최육사	Tick퍼포먼스 DB로그 기록
	DECL_ON_FUNC( DBE_TICK_PERFORMANCE_LOG_NOT );
	//}}
	//{{ 2013. 02. 21	최육사	로그 시스템 개선
	DECL_ON_FUNC( DBE_LOG_STATISTICS_INFO_NOT );
	//}}

	DECL_ON_FUNC_NOPARAM( DBE_SERVERGROUP_LIST_REQ );

	DECL_ON_FUNC( DBE_INSERT_LOGIN_RESULT_INFO_REQ );

};

template < class T >
void KChannelLogDBThread::SendToServer( unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_CHANNEL_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KChannelLogDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_CHANNEL_USER, nTo, NULL, usEventID, data );
}
