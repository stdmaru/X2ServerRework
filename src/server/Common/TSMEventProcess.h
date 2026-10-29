#pragma once

#include "Performer.h"

#include "Event.h"
#include "CommonPacket.h"
#include "SystemPacket.h"
#include "ClientPacket.h"
#include "ServerPacket.h"

#include "KncSend.h"

//{{ 2011. 11. 21  김민성	통계 로그 수치
//}}



class KTSMEventProcess : public KPerformer
{
	DeclPfID;

public:
	KTSMEventProcess();
	virtual ~KTSMEventProcess();

	// override.
	virtual void ProcessEvent( const KEventPtr& spEvent );    // derived from KPerformer

	DECL_ON_FUNC( E_STATISTICS_INFO_NOT );
	DECL_ON_FUNC_NOPARAM( EGS_SEND_STATISTICS_LOCLOG_REQ );
	DECL_ON_FUNC_NOPARAM( EGS_SEND_STATISTICS_DBLOG_REQ );
	DECL_ON_FUNC_NOPARAM( ECN_SEND_STATISTICS_LOCLOG_REQ );
	DECL_ON_FUNC_NOPARAM( ECN_SEND_STATISTICS_DBLOG_REQ );
	DECL_ON_FUNC( E_HENIR_RANKING_LOG_NOT );
	DECL_ON_FUNC( E_LOCAL_LOG_PVP_NOT );
	DECL_ON_FUNC( E_LOCAL_LOG_PVP_ROOM_NOT );
	DECL_ON_FUNC( E_LOCAL_LOG_DUNGEON_ROOM_NOT );
	DECL_ON_FUNC( E_LOCAL_LOG_TITLE_NOT );
	DECL_ON_FUNC( E_LOCAL_LOG_DUNGEON_NOT );
	DECL_ON_FUNC( E_LOCAL_LOG_HENIR_RANKING_NOT );
	//{{ 2010. 9. 14	최육사	펫 시스템
	DECL_ON_FUNC( E_LOCAL_LOG_PET_SUMMON_NOT );
	//}}
	//{{ 2010. 10. 11	최육사	서버간 접속 끊김 로그
	DECL_ON_FUNC( E_LOCAL_LOG_SERVER_DISCONNECT_NOT );
	//}}
	//{{ 2010. 10. 11	최육사	넥슨 빌링 패킷 로그
	DECL_ON_FUNC( E_LOCAL_LOG_BILLING_PACKET_NOT );
	//}}
	//{{ 2010. 11. 17	최육사	오토핵 검증 기능
	DECL_ON_FUNC( E_LOCAL_LOG_ABUSER_MORNITORING_NOT );
	//}}

#if X2_DEV_TOOLS
	DECL_ON_FUNC( E_LOCAL_LOG_UNDEFINED_QUEST_TEMPLET_NOT );
#endif // SERV_LOG_UNDEFINED_QUEST_TEMPLET

protected:

	//{{ 2011. 11. 21  김민성	통계 로그 수치
	//}}

};
