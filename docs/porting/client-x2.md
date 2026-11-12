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
the translated messages; `--trace-messages` logs every message `MsgProc` receives (except `WM_MOUSEMOVE`). `X2.cpp` replaced it
on 2026-11-12.

- **Icon**: `Elsword.ico`, copied from the original through git, attached with `x2_windows_icon` like the servers'
  ([Porting](README.md)). On Windows SDL3 uses the executable's first icon resource for the window too.
- `X2.cpp` hand edits: SMTP mail, Steam and nProtect calls removed ([Client](client.md#integrations-removed-by-hand-2026-11-05)); on
  2026-11-06 the Bandicam frame capture in `OnFrameRender` (`Work`, the REC particle hidden during the capture) went with
  Bandicam ([Client](client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage)).

## `X2.cpp` (2026-11-12, `client/x2-link`)

`X2.cpp` builds `x2_client` on `x2::client_x2lib`, and `X2Skeleton.cpp` is gone (with its test card and `--trace-messages`).
The executable links warning-free on Clang (dev tools ON) and GCC (dev tools OFF).

- **Entry point:** `main( argc, argv )` through `SDL_main.h` (which provides `WinMain` on Windows), in place of `WinMain`
  → `VirtualWinMain`. It stores the arguments with `DXUTSetCommandLine`.
- **Device callbacks** as in the original: `OnCreateDevice` creates `CKTDXApp` and `CX2Main`, `OnResetDevice`,
  `OnLostDevice` and `OnDestroyDevice` (deletes the app), on the device callbacks the
  [DXUT replacement](client-dxut.md) now has. `DXUTCreateDevice( D3DADAPTER_DEFAULT, true, 1024, 768 )` loses its
  `IsDeviceAcceptable` and `ModifyDeviceSettings` callbacks. They checked D3D caps and chose depth formats and behavior
  flags for DXUT's enumeration, and the port has one device (T38).
- **Release builds** (`X2_DEV_TOOLS` off) still need `PATCHER_RUN_ONLY` as the last argument, as the original did. The
  `ElswordX2` mutex is now `x2::platform::SingleInstance` ([Platform](platform.md)).
- **Removed:**
  - `ObtainSeDebugPrivilege` and `ReallySetForegroundWindow`, the HackShield workarounds;
  - the restore of the "Elsword - Windows Internet Explorer" window on exit (the embedded browser is gone,
    [Client](client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage));
  - `LoadIcon` (the window takes the executable's icon), `g_pD3D` and the CRT debug-heap flags.
- **Debug text** (`RenderText`, Home for special users): `DXUTGetFrameStats`/`DXUTGetDeviceStats` come from the DXUT
  replacement. With dev tools, the memory lines show physical memory and the process's resident and peak memory (the
  original showed the 32-bit virtual address space), through `x2::platform::PhysicalMemory`/`CurrentProcessMemory`.
  Legacy `swprintf` calls became `swprintf_s` (MSVC `%s` semantics).

## Start-up (2026-11-12, `client/x2-run`)

In the [client folder](../tools/client-folder.md), headless (`null` renderer, dev tools ON), the client reaches
`XS_LOGIN` in about 4 seconds. Fixed on the way: the config file name (T40; the importer had left the original's
`#elif CLIENT_COUNTRY_US` as an undefined `#if`, so the client asked for `Config_INTERNAL.lua` under the US names).

Left in the logs, from the data and as in the original:

- `OpenXET` fails for 40 `MOTION_*.XET` (e.g. `MOTION_ELSWORD_LK.XET`): no archive has them.
- 48 `_GetControl` misses while the community dialogs load.
- First start only: `GameOptions.lua` and `TutorSystem.lua` missing (the client writes them).

With a window (2026-11-12, Hyprland, workspace 9): the window floats at 1024×768 and reaches the login screen, but
the screen shows no textures (a 512×512 orange quad at the top left on gray). Next to investigate. Also fixed:
the string table (`general.ess`, see [Client: KTDXLIB](client-ktdxlib.md)), "not responding" during start-up
([Client: DXUT](client-dxut.md)).

## Status

`x2` links and starts on Linux, and reaches the login screen headless (2026-11-12). Next: the window on a GPU, then
login → channel → world on the Linux servers ([roadmap](../project/roadmap.md#phase-4--client-graphics)).
