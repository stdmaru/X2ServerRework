// Orchestration with real children: x2_terminal_demo plays a server (a KServerConsole with --machine, ready after a
// delay), x2ctl attaches to one already running, and x2_process_helper plays a server that ignores stop.

#include "x2/servermanager/orchestrator.h"

#include "x2/platform/control.h"
#include "x2/platform/process.h"

#include <doctest/doctest.h>

#include <algorithm>
#include <chrono>
#include <thread>

using namespace x2::servermanager;
using namespace std::chrono_literals;

namespace {

std::string Unique(const char* base)
{
    return std::string(base) + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count() % 10000000);
}

ServerSpec Demo(const std::string& name, std::vector<std::string> after = {}, int readyAfterMs = 300)
{
    ServerSpec s;
    s.name = name;
    s.program = X2_TERMINAL_DEMO;
    s.args = {"--ready-after=" + std::to_string(readyAfterMs)};
    s.after = std::move(after);
    s.control = name;
    return s;
}

// Phases and the last log entries of every server, for failure messages.
std::string Dump(const Orchestrator& o)
{
    std::string text;
    for (std::size_t i = 0; i < o.Size(); ++i) {
        const auto& s = o.At(i);
        text += "\n" + s.spec.name + ": " + ToString(s.phase) + " (" + ToString(s.session.Link()) + ", " +
                ToString(s.session.State()) + ") " + s.error;
        const LogStore& logs = s.session.Logs();
        const std::uint64_t from = logs.LastSeq() > 6 ? logs.LastSeq() - 6 : 1;
        for (std::uint64_t q = std::max(from, logs.FirstSeq()); q <= logs.LastSeq(); ++q)
            text += "\n    " + logs.Find(q)->log.message;
    }
    return text;
}

// Runs Update until `done` or 20 s; `each` sees every step.
template <class Done, class Each>
bool RunUntil(Orchestrator& o, Done done, Each each)
{
    const auto deadline = std::chrono::steady_clock::now() + 20s;
    while (!done()) {
        if (std::chrono::steady_clock::now() > deadline)
            return false;
        o.Events().Wait(20ms);
        o.Update();
        each();
    }
    return true;
}

template <class Done>
bool RunUntil(Orchestrator& o, Done done)
{
    return RunUntil(o, done, [] {});
}

}  // namespace

TEST_CASE("orchestrator: start in dependency order, stop in reverse")
{
    Profile profile;
    const std::string a = Unique("SmA"), b = Unique("SmB"), c = Unique("SmC");
    profile.servers = {Demo(c, {b}), Demo(a), Demo(b, {a})};    // listed out of order
    REQUIRE(profile.Validate().empty());
    Orchestrator o(profile);
    const int ia = profile.Find(a), ib = profile.Find(b), ic = profile.Find(c);

    o.StartAll();
    bool orderKept = true;
    bool sawWaiting = false;
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(ic).phase == Phase::Ready; }, [&] {
        // nothing connects before what it depends on is ready
        if (o.At(ib).phase != Phase::Offline && o.At(ib).phase != Phase::Waiting && o.At(ia).phase != Phase::Ready)
            orderKept = false;
        if (o.At(ic).phase != Phase::Offline && o.At(ic).phase != Phase::Waiting && o.At(ib).phase != Phase::Ready)
            orderKept = false;
        sawWaiting = sawWaiting || o.At(ic).phase == Phase::Waiting;
    }), Dump(o));
    CHECK(orderKept);
    CHECK(sawWaiting);
    for (const int i : {ia, ib, ic}) {
        CHECK(o.At(i).owned);
        CHECK(o.At(i).session.ReportedName() == o.At(i).spec.name);
    }
    CHECK_FALSE(o.Busy());

    // a command through the child's stdin
    std::optional<ResultMessage> result;
    REQUIRE(o.At(ib).session.RunLua("1+2", [&](const ResultMessage& r) { result = r; }));
    REQUIRE_MESSAGE(RunUntil(o, [&] { return result.has_value(); }), Dump(o));
    CHECK(result->output == "3\n");

    o.StopAll();
    bool reverseKept = true;
    REQUIRE_MESSAGE(RunUntil(o, [&] {
        return o.At(ia).phase == Phase::Offline && o.At(ib).phase == Phase::Offline && o.At(ic).phase == Phase::Offline;
    }, [&] {
        // a server does not go down while something being stopped still depends on it
        if (o.At(ia).phase == Phase::Offline && o.At(ib).phase != Phase::Offline)
            reverseKept = false;
        if (o.At(ib).phase == Phase::Offline && o.At(ic).phase != Phase::Offline)
            reverseKept = false;
    }), Dump(o));
    CHECK(reverseKept);
    for (const int i : {ia, ib, ic}) {
        CHECK(o.At(i).exitCode == 0);
        CHECK(o.At(i).session.State() == ServerState::Stopped);
    }
}

TEST_CASE("orchestrator: a crash fails the server and cancels what waits for it")
{
    Profile profile;
    const std::string a = Unique("SmCrash"), b = Unique("SmAfterCrash");
    ServerSpec crashing = Demo(a, {}, 60000);   // never ready
    crashing.args.push_back("--exit-after=300");
    profile.servers = {crashing, Demo(b, {a})};
    Orchestrator o(profile);

    o.StartAll();
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Failed && !o.Busy(); }), Dump(o));
    CHECK(o.At(0).exitCode == 5);
    CHECK(o.At(0).error.find('5') != std::string::npos);
    CHECK(o.At(1).phase == Phase::Offline);     // not started: what it depends on failed

    o.Start(0);     // a new start clears the failure
    CHECK(o.At(0).error.empty());
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Failed && !o.Busy(); }), Dump(o));
}

TEST_CASE("orchestrator: a missing program fails at once")
{
    Profile profile;
    ServerSpec s;
    s.name = "Missing";
    s.program = "./x2-no-such-server";
    profile.servers = {s};
    Orchestrator o(profile);
    o.StartAll();
    o.Update();
    CHECK(o.At(0).phase == Phase::Failed);
    CHECK_FALSE(o.At(0).error.empty());
    CHECK_FALSE(o.Busy());
}

TEST_CASE("orchestrator: a server that ignores stop is terminated, then killed")
{
    Profile profile;
    ServerSpec s;
    s.name = "Stubborn";
    s.program = X2_PROCESS_HELPER;
    s.args = {"sleep"};         // never says hello; reads nothing; on Windows Terminate (stdin closed) is not enough
    profile.servers = {s};
    OrchestratorOptions options;
    options.stopGrace = 200ms;
    options.killGrace = 200ms;
    Orchestrator o(profile, options);

    o.StartAll();
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Starting; }), Dump(o));
    o.StopAll();
    o.Update();
    CHECK(o.At(0).phase == Phase::Stopping);
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Offline; }), Dump(o));
    CHECK(o.At(0).exitCode);
    CHECK(o.At(0).exitCode != 0);
}

#if defined(X2_X2CTL)
TEST_CASE("orchestrator: a server already running is attached, and stopped through x2ctl")
{
    const std::string name = Unique("SmService");
    x2::platform::ProcessOptions service;
    service.program = X2_TERMINAL_DEMO;
    service.args = {"--no-stdin", "--control=" + name, "--ready-after=100"};
    std::string error;
    auto running = x2::platform::Process::Start(service, error);
    REQUIRE_MESSAGE(running, error);

    Profile profile;
    profile.x2ctl = X2_X2CTL;
    profile.servers = {Demo(name)};
    Orchestrator o(profile);

    // wait for its socket, then start: it must attach rather than start a second one
    const auto deadline = std::chrono::steady_clock::now() + 10s;
    auto listening = [&] {
        const auto names = x2::platform::ListControlSockets();
        return std::find(names.begin(), names.end(), name) != names.end();
    };
    while (!listening() && std::chrono::steady_clock::now() < deadline)
        std::this_thread::sleep_for(20ms);
    REQUIRE(listening());

    o.Start(0);
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Ready; }), Dump(o));
    CHECK_FALSE(o.At(0).owned);
    CHECK(o.At(0).session.Pid() == running->Pid());

    o.Detach(0);
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Offline; }), Dump(o));
    CHECK(running->Running());      // detaching leaves it running

    o.Attach(0);
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Ready; }), Dump(o));
    o.Stop(0);
    REQUIRE_MESSAGE(RunUntil(o, [&] { return o.At(0).phase == Phase::Offline; }), Dump(o));
    const auto code = running->WaitFor(10s);
    CHECK(code == 0);
}
#endif
