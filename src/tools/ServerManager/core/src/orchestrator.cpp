#include "x2/servermanager/orchestrator.h"

#include "x2/platform/control.h"

#include <algorithm>
#include <thread>

namespace x2::servermanager {

using namespace std::chrono_literals;

const char* ToString(Phase phase)
{
    switch (phase) {
    case Phase::Waiting: return "waiting";
    case Phase::Starting: return "starting";
    case Phase::Ready: return "ready";
    case Phase::Stopping: return "stopping";
    case Phase::Failed: return "failed";
    case Phase::Offline: break;
    }
    return "offline";
}

Orchestrator::Orchestrator(Profile profile, OrchestratorOptions options)
    : m_profile(std::move(profile)), m_options(std::move(options))
{
    if (!m_options.runningServers)
        m_options.runningServers = [] { return x2::platform::ListControlSockets(); };
    m_order = m_profile.StartOrder();
    for (const ServerSpec& spec : m_profile.servers)
        m_servers.push_back(std::make_unique<Server>(spec));
}

Orchestrator::~Orchestrator()
{
    for (auto& s : m_servers)
        if (s->transport)
            s->transport->CloseInput();
    const auto deadline = Clock::now() + 5s;
    for (auto& s : m_servers)
        if (s->transport && s->owned)
            while (s->transport->Running() && Clock::now() < deadline)
                std::this_thread::sleep_for(20ms);
    m_servers.clear();      // the transports kill what still runs
}

void Orchestrator::StartAll()
{
    for (std::size_t i = 0; i < m_servers.size(); ++i)
        Start(static_cast<int>(i));
}

void Orchestrator::Start(int i)
{
    Server& s = At(static_cast<std::size_t>(i));
    if (s.phase == Phase::Ready || s.phase == Phase::Starting || s.wantRun)
        return;
    s.wantRun = true;
    s.wantStop = false;
    s.error.clear();
    if (s.phase == Phase::Failed)
        s.phase = Phase::Offline;
    for (const std::string& dep : s.spec.after)
        Start(m_profile.Find(dep));
}

void Orchestrator::StopAll()
{
    for (std::size_t i = 0; i < m_servers.size(); ++i)
        Stop(static_cast<int>(i));
}

void Orchestrator::Stop(int i)
{
    Server& s = At(static_cast<std::size_t>(i));
    s.wantRun = false;
    if (s.phase == Phase::Waiting) {
        s.phase = Phase::Offline;
        s.session.NoteEvent(LogLevel::Note, "start cancelled");
        return;
    }
    if (s.transport && !s.stopRequested)
        s.wantStop = true;
}

void Orchestrator::Attach(int i)
{
    Server& s = At(static_cast<std::size_t>(i));
    if (!s.transport)
        Connect(i, false);
}

void Orchestrator::Detach(int i)
{
    Server& s = At(static_cast<std::size_t>(i));
    if (s.transport && !s.owned) {
        s.detaching = true;
        s.transport->CloseInput();
    }
}

void Orchestrator::Kill(int i)
{
    Server& s = At(static_cast<std::size_t>(i));
    if (s.transport && s.owned) {
        s.killed = true;
        s.session.NoteEvent(LogLevel::Warn, "killed");
        s.transport->Kill();
    }
}

bool Orchestrator::Connect(int i, bool owned)
{
    Server& s = At(static_cast<std::size_t>(i));
    x2::platform::ProcessOptions options;
    if (owned) {
        options.program = s.spec.program;
        options.args = s.spec.args;
        options.args.push_back("--machine");
        if (!s.spec.control.empty())
            options.args.push_back("--control=" + s.spec.control);
        options.workingDirectory = s.spec.directory;
    } else {
        options.program = m_profile.x2ctl;
        options.args = {"attach", s.spec.ControlName()};
    }

    s.owned = owned;
    s.error.clear();
    s.exitCode.reset();
    s.stopRequested = s.detaching = s.killed = false;
    s.wantStop = false;
    s.stopStage = 0;
    s.transport = std::make_unique<ProcessTransport>(options, std::string());
    s.session.OnConnecting((owned ? "start: " : "attach: ") + s.transport->Description());

    std::string error;
    if (!s.transport->Open(m_inbox, i, ++s.generation, error)) {
        s.transport.reset();
        s.session.OnDisconnected(error);
        s.phase = Phase::Failed;
        s.error = error;
        s.wantRun = false;
        return false;
    }
    ProcessTransport* transport = s.transport.get();
    s.session.OnConnected([transport](const std::string& line) { return transport->WriteLine(line); });
    s.phase = Phase::Starting;
    return true;
}

void Orchestrator::Closed(int i, const TransportEvent& event)
{
    Server& s = At(static_cast<std::size_t>(i));
    const bool expected = s.stopRequested || s.detaching || s.killed || s.session.State() == ServerState::Stopped;
    s.session.OnDisconnected(event.text, expected);
    s.transport.reset();
    s.exitCode = event.exitCode;

    if (expected) {
        s.phase = Phase::Offline;
    } else {
        s.phase = Phase::Failed;
        s.error = s.owned ? event.text : "connection closed (" + event.text + ")";
    }
    s.wantRun = s.wantStop = s.stopRequested = s.detaching = s.killed = false;
}

bool Orchestrator::DependentsRunning(int i) const
{
    const std::string& name = At(static_cast<std::size_t>(i)).spec.name;
    for (const auto& other : m_servers) {
        if (!other->wantStop && !other->stopRequested)
            continue;   // only servers being stopped too hold this one back
        if (std::find(other->spec.after.begin(), other->spec.after.end(), name) != other->spec.after.end() &&
            other->transport)
            return true;
    }
    return false;
}

bool Orchestrator::Update(Clock::time_point now)
{
    bool changed = false;

    for (TransportEvent& event : m_inbox.Drain()) {
        if (event.server < 0 || static_cast<std::size_t>(event.server) >= m_servers.size())
            continue;
        Server& s = At(static_cast<std::size_t>(event.server));
        if (event.generation != s.generation || !s.transport)
            continue;   // from an earlier connection
        changed = true;
        if (event.kind == TransportEvent::Line)
            s.session.OnLine(event.text);
        else
            Closed(event.server, event);
    }

    // what the servers report
    for (auto& s : m_servers) {
        if (!s->transport)
            continue;
        const ServerState state = s->session.State();
        if (s->phase == Phase::Starting && s->session.GotHello() && state == ServerState::Ready) {
            s->phase = Phase::Ready;
            changed = true;
        } else if ((s->phase == Phase::Starting || s->phase == Phase::Ready) && s->session.GotHello() &&
                   (state == ServerState::Stopping || state == ServerState::Stopped)) {
            s->phase = Phase::Stopping;     // stopped from elsewhere (x2ctl stop, its own prompt)
            changed = true;
        }
    }

    // starts, in dependency order
    for (const int i : m_order) {
        Server& s = At(static_cast<std::size_t>(i));
        if (!s.wantRun || (s.phase != Phase::Offline && s.phase != Phase::Waiting))
            continue;
        bool depsReady = true;
        std::string blocked;
        for (const std::string& dep : s.spec.after) {
            const Server& d = At(static_cast<std::size_t>(m_profile.Find(dep)));
            depsReady = depsReady && d.phase == Phase::Ready;
            if (d.phase == Phase::Failed || (d.phase == Phase::Offline && !d.wantRun))
                blocked = dep;
        }
        if (!blocked.empty()) {
            s.wantRun = false;
            s.phase = Phase::Offline;
            s.session.NoteEvent(LogLevel::Warn, "not started: " + blocked + " is not running");
        } else if (depsReady) {
            const std::vector<std::string> running = m_options.runningServers();
            const bool exists = std::find(running.begin(), running.end(), s.spec.ControlName()) != running.end();
            if (exists)
                s.session.NoteEvent(LogLevel::Note, "already running: attaching instead of starting it");
            s.wantRun = false;
            Connect(i, !exists);
        } else if (s.phase != Phase::Waiting) {
            s.phase = Phase::Waiting;
        } else {
            continue;
        }
        changed = true;
    }

    // stops, in reverse dependency order
    for (auto it = m_order.rbegin(); it != m_order.rend(); ++it) {
        const int i = *it;
        Server& s = At(static_cast<std::size_t>(i));
        if (s.wantStop && !s.stopRequested && !DependentsRunning(i)) {
            s.wantStop = false;
            s.stopRequested = true;
            s.stopSince = now;
            s.stopStage = 0;
            if (!s.session.Stop() && s.owned)
                s.transport->CloseInput();
            s.phase = Phase::Stopping;
            changed = true;
        }
        if (s.stopRequested && s.owned && s.transport) {
            if (s.stopStage == 0 && now - s.stopSince >= m_options.stopGrace) {
                s.session.NoteEvent(LogLevel::Warn, "did not stop in time: terminating");
                s.transport->Terminate();
                s.stopStage = 1;
                changed = true;
            } else if (s.stopStage == 1 && now - s.stopSince >= m_options.stopGrace + m_options.killGrace) {
                s.session.NoteEvent(LogLevel::Error, "did not stop in time: killing");
                s.transport->Kill();
                s.stopStage = 2;
                changed = true;
            }
        }
    }
    return changed;
}

bool Orchestrator::Busy() const
{
    for (const auto& s : m_servers)
        if (s->phase == Phase::Waiting || s->phase == Phase::Starting || s->phase == Phase::Stopping || s->wantRun ||
            s->wantStop)
            return true;
    return false;
}

}  // namespace x2::servermanager
