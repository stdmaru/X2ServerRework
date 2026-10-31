#include "LoginScriptRefreshThread.h"
#include "LoginScriptRefreshManager.h"

#include "BaseServer.h"

// include singleton class header file
#include "X2Data/XSLGuildManager.h"
//{{ 2010. 10. 12	최육사	서버 모니터링
//}}
//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	#include "GameSysVal/GameSysVal.h"
//}}
//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	#include "X2Data/XSLTitleManager.h"
//}}

//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

#include "X2Data/XSLSkillTree.h"

#define CASE_REFRESH_SCRIPT( flag, classname )	_CASE_REFRESH_SCRIPT( flag, classname, OpenScriptFile )

#define _CASE_REFRESH_SCRIPT( flag, classname, functionname ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	START_LOG( cout, L ## #classname L" 스크립트 갱신을 시작합니다!" ); \
	{ \
		KLocker lock( classname::GetTempCS() ); \
		if( SiTemp##classname()->functionname( SiKLoginScriptRefreshManager()->GetRefreshLuaState() ) == false ) \
		{ \
			START_LOG( cerr, L"스크립트 갱신 실패.!" ) \
			<< END_LOG; \
			return; \
		} \
	} \
	break; \


//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신


#define CLASS_TYPE  KLoginScriptRefreshThread

KLoginScriptRefreshThread::KLoginScriptRefreshThread( void )
{
}

KLoginScriptRefreshThread::~KLoginScriptRefreshThread( void )
{
	End( 15000 );
}

void KLoginScriptRefreshThread::ProcessEvent( const KEventPtr& spEvent_ )
{
	DWORD dwElapTime = ::GetTickCount();

	switch( spEvent_->m_usEventID )
	{
   _CASE( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT );

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

	return;
}

template < class T >
void KLoginScriptRefreshThread::SendToServer( unsigned short usEventID, const T& data )
{
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_LOGIN_SERVER, NULL, usEventID, data );
	KBaseServer::GetKObj()->QueueingEvent( spEvent );
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
	CASE_REFRESH_SCRIPT( OT_LG_GUILD_MANAGER, CXSLGuildManager );
	//{{ 2010. 10. 12	최육사	서버 모니터링
	//}}
	//{{ 2011. 08. 17	최육사	머신ID 중복 접속 차단
	CASE_REFRESH_SCRIPT( OT_LG_GAME_SYS_VAL, KGameSysVal );
	//}}
	//{{ 2011. 11. 3	최육사	헤니르 시공 랭킹 보상 안전성 패치
	CASE_REFRESH_SCRIPT( OT_LG_TITLE_MANAGER, CXSLTitleManager );
	//}}

	//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	CASE_REFRESH_SCRIPT( OT_LG_LOG_MANAGER, KLogManager );
	//}}

	CASE_REFRESH_SCRIPT( OT_LG_SKILL_TREE, CXSLSkillTree );

	default:
		{
			START_LOG( cerr, L"이쪽으로 오면 안되는 타입인데?" )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		return;
	}

	// 센터서버의 로직 스레드인 LoginServer에서 스크립트를 교체하도록 처리!
	SendToServer( ESR_ORDER_TO_REFRESH_MANAGER_ACK, kPacket_ );
}


//}}
