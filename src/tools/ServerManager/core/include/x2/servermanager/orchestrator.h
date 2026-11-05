#pragma once

// Starts, stops and follows the servers of a Profile (decision T30): a server starts once everything it depends on
// is ready, and stops once nothing being stopped still depends on it. A server whose control socket already exists
// here is attached to (x2ctl attach) instead of started a second time. A server on a host runs as that machine's
// service: it is attached to (ssh host x2ctl attach), after the host's start command when the profile has one, and
// stopped through the host's stop command, or the protocol without one. Owned by the UI thread: call Update() after
// Events() wakes it, and regularly for the stop timeouts.

#include "x2/servermanager/profile.h"
#include "x2/servermanager/session.h"
#include "x2/servermanager/transport.h"

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace x2::servermanager {

enum class Phase {
    Offline,    // not connected
    Waiting,    // to start, waiting for what it depends on
    Starting,   // connected, not ready yet
    Ready,
    Stopping,
    Failed,     // could not start, crashed, or the connection was lost: see Server::error
};
const char* ToString(Phase phase);

struct OrchestratorOptions {
    std::chrono::milliseconds stopGrace{60000};     // after the stop request, a server ServerManager started is terminated
    std::chrono::milliseconds killGrace{10000};     // and then killed
    std::chrono::seconds attachWait{60};            // after a remote start command, for the server to listen
    // Control socket names listening on this machine (x2::platform::ListControlSockets, in Profile::controlDir if set).
    std::function<std::vector<std::string>()> runningServers;
};

class Orchestrator {
public:
    using Clock = std::chrono::steady_clock;

    struct Server {
        explicit Server(ServerSpec s) : spec(std::move(s)), session(spec.name) {}
        bool Remote() const { return !spec.host.empty(); }

        ServerSpec spec;
        ServerSession session;
        Phase phase = Phase::Offline;
        bool owned = false;             // started by ServerManager (--machine child); false: attached through x2ctl
        std::string error;              // when Failed
        std::optional<int> exitCode;    // of the last child

    private:
        friend class Orchestrator;
        bool wantRun = false;
        bool wantStop = false;
        bool stopRequested = false;
        bool detaching = false;
        bool killed = false;
        int stopStage = 0;
        Clock::time_point stopSince;
        std::uint64_t generation = 0;
        std::unique_ptr<ProcessTransport> transport;
        // a remote start or stop command on its way (ssh host '<command>')
        enum class Task { None, Start, Stop } taskKind = Task::None;
        std::uint64_t taskGeneration = 0;
        std::unique_ptr<ProcessTransport> task;
    };

    // What `x2ctl list` reported on a host (Discover).
    struct Host {
        explicit Host(HostSpec s) : spec(std::move(s)) {}

        HostSpec spec;
        bool listing = false;                               // a query is on its way
        std::optional<std::vector<std::string>> running;    // control socket names, after a query that worked
        std::string error;                                  // of the last query (ssh or x2ctl failed)

    private:
        friend class Orchestrator;
        std::uint64_t generation = 0;
        std::vector<std::string> lines;
        std::vector<std::string> errors;
        std::unique_ptr<ProcessTransport> transport;
    };

    explicit Orchestrator(Profile profile, OrchestratorOptions options = {});
    // Servers it started get the end of stdin (they shut down) and up to 5 s before being killed; attached ones
    // are detached.
    ~Orchestrator();
    Orchestrator(const Orchestrator&) = delete;
    Orchestrator& operator=(const Orchestrator&) = delete;

    const Profile& GetProfile() const { return m_profile; }
    std::size_t Size() const { return m_servers.size(); }
    Server& At(std::size_t i) { return *m_servers[i]; }
    const Server& At(std::size_t i) const { return *m_servers[i]; }
    Inbox& Events() { return m_inbox; }
    std::size_t Hosts() const { return m_hosts.size(); }
    const Host& HostAt(std::size_t i) const { return *m_hosts[i]; }

    // Which servers are running: this machine's sockets now, each host's in the background (`x2ctl list` over ssh;
    // Update collects the answers).
    void Discover();
    // From the last Discover: nullopt when not known (never asked, or the host query failed or is on its way).
    std::optional<bool> Listed(int i) const;

    void StartAll();
    void Start(int i);      // and what it depends on
    void StopAll();
    // Only this one; one not connected but Listed is attached to, then stopped (a remote one with a stop command is
    // stopped through it, attached or not).
    void Stop(int i);
    bool HasStartCommand(int i) const;
    bool HasStopCommand(int i) const;
    void Attach(int i);     // connect to it if it is already running (here or on its host); never starts it
    void Detach(int i);     // attached servers: disconnect and leave it running
    void Kill(int i);       // servers ServerManager started

    // Handles the transports' events and moves the starts and stops along. True when anything changed.
    bool Update(Clock::time_point now = Clock::now());
    bool Busy() const;      // anything waiting, starting or stopping

private:
    bool Connect(int i, bool owned, std::chrono::seconds wait = {});
    bool RunTask(int i, Server::Task kind, const std::string& command);
    void TaskEvent(int i, TransportEvent& event);
    void Closed(int i, const TransportEvent& event);
    bool DependentsRunning(int i) const;
    void HostEvent(std::size_t h, TransportEvent& event);

    Profile m_profile;
    OrchestratorOptions m_options;
    std::vector<int> m_order;
    std::vector<std::unique_ptr<Server>> m_servers;
    std::vector<std::unique_ptr<Host>> m_hosts;      // their events come with server index -1 - host index
    // a server's start/stop command events come with server index Size() + i
    std::optional<std::vector<std::string>> m_localRunning;
    Inbox m_inbox;
};

}  // namespace x2::servermanager
