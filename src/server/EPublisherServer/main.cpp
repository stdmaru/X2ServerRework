// EPublisherServer <tag> [--no-stdin]: reads config_ep_<tag>.lua, connects to the Account database and serves the
// EPublisher SOAP service. Like the other servers, the terminal is a Lua prompt (T15); --no-stdin is service mode.
#include "EPublisherServer.h"

#include "x2/platform/console.h"
#include "x2/platform/unicode.h"

#include <KncLua.h>
#include <dbg/dbg.hpp>

#include <cstdio>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
    using namespace x2::epublisher;

    x2::platform::InitConsole();

    std::string tag;
    bool noStdin = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--no-stdin")
            noStdin = true;
        else if (tag.empty())
            tag = argv[i];
    }
    if (tag.empty()) {
        std::fputs("usage: EPublisherServer <tag> [--no-stdin]   (reads config_ep_<tag>.lua)\n", stderr);
        return 1;
    }

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
    LuaPromptOptions options;
    options.title = "EPublisherServer";
    options.ready = [] { return true; };   // listening already
    options.noStdin = noStdin;
    lua_interpreter(L, options);
    server.Stop();
    accepter.join();
    lua_close(L);
    return 0;
}
