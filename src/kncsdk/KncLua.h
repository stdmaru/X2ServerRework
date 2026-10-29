#pragma once

// LuaJIT (Lua 5.1 API, C linkage). The original compiled its own Lua copy as C++.
#include <lua.hpp>

#define DeclLuaValue void to_lua(lua_State *L) { lua_tinker::push_::invoke( L, this); }

//{{ 2009. 7. 24  최육사	lua함수 매크로
#define LUA_DOFILE luaL_dofile
//}}
