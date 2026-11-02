#include "ChannelScriptRefreshThread.h"
#include "ChannelScriptRefreshManager.h"

#include "BaseServer.h"

//{{ 2012. 10. 15	박세훈	채널 서버 실시간 스크립트 갱신 기능

//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
//}}
//{{ 2013. 02. 19	최육사	로그 시스템 개선
	#include "LogManager.h"
//}}

#define CASE_REFRESH_SCRIPT( flag, classname )	_CASE_REFRESH_SCRIPT( flag, classname, OpenScriptFile )

#define _CASE_REFRESH_SCRIPT( flag, classname, functionname ) \
case KESR_SCRIPT_REFRESH_ORDER_NOT::flag: \
	START_LOG( cout, L ## #classname L" script refresh started!" ); \
	{ \
		KLocker lock( classname::GetTempCS() ); \
		if( SiTemp##classname()->functionname( SiKChannelScriptRefreshManager()->GetRefreshLuaState() ) == false ) \
		{ \
			START_LOG( cerr, L"Script refresh failed!" ) \
			<< END_LOG; \
			return; \
		} \
	} \
	break; \


//{{ 2010. 06. 15  최육사	실시간 스크립트 갱신


#define CLASS_TYPE  KChannelScriptRefreshThread

KChannelScriptRefreshThread::KChannelScriptRefreshThread( void )
{
}

KChannelScriptRefreshThread::~KChannelScriptRefreshThread( void )
{
	End( 15000 );
}

void KChannelScriptRefreshThread::ProcessEvent( const KEventPtr& spEvent_ )
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
void KChannelScriptRefreshThread::SendToServer( unsigned short usEventID, const T& data )
{
	KEventPtr spEvent( new KEvent );
	spEvent->SetData( PI_CHANNEL_SERVER, NULL, usEventID, data );
	KBaseServer::GetKObj()->QueueingEvent( spEvent );
}

_IMPL_ON_FUNC( ESR_ORDER_TO_REFRESH_MANAGER_REQ, KESR_SCRIPT_REFRESH_ORDER_NOT )
{
	switch( kPacket_.m_iOrderType )
	{
	//{{ 2012. 10. 9	박세훈	SMS 전화번호 통합 관리
	//}}
	//{{ 2013. 02. 19	최육사	로그 시스템 개선
	CASE_REFRESH_SCRIPT( OT_CH_LOG_MANAGER, KLogManager );
	//}}

	default:
		{
			START_LOG( cerr, L"Unexpected type." )
				<< BUILD_LOG( kPacket_.m_iOrderType )
				<< END_LOG;
		}
		return;
	}

	// 로직 스레드에서 교체하도록 처리!
	SendToServer( ESR_ORDER_TO_REFRESH_MANAGER_ACK, kPacket_ );
}


//}}

//}}
