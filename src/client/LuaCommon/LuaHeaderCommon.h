#pragma once



#include <lua.hpp>  // upstream LuaJIT (decision T21)

// Lua 5.0 reference API: lauxlib.h of Lua 5.1 and KOG's LuaJIT 2.0.2 kept it as macros, upstream LuaJIT does not.
#ifndef lua_getref
#define lua_ref(L, lock) ((lock) ? luaL_ref((L), LUA_REGISTRYINDEX) : \
    (lua_pushstring((L), "unlocked references are obsolete"), lua_error(L), 0))
#define lua_unref(L, ref) luaL_unref((L), LUA_REGISTRYINDEX, (ref))
#define lua_getref(L, ref) lua_rawgeti((L), LUA_REGISTRYINDEX, (ref))
#endif
