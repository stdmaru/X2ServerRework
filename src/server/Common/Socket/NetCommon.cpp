#include "NetCommon.h"
#include "x2/platform/net_io.h"
#include <string>
#include <KncUtil.h>
// Port: FormatMessage became x2::platform::net::SocketErrorMessage. The text lives per thread
// (the original shared one static buffer between all threads).
const wchar_t* NetCommon::GetWsaMsg()
{
    thread_local std::wstring strMsg;
    strMsg = x2::platform::net::SocketErrorMessage( x2::platform::net::LastSocketError() );
    return strMsg.c_str();
}

namespace { std::string s_strLocalIP; }

void NetCommon::SetLocalIP( const char* szIP )
{
    s_strLocalIP = szIP ? szIP : "";
}

const char* NetCommon::GetLocalIP()
{
    if( !s_strLocalIP.empty() )
        return s_strLocalIP.c_str();

    char szHostName[256];
    HOSTENT* pHostInfo;

    if( gethostname(szHostName,sizeof(szHostName)) !=0)
    {
        return "";  // Port: the original returned false (NULL), which GetLocalIPW() then turned into a std::string
    }

    pHostInfo = gethostbyname(szHostName);

    if( pHostInfo == NULL)
    {
        return "";
    }

    return inet_ntoa(*(struct in_addr *)*pHostInfo->h_addr_list);
}

std::wstring NetCommon::GetLocalIPW()
{
	std::string strLocalIP = GetLocalIP();
	std::wstring wstrLocalIP = KncUtil::toWideString( strLocalIP.c_str(), strLocalIP.length() );

	return wstrLocalIP;
}

bool NetCommon::InitWinSock()
{
    // socket init
    WORD        wVer = MAKEWORD(2, 2);
    WSADATA     wsaData;

    if( ::WSAStartup(wVer, &wsaData) != 0 )
    {
        ::WSACleanup();
        return false;
    }

    if( wsaData.wVersion != MAKEWORD(2,2) )
    {
        ::WSACleanup();
        return false;
    }

    return true;
}

const char* NetCommon::CalcIP( DWORD dwIP )
{
    in_addr IP;
    IP.s_addr = dwIP;
    return inet_ntoa( IP );
}
