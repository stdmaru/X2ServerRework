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

## Port (2026-11-05, `client/skeleton`)

Target `x2::client_luacommon`, compiled warning-free on Linux and Windows against upstream LuaJIT 2.1 (T21). Changes:

- **Encoding**: KOG's LuaJIT had `lua_getencoding`/`lua_setencoding` (legacy CP949 vs. UTF-8 scripts); upstream has neither.
  `KLuabinder`/`KLuaManager` keep only the UTF-8 branch the client used: a buffer is LuaJIT bytecode (`\033`) or text that must
  start with a UTF-8 BOM, as the client's `.kom` scripts do. `_controlfp_s` (x87 precision) removed.
- `LuaHeaderCommon.h` includes `<lua.hpp>` and defines the Lua 5.0 `lua_ref`/`lua_unref`/`lua_getref` compatibility macros
  KOG's LuaJIT still had, on the registry (`luaL_ref`).
- `KLuaManager.h` redefines `LUA_OK` as `true` (its own meaning); the upstream `LUA_OK` is undefined first.
- `lua_tinker.h` made standard C++: `typename`/`template` where required, `struct user` declared before use, `__int64` →
  `long long` (`%lld`), no `static` on explicit specializations. The `functor` family was regenerated with
  `if constexpr (std::is_void_v<RVal>)` instead of `void` specializations. **Original bug fixed**: the 8-argument `functor`
  read arguments 6–8 with `read<T5>`; `push_functor` also needed a 10-argument primary.
- Smaller: `StringCchPrintfA` → `snprintf`, `_malloca` → `std::vector`, constructor init order, unused variables and parameters.

Test: `src/client/tests/test_luacommon.cpp` ([tests](client-dxut.md#tests)).

## Status

Compiles and is tested (2026-11-05); unifying with the server `lua_tinker` is left for Phase 5.
