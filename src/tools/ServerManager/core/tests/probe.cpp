// x2_servermanager_probe: the orchestrator against real servers, without a GUI (manual test, docs/tools/servermanager.md).
//   x2_servermanager_probe <dist folder | profile.json> [--attach-only]
// Default: starts every server in dependency order (attaching to the ones already running), waits until all are ready,
// then stops them in reverse order. --attach-only: attaches to the running servers, checks they are ready, detaches.
// Prints each phase change with the elapsed time.

#include "x2/servermanager/orchestrator.h"

#include "x2/platform/console.h"

#include <chrono>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>

using namespace x2::servermanager;
using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    x2::platform::InitConsole();
    const std::vector<std::string> args = x2::platform::Utf8Arguments(argc, argv);
    if (args.size() < 2) {
        std::fputs("usage: x2_servermanager_probe <dist folder | profile.json> [--attach-only]\n", stderr);
        return 2;
    }
    const bool attachOnly = args.size() > 2 && args[2] == "--attach-only";

    Profile profile;
    if (std::filesystem::is_directory(args[1])) {
        profile = DefaultProfile(std::filesystem::absolute(args[1]).lexically_normal().string());
    } else {
        std::string error;
        auto loaded = LoadProfile(args[1], error);
        if (!loaded) {
            std::fprintf(stderr, "%s\n", error.c_str());
            return 2;
        }
        profile = std::move(*loaded);
    }

    Orchestrator o(profile);
    const auto start = std::chrono::steady_clock::now();
    std::vector<Phase> last(o.Size(), Phase::Offline);
    auto report = [&] {
        for (std::size_t i = 0; i < o.Size(); ++i) {
            const auto& s = o.At(i);
            if (s.phase == last[i])
                continue;
            last[i] = s.phase;
            const double t = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
            const bool connected = s.phase == Phase::Starting || s.phase == Phase::Ready || s.phase == Phase::Stopping;
            const char* how = !connected ? "" : s.owned ? "started" : "attached";
            std::printf("%7.1fs  %-17s %-9s %-8s pid=%lld %s\n", t, s.spec.name.c_str(), ToString(s.phase), how,
                        static_cast<long long>(s.session.Pid()), s.error.c_str());
            std::fflush(stdout);
        }
    };
    auto runUntil = [&](auto done, std::chrono::seconds limit) {
        const auto deadline = std::chrono::steady_clock::now() + limit;
        while (!done() && std::chrono::steady_clock::now() < deadline) {
            o.Events().Wait(100ms);
            o.Update();
            report();
        }
        return done();
    };
    auto all = [&](Phase phase) {
        return [&o, phase] {
            for (std::size_t i = 0; i < o.Size(); ++i)
                if (o.At(i).phase != phase)
                    return false;
            return true;
        };
    };
    auto settled = [&] { return !o.Busy(); };

    // what runs here and on each host (x2ctl list, over ssh for the hosts)
    o.Discover();
    runUntil(
        [&o] {
            for (std::size_t h = 0; h < o.Hosts(); ++h)
                if (o.HostAt(h).listing)
                    return false;
            return true;
        },
        30s);
    for (std::size_t h = 0; h < o.Hosts(); ++h) {
        const auto& host = o.HostAt(h);
        std::string names;
        if (host.running)
            for (const std::string& name : *host.running)
                names += " " + name;
        std::printf("host %s (%s):%s%s\n", host.spec.name.c_str(), host.spec.destination.c_str(),
                    host.running ? names.c_str() : " FAILED ", host.error.c_str());
    }
    for (std::size_t i = 0; i < o.Size(); ++i) {
        const auto listed = o.Listed(static_cast<int>(i));
        std::printf("  %-17s %s\n", o.At(i).spec.name.c_str(), !listed ? "unknown" : *listed ? "running" : "not running");
    }
    std::fflush(stdout);

    if (attachOnly) {
        for (std::size_t i = 0; i < o.Size(); ++i)
            o.Attach(static_cast<int>(i));
    } else {
        o.StartAll();
    }
    const bool ready = runUntil([&] { return all(Phase::Ready)() || !o.Busy(); }, 300s) && all(Phase::Ready)();
    runUntil(settled, 1s);
    std::printf("%s\n", ready ? "all ready" : "NOT all ready");

    if (attachOnly) {
        for (std::size_t i = 0; i < o.Size(); ++i)
            o.Detach(static_cast<int>(i));
    } else {
        o.StopAll();
    }
    auto down = [&o] {
        for (std::size_t i = 0; i < o.Size(); ++i)
            if (o.At(i).phase != Phase::Offline && o.At(i).phase != Phase::Failed)
                return false;
        return true;
    };
    const bool offline = runUntil(down, 300s);
    std::printf("%s\n", offline ? "all offline" : "NOT all offline");
    for (std::size_t i = 0; i < o.Size(); ++i) {
        const auto& s = o.At(i);
        std::printf("  %-17s errors=%llu warnings=%llu exit=%s\n", s.spec.name.c_str(),
                    static_cast<unsigned long long>(s.session.Logs().Count(LogLevel::Error)),
                    static_cast<unsigned long long>(s.session.Logs().Count(LogLevel::Warn)),
                    s.exitCode ? std::to_string(*s.exitCode).c_str() : "-");
    }
    return ready && offline ? 0 : 1;
}
