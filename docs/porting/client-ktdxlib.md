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

`KTDIManager.*` and `KTDIDevice.*` read SDL3 state instead of DirectInput and first built as `x2::client_ktdi`
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
`ModifyMassFile` (an unused V.0.1 writer), `KFileVerifier` (nothing created it; deleted on 2026-11-06), the undefined
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

## Movies and web pages (2026-11-06, `client/video-browser-cximage`)

Decided with the user; overview in [Client › Bandi, embedded browser and CxImage](client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage).

- **Movies**: `CKTDGUIControl::CMovieData` plays on [`x2::movie::MoviePlayer`](client-movie.md) instead of BandiVideo; its
  Lua interface and its loop/stop at the end are unchanged. The frame goes into a managed `A8R8G8B8` texture
  (`CBandiVideoTexture_DX9` is gone), drawn over the render target with the engine's screen-space quad
  (`DrawFrameTexture`); the player advances by `GetElapsedTime()`. `KTDX.h` includes `MoviePlayer.h` instead of the four
  `BandiVideo*` headers.
- <a id="web-pages"></a>**Web pages**: `KBrowserWrapper` is new code with the interface its callers use (`Init`/`UnInit`,
  `Create`, `Navigate(url, NULL)`, `CloseBrowser`, `IsClosed`, `GetCheckClosed`/`SetCheckClosed`), building as
  `x2::client_browser`. `Navigate` opens the page in the system browser (`SDL_OpenURL`) and the wrapper counts as closed at
  once, so the shop can open it again; `OnFocusGained`, called by `CX2Main::MsgProc` on `WM_SETFOCUS`, clears
  `GetCheckClosed` after a page was opened, which makes `CX2State` ask for the cash balance as the original did when its
  dialog closed (and turns `SetDialogBoxMode` off, as the dialog's close did). A failed open counts as closed at once.
  POST data is refused (no caller sends any). `KPCHWebBrowser` and its keyboard forwarding (`CKTDXApp::OnWebBrowserKeyEvents`,
  `KM_WEB_BROWSER_KEY_EVENT` in `CX2Main`) are gone; the `KM_WEB_BROWSER_KEY_EVENT` enumerator stays for the numbering.
  Tests: `test_browser.cpp`.
- **CxImage**: `CKTDGManager::ScreenCaptureOld`/`ScreenCapture` (GDI + CxImage JPEG; no callers), the `ScreenCapture`
  folder created at start and `m_fCaptureDelay` deleted; `KTDX.h` no longer includes `CxImage/ximage.h`.

### Behavior changes

| Where | Original | Port | Why |
|---|---|---|---|
| Web pages | Inside the game, over its window | The system browser; the balance is asked for when the game window gets the focus back | User, 2026-11-06 |
| `ScreenCapture/` | Created at start | Not created (no screenshots) | Bandicam removed |

## D3D9 device (2026-11-06, `client/ktdxlib-device`)

`D3D9/` holds KTDXLIB's own D3D9 device on `x2::render` (T38) and the D3D9 vocabulary the engine keeps (`D3D9Types.h`),
building as `x2::client_d3d9`; the engine's files move onto it next. See [Client: D3D9 device](client-d3d9.md).

## Engine (2026-11-06, `client/ktdxlib-engine`)

`KTDX.h` (the precompiled header, through `stdafx.h`) and every header it includes compile on the
[D3D9 device](client-d3d9.md) and `x2::platform`: no DirectX, DirectSound, DirectShow or MFC headers; D3DX's types are
declared in `D3D9/D3DXTypes.h` (interfaces as plain classes with no COM counts, T38) until the `.x` and effect steps;
Boost is replaced (`x2::intrusive_ptr`, `KCircularBuffer.h`, `KLexicalCast.h`); the Code Virtualizer markers are empty;
FreeType comes from upstream. `KTDGDeviceFont.cpp` (`ID3DXFont`) is gone; the fonts are `CKTDGFreeTypeFont`'s.

`x2::client_ktdxlib` first built the engine files below (`KTDGStateManager`, `KTDGDynamicVBManager`, `KTDXDevice`,
the texture, render target and sound devices, `KTDSManager`, `KTDSMP3Play`, `KSuperAssert`); since
[the whole engine](#whole-engine-2026-11-06-clientktdxlib-build) it builds every file.

- **State manager:** caches render, texture stage and sampler states with its push/pop stack, on the device (the Xbox and
  Direct3D 8 branches are gone). Its render-state cache starts from the device's defaults; the texture stage and sampler
  defaults are D3D9's. Render targets are textures: `SetRenderTarget(texture)` binds one and sets the viewport to its
  size, `NULL` the back buffer.
- **Dynamic vertex data:** every mode (managed buffer, ring of dynamic buffers, `DrawPrimitiveUP`) is the device's
  `DrawPrimitiveUP`, itself a per-frame ring buffer. Draws longer than a type's size are cut in parts as the original did
  (strips by an even count, fans cut to one part).
- **Textures:** files load through D3DX's functions on the device (`D3D9/D3DXTexture.cpp`); `D3DXCheckTextureRequirements`
  follows the device's formats, and the `.dds`/`.tga`/`.png`/`.jpg`/`.bmp` files are read since 2026-11-07
  ([D3D9 device › Image files](client-d3d9.md#image-files-2026-11-07-clientd3dx-textures)), the detail levels included.
- **Assertions and logs** (`KSuperAssert`): the trace goes to stderr (`OutputDebugStringA` in the original), Retry in the
  assertion box traps into the debugger, Abort ends with `std::_Exit(3)`.

### Render targets

A render target texture is a device texture with `D3DUSAGE_RENDERTARGET` (A8R8G8B8 when no format is given), created at
once since the device is never lost or reset (T38). Its depth buffer is the device's, sized to the target in use (the
original created a depth surface per target). `BeginRender` binds it and `EndRender` binds back the target that was in use
(the original went back to the back buffer it saved at creation). The `StretchRect` copies (`CopySurface`,
`CopySurfaceToBackBuffer`, `CopyBackBufferToSurface`) fail quietly: `x2::render` has no blit yet and the swapchain image
cannot be read, as on the devices where the original set `m_bCannotStretchRect`.

### Sound

A silent backend (`KTDSSoundSystem.h`) in place of FMOD Ex until the [audio step](../project/roadmap.md#phase-4--client-graphics):
`CKTDSManager` is always initialized, with no system object and no 3D sound; its music player (`CKTDSMP3Play`) always
exists and keeps the file it was asked to play, loop, volume and pause, so the options and the states that read them
behave; sound devices (`CKTDXDeviceSound`) load without reading their file and never play (`IsPlaying` is false). Only
these files and `KTDSSoundSystem.h` change when the real backend comes.

### Behavior changes

| Where | Original | Port | Why |
|---|---|---|---|
| `CKTDSManager` | Not initialized when FMOD failed; no music player then | Always initialized, the music player always there | Silent backend |
| `CKTDSMP3Play::Play` | Recorded the file only when its path resolved | Records it whenever it is given | Nothing to resolve without playback |
| `CKTDXDeviceRenderTargetTexture::EndRender` | Back to the back buffer | Back to the target in use before `BeginRender` | Nested targets keep working |
| `CKTDXDeviceRenderTargetTexture::RenderQuad` | Drew its XYZ_TEX1 vertices as `DVB_TYPE_XYZRHW_DIFFUSE_TEX1` (wrong layout) | `DVB_TYPE_XYZ_TEX1` | Evident bug |
| Render target copies | `StretchRect` | Fail quietly | No blit in `x2::render` yet |

### Tests (`x2_client_tests -tc="ktdxlib*"`)

On the null backend ([source](../../src/client/tests/test_ktdxlib.cpp)): the state manager's cache and stack and the
viewport of a bound target; dynamic vertex data drawn in parts (lists, strips sharing their last vertices, multipass,
split data); render target textures created, bound and released while bound; the silent sound backend.

## Whole engine (2026-11-06, `client/ktdxlib-build`)

All 100 `.cpp` files compile and link: `x2::client_ktdxlib` holds 97 of them (the input layer joined it; its `CKTDXStage`
base lives here), `x2::client_massfile` and `x2::client_browser` the other three. GCC, Clang and clang-cl build them
without warnings. The tests link the real `CKTDXApp`, `CKTDGManager`, `CKTDXDeviceManager` and `CKTDXStage`; X2.cpp's
two globals (`g_pd3dDevice`, `g_pEffectStateManager`) are defined for them in `tests/x2_globals.cpp` until X2 builds.

- **Device callers** use the device's own API (T38): `HRESULT` results became `bool`/`void`, textures and vertex buffers
  are deleted (`SAFE_DELETE`) rather than released, render targets are textures (`CKTDGGrayScale` keeps the previous
  target as a texture). `CKTDGEffectStateManager` returns `D3D_OK` for what it forwards, ignores the shader constants
  the effects will not need until T37, and is no longer reference counted (`Release` deletes it).
- **D3DX interfaces** (`ID3DXMesh`, skin info, animation sets, `ID3DXEffect`) are declared and their creators fail
  ([D3D9 device](client-d3d9.md#files)), so the `.x` meshes and effects load nothing until their steps; `CComQIPtr`
  became `dynamic_cast` into `CComPtr`.
- **Threads**: `KJohnThread` is on `std::thread` (suspended start through a condition variable, `ResumeThread`,
  `HasThread`, `IsThreadFinished`); the device manager's loader uses it with an atomic sync flag.
- **Files**: `KFileSerializer` and `CRC_32::GetCRCFromFile` read and write with file streams; narrow names are UTF-8.
  Device directories are listed with `std::filesystem`.
- **UI and IME**: the Win32 window/keyboard functions and the clipboard come from
  [DXUT's `WindowFunctions`](client-dxut.md#files); the IME edit box's changes are on [Client: IME](client-ime.md#behavior-changes);
  Uniscribe (loaded but never used for layout) is gone from `CKTDGUIUniBuffer`.
- **UDP** (`CKTDNUDP`): non-blocking and polled every frame (`x2::platform::SocketReadable`) instead of a
  `WSAEventSelect` event; scattered sends through `x2::platform::SendToScattered`.
- **Start-up** (`CKTDXApp`): the unicows/usp10 preloader, COM initialization, the release build's "minimize every other
  window", the unused `FindProcess` (a process scan) and the Themida markers are gone.
- **Loading screens**: X2Lib's 32 `BeginScene`/`OnFrameRender`/`EndScene`/`Present` sequences call
  `CKTDXApp::RenderStageNow(stage)` (T38); the state-change one in `CX2Main` lost its lost-device handling (the device is
  never lost).
- **Warnings**: mostly mechanical ([Mechanical porting](../tools/mechanical-porting.md)); the release `ASSERT` keeps the
  variables it checks in use (`sizeof`), as LuaCommon's does. Recursive `__forceinline` frame walks are plain `inline`.

### Behavior changes

| Where | Original | Port | Why |
|---|---|---|---|
| `KJohnThread::EndThread(timeout)` | Waited up to the timeout, then closed the handle of a thread still running | Always waits for the thread to end | A `std::thread` cannot be dropped while it runs |
| `CKTDNUDP` local addresses | `h_addr_list[0]` read for every entry of the host's address list | Each entry read | Evident bug |
| `CKTDNUDP` address lookup | `gethostbyname` (crashed if the host name did not resolve), `inet_addr`/`inet_ntoa` | `getaddrinfo` (IPv4, first 10, empty list if unresolved), `x2::platform::IpFromString`/`IpToString` (dotted quads only) | Deprecated in Winsock |
| `CKTDNUDP` socket | `UDP_CHECKSUM_COVERAGE` set (Winsock UDP-Lite option) | Not set | UDP over IPv4 has checksums anyway |
| `_IsEqualSign` (`KTDXAnimationController`, key compression) | Or-ed `uiFlags1` with the uninitialized result itself | `uiFlags1 | uiFlags2` | Evident bug |
| Ctrl+\ in `CKTDGUIEditBox`/`CKTDGUIIMEEditBox` | The comment `// Ctrl \` spliced the next `break;` into it, so Ctrl+[, ], \ fell into Enter | Ignored, as the other control characters | Evident bug |
| `CKTDGPicChar` textures | Indexed by a signed `char` (below the array for bytes 0x80 and up) | Bounded by the byte value | Evident bug |
| `CKTDGUIDialog` script-name tool (dev) | Opened `MouseOverScript.txt` in notepad++ | Only writes the file | Windows-only editor |
| `CKTDXApp` start-up (release) | Minimized every other window unless `Inactivation_Mimize.txt` existed | Nothing minimized | Windows shell only; hostile to the desktop |
| `CKTDGUIIMEEditBox::Initialize` | Loaded `version.dll` and the IME's DLL | Nothing loaded | [Client: IME](client-ime.md#behavior-changes) |
| D3DX meshes, skins, animation controllers, effects | Loaded by D3DX | Fail (`E_NOTIMPL`) | Until the `.x` and effect steps |

### Tests (`x2_client_tests -tc="ktdxlib*,d3dx*"`)

The [engine tests](#tests-x2_client_tests--tcktdxlib) now run on the real `CKTDXApp`; added: `RenderStageNow` (one frame
drawn and submitted, a frame with no stage), `KFileSerializer` save/load/cap/missing file and `CRC_32::GetCRCFromFile`.

## Status

All of `KTDXLIB` compiles and links ([roadmap](../project/roadmap.md#phase-4--client-graphics)); the `.x` meshes, effects
and image loaders are the next steps.
