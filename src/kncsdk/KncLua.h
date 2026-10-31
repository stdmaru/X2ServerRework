#pragma once

// LuaJIT (Lua 5.1 API, C linkage). The original compiled its own Lua copy as C++.
#include <lua.hpp>

#define DeclLuaValue void to_lua(lua_State *L) { lua_tinker::push_::invoke( L, this); }

//{{ 2009-07-24 Lua function macro
#define LUA_DOFILE lua_dofile
//}}

// luaL_dofile, except for files past LuaJIT's limit of 65536 constants in one function (the original ran on Lua 5.1):
// those are split at their top-level statements and the pieces run in order (docs/porte/kncsdk.md).
int lua_dofile( lua_State* L, const char* filename );

// A plain Lua prompt over `L` (the server console of ServerConsole.h with no name), until the user exits or stdin ends.
int lua_interpreter( lua_State* L );
