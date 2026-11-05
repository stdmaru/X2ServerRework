#pragma once

// One server as ServerManager sees it, independent of how it is reached (child process with --machine, local control
// socket, or `ssh host x2ctl attach`): the transport passes its events and lines here, and the session keeps the
// state, the log and the pending requests. Not thread-safe: transports read on their own threads and hand the lines
// to the thread that owns the session (the UI thread).

#include "x2/servermanager/log_store.h"
#include "x2/servermanager/protocol.h"

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace x2::servermanager {

enum class LinkState { Disconnected, Connecting, Connected };
const char* ToString(LinkState state);

class ServerSession {
public:
    using Sender = std::function<bool(const std::string& line)>;   // writes one line (the newline is the transport's)
    using ResultHandler = std::function<void(const ResultMessage&)>;
    using Clock = std::function<std::string()>;                    // YYYY-MM-DD hh:mm:ss, for entries made here

    explicit ServerSession(std::string name, std::size_t logCapacity = 100000);

    void SetClock(Clock clock) { m_clock = std::move(clock); }
    // On by default: when a hello says the server already sent lines (an attach to a running server), ask for its
    // backlog. Until it arrives, log and output lines are held and then added in seq order with the backlog.
    void SetReplayHistory(bool replay) { m_replayHistory = replay; }

    // Transport events.
    void OnConnecting(std::string_view how);                // e.g. "ssh host x2ctl attach GameServer"
    void OnConnected(Sender sender);
    void OnLine(std::string_view line);
    // Pending requests fail with "disconnected". Logged as a warning unless `expected` (a detach, a stop that was
    // asked for) or the server had reported that it stopped.
    void OnDisconnected(std::string_view reason, bool expected = false);

    // Requests. The command is echoed to the log, and its result is logged when it arrives (then `done` is called).
    // nullopt when not connected or the line could not be written.
    std::optional<std::int64_t> RunLua(std::string_view lua, ResultHandler done = {});
    std::optional<std::int64_t> Stop(ResultHandler done = {});

    // An event noted by ServerManager itself (orchestration), in the log as EntryKind::Event.
    void NoteEvent(LogLevel level, std::string text) { Note(EntryKind::Event, level, std::move(text)); }

    const std::string& Name() const { return m_name; }                 // as configured in ServerManager
    const std::string& ReportedName() const { return m_hello.server; } // from hello
    std::int64_t Pid() const { return m_hello.pid; }
    int Protocol() const { return m_hello.protocol; }
    bool GotHello() const { return m_gotHello; }
    LinkState Link() const { return m_link; }
    ServerState State() const { return m_state; }
    std::size_t PendingRequests() const { return m_pending.size(); }
    bool ReplayingHistory() const { return m_historyId.has_value(); }

    LogStore& Logs() { return m_logs; }
    const LogStore& Logs() const { return m_logs; }

private:
    std::optional<std::int64_t> Send(std::string line, std::string_view echo, ResultHandler done);
    void Note(EntryKind kind, LogLevel level, std::string text);
    void SetState(ServerState state);
    void AddLine(std::optional<std::int64_t> seq, LogEntry entry);
    void FlushHistory();

    std::string m_name;
    LogStore m_logs;
    Clock m_clock;
    Sender m_sender;
    LinkState m_link = LinkState::Disconnected;
    ServerState m_state = ServerState::Unknown;
    HelloMessage m_hello;
    bool m_gotHello = false;
    std::int64_t m_nextId = 1;
    std::map<std::int64_t, ResultHandler> m_pending;
    bool m_replayHistory = true;
    std::optional<std::int64_t> m_historyId;
    std::vector<std::pair<std::optional<std::int64_t>, LogEntry>> m_held;   // while the history is on its way
};

// Local time as the protocol writes it.
std::string LocalTimeNow();

}  // namespace x2::servermanager
