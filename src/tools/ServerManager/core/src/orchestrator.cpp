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
        m_options.runningServers = [dir = m_profile.controlDir] {
            return dir.empty() ? x2::platform::ListControlSockets() : x2::platform::ListControlSockets(dir);
        };
    m_order = m_profile.StartOrder();
    for (const ServerSpec& spec : m_profile.servers)
        m_servers.push_back(std::make_unique<Server>(spec));
    for (const HostSpec& spec : m_profile.hosts)
        m_hosts.push_back(std::make_unique<Host>(spec));
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
    m_hosts.clear();
}

void Orchestrator::Discover()
{
    m_localRunning = m_options.runningServers();
    for (std::size_t h = 0; h < m_hosts.size(); ++h) {
        Host& host = *m_hosts[h];
        if (host.listing)
            continue;
        host.lines.clear();
        host.errors.clear();
        host.transport = std::make_unique<ProcessTransport>(m_profile.X2ctl(host.spec.name, {"list"}), std::string());
        std::string error;
        if (host.transport->Open(m_inbox, -1 - static_cast<int>(h), ++host.generation, error)) {
            host.listing = true;
        } else {
            host.transport.reset();
            host.running.reset();
            host.error = error;
        }
    }
}

std::optional<bool> Orchestrator::Listed(int i) const
{
    const Server& s = At(static_cast<std::size_t>(i));
    const std::vector<std::string>* names = nullptr;
    if (!s.Remote()) {
        if (m_localRunning)
            names = &*m_localRunning;
    } else {
        for (const auto& host : m_hosts)
            if (host->spec.name == s.spec.host && host->running && !host->listing)
                names = &*host->running;
    }
    if (!names)
        return std::nullopt;
    return std::find(names->begin(), names->end(), s.spec.ControlName()) != names->end();
}

void Orchestrator::HostEvent(std::size_t h, TransportEvent& event)
{
    Host& host = *m_hosts[h];
    if (event.generation != host.generation || !host.transport)
        return;
    if (event.kind == TransportEvent::Line) {
        (event.error ? host.errors : host.lines).push_back(std::move(event.text));
        return;
    }
    host.transport.reset();
    host.listing = false;
    if (event.exitCode == 0) {
        host.running = std::move(host.lines);
        host.error.clear();
    } else {
        host.running.reset();
        host.error = event.text;
        for (const std::string& line : host.errors)
            host.error += "\n" + line;
    }
    host.lines.clear();
    host.errors.clear();
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

bool Orchestrator::HasStartCommand(int i) const
{
    return !m_profile.StartCommand(At(static_cast<std::size_t>(i)).spec).empty();
}

bool Orchestrator::HasStopCommand(int i) const
{
    return !m_profile.StopCommand(At(static_cast<std::size_t>(i)).spec).empty();
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
    if (s.task && s.taskKind == Server::Task::Start) {
        s.task.reset();         // ssh is killed; what the command did there so far stays done
        s.taskKind = Server::Task::None;
        s.session.NoteEvent(LogLevel::Warn, "start command cancelled");
        if (!s.transport)
            s.phase = Phase::Offline;
    }
    const bool command = HasStopCommand(i);
    if (!s.transport && Listed(i) == true)
        Connect(i, false);      // to stop it through the protocol, and see it go
    if ((s.transport || command) && !s.stopRequested && !s.task)
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

bool Orchestrator::RunTask(int i, Server::Task kind, const std::string& command)
{
    Server& s = At(static_cast<std::size_t>(i));
    s.task = std::make_unique<ProcessTransport>(m_profile.Shell(s.spec.host, command), std::string());
    s.taskKind = kind;
    s.session.NoteEvent(LogLevel::Note, (kind == Server::Task::Start ? "start on " : "stop on ") + s.spec.host + ": " +
                                            command);
    std::string error;
    if (!s.task->Open(m_inbox, static_cast<int>(m_servers.size()) + i, ++s.taskGeneration, error)) {
        s.task.reset();
        s.taskKind = Server::Task::None;
        s.session.NoteEvent(LogLevel::Error, error);
        s.error = error;
        if (!s.transport)
            s.phase = Phase::Failed;
        return false;
    }
    s.task->CloseInput();
    return true;
}

void Orchestrator::TaskEvent(int i, TransportEvent& event)
{
    Server& s = At(static_cast<std::size_t>(i));
    if (event.generation != s.taskGeneration || !s.task)
        return;
    if (event.kind == TransportEvent::Line) {
        s.session.NoteEvent(LogLevel::Note, std::move(event.text));    // docker and systemctl report on stderr
        return;
    }
    const Server::Task kind = s.taskKind;
    s.task.reset();
    s.taskKind = Server::Task::None;
    const bool ok = event.exitCode == 0;
    if (kind == Server::Task::Start) {
        if (ok) {
            Connect(i, false, m_options.attachWait);
        } else {
            s.phase = Phase::Failed;
            s.error = "the start command failed (" + event.text + "): see the log";
            s.session.NoteEvent(LogLevel::Error, s.error);
        }
        return;
    }
    if (!ok) {
        s.session.NoteEvent(LogLevel::Error, "the stop command failed (" + event.text + ")");
        s.stopRequested = false;
        if (s.transport)
            s.phase = s.session.State() == ServerState::Ready ? Phase::Ready : Phase::Starting;
        else
            s.phase = Phase::Offline;
    } else if (!s.transport) {
        s.phase = Phase::Offline;
        s.stopRequested = false;
        s.session.NoteEvent(LogLevel::Note, "stopped");
    }
    // stopped and still attached: the connection closes as the server goes
}

bool Orchestrator::Connect(int i, bool owned, std::chrono::seconds wait)
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
        options = wait.count() > 0
                      ? m_profile.X2ctl(s.spec.host, {"attach", "--wait=" + std::to_string(wait.count()), s.spec.ControlName()})
                      : m_profile.X2ctl(s.spec.host, {"attach", s.spec.ControlName()});
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
        s.phase = s.task ? Phase::Stopping : Phase::Offline;    // its stop command still finishing
    } else {
        s.phase = Phase::Failed;
        if (s.owned)
            s.error = event.text;
        else if (!s.session.GotHello())
            s.error = (s.Remote() ? "could not attach on " + s.spec.host : std::string("could not attach")) + " (" +
                      event.text + "): see the log";
        else
            s.error = "connection closed (" + event.text + ")";
    }
    s.wantRun = s.wantStop = s.detaching = s.killed = false;
    if (!s.task)
        s.stopRequested = false;
}

bool Orchestrator::DependentsRunning(int i) const
{
    const std::string& name = At(static_cast<std::size_t>(i)).spec.name;
    for (const auto& other : m_servers) {
        if (!other->wantStop && !other->stopRequested)
            continue;   // only servers being stopped too hold this one back
        if (std::find(other->spec.after.begin(), other->spec.after.end(), name) != other->spec.after.end() &&
            (other->transport || other->task))
            return true;
    }
    return false;
}

bool Orchestrator::Watch(long long pid)
{
    if (pid <= 0)
        return false;
    bool ours = false;
    for (const auto& s : m_servers)
        ours = ours || (s->transport && s->transport->Pid() == pid) || (s->task && s->task->Pid() == pid);
    for (const auto& host : m_hosts)
        ours = ours || (host->transport && host->transport->Pid() == pid);
    if (ours)
        m_watched.insert_or_assign(pid, ProcessStatus{});
    return ours;
}

void Orchestrator::Unwatch(long long pid)
{
    m_watched.erase(pid);
}

std::optional<Orchestrator::ProcessStatus> Orchestrator::Watched(long long pid) const
{
    const auto it = m_watched.find(pid);
    if (it == m_watched.end())
        return std::nullopt;
    return it->second;
}

bool Orchestrator::Update(Clock::time_point now)
{
    bool changed = false;

    for (TransportEvent& event : m_inbox.Drain()) {
        if (const auto watched = m_watched.find(event.pid); watched != m_watched.end()) {
            ProcessStatus& status = watched->second;
            if (event.kind == TransportEvent::Closed) {
                status.closed = true;
                status.exitCode = event.exitCode;
            } else if (event.error) {
                if (!event.text.empty())
                    status.lastError = event.text;
            } else {
                status.output = true;
            }
            changed = true;
        }
        if (event.server < 0) {
            const auto h = static_cast<std::size_t>(-1 - event.server);
            if (h < m_hosts.size()) {
                HostEvent(h, event);
                changed = true;
            }
            continue;
        }
        if (static_cast<std::size_t>(event.server) >= m_servers.size()) {
            const auto i = static_cast<std::size_t>(event.server) - m_servers.size();
            if (i < m_servers.size()) {
                TaskEvent(static_cast<int>(i), event);
                changed = true;
            }
            continue;
        }
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
        } else if (depsReady && s.Remote()) {
            s.wantRun = false;      // a remote server is that machine's service: start it there, then attach
            const std::string command = m_profile.StartCommand(s.spec);
            if (command.empty())
                Connect(i, false);
            else if (RunTask(i, Server::Task::Start, command))
                s.phase = Phase::Starting;
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
            changed = true;
            if (const std::string command = m_profile.StopCommand(s.spec); !command.empty()) {
                if (!RunTask(i, Server::Task::Stop, command)) {
                    s.stopRequested = false;
                    continue;
                }
            } else if (!s.session.Stop() && s.owned) {
                s.transport->CloseInput();
            }
            s.phase = Phase::Stopping;
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
            s->wantStop || s->task)
            return true;
    return false;
}

}  // namespace x2::servermanager
