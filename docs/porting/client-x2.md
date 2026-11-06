[Home](../README.md) › [Porting](README.md) › Client: X2

# Client: X2 (`src/client/X2`)

The client executable: `X2.cpp` (`WinMain`, which sets up DXUT and its callbacks and runs `CX2Main`), the precompiled header of
the DXUT samples (`Common/dxstdafx.*`), `resource.h`, and `KOGGamePerformanceCheckDefine.h`. Overview of the client port:
[Client](client.md).

## Import (2026-11-05)

- Module `client-x2` ([report](../../tools/import/reports/client-x2/REPORT.md)): **5 files**, 1.4k lines written; 88 `#if`
  groups resolved, 1.2k lines removed; 8 blocks became `X2_DEV_TOOLS`.
- `KOGGamePerformanceCheckDefine.h` is in no `X2` project, but `KTDXLIB/KTDX.h` and `X2Lib/X2Main.h` include it: added as
  `extra`. Its `ACTIVE_KOG_GAME_PERFORMANCE_CHECK*` defines are commented out, so everything in it except the empty `KLagCheck`
  macros is dead.
- **Excluded**: BugTrap (`BugTrap.h`, `BTTrace.h`, closed-source `BugTrap.lib`) and the `TW_SERVICE/` build files. The `.rc`
  (icon, cursor, manifest) is not a source file for the importer; the skeleton step re-creates the icon like the servers'
  (`x2_windows_icon`, [Porting](README.md)).

## Skeleton (2026-11-05, `client/skeleton`)

`X2.cpp` needs `CKTDXApp` and `CX2Main`, which do not compile yet, so the `x2_client` executable (`x2` / `x2.exe`) is built from
`X2Skeleton.cpp`: the start of `VirtualWinMain` on the [DXUT replacement](client-dxut.md), with the callbacks `X2.cpp` installs
(`MsgProc`, `KeyboardProc`, `OnFrameMove`, `OnFrameRender`) as stubs. It opens the 1024×768 window, runs the loop and delivers
the translated messages; `--trace-messages` logs every message `MsgProc` receives (except `WM_MOUSEMOVE`). `X2.cpp` replaces it
once the engine builds.

- **Icon**: `Elsword.ico`, copied from the original through git, attached with `x2_windows_icon` like the servers'
  ([Porting](README.md)). On Windows SDL3 uses the executable's first icon resource for the window too.
- `X2.cpp` hand edits: SMTP mail, Steam and nProtect calls removed ([Client](client.md#integrations-removed-by-hand-2026-11-05)); on
  2026-11-06 the Bandicam frame capture in `OnFrameRender` (`Work`, the REC particle hidden during the capture) went with
  Bandicam ([Client](client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage)).

## Status

Skeleton builds and runs on Linux (Clang, GCC) and Windows (clang-cl); `X2.cpp` not compiled yet.
