#pragma once

// Starts, stops and follows the servers of a Profile (decision T30): a server starts once everything it depends on
// is ready, and stops once nothing being stopped still depends on it. A server whose control socket already exists
// here is attached to (x2ctl attach) instead of started a second time. Owned by the UI thread: call Update() after
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
    // Control socket names listening on this machine (x2::platform::ListControlSockets).
    std::function<std::vector<std::string>()> runningServers;
};

class Orchestrator {
public:
    using Clock = std::chrono::steady_clock;

    struct Server {
        explicit Server(ServerSpec s) : spec(std::move(s)), session(spec.name) {}

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

    void StartAll();
    void Start(int i);      // and what it depends on
    void StopAll();
    void Stop(int i);       // only this one
    void Attach(int i);     // connect to it if it is already running here; never starts it
    void Detach(int i);     // attached servers: disconnect and leave it running
    void Kill(int i);       // servers ServerManager started

    // Handles the transports' events and moves the starts and stops along. True when anything changed.
    bool Update(Clock::time_point now = Clock::now());
    bool Busy() const;      // anything waiting, starting or stopping

private:
    bool Connect(int i, bool owned);
    void Closed(int i, const TransportEvent& event);
    bool DependentsRunning(int i) const;

    Profile m_profile;
    OrchestratorOptions m_options;
    std::vector<int> m_order;
    std::vector<std::unique_ptr<Server>> m_servers;
    Inbox m_inbox;
};

}  // namespace x2::servermanager
