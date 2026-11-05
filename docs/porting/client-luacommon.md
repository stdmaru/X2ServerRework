[Home](../README.md) › [Porting](README.md) › Client: LuaCommon

# Client: LuaCommon (`src/client/LuaCommon`)

The client's Lua binding: `KLuaManager` (script loading and table access), `KLuabinder` and its own `lua_tinker`. In the original
it is compiled by the `luajitLib` project together with LuaJIT 2.0.2; LuaJIT itself comes from upstream (T21). Overview of the
client port: [Client](client.md).

## Import (2026-11-05)

- Module `client-luacommon` ([report](../../tools/import/reports/client-luacommon/REPORT.md)): the `LuaCommon/` files of
  `luajitLib.vcxproj` (`Release|Win32`, the configuration of the solution's `Release_US`): **7 files**, 5.5k lines written.
- `LuaHeaderCommon.h` includes `../luajitLib/lua.hpp` (under `_USE_LUAJIT_`, defined in `KTDX.h`): becomes upstream LuaJIT's
  `lua.hpp`.

## Two `lua_tinker`

There are three `lua_tinker` copies in the original: `KNCSDK_Server` (1,262 lines, the base of `src/kncsdk`, frozen),
`KNCSDK_Client` (1,088) and `LuaCommon` (1,955, a 2013 rewrite). The client uses the `LuaCommon` one: `KTDX.h` (through every
`stdafx.h`) includes `../LuaCommon/lua_tinker.h` first, and `KHandle.h`'s `<lua_tinker.h>`, which in the original resolved to
`KNCSDK_Client`, is then skipped because both use the guard `_LUA_TINKER_H_`. The port keeps that: the client compiles
`src/client/LuaCommon/lua_tinker.*`, and `src/kncsdk/lua_tinker.h` stays the server's. Unifying the two is left for later
(Phase 5); until then the client must keep the `LuaCommon` header first.

## Status

Imported, not compiled yet.
