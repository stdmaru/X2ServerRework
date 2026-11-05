#pragma once
#include "x2/platform/net.h"
#include "x2/platform/win32.h"    // CTime
#include "Socket/Overlapped.h"
#include "CircularQueue.h"
#include <Thread/Locker.h>
#include "SimObject.h"
#include "KncSmartPtr.h"

// 2006-01-16 florist: an event object is always created for now. It would be nice to make that optional,
// rename this to KSockBase and grow it into a base for every socket job
// (choice of I/O model, TCP/UDP, detailed socket options...).
//
// Port (decision T20): the socket is an x2::platform::net::TcpStream run by KIocp's I/O threads.
// Completions still arrive as OnRecvCompleted()/OnSendCompleted(), either through KIocp (completion key)
// or, when an event was set with SetIOEvent(), through OnIOCompleted() on the thread waiting on it.

SmartPointer( KSocketObject );

class KSocketObject : public KSimObject
{
public:
    KSocketObject(const KSocketObject& right);              // do not use (no implement.)
    KSocketObject& operator=(const KSocketObject& right);   // do not use (no implement.)

    KSocketObject(void);
    virtual ~KSocketObject(void);

public:
    void CloseSocket();
    bool Connect( const char* szIP = NULL,
                  unsigned short usPort = 0,
                  bool bUseIocp = true,
                  std::vector< std::pair<int,int> >* vecOpt = NULL );
    bool SendData( const char* szData, int iSize );
    bool InitRecv();
    void OnIOCompleted( KOVERLAPPED::ENUM_IO_MODE eMode ); // without IOCP: called after the event was signaled
    void OnSendCompleted( DWORD dwTransfered );

    // The remote address can be set beforehand or given to Connect().
    void SetRemoteIP( const char* szIP ) { m_sockaddr.sin_addr.s_addr   = x2::platform::IpFromString( szIP ); }
    void SetRemotePort( u_short usPort ) { m_sockaddr.sin_port          = htons( usPort ); }
    void SetSocketInfo( SOCKET socket, const SOCKADDR_IN& sockAddr ); // takes ownership of an accepted socket

    virtual void OnRecvCompleted( DWORD dwTransfered ) = 0;
    virtual void OnSocketError();
    virtual void OnAcceptConnection() {} // called right after accept

    bool    IsConnected() const     { return m_spStream && m_spStream->IsOpen(); }
    bool    IsSending() const       { return m_ovlSend.m_bSending; }
    void    SetKey( DWORD dwKey )   { m_dwKey = dwKey; }
    DWORD   GetKey() const          { return m_dwKey; }
    void    SetIOEvent( KOVERLAPPED::ENUM_IO_MODE eMode, HANDLE hEvent );   // event created by the caller
    // Port: the owner calls this before closing its events. Operations still pending (the aborted ones of a closed
    // socket complete later, on an I/O thread) are then dropped instead of signaling a closed event.
    void    DetachIOEvents();
    void    SetSendQueueSize( int nSize )   { m_kSendQueue.Resize( nSize ); }

    unsigned int GetIP() const			{ return m_sockaddr.sin_addr.s_addr; }
    const char* GetIPStr() const		{ return x2::platform::IpToString(m_sockaddr.sin_addr); }
    unsigned short GetPort() const		{ return ntohs(m_sockaddr.sin_port); }

    static void DumpSendQueueLog( std::wostream& stm ) { ms_kMaxSizeInfo.Dump( stm ); }

	int GetDisconnectReason() { return m_iDisconnectReason; }
	void SetDisconnectReason( int iDisconnectReason );

	//{{ 2009. 8. 31  최육사	IOCP종료사유
	DWORD GetLastIocpFailedError() { return m_dwLastIocpFailedError; }
	void SetLastIocpFailedError( DWORD dwLastIocpFailedError ) { m_dwLastIocpFailedError = dwLastIocpFailedError; }
	//}}

	SOCKET			GetSocket() const { return m_spStream ? m_spStream->Native() : INVALID_SOCKET; }
	KOVERLAPPED&	GetovlRecv(){return m_ovlRecv;}
	KOVERLAPPED&	GetovlSend(){return m_ovlSend;}

protected:
    inline void MovDataQue2Buf();   // moves data waiting in the send queue into the buffer
    void StartSend();               // sends m_ovlSend's buffer (m_csSock held)
    void OnAsyncCompleted( KOVERLAPPED& kOvl, const x2::platform::net::IoResult& kResult );
    void CloseStream();             // m_csSock held

protected:
    DWORD               m_dwKey;
    SOCKADDR_IN         m_sockaddr;
    KOVERLAPPED			m_ovlRecv;
    KOVERLAPPED			m_ovlSend;
    std::shared_ptr< x2::platform::net::TcpStream > m_spStream; // replaces the SOCKET; null when closed
    CRITICAL_SECTION    m_csSock;
    bool                m_bIOEvents = false;    // event mode (SetIOEvent): completions never go to KIocp

    KCircularQueue      m_kSendQueue;
    CRITICAL_SECTION    m_csSendQueue;

	// socket object 종료 사유
	int					m_iDisconnectReason;
	//{{ 2009. 8. 31  최육사	IOCP종료사유
	DWORD				m_dwLastIocpFailedError;
	//}}

    static struct KMaxSizeInfo {
        mutable KncCriticalSection  m_cs;
        int                         m_iMaxDataSize;
        int                         m_iQueueSize;
        CTime                       m_cTime;
        void Check( int iDataSize, int iQueueSize );
        void Dump( std::wostream& stm );
    } ms_kMaxSizeInfo;

    friend class KIocp;
};

//#define KNC_DONT_USE_IOCP
