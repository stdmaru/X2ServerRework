// Asynchronous TCP over loopback: connect, accept (with plain sockets, as KAccepter does), read and write.
#include "x2/platform/net_io.h"

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

using namespace x2::platform::net;

namespace {

struct Listener
{
    SOCKET sock = INVALID_SOCKET;
    SOCKADDR_IN addr{};

    Listener()
    {
        sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = 0;
        REQUIRE(::bind(sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)) == 0);
        REQUIRE(::listen(sock, 4) == 0);
        x2_socklen_t len = sizeof(addr);
        REQUIRE(::getsockname(sock, reinterpret_cast<SOCKADDR*>(&addr), &len) == 0);
    }
    ~Listener() { closesocket(sock); }

    SOCKET Accept() { return ::accept(sock, nullptr, nullptr); }
};

// runs the service on a background thread, like the server's I/O threads
struct Runner
{
    IoService& io;
    std::atomic<bool> stop{ false };
    std::thread thread;
    explicit Runner(IoService& s) : io(s), thread([this] { while (!stop) io.RunFor(20); }) {}
    ~Runner() { stop = true; thread.join(); }
};

template <typename F>
bool WaitFor(F&& done)
{
    for (int i = 0; i < 300 && !done(); ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return done();
}

} // namespace

TEST_CASE("TcpStream: connect, accept, async read/write, close")
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    IoService io;
    Runner runner(io);
    Listener listener;

    TcpStream client(io);
    REQUIRE(client.Connect(listener.addr));
    TcpStream server(io);
    REQUIRE(server.Assign(listener.Accept()));
    CHECK(server.IsOpen());
    CHECK(client.Native() != INVALID_SOCKET);

    // client -> server
    char rbuf[64] = {};
    std::atomic<int> got{ -1 };
    server.AsyncReadSome(rbuf, sizeof(rbuf), [&](const IoResult& r) { got = r.status == IoStatus::Ok ? int(r.bytes) : -2; });
    const std::string msg = "hello";
    std::atomic<int> sent{ -1 };
    client.AsyncWriteSome(msg.data(), msg.size(), [&](const IoResult& r) { sent = int(r.bytes); });
    REQUIRE(WaitFor([&] { return got >= 0 && sent >= 0; }));
    CHECK(sent == 5);
    CHECK(std::string(rbuf, size_t(got.load())) == "hello");

    // peer close -> Eof
    std::atomic<int> status{ -1 };
    server.AsyncReadSome(rbuf, sizeof(rbuf), [&](const IoResult& r) { status = int(r.status); });
    client.Close();
    REQUIRE(WaitFor([&] { return status >= 0; }));
    CHECK(status == int(IoStatus::Eof));

    // local close -> Aborted
    status = -1;
    TcpStream c2(io);
    REQUIRE(c2.Connect(listener.addr));
    TcpStream s2(io);
    REQUIRE(s2.Assign(listener.Accept()));
    s2.AsyncReadSome(rbuf, sizeof(rbuf), [&](const IoResult& r) { status = int(r.status); });
    s2.Close();
    REQUIRE(WaitFor([&] { return status >= 0; }));
    CHECK(status == int(IoStatus::Aborted));
}

TEST_CASE("TcpStream: connection refused")
{
    IoService io;
    SOCKADDR_IN addr{};
    {
        Listener unused; // grab a free port, then close it
        addr = unused.addr;
    }
    TcpStream s(io);
    int err = 0;
    CHECK_FALSE(s.Connect(addr, &err));
    CHECK(err != 0);
    CHECK_FALSE(SocketErrorMessage(err).empty());
}
