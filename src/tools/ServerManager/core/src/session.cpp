#include "x2/servermanager/session.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <ctime>
#include <type_traits>

namespace x2::servermanager {

const char* ToString(LinkState state)
{
    switch (state) {
    case LinkState::Connecting: return "connecting";
    case LinkState::Connected: return "connected";
    case LinkState::Disconnected: break;
    }
    return "disconnected";
}

std::string LocalTimeNow()
{
    const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return buffer;
}

ServerSession::ServerSession(std::string name, std::size_t logCapacity)
    : m_name(std::move(name)), m_logs(logCapacity), m_clock(LocalTimeNow)
{
}

void ServerSession::Note(EntryKind kind, LogLevel level, std::string text)
{
    LogEntry entry;
    entry.kind = kind;
    entry.log.time = m_clock();
    entry.log.level = level;
    entry.log.message = std::move(text);
    m_logs.Add(std::move(entry));
}

void ServerSession::SetState(ServerState state)
{
    if (state == m_state)
        return;
    m_state = state;
    Note(EntryKind::Event, LogLevel::Note, std::string("state: ") + ToString(state));
}

void ServerSession::AddLine(std::optional<std::int64_t> seq, LogEntry entry)
{
    if (m_historyId)
        m_held.emplace_back(seq, std::move(entry));
    else
        m_logs.Add(std::move(entry));
}

void ServerSession::FlushHistory()
{
    if (!m_historyId)
        return;
    m_historyId.reset();
    // Live lines and backlog lines in seq order; a line may have come both ways. Unnumbered lines keep their place
    // at the end.
    static constexpr std::int64_t kLast = std::numeric_limits<std::int64_t>::max();
    std::stable_sort(m_held.begin(), m_held.end(),
                     [](const auto& a, const auto& b) { return a.first.value_or(kLast) < b.first.value_or(kLast); });
    m_held.erase(std::unique(m_held.begin(), m_held.end(),
                             [](const auto& a, const auto& b) { return a.first && b.first && *a.first == *b.first; }),
                 m_held.end());
    const auto earlier = std::count_if(m_held.begin(), m_held.end(),
                                       [this](const auto& held) { return held.first && *held.first <= m_hello.seq.value_or(0); });
    Note(EntryKind::Event, LogLevel::Note, "history: " + std::to_string(earlier) + " earlier lines");
    for (auto& held : m_held)
        m_logs.Add(std::move(held.second));
    m_held.clear();
}

void ServerSession::OnConnecting(std::string_view how)
{
    m_link = LinkState::Connecting;
    Note(EntryKind::Event, LogLevel::Note, "connecting: " + std::string(how));
}

void ServerSession::OnConnected(Sender sender)
{
    m_sender = std::move(sender);
    m_link = LinkState::Connected;
    m_gotHello = false;
    m_historyId.reset();
    m_held.clear();
    Note(EntryKind::Event, LogLevel::Note, "connected");
}

void ServerSession::OnLine(std::string_view line)
{
    ServerMessage message = ParseServerMessage(line);
    std::visit(
        [this](auto& m) {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, HelloMessage>) {
                m_hello = m;
                m_gotHello = true;
                if (m.protocol != kProtocolVersion)
                    Note(EntryKind::Event, LogLevel::Warn,
                         "server speaks protocol " + std::to_string(m.protocol) + ", ServerManager " +
                             std::to_string(kProtocolVersion));
                SetState(m.state);
                if (m_replayHistory && !m_historyId && m.seq && *m.seq > 0) {
                    const std::int64_t upto = *m.seq;
                    m_historyId = Send(HistoryRequest(m_nextId, upto), {}, [this](const ResultMessage&) { FlushHistory(); });
                }
            } else if constexpr (std::is_same_v<T, LogMessage>) {
                const std::optional<std::int64_t> seq = m.seq;
                LogEntry entry;
                entry.kind = EntryKind::Log;
                entry.log = std::move(m);
                AddLine(seq, std::move(entry));
            } else if constexpr (std::is_same_v<T, OutputMessage>) {
                LogEntry entry;
                entry.kind = EntryKind::Output;
                entry.log.time = m_clock();
                entry.log.level = LogLevel::Info;
                entry.log.message = std::move(m.text);
                AddLine(m.seq, std::move(entry));
            } else if constexpr (std::is_same_v<T, StatusMessage>) {
                SetState(m.state);
            } else if constexpr (std::is_same_v<T, ResultMessage>) {
                std::string text = m.output;
                if (!m.error.empty())
                    text += (text.empty() || text.back() == '\n' ? "" : "\n") + m.error;
                while (!text.empty() && text.back() == '\n')
                    text.pop_back();
                if (!text.empty() || !m.ok)
                    Note(EntryKind::Result, m.ok ? LogLevel::Info : LogLevel::Error, text.empty() ? "failed" : text);
                if (m.id) {
                    const auto it = m_pending.find(*m.id);
                    if (it != m_pending.end()) {
                        ResultHandler done = std::move(it->second);
                        m_pending.erase(it);
                        if (done)
                            done(m);
                    }
                }
            } else {
                Note(EntryKind::Output, LogLevel::Info, std::move(m.text));
            }
        },
        message);
}

void ServerSession::OnDisconnected(std::string_view reason, bool expected)
{
    FlushHistory();
    m_sender = {};
    m_link = LinkState::Disconnected;
    Note(EntryKind::Event, expected || m_state == ServerState::Stopped ? LogLevel::Note : LogLevel::Warn,
         "disconnected" + (reason.empty() ? std::string() : ": " + std::string(reason)));

    std::map<std::int64_t, ResultHandler> pending;
    pending.swap(m_pending);
    for (auto& [id, done] : pending) {
        if (done) {
            ResultMessage failed;
            failed.id = id;
            failed.error = "disconnected";
            done(failed);
        }
    }
}

std::optional<std::int64_t> ServerSession::Send(std::string line, std::string_view echo, ResultHandler done)
{
    if (m_link != LinkState::Connected || !m_sender)
        return std::nullopt;
    const std::int64_t id = m_nextId++;
    if (!echo.empty())
        Note(EntryKind::Command, LogLevel::Note, std::string(echo));
    if (!m_sender(line)) {
        Note(EntryKind::Event, LogLevel::Error, "could not send the command");
        return std::nullopt;
    }
    m_pending.emplace(id, std::move(done));
    return id;
}

std::optional<std::int64_t> ServerSession::RunLua(std::string_view lua, ResultHandler done)
{
    return Send(LuaRequest(m_nextId, lua), lua, std::move(done));
}

std::optional<std::int64_t> ServerSession::Stop(ResultHandler done)
{
    return Send(StopRequest(m_nextId), "(stop)", std::move(done));
}

}  // namespace x2::servermanager
