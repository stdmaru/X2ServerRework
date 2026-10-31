// Manual test of the server terminal (T15): the Lua prompt of the servers, with a log line every 2 s from another
// thread (it must not break the line being typed) and the title going from _Loading to _Ready after 3 s.
//   x2_terminal_demo [--no-stdin]
#include "KncLua.h"
#include "dbg/dbg.hpp"
#include "x2/platform/console.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>

int main(int argc, char* argv[])
{
    x2::platform::InitConsole();

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    std::atomic<bool> running{true};
    const auto start = std::chrono::steady_clock::now();
    std::thread ticker([&running] {
        for (int i = 0; running; ++i) {
            for (int k = 0; k < 20 && running; ++k)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (running)
                START_LOG(clog, L"tick 한글") << BUILD_LOG(i) << END_LOG;
        }
    });

    LuaPromptOptions options;
    options.title = "TerminalDemo";
    options.ready = [start] { return std::chrono::steady_clock::now() - start > std::chrono::seconds(3); };
    options.noStdin = argc > 1 && std::strcmp(argv[1], "--no-stdin") == 0;
    lua_interpreter(L, options);

    running = false;
    ticker.join();
    lua_close(L);
    return 0;
}
