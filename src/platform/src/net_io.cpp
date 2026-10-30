#include "x2/platform/net_io.h"
#include "x2/platform/unicode.h"

#include <asio.hpp>

#include <chrono>
#include <optional>
#include <system_error>

namespace x2::platform::net {

struct IoService::Impl
{
    asio::io_context io;
    // keeps RunFor() waiting for work instead of returning at once when nothing is pending
    std::optional<asio::executor_work_guard<asio::io_context::executor_type>> work{ asio::make_work_guard(io) };
};

IoService::IoService() : m_impl(std::make_unique<Impl>()) {}

IoService::~IoService()
{
    // handlers destroyed by ~io_context would release sockets while its services are going away
    Drain();
}

void IoService::Drain()
{
    m_impl->work.reset();   // poll() returns once nothing is pending
    m_impl->io.restart();
    while (m_impl->io.poll() > 0) {
        m_impl->io.restart();
    }
    m_impl->work.emplace(asio::make_work_guard(m_impl->io));
}

std::size_t IoService::RunFor(unsigned milliseconds)
{
    return m_impl->io.run_for(std::chrono::milliseconds(milliseconds));
}

void IoService::Stop() { m_impl->io.stop(); }
void IoService::Restart() { m_impl->io.restart(); }
bool IoService::Stopped() const { return m_impl->io.stopped(); }

struct TcpStream::Impl
{
    explicit Impl(asio::io_context& io) : socket(io) {}
    asio::ip::tcp::socket socket;
};

namespace {

IoResult ToResult(const std::error_code& ec, std::size_t bytes)
{
    IoResult r;
    r.bytes = bytes;
    if (!ec)
        r.status = IoStatus::Ok;
    else if (ec == asio::error::eof)
        r.status = IoStatus::Eof;
    else if (ec == asio::error::operation_aborted)
        r.status = IoStatus::Aborted;
    else
        r.status = IoStatus::Error;
    r.error = ec.value();
    return r;
}

bool Fail(const std::error_code& ec, int* error)
{
    if (error)
        *error = ec.value();
    return false;
}

} // namespace

TcpStream::TcpStream(IoService& io) : m_impl(std::make_unique<Impl>(io.GetImpl().io)) {}

TcpStream::~TcpStream() { Close(); }

bool TcpStream::Open(int* error)
{
    std::error_code ec;
    if (!m_impl->socket.is_open())
        m_impl->socket.open(asio::ip::tcp::v4(), ec);
    return ec ? Fail(ec, error) : true;
}

bool TcpStream::SetOption(int level, int name, int value)
{
    if (!m_impl->socket.is_open())
        return false;
    return ::setsockopt(m_impl->socket.native_handle(), level, name, reinterpret_cast<const char*>(&value),
                        sizeof(value)) == 0;
}

bool TcpStream::Connect(const SOCKADDR_IN& addr, int* error)
{
    if (!Open(error))
        return false;
    const asio::ip::tcp::endpoint ep(asio::ip::address_v4(ntohl(addr.sin_addr.s_addr)), ntohs(addr.sin_port));
    std::error_code ec;
    m_impl->socket.connect(ep, ec);
    if (ec) {
        std::error_code ignored;
        m_impl->socket.close(ignored);
        return Fail(ec, error);
    }
    return true;
}

bool TcpStream::Assign(SOCKET native, int* error)
{
    std::error_code ec;
    m_impl->socket.assign(asio::ip::tcp::v4(), native, ec);
    return ec ? Fail(ec, error) : true;
}

void TcpStream::AsyncReadSome(char* buffer, std::size_t size, IoHandler handler)
{
    m_impl->socket.async_read_some(asio::buffer(buffer, size),
        [h = std::move(handler)](const std::error_code& ec, std::size_t n) { h(ToResult(ec, n)); });
}

void TcpStream::AsyncWriteSome(const char* buffer, std::size_t size, IoHandler handler)
{
    m_impl->socket.async_write_some(asio::buffer(buffer, size),
        [h = std::move(handler)](const std::error_code& ec, std::size_t n) { h(ToResult(ec, n)); });
}

void TcpStream::Close()
{
    if (!m_impl->socket.is_open())
        return;
    std::error_code ignored;
    m_impl->socket.cancel(ignored);  // pending operations end as Aborted (on Windows a shutdown first ends reads as EOF)
    // like the original CLOSE_SOCKET: shutdown both directions, then close
    m_impl->socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored);
    m_impl->socket.close(ignored);
}

bool TcpStream::IsOpen() const { return m_impl->socket.is_open(); }

SOCKET TcpStream::Native() const
{
    return m_impl->socket.is_open() ? static_cast<SOCKET>(m_impl->socket.native_handle()) : INVALID_SOCKET;
}

std::wstring SocketErrorMessage(int error)
{
    return Utf8ToWide(std::system_category().message(error));
}

int LastSocketError()
{
#if defined(_WIN32)
    return ::WSAGetLastError();
#else
    return errno;
#endif
}

} // namespace x2::platform::net
