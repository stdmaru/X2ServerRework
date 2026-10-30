// Asynchronous TCP I/O for the server's network layer (decision T20). Asio inside; nothing of it leaks here.
//
// The model is the one of IOCP, which the original was written for: operations are started from any thread
// and their completion handlers run on the threads that call IoService::RunFor() (the server's I/O threads).
#pragma once

#include "x2/platform/net.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <string>

namespace x2::platform::net {

enum class IoStatus
{
    Ok,       // `bytes` transferred
    Eof,      // the peer closed the connection (bytes == 0)
    Aborted,  // the socket was closed locally while the operation was pending
    Error,    // any other failure; see `error`
};

struct IoResult
{
    IoStatus    status = IoStatus::Ok;
    int         error = 0;  // system error code (errno / WSAGetLastError)
    std::size_t bytes = 0;
};

using IoHandler = std::function<void(const IoResult&)>;

class IoService
{
public:
    IoService();
    ~IoService();
    IoService(const IoService&) = delete;
    IoService& operator=(const IoService&) = delete;

    // Runs ready completion handlers for up to `milliseconds`; returns how many ran.
    std::size_t RunFor(unsigned milliseconds);
    void Stop();         // makes every RunFor() return; pending work stays queued
    void Restart();      // after Stop(), lets RunFor() run handlers again
    bool Stopped() const;
    // Runs, on the calling thread, every handler still queued (e.g. the Aborted completions of sockets just
    // closed), until none is left. Call it once the I/O threads are gone and before destroying the objects
    // the handlers reference; the destructor drains too, as a last resort.
    void Drain();

    struct Impl;
    Impl& GetImpl() { return *m_impl; }

private:
    std::unique_ptr<Impl> m_impl;
};

class TcpStream
{
public:
    explicit TcpStream(IoService& io);
    ~TcpStream();  // closes the socket
    TcpStream(const TcpStream&) = delete;
    TcpStream& operator=(const TcpStream&) = delete;

    bool Open(int* error = nullptr);                          // IPv4 TCP socket, not connected yet
    bool SetOption(int level, int name, int value);           // setsockopt with an int value
    bool Connect(const SOCKADDR_IN& addr, int* error = nullptr);  // blocking; opens the socket if needed
    bool Assign(SOCKET native, int* error = nullptr);         // takes ownership of an accepted socket

    // At most one read and one write may be pending at a time; the buffer must stay valid until the handler runs.
    void AsyncReadSome(char* buffer, std::size_t size, IoHandler handler);
    void AsyncWriteSome(const char* buffer, std::size_t size, IoHandler handler);

    void Close();  // pending operations complete with IoStatus::Aborted
    bool IsOpen() const;
    SOCKET Native() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

// Text of a socket error code (the original used FormatMessage on WSAGetLastError()).
std::wstring SocketErrorMessage(int error);
int LastSocketError();

} // namespace x2::platform::net
