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
  `KPCHWebBrowser`, `Browser.c`); `CrashRpt.h`, `KncFirewall.h`, `AntiHackingString.h`, XTRAP; the Thai font
  (`KTDGDeviceFontTH`) and the `OnlyGlobal/Always_BR/JP/PH/TWHK.h` headers of other countries.
- `KTDX.h` includes `../LuaCommon/*` ([LuaCommon](client-luacommon.md)) and `../X2/KOGGamePerformanceCheckDefine.h`
  (imported by [X2](client-x2.md)); `KTDXApp.cpp` and `KTDGUIUniBuffer.cpp` include `../X2Lib/X2Define.h`.
- `#pragma comment(lib)` removed, to re-create in CMake where still needed: D3D9/D3DX, DirectInput, DirectSound, DirectShow,
  `ddraw`, `dplayx`, `imm32`, `winmm`, `ws2_32`, `psapi`, `comctl32`, FreeType, libxml, libpng, libjpeg, zlib, LuaJIT, CxImage,
  CrashRpt, Mail MIME/SMTP and the KOG profiler.

## Status

Imported, not compiled yet (next step: `client/skeleton`, [roadmap](../project/roadmap.md#phase-3--client-platform)).
