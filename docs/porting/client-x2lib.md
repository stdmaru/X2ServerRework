[Home](../README.md) › [Porting](README.md) › Client: X2Lib

# Client: X2Lib (`src/client/X2Lib`)

The game layer of the client: states (`X2State*`), units and their skills (`X2GU*`, `X2GUUser`), NPCs, UI, items, quests, and
the glue to the network library (`X2Main`, `X2Data`). Uses Direct3D mostly as a math library and `HWND` through `MsgProc`
([analysis](../original/client.md)). Overview of the client port: [Client](client.md).

## Import (2026-11-05)

- Module `client-x2lib` ([report](../../tools/import/reports/client-x2lib/REPORT.md)): files of `X2Lib/` compiled by
  `X2Lib.vcxproj` (`US_INTERNAL|Win32`) and their headers: **552 files** (270 `.cpp`), 722k lines written; 27,462 `#if` groups
  resolved, 189k lines removed; 517 blocks became `X2_DEV_TOOLS`; 472 include spellings fixed. Encoding: 405 CP949, 93 ASCII,
  53 UTF-8, 1 CP949 with invalid bytes (`X2StateServerSelect.cpp`). Largest: the unit files `X2GUUser.cpp` (29k),
  `X2GuEve.cpp` (27k), `X2GUNPC.cpp` (23k), `X2GUChung.cpp` (21k), `X2GURaven.cpp` (20k).
- **Not imported**: `X2UDPPacket.*`, already in `src/server/X2Lib` (T33, [Client](client.md#code-shared-with-the-servers-t33)).
- **Excluded**: other countries (`OnlyGlobal/ID`, `OnlyGlobal/JP`, 10 references); `Debelzak/DumpData.cpp` (`_DUMP_ITEM_DATA_` is
  false); anti-cheat and code protection (`HShield.h`, `ThemidaSDK.h`, `VirtualizerSDK.h`, `X2CollectHackInfo.*`, XTRAP; the `X2HSGame.*`/`X2KNPGame.*`/`NPGameLib.h`
  wrappers were imported at first and deleted on 2026-11-05, leaving 547 files);
  publishers and ads (`X2NexonMsgHandler.*`, `X2Steam.*`, `X2IGA.*`, `Steamworks/`); SMTP mail of logs (`X2SMTPMail.*`,
  `X2MailLog.*`, `MailMIMEHeader.h`, `MailSMTPHeader.h`); Bandi screen capture (`bandicap.h`); the BugTrap header of `X2`.
- It includes 15 shared `KncWX2Server/Common` files, all present in `src/server/Common`; six of them are `.cpp` packet files
  compiled through `#include` (`ClientPacket.cpp`, `CommonPacket.cpp`, `SystemPacket.cpp` and their `OnlyGlobal/Packet/*_Global.cpp`).
- `#pragma comment(lib)` removed: BugTrap, Nexon module, Steam (`steam_api`, `sdkencryptedappticket`) and `X2ServerProtocol`.

## Hand edits (2026-11-05, `client/skeleton`)

The module is edited by hand and frozen. Steam, nProtect, Nexon, SMTP mail and BugTrap code removed from `X2Main.*`,
`X2CashShop.cpp`, `X2StateLogin.cpp`, `X2StateServerSelect.cpp` and `X2State.cpp`; `X2Main.h` includes `Enum/*.h` from
`src/server/Common` ([Client](client.md#integrations-removed-by-hand-2026-11-05)).

## Bandicam removed (2026-11-06, `client/video-browser-cximage`)

Decided with the user ([Client](client.md#bandi-embedded-browser-and-cximage-2026-11-06-clientvideo-browser-cximage)):
video recording and the Print Screen screenshot are gone.

- `X2Main.*`: `m_BandiCaptureLibrary`, `Bandi_*`, `IsCapturing`, `m_fCaptureDelay`, the disk space check
  (`m_iOldCaptureFileSize`, `m_fFileSizeCheckTime`), the Print Screen block in `OnFrameMove` and the Scroll Lock block in
  `KeyProcess`; the anti-cheat `MakeHackScreenShot*` (callers already commented out, now deleted with them); the dead
  `#ifdef BUG_TRAP` code (setup, crash callbacks, `MakeScreenShot` with CxImage).
- `X2Game.cpp`: Scroll Lock. `X2StateServerSelect.cpp`: the stop of a recording. `X2State.*`: the REC particle
  (`m_hParticleRec`, `ShowBandicamRecParticle`).
- `X2StateOption.*`: the Bandicam tab's handlers (`SOUCM_OPTION_CAM*`, `SOUCM_OPTION_VIDEO_*`) and the custom-settings
  dialog (`DLG_UI_Option_Cam_Ex.lua`, `m_pCamUserWindow`) are gone. The tab's controls are defined by the client's Lua
  dialogs, which do not change (T1): `HideBandiCamOption` hides them, the tab's check box `Option_Cam_Menu` included,
  wherever the original showed or hid them. The message IDs stay in the enum (the dialogs send them).
- Left for [Phase 5](../project/roadmap.md#phase-5--deep-cleanup): the recording settings in `CX2GameOption`
  (`m_UserSetting`, `m_VideoQuality`, FPS, sizes...), still read from and written to the option file, now unused.

## Header chain (2026-11-07, `client/x2lib-headers`)

Target `x2::client_x2lib` builds `stdafx.cpp` only: `stdafx.h` → `X2Main.h` and every header it includes compile
warning-free on Clang and GCC. It adds `src/server/X2Lib` (`X2UDPPacket.h`) to the include path. The `.cpp` files come in
the next steps ([roadmap](../project/roadmap.md#phase-4--client-graphics)).

- **Events.** The shared `Event.h` already declares the client events, so `X2ServerEvent.h`'s `SERVER_EVENT_ID` is an alias
  of `ENUM_CLIENT_EVENT_ID` and `EVENT_X2_STARTUP` names `E_CLIENT_EVENT_ID_BEGIN` (same values). `CX2ServerEvent` stays.
- **Mechanical pass** (`tools/port/mechanical.py`, `tools/port/fix_warnings.py`): Boost and Win32 includes,
  `boost::intrusive_ptr` → `x2::intrusive_ptr`, `boost::array` → `std::array`, `boost::unordered_map` →
  `std::unordered_map`, Boost.Bimap → `x2::bimap` (`X2KeyPad.h`, `X2RidingPet.h`), `boost::circular_buffer` →
  `KCircularBuffer` (`X2GUNPC`); switch defaults, unused parameters and member init order.
- **Standard C++ by hand:** `= NULL` pure specifiers → `= 0`, extra qualifications, `typename`, `FORCEINLINE`,
  `const` on by-value return types (128), `ARRAY_SIZE` loops with an `int` index → `std::ssize` (239), the item-ID tables
  in `X2Define.h` as `const DWORD`, `std::binary_function` bases, virtual destructors for the classes `X2Data.h` deletes
  (`CKTDGParticleSystem`, `CX2EventScene`, `CX2UIManager`), hidden virtuals (`using` declarations, `RayHit` and
  `CheckWindowInfo::BeginThread` with their base signatures; `CX2UIQuickSlot::SetEnable( bool )` now overrides the slot
  manager's, which nothing calls through a base pointer), `RAND_MAX` as a float, `const char*` Lua color arguments
  (`X2SlideShot`), `NULL` passed as `UidType`/`LPARAM` → `0`.
- **Removed:** the Themida and Code Virtualizer markers (third-party protection; the client's own anti-cheat stays:
  `KProtectedType`, `Verify()`, `CX2CheckSumManager`, the process-check thread, the hack reports); `X2SystemInfo.*`
  (system report, unused); `HttpSession.*` and WinINet (`g_hInternet`, `OpenURL`, `OpenGlobalURL`, `InternetOpen` in
  `X2.cpp`); `SetBetweenCharToCharSize` (HDC); the Bandicam settings of `CX2GameOption` (fields, Lua read, option file
  write; the leftover of [Bandicam removed](#bandicam-removed-2026-11-06-clientvideo-browser-cximage)); a commented-out
  macro in `X2GUUser.h`.
- **XML:** `CX2GameOption::_CalcBestGraphicOption` reads the graphics option table with `x2::platform::ParseXml`
  ([Platform](platform.md)) instead of libxml; the score helpers take an `XmlElement`. Same scores and defaults.
- **Original bugs fixed:**
  - Missing `return`s: `CX2GageData::GetShow` (now `false`), `CX2GameUnit::GetUnitSlashTraceDataSize`,
    `CX2BattleFieldManager::GetBitSetFlag`; `CX2ComboManager::SetCombo` returned nothing and is `void`.
  - `X2NumbersToLanguage.h`: `memcpy` over `std::string` arrays (each string freed twice) → `std::copy`.
  - `CX2UserSkillTree::ToggleSkillSlotAB`: `memcpy` over `KProtectedType` slots → `std::swap`.
- **Kept, recorded as a bug:** `CX2GUUser::DidReceiveFirstSyncPacket() const` did not override `CX2GameUnit`'s non-const
  one; it is now the non-virtual `DidReceiveFirstSyncPacketOfUser()`, called where the static type is `CX2GUUser*`
  ([Original bugs](../original/bugs.md)).

## `.cpp` files (2026-11-11, `client/x2lib-build`)

Target `x2::client_x2lib` compiles all 265 X2Lib `.cpp` files warning-free on Clang (dev tools ON) and GCC (dev tools
OFF). It does not link yet: `X2.cpp` and the executable are the next step
([roadmap](../project/roadmap.md#phase-4--client-graphics)).

- **Error passes.**
  - Boost: `BOOST_SCOPE_EXIT` → `x2::scope_exit`, `BOOST_FOREACH` → range-for, `iequals` → `x2::platform::IEquals`,
    `singleton_pool` → `KSingletonPool`; tokenizer, assign and `scoped_array` replaced by hand.
  - Win32 string helpers (`StrCmp`, `StrStrW`, `_itow`, `wsprintf`) → CRT and safe calls; legacy `swprintf` calls give
    their size.
  - DXUT gains the window and mouse calls (`GetCursorPos`, `ScreenToClient`, `WindowFromPoint`, ...), an SDL cursor
    from `x2.exe`'s `DefaultCursor.cur` (embedded with `x2_embed_file`), `DXUTDeviceSettings`, KOG's vertical sync,
    and the display-mode list that replaces `CD3DEnumeration`.
  - Standard C++: `goto` over initializations, temporaries bound to non-const references, out-of-line `const` return
    types. X2Lib's `NetError` copy gives way to the shared Common one (T33).
- **Anti-cheat and reports.** These run on `x2::platform` (`system_scan.h`: processes, windows, named events and
  mutexes; loaded modules; `IsDebuggerAttached`). The machine ID keeps the layout the servers check, built from
  `x2::platform::MachineIdentity`. Removed: the cases that only sent mail (SMTP is gone), the CrashRpt reporter, GDI
  font registration, BasicExcel, and the dead debug methods (`RenderStateParam` & co.).
  - `CX2DLLManager` keeps only `ForceFreeModule`. Its `LoadLibrary` hooks were 32-bit x86 jumps that the original
    never installed (the start function returned `false` first).
  - `Handler_EGS_TRAFFIC_ATTACKER_NOT` no longer kills the machine's `csrss.exe`; it only ends the client.
- **Warning passes** (`tools/port`, see [Mechanical porting](../tools/mechanical-porting.md)):
  - mechanical: switch defaults, unused parameters, member init order, unused locals, parentheses fix-its,
    sign-compare;
  - `NULL` passed as `LPARAM`/`WPARAM` → `0`, `const` dropped from by-value cast types, `[[fallthrough]]` on the
    intended fallthroughs;
  - virtual destructors (`TimeLimited`, `RoomSimpleInfo`, `CEffect`, `CX2PETAI`), about 30 private fields that were
    only initialized, `#pragma once` in `.cpp` files, trigraphs in strings and comments;
  - `[[maybe_unused]]` on locals and parameters that only `X2_DEV_TOOLS` code reads.
- **Strings.** `GET_STRING` goes through `CKTDXStringTable::GetDisplayString()`, which caches the debug text in
  `m_mapDebugInfoString`, so the pointer it returns outlives the call.
- **`KProtectedType<float>`** computes its check value with `std::bit_cast` instead of writing through an `int*` (same
  value, no aliasing UB).
- **Original bugs fixed:**
  - Missing `break`s:
    - the personal-shop "OK" fell into using the pet-pickup item;
    - the system menu with the bank open fell into the student list;
    - "cut disciple" on another tab fell into the PVP invite;
    - the personal-shop board's leg-accessory filter fell into the arm check, so it never matched.
  - Inverted or impossible conditions:
    - `CX2UIPersonalShop` refreshes the board: the original tested `NULL == g_pData && NULL == ...->Get()`, so it never
      ran;
    - `SetSkillAbilityMap` called `EndTable` on a `NULL` manager;
    - `X2SlotItem`'s gift-ribbon fallback set up the `NULL` picture instead of the new one;
    - the `UT_ARA` case of the unit-creation switch now returns `m_pDummyAra`.
  - Wrong comparisons:
    - `NPCUnitDieReq` returns `true`;
    - the riding-pet loops used `iter ==` for `iter =`;
    - the wedding motion-name check is `NULL == p || L'\0' == p[0]`;
    - the guild date uses `npos` instead of `find() >= 0`;
    - the buff `SKILL_ID` check tests the factor before the unsigned cast;
    - the dungeon skill-list highlight kept state IDs in `char`, so states above 127 never matched.
  - `NULL` guards followed by `||` are grouped under the guard (cash shop, dungeon game, party UI, quest UI, skill tree).
  - Uninitialized reads now have a value or are scoped to where they are set:
    - the chain-gate light rotation;
    - the dungeon map's open position;
    - notice statics and guide arrows;
    - wedding notice colors (white with a black outline when no hall is set);
    - a new dungeon clear entry;
    - the number-to-words table (an unknown language returns);
    - the line-map projection point;
    - `bUserUnit`, `pInformer` and `iFinalLeft`;
    - `m_pKeyPad` (its `REFORM_UI_KEYPAD` guard is gone).
  - Out of bounds:
    - `SkillCutIn` built a `D3DXVECTOR3` from a `D3DXVECTOR2` and read past it (`z` is now 0);
    - the SHA1 report wrote `" 255"` into `szTemp[4]`;
    - `CX2ComboManager` cleared half of `m_ComboContent`, and `AbsorbEffectAttackProcess` half of its `wstr`.
  - `ZeroMemory` over non-trivial types replaced by assignment (cash-shop dialog handles, the reserved resurrect packet).
  - `DXUTGetVerticalSync` returns `UINT`; SHA1 file hashing fails on a short read.
- **Windows (clang-cl).** `Types.h`'s endianness detection knew no 64-bit target. It now defines `LITTLE_ENDIAN` and
  asserts `std::endian::little`; on Linux it had only passed because glibc's `<endian.h>` defines both macros. The channel
  host resolves through `x2::platform::ResolveIPv4` (in place of `gethostbyname`/`inet_ntoa`), and `stricmp` is now
  `_stricmp`. `ASSERT` is a real assert there, so its conditions must be warning-free too. Linux debug only takes the
  `sizeof` of them (2026-11-11).
- **Removed as dead:** a copy of the premium-buff event dispatch after a `break` in `CX2State` (the reachable one runs
  first); the commented-out P2P connect-result packets in `CX2GUUser::OnFrameMove`.
- **Kept, recorded as bugs** ([Original bugs](../original/bugs.md)): quest reward auto-selection compares the item's
  class with itself; Ara's energy reinforce tests `AT_NORMAL` on a damage type; the shop's accessory warning tests
  `SPT_GP` on a period type; the result screens' `=-` timer; the field-entry period notices that never show; Arme
  falling into Lire's body reform.

## Game options file (2026-11-12, `client/x2-run`)

- `CX2GameOption::SaveScriptFile` wrote a UTF-8 BOM with `fputc` and the rest with `fwprintf`; glibc fixes a stream's
  orientation at its first write, so every `fwprintf` failed and `GameOptions.lua` held only the BOM. Each line is now
  formatted with `vswprintf` and written as UTF-8 bytes (`FilePrintf`); the file's contents are the original's.
- **Behavior change:** with no `GameOptions.lua`, the original asked whether to choose the graphics options (and ran the
  benchmark on Yes) and saved the file only after a login or an option change, so the question came back at every start
  until then. The port saves right after the answer.
- `FRAME_LIMIT = <fps>` (new, 2026-11-12, [T41](../project/decisions.md)): at most that many frames per second, 0 for
  no limit; read, saved and applied with the other options (`CX2GameOption::SetFrameLimit`).
- `MULTISAMPLE = 1|2|4|8` and `ANISOTROPY = 1|2|4|8|16` (new, 2026-11-12, `client/msaa`): anti-aliasing and anisotropic
  filtering, applied by the [D3D9 device](client-d3d9.md#anti-aliasing-and-anisotropic-filtering-2026-11-12-clientmsaa)
  from the next frame (`CX2GameOption::SetMultisample`, `SetAnisotropy`).
- `SUPERSAMPLE = 1|2` (new, 2026-11-12, `client/msaa`): supersampling, applied the same way (`SetSupersample`).
- **Graphics tab** (2026-11-12, `client/msaa`, content changes in [client_data](../tools/client-data.md)): combo boxes
  for anti-aliasing, anisotropic filtering, the frame limit and supersampling, and the vertical sync check box, moved from the other
  options' tab. Their labels are in `DLG_UI_Option_Graphic.lua`; `X2StateOption.cpp` maps each item index to its
  value (`kMultisampleItems`, `kAnisotropyItems`, `kSupersampleItems`, `kFrameLimitItems`: 0, 30, 60, 120, 144, 165 and 240 fps). A value
  edited in `GameOptions.lua` that no item has leaves its combo box empty. New messages
  `SOUCM_OPTION_GRAPHIC_MULTISAMPLE`/`ANISOTROPY`/`FRAME_LIMIT`/`SUPERSAMPLE` (2520–2523), also in `Enum.lua`.

## Widescreen canvas (2026-11-13, `client/widescreen-scale`, [T44](../project/decisions.md))

This is the X2Lib side of [the uniform scale](client-ktdxlib.md#widescreen-canvas-2026-11-13-clientwidescreen-scale).
Each use of the old X/Y scale is now a position, a size or a canvas edge:

- Tooltips (item slots, quests, party, enchant, skill trees, guild skills, world map): widths in layout units
  (`kUILayoutWidth / 3`, `/ 4`), kept on screen by the canvas edges. Compare tooltips can therefore extend into the area
  beside the column.
- Talk boxes, gauges (hit boxes, air gauge, hyper balls), pets, quick-slot effects, room clicks: positions and sizes
  sorted. Everything named `*Size*` now goes through the size conversion.
- `CX2Main::GetWindowPos`, the user list popup and the notice marquee are bound by the canvas. The marquee spans the screen
  and keeps the original's speed.
- `WordWrap` measures with 1 / scale.
- **Behavior change:** the rank and server icons over units (`X2GUUser`, `X2SquareUnit`, `X2GUNPC`) had a size scaled by X
  and Y separately, so they were not square at 16:9. Their offsets and size are now in layout units.
- **Behavior change:** the monster indicator's box (where off-screen monsters are pointed at) mixed layout units with the
  particles' screen pixels. It is now in screen pixels, from the viewport's size.
- **Behavior change, default resolution:** with no `GameOptions.lua`, or without its resolution keys, the client takes the
  largest of 3840×2160, 2560×1440, 1920×1080, 1600×900 and 1280×720 that fits the desktop (`DefaultResolution` in
  `X2GameOption.cpp`). The original used 1024×768. A failed mode change falls back to 1280×720, as does the low-spec
  preset (was 800×600), and the device opens at 1280×720 (`X2.cpp`). 4:3 sizes remain in the options list.

Known, left for the migration: screens whose texture does not match its rectangle's aspect stay as the original drew
them. The character select background, for example, is a 512×512 texture drawn at 1024×768, so it is stretched 33% in
the original as well.

## Status

All 265 `.cpp` files compile warning-free (2026-11-11), and the client links on them
([Client: X2](client-x2.md#x2cpp-2026-11-12-clientx2-link), 2026-11-12).

Notes for linking (2026-11-07):

- `CSystemInfo` (the machine ID) is still used by `X2Main.cpp`: the login packet needs it.
- The `switch( GetPublisher() )` branches for other countries are dead.
