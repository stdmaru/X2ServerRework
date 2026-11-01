[Home](../README.md) › [Original code](README.md) › Libraries — what to do with each

# Libraries — what to do with each

Action legend: **UP** = replace with upstream version via CMake · **PORT** = port custom code ·
**STD** = replace with the C++ standard library · **RM** = remove · **NEW** = replace with another library.

## KNCSDK (custom code — what actually needs porting)

`KNCSDK_Server` and `KNCSDK_Client` are the same SDK with divergences (36 files: `dbg/*`, `SecureBuffer`,
`Ser{Map,Set,Vector}.h`, `KCSLockType.h`, `lua_tinker.h`, `ToString.h`, `KncUtil.h`...). Unify into one.

| Sub-library (vcxproj) | Contents | Action |
|---|---|---|
| `KncDebug71` | `Include/dbg/{assert,logfile,profiler}` — logging used everywhere (`START_LOG`, `dbg::cerr`...) | **PORT** (likely rewrite on `fmt`/`spdlog`, retaining macros) |
| `KncSerialize71` | `Include/Serializer/*` + embedded zlib (`ZLIB_WINAPI`) | **PORT** Serializer (critical for protocol; see [Protocol](protocol.md)); zlib **UP** |
| `KncSecurity71` | Crypto++ **8.9.0** + `Include/KncSecurity/*` (`SecureBuffer`) | Crypto++ **RM** (T27); `KncSecurity` **PORT** |
| `KncLua71` | Lua 5.0.2 **and** 5.1.4 (612 `ExcludedFromBuild`), luabind, `lua_tinker.cpp` | **RM** embedded Lua versions → use one LuaJIT; **RM** luabind (2 server files) |
| loose headers | `Thread/`, `ObjectPool/`, `KNCSingleton.h`, `RTTI.h`, `ToString.h`, `KncUtil*.h`, `VariableSizeBuffer.h`, `vld.h` | **PORT**/**STD**; **RM** `vld.h` (Visual Leak Detector) |
| `UnitTest/` | Old crypto/lua/serializer tests | Use as test-design references |

## Lua

All copies expose the **Lua 5.1 API**:

| Copy | Used by | Action |
|---|---|---|
| `KNCSDK_*/Src/lua5.1.4`, `lua5.0.2` | server (through KncLua) | **RM** |
| `luaLib/` (5.1.4 + `KLuabinder`, `KLuaManager`, `lua_tinker`) | client (debug) / tools | **RM** (retain only useful wrappers) |
| `luajitLib/LuaJIT-2.0.2` | client | **UP** → LuaJIT 2.1 (x64, ARM64) for client **and** server |
| `LuaCommon/` | client | make `lua_tinker` + `KLuaManager` the single source → **PORT** |

`lua_tinker` is the C++↔Lua bridge: ~815 `class_def`, ~241 `decl`, ~148 `class_add` in the server alone; 73 client files.
It relies on old MSVC templates — expect work to compile with GCC/Clang.

### KOG's LuaJIT

Verified on 2026-10-30. The `.lua` files in the client's `data036.kom` (2014) are **LuaJIT 2.0 bytecode** without symbols, with a
modified header (`\x1bKL\x81` instead of `\x1bLJ\x01`); the rest of the format is 2.0.2. Version 2.1 cannot load this bytecode.
Decision: use upstream LuaJIT 2.1 and recompile scripts (or use source) in the client (T21, in [Decisions](../project/decisions.md)).

The `luajitLib/LuaJIT-2.0.2` copy is official 2.0.2 with the following changes (compared with upstream commit `RELEASE LuaJIT-2.0.2`):

| Change | Where | Purpose |
|---|---|---|
| `LUAJIT_MODIFIED_BYTECODE_HEADER` (enabled in `luaconf.h`): header `\x1bKL`, version `0x81` | `lj_bcdump.h`, `lj_lex.c` | bytecode rejected by official LuaJIT |
| `lua_setencoding`/`lua_getencoding`, `luaL_loadfilex(..., iEncoding)`: source file needs UTF-8 BOM (or must be bytecode); `dofile_UTF8`; `print` converting UTF-8 → ANSI code page | `lua.h`, `lj_obj.h`, `lj_state.c`, `lj_load.c`, `lib_base.c` | UTF-8 scripts on a Korean Windows system |
| `X2OPTIMIZE_REMOVE_LUA_INTERPRETER_MODULE` (enabled in the client's `KTDX.h`): parser and `string.dump` removed; bytecode only | `lj_parse.c`, `lj_lex.c`, `lj_bcwrite.c`, `lj_load.c`, `lib_string.c` | smaller binary; compiled scripts only |
| casts in `lj_ccallback.c` | | MSVC warnings |

The server did not use this copy (it compiled Lua 5.1.4 from `KNCSDK_Server`), and the port uses none of these extensions:
LuaJIT 2.1 already skips a UTF-8 BOM in scripts.

## Third-party libraries

| Library | Version in reference | Used by | Action |
|---|---|---|---|
| Boost | 1.91 (headers + precompiled `serialization`) | server (229 files), client (~90) | **UP**, minimize; much of `boost::shared_ptr`, `bind`, `thread`, `timer`, `format` → **STD** |
| Intel TBB | 2.2 (2009) | GameServer, 2 files | **STD** (or oneTBB) |
| DXSDK (Jun 2010) | d3d9, d3dx9, dinput8, dsound, dxerr9 | client; server math only | **RM** → see [Client](client.md) |
| FreeType | 2.4.11 (`freetype2411.lib`) | KTDXLIB fonts | **UP** |
| libxml (`libxml/`) | old libxml2 | KTDXLIB | **UP** libxml2 — or assess whether tinyxml (already in `KTDXLIB/tinyxml` and `Common/TinyXml`) is sufficient |
| CxImage + jpeg + png | old | KTDXLIB (21 files) | **NEW** → `stb_image`/`libpng` |
| zlib | embedded in several places (`KNCSDK`, `KTDXLIB/zlibstat.lib`) | `.kom`, serializer | **UP** |
| tinyxml | 2 copies | server/client | **UP** tinyxml2 or unify |
| gSOAP (`Common/SOAP`) | generated | server, publisher auth | **RM** (see proprietary binaries). The DEB copy (`AuthAndBilling/DEB`) was added then removed in T31 (custom client) |
| json (`json_vc71_libmt.lib`) | old binary jsoncpp | client/server | **NEW** → nlohmann/json or upstream jsoncpp |
| BasicExcel (`X2Lib`) | — | debug export | likely **RM** |

## Smaller custom libraries

| Library | Action |
|---|---|
| `X2ServerProtocol/` | Merge with `KncWX2Server/Common/Socket` + `Actor*`/`Event*` into a shared networking library |
| `Profiler/` | **RM** or replace with Tracy |
| `KTDXLIB/tinyxml`, `DXUT/` | **RM** DXUT (Microsoft sample framework) |
