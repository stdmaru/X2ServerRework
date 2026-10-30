#pragma once
#include "x2/platform/net_io.h"
#include "x2/platform/win32.h"    // ULONG_PTR
#include <KNCSingleton.h>
#include "ThreadManager.h"

#include "Session.h"

// The server's I/O completion engine: the socket objects registered with a completion key, and the I/O
// threads (KIOThread) that run the completions.
// Port (decision T20): the Win32 completion port became an x2::platform::net::IoService. The completion keys
// and the OnIOCompleted()/OnClosedByLocal() dispatch are kept, so sessions behave as before.
class KIocp : public KThreadManager
{
    DeclareSingleton( KIocp );
    DeclToStringW;
    NiDeclareRTTI;
    DeclPfID;

public:
    enum
    {
        SIGNAL_SHUTDOWN_SERVER = 0
    };

protected:
    KIocp();

public:
    virtual ~KIocp();

    virtual void EndThread( DWORD dwTimeOut ) { KThreadManager::EndThread( dwTimeOut ); } // the original hid it; same behavior
    virtual void EndThread();  // derived from KThreadManager

    bool AssociateSocket( KSocketObjectPtr spSockObj );
    // Only SIGNAL_SHUTDOWN_SERVER is posted by the server: it stops the I/O threads.
    bool PostStatus( ULONG_PTR CompKey );

    bool DeleteCompKey( DWORD dwKey );
    void OnIOCompleted( IN DWORD dwKey, IN DWORD dwBytesTransfer, IN KOVERLAPPED* povl );
    void OnClosedByLocal( IN DWORD dwKey, IN int iError );
    size_t GetSockObjNum() const;

    x2::platform::net::IoService& GetIoService() { return m_kIoService; }

private:
    KSocketObjectPtr GetSockObj( IN DWORD dwKey );
    virtual KThread* CreateThread();  // derived from KThreadManager
    inline DWORD GenNewCompKey() const;

    x2::platform::net::IoService m_kIoService;
    mutable KncCriticalSection m_csSockObj;
    std::map< DWORD, KSocketObjectPtr > m_mapSockObj;

    mutable KncCriticalSection  m_cs;               // Critical Section
    mutable DWORD               m_dwTerm;           // longest time taken to generate a key
    mutable CTime               m_cTime;            // when that happened
    mutable DWORD               m_dwSockObjNum;

    friend class KIOThread;
};

DefSingletonInline( KIocp );
