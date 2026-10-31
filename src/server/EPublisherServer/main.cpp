// EPublisherServer <tag>: reads config_ep_<tag>.lua, connects to the Account database and serves the
// EPublisher SOAP service. Like the other servers, stdin is a Lua prompt and its end shuts the server down.
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

    if (argc < 2) {
        std::fputs("usage: EPublisherServer <tag>   (reads config_ep_<tag>.lua)\n", stderr);
        return 1;
    }

    OdbcAccountStore store;
    EPublisherServer server(store);

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    RegisterLua(L, server);

    const std::string config = std::string("config_ep_") + argv[1] + ".lua";
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
    lua_interpreter(L);
    server.Stop();
    accepter.join();
    lua_close(L);
    return 0;
}
