[Home](../README.md) › [Porting](README.md) › Client

# Client (`src/client`)

Overview of the client port: layers, how they were imported, the code shared with the servers, and what the next step
(`client/skeleton`) has to deal with first. Module details are on each layer's page. Analysis of the original's Windows and
Direct3D 9 dependencies: [Client — Windows / Direct3D 9 dependencies](../original/client.md).

| Layer | Original | `src/` | Files | Page |
|---|---|---|---:|---|
| Executable (`WinMain`) | `X2` | `src/client/X2` | 5 | [Client: X2](client-x2.md) |
| Game | `X2Lib` | `src/client/X2Lib` | 552 | [Client: X2Lib](client-x2lib.md) |
| Engine | `KTDXLIB` | `src/client/KTDXLIB` | 253 | [Client: KTDXLIB](client-ktdxlib.md) |
| Network library | `X2ServerProtocol` | `src/client/X2ServerProtocol` | 37 | [Client: X2ServerProtocol](client-serverprotocol.md) |
| Lua binding | `LuaCommon` (built by `luajitLib`) | `src/client/LuaCommon` | 7 | [Client: LuaCommon](client-luacommon.md) |

Counts from the import on 2026-11-05: **854 files, 885k lines written** (215k removed by the flags), 612 blocks rewritten to
`X2_DEV_TOOLS`. Nothing is compiled yet.

## Import (2026-11-05, `client/import`)

- One importer module per layer (`tools/import/modules/client-*.toml`), all with `source_root = "."` (the layers include
  each other through `..`) and `include_scope` limited to their own folder, so a file belongs to exactly one module.
- Flags: `base` + `client.generated` ([Client results](../tools/flags.md#client-results-2026-11-05)). No hand-written
  `flags/client.toml` was needed. `X2ServerProtocol` is built as `X2TOOL|Win32` by the original `Release_US` but is resolved with
  the client set like the rest.
- `extern_dirs` = `src/kncsdk`, `src/platform/include`, `src/server/Common`, `src/server/X2Lib`: includes of already ported
  code are only checked and respelled.
- Layout mirrors the original (T14), so `../X2Lib/...` and `../X2/...` includes between layers keep working.

## Code shared with the servers (T33)

The client compiles the server's `src/server/Common` (packets, event IDs, enums, `KncUidType.h`, `ScriptID_Code.h`,
`NetError`) and `src/server/X2Lib/X2UDPPacket.*`; none of them was imported again. Every `KncWX2Server/Common` file the client
includes already exists in `src/server/Common` (checked on 2026-11-05, 15 files reached from `X2Lib` and `X2ServerProtocol`,
including the `.cpp` packet files that `X2Lib` compiles through `#include`).

- `X2UDPPacket.*`: `X2OPTIMIZE_NPC_NONHOST_SIMULATION` resolved as **true** by hand in `src/server/X2Lib` (the client's layout,
  the one on the wire; the original servers never read these structs), with `static_assert`s of the packed sizes
  (`KDYNAMIC_UNIT_NPC_SYNC` 35 bytes, `KDYNAMIC_UNIT_NPC_MINISYNC` 9) and a round-trip test of the optional tail bytes in
  `test_udp.cpp`. `ACTIVE_KOG_GAME_PERFORMANCE_CHECK`, the other `#ifdef` of that file, is undefined on both sides (commented
  out in `X2/KOGGamePerformanceCheckDefine.h`).
- `X2ServerProtocol/KncUidType.h` is an older copy of the `Common` one (four fewer declarations, otherwise comments): excluded,
  the client uses the shared header.

## KNCSDK_Client

`src/kncsdk` (frozen) already holds the merge of `KNCSDK_Server` and `KNCSDK_Client` ([KNCSDK](kncsdk.md)). On 2026-11-05 every
KNCSDK header the client includes resolves there, and every `KncUtil::` function it calls (`ConvertStringToCTime`,
`toWideString`, `toNarrowString`, `TimeToInt`, `GetMD5`) exists. What `KNCSDK_Client` has and `src/kncsdk` does not is Win32
internals of the same functions, Crypto++ (replaced, T27), `dbg/profiler.*` (not used by the client), `utf8_codecvt_facet` and a
thread test `Main.cpp`. **Nothing had to be brought in.** The client-specific behavior left is logging being off in service builds
(`KNCSDK_Client/dbg/logfile.hpp`), to reproduce through the log level.

## For the skeleton step

- **Includes of excluded files remain** where they were unconditional in the original (`CrashRpt.h`, `KncFirewall.h`,
  `../X2Lib/X2SMTPMail.h` in `KTDX.h`; DXUT headers; Bandi, FMOD, CxImage, browser, Nexon/Steam/IGA headers in `X2Lib`). Each
  report lists them under "References to excluded files"; remove them by hand with the code that uses them.
- `X2Lib/X2Main.h` includes `../../KncWX2Server/Common/Enum/*.h`, which does not exist in the `src/` layout: rewrite to
  `Enum/Enum.h` (found through the `src/server/Common` include directory).
- Include directories (from the original projects): `KTDXLIB`, `KTDXLIB/DXUT`, `X2Lib`, `X2Lib/Common`, `X2`, `X2ServerProtocol`,
  `X2ServerProtocol/Socket`, `src/server/Common`, `src/kncsdk`. `lua_tinker.h`: see [LuaCommon](client-luacommon.md#two-lua_tinker).
- **DXUT** (2026-11-05): used as the application framework, not as a library of widgets. 46 files use 49 distinct `DXUT*` names (44 of them
  functions, mostly `DXUTGetHWND` (56 calls), `DXUTTRACE` (21), `DXUTGetGlobalTimer` (18), `DXUTGetBackBufferSurfaceDesc` (11),
  device settings and lost/reset state; `X2.cpp` installs the callbacks (`DXUTSetCallback*`, `DXUTInit`, `DXUTMainLoop`).
  The GUI (`DXUTgui`), `CDXUTMesh`, the settings dialog and `DXUTsound` appear only in includes or one declaration each; the
  `SAFE_DELETE`/`SAFE_RELEASE`/`SAFE_DELETE_ARRAY` macros (1,338 uses) come from `DXUTmisc.h`. Plan: no DXUT; the skeleton
  provides the window and loop through `x2::app` (T32) and a small header with the `DXUT*` functions the code calls and the
  `SAFE_*` macros.
- `X2Lib/X2StateServerSelect.cpp` was decoded as `cp949-lossy` (invalid bytes in the original, replaced): review its strings.
