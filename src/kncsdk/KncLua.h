#pragma once

// LuaJIT (Lua 5.1 API, C linkage). The original compiled its own Lua copy as C++.
#include <lua.hpp>

#define DeclLuaValue void to_lua(lua_State *L) { lua_tinker::push_::invoke( L, this); }

//{{ 2009-07-24 Lua function macro
#define LUA_DOFILE luaL_dofile
//}}

// Interactive Lua prompt on stdin/stdout over an existing state, until end of input.
// The original came from a modified lua.c (KNCSDK lua5.1.4); the terminal UI (decision T15) will replace it.
int lua_interpreter( lua_State* L );
