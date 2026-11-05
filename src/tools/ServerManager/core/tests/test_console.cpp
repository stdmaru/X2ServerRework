// Against a real server console (KServerConsole in service mode, in this process) over its control socket: the same
// lines ServerManager gets from `x2ctl attach` or a --machine child.

#include "x2/servermanager/session.h"

#include "KncLua.h"
#include "ServerConsole.h"
#include "dbg/dbg.hpp"
#include "x2/platform/control.h"

#include <doctest/doctest.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace x2::servermanager;
using namespace std::chrono_literals;

namespace {

// Lines from the reader thread, handed to the test thread (as the UI thread will get them).
class LineQueue {
public:
    void Push(std::optional<std::string> line)
    {
        std::lock_guard lock(m_mutex);
        m_lines.push_back(std::move(line));
        m_cv.notify_one();
    }
    // nullopt inside: end of connection; false: timed out
    bool Pop(std::optional<std::string>& line, std::chrono::milliseconds timeout)
    {
        std::unique_lock lock(m_mutex);
        if (!m_cv.wait_for(lock, timeout, [this] { return !m_lines.empty(); }))
            return false;
        line = std::move(m_lines.front());
        m_lines.pop_front();
        return true;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::deque<std::optional<std::string>> m_lines;
};

// Feeds the session until `done` holds or 10 s pass.
template <class Done>
bool PumpUntil(ServerSession& session, LineQueue& queue, Done done)
{
    const auto deadline = std::chrono::steady_clock::now() + 10s;
    while (!done()) {
        if (std::chrono::steady_clock::now() > deadline)
            return false;
        std::optional<std::string> line;
        if (!queue.Pop(line, 100ms))
            continue;
        if (line)
            session.OnLine(*line);
        else
            session.OnDisconnected("end of connection");
    }
    return true;
}

}  // namespace

TEST_CASE("console: a session follows a real server console")
{
    const std::string name = "ServerManagerTest-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count() % 1000000);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    std::atomic<bool> ready{false};
    std::thread server([&] {
        KServerConsole::Options options;
        options.name = name;
        options.control = name;
        options.noStdin = true;
        KServerConsole console(options);
        console.Run(L, [&] { return ready.load(); });
    });

    x2::platform::ControlClient client;
    std::string error;
    bool connected = false;
    for (int i = 0; i < 100 && !connected; ++i) {
        connected = client.Connect(name, error);
        if (!connected)
            std::this_thread::sleep_for(50ms);
    }
    REQUIRE_MESSAGE(connected, error);

    LineQueue queue;
    std::thread reader([&] {
        std::string line;
        while (client.ReadLine(line))
            queue.Push(line);
        queue.Push(std::nullopt);
    });

    ServerSession session(name);
    session.OnConnecting("control socket " + name);
    session.OnConnected([&](const std::string& line) { return client.WriteLine(line); });

    CHECK(PumpUntil(session, queue, [&] { return session.GotHello(); }));
    CHECK(session.ReportedName() == name);
    CHECK(session.Protocol() == kProtocolVersion);
    CHECK(session.State() == ServerState::Loading);

    ready = true;
    CHECK(PumpUntil(session, queue, [&] { return session.State() == ServerState::Ready; }));

    START_LOG(cerr, L"ServerManager test 한글") << BUILD_LOG(name) << END_LOG;
    LogFilter mine;
    mine.search = "ServerManager test";
    mine.kinds = KindBit(EntryKind::Log);
    LogView view;
    CHECK(PumpUntil(session, queue, [&] { return view.Refresh(session.Logs(), mine), !view.Seqs().empty(); }));
    if (!view.Seqs().empty()) {
        const LogEntry& entry = *session.Logs().Find(view.Seqs().front());
        CHECK(entry.log.level == LogLevel::Error);
        CHECK(entry.log.message.find("한글") != std::string::npos);
        REQUIRE(entry.log.fields.size() == 1);
        CHECK(entry.log.fields[0].value == name);
    }

    std::optional<ResultMessage> sum, failure;
    REQUIRE(session.RunLua("1+1", [&](const ResultMessage& r) { sum = r; }));
    REQUIRE(session.RunLua("error('boom')", [&](const ResultMessage& r) { failure = r; }));
    CHECK(PumpUntil(session, queue, [&] { return sum && failure; }));
    if (sum && failure) {
        CHECK(sum->ok);
        CHECK(sum->output == "2\n");
        CHECK_FALSE(failure->ok);
        CHECK(failure->error.find("boom") != std::string::npos);
    }

    // A second client attaching now gets the lines it did not see, in order and once each. (The stop request is
    // process-wide, so this test cannot run another console after this one.)
    for (int i = 1; i <= 3; ++i)
        START_LOG(cout, L"history marker " << i) << END_LOG;
    {
        x2::platform::ControlClient late;
        REQUIRE_MESSAGE(late.Connect(name, error), error);
        LineQueue lateQueue;
        std::thread lateReader([&] {
            std::string line;
            while (late.ReadLine(line))
                lateQueue.Push(line);
            lateQueue.Push(std::nullopt);
        });
        ServerSession attached(name);
        attached.OnConnected([&](const std::string& line) { return late.WriteLine(line); });
        CHECK(PumpUntil(attached, lateQueue, [&] { return attached.ReplayingHistory(); }));
        START_LOG(cout, L"history marker 4") << END_LOG;   // live, while the backlog is on its way
        CHECK(PumpUntil(attached, lateQueue, [&] { return !attached.ReplayingHistory(); }));

        LogFilter markers;
        markers.search = "history marker";
        LogView markerView;
        CHECK(PumpUntil(attached, lateQueue,
                        [&] { return markerView.Refresh(attached.Logs(), markers), markerView.Seqs().size() >= 4; }));
        std::vector<std::string> messages;
        for (const std::uint64_t seq : markerView.Seqs())
            messages.push_back(attached.Logs().Find(seq)->log.message);
        CHECK(messages == std::vector<std::string>{"history marker 1", "history marker 2", "history marker 3",
                                                   "history marker 4"});
        // and what came before, the earlier test error included
        LogView errors;
        errors.Refresh(attached.Logs(), mine);
        CHECK(errors.Seqs().size() == 1);

        late.CloseWrite();
        late.Close();
        lateReader.join();
    }

    REQUIRE(session.Stop());
    CHECK(PumpUntil(session, queue, [&] { return session.Link() == LinkState::Disconnected; }));
    CHECK(session.State() == ServerState::Stopped);

    server.join();
    reader.join();
    client.Close();
    lua_close(L);
}
