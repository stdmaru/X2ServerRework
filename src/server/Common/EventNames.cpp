// The servers' half of KEvent (decision T34): the names of every event, for the logs, and the check that a game
// client did not send a server event. The client has its own, smaller table (src/client/X2ServerProtocol/EventNames.cpp).
#include "Event.h"

#undef _ENUM
#define _ENUM( id ) L ## #id,
const wchar_t*  KEvent::ms_szEventID[] = {
#   include "EventID_System.h"
#   include "EventID_Client.h"
#   include "EventID_Server.h"
};

const wchar_t* KEvent::GetIDStr( const unsigned short usEventID )
{
    if( usEventID >= EVENT_EVENTID_SENTINEL )
    {
        return ms_szEventID[ EVENT_EVENTID_SENTINEL ];
    }
    return ms_szEventID[ usEventID ];
}

// 2011-06-08: events that came from a game client must not be server events
bool KEvent::IsValidEventID() const
{
#undef _ENUM
#define _ENUM( id ) case id:

	if( m_usFromType == EFT_CLIENT )
	{
		switch( m_usEventID )
		{
#	include "EventID_Server.h"
			return false;
		}
	}

	return true;
}
