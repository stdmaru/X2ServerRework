#pragma once
#include "SubThread.h"
#include "ODBC/Odbc.h"
#include "KncSend.h"
#include "BaseServer.h"  // KBaseServer::GetKObj() in the template bodies below

//{{ 2009. 8. 13  최육사	sp profiler
#include "ODBC/DBUtil.h"
//}}

#include "Event.h"
#include "CommonPacket.h"
#include "ServerPacket.h"

class KChannelAccountDBThread : public KSubThread
{
    DeclareDBThread( KChannelAccountDBThread );
    DeclareSPProfiler;
    DeclPfID;
public:

    KChannelAccountDBThread();  // 구현부는 없음. 사용하지 않는다. (사용시 linking error 유발)
    virtual ~KChannelAccountDBThread(void);

protected:
    virtual void ProcessEvent( const KEventPtr& spEvent ); // Port: `inline` dropped, it is defined in the .cpp

protected:
    // packet send function
    template < class T > void SendToServer( unsigned short usEventID, const T& data );
    template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    void SendToServer( unsigned short usEventID );
    void SendToUser( UidType nTo, unsigned short usEventID );

    //void WriteServerInfoToDB( IN KServerInfo kPacket_, OUT KServerList& kPacket );

    // event handler
	DECL_ON_FUNC( DBE_CH_USER_GENERAL_LOGIN_REQ );
	DECL_ON_FUNC( DBE_CH_USER_KOGOTP_LOGIN_REQ );

	//DECL_ON_FUNC( DBE_GASH_USER_LOGIN_REQ );

	//{{ 2011. 01. 13 김민성	동접 모니터링 툴
	DECL_ON_FUNC( DBE_GET_CONCURRENT_USER_LOGIN_REQ );
	//}}
	//{{ 2011. 09. 14  김민성	해킹 프로세스 목록 전달 - 게임 로딩 이전 단계
	//}}
	//{{ 2011. 09. 26  김민성	머신 ID 체크 - 게임 로딩 이전 단계
	DECL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_REQ );
	//}}
	//{{ 2011.11.02     김민성    서버에서 SN 발급 및 유효성 검사
	//}}


};

template < class T >
void KChannelAccountDBThread::SendToServer( unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_CHANNEL_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KChannelAccountDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
    KncSend( GetPfID(), 0, PI_CHANNEL_USER, nTo, NULL, usEventID, data );
}
