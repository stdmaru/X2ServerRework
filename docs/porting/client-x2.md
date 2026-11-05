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

## Status

Imported, not compiled yet. The skeleton replaces DXUT's window and loop with `x2::app` (T32).
