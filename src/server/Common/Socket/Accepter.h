#pragma once

#include "x2/platform/net.h"
#include "x2/platform/win32.h"
#include <Thread/Thread.h>

class KAccepter : public KThread
{
    typedef void (*PFNOnAccept)( SOCKET, const SOCKADDR_IN& );

public:
    KAccepter();
    virtual ~KAccepter() {}

    void Init( u_short usPort, PFNOnAccept fnOnAccept, bool bNagleAlgOn = true );
    bool GetNagleAlgOn() const   { return m_bNagleAlgOn; }

    // derived from KThread
    virtual void End( DWORD dwTimeOut = 3000 );

protected:
    // derived from KThread
    virtual void Run();
    inline void Loop();

    SOCKET          m_sock;
    PFNOnAccept     m_fnOnAccept;
    bool            m_bNagleAlgOn;
};
