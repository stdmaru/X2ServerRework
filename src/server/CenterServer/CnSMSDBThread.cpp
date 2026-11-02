#include "x2/platform/net.h"
#include <iomanip>
#include <sstream>

#include <dbg/dbg.hpp>

#include "CnSMSDBThread.h"
#include "CnServer.h"
#include "ActorManager.h"
#include "NetError.h"
#include "CnSimLayer.h"

#define CLASS_TYPE KCnSMSDBThread
ImplPfID( KCnSMSDBThread, PI_CN_SMS_DB );

KCnSMSDBThread::~KCnSMSDBThread(void)
{
}

void KCnSMSDBThread::ProcessEvent( const KEventPtr& spEvent_ )
{
	switch( spEvent_->m_usEventID )
	{
		CASE( DBE_SEND_PHONE_MSG_NOT );
		//{{ 2010. 11. 2	최육사	서버간 접속 모니터링
		//}}

	default:
		START_LOG( cerr, L"Event handler is not defined. " << spEvent_->GetIDStr() );
	}
}

void KCnSMSDBThread::SendToServer( unsigned short usEventID )
{
	SendToServer( usEventID, char() );
}

void KCnSMSDBThread::SendToUser( UidType nTo, unsigned short usEventID )
{
	SendToUser( nTo, usEventID, char() );
}

void KCnSMSDBThread::SendToGSUser( UidType nGS, UidType nTo, unsigned short usEventID )
{
	SendToGSUser( nGS, nTo, usEventID, char() );
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

	START_LOG( cout2, L"Mobile message sent successfully!" )
		<< BUILD_LOG( kPacket_.m_wstrSMSMessage );
	return;

end_proc:
	START_LOG( cerr, L"Failed to send mobile message!" )
		<< BUILD_LOG( kPacket_.m_wstrSMSMessage );
}

//{{ 2010. 11. 2	최육사	서버간 접속 모니터링
//}}
