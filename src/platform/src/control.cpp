#include "x2/platform/control.h"

#if defined(_WIN32)
#include "x2/platform/net.h" // <winsock2.h>
#include <afunix.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#endif

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <filesystem>
#include <map>
#include <mutex>
#include <thread>

namespace x2::platform {

namespace {

#if defined(_WIN32)
using Socket = SOCKET;
constexpr Socket kNoSocket = INVALID_SOCKET;
void CloseSocket(Socket s) { ::closesocket(s); }
void ShutdownSocket(Socket s, bool both) { ::shutdown(s, both ? SD_BOTH : SD_SEND); }

void InitSockets()
{
    static const bool started = [] {
        WSADATA data;
        return ::WSAStartup(MAKEWORD(2, 2), &data) == 0;
    }();
    (void)started;
}
#else
using Socket = int;
constexpr Socket kNoSocket = -1;
void CloseSocket(Socket s) { ::close(s); }
void ShutdownSocket(Socket s, bool both) { ::shutdown(s, both ? SHUT_RDWR : SHUT_WR); }
void InitSockets() {}
#endif

constexpr std::size_t kMaxQueued = 8 * 1024 * 1024;

bool FillAddress(const std::string& path, sockaddr_un& addr)
{
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (path.size() >= sizeof(addr.sun_path))
        return false;
    std::memcpy(addr.sun_path, path.c_str(), path.size() + 1);
    return true;
}

Socket ConnectTo(const std::string& path)
{
    sockaddr_un addr;
    if (!FillAddress(path, addr))
        return kNoSocket;
    const Socket s = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == kNoSocket)
        return kNoSocket;
    if (::connect(s, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0) {
        CloseSocket(s);
        return kNoSocket;
    }
    return s;
}

void SetNonBlocking(Socket s)
{
#if defined(_WIN32)
    u_long on = 1;
    ::ioctlsocket(s, FIONBIO, &on);
#else
    ::fcntl(s, F_SETFL, ::fcntl(s, F_GETFL, 0) | O_NONBLOCK);
#endif
}

bool WouldBlock()
{
#if defined(_WIN32)
    return ::WSAGetLastError() == WSAEWOULDBLOCK;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}

bool WaitWritable(Socket s, int ms)
{
#if defined(_WIN32)
    WSAPOLLFD pfd = {s, POLLWRNORM, 0};
    return ::WSAPoll(&pfd, 1, ms) > 0;
#else
    struct pollfd pfd = {s, POLLOUT, 0};
    return ::poll(&pfd, 1, ms) > 0;
#endif
}

// With `cancel`, the socket is non-blocking and the wait gives up once it is set.
bool SendAll(Socket s, std::string_view data, const std::atomic<bool>* cancel = nullptr)
{
    while (!data.empty()) {
        if (cancel) {
            if (*cancel)
                return false;
            if (!WaitWritable(s, 100))
                continue;
        }
        const int chunk = static_cast<int>(std::min<std::size_t>(data.size(), 1 << 20));
#if defined(_WIN32)
        const int n = ::send(s, data.data(), chunk, 0);
#else
        const ssize_t n = ::send(s, data.data(), static_cast<std::size_t>(chunk), MSG_NOSIGNAL);
        if (n < 0 && errno == EINTR)
            continue;
#endif
        if (n < 0 && cancel && WouldBlock())
            continue;
        if (n <= 0)
            return false;
        data.remove_prefix(static_cast<std::size_t>(n));
    }
    return true;
}

// Waits up to `ms` for `s` to be readable (data, a connection to accept, or its end).
// Blocking calls are not used on the server side: on Windows, closing or shutting down an AF_UNIX socket from
// another thread does not wake an accept or recv blocked on it.
bool WaitReadable(Socket s, int ms)
{
#if defined(_WIN32)
    WSAPOLLFD pfd = {s, POLLRDNORM, 0};
    return ::WSAPoll(&pfd, 1, ms) > 0;
#else
    struct pollfd pfd = {s, POLLIN, 0};
    return ::poll(&pfd, 1, ms) > 0;
#endif
}

// Reads into `pending` until it holds a line; false at the end of the connection or once `cancel` is set.
bool ReceiveLine(Socket s, std::string& pending, std::string& line, const std::atomic<bool>* cancel = nullptr)
{
    for (;;) {
        const std::size_t end = pending.find('\n');
        if (end != std::string::npos) {
            line.assign(pending, 0, end);
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            pending.erase(0, end + 1);
            return true;
        }
        if (cancel) {
            if (*cancel)
                return false;
            if (!WaitReadable(s, 100))
                continue;
        }
        char buffer[4096];
#if defined(_WIN32)
        const int n = ::recv(s, buffer, sizeof(buffer), 0);
#else
        const ssize_t n = ::recv(s, buffer, sizeof(buffer), 0);
        if (n < 0 && errno == EINTR)
            continue;
#endif
        if (n < 0 && cancel && WouldBlock())
            continue;
        if (n <= 0)
            return false;
        pending.append(buffer, static_cast<std::size_t>(n));
    }
}

} // namespace

namespace {

std::string g_controlDir;     // SetControlSocketDir

} // namespace

void SetControlSocketDir(std::string dir)
{
    g_controlDir = std::move(dir);
}

std::string ControlSocketDir()
{
    namespace fs = std::filesystem;
    const char* configured = std::getenv("X2_CONTROL_DIR");
    if (!g_controlDir.empty() || (configured && *configured)) {
        const fs::path dir = !g_controlDir.empty() ? fs::path(g_controlDir) : fs::path(configured);
        std::error_code ec;
        if (!fs::exists(dir, ec)) {
            fs::create_directories(dir, ec);
#if !defined(_WIN32)
            ::chmod(dir.c_str(), 0700);
#endif
        }
        return dir.string();
    }
    fs::path dir;
#if defined(_WIN32)
    dir = fs::temp_directory_path() / "x2";
    std::error_code ec;
    fs::create_directories(dir, ec);
#else
    const char* runtime = std::getenv("XDG_RUNTIME_DIR");
    if (runtime && *runtime)
        dir = fs::path(runtime) / "x2";
    else
        dir = fs::path("/tmp") / ("x2-" + std::to_string(::getuid()));
    ::mkdir(dir.c_str(), 0700);
    ::chmod(dir.c_str(), 0700);
#endif
    return dir.string();
}

std::string ControlSocketPath(const std::string& name)
{
    return (std::filesystem::path(ControlSocketDir()) / (name + ".sock")).string();
}

std::vector<std::string> ListControlSockets()
{
    return ListControlSockets(ControlSocketDir());
}

std::vector<std::string> ListControlSockets(const std::string& dir)
{
    InitSockets();
    std::vector<std::string> names;
    std::error_code ec;
    // increment(ec): nothing here may throw on a socket file (see Start)
    for (std::filesystem::directory_iterator it(dir, ec), end; !ec && it != end; it.increment(ec)) {
        const std::filesystem::path& path = it->path();
        if (path.extension() != ".sock")
            continue;
        const Socket s = ConnectTo(path.string());
        if (s == kNoSocket)
            continue;
        CloseSocket(s);
        names.push_back(path.stem().string());
    }
    std::sort(names.begin(), names.end());
    return names;
}

//////////////////////////////////////////////////////////////////////////
// ControlServer

struct ControlServer::Impl
{
    struct Client
    {
        Socket socket = kNoSocket;
        std::mutex mutex;
        std::condition_variable wake;
        std::deque<std::string> queue;
        std::size_t queued = 0;
        bool closing = false;
        std::atomic<bool> cancel{false};  // ends the reader
        std::atomic<bool> done{false};    // the reader ended: Accept joins and removes the client
        std::atomic<bool> flushed{false}; // the writer ended
        std::thread reader;
        std::thread writer;
    };

    mutable std::mutex mutex;
    Socket listener = kNoSocket;
    std::string path;
    std::thread acceptor;
    std::map<int, std::shared_ptr<Client>> clients;
    int nextId = 1;
    bool stopping = false;
    Greeting greeting;
    LineHandler onLine;

    void Accept();
    void Serve(int id, std::shared_ptr<Client> client);
    void Drop(const std::shared_ptr<Client>& client);
};

void ControlServer::Impl::Drop(const std::shared_ptr<Client>& client)
{
    std::lock_guard lock(client->mutex);
    if (!client->closing) {
        client->closing = true;
        ShutdownSocket(client->socket, true);
    }
    client->cancel = true;
    client->wake.notify_all();
}

void ControlServer::Impl::Accept()
{
    for (;;) {
        {
            std::lock_guard lock(mutex);
            if (stopping)
                return;
        }
        if (!WaitReadable(listener, 100))
            continue;
        const Socket s = ::accept(listener, nullptr, nullptr);
        if (s == kNoSocket)
            continue;
        SetNonBlocking(s);
        auto client = std::make_shared<Client>();
        client->socket = s;

        int id = 0;
        std::vector<std::shared_ptr<Client>> finished;
        {
            std::lock_guard lock(mutex);
            if (stopping) {
                CloseSocket(s);
                return;
            }
            for (auto it = clients.begin(); it != clients.end();) {
                if (it->second->done) {
                    finished.push_back(it->second);
                    it = clients.erase(it);
                } else {
                    ++it;
                }
            }
            // Queued in the same step that puts the client in the list: nothing broadcast comes before it, and
            // anything broadcast after the greeting was made reaches it
            if (greeting) {
                client->queue.push_back(greeting() + "\n");
                client->queued = client->queue.back().size();
            }
            id = nextId++;
            clients[id] = client;
        }
        for (const auto& old : finished) {
            old->reader.join();
            old->writer.join();
            CloseSocket(old->socket);
        }
        Serve(id, client);
    }
}

void ControlServer::Impl::Serve(int id, std::shared_ptr<Client> client)
{
    client->writer = std::thread([client] {
        struct MarkFlushed {
            Client& c;
            ~MarkFlushed() { c.flushed = true; }
        } markFlushed{*client};
        for (;;) {
            std::string data;
            {
                std::unique_lock lock(client->mutex);
                client->wake.wait(lock, [&client] { return client->closing || !client->queue.empty(); });
                if (client->queue.empty())
                    return;
                while (!client->queue.empty()) {
                    data += client->queue.front();
                    client->queue.pop_front();
                }
                client->queued = 0;
            }
            if (!SendAll(client->socket, data, &client->cancel))
                return;
        }
    });
    client->reader = std::thread([this, id, client] {
        std::string pending, line;
        while (ReceiveLine(client->socket, pending, line, &client->cancel))
            if (onLine)
                onLine(id, line);
        Drop(client);
        client->done = true;
    });
}

ControlServer::ControlServer()
    : m_impl(std::make_unique<Impl>())
{
}

ControlServer::~ControlServer()
{
    Stop();
}

bool ControlServer::Start(const std::string& name, Greeting greeting, LineHandler onLine, std::string& error)
{
    Stop();
    InitSockets();
    Impl& d = *m_impl;
    const std::string path = ControlSocketPath(name);
    sockaddr_un addr;
    if (!FillAddress(path, addr)) {
        error = "socket path too long: " + path;
        return false;
    }
    // A socket file with nobody behind it is left over from a process that is gone. Not asked with exists():
    // on Windows the AF_UNIX socket file is a reparse point that std::filesystem cannot stat.
    const Socket probe = ConnectTo(path);
    if (probe != kNoSocket) {
        CloseSocket(probe);
        error = "another server is listening on " + path;
        return false;
    }
    std::error_code ec;
    std::filesystem::remove(path, ec);
    const Socket s = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == kNoSocket) {
        error = "cannot create a local socket";
        return false;
    }
    if (::bind(s, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0 || ::listen(s, 8) != 0) {
        CloseSocket(s);
        error = "cannot listen on " + path;
        return false;
    }
#if !defined(_WIN32)
    ::chmod(path.c_str(), 0600);
#endif
    d.listener = s;
    d.path = path;
    d.stopping = false;
    d.greeting = std::move(greeting);
    d.onLine = std::move(onLine);
    d.acceptor = std::thread([&d] { d.Accept(); });
    return true;
}

void ControlServer::Stop()
{
    Impl& d = *m_impl;
    std::map<int, std::shared_ptr<Impl::Client>> clients;
    {
        std::lock_guard lock(d.mutex);
        if (d.listener == kNoSocket)
            return;
        d.stopping = true;
        clients.swap(d.clients);
    }
    if (d.acceptor.joinable())
        d.acceptor.join();     // it sees `stopping` within 100 ms
    CloseSocket(d.listener);
    d.listener = kNoSocket;
    for (auto& [id, client] : clients) {
        {
            // what is queued still goes out (e.g. the final status), for up to 2 s, then the connection ends
            std::unique_lock lock(client->mutex);
            client->closing = true;
            client->wake.notify_all();
        }
        for (int i = 0; i < 20 && !client->flushed; ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        client->cancel = true;
        if (client->writer.joinable())
            client->writer.join();
        ShutdownSocket(client->socket, true);
        if (client->reader.joinable())
            client->reader.join();
        CloseSocket(client->socket);
    }
    std::error_code ec;
    std::filesystem::remove(d.path, ec);
}

bool ControlServer::Running() const
{
    std::lock_guard lock(m_impl->mutex);
    return m_impl->listener != kNoSocket && !m_impl->stopping;
}

std::string ControlServer::Path() const
{
    std::lock_guard lock(m_impl->mutex);
    return m_impl->path;
}

void ControlServer::Send(int client, std::string_view line)
{
    std::shared_ptr<Impl::Client> target;
    {
        std::lock_guard lock(m_impl->mutex);
        const auto it = m_impl->clients.find(client);
        if (it == m_impl->clients.end())
            return;
        target = it->second;
    }
    bool drop = false;
    {
        std::lock_guard lock(target->mutex);
        if (target->closing)
            return;
        target->queued += line.size() + 1;
        if (target->queued > kMaxQueued) {
            drop = true;
        } else {
            target->queue.emplace_back(line);
            target->queue.back() += '\n';
            target->wake.notify_one();
        }
    }
    if (drop)
        m_impl->Drop(target);
}

void ControlServer::Broadcast(std::string_view line)
{
    std::vector<int> ids;
    {
        std::lock_guard lock(m_impl->mutex);
        for (const auto& [id, client] : m_impl->clients)
            ids.push_back(id);
    }
    for (const int id : ids)
        Send(id, line);
}

std::size_t ControlServer::Clients() const
{
    std::lock_guard lock(m_impl->mutex);
    return m_impl->clients.size();
}

//////////////////////////////////////////////////////////////////////////
// ControlClient

struct ControlClient::Impl
{
    Socket socket = kNoSocket;
    std::string pending;
};

ControlClient::ControlClient()
    : m_impl(std::make_unique<Impl>())
{
}

ControlClient::~ControlClient()
{
    Close();
}

bool ControlClient::Connect(const std::string& name, std::string& error)
{
    Close();
    InitSockets();
    const std::string path = ControlSocketPath(name);
    m_impl->socket = ConnectTo(path);
    if (m_impl->socket == kNoSocket) {
        error = "no server listening on " + path;
        return false;
    }
    return true;
}

bool ControlClient::ReadLine(std::string& line)
{
    return m_impl->socket != kNoSocket && ReceiveLine(m_impl->socket, m_impl->pending, line);
}

bool ControlClient::WriteLine(std::string_view line)
{
    if (m_impl->socket == kNoSocket)
        return false;
    std::string data(line);
    data += '\n';
    return SendAll(m_impl->socket, data);
}

void ControlClient::CloseWrite()
{
    if (m_impl->socket != kNoSocket)
        ShutdownSocket(m_impl->socket, false);
}

void ControlClient::Close()
{
    if (m_impl->socket != kNoSocket) {
        CloseSocket(m_impl->socket);
        m_impl->socket = kNoSocket;
    }
    m_impl->pending.clear();
}

} // namespace x2::platform
