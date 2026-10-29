#include "MailManager.h"

#include <dbg/dbg.hpp>

ImplToStringW( KMailManager )
{
    return stm_ << L"KMailManager (mail is logged, not sent)" << std::endl;
}

void KMailManager::LogMail( const KEMAIL_SEND_EMAIL_NOT& kMail )
{
    std::wstring wstrTo;
    for( const std::wstring& wstrAddr : kMail.m_vecRecvMailAddrList )
        wstrTo += wstrAddr + L" ";

    START_LOG( cwarn, L"[Mail] sending is not available in this port; mail written to the log instead" )
        << BUILD_LOG( wstrTo )
        << BUILD_LOG( kMail.m_wstrTitle )
        << BUILD_LOG( kMail.m_wstrDesc )
        << BUILD_LOG( kMail.m_vecAttachFileFullPathList.size() )
        << END_LOG;
}
