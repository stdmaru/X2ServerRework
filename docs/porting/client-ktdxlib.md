[Home](../README.md) › [Porting](README.md) › Client: KTDXLIB

# Client: KTDXLIB (`src/client/KTDXLIB`)

The client engine: application loop and device (`KTDXApp`, `KTDXDevice*`), graphics (`KTDG*`: particles, meshes, skinning,
fonts, post effects), the UI widgets (`KTDGUI*`), input (`KTDI*`), sound (`KTDS*`), networking (`KTDN*`, including the client
`KTDNUDP`), `.kom` files (`KGCMassFile*`) and the configuration headers (`KTDX.h`, `Always.h`). D3D9 is concentrated here
([analysis](../original/client.md)). Overview of the client port: [Client](client.md).

## Import (2026-11-05)

- Module `client-ktdxlib` ([report](../../tools/import/reports/client-ktdxlib/REPORT.md)): files of `KTDXLIB/` compiled by
  `KTDXLIB.vcxproj` (`US_INTERNAL|Win32`) and their headers: **253 files** (102 `.cpp`), 151k lines written; 2,141 `#if` groups
  resolved, 17k lines removed; 78 blocks became `X2_DEV_TOOLS`; 179 include spellings fixed. Encoding: 168 CP949, 78 ASCII,
  7 UTF-8. Largest: `KTDGParticleSystem.cpp` (5.5k), `KTDGXMeshPlayer.cpp` (3.9k), `KTDGUIIMEEditBox.cpp` (3.6k).
- **Excluded** (references counted in the report): vendored libraries, which come from upstream or are replaced (T3): `DXUT/`
  (18 references; see [DXUT](client.md#for-the-skeleton-step)), `CxImage/` (24), `FMOD/` (12), `tinyxml/` (5), `zlib.h`/`zconf.h`,
  `vld*.h`, `usewinhoard.cpp`; the Bandi video player (`BandiVideo*`, `bandivid.h`, 16); the embedded browser (`KBrowserWrapper`,
  `KPCHWebBrowser`, `Browser.c`); `CrashRpt.h`, `KncFirewall.h`, XTRAP; the Thai font
  (`KTDGDeviceFontTH`) and the `OnlyGlobal/Always_BR/JP/PH/TWHK.h` headers of other countries.
- `KTDX.h` includes `../LuaCommon/*` ([LuaCommon](client-luacommon.md)) and `../X2/KOGGamePerformanceCheckDefine.h`
  (imported by [X2](client-x2.md)); `KTDXApp.cpp` and `KTDGUIUniBuffer.cpp` include `../X2Lib/X2Define.h`.
- `#pragma comment(lib)` removed, to re-create in CMake where still needed: D3D9/D3DX, DirectInput, DirectSound, DirectShow,
  `ddraw`, `dplayx`, `imm32`, `winmm`, `ws2_32`, `psapi`, `comctl32`, FreeType, libxml, libpng, libjpeg, zlib, LuaJIT, CxImage,
  CrashRpt, Mail MIME/SMTP and the KOG profiler.

- `AntiHackingString.h` was excluded at first but is only string `#define`s used by the game, not anti-cheat code: imported on
  2026-11-05 before the first hand edit.

## Hand edits (2026-11-05, `client/skeleton`)

The module is edited by hand and frozen. `KTDX.h` includes the [DXUT replacement](client-dxut.md) and no longer includes
CrashRpt, KncFirewall or the SMTP mail; `KTDXApp.cpp` lost the firewall exception
([Client](client.md#integrations-removed-by-hand-2026-11-05)). `SAFE_*` macros are `KTDX.h`'s own.

## Input layer (2026-11-05, `client/input`)

`KTDIManager.*` and `KTDIDevice.*` read SDL3 state instead of DirectInput and build as `x2::client_ktdi`
([Client: input](client-input.md)). `GAME_ACTION`/`JOYSTICK_BUTTON` moved to the new `KTDIGameAction.h`; the empty
`KTDIDeviceWinKey.*` were deleted (252 files left). `KTDX.h` includes `DXUT/DirectInputKeys.h` instead of `<dinput.h>`,
and `KTDXStage.h`/`KTDXDeviceHolder.h` include what they use (`std::wstring`, `std::list`, `x2/platform/win32.h`).

## IME edit box (2026-11-05, `client/ime-candidates`)

`CKTDGUIIMEEditBox::Initialize()` points its `_Imm*` pointers at the IMM32 emulation on SDL3 (`DXUT/Ime.h`,
`EMULATEDPROC` in `KTDGUIUniBuffer.h`) instead of loading `imm32.dll` and `version.dll`; `s_hDllImm32` became
`s_bImmBound`. See [Client: IME](client-ime.md).

## Mass files (2026-11-05, `client/kom-mass-file`)

<a id="mass-files"></a>`KGCMassFileManager` and `KGCMassFileBufferManager` were rewritten on [`x2::kom`](kom.md) (T35) and
build as `x2::client_massfile`; the original is surveyed on [`.kom` archives › The original](kom.md#the-original-ktdxlibkgcmassfilemanager-surveyed-2026-11-05).
The interface the engine calls stays: `LoadDataFile`, `IsValidFile`, `LoadTwoDataFiles`, the `*_LocalFile` variants,
`GetPullPath`, `MASSFILE_MEMBERFILEINFO_POINTER` (tested as a pointer: `Info == NULL`, `!Info`, `Info->size`),
`KGCMassFileBufferPtr`, the mapping and server-time functions bound to Lua. Gone: `CMassFile` as an archive (only the scope
of the info types is left), libxml2, Win32 handles, `RegisterCurrentThread` and the two-thread limit, `SaveMassFile`,
`ModifyMassFile` (an unused V.0.1 writer), `KFileVerifier` (never imported: nothing created it), the undefined
`SetWebAddress_LUA`/`GetUsableMappingIndex`, `CHECK_FILE_DUPLICATION`.

- **Archives**: `AddMassFiles(folder, names)` mounts `CX2Main`'s fixed list (`GetClientArchiveNames()`: `data001`–`150`,
  `datai001`–`023`) in that order; a name in two archives keeps the first, with a warning (`x2::kom::Mount`). Entries are
  decoded by their `Algorithm` (0, 2 and 3) into pooled buffers (`Archive::ReadInto`), checksums checked; names are full,
  case-insensitive (ASCII). `LockMassFileMap` ends mounting; loading threads then need no lock.
- **Data directories** (every build): `SetSubDirectory(root)` registers the game folder and its subfolders, recursively,
  except `root/data` and folders starting with `.`; `*_LocalFile` and `GetPullPath` search them in order (user settings
  such as `GameOptions.lua`, `music/`, `movie/`), case-insensitively on Linux. Never the archives.
- **Loose assets** (`X2_DEV_TOOLS` only, T35 (3)): `SetSubDirectory` also indexes `root/data` recursively by file name
  (`SetLooseFileFolder`; again to pick up new files), skipping folders starting with `.` — a
  [client_data](../tools/client-data.md) folder works as `/data/data`, its `.kom/` skipped. `LoadDataFile`, `IsValidFile`
  and `LoadTwoDataFiles` **prefer** a loose file (flagged `MFI_LOCALFILE`), then the archives; `*_LocalFile` and
  `GetPullPath` look there after the data directories. Release builds never read it.
- **Date-range renames** (`MassFileMappingList.lua` in `datai015`, kept by the user's choice): `MassFileMapping()` builds
  the renames in range at the server time and swaps them in under a lock, for archives and loose files alike.
- **Errors** go to a log sink given to the constructor (stderr by default); the engine's `ErrorLogMsg` once KTDXLIB builds.

### Behavior changes

| Where | Original | Port | Why |
|---|---|---|---|
| Duplicate names | The whole later archive rejected (message box); in release that was the deliberate crash | First wins, warning | T35 |
| Missing or rejected archive | Release: `*x = 1` crash; dev: continue | `CX2Main` shows a message naming the archive and the reason, logs it and exits, in every build | User, 2026-11-05 |
| `DeleteOtherFile`, `DeleteFile` at start | Deleted every `.kom`, `.kom_` and `.dll` not in its lists anywhere under the game folder, plus `item.kim` and a few stray files | Nothing deleted; a `.kom` of the folder outside the list is reported and ignored. `m_DLLList` removed | User, 2026-11-05 |
| Loose files in dev builds | Only for names in no archive; any folder under the current one | Preferred over the archives; only under `data/` | T35 (3) |
| Names | Upper-cased, truncated to 59 characters (`data110.kom`'s 61-character name was unreachable); `strFileName` upper-cased | Full, case-insensitive; `strFileName` is the stored spelling, a `std::wstring` | T35 |
| Decoding | zlib only; `LoadAndDoMemory(..., bEncryption)` asked for the stored blob (`MFI_COMPRESSEDDATA`) and decrypted XORCRC itself | The manager decodes by `Algorithm`; `MFI_COMPRESSEDDATA`, `compressedSize` and the client's `XORCRCDecrypt` copy removed, `bEncryption` ignored | One implementation of the format (T35) |
| Checksums | Never checked | Every blob checked against its adler32 | `x2::kom` |
| Renames | Kom names: the map rewritten in place at `MassFileMapping()` (a race with the loading thread); loose files: `ExternFileMapping` at every read | One rename table, replaced under a lock at `MassFileMapping()`, used for both | Thread safety |
| Missing-file logs | `LoadDataFile` logged; `LoadTwoDataFiles` logged the first name | Same; a missing second file (`.tet`) or `*_LocalFile` is not logged | |
| `LoadTwoRealFiles` (dev) | Opened the second file into the first file's handle (leaking it), so the second file was never read | Both read | Bug |
| `CX2Main::DeleteFile` (`DeleteFile_LUA`) | Tested the array instead of `GetPullPath`'s result: deleted the bare name when nothing was found | Deletes only a file that was found | Bug |
| Buffers | `boost::intrusive_ptr`, offsets alternating between the two ends of a block | `std::shared_ptr` returning blocks to their bucket, no offset trick; blocks over 4 MB not pooled (23 buckets, as before) | C++20 standard library |

### Tests (`ctest -R client`, `x2_client_tests -tc="massfile*"`)

Headless, on archives the tests build: the archive list, list order and first-wins with its warning, an archive outside
the list, algorithms 0/2/3, a 61-character name, size-only loads, missing and rejected archives named, the locked map;
loose files preferred (dev) or ignored (release), `.kom/` and dot folders skipped, a name in two loose folders, user
files and `GetPullPath` found case-insensitively, re-indexing; two files at once; date-range renames; 8 threads loading
64 entries; the buffer pool. **Opt-in**: `X2_MASSFILE_CLIENT_DATA=_client/data` mounts the 173 archives (101,826 names,
under 1 s, no repeated name, no extra archive) and decodes every entry through the manager with 8 threads (4.9 GB in 7 s,
`linux-debug`, 2026-11-05); with `X2_MASSFILE_LOOSE=<client_data>` it also indexes a client_data (101,827 files,
`Item.kim` being `Item.lua` + `SetItem.lua`) and loads its readable `Enum.lua`.

Open: with a client_data as loose folder, an edited `Item.lua`/`SetItem.lua` is not seen, since the client loads
`Item.kim` (from the archive); compiling it at start or in the tools is for when the engine runs.

## Status

Only the input files and the mass file manager compile ([roadmap](../project/roadmap.md#phase-3--client-platform)).
