#pragma once

#include "x2/platform/net_io.h"
#include "x2/platform/types.h"

//#define MAX_PACKET_SIZE 16384	// 1024 * 16 (16Kbyte)
#define MAX_PACKET_SIZE 32768	// 1024 * 32 (32Kbyte)
//#define MAX_PACKET_SIZE 65536	// 1024 * 64 (64Kbyte)

// Buffer and state of one direction (recv or send) of a socket object.
// Port: the original derived from OVERLAPPED and carried a WSABUF for WSARecv()/WSASend(); the I/O now goes
// through x2::platform::net (decision T20), so only the buffer, the mode and the completion event remain.
class KOVERLAPPED
{
public:
    enum ENUM_IO_MODE {
        IO_RECV,
        IO_SEND
    };

    KOVERLAPPED()
    {
        hEvent                = NULL;
        m_dwLeft              = 0;
        m_bSending            = false;
    }

    void Clear()
    {
        m_kResult = x2::platform::net::IoResult();
    }

public:
    ENUM_IO_MODE            m_eIOMode;
    // set when the owner waits on an event instead of the I/O threads (KSession without IOCP)
    HANDLE                  hEvent;
    // result of the last operation, read by KSocketObject::OnIOCompleted() in the event mode
    x2::platform::net::IoResult m_kResult;
    char                    m_pBuffer[MAX_PACKET_SIZE];
    // send: bytes waiting to be sent; recv: bytes received but not yet framed into a packet
    DWORD                   m_dwLeft;
    bool                    m_bSending;
};
