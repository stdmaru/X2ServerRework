// EPublisherServer <tag>: reads config_ep_<tag>.lua, connects to the Account database and serves the EPublisher
// SOAP service. The console and its options are the other servers' (KServerConsole, T15/T30).
#include "EPublisherServer.h"

#include "x2/platform/console.h"
#include "x2/platform/unicode.h"

#include <KncLua.h>
#include <ServerConsole.h>
#include <dbg/dbg.hpp>

#include <cstdio>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
    using namespace x2::epublisher;

    x2::platform::InitConsole();

    std::string tag;
    KServerConsole::Options consoleOptions;
    consoleOptions.name = "EPublisherServer";
    consoleOptions.control = consoleOptions.name;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--no-stdin")
            consoleOptions.noStdin = true;
        else if (arg == "--machine")
            consoleOptions.machine = true;
        else if (arg == "--no-control")
            consoleOptions.control.clear();
        else if (arg.rfind("--control=", 0) == 0)
            consoleOptions.control = arg.substr(10);
        else if (tag.empty())
            tag = arg;
    }
    if (tag.empty()) {
        std::fputs("usage: EPublisherServer <tag> [--no-stdin | --machine] [--control=<name> | --no-control]\n"
                   "       (reads config_ep_<tag>.lua)\n", stderr);
        return 1;
    }
    KServerConsole console(consoleOptions);

    OdbcAccountStore store;
    EPublisherServer server(store);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    RegisterLua(L, server);

    const std::string config = "config_ep_" + tag + ".lua";
    if (LUA_DOFILE(L, config.c_str()) != 0) {
        const char* error = lua_tostring(L, -1);
        START_LOG(cerr, L"Cannot load the configuration.")
            << BUILD_LOG(x2::platform::Utf8ToWide(error ? error : config))
            << END_LOG;
        lua_close(L);
        return 1;
    }

    if (!store.Connect(x2::platform::Utf8ToWide(server.GetDSN()))) {
        START_LOG(cerr, L"Cannot connect to the Account database.")
            << BUILD_LOG(x2::platform::Utf8ToWide(server.GetDSN()))
            << END_LOG;
        lua_close(L);
        return 1;
    }
    if (!server.Listen()) {
        lua_close(L);
        return 1;
    }

    std::thread accepter([&server] { server.Run(); });
    console.Run(L, [] { return true; });   // listening already
    server.Stop();
    accepter.join();
    lua_close(L);
    return 0;
}
