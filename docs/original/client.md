[Home](../README.md) › [Original code](README.md) › Client — Windows / Direct3D 9 dependencies

# Client — Windows / Direct3D 9 dependencies

Three layers: `X2` (exe, 2 sources) → `X2Lib` (game, 285 sources in vcxproj, 609 files) → `KTDXLIB` (engine, 120 sources, 338 files).
Two libraries below them: `X2ServerProtocol` (client network library, built as `X2TOOL|Win32` by the solution's `Release_US`) and
`LuaCommon` (the client's `KLuaManager`/`KLuabinder`/`lua_tinker`, compiled by the `luajitLib` project with LuaJIT 2.0.2).
`KTDXLIB` uses `DXUT/` as its application framework (window, device, loop, timer), not for its widgets; usage counts are on
[Client](../porting/client.md#for-the-skeleton-step).
Original build: Win32 x86, Unicode, MFC in some configurations (`_AFXDLL`), `_USE_32BIT_TIME_T`.

## Good news: D3D9 is concentrated in KTDXLIB

| Pattern | X2Lib | KTDXLIB | Server |
|---|---:|---:|---:|
| `IDirect3DDevice9` / `LPDIRECT3DDEVICE9` | 2 | 56 | 0 |
| `GetDevice()` | 22 | 29 | 0 |
| `LPDIRECT3DTEXTURE9` | 0 | 11 | 0 |
| `D3DRS_`/`D3DFVF`/`D3DPT_`/`D3DFMT` | 14 | 57 | 1 |
| D3DX math types (`D3DXVECTOR*`, `D3DXMATRIX`...) | 367 | 104 | 13 |
| D3DX math functions (`D3DXVec*`, `D3DXMatrix*`...) | 50 | 33 | 4 |
| `HWND` | 272 | 58 | 13 |
| `MessageBox` | 40 | 19 | 4 |

Interpretation: X2Lib uses D3D almost exclusively as a **math library** (`D3DXVECTOR3` appears ~8,000 times) and through
`HWND` in the `MsgProc` signature. A compatible math shim plus event translation isolate most of X2Lib from the graphics port.
The ~22 X2Lib files that call `GetDevice()` need individual review.

## D3DX features that need replacements

| Feature | Use | Replacement |
|---|---|---|
| `D3DXLoadMeshFromX` / `...HierarchyFromX`, `ID3DXAnimationController` | `KTDXDeviceXMesh`, `KTDXDeviceXSkinMesh`, `KTDXAnimationController` | custom `.x` parser (text/binary) + skinning; `.x` remains an asset format |
| `D3DXCreateEffect` | 4 files; 4 `.fx` in `data041.kom`: `XRenderer` (20 techniques), `RenderState` (4), `XPostEffect` (5), `GCSkin` (2), ~68 KB in all; almost all `vs_1_1` with fixed-function pixel stages (`ColorOp`/`AlphaOp`/`FogEnable`), `ps_2_0`/`vs_2_0` only in `XPostEffect`, `ps_1_1` twice in `RenderState` (2026-11-06) | rewrite by hand as modern HLSL (T37); this is **code**, not an asset |
| `D3DXCreateTextureFromFile`, cube textures | `KTDXDeviceTexture` (from memory, one level; the texture detail option resizes and re-encodes to DXT1/DXT3); no cube map in the assets (2026-11-07) | DDS loader + stb_image, done: [Image files](../porting/client-d3d9.md#image-files-2026-11-07-clientd3dx-textures) |
| `D3DXCreateFont`, `D3DXCreateSprite` | fonts/UI | FreeType (already used in `KTDGFreeTypeFont`) + 2D batcher |
| Fixed-function (`SetRenderState`, `DrawPrimitiveUP`, FVF) | 16 / 21 files | emulate fixed-function pipeline with shaders + dynamic buffers |

## Other subsystems

| Area | Where | Plan |
|---|---|---|
| Window / loop | `KTDXApp`, `DXUT/` | SDL3 |
| Input | `KTDIDevice*` (DirectInput8 in 6 files), `KTDIDeviceWinKey` | SDL3 |
| IME (Korean/CJK) | `KTDGUIIMEEditBox`, `KHangulPPWSelector` (`ImmGetContext` in 5 files, `WM_IME_*`) | SDL3 text input/IME; IMM32 emulated on SDL ([Client: IME](../porting/client-ime.md)) |
| Audio | `KTDSManager`, `KTDXDeviceSound`, `KTDSMP3Play`, `FMOD/` (FMOD Ex), DirectSound | miniaudio / SDL3 audio, or current FMOD Core |
| Video | `BandiVideo*` (15 files), `bdvid32.dll`: the 30 character creation movies in `data/movie` (MPEG-1 video in AVI, 400×220, 24 fps, I/P frames, no audio; 2026-11-06) | pl_mpeg ([Client: movies](../porting/client-movie.md)) |
| Recording | Bandicam (`bandicap.h`, `bdcap32.dll`): Scroll Lock video recording, Print Screen JPEG screenshots into `ScreenCapture/` | removed (2026-11-06, [Client](../porting/client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage)) |
| Networking | `KTDNManager`, `KTDNUDP`, `KTDNTransfer`, `X2ServerProtocol/Socket` (IOCP in the client too!) | same networking library as the server (Asio) |
| `.kom` files | `KGCMassFileManager`, `KGCMassFileBufferManager` (zlib `uncompress`, libxml2 for the header) | `x2::kom` (T35); survey in [`.kom` archives](../porting/kom.md#the-original-ktdxlibkgcmassfilemanager-surveyed-2026-11-05) |
| Embedded browser | `KBrowserWrapper`, `KPCHWebBrowser`, `Browser.c`: Internet Explorer for the cash charge page and the server's advertisement pages | the system browser (2026-11-06, [KTDXLIB](../porting/client-ktdxlib.md#web-pages)) |
| Crash/anti-hack | `CrashRpt`, `KncFirewall`, `KProtectedType`, `AntiHackingString.h`, XTRAP, HackShield | remove |
| Publisher | `NexonModule`, `Steamworks`, `IGA` (ads), `X2Lib/OnlyGlobal/<country>` | remove/stub |

## Client-specific risks

- **x64**: ~600 pointer→`DWORD/int/long` casts (regex estimate, client+server) and 12 files with `__asm`.
- `_USE_32BIT_TIME_T`: 32-bit `time_t` may occur in written/serialized structs.
- Initialization order of global singletons (`KJohnSingleton`, `KGlobalSingleton`, `KDynamicSingleton`).
- The `.fx` mix fixed-function state and shaders — manual translation (T37); visually validate against the original.
