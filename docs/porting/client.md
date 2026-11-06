[Home](../README.md) › [Porting](README.md) › Client

# Client (`src/client`)

Overview of the client port: layers, how they were imported, the code shared with the servers, and the build. Module details are on each layer's page. Analysis of the original's Windows and
Direct3D 9 dependencies: [Client — Windows / Direct3D 9 dependencies](../original/client.md).

| Layer | Original | `src/` | Files | Page |
|---|---|---|---:|---|
| Executable (`WinMain`) | `X2` | `src/client/X2` | 5 | [Client: X2](client-x2.md) |
| Game | `X2Lib` | `src/client/X2Lib` | 552 | [Client: X2Lib](client-x2lib.md) |
| Engine | `KTDXLIB` | `src/client/KTDXLIB` | 252 (2026-11-05) | [Client: KTDXLIB](client-ktdxlib.md) |
| Network library | `X2ServerProtocol` | `src/client/X2ServerProtocol` | 11 (10 of 37 imported, plus `EventNames.cpp`; 2026-11-05) | [Client: X2ServerProtocol](client-serverprotocol.md) |
| Lua binding | `LuaCommon` (built by `luajitLib`) | `src/client/LuaCommon` | 7 | [Client: LuaCommon](client-luacommon.md) |
| Application framework | DXUT (Microsoft, not imported) | `src/client/DXUT` | 9 (new; 2026-11-05) | [Client: DXUT](client-dxut.md), [input](client-input.md), [IME](client-ime.md) |

Counts from the import on 2026-11-05: **854 files, 885k lines written** (215k removed by the flags), 612 blocks rewritten to
`X2_DEV_TOOLS`. On 2026-11-05 (`client/skeleton`) the HackShield/nProtect wrappers were deleted (547 `X2Lib` files left), and
all imported client modules are edited by hand and **frozen** in the importer. On 2026-11-05 (`client/serverprotocol`, T34)
27 `X2ServerProtocol` files, copies of the servers' network core, were deleted (10 left).

## Build (2026-11-05, `client/skeleton`)

`src/client/CMakeLists.txt`, enabled by `X2_BUILD_CLIENT` (default `ON`). A build directory configured before 2026-11-05
keeps `OFF` in its cache: reconfigure with `-DX2_BUILD_CLIENT=ON`. What compiles so far:

| Target | Contents |
|---|---|
| `x2::client_luacommon` | `LuaCommon` on upstream LuaJIT (T21) |
| `x2::client_dxut` | the DXUT replacement on `x2::app` ([Client: DXUT](client-dxut.md)), including the SDL3 state behind the input layer and the IME edit box's IMM32 ([Client: IME](client-ime.md)) |
| `x2::client_ktdi` | `KTDXLIB`'s input layer (`KTDIManager`, `KTDIDevice`) on SDL3 instead of DirectInput (2026-11-05, [Client: input](client-input.md)) |
| `x2::client_massfile` | `KTDXLIB`'s `KGCMassFileManager` and buffer pool on `x2::kom` (T35, 2026-11-05, [Client: KTDXLIB › Mass files](client-ktdxlib.md#mass-files)) |
| `x2::client_serverprotocol` | `X2ServerProtocol` on the servers' `x2::common_net` (T34, 2026-11-05, [Client: X2ServerProtocol](client-serverprotocol.md)) |
| `x2_client` (`x2`/`x2.exe`) | the skeleton entry point `X2/X2Skeleton.cpp`, with the original icon ([Client: X2](client-x2.md)) |
| `x2_client_tests` | headless tests (ctest `client`) |

`x2_client_includes` carries the original projects' include directories: `KTDXLIB`, `DXUT`, `X2Lib`, `X2Lib/Common`, `X2`,
`X2ServerProtocol` and `src/server/Common` (`X2ServerProtocol/Socket` is gone since 2026-11-05: the shared `Socket/` is
`src/server/Common/Socket`). `KTDXLIB` (apart from its input files and mass file manager), `X2Lib` and `X2.cpp` are not compiled yet: each gets its target when its
dependencies are ported
([roadmap](../project/roadmap.md#phase-3--client-platform)).

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

## Integrations removed by hand (2026-11-05)

The importer excluded these files, but code still included or called them unconditionally. Removed with the code using them,
keeping the paths that run without the integration:

- `KTDX.h`: `CrashRpt.h`, `KncFirewall.h`, `X2SMTPMail.h`; the six DXUT headers became `DXUT.h` ([Client: DXUT](client-dxut.md)).
  `KTDXApp.cpp`: the Windows Firewall exception at start-up.
- SMTP mail of logs (`g_pX2SMTPMail` in `X2.cpp`, `CX2Main::SendCheckMail`, never called) and BugTrap (`X2Main.cpp`).
- Steam: no Steam client in the port. `X2Main.cpp` keeps only the command-line login, `X2CashShop.cpp` the non-Steam charge
  pages and browser, `X2StateLogin.cpp` drops the Steam-only error box, and `m_bSteamClient` is sent as `false` (the field stays
  in the packet). The Steam login password (`J2380DE81621XZ`) typed by hand still quits the client, as
  `CX2Steam::IsValidSteamUser` did.
- nProtect GameGuard and HackShield (T10): `X2HSGame.*`, `X2KNPGame.*` and `NPGameLib.h` deleted and excluded; the game-monitor
  check in `X2Main.cpp` removed; `EGS_KNPROTECT_AUTH_CHECK_NOT` is still read, then dropped.
- `X2Main.h`: `X2Steam.h`, `NPGameLib.h`, `X2KNPGame.h`, `X2NexonMsgHandler.h`, `X2MailLog.h` removed;
  `../../KncWX2Server/Common/Enum/*.h` became `Enum/*.h` (from `src/server/Common`, T33).
- **Left for their own items** (replaced, not removed): FMOD, Bandi video, CxImage, the embedded browser, tinyxml and zlib
  ([roadmap](../project/roadmap.md#phase-3--client-platform)). Each import report lists them under "References to excluded files".

## Notes

- `X2Lib/X2StateServerSelect.cpp` was decoded as `cp949-lossy` (invalid bytes in the original, replaced): review its strings.
