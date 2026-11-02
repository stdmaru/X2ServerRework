#include "GlobalScriptRefreshThread.h"
#include "GlobalScriptRefreshManager.h"

#include "BaseServer.h"

//{{ 2010. 10. 12	최육사	서버 모니터링
//}}

//{{ 2011. 04. 13  김민성  글로벌 서버 추가
#include "WorldMissionManager.h"
//}} 2011. 04. 13  김민성  글로벌 서버 추가
//{{ 2011. 07. 23	최육사	대전 개편
	#include "PvpMatch/PvpMatchManager.h"
//}}
//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	#include "X2Data/XSLQuestManager.h"
//}}

//{{ 2012. 10. 8	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2012. 11. 9	최육사	배틀필드 시스템
	#include "../Common/AutoParty/AutoPartyScriptManager.h"
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

	#include "X2Data/XSLBattleFieldManager.h"
	#include "X2Data/XSLFieldBossData.h"


#define CASE_REFRESH_SCRIPT( flag, classname )	_CASE_REFRESH_SCRIPT( flag, classname, OpenScriptFile )

#define _CASE_REFRESH_SCRIPT( flag, classname, functionname ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	START_LOG( cout, L ## #classname L" Starting script refresh." ); \
	{ \
		KLocker lock( classname::GetTempCS() ); \
		if( SiTemp##classname()->functionname( SiKGlobalScriptRefreshManager()->GetRefreshLuaState() ) == false ) \
		{ \
			START_LOG( cerr, L"Failed to refresh the script." ) \
			<< END_LOG; \
			return; \
		} \
	} \
	break; \


//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신


#define CLASS_TYPE  KGlobalScriptRefreshThread

KGlobalScriptRefreshThread::KGlobalScriptRefreshThread( void )
{
}

KGlobalScriptRefreshThread::~KGlobalScriptRefreshThread( void )
{
	End( 15000 );
}

void KGlobalScriptRefreshThread::ProcessEvent( const KEventPtr& spEvent_ )
{
	DWORD dwElapTime = ::GetTickCount();

	switch( spEvent_->m_usEventID )
	{
   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT );

	default:
		START_LOG( cerr, L"No handler is assigned to the event." )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< END_LOG;
	}

	dwElapTime = ::GetTickCount() - dwElapTime;
	if( dwElapTime > 3000 )
	{
		START_LOG( cwarn, L"Event processing time." )
			<< BUILD_LOG( spEvent_->GetIDStr() )
			<< BUILD_LOG( spEvent_->m_usEventID )
			<< BUILD_LOG( dwElapTime );
	}

	return;
}

template < class T >
void KGlobalScriptRefreshThread::SendToServer( unsigned short usEventID, const T& data )
{
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_GLOBAL_SERVER, NULL, usEventID, data );
	KBaseServer::GetKObj()->QueueingEvent( spEvent );
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	//{{ 2011. 04. 13  김민성  글로벌 서버 추가
	CASE_REFRESH_SCRIPT( OT_GB_WORLD_MISSION_MANAGER, KWorldMissionManager );
	//}} 2011. 04. 13  김민성  글로벌 서버 추가
	//{{ 2011. 07. 23	최육사	대전 개편
	CASE_REFRESH_SCRIPT( OT_GB_PVP_MATCH_MANAGER, KPvpMatchManager );
	//}}
	//{{ 2011. 08. 29	김민성       일일 랜덤 퀘스트
	CASE_REFRESH_SCRIPT( OT_GB_QUEST_MANAGER, CXSLQuestManager );
	//}}

	//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2012. 11. 9	최육사	배틀필드 시스템
	CASE_REFRESH_SCRIPT( OT_GB_AUTO_PARTY_SCRIPT_MANAGER, KAutoPartyScriptManager );
	//}}
	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	CASE_REFRESH_SCRIPT( OT_GB_LOG_MANAGER, KLogManager );
	//}}

	_CASE_REFRESH_SCRIPT( OT_GB_BATTLE_FIELD_MANAGER, CXSLBattleFieldManager, OpenScriptFile_AllBattleFieldScriptLoad );
	CASE_REFRESH_SCRIPT( OT_GB_FIELD_BOSS_DATA, CXSLFieldBossData );

	default:
		{
			START_LOG( cerr, L"Unexpected type reached this code path." )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		return;
	}

	// 로직 스레드에서 교체하도록 처리!
	SendToServer( ESR_ORDER_TO_REFRESH_MANAGER_ACK, kPacket_ );
}


//}}
