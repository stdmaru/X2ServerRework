// Manual test of the server terminal (T15): the Lua prompt of the servers, with a log line every 2 s from another
// thread (it must not break the line being typed) and the title going from _Loading to _Ready after 3 s.
// Its control socket is "TerminalDemo" (x2ctl attach TerminalDemo), unless --control=<name>.
//   x2_terminal_demo [--no-stdin | --machine] [--control=<name>] [--ready-after=<ms>] [--exit-after=<ms>]
//   x2_terminal_demo --interpreter
// --ready-after changes the 3 s before ready; --exit-after ends the process abruptly with code 5 (a crash, for the
// ServerManager tests). --interpreter runs lua_interpreter (the original's Lua prompt function) instead, for
// test_lua_tinker.cpp.
#include "KncLua.h"
#include "ServerConsole.h"
#include "dbg/dbg.hpp"
#include "x2/platform/console.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <string_view>
#include <thread>

int main(int argc, char* argv[])
{
    x2::platform::InitConsole();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    if (argc > 1 && std::string_view(argv[1]) == "--interpreter") {
        lua_interpreter(L);
        lua_close(L);
        return 0;
    }

    KServerConsole::Options options;
    options.name = "TerminalDemo";
    options.control = "TerminalDemo";
    std::chrono::milliseconds readyAfter(3000);
    long exitAfter = -1;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        options.noStdin = options.noStdin || arg == "--no-stdin";
        options.machine = options.machine || arg == "--machine";
        if (arg.starts_with("--control="))
            options.control = options.name = std::string(arg.substr(10));
        else if (arg.starts_with("--ready-after="))
            readyAfter = std::chrono::milliseconds(std::atol(argv[i] + 14));
        else if (arg.starts_with("--exit-after="))
            exitAfter = std::atol(argv[i] + 13);
    }

    std::atomic<bool> running{true};
    const auto start = std::chrono::steady_clock::now();
    if (exitAfter >= 0) {
        std::thread([exitAfter] {
            std::this_thread::sleep_for(std::chrono::milliseconds(exitAfter));
            std::_Exit(5);
        }).detach();
    }
    std::thread ticker([&running] {
        for (int i = 0; running; ++i) {
            for (int k = 0; k < 20 && running; ++k)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (running)
                START_LOG(clog, L"tick 한글") << BUILD_LOG(i) << END_LOG;
        }
    });

    {
        KServerConsole console(options);
        console.Run(L, [start, readyAfter] { return std::chrono::steady_clock::now() - start >= readyAfter; });
    }

    running = false;
    ticker.join();
    lua_close(L);
    return 0;
}
