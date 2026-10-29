#pragma once

// Port: the original sent e-mail through closed-source SMTP/MIME DLLs (X2SMTPMail). The only sender is
// KBaseServer::OnServerReadyComplete (script parsing error report, when enabled in the config). This
// replacement keeps the interface and writes the mail to the log instead of sending it.
// See docs/porte/server-common.md (pendências).

#include "Event.h"
#include "ServerPacket.h"

#include <ToString.h>

class KMailManager
{
    DeclToStringW;
    DeclDump;

public:
    KMailManager() = default;
    virtual ~KMailManager() = default;

    void Init() {}
    void Shutdown() {}

    template < class T >
    void QueueingEvent( unsigned short usEventID, const T& data );
    void QueueingEventID( unsigned short /*usEventID*/ ) {}

protected:
    void LogMail( const KEMAIL_SEND_EMAIL_NOT& kMail );
};

template < class T >
void KMailManager::QueueingEvent( unsigned short usEventID, const T& data )
{
    if constexpr ( std::is_same_v< T, KEMAIL_SEND_EMAIL_NOT > )
    {
        if( usEventID == EMAIL_SEND_EMAIL_NOT )
            LogMail( data );
    }
}
