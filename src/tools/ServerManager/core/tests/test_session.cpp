#include "x2/servermanager/session.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <vector>

using namespace x2::servermanager;

namespace {

struct Fixture {
    Fixture() : session("GameServer01")
    {
        session.SetClock([] { return std::string("2026-11-04 12:00:00"); });
    }
    void Connect()
    {
        session.OnConnecting("test");
        session.OnConnected([this](const std::string& line) {
            sent.push_back(line);
            return writable;
        });
    }
    const LogEntry& Last() const { return *session.Logs().Find(session.Logs().LastSeq()); }

    ServerSession session;
    std::vector<std::string> sent;
    bool writable = true;
};

}  // namespace

TEST_CASE("session: hello and status drive the state")
{
    Fixture f;
    CHECK(f.session.Link() == LinkState::Disconnected);
    f.Connect();
    CHECK(f.session.Link() == LinkState::Connected);
    CHECK_FALSE(f.session.GotHello());

    f.session.OnLine(R"({"type":"hello","protocol":1,"server":"GameServer","pid":10,"state":"loading"})");
    CHECK(f.session.GotHello());
    CHECK(f.session.ReportedName() == "GameServer");
    CHECK(f.session.Pid() == 10);
    CHECK(f.session.State() == ServerState::Loading);
    CHECK(f.Last().kind == EntryKind::Event);
    CHECK(f.Last().log.message == "state: loading");
    CHECK(f.Last().log.time == "2026-11-04 12:00:00");

    f.session.OnLine(R"({"type":"status","state":"ready"})");
    CHECK(f.session.State() == ServerState::Ready);
    f.session.OnLine(R"({"type":"status","state":"stopping"})");
    f.session.OnLine(R"({"type":"status","state":"stopped"})");
    CHECK(f.session.State() == ServerState::Stopped);
    f.session.OnDisconnected("end of file");
    CHECK(f.session.Link() == LinkState::Disconnected);
    CHECK(f.Last().log.level == LogLevel::Note);     // a stopped server going away is expected
    CHECK(f.session.State() == ServerState::Stopped);
}

TEST_CASE("session: another protocol version is noted")
{
    Fixture f;
    f.Connect();
    f.session.OnLine(R"({"type":"hello","protocol":2,"server":"X","pid":1,"state":"ready"})");
    bool warned = false;
    for (std::uint64_t s = f.session.Logs().FirstSeq(); s <= f.session.Logs().LastSeq(); ++s)
        warned = warned || f.session.Logs().Find(s)->log.level == LogLevel::Warn;
    CHECK(warned);
    CHECK(f.session.State() == ServerState::Ready);
}

TEST_CASE("session: logs, output and raw lines are stored")
{
    Fixture f;
    f.Connect();
    f.session.OnLine(R"({"type":"log","time":"2026-11-04 11:59:59","level":"error","func":"F","message":"boom","fields":[],"text":[]})");
    CHECK(f.Last().kind == EntryKind::Log);
    CHECK(f.Last().log.level == LogLevel::Error);
    CHECK(f.Last().log.time == "2026-11-04 11:59:59");    // the server's time, not ours
    f.session.OnLine(R"({"type":"output","text":"printf"})");
    CHECK(f.Last().kind == EntryKind::Output);
    f.session.OnLine("bash: x2ctl: command not found");
    CHECK(f.Last().kind == EntryKind::Output);
    CHECK(f.Last().log.message == "bash: x2ctl: command not found");
}

TEST_CASE("session: results reach their request")
{
    Fixture f;
    CHECK_FALSE(f.session.RunLua("1"));    // not connected
    f.Connect();

    std::vector<ResultMessage> results;
    const auto a = f.session.RunLua("GetLocalIP()", [&](const ResultMessage& r) { results.push_back(r); });
    const auto b = f.session.RunLua("error('x')", [&](const ResultMessage& r) { results.push_back(r); });
    REQUIRE(a);
    REQUIRE(b);
    CHECK(*a != *b);
    REQUIRE(f.sent.size() == 2);
    CHECK(f.sent[0] == R"({"id":)" + std::to_string(*a) + R"x(,"lua":"GetLocalIP()"})x");
    CHECK(f.Last().kind == EntryKind::Command);
    CHECK(f.Last().log.message == "error('x')");
    CHECK(f.session.PendingRequests() == 2);

    f.session.OnLine(R"({"type":"result","id":)" + std::to_string(*b) + R"(,"ok":false,"output":"","error":"[string]:1: x"})");
    REQUIRE(results.size() == 1);
    CHECK_FALSE(results[0].ok);
    CHECK(f.Last().kind == EntryKind::Result);
    CHECK(f.Last().log.level == LogLevel::Error);
    CHECK(f.Last().log.message == "[string]:1: x");

    f.session.OnLine(R"({"type":"result","id":)" + std::to_string(*a) + R"(,"ok":true,"output":"127.0.0.1\n","error":""})");
    REQUIRE(results.size() == 2);
    CHECK(results[1].output == "127.0.0.1\n");
    CHECK(f.Last().log.message == "127.0.0.1");
    CHECK(f.session.PendingRequests() == 0);

    // a successful command with no output adds nothing after the command line
    const auto c = f.session.RunLua("x = 1");
    const std::uint64_t last = f.session.Logs().LastSeq();
    f.session.OnLine(R"({"type":"result","id":)" + std::to_string(*c) + R"(,"ok":true,"output":"","error":""})");
    CHECK(f.session.Logs().LastSeq() == last);
}

TEST_CASE("session: disconnection fails pending requests")
{
    Fixture f;
    f.Connect();
    std::string error;
    REQUIRE(f.session.Stop([&](const ResultMessage& r) { error = r.error; }));
    CHECK(f.sent.back().find(R"("stop":true)") != std::string::npos);
    f.session.OnDisconnected("connection lost");
    CHECK(error == "disconnected");
    CHECK(f.session.PendingRequests() == 0);
    CHECK(f.Last().log.level == LogLevel::Warn);
    CHECK_FALSE(f.session.RunLua("1"));
}

TEST_CASE("session: a failed write is reported")
{
    Fixture f;
    f.Connect();
    f.writable = false;
    CHECK_FALSE(f.session.RunLua("1"));
    CHECK(f.session.PendingRequests() == 0);
    CHECK(f.Last().log.level == LogLevel::Error);
}

namespace {

std::vector<std::string> Messages(const LogStore& logs, EntryKind kind)
{
    std::vector<std::string> out;
    for (std::uint64_t seq = logs.FirstSeq(); seq <= logs.LastSeq(); ++seq)
        if (const LogEntry* entry = logs.Find(seq); entry && entry->kind == kind)
            out.push_back(entry->log.message);
    return out;
}

}  // namespace

TEST_CASE("session: an attach merges the history with the live lines")
{
    Fixture f;
    f.Connect();
    f.session.OnLine(R"({"type":"hello","protocol":1,"server":"GameServer","pid":10,"state":"ready","seq":3})");
    REQUIRE(f.sent.size() == 1);
    CHECK(f.sent[0] == R"({"id":1,"history":true,"upto":3})");
    CHECK(f.session.ReplayingHistory());
    CHECK(Messages(f.session.Logs(), EntryKind::Command).empty());   // not echoed

    // Live lines (3 came live too: sent while the greeting was made) interleaved with the backlog
    f.session.OnLine(R"({"seq":3,"type":"log","level":"info","message":"three"})");
    f.session.OnLine(R"({"seq":1,"type":"log","level":"info","message":"one"})");
    f.session.OnLine(R"({"seq":4,"type":"output","text":"four"})");
    f.session.OnLine(R"({"seq":2,"type":"log","level":"info","message":"two"})");
    f.session.OnLine(R"({"seq":3,"type":"log","level":"info","message":"three"})");
    CHECK(Messages(f.session.Logs(), EntryKind::Log).empty());
    f.session.OnLine(R"({"type":"result","id":1,"ok":true,"output":"","error":""})");
    CHECK_FALSE(f.session.ReplayingHistory());

    CHECK(Messages(f.session.Logs(), EntryKind::Log) == std::vector<std::string>{"one", "two", "three"});
    CHECK(Messages(f.session.Logs(), EntryKind::Output) == std::vector<std::string>{"four"});
    CHECK(Messages(f.session.Logs(), EntryKind::Result).empty());
    const std::vector<std::string> events = Messages(f.session.Logs(), EntryKind::Event);
    CHECK(std::find(events.begin(), events.end(), "history: 3 earlier lines") != events.end());

    f.session.OnLine(R"({"seq":5,"type":"log","level":"info","message":"five"})");
    CHECK(f.Last().log.message == "five");
}

TEST_CASE("session: no history to ask for")
{
    SUBCASE("a server that just started")
    {
        Fixture f;
        f.Connect();
        f.session.OnLine(R"({"type":"hello","protocol":1,"server":"GameServer","pid":10,"state":"loading","seq":0})");
        CHECK(f.sent.empty());
    }
    SUBCASE("a server without history")
    {
        Fixture f;
        f.Connect();
        f.session.OnLine(R"({"type":"hello","protocol":1,"server":"GameServer","pid":10,"state":"loading"})");
        CHECK(f.sent.empty());
    }
    SUBCASE("turned off")
    {
        Fixture f;
        f.session.SetReplayHistory(false);
        f.Connect();
        f.session.OnLine(R"({"type":"hello","protocol":1,"server":"GameServer","pid":10,"state":"ready","seq":9})");
        CHECK(f.sent.empty());
    }
}

TEST_CASE("session: held lines are kept when the connection drops")
{
    Fixture f;
    f.Connect();
    f.session.OnLine(R"({"type":"hello","protocol":1,"server":"GameServer","pid":10,"state":"ready","seq":2})");
    f.session.OnLine(R"({"seq":2,"type":"log","level":"info","message":"two"})");
    f.session.OnDisconnected("gone");
    CHECK_FALSE(f.session.ReplayingHistory());
    CHECK(Messages(f.session.Logs(), EntryKind::Log) == std::vector<std::string>{"two"});
    CHECK(f.Last().log.message == "disconnected: gone");
}
