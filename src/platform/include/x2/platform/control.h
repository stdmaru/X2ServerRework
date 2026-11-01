// Local control channel of the servers (decision T30): a Unix domain socket (AF_UNIX; Windows 10 1803+ has it too),
// in a folder only the user can reach, that x2ctl connects to stdin/stdout (`ssh host x2ctl attach GameServer`).
// The messages are lines (the JSON lines of the control protocol, docs/porting/kncsdk.md).
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace x2::platform {

// $XDG_RUNTIME_DIR/x2, else /tmp/x2-<uid> (Linux); %TEMP%\x2 (Windows). Created on demand, private to the user.
std::string ControlSocketDir();
// <dir>/<name>.sock
std::string ControlSocketPath(const std::string& name);
// Names of the sockets in ControlSocketDir() that a server is listening on.
std::vector<std::string> ListControlSockets();

class ControlServer
{
public:
    using Greeting = std::function<std::string()>;                           // first line of every client
    using LineHandler = std::function<void(int client, std::string line)>;   // from the client's reader thread

    ControlServer();
    ~ControlServer();   // Stop
    ControlServer(const ControlServer&) = delete;
    ControlServer& operator=(const ControlServer&) = delete;

    // Listens on ControlSocketPath(name). A socket left by a process that is gone is replaced; one with a
    // server behind it is an error (another instance: choose another name).
    bool Start(const std::string& name, Greeting greeting, LineHandler onLine, std::string& error);
    void Stop();
    bool Running() const;
    std::string Path() const;

    // Thread-safe and never blocks: each client has a send queue and a writer thread; a client that falls too far
    // behind (more than a few MB queued) is disconnected.
    void Send(int client, std::string_view line);   // the newline is added
    void Broadcast(std::string_view line);
    std::size_t Clients() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

// The x2ctl side.
class ControlClient
{
public:
    ControlClient();
    ~ControlClient();
    ControlClient(const ControlClient&) = delete;
    ControlClient& operator=(const ControlClient&) = delete;

    bool Connect(const std::string& name, std::string& error);
    // Blocks for the next line (without the newline); false when the server closed the connection.
    bool ReadLine(std::string& line);
    bool WriteLine(std::string_view line);
    // Ends the sending side: the server sees the end of input.
    void CloseWrite();
    void Close();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace x2::platform
