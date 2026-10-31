#include "KncLua.h"

#include "dbg/dbg.hpp"
#include "x2/platform/terminal.h"

#include <cstdio>
#include <optional>
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

// Runs one line; returns the error message, empty on success. Results go through Lua's print (stdout).
std::string Execute(lua_State* L, const std::string& line)
{
    std::string error;
    const int base = lua_gettop(L);
    int status = Load(L, line);
    if (status == 0)
        status = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (status != 0) {
        const char* msg = lua_tostring(L, -1);
        error = msg ? msg : "(error object is not a string)";
    } else if (lua_gettop(L) > base) {
        lua_getglobal(L, "print");
        lua_insert(L, base + 1);
        if (lua_pcall(L, lua_gettop(L) - base - 1, 0, 0) != 0) {
            const char* msg = lua_tostring(L, -1);
            error = msg ? msg : "(error object is not a string)";
        }
    }
    lua_settop(L, base);
    std::fflush(stdout);
    return error;
}

} // namespace

int lua_interpreter(lua_State* L)
{
    return lua_interpreter(L, LuaPromptOptions{});
}

int lua_interpreter(lua_State* L, const LuaPromptOptions& options)
{
    bool ready = false;
    auto checkReady = [&]() {
        if (ready || !options.ready || !options.ready())
            return false;
        ready = true;
        return true;
    };

    if (options.noStdin) {
        START_LOG(cout, L"No stdin (service mode): SIGTERM, SIGINT or SIGHUP shuts the server down.");
        x2::platform::WaitForStop([&] { checkReady(); });
        return 0;
    }

    std::optional<x2::platform::Terminal> own;
    x2::platform::Terminal* open = options.terminal;
    if (!open) {
        open = &own.emplace();
        AttachTerminal(*open, options.title);
    }
    x2::platform::Terminal& terminal = *open;
    const bool interactive = terminal.Interactive();
    if (interactive)
        terminal.Write("Lua prompt: Up/Down for the history, Ctrl+C or Ctrl+D (empty line) to exit.\n");

    auto idle = [&]() {
        if (checkReady() && interactive && !options.title.empty())
            terminal.SetTitle(options.title + "_Ready");
    };

    while (const std::optional<std::string> line = terminal.ReadLine(idle)) {
        if (line->empty())
            continue;
        const std::string error = Execute(L, *line);
        if (error.empty())
            continue;
        if (interactive) {
            terminal.Write("\x1b[31m" + error + "\x1b[0m\n");
        } else {
            std::fprintf(stderr, "%s\n", error.c_str());
            std::fflush(stderr);
        }
    }

    if (own)
        DetachTerminal(*own);
    return 0;
}

void AttachTerminal(x2::platform::Terminal& terminal, const std::string& title)
{
    if (!terminal.Interactive())
        return;
    // The log goes above the prompt instead of straight to stdout
    dbg::logfile::SetConsoleWriter([&terminal](std::string_view text) { terminal.Write(text); }, true);
    if (!title.empty())
        terminal.SetTitle(title + "_Loading");
}

void DetachTerminal(x2::platform::Terminal& terminal)
{
    if (terminal.Interactive())
        dbg::logfile::SetConsoleWriter({}, false);
}
