#pragma once

#include "Event.h"

#include "DEBBillingManager.h"


template <class T>
void KncSendToPublisherBilling( DWORD dwPIFrom_, UidType nFrom_, DWORD dwPITo_, UidType nTo_, UidType anTrace_[], u_short usEventID_, const T& data_ )
{
    KEventPtr spEvent( new KEvent );
    spEvent->SetData( dwPITo_, anTrace_, usEventID_, data_ );
    LIF( spEvent->m_kDestPerformer.AddUID( nTo_ ) );

	SiKDEBBillingManager()->QueueingEvent( spEvent );

}
