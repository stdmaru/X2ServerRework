#pragma once
#include "x2/platform/net.h"
#include "x2/platform/win32.h"
#include <string>
// shutdown with SD_BOTH is the same as calling closesocket right away:
// data being sent or received at that moment may be lost.
#define CLOSE_SOCKET( socket ) \
    if( socket != INVALID_SOCKET ) \
    { \
    ::shutdown( socket, SD_BOTH ); \
    ::closesocket( socket ); \
    socket = INVALID_SOCKET; \
    }

#define GET_WSA_MSG NetCommon::GetWsaMsg()

namespace NetCommon
{
    const wchar_t* GetWsaMsg();
    const char* GetLocalIP();
    // Fixes the address GetLocalIP() returns (config: SetLocalIP( "127.0.0.1" )); empty goes back to the
    // original lookup, the first address of the host name. The DB identifies each server by this address.
    void SetLocalIP( const char* szIP );
	std::wstring GetLocalIPW();
    bool InitWinSock();
    const char* CalcIP( DWORD dwIP );

}
