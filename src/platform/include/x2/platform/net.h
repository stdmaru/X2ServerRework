// Sockets with the Winsock names used by the original (SOCKET, closesocket, WSAGetLastError, SOCKADDR_IN...).
// Native Winsock 2 on Windows; BSD sockets elsewhere. Replaces `#include <winsock2.h>` / `<ws2tcpip.h>`.
// The asynchronous I/O (IOCP) is not here: the network layer is ported separately (server/common-net).
#pragma once

#include "x2/platform/sync.h" // HANDLE
#include "x2/platform/types.h"

#if defined(_WIN32)

#include <winsock2.h>
#include <ws2tcpip.h>
#undef GetCurrentTime // <windows.h> again (see types.h)
typedef int x2_socklen_t; // address length type of accept/getsockname (socklen_t elsewhere)

#else

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

typedef int SOCKET;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

typedef struct sockaddr     SOCKADDR, *PSOCKADDR, *LPSOCKADDR;
typedef struct sockaddr_in  SOCKADDR_IN, *PSOCKADDR_IN, *LPSOCKADDR_IN;
typedef struct in_addr      IN_ADDR;
typedef struct hostent      HOSTENT;
typedef struct linger       LINGER;
typedef socklen_t           x2_socklen_t;

#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR
#define SD_BOTH    SHUT_RDWR

struct WSADATA
{
    WORD wVersion = 0x0202;
    WORD wHighVersion = 0x0202;
};
typedef WSADATA* LPWSADATA;

#ifndef MAKEWORD
#define MAKEWORD(lo, hi) (static_cast<WORD>((static_cast<BYTE>(lo)) | (static_cast<WORD>(static_cast<BYTE>(hi)) << 8)))
#endif

inline int WSAStartup(WORD, LPWSADATA data) { if (data) *data = WSADATA{}; return 0; }
inline int WSACleanup() { return 0; }
inline int WSAGetLastError() { return errno; }
inline int closesocket(SOCKET s) { return ::close(s); }
inline int ioctlsocket(SOCKET s, long cmd, u_long* arg) { return ::ioctl(s, static_cast<unsigned long>(cmd), arg); }

#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAECONNRESET  ECONNRESET
#define WSAEINTR       EINTR
#define WSAEFAULT      EFAULT
#define WSAEINVAL      EINVAL
#define WSAEINPROGRESS EINPROGRESS
#define WSAEISCONN     EISCONN
#define WSAEMSGSIZE    EMSGSIZE
#define WSAENETDOWN    ENETDOWN
#define WSAENETRESET   ENETRESET
#define WSAENOTSOCK    ENOTSOCK
#define WSAEOPNOTSUPP  EOPNOTSUPP
#define WSAESHUTDOWN   ESHUTDOWN
#define WSAETIMEDOUT   ETIMEDOUT
#define WSANOTINITIALISED 10093 // Winsock's value: no errno has it, sockets need no start-up here

// Overlapped I/O names, so the headers of Common/Socket parse. Only the types: the IOCP sources are not
// compiled outside Windows, and the network layer is replaced in server/common-net.
struct OVERLAPPED // trivial like the Win32 struct: the original clears it with ZeroMemory
{
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    DWORD     Offset;
    DWORD     OffsetHigh;
    HANDLE    hEvent;
};
typedef OVERLAPPED* LPOVERLAPPED;
typedef OVERLAPPED  WSAOVERLAPPED, *LPWSAOVERLAPPED;

struct WSABUF
{
    ULONG len;
    char* buf;
};
typedef WSABUF* LPWSABUF;

#endif // _WIN32

#include <string>

namespace x2::platform {

// IPv4 text <-> address, in place of inet_addr/inet_ntoa (deprecated in Winsock). Dotted quads only: the
// shorthand forms inet_addr took ("127.1", hex) are rejected. IpToString returns a per-thread buffer, as
// inet_ntoa does.
inline const char* IpToString(in_addr addr)
{
    thread_local char buf[INET_ADDRSTRLEN];
    return inet_ntop(AF_INET, &addr, buf, sizeof(buf)) ? buf : "";
}

inline unsigned long IpFromString(const char* text)
{
    in_addr addr{};
    return text && inet_pton(AF_INET, text, &addr) == 1 ? addr.s_addr : INADDR_NONE;
}

// True when a receive on the socket would not block (data, or an error to report): poll with no wait, in place of
// WSAEventSelect + WSAEnumNetworkEvents(FD_READ).
inline bool SocketReadable(SOCKET s)
{
#if defined(_WIN32)
    WSAPOLLFD fd{};
    fd.fd = s;
    fd.events = POLLRDNORM;
    return ::WSAPoll(&fd, 1, 0) > 0 && (fd.revents & (POLLRDNORM | POLLERR | POLLHUP)) != 0;
#else
    pollfd fd{};
    fd.fd = s;
    fd.events = POLLIN;
    return ::poll(&fd, 1, 0) > 0 && (fd.revents & (POLLIN | POLLERR | POLLHUP)) != 0;
#endif
}

// Sends the buffers as one datagram, in order (WSASendTo without overlapped I/O; sendmsg elsewhere). Returns
// SOCKET_ERROR or 0, with the bytes sent in *sent.
inline int SendToScattered(SOCKET s, WSABUF* buffers, DWORD count, const sockaddr* to, int toLength, DWORD* sent)
{
#if defined(_WIN32)
    return ::WSASendTo(s, buffers, count, sent, 0, to, toLength, nullptr, nullptr);
#else
    iovec vectors[16];
    if (count > 16) {
        errno = EMSGSIZE;
        return SOCKET_ERROR;
    }
    for (DWORD i = 0; i < count; ++i) {
        vectors[i].iov_base = buffers[i].buf;
        vectors[i].iov_len = buffers[i].len;
    }
    msghdr message{};
    message.msg_name = const_cast<sockaddr*>(to);
    message.msg_namelen = static_cast<socklen_t>(toLength);
    message.msg_iov = vectors;
    message.msg_iovlen = count;
    const ssize_t n = ::sendmsg(s, &message, 0);
    if (n < 0) {
        return SOCKET_ERROR;
    }
    if (sent != nullptr) {
        *sent = static_cast<DWORD>(n);
    }
    return 0;
#endif
}

// First IPv4 address of a host name, or "" (getaddrinfo, in place of gethostbyname).
inline std::string ResolveIPv4(const char* host)
{
    addrinfo hints{};
    hints.ai_family = AF_INET;
    addrinfo* list = nullptr;
    if (getaddrinfo(host, nullptr, &hints, &list) != 0 || !list)
        return std::string();
    const std::string ip = IpToString(reinterpret_cast<const sockaddr_in*>(list->ai_addr)->sin_addr);
    freeaddrinfo(list);
    return ip;
}

} // namespace x2::platform
