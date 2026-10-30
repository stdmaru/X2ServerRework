#include "SocketObject.h"
#include "IOThread.h"
#include "dbg/dbg.hpp"
#include "IOCP.h"

_ImplementException( KIOThread )

KIOThread::KIOThread(void)
{
}

KIOThread::~KIOThread(void)
{
    // Port: the original also posted SIGNAL_SHUTDOWN_SERVER here, which ended whichever I/O thread dequeued it
    // (not necessarily this one). Ending this thread is enough: Loop() returns at least every 100 ms.
    End();
}

void KIOThread::Run()
{
    START_LOG( clog, L"Enter Run()" );

    DWORD ret;

    while( true )
    {
        ret = ::WaitForSingleObject( m_hKillEvent, 0 );

        if( ret == WAIT_OBJECT_0 ) break;

        if( ret == WAIT_TIMEOUT ) Loop();

        else std::cout <<  "*** WaitForSingleObject() - return : "<< ret << std::endl;

    }// while
}

// Runs I/O completions (KSocketObject::OnAsyncCompleted -> KIocp) for up to 100 ms.
void KIOThread::Loop()
{
    _BEGIN_TRY_BLOCK;

    x2::platform::net::IoService& kIoService = SiKIocp()->GetIoService();

    if( kIoService.Stopped() )
    {
        // SIGNAL_SHUTDOWN_SERVER (KIocp::PostStatus) was posted
        std::cout << "*** SIGNAL_SHUTDOWN_SERVER entered.\n" << std::endl;
        SetEvent( m_hKillEvent );
        return;
    }

    kIoService.RunFor( 100 );

    _END_TRY_BLOCK( L"" );
}
