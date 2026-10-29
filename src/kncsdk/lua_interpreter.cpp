#include "KncLua.h"

#include <cstdio>
#include <iostream>
#include <string>

namespace {

// Like lua.c: an expression line prints its values ("1+1" -> 2), anything else runs as a statement.
int Load(lua_State* L, const std::string& line)
{
    const std::string expr = "return " + line;
    if (luaL_loadbuffer(L, expr.data(), expr.size(), "=stdin") == 0)
        return 0;
    lua_pop(L, 1);
    return luaL_loadbuffer(L, line.data(), line.size(), "=stdin");
}

} // namespace

int lua_interpreter(lua_State* L)
{
    std::fputs("\n\t Starting Lua Interpreter : ( end of input ) To Terminate. \n\n", stdout);

    std::string line;
    for (;;) {
        std::fputs("> ", stdout);
        std::fflush(stdout);
        if (!std::getline(std::cin, line))
            break;
        if (line.empty())
            continue;

        const int base = lua_gettop(L);
        int status = Load(L, line);
        if (status == 0)
            status = lua_pcall(L, 0, LUA_MULTRET, 0);
        if (status != 0) {
            const char* msg = lua_tostring(L, -1);
            std::fprintf(stderr, "%s\n", msg ? msg : "(error object is not a string)");
            lua_settop(L, base);
            continue;
        }
        const int results = lua_gettop(L) - base;
        if (results > 0) {
            lua_getglobal(L, "print");
            lua_insert(L, base + 1);
            if (lua_pcall(L, results, 0, 0) != 0) {
                std::fprintf(stderr, "%s\n", lua_tostring(L, -1));
            }
        }
        lua_settop(L, base);
    }
    std::fputs("\n", stdout);
    return 0;
}
