// LuaJIT (upstream 2.1, decision T21) for running the client's data scripts: Enum.lua and the item scripts.
#pragma once

#include <lua.hpp>

#include <string>
#include <string_view>

namespace x2::lua::detail {

class LuaState
{
public:
    LuaState() : L_(luaL_newstate())
    {
        if (L_)
            luaL_openlibs(L_);
    }
    ~LuaState()
    {
        if (L_)
            lua_close(L_);
    }
    LuaState(const LuaState&) = delete;
    LuaState& operator=(const LuaState&) = delete;

    explicit operator bool() const { return L_ != nullptr; }
    operator lua_State*() const { return L_; }

private:
    lua_State* L_;
};

inline int lua_absindex(lua_State* L, int idx)
{
    return (idx > 0 || idx <= LUA_REGISTRYINDEX) ? idx : lua_gettop(L) + idx + 1;
}

// Loads and runs one chunk of source (no BOM); `error` gets Lua's message.
inline bool RunChunk(lua_State* L, std::string_view source, const char* chunkName, std::string& error)
{
    if (luaL_loadbuffer(L, source.data(), source.size(), chunkName) != 0 || lua_pcall(L, 0, 0, 0) != 0)
    {
        const char* message = lua_tostring(L, -1);
        error = message ? message : "Lua error";
        lua_pop(L, 1);
        return false;
    }
    return true;
}

// Lua 5.1 (which X2ScriptPreprocessingTool ran) reads an unknown escape such as "\R" as the plain character;
// LuaJIT rejects it. Rewrites those escapes inside string literals, as kim.py does.
std::string Lua51Escapes(std::string_view src);

} // namespace x2::lua::detail
