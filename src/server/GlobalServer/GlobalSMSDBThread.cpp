#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "GlobalSMSDBThread.h"
#include "GlobalServer.h"
#include "NetError.h"
#include "GlobalSimLayer.h"

//ImplementDBThread( KGlobalSMSDBThread );

#define CLASS_TYPE KGlobalSMSDBThread

KGlobalSMSDBThread::~KGlobalSMSDBThread(void)
{
}

void KGlobalSMSDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
	switch( spEvent_->m_usEventID )
	{
		CASE( DBE_SEND_PHONE_MSG_NOT );
		//{{ 2010. 11. 2	최육사	서버간 접속 모니터링
		//}}

	default:
		START_LOG( cerr, L"Event handler is not defined: " << spEvent_->GetIDStr() );
	}
}

void KGlobalSMSDBThread::SendToServer( unsigned short usEventID )
{
	SendToServer( usEventID, char() );
}

void KGlobalSMSDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
	SendToUser( nTo, usEventID, char() );
}

IMPL_ON_FUNC( DBE_SEND_PHONE_MSG_NOT )
{
	for( const std::wstring& wstrPhoneNum : kPacket_.m_vecPhoneNum )
	{
        std::wstring wstrProject = L"ES";
        std::wstring wstrNation = KSimLayer::GetKObj()->GetAuthTypeStr( KSimLayer::GetKObj()->GetAuthFlag() );
        DO_QUERY_NO_PROFILE( L"exec SMS.dbo.SMS_Send", L"N\'%s\', N\'%s\',N\'%s\',N\'%s\',N\'%s\',%d, %d",
            % wstrPhoneNum
            % wstrPhoneNum
            % kPacket_.m_wstrSMSMessage
            % wstrProject
            % wstrNation
            % 0
            % 0
            );
	}

	START_LOG( cout2, L"Successfully sent the text message." )
		<< BUILD_LOG( kPacket_.m_wstrSMSMessage );
	return;

end_proc:
	START_LOG( cerr, L"Failed to send the text message." )
		<< BUILD_LOG( kPacket_.m_wstrSMSMessage );
}

//{{ 2010. 11. 2	최육사	서버간 접속 모니터링
//}}
