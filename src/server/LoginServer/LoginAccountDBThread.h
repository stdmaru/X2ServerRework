#pragma once
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

class KLoginAccountDBThread : public KSubThread
{
    DeclareDBThread( KLoginAccountDBThread );
	//{{ 2009. 8. 17  최육사	sp profiler
	DeclareSPProfiler;
	//}}
    DeclPfID;
public:

    KLoginAccountDBThread();   // 구현부는 없음. 사용하지 않는다. (사용시 linking error 유발)
    virtual ~KLoginAccountDBThread(void);

protected:
    virtual inline void ProcessEvent( const KEventPtr& spEvent );

protected:
    // packet send function
	template < class T > void SendToServer( unsigned short usEventID, const T& data );
	template < class T > void SendToUser( UidType nTo, unsigned short usEventID, const T& data );
    template < class T > void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data );
	void SendToUser( UidType nTo, unsigned short usEventID );
    void SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID );

   _DECL_ON_FUNC( DBE_NEW_USER_JOIN_REQ, KEGS_NEW_USER_JOIN_REQ );
    DECL_ON_FUNC( ELG_NEXON_USER_AUTHENTICATE_REQ );
	//{{ 2009. 5. 26  최육사	채널이동
	DECL_ON_FUNC( ELG_CHANNEL_CHANGE_NEXON_USER_AUTHENTICATE_REQ );
	//}}
	DECL_ON_FUNC( DBE_UPDATE_IS_LOGIN_NOT );
	//{{ 2010. 06. 22  최육사	계정블럭기능
	DECL_ON_FUNC( DBE_CHECK_ACCOUNT_BLOCK_LIST_REQ );
	//}}
	//{{ 2010. 10. 05	최육사	센터서버 해킹체크
   _DECL_ON_FUNC( DBE_REG_REJECTED_USER_NOT, KELG_REG_REJECTED_USER_NOT );
	//}}
   //{{ 2011. 03. 02	최육사	개인 거래 차단
   _DECL_ON_FUNC( DBE_REG_TRADE_BLOCK_USER_NOT, KELG_REG_REJECTED_USER_NOT );
   //}}
   //{{  2012. 1. 30	김민성		한방핵 방어(해킹 프로세스 검사)
   //}}
   //{{ 2013. 3. 18	박세훈	 로컬 랭킹 시스템
   //}}

};

template < class T >
void KLoginAccountDBThread::SendToServer( unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), 0, PI_LOGIN_SERVER, KBaseServer::GetKObj()->GetUID(), NULL, usEventID, data );
}

template < class T >
void KLoginAccountDBThread::SendToUser( UidType nTo, unsigned short usEventID, const T& data )
{
	KncSend( GetPfID(), 0, PI_LOGIN_USER, nTo, NULL, usEventID, data );
}

template < class T >
void KLoginAccountDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID, const T& data )
{
    UidType anTrace[2] = { nGS, -1 };
    KncSend( GetPfID(), 0, PI_GS_USER, nTo, anTrace, usEventID, data );
}
