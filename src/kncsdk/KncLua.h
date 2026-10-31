#pragma once

// LuaJIT (Lua 5.1 API, C linkage). The original compiled its own Lua copy as C++.
#include <lua.hpp>

#include <functional>
#include <string>

#define DeclLuaValue void to_lua(lua_State *L) { lua_tinker::push_::invoke( L, this); }

//{{ 2009-07-24 Lua function macro
#define LUA_DOFILE lua_dofile
//}}

// luaL_dofile, except for files past LuaJIT's limit of 65536 constants in one function (the original ran on Lua 5.1):
// those are split at their top-level statements and the pieces run in order (docs/porte/kncsdk.md).
int lua_dofile( lua_State* L, const char* filename );

// The server's Lua prompt (decision T15), in place of the original's window input field (CServerUIDlg) and of
// the lua.c it was built from. On a terminal: live log above the prompt, line editing and history, status in
// the title, exit confirmed with y. Otherwise lines are read from stdin until its end (a tool driving the
// server); with noStdin it waits for a stop signal (service). Returns when the server should shut down.
namespace x2::platform { class Terminal; }

struct LuaPromptOptions
{
    x2::platform::Terminal* terminal = nullptr;     // one already open (log attached); else the prompt opens its own
    std::string             title;      // terminal title, "<title>_Loading" then "<title>_Ready"
    std::function<bool()>   ready;      // polled; true once the server is ready
    bool                    noStdin = false;
};

int lua_interpreter( lua_State* L, const LuaPromptOptions& options );
int lua_interpreter( lua_State* L );

// Sends the console log to `terminal` (above the prompt) and sets the "_Loading" title; Detach undoes it,
// before the terminal is destroyed. lua_interpreter does both for a terminal it opens itself.
void AttachTerminal( x2::platform::Terminal& terminal, const std::string& title );
void DetachTerminal( x2::platform::Terminal& terminal );
