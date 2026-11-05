// The client's half of KEvent (decision T34; the servers' is src/server/Common/EventNames.cpp): names, for the logs, of
// the system and client events only. The server-to-server events (EventID_Server.h) are left out so their names do
// not ship with the game; a client never sends or receives them, and GetIDStr() names them all as the last entry.
// IsValidEventID() is the servers' check on what clients send and is not defined here.
#include "Event.h"

#undef _ENUM
#define _ENUM( id ) L ## #id,
const wchar_t*  KEvent::ms_szEventID[] = {
#   include "EventID_System.h"
#   include "EventID_Client.h"
};

const wchar_t* KEvent::GetIDStr( const unsigned short usEventID )
{
    if( usEventID >= EGS_CLIENT_EVENT_ID_END )
    {
        return ms_szEventID[ EGS_CLIENT_EVENT_ID_END ];
    }
    return ms_szEventID[ usEventID ];
}
