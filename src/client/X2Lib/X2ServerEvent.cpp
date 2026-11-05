#include "stdafx.h"
#include "./X2ServerEvent.h"

#undef _ENUM
#define _ENUM( id ) L#id,

#if X2_DEV_TOOLS
const WCHAR*  CX2ServerEvent::SERVER_EVENT_ID_STR[] = {
#   include "EventID_System.h"
#   include "EventID_Client.h"
};


#else
const WCHAR*  CX2ServerEvent::SERVER_EVENT_ID_STR[] = {
	L"",
};

#endif



CX2ServerEvent::CX2ServerEvent(void)
{
}

CX2ServerEvent::~CX2ServerEvent(void)
{
}
