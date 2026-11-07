#pragma once

// The client's events are the servers' ENUM_CLIENT_EVENT_ID (Event.h, shared since T34). The original client had its
// own Event.h with only the system events and declared the client's here, with the same values: EVENT_X2_STARTUP was
// E_SYSTEM_EVENT_ID_END, as E_CLIENT_EVENT_ID_BEGIN is.
using SERVER_EVENT_ID = ENUM_CLIENT_EVENT_ID;
inline constexpr SERVER_EVENT_ID EVENT_X2_STARTUP = E_CLIENT_EVENT_ID_BEGIN;


class CX2ServerEvent
{
	public:
		CX2ServerEvent(void);
		~CX2ServerEvent(void);

		static const WCHAR*   SERVER_EVENT_ID_STR[];

};
